/*
 * wl offloads command module
 *
 * Copyright 2017 Broadcom
 *
 * This program is the proprietary software of Broadcom and/or
 * its licensors, and may only be used, duplicated, modified or distributed
 * pursuant to the terms and conditions of a separate, written license
 * agreement executed between you and Broadcom (an "Authorized License").
 * Except as set forth in an Authorized License, Broadcom grants no license
 * (express or implied), right to use, or waiver of any kind with respect to
 * the Software, and Broadcom expressly reserves all rights in and to the
 * Software and all intellectual property rights therein.  IF YOU HAVE NO
 * AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY
 * WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF
 * THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1. This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof, and to
 * use this information only in connection with your use of Broadcom
 * integrated circuit products.
 *
 * 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
 * "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR
 * OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL,
 * SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR
 * IN ANY WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 * IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii)
 * ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF
 * OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY
 * NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
 *
 *
 * <<Broadcom-WL-IPTag/Proprietary:>>
 *
 * $Id: wluc_offloads.c 458728 2014-02-27 18:15:25Z $
 */

#ifdef WIN32
#include <windows.h>
#endif // endif

#include <wlioctl.h>

#if	defined(DONGLEBUILD)
#include <typedefs.h>
#include <osl.h>
#endif // endif

/* Because IL_BIGENDIAN was removed there are few warnings that need
 * to be fixed. Windows was not compiled earlier with IL_BIGENDIAN.
 * Hence these warnings were not seen earlier.
 * For now ignore the following warnings
 */
#ifdef WIN32
#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4761)
#endif // endif

#include <bcmutils.h>
#include <bcmendian.h>
#include "wlu_common.h"
#include "wlu.h"

static cmd_func_t wlu_offloads_stats;
static cmd_func_t wl_ol_notify_bcn_ie;

static cmd_t wl_offloads_cmds[] = {
	{ "ol_stats", wlu_offloads_stats, WLC_GET_VAR, -1,
	"Give suboption \"list\" to list various suboptions" },
	{ "ol_eventlog", wlu_offloads_stats, WLC_GET_VAR, -1,
	"Give suboption \"list\" to list various suboptions" },
	{ "ol_cons", wlu_offloads_stats, WLC_GET_VAR, WLC_SET_VAR,
	"Display the ARM console or issue a command to the ARM console\n"
	"  Usage: ol_cons [<cmd>]\n"
	"\t\"?\" - Display the list of active console commands"
	},
	{ "ol_wowl_cons", wlu_offloads_stats, WLC_GET_VAR, -1,
	"Give suboption \"list\" to list various suboptions" },
	{ "ol_clr", wlu_offloads_stats, WLC_GET_VAR, -1,
	"Give suboption \"list\" to list various suboptions" },
	{ "ol_notify_bcn_ie", wl_ol_notify_bcn_ie, WLC_GET_VAR, WLC_SET_VAR,
	"Enable/Disable IE ID notification"},
	{ "ol_arp_hostip", wl_hostip, WLC_GET_VAR, WLC_SET_VAR,
	"Add a host-ip address or display them"},
	{ "ol_nd_hostip", wl_hostipv6, WLC_GET_VAR, WLC_SET_VAR,
	"Add a local host-ipv6 address or display them"},
	{ NULL, NULL, 0, 0, NULL }
};

static char *buf;

/* module initialization */
void
wluc_offloads_module_init(void)
{
	(void)g_swap;

	/* get the global buf */
	buf = wl_get_buf();

	/* register offloads commands */
	wl_module_cmds_register(wl_offloads_cmds);
}

static int
wlu_offloads_stats(void *wl, cmd_t *cmd, char **argv)
{
	int ret;
	char *dump_buf;
	int bufsz = WL_DUMP_BUF_LEN;
	bool cons_cmd = FALSE;

	if (cmd->get < 0)
		return -1;

	if (!strcmp(cmd->name, "ol_clr"))
	{
		ret = wlu_iovar_get(wl, cmd->name, NULL, 0);
		return ret;
	}

	if (!strcmp(cmd->name, "ol_cons")) {
		/* Check for command */
		if (*(argv + 1)) {
			argv++;
			cons_cmd = TRUE;
			bufsz = CMDLINESZ;
		}
	}

	dump_buf = malloc(bufsz);
	if (dump_buf == NULL) {
		fprintf(stderr, "Failed to allocate dump buffer of %d bytes\n", bufsz);
		return -1;
	}
	memset(dump_buf, 0, bufsz);

	while (*argv) {
		/* add space delimiter if this is not the first section name */
		if (dump_buf[0] != '\0')
			strcat(dump_buf, " ");

		strcat(dump_buf, *argv);

		argv++;
	}

	if (cons_cmd) {
		ret = wlu_iovar_set(wl, cmd->name, dump_buf, bufsz);
	} else {
		ret = wlu_iovar_get(wl, cmd->name, dump_buf, bufsz);
		if (!ret)
			fputs(dump_buf, stdout);
	}

	free(dump_buf);

	return ret;
}

static int
wl_ol_notify_bcn_ie(void *wl, cmd_t *cmd, char **argv)
{
	int ret = -1;
	void *ptr = NULL;

#define	VNDR_IE_ID		(221)
#define OLMSG_BCN_MAX_IE	(222)

	struct beacon_ie_notify_cmd {
		uint32		id;
		uint32		enable;
		struct ipv4_addr vndriemask;
	} beacon_notification_cmd;

	memset(&beacon_notification_cmd, 0, sizeof(beacon_notification_cmd));

	++argv; /* skip the command iteself */

	if (!*argv) {
		/* Get Everything */
		beacon_notification_cmd.id = -1; /* -1 indicates NONE */
		ret = wlu_var_getbuf(wl, cmd->name, &beacon_notification_cmd,
			sizeof(beacon_notification_cmd), &ptr);
	} else {
		if (stricmp(*argv, "enable") == 0) { /* Enable Global flag */
			beacon_notification_cmd.id = -1; /* -1 indicates NONE */
#ifdef DSLCPE_ENDIAN
			beacon_notification_cmd.enable = htod32(1);
#else
			beacon_notification_cmd.enable = 1;
#endif
			return wlu_var_setbuf(wl, cmd->name, &beacon_notification_cmd,
				sizeof(beacon_notification_cmd));
		} else if (stricmp(*argv, "disable") == 0) { /* Disable Global flag */
			beacon_notification_cmd.id = -1; /* -1 indicates NONE */

			beacon_notification_cmd.enable = 0;
			return wlu_var_setbuf(wl, cmd->name, &beacon_notification_cmd,
				sizeof(beacon_notification_cmd));
		} else { /* Get/Set Enable/Disable some ID */

		    beacon_notification_cmd.id = (uint8) atoi(*argv);

		    if (beacon_notification_cmd.id == VNDR_IE_ID) {
#ifdef DSLCPE_ENDIAN
			beacon_notification_cmd.id = htod32(beacon_notification_cmd.id);
#endif
			/* Get VNDR OUI */
			++argv;
			if (*argv) { /* Get a specific Vendor IE */
				if (!wl_atoip(*argv, &beacon_notification_cmd.vndriemask)) {
					return -1;
				}
				++argv;
				/* Get enable/disable flag */
				if (*argv) { /* Set a specific Vendor ie */
					if (stricmp(*argv, "enable") == 0)
#ifdef DSLCPE_ENDIAN
        					beacon_notification_cmd.enable = htod32(1);
#else
						beacon_notification_cmd.enable = 1;
#endif
					else if (stricmp(*argv, "disable") == 0)
						beacon_notification_cmd.enable = 0;
					else
						return -1;

					return wlu_var_setbuf(wl, cmd->name,
						&beacon_notification_cmd,
						sizeof(beacon_notification_cmd));
				} else {
					ret = wlu_var_getbuf(wl, cmd->name,
						&beacon_notification_cmd,
						sizeof(beacon_notification_cmd),
						&ptr);
				}
			} else { /* Get ALL Vendor IE */
				ret = wlu_var_getbuf(wl, cmd->name,
					&beacon_notification_cmd,
					sizeof(beacon_notification_cmd),
					&ptr);
			}
		    } else {
			if (beacon_notification_cmd.id > OLMSG_BCN_MAX_IE) {
				return -1;
			}
#ifdef DSLCPE_ENDIAN
			beacon_notification_cmd.id = htod32(beacon_notification_cmd.id);
#endif
			/* Get enable/disable flag */
			++argv;
			if(*argv) { /* Set IE ID */
				if (stricmp(*argv, "enable") == 0)
#ifdef DSLCPE_ENDIAN
				        beacon_notification_cmd.enable = htod32(1);
#else
					beacon_notification_cmd.enable = 1;
#endif
				else if (stricmp(*argv, "disable") == 0)
					beacon_notification_cmd.enable = 0;
				else
					return -1;
				return wlu_var_setbuf(wl, cmd->name,
					&beacon_notification_cmd,
					sizeof(beacon_notification_cmd));
			} else { /* Get IE ID */
				ret = wlu_var_getbuf(wl, cmd->name,
					&beacon_notification_cmd,
					sizeof(beacon_notification_cmd),
					&ptr);
			}
		    }
		}
	}
	if ((ret >= 0) && (ptr != NULL)) {
		printf("%s\n", (char *)ptr); /* Print Everything */
	}
	return ret;
}

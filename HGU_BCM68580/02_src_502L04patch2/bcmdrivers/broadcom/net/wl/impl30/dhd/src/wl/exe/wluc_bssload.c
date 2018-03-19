/*
 * wl bssload command module
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
 * $Id: wluc_bssload.c 458728 2014-02-27 18:15:25Z $
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

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <linux/if_packet.h>

static int wl_bssload_static(void *wl, cmd_t *cmd, char **argv);
#if defined(WLBSSLOAD_REPORT)
static int wl_bssload_report(void *wl, cmd_t *cmd, char **argv);
static int wl_bssload_report_event(void *wl, cmd_t *cmd, char **argv);
#if defined(linux)
static cmd_func_t wl_bssload_event_check;
#endif   /* linux */
#endif   /* WLBSSLOAD_REPORT */

static cmd_t wl_bssload_cmds[] = {
	{ "bssload_static", wl_bssload_static, WLC_GET_VAR, WLC_SET_VAR,
	"get or set static BSS load\n"
	"\tusage: wl bssload_static [off | <sta_count> <chan_util> <acc>]"},
#if defined(WLBSSLOAD_REPORT)
	{ "bssload_report", wl_bssload_report, WLC_GET_VAR, -1,
	"Get the latest BSS Load IE data from the associated AP's beacon\n"
	"\tUsage: bssload_report"
	},
	{ "bssload_report_event", wl_bssload_report_event, WLC_GET_VAR, WLC_SET_VAR,
	"Get/Set BSS load threshold for sending WLC_E_BSS_LOAD event\n"
	"\tUsage: wl bssload_report_event [rate_limit_msec] [level] [level] ...\n"
	"\t\t[level] is a 0...255 channel utilization value.\n"
	"\t\tUp to 8 levels in increasing order may be specified."
	},
#if defined(linux)
	{ "bssload_event_check", wl_bssload_event_check, -1, -1,
	"Listens forever for BSS Load events and prints them.\n"
	"\tUsage: wl bssload_event_check"
	},
#endif	/* linux */
#endif   /* WLBSSLOAD_REPORT */
	{ NULL, NULL, 0, 0, NULL }
};

static char *buf;

/* module initialization */
void
wluc_bssload_module_init(void)
{
	/* get the global buf */
	buf = wl_get_buf();

	/* register bssload commands */
	wl_module_cmds_register(wl_bssload_cmds);
}

static int
wl_bssload_static(void *wl, cmd_t *cmd, char **argv)
{
	int err = -1;
	wl_bssload_static_t bssload;

	UNUSED_PARAMETER(cmd);

	/* get */
	if (!argv[1]) {
		if ((err = wlu_iovar_get(wl, cmd->name, &bssload, sizeof(bssload))) < 0)
			return err;
		if (bssload.is_static) {
			printf("station count: %d\n", dtoh16(bssload.sta_count));
			printf("channel utilization: %d\n", bssload.chan_util);
			printf("avail admission capacity: %d\n", dtoh16(bssload.aac));
		}
	}
	else {
		/* set */
		argv++;
		memset(&bssload, 0, sizeof(bssload));
		if (!stricmp(*argv, "off")) {
			bssload.is_static = FALSE;
		}
		else {
			bssload.sta_count = htod16(strtoul(*argv, NULL, 0));

			if (*++argv == NULL) {
				printf("wl_bssload_static: "
					"channel utilization not provided\n");
				return -1;
			}
			bssload.chan_util = strtoul(*argv, NULL, 0);

			if (*++argv == NULL) {
				printf("wl_bssload_static: "
					"avail admission capacity not provided\n");
				return -1;
			}
			bssload.aac = htod16(strtoul(*argv, NULL, 0));

			bssload.is_static = TRUE;
		}

		err = wlu_iovar_set(wl, cmd->name, &bssload, sizeof(bssload));
	}

	return err;
}

#if defined(WLBSSLOAD_REPORT)
static int
wl_bssload_report(void *wl, cmd_t *cmd, char **argv)
{
	int ret = 0;
	wl_bssload_t *bssload = NULL;

	/* If any arguments are given, print help */
	if (ARGCNT(argv) > 1)
		goto usage;

	/* Get and print the current BSS Load values */
	if ((ret = wlu_var_getbuf_sm(wl, cmd->name, NULL, 0, (void*)&bssload)))
		return ret;
	if (bssload == NULL)
		return BCME_ERROR;
	printf("BSS Load from associated AP beacon:\n");
	printf("station count               : %u\n", dtoh16(bssload->sta_count));
	printf("channel utilization         : %u\n", bssload->chan_util);
	printf("available admission capacity: %u\n", dtoh16(bssload->aac));
	return 0;

usage:
	return -1;
}

static int
wl_bssload_report_event(void *wl, cmd_t *cmd, char **argv)
{
	int ret;
	wl_bssload_cfg_t blcfg;

	(void) wl;
	if (!*++argv) {
		/* get */
		void *ptr = NULL;
		uint i;

		if ((ret = wlu_var_getbuf(wl, cmd->name, NULL, 0, &ptr)) < 0)
			return ret;

		memcpy(&blcfg, ptr, sizeof(blcfg));
		blcfg.rate_limit_msec = dtoh32(blcfg.rate_limit_msec);

		printf("rate_limit_msec: %d\n", blcfg.rate_limit_msec);
		printf("%d channel utilization levels:", blcfg.num_util_levels);
		for (i = 0; i < blcfg.num_util_levels; i++) {
			printf(" %d", blcfg.util_levels[i]);
		}
		printf("\n");
	} else {
		/* set */
		memset(&blcfg, 0, sizeof(wl_bssload_cfg_t));
		blcfg.rate_limit_msec = atoi(*argv);

		while (*++argv && blcfg.num_util_levels < MAX_BSSLOAD_LEVELS) {
			blcfg.util_levels[blcfg.num_util_levels++] = atoi(*argv);
			if (blcfg.num_util_levels > 1 &&
				blcfg.util_levels[blcfg.num_util_levels - 1] <=
				blcfg.util_levels[blcfg.num_util_levels - 2]) {
				printf("Channel utilization level %u was <= level %u.\n",
					blcfg.util_levels[blcfg.num_util_levels - 1],
					blcfg.util_levels[blcfg.num_util_levels - 2]);
				goto usage;
			}
		}

		if (*argv) {
			printf("Too many parameters.\n");
			goto usage;
		}

		blcfg.rate_limit_msec = htod32(blcfg.rate_limit_msec);
		ret = wlu_var_setbuf(wl, cmd->name, &blcfg, sizeof(blcfg));
	}
	return ret;

usage:
	return -1;
}

#if defined(linux)
static int
wl_wait_for_event(void *wl, char **argv, uint event_id, uint evbuf_size,
	void (*event_cb_fn)(int event_type, bcm_event_t *bcm_event))
{
	int err = BCME_OK;
	int fd, octets;
	struct sockaddr_ll sll;
	struct ifreq ifr;
	char ifnames[IFNAMSIZ] = {"eth1"};
	char* data;
	uint8 event_inds_mask[WL_EVENTING_MASK_LEN];

	/* Override default ifname explicitly or implicitly */
	if (*++argv) {
		if (strlen(*argv) >= IFNAMSIZ) {
			printf("Interface name %s too long\n", *argv);
			return -1;
		}
		strncpy(ifnames, *argv, IFNAMSIZ);
	} else {
		strncpy(ifnames, ((struct ifreq *)wl)->ifr_name, (IFNAMSIZ - 1));
	}
	ifnames[IFNAMSIZ - 1] = '\0';

	/* Set the requested WLC_E_* event bit in the event mask */
	if (!(err = wlu_iovar_get(wl, "event_msgs", &event_inds_mask, WL_EVENTING_MASK_LEN))) {
		event_inds_mask[event_id / 8] |= 1 << (event_id % 8);
		err = wlu_iovar_set(wl, "event_msgs", &event_inds_mask, WL_EVENTING_MASK_LEN);
	}
	if (err < 0) {
		printf("Error setting event_msgs bit %u\n", err);
		goto exit2;
	}

	/* Open a socket to read driver WLC_E_* events */
	memset(&ifr, 0, sizeof(ifr));
	if (wl)
		strncpy(ifr.ifr_name, ((struct ifreq *)wl)->ifr_name, (IFNAMSIZ - 1));
	else
		strncpy(ifr.ifr_name, ifnames, (IFNAMSIZ - 1));

	fd = socket(PF_PACKET, SOCK_RAW, hton16(ETHER_TYPE_BRCM));
	if (fd < 0) {
		printf("Cannot create socket %d\n", fd);
		err = -1;
		goto exit2;
	}

	err = ioctl(fd, SIOCGIFINDEX, &ifr);
	if (err < 0) {
		printf("%s: Cannot get index %d\n", __FUNCTION__, err);
		goto exit1;
	}

	/* bind the socket first before starting so we won't miss any event */
	memset(&sll, 0, sizeof(sll));
	sll.sll_family = AF_PACKET;
	sll.sll_protocol = hton16(ETHER_TYPE_BRCM);
	sll.sll_ifindex = ifr.ifr_ifindex;
	err = bind(fd, (struct sockaddr *)&sll, sizeof(sll));
	if (err < 0) {
		printf("Cannot bind %d\n", err);
		goto exit1;
	}

	data = (char*)malloc(evbuf_size);

	if (data == NULL) {
		printf("Cannot not allocate %u bytes for events receive buffer\n",
			evbuf_size);
		err = -1;
		goto exit1;
	}

	/* Loop forever to receive driver events */
	while (1) {
		bcm_event_t *bcm_event;
		int event_type;

		octets = recv(fd, data, evbuf_size, 0);
		bcm_event = (bcm_event_t *)data;
		event_type = ntoh32(bcm_event->event.event_type);
		if (octets >= (int)sizeof(bcm_event_t)) {
			event_cb_fn(event_type, bcm_event);
		}
	}

	free(data);
exit1:
	close(fd);
exit2:
	return err;
}

static void
wl_bssload_event_cb(int event_type, bcm_event_t *bcm_event)
{
	struct timeval tv;
	uint32 sec, msec;
	int ret;
	wl_bssload_t *data = (wl_bssload_t *) (bcm_event + 1);

	if (event_type == WLC_E_BSS_LOAD) {
		/* Print a timestamp */
		ret = gettimeofday(&tv, NULL);
		if (ret == 0) {
			sec = (uint32) (tv.tv_sec % 10000);
			msec = (uint32) tv.tv_usec / 1000;
			printf("%04d.%03d\n", sec, msec);
		}

		printf("WLC_E_BSS_LOAD: chan_util = %u\n", data->chan_util);
		printf("                sta_count = %u\n", dtoh16(data->sta_count));
		printf("                aac       = %u\n", dtoh16(data->aac));
	}
}

static int
wl_bssload_event_check(void *wl, cmd_t *cmd, char **argv)
{
	if (argv[1] && argv[1][0] == '-') {
		wl_cmd_usage(stderr, cmd);
		return -1;
	}
	return wl_wait_for_event(wl, argv, WLC_E_BSS_LOAD, 2048, wl_bssload_event_cb);
}
#endif	/* linux */
#endif   /* WLBSSLOAD_REPORT */

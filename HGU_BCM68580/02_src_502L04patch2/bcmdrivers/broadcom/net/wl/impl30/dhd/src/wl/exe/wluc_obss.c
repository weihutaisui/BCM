/*
 * wl obss command module
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
 * $Id: wluc_obss.c 458728 2014-02-27 18:15:25Z $
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

static cmd_func_t wl_obss_scan, wl_obss_coex_action;

static cmd_t wl_obss_cmds[] = {
	{ "obss_scan_params", wl_obss_scan, WLC_GET_VAR, WLC_SET_VAR,
	"set/get Overlapping BSS scan parameters\n"
	"Usage: wl obss_scan a b c d e ...; where\n"
	"\ta-Passive Dwell, {5-1000TU}, default = 100\n"
	"\tb-Active Dwell, {10-1000TU}, default = 20\n"
	"\tc-Width Trigger Scan Interval, {10-900sec}, default = 300\n"
	"\td-Passive Total per Channel, {200-10000TU}, default = 200\n"
	"\te-Active Total per Channel, {20-1000TU}, default = 20\n"
	"\tf-Channel Transition Delay Factor, {5-100}, default = 5\n"
	"\tg-Activity Threshold, {0-100%}, default = 25"},
	{ "obss_coex_action", wl_obss_coex_action, -1, WLC_SET_VAR,
	"send OBSS 20/40 Coexistence Mangement Action Frame\n"
	"\tUsage: wl obss_coex_action -i <1/0> -w <1/0> -c <channel list>\n"
	"\t -i: 40MHz intolerate bit; -w: 20MHz width Req bit;\n"
	"\t -c: channel list, 1 - 14\n"
	"\t At least one option must be provided"
	},
	{ NULL, NULL, 0, 0, NULL }
};

static char *buf;

/* module initialization */
void
wluc_obss_module_init(void)
{
	/* get the global buf */
	buf = wl_get_buf();

	/* register obss commands */
	wl_module_cmds_register(wl_obss_cmds);
}

static int
wl_obss_scan_params_range_chk(wl_obss_scan_arg_t *obss_scan_arg)
{
	if (obss_scan_arg->passive_dwell < 0)
		obss_scan_arg->passive_dwell = WLC_OBSS_SCAN_PASSIVE_DWELL_DEFAULT;
	else if (obss_scan_arg->passive_dwell < WLC_OBSS_SCAN_PASSIVE_DWELL_MIN ||
		obss_scan_arg->passive_dwell > WLC_OBSS_SCAN_PASSIVE_DWELL_MAX) {
		printf("passive dwell not in range %d\n", obss_scan_arg->passive_dwell);
		return -1;
	}

	if (obss_scan_arg->active_dwell < 0)
		obss_scan_arg->active_dwell = WLC_OBSS_SCAN_ACTIVE_DWELL_DEFAULT;
	else if (obss_scan_arg->active_dwell < WLC_OBSS_SCAN_ACTIVE_DWELL_MIN ||
		obss_scan_arg->active_dwell > WLC_OBSS_SCAN_ACTIVE_DWELL_MAX) {
		printf("active dwell not in range %d\n", obss_scan_arg->active_dwell);
		return -1;
	}

	if (obss_scan_arg->bss_widthscan_interval < 0)
		obss_scan_arg->bss_widthscan_interval =
			WLC_OBSS_SCAN_WIDTHSCAN_INTERVAL_DEFAULT;
	else if (obss_scan_arg->bss_widthscan_interval < WLC_OBSS_SCAN_WIDTHSCAN_INTERVAL_MIN ||
		obss_scan_arg->bss_widthscan_interval > WLC_OBSS_SCAN_WIDTHSCAN_INTERVAL_MAX) {
		printf("Width Trigger Scan Interval not in range %d\n",
		       obss_scan_arg->bss_widthscan_interval);
		return -1;
	}

	if (obss_scan_arg->chanwidth_transition_delay < 0)
		obss_scan_arg->chanwidth_transition_delay =
			WLC_OBSS_SCAN_CHANWIDTH_TRANSITION_DLY_DEFAULT;
	else if ((obss_scan_arg->chanwidth_transition_delay <
		WLC_OBSS_SCAN_CHANWIDTH_TRANSITION_DLY_MIN) ||
		(obss_scan_arg->chanwidth_transition_delay >
		WLC_OBSS_SCAN_CHANWIDTH_TRANSITION_DLY_MAX)) {
		printf("Width Channel Transition Delay Factor not in range %d\n",
		       obss_scan_arg->chanwidth_transition_delay);
		return -1;
	}

	if (obss_scan_arg->passive_total < 0)
		obss_scan_arg->passive_total = WLC_OBSS_SCAN_PASSIVE_TOTAL_PER_CHANNEL_DEFAULT;
	else if (obss_scan_arg->passive_total < WLC_OBSS_SCAN_PASSIVE_TOTAL_PER_CHANNEL_MIN ||
		obss_scan_arg->passive_total > WLC_OBSS_SCAN_PASSIVE_TOTAL_PER_CHANNEL_MAX) {
		printf("Passive Total per Channel not in range %d\n", obss_scan_arg->passive_total);
		return -1;
	}

	if (obss_scan_arg->active_total < 0)
		obss_scan_arg->active_total = WLC_OBSS_SCAN_ACTIVE_TOTAL_PER_CHANNEL_DEFAULT;
	if (obss_scan_arg->active_total < WLC_OBSS_SCAN_ACTIVE_TOTAL_PER_CHANNEL_MIN ||
		obss_scan_arg->active_total > WLC_OBSS_SCAN_ACTIVE_TOTAL_PER_CHANNEL_MAX) {
		printf("Active Total per Channel not in range %d\n", obss_scan_arg->active_total);
		return -1;
	}

	if (obss_scan_arg->activity_threshold < 0)
		obss_scan_arg->activity_threshold = WLC_OBSS_SCAN_ACTIVITY_THRESHOLD_DEFAULT;
	else if (obss_scan_arg->activity_threshold < WLC_OBSS_SCAN_ACTIVITY_THRESHOLD_MIN ||
		obss_scan_arg->activity_threshold > WLC_OBSS_SCAN_ACTIVITY_THRESHOLD_MAX) {
		printf("Activity Threshold not in range %d\n", obss_scan_arg->activity_threshold);
		return -1;
	}
	return 0;
}

static int
wl_obss_scan(void *wl, cmd_t *cmd, char **argv)
{
	int err = -1;
	wl_obss_scan_arg_t obss_scan_arg;
	char *endptr = NULL;
	uint buflen;
	uint argc = 0;

	if (!*++argv) {
		void *ptr = NULL;
		wl_obss_scan_arg_t *obss_scan_param;

		err = wlu_var_getbuf(wl, cmd->name, NULL, 0, &ptr);
		if (err < 0)
		        return err;

		obss_scan_param = (wl_obss_scan_arg_t *)ptr;
		printf("%d %d %d %d %d %d %d\n",
		       dtoh16(obss_scan_param->passive_dwell),
		       dtoh16(obss_scan_param->active_dwell),
		       dtoh16(obss_scan_param->bss_widthscan_interval),
		       dtoh16(obss_scan_param->passive_total),
		       dtoh16(obss_scan_param->active_total),
		       dtoh16(obss_scan_param->chanwidth_transition_delay),
		       dtoh16(obss_scan_param->activity_threshold));
		return 0;
	}

	/* arg count */
	while (argv[argc])
		argc++;

	buflen = WL_OBSS_SCAN_PARAM_LEN;
	memset((uint8 *)&obss_scan_arg, 0, buflen);

	/* required argments */
	if (argc < WL_MIN_NUM_OBSS_SCAN_ARG) {
		fprintf(stderr, "Too few/many arguments (require %d, got %d)\n",
			WL_MIN_NUM_OBSS_SCAN_ARG, argc);
		return BCME_USAGE_ERROR;
	}

#ifdef DSLCPE_ENDIAN
	obss_scan_arg.passive_dwell = (int16)strtol(*argv++, &endptr, 0);
	if (*endptr != '\0')
		return BCME_USAGE_ERROR;
	obss_scan_arg.active_dwell = (int16)strtol(*argv++, &endptr, 0);
	if (*endptr != '\0')
		return BCME_USAGE_ERROR;
	obss_scan_arg.bss_widthscan_interval = (int16)strtol(*argv++, &endptr, 0);
	if (*endptr != '\0')
		return BCME_USAGE_ERROR;
	obss_scan_arg.passive_total = (int16)strtol(*argv++, &endptr, 0);
	if (*endptr != '\0')
		return BCME_USAGE_ERROR;
	obss_scan_arg.active_total = (int16)strtol(*argv++, &endptr, 0);
	if (*endptr != '\0')
		return BCME_USAGE_ERROR;
	obss_scan_arg.chanwidth_transition_delay = (int16)strtol(*argv++, &endptr, 0);
	if (*endptr != '\0')
		return BCME_USAGE_ERROR;
	obss_scan_arg.activity_threshold = (int16)strtol(*argv++, &endptr, 0);
#else
	obss_scan_arg.passive_dwell = htod16((int16)strtol(*argv++, &endptr, 0));
	if (*endptr != '\0')
		return BCME_USAGE_ERROR;
	obss_scan_arg.active_dwell = htod16((int16)strtol(*argv++, &endptr, 0));
	if (*endptr != '\0')
		return BCME_USAGE_ERROR;
	obss_scan_arg.bss_widthscan_interval = htod16((int16)strtol(*argv++, &endptr, 0));
	if (*endptr != '\0')
		return BCME_USAGE_ERROR;
	obss_scan_arg.passive_total = htod16((int16)strtol(*argv++, &endptr, 0));
	if (*endptr != '\0')
		return BCME_USAGE_ERROR;
	obss_scan_arg.active_total = htod16((int16)strtol(*argv++, &endptr, 0));
	if (*endptr != '\0')
		return BCME_USAGE_ERROR;
	obss_scan_arg.chanwidth_transition_delay = htod16((int16)strtol(*argv++, &endptr, 0));
	if (*endptr != '\0')
		return BCME_USAGE_ERROR;
	obss_scan_arg.activity_threshold = htod16((int16)strtol(*argv++, &endptr, 0));
#endif
	if (*endptr != '\0')
		return BCME_USAGE_ERROR;
	if (wl_obss_scan_params_range_chk(&obss_scan_arg))
		return BCME_RANGE;

#ifdef DSLCPE_ENDIAN
	obss_scan_arg.passive_dwell = htod16(obss_scan_arg.passive_dwell);
	obss_scan_arg.active_dwell = htod16(obss_scan_arg.active_dwell);
	obss_scan_arg.bss_widthscan_interval = htod16(obss_scan_arg.bss_widthscan_interval);
	obss_scan_arg.passive_total = htod16(obss_scan_arg.passive_total);
	obss_scan_arg.active_total = htod16(obss_scan_arg.active_total);
	obss_scan_arg.chanwidth_transition_delay = htod16(obss_scan_arg.chanwidth_transition_delay);
	obss_scan_arg.activity_threshold = htod16(obss_scan_arg.activity_threshold);
#endif
	err = wlu_var_setbuf(wl, cmd->name, &obss_scan_arg, buflen);

	return err;
}

static int
wl_obss_coex_action(void *wl, cmd_t *cmd, char **argv)
{
	int err;
	char var[256];
	wl_action_obss_coex_req_t *req = (wl_action_obss_coex_req_t *)var;
	int val;
	int num = 0;
	uint8 options = 0;

	argv++;
	memset(&var, 0, sizeof(wl_action_obss_coex_req_t));
	while (*argv) {
		if (!strncmp(*argv, "-i", 2) && ((options & 0x1) != 0x1)) {
			argv++;
			if (!*argv)
				return BCME_USAGE_ERROR;
			val = atoi(*argv);
			if ((val != 0) && (val != 1))
				return BCME_BADARG;
			req->info |= val ? WL_COEX_40MHZ_INTOLERANT : 0;
			options |= 0x1;
		}
		else if (!strncmp(*argv, "-w", 2) && ((options & 0x2) != 0x2)) {
			argv++;
			if (!*argv)
				return BCME_USAGE_ERROR;
			val = atoi(*argv);
			if ((val != 0) && (val != 1))
				return BCME_BADARG;
			req->info |= val ? WL_COEX_WIDTH20 : 0;
			options |= 0x2;
		}
		else if (!strncmp(*argv, "-c", 2) && ((options & 0x4) != 0x4)) {
			argv++;
			while (*argv) {
				if (isdigit((unsigned char)(**argv))) {
					val = htod32(strtoul(*argv, NULL, 0));
					if ((val == 0) || (val > 14)) {
						printf("Invalid channel %d\n", val);
						return BCME_BADARG;
					}
					req->ch_list[num] = (uint8)val;
					num++;
					argv++;
					if (num > 14) {
						printf("Too many channels (max 14)\n");
						return BCME_BADARG;
					}
				} else
					break;
			}
			if (!num) {
				printf("With option '-c' specified, a channel list is required\n");
				return BCME_BADARG;
			}
			req->num = num;
			options |= 0x4;
			continue;
		}
		else
			return BCME_USAGE_ERROR;
		argv++;
	}
	if (!options)
		return BCME_BADARG;
	err = wlu_var_setbuf(wl, cmd->name, &var, (sizeof(wl_action_obss_coex_req_t) +
		(req->num ? (req->num - 1) * sizeof(uint8) : 0)));
	return err;
}

/*
 * wl lq command module
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
 * $Id: wluc_lq.c 458728 2014-02-27 18:15:25Z $
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

static cmd_func_t wl_rssi_event, wl_chan_qual_event;
static cmd_func_t wl_chanim_state, wl_chanim_mode;
static cmd_func_t wl_dump_lq;
static cmd_func_t wl_monitor_lq;
static cmd_func_t wl_chanim_acs_record;
static cmd_func_t wl_chanim_stats;
static int _wl_dump_lq(void *wl);

static cmd_t wl_lq_cmds[] = {
	{ "rssi_event", wl_rssi_event, WLC_GET_VAR, WLC_SET_VAR,
	"Set parameters associated with RSSI event notification\n"
	"\tusage: wl rssi_event <rate_limit> <rssi_levels>\n"
	"\trate_limit: Number of events posted to application will be limited"
	" to 1 per this rate limit. Set to 0 to disable rate limit.\n"
	"\trssi_levels: Variable number of RSSI levels (maximum 8) "
	" in increasing order (e.g. -85 -70 -60). An event will be posted"
	" each time the RSSI of received beacons/packets crosses a level."},
	{ "chq_event", wl_chan_qual_event, WLC_GET_VAR, WLC_SET_VAR,
	"Set parameters associated with channel quality  event notification\n"
	"\tusage: wl chq_event <rate_limit> <cca_levels> <nf_levels> <nf_lte_levels>\n"
	"\trate_limit: Number of events posted to application will be limited"
	" to 1 per this rate limit. Set to 0 to disable rate limit.\n"
	"\tcsa/nf/nf_lte levels: Variable number of threshold levels (maximum 8)"
	" in pairs of hi-to-low/lo-to-hi, and in increasing order (e.g. -90 -85 -80)."
	" A 0 0 pair terminates level array for one metric."
	" An event will be posted whenever a threshold is being crossed."},
	{"chanim_state", wl_chanim_state, WLC_GET_VAR, -1,
	"get channel interference state\n"
	"\tUsage: wl chanim_state channel\n"
	"\tValid channels: 1 - 14\n"
	"\treturns: 0 - Acceptable; 1 - Severe"
	},
	{"chanim_mode", wl_chanim_mode, WLC_GET_VAR, WLC_SET_VAR,
	"get/set channel interference measure (chanim) mode\n"
	"\tUsage: wl chanim_mode <value>\n"
	"\tvalue: 0 - disabled; 1 - detection only; 2 - detection and avoidance"
	},
	{"chanim_acs_record", wl_chanim_acs_record, WLC_GET_VAR, -1,
	"get the auto channel scan record. \n"
	"\t Usage: wl acs_record"
	},
	{"chanim_stats", wl_chanim_stats, WLC_GET_VAR, -1,
	"get chanim stats \n"
	"\t Usage: wl chanim_stats"
	},
	{ "monitor_lq", wl_monitor_lq, WLC_GET_VAR, WLC_SET_VAR,
	"Start/Stop monitoring link quality metrics - RSSI and SNR\n"
	"\tUsage: wl monitor_lq <0: turn off / 1: turn on"},
	{ "monitor_lq_status", wl_dump_lq, WLC_GET_VAR, -1 /* Set not reqd */,
	"Returns averaged link quality metrics - RSSI and SNR values"},
	{ NULL, NULL, 0, 0, NULL }
};

static char *buf;

/* module initialization */
void
wluc_lq_module_init(void)
{
	/* get the global buf */
	buf = wl_get_buf();

	/* register lq commands */
	wl_module_cmds_register(wl_lq_cmds);
}

static int
wl_chan_qual_event(void *wl, cmd_t *cmd, char **argv)
{
	int ret;
	const char *CHAN_QUAL_NAME[WL_CHAN_QUAL_TOTAL] = {"   CCA", "    NF", "NF_LTE"};

	if (!*++argv) {
		/* get */
		void *ptr = NULL;
		wl_chan_qual_event_t chq;
		uint i, j;

		if ((ret = wlu_var_getbuf(wl, cmd->name, NULL, 0, &ptr)) < 0)
			return ret;

		memcpy(&chq, ptr, sizeof(chq));
		chq.rate_limit_msec = dtoh32(chq.rate_limit_msec);

		printf("rate per %dms\n", chq.rate_limit_msec);
		for (i = 0; i < WL_CHAN_QUAL_TOTAL; i++) {
			printf("%s[%d]:", CHAN_QUAL_NAME[i], chq.metric[i].id);
			for (j = 0; (j < chq.metric[i].num_levels) &&
				(j < MAX_CHAN_QUAL_LEVELS); j++) {
				printf(" (%d, %d)", chq.metric[i].htol[j], chq.metric[i].ltoh[j]);
			}
			printf("\n");
		}
	} else {
		/* set */
		wl_chan_qual_event_t chq;
		uint i;

		memset(&chq, 0, sizeof(wl_chan_qual_event_t));
		chq.rate_limit_msec = atoi(*argv++);
		chq.rate_limit_msec = htod32(chq.rate_limit_msec);
		chq.num_metrics = htod16(WL_CHAN_QUAL_TOTAL);

		for (i = 0; i < WL_CHAN_QUAL_TOTAL; i++) {
			chq.metric[i].id = i;
			while (argv[0] && argv[1]) {
				int16 htol, ltoh;
				htol = htod16(atoi(*argv++));
				ltoh = htod16(atoi(*argv++));

				/* double zeros terminate one metric */
				if ((htol == 0) && (ltoh == 0))
					break;

				/* make sure that ltoh >= htol */
				if (ltoh < htol)
					return -1;

				/* ignore extra thresholds */
				if (chq.metric[i].num_levels >= MAX_CHAN_QUAL_LEVELS)
					continue;

				chq.metric[i].htol[chq.metric[i].num_levels] = htol;
				chq.metric[i].ltoh[chq.metric[i].num_levels] = ltoh;

				/* all metric threshold levels must be in increasing order */
				if (chq.metric[i].num_levels > 0) {
					if ((chq.metric[i].htol[chq.metric[i].num_levels] <=
						chq.metric[i].htol[chq.metric[i].num_levels - 1]) ||
					    (chq.metric[i].ltoh[chq.metric[i].num_levels] <=
						chq.metric[i].ltoh[chq.metric[i].num_levels - 1])) {
						return -1;
					}
				}

				(chq.metric[i].num_levels)++;
			}
		}

		if (*argv) {
			/* too many parameters */
			return -1;
		}

		ret = wlu_var_setbuf(wl, cmd->name, &chq, sizeof(chq));
	}
	return ret;
}

static int
wl_rssi_event(void *wl, cmd_t *cmd, char **argv)
{
	int ret;

	if (!*++argv) {
		/* get */
		void *ptr = NULL;
		wl_rssi_event_t rssi;
		uint i;

		if ((ret = wlu_var_getbuf(wl, cmd->name, NULL, 0, &ptr)) < 0)
			return ret;

		memcpy(&rssi, ptr, sizeof(rssi));
		rssi.rate_limit_msec = dtoh32(rssi.rate_limit_msec);

		printf("%d", rssi.rate_limit_msec);
		for (i = 0; i < rssi.num_rssi_levels; i++) {
			printf(" %d", rssi.rssi_levels[i]);
		}
		printf("\n");
	} else {
		/* set */
		wl_rssi_event_t rssi;

		memset(&rssi, 0, sizeof(wl_rssi_event_t));
		rssi.rate_limit_msec = atoi(*argv);

		while (*++argv && rssi.num_rssi_levels < MAX_RSSI_LEVELS) {
			rssi.rssi_levels[rssi.num_rssi_levels++] = atoi(*argv);
			if (rssi.num_rssi_levels > 1) {
				if (rssi.rssi_levels[rssi.num_rssi_levels - 1] <=
					rssi.rssi_levels[rssi.num_rssi_levels - 2]) {
					/* rssi levels must be in increasing order */
					return BCME_USAGE_ERROR;
				}
			}
		}

		if (*argv) {
			/* too many parameters */
			return BCME_USAGE_ERROR;
		}

		rssi.rate_limit_msec = htod32(rssi.rate_limit_msec);
		ret = wlu_var_setbuf(wl, cmd->name, &rssi, sizeof(rssi));
	}
	return ret;
}

static int
wl_chanim_state(void *wl, cmd_t *cmd, char **argv)
{
	uint32 chanspec;
	int argc = 0;
	int ret, val;

	argv++;

	/* find the arg count */
	while (argv[argc])
		argc++;

	if (argc != 1)
		return BCME_USAGE_ERROR;

	chanspec = wf_chspec_aton(*argv);
	chanspec = wl_chspec32_to_driver(chanspec);
	if (chanspec == INVCHANSPEC) {
		return BCME_USAGE_ERROR;
	}

	ret = wlu_iovar_getbuf(wl, cmd->name, &chanspec, sizeof(chanspec),
	                       buf, WLC_IOCTL_SMLEN);
	if (ret < 0)
		return ret;
	val = *(int*)buf;
	val = dtoh32(val);

	printf("%d\n", val);
	return 0;
}

static int
wl_chanim_mode(void *wl, cmd_t *cmd, char **argv)
{
	int ret;
	int val;
	char *endptr;
	int mode;

	if (!*++argv) {
		if (cmd->get < 0)
			return -1;
		if ((ret = wlu_iovar_getint(wl, cmd->name, &mode)) < 0)
			return ret;

		switch (mode) {
		case CHANIM_DISABLE:
			printf("CHANIM mode: disabled.\n");
			break;
		case CHANIM_DETECT:
			printf("CHANIM mode: detect only.\n");
			break;
		case CHANIM_EXT:
			printf("CHANIM mode: external (acsd).\n");
			break;
		case CHANIM_ACT:
			printf("CHANIM mode: detect + act.\n");
			break;
		}
		return 0;
	} else {
		mode = CHANIM_DETECT;
		val = strtol(*argv, &endptr, 0);
		if (*endptr != '\0')
			return BCME_USAGE_ERROR;

		switch (val) {
			case 0:
				mode = CHANIM_DISABLE;
				break;
			case 1:
				mode = CHANIM_DETECT;
				break;
			case 2:
				mode = CHANIM_EXT;
				break;
			case 3:
				mode = CHANIM_ACT;
				break;
			default:
				return BCME_BADARG;
		}

#ifndef DSLCPE_ENDIAN
		mode = htod32(mode);
#endif
		return wlu_iovar_setint(wl, cmd->name, mode);
	}
}

static int
_wl_dump_lq(void *wl)
{
	int ret = BCME_OK, noise = 0;
	wl_lq_t *plq = NULL;
	void *ptr = NULL;

	memset(buf, 0, sizeof(wl_lq_t));

	/* Display stats when disabled */
	if ((ret = wlu_get(wl, WLC_GET_PHY_NOISE, &noise, sizeof(int))) < 0) {
		printf("wlc_get noise failed with retcode:%d\n", ret);
		return ret;
	}

	if ((ret = wlu_var_getbuf_sm (wl, "monitor_lq_status", NULL, 0, &ptr)) < 0) {
		printf("wlc_get lq_status failed with retcode:%d\n", ret);
		return ret;
	}

	plq = (wl_lq_t *)ptr;

	if (!plq->isvalid) {
		printf("Stats collection currently disabled"
	               "['wl monitor_lq 1' to enable statistics collection]\n");
		return ret;
	}

	noise = dtoh32(noise);
	plq->rssi[LQ_IDX_MIN] = dtoh32(plq->rssi[LQ_IDX_MIN]);
	plq->rssi[LQ_IDX_MAX] = dtoh32(plq->rssi[LQ_IDX_MAX]);
	plq->rssi[LQ_IDX_AVG] = dtoh32(plq->rssi[LQ_IDX_AVG]);

	printf("rss: %d, %d, %d\nsnr: %d, %d, %d\n",
		plq->rssi[LQ_IDX_MIN],
		plq->rssi[LQ_IDX_AVG],
		plq->rssi[LQ_IDX_MAX],
		plq->rssi[LQ_IDX_MIN]-noise,
		plq->rssi[LQ_IDX_AVG]-noise,
		plq->rssi[LQ_IDX_MAX]-noise);

	return ret;
} /* _wl_dump_lq */

static int
wl_dump_lq(void *wl, cmd_t *cmd, char **argv)
{
	int ret = BCME_OK;

	UNUSED_PARAMETER(cmd);

	if (!*++argv)
		ret = _wl_dump_lq(wl);

	return ret;
} /* wl_dump_lq */

static int
wl_monitor_lq(void *wl, cmd_t *cmd, char **argv)
{
	int ret = BCME_OK;
	char *endptr = NULL;
	char **startptr = argv;

	if (!*++startptr) { /* Get */
		ret = wl_varint(wl, cmd, argv);
	}
	else {
		int val = *startptr[0];
		val = strtol(*startptr, &endptr, 0);

		if (*endptr != '\0') {
			return BCME_USAGE_ERROR;
		}
#ifndef DSLCPE_ENDIAN
		val = htod32(val);
#endif
		if (val == LQ_STOP_MONITOR) {
			if ((ret = _wl_dump_lq(wl)))
				return ret;
		}

		ret = wl_varint(wl, cmd, argv); /* Standard set call after getting stats */
	}

	return ret;
} /* wl_monitor_lq */

static int
wl_chanim_acs_record(void *wl, cmd_t *cmd, char **argv)
{
	void *ptr = NULL;
	int err = 0, i;
	wl_acs_record_t *result;

	/* need to add to this str if new acs trigger type is added */
	const char *trig_str[] = {"None", "IOCTL", "CHANIM", "TIMER", "BTA"};

	UNUSED_PARAMETER(argv);

	if ((err = wlu_var_getbuf(wl, cmd->name, NULL, 0, &ptr)) < 0)
		return err;

	result = (wl_acs_record_t *) ptr;

	if (!result->count) {
		printf("There is no ACS recorded\n");
		return err;
	}
#ifdef DSLCPE_ENDIAN
	printf("current timestamp: %u (ms)\n", dtoh32(result->timestamp));
#else
	printf("current timestamp: %u (ms)\n", result->timestamp);
#endif

	printf("Timestamp(ms)  ACS Trigger  Selected Channel  Glitch Count  CCA Count\n");
	for (i = 0; i < result->count; i++) {
		uint8 idx = CHANIM_ACS_RECORD - result->count + i;
		chanim_acs_record_t * record = &result->acs_record[idx];

		record->selected_chspc = wl_chspec_from_driver(record->selected_chspc);
#ifdef DSLCPE_ENDIAN
		printf("%10u \t%s \t%10d \t%12d \t%8d\n", dtoh32(record->timestamp),
		   trig_str[record->trigger], wf_chspec_ctlchan(record->selected_chspc),
		   dtoh32(record->glitch_cnt), record->ccastats);

#else
		printf("%10u \t%s \t%10d \t%12d \t%8d\n", record->timestamp,
		   trig_str[record->trigger], wf_chspec_ctlchan(record->selected_chspc),
		   record->glitch_cnt, record->ccastats);
#endif
	}
	return err;
}

static void
wl_chanim_stats_us_print(void *ptr, uint32 count)
{
	wl_chanim_stats_us_t *list;
	chanim_stats_us_t *stats_us;
	list = (wl_chanim_stats_us_t*)ptr;

	list->buflen = dtoh32(list->buflen);
	list->count = dtoh32(list->count);

	if (list->count == WL_CHANIM_COUNT_US_RESET) {
		printf("Reset done\n");
		return;
	}
	if (list->buflen == 0) {
		list->count = 0;
	}
	if (list->count == 1 && count == WL_CHANIM_COUNT_US_ONE) {
		stats_us = list->stats_us;
		stats_us->total_tm = htod32(stats_us->total_tm);
		stats_us->busy_tm = htod32(stats_us->busy_tm);
		stats_us->chanspec = htod16(stats_us->chanspec);
		stats_us->tx_tm = htod32(stats_us->tx_tm);
		stats_us->rx_obss = htod32(stats_us->rx_obss);
		stats_us->rx_bss = htod32(stats_us->rx_bss);
		stats_us->rxcrs_pri20 = htod32(stats_us->rxcrs_pri20);
		stats_us->rxcrs_sec20 = htod32(stats_us->rxcrs_sec20);
		stats_us->rxcrs_sec40 = htod32(stats_us->rxcrs_sec40);
		printf("chanspec    tx           inbss          obss           total_tm           "
				"busy_tm           pri20            sec20          sec40\n");
		printf("0x%4x\t", stats_us->chanspec);
		printf("%10u\t%10u\t%10u\t%10u\t%10u\t%10u\t%10u\t%10u\t\n",
				dtoh32(stats_us->tx_tm),
				dtoh32(stats_us->rx_bss), dtoh32(stats_us->rx_obss),
				dtoh32(stats_us->total_tm), dtoh32(stats_us->busy_tm),
				dtoh32(stats_us->rxcrs_pri20), dtoh32(stats_us->rxcrs_sec20),
				dtoh32(stats_us->rxcrs_sec40));
	} else if (list->count >= 1 && count == WL_CHANIM_COUNT_US_ALL) {
		unsigned int i;
		printf("CHAN Interference Measurement:\n");
		printf("Stats during last scan:\n");

		for (i = 0; i < list->count; i++) {
			printf(" chanspec: 0x%4x total_time: %u busy_time: %u rx_obss: %u\n",
					list->stats_us[i].chanspec, list->stats_us[i].total_tm,
					list->stats_us[i].rxcrs_pri20, list->stats_us[i].rx_obss);
		}
	}
	printf("\n");
}

static void
wl_chanim_stats_print(void *ptr, uint32 count)
{
	wl_chanim_stats_t *list;
	chanim_stats_t *stats;

	list = (wl_chanim_stats_t*)ptr;

	list->buflen = dtoh32(list->buflen);
	list->version = dtoh32(list->version);
	list->count = dtoh32(list->count);

	printf("version: %d \n", list->version);

	if (list->buflen == 0) {
		list->version = 0;
		list->count = 0;
	} else if (list->version != WL_CHANIM_STATS_VERSION) {
		printf("Sorry, your driver has wl_chanim_stats version %d "
				"but this program supports only version %d.\n",
				list->version, WL_CHANIM_STATS_VERSION);
		list->buflen = 0;
		list->count = 0;
	}

	if (list->count == 1 && count == WL_CHANIM_COUNT_ONE) {
		unsigned int i;
		stats = list->stats;
#ifdef DSLCPE_ENDIAN
		stats->glitchcnt = dtoh32(stats->glitchcnt);
		stats->badplcp = dtoh32(stats->badplcp);
		stats->chanspec = dtoh16(stats->chanspec);
		stats->timestamp = dtoh32(stats->timestamp);
#else
		stats->glitchcnt = htod32(stats->glitchcnt);
		stats->badplcp = htod32(stats->badplcp);
		stats->chanspec = htod16(stats->chanspec);
		stats->timestamp = htod32(stats->timestamp);
#endif

		printf("chanspec tx   inbss   obss   nocat   nopkt   doze     txop     "
				"goodtx  badtx   glitch   badplcp  knoise  idle  timestamp\n");
		printf("0x%4x\t", stats->chanspec);
		for (i = 0; i < CCASTATS_MAX; i++) {
			printf("%d\t", stats->ccastats[i]);
		}
#ifdef DSLCPE
		printf("%d\t%d\t%d\t%d\t%d", stats->glitchcnt, stats->badplcp,
			stats->bgnoise, stats->chan_idle, stats->timestamp);
#else
		printf("%d\t%d\t%d\t%d\t%d", dtoh32(stats->glitchcnt), dtoh32(stats->badplcp),
				stats->bgnoise, stats->chan_idle, dtoh32(stats->timestamp));
#endif
	} else if (list->count >= 1 && count == WL_CHANIM_COUNT_ALL) {
		unsigned int i;
		printf("CHAN Interference Measurement:\n");
		printf("Stats during last scan:\n");
		for (i = 0; i < list->count; i++) {
			printf(" chanspec: 0x%x crsglitch cnt: %d bad plcp: %d noise: %d\n",
					list->stats[i].chanspec, list->stats[i].glitchcnt,
					list->stats[i].badplcp, list->stats[i].bgnoise);

			printf("\t cca_txdur: %d cca_inbss: %d cca_obss:"
					"%d cca_nocat: %d cca_nopkt: %d\n",
					list->stats[i].ccastats[CCASTATS_TXDUR],
					list->stats[i].ccastats[CCASTATS_INBSS],
					list->stats[i].ccastats[CCASTATS_OBSS],
					list->stats[i].ccastats[CCASTATS_NOCTG],
					list->stats[i].ccastats[CCASTATS_NOPKT]);
		}
	}

	printf("\n");
}
static int
wl_chanim_stats(void *wl, cmd_t *cmd, char **argv)
{
	int err;
	wl_chanim_stats_t param;
	void *ptr;

	argv++;
	if (*argv != NULL) {
		if (!strcmp(*argv, "us")) {
			argv++;
			if (*argv != NULL) {
				if (!strcmp(*argv, "all")) {
					param.count = htod32(WL_CHANIM_COUNT_US_ALL);
					param.buflen = htod32(sizeof(wl_chanim_stats_us_t)*28);
				} else if (!strcmp(*argv, "reset")) {
					param.count = htod32(WL_CHANIM_COUNT_US_RESET);
					param.buflen = 0;
				} else {
					printf("Invalid option\n");
					return 0;
				}
			} else {
				param.count = htod32(WL_CHANIM_COUNT_US_ONE);
				param.buflen = htod32(sizeof(wl_chanim_stats_us_t));
			}
			if ((err = wlu_var_getbuf(wl, cmd->name, &param,
					sizeof(wl_chanim_stats_us_t), &ptr)) < 0) {
				printf("failed to get chanim results");
				return err;
			}

#ifdef DSLCPE_ENDIAN
			param.count = dtoh32(param.count);
#endif
			wl_chanim_stats_us_print(ptr, param.count);
			return 0;
		} else if (!strcmp(*argv, "all")) {
			param.buflen = htod32(sizeof(wl_chanim_stats_t)*28);
			param.count = htod32(WL_CHANIM_COUNT_ALL);
		} else {
			printf("Invalid option\n");
			return 0;
		}
	} else {
		param.buflen = htod32(sizeof(wl_chanim_stats_t));
		param.count = htod32(WL_CHANIM_COUNT_ONE);
	}

	if ((err = wlu_var_getbuf(wl, cmd->name, &param,
		sizeof(wl_chanim_stats_t), &ptr)) < 0) {
		printf("failed to get chanim results");
		return err;
	}

#ifdef DSLCPE_ENDIAN
	param.count = dtoh32(param.count);
#endif
	
	wl_chanim_stats_print(ptr, param.count);
	return (err);
}

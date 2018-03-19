/*
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
 * $Id: acs.c 719698 2017-09-06 09:58:12Z $
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <assert.h>
#include <typedefs.h>
#include <bcmnvram.h>
#include <bcmutils.h>
#include <bcmendian.h>

#include <bcmwifi_channels.h>
#include <wlioctl.h>
#include <wlioctl_utils.h>
#include <wlutils.h>
#include <shutils.h>
#include <proto/ethernet.h>

#include "acsd_svr.h"

#define PREFIX_LEN 32

/* some channel bounds */
#define ACS_CS_MIN_2G_CHAN	1	/* min channel # in 2G band */
#define ACS_CS_MAX_2G_CHAN	CH_MAX_2G_CHANNEL	/* max channel # in 2G band */
#define ACS_CS_MIN_5G_CHAN	36	/* min channel # in 5G band */
#define ACS_CS_MAX_5G_CHAN	MAXCHANNEL	/* max channel # in 5G band */

/* possible min channel # in the band */
#define ACS_CS_MIN_CHAN(band)	((band == WLC_BAND_5G) ? ACS_CS_MIN_5G_CHAN : \
			(band == WLC_BAND_2G) ? ACS_CS_MIN_2G_CHAN : 0)
/* possible max channel # in the band */
#define ACS_CS_MAX_CHAN(band)	((band == WLC_BAND_5G) ? ACS_CS_MAX_5G_CHAN : \
			(band == WLC_BAND_2G) ? ACS_CS_MAX_2G_CHAN : 0)

#define BAND_2G(band) (band == WLC_BAND_2G)
#define BAND_5G(band) (band == WLC_BAND_5G)

#define ACS_SM_BUF_LEN  1024
#define ACS_SRSLT_BUF_LEN (32*1024)

/* Need 13, strlen("per_chan_info"), +4, sizeof(uint32). Rounded to 20. */
#define ACS_PER_CHAN_INFO_BUF_LEN 20

#define ACS_CHANNEL_1 1
#define ACS_CHANNEL_6 6
#define ACS_CHANNEL_11 11

#define MAX_KEY_LEN 16				/* the expanded key format string must fit within */
#define ACS_VIDEO_STA_TYPE "video"		/* video sta type */
static const char *station_key_fmtstr = "toa-sta-%d";	/* format string, passed to snprintf() */

/*
 * channel_pick_t: Return value from the channel pick (preference comparison) functions
 *	PICK_NONE	: Function made no choice, someone else is to decide.
 *	PICK_CANDIDATE	: Candidate chanspec preferred over current chanspec
 *	PICK_CURRENT	: Current chanspec is preferred over candidate.
 */
typedef enum { PICK_NONE = 0, PICK_CURRENT, PICK_CANDIDATE } channel_pick_t;

#define ACS_DFLT_FLAGS ACS_FLAGS_LASTUSED_CHK

acs_policy_t predefined_policy[ACS_POLICY_MAX] = {
/* threshld    Channel score weigths values                                      chan */
/* bgn  itf  {  BSS  BUSY  INTF I-ADJ   FCS TXPWR NOISE TOTAL   CNS   ADJ TXOP}  pick */
/* --- ----   ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----   func */
{  -65,  40, {   50,    1,    2,    0,    0,    0,    1,    1,    1,    0,   1}, NULL}, /* 0=DEFAULT   */
{    0, 100, { -100,    0,    0,    0,    0,    0,    0,    0,    1,    0,   0}, NULL}, /* 1=LEGACY    */
{  -65,  40, {   -1,    0, -100,   -1,    0,    0,    0,    0,    1,    0,   0}, NULL}, /* 2=INTF      */
{  -65,  40, {   -1, -100, -100,   -1,    0,    0, -100,    0,    1,    0,   0}, NULL}, /* 3=INTF_BUSY */
{  -65,  40, {   -1, -100, -100,   -1, -100,    0, -100,    0,    1,    0,   0}, NULL}, /* 4=OPTIMIZED */
{  -55,  45, { -200,    0, -100,  -50,    0,    0,  -50,    0,    1,    0,   0}, NULL}, /* 5=CUSTOM1   */
{  -70,  45, {   -1,  -50, -100,  -10,  -10,    0,  -50,    0,    1,    0,   0}, NULL}, /* 6=CUSTOM2   */
{    0, 100, {    0,    0,    0,    1,    0,    0,    0,    0,    1,    1,   0}, NULL}, /* 7=FCS       */
};

acs_info_t *acs_info;

/* is chanspec DFS channel */
static bool acs_is_dfs_chanspec(acs_chaninfo_t *c_info, chanspec_t chspec);

/* get traffic information of the interface */
static int acs_get_traffic_info(acs_chaninfo_t * c_info, acs_traffic_info_t *t_info);

/* get traffic information about TOAd video STAs (if any) */
static int acs_get_video_sta_traffic_info(acs_chaninfo_t * c_info, acs_traffic_info_t *t_info);

/* identifies the best DFS channel to do BGDFS on; either for preclearing or to move */
static int
acs_bgdfs_choose_channel(acs_chaninfo_t * c_info, bool include_unclear,	bool pick_160);

/* is chanspec DFS weather channel */
static bool acs_is_dfs_weather_chanspec(acs_chaninfo_t *c_info, chanspec_t chspec);

/* is EU country  */
static bool acs_is_country_edcrs_eu(char * country_code);

/* get country info */
static int acs_get_country(acs_chaninfo_t * c_info);

static bool
acsd_is_lp_chan(acs_chaninfo_t *c_info, chanspec_t chspec);

/* look for str in capability (wl cap) and return true if found */
static bool
acs_check_cap(acs_chaninfo_t *c_info, char *str);

/* To check whether bss is enabled for particaular interface or not */
static int
acs_check_bss_is_enabled(char *name, acs_chaninfo_t **c_info_ptr, char *prefix);

static void
acs_ci_scan_update_idx(acs_scan_chspec_t *chspec_q, uint8 increment)
{
	uint8 idx = chspec_q->idx + increment;
	uint32 chan_flags;

	if (idx >= chspec_q->count)
		idx = 0;

	do {
		chan_flags = chspec_q->chspec_list[idx].flags;

		/* check if it is preffered channel and pref scanning requested */
		if ((chspec_q->ci_scan_running == ACS_CI_SCAN_RUNNING_PREF)) {
			if ((chan_flags & ACS_CI_SCAN_CHAN_PREF))
				break;
		} else if (!(chan_flags & ACS_CI_SCAN_CHAN_EXCL))
			break;

		/* increment index */
		if (++idx == chspec_q->count)
			idx = 0;

	} while (idx != chspec_q->idx);

	chspec_q->idx = idx;
}

/*
 * Function to set a channel table by parsing a list consisting
 * of a comma-separated channel numbers.
 */
static int
acs_set_chan_table(char *channel_list, chanspec_t *chspec_list,
                      unsigned int vector_size)
{
	int chan_index;
	int channel;
	int chan_count = 0;
	char *chan_str;
	char *delim = ",";
	char chan_buf[ACS_MAX_VECTOR_LEN + 2];
	int list_length;

	/*
	* NULL list means no channels are set. Return without
	* modifying the vector.
	*/
	if (channel_list == NULL)
		return 0;

	/*
	* A non-null list means that we must set the vector.
	*  Clear it first.
	* Then parse a list of <chan>,<chan>,...<chan>
	*/
	memset(chan_buf, 0, sizeof(chan_buf));
	list_length = strlen(channel_list);
	list_length = MIN(list_length, ACS_MAX_VECTOR_LEN);
	strncpy(chan_buf, channel_list, list_length);
	strncat(chan_buf, ",", list_length);

	chan_str = strtok(chan_buf, delim);

	for (chan_index = 0; chan_index < vector_size; chan_index++)
	{
		if (chan_str == NULL)
			break;
		channel = strtoul(chan_str, NULL, 16);
		if (channel == 0)
			break;
		chspec_list[chan_count++] = channel;
		chan_str = strtok(NULL, delim);
	}
	return chan_count;
}

#ifdef DEBUG
static void
acs_dump_config_extra(acs_chaninfo_t *c_info)
{
	acs_fcs_t *fcs_info = &c_info->acs_fcs;
	uint8 intf_thld_setting = fcs_info->intfparams.thld_setting;

	ACSD_FCS("acs_dump_config_extra:\n");
	ACSD_FCS("\t acs_txdelay_period: %d\n", fcs_info->acs_txdelay_period);
	ACSD_FCS("\t acs_txdelay_cnt: %d\n", fcs_info->acs_txdelay_cnt);
	ACSD_FCS("\t acs_txdelay_ratio: %d\n", fcs_info->acs_txdelay_ratio);
	ACSD_FCS("\t acs_dfs: %d\n", fcs_info->acs_dfs);
	ACSD_FCS("\t acs_far_sta_rssi: %d\n", fcs_info->acs_far_sta_rssi);
	ACSD_FCS("\t acs_nofcs_least_rssi: %d\n", fcs_info->acs_nofcs_least_rssi);
	ACSD_FCS("\t acs_chan_dwell_time: %d\n", fcs_info->acs_chan_dwell_time);
	ACSD_FCS("\t acs_chan_flop_period: %d\n", fcs_info->acs_chan_flop_period);
	ACSD_FCS("\t acs_tx_idle_cnt: %d\n", fcs_info->acs_tx_idle_cnt);
	ACSD_FCS("\t acs_cs_scan_timer: %d\n", c_info->acs_cs_scan_timer);
	ACSD_FCS("\t acs_ci_scan_timeout: %d\n", fcs_info->acs_ci_scan_timeout);
	ACSD_FCS("\t acs_ci_scan_timer: %d\n", c_info->acs_ci_scan_timer);
	ACSD_FCS("\t acs_scan_chanim_stats: %d\n", fcs_info->acs_scan_chanim_stats);
	ACSD_FCS("\t acs_fcs_chanim_stats: %d\n", fcs_info->acs_fcs_chanim_stats);
	ACSD_FCS("\t acs_fcs_mode: %d\n", c_info->acs_fcs_mode);
	ACSD_FCS("\t tcptxfail:%d\n",
		fcs_info->intfparams.acs_txfail_thresholds[intf_thld_setting].tcptxfail_thresh);
	ACSD_FCS("\t txfail:%d\n",
		fcs_info->intfparams.acs_txfail_thresholds[intf_thld_setting].txfail_thresh);
	ACSD_FCS("\t fcs_txop_weight: %d\n", c_info->fcs_txop_weight);

}
#endif /* DEBUG */

static void
acs_fcs_retrieve_config(acs_chaninfo_t *c_info, char * prefix)
{
	/* retrieve policy related configuration from nvram */
	acs_fcs_t *fcs_info = &c_info->acs_fcs;
	char tmp[100], *str;
	uint8 chan_count;

	ACSD_INFO("retrieve FCS config from nvram ...\n");

	if ((str = nvram_get(strcat_r(prefix, "acs_txdelay_period", tmp))) == NULL)
		fcs_info->acs_txdelay_period = ACS_TXDELAY_PERIOD;
	else
		fcs_info->acs_txdelay_period = atoi(str);

	if ((str = nvram_get(strcat_r(prefix, "acs_start_on_nondfs", tmp))) == NULL)
		fcs_info->acs_start_on_nondfs = ACS_START_ON_NONDFS;
	else
		fcs_info->acs_start_on_nondfs = atoi(str);

	if ((str = nvram_get(strcat_r(prefix, "acs_txdelay_cnt", tmp))) == NULL)
		fcs_info->acs_txdelay_cnt = ACS_TXDELAY_CNT;
	else
		fcs_info->acs_txdelay_cnt = atoi(str);

	if ((str = nvram_get(strcat_r(prefix, "acs_txdelay_ratio", tmp))) == NULL)
		fcs_info->acs_txdelay_ratio = ACS_TXDELAY_RATIO;
	else
		fcs_info->acs_txdelay_ratio = atoi(str);

	if ((str = nvram_get(strcat_r(prefix, "acs_far_sta_rssi", tmp))) == NULL)
		fcs_info->acs_far_sta_rssi = ACS_FAR_STA_RSSI;
	else
		fcs_info->acs_far_sta_rssi = atoi(str);

	if ((str = nvram_get(strcat_r(prefix, "acs_nofcs_least_rssi", tmp))) == NULL)
		fcs_info->acs_nofcs_least_rssi = ACS_NOFCS_LEAST_RSSI;
	else
		fcs_info->acs_nofcs_least_rssi = atoi(str);

	if ((str = nvram_get(strcat_r(prefix, "acs_scan_chanim_stats", tmp))) == NULL)
		fcs_info->acs_scan_chanim_stats = ACS_SCAN_CHANIM_STATS;
	else
		fcs_info->acs_scan_chanim_stats = atoi(str);

	if ((str = nvram_get(strcat_r(prefix, "acs_fcs_chanim_stats", tmp))) == NULL)
		fcs_info->acs_fcs_chanim_stats = ACS_FCS_CHANIM_STATS;
	else
		fcs_info->acs_fcs_chanim_stats = atoi(str);

	memset(&fcs_info->pref_chans, 0, sizeof(fcs_conf_chspec_t));
	if ((str = nvram_get(strcat_r(prefix, "acs_pref_chans", tmp))) == NULL)	{
		fcs_info->pref_chans.count = 0;
	} else {
		chan_count = acs_set_chan_table(str, fcs_info->pref_chans.clist, ACS_MAX_LIST_LEN);
		fcs_info->pref_chans.count = chan_count;
	}

	memset(&fcs_info->excl_chans, 0, sizeof(fcs_conf_chspec_t));
	if ((str = nvram_get(strcat_r(prefix, "acs_excl_chans", tmp))) == NULL)	{
		fcs_info->excl_chans.count = 0;
	} else {
		chan_count = acs_set_chan_table(str, fcs_info->excl_chans.clist, ACS_MAX_LIST_LEN);
		fcs_info->excl_chans.count = chan_count;
	}

	if ((str = nvram_get(strcat_r(prefix, "acs_dfs", tmp))) == NULL)
		fcs_info->acs_dfs = ACS_DFS_ENABLED;
	else
		fcs_info->acs_dfs = atoi(str);

	if ((str = nvram_get(strcat_r(prefix, "acs_chan_dwell_time", tmp))) == NULL)
		fcs_info->acs_chan_dwell_time = ACS_CHAN_DWELL_TIME;
	else
		fcs_info->acs_chan_dwell_time = atoi(str);

	if ((str = nvram_get(strcat_r(prefix, "acs_chan_flop_period", tmp))) == NULL)
		fcs_info->acs_chan_flop_period = ACS_CHAN_FLOP_PERIOD;
	else
		fcs_info->acs_chan_flop_period = atoi(str);

	if ((str = nvram_get(strcat_r(prefix, "acs_tx_idle_cnt", tmp))) == NULL)
		fcs_info->acs_tx_idle_cnt = ACS_TX_IDLE_CNT;
	else
		fcs_info->acs_tx_idle_cnt = atoi(str);

	if ((str = nvram_get(strcat_r(prefix, "acs_ci_scan_timeout", tmp))) == NULL)
		fcs_info->acs_ci_scan_timeout = ACS_CI_SCAN_TIMEOUT;
	else
		fcs_info->acs_ci_scan_timeout = atoi(str);

	if ((str = nvram_get(strcat_r(prefix, "acs_cs_scan_timer", tmp))) == NULL)
		c_info->acs_cs_scan_timer = ACS_DFLT_CS_SCAN_TIMER;
	else
		c_info->acs_cs_scan_timer = atoi(str);

	if ((str = nvram_get(strcat_r(prefix, "acs_ci_scan_timer", tmp))) == NULL)
		c_info->acs_ci_scan_timer = ACS_DFLT_CI_SCAN_TIMER;
	else
		c_info->acs_ci_scan_timer = atoi(str);

	if ((str = nvram_get(strcat_r(prefix, "intfer_period", tmp))) == NULL)
		fcs_info->intfparams.period = ACS_INTFER_SAMPLE_PERIOD;
	else
		fcs_info->intfparams.period = atoi(str);

	if ((str = nvram_get(strcat_r(prefix, "intfer_cnt", tmp))) == NULL)
		fcs_info->intfparams.cnt = ACS_INTFER_SAMPLE_COUNT;
	else
		fcs_info->intfparams.cnt = atoi(str);

	fcs_info->intfparams.thld_setting = ACSD_INTFER_THLD_SETTING;

	if ((str = nvram_get(strcat_r(prefix, "intfer_txfail", tmp))) == NULL) {
		fcs_info->intfparams.acs_txfail_thresholds[ACSD_INTFER_PARAMS_80_THLD].txfail_thresh =
			ACS_INTFER_TXFAIL_THRESH;
	} else {
		fcs_info->intfparams.acs_txfail_thresholds[ACSD_INTFER_PARAMS_80_THLD].txfail_thresh =
			strtoul(str, NULL, 0);
	}

	if ((str = nvram_get(strcat_r(prefix, "intfer_tcptxfail", tmp))) == NULL) {
		fcs_info->intfparams.acs_txfail_thresholds[ACSD_INTFER_PARAMS_80_THLD].tcptxfail_thresh =
			ACS_INTFER_TCPTXFAIL_THRESH;
	} else {
		fcs_info->intfparams.acs_txfail_thresholds[ACSD_INTFER_PARAMS_80_THLD].tcptxfail_thresh =
			strtoul(str, NULL, 0);
	}

	if ((str = nvram_get(strcat_r(prefix, "intfer_txfail_hi", tmp))) == NULL) {
		fcs_info->intfparams.acs_txfail_thresholds[ACSD_INTFER_PARAMS_80_THLD_HI].txfail_thresh =
			ACS_INTFER_TXFAIL_THRESH_HI;
	} else {
		fcs_info->intfparams.acs_txfail_thresholds[ACSD_INTFER_PARAMS_80_THLD_HI].txfail_thresh =
			strtoul(str, NULL, 0);
	}

	if ((str = nvram_get(strcat_r(prefix, "intfer_tcptxfail_hi", tmp))) == NULL) {
		fcs_info->intfparams.acs_txfail_thresholds[ACSD_INTFER_PARAMS_80_THLD_HI].tcptxfail_thresh =
			ACS_INTFER_TCPTXFAIL_THRESH_HI;
	} else {
		fcs_info->intfparams.acs_txfail_thresholds[ACSD_INTFER_PARAMS_80_THLD_HI].tcptxfail_thresh =
			strtoul(str, NULL, 0);
	}

	if ((str = nvram_get(strcat_r(prefix, "intfer_txfail_160", tmp))) == NULL) {
		fcs_info->intfparams.acs_txfail_thresholds[ACSD_INTFER_PARAMS_160_THLD].txfail_thresh =
			ACS_INTFER_TXFAIL_THRESH_160;
	} else {
		fcs_info->intfparams.acs_txfail_thresholds[ACSD_INTFER_PARAMS_160_THLD].txfail_thresh =
			strtoul(str, NULL, 0);
	}

	if ((str = nvram_get(strcat_r(prefix, "intfer_tcptxfail_160", tmp))) == NULL) {
		fcs_info->intfparams.acs_txfail_thresholds[ACSD_INTFER_PARAMS_160_THLD].tcptxfail_thresh =
			ACS_INTFER_TCPTXFAIL_THRESH_160;
	} else {
		fcs_info->intfparams.acs_txfail_thresholds[ACSD_INTFER_PARAMS_160_THLD].tcptxfail_thresh =
			strtoul(str, NULL, 0);
	}

	if ((str = nvram_get(strcat_r(prefix, "intfer_txfail_160_hi", tmp))) == NULL) {
		fcs_info->intfparams.acs_txfail_thresholds[ACSD_INTFER_PARAMS_160_THLD_HI].txfail_thresh =
			ACS_INTFER_TXFAIL_THRESH_160_HI;
	} else {
		fcs_info->intfparams.acs_txfail_thresholds[ACSD_INTFER_PARAMS_160_THLD_HI].txfail_thresh =
			strtoul(str, NULL, 0);
	}

	if ((str = nvram_get(strcat_r(prefix, "intfer_tcptxfail_160_hi", tmp))) == NULL) {
		fcs_info->intfparams.acs_txfail_thresholds[ACSD_INTFER_PARAMS_160_THLD_HI].tcptxfail_thresh =
			ACS_INTFER_TCPTXFAIL_THRESH_160_HI;
	} else {
		fcs_info->intfparams.acs_txfail_thresholds[ACSD_INTFER_PARAMS_160_THLD_HI].tcptxfail_thresh =
			strtoul(str, NULL, 0);
	}

	if (nvram_match(strcat_r(prefix, "dcs_csa_unicast", tmp), "1"))
		fcs_info->acs_dcs_csa = CSA_UNICAST_ACTION_FRAME;
	else
		fcs_info->acs_dcs_csa = CSA_BROADCAST_ACTION_FRAME;

	if ((str = nvram_get(strcat_r(prefix, "fcs_txop_weight", tmp))) == NULL)
		fcs_info->txop_weight = 0;
	else
		fcs_info->txop_weight = strtol(str, NULL, 0);

#ifdef DEBUG
	acs_dump_config_extra(c_info);
#endif /* DEBUG */
}

static int
acs_toa_load_station(acs_fcs_t *fcs_info, const char *keyfmt, int stain)
{
	char keybuf[MAX_KEY_LEN];
	char *tokens, *sta_type;
	int index = fcs_info->video_sta_idx;
	char ea[ACS_STA_EA_LEN];

	if (snprintf(keybuf, sizeof(keybuf), keyfmt, stain) >= sizeof(keybuf)) {
		ACSD_ERROR("key buffer too small\n");
		return BCME_ERROR;
	}

	tokens = nvram_get(keybuf);
	if (!tokens) {
		ACSD_INFO("No toa NVRAM params set\n");
		return BCME_ERROR;
	}

	strncpy(ea, tokens, ACS_STA_EA_LEN);
	ea[ACS_STA_EA_LEN -1] = '\0';
	sta_type = strstr(tokens, ACS_VIDEO_STA_TYPE);
	if (sta_type) {
		fcs_info->acs_toa_enable = TRUE;
		if (index >= ACS_MAX_VIDEO_STAS) {
			ACSD_ERROR("MAX VIDEO STAs exceeded\n");
			return BCME_ERROR;
		}

		strncpy(fcs_info->vid_sta[index].vid_sta_mac, ea,
				ACS_STA_EA_LEN);
		fcs_info->vid_sta[index].vid_sta_mac[ACS_STA_EA_LEN -1] = '\0';
		if (!bcm_ether_atoe(fcs_info->vid_sta[index].vid_sta_mac,
					&fcs_info->vid_sta[index].ea)) {
			ACSD_ERROR("toa video sta ether addr NOT proper\n");
			return BCME_ERROR;
		}
		fcs_info->video_sta_idx++;
		ACSD_INFO("VIDEOSTA %s\n", fcs_info->vid_sta[index].vid_sta_mac);
	}

	return BCME_OK;
}

static void
acs_bgdfs_acs_toa_retrieve_config(acs_chaninfo_t *c_info, char * prefix)
{
	/* retrieve toa related configuration from nvram */
	acs_fcs_t *fcs_info = &c_info->acs_fcs;
	int stain, ret = BCME_OK;

	fcs_info->acs_toa_enable = FALSE;
	fcs_info->video_sta_idx = 0;
	ACSD_INFO("retrieve TOA config from nvram ...\n");

	/* Load station specific settings */
	for (stain = 1; stain <= ACS_MAX_VIDEO_STAS; ++stain) {
		ret = acs_toa_load_station(fcs_info, station_key_fmtstr, stain);
		if (ret != BCME_OK)
			return;
	}
}

/*
* acs_cns_normalize_score() - Normalizes the CNS score
* cns:		The highest and lowest noise scores to use for normalization
* score:	The CNS score to normalize
*
* This function normalizes the score by computing ((score-lowest noise score)*10)
* and dividing it by (highest noise score - lowest noise score)
*
*/
static int
acs_cns_normalize_score(cns_score_t *cns, int score)
{
	int diff = score - cns->lowest_score;
	int range = cns->highest_score - cns->lowest_score;
	ACSD_DFSR("Score before normalization = %d lowest noise score"
		" = %d highest noise score = %d \n Numerator = %d"
		" Denominator = %d\n", score, cns->lowest_score,
		cns->highest_score, diff * 10, range);
	return (diff * 10) / range;
}

/*
 * acs_pick_best_possible_channel - ACSD channel selection algorithm
 * Returns the best possible channel from a list of candidates
 */
static channel_pick_t
acs_pick_best_possible_channel(acs_chaninfo_t *c_info, ch_candidate_t *current,
		ch_candidate_t *candidate, int score_type, cns_score_t *cns)
{
	int current_normalized_score, candidate_normalized_score;

	ACSD_INFO("Current: %sDFS channel #%d (0x%x) score[%d] %d,"
			" candidate: %sDFS channel #%d (0x%x) score %d.\n",
			current->is_dfs ? "" : "non-",
			CHSPEC_CHANNEL(current->chspec), current->chspec,
			score_type,
			current->chscore[score_type].score,
			candidate->is_dfs ? "" : "non-",
			CHSPEC_CHANNEL(candidate->chspec), candidate->chspec,
			candidate->chscore[score_type].score);

	/* Pick the channel having the better score */
	if (candidate->chscore[score_type].score < current->chscore[score_type].score) {
		ACSD_INFO("-- selecting candidate: better score.\n");
		return PICK_CANDIDATE;
	} else if (current->chscore[score_type].score <
			candidate->chscore[score_type].score) {
		ACSD_INFO("-- keeping current: better score.\n");
		return PICK_CURRENT;
	}

	/* In the event of a tie, pick the channel with the lower noise for 5G
	 * This includes currently CCI, ACI.
	 * TBD - Non-Wifi Interference
	 */
	if (CHSPEC_IS5G(candidate->chspec)) {
		if (ACS_FCS_MODE(c_info)) {
			current_normalized_score = acs_cns_normalize_score(cns,
				current->chscore[CH_SCORE_CNS].score);
			candidate_normalized_score = acs_cns_normalize_score(cns,
				candidate->chscore[CH_SCORE_CNS].score);

			ACSD_INFO("current channel normalized score = %d, "
				"candidate normal score = %d\n", current_normalized_score,
				candidate_normalized_score);

			/* lower noise wins */
			if (candidate_normalized_score < current_normalized_score) {
				ACSD_INFO("-- selecting candidate: better CNS score.\n");
				return PICK_CANDIDATE;
			} else if (current_normalized_score < candidate_normalized_score) {
				ACSD_INFO("-- keeping current : better CNS score.\n");
				return PICK_CURRENT;
			}

		}
		/* Both same score - leave Channel selection to the other selection parameters
		*/
		return PICK_NONE;
	} else {
		/* For 2G if there is a tie in the first level score pick the channel
		 * with the least INTFADJ score
		 */
		int intadj_score = current->chscore[CH_SCORE_INTFADJ].score;
		if (candidate->chscore[CH_SCORE_INTFADJ].score < intadj_score) {
			ACSD_INFO("-- selecting candidate: quieter 2G channel.\n");
			return PICK_CANDIDATE;
		} else {
			ACSD_INFO("-- keeping current : quieter 2G channel.\n");
			return PICK_CURRENT;
		}
	}
}

/*
 * acs_remove_noisy_cns() - Find candidate with best CNS score and disable too distant candidates.
 *
 * candi:	pointer to the candidate array
 * c_count:	number of candidates in the array
 * distance:	value of configuration parameter acs_trigger_var
 *
 * This function looks up the candidate with the best (lowest) CNS score, and removes all
 * other candidates whose CNS score is further than a certain distance by disabling them.
 * It returns the best CNS noise score which is used to normalize the CNS scores
 */
static int
acs_remove_noisy_cns(ch_candidate_t *candi, int c_count, int distance)
{
	ch_score_t *best_score_p = NULL;
	int i;

	/* Determine candidate with the best (lowest) CNS score */
	for (i = 0; i < c_count; i++) {
		if (!candi[i].valid)
			continue;
		if (!best_score_p) {
			best_score_p = candi[i].chscore;
			continue;
		}
		if ((candi[i].chscore[CH_SCORE_CNS].score
			< best_score_p[CH_SCORE_CNS].score)) {
			best_score_p = candi[i].chscore;
		}
	}

	/* ban chanspec that are too far away from best figure */
	for (i = 0; i < c_count; i++) {
		if (candi[i].valid &&
			(candi[i].chscore[CH_SCORE_CNS].score >=
			(best_score_p[CH_SCORE_CNS].score + distance))) {
			ACSD_INFO("banning chanspec %x because of interference \n",
				candi[i].chspec);
			candi[i].valid = FALSE;
			candi[i].reason |= ACS_INVALID_NOISE;
		}
	}
	return best_score_p[CH_SCORE_CNS].score;
}

/* Select channel based on the following constraints
 * For EU - prefer DFS high power, DFS low power and then low power
 * For US - prefer high power, DFS, low power
 */
static int
acs_prioritize_channels(acs_chaninfo_t *c_info, ch_candidate_t *current,
	ch_candidate_t *candidate)
{
	bool is_cur_lp = acsd_is_lp_chan(c_info, current->chspec);
	bool is_candidate_lp = acsd_is_lp_chan(c_info, candidate->chspec);
	bool is_eu = acs_is_country_edcrs_eu(c_info->country.ccode);

	if (is_eu) {
		if (!current->is_dfs && candidate->is_dfs) {
			ACSD_INFO("-- selecting candidate: DFS channel. \n");
			goto pick_candidate;
		} else if (current->is_dfs && candidate->is_dfs) {
			if (is_cur_lp && !is_candidate_lp) {
				ACSD_INFO("-- selecting candidate: High power channel. \n");
				goto pick_candidate;
			} else if (CHSPEC_CHANNEL(current->chspec) <
					CHSPEC_CHANNEL(candidate->chspec)) {
				ACSD_INFO("-- selecting candidate: Higher channel. \n");
				goto pick_candidate;
			}
		} else if (!current->is_dfs && !candidate->is_dfs) {
			if (CHSPEC_CHANNEL(current->chspec) <
					CHSPEC_CHANNEL(candidate->chspec)) {
				ACSD_INFO("-- selecting candidate: Higher channel. \n");
				goto pick_candidate;
			}

		}
	} else {
		if (is_cur_lp && !is_candidate_lp) {
			ACSD_INFO("-- selecting candidate: High power channel. \n");
			goto pick_candidate;
		} else if (!is_cur_lp && !is_candidate_lp) {
			if (current->is_dfs && !candidate->is_dfs) {
				ACSD_INFO("-- selecting candidate: Non-DFS channel. \n");
				goto pick_candidate;
			} else if ((current->is_dfs && candidate->is_dfs)||
					(!current->is_dfs && !candidate->is_dfs)) {
				if (CHSPEC_CHANNEL(current->chspec) <
						CHSPEC_CHANNEL(candidate->chspec)) {
					ACSD_INFO("-- selecting candidate: Higher channel. \n");
					goto pick_candidate;
				}
			}
		} else if (is_cur_lp && is_candidate_lp) {
			if (CHSPEC_CHANNEL(current->chspec) <
					CHSPEC_CHANNEL(candidate->chspec)) {
				ACSD_INFO("-- selecting candidate: Higher channel. \n");
				goto pick_candidate;
			}
		}
	}
	ACSD_INFO("-- Keeping current channel. \n");
	return PICK_CURRENT;
pick_candidate:
	return PICK_CANDIDATE;
}

/* Add all customer specfic channel selection criteria here
 */
static int
acs_pick_customer_prioritization(acs_chaninfo_t *c_info, ch_candidate_t *current,
	ch_candidate_t *candidate)
{
	/* Customer specific selection criteria #1
	 * DFS Channel Preference
	*/
	if (c_info->acs_cs_dfs_pref) {
		if (candidate->is_dfs && !current->is_dfs) {
			ACSD_INFO("-- selecting candidate: DFS channel. \n");
			return PICK_CANDIDATE;
		} else if (current->is_dfs && !candidate->is_dfs) {
			ACSD_INFO("-- keeping current: DFS channel. \n");
			return PICK_CURRENT;
		}
	}

	/* Customer specific selection criteria #2
	 * Channel Power Preference
	*/
	if (c_info->acs_cs_high_pwr_pref) {
		if (acsd_is_lp_chan(c_info, current->chspec) &&
				!acsd_is_lp_chan(c_info, candidate->chspec)) {
			ACSD_INFO("-- selecting candidate: High power channel. \n");
			return PICK_CANDIDATE;
		} else if (!acsd_is_lp_chan(c_info, current->chspec) &&
				acsd_is_lp_chan(c_info, candidate->chspec)) {
			ACSD_INFO("-- keeping current: High power channel. \n");
			return PICK_CURRENT;
		}
	}

	return PICK_NONE;
}

/* For EU region, prefer weather over non-weather channels
 */
static int
acs_pick_eu_weather(acs_chaninfo_t *c_info, ch_candidate_t *current,
	ch_candidate_t *candidate)
{
	/* To break further ties, prefer non-weather over
	 * weather channels
	 */
	bool current_is_weather, candidate_is_weather;

	if (!acs_is_country_edcrs_eu(c_info->country.ccode))
		return PICK_NONE;

	current_is_weather = acs_is_dfs_weather_chanspec(c_info,
			(current->chspec));
	candidate_is_weather = acs_is_dfs_weather_chanspec(c_info,
			(candidate->chspec));

	ACSD_INFO("channel and weather:: current: 0x%x %d, candidate: 0x%x %d\n",
			current->chspec, current_is_weather,
			candidate->chspec, candidate_is_weather);

	/* Both DFS, same score - non weather wins. */
	if (!candidate_is_weather && current_is_weather) {
		ACSD_INFO("-- selecting candidate: not weather.\n");
		return PICK_CANDIDATE;
	} else if (candidate_is_weather && !current_is_weather) {
		ACSD_INFO("-- keeping current: not weather.\n");
		return PICK_CURRENT;
	}

	return PICK_NONE;
}
/*
 * acs_pick_chanspec_common() - shared function to pick a chanspec to switch to.
 *
 * c_info:	pointer to the acs_chaninfo_t for this interface.
 * bw:		bandwidth to chose from
 * score_type:	CS_SCORE_xxx (index into the candidate score array)
 *
 * Returned value:
 *	The returned value is the most preferred valid chanspec from the candidate array.
 *
 * This function starts by eliminating all candidates whose CNS is too far away from the best
 * CNS score, and then selects a chanspec to use by walking the list of valid candidates and
 * selecting the most preferred one. This selection is currently based on the score_type only,
 * further selection mechanisms are in the works.
 */
static chanspec_t
acs_pick_chanspec_common(acs_chaninfo_t *c_info, int bw, int score_type)
{
	chanspec_t chspec = 0;
	int i, index = -1;
	ch_candidate_t *candi = c_info->candidate[bw];
	cns_score_t cns;
	ch_candidate_t *current, *candidate;

	ACSD_INFO("Selecting channel, score type %d...\n", score_type);
	/* find the chanspec with best figure (cns) */
	cns.lowest_score = acs_remove_noisy_cns(candi, c_info->c_count[bw],
		c_info->chanim_info->config.acs_trigger_var);
	cns.highest_score = cns.lowest_score +
		c_info->chanim_info->config.acs_trigger_var;

	/* Walk all candidate chanspecs and select the best one to use. */
	for (i = 0; i < c_info->c_count[bw]; i++) {
		channel_pick_t choice;

		if (!candi[i].valid)
			continue;

		if (index < 0) { /* No previous candi, avoid comparing against random memory */
			index = i; /* Select first valid candidate as a starting point */
			ACSD_INFO("[%d] Default: %s channel #%d (0x%x) with score %d\n",
				i, (candi[i].is_dfs) ? "DFS" : "non-DFS",
				CHSPEC_CHANNEL(candi[i].chspec), candi[i].chspec,
				candi[i].chscore[score_type].score);
			continue;
		}

		ACSD_INFO("[%d] Checking %s channel #%d (0x%x) with score %d\n",
			i, (candi[i].is_dfs) ? "DFS" : "non-DFS",
			CHSPEC_CHANNEL(candi[i].chspec), candi[i].chspec,
			candi[i].chscore[score_type].score);

		/*
		 * See if one of our choice mechanisms has a preferred candidate. Whoever picks
		 * a chanspec first wins.
		 */
		choice = PICK_NONE;
		current = &candi[index];
		candidate = &candi[i];

		choice = acs_pick_customer_prioritization(c_info, current, candidate);

		if (choice == PICK_NONE) {
			choice = acs_pick_best_possible_channel(c_info, current, candidate, score_type,
					&cns);
		}

		if (choice == PICK_NONE) {
			/* EU region -- decide between weather and non-weather
			 */
			choice = acs_pick_eu_weather(c_info, current, candidate);
		}

		if (choice == PICK_NONE) {
			/* Use the default prioritzation schemes
			*/
			choice = acs_prioritize_channels(c_info, current, candidate);
		}

		if (choice == PICK_CANDIDATE) {
			index = i;
		}
	}

	/* reset monitoring state machine */
	chanim_mark(c_info->chanim_info).best_score = 0;

	if (index >= 0) {
		chspec = candi[index].chspec;
		ACSD_INFO("Selected Channel #%d (0x%x)\n", CHSPEC_CHANNEL(chspec), chspec);
	}
	return chspec;
}

/*
 * acs_pick_chanspec_fcs_policy() - FCS policy specific function to pick a chanspec to switch to.
 *
 * c_info:	pointer to the acs_chaninfo_t for this interface.
 * bw:		bandwidth to chose from
 *
 * Returned value:
 *	The returned value is the most preferred valid chanspec from the candidate array.
 *
 * This function picks the most preferred chanspec according to the FCS policy. At the time of
 * this writing, this is a selection based on the CH_SCORE_ADJ score.
 */
static chanspec_t
acs_pick_chanspec_fcs_policy(acs_chaninfo_t *c_info, int bw)
{
	if (c_info->acs_fcs.txop_weight) /* use ADJ + TXOP */
		return acs_pick_chanspec_common(c_info, bw, CH_SCORE_TOTAL);
	else
		return acs_pick_chanspec_common(c_info, bw, CH_SCORE_ADJ);
}

/*
 * This module retrieves the following information from the wl driver before
 * deciding on the best channel:
 * 1) scan result (wl_scan_result_t)
 * 2) channel interference stats (wl_chanim_stats_t)
 * 3) scan channel spec list
 * 4) channel spec candidate (all valid channel spec for the current band, bw, locale)
 * 5) band type, coex_enable, bw_cap.
 *
 * The facts which could be weighted in the channel scoring systems are:
 * 1) Number of BSS's detected during the scan process (from scan result)
 * 2) Channel Occupancy (percentage of time the channel is occupied by other BSS's)
 * 3) Channel Interference (from CCA stats)
 * 4) Channel FCS (from CCA stats)
 * 5) Channel MAX tx power
 * 6) Adjacent Channel Interference
 * The scoring algorithm for each factor is subject to update based on testing results.
 * The weight for each factor can be customized based on different channel eval policies.
 */

static int
acs_build_scanlist(acs_chaninfo_t *c_info)
{
	wl_uint32_list_t *list;
	chanspec_t input = 0, c = 0;
	int ret = 0, i, j;
	int count = 0;
	scan_chspec_elemt_t *ch_list;
	acs_rsi_t *rsi = &c_info->rs_info;
	fcs_conf_chspec_t *pref_chans = &(c_info->acs_fcs.pref_chans);
	fcs_conf_chspec_t *excl_chans = &(c_info->acs_fcs.excl_chans);

	char *data_buf, *data_buf1 = NULL;
	data_buf = acsd_malloc(ACS_SM_BUF_LEN);

	input |= WL_CHANSPEC_BW_20;

	if (BAND_5G(rsi->band_type))
		input |= WL_CHANSPEC_BAND_5G;
	else
		input |= WL_CHANSPEC_BAND_2G;

#ifdef DSLCPE_ENDIAN
	input = htod16(input);
#endif
	ret = wl_iovar_getbuf(c_info->name, "chanspecs", &input, sizeof(chanspec_t),
		data_buf, ACS_SM_BUF_LEN);
	if (ret < 0)
		ACS_FREE(data_buf);
	ACS_ERR(ret, "failed to get valid chanspec lists");

	list = (wl_uint32_list_t *)data_buf;
	count = dtoh32(list->count);

	c_info->scan_chspec_list.count = count;
	c_info->scan_chspec_list.idx = 0;
	c_info->scan_chspec_list.pref_count = 0;
	c_info->scan_chspec_list.excl_count = 0;

	if (!count) {
		ACSD_ERROR("number of valid chanspec is 0\n");
		ret = -1;
		goto cleanup_sl;
	}

	ACS_FREE(c_info->scan_chspec_list.chspec_list);

	ch_list = c_info->scan_chspec_list.chspec_list =
		(scan_chspec_elemt_t *)acsd_malloc(count * sizeof(scan_chspec_elemt_t));

	data_buf1 = acsd_malloc(ACS_SM_BUF_LEN);

	for (i = 0; i < count; i++) {
		c = (chanspec_t)dtoh32(list->element[i]);

		ch_list[i].chspec = c;

		if (BAND_5G(rsi->band_type)) {
			input = c;
#ifdef DSLCPE
			{
			uint32 u32input=htod32((uint32)input);
			ret = wl_iovar_getbuf(c_info->name, "per_chan_info", &u32input, 
				sizeof(uint32), data_buf1, ACS_SM_BUF_LEN);
#else
			ret = wl_iovar_getbuf(c_info->name, "per_chan_info", &input,
				sizeof(chanspec_t), data_buf1, ACS_SM_BUF_LEN);

#endif
#ifdef DSLCPE
			}
#endif
			if (ret < 0) {
				ACS_FREE(data_buf);
				ACS_FREE(data_buf1);
			}
			ACS_ERR(ret, "failed to get per_chan_info");

			ch_list[i].chspec_info = dtoh32(*(uint32 *)data_buf1);

			/* Exclude DFS channels if 802.11h spectrum management is off */
			if (!rsi->reg_11h && (ch_list[i].chspec_info & WL_CHAN_RADAR)) {
				ch_list[i].flags |= ACS_CI_SCAN_CHAN_EXCL;
				c_info->scan_chspec_list.excl_count++;
			}
		}

		/* Update preffered channel attribute */
		if (pref_chans && pref_chans->count) {
			for (j = 0; j < pref_chans->count; j++) {
				if (c == pref_chans->clist[j]) {
					ch_list[i].flags |= ACS_CI_SCAN_CHAN_PREF;
					c_info->scan_chspec_list.pref_count++;
					break;
				}
			}
		}

		/* Update exclude channel attribute */
		if (AUTOCHANNEL(c_info) && excl_chans && excl_chans->count) {
			for (j = 0; j < excl_chans->count; j++) {
				if (c == excl_chans->clist[j]) {
					ch_list[i].flags |= ACS_CI_SCAN_CHAN_EXCL;
					c_info->scan_chspec_list.excl_count++;
					break;
				}
			}
		}
		ACSD_INFO("chanspec: (0x%04x), chspec_info: 0x%x  pref_chan: 0x%x\n", c,
			ch_list[i].chspec_info, ch_list[i].flags);
	}
	acs_ci_scan_update_idx(&c_info->scan_chspec_list, 0);

cleanup_sl:
	ACS_FREE(data_buf);
	ACS_FREE(data_buf1);

	return ret;
}

static int
acs_scan_prep(acs_chaninfo_t *c_info, wl_scan_params_t *params, int *params_size)
{
	int ret = 0;
	int i, scount = 0;
	acs_scan_chspec_t* scan_chspec_p = &c_info->scan_chspec_list;

	memcpy(&params->bssid, &ether_bcast, ETHER_ADDR_LEN);
	params->bss_type = DOT11_BSSTYPE_ANY;
	params->scan_type = WL_SCANFLAGS_PASSIVE;
#ifdef DSLCPE_ENDIAN
	params->nprobes = htod32(-1);
	params->active_time = htod32(-1);
	params->passive_time = htod32(ACS_CS_SCAN_DWELL);
	params->home_time = htod32(-1);
	params->channel_num = 0;
#else
	params->nprobes = -1;
	params->active_time = -1;
	params->passive_time = ACS_CS_SCAN_DWELL;
	params->home_time = -1;
	params->channel_num = 0;
#endif

	ret = acs_build_scanlist(c_info);
	ACS_ERR(ret, "failed to build scan chanspec list");

	for (i = 0; i < scan_chspec_p->count; i++) {
		if (scan_chspec_p->chspec_list[i].flags & ACS_CI_SCAN_CHAN_EXCL)
			continue;
		params->channel_list[scount++] = htodchanspec(scan_chspec_p->chspec_list[i].chspec);
	}
	params->channel_num = htod32(scount & WL_SCAN_PARAMS_COUNT_MASK);
	ACSD_INFO("scan channel number: %d\n", params->channel_num);

	*params_size = WL_SCAN_PARAMS_FIXED_SIZE + scount * sizeof(uint16);
	ACSD_INFO("params size: %d\n", *params_size);

	return ret;
}

/* channel information (quick) scan at run time */
int
acs_run_ci_scan(acs_chaninfo_t *c_info)
{
	int ret = 0;
	if (c_info->acs_escan->acs_use_escan) {
		ret = acs_run_escan(c_info, ACS_SCAN_TYPE_CI);

#ifdef DSLCPE
		if (ret != BCME_OK) {
#else
		if (ret == BCME_UNSUPPORTED) {
#endif
			/* Escan unsupported. Use normal scan */
			c_info->acs_escan->acs_use_escan = 0;
		}
	}

	if (!c_info->acs_escan->acs_use_escan) {
		ret = acs_run_normal_ci_scan(c_info);
	}

	return ret;
}

/* Default CI scan - Non EScan
 */
int acs_run_normal_ci_scan(acs_chaninfo_t *c_info)
{
	int ret = 0;
	int i;
	wl_scan_params_t *params = NULL;
	int params_size = WL_SCAN_PARAMS_FIXED_SIZE + sizeof(uint16);
	acs_scan_chspec_t* scan_chspec_q = &c_info->scan_chspec_list;
	scan_chspec_elemt_t *scan_elemt = NULL;
	bool is_dfs = FALSE;
	channel_info_t ci;

	if ((scan_chspec_q->count - scan_chspec_q->excl_count) == 0) {
		ACSD_INFO("scan chanspec queue is empty.\n");
		return ret;
	}

	scan_elemt = &scan_chspec_q->chspec_list[scan_chspec_q->idx];
	if (scan_elemt->chspec_info & WL_CHAN_RADAR)
		is_dfs = TRUE;

	params = (wl_scan_params_t*)acsd_malloc(params_size);

	memcpy(&params->bssid, &ether_bcast, ETHER_ADDR_LEN);
	params->bss_type = DOT11_BSSTYPE_ANY;
	params->scan_type = is_dfs ? WL_SCANFLAGS_PASSIVE : 0;
#ifdef DSLCPE_ENDIAN
	params->nprobes = htod32(-1);
	params->active_time = htod32(ACS_CI_SCAN_DWELL);
	params->passive_time = htod32(ACS_CI_SCAN_DWELL);
	params->home_time = htod32(-1);
	params->channel_num = htod32(1); /* 1 channel for each ci scan */
#else
	params->nprobes = -1;
	params->active_time = ACS_CI_SCAN_DWELL;
	params->passive_time = ACS_CI_SCAN_DWELL;
	params->home_time = -1;
	params->channel_num = 1; /* 1 channel for each ci scan */
#endif

	params->channel_list[0] = htodchanspec(scan_elemt->chspec);

	ret = wl_ioctl(c_info->name, WLC_SCAN, params, params_size);
	if (ret < 0)
		ACS_FREE(params);
	ACS_ERR(ret, "WLC_SCAN failed");

	if (!ret) {
		acs_ci_scan_update_idx(scan_chspec_q, 1);
		if (ACS_FCS_MODE(c_info))
			c_info->acs_fcs.timestamp_acs_scan = time(NULL);
		sleep_ms(ACS_CI_SCAN_DWELL * 5);
		for (i = 0; i < 10; i++) {
			ret = wl_ioctl(c_info->name, WLC_GET_CHANNEL, &ci, sizeof(channel_info_t));

			if (ret < 0)
				ACS_FREE(params);
			ACS_ERR(ret, "WLC_GET_CHANNEL failed");

			ci.scan_channel = dtoh32(ci.scan_channel);
			if (!ci.scan_channel)
				break;

			ACSD_PRINT("scan in progress ...\n");
			sleep_ms(2);
		}
	}
	ACSD_INFO("ci scan on chspec: 0x%x\n", scan_elemt->chspec);
	ACS_FREE(params);
	return ret;
}

/* channel selection (full) scan at init/reset time */
int
acs_run_cs_scan(acs_chaninfo_t *c_info)
{
#ifdef DSLCPE
	int ret = 0;
#else
	int ret;
#endif

	if (c_info->acs_escan->acs_use_escan) {
		ret = acs_run_escan(c_info, ACS_SCAN_TYPE_CS);

#ifdef DSLCPE
		if (ret != BCME_OK) {
#else
		if (ret == BCME_UNSUPPORTED) {
#endif
			/* Escan unsupported. Use normal scan */
			c_info->acs_escan->acs_use_escan = 0;
		}
	}

	if (!c_info->acs_escan->acs_use_escan) {
		ret = acs_run_normal_cs_scan(c_info);
	}

	return ret;
}

/* Run full scan without using ESCAN
 */
int
acs_run_normal_cs_scan(acs_chaninfo_t *c_info)
{
	int ret = 0;
	int i;
	wl_scan_params_t *params = NULL;
	int params_size = WL_SCAN_PARAMS_FIXED_SIZE + ACS_NUMCHANNELS * sizeof(uint16);
	channel_info_t ci;

	params = (wl_scan_params_t*)acsd_malloc(params_size);
	ret = acs_scan_prep(c_info, params, &params_size);
	if (ret < 0) {
		ACS_FREE(params);
		ACS_ERR(ret, "failed to do scan prep");
	}

	ret = wl_ioctl(c_info->name, WLC_SCAN, params, params_size);
	if (ret < 0) {
		ACS_FREE(params);
		ACS_ERR(ret, "WLC_SCAN failed");
	}

	memset(&ci, 0, sizeof(channel_info_t));
	/* loop to check if cs scan is done, check for scan in progress */
	if (!ret) {
		if (ACS_FCS_MODE(c_info)) {
			c_info->acs_fcs.timestamp_acs_scan = time(NULL);
			c_info->acs_fcs.timestamp_tx_idle = c_info->acs_fcs.timestamp_acs_scan;
		}
		/* this time needs to be < 1000 to prevent mpc kicking in for 2nd radio */
		sleep_ms(ACS_CS_SCAN_DWELL);
		for (i = 0; i < 100; i++) {
			ret = wl_ioctl(c_info->name, WLC_GET_CHANNEL, &ci, sizeof(channel_info_t));
			if (ret < 0) {
				ACS_FREE(params);
				ACS_ERR(ret, "WLC_GET_CHANNEL failed");
			}

			ci.scan_channel = dtoh32(ci.scan_channel);
			if (!ci.scan_channel)
				break;

			ACSD_PRINT("scan in progress ...\n");
			sleep_ms(ACS_CS_SCAN_DWELL);
		}
	}
	ACS_FREE(params);
	return ret;
}

static int
#ifdef DSLCPE_ENDIAN
acs_get_scan(acs_chaninfo_t *c_info,  char *scan_buf, uint buf_len)
{
	char *name=c_info->name;
	int b=0,len=0;
	wl_bss_info_t *bi=NULL;
#else
acs_get_scan(char* name, char *scan_buf, uint buf_len)
{
#endif
	wl_scan_results_t *list = (wl_scan_results_t*)scan_buf;
	int ret = 0;

	list->buflen = htod32(buf_len);
	ret = wl_ioctl(name, WLC_SCAN_RESULTS, scan_buf, buf_len);
	if (ret)
		ACSD_ERROR("err from WLC_SCAN_RESULTS: %d\n", ret);

	list->buflen = dtoh32(list->buflen);
	list->version = dtoh32(list->version);
	list->count = dtoh32(list->count);
	if (list->buflen == 0) {
		list->version = 0;
		list->count = 0;
	} else if (list->version != WL_BSS_INFO_VERSION &&
	           list->version != LEGACY2_WL_BSS_INFO_VERSION &&
	           list->version != LEGACY_WL_BSS_INFO_VERSION) {
		fprintf(stderr, "Sorry, your driver has bss_info_version %d "
			"but this program supports only version %d.\n",
			list->version, WL_BSS_INFO_VERSION);
		list->buflen = 0;
		list->count = 0;
	}
	ACSD_INFO("list->count: %d, list->buflen: %d\n", list->count, list->buflen);
#ifdef DSLCPE_ENDIAN
	bi=list->bss_info;
	for (b = 0; b < list->count; b ++, bi = (wl_bss_info_t*)((int8*)bi + len)) {
		endian_convert_bss_info(bi,c_info);
		len=bi->length;
	}
#endif

	return ret;
}

/* channel selection (full) scan at init/reset time */
int
acs_run_escan(acs_chaninfo_t *c_info, uint8 scan_type)
{
	int params_size = (WL_SCAN_PARAMS_FIXED_SIZE + OFFSETOF(wl_escan_params_t, params)) +
		(WL_NUMCHANNELS * sizeof(uint16));
	wl_escan_params_t *params;
	int err;
	struct timeval tv, tv_tmp;
	time_t escan_timeout;

	params = (wl_escan_params_t*)acsd_malloc(params_size);
	if (params == NULL) {
		ACSD_ERROR("Error allocating %d bytes for scan params\n", params_size);
		return BCME_NOMEM;
	}
	memset(params, 0, params_size);

	if (scan_type == ACS_SCAN_TYPE_CS) {
		tv.tv_usec = 0;
		tv.tv_sec = 1;
		err = acs_escan_prep_cs(c_info, &params->params, &params_size);
		escan_timeout = time(NULL) + WL_CS_SCAN_TIMEOUT;
	} else if (scan_type == ACS_SCAN_TYPE_CI) {
		tv.tv_sec = 0;
		tv.tv_usec = WL_CI_SCAN_TIMEOUT;
		err = acs_escan_prep_ci(c_info, &params->params, &params_size);
		escan_timeout = time(NULL) + 1;
	} else {
		ACSD_ERROR("%s Unknown scan type %d\n", c_info->name, scan_type);
		return BCME_ERROR;
	}

	params->version = htod32(ESCAN_REQ_VERSION);
	params->action = htod16(WL_SCAN_ACTION_START);

	srand((unsigned)time(NULL));
	params->sync_id = htod16(random() & 0xffff);

	params_size += OFFSETOF(wl_escan_params_t, params);
	err = wl_iovar_set(c_info->name, "escan", params, params_size);
	if (err != 0)
		goto exit2;

	c_info->acs_escan->scan_type = scan_type;
	c_info->acs_escan->acs_escan_inprogress = TRUE;

	acs_escan_free(c_info->acs_escan->escan_bss_head);
	c_info->acs_escan->escan_bss_head = NULL;
	c_info->acs_escan->escan_bss_tail = NULL;

	ACSD_INFO("Escan start \n");
	while (time(NULL) < escan_timeout && c_info->acs_escan->acs_escan_inprogress) {
		memcpy(&tv_tmp, &tv, sizeof(tv));
		acsd_main_loop(&tv);
	}
	c_info->acs_escan->acs_escan_inprogress = FALSE;
exit2:
	free(params);

	return err;
}

/* channel information (quick) scan at run time */
int
acs_escan_prep_ci(acs_chaninfo_t *c_info, wl_scan_params_t *params, int *params_size)
{
	int ret = 0;
	acs_scan_chspec_t* scan_chspec_q = &c_info->scan_chspec_list;
	scan_chspec_elemt_t *scan_elemt = NULL;
	bool is_dfs = FALSE;

	if ((scan_chspec_q->count - scan_chspec_q->excl_count) == 0) {
		ACSD_INFO("scan chanspec queue is empty.\n");
		return ret;
	}

	scan_elemt = &scan_chspec_q->chspec_list[scan_chspec_q->idx];
	if (scan_elemt->chspec_info & WL_CHAN_RADAR)
		is_dfs = TRUE;

	memcpy(&params->bssid, &ether_bcast, ETHER_ADDR_LEN);
	params->bss_type = DOT11_BSSTYPE_ANY;
	params->scan_type = is_dfs ? WL_SCANFLAGS_PASSIVE : 0;
#ifdef DSLCPE_ENDIAN
	params->nprobes = htod32(-1);
	params->active_time = htod32(ACS_CI_SCAN_DWELL);
	params->passive_time = htod32(ACS_CI_SCAN_DWELL);
	params->home_time = htod32(-1);
	params->channel_num = htod32(1); /* 1 channel for each ci scan */
#else
	params->nprobes = -1;
	params->active_time = ACS_CI_SCAN_DWELL;
	params->passive_time = ACS_CI_SCAN_DWELL;
	params->home_time = -1;
	params->channel_num = 1; /* 1 channel for each ci scan */
#endif
	params->channel_list[0] = htodchanspec(scan_elemt->chspec);

	acs_ci_scan_update_idx(scan_chspec_q, 1);
	if (ACS_FCS_MODE(c_info))
		c_info->acs_fcs.timestamp_acs_scan = time(NULL);

	*params_size = WL_SCAN_PARAMS_FIXED_SIZE + params->channel_num * sizeof(uint16);
	ACSD_INFO("ci scan on chspec: 0x%x\n", scan_elemt->chspec);

	return ret;
}

int
acs_escan_prep_cs(acs_chaninfo_t *c_info, wl_scan_params_t *params, int *params_size)
{
	int ret = 0;
	int i, scount = 0;
	acs_scan_chspec_t* scan_chspec_p = &c_info->scan_chspec_list;

	memcpy(&params->bssid, &ether_bcast, ETHER_ADDR_LEN);
	params->bss_type = DOT11_BSSTYPE_ANY;
	params->scan_type = 0; /* ACTIVE SCAN; */

#ifdef DSLCPE_ENDIAN
	params->nprobes = htod32(-1);
	params->active_time = htod32(ACS_CS_SCAN_DWELL_ACTIVE);
	params->passive_time = htod32(ACS_CS_SCAN_DWELL);
	params->home_time = htod32(-1);
	params->channel_num = htod32(0); 
#else
	params->nprobes = -1;
	params->active_time = ACS_CS_SCAN_DWELL_ACTIVE;
	params->passive_time = ACS_CS_SCAN_DWELL;

	params->home_time = -1;
	params->channel_num = 0;
#endif

	ret = acs_build_scanlist(c_info);
	ACS_ERR(ret, "failed to build scan chanspec list");

	for (i = 0; i < scan_chspec_p->count; i++) {
		if (scan_chspec_p->chspec_list[i].flags & ACS_CI_SCAN_CHAN_EXCL)
			continue;
		params->channel_list[scount++] = htodchanspec(scan_chspec_p->chspec_list[i].chspec);
	}
	params->channel_num = htod32(scount & WL_SCAN_PARAMS_COUNT_MASK);
	ACSD_INFO("scan channel number: %d\n", params->channel_num);

	*params_size = WL_SCAN_PARAMS_FIXED_SIZE + scount * sizeof(uint16);
	ACSD_INFO("params size: %d\n", *params_size);

	return ret;
}

void
acs_escan_free(struct escan_bss *node)
{
	struct escan_bss *tmp;

	while (node) {
		tmp = node->next;
		free(node);
		node = tmp;
	}
}
#ifdef ACS_DEBUG
static void
acs_dump_map(void)
{
	int i;
	ifname_idx_map_t* cur_map;

	for (i = 0; i < ACS_MAX_IF_NUM; i++) {
		cur_map = &acs_info->acs_ifmap[i];
		if (cur_map->in_use) {
			ACSD_PRINT("i: %d, name: %s, idx: %d, in_use: %d\n",
				i, cur_map->name, cur_map->idx, cur_map->in_use);
		}
	}
}
#endif /* ACS_DEBUG */

static void
acs_add_map(char *name)
{
	int i;
	ifname_idx_map_t* cur_map = acs_info->acs_ifmap;
	size_t length = strlen(name);

	ACSD_DEBUG("add map entry for ifname: %s\n", name);

	if (length >= sizeof(cur_map->name)) {
		ACSD_ERROR("Interface Name Length Exceeded\n");
	} else {
		for (i = 0; i < ACS_MAX_IF_NUM; cur_map++, i++) {
			if (!cur_map->in_use) {
				memcpy(cur_map->name, name, length + 1);
				cur_map->idx = i;
				cur_map->in_use = TRUE;
				break;
			}
		}
	}
#ifdef ACS_DEBUG
	acs_dump_map();
#endif // endif
}

int
acs_idx_from_map(char *name)
{
	int i;
	ifname_idx_map_t *cur_map;

#ifdef ACS_DEBUG
	acs_dump_map();
#endif // endif
	for (i = 0; i < ACS_MAX_IF_NUM; i++) {
		cur_map = &acs_info->acs_ifmap[i];
		if (cur_map->in_use && !strcmp(name, cur_map->name)) {
			ACSD_DEBUG("name: %s, cur_map->name: %s idx: %d\n",
				name, cur_map->name, cur_map->idx);
			return cur_map->idx;
		}
	}
	ACSD_ERROR("cannot find the mapped entry for ifname: %s\n", name);
	return -1;
}

/* maybe we do not care about 11b anymore */
static bool
acs_bss_is_11b(wl_bss_info_t* bi)
{
	uint i;
	bool b = TRUE;

	for (i = 0; i < bi->rateset.count; i++) {
		b = bi->rateset.rates[i] & 0x80;
		if (!b)
			break;
	}
	return b;
}

static void
acs_parse_chanspec(chanspec_t chanspec, acs_channel_t* chan_ptr)
{
	bzero(chan_ptr, sizeof(acs_channel_t));
	GET_ALL_EXT(chanspec, (uint8 *) chan_ptr);
}

#ifdef ACS_DEBUG
static void
acs_dump_chan_bss(acs_chan_bssinfo_t* bssinfo, int ncis)
{
	int c;
	acs_chan_bssinfo_t *cur;

	printf("channel nCtrl nExt20 nExt40 nExt80\n");
	for (c = 0; c < ncis; c++) {
		cur = &bssinfo[c];
		printf("%3d  %5d%6d%7d%7d\n", cur->channel, cur->nCtrl,
			cur->nExt20, cur->nExt40, cur->nExt80);
	}
}
#endif /* ACS_DEBUG */

void
acs_expire_scan_entry(acs_chaninfo_t *c_info, time_t limit)
{
	time_t now;
	acs_bss_info_entry_t *curptr, *previous = NULL, *past;
	acs_bss_info_entry_t **rootp = &c_info->acs_bss_info_q;

	curptr = *rootp;
	now = time(NULL);

	while (curptr) {
		time_t diff = now - curptr->timestamp;
		if (diff > limit) {
			ACSD_FCS("Scan expire: diff %dsec chanspec 0x%x, SSID %s\n",
				(int)diff, curptr->binfo_local.chanspec, curptr->binfo_local.SSID);
			if (previous == NULL)
				*rootp = curptr->next;
			else
				previous->next = curptr->next;

			past = curptr;
			curptr = curptr->next;
			ACS_FREE(past);
			continue;
		}
		previous = curptr;
		curptr = curptr->next;
	}
}

void
acs_cleanup_scan_entry(acs_chaninfo_t *c_info)
{
	acs_bss_info_entry_t *headptr = c_info->acs_bss_info_q;
	acs_bss_info_entry_t *curptr;

	while (headptr) {
		curptr = headptr;
		headptr = headptr->next;
		ACS_FREE(curptr);
	}
	c_info->acs_bss_info_q = NULL;
}

static void
display_scan_entry_local(acs_bss_info_sm_t * bsm)
{
	char ssidbuf[SSID_FMT_BUF_LEN];
	wl_format_ssid(ssidbuf, bsm->SSID, bsm->SSID_len);

	printf("SSID: \"%s\"\n", ssidbuf);
	printf("BSSID: %s\t", wl_ether_etoa(&bsm->BSSID));
	printf("chanspec: 0x%x\n", bsm->chanspec);
	printf("RSSI: %d dBm\t", (int16)bsm->RSSI);
	printf("Type: %s", ((bsm->type == ACS_BSS_TYPE_11A) ? "802.11A" :
		((bsm->type == ACS_BSS_TYPE_11G) ? "802.11G" : "802.11B")));
	printf("\n");
}

void
acs_dump_scan_entry(acs_chaninfo_t *c_info)
{
	acs_bss_info_entry_t *curptr = c_info->acs_bss_info_q;

	while (curptr) {
		display_scan_entry_local(&curptr->binfo_local);
		printf("timestamp: %u\n", (uint32)curptr->timestamp);
		curptr = curptr->next;
	}
}

static int
acs_insert_scan_entry(acs_chaninfo_t *c_info, acs_bss_info_entry_t * new)
{
	acs_bss_info_entry_t *curptr, *previous = NULL;
	acs_bss_info_entry_t **rootp = &c_info->acs_bss_info_q;

	curptr = *rootp;
	previous = curptr;

	while (curptr &&
	   memcmp(&curptr->binfo_local.BSSID, &new->binfo_local.BSSID, sizeof(struct ether_addr))) {
		previous = curptr;
		curptr = curptr->next;
	}
	new->next = curptr;
	if (previous == NULL)
		*rootp = new;
	else {
		if (curptr == NULL)
			previous->next = new;
		else /* find an existing entry */ {
			curptr->timestamp = new->timestamp;
			memcpy(&curptr->binfo_local, &new->binfo_local, sizeof(acs_bss_info_sm_t));
			ACS_FREE(new);
		}
	}
	return 0;
}

static int
acs_update_escanresult_queue(acs_chaninfo_t *c_info)
{
	struct escan_bss *escan_bss_head;
	wl_bss_info_t *bi;
	acs_bss_info_entry_t * new_entry = NULL;
	acs_channel_t chan;
	chanspec_t cur_chspec;

	for (escan_bss_head = c_info->acs_escan->escan_bss_head;
		escan_bss_head != NULL;
		escan_bss_head = escan_bss_head->next) {

		new_entry = (acs_bss_info_entry_t*)acsd_malloc(sizeof(acs_bss_info_entry_t));
		bi = escan_bss_head->bss;
#ifdef DSLCPE_ENDIAN
		new_entry->binfo_local.chanspec = cur_chspec = (bi->chanspec);
		new_entry->binfo_local.RSSI = (bi->RSSI);
#else
		new_entry->binfo_local.chanspec = cur_chspec = dtoh16(bi->chanspec);
		new_entry->binfo_local.RSSI = dtoh16(bi->RSSI);
#endif
		new_entry->binfo_local.SSID_len = bi->SSID_len;
		memcpy(new_entry->binfo_local.SSID, bi->SSID, bi->SSID_len);
		memcpy(&new_entry->binfo_local.BSSID, &bi->BSSID, sizeof(struct ether_addr));
		new_entry->timestamp = time(NULL);
		acs_parse_chanspec(cur_chspec, &chan);
		ACSD_FCS("Scan: chanspec 0x%x, control %x SSID %s\n", cur_chspec,
			chan.control, new_entry->binfo_local.SSID);
		/* BSS type in 2.4G band */
		if (chan.control <= ACS_CS_MAX_2G_CHAN) {
			if (acs_bss_is_11b(bi))
				new_entry->binfo_local.type = ACS_BSS_TYPE_11B;
			else
				new_entry->binfo_local.type = ACS_BSS_TYPE_11G;
		}
		else
			new_entry->binfo_local.type = ACS_BSS_TYPE_11A;
		acs_insert_scan_entry(c_info, new_entry);
	}
	return 0;
}

static int
acs_update_scanresult_queue(acs_chaninfo_t *c_info)
{
	wl_scan_results_t* s_result = c_info->scan_results;
	wl_bss_info_t *bi = s_result->bss_info;
	int b, len = 0;
	acs_bss_info_entry_t * new_entry = NULL;
	acs_channel_t chan;
	chanspec_t cur_chspec;

	for (b = 0; b < s_result->count; b ++, bi = (wl_bss_info_t*)((int8*)bi + len)) {
#ifdef DSLCPE_ENDIAN
		len = (bi->length);
		new_entry = (acs_bss_info_entry_t*)acsd_malloc(sizeof(acs_bss_info_entry_t));
		new_entry->binfo_local.chanspec = cur_chspec = (bi->chanspec);
		new_entry->binfo_local.RSSI = (bi->RSSI);
		new_entry->binfo_local.SSID_len = bi->SSID_len;
		memcpy(new_entry->binfo_local.SSID, bi->SSID, bi->SSID_len);
		memcpy(&new_entry->binfo_local.BSSID, &bi->BSSID, sizeof(struct ether_addr));
		new_entry->timestamp = time(NULL);
#else
		len = dtoh32(bi->length);
		new_entry = (acs_bss_info_entry_t*)acsd_malloc(sizeof(acs_bss_info_entry_t));

		new_entry->binfo_local.chanspec = cur_chspec = dtoh16(bi->chanspec);
		new_entry->binfo_local.RSSI = dtoh16(bi->RSSI);
		new_entry->binfo_local.SSID_len = bi->SSID_len;
		memcpy(new_entry->binfo_local.SSID, bi->SSID, bi->SSID_len);
		memcpy(&new_entry->binfo_local.BSSID, &bi->BSSID, sizeof(struct ether_addr));
		new_entry->timestamp = time(NULL);
#endif
		acs_parse_chanspec(cur_chspec, &chan);
		ACSD_FCS("Scan: chanspec 0x%x, control %x SSID %s\n", cur_chspec,
			chan.control, new_entry->binfo_local.SSID);
		/* BSS type in 2.4G band */
		if (chan.control <= ACS_CS_MAX_2G_CHAN) {
			if (acs_bss_is_11b(bi))
				new_entry->binfo_local.type = ACS_BSS_TYPE_11B;
			else
				new_entry->binfo_local.type = ACS_BSS_TYPE_11G;
		}
		else
			new_entry->binfo_local.type = ACS_BSS_TYPE_11A;
		acs_insert_scan_entry(c_info, new_entry);
	}
	return 0;
}

static void
acs_incr_bss_count(acs_chan_bssinfo_t *bss_info,
	acs_channel_t *chan_p, uint8 channel)
{
	int i = 0;
	uint8 *chan_parse = (uint8 *) chan_p;
	uint8 *bss_info_chan = (uint8 *) bss_info;
	uint8 channel_info_length = sizeof(acs_channel_t)/sizeof(uint8);

	for (i = 0; i < channel_info_length; i++) {
		if (*(chan_parse + i * sizeof(*chan_parse)) == channel) {
			int j = 1;
			int k = i;
			/* Find the bss count index to increment
			 * index - 0		- increase nCtrl
			 * index - 1		- increase nExt20
			 * index - 2,3		- increase nExt40
			 * index - 4,5,6,7	- increase nExt80
			 */
			while (k) {
				k >>= 1;
				j++;
			}
			*(bss_info_chan + j * sizeof(*chan_parse)) += 1;
		}
	}
}

static int
acs_update_chan_bssinfo(acs_chaninfo_t *c_info)
{
	acs_bss_info_entry_t *biq;
	scan_chspec_elemt_t* chspec_list;
	char * new_buf = NULL;
	acs_channel_t chan;
	acs_channel_t *chan_p = &chan;
	chanspec_t cur_chspec;
	int count = 0, buf_size, c;
	acs_chan_bssinfo_t *bss_info;

	count = c_info->scan_chspec_list.count;
	chspec_list = c_info->scan_chspec_list.chspec_list;

	if (count == 0)
		return 0;

	buf_size = sizeof(acs_chan_bssinfo_t) * count;
	new_buf = acsd_malloc(buf_size);

	bss_info = (acs_chan_bssinfo_t *) new_buf;

	for (c = 0; c < count; c ++) {
		bzero(&bss_info[c], sizeof(acs_chan_bssinfo_t));

		biq = c_info->acs_bss_info_q;
		/* set channel range centered by the scan channel */
		bss_info[c].channel = CHSPEC_CHANNEL(chspec_list[c].chspec);
		ACSD_DEBUG("count: %d, channel: %d\n", c, bss_info[c].channel);

		while (biq) {
			assert(biq);
			cur_chspec = biq->binfo_local.chanspec;
			acs_parse_chanspec(cur_chspec, chan_p);

			/* Find and increase bss counts per channel */
			acs_incr_bss_count(&bss_info[c], chan_p, bss_info[c].channel);
			biq = biq->next;
		}
		ACSD_DEBUG(" channel %u: %u nCtrl %u nExt20 %u nExt40 %u nExt80\n",
			bss_info[c].channel, bss_info[c].nCtrl, bss_info[c].nExt20,
			bss_info[c].nExt40, bss_info[c].nExt80);
	}

	ACS_FREE(c_info->ch_bssinfo);
	c_info->ch_bssinfo = (acs_chan_bssinfo_t *) new_buf;

#ifdef ACS_DEBUG
	acs_dump_chan_bss(c_info->ch_bssinfo, c_info->scan_chspec_list.count);
	acs_dump_scan_entry(c_info);
#endif /* ACS_DEBUG */

	return 0;
}

/* radio setting information needed from the driver */
static int
acs_get_rs_info(acs_chaninfo_t * c_info, char* prefix)
{
	int ret = 0;
	char tmp[100];
	int band, pref_chspec, coex;
	acs_rsi_t *rsi = &c_info->rs_info;
	char *str;
	char data_buf[100];
	struct {
		uint32 band;
		uint32 bw_cap;
	} param = { 0, 0 };

	/*
	 * Check if the user set the "chanspec" nvram. If not, check if
	 * the "channel" nvram is set for backward compatibility.
	 */
	if ((str = nvram_get(strcat_r(prefix, "chanspec", tmp))) == NULL) {
		str = nvram_get(strcat_r(prefix, "channel", tmp));
	}

	if (str && strcmp(str, "0")) {
		ret = wl_iovar_getint(c_info->name, "chanspec", &pref_chspec);
		ACS_ERR(ret, "failed to get chanspec");

#ifdef DSLCPE_ENDIAN
		rsi->pref_chspec = pref_chspec; /* getint function call has already handled the endian */
#else
		rsi->pref_chspec = dtoh32(pref_chspec);
#endif
		ret = wl_iovar_setint(c_info->name, "pref_chanspec", pref_chspec);
		ACS_ERR(ret, "failed to set perf_chanspec");

	}
	else {
		ret = wl_iovar_setint(c_info->name, "pref_chanspec", 0);
		ACS_ERR(ret, "failed to set perf_chanspec");
	}

	ret = wl_iovar_getint(c_info->name, "obss_coex", &coex);
	ACS_ERR(ret, "failed to get obss_coex");

#ifdef DSLCPE_ENDIAN
	rsi->coex_enb =coex;
#else
	rsi->coex_enb = dtoh32(coex);
#endif
	ACSD_INFO("coex_enb: %d\n",  rsi->coex_enb);

	ret = wl_ioctl(c_info->name, WLC_GET_BAND, &band, sizeof(band));
	ACS_ERR(ret, "failed to get band info");

	rsi->band_type = dtoh32(band);
	ACSD_INFO("band_type: %d\n",  rsi->band_type);

	param.band = band;

	ret = wl_iovar_getbuf(c_info->name, "bw_cap", &param, sizeof(param),
		data_buf, sizeof(data_buf));
	ACS_ERR(ret, "failed to get bw_cap");

#ifdef DSLCPE_ENDIAN
	rsi->bw_cap = dtoh32(*((uint32 *)data_buf));
#else
	rsi->bw_cap = *((uint32 *)data_buf);
#endif
	ACSD_INFO("bw_cap: %d\n",  rsi->bw_cap);

	return ret;
}

int
acs_request_data(acs_chaninfo_t *c_info)
{
	if (c_info->acs_escan->acs_use_escan)
		return acs_request_escan_data(c_info);
	else
		return acs_request_normal_scan_data(c_info);
}

int
acs_request_escan_data(acs_chaninfo_t *c_info)
{
	int ret;

	ret = acs_update_escanresult_queue(c_info);
	acs_update_chan_bssinfo(c_info);

	acsd_chanim_query(c_info, WL_CHANIM_COUNT_ALL, 0);

	return ret;
}

int
acs_request_normal_scan_data(acs_chaninfo_t *c_info)
{
	int ret = 0;

	char *dump_buf = acsd_malloc(ACS_SRSLT_BUF_LEN);

#ifdef DSLCPE_ENDIAN
	ret = acs_get_scan(c_info, dump_buf, ACS_SRSLT_BUF_LEN);
#else
	ret = acs_get_scan(c_info->name, dump_buf, ACS_SRSLT_BUF_LEN);
#endif

	ACS_FREE(c_info->scan_results);
	c_info->scan_results = (wl_scan_results_t *)dump_buf;

	acs_update_scanresult_queue(c_info);
	acs_update_chan_bssinfo(c_info);

	acsd_chanim_query(c_info, WL_CHANIM_COUNT_ALL, 0);

	return ret;
}

/*
 * acs_pick_chanspec_default() - default policy function to pick a chanspec to switch to.
 *
 * c_info:	pointer to the acs_chaninfo_t for this interface.
 * bw:		bandwidth to chose from
 *
 * Returned value:
 *	The returned value is the most preferred valid chanspec from the candidate array.
 *
 * This function picks the most preferred chanspec according to the default policy. At the time of
 * this writing, this is a selection based on the CH_SCORE_BSS score.
 */
static chanspec_t
acs_pick_chanspec_default(acs_chaninfo_t* c_info, int bw)
{
	return acs_pick_chanspec_common(c_info, bw, CH_SCORE_TOTAL);
}

static chanspec_t
acs_pick_chanspec(acs_chaninfo_t* c_info, int bw)
{
	chanspec_t chspec = 0;
	int i, index = -1;
	int score_type = CH_SCORE_TOTAL;
	ch_candidate_t *candi = c_info->candidate[bw];

	/* pick the chanspec with the highest total score */
	for (i = 0; i < c_info->c_count[bw]; i++) {
		if (!candi[i].valid)
			continue;

		if (index < 0) { /* No previous candi, avoid comparing against random memory */
			index = i; /* Select first valid candidate as a starting point */
			ACSD_INFO("[%d] Default: %s channel #%d (0x%x) with score %d\n",
				i, (candi[i].is_dfs) ? "DFS" : "non-DFS",
				CHSPEC_CHANNEL(candi[i].chspec), candi[i].chspec,
				candi[i].chscore[score_type].score);
			continue;
		}

		ACSD_INFO("[%d] Checking %s channel #%d (0x%x) with score %d\n",
			i, (candi[i].is_dfs) ? "DFS" : "non-DFS",
			CHSPEC_CHANNEL(candi[i].chspec), candi[i].chspec,
			candi[i].chscore[score_type].score);

		if (candi[i].chscore[score_type].score >
		    candi[index].chscore[score_type].score) {
			ACSD_INFO("-- selected higher (=better) score channel.\n");
			index = i;
		}
	}

	if (index < 0) {
		ACSD_ERROR("No valid chanspec found\n");
	} else {
		chspec = candi[index].chspec;
		ACSD_INFO("The highest score: %d, chspec: 0x%x\n",
			candi[index].chscore[score_type].score,
			chspec);
	}
	return chspec;
}

void
acs_default_policy(acs_policy_t *a_pol, acs_policy_index index)
{
	if (index >= ACS_POLICY_MAX) {
		ACSD_ERROR("Invalid acs policy index %d, reverting to default (%d).\n",
			index, ACS_POLICY_DEFAULT);
		index = ACS_POLICY_DEFAULT;
	}

	memcpy(a_pol, &predefined_policy[index], sizeof(acs_policy_t));

	if (index == ACS_POLICY_DEFAULT) {
		a_pol->chan_selector = acs_pick_chanspec_default;
	} else if (index == ACS_POLICY_FCS) {
		a_pol->chan_selector = acs_pick_chanspec_fcs_policy;
	} else {
		a_pol->chan_selector = acs_pick_chanspec;
	}
}

#ifdef DEBUG
static void
acs_dump_policy(acs_policy_t *a_pol)
{
	printf("ACS Policy:\n");
	printf("Bg Noise threshold: %d\n", a_pol->bgnoise_thres);
	printf("Interference threshold: %d\n", a_pol->intf_threshold);
	printf("Channel Scoring Weights: \n");
	printf("\t BSS: %d\n", a_pol->acs_weight[CH_SCORE_BSS]);
	printf("\t BUSY: %d\n", a_pol->acs_weight[CH_SCORE_BUSY]);
	printf("\t INTF: %d\n", a_pol->acs_weight[CH_SCORE_INTF]);
	printf("\t INTFADJ: %d\n", a_pol->acs_weight[CH_SCORE_INTFADJ]);
	printf("\t FCS: %d\n", a_pol->acs_weight[CH_SCORE_FCS]);
	printf("\t TXPWR: %d\n", a_pol->acs_weight[CH_SCORE_TXPWR]);
	printf("\t BGNOISE: %d\n", a_pol->acs_weight[CH_SCORE_BGNOISE]);
	printf("\t CNS: %d\n", a_pol->acs_weight[CH_SCORE_CNS]);
	printf("\t TXOP: %d\n", a_pol->acs_weight[CH_SCORE_TXOP]);

}
#endif /* DEBUG */

static void
acs_retrieve_config_bgdfs(acs_bgdfs_info_t *acs_bgdfs, char * prefix)
{
	char conf_word[128], tmp[100];

	if (acs_bgdfs == NULL) {
		ACSD_ERROR("acs_bgdfs is NULL");
		return;
	}

	/* acs_bgdfs_ahead */
	acs_safe_get_conf(conf_word, sizeof(conf_word),
		strcat_r(prefix, "acs_bgdfs_ahead", tmp));

	if (!strcmp(conf_word, "")) {
		ACSD_INFO("No acs_bgdfs_ahead is set. Retrieve default.\n");
		acs_bgdfs->ahead = ACS_BGDFS_AHEAD;
	} else {
		char *endptr = NULL;
		acs_bgdfs->ahead = strtoul(conf_word, &endptr, 0);
		ACSD_DEBUG("acs_bgdfs_ahead: 0x%x\n", acs_bgdfs->ahead);
	}

	/* acs_bgdfs_idle_interval */
	acs_safe_get_conf(conf_word, sizeof(conf_word),
		strcat_r(prefix, "acs_bgdfs_idle_interval", tmp));

	if (!strcmp(conf_word, "")) {
		ACSD_INFO("No acs_bgdfs_idle_interval is set. Retrieve default.\n");
		acs_bgdfs->idle_interval = ACS_BGDFS_IDLE_INTERVAL;
	} else {
		char *endptr = NULL;
		acs_bgdfs->idle_interval = strtoul(conf_word, &endptr, 0);
		if (acs_bgdfs->idle_interval < ACS_TRAFFIC_INFO_UPDATE_INTERVAL(acs_bgdfs)) {
			acs_bgdfs->idle_interval = ACS_TRAFFIC_INFO_UPDATE_INTERVAL(acs_bgdfs);
		}
		ACSD_INFO("acs_bgdfs_idle_interval: 0x%x\n", acs_bgdfs->idle_interval);
	}

	/* acs_bgdfs_idle_frames_thld */
	acs_safe_get_conf(conf_word, sizeof(conf_word),
		strcat_r(prefix, "acs_bgdfs_idle_frames_thld", tmp));

	if (!strcmp(conf_word, "")) {
		ACSD_INFO("No acs_bgdfs_idle_frames_thld is set. Retrieve default.\n");
		acs_bgdfs->idle_frames_thld = ACS_BGDFS_IDLE_FRAMES_THLD;
	} else {
		char *endptr = NULL;
		acs_bgdfs->idle_frames_thld = strtoul(conf_word, &endptr, 0);
		ACSD_DEBUG("acs_bgdfs_idle_frames_thld: 0x%x\n", acs_bgdfs->idle_frames_thld);
	}

	/* acs_bgdfs_avoid_on_far_sta */
	acs_safe_get_conf(conf_word, sizeof(conf_word),
		strcat_r(prefix, "acs_bgdfs_avoid_on_far_sta", tmp));

	if (!strcmp(conf_word, "")) {
		ACSD_INFO("No acs_bgdfs_avoid_on_far_sta is set. Retrieve default.\n");
		acs_bgdfs->bgdfs_avoid_on_far_sta = ACS_BGDFS_AVOID_ON_FAR_STA;
	} else {
		char *endptr = NULL;
		acs_bgdfs->bgdfs_avoid_on_far_sta = strtoul(conf_word, &endptr, 0);
		ACSD_DEBUG("acs_bgdfs_avoid_on_far_sta: 0x%x\n", acs_bgdfs->bgdfs_avoid_on_far_sta);
	}

	/* acs_bgdfs_fallback_blocking_cac */
	acs_safe_get_conf(conf_word, sizeof(conf_word),
		strcat_r(prefix, "acs_bgdfs_fallback_blocking_cac", tmp));

	if (!strcmp(conf_word, "")) {
		ACSD_INFO("No acs_bgdfs_fallback_blocking_cac set. Get default.\n");
		acs_bgdfs->fallback_blocking_cac = ACS_BGDFS_FALLBACK_BLOCKING_CAC;
	} else {
		char *endptr = NULL;
		acs_bgdfs->fallback_blocking_cac = strtoul(conf_word, &endptr, 0);
		ACSD_DEBUG("acs_bgdfs_fallback_blocking_cac: 0x%x\n",
				acs_bgdfs->fallback_blocking_cac);
	}

	/* acs_bgdfs_txblank_threshold */
	acs_safe_get_conf(conf_word, sizeof(conf_word),
		strcat_r(prefix, "acs_bgdfs_txblank_threshold", tmp));

	if (!strcmp(conf_word, "")) {
		ACSD_INFO("No acs_bgdfs_txblank_threshold set. Get default.\n");
		acs_bgdfs->txblank_th = ACS_BGDFS_TX_LOADING;
	} else {
		char *endptr = NULL;
		acs_bgdfs->txblank_th = strtoul(conf_word, &endptr, 0);
		ACSD_DEBUG("acs_bgdfs_txblank_threshold: 0x%x\n", acs_bgdfs->txblank_th);
	}
}

static void
acs_retrieve_config(acs_chaninfo_t *c_info, char * prefix)
{
	/* retrieve policy related configuration from nvram */
	char conf_word[128], conf_var[16], tmp[100];
	char *next;
	int i = 0, val;
	acs_policy_index index;
	acs_policy_t *a_pol = &c_info->acs_policy;
	uint32 flags;
	int acs_bgdfs_enab = 0;

	/* the current layout of config */
	ACSD_INFO("retrieve config from nvram ...\n");

	acs_safe_get_conf(conf_word, sizeof(conf_word),
		strcat_r(prefix, "acs_scan_entry_expire", tmp));

	if (!strcmp(conf_word, "")) {
		ACSD_INFO("No acs_scan_entry_expire set. Retrieve default.\n");
		c_info->acs_scan_entry_expire = ACS_CI_SCAN_EXPIRE;
	}
	else {
		char *endptr = NULL;
		c_info->acs_scan_entry_expire = strtoul(conf_word, &endptr, 0);
		ACSD_DEBUG("acs_scan_entry_expire: 0x%x\n", c_info->acs_scan_entry_expire);
	}

	acs_safe_get_conf(conf_word, sizeof(conf_word),
		strcat_r(prefix, "acs_fcs_mode", tmp));

	if (!strcmp(conf_word, "")) {
		ACSD_INFO("No acs_fcs_mode is set. Retrieve default.\n");
		c_info->acs_fcs_mode = ACS_FCS_MODE_DEFAULT;
	}
	else {
		char *endptr = NULL;
		c_info->acs_fcs_mode = strtoul(conf_word, &endptr, 0);
		ACSD_DEBUG("acs_fcs_mode: 0x%x\n", c_info->acs_fcs_mode);
	}

	acs_safe_get_conf(conf_word, sizeof(conf_word),
		strcat_r(prefix, "acsd_scs_dfs_scan", tmp));

	if ((!strcmp(conf_word, "")) || c_info->acs_fcs_mode) {
		ACSD_INFO("No acsd_scs_dfs_scan retrieve default.\n");
		c_info->acsd_scs_dfs_scan = ACSD_SCS_DFS_SCAN_DEFAULT;
	}
	else {
		char *endptr = NULL;
		c_info->acsd_scs_dfs_scan = strtoul(conf_word, &endptr, 0);
		ACSD_DEBUG("acsd_scs_dfs_scan: 0x%x\n", c_info->acsd_scs_dfs_scan);
	}

	/* acs_bgdfs_enab */
	acs_safe_get_conf(conf_word, sizeof(conf_word),
		strcat_r(prefix, "acs_bgdfs_enab", tmp));

	if (!strcmp(conf_word, "")) {
		ACSD_INFO("No acs_bgdfs_enab is set. Retrieve default.\n");
		acs_bgdfs_enab = ACS_BGDFS_ENAB;
	}
	else {
		char *endptr = NULL;
		acs_bgdfs_enab = strtoul(conf_word, &endptr, 0);
		ACSD_DEBUG("acs_bgdfs_enab: 0x%x\n", acs_bgdfs_enab);
	}

	if (acs_bgdfs_enab) {
		/* allocate core data structure for bgdfs */
		c_info->acs_bgdfs =
			(acs_bgdfs_info_t *)acsd_malloc(sizeof(*(c_info->acs_bgdfs)));

		acs_retrieve_config_bgdfs(c_info->acs_bgdfs, prefix);
	}

	acs_safe_get_conf(conf_word, sizeof(conf_word),
		strcat_r(prefix, "acs_boot_only", tmp));

	if (!strcmp(conf_word, "")) {
		ACSD_INFO("No acs_boot_only is set. Retrieve default. \n");
		c_info->acs_boot_only = ACS_BOOT_ONLY_DEFAULT;
	} else {
		char *endptr = NULL;
		c_info->acs_boot_only = strtoul(conf_word, &endptr, 0);
		ACSD_DEBUG("acs_boot_only: 0x%x\n", c_info->acs_boot_only);
	}

	acs_safe_get_conf(conf_word, sizeof(conf_word),
		strcat_r(prefix, "acs_flags", tmp));

	if (!strcmp(conf_word, "")) {
		ACSD_INFO("No acs flag set. Retrieve default.\n");
		flags = ACS_DFLT_FLAGS;
	}
	else {
		char *endptr = NULL;
		flags = strtoul(conf_word, &endptr, 0);
		ACSD_DEBUG("acs flags: 0x%x\n", flags);
	}

	acs_safe_get_conf(conf_word, sizeof(conf_word),
		strcat_r(prefix, "acs_pol", tmp));

	if (!strcmp(conf_word, "")) {
		ACSD_INFO("No acs policy set. Retrieve default.\n");

		acs_safe_get_conf(conf_word, sizeof(conf_word),
			strcat_r(prefix, "acs_pol_idx", tmp));

		if (!strcmp(conf_word, "")) {
			if (ACS_FCS_MODE(c_info)) {
				index = ACS_POLICY_FCS;
			} else {
				index = ACS_POLICY_DEFAULT;
			}
		} else {
			index = atoi(conf_word);
		}
		acs_default_policy(a_pol, index);

	} else {

		index = ACS_POLICY_USER;
		memset(a_pol, 0, sizeof(*a_pol));	/* Initialise policy values to all zeroes */
		foreach(conf_var, conf_word, next) {
			val = atoi(conf_var);
			ACSD_DEBUG("i: %d conf_var: %s val: %d\n", i, conf_var, val);

			if (i == 0)
				a_pol->bgnoise_thres = val;
			else if (i == 1)
				a_pol->intf_threshold = val;
			else {
				if ((i - 2) >= CH_SCORE_MAX) {
					ACSD_ERROR("Ignoring excess values in %sacs_pol=\"%s\"\n",
						prefix, conf_word);
					break; /* Prevent overwriting innocent memory */
				}
				a_pol->acs_weight[i - 2] = val;
				ACSD_DEBUG("weight No. %d, value: %d\n", i-2, val);
			}
			i++;
		}
		a_pol->chan_selector = acs_pick_chanspec;
	}

	acs_fcs_retrieve_config(c_info, prefix);

	acs_bgdfs_acs_toa_retrieve_config(c_info, prefix);

	/* Customer Knob #1
	 * Preference for DFS and Non-DFS channels
	 */
	acs_safe_get_conf(conf_word, sizeof(conf_word),
			strcat_r(prefix, "acs_cs_dfs_pref", tmp));

	if (!strcmp(conf_word, "")) {
		ACSD_INFO("No acs_cs_dfs_pref set. Use val of acs_dfs instead\n");
		c_info->acs_cs_dfs_pref = c_info->acs_fcs.acs_dfs;
	}
	else {
		char *endptr = NULL;
		c_info->acs_cs_dfs_pref = strtoul(conf_word, &endptr, 0);
		ACSD_DEBUG("acs_cs_dfs_pref: 0x%x\n", c_info->acs_cs_dfs_pref);
	}

	/* Customer Knob #2
	 * Preference for channel power
	 */
	acs_safe_get_conf(conf_word, sizeof(conf_word),
			strcat_r(prefix, "acs_cs_high_pwr_pref", tmp));

	if (!strcmp(conf_word, "")) {
		ACSD_INFO("No acs_cs_high_pwr_pref set. Disabled by default.\n");
		c_info->acs_cs_high_pwr_pref = 0;
	}
	else {
		char *endptr = NULL;
		c_info->acs_cs_high_pwr_pref = strtoul(conf_word, &endptr, 0);
		ACSD_DEBUG("acs_cs_high_pwr_pref: 0x%x\n", c_info->acs_cs_high_pwr_pref);
	}

	/* allocate core data structure for escan */
	c_info->acs_escan =
		(acs_escaninfo_t *)acsd_malloc(sizeof(*(c_info->acs_escan)));

	acs_safe_get_conf(conf_word, sizeof(conf_word),
			strcat_r(prefix, "acs_use_escan", tmp));

	if (!strcmp(conf_word, "")) {
		ACSD_INFO("No escan config set. use defaults\n");
		c_info->acs_escan->acs_use_escan = ACS_ESCAN_DEFAULT;
	}
	else {
		char *endptr = NULL;
		c_info->acs_escan->acs_use_escan = strtoul(conf_word, &endptr, 0);
		ACSD_DEBUG("acs escan enable: %d\n", c_info->acs_escan->acs_use_escan);
	}

	c_info->flags = flags;
	c_info->policy_index = index;
#ifdef DEBUG
	acs_dump_policy(a_pol);
#endif // endif
}

/* look for str in capability (wl cap) and return true if found */
static bool
acs_check_cap(acs_chaninfo_t *c_info, char *str)
{
	char data_buf[WLC_IOCTL_MAXLEN];
	uint32 ret, param = 0;

	if (str == NULL || strlen(str) >= WLC_IOCTL_SMLEN) {
		ACSD_ERROR("%s invalid needle to look for in cap\n", c_info->name);
		return FALSE;
	}

	ret = wl_iovar_getbuf(c_info->name, "cap", &param, sizeof(param),
			data_buf, sizeof(data_buf));

	if (ret != BCME_OK) {
		ACSD_ERROR("%s Error %d in getting cap\n", c_info->name, ret);
		return FALSE;
	}

	data_buf[WLC_IOCTL_MAXLEN - 1] = '\0';
	if (strstr(data_buf, str) == NULL) {
		ACSD_INFO("%s '%s' not found in cap\n", c_info->name, str);
		return FALSE;
	} else {
		ACSD_INFO("%s '%s' found in cap\n", c_info->name, str);
		return TRUE;
	}
}

static int
acs_start(char *name, acs_chaninfo_t *c_info)
{
	int unit;
	char prefix[PREFIX_LEN], tmp[100];
	acs_rsi_t* rsi;
	int ret = 0;

	ACSD_INFO("acs_start for interface %s\n", name);

#ifdef DSLCPE_ENDIAN
	wl_endian_probe(name);
	c_info->endian_swap = gg_swap;
#endif

	ret = wl_ioctl(name, WLC_GET_INSTANCE, &unit, sizeof(unit));
#ifdef DSLCPE_ENDIAN
	unit = dtoh32(unit);
#endif
	snprintf(prefix, sizeof(prefix), "wl%d_", unit);

	/* check radio */
	if (nvram_match(strcat_r(prefix, "radio", tmp), "0")) {
		ACSD_INFO("ifname %s: radio is off\n", name);
		c_info->mode = ACS_MODE_DISABLE;
		goto acs_start_done;
	}

	acs_retrieve_config(c_info, prefix);

	if ((ret = acs_get_country(c_info)) != BCME_OK)
		ACSD_ERROR("Failed to get country info\n");

	rsi = &c_info->rs_info;
	acs_get_rs_info(c_info, prefix);

	if (rsi->pref_chspec == 0) {
		c_info->mode = ACS_MODE_SELECT;
	}
	else if (rsi->coex_enb &&
		nvram_match(strcat_r(prefix, "nmode", tmp), "-1")) {
		c_info->mode = ACS_MODE_COEXCHECK;
	}
	else
		c_info->mode = ACS_MODE_MONITOR; /* default mode */

	if ((c_info->mode == ACS_MODE_SELECT) && BAND_5G(rsi->band_type) &&
		(nvram_match(strcat_r(prefix, "reg_mode", tmp), "h") ||
		nvram_match(strcat_r(prefix, "reg_mode", tmp), "strict_h"))) {
		rsi->reg_11h = TRUE;
	}

	ret = acsd_chanim_init(c_info);
	if (ret < 0)
		ACS_FREE(c_info);
	ACS_ERR(ret, "chanim init failed\n");

	if (AUTOCHANNEL(c_info) && ACS_FCS_MODE(c_info))
		acs_intfer_config(c_info);

	/* Do not even allocate a DFS Reentry context on 2.4GHz which does not have DFS channels */
	/* or if 802.11h spectrum management is not enabled. */
	if (BAND_2G(c_info->rs_info.band_type) || (rsi->reg_11h == FALSE)) {
		ACSD_DFSR("DFS Reentry disabled %s\n", (BAND_2G(c_info->rs_info.band_type)) ?
			"on 2.4GHz band" : "as 802.11h is not enabled");
		c_info->acs_fcs.acs_dfs = ACS_DFS_DISABLED;
	} else {
		ACS_DFSR_CTX(c_info) = acs_dfsr_init(prefix,
			(ACS_FCS_MODE(c_info) && (c_info->acs_fcs.acs_dfs == ACS_DFS_REENTRY)), c_info->acs_bgdfs);
		ret = (ACS_DFSR_CTX(c_info) == NULL) ? -1 : 0;
		ACS_ERR(ret, "Failed to allocate DFS Reentry context\n");
	}

	/* When acsd starts, retrieve current traffic stats since boot */
	acs_get_initial_traffic_stats(c_info);

	if (!AUTOCHANNEL(c_info) && !COEXCHECK(c_info))
		goto acs_start_done;

	c_info->dyn160_cap = acs_check_cap(c_info, ACS_CAP_STRING_DYN160);
	if (c_info->dyn160_cap) {
		acs_update_dyn160_status(c_info);
	}

	ret = acs_run_cs_scan(c_info);
	ACS_ERR(ret, "cs scan failed\n");

	ACS_FREE(c_info->acs_bss_info_q);

	ret = acs_request_data(c_info);
	ACS_ERR(ret, "request data failed\n");

acs_start_done:
	return ret;
}

static int
acs_check_bss_is_enabled(char *name, acs_chaninfo_t **c_info_ptr, char *prefix)
{
	int index, ret;
	char buf[32] = { 0 }, *bss_check;

	if (strlen(name) >= sizeof((*c_info_ptr)->name)) {
		ACSD_ERROR("Interface Name Length Exceeded\n");
		return BCME_STRLEN;
	}

	if (prefix == NULL || prefix[0] == '\0') {
		strcat_r(name, "_bss_enabled", buf);
	} else {
		strcat_r(prefix, "_bss_enabled", buf);
	}

	bss_check = nvram_safe_get(buf);
	if (atoi(bss_check) != 1) { /* this interface is disabled */
		ACSD_INFO("interface is disabled %s\n",name);
		return BCME_DISABLED;
	}

	acs_add_map(name);
	index = acs_idx_from_map(name);

	if (index < 0) {
		ret = ACSD_FAIL;
		ACS_ERR(ret, "Mapped entry not present for interface");
	}

	/* allocate core data structure for this interface */
	*c_info_ptr = acs_info->chan_info[index] =
		(acs_chaninfo_t*)acsd_malloc(sizeof(acs_chaninfo_t));
	strncpy((*c_info_ptr)->name, name, sizeof((*c_info_ptr)->name));
	(*c_info_ptr)->name[sizeof((*c_info_ptr)->name) - 1] = '\0';
	ACSD_INFO("bss enabled for name :%s\n", (*c_info_ptr)->name);
	return BCME_OK;
}

static int
acs_build_candidates(acs_chaninfo_t *c_info, int bw)
{
	wl_uint32_list_t *list;
	chanspec_t input = 0, c = 0;
	int ret = 0, i, j;
	int count = 0;
	ch_candidate_t *candi;
	acs_rsi_t *rsi = &c_info->rs_info;

	char *data_buf;
	data_buf = acsd_malloc(ACS_SM_BUF_LEN);

	if (bw == ACS_BW_160) {
		input |= WL_CHANSPEC_BW_160;
	} else if (bw == ACS_BW_8080) {
		input |= WL_CHANSPEC_BW_8080;
	} else if (bw == ACS_BW_80) {
		input |= WL_CHANSPEC_BW_80;
	} else if (bw == ACS_BW_40) {
		input |= WL_CHANSPEC_BW_40;
	} else {
		input |= WL_CHANSPEC_BW_20;
	}

	if (BAND_5G(rsi->band_type)) {
		input |= WL_CHANSPEC_BAND_5G;
	} else {
		input |= WL_CHANSPEC_BAND_2G;

	}

#ifdef DSLCPE_ENDIAN
	input = htod16(input);
#endif
	ret = wl_iovar_getbuf(c_info->name, "chanspecs", &input, sizeof(chanspec_t),
		data_buf, ACS_SM_BUF_LEN);
	if (ret < 0)
		ACS_FREE(data_buf);
	ACS_ERR(ret, "failed to get valid chanspec lists");

	list = (wl_uint32_list_t *)data_buf;
	count = dtoh32(list->count);

	if (!count) {
		ACSD_ERROR("number of valid chanspec is 0\n");
		ret = -1;
		goto cleanup;
	}

	ACS_FREE(c_info->candidate[bw]);
	c_info->candidate[bw] = (ch_candidate_t*)acsd_malloc(count * sizeof(ch_candidate_t));
	candi = c_info->candidate[bw];

	ACSD_DEBUG("address of candi: 0x%x\n", (uint32)candi);
	for (i = 0; i < count; i++) {
		c = (chanspec_t)dtoh32(list->element[i]);
		candi[i].chspec = c;
		candi[i].valid = TRUE;

		if (acs_is_dfs_chanspec(c_info, candi[i].chspec)) {
			candi[i].is_dfs = TRUE;
			if (!rsi->reg_11h) {
				/* DFS Channels can be used only if 802.11h is enabled */
				candi[i].valid = FALSE;
				candi[i].reason |= ACS_INVALID_DFS_NO_11H;
			}
		} else {
			candi[i].is_dfs = FALSE;
		}

		/* assign weight based on config */
		for (j = 0; j < CH_SCORE_MAX; j++) {
			candi[i].chscore[j].weight = c_info->acs_policy.acs_weight[j];
			ACSD_DEBUG("chanspec: (0x%04x) score: %d, weight: %d\n",
				c, candi[i].chscore[j].score, candi[i].chscore[j].weight);
		}
	}
	c_info->c_count[bw] = count;

cleanup:
	free(data_buf);
	return ret;
}

/*
 * 20/40 Coex compliance check:
 * Return a 20/40 Coex compatible chanspec based on the scan data.
 * Verify that the 40MHz input_chspec passes 20/40 Coex rules.
 * If so, return the same chanspec.
 * Otherwise return a 20MHz chanspec which is centered on the
 * input_chspec's control channel.
 */
static chanspec_t
acs_coex_check(acs_chaninfo_t* c_info, chanspec_t input_chspec)
{
	int forty_center;
	uint ctrl_ch, ext_ch;
	acs_channel_t chan;
	chanspec_t chspec_out;
	int ci_index, ninfo = c_info->scan_chspec_list.count;
	bool conflict = FALSE;
	acs_chan_bssinfo_t *ci = c_info->ch_bssinfo;
	char err_msg[128];

	if (!CHSPEC_IS40(input_chspec))
		ACSD_ERROR("input channel spec is not 40MHz!");

	/* this will get us the center of the input 40MHz channel */
	forty_center = CHSPEC_CHANNEL(input_chspec);

	acs_parse_chanspec(input_chspec, &chan);

	ctrl_ch = chan.control;
	ext_ch = chan.ext20;

	ACSD_DEBUG("InputChanspec:  40Center %d, CtrlCenter %d, ExtCenter %d\n",
	          forty_center, ctrl_ch, ext_ch);

	/* Loop over scan data looking for interferance based on 20/40 Coex Rules. */
	for (ci_index = 0; ci_index < ninfo; ci_index++) {
		ACSD_DEBUG("Examining ci[%d].channel = %d, forty_center-5 = %d, "
		          "forty_center+5 = %d\n",
		          ci_index, ci[ci_index].channel, forty_center - WLC_2G_25MHZ_OFFSET,
		          forty_center+WLC_2G_25MHZ_OFFSET);

		/* Ignore any channels not within the range we care about.
		 * 20/40 Coex rules for 2.4GHz:
		 * Must look at all channels where a 20MHz BSS would overlap with our
		 * 40MHz BW + 5MHz on each side.  This means that we must inspect any channel
		 * within 5 5MHz channels of the center of our 40MHz chanspec.
		 *
		 * Example:
		 * 40MHz Chanspec centered on Ch.8
		 *              +5 ----------40MHz-------------  +5
		 *              |  |           |              |   |
		 * -1  0  1  2  3  4  5  6  7  8  9  10  11  12  13  14
		 *
		 * Existing 20MHz BSS on Ch. 1 (Doesn't interfere with our 40MHz AP)
		 *  -----20MHz---
		 *  |     |     |
		 * -1  0  1  2  3  4  5  6  7  8  9  10  11  12  13  14
		 *
		 * Existing 20MHz BSS on Ch. 3 (Does interfere our 40MHz AP)
		 *        -----20MHz---
		 *        |     |     |
		 * -1  0  1  2  3  4  5  6  7  8  9  10  11  12  13  14
		 *
		 *  In this example, we only pay attention to channels in the range of 3 thru 13.
		 */

		if (ci[ci_index].channel < forty_center - WLC_2G_25MHZ_OFFSET ||
		    ci[ci_index].channel > forty_center + WLC_2G_25MHZ_OFFSET) {
			ACSD_DEBUG("Not in range, continue.\n");
			continue;
		}

		ACSD_DEBUG("In range.\n");
		if (ci[ci_index].nCtrl || ci[ci_index].nExt20) {
			/* Is there an existing BSS? */
			ACSD_DEBUG("Existing BSSs on channel %d\n", ci[ci_index].channel);

			/* Existing BSS is ONLY okay if:
			 * Our control channel is aligned with existing 20 or Control Channel
			 * Our extension channel is aligned with an existing extension channel
			 */
			if (ci[ci_index].channel == ctrl_ch) {
				ACSD_DEBUG("Examining ctrl_ch\n");

				/* Two problems that we need to detect here:
				 *
				 * 1:  If this channel is being used as a 40MHz extension.
				 * 2:  If this channel is being used as a control channel for an
				 *     existing 40MHz, we must both use the same CTRL sideband
				 */

				if (ci[ci_index].nExt20) {
					snprintf(err_msg, sizeof(err_msg), "ctrl channel: %d"
							" existing ext. channel", ctrl_ch);
					conflict = TRUE;
					break;
				} else if (ci[ci_index].nCtrl) {
					snprintf(err_msg, sizeof(err_msg), "ctrl channel %d"
							" SB not aligned with existing 40BSS", ctrl_ch);
					conflict = TRUE;
					break;
				}
			} else if (ci[ci_index].channel == ext_ch) {
				ACSD_DEBUG("Examining ext_ch\n");

				/* Any BSS using this as it's center is an interference */
				if (ci[ci_index].nCtrl) {
					snprintf(err_msg, sizeof(err_msg), "ext channel %d"
							" used as ctrl channel by existing BSSs", ext_ch);
					conflict = TRUE;
					break;
				}
			} else {
				/* If anyone is using this channel, it's an conflict */
				conflict = TRUE;
				snprintf(err_msg, sizeof(err_msg),
						"channel %d used by exiting BSSs ", ci[ci_index].channel);
				break;
			}
		}
	}

	if (conflict) {
		chspec_out = CH20MHZ_CHSPEC(ctrl_ch);
		if (c_info->rs_info.pref_chspec)
			ACSD_PRINT("COEX: downgraded chanspec 0x%x to 0x%x: %s\n",
				input_chspec, chspec_out, err_msg);
	} else {
		chspec_out = input_chspec;
		ACSD_DEBUG("No conflict found, returning 40MHz chanspec 0x%x\n",
		          chspec_out);
	}
	return chspec_out;
}

static bool
acs_has_valid_candidate(acs_chaninfo_t* c_info, int bw)
{
	int i;
	bool result = FALSE;
	ch_candidate_t* candi = c_info->candidate[bw];

	for (i = 0; i < c_info->c_count[bw]; i++) {
		if (candi[i].valid) {
			result = TRUE;
			break;
		}
	}
	ACSD_DEBUG("result: %d\n", result);
	return result;
}

/*
 * Individual scoring algorithm. It is subject to tuning or customization based on
 * testing results or customer requirement.
 *
 */
static int
acs_chan_score_bss(ch_candidate_t* candi, acs_chan_bssinfo_t* bss_info, int ncis)
{
	acs_channel_t chan;
	int score = 0, tmp_score = 0;
	int i, min, max;
	int ch;
	bool ovlp = FALSE;
	int ovlp_offset = 0;

	if (CHSPEC_IS2G(candi->chspec) && (!nvram_match("acs_2g_ch_no_ovlp", "1"))) {
		ovlp = TRUE;
	}

	acs_parse_chanspec(candi->chspec, &chan);

	for (i = 0; i < ncis; i++) {
		ch = (int)bss_info[i].channel;

		/* control channel */
		min = max = (int)chan.control;
		if (ovlp) {
			/* 25 MHz seperation allows nearby channels to be
			   avoided.
			 */
			min -= CH_20MHZ_APART + CH_5MHZ_APART;
			max += CH_20MHZ_APART + CH_5MHZ_APART;
		}

		ACSD_DEBUG("ch: %d, min: %d, max: %d\n", ch, min, max);
		ACSD_DEBUG("nCtrl=%2d nExt20=%2d nExt40=%2d nExt80=%2d\n",
			bss_info[i].nCtrl,bss_info[i].nExt20,bss_info[i].nExt40,
			bss_info[i].nExt80);

		/* ban channel 144 as primary or secondary 20 */
		if (chan.control == 144 || chan.ext20 == 144) {
			candi->valid = FALSE;
			candi->reason = ACS_INVALID_144;
			break;
		}

		if (ch < min || ch > max)
			goto ext20;

		tmp_score = bss_info[i].nExt20;
		if (tmp_score > 0) {
			if ((!CHSPEC_IS20(candi->chspec)) &&
				(!nvram_match("acs_no_restrict_align", "1"))) {
				/* mark this candidate invalid if it is not used or is 80MHz */
				candi->valid = FALSE;
				candi->reason = ACS_INVALID_ALIGN;
				break;
			}
			else
				/* in_use and not 80MHz, allowed */
				score += ACS_NOT_ALIGN_WT * tmp_score;
		}
		if (ovlp) {
			ovlp_offset = ABS(ch - chan.control);
			if (ovlp_offset == 0) {
				score += 4*bss_info[i].nCtrl;
			} else if (ovlp_offset < CH_20MHZ_APART) {
				score += 6*bss_info[i].nCtrl;
			} else {
				/* bias against channels that are not overlapping
				 * but within 30 MHz. e.g. 1 <-> 6,  6<->11
				 */
				score += bss_info[i].nCtrl;
			}
		} else {
			score += bss_info[i].nCtrl;
		}
		score += bss_info[i].nExt40 + bss_info[i].nExt80;

ext20:
		/* this is to calculate bss score for ext20 channel */
		if (chan.ext20 == 0)
			continue;

		min = max = (int)chan.ext20;
		if (ovlp) {
			min -= CH_10MHZ_APART;
			max += CH_10MHZ_APART;
		}

		if (ch < min || ch > max)
			goto ext40;
		tmp_score = bss_info[i].nCtrl;

		if (tmp_score > 0) {
			if ((!CHSPEC_IS20(candi->chspec)) &&
				(!nvram_match("acs_no_restrict_align", "1"))) {
				/* mark this candidate invalid if it is not used or is 80MHz */
				candi->valid = FALSE;
				candi->reason = ACS_INVALID_ALIGN;
				break;
			}
			else
				/* in_use and not 80MHz, allowed */
				score += ACS_NOT_ALIGN_WT * tmp_score;
		}

		score += bss_info[i].nExt20 + bss_info[i].nExt40;
		score += bss_info[i].nExt80;

ext40:
		/* to calculate bss scroe for ext40 channel */
		if (chan.ext40[0] == 0) {
			continue;
		}

		/* no ovlp in 5G */
		if (ch != chan.ext40[0] && ch != chan.ext40[1]) {
			goto ext80;
		}

		score += bss_info[i].nCtrl;
		score += bss_info[i].nExt20 + bss_info[i].nExt40 +
			bss_info[i].nExt80;

ext80:
		/* to calculate bss score for ext80 channel */
		if (chan.ext80[0] == 0) {
			continue;
		}

		/* no ovlp in 5G */
		if (ch != chan.ext80[0] && ch != chan.ext80[1] &&
			ch != chan.ext80[2] && ch != chan.ext80[3]) {
			continue;
		}

		score += bss_info[i].nCtrl;
		score += bss_info[i].nExt20 + bss_info[i].nExt40 +
			bss_info[i].nExt80;

		ACSD_DEBUG("i: %d, score: %d\n", i, score);
	}
	ACSD_INFO("candidate: %x, score_bss: %d\n", candi->chspec, score);
	return score;
}

static void
acs_candidate_score_bss(ch_candidate_t *candi, acs_chaninfo_t* c_info)
{
	acs_chan_bssinfo_t* bss_info = c_info->ch_bssinfo;
	int score = 0;
	int ncis = c_info->scan_chspec_list.count;

	score = acs_chan_score_bss(candi, bss_info, ncis);
	candi->chscore[CH_SCORE_BSS].score = score;
	ACSD_DEBUG("bss score: %d for chanspec 0x%x\n", score, candi->chspec);
}

static void
acs_candidate_score_busy(ch_candidate_t *candi, acs_chaninfo_t* c_info)
{
	wl_chanim_stats_t *ch_stats = c_info->chanim_stats;
	chanim_stats_t *stats;
	chanspec_t chspec = candi->chspec;
	acs_channel_t chan;
	int i, ch, score = 0, hits = 0;
	bool done = FALSE;

	acs_parse_chanspec(candi->chspec, &chan);

	for (i = 0; i < ch_stats->count; i++) {
		stats = &ch_stats->stats[i];
		ch = CHSPEC_CHANNEL(stats->chanspec);

		if ((ch == chan.control) || (ch == chan.ext20)) {
			score += stats->ccastats[CCASTATS_OBSS];
			hits ++;
		}

		if (hits >= 1 || (hits && chan.control == chan.ext20)) {
			done = TRUE;
		}
	}
	if (!done) {
		ACSD_ERROR("busy check failed for chanspec: 0x%x\n", chspec);
		return;
	}

	if (hits)
		candi->chscore[CH_SCORE_BUSY].score = score/hits;
	ACSD_DEBUG("busy score: %d for chanspec 0x%x\n", score, chspec);
}

static void
acs_candidate_score_intf(ch_candidate_t *candi, acs_chaninfo_t* c_info)
{
	wl_chanim_stats_t *ch_stats = c_info->chanim_stats;
	chanim_stats_t *stats;
	chanspec_t chspec = candi->chspec;
	acs_channel_t chan;
	int i, ch, score = 0, hits = 0;
	bool done = FALSE;

	acs_parse_chanspec(chspec, &chan);

	for (i = 0; i < ch_stats->count; i++) {
		stats = &ch_stats->stats[i];
		ch = CHSPEC_CHANNEL(stats->chanspec);

		if (ch == chan.control || ch == chan.ext20) {
			score += stats->ccastats[CCASTATS_NOPKT];
			hits ++;
		}

		if (hits >= 1 || (hits && chan.control == chan.ext20)) {
			done = TRUE;
		}
	}
	if (!done) {
		ACSD_ERROR("intf check failed for chanspec: 0x%x\n", chspec);
		return;
	}

	if (hits)
		candi->chscore[CH_SCORE_INTF].score = score/hits;
	ACSD_DEBUG("intf score: %d for chanspec 0x%x\n", score, chspec);
}

static void
acs_candidate_score_intfadj(ch_candidate_t *candi, acs_chaninfo_t* c_info)
{
	wl_chanim_stats_t *ch_stats = c_info->chanim_stats;
	chanim_stats_t *stats;
	chanspec_t chspec = candi->chspec;
	acs_channel_t chan;
	int i, ch, score = 0;
	int dist, d_weight = 10;

	acs_parse_chanspec(chspec, &chan);

	for (i = 0; i < ch_stats->count; i++) {
		stats = &ch_stats->stats[i];
		ch = CHSPEC_CHANNEL(stats->chanspec);
		ACSD_DEBUG("channel: %d, ch: %d\n", chan.control, ch);

		if (ch != chan.control) {
			dist = ch - chan.control;
			dist = dist > 0 ? dist : dist * -1;
			score += stats->ccastats[CCASTATS_NOPKT] * d_weight / dist;

			ACSD_DEBUG("dist: %d, count: %d, score: %d\n",
				dist, stats->ccastats[CCASTATS_NOPKT], score);
			if (chan.ext20 != 0 && ch != chan.ext20) {
				dist = ABS(ch - chan.ext20);
				score += stats->ccastats[CCASTATS_NOPKT] * d_weight / dist;
			}

			ACSD_DEBUG("channel: %d, ch: %d score: %d\n", chan.control, ch, score);
		}
	}

	candi->chscore[CH_SCORE_INTFADJ].score = score / d_weight;
	ACSD_DEBUG("intf_adj score: %d for chanspec 0x%x\n", score, chspec);
}

static void
acs_candidate_score_fcs(ch_candidate_t *candi, acs_chaninfo_t* c_info)
{
	wl_chanim_stats_t *ch_stats = c_info->chanim_stats;
	chanim_stats_t *stats;
	chanspec_t chspec = candi->chspec;
	acs_channel_t chan;
	int i, ch, score = 0, hits = 0;
	bool done = FALSE;

	acs_parse_chanspec(chspec, &chan);

	for (i = 0; i < ch_stats->count; i++) {
		stats = &ch_stats->stats[i];
		ch = CHSPEC_CHANNEL(stats->chanspec);

		if (ch == chan.control || ch == chan.ext20) {
			score += stats->ccastats[CCASTATS_NOCTG];
			hits ++;
		}
		if (hits == 2 || (hits && (chan.ext20 == 0))) {
			done = TRUE;
			break;
		}
	}
	if (!done) {
		ACSD_ERROR("fcs check failed for chanspec: 0x%x\n", chspec);
		return;
	}

	if (hits)
		candi->chscore[CH_SCORE_FCS].score = score/hits;
	ACSD_DEBUG("fcs score: %d for chanspec 0x%x\n", score, chspec);
}

static void
acs_candidate_score_txpwr(ch_candidate_t *candi, acs_chaninfo_t* c_info)
{
	/* TBD */
}

static void
acs_candidate_score_bgnoise(ch_candidate_t *candi, acs_chaninfo_t* c_info)
{
	wl_chanim_stats_t *ch_stats = c_info->chanim_stats;
	chanim_stats_t *stats;
	chanspec_t chspec = candi->chspec;
	acs_channel_t chan;
	int i, ch, score = 0, hits = 0;
	bool done = FALSE;

	acs_parse_chanspec(chspec, &chan);

	for (i = 0; i < ch_stats->count; i++) {
		stats = &ch_stats->stats[i];
		ch = CHSPEC_CHANNEL(stats->chanspec);

		if (ch == chan.control || ch == chan.ext20) {
			if (stats->bgnoise && stats->bgnoise > ACS_BGNOISE_BASE) {
				score += MIN(10, stats->bgnoise - ACS_BGNOISE_BASE);
			}
			hits ++;
		}
		if (hits == 2 || (hits && (chan.ext20 == 0))) {
			done = TRUE;
			break;
		}
	}
	if (!done) {
		ACSD_ERROR("bgnoise check failed for chanspec: 0x%x\n", chspec);
		return;
	}
	if (hits)
		candi->chscore[CH_SCORE_BGNOISE].score = score/hits;
	ACSD_DEBUG("bgnoise score: %d for chanspec 0x%x\n", score, chspec);
}

static void
acs_candidate_score_total(ch_candidate_t *candi, acs_chaninfo_t* c_info)
{
	int i, total;
	ch_score_t *score_p;
	acs_fcs_t *fcs_info = &c_info->acs_fcs;

	total = 0;
	score_p = candi->chscore;

	for (i = 0; i < CH_SCORE_TOTAL; i++)
		total += score_p[i].score * score_p[i].weight;

	if (ACS_FCS_MODE(c_info) && fcs_info->txop_weight) {
		total += score_p[CH_SCORE_TXOP].score * fcs_info->txop_weight;
	} else {
		total += score_p[CH_SCORE_TXOP].score * score_p[CH_SCORE_TXOP].weight;
	}
	score_p[CH_SCORE_TOTAL].score = total;
}

static void
acs_candidate_score_cns(ch_candidate_t *candi, acs_chaninfo_t* c_info)
{
	wl_chanim_stats_t *ch_stats = c_info->chanim_stats;
	chanim_stats_t *stats;
	chanspec_t chspec = candi->chspec;
	acs_channel_t chan;
	int i, ch, score = 0, max_score = -200, hits = 0;

	/*
	 * Check ch_stats to handle rare crash occurence due to ch_stats being NULL.
	 */
	if (!ch_stats) {
		ACSD_ERROR("No chanim_stats available to calculate CNS scores\n");
		return;
	}

	acs_parse_chanspec(chspec, &chan);

	for (i = 0; i < ch_stats->count; i++) {
		stats = &ch_stats->stats[i];
		ch = CHSPEC_CHANNEL(stats->chanspec);

		if (ch == chan.control || ch == chan.ext20 || ch == chan.ext40[0] ||
			ch == chan.ext40[1] || ch == chan.ext80[0] || ch == chan.ext80[1] ||
			ch == chan.ext80[2] || ch == chan.ext80[3]) {
			score = stats->bgnoise;
			score += chanim_txop_to_noise(stats->chan_idle);
			hits ++;
			max_score = MAX(max_score, score);
		}
		if ((hits == 8) || (hits == 4 && !chan.ext80[0]) ||
				(hits == 2 && !chan.ext40[0]) || (hits && !chan.ext20)) {
			break;
		}
	}

	/*
	 * Calculate the CNS based on the noise on any valid 20MHz subchannel of a 40 or 80MHz
	 * channel. NOTE that setting CNS=0 is a high noise value and effectively invalidates
	 * the channel. Only set CNS=0 if all the 20MHz subchannels are excluded.
	 */
	if (!hits) {
#ifdef DSLCPE
		/* this seems like a right case when in 5G, so disable the warning */
		ACSD_INFO("knoise check failed for chanspec: 0x%x\n", chspec);
#else
		ACSD_ERROR("knoise check failed for chanspec: 0x%x\n", chspec);
#endif
		return;
	}
	candi->chscore[CH_SCORE_CNS].score = max_score;
	ACSD_INFO("Composite Noise Score (CNS): %d for chanspec 0x%x\n",
		max_score, chspec);
}

/* This function is used to determine whether the current channel is an adjacent channel
* to the candidate channel
*/
static bool
acs_check_adjacent_bss(acs_chaninfo_t *c_info, int ch, int channel_lower,
	int channel_higher)
{
	bool ret = FALSE;
	if (BAND_2G(c_info->rs_info.band_type)) {
		if ((ch == (channel_lower + (CH_20MHZ_APART + CH_5MHZ_APART))) ||
			(ch == (channel_lower - (CH_20MHZ_APART + CH_5MHZ_APART)))) {
			ret = TRUE;
		}
	} else {
		if ((ch == (channel_lower - CH_20MHZ_APART)) ||
			(ch == (channel_higher + CH_20MHZ_APART))) {
			ret = TRUE;
		}
	}
	return ret;
}

static void
acs_candidate_adjacent_bss(ch_candidate_t *candi, acs_chaninfo_t* c_info)
{
	acs_chan_bssinfo_t* bss_info = c_info->ch_bssinfo;
	int i, ch, adjbss = 0;
	int ncis = c_info->scan_chspec_list.count;
	acs_channel_t chan;
	bool add_to_bss_check = FALSE;
	uint8 channel_sb_lower, channel_sb_upper;

	acs_parse_chanspec(candi->chspec, &chan);

	if (CHSPEC_IS160(candi->chspec)) {
		channel_sb_lower = LLL_20_SB_160(CHSPEC_CHANNEL(candi->chspec));
		channel_sb_upper = UUU_20_SB_160(CHSPEC_CHANNEL(candi->chspec));
	} else if (CHSPEC_IS8080(candi->chspec)) {
		channel_sb_lower = LLL_20_SB_8080(CHSPEC_CHANNEL(candi->chspec));
		channel_sb_upper = LUU_20_SB_8080(CHSPEC_CHANNEL(candi->chspec));
	} else if (CHSPEC_IS80(candi->chspec)) {
		channel_sb_lower = LL_20_SB(CHSPEC_CHANNEL(candi->chspec));
		channel_sb_upper = UU_20_SB(CHSPEC_CHANNEL(candi->chspec));
	} else if (CHSPEC_IS40(candi->chspec)) {
		channel_sb_lower = LOWER_20_SB(CHSPEC_CHANNEL(candi->chspec));
		channel_sb_upper = UPPER_20_SB(CHSPEC_CHANNEL(candi->chspec));
	} else {
		channel_sb_lower = channel_sb_upper = CHSPEC_CHANNEL(candi->chspec);
	}

	for (i = 0; i < ncis; i++) {
		ch = (int)bss_info[i].channel;
		if (BAND_2G(c_info->rs_info.band_type)) {
			if (CHSPEC_SB_LOWER(candi->chspec)) {
				add_to_bss_check =
					acs_check_adjacent_bss(c_info, ch, channel_sb_lower, 0);
			} else {
				add_to_bss_check =
					acs_check_adjacent_bss(c_info, ch, channel_sb_upper, 0);
			}
		} else {
			add_to_bss_check = acs_check_adjacent_bss(c_info, ch,
				channel_sb_lower, channel_sb_upper);
			if (CHSPEC_IS8080(candi->chspec)) {
				uint8 chan2_sb_lower, chan2_sb_upper;

				chan2_sb_lower = ULL_20_SB_8080(
						CHSPEC_CHANNEL(candi->chspec));
				chan2_sb_upper = UUU_20_SB_8080(
						CHSPEC_CHANNEL(candi->chspec));
				add_to_bss_check |= acs_check_adjacent_bss(
						c_info, ch, chan2_sb_lower, chan2_sb_upper);
			}
		}

		if (add_to_bss_check) {
			adjbss += bss_info[i].nCtrl;
			adjbss += bss_info[i].nExt20 + bss_info[i].nExt40 +
				bss_info[i].nExt80;
		}
	}

	candi->chscore[CH_SCORE_ADJ].score = adjbss;
	ACSD_FCS("adjacent bss score: %d for chanspec 0x%x\n", adjbss, candi->chspec);
}

static void
acs_candidate_score_txop(ch_candidate_t *candi, acs_chaninfo_t* c_info)
{
	wl_chanim_stats_t *ch_stats = c_info->chanim_stats;
	chanim_stats_t *stats;
	chanspec_t chspec = candi->chspec;
	acs_channel_t chan;
	int i, ch, score = 0, hits = 0;
	bool done = FALSE;

	acs_parse_chanspec(candi->chspec, &chan);

	for (i = 0; i < ch_stats->count; i++) {
		stats = &ch_stats->stats[i];
		ch = CHSPEC_CHANNEL(stats->chanspec);

		if ((ch == chan.control) || (ch == chan.ext20)) {
			/* busy/unable-to-tx time */
			score += 100 - stats->ccastats[CCASTATS_TXOP] -
				stats->ccastats[CCASTATS_INBSS] -
				stats->ccastats[CCASTATS_TXDUR];

			hits ++;
			done = TRUE;
		}
	}
	if (!done) {
		ACSD_ERROR("txop check failed for chanspec: 0x%x\n", chspec);
		return;
	}

	if (hits) {
		candi->chscore[CH_SCORE_TXOP].score = score/hits;
	}
	ACSD_DEBUG("txop score: %d for chanspec 0x%x\n",
		candi->chscore[CH_SCORE_TXOP].score, chspec);
}
static void
acs_candidate_score(acs_chaninfo_t* c_info, int bw)
{
	ch_score_t *score_p;
	ch_candidate_t* candi;
	acs_fcs_t *fcs_info = &c_info->acs_fcs;
	int i;

	for (i = 0; i < c_info->c_count[bw]; i++) {
		candi = &c_info->candidate[bw][i];
		if (!candi->valid)
			continue;
		score_p = candi->chscore;

		ACSD_DEBUG("calc score for candidate chanspec: 0x%x\n",
			candi->chspec);

		/* calculate the score for each factor */
		if (score_p[CH_SCORE_BSS].weight) {
			acs_candidate_score_bss(candi, c_info);
		}

		if (score_p[CH_SCORE_BUSY].weight) {
			acs_candidate_score_busy(candi, c_info);
		}

		if (score_p[CH_SCORE_INTF].weight) {
			acs_candidate_score_intf(candi, c_info);
		}

		if (score_p[CH_SCORE_INTFADJ].weight) {
			acs_candidate_score_intfadj(candi, c_info);
		}

		if (score_p[CH_SCORE_FCS].weight) {
			acs_candidate_score_fcs(candi, c_info);
		}

		if (score_p[CH_SCORE_TXPWR].weight) {
			acs_candidate_score_txpwr(candi, c_info);
		}

		if (score_p[CH_SCORE_BGNOISE].weight) {
			acs_candidate_score_bgnoise(candi, c_info);
		}
		if (score_p[CH_SCORE_TXOP].weight ||
				(ACS_FCS_MODE(c_info) && fcs_info->txop_weight))
			acs_candidate_score_txop(candi, c_info);

		acs_candidate_score_cns(candi, c_info);

		if (score_p[CH_SCORE_ADJ].weight) {
			acs_candidate_adjacent_bss(candi, c_info);
		}

		acs_candidate_score_total(candi, c_info);
#ifdef ACS_DEBUG
		acs_dump_score(score_p);
#endif // endif
		acs_dump_score_csv(candi->chspec,score_p);
	}
}

static void
acs_candidate_check_intf(ch_candidate_t *candi, acs_chaninfo_t* c_info)
{
	wl_chanim_stats_t *ch_stats = c_info->chanim_stats;
	chanim_stats_t *stats;
	chanspec_t chspec = candi->chspec;
	acs_channel_t chan;
	int i, ch, intf = 0, hits = 0;
	bool done = FALSE;

	acs_parse_chanspec(chspec, &chan);
	for (i = 0; i < ch_stats->count; i++) {
		stats = &ch_stats->stats[i];
		ch = CHSPEC_CHANNEL(stats->chanspec);

		if (ch == chan.control || ch == chan.ext20) {
			if (c_info->flags & ACS_FLAGS_INTF_THRES_CCA) {
				intf = stats->ccastats[CCASTATS_NOPKT];
				if (intf > c_info->acs_policy.intf_threshold) {
					candi->valid = FALSE;
					candi->reason |= ACS_INVALID_INTF_CCA;
					break;
				}
			}
			if (c_info->flags & ACS_FLAGS_INTF_THRES_BGN) {
				intf = stats->bgnoise;
				if (intf && intf > c_info->acs_policy.bgnoise_thres) {
					candi->valid = FALSE;
					candi->reason |= ACS_INVALID_INTF_BGN;
					break;
				}
			}
			hits ++;
		}
		if (hits == 2 || (hits && (chan.ext20 == 0))) {
			done = TRUE;
			break;
		}
	}
	if (!done) {
		ACSD_ERROR("intf check failed for chanspec: 0x%x\n", chspec);
		return;
	}
}

static bool
acs_is_initial_selection(acs_chaninfo_t* c_info)
{
	bool initial_selection = FALSE;
	chanim_info_t * ch_info = c_info->chanim_info;
	uint8 cur_idx = chanim_mark(ch_info).record_idx;
	uint8 start_idx;
	chanim_acs_record_t *start_record;

	start_idx = MODSUB(cur_idx, 1, CHANIM_ACS_RECORD);
	start_record = &ch_info->record[start_idx];
	if ((start_idx == CHANIM_ACS_RECORD - 1) && (start_record->timestamp == 0))
		initial_selection = TRUE;

	ACSD_DFSR("Initial selection is %d\n", initial_selection);
	return initial_selection;
}

/*
 * Returns the channel info of the chspec passed (by combining per_chan_info of each 20MHz subband)
 */
uint32
acs_channel_info(acs_chaninfo_t *c_info, chanspec_t chspec)
{
	char resbuf[ACS_PER_CHAN_INFO_BUF_LEN];
	int ret;
	uint8 sub_channel;
	chanspec_t sub_chspec;
	uint32 chinfo = 0, max_inactive = 0, sub_chinfo;
	FOREACH_20_SB(chspec, sub_channel) {
		sub_chspec = (uint16) sub_channel;
#ifdef DSLCPE_ENDIAN
		{
		uint32 sub_chspec_32 = htod32((uint32)sub_channel);
		ret = wl_iovar_getbuf(c_info->name, "per_chan_info", &sub_chspec_32,
			sizeof(uint32), resbuf, ACS_PER_CHAN_INFO_BUF_LEN);
		}
#else
		ret = wl_iovar_getbuf(c_info->name, "per_chan_info", &sub_chspec,
			sizeof(chanspec_t), resbuf, ACS_PER_CHAN_INFO_BUF_LEN);
#endif
		if (ret != BCME_OK) {
			ACSD_ERROR("%s Failed to get channel (0x%02x) info: %d\n",
				c_info->name, sub_chspec, ret);
			return 0;
		}

		sub_chinfo = dtoh32(*(uint32 *)resbuf);
		ACSD_DFSR("%s: sub_chspec 0x%04x info %08x (%s, %d minutes)\n",
			c_info->name, sub_chspec, sub_chinfo,
			(sub_chinfo & WL_CHAN_INACTIVE) ? "inactive" :
			((sub_chinfo & WL_CHAN_PASSIVE) ? "passive" : "active"),
			GET_INACT_TIME(sub_chinfo));

		/* combine subband chinfo (except inactive time) using bitwise OR */
		chinfo |= ((~INACT_TIME_MASK) & sub_chinfo);
		/* compute maximum inactive time amongst each subband */
		if (max_inactive < GET_INACT_TIME(sub_chinfo)) {
			max_inactive = GET_INACT_TIME(sub_chinfo);
		}
	}
	/* merge maximum inactive time computed into the combined chinfo */
	chinfo |= max_inactive << INACT_TIME_OFFSET;

	ACSD_DFSR("%s: chanspec 0x%04x info %08x (%s, %d minutes)\n",
		c_info->name, chspec, chinfo,
		(chinfo & WL_CHAN_INACTIVE) ? "inactive" :
		((chinfo & WL_CHAN_PASSIVE) ? "passive" : "active"),
		GET_INACT_TIME(chinfo));

	return chinfo;
}

/*
 * acs_dfs_channel_is_usable() - check whether a specific DFS channel is usable right now.
 *
 * Returns TRUE if so, FALSE if not (ie because the channel is currently out of service).
 *
 */
static bool
acs_dfs_channel_is_usable(acs_chaninfo_t *c_info, chanspec_t chspec)
{
	return (acs_channel_info(c_info, chspec) & WL_CHAN_INACTIVE) ? FALSE : TRUE;
}

/* is chanspec DFS channel */
static bool
acs_is_dfs_chanspec(acs_chaninfo_t *c_info, chanspec_t chspec)
{
	return (acs_channel_info(c_info, chspec) & WL_CHAN_RADAR) ? TRUE : FALSE;
}

/* is chanspec DFS weather channel */
static bool
acs_is_dfs_weather_chanspec(acs_chaninfo_t *c_info, chanspec_t chspec)
{
	return (acs_channel_info(c_info, chspec) & WL_CHAN_RADAR_EU_WEATHER) ? TRUE : FALSE;
}

static bool
acsd_is_lp_chan(acs_chaninfo_t *c_info, chanspec_t chspec)
{
	UNUSED_PARAMETER(c_info);

	/* Need to check with real txpwr */
	if (wf_chspec_ctlchan(chspec) <= LOW_POWER_CHANNEL_MAX) {
		/* <= 80MHz & primary of 160/80p80 */
		return TRUE;
	} else if (CHSPEC_BW_GT(chspec, WL_CHANSPEC_BW_80)) {
		return (wf_chspec_secondary80_channel(chspec) <= LOW_POWER_CHANNEL_MAX);
	}

	return FALSE;
}

/* for 160M/80p80M/80M/40 bw chanspec,select control chan
 * with max AP number for neighbor friendliness
 *
 * For 80p80 - adjust ctrl chan within primary 80Mhz
 */
chanspec_t
acs_adjust_ctrl_chan(acs_chaninfo_t *c_info, chanspec_t chspec)
{
	chanspec_t selected = chspec;
	acs_chan_bssinfo_t* bss_info = c_info->ch_bssinfo;
	uint8 i, j, max_sb, ch, channel;
	uint8 ctrl_sb[8] = {0}, num_sb[8] = {0};
	uint8 selected_sb, last_chan_idx = 0;
	fcs_conf_chspec_t *excl_chans = &(c_info->acs_fcs.excl_chans);

	if (nvram_match("acs_ctrl_chan_adjust", "0"))
		return selected;

	if (CHSPEC_ISLE20(selected))
		return selected;

	if (CHSPEC_IS160(selected)) {
		max_sb = 8;
	} else if (CHSPEC_IS80(selected) ||
		CHSPEC_IS8080(selected)) {
		max_sb = 4;
	} else {
		max_sb = 2;
	}

	i = 0;
	/* calulate no. APs for all 20M side bands */
	FOREACH_20_SB(selected, channel) {
		ctrl_sb[i] = channel;

		for (j = last_chan_idx; j < c_info->scan_chspec_list.count; j++) {
			ch = (int)bss_info[j].channel;

			if (ch == ctrl_sb[i]) {
				last_chan_idx = j;
				num_sb[i] = bss_info[j].nCtrl;
				ACSD_INFO("sb:%d channel = %d num_sb = %d\n", i,
						channel, num_sb[i]);
				break;
			}
		}
		i++;
	}

	/* when dyn160 is enabled with DFS on FCC, control ch of 50o must be ULL or higher */
	if (max_sb == 8 && c_info->dyn160_enabled && ACS_11H(c_info) &&
		CHSPEC_IS160(selected) &&
		!acs_is_country_edcrs_eu(c_info->country.ccode) &&
		CHSPEC_CHANNEL(selected) == ACS_DYN160_CENTER_CH) {
		i = 4;
		selected_sb = WL_CHANSPEC_CTL_SB_ULL >> WL_CHANSPEC_CTL_SB_SHIFT;
	} else {
		i = 0;
		selected_sb = (selected & WL_CHANSPEC_CTL_SB_MASK) >>
			WL_CHANSPEC_CTL_SB_SHIFT;
	}

	/* find which valid sideband has max no. APs */
	for (; i < max_sb; i++) {
		bool excl = FALSE;
		selected &= ~(WL_CHANSPEC_CTL_SB_MASK);
		selected |= (i << WL_CHANSPEC_CTL_SB_SHIFT);

		if (excl_chans && excl_chans->count) {
			for (j = 0; j < excl_chans->count; j++) {
				if (selected == excl_chans->clist[j]) {
					excl = TRUE;
					break;
				}
			}
		}

		if (!excl && num_sb[i] > num_sb[selected_sb]) {
			selected_sb = i;
			ACSD_INFO("selected sb so far = %d n_sbs = %d\n",
					selected_sb, num_sb[selected_sb]);
		}
	}

	ACSD_INFO("selected sb: %d\n", selected_sb);
	selected &=  ~(WL_CHANSPEC_CTL_SB_MASK);
	selected |= (selected_sb << WL_CHANSPEC_CTL_SB_SHIFT);
	ACSD_INFO("Final selected chanspec: 0x%4x\n", selected);
	return selected;
}

/* Invalidate all channels from selection present in Exclusion list,
 * if present in SCS/FCS configuration
 */
static void
acs_invalidate_exclusion_channels(acs_chaninfo_t *c_info,
	int bw, fcs_conf_chspec_t *excl_chans)
{
	int i, j;
	ch_candidate_t* candi;
	candi = c_info->candidate[bw];
	for (i = 0; i < c_info->c_count[bw]; i++) {
		/* Exclude channels build candidate */
		if (excl_chans && excl_chans->count) {
			for (j = 0; j < excl_chans->count; j++) {
				if (candi[i].chspec == excl_chans->clist[j]) {
					candi[i].valid = FALSE;
					candi[i].reason |= ACS_INVALID_EXCL;
					break;
				}
			}
		}
	}
}

/* check for availability of high power channel present in the list of
 * valid channels to select
 */
static bool
acs_fcs_check_for_hp_chan(acs_chaninfo_t *c_info, int bw)
{
	int i;
	ch_candidate_t* candi;
	bool ret = FALSE;
	candi = c_info->candidate[bw];
	for (i = 0; i < c_info->c_count[bw]; i++) {
		if ((!candi[i].valid) || (candi[i].is_dfs)) {
			continue;
		}

		if (!acsd_is_lp_chan(c_info, candi[i].chspec)) {
			ret = TRUE;
			break;
		}
	}
	return ret;

}
static bool
acs_fcs_check_for_nondfs_chan(acs_chaninfo_t *c_info, int bw)
{
	int i;
	ch_candidate_t *candi;
	bool ret = FALSE;
	candi = c_info->candidate[bw];
	for (i = 0; i < c_info->c_count[bw]; i++) {
		if (!candi[i].is_dfs) {
			ret = TRUE;
			break;
		}
	}
	return ret;
}

/* check for overlap between the passed channel arguments */
static bool
acs_fcs_check_for_overlap(chanspec_t cur_chspec, chanspec_t candi_chspec)
{
	uint8 channel1, channel2;

	FOREACH_20_SB(candi_chspec, channel1) {
		FOREACH_20_SB(cur_chspec, channel2) {
			if (channel1 == channel2) {
				return TRUE;
			}
		}
	}

	return FALSE;
}

static void
acs_invalidate_candidates(acs_chaninfo_t *c_info, ch_candidate_t *candi, int bw)
{
	acs_rsi_t *rsi = &c_info->rs_info;
	bool need_coex_check = BAND_2G(rsi->band_type) &&
	(rsi->bw_cap == WLC_BW_CAP_40MHZ) &&
	rsi->coex_enb;

	bool dfsr_disable = !(acs_dfsr_reentry_type(ACS_DFSR_CTX(c_info)) == DFS_REENTRY_IMMEDIATE);
	bool hp_chan_present = FALSE;
	bool non_dfs_present = FALSE;
	chanim_info_t * ch_info = c_info->chanim_info;
	time_t now = time(NULL);
	fcs_conf_chspec_t *excl_chans;
	chanspec_t cur_chspec = 0;
	int tmp_chspec, i, j;
	chanspec_t in_chspec = 0, out_chspec = 0;

	excl_chans = &(c_info->acs_fcs.excl_chans);
	acs_invalidate_exclusion_channels(c_info, bw, excl_chans);
	if (ACS_FCS_MODE(c_info)) {
		if (wl_iovar_getint(c_info->name, "chanspec", &tmp_chspec) < 0)
			cur_chspec = c_info->selected_chspec;
		else
			cur_chspec = (chanspec_t)tmp_chspec;

		if (!BAND_2G(rsi->band_type)) {
			hp_chan_present = acs_fcs_check_for_hp_chan(c_info, bw);
			if (c_info->acs_fcs.acs_start_on_nondfs) {
				non_dfs_present = acs_fcs_check_for_nondfs_chan(c_info, bw);
			}
		}
	}

	for (i = 0; i < c_info->c_count[bw]; i++) {
		/* going through the  coex check if needed */
		if ((bw == ACS_BW_40) && need_coex_check) {
			in_chspec = candi[i].chspec;
			out_chspec = acs_coex_check(c_info, in_chspec);
			if (in_chspec != out_chspec) {
				candi[i].valid = FALSE;
				candi[i].reason |= ACS_INVALID_COEX;
				continue;
			}
		}

		/* going through the interference check if needed */
		if (c_info->flags & ACS_FLAGS_INTF_THRES_CCA ||
				c_info->flags & ACS_FLAGS_INTF_THRES_BGN) {
			acs_candidate_check_intf(&candi[i], c_info);
		}

		/*
		 * For 20MHz channels, only pick from 1, 6, 11
		 * For 40MHz channels, only pick from control channel being 1, 6, 11 (BT doc)
		 * Mark all the other candidates invalid
		 */

		if (BAND_2G(rsi->band_type)) {
			acs_channel_t chan;
			ACSD_DEBUG("Filter chanspecs for 2G 40/20 MHz channels\n");
			acs_parse_chanspec(candi[i].chspec, &chan);

			ACSD_DEBUG("channel: %d, ext: %d\n", chan.control, chan.ext20);

			if ((!nvram_match("acs_2g_ch_no_restrict", "1")) &&
					(chan.control != ACS_CHANNEL_1) &&
					(chan.control != ACS_CHANNEL_6) &&
					(chan.control != ACS_CHANNEL_11))  {
				candi[i].valid = FALSE;
				candi[i].reason |= ACS_INVALID_OVLP;
				continue;
			}
			ACSD_DEBUG("valid channel: %d\n", chan.control);

		}

		/* Invalidate Unusable DFS channels */
		if (candi[i].is_dfs &&
				!acs_dfs_channel_is_usable(c_info, candi[i].chspec)) {
			ACSD_DEBUG("Invalidating %x - unusable DFS channel\n",
					candi[i].chspec);

			candi[i].valid = FALSE;
			candi[i].reason |= ACS_INVALID_DFS;
		}

		if (ACS_FCS_MODE(c_info)) {
			/* when dyn160 is enabled with DFS on FCC, allow ch 50o/subset only */
			if (ACS_11H(c_info) && c_info->dyn160_enabled &&
				CHSPEC_IS160(candi[i].chspec) &&
				!acs_is_country_edcrs_eu(c_info->country.ccode) &&
				CHSPEC_CHANNEL(candi[i].chspec) != ACS_DYN160_CENTER_CH) {
				candi[i].valid = FALSE;
				candi[i].reason |= ACS_INVALID_EXCL;
			}

			if (!acs_is_initial_selection(c_info)) {
				/* avoid select same channel */
				if (acs_fcs_check_for_overlap(cur_chspec, candi[i].chspec) &&
						(c_info->switch_reason != APCS_CSTIMER) &&
						(c_info->acs_fcs.txop_weight == 0)) {
					ACSD_FCS("Skipping cur chan 0x%x\n",
							cur_chspec);
					candi[i].reason |= ACS_INVALID_SAMECHAN;
				}

				/* avoid ping pong on txfail if channel switched recently */
				if (ACS_11H_AND_BGDFS(c_info) &&
						candi[i].is_dfs &&
						c_info->switch_reason == APCS_TXFAIL &&
						candi[i].chspec == c_info->recent_prev_chspec &&
						(now - c_info->acs_prev_chan_at <
						 ACS_RECENT_CHANSWITCH_TIME)) {
					ACSD_FCS("%s Skipping recent chan 0x%x\n", c_info->name,
							c_info->recent_prev_chspec);
					candi[i].reason |= ACS_INVALID_AVOID_PREV;
				}

				/* Use DFS channels if DFS reentry is OK */
				/* In ETSI, avoid weather chan if not pre-cleared on DFSRentry */
				if (!(candi[i].reason & ACS_INVALID_DFS) &&
						candi[i].is_dfs && (dfsr_disable ||
						(c_info->rs_info.reg_11h &&
						acs_is_country_edcrs_eu(c_info->country.ccode) &&
						acs_is_dfs_weather_chanspec(c_info, candi[i].chspec) &&
						ACS_CHINFO_IS_UNCLEAR(acs_channel_info(c_info,
						 candi[i].chspec))))) {
					candi[i].reason |= ACS_INVALID_DFS;
				}

				if (!candi[i].is_dfs && !dfsr_disable) {
					/* DFS Re-entry - Invalidate Non-DFS channels */
					candi[i].reason |= ACS_INVALID_NONDFS;
				}

			} else if (!BAND_2G(rsi->band_type)) {
				/*
				 * Use DFS channels if we are just coming up
				 * unless
				 *  - DFS is disabled
				 *  - DFS channel is inactive
				 *  - in EU and it is marked as a weather channel
				 */
				if (!(candi[i].reason & ACS_INVALID_DFS) &&
						candi[i].is_dfs &&
						((c_info->acs_fcs.acs_dfs == ACS_DFS_DISABLED) ||
						(c_info->rs_info.reg_11h &&
						acs_is_country_edcrs_eu(c_info->country.ccode) &&
						acs_is_dfs_weather_chanspec(c_info, candi[i].chspec)))) {
					/* invalidate the candidate for the current trial */
					candi[i].reason |= ACS_INVALID_DFS;
				}

				if (acsd_is_lp_chan(c_info, candi[i].chspec) &&
						hp_chan_present) {
					candi[i].reason |= ACS_INVALID_LPCHAN;
				}
				if (c_info->country_is_edcrs_eu) {
					if (non_dfs_present && candi[i].is_dfs &&
							c_info->acs_fcs.acs_start_on_nondfs) {
						candi[i].reason |= ACS_INVALID_DFS;
					}

					if (!candi[i].is_dfs &&
						c_info->acs_fcs.acs_dfs != ACS_DFS_DISABLED &&
						!c_info->acs_fcs.acs_start_on_nondfs) {
						/* Need to pick DFS channels on bootup -
						 * Invalidate Non-DFS channels
						 */
							candi[i].reason |= ACS_INVALID_NONDFS;
					}
				} else {
					if (!candi[i].is_dfs &&
						c_info->acs_fcs.acs_dfs != ACS_DFS_DISABLED) {
						/* Need to pick DFS channels on bootup -
						 * Invalidate Non-DFS channels
						 */
							candi[i].reason |= ACS_INVALID_NONDFS;
					}
				}

			}
			candi[i].valid = (candi[i].reason == 0);
		}
	}

	if (ACS_FCS_MODE(c_info)) {
		acs_dfsr_reentry_done(ACS_DFSR_CTX(c_info));
		/* DFS Re-entry has been done
		 * avoid channel flip and flop, skip the channel which selected in
		 * some amount of time. If txop_weight is set, allow flip-flop in order
		 * to consider broader list of channels.
		 */
		if (c_info->acs_fcs.txop_weight == 0)
			for (i = 0; i < c_info->c_count[bw]; i++) {
				for (j = 0; j < CHANIM_ACS_RECORD; j++) {
					if (candi[i].chspec == ch_info->record[j].selected_chspc) {
						if (now - ch_info->record[j].timestamp <
								c_info->acs_fcs.acs_chan_flop_period) {
							candi[i].valid = FALSE;
							candi[i].reason |=
								ACS_INVALID_CHAN_FLOP_PERIOD;
							j = CHANIM_ACS_RECORD;
						}
					}
				}
			}
	}
}

bool
acs_select_chspec(acs_chaninfo_t *c_info)
{
	bool need_coex_check = FALSE;
	chanspec_t selected = 0;
	ch_candidate_t *candi;
	int bw = ACS_BW_20, iter = 0;
	acs_rsi_t *rsi = &c_info->rs_info;
	bool dfsr_disable = !(acs_dfsr_reentry_type(ACS_DFSR_CTX(c_info)) == DFS_REENTRY_IMMEDIATE);

	/* if given a chanspec but just need to pass coex check */
	need_coex_check = BAND_2G(rsi->band_type) &&
		(rsi->bw_cap == WLC_BW_CAP_40MHZ) &&
		rsi->coex_enb;

	if (CHSPEC_IS2G(rsi->pref_chspec) && CHSPEC_IS40(rsi->pref_chspec) &&
		need_coex_check) {
		selected = acs_coex_check(c_info, rsi->pref_chspec);
		goto done;
	}

	if (WL_BW_CAP_160MHZ(rsi->bw_cap)) {
			bw = ACS_BW_160;
	} else if (WL_BW_CAP_80MHZ(rsi->bw_cap)) {
			bw = ACS_BW_80;
	} else if (WL_BW_CAP_40MHZ(rsi->bw_cap)) {
			bw = ACS_BW_40;
	}

	if ((bw == ACS_BW_160) && (dfsr_disable) &&
			(!acs_is_initial_selection(c_info))) {
			/* On 160Mhz unless we need to DFS Re-entry
			 * reduce operation to 80Mhz.
			 */
			bw = ACS_BW_80;
	}
recheck:
	ACSD_INFO("Selected BW %d; 0-20Mhz, 3-160Mhz\n", bw);
	/* build the candidate chanspec list */
	acs_build_candidates(c_info, bw);
	candi = c_info->candidate[bw];

	if (!candi) {
		ACSD_DEBUG("No candidates, try again in 2 sec.\n");
		sleep_ms(2000);

		if (iter++ < ACS_BW_DWNGRD_ITERATIONS) {
			bw = bw -1;
			ACSD_INFO("wl %s no channels for BW %d for %d secs; Downgraded BW %d\n",
				c_info->name, bw+1, iter*2, bw);
		}
		goto recheck;
	}

	acs_invalidate_candidates(c_info, candi, bw);

	/* compute channel scores */
	acs_candidate_score(c_info, bw);

	/* if there is at least one valid chanspec */
	if (acs_has_valid_candidate(c_info, bw)) {
		acs_policy_t *a_pol = &c_info->acs_policy;
		if (a_pol->chan_selector)
			selected = a_pol->chan_selector(c_info, bw);
		else
			ACSD_ERROR("chan_selector is null for the selected policy");
		goto done;
	} else if (ACS_FCS_MODE(c_info)) {
		ACSD_DEBUG("FCS: no valid channel to select. BW is not adjust. \n");
		/* FCS doesn't downgrade bandwidth if there is no valid channel can be selected
		 * on 20/40/80 Mhz. For 160/80p80 we can downgrade bw*/

		if (bw > ACS_BW_80) {
			ACSD_INFO("Downgrading bw to find a proper channel of operation.\n");
			goto reduce_bw;
		}

		/* DFSR if channel switch is due to packet loss */
		if (c_info->switch_reason == APCS_TXFAIL) {
			/* since we don't have any non-DFS channels left, allow immediate DFSR */
			acs_dfsr_set_reentry_type(ACS_DFSR_CTX(c_info), DFS_REENTRY_IMMEDIATE);
		}

		return FALSE;
	}

reduce_bw:
	/* if we failed to pick a chanspec, fall back to lower bw */
	if (bw > ACS_BW_20) {
		ACSD_DEBUG("Failed to find a valid chanspec\n");
		bw = bw - 1;
		goto recheck;
	} /* pick a chanspec if we are here */
	else {
		if (BAND_5G(rsi->band_type) && c_info->c_count[bw])
			selected = candi[(c_info->c_count[bw])-1].chspec;
		else
			selected = candi[0].chspec;
	}

done:
        ACSD_PRINT("selected channel spec: 0x%4x\n", selected);
        selected = acs_adjust_ctrl_chan(c_info, selected);
        ACSD_PRINT("Adjusted channel spec: 0x%4x\n", selected);
        ACSD_PRINT("selected channel spec: 0x%4x\n", selected);
		c_info->selected_chspec = selected;

	if (c_info->cur_chspec == c_info->selected_chspec) {
	    return FALSE;
	}
	return TRUE;
}

static int
acs_ret_larger_bw(acs_chaninfo_t *c_info, chanspec_t cur, chanspec_t next)
{
	if (CHSPEC_IS160(cur) && !CHSPEC_IS160(next)) {
		return -1;
	} else if (CHSPEC_IS160(next) && !CHSPEC_IS160(cur)) {
		return 1;
	}

	if (CHSPEC_IS80(cur) && !CHSPEC_IS80(next)) {
		return -1;
	} else if (CHSPEC_IS80(next) && !CHSPEC_IS80(cur)) {
		return 1;
	}

	if (CHSPEC_IS40(cur) && !CHSPEC_IS40(next)) {
		return -1;
	} else if (CHSPEC_IS40(next) && !CHSPEC_IS40(cur)) {
		return 1;
	}

	if (CHSPEC_IS20(cur) && !CHSPEC_IS20(next)) {
		return -1;
	} else if (CHSPEC_IS20(next) && !CHSPEC_IS20(next)) {
		return 1;
	}

	return 0;
}

/* Sort the dfs forced chspec list */
static void
acs_sort_dfs_frcd_list(acs_chaninfo_t *c_info, wl_dfs_forced_t *dfs_frcd)
{
	int i, j;
	chanspec_list_t *chspec_list = &dfs_frcd->chspec_list;

	for (i = 0; i < chspec_list->num - 1; i++) {
		for (j = 0; j < chspec_list->num - i - 1; j++) {
			bool swp = FALSE;
			int ret = (acs_ret_larger_bw(c_info, chspec_list->list[j],
						chspec_list->list[j+1]));

			if (ret > 0) {
				swp = TRUE;
			} else if (ret < 0) {
				swp = FALSE;
			} else if (!acs_is_dfs_chanspec(c_info, chspec_list->list[j]) &&
					acs_is_dfs_chanspec(c_info, chspec_list->list[j+1])) {
				swp = TRUE;
			} else if (!acs_is_dfs_chanspec(c_info, chspec_list->list[j+1]) &&
						acs_is_dfs_chanspec(c_info, chspec_list->list[j])) {
				swp = FALSE;
			} else if (CHSPEC_CHANNEL(chspec_list->list[j]) <
					CHSPEC_CHANNEL(chspec_list->list[j+1])) {
				swp = TRUE;
			} else if (wf_chspec_ctlchan(chspec_list->list[j]) <
					wf_chspec_ctlchan(chspec_list->list[j+1])) {
				swp = TRUE;
			}

			if (swp) {
				chanspec_t chspec = chspec_list->list[j];
				chspec_list->list[j] = chspec_list->list[j+1];
				chspec_list->list[j+1] = chspec;
			}
		}
	}
}

static int
acs_return_curr_bw(acs_rsi_t *rsi)
{
	int bw = WL_CHANSPEC_BW_20;

	if (WL_BW_CAP_160MHZ(rsi->bw_cap)) {
		bw = WL_CHANSPEC_BW_160;
	} else if (WL_BW_CAP_80MHZ(rsi->bw_cap)) {
		bw = WL_CHANSPEC_BW_80;
	} else if (WL_BW_CAP_40MHZ(rsi->bw_cap)) {
		bw = WL_CHANSPEC_BW_40;
	}

	return bw;
}

static bool
acs_prep_dfs_forced_chspec_list(acs_chaninfo_t *c_info, wl_dfs_forced_t **dfs_frcd)
{
	int i, j, ret, count = 0;
	wl_uint32_list_t *list;
	chanspec_t input = 0, c = 0;
	acs_rsi_t *rsi = &c_info->rs_info;
	fcs_conf_chspec_t *excl_chans = &(c_info->acs_fcs.excl_chans);
	int bw = acs_return_curr_bw(rsi);

	char *data_buf;
	data_buf = acsd_malloc(ACS_SM_BUF_LEN);

	ret = wl_iovar_getbuf(c_info->name, "chanspecs", &input, sizeof(chanspec_t),
		data_buf, ACS_SM_BUF_LEN);

	if (ret < 0) {
		ACS_FREE(data_buf);
		ACSD_ERROR("failed to get valid chanspec lists");
		return FALSE;
	}

	list = (wl_uint32_list_t *)data_buf;
	count = dtoh32(list->count);

	if (count == 0) {
		ACSD_INFO("No channels to list \n");
		return FALSE;
	}

	(*dfs_frcd) = (wl_dfs_forced_t *)acsd_realloc((char *)(*dfs_frcd),
			WL_DFS_FORCED_PARAMS_SIZE(count));

	for (i = 0; i < count; i++) {
		bool excl = FALSE;
		c = (chanspec_t)dtoh32(list->element[i]);

		if (BAND_5G(rsi->band_type) && !CHSPEC_IS5G(c)) {
			excl = TRUE;
		}

		if (!excl && excl_chans &&
				excl_chans->count) {
			for (j = 0; j < excl_chans->count; j++) {
				if (c == excl_chans->clist[j]) {
					excl = TRUE;
					break;
				}
			}
		}

		if (bw < CHSPEC_BW(c)) {
			excl = TRUE;
		}

		if (excl) {
			continue;
		}

		(*dfs_frcd)->chspec_list.list[(*dfs_frcd)->chspec_list.num++] = c;
	}

	(*dfs_frcd) = (wl_dfs_forced_t *)acsd_realloc((char *)(*dfs_frcd),
			WL_DFS_FORCED_PARAMS_SIZE((*dfs_frcd)->chspec_list.num));

	/* Arrange this list in order of higher bw channels with dfs channels
	 * prioritzed over non-dfs channels
	 */
	acs_sort_dfs_frcd_list(c_info, *dfs_frcd);
	return TRUE;
}

static int
acs_get_dfs_forced_chspec(acs_chaninfo_t *c_info, char smbuf[WLC_IOCTL_SMLEN])
{
	wl_dfs_forced_t inp;
	int ret = 0;
	acs_rsi_t *rsi = &c_info->rs_info;

	if (BAND_2G(rsi->band_type) || !rsi->reg_11h ||
		((ACS_FCS_MODE(c_info)) && (c_info->acs_fcs.acs_dfs == ACS_DFS_DISABLED))) {
		return -1;
	}

#ifdef DSLCPE_ENDIAN
	inp.version = htoe16(DFS_PREFCHANLIST_VER);
#else
	inp.version = DFS_PREFCHANLIST_VER;
#endif
	ret = wl_iovar_getbuf(c_info->name, "dfs_channel_forced", &inp, sizeof(wl_dfs_forced_t),
		smbuf, WLC_IOCTL_SMLEN);
#ifdef DSLCPE_ENDIAN
	{
		int i=0;
		wl_dfs_forced_t *dfs=(wl_dfs_forced_t *)smbuf;
		dfs->chspec=dtoh16(dfs->chspec);
		dfs->version=dtoh16(dfs->version);
		dfs->chspec_list.num=dtoh32(dfs->chspec_list.num);
		for(;i<dfs->chspec_list.num;i++) {
			dfs->chspec_list.list[i]=dtoh16(dfs->chspec_list.list[i]);
		}
	}
#endif

	return ret;
}

void
acs_get_best_dfs_forced_chspec(acs_chaninfo_t *c_info)
{
	char smbuf[WLC_IOCTL_SMLEN];
	int ret = 0;
	wl_dfs_forced_t *inp;

	ret = acs_get_dfs_forced_chspec(c_info, smbuf);

	if (ret < 0) {
		ACSD_ERROR("Get dfs chanspec forced fails \n");
		c_info->dfs_forced_chspec = 0;
		return;
	}

	inp = (wl_dfs_forced_t *)smbuf;
	c_info->dfs_forced_chspec = inp->chspec_list.list[0];
}

void
acs_set_dfs_forced_chspec(acs_chaninfo_t * c_info)
{
	int ret = 0;
	chanspec_t chspec = c_info->dfs_forced_chspec;
	acs_rsi_t *rsi = &c_info->rs_info;
	wl_dfs_forced_t *dfs_frcd;
	char smbuf[WLC_IOCTL_SMLEN];

	ACSD_INFO("Setting forced chanspec: 0x%x!\n", chspec);

	if (BAND_2G(rsi->band_type) || !rsi->reg_11h ||
		((ACS_FCS_MODE(c_info)) && (c_info->acs_fcs.acs_dfs == ACS_DFS_DISABLED))) {
		return;
	}

	ret = acs_get_dfs_forced_chspec(c_info, smbuf);

	if (ret < 0) {
		ACSD_ERROR("get dfs forced chanspec fails!\n");
		return;
	}

	dfs_frcd = (wl_dfs_forced_t *) smbuf;

	if (dfs_frcd->chspec_list.num) {
		ACSD_INFO("User has already issued a dfs_forced_chanspec. Keep that\n");
		return;
	}

	dfs_frcd = (wl_dfs_forced_t *)acsd_malloc(WL_DFS_FORCED_PARAMS_SIZE(1));

	/* overwrite with latest forced */
	if (chspec) {
		dfs_frcd->chspec_list.num = 0;
#ifndef DSLCPE_ENDIAN
		chspec = htod32(chspec);
#endif
		dfs_frcd->chspec_list.list[dfs_frcd->chspec_list.num++] = chspec;
	} else {
		dfs_frcd->version = DFS_PREFCHANLIST_VER;
		if (!acs_prep_dfs_forced_chspec_list(c_info, &dfs_frcd)) {
			ACSD_ERROR("Prep dfs forced list error\n");
			goto exit;
		}
	}
#ifdef DSLCPE_ENDIAN 
	{
		int i=0;
		for(;i<dfs_frcd->chspec_list.num;i++) {
			dfs_frcd->chspec_list.list[i]=htod16(dfs_frcd->chspec_list.list[i]);
		}
		dfs_frcd->chspec_list.num = htod32(dfs_frcd->chspec_list.num );
		dfs_frcd->version = htod16(DFS_PREFCHANLIST_VER);
		dfs_frcd->chspec = htod16(dfs_frcd->chspec);
	}
#endif
	ret = wl_iovar_set(c_info->name, "dfs_channel_forced", dfs_frcd,
#ifdef DSLCPE_ENDIAN
		WL_DFS_FORCED_PARAMS_FIXED_SIZE + (dtoh32(dfs_frcd->chspec_list.num) *
#else
		WL_DFS_FORCED_PARAMS_FIXED_SIZE + (dfs_frcd->chspec_list.num *
#endif
		sizeof(chanspec_t)));
	ACSD_INFO("set dfs forced chanspec 0x%x %s!\n", chspec, ret? "Fail" : "Succ");

exit:
	ACS_FREE(dfs_frcd);
}

/*
 * acs_get_txduration - get the overall tx duration
 * c_info - pointer to acs_chaninfo_t for an interface
 * Returns TRUE if tx duration is more than the txblanking threshold
 * Returns FALSE otherwise
 */
static bool
acs_get_txduration(acs_chaninfo_t * c_info)
{
	int ret = 0;
	char *data_buf;
	wl_chanim_stats_t *list;
	wl_chanim_stats_t param;
	chanim_stats_t * stats;
	int buflen = ACS_CHANIM_BUF_LEN;
	uint32 count = WL_CHANIM_COUNT_ONE;
	uint8 tx_duration;

	data_buf = acsd_malloc(ACS_CHANIM_BUF_LEN);
	list = (wl_chanim_stats_t *) data_buf;

	param.buflen = htod32(buflen);
	param.count = htod32(count);

	ret = wl_iovar_getbuf(c_info->name, "chanim_stats", &param,
			sizeof(wl_chanim_stats_t), data_buf, buflen);
	if (ret < 0) {
		ACS_FREE(data_buf);
		ACSD_ERROR("failed to get chanim results");
		return FALSE;
	}

	list->buflen = dtoh32(list->buflen);
	list->version = dtoh32(list->version);
	list->count = dtoh32(list->count);

	stats = list->stats;
	stats->chanspec = htod16(stats->chanspec);
	tx_duration = stats->ccastats[CCASTATS_TXDUR];
	ACSD_INFO("chspec 0x%4x tx_duration %d txblank_th %d\n", stats->chanspec,
			tx_duration, c_info->acs_bgdfs->txblank_th);
	ACS_FREE(data_buf);
	return (tx_duration > c_info->acs_bgdfs->txblank_th) ? TRUE : FALSE;
}

/*
 * try to initiate background DFS scan and move; returns BCME_OK if successful
 */
int
acs_bgdfs_attempt(acs_chaninfo_t * c_info, chanspec_t chspec, bool stunt)
{
	int ret = 0;
	acs_bgdfs_info_t *acs_bgdfs = c_info->acs_bgdfs;

	if (acs_bgdfs == NULL) {
		return BCME_ERROR;
	}

	/* when mode is ACS_MODE_FIXCHSPEC (eg. used with WBD), BGDFS is allowed
	 *  - only in EU &
	 *  - without move (stunt to preclear only)
	 */
	if (FIXCHSPEC(c_info)) {
		if (!c_info->country_is_edcrs_eu) {
			ACSD_INFO("%s BGDFS ch:0x%04x not allowed in ACS_MODE_FIXCHSPEC\n",
				c_info->name, chspec);
			return BCME_USAGE_ERROR;
		}
		if (!stunt) { /* in EU, downgrade to stunt when mode is ACS_MODE_FIXCHSPEC */
			stunt = TRUE;
			ACSD_INFO("%s BGDFS ch:0x%04x downgraded to stunt\n", c_info->name, chspec);
		}
	}

	if (acs_bgdfs->state != BGDFS_STATE_IDLE) {
		// already in progress; just return silently
		return BCME_OK;
	}

	/* In case of Far Stas, 3+1 DFS is not allowed */
	if (acs_bgdfs->bgdfs_avoid_on_far_sta && (c_info->sta_status & ACS_STA_EXIST_FAR)) {
		ACSD_INFO("%s BGDFS ch:0x%04x rejected - far STA present\n", c_info->name, chspec);
		return BCME_OK;
	}

	/* If tx duration more than tx blanking threshold, avoid BGDFS */
	if (acs_get_txduration(c_info)) {
		ACSD_INFO("%s BGDFS avoided as Tx duration exceeding threshold\n", c_info->name);
		return BCME_OK;
	}

	if (acs_bgdfs->cap == BGDFS_CAP_UNKNOWN) {
		// to update capability if this is the first time
		(void) acs_bgdfs_get(c_info);
	}
	if (acs_bgdfs->cap != BGDFS_CAP_TYPE0) {
		// other types are not supported
		return BCME_ERROR;
	}

	/* If setting channel, ensure chanspec is neighbor friendly */
	if (((int)chspec) > 0 && !stunt) {
		chspec = acs_adjust_ctrl_chan(c_info, chspec);
	}

	ACSD_INFO("%s####Attempting 3+1 on channel 0x%x\n", c_info->name, chspec);
	if ((ret = acs_bgdfs_set(c_info, chspec)) == BCME_OK) {
		time_t now = time(NULL);
		bool is_dfs_weather = acs_is_dfs_weather_chanspec(c_info, chspec);
		acs_bgdfs->state = BGDFS_STATE_MOVE_REQUESTED;
		acs_bgdfs->timeout = now +
			(is_dfs_weather ? BGDFS_CCA_EU_WEATHER : BGDFS_CCA_FCC) +
			BGDFS_POST_CCA_WAIT;
		if (stunt && (ret = acs_bgdfs_set(c_info, DFS_AP_MOVE_STUNT)) != BCME_OK) {
			ACSD_ERROR("Failed to stunt dfs_ap_move");
		}
	}
	return ret;
}

void
acs_set_chspec(acs_chaninfo_t * c_info, bool update_dfs_params, int ch_chng_reason)
{
	int ret = 0;
	chanspec_t chspec = c_info->selected_chspec;
	acs_bgdfs_info_t *acs_bgdfs = c_info->acs_bgdfs;
	wl_chan_change_reason_t reason;
	reason = (wl_chan_change_reason_t)ch_chng_reason;

	if (chspec) {
		bool is_dfs = acs_is_dfs_chanspec(c_info, chspec);
		bool is_dfs_weather = acs_is_dfs_weather_chanspec(c_info, chspec);

		if (reason == WL_CHAN_REASON_CSA) {
			ret = wl_iovar_setint(c_info->name, "chanspec", htod32(chspec));
		} else if (reason == WL_CHAN_REASON_DFS_AP_MOVE_START) {

			c_info->switch_reason = APCS_DFS_REENTRY;
			/* set mode to WBD_HANDLE_REQUEST */
			c_info->mode = ACS_MODE_MONITOR;
			if (!is_dfs || c_info->switch_reason != APCS_DFS_REENTRY ||
				(ret = acs_bgdfs_attempt(c_info, chspec, FALSE))
				!= BCME_OK) {

				/* fallback to regular set chanspec */
#ifdef DSLCPE_ENDIAN
				ret = wl_iovar_setint(c_info->name, "chanspec", chspec);
#else

				ret = wl_iovar_setint(c_info->name, "chanspec",
					htod32(chspec));
#endif
			}
			/* revert the mode to fix chanspec */
			c_info->mode = ACS_MODE_FIXCHSPEC;
		} else if ((reason == WL_CHAN_REASON_DFS_AP_MOVE_STUNT) ||
				(reason == WL_CHAN_REASON_DFS_AP_MOVE_ABORTED) ||
				(reason == WL_CHAN_REASON_DFS_AP_MOVE_RADAR_FOUND)) {

			if ((acs_bgdfs->state != BGDFS_STATE_IDLE) &&
				(acs_bgdfs->cap == BGDFS_CAP_TYPE0)) {

				int arg = 0x00;
				arg = ((reason == WL_CHAN_REASON_DFS_AP_MOVE_STUNT) ?
						DFS_AP_MOVE_STUNT: DFS_AP_MOVE_CANCEL);

				ret = acs_bgdfs_set(c_info, arg);
				if (ret != BCME_OK) {
					ACSD_ERROR("Failed dfs_ap_move option %d \n", arg);
				} else {
					acs_bgdfs->state = BGDFS_STATE_IDLE;
				}
			}
		} else {
			/* Default behavior for USE_ACSD_DEF_METHOD:
			 * if target channel is a DFS channel on DFS reentry, attempt bgdfs first
			 */
			if (!is_dfs || c_info->switch_reason != APCS_DFS_REENTRY ||
				(ret = acs_bgdfs_attempt(c_info, chspec, FALSE)) != BCME_OK) {
				// fallback to regular set chanspec
#ifdef DSLCPE_ENDIAN
				ret = wl_iovar_setint(c_info->name, "chanspec", chspec);
#else
				ret = wl_iovar_setint(c_info->name, "chanspec", htod32(chspec));
#endif
			}
		}
		if (ret == 0) {
			c_info->cur_is_dfs = is_dfs;
			c_info->cur_is_dfs_weather = is_dfs_weather;
			if (update_dfs_params) {
				acs_dfsr_chanspec_update(ACS_DFSR_CTX(c_info), chspec,
#ifdef DSLCPE_ENDIAN
					__FUNCTION__, c_info->name, c_info);
#else
					__FUNCTION__, c_info->name);
#endif
				acs_set_dfs_forced_chspec(c_info);
			}
		}
		else {
			ACSD_ERROR("set chanspec 0x%x failed!\n", chspec);
		}
	}
}

static void
acs_init_info(acs_info_t ** acs_info_p)
{
	acs_info = (acs_info_t*)acsd_malloc(sizeof(acs_info_t));

	*acs_info_p = acs_info;
}

void
acs_init_run(acs_info_t ** acs_info_p)
{
	char name[16], *next, prefix[PREFIX_LEN], name_enab_if[32] = { 0 }, *vifname, *vif_next;
	acs_chaninfo_t * c_info=NULL;
	int ret = 0;
	acs_init_info(acs_info_p);

	foreach(name, nvram_safe_get("acs_ifnames"), next) {
		c_info = NULL;
		osifname_to_nvifname(name, prefix, sizeof(prefix));
		if (acs_check_bss_is_enabled(name, &c_info, prefix) != BCME_OK) {
			strcat(prefix,"_vifs");
			vifname = nvram_safe_get(prefix);
			foreach(name_enab_if, vifname, vif_next) {
				if (acs_check_bss_is_enabled(name_enab_if, &c_info, NULL) == BCME_OK) {
					break;
				}
			}
		}
		memset(name, 0, sizeof(name));
		if (c_info != NULL) {
			memcpy(name, c_info->name, strlen(c_info->name) + 1);
		} else {
			continue;
		}
		ret = acs_start(name, c_info);

		if (ret) {
#ifdef DSLCPE
			ACSD_INFO("acs_start failed for ifname: %s\n", name);
			/* continue to next interface to correctly setup the
			 * next inferface's c_info.*/
			continue;
#else
			ACSD_ERROR("acs_start failed for ifname: %s\n", name);
			break;
#endif
		}

		if (AUTOCHANNEL(c_info) || COEXCHECK(c_info)) {
			/* First call to pick the chanspec for exit DFS chan */
			c_info->switch_reason = APCS_INIT;

			/* call to pick up init cahnspec */
			acs_select_chspec(c_info);
			/* Other APP can request to change the channel via acsd, in that
			 * case proper reason will be provided by requesting APP, For ACSD
			 * USE_ACSD_DEF_METHOD: ACSD's own default method to set channel
			 */
			acs_set_chspec(c_info, TRUE, ACSD_USE_DEF_METHOD);

			ret = acs_update_driver(c_info);
			if (ret)
				ACSD_ERROR("update driver failed\n");

			ACSD_DEBUG("ifname %s - mode: %s\n", name,
			   AUTOCHANNEL(c_info)? "SELECT" :
			   COEXCHECK(c_info)? "COEXCHECK" :
			   ACS11H(c_info)? "11H" : "MONITOR");

			chanim_upd_acs_record(c_info->chanim_info,
				c_info->selected_chspec, APCS_INIT);
		}

		if (c_info->acs_boot_only) {
			c_info->mode = ACS_MODE_DISABLE;
		}
	}
}

/* check if there is still associated scbs. reture value: TRUE if yes. */
static bool
acs_check_assoc_scb(acs_chaninfo_t * c_info)
{
	bool connected = TRUE;
	int result = 0;
	int ret = 0;

	ret = wl_iovar_getint(c_info->name, "scb_assoced", &result);
	if (ret) {
		ACSD_ERROR("failed to get scb_assoced\n");
		return connected;
	}

#ifdef DSLCPE_ENDIAN
	connected = (result) ? TRUE : FALSE;
#else
	connected = dtoh32(result) ? TRUE : FALSE;
#endif
	ACSD_DEBUG("connected: %d\n",  connected);

	return connected;
}

int
acs_update_driver(acs_chaninfo_t * c_info)
{
	int ret = 0;
	bool param = TRUE;
	/* if we are already beaconing, after the acs scan and new chanspec selection,
	   we need to ask the driver to do some updates (beacon, probes, etc..).
	*/
#ifdef DSLCPE_ENDIAN
	ret = wl_iovar_setint(c_info->name, "acs_update", ((uint)param));
#else
	ret = wl_iovar_setint(c_info->name, "acs_update", htod32((uint)param));
#endif
	ACS_ERR(ret, "acs update failed\n");

	return ret;
}

int
acs_fcs_ci_scan_check(acs_chaninfo_t *c_info)
{
	acs_fcs_t *fcs_info = &c_info->acs_fcs;
	acs_scan_chspec_t* chspec_q = &c_info->scan_chspec_list;
	time_t now = time(NULL);

	/* return for non fcs mode or no chan to scan */
	if (!ACS_FCS_MODE(c_info) || (chspec_q->count <= chspec_q->excl_count)) {
		return 0;
	}

	if ((CHSPEC_BW(c_info->cur_chspec) > WL_CHANSPEC_BW_40) &&
			!acsd_is_lp_chan(c_info, c_info->cur_chspec)) {
		ACSD_FCS("%s@%d: No CI scan if running in %dM high chan\n", __FUNCTION__, __LINE__,
				((CHSPEC_BW(c_info->cur_chspec) == WL_CHANSPEC_BW_80) ? 80 :
				 (CHSPEC_BW(c_info->cur_chspec) == WL_CHANSPEC_BW_8080) ? 8080 : 160));
		return 0;
	}

	/* start ci scan:
	1. when txop is less than thld, start ci scan for pref chan
	2. if no scan for a long period, start ci scan
	*/

	/* scan pref chan: when txop < thld, start ci scan for pref chan */
	if (c_info->scan_chspec_list.ci_pref_scan_request && (chspec_q->pref_count > 0)) {
		c_info->scan_chspec_list.ci_pref_scan_request = FALSE;

		if (chspec_q->ci_scan_running != ACS_CI_SCAN_RUNNING_PREF) {
			ACSD_FCS("acs_ci_scan_timeout start CI pref scan: scan_count %d\n",
				chspec_q->pref_count);
			chspec_q->ci_scan_running = ACS_CI_SCAN_RUNNING_PREF;
			fcs_info->acs_ci_scan_count = chspec_q->pref_count;
			acs_ci_scan_update_idx(&c_info->scan_chspec_list, 0);
		}
	}

	/* check for current scanning status */
	if (chspec_q->ci_scan_running)
		return 1;

	/* check scan timeout, and trigger CI scan if timeout happened */
	if ((now - fcs_info->timestamp_acs_scan) >= fcs_info->acs_ci_scan_timeout) {
		fcs_info->acs_ci_scan_count = chspec_q->count - chspec_q->excl_count;
		chspec_q->ci_scan_running = ACS_CI_SCAN_RUNNING_NORM;
		acs_ci_scan_update_idx(&c_info->scan_chspec_list, 0);
		ACSD_FCS("acs_ci_scan_timeout start CI scan: now %u(%u), scan_count %d\n",
			(uint)now, fcs_info->timestamp_acs_scan,
			chspec_q->count - chspec_q->excl_count);
		return 1;
	}

	return 0;
}

static int
acs_fcs_ci_scan_finish_check(acs_chaninfo_t * c_info)
{
	acs_fcs_t *fcs_info = &c_info->acs_fcs;
	acs_scan_chspec_t* chspec_q = &c_info->scan_chspec_list;

	/* do nothing for fcs mode or scanning not active  */
	if ((!ACS_FCS_MODE(c_info)) || (!chspec_q->ci_scan_running))
		return 0;

	/* Check for end of scan: scanned all channels once */
	if ((fcs_info->acs_ci_scan_count) && (!(--fcs_info->acs_ci_scan_count))) {
		ACSD_FCS("acs_ci_scan_timeout stop CI scan: now %u \n", (uint)time(NULL));
		chspec_q->ci_scan_running = 0;
	}

	return 0;
}

static int
acs_fcs_tx_idle_check(acs_chaninfo_t *c_info)
{
	uint timer = c_info->acs_cs_scan_timer;
	time_t now = time(NULL);
	char cntbuf[WL_CNTBUF_MAX_SIZE];
	wl_cnt_info_t *cntinfo;
	wl_cnt_wlc_t *wlc_cnt;
	int full_scan = 0;
	int ret = 0;
	uint32 acs_txframe;
	acs_fcs_t *fcs_info = &c_info->acs_fcs;

	if (!ACS_FCS_MODE(c_info))
		return full_scan;

	/* Check for idle period "acs_cs_scan_timer" */
	if ((now - fcs_info->timestamp_tx_idle) < timer)
		return full_scan;

	ACSD_FCS("acs_fcs_tx_idle: now %u(%u)\n", (uint)now, fcs_info->timestamp_tx_idle);

	/* Check wl transmit activity and trigger full scan if it is idle */
	ret = wl_iovar_get(c_info->name, "counters", cntbuf, WL_CNTBUF_MAX_SIZE);
	if (ret < 0) {
		ACSD_ERROR("wl counters failed (%d)\n", ret);
		return full_scan;
	}

	cntinfo = (wl_cnt_info_t *)cntbuf;
	cntinfo->version = dtoh16(cntinfo->version);
	cntinfo->datalen = dtoh16(cntinfo->datalen);
#ifdef DSLCPE_ENDIAN
	wl_endian_probe(c_info->name);
#endif
	/* Translate traditional (ver <= 10) counters struct to new xtlv type struct */
	/* As we need only wlc layer ctrs here, no need to input corerev.  */
	ret = wl_cntbuf_to_xtlv_format(NULL, cntbuf, WL_CNTBUF_MAX_SIZE, 0);
	if (ret < 0) {
		ACSD_ERROR("wl_cntbuf_to_xtlv_format failed (%d)\n", ret);
		return full_scan;
	}

	if (!(wlc_cnt = GET_WLCCNT_FROM_CNTBUF(cntbuf))) {
		ACSD_ERROR("wlc_cnt NULL\n");
		return full_scan;
	}
#ifdef DSLCPE_ENDIAN
	wlc_cnt->txframe = dtoh32(wlc_cnt->txframe);
#endif

	ACSD_FCS("acs_fcs_tx_idle: txframe %d(%d)\n", wlc_cnt->txframe, fcs_info->acs_txframe);

	if (wlc_cnt->txframe > fcs_info->acs_txframe)
		acs_txframe = wlc_cnt->txframe - fcs_info->acs_txframe;
	else
		acs_txframe = wlc_cnt->txframe + ((uint32)0xFFFFFFFF - fcs_info->acs_txframe);

	if (acs_txframe < (fcs_info->acs_tx_idle_cnt * (now - fcs_info->timestamp_tx_idle))) {
		ACSD_FCS("acs_fcs_tx_idle fullscan: %d\n",	fcs_info->acs_txframe);
		full_scan = 1;
	}

	fcs_info->acs_txframe = wlc_cnt->txframe;
	fcs_info->timestamp_tx_idle = now;
	return full_scan;
}

/* gets (updates) bgdfs capability and status of the interface; returns bgdfs capability */
uint16
acs_bgdfs_get(acs_chaninfo_t * c_info)
{
	int ret = 0;
	acs_bgdfs_info_t *acs_bgdfs = c_info->acs_bgdfs;

	if (acs_bgdfs == NULL) {
		ACSD_ERROR("acs_bgdfs is NULL");
		return BCME_ERROR;
	}

	ret = wl_iovar_get(c_info->name, "dfs_ap_move", &acs_bgdfs->status,
			sizeof(acs_bgdfs->status));
	if (ret != BCME_OK) {
		ACSD_INFO("get dfs_ap_move returned %d.\n", ret);
		return acs_bgdfs->cap = BGDFS_CAP_UNSUPPORTED;
	}
	return acs_bgdfs->cap = BGDFS_CAP_TYPE0;
}

/* request bgdfs set; for valid values of 'arg' see help page of dfs_ap_move iovar */
int
acs_bgdfs_set(acs_chaninfo_t * c_info, int arg)
{
	int ret = 0;
#ifdef DSLCPE_ENDIAN
	ret = wl_iovar_setint(c_info->name, "dfs_ap_move",
			(int)(arg));
#else
	ret = wl_iovar_setint(c_info->name, "dfs_ap_move",
			(int)(htod32(arg)));
#endif
	if (arg > 0 && c_info->acs_bgdfs != NULL) {
		c_info->acs_bgdfs->last_attempted = arg;
		c_info->acs_bgdfs->last_attempted_at = (uint64) time(NULL);
	}
	if (ret != BCME_OK) {
		ACSD_ERROR("set dfs_ap_move %d returned %d.\n", arg, ret);
	}
	return ret;
}

/* Derive bandwidth from a given chanspec(i.e cur_chspec) */
int
acs_derive_bw_from_given_chspec(acs_chaninfo_t * c_info)
{
	int bw, chbw;

	chbw = CHSPEC_BW(c_info->cur_chspec);
	switch (chbw) {
		case WL_CHANSPEC_BW_160:
			bw = ACS_BW_160;
			break;
		case WL_CHANSPEC_BW_8080:
			bw = ACS_BW_8080;
			break;
		case WL_CHANSPEC_BW_80:
			bw = ACS_BW_80;
			break;
		case WL_CHANSPEC_BW_40:
			bw = ACS_BW_40;
			break;
		case WL_CHANSPEC_BW_20:
			bw = ACS_BW_20;
			break;
		default:
			ACSD_ERROR("bandwidth unsupported ");
			return BCME_UNSUPPORTED;
	}
	return bw;
}

/* When ACSD is in SCS mode on 5G, nvram control and 802.11h is enabled, this function checks
 * whether the channel is dfs or not if so, it will change from dfs to non dfs channel.
 * These changes are made according to customer requirement.
 */
int
acs_scs_cs_scan_change_from_dfs_to_nondfs(acs_chaninfo_t * c_info)
{

	int cur_chspec = 0;
	acs_rsi_t *rsi = &c_info->rs_info;
	int count = 0, ret = 0;
	int i, bw;
	ch_candidate_t *candi;

	if (!ACS_FCS_MODE(c_info) && BAND_5G(c_info->rs_info.band_type) &&
		(rsi->reg_11h == TRUE) && c_info->cur_is_dfs && c_info->acsd_scs_dfs_scan) {
		bw = acs_derive_bw_from_given_chspec(c_info);
		ACS_ERR(bw, "bandwidth unsupported for given chanspec\n");

find_chan:
		count = c_info->c_count[bw];
		if (!count) {
			if (bw > ACS_BW_20)
			{
				bw = bw - 1;
				goto find_chan;
			}
			ACSD_ERROR("acs number of valid chanspec is %d\n", count);
			return -1;
		}

		candi = c_info->candidate[bw];
		for (i = 0; i < count; i++) {
			if (!(acs_channel_info(c_info, candi[i].chspec) & WL_CHAN_RADAR)) {
				cur_chspec = candi[i].chspec;
				ACSD_DEBUG("picking non dfs chanspec is %x \n", cur_chspec);
				break;
			}
		}

		if (cur_chspec == 0) {
			if (bw > ACS_BW_20)
			{
				bw = bw - 1;
				goto find_chan;
			}
			ACSD_ERROR("acs found no valid non-DFS channel\n");
			return -1;
		}

		ACSD_DEBUG("Set to non dfs channel\n");
		ret = wl_iovar_setint(c_info->name, "chanspec", htod32(cur_chspec));
		ACS_ERR(ret, "set chanspec failed\n");

		c_info->cur_chspec = cur_chspec;
		ret = acs_update_driver(c_info);
		ACS_ERR(ret, "update driver failed\n");
	}
	return ret;
}
/*
 * acs_scan_timer_or_dfsr_check() - check for scan timer or dfs reentry, change channel if needed.
 *
 * This function checks whether we need to change channels because of CS scan timer expiration or
 * DFS Reentry, and does the channel switch if so.
 */
int
acs_scan_timer_or_dfsr_check(acs_chaninfo_t * c_info)
{
	uint cs_scan_timer;
	chanim_info_t * ch_info;
	int ret = 0;
	uint8 cur_idx;
	uint8 start_idx;
	chanim_acs_record_t *start_record;
	int switch_reason = APCS_INIT; /* Abuse APCS_INIT as undefined switch reason (no switch) */
	ch_info = c_info->chanim_info;
	cur_idx = chanim_mark(ch_info).record_idx;
	start_idx = MODSUB(cur_idx, 1, CHANIM_ACS_RECORD);
	start_record = &ch_info->record[start_idx];

	if (AUTOCHANNEL(c_info) && ((c_info->acsd_scs_dfs_scan && !ACS_FCS_MODE(c_info)) ||
		 !(c_info->cur_is_dfs))) {
		/* Check whether we should switch now because of the CS scan timer */
		cs_scan_timer = c_info->acs_cs_scan_timer;

		if (SCAN_TIMER_ON(c_info) && cs_scan_timer) {
			time_t passed;

			ACSD_DEBUG(" timer: %d\n", cs_scan_timer);

			passed = time(NULL) - start_record->timestamp;

			if (acs_fcs_tx_idle_check(c_info) ||
				((passed > cs_scan_timer) && (!acs_check_assoc_scb(c_info)))) {
				switch_reason = APCS_CSTIMER;
			}
		}
	}

	/* If not switching because of CS scan timer, see if DFS Reentry switch is needed */
	if ((switch_reason == APCS_INIT) &&
		ACS_FCS_MODE(c_info) &&
		acs_dfsr_reentry_type(ACS_DFSR_CTX(c_info)) == DFS_REENTRY_IMMEDIATE) {
			ACSD_DFSR("Switching Channels for DFS Reentry.\n");
			switch_reason = APCS_DFS_REENTRY;
	}
	c_info->switch_reason = switch_reason;

	switch (switch_reason) {
	case APCS_CSTIMER:
		/* Handling the ACS_SCS mode criteria according to customer requirement */
		ret = acs_scs_cs_scan_change_from_dfs_to_nondfs(c_info);
		ACS_ERR(ret, "acs_scs_cs_scan_change_from_dfs_to_non_dfs failed\n");
		/* start scan */
		ret = acs_run_cs_scan(c_info);
		ACS_ERR(ret, "cs scan failed\n");
		acs_cleanup_scan_entry(c_info);

		ret = acs_request_data(c_info);
		ACS_ERR(ret, "request data failed\n");

		/* Fall through to common case */

	case APCS_DFS_REENTRY:

		if (acs_select_chspec(c_info)) {
			/* Other APP can request to change the channel via acsd, in that
			 * case proper reason will be provided by requesting APP, For ACSD
			 * USE_ACSD_DEF_METHOD: ACSD's own default method to set channel
			 */
			acs_set_chspec(c_info, TRUE, ACSD_USE_DEF_METHOD);

			/*
			 * In FCC, on txfail, if bgdfs is not successful due to txblanking
			 * we enable flag to do CAC on Full MIMO
			 */
			if (ACS_11H_AND_BGDFS(c_info) &&
					c_info->acs_bgdfs->fallback_blocking_cac &&
					c_info->acs_bgdfs->state != BGDFS_STATE_IDLE &&
					!c_info->country_is_edcrs_eu) {
				c_info->acs_bgdfs->acs_bgdfs_on_txfail = TRUE;
			}

			/* for consecutive timer based trigger, replace the previous one */
			if ((switch_reason == APCS_CSTIMER) &&
			    start_record->trigger == APCS_CSTIMER)
				chanim_mark(ch_info).record_idx = start_idx;

			chanim_upd_acs_record(ch_info, c_info->selected_chspec, switch_reason);

			if (c_info->acs_bgdfs == NULL ||
					c_info->acs_bgdfs->state == BGDFS_STATE_IDLE) {
				ret = acs_update_driver(c_info);
				ACS_ERR(ret, "update driver failed\n");
			}
		}
		else {
		    start_record->timestamp = time(NULL);
		}
		break;

	default:
		break;
	}
	return ret;
}

int
acs_do_ci_update(uint ticks, acs_chaninfo_t * c_info)
{
	int ret = 0;

	if (ticks % c_info->acs_ci_scan_timer)
		return ret;

	acs_expire_scan_entry(c_info, (time_t)c_info->acs_scan_entry_expire);

	if (ACS_FCS_MODE(c_info) && (!(c_info->scan_chspec_list.ci_scan_running)))
		return ret;

	if (c_info->cur_is_dfs) {
		ACSD_INFO("No CI scan when running in DFS chan:%x\n", c_info->cur_chspec);
	}
	else {
		ret = acs_run_ci_scan(c_info);
		ACS_ERR(ret, "ci scan failed\n");

		ret = acs_request_data(c_info);
		ACS_ERR(ret, "request data failed\n");
	}

	acs_fcs_ci_scan_finish_check(c_info);

	return ret;
}

/* get country details for an interface */
static int
acs_get_country(acs_chaninfo_t * c_info)
{
	int ret = BCME_OK;

	ret = wl_iovar_get(c_info->name, "country", &c_info->country,
			sizeof(c_info->country));

	/* ensure null termination before logging/using */
	c_info->country.country_abbrev[WLC_CNTRY_BUF_SZ - 1] = '\0';
	c_info->country.ccode[WLC_CNTRY_BUF_SZ - 1] = '\0';
	c_info->country_is_edcrs_eu = acs_is_country_edcrs_eu(c_info->country.ccode);

	if (ret != BCME_OK) {
		ACSD_ERROR("get country on %s returned %d.\n", c_info->name, ret);
	} else {
		ACSD_INFO("get country on %s returned %d. ca=%s, cr=%d, cc=%s\n",
				c_info->name, ret,
				c_info->country.country_abbrev,
				c_info->country.rev, c_info->country.ccode);
	}

	return ret;
}

/*
 * acs_is_country_edcrs_eu - takes country_code string and
 * returns true if it is a EDCRS_EU country.
 * EDCRS_EU countries follow harmonized ETSI regulations.
 */
static bool
acs_is_country_edcrs_eu(char * country_code)
{

	static const char cc_list[][WLC_CNTRY_BUF_SZ] = {
		"AL", "DZ", "AD", "AT",    "AZ", "BE", "BJ", "BT",
		"BA", "BW", "IO", "BG",    "CD", "CI", "HR", "CY",
		"CZ", "DK", "EE", "FO",    "FI", "FR", "GE", "DE",
		"GH", "GI", "GR", "GL",    "GG", "GN", "HU", "IS",
		"IE", "IL", "IT", "JE",    "JO", "KE", "KW", "LV",
		"LB", "LI", "LT", "LU",    "MK", "MG", "MW", "MT",
		"IM", "MU", "MC", "MN",    "ME", "MA", "NL", "NG",
		"NO", "OM", "PK", "PN",    "PL", "PT", "QA", "RO",
		"RW", "SM", "SA", "SN",    "RS", "SK", "SI", "ZA",
		"ES", "SZ", "SE", "CH",    "TH", "TN", "TR", "AE",
		"UG", "GB", "VA", "ZW",    "E0"
	};
	int i, len = sizeof(cc_list)/sizeof(cc_list[0]);

	if (country_code == NULL || strlen(country_code) >= WLC_CNTRY_BUF_SZ) {
		ACSD_ERROR("country null or malformed\n");
		return FALSE;
	}

	for (i = 0; i < len; ++i) {
		if (strncmp(cc_list[i], country_code, WLC_CNTRY_BUF_SZ) == 0) {
			ACSD_INFO("country %s is an EDCRS_EU country (@%d/%d)\n",
					country_code, i, len);
			return TRUE;
		}
	}

	ACSD_INFO("country %s is not an EDCRS_EU country (%d)\n", country_code, len);
	return FALSE;
}

int
acs_update_status(acs_chaninfo_t * c_info)
{
	int ret = 0;
	int cur_chspec;

	ret = wl_iovar_getint(c_info->name, "chanspec", &cur_chspec);
	ACS_ERR(ret, "acs get chanspec failed\n");

#ifdef DSLCPE_ENDIAN
	if ((chanspec_t)(cur_chspec) == c_info->cur_chspec) {
#else
	/* return if the channel hasn't changed */
	if ((chanspec_t)dtoh32(cur_chspec) == c_info->cur_chspec) {
#endif
		return ret;
	}

	/* To add a acs_record when finding out channel change isn't made by ACSD */
#ifdef DSLCPE
	c_info->cur_chspec = (chanspec_t)(cur_chspec);
#else
	c_info->cur_chspec = (chanspec_t)dtoh32(cur_chspec);
#endif
	c_info->cur_is_dfs = acs_is_dfs_chanspec(c_info, cur_chspec);
	c_info->cur_is_dfs_weather = acs_is_dfs_weather_chanspec(c_info, cur_chspec);
	c_info->is160_bwcap = WL_BW_CAP_160MHZ((c_info->rs_info).bw_cap);

	chanim_upd_acs_record(c_info->chanim_info,
			c_info->cur_chspec, APCS_NONACSD);
	acs_dfsr_chanspec_update(ACS_DFSR_CTX(c_info), c_info->cur_chspec,
#ifdef DSLCPE_ENDIAN
		        __FUNCTION__, c_info->name, c_info);
#else
			__FUNCTION__, c_info->name);
#endif

	if (acs_get_country(c_info) == BCME_OK) {
		c_info->country_is_edcrs_eu = acs_is_country_edcrs_eu(c_info->country.ccode);
	} else {
		c_info->country_is_edcrs_eu = FALSE;
	}

	ACSD_INFO("%s: chanspec: 0x%x is160_bwcap %d is160_upgradable %d, is160_downgradable %d\n",
		c_info->name, c_info->cur_chspec, c_info->is160_bwcap,
		c_info->is160_upgradable, c_info->is160_downgradable);

	return ret;
}

/* acs_update_oper_mode read the current oper_mode and update */
int
acs_update_oper_mode(acs_chaninfo_t * c_info)
{
	int ret = BCME_ERROR, oper_mode = 0;

	if ((ret = wl_iovar_getint(c_info->name, "oper_mode", &oper_mode)) != BCME_OK) {
		ACSD_ERROR("%s read oper_mode failed with %d\n", c_info->name, ret);
		return ret;
	}
	c_info->oper_mode = (uint16) oper_mode;
	ACSD_INFO("%s read oper_mode succeeded 0x%02x\n", c_info->name, oper_mode);

	return ret;
}

/* acs_set_oper_mode set the oper_mode */
int
acs_set_oper_mode(acs_chaninfo_t * c_info, uint16 oper_mode)
{
	int ret = BCME_ERROR;

	if ((ret = wl_iovar_setint(c_info->name, "oper_mode", oper_mode)) != BCME_OK) {
		ACSD_ERROR("%s setting oper_mode (0x%02x) failed with %d\n", c_info->name,
			oper_mode, ret);
		return ret;
	}

	c_info->oper_mode = oper_mode;
	ACSD_INFO("%s setting oper_mode succeeded 0x%02x\n", c_info->name, oper_mode);

	return ret;
}

/**
 * check dyn160_enabled through iovar and if enabled, update phy_dyn_switch score
 */
int
acs_update_dyn160_status(acs_chaninfo_t * c_info)
{
	int ret = 0;
	int dyn160_enabled, phy_dyn_switch;

	/* fetch `wl dyn160` */
	ret = wl_iovar_getint(c_info->name, "dyn160", &dyn160_enabled);
	ACS_ERR(ret, "acs get dyn160 failed\n");

	c_info->dyn160_enabled = (dyn160_enabled != 0);

	if (!c_info->dyn160_enabled) {
		c_info->phy_dyn_switch = 0;
		return BCME_OK;
	}

	/* if dyn160 is enabled fetch metric `wl phy_dyn_switch` */
	ret = wl_iovar_getint(c_info->name, "phy_dyn_switch", &phy_dyn_switch);
	ACS_ERR(ret, "acs get phy_dyn_switch failed\n");

	c_info->phy_dyn_switch = (uint8) (phy_dyn_switch & 0xFFu);

	(void) acs_update_oper_mode(c_info);

	c_info->is160_upgradable = c_info->is160_bwcap && !CHSPEC_IS160(c_info->cur_chspec) &&
		!c_info->is_mu_active && c_info->phy_dyn_switch != 1;
	c_info->is160_downgradable = c_info->is160_bwcap && CHSPEC_IS160(c_info->cur_chspec) &&
		c_info->phy_dyn_switch == 1;

	ACSD_INFO("%s phy_dyn_switch: %d is160_upgradable %d is160_downgradable %d \n",
		c_info->name, c_info->phy_dyn_switch, c_info->is160_upgradable,
		c_info->is160_downgradable);

	return BCME_OK;
}

void
acs_cleanup(acs_info_t ** acs_info_p)
{
	int i;

	if (!*acs_info_p)
		return;

	for (i = 0; i < ACS_MAX_IF_NUM; i++) {
		acs_chaninfo_t* c_info = (*acs_info_p)->chan_info[i];

		ACS_FREE(c_info->scan_results);

		if (c_info->acs_escan->acs_use_escan)
			acs_escan_free(c_info->acs_escan->escan_bss_head);

		acs_cleanup_scan_entry(c_info);
		ACS_FREE(c_info->ch_bssinfo);
		ACS_FREE(c_info->chanim_stats);
		ACS_FREE(c_info->scan_chspec_list.chspec_list);
		ACS_FREE(c_info->candidate[ACS_BW_20]);
		ACS_FREE(c_info->candidate[ACS_BW_40]);
		ACS_FREE(c_info->chanim_info);
		ACS_FREE(c_info->acs_bgdfs);

		acs_dfsr_exit(ACS_DFSR_CTX(c_info));

		ACS_FREE(c_info);
	}
	ACS_FREE(acs_info);
	*acs_info_p = NULL;
}

/* set intfer trigger params */
int acs_intfer_config(acs_chaninfo_t *c_info)
{
	wl_intfer_params_t params;
	acs_intfer_params_t *intfer = &(c_info->acs_fcs.intfparams);
	int err = 0;
	uint8 thld_setting = ACSD_INTFER_PARAMS_80_THLD;

	ACSD_INFO("%s@%d\n", __FUNCTION__, __LINE__);

	/*
	 * When running 80MBW high chan, and far STA exists
	 * we will use the high threshold for txfail trigger
	 */
	if (!acsd_is_lp_chan(c_info, c_info->cur_chspec) &&
			(c_info->sta_status & ACS_STA_EXIST_FAR)) {
		if (CHSPEC_IS80(c_info->cur_chspec)) {
			thld_setting = ACSD_INTFER_PARAMS_80_THLD_HI;
		} else if (CHSPEC_BW_GE(c_info->cur_chspec,
					WL_CHANSPEC_BW_160)) {
			thld_setting = ACSD_INTFER_PARAMS_160_THLD_HI;
		}
	} else if (CHSPEC_BW_GE(c_info->cur_chspec, WL_CHANSPEC_BW_160)) {
		thld_setting = ACSD_INTFER_PARAMS_160_THLD;
	}

	if (thld_setting == intfer->thld_setting) {
		ACSD_FCS("Same Setting intfer[%d].\n", thld_setting);
		return err;
	}
	intfer->thld_setting = thld_setting;
#ifdef DSLCPE_ENDIAN
        /* only version is in uint16, need to convert */
        params.version = htod16(INTFER_VERSION);
#else
	params.version = INTFER_VERSION;
#endif
	params.period = intfer->period;
	params.cnt = intfer->cnt;
	params.txfail_thresh =
		intfer->acs_txfail_thresholds[intfer->thld_setting].txfail_thresh;
	params.tcptxfail_thresh =
		intfer->acs_txfail_thresholds[intfer->thld_setting].tcptxfail_thresh;

	err = wl_iovar_set(c_info->name, "intfer_params", (void *)&params,
			sizeof(wl_intfer_params_t));

        if (err < 0) {
#ifdef DSLCPE_ENDIAN
                /* for now, NIC mode does not support intfer_params, compress the error */
                ACSD_INFO("intfer_params error! ret code: %d\n", err);
#else
                ACSD_ERROR("intfer_params error! ret code: %d\n", err);
#endif
        }

	ACSD_FCS("Setting intfer[%d]: cnt:%d period:%d tcptxfail:%d txfail:%d\n",
			thld_setting, params.period, params.cnt,
			params.tcptxfail_thresh, params.txfail_thresh);

	return err;
}

int acs_update_assoc_info(acs_chaninfo_t *c_info)
{
        struct maclist *list;
        acs_assoclist_t *acs_assoclist;
        int ret = 0, cnt, size;
        acs_fcs_t *fcs_info = &c_info->acs_fcs;

        /* reset assoc STA staus */
        c_info->sta_status = ACS_STA_NONE;

        ACSD_INFO("%s@%d\n", __FUNCTION__, __LINE__);

        /* read assoclist */
        list = (struct maclist *)acsd_malloc(ACSD_BUFSIZE_4K);
        memset(list, 0, ACSD_BUFSIZE_4K);
        ACSD_FCS("WLC_GET_ASSOCLIST\n");
        list->count = htod32((ACSD_BUFSIZE_4K - sizeof(int)) / ETHER_ADDR_LEN);
        ret = wl_ioctl(c_info->name, WLC_GET_ASSOCLIST, list, ACSD_BUFSIZE_4K);
        if (ret < 0) {
                ACSD_ERROR("WLC_GET_ASSOCLIST failure\n");
                ACS_FREE(list);
                return ret;
        }

        ACS_FREE(c_info->acs_assoclist);
        list->count = dtoh32(list->count);
        if (list->count <= 0) {
                ACS_FREE(list);
                return ret;
        }

        size = sizeof(acs_assoclist_t) + (list->count)* sizeof(acs_sta_info_t);
        acs_assoclist = (acs_assoclist_t *)acsd_malloc(size);

        c_info->acs_assoclist = acs_assoclist;
        acs_assoclist->count = list->count;

        for (cnt = 0; cnt < list->count; cnt++) {
                scb_val_t scb_val;

                memset(&scb_val, 0, sizeof(scb_val));
                memcpy(&scb_val.ea, &list->ea[cnt], ETHER_ADDR_LEN);

                ret = wl_ioctl(c_info->name, WLC_GET_RSSI, &scb_val, sizeof(scb_val));

                if (ret < 0) {
                        ACSD_ERROR("Err: reading intf:%s STA:"MACF" RSSI\n",
                                c_info->name, ETHER_TO_MACF(list->ea[cnt]));
                        ACS_FREE(c_info->acs_assoclist);
                        break;
                }

                acs_assoclist->sta_info[cnt].rssi = dtoh32(scb_val.val);
                ether_copy(&(list->ea[cnt]), &(acs_assoclist->sta_info[cnt].ea));
                ACSD_FCS("%s@%d sta_info sta:"MACF" rssi:%d [%d]\n",
                        __FUNCTION__, __LINE__,
                        ETHER_TO_MACF(list->ea[cnt]), dtoh32(scb_val.val),
                        fcs_info->acs_far_sta_rssi);

                if (acs_assoclist->sta_info[cnt].rssi < fcs_info->acs_far_sta_rssi)
                        c_info->sta_status |= ACS_STA_EXIST_FAR;
                else
                        c_info->sta_status |= ACS_STA_EXIST_CLOSE;

                ACSD_FCS("%s@%d sta_status:0x%x\n", __FUNCTION__, __LINE__, c_info->sta_status);
        }
        ACS_FREE(list);

        if (!ret) {
                /* check to see if we need to update intfer params */
                acs_intfer_config(c_info);
        }

        return ret;
}

/*
 * Check to see if we need to enable DFS reentry for
 * (1) all the STA are far
 * (2) We running in high power chan
 */
int
acsd_trigger_dfsr_check(acs_chaninfo_t *c_info)
{
        acs_assoclist_t *acs_assoclist = c_info->acs_assoclist;
        bool dfsr_disable = (c_info->acs_fcs.acs_dfs != ACS_DFS_REENTRY);
        bool is_dfs = c_info->cur_is_dfs;
	int bw = CHSPEC_BW(c_info->cur_chspec);

        ACSD_FCS("sta_status:0x%x chanspec:0x%x acs_dfs:%d acs_assoclist:%p is_dfs:%d\n",
                c_info->sta_status, c_info->cur_chspec,
                c_info->acs_fcs.acs_dfs, c_info->acs_assoclist, is_dfs);

        if ((bw > WL_CHANSPEC_BW_40) &&
                !acsd_is_lp_chan(c_info, c_info->cur_chspec) &&
                acs_assoclist &&
                (c_info->sta_status & ACS_STA_EXIST_FAR) &&
                !dfsr_disable &&
                !is_dfs) {
                ACSD_FCS("goto DFSR.\n");
                return TRUE;
        }

        return FALSE;
}

/*
 * Check to see if we need goto hi power cahn
 * (1) if exit from DSF chan, we goto hi power chan
 */
int
acsd_hi_chan_check(acs_chaninfo_t *c_info)
{
	bool is_dfs = c_info->cur_is_dfs;
	int bw = CHSPEC_BW(c_info->cur_chspec);

	if (bw <= WL_CHANSPEC_BW_40 || !is_dfs) {
		ACSD_FCS("Not running in 80MBW or higher DFS Chanspec:0x%x\n",
				c_info->cur_chspec);
		return FALSE;
	}

	ACSD_FCS("running in 80Mbw DFS Chanspec:0x%x\n",
			c_info->cur_chspec);
	return TRUE;
}

/*
 *  check if need to switch chan:
 * (1) if run in hi-chan, all STA are far, DFS-reentry is disabled,
 *  chan switch is needed
 */
bool
acsd_need_chan_switch(acs_chaninfo_t *c_info)
{
	acs_assoclist_t *acs_assoclist = c_info->acs_assoclist;
	bool dfsr_disable = (c_info->acs_fcs.acs_dfs != ACS_DFS_REENTRY);
	bool is_dfs = c_info->cur_is_dfs;
	int bw = CHSPEC_BW(c_info->cur_chspec);

	ACSD_FCS("sta_status:0x%x chanspec:0x%x acs_dfs:%d acs_assoclist:%p is_dfs:%d\n",
			c_info->sta_status, c_info->cur_chspec,
			c_info->acs_fcs.acs_dfs, c_info->acs_assoclist, is_dfs);

	if ((bw > WL_CHANSPEC_BW_40) &&
			!acsd_is_lp_chan(c_info, c_info->cur_chspec) &&
			acs_assoclist &&
			(c_info->sta_status & ACS_STA_EXIST_FAR) &&
			dfsr_disable &&
			!is_dfs) {
		ACSD_FCS("No chan switch is needed.\n");
		return FALSE;
	}
	return TRUE;
}

/* get traffic information of the interface */
static int
acs_get_traffic_info(acs_chaninfo_t * c_info, acs_traffic_info_t *t_info)
{
	char cntbuf[WL_CNTBUF_MAX_SIZE];
	wl_cnt_info_t *cntinfo;
	wl_cnt_wlc_t *wlc_cnt;
	int ret = BCME_OK;

	if (wl_iovar_get(c_info->name, "counters", cntbuf, WL_CNTBUF_MAX_SIZE) < 0) {
		ACSD_DFSR("Failed to fetch interface counters for '%s'\n", c_info->name);
		ret = BCME_ERROR;
		goto exit;
	}

	cntinfo = (wl_cnt_info_t *)cntbuf;
	cntinfo->version = dtoh16(cntinfo->version);
	cntinfo->datalen = dtoh16(cntinfo->datalen);
#ifdef DSLCPE_ENDIAN
	wl_endian_probe(c_info->name);
#endif
	/* Translate traditional (ver <= 10) counters struct to new xtlv type struct */
	if (wl_cntbuf_to_xtlv_format(NULL, cntbuf, WL_CNTBUF_MAX_SIZE, 0)
		!= BCME_OK) {
		ACSD_DFSR("wl_cntbuf_to_xtlv_format failed for '%s'\n", c_info->name);
		ret = BCME_ERROR;
		goto exit;
	}

	if ((wlc_cnt = GET_WLCCNT_FROM_CNTBUF(cntbuf)) == NULL) {
		ACSD_DFSR("GET_WLCCNT_FROM_CNTBUF NULL for '%s'\n", c_info->name);
		ret = BCME_ERROR;
		goto exit;
	}

	t_info->timestamp = time(NULL);
#ifdef DSLCPE_ENDIAN
	t_info->txbyte = dtoh32(wlc_cnt->txbyte);
	t_info->rxbyte = dtoh32(wlc_cnt->rxbyte);
	t_info->txframe = dtoh32(wlc_cnt->txframe);
	t_info->rxframe = dtoh32(wlc_cnt->rxframe);
#else
	t_info->txbyte = wlc_cnt->txbyte;
	t_info->rxbyte = wlc_cnt->rxbyte;
	t_info->txframe = wlc_cnt->txframe;
	t_info->rxframe = wlc_cnt->rxframe;
#endif
exit:
	return ret;
}

/* get traffic information about TOAd video STAs (if any) */
static int
acs_get_video_sta_traffic_info(acs_chaninfo_t * c_info, acs_traffic_info_t *t_info)
{
	acs_fcs_t *fcs_info = &c_info->acs_fcs;
	char stabuf[ACS_MAX_STA_INFO_BUF];
	sta_info_t *sta;
	int i, ret = BCME_OK;
	int index = fcs_info->video_sta_idx;

	struct ether_addr ea;
	acs_traffic_info_t total;

	memset(&total, 0, sizeof(acs_traffic_info_t));
	/* Consolidate the traffic info of all video stas */
	for (i = 0; i < index; i++) {
		memset(stabuf, 0, sizeof(stabuf));
		memcpy(&ea, &fcs_info->vid_sta[i].ea, sizeof(ea));
		if (wl_iovar_getbuf(c_info->name, "sta_info",
				&ea, sizeof(ea),
				stabuf, sizeof(stabuf)) < 0) {
			ACSD_ERROR("sta_info for %s failed\n", fcs_info->vid_sta[i].vid_sta_mac);
			return BCME_ERROR;
		}
		sta = (sta_info_t *)stabuf;
		total.txbyte = total.txbyte + dtoh64(sta->tx_tot_bytes);
		total.rxbyte = total.rxbyte + dtoh64(sta->rx_tot_bytes);
		total.txframe = total.txframe + dtoh32(sta->tx_tot_pkts);
		total.rxframe = total.rxframe + dtoh32(sta->rx_tot_pkts);
	}
	t_info->timestamp = time(NULL);
	t_info->txbyte = total.txbyte;
	t_info->rxbyte = total.rxbyte;
	t_info->txframe = total.txframe;
	t_info->rxframe = total.rxframe;
	return ret;
}

/*
 * acs_get_initial_traffic_stats - retrieve and store traffic activity info when acsd starts
 *
 * c_info - pointer to acs_chaninfo_t for an interface
 *
 * Returns BCME_OK when successful; error status otherwise
 */
int
acs_get_initial_traffic_stats(acs_chaninfo_t *c_info)
{
	acs_activity_info_t *acs_act = &c_info->acs_activity;
	acs_fcs_t *fcs_info = &c_info->acs_fcs;
	acs_traffic_info_t *t_prev = &acs_act->prev_bss_traffic;
	acs_traffic_info_t t_curr;
	int ret;

	if (!fcs_info->acs_toa_enable) {
		if ((ret = acs_get_traffic_info(c_info, &t_curr)) != BCME_OK) {
			ACSD_ERROR("Failed to get traffic information\n");
			return ret;
		}
	} else {
		if ((ret = acs_get_video_sta_traffic_info(c_info, &t_curr)) != BCME_OK) {
			ACSD_ERROR("Failed to get video sta traffic information\n");
			return ret;
		}
	}

	t_prev->txframe = t_curr.txframe;
	t_prev->rxframe = t_curr.rxframe;

	return BCME_OK;
}

/*
 * acs_activity_update - updates traffic activity information
 *
 * c_info - pointer to acs_chaninfo_t for an interface
 *
 * Returns BCME_OK when successful; error status otherwise
 */
int
acs_activity_update(acs_chaninfo_t * c_info)
{
	acs_activity_info_t *acs_act = &c_info->acs_activity;
	acs_fcs_t *fcs_info = &c_info->acs_fcs;
	time_t now = time(NULL);
	acs_traffic_info_t t_curr;
	acs_traffic_info_t *t_prev = &acs_act->prev_bss_traffic;
	acs_traffic_info_t *t_accu_diff = &acs_act->accu_diff_bss_traffic;
	acs_traffic_info_t *t_prev_diff = &acs_act->prev_diff_bss_traffic;
	uint32 total_frames; /* total tx and rx frames on link */
	int ret;

#ifdef DSLCPE_ENDIAN
	/*  this function will  call some function to use htol16 etc in bcmxtlv.c where
	 *  it is changed to etoh16 need the probo to set gg_swap right*/
	wl_endian_probe(c_info->name);
#endif
	if (!fcs_info->acs_toa_enable) {
		if ((ret = acs_get_traffic_info(c_info, &t_curr)) != BCME_OK) {
			ACSD_ERROR("Failed to get traffic information\n");
			return ret;
		}
	} else {
		if ((ret = acs_get_video_sta_traffic_info(c_info, &t_curr)) != BCME_OK) {
			ACSD_ERROR("Failed to get video sta traffic information\n");
			return ret;
		}
	}

	/* update delta between current and previous fetched */
	t_prev_diff->timestamp = now - t_prev->timestamp;
	t_prev_diff->txbyte = DELTA_FRAMES((t_prev->txbyte), (t_curr.txbyte));
	t_prev_diff->rxbyte = DELTA_FRAMES((t_prev->rxbyte), (t_curr.rxbyte));
	t_prev_diff->txframe = DELTA_FRAMES((t_prev->txframe), (t_curr.txframe));
	t_prev_diff->rxframe = DELTA_FRAMES((t_prev->rxframe), (t_curr.rxframe));

	/* add delta (calculated above) to accumulated deltas */
	t_accu_diff->timestamp += t_prev_diff->timestamp;
	t_accu_diff->txbyte += t_prev_diff->txbyte;
	t_accu_diff->rxbyte += t_prev_diff->rxbyte;
	t_accu_diff->txframe += t_prev_diff->txframe;
	t_accu_diff->rxframe += t_prev_diff->rxframe;

	acs_act->num_accumulated++;

	total_frames =  t_prev_diff->txframe + t_prev_diff->rxframe;

	acs_bgdfs_sw_add(ACS_DFSR_CTX(c_info), now, total_frames);

	/* save current in t_prev (previous) to help with next time delta calculation */
	memcpy(t_prev, &t_curr, sizeof(*t_prev));

	return BCME_OK;
}

/*
 * acs_bgdfs_idle_check - checks if ahead of time BGDFS scan could be done.
 *
 * c_info - pointer to acs_chaninfo_t for an interface
 *
 * Returns BCME_OK when ahead of time scan is allowed. BCME_ERROR or BCME_BUSY otherwise.
 */
int
acs_bgdfs_idle_check(acs_chaninfo_t * c_info)
{
	acs_activity_info_t *acs_act = &c_info->acs_activity;
	acs_bgdfs_info_t *acs_bgdfs;
	uint32 th_frames, total_frames;
	int min_accumulate;

	if (!ACS_11H_AND_BGDFS(c_info)) {
		return BCME_ERROR;
	}

	acs_bgdfs = c_info->acs_bgdfs;

	min_accumulate = (acs_bgdfs->idle_interval / (ACS_TRAFFIC_INFO_UPDATE_INTERVAL(acs_bgdfs)));

	if (acs_act->num_accumulated < min_accumulate) {
		return BCME_OK;
	}

	th_frames = acs_bgdfs->idle_frames_thld;

	total_frames = acs_bgdfs_sw_sum(ACS_DFSR_CTX(c_info));

	/* avoid background scan if accumulated traffic exceeds threshold */
	if (total_frames > th_frames) {
		acs_bgdfs->idle = FALSE;
		ACSD_INFO("%s: Link not idle.Accumulated frames tx+rx:%u, th:%u, num_acc:%u\n",
			c_info->name, total_frames, th_frames, acs_act->num_accumulated);
		return BCME_BUSY;
	}

	ACSD_INFO("%s: Link idle. Initiaiting 3+1 bgdfs. Accumulated frames tx+rx:%u,"
		"th:%u, num_acc:%u\n", c_info->name, total_frames, th_frames,
		acs_act->num_accumulated);

	acs_bgdfs->idle = TRUE;

	return BCME_OK;
}

/* acs_get_recent_timestamp - gets timestamp of the recent most acs record
 * (or returns zero when record isn't found)
 * c_info - pointer to acs_chaninfo_t for an interface
 * chspec - channel spec to find in acs record
 *
 * Returns timestamp if acs record is found (or zero)
 */
static uint64
acs_get_recent_timestamp(acs_chaninfo_t *c_info, chanspec_t chspec)
{
	uint64 timestamp = 0;
	int i;
	chanim_info_t * ch_info = c_info->chanim_info;

	for (i = CHANIM_ACS_RECORD - 1; i >= 0; i--) {
		if (chspec == ch_info->record[i].selected_chspc) {
			if (ch_info->record[i].timestamp > timestamp) {
				timestamp = (uint64) ch_info->record[i].timestamp;
			}
		}
	}

	return timestamp;
}

/*
 * acs_bgdfs_choose_channel - identifies the best channel to
 *   - do BGDFS scan ahead of time if include_unclear is TRUE
 *
 * c_info - pointer to acs_chaninfo_t for an interface
 * include_unclear - DFS channels that can be cleared by BGDFS are considered
 * pick_160        - To pick a 160 Mhz chanspec for upgrading
 *
 * Returns BCME_OK when successful; error status otherwise.
 */
static int
acs_bgdfs_choose_channel(acs_chaninfo_t * c_info, bool include_unclear,	bool pick_160)
{
	chanspec_t cand_ch = 0, best_ch = 0;
	int ret, i, count, considered = 0;
	bool cand_is_weather = FALSE, best_is_weather = FALSE;
	bool cand_attempted = FALSE, best_attempted = FALSE;
	uint64 cand_ts = 0, best_ts = 0; /* recent time stamp in acs record */
	uint32 cand_chinfo;
	uint32 requisite = WL_CHAN_VALID_HW | WL_CHAN_VALID_SW | WL_CHAN_BAND_5G | WL_CHAN_RADAR;
	uint64 now = (uint64)(time(NULL));

	ch_candidate_t *cand_arr;
	int bw = ACS_BW_80;
	int chbw = pick_160 ? WL_CHANSPEC_BW_160 : CHSPEC_BW(c_info->cur_chspec);

	switch (chbw) {
		case WL_CHANSPEC_BW_160:
			bw = ACS_BW_160;
			break;
		case WL_CHANSPEC_BW_8080:
			bw = ACS_BW_8080;
			break;
		case WL_CHANSPEC_BW_80:
			bw = ACS_BW_80;
			break;
		case WL_CHANSPEC_BW_40:
			bw = ACS_BW_40;
			break;
		case WL_CHANSPEC_BW_20:
			bw = ACS_BW_20;
			break;
		default:
			ACSD_ERROR("bandwidth unsupported ");
			return BCME_UNSUPPORTED;
	}
	ret = acs_build_candidates(c_info, bw); /* build the candidate chanspec list */
	if (ret != BCME_OK) {
		ACSD_ERROR("%s: %s could not get list of candidates\n", c_info->name, __FUNCTION__);
		return BCME_ERROR;
	}
	cand_arr = c_info->candidate[bw];

	if (!c_info->rs_info.reg_11h) {
		ACSD_ERROR("%s: %s called when 11h is not enabled\n", c_info->name, __FUNCTION__);
		return BCME_ERROR;
	}

	count = c_info->c_count[bw];

	for (i = 0; i < count; i++) {
		cand_ch = cand_arr[i].chspec;
		cand_ts = acs_get_recent_timestamp(c_info, cand_ch);

		cand_chinfo = acs_channel_info(c_info, cand_ch);
		cand_is_weather = ((cand_chinfo & WL_CHAN_WEATHER_RADAR) != 0);
		cand_attempted = (cand_ch == c_info->acs_bgdfs->last_attempted) ||
			(((~WL_CHANSPEC_CTL_SB_MASK) & cand_ch) ==
			 ((~WL_CHANSPEC_CTL_SB_MASK) & c_info->acs_bgdfs->last_attempted));

		ACSD_INFO("%s: %s Candidate %d: 0x%x, chinfo: 0x%x, weather:%d, attempted:%d\n",
				c_info->name, __FUNCTION__,
				i, cand_ch, cand_chinfo, cand_is_weather, cand_attempted);

		/* reject if already the current channel */
		if (c_info->cur_chspec == cand_ch) {
			continue;
		}

		/* reject overlap with current; just match center channel ignoring control offset */
		if (((~WL_CHANSPEC_CTL_SB_MASK) & c_info->cur_chspec) ==
				((~WL_CHANSPEC_CTL_SB_MASK) & cand_ch)) {
			continue;
		}

		/* reject if all requisites aren't met */
		if ((cand_chinfo & requisite) != requisite) {
			continue;
		}

		/* reject if marked inactive */
		if (ACS_CHINFO_IS_INACTIVE(cand_chinfo)) {
			continue;
		}

		/* reject if already cleared */
		if (include_unclear && ACS_CHINFO_IS_CLEARED(cand_chinfo)) {
			continue;
		}

		/* avoid frequent flip flop; reject recently used ones */
		if ((now - cand_ts) < c_info->acs_fcs.acs_chan_flop_period) {
			continue;
		}

		/* avoid recent BGDFS attempted channel */
		if (cand_attempted && (now - c_info->acs_bgdfs->last_attempted_at) <
				(c_info->acs_bgdfs->idle_interval * 2)) {
			continue;
		}

		ACSD_DEBUG("%s: %s Considered %d: 0x%x\n", c_info->name, __FUNCTION__, i, cand_ch);

		/* passed all checks above; now it may be considered for rating best */
		considered ++;

		if (considered == 1) {
			best_ch = cand_ch;
			best_attempted = cand_attempted;
			best_ts = cand_ts;
			best_is_weather = cand_is_weather;
		}

		/* start comparison after we have more than one per_chan_info */
		if (considered < 2) continue;

		/* reject if candidate is a low power channel and chosen best is high power */
		if (acsd_is_lp_chan(c_info, cand_ch) &&
				!acsd_is_lp_chan(c_info, best_ch)) {
			continue;
		}

		if ((best_ts > cand_ts) || /* prefer least recently used */
				/* prefer a channel different from recently BGDFS attempted one */
				(best_attempted && !cand_attempted) ||
				/* when both are non-weather, prefer higher channel */
				(!best_is_weather && !cand_is_weather &&
				 CHSPEC_CHANNEL(cand_ch) > CHSPEC_CHANNEL(best_ch))) {
			/* update best with candidate since better */
			best_ch = cand_ch;
			best_attempted = cand_attempted;
			best_ts = cand_ts;
			best_is_weather = cand_is_weather;
		}
	}

	if (considered > 0) {
		best_ch = acs_adjust_ctrl_chan(c_info, best_ch);
		ACSD_INFO("%s: %s best_ch: 0x%x\n", c_info->name, __FUNCTION__, best_ch);
		c_info->acs_bgdfs->next_scan_chan = best_ch;
		if (pick_160) {
			c_info->selected_chspec = best_ch;
		}
	}

	return BCME_OK;
}

/*
 * acs_bgdfs_check_status - checks status of previous BGDFS scan
 *
 * c_info - pointer to acs_chaninfo_t for an interface
 *
 * Returns BCME_OK when previous scan result matches the channel requested by
 * ACSD and indicates it as radar free; other error statuses otherwise.
 */
int
acs_bgdfs_check_status(acs_chaninfo_t * c_info, bool bgdfs_on_txfail)
{
	acs_bgdfs_info_t *acs_bgdfs = c_info->acs_bgdfs;
	wl_dfs_ap_move_status_t *status;
	chanspec_t scan_ch;

	if (acs_bgdfs == NULL) {
		return BCME_UNSUPPORTED;
	}

	if (bgdfs_on_txfail) {
		scan_ch = c_info->selected_chspec;
	} else if (acs_bgdfs->next_scan_chan != 0) {
		scan_ch = acs_bgdfs->next_scan_chan;
	} else {
		return BCME_ERROR;
	}

	/* if channel is cleared, don't bother to verify move status for error */
	if (ACS_CHINFO_IS_CLEARED(acs_channel_info(c_info, scan_ch))) {
		return BCME_OK;
	}

	if (acs_bgdfs_get(c_info) != BGDFS_CAP_TYPE0) {
		return BCME_UNSUPPORTED;
	}

	status = &acs_bgdfs->status;

	/* sanity checks */
	if (status->version != WL_DFS_AP_MOVE_VERSION ||
			status->scan_status.version != WL_DFS_STATUS_ALL_VERSION ||
			status->scan_status.num_sub_status < BGDFS_STATES_MIN_SUB_STATES) {
		return BCME_UNSUPPORTED;
	}
	if (BGDFS_SUB_CHAN(status, BGDFS_SUB_MAIN_CORE) != scan_ch &&
			BGDFS_SUB_LAST(status, BGDFS_SUB_MAIN_CORE) != scan_ch &&
			BGDFS_SUB_CHAN(status, BGDFS_SUB_SCAN_CORE) != scan_ch &&
			BGDFS_SUB_LAST(status, BGDFS_SUB_SCAN_CORE) != scan_ch) {
		ACSD_ERROR("background scan channel 0x%x mismatch [0x%x, 0x%x, 0x%x, 0x%x]\n",
				scan_ch,
				BGDFS_SUB_CHAN(status, BGDFS_SUB_MAIN_CORE),
				BGDFS_SUB_LAST(status, BGDFS_SUB_MAIN_CORE),
				BGDFS_SUB_CHAN(status, BGDFS_SUB_MAIN_CORE),
				BGDFS_SUB_LAST(status, BGDFS_SUB_MAIN_CORE));
		return BCME_ERROR;
	}

	if (status->move_status != DFS_SCAN_S_RADAR_FREE) {
		return BCME_ERROR;
	}

	return BCME_OK;
}

/*
 * acs_bgdfs_ahead_trigger_scan - triggers ahead of time BGDFS scan
 *
 * c_info - pointer to acs_chaninfo_t for an interface
 *
 * Returns BCME_OK when successful; error status otherwise.
 */
int
acs_bgdfs_ahead_trigger_scan(acs_chaninfo_t * c_info)
{
	int ret;
	acs_bgdfs_info_t *acs_bgdfs = c_info->acs_bgdfs;
	chanspec_t chosen_chspec = 0;
	bool is_etsi = c_info->country_is_edcrs_eu;
	bool is_dfs = c_info->cur_is_dfs;

	if (acs_bgdfs == NULL) {
		return BCME_UNSUPPORTED;
	}

	/* In FCC, and already on a DFS channel return silently */
	if (!is_etsi && is_dfs) {
		return BCME_OK;
	}

	/* attempt to find a channel to do DFS scan on */
	/* find best excluding precleared channels */
	if (acs_bgdfs->next_scan_chan == 0 &&
			(ret = acs_bgdfs_choose_channel(c_info, TRUE, FALSE)) != BCME_OK) {
		ACSD_INFO("acs_bgdfs_choose_channel returned %d\n", ret);
	}

	chosen_chspec = acs_bgdfs->next_scan_chan;
	ACSD_INFO("%s: chosen_chspec = 0x%0x\n", c_info->name, chosen_chspec);

	/* if still chosen_chspec is 0, return */
	if (chosen_chspec == 0) {
		return BCME_OK;
	}

	/* if chosen chosen_chspec is in use; mark and return */
	if (chosen_chspec == c_info->cur_chspec) {
		acs_bgdfs->next_scan_chan = 0;
		return BCME_OK;
	}

	/* In FCC/ETSI, if on a low power Non-DFS, attempt a DFS 3+1 move */
	if (!acs_is_dfs_chanspec(c_info, c_info->cur_chspec) &&
			acsd_is_lp_chan(c_info, c_info->cur_chspec)) {
		ACSD_INFO("%s: moving to DFS channel 0x%0x\n", c_info->name, chosen_chspec);
		if ((ret = acs_bgdfs_attempt(c_info, chosen_chspec, FALSE)) != BCME_OK) {
			ACSD_ERROR("dfs_ap_move Failed\n");
			return ret;
		}
		return BCME_OK;
	}
	if (is_etsi) {
		/* Pre-clearing/stunt the selected channel for future use */
		ACSD_INFO("%s: pre-clearing DFS channel 0x%0x\n", c_info->name, chosen_chspec);
		if ((ret = acs_bgdfs_attempt(c_info, chosen_chspec, TRUE)) != BCME_OK) {
			return ret;
		}
	}
	return BCME_OK;
}

/*
 * acs_bgdfs_attempt_on_txfail - attempts BGDFS scan on txfail
 *
 * c_info - pointer to acs_chaninfo_t for an interface
 *
 * Returns TRUE when BGDFS attempt is success in ETSI;
 * FALSE otherwise.
 */
bool
acs_bgdfs_attempt_on_txfail(acs_chaninfo_t * c_info)
{
	int ret = BCME_OK;
	chanspec_t chspec = 0;
	if (ACS_11H_AND_BGDFS(c_info) &&
			!c_info->cur_is_dfs &&
			c_info->country_is_edcrs_eu) {
		acs_dfsr_set_reentry_type(ACS_DFSR_CTX(c_info), DFS_REENTRY_IMMEDIATE);
		acs_select_chspec(c_info);
		chspec = c_info->selected_chspec;
		ACSD_INFO("%s Selected chan 0x%x for attempting bgdfs\n", c_info->name, chspec);
		if (chspec) {
			ret = acs_bgdfs_attempt(c_info, chspec, FALSE);
			if (ret != BCME_OK) {
				ACSD_ERROR("Failed bgdfs on %x\n", chspec);
				return FALSE;
			}
			c_info->acs_bgdfs->acs_bgdfs_on_txfail = TRUE;
			return TRUE;
		}
	}
	return FALSE;
}

void
acs_process_cmd(acs_chaninfo_t * c_info, chanspec_t chspec, int dfs_ap_move)
{
	int ret = 0;
	wl_chan_change_reason_t reason;

	reason = (wl_chan_change_reason_t)dfs_ap_move;

	c_info->selected_chspec = chspec;
	c_info->cur_chspec = chspec;
	acs_set_chspec(c_info, FALSE, dfs_ap_move);

	/* No need to update the driver for reason DFS_AP_MOVE stop and
	 * stunt operation as it can stop running dfs cac state machine
	 * for stunt operation.
	 */
	if ((reason != WL_CHAN_REASON_DFS_AP_MOVE_RADAR_FOUND) &&
		(reason != WL_CHAN_REASON_DFS_AP_MOVE_STUNT) &&
		(reason != WL_CHAN_REASON_DFS_AP_MOVE_ABORTED)) {

		ret = acs_update_driver(c_info);

		if (ret)
			ACSD_ERROR("update driver failed\n");
	}
	ACSD_DEBUG("ifname %s - mode: %s\n", c_info->name,
		AUTOCHANNEL(c_info)? "SELECT" :
		COEXCHECK(c_info)? "COEXCHECK" :
		ACS11H(c_info)? "11H" : "MONITOR");

	chanim_upd_acs_record(c_info->chanim_info,
		c_info->selected_chspec, APCS_IOCTL);
}

/*
 * acs_upgrade_to160 - upgrade to 160Mhz BW
 *
 * c_info - pointer to acs_chaninfo_t for an interface
 *
 * Returns BCME_OK when successful; error status otherwise.
 */
int
acs_upgrade_to160(acs_chaninfo_t * c_info)
{
	int ret;
	if ((ret = acs_bgdfs_choose_channel(c_info, TRUE, TRUE)) != BCME_OK) {
		ACSD_ERROR("%s Picking a 160Mhz channel failed\n", c_info->name);
		return BCME_ERROR;
	}
	return BCME_OK;
}

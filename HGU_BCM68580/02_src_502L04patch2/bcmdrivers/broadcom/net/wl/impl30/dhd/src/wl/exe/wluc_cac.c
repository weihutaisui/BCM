/*
 * wl cac command module
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
 * $Id: wluc_cac.c 458728 2014-02-27 18:15:25Z $
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

#include <proto/802.11e.h>
#include <bcmutils.h>
#include <bcmendian.h>
#include "wlu_common.h"
#include "wlu.h"

static cmd_func_t wl_cac, wl_tslist, wl_tspec, wl_tslist_ea, wl_tspec_ea, wl_cac_delts_ea;
static void wl_cac_addts_usage(void);
static void wl_cac_delts_usage(void);
static void wl_print_tspec(tspec_arg_t *ts);

static cmd_t wl_cac_cmds[] = {
	{ "cac_addts", wl_cac, -1, WLC_SET_VAR,
	"add TSPEC, error if STA is not associated or WME is not enabled\n"
	"\targ: TSPEC parameter input list"},
	{ "cac_delts", wl_cac, -1, WLC_SET_VAR,
	"delete TSPEC, error if STA is not associated or WME is not enabled\n"
	"\targ: TSINFO for the target tspec"},
	{ "cac_delts_ea", wl_cac_delts_ea, -1, WLC_SET_VAR,
	"delete TSPEC, error if STA is not associated or WME is not enabled\n"
	"\targ1: Desired TSINFO for the target tspec\n"
	"\targ2: Desired MAC address"},
	{ "cac_tslist", wl_tslist, WLC_GET_VAR, -1,
	"Get the list of TSINFO in driver\n"
	"\teg. 'wl cac_tslist' get a list of TSINFO"},
	{ "cac_tslist_ea", wl_tslist_ea, WLC_GET_VAR, -1,
	"Get the list of TSINFO for given STA in driver\n"
	"\teg. 'wl cac_tslist_ea ea' get a list of TSINFO"},
	{ "cac_tspec", wl_tspec, WLC_GET_VAR, -1,
	"Get specific TSPEC with matching TSINFO\n"
	"\teg. 'wl cac_tspec 0xaa 0xbb 0xcc' where 0xaa 0xbb & 0xcc are TSINFO octets"},
	{ "cac_tspec_ea", wl_tspec_ea, WLC_GET_VAR, -1,
	"Get specific TSPEC for given STA with matching TSINFO\n"
	"\teg. 'wl cac_tspec 0xaa 0xbb 0xcc xx:xx:xx:xx:xx:xx'\n"
	"\t    where 0xaa 0xbb & 0xcc are TSINFO octets and xx is mac address"},
	{ NULL, NULL, 0, 0, NULL }
};

static char *buf;

/* module initialization */
void
wluc_cac_module_init(void)
{
	/* get the global buf */
	buf = wl_get_buf();

	/* register cac commands */
	wl_module_cmds_register(wl_cac_cmds);
}

#define NUM_TSLIST_ARG 3		/* minimum number of arguments required for TSLIST */
#define NUM_TSLIST_PER_EA_ARG 3	/* minimum number of arguments required for TSLIST */
#define MIN_NUM_DELTS_ARG 4		/* minimum number of arguments required for DELTS */
#define MIN_NUM_DELTS_EA_ARG 5	/* minimum number of arguments required for DELTS */
#define MIN_NUM_ADDTS_ARG 20	/* minimum number of arguments required for ADDTS */
#define PERIODIC_TRAFFIC 1		/* Periodic traffic type */
#define VO_TID (0 << 1)			/* voice TID */
#define VI_TID (1 << 1)			/* signal TID */
#define UPLINK_DIRECTION (0 << 5)	/* uplink direction traffic stream */
#define DOWNLINK_DIRECTION (1 << 5)	/* downlink direction traffic stream */
#define BI_DIRECTION (3 << 5)	/* bi direction traffic stream */
#define EDCA_ACCESS (1 << 7)	/* EDCA access policy */
#define UAPSD_PSB (1 << 2)		/* U-APSD power saving behavior */
#define VO_USER_PRIO (6 << 3)	/* voice user priority */
#define VI_USER_PRIO (4 << 3)	/* signal user priority */
#define TID_SHIFT 1				/* TID Shift */
#define UP_SHIFT 3				/* UP Shift */

static void
wl_cac_format_tspec_htod(tspec_arg_t *tspec_arg)
{
	tspec_arg->version = htod16(tspec_arg->version);
	tspec_arg->length = htod16(tspec_arg->length);
	tspec_arg->flag = htod32(tspec_arg->flag);
	tspec_arg->nom_msdu_size = htod16(tspec_arg->nom_msdu_size);
	tspec_arg->max_msdu_size = htod16(tspec_arg->max_msdu_size);
	tspec_arg->min_srv_interval = htod32(tspec_arg->min_srv_interval);
	tspec_arg->max_srv_interval = htod32(tspec_arg->max_srv_interval);
	tspec_arg->inactivity_interval = htod32(tspec_arg->inactivity_interval);
	tspec_arg->suspension_interval = htod32(tspec_arg->suspension_interval);
	tspec_arg->srv_start_time = htod32(tspec_arg->srv_start_time);
	tspec_arg->min_data_rate = htod32(tspec_arg->min_data_rate);
	tspec_arg->mean_data_rate = htod32(tspec_arg->mean_data_rate);
	tspec_arg->peak_data_rate = htod32(tspec_arg->peak_data_rate);
	tspec_arg->max_burst_size = htod32(tspec_arg->max_burst_size);
	tspec_arg->delay_bound = htod32(tspec_arg->delay_bound);
	tspec_arg->min_phy_rate = htod32(tspec_arg->min_phy_rate);
	tspec_arg->surplus_bw = htod16(tspec_arg->surplus_bw);
	tspec_arg->medium_time = htod16(tspec_arg->medium_time);
}

static void
wl_cac_format_tspec_dtoh(tspec_arg_t *tspec_arg)
{
	tspec_arg->version = dtoh16(tspec_arg->version);
	tspec_arg->length = dtoh16(tspec_arg->length);
	tspec_arg->flag = dtoh32(tspec_arg->flag);
	tspec_arg->nom_msdu_size = dtoh16(tspec_arg->nom_msdu_size);
	tspec_arg->max_msdu_size = dtoh16(tspec_arg->max_msdu_size);
	tspec_arg->min_srv_interval = dtoh32(tspec_arg->min_srv_interval);
	tspec_arg->max_srv_interval = dtoh32(tspec_arg->max_srv_interval);
	tspec_arg->inactivity_interval = dtoh32(tspec_arg->inactivity_interval);
	tspec_arg->suspension_interval = dtoh32(tspec_arg->suspension_interval);
	tspec_arg->srv_start_time = dtoh32(tspec_arg->srv_start_time);
	tspec_arg->min_data_rate = dtoh32(tspec_arg->min_data_rate);
	tspec_arg->mean_data_rate = dtoh32(tspec_arg->mean_data_rate);
	tspec_arg->peak_data_rate = dtoh32(tspec_arg->peak_data_rate);
	tspec_arg->max_burst_size = dtoh32(tspec_arg->max_burst_size);
	tspec_arg->delay_bound = dtoh32(tspec_arg->delay_bound);
	tspec_arg->min_phy_rate = dtoh32(tspec_arg->min_phy_rate);
	tspec_arg->surplus_bw = dtoh16(tspec_arg->surplus_bw);
	tspec_arg->medium_time = dtoh16(tspec_arg->medium_time);

}

static void wl_cac_addts_usage(void)
{
	fprintf(stderr, "Too few arguments\n");
	fprintf(stderr, "wl cac_addts ver dtoken tid dir psb up a b c d e ...\n");
	fprintf(stderr, "\twhere ver is the structure version\n");
	fprintf(stderr, "\twhere dtoken is the dialog token [range 1-255]\n");
	fprintf(stderr, "\twhere tid is the tspec identifier [range 0-7]\n");
	fprintf(stderr, "\twhere dir is direction [uplink | downlink | bi-directional]\n");
	fprintf(stderr, "\twhere psb is power save mode [legacy|U-APSD]\n");
	fprintf(stderr, "\twhere up is user priority [range 0-7]\n");
	fprintf(stderr, "\twhere a is the nominal MSDU size\n");
	fprintf(stderr, "\twhere b is bool for fixed size msdu [ 0 and 1]\n");
	fprintf(stderr, "\twhere c is the maximum MSDU size\n");
	fprintf(stderr, "\twhere d is the minimum service interval\n");
	fprintf(stderr, "\twhere e is the maximum service interval\n");
	fprintf(stderr, "\twhere f is the inactivity interval\n");
	fprintf(stderr, "\twhere g is the suspension interval\n");
	fprintf(stderr, "\twhere h is the minimum data rate\n");
	fprintf(stderr, "\twhere i is the mean data rate\n");
	fprintf(stderr, "\twhere j is the peak data rate\n");
	fprintf(stderr, "\twhere k is the max burst size\n");
	fprintf(stderr, "\twhere l is the delay bound\n");
	fprintf(stderr, "\twhere m is the surplus bandwidth [fixed point notation]\n");
	fprintf(stderr, "\twhere n is the minimum PHY rate\n");
}

static void wl_cac_delts_usage(void)
{
	fprintf(stderr, "Too few arguments\n");
	fprintf(stderr, "wl cac_delts ver a b c \n");
	fprintf(stderr, "\twhere ver is the tspec version\n");
	fprintf(stderr, "\twhere a is byte[0] of tsinfo (bits 0-7)\n");
	fprintf(stderr, "\twhere b is byte[1] of tsinfo (bits 8-15)\n");
	fprintf(stderr, "\twhere c is byte[2] of tsinfo (bits 16-23)\n");
}

static int
wl_cac(void *wl, cmd_t *cmd, char **argv)
{
	int err = -1;
	int ap_mode = 0;
	int apsta_mode = 0;
	int cmd_type = 0;
	tspec_arg_t tspec_arg;
	char *endptr = NULL;
	uint buflen;
	char *arg1, *user_argv;
	uint8 direction = BI_DIRECTION;
	uint8 user_tid, user_prio, user_psb;
	uint fixed;

	if ((err = wlu_iovar_get(wl, "apsta", &apsta_mode, sizeof(apsta_mode))))
		return err;

	if (!apsta_mode) {
		if ((err = wlu_get(wl, WLC_GET_AP, &ap_mode, sizeof(ap_mode))))
			return err;
		else {
			if (dtoh32(ap_mode)) {
				fprintf(stderr,
					"This command can ONLY be executed on a STA or APSTA\n");
				return err;
			}
		}
	}

	if (!strcmp(*argv, "cac_addts"))
		cmd_type = 1;
	else if (!strcmp(*argv, "cac_delts"))
		cmd_type = 2;
	else {
		fprintf(stderr, "unknown command\n");
		return BCME_USAGE_ERROR;
	}

	/* eat command name */
	if (!*++argv) {
		(cmd_type == 1) ? wl_cac_addts_usage():wl_cac_delts_usage();
		return BCME_BADARG;
	}

	buflen = sizeof(tspec_arg_t);
	memset((uint8 *)&tspec_arg, 0, buflen);

	/* get direction option */
	arg1 = *argv;

	/* Unidirectional DL/UL */
	if (!strcmp(arg1, "UDL") || (!strcmp(arg1, "UUL")))
		direction = DOWNLINK_DIRECTION;

	if (cmd_type == 1) {
		uint argc = 0;

		/* arg count */
		while (argv[argc])
			argc++;

		/* required argments */
		if (argc < MIN_NUM_ADDTS_ARG) {
			wl_cac_addts_usage();
			return BCME_USAGE_ERROR;
		}

		tspec_arg.length = sizeof(tspec_arg_t) - (2 * sizeof(uint16));
		tspec_arg.version = (uint16)strtol(*argv++, &endptr, 0);
		if (*endptr != '\0')
			return BCME_USAGE_ERROR;

		tspec_arg.dialog_token = (uint8)strtol(*argv++, &endptr, 0);
		if (*endptr != '\0')
			return BCME_USAGE_ERROR;

		user_tid = (uint8)strtol(*argv++, &endptr, 0);
		user_tid <<= TID_SHIFT;
		if (*endptr != '\0')
			return BCME_USAGE_ERROR;

		/* store the pointer for parsing */
		user_argv = *argv++;

		if (!strcmp(user_argv, "uplink"))
			direction = UPLINK_DIRECTION;
		else if (!strcmp(user_argv, "downlink"))
			direction = DOWNLINK_DIRECTION;
		else if (!strcmp(user_argv, "bi-directional"))
			direction = BI_DIRECTION;
		else
			return BCME_USAGE_ERROR;

		/* store the pointer for parsing */
		user_argv = *argv++;

		if (!strcmp(user_argv, "legacy"))
			user_psb = 0;
		else if (!strcmp(user_argv, "U-APSD"))
			user_psb = UAPSD_PSB;
		else
			return BCME_USAGE_ERROR;

		user_prio = (uint8)strtol(*argv++, &endptr, 0);
		user_prio <<= UP_SHIFT;
		if (*endptr != '\0')
			return BCME_USAGE_ERROR;

		tspec_arg.tsinfo.octets[0] = (uint8)(user_tid |
			direction | EDCA_ACCESS);

		tspec_arg.tsinfo.octets[1] = (uint8)(user_prio | user_psb);
		tspec_arg.tsinfo.octets[2] = 0x00;

		tspec_arg.nom_msdu_size = (uint16)strtol(*argv++, &endptr, 0);
		if (*endptr != '\0')
			return BCME_USAGE_ERROR;

		fixed = (uint)strtol(*argv++, &endptr, 0);
		if (*endptr != '\0')
			return BCME_USAGE_ERROR;

		if (fixed == 1)
			tspec_arg.nom_msdu_size |= 0x8000;

		tspec_arg.max_msdu_size = (uint16)strtol(*argv++, &endptr, 0);
		if (*endptr != '\0')
			return BCME_USAGE_ERROR;

		tspec_arg.min_srv_interval = strtol(*argv++, &endptr, 0);
		if (*endptr != '\0')
			return BCME_USAGE_ERROR;

		tspec_arg.max_srv_interval = strtol(*argv++, &endptr, 0);
		if (*endptr != '\0')
			return BCME_USAGE_ERROR;

		tspec_arg.inactivity_interval = strtol(*argv++, &endptr, 0);
		if (*endptr != '\0')
			return BCME_USAGE_ERROR;

		tspec_arg.suspension_interval = strtol(*argv++, &endptr, 0);
		if (*endptr != '\0')
			return BCME_USAGE_ERROR;

		tspec_arg.min_data_rate = strtol(*argv++, &endptr, 0);
		if (*endptr != '\0')
			return BCME_USAGE_ERROR;

		tspec_arg.mean_data_rate = strtol(*argv++, &endptr, 0);
		if (*endptr != '\0')
			return BCME_USAGE_ERROR;

		tspec_arg.peak_data_rate = strtol(*argv++, &endptr, 0);
		if (*endptr != '\0')
			return BCME_USAGE_ERROR;

		tspec_arg.max_burst_size = strtol(*argv++, &endptr, 0);
		if (*endptr != '\0')
			return BCME_USAGE_ERROR;

		tspec_arg.delay_bound = strtol(*argv++, &endptr, 0);
		if (*endptr != '\0')
			return BCME_USAGE_ERROR;

		tspec_arg.surplus_bw = (uint16)strtol(*argv++, &endptr, 0);
		if (*endptr != '\0')
			return BCME_USAGE_ERROR;

		tspec_arg.min_phy_rate = strtol(*argv++, &endptr, 0);
		if (*endptr != '\0')
			return BCME_USAGE_ERROR;
		printf("Setting min_phy_rate to 0x%x\n", tspec_arg.min_phy_rate);
	} else {
		uint argc = 0;

		/* arg count */
		while (argv[argc])
			argc++;

		/* required argments */
		if (argc < MIN_NUM_DELTS_ARG) {
			wl_cac_delts_usage();
			return BCME_USAGE_ERROR;
		}

		tspec_arg.length = sizeof(tspec_arg_t) - (2 * sizeof(uint16));
		tspec_arg.version = (uint16)strtol(*argv++, &endptr, 0);
		if (*endptr != '\0')
			return BCME_USAGE_ERROR;

		tspec_arg.tsinfo.octets[0] = (uint8)strtol(*argv++, &endptr, 0);
		if (*endptr != '\0')
			return BCME_USAGE_ERROR;

		tspec_arg.tsinfo.octets[1] = (uint8)strtol(*argv++, &endptr, 0);
		if (*endptr != '\0')
			return BCME_USAGE_ERROR;

		tspec_arg.tsinfo.octets[2] = (uint8)strtol(*argv++, &endptr, 0);
		if (*endptr != '\0')
			return BCME_USAGE_ERROR;
	}

	wl_cac_format_tspec_htod(&tspec_arg);
	err = wlu_var_setbuf(wl, cmd->name, &tspec_arg, buflen);

	return err;
}

/* get a list of traffic stream (TSINFO) in driver */
static int
wl_tslist(void *wl, cmd_t *cmd, char **argv)
{
	void *ptr;
	int i;
	int ap_mode, err = -1;
	int apsta_mode = 0;
	struct tslist *tslist;

	UNUSED_PARAMETER(argv);

	if ((err = wlu_iovar_get(wl, "apsta", &apsta_mode, sizeof(apsta_mode))))
		return err;

	if (!apsta_mode) {
		if ((err = wlu_get(wl, WLC_GET_AP, &ap_mode, sizeof(ap_mode))))
			return err;
		else {
			if (dtoh32(ap_mode)) {
				fprintf(stderr,
					"This command can ONLY be executed on a STA or APSTA\n");
				return err;
			}
		}
	}

	if ((err = wlu_var_getbuf(wl, cmd->name, NULL, 0, &ptr)) < 0)
		return err;

	tslist = (struct tslist *)ptr;
	tslist->count = dtoh32(tslist->count);
	for (i = 0; i < tslist->count; i++)
		printf("tsinfo 0x%02X 0x%02X 0x%02X  TID %d  User Prio %d  Direction %d\n",
		       tslist->tsinfo[i].octets[0],
		       tslist->tsinfo[i].octets[1],
		       tslist->tsinfo[i].octets[2],
		       WLC_CAC_GET_TID(tslist->tsinfo[i]),
		       WLC_CAC_GET_USER_PRIO(tslist->tsinfo[i]),
		       WLC_CAC_GET_DIR(tslist->tsinfo[i]));

	return 0;
}

/* get specific TSPEC in driver */
static int
wl_tspec(void *wl, cmd_t *cmd, char **argv)
{
	void *ptr;
	int ap_mode, err = -1;
	tspec_arg_t *ts, tspec_arg;
	char *temp = NULL;
	uint argc = 0;

	if ((wlu_get(wl, WLC_GET_AP, &ap_mode, sizeof(ap_mode))))
		return err;
	ap_mode = dtoh32(ap_mode);

	if (ap_mode) {
		fprintf(stderr, "This command can only be executed on the STA\n");
		return err;
	}

	/* eat command name */
	argv++;

	/* arg count */
	while (argv[argc])
		argc++;

	/* required argments */
	if (argc < NUM_TSLIST_ARG) {
		fprintf(stderr, "Too few arguments\n");
		fprintf(stderr, "wl cac_tspec 0xaa 0xbb 0xcc \n");
		fprintf(stderr, "\twhere 0xaa is byte[0] of tsinfo (bits 0-7)\n");
		fprintf(stderr, "\twhere 0xbb is byte[1] of tsinfo (bits 8-15)\n");
		fprintf(stderr, "\twhere 0xcc is byte[2] of tsinfo (bits 16-23)\n");
		return BCME_USAGE_ERROR;
	}

	memset((uint8 *)&tspec_arg, 0, sizeof(tspec_arg_t));

	tspec_arg.tsinfo.octets[0] = (uint8)strtol(*argv++, &temp, 0);
	if (*temp != '\0')
		return BCME_USAGE_ERROR;

	tspec_arg.tsinfo.octets[1] = (uint8)strtol(*argv++, &temp, 0);
	if (*temp != '\0')
		return BCME_USAGE_ERROR;

	tspec_arg.tsinfo.octets[2] = (uint8)strtol(*argv++, &temp, 0);
	if (*temp != '\0')
		return BCME_USAGE_ERROR;
	if ((err = wlu_var_getbuf(wl, cmd->name, &tspec_arg, sizeof(tspec_arg_t), &ptr)) < 0)
		return err;

	ts = (tspec_arg_t *)ptr;
	wl_cac_format_tspec_dtoh(ts);
	wl_print_tspec(ts);

	return 0;
}

/* get the tspec list for the given station */
static int
wl_tslist_ea(void *wl, cmd_t *cmd, char **argv)
{
	void *ptr;
	int i;
	int ap_mode, err = -1;
	struct tslist *tslist;
	scb_val_t scb_val;

	if (!*++argv) {
		printf("MAC address must be specified\n");
		return BCME_USAGE_ERROR;
	} else if (!wl_ether_atoe(*argv, &scb_val.ea)) {
		printf("Malformed MAC address parameter\n");
		return BCME_USAGE_ERROR;
	}
	if ((err = wlu_get(wl, WLC_GET_AP, &ap_mode, sizeof(ap_mode))))
		return err;

	ap_mode = dtoh32(ap_mode);

	if ((err = wlu_var_getbuf(wl, cmd->name, &scb_val.ea, ETHER_ADDR_LEN, &ptr)) < 0)
		return err;

	tslist = (struct tslist *)ptr;
#ifdef DSLCPE_ENDIAN
	tslist->count = dtoh32(tslist->count);
#endif
	for (i = 0; i < tslist->count; i++)
		printf("tsinfo 0x%02X 0x%02X 0x%02X  TID %d  User Prio %d  Direction %d\n",
		       tslist->tsinfo[i].octets[0],
		       tslist->tsinfo[i].octets[1],
		       tslist->tsinfo[i].octets[2],
		       WLC_CAC_GET_TID(tslist->tsinfo[i]),
		       WLC_CAC_GET_USER_PRIO(tslist->tsinfo[i]),
		       WLC_CAC_GET_DIR(tslist->tsinfo[i]));

	return 0;

}

/* get specific TSPEC for a STA */
static int
wl_tspec_ea(void *wl, cmd_t *cmd, char **argv)
{
	void *ptr;
	int err = -1;
	tspec_per_sta_arg_t tsea;
	tspec_arg_t *ts;
	char *temp;
	uint argc = 0;

	/* eat command name */
	argv++;

	while (argv[argc])
		argc++;

	/* required argments */
	if (argc < (NUM_TSLIST_PER_EA_ARG + 1)) {
		fprintf(stderr, "Too few arguments\n");
		fprintf(stderr, "wl cac_tspec 0xaa 0xbb 0xcc xx:xx:xx:xx:xx:xx\n");
		fprintf(stderr, "\twhere 0xaa is byte[0] of tsinfo (bits 0-7)\n");
		fprintf(stderr, "\twhere 0xbb is byte[1] of tsinfo (bits 8-15)\n");
		fprintf(stderr, "\twhere 0xcc is byte[2] of tsinfo (bits 16-23)\n");
		fprintf(stderr, "\twhere xx:xx:xx:xx:xx:xx is mac address )\n");
		return BCME_USAGE_ERROR;
	}

	memset((uint8 *)&tsea, 0, sizeof(tspec_per_sta_arg_t));

	ts = &tsea.ts;

	ts->tsinfo.octets[0] = (uint8)strtol(*argv++, &temp, 0);
	if (*temp != '\0')
		return BCME_USAGE_ERROR;

	ts->tsinfo.octets[1] = (uint8)strtol(*argv++, &temp, 0);
	if (*temp != '\0')
		return BCME_USAGE_ERROR;

	ts->tsinfo.octets[2] = (uint8)strtol(*argv++, &temp, 0);
	if (*temp != '\0')
		return BCME_USAGE_ERROR;

	/* add the ether address after tsinfo */
	if (!*argv) {
		printf("MAC address must be specified\n");
		return BCME_USAGE_ERROR;
	} else if (!wl_ether_atoe(*argv, &tsea.ea)) {
		printf("Malformed MAC address parameter\n");
		return BCME_USAGE_ERROR;
	}

	if ((err = wlu_var_getbuf(wl, cmd->name, &tsea, sizeof(tspec_per_sta_arg_t), &ptr)) < 0)
		return err;

	ts = (tspec_arg_t *)ptr;
	wl_cac_format_tspec_dtoh(ts);
	wl_print_tspec(ts);
	return 0;

}

static const uint8 wlu_wme_fifo2ac[] = { AC_BK, AC_BE, AC_VI, AC_VO, AC_BE,
             AC_BE };
static const uint8 wlu_prio2fifo[NUMPRIO] = {
	0,	/* 0	BE	AC_BE	Best Effort */
	1,	/* 1	BK	AC_BK	Background */
	2,	/* 2	--	AC_BK	Background */
	3,	/* 3	EE	AC_BE	Best Effort */
	4,	/* 4	CL	AC_VI	Video */
	5,	/* 5	VI	AC_VI	Video */
	6,	/* 6	VO	AC_VO	Voice */
	7	/* 7	NC	AC_VO	Voice */
};
#define WME_PRIO2AC(prio)	wlu_wme_fifo2ac[wlu_prio2fifo[(prio)]]

static void
wl_print_tspec(tspec_arg_t *ts)
{
	const char *str;
	if (ts->version != TSPEC_ARG_VERSION) {
		printf("\tIncorrect version of TSPEC struct: expected %d; got %d\n",
		       TSPEC_ARG_VERSION, ts->version);
		return;
	}

	if (ts->length < (sizeof(tspec_arg_t) - (2 * sizeof(uint16)))) {
		printf("\tTSPEC arg length too short: expected %d; got %d\n",
		       (int)(sizeof(tspec_arg_t) - (2 * sizeof(uint16))), ts->length);
		return;
	}

	switch (ts->flag & TSPEC_STATUS_MASK) {
		case TSPEC_PENDING:
			str = "PENDING";
			break;
		case TSPEC_ACCEPTED:
			str = "ACCEPTED";
			break;
		case TSPEC_REJECTED:
			str = "REJECTED";
			break;
		default:
			str = "UNKNOWN";
			break;
	}

	printf("version %d\n", ts->version);
	printf("length %d\n", ts->length);

	printf("TID %d %s\n", WLC_CAC_GET_TID(ts->tsinfo), str);
	printf("tsinfo 0x%02x 0x%02x 0x%02x\n", ts->tsinfo.octets[0],
	       ts->tsinfo.octets[1], ts->tsinfo.octets[2]);

	/* breakout bitfields for apsd */
	if (WLC_CAC_GET_PSB(ts->tsinfo)) {
		int ac = WME_PRIO2AC(WLC_CAC_GET_USER_PRIO(ts->tsinfo));
		switch (WLC_CAC_GET_DIR(ts->tsinfo)) {
			case (TS_INFO_UPLINK >> TS_INFO_DIRECTION_SHIFT):
				printf("AC[%d] : Trigger enabled\n", ac);
				break;

			case (TS_INFO_DOWNLINK >> TS_INFO_DIRECTION_SHIFT):
				printf("AC[%d] : Delivery enabled\n", ac);
				break;

			case (TS_INFO_BIDIRECTIONAL >>
				TS_INFO_DIRECTION_SHIFT):
				printf("AC[%d] : Trig & Delv enabled\n", ac);
				break;
		}
	} else {
		int ac;
		ac = WME_PRIO2AC(WLC_CAC_GET_USER_PRIO(ts->tsinfo));
		printf("AC [%d] : Legacy Power save\n", ac);
	}

	printf("nom_msdu_size %d %s\n", (ts->nom_msdu_size & 0x7fff),
	       ((ts->nom_msdu_size & 0x8000) ? "fixed size" : ""));
	printf("max_msdu_size %d\n", ts->max_msdu_size);
	printf("min_srv_interval %d\n", ts->min_srv_interval);
	printf("max_srv_interval %d\n", ts->max_srv_interval);
	printf("inactivity_interval %d\n", ts->inactivity_interval);
	printf("suspension_interval %d\n", ts->suspension_interval);
	printf("srv_start_time %d\n", ts->srv_start_time);
	printf("min_data_rate %d\n", ts->min_data_rate);
	printf("mean_data_rate %d\n", ts->mean_data_rate);
	printf("peak_data_rate %d\n", ts->peak_data_rate);
	printf("max_burst_size %d\n", ts->max_burst_size);
	printf("delay_bound %d\n", ts->delay_bound);
	printf("min_phy_rate %d\n", ts->min_phy_rate);
	printf("surplus_bw %d\n", ts->surplus_bw);
	printf("medium_time %d\n", ts->medium_time);
}

/* send delts for a specific ea */
/* TODO : Club this with wl_tspec_ea */
static int
wl_cac_delts_ea(void *wl, cmd_t *cmd, char **argv)
{
	void *ptr;
	int err = -1;
	char *endptr = NULL;
	tspec_per_sta_arg_t tsea;
	tspec_arg_t *ts;
	uint argc = 0;

	/* eat command name */
	argv++;

	while (argv[argc])
		argc++;

	/* required argments */
	if (argc < (NUM_TSLIST_PER_EA_ARG + 1)) {
		fprintf(stderr, "Too few arguments\n");
		fprintf(stderr, "wl cac_delts_ea ver 0xaa 0xbb 0xcc xx:xx:xx:xx:xx:xx\n");
		fprintf(stderr, "\twhere ver is the tspec version\n");
		fprintf(stderr, "\twhere 0xaa is byte[0] of tsinfo (bits 0-7)\n");
		fprintf(stderr, "\twhere 0xbb is byte[1] of tsinfo (bits 8-15)\n");
		fprintf(stderr, "\twhere 0xcc is byte[2] of tsinfo (bits 16-23)\n");
		fprintf(stderr, "\twhere xx:xx:xx:xx:xx:xx is mac address )\n");
		return BCME_USAGE_ERROR;
	}

	memset((uint8 *)&tsea, 0, sizeof(tspec_per_sta_arg_t));

	ts = &tsea.ts;

	ts->length = sizeof(tspec_arg_t) - (2 * sizeof(uint16));
	ts->version = (uint16)strtol(*argv++, &endptr, 0);

	if (*endptr != '\0')
		return BCME_USAGE_ERROR;

	ts->tsinfo.octets[0] = (uint8)strtol(*argv++, &endptr, 0);
	if (*endptr != '\0')
		return BCME_USAGE_ERROR;

	ts->tsinfo.octets[1] = (uint8)strtol(*argv++, &endptr, 0);
	if (*endptr != '\0')
		return BCME_USAGE_ERROR;

	ts->tsinfo.octets[2] = (uint8)strtol(*argv++, &endptr, 0);
	if (*endptr != '\0')
		return BCME_USAGE_ERROR;

	/* add the ether address after tsinfo */
	if (!*argv) {
		printf("MAC address must be specified\n");
		return BCME_USAGE_ERROR;
	} else if (!wl_ether_atoe(*argv, &tsea.ea)) {
		printf("Malformed MAC address parameter\n");
		return BCME_USAGE_ERROR;
	}

	wl_cac_format_tspec_htod(ts);
	if ((err = wlu_var_getbuf(wl, cmd->name, &tsea, sizeof(tspec_per_sta_arg_t), &ptr)) < 0)
		return err;

	return 0;

}

/*
 * wl ampdu command module
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
 * $Id: wluc_ampdu.c 458728 2014-02-27 18:15:25Z $
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

static cmd_func_t wl_ampdu_tid;
static cmd_func_t wl_ampdu_aggr;
static cmd_func_t wl_ampdu_retry_limit_tid;
static cmd_func_t wl_ampdu_rr_retry_limit_tid;
static cmd_func_t wl_ampdu_send_addba;
static cmd_func_t wl_ampdu_send_delba;

static cmd_t wl_ampdu_cmds[] = {
	{ "ampdu_tid", wl_ampdu_tid, WLC_GET_VAR, WLC_SET_VAR,
	"enable/disable per-tid ampdu; usage: wl ampdu_tid <tid> [0/1]" },
	{ "ampdu_txaggr", wl_ampdu_aggr, WLC_GET_VAR, WLC_SET_VAR,
	"enable/disable tx aggregation per tid or all tid for specific interface;\n"
	"\tget current status: wl ampdu_txaggr\n"
	"\tenable/disable all category(tid): wl ampdu_txaggr <0/1>\n"
	"\tenable/disable per category(tid): wl ampdu_txaggr [<tid> <0/1>]"},
	{ "ampdu_rxaggr", wl_ampdu_aggr, WLC_GET_VAR, WLC_SET_VAR,
	"enable/disable rx aggregation per tid or all tid for specific interface;\n"
	"\tget current status: wl ampdu_rxaggr\n"
	"\tenable/disable all category(tid): wl ampdu_rxaggr <0/1>\n"
	"\tenable/disable per category(tid): wl ampdu_rxaggr [<tid> <0/1>]"},
	{ "ampdu_retry_limit_tid", wl_ampdu_retry_limit_tid, WLC_GET_VAR, WLC_SET_VAR,
	"Set per-tid ampdu retry limit; usage: wl ampdu_retry_limit_tid <tid> [0~31]" },
	{ "ampdu_rr_retry_limit_tid", wl_ampdu_rr_retry_limit_tid, WLC_GET_VAR, WLC_SET_VAR,
	"Set per-tid ampdu regular rate retry limit; usage: "
	"wl ampdu_rr_retry_limit_tid <tid> [0~31]" },
	{ "ampdu_send_addba", wl_ampdu_send_addba, WLC_GET_VAR, WLC_SET_VAR,
	"send addba to specified ea-tid; usage: wl ampdu_send_addba <tid> <ea>" },
	{ "ampdu_send_delba", wl_ampdu_send_delba, WLC_GET_VAR, WLC_SET_VAR,
	"send delba to specified ea-tid; usage: wl ampdu_send_delba <tid> <ea> [initiator]" },
	{ "ampdu_txq_prof_start", wl_var_void, -1, WLC_SET_VAR,
	"start sample txq profiling data"},
	{ "ampdu_txq_prof_dump", wl_var_void, -1, WLC_SET_VAR,
	"show txq histogram"},
	{ "ampdu_txq_ss", wl_var_void, -1, WLC_SET_VAR,
	"take txq snapshot"},
	{ NULL, NULL, 0, 0, NULL }
};

static char *buf;

/* module initialization */
void
wluc_ampdu_module_init(void)
{
	(void)g_swap;

	/* get the global buf */
	buf = wl_get_buf();

	/* register ampdu commands */
	wl_module_cmds_register(wl_ampdu_cmds);
}

static int
wl_ampdu_tid(void *wl, cmd_t *cmd, char **argv)
{
	char *param;
	const char *cmdname = "ampdu_tid";
	struct ampdu_tid_control atc, *reply;
	uint8 tid;
	int err;
	void *ptr = NULL;

	UNUSED_PARAMETER(cmd);

	if ((param = *++argv) == NULL)
		return BCME_USAGE_ERROR;

	tid = atoi(param);
	if (tid > MAXPRIO)
		return BCME_USAGE_ERROR;
	atc.tid = tid;

	if ((param = *++argv)) {
		atc.enable = atoi(param);
		err = wlu_var_setbuf(wl, cmdname, &atc, sizeof(atc));
	} else {
		if ((err = wlu_var_getbuf_sm(wl, cmdname, &atc, sizeof(atc), &ptr) < 0))
			return err;
		reply = (struct ampdu_tid_control *)ptr;
		printf("AMPDU for tid %d: %d\n", tid, reply->enable);
	}
	return err;
}

static int
wl_ampdu_aggr(void *wl, cmd_t *cmd, char **argv)
{
	struct ampdu_aggr aggr, *reply;
	int err;
	int idx;
	void *ptr = NULL;

	if (argv[1] == NULL) {
		/* get current status of aggregation */
		if ((err = wlu_var_getbuf_sm(wl, cmd->name, &aggr, sizeof(aggr), &ptr) < 0)) {
			return err;
		}
		reply = ptr;
		printf("%s_override: %s\n", cmd->name, (reply->aggr_override == AUTO) ? "AUTO" :
			((reply->aggr_override == ON) ? "ON" : "OFF"));
		for (idx = 0; idx < NUMPRIO; idx++) {
			printf("tid:%d status:%d\n", idx, isbitset(reply->enab_TID_bmap, idx));
		}
		return err;
	}
	memset(&aggr, 0, sizeof(aggr));
	if (argv[2] == NULL) {
		/* Set for all TID */
		bool enab = atoi(*++argv);
		aggr.enab_TID_bmap = enab ? NBITMASK(NUMPRIO) : 0;
		aggr.conf_TID_bmap = NBITMASK(NUMPRIO);
	} else {
		char *param;
		/* Set for specific TIDs */
		while ((param = *++argv) != NULL) {
			uint8 tid;
			bool enab;
			tid = atoi(param);
			if (tid > MAXPRIO) {
				return BCME_USAGE_ERROR;
			}
			if ((param = *++argv) == NULL) {
				return BCME_USAGE_ERROR;
			}
			enab = atoi(param);
			setbit(&aggr.conf_TID_bmap, tid);
			if (enab) {
				setbit(&aggr.enab_TID_bmap, tid);
			}
		}
	}
	err = wlu_var_setbuf(wl, cmd->name, &aggr, sizeof(aggr));
	return err;
}

static int
wl_ampdu_retry_limit_tid(void *wl, cmd_t *cmd, char **argv)
{
	char *param;
	const char *cmdname = "ampdu_retry_limit_tid";
	struct ampdu_retry_tid retry_limit, *reply;
	uint8 tid;
	int err;
	void *ptr = NULL;

	UNUSED_PARAMETER(cmd);

	if ((param = *++argv) == NULL)
		return BCME_USAGE_ERROR;

	tid = atoi(param);
	if (tid > MAXPRIO)
		return BCME_USAGE_ERROR;
	retry_limit.tid = tid;

	if ((param = *++argv)) {
		retry_limit.retry = atoi(param);
		err = wlu_var_setbuf(wl, cmdname, &retry_limit, sizeof(retry_limit));
	} else {
		if ((err = wlu_var_getbuf(wl, cmdname, &retry_limit,
			sizeof(retry_limit), &ptr)) < 0)
			return err;
		reply = (struct ampdu_retry_tid *)ptr;
		printf("AMPDU retry limit for tid %d: %d\n", tid, reply->retry);
	}
	return err;
}

static int
wl_ampdu_rr_retry_limit_tid(void *wl, cmd_t *cmd, char **argv)
{
	char *param;
	const char *cmdname = "ampdu_rr_retry_limit_tid";
	struct ampdu_retry_tid retry_limit, *reply;
	uint8 tid;
	int err;
	void *ptr = NULL;

	UNUSED_PARAMETER(cmd);

	if ((param = *++argv) == NULL)
		return BCME_USAGE_ERROR;

	tid = atoi(param);
	if (tid > MAXPRIO)
		return BCME_USAGE_ERROR;
	retry_limit.tid = tid;

	if ((param = *++argv)) {
		retry_limit.retry = atoi(param);
		err = wlu_var_setbuf(wl, cmdname, &retry_limit, sizeof(retry_limit));
	} else {
		if ((err = wlu_var_getbuf(wl, cmdname, &retry_limit,
			sizeof(retry_limit), &ptr)) < 0)
			return err;
		reply = (struct ampdu_retry_tid *)ptr;
		printf("AMPDU regular rate retry limit for tid %d: %d\n", tid, reply->retry);
	}
	return err;
}

static int
wl_ampdu_send_addba(void *wl, cmd_t *cmd, char **argv)
{
	char *param;
	const char *cmdname = "ampdu_send_addba";
	struct ampdu_ea_tid aet;
	uint8 tid;

	UNUSED_PARAMETER(cmd);

	if ((param = *++argv) == NULL)
		return BCME_USAGE_ERROR;

	tid = atoi(param);
	if (tid > MAXPRIO)
		return BCME_USAGE_ERROR;
	aet.tid = tid;

	argv++;
	if (!*argv) {
		printf("error: missing address\n");
		return BCME_USAGE_ERROR;
	}

	if (!wl_ether_atoe(*argv, &aet.ea)) {
		printf("error: could not parse MAC address %s\n", *argv);
		return BCME_USAGE_ERROR;
	}

	return wlu_var_setbuf(wl, cmdname, &aet, sizeof(aet));
}

static int
wl_ampdu_send_delba(void *wl, cmd_t *cmd, char **argv)
{
	char *param;
	const char *cmdname = "ampdu_send_delba";
	struct ampdu_ea_tid aet;
	uint8 tid;

	UNUSED_PARAMETER(cmd);

	if ((param = *++argv) == NULL)
		return BCME_USAGE_ERROR;

	tid = atoi(param);
	if (tid > MAXPRIO)
		return BCME_USAGE_ERROR;
	aet.tid = tid;

	argv++;
	if (!*argv) {
		printf("error: missing address\n");
		return BCME_USAGE_ERROR;
	}

	if (!wl_ether_atoe(*argv, &aet.ea)) {
		printf("error: could not parse MAC address %s\n", *argv);
		return BCME_USAGE_ERROR;
	}

	/* initiator (optional argument), 0 is recipient, 1 is originator */
	argv++;
	if (*argv) {
		aet.initiator = atoi(*argv);
	}
	else {
		/* default is originator */
		aet.initiator = TRUE;
	}

	return wlu_var_setbuf(wl, cmdname, &aet, sizeof(aet));
}

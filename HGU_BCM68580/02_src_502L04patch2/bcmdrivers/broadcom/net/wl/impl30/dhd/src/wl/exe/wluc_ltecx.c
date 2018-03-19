/*
 * wl ltecx command module
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
 * $Id: wluc_ltecx.c 458728 2014-02-27 18:15:25Z $
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

/* LTE coex funcs */
static cmd_func_t wl_wci2_config;
static cmd_func_t wl_mws_params;
static cmd_func_t wl_mws_wci2_msg;

static cmd_t wl_ltecx_cmds[] = {
	{ "wci2_config", wl_wci2_config, WLC_GET_VAR, WLC_SET_VAR,
	"Get/Set LTE coex MWS signaling config\n"
	"\tUsage: wl wci2_config <rxassert_off> <rxassert_jit> <rxdeassert_off> <rxdeassert_jit> "
	"<txassert_off> <txassert_jit> <txdeassert_off> <txdeassert_jit> "
	"<patassert_off> <patassert_jit> <inactassert_off> <inactassert_jit> "
	"<scanfreqassert_off> <scanfreqassert_jit> <priassert_off_req>"},
	{ "mws_params", wl_mws_params, WLC_GET_VAR, WLC_SET_VAR,
	"Get/Set LTE coex MWS channel params\n"
	"\tUsage: wl mws_params <rx_center_freq> <tx_center_freq> "
	"<rx_channel_bw> <tx_channel_bw> <channel_en> <channel_type>"},
	{ "mws_debug_msg", wl_mws_wci2_msg, WLC_GET_VAR, WLC_SET_VAR,
	"Get/Set LTE coex BT-SIG message\n"
	"\tUsage: wl mws_debug_msg <Message> <Interval 20us-32000us> "
	"<Repeats>"},
	{ NULL, NULL, 0, 0, NULL }
};

static char *buf;

/* module initialization */
void
wluc_ltecx_module_init(void)
{
	/* get the global buf */
	buf = wl_get_buf();

	/* register ltecx commands */
	wl_module_cmds_register(wl_ltecx_cmds);
}

static int
wl_wci2_config(void *wl, cmd_t *cmd, char **argv)
{
	uint32 val;
	char *endptr = NULL;
	uint argc;
	wci2_config_t wci2_config;
	uint16 *configp = (uint16 *)&wci2_config;
	int ret, i;

	UNUSED_PARAMETER(cmd);

	val = 0;

	/* eat command name */
	argv++;
	/* arg count */
	for (argc = 0; argv[argc]; argc++);

	memset(&wci2_config, '\0', sizeof(wci2_config_t));

	if (argc == 0) {
		/* Get and print the values */
		ret = wlu_iovar_getbuf(wl, "wci2_config", &wci2_config, sizeof(wci2_config_t),
		buf, WLC_IOCTL_SMLEN);
		if (ret)
			return ret;

		printf("rxassert_off %d rxassert_jit %d rxdeassert_off %d rxdeassert_jit %d "
			"txassert_off %d txassert_jit %d txdeassert_off %d txdeassert_jit %d "
			"patassert_off %d patassert_jit %d inactassert_off %d inactassert_jit %d "
			"scanfreqassert_off %d scanfreqassert_jit %d priassert_off_req %d\n",
			dtoh16(((uint16 *)buf)[0]), dtoh16(((uint16 *)buf)[1]),
			dtoh16(((uint16 *)buf)[2]), dtoh16(((uint16 *)buf)[3]),
			dtoh16(((uint16 *)buf)[4]), dtoh16(((uint16 *)buf)[5]),
			dtoh16(((uint16 *)buf)[6]), dtoh16(((uint16 *)buf)[7]),
			dtoh16(((uint16 *)buf)[8]), dtoh16(((uint16 *)buf)[9]),
			dtoh16(((uint16 *)buf)[10]), dtoh16(((uint16 *)buf)[11]),
			dtoh16(((uint16 *)buf)[12]), dtoh16(((uint16 *)buf)[13]),
			dtoh16(((uint16 *)buf)[14]));
		return 0;
	}

	if (argc < 15)
		goto usage;

	for (i = 0; i < 15; ++i) {
		val = strtoul(argv[i], &endptr, 0);
		if (*endptr != '\0')
			goto usage;
		configp[i] = htod16((uint16)val);
	}
	return wlu_iovar_setbuf(wl, "wci2_config", &wci2_config, sizeof(wci2_config_t),
		buf, WLC_IOCTL_SMLEN);

usage:
	return BCME_USAGE_ERROR;
}

static int
wl_mws_params(void *wl, cmd_t *cmd, char **argv)
{
	uint32 val;
	char *endptr = NULL;
	uint argc;
	mws_params_t mws_params;
	uint16 *paramsp = (uint16 *)&mws_params;
	int ret, i;

	UNUSED_PARAMETER(cmd);

	val = 0;

	/* eat command name */
	argv++;

	/* arg count */
	for (argc = 0; argv[argc]; argc++);

	memset(&mws_params, '\0', sizeof(mws_params_t));

	if (argc == 0) {
		/* Get and print the values */
		ret = wlu_iovar_getbuf(wl, "mws_params", &mws_params, sizeof(mws_params_t),
		buf, WLC_IOCTL_SMLEN);
		if (ret)
			return ret;

		printf("rx_center_freq %d tx_center_freq %d  rx_channel_bw %d tx_channel_bw %d "
			"channel_en %d channel_type %d\n",
			dtoh16(((uint16 *)buf)[0]), dtoh16(((uint16 *)buf)[1]),
			dtoh16(((uint16 *)buf)[2]), dtoh16(((uint16 *)buf)[3]), buf[8], buf[9]);
		return 0;
	}

	if (argc < 6)
		goto usage;
	for (i = 0; i < 4; ++i) {
		val = strtoul(argv[i], &endptr, 0);
		if (*endptr != '\0')
			goto usage;
		paramsp[i] = htod16((uint16)val);
	}
	val = strtoul(argv[i], &endptr, 0);
	if (*endptr != '\0')
		goto usage;
	mws_params.mws_channel_en = val;
	++i;
	val = strtoul(argv[i], &endptr, 0);
	if (*endptr != '\0')
		goto usage;
	mws_params.mws_channel_type = val;

	return wlu_iovar_setbuf(wl, "mws_params", &mws_params, sizeof(mws_params_t),
		buf, WLC_IOCTL_SMLEN);

usage:
	return BCME_USAGE_ERROR;
}

static int
wl_mws_wci2_msg(void *wl, cmd_t *cmd, char **argv)
{
	uint32 val;
	char *endptr = NULL;
	uint argc;
	mws_wci2_msg_t mws_wci2_msg;
	uint16 *paramsp = (uint16 *)&mws_wci2_msg;
	int ret, i = 0;

	UNUSED_PARAMETER(cmd);

	val = 0;

	/* eat command name */
	argv++;

	/* arg count */
	for (argc = 0; argv[argc]; argc++);

	memset(&mws_wci2_msg, '\0', sizeof(mws_wci2_msg_t));

	if (argc == 0) {
		/* Get and print the values */
		ret = wlu_iovar_getbuf(wl, "mws_debug_msg", &mws_wci2_msg, sizeof(mws_wci2_msg_t),
		buf, WLC_IOCTL_SMLEN);
		if (ret)
			return ret;

		printf("Message %d Interval %d  Repeats %d \n",
#ifdef DSLCPE_ENDIAN
			((uint16 *)buf)[0], dtoh16(((uint16 *)buf)[1]),
#else
			dtoh16(((uint16 *)buf)[0]), dtoh16(((uint16 *)buf)[1]),
#endif
			dtoh16(((uint16 *)buf)[2]));
		return 0;
	}

	if (argc < 3)
		goto usage;

	for (i = 0; i < 3; ++i) {
		val = strtoul(argv[i], &endptr, 0);
		if (*endptr != '\0')
			goto usage;
#ifdef DSLCPE_ENDIAN
		if(i==0) paramsp[i] = (uint8)val;
		else
#endif
		paramsp[i] = htod16((uint16)val);
	}
#ifdef DSLCPE_ENDIAN
	if ((dtoh16(paramsp[1]) < 20) || (dtoh16(paramsp[1]) > 32000))
#else
	if ((paramsp[1] < 20) || (paramsp[1] > 32000))
#endif
		goto usage;
	return wlu_iovar_setbuf(wl, "mws_debug_msg", &mws_wci2_msg, sizeof(mws_wci2_msg_t),
		buf, WLC_IOCTL_SMLEN);

usage:
	return BCME_USAGE_ERROR;
}

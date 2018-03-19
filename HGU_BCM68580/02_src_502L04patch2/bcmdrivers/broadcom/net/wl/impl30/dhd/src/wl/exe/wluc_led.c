/*
 * wl led command module
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
 * $Id: wluc_led.c 458728 2014-02-27 18:15:25Z $
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

static cmd_func_t wl_ledbh;
static cmd_func_t wl_led_blink_sync;

static cmd_t wl_led_cmds[] = {
	{ "ledbh", wl_ledbh, WLC_GET_VAR, WLC_SET_VAR,
	"set/get led behavior\n"
	"\tUsage: wl ledbh [0-3] [0-15]"},
	{ "led_blink_sync", wl_led_blink_sync, WLC_GET_VAR, WLC_SET_VAR, "set/get led_blink_sync\n"
	"\tUsage: wl led_blink_sync [0-3] [0/1]"},
	{ NULL, NULL, 0, 0, NULL }
};

static char *buf;

/* module initialization */
void
wluc_led_module_init(void)
{
	/* get the global buf */
	buf = wl_get_buf();

	/* register led commands */
	wl_module_cmds_register(wl_led_cmds);
}

static int
wl_ledbh(void *wl, cmd_t *cmd, char **argv)
{
	int err;
	wl_led_info_t led;
	void	*ptr = NULL;

	memset(&led, 0, sizeof(wl_led_info_t));
	if (*++argv == NULL) {
		printf("Usage: ledbh [led#] [behavior#]\n");
		return BCME_USAGE_ERROR;
	}
	led.index = (int)strtoul(*argv, NULL, 10);
#ifdef DSLCPE
	if (led.index >= WL_LED_NUMGPIO) {
		printf("exceed the max led support\n");
		return BCME_BADARG;
	}
#else
	if (led.index >= LED_MAX_INDEX) {
		printf("only %d led supported\n", LED_MAX_INDEX);
		return BCME_BADARG;
	}
#endif

#ifdef DSLCPE_ENDIAN
	led.index = htod32(led.index);
#endif
	if (*++argv) {	/* set */
		/* Read the original back so we don't toggle the activehi */
		if ((err = wlu_var_getbuf(wl, cmd->name, (void*)&led,
			sizeof(wl_led_info_t), &ptr)) < 0) {
			printf("wl_ledbh: fail to get. code %x\n", err);
		}
#ifdef DSLCPE_ENDIAN
		led.behavior = htod32((int)strtoul(*argv, NULL, 10));
#else
		led.behavior = (int)strtoul(*argv, NULL, 10);
#endif
		led.activehi = ((wl_led_info_t*)ptr)->activehi;

		if ((err = wlu_var_setbuf(wl, cmd->name, (void*)&led,
			sizeof(wl_led_info_t))) < 0) {
			printf("wl_ledbh: fail to set\n");
		}
	} else {	/* get */
		wl_led_info_t *ledo;

		if ((err = wlu_var_getbuf(wl, cmd->name, (void*)&led,
			sizeof(wl_led_info_t), &ptr)) < 0) {
			printf("wl_ledbh: fail to get\n");
		}
		ledo = (wl_led_info_t*)ptr;
#ifdef DSLCPE_ENDIAN
		printf("led %d behavior %d\n", dtoh32(ledo->index), dtoh32(ledo->behavior));
#else
		printf("led %d behavior %d\n", ledo->index, ledo->behavior);
#endif
	}

	return err;
}

static int
wl_led_blink_sync(void *wl, cmd_t *cmd, char **argv)
{
	int argc;
	int err = 0;
	int in_arg[2];
	void *ptr = NULL;
	char *endptr;

	/* arg count */
	for (argc = 0; argv[argc]; argc++)
		;

	if (argc > 3 || argc < 2)
		return BCME_USAGE_ERROR;

	in_arg[0] = htod32((uint32)(strtoul(argv[1], &endptr, 0)));

	if (in_arg[0] > 3) {
		printf("only 4 led supported\n");
		return BCME_BADARG;
	}

	if (argc == 2) {
		err = wlu_var_getbuf(wl, cmd->name, (void*)in_arg, sizeof(int), &ptr);
		if (err < 0)
			return err;
#ifdef DSLCPE_ENDIAN
		printf("led%d, blink_sync is %s\n", dtoh32(in_arg[0]),
#else
		printf("led%d, blink_sync is %s\n", in_arg[0],
#endif
			(dtoh32(*(int*)ptr) != 0) ? "TRUE" : "FALSE");
	}
	else if (argc == 3) {
		in_arg[1] = htod32((uint32)(strtoul(argv[2], &endptr, 0)));
		err = wlu_var_setbuf(wl, cmd->name, in_arg, sizeof(in_arg));
	}

	return err;
}

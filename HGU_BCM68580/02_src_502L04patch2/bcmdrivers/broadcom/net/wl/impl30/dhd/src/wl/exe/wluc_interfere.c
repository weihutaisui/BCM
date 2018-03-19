/*
 * wl interfere command module
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
 * $Id: wluc_interfere.c 458728 2014-02-27 18:15:25Z $
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

static cmd_func_t wl_itfr_get_stats;

static cmd_t wl_interfere_cmds[] = {
	{ "itfr_get_stats", wl_itfr_get_stats, WLC_GET_VAR, -1,
	"get interference source information"
	},
	{ "itfr_enab", wl_varint, WLC_GET_VAR, WLC_SET_VAR,
	"get/set STA interference detection mode(STA only)\n"
	"\t 0  - disable\n"
	"\t 1  - enable maual detection\n"
	"\t 2  - enable auto detection"
	},
	{ "itfr_detect", wl_var_void, -1, WLC_SET_VAR,
	"issue an interference detection request"
	},
	{ NULL, NULL, 0, 0, NULL }
};

static char *buf;

/* module initialization */
void
wluc_interfere_module_init(void)
{
	(void)g_swap;

	/* get the global buf */
	buf = wl_get_buf();

	/* register interfere commands */
	wl_module_cmds_register(wl_interfere_cmds);
}

static int
wl_itfr_get_stats(void *wl, cmd_t *cmd, char **argv)
{
	int err;
	interference_source_rep_t *iftr_stats = NULL;
	const char *iftr_source[] = {"none", "wireless phone", "wireless video camera",
		"microwave oven", "wireless baby monitor", "bluetooth device",
		"wireless video camera or baby monitor", "bluetooth or baby monitor",
		"video camera or phone", "unidentified"}; /* sync with interference_source_t */

	UNUSED_PARAMETER(argv);

	if ((err = wlu_var_getbuf(wl, cmd->name, NULL, 0, (void*)&iftr_stats)) < 0)
		return err;
#ifdef DSLCPE_ENDIAN
	if (dtoh32(iftr_stats->flags) & ITFR_NOISY_ENVIRONMENT)
#else
	if (iftr_stats->flags & ITFR_NOISY_ENVIRONMENT)
#endif
		printf("Feature is stopped due to noisy environment\n");
	else
		printf("Interference %s detected. last interference at timestamp %d: "
			"source is %s on %s channel\n",
#ifdef DSLCPE_ENDIAN
		        (dtoh32(iftr_stats->flags) & ITFR_INTERFERENCED) ? "is" : "is not",
		        dtoh32(iftr_stats->timestamp), iftr_source[iftr_stats->source],
		        (dtoh32(iftr_stats->flags) & ITFR_HOME_CHANNEL) ? "home" : "non-home");
#else
			(iftr_stats->flags & ITFR_INTERFERENCED) ? "is" : "is not",
			iftr_stats->timestamp, iftr_source[iftr_stats->source],
			(iftr_stats->flags & ITFR_HOME_CHANNEL) ? "home" : "non-home");
#endif
	return err;
}

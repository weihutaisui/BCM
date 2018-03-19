/*
 * Wireless Application Event Service
 * shared header file
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
 * $Id: $
 */

#ifndef _appeventd_h_
#define _appeventd_h_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <typedefs.h>
#include <bcmnvram.h>
#include <bcmutils.h>
#include <bcmtimer.h>
#include <bcmendian.h>
#include <shutils.h>
#include <security_ipc.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <appevent_hdr.h>

extern int appeventd_debug_level;

#define APPEVENTD_DEBUG_ERROR	0x0001
#define APPEVENTD_DEBUG_WARNING	0x0002
#define APPEVENTD_DEBUG_INFO	0x0004
#define APPEVENTD_DEBUG_DETAIL	0x0008

#define APPEVENTD_ERROR(fmt, arg...) \
		do { if (appeventd_debug_level & APPEVENTD_DEBUG_ERROR) \
			printf("APPEVENTD >> "fmt, ##arg); } while (0)

#define APPEVENTD_WARNING(fmt, arg...) \
		do { if (appeventd_debug_level & APPEVENTD_DEBUG_WARNING) \
			printf("APPEVENTD >> "fmt, ##arg); } while (0)

#define APPEVENTD_INFO(fmt, arg...) \
		do { if (appeventd_debug_level & APPEVENTD_DEBUG_INFO) \
			printf("APPEVENTD >> "fmt, ##arg); } while (0)

#define APPEVENTD_DEBUG(fmt, arg...) \
		do { if (appeventd_debug_level & APPEVENTD_DEBUG_DETAIL) \
			printf("APPEVENTD >> "fmt, ##arg); } while (0)

#define APPEVENTD_BUFSIZE	2048

#define APPEVENTD_OK	0
#define APPEVENTD_FAIL -1

/* WiFi Application Event ID */
#define APP_E_BSD_STEER_START 1  /* status: STEERING */
#define APP_E_BSD_STEER_END   2  /* status: SUCC/FAIL */
#define APP_E_BSD_STATS_QUERY 3  /* status: STA/RADIO */
#define APP_E_WBD_SLAVE_WEAK_CLIENT 4  /* status: SUCC */
#define APP_E_WBD_SLAVE_STEER_START 5  /* status: SUCC */
#define APP_E_WBD_SLAVE_STEER_RESP  6  /* status: SUCC */
#define APP_E_WBD_MASTER_WEAK_CLIENT 7  /* status: SUCC */
#define APP_E_WBD_MASTER_STEER_START 8  /* status: SUCC */
#define APP_E_WBD_MASTER_STEER_RESP  9  /* status: SUCC */
#define APP_E_WBD_MASTER_STEER_END   10  /* status: SUCC */
#endif /* _appeventd_h_ */

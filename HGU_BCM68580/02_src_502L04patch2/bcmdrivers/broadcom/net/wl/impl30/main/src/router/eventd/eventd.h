/*
 * EVENTD shared include file
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
 * $Id: eventd.h 506825 2014-10-07 13:05:36Z $
 */

#ifndef _eventd_h_
#define _eventd_h_

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
#include <bcmendian.h>
#include <bcmwifi_channels.h>
#include <wlioctl.h>
#include <wlutils.h>

#include <security_ipc.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

#ifndef DSLCPE_ENDIAN
extern bool eventd_swap;
#define htod32(i) (eventd_swap?bcmswap32(i):(uint32)(i))
#define htod16(i) (eventd_swap?bcmswap16(i):(uint16)(i))
#define dtoh32(i) (eventd_swap?bcmswap32(i):(uint32)(i))
#define dtoh16(i) (eventd_swap?bcmswap16(i):(uint16)(i))
#define htodchanspec(i) (eventd_swap?htod16(i):i)
#define dtohchanspec(i) (eventd_swap?dtoh16(i):i)
#define htodenum(i) (eventd_swap?((sizeof(i) == 4) ? \
			htod32(i) : ((sizeof(i) == 2) ? htod16(i) : i)):i)
#define dtohenum(i) (eventd_swap?((sizeof(i) == 4) ? \
			dtoh32(i) : ((sizeof(i) == 2) ? htod16(i) : i)):i)
#endif

extern int eventd_debug_level;

#define EVENTD_DEBUG_ERROR	0x0001
#define EVENTD_DEBUG_WARNING	0x0002
#define EVENTD_DEBUG_INFO		0x0004
#define EVENTD_DEBUG_DETAIL	0x0008

#define EVENTD_ERROR(fmt, arg...) \
		do { if (eventd_debug_level & EVENTD_DEBUG_ERROR) \
			printf("EVENTD >>%s(%d): "fmt, __FUNCTION__, __LINE__, ##arg); } while (0)

#define EVENTD_WARNING(fmt, arg...) \
		do { if (eventd_debug_level & EVENTD_DEBUG_WARNING) \
			printf("EVENTD >>%s(%d): "fmt, __FUNCTION__, __LINE__, ##arg); } while (0)

#define EVENTD_INFO(fmt, arg...) \
		do { if (eventd_debug_level & EVENTD_DEBUG_INFO) \
			printf("EVENTD >>%s(%d): "fmt, __FUNCTION__, __LINE__, ##arg); } while (0)

#define EVENTD_DEBUG(fmt, arg...) \
		do { if (eventd_debug_level & EVENTD_DEBUG_DETAIL) \
			printf("EVENTD >>%s(%d): "fmt, __FUNCTION__, __LINE__, ##arg); } while (0)

#define EVENTD_BUFSIZE_4K	4096

#define EVENTD_OK	0
#define EVENTD_FAIL -1

#define EVENTD_IFNAME_SIZE		16
#define EVENTD_MAX_INTERFACES		3
#define EVENT_MAX_IF_NUM EVENTD_MAX_INTERFACES

#define EVENTD_DFLT_POLL_INTERVAL 1  /* default polling interval */

#endif /*  _eventd_h_ */

/*
 * Wireless Application Event Service
 * appeventd-wbd header file
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
 * $Id: appeventd_wbd.h 683328 2017-02-07 11:08:40Z $
 */

#ifndef _appeventd_wbd_h_
#define _appeventd_wbd_h_

/* WiFi application event WBD status. */
#define APP_E_WBD_STATUS_SUCCESS	1

#ifndef IFNAMSIZ
#define IFNAMSIZ			16	/* Ifname size. */
#endif // endif

#define RESP_BUFSIZE			32	/* Resp buffer size. */

/* Weak sta event data. */
typedef struct app_event_wbd_weak_sta {
	char ifname[IFNAMSIZ];		/* Interface name. */
	struct ether_addr sta_addr;	/* STA mac addr. */
	int rssi;			/* STA rssi. */
	uint32 tx_failures;		/* Tx fail count. */
	float tx_rate;			/* Tx rate. */
} app_event_wbd_weak_sta_t;

/* Steer sta event data. */
typedef struct app_event_wbd_steer_sta {
	char ifname[IFNAMSIZ];		/* Interface name. */
	struct ether_addr sta_addr;	/* STA mac addr. */
	struct ether_addr dst_addr;	/* Destination slave bssid. */
	int src_rssi;			/* STA rssi at source ap. */
	int dst_rssi;			/* STA rssi at target ap. */
} app_event_wbd_steer_sta_t;

/* Steer response event data. */
typedef struct app_event_wbd_steer_resp {
	char ifname[IFNAMSIZ];		/* Interface name. */
	struct ether_addr sta_addr;	/* STA mac addr. */
	int resp_code;			/* Steer response code. */
	char resp[RESP_BUFSIZE];	/* Steer response. */
} app_event_wbd_steer_resp_t;

/* Sta stats event data. */
typedef struct app_event_wbd_sta_stats {
	struct ether_addr sta_addr;	/* STA mac addr. */
	int rssi;			/* STA rssi. */
	float tx_rate;			/* STA tx-rate. */
} app_event_wbd_sta_stats_t;

/* Steer complete event data. */
typedef struct app_event_wbd_steer_complete {
	char ifname[IFNAMSIZ];		/* Interface name. */
	struct ether_addr sta_addr;	/* STA mac addr */
	struct ether_addr src_addr;	/* Source slave bssid. */
	struct ether_addr dst_addr;	/* Destination slave bssid. */
	app_event_wbd_sta_stats_t sta_stats;	/* STA stats after steering completion. */
} app_event_wbd_steer_complete_t;
#endif /* _appeventd_wbd_h_ */

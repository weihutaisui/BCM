/***********************************************************************
 *
 *  Copyright (c) 2014  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2014:proprietary:standard 

 This program is the proprietary software of Broadcom and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").  Except as set forth in
 an Authorized License, Broadcom grants no license (express or implied), right
 to use, or waiver of any kind with respect to the Software, and Broadcom
 expressly reserves all rights in and to the Software and all intellectual
 property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,

 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use
    all reasonable efforts to protect the confidentiality thereof, and to
    use this information only in connection with your use of Broadcom
    integrated circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
    PERFORMANCE OF THE SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
    LIMITED REMEDY.
:> 
 *
 ************************************************************************/

#ifndef __WSD_MAIN_H__
#define __WSD_MAIN_H__

/*!\file wsd_main.h
 * Main functions for websockets server.
 *
 */


#define WSD_VERSION                  "0.2"
#define WSD_PORT                     7681
#define WSD_TIMEOUT_MSEC             100
#define WSD_EPOLL_NUM_EVTS           5


#include "cms.h"

/* The order of "enum wsd_protocols"
 * should match with the order of
 * "struct libwebsocket_protocols protocols" 
 * that is defined in wsd_main.c
 */

enum wsd_protocols {
    /* always first */
    WSD_PROTOCOL_HTTP = 0,
    WSD_PROTOCOL_SYS_MEM,
#ifdef SUPPORT_DPI
    WSD_PROTOCOL_DPI,
#endif
#ifdef DMP_X_BROADCOM_COM_SPDSVC_1
    WSD_PROTOCOL_SPEED_SERVICE,
#endif
    /* always last */
    WSD_PROTOCOL_COUNT
};


struct psd_session_key {
    int sessionKey;
};


int wsd_callback_http
    (struct lws *wsi,
     enum lws_callback_reasons reason,
     void *user,
     void *in,
     size_t len);


int wsd_callback_cpu_mem
    (struct lws *wsi,
     enum lws_callback_reasons reason,
     void *user,
     void *in,
     size_t len);


#ifdef SUPPORT_DPI

int wsd_callback_dpi(struct lws *wsi, enum lws_callback_reasons reason,
                     void *user, void *in, size_t len);

struct wsd_dpi_response {
  int size;
  int pos;
  int send_count;
  unsigned char *data;
  struct wsd_dpi_response *next;
};
struct wsd_dpi_state {
   struct psd_session_key   session;
   struct wsd_dpi_response *responses;
   struct lws              *wsi;
};


#endif    // SUPPORT_DPI


#ifdef DMP_X_BROADCOM_COM_SPDSVC_1

struct psd_speed_service {
     char mode[BUFLEN_16];
     char serverAddr[BUFLEN_32];
     char direction[BUFLEN_16];
     char dataPath[BUFLEN_16];
     char algorithm[BUFLEN_8];
     UINT32 tcpPort;
     UINT32 duration;
     UINT32 pktLength;
     UINT32 kbps;
     UINT32 steps;
     UINT32 loss;
     SINT32 latency;
     UINT32 lossPercentage;
     UINT32 status;
     SINT32 sessionKey;
};


int wsd_callback_speed_service
    (struct lws *wsi,
     enum lws_callback_reasons reason,
     void *user,
     void *in,
     size_t len);

#endif    // DMP_X_BROADCOM_COM_SPDSVC_1


void dump_handshake_info(struct lws *wsi);
UBOOL8 wsd_validate_session_key(int sessionKey);


#endif  // __WSD_MAIN_H__

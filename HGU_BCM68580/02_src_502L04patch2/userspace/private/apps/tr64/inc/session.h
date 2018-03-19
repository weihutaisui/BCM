/*----------------------------------------------------------------------*
 *
 * Copyright (c) 2005-2012 Broadcom Corporation
 * All Rights Reserved
 *
 * <:label-BRCM:2012:proprietary:standard
 * 
 *  This program is the proprietary software of Broadcom and/or its
 *  licensors, and may only be used, duplicated, modified or distributed pursuant
 *  to the terms and conditions of a separate, written license agreement executed
 *  between you and Broadcom (an "Authorized License").  Except as set forth in
 *  an Authorized License, Broadcom grants no license (express or implied), right
 *  to use, or waiver of any kind with respect to the Software, and Broadcom
 *  expressly reserves all rights in and to the Software and all intellectual
 *  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 *  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 *  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 * 
 *  Except as expressly set forth in the Authorized License,
 * 
 *  1. This program, including its structure, sequence and organization,
 *     constitutes the valuable trade secrets of Broadcom, and you shall use
 *     all reasonable efforts to protect the confidentiality thereof, and to
 *     use this information only in connection with your use of Broadcom
 *     integrated circuit products.
 * 
 *  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
 *     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
 *     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
 *     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
 *     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
 *     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
 *     PERFORMANCE OF THE SOFTWARE.
 * 
 *  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
 *     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
 *     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
 *     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 *     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
 *     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
 *     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
 *     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
 *     LIMITED REMEDY.
 * :>
 *
 *----------------------------------------------------------------------*
 * File Name  : session.h
 *
 *----------------------------------------------------------------------*/

#ifndef SESSION_H
#define SESSION_H

/* session timeout should be in the range of 30-45 seconds */
#define TR64_SESSION_TIMEOUT      45  
#define TR64_MAX_WAIT_EVENT       10
#define TR64_SSID_LEN             16

typedef enum {
   WAIT_EVENT_RESOURCE_ERROR,
   WAIT_EVENT_OK
} sessionWaitEventStatus;


typedef enum {
   SESSION_ACTIVE,
   SESSION_EXPIRED
} sessionState;

typedef enum {
   CHANGE_APPLIED,
   REBOOT_REQUIRED,
} sessionConfigStatus;

typedef struct waitEvent
{
   struct http_connection *c;
   struct waitEvent *next;
} waitEvent, *pWaitEvent;

typedef struct waitEventList
{
   int count;
   pWaitEvent head;
   pWaitEvent tail;
} waitEventList, *pWaitEventList;

typedef struct sessionInfo
{
#ifdef UUID_TYPE
   uuid_t sessionId;              /* session ID in uuid_t format */
#else
   char sessionId[TR64_SSID_LEN]; /* session ID 16 hex octets format */
#endif
   sessionState state;    /* session state; active or expired  */
   time_t timer;          /* timer handle */
   time_t expires;        /* keep track of when session expires */
   sessionAuth auth;      /* digest authentication details */
   sessionConfigStatus configStatus;  /* status of session: changeApplied, rebootRequired */
   struct waitEventList eventList; /* event to be processed after session unlocks. */
} sessionInfo, *pSessionInfo;

int sidStrToUuid(char *sidStr, char *sid);
sessionWaitEventStatus sessionEnqueueWaitEvent(struct http_connection *c);
void createSession(char *sessionId);

#endif   // SESSION_H

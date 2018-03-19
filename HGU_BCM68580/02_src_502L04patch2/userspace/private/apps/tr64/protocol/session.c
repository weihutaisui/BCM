/*****************************************************************************
//
// <:copyright-BRCM:2012:proprietary:standard
// 
//    Copyright (c) 2012 Broadcom 
//    All Rights Reserved
// 
//  This program is the proprietary software of Broadcom and/or its
//  licensors, and may only be used, duplicated, modified or distributed pursuant
//  to the terms and conditions of a separate, written license agreement executed
//  between you and Broadcom (an "Authorized License").  Except as set forth in
//  an Authorized License, Broadcom grants no license (express or implied), right
//  to use, or waiver of any kind with respect to the Software, and Broadcom
//  expressly reserves all rights in and to the Software and all intellectual
//  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
//  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
//  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
// 
//  Except as expressly set forth in the Authorized License,
// 
//  1. This program, including its structure, sequence and organization,
//     constitutes the valuable trade secrets of Broadcom, and you shall use
//     all reasonable efforts to protect the confidentiality thereof, and to
//     use this information only in connection with your use of Broadcom
//     integrated circuit products.
// 
//  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
//     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
//     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
//     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
//     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
//     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
//     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
//     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
//     PERFORMANCE OF THE SOFTWARE.
// 
//  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
//     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
//     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
//     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
//     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
//     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
//     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
//     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
//     LIMITED REMEDY.
// :>
//
******************************************************************************/

#include "upnp_osl.h"
#include "upnp_dbg.h"
#include "upnp.h"
#include "session.h"
#include "tr64defs.h"
#include "linux_timer.h"

extern int process_http_request(struct http_connection *c);
extern pSessionInfo pCurrentSession;
void sessionReaper(void);

/* return number parsed in sidStr */
int sidStrToUuid(char *sidStr, char *sid)
{
   int ret, len;

   len = strlen(sidStr);
   UPNP_TRACE(("sidStrToUuid(sidStr %s): len %d.\n",sidStr,len));

   if (len > (TR64_SSID_LEN*2))
   {
      ret = sscanf(sidStr,"%2hhx%2hhx%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",
                   (u_int8*)&sid[0],(u_int8*)&sid[1],(u_int8*)&sid[2],
                   (u_int8*)&sid[3],(u_int8*)&sid[4],(u_int8*)&sid[5],
                   (u_int8*)&sid[6],(u_int8*)&sid[7],(u_int8*)&sid[8],
                   (u_int8*)&sid[9],(u_int8*)&sid[10],(u_int8*)&sid[11],
                   (u_int8*)&sid[12],(u_int8*)&sid[13],(u_int8*)&sid[14],
                   (u_int8*)&sid[15]);
   }
   else
   {
      ret = sscanf(sidStr,"%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",
                   (u_int8*)&sid[0],(u_int8*)&sid[1],(u_int8*)&sid[2],
                   (u_int8*)&sid[3],(u_int8*)&sid[4],(u_int8*)&sid[5],
                   (u_int8*)&sid[6],(u_int8*)&sid[7],(u_int8*)&sid[8],
                   (u_int8*)&sid[9],(u_int8*)&sid[10],(u_int8*)&sid[11],
                   (u_int8*)&sid[12],(u_int8*)&sid[13],(u_int8*)&sid[14],
                   (u_int8*)&sid[15]);
   }

   UPNP_TRACE(("sidStrToUuid(end): sscanf returns %d; sid content is--\n",ret));
   for (len = 0; len < ret; len++)
      printf("sid[%d] = %2x\n",len,(u_int8)sid[len]);

   return (ret);
} /* sidStrToUuid */

/* put waitEvent to the sessionList; return error if resource error */
sessionWaitEventStatus sessionEnqueueWaitEvent(struct http_connection *c)
{
   pWaitEvent pEvent;
   
   
   if (pCurrentSession->eventList.count == TR64_MAX_WAIT_EVENT)
   {
      return WAIT_EVENT_RESOURCE_ERROR;
   }

   pEvent = (pWaitEvent)malloc(sizeof(waitEvent));
   if (pEvent == NULL)
   {
      return WAIT_EVENT_RESOURCE_ERROR;
   }
      
   pEvent->c = c;
   pEvent->next = NULL;
   pCurrentSession->eventList.tail->next = pEvent;
   pCurrentSession->eventList.tail = pEvent;
   pCurrentSession->eventList.count += 1;
   return WAIT_EVENT_OK;
} 

struct http_connection *sessionDequeueWaitEvent(void)
{
   pWaitEvent pEvent;
   struct http_connection *c;

   /* take one event out */
   pEvent  = pCurrentSession->eventList.head;
   pCurrentSession->eventList.head = pEvent->next;
   pCurrentSession->eventList.count -= 1;
   c = pEvent->c;
   free(pEvent);
   return c;
}

/* this routine is called when ConfigurationStarted is received */
void createSession(char *sessionId)
{
   int i;
   struct  itimerspec  timer;

   UPNP_TRACE(("createSession with sessionId:\n"));
   for (i = 0; i < 16; i++)
   {
      UPNP_TRACE(("%2x ",(unsigned char)sessionId[i]));
   }

   if (pCurrentSession == NULL) 
   {
      pCurrentSession = malloc(sizeof(sessionInfo));
      memset(pCurrentSession,0,sizeof(sessionInfo));
   }

   if (pCurrentSession)
   {
      memcpy(pCurrentSession->sessionId,sessionId,sizeof(pCurrentSession->sessionId));
      pCurrentSession->state = SESSION_ACTIVE;
      pCurrentSession->configStatus = CHANGE_APPLIED;
      pCurrentSession->expires = time(NULL) + TR64_SESSION_TIMEOUT;
   }
   memset(&timer, 0, sizeof(timer));
   timer.it_interval.tv_sec = 1;
   timer.it_value.tv_sec = 1;
   pCurrentSession->timer = 
      (time_t)enqueue_event(&timer, (event_callback_t)sessionReaper, NULL);
} /* createSession */

/* this routine is called when ConfigurationStarted is received */
void sessionReaper(void)
{
   int i;
   time_t now;
   struct http_connection *c;

   now = time(NULL);

   if (pCurrentSession == NULL)
      return;


   if (pCurrentSession->state == SESSION_ACTIVE)
   {
      if (pCurrentSession->expires < now)
      {
         UPNP_TRACE(("session expires (eventCount %x), sesionID:\n",
                    pCurrentSession->eventList.count));
         for (i = 0; i < 16; i++)
         {
            UPNP_TRACE(("%2x ",(unsigned char)pCurrentSession->sessionId[i]));
         }
         pCurrentSession->state = SESSION_EXPIRED;
      }
   } /* active */
   else 
   {
      /* inactive */
      UPNP_TRACE(("session expired, process session->event (event %x)\n",
                  pCurrentSession->eventList.count));
      if (pCurrentSession->eventList.count > 0)
      {
         /* take one event out */
         c = sessionDequeueWaitEvent();
         /* process this http_connection */
         process_http_request(c);
      }
      if (pCurrentSession->eventList.count == 0)
      {
         timer_delete((timer_t)(pCurrentSession->timer));
         free(pCurrentSession);
         pCurrentSession = NULL;
      }
   }
} /* sessionReaper */


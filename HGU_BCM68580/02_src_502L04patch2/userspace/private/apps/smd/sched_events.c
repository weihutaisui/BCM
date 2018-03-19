/***********************************************************************
 *
 *  Copyright (c) 2006-2008  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2012:proprietary:standard

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

#include "cms.h"
#include "cms_util.h"
#include "sched_events.h"
#include "oal.h"

/*!\file sched_event.c
 * \brief This is supposed to be a generic scheduling mechanism for smd,
 *        but it is turning out to be specific for delayed msg.
 *        Currently, it just has a delayed reboot timer feature tacked on.
 *
 */


/** Structure to track a single delayed/timer event.
 *
 */
typedef struct sched_event_info
{
   struct sched_event_info *prev; /**< prev pointer, NULL if this is first on list */
   struct sched_event_info *next; /**< next pointer, NULL if this is last on list */
   UINT32                   id;   /**< Identifier of the delayed message */
   void *                   ctx;  /**< Pointer to the dInfo structure */
} SchedEventInfo;


void  *tmrHandle=NULL;
SchedEventInfo *schedInfoList = NULL;

static SchedEventInfo *createSchedInfo(UINT32 id, void *ctx);
static SchedEventInfo *getSchedInfo(UINT32 id, void *ctx);
static void deleteSchedInfo(SchedEventInfo **schedInfo);

CmsRet sched_init(void)
{
   CmsRet ret;

   if ((ret = cmsTmr_init(&tmrHandle)) != CMSRET_SUCCESS)
   {
      cmsLog_error("timer initialization failed, ret=%d", ret);
   }

   return ret;
}


void sched_cleanup(void)
{
   SchedEventInfo *schedInfo;

   while ((schedInfo = schedInfoList) != NULL)
   {
      schedInfoList = schedInfo->next;
      cmsMem_free(schedInfo);
   }

   cmsTmr_cleanup(&tmrHandle);
   return;
}


SchedEventInfo *getSchedInfo(UINT32 id, void *ctx)
{
   SchedEventInfo *schedInfo = schedInfoList;

   while (schedInfo != NULL)
   {
      if (schedInfo->id == id && schedInfo->ctx == ctx)
      {
         return schedInfo;
      }

      schedInfo = schedInfo->next;
   }

   return NULL;
}


SchedEventInfo *createSchedInfo(UINT32 id, void *ctx)
{
   SchedEventInfo *schedInfo;

   schedInfo = cmsMem_alloc(sizeof(SchedEventInfo), ALLOC_ZEROIZE);
   if (schedInfo != NULL)
   {
      schedInfo->id = id;
      schedInfo->ctx = ctx;

      /* link it at the head of the list, order does not matter. */
      schedInfo->next = schedInfoList;
      if (schedInfoList != NULL)
      {
         schedInfoList->prev = schedInfo;
      }
      schedInfoList = schedInfo;
   }

   return schedInfo;
}


void deleteSchedInfo(SchedEventInfo **schedInfo)
{
   if (schedInfoList == (*schedInfo))
   {
      /* delete from head of the list */
      schedInfoList = (*schedInfo)->next;
      if (schedInfoList != NULL)
      {
         schedInfoList->prev = NULL;
      }
   }
   else
   {
      (*schedInfo)->prev->next = (*schedInfo)->next;
      if ((*schedInfo)->next != NULL)
      {
         (*schedInfo)->next->prev = (*schedInfo)->prev;
      }
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(*schedInfo);

   return;
}

/** Timer handler func.
 *
 * Sched defines its own timer func so that it can break out the
 * fields of SchedEventInfo before calling oal_sendDelayedMsg, 
 * so that oal_events.c do not need to know the declaration of SchedEventInfo.
 */
void sched_timerHandlerFunc(void *ctx)
{
   SchedEventInfo *schedInfo = (SchedEventInfo *) ctx;

   cmsLog_debug("calling oal_sendDealyedMsg with id=0x%x dInfo=%p",
                schedInfo->id, schedInfo->ctx);

   oal_sendDelayedMsg(schedInfo->id, schedInfo->ctx);
}


CmsRet sched_set(UINT32 id, void *ctx, SINT32 ms)
{
   SchedEventInfo *schedInfo;
   CmsRet ret;

   if ((schedInfo = createSchedInfo(id, ctx)) == NULL)
   {
      cmsLog_error("could not allocate memory for schedInfo");
      ret = CMSRET_RESOURCE_EXCEEDED;
   }
   else
   {
      ret = cmsTmr_set(tmrHandle, sched_timerHandlerFunc, schedInfo, ms, "delayed_msg");
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("setting sched event failed, ret=%d", ret);
      }
   }

   return ret;
}


CmsRet sched_cancel(UINT32 id, void *ctx)
{
   SchedEventInfo *schedInfo;
   CmsRet ret = CMSRET_INVALID_ARGUMENTS;

   if ((schedInfo = getSchedInfo(id, ctx)) != NULL)
   {
      cmsTmr_cancel(tmrHandle, sched_timerHandlerFunc, schedInfo);
      deleteSchedInfo(&schedInfo);
      ret = CMSRET_SUCCESS;
   }

   return ret;
}


UINT32 sched_getTimeToNextEvent(void)
{
   UINT32 ms;

   cmsTmr_getTimeToNextEvent(tmrHandle, &ms);

   return ms;
}


CmsRet sched_getTimeRemaining(UINT32 id, void *ctx, UINT32 *ms)
{
   SchedEventInfo *schedInfo;
   CmsRet ret = CMSRET_INVALID_ARGUMENTS;

   if ((schedInfo = getSchedInfo(id, ctx)) != NULL)
   {
      ret = cmsTmr_getTimeRemaining(tmrHandle,sched_timerHandlerFunc,schedInfo,ms);
   }

   return ret;
}


void sched_process(void)
{
   cmsTmr_executeExpiredEvents(tmrHandle);
}

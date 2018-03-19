/***********************************************************************
 *
 *  Copyright (c) 2007  Broadcom Corporation
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
#include "oal.h"
#include "event_interest.h"

DLIST_HEAD(evtInterestHead);

void interest_init(void)
{
   return;
}


void interest_cleanup(void)
{
   EvtInterest *evtInterest;
   EvtInterestInfo *evtInfo;

   /* free all malloc'd structures */
   while (!(dlist_empty(&evtInterestHead)))
   {
      evtInterest = (EvtInterest *) evtInterestHead.next;

      while (!(dlist_empty(&(evtInterest->evtInfoHead))))
      {
         evtInfo = (EvtInterestInfo *) evtInterest->evtInfoHead.next;

         dlist_unlink((DlistNode *) evtInfo);
         cmsMem_free(evtInfo->matchData);
         cmsMem_free(evtInfo);
      }

      /* unlink the evtInterest from the dlist and free it */
      dlist_unlink((DlistNode *) evtInterest);
      cmsMem_free(evtInterest);
   }

   return;
}


CmsRet interest_register(CmsMsgType msgType, CmsEntityId interestedEid, const char *matchData)
{
   EvtInterest *evtInterest;
   EvtInterestInfo *evtInfo;

   cmsLog_debug("msgType=0x%x for eid %d (0x%x) matchData=%s", msgType, interestedEid, interestedEid, matchData);

   if ((evtInterest = interest_findType(msgType)) == NULL)
   {
      /*
       * First registration for this event.  Allocate an EventInterest structure
       * and link it into the interestHandle.
       */
      evtInterest = (EvtInterest *) cmsMem_alloc(sizeof(EvtInterest), ALLOC_ZEROIZE);
      if (evtInterest == NULL)
      {
         cmsLog_error("could not allocate evtInterest");
         return CMSRET_RESOURCE_EXCEEDED;
      }

      evtInterest->type = msgType;
      DLIST_HEAD_IN_STRUCT_INIT(evtInterest->evtInfoHead);

      /* link new evtInterest onto list */
      dlist_append((struct dlist_node *) evtInterest, &evtInterestHead);
   }

   /*
    * at this point, evtInterest is pointing to the event of interest,
    * check for duplicate registration.
    */
   if ((evtInfo = interest_findInfo(evtInterest, interestedEid)) != NULL)
   {
      if (cmsUtl_strcmp(evtInfo->matchData, matchData))
      {
         cmsLog_error("different match data fields for same eid %d, ignore new matchData", evtInfo->eid);
      }
      cmsLog_debug("duplicate registration for event 0x%x eid %d (0x%x), ignore registration",
                   evtInterest->type, interestedEid, interestedEid);
      return CMSRET_INVALID_ARGUMENTS;
   }
    

   /* No duplicate found, allocate a new EvtInterestInfo and link it into list */
   evtInfo = (EvtInterestInfo *) cmsMem_alloc(sizeof(EvtInterestInfo), ALLOC_ZEROIZE);
   if (evtInfo == NULL)
   {
      cmsLog_error("Could not allocate EvtInterestInfo");
      return CMSRET_RESOURCE_EXCEEDED;
   }

   evtInfo->eid = interestedEid;
   evtInfo->matchData = cmsMem_strdup(matchData);

   dlist_append((DlistNode *) evtInfo, &(evtInterest->evtInfoHead));

   return CMSRET_SUCCESS;
}



CmsRet interest_unregister(CmsMsgType msgType, CmsEntityId interestedEid)
{
   EvtInterest *evtInterest;
   EvtInterestInfo *evtInfo;

   cmsLog_debug("msgType=0x%x eid=%d (0x%x)", msgType, interestedEid, interestedEid);
   
   if ((evtInterest = interest_findType(msgType)) == NULL)
   {
      cmsLog_error("could not find record of msgType 0x%x", msgType);
      return CMSRET_INVALID_ARGUMENTS;
   }

   /* try to find evtInfo for the interestedEid */
   if ((evtInfo = interest_findInfo(evtInterest, interestedEid)) == NULL)
   {
      cmsLog_error("could not find record for msgType 0x%x eid %d (0x%x)",
         msgType, interestedEid, interestedEid);
      return CMSRET_INVALID_ARGUMENTS;
   }

   /* unlink evtInfo and free the evtInfo struct. */
   dlist_unlink((DlistNode *) evtInfo);
   cmsMem_free(evtInfo->matchData);
   cmsMem_free(evtInfo);
   
   /*
    * Check if there are no apps interested in this msg type.
    * If so, we can also unlink and free the evtInterest struct.
    */
   if (dlist_empty(&(evtInterest->evtInfoHead)))
   {
      dlist_unlink((DlistNode *) evtInterest);
      cmsMem_free(evtInterest);
   }

   return CMSRET_SUCCESS;
}


void interest_unregisterAll(CmsEntityId interestedEid)
{
   EvtInterest *evtInterest;
   EvtInterestInfo *evtInfo;
   UBOOL8 checkAgain = TRUE;
   
   cmsLog_debug("eid=%d (0x%x)", interestedEid, interestedEid);
   
   while (checkAgain)
   {
      /* no need to check again unless we find a match in our search */
      checkAgain = FALSE;
      
      dlist_for_each_entry(evtInterest, &evtInterestHead, dlist)
      {
         if ((evtInfo = interest_findInfo(evtInterest, interestedEid)) != NULL)
         {
            cmsLog_debug("found evtInfo on msgType=%d", evtInterest->type);
            interest_unregister(evtInterest->type, interestedEid);
            
            /*
             * This is tricky.  When we unregistered the eid for a
             * particular msg type, that could have been the last
             * interestedEid, which means the EvtInterest would have been
             * deleted also.  This might screw up our for loop, so
             * start search from the while(checkAgain) loop again.
             * This would be horribly inefficient if we had large numbers
             * of interest structures, but I don't think we will.
             */
            checkAgain = TRUE;
            break;
         }
      }
   }   
}


EvtInterest *interest_findType(CmsMsgType msgType)
{
   EvtInterest *evtInterest;

   dlist_for_each_entry(evtInterest, &evtInterestHead, dlist)
   {
      if (evtInterest->type == msgType)
      {
         return evtInterest;
      }
   }
   
   return NULL;
}


EvtInterestInfo *interest_findInfo(EvtInterest *evtInterest, CmsEntityId eid)
{
   EvtInterestInfo *evtInfo;
   
   dlist_for_each_entry(evtInfo, &(evtInterest->evtInfoHead), dlist)
   {
      if (evtInfo->eid == eid)
      {
         return evtInfo;
      }
   }
   
   return NULL;
}






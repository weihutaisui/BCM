/***********************************************************************
 *
 *  Copyright (c) 2012  Broadcom Corporation
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

#include <stdio.h>
#include <signal.h>
#include <errno.h>

#include "cms.h"
#include "cms_mdm.h"
#include "cms_lck.h"
#include "cms_obj.h"
#include "cms_util.h"
#include "cms_linklist.h"
#include "cms_msg.h"
#include "cms_msg_modsw.h"
#include "modsw.h"

#include "linmosd_private.h"

/*!\file linmosd_duresp.c
 * \brief This file contains functions to handle async response messages
 *        that linmosd needs to send to the requestor of a DU state change.
 *
 */

LIST_TYPE duResponseList={NULL, NULL};


void createDuResponse(CmsMsgHeader *msg, const char *operation,
                      const char *uuid, UINT16 reqId)
{
   CmsMsgHeader *rmsg;
   DUresponseStateChangedMsgBody *respBody;
   ENTRY_TYPE *listEntry;

   rmsg = cmsMem_alloc(sizeof(CmsMsgHeader)+sizeof(DUresponseStateChangedMsgBody),
                       ALLOC_ZEROIZE);
   if (rmsg == NULL)
   {
      cmsLog_error("Could not allocate response msg!");
      return;
   }

   listEntry = cmsMem_alloc(sizeof(ENTRY_TYPE), ALLOC_ZEROIZE);
   if (listEntry == NULL)
   {
      cmsLog_error("could not allocate LIST_ENTRY!");
      cmsMem_free(rmsg);
      return;
   }

   /* fill in response msg header */
   rmsg->type = (CmsMsgType) CMS_MSG_RESPONSE_DU_STATE_CHANGE;
   rmsg->dst = msg->src;
   rmsg->src = EID_LINMOSD;
   rmsg->dataLength = sizeof(DUresponseStateChangedMsgBody);
   rmsg->flags_response = 1;
   rmsg->wordData = 1; /* just one response in this message */

   /* just fill in operation and uuid for now */
   respBody = (DUresponseStateChangedMsgBody *) (rmsg+1);
   cmsUtl_strncpy(respBody->operation, operation, sizeof(respBody->operation));
   cmsUtl_strncpy(respBody->UUID, uuid, sizeof(respBody->UUID));
   respBody->reqId = reqId;

   /* fill in linked list entry and link into list. */
   listEntry->keyType = KEY_STRING;
   listEntry->key = (void *) respBody->UUID;
   listEntry->data = rmsg;
   addFront(listEntry, &duResponseList);

   return;
}

void populateDuResponse(const char *uuid)
{
   ENTRY_TYPE *listEntry=NULL;
   ENTRY_TYPE *prevEntry=NULL;
   CmsMsgHeader *rmsg;
   DUresponseStateChangedMsgBody *respBody;
   DUObject *duObject=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found=FALSE;
   CmsRet ret;

   if (!findEntry(&duResponseList, uuid, KEY_STRING, &prevEntry, &listEntry))
   {
      cmsLog_error("Could not find entry for uuid %s", uuid);
      return;
   }

   rmsg = (CmsMsgHeader *) listEntry->data;
   respBody = (DUresponseStateChangedMsgBody *) (rmsg+1);

   if ((ret = cmsLck_acquireLockWithTimeout(LINMOSD_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
       cmsLog_error("failed to get lock, ret=%d", ret);
       cmsLck_dumpInfo();
       return;
   }

   while (!found &&
          (ret = cmsObj_getNext(MDMOID_DU, &iidStack, (void **)&duObject)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(duObject->UUID, uuid))
      {
         MdmPathDescriptor pathDesc;
         char *fullpath=NULL;

         /* getting the DUlist is a little complicated: get the fullpath
          * of this DUObject.
          */
         memset(&pathDesc, 0, sizeof(MdmPathDescriptor));
         pathDesc.oid = MDMOID_DU;
         pathDesc.iidStack = iidStack;
         cmsMdm_pathDescriptorToFullPath(&pathDesc, &fullpath);
         cmsUtl_strncpy(respBody->DUlist, fullpath, sizeof(respBody->DUlist));
         CMSMEM_FREE_BUF_AND_NULL_PTR(fullpath);

         /* the rest is easy: just copy string from DUObject */
         cmsUtl_strncpy(respBody->URL, duObject->URL, sizeof(respBody->URL));
         cmsUtl_strncpy(respBody->version, duObject->version, sizeof(respBody->version));
         cmsUtl_strncpy(respBody->EUlist, duObject->executionUnitList, sizeof(respBody->EUlist));

         found = TRUE;
      }

      cmsObj_free((void **)&duObject);
   }


   cmsLck_releaseLock();
}


void sendDuResponseByUuid(const char *uuid, int faultCode)
{
   CmsMsgHeader *rmsg;
   DUresponseStateChangedMsgBody *respBody;
   ENTRY_TYPE *listEntry;
   CmsRet ret;

   listEntry = removeStrEntry(&duResponseList, uuid);
   if (listEntry == NULL)
   {
      cmsLog_error("Could not find entry uuid=%s", uuid);
      return;
   }

   rmsg = (CmsMsgHeader *) listEntry->data;
   respBody = (DUresponseStateChangedMsgBody *) (rmsg+1);
   respBody->faultCode = faultCode;

   ret = cmsMsg_send(msgHandle, rmsg);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Send DU_RESPONSE msg failed, ret=%d", ret);
   }
   else
   {
      cmsLog_debug("DU response msg sent, dst=%d", rmsg->dst);
   }

   cmsMem_free(listEntry);
   cmsMem_free(rmsg);

   return;
}



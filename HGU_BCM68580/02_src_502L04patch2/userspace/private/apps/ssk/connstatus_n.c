/***********************************************************************
 *
 *  Copyright (c) 2010  Broadcom Corporation
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

#ifdef DMP_X_BROADCOM_COM_AUTODETECTION_1

#include "cms.h"
#include "cms_util.h"
#include "prctl.h"
#include "cms_core.h"
#include "cms_dal.h"
#include "cms_msg.h"
#include "ssk.h"


/*!\file connstatus-n.c
 * \brief  This file handles auto detection (for now, just one ipoe or pppoe WAN connetion) on the
 *         connection status and updates the MDM with that status.
 *
 */


/** Need to keep track of the wan connection object for to be tried on.
 *  The first one is usually the lastConnected one.
 */
 typedef struct
{
   DlistNode dlist;   
   MdmObjectId oid;           /**< Object Identifier */
   InstanceIdStack iidStack;  /**< Instance Id Stack. for the ip/ppp Conn Object */
   UBOOL8 isUsed;             /**< if TRUE, this WanConnInfo is the first one to be connected (was connected last time) */
} WanConnInfo;

/** wanConnInfoHead is the head of a linked list of wanConnInfo structs. */
DLIST_HEAD(wanConnInfoHead);


/**
 * isConnectedWanExited is used in auto detection mode and if it is TRUE, one connected wan existed.
 * isWanLinkUp is the layer 2 link status.  If TRUE, the link is up.
 */
static UBOOL8 isConnectedWanExited = FALSE;
static UBOOL8 isWanLinkUp = FALSE;


static void freeWanConnInfoList(void)
{
   WanConnInfo *tmp = NULL;

   while (dlist_empty(&wanConnInfoHead) == 0)
   {
      tmp = (WanConnInfo *) wanConnInfoHead.next;
      cmsLog_debug("Free wanConnInfo iidstack: %s", cmsMdm_dumpIidStack(&(tmp->iidStack)));
      dlist_unlink((DlistNode *) tmp);
      cmsMem_free(tmp);
   }
   
   cmsLog_debug("Done free wanConnInfo list.");
}


static UBOOL8 insertWanConnInfo(MdmObjectId oid, const InstanceIdStack *iidStack, UBOOL8 firstOneToUse)
{
   WanConnInfo *wanConnInfo=NULL;
   UBOOL8 inserted=TRUE;
   
   /* allocate the struct to be added to the list */
   if ((wanConnInfo = cmsMem_alloc(sizeof(WanConnInfo), ALLOC_ZEROIZE)) == NULL)
   {
      cmsLog_error("wanConnInfo allocation failed");
      inserted = FALSE;
   }
   else
   {
      wanConnInfo->iidStack = *iidStack;
      wanConnInfo->oid = oid;
      wanConnInfo->isUsed = firstOneToUse;

      /* append this to the end of the list */
      dlist_prepend((DlistNode *) wanConnInfo, &wanConnInfoHead);
   }
   
   return inserted;
   
}


static CmsRet getWanConnObjFromWanConnInfo(const WanConnInfo *wanConnInfo,  void **wanConnObj)
{
   CmsRet ret;

   if ((ret = cmsObj_get(wanConnInfo->oid,  &(wanConnInfo->iidStack), 0, (void **) wanConnObj)) != CMSRET_SUCCESS)
   {
      /* should not come to here but if it did, release the linked list below */
      cmsLog_debug("Failed to get wanConnObj.  ret=%d, oid=%d, iidStack=%s", 
         ret, wanConnInfo->oid, cmsMdm_dumpIidStack(&(wanConnInfo->iidStack)));
   }

   return ret;
}



static void startWanConnection(void)
{
   WanConnInfo *wanConnInfo=NULL;
   void *wanConnObj=NULL;
   UBOOL8 started=FALSE;
   
   dlist_for_each_entry(wanConnInfo, &wanConnInfoHead, dlist)
   {
      cmsLog_debug("->oid=%d, wanConnInfo->iidStack=%s, wanConnInfo->isUsed=%d", 
         wanConnInfo->oid,  cmsMdm_dumpIidStack(&wanConnInfo->iidStack), wanConnInfo->isUsed);
      if (wanConnInfo->isUsed)
      {
         if (getWanConnObjFromWanConnInfo(wanConnInfo, &wanConnObj) == CMSRET_SUCCESS)
         {
            cmsLog_debug("starting wanConn with isUsed==TRUE...(link status is %d)...", isWanLinkUp);
            updateSingleWanConnStatusLocked(&(wanConnInfo->iidStack), wanConnObj, isWanLinkUp);
            cmsObj_free((void **) &wanConnObj);
            started = TRUE;
            break;
         }
      }
   }

   if (!started)
   {
      dlist_for_each_entry(wanConnInfo, &wanConnInfoHead, dlist)
      {
         cmsLog_debug("No isUsed found.  Use the first one.  wanConnInfo->oid=%d", wanConnInfo->oid);
         if (getWanConnObjFromWanConnInfo(wanConnInfo, &wanConnObj) == CMSRET_SUCCESS)
         {
             cmsLog_debug("starting wanConn when no isUsed found,  wanConnInfo->iidStack=%s",  cmsMdm_dumpIidStack(&wanConnInfo->iidStack));
             updateSingleWanConnStatusLocked(&(wanConnInfo->iidStack), wanConnObj, isWanLinkUp);
             cmsObj_free((void **) &wanConnObj);
             started = TRUE;
             wanConnInfo->isUsed = TRUE;
             break;
         }
      }
   }

   if (started)
   {
      cmsLog_debug("wanConnInfo->oid=%d is started...", wanConnInfo->oid);
   }

}

static void stopWanConnection(void)
{
   WanConnInfo *wanConnInfo=NULL;
   void *wanConnObj=NULL;
   
   dlist_for_each_entry(wanConnInfo, &wanConnInfoHead, dlist)
   {
      cmsLog_debug("wanConnInfo->oid=%d, wanConnInfo->isUsed=%d", wanConnInfo->oid, wanConnInfo->isUsed);
      if (wanConnInfo->isUsed)
      {
         if (getWanConnObjFromWanConnInfo(wanConnInfo, &wanConnObj) == CMSRET_SUCCESS)
         {
             cmsLog_debug("stop wanConn (link status is %d)...", isWanLinkUp);
             updateSingleWanConnStatusLocked(&(wanConnInfo->iidStack), wanConnObj, isWanLinkUp);
             cmsObj_free((void **) &wanConnObj);
         }
         break;
      }
   }

}



void addNewWanConnObj(const InstanceIdStack *iidStack, void *wanConnObj)
{
   UBOOL8 inserted = TRUE;
   UBOOL8 found = FALSE;
   WanConnInfo *wanConnInfo=NULL;
   MdmObjectId wanConnOid = GET_MDM_OBJECT_ID(wanConnObj);
      
   cmsLog_debug("=%d, iidstack: %s", wanConnOid, cmsMdm_dumpIidStack(iidStack));

   /* This is only called whne the layer 2 link is up in case.  In updateWanConnStatusInSubtreeLocked_n 
   * this boolean will be set accordingly when link up/down happens
   */
   isWanLinkUp = TRUE;        
   
   dlist_for_each_entry(wanConnInfo, &wanConnInfoHead, dlist)
   {
      if (!cmsMdm_compareIidStacks(iidStack, &(wanConnInfo->iidStack)) && 
         wanConnOid == wanConnInfo->oid)        
      {
         found = TRUE;
         break;
      }
   }

   if (found)
   {
      cmsLog_debug("Already found. No adding");
      return;
   }
   
   if (wanConnOid == MDMOID_WAN_IP_CONN)
   {
      WanIpConnObject *ipConn = (WanIpConnObject *) wanConnObj;

      inserted = insertWanConnInfo(wanConnOid, iidStack, ipConn->X_BROADCOM_COM_LastConnected);
      cmsObj_free((void **) &ipConn);
   }
   else if (wanConnOid == MDMOID_WAN_PPP_CONN)
   {
      WanPppConnObject *pppConn = (WanPppConnObject *) wanConnObj;
   
      inserted = insertWanConnInfo(wanConnOid, iidStack, pppConn->X_BROADCOM_COM_LastConnected);
      cmsObj_free((void **) &pppConn);
   }

   if (!inserted)
   {
      cmsLog_error("Failed to insert the wanConnInfo node.");
      freeWanConnInfoList();
      return;
   }

}


void  stopAllWanConn(const InstanceIdStack *parentIidStack, UBOOL8 oldAutoDetectEnabled)
{
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   WanIpConnObject *ipConn=NULL;
   WanPppConnObject *pppConn=NULL;
   
   cmsLog_debug("iidStack=%s",  cmsMdm_dumpIidStack(parentIidStack));

   while (cmsObj_getNextInSubTree(MDMOID_WAN_IP_CONN, parentIidStack, &iidStack, (void **) &ipConn) == CMSRET_SUCCESS)
   {
      updateSingleWanConnStatusLocked(&iidStack, (void *) ipConn, FALSE);
      cmsObj_free((void **) &ipConn);
   }
 
   INIT_INSTANCE_ID_STACK(&iidStack);
   while (cmsObj_getNextInSubTree(MDMOID_WAN_PPP_CONN, parentIidStack, &iidStack, (void **) &pppConn) == CMSRET_SUCCESS)
   {
      updateSingleWanConnStatusLocked(&iidStack,  (void *) pppConn, FALSE);
      cmsObj_free((void **) &pppConn);
   }
 
   /* Need to free  the linked list for auto detect is Enabled */
   if (oldAutoDetectEnabled)
   {
      freeWanConnInfoList();
   }

   /* There should be no connected WAN service */
   isConnectedWanExited = FALSE;
   
}


void updateAutoDetectWanConnListForDeletion(const InstanceIdStack *iidStack, MdmObjectId wanConnOid)
{
   WanConnInfo *wanConnInfo=NULL;
      
   cmsLog_debug("=%d, iidstack: %s", wanConnOid,  cmsMdm_dumpIidStack(iidStack));

   dlist_for_each_entry(wanConnInfo, &wanConnInfoHead, dlist)
   {
      if (!cmsMdm_compareIidStacks(iidStack, &(wanConnInfo->iidStack)) && wanConnOid == wanConnInfo->oid)        
      {
         if (wanConnInfo->isUsed)
         {
            isConnectedWanExited = FALSE;
         }
         cmsLog_debug("Remove oid=%d, (isUsed=%d)  from the wanConnInfo list", wanConnOid, wanConnInfo->isUsed);         
         dlist_unlink((DlistNode *) wanConnInfo);
         cmsMem_free(wanConnInfo);
         break;
      }
   }
}


void processAutoDetectTask(void)
{
   WanConnInfo *wanConnInfo=NULL;
   void *wanConnObj=NULL;
   CmsRet ret;
   UBOOL8 foundConnected=FALSE;
   UBOOL8 getWanConnFailed=FALSE;

   cmsLog_debug("In processAutoDetectTask, isWanLinkUp=%d, isConnectedWanExited=%d", isWanLinkUp, isConnectedWanExited);

   dlist_for_each_entry(wanConnInfo, &wanConnInfoHead, dlist)
   {
      cmsLog_debug("Debug list:  wanConnInfo->oid=%d, iidStack=%s isUsed=%d", 
         wanConnInfo->oid, cmsMdm_dumpIidStack(&(wanConnInfo->iidStack)), wanConnInfo->isUsed);
   }         

   if (isWanLinkUp && isConnectedWanExited)
   {
      cmsLog_debug("Link is up and one connected WAN existed already. Do nothing.");
      return;
   }

   if (!isWanLinkUp)
   {
      cmsLog_debug("Link down is handled in updateWanConnStatusInSubtreeLocked_n.  Do nothing.");
      return;
   }

   dlist_for_each_entry(wanConnInfo, &wanConnInfoHead, dlist)
   {
      cmsLog_debug("->oid=%d, iidStack=%s isUsed=%d", 
         wanConnInfo->oid, cmsMdm_dumpIidStack(&(wanConnInfo->iidStack)), wanConnInfo->isUsed);
      if ((ret = getWanConnObjFromWanConnInfo(wanConnInfo, &wanConnObj)) != CMSRET_SUCCESS)
      {
         getWanConnFailed = TRUE;
         break;
      }
      else
      {
         if (wanConnInfo->isUsed)
         {
            if (wanConnInfo->oid == MDMOID_WAN_IP_CONN)
            {
               WanIpConnObject *wanIpConnObj = (WanIpConnObject *) wanConnObj;         
               foundConnected = !cmsUtl_strcmp(wanIpConnObj->connectionStatus, MDMVS_CONNECTED);
            }
            else if (wanConnInfo->oid == MDMOID_WAN_PPP_CONN)
            {
               WanPppConnObject *wanPppConnObj = (WanPppConnObject *) wanConnObj;         
               foundConnected = !cmsUtl_strcmp(wanPppConnObj->connectionStatus, MDMVS_CONNECTED);
            }

            if (!foundConnected)
            {
               if (wanConnInfo->oid == MDMOID_WAN_IP_CONN)
               {
                  WanIpConnObject *wanIpConnObj = (WanIpConnObject *) wanConnObj; 
                  wanIpConnObj->X_BROADCOM_COM_LastConnected = FALSE;
               }
               else if (wanConnInfo->oid == MDMOID_WAN_PPP_CONN)
               {
                  WanPppConnObject *wanPppConnObj = (WanPppConnObject *) wanConnObj;         
                  wanPppConnObj->X_BROADCOM_COM_LastConnected = FALSE;
               }

               /* stop this wan conn interface by giving it calling updateSingleWanConnStatusLocked with wanLink being FALSE */
               updateSingleWanConnStatusLocked(&(wanConnInfo->iidStack), wanConnObj, FALSE);

               /* Remove this from this list and then set the isUsed to FALSE, and then append to the end 
               * of the list again so that it will not be tried if there are any wan conn in front of it.
               */
               dlist_unlink((DlistNode *) wanConnInfo);
               wanConnInfo->isUsed = FALSE;
               dlist_prepend((DlistNode *) wanConnInfo, &wanConnInfoHead);
            }
            /* break out of the loop, either foundConnected or !foundConnected */
            break;
         }
         else
         {
            cmsLog_debug("isUsed == FALSE, try next one");
         }
         
         cmsObj_free((void **) &wanConnObj);
      }

      if (foundConnected)
      {
         cmsLog_debug("found connected wan and break the loop");
         break;
      }
      
   }

   if (getWanConnFailed)
   {
      cmsLog_error("Free the WanConnInfo list");   
      /* Need to free the linked list */
      freeWanConnInfoList();
   }
   else 
   {
      if (foundConnected)
      {
         cmsLog_debug("wan connection is up.");
         isConnectedWanExited = TRUE;
      }    
      else
      {
         cmsLog_debug("wan connection not found or no isUsed==TRUE.  Need to start a new one");
         startWanConnection();
      }
   }

   cmsLog_debug("return from processAutoDetectTask");   
   
}



void updateWanConnStatusInSubtreeLocked_n(const InstanceIdStack *parentIidStack, UBOOL8 isLinkUp, UBOOL8 startNewConnection)
{
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   WanIpConnObject *ipConn=NULL;
   WanPppConnObject *pppConn=NULL;
   UBOOL8 inserted=TRUE;
   
   cmsLog_debug("isLinkUp=%d iidStack=%s", isLinkUp, cmsMdm_dumpIidStack(parentIidStack));

   isWanLinkUp = isLinkUp;

   if (isWanLinkUp)
   {   
      freeWanConnInfoList();
      /* Need to build the linked list each time when the layer 2 link is up */
      while (inserted && cmsObj_getNextInSubTree(MDMOID_WAN_IP_CONN, parentIidStack, &iidStack, (void **) &ipConn) == CMSRET_SUCCESS)
      {
         inserted = insertWanConnInfo(MDMOID_WAN_IP_CONN, &iidStack, ipConn->X_BROADCOM_COM_LastConnected);
         cmsObj_free((void **) &ipConn);
      }

      INIT_INSTANCE_ID_STACK(&iidStack);
      while (inserted && cmsObj_getNextInSubTree(MDMOID_WAN_PPP_CONN, parentIidStack, &iidStack, (void **) &pppConn) == CMSRET_SUCCESS)
      {
         inserted = insertWanConnInfo(MDMOID_WAN_PPP_CONN, &iidStack, pppConn->X_BROADCOM_COM_LastConnected);
         cmsObj_free((void **) &pppConn);
      }

      if (!inserted)
      {
         cmsLog_error("Failed to insert the wanConnInfo node.");
         freeWanConnInfoList();
         return;
      }

      if (startNewConnection)
      {
         startWanConnection();
      }        
      
   }
   else  /* isWanLinkUp == FASLE */
   {
      stopWanConnection();
      /* Need to free  the linked list each time when the link is down */
      freeWanConnInfoList();
   }
   
}


#endif /* DMP_X_BROADCOM_COM_AUTODETECTION_1 */
          


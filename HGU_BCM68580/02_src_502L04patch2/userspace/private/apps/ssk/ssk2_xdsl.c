/* 
* <:copyright-BRCM:2011:proprietary:standard
* 
*    Copyright (c) 2011 Broadcom 
*    All Rights Reserved
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
*/

#ifdef DMP_DEVICE2_BASELINE_1
#ifdef DMP_DEVICE2_DSL_1


#include "cms.h"
#include "cms_util.h"
#include "cms_core.h"
#include "cms_qdm.h"
#include "ssk.h"
#include "AdslMibDef.h"
#include "devctl_adsl.h"
#include "bcmnetlink.h"
#include "devctl_xtm.h"

/*!\file ssk2_xdsl.c
 *
 * This file contains TR181 related DSL code, including bonding and
 * DSL diagnostics.
 */
extern UBOOL8 dslDiagInProgress;
#ifdef DMP_DSLDIAGNOSTICS_1
extern dslLoopDiag dslLoopDiagInfo;
#endif

void processXdslCfgSaveMessage_dev2(UINT32 msgId)
{
    long    dataLen;
    char    oidStr[] = { 95 };      /* kOidAdslPhyCfg */
    adslCfgProfile  adslCfg;
    CmsRet          cmsRet;
    Dev2DslLineObject *dslLineObj = NULL;
    InstanceIdStack         iidStack = EMPTY_INSTANCE_ID_STACK;
    
    dataLen = sizeof(adslCfgProfile);
    cmsRet = xdslCtl_GetObjectValue(0, oidStr, sizeof(oidStr), (char *)&adslCfg, &dataLen);
    
    if( cmsRet != (CmsRet) BCMADSL_STATUS_SUCCESS) {
        cmsLog_error("Could not get adsCfg, ret=%d", cmsRet);
        return;
    }
    
    if ((cmsRet = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS) {
        cmsLog_error("Could not get lock, ret=%d", cmsRet);
        cmsLck_dumpInfo();
        /* just a kernel event, I guess we can try later. */
        return;
    }
    
    cmsRet = cmsObj_getNext(MDMOID_DEV2_DSL_LINE, &iidStack, (void **) &dslLineObj);
    if (cmsRet != CMSRET_SUCCESS) {
        cmsLck_releaseLock();
        cmsLog_error("Could not get DSL intf cfg, ret=%d", cmsRet);
        return;
    }
    
    if(MSG_ID_BRCM_SAVE_DSL_CFG_ALL == msgId)
        xdslUtil_IntfCfgInit_dev2(&adslCfg, dslLineObj);
#if defined(SUPPORT_MULTI_PHY) || defined(SUPPORT_DSL_GFAST)
    else if (MSG_ID_BRCM_SAVE_DSL_PREFERRED_LINE == msgId) {
        dslLineObj->X_BROADCOM_COM_DslPhyMiscCfgParam &= ~(BCM_PREFERREDTYPE_FOUND | BCM_MEDIATYPE_MSK);
        dslLineObj->X_BROADCOM_COM_DslPhyMiscCfgParam |= (adslCfg.xdslMiscCfgParam & (BCM_PREFERREDTYPE_FOUND | BCM_MEDIATYPE_MSK));
    }
#endif
    else {
        cmsObj_free((void **) &dslLineObj);
        cmsLck_releaseLock();
        return;
    }
    
    cmsRet = cmsObj_set(dslLineObj, &iidStack);
    if (cmsRet != CMSRET_SUCCESS)
        cmsLog_error("Could not set DSL intf cfg, ret=%d", cmsRet);
    else
        cmsRet = cmsMgm_saveConfigToFlash();
    
    cmsObj_free((void **) &dslLineObj);
    
    cmsLck_releaseLock();
    
    if(cmsRet != CMSRET_SUCCESS)
        cmsLog_error("Writing  Xdsl Cfg to flash.failed!");
}


/* called from updateLinkStatus_dev2 */
void checkDslLinkStatusLocked_dev2()
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2DslLineObject *dslLineObj=NULL;
   CmsMsgType msgType;
   UBOOL8 isLinkStatusChanged = FALSE;
#ifdef DMP_DEVICE2_FAST_1
   UBOOL8 done = FALSE;
   Dev2FastLineObject *fastLineObj=NULL;   
#endif

   while (cmsObj_getNext(MDMOID_DEV2_DSL_LINE, &iidStack,
                                    (void **) &dslLineObj) == CMSRET_SUCCESS)
   {
      cmsLog_debug("IfName=%s status=%s",
                   dslLineObj->name, dslLineObj->status);

      isLinkStatusChanged = comparePreviousLinkStatus(dslLineObj->name,
                                                      dslLineObj->upstream,
                                                      dslLineObj->status);

      if (isLinkStatusChanged)
      {
#ifdef DMP_DEVICE2_FAST_1
         done = TRUE;
#endif
         if (!cmsUtl_strcmp(dslLineObj->status, MDMVS_UP))
         {
            printf("(ssk) DSL LINE %s link up\n", dslLineObj->name);
            msgType = CMS_MSG_WAN_LINK_UP;
         }
         else
         {
            printf("(ssk) DSL LINE %s link down\n", dslLineObj->name);
            msgType = CMS_MSG_WAN_LINK_DOWN;
         }

         intfStack_propagateStatusByIidLocked(MDMOID_DEV2_DSL_LINE,
                                              &iidStack, dslLineObj->status);

         sendStatusMsgToSmd(msgType, dslLineObj->name);
      }
      
      cmsObj_free((void **) &dslLineObj);
   }

#ifdef DMP_DEVICE2_FAST_1
   INIT_INSTANCE_ID_STACK(&iidStack);
   while (!done && cmsObj_getNext(MDMOID_DEV2_FAST_LINE, &iidStack,
                                  (void **) &fastLineObj) == CMSRET_SUCCESS)
   {
      cmsLog_debug("IfName=%s status=%s",
                   fastLineObj->name, fastLineObj->status);

      isLinkStatusChanged = comparePreviousLinkStatus(fastLineObj->name,
                                                      fastLineObj->upstream,
                                                      fastLineObj->status);

      if (isLinkStatusChanged)
      {
         done = TRUE;
         if (!cmsUtl_strcmp(fastLineObj->status, MDMVS_UP))
         {
            printf("(ssk) FAST LINE %s link up\n", fastLineObj->name);
            msgType = CMS_MSG_WAN_LINK_UP;
         }
         else
         {
            printf("(ssk) FAST LINE %s link down\n", fastLineObj->name);
            msgType = CMS_MSG_WAN_LINK_DOWN;
         }

         intfStack_propagateStatusByIidLocked(MDMOID_DEV2_FAST_LINE,
                                              &iidStack, fastLineObj->status);

         sendStatusMsgToSmd(msgType, fastLineObj->name);
      }
      
      cmsObj_free((void **) &fastLineObj);
   }
#endif /* DMP_DEVICE2_FAST_1 */   
   return;
}



#ifdef TODO_LATER
/* this is needed, special handling in interface stack to call this one here for PTM mode */
/* we need to have the iidstack of the exact ptm link, and not the parentIidStack */
#ifdef DMP_PTMWAN_1

void informErrorSampleStatusChangeLocked_dev2(const InstanceIdStack *parentIidStack)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanPtmLinkCfgObject *ptmLinkCfg = NULL;

   cmsLog_debug("Inform Error sample status");
   while (cmsObj_getNextInSubTree(MDMOID_WAN_PTM_LINK_CFG, parentIidStack, &iidStack, (void **)&ptmLinkCfg) == CMSRET_SUCCESS)
   {
      if (ptmLinkCfg->enable)
      {
        sendStatusMsgToSmd(CMS_MSG_WAN_ERRORSAMPLES_AVAILABLE, ptmLinkCfg->X_BROADCOM_COM_IfName);
        cmsLog_debug("ErrorSamplesAvailable event received");
      }
      cmsObj_free((void **) &ptmLinkCfg);
      if (ret != CMSRET_SUCCESS)
      {
        cmsLog_error("Failed to set ptmLinkCfg. ret=%d", ret);
        return;
      }
   }
}
#endif /* DMP_PTMWAN_1 */
#endif /* TODO_LATER */


#ifdef DMP_DSLDIAGNOSTICS_1
void processWatchDslLoopDiag_dev2(CmsMsgHeader *msg)
{
   Dev2DslDiagnosticsADSLLineTestObject *dslDiagObj;
   dslDiagMsgBody *info = (dslDiagMsgBody*) (msg+1);
   InstanceIdStack iidStack = info->iidStack;
   CmsRet ret;

   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) == CMSRET_SUCCESS)
   {
      if (cmsObj_get(MDMOID_ADSL_LINE_TEST, &iidStack, 0, (void **) &dslDiagObj) == CMSRET_SUCCESS)
      {
         CMSMEM_REPLACE_STRING(dslDiagObj->diagnosticsState,MDMVS_REQUESTED);
         if ((ret = cmsObj_set(dslDiagObj, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsObj_set returns error. ret=%d", ret);
         }
         else
         {
            dslDiagInProgress = TRUE;
            dslLoopDiagInfo.dslLoopDiagInProgress = TRUE;
            dslLoopDiagInfo.iidStack = info->iidStack;
            dslLoopDiagInfo.pollRetries = 0;
            dslLoopDiagInfo.src = msg->src;
         }
         cmsObj_free((void **) &dslDiagObj);
      }
      cmsLck_releaseLock();
   } /* aquire lock ok */
}

void getDslLoopDiagResults_dev2(void)
{
   Dev2DslDiagnosticsADSLLineTestObject *dslDiagObj;
   InstanceIdStack iidStack = dslLoopDiagInfo.iidStack;
   UINT32 lineId=0;

   cmsLog_debug("Enter: dslLoopDiagInfo.pollRetries %d, inProgress %d",dslLoopDiagInfo.pollRetries,
                dslLoopDiagInfo.dslLoopDiagInProgress);

   if ((cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) == CMSRET_SUCCESS)
   {
      if (cmsObj_get(MDMOID_ADSL_LINE_TEST, &iidStack, 0, (void **) &dslDiagObj) == CMSRET_SUCCESS)
      {
         if ((cmsUtl_strcmp(dslDiagObj->diagnosticsState, MDMVS_REQUESTED) == 0) &&
             (dslLoopDiagInfo.pollRetries < DIAG_DSL_LOOPBACK_TIMEOUT_PERIOD))
         {
            dslLoopDiagInfo.pollRetries++;
         }
         else
         {
            if (dslLoopDiagInfo.pollRetries >= DIAG_DSL_LOOPBACK_TIMEOUT_PERIOD)
            {
               CMSMEM_REPLACE_STRING(dslDiagObj->diagnosticsState,MDMVS_ERROR_INTERNAL);
               cmsObj_set(dslDiagObj, &iidStack);
            }
            dslDiagInProgress=FALSE;
            CmsMsgHeader msg = EMPTY_MSG_HEADER;
            dslLoopDiagInfo.dslLoopDiagInProgress = FALSE;
            dslLoopDiagInfo.pollRetries = 0;

            if (dslLoopDiagInfo.src == EID_TR69C)
            {
               msg.type = CMS_MSG_DSL_LOOP_DIAG_COMPLETE;
               msg.src =  EID_SSK;
               msg.dst = EID_TR69C;
               msg.flags_event = 1;
               qdmDsl_getLineIdFromChannelFullPathLocked_dev2(dslDiagObj->interface,&lineId);
               msg.wordData = lineId;
               if (cmsMsg_send(msgHandle, &msg) != CMSRET_SUCCESS)
               {
                  cmsLog_error("could not send out CMS_MSG_DSL_LOOP_DIAG_COMPLETE event msg");
               }
               else
               {
                  cmsLog_debug("Send out CMS_MSG_DSL_LOOP_DIAG_COMPLETE event msg.");
               }
            }
         }
         cmsObj_free((void **) &dslDiagObj);

      } /* get obj ok */
      cmsLck_releaseLock();
   } /* lock requested ok */
}
#endif /* DMP_DSLDIAGNOSTICS_1 */

#ifdef DMP_DEVICE2_BONDEDDSL_1
/* XDSL driver sends a Traffic Mismatch Message to SSK.   But at this point,
 * the driver has dynamically reconfigured everything--bond mode or non-bond mode.
 * To better reflect what mode the system is running in, this is what happens:
 *    1. CPE has bonding enabled, link is trained non-bonded.
 *    2. CPE has bonding disabled, link is trained bonded.  It doesn't matter what users configured,
 *         if the link is trained bonded, the configuration is changed to "bonding enabled".
 *         On the other hand, for case 1, the configuration is left alone to "bonding enabled".
 *     
 * If there is a traffic mismatch, this routine is called. And these will happen:
 *    1. bonding is enabled, and link traffic type is ATM or PTM, then the Bonding Group status is
 *       changed to PeerBondingSchemeMismatch.   ATM.Link or PTM.Link's lowerlayer is changed from
 *       BondingGroup.{i} to DSL.Channel.{i}.
 *    2. bonding is enabled, and link traffic type is now ATM_BONDED or PTM_BONDED, Bonding Group Status
 *       is set to None; and the ATM.Link or PTM.Link's lowerLayers is changed back to BondGroup.{i}.
 */
void updateXtmLowerLayerLocked()
{
   XTM_BOND_INFO bondInfo;
   UBOOL8 trainBonded=FALSE;
   UBOOL8 isATM=TRUE;

   /* get the bonding status from driver */
   memset(&bondInfo,0,sizeof(XTM_BOND_INFO));

   /* regardless of what the status this API returns, bondInfo will contain the traffic type info. */
   /* when error, it's not bonding mode. */
   devCtl_xtmGetBondingInfo (&bondInfo);

   switch (bondInfo.ulTrafficType)
   {
   case TRAFFIC_TYPE_ATM_BONDED:
      trainBonded = TRUE;
      isATM = TRUE;
      break;
   case TRAFFIC_TYPE_ATM:
      trainBonded=FALSE;
      isATM = TRUE;
      break;
   case TRAFFIC_TYPE_PTM_BONDED:
      trainBonded = TRUE;
      isATM = FALSE;
      break;
   case TRAFFIC_TYPE_PTM:
      trainBonded=FALSE;
      isATM=FALSE;
   default:
      break;
   } 

   if (isATM)
   {
      updateAtmLowerLayerLocked(trainBonded);
   }
   else
   {
      updatePtmLowerLayerLocked(trainBonded);
   }
} 

void updateAtmLowerLayerLocked(UBOOL8 trainBonded)
{
   Dev2AtmLinkObject *pAtmLinkObj;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   MdmPathDescriptor lowerLayerPathDesc;
   MdmPathDescriptor pathDesc;
   Dev2DslBondingGroupObject *bondingGroupObj=NULL;
   InstanceIdStack bondGroupIidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS; 
   char *newLowerLayerFullPath = NULL;
   UBOOL8 found = FALSE;
   char *pLowerLayer;
   char *tmp;

   cmsLog_debug("ATM: trained Bonded= %d",(int)trainBonded);

   /* loop through the ATM first */
   while (!found && (ret = cmsObj_getNext(MDMOID_DEV2_ATM_LINK, &iidStack, (void **)&pAtmLinkObj)) == CMSRET_SUCCESS)
   {
      /* we only support one bonding group and a fix numbers of channel, so this task
       * of figuring out new lowerLayer needs to be done once for all the ATM VCC.
       */
      if (pAtmLinkObj->enable == TRUE)
      {
         found = TRUE;

         cmsLog_debug("ATM: current LowerLayers is %s",pAtmLinkObj->lowerLayers);

         /* find out what the current LowerLayers is.  ATM link's lowerLayers should just be
          * a bonding group or a CSL of DSL.Channel. */
         INIT_PATH_DESCRIPTOR(&lowerLayerPathDesc);
         tmp = cmsMem_strdup(pAtmLinkObj->lowerLayers);
         pLowerLayer = strtok(tmp,",");
         ret = cmsMdm_fullPathToPathDescriptor(pLowerLayer, &lowerLayerPathDesc);
         CMSMEM_FREE_BUF_AND_NULL_PTR(tmp);
         if (ret == CMSRET_SUCCESS)
         {
            if (lowerLayerPathDesc.oid == MDMOID_DEV2_DSL_BONDING_GROUP)
            {
               if (trainBonded == FALSE)
               {
                  /* the lower interface needs to be changed to DSL.Channel that is UP now.
                   * and the bonding group's status needs update.
                   * Find out the lowerLayers of this bonding group now; and figure out the channel.
                   */
                  if (cmsObj_get(lowerLayerPathDesc.oid,&lowerLayerPathDesc.iidStack,
                                 0,(void **)&bondingGroupObj) == CMSRET_SUCCESS)
                  {
                     /* so I do not know which channel will be UP because there could be mediaSearch involved.
                      * I will make the lower layers point to either channels.
                      */
                     newLowerLayerFullPath = cmsMem_strdup(bondingGroupObj->lowerLayers);
                     cmsObj_free((void **) &bondingGroupObj);
                  }
               } /* not trainedBonded */
            } /* current LowerLayer is bondingGroup */
            else
            {
               /* current LowerLayer is non bonding, DSL.Channel */
               if (trainBonded == TRUE)
               {
                  /* the lower interface needs to be changed to bonding group, 
                   * find the bonding group that is up.
                   */
                  while ((ret = cmsObj_getNext(MDMOID_DEV2_DSL_BONDING_GROUP, &bondGroupIidStack, (void **)&bondingGroupObj)) == CMSRET_SUCCESS)
                  {
                     if (cmsUtl_isFullPathInCSL(pAtmLinkObj->lowerLayers,bondingGroupObj->lowerLayers))
                     {
                        found = TRUE;
                        INIT_PATH_DESCRIPTOR(&pathDesc);
                        pathDesc.oid = MDMOID_DEV2_DSL_BONDING_GROUP;
                        pathDesc.iidStack = bondGroupIidStack;
                        if ((ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &newLowerLayerFullPath)) == CMSRET_SUCCESS)
                        {
                           cmsObj_free((void **) &bondingGroupObj);
                           break;
                        }
                        cmsObj_free((void **) &bondingGroupObj);
                     }
                  } /* found the bonding group */
               } /* trained bonded mode */
            } 
         } /* lowerLayer */
         cmsObj_free((void **) &pAtmLinkObj);
      }
   } /* while  ATM LINK */

   /* update all the ATM LINK to new lower layer */
   if (found && (newLowerLayerFullPath != NULL))
   {
      INIT_INSTANCE_ID_STACK(&iidStack);
      while ((ret = cmsObj_getNext(MDMOID_DEV2_ATM_LINK, &iidStack, (void **)&pAtmLinkObj)) == CMSRET_SUCCESS)
      {
         CMSMEM_REPLACE_STRING(pAtmLinkObj->lowerLayers,newLowerLayerFullPath);
         if (cmsObj_set(pAtmLinkObj,&iidStack) != CMSRET_SUCCESS)
         {
            cmsLog_error("Fail to set new LowerLayers (%s) for ATM LINK",newLowerLayerFullPath);
         }
         cmsObj_free((void **) &pAtmLinkObj);
      }
      CMSMEM_FREE_BUF_AND_NULL_PTR(newLowerLayerFullPath);
   } /* found */
}

void updatePtmLowerLayerLocked(UBOOL8 trainBonded)
{
   Dev2PtmLinkObject *pPtmLinkObj;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   MdmPathDescriptor lowerLayerPathDesc;
   MdmPathDescriptor pathDesc;
   Dev2DslBondingGroupObject *bondingGroupObj=NULL;
   InstanceIdStack bondingGroupIidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS; 
   char *newLowerLayerFullPath = NULL;
   UBOOL8 found = FALSE;
   char *pLowerLayer;
   char *tmp;

   cmsLog_debug("PTM: trained Bonded= %d",(int)trainBonded);

   /* we only support one bonding group and a fix numbers of channel, so this task
    * of figuring out new lowerLayer needs to be done once for all the PTM link
    */
   while (!found && (ret = cmsObj_getNext(MDMOID_DEV2_PTM_LINK, &iidStack, (void **)&pPtmLinkObj)) == CMSRET_SUCCESS)
   {
      if (pPtmLinkObj->enable == TRUE)
      {
         /* all PTM link is the same, if one doesn't need to change, none needs to be changed */
         found = TRUE;

         cmsLog_debug("PTM: current LowerLayers is %s",pPtmLinkObj->lowerLayers);

         /* find out what the current LowerLayers is.  PTM link's lowerLayers should just be
          * a bonding group or a CSL of DSL.Channel. */
         INIT_PATH_DESCRIPTOR(&lowerLayerPathDesc);
         /* just pick the first channel in the list or just the bonding group */
         tmp = cmsMem_strdup(pPtmLinkObj->lowerLayers);
         pLowerLayer = strtok(tmp,",");
         ret = cmsMdm_fullPathToPathDescriptor(pLowerLayer, &lowerLayerPathDesc);
         CMSMEM_FREE_BUF_AND_NULL_PTR(tmp);
         if (ret == CMSRET_SUCCESS)
         {
            if (lowerLayerPathDesc.oid == MDMOID_DEV2_DSL_BONDING_GROUP)
            {
               if (trainBonded == FALSE)
               {
                  /* the lower interface needs to be changed to DSL.Channel that is UP now.
                   * and the bonding group's status needs update.
                   * Find out the lowerLayers of this bonding group now; and figure out the channel.
                   */
                  if (cmsObj_get(lowerLayerPathDesc.oid,&lowerLayerPathDesc.iidStack,
                                 OGF_NO_VALUE_UPDATE,(void **)&bondingGroupObj) == CMSRET_SUCCESS)
                  {
                     /* so I do not know which channel will be UP because there could be mediaSearch involved.
                      * I will make the lower layers point to either channels.
                      */
                     newLowerLayerFullPath = cmsMem_strdup(bondingGroupObj->lowerLayers);
                     cmsObj_free((void **) &bondingGroupObj);
                  }
               } /* not trainedBonded */
            } /* current LowerLayer is bondingGroup */
            else
            {
               /* current LowerLayer is non bonding, DSL.Channel */
               if (trainBonded == TRUE)
               {
                  /* the lower interface needs to be changed to bonding group, 
                   * find the bonding group that is up.
                   */
                  while ((ret = cmsObj_getNext(MDMOID_DEV2_DSL_BONDING_GROUP, &bondingGroupIidStack, (void **)&bondingGroupObj)) == CMSRET_SUCCESS)
                  {
                     if (cmsUtl_isFullPathInCSL(pPtmLinkObj->lowerLayers,bondingGroupObj->lowerLayers))
                     {
                        found = TRUE;
                        INIT_PATH_DESCRIPTOR(&pathDesc);
                        pathDesc.oid = MDMOID_DEV2_DSL_BONDING_GROUP;
                        pathDesc.iidStack = bondingGroupIidStack;
                        if ((ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &newLowerLayerFullPath)) == CMSRET_SUCCESS)
                        {
                           cmsObj_free((void **) &bondingGroupObj);
                           break;
                        }
                     } /* found the bonding group */
                     cmsObj_free((void **) &bondingGroupObj);
                  } /* while */
               } /* trained bonded mode */
            } 
         } /* lowerLayer */
         cmsObj_free((void **) &pPtmLinkObj);
      }
   } /* while  PTM LINK */

   /* update all the PTM LINK to new lower layer */
   if (found && (newLowerLayerFullPath != NULL))
   {
      INIT_INSTANCE_ID_STACK(&iidStack);
      while ((ret = cmsObj_getNext(MDMOID_DEV2_PTM_LINK, &iidStack, (void **)&pPtmLinkObj)) == CMSRET_SUCCESS)
      {
         CMSMEM_REPLACE_STRING(pPtmLinkObj->lowerLayers,newLowerLayerFullPath);
         if (cmsObj_set(pPtmLinkObj,&iidStack) != CMSRET_SUCCESS)
         {
            cmsLog_error("Fail to set new LowerLayers (%s) for PTM LINK",newLowerLayerFullPath);
         }
         cmsObj_free((void **) &pPtmLinkObj);
      }
      CMSMEM_FREE_BUF_AND_NULL_PTR(newLowerLayerFullPath);
   } /* found */
}

#endif /*  DMP_DEVICE2_BONDEDDSL_1 */

#endif /* DMP_DEVICE2_DSL_1  */
#endif /* DMP_DEVICE2_BASELINE_1 */

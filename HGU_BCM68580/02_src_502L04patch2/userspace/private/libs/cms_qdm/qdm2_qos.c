/***********************************************************************
 *
 *  Copyright (c) 2013  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2013:proprietary:standard

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

#ifdef DMP_DEVICE2_QOS_1


#include "cms.h"
#include "cms_mdm.h"
#include "cms_qos.h"
#include "cms_obj.h"
#include "cms_qdm.h"
#include "cms_util.h"
#include "cms_core.h"


UBOOL8 qdmQos_isQosEnabled_dev2(void)
{
   return TRUE;
}




/* I hate to call a RUT function from QDM, but I really don't want to
 * move that function.  I think this is the lesser of two evils.
 */
extern UINT32 rutQos_getQidMark(const char *queueIntfName,
                                UBOOL8 isWan,
                                UINT32 queueId,
                                UINT32 queuePrecedence,
                                char *schedulerAlgorithm,
                                SINT32 dslLat,
                                SINT32 ptmPriority);

CmsRet qdmQos_getQueueInfoByClassQueueLocked_dev2(SINT32 classQueue,
                                                  UBOOL8 *enabled,
                                                  UINT32 *qidMark,
                                                  char *l2IntfName)
{
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   Dev2QosQueueObject *qObj=NULL;
   CmsRet ret;

   if (classQueue <= 0)
   {
      cmsLog_error("invalid classQueue=%d", classQueue);
      return CMSRET_INVALID_ARGUMENTS;
   }

   /*
    * Since the classQueue is actually the instance number of the Queue obj,
    * we can manually fill in the iidStack and get the object without
    * searching for it.
    */
   iidStack.instance[0] = classQueue;
   iidStack.currentDepth = 1;
   if ((ret = cmsObj_get(MDMOID_DEV2_QOS_QUEUE, &iidStack, 0, (void **) &qObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_get MDMOID_DEV2_QOS_QUEUE  returns error. ret=%d", ret);
   }
   else
   {
      if (enabled != NULL)
      {
         *enabled = qObj->enable;
      }

      if (l2IntfName != NULL)
      {
         ret = qdmIntf_fullPathToIntfnameLocked(qObj->interface, l2IntfName);
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("could not convert %s to layer 2 IntfName",
                         qObj->interface);
         }
      }

      if (qidMark != NULL)
      {
         CmsQosQueueInfo qInfo;
         memset((void *) &qInfo, 0, sizeof(qInfo));
         qdmQos_convertDmQueueObjToCmsQueueInfoLocked(qObj, &qInfo);

         *qidMark = rutQos_getQidMark(qInfo.intfName,
                                      qInfo.isWan,
                                      qInfo.queueId,
                                      qInfo.queuePrecedence,
                                      qInfo.schedulerAlgorithm,
                                      qInfo.dslLatency,
                                      qInfo.ptmPriority);
      }

      cmsObj_free((void **) &qObj);
   }

   return ret;
}


void qdmQos_convertDmQueueObjToCmsQueueInfoLocked_dev2(const void *obj,
                                                 CmsQosQueueInfo *qInfo)
{
   Dev2QosQueueObject *qObj = (Dev2QosQueueObject *) obj;
   MdmObjectId oid = GET_MDM_OBJECT_ID(qObj);
   CmsRet ret;

   memset(qInfo, 0, sizeof(CmsQosQueueInfo));

   if (oid != MDMOID_DEV2_QOS_QUEUE)
   {
      cmsLog_error("Wrong object: got %d expected %d",
                   oid, MDMOID_DEV2_QOS_QUEUE);
      return;
   }

   qInfo->enable = qObj->enable;

   if (cmsUtl_strlen(qObj->X_BROADCOM_COM_QueueName) > (int) sizeof(qInfo->queueName) - 1)
   {
      cmsLog_error("QueueName %s is longer than buf, will be truncated",
                   qObj->X_BROADCOM_COM_QueueName);
   }
   cmsUtl_strncpy(qInfo->queueName, qObj->X_BROADCOM_COM_QueueName,
                  sizeof(qInfo->queueName));

   /* convert MDM fullpath to linux intf name */
   if (!IS_EMPTY_STRING(qObj->interface))
   {
      ret = qdmIntf_fullPathToIntfnameLocked(qObj->interface, qInfo->intfName);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not get intfName for %s, ret=%d",
                      qObj->interface, ret);
         return;
      }

      qInfo->isWan = qdmIntf_isLayer2IntfNameUpstreamLocked(qInfo->intfName);
   }

   qInfo->queueId = qObj->X_BROADCOM_COM_QueueId;

   if (cmsUtl_strlen(qObj->schedulerAlgorithm) > (int) sizeof(qInfo->schedulerAlgorithm) - 1)
   {
      cmsLog_error("SchedulerAlgorithm %s is longer than buf, will be truncated",
                   qObj->schedulerAlgorithm);
   }
   cmsUtl_strncpy(qInfo->schedulerAlgorithm, qObj->schedulerAlgorithm,
                  sizeof(qInfo->schedulerAlgorithm));

   qInfo->queuePrecedence = qObj->precedence;
   qInfo->queueWeight = qObj->weight;

   if (cmsUtl_strlen(qObj->dropAlgorithm) > (int) sizeof(qInfo->dropAlgorithm) - 1)
   {
      cmsLog_error("DropAlgorithm %s is longer than buf, will be truncated",
                   qObj->dropAlgorithm);
   }
   cmsUtl_strncpy(qInfo->dropAlgorithm, qObj->dropAlgorithm,
                  sizeof(qInfo->dropAlgorithm));

   qInfo->loMinThreshold = qObj->REDThreshold;
   qInfo->loMaxThreshold = qObj->X_BROADCOM_COM_LowClassMaxThreshold;
   qInfo->hiMinThreshold = qObj->X_BROADCOM_COM_HighClassMinThreshold;
   qInfo->hiMaxThreshold = qObj->X_BROADCOM_COM_HighClassMaxThreshold;

   qInfo->shapingRate = qObj->shapingRate;
   qInfo->shapingBurstSize = qObj->shapingBurstSize;

   qInfo->minBitRate = qObj->X_BROADCOM_COM_MinBitRate;
   qInfo->dslLatency = qObj->X_BROADCOM_COM_DslLatency;
   qInfo->ptmPriority = qObj->X_BROADCOM_COM_PtmPriority;

   return;
}


void qdmQos_convertDmClassObjToCmsClassInfoLocked_dev2(const void *obj,
                                                 CmsQosClassInfo *cInfo)
{
   Dev2QosClassificationObject *cObj = (Dev2QosClassificationObject *) obj;
   MdmObjectId oid = GET_MDM_OBJECT_ID(cObj);

   memset(cInfo, 0, sizeof(CmsQosClassInfo));

   if (oid != MDMOID_DEV2_QOS_CLASSIFICATION)
   {
      cmsLog_error("Wrong object: got %d expected %d",
                   oid, MDMOID_DEV2_QOS_CLASSIFICATION);
      return;
   }

   cInfo->enable = cObj->enable;

   if (cmsUtl_strlen(cObj->X_BROADCOM_COM_ClassName) > (int) sizeof(cInfo->name) - 1)
   {
      cmsLog_error("ClassName %s is longer than buf, will be truncated",
                   cObj->X_BROADCOM_COM_ClassName);
   }
   cmsUtl_strncpy(cInfo->name, cObj->X_BROADCOM_COM_ClassName,
                  sizeof(cInfo->name));

   cInfo->order = cObj->order;
   cInfo->key = cObj->X_BROADCOM_COM_ClassKey;
   cInfo->etherType = cObj->ethertype;
   cInfo->etherTypeExclude = cObj->ethertypeExclude;

   cmsUtl_strncpy(cInfo->ingressIntfFullPath, cObj->interface,
                  sizeof(cInfo->ingressIntfFullPath));
   if (!IS_EMPTY_STRING(cInfo->ingressIntfFullPath) &&
       cmsUtl_strcmp(cInfo->ingressIntfFullPath, MDMVS_LAN) &&
       cmsUtl_strcmp(cInfo->ingressIntfFullPath, MDMVS_WAN) &&
       cmsUtl_strcmp(cInfo->ingressIntfFullPath, MDMVS_LOCAL))
   {
      char intfNameBuf[CMS_IFNAME_LENGTH]={0};
      CmsRet r2;

      /* ingressIntfFullPath must be pointing to a specific interface */
      r2 = qdmIntf_fullPathToIntfnameLocked(cInfo->ingressIntfFullPath, intfNameBuf);
      if (r2 != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not convert %s to intfName, r2=%d",
                      cInfo->ingressIntfFullPath, r2);
      }
      else
      {
         if (qdmIpIntf_isIntfNameUpstreamLocked(intfNameBuf))
         {
            cInfo->ingressIsSpecificWan = TRUE;
         }
         else
         {
            cInfo->ingressIsSpecificLan = TRUE;
         }
      }
   }

   cmsUtl_strncpy(cInfo->destIP, cObj->destIP, sizeof(cInfo->destIP));
   cmsUtl_strncpy(cInfo->destMask, cObj->destMask, sizeof(cInfo->destMask));
   cInfo->destIPExclude = cObj->destIPExclude;

   cmsUtl_strncpy(cInfo->sourceIP, cObj->sourceIP, sizeof(cInfo->sourceIP));
   cmsUtl_strncpy(cInfo->sourceMask, cObj->sourceMask, sizeof(cInfo->sourceMask));
   cInfo->sourceIPExclude = cObj->sourceIPExclude;

   cInfo->protocol = cObj->protocol;
   cInfo->protocolExclude = cObj->protocolExclude;

   cInfo->destPort = cObj->destPort;
   cInfo->destPortRangeMax = cObj->destPortRangeMax;
   cInfo->destPortExclude = cObj->destPortExclude;

   cInfo->sourcePort = cObj->sourcePort;
   cInfo->sourcePortRangeMax = cObj->sourcePortRangeMax;
   cInfo->sourcePortExclude = cObj->sourcePortExclude;

   cmsUtl_strncpy(cInfo->destMACAddress, cObj->destMACAddress, sizeof(cInfo->destMACAddress));
   cmsUtl_strncpy(cInfo->destMACMask, cObj->destMACMask, sizeof(cInfo->destMACMask));
   cInfo->destMACExclude = cObj->destMACExclude;

   cmsUtl_strncpy(cInfo->sourceMACAddress, cObj->sourceMACAddress, sizeof(cInfo->sourceMACAddress));
   cmsUtl_strncpy(cInfo->sourceMACMask, cObj->sourceMACMask, sizeof(cInfo->sourceMACMask));
   cInfo->sourceMACExclude = cObj->sourceMACExclude;

   if (cmsUtl_strlen(cObj->sourceVendorClassID) > (int) sizeof(cInfo->sourceVendorClassID) - 1)
   {
      cmsLog_error("sourceVendorClassID %s is longer than buf, will be truncated",
                   cObj->sourceVendorClassID);
   }
   cmsUtl_strncpy(cInfo->sourceVendorClassID, cObj->sourceVendorClassID,
                  sizeof(cInfo->sourceVendorClassID));
   cInfo->sourceVendorClassIDExclude = cObj->sourceVendorClassIDExclude;

   /* Note in TR181, sourceUserClssID is HEXBINARY */
   if (cmsUtl_strlen(cObj->sourceUserClassID) > (int) sizeof(cInfo->sourceUserClassID) - 1)
   {
      cmsLog_error("sourceUserClassID %s is longer than buf, will be truncated",
                   cObj->sourceUserClassID);
   }
   cmsUtl_strncpy(cInfo->sourceUserClassID, cObj->sourceUserClassID,
                  sizeof(cInfo->sourceUserClassID));
   cInfo->sourceUserClassIDExclude = cObj->sourceUserClassIDExclude;

   cInfo->DSCPCheck = cObj->DSCPCheck;
   cInfo->DSCPExclude = cObj->DSCPExclude;
   cInfo->DSCPMark = cObj->DSCPMark;

   cInfo->ethernetPriorityCheck = cObj->ethernetPriorityCheck;
   cInfo->ethernetPriorityExclude = cObj->ethernetPriorityExclude;
   cInfo->ethernetPriorityMark = cObj->ethernetPriorityMark;

   cInfo->vlanIdTag = cObj->X_BROADCOM_COM_VLANIDTag;

   cmsUtl_strncpy(cInfo->egressIntfName, cObj->X_BROADCOM_COM_egressInterface,
                  sizeof(cInfo->egressIntfName));
   cInfo->egressQueueInstance = cObj->X_BROADCOM_COM_ClassQueue;

   cInfo->policerInstance = cObj->X_BROADCOM_COM_ClassPolicer;
   cInfo->classRate = cObj->X_BROADCOM_COM_ClassRate;

   return;
}


CmsRet qdmQos_referenceCheckLocked_dev2(CmsQosClassRefTargetEnum targ, SINT32 instance, UBOOL8 *isRefered)
{
   SINT32 refInstance=-1;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2QosClassificationObject *cObj=NULL;
   CmsRet ret;

   *isRefered = FALSE;

   while ((ret = cmsObj_getNext(MDMOID_DEV2_QOS_CLASSIFICATION, &iidStack,
                                (void **)&cObj)) == CMSRET_SUCCESS)
   {
      if (cObj->enable)
      {
         if (targ == CMS_QOS_REF_TARGET_POLICER)
         {
            refInstance = cObj->X_BROADCOM_COM_ClassPolicer;
         }
         else if (targ == CMS_QOS_REF_TARGET_QUEUE)
         {
            refInstance = cObj->X_BROADCOM_COM_ClassQueue;
         }
         else
         {
            cmsLog_error("Unsupported target selector %d", targ);
            ret = CMSRET_INVALID_ARGUMENTS;
         }

         cmsObj_free((void **)&cObj);

         if (refInstance == instance)
         {
            *isRefered = TRUE;
            break;
         }

         if (ret != CMSRET_SUCCESS)
         {
            break;
         }
      }
   }

   if (ret != CMSRET_NO_MORE_INSTANCES && ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_getNext <MDMOID_DEV2_QOS_CLASSIFICATION> returns error. ret=%d", ret);
      return ret;
   }

   return CMSRET_SUCCESS;
}


CmsRet qdmQos_getClassPolicerInfoLocked_dev2(SINT32 instance,
                                             CmsQosPolicerInfo *pInfo)
{
   Dev2QosPolicerObject *pObj=NULL;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   memset((void *) pInfo, 0, sizeof(CmsQosPolicerInfo));

   if (instance <= 0)
   {
      cmsLog_error("invalid instance=%d", instance);
      return CMSRET_INVALID_ARGUMENTS;
   }

   /* the fullpath of the Policer table is Device.QoS.Policer.{i}.
    * so we just need to push the instance number into the first position
    * of the instance id stack.
    */
   PUSH_INSTANCE_ID(&iidStack, instance);
   if ((ret = cmsObj_get(MDMOID_DEV2_QOS_POLICER, &iidStack, 0, (void **) &pObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_get <MDMOID_DEV2_QOS_POLICER> returns error. ret=%d", ret);
      return ret;
   }

   /* copy info from pObj to pInfo struct */
   pInfo->enable = pObj->enable;
   pInfo->committedRate = pObj->committedRate;
   pInfo->committedBurstSize = pObj->committedBurstSize;
   pInfo->excessBurstSize = pObj->excessBurstSize;
   pInfo->peakRate = pObj->peakRate;
   pInfo->peakBurstSize = pObj->peakBurstSize;
   cmsUtl_strncpy(pInfo->meterType, pObj->meterType, sizeof(pInfo->meterType));
   cmsUtl_strncpy(pInfo->conformingAction, pObj->conformingAction, sizeof(pInfo->conformingAction));
   cmsUtl_strncpy(pInfo->partialConformingAction, pObj->partialConformingAction, sizeof(pInfo->partialConformingAction));
   cmsUtl_strncpy(pInfo->nonConformingAction, pObj->nonConformingAction, sizeof(pInfo->nonConformingAction));
   cmsUtl_strncpy(pInfo->name, pObj->X_BROADCOM_COM_PolicerName, sizeof(pInfo->name));

   cmsObj_free((void **) &pObj);

   return ret;
}


UBOOL8 qdmQos_isEgressEthPortClassificationPresentLocked_dev2(UINT32 excludeClassKey)
{
   Dev2QosClassificationObject *cObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 present = FALSE;

   cmsLog_debug("Entered: excludeClassKey=0x%x", excludeClassKey);

   while (!present &&
          cmsObj_getNextFlags(MDMOID_DEV2_QOS_CLASSIFICATION , &iidStack,
                     OGF_NO_VALUE_UPDATE, (void **)&cObj) == CMSRET_SUCCESS)
   {
      cmsLog_debug("checking classKey=%d enable=%d status=%s classQueue=%d",
                   cObj->X_BROADCOM_COM_ClassKey,
                   cObj->enable,
                   cObj->status,
                   cObj->X_BROADCOM_COM_ClassQueue);

      if ((cObj->X_BROADCOM_COM_ClassKey != excludeClassKey) &&
          (cObj->enable == TRUE) &&
          (!cmsUtl_strcmp(cObj->status, MDMVS_ENABLED)) &&
          (cObj->X_BROADCOM_COM_ClassQueue > 0))
      {
         char intfNameBuf[CMS_IFNAME_LENGTH]={0};
         CmsRet r2;

         r2 = qdmQos_getQueueInfoByClassQueueLocked(cObj->X_BROADCOM_COM_ClassQueue,
                                                    NULL, NULL, intfNameBuf);
         if (r2 != CMSRET_SUCCESS)
         {
            cmsLog_error("Could not get classQueue %d, ret=%d",
                         cObj->X_BROADCOM_COM_ClassQueue, r2);
         }
         else
         {
            /* what about Moca? */
            /* We want to match both LAN and WAN Eth */
            /* XXX TODO: also need to match optical interfaces */
           if (!cmsUtl_strncmp(intfNameBuf, ETH_IFC_STR, strlen(ETH_IFC_STR)))
           {
              cmsLog_debug("Detected Egress Eth Port queue %s", intfNameBuf);
              present = TRUE;
           }
         }
       }

       cmsObj_free((void **)&cObj);
   }

   return present;
}


UBOOL8 qdmQos_isLanSwitchPortToLanSwitchPortClassificationPresentLocked_dev2(UINT32 excludeClassKey)
{
   Dev2QosClassificationObject *cObj = NULL;
   InstanceIdStack           iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 present=FALSE;

   cmsLog_debug("Entered: excludeClassKey=0x%x", excludeClassKey);

   while (!present &&
          cmsObj_getNextFlags(MDMOID_DEV2_QOS_CLASSIFICATION, &iidStack,
                    OGF_NO_VALUE_UPDATE, (void **)&cObj) == CMSRET_SUCCESS)
   {
      cmsLog_debug("checking classKey=%d enable=%d status=%s classQueue=%d",
                   cObj->X_BROADCOM_COM_ClassKey,
                   cObj->enable,
                   cObj->status,
                   cObj->X_BROADCOM_COM_ClassQueue);

      if ((cObj->X_BROADCOM_COM_ClassKey != excludeClassKey) &&
          (cObj->enable == TRUE) &&
          (!cmsUtl_strcmp(cObj->status, MDMVS_ENABLED)) &&
          (cObj->X_BROADCOM_COM_ClassQueue > 0))
      {
         char ingressIntfNameBuf[CMS_IFNAME_LENGTH]={0};
         char egressIntfNameBuf[CMS_IFNAME_LENGTH]={0};
         UBOOL8 ingressUpstream=TRUE;
         UBOOL8 egressUpstream=TRUE;
         CmsRet r2=CMSRET_INVALID_ARGUMENTS;
         CmsRet r3=CMSRET_INVALID_ARGUMENTS;

         /* First look at the ingress interface */
         if (!cmsUtl_strcmp(cObj->interface, MDMVS_LAN))
         {
            strcpy(ingressIntfNameBuf, MDMVS_LAN);
            ingressUpstream = FALSE;
         }
         else
         {
            r2 = qdmIntf_fullPathToIntfnameLocked(cObj->interface,
                                                  ingressIntfNameBuf);
            if (r2 != CMSRET_SUCCESS)
            {
               cmsLog_error("Could not convert %s to intfName, r2=%d",
                            cObj->interface, r2);
            }
            else
            {
               ingressUpstream = qdmIntf_isLayer2IntfNameUpstreamLocked(ingressIntfNameBuf);
            }
         }

         /* Now look at egress queue */
         r3 = qdmQos_getQueueInfoByClassQueueLocked(cObj->X_BROADCOM_COM_ClassQueue,
                                                    NULL, NULL,
                                                    egressIntfNameBuf);

         if (r3 != CMSRET_SUCCESS)
         {
            cmsLog_error("Could not get classQueue %d, ret=%d",
                         cObj->X_BROADCOM_COM_ClassQueue, r2);
         }
         else
         {
            egressUpstream = qdmIntf_isLayer2IntfNameUpstreamLocked(egressIntfNameBuf);
         }

         if (r2 == CMSRET_SUCCESS && r3 == CMSRET_SUCCESS)
         {
            if ((ingressUpstream == FALSE) &&
                (!strcmp(ingressIntfNameBuf, MDMVS_LAN) ||
#ifdef DMP_DEVICE2_MOCA_1
                 !cmsUtl_strncmp(ingressIntfNameBuf, MOCA_IFC_STR, strlen(MOCA_IFC_STR)) ||
#endif
                 !cmsUtl_strncmp(ingressIntfNameBuf, ETH_IFC_STR, strlen(ETH_IFC_STR))
                ) &&
                (egressUpstream == FALSE) &&
                (
#ifdef DMP_DEVICE2_MOCA_1
                 !cmsUtl_strncmp(egressIntfNameBuf, MOCA_IFC_STR, strlen(MOCA_IFC_STR))||
#endif
                 !cmsUtl_strncmp(egressIntfNameBuf, ETH_IFC_STR, strlen(ETH_IFC_STR))
                ))
            {
               cmsLog_debug("detected LAN to LAN class rule: ingr=%s egr=%s",
                            ingressIntfNameBuf, egressIntfNameBuf);
               present = TRUE;
            }
         }
      }

      cmsObj_free((void **)&cObj);
   }

   return present;
}


#endif /* DMP_DEVICE2_QOS_1  */

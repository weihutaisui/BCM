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

#ifdef DMP_QOS_1


#include "cms.h"
#include "cms_mdm.h"
#include "cms_qos.h"
#include "cms_obj.h"
#include "cms_qdm.h"
#include "cms_util.h"


UBOOL8 qdmQos_isQosEnabled_igd(void)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   QMgmtObject *qMgmtObj = NULL;
   UBOOL8 enabled = FALSE;
   CmsRet ret;

   if ((ret = cmsObj_get(MDMOID_Q_MGMT, &iidStack, 0, (void **)&qMgmtObj)) == CMSRET_SUCCESS)
   {
      enabled = qMgmtObj->enable;
      cmsObj_free((void **)&qMgmtObj);
   }
   else
   {
      cmsLog_error("cmsObj_get <MDMOID_Q_MGMT> returns error. ret=%d", ret);
   }

   return enabled;
}


/** Internal helper function.  Caller must free qObj
 *
 */
static CmsRet getQueueObjByClassQueueLocked(SINT32 classQueue,
                                            InstanceIdStack *iidStack,
                                            QMgmtQueueObject **qObj)
{
   CmsRet ret;

   /*
    * Since the classQueue is actually the instance number of the Queue obj,
    * we can manually fill in the iidStack and get the object without
    * searching for it.
    */
   INIT_INSTANCE_ID_STACK(iidStack);
   iidStack->instance[0] = classQueue;
   iidStack->currentDepth = 1;
   if ((ret = cmsObj_get(MDMOID_Q_MGMT_QUEUE, iidStack, 0, (void **) qObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_get <MDMOID_Q_MGMT_QUEUE> returns error. ret=%d", ret);
   }

   return ret;
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

CmsRet qdmQos_getQueueInfoByClassQueueLocked_igd(SINT32 classQueue,
                                                 UBOOL8 *enabled,
                                                 UINT32 *qidMark,
                                                 char *l2IntfName)
{
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   QMgmtQueueObject *qObj=NULL;
   CmsRet ret;

   if (classQueue <= 0)
   {
      cmsLog_error("invalid classQueue=%d", classQueue);
      return CMSRET_INVALID_ARGUMENTS;
   }


   if ((ret = getQueueObjByClassQueueLocked(classQueue, &iidStack, &qObj)) == CMSRET_SUCCESS)
   {
      if (enabled != NULL)
      {
         *enabled = qObj->queueEnable;
      }

      if (l2IntfName != NULL)
      {
         ret = qdmIntf_fullPathToIntfnameLocked(qObj->queueInterface, l2IntfName);
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("could not convert %s to layer 2 IntfName",
                         qObj->queueInterface);
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


void qdmQos_convertDmQueueObjToCmsQueueInfoLocked_igd(const void *obj,
                                                 CmsQosQueueInfo *qInfo)
{
   QMgmtQueueObject *qObj = (QMgmtQueueObject *) obj;
   MdmObjectId oid = GET_MDM_OBJECT_ID(qObj);
   CmsRet ret;

   memset(qInfo, 0, sizeof(CmsQosQueueInfo));

   if (oid != MDMOID_Q_MGMT_QUEUE)
   {
      cmsLog_error("Wrong object: got %d expected %d",
                   oid, MDMOID_Q_MGMT_QUEUE);
      return;
   }

   qInfo->enable = qObj->queueEnable;

   if (cmsUtl_strlen(qObj->X_BROADCOM_COM_QueueName) > (int) sizeof(qInfo->queueName) - 1)
   {
      cmsLog_error("QueueName %s is longer than buf, will be truncated",
                   qObj->X_BROADCOM_COM_QueueName);
   }
   cmsUtl_strncpy(qInfo->queueName, qObj->X_BROADCOM_COM_QueueName,
                  sizeof(qInfo->queueName));

   /* convert MDM fullpath to linux intf name */
   if (!IS_EMPTY_STRING(qObj->queueInterface))
   {
      ret = qdmIntf_fullPathToIntfnameLocked(qObj->queueInterface, qInfo->intfName);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not get intfName for %s, ret=%d",
                      qObj->queueInterface, ret);
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

   qInfo->queuePrecedence = qObj->queuePrecedence;
   qInfo->queueWeight = qObj->queueWeight;

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


void qdmQos_convertDmClassObjToCmsClassInfoLocked_igd(const void *obj,
                                                 CmsQosClassInfo *cInfo)
{
   QMgmtClassificationObject *cObj = (QMgmtClassificationObject *) obj;
   MdmObjectId oid = GET_MDM_OBJECT_ID(cObj);

   memset(cInfo, 0, sizeof(CmsQosClassInfo));

   if (oid != MDMOID_Q_MGMT_CLASSIFICATION)
   {
      cmsLog_error("Wrong object: got %d expected %d",
                   oid, MDMOID_Q_MGMT_CLASSIFICATION);
      return;
   }

   cInfo->enable = cObj->classificationEnable;

   if (cmsUtl_strlen(cObj->X_BROADCOM_COM_ClassName) > (int) sizeof(cInfo->name) - 1)
   {
      cmsLog_error("ClassName %s is longer than buf, will be truncated",
                   cObj->X_BROADCOM_COM_ClassName);
   }
   cmsUtl_strncpy(cInfo->name, cObj->X_BROADCOM_COM_ClassName,
                  sizeof(cInfo->name));

   cInfo->order = cObj->classificationOrder;
   cInfo->key = cObj->X_BROADCOM_COM_ClassKey;
   cInfo->etherType = cObj->ethertype;
   cInfo->etherTypeExclude = cObj->ethertypeExclude;

   cmsUtl_strncpy(cInfo->ingressIntfFullPath, cObj->classInterface,
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
   cInfo->egressQueueInstance = cObj->classQueue;

   cInfo->policerInstance = cObj->classPolicer;
   cInfo->classRate = cObj->X_BROADCOM_COM_ClassRate;

   return;
}


extern CmsRet rutQos_referenceCheck(MdmObjectId oid, SINT32 instance, UBOOL8 *isRefered);

CmsRet qdmQos_referenceCheckLocked_igd(CmsQosClassRefTargetEnum targ, SINT32 instance, UBOOL8 *isRefered)
{
   MdmObjectId oid;

   if (targ == CMS_QOS_REF_TARGET_POLICER)
   {
      oid = MDMOID_Q_MGMT_POLICER;
   }
   else if (targ == CMS_QOS_REF_TARGET_QUEUE)
   {
      oid = MDMOID_Q_MGMT_QUEUE;
   }
   else
   {
      cmsLog_error("Unsupported target selector %d", targ);
      return CMSRET_INVALID_ARGUMENTS;
   }

   return rutQos_referenceCheck(oid, instance, isRefered);
}


CmsRet qdmQos_getClassPolicerInfoLocked_igd(SINT32 instance,
                                             CmsQosPolicerInfo *pInfo)
{
   QMgmtPolicerObject *pObj=NULL;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   memset((void *) pInfo, 0, sizeof(CmsQosPolicerInfo));

   if (instance <= 0)
   {
      cmsLog_error("invalid instance=%d", instance);
      return CMSRET_INVALID_ARGUMENTS;
   }


   /* the fullpath of the Policer table is InternetGatewayDevice.QueueManagement.Policer.{i}.
    * so we just need to push the instance number into the first position
    * of the instance id stack.
    */
   PUSH_INSTANCE_ID(&iidStack, instance);
   if ((ret = cmsObj_get(MDMOID_Q_MGMT_POLICER, &iidStack, 0, (void **) &pObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_get <MDMOID_Q_MGMT_POLICER> returns error. ret=%d", ret);
      return ret;
   }

   /* copy info from pObj to pInfo struct */
   pInfo->enable = pObj->policerEnable;
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


UBOOL8 qdmQos_isEgressEthPortClassificationPresentLocked_igd(UINT32 excludeClassKey)
{

   QMgmtClassificationObject *cObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 present = FALSE;

   cmsLog_debug("Entered: excludeClassKey=0x%x", excludeClassKey);

   while (!present &&
          cmsObj_getNextFlags(MDMOID_Q_MGMT_CLASSIFICATION, &iidStack,
                     OGF_NO_VALUE_UPDATE, (void **)&cObj) == CMSRET_SUCCESS)
   {
      if ((cObj->X_BROADCOM_COM_ClassKey != excludeClassKey) &&
          (cObj->classificationEnable == TRUE) &&
          (cObj->classQueue > 0))
      {
         QMgmtQueueObject   *qObj = NULL;
         InstanceIdStack iidStack2=EMPTY_INSTANCE_ID_STACK;
         CmsRet r2;

         r2 = getQueueObjByClassQueueLocked(cObj->classQueue, &iidStack2, &qObj);
         if (r2 != CMSRET_SUCCESS)
         {
            cmsLog_error("Could not get classQueue %d, ret=%d",
                         cObj->classQueue, r2);
         }
         else
         {
            /* what about Moca? */
            /* "EthernetInterfaceConfig" will match both LAN and WAN Eth */
           if (strstr(qObj->queueInterface, "EthernetInterfaceConfig")
#ifdef DMP_X_BROADCOM_COM_PONWAN_1
               || strstr(qObj->queueInterface, "PonInterfaceConfig")
#endif
               )
           {
              cmsLog_debug("Detected Egress Eth Port queue %s", qObj->queueInterface);
              present = TRUE;
           }

           cmsObj_free((void **)&qObj);
         }
       }
       cmsObj_free((void **)&cObj);
   }

   return present;
}


UBOOL8 qdmQos_isLanSwitchPortToLanSwitchPortClassificationPresentLocked_igd(UINT32 excludeClassKey)
{
   QMgmtClassificationObject *cObj = NULL;
   InstanceIdStack           iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 present=FALSE;

   cmsLog_debug("Entered: excludeClassKey=0x%x", excludeClassKey);

   while (!present &&
          cmsObj_getNextFlags(MDMOID_Q_MGMT_CLASSIFICATION, &iidStack,
                    OGF_NO_VALUE_UPDATE, (void **)&cObj) == CMSRET_SUCCESS)
   {
      if ((cObj->X_BROADCOM_COM_ClassKey != excludeClassKey) &&
          (cObj->classificationEnable == TRUE ) &&
          (cObj->classQueue > 0))
      {
         QMgmtQueueObject *qObj = NULL;
         InstanceIdStack iidStack2=EMPTY_INSTANCE_ID_STACK;
         CmsRet r2;

         r2 = getQueueObjByClassQueueLocked(cObj->classQueue, &iidStack2, &qObj);
         if (r2 != CMSRET_SUCCESS)
         {
            cmsLog_error("Could not get classQueue %d, ret=%d",
                         cObj->classQueue, r2);
         }
         else
         {
            if ((!strcmp(cObj->classInterface, MDMVS_LAN) ||
#ifdef DMP_X_BROADCOM_COM_MOCALAN_1
                 strstr(cObj->classInterface, "LANMocaInterfaceConfig") ||
#endif
                 strstr(cObj->classInterface, "LANEthernetInterfaceConfig")
                ) &&
                (
#ifdef DMP_X_BROADCOM_COM_MOCALAN_1
                 strstr(qObj->queueInterface, "LANMocaInterfaceConfig") ||
#endif
                 strstr(qObj->queueInterface, "LANEthernetInterfaceConfig")
                ))
            {
               cmsLog_debug("detected LAN to LAN class rule: ingr=%s egr=%s",
                            cObj->classInterface, qObj->queueInterface);
               present = TRUE;
            }

            cmsObj_free((void **)&qObj);
         }
      }

      cmsObj_free((void **)&cObj);
   }

   return present;
}



#endif /* DMP_QOS_1  */

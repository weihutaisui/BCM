/***********************************************************************
 *
 *  Copyright (c) 2009  Broadcom Corporation
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

#ifdef SUPPORT_QOS

/* this file contains mostly TR98 specific QoS functions, but also does
 * contain some functions used by both TR98 and TR181 QoS code.
 */


#include "cms_core.h"
#include "cms_dal.h"
#include "cms_qos.h"
#include "cms_qdm.h"
#include "cms_util.h"
#include "devctl_xtm.h"

#ifdef DMP_QOS_1

CmsRet dalQos_configQosMgmt_igd(UBOOL8 enable, SINT32 dscp, UINT32 defaultQueue)
{
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   QMgmtObject *qMgmtObj = NULL;
   CmsRet ret;

   cmsLog_debug("Enter: enable=%d dscp=%d defQ=%d", enable, dscp, defaultQueue);

   /* get the current queue management config */
   if ((ret = cmsObj_get(MDMOID_Q_MGMT, &iidStack, 0, (void **)&qMgmtObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_get <MDMOID_Q_MGMT> returns error. ret=%d", ret);
      return ret;
   }

   /* don't do anything if configuration does not change */
   if (qMgmtObj->enable == enable &&
       qMgmtObj->defaultDSCPMark == dscp &&
       qMgmtObj->defaultQueue == defaultQueue)
   {
      cmsLog_debug("There is no change in queue management configuration");
      cmsObj_free((void **)&qMgmtObj);
      return CMSRET_SUCCESS;
   }

   /* overwrite with user's configuration */
   qMgmtObj->enable = enable;
   qMgmtObj->defaultDSCPMark = dscp;
   qMgmtObj->defaultQueue = defaultQueue;

   if ((ret = cmsObj_set((void *)qMgmtObj, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("set of Q_MGMT object failed, ret=%d", ret);
   }

   cmsObj_free((void **)&qMgmtObj);

   return ret;
}


CmsRet dalQos_duplicateClassCheck_igd(const void *mdmObj, UBOOL8 *isDuplicate)
{
   InstanceIdStack iidStack;
   const QMgmtClassificationObject *clsObj = (const QMgmtClassificationObject *) mdmObj;
   QMgmtClassificationObject *cObj = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   *isDuplicate = FALSE;

   /* first see if the classification already existed */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_Q_MGMT_CLASSIFICATION, &iidStack, (void **)&cObj)) == CMSRET_SUCCESS)
   {
      /* two classifications are same, if all their criteria are identical. */
      if (cObj->ethertype == clsObj->ethertype &&
          cmsUtl_strcmp(cObj->classInterface, clsObj->classInterface) == 0 &&
          cmsUtl_strcmp(cObj->destIP, clsObj->destIP) == 0 &&
          cmsUtl_strcmp(cObj->destMask, clsObj->destMask) == 0 &&
          cObj->destIPExclude == clsObj->destIPExclude &&
          cmsUtl_strcmp(cObj->sourceIP, clsObj->sourceIP) == 0 &&
          cmsUtl_strcmp(cObj->sourceMask, clsObj->sourceMask) == 0 &&
          cObj->sourceIPExclude == clsObj->sourceIPExclude &&
          cObj->protocol == clsObj->protocol &&
          cObj->protocolExclude == clsObj->protocolExclude &&
          cObj->destPort == clsObj->destPort &&
          cObj->destPortRangeMax == clsObj->destPortRangeMax &&
          cObj->destPortExclude == clsObj->destPortExclude &&
          cObj->sourcePort == clsObj->sourcePort &&
          cObj->sourcePortRangeMax == clsObj->sourcePortRangeMax &&
          cObj->sourcePortExclude == clsObj->sourcePortExclude &&
          cmsUtl_strcmp(cObj->destMACAddress, clsObj->destMACAddress) == 0 &&
          cmsUtl_strcmp(cObj->destMACMask, clsObj->destMACMask) == 0 &&
          cObj->destMACExclude == clsObj->destMACExclude &&
          cmsUtl_strcmp(cObj->sourceMACAddress, clsObj->sourceMACAddress) == 0 &&
          cmsUtl_strcmp(cObj->sourceMACMask, clsObj->sourceMACMask) == 0 &&
          cObj->sourceMACExclude == clsObj->sourceMACExclude &&
          cmsUtl_strcmp(cObj->sourceVendorClassID, clsObj->sourceVendorClassID) == 0 &&
          cObj->sourceVendorClassIDExclude == clsObj->sourceVendorClassIDExclude &&
          cmsUtl_strcmp(cObj->sourceUserClassID, clsObj->sourceUserClassID) == 0 &&
          cObj->sourceUserClassIDExclude == clsObj->sourceUserClassIDExclude &&
          cObj->DSCPCheck == clsObj->DSCPCheck &&
          cObj->DSCPExclude == clsObj->DSCPExclude &&
          cObj->ethernetPriorityCheck == clsObj->ethernetPriorityCheck &&
          cObj->ethernetPriorityExclude == clsObj->ethernetPriorityExclude)
      {
         if (cmsUtl_strcmp(cObj->classInterface, MDMVS_LOCAL) == 0)
         {
            if (cmsUtl_strcmp(cObj->X_BROADCOM_COM_egressInterface, clsObj->X_BROADCOM_COM_egressInterface) == 0)
            {
               *isDuplicate = TRUE;
               cmsObj_free((void **)&cObj);
               break; 
            }
         }
         else
         {
            *isDuplicate = TRUE;
            cmsObj_free((void **)&cObj);
            break;
         }
      }
      cmsObj_free((void **)&cObj);
   }
   if (ret != CMSRET_NO_MORE_INSTANCES && ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_getNext <MDMOID_Q_MGMT_CLASSIFICATION> returns error. ret=%d", ret);
      return ret;
   }

   return CMSRET_SUCCESS;

}  /* End of dalQos_duplicateClassCheck() */


CmsRet dalQos_queueAdd_igd(const char *intfName, const char *schedulerAlg,
                  UBOOL8 enable, const char *queueName, UINT32 queueId,
                  UINT32 weight, UINT32 precedence,
                  SINT32 minRate, SINT32 shapingRate, UINT32 shapingBurstSize,
                  SINT32 dslLatency, SINT32 ptmPriority, const char *dropAlg,
                  UINT32 loMinThreshold, UINT32 loMaxThreshold,
                  UINT32 hiMinThreshold, UINT32 hiMaxThreshold)
{
   InstanceIdStack iidStack;
   QMgmtQueueObject *qObj = NULL;
   char *intfFullPath=NULL;
   CmsRet ret;

   if (intfName == NULL || schedulerAlg == NULL)
   {
      cmsLog_error("Invalid input argument");
      return CMSRET_INVALID_ARGUMENTS;
   }

   /* convert user friendly interface name to mdm full path */
   if ((ret = qdmIntf_intfnameToFullPathLocked(intfName, TRUE, &intfFullPath)) == CMSRET_SUCCESS)
   {
      /* strip the ending '.' */
      intfFullPath[strlen(intfFullPath)-1] = '\0';
   }
   else
   {
      cmsLog_error("Invalid queue interface");
      return CMSRET_INVALID_ARGUMENTS;
   }


   /* add a new queue object instance */
   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_addInstance(MDMOID_Q_MGMT_QUEUE, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_addInstance returns error, ret=%d", ret);
      CMSMEM_FREE_BUF_AND_NULL_PTR(intfFullPath);
      return ret;
   }

   /* get the object, it will be initially filled in with default values */
   if ((ret = cmsObj_get(MDMOID_Q_MGMT_QUEUE, &iidStack, 0, (void **)&qObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_get returns error. ret=%d", ret);
      CMSMEM_FREE_BUF_AND_NULL_PTR(intfFullPath);
      cmsObj_deleteInstance(MDMOID_Q_MGMT_QUEUE, &iidStack);
      return ret;
   }


   qObj->queueEnable = enable;
   CMSMEM_REPLACE_STRING(qObj->queueInterface, intfFullPath);
   CMSMEM_REPLACE_STRING(qObj->schedulerAlgorithm, schedulerAlg);

   qObj->queuePrecedence  = precedence;
   qObj->queueWeight      = weight;

   CMSMEM_REPLACE_STRING(qObj->dropAlgorithm, dropAlg);
   qObj->REDThreshold                         = loMinThreshold;
   qObj->X_BROADCOM_COM_LowClassMaxThreshold  = loMaxThreshold;
   qObj->X_BROADCOM_COM_HighClassMinThreshold = hiMinThreshold;
   qObj->X_BROADCOM_COM_HighClassMaxThreshold = hiMaxThreshold;

   qObj->X_BROADCOM_COM_MinBitRate = minRate;
   qObj->shapingRate      = shapingRate;
   qObj->shapingBurstSize = shapingBurstSize;
   
   qObj->X_BROADCOM_COM_DslLatency = dslLatency;
   qObj->X_BROADCOM_COM_PtmPriority = ptmPriority;
   qObj->X_BROADCOM_COM_QueueId = queueId;
   CMSMEM_REPLACE_STRING(qObj->X_BROADCOM_COM_QueueName, queueName);


   /* set the Queue Object instance */
   if ((ret = cmsObj_set(qObj, &iidStack)) != CMSRET_SUCCESS)
   {
      CmsRet r2;
      cmsLog_error("cmsObj_set returns error, ret = %d", ret);
       
      /* since set failed, we have to delete the instance that we just added */       
      if ((r2 = cmsObj_deleteInstance(MDMOID_Q_MGMT_QUEUE, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsObj_deleteInstance returns error, r2=%d", r2);
      }
   }

#ifdef DMP_X_BROADCOM_COM_RDPA_1
   if ((ret = dalQos_addAccordingQueueStatsobject(qObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("dalQos_addAccordingQueueStatsobject returns error, ret = %d", ret);
   }
#endif   
   cmsObj_free((void **)&qObj);
   CMSMEM_FREE_BUF_AND_NULL_PTR(intfFullPath);

   return ret;
}


CmsRet dalQos_duplicateQueueCheck_igd(UINT32 queueId, SINT32 dslLatency,
                              SINT32 ptmPriority, const char *intfName,
                              UBOOL8 *isDuplicate)
{
   InstanceIdStack iidStack;
   QMgmtQueueObject *qObj = NULL;
   char *intfFullPath=NULL;
   CmsRet ret = CMSRET_SUCCESS;

   *isDuplicate = FALSE;

   /* convert user friendly interface name to mdm full path */
   if ((ret = qdmIntf_intfnameToFullPathLocked(intfName, TRUE, &intfFullPath)) == CMSRET_SUCCESS)
   {
      /* strip the ending '.' */
      intfFullPath[strlen(intfFullPath)-1] = '\0';
   }
   else
   {
      cmsLog_error("Invalid queue interface %s", intfName);
      return CMSRET_INVALID_ARGUMENTS;
   }

   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((*isDuplicate) == FALSE &&
          (ret = cmsObj_getNext(MDMOID_Q_MGMT_QUEUE, &iidStack, (void **)&qObj)) == CMSRET_SUCCESS)
   {
      if (qObj->X_BROADCOM_COM_QueueId     == queueId &&
          qObj->X_BROADCOM_COM_DslLatency  == dslLatency &&
          qObj->X_BROADCOM_COM_PtmPriority == ptmPriority &&
          cmsUtl_strcmp(qObj->queueInterface, intfFullPath) == 0)
      {
         *isDuplicate = TRUE;
      }

      cmsObj_free((void **)&qObj);
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(intfFullPath);

   if (ret == CMSRET_NO_MORE_INSTANCES)
   {
      ret = CMSRET_SUCCESS;
   }

   return ret;
}  /* End of dalQos_duplicateQueueCheck() */


CmsRet dalQos_policerAdd_igd(const CmsQosPolicerInfo *pInfo)
{
   QMgmtPolicerObject *pObj=NULL;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   /* add a new qos policer entry */
   cmsLog_debug("Adding new qos policer:");
   cmsLog_debug("Enable=%d", pInfo->enable);
   cmsLog_debug("Name=%s", pInfo->name);
   cmsLog_debug("CommittedRate=%d", pInfo->committedRate);
   cmsLog_debug("CommittedBurstSize=%d", pInfo->committedBurstSize);
   cmsLog_debug("ExcessBurstSize=%d", pInfo->excessBurstSize);
   cmsLog_debug("PeakRate=%d", pInfo->peakRate);
   cmsLog_debug("PeakBurstSize=%d", pInfo->peakBurstSize);
   cmsLog_debug("MeterType=%s", pInfo->meterType);
   cmsLog_debug("ConformingAction=%s", pInfo->conformingAction);
   cmsLog_debug("PartialConformingAction=%s", pInfo->partialConformingAction);
   cmsLog_debug("NonConformingAction=%s", pInfo->nonConformingAction);

   /* add a new policer object instance */
   if ((ret = cmsObj_addInstance(MDMOID_Q_MGMT_POLICER, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_addInstance returns error, ret=%d", ret);
      return ret;
   }

   ret = cmsObj_get(MDMOID_Q_MGMT_POLICER, &iidStack, 0, (void **)&pObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get obj after create, ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_Q_MGMT_POLICER, &iidStack);
      return ret;
   }

   /* transfer the settings from pInfo to the object */
   pObj->policerEnable = pInfo->enable;
   pObj->committedRate = pInfo->committedRate;
   pObj->committedBurstSize = pInfo->committedBurstSize;
   pObj->excessBurstSize = pInfo->excessBurstSize;
   pObj->peakRate = pInfo->peakRate;
   pObj->peakBurstSize = pInfo->peakBurstSize;
   CMSMEM_REPLACE_STRING(pObj->meterType, pInfo->meterType);
   CMSMEM_REPLACE_STRING(pObj->conformingAction, pInfo->conformingAction);
   CMSMEM_REPLACE_STRING(pObj->partialConformingAction, pInfo->partialConformingAction);
   CMSMEM_REPLACE_STRING(pObj->nonConformingAction, pInfo->nonConformingAction);
   CMSMEM_REPLACE_STRING(pObj->X_BROADCOM_COM_PolicerName, pInfo->name);

   /* set the policer Object instance */
   if ((ret = cmsObj_set(pObj, &iidStack)) != CMSRET_SUCCESS)
   {
      CmsRet r2;
      cmsLog_error("cmsObj_set returns error, ret = %d", ret);
       
      /* since set failed, we have to delete the instance that we just added */       
      if ((r2 = cmsObj_deleteInstance(MDMOID_Q_MGMT_POLICER, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsObj_deleteInstance returns error, r2=%d", r2);
      }
   }

   cmsObj_free((void **) &pObj);

   return ret;

}  /* End of dalQos_policerAdd() */

CmsRet dalQos_duplicatePolicerCheck_igd(const CmsQosPolicerInfo *policerObj, UBOOL8 *isDuplicate)
{
   InstanceIdStack iidStack;
   QMgmtPolicerObject *pObj = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   *isDuplicate = FALSE;

   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_Q_MGMT_POLICER, &iidStack, (void **)&pObj)) == CMSRET_SUCCESS)
   {
      if (pObj->committedRate      == policerObj->committedRate &&
          pObj->committedBurstSize == policerObj->committedBurstSize &&
          pObj->excessBurstSize    == policerObj->excessBurstSize &&
          pObj->peakRate           == policerObj->peakRate &&
          pObj->peakBurstSize      == policerObj->peakBurstSize &&
          cmsUtl_strcmp(pObj->meterType, policerObj->meterType) == 0 &&
          cmsUtl_strcmp(pObj->conformingAction, policerObj->conformingAction) == 0 &&
          cmsUtl_strcmp(pObj->partialConformingAction, policerObj->partialConformingAction) == 0 &&
          cmsUtl_strcmp(pObj->nonConformingAction, policerObj->nonConformingAction) == 0)
      {
         *isDuplicate = TRUE;
         cmsObj_free((void **)&pObj);
         break;
      }
      cmsObj_free((void **)&pObj);
   }
   if (ret != CMSRET_NO_MORE_INSTANCES && ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_getNext returns error. ret=%d", ret);
      return ret;
   }
   return CMSRET_SUCCESS;

}  /* End of dalQos_duplicatePolicerCheck() */

CmsRet dalQos_addAccordingQueueStatsobject(const _QMgmtQueueObject *qObj)
{
   InstanceIdStack stats_iidStack;
   _QMgmtQueueStatsObject *qStatsObj = NULL;
   CmsRet ret;

   /* add a new queue stats object instance */
   INIT_INSTANCE_ID_STACK(&stats_iidStack);
   if ((ret = cmsObj_addInstance(MDMOID_Q_MGMT_QUEUE_STATS, &stats_iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_addInstance returns error, ret=%d", ret);
      return ret;
   }

   /* get the object, it will be initially filled in with default values */
   if ((ret = cmsObj_get(MDMOID_Q_MGMT_QUEUE_STATS, &stats_iidStack, OGF_NO_VALUE_UPDATE, (void **)&qStatsObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_get returns error. ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_Q_MGMT_QUEUE_STATS, &stats_iidStack);
      return ret;
   }

   qStatsObj->X_BROADCOM_COM_QueueId = qObj->X_BROADCOM_COM_QueueId;
   CMSMEM_REPLACE_STRING_FLAGS(qStatsObj->queueInterface, qObj->queueInterface, ALLOC_ZEROIZE);

   /* set the Queue Stats Object instance */
   if ((ret = cmsObj_set(qStatsObj, &stats_iidStack)) != CMSRET_SUCCESS)
   {
      CmsRet r2;
      cmsLog_error("cmsObj_set returns error, ret = %d", ret);

      /* since set failed, we have to delete the instance that we just added */       
      if ((r2 = cmsObj_deleteInstance(MDMOID_Q_MGMT_QUEUE_STATS, &stats_iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsObj_deleteInstance returns error, r2=%d", r2);
      }

      cmsObj_free((void **)&qStatsObj);
      return ret;
   }

   cmsObj_free((void **)&qStatsObj);
   return CMSRET_SUCCESS;
}

CmsRet dalQos_delAccordingQueueStatsobject(const _QMgmtQueueObject *qObj)
{
   InstanceIdStack iidStack;
   _QMgmtQueueStatsObject *qStatsObj = NULL;
   CmsRet ret;
   UBOOL8 found = FALSE;

   INIT_INSTANCE_ID_STACK(&iidStack);
   while (!found &&
    (ret = cmsObj_getNextFlags(MDMOID_Q_MGMT_QUEUE_STATS, &iidStack,
    OGF_NO_VALUE_UPDATE, (void **)&qStatsObj)) == CMSRET_SUCCESS)
   {
      if (strcmp(qObj->queueInterface, qStatsObj->queueInterface) == 0 &&
        qObj->X_BROADCOM_COM_QueueId == qStatsObj->X_BROADCOM_COM_QueueId)
      {
         found = TRUE;
         if ((ret = cmsObj_deleteInstance(MDMOID_Q_MGMT_QUEUE_STATS, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("deleteInstance returns error, ret = %d", ret);
         }
         cmsObj_free((void **)&qStatsObj);
         break;
      }
      cmsObj_free((void **)&qStatsObj);
   }
   return ret;
}

#endif  /* DMP_QOS_1 */


/* use these functions declared in rut_qos.h */
extern CmsRet rutQos_convertPrecedenceToPriority(const char *qIntfPath, UINT32 prec, UINT32 *prio);
extern CmsRet rutQos_getAvailableQueueId(const char *l2Ifname, UINT32 prio, const char *alg, UINT32 *qId);
extern void   rutQos_getIntfNumQueuesAndLevels(const char *l2IfName, UINT32 *numQueues, UINT32 *numLevels);
extern CmsRet rutQos_getAvailableClsKey(UINT32 *clsKey);


CmsRet dalQos_convertPrecedenceToPriority(const char *qIntfPath, UINT32 prec, UINT32 *prio)
{
   return rutQos_convertPrecedenceToPriority(qIntfPath, prec, prio);
}

CmsRet dalQos_getAvailableQueueId(const char *l2Ifname,
                                  UINT32 prio, const char *alg, UINT32 *qId)
{
   return rutQos_getAvailableQueueId(l2Ifname, prio, alg, qId);
}

void dalQos_getIntfNumQueuesAndLevels(const char *l2IfName,
                                      UINT32 *numQueues, UINT32 *numLevels)
{
   rutQos_getIntfNumQueuesAndLevels(l2IfName, numQueues, numLevels);
}

CmsRet dalQos_getAvailableClsKey(UINT32 *clsKey)
{
   return rutQos_getAvailableClsKey(clsKey);
}



#endif  /* SUPPORT_QOS */


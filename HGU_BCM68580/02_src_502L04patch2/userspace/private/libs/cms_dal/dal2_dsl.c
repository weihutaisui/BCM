/***********************************************************************
 *
 *  Copyright (c) 2008  Broadcom Corporation
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

#ifdef DMP_DEVICE2_BASELINE_1

#ifdef DMP_DEVICE2_DSL_1


#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"
#include "cms_qos.h"
#include "cms_qdm.h"
#include "dal.h"
#include "dal_network.h"
#include "adslctlapi.h" /* for ANNEX_A_LINE_PAIR_INNER/OUTER */
#include "devctl_xtm.h"

static CmsRet fillAtmLinkQosObject(const WEB_NTWK_VAR *webVar, Dev2AtmLinkQosObject *atmLinkQos)
{
   CmsRet ret = CMSRET_SUCCESS;

   if (webVar->atmServiceCategory[0] != '\0')
   {
      CMSMEM_REPLACE_STRING(atmLinkQos->qoSClass, webVar->atmServiceCategory);
   }

   atmLinkQos->peakCellRate = webVar->atmPeakCellRate;
   atmLinkQos->sustainableCellRate = webVar->atmSustainedCellRate;
   atmLinkQos->maximumBurstSize = webVar->atmMaxBurstSize;
   atmLinkQos->X_BROADCOM_COM_MinimumCellRate = webVar->atmMinCellRate;

   return ret;
}


static CmsRet fillAtmLinkObject(const WEB_NTWK_VAR *webVar, char *destAddr, Dev2AtmLinkObject *atmLink)
{
   CmsRet ret = CMSRET_SUCCESS;

   /* set portid */
   atmLink->X_BROADCOM_COM_ATMInterfaceId = webVar->portId;
   
   /* set connection mode value */
   if (webVar->connMode == CMS_CONNECTION_MODE_VLANMUX)
   {
      CMSMEM_REPLACE_STRING(atmLink->X_BROADCOM_COM_ConnectionMode, MDMVS_VLANMUXMODE);
   }
      
   /* set Qos enable */
   atmLink->X_BROADCOM_COM_ATMEnbQos = webVar->enblQos;

   /* replace connection scheduler algorithm with new value */
   CMSMEM_REPLACE_STRING(atmLink->X_BROADCOM_COM_GrpScheduler, webVar->grpScheduler);
   
   /* set connection MPAAL weight value */
   atmLink->X_BROADCOM_COM_GrpWeight = webVar->grpWeight;

   /* set connection MPAAL priority */
   atmLink->X_BROADCOM_COM_GrpPrecedence = webVar->grpPrecedence;
   
   /* replace default queue scheduler algorithm with new value */
   CMSMEM_REPLACE_STRING(atmLink->X_BROADCOM_COM_SchedulerAlgorithm, webVar->schedulerAlgorithm);
   
   /* set default queue weight value */
   atmLink->X_BROADCOM_COM_QueueWeight = webVar->queueWeight;

   /* set default queue priority */
   atmLink->X_BROADCOM_COM_QueuePrecedence = webVar->queuePrecedence;
   
   CMSMEM_REPLACE_STRING(atmLink->X_BROADCOM_COM_DropAlgorithm, webVar->dropAlgorithm);
   atmLink->X_BROADCOM_COM_LowClassMinThreshold  = webVar->loMinThreshold;
   atmLink->X_BROADCOM_COM_LowClassMaxThreshold  = webVar->loMaxThreshold;
   atmLink->X_BROADCOM_COM_HighClassMinThreshold = webVar->hiMinThreshold;
   atmLink->X_BROADCOM_COM_HighClassMaxThreshold = webVar->hiMaxThreshold;

   /* replace destinationAddress with new value */
   CMSMEM_REPLACE_STRING(atmLink->destinationAddress, destAddr);

   /* replace ATMEncapsulation with new value */
   cmsMem_free(atmLink->encapsulation);

   if (cmsUtl_strcmp(webVar->linkType, MDMVS_PPPOA) != 0)
   {
      if (webVar->encapMode == 0)
      {
         atmLink->encapsulation = cmsMem_strdup(MDMVS_LLC);
      }
      else
      {
         atmLink->encapsulation = cmsMem_strdup(MDMVS_VCMUX);
      }
   }
   else
   {
      /* for PPPOA vcMux is the default ie. encapMode = 0 */
      if (webVar->encapMode == 0)
      {
         atmLink->encapsulation = cmsMem_strdup(MDMVS_VCMUX);
      }
      else
      {
         atmLink->encapsulation = cmsMem_strdup(MDMVS_LLC);
      }
   }

   /* assigne ifNameType, for pppoe, bridge and mer, it's MDMVS_EOA */
   if (webVar->linkType[0] != '\0')
   {
      CMSMEM_REPLACE_STRING(atmLink->linkType, webVar->linkType);
   }

   /* leave atm interface disabled.  Will be enabled when it is used later on */

   return ret;
}

CmsRet dalDsl_addAtmInterface_dev2(const WEB_NTWK_VAR *webVar)
{
   char destAddr[BUFLEN_128];   
   Dev2AtmLinkObject *atmLink = NULL;
   Dev2AtmLinkQosObject *atmLinkQos = NULL;
   InstanceIdStack atmLinkIid = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;
   
   cmsLog_debug("Adding port=%d vpi=%d vci=%d", webVar->portId, webVar->atmVpi, webVar->atmVci);

   if ((ret = cmsUtl_atmVpiVciNumToStr_dev2(webVar->atmVpi, webVar->atmVci, destAddr)) != CMSRET_SUCCESS)
   {
      return ret;
   }

   /* add a new Dev2AtmLinkObject on a new PORT/VPI/VCI */
   cmsLog_debug("Adding new Dev2AtmLinkObject at %d/%d/%d", webVar->portId, webVar->atmVpi, webVar->atmVci);
   
   /* add new instance of Dev2AtmLinkObject */
   if ((ret = cmsObj_addInstance(MDMOID_DEV2_ATM_LINK, &atmLinkIid)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create new Dev2AtmLinkObject, ret=%d", ret);
      return ret;
   }
   
   /* get the instance of Dev2AtmLinkObject */
   if ((ret = cmsObj_get(MDMOID_DEV2_ATM_LINK, &atmLinkIid, 0, (void **) &atmLink)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get Dev2AtmLinkObject, ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_ATM_LINK, &atmLinkIid);
      return ret;
   }       

   /* get the instance of Dev2AtmLinkQosObject */
   if ((ret = cmsObj_get(MDMOID_DEV2_ATM_LINK_QOS, &atmLinkIid, 0, (void **) &atmLinkQos)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get Dev2AtmLinkQosObject, ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_ATM_LINK, &atmLinkIid);
      cmsObj_free((void **) &atmLink);
      return ret;
   }       
   /* fill Dev2AtmLinkQosObject with configured parameters */
   fillAtmLinkQosObject(webVar, atmLinkQos);
   
   /* set Dev2AtmLinkQosObject */
   if ((ret = cmsObj_set(atmLinkQos, &atmLinkIid)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set Dev2AtmLinkQosObject, ret = %d", ret);
   }

   /* free Dev2AtmLinkQosObject */
   cmsObj_free((void **) &atmLinkQos);

   /* fill Dev2AtmLinkObject with configured parameters*/
   fillAtmLinkObject(webVar, destAddr, atmLink);
   atmLink->enable = TRUE;

   /* set Dev2AtmLinkObject */
   if ((ret = cmsObj_set(atmLink, &atmLinkIid)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set Dev2AtmLinkObject, ret = %d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_ATM_LINK, &atmLinkIid);
   }

   /* free Dev2AtmLinkObject */
   cmsObj_free((void **) &atmLink);

#ifdef SUPPORT_QOS
   /*
    * In TR181, the atmLink->name is set in the RCL handler function.
    * Get it again so we can have a valid name to pass into defaultQueueAdd
    */
   if (ret == CMSRET_SUCCESS)
   {
      if ((ret = cmsObj_get(MDMOID_DEV2_ATM_LINK, &atmLinkIid, 0,
                            (void **) &atmLink)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not get ATM_LINK obj, ret=%d", ret);
      }
      else
      {
         SINT32 dslLatency = (atmLink->X_BROADCOM_COM_ATMInterfaceId == PHY0_PATH1) ? PHY0_PATH1 : PHY0_PATH0;

         if (atmLink->X_BROADCOM_COM_ATMEnbQos)
         {
            /* set up default queue for this interface */
            dalQos_queueAdd(atmLink->name,
                            atmLink->X_BROADCOM_COM_SchedulerAlgorithm,
                            TRUE, DEFAULT_QUEUE_NAME, 1,
                            atmLink->X_BROADCOM_COM_QueueWeight,
                            atmLink->X_BROADCOM_COM_QueuePrecedence,
                            QOS_QUEUE_NO_SHAPING,
                            QOS_QUEUE_NO_SHAPING,
                            0,
                            dslLatency, -1,
                            atmLink->X_BROADCOM_COM_DropAlgorithm,
                            atmLink->X_BROADCOM_COM_LowClassMinThreshold,
                            atmLink->X_BROADCOM_COM_LowClassMaxThreshold,
                            atmLink->X_BROADCOM_COM_HighClassMinThreshold,
                            atmLink->X_BROADCOM_COM_HighClassMaxThreshold);
         }

         cmsObj_free((void **) &atmLink);
      }
   }
#endif /* SUPPORT_QOS */

   cmsLog_debug("Exit, ret=%d", ret);

   return ret;
}


CmsRet dalDsl_deleteAtmInterface_dev2(WEB_NTWK_VAR *webVar)
{
   UBOOL8 found = FALSE;
   UBOOL8 atmLinkFound = FALSE;
   InstanceIdStack iidStack;
   InstanceIdStack atmIidStack;
   Dev2AtmLinkObject *atmLinkObj = NULL;
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("wanL2IfName: %s", webVar->wanL2IfName);

   INIT_INSTANCE_ID_STACK(&atmIidStack);

   while (atmLinkFound == FALSE &&
          cmsObj_getNextFlags(MDMOID_DEV2_ATM_LINK,
                              &atmIidStack,
                              OGF_NO_VALUE_UPDATE,
                         (void **)&atmLinkObj) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strcmp(atmLinkObj->name, (char *)webVar->wanL2IfName) == 0)
      {
         atmLinkFound = TRUE;

         if (atmLinkObj->enable == TRUE)
         {
            INIT_INSTANCE_ID_STACK(&iidStack);

            // find upper interface (Ethernet.Link.{i}.) associated
            // with atmLink to determine this interface can be deleted or not
            while (found == FALSE &&
                   cmsObj_getNextFlags(MDMOID_DEV2_IP_INTERFACE,
                                       &iidStack,
                                       OGF_NO_VALUE_UPDATE,
                                  (void **)&ipIntfObj) == CMSRET_SUCCESS)
            {
               char currL2IntfNameBuf[CMS_IFNAME_LENGTH]={0};

               /* Get the L2Intf */
               ret = qdmIpIntf_getLayer2IntfNameByLayer3IntfNameLocked_dev2(ipIntfObj->name, currL2IntfNameBuf);
               found = (ret == CMSRET_SUCCESS &&
                        ipIntfObj->X_BROADCOM_COM_Upstream == TRUE &&
                        cmsUtl_strcmp(currL2IntfNameBuf, atmLinkObj->name) == 0);

               cmsObj_free((void **)&ipIntfObj);
            }

            // there is no upper interface associated with atmLink
            if (found == FALSE)
            {
               ret = cmsObj_deleteInstance(MDMOID_DEV2_ATM_LINK, &atmIidStack);
               if (ret != CMSRET_SUCCESS)
               {
                  cmsLog_error("Failed to delete MDMOID_DEV2_ATM_LINK, ret=%d", ret);      
               }
            }
            else
            {
               cmsLog_debug("atmLink is still in use and cannot be removed");
               ret = CMSRET_REQUEST_DENIED;
            }
         }
         else
         {
            ret = cmsObj_deleteInstance(MDMOID_DEV2_ATM_LINK, &atmIidStack);
            if (ret != CMSRET_SUCCESS)
            {
               cmsLog_error("Failed to delete MDMOID_DEV2_ATM_LINK, ret=%d", ret);      
            }
         }
      }            

      cmsObj_free((void **) &atmLinkObj);
   }

   if (atmLinkFound == FALSE)
   {
      cmsLog_error("Cannot find ATM link with wanL2IfName == %s",
                   webVar->wanL2IfName);
      ret = CMSRET_INVALID_ARGUMENTS;
   }

   cmsLog_debug("Exit, ret=%d", ret);
   
   return ret;
}


CmsRet dalDsl_deleteAtmInterfaceWithoutIfName_dev2(WEB_NTWK_VAR * webVar)
{
   SINT32 vpi = 0, vci = 0;
   UBOOL8 atmLinkFound = FALSE;
   InstanceIdStack atmIidStack;
   Dev2AtmLinkObject *atmLinkObj = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("port %d vpi %d vci %d",
                webVar->portId, webVar->atmVpi, webVar->atmVci);

   INIT_INSTANCE_ID_STACK(&atmIidStack);

   while (atmLinkFound == FALSE &&
          cmsObj_getNextFlags(MDMOID_DEV2_ATM_LINK,
                              &atmIidStack,
                              OGF_NO_VALUE_UPDATE,
                         (void **)&atmLinkObj) == CMSRET_SUCCESS)
   {
      cmsUtl_atmVpiVciStrToNum_dev2(atmLinkObj->destinationAddress, &vpi, &vci);

      if (webVar->portId == (int)atmLinkObj->X_BROADCOM_COM_ATMInterfaceId &&
          webVar->atmVpi == vpi &&
          webVar->atmVci == vci)
      {
         atmLinkFound = TRUE;

         cmsUtl_strcpy(webVar->wanL2IfName, atmLinkObj->name);
      }

      cmsObj_free((void **)&atmLinkObj);
   }

   if (atmLinkFound == TRUE)
   {
      ret = dalDsl_deleteAtmInterface(webVar);
   }
   else
   {
      cmsLog_error("Cannot find ATM link port %d vpi %d, vci %d",
                   webVar->portId, webVar->atmVpi, webVar->atmVci);
      ret = CMSRET_INVALID_ARGUMENTS;
   }

   cmsLog_debug("Exit, ret=%d", ret);

   return ret;
}



/* if isAtm is TRUE, it means to display ATM stats; isUp indicates status of link */
/* if isAtm is FALSE, it means to display PTM stats; isUp indicates status of link */
CmsRet dalDsl_displayXtmStatsType_dev2(UBOOL8 *isAtm)
{
	InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2AtmLinkObject *atmLinkObj;
#ifdef DMP_DEVICE2_PTMLINK_1
   Dev2PtmLinkObject *ptmLinkObj;
#endif
   UBOOL8 linkUp = FALSE;
   UBOOL8 atmFound = FALSE;
   UBOOL8 ptmFound = FALSE;

   /* is this a ATM stats or PTM stats? */
   while (cmsObj_getNext(MDMOID_DEV2_ATM_LINK, &iidStack,(void **)&atmLinkObj) == CMSRET_SUCCESS)
   {
      if (!strcmp(atmLinkObj->status,MDMVS_UP))
      {
         linkUp = TRUE;
         break;
      }
      atmFound = TRUE;
      cmsObj_free((void **) &atmLinkObj);
   }
   if (linkUp)
   {
      *isAtm = TRUE;
      cmsObj_free((void **) &atmLinkObj);
      return CMSRET_SUCCESS;
   }

#ifdef DMP_DEVICE2_PTMLINK_1
   INIT_INSTANCE_ID_STACK(&iidStack);
   if  (!linkUp)
   {
      while (cmsObj_getNext(MDMOID_DEV2_PTM_LINK, &iidStack,(void **)&ptmLinkObj) == CMSRET_SUCCESS)
      {
         if (!strcmp(ptmLinkObj->status,MDMVS_UP))
         {
            linkUp = TRUE;
            break;
         }
         cmsObj_free((void **) &ptmLinkObj);
         ptmFound = TRUE;
      }  
   }
   if (linkUp)
   {
      *isAtm = FALSE;
      cmsObj_free((void **) &ptmLinkObj);
      return CMSRET_SUCCESS;
   }
#endif
   /* this means atm nor ptm are up, we will return whatever is configured;
    * if both are configured and down, we just assume it's ATM stats we want to display
    */
   if ( (atmFound && ptmFound) || (atmFound) || (!atmFound && !ptmFound))
   {
      *isAtm = TRUE;
   }
   else if (ptmFound)
   {
      *isAtm = FALSE;
   }
   return CMSRET_SUCCESS;
}

#ifdef DMP_DEVICE2_PTMLINK_1
/*
*  PTM dal functions
*/

CmsRet dalDsl_addPtmInterface_dev2(const WEB_NTWK_VAR *webVar)
{
   Dev2PtmLinkObject *ptmLink = NULL;
   InstanceIdStack ptmLinkIid = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 found=FALSE;
   
   cmsLog_debug("Adding port=%d, ptmPriorityHigh=%d, ptmPriorityNorm=%d", 
      webVar->portId, webVar->ptmPriorityHigh, webVar->ptmPriorityNorm);

   /* check if ptm interface exits first */
   while ((ret = cmsObj_getNext(MDMOID_DEV2_PTM_LINK,
                                &ptmLinkIid, (void **) &ptmLink)) == CMSRET_SUCCESS)
   {
      cmsLog_debug("ptm obj portId=%d, priorityHigh=%d",  ptmLink->X_BROADCOM_COM_PTMPortId,  
                   ptmLink->X_BROADCOM_COM_PTMPriorityHigh);
      if ((PORTID_TO_PORTMASK(webVar->portId) & PORTID_TO_PORTMASK(ptmLink->X_BROADCOM_COM_PTMPortId)) &&
          ((webVar->ptmPriorityHigh & ptmLink->X_BROADCOM_COM_PTMPriorityHigh) ||
           (webVar->ptmPriorityNorm & ptmLink->X_BROADCOM_COM_PTMPriorityLow)))
      {
         found = TRUE;
         break;
      }
      else
      {
         cmsObj_free((void **) &ptmLink);
      }
   }
   if (found)
   {
      cmsLog_error("PTM link existed!");
      cmsObj_free((void **) &ptmLink);
      return CMSRET_INTERNAL_ERROR;
   }

   INIT_INSTANCE_ID_STACK(&ptmLinkIid);
   /* add new instance of Dev2PtmLinkObject */
   if ((ret = cmsObj_addInstance(MDMOID_DEV2_PTM_LINK, &ptmLinkIid)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create new Dev2PtmLinkObject, ret=%d", ret);
      return ret;
   }
   
   
   /* get the instance of Dev2PtmLinkObject */
   if ((ret = cmsObj_get(MDMOID_DEV2_PTM_LINK, &ptmLinkIid, 0, (void **) &ptmLink)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get Dev2PtmLinkObject, ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_PTM_LINK, &ptmLinkIid);
      return ret;
   }       

   ptmLink->X_BROADCOM_COM_PTMPortId = webVar->portId;
   ptmLink->X_BROADCOM_COM_PTMPriorityHigh = webVar->ptmPriorityHigh;
   ptmLink->X_BROADCOM_COM_PTMPriorityLow = webVar->ptmPriorityNorm;
   
   /* set connection mode value */
   if (webVar->connMode == CMS_CONNECTION_MODE_VLANMUX)
   {
      CMSMEM_REPLACE_STRING(ptmLink->X_BROADCOM_COM_ConnectionMode, MDMVS_VLANMUXMODE);
   }

   ptmLink->X_BROADCOM_COM_PTMEnbQos = webVar->enblQos;

   /* replace connection scheduler algorithm with new value */
   CMSMEM_REPLACE_STRING(ptmLink->X_BROADCOM_COM_GrpScheduler, webVar->grpScheduler);
   
   /* set connection MPAAL weight value */
   ptmLink->X_BROADCOM_COM_GrpWeight = webVar->grpWeight;

   /* set connection MPAAL priority */
   ptmLink->X_BROADCOM_COM_GrpPrecedence = webVar->grpPrecedence;
   
   /* replace default queue scheduler algorithm with new value */
   CMSMEM_REPLACE_STRING(ptmLink->X_BROADCOM_COM_SchedulerAlgorithm, webVar->schedulerAlgorithm);
   
   /* set default queue weight value */
   ptmLink->X_BROADCOM_COM_QueueWeight = webVar->queueWeight;

   /* set default queue priority */
   ptmLink->X_BROADCOM_COM_QueuePrecedence = webVar->queuePrecedence;
   
   CMSMEM_REPLACE_STRING(ptmLink->X_BROADCOM_COM_DropAlgorithm, webVar->dropAlgorithm);
   ptmLink->X_BROADCOM_COM_LowClassMinThreshold  = webVar->loMinThreshold;
   ptmLink->X_BROADCOM_COM_LowClassMaxThreshold  = webVar->loMaxThreshold;
   ptmLink->X_BROADCOM_COM_HighClassMinThreshold = webVar->hiMinThreshold;
   ptmLink->X_BROADCOM_COM_HighClassMaxThreshold = webVar->hiMaxThreshold;

   /* set default queue minimum shaping rate */
   if (webVar->queueMinimumRate > QOS_QUEUE_NO_SHAPING && webVar->queueShapingBurstSize > 0)
   {
      ptmLink->X_BROADCOM_COM_QueueMinimumRate      = webVar->queueMinimumRate;
      ptmLink->X_BROADCOM_COM_QueueShapingBurstSize = webVar->queueShapingBurstSize;
   }
   
   /* set default queue shaping rate */
   if (webVar->queueShapingRate > QOS_QUEUE_NO_SHAPING && webVar->queueShapingBurstSize > 0)
   {
      ptmLink->X_BROADCOM_COM_QueueShapingRate      = webVar->queueShapingRate;
      ptmLink->X_BROADCOM_COM_QueueShapingBurstSize = webVar->queueShapingBurstSize;
   }
   
   cmsLog_debug("ptmLinkCfg->X_BROADCOM_COM_PTMPortId=%d, X_BROADCOM_COM_PTMPriorityHigh=%d, X_BROADCOM_COM_PTMEnbQos=%d",
                ptmLink->X_BROADCOM_COM_PTMPortId, ptmLink->X_BROADCOM_COM_PTMPriorityHigh, 
                ptmLink->X_BROADCOM_COM_PTMEnbQos);
   

   /* set it to enabled assuming that it is created to be used now */
   ptmLink->enable = TRUE;

   /* set Dev2PtmLinkObject */
   if ((ret = cmsObj_set(ptmLink, &ptmLinkIid)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set Dev2PtmLinkObject, ret = %d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_PTM_LINK, &ptmLinkIid);
   }

   /* free Dev2PtmLinkObject */
   cmsObj_free((void **) &ptmLink);

#ifdef SUPPORT_QOS
   /*
    * In TR181, the ptmLink->name is set in the RCL handler function.
    * Get it again so we can have a valid name to pass into defaultQueueAdd
    */
   if (ret == CMSRET_SUCCESS)
   {
      if ((ret = cmsObj_get(MDMOID_DEV2_PTM_LINK, &ptmLinkIid, 0,
                            (void **) &ptmLink)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not get PTM_LINK obj, ret=%d", ret);
      }
      else
      {
         if (ptmLink->X_BROADCOM_COM_PTMEnbQos)
         {
            SINT32 dslLatency = (ptmLink->X_BROADCOM_COM_PTMPortId == PHY0_PATH1) ? PHY0_PATH1 : PHY0_PATH0;

            /* set up default queue for this interface */
            dalQos_queueAdd(ptmLink->name,
                            ptmLink->X_BROADCOM_COM_SchedulerAlgorithm,
                            TRUE, DEFAULT_QUEUE_NAME, 1,
                            ptmLink->X_BROADCOM_COM_QueueWeight,
                            ptmLink->X_BROADCOM_COM_QueuePrecedence,
                            ptmLink->X_BROADCOM_COM_QueueMinimumRate,
                            ptmLink->X_BROADCOM_COM_QueueShapingRate,
                            ptmLink->X_BROADCOM_COM_QueueShapingBurstSize,
                            dslLatency, 0,
                            ptmLink->X_BROADCOM_COM_DropAlgorithm,
                            ptmLink->X_BROADCOM_COM_LowClassMinThreshold,
                            ptmLink->X_BROADCOM_COM_LowClassMaxThreshold,
                            ptmLink->X_BROADCOM_COM_HighClassMinThreshold,
                            ptmLink->X_BROADCOM_COM_HighClassMaxThreshold);
         }

         cmsObj_free((void **) &ptmLink);
      }
   }
#endif

   cmsLog_debug("Exit, ret=%d", ret);

   return ret;
}


CmsRet dalDsl_deletePtmInterface_dev2(WEB_NTWK_VAR *webVar)
{
   UBOOL8 found = FALSE;
   UBOOL8 ptmLinkFound = FALSE;
   InstanceIdStack iidStack;
   InstanceIdStack ptmIidStack;
   Dev2PtmLinkObject *ptmLinkObj = NULL;
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("wanL2IfName: %s", webVar->wanL2IfName);

   INIT_INSTANCE_ID_STACK(&ptmIidStack);

   while (ptmLinkFound == FALSE &&
          cmsObj_getNextFlags(MDMOID_DEV2_PTM_LINK,
                              &ptmIidStack,
                              OGF_NO_VALUE_UPDATE,
                         (void **)&ptmLinkObj) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strcmp(ptmLinkObj->name, (char *)webVar->wanL2IfName) == 0)
      {
         ptmLinkFound = TRUE;

         if (ptmLinkObj->enable == TRUE)
         {
            INIT_INSTANCE_ID_STACK(&iidStack);

            // find upper interface (Ethernet.Link.{i}.) associated
            // with ptmLink to determine this interface can be deleted or not
            while (found == FALSE &&
                   cmsObj_getNextFlags(MDMOID_DEV2_IP_INTERFACE,
                                       &iidStack,
                                       OGF_NO_VALUE_UPDATE,
                                  (void **)&ipIntfObj) == CMSRET_SUCCESS)
            {
               char currL2IntfNameBuf[CMS_IFNAME_LENGTH]={0};

               // Get the L2Intf
               ret = qdmIpIntf_getLayer2IntfNameByLayer3IntfNameLocked_dev2(ipIntfObj->name, currL2IntfNameBuf);

               found = (ret == CMSRET_SUCCESS &&
                        ipIntfObj->X_BROADCOM_COM_Upstream == TRUE &&
                        cmsUtl_strcmp(currL2IntfNameBuf, ptmLinkObj->name) == 0);

               cmsObj_free((void **)&ipIntfObj);
            }

            // there is no upper interface associated with ptmLink
            if (found == FALSE)
            {
               ret = cmsObj_deleteInstance(MDMOID_DEV2_PTM_LINK, &ptmIidStack);
               if (ret != CMSRET_SUCCESS)
               {
                  cmsLog_error("Failed to delete MDMOID_DEV2_PTM_LINK, ret=%d", ret);      
               }
            }
            else
            {
               cmsLog_debug("ptmLink is still in use and cannot be removed");
               ret = CMSRET_REQUEST_DENIED;
            }
         }
         else
         {
            ret = cmsObj_deleteInstance(MDMOID_DEV2_PTM_LINK, &ptmIidStack);
            if (ret != CMSRET_SUCCESS)
            {
               cmsLog_error("Failed to delete MDMOID_DEV2_PTM_LINK, ret=%d", ret);      
            }
         }
      }            

      cmsObj_free((void **) &ptmLinkObj);
   }

   if (ptmLinkFound == FALSE)
   {
      cmsLog_error("Cannot find PTM link with wanL2IfName == %s",
                   webVar->wanL2IfName);
      ret = CMSRET_INVALID_ARGUMENTS;
   }

   cmsLog_debug("Exit, ret=%d", ret);
   
   return ret;
}


CmsRet dalDsl_deletePtmInterfaceWithoutIfName_dev2(WEB_NTWK_VAR * webVar)
{
   UBOOL8 ptmLinkFound = FALSE;
   InstanceIdStack ptmIidStack;
   Dev2PtmLinkObject *ptmLinkObj = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("port %d priorityHigh %d priorityLow %d",
                webVar->portId, webVar->ptmPriorityHigh, webVar->ptmPriorityNorm);

   INIT_INSTANCE_ID_STACK(&ptmIidStack);

   while (ptmLinkFound == FALSE &&
          cmsObj_getNextFlags(MDMOID_DEV2_PTM_LINK,
                              &ptmIidStack,
                              OGF_NO_VALUE_UPDATE,
                         (void **)&ptmLinkObj) == CMSRET_SUCCESS)
   {
      cmsLog_debug("ptm obj portId=%d, priorityHigh=%d, priorityLow=%d",
                   ptmLinkObj->X_BROADCOM_COM_PTMPortId,
                   ptmLinkObj->X_BROADCOM_COM_PTMPriorityHigh,
                   ptmLinkObj->X_BROADCOM_COM_PTMPriorityLow);

      if ((PORTID_TO_PORTMASK(webVar->portId) & PORTID_TO_PORTMASK(ptmLinkObj->X_BROADCOM_COM_PTMPortId)) &&
          ((webVar->ptmPriorityHigh & ptmLinkObj->X_BROADCOM_COM_PTMPriorityHigh) ||
           (webVar->ptmPriorityNorm & ptmLinkObj->X_BROADCOM_COM_PTMPriorityLow)))
      {
         ptmLinkFound = TRUE;

         cmsUtl_strcpy(webVar->wanL2IfName, ptmLinkObj->name);
      }

      cmsObj_free((void **)&ptmLinkObj);
   }

   if (ptmLinkFound == TRUE)
   {
      ret = dalDsl_deletePtmInterface(webVar);
   }
   else
   {
      cmsLog_error("Cannot find PTM link port %d priorityHigh %d, priorityLow %d",
                   webVar->portId, webVar->ptmPriorityHigh, webVar->ptmPriorityNorm);
      ret = CMSRET_INVALID_ARGUMENTS;
   }

   cmsLog_debug("Exit, ret=%d", ret);

   return ret;
}

#endif   /* DMP_DEVICE2_PTMLINK_1 */



#ifdef DMP_DEVICE2_BONDEDDSL_1
CmsRet dalDsl_setDslBonding_dev2(int enable)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2DslBondingGroupObject *dslBondingObj=NULL;
   CmsRet ret = CMSRET_SUCCESS;
   
   cmsLog_debug("Set DSL Bonding to %s ", enable ? "Enable" : "Disable") ;
   
   /* go to both groups (ATM/PTM) and enable/disable them */
   while ((cmsObj_getNext(MDMOID_DEV2_DSL_BONDING_GROUP, &iidStack, (void **) &dslBondingObj)) == CMSRET_SUCCESS)
   {
      if (dslBondingObj->enable != (UBOOL8) enable)
      {
         dslBondingObj->enable = (UBOOL8) enable;
         ret = cmsObj_set(dslBondingObj, &iidStack);
         cmsObj_free((void **) &dslBondingObj);
      }
   }
   return ret;
}

CmsRet dalDsl_getDslBonding_dev2(int *enable)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2DslBondingGroupObject *dslBondingObj=NULL;
   CmsRet ret = CMSRET_SUCCESS;
   
   cmsLog_debug("Get DSL Bonding to %s ", (*enable) ? "Enable" : "Disable") ;
   
   /* go to both groups (ATM/PTM) and enable/disable them */
   while ((cmsObj_getNext(MDMOID_DEV2_DSL_BONDING_GROUP, &iidStack, (void **) &dslBondingObj)) == CMSRET_SUCCESS)
   {
      *enable = (int)dslBondingObj->enable;
      cmsObj_free((void **) &dslBondingObj);
      break;
   }
   cmsLog_debug("Get DSL XTM Bonding %s", *enable ? "Enable" : "Disable") ;
   return ret;
}

#endif  /* DMP_DEVICE2_BONDEDDSL_1 */


#endif  /* DMP_DEVICE2_DSL_1 */

#endif  /* DMP_DEVICE2_BASELINE_1 */


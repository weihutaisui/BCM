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


#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"
#include "cms_qos.h"
#include "dal.h"
#include "dal_network.h"
#include "adslctlapi.h" /* for ANNEX_A_LINE_PAIR_INNER/OUTER */
#include "devctl_xtm.h"

#ifdef DMP_ADSLWAN_1
extern CmsRet rutDsl_fillL2IfName(const Layer2IfNameType ifNameType, char **ifName);
extern UBOOL8 rutDsl_getDslWanDevIidStack(UBOOL8 isAtm, InstanceIdStack *wanDevIid);
extern UBOOL8 rutDsl_getDslLinkByIfName(char *ifName, InstanceIdStack *iidStack, WanDslLinkCfgObject **dslLinkCfg);
#ifdef DMP_PTMWAN_1
extern UBOOL8 rutDsl_getPtmLinkByIfName(char *ifName, InstanceIdStack *iidStack, WanPtmLinkCfgObject **ptmLinkCfg);
extern CmsRet rutWl2_getPtmDslIntfObject(InstanceIdStack *iidStack, WanDslIntfCfgObject **wanDslIntfObj);

#endif

#ifdef DMP_X_BROADCOM_COM_ATMWAN_1

CmsRet dalDsl_getAtmDslIntfObject(InstanceIdStack *iidStack,
                                  WanDslIntfCfgObject **wanDslIntfObj);
extern CmsRet rutWl2_getAtmDslIntfObject(InstanceIdStack *iidStack,
                               WanDslIntfCfgObject **wanDslIntfObj);
#endif

#ifdef DMP_X_BROADCOM_COM_DSLBONDING_1
extern CmsRet rutWl2_getBondingPtmDslIntfObject(InstanceIdStack *iidStack, WanDslIntfCfgObject **wanDslIntfObj);
extern CmsRet rutWl2_getBondingAtmDslIntfObject(InstanceIdStack *iidStack, WanDslIntfCfgObject **wanDslIntfObj);
#endif

#endif /* #ifdef DMP_ADSLWAN_1 */

#ifdef SUPPORT_DSL_BONDING
extern UBOOL8 rutDsl_isDslBondingEnabled(void);

UBOOL8 dalDsl_isDslBondingEnabled(void)
{
   return (rutDsl_isDslBondingEnabled());
}
#endif /* SUPPORT_DSL_BONDING */

#ifdef DMP_ADSLWAN_1 
CmsRet dalDsl_fillL2IfName(const Layer2IfNameType ifNameType, char **ifName)
{
   return(rutDsl_fillL2IfName(ifNameType, ifName));
}

UBOOL8 dalDsl_getDslWanDevIidStack(UBOOL8 isAtm, InstanceIdStack *wanDevIid)
{
   return rutDsl_getDslWanDevIidStack(isAtm, wanDevIid);   
}

UBOOL8 getDslLinkCfg(UINT32 portId, const char *destAddr, InstanceIdStack *iidStack, WanDslLinkCfgObject **dslLinkCfg)
{
   WanDslLinkCfgObject *obj;
   UBOOL8 found=FALSE;
   UBOOL8 isAtm;
   InstanceIdStack wanDevIid;

   isAtm = TRUE;
   if (dalDsl_getDslWanDevIidStack(isAtm, &wanDevIid) == FALSE)
   {
      return found;
   }
   while (cmsObj_getNextInSubTree(MDMOID_WAN_DSL_LINK_CFG, &wanDevIid, iidStack, (void **)&obj) == CMSRET_SUCCESS)
   {
      cmsLog_debug("vcc %d/%s, obj_vcc=%d/%s",
             portId, destAddr, obj->X_BROADCOM_COM_ATMInterfaceId, obj->destinationAddress);
      if ((PORTID_TO_PORTMASK(portId) & PORTID_TO_PORTMASK(obj->X_BROADCOM_COM_ATMInterfaceId)) &&
          (!cmsUtl_strcmp(destAddr, obj->destinationAddress)))
      {
         found = TRUE;
         break;
      }
      else
      {
         cmsObj_free((void **) &obj);
      }
   }

   if (found)
   {
      if (dslLinkCfg != NULL)
      {
         *dslLinkCfg = obj;
      }
      else
      {
         cmsObj_free((void **) &obj);
      }
   }

   return found;
}


UBOOL8 dalDsl_getDslLinkByIfName(char *ifName, InstanceIdStack *iidStack, WanDslLinkCfgObject **dslLinkCfg)
{
   return (rutDsl_getDslLinkByIfName(ifName, iidStack, dslLinkCfg));
}


/** Fill the given DslLinkCfg object with the data in the webVar
 *
 */
CmsRet fillDslLinkCfg(const WEB_NTWK_VAR *webVar, const char *destAddr, WanDslLinkCfgObject *dslLinkCfg)
{
   CmsRet ret=CMSRET_SUCCESS;
   Layer2IfNameType ifNameType = ATM_EOA;
   
   /* set portid */
   dslLinkCfg->X_BROADCOM_COM_ATMInterfaceId = webVar->portId;
   
   /* set connection mode value */
   if (webVar->connMode == CMS_CONNECTION_MODE_VLANMUX)
   {
      CMSMEM_REPLACE_STRING(dslLinkCfg->X_BROADCOM_COM_ConnectionMode, MDMVS_VLANMUXMODE);
   }
      
   /* set Qos enable */


   dslLinkCfg->X_BROADCOM_COM_ATMEnbQos = webVar->enblQos;
   
   if (dslLinkCfg->X_BROADCOM_COM_ATMEnbQos)
   {
      /* replace connection scheduler algorithm with new value */
      CMSMEM_REPLACE_STRING(dslLinkCfg->X_BROADCOM_COM_GrpScheduler, webVar->grpScheduler);
      
      /* set connection MPAAL weight value */
      dslLinkCfg->X_BROADCOM_COM_GrpWeight = webVar->grpWeight;

      /* set connection MPAAL priority */
      dslLinkCfg->X_BROADCOM_COM_GrpPrecedence = webVar->grpPrecedence;
      
      /* replace default queue scheduler algorithm with new value */
      CMSMEM_REPLACE_STRING(dslLinkCfg->X_BROADCOM_COM_SchedulerAlgorithm, webVar->schedulerAlgorithm);
      
      /* set default queue weight value */
      dslLinkCfg->X_BROADCOM_COM_QueueWeight = webVar->queueWeight;

      /* set default queue priority */
      dslLinkCfg->X_BROADCOM_COM_QueuePrecedence = webVar->queuePrecedence;
      
      CMSMEM_REPLACE_STRING(dslLinkCfg->X_BROADCOM_COM_DropAlgorithm, webVar->dropAlgorithm);
      dslLinkCfg->X_BROADCOM_COM_LowClassMinThreshold  = webVar->loMinThreshold;
      dslLinkCfg->X_BROADCOM_COM_LowClassMaxThreshold  = webVar->loMaxThreshold;
      dslLinkCfg->X_BROADCOM_COM_HighClassMinThreshold = webVar->hiMinThreshold;
      dslLinkCfg->X_BROADCOM_COM_HighClassMaxThreshold = webVar->hiMaxThreshold;

      /* replace ATMEncapsulation with new value */
      cmsMem_free(dslLinkCfg->ATMEncapsulation);
   }

   /* replace destinationAddress with new value */
   CMSMEM_REPLACE_STRING(dslLinkCfg->destinationAddress, destAddr);

   if (cmsUtl_strcmp(webVar->linkType, MDMVS_PPPOA) != 0)
   {
      if (webVar->encapMode == 0)
      {
         dslLinkCfg->ATMEncapsulation = cmsMem_strdup(MDMVS_LLC);
      }
      else
      {
         dslLinkCfg->ATMEncapsulation = cmsMem_strdup(MDMVS_VCMUX);
      }
   }
   else
   {
      /* for PPPOA vcMux is the default ie. encapMode = 0 */
      if (webVar->encapMode == 0)
      {
         dslLinkCfg->ATMEncapsulation = cmsMem_strdup(MDMVS_VCMUX);
      }
      else
      {
         dslLinkCfg->ATMEncapsulation = cmsMem_strdup(MDMVS_LLC);
      }
   }

   if (webVar->atmServiceCategory[0] != '\0')
   {
      CMSMEM_REPLACE_STRING(dslLinkCfg->ATMQoS, webVar->atmServiceCategory);
   }

   /* assigne ifNameType, for pppoe, bridge and mer, it's MDMVS_EOA */
   if (webVar->linkType[0] != '\0')
   {
      CMSMEM_REPLACE_STRING(dslLinkCfg->linkType, webVar->linkType);
   }

   /* get the correct ifNameType */
   if  (cmsUtl_strcmp(dslLinkCfg->linkType, MDMVS_IPOA) == 0)
   {
      ifNameType = ATM_IPOA;
   }
   else if (cmsUtl_strcmp(dslLinkCfg->linkType, MDMVS_PPPOA) == 0)
   {
      ifNameType = ATM_PPPOA;
   }
   /* fill the ifName */
   if (dalDsl_fillL2IfName(ifNameType, &(dslLinkCfg->X_BROADCOM_COM_IfName)) != CMSRET_SUCCESS)
   {
      return CMSRET_INTERNAL_ERROR;
   }

   if ((dslLinkCfg->destinationAddress == NULL) ||
       (dslLinkCfg->ATMEncapsulation == NULL) ||
       (dslLinkCfg->ATMQoS == NULL) ||
       (dslLinkCfg->linkType == NULL))
   {
      cmsLog_error("malloc failed.");
      cmsObj_free((void **) &dslLinkCfg);
      return CMSRET_RESOURCE_EXCEEDED;
   }
      
   dslLinkCfg->ATMPeakCellRate = webVar->atmPeakCellRate;
   dslLinkCfg->ATMSustainableCellRate = webVar->atmSustainedCellRate;
   dslLinkCfg->ATMMaximumBurstSize = webVar->atmMaxBurstSize;
   dslLinkCfg->X_BROADCOM_COM_ATMMinimumCellRate = webVar->atmMinCellRate;
   /* leave atm interface disabled.  Will be enabled when it is used later on */

   cmsLog_debug("linkType=%s", dslLinkCfg->linkType);
   cmsLog_debug("ATMQoS=%s", dslLinkCfg->ATMQoS);
   cmsLog_debug("ATMEncap=%s", dslLinkCfg->ATMEncapsulation);
   cmsLog_debug("destAddr=%s", dslLinkCfg->destinationAddress);
   cmsLog_debug("PeakCellRate=%d", dslLinkCfg->ATMPeakCellRate);
   cmsLog_debug("SustainCellRate=%d", dslLinkCfg->ATMSustainableCellRate);
   cmsLog_debug("MaxBurstSize=%d", dslLinkCfg->ATMMaximumBurstSize);
   cmsLog_debug("MinCellRate=%d", dslLinkCfg->X_BROADCOM_COM_ATMMinimumCellRate);

   return ret;
}


CmsRet dalDsl_addAtmInterface_igd(const WEB_NTWK_VAR *webVar)
{
   char destAddr[BUFLEN_128];   
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanDslLinkCfgObject *dslLinkCfg = NULL;
   UBOOL8 existingDslLinkFound;
   UBOOL8 isAtm;
   CmsRet ret = CMSRET_SUCCESS;
   
   cmsLog_debug("Adding port=%d vpi=%d vci=%d", webVar->portId, webVar->atmVpi, webVar->atmVci);

   if ((ret = cmsUtl_atmVpiVciNumToStr(webVar->atmVpi, webVar->atmVci, destAddr)) != CMSRET_SUCCESS)
   {
      return ret;
   }

    /* Normal ATM PVC add case. */
   existingDslLinkFound = getDslLinkCfg(webVar->portId, destAddr, &iidStack, NULL);
   if (existingDslLinkFound)
   {
      cmsLog_error("This DSL Interface is already existed.");
      return CMSRET_INTERNAL_ERROR;
   }

   /* adding a new service on a new PORT/VPI/VCI */
   cmsLog_debug("Adding new WanConnDev at %d/%d/%d", webVar->portId, webVar->atmVpi, webVar->atmVci);
   
   /* first we need to get the iidStack of the ATM WanDevice object. */
   isAtm = TRUE;
   if (dalDsl_getDslWanDevIidStack(isAtm, &iidStack) == FALSE)
   {
      return CMSRET_INTERNAL_ERROR;
   }      

   /* add new instance of WanConnectionDevice */
   if ((ret = cmsObj_addInstance(MDMOID_WAN_CONN_DEVICE, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create new WanConnectionDevice, ret=%d", ret);
      return ret;
   }
   
   /* get the instance of dslLinkConfig in the newly created
    * WanConnectionDevice sub-tree */
   if ((ret = cmsObj_get(MDMOID_WAN_DSL_LINK_CFG, &iidStack, 0, (void **) &dslLinkCfg)) != CMSRET_SUCCESS)
   {
      cmsObj_deleteInstance(MDMOID_WAN_CONN_DEVICE, &iidStack);
      cmsLog_error("Failed to get dslLinkCfgObj, ret=%d", ret);
      return ret;
   }       
   
   if ((ret = fillDslLinkCfg(webVar, destAddr, dslLinkCfg)) != CMSRET_SUCCESS)
   {
      cmsObj_free((void **) &dslLinkCfg);
      cmsObj_deleteInstance(MDMOID_WAN_CONN_DEVICE, &iidStack);
      return ret;
   }

   dslLinkCfg->enable = TRUE;

   /* set WanDslLinkCfgObject */
   if ((ret = cmsObj_set(dslLinkCfg, &iidStack)) == CMSRET_SUCCESS)
   {
#ifdef DMP_QOS_1
      if (dslLinkCfg->X_BROADCOM_COM_ATMEnbQos)
      {
         SINT32 dslLatency = (dslLinkCfg->X_BROADCOM_COM_ATMInterfaceId == PHY0_PATH1) ? PHY0_PATH1 : PHY0_PATH0;

         /* set up default queue for this interface */
         dalQos_queueAdd(dslLinkCfg->X_BROADCOM_COM_IfName,
                         dslLinkCfg->X_BROADCOM_COM_SchedulerAlgorithm,
                         TRUE, DEFAULT_QUEUE_NAME, 1,
                         dslLinkCfg->X_BROADCOM_COM_QueueWeight,
                         dslLinkCfg->X_BROADCOM_COM_QueuePrecedence,
                         QOS_QUEUE_NO_SHAPING,
                         QOS_QUEUE_NO_SHAPING,
                         0,
                         dslLatency, -1,
                         dslLinkCfg->X_BROADCOM_COM_DropAlgorithm,
                         dslLinkCfg->X_BROADCOM_COM_LowClassMinThreshold,
                         dslLinkCfg->X_BROADCOM_COM_LowClassMaxThreshold,
                         dslLinkCfg->X_BROADCOM_COM_HighClassMinThreshold,
                         dslLinkCfg->X_BROADCOM_COM_HighClassMaxThreshold);
      }
#endif
   }
   else
   {
      cmsLog_error("Failed to set dslLinkCfgObj, ret = %d", ret);
      cmsObj_deleteInstance(MDMOID_WAN_CONN_DEVICE, &iidStack);
   }

   cmsObj_free((void **) &dslLinkCfg);
   cmsLog_debug("Exit, ret=%d", ret);
   return ret;
}


CmsRet dalDsl_deleteAtmInterface_igd(WEB_NTWK_VAR *webVar)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanConnDeviceObject *wanConn=NULL;
   CmsRet ret = CMSRET_SUCCESS;

   if (dalDsl_getDslLinkByIfName((char *)webVar->wanL2IfName, &iidStack, NULL) == FALSE)
   {
      cmsLog_debug("Interface %s not found.", webVar->wanL2IfName);
      return CMSRET_INTERNAL_ERROR;
   }

   /*
    * Finally, check if there are any services left on the WanConnectionDevice.
    * If not, delete the entire WanConnectionDevice sub-tree, which includes
    * the DslLinkCfg instance.
    */
   if ((ret = cmsObj_get(MDMOID_WAN_CONN_DEVICE, &iidStack, 0, (void **) &wanConn)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get WanConnDev, ret=%d", ret);
      return ret;
   }

   if (wanConn->WANIPConnectionNumberOfEntries == 0 &&
       wanConn->WANPPPConnectionNumberOfEntries == 0)
   {
      if ((ret = cmsObj_deleteInstance(MDMOID_WAN_CONN_DEVICE, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to delete MDMOID_WAN_CONN_DEVICE sub-tree, ret=%d", ret);      
      }
   }
   else
   {
      cmsLog_debug("WanConnDevice is still in use and cannot be removed");
      ret = CMSRET_REQUEST_DENIED;
   }

   cmsObj_free((void **) &wanConn);

   cmsLog_debug("Exit, ret=%d", ret);
   
   return ret;
}


CmsRet dalDsl_deleteAtmInterfaceWithoutIfName_igd(WEB_NTWK_VAR *webVar)
{
   InstanceIdStack iidStack;
   WanDslLinkCfgObject *dslLinkCfg = NULL;
   CmsRet ret = CMSRET_INVALID_ARGUMENTS;

   cmsLog_debug("port %d vpi %d vci %d",
                webVar->portId, webVar->atmVpi, webVar->atmVci);

   INIT_INSTANCE_ID_STACK(&iidStack);

   // get L2 iface name
   if (dalWan_getDslLinkCfg(webVar, &iidStack, &dslLinkCfg) == TRUE)
   {
      cmsLog_debug("found dslLinkCfg");
      cmsUtl_strcpy(webVar->wanL2IfName, dslLinkCfg->X_BROADCOM_COM_IfName);
      cmsObj_free((void **)&dslLinkCfg);

      ret = dalDsl_deleteAtmInterface(webVar);
   }

   return ret;
}



#ifdef DMP_PTMWAN_1
/*
*  PTM dal functions
*/

UBOOL8 dalDsl_getPtmLinkCfg(UINT32 portId, 
                            UINT32 priorityNorm,
                            UINT32 priorityHigh,
                            InstanceIdStack *ptmIid,
                            WanPtmLinkCfgObject **ptmCfgObj)
{
   WanPtmLinkCfgObject *ptmObj;
   UBOOL8 found = FALSE;
   UBOOL8 isAtm;
   CmsRet ret;
   InstanceIdStack wanDevIid;
   
   isAtm = FALSE;
   if (dalDsl_getDslWanDevIidStack(isAtm, &wanDevIid) ==FALSE)
   {
      return FALSE;
   }

   cmsLog_debug("wanDevIid=%s",  cmsMdm_dumpIidStack(&wanDevIid));

   /* if ptm WANDev is already exist,  need to go through the ptm link cfg
   */
   while ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_PTM_LINK_CFG,  &wanDevIid,  ptmIid, (void **) &ptmObj)) == CMSRET_SUCCESS)
   {

      cmsLog_debug("ptm obj portId=%d, priorityHigh=%d",  ptmObj->X_BROADCOM_COM_PTMPortId,  ptmObj->X_BROADCOM_COM_PTMPriorityHigh);
      if ((PORTID_TO_PORTMASK(portId) & PORTID_TO_PORTMASK(ptmObj->X_BROADCOM_COM_PTMPortId)) &&
          ((priorityHigh & ptmObj->X_BROADCOM_COM_PTMPriorityHigh) ||
           (priorityNorm & ptmObj->X_BROADCOM_COM_PTMPriorityLow)))
      {
         found = TRUE;
         break;
      }
      else
      {
         cmsObj_free((void **) &ptmObj);
      }
   }

   if (found)
   {
      if (ptmCfgObj != NULL)
      {
         *ptmCfgObj = ptmObj;
      }
      else
      {
         cmsObj_free((void **) &ptmObj);
      }
   }

   cmsLog_debug("ptmWanIid return, ptmIid=%s, found=%d ", cmsMdm_dumpIidStack(ptmIid),  found);

   return found;
}



UBOOL8 dalDsl_getPtmLinkByIfName(char *ifName, InstanceIdStack *iidStack, WanPtmLinkCfgObject **ptmLinkCfg)
{
   return (rutDsl_getPtmLinkByIfName(ifName, iidStack, ptmLinkCfg));
}


CmsRet dalDsl_addPtmInterface_igd(const WEB_NTWK_VAR *webVar)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 existingPtmFound;
   WanPtmLinkCfgObject *ptmLinkCfg;
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 isAtm;
   
   cmsLog_debug("Adding port=%d, ptmPriorityHigh=%d, ptmPriorityNorm=%d", 
      webVar->portId, webVar->ptmPriorityHigh, webVar->ptmPriorityNorm);

   existingPtmFound = dalDsl_getPtmLinkCfg(webVar->portId, webVar->ptmPriorityNorm, webVar->ptmPriorityHigh,  &iidStack,  NULL);
   
   if (existingPtmFound)
   {
      cmsLog_error("This DSL PTM Interface is already existed.");
      return CMSRET_INTERNAL_ERROR;
   }

   /* first we need to get the iidStack of the PTM WanDevice object. */
   isAtm = FALSE;
   if (dalDsl_getDslWanDevIidStack(isAtm, &iidStack) == FALSE)
   {
      return CMSRET_INTERNAL_ERROR;
   }      
   
   /* add new instance of WanConnectionDevice */
   if ((ret = cmsObj_addInstance(MDMOID_WAN_CONN_DEVICE, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create new WanConnectionDevice, ret=%d", ret);
      return ret;
   }
   
   /* Get the WANPTMLinkConfig to set the new values to it */
   if ((ret = cmsObj_get(MDMOID_WAN_PTM_LINK_CFG, &iidStack, 0, (void **) &ptmLinkCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get iidStack, ret = %d", ret);
      cmsObj_deleteInstance(MDMOID_WAN_CONN_DEVICE, &iidStack);
      return ret;
   }

  if (dalDsl_fillL2IfName(PTM_EOA, &(ptmLinkCfg->X_BROADCOM_COM_IfName)) != CMSRET_SUCCESS)
  {
      cmsObj_free((void **) &ptmLinkCfg);   
      cmsObj_deleteInstance(MDMOID_WAN_CONN_DEVICE, &iidStack);
      return CMSRET_INTERNAL_ERROR;
  }

   ptmLinkCfg->X_BROADCOM_COM_PTMPortId = webVar->portId;
   ptmLinkCfg->X_BROADCOM_COM_PTMPriorityHigh = webVar->ptmPriorityHigh;
   ptmLinkCfg->X_BROADCOM_COM_PTMPriorityLow = webVar->ptmPriorityNorm;
   
   /* set connection mode value */
   if (webVar->connMode == CMS_CONNECTION_MODE_VLANMUX)
   {
      CMSMEM_REPLACE_STRING(ptmLinkCfg->X_BROADCOM_COM_ConnectionMode, MDMVS_VLANMUXMODE);
   }

   ptmLinkCfg->X_BROADCOM_COM_PTMEnbQos = webVar->enblQos;

   if (ptmLinkCfg->X_BROADCOM_COM_PTMEnbQos)
   {
      /* replace connection scheduler algorithm with new value */
      CMSMEM_REPLACE_STRING(ptmLinkCfg->X_BROADCOM_COM_GrpScheduler, webVar->grpScheduler);
      
      /* set connection MPAAL weight value */
      ptmLinkCfg->X_BROADCOM_COM_GrpWeight = webVar->grpWeight;

      /* set connection MPAAL priority */
      ptmLinkCfg->X_BROADCOM_COM_GrpPrecedence = webVar->grpPrecedence;
      
      /* replace default queue scheduler algorithm with new value */
      CMSMEM_REPLACE_STRING(ptmLinkCfg->X_BROADCOM_COM_SchedulerAlgorithm, webVar->schedulerAlgorithm);
      
      /* set default queue weight value */
      ptmLinkCfg->X_BROADCOM_COM_QueueWeight = webVar->queueWeight;

      /* set default queue priority */
      ptmLinkCfg->X_BROADCOM_COM_QueuePrecedence = webVar->queuePrecedence;
      
      CMSMEM_REPLACE_STRING(ptmLinkCfg->X_BROADCOM_COM_DropAlgorithm, webVar->dropAlgorithm);
      ptmLinkCfg->X_BROADCOM_COM_LowClassMinThreshold  = webVar->loMinThreshold;
      ptmLinkCfg->X_BROADCOM_COM_LowClassMaxThreshold  = webVar->loMaxThreshold;
      ptmLinkCfg->X_BROADCOM_COM_HighClassMinThreshold = webVar->hiMinThreshold;
      ptmLinkCfg->X_BROADCOM_COM_HighClassMaxThreshold = webVar->hiMaxThreshold;

      /* set default queue minimum shaping rate */
      if (webVar->queueMinimumRate > QOS_QUEUE_NO_SHAPING && webVar->queueShapingBurstSize > 0)
      {
         ptmLinkCfg->X_BROADCOM_COM_QueueMinimumRate      = webVar->queueMinimumRate;
         ptmLinkCfg->X_BROADCOM_COM_QueueShapingBurstSize = webVar->queueShapingBurstSize;
      }
      
      /* set default queue shaping rate */
      if (webVar->queueShapingRate > QOS_QUEUE_NO_SHAPING && webVar->queueShapingBurstSize > 0)
      {
         ptmLinkCfg->X_BROADCOM_COM_QueueShapingRate      = webVar->queueShapingRate;
         ptmLinkCfg->X_BROADCOM_COM_QueueShapingBurstSize = webVar->queueShapingBurstSize;
      }

   }

   
   cmsLog_debug("ptmLinkCfg->X_BROADCOM_COM_PTMPortId=%d, X_BROADCOM_COM_PTMPriorityHigh=%d, X_BROADCOM_COM_PTMEnbQos=%d",
     ptmLinkCfg->X_BROADCOM_COM_PTMPortId, ptmLinkCfg->X_BROADCOM_COM_PTMPriorityHigh, ptmLinkCfg->X_BROADCOM_COM_PTMEnbQos);
   

   /* set it to enabled assuming that it is created to be used now */
   ptmLinkCfg->enable = TRUE;

   /* set WanDslLinkCfgObject */
   if ((ret = cmsObj_set(ptmLinkCfg, &iidStack)) == CMSRET_SUCCESS)
   {
#ifdef DMP_QOS_1
      if (ptmLinkCfg->X_BROADCOM_COM_PTMEnbQos)
      {
         SINT32 dslLatency = (ptmLinkCfg->X_BROADCOM_COM_PTMPortId == PHY0_PATH1) ? PHY0_PATH1 : PHY0_PATH0;

         /* set up default queue for this interface */
         dalQos_queueAdd(ptmLinkCfg->X_BROADCOM_COM_IfName,
                         ptmLinkCfg->X_BROADCOM_COM_SchedulerAlgorithm,
                         TRUE, DEFAULT_QUEUE_NAME, 1,
                         ptmLinkCfg->X_BROADCOM_COM_QueueWeight,
                         ptmLinkCfg->X_BROADCOM_COM_QueuePrecedence,
                         ptmLinkCfg->X_BROADCOM_COM_QueueMinimumRate,
                         ptmLinkCfg->X_BROADCOM_COM_QueueShapingRate,
                         ptmLinkCfg->X_BROADCOM_COM_QueueShapingBurstSize,
                         dslLatency, 0,
                         ptmLinkCfg->X_BROADCOM_COM_DropAlgorithm,
                         ptmLinkCfg->X_BROADCOM_COM_LowClassMinThreshold,
                         ptmLinkCfg->X_BROADCOM_COM_LowClassMaxThreshold,
                         ptmLinkCfg->X_BROADCOM_COM_HighClassMinThreshold,
                         ptmLinkCfg->X_BROADCOM_COM_HighClassMaxThreshold);
      }
#endif
   }
   else
   {
      cmsLog_error("Failed to set ptmLinkCfgObj, ret = %d", ret);
      cmsObj_deleteInstance(MDMOID_WAN_CONN_DEVICE, &iidStack);
   }

   cmsObj_free((void **) &ptmLinkCfg);
   cmsLog_debug("Exit, ret=%d", ret);
   return ret;
}


CmsRet dalDsl_deletePtmInterface_igd(WEB_NTWK_VAR *webVar)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanConnDeviceObject *wanConn=NULL;
   CmsRet ret = CMSRET_SUCCESS;
   
   cmsLog_debug("Deleting %s", webVar->wanL2IfName);

   if (dalDsl_getPtmLinkByIfName((char *)webVar->wanL2IfName, &iidStack, NULL) == FALSE)
   {
      cmsLog_debug("Interface %s not found.", webVar->wanL2IfName);
      return CMSRET_INTERNAL_ERROR;
   }
   
   /*
    * Finally, check if there are any services left on the WanConnectionDevice.
    * If not, delete the entire WanConnectionDevice sub-tree, which includes
    * the DslLinkCfg instance.
    */
   if ((ret = cmsObj_get(MDMOID_WAN_CONN_DEVICE, &iidStack, 0, (void **) &wanConn)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get WanConnDev, ret=%d", ret);
      return ret;
   }
   
   if (wanConn->WANIPConnectionNumberOfEntries == 0 &&
       wanConn->WANPPPConnectionNumberOfEntries == 0)
   {
      if ((ret = cmsObj_deleteInstance(MDMOID_WAN_CONN_DEVICE, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to delete MDMOID_WAN_CONN_DEVICE, ret=%d", ret);      
      }
   }
   else
   {
      cmsLog_debug("WanConnDevice is still in use and cannot be removed");
      ret = CMSRET_REQUEST_DENIED;
   }

   cmsObj_free((void **) &wanConn);

   cmsLog_debug("Exit, ret=%d", ret);
   
   return ret;
}


CmsRet dalDsl_deletePtmInterfaceWithoutIfName_igd(WEB_NTWK_VAR * webVar)
{
   InstanceIdStack iidStack;
   WanPtmLinkCfgObject *ptmLinkCfg = NULL;
   CmsRet ret = CMSRET_INVALID_ARGUMENTS;

   cmsLog_debug("port %d priorityHigh %d priorityLow %d",
                webVar->portId, webVar->ptmPriorityHigh, webVar->ptmPriorityNorm);

   INIT_INSTANCE_ID_STACK(&iidStack);

   // get L2 iface name
   if (dalDsl_getPtmLinkCfg(webVar->portId,
                            webVar->ptmPriorityNorm,
                            webVar->ptmPriorityHigh, 
                            &iidStack,
                            &ptmLinkCfg) == TRUE)
   {
      cmsLog_debug("found ptmLinkCfg");
      cmsUtl_strcpy(webVar->wanL2IfName, ptmLinkCfg->X_BROADCOM_COM_IfName);
      cmsObj_free((void **)&ptmLinkCfg);

      ret = dalDsl_deletePtmInterface(webVar);
   }

   return ret;
}

CmsRet dalDsl_getPtmDslIntfObject(InstanceIdStack *iidStack,
                                  WanDslIntfCfgObject **wanDslIntfObj)
{
   return (rutWl2_getPtmDslIntfObject(iidStack, wanDslIntfObj));
}

#endif   /* DMP_PTMWAN_1 */


#ifdef DMP_X_BROADCOM_COM_ATMWAN_1

CmsRet dalDsl_getAtmDslIntfObject(InstanceIdStack *iidStack,
                                  WanDslIntfCfgObject **wanDslIntfObj)
{
   return (rutWl2_getAtmDslIntfObject(iidStack, wanDslIntfObj));
}
#endif

#ifdef DMP_X_BROADCOM_COM_DSLBONDING_1
CmsRet dalDsl_getBondingPtmDslIntfObject(InstanceIdStack *iidStack, WanDslIntfCfgObject **wanDslIntfObj)
{
   return (rutWl2_getBondingPtmDslIntfObject(iidStack, wanDslIntfObj));
}

CmsRet dalDsl_getBondingAtmDslIntfObject(InstanceIdStack *iidStack, WanDslIntfCfgObject **wanDslIntfObj)
{
   return (rutWl2_getBondingAtmDslIntfObject(iidStack, wanDslIntfObj));
}

CmsRet dalDsl_setDslBonding_igd(int enable)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanDslIntfCfgObject *dslIntfObj=NULL;
   CmsRet ret = CMSRET_SUCCESS;
   
   cmsLog_debug("Set DSL Bonding to %s ", enable ? "Enable" : "Disable") ;

   /* PTM Bonding */
   if ((ret = dalDsl_getPtmDslIntfObject(&iidStack, &dslIntfObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get PtmDslBondingCfgObject, ret=%d", ret);
      return ret;
   } 
   
   dslIntfObj->X_BROADCOM_COM_EnableBonding = (UBOOL8) enable;
   /* set PtmDslBondingCfgObject */
   ret = cmsObj_set(dslIntfObj, &iidStack);
   cmsObj_free((void **) &dslIntfObj);

   if (ret == CMSRET_SUCCESS)
   {
      /* also enable the secondary object */
      if ((ret = dalDsl_getBondingPtmDslIntfObject(&iidStack, &dslIntfObj)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to get PtmDslBondingCfgObject, ret=%d", ret);
         return ret;
      }
   
      dslIntfObj->X_BROADCOM_COM_EnableBonding = (UBOOL8) enable;
      /* set DslBondingCfgObject */
      ret = cmsObj_set(dslIntfObj, &iidStack);
      cmsObj_free((void **) &dslIntfObj);
   }

   /* ATM Bonding */

   memset (&iidStack, 0, sizeof (InstanceIdStack)) ;
   dslIntfObj=NULL;
   ret = CMSRET_SUCCESS;

   if ((ret = dalDsl_getAtmDslIntfObject(&iidStack, &dslIntfObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get AtmDslBondingCfgObject, ret=%d", ret);
      return ret;
   } 
   
   dslIntfObj->X_BROADCOM_COM_EnableBonding = (UBOOL8) enable;
   /* set DslBondingCfgObject */
   ret = cmsObj_set(dslIntfObj, &iidStack);
   cmsObj_free((void **) &dslIntfObj);

   if (ret == CMSRET_SUCCESS)
   {
      /* also enable the secondary object */
      if ((ret = dalDsl_getBondingAtmDslIntfObject(&iidStack, &dslIntfObj)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to get AtmDslBondingCfgObject, ret=%d", ret);
         return ret;
      }
   
      dslIntfObj->X_BROADCOM_COM_EnableBonding = (UBOOL8) enable;
      /* set Atm DslBondingCfgObject */
      ret = cmsObj_set(dslIntfObj, &iidStack);
      cmsObj_free((void **) &dslIntfObj);
   }

   return ret;
}

CmsRet dalDsl_getDslBonding_igd(int *enable)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanDslIntfCfgObject *dslIntfObj=NULL;
   CmsRet ret = CMSRET_SUCCESS;
  
   /* just need to get one.  If bonding is enabled in one, it should be enabled in the other */
   if ((ret = dalDsl_getPtmDslIntfObject(&iidStack, &dslIntfObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get Ptm DslBondingCfgObject, ret=%d", ret);
      memset (&iidStack, 0, sizeof (InstanceIdStack)) ;
      if ((ret = dalDsl_getAtmDslIntfObject(&iidStack, &dslIntfObj)) != CMSRET_SUCCESS) {
         cmsLog_error("Failed to get Ptm DslBondingCfgObject, ret=%d", ret);
         return ret;
      }
   }

   *enable = dslIntfObj->X_BROADCOM_COM_EnableBonding;
   cmsObj_free((void **) &dslIntfObj);

   cmsLog_debug("Get DSL XTM Bonding %s", *enable ? "Enable" : "Disable") ;
   return ret;
}

#endif  /* DMP_X_BROADCOM_COM_DSLBONDING_1 */


#endif /* DMP_ADSLWAN_1 */

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


#ifdef SUPPORT_CELLULAR
#ifdef DMP_CELLULARINTERFACEBASE_1


/* this is the TR181 implementation of Cellular */


#include "cms_core.h"
#include "cms_dal.h"
#include "cms_qdm.h"
#include "cms_util.h"
#include "dal.h"
#include "dal2_wan.h"


void dalCellular_getCellularCfg(WEB_NTWK_VAR *webVar)
{
   Dev2CellularObject* cellularObj = NULL;
   Dev2CellularInterfaceObject* cellularIntfObj = NULL;
   Dev2CellularInterfaceUsimObject* cellularIntfUsimObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;
   
   cmsLog_debug("update glbWebVar Cellular info from MDM");

   if ((ret = cmsObj_get(MDMOID_DEV2_CELLULAR, &iidStack, OGF_NO_VALUE_UPDATE,
                         (void **) &cellularObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get MDMOID_DEV2_CELLULAR error, ret = %d", ret);
      return;	  
   }   	

   webVar->cellularRoamingEnbl = cellularObj->roamingEnabled;
   
   cmsObj_free((void **) &cellularObj);

   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_getNext(MDMOID_DEV2_CELLULAR_INTERFACE, &iidStack,
                        (void**) &cellularIntfObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get MDMOID_DEV2_CELLULAR_INTERFACE error, ret = %d", ret);
      return;	  
   }   	

   webVar->cellularIntfEnable = cellularIntfObj->enable;
   STR_COPY_OR_NULL_TERMINATE(webVar->cellularSupportedAccTech, cellularIntfObj->supportedAccessTechnologies);
//   STR_COPY_OR_NULL_TERMINATE(webVar->cellularPrefdAccTech, cellularIntfObj->preferredAccessTechnology);
//   STR_COPY_OR_NULL_TERMINATE(webVar->cellularNtwkReq, cellularIntfObj->networkRequested);
//   STR_COPY_OR_NULL_TERMINATE(webVar->cellularNtwkReq_MCCMNC, cellularIntfObj->X_BROADCOM_COM_NetworkRequested_MCCMNC);
   
   cmsObj_free((void **) &cellularIntfObj);

   if ((ret = cmsObj_get(MDMOID_DEV2_CELLULAR_INTERFACE_USIM, &iidStack, OGF_NO_VALUE_UPDATE,
                         (void **) &cellularIntfUsimObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get MDMOID_DEV2_CELLULAR_INTERFACE_USIM error, ret = %d", ret);
      return;	  
   }   	

//   STR_COPY_OR_NULL_TERMINATE(webVar->cellularPinChk, cellularIntfUsimObj->PINCheck);
//   STR_COPY_OR_NULL_TERMINATE(webVar->cellularPin, cellularIntfUsimObj->PIN);

   cmsObj_free((void **) &cellularIntfUsimObj);
}


CmsRet dalCellular_configCellular(UBOOL8 roamingEnabled)
{
   CmsRet ret;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2CellularObject* cellularObj = NULL;

   cmsLog_debug("Enter: roamingEnabled=%d", roamingEnabled);
   
   if ((ret = cmsObj_get(MDMOID_DEV2_CELLULAR, &iidStack, OGF_NO_VALUE_UPDATE,
                         (void **) &cellularObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_get MDMOID_DEV2_CELLULAR returns error. ret=%d", ret);
      return ret;
   }

   if (cellularObj->roamingEnabled == roamingEnabled)
   {
      cmsLog_debug("There is no change in roamingEnabled configuration");
      cmsObj_free((void **)&cellularObj);
      return CMSRET_SUCCESS;
   }

   /* overwrite with user's configuration */
   cellularObj->roamingEnabled = roamingEnabled;

   if ((ret = cmsObj_set((void *)cellularObj, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("set of MDMOID_DEV2_CELLULAR object failed, ret=%d", ret);
   }

   cmsObj_free((void **)&cellularObj);

   return ret;
}


CmsRet dalCellular_configCellularInterface(UBOOL8 cellularIntfEnable, char* cellularPrefdAccTech, 
                                             char* cellularNtwkReq, char* cellularNtwkReq_MCCMNC, char* cellularPinChk, char* cellularPin)
{
   CmsRet ret;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2CellularInterfaceObject* cellularIntfObj = NULL;
//   Dev2CellularInterfaceUsimObject* cellularIntfUsimObj = NULL;

   cmsLog_debug("cellularIntfEnable=%d, cellularPrefdAccTech=%s, cellularNtwkReq=%s, cellularNtwkReq_MCCMNC=%s, cellularPinChk=%s, cellularPin=%s", 
                cellularIntfEnable, cellularPrefdAccTech, cellularNtwkReq, cellularNtwkReq_MCCMNC, cellularPinChk, cellularPin);

   if ((ret = cmsObj_getNext(MDMOID_DEV2_CELLULAR_INTERFACE, &iidStack,
                        (void**) &cellularIntfObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_get MDMOID_DEV2_CELLULAR_INTERFACE returns error. ret=%d", ret);
      return ret;
   }

   if (cellularIntfObj->enable == cellularIntfEnable) 
//      && !cmsUtl_strcmp(cellularIntfObj->preferredAccessTechnology, cellularPrefdAccTech) && 
//      !cmsUtl_strcmp(cellularIntfObj->networkRequested, cellularNtwkReq)) 
   {
      cmsLog_debug("There is no change in MDMOID_DEV2_CELLULAR_INTERFACE configuration");
   }
   else
   {
      cellularIntfObj->enable = cellularIntfEnable;   
//      REPLACE_STRING_IF_NOT_EQUAL(cellularIntfObj->preferredAccessTechnology, cellularPrefdAccTech);
//      REPLACE_STRING_IF_NOT_EQUAL(cellularIntfObj->networkRequested, cellularNtwkReq);
//      REPLACE_STRING_IF_NOT_EQUAL(cellularIntfObj->X_BROADCOM_COM_NetworkRequested_MCCMNC, cellularNtwkReq_MCCMNC);

      if ((ret = cmsObj_set((void *)cellularIntfObj, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("set of MDMOID_DEV2_CELLULAR_INTERFACE object failed, ret=%d", ret);
      }
   }
   
  cmsObj_free((void **)&cellularIntfObj);

//Sarah: TODO: phase 2 feature
#if 0  
   if ((ret = cmsObj_get(MDMOID_DEV2_CELLULAR_INTERFACE_USIM, &iidStack, OGF_NO_VALUE_UPDATE,
                        (void **) &cellularIntfUsimObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_get MDMOID_DEV2_CELLULAR_INTERFACE returns error. ret=%d", ret);
      return ret;
   }
   
   if (!cmsUtl_strcmp(cellularIntfUsimObj->PINCheck, cellularPinChk) && 
       !cmsUtl_strcmp(cellularIntfUsimObj->PIN, cellularPin))
   {
      cmsLog_debug("There is no change in MDMOID_DEV2_CELLULAR_INTERFACE_USIM configuration");
      cmsObj_free((void **)&cellularIntfUsimObj);
      return ret;
   }

   REPLACE_STRING_IF_NOT_EQUAL(cellularIntfUsimObj->PINCheck, cellularPinChk);
   REPLACE_STRING_IF_NOT_EQUAL(cellularIntfUsimObj->PIN, cellularPin);

   if ((ret = cmsObj_set((void *)cellularIntfUsimObj, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("set of MDMOID_DEV2_CELLULAR_INTERFACE_USIM object failed, ret=%d", ret);
   }

   cmsObj_free((void **)&cellularIntfUsimObj);
#endif

   return ret;
}       


#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
CmsRet dalCellular2_addCellularWanService(const char *ifname)
{
   char *lowerLayerFullPath=NULL;
   UINT8 lowerLayerFullPathLen;   
   char IpIntfPathRef[MDM_SINGLE_FULLPATH_BUFLEN] = {0};
   Dev2CellularLinkObject *cellularLinkObj;
   MdmPathDescriptor ethVlanTerminationPathDesc;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;   
   MdmPathDescriptor ipIntfPathDesc;
   CmsRet ret;

   ret = qdmIntf_intfnameToFullPathLocked(ifname, TRUE, &lowerLayerFullPath);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get fullpath of %s, ret=%d", ifname, ret);
      return ret;
   }

   cmsLog_debug("lowerLayerFullPath = %s", lowerLayerFullPath);


   if ((ret = cmsObj_addInstance(MDMOID_DEV2_CELLULAR_LINK, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to add cellularLink Instance, ret = %d", ret);
      CMSMEM_FREE_BUF_AND_NULL_PTR(lowerLayerFullPath);
      return ret;
   } 

   if ((ret = cmsObj_get(MDMOID_DEV2_CELLULAR_LINK, &iidStack, 0, (void **) &cellularLinkObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get cellularLinkObj, ret = %d", ret);
      CMSMEM_FREE_BUF_AND_NULL_PTR(lowerLayerFullPath);
      cmsObj_deleteInstance(MDMOID_DEV2_CELLULAR_LINK, &iidStack);
      return ret;
   }
   
   lowerLayerFullPathLen = strlen(lowerLayerFullPath);
   if (lowerLayerFullPath[lowerLayerFullPathLen-1] == '.')
   {
      lowerLayerFullPath[lowerLayerFullPathLen-1] = '\0';
   }   
   CMSMEM_REPLACE_STRING(cellularLinkObj->lowerLayers, lowerLayerFullPath);
   CMSMEM_FREE_BUF_AND_NULL_PTR(lowerLayerFullPath);   
   cellularLinkObj->enable = TRUE;

   /* MacAddress need to be set when the layer 2 link is up in rcl/rut */
   ret =  cmsObj_set(cellularLinkObj, &iidStack);
   cmsObj_free((void **) &cellularLinkObj); 

   if (ret  != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set cellularLinkObj. ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_CELLULAR_LINK, &iidStack);
      return ret;
   } 

      /* Need to get the Cellular.X_BROADCOM_COM_Link object full path string to be used as ip/ppp Intf lowlayerStr */
   INIT_PATH_DESCRIPTOR(&ethVlanTerminationPathDesc);
   ethVlanTerminationPathDesc.iidStack = iidStack;
   ethVlanTerminationPathDesc.oid = MDMOID_DEV2_CELLULAR_LINK;
   if ((ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&ethVlanTerminationPathDesc, &lowerLayerFullPath)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_pathDescriptorToFullPath returns error. ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_CELLULAR_LINK, &iidStack);
      return ret;
   }

  
   if ((ret = dalIp_addIntfObject_dev2(FALSE, //supportIpv4
                                       TRUE, //supportIpv6
                                       NULL,  /* intfGroupName */
                                       FALSE, NULL,  /* isBridgeService, bridgeName */
                                       FALSE, //enblFirewall
                                       FALSE, FALSE, //enableIgmp, enableIgmpSource
                                       FALSE, FALSE, //enableMld, enableMldSource
                                       lowerLayerFullPath,
                                       IpIntfPathRef,
                                       sizeof(IpIntfPathRef), 
                                       &ipIntfPathDesc)) != CMSRET_SUCCESS)
   {
      cmsLog_error("dalIp_addIntfObject_dev2 failed. ret=%d", ret);
      CMSMEM_FREE_BUF_AND_NULL_PTR(lowerLayerFullPath);
      dalWan_removeFailedWanServiceByL2IfName_dev2(ifname);
      return ret;
   }
   
   CMSMEM_FREE_BUF_AND_NULL_PTR(lowerLayerFullPath);

   return ret;
}



CmsRet dalCellular2_delCellularWanService(const WEB_NTWK_VAR *webVar)
{
   Dev2CellularLinkObject *cellularLinkObj;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;   
   CmsRet ret;

   cmsLog_debug("wanIfName=%s", webVar->wanIfName);

   if ((ret = dalWan_deleteService_dev2(webVar)) !=  CMSRET_SUCCESS)
   {
      cmsLog_error("dalWan_deleteService_dev2 failed. ret=%d", ret);
      return ret;
   }

   while ((ret = cmsObj_getNext(MDMOID_DEV2_CELLULAR_LINK, &iidStack, (void **)&cellularLinkObj)) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strstr(cellularLinkObj->name,  webVar->wanIfName))
      {
         cmsLog_debug("cmsObj_deleteInstance:ifname = %s", webVar->wanIfName);
         cmsObj_deleteInstance(MDMOID_DEV2_CELLULAR_LINK, &iidStack);
         cmsObj_free((void **)&cellularLinkObj);
         break;
      }       

      cmsObj_free((void **)&cellularLinkObj);
   }
   
   return CMSRET_SUCCESS;
}

#endif

#else /* DMP_CELLULARINTERFACEBASE_1 is not defined */
#error "Cellular objects incompatible with current Data Model mode, go to make menuconfig to fix"
#endif
#endif  /* SUPPORT_CELLULAR */





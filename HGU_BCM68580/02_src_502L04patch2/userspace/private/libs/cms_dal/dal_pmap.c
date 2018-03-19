/***********************************************************************
 *
 *  Copyright (c) 2006  Broadcom Corporation
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





/* use functions defined in rut_pmap.c */
extern CmsRet rutPMap_getBridgeByKey(UINT32 bridgeKey, InstanceIdStack *iidStack, L2BridgingEntryObject **bridgeObj);
extern CmsRet rutPMap_getFilter(const char *filterInterface, InstanceIdStack *iidStack, L2BridgingFilterObject **filterObj);
extern CmsRet rutPMap_getAvailableInterfaceByRef(const char *availInterfaceReference, InstanceIdStack *iidStack, L2BridgingIntfObject **availIntfObj);
extern CmsRet rutPMap_getAvailableInterfaceByKey(UINT32 availableInterfaceKey, InstanceIdStack *iidStack, L2BridgingIntfObject **availIntfObj);
extern CmsRet rutPMap_wanIfNameToAvailableInterfaceReference(const char *wanIfName, char *availInterfaceReference);
extern CmsRet rutPMap_lanIfNameToAvailableInterfaceReference(const char *lanIfName, char *availInterfaceReference);
extern CmsRet rutPMap_availableInterfaceReferenceToIfName(const char *availableInterfaceReference, char *ifName);
extern UBOOL8 rutPMap_isWanUsedForIntfGroup(const char *ifName);
extern CmsRet rutPMap_getBridgeByName(const char *bridgeName, InstanceIdStack *iidStack, L2BridgingEntryObject **bridgeObj);
extern CmsRet rutPMap_getBridgeByKey(UINT32 bridgeKey, InstanceIdStack *iidStack, L2BridgingEntryObject **bridgeObj);
extern void rutPMap_deleteBridge(const char *bridgeName);
extern CmsRet rutPMap_getFilterDhcpVendorIdByBridgeName(const char *bridgeName, InstanceIdStack *iidStack, L2BridgingFilterObject **filterObj);
extern CmsRet rutPMap_disassocAllFilterIntfFromBridge(const char *bridgeName);
extern void rutPmap_deleteFilterDhcpVendorId(const char *bridgeName);
extern CmsRet rutPMap_getBridgeKey(const char *bridgeName, UINT32 *bridgeKey);
#ifdef SUPPORT_LANVLAN
extern CmsRet rutPMap_getFilterWithVlan(const char *filterInterface, SINT32 VLANIDFilter, InstanceIdStack *iidStack, L2BridgingFilterObject **filterObj);
#endif
   
CmsRet dalPMap_getBridgeByName(const char *bridgeName, InstanceIdStack *iidStack, L2BridgingEntryObject **bridgeObj)
{
   return (rutPMap_getBridgeByName(bridgeName, iidStack, bridgeObj));
}

CmsRet dalPMap_getBridgeByKey(UINT32 bridgeKey, InstanceIdStack *iidStack, L2BridgingEntryObject **bridgeObj)
{
   return (rutPMap_getBridgeByKey(bridgeKey, iidStack, bridgeObj));
}


CmsRet dalPMap_getBridgeKey(const char *bridgeName, UINT32 *bridgeKey)
{
   return rutPMap_getBridgeKey(bridgeName, bridgeKey);
}


CmsRet dalPMap_addBridge_igd(IntfGrpBridgeMode mode, const char *bridgeName)
{
   CmsRet                ret        = CMSRET_SUCCESS;
   InstanceIdStack       iidStack   = EMPTY_INSTANCE_ID_STACK;
   L2BridgingEntryObject *bridgeObj  = NULL;


   if ((ret = dalPMap_getBridgeByName(bridgeName, &iidStack, &bridgeObj)) == CMSRET_SUCCESS)
   {
      cmsLog_debug("Add bridge entry failed. Bridge %s already exists", bridgeName);
      cmsObj_free((void **) &bridgeObj);
      return CMSRET_INVALID_ARGUMENTS;
   }

   /* add new instance of Layer2Bridging.Bridge.{i}. */
   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_addInstance(MDMOID_L2_BRIDGING_ENTRY, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create new L2BridgingEntry, ret=%d", ret);
      return ret;
   }

   /* read it back */
   if ((ret = cmsObj_get(MDMOID_L2_BRIDGING_ENTRY, &iidStack, 0, (void **) &bridgeObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get newly created bridge, ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_L2_BRIDGING_ENTRY, &iidStack);
      return ret;
   }


   /*
    * modify params and set it into MDM.
    * bridgeKey and X_BROADCOM_COM_IfName was assigned by the rcl handler function
    * during object creation.
    */
   bridgeObj->bridgeName = cmsMem_strdup(bridgeName);
   bridgeObj->bridgeEnable          = TRUE;
   bridgeObj->X_BROADCOM_COM_Mode   = mode;
 
   cmsLog_debug("set new bridge %s with IfName br%d", bridgeName, bridgeObj->bridgeKey);

   if ((ret = cmsObj_set(bridgeObj, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set L2BridgingEntryObject, ret = %d", ret);
      cmsObj_deleteInstance(MDMOID_L2_BRIDGING_ENTRY, &iidStack);
   }

   cmsObj_free((void **) &bridgeObj);

   return ret;
}


void dalPMap_deleteBridge_igd(const char *bridgeName)
{
   return rutPMap_deleteBridge(bridgeName);
}

#ifdef SUPPORT_LANVLAN
CmsRet dalPMap_getFilterWithVlan(const char *filterInterface, SINT32 VLANIDFilter, InstanceIdStack *iidStack, L2BridgingFilterObject **filterObj)
{
   return (rutPMap_getFilterWithVlan(filterInterface, VLANIDFilter, iidStack, filterObj));
}

#endif

CmsRet dalPMap_getFilter(const char *filterInterface, InstanceIdStack *iidStack, L2BridgingFilterObject **filterObj)
{
   return (rutPMap_getFilter(filterInterface, iidStack, filterObj));
}



CmsRet dalPMap_getFilterDhcpVendorIdByBridgeName(const char *bridgeName, InstanceIdStack *iidStack, L2BridgingFilterObject **filterObj)
{
   return rutPMap_getFilterDhcpVendorIdByBridgeName(bridgeName, iidStack, filterObj);
}


CmsRet dalPMap_addFilterDhcpVendorId_igd(const char *bridgeName, const char *aggregateString)
{
   L2BridgingFilterObject *filterObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UINT32 bridgeKey=0;
   CmsRet ret;
   
   if ((ret = dalPMap_getBridgeKey(bridgeName, &bridgeKey)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cannot find bridge key for bridge %s", bridgeName);
      return ret;
   }
   
   /* check for duplicates */   
   if ((ret = dalPMap_getFilterDhcpVendorIdByBridgeName(bridgeName, &iidStack, &filterObj)) == CMSRET_SUCCESS)
   {
      cmsObj_free((void **) &filterObj);
      cmsLog_error("filter dhcp vendor id on bridge %s already exists (iidStack=%s)", bridgeName, cmsMdm_dumpIidStack(&iidStack));
      return CMSRET_INVALID_ARGUMENTS;
   }
   
   /* add new instance of Layer2Bridging.Filter.{i}. */
   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_addInstance(MDMOID_L2_BRIDGING_FILTER, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create new bridging filter interface, ret=%d", ret);
      return ret;
   }

   /* read it back */
   if ((ret = cmsObj_get(MDMOID_L2_BRIDGING_FILTER, &iidStack, 0, (void **) &filterObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get newly created filter interface, ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_L2_BRIDGING_FILTER, &iidStack);
      return ret;
   }

   /* Set dhcp vendor id parameters */
   CMSMEM_REPLACE_STRING(filterObj->filterInterface, MDMVS_LANINTERFACES);
   filterObj->filterBridgeReference = (SINT32) bridgeKey;
   CMSMEM_REPLACE_STRING(filterObj->sourceMACFromVendorClassIDFilter, aggregateString);
   filterObj->sourceMACFromVendorClassIDFilterExclude = FALSE;
   if ((ret = cmsObj_set(filterObj, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set L2BridgingEntryObject, ret = %d", ret);
      cmsObj_deleteInstance(MDMOID_L2_BRIDGING_FILTER, &iidStack);
   }

   cmsObj_free((void **) &filterObj);
   
   return ret;
}


void dalPMap_deleteFilterDhcpVendorId_igd(const char *bridgeName)
{
   rutPmap_deleteFilterDhcpVendorId(bridgeName);
}


CmsRet dalPMap_assocFilterIntfToBridge_igd(const char *ifName, const char *grpName)
{
   CmsRet                 ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack        filterIidStack = EMPTY_INSTANCE_ID_STACK;
   L2BridgingFilterObject *filterObj = NULL;
   L2BridgingIntfObject *availIntfObj=NULL;
   char filterInterface[BUFLEN_32];
   char availInterfaceReference[BUFLEN_256];
   SINT32 bridgeRef;
#ifdef SUPPORT_LANVLAN
   char *p = NULL, lanIfName[BUFLEN_32];
   UINT32 vlanId = 0;
#endif

   cmsLog_debug("Associating filter interface %s with bridge %s", ifName, grpName);
   
#ifdef SUPPORT_LANVLAN
   strncpy(lanIfName, ifName, sizeof(lanIfName));
   if (strstr(lanIfName, "eth") && (p=strchr(lanIfName, '.')) != NULL) *p = '\0';

   if ((ret = dalPMap_lanIfNameToAvailableInterfaceReference(lanIfName, availInterfaceReference)) != CMSRET_SUCCESS)
#else
   if ((ret = dalPMap_lanIfNameToAvailableInterfaceReference(ifName, availInterfaceReference)) != CMSRET_SUCCESS)
#endif   	
   {
      cmsLog_debug("could not find LAN ifName=%s, try WAN", ifName);
      if ((ret = dalPMap_wanIfNameToAvailableInterfaceReference(ifName, availInterfaceReference)) != CMSRET_SUCCESS)
      {
         cmsLog_error("could not find WAN ifName eithter=%s", ifName);
         return ret;
      }
   }
#ifdef SUPPORT_LANVLAN
   else
   {
      if (p != NULL) vlanId = atoi(p+1);
   }
#endif   	
   
   /*
    * very anoying, the TR98 spec says the fullname does not end in dot,
    * but our internal conversion routines put in the final dot, so strip
    * it out before putting into the MDM.
    */
   availInterfaceReference[strlen(availInterfaceReference)-1] = '\0';    
   if ((ret = dalPMap_getAvailableInterfaceByRef(availInterfaceReference, &iidStack, &availIntfObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not find avail intf %s", availInterfaceReference);
      return ret;
   }
   
   sprintf(filterInterface, "%u", availIntfObj->availableInterfaceKey);
   cmsObj_free((void **) &availIntfObj);

#ifdef SUPPORT_LANVLAN
   if ((ret = dalPMap_getFilterWithVlan(filterInterface, vlanId, &filterIidStack, &filterObj)) != CMSRET_SUCCESS)
#else
   if ((ret = dalPMap_getFilter(filterInterface, &filterIidStack, &filterObj)) != CMSRET_SUCCESS)
#endif
   {
      cmsLog_error("could not find filter interface %s", ifName);
      return ret;
   }


   // A special case when the grpName is NULL, it indicates that the interface
   // in the filter table must be set to not associated (i.e filterBridgeReference
   // value must be set to -1, which means not associated)
   if (grpName == NULL)
   {
      bridgeRef = -1;
   }
   else
   {
      if ((ret = dalPMap_getBridgeKey(grpName, (UINT32 *) &bridgeRef)) != CMSRET_SUCCESS)
      {
         cmsLog_error("could not find bridge %s", grpName);
         cmsObj_free((void **) &filterObj);
         return ret;
      }
   }


   /* set filter interface to new bridge ref */
   filterObj->filterBridgeReference = bridgeRef;
#ifdef SUPPORT_LANVLAN
   filterObj->X_BROADCOM_COM_VLANIDFilter = vlanId;
#endif

   if ((ret = cmsObj_set(filterObj, &filterIidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to set new filterBridgeReference, ret=%d", ret);
   }

   cmsObj_free((void **) &filterObj);

   return ret;
}


CmsRet dalPMap_disassocAllFilterIntfFromBridge_igd(const char *bridgeName)
{
   return rutPMap_disassocAllFilterIntfFromBridge(bridgeName);
}


CmsRet dalPMap_getAvailableInterfaceByRef(const char *availInterfaceReference, InstanceIdStack *iidStack, L2BridgingIntfObject **availIntfObj)
{
   return (rutPMap_getAvailableInterfaceByRef(availInterfaceReference, iidStack, availIntfObj));   
}


CmsRet dalPMap_getAvailableInterfaceByKey(UINT32 availableInterfaceKey, InstanceIdStack *iidStack, L2BridgingIntfObject **availIntfObj)
{
   return (rutPMap_getAvailableInterfaceByKey(availableInterfaceKey, iidStack, availIntfObj));   
}


CmsRet dalPMap_wanIfNameToAvailableInterfaceReference(const char *ifName, char *availInterfaceReference)
{
   return (rutPMap_wanIfNameToAvailableInterfaceReference(ifName, availInterfaceReference));
}


CmsRet dalPMap_lanIfNameToAvailableInterfaceReference(const char *ifName, char *availInterfaceReference)
{
   return (rutPMap_lanIfNameToAvailableInterfaceReference(ifName, availInterfaceReference));
}


CmsRet dalPMap_availableInterfaceReferenceToIfName(const char *availableInterfaceReference, char *ifName)
{
   return (rutPMap_availableInterfaceReferenceToIfName(availableInterfaceReference, ifName));
}


UBOOL8 dalPMap_isWanUsedForIntfGroup(const char *ifName)
{
   return(rutPMap_isWanUsedForIntfGroup(ifName));

}


CmsRet dalPMap_addBeepBridge(void)
{
   CmsRet ret;
   ret = dalPMap_addBridge(INTFGRP_BR_BEEP_SECONDARY_MODE, BEEP_NETWORKING_GROUP_SECONDARY);

   if (ret == CMSRET_SUCCESS)
   {
      ret = dalPMap_addBridge(INTFGRP_BR_BEEP_WANONLY_MODE, BEEP_NETWORKING_GROUP_WANONLY);
   }

   return ret;
}


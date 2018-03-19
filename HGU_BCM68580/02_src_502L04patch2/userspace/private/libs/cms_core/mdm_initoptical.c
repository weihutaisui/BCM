/***********************************************************************
 *
 *  Copyright (c) 2006-2008  Broadcom Corporation
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


#include "cms.h"
#include "cms_core.h"
#include "cms_util.h"
#include "mdm.h"
#include "mdm_private.h"
#include "qdm_intf.h"

extern CmsRet rutBridge_addFullPathToBridge_dev2(const char *fullPath, const char *brIntfName);

#ifdef DMP_DEVICE2_OPTICAL_1

static CmsRet mdm_addOpticalInterfaceObject(const char *ifName)
{
    CmsRet ret = CMSRET_SUCCESS;
    MdmPathDescriptor  pathDesc;
    DeviceOpticalObject *mdmObjDev = NULL;
    OpticalInterfaceObject *mdmObj = NULL;

    INIT_PATH_DESCRIPTOR(&pathDesc);
    pathDesc.oid = MDMOID_OPTICAL_INTERFACE;

    if ((ret = mdm_addObjectInstance(&pathDesc, NULL, NULL)) != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to add OpticalInterfaceObject, ret=%d", ret);
        return ret;
    }

    if ((ret = mdm_getObject(pathDesc.oid, &pathDesc.iidStack, (void **) &mdmObj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get OpticalInterfaceObject, ret=%d", ret);
        return ret;
    }

    CMSMEM_REPLACE_STRING_FLAGS(mdmObj->status, MDMVS_DOWN, mdmLibCtx.allocFlags);
    CMSMEM_REPLACE_STRING_FLAGS(mdmObj->name, ifName, mdmLibCtx.allocFlags);
    mdmObj->upstream = TRUE;
#if (defined(DMP_X_BROADCOM_COM_EPONWAN_1) && defined(EPON_SFU) || defined(DMP_X_BROADCOM_COM_GPONWAN_1) && defined(GPON_SFU))
    mdmObj->enable = TRUE;
#endif

    ret = mdm_setObject((void **)&mdmObj, &pathDesc.iidStack, FALSE);
    mdm_freeObject((void **) &mdmObj);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to set OpticalInterfaceObject. ret=%d", ret);
        return ret;
    }

    INIT_PATH_DESCRIPTOR(&pathDesc);
    pathDesc.oid = MDMOID_DEVICE_OPTICAL;

    if ((ret = mdm_getObject(pathDesc.oid, &pathDesc.iidStack, (void **) &mdmObjDev)) != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get DeviceOpticalObject, ret=%d", ret);
        return ret;
    }

    mdmObjDev->interfaceNumberOfEntries++;
    ret = mdm_setObject((void **)&mdmObjDev, &pathDesc.iidStack, FALSE);
    mdm_freeObject((void **) &mdmObjDev);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to set DeviceOpticalObject. ret=%d", ret);
        return ret;
    }

    return ret;
}

static int mdm_isOpticalInterfaceObjectExists(const char *ifName)
{
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    OpticalInterfaceObject *mdmObj = NULL;
    int found = 0;

    while (!found && mdm_getNextObject(MDMOID_OPTICAL_INTERFACE, &iidStack, (void **) &mdmObj) == CMSRET_SUCCESS)
    {
        found = (cmsUtl_strcmp(mdmObj->name, ifName) == 0);
        mdm_freeObject((void **) &mdmObj);
    }

    return found;
}

#ifdef DMP_X_BROADCOM_COM_EPONWAN_1   
#ifdef EPON_SFU
static CmsRet addEthernetLink(const char *lowerLayer, char *myPathRef, UINT32 bufLen)
{
   char *fullPathStringPtr = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;   
   Dev2EthernetLinkObject *ethLinkObj = NULL;
   MdmPathDescriptor ethLinkPathDesc;
   CmsRet ret;

   cmsLog_debug("lowerLayer %s", lowerLayer);
   
   /* Create an Ethernet link object */
   if ((ret = cmsObj_addInstance(MDMOID_DEV2_ETHERNET_LINK, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to add ethLink Instance, ret = %d", ret);
      return ret;
   } 

   if ((ret = cmsObj_get(MDMOID_DEV2_ETHERNET_LINK, &iidStack, 0, (void **) &ethLinkObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get ethLinkObj, ret = %d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_ETHERNET_LINK, &iidStack);
      return ret;
   }

   CMSMEM_REPLACE_STRING(ethLinkObj->lowerLayers, lowerLayer);
   ethLinkObj->enable = TRUE;

   /* MacAddress need to be set when the layer 2 link is up in rcl/rut */
   ret =  cmsObj_set(ethLinkObj, &iidStack);
   cmsObj_free((void **) &ethLinkObj); 

   if (ret  != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set ethLinkObj. ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_ETHERNET_LINK, &iidStack);
      return ret;
   } 

   /* Get the Ethernet Link object full path string to be used as ip interface lower layer */
   INIT_PATH_DESCRIPTOR(&ethLinkPathDesc);
   ethLinkPathDesc.iidStack = iidStack;
   ethLinkPathDesc.oid = MDMOID_DEV2_ETHERNET_LINK;
   if ((ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&ethLinkPathDesc, &fullPathStringPtr)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_pathDescriptorToFullPath returns error. ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_ETHERNET_LINK, &iidStack);
      return ret;
   }
   if (cmsUtl_strlen(fullPathStringPtr) > (SINT32) bufLen)
   {
      ret = CMSRET_INTERNAL_ERROR;
   }
   else
   {
      cmsUtl_strncpy(myPathRef, fullPathStringPtr, bufLen);
      ret = CMSRET_SUCCESS;
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(fullPathStringPtr);

   cmsLog_debug("Exit, ret=%d", ret);

   return ret;

}

static CmsRet addEthernetVlanTermination(const char *lowerLayer, char *myPathRef, UINT32 bufLen)
{
   char *fullPathStringPtr = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;   
   Dev2VlanTerminationObject *ethVlanObj = NULL;
   MdmPathDescriptor ethVlanTerminationPathDesc;
   CmsRet ret;

   cmsLog_debug("lowerLayer %s", lowerLayer);
   
   /* Create an Ethernet VlanTermination object */
   if ((ret = cmsObj_addInstance(MDMOID_DEV2_VLAN_TERMINATION, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to add VLAN_TERMINATION Instance, ret = %d", ret);
      return ret;
   } 

   if ((ret = cmsObj_get(MDMOID_DEV2_VLAN_TERMINATION, &iidStack, 0, (void **) &ethVlanObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get VLAN_TERMINATION object, ret = %d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_VLAN_TERMINATION, &iidStack);
      return ret;
   }

   CMSMEM_REPLACE_STRING(ethVlanObj->lowerLayers, lowerLayer);
   ethVlanObj->VLANID = -1;
   ethVlanObj->X_BROADCOM_COM_Vlan8021p = -1;
   ethVlanObj->TPID = -1;
   ethVlanObj->enable = TRUE;

   ret =  cmsObj_set(ethVlanObj, &iidStack);
   cmsObj_free((void **) &ethVlanObj); 

   if (ret  != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set ethVlanObj. ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_VLAN_TERMINATION, &iidStack);
      return ret;
   } 

   /* Get the Ethernet VlanTermination object full path string to be used as ip interface lower layer */
   INIT_PATH_DESCRIPTOR(&ethVlanTerminationPathDesc);
   ethVlanTerminationPathDesc.iidStack = iidStack;
   ethVlanTerminationPathDesc.oid = MDMOID_DEV2_VLAN_TERMINATION;
   if ((ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&ethVlanTerminationPathDesc, &fullPathStringPtr)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_pathDescriptorToFullPath returns error. ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_VLAN_TERMINATION, &iidStack);
      return ret;
   }
   if (cmsUtl_strlen(fullPathStringPtr) > (SINT32) bufLen)
   {
      ret = CMSRET_INTERNAL_ERROR;
   }
   else
   {
      cmsUtl_strncpy(myPathRef, fullPathStringPtr, bufLen);
      ret = CMSRET_SUCCESS;
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(fullPathStringPtr);


   cmsLog_debug("Exit, ret=%d", ret);

   return ret;

}

static CmsRet addIntfObject(const char *lowerLayer)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2IpInterfaceObject *ipIntf = NULL;
   CmsRet ret;
   
   cmsLog_debug("Enter: lowerLayer=%s", lowerLayer);

   if ((ret = cmsObj_addInstance(MDMOID_DEV2_IP_INTERFACE, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to add IP Instance, ret = %d", ret);
      return ret;
   } 

   if ((ret = cmsObj_get(MDMOID_DEV2_IP_INTERFACE, &iidStack, 0, (void **) &ipIntf)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get IP interface object, ret = %d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_IP_INTERFACE, &iidStack);
      return ret;
   }
   
   ipIntf->enable = TRUE;
   ipIntf->IPv4Enable = TRUE;
   CMSMEM_REPLACE_STRING(ipIntf->lowerLayers, lowerLayer);

   ipIntf->X_BROADCOM_COM_BridgeService = TRUE;
   CMSMEM_REPLACE_STRING(ipIntf->X_BROADCOM_COM_BridgeName, "br0");

#ifdef DMP_X_BROADCOM_COM_IGMP_1
   ipIntf->X_BROADCOM_COM_IGMPEnabled = TRUE;
   ipIntf->X_BROADCOM_COM_IGMP_SOURCEEnabled = TRUE;
#endif

#ifdef DMP_X_BROADCOM_COM_MLD_1
   ipIntf->X_BROADCOM_COM_MLDEnabled = TRUE;
   ipIntf->X_BROADCOM_COM_MLD_SOURCEEnabled = TRUE;
#endif

   ret = cmsObj_set(ipIntf, &iidStack);
   cmsObj_free((void **) &ipIntf);
   
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set ipIntf. ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_IP_INTERFACE, &iidStack);
      return ret;      
   } 

   cmsLog_debug("Exit: ret=%d", ret);

   return ret;
}

static CmsRet addBridgeWanService(const char *wanL2IfName)
{
    CmsRet ret = CMSRET_SUCCESS;
    UBOOL8 layer2 = TRUE;
    char *layer2FullPath = NULL;
    char ethLinkPathRef[MDM_SINGLE_FULLPATH_BUFLEN] = { 0 };
    char ethVlanPathRef[MDM_SINGLE_FULLPATH_BUFLEN] = { 0 };

    /* First get the fullpath to the layer 2 interface name */
    if ((ret = qdmIntf_intfnameToFullPathLocked(wanL2IfName, layer2, &layer2FullPath)) != CMSRET_SUCCESS)
    {
        cmsLog_error("failed to get fullpath of %s, ret=%d", wanL2IfName, ret);
        return ret;
    }
    cmsLog_debug("wanL2IfName %s ==> L2FullPath %s", wanL2IfName, layer2FullPath);

    /* Create Ethernet.Link object */
    ret = addEthernetLink(layer2FullPath, ethLinkPathRef, sizeof(ethLinkPathRef));
    CMSMEM_FREE_BUF_AND_NULL_PTR(layer2FullPath);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("addEthernetLink failed, ret=%d", ret);
        return ret;      
    }

    /* Create Ethernet.VlanTermination object */
    ret = addEthernetVlanTermination(ethLinkPathRef, ethVlanPathRef, sizeof(ethVlanPathRef));
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("addEthernetVlanTermination failed, ret=%d", ret);
        return ret;      
    }

    /* Create IP interface object */
    ret = addIntfObject(ethVlanPathRef);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("addIntfObject failed. ret=%d", ret);
        return ret;
    }

    /* Add a bridge port and join br0 */
    ret = rutBridge_addFullPathToBridge_dev2(ethVlanPathRef, "br0");

    return ret;
}
#endif
#endif

CmsRet mdm_addDefaultOpticalObject(void)
{
    CmsRet ret = CMSRET_SUCCESS;

#ifdef DMP_X_BROADCOM_COM_EPONWAN_1
    if (!mdm_isOpticalInterfaceObjectExists(EPON_WAN_IF_NAME))
    {
        if ((ret = mdm_addOpticalInterfaceObject(EPON_WAN_IF_NAME)) != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to add EPON optical interface %s, ret=%d", EPON_WAN_IF_NAME, ret);
            goto Error;
        }
#ifdef EPON_SFU
        if ((ret = addBridgeWanService(EPON_WAN_IF_NAME)) != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to add WAN service for %s, ret=%d", EPON_WAN_IF_NAME, ret);
            goto Error;
        }
#endif
    }
#endif

#ifdef DMP_X_BROADCOM_COM_GPONWAN_1
    if (!mdm_isOpticalInterfaceObjectExists(GPON_WAN_IF_NAME))
    {
        if ((ret = mdm_addOpticalInterfaceObject(GPON_WAN_IF_NAME)) != CMSRET_SUCCESS)
        {
            cmsLog_error("Failed to add GPON optical interface %s, ret=%d", GPON_WAN_IF_NAME, ret);
            goto Error;
        }
    }
#endif

Error:
    return ret;
}

#endif /* DMP_DEVICE2_OPTICAL_1 */




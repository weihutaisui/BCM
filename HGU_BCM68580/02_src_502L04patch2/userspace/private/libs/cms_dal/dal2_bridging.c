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


#ifdef DMP_DEVICE2_BASELINE_1

#ifdef DMP_DEVICE2_BRIDGE_1

#include "cms_core.h"
#include "qdm_intf.h"
#include "cms_util.h"

#include "cms_dal.h"
#include "dal.h"
#include "dal2_wan.h"
#include "qdm_ipintf.h"




/* in rut2_bridging.c */
extern CmsRet rutBridge_addIntfNameToBridge_dev2(const char *intfName, const char *brIntfName);
extern CmsRet rutBridge_addFullPathToBridge_dev2(const char *fullPath, const char *brIntfName);
extern void rutBridge_deleteIntfNameFromBridge_dev2(const char *intfName);


CmsRet dalBridge_addIntfNameToBridge_dev2(const char *intfName, const char *brIntfName)
{
   return (rutBridge_addIntfNameToBridge_dev2(intfName, brIntfName));
}


CmsRet dalBridge_addFullPathToBridge_dev2(const char *fullPath, const char *brIntfName)
{
   return (rutBridge_addFullPathToBridge_dev2(fullPath, brIntfName));
}


void dalBridge_deleteIntfNameFromBridge_dev2(const char *intfName)
{
   rutBridge_deleteIntfNameFromBridge_dev2(intfName);
}


CmsRet dalBridge_addBridge_dev2(IntfGrpBridgeMode mode, const char *groupName)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2BridgeObject *brObj=NULL;
   Dev2BridgePortObject *brPortObj=NULL;
   char *brPortFullPath=NULL;
   char ethLinkFullPathBuf[MDM_SINGLE_FULLPATH_BUFLEN]={0};
   char ipIntfFullPathBuf[MDM_SINGLE_FULLPATH_BUFLEN]={0};
   MdmPathDescriptor ipIntfPathDesc=EMPTY_PATH_DESCRIPTOR;
   char ipAddrBuf[CMS_IPADDR_LENGTH]={0};
   char minAddrBuf[CMS_IPADDR_LENGTH]={0};
   char maxAddrBuf[CMS_IPADDR_LENGTH]={0};
   char subnetBuf[CMS_IPADDR_LENGTH]={0};
   UINT32 bridgeNum;
   CmsRet ret;
   char brIfName[CMS_IFNAME_LENGTH]={0};

   cmsLog_debug("Entered: intf groupName=%s", groupName);

   dalLan_getBridgeIfNameFromBridgeName_dev2(groupName, brIfName);
   if (!IS_EMPTY_STRING(brIfName))
   {
      cmsLog_notice("Add bridge entry failed. groupName %s already exists", groupName);
      return CMSRET_INVALID_ARGUMENTS;
   }

   /* Add Bridge.{i}. and enable it */
   if ((ret = cmsObj_addInstance(MDMOID_DEV2_BRIDGE, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create new Bridge, ret=%d", ret);
      return ret;
   }

   if ((ret = cmsObj_get(MDMOID_DEV2_BRIDGE, &iidStack, 0, (void **)&brObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get new Bridge obj, ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_BRIDGE, &iidStack);
      return ret;
   }

   brObj->enable = TRUE;
   brObj->X_BROADCOM_COM_Mode = mode;

   ret = cmsObj_set(brObj, &iidStack);
   cmsObj_free((void **) &brObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not enable Bridge obj, ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_BRIDGE, &iidStack);
      return ret;
   }

   /*
    * Once we enable the bridge, the RCL handler will create a brx interface.
    * Get the object again so we know what x is.
    */
   if ((ret = cmsObj_get(MDMOID_DEV2_BRIDGE, &iidStack, 0, (void **)&brObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get new Bridge obj, ret=%d", ret);
      return ret;
   }

   bridgeNum = atoi(&(brObj->X_BROADCOM_COM_IfName[2]));
   cmsLog_debug("bridgeName=%s bridgeNum=%d",
                brObj->X_BROADCOM_COM_IfName, bridgeNum);

   cmsObj_free((void **) &brObj);


   /* Add Bridge.{i}.Port.{i} management port */
   if ((ret = cmsObj_addInstance(MDMOID_DEV2_BRIDGE_PORT, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create new Bridge Port, ret=%d", ret);
      return ret;
   }

   if ((ret = cmsObj_get(MDMOID_DEV2_BRIDGE_PORT, &iidStack, 0, (void **)&brPortObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get new Bridge Port obj, ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_BRIDGE_PORT, &iidStack);
      return ret;
   }

   brPortObj->enable = TRUE;
   brPortObj->managementPort = TRUE;

   ret = cmsObj_set(brPortObj, &iidStack);
   cmsObj_free((void **) &brPortObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not enable Bridge obj, ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_BRIDGE_PORT, &iidStack);
      return ret;
   }

   /* make fullpath to bridge port, which is the lowerlayers of Eth.Link */
   {
      MdmPathDescriptor brPortPathDesc=EMPTY_PATH_DESCRIPTOR;
      brPortPathDesc.oid = MDMOID_DEV2_BRIDGE_PORT;
      brPortPathDesc.iidStack = iidStack;
      cmsMdm_pathDescriptorToFullPathNoEndDot(&brPortPathDesc, &brPortFullPath);
   }


   /* Add Ethernet.Link which points down to management port */
   ret = dalEth_addEthernetLink_dev2(brPortFullPath,
                              ethLinkFullPathBuf, sizeof(ethLinkFullPathBuf));

   CMSMEM_FREE_BUF_AND_NULL_PTR(brPortFullPath);

   /* Add IP.Interface which points down to Ethernet.Link */
   {
      UBOOL8 supportIpv4=TRUE;
#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
      UBOOL8 supportIpv6 = TRUE;
#else
      UBOOL8 supportIpv6=FALSE;
#endif

      ret = dalIp_addIntfObject_dev2(supportIpv4,
                                     supportIpv6,
                                     groupName,
                                     FALSE, NULL,  /* isWanBridgeSerivce, bridgeName */
                                     FALSE,  /* firewall XXX TODO: what about LAN side firewall? */
                                     FALSE, FALSE, /* igmp, igmpSource: XXX TODO: what about snooping? */
                                     FALSE, FALSE, /* mld, mldSource */
                                     ethLinkFullPathBuf,
                                     ipIntfFullPathBuf,
                                     sizeof(ipIntfFullPathBuf),
                                     &ipIntfPathDesc);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("dalIp_addIntfObj_dev2 failed, ret=%d", ret);
      }
   }

   /*
    * Now add some default IPv4 addresses for the newly created bridge, this
    * must be done after the IP.Interface is created.
    * use same hardcoded values and algorithm as rutLan_addBridge
    */
   sprintf(ipAddrBuf, "192.168.%d.1", bridgeNum+1);
   sprintf(subnetBuf, "255.255.255.0");
   dalIp_addIpIntfIpv4Address_dev2(&ipIntfPathDesc.iidStack,
                                   ipAddrBuf, subnetBuf);


   /*
    * Also tell dhcpd to start serving on this subnet.
    */
   sprintf(minAddrBuf, "192.168.%d.2", bridgeNum+1);
   sprintf(maxAddrBuf, "192.168.%d.254", bridgeNum+1);
   dalLan_addDhcpdSubnet_dev2(ipIntfFullPathBuf,
                              ipAddrBuf, minAddrBuf, maxAddrBuf, subnetBuf);

   cmsLog_debug("Exit: ret=%d", ret);

   return ret;
}


void dalBridge_deleteBridge_dev2(const char *groupName)
{
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   InstanceIdStack ipIntfIidStack = EMPTY_INSTANCE_ID_STACK;
   char brIntfName[CMS_IFNAME_LENGTH]={0};
   UBOOL8 found=FALSE;
   CmsRet ret;

   cmsLog_debug("Entered: intfGroup=%s", groupName);

   /*
    * First find the right IP.Interface object for the groupName.
    */
   while (!found &&
          (ret = cmsObj_getNextFlags(MDMOID_DEV2_IP_INTERFACE, &ipIntfIidStack,
                               OGF_NO_VALUE_UPDATE,
                               (void **)&ipIntfObj)) == CMSRET_SUCCESS)
   {
      if (!ipIntfObj->X_BROADCOM_COM_Upstream &&
          !cmsUtl_strcmp(ipIntfObj->X_BROADCOM_COM_GroupName, groupName))
      {
         found = TRUE;
         strcpy(brIntfName, ipIntfObj->name);
      }

      cmsObj_free((void **)&ipIntfObj);
   }

   if (!found)
   {
      cmsLog_error("Could not find groupName %s", groupName);
      return;
   }


   /* Tell dhcpd not to service this interface anymore */
   {
      char *ipIntfFullPath=NULL;
      MdmPathDescriptor pathDesc=EMPTY_PATH_DESCRIPTOR;
      pathDesc.oid = MDMOID_DEV2_IP_INTERFACE;
      pathDesc.iidStack = ipIntfIidStack;
      cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &ipIntfFullPath);

      dalLan_deleteDhcpdSubnet_dev2(ipIntfFullPath);
      CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);
   }


   /* delete the IP.Interface obj */
   ret = cmsObj_deleteInstance(MDMOID_DEV2_IP_INTERFACE, &ipIntfIidStack);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Delete of IP.Interface failed, ret=%d", ret);
      /* complain but keep going */
   }

  /* delete Ethernet.Link which points down to management port */
   ret = dalEth_deleteEthernetLinkByName_dev2(brIntfName);

   /*
    * Find the Device.Bridge object matching the linux ifName (e.g. br1).
    * Once we find it, we can delete the entire sub-tree.
    */
   cmsLog_debug("find %s and delete sub-tree", brIntfName);
   found = FALSE;
   {
      InstanceIdStack brIidStack = EMPTY_INSTANCE_ID_STACK;
      Dev2BridgeObject *brObj=NULL;

      while (!found &&
             (ret = cmsObj_getNextFlags(MDMOID_DEV2_BRIDGE, &brIidStack,
                                        OGF_NO_VALUE_UPDATE,
                                        (void **) &brObj) == CMSRET_SUCCESS))
      {
         if (!cmsUtl_strcmp(brObj->X_BROADCOM_COM_IfName, brIntfName))
         {
            found = TRUE;
			
            ret = cmsObj_deleteInstance(MDMOID_DEV2_BRIDGE, &brIidStack);
            if (ret != CMSRET_SUCCESS)
            {
               cmsLog_error("Could not delete Bridge object, r2=%d", ret);
            }
         }
         cmsObj_free((void **) &brObj);
      }
   }

   if (!found)
   {
      cmsLog_error("Could not find Bridge %s to delete", brIntfName);
   }

   cmsLog_debug("Exit:");
   return;
}


CmsRet dalBridge_addFilterDhcpVendorId_dev2(const char *groupName, const char *aggregateString)
{
   cmsLog_debug("Entered: groupName=%s aggreg=%s", groupName, aggregateString);

   /* XXX TODO: when the DHCP vendorId field is added to IP.Interface obj,
     * just add this string to the field.
     *
     */

   return CMSRET_SUCCESS;
}


void dalBridge_deleteFilterDhcpVendorId_dev2(const char *bridgeName)
{
   cmsLog_debug("Entered: bridgeName=%s", bridgeName);

   /* XXX TODO: when the DHCP vendorId field is added to IP.Interface obj,
    * just zero it out.
    * Should i also move all the interfaces that were moved to do DHCP
    * vendor ID?  too much work....
    */

   return;
}


static UBOOL8 getWanIpInterfaceObjByIfName(const char *ifName,
                                           Dev2IpInterfaceObject **ipIntfObj,
                                           InstanceIdStack *iidStack)
{
   UBOOL8 found=FALSE;

   while (!found &&
          cmsObj_getNextFlags(MDMOID_DEV2_IP_INTERFACE, iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **) ipIntfObj) == CMSRET_SUCCESS)
   {
      if ((*ipIntfObj)->X_BROADCOM_COM_Upstream &&
          !cmsUtl_strcmp((*ipIntfObj)->name, ifName))
      {
         found = TRUE;
         /* don't free obj, pass back to caller who is responsible for free */
      }
      else
      {
         cmsObj_free((void **) ipIntfObj);
      }
   }

   return found;
}


CmsRet dalBridge_assocFilterIntfToBridge_dev2(const char *ifName, const char *grpName)
{
   char brIntfName[CMS_IFNAME_LENGTH]={0};
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   InstanceIdStack ipIntfIidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 foundWan=FALSE;
   UBOOL8 isBridgeService=FALSE;
   CmsRet ret=CMSRET_SUCCESS;

   cmsLog_debug("Entered: ifName=%s intfgroup=%s", ifName, grpName);


   /* convert grpName to brIntfName via QDM */
   ret = qdmIpIntf_getBridgeIntfNameByGroupNameLocked_dev2(grpName, brIntfName);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not convert groupName %s to intfName", grpName);
      return ret;
   }

   /* See if this intfName is a WAN interface, we will need to update the
    * X_BROADCOM_COM_BridgeName as part of this move
    */
   foundWan = getWanIpInterfaceObjByIfName(ifName, &ipIntfObj, &ipIntfIidStack);
   if (foundWan)
   {
      CmsRet r2;

      CMSMEM_REPLACE_STRING(ipIntfObj->X_BROADCOM_COM_BridgeName, brIntfName);
      CMSMEM_REPLACE_STRING(ipIntfObj->X_BROADCOM_COM_GroupName, grpName);
      r2 = cmsObj_set((void *) ipIntfObj, &ipIntfIidStack);
      if (r2 != CMSRET_SUCCESS)
      {
         cmsLog_error("Setting new bridgeName failed, r2=%d", r2);
      }
      isBridgeService = ipIntfObj->X_BROADCOM_COM_BridgeService;
      cmsObj_free((void **) &ipIntfObj);
   }

   if (!foundWan || isBridgeService)   
   {
      /* remove this interface from its current bridge */
      dalBridge_deleteIntfNameFromBridge_dev2(ifName);
	  
      /* Add this interface to its new bridge. */
      ret = dalBridge_addIntfNameToBridge_dev2(ifName, brIntfName);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("AddIntfNameToBridge failed, ret=%d", ret);
         return ret;
      }
   }

   cmsLog_debug("Exit: ret=%d", ret);
   return ret;
}


CmsRet dalBridge_disassocAllFilterIntfFromBridge_dev2(const char *grpName)
{
   char brIntfName[CMS_IFNAME_LENGTH]={0};
   Dev2BridgeObject *brObj=NULL;
   Dev2BridgePortObject *brPortObj=NULL;
   InstanceIdStack brIidStack=EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack brPortIidStack=EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack brPortIidStack2=EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found=FALSE;
   CmsRet ret=CMSRET_SUCCESS;
   char wanIntfName[CMS_IFNAME_LENGTH]={0};
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   InstanceIdStack ipIntfIidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet r2;

   cmsLog_debug("Entered: grpName=%s", grpName);

   /* convert grpName to brIntfName via QDM */
   ret = qdmIpIntf_getBridgeIntfNameByGroupNameLocked_dev2(grpName, brIntfName);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not convert groupName %s to intfName", grpName);
      return ret;
   }


   /* Find bridge with matching intf name */
   while (!found &&
          (ret = cmsObj_getNextFlags(MDMOID_DEV2_BRIDGE, &brIidStack,
                                     OGF_NO_VALUE_UPDATE,
                                     (void **) &brObj) == CMSRET_SUCCESS))
   {
      if (!cmsUtl_strcmp(brObj->X_BROADCOM_COM_IfName, brIntfName))
      {
         found = TRUE;
      }
      /* we don't need the bridge object, so always free it */
      cmsObj_free((void **) &brObj);
   }

   if (!found)
   {
      cmsLog_error("Could not find bridge %s", brIntfName);
      return ret;
   }


   /*
    * Move all the interfaces back to default bridge br0
    */
   INIT_INSTANCE_ID_STACK(&brPortIidStack);
   while (cmsObj_getNextInSubTreeFlags(MDMOID_DEV2_BRIDGE_PORT,
                                       &brIidStack,
                                       &brPortIidStack,
                                       OGF_NO_VALUE_UPDATE,
                                       (void **) &brPortObj) == CMSRET_SUCCESS)
   {
      if (!brPortObj->managementPort)
      {
         cmsLog_debug("Move interface %s to br0", brPortObj->name);
 
         cmsObj_deleteInstance(MDMOID_DEV2_BRIDGE_PORT, &brPortIidStack);
         ret = dalBridge_addIntfNameToBridge_dev2(brPortObj->name, "br0");
		 
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("AddIntfNameToBridge failed, ret=%d", ret);
         }
		 
         brPortIidStack = brPortIidStack2;
      }
      else
      {
         brPortIidStack2 = brPortIidStack;
      }
      cmsObj_free((void **) &brPortObj);
   }

   qdmIpIntf_getWanIntfNameByGroupNameLocked_dev2(grpName, wanIntfName);
  
   if (getWanIpInterfaceObjByIfName(wanIntfName, &ipIntfObj, &ipIntfIidStack))
   {
      cmsLog_debug("cmsObj_set ipIntfObj %s to default group", ipIntfObj->name);
  
      if (ipIntfObj->X_BROADCOM_COM_BridgeService)
      {
         CMSMEM_REPLACE_STRING(ipIntfObj->X_BROADCOM_COM_BridgeName, "br0");
      }
      else
      {
         CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfObj->X_BROADCOM_COM_BridgeName);
      }
  	
      CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfObj->X_BROADCOM_COM_GroupName);
  	
      r2 = cmsObj_set((void *) ipIntfObj, &ipIntfIidStack);
      if (r2 != CMSRET_SUCCESS)
      {
         cmsLog_error("Set of IP.Interface bridgeName failed, r2=%d", r2);
      }
	  
      cmsObj_free((void **) &ipIntfObj);
   }
		 
   cmsLog_debug("Exit: ret=%d", ret);
   return ret;
}

#endif  /* DMP_DEVICE2_BRIDGE_1 */

#endif  /* DMP_DEVICE2_BASELINE_1 */


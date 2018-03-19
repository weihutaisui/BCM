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


#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"
#include "dal.h"
#include "qdm_intf.h"

#ifdef DMP_DEVICE2_BRIDGE_1
extern CmsRet dalBridge_addIntfNameToBridge_dev2(const char *intfName, const char *brIntfName);
extern void dalBridge_deleteIntfNameFromBridge_dev2(const char *intfName);
#endif

extern CmsRet rutLan_setLanIPv4Info_dev2
   (const char *ifName, const char *addr, const char *subnetmask);

void dalLan_getFirstIntfGroupName_dev2(char *groupName)
{
   CmsRet ret = CMSRET_INVALID_ARGUMENTS;
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   if (groupName == NULL)
   {
      cmsLog_error("Group name is NULL");
      return;
   }

   if ((ret = cmsObj_getNextFlags(MDMOID_DEV2_IP_INTERFACE, &iidStack, OGF_NO_VALUE_UPDATE,
                                  (void **)&ipIntfObj))== CMSRET_SUCCESS)
   {
      cmsUtl_strncpy(groupName, ipIntfObj->X_BROADCOM_COM_GroupName, CMS_IFNAME_LENGTH);
      cmsObj_free((void **) &ipIntfObj);
   }
   else
   {
      cmsLog_error("Could not find first IP Intf object, ret=%d", ret);
   }
}

void dalLan_getDhcpServerInfo_dev2
   (const char *ifName, SINT32 *enabled, char *minAddress,
    char *maxAddress, SINT32 *leaseTime, char *relaySrv)
{
   char ifNameBuf[CMS_IFNAME_LENGTH]={0};
   UBOOL8 found=FALSE;
   Dev2Dhcpv4ServerPoolObject *dhcpv4ServerPoolObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   IF_PTR_NOT_NULL_TERMINATE(minAddress);
   IF_PTR_NOT_NULL_TERMINATE(maxAddress);
   IF_PTR_NOT_NULL_TERMINATE(relaySrv);


   while (found == FALSE &&
          cmsObj_getNext(MDMOID_DEV2_DHCPV4_SERVER_POOL, &iidStack, (void **)&dhcpv4ServerPoolObj) == CMSRET_SUCCESS)
   {
      if (qdmIntf_fullPathToIntfnameLocked_dev2(dhcpv4ServerPoolObj->interface, ifNameBuf) == CMSRET_SUCCESS &&
          cmsUtl_strcmp(ifName, ifNameBuf) == 0)
      {
         found = TRUE;

         if ( !IS_EMPTY_STRING(dhcpv4ServerPoolObj->minAddress) && minAddress != NULL)
         {
            cmsUtl_strncpy(minAddress, dhcpv4ServerPoolObj->minAddress, CMS_IPADDR_LENGTH);
         }

         if ( !IS_EMPTY_STRING(dhcpv4ServerPoolObj->maxAddress) && maxAddress != NULL)
         {
            cmsUtl_strncpy(maxAddress, dhcpv4ServerPoolObj->maxAddress, CMS_IPADDR_LENGTH);
         }

         /* first set to 0 or 1 based on whether the server is enabled or not */
         if (enabled != NULL)
         {
            *enabled = (SINT32) dhcpv4ServerPoolObj->enable;
         }

#ifdef DMP_DEVICE2_DHCPV4RELAY_1
         {
            UBOOL8 relayEnabled=FALSE;

            qdmDhcpv4Relay_getInfo_dev2(dhcpv4ServerPoolObj->interface, &relayEnabled, relaySrv);
            /* translate from MDM Enabled to webVar's encoding for relay enabled (2) */
            if (enabled != NULL && relayEnabled)
            {
               *enabled = 2;
            }
         }
#endif /* DMP_DEVICE2_DHCPV4RELAY_1 */

         if (leaseTime != NULL)
         {
            *leaseTime = dhcpv4ServerPoolObj->leaseTime / (60 * 60); /* MDM in seconds, webVar in hours */
         }
      }

      cmsObj_free((void **)&dhcpv4ServerPoolObj);
   }
}


CmsRet dalLan_setLanDevice_dev2(const WEB_NTWK_VAR *webVar)
{
   CmsRet ret = CMSRET_OBJECT_NOT_FOUND;
   char bridgeIfName[CMS_IFNAME_LENGTH], ifName[CMS_IFNAME_LENGTH];
   char buff[BUFLEN_64];
   UBOOL8 found = FALSE;
   Dev2Dhcpv4ServerPoolObject *dhcpv4ServerPoolObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   MdmPathDescriptor pathDesc;
   Dev2IpInterfaceObject *ipIntfObj=NULL;

   IF_PTR_NOT_NULL_TERMINATE(bridgeIfName);
   IF_PTR_NOT_NULL_TERMINATE(ifName);
   IF_PTR_NOT_NULL_TERMINATE(buff);

   dalLan_getBridgeIfNameFromBridgeName_dev2(webVar->brName, bridgeIfName);

   /* Firewall */
   if(qdmIntf_getPathDescFromIntfnameLocked_dev2(bridgeIfName,FALSE,&pathDesc)==CMSRET_SUCCESS)
   {
      if(cmsObj_get(pathDesc.oid,&pathDesc.iidStack,OGF_NORMAL_UPDATE,(void *)&ipIntfObj)==CMSRET_SUCCESS)
      {
         ipIntfObj->X_BROADCOM_COM_FirewallEnabled = webVar->enblLanFirewall;			
         cmsObj_set(ipIntfObj,&pathDesc.iidStack);			
         cmsObj_free((void **)&ipIntfObj);
      }
   }

   /* Per-subnet information */
   while (found == FALSE &&
          cmsObj_getNext(MDMOID_DEV2_DHCPV4_SERVER_POOL, &iidStack,
                         (void **)&dhcpv4ServerPoolObj) == CMSRET_SUCCESS)
   {
      if (qdmIntf_fullPathToIntfnameLocked_dev2(dhcpv4ServerPoolObj->interface, ifName) == CMSRET_SUCCESS &&
          cmsUtl_strcmp(ifName, bridgeIfName) == 0)
      {
         found = TRUE;

         if (webVar->enblDhcpSrv == 1)
         {
            dhcpv4ServerPoolObj->enable = TRUE;

            /* update dns server info, comman seperated dns list */
            snprintf(buff, sizeof(buff), "%s,%s", webVar->dnsPrimary, webVar->dnsSecondary);
            CMSMEM_REPLACE_STRING(dhcpv4ServerPoolObj->DNSServers, buff);

            CMSMEM_REPLACE_STRING(dhcpv4ServerPoolObj->minAddress, webVar->dhcpEthStart);

            CMSMEM_REPLACE_STRING(dhcpv4ServerPoolObj->maxAddress, webVar->dhcpEthEnd);   

            CMSMEM_REPLACE_STRING(dhcpv4ServerPoolObj->subnetMask, webVar->ethSubnetMask);

            CMSMEM_REPLACE_STRING(dhcpv4ServerPoolObj->IPRouters, webVar->ethIpAddress);
            
            dhcpv4ServerPoolObj->leaseTime = webVar->dhcpLeasedTime * 60 * 60; /* webVar is in hours, MDM in seconds */

#ifdef DMP_DEVICE2_DHCPV4RELAY_1
            /* Disable DHCP Relay */
            dalDhcpv4Relay_set_dev2(dhcpv4ServerPoolObj->interface, FALSE, NULL);
#endif
         }
#ifdef DMP_DEVICE2_DHCPV4RELAY_1
         else if (webVar->enblDhcpSrv == 2)
         {
            dhcpv4ServerPoolObj->enable = FALSE;
            dalDhcpv4Relay_set_dev2(dhcpv4ServerPoolObj->interface, TRUE, webVar->dhcpRelayServer);
         }
#endif
         else
         {
            cmsLog_debug("disable both DHCP server and relay");
            dhcpv4ServerPoolObj->enable = FALSE;
#ifdef DMP_DEVICE2_DHCPV4RELAY_1
            /* Disable DHCP Relay */
            dalDhcpv4Relay_set_dev2(dhcpv4ServerPoolObj->interface, FALSE, NULL);
#endif
         }
 
         if ((ret = cmsObj_set(dhcpv4ServerPoolObj, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Failed to set dhcpv4ServerPoolObj, ret=%d", ret);
         }
 
         if ((ret = rutLan_setLanIPv4Info_dev2(bridgeIfName,
                                               webVar->ethIpAddress,
                                               webVar->ethSubnetMask)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Failed to setLanIPv4Info_dev2, ret=%d", ret);
         }
      }

      cmsObj_free((void **)&dhcpv4ServerPoolObj);
   }
   
   return ret;
}


CmsRet dalLan_addDhcpdSubnet_dev2(const char *ipIntfFullPath,
                                  const char *gwIpAddr,
                                  const char *minIpAddr,
                                  const char *maxIpAddr,
                                  const char *subnetMask)
{
   Dev2Dhcpv4ServerPoolObject *dhcpv4ServerPoolObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("Entered: ipIntf=%s min=%s max=%s mask=%s",
                ipIntfFullPath, minIpAddr, maxIpAddr, subnetMask);

   if ((ret = cmsObj_addInstance(MDMOID_DEV2_DHCPV4_SERVER_POOL,
                                 &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not add server pool obj, ret=%d", ret);
      return ret;
   }

   if ((ret = cmsObj_get(MDMOID_DEV2_DHCPV4_SERVER_POOL,
                         &iidStack, 0,
                         (void **) &dhcpv4ServerPoolObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get newly created server pool obj, ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_DHCPV4_SERVER_POOL, &iidStack);
      return ret;
   }

   dhcpv4ServerPoolObj->enable = TRUE;
   dhcpv4ServerPoolObj->leaseTime = 24 * 60 * 60;   // 24 hours
   CMSMEM_REPLACE_STRING(dhcpv4ServerPoolObj->interface, ipIntfFullPath);
   CMSMEM_REPLACE_STRING(dhcpv4ServerPoolObj->IPRouters, gwIpAddr);
   CMSMEM_REPLACE_STRING(dhcpv4ServerPoolObj->minAddress, minIpAddr);
   CMSMEM_REPLACE_STRING(dhcpv4ServerPoolObj->maxAddress, maxIpAddr);
   CMSMEM_REPLACE_STRING(dhcpv4ServerPoolObj->subnetMask, subnetMask);

   if ((ret = cmsObj_set(dhcpv4ServerPoolObj, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set Dev2Dhcpv4ServerPoolStaticAddressObject, ret = %d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_DHCPV4_SERVER_POOL, &iidStack);
   }

   cmsObj_free((void **) &dhcpv4ServerPoolObj);

   return ret;
}


void dalLan_deleteDhcpdSubnet_dev2(const char *ipIntfFullPath)
{
   Dev2Dhcpv4ServerPoolObject *dhcpv4ServerPoolObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found=FALSE;

   cmsLog_debug("Entered: ipIntf=%s", ipIntfFullPath);

   while (!found &&
          cmsObj_getNextFlags(MDMOID_DEV2_DHCPV4_SERVER_POOL, &iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **) &dhcpv4ServerPoolObj) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(dhcpv4ServerPoolObj->interface, ipIntfFullPath))
      {
         CmsRet r2;
         found = TRUE;
         r2 = cmsObj_deleteInstance(MDMOID_DEV2_DHCPV4_SERVER_POOL, &iidStack);
         if (r2 != CMSRET_SUCCESS)
         {
            cmsLog_error("Delete of server pool obj failed, ret=%d", r2);
         }
      }

      cmsObj_free((void **) &dhcpv4ServerPoolObj);
   }

   cmsLog_debug("Exit: found=%d", found);

   return;
}


void dalLan_getIpIntfInfo_dev2(UBOOL8 getDefaults __attribute((unused)),
                               const char *ifName,
                               char *ipAddr,
                               char *subnetMask,
                               UBOOL8 *lanFirewallEnabled)
{
   IF_PTR_NOT_NULL_TERMINATE(ipAddr);
   IF_PTR_NOT_NULL_TERMINATE(subnetMask);

   qdmIpIntf_getIpv4AddrAndSubnetByNameLocked_dev2(ifName, ipAddr, subnetMask);


   if (lanFirewallEnabled != NULL)
   {
      *lanFirewallEnabled = qdmIpIntf_isFirewallEnabledOnIntfnameLocked_dev2(ifName);
   }
}


void dalLan_getBridgeIfNameFromBridgeName_dev2(const char *groupName,
                                               char *ifName)
{
   /* groupName is the Interface Group name, e.g. Default or IPTV */
   /* ifName is the Linux interface name for the bridge, e.g. br0 */

   CmsRet ret = CMSRET_INVALID_ARGUMENTS;
   UBOOL8 found = FALSE;
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   if (groupName == NULL)
   {
      cmsLog_error("Group name is NULL");
      return;
   }

   if (ifName == NULL)
   {
      cmsLog_error("Interface name is NULL");
      return;
   }

   IF_PTR_NOT_NULL_TERMINATE(ifName);

   while (found == FALSE &&
          (ret = cmsObj_getNextFlags(MDMOID_DEV2_IP_INTERFACE, &iidStack, OGF_NO_VALUE_UPDATE,
                                     (void **)&ipIntfObj))== CMSRET_SUCCESS)
   {
      if ( (!cmsUtl_strncmp(ipIntfObj->name, "br", strlen("br"))) && (cmsUtl_strcmp(groupName, ipIntfObj->X_BROADCOM_COM_GroupName) == 0))
      {
         found = TRUE;
         cmsUtl_strncpy(ifName, ipIntfObj->name, CMS_IFNAME_LENGTH);
      }
      
      cmsObj_free((void **) &ipIntfObj);
   }
}


/* For dhcpd static IP lease 
*/
void dalLan_GetStaticIpLease_dev2(const char *brName, char *varValue)
{
   char ifName[CMS_IFNAME_LENGTH]={0};
   char buff[BUFLEN_128]={0};
   UINT32 len = 0;
   UBOOL8 found = FALSE;
   Dev2Dhcpv4ServerPoolObject *dhcpv4ServerPoolObj = NULL;
   Dev2Dhcpv4ServerPoolStaticAddressObject *dhcpv4StaticAddressObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStackChild = EMPTY_INSTANCE_ID_STACK;

   if (brName == NULL)
   {
      cmsLog_error("Group name is NULL");
      return;
   }

   if (varValue == NULL)
   {
      cmsLog_error("varValue is NULL");
      return;
   }
   else
   {
      varValue[0] = '\0';
   }

   cmsLog_debug("Entered: brName=%s", brName);


   /*
    * Convert the Bridge Name (Interface group name), e.g. Default,
    * to the bridgeIfName, e.g. br0.
    */
   dalLan_getBridgeIfNameFromBridgeName_dev2(brName, ifName);

   while (found == FALSE &&
          cmsObj_getNext(MDMOID_DEV2_DHCPV4_SERVER_POOL,
                         &iidStack, (void **)&dhcpv4ServerPoolObj) == CMSRET_SUCCESS)
   {
      if (qdmIntf_fullPathToIntfnameLocked_dev2(dhcpv4ServerPoolObj->interface, buff) == CMSRET_SUCCESS &&
          cmsUtl_strcmp(ifName, buff) == 0)
      {
         found = TRUE;

         /* loop through all static IP entries to build up varValue */
         while (cmsObj_getNextInSubTree(MDMOID_DEV2_DHCPV4_SERVER_POOL_STATIC_ADDRESS,
                                        &iidStack, &iidStackChild,
                                        (void **) &dhcpv4StaticAddressObj) == CMSRET_SUCCESS)
         {
            if (dhcpv4StaticAddressObj->enable == TRUE) 
            {
               sprintf(buff, "%s/%s|", dhcpv4StaticAddressObj->chaddr, dhcpv4StaticAddressObj->yiaddr);
               cmsUtl_strncat(varValue, BUFLEN_128, buff);
            }

            cmsObj_free((void **) &dhcpv4StaticAddressObj);
         }
      }

      cmsObj_free((void **) &dhcpv4ServerPoolObj);
   }

   len = strlen(varValue);
   if (len > 0)
   {
      varValue[len-1] = '\0';   // remove the last '|' character
   }

   cmsLog_debug("Static lease info: %s", varValue);
}


CmsRet dalLan_StaticIPAdd_dev2(const char * brName, const char *static_ip, const char *mac)
{
   CmsRet ret = CMSRET_INVALID_ARGUMENTS;
   char ifName[CMS_IFNAME_LENGTH];
   char buff[CMS_IFNAME_LENGTH];
   UBOOL8 found = FALSE;
   Dev2Dhcpv4ServerPoolObject *dhcpv4ServerPoolObj = NULL;
   Dev2Dhcpv4ServerPoolStaticAddressObject *dhcpv4StaticAddressObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   IF_PTR_NOT_NULL_TERMINATE(ifName);
   IF_PTR_NOT_NULL_TERMINATE(buff);

   /*
    * Convert the Bridge Name (Interface group name), e.g. Default,
    * to the bridgeIfName, e.g. br0.
    */
   dalLan_getBridgeIfNameFromBridgeName_dev2(brName, ifName);

   while (found == FALSE &&
          cmsObj_getNext(MDMOID_DEV2_DHCPV4_SERVER_POOL,
                         &iidStack, (void **)&dhcpv4ServerPoolObj) == CMSRET_SUCCESS)
   {
      if (qdmIntf_fullPathToIntfnameLocked_dev2(dhcpv4ServerPoolObj->interface, buff) == CMSRET_SUCCESS &&
          cmsUtl_strcmp(ifName, buff) == 0)
      {
         found = TRUE;

         /* adding a new static IP lease entry */
         cmsLog_error("Adding new static IP lease with %s/%s", static_ip, mac);

         /* add new instance of Device.DHCPv4.Server.Pool.{i}.StaticAddress.{i}. */
         if ((ret = cmsObj_addInstance(MDMOID_DEV2_DHCPV4_SERVER_POOL_STATIC_ADDRESS,
                                       &iidStack)) == CMSRET_SUCCESS)
         {
            if ((ret = cmsObj_get(MDMOID_DEV2_DHCPV4_SERVER_POOL_STATIC_ADDRESS,
                                  &iidStack, 0,
                                  (void **) &dhcpv4StaticAddressObj)) == CMSRET_SUCCESS)
            {
               dhcpv4StaticAddressObj->enable = TRUE;
               CMSMEM_REPLACE_STRING(dhcpv4StaticAddressObj->chaddr, mac);
               CMSMEM_REPLACE_STRING(dhcpv4StaticAddressObj->yiaddr, static_ip);

               if ((ret = cmsObj_set(dhcpv4StaticAddressObj, &iidStack)) != CMSRET_SUCCESS)
               {
                  cmsLog_error("Failed to set Dev2Dhcpv4ServerPoolStaticAddressObject, ret = %d", ret);
                  cmsObj_deleteInstance(MDMOID_DEV2_DHCPV4_SERVER_POOL_STATIC_ADDRESS, &iidStack);
               }

               cmsObj_free((void **) &dhcpv4StaticAddressObj); 
            }
            else
            {
               cmsLog_error("Failed to get Dev2Dhcpv4ServerPoolStaticAddressObject, ret=%d", ret);
               cmsObj_deleteInstance(MDMOID_DEV2_DHCPV4_SERVER_POOL_STATIC_ADDRESS, &iidStack);
            }
         }
         else
         {
            cmsLog_error("Could not create new Dev2Dhcpv4ServerPoolStaticAddressObject, ret=%d", ret);
         }
      }

      cmsObj_free((void **) &dhcpv4ServerPoolObj);
   }

   return ret;
}


CmsRet dalLan_StaticIPRemove_dev2(const char * brName, const char *mac)
{
   CmsRet ret = CMSRET_INVALID_ARGUMENTS;
   char ifName[CMS_IFNAME_LENGTH];
   char buff[CMS_IFNAME_LENGTH];
   UBOOL8 foundServer = FALSE, foundAddress = FALSE;
   Dev2Dhcpv4ServerPoolObject *dhcpv4ServerPoolObj = NULL;
   Dev2Dhcpv4ServerPoolStaticAddressObject *dhcpv4StaticAddressObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStackChild = EMPTY_INSTANCE_ID_STACK;

   IF_PTR_NOT_NULL_TERMINATE(ifName);
   IF_PTR_NOT_NULL_TERMINATE(buff);

   cmsLog_error("Deleting static IP lease with mac = %s", mac);

   /*
    * Convert the Bridge Name (Interface group name), e.g. Default,
    * to the bridgeIfName, e.g. br0.
    */
   dalLan_getBridgeIfNameFromBridgeName(brName, ifName);

   while (foundServer == FALSE &&
          cmsObj_getNext(MDMOID_DEV2_DHCPV4_SERVER_POOL,
                         &iidStack, (void **)&dhcpv4ServerPoolObj) == CMSRET_SUCCESS)
   {
      if (qdmIntf_fullPathToIntfnameLocked_dev2(dhcpv4ServerPoolObj->interface, buff) == CMSRET_SUCCESS &&
          cmsUtl_strcmp(ifName, buff) == 0)
      {
         foundServer = TRUE;

         while (foundAddress == FALSE &&
                cmsObj_getNextInSubTree(MDMOID_DEV2_DHCPV4_SERVER_POOL_STATIC_ADDRESS,
                                        &iidStack, &iidStackChild,
                                        (void **) &dhcpv4StaticAddressObj) == CMSRET_SUCCESS)
         {
            if (dhcpv4StaticAddressObj->enable == TRUE &&
                cmsUtl_strcmp(mac, dhcpv4StaticAddressObj->chaddr) == 0) 
            {
               foundAddress = TRUE;
               
               cmsLog_debug("Disable static IP lease, found a match mac = %s", mac);
               dhcpv4StaticAddressObj->enable = FALSE;
               cmsObj_set(dhcpv4StaticAddressObj, &iidStackChild);
               cmsObj_free((void **) &dhcpv4StaticAddressObj);

               cmsLog_debug("Deleting static IP lease, found a match mac = %s", mac);
               ret = cmsObj_deleteInstance(MDMOID_DEV2_DHCPV4_SERVER_POOL_STATIC_ADDRESS,
                                           &iidStackChild);
               if (ret != CMSRET_SUCCESS)
               {
                  cmsLog_error("Failed to delete Dev2Dhcpv4ServerPoolStaticAddressObject, ret = %d", ret);
               }
            }

            cmsObj_free((void **) &dhcpv4StaticAddressObj);
         }
      }

      cmsObj_free((void **) &dhcpv4ServerPoolObj);
   }

   if (foundAddress == FALSE) 
   {
      cmsLog_error("Bad mac %s, no static IP lease entry found", mac);
      ret = CMSRET_INVALID_ARGUMENTS;
   }

   return ret;
}


CmsRet dalLan_getIgmpSnoopingObject_dev2(const char *bridgeIfName __attribute((unused)),
                                         InstanceIdStack *iidStack __attribute((unused)),
                                         IgmpSnoopingCfgObject **igmpSnpObj __attribute((unused)))
{
   cmsLog_error("not implemented yet");

   return CMSRET_INTERNAL_ERROR;
}


#ifdef SUPPORT_LANVLAN
static CmsRet dal_enableVlanTermination_dev2(const char *fullPath, UINT32 enable);
static CmsRet dal_createVlanTermination_dev2(const char *lowerLayer, SINT32 vlanId, UINT32 pBit, UINT32 enable);
static CmsRet dal_deleteVlanTermination_dev2(const char *lowerLayer, SINT32 vlanId);

void dalLan_getLanVlan_dev2(WEB_NTWK_VAR *webVar) 
{
   webVar->lanVlanEnable = 0;
   webVar->lanVlanTagList[0] = '\0';

#ifdef DMP_DEVICE2_VLANTERMINATION_1
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2VlanTerminationObject *vlanTerminationObj;

   while (cmsObj_getNext(MDMOID_DEV2_VLAN_TERMINATION,
                         &iidStack, (void **)&vlanTerminationObj) == CMSRET_SUCCESS)
   {
      if (!IS_EMPTY_STRING(vlanTerminationObj->name) && 
          cmsUtl_strstr(vlanTerminationObj->name, webVar->lanName) != NULL)
      {
         char strVlanTag[BUFLEN_16];
         snprintf(strVlanTag, BUFLEN_16, "%d/%d", vlanTerminationObj->VLANID,
                 vlanTerminationObj->X_BROADCOM_COM_Vlan8021p);
         cmsUtl_strncat(webVar->lanVlanTagList, BUFLEN_16, strVlanTag);
         if (vlanTerminationObj->enable)
         {
            webVar->lanVlanEnable = 1;
         }
      }
      cmsObj_free((void **) &vlanTerminationObj);
   }
#endif

   cmsLog_debug("ifName=%s lanVlanEnable=%d lanVlanTagList=%s", webVar->lanName, webVar->lanVlanEnable, webVar->lanVlanTagList);
}

void dalLan_setLanVlan_dev2(WEB_NTWK_VAR *webVar) 
{
#ifdef DMP_DEVICE2_VLANTERMINATION_1
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2VlanTerminationObject *vlanTerminationObj;
   char vlanPathRef[BUFLEN_64];
#endif
   MdmPathDescriptor pathDesc;
   char *lanFullPath = NULL;
   CmsRet ret;
   char tmpStr[BUFLEN_32];
   char *ptr, *token, *saveptr;
   unsigned int i, j, vlans[16], vid, pBit;
   UBOOL8 found = FALSE;

   qdmIntf_getPathDescFromIntfnameLocked_dev2(webVar->lanName, TRUE, &pathDesc);
   if ((ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &lanFullPath)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not find fullpath for lan interface(%s), ret=%d", webVar->lanName, ret);
      return;
   }
   cmsLog_debug("FullPath of LAN is %s", lanFullPath);

   memset(vlans, 0, sizeof(vlans));
   snprintf(tmpStr, BUFLEN_32, "%s", webVar->lanVlanTagList);

   /* parsing the lavVlanTagList string in webVar for creating new vlan */
   for (i = 0, ptr = tmpStr; ; ptr = NULL, i++)
   {
      token = strtok_r(ptr, "|", &saveptr);
      if (token == NULL)
      {
         break;
      }

      if (sscanf(token, "%u/%u", &vid, &pBit) != 2)
      {
           cmsLog_error("Wrong format of webVar->lanVlanTagList, [%s]", webVar->lanVlanTagList);
           break;
      }
      vlans[i] = vid;

#ifdef DMP_DEVICE2_VLANTERMINATION_1
      //if found, update enable/disable; else create new instance
      if ((ret = qdmEth_getEthernetVlanTermination_dev2(vid, lanFullPath, vlanPathRef, BUFLEN_64)) == CMSRET_SUCCESS)
      {
         dal_enableVlanTermination_dev2(vlanPathRef, webVar->lanVlanEnable);
      }
      else
      {
         char vlanName[BUFLEN_8];

         dal_createVlanTermination_dev2(lanFullPath, vid, pBit, webVar->lanVlanEnable);
         snprintf(vlanName, BUFLEN_8, "%s.%d", webVar->lanName, vid);
#ifdef DMP_DEVICE2_BRIDGE_1
         dalBridge_addIntfNameToBridge_dev2(vlanName, "br0");
#endif
      }
#endif /* DMP_DEVICE2_VLANTERMINATION_1 */
   } /* for loop -- tag list parsing */

   /* Go through all VlanTermination Objects to delete none exist vlan */
#ifdef DMP_DEVICE2_VLANTERMINATION_1
   INIT_INSTANCE_ID_STACK(&iidStack);
   while (cmsObj_getNext(MDMOID_DEV2_VLAN_TERMINATION, &iidStack,
                         (void **)&vlanTerminationObj) == CMSRET_SUCCESS)
   {
      found = FALSE;
      for (j = 0 ; j < i ; j++)
      { 
         if (!IS_EMPTY_STRING(vlanTerminationObj->lowerLayers) && 
               cmsUtl_strstr(vlanTerminationObj->lowerLayers, lanFullPath) != NULL &&
               (unsigned int)vlanTerminationObj->VLANID == vlans[j])
         {
            found = TRUE;
            break;
         }
      }

      if (!found)
      {
#ifdef DMP_DEVICE2_BRIDGE_1
         dalBridge_deleteIntfNameFromBridge_dev2(vlanTerminationObj->name);
#endif
         dal_deleteVlanTermination_dev2(lanFullPath, vlanTerminationObj->VLANID);
      }

      cmsObj_free((void **) &vlanTerminationObj);
   }
#endif /* DMP_DEVICE2_VLANTERMINATION_1 */
   CMSMEM_FREE_BUF_AND_NULL_PTR(lanFullPath);
}

static CmsRet dal_enableVlanTermination_dev2(const char *fullPath, UINT32 enable)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2VlanTerminationObject *ethVlanObj = NULL;
   MdmPathDescriptor pathDesc;
   CmsRet ret = CMSRET_SUCCESS;

   if ((ret = cmsMdm_fullPathToPathDescriptor(fullPath, &pathDesc)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not translate fullPath(%s) to path descriptor, ret=%d", fullPath, ret);
      return ret;
   }
   if ((ret = cmsObj_get(MDMOID_DEV2_VLAN_TERMINATION, &(pathDesc.iidStack), 0, (void **) &ethVlanObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get VlanTermination Object, ret=%d", ret);
      return ret;
   }

   if (ethVlanObj->enable != enable)
   {
      ethVlanObj->enable = enable;
      if ((ret = cmsObj_set(ethVlanObj, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("could not set VlanTermination object, ret=%d", ret);
      }
   }
   cmsObj_free((void **) &ethVlanObj);
   return ret;
}

static CmsRet dal_createVlanTermination_dev2(const char *lowerLayer,
                                                  SINT32 vlanId,
                                                  UINT32 pBit,
                                                  UINT32 enable)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2VlanTerminationObject *ethVlanObj;
   CmsRet ret = CMSRET_SUCCESS;

   if ((ret = cmsObj_addInstance(MDMOID_DEV2_VLAN_TERMINATION, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create VlanTermination object instance, ret=%d", ret);
      return ret;
   }

   if ((ret = cmsObj_get(MDMOID_DEV2_VLAN_TERMINATION, &iidStack, 0, (void**) &ethVlanObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get VlanTermination object, ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_VLAN_TERMINATION, &iidStack);
      return ret;
   }

   ethVlanObj->enable = enable;
   ethVlanObj->VLANID = vlanId;
   ethVlanObj->X_BROADCOM_COM_Vlan8021p = pBit;
   CMSMEM_REPLACE_STRING(ethVlanObj->lowerLayers, lowerLayer);
   if ((ret = cmsObj_set(ethVlanObj, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not set VlanTermination object, ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_VLAN_TERMINATION, &iidStack);
   }
   cmsObj_free((void **) &ethVlanObj);
   return ret;
}

static CmsRet dal_deleteVlanTermination_dev2(const char *lowerLayer, SINT32 vlanId)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;   
   Dev2VlanTerminationObject *ethVlanObj = NULL;
   CmsRet ret;

   cmsLog_debug("=====> Entered");
   
   if (lowerLayer == NULL)
   {
      cmsLog_error("Invalid aruments.");
      return CMSRET_INVALID_ARGUMENTS;
   }


   while ((ret = cmsObj_getNext(MDMOID_DEV2_VLAN_TERMINATION, &iidStack, (void **) &ethVlanObj)) == CMSRET_SUCCESS)
   {
      if (ethVlanObj->VLANID == vlanId && !cmsUtl_strcmp(ethVlanObj->lowerLayers, lowerLayer))
      {
         ret = cmsObj_deleteInstance(MDMOID_DEV2_VLAN_TERMINATION, &iidStack);
         cmsObj_free((void **) &ethVlanObj);
         return ret;
      }
      cmsObj_free((void **) &ethVlanObj);
   }

   cmsLog_debug("Exit. ret %d", ret);

   return ret;
}

#endif  /* SUPPORT_LANVLAN */
#endif  /* DMP_DEVICE2_BASELINE_1 */


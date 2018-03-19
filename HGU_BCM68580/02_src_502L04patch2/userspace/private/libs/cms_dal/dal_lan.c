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

#include "cms.h"
#include "qdm_multicast.h"
#include "qdm_lan.h"
#include "cms_obj.h"
#include "cms_dal.h"
#include "cms_util.h"
#include "cms_msg.h"
#include "dal.h"



#ifdef DMP_BASELINE_1

CmsRet dalLan_setLanDevice_igd(const WEB_NTWK_VAR *webVar)
{
   CmsRet ret = CMSRET_SUCCESS;
   char bridgeIfName[CMS_IFNAME_LENGTH];

   InstanceIdStack lanDevIidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack lanHostIidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack lanIpIntfIidStack = EMPTY_INSTANCE_ID_STACK;
   LanDevObject *lanDeviceObj = NULL;
   LanHostCfgObject *lanHostCfgObj = NULL;
   LanIpIntfObject *lanIpObj = NULL;
   char dnsServerList[BUFLEN_64];  /* 2 comma separated list of DNS servers for now */
   UBOOL8 found = FALSE;

   dalLan_getBridgeIfNameFromBridgeName(webVar->brName, bridgeIfName);

   if ((ret = dalLan_getLanDevByBridgeIfName(bridgeIfName, &lanDevIidStack, &lanDeviceObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not find LANDevice for bridgeIfname %s, ret=%d", bridgeIfName, ret);
   }
   else
   {
         /* don't need the object, just need the iidStack */
         cmsObj_free((void **) &lanDeviceObj);

         /* Set the LANHostConfigManagent Configration*/
         lanHostIidStack = lanDevIidStack;
         if((ret = cmsObj_get(MDMOID_LAN_HOST_CFG, &lanHostIidStack, 0, (void **)&lanHostCfgObj)) == CMSRET_SUCCESS)
         {           
            if (webVar->enblDhcpSrv == 1)
            {
               lanHostCfgObj->DHCPServerEnable = TRUE;

               /* update dns server info, comman seperated dns list */
               snprintf(dnsServerList, sizeof(dnsServerList), "%s,%s", webVar->dnsPrimary, webVar->dnsSecondary);
               CMSMEM_REPLACE_STRING(lanHostCfgObj->DNSServers, dnsServerList);

               CMSMEM_REPLACE_STRING(lanHostCfgObj->minAddress, webVar->dhcpEthStart);

               CMSMEM_REPLACE_STRING(lanHostCfgObj->maxAddress, webVar->dhcpEthEnd);   

               CMSMEM_REPLACE_STRING(lanHostCfgObj->subnetMask, webVar->ethSubnetMask);
               lanHostCfgObj->DHCPLeaseTime = webVar->dhcpLeasedTime * 60 * 60; /* webVar is in hours, MDM in seconds */
               /* Disable DHCP Relay */
               CMSMEM_FREE_BUF_AND_NULL_PTR(lanHostCfgObj->X_BROADCOM_COM_DhcpRelayServer); 
            }
#ifdef DHCP_RELAY
            else if (webVar->enblDhcpSrv == 2)
            {
               lanHostCfgObj->DHCPServerEnable = TRUE;
	       
	       /* Enable DHCP Relay */
               CMSMEM_REPLACE_STRING(lanHostCfgObj->X_BROADCOM_COM_DhcpRelayServer, webVar->dhcpRelayServer);
            }
#endif
            else
            {
               cmsLog_debug("set LanHostCfgObj->DHCPServerEnable to FALSE");
               lanHostCfgObj->DHCPServerEnable = FALSE;

#ifdef DHCP_RELAY
	       /* Disable DHCP Relay */
               CMSMEM_FREE_BUF_AND_NULL_PTR(lanHostCfgObj->X_BROADCOM_COM_DhcpRelayServer); 
#endif
            }
 
            if ((ret = cmsObj_set(lanHostCfgObj, &lanHostIidStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("Failed to set Lan Host CfgMangmnt, ret = %d", ret);
            }

            cmsObj_free((void **) &lanHostCfgObj);
         }
         else
         {
            cmsLog_error("Failed to get MDMOID_LAN_HOST_CFG obj, ret = %d", ret);
         }

         /* Set the primary LANHostConfigManagement.IPInterface. Configration*/
         if((ret = cmsObj_getNextInSubTree(MDMOID_LAN_IP_INTF, &lanHostIidStack, &lanIpIntfIidStack, (void **) &lanIpObj)) == CMSRET_SUCCESS)
         {
            UBOOL8 doSet=FALSE;

            /* update IpInterface with the new values if they are changed from current.
             * This prevents us from setting the AddressintType to STATIC
             * even though user was only trying to config something else on
             * the LAN webpage (for homeplug).
             */
            if (cmsUtl_strcmp(lanIpObj->IPInterfaceIPAddress, webVar->ethIpAddress) ||
                cmsUtl_strcmp(lanIpObj->IPInterfaceSubnetMask, webVar->ethSubnetMask))
            {
                doSet = TRUE;
                CMSMEM_REPLACE_STRING(lanIpObj->IPInterfaceAddressingType, MDMVS_STATIC);
                CMSMEM_REPLACE_STRING(lanIpObj->IPInterfaceIPAddress, webVar->ethIpAddress);
                CMSMEM_REPLACE_STRING(lanIpObj->IPInterfaceSubnetMask, webVar->ethSubnetMask);
            }

            if (lanIpObj->X_BROADCOM_COM_FirewallEnabled != webVar->enblLanFirewall)
            {
               doSet = TRUE;
               lanIpObj->X_BROADCOM_COM_FirewallEnabled = webVar->enblLanFirewall;
            }

            if (doSet)
            {
               if ((ret = cmsObj_set(lanIpObj, &lanIpIntfIidStack)) != CMSRET_SUCCESS)
               {
                  cmsLog_error("Failed to set IPInterface, ret = %d", ret);
               }
            }
            cmsObj_free((void **) &lanIpObj);
         }
         else
         {
            cmsLog_error("Failed to get MDMOID_LAN_IP_INTF obj, ret = %d", ret);
         }

         /* Set the Second LANHostConfigManagement.IPInterface. Configration*/
         INIT_INSTANCE_ID_STACK(&lanIpIntfIidStack);
         found = FALSE;
         while ((ret = cmsObj_getNextInSubTree(MDMOID_LAN_IP_INTF, &lanHostIidStack, &lanIpIntfIidStack, (void **) &lanIpObj)) == CMSRET_SUCCESS)
         {
            /* Assume that bridge name associated with secondary ip address have ":" */
            if (cmsUtl_strstr(lanIpObj->X_BROADCOM_COM_IfName, ":") != NULL)
            {
               found = TRUE;
               break;
            }
            cmsObj_free((void **) &lanIpObj);
         }

         if (found == FALSE && webVar->enblLan2 == TRUE)
         {
            /* could not find it, create a new one */
            if ((ret = cmsObj_addInstance(MDMOID_LAN_IP_INTF, &lanHostIidStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("could not create new lan ip interface instance , ret=%d", ret);
               return ret;
            }

            if ((ret = cmsObj_get(MDMOID_LAN_IP_INTF, &lanHostIidStack, 0, (void **) &lanIpObj)) != CMSRET_SUCCESS)
            {
               cmsLog_error("Failed to get LanIpIntfObject, ret=%d", ret);
               cmsObj_deleteInstance(MDMOID_LAN_IP_INTF, &lanHostIidStack);
               return ret;
            }

            CMSMEM_REPLACE_STRING(lanIpObj->IPInterfaceIPAddress, webVar->lan2IpAddress);
            CMSMEM_REPLACE_STRING(lanIpObj->IPInterfaceSubnetMask, webVar->lan2SubnetMask);
            lanIpObj->enable = webVar->enblLan2;

            if((ret = cmsObj_set(lanIpObj, &lanHostIidStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("Failed to set LanIpIntfObject, ret=%d", ret);
               cmsObj_deleteInstance(MDMOID_LAN_IP_INTF, &lanHostIidStack);
            }
            cmsObj_free((void **) &lanIpObj);
         }
         else if(found == TRUE && webVar->enblLan2 == TRUE)
         {
            /* to update secondary ip address if necessary */
            CMSMEM_REPLACE_STRING(lanIpObj->IPInterfaceIPAddress, webVar->lan2IpAddress);
            CMSMEM_REPLACE_STRING(lanIpObj->IPInterfaceSubnetMask, webVar->lan2SubnetMask);
            lanIpObj->enable = webVar->enblLan2;
            if((ret = cmsObj_set(lanIpObj, &lanIpIntfIidStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("Failed to set LanIpIntfObject, ret=%d", ret);
            }
            cmsObj_free((void **) &lanIpObj);
         }
         else if(found == TRUE && webVar->enblLan2 == FALSE)
         {  
            /* found it, delete secondary ip address */
            cmsObj_free((void **) &lanIpObj);
            if((ret = cmsObj_deleteInstance(MDMOID_LAN_IP_INTF, &lanIpIntfIidStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("Failed to delete LanIpIntfObject, ret=%d", ret);
            }
         }
         else
         {
            ret = CMSRET_SUCCESS;
         }
   }

   return ret;
}

#endif  /* DMP_BASELINE_1 */



void dalLan_getFirstIntfGroupName_igd(char *brName)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

#ifdef DMP_BRIDGING_1
   {
      /*
       * If TR-098 Layer 2 Bridging is enabled, we want to set the brName
       * to the first interface group name in the layer 2 bridging objects.
       */
      L2BridgingEntryObject    *bridgeObj = NULL;
      if (CMSRET_SUCCESS == cmsObj_getNextFlags(MDMOID_L2_BRIDGING_ENTRY, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&bridgeObj))
      {
         strcpy(brName, bridgeObj->bridgeName);
         cmsObj_free((void **) &bridgeObj);
      }
      else
      {
         cmsLog_error("could not find first Layer 2 bridge object");
         return;
      }
   }
#else
   {
      /*
       * If TR-098 Layer 2 Bridging not enabled, we want to set the brName
       * to the first IP Interface name.
       */
      LanIpIntfObject          *lanIpObj = NULL;

      if (CMSRET_SUCCESS == cmsObj_getNextFlags(MDMOID_LAN_IP_INTF, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&lanIpObj))
      {
         strcpy(brName, lanIpObj->X_BROADCOM_COM_IfName);
         cmsObj_free((void **) &lanIpObj);
      }
      else
      {
         cmsLog_error("could not find first IP Intf object");
         return;
      }
   }
#endif

   return;
}



void dalLan_getLanPageInfo(WEB_NTWK_VAR *webVar)
{
   char ifName[CMS_IFNAME_LENGTH]={0};

   dalLan_getBridgeIfNameFromBridgeName(webVar->brName, ifName);

   /* get primary IP interface info */
   dalLan_getIpIntfInfo(FALSE, ifName, webVar->ethIpAddress,
                                       webVar->ethSubnetMask,
                                       &(webVar->enblLanFirewall));

   webVar->dataModelDevice2 = cmsMdm_isDataModelDevice2();

   /* secondary IP interface is not supported in PURE TR181 */
   if (webVar->dataModelDevice2 == FALSE)
   {
      /* get secondary IP interface info, if it is present */
      dalLan_get2IpIntfInfo(ifName, &webVar->enblLan2,
                            webVar->lan2IpAddress,
                            webVar->lan2SubnetMask);
   }

   /* get DHCP server info */
   dalLan_getDhcpServerInfo(ifName,
                            &(webVar->enblDhcpSrv),
                            webVar->dhcpEthStart,
                            webVar->dhcpEthEnd,
                            &(webVar->dhcpLeasedTime),
                            webVar->dhcpRelayServer);

   return;
}



#if defined(DMP_BASELINE_1)
void dalLan_getIpIntfInfo_igd(UBOOL8 getDefaults,
                          const char *ifName,
                          char *ipAddr,
                          char *subnetMask,
                          UBOOL8 *lanFirewallEnabled)
{
   LanIpIntfObject *lanIpIntf=NULL;
   CmsRet ret;

   /* terminate the return string in case we cannot get data */
   IF_PTR_NOT_NULL_TERMINATE(ipAddr);
   IF_PTR_NOT_NULL_TERMINATE(subnetMask);


   if (getDefaults)
   {
      /* getting a default value should always work.  No need to pass in iidStack. */
      if ((ret = cmsObj_get(MDMOID_LAN_IP_INTF,
                            NULL,
                            OGF_DEFAULT_VALUES,
                            (void *) &lanIpIntf)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not get default lanIpIntf, ret=%d", ret);
         return;
      }
   }
   else
   {
      UBOOL8 found = FALSE;
      InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

      while (!found)
      {
         if ((ret = cmsObj_getNext(MDMOID_LAN_IP_INTF,
                                   &iidStack,
                                   (void *) &lanIpIntf)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Could not get current lanIpIntf, ret=%d", ret);
            return;
         }

         cmsLog_debug("Found lanIpIntf Ifname=%s looking for %s",
                      lanIpIntf->X_BROADCOM_COM_IfName, ifName);

         if (!strcmp(ifName, lanIpIntf->X_BROADCOM_COM_IfName))
         {
            found = TRUE;
         }
         else
         {
            cmsObj_free((void **) &lanIpIntf);
         }
      }
   }

   if ((lanIpIntf->IPInterfaceIPAddress != NULL) && (ipAddr != NULL))
   {
      sprintf(ipAddr, "%s", lanIpIntf->IPInterfaceIPAddress);
   }

   if ((lanIpIntf->IPInterfaceSubnetMask != NULL) && (subnetMask != NULL))
   {
      sprintf(subnetMask, "%s", lanIpIntf->IPInterfaceSubnetMask);
   }

   if (lanFirewallEnabled != NULL)
   {
      *lanFirewallEnabled = lanIpIntf->X_BROADCOM_COM_FirewallEnabled;
   }


   cmsObj_free((void **) &lanIpIntf);

   return;
}
#endif  /* DMP_BASELINE_1 */


void dalLan_get2IpIntfInfo( const char *primaryIfName,
                            SINT32 *enable,
                            char *ipAddr,
                            char *subnetMask)
{
   LanIpIntfObject *lanIpIntf=NULL;
   CmsRet ret;
   char ifName2[CMS_IFNAME_LENGTH];
   UBOOL8 found = FALSE;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;


   /* terminate the return string in case we cannot get data */
   IF_PTR_NOT_NULL_TERMINATE(ipAddr);
   IF_PTR_NOT_NULL_TERMINATE(subnetMask);
   *enable = FALSE;

   sprintf(ifName2, "%s:0", primaryIfName);
   while ((!found) &&
          ((ret = cmsObj_getNext(MDMOID_LAN_IP_INTF,
                                 &iidStack,
                                 (void *) &lanIpIntf)) == CMSRET_SUCCESS))
   {
      cmsLog_debug("Found lanIpIntf Ifname=%s looking for %s",
                   lanIpIntf->X_BROADCOM_COM_IfName, ifName2);

      if (!cmsUtl_strcmp(ifName2, lanIpIntf->X_BROADCOM_COM_IfName))
      {
         found = TRUE;
         *enable = TRUE;

         if ((lanIpIntf->IPInterfaceIPAddress != NULL) && (ipAddr != NULL))
         {
            strcpy(ipAddr, lanIpIntf->IPInterfaceIPAddress);
         }

         if ((lanIpIntf->IPInterfaceSubnetMask != NULL) && (subnetMask != NULL))
         {
            strcpy(subnetMask, lanIpIntf->IPInterfaceSubnetMask);
         }
      }

      cmsObj_free((void **) &lanIpIntf);
   }

   return;
}


void dalLan_getDhcpServerInfo_igd(const char *ifName,
                                  SINT32 *enabled,
                                  char *minAddress,
                                  char *maxAddress,
                                  SINT32 *leaseTime,
                                  char *relaySrv)
 {
   LanIpIntfObject *lanIpIntfObj = NULL;
   LanHostCfgObject *lanHostCfg = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found=FALSE;
   CmsRet ret;


   IF_PTR_NOT_NULL_TERMINATE(minAddress);
   IF_PTR_NOT_NULL_TERMINATE(maxAddress);


   /*
    * First find the specified ifName (br0, br1, etc)
    */
   while((!found) &&
         (ret = cmsObj_getNext(MDMOID_LAN_IP_INTF, &iidStack, (void **)&lanIpIntfObj)) == CMSRET_SUCCESS)
   {

      if (0 == cmsUtl_strcmp(ifName, lanIpIntfObj->X_BROADCOM_COM_IfName))
      {
         found = TRUE;
      }

      cmsObj_free((void **)&lanIpIntfObj);
   }

   if (!found)
   {
      cmsLog_error("could not find ifName=%s", ifName);
      return;
   }


   /*
    * Now get the LANHostConfigManagement object, which is the parent
    * of the LANIPIntf object.
    */
   ret = cmsObj_getAncestor(MDMOID_LAN_HOST_CFG,
                            MDMOID_LAN_IP_INTF,
                            &iidStack,
                           (void **) &lanHostCfg);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("get ancestor of %s failed", cmsMdm_dumpIidStack(&iidStack));
      return;
   }


   if (lanHostCfg->minAddress != NULL && minAddress != NULL)
   {
      strcpy(minAddress, lanHostCfg->minAddress);
      cmsLog_debug("minAddress=%s", minAddress);
   }

   if (lanHostCfg->maxAddress != NULL && maxAddress != NULL)
   {
      strcpy(maxAddress, lanHostCfg->maxAddress);
      cmsLog_debug("maxAddress=%s", maxAddress);
   }

#ifdef DHCP_RELAY
   if (lanHostCfg->X_BROADCOM_COM_DhcpRelayServer!= NULL && relaySrv != NULL)
   {
      strcpy(relaySrv, lanHostCfg->X_BROADCOM_COM_DhcpRelayServer);
      cmsLog_debug("relaySrv=%s", relaySrv);
   }
#else
   *relaySrv = 0;
#endif

   if (enabled != NULL)
   {
#ifdef DHCP_RELAY
      if (lanHostCfg->DHCPServerEnable)
      {
         if (lanHostCfg->X_BROADCOM_COM_DhcpRelayServer)
            *enabled = 2;
         else
            *enabled = 1;
      }
      else
         *enabled = 0;
#else
      *enabled = (SINT32) lanHostCfg->DHCPServerEnable;
#endif
      cmsLog_debug("enabled=%d", *enabled);
   }

   if (leaseTime != NULL)
   {
      *leaseTime = lanHostCfg->DHCPLeaseTime / (60 * 60); /* MDM in seconds, webVar in hours */
      cmsLog_debug("leaseTime=%d hours", *leaseTime);
   }

   cmsObj_free((void **) &lanHostCfg);

   return;
}


/* For dhcpd static IP lease 
*/
void dalLan_GetStaticIpLease_igd(const char *brName, char *varValue)
{
   char bridgeIfName[CMS_IFNAME_LENGTH]={0};
   char temp[BUFLEN_128];
   UINT32 len = 0;
   LanDevObject *lanDeviceObj=NULL;
   DHCPConditionalServingObject *lanConditionServeObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack1 = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;


   varValue[0] = '\0';

   cmsLog_debug("Getting Static IP lease info brName=%s", brName);


   /*
    * Convert the Bridge Name (Interface group name), e.g. Default,
    * to the bridgeIfName, e.g. br0.
    */
   dalLan_getBridgeIfNameFromBridgeName(brName, bridgeIfName);

   ret = dalLan_getLanDevByBridgeIfName(bridgeIfName, &iidStack, &lanDeviceObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not find LANDevice for bridgeIfName=%s, ret=%d", bridgeIfName, ret);
      return;
   }

   /* we only need the instance id, not the lanDeviceObj */
   cmsObj_free((void **)&lanDeviceObj);

   cmsLog_debug("lanDevice at %s, ifName=%s", cmsMdm_dumpIidStack(&iidStack), bridgeIfName);


   /* loop through all static IP entries to build up varValue */
   while ( cmsObj_getNextInSubTree(MDMOID_DHCP_CONDITIONAL_SERVING, &iidStack, &iidStack1, (void **) &lanConditionServeObj) == CMSRET_SUCCESS)
   {
      if ( lanConditionServeObj->enable == TRUE) 
      {
         sprintf(temp, "%s/%s|", lanConditionServeObj->chaddr, lanConditionServeObj->reservedAddresses);
         strcat(varValue, temp);

         cmsObj_free((void **) &lanConditionServeObj);
      }
   }

   len = strlen(varValue);
   if (len > 0)
   {
      varValue[len-1] = '\0';   // remove the last '|' character
   }


   cmsLog_debug("Static lease info: %s", varValue);
}


CmsRet dalLan_StaticIPAdd_igd(const char * brName, const char *static_ip, const char *mac)
{
   char bridgeIfName[CMS_IFNAME_LENGTH]={0};
   LanDevObject *lanDeviceObj = NULL;
   DHCPConditionalServingObject *lanConditionServeObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;


   /*
    * Convert the Bridge Name (Interface group name), e.g. Default,
    * to the bridgeIfName, e.g. br0.
    */
   dalLan_getBridgeIfNameFromBridgeName(brName, bridgeIfName);

   ret = dalLan_getLanDevByBridgeIfName(bridgeIfName, &iidStack, &lanDeviceObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not find LANDevice for bridgeIfName=%s, ret=%d", bridgeIfName, ret);
      return ret;
   }

   /* we only need the instance id, not the lanDeviceObj */
   cmsObj_free((void **)&lanDeviceObj);


   /* adding a new static IP lease entry */
   cmsLog_debug("Adding new static IP lease with %s/%s", static_ip, mac);

   /* add new instance of LANHostConfigManagement.DHCPConditionalServingPool.{i}. */
   if ((ret = cmsObj_addInstance(MDMOID_DHCP_CONDITIONAL_SERVING, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create new LANHostConfigManagement.DHCPConditionalServingPool.{i}., ret=%d", ret);
      return ret;
   }
  
   if ((ret = cmsObj_get(MDMOID_DHCP_CONDITIONAL_SERVING, &iidStack, 0, (void **) &lanConditionServeObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get LANHostConfigManagement.DHCPConditionalServingPool.{i}., ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DHCP_CONDITIONAL_SERVING, &iidStack);
      return ret;
   } 

   lanConditionServeObj->enable = TRUE;
   CMSMEM_REPLACE_STRING(lanConditionServeObj->chaddr, mac);
   CMSMEM_REPLACE_STRING(lanConditionServeObj->reservedAddresses, static_ip);

   if ((ret = cmsObj_set(lanConditionServeObj, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set DHCPConditionalServingPool, ret = %d", ret);
      cmsObj_deleteInstance(MDMOID_DHCP_CONDITIONAL_SERVING, &iidStack);
   }

   cmsObj_free((void **) &lanConditionServeObj); 

   return ret;
}


CmsRet dalLan_StaticIPRemove_igd(const char * brName, const char *mac)
{
   char bridgeIfName[CMS_IFNAME_LENGTH]={0};
   DHCPConditionalServingObject *lanConditionServeObj = NULL;
   LanDevObject *lanDeviceObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack lanConditionServeIidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found = FALSE;
   CmsRet ret;


   cmsLog_debug("Deleting static IP lease with mac = %s", mac);


   /*
    * Convert the Bridge Name (Interface group name), e.g. Default,
    * to the bridgeIfName, e.g. br0.
    */
   dalLan_getBridgeIfNameFromBridgeName(brName, bridgeIfName);

   ret = dalLan_getLanDevByBridgeIfName(bridgeIfName, &iidStack, &lanDeviceObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not find LANDevice for bridgeIfName=%s, ret=%d", bridgeIfName, ret);
      return ret;
   }

   /* we only need the instance id, not the lanDeviceObj */
   cmsObj_free((void **)&lanDeviceObj);


   while ((ret = cmsObj_getNextInSubTree(MDMOID_DHCP_CONDITIONAL_SERVING, 
                                         &iidStack, 
                                         &lanConditionServeIidStack, 
                                         (void **) &lanConditionServeObj)) == CMSRET_SUCCESS)
   {
      if( lanConditionServeObj->enable && !cmsUtl_strcmp(mac, lanConditionServeObj->chaddr))
      {
         found = TRUE;
         break;
      }
      cmsObj_free((void **) &lanConditionServeObj);
   }

   if(found == FALSE) 
   {
      cmsLog_debug("bad mac %s, no static IP lease entry found", mac);
      ret = CMSRET_INVALID_ARGUMENTS;
   }
   else 
   {
      /* For udhcpd.conf generation, we disable the object first then delete the object */
      /* mwang: the rcl handler function and udhcpd.conf generatation should
       * work correctly if you just delete the entry without disabling it first,
       * but I'll leave the code the way it is for now.
       */
      cmsLog_debug("disable static IP lease, found a match mac = %s", mac);
      lanConditionServeObj->enable = FALSE;
      cmsObj_set(lanConditionServeObj, &lanConditionServeIidStack);
      cmsObj_free((void **) &lanConditionServeObj);

      cmsLog_debug("Deleting static IP lease, found a match mac = %s", mac);
      ret = cmsObj_deleteInstance(MDMOID_DHCP_CONDITIONAL_SERVING, &lanConditionServeIidStack);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to delete lanConditionServeObj, ret = %d", ret);
      }
   }

   return ret;
}


#if defined(DMP_X_BROADCOM_COM_IGMPSNOOP_1)
/* this function does not care which data model we are using */
static CmsRet getIgmpSnoopingObject(const char *brName, InstanceIdStack *iidStack, IgmpSnoopingCfgObject **igmpSnpObj)
{
   char bridgeIfName[CMS_IFNAME_LENGTH]={0};
   char *fullPath=NULL;
   MdmPathDescriptor pathDesc=EMPTY_PATH_DESCRIPTOR;
   CmsRet ret = CMSRET_SUCCESS;

   /*
    * Convert the Bridge Name (Interface group name), e.g. Default,
    * to the bridgeIfName, e.g. br0.
    */
   dalLan_getBridgeIfNameFromBridgeName(brName, bridgeIfName);

   /* go from brIntfName to igmp snooping object full path */
   ret = qdmMulti_getAssociatedIgmpSnoopingFullPathLocked(bridgeIfName, &fullPath);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("getAssociatedIgmpSnooping on %s failed, ret=%d", bridgeIfName, ret);
      return ret;
   }

   /* go from igmp snooping obj fullpath to path descriptor */
   ret = cmsMdm_fullPathToPathDescriptor(fullPath, &pathDesc);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("fullPathToPathDesc on %s failed, ret=%d", fullPath, ret);
      CMSMEM_FREE_BUF_AND_NULL_PTR(fullPath);
      return ret;
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(fullPath);

   /* get snooping obj */
   ret = cmsObj_get(pathDesc.oid, &pathDesc.iidStack, 0, (void **) igmpSnpObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("get of snoopObj failed, ret=%d", ret);
      return ret;
   }

   *iidStack = pathDesc.iidStack;

   return ret;
}



CmsRet dalLan_setIgmpSnooping(const char *brName, SINT32 enblIgmpSnp, SINT32 enblIgmpMode, SINT32 enblIgmpLanToLanMcast)
{
   IgmpSnoopingCfgObject *igmpSnpObj = NULL; 
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;

   if ((ret = getIgmpSnoopingObject(brName, &iidStack, &igmpSnpObj)) != CMSRET_SUCCESS)
   {
      return ret;
   }

   igmpSnpObj->enable = enblIgmpSnp;

   if (enblIgmpMode == 0)
   {
      CMSMEM_REPLACE_STRING(igmpSnpObj->mode, MDMVS_STANDARD);
   }
   else
   {
      CMSMEM_REPLACE_STRING(igmpSnpObj->mode, MDMVS_BLOCKING);
   }

   igmpSnpObj->lanToLanEnable = enblIgmpLanToLanMcast;

   if ((ret = cmsObj_set(igmpSnpObj, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set IgmpSnpObj, ret = %d", ret);
   }

   cmsObj_free((void **) &igmpSnpObj);
   return ret;
}


void dalLan_getIgmpSnoopingEnabled(const char *brName, char *varValue) 
{
   IgmpSnoopingCfgObject *igmpSnpObj = NULL; 
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;


   if ((ret = getIgmpSnoopingObject(brName, &iidStack, &igmpSnpObj)) != CMSRET_SUCCESS)
   {
      return;
   }


   sprintf(varValue, "%d", igmpSnpObj->enable);

   cmsObj_free((void **) &igmpSnpObj);
   return;
}

void dalLan_getIgmpLanToLanMcastEnable(const char *brName, char *varValue)
{
   IgmpSnoopingCfgObject *igmpSnpObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;

   if ((ret = getIgmpSnoopingObject(brName, &iidStack, &igmpSnpObj)) != CMSRET_SUCCESS)
   {
      return;
   }

   sprintf(varValue, "%d", igmpSnpObj->lanToLanEnable);

   cmsObj_free((void **) &igmpSnpObj);
   return;
}


void dalLan_getIgmpSnoopingMode(const char *brName, char *varValue) 
{
   IgmpSnoopingCfgObject *igmpSnpObj = NULL; 
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;


   if ((ret = getIgmpSnoopingObject(brName, &iidStack, &igmpSnpObj)) != CMSRET_SUCCESS)
   {
      return;
   }


   if( !cmsUtl_strcmp(igmpSnpObj->mode, MDMVS_STANDARD) )
   {
      sprintf(varValue, "0");
   }
   else
   {
      sprintf(varValue, "1");
   }

   cmsObj_free((void **) &igmpSnpObj);
   return;
}
#endif

#ifdef notUsed
/* This function finds out if there is any firewall enable LAN  service.
 * Currently, it is not used
 */
UBOOL8 dalLan_isAnyFirewallEnabled(void)
{
   UBOOL8 enabled = FALSE;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanIpIntfObject *ipIntfObj=NULL;
   CmsRet ret;
   
   while ((enabled == FALSE) &&
          (ret = cmsObj_getNext(MDMOID_LAN_IP_INTF, &iidStack, (void **) &ipIntfObj)) == CMSRET_SUCCESS)
   {
      enabled = ipIntfObj->X_BROADCOM_COM_FirewallEnabled;
      cmsObj_free((void **) &ipIntfObj);
   }

   cmsLog_debug("returning enabled=%d", enabled);

   return enabled;
}
#endif /* notUsed */


/* callers of this function will have to change for TR181 since there is
 * no LANDevice in TR181.  For now, I will leave it out of ifdef DMP_BASELINE_1
 */
CmsRet dalLan_getLanDevByBridgeIfName(const char *brIfName, InstanceIdStack *iidStack, LanDevObject **lanDev)
{
   LanIpIntfObject *ipIntfObj=NULL;
   UBOOL8 found=FALSE;
   CmsRet ret;

   INIT_INSTANCE_ID_STACK(iidStack);

   while (!found &&
          (ret = cmsObj_getNext(MDMOID_LAN_IP_INTF, iidStack, (void **) &ipIntfObj)) == CMSRET_SUCCESS)
   {
      if (!strcmp(ipIntfObj->X_BROADCOM_COM_IfName, brIfName))
      {
         found = TRUE;
      }

      cmsObj_free((void **) &ipIntfObj);
   }

   if (found)
   {
      ret = cmsObj_getAncestor(MDMOID_LAN_DEV, MDMOID_LAN_IP_INTF, iidStack, (void **) lanDev);
   }
   else
   {
      cmsLog_error("could not find brIfName %s", brIfName);
   }

   return ret;
}


#ifdef DMP_BASELINE_1

void dalLan_getBridgeIfNameFromBridgeName_igd(const char *bridgeName __attribute__((unused)), char *bridgeIfName)
{

#ifdef DMP_BRIDGING_1  /* aka SUPPORT_PORT_MAP */
   {
      UINT32 key;
      CmsRet ret;

      if ((ret = dalPMap_getBridgeKey(bridgeName, &key)) != CMSRET_SUCCESS)
      {
         cmsLog_error("could not find bridge name %s, ret=%d", bridgeName, ret);
         return;
      }

      sprintf(bridgeIfName, "br%d", key);
   }

#else

   /* if no bridging, then only br is br0 */
   /* Perhaps we should use the convention that if TR-098 Layer 2 Bridging is not defined,
    * then bridgeName is the bridgeIfName.  See dalLan_getFirstIntfGroupName.
    */
   sprintf(bridgeIfName, "br0");

#endif

}

#endif /* DMP_BASELINE_1 */



#if defined(DMP_BASELINE_1) || defined(SUPPORT_LANVLAN)

CmsRet dalLan_getEthInterface(const char *ifName, InstanceIdStack *iidStack, LanEthIntfObject **lanEthObj)
{
   CmsRet ret;

   while ((ret = cmsObj_getNext(MDMOID_LAN_ETH_INTF, iidStack, (void **) lanEthObj)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp((*lanEthObj)->X_BROADCOM_COM_IfName, ifName))
      {
         /* need to return the object to the caller */
         break;
      }
      cmsObj_free((void **) lanEthObj);
   }

   return ret;
}

#endif /* TR98 BASELINE || LANVLAN */



#ifdef SUPPORT_IPV6
#if defined(DMP_X_BROADCOM_COM_MLDSNOOP_1)
/* this function does not care which data model we are using */
static CmsRet getMldSnoopingObject(const char *brName, InstanceIdStack *iidStack, MldSnoopingCfgObject **mldSnpObj)
{
   char bridgeIfName[CMS_IFNAME_LENGTH]={0};
   char *fullPath=NULL;
   MdmPathDescriptor pathDesc=EMPTY_PATH_DESCRIPTOR;
   CmsRet ret = CMSRET_SUCCESS;

   /*
    * Convert the Bridge Name (Interface group name), e.g. Default,
    * to the bridgeIfName, e.g. br0.
    */
   dalLan_getBridgeIfNameFromBridgeName(brName, bridgeIfName);

   /* go from brIntfName to MLD snooping object full path */
   ret = qdmMulti_getAssociatedMldSnoopingFullPathLocked(bridgeIfName, &fullPath);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("getAssociatedMldSnooping on %s failed, ret=%d", bridgeIfName, ret);
      return ret;
   }

   /* go from MLD snooping obj fullpath to path descriptor */
   ret = cmsMdm_fullPathToPathDescriptor(fullPath, &pathDesc);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("fullPathToPathDesc on %s failed, ret=%d", fullPath, ret);
      CMSMEM_FREE_BUF_AND_NULL_PTR(fullPath);
      return ret;
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(fullPath);

   /* get snooping obj */
   ret = cmsObj_get(pathDesc.oid, &pathDesc.iidStack, 0, (void **)mldSnpObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("get of snoopObj failed, ret=%d", ret);
      return ret;
   }

   *iidStack = pathDesc.iidStack;

   return ret;
}



CmsRet dalLan_setMldSnooping(const char *brName, SINT32 enblMldSnp, SINT32 enblMldMode, SINT32 enblMldLanToLanMcast)
{
   MldSnoopingCfgObject *mldSnpObj = NULL; 
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;

   if ((ret = getMldSnoopingObject(brName, &iidStack, &mldSnpObj)) != CMSRET_SUCCESS)
   {
      return ret;
   }


   mldSnpObj->enable = enblMldSnp;
   if (enblMldMode == 0)
   {
      CMSMEM_REPLACE_STRING(mldSnpObj->mode, MDMVS_STANDARD);
   }
   else
   {
      CMSMEM_REPLACE_STRING(mldSnpObj->mode, MDMVS_BLOCKING);
   }

   mldSnpObj->lanToLanEnable = enblMldLanToLanMcast;

   if ((ret = cmsObj_set(mldSnpObj, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set MldSnpObj, ret = %d", ret);
   }

   cmsObj_free((void **) &mldSnpObj);
   return ret;
}


void dalLan_getMldSnoopingEnabled(const char *brName, char *varValue) 
{
   MldSnoopingCfgObject *mldSnpObj = NULL; 
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;

   if ((ret = getMldSnoopingObject(brName, &iidStack, &mldSnpObj)) != CMSRET_SUCCESS)
   {
      return;
   }


   sprintf(varValue, "%d", mldSnpObj->enable);

   cmsObj_free((void **) &mldSnpObj);
   return;
}

void dalLan_getMldLanToLanMulticastEnable (const char *brName, char *varValue)
{
	MldSnoopingCfgObject *mldSnpObj = NULL;
	InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
	CmsRet ret = CMSRET_SUCCESS;

	if ((ret = getMldSnoopingObject(brName, &iidStack, &mldSnpObj)) != CMSRET_SUCCESS)
	{
	   return;
	}

	sprintf(varValue, "%d", mldSnpObj->lanToLanEnable);

	cmsObj_free((void **) &mldSnpObj);
	return;
}


void dalLan_getMldSnoopingMode(const char *brName, char *varValue)
{
   MldSnoopingCfgObject *mldSnpObj = NULL; 
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;

   if ((ret = getMldSnoopingObject(brName, &iidStack, &mldSnpObj)) != CMSRET_SUCCESS)
   {
      return;
   }


   if( !cmsUtl_strcmp(mldSnpObj->mode, MDMVS_STANDARD) )
   {
      sprintf(varValue, "0");
   }
   else
   {
      sprintf(varValue, "1");
   }

   cmsObj_free((void **) &mldSnpObj);
   return;
}
#endif /* DMP_X_BROADCOM_COM_MLDSNOOP_1 */
#endif /* SUPPORT_IPV6 */


#ifdef SUPPORT_LANVLAN
void dalLan_getLanVlan_igd(WEB_NTWK_VAR *webVar) 
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanEthIntfObject *lanEthObj;
   CmsRet ret = CMSRET_SUCCESS;


   if ((ret = dalLan_getEthInterface(webVar->lanName, &iidStack, &lanEthObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not find ethIfName %s", webVar->lanName);
      return;
   }

   cmsLog_debug("ifName=%s lanVlanEnable=%d lanVlanTagList=%s", 
   	webVar->lanName, lanEthObj->X_BROADCOM_COM_VLAN_Enable, lanEthObj->X_BROADCOM_COM_VLAN_TagList);
   webVar->lanVlanEnable = lanEthObj->X_BROADCOM_COM_VLAN_Enable;
   webVar->lanVlanTagList[0] = '\0';
   if (lanEthObj->X_BROADCOM_COM_VLAN_TagList)
   {
       //format is vid1/pbits1, vid2/pbits2, ...   
       int pos=0;
       UBOOL8 first = TRUE;
       UINT32 m,n;
       char tmpStr[BUFLEN_16];

       while (pos < (int) strlen(lanEthObj->X_BROADCOM_COM_VLAN_TagList))
       {
           if (sscanf(&lanEthObj->X_BROADCOM_COM_VLAN_TagList[pos], "%u/%u", &m, &n) != 2)
           {
               cmsLog_error("Wrong format of lanEthObj->X_BROADCOM_COM_VLAN_TagList, [%s]", lanEthObj->X_BROADCOM_COM_VLAN_TagList);
               break;
           }
           sprintf(tmpStr, "%u/%u", m, n);
           if (first)
           {
               first = FALSE;
           }
           else
           {
               cmsUtl_strcat(webVar->lanVlanTagList, "|");
           }
           cmsUtl_strcat(webVar->lanVlanTagList, tmpStr);
           pos += cmsUtl_strlen(tmpStr);
           while (lanEthObj->X_BROADCOM_COM_VLAN_TagList[pos] != '\0' && 
                (lanEthObj->X_BROADCOM_COM_VLAN_TagList[pos] == ' ' || lanEthObj->X_BROADCOM_COM_VLAN_TagList[pos] == ','))
           {
               pos++;
           }
       }

   }
   cmsLog_debug("ifName=%s lanVlanEnable=%d lanVlanTagList=%s", webVar->lanName, webVar->lanVlanEnable, webVar->lanVlanTagList);
   cmsObj_free((void **) &lanEthObj);
   
}

void dalLan_setLanVlan_igd(WEB_NTWK_VAR *webVar) 
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanEthIntfObject *lanEthObj;
   CmsRet ret = CMSRET_SUCCESS;
   char list[BUFLEN_128]="";


   if ((ret = dalLan_getEthInterface(webVar->lanName, &iidStack, &lanEthObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not find ethIfName %s", webVar->lanName);
      return;
   }
   
   cmsLog_debug("lanName=%s lanVlanEnable=%d lanVlanTagList=%s", 
      webVar->lanName, webVar->lanVlanEnable, webVar->lanVlanTagList);
   
   lanEthObj->X_BROADCOM_COM_VLAN_Enable = webVar->lanVlanEnable;
   
   /*lanVlanTagList web format is vid1/pbits1|vid2/pbits2|...  
      translate to datamodel format vid1/pbits1, vid2/pbits2, ... */
   int pos=0;
   UBOOL8 first = TRUE;
   UINT32 m,n;
   char tmpStr[BUFLEN_16];

   while (pos < (int) strlen(webVar->lanVlanTagList))
   {
       if (sscanf(&webVar->lanVlanTagList[pos], "%u/%u", &m, &n) != 2)
       {
           cmsLog_error("Wrong format of webVar->lanVlanTagList, [%s]", webVar->lanVlanTagList);
           break;
       }
       sprintf(tmpStr, "%u/%u", m, n);
       if (first)
       {
          first = FALSE;
       }
       else
       {
           cmsUtl_strcat(list, ", ");
       }
       cmsUtl_strcat(list, tmpStr);
       pos += cmsUtl_strlen(tmpStr);
       if (webVar->lanVlanTagList[pos] != '\0' && webVar->lanVlanTagList[pos] == '|')
       {
           pos++;
       }
   }
   cmsLog_debug("X_BROADCOM_COM_VLAN_TagList is [%s]", list);
   CMSMEM_REPLACE_STRING_FLAGS(lanEthObj->X_BROADCOM_COM_VLAN_TagList, list, ALLOC_ZEROIZE);

   if ((ret = cmsObj_set(lanEthObj, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not set LAN eth intf object, ret=%d", ret);
   }

   cmsObj_free((void **) &lanEthObj);
   
}

#endif  /* SUPPORT_LANVLAN */



/*
* <:copyright-BRCM:2011:proprietary:standard
* 
*    Copyright (c) 2011 Broadcom 
*    All Rights Reserved
* 
*  This program is the proprietary software of Broadcom and/or its
*  licensors, and may only be used, duplicated, modified or distributed pursuant
*  to the terms and conditions of a separate, written license agreement executed
*  between you and Broadcom (an "Authorized License").  Except as set forth in
*  an Authorized License, Broadcom grants no license (express or implied), right
*  to use, or waiver of any kind with respect to the Software, and Broadcom
*  expressly reserves all rights in and to the Software and all intellectual
*  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
*  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
*  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
* 
*  Except as expressly set forth in the Authorized License,
* 
*  1. This program, including its structure, sequence and organization,
*     constitutes the valuable trade secrets of Broadcom, and you shall use
*     all reasonable efforts to protect the confidentiality thereof, and to
*     use this information only in connection with your use of Broadcom
*     integrated circuit products.
* 
*  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
*     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
*     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
*     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
*     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
*     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
*     PERFORMANCE OF THE SOFTWARE.
* 
*  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
*     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
*     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
*     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
*     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
*     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
*     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
*     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
*     LIMITED REMEDY.
:>
*/

#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"
#include "cms_qdm.h"
#include "dal.h"
#include "dal2_wan.h"
#include "adslctlapi.h" 

#ifdef DMP_DEVICE2_BASELINE_1    /* this file touches TR181 objects */

CmsRet dalWan_removeFailedWanServiceByL2IfName_dev2(const char * ifName)
{
   Dev2EthernetLinkObject *ethLinkObj = NULL;
   char *objFullPath=NULL;;   
   MdmPathDescriptor pathDesc = EMPTY_PATH_DESCRIPTOR;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   char lowerLayersBuf[MDM_SINGLE_FULLPATH_BUFLEN]={0};   
   char ipIntfFullPath[MDM_SINGLE_FULLPATH_BUFLEN]={0};   
   UBOOL8 found = FALSE;
   CmsRet ret;
   
   cmsLog_debug("Enter: Layer2 ifName %s", ifName);

   /* Find the eth.link first */
   while (!found &&
         (ret = cmsObj_getNext(MDMOID_DEV2_ETHERNET_LINK, &iidStack,
                               (void **)&ethLinkObj)) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strcmp(ethLinkObj->name, ifName) == 0)
      {
         found = TRUE;

         /* form a fullpath to this object */
         pathDesc.oid = MDMOID_DEV2_ETHERNET_LINK;
         pathDesc.iidStack = iidStack;
         if ((ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &objFullPath)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Could not form fullpath to %d %s", pathDesc.oid, cmsMdm_dumpIidStack(&pathDesc.iidStack));
            cmsObj_free((void **)&ethLinkObj);
            return ret;
         }

         /* save the lowerlayer pointer */
         cmsLog_debug("Saving eth.link full path %s", objFullPath);
         cmsUtl_strncpy(lowerLayersBuf, objFullPath, sizeof(lowerLayersBuf));
         CMSMEM_FREE_BUF_AND_NULL_PTR(objFullPath);
         
         /* Now delete the eth.link obj */
         if ((ret = cmsObj_deleteInstance(MDMOID_DEV2_ETHERNET_LINK, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Could not delete eth.link at %s, ret=%d",cmsMdm_dumpIidStack(&iidStack), ret);
         }
      }

      cmsObj_free((void **)&ethLinkObj);
   }

   if (!found || ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Error during delete eth.link with %s, found=%d ret=%d",
                    ifName, found, ret);
      return ret;
   }
   

   /* Try to delete eth.vlantermination object if it exists with eth.link fullPth
   * saved in lowerLayersBuf 
   */
   if ((ret = dalEth_deleteEthernetVlanTermination_dev2(lowerLayersBuf)) ==  CMSRET_SUCCESS)
   {
      cmsLog_debug("Deleted EthernetVlanTermination object successfully");
   }
   else if (ret == CMSRET_NO_MORE_INSTANCES)
   {
      cmsLog_notice("Not found EthernetVlanTermination object over %s, ret=%d", lowerLayersBuf, ret);
      /* complain, but keep going */
   }

   /* Try to delete ppp object if it exists with eth.vlan fullPth 
   * saved in lowerLayersBuf 
   */
   if ((ret = dalPpp_deleteIntfObject_dev2(lowerLayersBuf)) ==  CMSRET_SUCCESS)
   {
      cmsLog_debug("Deleted PPP interface object successfully");
   }
   else if (ret == CMSRET_NO_MORE_INSTANCES)
   {
      cmsLog_notice("Not found PPP interface object over %s, ret=%d", lowerLayersBuf, ret);
      /* complain, but keep going */
   }

   /* Now try to delete ip intf full path in lowerLayerBuf */
   if ((ret = dalIp_deleteIntfObject_dev2(lowerLayersBuf, ipIntfFullPath)) ==  CMSRET_SUCCESS)
   {
      cmsLog_debug("Deleted IP interface object successfully");

#ifdef DMP_DEVICE2_DHCPV4CLIENT_1
      /* delete DHCPv4 client (if present) associated with this IP.Interface */
      ret = dalDhcp_deleteIpIntfClient_dev2(ipIntfFullPath);
   
      if (ret != CMSRET_SUCCESS && ret != CMSRET_NO_MORE_INSTANCES)
      {
         cmsLog_error("Error while deleting DHCPv4 client for %s, ret=%d",
                      ipIntfFullPath, ret);
         /* complain, but keep going */
      }
#endif

#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
      /* delete DHCPv6 client (if present) associated with this IP.Interface */
      ret = dalIp_deleteDhcpv6Client_dev2(ipIntfFullPath);
      if (ret != CMSRET_SUCCESS && ret != CMSRET_NO_MORE_INSTANCES)
      {
         cmsLog_error("Error while deleting DHCPv6 client for %s, ret=%d",
                      ipIntfFullPath, ret);
         /* complain, but keep going */
      }
#endif

   }
   else if (ret == CMSRET_NO_MORE_INSTANCES)
   {
      cmsLog_notice("Not found IP interface object over %s, ret=%d", lowerLayersBuf, ret);
      /* complain, but keep going */
   }

   cmsLog_debug("Exit ret %d", ret);
   return ret;
   
}



CmsRet dalWan_addIPv4Service_dev2(const WEB_NTWK_VAR *webVar)
{
   char ipIntfFullPathBuf[MDM_SINGLE_FULLPATH_BUFLEN] = {0};
   char ipIntfLowerLayer[MDM_SINGLE_FULLPATH_BUFLEN]={0};      
   MdmPathDescriptor ipIntfPathDesc;       
   char *layer2FullPath=NULL;
   char ethLinkPathRef[MDM_SINGLE_FULLPATH_BUFLEN]={0};
   char ethVlanPathRef[MDM_SINGLE_FULLPATH_BUFLEN]={0};
   UBOOL8 isPPP=FALSE;
   UBOOL8 isBridge=FALSE;
   char pppPathRef[MDM_SINGLE_FULLPATH_BUFLEN]={0};   
   UBOOL8 supportIpv4;
   UBOOL8 supportIpv6;
   UBOOL8 enableIgmp = FALSE;
   UBOOL8 enableIgmpSource = FALSE;
   UBOOL8 enableMld = FALSE;
   UBOOL8 enableMldSource = FALSE;
   CmsRet ret = CMSRET_SUCCESS;
   char wanL2IfName[CMS_IFNAME_LENGTH]={0};
   char *p;


   cmsLog_debug("Entered: ifName=%s, protocol=%d enblIpVer=%d",
                webVar->wanL2IfName, webVar->ntwkPrtcl, webVar->enblIpVer);


   /* set IP version */
   if (webVar->enblIpVer == ENBL_IPV4_ONLY)
   {
      supportIpv4 = TRUE;
      supportIpv6 = FALSE;
   }
   else if (webVar->enblIpVer == ENBL_IPV6_ONLY)
   {
      cmsLog_debug("This function does not handle IPv6 only, just return");
      return CMSRET_SUCCESS;
   }
   else
   {
      supportIpv4 = TRUE;
      supportIpv6 = TRUE;
   }


   /* set wan service type */
   isPPP = dalWan_isPPPWanConnection_dev2(webVar);

   isBridge = (webVar->ntwkPrtcl == CMS_WAN_TYPE_BRIDGE ||
               webVar->ntwkPrtcl == CMS_WAN_TYPE_ETHERNET_BRIDGE);


   strncpy(wanL2IfName, webVar->wanL2IfName, sizeof(wanL2IfName)-1);
   
   if ((p = strchr(wanL2IfName, '/')))
   {
      /* now wanL2IfName is "ptm0", "atm0" etc. */
      *p = '\0';
   }
   else if (cmsUtl_strstr(wanL2IfName, ETH_IFC_STR) ||
      cmsUtl_strstr(wanL2IfName, MOCA_IFC_STR) ||
      cmsUtl_strstr(wanL2IfName, GPON_IFC_STR) ||
      cmsUtl_strstr(wanL2IfName, EPON_IFC_STR) ||
      cmsUtl_strstr(wanL2IfName, WLAN_IFC_STR))
   {
      /* do nothing for now */
   }
   else
   {
      cmsLog_error("wanL2IfName %s - wrong format", wanL2IfName);
      return ret;
   }

   cmsLog_debug("wanL2IfName %s isBridge=%d", wanL2IfName, isBridge);

   /* First get the fullpath to the layer 2 interface name. */
   {
      UBOOL8 layer2 = TRUE;

      if ((ret = qdmIntf_intfnameToFullPathLocked(wanL2IfName, layer2, &layer2FullPath)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get fullpath of %s, ret=%d", wanL2IfName, ret);
         return ret;
      }
      cmsLog_debug("wanL2IfName %s ==> L2FullPath %s", wanL2IfName, layer2FullPath);
   }


   /* Create the Ethernet.Link object */
   /* XXX TODO: homeplug and wifi.SSID have mac address already, so do we
    * need a separate Ethernet.Link object on top of that?  Is the
    * Ethernet.Link object for the IP.Interface object or the layer 2
    * interface object? */
   if (    (webVar->ntwkPrtcl == CMS_WAN_TYPE_PPPOA)
        || (webVar->ntwkPrtcl == CMS_WAN_TYPE_IPOA )
      )
   {
      /* For IPoA/PPPoA link lowLayer to ATM directly */
      cmsUtl_strncpy(ethVlanPathRef, layer2FullPath, sizeof(ethVlanPathRef));

      CMSMEM_FREE_BUF_AND_NULL_PTR(layer2FullPath);
   }
   else
   {
      /* For EoA */

      ret = dalEth_addEthernetLink_dev2(layer2FullPath, ethLinkPathRef, sizeof(ethLinkPathRef));

      CMSMEM_FREE_BUF_AND_NULL_PTR(layer2FullPath);

      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("dalEth_addEthernetLink_dev2 failed, ret = %d", ret);
         return ret;      
      }


      /* For now, always create ethernet.vlantermination object since our TR98
      * always has this even there is no vlan id to set.
      */
      ret = dalEth_addEthernetVlanTermination_dev2(webVar,
                                                   ethLinkPathRef,
                                                   ethVlanPathRef,
                                                   sizeof(ethVlanPathRef));
   }

   if (ret != CMSRET_SUCCESS)
   {
      dalWan_removeFailedWanServiceByL2IfName_dev2(wanL2IfName);
      cmsLog_error("dalEth_addEthernetVlanTermination_dev2 failed, ret = %d", ret);
      return ret;
   }

   /* save ethVlanPathRef as ipIntfLowerLayer for non ppp model */
   cmsUtl_strncpy(ipIntfLowerLayer, ethVlanPathRef, sizeof(ipIntfLowerLayer));


   /*
   *  PPP interface if exist, is between ethernet.vlantermination and ip interface.
   */
   if (isPPP)
   {
      if ((ret = dalPpp_addIntfObject_dev2(webVar, 
                                           supportIpv4,
                                           supportIpv6,
                                           ethVlanPathRef,
                                           pppPathRef,
                                           sizeof(pppPathRef))) != CMSRET_SUCCESS)
      {
         cmsLog_error("dalPpp_addPPPIntfObject_dev2 failed, ret %d", ret);
         dalWan_removeFailedWanServiceByL2IfName_dev2(wanL2IfName);
         return ret;
      }
      /* Use pppPathRef as ipIntfLowerLayer now for ppp WAN */
      cmsUtl_strncpy(ipIntfLowerLayer, pppPathRef, sizeof(ipIntfLowerLayer));
   }


#ifdef DMP_X_BROADCOM_COM_IGMP_1
   enableIgmp = (UBOOL8) webVar->enblIgmp;
   enableIgmpSource = (UBOOL8) webVar->enblIgmpMcastSource;
#endif

#ifdef DMP_X_BROADCOM_COM_MLD_1
   enableMld = (UBOOL8) webVar->enblMld;
   enableMldSource = (UBOOL8) webVar->enblMldMcastSource;
#endif

   /*  todo: use X_BROADCOM_COM_ServiceName. */
   if ((ret = dalIp_addIntfObject_dev2(supportIpv4,
                                       supportIpv6, 
                                       NULL,  /* intfGroupName */
                                       isBridge,
                                       (isBridge ? "br0" : NULL),
                                       webVar->enblFirewall,
                                       enableIgmp, enableIgmpSource,
                                       enableMld, enableMldSource,
                                       ipIntfLowerLayer,
                                       ipIntfFullPathBuf,
                                       sizeof(ipIntfFullPathBuf),
                                       &ipIntfPathDesc)) != CMSRET_SUCCESS)
   {
      cmsLog_error("dalIp_addIntfObject_dev2 failed. ret=%d", ret);
      dalWan_removeFailedWanServiceByL2IfName_dev2(wanL2IfName);
      return ret;
   }


   if (isBridge)
   {
#ifdef DMP_DEVICE2_BRIDGE_1
      /* Need to add a bridge port and join br0 */
      ret = dalBridge_addFullPathToBridge_dev2(ethVlanPathRef, "br0");
#endif

      /* Bridge does not have any higher layer services, so return now */
      return ret;
   }


   /* Now need add/set objects related to this ip interface  */
   
   /*
   * Add and set the ipv4 address object 
   */ 
   if (!isPPP)
   {
      /* if webVar->wanIpAddress is "0.0.0.0", it is dynamic WAN service */
      UBOOL8 isDynamicWan = !cmsUtl_strcmp(webVar->wanIpAddress, "0.0.0.0");

      /* IPoE */
      if (isDynamicWan)
      {
         /* Dynamic IPoE WAN needs dhcpc */
         if ((ret = dalDhcp_addIpIntfClient_dev2(ipIntfFullPathBuf,
                                                 webVar->dhcpcOp60VID,
                                                 webVar->dhcpcOp61DUID,
                                                 webVar->dhcpcOp61IAID,
                                                 webVar->dhcpcOp77UID,
                                                 webVar->dhcpcOp125Enabled,
                                                 webVar->dhcpcOp50IpAddress,
                                                 webVar->dhcpcOp54ServerIpAddress,
                                                 webVar->dhcpcOp51LeasedTime)) != CMSRET_SUCCESS)
         {
            cmsLog_error("dalDhcp_addIpIntfClient_dev2 failed. ret=%d", ret);
         }   
      }
      else
      {
         /* Static WAN.  
         * Need to create ipv4 address and routing.router.forwarding objects to hold the static ip info. 
         */
         if ((ret = dalIp_addIpIntfIpv4Address_dev2(&(ipIntfPathDesc.iidStack),
                                    webVar->wanIpAddress,
                                    webVar->wanSubnetMask)) != CMSRET_SUCCESS)
         {
            cmsLog_error("dalIp_addIpIntfIpv4Address_dev2 failed. ret=%d", ret);
         }

#ifdef DMP_DEVICE2_ROUTING_1
         if (ret == CMSRET_SUCCESS)
         {
            ret = dalRt_addStaticIpv4DefaultGateway_dev2(webVar->wanIntfGateway,
                                                         ipIntfFullPathBuf);
            if (ret != CMSRET_SUCCESS)
            {
               cmsLog_error("add static gw %s on %s failed, ret=%d",
                            webVar->wanIntfGateway, ipIntfFullPathBuf, ret);
            }
         }
#endif
      }
   }

#ifdef DMP_DEVICE2_ROUTING_1
    if (ret == CMSRET_SUCCESS)
    {
       ret = dalRt_setDefaultGatewayList_dev2(webVar->defaultGatewayList);
       if (ret != CMSRET_SUCCESS)
       {
          cmsLog_error("failed to add defaultGatewayList %s", webVar->defaultGatewayList);
       }
    }
#endif

    if (ret != CMSRET_SUCCESS)
    {
       dalWan_removeFailedWanServiceByL2IfName_dev2(wanL2IfName);
       return ret;
    }

   /*
    * The algorithm for adding of DNS servers is independent of whether
    * this is a DHCP client over IPoE, or PPP, or static DNS, or
    * dynamically acquired DNS.  All this algorithm needs to know is
    * whether the user set a static DNS or whether a list of DNS ifNames
    * was provided.  If static DNS was specified, use it.  If DNS ifNames
    * was given, use it (and delete the static DNS).  See
    * dalWan_setDefaultGatewayAndDns.
    */
   if (!IS_EMPTY_STRING(webVar->dnsPrimary) &&
       cmsUtl_strncmp(webVar->dnsPrimary, "0.0.0.0", 7))
   {
      ret = dalDns_setStaticIpvxDnsServers(CMS_AF_SELECT_IPV4,
                                           webVar->dnsPrimary,
                                           webVar->dnsSecondary);

      /* IPv6 static DNS servers are stored in dns6Pri and dns6Sec */
   }
   else if (!IS_EMPTY_STRING(webVar->dnsIfcsList))
   {
      /* set this list of ifNames to search for default system DNS */
      /*
       * If we want to use DnsIfNameList, we must delete any static DNS
       * servers because static DNS servers have higher precedence.
       */
      dalDns_deleteAllStaticIpvxDnsServers(CMS_AF_SELECT_IPV4);
      ret = dalDns_setIpvxDnsIfNameList(CMS_AF_SELECT_IPV4, webVar->dnsIfcsList);

      /* IPv6 does not have a list, only a single intf name stored in dns6Ifc */
   }
   else
   {
      cmsLog_error("Not enough dns info, ifcsList=%s primary=%s",
            webVar->dnsIfcsList, webVar->dnsPrimary);
      ret = CMSRET_INVALID_PARAM_VALUE;
   }


   /* Add Device2.NAT if it is IPoE with NAT enabled or it is PPP which is always has the NAT enabled */
   if (ret != CMSRET_SUCCESS )
   {
      cmsLog_error("Add DNS failed, undo previous objects. ret=%d", ret);
   }
   else
   {
      if ((ret = dalNat_set_dev2(ipIntfFullPathBuf,webVar->enblNat,webVar->enblFullcone)) != CMSRET_SUCCESS)
      {
         cmsLog_error("dalNat_set_dev2 failed. ret=%d", ret);
      }
      else if ((ret = dalPcp_set_dev2(ipIntfFullPathBuf, webVar->pcpMode,
                                      webVar->pcpServer)) != CMSRET_SUCCESS)
      {
         cmsLog_error("dalPcp_set_dev2 failed. ret=%d", ret);
      }
   }

   if (ret != CMSRET_SUCCESS)
   {
      dalWan_removeFailedWanServiceByL2IfName_dev2(wanL2IfName);
   }
   
   cmsLog_debug("Exist ret %d", ret);
   
   return ret;
   
}


/*
 * Add WAN service functions
 */
CmsRet dalWan_addService_dev2(const WEB_NTWK_VAR *webVar)
{
   CmsRet ret = CMSRET_SUCCESS;
   
   if ((ret =  dalWan_addIPv4Service(webVar)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed dalWan_addIPv4Service, ret %d", ret);
      return ret;
   }
   
#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
   if (webVar->enblIpVer != ENBL_IPV4_ONLY)
   {
      /* now create the IPv6 related wan service objects */
      if ((ret =  dalWan_addIPv6Service(webVar)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed dalWan_addIPv6Service, ret %d", ret);
      }
   }
#endif /* DMP_X_BROADCOM_COM_DEV2_IPV6_1 */

   return ret;
}



/*
 * Delete TR181 WAN service.  This function is called for Hybrid IPv6
 * and Pure181 modes.
 */
CmsRet dalWan_deleteService_dev2(const WEB_NTWK_VAR *webVar)
{
   MdmPathDescriptor pathDesc = EMPTY_PATH_DESCRIPTOR;
   char lowerLayersBuf[MDM_SINGLE_FULLPATH_BUFLEN]={0};
   char savedIntfNameBuf[CMS_IFNAME_LENGTH]={0};
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   InstanceIdStack ipIntfIidStack = EMPTY_INSTANCE_ID_STACK;
   char *ipIntfFullPath=NULL;
   UBOOL8 found = FALSE;
   CmsRet ret;
   
   cmsLog_debug("Enter: webVar->wanIfName %s", webVar->wanIfName);

   /*
    * Most of the delete operation applies to IPv4 and IPv6 at the same
    * time, so they can be done in this function.  Only call out to other
    * functions to delete IPv4 or IPv6 specific objects.
    */

   /* find the IP.Interface */
   while (!found &&
          (ret = cmsObj_getNext(MDMOID_DEV2_IP_INTERFACE, &ipIntfIidStack,
                                    (void **)&ipIntfObj)) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strcmp(ipIntfObj->name, webVar->wanIfName) == 0)
      {
         found = TRUE;

         /* form a fullpath to this IP.Interface object */
         pathDesc.oid = MDMOID_DEV2_IP_INTERFACE;
         pathDesc.iidStack = ipIntfIidStack;
         ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &ipIntfFullPath);
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("Could not form fullpath to %d %s",
                      pathDesc.oid, cmsMdm_dumpIidStack(&pathDesc.iidStack));
            /* complain, but keep going */
         }

         /* if Bridge WAN service, save the br intf name */
         if (ipIntfObj->X_BROADCOM_COM_BridgeService)
         {
            cmsUtl_strncpy(savedIntfNameBuf, ipIntfObj->name, sizeof(savedIntfNameBuf));
         }

         /* save the lowerlayer pointer */
         cmsLog_debug("saving lowerlayer %s", ipIntfObj->lowerLayers);
         cmsUtl_strncpy(lowerLayersBuf, ipIntfObj->lowerLayers, sizeof(lowerLayersBuf));

#ifdef DMP_DEVICE2_NAT_1
         /* delete NAT port mapping (virtual server, dmz)
            (if present) associated with this IP.Interface */
         ret = dalNat_deletePortMapping_dev2(ipIntfFullPath);
         if (ret != CMSRET_SUCCESS && ret != CMSRET_NO_MORE_INSTANCES)
         {
            cmsLog_error("Error while deleting NAT port mapping setting for %s, ret=%d",
                         ipIntfFullPath, ret);
            /* complain, but keep going */
         }

         /* delete NAT interface setting, port triggering
            (if present) associated with this IP.Interface */
         ret = dalNat_deleteIntfSetting_dev2(ipIntfFullPath);
         if (ret != CMSRET_SUCCESS && ret != CMSRET_NO_MORE_INSTANCES)
         {
            cmsLog_error("Error while deleting NAT interface setting for %s, ret=%d",
                         ipIntfFullPath, ret);
            /* complain, but keep going */
         }
#endif

         /* Now delete the IP.Interface obj, which will move the IPv4 and
          * IPv6 state machines to ServiceDown.
          */
         ret = cmsObj_deleteInstance(MDMOID_DEV2_IP_INTERFACE, &ipIntfIidStack);
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("Could not delete IP.Interface at %s, ret=%d",
                          cmsMdm_dumpIidStack(&ipIntfIidStack), ret);
         }
      }

      cmsObj_free((void **)&ipIntfObj);
   }

   /* For hybrid IPv4 only configuration, found would be FALSE
   * and just return
   */
   if (!found && webVar->enblIpVer == ENBL_IPV4_ONLY)
   {
      cmsLog_debug("Not delete %s since it is a hybrid IPv4 only interface",  webVar->wanIfName);
      return ret;
   }

   if (ret != CMSRET_SUCCESS)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);
      cmsLog_error("Failed to delete %s, ret %d",  webVar->wanIfName, ret);
   }
   

#ifdef DMP_DEVICE2_DHCPV4CLIENT_1
   /* delete DHCPv4 client (if present) associated with this IP.Interface */
   ret = dalDhcp_deleteIpIntfClient_dev2(ipIntfFullPath);
   if (ret != CMSRET_SUCCESS && ret != CMSRET_NO_MORE_INSTANCES)
   {
      cmsLog_error("Error while deleting DHCPv4 client for %s, ret=%d",
                   ipIntfFullPath, ret);
      /* complain, but keep going */
   }
#endif

   /* delete statically configured IPv4 default route (if any) */
   dalRt_deleteStaticIpv4DefaultGateway_dev2(ipIntfFullPath);

#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
   /* delete DHCPv6 client (if present) associated with this IP.Interface */
   ret = dalIp_deleteDhcpv6Client_dev2(ipIntfFullPath);
   if (ret != CMSRET_SUCCESS && ret != CMSRET_NO_MORE_INSTANCES)
   {
      cmsLog_error("Error while deleting DHCPv6 client for %s, ret=%d",
                   ipIntfFullPath, ret);
      /* complain, but keep going */
   }
#endif

#ifdef SUPPORT_RIP
   /* delete rip interface setting (if present) associated with this IP.Interface */
   ret = dalRip_deleteRipInterfaceSetting_dev2(ipIntfFullPath);
   if (ret != CMSRET_SUCCESS && ret != CMSRET_NO_MORE_INSTANCES)
   {
      cmsLog_error("Error while deleting rip interface setting for %s, ret=%d",
                   ipIntfFullPath, ret);
      /* complain, but keep going */
   }
#endif

   CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);


   /*
    * Now we can do some generic code, just walk down the lowerlayers pointer
    * and delete until we hit a layer 2 interface (cannot delete that).
    * Typically, we expect to delete PPP (maybe), Vlan (maybe),
    * and finally Ethernet.Link.
    */
   ret = CMSRET_SUCCESS;
   while (!qdmIntf_isFullPathLayer2Locked_dev2(lowerLayersBuf) &&
          ret == CMSRET_SUCCESS)
   {
      cmsLog_debug("deleting %s", lowerLayersBuf);

      ret = cmsMdm_fullPathToPathDescriptor(lowerLayersBuf, &pathDesc);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("could not convert %s to pathDesc, ret=%d",
                       lowerLayersBuf, ret);
         break;
      }

      /* get the lowerlayer value of this object before deleting it */
      memset(lowerLayersBuf, 0, sizeof(lowerLayersBuf));
      ret = qdmIntf_getLowerLayersFromPathDescLocked_dev2(&pathDesc,
                                  lowerLayersBuf, sizeof(lowerLayersBuf));
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not get lowerLayers from %d %s, ret=%d",
                pathDesc.oid, cmsMdm_dumpIidStack(&pathDesc.iidStack), ret);
      }
      else
      {
         cmsLog_debug("next lowerLayer = %s", lowerLayersBuf);
         if (strstr(lowerLayersBuf, ","))
         {
            cmsLog_error("cannot handle multiple lowerlayer intfs (%s)", lowerLayersBuf);
            ret = CMSRET_INVALID_ARGUMENTS;
         }
      }

      /* delete the object */
      ret = cmsObj_deleteInstance(pathDesc.oid, &pathDesc.iidStack);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not delete %d %s, ret=%d",
                      pathDesc.oid, cmsMdm_dumpIidStack(&pathDesc.iidStack), ret);
      }
   }

#ifdef DMP_DEVICE2_BRIDGE_1
   /*
    * If this is a Bridge WAN service, delete the association with
    * the LAN side bridge now.
    */
   if (ret == CMSRET_SUCCESS &&
       !IS_EMPTY_STRING(savedIntfNameBuf))
   {
      dalBridge_deleteIntfNameFromBridge_dev2(savedIntfNameBuf);
   }
#endif

   cmsLog_debug("exit, ret=%d", ret);

   return ret;
}



CmsRet dalWan_getWanConInfoForEdit_dev2(WEB_NTWK_VAR *webVar)
{
   UBOOL8 isPpp = FALSE;
   CmsRet ret = CMSRET_OBJECT_NOT_FOUND;
   UBOOL8 found = FALSE;
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   MdmPathDescriptor pathDesc;
   char *ipIntfFullPath=NULL;
   char ipIntfFullPathBuf[MDM_SINGLE_FULLPATH_BUFLEN]={0};
   
   cmsLog_debug("Enter %s", webVar->wanIfName);
   while (!found  &&
          (ret = cmsObj_getNext(MDMOID_DEV2_IP_INTERFACE, 
                                &iidStack,
                                (void **)&ipIntfObj)) == CMSRET_SUCCESS)
   {
      if (ipIntfObj->X_BROADCOM_COM_Upstream &&
         !ipIntfObj->X_BROADCOM_COM_BridgeService &&
         !cmsUtl_strcmp(ipIntfObj->name, webVar->wanIfName))
      {
         found = TRUE;
      }
      else
      {
         cmsObj_free((void **) &ipIntfObj);
      }         
   }


   if (!found)
   {
      cmsLog_error("Cannot find WAN IP interface %s for editing?", webVar->wanIfName);
      return CMSRET_INTERNAL_ERROR;
   }
   else
   {
      INIT_PATH_DESCRIPTOR(&pathDesc);
      pathDesc.oid = MDMOID_DEV2_IP_INTERFACE;
      pathDesc.iidStack = iidStack;
      if ((ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &ipIntfFullPath)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsMdm_pathDescriptorToFullPath returns error. ret=%d", ret);
         return ret;
      }
      cmsUtl_strncpy(ipIntfFullPathBuf, ipIntfFullPath, sizeof(ipIntfFullPathBuf));
      CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);      
   }
   if (cmsUtl_strstr(ipIntfObj->lowerLayers, "Device.PPP."))  /* a bit of a hack, but it works */
   {
      isPpp = TRUE;
   }
   
      
   if (!isPpp)
   {
      if (ipIntfObj->alias != NULL)
      {
         cmsUtl_strcpy(webVar->serviceName, ipIntfObj->alias);
      }  
      
      webVar->enblNat = qdmIpIntf_isNatEnabledOnIpIntfFullPathLocked_dev2(ipIntfFullPathBuf);
      webVar->enblFullcone = qdmIpIntf_isFullConeNatEnabledOnIpIntfFullPathLocked_dev2(ipIntfFullPathBuf);
      webVar->enblFirewall = ipIntfObj->X_BROADCOM_COM_FirewallEnabled;
#ifdef DMP_X_BROADCOM_COM_IGMP_1
      webVar->enblIgmp = ipIntfObj->X_BROADCOM_COM_IGMPEnabled;
      webVar->enblIgmpMcastSource = ipIntfObj->X_BROADCOM_COM_IGMP_SOURCEEnabled;
#endif
/* TODO:       webVar->pcpMode = ipIntfObj->X_BROADCOM_COM_PCPMode; 
 *
 *      webVar->enblIpVer = 1 - (ipIntfObj->X_BROADCOM_COM_IPv6Enabled ? 0 : 1) + (ipIntfObj->X_BROADCOM_COM_IPv4Enabled ? 0 : 1);
 */ 
#ifdef DMP_X_BROADCOM_COM_GPONWAN_1
      webVar->noMcastVlanFilter = ipIntfObj->X_BROADCOM_COM_NoMcastVlanFilter;
#endif
#ifdef DMP_X_BROADCOM_COM_MLD_1
      webVar->enblMld = ipIntfObj->X_BROADCOM_COM_MLDEnabled;
      webVar->enblMldMcastSource= ipIntfObj->X_BROADCOM_COM_MLD_SOURCEEnabled;
#endif

      /* if wanIpConn->externalIPAddress (webVar->wanIpAddress,) is "0.0.0.0", it is dynamic MER */
      if (!qdmIpIntf_isStaticWanLocked_dev2(ipIntfObj->name))
      {
         webVar->enblDhcpClnt = TRUE;
         cmsUtl_strcpy(webVar->wanIpAddress, "0.0.0.0");
      }
      else
      {
         char defaultGateway[CMS_IFNAME_LENGTH] = {0};
         
         webVar->enblDhcpClnt = FALSE; 
         qdmRt_getGatewayIpv4AddrByFullPathLocked_dev2(ipIntfFullPathBuf, defaultGateway);
         if (!IS_EMPTY_STRING(defaultGateway))
         {
            cmsUtl_strcpy(webVar->wanIntfGateway, defaultGateway);
         }
      }
   }
   else 
   {
      Dev2PppInterfaceObject *pppIntfObj = NULL;
      Dev2PppInterfacePpoeObject *pppoeObj = NULL;      
      CmsRet ret = CMSRET_SUCCESS;
      
      INIT_INSTANCE_ID_STACK(&iidStack);
      found = FALSE;
      
      /* Look thru  all pppoe interfaces and check for the lowerlayer matches with
      * the layer 2 interface (vlantermation here).  If found, get the pppoe object for
      * X_BROADCOM_COM_AddPppToBridge and other params.
      */
      while (!found &&
             (cmsObj_getNextFlags(MDMOID_DEV2_PPP_INTERFACE, &iidStack,
                                  OGF_NO_VALUE_UPDATE,
                                  (void **)&pppIntfObj) == CMSRET_SUCCESS))
      {
         if (!cmsUtl_strcmp(pppIntfObj->name, webVar->wanIfName))
         {
            InstanceIdStack pppoeIidStack = EMPTY_INSTANCE_ID_STACK;
                  
            if ((ret = cmsObj_getNextInSubTree(MDMOID_DEV2_PPP_INTERFACE_PPOE, 
                                                   &iidStack, 
                                                   &pppoeIidStack,
                                                   (void **) &pppoeObj)) != CMSRET_SUCCESS)
            {
               cmsLog_error("Failed to get pppoeObj, error=%d", ret);
               cmsObj_free((void **)&pppIntfObj);
               return ret;
            }
            
            found = TRUE;
         }
         else
         {
            cmsObj_free((void **)&pppIntfObj);
         }
      }

      if (!found)
      {
         cmsLog_error("Failed to find the matching pppIntfObj");
         return CMSRET_INTERNAL_ERROR;
      }
 
      if (pppIntfObj->username != NULL)
      {
         cmsUtl_strcpy(webVar->pppUserName, pppIntfObj->username);
      }
      if (pppIntfObj->password != NULL)
      {
         cmsUtl_strcpy(webVar->pppPassword, pppIntfObj->password);
      }
      if (pppIntfObj->alias!= NULL)
      {
         cmsUtl_strcpy(webVar->serviceName, pppIntfObj->alias);
      }
      if (pppoeObj->serviceName != NULL)
      {
         cmsUtl_strcpy(webVar->pppServerName, pppoeObj->serviceName);
      }
      
#ifdef DMP_X_BROADCOM_COM_IGMP_1
      webVar->enblIgmp = ipIntfObj->X_BROADCOM_COM_IGMPEnabled;
      webVar->enblIgmpMcastSource = ipIntfObj->X_BROADCOM_COM_IGMP_SOURCEEnabled;
#endif
/* Still need?  if not, need to delete
webVar->enblIpVer = 1 - (pppIntfObj->X_BROADCOM_COM_IPv6Enabled ? 0 : 1) + (pppIntfObj->X_BROADCOM_COM_IPv4Enabled ? 0 : 1);
*/

#ifdef DMP_X_BROADCOM_COM_GPONWAN_1
      webVar->noMcastVlanFilter = pppIntfObj->X_BROADCOM_COM_NoMcastVlanFilter;
#endif
#ifdef DMP_X_BROADCOM_COM_MLD_1
      webVar->enblMld = ipIntfObj->X_BROADCOM_COM_MLDEnabled;
      webVar->enblMldMcastSource = ipIntfObj->X_BROADCOM_COM_MLD_SOURCEEnabled;
#endif
      webVar->enblService = 1;  /* always enable service even though data model default is false */

      if (pppoeObj->X_BROADCOM_COM_IPExtension)
      {
        webVar->pppIpExtension = TRUE;
      }
      else
      {
         webVar->enblFullcone = qdmIpIntf_isFullConeNatEnabledOnIpIntfFullPathLocked_dev2(ipIntfFullPathBuf);         
         webVar->enblFirewall = ipIntfObj->X_BROADCOM_COM_FirewallEnabled;
         /* TODO        webVar->pcpMode = pppConn->X_BROADCOM_COM_PCPMode;         */
      }
      
      webVar->pppToBridge = pppoeObj->X_BROADCOM_COM_AddPppToBridge;
      if (pppIntfObj->X_BROADCOM_COM_LocalIPAddress != NULL)
      {
         cmsUtl_strcpy(webVar->pppLocalIpAddress, pppIntfObj->X_BROADCOM_COM_LocalIPAddress);
      }
      else
      {
         cmsUtl_strcpy(webVar->pppLocalIpAddress, "0.0.0.0");
      }
      
      webVar->pppAuthMethod = cmsUtl_pppAuthToNum(pppIntfObj->authenticationProtocol);
      
      if (pppIntfObj->idleDisconnectTime != 0)
      {
         webVar->enblOnDemand = TRUE;
         /* get on demand ideltime out (seconds in mdm) */
         webVar->pppTimeOut = pppIntfObj->idleDisconnectTime / 60;
      }
      else
      {
         webVar->enblOnDemand = FALSE;
         webVar->pppTimeOut = 0;
      }

      if (pppIntfObj->X_BROADCOM_COM_UseStaticIPAddress)
      {
         webVar->useStaticIpAddress = TRUE;
         cmsUtl_strcpy(webVar->pppLocalIpAddress, pppIntfObj->X_BROADCOM_COM_LocalIPAddress);
      }
      else
      {
         webVar->useStaticIpAddress = FALSE;
      }
      webVar->enblPppDebug = pppIntfObj->X_BROADCOM_COM_Enable_Debug;

      cmsObj_free((void **)&pppoeObj); 
      cmsObj_free((void **)&pppIntfObj); 
  
   }

   cmsObj_free((void **) &ipIntfObj);

   cmsLog_debug("Exit Edit. ret=%d", ret);

   return ret;

}


CmsRet dalWan_enableDisablePPPIntfObj_dev2(const char *ipIntfName, UBOOL8 enableFlag)
{
   Dev2PppInterfaceObject *pppIntfObj = NULL;
   InstanceIdStack pppIntfiidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 foundPPPIntf = FALSE;
   
   /* Look thru  all pppoe interfaces and check for the lowerlayer matches with
   * the layer 2 interface (vlantermation here).  If found, get the pppoe object for
   * X_BROADCOM_COM_AddPppToBridge and other params.
   */
   while (!foundPPPIntf &&
          (cmsObj_getNextFlags(MDMOID_DEV2_PPP_INTERFACE, &pppIntfiidStack,
                               OGF_NO_VALUE_UPDATE,
                               (void **)&pppIntfObj) == CMSRET_SUCCESS))
   {
      if (!cmsUtl_strcmp(ipIntfName, pppIntfObj->name))
      {
         pppIntfObj->enable = enableFlag;
         foundPPPIntf = TRUE;
         
         if ((ret = cmsObj_set(pppIntfObj, &pppIntfiidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Failed to set pppIntfObj. ret=%d", ret);
         }
      }         
      cmsObj_free((void **) &pppIntfObj); 
   }

   cmsLog_debug("Exit ret=%d", ret);

   return ret;

}




CmsRet dalWan_enableDisableIpIntfObj_dev2(const char *ifName, UBOOL8 enableFlag)
{
   Dev2IpInterfaceObject *ipIntf=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;   
   UBOOL8 found=FALSE; 
   CmsRet ret;
   
   cmsLog_debug("Enter.");
   
   while (!found &&
          (ret = cmsObj_getNextFlags(MDMOID_DEV2_IP_INTERFACE, 
                                     &iidStack,
                                     OGF_NO_VALUE_UPDATE, 
                                     (void **) &ipIntf)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(ipIntf->name, ifName))
      {
         ipIntf->enable = enableFlag;
         if ((ret = cmsObj_set(ipIntf, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Failed to set ipIntf obj. ret=%d", ret);
         } 
         found = TRUE;
      }
      cmsObj_free((void **) &ipIntf);
   }

   cmsLog_debug("Exit, ret=%d", ret);

   return ret;
}


CmsRet dalWan_editInterface_dev2(const WEB_NTWK_VAR *webVar)
{
   UBOOL8 isPpp = FALSE;
   CmsRet ret = CMSRET_OBJECT_NOT_FOUND;
   UBOOL8 found = FALSE;
   UBOOL8 change = FALSE;
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   MdmPathDescriptor pathDesc;
   char *ipIntfFullPath=NULL;
   char ipIntfFullPathBuf[MDM_SINGLE_FULLPATH_BUFLEN]={0};
   
   while (!found  &&
          (ret = cmsObj_getNext(MDMOID_DEV2_IP_INTERFACE, 
                                &iidStack,
                                (void **)&ipIntfObj)) == CMSRET_SUCCESS)
   {
   
      if (ipIntfObj->X_BROADCOM_COM_Upstream &&
         !ipIntfObj->X_BROADCOM_COM_BridgeService &&
         !cmsUtl_strcmp(ipIntfObj->name, webVar->wanIfName))
      {
         found = TRUE;
      }
      else
      {
         cmsObj_free((void **) &ipIntfObj);
      }         
   }


   if (!found)
   {
      cmsLog_error("Cannot find WAN IP interface %s for editing?", webVar->wanIfName);
      return CMSRET_INTERNAL_ERROR;
   }
   else
   {
      INIT_PATH_DESCRIPTOR(&pathDesc);
      pathDesc.oid = MDMOID_DEV2_IP_INTERFACE;
      pathDesc.iidStack = iidStack;

      if ((ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &ipIntfFullPath)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsMdm_pathDescriptorToFullPath returns error. ret=%d", ret);
         cmsObj_free((void **) &ipIntfObj);
         return ret;
      }
   }

   cmsUtl_strncpy(ipIntfFullPathBuf, ipIntfFullPath, sizeof(ipIntfFullPathBuf));
   CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);
   
   if (cmsUtl_strstr(ipIntfObj->lowerLayers, "Device.PPP."))  /* a bit of a hack, but it works */
   {
      isPpp = TRUE;
   }


   if (isPpp)
   {
      Dev2PppInterfaceObject *pppIntfObj = NULL;
      InstanceIdStack pppIntfiidStack = EMPTY_INSTANCE_ID_STACK;
      Dev2PppInterfacePpoeObject *pppoeObj = NULL;      
      CmsRet ret = CMSRET_SUCCESS;
      UBOOL8 foundPPPIntf = FALSE;
      
      /* Look thru. all pppoe interfaces and check for the lowerlayer matches with
      * the layer 2 interface (vlantermation here).  If found, get the pppoe object for
      * X_BROADCOM_COM_AddPppToBridge and other params.
      */
      while (!foundPPPIntf &&
             (cmsObj_getNextFlags(MDMOID_DEV2_PPP_INTERFACE, &pppIntfiidStack,
                                  OGF_NO_VALUE_UPDATE,
                                  (void **)&pppIntfObj) == CMSRET_SUCCESS))
      {
         if (!cmsUtl_strcmp(ipIntfObj->name, pppIntfObj->name))
         {
            if ((ret = cmsObj_get(MDMOID_DEV2_PPP_INTERFACE_PPOE, 
                                 &pppIntfiidStack, 
                                 0,
                                 (void **) &pppoeObj)) != CMSRET_SUCCESS)
            {
               cmsLog_error("Failed to get pppoeObj, error=%d", ret);
               cmsObj_free((void **) &ipIntfObj);
               cmsObj_free((void **) &pppIntfObj);
               return ret;
            }
            foundPPPIntf = TRUE;
         }
         else
         {
            cmsObj_free((void **) &pppIntfObj);
         }
      }

      if (!foundPPPIntf)
      {
         cmsLog_error("Failed to find ppp interface object %s", ipIntfObj->name);
         return CMSRET_INTERNAL_ERROR;
      }     

      /* PPPoE service name is for PPPoE only, not PPPoA */
/* TODO later For PPPoA      if (!dalWan_isPPPoA(&pppIntfiidStack) && cmsUtl_strcmp(pppConn->PPPoEServiceName, webVar->pppServerName))
*/
      if (cmsUtl_strcmp(pppoeObj->serviceName, webVar->pppServerName))
      {
         CMSMEM_REPLACE_STRING(pppoeObj->serviceName, webVar->pppServerName); 
         change = TRUE;
      }      
      
      if (cmsUtl_strcmp(pppIntfObj->username, webVar->pppUserName))
      {
         CMSMEM_REPLACE_STRING(pppIntfObj->username, webVar->pppUserName);
         change = TRUE;
      }      
      if (cmsUtl_strcmp(pppIntfObj->password, webVar->pppPassword))
      {
         CMSMEM_REPLACE_STRING(pppIntfObj->password, webVar->pppPassword);
         change = TRUE;
      }      
      
      if (cmsUtl_pppAuthToNum(pppIntfObj->authenticationProtocol) != webVar->pppAuthMethod)
      {
         CMSMEM_REPLACE_STRING(pppIntfObj->authenticationProtocol, cmsUtl_numToPppAuthString(webVar->pppAuthMethod));
         change = TRUE;         
      }

      /* Only set this for device 2 data model.  For hybrid data model, nat in in igd wan object.  */
      if (cmsMdm_isDataModelDevice2())
      {
         /* fullconeNAT */
         if (webVar->enblFullcone != qdmIpIntf_isFullConeNatEnabledOnIpIntfFullPathLocked_dev2(ipIntfFullPathBuf)) 
         {
            if ((ret = dalNat_set_dev2(ipIntfFullPathBuf,webVar->enblNat,webVar->enblFullcone)) != CMSRET_SUCCESS)
            {
               /* complain but continue */
               cmsLog_error("Can't set NAT on %s ?. ret %d", webVar->wanIfName, ret);
            }
            change = TRUE;
         }
      }

      if (pppIntfObj->idleDisconnectTime == 0 && webVar->enblOnDemand)
      {
         pppIntfObj->idleDisconnectTime = webVar->pppTimeOut * 60;
         change = TRUE;
      }
      if (pppIntfObj->idleDisconnectTime != 0 && !webVar->enblOnDemand)
      {
          /* 0 is no OnDemand feature.  Change from OnDemand to not OnDemand */
         pppIntfObj->idleDisconnectTime = 0;
         change = TRUE;         
      }
      if (pppIntfObj->idleDisconnectTime != 0 && webVar->enblOnDemand)
      {
         /* Need to set change to TRUE if the idleDisconnectTim differs */
         if (pppIntfObj->idleDisconnectTime != (UINT32) (webVar->pppTimeOut * 60))
         {
            pppIntfObj->idleDisconnectTime = (UINT32) webVar->pppTimeOut * 60;
            change = TRUE;
         }            
      }
      if (pppIntfObj->X_BROADCOM_COM_UseStaticIPAddress != webVar->useStaticIpAddress)
      {
         pppIntfObj->X_BROADCOM_COM_UseStaticIPAddress = webVar->useStaticIpAddress;
         CMSMEM_REPLACE_STRING(pppIntfObj->X_BROADCOM_COM_LocalIPAddress , webVar->pppLocalIpAddress);
         change = TRUE;         
      }    
      
      if (pppIntfObj->X_BROADCOM_COM_UseStaticIPAddress &&  webVar->useStaticIpAddress)
      {
      
         if (cmsUtl_strcmp(pppIntfObj->X_BROADCOM_COM_LocalIPAddress, webVar->pppLocalIpAddress))
         {
            /* Need to set change to TRUE if the locaIPAddress differs */
            CMSMEM_REPLACE_STRING(pppIntfObj->X_BROADCOM_COM_LocalIPAddress , webVar->pppLocalIpAddress);
            change = TRUE;             
         }
      }
      
      if (pppIntfObj->X_BROADCOM_COM_Enable_Debug != webVar->enblPppDebug)
      {
         pppIntfObj->X_BROADCOM_COM_Enable_Debug = webVar->enblPppDebug;
         change = TRUE;         
      }
      if (pppoeObj->X_BROADCOM_COM_AddPppToBridge != webVar->pppToBridge)
      {
         pppoeObj->X_BROADCOM_COM_AddPppToBridge = webVar->pppToBridge;
         change = TRUE;
      }
#ifdef DMP_X_BROADCOM_COM_IGMP_1
      if (ipIntfObj->X_BROADCOM_COM_IGMPEnabled != webVar->enblIgmp)
      {
         ipIntfObj->X_BROADCOM_COM_IGMPEnabled = webVar->enblIgmp;
         change = TRUE;
      } 
      if (ipIntfObj->X_BROADCOM_COM_IGMP_SOURCEEnabled != webVar->enblIgmpMcastSource)
      {
         ipIntfObj->X_BROADCOM_COM_IGMP_SOURCEEnabled = webVar->enblIgmpMcastSource;
         change = TRUE;
      } 
#endif      
 /* TODO: need to add PCPMode to xml file
      if (ipIntfObj->X_BROADCOM_COM_PCPMode != webVar->pcpMode) 
      {
         ipIntfObj->X_BROADCOM_COM_PCPMode = webVar->pcpMode;
         change = TRUE;
      }
*/    

#ifdef DMP_X_BROADCOM_COM_MLDSNOOP_1
      if (ipIntfObj->X_BROADCOM_COM_MLDEnabled != webVar->enblMld)
      {
         ipIntfObj->X_BROADCOM_COM_MLDEnabled = webVar->enblMld;
         change = TRUE;
      } 
      if (ipIntfObj->X_BROADCOM_COM_MLD_SOURCEEnabled != webVar->enblMldMcastSource)
      {
         ipIntfObj->X_BROADCOM_COM_MLD_SOURCEEnabled = webVar->enblMldMcastSource;
         change = TRUE;
      } 
#endif /* DMP_X_BROADCOM_COM_MLDSNOOP_1 */

      
#ifdef DMP_X_BROADCOM_COM_GPONWAN_1      
      if (ipIntfObj->X_BROADCOM_COM_NoMcastVlanFilter != webVar->noMcastVlanFilter)
      {
         ipIntfObj->X_BROADCOM_COM_NoMcastVlanFilter = webVar->noMcastVlanFilter;
         change = TRUE;
      }
#endif  /* DMP_X_BROADCOM_COM_GPONWAN_1 */

      /* set ppp stuff here first if any ppp parameter changes 
      */
      if (change)
      {
         /* Need to disable PPP interface first and
         * later ip interface will propaget the status from the lowerlay of ppp
         * to kick the interface stack when ip interface is enable
         */
         if ((ret = cmsObj_set(pppoeObj, &pppIntfiidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Failed to set pppoe obj. ret %d", ret);
         }         

         pppIntfObj->enable = FALSE;
         if ((ret = cmsObj_set(pppIntfObj, &pppIntfiidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Failed to set pppIntfObj. ret %d", ret);
         }         
      }

      cmsObj_free((void **) &pppoeObj);
      cmsObj_free((void **) &pppIntfObj);
 
      if (ret != CMSRET_SUCCESS)
      {
          cmsObj_free((void **) &ipIntfObj);
          return ret;
      }
      else
      {
         cmsLog_debug("Done set ppp to disabled");
      }
      
   }
   else
   {
      /* For IPoE, IPoA */
      UBOOL8 isCurrNatEnabled = qdmIpIntf_isNatEnabledOnIpIntfFullPathLocked_dev2(ipIntfFullPathBuf);
      UBOOL8 isCurrFullConeNatEnabled = qdmIpIntf_isFullConeNatEnabledOnIpIntfFullPathLocked_dev2(ipIntfFullPathBuf);


      /* Only set this for device 2 data model.  For hybrid data model, nat and firewall in in igd wan object.  */
      if (cmsMdm_isDataModelDevice2())
      {
         if ( (isCurrNatEnabled != webVar->enblNat) || (isCurrFullConeNatEnabled != webVar->enblFullcone) ) 
         {
            if ((ret = dalNat_set_dev2(ipIntfFullPathBuf,webVar->enblNat,webVar->enblFullcone)) != CMSRET_SUCCESS)
            {
               /* complain but continue */
               cmsLog_error("Can't set NAT on %s ?. ret %d", webVar->wanIfName, ret);
            }              
            change = TRUE;
         }
         if ((qdmIpIntf_isFirewallEnabledOnIntfnameLocked_dev2(ipIntfObj->name)) != webVar->enblFirewall) 
         {
            ipIntfObj->X_BROADCOM_COM_FirewallEnabled = webVar->enblFirewall;
            change = TRUE;
         }  
      }
      
#ifdef DMP_X_BROADCOM_COM_IGMP_1      
      if (ipIntfObj->X_BROADCOM_COM_IGMPEnabled != webVar->enblIgmp)
      {
         ipIntfObj->X_BROADCOM_COM_IGMPEnabled = webVar->enblIgmp;
         change = TRUE;
      } 
      if (ipIntfObj->X_BROADCOM_COM_IGMP_SOURCEEnabled != webVar->enblIgmpMcastSource)
      {
         ipIntfObj->X_BROADCOM_COM_IGMP_SOURCEEnabled = webVar->enblIgmpMcastSource;
         change = TRUE;
      } 
#endif      
 /* TODO: need to add PCPMode to xml file
      if (ipIntfObj->X_BROADCOM_COM_PCPMode != webVar->pcpMode) 
      {
         ipIntfObj->X_BROADCOM_COM_PCPMode = webVar->pcpMode;
         change = TRUE;
      }
*/    

#ifdef DMP_X_BROADCOM_COM_MLDSNOOP_1
      if (ipIntfObj->X_BROADCOM_COM_MLDEnabled != webVar->enblMld)
      {
         ipIntfObj->X_BROADCOM_COM_MLDEnabled = webVar->enblMld;
         change = TRUE;
      } 
      if (ipIntfObj->X_BROADCOM_COM_MLD_SOURCEEnabled != webVar->enblMldMcastSource)
      {
         ipIntfObj->X_BROADCOM_COM_MLD_SOURCEEnabled = webVar->enblMldMcastSource;
         change = TRUE;
      } 
#endif /* DMP_X_BROADCOM_COM_MLDSNOOP_1 */

      
#ifdef DMP_X_BROADCOM_COM_GPONWAN_1      
      if (ipIntfObj->X_BROADCOM_COM_NoMcastVlanFilter != webVar->noMcastVlanFilter)
      {
         ipIntfObj->X_BROADCOM_COM_NoMcastVlanFilter = webVar->noMcastVlanFilter;
         change = TRUE;
      }
#endif  /* DMP_X_BROADCOM_COM_GPONWAN_1 */

   }

   if (change)
   {
      /* Need to bring the WAN ip interface down with the new parameters first 
	  * and reactivate this ip interface to have the new parameters 
      */
      ipIntfObj->enable= FALSE;
      ret = cmsObj_set(ipIntfObj, &iidStack);
      cmsObj_free((void **) &ipIntfObj);
      
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to set ipIntf obj.  ret=%d", ret);
      } 
      else
      {
         if (isPpp)
         {
            if ((ret = dalWan_enableDisablePPPIntfObj_dev2(webVar->wanIfName, TRUE)) != CMSRET_SUCCESS)
            {
               cmsLog_error("Failed to set pppIntf obj. ret=%d", ret);
               return ret;
            }
         }
         
         ret = dalWan_enableDisableIpIntfObj_dev2(webVar->wanIfName, TRUE);
      }
   }
   else
   {
      cmsObj_free((void **) &ipIntfObj);
   }

   cmsLog_debug("Exit, ret=%d", ret);

   return ret;
}


CmsRet cmsDal_getAvailableIfForWanService_dev2(NameList **ifList, UBOOL8 skipUsed __attribute__((unused)))
{
   char name[CMS_IFNAME_LENGTH+BUFLEN_32]={0};


   /* Look for DSL (ATM and PTM) WAN connection devices */
#ifdef DMP_DEVICE2_ATMLINK_1
      {
         Dev2AtmLinkObject *atmLink = NULL;
         InstanceIdStack atmLinkIid = EMPTY_INSTANCE_ID_STACK;
         SINT32 vpi = 0;
         SINT32 vci = 0;
         while (cmsObj_getNext(MDMOID_DEV2_ATM_LINK, &atmLinkIid, (void **)&atmLink) == CMSRET_SUCCESS)
         {
             cmsLog_debug("Found %s/%s ATM link",
                           atmLink->name, atmLink->destinationAddress);

             cmsUtl_atmVpiVciStrToNum_dev2(atmLink->destinationAddress, &vpi, &vci);
             sprintf(name, "%s/(%d_%d_%d)",
                           atmLink->name,
                           atmLink->X_BROADCOM_COM_ATMInterfaceId,
                           vpi, vci);

             if (cmsDal_addNameToNameList(name, ifList) == NULL)
             {
                cmsDal_freeNameList(*ifList);
                cmsObj_free((void **) &atmLink);
                return CMSRET_RESOURCE_EXCEEDED;
             }

             cmsObj_free((void **) &atmLink);
         }
      }
#endif /* DMP_DEVICE2_ATMLINK_1 */

#ifdef DMP_DEVICE2_X_BROADCOM_COM_PTMLINK_1
      {
         Dev2PtmLinkObject *ptmLink = NULL;
         InstanceIdStack ptmLinkIid = EMPTY_INSTANCE_ID_STACK;
         while (cmsObj_getNext(MDMOID_DEV2_PTM_LINK, &ptmLinkIid, (void **)&ptmLink) == CMSRET_SUCCESS)
          {
             cmsLog_debug("Found %s PTM link", ptmLink->name);

             /*
              * TR98 code has some code here which skipped the interface
              * if it is non-VLANMUX mode and already had one WAN service
              * defined on it.  But in TR181, all connections are
              * VLANMUX mode, so don't worry about it.
              */

            sprintf(name, "%s/(%d_%d_%d)", ptmLink->name, ptmLink->X_BROADCOM_COM_PTMPortId, 
                    ptmLink->X_BROADCOM_COM_PTMPriorityHigh, ptmLink->X_BROADCOM_COM_PTMPriorityLow);

            if (cmsDal_addNameToNameList(name, ifList) == NULL)
            {
               cmsDal_freeNameList(*ifList);
               cmsObj_free((void **) &ptmLink);
               return CMSRET_RESOURCE_EXCEEDED;
            }

            cmsObj_free((void **) &ptmLink);
         }
      }
#endif /* DMP_DEVICE2_X_BROADCOM_COM_PTMLINK_1 */    

#ifdef DMP_DEVICE2_ETHERNETINTERFACE_1
      {
         Dev2EthernetInterfaceObject *ethIntfObj=NULL;
         InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
         UBOOL8 found=FALSE;

         while (!found &&
                cmsObj_getNextFlags(MDMOID_DEV2_ETHERNET_INTERFACE, &iidStack,
                                    OGF_NO_VALUE_UPDATE,
                                    (void **) &ethIntfObj) == CMSRET_SUCCESS)
         {
            if (ethIntfObj->upstream && ethIntfObj->enable)
            {
               found = TRUE;  // only support 1 ethWan intf.  Once we find one, we are done

               /*
                * TR98 code has some code here which skipped the interface
                * if it is non-VLANMUX mode and already had one WAN service
                * defined on it.  But in TR181, all connections are
                * VLANMUX mode, so don't worry about it.
                */

               /* format interface name */
               strcpy(name, ethIntfObj->name);

               if (cmsDal_addNameToNameList(name, ifList) == NULL)
               {
                  cmsDal_freeNameList(*ifList);
                  cmsObj_free((void **)&ethIntfObj);
                  return CMSRET_RESOURCE_EXCEEDED;
               }
            }

            cmsObj_free((void **)&ethIntfObj);
         }
      }
#endif /* DMP_DEVICE2_ETHERNETINTERFACE_1 */

#ifdef DMP_X_BROADCOM_COM_MOCAWAN_1
#endif /* DMP_X_BROADCOM_COM_MOCAWAN_1 */

#ifdef DMP_X_BROADCOM_COM_GPONWAN_1
    return dalOptical_getInterface(ifList, GPON_WAN_IF_NAME, TRUE);
#endif /* DMP_X_BROADCOM_COM_GPONWAN_1 */

#ifdef DMP_X_BROADCOM_COM_EPONWAN_1
    return dalOptical_getInterface(ifList, EPON_WAN_IF_NAME, TRUE);
#endif /* DMP_X_BROADCOM_COM_EPONWAN_1 */

#ifdef DMP_X_BROADCOM_COM_WIFIWAN_1
#endif /* DMP_X_BROADCOM_COM_WIFIWAN_1 */


   return CMSRET_SUCCESS;
}


UBOOL8 dalWan_isInterfaceVlanEnabled_dev2(const char *l3IntfName __attribute__((unused)))
{
   /* In TR181, all WAN services are VLANMUX enabled, so always return TRUE */

   return TRUE;
}


UBOOL8 dalWan_isValidWanInterface_dev2(const char *ifName)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2IpInterfaceObject *ipIntfObj=NULL;
   UBOOL8 found=FALSE;

   cmsLog_debug("Enter: ifName=%s", ifName);

   while (!found &&
          cmsObj_getNext(MDMOID_DEV2_IP_INTERFACE, &iidStack,
                         (void **)&ipIntfObj) == CMSRET_SUCCESS)
   {
      /* Even bridged WAN service will have an IP.Interface object even
       * though a bridge is not "IP" layer.
       */
      cmsLog_debug("got %s upstream=%d", ipIntfObj->name, ipIntfObj->X_BROADCOM_COM_Upstream);

      if (ipIntfObj->X_BROADCOM_COM_Upstream &&
          !cmsUtl_strcmp(ipIntfObj->name, ifName))
      {
         found = TRUE;
      }

      cmsObj_free((void **)&ipIntfObj);
   }

   cmsLog_debug("Exit: ifName=%s (found=%d)", ifName, found);

   return found;
}

#ifdef DMP_X_BROADCOM_COM_DSL_1 

CmsRet dalWan_getAdslFlags_dev2(UINT32 *adslFlags)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2DslLineObject *dslLineObj;
   CmsRet ret;
   char cfgModType[BUFLEN_128];
   char *pToken = NULL;
   char *pLast = NULL;

   cmsLog_debug("Enter");

   /* we are always configuring only the main DSL line */
   ret = cmsObj_getNext(MDMOID_DEV2_DSL_LINE, &iidStack, (void **)&dslLineObj);
   if (ret != CMSRET_SUCCESS)
   {
      return ret;
   }

   *adslFlags = 0;
   strncpy(cfgModType,dslLineObj->X_BROADCOM_COM_AdslModulationCfg,BUFLEN_128);
   pToken = strtok_r(cfgModType, ", ", &pLast);
   while (pToken != NULL)
   {
      if (cmsUtl_strcmp(pToken,MDMVS_ADSL_MODULATION_ALL) == 0)
      {
#ifdef DMP_VDSL2WAN_1
         *adslFlags |= ANNEX_A_MODE_ALL_MOD_68;
#else
         *adslFlags |= ANNEX_A_MODE_ALL_MOD_48;
#endif
         break;
      }
      else 
      {
         if (cmsUtl_strcmp(pToken,MDMVS_ADSL_G_DMT) == 0)
         {
            *adslFlags |= ANNEX_A_MODE_GDMT;
         }
         if (cmsUtl_strcmp(pToken,MDMVS_ADSL_2PLUS) == 0)
         {
            *adslFlags |= ANNEX_A_MODE_ADSL2PLUS;
         }
         if (cmsUtl_strcmp(pToken,MDMVS_ADSL_G_DMT_BIS) == 0)
         {
            *adslFlags |= ANNEX_A_MODE_ADSL2;
         }
         if (cmsUtl_strcmp(pToken,MDMVS_ADSL_RE_ADSL) == 0)
         {
            *adslFlags |= ANNEX_A_MODE_ANNEXL;
         }
         if (cmsUtl_strcmp(pToken,MDMVS_ADSL_G_LITE) == 0)
         {
            *adslFlags |= ANNEX_A_MODE_GLITE;
         }
         if (cmsUtl_strcmp(pToken,MDMVS_ADSL_ANSI_T1_413) == 0)
         {
            *adslFlags |= ANNEX_A_MODE_T1413;
         }
         if (cmsUtl_strcmp(pToken,MDMVS_ANNEXM) == 0)
         {
            *adslFlags |= ANNEX_A_MODE_ANNEXM;
         }         
#ifdef DMP_VDSL2WAN_1
        if (cmsUtl_strcmp(pToken,MDMVS_VDSL2) == 0)
            *adslFlags |=  ANNEX_A_MODE_VDSL2;
#ifdef SUPPORT_DSL_GFAST
        if (cmsUtl_strcmp(pToken,MDMVS_G_FAST) == 0)
            *adslFlags |=  ANNEX_A_MODE_GFAST;
#endif
#endif

      } /* else */
      pToken = strtok_r(NULL, ", ", &pLast);
   } /* while pToken */

   if (dslLineObj->X_BROADCOM_COM_ADSL2_AnnexM == TRUE)
   {
      *adslFlags |= ANNEX_A_MODE_ANNEXM;
   }
   
   if (cmsUtl_strcmp(dslLineObj->X_BROADCOM_COM_PhoneLinePair,MDMVS_OUTER_PAIR) == 0)
   {
      *adslFlags |= ANNEX_A_LINE_PAIR_OUTER;
   }
   else
   {
      *adslFlags |= ANNEX_A_LINE_PAIR_INNER;
   }

   if (cmsUtl_strcmp(dslLineObj->X_BROADCOM_COM_Bitswap,MDMVS_OFF) == 0)
   {
      *adslFlags |= ANNEX_A_BITSWAP_DISENABLE;
   }
   if (cmsUtl_strcmp(dslLineObj->X_BROADCOM_COM_SRA,MDMVS_ON) == 0)
   {
      *adslFlags |=  ANNEX_A_SRA_ENABLE;
   }

#ifdef DMP_VDSL2WAN_1
   *adslFlags |= (dslLineObj->X_BROADCOM_COM_VDSL_8a) ? VDSL_PROFILE_8a : 0;
   *adslFlags |= (dslLineObj->X_BROADCOM_COM_VDSL_8b) ? VDSL_PROFILE_8b : 0;
   *adslFlags |= (dslLineObj->X_BROADCOM_COM_VDSL_8c) ? VDSL_PROFILE_8c : 0;
   *adslFlags |= (dslLineObj->X_BROADCOM_COM_VDSL_8d) ? VDSL_PROFILE_8d : 0;
   *adslFlags |= (dslLineObj->X_BROADCOM_COM_VDSL_12a) ? VDSL_PROFILE_12a : 0;
   *adslFlags |= (dslLineObj->X_BROADCOM_COM_VDSL_12b) ? VDSL_PROFILE_12b : 0;
   *adslFlags |= (dslLineObj->X_BROADCOM_COM_VDSL_17a) ? VDSL_PROFILE_17a : 0;
   *adslFlags |= (dslLineObj->X_BROADCOM_COM_VDSL_30a) ? VDSL_PROFILE_30a : 0;
   *adslFlags |= (dslLineObj->X_BROADCOM_COM_VDSL_US0_8a) ? VDSL_US0_8a : 0;

   cmsLog_debug("VDSL2 profiles 8a=%d 8b=%d 8c=%d 8d=%d",
                dslLineObj->X_BROADCOM_COM_VDSL_8a,
                dslLineObj->X_BROADCOM_COM_VDSL_8b,
                dslLineObj->X_BROADCOM_COM_VDSL_8c,
                dslLineObj->X_BROADCOM_COM_VDSL_8d);
#endif

   cmsObj_free((void **) &dslLineObj);

   cmsLog_debug("End: ret %d, adslFlags 0x%x", ret, *adslFlags);
   return ret;
}


CmsRet dalWan_setAdslFlags_dev2(UINT32 adslFlags)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2DslLineObject *dslLineObj = NULL;
   CmsRet ret;
   UINT32 modMask = 0;
   char cfgModType[BUFLEN_128];
   int len;

   cmsLog_debug("Enter: adslFlags 0x%x",adslFlags);

   /* we are assuming there is only 1 WANDevice */
   ret = cmsObj_getNext(MDMOID_DEV2_DSL_LINE, &iidStack, (void **) &dslLineObj);
   if (ret != CMSRET_SUCCESS)
   {
      return ret;
   }
#ifdef DMP_VDSL2WAN_1
   modMask = ANNEX_A_MODE_ALL_MOD_68;
#else
   modMask = ANNEX_A_MODE_ALL_MOD_48;
#endif
   if ((adslFlags & modMask) == modMask)
   {
      cmsMem_free(dslLineObj->X_BROADCOM_COM_AdslModulationCfg);
      dslLineObj->X_BROADCOM_COM_AdslModulationCfg = cmsMem_strdup(MDMVS_ADSL_MODULATION_ALL);
   }
   else
   {
      memset(cfgModType,0,BUFLEN_128);
      if ((adslFlags & ANNEX_A_MODE_GDMT) == ANNEX_A_MODE_GDMT)
      {
         strcat(cfgModType,MDMVS_ADSL_G_DMT);
         strcat(cfgModType,", ");
      }
      if ((adslFlags & ANNEX_A_MODE_ADSL2PLUS) == ANNEX_A_MODE_ADSL2PLUS)
      {
         strcat(cfgModType,MDMVS_ADSL_2PLUS);
         strcat(cfgModType,", ");
      }
      if ((adslFlags & ANNEX_A_MODE_ADSL2) == ANNEX_A_MODE_ADSL2)
      {
         strcat(cfgModType,MDMVS_ADSL_G_DMT_BIS);
         strcat(cfgModType,", ");
      }
      if ((adslFlags & ANNEX_A_MODE_ANNEXL) == ANNEX_A_MODE_ANNEXL)
      {
         strcat(cfgModType,MDMVS_ADSL_RE_ADSL);
         strcat(cfgModType,", ");
      }
      if ((adslFlags &  ANNEX_A_MODE_GLITE) ==  ANNEX_A_MODE_GLITE)
      {
         strcat(cfgModType,MDMVS_ADSL_G_LITE);
         strcat(cfgModType,", ");
      }
      if ((adslFlags &  ANNEX_A_MODE_T1413) ==  ANNEX_A_MODE_T1413)
      {
         strcat(cfgModType,MDMVS_ADSL_ANSI_T1_413);
         strcat(cfgModType,", ");
      }
      if ((adslFlags & ANNEX_A_MODE_ANNEXM) ==  ANNEX_A_MODE_ANNEXM)
      {
         strcat(cfgModType, MDMVS_ANNEXM);
         strcat(cfgModType,", ");
      }      
#ifdef DMP_VDSL2WAN_1
      if ((adslFlags &  ANNEX_A_MODE_VDSL2) ==  ANNEX_A_MODE_VDSL2)
      {
         strcat(cfgModType,MDMVS_VDSL2);
         strcat(cfgModType,", ");
      }
#ifdef SUPPORT_DSL_GFAST
      if ((adslFlags &  ANNEX_A_MODE_GFAST) ==  ANNEX_A_MODE_GFAST)
      {
         strcat(cfgModType,MDMVS_G_FAST);
         strcat(cfgModType,", ");
      }
#endif
#endif
      /* take out the last ", " */
      len = strlen(cfgModType);
      if (len > 2)
      {
         cfgModType[len-2] = '\0';
         cmsMem_free(dslLineObj->X_BROADCOM_COM_AdslModulationCfg);
         dslLineObj->X_BROADCOM_COM_AdslModulationCfg = cmsMem_strdup(cfgModType);
      }
      else
      {
         /* default will be all */
         cmsMem_free(dslLineObj->X_BROADCOM_COM_AdslModulationCfg);
         dslLineObj->X_BROADCOM_COM_AdslModulationCfg = cmsMem_strdup(MDMVS_ADSL_MODULATION_ALL);
      }
   } /* not MODALL*/

   /* We now have MDMVS_ANNEXM, so this is only for backward compatibility. */
   dslLineObj->X_BROADCOM_COM_ADSL2_AnnexM = ((adslFlags & ANNEX_A_MODE_ANNEXM) != 0);

   cmsMem_free(dslLineObj->X_BROADCOM_COM_PhoneLinePair);   
   if ((adslFlags & ANNEX_A_LINE_PAIR_OUTER) == ANNEX_A_LINE_PAIR_OUTER)
   {
      dslLineObj->X_BROADCOM_COM_PhoneLinePair = cmsMem_strdup(MDMVS_OUTER_PAIR);
   }
   else
   {
      dslLineObj->X_BROADCOM_COM_PhoneLinePair = cmsMem_strdup(MDMVS_INNER_PAIR);
   }

   /* bitswap */
   cmsMem_free(dslLineObj->X_BROADCOM_COM_Bitswap);
   if ((adslFlags & ANNEX_A_BITSWAP_DISENABLE) == ANNEX_A_BITSWAP_DISENABLE)
   {
      dslLineObj->X_BROADCOM_COM_Bitswap = cmsMem_strdup(MDMVS_OFF);
   }
   else
   {
      dslLineObj->X_BROADCOM_COM_Bitswap = cmsMem_strdup(MDMVS_ON);
   }

   cmsMem_free(dslLineObj->X_BROADCOM_COM_SRA);
   if ((adslFlags & ANNEX_A_SRA_ENABLE) == ANNEX_A_SRA_ENABLE)
   {
      dslLineObj->X_BROADCOM_COM_SRA = cmsMem_strdup(MDMVS_ON);
   }
   else
   {
      dslLineObj->X_BROADCOM_COM_SRA = cmsMem_strdup(MDMVS_OFF);
   }

#ifdef DMP_VDSL2WAN_1
   /* set the various VDSL profile bits, defined in adslctlapi.h */
   dslLineObj->X_BROADCOM_COM_VDSL_8a = ((adslFlags & VDSL_PROFILE_8a) != 0);
   dslLineObj->X_BROADCOM_COM_VDSL_8b = ((adslFlags & VDSL_PROFILE_8b) != 0);
   dslLineObj->X_BROADCOM_COM_VDSL_8c = ((adslFlags & VDSL_PROFILE_8c) != 0);
   dslLineObj->X_BROADCOM_COM_VDSL_8d = ((adslFlags & VDSL_PROFILE_8d) != 0);
   dslLineObj->X_BROADCOM_COM_VDSL_12a = ((adslFlags & VDSL_PROFILE_12a) != 0);
   dslLineObj->X_BROADCOM_COM_VDSL_12b = ((adslFlags & VDSL_PROFILE_12b) != 0);
   dslLineObj->X_BROADCOM_COM_VDSL_17a = ((adslFlags & VDSL_PROFILE_17a) != 0);
   dslLineObj->X_BROADCOM_COM_VDSL_30a = ((adslFlags & VDSL_PROFILE_30a) != 0);

   dslLineObj->X_BROADCOM_COM_VDSL_US0_8a = ((adslFlags & VDSL_US0_8a) != 0);
#endif

   ret = cmsObj_set((void *) dslLineObj,&iidStack );

   cmsObj_free((void **) &dslLineObj);

   cmsLog_debug("End: ret %d",ret);

   return ret;
}
#endif /* DMP_X_BROADCOM_COM_DSL_1  */

#endif /* DMP_DEVICE2_BASELINE_1 */


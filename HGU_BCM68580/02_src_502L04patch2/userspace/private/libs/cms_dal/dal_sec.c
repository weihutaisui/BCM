/***********************************************************************
 *
 *  Copyright (c) 2007-2008  Broadcom Corporation
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
#include "cms_util.h"
#include "mdm_validstrings.h"
#include "cms_obj.h"
#include "cms_dal.h"


#ifdef DMP_X_BROADCOM_COM_SECURITY_1
/* We can enable IP filter out feature to support interface group (br0, br1, ...) by passing interface argument in the future */
CmsRet dalSec_addIpFilterOut_igd(const char *name, const char *ipver, const char *protocol,
              const char *srcAddr, const char *srcMask,  const char *srcPort,
              const char *dstAddr, const char *dstMask, const char *dstPort,
              const char *interfaces __attribute__((unused)))
{
   char port_start[BUFLEN_8], *port_end;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   IpFilterCfgObject *ipFilterCfg = NULL;
   LanIpIntfObject *ipIntfObj = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   /* adding a new IP filter out entry */
   cmsLog_debug("Adding new IP filter entry with %s/%s/%s/%s/%s/%s/%s/%s/%s", \
                name, ipver, protocol, srcAddr, srcMask, srcPort, dstAddr, dstMask, dstPort);

   if (cmsObj_getNext(MDMOID_LAN_IP_INTF, &iidStack, (void **)&ipIntfObj) == CMSRET_SUCCESS) 
   {
      cmsObj_free((void **) &ipIntfObj);
   }
                     
   /* add new instance of IP filter out */
   if ((ret = cmsObj_addInstance(MDMOID_IP_FILTER_CFG, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create new IpFilterCfgObject, ret=%d", ret);
      return ret;
   }

   if ((ret = cmsObj_get(MDMOID_IP_FILTER_CFG, &iidStack, 0, (void **) &ipFilterCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get IpFilterCfgObject, ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_IP_FILTER_CFG, &iidStack);
      return ret;
   }

   ipFilterCfg->enable = TRUE;
   CMSMEM_REPLACE_STRING(ipFilterCfg->filterName, name);
   CMSMEM_REPLACE_STRING(ipFilterCfg->IPVersion, ipver);
   CMSMEM_REPLACE_STRING(ipFilterCfg->sourceIPAddress, srcAddr);
   CMSMEM_REPLACE_STRING(ipFilterCfg->sourceNetMask, srcMask);

   if( (port_end = strchr(srcPort, ':')) == NULL)
   {
      ipFilterCfg->sourcePortStart = atoi(srcPort);
   }
   else
   {
      strncpy(port_start, srcPort, (port_end - srcPort));
      ipFilterCfg->sourcePortStart = atoi(port_start);
      ipFilterCfg->sourcePortEnd= atoi(port_end + 1);
   }
   
   CMSMEM_REPLACE_STRING(ipFilterCfg->destinationIPAddress, dstAddr);
   CMSMEM_REPLACE_STRING(ipFilterCfg->destinationNetMask, dstMask);

   if( (port_end = strchr(dstPort, ':')) == NULL)
   {
      ipFilterCfg->destinationPortStart = atoi(dstPort);
   }
   else
   {
      strncpy(port_start, dstPort, (port_end - dstPort));
      ipFilterCfg->destinationPortStart = atoi(port_start);
      ipFilterCfg->destinationPortEnd= atoi(port_end + 1);
   }
   
   switch ( atoi(protocol) ) 
   {
      case 0:  // TCP or UDP
         CMSMEM_REPLACE_STRING(ipFilterCfg->protocol, MDMVS_TCP_OR_UDP);
         break;
      case 1: // TCP
         CMSMEM_REPLACE_STRING(ipFilterCfg->protocol, MDMVS_TCP);
         break;
      case 2: // UDP
         CMSMEM_REPLACE_STRING(ipFilterCfg->protocol, MDMVS_UDP);
         break;
      case 3: // ICMP
         if (atoi(ipver) == 4)
         {
            CMSMEM_REPLACE_STRING(ipFilterCfg->protocol, MDMVS_ICMP);
         }
         else
         {
            CMSMEM_REPLACE_STRING(ipFilterCfg->protocol, MDMVS_ICMPV6);
         }
         break;
      default: // None
         CMSMEM_REPLACE_STRING(ipFilterCfg->protocol, MDMVS_NONE);
         break;
   }
   
   /* set and activate IpFilterCfgObject */
   ret = cmsObj_set(ipFilterCfg, &iidStack);
   cmsObj_free((void **) &ipFilterCfg);

   if (ret != CMSRET_SUCCESS)
   {
      CmsRet r2;
      cmsLog_error("Failed to set IpFilterCfgObject, ret = %d", ret);
       
      r2 = cmsObj_deleteInstance(MDMOID_IP_FILTER_CFG, &iidStack);
      if (r2 != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to delete created IpFilterCfgObject, r2=%d", r2);
      }

      cmsLog_debug("Failed to set IP filter entry and successfully delete created IpFilterCfgObject");
   }

   return ret;
}


CmsRet dalSec_deleteIpFilterOut_igd(const char* name)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   IpFilterCfgObject *ipFilterCfg = NULL;
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 found = FALSE;

   /* deleting a IP filter out entry */
   cmsLog_debug("Deleting a IP filter entry with filter name: %s", name);

   while ((ret = cmsObj_getNext
         (MDMOID_IP_FILTER_CFG, &iidStack, (void **) &ipFilterCfg)) == CMSRET_SUCCESS)
   {
      if(!cmsUtl_strcmp(name, ipFilterCfg->filterName) ) 
      {
         found = TRUE;
         cmsObj_free((void **) &ipFilterCfg);
         break;                                               //For multiple subnet purpose we should not break!! currently not supported!! 
      }
      else
      {
         cmsObj_free((void **) &ipFilterCfg);
      }
   }

   if(found == FALSE) 
   {
      cmsLog_debug("bad filter name %s, no entry found", name);
      ret = CMSRET_INVALID_ARGUMENTS;
   }
   else 
   {
      ret = cmsObj_deleteInstance(MDMOID_IP_FILTER_CFG, &iidStack);

      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to delete IpFilterCfgObject, ret = %d", ret);
      }
   }

   return ret;
}


static CmsRet addAndSetFirewallException(MdmObjectId oid, InstanceIdStack *iidStack,
                            const char *name, const char *ipver, const char *protocol,
                            const char *srcAddr, const char *srcMask,  const char *srcPort, 
                            const char *dstAddr, const char *dstMask, const char *dstPort)

{
   char port_start[BUFLEN_8], *port_end;
   CmsRet ret;

   /*
    * I use the WanPppConnFirewallExceptionObject, but this object has the
    * exact same fields as the WanIpConnFirewallExceptionObject and the
    * LanIpIntfFirewallExceptionObject.
    */
   WanPppConnFirewallExceptionObject *commonExceptionObj=NULL;

   cmsLog_debug("oid=%d iidStack=%s", oid, cmsMdm_dumpIidStack(iidStack));


   /* add new instance of IP filter in*/
   if ((ret = cmsObj_addInstance(oid, iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create new FirewallExceptionObject under oid=%d iidStack=%s, ret=%d",
                   oid, cmsMdm_dumpIidStack(iidStack), ret);
      return ret;
   }
      
   if ((ret = cmsObj_get(oid, iidStack, 0, (void **) &commonExceptionObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get FirewallExceptionObject, ret=%d", ret);
      cmsObj_deleteInstance(oid, iidStack);
      return ret;
   }
      
   commonExceptionObj->enable = TRUE;
   CMSMEM_REPLACE_STRING(commonExceptionObj->filterName, name);
   CMSMEM_REPLACE_STRING(commonExceptionObj->IPVersion, ipver);
   CMSMEM_REPLACE_STRING(commonExceptionObj->sourceIPAddress, srcAddr);
   CMSMEM_REPLACE_STRING(commonExceptionObj->sourceNetMask, srcMask);
      
   if( (port_end = strchr(srcPort, ':')) == NULL)
   {
      commonExceptionObj->sourcePortStart = atoi(srcPort);
   }
   else
   {
      strncpy(port_start, srcPort, (port_end - srcPort));
      commonExceptionObj->sourcePortStart = atoi(port_start);
      commonExceptionObj->sourcePortEnd= atoi(port_end + 1);
   }
         
   CMSMEM_REPLACE_STRING(commonExceptionObj->destinationIPAddress, dstAddr);
   CMSMEM_REPLACE_STRING(commonExceptionObj->destinationNetMask, dstMask);
      
   if( (port_end = strchr(dstPort, ':')) == NULL)
   {
      commonExceptionObj->destinationPortStart = atoi(dstPort);
   }
   else
   {
      strncpy(port_start, dstPort, (port_end - dstPort));
      commonExceptionObj->destinationPortStart = atoi(port_start);
      commonExceptionObj->destinationPortEnd= atoi(port_end + 1);
   }
         
   switch ( atoi(protocol) ) 
   {
   case 0:  // TCP or UDP
      CMSMEM_REPLACE_STRING(commonExceptionObj->protocol, MDMVS_TCP_OR_UDP);
      break;
   case 1: // TCP
      CMSMEM_REPLACE_STRING(commonExceptionObj->protocol, MDMVS_TCP);
      break;
   case 2: // UDP
      CMSMEM_REPLACE_STRING(commonExceptionObj->protocol, MDMVS_UDP);
      break;
   case 3: // ICMP
      if (atoi(ipver) == 4)
      {
         CMSMEM_REPLACE_STRING(commonExceptionObj->protocol, MDMVS_ICMP);
      }
      else
      {
         CMSMEM_REPLACE_STRING(commonExceptionObj->protocol, MDMVS_ICMPV6);
      }
      break;
   default: // None
      CMSMEM_REPLACE_STRING(commonExceptionObj->protocol, MDMVS_NONE);
      break;
   }
         
   /* set and activate the FirewallExceptionObject */
   ret = cmsObj_set(commonExceptionObj, iidStack);
   cmsObj_free((void **) &commonExceptionObj);
      
   if (ret != CMSRET_SUCCESS)
   {
      CmsRet r2;
      cmsLog_error("Failed to set FirewallExceptionObject, ret = %d", ret);
             
      r2 = cmsObj_deleteInstance(oid, iidStack);
      if (r2 != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to delete created FirewallExceptionObject, r2=%d", r2);
      }
      
      cmsLog_debug("Failed to set FirewallException object but successfully delete created FirewallExceptionObject");
   }

   return ret;
}





CmsRet dalSec_addIpFilterIn_igd(const char *name, const char *ipver, const char *protocol,
                            const char *srcAddr, const char *srcMask,  const char *srcPort, 
                            const char *dstAddr, const char *dstMask, const char *dstPort,
                            const char *ifName)
{
   UBOOL8 isPpp=FALSE;
   UBOOL8 isLan=FALSE;
   UBOOL8 found=FALSE;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanPppConnObject *wan_ppp_conn = NULL;
   WanIpConnObject *wan_ip_conn = NULL;
   LanIpIntfObject *ipIntfObj = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   /* adding a new IP filter in entry */
   cmsLog_debug("Adding new IP filter in entry with %s/%s/%s/%s/%s/%s/%s/%s/%s/%s",
                 name, ipver, protocol, srcAddr, srcMask, srcPort, dstAddr, dstMask, dstPort, ifName);
   
   if (strstr(ifName, "ppp") != NULL)
   {
      isPpp = TRUE;
   }
   else if (strstr(ifName, "br") != NULL)
   {
      isLan = TRUE;
   }

   if (isPpp)
   {
      while (!found &&
             cmsObj_getNext(MDMOID_WAN_PPP_CONN, &iidStack, (void **)&wan_ppp_conn) == CMSRET_SUCCESS) 
      {
         if (0 == cmsUtl_strcmp(ifName, wan_ppp_conn->X_BROADCOM_COM_IfName))
         {
            found = TRUE;

            ret = addAndSetFirewallException(MDMOID_WAN_PPP_CONN_FIREWALL_EXCEPTION, &iidStack,
                                             name, ipver, protocol,
                                             srcAddr, srcMask, srcPort,
                                             dstAddr, dstMask, dstPort);
         }

         cmsObj_free((void **) &wan_ppp_conn);
      }
   }
   else if (!isPpp && !isLan)
   {
      while (!found &&
             cmsObj_getNext(MDMOID_WAN_IP_CONN, &iidStack, (void **)&wan_ip_conn) == CMSRET_SUCCESS) 
      {
         if (0 == cmsUtl_strcmp(ifName, wan_ip_conn->X_BROADCOM_COM_IfName))
         {
            found = TRUE;

            ret = addAndSetFirewallException(MDMOID_WAN_IP_CONN_FIREWALL_EXCEPTION, &iidStack,
                                             name, ipver, protocol,
                                             srcAddr, srcMask, srcPort,
                                             dstAddr, dstMask, dstPort);
         }
      }
   }
   else
   {
      while (!found &&
             cmsObj_getNext(MDMOID_LAN_IP_INTF, &iidStack, (void **)&ipIntfObj) == CMSRET_SUCCESS) 
      {
         if (0 == cmsUtl_strcmp(ifName, ipIntfObj->X_BROADCOM_COM_IfName))
         {
            found = TRUE;

            ret = addAndSetFirewallException(MDMOID_LAN_IP_INTF_FIREWALL_EXCEPTION, &iidStack,
                                             name, ipver, protocol,
                                             srcAddr, srcMask, srcPort,
                                             dstAddr, dstMask, dstPort);
         }
      }
   }

   if (!found)
   {
      cmsLog_error("Could not find ifName %s", ifName);
      ret = CMSRET_INVALID_ARGUMENTS;
   }

   return ret;
}


CmsRet dalSec_deleteIpFilterIn_igd(const char* filtername)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack parentIidStack = EMPTY_INSTANCE_ID_STACK;
   WanPppConnObject *wanPppConn = NULL;
   WanIpConnObject *wanIpConn = NULL;
   LanIpIntfObject *ipIntfObj = NULL;
   WanPppConnFirewallExceptionObject *PppConnFirewall = NULL;
   WanIpConnFirewallExceptionObject *IpConnFirewall = NULL;
   LanIpIntfFirewallExceptionObject *lanFirewall = NULL;
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 found = FALSE;
   UBOOL8 deleted = FALSE;
   
   cmsLog_debug("Deleting all FirewallException entries with filter name: %s", filtername);

   while(cmsObj_getNext(MDMOID_WAN_PPP_CONN, &parentIidStack, (void **)&wanPppConn) == CMSRET_SUCCESS)
   {
      cmsObj_free((void **) &wanPppConn);
      while(!deleted && cmsObj_getNextInSubTree(MDMOID_WAN_PPP_CONN_FIREWALL_EXCEPTION, &parentIidStack, &iidStack, (void **)&PppConnFirewall) == CMSRET_SUCCESS)
      {
         if(!cmsUtl_strcmp(filtername, PppConnFirewall->filterName))
         {
            deleted = TRUE;
            found = TRUE;
            ret = cmsObj_deleteInstance(MDMOID_WAN_PPP_CONN_FIREWALL_EXCEPTION, &iidStack);
            if (ret != CMSRET_SUCCESS)
            {
               cmsLog_error("Failed to delete WanPppConnFirewallExceptionObject, ret = %d", ret);
            }
         }
         cmsObj_free((void **) &PppConnFirewall);         
      }
      INIT_INSTANCE_ID_STACK(&iidStack);
      deleted = FALSE;
   }

   INIT_INSTANCE_ID_STACK(&parentIidStack);
   while(cmsObj_getNext(MDMOID_WAN_IP_CONN, &parentIidStack, (void **)&wanIpConn) == CMSRET_SUCCESS)
   {
      cmsObj_free((void **) &wanIpConn);
      while(!deleted && cmsObj_getNextInSubTree(MDMOID_WAN_IP_CONN_FIREWALL_EXCEPTION, &parentIidStack, &iidStack, (void **)&IpConnFirewall) == CMSRET_SUCCESS)
      {
         if(!cmsUtl_strcmp(filtername, IpConnFirewall->filterName))
         {
            deleted = TRUE;
            found = TRUE;
            ret = cmsObj_deleteInstance(MDMOID_WAN_IP_CONN_FIREWALL_EXCEPTION, &iidStack);
            if (ret != CMSRET_SUCCESS)
            {
               cmsLog_error("Failed to delete WanIpConnFirewallExceptionObject, ret = %d", ret);
            }
         }
         cmsObj_free((void **) &IpConnFirewall);         
      }
      INIT_INSTANCE_ID_STACK(&iidStack);
      deleted = FALSE;
   }

   INIT_INSTANCE_ID_STACK(&parentIidStack);
   while(cmsObj_getNext(MDMOID_LAN_IP_INTF, &parentIidStack, (void **)&ipIntfObj) == CMSRET_SUCCESS)
   {
      cmsObj_free((void **) &ipIntfObj);
      while(!deleted && cmsObj_getNextInSubTree(MDMOID_LAN_IP_INTF_FIREWALL_EXCEPTION, &parentIidStack, &iidStack, (void **)&lanFirewall) == CMSRET_SUCCESS)
      {
         if(!cmsUtl_strcmp(filtername, lanFirewall->filterName))
         {
            deleted = TRUE;
            found = TRUE;
            ret = cmsObj_deleteInstance(MDMOID_LAN_IP_INTF_FIREWALL_EXCEPTION, &iidStack);
            if (ret != CMSRET_SUCCESS)
            {
               cmsLog_error("Failed to delete LanIpIntfFirewallExceptionObject, ret = %d", ret);
            }
         }
         cmsObj_free((void **) &lanFirewall);         
      }
      INIT_INSTANCE_ID_STACK(&iidStack);
      deleted = FALSE;
   }

   if(found == FALSE) 
   {
      cmsLog_error("bad filter name %s, no entry found", filtername);
      ret = CMSRET_INVALID_ARGUMENTS;
   }
   else
   {
      ret = CMSRET_SUCCESS;
   }

   return ret;
}


CmsRet dalSec_addMacFilter_igd(const char *protocol, const char *srcMac, const char *dstMac, const char *direction, const char *ifName)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack parentIidStack1 = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack parentIidStack2 = EMPTY_INSTANCE_ID_STACK;
   WanIpConnObject *wan_ip_conn = NULL;
   MacFilterCfgObject *macFilterCfg=NULL;
   MacFilterObject *macFilterObj=NULL;

   while (cmsObj_getNext(MDMOID_WAN_IP_CONN, &parentIidStack1, (void **) &wan_ip_conn) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strcmp(ifName, wan_ip_conn->X_BROADCOM_COM_IfName) == 0)
      {
         cmsObj_free((void **) &wan_ip_conn);
         break;
      }
      cmsObj_free((void **) &wan_ip_conn);
   }

   if(cmsObj_getNextInSubTree(MDMOID_MAC_FILTER, &parentIidStack1, &parentIidStack2, (void **)&macFilterObj) != CMSRET_SUCCESS)
   {
      cmsLog_error("Cannot get MacFilterCfgObject!!");
      return CMSRET_INTERNAL_ERROR;
   }
   cmsObj_free((void **) &macFilterCfg);

   iidStack = parentIidStack2;

   /* add new instance of mac filter */
   if ((ret = cmsObj_addInstance(MDMOID_MAC_FILTER_CFG, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create new MacFilterCfgObject, ret=%d", ret);
      return ret;
   }
      
   if ((ret = cmsObj_get(MDMOID_MAC_FILTER_CFG, &iidStack, 0, (void **) &macFilterCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get MacFilterCfgObject, ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_MAC_FILTER_CFG, &iidStack);
      return ret;
   }
      
   macFilterCfg->enable = TRUE;
   CMSMEM_REPLACE_STRING(macFilterCfg->sourceMAC, srcMac);
   CMSMEM_REPLACE_STRING(macFilterCfg->destinationMAC, dstMac);

   switch(atoi(protocol))
   {
      case 1:  // PPPoE
         CMSMEM_REPLACE_STRING(macFilterCfg->protocol, MDMVS_PPPOE);
         break;
      case 2: // IPv4
         CMSMEM_REPLACE_STRING(macFilterCfg->protocol, MDMVS_IPV4);
         break;
      case 3: // IPv6
         CMSMEM_REPLACE_STRING(macFilterCfg->protocol, MDMVS_IPV6);
         break;
      case 4: // AppleTalk
         CMSMEM_REPLACE_STRING(macFilterCfg->protocol, MDMVS_APPLETALK);
         break;
      case 5: // IPX
         CMSMEM_REPLACE_STRING(macFilterCfg->protocol, MDMVS_IPX);
         break;
      case 6: // NetBEUI
         CMSMEM_REPLACE_STRING(macFilterCfg->protocol, MDMVS_NETBEUI);
         break;
      case 7: // IGMP
         CMSMEM_REPLACE_STRING(macFilterCfg->protocol, MDMVS_IGMP);
         break;
      default: // None
         CMSMEM_REPLACE_STRING(macFilterCfg->protocol, MDMVS_NONE);
         break;
   }

   switch(atoi(direction))
   {
      case 0:  // LAN to WAN
         CMSMEM_REPLACE_STRING(macFilterCfg->direction, MDMVS_LAN_TO_WAN);
         break;
      case 1: // WAN to LAN
         CMSMEM_REPLACE_STRING(macFilterCfg->direction, MDMVS_WAN_TO_LAN);
         break;
      default: // BOTH
         CMSMEM_REPLACE_STRING(macFilterCfg->direction, MDMVS_BOTH);
         break;
   }

   /* set and activate WanIpConnFirewallExceptionObject */
   ret = cmsObj_set(macFilterCfg, &iidStack);
   cmsObj_free((void **) &macFilterCfg);
      
   if (ret != CMSRET_SUCCESS)
   {
      CmsRet r2;
      cmsLog_error("Failed to set MacFilterCfgObject, ret = %d", ret);
             
      r2 = cmsObj_deleteInstance(MDMOID_MAC_FILTER_CFG, &iidStack);
      if (r2 != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to delete created MacFilterCfgObject, r2=%d", r2);
      }
      
      cmsLog_debug("Failed to set IP filter entry and successfully delete created MacFilterCfgObject");
   }
   return ret;
}


CmsRet dalSec_deleteMacFilter_igd(const char *protocol, const char *srcMac, const char *dstMac, const char *direction, const char *ifName)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   MacFilterCfgObject *macFilterCfg = NULL;
   InstanceIdStack parentIidStack = EMPTY_INSTANCE_ID_STACK;
   WanIpConnObject *wan_ip_conn = NULL;
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 found = FALSE;

   /* deleting a mac filter entry */
   cmsLog_debug("Deleting a mac filter entry with %s/%s/%s/%s/%s", protocol, srcMac, dstMac, direction, ifName);

   while (cmsObj_getNext(MDMOID_MAC_FILTER_CFG, &iidStack, (void **) &macFilterCfg) == CMSRET_SUCCESS)
   {
      if(!cmsUtl_strcmp(protocol, macFilterCfg->protocol) && !cmsUtl_strcmp(srcMac, macFilterCfg->sourceMAC) &&\
          !cmsUtl_strcmp(dstMac, macFilterCfg->destinationMAC) && !cmsUtl_strcmp(direction, macFilterCfg->direction) ) 
      {
         parentIidStack = iidStack;
         if ((ret = cmsObj_getAncestor(MDMOID_WAN_IP_CONN, MDMOID_MAC_FILTER_CFG, 
                                                           &parentIidStack, (void **) &wan_ip_conn)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Failed to get WanIpConnObject. ret=%d", ret);
            cmsObj_free((void **) &macFilterCfg);
            return ret;
         }

         if (!cmsUtl_strcmp(ifName, wan_ip_conn->X_BROADCOM_COM_IfName))
         {
            found = TRUE;
            cmsObj_free((void **) &macFilterCfg);
            cmsObj_free((void **) &wan_ip_conn);
            break;
         }
         cmsObj_free((void **) &macFilterCfg);
         cmsObj_free((void **) &wan_ip_conn);
      }
      else
      {
         cmsObj_free((void **) &macFilterCfg);
      }
   }

   if(found == FALSE) 
   {
      cmsLog_debug("no mac filter entry found: %s/%s/%s/%s/%s", protocol, srcMac, dstMac, direction, ifName);
      ret = CMSRET_INVALID_ARGUMENTS;
   }
   else 
   {
      ret = cmsObj_deleteInstance(MDMOID_MAC_FILTER_CFG, &iidStack);

      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to delete MacFilterCfgObject, ret = %d", ret);
      }
   }

   return ret;
}


CmsRet dalSec_ChangeMacFilterPolicy_igd(const char *ifName)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack parentIidStack = EMPTY_INSTANCE_ID_STACK;
   WanIpConnObject *wan_ip_conn = NULL;
   MacFilterCfgObject *macFilterCfg=NULL;
   MacFilterObject *macFilterObj=NULL;

   cmsLog_debug("Enter dalSec_ChangeMacFilterPolicy() with ifName = %s", ifName);

   while (cmsObj_getNext(MDMOID_WAN_IP_CONN, &parentIidStack, (void **) &wan_ip_conn) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strcmp(ifName, wan_ip_conn->X_BROADCOM_COM_IfName) == 0)
      {
         cmsObj_free((void **) &wan_ip_conn);
         break;
      }
      cmsObj_free((void **) &wan_ip_conn);
   }

   while(cmsObj_getNextInSubTree(MDMOID_MAC_FILTER_CFG, &parentIidStack, &iidStack, (void **)&macFilterCfg) == CMSRET_SUCCESS)
   {
      cmsObj_free((void **) &macFilterCfg);
	  
      ret = cmsObj_deleteInstance(MDMOID_MAC_FILTER_CFG, &iidStack);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Fail to delete MacFilterCfgObject in dalSec_ChangeMacFilterPolicy()");
         return CMSRET_INTERNAL_ERROR;
      }
      INIT_INSTANCE_ID_STACK(&iidStack);
   }
   
   INIT_INSTANCE_ID_STACK(&iidStack);
   if (cmsObj_getNextInSubTree(MDMOID_MAC_FILTER, &parentIidStack, &iidStack, (void **)&macFilterObj) != CMSRET_SUCCESS)
   {
      cmsLog_error("Fail to get MacFilterObject in dalSec_ChangeMacFilterPolicy()");
      return CMSRET_INTERNAL_ERROR;
   }

   macFilterObj->enable = TRUE;
   if (cmsUtl_strcmp(macFilterObj->policy, MDMVS_FORWARD) == 0)
   {
      CMSMEM_REPLACE_STRING(macFilterObj->policy, MDMVS_BLOCKED);
   }
   else if (cmsUtl_strcmp(macFilterObj->policy, MDMVS_BLOCKED) == 0)
   {
      CMSMEM_REPLACE_STRING(macFilterObj->policy, MDMVS_FORWARD);
   }

   /* set and activate MacFilterCfgObject */
   ret = cmsObj_set(macFilterObj, &iidStack);
   cmsObj_free((void **) &macFilterObj);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set macFilterObj, ret = %d", ret);
   }

   return ret;
}
#endif


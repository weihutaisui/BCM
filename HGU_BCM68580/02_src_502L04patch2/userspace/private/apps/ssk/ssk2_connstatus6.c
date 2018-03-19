/* 
* <:copyright-BRCM:2013:proprietary:standard
* 
*    Copyright (c) 2013 Broadcom 
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
* :>
*/

#ifdef DMP_DEVICE2_BASELINE_1

#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1 /* aka SUPPORT_IPV6 */

#include "cms.h"
#include "cms_util.h"
#include "cms_core.h"
#include "prctl.h"
#include "ssk.h"
#if defined (DMP_TIME_1) || defined (DMP_DEVICE2_TIME_1)
#include "ssk_time.h"
#endif
#include "cms_qdm.h"
#include "rut2_dns.h"

/*!\file ssk2_connstatus.c
 *
 * This file deals with status of ppp client, dhcp client, service state
 * machines for IPv6.  This is the TR181 equivalent to connstatus.c
 */

static UBOOL8 getDhcpv6ClientObjByPid(UINT32 pid,
                                 InstanceIdStack *dhcpClientIidStack,
                                 Dev2Dhcpv6ClientObject **dhcpClientObj);

static void fillInPppIpv6cpObject(const InstanceIdStack *pppIntfIidStack,
                      const char *localIntfId,
                      const char *remoteIntfId);

static void blankOutPppIpv6cpObject(const InstanceIdStack *pppIntfIidStack);


static void processRouteInfoIntfSetting(RAStatus6MsgBody *raInfo,
                      const char *intfFullPath);

#define IFADDRCONF_ADD     0
#define IFADDRCONF_REMOVE  1

void processDhcp6cStateChanged_dev2(const CmsMsgHeader *msg)
{
   Dhcp6cStateChangedMsgBody *dhcp6cInfo = (Dhcp6cStateChangedMsgBody *) (msg + 1);
   Dev2Dhcpv6ClientObject *dhcpClientObj=NULL;
   InstanceIdStack dhcpClientIidStack=EMPTY_INSTANCE_ID_STACK;
   Dev2Dhcp6cRcvOptionObject *dhcp6cRcvObj=NULL;
   InstanceIdStack dhcp6cRcvIidStack;
   Dev2IpInterfaceObject *ipIntfObj=NULL;
   InstanceIdStack ipIntfIidStack=EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found, pdChanged, dnsChanged;
   UINT32 pid;
   CmsRet ret;
   char ifName[CMS_IFNAME_LENGTH]={0};
   UBOOL8 connected=FALSE;
   
   pid = PID_IN_EID(msg->src);

   cmsLog_debug("pid=%d dataLength=%d", pid, msg->dataLength);
   cmsLog_debug("prefixAssigned=%d prefixCmd=%d sitePrefix=%s pdIfAddress=%s prefixPltime=%d prefixVltime=%d\n"
                "addrAssigned=%d addrCmd=%d address=%s ifname=%s\n"
                "dnsAssigned=%d nameserver=%s aftrAssigned=%d aftr=%s",
                dhcp6cInfo->prefixAssigned, dhcp6cInfo->prefixCmd, dhcp6cInfo->sitePrefix,
                dhcp6cInfo->pdIfAddress, dhcp6cInfo->prefixPltime, dhcp6cInfo->prefixVltime,
                dhcp6cInfo->addrAssigned, dhcp6cInfo->addrCmd, dhcp6cInfo->address, dhcp6cInfo->ifname, 
                dhcp6cInfo->dnsAssigned, dhcp6cInfo->nameserver, dhcp6cInfo->aftrAssigned, dhcp6cInfo->aftr);
   cmsLog_debug("maptAssigned=%d eaLen=%d v4prefix=%s v6prefix=%s psid=%x BR=%s",
                dhcp6cInfo->maptAssigned, dhcp6cInfo->eaLen, dhcp6cInfo->ruleIPv4Prefix,
                dhcp6cInfo->ruleIPv6Prefix, dhcp6cInfo->psid, dhcp6cInfo->brIPv6Prefix);

   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get lock, ret=%d", ret);
      /* hmm, this is bad, I can't update the MDM with new dhcp6c info */
      return;
   }

   found = FALSE;

   /* find the right dhcpv6 client object */
   found = getDhcpv6ClientObjByPid(pid, &dhcpClientIidStack, &dhcpClientObj);
   if (!found)
   {
      cmsLog_error("Could not find dhcpv6 client object with pid=%d", pid);
      cmsLck_releaseLock();
      return;
   }

   /* Get the IP.Interface object which this dhcp client runs on */
   if ((ret = getIpIntfByFullPath(dhcpClientObj->interface,
                           &ipIntfIidStack, &ipIntfObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get IP.Interface from dhcp client object!");
      cmsObj_free((void **) &dhcpClientObj);
      return;
   }

   dhcp6cRcvIidStack = dhcpClientIidStack;
   if ((ret = cmsObj_get(MDMOID_DEV2_DHCP6C_RCV_OPTION, &dhcp6cRcvIidStack,
                         OGF_NO_VALUE_UPDATE,
                         (void **)&dhcp6cRcvObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get DHCPv6.Client.{i}.X_BROADCOM_COM_RcvOption");
      cmsObj_free((void **) &dhcpClientObj);
      cmsObj_free((void **) &ipIntfObj);
      return;
   }

   /* 
    * Verify the difference between current dhcp6cRcvObj and new info
    * to decide teardown or not
    */
   pdChanged = cmsUtl_strcmp(dhcp6cRcvObj->prefix, dhcp6cInfo->sitePrefix);

   dnsChanged = cmsUtl_strcmp(dhcp6cRcvObj->DNSServers, dhcp6cInfo->nameserver);

   if ((dhcp6cInfo->prefixAssigned && (!(dhcp6cInfo->prefixCmd)?pdChanged:TRUE)) ||
        (dhcp6cInfo->dnsAssigned && dnsChanged))
   {
      cmsLog_debug("dhcp6c gets different PD or DNSServer");
   
      setIpv6ServiceStatusByFullPathLocked(dhcpClientObj->interface, MDMVS_SERVICESTARTING);
   }

   intfStack_setStatusByFullPathLocked(dhcpClientObj->interface, MDMVS_UP);
   cmsLog_debug("found the IpIntfObj");

#ifdef SUPPORT_TR69C
   if (strlen(dhcp6cInfo->acsURL) > 0 ||
       strlen(dhcp6cInfo->acsProvisioningCode) > 0 ||
       dhcp6cInfo->cwmpRetryMinimumWaitInterval != 0 ||
       dhcp6cInfo->cwmpRetryIntervalMultiplier != 0)
   {
      cmsLog_debug("got acsURL=%s provisioningCode=%s MinWaitInterval=%d intMult=%d",
                   dhcp6cInfo->acsURL,
                   dhcp6cInfo->acsProvisioningCode,
                   dhcp6cInfo->cwmpRetryMinimumWaitInterval,
                   dhcp6cInfo->cwmpRetryIntervalMultiplier);
      setAcsParams(dhcp6cInfo->acsURL, dhcp6cInfo->acsProvisioningCode,
                   dhcp6cInfo->cwmpRetryMinimumWaitInterval,
                   dhcp6cInfo->cwmpRetryIntervalMultiplier);
   }
#endif /* SUPPORT_TR69 */

#if defined (DMP_TIME_1) || defined (DMP_DEVICE2_TIME_1)
   if (strlen(dhcp6cInfo->ntpserver) > 0)
   {
      cmsLog_debug("got ntpserver(s)=%s", dhcp6cInfo->ntpserver);
      setNtpServers(dhcp6cInfo->ntpserver);
   }
#endif /* DMP_TIME_1 or DMP_DEVICE2_TIME_1 */

   /*
    * set the standard DHCP client stuff.
    * The RCL handler function for the Dev2Dhcp6cRcvOptionObject will create the
    * corresponding objects like IPv6Address.
    */

   /* dhcp6c receives an IPv6 address for WAN interface */
   if (dhcp6cInfo->addrAssigned)
   {
      if (dhcp6cInfo->addrCmd == IFADDRCONF_ADD)
      {
         cmsLog_debug("assigned IPv6 address");
         connected = TRUE;

         CMSMEM_REPLACE_STRING(dhcp6cRcvObj->address, dhcp6cInfo->address);
         dhcp6cRcvObj->addressPlt = dhcp6cInfo->addressPltime;
         dhcp6cRcvObj->addressVlt = dhcp6cInfo->addressVltime;
      }
      else  /* IFADDRCONF_REMOVE */
      {
         cmsLog_debug("FIXME: remove IPv6 address, update connstatus??");

         CMSMEM_FREE_BUF_AND_NULL_PTR(dhcp6cRcvObj->address);
      }
   }

   /* dhcp6c receives prefix delegation for LAN */
   if (dhcp6cInfo->prefixAssigned)
   {
      if (dhcp6cInfo->prefixCmd == IFADDRCONF_ADD &&
          dhcp6cInfo->prefixPltime != 0 && dhcp6cInfo->prefixVltime != 0)
      {
         cmsLog_debug("assigned prefix");

         connected = TRUE;

         CMSMEM_REPLACE_STRING(dhcp6cRcvObj->prefix, dhcp6cInfo->sitePrefix);
         dhcp6cRcvObj->prefixPlt = dhcp6cInfo->prefixPltime;
         dhcp6cRcvObj->prefixVlt = dhcp6cInfo->prefixVltime;

         if (dhcp6cInfo->prefixVltimeOld != 0)
         {
            CMSMEM_REPLACE_STRING(dhcp6cRcvObj->prefixOld, dhcp6cInfo->sitePrefixOld);
            dhcp6cRcvObj->prefixVltOld = dhcp6cInfo->prefixVltimeOld;
         }
      }
      else  /* IFADDRCONF_REMOVE: prefix remove */
      {
        cmsLog_debug("remove prefix");

        CMSMEM_FREE_BUF_AND_NULL_PTR(dhcp6cRcvObj->prefix);
        CMSMEM_FREE_BUF_AND_NULL_PTR(dhcp6cRcvObj->prefixOld);
        dhcp6cRcvObj->prefixPlt = 0;
        dhcp6cRcvObj->prefixVlt = 0;	
        dhcp6cRcvObj->prefixVltOld = 0;
      }
   }

   /* dhcp6c receives dns information */
   if (dhcp6cInfo->dnsAssigned)
   {
      cmsLog_debug("assigned nameserver=%s", dhcp6cInfo->nameserver);
      CMSMEM_REPLACE_STRING(dhcp6cRcvObj->DNSServers, dhcp6cInfo->nameserver);
   }

   /* dhcp6c receives domain name information */
   if ( dhcp6cInfo->domainNameAssigned && !IS_EMPTY_STRING(dhcp6cInfo->domainName) )
   {
      cmsLog_debug("assigned domain name=%s", dhcp6cInfo->domainName);
      CMSMEM_REPLACE_STRING(dhcp6cRcvObj->domainName, dhcp6cInfo->domainName);
   }

   /* 
    * dhcp6c receives AFTR information
    * TODO: should we update aftr even WAN is not connected?
    */
   if ( connected && dhcp6cInfo->aftrAssigned && !IS_EMPTY_STRING(dhcp6cInfo->aftr) )
   {
      cmsLog_debug("assigned aftr=%s", dhcp6cInfo->aftr);
      CMSMEM_REPLACE_STRING(dhcp6cRcvObj->aftr, dhcp6cInfo->aftr);
   }

   /* 
    * dhcp6c receives MAPT information
    * TODO: should we update MAPT even WAN is not connected?
    */
   if ( connected && dhcp6cInfo->maptAssigned && 
        !IS_EMPTY_STRING(dhcp6cInfo->ruleIPv4Prefix) && !IS_EMPTY_STRING(dhcp6cInfo->brIPv6Prefix))
   {
      cmsLog_debug("assigned MAPT");
      CMSMEM_REPLACE_STRING(dhcp6cRcvObj->maptBRPrefix, dhcp6cInfo->brIPv6Prefix);
      CMSMEM_REPLACE_STRING(dhcp6cRcvObj->maptRuleIPv4Prefix, dhcp6cInfo->ruleIPv4Prefix);
      CMSMEM_REPLACE_STRING(dhcp6cRcvObj->maptRuleIPv6Prefix, dhcp6cInfo->ruleIPv6Prefix);
      dhcp6cRcvObj->maptEALen = dhcp6cInfo->eaLen;
      dhcp6cRcvObj->maptPSIDOffset = dhcp6cInfo->psidOffset;
      dhcp6cRcvObj->maptPSIDLen = dhcp6cInfo->psidLen;
      dhcp6cRcvObj->maptPSID = dhcp6cInfo->psid;
      dhcp6cRcvObj->maptIsFMR = dhcp6cInfo->isFMR;
   }

   if ((ret = cmsObj_set(dhcp6cRcvObj, &dhcp6cRcvIidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("set of dhcp6cRcvObj failed, ret=%d", ret);
   }

   if (connected)
   {
      cmsLog_debug("sending WAN_CONNECTION_UP msg for IPv6 connection %s", ifName);

      setIpv6ServiceStatusByFullPathLocked(dhcpClientObj->interface, MDMVS_SERVICEUP);
   }

   cmsLck_releaseLock();

   cmsObj_free((void **) &ipIntfObj);
   cmsObj_free((void **) &dhcpClientObj);

   return;
}


UBOOL8 getDhcpv6ClientObjByPid(UINT32 pid,
                          InstanceIdStack *dhcpClientIidStack,
                          Dev2Dhcpv6ClientObject **dhcpClientObj)
{
   UBOOL8 found = FALSE;
   CmsRet ret;

   while (!found &&
          ((ret = cmsObj_getNext(MDMOID_DEV2_DHCPV6_CLIENT,
                                 dhcpClientIidStack,
                                 (void **) dhcpClientObj)) == CMSRET_SUCCESS))
   {
      if ((*dhcpClientObj)->X_BROADCOM_COM_Pid == pid)
      {
         /* found it, do not free it */
         found = TRUE;
      }
      else
      {
         cmsObj_free((void **) dhcpClientObj);
      }
   }

   return found;
}


void fillInPppIpv6cpObject(const InstanceIdStack *pppIntfIidStack,
                         const char *localIntfId,
                         const char *remoteIntfId)
{
   InstanceIdStack iidStack = *pppIntfIidStack;
   Dev2PppInterfaceIpv6cpObject *ipv6cpObj=NULL;
   CmsRet ret;

   /* Since there is exactly 1 IPCP object associated with this PPP
    * interface object, and we have the iidStack already, we can get
    * the object directly.
    */
   ret = cmsObj_get(MDMOID_DEV2_PPP_INTERFACE_IPV6CP, &iidStack,
                    OGF_NO_VALUE_UPDATE, (void **) &ipv6cpObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get IPv6CP object at iidStack %d, ret=%d",
                    cmsMdm_dumpIidStack(pppIntfIidStack), ret);
      return;
   }

   CMSMEM_REPLACE_STRING(ipv6cpObj->localInterfaceIdentifier, localIntfId);
   CMSMEM_REPLACE_STRING(ipv6cpObj->remoteInterfaceIdentifier, remoteIntfId);

   if ((ret = cmsObj_set(ipv6cpObj, &iidStack)) != CMSRET_SUCCESS)
   {
       cmsLog_error("Set of IPV6CP object failed, ret=%d", ret);
   }

   cmsObj_free((void **) &ipv6cpObj);

   return;

}

void blankOutPppIpv6cpObject(const InstanceIdStack *pppIntfIidStack)
{
   InstanceIdStack iidStack = *pppIntfIidStack;
   Dev2PppInterfaceIpv6cpObject *ipv6cpObj=NULL;
   CmsRet ret;

   /* Since there is exactly 1 IPCP object associated with this PPP
    * interface object, and we have the iidStack already, we can get
    * the object directly.
    */
   ret = cmsObj_get(MDMOID_DEV2_PPP_INTERFACE_IPV6CP, &iidStack,
                    OGF_NO_VALUE_UPDATE, (void **) &ipv6cpObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get IPv6CP object at iidStack %d, ret=%d",
                    cmsMdm_dumpIidStack(pppIntfIidStack), ret);
      return;
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(ipv6cpObj->localInterfaceIdentifier);
   CMSMEM_FREE_BUF_AND_NULL_PTR(ipv6cpObj->remoteInterfaceIdentifier);

   if ((ret = cmsObj_set(ipv6cpObj, &iidStack)) != CMSRET_SUCCESS)
    {
       cmsLog_error("Set of IPV6CP object failed, ret=%d", ret);
    }

   cmsObj_free((void **) &ipv6cpObj);

   return;
}


static void processRouteInfoIntfSetting(RAStatus6MsgBody *raInfo,
                      const char *intfFullPath)
{
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 found = FALSE;
   Dev2RouteInfoIntfSettingObject *intfSettingObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   char strTemp[BUFLEN_64];

   while (!found &&
              (cmsObj_getNextFlags(MDMOID_DEV2_ROUTE_INFO_INTF_SETTING, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&intfSettingObj) == CMSRET_SUCCESS))
   {
      if (!cmsUtl_strcmp(intfFullPath, intfSettingObj->interface))
      {
         found = TRUE;
         break;
      }
      cmsObj_free((void **) &intfSettingObj);
   }

   /* Not found! add instance for route information interface setting */
   if (!found)
   {
      INIT_INSTANCE_ID_STACK(&iidStack);
      if ((ret = cmsObj_addInstance(MDMOID_DEV2_ROUTE_INFO_INTF_SETTING, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Create Dev2RouteInfoIntfSettingObject failed!, ret=%d", ret);
         return;
      }
      if ((ret=cmsObj_get(MDMOID_DEV2_ROUTE_INFO_INTF_SETTING, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&intfSettingObj)) != CMSRET_SUCCESS)
      {
         cmsLog_error("could not find newly created instance!, ret=%d", ret);
         return;
      }
      CMSMEM_REPLACE_STRING(intfSettingObj->interface, intfFullPath);
      cmsLog_debug("create RouteInfoIntfSetting Object for %s", intfFullPath);
   }

   CMSMEM_REPLACE_STRING(intfSettingObj->status, MDMVS_FORWARDINGENTRYCREATED); 
   cmsTms_getXSIDateTime(raInfo->router_lifetime, strTemp, BUFLEN_64);
   CMSMEM_REPLACE_STRING(intfSettingObj->routeLifetime, strTemp); 
   sprintf(strTemp, "%s/%u", raInfo->pio_prefix, raInfo->pio_prefixLen);
   CMSMEM_REPLACE_STRING(intfSettingObj->prefix, strTemp);
   CMSMEM_REPLACE_STRING(intfSettingObj->sourceRouter, raInfo->router);
   switch(raInfo->router_P_flags)
   {
   case 0:
      CMSMEM_REPLACE_STRING(intfSettingObj->preferredRouteFlag, MDMVS_MEDIUM);
      break;
   case 1:
      CMSMEM_REPLACE_STRING(intfSettingObj->preferredRouteFlag, MDMVS_HIGH);
      break;
   case 2:
      CMSMEM_REPLACE_STRING(intfSettingObj->preferredRouteFlag, MDMVS_LOW);
      break;
   default:
      CMSMEM_REPLACE_STRING(intfSettingObj->preferredRouteFlag, MDMVS_MEDIUM);
      break;
   }

   cmsLog_debug("routeLifeTime:%s", intfSettingObj->routeLifetime);
   cmsLog_debug("prefix:%s", intfSettingObj->prefix);
   cmsLog_debug("sourceRouter:%s", intfSettingObj->sourceRouter);
   cmsLog_debug("preferredRouteFlag:%s", intfSettingObj->preferredRouteFlag);

   if ((ret = cmsObj_set(intfSettingObj, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Set of RouteInfoIntfSetting object failed, ret=%d", ret);
   }

   cmsObj_free((void **) &intfSettingObj);

}

/* FIXME: how to share rut functions? */
extern UBOOL8 rutIp_findIpv6Addr(const InstanceIdStack *iidStackIpIntf, const char *addr,
                                                  const char *origin, InstanceIdStack *iidStackIpv6Addr);
extern CmsRet rutIp_addIpv6Addr(const InstanceIdStack *iidStackIpIntf, const char *addr,
                                                  const char *origin, const char *prefix, int plt, int vlt);

extern UBOOL8 rutIp_findIpv6Prefix(const InstanceIdStack *iidStackIpIntf, const char *prefix,
                                                  const char *origin, const char *staticType, InstanceIdStack *iidStackIpv6Prefix);
extern CmsRet rutIp_addIpv6Prefix(const InstanceIdStack *iidStackIpIntf, const char *prefix,
                                                  const char *origin, const char *staticType, const char *parent,
                                                  const char *child, UBOOL8 onLink, UBOOL8 Autonomous,
                                                  int plt, int vlt, char *myPathRef, UINT32 pathLen);

UBOOL8 rutRt_findIpv6ForwardingObject_dev2(const char *ipIntfPathRef, const char *prefix, const char *nexthop, const char *origin, InstanceIdStack *iidStackIpv6Forward)
{
   Dev2Ipv6ForwardingObject *ForwardingObj=NULL;
   UBOOL8 found = FALSE;

   INIT_INSTANCE_ID_STACK(iidStackIpv6Forward);
   while (!found &&
              (cmsObj_getNextFlags(MDMOID_DEV2_IPV6_FORWARDING, iidStackIpv6Forward, OGF_NO_VALUE_UPDATE, (void **)&ForwardingObj) == CMSRET_SUCCESS))
   {
      if (!cmsUtl_strcmp(ForwardingObj->interface, ipIntfPathRef) && 
          !cmsUtl_strcmp(ForwardingObj->destIPPrefix, prefix) &&
          !cmsUtl_strcmp(ForwardingObj->nextHop, nexthop) &&
          !cmsUtl_strcmp(ForwardingObj->origin, origin))
      {
         found = TRUE;
      }

      cmsObj_free((void *) &ForwardingObj);
   }

   cmsLog_debug("found=%d", found);
   return found;
}

CmsRet rutRt_addIpv6ForwardingObject_dev2(const char *ipIntfPathRef, const char *prefix, const char *nexthop, const char *origin)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2RouterObject *routerObj=NULL;
   Dev2Ipv6ForwardingObject *ForwardingObj=NULL;
   CmsRet ret;

   cmsLog_debug("Enter: ipIntfPathRef=%s gatewayIPAddr=%s origin=%s",
                ipIntfPathRef, nexthop, origin);

   /*
    * Router object is created in mdm2_init.c
    */
   if ((ret = cmsObj_getNext(MDMOID_DEV2_ROUTER, &iidStack, (void **)&routerObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get MDMOID_DEV2_ROUTER Instance, ret = %d", ret);
      return ret;
   }
   /* we only want the iidStack, not the object */
   cmsObj_free((void *) &routerObj);

   if ((ret = cmsObj_addInstance(MDMOID_DEV2_IPV6_FORWARDING, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to add MDMOID_DEV2_IPV6_FORWARDING Instance, ret = %d", ret);
      return ret;
   } 

   if ((ret = cmsObj_get(MDMOID_DEV2_IPV6_FORWARDING, &iidStack, 0, (void **) &ForwardingObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get ipv6Forwarding, ret = %d", ret);
      return ret;
   }

   ForwardingObj->enable = TRUE;
   CMSMEM_REPLACE_STRING_FLAGS(ForwardingObj->interface, ipIntfPathRef, ALLOC_ZEROIZE);
   CMSMEM_REPLACE_STRING_FLAGS(ForwardingObj->destIPPrefix, prefix, ALLOC_ZEROIZE);
   CMSMEM_REPLACE_STRING_FLAGS(ForwardingObj->nextHop, nexthop, ALLOC_ZEROIZE);
   CMSMEM_REPLACE_STRING_FLAGS(ForwardingObj->origin, origin, ALLOC_ZEROIZE);
   CMSMEM_REPLACE_STRING_FLAGS(ForwardingObj->status, MDMVS_ENABLED, ALLOC_ZEROIZE);

   if((ret = cmsObj_set (ForwardingObj, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Set of ForwardingObj object failed, ret=%d", ret);
   }
   else
   {
      /*
       * If origin is not Static, this object instance should not be written
       * to config file.  So mark it as non-persistent.
       */
      if (cmsUtl_strcmp(origin, MDMVS_STATIC))
      {
         if ((ret = cmsObj_setNonpersistentInstance(MDMOID_DEV2_IPV6_FORWARDING, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Failed to set non-persistent ForwardingObj. ret=%d", ret);
         }
      }
   }

   cmsObj_free((void **)&ForwardingObj);

   cmsLog_debug("Exit, ret=%d", ret);
   
   return ret;
}


void processRAStatus6Info_dev2(const CmsMsgHeader *msg)
{
   CmsRet ret;
   RAStatus6MsgBody *raInfo = (RAStatus6MsgBody *) (msg + 1);
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   InstanceIdStack iidStackIpIntf;
   UBOOL8 found = FALSE;

   cmsLog_debug("PIO with L<%d> A<%d> plt<%u> vlt<%u> prefix<%s/%u>", 
                raInfo->pio_L_flag, raInfo->pio_A_flag, raInfo->pio_plt, 
                raInfo->pio_vlt, raInfo->pio_prefix, raInfo->pio_prefixLen);

   cmsLog_debug("dns with servers<%s>, lifetime<%d>", 
                raInfo->dns_servers, raInfo->dns_lifetime);

   cmsLog_debug("domain with domain<%s>, lifetime<%d>", 
                raInfo->domainName, raInfo->domainName_lifetime);

   cmsLog_debug("gwAddr<%s> rtrLife<%d> M<%d> O<%d> ifName<%s>", 
                raInfo->router, raInfo->router_lifetime, raInfo->router_M_flags, 
                raInfo->router_O_flags, raInfo->ifName);

#ifdef DMP_DEVICE2_HOMEPLUG_1
   /* HomePlug devices needs to monitor RA at LAN network */
#else
   /* No need to monitor RA at LAN network */
   if (cmsUtl_strncmp(raInfo->ifName, "br", 2) == 0)
   {
      cmsLog_debug("No need to monitor RA at LAN network");
      return;
   }
#endif

   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get lock, ret=%d, ramonitor update dropped!", ret);
      return;
   }

   INIT_INSTANCE_ID_STACK(&iidStackIpIntf);

   /* Search the WAN IP.Interface object */
   while ((!found) &&
              (cmsObj_getNextFlags(MDMOID_DEV2_IP_INTERFACE, &iidStackIpIntf,
                 OGF_NO_VALUE_UPDATE, (void **)&ipIntfObj) == CMSRET_SUCCESS))
   {
      if (!cmsUtl_strcmp(ipIntfObj->name, raInfo->ifName))
      {
         found = TRUE;
      }
      else
      {
         cmsObj_free((void **) &ipIntfObj);
      }
   }

   if (found)
   {
      MdmPathDescriptor pathDesc;
      char *fullPathStringPtr=NULL;
      UBOOL8 routerExist = FALSE;
      InstanceIdStack fwdIidStack;
      UBOOL8 mflag = (raInfo->router_M_flags?1:0);

      INIT_PATH_DESCRIPTOR(&pathDesc);

      pathDesc.oid = MDMOID_DEV2_IP_INTERFACE;
      pathDesc.iidStack = iidStackIpIntf;

      if ((ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &fullPathStringPtr)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsMdm_fullPathToPathDescriptor returns error. ret=%d", ret);
         cmsObj_free((void **) &ipIntfObj);
         return;
      }

      /* update Mflag if necessary */
      if (ipIntfObj->X_BROADCOM_COM_Mflag_Upstream != mflag)
      {
         ipIntfObj->X_BROADCOM_COM_Mflag_Upstream = mflag;

         if ((ret = cmsObj_set(ipIntfObj, &iidStackIpIntf)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Set of IP.Interface object failed, ret=%d", ret);
         }
      }
      cmsObj_free((void **) &ipIntfObj);

      /* update prefix to IPv6Prefix and address to IPv6Address */
      if (raInfo->pio_prefixLen != 0)
      {
         char prefixPathRef[MDM_SINGLE_FULLPATH_BUFLEN]={0};
         char prefix[CMS_IPADDR_LENGTH]={0};
         InstanceIdStack iidStackIpv6Prefix;

         sprintf(prefix, "%s/%d", raInfo->pio_prefix, raInfo->pio_prefixLen);

         /* FIXME: use length instead of real datetime for now */

         cmsLog_debug("RA prefix");

         if (!rutIp_findIpv6Prefix(&iidStackIpIntf, prefix, MDMVS_ROUTERADVERTISEMENT,
                              MDMVS_INAPPLICABLE, &iidStackIpv6Prefix))
         {
            cmsLog_debug("cannot find ipv6prefix obj from RA");
            rutIp_addIpv6Prefix(&iidStackIpIntf, prefix, MDMVS_ROUTERADVERTISEMENT,
                 MDMVS_INAPPLICABLE, NULL, NULL, (raInfo->pio_L_flag?1:0), (raInfo->pio_A_flag?1:0),
                 raInfo->pio_plt, raInfo->pio_vlt, prefixPathRef, sizeof(prefixPathRef));

            if (raInfo->pio_A_flag)
            {
               char guAddr[CMS_IPADDR_LENGTH]={0};
               UINT32 prefixLen=0;
               CmsRet r2;
      
               r2 = cmsNet_getGloballyUniqueIfAddr6(raInfo->ifName, guAddr, &prefixLen);
               if (CMSRET_SUCCESS == r2)
               {
                  /* Address should be configured by kernel already. Fetch the address and set to data model */
                  rutIp_addIpv6Addr(&iidStackIpIntf, guAddr, MDMVS_AUTOCONFIGURED, prefixPathRef, raInfo->pio_plt, raInfo->pio_vlt);
               }
               else
               {
                  cmsLog_debug("A flag is 1 but no address from kernel?");
               }
            }
         }
         else
         {
            cmsLog_debug("ipv6prefix obj from RA exists!");
         }
      }

      /* update RouteInfo.InterfaceSetting object */
      processRouteInfoIntfSetting(raInfo, fullPathStringPtr);

      /* update router object */
      routerExist = rutRt_findIpv6ForwardingObject_dev2(fullPathStringPtr, "", raInfo->router, MDMVS_RA, &fwdIidStack);

      if (raInfo->router_lifetime)
      {
         if (!routerExist)
         {
            rutRt_addIpv6ForwardingObject_dev2(fullPathStringPtr, "", raInfo->router, MDMVS_RA);
         }
      }
      else
      {
         if (routerExist)
         {
            cmsLog_debug("router lifetime is 0: delete obj");
            if ((ret = cmsObj_deleteInstance(MDMOID_DEV2_IPV6_FORWARDING, &fwdIidStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("cmsObj_deleteInstance<MDMOID_DEV2_IPV6_FORWARDING> returns error. ret=%d", ret);
            }
         }
      }

      /* update DNS object */
      if (raInfo->dns_lifetime)
      {
//         REPLACE_STRING_IF_NOT_EQUAL(ipConn->X_BROADCOM_COM_IPv6DNSServers, raInfo->dns_servers);
      }

      CMSMEM_FREE_BUF_AND_NULL_PTR(fullPathStringPtr);
   }

   /* FIXME: ITU related stuff */

   cmsLck_releaseLock();
}


void processPppv6StateUp_dev2(const InstanceIdStack *pppIntfIidStack, const char *localIntfId, const char *remoteIntfId)
{
   MdmPathDescriptor pathDesc=EMPTY_PATH_DESCRIPTOR;

   cmsLog_debug("Entered: set DEV2_PPP instance %s UP and propagate",
                 cmsMdm_dumpIidStack(pppIntfIidStack));

   /* Record the info from ppp server in PPP.Interface.{i}.IPv6CP obj */
   fillInPppIpv6cpObject(pppIntfIidStack, localIntfId, remoteIntfId);

   /* set PPP connStatus to CONNECTED */
   sskConn_setPppConnStatusByIidLocked(pppIntfIidStack, MDMVS_CONNECTED);


   /* set PPP interface status to UP and propagate */
   pathDesc.oid = MDMOID_DEV2_PPP_INTERFACE;
   pathDesc.iidStack = *pppIntfIidStack;

   intfStack_setStatusByPathDescLocked(&pathDesc, MDMVS_UP);
   intfStack_propagateStatusByIidLocked(MDMOID_DEV2_PPP_INTERFACE,
                                        pppIntfIidStack,
                                        MDMVS_UP);
}


void processPppv6StateDown_dev2(const InstanceIdStack *pppIntfIidStack)
{
   MdmPathDescriptor pathDesc=EMPTY_PATH_DESCRIPTOR;

   cmsLog_debug("Entered: set DEV2_PPP instance %s DOWN and propagate",
                 cmsMdm_dumpIidStack(pppIntfIidStack));

   blankOutPppIpv6cpObject(pppIntfIidStack);

   /* set PPP connStatus to CONNECTED */
   sskConn_setPppConnStatusByIidLocked(pppIntfIidStack, MDMVS_DISCONNECTED);

   /* set PPP interface status to LOWERLAYERDOWN and propagate */
   pathDesc.oid = MDMOID_DEV2_PPP_INTERFACE;
   pathDesc.iidStack = *pppIntfIidStack;

   intfStack_setStatusByPathDescLocked(&pathDesc, MDMVS_LOWERLAYERDOWN);
   intfStack_propagateStatusByIidLocked(MDMOID_DEV2_PPP_INTERFACE,
                                        pppIntfIidStack,
                                        MDMVS_LOWERLAYERDOWN);
}


void setIpv6ServiceStatusByFullPathLocked(const char *ipIntfFullPath, const char *serviceStatus)
{
   MdmPathDescriptor pathDesc;
   CmsRet ret;

   INIT_PATH_DESCRIPTOR(&pathDesc);
   ret = cmsMdm_fullPathToPathDescriptor(ipIntfFullPath, &pathDesc);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_fullPathToPathDescriptor failed for %s, ret=%d",
                   ipIntfFullPath, ret);
      return;
   }

   if (pathDesc.oid != MDMOID_DEV2_IP_INTERFACE)
   {
      cmsLog_error("fullPath %s must point to IP.Interface", ipIntfFullPath);
      return;
   }

   setIpv6ServiceStatusByIidLocked(&pathDesc.iidStack, serviceStatus);

   return;
}


void setIpv6ServiceStatusByIidLocked(const InstanceIdStack *ipIntfIidStack, const char *serviceStatus)
{
   Dev2IpInterfaceObject *ipIntfObj=NULL;
   CmsRet ret;

   if ((ret = cmsObj_get(MDMOID_DEV2_IP_INTERFACE, ipIntfIidStack,
                         OGF_NO_VALUE_UPDATE,
                         (void **)&ipIntfObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get IP.Interface object, ret=%d", ret);
      return;
   }

   if (ipIntfObj->IPv6Enable)
   {
      CMSMEM_REPLACE_STRING(ipIntfObj->X_BROADCOM_COM_IPv6ServiceStatus, serviceStatus);

      if ((ret = cmsObj_set(ipIntfObj, ipIntfIidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Set of IP.Interface object failed, ret=%d", ret);
      }
   }

   cmsObj_free((void **) &ipIntfObj);

   return;
}


void getIpv6ServiceStatusByIidLocked(const InstanceIdStack *ipIntfIidStack,
                                     char *serviceStatus, UINT32 bufLen)
{
   Dev2IpInterfaceObject *ipIntfObj=NULL;
   CmsRet ret;

   if ((ret = cmsObj_get(MDMOID_DEV2_IP_INTERFACE, ipIntfIidStack,
                         OGF_NO_VALUE_UPDATE,
                         (void **)&ipIntfObj)) != CMSRET_SUCCESS)
   {
      cmsLog_debug("Could not get IP.Interface object at %s, ret=%d",
                   cmsMdm_dumpIidStack(ipIntfIidStack), ret);
      /* For intf grouping, the entire IP.Interface subtree could get
       * deleted, so we cannot see the IP.Interface status.  Just report a
       * DOWN status.
       */
      cmsUtl_strncpy(serviceStatus, MDMVS_DOWN, bufLen);
      return;
   }

   if (cmsUtl_strlen(ipIntfObj->X_BROADCOM_COM_IPv6ServiceStatus) >= (SINT32) bufLen)
   {
      cmsLog_error("status %s cannot fit into bufLen %d",
            ipIntfObj->X_BROADCOM_COM_IPv6ServiceStatus, bufLen);
   }
   else
   {
      strcpy(serviceStatus, ipIntfObj->X_BROADCOM_COM_IPv6ServiceStatus);
   }

   cmsObj_free((void **) &ipIntfObj);

   return;
}


UBOOL8 sskConn_hasStaticIpv6AddressLocked(const InstanceIdStack *ipIntfIidStack)
{
   MdmPathDescriptor pathDesc;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2Ipv6AddressObject *ipv6AddrObj=NULL;
   UBOOL8 hasAddr=FALSE;
   CmsRet ret;

   INIT_PATH_DESCRIPTOR(&pathDesc);
   while (!hasAddr &&
          ((ret = cmsObj_getNextInSubTreeFlags(MDMOID_DEV2_IPV6_ADDRESS,
                              ipIntfIidStack,
                              &iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **)&ipv6AddrObj)) == CMSRET_SUCCESS))
   {
      if (!cmsUtl_strcmp(ipv6AddrObj->origin, MDMVS_STATIC) &&
          !cmsUtl_isZeroIpvxAddress(CMS_AF_SELECT_IPVX, ipv6AddrObj->IPAddress))
      {
         hasAddr = TRUE;
      }
      cmsObj_free((void **) &ipv6AddrObj);
   }

   return hasAddr;
}


UBOOL8 sskConn_hasAnyIpv6AddressLocked(const InstanceIdStack *ipIntfIidStack)
{
   MdmPathDescriptor pathDesc;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2Ipv6AddressObject *ipv6AddrObj=NULL;
   UBOOL8 hasAddr=FALSE;
   CmsRet ret;

   INIT_PATH_DESCRIPTOR(&pathDesc);
   while (!hasAddr &&
          ((ret = cmsObj_getNextInSubTreeFlags(MDMOID_DEV2_IPV6_ADDRESS,
                              ipIntfIidStack,
                              &iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **)&ipv6AddrObj)) == CMSRET_SUCCESS))
   {
      if (!cmsUtl_isZeroIpvxAddress(CMS_AF_SELECT_IPVX, ipv6AddrObj->IPAddress))
      {
         hasAddr = TRUE;
      }
      cmsObj_free((void **) &ipv6AddrObj);
   }

   return hasAddr;
}

#ifdef SUPPORT_CELLULAR
void processCellularState6Changed(DevCellularDataCallStateChangedBody *cellularInfo)
{
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   WanIpConnObject *ipConn=NULL;
   InstanceIdStack iidStackIpIntf = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack;
   InstanceIdStack savedIidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found=FALSE, isChange = FALSE;
   CmsRet ret;

   cmsLog_notice("ifname=%s, active=%d, ip=%s, gateway=%s, nameserver=%s", 
                cellularInfo->ifname, cellularInfo->active, cellularInfo->addresses, 
                cellularInfo->gateways, cellularInfo->dnses);
   
   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get lock, ret=%d", ret);
      return;
   }

   while ((!found) &&
          (ret = cmsObj_getNext(MDMOID_DEV2_IP_INTERFACE, &iidStackIpIntf,
                               (void **)&ipIntfObj)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(ipIntfObj->name, cellularInfo->ifname))
      {
         found = TRUE;

         cmsLog_debug("IPv6Enable=%d, status=%s", ipIntfObj->IPv6Enable, ipIntfObj->status);
		
        /*Cellular ip will keep same after registration, so we only monior up and down*/
         if (ipIntfObj->IPv6Enable &&
             ((cellularInfo->active && cmsUtl_strcmp(ipIntfObj->status, MDMVS_UP)) ||
             (cellularInfo->active && !cmsUtl_strcmp(ipIntfObj->status, MDMVS_UP))))
         {
           isChange = TRUE;
         }
      }
	  
      cmsObj_free((void **) &ipIntfObj);
   }
 
 
   if (!found)
   {
      cmsLog_notice("ipIntfObj not found for %s, ret = %d", cellularInfo->ifname, ret);
      cmsLck_releaseLock();
      return;

   }
   else if (!isChange)
   {
      cmsLog_debug("ipIntfObj status not change for %s", cellularInfo->ifname);
      cmsLck_releaseLock();
      return;
   }

   if (cellularInfo->active)
   {
      Dev2DnsServerObject *dnsServerObj=NULL;
      MdmPathDescriptor pathDesc;      
      char *ipIfFullPathStringPtr=NULL;

      INIT_PATH_DESCRIPTOR(&pathDesc);
      pathDesc.oid = MDMOID_DEV2_IP_INTERFACE;
      pathDesc.iidStack = iidStackIpIntf;
 
      if ((ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &ipIfFullPathStringPtr)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsMdm_fullPathToPathDescriptor returns error. ret=%d", ret);
         cmsLck_releaseLock();
         return;
      }
 

/*Sarah: There's 2 kind of address assign, by PDN GW or RA,  
   At this time, rild didn't tell ip6 addr/gw is assgined by PDN GW or RA,
   so config only DNS, later to support full address
*/
#if 0	  
      Dev2Ipv6PrefixObject *ipv6Prefix = NULL;   
      Dev2Ipv6AddressObject *ipv6AddrObj = NULL;
      Dev2RouterObject *routerObj=NULL;
      Dev2Ipv6ForwardingObject *ForwardingObj=NULL;
      char *fullPathStringPtr=NULL;
      char addrtmp[CMS_IPADDR_LENGTH];
      char *tmp;   
 
      if (!(tmp = strchr(cellularInfo->addresses, '/')))
      {
         strcat(cellularInfo->addresses, "/64");
      }    
 
      /*  Get or Add ipv6 prefix object */
      INIT_INSTANCE_ID_STACK(&iidStack);
      if ((ret = cmsObj_getNextInSubTreeFlags(MDMOID_DEV2_IPV6_PREFIX, &iidStackIpIntf, &iidStack,
                           OGF_NO_VALUE_UPDATE, (void **)&ipv6Prefix)) != CMSRET_SUCCESS)
      {
         iidStack = iidStackIpIntf;
         if ((ret = cmsObj_addInstance(MDMOID_DEV2_IPV6_PREFIX, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Failed to add device2.ip.interface.{i}.IPv6Prefix, ret = %d", ret);
            cmsLck_releaseLock();
            return;
         }
      }		 
 	 
      if ((ret = cmsObj_get(MDMOID_DEV2_IPV6_PREFIX, &iidStack, 0, (void **) &ipv6Prefix)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to get device2.ip.interface.{i}.IPv6Preifx, ret = %d", ret);
         cmsLck_releaseLock();
         return;
      }
 		 
      ipv6Prefix->enable = TRUE;
      CMSMEM_REPLACE_STRING(ipv6Prefix->prefix, cellularInfo->addresses);      
      CMSMEM_REPLACE_STRING(ipv6Prefix->origin, MDMVS_X_BROADCOM_COM_NAS);
      CMSMEM_REPLACE_STRING(ipv6Prefix->staticType, MDMVS_INAPPLICABLE);
   
      cmsLog_debug("cmsObj_set ipv6Prefix = %s", ipv6Prefix->prefix);
      if ((ret = cmsObj_set(ipv6Prefix, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed setting ipPrefix. ret %d", ret);
         cmsObj_free((void **) &ipv6Prefix);
         cmsLck_releaseLock();
         return;
      }
   
      cmsObj_free((void **) &ipv6Prefix);
 
      memset(&pathDesc, 0, sizeof(MdmPathDescriptor));
      pathDesc.oid = MDMOID_DEV2_IPV6_PREFIX;
      pathDesc.iidStack = iidStack;
      if ((ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &fullPathStringPtr)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsMdm_pathDescriptorToFullPathNoEndDot returns error. ret=%d", ret);
         cmsLck_releaseLock();
         return;
      }
 
      /*  Get or Add ipv6 address object */ 
      INIT_INSTANCE_ID_STACK(&iidStack);
      if ((ret = cmsObj_getNextInSubTreeFlags(MDMOID_DEV2_IPV6_ADDRESS, &iidStackIpIntf, &iidStack,
                           OGF_NO_VALUE_UPDATE, (void **)&ipv6AddrObj)) != CMSRET_SUCCESS)
      {
        iidStack = iidStackIpIntf;
         if ((ret = cmsObj_addInstance(MDMOID_DEV2_IPV6_ADDRESS, &iidStack)) != CMSRET_SUCCESS)
         {
            CMSMEM_FREE_BUF_AND_NULL_PTR(fullPathStringPtr);
            cmsLog_error("Failed to add MDMOID_DEV2_IPV6_ADDRESS Instance, ret = %d", ret);
            cmsLck_releaseLock();
            return;
         } 
      } 
   
      if ((ret = cmsObj_get(MDMOID_DEV2_IPV6_ADDRESS, &iidStack, 0, (void **) &ipv6AddrObj)) != CMSRET_SUCCESS)
      {
 
         CMSMEM_FREE_BUF_AND_NULL_PTR(fullPathStringPtr);
         cmsLog_error("Failed to get ipv6AddrObj, ret = %d", ret);
         cmsLck_releaseLock();
         return;
      }
   
      ipv6AddrObj->enable = TRUE;
      cmsUtl_strncpy(addrtmp, cellularInfo->addresses, sizeof(addrtmp)-1);
      tmp = strchr(addrtmp, '/');
      *tmp = '\0';
      CMSMEM_REPLACE_STRING(ipv6AddrObj->IPAddress, addrtmp);
      CMSMEM_REPLACE_STRING(ipv6AddrObj->prefix, fullPathStringPtr);
      CMSMEM_REPLACE_STRING(ipv6AddrObj->origin, MDMVS_X_BROADCOM_COM_NAS);
      ipv6AddrObj->enable = TRUE;
 	 
      cmsLog_debug("cmsObj_set IPAddress = %s", ipv6AddrObj->IPAddress);
      if ((ret = cmsObj_set(ipv6AddrObj, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to set ipv6AddrObj. ret=%d", ret);
         cmsObj_free((void **) &ipv6AddrObj); 
         cmsLck_releaseLock();
         return;
      } 
 	 
      CMSMEM_FREE_BUF_AND_NULL_PTR(fullPathStringPtr);
      cmsObj_free((void **) &ipv6AddrObj); 
 
 
      /* Setup routing interface and ipv6 forwarding*/
      INIT_INSTANCE_ID_STACK(&iidStack);
      if ((ret = cmsObj_getNext(MDMOID_DEV2_ROUTER, &iidStack, (void **)&routerObj)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not get MDMOID_DEV2_ROUTER, ret=%d", ret);
         cmsLck_releaseLock();
         return;
      }
   
 
      CMSMEM_REPLACE_STRING(routerObj->X_BROADCOM_COM_DefaultIpv6ConnectionServices,
                            cellularInfo->ifname);
   
      cmsLog_debug("cmsObj_set DefaultIpv6ConnectionServices = %s", 
                    routerObj->X_BROADCOM_COM_DefaultIpv6ConnectionServices);
      ret = cmsObj_set(routerObj, &iidStack);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Set of routerObj failed, ret=%d", ret);
         cmsObj_free((void **) &routerObj);
         cmsLck_releaseLock();
         return;
      }
   
      cmsObj_free((void **) &routerObj);
 

      found = FALSE;
      INIT_INSTANCE_ID_STACK(&iidStack);
      while (!found &&
            (cmsObj_getNextFlags(MDMOID_DEV2_IPV6_FORWARDING, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&ForwardingObj) == CMSRET_SUCCESS))
      {
         if (!cmsUtl_strcmp(ForwardingObj->interface, ipIfFullPathStringPtr))
         {
            found = TRUE;
         }
   
         cmsObj_free((void *) &ForwardingObj);
      }
   
      if (!found)
      {
         INIT_INSTANCE_ID_STACK(&iidStack);
 		PUSH_INSTANCE_ID(&iidStack, 1);
         if ((ret = cmsObj_addInstance(MDMOID_DEV2_IPV6_FORWARDING, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Failed to add MDMOID_DEV2_IPV6_FORWARDING Instance, ret = %d", ret);
            CMSMEM_FREE_BUF_AND_NULL_PTR(ipIfFullPathStringPtr);
            cmsLck_releaseLock();
            return;
         } 
      } 
      
      if ((ret = cmsObj_get(MDMOID_DEV2_IPV6_FORWARDING, &iidStack, 0, (void **) &ForwardingObj)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to get ipv6Forwarding, ret = %d", ret);
         CMSMEM_FREE_BUF_AND_NULL_PTR(ipIfFullPathStringPtr);
         cmsLck_releaseLock();
         return;
      }
   
      ForwardingObj->enable = TRUE;
      CMSMEM_REPLACE_STRING_FLAGS(ForwardingObj->interface, ipIfFullPathStringPtr, ALLOC_ZEROIZE);
      CMSMEM_REPLACE_STRING_FLAGS(ForwardingObj->destIPPrefix, "default", ALLOC_ZEROIZE);
      CMSMEM_REPLACE_STRING_FLAGS(ForwardingObj->nextHop, cellularInfo->gateways, ALLOC_ZEROIZE);
      CMSMEM_REPLACE_STRING_FLAGS(ForwardingObj->origin, MDMVS_X_BROADCOM_COM_NAS, ALLOC_ZEROIZE);
      CMSMEM_REPLACE_STRING_FLAGS(ForwardingObj->status, MDMVS_ENABLED, ALLOC_ZEROIZE);
         
      if((ret = cmsObj_set (ForwardingObj, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Set of ForwardingObj object failed, ret=%d", ret);
         cmsObj_free((void *) &ForwardingObj);
         cmsLck_releaseLock();
         return;
      }
	  
      cmsObj_free((void *) &ForwardingObj);
 #endif

      /* Clear and Setup DNS*/
      INIT_INSTANCE_ID_STACK(&iidStack);
      while(cmsObj_getNextFlags(MDMOID_DEV2_DNS_SERVER, &iidStack,
                                OGF_NO_VALUE_UPDATE, (void **) &dnsServerObj) == CMSRET_SUCCESS)
      {
         if (!cmsUtl_strcmp(dnsServerObj->type, MDMVS_X_BROADCOM_COM_NAS))
         {
            cmsObj_deleteInstance(MDMOID_DEV2_DNS_SERVER, &iidStack);
            iidStack = savedIidStack;
         }
   
         cmsObj_free((void **)&dnsServerObj);
         savedIidStack = iidStack;
      }
	  
      if ((ret = rutDns_addServerObject_dev2(ipIfFullPathStringPtr, cellularInfo->dnses, 
                                                    MDMVS_X_BROADCOM_COM_NAS)) != CMSRET_SUCCESS)
      {
         cmsLog_error("rutDns_addServerObject_dev2 failed. ret %d", ret);
         CMSMEM_FREE_BUF_AND_NULL_PTR(ipIfFullPathStringPtr);
         cmsLck_releaseLock();
         return;
     }    
	  
     CMSMEM_FREE_BUF_AND_NULL_PTR(ipIfFullPathStringPtr);
   }

   /* propagateStatus */
   /*Sarah: TODO: TR181 implement*/  
   found = FALSE;   
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((!found) && 
          ((ret = cmsObj_getNext(MDMOID_WAN_IP_CONN, &iidStack, (void **) &ipConn)) == CMSRET_SUCCESS))
   {		 
      if (!(cmsUtl_strcmp(ipConn->X_BROADCOM_COM_IfName, cellularInfo->ifname)))
      {
         found = TRUE;
      }
	  
      cmsObj_free((void **) &ipConn);
   }
	
   if (found)
   {
      if (cellularInfo->active)
      {     
         cmsLog_debug("injecting status for MDMOID_WAN_IP_CONN %s, active=%d", cellularInfo->ifname, cellularInfo->active);
         intfStack_propagateStatusByIidLocked(MDMOID_WAN_IP_CONN, &iidStack,
                                              (cellularInfo->active ? MDMVS_UP : MDMVS_DOWN));
      }
      else
      {
         InstanceIdStack linkIidStack = iidStack;
   	  
         cmsLog_debug("send link down message");
         sendStatusMsgToSmd(CMS_MSG_WAN_LINK_DOWN, cellularInfo->ifname);
   
         linkIidStack.currentDepth--;
         updateWanConnStatusInSubtreeLocked(&linkIidStack, FALSE);
      }   
   }
   else
   {
         cmsLog_notice("MDMOID_WAN_IP_CONN not found for %s", cellularInfo->ifname);
   }
   
   cmsLck_releaseLock();
   
   return;
}
#endif  /* SUPPORT_CELLULAR */

#endif  /* DMP_X_BROADCOM_COM_DEV2_IPV6_1 */

#endif  /* DMP_DEVICE2_BASELINE_1 */

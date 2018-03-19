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

#ifdef DMP_BASELINE_1  
#ifdef DMP_X_BROADCOM_COM_IPV6_1 /* aka SUPPORT_IPV6 */


#include "cms.h"
#include "cms_mdm.h"
#include "cms_obj.h"
#include "cms_qdm.h"
#include "cms_util.h"


/*!\file qdm_ipv6.c
 *
 */
CmsRet qdmIpv6_getLanULAAddr6_igd(const char *ifname, char *addr)
{
   InstanceIdStack iidStack;
   LanIpIntfObject *lanObj = NULL;
   IPv6LanIntfAddrObject *ipv6lanObj = NULL;
   CmsRet ret = CMSRET_SUCCESS;
   
   if (addr == NULL)
   {
      cmsLog_error("NULL string.");
      return CMSRET_INVALID_ARGUMENTS;
   }
   *addr = '\0';

   INIT_INSTANCE_ID_STACK(&iidStack);
   while (cmsObj_getNext(MDMOID_LAN_IP_INTF, &iidStack, (void **)&lanObj) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strcmp(ifname, lanObj->X_BROADCOM_COM_IfName) == 0)
      {
      	 if(cmsObj_get(MDMOID_I_PV6_LAN_INTF_ADDR, &iidStack, 0, (void **)&ipv6lanObj) == CMSRET_SUCCESS)
      	 {
            if (!IS_EMPTY_STRING(ipv6lanObj->uniqueLocalAddress))
            {
               strcpy(addr, ipv6lanObj->uniqueLocalAddress);
            }
            cmsObj_free((void **)&ipv6lanObj);
       	 }
         cmsObj_free((void **)&lanObj);
         break;
      }
      cmsObj_free((void **)&lanObj);
   }   

   return ret;
}  /* End of qdmIpv6_getLanULAAddr6() */

CmsRet qdmIpv6_getDns6Info_igd(char *dnsType, char *dnsIfc, char *dnsPri, char *dnsSec)
{
   InstanceIdStack iidStack;
   IPv6LanHostCfgObject *ipv6Obj = NULL;
   CmsRet ret = CMSRET_SUCCESS;
   
   if (dnsType == NULL && dnsIfc == NULL && dnsPri == NULL && dnsSec == NULL)
   {
      cmsLog_error("NULL string.");
      return CMSRET_INVALID_ARGUMENTS;
   }
   if (dnsType != NULL)
   {
      *dnsType = '\0';
   }
   if (dnsIfc != NULL)
   {
      *dnsIfc = '\0';
   }
   if (dnsPri != NULL)
   {
      *dnsPri = '\0';
   }
   if (dnsSec != NULL)
   {
      *dnsSec = '\0';
   }
         
   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_getNext(MDMOID_I_PV6_LAN_HOST_CFG, &iidStack, (void **)&ipv6Obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_getNext <MDMOID_I_PV6_LAN_HOST_CFG> returns error. ret=%d", ret);
      return ret;
   }   

   if (dnsType != NULL)
   {
      strcpy(dnsType, ipv6Obj->IPv6DNSConfigType);
   }
   
   if (dnsIfc != NULL)
   {
      if (!IS_EMPTY_STRING(ipv6Obj->IPv6DNSWANConnection))
      {
         strcpy(dnsIfc, ipv6Obj->IPv6DNSWANConnection);
      }
   }
   
   if (dnsPri != NULL || dnsSec != NULL)
   {
      char dnsServers[BUFLEN_128];

      *dnsServers = '\0';

      if (!IS_EMPTY_STRING(ipv6Obj->IPv6DNSServers))
      {
         strcpy(dnsServers, ipv6Obj->IPv6DNSServers); 
      }
      else if (!IS_EMPTY_STRING(ipv6Obj->IPv6DNSWANConnection))
      {
         InstanceIdStack iidStacktmp = EMPTY_INSTANCE_ID_STACK;
         WanIpConnObject *ipConnObj=NULL;
         WanPppConnObject *pppConnObj=NULL;
         UBOOL8 found = FALSE;

         while (!found &&
                (ret = cmsObj_getNext(MDMOID_WAN_IP_CONN, &iidStacktmp, (void **) &ipConnObj)) == CMSRET_SUCCESS)
         {
            if (cmsUtl_strcmp(ipConnObj->X_BROADCOM_COM_IfName, ipv6Obj->IPv6DNSWANConnection) == 0)
            {
               found = TRUE;

               if (!IS_EMPTY_STRING(ipConnObj->X_BROADCOM_COM_IPv6DNSServers))
               {
                  strcpy(dnsServers, ipConnObj->X_BROADCOM_COM_IPv6DNSServers); 
               }         
            }
      
            cmsObj_free((void **) &ipConnObj);
         }
      
         INIT_INSTANCE_ID_STACK(&iidStacktmp);
         while (!found &&
                (ret = cmsObj_getNext(MDMOID_WAN_PPP_CONN, &iidStacktmp, (void **) &pppConnObj)) == CMSRET_SUCCESS)
         {
            if (cmsUtl_strcmp(pppConnObj->X_BROADCOM_COM_IfName, ipv6Obj->IPv6DNSWANConnection) == 0)
            {
               found = TRUE;

               if (!IS_EMPTY_STRING(pppConnObj->X_BROADCOM_COM_IPv6DNSServers))
               {
                  strcpy(dnsServers, pppConnObj->X_BROADCOM_COM_IPv6DNSServers); 
               }
            }
      
            cmsObj_free((void **) &pppConnObj);
         }

      }

      if ((ret = cmsUtl_parseDNS(dnsServers, dnsPri, dnsSec, FALSE)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsUtl_parseDNS returns error. ret=%d", ret);
      }
   }

   cmsObj_free((void **)&ipv6Obj);
   return ret;        
    
}  /* End of qdmIpv6_getDns6Info_igd() */

void qdmIpv6_getDhcp6sInfo_igd(UBOOL8 *stateful, char *intfIDStart, char *intfIDEnd, SINT32 *leasedTime)
{
   InstanceIdStack iidStack;
   IPv6LanHostCfgObject *ipv6Obj = NULL;
   CmsRet ret;

   cmsLog_debug("Enter");
   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_getNext(MDMOID_I_PV6_LAN_HOST_CFG, &iidStack, (void **)&ipv6Obj)) == CMSRET_SUCCESS)
   {
      *stateful = ipv6Obj->statefulDHCPv6Server;

      if ( ipv6Obj->minInterfaceID )
      {
         cmsUtl_strncpy(intfIDStart, ipv6Obj->minInterfaceID, 
                        CMS_IPADDR_LENGTH);
      }

      if ( ipv6Obj->maxInterfaceID )
      {
         cmsUtl_strncpy(intfIDEnd, ipv6Obj->maxInterfaceID, 
                        CMS_IPADDR_LENGTH);
      }

      /* webVar is in hours, MDM in seconds */
      *leasedTime = ipv6Obj->DHCPv6LeaseTime/3600; 

      cmsObj_free((void **)&ipv6Obj);
   }
   else
   {
      cmsLog_error("cmsObj_get returns error. ret=%d", ret);
   }

   cmsLog_debug("enblDhcp6sStateful<%d> minIID<%s> maxIID<%s> leaseTime<%d>", *stateful, intfIDStart, intfIDEnd, *leasedTime);
} 

UBOOL8 qdmIpv6_isDhcp6sEnabled_igd(void)
{
   InstanceIdStack iidStack;
   IPv6LanHostCfgObject *ipv6Obj = NULL;
   UBOOL8 enabled = FALSE;
   CmsRet ret;

   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_getNextFlags(MDMOID_I_PV6_LAN_HOST_CFG, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&ipv6Obj)) == CMSRET_SUCCESS)
   {
      enabled = ipv6Obj->DHCPv6ServerEnable;
      cmsObj_free((void **)&ipv6Obj);
   }
   else
   {
      cmsLog_error("cmsObj_get returns error. ret=%d", ret);
   }

   cmsLog_debug("enabled<%d>", enabled);
   return enabled;
} 

void qdmIpv6_getRadvdUlaInfo_igd(UBOOL8 *enblUla, UBOOL8 *randomUla, 
                                 char *ulaPrefix, SINT32 *plt, SINT32 *vlt)
{
   InstanceIdStack iidStack;
   ULAPrefixInfoObject *radvdUlaObj = NULL;
   CmsRet ret;

   cmsLog_debug("Enter");
   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_getNext(MDMOID_ULA_PREFIX_INFO, &iidStack, (void **)&radvdUlaObj)) == CMSRET_SUCCESS)
   {
      *enblUla = radvdUlaObj->enable;
      *randomUla = radvdUlaObj->random;

      if ( radvdUlaObj->prefix )
      {
         cmsUtl_strncpy(ulaPrefix, radvdUlaObj->prefix, 
                        CMS_IPADDR_LENGTH);
      }

      /* webVar is in hours, MDM in seconds */
      if ( radvdUlaObj->preferredLifeTime > 0 )
      {
         *plt = radvdUlaObj->preferredLifeTime/3600; 
      }
      else
      {
         *plt = radvdUlaObj->preferredLifeTime;
      }

      if ( radvdUlaObj->validLifeTime > 0 )
      {
         *vlt = radvdUlaObj->validLifeTime/3600; 
      }
      else
      {
         *vlt = radvdUlaObj->validLifeTime;
      }

      cmsObj_free((void **)&radvdUlaObj);
   }
   else
   {
      cmsLog_error("cmsObj_get MDMOID_ULA_PREFIX_INFO returns error. ret=%d", ret);
   }

   cmsLog_debug("enblRadvdUla<%d> enblRandomULA<%d> prefix<%s> plt<%d> vlt<%d>", 
                *enblUla, *randomUla, ulaPrefix, *plt, *vlt);
} 

UBOOL8 qdmIpv6_isRadvdEnabled_igd(void)
{
   InstanceIdStack iidStack;
   RadvdConfigMgtObject *radvdObj = NULL;
   UBOOL8 enabled = FALSE;
   CmsRet ret;

   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_getNextFlags(MDMOID_RADVD_CONFIG_MGT, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&radvdObj)) == CMSRET_SUCCESS)
   {
      enabled = radvdObj->enable;
      cmsObj_free((void **)&radvdObj);
   }
   else
   {
      cmsLog_error("cmsObj_get returns error. ret=%d", ret);
   }

   cmsLog_debug("enabled<%d>", enabled);
   return enabled;
} 


CmsRet qdmIpv6_getSitePrefixInfo_igd(char *sitePrefixType, char *pdWanIfc, char *sitePrefix)
{
   InstanceIdStack iidStack;
   IPv6LanHostCfgObject *ipv6Obj = NULL;
   CmsRet ret = CMSRET_SUCCESS;
   
   if (sitePrefixType == NULL && pdWanIfc == NULL && sitePrefix == NULL)
   {
      cmsLog_error("NULL string.");
      return CMSRET_INVALID_ARGUMENTS;
   }
   if (sitePrefixType != NULL)
   {
      *sitePrefixType = '\0';
   }
   if (pdWanIfc != NULL)
   {
      *pdWanIfc = '\0';
   }
   if (sitePrefix != NULL)
   {
      *sitePrefix = '\0';
   }
         
   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_getNext(MDMOID_I_PV6_LAN_HOST_CFG, &iidStack, (void **)&ipv6Obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_getNext <MDMOID_I_PV6_LAN_HOST_CFG> returns error. ret=%d", ret);
      return ret;
   }   

   cmsObj_free((void **)&ipv6Obj);
   return ret;        
    
}  /* End of qdmIpv6_getSitePrefixInfo_igd() */

#endif  /* DMP_X_BROADCOM_COM_IPV6_1 */
#endif /* DMP_BASELINE_1  */

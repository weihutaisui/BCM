/***********************************************************************
 *
 *  Copyright (c) 2006-2011  Broadcom Corporation
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


#ifdef DMP_BASELINE_1

#include "cms_util.h"
#include "cms_qdm.h"


/*!\file qdm_dns.c
 * \brief This file contains TR98 DNS query functions.
 *
 */


void qdmDns_getActiveIpvxDnsIpLocked_igd(UINT32 ipvx, char *dns1, char *dns2)
{
   NetworkConfigObject *networkCfg=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   cmsLog_debug("Enter: ipvx=%d", ipvx);

   if (!dns1)
   {
      cmsLog_error("NULL dns1 buffer passed in!");
      return;
   }
   *dns1 = '\0';

   if (dns2)
   {
      *dns2 = '\0';
   }

#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
   /* In Hybrid IPv6 mode, we need to check for IPv6 on the _dev2 side first */
   if (ipvx & CMS_AF_SELECT_IPV6)
   {
      qdmDns_getActiveIpvxDnsIpLocked_dev2(CMS_AF_SELECT_IPV6, dns1, dns2);
      if (!cmsUtl_isZeroIpvxAddress(CMS_AF_SELECT_IPVX, dns1))
      {
         /* got IPv6 address, return now */
         cmsLog_debug("Active IPv6 from _dev2, dns1=%s, dns2=%s", dns1, dns2);
         return;
      }
   }
#endif

   if (cmsObj_get(MDMOID_NETWORK_CONFIG, &iidStack, OGF_NO_VALUE_UPDATE, (void *) &networkCfg) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get NETWORK_CONFIG!");
      return;
   }

#ifdef DMP_X_BROADCOM_COM_IPV6_1
   if ((ipvx & CMS_AF_SELECT_IPV6) &&
       (!IS_EMPTY_STRING(networkCfg->activeDNSServers)))
   {
      char tmp1[CMS_IPADDR_LENGTH]={0};
      char tmp2[CMS_IPADDR_LENGTH]={0};
      cmsLog_debug("calling parseDNS for Legacy IPv6...");
      cmsUtl_parseDNS(networkCfg->activeDNSServers, tmp1, tmp2, FALSE);
      if (cmsUtl_isValidIpAddress(AF_INET6, tmp1))
      {
         strcpy(dns1, tmp1);
         if (dns2)
         {
            strcpy(dns2, tmp2);
         }

         cmsLog_debug("(legacy v6) Active dns1=%s, dns2=%s", dns1, dns2);
         return;
      }
   }
#endif

   if (cmsUtl_isZeroIpvxAddress(CMS_AF_SELECT_IPVX, dns1) &&
       (ipvx & CMS_AF_SELECT_IPV4) &&
       (!IS_EMPTY_STRING(networkCfg->activeDNSServers)))
   {
      cmsLog_debug("activeDNSServers=%s", networkCfg->activeDNSServers);
      cmsLog_debug("calling parseDNS for IPv4...");
      cmsUtl_parseDNS(networkCfg->activeDNSServers, dns1, dns2, TRUE);
      cmsLog_debug("Active dns1=%s, dns2=%s", dns1, dns2);
   }

   cmsObj_free((void **) &networkCfg);

   cmsLog_debug("Exit: dns1 %s, dns2 %s", dns1, dns2);
}


UBOOL8 qdmDns_getStaticIpvxDnsServersLocked_igd(UINT32 ipvx, char *staticDnsServers)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   NetworkConfigObject *networkCfg=NULL;
   SINT32 assumedBufLen=(CMS_MAX_ACTIVE_DNS_IP * CMS_IPADDR_LENGTH);
   CmsRet ret;
   UBOOL8 found=FALSE;

   cmsLog_debug("Entered: ipvx=%d", ipvx);


   if (staticDnsServers == NULL)
   {
      cmsLog_error("staticDnsServers is NULL!");
      return FALSE;
   }
   staticDnsServers[0] = '\0';

#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
   /*
    * In Hybrid IPv6 mode, we need to check for IPv6 on the _dev2 side first.
    */
   if (ipvx & CMS_AF_SELECT_IPV6)
   {
      found = qdmDns_getStaticIpvxDnsServersLocked_dev2(CMS_AF_SELECT_IPV6,
                                                        staticDnsServers);
      if (found)
      {
         return found;
      }
   }
#endif

#ifdef DMP_X_BROADCOM_COM_IPV6_1
   if (ipvx & CMS_AF_SELECT_IPV6)
   {
      /* see fetchActiveDnsServers6 in rut_dns6.c */
      /* technically, we should move this function out to
       * qdmDns_getStaticIpv6DnsServersLocked_igd in qdm_dns6.c
       */
      InstanceIdStack iidStack2 = EMPTY_INSTANCE_ID_STACK;
      IPv6LanHostCfgObject *ipv6Obj = NULL;

      if ((ret = cmsObj_getNext(MDMOID_I_PV6_LAN_HOST_CFG, &iidStack2, (void **)&ipv6Obj)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsObj_getNext <MDMOID_I_PV6_LAN_HOST_CFG> returns error. ret=%d", ret);
         /* keep going and try IPv4 */
      }
      else
      {
         if (!cmsUtl_strcmp(ipv6Obj->IPv6DNSConfigType, MDMVS_STATIC))
         {
            /* static ipv6 dns ip is in this object */
            if (!IS_EMPTY_STRING(ipv6Obj->IPv6DNSServers))
            {
               if (cmsUtl_strlen(ipv6Obj->IPv6DNSServers) >= assumedBufLen)
               {
                  cmsLog_error("DNSServer string %d too long for assumedBufLen %d",
                               ipv6Obj->IPv6DNSServers, assumedBufLen);
               }
               else
               {
                  strcat(staticDnsServers, ipv6Obj->IPv6DNSServers);
                  found = TRUE;
               }
            }
         }
         cmsObj_free((void **)&ipv6Obj);

         if (found)
         {
            return found;
         }
      }
   }
#endif

   if (ipvx & CMS_AF_SELECT_IPV4)
   {
      if ((ret = cmsObj_get(MDMOID_NETWORK_CONFIG, &iidStack, OGF_NO_VALUE_UPDATE,
                  (void *) &networkCfg)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not get NETWORK_CONFIG. ret=%d", ret);
         return FALSE;
      }

      /* networkCfg->DNSServers holds IPv4 static DNS addrs only. */
      if (!IS_EMPTY_STRING(networkCfg->DNSServers) &&
          strncmp(networkCfg->DNSServers, "0.0.0.0", 7))
      {
         if (cmsUtl_strlen(networkCfg->DNSServers) >= assumedBufLen)
         {
            cmsLog_error("DNSServer string %d too long for assumedBufLen %d",
                         networkCfg->DNSServers, assumedBufLen);
         }
         else
         {
            strcpy(staticDnsServers, networkCfg->DNSServers);
            found = TRUE;
         }
      }

      cmsObj_free((void **)&networkCfg);
   }

   cmsLog_debug("Exit: found=%d staticDnsServers=%s", found, staticDnsServers);

   return found;
}

#endif    /* DMP_BASELINE_1 */




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


#ifdef DMP_DEVICE2_BASELINE_1

#include "cms_util.h"
#include "cms_qdm.h"



/*!\file qdm2_dns.c
 * \brief This file contains Device 2 DNS query functions.
 *
 */


void qdmDns_getActiveIpvxDnsIpLocked_dev2(UINT32 ipvx, char *dns1, char *dns2)
{
   Dev2DnsClientObject *dnsClientObj=NULL;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

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

   ret = cmsObj_get(MDMOID_DEV2_DNS_CLIENT, &iidStack, OGF_NO_VALUE_UPDATE,
                    (void**)&dnsClientObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get MDMOID_DEV2_DNS_CLIENT, ret=%d", ret);
      return;
   }

   cmsLog_debug("dnsClientObj->X_BROADCOM_COM_ActiveDnsServers %s, dnsClientObj->X_BROADCOM_COM_Ipv6_ActiveDnsServers %s", 
                dnsClientObj->X_BROADCOM_COM_ActiveDnsServers,
                dnsClientObj->X_BROADCOM_COM_Ipv6_ActiveDnsServers);

#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
   if (ipvx & CMS_AF_SELECT_IPV6)
   {
      if (!IS_EMPTY_STRING(dnsClientObj->X_BROADCOM_COM_Ipv6_ActiveDnsServers))
      {
         cmsLog_debug("calling parseDNS for IPv6...");
         cmsUtl_parseDNS(dnsClientObj->X_BROADCOM_COM_Ipv6_ActiveDnsServers,
                         dns1, dns2, FALSE);
         cmsLog_debug("Active IPv6 dns1=%s, dns2=%s", dns1, dns2);
      }
   }
#endif


   if (cmsUtl_isZeroIpvxAddress(CMS_AF_SELECT_IPVX, dns1))
   {
      /* we did not get IPv6 address, try IPv4 if requested */
      if (ipvx & CMS_AF_SELECT_IPV4)
      {
         if (!IS_EMPTY_STRING(dnsClientObj->X_BROADCOM_COM_ActiveDnsServers))
         {
            cmsLog_debug("calling parseDNS for IPv4...");
            cmsUtl_parseDNS(dnsClientObj->X_BROADCOM_COM_ActiveDnsServers,
                            dns1, dns2, TRUE);
            cmsLog_debug("Active IPv4 dns1=%s, dns2=%s", dns1, dns2);
         }
      }
   }

   /*
    * Later: we could be even more clever and detect that we only got a
    * single IPv6 DNS address in dns1, so try to find an IPv4 DNS address
    * to put into dns2.
    */

   cmsObj_free((void **) &dnsClientObj);

   cmsLog_debug("Exit: dns1 %s, dns2 %s", dns1, dns2);
}


UBOOL8 qdmDns_getStaticIpvxDnsServersLocked_dev2(UINT32 ipvx, char *staticDnsServers)
{
   Dev2DnsServerObject *dnsServerObj=NULL;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   SINT32 assumedBufLen=(CMS_MAX_ACTIVE_DNS_IP * CMS_IPADDR_LENGTH);
   char dns1[CMS_IPADDR_LENGTH];
   char dns2[CMS_IPADDR_LENGTH];
   UBOOL8 isIpv4Addr, isIpv6Addr;
   UBOOL8 found=FALSE;

   cmsLog_debug("Entered: ipvx=%d", ipvx);


   if (staticDnsServers == NULL)
   {
      cmsLog_error("staticDnsServers is NULL!");
      return FALSE;
   }
   staticDnsServers[0] = '\0';

   /*
    * This algorithm does not give preference to IPv6 addrs, but it should.
    * Also, because we can only return a max of CMS_MAX_ACTIVE_DNS_IP (2)
    * IP addresses in the list and the algorithm is not smart enough to
    * know how many addresses are in the list so far, stop after the first
    * list (which may only contain 1 IP address).
    */
   while(!found &&
         cmsObj_getNextFlags(MDMOID_DEV2_DNS_SERVER,
                             &iidStack, OGF_NO_VALUE_UPDATE,
                             (void **) &dnsServerObj) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(dnsServerObj->type, MDMVS_STATIC))
      {


         /* do a parse to find out what type of addr it is */
         memset(dns1, 0, sizeof(dns1));
         if ((cmsUtl_parseDNS(dnsServerObj->DNSServer, dns1, dns2, TRUE) == CMSRET_SUCCESS) &&
             !cmsUtl_isZeroIpvxAddress(CMS_AF_SELECT_IPV4, dns1) &&
             cmsUtl_isValidIpv4Address(dns1))
         {
            isIpv4Addr = TRUE;
            isIpv6Addr = FALSE;
         }
         else
         {
            isIpv4Addr = FALSE;
            isIpv6Addr = TRUE;
         }

         /* verify the IP addr we got is what caller asked for */
         if ((isIpv4Addr && (ipvx & CMS_AF_SELECT_IPV4)) ||
             (isIpv6Addr && (ipvx & CMS_AF_SELECT_IPV6)))
         {
            if (cmsUtl_strlen(dnsServerObj->DNSServer) >= assumedBufLen)
            {
               cmsLog_error("DNSServer string %d too long for assumedBufLen %d",
                            dnsServerObj->DNSServer, assumedBufLen);
            }
            else
            {
               /* copy out */
               strcpy(staticDnsServers, dnsServerObj->DNSServer);
               found = TRUE;
            }

         }
      }

      cmsObj_free((void **) &dnsServerObj);
   }

   cmsLog_debug("Exit: found=%d staticDnsServers=%s", found, staticDnsServers);

   return found;
}


#endif    /* DMP_DEVICE2_BASELINE_1 */




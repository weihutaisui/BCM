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
:>
*/

#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"
#include "cms_qdm.h"
#include "dal.h"
#include "dal2_wan.h"

#ifdef DMP_DEVICE2_BASELINE_1    /* this file touches TR181 objects */



void dalDns_deleteAllStaticIpvxDnsServers_dev2(UINT32 ipvx)
{
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack savedIidStack=EMPTY_INSTANCE_ID_STACK;
   Dev2DnsServerObject *dnsServerObj=NULL;
   Dev2DnsClientObject *dnsClientObj=NULL;
   CmsRet ret;

   cmsLog_debug("Enter: ipvx=%d", ipvx);


   while(cmsObj_getNextFlags(MDMOID_DEV2_DNS_SERVER,
                             &iidStack,
                             OGF_NO_VALUE_UPDATE,
                             (void **) &dnsServerObj) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(dnsServerObj->type, MDMVS_STATIC))
      {
         char dns1[CMS_IFNAME_LENGTH];
         char dns2[CMS_IFNAME_LENGTH];
         UBOOL8 isIpv4Addr, isIpv6Addr;

         /* do a parse on the DNSServers string to see whether this is
          * IPv4 or IPv6 static address.
          */
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


         if ((isIpv4Addr && (ipvx & CMS_AF_SELECT_IPV4)) ||
             (isIpv6Addr && (ipvx & CMS_AF_SELECT_IPV6)))
         {
            cmsLog_debug("Deleting %s static DNS %s",
                         (isIpv4Addr ? "IPv4" : "IPv6"),
                         dnsServerObj->DNSServer);

            cmsObj_deleteInstance(MDMOID_DEV2_DNS_SERVER, &iidStack);
            /*
             * Because we deleted the current instance, we have to set the
             * iidStack to the last not deleted instance so that when
             * we loop back up to cmsObj_getNext, it will know where to
             * start from.
             */
            iidStack = savedIidStack;
         }
      }

      cmsObj_free((void **)&dnsServerObj);
      /* save the current iidStack in case we delete the next one */
      savedIidStack = iidStack;
   }

   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_get(MDMOID_DEV2_DNS_CLIENT, &iidStack,
                         OGF_NO_VALUE_UPDATE,
                         (void **) &dnsClientObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get dnsClientObj, ret=%d", ret);
      return;
   }

   if (ipvx & CMS_AF_SELECT_IPV4)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(dnsClientObj->X_BROADCOM_COM_ActiveDnsServers);
   }

#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
   if (ipvx & CMS_AF_SELECT_IPV6)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(dnsClientObj->X_BROADCOM_COM_Ipv6_ActiveDnsServers);
   }
#endif

   if ((ret = cmsObj_set(dnsClientObj, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set dnsClientObj, ret=%d", ret);
   }

   cmsObj_free((void **) &dnsClientObj);

   cmsLog_debug("Exit");

   return;
}


CmsRet dalDns_setStaticIpvxDnsServers_dev2(UINT32 ipvx,
                                           const char *dnsPrimary,
                                           const char *dnsSecondary)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2DnsServerObject *dnsServerObj=NULL;
   Dev2DnsClientObject *dnsClientObj=NULL;
   char staticDnsServersBuf[CMS_IFNAME_LENGTH * 2]={0};
   CmsRet ret;
   
   cmsLog_debug("Enter: ipvx=%d dnsPrim=%s dnsSec=%s",
                 ipvx, dnsPrimary, dnsSecondary);

   if (IS_EMPTY_STRING(dnsPrimary))
   {
      /* Do not pass in empty or zero dnsPrimary for delete, call dalDns_deleteAllStaticIpvxDnsServers */
      cmsLog_error("NULL or empty dnsPrimary");
      return CMSRET_INVALID_PARAM_VALUE;
   }

   if ((ipvx & CMS_AF_SELECT_IPVX) == CMS_AF_SELECT_IPVX)
   {
      cmsLog_error("must specify either IPv4 or IPv6 (not both)");
      return CMSRET_INVALID_ARGUMENTS;
   }

   /*
    * In the TR98 data model, we have only one parameter to hold
    * the static IPv4 DNS servers (networkCfg->DNSServers), so whenever we add
    * static IPv4 DNS servers, we will delete the previous static IPv4 DNS
    * servers due to overwrite of the single param.  Same thing for static
    * IPv6 DNS servers, which are in another param.  Mirror that behavior here.
    * I suppose if there is an existing static entry, we could just
    * reuse that one instead of deleting it and creating a new one.
    */
   dalDns_deleteAllStaticIpvxDnsServers(ipvx);


   if ((ret = cmsObj_addInstance(MDMOID_DEV2_DNS_SERVER, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to add MDMOID_DEV2_DNS_SERVER Instance, ret = %d", ret);
      return ret;
   } 

   if ((ret = cmsObj_get(MDMOID_DEV2_DNS_SERVER, &iidStack,
                         OGF_NO_VALUE_UPDATE,
                         (void **) &dnsServerObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get dnsServerObj, ret = %d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_DNS_SERVER, &iidStack);
      return ret;
   }


   cmsUtl_concatDNS(dnsPrimary, dnsSecondary, staticDnsServersBuf, sizeof(staticDnsServersBuf));

   /*
    * Note here we do not care if address is IPv4 or IPv6, just set the
    * strings into the object.
    */
   CMSMEM_REPLACE_STRING(dnsServerObj->DNSServer, staticDnsServersBuf);

   dnsServerObj->enable = TRUE;

   /*
    * Note that a static DNS server object's interface param is blank,
    * which means the static DNS server is "global".  We do not support
    * per-interface static DNS server even though the data model does.
    */

   /* The default value for Type is "static" so no need to modify it here */

   if ((ret = cmsObj_set(dnsServerObj, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set dnsServerObj. ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_DNS_SERVER, &iidStack);
   } 
   cmsObj_free((void **) &dnsServerObj); 

   INIT_INSTANCE_ID_STACK(&iidStack);

   if ((ret = cmsObj_get(MDMOID_DEV2_DNS_CLIENT, &iidStack,
                         OGF_NO_VALUE_UPDATE,
                         (void **) &dnsClientObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get dnsClientObj, ret=%d", ret);
      return ret;
   }

   if (ipvx & CMS_AF_SELECT_IPV4)
   {
      CMSMEM_REPLACE_STRING(dnsClientObj->X_BROADCOM_COM_ActiveDnsServers, staticDnsServersBuf);
      CMSMEM_FREE_BUF_AND_NULL_PTR(dnsClientObj->X_BROADCOM_COM_DnsIfNames);
   }

#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
   if (ipvx & CMS_AF_SELECT_IPV6)
   {
      CMSMEM_REPLACE_STRING(dnsClientObj->X_BROADCOM_COM_Ipv6_ActiveDnsServers, staticDnsServersBuf);
      CMSMEM_FREE_BUF_AND_NULL_PTR(dnsClientObj->X_BROADCOM_COM_Ipv6_DnsIfNames);
   }
#endif

   if ((ret = cmsObj_set(dnsClientObj, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set dnsClientObj, ret=%d", ret);
   }
   cmsObj_free((void **) &dnsClientObj); 

   cmsLog_debug("Exit, ret=%d", ret);

   return ret;
}


CmsRet dalDns_setIpvxDnsIfNameList_dev2(UINT32 ipvx, const char *dnsIfNameList)
{
   char *newDnsList=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2DnsClientObject *dnsClientObj=NULL;
   CmsRet ret=CMSRET_SUCCESS;

   cmsLog_debug("Enter: ipvx=%d dnsIfNames=%s", ipvx, dnsIfNameList);

   if ((ipvx & CMS_AF_SELECT_IPVX) == CMS_AF_SELECT_IPVX)
   {
      cmsLog_error("must specify either IPv4 or IPv6 (not both)");
      return CMSRET_INVALID_ARGUMENTS;
   }

   ret = dalWan_validateIfNameList(dnsIfNameList, CMS_MAX_DNSIFNAME,
                                   &newDnsList);
   if (ret != CMSRET_SUCCESS)
   {
      return ret;
   }

   if ((ret = cmsObj_get(MDMOID_DEV2_DNS_CLIENT, &iidStack,
                         OGF_NO_VALUE_UPDATE,
                         (void **) &dnsClientObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get MDMOID_DEV2_DNS_CLIENT, ret=%d", ret);
      CMSMEM_FREE_BUF_AND_NULL_PTR(newDnsList);
      return ret;
   }

   if (ipvx & CMS_AF_SELECT_IPV4)
   {
      CMSMEM_REPLACE_STRING(dnsClientObj->X_BROADCOM_COM_DnsIfNames, newDnsList);
   }

#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
   if (ipvx & CMS_AF_SELECT_IPV6)
   {
      CMSMEM_REPLACE_STRING(dnsClientObj->X_BROADCOM_COM_Ipv6_DnsIfNames, newDnsList);
   }
#endif

   CMSMEM_FREE_BUF_AND_NULL_PTR(newDnsList);

   if ((ret = cmsObj_set(dnsClientObj, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not set MDMOID_DEV2_DNS_CLIENT, ret=%d", ret);
   }

   cmsObj_free((void **) &dnsClientObj);

   return ret;
}


CmsRet dalDns_getIpvxDnsIfNameList_dev2(UINT32 ipvx, char *dnsIfNameList)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2DnsClientObject *dnsClientObj=NULL;
   CmsRet ret;


   if (!dnsIfNameList)
   {
      cmsLog_error("dnsIfNamelist is NULL!");
      return CMSRET_INVALID_ARGUMENTS;
   }
   dnsIfNameList[0] = '\0';

   if ((ipvx & CMS_AF_SELECT_IPVX) == CMS_AF_SELECT_IPVX)
   {
      cmsLog_error("must specify either IPv4 or IPv6 (not both)");
      return CMSRET_INVALID_ARGUMENTS;
   }

   /* Get dnsIfNameList from DNS.Client object */
   if ((ret = cmsObj_get(MDMOID_DEV2_DNS_CLIENT, &iidStack, 0,
                         (void **) &dnsClientObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get MDMOID_DEV2_DNS_CLIENT, ret=%d", ret);
      return ret;
   }

   if (ipvx & CMS_AF_SELECT_IPV4)
   {
      if (!IS_EMPTY_STRING(dnsClientObj->X_BROADCOM_COM_DnsIfNames))
      {
         strcpy(dnsIfNameList, dnsClientObj->X_BROADCOM_COM_DnsIfNames);
      }
   }

#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
   if (ipvx & CMS_AF_SELECT_IPV6)
   {
      if (!IS_EMPTY_STRING(dnsClientObj->X_BROADCOM_COM_Ipv6_DnsIfNames))
      {
         strcpy(dnsIfNameList, dnsClientObj->X_BROADCOM_COM_Ipv6_DnsIfNames);
      }
   }
#endif

   cmsObj_free((void **) &dnsClientObj);

   cmsLog_debug("Exit: ret=%d ipvx=%d dnsIfNamelist=%s",
                 ret, ipvx, dnsIfNameList);

   return ret;
}


#endif /* DMP_DEVICE2_BASELINE_1 */


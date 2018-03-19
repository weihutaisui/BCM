/***********************************************************************
 *
 *  Copyright (c) 2007-2010  Broadcom Corporation
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

#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"


#if defined(DMP_BASELINE_1)


void dalDns_deleteAllStaticIpvxDnsServers_igd(UINT32 ipvx)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   NetworkConfigObject *networkCfg=NULL;
   CmsRet ret;

   cmsLog_debug("Enter: ipvx=%d", ipvx);


#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
   /* In case of Hybrid IPv6, use _dev2 func */
   if (ipvx & CMS_AF_SELECT_IPV6)
   {
      dalDns_deleteAllStaticIpvxDnsServers_dev2(CMS_AF_SELECT_IPV6);
   }
#endif

#ifdef DMP_X_BROADCOM_COM_IPV6_1
   if (ipvx & CMS_AF_SELECT_IPV6)
   {
      /* see code in cgiConfigDns */
      cmsLog_error("proprietary Broadcom IPv6 case not handled");
   }
#endif


   /* by the time we get here, only IPv4 is valid */
   if (ipvx & CMS_AF_SELECT_IPV4)
   {
      if ((ret = cmsObj_get(MDMOID_NETWORK_CONFIG, &iidStack,
                            OGF_NO_VALUE_UPDATE,
                            (void **) &networkCfg)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not get NETWORK_CONFIG, ret=%d", ret);
         return;
      }

      /* DNSServers contains the static IPv4 DNS servers only */
      CMSMEM_FREE_BUF_AND_NULL_PTR(networkCfg->DNSServers);

      if ((ret = cmsObj_set(networkCfg, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not set NETWORK_CONFIG, ret=%d", ret);
      }

      cmsObj_free((void **) &networkCfg);
   }

   return;
}


CmsRet dalDns_setStaticIpvxDnsServers_igd(UINT32 ipvx,
                                          const char *dnsPrimary,
                                          const char *dnsSecondary)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   NetworkConfigObject *networkCfg=NULL;
   char staticDnsServersBuf[CMS_IFNAME_LENGTH * 2]={0};
   CmsRet ret=CMSRET_SUCCESS;

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

#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
   /* In case of Hybrid IPv6, set list using _dev2 func */
   if (ipvx & CMS_AF_SELECT_IPV6)
   {
      return (dalDns_setStaticIpvxDnsServers_dev2(CMS_AF_SELECT_IPV6,
                                                  dnsPrimary, dnsSecondary));
   }
#endif

#ifdef DMP_X_BROADCOM_COM_IPV6_1
   if (ipvx == CMS_AF_SELECT_IPV6)
   {
      /* see code in cgiConfigDns */
      cmsLog_error("proprietary Broadcom IPv6 case not handled");
      return CMSRET_INTERNAL_ERROR;
   }
#endif

   /* by the time we get here, only IPv4 is valid */
   if (ipvx & CMS_AF_SELECT_IPV4)
   {
      if ((ret = cmsObj_get(MDMOID_NETWORK_CONFIG, &iidStack,
                            OGF_NO_VALUE_UPDATE,
                            (void **) &networkCfg)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not get NETWORK_CONFIG, ret=%d", ret);
         return ret;
      }

      cmsUtl_concatDNS(dnsPrimary, dnsSecondary, staticDnsServersBuf, sizeof(staticDnsServersBuf));

      /*
       * Overwrite the single DNSServers param.  This holds the IPv4 static
       * DNS servers only.  IPv6 static DNS servers (for both proprietary and
       * Hybrid) are stored somewhere else in the data model.
       */
      CMSMEM_REPLACE_STRING(networkCfg->DNSServers, staticDnsServersBuf);
      CMSMEM_FREE_BUF_AND_NULL_PTR(networkCfg->DNSIfName);

      if ((ret = cmsObj_set(networkCfg, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not set NETWORK_CONFIG, ret=%d", ret);
      }

      cmsObj_free((void **) &networkCfg);
   }

   return ret;
}


CmsRet dalDns_setIpvxDnsIfNameList_igd(UINT32 ipvx, const char *dnsIfNameList)
{
   char *newDnsList=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   NetworkConfigObject *networkCfg=NULL;
   CmsRet ret=CMSRET_SUCCESS;

   cmsLog_debug("Enter: ipvx=%d dnsIfNameList=%s", ipvx, dnsIfNameList);

   if ((ipvx & CMS_AF_SELECT_IPVX) == CMS_AF_SELECT_IPVX)
   {
      cmsLog_error("must specify either IPv4 or IPv6 (not both)");
      return CMSRET_INVALID_ARGUMENTS;
   }

#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
   /* In case of Hybrid IPv6, set list using _dev2 func */
   if (ipvx & CMS_AF_SELECT_IPV6)
   {
      return (dalDns_setIpvxDnsIfNameList_dev2(CMS_AF_SELECT_IPV6, dnsIfNameList));
   }
#endif

#ifdef DMP_X_BROADCOM_COM_IPV6_1
   if (ipvx == CMS_AF_SELECT_IPV6)
   {
      /* see code in cgiConfigDns */
      cmsLog_error("proprietary Broadcom IPv6 case not handled");
      return CMSRET_INTERNAL_ERROR;
   }
#endif

   /* by the time we get here, only IPv4 is valid */
   if (ipvx & CMS_AF_SELECT_IPV4)
   {
      ret = dalWan_validateIfNameList(dnsIfNameList, CMS_MAX_DNSIFNAME,
                                      &newDnsList);
      if (ret != CMSRET_SUCCESS)
      {
         return ret;
      }

      if ((ret = cmsObj_get(MDMOID_NETWORK_CONFIG, &iidStack,
                            OGF_NO_VALUE_UPDATE,
                            (void **) &networkCfg)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not get NETWORK_CONFIG, ret=%d", ret);
         CMSMEM_FREE_BUF_AND_NULL_PTR(newDnsList);
         return ret;
      }

      CMSMEM_REPLACE_STRING(networkCfg->DNSIfName, newDnsList);

      CMSMEM_FREE_BUF_AND_NULL_PTR(newDnsList);

      if ((ret = cmsObj_set(networkCfg, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not set NETWORK_CONFIG, ret=%d", ret);
      }

      cmsObj_free((void **) &networkCfg);
   }

   return ret;
}


CmsRet dalDns_getIpvxDnsIfNameList_igd(UINT32 ipvx, char *dnsIfNameList)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   NetworkConfigObject *networkCfg=NULL;
   CmsRet ret = 0;


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

#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
   /* In case of Hybrid IPv6, use _dev2 function */
   if (ipvx & CMS_AF_SELECT_IPV6)
   {
      return (dalDns_getIpvxDnsIfNameList_dev2(ipvx, dnsIfNameList));
   }
#endif

#ifdef DMP_X_BROADCOM_COM_IPV6_1
   if (ipvx & CMS_AF_SELECT_IPV6)
   {
      /* see code in cgiConfigDns and rut_dns6.c */
      cmsLog_error("proprietary Broadcom IPv6 case not handled");
      return CMSRET_INTERNAL_ERROR;
   }
#endif

   if (ipvx & CMS_AF_SELECT_IPV4)
   {
      if ((ret = cmsObj_get(MDMOID_NETWORK_CONFIG, &iidStack, 0, (void **) &networkCfg)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not get NETWORK_CONFIG, ret=%d", ret);
         return ret;
      }

      cmsLog_debug("Current networkCfg->DNSIfName=%s", networkCfg->DNSIfName);

      if (networkCfg->DNSIfName != NULL && dnsIfNameList)
      {
         strcpy(dnsIfNameList, networkCfg->DNSIfName);
      }

      cmsObj_free((void **) &networkCfg);
   }

   cmsLog_debug("Exit: ret=%d ipvx=%d dnsIfNamelist=%s",
                  ret, ipvx, dnsIfNameList);

   return ret;
}

#endif /* DMP_BASELINE_1 */



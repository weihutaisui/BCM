/***********************************************************************
 *
 *  Copyright (c) 2012  Broadcom Corporation
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
#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1 /* aka SUPPORT_IPV6 */

#include "cms.h"
#include "cms_mdm.h"
#include "cms_obj.h"
#include "cms_qdm.h"
#include "cms_util.h"
#include "cms_core.h"


CmsRet qdmIpv6_fullPathToPefixLocked_dev2(const char *mdmPath, char *prefix)
{
   MdmPathDescriptor pathDesc;
   MdmPathDescriptor pathDesc2;
   PhlGetParamValue_t   *pParamValue = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   if (IS_EMPTY_STRING(mdmPath) || prefix == NULL)
   {
      cmsLog_error("invalid argument. mdmPath=0x%x prefix=0x%x", (uintptr_t)mdmPath, (uintptr_t)prefix);
      return CMSRET_INVALID_ARGUMENTS;
   }

   INIT_PATH_DESCRIPTOR(&pathDesc);
   ret = cmsMdm_fullPathToPathDescriptor(mdmPath, &pathDesc);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_fullPathToPathDescriptor failed for %s, ret=%d", mdmPath, ret);
      return ret;
   }

   memset(prefix, 0, CMS_IPADDR_LENGTH);

   INIT_PATH_DESCRIPTOR(&pathDesc2);
   pathDesc2.oid = pathDesc.oid;
   pathDesc2.iidStack = pathDesc.iidStack;
   sprintf(pathDesc.paramName, "Prefix");

   ret = cmsPhl_getParamValue(&pathDesc, &pParamValue);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsPhl_getParamValue error: %d", ret);
   }
   else
   {
      if (cmsUtl_strlen(pParamValue->pValue) > 0)
      {
         snprintf(prefix, CMS_IPADDR_LENGTH, "%s", pParamValue->pValue);
      }
      cmsPhl_freeGetParamValueBuf(pParamValue, 1);
   }

   cmsLog_debug("prefix<%s>", prefix);
   return ret;
}

CmsRet qdmIpv6_getLanULAAddr6_dev2(const char *ifname __attribute((unused)), char *addr)
{
   CmsRet ret;

   cmsLog_debug("ifname<%s>", ifname);

   if (addr == NULL)
   {
      cmsLog_error("NULL string.");
      return CMSRET_INVALID_ARGUMENTS;
   }
   *addr = '\0';

   ret = qdmIpIntf_getIpv6AddressByNameLocked_dev2("br0", addr);

   cmsLog_debug("addr<%s>", addr);

   return ret;
}  /* End of qdmIpv6_getLanULAAddr6_dev2 */

CmsRet qdmIpv6_getDns6Info_dev2(char *dnsType, char *dnsIfc, char *dnsPri, char *dnsSec)
{
   InstanceIdStack iidStack;
   Dev2DnsClientObject *dnsClientObj = NULL;
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
   if ((ret = cmsObj_get(MDMOID_DEV2_DNS_CLIENT, &iidStack, OGF_NO_VALUE_UPDATE, (void **) &dnsClientObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_get <MDMOID_DEV2_DNS_CLIENT> returns error. ret=%d", ret);
      return ret;
   }

   if (dnsType != NULL)
   {
      if (!cmsUtl_strstr(dnsClientObj->X_BROADCOM_COM_Ipv6_ActiveDnsIfName, MDMVS_STATIC))
      {
         cmsUtl_strcpy(dnsType, MDMVS_DHCP);
      }
      else
      {
         cmsUtl_strcpy(dnsType, MDMVS_STATIC);
      }
   }

   if (dnsIfc != NULL)
   {
      if (!IS_EMPTY_STRING(dnsClientObj->X_BROADCOM_COM_Ipv6_DnsIfNames))
      {
         cmsUtl_strcpy(dnsIfc, dnsClientObj->X_BROADCOM_COM_Ipv6_DnsIfNames);
      }
   }

   if (dnsPri != NULL || dnsSec != NULL)
   {
      char dnsServers[BUFLEN_128];

      *dnsServers = '\0';

      if (!IS_EMPTY_STRING(dnsClientObj->X_BROADCOM_COM_Ipv6_ActiveDnsServers))
      {
         strcpy(dnsServers, dnsClientObj->X_BROADCOM_COM_Ipv6_ActiveDnsServers);
      }

      if ((ret = cmsUtl_parseDNS(dnsServers, dnsPri, dnsSec, FALSE)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsUtl_parseDNS returns error. ret=%d", ret);
      }
   }

   cmsObj_free((void **) &dnsClientObj);
   return ret;        
    
}  /* End of qdmIpv6_getDns6Info_dev2() */

void qdmIpv6_getDhcp6sInfo_dev2(UBOOL8 *stateful, char *intfIDStart, 
                                char *intfIDEnd, SINT32 *leasedTime)
{
   InstanceIdStack iidStack;
   Dev2Dhcpv6ServerPoolObject *serverPoolObj=NULL;
   UBOOL8 found = FALSE;

   cmsLog_debug("Enter");
   INIT_INSTANCE_ID_STACK(&iidStack);

   /* FIXME: In the future, we may query dhcp6s info on brX */
   while (!found && cmsObj_getNext(MDMOID_DEV2_DHCPV6_SERVER_POOL,
                        &iidStack, (void **) &serverPoolObj) == CMSRET_SUCCESS)
   {
      found = TRUE;
      *stateful = serverPoolObj->IANAEnable;
      *leasedTime = serverPoolObj->X_BROADCOM_COM_IANALeaseTime/3600;
      if ( serverPoolObj->X_BROADCOM_COM_MinInterfaceID )
      {
         cmsUtl_strncpy(intfIDStart, serverPoolObj->X_BROADCOM_COM_MinInterfaceID,
                        CMS_IPADDR_LENGTH);
      }

      if ( serverPoolObj->X_BROADCOM_COM_MaxInterfaceID )
      {
         cmsUtl_strncpy(intfIDEnd, serverPoolObj->X_BROADCOM_COM_MaxInterfaceID,
                        CMS_IPADDR_LENGTH);
      }

      cmsObj_free((void **) &serverPoolObj);
   }

   if (!found)
   {
      cmsLog_error("cannot find Dev2Dhcpv6ServerPoolObject");
   }

   cmsLog_debug("enblDhcp6sStateful<%d> minIID<%s> maxIID<%s> leaseTime<%d>", *stateful, intfIDStart, intfIDEnd, *leasedTime);
}

UBOOL8 qdmIpv6_isDhcp6sEnabled_dev2(void)
{
   InstanceIdStack iidStack;
   UBOOL8 enabled = FALSE;
   Dev2Dhcpv6ServerPoolObject *serverPoolObj=NULL;
   UBOOL8 found = FALSE;

   cmsLog_debug("Enter");
   INIT_INSTANCE_ID_STACK(&iidStack);

   /* FIXME: We assume Dev2Dhcpv6ServerObject->enable is always true */
   /* FIXME: In the future, we may query whether dhcp6s on brX is enabled */
   while (!found && cmsObj_getNext(MDMOID_DEV2_DHCPV6_SERVER_POOL,
                        &iidStack, (void **) &serverPoolObj) == CMSRET_SUCCESS)
   {
      found = TRUE;
      enabled = serverPoolObj->enable;
      cmsObj_free((void **) &serverPoolObj);
   }

   if (!found)
   {
      cmsLog_error("cannot find Dev2Dhcpv6ServerPoolObject");
   }

   cmsLog_debug("enabled<%d>", enabled);
   return enabled;
}

UBOOL8 qdmIpv6_isRadvdEnabled_dev2(void)
{
   InstanceIdStack iidStack;
   UBOOL8 enabled = FALSE;
   Dev2RouterAdvertisementInterfaceSettingObject *raIntfObj=NULL;
   UBOOL8 foundRa = FALSE;

   cmsLog_debug("Enter");
   INIT_INSTANCE_ID_STACK(&iidStack);

   /* FIXME: We assume Dev2RouterAdvertisementObject->enable is always true */
   /* FIXME: In the future, we may query whether radvd on brX is enabled */
   while (!foundRa && cmsObj_getNext(MDMOID_DEV2_ROUTER_ADVERTISEMENT_INTERFACE_SETTING,
                        &iidStack, (void **) &raIntfObj) == CMSRET_SUCCESS)
   {
      foundRa = TRUE;
      enabled = raIntfObj->enable;
      cmsObj_free((void **) &raIntfObj);
   }

   if (!foundRa)
   {
      cmsLog_error("cannot find Dev2RouterAdvertisementInterfaceSettingObject");
   }

   cmsLog_debug("enabled<%d>", enabled);
   return enabled;
}

void qdmIpv6_getRadvdUlaInfo_dev2(UBOOL8 *enblUla, UBOOL8 *randomUla, 
                                  char *ulaPrefix, SINT32 *plt, SINT32 *vlt)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStackUlaPrefix = EMPTY_INSTANCE_ID_STACK;
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   Dev2Ipv6PrefixObject *ipv6Prefix = NULL;
   UBOOL8 found = FALSE;

   cmsLog_debug("Enter");
   /*
    * Find the default lan ip interface "br0" 
    * FIXME: In the future, we may query ULA prefix info on brX
    */
   while (!found && (cmsObj_getNext(MDMOID_DEV2_IP_INTERFACE, &iidStack, (void **) &ipIntfObj)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(ipIntfObj->name, "br0"))
      {
         found = TRUE;
         *enblUla = ipIntfObj->ULAEnable;
         *randomUla = ipIntfObj->X_BROADCOM_COM_ULARandom;
      }

      cmsObj_free((void **) &ipIntfObj);
   }
   
   if (!found)
   {
      cmsLog_error("Failed to fund the default lan ip interface br0");
      return;
   }

   found = FALSE;
   while (!found && cmsObj_getNextInSubTreeFlags(MDMOID_DEV2_IPV6_PREFIX,
                        &iidStack, &iidStackUlaPrefix,
                        OGF_NO_VALUE_UPDATE,
                        (void **)&ipv6Prefix) == CMSRET_SUCCESS)
   {
      /* FIXME: only support ONE ULA prefix on one interface */
      if (ipv6Prefix->X_BROADCOM_COM_UniqueLocalFlag)
      {
         found = TRUE;
         cmsUtl_strncpy(ulaPrefix, ipv6Prefix->prefix, CMS_IPADDR_LENGTH);
         *plt = ipv6Prefix->X_BROADCOM_COM_Plt/3600;
         *vlt = ipv6Prefix->X_BROADCOM_COM_Vlt/3600;
      }
      cmsObj_free((void **) &ipv6Prefix);
   }

   /* FIXME: In the future, we may query ULA prefix info on brX */
   cmsLog_debug("enblRadvdUla<%d> enblRandomULA<%d> prefix<%s> plt<%d> vlt<%d>",
                *enblUla, *randomUla, ulaPrefix, *plt, *vlt);
} 


/* To be deleted later on if Broacom IPV6 is not used any more */
CmsRet qdmIpv6_getSitePrefixInfo_dev2(char *sitePrefixType, char *pdWanIfc, char *sitePrefix)
{
//   InstanceIdStack iidStack;
   CmsRet ret = CMSRET_SUCCESS;
   
   if (sitePrefixType == NULL && pdWanIfc == NULL && sitePrefix == NULL)
   {
      cmsLog_error("NULL string.");
      return CMSRET_INVALID_ARGUMENTS;
   }
//   INIT_INSTANCE_ID_STACK(&iidStack);

   return ret;        
}


CmsRet qdmIpv6_getIpPrefixInfo_dev2(const char *ifName, const char *origin, const char *staticType, char *prefix, UINT32 prefixLen)
{
   CmsRet ret;
   Dev2Ipv6PrefixObject *ipv6PrefixObj=NULL;
   UBOOL8 found=FALSE;
   Dev2IpInterfaceObject *ipIntfObj=NULL;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStackIpv6Prefix=EMPTY_INSTANCE_ID_STACK;

   cmsLog_debug("Enter: ifName/origin/staticType: %s/%s/%s", ifName, origin, staticType);
   
   if (ifName == NULL && prefix == NULL && origin == NULL && staticType == NULL)
   {
      cmsLog_error("NULL string.");
      return CMSRET_INVALID_ARGUMENTS;
   }
   
   *prefix = '\0';

   while (!found  &&
         (ret = cmsObj_getNextFlags(MDMOID_DEV2_IP_INTERFACE, &iidStack,
                                    OGF_NO_VALUE_UPDATE, (void **) &ipIntfObj)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(ipIntfObj->name, ifName))
      {
         while (!found && 
                (ret = cmsObj_getNextInSubTree(MDMOID_DEV2_IPV6_PREFIX,
                                           &iidStack, 
                                           &iidStackIpv6Prefix,
                                           (void **) &ipv6PrefixObj)) == CMSRET_SUCCESS)
         {
            cmsLog_debug("ipIntfObj->name %s ipv6PrefixObj->origin %s origin %s, ipv6PrefixObj->staticType %s", 
                     ipIntfObj->name, ipv6PrefixObj->origin, origin, staticType);

         
            if ((!cmsUtl_strcmp(ipv6PrefixObj->origin, origin)) &&
                (!cmsUtl_strcmp(ipv6PrefixObj->staticType, staticType)))
            {
               
               if (!IS_EMPTY_STRING(ipv6PrefixObj->prefix))
               {
                  if (cmsUtl_strlen(ipv6PrefixObj->prefix) >= (SINT32) prefixLen)
                  {
                     cmsLog_error("ipv6Prefix %s is too long for buf %d (truncate!)",
                                  ipv6PrefixObj->prefix, prefixLen);
                  }
                  cmsUtl_strncpy(prefix, ipv6PrefixObj->prefix, prefixLen);
                  found = TRUE;
                  ret = CMSRET_SUCCESS;
               }
               else
               {
                  cmsLog_debug("prefix not found: on %s", ifName);
               }
            }
            cmsObj_free((void **) &ipv6PrefixObj);
         }
      }         

      cmsObj_free((void **) &ipIntfObj);
   }

   cmsLog_debug("found<%d>/prefix<%s>", found, prefix);

   return ret;   
   
}  


#endif  /* DMP_X_BROADCOM_COM_DEV2_IPV6_1 */
#endif /* DMP_DEVICE2_BASELINE_1 */


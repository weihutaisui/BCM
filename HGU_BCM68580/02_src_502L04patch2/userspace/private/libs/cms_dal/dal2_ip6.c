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

#ifdef DMP_DEVICE2_BASELINE_1    /* this file touches TR181 objects */
#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1

CmsRet dalIp_addIpIntfIPv6Prefix_dev2(const InstanceIdStack *ipIntfIidStack,
                                      const WEB_NTWK_VAR *webVar,
                                      char *myPathRef, 
                                      UINT32 pathLen)
{
   InstanceIdStack iidStack =  *ipIntfIidStack;
   Dev2Ipv6PrefixObject *ipv6Prefix = NULL;   
   char addrtmp[CMS_IPADDR_LENGTH];
   MdmPathDescriptor pathDesc;      
   char *fullPathStringPtr=NULL;
   CmsRet ret;
   
   cmsLog_debug("Enter");
   if ((ret = cmsObj_addInstance(MDMOID_DEV2_IPV6_PREFIX, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to add device2.ip.interface.{i}.IPv6Prefix, ret = %d", ret);
      return ret;
   }

   if ((ret = cmsObj_get(MDMOID_DEV2_IPV6_PREFIX, &iidStack, 0, (void **) &ipv6Prefix)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get device2.ip.interface.{i}.IPv6Preifx, ret = %d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_IPV6_PREFIX, &iidStack);
      return ret;
   }
   ipv6Prefix->enable = TRUE;
   cmsNet_subnetIp6SitePrefix(webVar->wanAddr6, 0, 64, addrtmp);
   if (!IS_EMPTY_STRING(addrtmp))
   {
      sprintf(addrtmp, "%s/64", addrtmp);
      CMSMEM_REPLACE_STRING(ipv6Prefix->prefix, addrtmp);      
   }      

   if ((ret = cmsObj_set(ipv6Prefix, &iidStack)) != CMSRET_SUCCESS)
   {
       cmsLog_error("Failed setting ipPrefix. ret %d", ret);
       cmsObj_deleteInstance(MDMOID_DEV2_IPV6_PREFIX, &iidStack);
       return ret;
   }

   cmsObj_free((void **) &ipv6Prefix);

   memset(&pathDesc, 0, sizeof(MdmPathDescriptor));
   pathDesc.oid = MDMOID_DEV2_IPV6_PREFIX;
   pathDesc.iidStack = iidStack;
   if ((ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &fullPathStringPtr)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_pathDescriptorToFullPathNoEndDot returns error. ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_IPV6_PREFIX, &iidStack);
      return ret;
   }
   if (cmsUtl_strlen(fullPathStringPtr)+1  > (SINT32) pathLen)
   {
      cmsLog_error("fullpath %s too long to fit in param", fullPathStringPtr);
   }
   else
   {
      cmsUtl_strncpy(myPathRef, fullPathStringPtr, pathLen);
   }
   
   CMSMEM_FREE_BUF_AND_NULL_PTR(fullPathStringPtr);

   cmsLog_debug("Exit, ret=%d", ret);
   
   return ret;
   
}


CmsRet dalIp_addIPv6DelegatedPrefix_dev2(const InstanceIdStack *ipIntfIidStack,
                                              char *myPathRef, UINT32 pathLen)
{
   InstanceIdStack iidStack =  *ipIntfIidStack;
   Dev2Ipv6PrefixObject *ipv6Prefix = NULL;   
   MdmPathDescriptor pathDesc;      
   char *fullPathStringPtr=NULL;
   CmsRet ret;

   cmsLog_debug("Enter");

   if (myPathRef == NULL)
   {
      cmsLog_error("invalid argument");
      return CMSRET_INVALID_ARGUMENTS;
   }

   if ((ret = cmsObj_addInstance(MDMOID_DEV2_IPV6_PREFIX, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to add device2.ip.interface.{i}.IPv6Prefix, ret = %d", ret);
      return ret;
   }

   if ((ret = cmsObj_get(MDMOID_DEV2_IPV6_PREFIX, &iidStack, 0, (void **) &ipv6Prefix)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get device2.ip.interface.{i}.IPv6Preifx, ret = %d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_IPV6_PREFIX, &iidStack);
      return ret;
   }
   ipv6Prefix->enable = TRUE;
   CMSMEM_REPLACE_STRING(ipv6Prefix->origin, MDMVS_STATIC);
   CMSMEM_REPLACE_STRING(ipv6Prefix->staticType, MDMVS_PREFIXDELEGATION);

   if ((ret = cmsObj_set(ipv6Prefix, &iidStack)) != CMSRET_SUCCESS)
   {
       cmsLog_error("Failed setting ipPrefix. ret %d", ret);
       cmsObj_deleteInstance(MDMOID_DEV2_IPV6_PREFIX, &iidStack);
       return ret;
   }

   memset(&pathDesc, 0, sizeof(MdmPathDescriptor));
   pathDesc.oid = MDMOID_DEV2_IPV6_PREFIX;
   pathDesc.iidStack = iidStack;
   if ((ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &fullPathStringPtr)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_pathDescriptorToFullPathNoEndDot returns error. ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_IPV6_PREFIX, &iidStack);
      return ret;
   }
   if (cmsUtl_strlen(fullPathStringPtr)+1  > (SINT32) pathLen)
   {
      cmsLog_error("fullpath %s too long to fit in param", fullPathStringPtr);
   }
   else
   {
      cmsUtl_strncpy(myPathRef, fullPathStringPtr, pathLen);
   }
   
   CMSMEM_FREE_BUF_AND_NULL_PTR(fullPathStringPtr);

   cmsLog_debug("Exit, ret=%d", ret);
   
   return ret;
}


CmsRet dalIp_addIPv6ChildPrefix_dev2(const char *ifname,
                           const char *parentRef, const char *childPrefixBits)
{
   InstanceIdStack iidStack;
   Dev2Ipv6PrefixObject *ipv6Prefix = NULL;
   UBOOL8 found=FALSE;
   void *mdmObj = NULL;
   CmsRet ret;

   cmsLog_debug("ifname/parent/child", ifname, parentRef, childPrefixBits);

   if (IS_EMPTY_STRING(ifname) ||IS_EMPTY_STRING(parentRef) ||
       IS_EMPTY_STRING(childPrefixBits))
   {
      cmsLog_error("invalid argument");
      return CMSRET_INVALID_ARGUMENTS;
   }

   /* Obtain iid of corresponding ifname (br0) */
   INIT_INSTANCE_ID_STACK(&iidStack);      
   while ((!found) &&
          ((ret = cmsObj_getNext(MDMOID_DEV2_IP_INTERFACE, &iidStack, (void **) &mdmObj)) == CMSRET_SUCCESS))
   {
      found = (0 == cmsUtl_strcmp(((Dev2IpInterfaceObject*)mdmObj)->name, ifname));
      cmsObj_free((void **) &mdmObj);
   }

   /* Add prefix object to the corresponding ip.interface.i */
   if ((ret = cmsObj_addInstance(MDMOID_DEV2_IPV6_PREFIX, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to add device2.ip.interface.{i}.IPv6Prefix, ret = %d", ret);
      return ret;
   }

   if ((ret = cmsObj_get(MDMOID_DEV2_IPV6_PREFIX, &iidStack, 0, (void **) &ipv6Prefix)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get device2.ip.interface.{i}.IPv6Preifx, ret = %d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_IPV6_PREFIX, &iidStack);
      return ret;
   }
   ipv6Prefix->enable = TRUE;
   CMSMEM_REPLACE_STRING(ipv6Prefix->origin, MDMVS_STATIC);
   CMSMEM_REPLACE_STRING(ipv6Prefix->staticType, MDMVS_CHILD);
   CMSMEM_REPLACE_STRING(ipv6Prefix->parentPrefix, parentRef);
   CMSMEM_REPLACE_STRING(ipv6Prefix->childPrefixBits, childPrefixBits);

   if ((ret = cmsObj_set(ipv6Prefix, &iidStack)) != CMSRET_SUCCESS)
   {
       cmsLog_error("Failed setting ipPrefix. ret %d", ret);
       cmsObj_deleteInstance(MDMOID_DEV2_IPV6_PREFIX, &iidStack);
       return ret;
   }
   cmsLog_debug("Exit, ret=%d", ret);
   
   return ret;
}


CmsRet dalIp_addIpIntfIpv6Address_dev2(const InstanceIdStack *ipIntfIidStack, 
                                       const WEB_NTWK_VAR *webVar,
                                       char * prefixPathRef)
{

   InstanceIdStack iidStack = *ipIntfIidStack;
   Dev2Ipv6AddressObject *ipv6AddrObj = NULL;
   char addrtmp[CMS_IPADDR_LENGTH];
   char *tmp;   
   CmsRet ret;
   
   cmsLog_debug("Enter");     
   if ((ret = cmsObj_addInstance(MDMOID_DEV2_IPV6_ADDRESS, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to add MDMOID_DEV2_IPV6_ADDRESS Instance, ret = %d", ret);
      return ret;
   } 

   if ((ret = cmsObj_get(MDMOID_DEV2_IPV6_ADDRESS, &iidStack, 0, (void **) &ipv6AddrObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get ipv6AddrObj, ret = %d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_IPV6_ADDRESS, &iidStack);
      return ret;
   }

   ipv6AddrObj->enable = TRUE;
   cmsUtl_strncpy(addrtmp, webVar->wanAddr6, sizeof(addrtmp)-1);
   tmp = strchr(addrtmp, '/');
   *tmp = '\0';
   CMSMEM_REPLACE_STRING(ipv6AddrObj->IPAddress, addrtmp);
   CMSMEM_REPLACE_STRING(ipv6AddrObj->prefix, prefixPathRef);
   
   /* wanAddr6Type is either STATIC or DHCP here */
   CMSMEM_REPLACE_STRING(ipv6AddrObj->origin, webVar->wanAddr6Type);

   /* Enable the ipv6 address object */
   ipv6AddrObj->enable = TRUE;
   if ((ret = cmsObj_set(ipv6AddrObj, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set ipv6AddrObj. ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_IPV6_ADDRESS, &iidStack);
   } 
   cmsObj_free((void **) &ipv6AddrObj); 

   cmsLog_debug("Exit, ret=%d", ret);
   
   return ret;
   
}      



CmsRet dalIp_addIpIntfDhcpv6Client_dev2(const WEB_NTWK_VAR *webVar, const char * ipIntfPathRef)
{                                        
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   Dev2Dhcpv6ClientObject *dhcp6ClientObj=NULL;
   CmsRet ret;
   
   cmsLog_debug("enter");
   
   if ((ret = cmsObj_addInstance(MDMOID_DEV2_DHCPV6_CLIENT, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to add MDMOID_DEV2_DHCPV6_CLIENT Instance, ret = %d", ret);
      return ret;
   } 

   if ((ret = cmsObj_get(MDMOID_DEV2_DHCPV6_CLIENT, &iidStack, 0, (void **) &dhcp6ClientObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get dhcp6ClientObj, ret = %d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_DHCPV6_CLIENT, &iidStack);
      return ret;
   }

   dhcp6ClientObj->enable = TRUE;
   dhcp6ClientObj->requestAddresses = webVar->dhcp6cForAddr;
   dhcp6ClientObj->requestPrefixes = webVar->dhcp6cForPd;
   dhcp6ClientObj->X_BROADCOM_COM_UnnumberedModel = webVar->unnumberedModel;

   CMSMEM_REPLACE_STRING(dhcp6ClientObj->interface, ipIntfPathRef);

   ret = cmsObj_set(dhcp6ClientObj, &iidStack);
   cmsObj_free((void **) &dhcp6ClientObj); 

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set dhcp6ClientObj. ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_DHCPV6_CLIENT, &iidStack);
   }     
   
   cmsLog_debug("Exit, ret=%d", ret);

   return ret;

}


CmsRet dalIp_deleteDhcpv6Client_dev2(const char *ipIntfFullPath)
{
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   Dev2Dhcpv6ClientObject *dhcpv6ClientObj=NULL;
   UBOOL8 found=FALSE;
   CmsRet ret;

   cmsLog_debug("enter: ipIntf %s", ipIntfFullPath);

   while (!found &&
          (ret = cmsObj_getNextFlags(MDMOID_DEV2_DHCPV6_CLIENT,
                              &iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **)&dhcpv6ClientObj)) == CMSRET_SUCCESS)
    {
       if (!cmsUtl_strcmp(dhcpv6ClientObj->interface, ipIntfFullPath))
       {
          found = TRUE;
          cmsObj_deleteInstance(MDMOID_DEV2_DHCPV6_CLIENT, &iidStack);
       }
       cmsObj_free((void **)&dhcpv6ClientObj);
    }

    cmsLog_debug("Exit: found=%d ret=%d", found, ret);

   return ret;
}


CmsRet dalIp_addIpIntfDnsClientServerIpv6_dev2(const WEB_NTWK_VAR *webVar, const char * ipIntfPathRef)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2DnsServerObject *dnsServerObj=NULL;
   char dnsServers[BUFLEN_32 * 2]={0};
   CmsRet ret;
   
   cmsLog_debug("Enter.");
   
   if ((ret = cmsObj_addInstance(MDMOID_DEV2_DNS_SERVER, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to add MDMOID_DEV2_DNS_SERVER Instance, ret = %d", ret);
      return ret;
   } 

   if ((ret = cmsObj_get(MDMOID_DEV2_DNS_SERVER, &iidStack, 0, (void **) &dnsServerObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get dnsServerObj, ret = %d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_DNS_SERVER, &iidStack);
      return ret;
   }

   /* For static, use STATIC or dynamic */
   if (!cmsUtl_strcmp(webVar->dns6Type, MDMVS_STATIC))
   {
      /* For static dns,  save the static dns ip (up to 2) in DNSServer. */
      snprintf(dnsServers, sizeof(dnsServers), "%s,%s", webVar->dns6Pri, webVar->dns6Sec);
      CMSMEM_REPLACE_STRING(dnsServerObj->DNSServer, dnsServers);
   }
   else
   { 
      /* For dynamic IPoE, DNSServer is NULL.  dnsServerObj->type is read only param 
      * and can not be set by httpd.  Use DNSServer to distinquish if it is static or dynamic 
      */
      CMSMEM_FREE_BUF_AND_NULL_PTR(dnsServerObj->DNSServer);
   }

   /* Need to set the ip interface to dnsServer object */
   CMSMEM_REPLACE_STRING(dnsServerObj->interface, ipIntfPathRef);

   dnsServerObj->enable = TRUE;

   if ((ret = cmsObj_set(dnsServerObj, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set dnsServerObj. ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_DNS_SERVER, &iidStack);
   } 
   cmsObj_free((void **) &dnsServerObj); 

   cmsLog_debug("Exit, ret=%d", ret);

   return ret;
   
}


#endif /* DMP_X_BROADCOM_COM_DEV2_IPV6_1 */

#endif /* DMP_DEVICE2_BASELINE_1 */


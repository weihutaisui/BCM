/***********************************************************************
 *
 *  Copyright (c) 2007  Broadcom Corporation
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
#include "cms_qdm.h"


void dalTunnel_update6rdObject_dev2( const char *prefix __attribute__((unused)), const char *brAddr __attribute__((unused)), const char *ifName __attribute__((unused)), 
                                SINT32 ipv4MaskLen __attribute__((unused)), SINT32 ipv6PrefixLen  __attribute__((unused)))
{
   cmsLog_debug("v4MaskLen<%d> v6PrefixLen<%d> prefix<%s> brAddr<%s> ifname<%s>",
                ipv4MaskLen, ipv6PrefixLen, prefix, brAddr, ifName);

#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1 /* aka SUPPORT_IPV6 */
   if ( (cmsUtl_isValidIpAddress(AF_INET, brAddr) == FALSE) ||
         ( (ipv4MaskLen < 0) ||(ipv4MaskLen > 32) ) ||
         (cmsUtl_isValidIpAddress(AF_INET6, prefix) == FALSE) )
   {
      cmsLog_debug("Invalid 6rd configuration");
   }
   else
   {
      UBOOL8 found = FALSE;
      Dev2Ipv6rdInterfaceSettingObject *ipv6rdObj = NULL;
      InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
      char *ifpath = NULL;
      UBOOL8 isLayer2 = FALSE;

      if (qdmIntf_intfnameToFullPathLocked(ifName, isLayer2, &ifpath) != CMSRET_SUCCESS)
      {
         cmsLog_error("cannot get %s's full path", ifName);
         return;
      }

      while ( !found &&
              (cmsObj_getNextFlags(MDMOID_DEV2_IPV6RD_INTERFACE_SETTING, &iidStack, 
                 OGF_NO_VALUE_UPDATE, (void **)&ipv6rdObj) == CMSRET_SUCCESS) )
      {
         if (cmsUtl_strcmp(ipv6rdObj->X_BROADCOM_COM_TunneledInterface, ifpath) == 0)
         {
            found = TRUE;

            /* only update 6rd info for dynamic tunnel case */
            if (ipv6rdObj->X_BROADCOM_COM_Dynamic == TRUE)
            {
               char ipv6rdPrefix[CMS_IPADDR_LENGTH];

               snprintf(ipv6rdPrefix, sizeof(ipv6rdPrefix), "%s/%d", prefix, ipv6PrefixLen);
               ipv6rdObj->IPv4MaskLength = ipv4MaskLen;
               REPLACE_STRING_IF_NOT_EQUAL(ipv6rdObj->SPIPv6Prefix, ipv6rdPrefix);
               REPLACE_STRING_IF_NOT_EQUAL(ipv6rdObj->borderRelayIPv4Addresses, brAddr);

               if ( cmsObj_set(ipv6rdObj, &iidStack) != CMSRET_SUCCESS )
               {
                  cmsLog_error("Failed to set MDMOID_DEV2_IPV6RD_INTERFACE_SETTING object");
               }
            }
         }
   
         cmsObj_free((void **)&ipv6rdObj);
      }
   
      if ( !found )
      {
         cmsLog_debug("dhcpc receives 6rdOpt but no 6rd tunnel configuration");
      }

      CMSMEM_FREE_BUF_AND_NULL_PTR(ifpath);
   }
#endif

   return;
}


#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1 /* aka SUPPORT_IPV6 */
static CmsRet getFullPathsAndStatus(const char *lanIntf, const char *wanIntf,
                                    char **lanfullPath, char **wanfullPath, 
                                    UBOOL8 *wanUp, UBOOL8 is6rd)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2IpInterfaceObject *ipIntfObj=NULL;
   UBOOL8 foundLan = FALSE;
   UBOOL8 foundWan = FALSE;
   MdmPathDescriptor pathDesc;
   CmsRet ret = CMSRET_SUCCESS;

   INIT_PATH_DESCRIPTOR(&pathDesc);
   pathDesc.oid = MDMOID_DEV2_IP_INTERFACE;

   cmsLog_debug("enter lan/wan: %s/%s", lanIntf, wanIntf);
   while ((!foundLan || !foundWan) &&
          (cmsObj_getNextFlags(MDMOID_DEV2_IP_INTERFACE, &iidStack, OGF_NO_VALUE_UPDATE, (void **) &ipIntfObj)) == CMSRET_SUCCESS)
   {
      pathDesc.iidStack = iidStack;

      if (!cmsUtl_strcmp(wanIntf, ipIntfObj->name))
      {
         /* 
          * In hybrid 6rd senario, always use WANIPConn/WANPppConn 
          * instead of IP.Interface because IP.Interface may not
          * be available 
          */
         if (!is6rd || (is6rd && cmsMdm_isDataModelDevice2()))
         {
            foundWan = TRUE;

            if (cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, wanfullPath) != CMSRET_SUCCESS)
            {
               cmsLog_error("cmsMdm_pathDescriptorToFullPath returns error. ret=%d", ret);
               cmsObj_free((void **) &ipIntfObj);
               return CMSRET_INTERNAL_ERROR;
            }

            if (is6rd)
            {
               *wanUp = (cmsUtl_strcmp(ipIntfObj->X_BROADCOM_COM_IPv4ServiceStatus, MDMVS_SERVICEUP) == 0);
            }
            else
            {
               *wanUp = (cmsUtl_strcmp(ipIntfObj->X_BROADCOM_COM_IPv6ServiceStatus, MDMVS_SERVICEUP) == 0);
            }
         }
      }
      else if (!cmsUtl_strcmp(lanIntf, ipIntfObj->name))
      {
         foundLan = TRUE;

         if (cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, lanfullPath) != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsMdm_pathDescriptorToFullPath returns error. ret=%d", ret);
            cmsObj_free((void **) &ipIntfObj);
            return CMSRET_INTERNAL_ERROR;
         }
      }

      cmsObj_free((void **) &ipIntfObj);
   }

   if (!foundWan || !foundLan)
   {
      if (!cmsMdm_isDataModelDevice2() && is6rd && !foundWan)
      {
         UBOOL8 islayer2 = FALSE;
         UBOOL8 isIPv4 = TRUE;

         /* 
          * In hybrid model 6rd scenario, pure IPv4 WAN does not 
          * have IP.Interface. So return fullpath to WANIPConn/WANPppConn
          */
         qdmIntf_intfnameToFullPathLocked(wanIntf, islayer2, wanfullPath);
         *wanUp = qdmIpIntf_isWanInterfaceUpLocked(wanIntf, isIPv4);
      }
      else
      {
         cmsLog_error("Invalid parameters");
         ret = CMSRET_INVALID_PARAM_VALUE;
      }
   }

   return ret;
}

CmsRet dal6in4Tunnel_add_dev2(const char* tunnelName, const char *mechanism, const char *wanIntf, const char *lanIntf, 
	                     UBOOL8 dynamic, const char *ipv6rdPrefix, SINT8 ipv4MaskLen, const char *brAddr)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 wanUp = FALSE;
   char *lanfullPath=NULL;
   char *wanfullPath=NULL;
   Dev2Ipv6rdInterfaceSettingObject *ipv6rdObj = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("Adding new 6in4 tunnel: %s/%s/%s/%s/%d", tunnelName, mechanism, wanIntf, lanIntf, dynamic);
   cmsLog_debug("6rdPrefix<%s> brAddr<%s> maskLen<%d>", ipv6rdPrefix, brAddr, ipv4MaskLen);

   if ( (tunnelName == NULL) || (mechanism == NULL) || (wanIntf == NULL) || (lanIntf == NULL) ||
   	  ((dynamic == 0) && ((ipv6rdPrefix == NULL) || (brAddr == NULL)) ) )
   {
      cmsLog_error("Invalid parameters");
      return CMSRET_INVALID_PARAM_VALUE;
   }

   if (getFullPathsAndStatus(lanIntf, wanIntf, &lanfullPath, &wanfullPath, &wanUp, TRUE) != CMSRET_SUCCESS)
   {
      goto free_ret;
   }

   /*
    * FIXME: webUI doesn't allow more than one 6rd tunnel configuration.
    * So we don't need to check existing configuration before add.
    */
   if ((ret = cmsObj_addInstance(MDMOID_DEV2_IPV6RD_INTERFACE_SETTING, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create MDMOID_DEV2_DSLITE_INTERFACE_SETTING object, ret=%d", ret);
      goto free_ret;
   }

   if ((ret = cmsObj_get(MDMOID_DEV2_IPV6RD_INTERFACE_SETTING, &iidStack, 0, (void **) &ipv6rdObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get dsLiteObj, ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_IPV6RD_INTERFACE_SETTING, &iidStack);
      goto free_ret;
   }

   /* FIXME: 
    * allTrafficToBorderRelay and addressSource are not supported
    */
   REPLACE_STRING_IF_NOT_EQUAL(ipv6rdObj->X_BROADCOM_COM_TunnelInterface, lanfullPath);
   REPLACE_STRING_IF_NOT_EQUAL(ipv6rdObj->X_BROADCOM_COM_TunneledInterface, wanfullPath);
   REPLACE_STRING_IF_NOT_EQUAL(ipv6rdObj->X_BROADCOM_COM_TunnelName, tunnelName);
   ipv6rdObj->enable = TRUE;
   ipv6rdObj->X_BROADCOM_COM_Dynamic = dynamic;

   if ( dynamic == FALSE )
   {
      if (cmsUtl_isValidIpAddress(AF_INET, brAddr) == FALSE || !strcmp(brAddr, "0.0.0.0"))
      {
         cmsLog_error("Invalid border relay IP address");
         cmsObj_deleteInstance(MDMOID_DEV2_IPV6RD_INTERFACE_SETTING, &iidStack);
         goto free_obj;
      }

      if ( (ipv4MaskLen < 0) ||(ipv4MaskLen > 32) )
      {
         cmsLog_error("Invalid IPv4 mask length");
         cmsObj_deleteInstance(MDMOID_DEV2_IPV6RD_INTERFACE_SETTING, &iidStack);
         goto free_obj;
      }

      if (cmsUtl_isValidIpAddress(AF_INET6, ipv6rdPrefix) == FALSE)
      {
         cmsLog_error("Invalid 6rd prefix");
         cmsObj_deleteInstance(MDMOID_DEV2_IPV6RD_INTERFACE_SETTING, &iidStack);
         goto free_obj;
      }

      ipv6rdObj->IPv4MaskLength = ipv4MaskLen;
      REPLACE_STRING_IF_NOT_EQUAL(ipv6rdObj->SPIPv6Prefix, ipv6rdPrefix);
      REPLACE_STRING_IF_NOT_EQUAL(ipv6rdObj->borderRelayIPv4Addresses, brAddr);
   }
   else
   {
      ipv6rdObj->IPv4MaskLength = 0;
      CMSMEM_FREE_BUF_AND_NULL_PTR(ipv6rdObj->SPIPv6Prefix);
      CMSMEM_FREE_BUF_AND_NULL_PTR(ipv6rdObj->borderRelayIPv4Addresses);
   }

   if ( (ret = cmsObj_set(ipv6rdObj, &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error("Failed to set MDMOID_DEV2_IPV6RD_INTERFACE_SETTING object");
   }

free_obj:
   cmsObj_free((void **) &ipv6rdObj);

free_ret:
   CMSMEM_FREE_BUF_AND_NULL_PTR(lanfullPath);
   CMSMEM_FREE_BUF_AND_NULL_PTR(wanfullPath);

   return ret;
}

CmsRet dal4in6Tunnel_add_dev2(const char* tunnelName, const char *mechanism,
                              const char *wanIntf, const char *lanIntf,
                              UBOOL8 dynamic, const char *remoteIp)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2DsliteInterfaceSettingObject *dsLiteObj = NULL;
   UBOOL8 wanUp = FALSE;
   char *lanfullPath=NULL;
   char *wanfullPath=NULL;
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("Adding new 4in6 tunnel: %s/%s/%s/%s/%d/%s",
                tunnelName, mechanism, wanIntf, lanIntf, dynamic, remoteIp);

   if ( (tunnelName == NULL) || (mechanism == NULL) || (wanIntf == NULL) ||
   	    (lanIntf == NULL) ||
        ((dynamic == 0) && !cmsUtl_isValidIpAddress(AF_INET6, remoteIp)) )
   {
      cmsLog_error("Invalid parameters");
      return CMSRET_INVALID_PARAM_VALUE;
   }

   if (getFullPathsAndStatus(lanIntf, wanIntf, &lanfullPath, &wanfullPath, &wanUp, FALSE) != CMSRET_SUCCESS)
   {
      goto free_ret;
   }

   cmsLog_debug("%s/%s: %s/%s  wanUp<%d>", lanIntf, wanIntf, lanfullPath, wanfullPath, wanUp);

   /*
    * FIXME: webUI doesn't allow more than one dslite tunnel configuration.
    * So we don't need to check existing configuration before add.
    */
   if ((ret = cmsObj_addInstance(MDMOID_DEV2_DSLITE_INTERFACE_SETTING, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create MDMOID_DEV2_DSLITE_INTERFACE_SETTING object, ret=%d", ret);
      goto free_ret;
   }

   if ((ret = cmsObj_get(MDMOID_DEV2_DSLITE_INTERFACE_SETTING, &iidStack, 0, (void **) &dsLiteObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get dsLiteObj, ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_DSLITE_INTERFACE_SETTING, &iidStack);
      goto free_ret;
   }

   /*
    * EndpointAddress: If dynamic, empty string. Otherwise, set with remoteIp
    * EndpointName: If dynamic, dhcp6c will set with AFTR info
    *               Otherwise, empty string
    * Origin: In rcl, set to static if EndpointAddress is not empty
    * EndpointAddressInUse (in rcl only): 
    *   If dynamic, resolved addr of EndpointName is set.
    *   Otherwise, copy EndpointAddress to EndpointAddressInUse
    */

   /* 
    * FIXME: Assumption below
    *   Static AFTR configuration from GUI must be in address mode
    *   Dynamic AFTR configuration from DHCP must be in FQDN mode
    *   EndpointAssignmentPrecedence is not supported
    *   EndpointAddressTypePrecedence is not supported
    *
    *   Tunnel(ed)Interface in IP.Interface is not supported
    */
   REPLACE_STRING_IF_NOT_EQUAL(dsLiteObj->X_BROADCOM_COM_TunnelInterface, lanfullPath);
   REPLACE_STRING_IF_NOT_EQUAL(dsLiteObj->X_BROADCOM_COM_TunneledInterface, wanfullPath);
   REPLACE_STRING_IF_NOT_EQUAL(dsLiteObj->X_BROADCOM_COM_TunnelName, tunnelName);
   dsLiteObj->enable = TRUE;

   if ( dynamic == FALSE )
   {
      REPLACE_STRING_IF_NOT_EQUAL(dsLiteObj->endpointAddress, remoteIp);
   }

   if ( (ret = cmsObj_set(dsLiteObj, &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error("Failed to set MDMOID_DEV2_DSLITE_INTERFACE_SETTING object");
      cmsObj_deleteInstance(MDMOID_DEV2_DSLITE_INTERFACE_SETTING, &iidStack);
   }

   cmsObj_free((void **) &dsLiteObj);
   
free_ret:
   CMSMEM_FREE_BUF_AND_NULL_PTR(lanfullPath);
   CMSMEM_FREE_BUF_AND_NULL_PTR(wanfullPath);

   return ret;
}

CmsRet dalTunnel_delete_dev2(const char* tunnelName, const char *mode)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 found = FALSE;

   cmsLog_debug("Deleting a tunnel: name<%s> mode<%s>", tunnelName, mode);

   if (!cmsUtl_strcmp(mode, MDMVS_IPV4INIPV6))
   {
      Dev2DsliteInterfaceSettingObject *dsLiteObj = NULL;

      while (cmsObj_getNextFlags(MDMOID_DEV2_DSLITE_INTERFACE_SETTING, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&dsLiteObj) == CMSRET_SUCCESS)
      {
         if (!cmsUtl_strcmp(tunnelName, dsLiteObj->X_BROADCOM_COM_TunnelName))
         {
            found = TRUE;

            if ((ret = cmsObj_deleteInstance(MDMOID_DEV2_DSLITE_INTERFACE_SETTING, &iidStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("Failed to delete MDMOID_DEV2_DSLITE_INTERFACE_SETTING, ret = %d", ret);
            }
         }

         cmsObj_free((void **) &dsLiteObj);
      }
   }
   else
   {
      Dev2Ipv6rdInterfaceSettingObject *ipv6rdObj = NULL;

      while (cmsObj_getNextFlags(MDMOID_DEV2_IPV6RD_INTERFACE_SETTING, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&ipv6rdObj) == CMSRET_SUCCESS)
      {
         if (!cmsUtl_strcmp(tunnelName, ipv6rdObj->X_BROADCOM_COM_TunnelName))
         {
            found = TRUE;

            if ((ret = cmsObj_deleteInstance(MDMOID_DEV2_IPV6RD_INTERFACE_SETTING, &iidStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("Failed to delete MDMOID_DEV2_DSLITE_INTERFACE_SETTING, ret = %d", ret);
            }
         }

         cmsObj_free((void **) &ipv6rdObj);
      }
   }

   if(found == FALSE)
   {
      cmsLog_debug("no tunnel entry found with %s", tunnelName);
      return CMSRET_INVALID_ARGUMENTS;
   }

   return ret;
}

#ifdef SUPPORT_MAPT
CmsRet dalMapt_add_dev2(const char *wanIntf, const char *lanIntf,
                        UBOOL8 dynamic, const char *BRPrefix,
                        const char *ipv6Prefix, const char *ipv4Prefix,
                        UINT32 psidOffset, UINT32 psidLen, UINT32 psid)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack_map = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack_ipv6Prefix;
   Dev2MapDomainObject *domain = NULL;
   Dev2MapObject *map = NULL;
   MdmPathDescriptor pathDesc=EMPTY_PATH_DESCRIPTOR;   
   UBOOL8 wanUp = FALSE;
   char *lanfullPath=NULL;
   char *wanfullPath=NULL;
   char *prefixfullpath = NULL;   
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("Configure new MAP-T: %s/%s/%d/%s",
                wanIntf, lanIntf, dynamic, BRPrefix);

   if ( (wanIntf == NULL) || (lanIntf == NULL) ||
        ((dynamic == 0) && !cmsUtl_isValidIpAddress(AF_INET6, BRPrefix) &&
         !cmsUtl_isValidIpAddress(AF_INET6, ipv6Prefix)) )
   {
      cmsLog_error("Invalid parameters");
      return CMSRET_INVALID_PARAM_VALUE;
   }

   /* get delegated prefix object path associated wanIntf */
   {
      char *fullPath;

      ret = qdmIntf_intfnameToFullPathLocked_dev2(wanIntf,FALSE,&fullPath);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("cannot get fullpath of %s", wanIntf);
         return ret;
      }

      ret = cmsMdm_fullPathToPathDescriptor(fullPath, &pathDesc);
      CMSMEM_FREE_BUF_AND_NULL_PTR(fullPath);

      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("cannot get pathDesc of %s", wanIntf);
         return ret;
      }
   }

   if (!qdmIpIntf_findIpv6Prefix(&pathDesc.iidStack, NULL, MDMVS_STATIC,
                           MDMVS_PREFIXDELEGATION, &iidStack_ipv6Prefix))
   {
      cmsLog_notice("no PD prefix found");
      return CMSRET_INVALID_PARAM_VALUE;
   }

   if ((ret = cmsObj_get(MDMOID_DEV2_MAP, &iidStack_map, 0, (void **) &map)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get map, ret=%d", ret);
      return ret;
   }

   map->enable = TRUE;

   if ( (ret = cmsObj_set(map, &iidStack_map)) != CMSRET_SUCCESS )
   {
      cmsLog_error("Failed to set MDMOID_DEV2_MAP object");
      return ret;
   }

   if (getFullPathsAndStatus(lanIntf, wanIntf, &lanfullPath, &wanfullPath, &wanUp, FALSE) != CMSRET_SUCCESS)
   {
      goto free_ret;
   }

   cmsLog_debug("%s/%s: %s/%s  wanUp<%d>", lanIntf, wanIntf, lanfullPath, wanfullPath, wanUp);

   /*
    * FIXME: webUI doesn't allow more than one MAP-T configuration.
    * So we don't need to check existing configuration before add.
    */
   if ((ret = cmsObj_addInstance(MDMOID_DEV2_MAP_DOMAIN, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create MDMOID_DEV2_MAP_DOMAIN object, ret=%d", ret);
      goto free_ret;
   }

   if ((ret = cmsObj_get(MDMOID_DEV2_MAP_DOMAIN, &iidStack, 0, (void **) &domain)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get domain, ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_MAP_DOMAIN, &iidStack);
      goto free_ret;
   }

   domain->enable = TRUE;
   REPLACE_STRING_IF_NOT_EQUAL(domain->WANInterface, wanfullPath);
   REPLACE_STRING_IF_NOT_EQUAL(domain->transportMode, MDMVS_TRANSLATION);

   pathDesc.oid = MDMOID_DEV2_IPV6_PREFIX;
   pathDesc.iidStack = iidStack_ipv6Prefix;
   if (cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &prefixfullpath) != CMSRET_SUCCESS)
   {
      cmsLog_error("pathDescToFullPath failed!");
      goto free_ret;
   }
   REPLACE_STRING_IF_NOT_EQUAL(domain->IPv6Prefix, prefixfullpath);
   CMSMEM_FREE_BUF_AND_NULL_PTR(prefixfullpath);
   
   if ( dynamic == FALSE )
   {
      REPLACE_STRING_IF_NOT_EQUAL(domain->BRIPv6Prefix, BRPrefix);
      domain->PSIDOffset = psidOffset;
      domain->PSIDLength = psidLen;
      domain->PSID = psid;
   }
   else
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(domain->BRIPv6Prefix);
   }

   if ( (ret = cmsObj_set(domain, &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error("Failed to set MDMOID_DEV2_MAP_DOMAIN object");
      cmsObj_deleteInstance(MDMOID_DEV2_MAP_DOMAIN, &iidStack);
   }
   else
   {
      Dev2MapDomainRuleObject *rule = NULL;

      if ((ret = cmsObj_addInstance(MDMOID_DEV2_MAP_DOMAIN_RULE, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("could not create MDMOID_DEV2_MAP_DOMAIN_RULE object, ret=%d", ret);
         goto free_domain_ret;
      }

      if ((ret = cmsObj_get(MDMOID_DEV2_MAP_DOMAIN_RULE, &iidStack, 0, (void **) &rule)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to get domain_rule, ret=%d", ret);
         cmsObj_deleteInstance(MDMOID_DEV2_MAP_DOMAIN_RULE, &iidStack);
         goto free_domain_ret;
      }

      rule->enable = TRUE;

      /*
       * IPv6Prefix/IPv4Prefix: If dynamic, empty string.
       * Origin: In rcl, set to static if IPv6Prefix/IPv4Prefix is not empty
       */
      if ( dynamic == FALSE )
      {
         REPLACE_STRING_IF_NOT_EQUAL(rule->IPv6Prefix, ipv6Prefix);
         REPLACE_STRING_IF_NOT_EQUAL(rule->IPv4Prefix, ipv4Prefix);
      }
      else
      {
         CMSMEM_FREE_BUF_AND_NULL_PTR(rule->IPv6Prefix);
         CMSMEM_FREE_BUF_AND_NULL_PTR(rule->IPv4Prefix);
      }

      if ( (ret = cmsObj_set(rule, &iidStack)) != CMSRET_SUCCESS )
      {
         cmsLog_error("Failed to set MDMOID_DEV2_MAP_DOMAIN_RULE object");
         cmsObj_deleteInstance(MDMOID_DEV2_MAP_DOMAIN_RULE, &iidStack);
      }

      cmsObj_free((void **) &rule);
   }

free_domain_ret:
   cmsObj_free((void **) &domain);
   
free_ret:
   CMSMEM_FREE_BUF_AND_NULL_PTR(lanfullPath);
   CMSMEM_FREE_BUF_AND_NULL_PTR(wanfullPath);

   return ret;
}

CmsRet dalMapt_delete_dev2(const char* wanIntf)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack_map = EMPTY_INSTANCE_ID_STACK;
   Dev2MapDomainObject *domain = NULL;
   Dev2MapObject *map = NULL;
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 found = FALSE;

   if (wanIntf == NULL)
   {
      cmsLog_error("Invalid parameters");
      return CMSRET_INVALID_PARAM_VALUE;
   }

   cmsLog_debug("Delete MAP-T: %s", wanIntf);

   while (!found && cmsObj_getNextFlags(MDMOID_DEV2_MAP_DOMAIN, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&domain) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strcmp(wanIntf, domain->WANInterface) == 0)
      {
         found = TRUE;
      }

      cmsObj_free((void **) &domain);
   }

   if (!found)
   {
      cmsLog_debug("cannot find domain obj with %s", wanIntf);
      return CMSRET_INVALID_PARAM_VALUE;
   }

   cmsObj_deleteInstance(MDMOID_DEV2_MAP_DOMAIN, &iidStack);


   if ((ret = cmsObj_get(MDMOID_DEV2_MAP, &iidStack_map, 0, (void **) &map)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get map, ret=%d", ret);
      return ret;
   }

   map->enable = FALSE;

   if ( (ret = cmsObj_set(map, &iidStack_map)) != CMSRET_SUCCESS )
   {
      cmsLog_error("Failed to set MDMOID_DEV2_MAP object");
   }

   return ret;
}
#endif

#endif

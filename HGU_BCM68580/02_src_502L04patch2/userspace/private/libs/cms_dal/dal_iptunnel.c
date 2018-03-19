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

void dalTunnel_update6rdObject_igd( const char *prefix, const char *brAddr, const char *ifName, 
                                SINT32 ipv4MaskLen, SINT32 ipv6PrefixLen )
{
    cmsLog_debug("v4MaskLen<%d> v6PrefixLen<%d> prefix<%s> brAddr<%s> ifname<%s>",
                 ipv4MaskLen, ipv6PrefixLen, prefix, brAddr, ifName);

#ifdef DMP_X_BROADCOM_COM_IPV6_1 /* aka SUPPORT_IPV6 */
   {
   UBOOL8 found = FALSE;
   IPTunnelObject *tunnelCfg;
   InstanceIdStack iidStackTunnel=EMPTY_INSTANCE_ID_STACK;

   if ( (cmsUtl_isValidIpAddress(AF_INET, brAddr) == FALSE) ||
         ( (ipv4MaskLen < 0) ||(ipv4MaskLen > 32) ) ||
         (cmsUtl_isValidIpAddress(AF_INET6, prefix) == FALSE) )
   {
      cmsLog_debug("Invalid 6rd configuration");
   }
   else
   {
      while ( !found &&
              (cmsObj_getNextFlags(MDMOID_IP_TUNNEL, &iidStackTunnel, 
                 OGF_NO_VALUE_UPDATE, (void **)&tunnelCfg) == CMSRET_SUCCESS) )
      {
         if (cmsUtl_strcmp(tunnelCfg->associatedWanIfName, ifName) == 0)
         {
            found = TRUE;
         }
   
         cmsObj_free((void **)&tunnelCfg);
      }
   
      if ( found )
      {
         Ipv6inIpv4TunnelObject *ipv6inipv4Obj;
         InstanceIdStack iidStack1=EMPTY_INSTANCE_ID_STACK;
   
         if( cmsObj_getNextInSubTreeFlags(MDMOID_IPV6IN_IPV4_TUNNEL, &iidStackTunnel, &iidStack1, 
                                OGF_NO_VALUE_UPDATE, (void **)&ipv6inipv4Obj) == CMSRET_SUCCESS )
         {
            char ipv6rdPrefix[CMS_IPADDR_LENGTH];
   
            /* only update 6rd info for dynamic tunnel case */
            if (ipv6inipv4Obj->dynamic == TRUE)
            {            
               snprintf(ipv6rdPrefix, sizeof(ipv6rdPrefix), "%s/%d", prefix, ipv6PrefixLen);
               ipv6inipv4Obj->ipv4MaskLen = ipv4MaskLen;
               REPLACE_STRING_IF_NOT_EQUAL(ipv6inipv4Obj->prefix, ipv6rdPrefix);
               REPLACE_STRING_IF_NOT_EQUAL(ipv6inipv4Obj->borderRelayAddress, brAddr);
   
               cmsObj_set(ipv6inipv4Obj, &iidStack1);
            }
   
            cmsObj_free((void **)&ipv6inipv4Obj);
         }
         else
         {
            cmsLog_debug("dhcpc receives 6rdOpt but no 6rd tunnel configuration");
         }
      }
      else
      {
         cmsLog_debug("dhcpc receives 6rdOpt but no 6rd tunnel configuration");
      }
   }
   }
#endif

   return;
}


#ifdef DMP_X_BROADCOM_COM_IPV6_1 /* aka SUPPORT_IPV6 */
void dalTunnel_updateDSLiteObject( const char *aftr, const char *ifName )
{
   UBOOL8 found = FALSE;
   IPTunnelObject *tunnelCfg;
   InstanceIdStack iidStackTunnel=EMPTY_INSTANCE_ID_STACK;

   cmsLog_debug("aftr<%s> ifName<%s>", aftr, ifName);

   while ( !found &&
           (cmsObj_getNextFlags(MDMOID_IP_TUNNEL, &iidStackTunnel, 
                 OGF_NO_VALUE_UPDATE, (void **)&tunnelCfg) == CMSRET_SUCCESS) )
   {
      if (cmsUtl_strcmp(tunnelCfg->associatedWanIfName, ifName) == 0)
      {
         found = TRUE;
      }

      cmsObj_free((void **)&tunnelCfg);
   }
   
   if ( found )
   {
      Ipv4inIpv6TunnelObject *ipv4inipv6Obj;
      InstanceIdStack iidStack1=EMPTY_INSTANCE_ID_STACK;
   
      if( cmsObj_getNextInSubTreeFlags(MDMOID_IPV4IN_IPV6_TUNNEL, &iidStackTunnel, &iidStack1, 
                             OGF_NO_VALUE_UPDATE, (void **)&ipv4inipv6Obj) == CMSRET_SUCCESS )
      {
         REPLACE_STRING_IF_NOT_EQUAL(ipv4inipv6Obj->currentAftrName, aftr);
         cmsObj_set(ipv4inipv6Obj, &iidStack1);
         cmsObj_free((void **)&ipv4inipv6Obj);
      }
      else
      {
         cmsLog_debug("dhcp6c receives AFTR option but no DS-Lite configured");
      }
   }
   else
   {
      cmsLog_debug("dhcp6c receives AFTR option but no DS-Lite configuration");
   }

   return;
}


CmsRet fill6in4ObjCfg( const char* mechanism, UBOOL8 dynamic, const char *ipv6rdPrefix, 
	                   SINT8 ipv4MaskLen, const char *brAddr, Ipv6inIpv4TunnelObject *ip6in4Obj )
{
   REPLACE_STRING_IF_NOT_EQUAL(ip6in4Obj->mechanism, mechanism);

   ip6in4Obj->dynamic = dynamic;

   if ( dynamic == FALSE )
   {
      if (cmsUtl_isValidIpAddress(AF_INET, brAddr) == FALSE || !strcmp(brAddr, "0.0.0.0"))
      {
         cmsLog_error("Invalid border relay IP address");
         return CMSRET_INVALID_PARAM_VALUE;
      }

      if ( (ipv4MaskLen < 0) ||(ipv4MaskLen > 32) )
      {
         cmsLog_error("Invalid IPv4 mask length");
         return CMSRET_INVALID_PARAM_VALUE;
      }

      if (cmsUtl_isValidIpAddress(AF_INET6, ipv6rdPrefix) == FALSE)
      {
         cmsLog_error("Invalid 6rd prefix");
         return CMSRET_INVALID_PARAM_VALUE;
      }

      ip6in4Obj->ipv4MaskLen = ipv4MaskLen;
      REPLACE_STRING_IF_NOT_EQUAL(ip6in4Obj->prefix, ipv6rdPrefix);
      REPLACE_STRING_IF_NOT_EQUAL(ip6in4Obj->borderRelayAddress, brAddr);
   }
   else
   {
      ip6in4Obj->ipv4MaskLen = 0;
      CMSMEM_FREE_BUF_AND_NULL_PTR(ip6in4Obj->prefix);
      CMSMEM_FREE_BUF_AND_NULL_PTR(ip6in4Obj->borderRelayAddress);
   }

   return CMSRET_SUCCESS;
}


CmsRet fillTunnelCfg(const char* tunnelName, const char *mode, const char *wanIntf,
                     const char *lanIntf, UBOOL8 dynamic, IPTunnelObject *tunnelObj)
{
   REPLACE_STRING_IF_NOT_EQUAL(tunnelObj->tunnelName, tunnelName);
   REPLACE_STRING_IF_NOT_EQUAL(tunnelObj->mode, mode);
   REPLACE_STRING_IF_NOT_EQUAL(tunnelObj->associatedWanIfName, wanIntf);
   REPLACE_STRING_IF_NOT_EQUAL(tunnelObj->associatedLanIfName, lanIntf);

   if ( !cmsUtl_strcmp(mode, MDMVS_IPV6INIPV4) )
   {
      if ( !dynamic && dalWan_isWanLayer3ServiceUp(wanIntf, TRUE) )
      {
         tunnelObj->activated = TRUE;
      }
      else
      {
         tunnelObj->activated = FALSE;
      }
   }
   else if ( !cmsUtl_strcmp(mode, MDMVS_IPV4INIPV6) )
   {
      if ( !dynamic && dalWan_isWanLayer3ServiceUp(wanIntf, FALSE) )
      {
         tunnelObj->activated = TRUE;
      }
      else
      {
         tunnelObj->activated = FALSE;

      }
   }

   return CMSRET_SUCCESS;
}


CmsRet dal6in4Tunnel_add_igd(const char* tunnelName, const char *mechanism, const char *wanIntf, const char *lanIntf, 
	                     UBOOL8 dynamic, const char *ipv6rdPrefix, SINT8 ipv4MaskLen, const char *brAddr)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack1;
   IPTunnelObject *tunnelObj = NULL;
   Ipv6inIpv4TunnelObject *ip6in4Obj = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("Adding new 6in4 tunnel: %s/%s/%s/%s/%d", tunnelName, mechanism, wanIntf, lanIntf, dynamic);
   cmsLog_debug("6rdPrefix<%s> brAddr<%s> maskLen<%d>", ipv6rdPrefix, brAddr, ipv4MaskLen);

   if ( (tunnelName == NULL) || (mechanism == NULL) || (wanIntf == NULL) || (lanIntf == NULL) ||
   	  ((dynamic == 0) && ((ipv6rdPrefix == NULL) || (brAddr == NULL)) ) )
   {
      cmsLog_error("Invalid parameters");
      return CMSRET_INVALID_PARAM_VALUE;
   }

   if ((ret = cmsObj_addInstance(MDMOID_IP_TUNNEL, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create MDMOID_IP_TUNNEL object, ret=%d", ret);
      return ret;
   }

   iidStack1 = iidStack;
   if ((ret = cmsObj_addInstance(MDMOID_IPV6IN_IPV4_TUNNEL, &iidStack1)) != CMSRET_SUCCESS)
   {
      CmsRet r2;

      cmsLog_error("could not create MDMOID_IPV6IN_IPV4_TUNNEL object, ret=%d", ret);

      r2 = cmsObj_deleteInstance(MDMOID_IP_TUNNEL, &iidStack);
      if (r2 != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to delete created MDMOID_IP_TUNNEL, r2=%d", r2);
      }
   
      cmsLog_debug("Failed to create 6in4 tunnel and successfully delete created MDMOID_IP_TUNNEL");
   
      return ret;
   }

   /* set Ipv6inIpv4TunnelObject object first */
   if ((ret = cmsObj_get(MDMOID_IPV6IN_IPV4_TUNNEL, &iidStack1, 0, (void **) &ip6in4Obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get Ipv6inIpv4TunnelObject, ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_IPV6IN_IPV4_TUNNEL, &iidStack1);
      cmsObj_deleteInstance(MDMOID_IP_TUNNEL, &iidStack);
      return ret;
   }       

   if ((ret = fill6in4ObjCfg(mechanism, dynamic, ipv6rdPrefix, ipv4MaskLen, brAddr, ip6in4Obj)) != CMSRET_SUCCESS)
   {
      cmsObj_free((void **) &ip6in4Obj);
      cmsObj_deleteInstance(MDMOID_IPV6IN_IPV4_TUNNEL, &iidStack1);
      cmsObj_deleteInstance(MDMOID_IP_TUNNEL, &iidStack);
      return ret;
   }

   if ( (ret = cmsObj_set(ip6in4Obj, &iidStack1)) != CMSRET_SUCCESS )
   {
      cmsLog_error("Failed to set MDMOID_IPV6IN_IPV4_TUNNEL object");
      cmsObj_free((void **) &ip6in4Obj);
      cmsObj_deleteInstance(MDMOID_IPV6IN_IPV4_TUNNEL, &iidStack1);
      cmsObj_deleteInstance(MDMOID_IP_TUNNEL, &iidStack);
      return ret;
   }

   cmsObj_free((void **) &ip6in4Obj);

   /* set tunnel object and activate the tunnel if the associated interface is up. */
   if ((ret = cmsObj_get(MDMOID_IP_TUNNEL, &iidStack, 0, (void **) &tunnelObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get IPTunnelObject, ret=%d", ret);
      return ret;
   }       

   if ((ret = fillTunnelCfg(tunnelName, MDMVS_IPV6INIPV4, wanIntf, lanIntf, dynamic, tunnelObj)) != CMSRET_SUCCESS)
   {
      cmsObj_free((void **) &tunnelObj);
      return ret;
   }

   if ( (ret = cmsObj_set(tunnelObj, &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error("Failed to set MDMOID_IP_TUNNEL object");
   }

   cmsObj_free((void **) &tunnelObj);

   /* 
     * If a dynamic 6rd tunnel is configured and the associated WAN interface is up already, we need to
     * bring down the interface first. Then bring the WAN interface up again so that Dhcpc can send
     * 6rd option.
     */
   if ( dynamic && dalWan_isWanLayer3ServiceUp(wanIntf, TRUE) )
   {
      if ((ret = dalWan_enableDisableWanConnObj(wanIntf, FALSE)) != CMSRET_SUCCESS)
      {
         cmsLog_error("dalWan_enableDisableWanConnObj failed for %s (ret=%d)", wanIntf, ret);
      }

      if ((ret = dalWan_enableDisableWanConnObj(wanIntf, TRUE)) != CMSRET_SUCCESS)
      {
         cmsLog_error("dalWan_enableDisableWanConnObj failed for %s (ret=%d)", wanIntf, ret);
      }
   }

   return ret;

}


CmsRet dalTunnel_delete_igd(const char* tunnelName, const char *mode)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   IPTunnelObject *tunnelObj = NULL;
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 found = FALSE;

   cmsLog_debug("Deleting a tunnel: name<%s> mode<%s>", tunnelName, mode);

   while ((ret = cmsObj_getNextFlags(MDMOID_IP_TUNNEL, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&tunnelObj)) == CMSRET_SUCCESS)
   {
      if( !cmsUtl_strcmp(tunnelName, tunnelObj->tunnelName) && !cmsUtl_strcmp(mode, tunnelObj->mode)) 
      {
         found = TRUE;
         break;
      }
      else
      {
         cmsObj_free((void **) &tunnelObj);
      }
   }

   if(found == FALSE) 
   {
      cmsLog_debug("no tunnel entry found with %s", tunnelName);
      return CMSRET_INVALID_ARGUMENTS;
   }
   else 
   {
      ret = cmsObj_deleteInstance(MDMOID_IP_TUNNEL, &iidStack);
      cmsObj_free((void **) &tunnelObj);

      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to delete MDMOID_IP_TUNNEL, ret = %d", ret);
         return ret;
      }
   }

   return ret;
}


CmsRet dal4in6Tunnel_add_igd(const char* tunnelName, const char *mechanism, const char *wanIntf, const char *lanIntf, 
	                                           UBOOL8 dynamic, const char *remoteIp)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack1;
   IPTunnelObject *tunnelObj = NULL;
   Ipv4inIpv6TunnelObject *ip4in6Obj = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("Adding new 4in6 tunnel: %s/%s/%s/%s/%d/%s", tunnelName, mechanism, wanIntf, lanIntf, dynamic, remoteIp);

   if ( (tunnelName == NULL) || (mechanism == NULL) || (wanIntf == NULL) || (lanIntf == NULL) ||
   	  ((dynamic == 0) && (remoteIp == NULL)) )
   {
      cmsLog_error("Invalid parameters");
      return CMSRET_INVALID_PARAM_VALUE;
   }

   if ((ret = cmsObj_addInstance(MDMOID_IP_TUNNEL, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create MDMOID_IP_TUNNEL object, ret=%d", ret);
      return ret;
   }

   iidStack1 = iidStack;
   if ((ret = cmsObj_addInstance(MDMOID_IPV4IN_IPV6_TUNNEL, &iidStack1)) != CMSRET_SUCCESS)
   {
      CmsRet r2;

      cmsLog_error("could not create MDMOID_IPV4IN_IPV6_TUNNEL object, ret=%d", ret);

      r2 = cmsObj_deleteInstance(MDMOID_IP_TUNNEL, &iidStack);
      if (r2 != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to delete created MDMOID_IP_TUNNEL, r2=%d", r2);
      }
   
      cmsLog_debug("Failed to create 4in6 tunnel and successfully delete created MDMOID_IP_TUNNEL");
   
      return ret;
   }

   /* set Ipv4inIpv6TunnelObject object first */
   if ((ret = cmsObj_get(MDMOID_IPV4IN_IPV6_TUNNEL, &iidStack1, 0, (void **) &ip4in6Obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get Ipv4inIpv6TunnelObject, ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_IPV4IN_IPV6_TUNNEL, &iidStack1);
      cmsObj_deleteInstance(MDMOID_IP_TUNNEL, &iidStack);
      return ret;
   }       

   REPLACE_STRING_IF_NOT_EQUAL(ip4in6Obj->mechanism, mechanism);
   ip4in6Obj->dynamic = dynamic;

   if ( dynamic == FALSE )
   {
      REPLACE_STRING_IF_NOT_EQUAL(ip4in6Obj->remoteIpv6Address, remoteIp);
   }

   if ( (ret = cmsObj_set(ip4in6Obj, &iidStack1)) != CMSRET_SUCCESS )
   {
      cmsLog_error("Failed to set MDMOID_IPV4IN_IPV6_TUNNEL object");
      cmsObj_free((void **) &ip4in6Obj);
      cmsObj_deleteInstance(MDMOID_IPV4IN_IPV6_TUNNEL, &iidStack1);
      cmsObj_deleteInstance(MDMOID_IP_TUNNEL, &iidStack);
      return ret;
   }

   cmsObj_free((void **) &ip4in6Obj);

   /* set tunnel object and activate the tunnel if the associated interface is up. */
   if ((ret = cmsObj_get(MDMOID_IP_TUNNEL, &iidStack, 0, (void **) &tunnelObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get IPTunnelObject, ret=%d", ret);
      return ret;
   }       

   if ((ret = fillTunnelCfg(tunnelName, MDMVS_IPV4INIPV6, wanIntf, lanIntf, dynamic, tunnelObj)) != CMSRET_SUCCESS)
   {
      cmsObj_free((void **) &tunnelObj);
      return ret;
   }

   if ( (ret = cmsObj_set(tunnelObj, &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error("Failed to set MDMOID_IP_TUNNEL object");
   }

   cmsObj_free((void **) &tunnelObj);

   /* 
     * If a dynamic ds-lite tunnel is configured and the associated WAN interface is up already, we need to
     * bring down the interface first. Then bring the WAN interface up again so that Dhcp6c can send
     * aftr name option 64.
     */
   if ( dynamic && dalWan_isWanLayer3ServiceUp(wanIntf, FALSE) )
   {
      if ((ret = dalWan_enableDisableWanConnObj(wanIntf, FALSE)) != CMSRET_SUCCESS)
      {
         cmsLog_error("dalWan_enableDisableWanConnObj failed for %s (ret=%d)", wanIntf, ret);
      }

      if ((ret = dalWan_enableDisableWanConnObj(wanIntf, TRUE)) != CMSRET_SUCCESS)
      {
         cmsLog_error("dalWan_enableDisableWanConnObj failed for %s (ret=%d)", wanIntf, ret);
      }
   }

   return ret;

}

#endif

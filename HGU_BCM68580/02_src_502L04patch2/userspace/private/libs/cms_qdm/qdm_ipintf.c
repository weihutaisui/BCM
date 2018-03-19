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
 
#ifdef DMP_BASELINE_1  /* this file touches TR98 objects */


#include "cms.h"
#include "cms_mdm.h"
#include "cms_obj.h"
#include "cms_phl.h"
#include "cms_qdm.h"
#include "cms_util.h"
#include "cms_core.h"



/* This is a QDM private utility function for QDM internal use only! */
/* Caller is responsible for freeing the objects passed back */
CmsRet qdmPrivate_getLanWanConnObj(const char *intfName,
                                 WanIpConnObject **wanIpConnObj,
                                 WanPppConnObject **wanPppConnObj,
                                 LanIpIntfObject **lanIpIntfObj,
                                 IPv6LanIntfAddrObject **ipv6LanIpAddrObj)
{
   char *fullPath=NULL;
   MdmPathDescriptor pathDesc = EMPTY_PATH_DESCRIPTOR;
   UBOOL8 layer2=FALSE;
   CmsRet ret;

   cmsLog_debug("Entered: intfName=%s", intfName);

   /* go from intfName to fullPath (this function does not handle brx) */
   ret = qdmIntf_intfnameToFullPathLocked(intfName, layer2, &fullPath);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not convert intfName %s to fullPath, ret=%d",
                   intfName, ret);
      return ret;
   }

   cmsLog_debug("%s => fullpath %s", intfName, fullPath);

   /* convert fullpath to pathDesc */
   ret = cmsMdm_fullPathToPathDescriptor(fullPath, &pathDesc);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not convert fullpath %s to pathDesc, ret=%d",
                   fullPath, ret);
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(fullPath);

   if (ret != CMSRET_SUCCESS)
   {
      return ret;
   }


   if (pathDesc.oid == MDMOID_WAN_IP_CONN)
   {
      if (wanIpConnObj)
      {
         ret = cmsObj_get(pathDesc.oid, &pathDesc.iidStack, 0,
                                                  (void **) wanIpConnObj);
      }
      else
      {
         cmsLog_error("Need to get WanIpConnObj, but NULL ptr");
         ret = CMSRET_INTERNAL_ERROR;
      }
   }
   else if (pathDesc.oid == MDMOID_WAN_PPP_CONN)
   {
      if (wanPppConnObj)
      {
         ret = cmsObj_get(pathDesc.oid, &pathDesc.iidStack, 0,
                                                  (void **) wanPppConnObj);
      }
      else
      {
         cmsLog_error("Need to get WanPppConnObj, but NULL ptr");
         ret = CMSRET_INTERNAL_ERROR;
      }
   }
   else if (pathDesc.oid == MDMOID_LAN_IP_INTF)
   {
      if (lanIpIntfObj)
      {
         ret = cmsObj_get(pathDesc.oid, &pathDesc.iidStack, 0,
                                                  (void **) lanIpIntfObj);
      }

#ifdef DMP_X_BROADCOM_COM_IPV6_1
      if (ipv6LanIpAddrObj && ret == CMSRET_SUCCESS)
      {
        /*
         * Broadcom Proprietary IPv6:
         * The IPv6 IP Address object is at the same level as Lan IPInterface,
         * so I can use the same iidStack to get the object.
         */
        ret = cmsObj_get(MDMOID_I_PV6_LAN_INTF_ADDR, &pathDesc.iidStack, 0,
                                            (void **) &ipv6LanIpAddrObj);
        if (ret != CMSRET_SUCCESS)
        {
           /* if there is an error on the IPv6 obj, we have to free the
            * IPv4 obj since caller will see the error and not free anything.
            */
           if (lanIpIntfObj && *lanIpIntfObj)
           {
              cmsObj_free((void **) lanIpIntfObj);
           }
        }
      }
#endif

      if (lanIpIntfObj == NULL && ipv6LanIpAddrObj == NULL)
      {
         cmsLog_error("Need to get LanIpIntfObj or ipv6LanIpAddrObj, but both ptrs NULL");
         ret = CMSRET_INTERNAL_ERROR;
      }
   }
   else
   {
      cmsLog_error("Unexpected oid %d (expected %d or %d)",
            pathDesc.oid, MDMOID_WAN_IP_CONN, MDMOID_WAN_PPP_CONN);
      ret = CMSRET_INVALID_ARGUMENTS;
   }

   cmsLog_debug("ret=%d OID=%d", ret, pathDesc.oid);

   return ret;
}


/* in rut_wan.c */
extern UBOOL8 rut_isWanInterfaceUp(const char *ifName, UBOOL8 isIPv4);

UBOOL8 qdmIpIntf_isWanInterfaceUpLocked_igd(const char *ifName, UBOOL8 isIPv4)
{
   return (rut_isWanInterfaceUp(ifName, isIPv4));
}


CmsRet qdmIpIntf_getIpv4AddressByNameLocked_igd(const char *ipIntfName,
                                                char *ipv4Address)
{
   return (qdmIpIntf_getIpvxAddressByNameLocked_igd(CMS_AF_SELECT_IPV4,
                                                    ipIntfName, ipv4Address));
}

CmsRet qdmIpIntf_getIpv6AddressByNameLocked_igd(const char *ipIntfName,
                                                char *ipv6Address)
{
   return (qdmIpIntf_getIpvxAddressByNameLocked_igd(CMS_AF_SELECT_IPV6,
                                                    ipIntfName, ipv6Address));
}


CmsRet qdmIpIntf_getIpvxAddressByNameLocked_igd(UINT32 ipvx,
                                                const char *ipIntfName,
                                                char *ipAddress)
{
   WanIpConnObject *ipConnObj=NULL;
   WanPppConnObject *pppConnObj=NULL;
   LanIpIntfObject *ipIntfObj=NULL;
   IPv6LanIntfAddrObject *ipv6LanIpAddrObj=NULL;
   CmsRet ret;
   UBOOL8 found=FALSE;

   if (ipAddress == NULL)
   {
      cmsLog_error("ipAddress is NULL!");
      return CMSRET_INVALID_ARGUMENTS;
   }
   ipAddress[0] = '\0';


#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
   /*
    * In Hybrid mode, the ifName could be a connected IPv6.  So we
    * need to check on _dev2 side first.
    */
   if (ipvx & CMS_AF_SELECT_IPV6)
   {
      ret = qdmIpIntf_getIpvxAddressByNameLocked_dev2(CMS_AF_SELECT_IPV6,
                                                      ipIntfName, ipAddress);
      if (ret == CMSRET_SUCCESS)
      {
         /* found IPv6 address, just return now */
         return ret;
      }
   }
#endif

   /* Look in TR98 objects */
   ret = qdmPrivate_getLanWanConnObj(ipIntfName,
                                     &ipConnObj, &pppConnObj,
                                     &ipIntfObj, &ipv6LanIpAddrObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not find LAN or WAN conn object for %s", ipIntfName);
      return ret;
   }

   if (pppConnObj)
   {
      /* Always try to use IPv6 address if IPv6 is available at that WAN interface */
#ifdef DMP_X_BROADCOM_COM_IPV6_1
      if ((ipvx & CMS_AF_SELECT_IPV6) &&
          !cmsUtl_strcmp(pppConnObj->X_BROADCOM_COM_IPv6ConnStatus, MDMVS_CONNECTED) &&
          !IS_EMPTY_STRING(pppConnObj->X_BROADCOM_COM_ExternalIPv6Address))
      {
         strcat(ipAddress, pppConnObj->X_BROADCOM_COM_ExternalIPv6Address);
         found = TRUE;
      }
#endif

      if (!found &&
          (ipvx & CMS_AF_SELECT_IPV4) &&
          !cmsUtl_strcmp(pppConnObj->connectionStatus, MDMVS_CONNECTED))
      {
         strcat(ipAddress, pppConnObj->externalIPAddress);
         found = TRUE;
      }
   }

   if (ipConnObj)
   {
      /* Always try to use IPv6 address if IPv6 is available at that WAN interface */
#ifdef DMP_X_BROADCOM_COM_IPV6_1
      if ((ipvx & CMS_AF_SELECT_IPV6) &&
          !cmsUtl_strcmp(ipConnObj->X_BROADCOM_COM_IPv6ConnStatus, MDMVS_CONNECTED) &&
          !IS_EMPTY_STRING(ipConnObj->X_BROADCOM_COM_ExternalIPv6Address))
      {
         strcat(ipAddress, ipConnObj->X_BROADCOM_COM_ExternalIPv6Address);
         found = TRUE;
      }
#endif

      if (!found &&
          (ipvx & CMS_AF_SELECT_IPV4) &&
          !cmsUtl_strcmp(ipConnObj->connectionStatus, MDMVS_CONNECTED))
      {
         strcat(ipAddress, ipConnObj->externalIPAddress);
         found = TRUE;
      }
   }

   if (ipIntfObj)
   {
      /* Always try to use IPv6 address if IPv6 is available at that WAN interface */
#ifdef DMP_X_BROADCOM_COM_IPV6_1
      if ((ipvx & CMS_AF_SELECT_IPV6) &&
          ipv6LanIpAddrObj &&
          !cmsUtl_isZeroIpvxAddress(CMS_AF_SELECT_IPVX, ipv6LanIpAddrObj->uniqueLocalAddress))
      {
         strcat(ipAddress, ipv6LanIpAddrObj->uniqueLocalAddress);
         found = TRUE;
      }
#endif

      if (!found &&
          (ipvx & CMS_AF_SELECT_IPV4) &&
          !cmsUtl_isZeroIpvxAddress(CMS_AF_SELECT_IPVX, ipIntfObj->IPInterfaceIPAddress))
      {
         strcat(ipAddress, ipIntfObj->IPInterfaceIPAddress);
         found = TRUE;
      }
   }

   cmsObj_free((void **)&ipConnObj);
   cmsObj_free((void **)&pppConnObj);
   cmsObj_free((void **)&ipIntfObj);
   cmsObj_free((void **)&ipv6LanIpAddrObj);

   if (found == TRUE)
   {
      cmsLog_debug("found ifName=%s, ipAddress=%s", ipIntfName, ipAddress);
      ret = CMSRET_SUCCESS;
   }
   else
   {
      cmsLog_debug("no IP address cannot be found, ifName=%s", ipIntfName);
      ret = CMSRET_OBJECT_NOT_FOUND;
   }

   return ret;
}


UBOOL8 qdmIpIntf_isIntfNameUpstreamLocked_igd(const char *l3IntfName)
{
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   WanPppConnObject *pppConn = NULL;
   WanIpConnObject  *ipConn = NULL;
   UBOOL8 found = FALSE;

   /*
    * the algorithm is similar to rut_isWanInterfaceUp, but we are only
    * interested to find the interface name.  Don't care about status.
    */

 #ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
   /* In Hybrid mode, we have to check IPv6 on _dev2 side */
   if (qdmIpIntf_isIntfNameUpstreamLocked_dev2(l3IntfName))
   {
      return TRUE;
   }
 #endif

   while (!found &&
          (cmsObj_getNextFlags(MDMOID_WAN_IP_CONN, &iidStack, OGF_NO_VALUE_UPDATE, (void **) &ipConn)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(l3IntfName, ipConn->X_BROADCOM_COM_IfName) )
      {
         found = TRUE;
      }
      cmsObj_free((void **) &ipConn);
   }

   INIT_INSTANCE_ID_STACK(&iidStack);
   while (!found &&
          (cmsObj_getNextFlags(MDMOID_WAN_PPP_CONN, &iidStack, OGF_NO_VALUE_UPDATE, (void **) &pppConn)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(l3IntfName, pppConn->X_BROADCOM_COM_IfName) )
      {
         found = TRUE;
      }
      cmsObj_free((void **) &pppConn);
   }

   return found;
}


UBOOL8 qdmIpIntf_isFirewallEnabledOnIntfnameLocked_igd(const char *ifName)
{
   WanIpConnObject *wanIpConnObj = NULL;
   WanPppConnObject *wanPppConnObj = NULL;
   LanIpIntfObject *ipIntfObj = NULL;
   CmsRet ret;
   UBOOL8 fwUp = FALSE;
   UBOOL8 found = FALSE;

   cmsLog_debug("Enter: ifName=%s", ifName);

   /* Check lan and wan side interfaces */
   ret = qdmPrivate_getLanWanConnObj(ifName,
                                     &wanIpConnObj, &wanPppConnObj,
                                     &ipIntfObj, NULL);
   if (ret == CMSRET_SUCCESS)
   {
      found = TRUE;
      if (wanIpConnObj != NULL)
      {
         fwUp = wanIpConnObj->X_BROADCOM_COM_FirewallEnabled;
         cmsObj_free((void **)&wanIpConnObj);
      }
      else if (wanPppConnObj != NULL)
      {
         fwUp = wanPppConnObj->X_BROADCOM_COM_FirewallEnabled;
         cmsObj_free((void **)&wanPppConnObj);
      }
      else if (ipIntfObj != NULL)
      {
         fwUp = ipIntfObj->X_BROADCOM_COM_FirewallEnabled;
         cmsObj_free((void **) &ipIntfObj);
      }
   }

   if (found)
   {
      cmsLog_debug("[%s] Firewall is %s", ifName, fwUp ? "Up" : "Down");
   }
   else
   {
      cmsLog_error("Could not find object for ifName %s", ifName);
   }

   return fwUp;
}


UBOOL8 qdmIpIntf_isFirewallEnabledOnIpIntfFullPathLocked_igd(const char * ipIntfFullPath)
{
   char ifName[CMS_IFNAME_LENGTH];
   if(qdmIntf_fullPathToIntfnameLocked(ipIntfFullPath,ifName)==CMSRET_SUCCESS)
      return qdmIpIntf_isFirewallEnabledOnIntfnameLocked_igd(ifName);

   return FALSE;
}


/* in rut_util.c */
extern UBOOL8 rut_isWanInterfaceNatEnable_igd(const char *ifcName);

UBOOL8 qdmIpIntf_isNatEnabledOnIntfNameLocked_igd(const char *ifName)
{
   return (rut_isWanInterfaceNatEnable_igd(ifName));
}


UBOOL8 qdmIpIntf_isNatEnabledOnIpIntfFullPathLocked_igd(const char *ipIntfFullPath)
{
   char ifName[CMS_IFNAME_LENGTH]={0};
   UBOOL8 enabled=FALSE;
   CmsRet ret;

   ret = qdmIntf_fullPathToIntfnameLocked(ipIntfFullPath,ifName);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("bad fullpath %s, ret=%d", ipIntfFullPath, ret);
   }
   else
   {
      enabled = qdmIpIntf_isNatEnabledOnIntfNameLocked_igd(ifName);
   }

   return enabled;
}


extern UBOOL8 rut_isWanInterfaceBridged(const char *ifname);  /* in rut_util.c */
UBOOL8 qdmIpIntf_isWanInterfaceBridgedLocked_igd(const char *ifname)
{
   return rut_isWanInterfaceBridged(ifname);
}


extern UBOOL8 rutWan_isAllBridgePvcs_igd(void);  /* in rut_wan.c */
UBOOL8 qdmIpIntf_isAllBridgeWanServiceLocked_igd()
{
   return (rutWan_isAllBridgePvcs_igd());
}


extern UBOOL8 rut_isBridgedWanExisted(void);  /* in rut_ebtables.c */
UBOOL8 qdmIpIntf_isBridgedWanExistedLocked_igd(void)
{
   return rut_isBridgedWanExisted();
}


extern UBOOL8 rut_isRoutedWanExisted(void);  /* in rut_ebtables.c */
UBOOL8 qdmIpIntf_isRoutedWanExistedLocked_igd(void)
{
   return rut_isRoutedWanExisted();
}




extern CmsRet rutWl2_getL2IfnameFromL3Ifname(const char *l3Ifname, char *l2Ifname);

CmsRet qdmIpIntf_getLayer2IntfNameByLayer3IntfNameLocked_igd(const char *l3IntfName, char *l2IntfName)
{
   CmsRet ret;

   cmsLog_debug("Entered: l3IntfName=%s", l3IntfName);

   /*
    * The function below only knows about TR98, but surprisingly, due to the
    * way the Hybrid data model is set up, it also works in the Hybrid IPv6
    * case where the L3 intfname comes from the Dev2 IP.Interface object,
    * but the L2 intf is under TR98 WANDevice.
    */
   ret = rutWl2_getL2IfnameFromL3Ifname(l3IntfName, l2IntfName);

   cmsLog_debug("Exit: l3IntfName %s => l2IntfName %s (ret=%d)",
                l3IntfName, l2IntfName, ret);

   return ret;
}




UBOOL8 qdmIpIntf_isIpv6EnabledOnIntfNameLocked_igd(const char *ipIntfName __attribute__((unused)))
{
#ifdef DMP_X_BROADCOM_COM_IPV6_1
   WanIpConnObject *wanIpConnObj = NULL;
   WanPppConnObject  *wanPppConnObj = NULL;
   UBOOL8 enabled=FALSE;
   CmsRet ret;

   ret = qdmPrivate_getLanWanConnObj(ipIntfName,
                                  &wanIpConnObj, &wanPppConnObj, NULL, NULL);
   if (ret == CMSRET_SUCCESS)
   {
      if (wanIpConnObj != NULL)
      {
         enabled = wanIpConnObj->X_BROADCOM_COM_IPv6Enabled;
         cmsObj_free((void **)&wanIpConnObj);
      }
      else if (wanPppConnObj != NULL)
      {
         enabled = wanPppConnObj->X_BROADCOM_COM_IPv6Enabled;
         cmsObj_free((void **)&wanPppConnObj);
      }

      cmsLog_debug("[%s] ipv6Enabled=%d", ipIntfName, enabled);
   }
   else
   {
      cmsLog_error("Could not find object for %s", ipIntfName);
   }

   return enabled;
#else
   return FALSE;
#endif
}


/* 
 * This function is currently used by MAP-T for hybrid data model.
 * So this function only fetch br0 IPv4 address info in TR98
 */
CmsRet qdmIpIntf_getIpv4AddrInfoByNameLocked_igd(const char *ifName, 
                                                  char *ipAddress, 
                                                  char *subnetMask, 
                                                  UBOOL8 *isWan,
                                                  UBOOL8 *isStatic)
{
   WanIpConnObject *ipConnObj=NULL;
   WanPppConnObject *pppConnObj=NULL;
   LanIpIntfObject *ipIntfObj=NULL;
   IPv6LanIntfAddrObject *ipv6LanIpAddrObj=NULL;
   CmsRet ret;

   cmsLog_debug("ifName=%s", ifName);

   if (ipAddress == NULL || subnetMask == NULL)
   {
      cmsLog_error("ipAddress/subnetMask is NULL!");
      return CMSRET_INVALID_ARGUMENTS;
   }
   ipAddress[0] = subnetMask[0] = '\0';

   /* Look in TR98 objects */
   ret = qdmPrivate_getLanWanConnObj(ifName,
                                     &ipConnObj, &pppConnObj,
                                     &ipIntfObj, &ipv6LanIpAddrObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not find LAN or WAN conn object for %s", ifName);
      return ret;
   }

   if (ipIntfObj)
   {
      if (!cmsUtl_isZeroIpvxAddress(CMS_AF_SELECT_IPVX, ipIntfObj->IPInterfaceIPAddress))
      {
         cmsUtl_strncpy(ipAddress, ipIntfObj->IPInterfaceIPAddress, CMS_IPADDR_LENGTH);
         cmsUtl_strncpy(subnetMask, ipIntfObj->IPInterfaceSubnetMask, CMS_IPADDR_LENGTH);
      }

      *isWan = FALSE;
      *isStatic = TRUE;
   }
   else
   {
      /* 
       * ifName can only be bridge in this function. Unless this function is
       * called by non-MAP-T purpose.
       */
      ret = CMSRET_INVALID_ARGUMENTS;
   }

   cmsObj_free((void **)&ipConnObj);
   cmsObj_free((void **)&pppConnObj);
   cmsObj_free((void **)&ipIntfObj);
   cmsObj_free((void **)&ipv6LanIpAddrObj);

   return ret;

}
#endif /* DMP_BASELINE_1 */


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
 
#ifdef DMP_DEVICE2_BASELINE_1 /* this file touches TR181 objects */

#include "cms.h"
#include "cms_mdm.h"
#include "cms_obj.h"
#include "cms_phl.h"
#include "cms_qdm.h"
#include "cms_util.h"
#include "cms_core.h"


/* private helper function for use within QDM only!! */
/* Caller is responsible for freeing the returned ipIntfObj */
CmsRet qdmPrivate_getIpIntfObj(const char *ifName,
                               InstanceIdStack *iidStack,
                               Dev2IpInterfaceObject **ipIntfObj)
{
   CmsRet ret;

   INIT_INSTANCE_ID_STACK(iidStack);

   while ((ret = cmsObj_getNextFlags(MDMOID_DEV2_IP_INTERFACE,
                                     iidStack, OGF_NO_VALUE_UPDATE,
                                     (void **) ipIntfObj)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp((*ipIntfObj)->name, ifName))
      {
         /* return immediately, don't free object */
         return CMSRET_SUCCESS;
      }

      cmsObj_free((void **) ipIntfObj);
   }

   /* if we reach here, we did not find object */
   return ret;
}


CmsRet qdmIpIntf_getDefaultLanIntfNameLocked_dev2(char *ifName)
{
   CmsRet ret = CMSRET_OBJECT_NOT_FOUND;
   UBOOL8 found = FALSE;
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   while (found == FALSE &&
          (ret = cmsObj_getNext(MDMOID_DEV2_IP_INTERFACE, &iidStack,
                                     (void **)&ipIntfObj)) == CMSRET_SUCCESS)
   {
      if (ipIntfObj->X_BROADCOM_COM_Upstream == FALSE)
      {
         found = TRUE;
         sprintf(ifName, "%s", ipIntfObj->name);
         if (cmsUtl_strcmp(ipIntfObj->name, "br0") ||
             cmsUtl_strcmp(ipIntfObj->X_BROADCOM_COM_GroupName, "Default"))
         {
            cmsLog_error("found unexpected LAN interface %s (group=%s) expected br0 Default",
                      ipIntfObj->name, ipIntfObj->X_BROADCOM_COM_GroupName);
            /* complain, but keep going */
         }
      }
      
      cmsObj_free((void **) &ipIntfObj);
   }

   if (found == FALSE)
   {
      cmsLog_error("Cannot find any available LAN IP interface");
   }

   return ret;
}


CmsRet qdmIpIntf_getIpvxAddressByNameLocked_dev2(UINT32 ipvx,
                                                 const char *ipIntfName,
                                                 char *ipAddress)
{
   CmsRet ret=CMSRET_INVALID_ARGUMENTS;

   if (ipAddress == NULL)
   {
      cmsLog_error("ipAddress is NULL!");
      return CMSRET_INVALID_ARGUMENTS;
   }
   ipAddress[0] = '\0';


#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
   if (ipvx & CMS_AF_SELECT_IPV6)
   {
      /* If caller can handle IPv6 or IPv4, look for IPv6 first. */
      cmsLog_debug("Looking for IPv6 address on %s", ipIntfName);

      /* we trust the user passed in a buffer of the required length */
      ret = qdmIpIntf_getIpv6AddressByNameLocked_dev2(ipIntfName, ipAddress);
      if (ret == CMSRET_SUCCESS)
      {
         cmsLog_debug("Found IPv6 address %s on %s", ipAddress, ipIntfName);
         return ret;
      }
   }
#endif

   if (ipvx & CMS_AF_SELECT_IPV4)
   {
      cmsLog_debug("Looking for IPv4 address on %s", ipIntfName);

      ret = qdmIpIntf_getIpv4AddressByNameLocked_dev2(ipIntfName, ipAddress);
      if (ret == CMSRET_SUCCESS)
      {
         cmsLog_debug("Found IPv4 address %s on %s", ipAddress, ipIntfName);
         return ret;
      }
   }

   return ret;
}


CmsRet qdmIpIntf_getIpv4AddrInfoByNameLocked_dev2(const char *ifName, 
                                                  char *ipAddress, 
                                                  char *subnetMask, 
                                                  UBOOL8 *isWan,
                                                  UBOOL8 *isStatic)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStackChild=EMPTY_INSTANCE_ID_STACK;
   Dev2IpInterfaceObject *ipIntf=NULL;
   UBOOL8 foundIpAdd=FALSE;
   Dev2Ipv4AddressObject *ipv4Addr=NULL;
   CmsRet ret;
   
   if (cmsUtl_strlen(ifName) == 0)
   {
      cmsLog_error("no ipIntfName, just return");
      return CMSRET_INVALID_ARGUMENTS;
   }
   
   cmsLog_debug("Entered: ifName=%s", ifName);


   /* Get the right ipIntfObj and also set iidStack */
   ret = qdmPrivate_getIpIntfObj(ifName, &iidStack, &ipIntf);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_debug("Cannot find IP interface %s", ifName);
      return CMSRET_OBJECT_NOT_FOUND;
   }
   else
   {
      /* This function is called getIpv4AddrInfo, but isWan is not a property
       * of the address.  It is a property of the IP interface itself.
       * Should remove this param.
       */
      if (*isWan)
      {
         *isWan = ipIntf->X_BROADCOM_COM_Upstream;
      }
      cmsObj_free((void **) &ipIntf);
   }


   while (!foundIpAdd && cmsObj_getNextInSubTree(MDMOID_DEV2_IPV4_ADDRESS,
                                                 &iidStack, 
                                                 &iidStackChild,
                                                 (void **) &ipv4Addr) == CMSRET_SUCCESS)
   {
      /* Can be more than 1 enabled ipv4Address ? */
      if (ipv4Addr->enable == TRUE)
      {
         cmsLog_debug("Found IPv4 address %s for %s", ipv4Addr->IPAddress, ifName);
         if (ipAddress)
         {
            cmsUtl_strncpy(ipAddress, ipv4Addr->IPAddress, CMS_IPADDR_LENGTH);
         }
         if (subnetMask)
         {
            cmsUtl_strncpy(subnetMask, ipv4Addr->subnetMask, CMS_IPADDR_LENGTH);
         }
         if (*isStatic)
         {
            *isStatic = !cmsUtl_strcmp(ipv4Addr->addressingType, MDMVS_STATIC);
         }
         foundIpAdd = TRUE;
         ret = CMSRET_SUCCESS;
      }
      cmsObj_free((void **) &ipv4Addr);
   }      

   if (!foundIpAdd)
   {
      cmsLog_debug("Cannot find IP address for IP interface %s", ifName);
      ret =  CMSRET_OBJECT_NOT_FOUND;
   }
   
   cmsLog_debug("Exit. ret %d", ret);

   return ret;
   
}


CmsRet qdmIpIntf_getIpv4AddressByNameLocked_dev2(const char *ipIntfName,
                                                 char *ipv4Address)
{
   char subnetMask[CMS_IPADDR_LENGTH];
   UBOOL8 isStatic;
   UBOOL8 isWan;

   /* For getLanIPv4Info, subnetMask, isWan and isStatic are not needed */
   return(qdmIpIntf_getIpv4AddrInfoByNameLocked_dev2(ipIntfName,
                             ipv4Address, subnetMask,  &isWan, &isStatic));
}

CmsRet qdmIpIntf_getIpv4AddrAndSubnetByNameLocked_dev2(const char *ipIntfName,
                                                       char *ipAddress,
                                                       char *subnetMask)
{
   UBOOL8 isStatic;
   UBOOL8 isWan;

   /* For getLanIPv4Info, subnetMask, isWan and isStatic are not needed */
   return(qdmIpIntf_getIpv4AddrInfoByNameLocked_dev2(ipIntfName, ipAddress, subnetMask,  &isWan, &isStatic));
}


UBOOL8 qdmIpIntf_isStaticWanLocked_dev2(const char *ipIntfName)
{
   UBOOL8 isWan=FALSE;
   UBOOL8 isStatic=FALSE;
   CmsRet ret;

   ret = qdmIpIntf_getIpv4AddrInfoByNameLocked_dev2(ipIntfName, NULL, NULL,
                                                    &isWan, &isStatic);

   cmsLog_debug("ipIntfName=%s : ret=%d isWan=%d isStatic=%d",
                ipIntfName, isWan, isStatic);
   
   if (ret != CMSRET_SUCCESS)
   {
      /* if the getIpv4AddrInfo failed, just assume FALSE */
      return FALSE;
   }

   return (isWan && isStatic);
}




CmsRet qdmIpIntf_getIpvxServiceStatusFromFullPathLocked_dev2(
                                       const char *ipIntfFullPath,
                                       UINT32 ipvx,
                                       char *statusBuf, UINT32 statusBufLen)
{
   MdmPathDescriptor ipIntfPathDesc=EMPTY_PATH_DESCRIPTOR;
   CmsRet ret;

   ret = cmsMdm_fullPathToPathDescriptor(ipIntfFullPath, &ipIntfPathDesc);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_fullPathToPathDescriptor failed for %s, ret=%d",
                   ipIntfFullPath, ret);
   }
   else
   {
      ret = qdmIpIntf_getIpvxServiceStatusFromPathDescLocked_dev2(
                                                    &ipIntfPathDesc,
                                                    ipvx,
                                                    statusBuf, statusBufLen);
   }

   return ret;
}


CmsRet qdmIpIntf_getIpvxServiceStatusFromPathDescLocked_dev2(
                                  const MdmPathDescriptor *ipIntfPathDesc,
                                  UINT32 ipvx,
                                  char *statusBuf, UINT32 statusBufLen)
{
   Dev2IpInterfaceObject *ipIntfObj=NULL;
   char *copyPtr;
   CmsRet ret;

   if ((ipvx != CMS_AF_SELECT_IPV4) && (ipvx != CMS_AF_SELECT_IPV6))
   {
      cmsLog_error("ipvx must be IPV4 or IPV6, got %d", ipvx);
      return CMSRET_INVALID_ARGUMENTS;
   }

   if (ipIntfPathDesc->oid != MDMOID_DEV2_IP_INTERFACE)
   {
      cmsLog_error("pathDesc must point to MDMOID_DEV2_IP_INTERFACE");
      return CMSRET_INVALID_ARGUMENTS;
   }

   ret = cmsObj_get(ipIntfPathDesc->oid, &ipIntfPathDesc->iidStack, 0,
                    (void **) &ipIntfObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get IP.Intf obj, ret=%d", ret);
      return ret;
   }

   if (ipvx == CMS_AF_SELECT_IPV4)
   {
      copyPtr = ipIntfObj->X_BROADCOM_COM_IPv4ServiceStatus;
   }
   else
   {
      copyPtr = ipIntfObj->X_BROADCOM_COM_IPv6ServiceStatus;
   }

   if (cmsUtl_strlen(copyPtr) > (SINT32) (statusBufLen-1))
   {
      cmsLog_error("statusBufLen (%d) too short, need %d bytes for %s",
                   statusBufLen, cmsUtl_strlen(copyPtr)+1, copyPtr);
      ret = CMSRET_INVALID_ARGUMENTS;
   }
   else
   {
      strcpy(statusBuf, copyPtr);
   }

   cmsObj_free((void **)&ipIntfObj);

   return ret;
}


CmsRet qdmIpIntf_getIpv4ServiceStatusFromPathDescLocked_dev2(
                                  const MdmPathDescriptor *ipIntfPathDesc,
                                  char *statusBuf, UINT32 statusBufLen)
{
   return (qdmIpIntf_getIpvxServiceStatusFromPathDescLocked_dev2(ipIntfPathDesc,
                                           CMS_AF_SELECT_IPV4,
                                           statusBuf, statusBufLen));
}

CmsRet qdmIpIntf_getIpv6ServiceStatusFromPathDescLocked_dev2(
                                  const MdmPathDescriptor *ipIntfPathDesc,
                                  char *statusBuf, UINT32 statusBufLen)
{
   return (qdmIpIntf_getIpvxServiceStatusFromPathDescLocked_dev2(ipIntfPathDesc,
                                           CMS_AF_SELECT_IPV6,
                                           statusBuf, statusBufLen));
}

CmsRet qdmIpIntf_getIpv6ServiceStatusFromFullPathLocked_dev2(
                                  const char *fullPath, char *statusBuf, UINT32 statusBufLen)
{
   MdmPathDescriptor pathDesc;
   CmsRet ret;

   INIT_PATH_DESCRIPTOR(&pathDesc);
   ret = cmsMdm_fullPathToPathDescriptor(fullPath, &pathDesc);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_fullPathToPathDescriptor failed for %s, ret=%d",
                              fullPath, ret);
      return ret;
   }

   return (qdmIpIntf_getIpvxServiceStatusFromPathDescLocked_dev2(&pathDesc,
                                           CMS_AF_SELECT_IPV6,
                                           statusBuf, statusBufLen));
}


Dev2IpInterfaceObject * getIpIntfObjByDirection(const char *ifName,
                                                UINT32 dirMask)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2IpInterfaceObject *ipIntfObj=NULL;

   while (cmsObj_getNextFlags(MDMOID_DEV2_IP_INTERFACE, &iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **)&ipIntfObj) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(ipIntfObj->name, ifName))
      {
         /*
          * Once we have the right ifName, we still have to match the
          * direction.  Normally, we don't care about direction, so
          * dirMask = (QDM_IPINTF_DIR_LAN|QDM_IPINTF_DIR_WAN).
          * But in order to be technically correct
          * with the "isWanInterfaceUp" we sometimes have to limit our
          * check to WAN interface only.
          */
         if (((dirMask & QDM_IPINTF_DIR_WAN) && ipIntfObj->X_BROADCOM_COM_Upstream) ||
             ((dirMask & QDM_IPINTF_DIR_LAN) && !ipIntfObj->X_BROADCOM_COM_Upstream))
         {
            /* return the object immediately.  caller will free it */
            return ipIntfObj;
         }
      }
      cmsObj_free((void **)&ipIntfObj);
   }

   return NULL;
}


UBOOL8 qdmIpIntf_isIpvxServiceUpLocked_dev2(const char *ifName,
                                            UINT32 dirMask, UINT8 ipvx)
{
   UBOOL8 isUp=FALSE;

   if (ipvx & CMS_AF_SELECT_IPV6)
   {
      isUp = qdmIpIntf_isIpv6ServiceUpLocked_dev2(ifName, dirMask);
      if (isUp)
         return isUp;
   }

   if (ipvx & CMS_AF_SELECT_IPV4)
   {
      isUp = qdmIpIntf_isIpv4ServiceUpLocked_dev2(ifName, dirMask);
   }

   return isUp;
}

UBOOL8 qdmIpIntf_isIpv4ServiceStartingLocked_dev2(const char *ifName,
                                            UINT32 dirMask)
{
   UBOOL8 isServiceStarting=FALSE;

#ifdef DESKTOP_LINUX

   isServiceStarting = TRUE;

#else
   Dev2IpInterfaceObject *ipIntfObj=NULL;

   ipIntfObj = getIpIntfObjByDirection(ifName, dirMask);
   if (ipIntfObj == NULL)
   {
      cmsLog_error("Could not find IP.Intf obj for ifName=%s (dirMask=0x%x)",
                   ifName, dirMask);
   }
   else
   {
      isServiceStarting = !cmsUtl_strcmp(ipIntfObj->X_BROADCOM_COM_IPv4ServiceStatus,
                                   MDMVS_SERVICESTARTING);
      cmsObj_free((void **) &ipIntfObj);
   }
#endif /* DESKTOP_LINUX */

   cmsLog_debug("ifName=%s (dirMask=0x%x) isServiceStarting=%d",
                 ifName, dirMask, isServiceStarting);

   return isServiceStarting;
}


UBOOL8 qdmIpIntf_isIpv6ServiceStartingLocked_dev2(const char *ifName,
                                            UINT32 dirMask)
{
   UBOOL8 isServiceStarting=FALSE;

#ifdef DESKTOP_LINUX

   isServiceStarting = TRUE;

#else
   Dev2IpInterfaceObject *ipIntfObj=NULL;

   ipIntfObj = getIpIntfObjByDirection(ifName, dirMask);
   if (ipIntfObj == NULL)
   {
      cmsLog_error("Could not find IP.Intf obj for ifName=%s (dirMask=0x%x)",
                   ifName, dirMask);
   }
   else
   {
      isServiceStarting = !cmsUtl_strcmp(ipIntfObj->X_BROADCOM_COM_IPv6ServiceStatus,
                                   MDMVS_SERVICESTARTING);
      cmsObj_free((void **) &ipIntfObj);
   }
#endif /* DESKTOP_LINUX */

   cmsLog_debug("ifName=%s (dirMask=0x%x) isServiceStarting=%d",
                 ifName, dirMask, isServiceStarting);

   return isServiceStarting;
}


UBOOL8 qdmIpIntf_isIpv4ServiceUpLocked_dev2(const char *ifName, UINT32 dirMask)
{
   UBOOL8 isServiceUp=FALSE;

#ifdef DESKTOP_LINUX

   isServiceUp = TRUE;

#else
   Dev2IpInterfaceObject *ipIntfObj=NULL;

   ipIntfObj = getIpIntfObjByDirection(ifName, dirMask);
   if (ipIntfObj == NULL)
   {
      /*
       * This can be called when a WAN service is being deleted, and we
       * are reconfiguring DNS and Routing and other services.  These
       * services look at each interface to see if they are up, but since
       * a delete is in progress, we don't see this interface.  So basically,
       * it is not in SERVICEUP state anymore.
       */
      cmsLog_debug("Could not find IP.Intf obj for ifName=%s (dirMask=0x%x)",
                   ifName, dirMask);
   }
   else
   {
      isServiceUp = !cmsUtl_strcmp(ipIntfObj->X_BROADCOM_COM_IPv4ServiceStatus,
                                   MDMVS_SERVICEUP);
      cmsObj_free((void **) &ipIntfObj);
   }
#endif /* DESKTOP_LINUX */

   cmsLog_debug("ifName=%s (dirMask=0x%x) isServiceUp=%d",
                 ifName, dirMask, isServiceUp);

   return isServiceUp;
}


UBOOL8 qdmIpIntf_isIpv6ServiceUpLocked_dev2(const char *ifName, UINT32 dirMask)
{
   UBOOL8 isServiceUp=FALSE;

#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
#ifdef DESKTOP_LINUX

   isServiceUp = TRUE;

#else
   Dev2IpInterfaceObject *ipIntfObj=NULL;

   ipIntfObj = getIpIntfObjByDirection(ifName, dirMask);
   if (ipIntfObj == NULL)
   {
      /*
       * In Hybrid mode, if WAN connection is created with IPv4 only, there
       * is no IP.Interface object for the IPv6 part, so we will not find
       * an object.  So this is not a really error.  Just return FALSE.
       */
      cmsLog_debug("Could not find IP.Intf obj for ifName=%s (dirMask=0x%x)",
                   ifName, dirMask);
   }
   else
   {
      isServiceUp = !cmsUtl_strcmp(ipIntfObj->X_BROADCOM_COM_IPv6ServiceStatus,
                                   MDMVS_SERVICEUP);
      cmsObj_free((void **) &ipIntfObj);
   }
#endif /* DESKTOP_LINUX */
#endif

   cmsLog_debug("ifName=%s (dirMask=0x%x) isServiceUp=%d",
                 ifName, dirMask, isServiceUp);

   return isServiceUp;
}


UBOOL8 qdmIpIntf_isWanInterfaceUpLocked_dev2(const char *ifName, UBOOL8 isIPv4)
{
   if (isIPv4)
   {
      return (qdmIpIntf_isIpv4ServiceUpLocked_dev2(ifName, QDM_IPINTF_DIR_WAN));
   }
   else
   {
      return (qdmIpIntf_isIpv6ServiceUpLocked_dev2(ifName, QDM_IPINTF_DIR_WAN));
   }
}


UBOOL8 qdmIpIntf_isIntfNameUpstreamLocked_dev2(const char *l3IntfName)
{
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;
   UBOOL8 found = FALSE;
   UBOOL8 isUpstream = FALSE;

   while (found == FALSE &&
          (ret = cmsObj_getNext(MDMOID_DEV2_IP_INTERFACE, &iidStack,
                                (void **)&ipIntfObj)) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strcmp(ipIntfObj->name, l3IntfName) == 0)
      {
         found = TRUE;
         isUpstream = ipIntfObj->X_BROADCOM_COM_Upstream;
      }

      cmsObj_free((void **) &ipIntfObj);
   }

   return isUpstream;
}


UBOOL8 qdmIpIntf_isFirewallEnabledOnIntfnameLocked_dev2(const char *ifName)
{
   MdmPathDescriptor pathDesc;
   Dev2IpInterfaceObject *ipIntfObj=NULL;
   UBOOL8 enabled=FALSE;

   if(qdmIntf_getPathDescFromIntfnameLocked_dev2(ifName,FALSE,&pathDesc)==CMSRET_SUCCESS)
   {
      if(cmsObj_get(pathDesc.oid,&pathDesc.iidStack,OGF_NO_VALUE_UPDATE,(void *)&ipIntfObj)==CMSRET_SUCCESS)
      {
         enabled = ipIntfObj->X_BROADCOM_COM_FirewallEnabled;
         cmsObj_free((void **)&ipIntfObj);
      }
   }

   return enabled;
}


UBOOL8 qdmIpIntf_isFirewallEnabledOnIpIntfFullPathLocked_dev2(const char * ipIntfFullPath)
{
   MdmPathDescriptor pathDesc;
   UBOOL8 firewallOn=FALSE;
   Dev2IpInterfaceObject *ipIntfObj=NULL;
   CmsRet ret;

   INIT_PATH_DESCRIPTOR(&pathDesc);
   ret = cmsMdm_fullPathToPathDescriptor(ipIntfFullPath, &pathDesc);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_fullPathToPathDescriptor failed for %s, ret=%d",
                   ipIntfFullPath, ret);
      return firewallOn;
   }

   if(cmsObj_get(pathDesc.oid, &pathDesc.iidStack, OGF_NO_VALUE_UPDATE,
                           (void *)&ipIntfObj)==CMSRET_SUCCESS)
   {
      firewallOn = ipIntfObj->X_BROADCOM_COM_FirewallEnabled;
      cmsObj_free((void **)&ipIntfObj);
   }

   return firewallOn;
}


UBOOL8 qdmIpIntf_isNatEnabledOnIntfNameLocked_dev2(const char *ifName)
{
   char *ipIntfFullPath=NULL;
   UBOOL8 isLayer2 = FALSE;
   UBOOL8 enabled=FALSE;
   CmsRet ret;

   ret = qdmIntf_intfnameToFullPathLocked_dev2(ifName, isLayer2, &ipIntfFullPath);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("bad ifName %s, ret=%d", ifName, ret);
   }
   else
   {
      enabled = qdmIpIntf_isNatEnabledOnIpIntfFullPathLocked_dev2(ipIntfFullPath);
      CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);
   }

   return enabled;
}


UBOOL8 qdmIpIntf_isNatEnabledOnIpIntfFullPathLocked_dev2(const char *ipIntfFullPath)
{
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   Dev2NatIntfSettingObject *natIntfObj=NULL;
   UBOOL8 found=FALSE,NatEnabled=FALSE;;

   while(!found &&
         cmsObj_getNextFlags(MDMOID_DEV2_NAT_INTF_SETTING,
                             &iidStack, OGF_NORMAL_UPDATE,
                             (void **) &natIntfObj) == CMSRET_SUCCESS)
   {
       if (!cmsUtl_strcmp(natIntfObj->interface, ipIntfFullPath))
       {
           found = TRUE;
           NatEnabled = natIntfObj->enable;
       }
       cmsObj_free((void **)&natIntfObj);
   }

   cmsLog_debug("Exit found %d", found);

   return NatEnabled;
}


UBOOL8 qdmIpIntf_isFullConeNatEnabledOnIntfNameLocked_dev2(const char *ifName)
{
   char *ipIntfFullPath=NULL;
   UBOOL8 isLayer2 = FALSE;
   UBOOL8 enabled=FALSE;
   CmsRet ret;

   ret = qdmIntf_intfnameToFullPathLocked_dev2(ifName, isLayer2, &ipIntfFullPath);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("bad ifName %s, ret=%d", ifName, ret);
   }
   else
   {
      enabled = qdmIpIntf_isFullConeNatEnabledOnIpIntfFullPathLocked_dev2(ipIntfFullPath);
      CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);
   }

   return enabled;
}


UBOOL8 qdmIpIntf_isFullConeNatEnabledOnIpIntfFullPathLocked_dev2(const char *ipIntfFullPath)
{
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   Dev2NatIntfSettingObject *natIntfObj=NULL;
   UBOOL8 found=FALSE,FullConeNatEnabled=FALSE;;


   while(!found &&
         cmsObj_getNextFlags(MDMOID_DEV2_NAT_INTF_SETTING,
                             &iidStack, OGF_NO_VALUE_UPDATE,
                             (void **) &natIntfObj) == CMSRET_SUCCESS)
   {
       if (!cmsUtl_strcmp(natIntfObj->interface, ipIntfFullPath))
       {
           found = TRUE;
           FullConeNatEnabled = natIntfObj->X_BROADCOM_COM_FullconeNATEnabled;
       }
       cmsObj_free((void **)&natIntfObj);
   }

   cmsLog_debug("Exit found %d", found);

   return FullConeNatEnabled;
}


UBOOL8 qdmIpIntf_isWanInterfaceBridgedLocked_dev2(const char *intfName)
{
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 isBridge=FALSE;
   UBOOL8 found=FALSE;

   while (!found &&
          (cmsObj_getNextFlags(MDMOID_DEV2_IP_INTERFACE, &iidStack,
                               OGF_NO_VALUE_UPDATE,
                               (void **)&ipIntfObj) == CMSRET_SUCCESS))
   {
      if (!cmsUtl_strcmp(ipIntfObj->name, intfName))
      {
         found = TRUE;
         isBridge = ipIntfObj->X_BROADCOM_COM_BridgeService;
      }

      cmsObj_free((void **)&ipIntfObj);      

   }

   /* If not found, need to check for pppoe passthru where the layer 2 interface  
   * isBridge should be also set to TRUE.
   */
   if (!found)
   {
      Dev2PppInterfaceObject *pppIntfObj = NULL;
      CmsRet ret = CMSRET_SUCCESS;
      
      INIT_INSTANCE_ID_STACK(&iidStack);

      /* Look thru  all pppoe interfaces and check for the lowerlayer matches with
      * the layer 2 interface (vlantermation here).  If found, get the pppoe object to 
      * check for X_BROADCOM_COM_AddPppToBridge
      */
      while (!found &&
             (cmsObj_getNextFlags(MDMOID_DEV2_PPP_INTERFACE, &iidStack,
                                  OGF_NO_VALUE_UPDATE,
                                  (void **)&pppIntfObj) == CMSRET_SUCCESS))
      {
         char ifName[CMS_IFNAME_LENGTH];
         
         if ((ret = qdmIntf_fullPathToIntfnameLocked_dev2(pppIntfObj->lowerLayers, ifName)) != CMSRET_SUCCESS)
         {
            cmsLog_error("qdmIntf_fullPathToIntfnameLocked_dev2 failed, error=%d", ret);
         }
         else
         {
            if (!cmsUtl_strcmp(ifName, intfName))
            {
               Dev2PppInterfacePpoeObject *pppoeObj = NULL;
               InstanceIdStack pppoeIidStack = EMPTY_INSTANCE_ID_STACK;
                     
               if ((ret = cmsObj_getNextInSubTree(MDMOID_DEV2_PPP_INTERFACE_PPOE, 
                                                      &iidStack, 
                                                      &pppoeIidStack,
                                                      (void **) &pppoeObj)) != CMSRET_SUCCESS)
               {
                  cmsLog_error("Failed to get pppoeObj, error=%d", ret);
               }
               else
               {
                  if (pppoeObj->X_BROADCOM_COM_AddPppToBridge)
                  {
                     found = TRUE;
                     isBridge = TRUE;
                  }
                  cmsObj_free((void **)&pppoeObj); 
               }               
            }
         }
         cmsObj_free((void **)&pppIntfObj); 
      }
   }
   
   cmsLog_debug("isBridge %d", isBridge);

   return isBridge;
   
}


UBOOL8 qdmIpIntf_isAllBridgeWanServiceLocked_dev2()
{
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 wanServiceFound = FALSE;
   UBOOL8 isAllBridge = TRUE;

   while (cmsObj_getNextFlags(MDMOID_DEV2_IP_INTERFACE, &iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **)&ipIntfObj) == CMSRET_SUCCESS)
   {
      if (ipIntfObj->X_BROADCOM_COM_Upstream)
      {
         wanServiceFound = TRUE;
         if (ipIntfObj->X_BROADCOM_COM_BridgeService == FALSE)
         {
            /* we found a Upstream (WAN) IP.Interface which is not bridge service */
            isAllBridge = FALSE;
         }
      }

      cmsObj_free((void **) &ipIntfObj);
   }

   if (wanServiceFound)
   {
      cmsLog_debug("at least one WAN service found, return %d", isAllBridge);
      return isAllBridge;
   }
   else
   {
      cmsLog_debug("no WAN service found, return FALSE");
      return FALSE;
   }
}


UBOOL8 qdmIpIntf_isBridgedWanExistedLocked_dev2(void)
{
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 bridgeExists=FALSE;

   while (!bridgeExists &&
          (cmsObj_getNextFlags(MDMOID_DEV2_IP_INTERFACE, &iidStack,
                               OGF_NO_VALUE_UPDATE,
                               (void **)&ipIntfObj) == CMSRET_SUCCESS))
   {
      if (ipIntfObj->X_BROADCOM_COM_BridgeService)
      {
         bridgeExists = TRUE;
      }

      cmsObj_free((void **)&ipIntfObj);
   }

   return bridgeExists;
}


UBOOL8 qdmIpIntf_isRoutedWanExistedLocked_dev2(void)
{
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 routedExists=FALSE;

   while (!routedExists &&
          (cmsObj_getNextFlags(MDMOID_DEV2_IP_INTERFACE, &iidStack,
                               OGF_NO_VALUE_UPDATE,
                               (void **)&ipIntfObj) == CMSRET_SUCCESS))
   {
      if (ipIntfObj->X_BROADCOM_COM_Upstream &&
          ipIntfObj->X_BROADCOM_COM_BridgeService == FALSE)
      {
         routedExists = TRUE;
      }

      cmsObj_free((void **)&ipIntfObj);
   }

   return routedExists;
}


UINT32 qdmIpIntf_getNumberOfWanServicesOnLayer2IntfNameLocked_dev2(const char *l2IntfName)
{
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UINT32 count=0;

   cmsLog_debug("Entered: l2IntfName=%s", l2IntfName);

   while ((cmsObj_getNextFlags(MDMOID_DEV2_IP_INTERFACE, &iidStack,
                               OGF_NO_VALUE_UPDATE,
                               (void **)&ipIntfObj) == CMSRET_SUCCESS))
   {
      if (ipIntfObj->X_BROADCOM_COM_Upstream)
      {
         char currL2IntfNameBuf[CMS_IFNAME_LENGTH]={0};
         CmsRet r2;
         r2 = qdmIpIntf_getLayer2IntfNameByLayer3IntfNameLocked_dev2(ipIntfObj->name,
                                                       currL2IntfNameBuf);
         if (r2 != CMSRET_SUCCESS)
         {
            cmsLog_error("Could not get L2IntfName from %s, ret=%d",
                         ipIntfObj->name, r2);
         }
         else
         {
            if (!cmsUtl_strcmp(l2IntfName, currL2IntfNameBuf))
            {
               count++;
            }
         }
      }

      cmsObj_free((void **)&ipIntfObj);
   }

   cmsLog_debug("Exit: l2IntfName=%s WAN Services=%d", l2IntfName, count);

   return count;
}



CmsRet qdmIpIntf_getLayer2IntfNameByLayer3IntfNameLocked_dev2(const char *l3IntfName, char *l2IntfName)
{
   char fullPathBuf[MDM_SINGLE_FULLPATH_BUFLEN]={0};
   CmsRet ret;
   UBOOL8 found=FALSE;

   cmsLog_debug("Entered: l3IntfName=%s", l3IntfName);

   /* convert l3IntfName to fullpath */
   {
      char *fullPath=NULL;
      ret = qdmIntf_intfnameToFullPathLocked(l3IntfName, FALSE, &fullPath);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not get fullpath for %s, ret=%d", l3IntfName, ret);
         return ret;
      }
      else
      {
         cmsUtl_strncpy(fullPathBuf, fullPath, sizeof(fullPathBuf));
         CMSMEM_FREE_BUF_AND_NULL_PTR(fullPath);
      }
   }

   while (!found)
   {
      cmsLog_debug("Check fullpath %s", fullPathBuf);
      if (qdmIntf_isFullPathLayer2Locked_dev2(fullPathBuf))
      {
         found = TRUE;
         ret = qdmIntf_fullPathToIntfnameLocked(fullPathBuf, l2IntfName);
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("Could not convert %s to intfName, ret=%d", fullPathBuf, ret);
            return ret;
         }
      }
      else
      {
         char fullPathBuf2[MDM_SINGLE_FULLPATH_BUFLEN]={0};

         /* current fullpath is not a layer 2 intf, go down to next level */
         ret = qdmIntf_getFirstLowerLayerFromFullPathLocked_dev2(fullPathBuf,
                                          fullPathBuf2, sizeof(fullPathBuf2));
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("Could not get next LowerLayer from %s, ret=%d", fullPathBuf, ret);
            return ret;
         }
         else if (IS_EMPTY_STRING(fullPathBuf2))
         {
            cmsLog_error("Hit end of stack at %s", fullPathBuf);
            return CMSRET_OBJECT_NOT_FOUND;
         }
         else
         {
            /* copy results to fullPathBuf and go to the top of the while
             * loop to check whether we hit a layer 2 interface
             */
            cmsUtl_strcpy(fullPathBuf, fullPathBuf2);
         }
      }
   }

   cmsLog_debug("Exit: l3IntfName %s => l2IntfName=%s (ret=%d)",
                l3IntfName, l2IntfName, ret);

   return ret;
}


CmsRet qdmIpIntf_getBridgeIntfNameByGroupNameLocked_dev2(const char *groupName,
                                                   char *intfName)
{
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found=FALSE;
   CmsRet ret;

   while (!found &&
          (ret = cmsObj_getNextFlags(MDMOID_DEV2_IP_INTERFACE, &iidStack,
                               OGF_NO_VALUE_UPDATE,
                               (void **)&ipIntfObj)) == CMSRET_SUCCESS)
   {
      if (!ipIntfObj->X_BROADCOM_COM_Upstream &&
          !cmsUtl_strcmp(ipIntfObj->X_BROADCOM_COM_GroupName, groupName))
      {
         found = TRUE;
         strcpy(intfName, ipIntfObj->name);
      }

      cmsObj_free((void **)&ipIntfObj);
   }

   return ret;
}


UBOOL8 qdmIpIntf_getWanIntfNameByGroupNameLocked_dev2(const char *groupName, char *intfName)
{
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found=FALSE;
   CmsRet ret;

   while (!found &&
          (ret = cmsObj_getNextFlags(MDMOID_DEV2_IP_INTERFACE, &iidStack,
                               OGF_NO_VALUE_UPDATE,
                               (void **)&ipIntfObj)) == CMSRET_SUCCESS)
   {
      if (ipIntfObj->X_BROADCOM_COM_Upstream &&
          !cmsUtl_strcmp(ipIntfObj->X_BROADCOM_COM_GroupName, groupName))
      {
         found = TRUE;
         if (intfName)		 
         {
            cmsUtl_strcpy(intfName, ipIntfObj->name);
         }
      }

      cmsObj_free((void **)&ipIntfObj);
   }

   return found;
}


UBOOL8 qdmIpIntf_isIpv6EnabledOnIntfNameLocked_dev2(const char *ipIntfName __attribute__((unused)))
{
#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2IpInterfaceObject *ipIntfObj=NULL;
   UBOOL8 isIpv6Enabled=FALSE;
   CmsRet ret;

   if (cmsUtl_strlen(ipIntfName) == 0)
   {
      cmsLog_error("no ipIntfName, just return");
      return FALSE;
   }

   /* Get the right ipIntfObj and also set iidStack */
   ret = qdmPrivate_getIpIntfObj(ipIntfName, &iidStack, &ipIntfObj);
   if (ret != CMSRET_SUCCESS)
   {
      /*
       * In Hybrid mode, we might set up an IPv4 only WAN connection, so
       * no TR181 IP.Interface is created.  So we will not find an
       * IP.Interface object, but that is OK.  That just means IPv6 is not
       * enabled.
       */
      cmsLog_debug("Cannot find IP.Interface for %s", ipIntfName);
   }
   else
   {
      isIpv6Enabled = ipIntfObj->IPv6Enable;
      cmsObj_free((void **) &ipIntfObj);
   }

   return isIpv6Enabled;
#else
   return FALSE;
#endif
}


CmsRet qdmIpIntf_getIpv6AddrInfoByNameLocked_dev2(const char *ifName, 
                                                  char *ipAddress, 
                                                  UINT32 ipAdressLen,
                                                  char *origin,
                                                  UINT32 originLen,
                                                  char *prefixPath,
                                                  UINT32 prefixPathLen,
                                                  UBOOL8 *isWan,
                                                  CmsIpv6OriginFilterEnum wanOriginFilter __attribute((unused)),
                                                  CmsIpv6OriginFilterEnum lanOriginFilter)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStackChild=EMPTY_INSTANCE_ID_STACK;
   Dev2IpInterfaceObject *ipIntf=NULL;
   UBOOL8 foundIpAdd=FALSE;
   Dev2Ipv6AddressObject *ipv6Addr=NULL;
   CmsRet ret=CMSRET_INTERNAL_ERROR;
   
   if (cmsUtl_strlen(ifName) == 0)
   {
      cmsLog_error("no ipIntfName, just return");
      return CMSRET_INVALID_ARGUMENTS;
   }
   
   cmsLog_debug("Looking for IPv6 address info of %s", ifName);

   /* Get the right ipIntfObj and also set iidStack */
   ret = qdmPrivate_getIpIntfObj(ifName, &iidStack, &ipIntf);
   if (ret != CMSRET_SUCCESS)
   {
      /*
       * In Hybrid mode, if WAN connection is created with IPv4 only, there
       * is no IP.Interface object for the IPv6 part, so we will not find
       * an object.  So this is not a really error.
       */
      cmsLog_debug("Cannot find IP.Interface obj for %s", ifName);
      return CMSRET_OBJECT_NOT_FOUND;
   }
   else
   {
      *isWan = ipIntf->X_BROADCOM_COM_Upstream;
       cmsObj_free((void **) &ipIntf);
   }

   while (!foundIpAdd && cmsObj_getNextInSubTree(MDMOID_DEV2_IPV6_ADDRESS,
                                                 &iidStack, 
                                                 &iidStackChild,
                                                 (void **) &ipv6Addr) == CMSRET_SUCCESS)
   {
      UBOOL8 match = FALSE;

      /* FIXME: Can be more than 1 enabled ipv6Address ? */
      if (ipv6Addr->enable == TRUE)
      {
         if (*isWan == TRUE) //FIXME: So far, no wanOriginFilter is supported.
         {
            match = TRUE;
         }
         else if ((lanOriginFilter == CMS_IPV6_ORIGIN_ANY) ||
                  ((lanOriginFilter == CMS_IPV6_ORIGIN_STATIC) && !cmsUtl_strcmp(ipv6Addr->origin, MDMVS_STATIC)) ||
                  ((lanOriginFilter == CMS_IPV6_ORIGIN_AUTOCONFIG) && !cmsUtl_strcmp(ipv6Addr->origin, MDMVS_AUTOCONFIGURED)) ||
                  ((lanOriginFilter == CMS_IPV6_ORIGIN_DHCPV6) && !cmsUtl_strcmp(ipv6Addr->origin, MDMVS_DHCPV6))
                 )
         {
            match = TRUE;
         }
      }

      if (match)
      {
         cmsLog_debug("Found IPv6 address %s for %s", ipv6Addr->IPAddress, ifName);
         if (ipAddress)
         {
            cmsUtl_strncpy(ipAddress, ipv6Addr->IPAddress, ipAdressLen);
            cmsUtl_strncpy(origin, ipv6Addr->origin, originLen);
            cmsUtl_strncpy(prefixPath, ipv6Addr->prefix, prefixPathLen);
         }
         foundIpAdd = TRUE;
         ret = CMSRET_SUCCESS;
      }
      cmsObj_free((void **) &ipv6Addr);
   }      

   if (!foundIpAdd)
   {
      /*
       * If IPv6 address has not been statically configured and no WAN
       * prefix delegation has occurred, then there is no IPv6Address object.
       * Do not complain so loudly.  Return error code and let caller decide
       * what to do.
       */
      cmsLog_debug("Cannot find IPv6 address info for IP interface %s", ifName);
      ret =  CMSRET_OBJECT_NOT_FOUND;
   }
   
   cmsLog_debug("Exit. ret %d", ret);

   return ret;
}


CmsRet qdmIpIntf_getIpv6AddressByNameLocked_dev2(const char *ifName __attribute__((unused)),
                                  char *ipAddress __attribute__((unused)))
{
#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
   char ipv6AddrStrBuf[CMS_IPADDR_LENGTH]={0};
   char origin[BUFLEN_32];
   char prefixPath[BUFLEN_128];
   UBOOL8 isWan;
   CmsRet ret;
   
   cmsLog_debug("Enter: ifName=%s", ifName);

   if (ipAddress == NULL)
   {
      cmsLog_error("Must supply ipAddress buffer");
      return CMSRET_INVALID_ARGUMENTS;
   }
   ipAddress[0] = '\0';
   prefixPath[0] = '\0';

   ret = qdmIpIntf_getIpv6AddrInfoByNameLocked_dev2(ifName,
                            ipv6AddrStrBuf, sizeof(ipv6AddrStrBuf),
                            origin, sizeof(origin), 
                            prefixPath, sizeof(prefixPath), &isWan,
                            CMS_IPV6_ORIGIN_ANY, CMS_IPV6_ORIGIN_STATIC);

   if (ret == CMSRET_SUCCESS)
   {
      /* trusting the user to have allocated enough space in the buffer */
      /* Be consistent with legacy TR98, always give address and prefix length. */
      if (!IS_EMPTY_STRING(prefixPath))
      {
         MdmPathDescriptor pathDesc;

         INIT_PATH_DESCRIPTOR(&pathDesc);
         ret = cmsMdm_fullPathToPathDescriptor(prefixPath, &pathDesc);
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsMdm_fullPathToPathDescriptor failed for %s, ret=%d",
                                 prefixPath, ret);
         }
         else
         {
            Dev2Ipv6PrefixObject *ipv6Prefix = NULL;

            ret = cmsObj_get(pathDesc.oid, &pathDesc.iidStack, 0,
                    (void **) &ipv6Prefix);
            if (ret != CMSRET_SUCCESS)
            {
               cmsLog_error("Could not get IP.Intf.IPv6Prefix obj, ret=%d", ret);
            }
            else
            {
               char *ptr = strchr(ipv6Prefix->prefix, '/');

               sprintf(ipAddress, "%s%s", ipv6AddrStrBuf, ptr);
            }

            cmsObj_free((void **) &ipv6Prefix);
         }
      }
      else
      {
         sprintf(ipAddress, "%s/128", ipv6AddrStrBuf);
      }

      cmsLog_debug("ifName %s => ipv6Addr %s", ifName, ipAddress);
   }
   
   return ret;
#else
   return CMSRET_RESOURCE_NOT_CONFIGURED;
#endif
}


CmsRet qdmIpIntf_getDproxyIpv6AddressByNameLocked_dev2(const char *ifName __attribute__((unused)),
                                  char *ipAddress __attribute__((unused)))
{
#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
   char ipv6AddrStrBuf[CMS_IPADDR_LENGTH]={0};
   char origin[BUFLEN_32];
   char prefixPath[BUFLEN_128];
   UBOOL8 isWan;
   CmsRet ret;
   
   cmsLog_debug("Enter: ifName=%s", ifName);

   if (ipAddress == NULL)
   {
      cmsLog_error("Must supply ipAddress buffer");
      return CMSRET_INVALID_ARGUMENTS;
   }
   ipAddress[0] = '\0';
   prefixPath[0] = '\0';

   ret = qdmIpIntf_getIpv6AddrInfoByNameLocked_dev2(ifName,
                            ipv6AddrStrBuf, sizeof(ipv6AddrStrBuf),
                            origin, sizeof(origin), 
                            prefixPath, sizeof(prefixPath), &isWan,
                            CMS_IPV6_ORIGIN_ANY, CMS_IPV6_ORIGIN_AUTOCONFIG);

   if (ret == CMSRET_SUCCESS)
   {
      if (isWan == FALSE)
      {
         cmsUtl_strcpy(ipAddress, ipv6AddrStrBuf);
         cmsLog_debug("ipv6Addr %s", ipAddress);
      }
      else
      {
         cmsLog_notice("ifName<%s> is WAN intf", ifName);
      }
   }
   
   return ret;
#else
   return CMSRET_RESOURCE_NOT_CONFIGURED;
#endif
}


UBOOL8 qdmIpIntf_findIpv6Prefix(const InstanceIdStack *iidStackIpIntf, const char *prefix,
                                const char *origin, const char *staticType, InstanceIdStack *iidStackIpv6Prefix)
{
   Dev2Ipv6PrefixObject *ipv6PrefixObj=NULL;
   UBOOL8 found = FALSE;
   CmsRet ret;

   cmsLog_debug("prefix/origin/staticT: %s/%s/%s/%s", prefix, origin, staticType, cmsMdm_dumpIidStack(iidStackIpIntf));
   INIT_INSTANCE_ID_STACK(iidStackIpv6Prefix);
   while (!found && 
              (ret = cmsObj_getNextInSubTreeFlags(MDMOID_DEV2_IPV6_PREFIX,
                              iidStackIpIntf, iidStackIpv6Prefix,
                              OGF_NO_VALUE_UPDATE,
                              (void **)&ipv6PrefixObj)) == CMSRET_SUCCESS)
   {
      /*
       * find prefix is different from address because of PD implementation.
       * If it's related to PD, we only match origin and staticType. So prefix argument MUST be NULL in this case.
       */
      if ((!prefix || !cmsUtl_strcmp(ipv6PrefixObj->prefix, prefix)) && 
          (!cmsUtl_strcmp(ipv6PrefixObj->origin, origin) && !cmsUtl_strcmp(ipv6PrefixObj->staticType, staticType)))
      {
         found = TRUE;
      }
      cmsObj_free((void **) &ipv6PrefixObj);
   }

   cmsLog_debug("found<%d>", found);
   return found;
}


UBOOL8 qdmIpIntf_getIpv6DelegatedPrefixByNameLocked_dev2(const char *ifname, char *prefix)
{
   MdmPathDescriptor pathDesc;
   InstanceIdStack iidStack_ipv6Prefix;
   UBOOL8 found = FALSE;

   cmsLog_debug("enter with ifname<%s>", ifname);

   if (prefix)
   {
      prefix[0] = '\0';
   }
   else
   {
      cmsLog_error("prefix is NULL");
      return found;
   }

   INIT_PATH_DESCRIPTOR(&pathDesc);

   qdmIntf_getPathDescFromIntfnameLocked_dev2(ifname, FALSE, &pathDesc);

   if (qdmIpIntf_findIpv6Prefix(&pathDesc.iidStack, NULL, MDMVS_STATIC,
                             MDMVS_CHILD, &iidStack_ipv6Prefix))
   {
      Dev2Ipv6PrefixObject *prefixObj=NULL;

      if (cmsObj_get(MDMOID_DEV2_IPV6_PREFIX, &iidStack_ipv6Prefix, 0, (void **) &prefixObj) != CMSRET_SUCCESS)
      {
         cmsLog_error("cannot get prefixObj");
         return found;
      }

      if (prefixObj->enable && 
          !cmsUtl_strncmp(prefixObj->status, MDMVS_ENABLED, strlen(MDMVS_ENABLED)))
      {
         cmsUtl_strcpy(prefix, prefixObj->prefix);
         found = TRUE;
      }

      cmsObj_free((void **)&prefixObj);
   }

   cmsLog_debug("found<%d> prefix<%s>", found, prefix);
   return found;
}


CmsRet qdmIpIntf_getAssociatedWanIpIntfPathDescByLanFullPathLocked_dev2(const char *lanFullPath __attribute((unused)),
      MdmPathDescriptor *wanIpIntfPathDesc, UINT32 ipvx)
{
   CmsRet ret = CMSRET_SUCCESS;

   INIT_PATH_DESCRIPTOR(wanIpIntfPathDesc);

   if (ipvx & CMS_AF_SELECT_IPV4)
   {
      cmsLog_debug("not implement yet");
      return ret;
   }

#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
   if (ipvx & CMS_AF_SELECT_IPV6)
   {
      MdmPathDescriptor lanIpIntfPathDesc=EMPTY_PATH_DESCRIPTOR;

      /*
       * 1. Get LAN IP.Interface
       * 2. Get child prefix's parent prefix
       * 3. Get associated WAN IP.Interface
       */
      ret = cmsMdm_fullPathToPathDescriptor(lanFullPath, &lanIpIntfPathDesc);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsMdm_fullPathToPathDescriptor failed for %s, ret=%d",
                      lanFullPath, ret);
      }
      else
      {
         InstanceIdStack iidStack_ipv6Prefix;

         if (!qdmIpIntf_findIpv6Prefix(&lanIpIntfPathDesc.iidStack, NULL, MDMVS_STATIC,
                                 MDMVS_CHILD, &iidStack_ipv6Prefix))
         {
            cmsLog_notice("no child prefix found");
         }
         else
         {
            Dev2Ipv6PrefixObject *childPrefix=NULL;

            if ((ret = cmsObj_get(MDMOID_DEV2_IPV6_PREFIX, &iidStack_ipv6Prefix, 0, (void **) &childPrefix)) != CMSRET_SUCCESS)
            {
               cmsLog_error("Failed to get childPrefix, ret = %d", ret);
            }
            else
            {
               MdmPathDescriptor parentPrefixfPathDesc=EMPTY_PATH_DESCRIPTOR;
               void *wanIpintf = NULL;
               InstanceIdStack iidStack_wanIpIntf;

               if ((ret = cmsMdm_fullPathToPathDescriptor(childPrefix->parentPrefix, &parentPrefixfPathDesc)) != CMSRET_SUCCESS)
               {
                  cmsLog_error("fail to convert parentPrefix");
                  cmsObj_free((void **)&childPrefix);
                  return ret;
               }
               cmsLog_debug("parent prefix<%s>", childPrefix->parentPrefix);
               cmsObj_free((void **)&childPrefix);

               iidStack_wanIpIntf = parentPrefixfPathDesc.iidStack;
               ret = cmsObj_getAncestor(MDMOID_DEV2_IP_INTERFACE,
                                 MDMOID_DEV2_IPV6_PREFIX,
                                 &iidStack_wanIpIntf,
                                 &wanIpintf);
               if (ret != CMSRET_SUCCESS)
               {
                  cmsLog_error("could not get parent IP.Interface, ret=%d", ret);
               }
               else
               {
                  wanIpIntfPathDesc->oid = MDMOID_DEV2_IP_INTERFACE;
                  wanIpIntfPathDesc->iidStack = iidStack_wanIpIntf;

                  cmsLog_debug("IpIntfIidStack:%s", cmsMdm_dumpIidStack(&wanIpIntfPathDesc->iidStack));
                  cmsObj_free(&wanIpintf);
               }
            }
         }
      }
   }
#endif

   return ret;
}

UBOOL8 qdmIpIntf_isAssociatedWanInterfaceUpLocked_dev2(const char *lanFullPath __attribute((unused)), UINT32 ipvx)
{
   UBOOL8 isUp = FALSE;

   if (ipvx == CMS_AF_SELECT_IPV4)
   {
      cmsLog_debug("not implement yet");
      return isUp;
   }

#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
   if (ipvx == CMS_AF_SELECT_IPV6)
   {
      MdmPathDescriptor wanIpIntfPathDesc;
      CmsRet ret;

      cmsLog_debug("lan<%s>", lanFullPath);
      ret = qdmIpIntf_getAssociatedWanIpIntfPathDescByLanFullPathLocked_dev2(lanFullPath, &wanIpIntfPathDesc, ipvx);

      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("fail with ret<%d>", ret);
      }
      else if (wanIpIntfPathDesc.oid == MDMOID_DEV2_IP_INTERFACE) //found associated WAN
      {
         Dev2IpInterfaceObject *ipIntfObj=NULL;

         if ((ret = cmsObj_get(MDMOID_DEV2_IP_INTERFACE, &wanIpIntfPathDesc.iidStack, 0, (void **) &ipIntfObj)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Failed to get WAN IP.interface, ret = %d", ret);
         }
         else
         {
            cmsLog_debug("ipv6Service<%s>", ipIntfObj->X_BROADCOM_COM_IPv6ServiceStatus);
            isUp = !cmsUtl_strcmp(ipIntfObj->X_BROADCOM_COM_IPv6ServiceStatus,
                                  MDMVS_SERVICEUP);
            cmsObj_free((void **) &ipIntfObj);
         }
      }
   }
#endif

   cmsLog_debug("isUp<%d>", isUp);
   return isUp; 
}


CmsRet qdmIpIntf_getMacFilterByFullPathLocked_dev2(const char *ipIntfFullPath, MdmPathDescriptor *macFiltPathDesc)
{
   int found = FALSE;
   CmsRet ret = CMSRET_INTERNAL_ERROR;
   Dev2MacFilterObject *macFilterObj=NULL;
   InstanceIdStack iidStack;

   if(macFiltPathDesc && ipIntfFullPath)
   {
      cmsLog_debug("entered, ipIntfFullPath=%s", ipIntfFullPath);
      
      INIT_INSTANCE_ID_STACK(&iidStack);
      INIT_PATH_DESCRIPTOR(macFiltPathDesc);      
      
      while (!found && cmsObj_getNext(MDMOID_DEV2_MAC_FILTER, &iidStack, (void **) &macFilterObj) == CMSRET_SUCCESS)
      {
            if (cmsUtl_strcmp(macFilterObj->IPInterface, ipIntfFullPath) == 0 )
            {
                  found=TRUE;
                  ret = CMSRET_SUCCESS;
                  macFiltPathDesc->oid = MDMOID_DEV2_MAC_FILTER;
                  macFiltPathDesc->iidStack = iidStack;
            }
            cmsObj_free((void **)&macFilterObj);
      }
   }
   return ret;
}


#endif /* DMP_DEVICE2_BASELINE_1 */


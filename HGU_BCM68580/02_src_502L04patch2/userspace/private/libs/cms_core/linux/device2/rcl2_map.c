/***********************************************************************
 *
 *  Copyright (c) 2014  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2014:proprietary:standard

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


#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
#ifdef SUPPORT_MAPT

#include "cms_util.h"
#include "cms_qdm.h"

#include "rcl.h"
#include "rut_util.h"
#include "rut2_util.h"
#include <sys/utsname.h>

#define MAP_MODULE_NAME             "ivi"
#define MAP_MODULE_NAME_LEN         32
#define DEFRAGV6_MODULE_NAME             "nf_defrag_ipv6"

static UBOOL8 isMapModuleInserted()
{
   UBOOL8 isInserted=FALSE;
   char buf[BUFLEN_512];
   FILE* fs = fopen("/proc/modules", "r");

   if (fs != NULL)
   {
      while (fgets(buf, BUFLEN_512, fs) != NULL)
      {
          char *p = strtok(buf, " ");
          if ((p != NULL) && !cmsUtl_strcmp(p, "ivi"))
         {
            isInserted = TRUE;
            break;
         }
      }
      fclose(fs);;
   }

   cmsLog_debug("ivi is %s", isInserted ? "in." : "not in.");

   return isInserted;
}

static UBOOL8 isDefragv6ModuleInserted()
{
   UBOOL8 isInserted=FALSE;
   char buf[BUFLEN_512];
   FILE* fs = fopen("/proc/modules", "r");

   if (fs != NULL)
   {
      while (fgets(buf, BUFLEN_512, fs) != NULL)
      {
         char *p = strtok(buf, " ");
         if ((p != NULL) && !cmsUtl_strcmp(p, DEFRAGV6_MODULE_NAME))
         {
            isInserted = TRUE;
            break;
         }
      }
      fclose(fs);;
   }

   cmsLog_debug("%s is %s", DEFRAGV6_MODULE_NAME, isInserted ? "in." : "not in.");

   return isInserted;
}

static void insertDefragv6Module()
{
   char cmdStr[CMS_MAX_FULLPATH_LENGTH];
   struct utsname kernel;

   if (uname(&kernel) == -1)
   {
      cmsLog_error("Failed to get kernel version");
      return;
   }

#ifdef DESKTOP_LINUX
   cmsLog_debug("fake insert of nf_defrag_ipv6 module");
#else

   snprintf(cmdStr, sizeof(cmdStr), "insmod /lib/modules/%s/kernel/net/ipv6/netfilter/%s.ko", kernel.release, DEFRAGV6_MODULE_NAME);
   rut_doSystemAction("insertDefragv6Module", cmdStr);

#endif /* DESKTOP_LINUX */

   return;
}

static void insertMapModule()
{
   char cmdStr[CMS_MAX_FULLPATH_LENGTH];
   struct utsname kernel;

   if (uname(&kernel) == -1)
   {
      cmsLog_error("Failed to get kernel version");
      return;
   }

#ifdef DESKTOP_LINUX
   cmsLog_debug("fake insert of Map module");
#else

   if (access("/dev/ivi", F_OK) == -1)
   {
      snprintf(cmdStr, sizeof(cmdStr), "mknod /dev/ivi c 3015 0");
      rut_doSystemAction("insertMapModule", cmdStr);
   }

   if (!isDefragv6ModuleInserted())
   {
      insertDefragv6Module();
   }

   snprintf(cmdStr, sizeof(cmdStr), "insmod /lib/modules/%s/extra/%s.ko", kernel.release, MAP_MODULE_NAME);
   rut_doSystemAction("insertMapModule", cmdStr);

#endif /* DESKTOP_LINUX */

   return;
}

static CmsRet getDomainInfo(const InstanceIdStack *ruleiidStack, char *brprefix, char *wanintf,
                            UINT32 *psidOffset, UINT32 *psidLen, UINT32 *psid)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack;
   Dev2MapDomainObject *domain = NULL;

   iidStack = *ruleiidStack;
   if (cmsObj_getAncestorFlags(MDMOID_DEV2_MAP_DOMAIN, MDMOID_DEV2_MAP_DOMAIN_RULE,
                               &iidStack, OGF_NO_VALUE_UPDATE,
                               (void **) &domain) != CMSRET_SUCCESS )
   {
      cmsLog_error("Cannot get domain obj");
      return CMSRET_MDM_TREE_ERROR;
   }

   cmsUtl_strncpy(brprefix, domain->BRIPv6Prefix, BUFLEN_32);
   qdmIntf_getIntfnameFromFullPathLocked_dev2(domain->WANInterface, wanintf, CMS_IFNAME_LENGTH);
   *psidOffset = domain->PSIDOffset;
   *psidLen = domain->PSIDLength;
   *psid = domain->PSID;

   cmsObj_free((void **) &domain);

   return ret;
}

static CmsRet launchMapt(const char *brprefix, const char *wanintf,
                         UINT32 psidOffset, UINT32 psidLen, UINT32 psid,
                         const char *IPv6Prefix, const char *IPv4Prefix,
                         UBOOL8 add)
{
   CmsRet ret = CMSRET_SUCCESS;
   char cmdStr[BUFLEN_256];
   UINT32 ratio;

   cmsLog_debug("%s: brprefix<%s> wanintf<%s> BMRv6Prefix<%s>", add?"ADD":"DEL", brprefix, wanintf, IPv6Prefix);

   ratio = (1 << psidLen);

   if ( add )
   {
      char ifNameBuf[CMS_IFNAME_LENGTH]={0};
      char ipv4AddrBuf[CMS_IPADDR_LENGTH]={0};
      char ipv4SubnetMaskBuf[CMS_IPADDR_LENGTH]={0};
      char subnetCidr4[CMS_IPADDR_LENGTH+3]={0};
      UBOOL8 isWan=FALSE;
      UBOOL8 isStatic=FALSE;

      if (!isMapModuleInserted())
      {
         insertMapModule();
      }

      snprintf(cmdStr, sizeof(cmdStr), "ivictl -r -d -P %s -R %d -T", brprefix, ratio);
      rut_doSystemAction("MaptAdd", cmdStr);

      /* fetch LAN subnet information */
      qdmIpIntf_getDefaultLanIntfNameLocked_dev2(ifNameBuf);
      qdmIpIntf_getIpv4AddrInfoByNameLocked(ifNameBuf,
                                         ipv4AddrBuf, ipv4SubnetMaskBuf,
                                         &isWan, &isStatic);

      /* convert ipv4addr and subnet to cidr4 */
      cmsNet_inet_ipv4AddrStrtoCidr4(ipv4AddrBuf, ipv4SubnetMaskBuf, subnetCidr4);
      cmsLog_debug("ifname=%s ipv4AddrBuf=%s ipv4SubnetMaskBuf=%s cidr4=%s",
                   ifNameBuf, ipv4AddrBuf, ipv4SubnetMaskBuf, subnetCidr4);          

      snprintf(cmdStr, sizeof(cmdStr), "ivictl -s -i %s -I %s -P %s -H -N -a %s -A %s -z %d -R %d -o %d -T", 
               ifNameBuf, wanintf, IPv6Prefix, subnetCidr4, IPv4Prefix, psidOffset, ratio, psid);
      rut_doSystemAction("MaptAdd", cmdStr);
   }
   else
   {
      snprintf(cmdStr, sizeof(cmdStr), "ivictl -q");
      rut_doSystemAction("MaptAdd", cmdStr);

      if (isMapModuleInserted())
      {
         snprintf(cmdStr, sizeof(cmdStr), "rmmod %s", MAP_MODULE_NAME);
         rut_doSystemAction("MaptAdd", cmdStr);
      }
   }

   return ret;
}

CmsRet rcl_dev2MapObject( _Dev2MapObject *newObj,
                const _Dev2MapObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   if (newObj && newObj->enable && !isMapModuleInserted())
   {
      insertMapModule();
   }

   return CMSRET_SUCCESS;
}


CmsRet rcl_dev2MapDomainObject( _Dev2MapDomainObject *newObj,
                const _Dev2MapDomainObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   if (newObj && newObj->enable)
   {
      CMSMEM_REPLACE_STRING_FLAGS(newObj->status, MDMVS_ENABLED, mdmLibCtx.allocFlags);
   }
   else if (newObj && !newObj->enable)
   {
      CMSMEM_REPLACE_STRING_FLAGS(newObj->status, MDMVS_DISABLED, mdmLibCtx.allocFlags);
   }

   return CMSRET_SUCCESS;
}


CmsRet rcl_dev2MapDomainRuleObject( _Dev2MapDomainRuleObject *newObj,
                const _Dev2MapDomainRuleObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;
   char brprefix[CMS_IPADDR_LENGTH];
   char wanintf[CMS_IFNAME_LENGTH];
   UINT32 psidOffset, psidLen, psid;

   if ( ADD_NEW(newObj, currObj) )
   {
      return ret;
   }

   if (ENABLE_NEW_OR_ENABLE_EXISTING(newObj, currObj))
   {
      /*
       * By default, orig is set to static. When the first time this object 
       * is configured, orig will be set to DHCP if IPv6Prefix is null.
       * Once orig is set to DHCP, the orig will not change any more.
       */
      if (cmsUtl_strcmp(currObj->origin, MDMVS_DHCPV6) != 0)
      {
         if (newObj->IPv6Prefix)
         {
            if (!cmsUtl_isValidIpAddress(AF_INET6, newObj->IPv6Prefix))
            {
               cmsLog_error("invalid static BMR IPv6 prefix <%s>", newObj->IPv6Prefix);
               return CMSRET_INVALID_ARGUMENTS;
            }

            CMSMEM_REPLACE_STRING_FLAGS(newObj->origin, MDMVS_STATIC, mdmLibCtx.allocFlags);
         }
         else
         {
            CMSMEM_REPLACE_STRING_FLAGS(newObj->origin, MDMVS_DHCPV6, mdmLibCtx.allocFlags);

            /*
             * if configure dynamic, need to re-launch dhcp6c to request mapt
             */
            if (cmsUtl_strcmp(currObj->origin, MDMVS_DHCPV6) != 0)
            {
               InstanceIdStack iidStack_domain;
               Dev2MapDomainObject *domain = NULL;

               iidStack_domain = *iidStack;
               if (cmsObj_getAncestorFlags(MDMOID_DEV2_MAP_DOMAIN, MDMOID_DEV2_MAP_DOMAIN_RULE,
                                           &iidStack_domain, OGF_NO_VALUE_UPDATE,
                                           (void **) &domain) != CMSRET_SUCCESS )
               {
                  cmsLog_error("Cannot get domain obj");
                  return CMSRET_MDM_TREE_ERROR;
               }

               rutTunnel_restartDhcpClientForTunnel(domain->WANInterface, FALSE);

               cmsLog_debug("dynamic MAP-T restarted dhcpv6");

               cmsObj_free((void **) &domain);

               return ret;
            }
         }
      }
   }

   /* Fetch the associated domain information */
   if ((ret = getDomainInfo(iidStack, brprefix, wanintf, &psidOffset,
                            &psidLen, &psid)) != CMSRET_SUCCESS)
   {
      cmsLog_error("rutMap_getDomainInfo fail");
      return ret;
   }

   cmsLog_debug( "Configuring MAP-T: OldStatus<%s> "
                 "BRPrefix<%s> wanintf<%s> psid<0x%x>, psidOff<%d> psidLen<%d>", 
                 currObj->status,
                 brprefix, wanintf, psid, psidOffset, psidLen);

   if (newObj != NULL && newObj->enable)
   {
      if (qdmIpIntf_isWanInterfaceUpLocked_dev2(wanintf, FALSE))
      {
         ret = launchMapt(brprefix, wanintf, psidOffset, psidLen, psid, newObj->IPv6Prefix, newObj->IPv4Prefix, TRUE);
         CMSMEM_REPLACE_STRING_FLAGS(newObj->status, MDMVS_ENABLED, mdmLibCtx.allocFlags);
      }
      else
      {
         cmsLog_debug("WAN<%s> is not up", wanintf);
         if (cmsUtl_strcmp(currObj->status, MDMVS_ENABLED) == 0)
         {
            ret = launchMapt(brprefix, wanintf, psidOffset, psidLen, psid, newObj->IPv6Prefix, newObj->IPv4Prefix, FALSE);
            CMSMEM_REPLACE_STRING_FLAGS(newObj->status, MDMVS_DISABLED, mdmLibCtx.allocFlags);
         }
      }
   }
   else if (DELETE_OR_DISABLE_EXISTING(newObj, currObj))
   {
      cmsLog_debug("Deleting MAP-T");

      if (cmsUtl_strcmp(currObj->status, MDMVS_ENABLED) == 0)
      {
         ret = launchMapt(brprefix, wanintf, psidOffset, psidLen, psid, currObj->IPv6Prefix, currObj->IPv4Prefix, FALSE);
      }

      if (newObj) /* case of disable */
      {
         CMSMEM_REPLACE_STRING_FLAGS(newObj->status, MDMVS_DISABLED, mdmLibCtx.allocFlags);
      }
   }   

   return ret;
}

#endif /* SUPPORT_MAPT */
#endif /* DMP_X_BROADCOM_COM_DEV2_IPV6_1 */



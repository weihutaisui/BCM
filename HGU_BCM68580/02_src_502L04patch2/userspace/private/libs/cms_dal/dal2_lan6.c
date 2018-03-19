/*
* <:copyright-BRCM:2011:proprietary:standard
* 
*    Copyright (c) 2011 Broadcom 
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
/** For TR181 IPv6 - support dev2 objects
 */
#ifdef DMP_DEVICE2_BASELINE_1    /* this file touches TR181 objects */

/* this file touches TR181 IPv6 objects */
#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1 /* aka SUPPORT_IPV6 */

#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"
#include "cms_qdm.h"
#include "dal.h"

static CmsRet updateUlaPrefixToIpObj(const char *ulaPrefix)
{
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("update ula prefix<%s> to ipObj", ulaPrefix);

   if (cmsUtl_isValidIpAddress(AF_INET6, ulaPrefix))
   {
      InstanceIdStack iidStack_ip = EMPTY_INSTANCE_ID_STACK;
      Dev2IpObject *ipObj = NULL;

      if ((ret = cmsObj_get(MDMOID_DEV2_IP, &iidStack_ip, 0, (void **) &ipObj)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to get device2.ip, ret = %d", ret);
         return ret;
      }

      /* FIXME: should be fdxx::/48 */
      CMSMEM_REPLACE_STRING(ipObj->ULAPrefix, ulaPrefix);
      ret = cmsObj_set(ipObj, &iidStack_ip);
      if (ret != CMSRET_SUCCESS)
      {
          cmsLog_error("Failed setting ip.ULAPrefix. ret %d", ret);
      }

      cmsObj_free((void **) &ipObj);
   }

   return ret;
}

static void updateUlaPrefixToIpv6PrefixObj(Dev2Ipv6PrefixObject *ipv6Prefix, 
                                           const WEB_NTWK_VAR *glbWebVar)
{
   char addrtmp[CMS_IPADDR_LENGTH];
   char timestring[BUFLEN_128];

   cmsLog_debug("enter: prefix<%s>", glbWebVar->ipv6UlaPrefix);

   ipv6Prefix->enable = ipv6Prefix->onLink = glbWebVar->enblRadvdUla;
   ipv6Prefix->X_BROADCOM_COM_UniqueLocalFlag = TRUE;

   /* If ULA prefix is disabled at webUI, prefix/plt/vlt won't be available */
   if (glbWebVar->enblRadvdUla)
   {
      cmsNet_subnetIp6SitePrefix(glbWebVar->ipv6UlaPrefix, 0, 64, addrtmp);
      sprintf(addrtmp, "%s/64", addrtmp);
      CMSMEM_REPLACE_STRING(ipv6Prefix->prefix, addrtmp);

      if ( glbWebVar->ipv6UlaPlt > 0 )
      {
         ipv6Prefix->X_BROADCOM_COM_Plt = glbWebVar->ipv6UlaPlt * 3600;
      }
      else
      {
         ipv6Prefix->X_BROADCOM_COM_Plt = glbWebVar->ipv6UlaPlt;
      }

      cmsTms_getXSIDateTime(ipv6Prefix->X_BROADCOM_COM_Plt, timestring, sizeof(timestring));
      CMSMEM_REPLACE_STRING(ipv6Prefix->preferredLifetime, timestring);

      if ( glbWebVar->ipv6UlaVlt > 0 )
      {
         ipv6Prefix->X_BROADCOM_COM_Vlt = glbWebVar->ipv6UlaVlt * 3600;
      }
      else
      {
         ipv6Prefix->X_BROADCOM_COM_Vlt = glbWebVar->ipv6UlaVlt;
      }
      cmsTms_getXSIDateTime(ipv6Prefix->X_BROADCOM_COM_Vlt, timestring, sizeof(timestring));
      CMSMEM_REPLACE_STRING(ipv6Prefix->preferredLifetime, timestring);
   }

   return;
}

CmsRet dalLan_setLan6Cfg_dev2(const WEB_NTWK_VAR *glbWebVar)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStackIpv6Addr = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack savedIpifIidStack = EMPTY_INSTANCE_ID_STACK;   
   InstanceIdStack savedPrefexIidStack = EMPTY_INSTANCE_ID_STACK;      
   Dev2IpInterfaceObject *ipIntfObj = NULL;
   Dev2Ipv6PrefixObject *ipv6Prefix = NULL;   
   char addrtmp[CMS_IPADDR_LENGTH];
   char *tmp;
   Dev2Ipv6AddressObject *ipv6AddrObj = NULL;
   MdmPathDescriptor pathDesc;   
   char *fullStr;
   UBOOL8 found=FALSE;
   UBOOL8 dhcp6StatefulChange = FALSE;
   UBOOL8 ulaPrefixChange = FALSE;
   UBOOL8 radvdUlaChange = FALSE;
   CmsRet ret = !CMSRET_SUCCESS;

   /* Find the default lan ip interface "br0" and set the ipv6Enable flag */
   while (!found && (cmsObj_getNext(MDMOID_DEV2_IP_INTERFACE, &iidStack, (void **) &ipIntfObj)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(ipIntfObj->name, "br0"))
      {
         /* if found, exit the while loop and do not free the object here */
         found = TRUE;
      }  
      else
      {
         cmsObj_free((void **) &ipIntfObj);
      }         
   }
   
   if (!found)
   {
      cmsLog_error("Failed to fund the default lan ip interface br0");
      return ret;      
   }

   /* Set the  ip interface enable and IPv6Enable flag */
   ipIntfObj->enable = TRUE;
   ipIntfObj->IPv6Enable = TRUE;

   if ((ipIntfObj->ULAEnable != glbWebVar->enblRadvdUla) ||
       (ipIntfObj->X_BROADCOM_COM_ULARandom != glbWebVar->enblRandomULA) )
   {
      ulaPrefixChange = TRUE;

      ipIntfObj->ULAEnable = glbWebVar->enblRadvdUla;
      ipIntfObj->X_BROADCOM_COM_ULARandom = glbWebVar->enblRandomULA;
   }
   
   ret = cmsObj_set(ipIntfObj, &iidStack);
   cmsObj_free((void **) &ipIntfObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set device2.ip.interface.{i}.ipIntfObj, ret = %d", ret);
      return ret;
   }

   savedIpifIidStack = iidStack;
   
   /*
    *  Configuration order:
    *     1. ULA address
    *     2. ULA prefix/enable/change
    *     3. DHCPv6 Server
    *     4. Radvd configuration depending on 1 and 3
    */

   /*
    * Handle ULA address configuration:
    *    ULA address is treated as static address configuration on br0
    *    So the origin of IPv6Address/IPv6Prefix object is static
    *    It's not related to ULA's prefix assignment.
    */
   {
   found = FALSE;
   while (!found && cmsObj_getNextInSubTreeFlags(MDMOID_DEV2_IPV6_ADDRESS,
                           &iidStack, &iidStackIpv6Addr,
                           OGF_NO_VALUE_UPDATE,
                           (void **)&ipv6AddrObj) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(ipv6AddrObj->origin, MDMVS_STATIC))
      {
         found = TRUE;
         break;
      }
      cmsObj_free((void **) &ipv6AddrObj);
   }

   /* New ULA configuration */
   if (!found)
   {
      ipv6AddrObj = NULL;

      if (cmsUtl_isValidIpAddress(AF_INET6, glbWebVar->lanIntfAddr6) && 
          cmsUtl_isGUAorULA(glbWebVar->lanIntfAddr6))
      {
         int len;
         char *ptr = strchr(glbWebVar->lanIntfAddr6, '/');

         cmsLog_debug("add ula address config, addr=%s", glbWebVar->lanIntfAddr6);

         /* Add and set the prefix object */
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
         ipv6Prefix->onLink = TRUE;

         len = atoi(++ptr);
         cmsNet_subnetIp6SitePrefix(glbWebVar->lanIntfAddr6, 0, len, addrtmp);
         sprintf(addrtmp, "%s/%d", addrtmp, len);
         CMSMEM_REPLACE_STRING(ipv6Prefix->prefix, addrtmp);
         cmsLog_debug("ipv6Prefix->prefix: %s", ipv6Prefix->prefix);

         ret = cmsObj_set(ipv6Prefix, &iidStack);
         cmsObj_free((void **) &ipv6Prefix);
         if (ret != CMSRET_SUCCESS)
         {
             cmsLog_error("Failed setting ipPrefix. ret %d", ret);
             cmsObj_deleteInstance(MDMOID_DEV2_IPV6_PREFIX, &iidStack);
             return ret;
         }
         savedPrefexIidStack = iidStack;

         /* Add and set the ipv6 address object */
         iidStack = savedIpifIidStack;
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
//       CMSMEM_REPLACE_STRING(ipv6AddrObj->origin, MDMVS_STATIC); //origin is readonly with default value static
         strncpy(addrtmp, glbWebVar->lanIntfAddr6, sizeof(addrtmp));
         tmp = strchr(addrtmp, '/');
         *tmp = '\0';
         CMSMEM_REPLACE_STRING(ipv6AddrObj->IPAddress, addrtmp);
         cmsLog_debug("ipv6AddrObj->IPAddress: %s", ipv6AddrObj->IPAddress);

         memset(&pathDesc, 0, sizeof(MdmPathDescriptor));
         pathDesc.oid = MDMOID_DEV2_IPV6_PREFIX;
         pathDesc.iidStack = savedPrefexIidStack;
         if ((ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &fullStr)) != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsMdm_fullPathToPathDescriptor returns error. ret=%d", ret);
            cmsObj_deleteInstance(MDMOID_DEV2_IPV6_ADDRESS, &iidStack);
            return ret;
         }
         if (cmsUtl_strlen(fullStr)+1  > BUFLEN_256)
         {
            cmsLog_error("fullpath %s too long to fit in param", fullStr);
            CMSMEM_FREE_BUF_AND_NULL_PTR(fullStr);
            cmsObj_deleteInstance(MDMOID_DEV2_IPV6_ADDRESS, &iidStack);
            return ret;
         }
         else
         {
            CMSMEM_REPLACE_STRING(ipv6AddrObj->prefix, fullStr);
         }
   
         CMSMEM_FREE_BUF_AND_NULL_PTR(fullStr);

         if ((ret = cmsObj_set(ipv6AddrObj, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsObj_deleteInstance(MDMOID_DEV2_IPV6_ADDRESS, &iidStack);
            cmsLog_error("Failed to set ipv6AddrObj. ret=%d", ret);
         } 
         cmsObj_free((void **) &ipv6AddrObj);
      }
      else
      {
         cmsLog_notice("Invalid IPv6 GUA/ULA: %s", glbWebVar->lanIntfAddr6);
      }
   }
   else /* case of modifying/deleting ULA */
   {
      MdmPathDescriptor pathDesc;

      /*
       * ipv6AddrObj->IPAddress does not have prefix length info.
       * Need to fetch the info from corresponding prefixObj
       */
      INIT_PATH_DESCRIPTOR(&pathDesc);
      ret = cmsMdm_fullPathToPathDescriptor(ipv6AddrObj->prefix, &pathDesc);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsMdm_fullPathToPathDescriptor failed for %s, ret=%d",
                              ipv6AddrObj->prefix, ret);
      }
      else
      {
         ret = cmsObj_get(pathDesc.oid, &pathDesc.iidStack, 0,
                 (void **) &ipv6Prefix);
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("Could not get IP.Intf.IPv6Prefix obj, ret=%d", ret);
         }
         else
         {
            char *ptr = strchr(ipv6Prefix->prefix, '/');

            sprintf(addrtmp, "%s%s", ipv6AddrObj->IPAddress, ptr);
         }
      }

      cmsLog_debug("ULA old<%s>", addrtmp);

      if (cmsUtl_strcmp(glbWebVar->lanIntfAddr6, addrtmp))
      {
         /* ULA modification case */
         if (!IS_EMPTY_STRING(glbWebVar->lanIntfAddr6))
         {
            if (cmsUtl_isValidIpAddress(AF_INET6, glbWebVar->lanIntfAddr6) && 
                cmsUtl_isGUAorULA(glbWebVar->lanIntfAddr6))
            {
               /* update existing IPv6Address and IPv6Prefix Objects */
               int len;
               char *ptr = strchr(glbWebVar->lanIntfAddr6, '/');

               cmsLog_debug("handle ula address change, new addr=%s", glbWebVar->lanIntfAddr6);

               len = atoi(++ptr);
               cmsNet_subnetIp6SitePrefix(glbWebVar->lanIntfAddr6, 0, len, addrtmp);
               sprintf(addrtmp, "%s/%d", addrtmp, len);
               CMSMEM_REPLACE_STRING(ipv6Prefix->prefix, addrtmp);
               cmsLog_debug("new ipv6Prefix->prefix: %s", ipv6Prefix->prefix);

               ret = cmsObj_set(ipv6Prefix, &pathDesc.iidStack);
               if (ret != CMSRET_SUCCESS)
               {
                  cmsLog_error("Failed setting ipPrefix. ret %d", ret);
                  cmsObj_free((void **) &ipv6Prefix);
                  cmsObj_free((void **) &ipv6AddrObj);
                  return ret;
               }

               strncpy(addrtmp, glbWebVar->lanIntfAddr6, sizeof(addrtmp));
               ptr = strchr(addrtmp, '/');
               *ptr = '\0';
               CMSMEM_REPLACE_STRING(ipv6AddrObj->IPAddress, addrtmp);
               cmsLog_debug("new ipv6AddrObj->IPAddress: %s", ipv6AddrObj->IPAddress);

               if ((ret = cmsObj_set(ipv6AddrObj, &iidStackIpv6Addr)) != CMSRET_SUCCESS)
               {
                  cmsLog_error("Failed to set ipv6AddrObj. ret=%d", ret);
                  if ((ret = cmsObj_deleteInstance(MDMOID_DEV2_IPV6_PREFIX, &pathDesc.iidStack)) != CMSRET_SUCCESS)
                  {
                     cmsLog_error("Failed to delete ipv6PrefixObj. ret=%d", ret);
                  }

                  cmsObj_free((void **) &ipv6Prefix);
                  cmsObj_free((void **) &ipv6AddrObj);
                  return ret;
               } 
            }
            else
            {
               cmsLog_error("Invalid IPv6 GUA/ULA: %s", glbWebVar->lanIntfAddr6);
            }
         }
         else /* ULA removal case */
         {
            /* delete IPv6Address and IPv6Prefix objects */
            if ((ret = cmsObj_deleteInstance(MDMOID_DEV2_IPV6_ADDRESS, &iidStackIpv6Addr)) != CMSRET_SUCCESS)
            {
               cmsLog_error("Failed to delete ipv6AddrObj. ret=%d", ret);
            }

            if ((ret = cmsObj_deleteInstance(MDMOID_DEV2_IPV6_PREFIX, &pathDesc.iidStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("Failed to delete ipv6PrefixObj. ret=%d", ret);
            }
         }
      }
      else
      {
         cmsLog_debug("ULA no change");
      }

      cmsObj_free((void **) &ipv6Prefix);
      cmsObj_free((void **) &ipv6AddrObj);
   }
   }

   /*
    * Handle ULA prefix configuration:
    *    1. update IP.ULAPrefix
    *    2. ULA prefix has the origin of AutoConfigured
    *    3. set IP.Interface.ULAEnable and IP.Interface.ULARandom (done above)
    *    4. if (first time configure ULAPrefix)
    *          {create prefixObj accordingly}
    *       else if (modify ULAPrefix or disable ULAPrefix)
    *          {update objects accordingly without deleting object}
    *    Basically, as long as prefixObj is created for ULAPrefix, it will be 
    *    persistent to remember configured ULAPrefix. It can only be disabled
    *    but cannot be deleted.
    */
   {
      InstanceIdStack iidStackUlaPrefix = EMPTY_INSTANCE_ID_STACK;

      found = FALSE;
      iidStack = savedIpifIidStack;
      while (!found && cmsObj_getNextInSubTreeFlags(MDMOID_DEV2_IPV6_PREFIX,
                           &iidStack, &iidStackUlaPrefix,
                           OGF_NO_VALUE_UPDATE,
                           (void **)&ipv6Prefix) == CMSRET_SUCCESS)
      {
         /* FIXME: only support ONE ULA prefix on one interface */
         if (ipv6Prefix->X_BROADCOM_COM_UniqueLocalFlag)
         {
            found = TRUE;
            break;
         }
         cmsObj_free((void **) &ipv6Prefix);
      }

      if (!found)
      {
         /* first time ULA prefix configuration */
         if (glbWebVar->enblRadvdUla)
         {
            updateUlaPrefixToIpObj(glbWebVar->ipv6UlaPrefix);
      
            iidStack = savedIpifIidStack;
            /* Add and set the prefix object */
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

            updateUlaPrefixToIpv6PrefixObj(ipv6Prefix, glbWebVar);

            ret = cmsObj_set(ipv6Prefix, &iidStack);
            if (ret != CMSRET_SUCCESS)
            {
               cmsLog_error("Failed setting ipPrefix. ret %d", ret);
               cmsObj_free((void **) &ipv6Prefix);   
               cmsObj_deleteInstance(MDMOID_DEV2_IPV6_PREFIX, &iidStack);
               return ret;
            }

            radvdUlaChange = TRUE;
         }
      }
      else if (ulaPrefixChange ||
               (ipv6Prefix->X_BROADCOM_COM_Plt != glbWebVar->ipv6UlaPlt*3600) ||
               (ipv6Prefix->X_BROADCOM_COM_Vlt != glbWebVar->ipv6UlaVlt*3600) ||
               (cmsUtl_strcmp(ipv6Prefix->prefix, glbWebVar->ipv6UlaPrefix))
              )
      {
         /* ULA Prefix configuration changed */
         cmsLog_debug("ULAPrefix orig: %d/%d/%d/%s  new: %d/%d/%d/%s random<%d>",
                 ipv6Prefix->enable, ipv6Prefix->X_BROADCOM_COM_Plt,
                 ipv6Prefix->X_BROADCOM_COM_Vlt, ipv6Prefix->prefix,
                 glbWebVar->enblRadvdUla, glbWebVar->ipv6UlaPlt*3600,
                 glbWebVar->ipv6UlaVlt*3600, glbWebVar->ipv6UlaPrefix,
                 glbWebVar->enblRandomULA);

         if (cmsUtl_strcmp(ipv6Prefix->prefix, glbWebVar->ipv6UlaPrefix))
         {
            updateUlaPrefixToIpObj(glbWebVar->ipv6UlaPrefix);
         }

         updateUlaPrefixToIpv6PrefixObj(ipv6Prefix, glbWebVar);

         ret = cmsObj_set(ipv6Prefix, &iidStackUlaPrefix);
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("Failed setting ipPrefix. ret %d", ret);
            cmsObj_free((void **) &ipv6Prefix);   
            return ret;
         }

         radvdUlaChange = TRUE;
      }
      cmsObj_free((void **) &ipv6Prefix);
   }

   /* 
    * Handle Dhcp6s configuration:
    *    1. DHCPv6.Server.Enable is always set to true in mdm2_initlan6.c
    */
   {
      Dev2Dhcpv6ServerPoolObject *serverPoolObj=NULL;
      InstanceIdStack iidStackDhcp6s = EMPTY_INSTANCE_ID_STACK;

      memset(&pathDesc, 0, sizeof(MdmPathDescriptor));
      pathDesc.oid = MDMOID_DEV2_IP_INTERFACE;
      pathDesc.iidStack = savedIpifIidStack;
      if ((ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &fullStr)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsMdm_fullPathToPathDescriptor returns error. ret=%d", ret);
         return ret;
      }
      if (cmsUtl_strlen(fullStr)+1  > BUFLEN_256)
      {
         cmsLog_error("fullpath %s too long to fit in param", fullStr);
         CMSMEM_FREE_BUF_AND_NULL_PTR(fullStr);
         return ret;
      }
   
      found = FALSE;
      while (!found && (cmsObj_getNext(MDMOID_DEV2_DHCPV6_SERVER_POOL, &iidStackDhcp6s, (void **) &serverPoolObj)) == CMSRET_SUCCESS)
      {
         if (!cmsUtl_strcmp(serverPoolObj->interface, fullStr))
         {
            /* if found, exit the while loop and do not free the object here */
            found = TRUE;
         }  
         else
         {
            cmsObj_free((void **) &serverPoolObj);
         }         
      }
   
      if (!found)
      {
         cmsLog_error("Failed to fund the corresponding dhcp6s.pool");
         CMSMEM_FREE_BUF_AND_NULL_PTR(fullStr);
         return ret;      
      }

      cmsLog_debug("handle dhcp6s config of %s", fullStr);
      CMSMEM_FREE_BUF_AND_NULL_PTR(fullStr);

      if (glbWebVar->enblDhcp6sStateful != serverPoolObj->IANAEnable)
      {
         dhcp6StatefulChange = TRUE;
      }

      /* set object if any of the parameters changes */
      if ((glbWebVar->enblDhcp6s  != serverPoolObj->enable) || dhcp6StatefulChange ||
          (cmsUtl_strcmp(glbWebVar->ipv6IntfIDStart, serverPoolObj->X_BROADCOM_COM_MinInterfaceID)) ||
          (cmsUtl_strcmp(glbWebVar->ipv6IntfIDEnd, serverPoolObj->X_BROADCOM_COM_MaxInterfaceID)) ||
          ((glbWebVar->dhcp6LeasedTime * 3600) != serverPoolObj->X_BROADCOM_COM_IANALeaseTime))
      {
         /* update user's configuration to data model */
         serverPoolObj->enable = glbWebVar->enblDhcp6s;
         serverPoolObj->IANAEnable = glbWebVar->enblDhcp6sStateful;
         CMSMEM_REPLACE_STRING(serverPoolObj->X_BROADCOM_COM_MinInterfaceID, glbWebVar->ipv6IntfIDStart);
         CMSMEM_REPLACE_STRING(serverPoolObj->X_BROADCOM_COM_MaxInterfaceID, glbWebVar->ipv6IntfIDEnd);
         /* webVar is in hours, MDM in seconds */
         serverPoolObj->X_BROADCOM_COM_IANALeaseTime = glbWebVar->dhcp6LeasedTime * 3600;

         if ((ret = cmsObj_set((void *)serverPoolObj, &iidStackDhcp6s)) != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsObj_set returns error. ret=%d", ret);
            cmsObj_free((void **)&serverPoolObj);         
            return ret;         
         }
      }
      else
      {
         cmsLog_debug("There is no change in dhcp6s");
      }
   }

   /*
    * Handle Radvd configuration:
    *    ULA prefix has the origin of AutoConfigured
    *    FIXME: Consider MFlag/plt/vlt/AdvDefaultLifetim
    */
   {
      InstanceIdStack iidStack_raIntf = EMPTY_INSTANCE_ID_STACK;
      Dev2RouterAdvertisementInterfaceSettingObject *raIntfObj = NULL;

      // FIXME: Assuming only one RouterAdvertisement.InterfaceSetting.1
      if (cmsObj_getNext(MDMOID_DEV2_ROUTER_ADVERTISEMENT_INTERFACE_SETTING, 
                         &iidStack_raIntf, (void **) &raIntfObj) == CMSRET_SUCCESS)
      {
         if ((glbWebVar->enblRadvd != raIntfObj->enable) || 
             radvdUlaChange || dhcp6StatefulChange)
         {
            cmsLog_debug("handle radvd config change");

            raIntfObj->enable = glbWebVar->enblRadvd;
            raIntfObj->advManagedFlag = glbWebVar->enblDhcp6sStateful?TRUE:FALSE;
            raIntfObj->advOtherConfigFlag = TRUE; //FIXME: RDNSS should be considered

            ret = cmsObj_set(raIntfObj, &iidStack_raIntf);

            if (ret != CMSRET_SUCCESS)
            {
               cmsLog_error("Failed setting RAInterfaceSetting. ret %d", ret);
               cmsObj_free((void **) &raIntfObj);   
               return ret;
            }
         }

         cmsObj_free((void **) &raIntfObj);   
      }
      else
      {
         cmsLog_error("Failed getting RAInterfaceSetting.");
      }
   }

#if defined(DMP_X_BROADCOM_COM_MLDSNOOP_1)
   if ((ret = dalLan_setMldSnooping(glbWebVar->brName, glbWebVar->enblMldSnp,
                                    glbWebVar->enblMldMode, glbWebVar->enblMldLanToLanMcast)) != CMSRET_SUCCESS)
   {
      cmsLog_error("set of MLD snooping failed, ret=%d", ret);
   }
#endif

   return ret;
}


#endif /* DMP_X_BROADCOM_COM_DEV2_IPV6_1 */

#endif /* DMP_DEVICE2_BASELINE_1 */





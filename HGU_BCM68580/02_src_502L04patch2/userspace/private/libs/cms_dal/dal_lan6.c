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

#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"
#include "dal.h"

#ifdef DMP_BASELINE_1       /* this file touches TR98 objects */

#ifdef DMP_X_BROADCOM_COM_IPV6_1  /* this file touches Broadcom proprietary IPV6 objects */

CmsRet dalLan_setLan6Cfg_igd(const WEB_NTWK_VAR *glbWebVar)
{
   UBOOL8 radvdUlaChange = FALSE;
   UBOOL8 dhcp6StatefulChange = FALSE;
   InstanceIdStack iidStack= EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack2= EMPTY_INSTANCE_ID_STACK;
   IPv6LanHostCfgObject *ipv6Obj = NULL;
   RadvdConfigMgtObject *radvdObj = NULL;
   ULAPrefixInfoObject *radvdUlaObj = NULL;
   CmsRet ret;
   IPv6LanIntfAddrObject *ulaAddrObj = NULL;

   cmsLog_debug("Enter");

   /* get the current lan configuration */
   if ((ret = cmsObj_getNextFlags(MDMOID_I_PV6_LAN_HOST_CFG, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&ipv6Obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_get <MDMOID_I_PV6_LAN_HOST_CFG> returns error. ret=%d", ret);
      return ret;
   }

   if ((ret = cmsObj_getNextFlags(MDMOID_I_PV6_LAN_INTF_ADDR, &iidStack2, OGF_NO_VALUE_UPDATE, (void **)&ulaAddrObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_get<MDMOID_I_PV6_LAN_INTF_ADDR> returns error. ret=%d", ret);
      cmsObj_free((void **)&ipv6Obj);
      return ret;
   }

   if ( glbWebVar->enblDhcp6sStateful != ipv6Obj->statefulDHCPv6Server )
   {
      dhcp6StatefulChange = TRUE;
   }

   /* set object if any of the parameters changes */
   if ((glbWebVar->enblRadvd  != ipv6Obj->DHCPv6ServerEnable) || dhcp6StatefulChange ||
       (cmsUtl_strcmp(glbWebVar->ipv6IntfIDStart, ipv6Obj->minInterfaceID)) ||
       (cmsUtl_strcmp(glbWebVar->ipv6IntfIDEnd, ipv6Obj->maxInterfaceID)) ||
       (glbWebVar->dhcp6LeasedTime != ipv6Obj->DHCPv6LeaseTime))
   {
      /* overwrite with user's configuration */
      ipv6Obj->DHCPv6ServerEnable = glbWebVar->enblDhcp6s;
      ipv6Obj->statefulDHCPv6Server = glbWebVar->enblDhcp6sStateful;
      CMSMEM_REPLACE_STRING(ipv6Obj->minInterfaceID, glbWebVar->ipv6IntfIDStart);
      CMSMEM_REPLACE_STRING(ipv6Obj->maxInterfaceID, glbWebVar->ipv6IntfIDEnd);
      /* webVar is in hours, MDM in seconds */
      ipv6Obj->DHCPv6LeaseTime = glbWebVar->dhcp6LeasedTime * 3600;

      if ((ret = cmsObj_set((void *)ipv6Obj, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsObj_set returns error. ret=%d", ret);
         cmsObj_free((void **)&ipv6Obj);         
         return ret;         
      }
   }
   else
   {
      cmsLog_debug("There is no change in IPv6 LAN host configuration management");
   }

   cmsObj_free((void **)&ipv6Obj);

   
   /* set LAN intf IPv6 addr if the parameters changes */
   if (cmsUtl_strcmp(glbWebVar->lanIntfAddr6, ulaAddrObj->uniqueLocalAddress))
   {
      /* For correct static IPv6 LAN address configuration */
      if ( !IS_EMPTY_STRING(glbWebVar->lanIntfAddr6) )
      {
         if ( cmsUtl_isValidIpAddress(AF_INET6, glbWebVar->lanIntfAddr6) )
         {
            if (cmsUtl_isGUAorULA(glbWebVar->lanIntfAddr6))
            {
               CMSMEM_REPLACE_STRING(ulaAddrObj->uniqueLocalAddress, glbWebVar->lanIntfAddr6);
            }
            else
            {
               cmsLog_error("Invalid IPv6 GUA/ULA: %s", glbWebVar->lanIntfAddr6);
            }
         }
         else
         {
            cmsLog_error("invalid IPv6 address: %s", glbWebVar->lanIntfAddr6);
         }

      }
      /* 
       * If current config contains ULA and input is NULL or invalid IPv6 address, 
       * remove the current ULA 
       */
      else if ( ulaAddrObj->uniqueLocalAddress != NULL )
      {
         CMSMEM_FREE_BUF_AND_NULL_PTR(ulaAddrObj->uniqueLocalAddress);
      }

      if ((ret = cmsObj_set(ulaAddrObj, &iidStack2)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsObj_set<MDMOID_I_PV6_LAN_INTF_ADDR> returns error. ret=%d", ret);
         cmsObj_free((void **)&ulaAddrObj);
         return ret;
      }
   }
   else
   {
      cmsLog_debug("No change in IPv6 LAN ULA address");
   }

   cmsObj_free((void **)&ulaAddrObj);

   /* Radvd for ULA */
   INIT_INSTANCE_ID_STACK(&iidStack2);

   /* get the current lan configuration */
   if ((ret = cmsObj_getNextFlags(MDMOID_ULA_PREFIX_INFO, &iidStack2, OGF_NO_VALUE_UPDATE, (void **)&radvdUlaObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_get <MDMOID_ULA_PREFIX_INFO> returns error. ret=%d", ret);
      return ret;
   }


   if ( (radvdUlaObj->enable != glbWebVar->enblRadvdUla) ||
        (radvdUlaObj->random != glbWebVar->enblRandomULA) ||
        (radvdUlaObj->preferredLifeTime != glbWebVar->ipv6UlaPlt) ||
        (radvdUlaObj->validLifeTime != glbWebVar->ipv6UlaVlt) ||
        (cmsUtl_strcmp(radvdUlaObj->prefix, glbWebVar->ipv6UlaPrefix))
      )
   {
      radvdUlaChange = TRUE;
      radvdUlaObj->enable = glbWebVar->enblRadvdUla;
      radvdUlaObj->random = glbWebVar->enblRandomULA;
      CMSMEM_REPLACE_STRING(radvdUlaObj->prefix, glbWebVar->ipv6UlaPrefix);

      if ( glbWebVar->ipv6UlaPlt > 0 )
      {
         radvdUlaObj->preferredLifeTime = glbWebVar->ipv6UlaPlt * 3600;
      }
      else
      {
         radvdUlaObj->preferredLifeTime = glbWebVar->ipv6UlaPlt;
      }

      if ( glbWebVar->ipv6UlaVlt > 0 )
      {
         radvdUlaObj->validLifeTime = glbWebVar->ipv6UlaVlt * 3600;
      }
      else
      {
         radvdUlaObj->validLifeTime = glbWebVar->ipv6UlaVlt;
      }
   }

   if ((ret = cmsObj_set(radvdUlaObj, &iidStack2)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_set<MDMOID_ULA_PREFIX_INFO> returns error. ret=%d", ret);
      cmsObj_free((void **)&radvdUlaObj);
      return ret;
   }

   cmsObj_free((void **)&radvdUlaObj);

   /* Radvd Launching */
   if ((ret = cmsObj_get(MDMOID_RADVD_CONFIG_MGT, &iidStack, 0, (void **) &radvdObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_get<MDMOID_RADVD_CONFIG_MGT> returns error. ret=%d", ret);
      return ret;
   }

   if ( (glbWebVar->enblRadvd  != radvdObj->enable) || radvdUlaChange || dhcp6StatefulChange )
   {
      radvdObj->enable  = glbWebVar->enblRadvd;
      if ((ret = cmsObj_set((void *)radvdObj, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsObj_set returns error. ret=%d", ret);
         cmsObj_free((void **)&radvdObj);
         return ret;
      }
   }
   else
   {
      cmsLog_debug("There is no change in Radvd and RadvdUla configuration management");
   }
   cmsObj_free((void **)&radvdObj);

#if defined(DMP_X_BROADCOM_COM_MLDSNOOP_1)
   if ((ret = dalLan_setMldSnooping(glbWebVar->brName, glbWebVar->enblMldSnp,
                                    glbWebVar->enblMldMode, glbWebVar->enblMldLanToLanMcast)) != CMSRET_SUCCESS)
   {
      cmsLog_error("set of MLD snooping failed, ret=%d", ret);
   }
#endif

   return ret;

}

#endif  /* DMP_X_BROADCOM_COM_IPV6_1 */

#endif /* DMP_BASELINE_1 */




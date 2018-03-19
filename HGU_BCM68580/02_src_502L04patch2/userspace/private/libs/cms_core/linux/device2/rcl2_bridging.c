/***********************************************************************
 *
 *  Copyright (c) 2006-2011  Broadcom Corporation
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

#ifdef DMP_DEVICE2_BRIDGE_1

#include "odl.h"
#include "cms_core.h"
#include "cms_qdm.h"
#include "cms_util.h"
#include "cms_strconv2.h"
#include "rcl.h"
#include "rut_util.h"
#include "rut_lan.h"
#include "rut2_util.h"
#include "rut2_bridging.h"
#include "beep_networking.h"
#include "qdm_route.h"



/*!\file rcl2_bridging.c
 * \brief This file contains Device.Bridging. objects.
 * This file currently contains the vlan objects, but they can be moved
 * out to another file if we want more separation of the code.
 *
 */
CmsRet rcl_dev2BridgingObject( _Dev2BridgingObject *newObj __attribute__((unused)),
                const _Dev2BridgingObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   return CMSRET_SUCCESS;
}

CmsRet rcl_dev2BridgeObject( _Dev2BridgeObject *newObj,
                const _Dev2BridgeObject *currObj,
                const InstanceIdStack *iidStack,
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{

   if (ADD_NEW(newObj, currObj))
   {
      rutUtil_modifyNumBridge(iidStack, 1);
   }

   if (ENABLE_NEW_OR_ENABLE_EXISTING(newObj, currObj))
   {
      UBOOL8 enableBridge=TRUE;
      SINT32 bridgeNum=-1;

      if (newObj != NULL && currObj == NULL && IS_EMPTY_STRING(newObj->X_BROADCOM_COM_IfName))
      {
         cmsLog_error("Bridge does not have a name... generate one");
         /* complain, but keep going */
      }

      if (IS_EMPTY_STRING(newObj->X_BROADCOM_COM_IfName))
      {
         char brIntfNameBuf[CMS_IFNAME_LENGTH]={0};

         if ((bridgeNum = rutLan_getNextAvailableBridgeNumber()) < 0)
         {
            return CMSRET_RESOURCE_EXCEEDED;
         }
         else
         {
            sprintf(brIntfNameBuf, "br%d", bridgeNum);
            CMSMEM_REPLACE_STRING_FLAGS(newObj->X_BROADCOM_COM_IfName, brIntfNameBuf, mdmLibCtx.allocFlags);
         }
      }

      /*
       * Just enable the bridge hre.  IP addrs are enabled from the RCL
       * handler functions for the IPv4 and IPv6 addr objects.
       */
      cmsLog_debug("enable linux bridge %s (without IP addr)",
                   newObj->X_BROADCOM_COM_IfName);
      rutLan_enableBridge(newObj->X_BROADCOM_COM_IfName, enableBridge,
                          "0.0.0.0", "0.0.0.0", "0.0.0.0");

      /* 
       * BEEP requires interface group to create separated bridge
       * and need to add firewall rules and NAT rules
       */
      if (newObj->X_BROADCOM_COM_Mode == INTFGRP_BR_BEEP_SECONDARY_MODE)
      {
          char activeGwIfName[CMS_IFNAME_LENGTH]={0};
          UBOOL8 isIPv4 = TRUE;

          /* firewall rules similar to guest wifi */
          rutIpt_beepNetworkingSecurity(newObj->X_BROADCOM_COM_IfName, INTFGRP_BR_BEEP_SECONDARY_MODE);

          qdmRt_getActiveDefaultGatewayLocked(activeGwIfName);
          if (!IS_EMPTY_STRING(activeGwIfName))
          {
             if (qdmIpIntf_isWanInterfaceUpLocked(activeGwIfName, isIPv4) &&
                 qdmIpIntf_isNatEnabledOnIntfNameLocked(activeGwIfName))
             {
                 rutIpt_beepNetworkingMasqueurade(newObj->X_BROADCOM_COM_IfName, activeGwIfName);
             }
          }
      }
      else if (newObj->X_BROADCOM_COM_Mode == INTFGRP_BR_BEEP_WANONLY_MODE)
      {
          char activeGwIfName[CMS_IFNAME_LENGTH]={0};
          UBOOL8 isIPv4 = TRUE;

          /* firewall rules to only allow WAN access */
          rutIpt_beepNetworkingSecurity(newObj->X_BROADCOM_COM_IfName, INTFGRP_BR_BEEP_WANONLY_MODE);

          qdmRt_getActiveDefaultGatewayLocked(activeGwIfName);
          if (!IS_EMPTY_STRING(activeGwIfName))
          {
             if (qdmIpIntf_isWanInterfaceUpLocked(activeGwIfName, isIPv4) &&
                 qdmIpIntf_isNatEnabledOnIntfNameLocked(activeGwIfName))
             {
                 rutIpt_beepNetworkingMasqueurade(newObj->X_BROADCOM_COM_IfName, activeGwIfName);
             }
          }
      }
   }


   if (DELETE_OR_DISABLE_EXISTING(newObj, currObj))
   {
      cmsLog_debug("disable linux bridge %s", currObj->X_BROADCOM_COM_IfName);
      rutLan_disableBridge(currObj->X_BROADCOM_COM_IfName);

      if (DELETE_EXISTING(newObj, currObj))
      {
         rutUtil_modifyNumBridge(iidStack, -1);
      }
   }

   return CMSRET_SUCCESS;
}


CmsRet rcl_dev2BridgePortObject( _Dev2BridgePortObject *newObj,
                                const _Dev2BridgePortObject *currObj,
                                const InstanceIdStack *iidStack,
                                char **errorParam __attribute__((unused)),
                                CmsRet *errorCode __attribute__((unused)))
{
   char brIntfNameBuf[CMS_IFNAME_LENGTH]={0};
   CmsRet ret;

   cmsLog_debug("Entered:");

   if (ADD_NEW(newObj, currObj))
   {
      rutUtil_modifyNumBridgePort(iidStack, 1);
   }


   /*
    * Always force management Port status to UP.  This allows brx which sits
    * on top of Management Port to always be UP.
    */
   if (newObj && newObj->managementPort && cmsUtl_strcmp(newObj->status, MDMVS_UP))
   {
      cmsLog_debug("force mgmtPort %s to status UP", cmsMdm_dumpIidStack(iidStack));
      CMSMEM_REPLACE_STRING_FLAGS(newObj->status, MDMVS_UP, mdmLibCtx.allocFlags);
      newObj->X_BROADCOM_COM_LastChange = cmsTms_getSeconds();
   }

   IF_STATUS_HAS_CHANGED_SET_LASTCHANGE(newObj, currObj);

   /*
    * First figure out what my Linux bridge intf name is.
    */
   ret = rutBridge_getParentBridgeIntfName_dev2(iidStack, brIntfNameBuf);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get parent bridge intfName, newObj=%p currObj=%p iidStack=%s",
                   newObj, currObj,
                   cmsMdm_dumpIidStack(iidStack));
      return ret;
   }


   /*
    * If we are not in a delete situation (newObj != NULL) and
    * we don't have a Linux interface name yet, try to figure it out.
    * (a) if this is a management port, ifName is in parent bridge.
    * (b) if this is not a management port, go down on LowerLayers
    */
   if ((newObj != NULL) && IS_EMPTY_STRING(newObj->name))
   {
      if (newObj->managementPort)
      {
         CMSMEM_REPLACE_STRING_FLAGS(newObj->name, brIntfNameBuf, mdmLibCtx.allocFlags);
      }
      else if (!IS_EMPTY_STRING(newObj->lowerLayers))
      {
         char lowerIntfNameBuf[CMS_IFNAME_LENGTH]={0};

         if ((ret = qdmIntf_fullPathToIntfnameLocked(newObj->lowerLayers, lowerIntfNameBuf)) != CMSRET_SUCCESS)
         {
            cmsLog_error("qdmIntf_getIntfnameFromFullPathLocked failed. ret %d", ret);
            return ret;
         }

         CMSMEM_REPLACE_STRING_FLAGS(newObj->name, lowerIntfNameBuf, mdmLibCtx.allocFlags);
      }
      cmsLog_debug("assigned port (mgmt=%d) name %s",
                   newObj->managementPort, newObj->name);
   }


   if (newObj && currObj &&
       (!newObj->managementPort) &&
        cmsUtl_strcmp(newObj->status, currObj->status))
   {
      /* link status has changed on lower layer port */
      if (!cmsUtl_strcmp(newObj->status, MDMVS_UP))
      {
         cmsLog_debug("bridge port %s is UP! add to %s",
                      newObj->name, brIntfNameBuf);
         rutLan_addInterfaceToBridge(newObj->name, FALSE, brIntfNameBuf);
         rutMulti_updateIgmpMldProxyIntfList();
      }
      else if (!cmsUtl_strcmp(currObj->status, MDMVS_UP))
      {
         /* remove from bridge only when going from UP to any non-UP state */
         /* don't care if going from non-UP to non-UP state */
         cmsLog_debug("bridge port %s is %s, remove from %s",
                      newObj->name, newObj->status, brIntfNameBuf);
         rutLan_removeInterfaceFromBridge(newObj->name, brIntfNameBuf);
         rutMulti_updateIgmpMldProxyIntfList();
      }
   }

   if (DELETE_EXISTING(newObj, currObj))
   {
      rutUtil_modifyNumBridgePort(iidStack, -1);

      if (!currObj->managementPort)
      {
         if (!cmsUtl_strcmp(currObj->status, MDMVS_UP))
         {
            cmsLog_debug("bridge port %s is being deleted, remove from %s",
                         currObj->name, brIntfNameBuf);
            rutLan_removeInterfaceFromBridge(currObj->name, brIntfNameBuf);
         }
      }
   }


   return CMSRET_SUCCESS;
}


CmsRet rcl_dev2BridgePortStatsObject( _Dev2BridgePortStatsObject *newObj __attribute__((unused)),
                const _Dev2BridgePortStatsObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   return CMSRET_SUCCESS;
}

CmsRet rcl_dev2BridgeVlanObject( _Dev2BridgeVlanObject *newObj,
                const _Dev2BridgeVlanObject *currObj,
                const InstanceIdStack *iidStack,
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   if (ADD_NEW(newObj, currObj))
   {
      rutUtil_modifyNumBridgeVlan(iidStack, 1);
   }

   if (DELETE_EXISTING(newObj, currObj))
   {
      rutUtil_modifyNumBridgeVlan(iidStack, -1);
   }

   return CMSRET_SUCCESS;
}

CmsRet rcl_dev2BridgeVlanPortObject( _Dev2BridgeVlanPortObject *newObj,
                const _Dev2BridgeVlanPortObject *currObj,
                const InstanceIdStack *iidStack,
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   if (ADD_NEW(newObj, currObj))
   {
      rutUtil_modifyNumBridgeVlanPort(iidStack, 1);
   }

   if (DELETE_EXISTING(newObj, currObj))
   {
      rutUtil_modifyNumBridgeVlanPort(iidStack, -1);
   }

   return CMSRET_SUCCESS;
}

CmsRet rcl_dev2BridgeFilterObject( _Dev2BridgeFilterObject *newObj,
                const _Dev2BridgeFilterObject *currObj,
                const InstanceIdStack *iidStack,
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{

   if (ADD_NEW(newObj, currObj))
   {
      rutUtil_modifyNumBridgeFilter(iidStack, 1);
   }

   if (DELETE_EXISTING(newObj, currObj))
   {
      rutUtil_modifyNumBridgeFilter(iidStack, -1);
   }

   return CMSRET_SUCCESS;
}

#endif  /* DMP_DEVICE2_BRIDGE_1 */

#endif    /* DMP_DEVICE2_BASELINE_1 */




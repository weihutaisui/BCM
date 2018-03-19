/***********************************************************************
 *
 *  Copyright (c) 2013  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2013:proprietary:standard

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

#ifdef DMP_BASELINE_1  


#include "cms.h"
#include "cms_mdm.h"
#include "cms_obj.h"
#include "cms_qdm.h"
#include "cms_util.h"
#include "cms_core.h"



UBOOL8 qdmMulti_isIgmpSnoopingEnabledOnBridgeLocked_igd(const char *bridgeIfName)
{
   CmsRet ret;
   UBOOL8 isMld = FALSE;
   UINT32 mode = 0;
   int lanToLanEnabled = 0;
   UBOOL8 enabled;

   ret = qdmMulti_getSnoopingInfoLocked_igd(bridgeIfName, isMld,
                                    &mode, &lanToLanEnabled, &enabled);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not find IGMP Snooping Info for bridgeIfName=%s, ret=%d", bridgeIfName, ret);
      return FALSE;
   }

   return enabled;
}


CmsRet qdmMulti_getSnoopingInfoLocked_igd(const char *brIfName, UBOOL8 isMld,
                 UINT32 *mode, int *lanToLanEnable, UBOOL8 * enabled)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found=FALSE;
   LanIpIntfObject *ipIntfObj=NULL;
   LanDevObject *lanDeviceObj=NULL;
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("Entered: brIfName=%s isMld=%d", brIfName, isMld);

   *enabled = FALSE;

   while (!found &&  cmsObj_getNext(MDMOID_LAN_IP_INTF, &iidStack, (void **) &ipIntfObj) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(ipIntfObj->X_BROADCOM_COM_IfName, brIfName))
      {
         found = TRUE;
      }
      cmsObj_free((void **) &ipIntfObj);
   }

   if (!found)
   {
      cmsLog_notice("could not find brIfName %s", brIfName);
      return CMSRET_INTERNAL_ERROR; /* enabled is false */
   }

   if ((ret = cmsObj_getAncestor(MDMOID_LAN_DEV, MDMOID_LAN_IP_INTF, &iidStack, (void **) &lanDeviceObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not getAncestor MDMOID_LAN_DEV. ret = %d", ret);
      return CMSRET_INTERNAL_ERROR; /* enabled is false */
   }

   /* we only need the instance id, not the lanDeviceObj */
   cmsObj_free((void **)&lanDeviceObj);

   *mode = 0;

   if (!isMld)
   {
      IgmpSnoopingCfgObject *igmpObj = NULL;

      if((ret = cmsObj_get(MDMOID_IGMP_SNOOPING_CFG, &iidStack, 0, (void **) &igmpObj)) != CMSRET_SUCCESS)
      {
         cmsLog_error("could not get igmpObj, ret=%d", ret);
         return CMSRET_INTERNAL_ERROR; /* enabled is false */
      }

      if (igmpObj->enable)
      {
         if(cmsUtl_strcmp(igmpObj->mode, MDMVS_STANDARD) == 0)
         {
            *mode = 1;
         }
         else
         {
            *mode = 2;
         }
         *enabled = TRUE;
      }
      *lanToLanEnable = igmpObj->lanToLanEnable;

      cmsObj_free((void **) &igmpObj);
   }
   else
   {
      MldSnoopingCfgObject *mldObj = NULL;

      if((ret = cmsObj_get(MDMOID_MLD_SNOOPING_CFG, &iidStack, 0, (void **) &mldObj)) != CMSRET_SUCCESS)
      {
         cmsLog_error("could not get mldObj, ret=%d", ret);
         return CMSRET_INTERNAL_ERROR; /* enabled is false */
      }

      if (mldObj->enable)
      {
         if(cmsUtl_strcmp(mldObj->mode, MDMVS_STANDARD) == 0)
         {
            *mode = 1;
         }
         else
         {
            *mode = 2;
         }
         *enabled = TRUE;
      }
      *lanToLanEnable = mldObj->lanToLanEnable;

      cmsObj_free((void **) &mldObj);
   }

   cmsLog_debug("[%s] return enable=%d mode=%d lan2lan=%d",
                 brIfName, *enabled, *mode, *lanToLanEnable);

   return ret;
}


CmsRet qdmMulti_getAssociatedBridgeIntfNameLocked_igd(const char *snoopFullPath,
                                                      char *bridgeIntfName)
{
   LanIpIntfObject *lanIpObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   MdmPathDescriptor pathDesc=EMPTY_PATH_DESCRIPTOR;
   UBOOL8 found=FALSE;
   CmsRet ret;

   /*
    * First convert from snooping obj fullpath to path descriptor.  Note
    * we don't care whether this fullpath points to an IGMP or MLD snooping
    * obj.  They will both yield the required iidStack which is used to
    * search for the corresponding LAN_IP_INTF obj.
    */
   ret = cmsMdm_fullPathToPathDescriptor(snoopFullPath, &pathDesc);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not convert %s, ret=%d", snoopFullPath, ret);
      return ret;
   }

   while (!found &&
          (ret = cmsObj_getNextInSubTree(MDMOID_LAN_IP_INTF,
                                         &pathDesc.iidStack,
                                         &iidStack,
                                   (void **) &lanIpObj)) == CMSRET_SUCCESS)
   {
      /* if the bridge intf name does not contain :, then we found it */
      if (lanIpObj->X_BROADCOM_COM_IfName &&
          (NULL == strstr(lanIpObj->X_BROADCOM_COM_IfName, ":")))
      {
         found = TRUE;
         strcpy(bridgeIntfName, lanIpObj->X_BROADCOM_COM_IfName);
      }
      cmsObj_free((void **)&lanIpObj);
   }

   /*
    * This function is sometimes called during create or delete, and
    * due to hierarchy of data model objects in TR98, the LAN_IP_INTF
    * object either does not exist or ifName is not set yet.  Don't complain
    * loudly about it.
    */
   if (found)
   {
      cmsLog_debug("%s => assoc brIntfName %s", snoopFullPath, bridgeIntfName);
   }
   else
   {
      cmsLog_debug("Could not find bridge associated with %s", snoopFullPath);
   }

   return ret;
}


CmsRet qdmMulti_getAssociatedIgmpSnoopingFullPathLocked_igd(const char *brIntfName,
                                                            char **fullPath)
{
   LanIpIntfObject *lanIpObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found=FALSE;
   CmsRet ret;

   while (!found &&
          (ret = cmsObj_getNextFlags(MDMOID_LAN_IP_INTF, &iidStack,
                                     OGF_NO_VALUE_UPDATE,
                                     (void **)&lanIpObj)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(brIntfName, lanIpObj->X_BROADCOM_COM_IfName))
      {
         MdmPathDescriptor pathDesc=EMPTY_PATH_DESCRIPTOR;

         pathDesc.oid = MDMOID_IGMP_SNOOPING_CFG;
         pathDesc.iidStack = iidStack;
         /*
          * iidStack points to the LanIpIntfObj.  IGMPSnoop is exactly 1 level
          * up from LanIpIntfObj, so just pop out the last instance id.
          */
         POP_INSTANCE_ID(&pathDesc.iidStack);
         ret = cmsMdm_pathDescriptorToFullPath(&pathDesc, fullPath);
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsMdm_pathDescToFullPath failed, ret=%d", ret);
         }

         found = TRUE;
      }
      cmsObj_free((void **)&lanIpObj);
   }

   /*
    * This function is sometimes called during create or delete, and
    * due to hierarchy of data model objects in TR98, the LAN_IP_INTF
    * object either does not exist or ifName is not set yet.  Don't complain
    * loudly about it.
    */
   if (found && (ret == CMSRET_SUCCESS))
   {
      cmsLog_debug("%s => igmpSnoop %s", brIntfName, fullPath);
   }
   else
   {
      cmsLog_debug("Could not find bridge %d", brIntfName);
   }

   return ret;
}


#ifdef DMP_X_BROADCOM_COM_MLDSNOOP_1
CmsRet qdmMulti_getAssociatedMldSnoopingFullPathLocked_igd(const char *brIntfName,
                                                            char **fullPath)
{
   LanIpIntfObject *lanIpObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found=FALSE;
   CmsRet ret;


   while (!found &&
          (ret = cmsObj_getNextFlags(MDMOID_LAN_IP_INTF, &iidStack,
                                     OGF_NO_VALUE_UPDATE,
                                     (void **)&lanIpObj)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(brIntfName, lanIpObj->X_BROADCOM_COM_IfName))
      {
         MdmPathDescriptor pathDesc=EMPTY_PATH_DESCRIPTOR;

         pathDesc.oid = MDMOID_MLD_SNOOPING_CFG;
         pathDesc.iidStack = iidStack;
         /*
          * iidStack points to the LanIpIntfObj.  MLDSnoop is exactly 1 level
          * up from LanIpIntfObj, so just pop out the last instance id.
          */
         POP_INSTANCE_ID(&pathDesc.iidStack);
         ret = cmsMdm_pathDescriptorToFullPath(&pathDesc, fullPath);
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsMdm_pathDescToFullPath failed, ret=%d", ret);
         }

         found = TRUE;
      }
      cmsObj_free((void **)&lanIpObj);
   }

   /*
    * This function is sometimes called during create or delete, and
    * due to hierarchy of data model objects in TR98, the LAN_IP_INTF
    * object either does not exist or ifName is not set yet.  Don't complain
    * loudly about it.
    */
   if (found && (ret == CMSRET_SUCCESS))
   {
      cmsLog_debug("%s => mldSnoop %s", brIntfName, fullPath);
   }
   else
   {
      cmsLog_debug("Could not find bridge %d", brIntfName);
   }

   return ret;
}
#endif  /* DMP_X_BROADCOM_COM_MLDSNOOP_1 */




/* in qdm_ipintf.c */
extern CmsRet qdmPrivate_getLanWanConnObj(const char *intfName,
                                          WanIpConnObject **wanIpConnObj,
                                          WanPppConnObject **wanPppConnObj,
                                          LanIpIntfObject **lanIpIntfObj,
                                    IPv6LanIntfAddrObject **ipv6LanIpAddrObj);


UBOOL8 qdmMulti_isIgmpProxyEnabledOnIntfNameLocked_igd(const char *intfName)
{
#ifdef DMP_X_BROADCOM_COM_IGMP_1
   WanIpConnObject *wanIpConnObj=NULL;
   WanPppConnObject *wanPppConnObj=NULL;
   CmsRet ret;
   UBOOL8 enabled=FALSE;

   ret = qdmPrivate_getLanWanConnObj(intfName,
                                  &wanIpConnObj, &wanPppConnObj, NULL, NULL);
   if (ret != CMSRET_SUCCESS)
   {
      /* in hybrid mode check dev2 objects */
#if defined(SUPPORT_DM_HYBRID)
      ret = qdmMulti_isIgmpProxyEnabledOnIntfNameLocked_dev2(intfName);
      if ( ret != CMSRET_SUCCESS )
      {
         return FALSE;
      }
#else
      cmsLog_debug("Could not find WAN IP/PPP obj for %s", intfName);
      return FALSE;
#endif
   }

   if (wanIpConnObj != NULL)
   {
      enabled = wanIpConnObj->X_BROADCOM_COM_IGMPEnabled;
      cmsObj_free((void **) &wanIpConnObj);
   }
   else if (wanPppConnObj != NULL)
   {
      enabled = wanPppConnObj->X_BROADCOM_COM_IGMPEnabled;
      cmsObj_free((void **) &wanPppConnObj);
   }
   return enabled;
#else
   return FALSE;
#endif
}

UBOOL8 qdmMulti_isIgmpSourceEnabledOnIntfNameLocked_igd(const char *intfName)
{
#ifdef DMP_X_BROADCOM_COM_IGMP_1
   WanIpConnObject *wanIpConnObj=NULL;
   WanPppConnObject *wanPppConnObj=NULL;
   CmsRet ret;
   UBOOL8 enabled=FALSE;

   ret = qdmPrivate_getLanWanConnObj(intfName,
                                  &wanIpConnObj, &wanPppConnObj, NULL, NULL);
   if (ret != CMSRET_SUCCESS)
   {
      /* in hybrid mode check dev2 objects */
#if defined(SUPPORT_DM_HYBRID)
      ret = qdmMulti_isIgmpSourceEnabledOnIntfNameLocked_dev2(intfName);
      if ( ret != CMSRET_SUCCESS )
      {
         return FALSE;
      }
#else
      cmsLog_debug("Could not find WAN IP/PPP obj for %s", intfName);
      return FALSE;
#endif
   }

   if (wanIpConnObj != NULL)
   {
      enabled = wanIpConnObj->X_BROADCOM_COM_IGMP_SOURCEEnabled;
      cmsObj_free((void **) &wanIpConnObj);
   }
   else if (wanPppConnObj != NULL)
   {
      enabled = wanPppConnObj->X_BROADCOM_COM_IGMP_SOURCEEnabled;
      cmsObj_free((void **) &wanPppConnObj);
   }
   return enabled;
#else
   return FALSE;
#endif
}


UBOOL8 qdmMulti_isMldProxyEnabledOnIntfNameLocked_igd(const char *intfName __attribute__((unused)))
{
#ifdef DMP_X_BROADCOM_COM_MLD_1
   WanIpConnObject *wanIpConnObj=NULL;
   WanPppConnObject *wanPppConnObj=NULL;
   CmsRet ret;
   UBOOL8 enabled=FALSE;

   ret = qdmPrivate_getLanWanConnObj(intfName,
                                 &wanIpConnObj, &wanPppConnObj, NULL, NULL);
   if (ret != CMSRET_SUCCESS)
   {
      /* in hybrid mode check dev2 objects */
#if defined(SUPPORT_DM_HYBRID)
      ret = qdmMulti_isMldProxyEnabledOnIntfNameLocked_dev2(intfName);
      if ( ret != CMSRET_SUCCESS )
      {
         return FALSE;
      }
#else
      cmsLog_debug("Could not find WAN IP/PPP obj for %s", intfName);
      return FALSE;
#endif
   }

   if (wanIpConnObj != NULL)
   {
      enabled = wanIpConnObj->X_BROADCOM_COM_MLDEnabled;
      cmsObj_free((void **) &wanIpConnObj);
   }
   else if (wanPppConnObj != NULL)
   {
      enabled = wanPppConnObj->X_BROADCOM_COM_MLDEnabled;
      cmsObj_free((void **) &wanPppConnObj);
   }

   return enabled;
#else
   return FALSE;
#endif
}


UBOOL8 qdmMulti_isMldSourceEnabledOnIntfNameLocked_igd(const char *intfName __attribute__((unused)))
{
#ifdef DMP_X_BROADCOM_COM_MLD_1
   WanIpConnObject *wanIpConnObj=NULL;
   WanPppConnObject *wanPppConnObj=NULL;
   CmsRet ret;
   UBOOL8 enabled=FALSE;

   ret = qdmPrivate_getLanWanConnObj(intfName,
                          &wanIpConnObj, &wanPppConnObj, NULL, NULL);
   if (ret != CMSRET_SUCCESS)
   {
      /* in hybrid mode check dev2 objects */
#if defined(SUPPORT_DM_HYBRID)
      ret = qdmMulti_isMldSourceEnabledOnIntfNameLocked_dev2(intfName);
      if ( ret != CMSRET_SUCCESS )
      {
         return FALSE;
      }
#else
      cmsLog_debug("Could not find WAN IP/PPP obj for %s", intfName);
      return FALSE;
#endif
   }

   if (wanIpConnObj != NULL)
   {
      enabled = wanIpConnObj->X_BROADCOM_COM_MLD_SOURCEEnabled;
      cmsObj_free((void **) &wanIpConnObj);
   }
   else if (wanPppConnObj != NULL)
   {
      enabled = wanPppConnObj->X_BROADCOM_COM_MLD_SOURCEEnabled;
      cmsObj_free((void **) &wanPppConnObj);
   }

   return enabled;
#else
   return FALSE;
#endif
}


#endif /* DMP_BASELINE_1  */

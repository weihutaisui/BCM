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
 
#ifdef DMP_DEVICE2_BASELINE_1

#include "cms.h"
#include "cms_mdm.h"
#include "cms_obj.h"
#include "cms_qdm.h"
#include "cms_util.h"



/* in qdm2_ipintf.c */
extern CmsRet qdmPrivate_getIpIntfObj(const char *ifName,
                               InstanceIdStack *iidStack,
                               Dev2IpInterfaceObject **ipIntfObj);


UBOOL8 qdmMulti_isIgmpSnoopingEnabledOnBridgeLocked_dev2(const char *bridgeIfName)
{
   CmsRet ret;
   UBOOL8 isMld = FALSE;
   UINT32 mode = 0;
   int lanToLanEnabled = 0;
   UBOOL8 enabled;

   ret = qdmMulti_getSnoopingInfoLocked_dev2(bridgeIfName, isMld,
                                        &mode, &lanToLanEnabled, &enabled);
   if (ret != CMSRET_SUCCESS)
   {
     cmsLog_error("could not find IGMP Snooping Info for bridgeIfName=%s, ret=%d", bridgeIfName, ret);
     return FALSE;
   }

   return enabled;
}


CmsRet qdmMulti_getSnoopingInfoLocked_dev2(const char *brIfName, UBOOL8 isMld,
                 UINT32 *mode, int *lanToLanEnable, UBOOL8 *enabled)
{
   CmsRet ret=CMSRET_INTERNAL_ERROR;

   cmsLog_debug("Entered: brIfName=%s isMld=%d", brIfName, isMld);

   *enabled = FALSE;

   if (!isMld)
   {
#ifdef DMP_X_BROADCOM_COM_IGMPSNOOP_1
      char *fullPath=NULL;
      MdmPathDescriptor pathDesc=EMPTY_PATH_DESCRIPTOR;
      IgmpSnoopingCfgObject *igmpSnpObj=NULL;

      /* go from brIntfName to IGMP snooping object full path */
      ret = qdmMulti_getAssociatedIgmpSnoopingFullPathLocked(brIfName, &fullPath);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_notice("getAssociatedIgmpSnooping on %s failed, ret=%d", brIfName, ret);
         return ret;
      }

      /* go from IGMP snooping obj fullpath to path descriptor */
      ret = cmsMdm_fullPathToPathDescriptor(fullPath, &pathDesc);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("fullPathToPathDesc on %s failed, ret=%d", fullPath, ret);
         CMSMEM_FREE_BUF_AND_NULL_PTR(fullPath);
         return ret;
      }

      CMSMEM_FREE_BUF_AND_NULL_PTR(fullPath);

      /* get snooping obj */
      ret = cmsObj_get(pathDesc.oid, &pathDesc.iidStack, 0, (void **) &igmpSnpObj);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("get of snoopObj failed, ret=%d", ret);
         return ret;
      }

      if (igmpSnpObj->enable)
      {
         if(cmsUtl_strcmp(igmpSnpObj->mode, MDMVS_STANDARD) == 0)
         {
            *mode = 1;
         }
         else
         {
            *mode = 2;
         }
         *enabled = TRUE;
      }
      *lanToLanEnable = igmpSnpObj->lanToLanEnable;

      cmsObj_free((void **) &igmpSnpObj);
#else
      cmsLog_error("IGMP snooping is not configured");
      ret = CMSRET_INTERNAL_ERROR;
#endif
   }
   else
   {
#ifdef DMP_X_BROADCOM_COM_MLDSNOOP_1
      char *fullPath=NULL;
      MdmPathDescriptor pathDesc=EMPTY_PATH_DESCRIPTOR;
      MldSnoopingCfgObject *mldSnpObj=NULL;

      /* go from brIntfName to MLD snooping object full path */
      ret = qdmMulti_getAssociatedMldSnoopingFullPathLocked(brIfName, &fullPath);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("getAssociatedMldSnooping on %s failed, ret=%d", brIfName, ret);
         return ret;
      }

      /* go from MLD snooping obj fullpath to path descriptor */
      ret = cmsMdm_fullPathToPathDescriptor(fullPath, &pathDesc);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("fullPathToPathDesc on %s failed, ret=%d", fullPath, ret);
         CMSMEM_FREE_BUF_AND_NULL_PTR(fullPath);
         return ret;
      }

      CMSMEM_FREE_BUF_AND_NULL_PTR(fullPath);

      /* get snooping obj */
      ret = cmsObj_get(pathDesc.oid, &pathDesc.iidStack, 0, (void **) &mldSnpObj);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("get of snoopObj failed, ret=%d", ret);
         return ret;
      }

      if (mldSnpObj->enable)
      {
         if(cmsUtl_strcmp(mldSnpObj->mode, MDMVS_STANDARD) == 0)
         {
            *mode = 1;
         }
         else
         {
            *mode = 2;
         }
         *enabled = TRUE;
      }
      *lanToLanEnable = mldSnpObj->lanToLanEnable;

      cmsObj_free((void **) &mldSnpObj);
#else
      cmsLog_error("MLD snooping is not configured");
      ret = CMSRET_INTERNAL_ERROR;
#endif
   }

   cmsLog_debug("[%s] return enable=%d mode=%d lan2lan=%d",
                 brIfName, *enabled, *mode, *lanToLanEnable);

   return ret;
}


CmsRet qdmMulti_getAssociatedBridgeIntfNameLocked_dev2(const char *snoopFullPath,
                                                      char *bridgeIntfName)
{
   Dev2BridgeObject *brObj=NULL;
   MdmPathDescriptor pathDesc=EMPTY_PATH_DESCRIPTOR;
   CmsRet ret;

   /*
    * First convert from snooping obj fullpath to path descriptor.  Note
    * we don't care whether this fullpath points to an IGMP or MLD snooping
    * obj.  They will both yield the required iidStack which is used to
    * get the associated bridge object.
    */
   ret = cmsMdm_fullPathToPathDescriptor(snoopFullPath, &pathDesc);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not convert %s, ret=%d", snoopFullPath, ret);
      return ret;
   }

   /*
    * In TR181, the IGMP and MLD snoop objects are TYPE 1 objects attached
    * to the bridge object.  So we can get the bridge object directly.
    */

   ret = cmsObj_get(MDMOID_DEV2_BRIDGE, &pathDesc.iidStack, 0, (void **) &brObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get bridge obj, ret=%d", ret);
   }
   else
   {
      if (brObj->X_BROADCOM_COM_IfName == NULL)
      {
         /* this could happen when the Bridge object is getting created
          * and the Igmp and Mld snoopingConfig subtrees are also getting
          * automatically created.  Don't worry about it.  igmp and mld
          * will be updated again when the bridge is enabled.
          */
         cmsLog_debug("Bridge object still has NULL ifname");
      }
      else
      {
         strcpy(bridgeIntfName, brObj->X_BROADCOM_COM_IfName);
      }

      cmsObj_free((void **)&brObj);
   }

   return ret;
}

CmsRet qdmMulti_getAssociatedIgmpSnoopingFullPathLocked_dev2(const char *brIntfName,
                                                            char **fullPath)
{
   Dev2BridgeObject *brObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found=FALSE;
   CmsRet ret;

   while (!found &&
          (ret = cmsObj_getNextFlags(MDMOID_DEV2_BRIDGE, &iidStack,
                                     OGF_NO_VALUE_UPDATE,
                                     (void **)&brObj)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(brIntfName, brObj->X_BROADCOM_COM_IfName))
      {
         MdmPathDescriptor pathDesc=EMPTY_PATH_DESCRIPTOR;

         pathDesc.oid = MDMOID_IGMP_SNOOPING_CFG;
         /*
          * In TR181, IGMP snooping is at the same level as bridge, so
          * I can use the exact iidStack to convert to fullpath.
          */
         pathDesc.iidStack = iidStack;
         ret = cmsMdm_pathDescriptorToFullPath(&pathDesc, fullPath);
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsMdm_pathDescToFullPath failed, ret=%d", ret);
         }

         found = TRUE;
      }
      cmsObj_free((void **)&brObj);
   }

   if (!found)
   {
      cmsLog_notice("Could not find brIntfName=%s", brIntfName);
   }

   return ret;
}


#ifdef DMP_X_BROADCOM_COM_MLDSNOOP_1
CmsRet qdmMulti_getAssociatedMldSnoopingFullPathLocked_dev2(const char *brIntfName,
                                                            char **fullPath)
{
   Dev2BridgeObject *brObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found=FALSE;
   CmsRet ret;

   while (!found &&
          (ret = cmsObj_getNextFlags(MDMOID_DEV2_BRIDGE, &iidStack,
                                     OGF_NO_VALUE_UPDATE,
                                     (void **)&brObj)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(brIntfName, brObj->X_BROADCOM_COM_IfName))
      {
         MdmPathDescriptor pathDesc=EMPTY_PATH_DESCRIPTOR;

         pathDesc.oid = MDMOID_MLD_SNOOPING_CFG;
         /*
          * In TR181, MLD snooping is at the same level as bridge, so
          * I can use the exact iidStack to convert to fullpath.
          */
         pathDesc.iidStack = iidStack;
         ret = cmsMdm_pathDescriptorToFullPath(&pathDesc, fullPath);
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsMdm_pathDescToFullPath failed, ret=%d", ret);
         }

         found = TRUE;
      }
      cmsObj_free((void **)&brObj);
   }

   if (!found)
   {
      cmsLog_notice("Could not find brIntfName=%s", brIntfName);
   }

   return ret;
}
#endif


UBOOL8 qdmMulti_isIgmpProxyEnabledOnIntfNameLocked_dev2(const char *intfName __attribute__((unused)))
{
#ifdef DMP_X_BROADCOM_COM_IGMP_1
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2IpInterfaceObject *ipIntfObj=NULL;
   CmsRet ret;
   UBOOL8 enabled=FALSE;

   ret = qdmPrivate_getIpIntfObj(intfName, &iidStack, &ipIntfObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_debug("Could not find IP.Interface obj for %s", intfName);
      return FALSE;
   }

   enabled = ipIntfObj->X_BROADCOM_COM_IGMPEnabled;

   cmsObj_free((void **) &ipIntfObj);

   return enabled;
#else
   /* If IGMP proxy not enabled, the param is not in object, so don't even
    * bother querying the MDM.  Just return FALSE.
    */
   return FALSE;
#endif
}

UBOOL8 qdmMulti_isIgmpSourceEnabledOnIntfNameLocked_dev2(const char *intfName __attribute__((unused)))
{
#ifdef DMP_X_BROADCOM_COM_IGMP_1
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2IpInterfaceObject *ipIntfObj=NULL;
   CmsRet ret;
   UBOOL8 enabled=FALSE;

   ret = qdmPrivate_getIpIntfObj(intfName, &iidStack, &ipIntfObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_debug("Could not find IP.Interface obj for %s", intfName);
      return FALSE;
   }

   enabled = ipIntfObj->X_BROADCOM_COM_IGMP_SOURCEEnabled;

   cmsObj_free((void **) &ipIntfObj);

   return enabled;
#else
   /* If IGMP proxy not enabled, the param is not in object, so don't even
    * bother querying the MDM.  Just return FALSE.
    */
   return FALSE;
#endif
}


UBOOL8 qdmMulti_isMldProxyEnabledOnIntfNameLocked_dev2(const char *intfName __attribute__((unused)))
{
#ifdef DMP_X_BROADCOM_COM_MLD_1
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2IpInterfaceObject *ipIntfObj=NULL;
   CmsRet ret;
   UBOOL8 enabled=FALSE;

   ret = qdmPrivate_getIpIntfObj(intfName, &iidStack, &ipIntfObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_debug("Could not find IP.Interface obj for %s", intfName);
      return FALSE;
   }

   enabled = ipIntfObj->X_BROADCOM_COM_MLDEnabled;

   cmsObj_free((void **) &ipIntfObj);

   return enabled;
#else
   /* If MLD proxy not enabled, the param is not in object, so don't even
    * bother querying the MDM.  Just return FALSE.
    */
   return FALSE;
#endif
}


UBOOL8 qdmMulti_isMldSourceEnabledOnIntfNameLocked_dev2(const char *intfName __attribute__((unused)))
{
#ifdef DMP_X_BROADCOM_COM_MLD_1
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2IpInterfaceObject *ipIntfObj=NULL;
   CmsRet ret;
   UBOOL8 enabled=FALSE;

   ret = qdmPrivate_getIpIntfObj(intfName, &iidStack, &ipIntfObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_debug("Could not find IpIntfObj for %s", intfName);
      return FALSE;
   }

   enabled = ipIntfObj->X_BROADCOM_COM_MLD_SOURCEEnabled;

   cmsObj_free((void **) &ipIntfObj);

   return enabled;
#else
   /* If MLD proxy not enabled, the param is not in object, so don't even
    * bother querying the MDM.  Just return FALSE.
    */
   return FALSE;
#endif
}


#endif /* DMP_DEVICE2_BASELINE_1 */


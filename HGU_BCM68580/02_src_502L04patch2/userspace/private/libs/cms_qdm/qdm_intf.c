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

#include "cms.h"
#include "cms_mdm.h"
#include "cms_obj.h"
#include "cms_qdm.h"
#include "cms_util.h"

#ifdef DMP_BASELINE_1  

/* this is how it was done in dal_main.c, which calls the function in
 * rut_util.c to do the for for the IGD data model. Preserve the existing
 * code, even though it would be better to just implement the function here.
 */

extern CmsRet rut_intfnameToFullPath(const char *intfname, UBOOL8 layer2, char **mdmPath);
CmsRet qdmIntf_intfnameToFullPathLocked_igd(const char *intfname, UBOOL8 layer2, char **mdmPath)
{
   return rut_intfnameToFullPath(intfname, layer2, mdmPath);
}


extern CmsRet rut_fullPathToIntfname(const char *mdmPath, char *intfname);
CmsRet qdmIntf_fullPathToIntfnameLocked_igd(const char *mdmPath, char *intfname)
{
   return rut_fullPathToIntfname(mdmPath, intfname);
}


UBOOL8 qdmIntf_isLayer2IntfNameUpstreamLocked_igd(const char *l2IntfName)
{
    char *fullPath=NULL;
    UBOOL8 isWan = FALSE;
    UBOOL8 isLayer2 = TRUE;
    CmsRet ret;

    if ((ret = qdmIntf_intfnameToFullPathLocked(l2IntfName, isLayer2, &fullPath)) != CMSRET_SUCCESS)
    {
       cmsLog_error("intfnameToFullPath on %s returned %d", l2IntfName, ret);
       return FALSE;
    }

    /* XXX What about Wlan and USB ? */
    if ((cmsUtl_strstr(fullPath, "WANEthernetInterfaceConfig") != NULL) ||
        (cmsUtl_strstr(fullPath, "WANDSLLinkConfig") != NULL) ||
        (cmsUtl_strstr(fullPath, "WANPTMLinkConfig") != NULL) ||
        (cmsUtl_strstr(fullPath, "WANGPONLinkConfig") != NULL) ||
        (cmsUtl_strstr(fullPath, "WANEPONLinkConfig") != NULL) ||
        (cmsUtl_strstr(fullPath, "_EponInterfaceConfig") != NULL))
    {
       isWan = TRUE;
    }

    CMSMEM_FREE_BUF_AND_NULL_PTR(fullPath);

    return isWan;
}

extern CmsRet rutPMap_getBridgeKey(const char *bridgeName, UINT32 *bridgeKey);
CmsRet qdmIntf_getIntfKeyByGroupName_igd(char *bridgeName, UINT32 *bridgeKey)
{
    CmsRet ret = CMSRET_SUCCESS;
    if ((ret = rutPMap_getBridgeKey(bridgeName, bridgeKey)) != CMSRET_SUCCESS)
    {
       cmsLog_notice("cannot find bridge %s, ret=%d", bridgeName, ret);
    }
    return ret;
}

extern CmsRet rutPMap_getBridgeByKey(UINT32 bridgeKey, InstanceIdStack *iidStack, L2BridgingEntryObject **bridgeObj);
CmsRet qdmIntf_getIntfGroupNameByBrKey_igd(char *bridgeName, UINT32 bridgeKey)
{
    CmsRet ret = CMSRET_SUCCESS;
    InstanceIdStack brIidStack = EMPTY_INSTANCE_ID_STACK;
    L2BridgingEntryObject *bridgeObj = NULL;

    INIT_INSTANCE_ID_STACK(&brIidStack);
    if ((ret = rutPMap_getBridgeByKey(bridgeKey, &brIidStack, &bridgeObj)) == CMSRET_SUCCESS)
    {
        strcpy(bridgeName, bridgeObj->bridgeName);
        cmsObj_free((void **) &bridgeObj);
    }
    return ret;
}



UBOOL8 qdmIntf_isInterfaceWANOnly_igd(const char *ifName)
{
   UBOOL8 found = FALSE;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanEthIntfObject *lanEthObj = NULL;

   while (!found &&
          (cmsObj_getNextFlags(MDMOID_LAN_ETH_INTF, &iidStack, OGF_NO_VALUE_UPDATE, (void **) &lanEthObj) == CMSRET_SUCCESS))
   {
      if (!cmsUtl_strcmp(lanEthObj->X_BROADCOM_COM_IfName, ifName)  && 
          !cmsUtl_strcmp(lanEthObj->X_BROADCOM_COM_WanLan_Attribute, MDMVS_WANONLY))
      {
         found = TRUE;
      }
      cmsObj_free((void **) &lanEthObj);
   }


   if (!found)
   {
      WanEthIntfObject *wanEthIntfObj = NULL;

      INIT_INSTANCE_ID_STACK(&iidStack);
      while (!found &&
             (cmsObj_getNextFlags(MDMOID_WAN_ETH_INTF, &iidStack, OGF_NO_VALUE_UPDATE, (void **) &wanEthIntfObj) == CMSRET_SUCCESS))
      {
         if (!cmsUtl_strcmp(wanEthIntfObj->X_BROADCOM_COM_IfName, ifName)  && 
             !cmsUtl_strcmp(wanEthIntfObj->X_BROADCOM_COM_WanLan_Attribute, MDMVS_WANONLY))
         {
            found = TRUE;
         }
         cmsObj_free((void **) &wanEthIntfObj);
      }      
   }

   return found;
}

#endif /* DMP_BASELINE_1  */

/* resolve beep:pmd depency symbol ld issue in run time for Hybrid,TR181
 * if release binary EXE to customer we need keep CMS QDM API outside DMP_BASELINE_1 support both mode */
CmsRet qdmIntf_getIntfKeyByGroupName(char *brName, UINT32 *brKey)
{
#if defined(SUPPORT_DM_LEGACY98)
   return qdmIntf_getIntfKeyByGroupName_igd(brName, brKey);
#elif defined(SUPPORT_DM_HYBRID)
   return qdmIntf_getIntfKeyByGroupName_igd(brName, brKey);
#elif defined(SUPPORT_DM_PURE181)
   return qdmIntf_getIntfKeyByGroupName_dev2(brName, brKey);
#elif defined(SUPPORT_DM_DETECT)
   if (cmsMdm_isDataModelDevice2())
      return qdmIntf_getIntfKeyByGroupName_dev2(brName, brKey);
   else
      return qdmIntf_getIntfKeyByGroupName_igd(brName, brKey);
#endif
}


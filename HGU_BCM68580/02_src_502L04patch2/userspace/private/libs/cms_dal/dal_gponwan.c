/***********************************************************************
 *
 *  Copyright (c) 2010  Broadcom Corporation
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
#include "dal.h"

#ifdef DMP_X_BROADCOM_COM_GPONWAN_1

extern CmsRet rutWl2_getGponWanIidStack(InstanceIdStack *gponWanIidStack);
extern CmsRet rutGpon_getGponLinkByIfName(char *ifName, InstanceIdStack *iidStack, WanGponLinkCfgObject **gponLinkCfg);
extern CmsRet rutGpon_getServiceOidAndIidStack(const InstanceIdStack *gponLinkCfgIid, MdmObjectId *oid, InstanceIdStack *iidStack);
extern CmsRet rutGpon_getWanServiceParams(const MdmObjectId oid, const InstanceIdStack *iidStack, GponWanServiceParams *pServiceParams);
extern CmsRet rutGpon_getWanServiceL2IfName(const MdmObjectId oid, const InstanceIdStack *iidStack, char *pL2Ifname);

CmsRet dalGpon_getGponWanIidStatck(InstanceIdStack *gponWanIidStack)
{
   return (rutWl2_getGponWanIidStack(gponWanIidStack));
}

#ifdef SUPPORT_DM_PURE181
CmsRet dalGpon_configGponInterface_dev2(const WEB_NTWK_VAR *webVar, UBOOL8 isAdd)
{
    CmsRet ret = CMSRET_SUCCESS;
    OpticalInterfaceObject *optIntfObj = NULL;
    InstanceIdStack optIntfIid = EMPTY_INSTANCE_ID_STACK;

    cmsLog_debug("isAdd=%d (1=Add, 0=Del) ifName=%s", isAdd, webVar->wanL2IfName);

    if (dalOptical_getIntfByIfNameEnabled(webVar->wanL2IfName, &optIntfIid, &optIntfObj, !isAdd) == FALSE)
        return CMSRET_INTERNAL_ERROR;

    optIntfObj->enable = isAdd;

    if ((ret = cmsObj_set(optIntfObj, &optIntfIid)) != CMSRET_SUCCESS)
        cmsLog_error("Failed to set OpticalInterfaceObject, ret = %d", ret);

    cmsObj_free((void **) &optIntfObj);

    return ret;
}
#else
CmsRet dalGpon_configGponInterface_igd(const WEB_NTWK_VAR *webVar, UBOOL8 isAdd)
{
   InstanceIdStack gponWanIid = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack gponLinkIid = EMPTY_INSTANCE_ID_STACK;   
   WanGponLinkCfgObject *gponLinkCfg = NULL;
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 done = FALSE;

   /* Get Gpon WanDevice iidStack first */      
   if ((ret = dalGpon_getGponWanIidStatck(&gponWanIid)) != CMSRET_SUCCESS)
   {
      return ret;
   }

   cmsLog_debug("isAdd=%d (1=Add, 0=Del) ifName=%s", isAdd, webVar->wanL2IfName);

   /* go over all the wan gpon link config objects to find a match on the layer 2 ifName and 
   * perform the action according to isAdd flag
   */
   while (!done && (ret = cmsObj_getNextInSubTreeFlags
      (MDMOID_WAN_GPON_LINK_CFG, &gponWanIid, &gponLinkIid, OGF_NO_VALUE_UPDATE, (void **)&gponLinkCfg)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(gponLinkCfg->ifName, webVar->wanL2IfName))
      {
         /* found the matcing gpon wan interface */
         if (isAdd && gponLinkCfg->enable)
         {
            cmsLog_error("%s is already added.");
            ret = CMSRET_INTERNAL_ERROR;
            done = TRUE;
         }
         else if (!isAdd && !gponLinkCfg->enable)
         {
            cmsLog_error("%s is already removed..");
            ret = CMSRET_INTERNAL_ERROR;
            done = TRUE;
         }
         else
         {
            if (isAdd)
            {
               /* For add case, set gpon link config object to enable so that it is considereed configured */
               gponLinkCfg->enable = TRUE;
               
               /* set connection mode value */
               if (webVar->connMode == CMS_CONNECTION_MODE_VLANMUX)
               {
                  CMSMEM_REPLACE_STRING(gponLinkCfg->connectionMode, MDMVS_VLANMUXMODE);
               }
            }
            else
            {
               WanConnDeviceObject *wanConn=NULL;
            
               /* For delete case, need to check if there are any services left on the WanConnectionDevice.
                * If no service left, disable the gponLinkCfg object.
                */
               if ((ret = cmsObj_get(MDMOID_WAN_CONN_DEVICE, &gponLinkIid, 0, (void **) &wanConn)) != CMSRET_SUCCESS)
               {
                  cmsLog_error("failed to get WanConnDev, ret=%d", ret);
                  done = TRUE;
               }
               else
               {
                  if (wanConn->WANIPConnectionNumberOfEntries == 0 &&
                      wanConn->WANPPPConnectionNumberOfEntries == 0)
                  {
                     /* For gpon wan delete case, just set enable=FALSE and restore to default connection mode */
                     gponLinkCfg->enable = FALSE;
                     CMSMEM_REPLACE_STRING(gponLinkCfg->connectionMode, MDMVS_DEFAULTMODE);
                  }
                  else
                  {
                     cmsLog_debug("WanConnDevice is still in use and cannot be removed");
                     ret = CMSRET_REQUEST_DENIED;
                     done = TRUE;
                  }
               }
            }

            /* do the gponlinkCfg set only if not done */
            if (!done)
            {
               if ((ret = cmsObj_set(gponLinkCfg, &gponLinkIid)) != CMSRET_SUCCESS)
               {
                  cmsLog_error("Failed to set gponLinkCfg, ret = %d", ret);
               }
               done = TRUE;               
            }
         }
      }
      
      cmsObj_free((void **)&gponLinkCfg);
      
   }

   cmsLog_debug("Exit, ret=%d", ret);
   
   return ret;
}
#endif

CmsRet dalGpon_configGponInterface(const WEB_NTWK_VAR *webVar, UBOOL8 isAdd)
{
#ifdef SUPPORT_DM_PURE181
    return dalGpon_configGponInterface_dev2(webVar, isAdd);
#else
    return dalGpon_configGponInterface_igd(webVar, isAdd);
#endif
}

UBOOL8 dalGpon_getGponLinkByIfName(char *ifName, InstanceIdStack *iidStack, WanGponLinkCfgObject **gponLinkCfg)
{
   return (rutGpon_getGponLinkByIfName(ifName, iidStack, gponLinkCfg));
}


CmsRet dalGpon_getServiceOidAndIidStack(const InstanceIdStack *gponLinkCfgIid,
                                        MdmObjectId *oid, InstanceIdStack *iidStack)
{
   return rutGpon_getServiceOidAndIidStack(gponLinkCfgIid,oid,iidStack);
}


CmsRet dalGpon_getWanServiceParams(const MdmObjectId oid, const InstanceIdStack *iidStack,
                                   GponWanServiceParams *pServiceParams)
{
   return rutGpon_getWanServiceParams(oid,iidStack,pServiceParams);
}

CmsRet dalGpon_getWanServiceL2IfName(const MdmObjectId oid, 
                                     const InstanceIdStack *iidStack,
                                     char *pL2Ifname)
{
   return rutGpon_getWanServiceL2IfName(oid,iidStack,pL2Ifname);
}
#endif /* DMP_X_BROADCOM_COM_GPONWAN_1 */



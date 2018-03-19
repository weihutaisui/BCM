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

#ifdef DMP_X_BROADCOM_COM_EPONWAN_1

extern CmsRet rutWl2_getEponWanIidStack(InstanceIdStack *eponWanIidStack);
extern UBOOL8 rutEpon_getEponIntfByIfName(const char *ifName, InstanceIdStack *iidStack, WanEponIntfObject**eponIntfCfg);
extern UBOOL8 rutEpon_getEponLinkByIfName(const char *ifName, InstanceIdStack *iidStack, WanEponLinkCfgObject **eponLinkCfg);

CmsRet dalEpon_getEponWanIidStatck(InstanceIdStack *eponWanIidStack)
{
   return (rutWl2_getEponWanIidStack(eponWanIidStack));
}

UBOOL8 dalEpon_getEponIntfByIfName(char *ifName, InstanceIdStack *iidStack, WanEponIntfObject **eponLIntf)
{
   return (rutEpon_getEponIntfByIfName(ifName, iidStack, eponLIntf));
}

UBOOL8 dalEpon_getEponLinkByIfName(char *ifName, InstanceIdStack *iidStack, WanEponLinkCfgObject **eponLinkCfg)
{
   return (rutEpon_getEponLinkByIfName(ifName, iidStack, eponLinkCfg));
}

#ifdef SUPPORT_DM_PURE181
static CmsRet dalEpon_addEponInterface_dev2(const WEB_NTWK_VAR *webVar)
{
    CmsRet ret = CMSRET_SUCCESS;
    OpticalInterfaceObject *optIntfObj = NULL;
    InstanceIdStack optIntfIid = EMPTY_INSTANCE_ID_STACK;

    cmsLog_debug("ifName=%s connMode=%d", webVar->wanL2IfName, webVar->connMode);

    if (dalOptical_getIntfByIfNameEnabled(webVar->wanL2IfName, &optIntfIid, &optIntfObj, FALSE) == FALSE)
        return CMSRET_INTERNAL_ERROR;

    optIntfObj->enable = TRUE;

    if ((ret = cmsObj_set(optIntfObj, &optIntfIid)) != CMSRET_SUCCESS)
        cmsLog_error("Failed to set OpticalInterfaceObject, ret = %d", ret);

    cmsObj_free((void **) &optIntfObj);

    return ret;
}
#else
static CmsRet dalEpon_addEponInterface_igd(const WEB_NTWK_VAR *webVar)
{
   InstanceIdStack eponWanIid = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;
   
#ifdef EPON_SFU
   WanEponIntfObject *wanEponObj = NULL;
   /*
     * Find the WANDevice that our epon interface was created under.
     */
   if (!dalEpon_getEponIntfByIfName((char *) webVar->wanL2IfName, &eponWanIid, &wanEponObj))
   {
      return CMSRET_INTERNAL_ERROR;
   }

   /* set connection mode value */
   if (webVar->connMode == CMS_CONNECTION_MODE_VLANMUX)
   {
      CMSMEM_REPLACE_STRING(wanEponObj->connectionMode, MDMVS_VLANMUXMODE);
   }
   else
   {
      CMSMEM_REPLACE_STRING(wanEponObj->connectionMode, MDMVS_DEFAULTMODE);
   }

   wanEponObj->enable = TRUE;
              
   if ((ret = cmsObj_set(wanEponObj, &eponWanIid)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set WanEponIntfObject, ret = %d", ret);
   }
   cmsObj_free((void **) &wanEponObj);


   /*
     * Also create a single WANConnectionDevice in this WANDevice.
     */
   cmsLog_debug("create a single WANConnectionDevice in this EponWAN device");
   if ((ret = cmsObj_addInstance(MDMOID_WAN_CONN_DEVICE, &eponWanIid)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create new WanConnectionDevice, ret=%d", ret);
      return ret;
   }
#else
   InstanceIdStack eponLinkIid = EMPTY_INSTANCE_ID_STACK;   
   WanEponLinkCfgObject *eponLinkCfg = NULL;
   UBOOL8 done = FALSE;
   /* Get Epon WanDevice iidStack first */      
   if ((ret = dalEpon_getEponWanIidStatck(&eponWanIid)) != CMSRET_SUCCESS)
   {
      return ret;
   }

   /* go over all the wan epon link config objects to find a match on the layer 2 ifName */
   while (!done && (ret = cmsObj_getNextInSubTreeFlags
      (MDMOID_WAN_EPON_LINK_CFG, &eponWanIid, &eponLinkIid, OGF_NO_VALUE_UPDATE, (void **)&eponLinkCfg)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(eponLinkCfg->ifName, webVar->wanL2IfName))
      {
         if (eponLinkCfg->enable)
         {
            cmsLog_error("epon wan conn device already added.");
            ret = CMSRET_INTERNAL_ERROR;
         }
         else
         {
            /* For add case, set epon link config object to enable so that it is considereed configured */
            eponLinkCfg->enable = TRUE;
               
            /* set connection mode value */
            if (webVar->connMode == CMS_CONNECTION_MODE_VLANMUX)
            {
               CMSMEM_REPLACE_STRING(eponLinkCfg->connectionMode, MDMVS_VLANMUXMODE);
            }
            
            if ((ret = cmsObj_set(eponLinkCfg, &eponLinkIid)) != CMSRET_SUCCESS)
            {
               cmsLog_error("Failed to set eponLinkCfg, ret = %d", ret);
            }
         }
         done = TRUE;               
      }
      cmsObj_free((void **)&eponLinkCfg);
   }
#endif
   cmsLog_debug("Exit, ret=%d", ret);
   
   return ret;
}
#endif

CmsRet dalEpon_addEponInterface(const WEB_NTWK_VAR *webVar)
{
#ifdef SUPPORT_DM_PURE181
    return dalEpon_addEponInterface_dev2(webVar);
#else
    return dalEpon_addEponInterface_igd(webVar);
#endif
}

#ifdef SUPPORT_DM_PURE181
static CmsRet dalEpon_deleteEponInterface_dev2(const WEB_NTWK_VAR *webVar)
{
    CmsRet ret = CMSRET_SUCCESS;
    OpticalInterfaceObject *optIntfObj = NULL;
    InstanceIdStack optIntfIid = EMPTY_INSTANCE_ID_STACK;

    cmsLog_debug("ifName=%s", webVar->wanL2IfName);

    if (dalOptical_getIntfByIfNameEnabled(webVar->wanL2IfName, &optIntfIid, &optIntfObj, TRUE) == FALSE)
        return CMSRET_INTERNAL_ERROR;

    optIntfObj->enable = FALSE;

    if ((ret = cmsObj_set(optIntfObj, &optIntfIid)) != CMSRET_SUCCESS)
        cmsLog_error("Failed to set OpticalInterfaceObject, ret = %d", ret);

    cmsObj_free((void **) &optIntfObj);

    return ret;
}
#else
static CmsRet dalEpon_deleteEponInterface_igd(const WEB_NTWK_VAR *webVar)
{
   InstanceIdStack eponWanIid = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;
   
#ifdef EPON_SFU
   WanEponIntfObject *wanEponObj = NULL;
   InstanceIdStack wanConnDevdStack = EMPTY_INSTANCE_ID_STACK;
   WanConnDeviceObject *wanConn=NULL;
   InstanceIdStack saveIid = EMPTY_INSTANCE_ID_STACK;
   /*
     * Find the WANDevice that our epon interface was created under.
     */
   if (!dalEpon_getEponIntfByIfName((char *) webVar->wanL2IfName, &eponWanIid, &wanEponObj))
   {
      return CMSRET_INTERNAL_ERROR;
   }

   saveIid = eponWanIid;
   
   wanEponObj->enable = FALSE;
              
   if ((ret = cmsObj_set(wanEponObj, &eponWanIid)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set WanEponIntfObject, ret = %d", ret);
   }
   cmsObj_free((void **) &wanEponObj);

   /*
    * Finally, check if there are any services left on the WanConnectionDevice.
    * If not, delete the entire WanConnectionDevice sub-tree, which includes
    * the DslLinkCfg instance.
    */
   eponWanIid = saveIid;
   ret = cmsObj_getNextInSubTree(MDMOID_WAN_CONN_DEVICE, &eponWanIid, &wanConnDevdStack, (void **) &wanConn);
   if (ret == CMSRET_NO_MORE_INSTANCES)
   {
      cmsLog_debug("No WanConnDev created yet, ret=%d", ret);
      return CMSRET_SUCCESS;
   }
   else if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get WanConnDev, ret=%d", ret);
      return ret;
   }
   
   /* we don't need to del  obj MDMOID_WAN_CONN_DEVICE here,
    * it be deleted in rcl_interfaceControlObject when eth move to lan.
    */
   if (wanConn->WANIPConnectionNumberOfEntries != 0 ||
       wanConn->WANPPPConnectionNumberOfEntries != 0)
   {
      cmsLog_debug("WanConnDevice is still in use and cannot be removed");
      cmsObj_free((void **) &wanConn);
      return CMSRET_REQUEST_DENIED;
   }
   else
   {  
      cmsLog_debug("WanConnDevice is removed");
      if ((ret = cmsObj_deleteInstance(MDMOID_WAN_CONN_DEVICE, &wanConnDevdStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to delete WanConnDev, ret=%d", ret);
      }
   }
   cmsObj_free((void **) &wanConn);
#else
   InstanceIdStack eponLinkIid = EMPTY_INSTANCE_ID_STACK;   
   WanEponLinkCfgObject *eponLinkCfg = NULL;
   UBOOL8 done = FALSE;
   /* Get Epon WanDevice iidStack first */      
   if ((ret = dalEpon_getEponWanIidStatck(&eponWanIid)) != CMSRET_SUCCESS)
   {
      return ret;
   }

   /* go over all the wan epon link config objects to find a match on the layer 2 ifName*/
   while (!done && (ret = cmsObj_getNextInSubTreeFlags
      (MDMOID_WAN_EPON_LINK_CFG, &eponWanIid, &eponLinkIid, OGF_NO_VALUE_UPDATE, (void **)&eponLinkCfg)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(eponLinkCfg->ifName, webVar->wanL2IfName))
      {
         if (!eponLinkCfg->enable)
         {
            cmsLog_error("epon wan conn device is already removed..");
            ret = CMSRET_INTERNAL_ERROR;
            done = TRUE;
         }
         else
         {
            WanConnDeviceObject *wanConn=NULL;
            
            /* For delete case, need to check if there are any services left on the WanConnectionDevice.
                    * If no service left, disable the eponLinkCfg object.
                    */
            if ((ret = cmsObj_get(MDMOID_WAN_CONN_DEVICE, &eponLinkIid, 0, (void **) &wanConn)) != CMSRET_SUCCESS)
            {
               cmsLog_error("failed to get WanConnDev, ret=%d", ret);
               done = TRUE;
            }
            else
            {
               if (wanConn->WANIPConnectionNumberOfEntries == 0 &&
                   wanConn->WANPPPConnectionNumberOfEntries == 0)
               {
                  /* For epon wan delete case, just set enable=FALSE and restore to default connection mode */
                  eponLinkCfg->enable = FALSE;
                  CMSMEM_REPLACE_STRING(eponLinkCfg->connectionMode, MDMVS_DEFAULTMODE);
               }
               else
               {
                  cmsLog_debug("WanConnDevice is still in use and cannot be removed");
                  ret = CMSRET_REQUEST_DENIED;
                  done = TRUE;
               }
            }
            /* do the eponlinkCfg set only if not done */
            if (!done)
            {
               if ((ret = cmsObj_set(eponLinkCfg, &eponLinkIid)) != CMSRET_SUCCESS)
               {
                  cmsLog_error("Failed to set eponLinkCfg, ret = %d", ret);
               }
               done = TRUE;               
            }
         }
      }
    }
#endif
   cmsLog_debug("Exit, ret=%d", ret);
   
   return ret;
}   
#endif

CmsRet dalEpon_deleteEponInterface(const WEB_NTWK_VAR *webVar)
{
#ifdef SUPPORT_DM_PURE181
    return dalEpon_deleteEponInterface_dev2(webVar);
#else
    return dalEpon_deleteEponInterface_igd(webVar);
#endif
}

#endif /* DMP_X_BROADCOM_COM_EPONWAN_1 */



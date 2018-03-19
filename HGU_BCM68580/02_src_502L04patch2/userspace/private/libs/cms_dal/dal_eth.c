/***********************************************************************
 *
 *  Copyright (c) 2008  Broadcom Corporation
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

#ifdef DMP_BASELINE_1
#ifdef DMP_ETHERNETWAN_1
extern UBOOL8 rutEth_getEthIntfByIfName(char *ifName, InstanceIdStack *iidStack, WanEthIntfObject **ethIntfCfg);


UBOOL8 dalEth_getEthIntfByIfName(char *ifName, InstanceIdStack *iidStack, WanEthIntfObject **ethIntfCfg)
{
   return (rutEth_getEthIntfByIfName(ifName, iidStack, ethIntfCfg));
}

CmsRet dalEth_addEthInterface_igd(const WEB_NTWK_VAR *webVar)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 existingEthFound;
   WanEthIntfObject *wanEthObj = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("wanL2IfName=%s", webVar->wanL2IfName);

   existingEthFound = dalEth_getEthIntfByIfName((char *)webVar->wanL2IfName, &iidStack, NULL);
   if (existingEthFound)
   {
      cmsLog_error("An ETH interface is already in use as WAN Interface");
      return CMSRET_INTERNAL_ERROR;
   }

   {
      UBOOL8 moveToLan = FALSE;
      UBOOL8 moveToWan = TRUE;

      ret = cmsDal_moveIntfLanWan(webVar->wanL2IfName, moveToLan, moveToWan);
      if (ret != CMSRET_SUCCESS)
      {
         return ret;
      }
   }

   /*
    * Find the WANDevice that our eth interface was created under.
    */
   INIT_INSTANCE_ID_STACK(&iidStack);
   existingEthFound = dalEth_getEthIntfByIfName((char *) webVar->wanL2IfName, &iidStack, &wanEthObj);
   if (!existingEthFound)
   {
      cmsLog_error("could not find the ethWan object!");
      return CMSRET_INTERNAL_ERROR;
   }

   cmsLog_debug("WAN %s created at %s", (char *) webVar->wanL2IfName, cmsMdm_dumpIidStack(&iidStack));


   /* set connection mode value */
   if (webVar->connMode == CMS_CONNECTION_MODE_VLANMUX)
   {
      CMSMEM_REPLACE_STRING(wanEthObj->X_BROADCOM_COM_ConnectionMode, MDMVS_VLANMUXMODE);
   }
   else
   {
      CMSMEM_REPLACE_STRING(wanEthObj->X_BROADCOM_COM_ConnectionMode, MDMVS_DEFAULTMODE);
   }

   wanEthObj->enable = TRUE;

   ret = cmsObj_set(wanEthObj, &iidStack);
   cmsObj_free((void **) &wanEthObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set WanEthIntfObject, ret = %d", ret);
      return ret;
   }


   /*
    * Also create a single WANConnectionDevice in this WANDevice.
    */
   if ((ret = cmsObj_addInstance(MDMOID_WAN_CONN_DEVICE, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create new WanConnectionDevice, ret=%d", ret);
      return ret;
   }


   cmsLog_debug("Exit, ret=%d", ret);
   
   return ret;
}


CmsRet dalEth_deleteEthInterface_igd(const WEB_NTWK_VAR *webVar)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack wanConnDevdStack = EMPTY_INSTANCE_ID_STACK;
   WanConnDeviceObject *wanConn=NULL;
   LanEthIntfObject *lanEthObj=NULL;
   CmsRet ret = CMSRET_SUCCESS;
   
   cmsLog_debug("Deleting %s", webVar->wanL2IfName);

   if (dalEth_getEthIntfByIfName((char *)webVar->wanL2IfName, &iidStack, NULL) == FALSE)
   {
      cmsLog_debug("Interface %s not found.", webVar->wanL2IfName);
      return CMSRET_INTERNAL_ERROR;
   }

   /*
    * Finally, check if there are any services left on the WanConnectionDevice.
    * If not, delete the entire WanConnectionDevice sub-tree, which includes
    * the DslLinkCfg instance.
    */
   if ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_CONN_DEVICE, &iidStack, &wanConnDevdStack, (void **) &wanConn)) != CMSRET_SUCCESS)
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

   cmsObj_free((void **) &wanConn);


   {
      UBOOL8 moveToLan = TRUE;
      UBOOL8 moveToWan = FALSE;

      ret = cmsDal_moveIntfLanWan(webVar->wanL2IfName, moveToLan, moveToWan);
      if (ret != CMSRET_SUCCESS)
      {
         return ret;
      }
   }


   /*
    * To be symetrical with the move to WAN side case, I also need to enable the
    * eth interface that was just moved back to the LAN side.
    */
   INIT_INSTANCE_ID_STACK(&iidStack);
   ret = dalLan_getEthInterface(webVar->wanL2IfName, &iidStack, &lanEthObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get the LAN eth obj that was moved back, ret=%d", ret);
   }
   else
   {
      lanEthObj->enable = TRUE;
      cmsObj_set(lanEthObj, &iidStack);
      cmsObj_free((void **) &lanEthObj);
   }

   cmsLog_debug("Exit, ret=%d", ret);
   
   return ret;
}
#endif  /* DMP_ETHERNETWAN_1 */
#endif  /* DMP_BASELINE_1 */

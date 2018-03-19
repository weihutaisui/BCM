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


#ifdef DMP_X_BROADCOM_COM_WIFIWAN_1


#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"
#include "dal.h"



CmsRet dalWifiWan_getAvailableL2WlIntf_igd(NameList **ifList)
{
   CmsRet            ret;
   LanDevObject      *lanDev  = NULL;
   WlVirtIntfCfgObject *wlVirtIntfCfgObj = NULL;
   InstanceIdStack   wlIid;
   InstanceIdStack   lanDevIid;
   NameList          head;
   NameList          *tail, *nameListElement;


   *ifList = NULL;
   head.name = NULL;
   head.next = NULL;
   tail = &head;

   cmsLog_debug("Entering.....\n");

   /* Look for Wifi interface under LANDev.1. */
   INIT_INSTANCE_ID_STACK(&lanDevIid);
   if ((ret = cmsObj_getNext(MDMOID_LAN_DEV, &lanDevIid, (void **)&lanDev)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_getNext <MDMOID_LAN_DEV> returns error. ret=%d", ret);
      return ret;
   }

   cmsObj_free((void **)&lanDev);   /* no longer needed */

   INIT_INSTANCE_ID_STACK(&wlIid);
   while ((ret = cmsObj_getNextInSubTree(MDMOID_WL_VIRT_INTF_CFG, &lanDevIid, &wlIid, (void **)&wlVirtIntfCfgObj)) == CMSRET_SUCCESS)
   {

      if (!wlVirtIntfCfgObj->wlEnblSsid)
      {
         cmsObj_free((void **)&wlVirtIntfCfgObj);
         continue;
      }

      nameListElement = cmsMem_alloc(sizeof(NameList), ALLOC_ZEROIZE);
      if (nameListElement == NULL)
      {
         cmsLog_error("could not allocate nameListElement");
         cmsObj_free((void **)&wlVirtIntfCfgObj);
         cmsDal_freeNameList(head.next);
         return CMSRET_RESOURCE_EXCEEDED;
      }
      /* append to name list */
      nameListElement->name = cmsMem_strdup(wlVirtIntfCfgObj->wlIfcname);
      nameListElement->next = NULL;
      tail->next = nameListElement;
      tail = nameListElement;

      cmsObj_free((void **)&wlVirtIntfCfgObj);
   }  /* while */

   if (ret != CMSRET_NO_MORE_INSTANCES)
   {
      cmsLog_error("cmsObj_getNextInSubTree returns error. ret=%d", ret);
      cmsDal_freeNameList(head.next);
      return ret;
   }

   /* return the name list */
   *ifList = head.next;

   return CMSRET_SUCCESS;

}




extern UBOOL8 rutWanWifi_getWlIntfByIfName(char *ifName, InstanceIdStack *iidStack, WanWifiIntfObject **wlIntfCfg);

UBOOL8 dalWifiWan_getWlIntfByIfName(char *ifName, InstanceIdStack *iidStack, WanWifiIntfObject **wlIntfCfg)
{
   return (rutWanWifi_getWlIntfByIfName(ifName, iidStack, wlIntfCfg));
}

CmsRet dalWifiWan_addWlInterface_igd(const WEB_NTWK_VAR *webVar)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 existingWlFound;
   InterfaceControlObject *ifcObj = NULL;
   WanWifiIntfObject *wanWifiObj = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("Entering....wanL2IfName=%s", webVar->wanL2IfName);
   
   existingWlFound = dalWifiWan_getWlIntfByIfName((char *)webVar->wanL2IfName, &iidStack, NULL);
   if (existingWlFound)
   {
      cmsLog_error("Wl interface %s is already in use as WAN Interface",
                    webVar->wanL2IfName);
      return CMSRET_INTERNAL_ERROR;
   }

   /*
    * Use the Interface Control object to move the wifi interface from LAN side
    * to WAN side.
    */
   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_get(MDMOID_INTERFACE_CONTROL, &iidStack, 0, (void **) &ifcObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get InterfaceControlObject, ret=%d", ret);
      return ret;
   }
   
   CMSMEM_REPLACE_STRING(ifcObj->ifName, (char *)webVar->wanL2IfName);
   ifcObj->moveToLANSide = FALSE;
   ifcObj->moveToWANSide = TRUE;

   /* set InterfaceControlObject */
   if ((ret = cmsObj_set(ifcObj, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set InterfaceControlObject, ret = %d", ret);
      cmsObj_free((void **) &ifcObj);
      return ret;
   }		 

   cmsObj_free((void **) &ifcObj);

   /*
    * Find the WANDevice that our wifi interface was created under.
    */
   INIT_INSTANCE_ID_STACK(&iidStack);
   existingWlFound = dalWifiWan_getWlIntfByIfName((char *) webVar->wanL2IfName, &iidStack, &wanWifiObj);
   if (!existingWlFound)
   {
      cmsLog_error("could not find the wifiWan object!");
      return CMSRET_INTERNAL_ERROR;
   }

   cmsLog_debug("WAN %s created at %s", (char *) webVar->wanL2IfName, cmsMdm_dumpIidStack(&iidStack));

   /* set connection mode value */
   if (webVar->connMode == CMS_CONNECTION_MODE_VLANMUX)
   {
      CMSMEM_REPLACE_STRING(wanWifiObj->connectionMode, MDMVS_VLANMUXMODE);
   }
   else
   {
      CMSMEM_REPLACE_STRING(wanWifiObj->connectionMode, MDMVS_DEFAULTMODE);
   }

   wanWifiObj->enable = TRUE;
              
   if ((ret = cmsObj_set(wanWifiObj, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set WanWifiIntfObject, ret = %d", ret);
   }
   cmsObj_free((void **) &wanWifiObj);


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


CmsRet dalWifiWan_deleteWlInterface_igd(const WEB_NTWK_VAR *webVar)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack wanConnDevdStack = EMPTY_INSTANCE_ID_STACK;
   WanConnDeviceObject *wanConn=NULL;
   InterfaceControlObject *ifcObj = NULL;
   CmsRet ret = CMSRET_SUCCESS;
   
   cmsLog_debug("Deleting %s", webVar->wanL2IfName);

   if (dalWifiWan_getWlIntfByIfName((char *)webVar->wanL2IfName, &iidStack, NULL) == FALSE)
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
    * it be deleted in rcl_interfaceControlObject when wl move to lan.
    */
   if (wanConn->WANIPConnectionNumberOfEntries != 0 ||
       wanConn->WANPPPConnectionNumberOfEntries != 0)
   {
      cmsLog_debug("WanConnDevice is still in use and cannot be removed");
      cmsObj_free((void **) &wanConn);
      return CMSRET_REQUEST_DENIED;
   }

   cmsObj_free((void **) &wanConn);

   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_get(MDMOID_INTERFACE_CONTROL, &iidStack, 0, (void **) &ifcObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get InterfaceControlObject, ret=%d", ret);
      return ret;
   }
   
   CMSMEM_REPLACE_STRING(ifcObj->ifName, webVar->wanL2IfName);
   ifcObj->moveToLANSide = TRUE;
   ifcObj->moveToWANSide = FALSE;

   /* set InterfaceControlObject */
   if ((ret = cmsObj_set(ifcObj, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set InterfaceControlObject, ret = %d", ret);
      cmsObj_free((void **) &ifcObj);
      return ret;
   }		 

   cmsObj_free((void **) &ifcObj);

   cmsLog_debug("Exit, ret=%d", ret);
   
   return ret;
}


CmsRet dalWifiWan_setUreMode(SINT32 ureMode)
{
   WlBaseCfgObject *wlBaseCfgObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   /* FIXME: This algorithm assumes the Wifi Wan intf is on first adapter */
   if ((ret = cmsObj_getNext(MDMOID_WL_BASE_CFG, &iidStack,
                               (void **)&wlBaseCfgObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get WL_BASE_CFG, ret=%d", ret);
      return ret;
   }

   wlBaseCfgObj->wlEnableUre = ureMode;

   ret = cmsObj_set(wlBaseCfgObj, &iidStack);
   cmsObj_free((void **) &wlBaseCfgObj);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Set of WL_BASE_CFG for URE mode failed");
   }

   return ret;
}
#endif /*DMP_X_BROADCOM_COM_WIFIWAN_1*/


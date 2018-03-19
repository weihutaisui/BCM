/***********************************************************************
 *
 *  Copyright (c) 2006-2010  Broadcom Corporation
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


#include "cms.h"
#include "cms_util.h"
#include "cms_core.h"
#include "mdm.h"
#include "mdm_private.h"
#include "odl.h"
#include "oal.h"
#ifdef DMP_X_BROADCOM_COM_CELLULARWAN_1  
#include <linux/device2/rut2_cellular.h>
#endif


/*!\file mdm_initcellular.c
 * \brief This file contains cellular mdm init related functions, for WAN side.
 *
 */

#ifdef DMP_BRIDGING_1
CmsRet addDefaultL2BridgingAvailableInterfaceObject(const char *interfaceReference, SINT32 bridgeRef);
#endif


#ifdef DMP_X_BROADCOM_COM_CELLULARWAN_1  
CmsRet mdm_addDefaultWanCellularObjects(void)
{
   WanCommonIntfCfgObject *wanCommonObj = NULL;   
   WanCellularInterfaceCfgObject *wanCellularIntfObj = NULL;   
   void *mdmObj = NULL;
   CmsRet ret;
   MdmPathDescriptor pathDesc;
   InstanceIdStack savedIidStack = EMPTY_INSTANCE_ID_STACK;
   
   /*
    * User has selected Cellular as a WAN interface.  See if there is aleady a Cellular
    * WAN device.  If not, create it at the fixed instance number.
    */
   PUSH_INSTANCE_ID(&savedIidStack, CMS_WANDEVICE_CELLULAR);
   ret = mdm_getObject(MDMOID_WAN_DEV, &savedIidStack, &mdmObj);
   
   if (ret == CMSRET_SUCCESS)
   {
      /* Cellular WANDevice is already present, no action needed */
      mdm_freeObject(&mdmObj);
      return ret;
   }


   INIT_PATH_DESCRIPTOR(&pathDesc);
   pathDesc.oid = MDMOID_WAN_DEV;
   PUSH_INSTANCE_ID(&pathDesc.iidStack, CMS_WANDEVICE_CELLULAR);
   cmsLog_notice("adding Cellular WANDevice");

   if ((ret = mdm_addObjectInstance(&pathDesc, NULL, NULL)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not add Cellular WANDevice at %s, ret=%d", ret, cmsMdm_dumpIidStack(&pathDesc.iidStack));
      return ret;
   }


   /* update InternetGatewayDevice.WANDevice.{i}.WANCommonInterfaceConfig. with the Cellular WAN WANAccessType */
   if ((ret = mdm_getObject(MDMOID_WAN_COMMON_INTF_CFG, &(pathDesc.iidStack), (void **) &wanCommonObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get MDMOID_WAN_COMMON_INTF_CFG, ret=%d", ret);
      return ret;
   }

   CMSMEM_REPLACE_STRING_FLAGS(wanCommonObj->WANAccessType, MDMVS_X_BROADCOM_COM_CELLULAR, mdmLibCtx.allocFlags); 

   if ((ret = mdm_setObject((void **) &wanCommonObj, &(pathDesc.iidStack), FALSE)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set MDMOID_WAN_COMMON_INTF_CFG, ret = %d", ret);
      mdm_freeObject((void **) &wanCommonObj);
      return ret;
   }

   mdm_freeObject((void **) &wanCommonObj);

   /* update InternetGatewayDevice.WANDevice.{i}.X_BROADCOM_COM_WANCellularInterfaceConfig. with the Cellular Interface */
   if ((ret = mdm_getObject(MDMOID_WAN_CELLULAR_INTERFACE_CFG, &(pathDesc.iidStack), (void **) &wanCellularIntfObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get MDMOID_WAN_CELLULAR_INTERFACE_CFG, ret=%d", ret);
      return ret;
   }

   CMSMEM_REPLACE_STRING_FLAGS(wanCellularIntfObj->cellularInterface, "InternetGatewayDevice.Device.Cellular.Interface.1", mdmLibCtx.allocFlags); 

   if ((ret = mdm_setObject((void **) &wanCellularIntfObj, &(pathDesc.iidStack), FALSE)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set MDMOID_WAN_CELLULAR_INTERFACE_CFG, ret = %d", ret);
      mdm_freeObject((void **) &wanCellularIntfObj);
      return ret;
   }
                
   mdm_freeObject((void **) &wanCellularIntfObj);
	  
   mdm_increaseWanDeviceCount(1);

   return ret;
}


/*Default APN is auto selected by cellularApp/rild if no user configration*/
CmsRet mdm_addDefaultCellularAPNObjects(void)
{
   Dev2CellularAccessPointObject *cellularApnObj = NULL;
   Dev2CellularObject *cellularObj = NULL;
   WanIpConnObject *wanIpConn = NULL; 
   WanConnDeviceObject *wanConnDev = NULL;
   WanDevObject *wanDev = NULL;
   L3ForwardingObject *L3ForwadingObj=NULL;
   NetworkConfigObject *networkCfg=NULL;
   void *mdmObj = NULL;
   CmsRet ret;
   MdmPathDescriptor pathDesc;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanCellularLinkCfgObject *cellularLinkCfgObj = NULL;
   
   ret = mdm_getNextObject(MDMOID_DEV2_CELLULAR_ACCESS_POINT, &iidStack, &mdmObj);
   
   if (ret == CMSRET_SUCCESS)
   {
      /* APN is already configured, no action needed */
      mdm_freeObject(&mdmObj);
      return ret;
   }


   /*Add default APN -- "Auto" if no user configration. */
   INIT_PATH_DESCRIPTOR(&pathDesc);
   pathDesc.oid = MDMOID_DEV2_CELLULAR_ACCESS_POINT;
   PUSH_INSTANCE_ID(&pathDesc.iidStack, 1);
   cmsLog_notice("adding cellular access point");

   if ((ret = mdm_addObjectInstance(&pathDesc, NULL, NULL)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not add Cellular access point, ret=%d", ret);
      return ret;
   }

   if ((ret = mdm_getObject(MDMOID_DEV2_CELLULAR_ACCESS_POINT, &(pathDesc.iidStack), (void **) &cellularApnObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get MDMOID_DEV2_CELLULAR_ACCESS_POINT, ret=%d", ret);
      return ret;
   }

   cellularApnObj->enable = 1;
   CMSMEM_REPLACE_STRING_FLAGS(cellularApnObj->APN, CELLULAR_AUTO_SEL_APN_NAME, mdmLibCtx.allocFlags); 
   CMSMEM_REPLACE_STRING_FLAGS(cellularApnObj->X_BROADCOM_COM_Type, CELLULAR_APN_TYPE_DEFAULT, mdmLibCtx.allocFlags); 

   if ((ret = mdm_setObject((void **) &cellularApnObj, &(pathDesc.iidStack), FALSE)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set MDMOID_DEV2_CELLULAR_ACCESS_POINT, ret = %d", ret);
      mdm_freeObject((void **) &cellularApnObj);
      return ret;
   }

   mdm_freeObject((void **) &cellularApnObj);


   /*modify APN number manually */
   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = mdm_getObject(MDMOID_DEV2_CELLULAR, &iidStack, (void **) &cellularObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get MDMOID_DEV2_CELLULAR, ret=%d", ret);
      return ret;
   }

   cellularObj->accessPointNumberOfEntries = 1;
   
   if ((ret = mdm_setObject((void **) &cellularObj, &iidStack, FALSE)) != CMSRET_SUCCESS)
   {
      cmsLog_error("mdm_setObject <MDMOID_DEV2_CELLULAR> returns error. ret=%d", ret);
   }

   mdm_freeObject((void **) &cellularObj);


   /*Sarah: TODO: create another function to support TR181*/
   /*Add default wan ip connection. */
   INIT_PATH_DESCRIPTOR(&pathDesc);
   pathDesc.oid = MDMOID_WAN_CONN_DEVICE;
   PUSH_INSTANCE_ID(&pathDesc.iidStack, CMS_WANDEVICE_CELLULAR);
   PUSH_INSTANCE_ID(&pathDesc.iidStack, 1);
   
   if ((ret = mdm_addObjectInstance(&pathDesc, NULL, NULL)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create new WanConnectionDevice, ret=%d", ret);
      return ret;
   }
   
   if ((ret = mdm_getObject(MDMOID_WAN_CELLULAR_LINK_CFG, &(pathDesc.iidStack), (void **) &cellularLinkCfgObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get MDMOID_WAN_CELLULAR_LINK_CFG, ret=%d", ret);
      return ret;
   }       

   CMSMEM_REPLACE_STRING_FLAGS(cellularLinkCfgObj->cellularAPN, "InternetGatewayDevice.Device.Cellular.AccessPoint.1", mdmLibCtx.allocFlags); 
   
   if ((ret = mdm_setObject((void **) &cellularLinkCfgObj, &(pathDesc.iidStack), FALSE)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set cellularLinkCfgObj, ret = %d", ret);
   }
   
   mdm_freeObject((void **)&cellularLinkCfgObj);

   /* Need to update the WanConnDevice count */
   if ((ret = mdm_getAncestorObject(MDMOID_WAN_DEV, MDMOID_WAN_CONN_DEVICE,
                                    &(pathDesc.iidStack),(void **) &wanDev)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Fail to get WanDev object, ret=%d", ret);
      return ret;
   }
 
   /* update wanConnectionDevice counter */
   wanDev->WANConnectionNumberOfEntries = 1;
      
   if ((ret = mdm_setObject((void **) &wanDev, &(pathDesc.iidStack), FALSE)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not set wanDev, ret=%d", ret);
      mdm_freeObject((void **) &wanDev);               
      return ret;
   }
   
   INIT_PATH_DESCRIPTOR(&pathDesc);
   pathDesc.oid = MDMOID_WAN_IP_CONN;
   PUSH_INSTANCE_ID(&pathDesc.iidStack, CMS_WANDEVICE_CELLULAR);
   PUSH_INSTANCE_ID(&pathDesc.iidStack, 1);
   PUSH_INSTANCE_ID(&pathDesc.iidStack, 1);
   
   /* add WAN_IP_CONN instance */
   if ((ret = mdm_addObjectInstance(&pathDesc, NULL, NULL)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to add cellular wanIpConnInstance, ret = %d", ret);
      return ret;
   }
   
   if ((ret = mdm_getObject(MDMOID_WAN_IP_CONN, &pathDesc.iidStack, (void **) &wanIpConn)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get wanIpConn, ret = %d", ret);
      return ret;
   }

   wanIpConn->enable = TRUE;
   wanIpConn->X_BROADCOM_COM_ConnectionId = 0;
   CMSMEM_REPLACE_STRING_FLAGS(wanIpConn->X_BROADCOM_COM_IfName, CELLULAR_IFC_STR "0", mdmLibCtx.allocFlags);
   CMSMEM_REPLACE_STRING_FLAGS(wanIpConn->name, "AUTO", mdmLibCtx.allocFlags);
   wanIpConn->NATEnabled = TRUE; 
   CMSMEM_REPLACE_STRING_FLAGS(wanIpConn->connectionType, MDMVS_IP_ROUTED, mdmLibCtx.allocFlags);
   CMSMEM_REPLACE_STRING_FLAGS(wanIpConn->addressingType, MDMVS_X_BROADCOM_COM_NAS, mdmLibCtx.allocFlags);
   wanIpConn->X_BROADCOM_COM_IPv4Enabled = 1;
   
   if ((ret = mdm_setObject((void **) &wanIpConn, &(pathDesc.iidStack), FALSE)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not set MDMOID_WAN_IP_CONN, ret=%d", ret);
   }
   
   mdm_freeObject((void **) &wanIpConn);

#ifdef DMP_BRIDGING_1
   /* Add pmap stuff */
   {
      char *fullPath=NULL;

      cmsMdm_pathDescriptorToFullPath(&pathDesc, &fullPath);
      /* Annoying TR-98 format: remove the last . */
      fullPath[strlen(fullPath)-1] = '\0';
      cmsLog_debug("fullpathname is %s", fullPath);

      ret = addDefaultL2BridgingAvailableInterfaceObject(fullPath, 0);
      cmsMem_free(fullPath);
   }
#endif

   iidStack = pathDesc.iidStack;
   /* Need to update the WanConnDevice count */
   if ((ret = mdm_getAncestorObject(MDMOID_WAN_CONN_DEVICE, MDMOID_WAN_IP_CONN,
                                    &iidStack,(void **) &wanConnDev)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Fail to get WanDev object, ret=%d", ret);
      return ret;
   }
 
   /* update wanConnectionDevice counter */
   wanConnDev->WANIPConnectionNumberOfEntries = 1;
      
   if ((ret = mdm_setObject((void **) &wanConnDev, &iidStack, FALSE)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not set wanConnDev, ret=%d", ret);
      mdm_freeObject((void **) &wanConnDev);               
      return ret;
   }
   
   mdm_freeObject((void **) &wanConnDev);    


    /* set up gateway */
   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = mdm_getObject(MDMOID_L3_FORWARDING, &iidStack, (void **) &L3ForwadingObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get MDMOID_L3_FORWARDING, ret=%d", ret);
      return ret;
   }

   CMSMEM_REPLACE_STRING_FLAGS(L3ForwadingObj->X_BROADCOM_COM_DefaultConnectionServices, CELLULAR_IFC_STR "0", mdmLibCtx.allocFlags);
   
   if ((ret = mdm_setObject((void **) &L3ForwadingObj, &iidStack, FALSE)) != CMSRET_SUCCESS)
   {
      cmsLog_error("mdm_setObject <MDMOID_L3_FORWARDING> returns error. ret=%d", ret);
   }

   mdm_freeObject((void **) &L3ForwadingObj);


   /* set up dns info */
   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = mdm_getObject(MDMOID_NETWORK_CONFIG, &iidStack, (void **) &networkCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get NETWORK_CONFIG, ret=%d", ret);
      return ret;
   }
 
   CMSMEM_REPLACE_STRING_FLAGS(networkCfg->DNSIfName, CELLULAR_IFC_STR "0", mdmLibCtx.allocFlags);
  
   if ((ret = mdm_setObject((void **) &networkCfg, &iidStack, FALSE)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not set NETWORK_CONFIG, ret=%d", ret);
   }
 
   mdm_freeObject((void **) &networkCfg);
   return ret;
}
   
#endif /* DMP_X_BROADCOM_COM_CELLULARWAN_1 */


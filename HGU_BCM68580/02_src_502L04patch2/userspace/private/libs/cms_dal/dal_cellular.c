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
#include "dal_network.h"
#include "rut_wan.h"

#ifdef DMP_BASELINE_1

#ifdef DMP_X_BROADCOM_COM_CELLULARWAN_1
/*
    PARAM IN: apnName, apnInstanceID
    PARAM OUT: ifname
*/
CmsRet dalCellular_addCellularWanService(char *apnName, WanConnL3Type enblIpVer, UINT8 apnInstanceID, char *ifname)
{
   WanCellularLinkCfgObject *cellularLinkCfgObj = NULL;
   WanIpConnObject *wanIpConn = NULL; 
   NetworkConfigObject *networkCfg=NULL;
   L3ForwardingObject *L3ForwadingObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;  
   char buf[BUFLEN_64];   
   CmsRet ret = CMSRET_SUCCESS;
   SINT32 conId;
   
   cmsLog_debug("apnName=%s, apnInstanceID=%d", apnName, apnInstanceID);

   if ((ret = rutWan_getAvailableConIdForCellular(&conId)) != CMSRET_SUCCESS)
   {
      cmsLog_error("rutWan_getAvailableConIdForCellular failed on APN %s. ret=%d", apnName, ret);
   }

   /* add a new cellular layer2 instance */
   INIT_INSTANCE_ID_STACK(&iidStack);
   PUSH_INSTANCE_ID(&iidStack, CMS_WANDEVICE_CELLULAR);
   
   if ((ret = cmsObj_addInstance(MDMOID_WAN_CONN_DEVICE, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create new WanConnectionDevice, ret=%d", ret);
      return ret;
   }
   
   if ((ret = cmsObj_get(MDMOID_WAN_CELLULAR_LINK_CFG, &iidStack, 0, (void **) &cellularLinkCfgObj)) != CMSRET_SUCCESS)
   {
      cmsObj_deleteInstance(MDMOID_WAN_CONN_DEVICE, &iidStack);
      cmsLog_error("Failed to get MDMOID_WAN_CELLULAR_LINK_CFG, ret=%d", ret);
      return ret;
   }       

   sprintf(buf, "InternetGatewayDevice.Device.Cellular.AccessPoint.%d", apnInstanceID);   
   CMSMEM_REPLACE_STRING(cellularLinkCfgObj->cellularAPN, buf);
   
   if ((ret = cmsObj_set(cellularLinkCfgObj, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set cellularLinkCfgObj, ret = %d", ret);
      cmsObj_deleteInstance(MDMOID_WAN_CONN_DEVICE, &iidStack);
   }
   
   cmsObj_free((void **)&cellularLinkCfgObj);

   /* add WAN_IP_CONN instance */
   if ((ret = cmsObj_addInstance(MDMOID_WAN_IP_CONN, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to add wanIpConnInstance, ret = %d", ret);
      return ret;
   }

   if ((ret = cmsObj_get(MDMOID_WAN_IP_CONN, &iidStack, 0, (void **) &wanIpConn)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get wanIpConn, ret = %d", ret);
      cmsObj_deleteInstance(MDMOID_WAN_IP_CONN, &iidStack);
      return ret;
   }


   wanIpConn->enable = TRUE;
   wanIpConn->X_BROADCOM_COM_ConnectionId = conId;
   sprintf(buf, "%s%d", CELLULAR_IFC_STR, conId);   
   CMSMEM_REPLACE_STRING(wanIpConn->X_BROADCOM_COM_IfName, buf);
   CMSMEM_REPLACE_STRING(wanIpConn->name, apnName);
   wanIpConn->NATEnabled = TRUE; 
   CMSMEM_REPLACE_STRING(wanIpConn->connectionType, MDMVS_IP_ROUTED);
   CMSMEM_REPLACE_STRING(wanIpConn->addressingType, MDMVS_X_BROADCOM_COM_NAS);
   wanIpConn->X_BROADCOM_COM_IPv4Enabled = (enblIpVer != ENBL_IPV6_ONLY);
#ifdef SUPPORT_IPV6
   wanIpConn->X_BROADCOM_COM_IPv6Enabled = (enblIpVer != ENBL_IPV4_ONLY);
#endif

   if ((ret = cmsObj_set(wanIpConn, &iidStack)) != CMSRET_SUCCESS)
   {
     cmsLog_error("Failed to set wanIpConnObj, ret = %d, delete the instance", ret);
     cmsObj_deleteInstance(MDMOID_WAN_IP_CONN, &iidStack);
   }

   cmsUtl_strcpy(ifname, wanIpConn->X_BROADCOM_COM_IfName);
   cmsObj_free((void **)&wanIpConn);


   /* set up gateway */
   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_get(MDMOID_L3_FORWARDING, &iidStack, 0, (void **)&L3ForwadingObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get MDMOID_L3_FORWARDING, ret=%d", ret);
      return ret;
   }

   CMSMEM_REPLACE_STRING(L3ForwadingObj->X_BROADCOM_COM_DefaultConnectionServices, ifname);
   
   if ((ret = cmsObj_set(L3ForwadingObj, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_set <MDMOID_L3_FORWARDING> returns error. ret=%d", ret);
   }

   cmsObj_free((void **) &L3ForwadingObj);


   /* set up dns info */
   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_get(MDMOID_NETWORK_CONFIG, &iidStack,
                         OGF_NO_VALUE_UPDATE,
                         (void **) &networkCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get NETWORK_CONFIG, ret=%d", ret);
      return ret;
   }
 
   CMSMEM_REPLACE_STRING(networkCfg->DNSIfName, ifname);
  
   if ((ret = cmsObj_set(networkCfg, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not set NETWORK_CONFIG, ret=%d", ret);
   }
 
   cmsObj_free((void **) &networkCfg);

#if defined(DMP_X_BROADCOM_COM_DEV2_IPV6_1)
   if (enblIpVer != ENBL_IPV4_ONLY &&
      (ret = dalCellular2_addCellularWanService(ifname)) != CMSRET_SUCCESS)
   {
      cmsLog_error("dalCellular2_addCellularWanService returns error. ret=%d", ret);
   }   
#endif

   return ret;
}


/*
    PARAM: webVar->wanIfName will be overide to apn interface name
*/
CmsRet dalCellular_delCellularWanService(UINT8 apnInstanceID, WEB_NTWK_VAR *webVar)
{
   WanCellularLinkCfgObject *cellularLinkCfgObj = NULL;
   WanIpConnObject *wanIpConn = NULL; 
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack2 = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;
   char apnInstanceIDStr[BUFLEN_8];
   UBOOL8 ipv6Ebl = FALSE;
   
   cmsLog_debug("apnInstanceID=%d", apnInstanceID);

   sprintf(apnInstanceIDStr, "%d", apnInstanceID); 
   
   while ((ret = cmsObj_getNext(MDMOID_WAN_CELLULAR_LINK_CFG, &iidStack, (void **)&cellularLinkCfgObj)) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strstr(cellularLinkCfgObj->cellularAPN, apnInstanceIDStr))
      {
         if ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_IP_CONN, &iidStack, &iidStack2, (void **)&wanIpConn)) == CMSRET_SUCCESS)      
         {
            cmsUtl_strcpy(webVar->wanIfName, wanIpConn->X_BROADCOM_COM_IfName);
            ipv6Ebl = wanIpConn->X_BROADCOM_COM_IPv6Enabled;
            cmsObj_free((void **)&wanIpConn);
         }       
         else		 
         {
            cmsLog_error("Failed to cmsObj_getNextInSubTree, ret = %d", ret);
         }       
         cmsObj_deleteInstance(MDMOID_WAN_CONN_DEVICE, &iidStack);
         cmsObj_free((void **)&cellularLinkCfgObj);
         break;
      }       

      cmsObj_free((void **)&cellularLinkCfgObj);
   }

#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
   if (ipv6Ebl && (ret = dalCellular2_delCellularWanService(webVar)) != CMSRET_SUCCESS)
   {
      cmsLog_error("dalCellular2_delCellularWanService returns error. ret=%d", ret);
   }   
#endif     
   
   return ret;
}


#endif /* DMP_X_BROADCOM_COM_CELLULARWAN_1 */

#endif /* DMP_BASELINE_1 */

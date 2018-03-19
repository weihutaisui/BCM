/***********************************************************************
 *
 *  Copyright (c) 2009  Broadcom Corporation
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
#include "dal_wan.h"

#ifdef DMP_X_BROADCOM_COM_PPTPAC_1

CmsRet dalPptpAc_deletePptpAcInterface(WEB_NTWK_VAR *webVar)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack1 = EMPTY_INSTANCE_ID_STACK; 
   void *obj = NULL;
   PptpAcIntfConfigObject *PptpAcIntf = NULL;
   CmsRet ret = CMSRET_SUCCESS;
  
   cmsLog_debug("Deleting %s", webVar->pptpTunnelName);

   if ((ret = dalWan_getWanPptpAcObject(&iidStack, &PptpAcIntf)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get PptpAcIntf object");
   }
   else
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(PptpAcIntf->pptpTunnelName);
      CMSMEM_FREE_BUF_AND_NULL_PTR(PptpAcIntf->pnsIpAddress);
      CMSMEM_REPLACE_STRING(PptpAcIntf->intfStatus, MDMVS_DOWN);
      PptpAcIntf->enable = FALSE;
      if ((ret = cmsObj_set(PptpAcIntf, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to set PptpAcIntf object, ret = %d", ret);
      }
      cmsObj_free((void **) &PptpAcIntf);
      
      /* get iidstack of WANConnectionDevice */
      if ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_CONN_DEVICE, &iidStack, &iidStack1, (void **)&obj)) 
         != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get WanConnDevice, ret=%d", ret);
         return ret;
      }
      cmsObj_free(&obj);   /* no longer needed */

      /* If the tunnel is removed, just delete the WanConnDevice */
      if ((ret = cmsObj_deleteInstance(MDMOID_WAN_CONN_DEVICE, &iidStack1)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to delete wanConn Object, ret = %d", ret);
      }
   }
   
   cmsLog_debug("Exit, ret=%d", ret);
   
   return ret;
}


CmsRet dalPptpAc_addPptpAcInterface(const WEB_NTWK_VAR *webVar)
{
   WanPppConnObject *pppConn = NULL; 
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack PptpAcIntfIidStack;
   PptpAcIntfConfigObject *PptpAcIntf = NULL;
   PptpAcLinkConfigObject *PptpAclinkCfg = NULL;   
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("pptpTunnelName=%s, pnsIpAddress=%s", webVar->pptpTunnelName, webVar->pnsIpAddress);

   if ((ret = dalWan_getWanPptpAcObject(&iidStack, &PptpAcIntf)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get PptpAcIntf object");
      return ret;
   }
   PptpAcIntfIidStack = iidStack;
   
   /* add new instance of WanConnectionDevice */
   if ((ret = cmsObj_addInstance(MDMOID_WAN_CONN_DEVICE, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not create new WanConnectionDevice, ret=%d", ret);
      cmsObj_free((void **) &PptpAcIntf);
      return ret;
   }
   
   /* Get the WANPTMLinkConfig to set the new values to it */
   if ((ret = cmsObj_get(MDMOID_PPTP_AC_LINK_CONFIG, &iidStack, 0, (void **) &PptpAclinkCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get iidStack, ret = %d", ret);
   }
   else 
   {
      /* Active PPTPAC link Config object ? or later after Pptpd link is up - cwu todo */
      PptpAclinkCfg->enable = TRUE;
      if ((ret = cmsObj_set(PptpAclinkCfg, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("set of PptpAclinkCfg failed");
      }
      else
      {
         cmsLog_debug("cmsObj_set PptpAclinkCfg ok.");
      }
   }   

   if (ret != CMSRET_SUCCESS)
   {
      cmsObj_deleteInstance(MDMOID_WAN_CONN_DEVICE, &iidStack);
      cmsObj_free((void **) &PptpAcIntf);
      return ret;
   }
   
   cmsLog_debug("Adding PPP_CONN, iidStack=%s", cmsMdm_dumpIidStack(&iidStack));

   /* add WAN_PPP_CONN instance */
   if ((ret = cmsObj_addInstance(MDMOID_WAN_PPP_CONN, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to add pppConnInstance, ret = %d", ret);
   }
   else
   {
      if ((ret = cmsObj_get(MDMOID_WAN_PPP_CONN, &iidStack, 0, (void **) &pppConn)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to get pppConn, ret = %d", ret);
      }
   }
   
   if (ret != CMSRET_SUCCESS)
   {
      cmsObj_deleteInstance(MDMOID_WAN_CONN_DEVICE, &iidStack);
      cmsObj_free((void **) &PptpAcIntf);
      return ret;
   }
   
   /* get sevice name */
   if (strlen(webVar->pppServerName) > 0)
   {
      CMSMEM_REPLACE_STRING(pppConn->PPPoEServiceName, webVar->pppServerName);
   }
   CMSMEM_REPLACE_STRING(pppConn->connectionType, MDMVS_IP_ROUTED);
   CMSMEM_REPLACE_STRING(pppConn->username, webVar->pppUserName);
   CMSMEM_REPLACE_STRING(pppConn->password, webVar->pppPassword);

   /* Convert the number to string for auth protocol (method) */
   CMSMEM_REPLACE_STRING(pppConn->PPPAuthenticationProtocol, cmsUtl_numToPppAuthString(webVar->pppAuthMethod)); 
   
   /* get on demand ideltime out in seconds if it is enabled */
   if (webVar->enblOnDemand)
   {
      pppConn->idleDisconnectTime = webVar->pppTimeOut * 60;
   }
   else
   {
      /* 0 is no on demand feature */
      pppConn->idleDisconnectTime = 0;
   }

   /* todo: Assuming NO IpExtension and static IP, passthru, connection mode and advanced DMZ here */
   pppConn->NATEnabled = TRUE;
   pppConn->X_BROADCOM_COM_FullconeNATEnabled = webVar->enblFullcone;
   pppConn->X_BROADCOM_COM_FirewallEnabled = TRUE;

   /* get ppp debug flag */
   pppConn->X_BROADCOM_COM_Enable_Debug = webVar->enblPppDebug;

#ifdef DMP_X_BROADCOM_COM_IPV6_1 /* aka SUPPORT_IPV6 */
      if (webVar->enblIpVer != ENBL_IPV4_ONLY)
      {
         pppConn->X_BROADCOM_COM_IPv6Enabled = TRUE;
         CMSMEM_REPLACE_STRING(pppConn->X_BROADCOM_COM_IPv6AddressingType, webVar->wanAddr6Type);
         if (cmsUtl_strcmp(webVar->wanAddr6Type, MDMVS_STATIC) == 0)
         {
            CMSMEM_REPLACE_STRING(pppConn->X_BROADCOM_COM_ExternalIPv6Address, webVar->wanAddr6);
         }
         CMSMEM_REPLACE_STRING(pppConn->X_BROADCOM_COM_DefaultIPv6Gateway, webVar->wanGtwy6);
      }
#endif
   
   pppConn->enable = TRUE;

#ifdef DMP_X_BROADCOM_COM_IGMP_1
   pppConn->X_BROADCOM_COM_IGMPEnabled  = webVar->enblIgmp;
   pppConn->X_BROADCOM_COM_IGMP_SOURCEEnabled  = webVar->enblIgmpMcastSource;
#endif
#ifdef DMP_X_BROADCOM_COM_GPONWAN_1
   pppConn->X_BROADCOM_COM_NoMcastVlanFilter  = webVar->noMcastVlanFilter;
#endif
#ifdef DMP_X_BROADCOM_COM_MLD_1
   pppConn->X_BROADCOM_COM_MLDEnabled  = webVar->enblMld;
   pppConn->X_BROADCOM_COM_MLD_SOURCEEnabled  = webVar->enblMldMcastSource;
#endif

   /* fill in service name */
   CMSMEM_REPLACE_STRING(pppConn->name, "PPPoPptpAc");   /* todo. just assign some thing for now */
   
   /* set and activate pppConnObj */
   if ((ret = cmsObj_set(pppConn, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Set pppConn failed");
   }
   else
   {
      cmsLog_debug("cmsObj_set pppConn ok.");
      cmsObj_free((void **) &pppConn);

      /* enable the PPTPAC config object and set the tunnel name and Pns Ip Address after
      * WanPPPConn object is created since Pptpd needs ppp username/password info
      */
      CMSMEM_REPLACE_STRING(PptpAcIntf->pptpTunnelName, webVar->pptpTunnelName);
      CMSMEM_REPLACE_STRING(PptpAcIntf->pnsIpAddress, webVar->pnsIpAddress);
      PptpAcIntf->enable = TRUE;
              
      if ((ret = cmsObj_set(PptpAcIntf, &PptpAcIntfIidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to set PptpAcIntf object, ret = %d", ret);
      }
      else
      {
         cmsObj_free((void **) &PptpAcIntf);
         cmsLog_debug("Set PptpAcIntf ok.");
      }
   }
   
   if (ret != CMSRET_SUCCESS)
   {
      cmsObj_deleteInstance(MDMOID_WAN_CONN_DEVICE, &iidStack);
   }
   
   return ret;

}

#endif /* DMP_X_BROADCOM_COM_PPTPAC_1 */


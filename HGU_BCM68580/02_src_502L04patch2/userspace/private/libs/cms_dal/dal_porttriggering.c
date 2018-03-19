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

#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"

/* local functions */
static CmsRet fillPrtTrgCfg(const UBOOL8 ppp_found, const char *appName,
   const char *tProto, const char *oProto, const UINT16 tPS, const UINT16 tPE,
   const UINT16 oPS, const UINT16 oPE, void * obj)
{
   WanPppConnPortTriggeringObject *pppConnPT = NULL;
   WanIpConnPortTriggeringObject *ipConnPT = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("%u/%s/%s/%hu/%hu/%s/%hu/%hu", ppp_found, appName, tProto, tPS, tPE, oProto, oPS, oPE);
   if (ppp_found == TRUE)
   {
      pppConnPT = (WanPppConnPortTriggeringObject *) obj;
      pppConnPT->enable = TRUE;

      CMSMEM_REPLACE_STRING(pppConnPT->name, appName);
      if (strcmp(tProto, "1") == 0)
      {
         CMSMEM_REPLACE_STRING(pppConnPT->triggerProtocol, "TCP");
      }
      else if (strcmp(tProto, "2") == 0)
      {
         CMSMEM_REPLACE_STRING(pppConnPT->triggerProtocol, "UDP");
      }
      else
      {
         CMSMEM_REPLACE_STRING(pppConnPT->triggerProtocol, "TCP or UDP");
      }
      pppConnPT->triggerPortStart= tPS;
      pppConnPT->triggerPortEnd = tPE;
      if (strcmp(oProto, "1") == 0)
      {
         CMSMEM_REPLACE_STRING(pppConnPT->openProtocol, "TCP");
      }
      else if (strcmp(oProto, "2") == 0)
      {
         CMSMEM_REPLACE_STRING(pppConnPT->openProtocol, "UDP");
      }
      else
      {
         CMSMEM_REPLACE_STRING(pppConnPT->openProtocol, "TCP or UDP");
      }
      pppConnPT->openPortStart= oPS;
      pppConnPT->openPortEnd = oPE;

      if ((pppConnPT->name == NULL) ||(pppConnPT->triggerProtocol == NULL) ||(pppConnPT->openProtocol == NULL))
      {
         cmsLog_error("malloc failed.");
         cmsObj_free((void **) &pppConnPT);
         return CMSRET_RESOURCE_EXCEEDED;
      }
   }
   else
   {
      ipConnPT = (WanIpConnPortTriggeringObject *) obj;
      ipConnPT->enable = TRUE;

      CMSMEM_REPLACE_STRING(ipConnPT->name, appName);
      if (strcmp(tProto, "1") == 0)
      {
         CMSMEM_REPLACE_STRING(ipConnPT->triggerProtocol, "TCP");
      }
      else if (strcmp(tProto, "2") == 0)
      {
         CMSMEM_REPLACE_STRING(ipConnPT->triggerProtocol, "UDP");
      }
      else
      {
         CMSMEM_REPLACE_STRING(ipConnPT->triggerProtocol, "TCP or UDP");
      }
      ipConnPT->triggerPortStart= tPS;
      ipConnPT->triggerPortEnd = tPE;
      if (strcmp(oProto, "1") == 0)
      {
         CMSMEM_REPLACE_STRING(ipConnPT->openProtocol, "TCP");
      }
      else if (strcmp(oProto, "2") == 0)
      {
         CMSMEM_REPLACE_STRING(ipConnPT->openProtocol, "UDP");
      }
      else
      {
         CMSMEM_REPLACE_STRING(ipConnPT->openProtocol, "TCP or UDP");
      }
      ipConnPT->openPortStart= oPS;
      ipConnPT->openPortEnd = oPE;

      if ((ipConnPT->name == NULL) ||(ipConnPT->triggerProtocol == NULL) ||(ipConnPT->openProtocol == NULL))
      {
         cmsLog_error("malloc failed.");
         cmsObj_free((void **) &ipConnPT);
         return CMSRET_RESOURCE_EXCEEDED;
      }
   }

   return ret;
}

CmsRet dalPortTriggering_addEntry_igd(const char *dstWanIf, const char *appName,
   const char *tProto, const char *oProto, const UINT16 tPS,
   const UINT16 tPE, const UINT16 oPS, const UINT16 oPE)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack pppIidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack ipIidStack = EMPTY_INSTANCE_ID_STACK;
   WanPppConnObject *pppConn = NULL;   
   WanIpConnObject  *ipConn = NULL;
   UBOOL8 ppp_found = FALSE;
   UBOOL8 ip_found = FALSE;
   WanPppConnPortTriggeringObject *pppConnPT = NULL;
   WanIpConnPortTriggeringObject *ipConnPT = NULL;

   /* adding a new port triggering entry */
   cmsLog_debug("Adding new port triggering with %s/%s/%s/%hu/%hu/%s/%hu/%hu",
      dstWanIf, appName, tProto, tPS, tPE, oProto, oPS, oPE);

   while (!ppp_found && 
      (cmsObj_getNextFlags(MDMOID_WAN_PPP_CONN, &pppIidStack, OGF_NO_VALUE_UPDATE, (void **) &pppConn)) == CMSRET_SUCCESS)
   {
      if(!cmsUtl_strcmp(pppConn->X_BROADCOM_COM_IfName, dstWanIf))
      {
         ppp_found = TRUE;
      }
      cmsObj_free((void **) &pppConn);
   }

   while (!ppp_found && !ip_found &&
      (cmsObj_getNextFlags(MDMOID_WAN_IP_CONN, &ipIidStack, OGF_NO_VALUE_UPDATE, (void **) &ipConn)) == CMSRET_SUCCESS)
   {
      if(!cmsUtl_strcmp(ipConn->X_BROADCOM_COM_IfName, dstWanIf))
      {
         ip_found = TRUE;
      }
      cmsObj_free((void **) &ipConn);
   }

   /* add new instance */
   if (ppp_found == TRUE)
   {
      if ((ret = cmsObj_addInstance(MDMOID_WAN_PPP_CONN_PORT_TRIGGERING, &pppIidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("could not create new port triggering entry, ret=%d", ret);
         return ret;
      }

      if ((ret = cmsObj_get(MDMOID_WAN_PPP_CONN_PORT_TRIGGERING, &pppIidStack, 0, (void **) &pppConnPT)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to get PortTriggeringObject, ret=%d", ret);
         cmsObj_deleteInstance(MDMOID_WAN_PPP_CONN_PORT_TRIGGERING, &pppIidStack);
         return ret;
      }

      if ((ret = fillPrtTrgCfg(ppp_found, appName, tProto, oProto, tPS, tPE, oPS, oPE, pppConnPT)) != CMSRET_SUCCESS)
      {
         cmsObj_free((void **) &pppConnPT);
         cmsObj_deleteInstance(MDMOID_WAN_PPP_CONN_PORT_TRIGGERING, &pppIidStack);
         return ret;
      }

      cmsLog_debug("in pppConnPT, %s/%s/%u/%u/%s/%u/%u", pppConnPT->name,
         pppConnPT->triggerProtocol, pppConnPT->triggerPortStart, pppConnPT->triggerPortEnd,
         pppConnPT->openProtocol, pppConnPT->openPortStart, pppConnPT->openPortEnd);

      /* set and activate WanPppConnPortTriggeringObject */
      ret = cmsObj_set(pppConnPT, &pppIidStack);
      cmsObj_free((void **) &pppConnPT);

      if (ret != CMSRET_SUCCESS)
      {
         CmsRet r2;
         cmsLog_error("Failed to set WanPppConnPortTriggeringObject, ret = %d", ret);

         r2 = cmsObj_deleteInstance(MDMOID_WAN_PPP_CONN_PORT_TRIGGERING, &pppIidStack);
         if (r2 != CMSRET_SUCCESS)
         {
            cmsLog_error("failed to delete created WanPppConnPortTriggeringObject, r2=%d", r2);
         }

         cmsLog_debug("Failed to set port triggering and successfully delete created WanPppConnPortTriggeringObject");

      }
   }
   else if (ip_found == TRUE)
   {
      if ((ret = cmsObj_addInstance(MDMOID_WAN_IP_CONN_PORT_TRIGGERING, &ipIidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("could not create new port triggering entry, ret=%d", ret);
         return ret;
      }

      if ((ret = cmsObj_get(MDMOID_WAN_IP_CONN_PORT_TRIGGERING, &ipIidStack, 0, (void **) &ipConnPT)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to get WanIpConnPortTriggeringObject, ret=%d", ret);
         cmsObj_deleteInstance(MDMOID_WAN_IP_CONN_PORT_TRIGGERING, &ipIidStack);
         return ret;
      }

      if ((ret = fillPrtTrgCfg(ppp_found, appName, tProto, oProto, tPS, tPE, oPS, oPE, ipConnPT)) != CMSRET_SUCCESS)
      {
         cmsObj_free((void **) &ipConnPT);
         cmsObj_deleteInstance(MDMOID_WAN_IP_CONN_PORT_TRIGGERING, &ipIidStack);
         return ret;
      }

      cmsLog_debug("in ipConnPT, %s/%s/%u/%u/%s/%u/%u", ipConnPT->name,
         ipConnPT->triggerProtocol, ipConnPT->triggerPortStart, ipConnPT->triggerPortEnd,
         ipConnPT->openProtocol, ipConnPT->openPortStart, ipConnPT->openPortEnd);

      /* set and activate WanIpConnPortTriggeringObject */
      ret = cmsObj_set(ipConnPT, &ipIidStack);
      cmsObj_free((void **) &ipConnPT);

      if (ret != CMSRET_SUCCESS)
      {
         CmsRet r2;
         cmsLog_error("Failed to set WanIpConnPortTriggeringObject, ret = %d", ret);

         r2 = cmsObj_deleteInstance(MDMOID_WAN_IP_CONN_PORT_TRIGGERING, &ipIidStack);
         if (r2 != CMSRET_SUCCESS)
         {
            cmsLog_error("failed to delete created WanIpConnPortiTriggerCfgObject, r2=%d", r2);
         }

         cmsLog_debug("Failed to set port triggering and successfully delete created WanIpConnPortTriggeringObject");

      }
   }
   else
   {
      return CMSRET_INVALID_ARGUMENTS;
   }

   return ret;
}


CmsRet dalPortTriggering_deleteEntry_igd(const char *dstWanIf, const char *tProto, 
                                         const UINT16 tPS, const UINT16 tPE,
                                         const UINT16 tOS, const UINT16 tOE)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack wanpppiidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack wanipiidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 ppp_found = FALSE;
   UBOOL8 ip_found = FALSE;
   WanPppConnPortTriggeringObject *pppConnPT = NULL;
   WanIpConnPortTriggeringObject *ipConnPT = NULL;
   WanPppConnObject *wan_ppp_con = NULL;
   WanIpConnObject *wan_ip_con = NULL;

   /* deleting a port triggering entry */
   cmsLog_debug("Deleting port triggering with %s/%s/%hu/%hu/%hu/%hu", dstWanIf, tProto, tPS, tPE, tOS, tOE);

   while (!ppp_found &&
          (ret = cmsObj_getNext(MDMOID_WAN_PPP_CONN_PORT_TRIGGERING, &iidStack,
                                (void **) &pppConnPT)) == CMSRET_SUCCESS)
   {
      if(!strcmp(tProto, pppConnPT->triggerProtocol) && 
          tPS == pppConnPT->triggerPortStart && tPE == pppConnPT->triggerPortEnd &&
          tOS == pppConnPT->openPortStart && tOE == pppConnPT->openPortEnd) 
      {
         memcpy(&wanpppiidStack, &iidStack, sizeof(iidStack));
         cmsObj_getAncestor(MDMOID_WAN_PPP_CONN,
                            MDMOID_WAN_PPP_CONN_PORT_TRIGGERING,
                            &wanpppiidStack, (void **) &wan_ppp_con);
         ppp_found = (0 == cmsUtl_strcmp(dstWanIf, wan_ppp_con->X_BROADCOM_COM_IfName));

         cmsObj_free((void **)&wan_ppp_con);
      }

      cmsObj_free((void **) &pppConnPT);
   }

   if (ppp_found == FALSE)
   {
      INIT_INSTANCE_ID_STACK(&iidStack);
      while (!ip_found &&
             (ret = cmsObj_getNext(MDMOID_WAN_IP_CONN_PORT_TRIGGERING, &iidStack,
                                   (void **) &ipConnPT)) == CMSRET_SUCCESS)
      {
         if(!strcmp(tProto, ipConnPT->triggerProtocol) && 
            tPS == ipConnPT->triggerPortStart && tPE == ipConnPT->triggerPortEnd &&
            tOS == ipConnPT->openPortStart && tOE == ipConnPT->openPortEnd) 
         {
            memcpy(&wanipiidStack, &iidStack, sizeof(iidStack));
            cmsObj_getAncestor(MDMOID_WAN_IP_CONN,
                               MDMOID_WAN_IP_CONN_PORT_TRIGGERING,
                               &wanipiidStack, (void **) &wan_ip_con);
            ip_found = (0 == cmsUtl_strcmp(dstWanIf, wan_ip_con->X_BROADCOM_COM_IfName));

            cmsObj_free((void **) &wan_ip_con);
         }

         cmsObj_free((void **) &ipConnPT);
      }
   }

   if(ppp_found == TRUE)
   {
      ret = cmsObj_deleteInstance(MDMOID_WAN_PPP_CONN_PORT_TRIGGERING, &iidStack);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to delete WanPppConnPortTriggeringObject, ret = %d", ret);
      }
   }
   else if(ip_found == TRUE)
   {
      ret = cmsObj_deleteInstance(MDMOID_WAN_IP_CONN_PORT_TRIGGERING, &iidStack);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to delete WanIpConnPortTriggeringObject, ret = %d", ret);
      }
   }
   else
   {
      cmsLog_debug("bad %s/%s/%hu/%hu/%hu/%hu, no port triggering entry found", dstWanIf, tProto, tPS, tPE, tOS, tOE);
      ret = CMSRET_INVALID_ARGUMENTS;
   }

   return ret;
}


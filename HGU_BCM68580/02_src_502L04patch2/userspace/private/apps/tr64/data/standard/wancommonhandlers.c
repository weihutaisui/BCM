/*
 *
 * <:copyright-BRCM:2012:proprietary:standard
 * 
 *    Copyright (c) 2012 Broadcom 
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
 * :>
 *
 * $Id: wancommon.c,v 1.26.20.2 2003/10/31 21:31:35 mthawani Exp $
 */

#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "igd.h"
#include "wancommon.h"

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"

int SetEnabledForInternet(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
#if 0
   /* cms lib doesn't support this parameter */
   struct Param *pParams;
      
   pParams = findActionParamByRelatedVar(ac,VAR_EnabledForInternet);
   if (pParams != NULL)
   {
      InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
      WanCommonIntfCfgObject *wanCommIfCfgObj=NULL;
      CmsRet ret;

      int instanceOfWANDevice;

      instanceOfWANDevice = psvc->device->instance;

      PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);

      if(strlen(pParams->value)==0)
      {
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }

      if ((ret = cmsObj_get(MDMOID_WAN_COMMON_INTF_CFG, &iidStack, 0, (void *) &wanCommIfCfgObj)) != CMSRET_SUCCESS)
      {
         cmsLog_error("get of MDMOID_WAN_COMMON_INTF_CFG failed, ret=%d", ret);
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }

      if( strcmp(pParams->value,"0") == 0 )
      {
         wanCommIfCfgObj->enabledForInternet = 0;
      }
      else
      {
         wanCommIfCfgObj->enabledForInternet = 1;
      }

      ret = cmsObj_set(wanCommIfCfgObj, &iidStack);
      cmsObj_free((void **) &wanCommIfCfgObj);;

      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("set of MDMOID_WAN_COMMON_INTF_CFG failed, ret=%d", ret);
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }

   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
#endif /*0 cms lib doesn't support this parameter now */
   return TRUE;
}


int WANCommonInterfaceConfig_GetVar(struct Service *psvc, int varindex)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack sonIidStack = EMPTY_INSTANCE_ID_STACK;

//   WanDevObject *wanDevObj=NULL;
   WanCommonIntfCfgObject *wanCommIfCfgObj=NULL;
   WanPppConnStatsObject *pppConnStats = NULL; 
   WanIpConnStatsObject *ipConnStats = NULL;   
   CmsRet ret;

   int totalBytesSent = 0;
   int totalBytesReceived = 0;
   int totalPacketsSent = 0;
   int totalPacketsReceived = 0;

   int instanceOfWANDevice;
   struct StateVar *var;
   var = &(psvc->vars[varindex]);
   
   instanceOfWANDevice = psvc->instance;

   INIT_INSTANCE_ID_STACK(&iidStack);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   INIT_INSTANCE_ID_STACK(&sonIidStack);
   while (cmsObj_getNextInSubTree(MDMOID_WAN_PPP_CONN_STATS, &iidStack, &sonIidStack,(void **)&pppConnStats) == CMSRET_SUCCESS)
   {
      totalBytesSent = totalBytesSent + pppConnStats->ethernetBytesSent;
      totalPacketsSent = totalPacketsSent + pppConnStats->ethernetPacketsSent;
      totalBytesReceived = totalBytesReceived + pppConnStats->ethernetBytesReceived;
      totalPacketsReceived = totalPacketsReceived + pppConnStats->ethernetPacketsReceived;
      cmsObj_free((void **) &pppConnStats);
   }

   INIT_INSTANCE_ID_STACK(&iidStack);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   INIT_INSTANCE_ID_STACK(&sonIidStack);
   while (cmsObj_getNextInSubTree(MDMOID_WAN_IP_CONN_STATS, &iidStack, &sonIidStack,(void **)&ipConnStats) == CMSRET_SUCCESS)
   {
      totalBytesSent = totalBytesSent + ipConnStats->ethernetBytesSent;
      totalPacketsSent = totalPacketsSent + ipConnStats->ethernetPacketsSent;
      totalBytesReceived = totalBytesReceived + ipConnStats->ethernetBytesReceived;
      totalPacketsReceived = totalPacketsReceived + ipConnStats->ethernetPacketsReceived;
      cmsObj_free((void **) &ipConnStats);
   }

   INIT_INSTANCE_ID_STACK(&iidStack);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);

      if ((ret = cmsObj_get(MDMOID_WAN_COMMON_INTF_CFG, &iidStack, 0, (void *) &wanCommIfCfgObj)) == CMSRET_SUCCESS)
      {
         switch (varindex)
         {
            case VAR_WANAccessType:
               strcpy(var->value, wanCommIfCfgObj->WANAccessType);
               break;
            case VAR_TotalBytesSent:
            sprintf(var->value, "%u", totalBytesSent);
               break;
            case VAR_TotalBytesReceived:
            sprintf(var->value, "%u", totalBytesReceived);
               break;
            case VAR_TotalPacketsSent:
            sprintf(var->value, "%u", totalPacketsSent);
               break;
            case VAR_TotalPacketsReceived:
            sprintf(var->value, "%u", totalPacketsReceived);
               break;
            case VAR_Layer1UpstreamMaxBitRate:
               sprintf(var->value, "%u", wanCommIfCfgObj->layer1UpstreamMaxBitRate);
               break;
            case VAR_Layer1DownstreamMaxBitRate:
               sprintf(var->value, "%u", wanCommIfCfgObj->layer1DownstreamMaxBitRate);
               break;
            case VAR_PhysicalLinkStatus:
               strcpy(var->value,wanCommIfCfgObj->physicalLinkStatus);
               break;
#if 0
               /* cms_core lib doesn't support this parameter yet */
            case VAR_EnabledForInternet:
               if(wanCommIfCfgObj->enabledForInternet)
               {
                  strcpy(var->value,"1");
               }
               else
               {
                  strcpy(var->value, "0");
               }
               break;
#endif /* 0 */
         }
         cmsObj_free((void **)&wanCommIfCfgObj);
      }
   return TRUE;
}


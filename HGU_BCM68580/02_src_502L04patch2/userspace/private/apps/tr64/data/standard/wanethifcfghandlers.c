/*****************************************************************************
//
// Copyright (c) 2005-2012 Broadcom Corporation
// All Rights Reserved
//
// <:label-BRCM:2012:proprietary:standard
// 
//  This program is the proprietary software of Broadcom and/or its
//  licensors, and may only be used, duplicated, modified or distributed pursuant
//  to the terms and conditions of a separate, written license agreement executed
//  between you and Broadcom (an "Authorized License").  Except as set forth in
//  an Authorized License, Broadcom grants no license (express or implied), right
//  to use, or waiver of any kind with respect to the Software, and Broadcom
//  expressly reserves all rights in and to the Software and all intellectual
//  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
//  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
//  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
// 
//  Except as expressly set forth in the Authorized License,
// 
//  1. This program, including its structure, sequence and organization,
//     constitutes the valuable trade secrets of Broadcom, and you shall use
//     all reasonable efforts to protect the confidentiality thereof, and to
//     use this information only in connection with your use of Broadcom
//     integrated circuit products.
// 
//  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
//     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
//     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
//     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
//     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
//     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
//     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
//     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
//     PERFORMANCE OF THE SOFTWARE.
// 
//  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
//     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
//     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
//     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
//     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
//     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
//     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
//     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
//     LIMITED REMEDY.
// :>
//
******************************************************************************
//
//  Filename:       wanethifcfghandler.c
//
******************************************************************************/
#ifdef INCLUDE_WANETHERNETCONFIG
#ifdef DMP_ETHERNETWAN_1
#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "igd.h"
#include "wanethifcfgparams.h"
#include "tr64defs.h"

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"

int WANETHInterfaceConfig_GetVar(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{

   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanEthIntfObject *ethIntfCfg = NULL;
   CmsRet ret;   
   int instanceOfWANDevice;

   instanceOfWANDevice = psvc->device->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);

   ret = cmsObj_get(MDMOID_WAN_ETH_INTF, &iidStack, 0, (void **) &ethIntfCfg);
   if(ret != CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   OutputNumValueToAC(ac, VAR_Enable, ethIntfCfg->enable);  
   OutputCharValueToAC(ac, VAR_Status, ethIntfCfg->status);      
   OutputCharValueToAC(ac, VAR_MACAddress, ethIntfCfg->MACAddress);      
   OutputCharValueToAC(ac, VAR_MaxBitRate, ethIntfCfg->maxBitRate); 

   cmsObj_free((void **) &ethIntfCfg);
   return TRUE;
}

int SetETHInterfaceEnable(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)

{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanEthIntfObject *ethIntfCfg = NULL;
   CmsRet ret;

   struct Param *pParams;
   int instanceOfWANDevice;

   instanceOfWANDevice = psvc->device->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
      
   pParams = findActionParamByRelatedVar(ac,VAR_Enable);
   if (pParams != NULL)
   {
      if(strlen(pParams->value)==0)
      {
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }

      if ((ret = cmsObj_get(MDMOID_WAN_ETH_INTF, &iidStack, 0, (void *) &ethIntfCfg)) != CMSRET_SUCCESS)
      {
         cmsLog_error("get of MDMOID_WAN_ETH_INTF failed, ret=%d", ret);
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }

      if( strcmp(pParams->value,"0") == 0 )
      {
         ethIntfCfg->enable= 0;
      }
      else
      {
         ethIntfCfg->enable= 1;
      }

      ret = cmsObj_set(ethIntfCfg, &iidStack);
      cmsObj_free((void **) &ethIntfCfg);;

      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("set of MDMOID_WAN_ETH_INTF failed, ret=%d", ret);
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }

   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   cmsMgm_saveConfigToFlash();

   return TRUE;

}

int SetMaxBitRate(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)

{

   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanEthIntfObject *ethIntfCfg = NULL;
   CmsRet ret;

   struct Param *pParams;
   int instanceOfWANDevice;

   instanceOfWANDevice = psvc->device->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
      
   pParams = findActionParamByRelatedVar(ac,VAR_MaxBitRate);
   if (pParams != NULL)
   {
      if(strlen(pParams->value)==0)
      {
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }

      if ((ret = cmsObj_get(MDMOID_WAN_ETH_INTF, &iidStack, 0, (void *) &ethIntfCfg)) != CMSRET_SUCCESS)
      {
         cmsLog_error("get of MDMOID_WAN_ETH_INTF failed, ret=%d", ret);
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }
      cmsMem_free(ethIntfCfg->maxBitRate);
      ethIntfCfg->maxBitRate = cmsMem_strdup(pParams->value);

      ret = cmsObj_set(ethIntfCfg, &iidStack);
      cmsObj_free((void **) &ethIntfCfg);;

      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("set of MDMOID_WAN_ETH_INTF failed, ret=%d", ret);
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }

   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   cmsMgm_saveConfigToFlash();

   return TRUE;
}

int GetStatisticsWANETH(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)

{
   int errorinfo = 0;

   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanEthIntfStatsObject *wanEthIntfSts = NULL;


   int instanceOfWANDevice;

   instanceOfWANDevice = psvc->device->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);

   if (cmsObj_get(MDMOID_WAN_ETH_INTF_STATS, &iidStack, 0, (void **) &wanEthIntfSts) == CMSRET_SUCCESS)
   {  
      errorinfo |= OutputNumValueToAC(ac, VAR_BytesSent, wanEthIntfSts->bytesSent);
      errorinfo |= OutputNumValueToAC(ac, VAR_BytesReceived, wanEthIntfSts->bytesReceived);
      errorinfo |= OutputNumValueToAC(ac, VAR_PacketsSent, wanEthIntfSts->packetsSent);
      errorinfo |= OutputNumValueToAC(ac, VAR_PacketsReceived, wanEthIntfSts->packetsReceived);

      cmsObj_free((void **) &wanEthIntfSts);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   } 

   if(errorinfo)
   {
      soap_error( uclient, errorinfo );
      return FALSE;
   } 
   return TRUE;
}
#endif
#endif

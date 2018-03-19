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
//  Filename:       lanusbifcfghandlers.c
//
******************************************************************************/
#ifdef USB
#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "igd.h"
#include "lanusbifcfgparams.h"

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"

int LANUSBIFCfg_GetInfo(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanUsbIntfObject *usbObj = NULL;
   int instanceOfLANDevice;
   int instanceOfService;

   instanceOfLANDevice = psvc->device->instance;
   instanceOfService = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);

   if (cmsObj_get(MDMOID_LAN_USB_INTF, &iidStack, 0, (void **) &usbObj) != CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   OutputNumValueToAC(ac, VAR_Enable, usbObj->enable);  
   OutputCharValueToAC(ac, VAR_Status, usbObj->status);      
   OutputCharValueToAC(ac, VAR_MACAddress, usbObj->MACAddress);      
   OutputCharValueToAC(ac, VAR_MACAddressControlEnabled, usbObj->MACAddressControlEnabled);   
   OutputCharValueToAC(ac, VAR_Standard, usbObj->standard);      
   OutputCharValueToAC(ac, VAR_Type, usbObj->type);      
   OutputCharValueToAC(ac, VAR_Rate, usbObj->rate);      

   cmsObj_free((void **) &usbObj);
   return TRUE;
}

int LANUSBIFCfg_GetStatistics(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanUsbIntfStatsObject *usbStatsObj=NULL;
   int instanceOfLANDevice;
   int instanceOfService;

   instanceOfLANDevice = psvc->device->instance;
   instanceOfService = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);

   if (cmsObj_get(MDMOID_LAN_USB_INTF_STATS, &iidStack, 0, (void **) &usbStatsObj) != CMSRET_SUCCESS)   
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   OutputNumValueToAC(ac, VAR_BytesSent, usbStatsObj->bytesSent);  
   OutputNumValueToAC(ac, VAR_BytesReceived, usbStatsObj->bytesReceived);      
   OutputNumValueToAC(ac, VAR_CellsSent, usbStatsObj->cellsSent);      
   OutputNumValueToAC(ac, VAR_CellsReceived, usbStatsObj->cellsReceived);   

   cmsObj_free((void **) &usbStatsObj);
   return TRUE;
}

int SetUSBEnable(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanUsbIntfObject *usbObj = NULL;

   int instanceOfLANDevice;
   int instanceOfService;

   instanceOfLANDevice = psvc->device->instance;
   instanceOfService = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);

   struct Param *pParams;
   pParams = findActionParamByRelatedVar(ac,VAR_Enable);
   if (pParams != NULL)
   {

      if (cmsObj_get(MDMOID_LAN_USB_INTF, &iidStack, 0, (void **) &usbObj) == CMSRET_SUCCESS)
      {  
         usbObj->enable = atoi(pParams->value);
         cmsObj_set(usbObj, &iidStack);
         cmsObj_free((void **) &usbObj);
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
#endif


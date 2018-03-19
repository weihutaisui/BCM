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
//  Filename:       deviceinfohandlers.c
//
******************************************************************************/

#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "igd.h"
#include "deviceinfoparams.h"
#include "tr64defs.h"

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"

extern tr64PersistentData *pTr64Data;
extern void setCurrentState(pTr64PersistentData pData);


int DeviceInfo_GetVar(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{

   IGDDeviceInfoObject *devInfoObj;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   if ((ret = cmsObj_get(MDMOID_IGD_DEVICE_INFO, &iidStack, 0, (void *) &devInfoObj)) != CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   OutputCharValueToAC(ac, VAR_Manufacturer, devInfoObj->manufacturer);  
   OutputCharValueToAC(ac, VAR_ManufacturerOUI, devInfoObj->manufacturerOUI);      
   OutputCharValueToAC(ac, VAR_ModelName, devInfoObj->modelName);      
   OutputCharValueToAC(ac, VAR_Description, devInfoObj->description);      
   OutputCharValueToAC(ac, VAR_ProductClass, devInfoObj->productClass);      
   OutputCharValueToAC(ac, VAR_SerialNumber, devInfoObj->serialNumber);      
   OutputCharValueToAC(ac, VAR_SoftwareVersion, devInfoObj->softwareVersion);      
   OutputCharValueToAC(ac, VAR_HardwareVersion, devInfoObj->hardwareVersion);      
   OutputCharValueToAC(ac, VAR_SpecVersion, devInfoObj->specVersion);      
   OutputCharValueToAC(ac, VAR_ProvisioningCode, pTr64Data->provisioningCode);    
   OutputNumValueToAC(ac, VAR_UpTime, devInfoObj->upTime);    

   cmsObj_free((void **) &devInfoObj);

   return TRUE;
}

int SetProvisioningCode(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   struct Param *pParams;

   pParams = findActionParamByRelatedVar(ac,VAR_ProvisioningCode);
   if (pParams != NULL)
   {
      if(pParams->value!=NULL)
      {
         strcpy(pTr64Data->provisioningCode, pParams->value);
      }
      setCurrentState(pTr64Data);
   }
   else
   {
      cmsLog_error("input pParam error");
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   return TRUE;
}

int GetDeviceLog(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   struct Param *pParams;
   int errorinfo = 0;
   InstanceIdStack iidStack;
   IGDDeviceInfoObject *devInfoObj;
   CmsRet ret;


   pParams = findActionParamByRelatedVar(ac, VAR_DeviceLog);
   if (pParams != NULL)
   {
      INIT_INSTANCE_ID_STACK(&iidStack);
      if ((ret = cmsObj_get(MDMOID_IGD_DEVICE_INFO, &iidStack, 0, (void **)&devInfoObj)) == CMSRET_SUCCESS)
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_DeviceLog, devInfoObj->deviceLog);
         cmsObj_free((void **)&devInfoObj);
      }
      else
      {
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }

      if(errorinfo)
      {
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   
   return TRUE;
}


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
//  Filename:       atmf5loophandlers.c
//
******************************************************************************/
#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "igd.h"
#include "atmf5loopparams.h"

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"

int WANATMF5LoopbackDiagnostics_GetVar(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanAtm5LoopbackDiagObject *obj;
   CmsRet ret = CMSRET_INVALID_ARGUMENTS;

   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);

   if ((ret = cmsObj_get(MDMOID_WAN_ATM5_LOOPBACK_DIAG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   OutputCharValueToAC(ac, VAR_DiagnosticsState, obj->diagnosticsState);  
   OutputNumValueToAC(ac, VAR_NumberOfRepetitions, obj->numberOfRepetitions);      
   OutputNumValueToAC(ac, VAR_Timeout, obj->timeout);      
   OutputNumValueToAC(ac, VAR_SuccessCount, obj->successCount);   
   OutputNumValueToAC(ac, VAR_FailureCount, obj->failureCount);      
   OutputNumValueToAC(ac, VAR_AverageResponseTime, obj->averageResponseTime);      
   OutputNumValueToAC(ac, VAR_MinimumResponseTime, obj->minimumResponseTime);      
   OutputNumValueToAC(ac, VAR_MaximumResponseTime, obj->maximumResponseTime); 

   cmsObj_free((void **) &obj);

   return TRUE;
}

int SetDiagnosticsStateATMF5(UFILE *uclient, PService psvc, PAction ac,
                      pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanAtm5LoopbackDiagObject *obj;
   CmsRet ret = CMSRET_INVALID_ARGUMENTS;

   struct Param *pParams;

   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   
   pParams = findActionParamByRelatedVar(ac,VAR_DiagnosticsState);
   if (pParams == NULL)
   {
      cmsLog_error("pParams is null");
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   if ((ret = cmsObj_get(MDMOID_WAN_ATM5_LOOPBACK_DIAG, &iidStack, 0, (void **) &obj)) == CMSRET_SUCCESS)
   {   
      cmsMem_free(obj->diagnosticsState);
      obj->diagnosticsState = cmsMem_strdup(pParams->value);
      ret = cmsObj_set(obj, &iidStack);
      cmsObj_free((void **) &obj);
   }
   else
   {
      cmsLog_error("get of MDMOID_WAN_ATM5_LOOPBACK_DIAG failed, ret=%d", ret);
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   if(ret!=CMSRET_SUCCESS)
   {
      cmsLog_error("set of MDMOID_WAN_ATM5_LOOPBACK_DIAG failed, ret=%d", ret);
      return TRUE;
   }
   return TRUE;
}

int SetNumberOfRepetitionsATMF5(UFILE *uclient, PService psvc, PAction ac,
                      pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanAtm5LoopbackDiagObject *obj;
   CmsRet ret = CMSRET_INVALID_ARGUMENTS;


   struct Param *pParams;

   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   
   pParams = findActionParamByRelatedVar(ac,VAR_NumberOfRepetitions);
   if (pParams == NULL)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   if ((ret = cmsObj_get(MDMOID_WAN_ATM5_LOOPBACK_DIAG, &iidStack, 0, (void **) &obj)) == CMSRET_SUCCESS)
   {   
      obj->numberOfRepetitions = atoi(pParams->value);
      ret = cmsObj_set(obj, &iidStack);
      cmsObj_free((void **) &obj);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   if(ret!=CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   cmsMgm_saveConfigToFlash();
   return TRUE;
}

int SetTimeoutATMF5(UFILE *uclient, PService psvc, PAction ac,
                      pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanAtm5LoopbackDiagObject *obj;
   CmsRet ret = CMSRET_INVALID_ARGUMENTS;

   struct Param *pParams;

   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;
   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
   
   pParams = findActionParamByRelatedVar(ac,VAR_Timeout);
   if (pParams == NULL)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   if ((ret = cmsObj_get(MDMOID_WAN_ATM5_LOOPBACK_DIAG, &iidStack, 0, (void **) &obj)) == CMSRET_SUCCESS)
   {   
      obj->timeout= atoi(pParams->value);
      ret = cmsObj_set(obj, &iidStack);
      cmsObj_free((void **) &obj);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   if(ret!=CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
 
   cmsMgm_saveConfigToFlash();
   return TRUE;
}


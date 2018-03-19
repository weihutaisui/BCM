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
//  Filename:       ippinghandlers.c
//
******************************************************************************/
#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "igd.h"
#include "ippingparams.h"

#include "tr64defs.h"
#include "session.h"

#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"

#define WORK_AROUND_RCL_LAYER 1

#ifdef WORK_AROUND_RCL_LAYER
IPPingDiagObject gPingDiag;
#endif

int IPPing_GetVar(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   IPPingDiagObject *pingDiag;
   CmsRet ret;

   if ((ret = cmsObj_get(MDMOID_IP_PING_DIAG, &iidStack, 0, (void **) &pingDiag)) != CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
#ifdef WORK_AROUND_RCL_LAYER
   if ((cmsUtl_strcmp(pingDiag->diagnosticsState,MDMVS_COMPLETE) == 0)
       ||(cmsUtl_strcmp(pingDiag->diagnosticsState,MDMVS_ERROR_CANNOTRESOLVEHOSTNAME) == 0))
   {

   OutputCharValueToAC(ac, VAR_DiagnosticsState, pingDiag->diagnosticsState);  
   OutputCharValueToAC(ac, VAR_Interface, pingDiag->interface);      
   OutputCharValueToAC(ac, VAR_Host, pingDiag->host);      

   OutputNumValueToAC(ac, VAR_NumberOfRepetitions, pingDiag->numberOfRepetitions);   
   OutputNumValueToAC(ac, VAR_Timeout, pingDiag->timeout);      
   OutputNumValueToAC(ac, VAR_DataBlockSize, pingDiag->dataBlockSize);      
   OutputNumValueToAC(ac, VAR_DSCP, pingDiag->DSCP);      
   }
   else
   {
      if(gPingDiag.diagnosticsState==NULL)
      {
         OutputCharValueToAC(ac, VAR_DiagnosticsState, MDMVS_NONE); 
      }
      else
      {
         OutputCharValueToAC(ac, VAR_DiagnosticsState, gPingDiag.diagnosticsState);  
      }
      OutputCharValueToAC(ac, VAR_Interface, gPingDiag.interface);      
      OutputCharValueToAC(ac, VAR_Host, gPingDiag.host);      
      OutputNumValueToAC(ac, VAR_NumberOfRepetitions, gPingDiag.numberOfRepetitions);   
      OutputNumValueToAC(ac, VAR_Timeout, gPingDiag.timeout);      
      OutputNumValueToAC(ac, VAR_DataBlockSize, gPingDiag.dataBlockSize);      
      OutputNumValueToAC(ac, VAR_DSCP, gPingDiag.DSCP);    
   }
#else
   OutputCharValueToAC(ac, VAR_DiagnosticsState, pingDiag->diagnosticsState);  
   OutputCharValueToAC(ac, VAR_Interface, pingDiag->interface);      
   OutputCharValueToAC(ac, VAR_Host, pingDiag->host);      

   OutputNumValueToAC(ac, VAR_NumberOfRepetitions, pingDiag->numberOfRepetitions);   
   OutputNumValueToAC(ac, VAR_Timeout, pingDiag->timeout);      
   OutputNumValueToAC(ac, VAR_DataBlockSize, pingDiag->dataBlockSize);      
   OutputNumValueToAC(ac, VAR_DSCP, pingDiag->DSCP);  
#endif

   OutputNumValueToAC(ac, VAR_SuccessCount, pingDiag->successCount); 
   OutputNumValueToAC(ac, VAR_FailureCount, pingDiag->failureCount);      
   OutputNumValueToAC(ac, VAR_AverageResponseTime, pingDiag->averageResponseTime);      
   OutputNumValueToAC(ac, VAR_MinimumResponseTime, pingDiag->minimumResponseTime);      
   OutputNumValueToAC(ac, VAR_MaximumResponseTime, pingDiag->maximumResponseTime); 


   cmsObj_free((void **) &pingDiag);

   return TRUE;
}

int SetDiagnosticsStateIPPing(UFILE *uclient, PService psvc, PAction ac,
                      pvar_entry_t args, int nargs)
{
   struct Param *pParams;
   
   pParams = findActionParamByRelatedVar(ac,VAR_DiagnosticsState);
   if (pParams != NULL)
   {
      InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
      IPPingDiagObject *pingDiag;
      CmsRet ret;

      if (strcmp(pParams->value,MDMVS_REQUESTED) != 0)
      {
         soap_error( uclient, SOAP_ACTIONFAILED );
         cmsLog_error(" input parameter value failed");
         return FALSE;
      }


      if ((ret = cmsObj_get(MDMOID_IP_PING_DIAG, &iidStack, 0, (void **) &pingDiag)) == CMSRET_SUCCESS)
      {     
#ifdef WORK_AROUND_RCL_LAYER
         cmsMem_free(gPingDiag.diagnosticsState);    
         gPingDiag.diagnosticsState = cmsMem_strdup(MDMVS_REQUESTED);   

         cmsMem_free(pingDiag->interface);    
         pingDiag->interface = cmsMem_strdup(gPingDiag.interface);   
  
         cmsMem_free(pingDiag->host);    
         pingDiag->host = cmsMem_strdup(gPingDiag.host);   

         pingDiag->numberOfRepetitions = gPingDiag.numberOfRepetitions;
         pingDiag->timeout = gPingDiag.timeout;     
         pingDiag->dataBlockSize = gPingDiag.dataBlockSize;
         pingDiag->DSCP = gPingDiag.DSCP;
#endif    
         cmsMem_free(pingDiag->diagnosticsState);
         pingDiag->diagnosticsState = cmsMem_strdup(MDMVS_REQUESTED);

         ret = cmsObj_set(pingDiag, &iidStack);

         cmsObj_free((void **) &pingDiag);

         if(ret != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsObj_set MDMOID_IP_PING_DIAG failed ret=%d", ret);
            soap_error( uclient, SOAP_ACTIONFAILED );
            return FALSE;
         }
      }
      else
      {
         cmsLog_error("cmsObj_get MDMOID_IP_PING_DIAG failed ret=%d", ret);
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }
   }
   else
   {
      cmsLog_error("input pParam->value is NULL");
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   return TRUE;
}

int SetInterfaceIPPing(UFILE *uclient, PService psvc, PAction ac,
                      pvar_entry_t args, int nargs)
{
   struct Param *pParams;
   
   pParams = findActionParamByRelatedVar(ac,VAR_Interface);
   if (pParams != NULL)
   {
#ifdef WORK_AROUND_RCL_LAYER
      cmsMem_free(gPingDiag.interface);
      gPingDiag.interface= cmsMem_strdup(pParams->value);      
#else
      InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
      IPPingDiagObject *pingDiag;
      CmsRet ret;

      if ((ret = cmsObj_get(MDMOID_IP_PING_DIAG, &iidStack, 0, (void **) &pingDiag)) == CMSRET_SUCCESS)
      {     
         cmsMem_free(pingDiag->interface);
         pingDiag->interface= cmsMem_strdup(pParams->value);
         ret = cmsObj_set(pingDiag, &iidStack);

         cmsObj_free((void **) &pingDiag);

         if(ret != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsObj_set MDMOID_IP_PING_DIAG failed ret=%d", ret);
            soap_error( uclient, SOAP_ACTIONFAILED );
            return FALSE;
         }
      }
      else
      {
         cmsLog_error("cmsObj_get MDMOID_IP_PING_DIAG failed ret=%d", ret);
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }
#endif
   }
   else
   {
      cmsLog_error("input pParam->value is NULL");
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   return TRUE;

}

int SetHostIPPing(UFILE *uclient, PService psvc, PAction ac,
                      pvar_entry_t args, int nargs)
{
   struct Param *pParams;
   
   pParams = findActionParamByRelatedVar(ac,VAR_Host);
   if (pParams != NULL)
   {

#ifdef WORK_AROUND_RCL_LAYER
      cmsMem_free(gPingDiag.host);
      gPingDiag.host = cmsMem_strdup(pParams->value);      
#else
      InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
      IPPingDiagObject *pingDiag;
      CmsRet ret;

      if ((ret = cmsObj_get(MDMOID_IP_PING_DIAG, &iidStack, 0, (void **) &pingDiag)) == CMSRET_SUCCESS)
      {     
         cmsMem_free(pingDiag->host);
         pingDiag->host= cmsMem_strdup(pParams->value);

         ret = cmsObj_set(pingDiag, &iidStack);

         cmsObj_free((void **) &pingDiag);

         if(ret != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsObj_set MDMOID_IP_PING_DIAG failed ret=%d", ret);
            soap_error( uclient, SOAP_ACTIONFAILED );
            return FALSE;
         }
      }
      else
      {
         cmsLog_error("cmsObj_get MDMOID_IP_PING_DIAG failed ret=%d", ret);
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }
#endif
   }
   else
   {
      cmsLog_error("input pParam is NULL");
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   return TRUE;
}

int NumberOfRepetitionsIPPing(UFILE *uclient, PService psvc, PAction ac,
                      pvar_entry_t args, int nargs)
{
   struct Param *pParams;
   
   pParams = findActionParamByRelatedVar(ac,VAR_NumberOfRepetitions);
   if (pParams != NULL)
   {
#ifdef WORK_AROUND_RCL_LAYER
      gPingDiag.numberOfRepetitions = atoi(pParams->value);
#else
      InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
      IPPingDiagObject *pingDiag;
      CmsRet ret;

      if ((ret = cmsObj_get(MDMOID_IP_PING_DIAG, &iidStack, 0, (void **) &pingDiag)) == CMSRET_SUCCESS)
      {     
         pingDiag->numberOfRepetitions = atoi(pParams->value);
         ret = cmsObj_set(pingDiag, &iidStack);
         cmsObj_free((void **) &pingDiag);
         if(ret != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsObj_set MDMOID_IP_PING_DIAG failed ret=%d", ret);
            soap_error( uclient, SOAP_ACTIONFAILED );
            return FALSE;
         }
      }
      else
      {
         cmsLog_error("cmsObj_get MDMOID_IP_PING_DIAG failed ret=%d", ret);
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }
#endif
   }
   else
   {
      cmsLog_error("input pParam is NULL");
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   return TRUE;
}

int SetTimeoutIPPing(UFILE *uclient, PService psvc, PAction ac,
                      pvar_entry_t args, int nargs)
{
   struct Param *pParams;
   
   pParams = findActionParamByRelatedVar(ac,VAR_Timeout);
   if (pParams != NULL)
   {
#ifdef WORK_AROUND_RCL_LAYER
      gPingDiag.timeout = atoi(pParams->value);
#else
      InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
      IPPingDiagObject *pingDiag;
      CmsRet ret;

      if ((ret = cmsObj_get(MDMOID_IP_PING_DIAG, &iidStack, 0, (void **) &pingDiag)) == CMSRET_SUCCESS)
      {     
         pingDiag->timeout = atoi(pParams->value);

         ret = cmsObj_set(pingDiag, &iidStack);
         cmsObj_free((void **) &pingDiag);
         if(ret != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsObj_set MDMOID_IP_PING_DIAG failed ret=%d", ret);
            soap_error( uclient, SOAP_ACTIONFAILED );
            return FALSE;
         }
      }
      else
      {
         cmsLog_error("cmsObj_get MDMOID_IP_PING_DIAG failed ret=%d", ret);
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }
#endif
   }
   else
   {
      cmsLog_error("input pParams is NULL");
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   return TRUE;
}

int SetDataBlockSizeIPPing(UFILE *uclient, PService psvc, PAction ac,
                      pvar_entry_t args, int nargs)
{
   struct Param *pParams;
   
   pParams = findActionParamByRelatedVar(ac,VAR_DataBlockSize);
   if (pParams != NULL)
   {
#ifdef WORK_AROUND_RCL_LAYER
      gPingDiag.dataBlockSize = atoi(pParams->value);
#else
      InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
      IPPingDiagObject *pingDiag;
      CmsRet ret;

      if ((ret = cmsObj_get(MDMOID_IP_PING_DIAG, &iidStack, 0, (void **) &pingDiag)) == CMSRET_SUCCESS)
      {     
         pingDiag->dataBlockSize = atoi(pParams->value);
         ret = cmsObj_set(pingDiag, &iidStack);
         cmsObj_free((void **) &pingDiag);
         if(ret != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsObj_set MDMOID_IP_PING_DIAG failed ret=%d", ret);
            soap_error( uclient, SOAP_ACTIONFAILED );
            return FALSE;
         }
      }
      else
      {
         cmsLog_error("cmsObj_get MDMOID_IP_PING_DIAG failed ret=%d", ret);
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }
#endif
   }
   else
   {
      cmsLog_error("input pParams is NULL");
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   return TRUE;
}

int SetDSCPIPPing(UFILE *uclient, PService psvc, PAction ac,
                      pvar_entry_t args, int nargs)
{
   struct Param *pParams;
   
   pParams = findActionParamByRelatedVar(ac,VAR_DSCP);
   if (pParams != NULL)
   {
#ifdef WORK_AROUND_RCL_LAYER
      gPingDiag.DSCP = atoi(pParams->value);
#else
      InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
      IPPingDiagObject *pingDiag;
      CmsRet ret;

      if ((ret = cmsObj_get(MDMOID_IP_PING_DIAG, &iidStack, 0, (void **) &pingDiag)) == CMSRET_SUCCESS)
      {     
         pingDiag->DSCP = atoi(pParams->value);

         ret = cmsObj_set(pingDiag, &iidStack);
         cmsObj_free((void **) &pingDiag);
         if(ret != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsObj_set MDMOID_IP_PING_DIAG failed ret=%d", ret);
            soap_error( uclient, SOAP_ACTIONFAILED );
            return FALSE;
         }
      }
      else
      {
         cmsLog_error("cmsObj_get MDMOID_IP_PING_DIAG failed ret=%d", ret);
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }
#endif
   }
   else
   {
      cmsLog_error("input pParams is NULL");
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   return TRUE;
}

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
//  Filename:       timehandlers.c
//
******************************************************************************/
#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "igd.h"
#include "timeparams.h"
#include "tr64defs.h"

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"


int TimeServer_GetVar(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   TimeServerCfgObject *ntpCfg=NULL;
   CmsRet ret = CMSRET_SUCCESS;

   if ((ret = cmsObj_get(MDMOID_TIME_SERVER_CFG, &iidStack, 0, (void *) &ntpCfg)) != CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   OutputCharValueToAC(ac, VAR_NTPServer1, ntpCfg->NTPServer1);  
   OutputCharValueToAC(ac, VAR_NTPServer2, ntpCfg->NTPServer2);      
   OutputCharValueToAC(ac, VAR_NTPServer3, ntpCfg->NTPServer3);      
   OutputCharValueToAC(ac, VAR_NTPServer4, ntpCfg->NTPServer4);   
   OutputCharValueToAC(ac, VAR_NTPServer5, ntpCfg->NTPServer5);      
   OutputCharValueToAC(ac, VAR_CurrentLocalTime, ntpCfg->currentLocalTime);      
   OutputCharValueToAC(ac, VAR_LocalTimeZone, ntpCfg->localTimeZone);      
   OutputCharValueToAC(ac, VAR_LocalTimeZoneName, ntpCfg->localTimeZoneName);      
   OutputNumValueToAC(ac, VAR_DaylightSavingsUsed, ntpCfg->daylightSavingsUsed);  
   OutputCharValueToAC(ac, VAR_DaylightSavingsStart, ntpCfg->daylightSavingsStart);      
   OutputCharValueToAC(ac, VAR_DaylightSavingsEnd, ntpCfg->daylightSavingsEnd);  

   cmsObj_free((void **) &ntpCfg);

   return TRUE;
}

int SetNTPServers(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   TimeServerCfgObject *ntpCfg=NULL;
   CmsRet ret = CMSRET_SUCCESS;

   struct Param *pParams;
   char NTPServer1[64];
   char NTPServer2[64];
   char NTPServer3[64];
   char NTPServer4[64];
   char NTPServer5[64];

   pParams = findActionParamByRelatedVar(ac,VAR_NTPServer1);
   if (pParams != NULL)
   {
      if(strlen(pParams->value)==0)
      {
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }
      strcpy(NTPServer1, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   pParams = findActionParamByRelatedVar(ac,VAR_NTPServer2);
   if (pParams != NULL)
   {
      if(strlen(pParams->value)==0)
      {
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }
      strcpy(NTPServer2, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   pParams = findActionParamByRelatedVar(ac,VAR_NTPServer3);
   if (pParams != NULL)
   {
      if(strlen(pParams->value)==0)
      {
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }
      strcpy(NTPServer3, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   pParams = findActionParamByRelatedVar(ac,VAR_NTPServer4);
   if (pParams != NULL)
   {
      if(strlen(pParams->value)==0)
      {
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }
      strcpy(NTPServer4, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   pParams = findActionParamByRelatedVar(ac,VAR_NTPServer5);
   if (pParams != NULL)
   {
      if(strlen(pParams->value)==0)
      {
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }
      strcpy(NTPServer5, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }


   if ((ret = cmsObj_get(MDMOID_TIME_SERVER_CFG, &iidStack, 0, (void *) &ntpCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of TIME_SERVER_CFG failed, ret=%d", ret);
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   
   CMSMEM_REPLACE_STRING(ntpCfg->NTPServer1, NTPServer1);
   CMSMEM_REPLACE_STRING(ntpCfg->NTPServer2, NTPServer2);
   CMSMEM_REPLACE_STRING(ntpCfg->NTPServer3, NTPServer3);
   CMSMEM_REPLACE_STRING(ntpCfg->NTPServer4, NTPServer4);
   CMSMEM_REPLACE_STRING(ntpCfg->NTPServer5, NTPServer5);

   ret = cmsObj_set(ntpCfg, &iidStack);

   cmsObj_free((void **) &ntpCfg);
   
   if(ret!=CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   cmsMgm_saveConfigToFlash();
   return TRUE;
}

int SetLocalTimeZone(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   TimeServerCfgObject *ntpCfg=NULL;
   CmsRet ret = CMSRET_SUCCESS;

   struct Param *pParams;
   char LocalTimeZone[6];
   char LocalTimeZoneName[64];
   char DaylightSavingsUsed[4];
   char DaylightSavingsStart[64];
   char DaylightSavingsEnd[64];

   pParams = findActionParamByRelatedVar(ac,VAR_LocalTimeZone);
   if (pParams != NULL)
   {
      strcpy(LocalTimeZone, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   pParams = findActionParamByRelatedVar(ac,VAR_LocalTimeZoneName);
   if (pParams != NULL)
   {
      strcpy(LocalTimeZoneName, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   pParams = findActionParamByRelatedVar(ac,VAR_DaylightSavingsUsed);
   if (pParams != NULL)
   {
      strcpy(DaylightSavingsUsed, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   pParams = findActionParamByRelatedVar(ac,VAR_DaylightSavingsStart);
   if (pParams != NULL)
   {
      strcpy(DaylightSavingsStart, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   pParams = findActionParamByRelatedVar(ac,VAR_DaylightSavingsEnd);
   if (pParams != NULL)
   {
      strcpy(DaylightSavingsEnd, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   if ((ret = cmsObj_get(MDMOID_TIME_SERVER_CFG, &iidStack, 0, (void *) &ntpCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of TIME_SERVER_CFG failed, ret=%d", ret);
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   
   CMSMEM_REPLACE_STRING(ntpCfg->localTimeZone, LocalTimeZone);

   CMSMEM_REPLACE_STRING(ntpCfg->localTimeZoneName, LocalTimeZoneName);
   ntpCfg->daylightSavingsUsed = atoi(DaylightSavingsUsed);
   CMSMEM_REPLACE_STRING(ntpCfg->daylightSavingsStart, DaylightSavingsStart);
   CMSMEM_REPLACE_STRING(ntpCfg->daylightSavingsEnd, DaylightSavingsEnd);

   ret = cmsObj_set(ntpCfg, &iidStack);

   cmsObj_free((void **) &ntpCfg);
   
   if(ret!=CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   cmsMgm_saveConfigToFlash();
   return TRUE;
}

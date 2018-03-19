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
//  Filename:       mgtserverhandlers.c
//
******************************************************************************/
#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "igd.h"
#include "mgtserverparams.h"
#include "tr64defs.h"
#include <time.h>
#include <sys/time.h>

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"

int MgtServer_GetVar(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   ManagementServerObject *acsCfg=NULL;
   CmsRet ret;

   if ((ret = cmsObj_get(MDMOID_MANAGEMENT_SERVER, &iidStack, 0, (void *) &acsCfg)) != CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   OutputCharValueToAC(ac, VAR_URL, acsCfg->URL);  
   OutputNumValueToAC(ac, VAR_PeriodicInformEnable, acsCfg->periodicInformEnable);      
   OutputNumValueToAC(ac, VAR_PeriodicInformInterval, acsCfg->periodicInformInterval);      
   OutputCharValueToAC(ac, VAR_PeriodicInformTime, acsCfg->periodicInformTime);   
   OutputCharValueToAC(ac, VAR_ParameterKey, acsCfg->parameterKey);      
   OutputCharValueToAC(ac, VAR_ParameterHash, "");      
   OutputCharValueToAC(ac, VAR_ConnectionRequestURL, acsCfg->connectionRequestURL);      
   OutputCharValueToAC(ac, VAR_ConnectionRequestUsername, acsCfg->connectionRequestUsername);      
   OutputNumValueToAC(ac, VAR_UpgradesManaged, acsCfg->upgradesManaged);      
 
   cmsObj_free((void **)&acsCfg);
   return TRUE;
}

int SetUpgradesManagement(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{

   struct Param *pParams;
      
   pParams = findActionParamByRelatedVar(ac,VAR_UpgradesManaged);
   if (pParams != NULL)
   {
      InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
      ManagementServerObject *acsCfg=NULL;
      CmsRet ret;

      if(strlen(pParams->value)==0)
      {
         cmsLog_error("input pParams->value is NULL");
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }

      if ((ret = cmsObj_get(MDMOID_MANAGEMENT_SERVER, &iidStack, 0, (void *) &acsCfg)) != CMSRET_SUCCESS)
      {
         cmsLog_error("get of MANAGEMENT_SERVER failed, ret=%d", ret);
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }

      if( strcmp(pParams->value,"0") == 0 )
      {
         acsCfg->upgradesManaged= 0;
      }
      else
      {
         acsCfg->upgradesManaged = 1;
      }

      ret = cmsObj_set(acsCfg, &iidStack);
      cmsObj_free((void **) &acsCfg);;

      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("set of MANAGEMENT_SERVER failed, ret=%d", ret);
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }

   }
   else
   {
      cmsLog_error("input pParams is NULL");
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   cmsMgm_saveConfigToFlash();
   return TRUE;
}

int SetManagementServerURL(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   struct Param *pParams;
      
   pParams = findActionParamByRelatedVar(ac,VAR_URL);
   if (pParams != NULL)
   {
      InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
      ManagementServerObject *acsCfg=NULL;
      CmsRet ret;

      if(strlen(pParams->value)==0)
      {
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }

      if ((ret = cmsObj_get(MDMOID_MANAGEMENT_SERVER, &iidStack, 0, (void *) &acsCfg)) != CMSRET_SUCCESS)
      {
         cmsLog_error("get of MANAGEMENT_SERVER failed, ret=%d", ret);
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }
      cmsMem_free(acsCfg->URL);
      acsCfg->URL = cmsMem_strdup(pParams->value);

      ret = cmsObj_set(acsCfg, &iidStack);
      cmsObj_free((void **) &acsCfg);;

      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("set of MANAGEMENT_SERVER failed, ret=%d", ret);
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

int SetManagementServerPassword(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   struct Param *pParams;
   
   pParams = findActionParamByRelatedVar(ac,VAR_Password);
   if (pParams != NULL)
   {
      InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
      ManagementServerObject *acsCfg=NULL;
      CmsRet ret;

      if(strlen(pParams->value)==0)
      {
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }

      if ((ret = cmsObj_get(MDMOID_MANAGEMENT_SERVER, &iidStack, 0, (void *) &acsCfg)) != CMSRET_SUCCESS)
      {
         cmsLog_error("get of MANAGEMENT_SERVER failed, ret=%d", ret);
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }

      cmsMem_free(acsCfg->password);
      acsCfg->password = cmsMem_strdup(pParams->value);

      ret = cmsObj_set(acsCfg, &iidStack);
      cmsObj_free((void **) &acsCfg);;

      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("set of MANAGEMENT_SERVER failed, ret=%d", ret);
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

int SetPeriodicInform(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   struct Param *pParams;
   static char PeriodicInformEnable[4];
   int PeriodicInformInterval;
   char PeriodicInformTime[64];

   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   ManagementServerObject *acsCfg=NULL;
   CmsRet ret;
   
   pParams = findActionParamByRelatedVar(ac,VAR_PeriodicInformEnable);
   if (pParams != NULL)
   {
      if(strlen(pParams->value)==0)
      {
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }
      strcpy(PeriodicInformEnable, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   pParams = findActionParamByRelatedVar(ac,VAR_PeriodicInformInterval);
   if (pParams != NULL)
   {
      if(strlen(pParams->value)==0)
      {
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }
      PeriodicInformInterval = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   pParams = findActionParamByRelatedVar(ac,VAR_PeriodicInformTime);
   if (pParams != NULL)
   {
      if(strlen(pParams->value)==0)
      {
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }
      strcpy(PeriodicInformTime, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }   

   if ((ret = cmsObj_get(MDMOID_MANAGEMENT_SERVER, &iidStack, 0, (void *) &acsCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of MANAGEMENT_SERVER failed, ret=%d", ret);
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   acsCfg->periodicInformEnable= atoi(PeriodicInformEnable);
   acsCfg->periodicInformInterval = PeriodicInformInterval;
   cmsMem_free(acsCfg->periodicInformTime);
   acsCfg->periodicInformTime= cmsMem_strdup(PeriodicInformTime);

   ret = cmsObj_set(acsCfg, &iidStack);
   cmsObj_free((void **) &acsCfg);;

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("set of MANAGEMENT_SERVER failed, ret=%d", ret);
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   cmsMgm_saveConfigToFlash();
   return TRUE;
}

int SetConnectionRequestAuthentication(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   struct Param *pParams;
   char ConnectionRequestUsername[256];
   char ConnectionRequestPassword[256];
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   ManagementServerObject *acsCfg=NULL;
   CmsRet ret;
      
   pParams = findActionParamByRelatedVar(ac,VAR_ConnectionRequestUsername);
   if (pParams != NULL)
   {
      strcpy(ConnectionRequestUsername, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   pParams = findActionParamByRelatedVar(ac,VAR_ConnectionRequestPassword);
   if (pParams != NULL)
   {
      strcpy(ConnectionRequestPassword, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   if ((ret = cmsObj_get(MDMOID_MANAGEMENT_SERVER, &iidStack, 0, (void *) &acsCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of MANAGEMENT_SERVER failed, ret=%d", ret);
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   cmsMem_free(acsCfg->connectionRequestUsername);
   acsCfg->connectionRequestUsername= cmsMem_strdup(ConnectionRequestUsername);
   cmsMem_free(acsCfg->connectionRequestPassword);
   acsCfg->connectionRequestPassword= cmsMem_strdup(ConnectionRequestPassword);

   ret = cmsObj_set(acsCfg, &iidStack);
   cmsObj_free((void **) &acsCfg);;

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("set of MANAGEMENT_SERVER failed, ret=%d", ret);
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }   
   cmsMgm_saveConfigToFlash();
   return TRUE;
}

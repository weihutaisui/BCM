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
//  Filename:       lanhosthanlders.c
//
******************************************************************************/
#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "igd.h"

#include "lanhostsparams.h"

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"

int GetHostNumberOfEntries(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
      
{
   InstanceIdStack iidStack;
   LanHostsObject *lanHostsObj;
   CmsRet ret;
   uint32 numOfEntries = 0;
   int errorinfo = 0;
   
   int instanceOfLANDevice;

   instanceOfLANDevice = psvc->device->instance;

   INIT_INSTANCE_ID_STACK(&iidStack);

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);

   if ((ret = cmsObj_get(MDMOID_LAN_HOSTS, &iidStack, 0, (void **)&lanHostsObj)) == CMSRET_SUCCESS)
   {
      numOfEntries = lanHostsObj->hostNumberOfEntries;
      cmsObj_free((void **)&lanHostsObj);
   }
   else
   {
       soap_error( uclient, SOAP_ACTIONFAILED );
       return FALSE;
   }
   
   errorinfo |= OutputNumValueToAC(ac, VAR_HostNumberOfEntries, numOfEntries);
   if(errorinfo)
   {
      soap_error( uclient, errorinfo );
      return FALSE;
   }
 
    return TRUE;
}


int GetGenericHostEntry(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   uint32 index = 0;
   int errorinfo = 0;

   InstanceIdStack lanDevIidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanHostEntryObject *lanHostsEntryCfg = NULL;
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 found = FALSE;
   
   struct Param *pParams;

   int instanceOfLANDevice;

   instanceOfLANDevice = psvc->device->instance;
   PUSH_INSTANCE_ID(&lanDevIidStack, instanceOfLANDevice);


   pParams = findActionParamByRelatedVar(ac,VAR_HostNumberOfEntries);
   if (pParams != NULL)
   {
       index = atoi(pParams->value);

   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   while ((ret = cmsObj_getNextInSubTree(MDMOID_LAN_HOST_ENTRY, &lanDevIidStack, &iidStack, (void **) &lanHostsEntryCfg)) == CMSRET_SUCCESS)
   {
      if(iidStack.instance[iidStack.currentDepth-1] == index)
      {
         found = TRUE;
         break;
      }
      else
      {
         cmsObj_free((void **) &lanHostsEntryCfg);
      }
   }

   if(found)
   {
      errorinfo |= OutputCharValueToAC(ac, VAR_IPAddress, lanHostsEntryCfg->IPAddress);
      errorinfo |= OutputCharValueToAC(ac, VAR_AddressSource, lanHostsEntryCfg->addressSource);
      errorinfo |= OutputNumValueToAC(ac, VAR_LeaseTimeRemaining, lanHostsEntryCfg->leaseTimeRemaining);
      errorinfo |= OutputCharValueToAC(ac, VAR_MACAddress, lanHostsEntryCfg->MACAddress);
      errorinfo |= OutputCharValueToAC(ac, VAR_InterfaceType, lanHostsEntryCfg->interfaceType);
      errorinfo |= OutputCharValueToAC(ac, VAR_HostName, lanHostsEntryCfg->hostName);
      if(lanHostsEntryCfg->active)
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_Active, "1");
      }
      else
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_Active, "0");
      }
      cmsObj_free((void **) &lanHostsEntryCfg);
   }
   else
   {
      soap_error( uclient, SOAP_SPECIFIEDARRAYINDEXINVALID );
      return FALSE;
   }

   if(errorinfo)
   {
      soap_error( uclient, errorinfo );
      return FALSE;
   }
     
   return TRUE;
}

int GetSpecificHostEntry(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   int errorinfo = 0;
   char MACAddress[32];
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack lanDevIidStack = EMPTY_INSTANCE_ID_STACK;
   LanHostEntryObject *lanHostsEntryCfg = NULL;
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 found = FALSE;   

   struct Param *pParams;


   int instanceOfLANDevice;

   instanceOfLANDevice = psvc->device->instance;
   PUSH_INSTANCE_ID(&lanDevIidStack, instanceOfLANDevice);

   pParams = findActionParamByRelatedVar(ac,VAR_MACAddress);
   if (pParams != NULL)
   {
      strcpy(MACAddress, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
          
   while ((ret = cmsObj_getNextInSubTree(MDMOID_LAN_HOST_ENTRY, &lanDevIidStack, &iidStack, (void **) &lanHostsEntryCfg)) == CMSRET_SUCCESS)
   {
      if( 0 == strcmp(MACAddress, lanHostsEntryCfg->MACAddress) )
      {
         found = TRUE;
         break;
      }
      else
      {
         cmsObj_free((void **) &lanHostsEntryCfg);
      }
   }

   if(found)
   {
      errorinfo |= OutputCharValueToAC(ac, VAR_IPAddress, lanHostsEntryCfg->IPAddress);
      errorinfo |= OutputCharValueToAC(ac, VAR_AddressSource, lanHostsEntryCfg->addressSource);
      errorinfo |= OutputNumValueToAC(ac, VAR_LeaseTimeRemaining, lanHostsEntryCfg->leaseTimeRemaining);
      errorinfo |= OutputCharValueToAC(ac, VAR_InterfaceType, lanHostsEntryCfg->interfaceType);
      if(lanHostsEntryCfg->active)
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_Active, "1");
      }
      else
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_Active, "0");
      }
      errorinfo |= OutputCharValueToAC(ac, VAR_HostName, lanHostsEntryCfg->hostName);

      cmsObj_free((void **) &lanHostsEntryCfg);
   }
   else
   {
      soap_error( uclient, SOAP_SPECIFIEDARRAYINDEXINVALID );
      return FALSE;
   }

   if(errorinfo)
   {
      soap_error( uclient, errorinfo );
      return FALSE;
   }
   
   return TRUE;
}


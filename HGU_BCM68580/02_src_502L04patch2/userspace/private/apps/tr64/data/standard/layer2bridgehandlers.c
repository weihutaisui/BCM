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
//  Filename:       layer2bridgehandlers.c
//
******************************************************************************/
#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "igd.h"
#include "layer2bridgeparams.h"
#include "tr64defs.h"

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"

      
int GetLayer2Bridge_Info(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)       
{
   InstanceIdStack iidStack;
   L2BridgingObject *l2BridgeObj;
   CmsRet ret;

   int errorinfo = 0;
    
   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_get(MDMOID_L2_BRIDGING, &iidStack, 0, (void **)&l2BridgeObj)) == CMSRET_SUCCESS)
   {
      errorinfo |= OutputNumValueToAC(ac, VAR_MaxBridgeEntries, l2BridgeObj->maxBridgeEntries);  
      errorinfo |= OutputNumValueToAC(ac, VAR_MaxFilterEntries, l2BridgeObj->maxFilterEntries);
      errorinfo |= OutputNumValueToAC(ac, VAR_MaxMarkingEntries, l2BridgeObj->maxMarkingEntries);
      errorinfo |= OutputNumValueToAC(ac, VAR_BridgeNumberOfEntries, l2BridgeObj->bridgeNumberOfEntries);
      errorinfo |= OutputNumValueToAC(ac, VAR_FilterNumberOfEntries, l2BridgeObj->filterNumberOfEntries);
      errorinfo |= OutputNumValueToAC(ac, VAR_MarkingNumberOfEntries, l2BridgeObj->markingNumberOfEntries);
      errorinfo |= OutputNumValueToAC(ac, VAR_AvailableInterfaceNumberOfEntries, l2BridgeObj->availableInterfaceNumberOfEntries);
      cmsObj_free((void **)&l2BridgeObj);
   }
   else
   {  
      soap_error( uclient, errorinfo );
      return FALSE;
   }
    
   if(errorinfo)
   {
      soap_error( uclient, errorinfo );
      return FALSE;
   } 
   return TRUE;
}

int AddBridgeEntry( UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   CmsRet                ret        = CMSRET_SUCCESS;
   InstanceIdStack       iidStack   = EMPTY_INSTANCE_ID_STACK;
   L2BridgingEntryObject *l2BridgeObj  = NULL;

   int errorinfo = 0;
   struct Param *pParams;

   char BridgeEnable[4];
   char BridgeName[32];
   char VLANID[4];   
   
   pParams = findActionParamByRelatedVar(ac,VAR_BridgeEnable);
   if (pParams != NULL)
   {
      strcpy(BridgeEnable, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }  

   pParams = findActionParamByRelatedVar(ac,VAR_BridgeName);
   if (pParams != NULL)
   {
      strcpy(BridgeName, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   } 

   pParams = findActionParamByRelatedVar(ac,VAR_VLANID);
   if (pParams != NULL)
   {
      strcpy(VLANID, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   } 

   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_addInstance(MDMOID_L2_BRIDGING_ENTRY, &iidStack)) != CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   /* read it back */
   if ((ret = cmsObj_get(MDMOID_L2_BRIDGING_ENTRY, &iidStack, 0, (void **) &l2BridgeObj)) != CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   cmsMem_free(l2BridgeObj->bridgeName);
   l2BridgeObj->bridgeName = cmsMem_strdup(BridgeName);

   l2BridgeObj->VLANID = atoi(VLANID);

   if(strcmp(BridgeEnable, "0")==0)
   {
      l2BridgeObj->bridgeEnable = 0;
   }
   else
   {
      l2BridgeObj->bridgeEnable = 1;
   }
   errorinfo |= OutputNumValueToAC(ac, VAR_BridgeKey, l2BridgeObj->bridgeKey);

   ret = cmsObj_set(l2BridgeObj, &iidStack);

   cmsObj_free((void **) &l2BridgeObj);
   
   if (ret != CMSRET_SUCCESS)
   {
      soap_error( uclient, errorinfo );
      return FALSE;
   }

   if (errorinfo)
   {
      soap_error( uclient, errorinfo );
      return FALSE;
   }
   cmsMgm_saveConfigToFlash();
   return TRUE;    
}

int DeleteBridgeEntry( UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   CmsRet                ret        = CMSRET_SUCCESS;
   InstanceIdStack       iidStack   = EMPTY_INSTANCE_ID_STACK;
   L2BridgingEntryObject *l2BridgeObj  = NULL;

   int errorinfo = 0;
   struct Param *pParams;

   char BridgeKey[32]; 

   pParams = findActionParamByRelatedVar(ac,VAR_BridgeKey);
   if (pParams != NULL)
   {
      strcpy(BridgeKey, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }    
   
   while ((ret = cmsObj_getNext(MDMOID_L2_BRIDGING_ENTRY, &iidStack, (void **) &l2BridgeObj)) == CMSRET_SUCCESS)
   {
      if(l2BridgeObj->bridgeKey == atoi(BridgeKey))
      {
         cmsObj_free((void **) &l2BridgeObj);
         if ((ret = cmsObj_deleteInstance(MDMOID_L2_BRIDGING_ENTRY, &iidStack)) != CMSRET_SUCCESS)
         {
            errorinfo = SOAP_ACTIONFAILED;
         }
         break;
      }
      cmsObj_free((void **) &l2BridgeObj);
   }
   
   if (errorinfo)
   {
      soap_error( uclient, errorinfo );
      return FALSE;
   }
   return TRUE;    
}
      
int GetSpecificBridgeEntry(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)       
{
   CmsRet                ret        = CMSRET_SUCCESS;
   InstanceIdStack       iidStack   = EMPTY_INSTANCE_ID_STACK;
   L2BridgingEntryObject *l2BridgeObj  = NULL;

   int errorinfo = 0;
   int found = 0;
   struct Param *pParams;

   char BridgeKey[32];

   pParams = findActionParamByRelatedVar(ac,VAR_BridgeKey);
   if (pParams != NULL)
   {
      strcpy(BridgeKey, pParams->value);
   }
   else
   {
      cmsLog_error("input pParams is NULL");
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }     

   while ((ret = cmsObj_getNext(MDMOID_L2_BRIDGING_ENTRY, &iidStack, (void **) &l2BridgeObj)) == CMSRET_SUCCESS)
   {
      if(l2BridgeObj->bridgeKey == atoi(BridgeKey))
      {
         found = 1;
         errorinfo |= OutputCharValueToAC(ac, VAR_BridgeStatus, l2BridgeObj->bridgeStatus);
         errorinfo |= OutputNumValueToAC(ac, VAR_VLANID, l2BridgeObj->VLANID);
         if(l2BridgeObj->bridgeEnable)
         {
            errorinfo |= OutputCharValueToAC(ac, VAR_BridgeEnable, "1");
         }
         else
         {
            errorinfo |= OutputCharValueToAC(ac, VAR_BridgeEnable, "0");
         }
         cmsObj_free((void **) &l2BridgeObj);
         break;
      }
      cmsObj_free((void **) &l2BridgeObj);
   }

   if(found ==0)
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
     
int GetGenericBridgeEntry(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)       
{
   CmsRet                ret        = CMSRET_SUCCESS;
   InstanceIdStack       iidStack   = EMPTY_INSTANCE_ID_STACK;
   L2BridgingEntryObject *l2BridgeObj  = NULL;

   int errorinfo = 0;
   int found = 0;
   struct Param *pParams;

   pParams = findActionParamByRelatedVar(ac,VAR_BridgeNumberOfEntries);

   while ((ret = cmsObj_getNext(MDMOID_L2_BRIDGING_ENTRY, &iidStack, (void **) &l2BridgeObj)) == CMSRET_SUCCESS)
   {
      if(iidStack.instance[iidStack.currentDepth-1] == atoi(pParams->value))
      {
         found = 1;
         errorinfo |= OutputNumValueToAC(ac, VAR_BridgeKey, l2BridgeObj->bridgeKey);
         if(l2BridgeObj->bridgeEnable)
         {
            errorinfo |= OutputCharValueToAC(ac, VAR_BridgeEnable, "1");
         }
         else
         {
            errorinfo |= OutputCharValueToAC(ac, VAR_BridgeEnable, "0");
         }
         errorinfo |= OutputCharValueToAC(ac, VAR_BridgeStatus, l2BridgeObj->bridgeStatus);
         errorinfo |= OutputNumValueToAC(ac, VAR_VLANID, l2BridgeObj->VLANID);

          cmsObj_free((void **) &l2BridgeObj);
          break;
      }
      cmsObj_free((void **) &l2BridgeObj);
   }

   if(found ==0)
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

int SetBridgeEntryEnable(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)       
{
   CmsRet                ret        = CMSRET_SUCCESS;
   InstanceIdStack       iidStack   = EMPTY_INSTANCE_ID_STACK;
   L2BridgingEntryObject *l2BridgeObj  = NULL;

   int errorinfo = 0;
   struct Param *pParams;

   char BridgeKey[32];
   char BridgeEnable[4];

   pParams = findActionParamByRelatedVar(ac,VAR_BridgeKey);
   if (pParams != NULL)
   {
      strcpy(BridgeKey, pParams->value);
   }
   else
   {
      cmsLog_error("input pParams is NULL");
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }     

   pParams = findActionParamByRelatedVar(ac,VAR_BridgeEnable);
   if (pParams != NULL)
   {
      strcpy(BridgeEnable, pParams->value);
   }
   else
   {
      cmsLog_error("input pParams is NULL");
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }  
   
   while ((ret = cmsObj_getNext(MDMOID_L2_BRIDGING_ENTRY, &iidStack, (void **) &l2BridgeObj)) == CMSRET_SUCCESS)
   {
      if(l2BridgeObj->bridgeKey == atoi(BridgeKey))
      {
         if(atoi(BridgeEnable))
         {
            l2BridgeObj->bridgeEnable = 1;
         }
         else
         {
            l2BridgeObj->bridgeEnable = 0;
         }

         if ((ret = cmsObj_set(l2BridgeObj, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Failed to set L2BridgingEntry Object, ret = %d", ret);
            errorinfo = SOAP_ACTIONFAILED;
         }
      }
      cmsObj_free((void **) &l2BridgeObj);
   }
   
   
   if(errorinfo)
   {
      soap_error( uclient, errorinfo );
      return FALSE;
   } 
   return TRUE;
}

int GetSpecificAvailableInterfaceEntry(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)       
{
   CmsRet                ret        = CMSRET_SUCCESS;
   InstanceIdStack       iidStack   = EMPTY_INSTANCE_ID_STACK;
   L2BridgingIntfObject *l2BridgeIntfObj  = NULL;

   int errorinfo = 0;
   int found = 0;
   struct Param *pParams;

   char AvailableInterfaceKey[32];

   pParams = findActionParamByRelatedVar(ac,VAR_AvailableInterfaceKey);
   if (pParams != NULL)
   {
      strcpy(AvailableInterfaceKey, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }     

   while ((ret = cmsObj_getNext(MDMOID_L2_BRIDGING_INTF, &iidStack, (void **) &l2BridgeIntfObj)) == CMSRET_SUCCESS)
   {
      if(l2BridgeIntfObj->availableInterfaceKey== atoi(AvailableInterfaceKey))
      {
         found = 1;
         errorinfo |= OutputCharValueToAC(ac, VAR_InterfaceType, l2BridgeIntfObj->interfaceType);
         errorinfo |= OutputCharValueToAC(ac, VAR_InterfaceReference, l2BridgeIntfObj->interfaceReference);
         cmsObj_free((void **) &l2BridgeIntfObj);
         break;
      }
      cmsObj_free((void **) &l2BridgeIntfObj);
   }

   if(found ==0)
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


int GetGenericAvailableInterfaceEntry(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)       
{

   CmsRet                ret        = CMSRET_SUCCESS;
   InstanceIdStack       iidStack   = EMPTY_INSTANCE_ID_STACK;
   L2BridgingIntfObject *l2BridgeIntfObj  = NULL;

   int errorinfo = 0;
   int found = 0;
   int index = 0;
   struct Param *pParams;

   pParams = findActionParamByRelatedVar(ac,VAR_AvailableInterfaceNumberOfEntries);
   if (pParams != NULL)
   {
      index = atoi(pParams->value);
   }
   else
   {
      cmsLog_error("input pParams is NULL");
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }     

   while ((ret = cmsObj_getNext(MDMOID_L2_BRIDGING_INTF, &iidStack, (void **) &l2BridgeIntfObj)) == CMSRET_SUCCESS)
   {
      if(index == iidStack.instance[iidStack.currentDepth-1])
      {
          found = 1;
          errorinfo |= OutputNumValueToAC(ac, VAR_AvailableInterfaceKey, l2BridgeIntfObj->availableInterfaceKey);
          errorinfo |= OutputCharValueToAC(ac, VAR_InterfaceType, l2BridgeIntfObj->interfaceType);
          errorinfo |= OutputCharValueToAC(ac, VAR_InterfaceReference, l2BridgeIntfObj->interfaceReference);
          cmsObj_free((void **) &l2BridgeIntfObj);
          break;
      }
      cmsObj_free((void **) &l2BridgeIntfObj);
   }

   if(found ==0)
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

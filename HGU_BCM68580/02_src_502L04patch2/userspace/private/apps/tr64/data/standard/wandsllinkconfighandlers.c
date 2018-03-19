/*****************************************************************************
 *
 *  Copyright (c) 2005-2012  Broadcom Corporation
 *  All Rights Reserved
 *
 *  <:label-BRCM:2012:proprietary:standard
 *  
 *   This program is the proprietary software of Broadcom and/or its
 *   licensors, and may only be used, duplicated, modified or distributed pursuant
 *   to the terms and conditions of a separate, written license agreement executed
 *   between you and Broadcom (an "Authorized License").  Except as set forth in
 *   an Authorized License, Broadcom grants no license (express or implied), right
 *   to use, or waiver of any kind with respect to the Software, and Broadcom
 *   expressly reserves all rights in and to the Software and all intellectual
 *   property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 *   NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 *   BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *  
 *   Except as expressly set forth in the Authorized License,
 *  
 *   1. This program, including its structure, sequence and organization,
 *      constitutes the valuable trade secrets of Broadcom, and you shall use
 *      all reasonable efforts to protect the confidentiality thereof, and to
 *      use this information only in connection with your use of Broadcom
 *      integrated circuit products.
 *  
 *   2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *      AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *      WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
 *      RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
 *      ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
 *      FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
 *      COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
 *      TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
 *      PERFORMANCE OF THE SOFTWARE.
 *  
 *   3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
 *      ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
 *      INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
 *      WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 *      IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
 *      OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
 *      SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
 *      SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
 *      LIMITED REMEDY.
 *  :>
 *
 ************************************************************************/

#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "igd.h"

#include "wandsllinkconfig.h"
#include "tr64defs.h"

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"

int WANDSLLinkConfig_GetInfo(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   WanDslLinkCfgObject *dslLinkCfg = NULL;   
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);


   if (cmsObj_get(MDMOID_WAN_DSL_LINK_CFG, &iidStack, 0, (void **)&dslLinkCfg) != CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   OutputNumValueToAC(ac, VAR_Enable, dslLinkCfg->enable);  
   OutputCharValueToAC(ac, VAR_LinkType, dslLinkCfg->linkType);      
   OutputCharValueToAC(ac, VAR_LinkStatus, dslLinkCfg->linkStatus);      
   OutputCharValueToAC(ac, VAR_DestinationAddress, dslLinkCfg->destinationAddress);   
   OutputCharValueToAC(ac, VAR_ATMEncapsulation, dslLinkCfg->ATMEncapsulation);      
   OutputCharValueToAC(ac, VAR_ATMQoS, dslLinkCfg->ATMQoS);      
   OutputNumValueToAC(ac, VAR_ATMPeakCellRate, dslLinkCfg->ATMPeakCellRate);      
   OutputNumValueToAC(ac, VAR_ATMMaximumBurstSize, dslLinkCfg->ATMMaximumBurstSize);      
   OutputNumValueToAC(ac, VAR_ATMSustainableCellRate, dslLinkCfg->ATMSustainableCellRate);  

   cmsObj_free((void **) &dslLinkCfg);
   return TRUE;
}

int WANDSLLinkConfig_GetDSLLinkInfo(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   WanDslLinkCfgObject *dslLinkCfg = NULL;   
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);


   if (cmsObj_get(MDMOID_WAN_DSL_LINK_CFG, &iidStack, 0, (void **)&dslLinkCfg) != CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   OutputCharValueToAC(ac, VAR_LinkType, dslLinkCfg->linkType);      

   cmsObj_free((void **) &dslLinkCfg);
   return TRUE;
}
int GetDestinationAddress(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   WanDslLinkCfgObject *dslLinkCfg = NULL;   
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);


   if (cmsObj_get(MDMOID_WAN_DSL_LINK_CFG, &iidStack, 0, (void **)&dslLinkCfg) != CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   OutputCharValueToAC(ac, VAR_DestinationAddress, dslLinkCfg->destinationAddress);   

   cmsObj_free((void **) &dslLinkCfg);
   return TRUE;
}

int GetATMEncapsulation(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   WanDslLinkCfgObject *dslLinkCfg = NULL;   
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);


   if (cmsObj_get(MDMOID_WAN_DSL_LINK_CFG, &iidStack, 0, (void **)&dslLinkCfg) != CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   OutputCharValueToAC(ac, VAR_ATMEncapsulation, dslLinkCfg->ATMEncapsulation);  

   cmsObj_free((void **) &dslLinkCfg);
   return TRUE;
}


int SetDSLEnable(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{

   WanDslLinkCfgObject *dslLinkCfg = NULL;   
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret=CMSRET_INTERNAL_ERROR;

   struct Param *pParams;

   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
      
   pParams = findActionParamByRelatedVar(ac,VAR_Enable);
   if (pParams != NULL)
   {

      if(strlen(pParams->value)==0)
      {
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }

      if( cmsObj_get(MDMOID_WAN_DSL_LINK_CFG, &iidStack, 0, (void **)&dslLinkCfg) == CMSRET_SUCCESS)
      {
         if( strcmp(pParams->value,"0") == 0 )
         {
            dslLinkCfg->enable= 0;
         }
         else
         {
            dslLinkCfg->enable= 1;
         }
         ret = cmsObj_set(dslLinkCfg, &iidStack);

         cmsObj_free((void **) &dslLinkCfg);
      }
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("set of MDMOID_WAN_DSL_LINK_CFG failed, ret=%d", ret);
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

int SetATMEncapsulation(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanDslLinkCfgObject *dslLinkCfg = NULL;   
   CmsRet ret=CMSRET_INTERNAL_ERROR;

   struct Param *pParams;

   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
      
   pParams = findActionParamByRelatedVar(ac,VAR_ATMEncapsulation);
   if (pParams != NULL)
   {
      if(strlen(pParams->value)==0)
      {
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }

      if( cmsObj_get(MDMOID_WAN_DSL_LINK_CFG, &iidStack, 0, (void **)&dslLinkCfg) == CMSRET_SUCCESS)
      {
         cmsMem_free(dslLinkCfg->ATMEncapsulation);
         dslLinkCfg->ATMEncapsulation = cmsMem_strdup(pParams->value);
         ret = cmsObj_set(dslLinkCfg, &iidStack);
         cmsObj_free((void **) &dslLinkCfg);
      }

      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("set of MDMOID_WAN_DSL_LINK_CFG failed, ret=%d", ret);
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


int SetDestinationAddress(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   WanDslLinkCfgObject *dslLinkCfg = NULL;   
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret =  CMSRET_INTERNAL_ERROR;

   struct Param *pParams;

   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
      
   pParams = findActionParamByRelatedVar(ac,VAR_DestinationAddress);
   if (pParams != NULL)
   {
      if(strlen(pParams->value)==0)
      {
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }

      if( cmsObj_get(MDMOID_WAN_DSL_LINK_CFG, &iidStack, 0, (void **)&dslLinkCfg) == CMSRET_SUCCESS)
      {
         cmsMem_free(dslLinkCfg->destinationAddress);
         dslLinkCfg->destinationAddress = cmsMem_strdup(pParams->value);
         ret = cmsObj_set(dslLinkCfg, &iidStack);
         cmsObj_free((void **) &dslLinkCfg);
      }

      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("set of MDMOID_WAN_DSL_LINK_CFG failed, ret=%d", ret);
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


int SetATMQoS(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   WanDslLinkCfgObject *dslLinkCfg = NULL;   
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   char ATMQoS[32];
   char ATMPeakCellRate[32];
   char ATMMaximumBurstSize[32];
   char ATMSustainableCellRate[32];
   struct Param *pParams;

   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
  
   pParams = findActionParamByRelatedVar(ac,VAR_ATMQoS);
   if (pParams != NULL)
   {
      if(strlen(pParams->value)==0)
      {
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }     

      strcpy(ATMQoS, pParams->value); 
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   pParams = findActionParamByRelatedVar(ac,VAR_ATMPeakCellRate);
   if (pParams != NULL)
   {
      if(strlen(pParams->value)==0)
      {
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }

      strcpy(ATMPeakCellRate, pParams->value); 
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   pParams = findActionParamByRelatedVar(ac,VAR_ATMMaximumBurstSize);
   if (pParams != NULL)
   {
      if(strlen(pParams->value)==0)
      {
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }

      strcpy(ATMMaximumBurstSize, pParams->value); 
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   pParams = findActionParamByRelatedVar(ac,VAR_ATMSustainableCellRate);
   if (pParams != NULL)
   {
      if(strlen(pParams->value)==0)
      {
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }

      strcpy(ATMSustainableCellRate, pParams->value); 
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }   
   
   if( cmsObj_get(MDMOID_WAN_DSL_LINK_CFG, &iidStack, 0, (void **)&dslLinkCfg) == CMSRET_SUCCESS)
   {
      cmsMem_free(dslLinkCfg->ATMQoS);
      dslLinkCfg->ATMQoS= cmsMem_strdup(ATMQoS);

      dslLinkCfg->ATMPeakCellRate = atoi(ATMPeakCellRate);
      dslLinkCfg->ATMMaximumBurstSize = atoi(ATMMaximumBurstSize);
      dslLinkCfg->ATMSustainableCellRate = atoi(ATMSustainableCellRate);

      ret = cmsObj_set(dslLinkCfg, &iidStack);
      cmsObj_free((void **) &dslLinkCfg);
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

int SetDSLLinkType(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   WanDslLinkCfgObject *dslLinkCfg = NULL;   
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret=CMSRET_INTERNAL_ERROR;

   struct Param *pParams;

   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);
      
   pParams = findActionParamByRelatedVar(ac,VAR_LinkType);
   if (pParams != NULL)
   {
      if(strlen(pParams->value)==0)
      {
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }

      if( cmsObj_get(MDMOID_WAN_DSL_LINK_CFG, &iidStack, 0, (void **)&dslLinkCfg) == CMSRET_SUCCESS)
      {
         cmsMem_free(dslLinkCfg->linkType);
         dslLinkCfg->linkType= cmsMem_strdup(pParams->value);
         ret = cmsObj_set(dslLinkCfg, &iidStack);
         cmsObj_free((void **) &dslLinkCfg);
      }

      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("set of MDMOID_WAN_DSL_LINK_CFG failed, ret=%d", ret);
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
      
/*
   Currently the system MDM datastruct don't support the 3 paramters:
   ATMTransmittedBlocks/ATMReceivedBlocks/ATMCRCErrors
*/
int GetStatisticsWANDSL(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
#ifdef LGD_TODO  
   WanDslLinkCfgObject *dslLinkCfg = NULL;   
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   int errorinfo = 0;

   static char AAL5CRCErrors[32];

   int instanceOfWANDevice;
   int instanceOfWANConnectionDevice;

   instanceOfWANDevice = psvc->device->parent->instance;
   instanceOfWANConnectionDevice = psvc->device->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfWANConnectionDevice);

   if (cmsObj_get(MDMOID_WAN_DSL_LINK_CFG, &iidStack, 0, (void **)&dslLinkCfg) == CMSRET_SUCCESS)
   {
      sprintf(AAL5CRCErrors, "%d", dslLinkCfg->AAL5CRCErrors);
      cmsObj_free((void **) &dslLinkCfg);         
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   errorinfo |= OutputCharValueToAC(ac, VAR_AAL5CRCErrors, AAL5CRCErrors);
   
   if(errorinfo)
   {
      soap_error( uclient, errorinfo );
      return FALSE;
   }
#endif
   return TRUE;

}


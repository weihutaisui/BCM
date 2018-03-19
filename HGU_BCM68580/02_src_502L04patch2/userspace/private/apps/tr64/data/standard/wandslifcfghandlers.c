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
#include "wandslifcfgparams.h"
#include "tr64defs.h"

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"

int WANDSLInterfaceConfig_GetVar(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanDslIntfCfgObject *dslIntfCfg = NULL;
   CmsRet ret;
   
   int instanceOfWANDevice;
   instanceOfWANDevice = psvc->device->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);

   ret = cmsObj_get(MDMOID_WAN_DSL_INTF_CFG, &iidStack, 0, (void **) &dslIntfCfg);
   if(ret != CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   OutputNumValueToAC(ac, VAR_Enable, dslIntfCfg->enable);  
   OutputCharValueToAC(ac, VAR_Status, dslIntfCfg->status);      
   // [JIRA SWBCACPE-10306]: TR-098 requires
   // upstreamCurrRate, downstreamCurrRate, upstreamMaxRate, downstreamMaxRate
   // in Kbps instead of Bps
   OutputNumValueToAC(ac, VAR_UpstreamCurrRate, dslIntfCfg->upstreamCurrRate * 1000);      
   OutputNumValueToAC(ac, VAR_DownstreamCurrRate, dslIntfCfg->downstreamCurrRate * 1000);   
   OutputNumValueToAC(ac, VAR_UpstreamMaxRate, dslIntfCfg->upstreamMaxRate * 1000);      
   OutputNumValueToAC(ac, VAR_DownstreamMaxRate, dslIntfCfg->downstreamMaxRate * 1000);      
   OutputNumValueToAC(ac, VAR_UpstreamNoiseMargin, dslIntfCfg->upstreamNoiseMargin);      
   OutputNumValueToAC(ac, VAR_DownstreamNoiseMargin, dslIntfCfg->downstreamNoiseMargin);      
   OutputNumValueToAC(ac, VAR_UpstreamAttenuation, dslIntfCfg->upstreamAttenuation);  
   OutputNumValueToAC(ac, VAR_DownstreamAttenuation, dslIntfCfg->downstreamAttenuation);   
   OutputNumValueToAC(ac, VAR_UpstreamPower, dslIntfCfg->upstreamPower);      
   OutputNumValueToAC(ac, VAR_DownstreamPower, dslIntfCfg->downstreamPower);      
   OutputCharValueToAC(ac, VAR_ATURVendor, dslIntfCfg->ATURVendor);  
   OutputNumValueToAC(ac, VAR_ATURCountry, dslIntfCfg->ATURCountry);    
   OutputCharValueToAC(ac, VAR_ATUCVendor, dslIntfCfg->ATUCVendor);  
   OutputNumValueToAC(ac, VAR_ATUCCountry, dslIntfCfg->ATURCountry);    
   OutputNumValueToAC(ac, VAR_TotalStart, dslIntfCfg->totalStart);  

   cmsObj_free((void **) &dslIntfCfg);
   return TRUE;
}

#if 0 //remove this action firstly, set atm to disabled will make cpe hang on 6338 chip 
int SetDSLInterfaceEnable(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanDslIntfCfgObject *dslIntfCfg = NULL;
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

      if ((ret = cmsObj_get(MDMOID_WAN_DSL_INTF_CFG, &iidStack, 0, (void *) &dslIntfCfg)) != CMSRET_SUCCESS)
      {
         cmsLog_error("get of MDMOID_WAN_DSL_INTF_CFG failed, ret=%d", ret);
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }

      if( strcmp(pParams->value,"0") == 0 )
      {
         dslIntfCfg->enable= 0;
      }
      else
      {
         dslIntfCfg->enable= 1;
      }

      ret = cmsObj_set(dslIntfCfg, &iidStack);
      cmsObj_free((void **) &dslIntfCfg);;

      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("set of MDMOID_WAN_DSL_INTF_CFG failed, ret=%d", ret);
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
#endif

int GetStatisticsTotal(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   int errorinfo = 0;

   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanDslIntfCfgObject *dslIntfObj = NULL;
   WanDslIntfStatsTotalObject *dslTotalStatsObj = NULL;

   int instanceOfWANDevice;

   instanceOfWANDevice = psvc->device->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);

   if (cmsObj_get(MDMOID_WAN_DSL_INTF_CFG, &iidStack, 0, (void **) &dslIntfObj) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strcmp(dslIntfObj->status,MDMVS_UP) == 0)
      {
         if (cmsObj_get(MDMOID_WAN_DSL_INTF_STATS_TOTAL, &iidStack, 0, (void **) &dslTotalStatsObj) == CMSRET_SUCCESS)
         {
            errorinfo |= OutputNumValueToAC(ac, VAR_ReceiveBlocks, dslTotalStatsObj->receiveBlocks);
            errorinfo |= OutputNumValueToAC(ac, VAR_TransmitBlocks, dslTotalStatsObj->transmitBlocks);
            errorinfo |= OutputNumValueToAC(ac, VAR_ErroredSecs, dslTotalStatsObj->erroredSecs);
            errorinfo |= OutputNumValueToAC(ac, VAR_SeverelyErroredSecs, dslTotalStatsObj->severelyErroredSecs);
            errorinfo |= OutputNumValueToAC(ac, VAR_HECErrors, dslTotalStatsObj->HECErrors);
            errorinfo |= OutputNumValueToAC(ac, VAR_ATUCHECErrors, dslTotalStatsObj->ATUCHECErrors);
            errorinfo |= OutputNumValueToAC(ac, VAR_CRCErrors, dslTotalStatsObj->CRCErrors);
            errorinfo |= OutputNumValueToAC(ac, VAR_ATUCCRCErrors, dslTotalStatsObj->ATUCCRCErrors);
	#ifdef DMP_X_BROADCOM_COM_ADSLWAN_1
            errorinfo |= OutputNumValueToAC(ac, VAR_FECErrors, dslTotalStatsObj->X_BROADCOM_COM_FECErrors_2);                
	#endif
            errorinfo |= OutputNumValueToAC(ac, VAR_ATUCFECErrors, 0);     
            errorinfo |= OutputNumValueToAC(ac, VAR_CellDelin, 0);     
            errorinfo |= OutputNumValueToAC(ac, VAR_LinkRetrain, 0);
            errorinfo |= OutputNumValueToAC(ac, VAR_InitErrors, 0);
            errorinfo |= OutputNumValueToAC(ac, VAR_InitTimeouts, 0);
            errorinfo |= OutputNumValueToAC(ac, VAR_LossOfFraming, 0);

            cmsObj_free((void **) &dslTotalStatsObj);
         }
         else
         {
            soap_error( uclient, SOAP_ACTIONFAILED );
            cmsObj_free((void **) &dslIntfObj);
            return FALSE;
         } 
      } 
      else 
      {
         errorinfo |= OutputNumValueToAC(ac, VAR_ReceiveBlocks, 0);
         errorinfo |= OutputNumValueToAC(ac, VAR_TransmitBlocks, 0);
         errorinfo |= OutputNumValueToAC(ac, VAR_ErroredSecs, 0);
         errorinfo |= OutputNumValueToAC(ac, VAR_SeverelyErroredSecs, 0);
         errorinfo |= OutputNumValueToAC(ac, VAR_HECErrors, 0);
         errorinfo |= OutputNumValueToAC(ac, VAR_ATUCHECErrors, 0);
         errorinfo |= OutputNumValueToAC(ac, VAR_CRCErrors, 0);
         errorinfo |= OutputNumValueToAC(ac, VAR_ATUCCRCErrors, 0);

         errorinfo |= OutputNumValueToAC(ac, VAR_ATUCFECErrors, 0);     
         errorinfo |= OutputNumValueToAC(ac, VAR_FECErrors, 0);           
         errorinfo |= OutputNumValueToAC(ac, VAR_CellDelin, 0);     
         errorinfo |= OutputNumValueToAC(ac, VAR_LinkRetrain, 0);
         errorinfo |= OutputNumValueToAC(ac, VAR_InitErrors, 0);
         errorinfo |= OutputNumValueToAC(ac, VAR_InitTimeouts, 0);
         errorinfo |= OutputNumValueToAC(ac, VAR_LossOfFraming, 0);
      } 
      cmsObj_free((void **) &dslIntfObj);
   }

   if(errorinfo)
   {
      soap_error( uclient, errorinfo );
      return FALSE;
   } 
   return TRUE;
}

int GetStatisticsShowTime(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{

   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanDslIntfCfgObject *dslIntfObj = NULL;
   WanDslIntfStatsTotalObject *dslTotalStatsObj = NULL;
   int errorinfo = 0;

   int instanceOfWANDevice;

   instanceOfWANDevice = psvc->device->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfWANDevice);

   if (cmsObj_get(MDMOID_WAN_DSL_INTF_CFG, &iidStack, 0, (void **) &dslIntfObj) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strcmp(dslIntfObj->status,MDMVS_UP) == 0)
      {
         if (cmsObj_get(MDMOID_WAN_DSL_INTF_STATS_TOTAL, &iidStack, 0, (void **) &dslTotalStatsObj) == CMSRET_SUCCESS)
         {
            errorinfo |= OutputNumValueToAC(ac, VAR_ReceiveBlocks, dslTotalStatsObj->receiveBlocks);
            errorinfo |= OutputNumValueToAC(ac, VAR_TransmitBlocks, dslTotalStatsObj->transmitBlocks);
            errorinfo |= OutputNumValueToAC(ac, VAR_ErroredSecs, dslTotalStatsObj->erroredSecs);
            errorinfo |= OutputNumValueToAC(ac, VAR_SeverelyErroredSecs, dslTotalStatsObj->severelyErroredSecs);
            errorinfo |= OutputNumValueToAC(ac, VAR_HECErrors, dslTotalStatsObj->HECErrors);
            errorinfo |= OutputNumValueToAC(ac, VAR_ATUCHECErrors, dslTotalStatsObj->ATUCHECErrors);
            errorinfo |= OutputNumValueToAC(ac, VAR_CRCErrors, dslTotalStatsObj->CRCErrors);
            errorinfo |= OutputNumValueToAC(ac, VAR_ATUCCRCErrors, dslTotalStatsObj->ATUCCRCErrors);

	#ifdef DMP_X_BROADCOM_COM_ADSLWAN_1
            errorinfo |= OutputNumValueToAC(ac, VAR_FECErrors, dslTotalStatsObj->X_BROADCOM_COM_FECErrors_2);                
	#endif
            errorinfo |= OutputNumValueToAC(ac, VAR_ATUCFECErrors, 0);     
            errorinfo |= OutputNumValueToAC(ac, VAR_CellDelin, 0);     
            errorinfo |= OutputNumValueToAC(ac, VAR_LinkRetrain, 0);
            errorinfo |= OutputNumValueToAC(ac, VAR_InitErrors, 0);
            errorinfo |= OutputNumValueToAC(ac, VAR_InitTimeouts, 0);
            errorinfo |= OutputNumValueToAC(ac, VAR_LossOfFraming, 0);
            cmsObj_free((void **) &dslTotalStatsObj);
         } 
         else
         {
            soap_error( uclient, SOAP_ACTIONFAILED );
            cmsObj_free((void **) &dslIntfObj);
            return FALSE;
         } 
      } 
      else 
      {
         errorinfo |= OutputNumValueToAC(ac, VAR_ReceiveBlocks, 0);
         errorinfo |= OutputNumValueToAC(ac, VAR_TransmitBlocks, 0);
         errorinfo |= OutputNumValueToAC(ac, VAR_ErroredSecs, 0);
         errorinfo |= OutputNumValueToAC(ac, VAR_SeverelyErroredSecs, 0);
         errorinfo |= OutputNumValueToAC(ac, VAR_HECErrors, 0);
         errorinfo |= OutputNumValueToAC(ac, VAR_ATUCHECErrors, 0);
         errorinfo |= OutputNumValueToAC(ac, VAR_CRCErrors, 0);
         errorinfo |= OutputNumValueToAC(ac, VAR_ATUCCRCErrors, 0);
 
         errorinfo |= OutputNumValueToAC(ac, VAR_ATUCFECErrors, 0);     
         errorinfo |= OutputNumValueToAC(ac, VAR_FECErrors, 0);           
         errorinfo |= OutputNumValueToAC(ac, VAR_CellDelin, 0);     
         errorinfo |= OutputNumValueToAC(ac, VAR_LinkRetrain, 0);
         errorinfo |= OutputNumValueToAC(ac, VAR_InitErrors, 0);
         errorinfo |= OutputNumValueToAC(ac, VAR_InitTimeouts, 0);
         errorinfo |= OutputNumValueToAC(ac, VAR_LossOfFraming, 0);
 
      } 
      cmsObj_free((void **) &dslIntfObj);
   }

   if(errorinfo)
   {
      soap_error( uclient, errorinfo );
      return FALSE;
   } 
   return TRUE;

}


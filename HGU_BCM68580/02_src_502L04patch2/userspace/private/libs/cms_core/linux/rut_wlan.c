/***********************************************************************
 *
 *  Copyright (c) 2006-2008  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2012:proprietary:standard

 This program is the proprietary software of Broadcom and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").  Except as set forth in
 an Authorized License, Broadcom grants no license (express or implied), right
 to use, or waiver of any kind with respect to the Software, and Broadcom
 expressly reserves all rights in and to the Software and all intellectual
 property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,

 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use
    all reasonable efforts to protect the confidentiality thereof, and to
    use this information only in connection with your use of Broadcom
    integrated circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
    PERFORMANCE OF THE SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
    LIMITED REMEDY.
:>
 *
 ************************************************************************/

#include "cms_core.h"
#include "cms_util.h"
#include "rcl.h"
#include "rut_util.h"
#include "rut_pmap.h"
#include "rut_lan.h"
#include "rut_wlan.h"
#ifdef WIRELESS
#include <wlcsm_lib_api.h>
#endif

void rutWlan_requestRestart(char *cmd, int index) {

#ifdef SUPPORT_UNIFIED_WLMNGR
	if(mdmLibCtx.eid == EID_TR69C || mdmLibCtx.eid == EID_CWMPD)
		wlcsm_mngr_restart(index,WLCSM_MNGR_RESTART_TR69C,WLCSM_MNGR_RESTART_NOSAVEDM,0);
	else if(mdmLibCtx.eid != EID_WLMNGR)
		wlcsm_mngr_restart(index,WLCSM_MNGR_RESTART_MDM,WLCSM_MNGR_RESTART_NOSAVEDM,0);
#else

#if defined(SUPPORT_DM_DETECT)
   if(cmsMdm_isDataModelDevice2())
   {
#endif
#if defined(SUPPORT_TR181_WLMNGR)
	wlcsm_mngr_restart(index,WLCSM_MNGR_RESTART_MDM,WLCSM_MNGR_RESTART_NOSAVEDM,0);
#endif
#if defined(SUPPORT_DM_DETECT)
   } else 
#endif
#if !defined(SUPPORT_TR181_WLMNGR) || defined(SUPPORT_DM_DETECT)
   {
	char buf[sizeof(CmsMsgHeader) + 100]={0};
	CmsMsgHeader *msg=(CmsMsgHeader *) buf;	
	sprintf((char *)(msg+1), "%s:%d", cmd,index+1);
	msg->type = CMS_MSG_WLAN_CHANGED;
	msg->src = mdmLibCtx.eid;
	msg->dst = EID_WLMNGR;
	msg->flags_bounceIfNotRunning = 1;
	msg->flags_event = 1;
	msg->flags_request = 0;
	msg->dataLength=strlen((char *)(msg+1));	
	if (cmsMsg_send(mdmLibCtx.msgHandle, msg) != CMSRET_SUCCESS)
	{
		cmsLog_error("could not send CMS_MSG_WLAN_CHANGED msg to wlan");
	}
   }
#endif
#endif
}

void rutWlan_modifyVirtIntfFilters(const InstanceIdStack *iidStack __attribute__((unused)),
                                   UBOOL8 enable __attribute__((unused)))
{
#ifdef DMP_BRIDGING_1

   WlVirtIntfCfgObject *virtIntfObj=NULL;
   InstanceIdStack virtIidStack=EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   cmsLog_debug("master wlEnbl=%d", enable);

   while ((ret = cmsObj_getNextInSubTreeFlags(MDMOID_WL_VIRT_INTF_CFG, iidStack, &virtIidStack, OGF_NO_VALUE_UPDATE, (void **) &virtIntfObj)) == CMSRET_SUCCESS)
   {
      if (enable && virtIntfObj->wlEnblSsid)
      {
         cmsLog_debug("enabling virtIntf %s", virtIntfObj->wlIfcname);
         rutWlan_enableVirtIntfFilter(virtIntfObj->wlIfcname, atoi(&(virtIntfObj->wlBrName[2])));
      }
      else if (!enable)
      {
         cmsLog_debug("disabling virtIntf %s", virtIntfObj->wlIfcname);
         rutWlan_disableVirtIntfFilter(virtIntfObj->wlIfcname);

         if (strcmp(virtIntfObj->wlBrName, "br0"))
         {
            /*
             * wlBrName is an internal book-keeping parameter used by wlmngr.
             * set it to br0 to be consistent with the filterBridgeRefence setting
             * of -1.  This is also done at the bottom of rcl_wlVirtIntfCfgObject.
             */
            CMSMEM_REPLACE_STRING_FLAGS(virtIntfObj->wlBrName, "br0", mdmLibCtx.allocFlags);
            cmsObj_set(virtIntfObj, &virtIidStack);
         }
      }

      cmsObj_free((void **) &virtIntfObj);
   }

#endif
}

#ifdef DMP_BRIDGING_1
void rutWlan_enableVirtIntfFilter(const char *wlIfcname, UINT32 bridgeKey)
{
   L2BridgingFilterObject *filterObj=NULL;
   InstanceIdStack filterIidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   cmsLog_debug("wlIfcname=%s bridgeKey=%d", wlIfcname, bridgeKey);

   if ((ret = rutPMap_ifNameToFilterObject(wlIfcname, &filterIidStack, &filterObj)) != CMSRET_SUCCESS)
   {
      cmsLog_debug("could not get filterObj for %s", wlIfcname);
   }
   else
   {
      /*
       * The bridgeKey is just the number in the brx linux interface name.
       * So br0 has bridgeKey of 0,
       * br1 has bridgeKey of 1, etc.
       */
      cmsLog_debug("got filter at %p iidStack=%s", filterObj, cmsMdm_dumpIidStack(&filterIidStack));

      if (filterObj->filterBridgeReference != (int) bridgeKey)
      {
         filterObj->filterBridgeReference = bridgeKey;

         if ((ret = cmsObj_set(filterObj, &filterIidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("could not set FilterBridgeReference to %d, ret=%d", filterObj->filterBridgeReference, ret);
         }
            
         cmsObj_free((void **) &filterObj);
      }
   }

   return;
}


void rutWlan_disableVirtIntfFilter(const char *wlIfcname)
{
   L2BridgingFilterObject *filterObj=NULL;
   InstanceIdStack filterIidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   cmsLog_debug("wlIfcname=%s", wlIfcname);

   /*
    * by setting the FilterBridgeRef to -1, the rut_pmap will disassociate
    * this interface from its bridge.
    */
   if ((ret = rutPMap_ifNameToFilterObject(wlIfcname, &filterIidStack, &filterObj)) != CMSRET_SUCCESS)
   {
      cmsLog_debug("could not get filterObj for %s", wlIfcname);
   }
   else
   {
      cmsLog_debug("filterBridgeRef=%d", filterObj->filterBridgeReference);

      if (filterObj->filterBridgeReference != -1)
      {
         filterObj->filterBridgeReference = -1;
         if ((ret = cmsObj_set(filterObj, &filterIidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("could not set FilterBridgeReference to -1, ret=%d", ret);
         }
      }
            
      cmsObj_free((void **) &filterObj);
   }

   return;
}

#endif /* DMP_BRIDGING_1 */

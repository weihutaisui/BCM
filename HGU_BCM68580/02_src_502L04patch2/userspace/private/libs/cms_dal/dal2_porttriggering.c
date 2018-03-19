/***********************************************************************
 *
 *  Copyright (c) 2006-2010  Broadcom Corporation
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
#ifdef DMP_DEVICE2_BASELINE_1

#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"
#include "cms_qdm.h"

/* local functions */

static CmsRet fillPrtTrgCfg_dev2(const char *appName,
   const char *tProto, const char *oProto, const UINT16 tPS, const UINT16 tPE,
   const UINT16 oPS, const UINT16 oPE, void * obj)
{
   Dev2NatPortTriggeringObject *ipConnPT = NULL;

   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("%s/%s/%hu/%hu/%s/%hu/%hu", appName, tProto, tPS, tPE, oProto, oPS, oPE);

   ipConnPT = (Dev2NatPortTriggeringObject *) obj;
   ipConnPT->enable = TRUE;

   CMSMEM_REPLACE_STRING(ipConnPT->name, appName);
   if (strcmp(tProto, "1") == 0)
   {
      CMSMEM_REPLACE_STRING(ipConnPT->triggerProtocol, "TCP");
   }
   else if (strcmp(tProto, "2") == 0)
   {
      CMSMEM_REPLACE_STRING(ipConnPT->triggerProtocol, "UDP");
   }
   else
   {
      CMSMEM_REPLACE_STRING(ipConnPT->triggerProtocol, "TCP or UDP");
   }
   ipConnPT->triggerPortStart= tPS;
   ipConnPT->triggerPortEnd = tPE;
   if (strcmp(oProto, "1") == 0)
   {
      CMSMEM_REPLACE_STRING(ipConnPT->openProtocol, "TCP");
   }
   else if (strcmp(oProto, "2") == 0)
   {
      CMSMEM_REPLACE_STRING(ipConnPT->openProtocol, "UDP");
   }
   else
   {
      CMSMEM_REPLACE_STRING(ipConnPT->openProtocol, "TCP or UDP");
   }
   ipConnPT->openPortStart= oPS;
   ipConnPT->openPortEnd = oPE;

   if ((ipConnPT->name == NULL) ||(ipConnPT->triggerProtocol == NULL) ||(ipConnPT->openProtocol == NULL))
   {
      cmsLog_error("malloc failed.");
      cmsObj_free((void **) &ipConnPT);
      return CMSRET_RESOURCE_EXCEEDED;
   }

   return ret;
}

CmsRet dalPortTriggering_addEntry_dev2(const char *dstWanIf, const char *appName,
   const char *tProto, const char *oProto, const UINT16 tPS,
   const UINT16 tPE, const UINT16 oPS, const UINT16 oPE)
{
   CmsRet ret = CMSRET_SUCCESS;
   char *ipIntfFullPath=NULL;
   Dev2NatPortTriggeringObject *portrigObj = NULL;
   Dev2NatIntfSettingObject *natIntfObj=NULL;   
   InstanceIdStack NAT_iidStack=EMPTY_INSTANCE_ID_STACK;

   UBOOL8 found=FALSE;

   ret = qdmIntf_intfnameToFullPathLocked_dev2(dstWanIf, FALSE, &ipIntfFullPath);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("bad ifName %s, ret=%d", dstWanIf, ret);
      return ret;      
   }
   else
   {
      while(!found &&
            cmsObj_getNextFlags(MDMOID_DEV2_NAT_INTF_SETTING,
                                &NAT_iidStack, OGF_NO_VALUE_UPDATE,
                                (void **) &natIntfObj) == CMSRET_SUCCESS)
      {
          if (!cmsUtl_strcmp(natIntfObj->interface, ipIntfFullPath))
          {
              found = TRUE;
          }
          cmsObj_free((void **)&natIntfObj);
      }
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);

   if(found)
   {
      if ((ret = cmsObj_addInstance(MDMOID_DEV2_NAT_PORT_TRIGGERING, &NAT_iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("could not create new port triggering entry, ret=%d", ret);
         return ret;
      }

      if ((ret = cmsObj_get(MDMOID_DEV2_NAT_PORT_TRIGGERING,&NAT_iidStack, 0, (void **) &portrigObj)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to add Dev2NatPortTriggeringObject on %s, ret = %d", dstWanIf,ret);
         cmsObj_deleteInstance(MDMOID_DEV2_NAT_PORT_TRIGGERING, &NAT_iidStack);
         return ret;
      }
 
      if ((ret = fillPrtTrgCfg_dev2(appName, tProto, oProto, tPS, tPE, oPS, oPE, portrigObj)) != CMSRET_SUCCESS)
      {
         cmsObj_free((void **) &portrigObj);
         cmsObj_deleteInstance(MDMOID_DEV2_NAT_PORT_TRIGGERING, &NAT_iidStack);
         return ret;
      }

      /* set and activate Dev2NatPortTriggeringObject */
      if (( ret = cmsObj_set(portrigObj, &NAT_iidStack)) != CMSRET_SUCCESS)
         cmsObj_deleteInstance(MDMOID_DEV2_NAT_PORT_TRIGGERING, &NAT_iidStack);

      cmsObj_free((void **) &portrigObj);
   }else
   {
      ret = CMSRET_INVALID_ARGUMENTS;
   }

   return ret;

}


CmsRet dalPortTriggering_deleteEntry_dev2(const char *dstWanIf __attribute__((unused)),
      const char *tProto, const UINT16 tPS, const UINT16 tPE, const UINT16 tOS, const UINT16 tOE)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   Dev2NatPortTriggeringObject *portrigObj = NULL;

   while ((ret = cmsObj_getNext(MDMOID_DEV2_NAT_PORT_TRIGGERING,&iidStack,(void **)&portrigObj)) == CMSRET_SUCCESS)
   {
      if(!strcmp(tProto, portrigObj->triggerProtocol) && 
         tPS == portrigObj->triggerPortStart && tPE == portrigObj->triggerPortEnd &&
         tOS == portrigObj->openPortStart && tOE == portrigObj->openPortEnd) 
      {
         ret = cmsObj_deleteInstance(MDMOID_DEV2_NAT_PORT_TRIGGERING, &iidStack);
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("Failed to delete Dev2NatPortTriggeringObject, ret = %d", ret);
         }
         cmsObj_free((void **) &portrigObj);
         break;
      }
      cmsObj_free((void **) &portrigObj);
   }

   return ret;

}


CmsRet dalNat_deleteIntfSetting_dev2(const char * ipIntffullPath)
{                                        
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStackPrev = EMPTY_INSTANCE_ID_STACK;
   Dev2NatIntfSettingObject *natIntfObj = NULL;   
   CmsRet ret = CMSRET_INVALID_ARGUMENTS;
   
   if (ipIntffullPath == NULL)
   {
      cmsLog_error("NULL string.");
      return ret;
   }
   cmsLog_debug("Enter ipIntffullPath %s.", ipIntffullPath);
   
   while((ret = cmsObj_getNextFlags(MDMOID_DEV2_NAT_INTF_SETTING,
                                    &iidStack, 
                                    OGF_NO_VALUE_UPDATE,
                                    (void **)&natIntfObj)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcasecmp(natIntfObj->interface, ipIntffullPath))
      {
         cmsObj_deleteInstance(MDMOID_DEV2_NAT_INTF_SETTING, &iidStack);
         iidStack = iidStackPrev;
      }
      else
      {
         iidStackPrev = iidStack;
      }

      cmsObj_free((void **)&natIntfObj);
   }    
   
   cmsLog_debug("Exit, ret=%d", ret);

   return ret;
}


#endif

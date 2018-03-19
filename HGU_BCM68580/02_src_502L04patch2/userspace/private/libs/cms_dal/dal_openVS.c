/***********************************************************************
 *
 *  Copyright (c) 2006-2010  Broadcom 
 *  All Rights Reserved
 *
<:label-BRCM:2012:proprietary:standard

 This program is the proprietary software of Broadcom  and/or its
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


#ifdef DMP_X_BROADCOM_COM_OPENVSWITCH_1

#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"



CmsRet dalOpenVSCfg(const char *enable, const char *ofControllerAddr,UINT32 ofControllerPort, const char *openVSports )
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   OpenvswitchCfgObject *openVSCfg = NULL;   

   cmsLog_debug("configure Openvswitch with %s/%s/%d/%s", enable,ofControllerAddr,ofControllerPort,openVSports);

   if ((ret = cmsObj_get(MDMOID_OPENVSWITCH_CFG, &iidStack, 0, (void **) &openVSCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get OpenvswitchCfgObject, ret=%d", ret);
      return ret;
   }

   if (strcmp(enable, "1") == 0)
   {
      openVSCfg->enable = TRUE;
      CMSMEM_REPLACE_STRING(openVSCfg->OFControllerIPAddress, ofControllerAddr);
      openVSCfg->OFControllerPortNumber = ofControllerPort;
      CMSMEM_REPLACE_STRING(openVSCfg->ifNameList, openVSports);
   }
   else
   {
      openVSCfg->enable = FALSE;
   }
   
   ret = cmsObj_set(openVSCfg, &iidStack);
   cmsObj_free((void **) &openVSCfg);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set OpenvswitchCfgObject, ret = %d", ret);
   }   

   return ret;
}

CmsRet dalGetOpenVS(char *info)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   OpenvswitchCfgObject *openVSCfg = NULL; 

   if(info == NULL)
   {
      return CMSRET_INVALID_PARAM_TYPE;
   }

   if ((ret = cmsObj_get(MDMOID_OPENVSWITCH_CFG, &iidStack, 0, (void **) &openVSCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get OpenvswitchCfgObject, ret=%d", ret);
      return ret;
   }  

   if (openVSCfg->ifNameList != NULL) 
      sprintf(info, "%d|%s|%d|%s", openVSCfg->enable, openVSCfg->OFControllerIPAddress,openVSCfg->OFControllerPortNumber,openVSCfg->ifNameList);
   else
      sprintf(info, "%d|%s|%d|,", openVSCfg->enable, openVSCfg->OFControllerIPAddress,openVSCfg->OFControllerPortNumber);

   cmsObj_free((void **) &openVSCfg);

   return ret;
}

UBOOL8 dalIsOpenVSEnabled()
{
   UBOOL8 openVSEnable = FALSE;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   _OpenvswitchCfgObject *openVSCfg=NULL;
   CmsRet ret;
  
   cmsLog_debug("Enter");
   if ((ret = cmsObj_get(MDMOID_OPENVSWITCH_CFG, &iidStack, 0, (void **) &openVSCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get OPENVSWITCH_CFG, ret=%d", ret);
   }
   else
   {
      openVSEnable = openVSCfg->enable;
      cmsObj_free((void **) &openVSCfg);
   }
   cmsLog_debug("openvswitch enable status = %d", openVSEnable);
  
   return openVSEnable;
}
#endif

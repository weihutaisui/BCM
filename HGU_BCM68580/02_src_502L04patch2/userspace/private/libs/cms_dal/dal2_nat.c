/*
* <:copyright-BRCM:2013:proprietary:standard
* 
*    Copyright (c) 2013 Broadcom 
*    All Rights Reserved
* 
*  This program is the proprietary software of Broadcom and/or its
*  licensors, and may only be used, duplicated, modified or distributed pursuant
*  to the terms and conditions of a separate, written license agreement executed
*  between you and Broadcom (an "Authorized License").  Except as set forth in
*  an Authorized License, Broadcom grants no license (express or implied), right
*  to use, or waiver of any kind with respect to the Software, and Broadcom
*  expressly reserves all rights in and to the Software and all intellectual
*  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
*  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
*  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
* 
*  Except as expressly set forth in the Authorized License,
* 
*  1. This program, including its structure, sequence and organization,
*     constitutes the valuable trade secrets of Broadcom, and you shall use
*     all reasonable efforts to protect the confidentiality thereof, and to
*     use this information only in connection with your use of Broadcom
*     integrated circuit products.
* 
*  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
*     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
*     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
*     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
*     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
*     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
*     PERFORMANCE OF THE SOFTWARE.
* 
*  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
*     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
*     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
*     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
*     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
*     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
*     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
*     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
*     LIMITED REMEDY.
:>
*/

#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"
#include "cms_qdm.h"
#include "dal.h"
#include "dal2_wan.h"

#ifdef DMP_DEVICE2_BASELINE_1    /* this file touches TR181 objects */

CmsRet dalNat_set_dev2(const char *ipIntfFullPath, UBOOL8 isNATEnabled, UBOOL8 isFullcone)
{
   UBOOL8 found=FALSE;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   Dev2NatIntfSettingObject *natIntfObj=NULL;
   CmsRet ret;

   cmsLog_debug("Enter.");


   while(!found &&
         (ret = cmsObj_getNextFlags(MDMOID_DEV2_NAT_INTF_SETTING,
                             &iidStack,
                             OGF_NO_VALUE_UPDATE,
                             (void **) &natIntfObj)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(natIntfObj->interface, ipIntfFullPath))
      {
         found = TRUE;
         break;
      }

      cmsObj_free((void **)&natIntfObj);
   }


   if(!found)
   {
      if ((ret = cmsObj_addInstance(MDMOID_DEV2_NAT_INTF_SETTING, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to add MDMOID_DEV2_NAT_INTF_SETTING Instance, ret = %d", ret);
         return ret;
      } 

      if ((ret = cmsObj_get(MDMOID_DEV2_NAT_INTF_SETTING, &iidStack, 0, (void **) &natIntfObj)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to get MDMOID_DEV2_NAT_INTF_SETTING, ret = %d", ret);
         cmsObj_deleteInstance(MDMOID_DEV2_NAT_INTF_SETTING, &iidStack);
         return ret;
      }
   }

   natIntfObj->enable = isNATEnabled ;
   CMSMEM_REPLACE_STRING(natIntfObj->interface, ipIntfFullPath);
   natIntfObj->X_BROADCOM_COM_FullconeNATEnabled=isFullcone;

   if ((ret = cmsObj_set(natIntfObj, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set natIntfObj. ret=%d", ret);
      cmsObj_deleteInstance(MDMOID_DEV2_NAT_INTF_SETTING, &iidStack);
   } 
   cmsObj_free((void **) &natIntfObj);


   cmsLog_debug("Exit, ret=%d", ret);

   return ret;
}



CmsRet dalNat_delete_dev2(const char *ipIntfFullPath)
{
   UBOOL8 found=FALSE;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK; 
   Dev2NatIntfSettingObject *natIntfObj=NULL;
   CmsRet ret;
   
   cmsLog_debug("Enter %s", ipIntfFullPath);

   while(!found &&
         (ret = cmsObj_getNextFlags(MDMOID_DEV2_NAT_INTF_SETTING,
                             &iidStack,
                             OGF_NO_VALUE_UPDATE,
                             (void **) &natIntfObj)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(natIntfObj->interface, ipIntfFullPath))
      {
         found = TRUE;
         if ((ret = cmsObj_deleteInstance(MDMOID_DEV2_NAT_INTF_SETTING, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Failed to delete  MDMOID_DEV2_NAT_INTF_SETTING, ret=%d", ret);
         }
      }

      cmsObj_free((void **)&natIntfObj);
   }

   if (!found)
   {
      cmsLog_error("Failed to find the NAT info for %s", ipIntfFullPath);
   }

   cmsLog_debug("Exit %d", ret);  
   
   return ret;
}


#ifdef NOT_USED
/* Not used but may be later on need this for setting NAT object enable flag */
CmsRet dalNat_setEnableFlag(const char *ipIntfFullPath, UBOOL8 natEnableFlag)
{
   UBOOL8 found=FALSE;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK; 
   Dev2NatIntfSettingObject *natIntfObj=NULL;
   CmsRet ret;
   
   cmsLog_debug("Enter %s", ipIntfFullPath);

   while(!found &&
         (ret = cmsObj_getNextFlags(MDMOID_DEV2_NAT_INTF_SETTING,
                             &iidStack,
                             OGF_NO_VALUE_UPDATE,
                             (void **) &natIntfObj)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(natIntfObj->interface, ipIntfFullPath))
      {

         natIntfObj->enable = natEnableFlag;
         if ((ret = cmsObj_set(natIntfObj, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Failed to set natIntfObj. ret=%d", ret);
         } 
         found = TRUE;
      }
      cmsObj_free((void **)&natIntfObj);
   }

   if (!found)
   {
      cmsLog_error("Failed to find the NAT info for %s", ipIntfFullPath);
   }

   cmsLog_debug("Exit %d", ret);  
   
   return ret;
}
#endif /* NOT_USED */



#endif /* DMP_DEVICE2_BASELINE_1 */


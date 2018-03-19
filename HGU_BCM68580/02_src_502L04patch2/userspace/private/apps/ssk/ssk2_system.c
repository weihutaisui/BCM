/* 
* <:copyright-BRCM:2014:proprietary:standard
* 
*    Copyright (c) 2014 Broadcom 
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
* :>
*/

#ifdef DMP_DEVICE2_BASELINE_1

#include "cms.h"
#include "cms_util.h"
#include "cms_core.h"
#include "ssk.h"


/*!\file ssk2_system.c
 *
 * This file contains functions which are called from the ssk.
 * The functions in this file assume a TR181 Device
 * based data model (Pure TR181 mode).
 */

void processVendorConfigUpdate_dev2(CmsMsgHeader *msg)
{
   Dev2DeviceVendorConfigFileObject *pVendorConfigObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;
   Dev2DeviceInfoObject *pDevInfo;
   UBOOL8 found = FALSE;
   char fileName[BUFLEN_64];
   int instance=0;
   int numberOfVendorFiles;
   vendorConfigUpdateMsgBody *vendorConfig = (vendorConfigUpdateMsgBody *) (msg + 1);
   CmsMsgHeader replyMsg = EMPTY_MSG_HEADER;

   replyMsg.type = msg->type;
   replyMsg.src = EID_SSK;
   replyMsg.dst = msg->src;
   replyMsg.flags_request = 0;
   replyMsg.flags_response = 1;

   ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT);
   if (ret != CMSRET_SUCCESS)
   {
      return;
   }
   if (cmsObj_get(MDMOID_DEV2_DEVICE_INFO, &iidStack, 0, (void **) &pDevInfo) != CMSRET_SUCCESS)
   {
      cmsLck_releaseLock();
      return;
   }
   numberOfVendorFiles = pDevInfo->vendorConfigFileNumberOfEntries;

   cmsLog_debug("numberOfVendorFiles %d",numberOfVendorFiles);

   cmsObj_free((void **) &pDevInfo);

   /* first look for instance of existing file */
   if (numberOfVendorFiles > 0)
   {
      if (vendorConfig->name[0] != '\0')
      {
         INIT_INSTANCE_ID_STACK(&iidStack);
         while (!found && (cmsObj_getNext(MDMOID_DEV2_DEVICE_VENDOR_CONFIG_FILE,&iidStack, (void **) &pVendorConfigObj)) == CMSRET_SUCCESS)
         {
            if ((cmsUtl_strcmp(pVendorConfigObj->name,vendorConfig->name) == 0))
            {
               cmsLog_debug("Same entry found, update vendor config entry");
               CMSMEM_REPLACE_STRING(pVendorConfigObj->version,vendorConfig->version);
               CMSMEM_REPLACE_STRING(pVendorConfigObj->date,vendorConfig->date);
               CMSMEM_REPLACE_STRING(pVendorConfigObj->description,vendorConfig->description);
               found = TRUE;
               cmsObj_set(pVendorConfigObj,&iidStack);
            }
            cmsObj_free((void **) &pVendorConfigObj);
         } /* while */
      } /* vendorConfig->name is not empty */
   }
   if (!found)
   {
      if (numberOfVendorFiles == MAX_NUMBER_OF_VENDOR_CONFIG_RECORD) 
      {
         INIT_INSTANCE_ID_STACK(&iidStack);
         while ((cmsObj_getNext(MDMOID_DEV2_DEVICE_VENDOR_CONFIG_FILE,&iidStack, (void **) &pVendorConfigObj)) == CMSRET_SUCCESS)
         {
            /* if the list is full, we delete the first instance, and add another */
            cmsLog_debug("numberOfVendorFiles has reached %d. Deleteing vendor config entry",numberOfVendorFiles);
            cmsObj_deleteInstance(MDMOID_DEV2_DEVICE_VENDOR_CONFIG_FILE, &iidStack);               
            cmsObj_free((void **) &pVendorConfigObj);
            break;
         }
      }
      /* add the instance */
      INIT_INSTANCE_ID_STACK(&iidStack);
      if ((ret = cmsObj_addInstance(MDMOID_DEV2_DEVICE_VENDOR_CONFIG_FILE,&iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("add instance of vendor config file returned %d", ret);
      }
      else
      {
         cmsObj_get(MDMOID_DEV2_DEVICE_VENDOR_CONFIG_FILE, &iidStack, 0, (void **) &pVendorConfigObj);
         if (vendorConfig->name[0] == '\0')
         {
            strcpy(fileName,"ConfigFile");
            instance = iidStack.instance[iidStack.currentDepth-1];
            sprintf(pVendorConfigObj->name,"%s%d",fileName,instance);
         }
         else
         {
            CMSMEM_REPLACE_STRING(pVendorConfigObj->name,vendorConfig->name);
         }
         CMSMEM_REPLACE_STRING(pVendorConfigObj->version,vendorConfig->version);
         CMSMEM_REPLACE_STRING(pVendorConfigObj->date,vendorConfig->date);
         CMSMEM_REPLACE_STRING(pVendorConfigObj->description,vendorConfig->description);

         if ((ret = cmsObj_set(pVendorConfigObj, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("set of vendor config object failed, ret=%d", ret);
         }
         else
         {
            cmsLog_debug("vendor config entry added");
         }
         cmsObj_free((void **) &pVendorConfigObj);
      } /* add instance, ok */
   } /* !found */
   cmsMgm_saveConfigToFlash();
   cmsLck_releaseLock();

   if ((ret = cmsMsg_send(msgHandle, &replyMsg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("msg send failed, ret=%d",ret);
   }
   
} /* processVendorConfigUpdate */

#endif  /* DMP_DEVICE2_BASELINE_1 */

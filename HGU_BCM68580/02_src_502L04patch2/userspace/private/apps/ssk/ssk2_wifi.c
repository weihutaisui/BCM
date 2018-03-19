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

#ifdef DMP_DEVICE2_WIFIACCESSPOINT_1
#include "cms.h"
#include "cms_util.h"
#include "cms_core.h"

#include "wlcsm_lib_api.h"
#include "ssk.h"
static void findAccessPointByIndex_locked(int radioIndex, int apIndex, InstanceIdStack *iidStack, void **obj);
static void findStationByMacAddr_locked(char *macAddr, InstanceIdStack *apIidStack, InstanceIdStack *adIidStack, void **obj);

void processAssociatedDeviceUpdated_dev2(CmsMsgHeader *msg)
{
   CmsRet ret;
   Dev2WifiAccessPointObject *apObj = NULL;
   Dev2WifiAssociatedDeviceObject *associatedDeviceObj = NULL;
   InstanceIdStack apIidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack adIidStack = EMPTY_INSTANCE_ID_STACK;
      
   WL_STALIST_SUMMARIES *sta_summaries =(WL_STALIST_SUMMARIES *)(msg+1);
   WL_STATION_LIST_ENTRY *pStation = sta_summaries->stalist_summary;
   int numStations = sta_summaries->num_of_stas;
   int i = 0;

   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get lock, ret=%d", ret);
      cmsLck_dumpInfo();
      return;
   }

   /* Add new or update existed associated devices */
   for (i = 0 ; i < numStations && pStation != NULL ; i++, pStation++)
   {
      INIT_INSTANCE_ID_STACK(&apIidStack);
      INIT_INSTANCE_ID_STACK(&adIidStack);
      apObj = NULL;
      associatedDeviceObj = NULL;

      findAccessPointByIndex_locked(pStation->radioIndex, pStation->ssidIndex, &apIidStack,(void **)&apObj);
      if (apObj == NULL)
      {
         cmsLog_error("Could not find AccessPointObj for associated device %s", pStation->macAddress);
         continue;
      }
      findStationByMacAddr_locked(pStation->macAddress, &apIidStack, &adIidStack, (void **)&associatedDeviceObj);
      if (associatedDeviceObj == NULL) // Add new instance
      {
         memcpy(&adIidStack, &apIidStack, sizeof(InstanceIdStack));
         if ((ret = cmsObj_addInstance(MDMOID_DEV2_WIFI_ASSOCIATED_DEVICE, &adIidStack) ) == CMSRET_SUCCESS)
         {
            cmsObj_get(MDMOID_DEV2_WIFI_ASSOCIATED_DEVICE, &adIidStack, 0, (void **)&associatedDeviceObj);
            CMSMEM_REPLACE_STRING(associatedDeviceObj->MACAddress, pStation->macAddress); 
            associatedDeviceObj->active = pStation->associated;
            associatedDeviceObj->authenticationState = pStation->authorized;
            cmsObj_set(associatedDeviceObj, &adIidStack); 
            cmsObj_free((void **) &associatedDeviceObj);
         }
         else
            cmsLog_error("could not add _Dev2WifiAssociatedDeviceObject instance, ret=%d", ret);
      }
      else  // Update existed instance
      {
         associatedDeviceObj->active = pStation->associated;
         associatedDeviceObj->authenticationState = pStation->authorized;
         cmsObj_set(associatedDeviceObj, &adIidStack); 
         cmsObj_free((void **) &associatedDeviceObj);
      }

      cmsObj_free((void **)&apObj);
   }

   /* Delete non-existed associated devices */
   INIT_INSTANCE_ID_STACK(&apIidStack);
   INIT_INSTANCE_ID_STACK(&adIidStack);
   while ((ret = cmsObj_getNext(MDMOID_DEV2_WIFI_ACCESS_POINT, &apIidStack, (void **)&apObj)) == CMSRET_SUCCESS)
   { 
      while ((ret = cmsObj_getNextInSubTree(MDMOID_DEV2_WIFI_ASSOCIATED_DEVICE, &apIidStack, &adIidStack, (void **) &associatedDeviceObj)) == CMSRET_SUCCESS)
      {
         int found = 0;
         pStation = sta_summaries->stalist_summary;
         for (i = 0 ; i < numStations && pStation != NULL ; i++, pStation++)
         {
            if (pStation->radioIndex == apObj->X_BROADCOM_COM_Adapter && pStation->ssidIndex == apObj->X_BROADCOM_COM_Index)
            {
               if (!cmsUtl_strcmp(pStation->macAddress, associatedDeviceObj->MACAddress))
               {
                  found = 1;
                  break;
               }
            }
         }

         cmsObj_free((void **) &associatedDeviceObj);
         if (!found)
         {
            cmsObj_deleteInstance(MDMOID_DEV2_WIFI_ASSOCIATED_DEVICE, &adIidStack);
            INIT_INSTANCE_ID_STACK(&adIidStack);
         }
      }
      cmsObj_free((void **) &apObj);
   }
   
   cmsLck_releaseLock();

   return;
}

static void findAccessPointByIndex_locked(int radioIndex, int ssidIndex, InstanceIdStack *iidStack, void **obj)
{
   CmsRet ret = CMSRET_SUCCESS;
   _Dev2WifiAccessPointObject *apObj = NULL;

   while ((ret = cmsObj_getNext(MDMOID_DEV2_WIFI_ACCESS_POINT, iidStack, (void **)&apObj )) == CMSRET_SUCCESS)
   {
      if (apObj->X_BROADCOM_COM_Index == ssidIndex && apObj->X_BROADCOM_COM_Adapter == radioIndex)
      {
         cmsLog_debug("find access point ID:%d/%d", apObj->X_BROADCOM_COM_Adapter,apObj->X_BROADCOM_COM_Index);
         break;
      }
      cmsObj_free((void **) &apObj);
   }

   if (apObj != NULL)
      *((_Dev2WifiAccessPointObject **)obj) = apObj; 
}

static void findStationByMacAddr_locked(char *macAddr, InstanceIdStack *apIidStack, InstanceIdStack *adIidStack, void **obj)
{
   CmsRet ret = CMSRET_SUCCESS;
   _Dev2WifiAssociatedDeviceObject *associatedDeviceObj = NULL;
  
   while ((ret = cmsObj_getNextInSubTree(MDMOID_DEV2_WIFI_ASSOCIATED_DEVICE, apIidStack, adIidStack, (void **)&associatedDeviceObj)) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strcmp(macAddr, associatedDeviceObj->MACAddress) == 0) // find related associated device object
      {
         cmsLog_debug("find associated device MAC:%s", associatedDeviceObj->MACAddress);
         break;
      }
      cmsObj_free((void **) &associatedDeviceObj);
   }

   if (associatedDeviceObj)
      *((_Dev2WifiAssociatedDeviceObject **)obj) = associatedDeviceObj;
}
#endif /* DMP_DEVICE2_WIFIACCESSPOINT_1 */

#endif /* DMP_DEVICE2_BASELINE_1 */

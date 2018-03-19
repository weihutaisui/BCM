/***********************************************************************
 *
 *  Copyright (c) 2013  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2013:proprietary:standard

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

#ifdef DMP_DEVICE2_WIFIACCESSPOINT_1

/*!\file rcl2_wifi_accesspoint.c
 * \brief This file contains TR181 and X_BROADCOM_COM Wifi Access Point (LAN)
 *        side functions.  General Wifi objects (wifi and radio) and
 *        End Point specific functions are in separate files.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include "odl.h"
#include "cms_core.h"
#include "cms_util.h"
#include "rcl.h"
#include "rut_util.h"
#include "rut2_util.h"
#include "rut2_wifi.h"
#include "cms_qdm.h"


CmsRet rcl_dev2WifiAccessPointObject( _Dev2WifiAccessPointObject *newObj,
                      const _Dev2WifiAccessPointObject *currObj,
                      const InstanceIdStack *iidStack __attribute__((unused)),
                      char **errorParam __attribute__((unused)),
                      CmsRet *errorCode __attribute__((unused)))
{
   /* UINT32 radioIndex; */
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("===> Enter");
   if (ADD_NEW(newObj, currObj))
   {
      rutUtil_modifyNumWifiAccessPoint(iidStack, 1);
      if (mdmLibCtx.eid == EID_SSK)
         return CMSRET_SUCCESS;
      else
         return CMSRET_REQUEST_DENIED;
   }
   else if (DELETE_EXISTING(newObj, currObj))
   {
      rutUtil_modifyNumWifiAccessPoint(iidStack, -1);
      return CMSRET_SUCCESS;
   }

   if (mdmLibCtx.eid != EID_WLMNGR && mdmLibCtx.eid  != EID_SSK)
   {
      UINT32 radioIndex = 0;
      paramNodeList *changedParams = NULL;

      radioIndex = newObj->X_BROADCOM_COM_Adapter; 

      if ((ret = cmsObj_compareObjects(newObj, currObj, &changedParams)) == CMSRET_SUCCESS)
      {
         int apIndex = newObj->X_BROADCOM_COM_Index;
         if (changedParams != NULL)
         {
            ret = rutWifi_updateWlmngr(MDMOID_DEV2_WIFI_ACCESS_POINT, radioIndex, apIndex, 0, changedParams);

            while (changedParams)
            {
               paramNodeList *tmpParamNode = changedParams;
               changedParams = tmpParamNode->nextNode;
               cmsMem_free(tmpParamNode);
            }
         }
      }
   }

   return ret;
}


CmsRet rcl_dev2WifiAccessPointSecurityObject( _Dev2WifiAccessPointSecurityObject *newObj __attribute__((unused)),
                      const _Dev2WifiAccessPointSecurityObject *currObj __attribute__((unused)),
                      const InstanceIdStack *iidStack __attribute__((unused)),
                      char **errorParam __attribute__((unused)),
                      CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;
   cmsLog_debug("===> Enter");
   if (ADD_NEW(newObj, currObj))
   {
         return CMSRET_SUCCESS;
   }
   else if (DELETE_EXISTING(newObj, currObj))
   {
      return CMSRET_SUCCESS;
   } 

   if (mdmLibCtx.eid == EID_TR69C || mdmLibCtx.eid == EID_CWMPD)
   {
      UINT32 radioIndex = 0;
      UINT32 apIndex = 0;
      paramNodeList *changedParams = NULL;

      if (newObj && currObj)
      {
         if (cmsUtl_strcmp(newObj->WEPKey, currObj->WEPKey) != 0)
         {
            switch(newObj->X_BROADCOM_COM_WlKeyIndex)
            {
               case 1:
                  REPLACE_STRING_IF_NOT_EQUAL_FLAGS(newObj->X_BROADCOM_COM_WlKey1, newObj->WEPKey, mdmLibCtx.allocFlags);
                  break;
               case 2:
                  REPLACE_STRING_IF_NOT_EQUAL_FLAGS(newObj->X_BROADCOM_COM_WlKey2, newObj->WEPKey, mdmLibCtx.allocFlags);
                  break;
               case 3:
                  REPLACE_STRING_IF_NOT_EQUAL_FLAGS(newObj->X_BROADCOM_COM_WlKey3, newObj->WEPKey, mdmLibCtx.allocFlags);
                  break;
               case 4:
                  REPLACE_STRING_IF_NOT_EQUAL_FLAGS(newObj->X_BROADCOM_COM_WlKey4, newObj->WEPKey, mdmLibCtx.allocFlags);
                  break;
               default:
                  newObj->X_BROADCOM_COM_WlKeyIndex = 1;
                  REPLACE_STRING_IF_NOT_EQUAL_FLAGS(newObj->X_BROADCOM_COM_WlKey1, newObj->WEPKey, mdmLibCtx.allocFlags);
                  break;
            }
         }   
         else
         {
            switch(newObj->X_BROADCOM_COM_WlKeyIndex)
            {
               case 1:
                  REPLACE_STRING_IF_NOT_EQUAL_FLAGS(newObj->WEPKey, newObj->X_BROADCOM_COM_WlKey1, mdmLibCtx.allocFlags);
                  break;
               case 2:
                  REPLACE_STRING_IF_NOT_EQUAL_FLAGS(newObj->WEPKey, newObj->X_BROADCOM_COM_WlKey2, mdmLibCtx.allocFlags);
                  break;
               case 3:
                  REPLACE_STRING_IF_NOT_EQUAL_FLAGS(newObj->WEPKey, newObj->X_BROADCOM_COM_WlKey3, mdmLibCtx.allocFlags);
                  break;
               case 4:
                  REPLACE_STRING_IF_NOT_EQUAL_FLAGS(newObj->WEPKey, newObj->X_BROADCOM_COM_WlKey4, mdmLibCtx.allocFlags);
                  break;
               default:
                  newObj->X_BROADCOM_COM_WlKeyIndex = 1;
                  REPLACE_STRING_IF_NOT_EQUAL_FLAGS(newObj->WEPKey, newObj->X_BROADCOM_COM_WlKey1, mdmLibCtx.allocFlags);
                  break;
            }
         } 
      } 

      if (newObj)
      {
         if (newObj->reset)
         {
            _Dev2WifiAccessPointSecurityObject *defaultObj = NULL;
            ret = cmsObj_get(MDMOID_DEV2_WIFI_ACCESS_POINT_SECURITY, iidStack, OGF_DEFAULT_VALUES, (void **)&defaultObj);
            if (ret != CMSRET_SUCCESS)
               return ret;

            newObj->reset = FALSE;
            REPLACE_STRING_IF_NOT_EQUAL_FLAGS(newObj->modeEnabled, defaultObj->modeEnabled, mdmLibCtx.allocFlags);
            REPLACE_STRING_IF_NOT_EQUAL_FLAGS(newObj->WEPKey, defaultObj->WEPKey, mdmLibCtx.allocFlags);
            REPLACE_STRING_IF_NOT_EQUAL_FLAGS(newObj->preSharedKey, defaultObj->preSharedKey, mdmLibCtx.allocFlags);
            REPLACE_STRING_IF_NOT_EQUAL_FLAGS(newObj->keyPassphrase, defaultObj->keyPassphrase, mdmLibCtx.allocFlags);
            newObj->rekeyingInterval = defaultObj->rekeyingInterval;

            REPLACE_STRING_IF_NOT_EQUAL_FLAGS(newObj->radiusServerIPAddr, defaultObj->radiusServerIPAddr, mdmLibCtx.allocFlags);
            REPLACE_STRING_IF_NOT_EQUAL_FLAGS(newObj->secondaryRadiusServerIPAddr, defaultObj->secondaryRadiusServerIPAddr, mdmLibCtx.allocFlags);
            REPLACE_STRING_IF_NOT_EQUAL_FLAGS(newObj->radiusSecret, defaultObj->radiusSecret, mdmLibCtx.allocFlags);
            REPLACE_STRING_IF_NOT_EQUAL_FLAGS(newObj->secondaryRadiusSecret, defaultObj->radiusSecret, mdmLibCtx.allocFlags);
            newObj->radiusServerPort= defaultObj->radiusServerPort;
            newObj->secondaryRadiusServerPort = defaultObj->secondaryRadiusServerPort;

            REPLACE_STRING_IF_NOT_EQUAL_FLAGS(newObj->X_BROADCOM_COM_WlKey1, defaultObj->X_BROADCOM_COM_WlKey1, mdmLibCtx.allocFlags);
            REPLACE_STRING_IF_NOT_EQUAL_FLAGS(newObj->X_BROADCOM_COM_WlKey2, defaultObj->X_BROADCOM_COM_WlKey2, mdmLibCtx.allocFlags);
            REPLACE_STRING_IF_NOT_EQUAL_FLAGS(newObj->X_BROADCOM_COM_WlKey3, defaultObj->X_BROADCOM_COM_WlKey3, mdmLibCtx.allocFlags);
            REPLACE_STRING_IF_NOT_EQUAL_FLAGS(newObj->X_BROADCOM_COM_WlKey4, defaultObj->X_BROADCOM_COM_WlKey4, mdmLibCtx.allocFlags);
            newObj->X_BROADCOM_COM_WlKeyIndex = defaultObj->X_BROADCOM_COM_WlKeyIndex;
            newObj->X_BROADCOM_COM_WlKeyBit = defaultObj->X_BROADCOM_COM_WlKeyBit;

            cmsObj_free((void **)&defaultObj);
         }

         if (!cmsUtl_strcmp(newObj->modeEnabled, MDMVS_NONE))
         {
            REPLACE_STRING_IF_NOT_EQUAL_FLAGS(newObj->wlAuthMode, "open", mdmLibCtx.allocFlags);
            REPLACE_STRING_IF_NOT_EQUAL_FLAGS(newObj->wlWep, "disabled", mdmLibCtx.allocFlags);
         }
         else if (!cmsUtl_strcmp(newObj->modeEnabled, MDMVS_WEP_64) || !cmsUtl_strcmp(newObj->modeEnabled, MDMVS_WEP_128))
         { 
            REPLACE_STRING_IF_NOT_EQUAL_FLAGS(newObj->wlWep, "enabled", mdmLibCtx.allocFlags);
            if (IS_EMPTY_STRING(newObj->radiusServerIPAddr) || !cmsUtl_strcmp(newObj->radiusServerIPAddr, "0.0.0.0"))
            {
               REPLACE_STRING_IF_NOT_EQUAL_FLAGS(newObj->wlAuthMode, "shared", mdmLibCtx.allocFlags);
            }
            else 
            {
               REPLACE_STRING_IF_NOT_EQUAL_FLAGS(newObj->wlAuthMode, "radius", mdmLibCtx.allocFlags);
            }

            if (!cmsUtl_strcmp(newObj->modeEnabled, MDMVS_WEP_64))
               newObj->X_BROADCOM_COM_WlKeyBit = 1;
            else
               newObj->X_BROADCOM_COM_WlKeyBit = 0;
         }
         else if (!cmsUtl_strcmp(newObj->modeEnabled, MDMVS_WPA2_PERSONAL))
         {
            REPLACE_STRING_IF_NOT_EQUAL_FLAGS(newObj->wlAuthMode, "psk2", mdmLibCtx.allocFlags);
         
         }
         else if (!cmsUtl_strcmp(newObj->modeEnabled, MDMVS_WPA_WPA2_PERSONAL))
         {
            REPLACE_STRING_IF_NOT_EQUAL_FLAGS(newObj->wlAuthMode, "psk psk2", mdmLibCtx.allocFlags);
         }
         else if (!cmsUtl_strcmp(newObj->modeEnabled, MDMVS_WPA2_ENTERPRISE))
         {
            if (IS_EMPTY_STRING(newObj->radiusServerIPAddr) || !cmsUtl_strcmp(newObj->radiusServerIPAddr, "0.0.0.0"))
               return CMSRET_INVALID_PARAM_VALUE;

            REPLACE_STRING_IF_NOT_EQUAL_FLAGS(newObj->wlAuthMode, "wpa2", mdmLibCtx.allocFlags);
         }
         else if (!cmsUtl_strcmp(newObj->modeEnabled, MDMVS_WPA_WPA2_ENTERPRISE))
         {
            if (IS_EMPTY_STRING(newObj->radiusServerIPAddr) || !cmsUtl_strcmp(newObj->radiusServerIPAddr, "0.0.0.0"))
               return CMSRET_INVALID_PARAM_VALUE;

            REPLACE_STRING_IF_NOT_EQUAL_FLAGS(newObj->wlAuthMode, "wpa wpa2", mdmLibCtx.allocFlags);
         }

         if (currObj && cmsUtl_strcmp(newObj->WEPKey, currObj->WEPKey))
         {
            newObj->X_BROADCOM_COM_WlKeyIndex = 1;
            REPLACE_STRING_IF_NOT_EQUAL_FLAGS(newObj->X_BROADCOM_COM_WlKey1, newObj->WEPKey, mdmLibCtx.allocFlags);
         }
      }

      Dev2WifiAccessPointObject *wifiAccessPointObj;

      if (cmsObj_get(MDMOID_DEV2_WIFI_ACCESS_POINT, iidStack, 0, (void *) &wifiAccessPointObj) == CMSRET_SUCCESS)
      {
         radioIndex = wifiAccessPointObj->X_BROADCOM_COM_Adapter;
         apIndex = wifiAccessPointObj->X_BROADCOM_COM_Index;
         cmsObj_free((void **) &wifiAccessPointObj);
      }

      if ((ret = cmsObj_compareObjects(newObj, currObj, &changedParams)) == CMSRET_SUCCESS)
      {
         if (changedParams != NULL)
         {
            ret = rutWifi_updateWlmngr(MDMOID_DEV2_WIFI_ACCESS_POINT_SECURITY, radioIndex, apIndex, 0, changedParams);

            while (changedParams)
            {
               paramNodeList *tmpParamNode = changedParams;
               changedParams = tmpParamNode->nextNode;
               cmsMem_free(tmpParamNode);
            }
         }
      }
   }

   return ret;
}
/* not support */
#if 0
CmsRet rcl_dev2WifiAccessPointAccountingObject( _Dev2WifiAccessPointAccountingObject *newObj __attribute__((unused)),
                      const _Dev2WifiAccessPointAccountingObject *currObj __attribute__((unused)),
                      const InstanceIdStack *iidStack __attribute__((unused)),
                      char **errorParam __attribute__((unused)),
                      CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;
   /* example for getting radio index
   UINT32 radioIndex;
   Dev2WifiAccessPointObject *wifiAccessPointObj;

   if (cmsObj_get(MDMOID_DEV2_WIFI_ACCESS_POINT, iidStack, 0, (void *) &wifiAccessPointObj) == CMSRET_SUCCESS)
   {
      radioIndex = qdmWifi_getRadioIndexBySsidFullPathLocked_dev2(wifiAccessPointObj->SSIDReference);
      cmsObj_free((void **) &wifiAccessPointObj);
   }
   */
   if (mdmLibCtx.eid == EID_TR69C)
   {
      UINT32 radioIndex = 0;
      paramNodeList *changedParams = NULL;

      Dev2WifiAccessPointObject *wifiAccessPointObj;

      if (cmsObj_get(MDMOID_DEV2_WIFI_ACCESS_POINT, iidStack, 0, (void *) &wifiAccessPointObj) == CMSRET_SUCCESS)
      {
         radioIndex = qdmWifi_getRadioIndexBySsidFullPathLocked_dev2(wifiAccessPointObj->SSIDReference);
         cmsObj_free((void **) &wifiAccessPointObj);
      }

      if ((ret = cmsObj_compareObjects(newObj, currObj, &changedParams)) == CMSRET_SUCCESS)
      {
         int apIndex= PEEK_INSTANCE_ID(iidStack) - 1;
         if (changedParams != NULL)
         {
            ret = rutWifi_updateWlmngr(MDMOID_DEV2_WIFI_SSID, radioIndex, apIndex, 0, changedParams);

            while (changedParams)
            {
               paramNodeList *tmpParamNode = changedParams;
               changedParams = tmpParamNode->nextNode;
               cmsMem_free(tmpParamNode);
            }
         }
      }
   }

   return ret;
}
#endif 

CmsRet rcl_dev2WifiAccessPointWpsObject( _Dev2WifiAccessPointWpsObject *newObj __attribute__((unused)),
                      const _Dev2WifiAccessPointWpsObject *currObj __attribute__((unused)),
                      const InstanceIdStack *iidStack __attribute__((unused)),
                      char **errorParam __attribute__((unused)),
                      CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;
   cmsLog_debug("===> Enter");
   if (ADD_NEW(newObj, currObj))
   {
      return CMSRET_SUCCESS;
   } 
   else if (DELETE_EXISTING(newObj, currObj))
   {
      return CMSRET_SUCCESS;
   }

   if ( newObj )
   {
      if (!IS_EMPTY_STRING(newObj->configMethodsSupported) && !IS_EMPTY_STRING(newObj->configMethodsEnabled))
      {
         if (cmsUtl_strstr(newObj->configMethodsSupported, newObj->configMethodsEnabled) == NULL)
         {
            return CMSRET_INVALID_PARAM_VALUE;
         }
      }
   }


   if (mdmLibCtx.eid == EID_TR69C || mdmLibCtx.eid == EID_CWMPD)
   {
      UINT32 radioIndex = 0;
      UINT32 apIndex = 0;
      paramNodeList *changedParams = NULL;

      Dev2WifiAccessPointObject *wifiAccessPointObj;

      if (cmsObj_get(MDMOID_DEV2_WIFI_ACCESS_POINT, iidStack, 0, (void *) &wifiAccessPointObj) == CMSRET_SUCCESS)
      {
         radioIndex = wifiAccessPointObj->X_BROADCOM_COM_Adapter;
         apIndex = wifiAccessPointObj->X_BROADCOM_COM_Index;
         cmsObj_free((void **) &wifiAccessPointObj);
      }

      if (newObj)
      {
         if (newObj->enable)
         {
            CMSMEM_REPLACE_STRING_FLAGS(newObj->X_BROADCOM_COM_Wsc_mode, "enabled", mdmLibCtx.allocFlags);
         }
         else
         {
            CMSMEM_REPLACE_STRING_FLAGS(newObj->X_BROADCOM_COM_Wsc_mode, "disabled", mdmLibCtx.allocFlags);
         }
      }

      if ((ret = cmsObj_compareObjects(newObj, currObj, &changedParams)) == CMSRET_SUCCESS)
      {
         if (changedParams != NULL)
         {
            ret = rutWifi_updateWlmngr(MDMOID_DEV2_WIFI_ACCESS_POINT_WPS, radioIndex, apIndex, 0, changedParams);

            while (changedParams)
            {
               paramNodeList *tmpParamNode = changedParams;
               changedParams = tmpParamNode->nextNode;
               cmsMem_free(tmpParamNode);
            }
         }
      }
   }

   return ret;
}


CmsRet rcl_dev2WifiAssociatedDeviceObject( _Dev2WifiAssociatedDeviceObject *newObj __attribute__((unused)),
                      const _Dev2WifiAssociatedDeviceObject *currObj __attribute__((unused)),
                      const InstanceIdStack *iidStack __attribute__((unused)),
                      char **errorParam __attribute__((unused)),
                      CmsRet *errorCode __attribute__((unused)))
{
   cmsLog_debug("===> Enter");
   if (ADD_NEW(newObj, currObj))
   {
      rutUtil_modifyNumWifiAssociatedDevice(iidStack, 1);
   }
   else if (DELETE_EXISTING(newObj, currObj))
   {
      rutUtil_modifyNumWifiAssociatedDevice(iidStack, -1);
   }

   return CMSRET_SUCCESS;
}

#endif  /* DMP_DEVICE2_WIFIACCESSPOINT_1 */

#endif    /* DMP_DEVICE2_BASELINE_1 */

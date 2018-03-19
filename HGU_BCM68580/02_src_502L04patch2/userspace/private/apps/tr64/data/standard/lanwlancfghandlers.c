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
//  Filename:       lanwlancfghandlers.c
//
******************************************************************************/
#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "igd.h"
#include "lanwlancfgparams.h"

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"


#define WL_MIN_PSK_LEN                  8
#define WL_MAX_PSK_LEN                  64
#define WL_WPA_PSK_SIZE_MAX             72  // max 64 hex or 63 char
#define WL_KEY64_SIZE_HEX               10
#define WL_KEY64_SIZE_CHAR              5
#define WL_KEY128_SIZE_HEX              26
#define WL_KEY128_SIZE_CHAR             13

#define FOLLOW_TR098

#ifndef FOLLOW_TR098
#define WL_DISABLED              "disabled"
#define WL_ENABLED               "enabled"
#define WL_AUTH_OPEN             "open"
#define WL_AUTH_SHARED           "shared"
#define WL_AUTH_RADIUS           "radius"
#define WL_AUTH_WPA              "wpa"
#define WL_AUTH_WPA_PSK          "psk"
#define WL_AUTH_WPA2             "wpa2"
#define WL_AUTH_WPA2_PSK         "psk2"
#define WL_AUTH_WPA2_MIX         "wpa wpa2"
#define WL_AUTH_WPA2_PSK_MIX     "psk psk2"
#define TKIP_ONLY                "tkip"
#define AES_ONLY                 "aes"
#define TKIP_AND_AES             "tkip+aes"

#endif

#ifndef FOLLOW_TR098
void DoNothingJustWorkAroundWirelessNonStandardTR98Support()
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanWlanObject *wirelessObj=NULL;
   CmsRet ret = CMSRET_SUCCESS;

   PUSH_INSTANCE_ID(&iidStack, 1);
   PUSH_INSTANCE_ID(&iidStack, 1);

   if (cmsObj_get(MDMOID_LAN_WLAN, &iidStack, 0, (void **) &wirelessObj) == CMSRET_SUCCESS)
   {
      ret = cmsObj_set(wirelessObj, &iidStack);
      cmsObj_free((void **) &wirelessObj);   
   }
}
#endif

int GetWlanInfo(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)       
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanWlanObject *wirelessObj=NULL;

   int errorinfo = 0;
   int instanceOfLANDevice;
   int instanceOfService;

   instanceOfLANDevice = psvc->device->instance;
   instanceOfService = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);
                      
   if (cmsObj_get(MDMOID_LAN_WLAN, &iidStack, 0, (void **) &wirelessObj) == CMSRET_SUCCESS)
   {
      if(wirelessObj->enable)
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_Enable, "1");  
      }
      else
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_Enable, "0");  
      }
      errorinfo |= OutputCharValueToAC(ac, VAR_Status, wirelessObj->status);
      errorinfo |= OutputCharValueToAC(ac, VAR_MaxBitRate, wirelessObj->maxBitRate);
      errorinfo |= OutputCharValueToAC(ac, VAR_SSID, wirelessObj->SSID);
#ifdef FOLLOW_TR098
      errorinfo |= OutputCharValueToAC(ac, VAR_BeaconType, wirelessObj->beaconType);
      errorinfo |= OutputNumValueToAC(ac, VAR_Channel, wirelessObj->channel);
#endif
      if(wirelessObj->MACAddressControlEnabled)
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_MACAddressControlEnabled, "1");      
      }
      else
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_MACAddressControlEnabled, "0");      
      }
      errorinfo |= OutputCharValueToAC(ac, VAR_Standard, wirelessObj->standard);
      errorinfo |= OutputCharValueToAC(ac, VAR_BSSID, wirelessObj->BSSID);
#ifdef FOLLOW_TR098
      errorinfo |= OutputCharValueToAC(ac, VAR_BasicEncryptionModes, wirelessObj->basicEncryptionModes);
      errorinfo |= OutputCharValueToAC(ac, VAR_BasicAuthenticationMode, wirelessObj->basicAuthenticationMode);
#endif
      cmsObj_free((void **) &wirelessObj);   
   }   
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
#ifndef FOLLOW_TR098
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);
   _WlVirtIntfCfgObject *wlVirtIntfCfgObj=NULL;
   if (cmsObj_get(MDMOID_WL_VIRT_INTF_CFG, &iidStack, 0, (void **) &wlVirtIntfCfgObj)== CMSRET_SUCCESS)
   {
      if( strcmp(wlVirtIntfCfgObj->wlWep, WL_ENABLED) == 0 )
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_BasicEncryptionModes, "WEPEncryption");
      }
      else
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_BasicEncryptionModes, "None");
      }

      if( strcmp(wlVirtIntfCfgObj->wlAuthMode, WL_AUTH_WPA_PSK) == 0 )
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_BeaconType, "WPA");  
         errorinfo |= OutputCharValueToAC(ac, VAR_BasicAuthenticationMode, "None");
      }
      else if( strcmp(wlVirtIntfCfgObj->wlAuthMode, WL_AUTH_RADIUS) == 0 )
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_BeaconType, "Basic");  
         errorinfo |= OutputCharValueToAC(ac, VAR_BasicAuthenticationMode, "EAPAuthentication");
      }
      else
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_BeaconType, "Basic");  
         errorinfo |= OutputCharValueToAC(ac, VAR_BasicAuthenticationMode, "None");
      }
      cmsObj_free((void **) &wlVirtIntfCfgObj);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   INIT_INSTANCE_ID_STACK(&iidStack);
   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);
   _WlBaseCfgObject *wlBaseCfgObj=NULL;
   if (cmsObj_get(MDMOID_WL_BASE_CFG, &iidStack, 0, (void **) &wlBaseCfgObj)== CMSRET_SUCCESS)
   {
      errorinfo |= OutputNumValueToAC(ac, VAR_Channel, wlBaseCfgObj->wlChannel);
      cmsObj_free((void **) &wlBaseCfgObj);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
#endif

   if(errorinfo)
   {
      soap_error( uclient, errorinfo );
      return FALSE;
   } 
   return TRUE;
}

int GetWlanStatistics(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)       
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanWlanObject *wirelessObj=NULL;

   int errorinfo = 0;
                      
   int instanceOfLANDevice;
   int instanceOfService;

   instanceOfLANDevice = psvc->device->instance;
   instanceOfService = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);
                      
   if (cmsObj_get(MDMOID_LAN_WLAN, &iidStack, 0, (void **) &wirelessObj) == CMSRET_SUCCESS)
   {
      errorinfo |= OutputNumValueToAC(ac, VAR_TotalBytesSent, wirelessObj->totalBytesSent);  
      errorinfo |= OutputNumValueToAC(ac, VAR_TotalBytesReceived, wirelessObj->totalBytesReceived);
      errorinfo |= OutputNumValueToAC(ac, VAR_TotalPacketsSent, wirelessObj->totalPacketsSent);
      errorinfo |= OutputNumValueToAC(ac, VAR_TotalPacketsReceived, wirelessObj->totalPacketsReceived);
      cmsObj_free((void **) &wirelessObj);   
   }
   else
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

int GetWlanByteStatistics(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)       
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanWlanObject *wirelessObj=NULL;

   int errorinfo = 0;                
   int instanceOfLANDevice;
   int instanceOfService;

   instanceOfLANDevice = psvc->device->instance;
   instanceOfService = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);
                      
   if (cmsObj_get(MDMOID_LAN_WLAN, &iidStack, 0, (void **) &wirelessObj) == CMSRET_SUCCESS)
   {
      errorinfo |= OutputNumValueToAC(ac, VAR_TotalBytesSent, wirelessObj->totalBytesSent);  
      errorinfo |= OutputNumValueToAC(ac, VAR_TotalBytesReceived, wirelessObj->totalBytesReceived);      
      cmsObj_free((void **) &wirelessObj);   
   }
   else
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

int GetWlanPacketStatistics(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)       
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanWlanObject *wirelessObj=NULL;

   int errorinfo = 0;
                     
   int instanceOfLANDevice;
   int instanceOfService;

   instanceOfLANDevice = psvc->device->instance;
   instanceOfService = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);
                      
   if (cmsObj_get(MDMOID_LAN_WLAN, &iidStack, 0, (void **) &wirelessObj) == CMSRET_SUCCESS)
   {
      errorinfo |= OutputNumValueToAC(ac, VAR_TotalPacketsSent, wirelessObj->totalPacketsSent);
      errorinfo |= OutputNumValueToAC(ac, VAR_TotalPacketsReceived, wirelessObj->totalPacketsReceived);

      cmsObj_free((void **) &wirelessObj);   
   }
   else
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

int SetEnable(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
#ifdef FOLLOW_TR098                  
   LanWlanObject *wirelessObj=NULL;
#endif
   CmsRet ret = CMSRET_SUCCESS;
   int Enable = 0;
   struct Param *pParams;                    
   int instanceOfLANDevice;
   int instanceOfService;

   instanceOfLANDevice = psvc->device->instance;
   instanceOfService = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);
    
   pParams = findActionParamByRelatedVar(ac,VAR_Enable);
   if (pParams != NULL)
   {
      Enable = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
#ifdef FOLLOW_TR098                  
   if (cmsObj_get(MDMOID_LAN_WLAN, &iidStack, 0, (void **) &wirelessObj) == CMSRET_SUCCESS)
   {
      wirelessObj->enable = Enable;
      ret = cmsObj_set(wirelessObj, &iidStack);
      cmsObj_free((void **) &wirelessObj);   
   }
#else
   _WlBaseCfgObject *wlBaseCfgObj=NULL;
   if (cmsObj_get(MDMOID_WL_BASE_CFG, &iidStack, 0, (void **) &wlBaseCfgObj)== CMSRET_SUCCESS)
   {
      wlBaseCfgObj->wlEnbl = Enable;  
      ret = cmsObj_set(wlBaseCfgObj, &iidStack);
      cmsObj_free((void **) &wlBaseCfgObj);
   }
#endif
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

#ifndef FOLLOW_TR098
   DoNothingJustWorkAroundWirelessNonStandardTR98Support();
#endif
   return TRUE;

}

int GetEnable(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
#ifdef FOLLOW_TR098                  
   LanWlanObject *wirelessObj=NULL;
#endif
   int errorinfo = 0;
   int instanceOfLANDevice;
   int instanceOfService;

   instanceOfLANDevice = psvc->device->instance;
   instanceOfService = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);
#ifdef FOLLOW_TR098
   if (cmsObj_get(MDMOID_LAN_WLAN, &iidStack, 0, (void **) &wirelessObj) == CMSRET_SUCCESS)
   {
      if(wirelessObj->enable)
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_Enable, "1");
      }
      else
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_Enable, "0");
      }

      cmsObj_free((void **) &wirelessObj);   
   }
#else
   _WlBaseCfgObject *wlBaseCfgObj=NULL;
   if (cmsObj_get(MDMOID_WL_BASE_CFG, &iidStack, 0, (void **) &wlBaseCfgObj)== CMSRET_SUCCESS)
   {
      if(wlBaseCfgObj->wlEnbl)
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_Enable, "1");
      }
      else
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_Enable, "0");
      }

      cmsObj_free((void **) &wlBaseCfgObj);
   }
#endif
   else
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

int SetSSID(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
#ifdef FOLLOW_TR098                  
   LanWlanObject *wirelessObj=NULL;
#endif
   CmsRet ret = CMSRET_SUCCESS;

   struct Param *pParams;
                      
   int instanceOfLANDevice;
   int instanceOfService;

   instanceOfLANDevice = psvc->device->instance;
   instanceOfService = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);
    
   pParams = findActionParamByRelatedVar(ac,VAR_SSID);
   if (pParams != NULL)
   {
#ifdef FOLLOW_TR098   
      if (cmsObj_get(MDMOID_LAN_WLAN, &iidStack, 0, (void **) &wirelessObj) == CMSRET_SUCCESS)
      {

         cmsMem_free(wirelessObj->SSID);
         wirelessObj->SSID = cmsMem_strdup(pParams->value);  
         ret = cmsObj_set(wirelessObj, &iidStack);

         cmsObj_free((void **) &wirelessObj);   
      }
#else
      PUSH_INSTANCE_ID(&iidStack, instanceOfService);
      _WlVirtIntfCfgObject *wlVirtIntfCfgObj=NULL;
      if (cmsObj_get(MDMOID_WL_VIRT_INTF_CFG, &iidStack, 0, (void **) &wlVirtIntfCfgObj)== CMSRET_SUCCESS)
      {
         cmsMem_free(wlVirtIntfCfgObj->wlSsid);
         wlVirtIntfCfgObj->wlSsid = cmsMem_strdup(pParams->value);  
         ret = cmsObj_set(wlVirtIntfCfgObj, &iidStack);
         cmsObj_free((void **) &wlVirtIntfCfgObj);
      }
#endif
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
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }         

   cmsMgm_saveConfigToFlash();

#ifndef FOLLOW_TR098
   DoNothingJustWorkAroundWirelessNonStandardTR98Support();
#endif
   return TRUE;
}

int GetSSID(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
#ifdef FOLLOW_TR098                  
   LanWlanObject *wirelessObj=NULL;
#endif
   int errorinfo = 0;
   int instanceOfLANDevice;
   int instanceOfService;

   instanceOfLANDevice = psvc->device->instance;
   instanceOfService = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);

#ifdef FOLLOW_TR098
   if (cmsObj_get(MDMOID_LAN_WLAN, &iidStack, 0, (void **) &wirelessObj) == CMSRET_SUCCESS)
   {
      errorinfo |= OutputCharValueToAC(ac, VAR_SSID, wirelessObj->SSID);
      cmsObj_free((void **) &wirelessObj);   
   }
#else
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);
   _WlVirtIntfCfgObject *wlVirtIntfCfgObj=NULL;
   if (cmsObj_get(MDMOID_WL_VIRT_INTF_CFG, &iidStack, 0, (void **) &wlVirtIntfCfgObj)== CMSRET_SUCCESS)
   {
      errorinfo |= OutputCharValueToAC(ac, VAR_SSID, wlVirtIntfCfgObj->wlSsid);
      cmsObj_free((void **) &wlVirtIntfCfgObj);
   }
#endif

   else
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


int GetBSSID(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{

   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanWlanObject *wirelessObj=NULL;

   int errorinfo = 0;
   int instanceOfLANDevice;
   int instanceOfService;

   instanceOfLANDevice = psvc->device->instance;
   instanceOfService = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);

   if (cmsObj_get(MDMOID_LAN_WLAN, &iidStack, 0, (void **) &wirelessObj) == CMSRET_SUCCESS)
   {
      errorinfo |= OutputCharValueToAC(ac, VAR_BSSID, wirelessObj->BSSID);
      cmsObj_free((void **) &wirelessObj);   
   }
   else
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

int SetChannel(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
#ifdef FOLLOW_TR098                  
   LanWlanObject *wirelessObj=NULL;
#endif
   CmsRet ret = CMSRET_SUCCESS;

   int Channel = 0;
   struct Param *pParams;
                      
   int instanceOfLANDevice;
   int instanceOfService;

   instanceOfLANDevice = psvc->device->instance;
   instanceOfService = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);
    
   pParams = findActionParamByRelatedVar(ac, VAR_Channel);
   if (pParams != NULL)
   {
      Channel = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
#ifdef FOLLOW_TR098                  
   if (cmsObj_get(MDMOID_LAN_WLAN, &iidStack, 0, (void **) &wirelessObj) == CMSRET_SUCCESS)
   {
      wirelessObj->channel = Channel;
      ret = cmsObj_set(wirelessObj, &iidStack);
      cmsObj_free((void **) &wirelessObj);   
   }
#else
   _WlBaseCfgObject *wlBaseCfgObj=NULL;
   if (cmsObj_get(MDMOID_WL_BASE_CFG, &iidStack, 0, (void **) &wlBaseCfgObj)== CMSRET_SUCCESS)
   {
      wlBaseCfgObj->wlChannel = Channel;
      ret = cmsObj_set(wlBaseCfgObj, &iidStack);
      cmsObj_free((void **) &wlBaseCfgObj);
   }
#endif
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

int GetChannelInfo(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanWlanObject *wirelessObj=NULL;
   int errorinfo = 0;
   int instanceOfLANDevice;
   int instanceOfService;

   instanceOfLANDevice = psvc->device->instance;
   instanceOfService = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);

   if (cmsObj_get(MDMOID_LAN_WLAN, &iidStack, 0, (void **) &wirelessObj) == CMSRET_SUCCESS)
   {
#ifdef FOLLOW_TR098
      errorinfo |= OutputNumValueToAC(ac, VAR_Channel, wirelessObj->channel);
#endif
      errorinfo |= OutputCharValueToAC(ac, VAR_PossibleChannels, wirelessObj->possibleChannels);
      cmsObj_free((void **) &wirelessObj);   
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

#ifndef FOLLOW_TR098
   INIT_INSTANCE_ID_STACK(&iidStack);
   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);
   _WlBaseCfgObject *wlBaseCfgObj=NULL;
   if (cmsObj_get(MDMOID_WL_BASE_CFG, &iidStack, 0, (void **) &wlBaseCfgObj)== CMSRET_SUCCESS)
   {
      errorinfo |= OutputNumValueToAC(ac, VAR_Channel, wlBaseCfgObj->wlChannel);
      cmsObj_free((void **) &wlBaseCfgObj);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
#endif

   if(errorinfo)
   {
      soap_error( uclient, errorinfo );
      return FALSE;
   }
    
   return TRUE;

}

int GetBeaconType(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
#ifdef FOLLOW_TR098
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanWlanObject *wirelessObj=NULL;
   int errorinfo = 0;
   int instanceOfLANDevice;
   int instanceOfService;

   instanceOfLANDevice = psvc->device->instance;
   instanceOfService = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);

   if (cmsObj_get(MDMOID_LAN_WLAN, &iidStack, 0, (void **) &wirelessObj) == CMSRET_SUCCESS)
   {
      errorinfo |= OutputCharValueToAC(ac, VAR_BeaconType, wirelessObj->beaconType);
      cmsObj_free((void **) &wirelessObj);   
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   if(errorinfo)
   {
      soap_error( uclient, errorinfo );
      return FALSE;
   }
#else
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   _WlVirtIntfCfgObject *wlVirtIntfCfgObj=NULL;
   int errorinfo = 0;
   int instanceOfLANDevice;
   int instanceOfService;

   instanceOfLANDevice = psvc->device->instance;
   instanceOfService = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);


   if (cmsObj_get(MDMOID_WL_VIRT_INTF_CFG, &iidStack, 0, (void **) &wlVirtIntfCfgObj)== CMSRET_SUCCESS)
   {
      if(strcmp(wlVirtIntfCfgObj->wlAuthMode, WL_AUTH_WPA_PSK)==0)
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_BeaconType, "WPA");  
      }
      else if(strcmp(wlVirtIntfCfgObj->wlAuthMode, WL_AUTH_OPEN)==0)
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_BeaconType, "Basic");  
      }
      else if(strcmp(wlVirtIntfCfgObj->wlAuthMode, WL_AUTH_SHARED)==0)
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_BeaconType, "Basic");  
      }
      else
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_BeaconType, "Basic");  
      }
      cmsObj_free((void **) &wlVirtIntfCfgObj);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   if(errorinfo)
   {
      soap_error( uclient, errorinfo );
      return FALSE;
   }
#endif    

   return TRUE;
}

int SetRadioMode(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
#ifdef FOLLOW_TR098                  
   LanWlanObject *wirelessObj=NULL;
#endif

   int RadioEnable = 0;
   struct Param *pParams;
                      
   int instanceOfLANDevice;
   int instanceOfService;

   instanceOfLANDevice = psvc->device->instance;
   instanceOfService = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);
    
   pParams = findActionParamByRelatedVar(ac,VAR_RadioEnabled);
   if (pParams != NULL)
   {
      RadioEnable = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
#ifdef FOLLOW_TR098                  
   if (cmsObj_get(MDMOID_LAN_WLAN, &iidStack, 0, (void **) &wirelessObj) == CMSRET_SUCCESS)
   {
      wirelessObj->radioEnabled = RadioEnable;
      cmsObj_set(wirelessObj, &iidStack);
      cmsObj_free((void **) &wirelessObj);   
   }
#else
   _WlBaseCfgObject *wlBaseCfgObj=NULL;
   if (cmsObj_get(MDMOID_WL_BASE_CFG, &iidStack, 0, (void **) &wlBaseCfgObj)== CMSRET_SUCCESS)
   {
      wlBaseCfgObj->wlEnbl = RadioEnable;  
      cmsObj_set(wlBaseCfgObj, &iidStack);
      cmsObj_free((void **) &wlBaseCfgObj);
   }
#endif
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   } 

   cmsMgm_saveConfigToFlash();
#ifndef FOLLOW_TR098
   DoNothingJustWorkAroundWirelessNonStandardTR98Support();
#endif
   return TRUE;
}

int GetRadioMode(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
#ifdef FOLLOW_TR098                  
   LanWlanObject *wirelessObj=NULL;
#endif

   int errorinfo = 0;

   int instanceOfLANDevice;
   int instanceOfService;

   instanceOfLANDevice = psvc->device->instance;
   instanceOfService = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);
#ifdef FOLLOW_TR098
   if (cmsObj_get(MDMOID_LAN_WLAN, &iidStack, 0, (void **) &wirelessObj) == CMSRET_SUCCESS)
   {
      if(wirelessObj->radioEnabled)
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_RadioEnabled, "1");
      }
      else
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_RadioEnabled, "0");
      }

      cmsObj_free((void **) &wirelessObj);   
   }
#else
   _WlBaseCfgObject *wlBaseCfgObj=NULL;
   if (cmsObj_get(MDMOID_WL_BASE_CFG, &iidStack, 0, (void **) &wlBaseCfgObj)== CMSRET_SUCCESS)
   {
      if(wlBaseCfgObj->wlEnbl)
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_RadioEnabled, "1");
      }
      else
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_RadioEnabled, "0");
      }

      cmsObj_free((void **) &wlBaseCfgObj);
   }
#endif
   else
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

int SetConfig(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanWlanObject *wirelessObj=NULL;
   CmsRet ret = CMSRET_SUCCESS;

   char MaxBitRate[32];
   char Channel[32];
   char SSID[32];
   char BeaconType[32];
   char BasicEncryptionModes[32];
   char BasicAuthenticationMode[32];
   int MACAddressControlEnabled;
   
   struct Param *pParams;

   int instanceOfLANDevice;
   int instanceOfService;

   instanceOfLANDevice = psvc->device->instance;
   instanceOfService = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);
   
    //MaxBitRate
   pParams = findActionParamByRelatedVar(ac,VAR_MaxBitRate);
   if (pParams != NULL)
   {
      strcpy(MaxBitRate, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   //Channel
   pParams = findActionParamByRelatedVar(ac,VAR_Channel);
   if (pParams != NULL)
   {
      strcpy(Channel, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   //SSID
   pParams = findActionParamByRelatedVar(ac,VAR_SSID);
   if (pParams != NULL)
   {
      strcpy(SSID, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }   

   //BeaconType
   pParams = findActionParamByRelatedVar(ac,VAR_BeaconType);
   if (pParams != NULL)
   {
      strcpy(BeaconType, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }   
   //BasicEncryptionModes
   pParams = findActionParamByRelatedVar(ac,VAR_BasicEncryptionModes);
   if (pParams != NULL)
   {
      strcpy(BasicEncryptionModes, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   //BasicAuthenticationMode
   pParams = findActionParamByRelatedVar(ac,VAR_BasicAuthenticationMode);
   if (pParams != NULL)
   {
      strcpy(BasicAuthenticationMode, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }   

   //MACAddressControlEnabled
   pParams = findActionParamByRelatedVar(ac,VAR_MACAddressControlEnabled);
   if (pParams != NULL)
   {
       MACAddressControlEnabled = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   if (cmsObj_get(MDMOID_LAN_WLAN, &iidStack, 0, (void **) &wirelessObj) == CMSRET_SUCCESS)
   {
      cmsMem_free(wirelessObj->maxBitRate);
      wirelessObj->maxBitRate = cmsMem_strdup(MaxBitRate); 

      wirelessObj->channel = atoi(Channel);

#ifdef FOLLOW_TR098
      cmsMem_free(wirelessObj->SSID);
      wirelessObj->SSID = cmsMem_strdup(SSID); 

      cmsMem_free(wirelessObj->beaconType);
      wirelessObj->beaconType = cmsMem_strdup(BeaconType); 

      wirelessObj->MACAddressControlEnabled = MACAddressControlEnabled?TRUE:FALSE;

      cmsMem_free(wirelessObj->basicEncryptionModes);
      wirelessObj->basicEncryptionModes = cmsMem_strdup(BasicEncryptionModes); 

      cmsMem_free(wirelessObj->basicAuthenticationMode);
      wirelessObj->basicAuthenticationMode = cmsMem_strdup(BasicAuthenticationMode); 
#endif
      ret = cmsObj_set(wirelessObj, &iidStack);

      cmsObj_free((void **) &wirelessObj);   
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

#ifndef FOLLOW_TR098
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);
   _WlVirtIntfCfgObject *wlVirtIntfCfgObj=NULL;
   if (cmsObj_get(MDMOID_WL_VIRT_INTF_CFG, &iidStack, 0, (void **) &wlVirtIntfCfgObj)== CMSRET_SUCCESS)
   {
      cmsMem_free(wlVirtIntfCfgObj->wlSsid);
      wlVirtIntfCfgObj->wlSsid = cmsMem_strdup(SSID);  

      if(strcmp(BeaconType, "WPA")==0)
      {
         cmsMem_free(wlVirtIntfCfgObj->wlAuthMode);
         wlVirtIntfCfgObj->wlAuthMode = cmsMem_strdup(WL_AUTH_WPA_PSK); 
      }
      else if(strcmp(BeaconType, "Basic")==0)
      {
         cmsMem_free(wlVirtIntfCfgObj->wlAuthMode);
         wlVirtIntfCfgObj->wlAuthMode = cmsMem_strdup(WL_AUTH_OPEN); 
      }

      if(strcmp(BasicEncryptionModes, "WEPEncryption")==0)
      {
         cmsMem_free(wlVirtIntfCfgObj->wlWep);
         wlVirtIntfCfgObj->wlWep = cmsMem_strdup(WL_ENABLED); 
      }
      else if(strcmp(BasicEncryptionModes, "None")==0)
      {
         cmsMem_free(wlVirtIntfCfgObj->wlWep);
         wlVirtIntfCfgObj->wlWep = cmsMem_strdup(WL_DISABLED); 
      }

      if(strcmp(BasicAuthenticationMode, "EAPAuthentication")==0)
      {
         if(strcmp(BeaconType, "Basic")==0)
         {
            cmsMem_free(wlVirtIntfCfgObj->wlAuthMode);
            wlVirtIntfCfgObj->wlAuthMode = cmsMem_strdup(WL_AUTH_RADIUS); 
         }
      }

      ret = cmsObj_set(wlVirtIntfCfgObj, &iidStack);

      cmsObj_free((void **) &wlVirtIntfCfgObj);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

#endif

   if(ret!=CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }     

   cmsMgm_saveConfigToFlash();
   
   return TRUE;

}

int SetBeaconType(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
#ifdef FOLLOW_TR098                  
   LanWlanObject *wirelessObj=NULL;
#endif
   CmsRet ret = CMSRET_SUCCESS;

   char BeaconType[32];
   
   struct Param *pParams;

   int instanceOfLANDevice;
   int instanceOfService;

   instanceOfLANDevice = psvc->device->instance;
   instanceOfService = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);
   
   //BeaconType
   pParams = findActionParamByRelatedVar(ac,VAR_BeaconType);
   if (pParams != NULL)
   {
      strcpy(BeaconType, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }   
#ifdef FOLLOW_TR098
   if (cmsObj_get(MDMOID_LAN_WLAN, &iidStack, 0, (void **) &wirelessObj) == CMSRET_SUCCESS)
   {
      cmsMem_free(wirelessObj->beaconType);
      wirelessObj->beaconType = cmsMem_strdup(BeaconType); 
      ret = cmsObj_set(wirelessObj, &iidStack);
      cmsObj_free((void **) &wirelessObj);   
   }
#else
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);
   _WlVirtIntfCfgObject *wlVirtIntfCfgObj=NULL;
   if (cmsObj_get(MDMOID_WL_VIRT_INTF_CFG, &iidStack, 0, (void **) &wlVirtIntfCfgObj)== CMSRET_SUCCESS)
   {
      if(strcmp(BeaconType, "WPA")==0)
      {
         cmsMem_free(wlVirtIntfCfgObj->wlAuthMode);
         wlVirtIntfCfgObj->wlAuthMode = cmsMem_strdup(WL_AUTH_WPA_PSK); 
      }
      else if(strcmp(BeaconType, "Basic")==0)
      {
         cmsMem_free(wlVirtIntfCfgObj->wlAuthMode);
         wlVirtIntfCfgObj->wlAuthMode = cmsMem_strdup(WL_AUTH_OPEN);         
      }
      
      ret = cmsObj_set(wlVirtIntfCfgObj, &iidStack);

      cmsObj_free((void **) &wlVirtIntfCfgObj);
   }
#endif
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

#ifndef FOLLOW_TR098
   DoNothingJustWorkAroundWirelessNonStandardTR98Support();
#endif

   return TRUE;

}

int GetDefaultWEPKeyIndex(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
#ifdef FOLLOW_TR098                  
   LanWlanObject *wirelessObj=NULL;
#endif

   int errorinfo = 0;
   int instanceOfLANDevice;
   int instanceOfService;

   instanceOfLANDevice = psvc->device->instance;
   instanceOfService = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);
#ifdef FOLLOW_TR098
   if (cmsObj_get(MDMOID_LAN_WLAN, &iidStack, 0, (void **) &wirelessObj) == CMSRET_SUCCESS)
   {
      errorinfo |= OutputNumValueToAC(ac, VAR_WEPKeyIndex, wirelessObj->WEPKeyIndex);
      cmsObj_free((void **) &wirelessObj);   
   }
#else
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);
   _WlVirtIntfCfgObject *wlVirtIntfCfgObj=NULL;
   if (cmsObj_get(MDMOID_WL_VIRT_INTF_CFG, &iidStack, 0, (void **) &wlVirtIntfCfgObj)== CMSRET_SUCCESS)
   {
      if(wlVirtIntfCfgObj->wlKeyBit==0)
      {
         errorinfo |= OutputNumValueToAC(ac, VAR_WEPKeyIndex, wlVirtIntfCfgObj->wlKeyIndex128);
      }
      else
      {
         errorinfo |= OutputNumValueToAC(ac, VAR_WEPKeyIndex, wlVirtIntfCfgObj->wlKeyIndex64);
      }

      cmsObj_free((void **) &wlVirtIntfCfgObj);
   }
#endif
   else
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

int SetDefaultWEPKeyIndex(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
#ifdef FOLLOW_TR098                  
   LanWlanObject *wirelessObj=NULL;
#endif
   CmsRet ret = CMSRET_SUCCESS;
   int WEPKeyIndex = 0;
   struct Param *pParams;                    
   int instanceOfLANDevice;
   int instanceOfService;

   instanceOfLANDevice = psvc->device->instance;
   instanceOfService = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);
    
   pParams = findActionParamByRelatedVar(ac,VAR_WEPKeyIndex);
   if (pParams != NULL)
   {
      WEPKeyIndex = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
#ifdef FOLLOW_TR098                  
   if (cmsObj_get(MDMOID_LAN_WLAN, &iidStack, 0, (void **) &wirelessObj) == CMSRET_SUCCESS)
   {
      wirelessObj->WEPKeyIndex = WEPKeyIndex;
      ret = cmsObj_set(wirelessObj, &iidStack);
      cmsObj_free((void **) &wirelessObj);   
   }
#else
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);
   _WlVirtIntfCfgObject *wlVirtIntfCfgObj=NULL;
   if (cmsObj_get(MDMOID_WL_VIRT_INTF_CFG, &iidStack, 0, (void **) &wlVirtIntfCfgObj)== CMSRET_SUCCESS)
   {
      if(wlVirtIntfCfgObj->wlKeyBit==0)
      {
         wlVirtIntfCfgObj->wlKeyIndex128 = WEPKeyIndex;  
      }
      else
      {
         wlVirtIntfCfgObj->wlKeyIndex64 = WEPKeyIndex;  
      }
      ret = cmsObj_set(wlVirtIntfCfgObj, &iidStack);
      cmsObj_free((void **) &wlVirtIntfCfgObj);
   }
#endif
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

#ifndef FOLLOW_TR098
   DoNothingJustWorkAroundWirelessNonStandardTR98Support();
#endif
   return TRUE;
}

int GetTotalAssociations(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanWlanObject *wirelessObj=NULL;
   int errorinfo = 0;
   int instanceOfLANDevice;
   int instanceOfService;

   instanceOfLANDevice = psvc->device->instance;
   instanceOfService = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);

   if (cmsObj_get(MDMOID_LAN_WLAN, &iidStack, 0, (void **) &wirelessObj) == CMSRET_SUCCESS)
   {
      errorinfo |= OutputNumValueToAC(ac, VAR_TotalAssociations, wirelessObj->totalAssociations);
      cmsObj_free((void **) &wirelessObj);   
   }
   else
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
              
int GetGenericAssociatedDeviceInfo(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack parentIidStack = EMPTY_INSTANCE_ID_STACK;

   LanWlanAssociatedDeviceEntryObject *wirelessAssociatedObj=NULL;

   uint32 AssociatedDeviceIndex = 0;
   int found = 0;
   int errorinfo = 0;
   
   struct Param *pParams;

   int instanceOfLANDevice;
   int instanceOfService;

   instanceOfLANDevice = psvc->device->instance;
   instanceOfService = psvc->instance;

   PUSH_INSTANCE_ID(&parentIidStack, instanceOfLANDevice);
   PUSH_INSTANCE_ID(&parentIidStack, instanceOfService);

   pParams = findActionParamByRelatedVar(ac,VAR_TotalAssociations);
   if (pParams != NULL)
   {
      AssociatedDeviceIndex = atoi(pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   while (cmsObj_getNextInSubTree(MDMOID_LAN_WLAN_ASSOCIATED_DEVICE_ENTRY, &parentIidStack, &iidStack,
                                     (void **)&wirelessAssociatedObj) == CMSRET_SUCCESS)
   {
      if(iidStack.instance[iidStack.currentDepth-1] == AssociatedDeviceIndex) 
      {
         found = 1;
         errorinfo |= OutputCharValueToAC(ac, VAR_AssociatedDeviceMACAddress, wirelessAssociatedObj->associatedDeviceMACAddress);
         errorinfo |= OutputCharValueToAC(ac, VAR_AssociatedDeviceIPAddress, wirelessAssociatedObj->associatedDeviceIPAddress);
         if(wirelessAssociatedObj->associatedDeviceAuthenticationState)
         {
            errorinfo |= OutputCharValueToAC(ac, VAR_AssociatedDeviceAuthenticationState, "1");
         }
         else
         {
            errorinfo |= OutputCharValueToAC(ac, VAR_AssociatedDeviceAuthenticationState, "0");
         }
      }                 
      cmsObj_free((void **) &wirelessAssociatedObj);
   }
   
   if(!found)
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

int GetSpecificAssociatedDeviceInfo(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack parentIidStack = EMPTY_INSTANCE_ID_STACK;

   LanWlanAssociatedDeviceEntryObject *wirelessAssociatedObj=NULL;

   int found = 0;
   int errorinfo = 0;
   char AssociatedDeviceMACAddress[24];
   
   struct Param *pParams;

   int instanceOfLANDevice;
   int instanceOfService;

   instanceOfLANDevice = psvc->device->instance;
   instanceOfService = psvc->instance;

   PUSH_INSTANCE_ID(&parentIidStack, instanceOfLANDevice);
   PUSH_INSTANCE_ID(&parentIidStack, instanceOfService);

   pParams = findActionParamByRelatedVar(ac,VAR_AssociatedDeviceMACAddress);
   if (pParams != NULL)
   {
      strcpy(AssociatedDeviceMACAddress, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   while (cmsObj_getNextInSubTree(MDMOID_LAN_WLAN_ASSOCIATED_DEVICE_ENTRY, &parentIidStack, &iidStack,
                                  (void **)&wirelessAssociatedObj) == CMSRET_SUCCESS)
   {
      if(0 == strcmp(AssociatedDeviceMACAddress, wirelessAssociatedObj->associatedDeviceMACAddress)) 
      {
         found = 1;
         errorinfo |= OutputCharValueToAC(ac, VAR_AssociatedDeviceIPAddress, wirelessAssociatedObj->associatedDeviceIPAddress);
         if(wirelessAssociatedObj->associatedDeviceAuthenticationState)
         {
            errorinfo |= OutputCharValueToAC(ac, VAR_AssociatedDeviceAuthenticationState, "1");
         }
         else
         {
            errorinfo |= OutputCharValueToAC(ac, VAR_AssociatedDeviceAuthenticationState, "0");
         }
      }                 
      cmsObj_free((void **) &wirelessAssociatedObj);
   }
  
   if(!found)
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


int GetBasBeaconSecurityProperties(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
#ifdef FOLLOW_TR098                  
   LanWlanObject *wirelessObj=NULL;
#endif
   int errorinfo = 0;
   int instanceOfLANDevice;
   int instanceOfService;

   instanceOfLANDevice = psvc->device->instance;
   instanceOfService = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);
#ifdef FOLLOW_TR098
   if (cmsObj_get(MDMOID_LAN_WLAN, &iidStack, 0, (void **) &wirelessObj) == CMSRET_SUCCESS)
   {
      errorinfo |= OutputCharValueToAC(ac, VAR_BasicEncryptionModes, wirelessObj->basicEncryptionModes);
      errorinfo |= OutputCharValueToAC(ac, VAR_BasicAuthenticationMode, wirelessObj->basicAuthenticationMode);
      cmsObj_free((void **) &wirelessObj);   
   }
#else
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);
    _WlVirtIntfCfgObject *wlVirtIntfCfgObj=NULL;
   if (cmsObj_get(MDMOID_WL_VIRT_INTF_CFG, &iidStack, 0, (void **) &wlVirtIntfCfgObj)== CMSRET_SUCCESS)
   {
      
      if(strcmp(wlVirtIntfCfgObj->wlWep, WL_ENABLED)==0)
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_BasicEncryptionModes, "WEPEncryption");
       }
      else
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_BasicEncryptionModes, "None");
      }

      if(strcmp(wlVirtIntfCfgObj->wlAuthMode, WL_AUTH_RADIUS)==0)
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_BasicAuthenticationMode, "EAPAuthentication");
      }
      else
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_BasicAuthenticationMode, "None");
      }

      cmsObj_free((void **) &wlVirtIntfCfgObj);
   }
#endif
   else   
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


int SetBasBeaconSecurityProperties(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
#ifdef FOLLOW_TR098                  
   LanWlanObject *wirelessObj=NULL;
#endif
   CmsRet ret = CMSRET_SUCCESS;

   char BasicEncryptionModes[32];
   char BasicAuthenticationMode[32];
   
   struct Param *pParams;

   int instanceOfLANDevice;
   int instanceOfService;

   instanceOfLANDevice = psvc->device->instance;
   instanceOfService = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);
   
   //BasicEncryptionModes
   pParams = findActionParamByRelatedVar(ac,VAR_BasicEncryptionModes);
   if (pParams != NULL)
   {
      strcpy(BasicEncryptionModes, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   //BasicAuthenticationMode
   pParams = findActionParamByRelatedVar(ac,VAR_BasicAuthenticationMode);
   if (pParams != NULL)
   {
      strcpy(BasicAuthenticationMode, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }   
#ifdef FOLLOW_TR098
   if (cmsObj_get(MDMOID_LAN_WLAN, &iidStack, 0, (void **) &wirelessObj) == CMSRET_SUCCESS)
   {
      cmsMem_free(wirelessObj->basicEncryptionModes);
      wirelessObj->basicEncryptionModes = cmsMem_strdup(BasicEncryptionModes); 

      cmsMem_free(wirelessObj->basicAuthenticationMode);
      wirelessObj->basicAuthenticationMode = cmsMem_strdup(BasicAuthenticationMode); 

      ret = cmsObj_set(wirelessObj, &iidStack);

      cmsObj_free((void **) &wirelessObj);   
   }
#else
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);
   _WlVirtIntfCfgObject *wlVirtIntfCfgObj=NULL;
   if (cmsObj_get(MDMOID_WL_VIRT_INTF_CFG, &iidStack, 0, (void **) &wlVirtIntfCfgObj)== CMSRET_SUCCESS)
   {
      if(strcmp(BasicEncryptionModes, "WEPEncryption")==0)
      {
         cmsMem_free(wlVirtIntfCfgObj->wlWep);
         wlVirtIntfCfgObj->wlWep = cmsMem_strdup(WL_ENABLED); 
      }
      else if(strcmp(BasicEncryptionModes, "None")==0)
      {
         cmsMem_free(wlVirtIntfCfgObj->wlWep);
         wlVirtIntfCfgObj->wlWep = cmsMem_strdup(WL_DISABLED); 
      }

      if(strcmp(BasicAuthenticationMode, "EAPAuthentication")==0)
      {
         cmsMem_free(wlVirtIntfCfgObj->wlAuthMode);
         wlVirtIntfCfgObj->wlAuthMode = cmsMem_strdup(WL_AUTH_RADIUS); 
      }

      ret = cmsObj_set(wlVirtIntfCfgObj, &iidStack);

      cmsObj_free((void **) &wlVirtIntfCfgObj);
   }
#endif
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

#ifndef FOLLOW_TR098
   DoNothingJustWorkAroundWirelessNonStandardTR98Support();
#endif

   return TRUE;

}

int GetWPABeaconSecurityProperties(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
#ifdef FOLLOW_TR098                  
   LanWlanObject *wirelessObj=NULL;
#endif
   int errorinfo = 0;
   int instanceOfLANDevice;
   int instanceOfService;

   instanceOfLANDevice = psvc->device->instance;
   instanceOfService = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);
#ifdef FOLLOW_TR098
   if (cmsObj_get(MDMOID_LAN_WLAN, &iidStack, 0, (void **) &wirelessObj) == CMSRET_SUCCESS)
   {
      errorinfo |= OutputCharValueToAC(ac, VAR_WPAEncryptionModes, wirelessObj->WPAEncryptionModes);
      errorinfo |= OutputCharValueToAC(ac, VAR_WPAAuthenticationMode, wirelessObj->WPAAuthenticationMode);
      cmsObj_free((void **) &wirelessObj);   
   }
#else
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);
    _WlVirtIntfCfgObject *wlVirtIntfCfgObj=NULL;
   if (cmsObj_get(MDMOID_WL_VIRT_INTF_CFG, &iidStack, 0, (void **) &wlVirtIntfCfgObj)== CMSRET_SUCCESS)
   {     
      if(strcmp(wlVirtIntfCfgObj->wlAuthMode, WL_AUTH_WPA_PSK)==0)
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_WPAAuthenticationMode, "PSKAuthentication");
      }
      else if(strcmp(wlVirtIntfCfgObj->wlAuthMode, WL_AUTH_WPA)==0)
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_WPAAuthenticationMode, "EAPAuthentication");
      }
      else
      {
         errorinfo |= OutputCharValueToAC(ac, VAR_WPAAuthenticationMode, "PSKAuthentication");
      }

      if(strcmp(wlVirtIntfCfgObj->wlWep, WL_ENABLED)==0)
      {
         if(strcmp(wlVirtIntfCfgObj->wlWpa, TKIP_ONLY)==0)
         {
            errorinfo |= OutputCharValueToAC(ac, VAR_WPAEncryptionModes, "WEPandTKIPEncryption");
         }
         else if(strcmp(wlVirtIntfCfgObj->wlWpa, AES_ONLY)==0)
         {
            errorinfo |= OutputCharValueToAC(ac, VAR_WPAEncryptionModes, "WEPandAESEncryption");
         }
         else if(strcmp(wlVirtIntfCfgObj->wlWpa, TKIP_AND_AES)==0)
         {
            errorinfo |= OutputCharValueToAC(ac, VAR_WPAEncryptionModes, "WEPandTKIPandAESEncryption");
         }
         else
         {
            errorinfo |= OutputCharValueToAC(ac, VAR_WPAEncryptionModes, "WEPEncryption");
         }
      }
      else
      {
         if(strcmp(wlVirtIntfCfgObj->wlWpa, TKIP_ONLY)==0)
         {
            errorinfo |= OutputCharValueToAC(ac, VAR_WPAEncryptionModes, "TKIPEncryption");
         }
         else if(strcmp(wlVirtIntfCfgObj->wlWpa, AES_ONLY)==0)
         {
            errorinfo |= OutputCharValueToAC(ac, VAR_WPAEncryptionModes, "AESEncryption");
         }
         else if(strcmp(wlVirtIntfCfgObj->wlWpa, TKIP_AND_AES)==0)
         {
            errorinfo |= OutputCharValueToAC(ac, VAR_WPAEncryptionModes, "TKIPandAESEncryption");
         }

      }

      cmsObj_free((void **) &wlVirtIntfCfgObj);
   }
#endif
   else
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

int SetWPABeaconSecurityProperties(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
#ifdef FOLLOW_TR098                  
   LanWlanObject *wirelessObj=NULL;
#endif
   CmsRet ret = CMSRET_SUCCESS;

   char WPAEncryptionModes[32];
   char WPAAuthenticationMode[24];
   
   struct Param *pParams;

   int instanceOfLANDevice;
   int instanceOfService;

   instanceOfLANDevice = psvc->device->instance;
   instanceOfService = psvc->instance;

   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);
   
   //BasicEncryptionModes
   pParams = findActionParamByRelatedVar(ac,VAR_WPAEncryptionModes);
   if (pParams != NULL)
   {
      strcpy(WPAEncryptionModes, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   //BasicAuthenticationMode
   pParams = findActionParamByRelatedVar(ac,VAR_WPAAuthenticationMode);
   if (pParams != NULL)
   {
      strcpy(WPAAuthenticationMode, pParams->value);
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }   
#ifdef FOLLOW_TR098
   if (cmsObj_get(MDMOID_LAN_WLAN, &iidStack, 0,(void **) &wirelessObj) == CMSRET_SUCCESS)
   {
      cmsMem_free(wirelessObj->WPAEncryptionModes);
      wirelessObj->WPAEncryptionModes = cmsMem_strdup(WPAEncryptionModes); 

      cmsMem_free(wirelessObj->WPAAuthenticationMode);
      wirelessObj->WPAAuthenticationMode = cmsMem_strdup(WPAAuthenticationMode); 

      ret = cmsObj_set(wirelessObj, &iidStack);

      cmsObj_free((void **) &wirelessObj);   
   }
#else
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);
   _WlVirtIntfCfgObject *wlVirtIntfCfgObj=NULL;
   if (cmsObj_get(MDMOID_WL_VIRT_INTF_CFG, &iidStack, 0, (void **) &wlVirtIntfCfgObj)== CMSRET_SUCCESS)
   {
      if(strcmp(WPAEncryptionModes, "WEPEncryption")==0)
      {
         cmsMem_free(wlVirtIntfCfgObj->wlWep);
         wlVirtIntfCfgObj->wlWep = cmsMem_strdup(WL_ENABLED); 
      }
      else if(strcmp(WPAEncryptionModes, "TKIPEncryption")==0)
      {
         cmsMem_free(wlVirtIntfCfgObj->wlWpa);
         wlVirtIntfCfgObj->wlWpa = cmsMem_strdup(TKIP_ONLY); 
      }
      else if(strcmp(WPAEncryptionModes, "AESEncryption")==0)
      {
         cmsMem_free(wlVirtIntfCfgObj->wlWpa);
         wlVirtIntfCfgObj->wlWpa = cmsMem_strdup(AES_ONLY); 
      }
      else if(strcmp(WPAEncryptionModes, "TKIPandAESEncryption")==0)
      {
         cmsMem_free(wlVirtIntfCfgObj->wlWpa);
         wlVirtIntfCfgObj->wlWpa = cmsMem_strdup(TKIP_AND_AES); 
      }
      else if(strcmp(WPAEncryptionModes, "WEPandTKIPEncryption")==0)
      {
         cmsMem_free(wlVirtIntfCfgObj->wlWep);
         wlVirtIntfCfgObj->wlWep = cmsMem_strdup(WL_ENABLED); 
         cmsMem_free(wlVirtIntfCfgObj->wlWpa);
         wlVirtIntfCfgObj->wlWpa = cmsMem_strdup(TKIP_ONLY); 
      }
      else if(strcmp(WPAEncryptionModes, "WEPandAESEncryption")==0)
      {
         cmsMem_free(wlVirtIntfCfgObj->wlWep);
         wlVirtIntfCfgObj->wlWep = cmsMem_strdup(WL_ENABLED); 
         cmsMem_free(wlVirtIntfCfgObj->wlWpa);
         wlVirtIntfCfgObj->wlWpa = cmsMem_strdup(AES_ONLY); 
      }
      else if(strcmp(WPAEncryptionModes, "WEPandTKIPandAESEncryption")==0)
      {
         cmsMem_free(wlVirtIntfCfgObj->wlWep);
         wlVirtIntfCfgObj->wlWep = cmsMem_strdup(WL_ENABLED); 
         cmsMem_free(wlVirtIntfCfgObj->wlWpa);
         wlVirtIntfCfgObj->wlWpa = cmsMem_strdup(TKIP_AND_AES); 
      }

      if(strcmp(WPAAuthenticationMode, "PSKAuthentication")==0)
      {
         cmsMem_free(wlVirtIntfCfgObj->wlAuthMode);
         wlVirtIntfCfgObj->wlAuthMode = cmsMem_strdup(WL_AUTH_WPA_PSK); 
      }
      else if(strcmp(WPAAuthenticationMode, "EAPAuthentication")==0)
      {
         cmsMem_free(wlVirtIntfCfgObj->wlAuthMode);
         wlVirtIntfCfgObj->wlAuthMode = cmsMem_strdup(WL_AUTH_WPA); 
      }
      ret = cmsObj_set(wlVirtIntfCfgObj, &iidStack);

      cmsObj_free((void **) &wlVirtIntfCfgObj);
   }
#endif
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
  
#ifndef FOLLOW_TR098
   DoNothingJustWorkAroundWirelessNonStandardTR98Support();
#endif

   return TRUE;

}

int GetSecurityKeys(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   int errorinfo = 0;
   int i = 0;
   CmsRet ret;
   static char wepkey[4][128];
   static char PreSharedKey[WL_WPA_PSK_SIZE_MAX];

   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
#ifdef FOLLOW_TR098
   LanWlanWepKeyObject *wlwepKeyObj = NULL;
   LanWlanPreSharedKeyObject *wlPreSharedKeyObj = NULL;
#endif

   int instanceOfLANDevice;
   int instanceOfService;

   instanceOfLANDevice = psvc->device->instance;
   instanceOfService = psvc->instance;

#ifdef FOLLOW_TR098
   for ( i=0; i<4; i++ ) 
   {
      INIT_INSTANCE_ID_STACK(&iidStack);
      PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
      PUSH_INSTANCE_ID(&iidStack, instanceOfService);
      PUSH_INSTANCE_ID(&iidStack, 1); 
      PUSH_INSTANCE_ID(&iidStack, i+1); 

      if ((ret = cmsObj_get(MDMOID_LAN_WLAN_WEP_KEY, &iidStack, 0, (void **) &wlwepKeyObj)) != CMSRET_SUCCESS)
      {
         break;
      }
      if ( wlwepKeyObj->WEPKey!= NULL ) 
      {
         strcpy( wepkey[i], wlwepKeyObj->WEPKey);
      }
      cmsObj_free((void **) &wlwepKeyObj);
   }
#else
   INIT_INSTANCE_ID_STACK(&iidStack);
   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);
   PUSH_INSTANCE_ID(&iidStack, 1); 
   _WlVirtIntfCfgObject *wlVirtIntfCfgObj=NULL;
   int isBit128 = 0;
   if (cmsObj_get(MDMOID_WL_VIRT_INTF_CFG, &iidStack, 0, (void **) &wlVirtIntfCfgObj)== CMSRET_SUCCESS)
   {
      if(wlVirtIntfCfgObj->wlKeyBit==0)
      {
         isBit128 = 1;
      }
      else
      {
         isBit128 = 0;
      }

      cmsObj_free((void **) &wlVirtIntfCfgObj);
   }

   for ( i=0; i<4; i++ ) 
   {
      INIT_INSTANCE_ID_STACK(&iidStack);
      PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
      PUSH_INSTANCE_ID(&iidStack, instanceOfService);
      PUSH_INSTANCE_ID(&iidStack, 1);
      PUSH_INSTANCE_ID(&iidStack, i+1); 

      if(isBit128 == 0)
      {
         _WlKey64CfgObject *wepkey64Obj=NULL;

         if ((ret = cmsObj_get(MDMOID_WL_KEY64_CFG, &iidStack, 0, (void **)&wepkey64Obj)) != CMSRET_SUCCESS)
         {
            break;
         }
         strcpy( wepkey[i], wepkey64Obj->wlKey64);
         
         cmsObj_free((void **) &wepkey64Obj);
      }
      else
      {
         _WlKey128CfgObject *wepkey128Obj=NULL;

         if ((ret = cmsObj_get(MDMOID_WL_KEY128_CFG, &iidStack, 0, (void **) &wepkey128Obj)) != CMSRET_SUCCESS)
         {
            break;
         }

         strcpy( wepkey[i], wepkey128Obj->wlKey128);

         cmsObj_free((void **) &wepkey128Obj);
      }

   }

#endif
   if( ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get current MDMOID_LAN_WLAN_WEP_KEY, ret=%d", ret);
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   else
   {
      INIT_INSTANCE_ID_STACK(&iidStack);
      PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
      PUSH_INSTANCE_ID(&iidStack, instanceOfService);
      PUSH_INSTANCE_ID(&iidStack, 1); 

   /*Wireless PreSharedKey MDM Data model don't follow TR098 Data Modules*/
#ifdef FOLLOW_TR098
      if ((ret = cmsObj_get(MDMOID_LAN_WLAN_PRE_SHARED_KEY, &iidStack, 0, (void **) &wlPreSharedKeyObj)) == CMSRET_SUCCESS)
      {
         if ( wlPreSharedKeyObj->preSharedKey!= NULL ) 
         {
            strcpy( PreSharedKey, wlPreSharedKeyObj->preSharedKey);
         }
         cmsObj_free((void **) &wlPreSharedKeyObj);
      }
#else
      _WlVirtIntfCfgObject *wlVirtIntfCfgObj=NULL;

      if ((ret = cmsObj_get(MDMOID_WL_VIRT_INTF_CFG, &iidStack, 0, (void **) &wlVirtIntfCfgObj)) == CMSRET_SUCCESS)
      {
         if ( wlVirtIntfCfgObj->wlWpaPsk!= NULL ) 
         {
            strcpy( PreSharedKey, wlVirtIntfCfgObj->wlWpaPsk);
         }
         cmsObj_free((void **) &wlVirtIntfCfgObj);
      }
#endif
      else
      {
         cmsLog_error("Could not get current MDMOID_LAN_WLAN_PRE_SHARED_KEY, ret=%d", ret);
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }
   }

   ac->params[0].value = wepkey[0];
   ac->params[1].value = wepkey[1];
   ac->params[2].value = wepkey[2];
   ac->params[3].value = wepkey[3];
   errorinfo |= OutputCharValueToAC(ac, VAR_PreSharedKey, PreSharedKey);
   errorinfo |= OutputCharValueToAC(ac, VAR_KeyPassphrase, "");  

   if(errorinfo)
   {
      cmsLog_error("errorinfo = %d", errorinfo);
      soap_error( uclient, errorinfo );
      return FALSE;
   }
   return TRUE;
}

bool util_ValidateHexNumber(const char *objValue) 
{
    int i = 0, size = 0;
    bool status = FALSE;

    if ( objValue == NULL ) return status;

    size = strlen(objValue);

    for ( i = 0; i < size; i++ ) 
    {
        if ( isxdigit(objValue[i]) == FALSE )
        {
           break;
        }
    }

    if ( size > 0 && i == size )
    {
        status = TRUE;
    }
    return status;
}

int SetSecurityKeys(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   int i = 0;
#ifndef FOLLOW_TR098
   char PreSharedKey[WL_WPA_PSK_SIZE_MAX];
#endif
   char KeyPassPhrase[WL_WPA_PSK_SIZE_MAX];
   struct Param *pParams;
   CmsRet ret;
   int valueSize;

   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
#ifdef FOLLOW_TR098
   LanWlanWepKeyObject *wlwepKeyObj = NULL;
   LanWlanPreSharedKeyObject *wlPreSharedKeyObj = NULL;
#endif

   int instanceOfLANDevice;
   int instanceOfService;

   instanceOfLANDevice = psvc->device->instance;
   instanceOfService = psvc->instance;
   
   for (i=0; i<4;i++)
   {
      valueSize = strlen(ac->params[i].value);
      if (valueSize != WL_KEY128_SIZE_HEX && valueSize != WL_KEY64_SIZE_HEX)
      {
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }      
      if (util_ValidateHexNumber(ac->params[i].value) == FALSE)
      {
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE; 
      }
   }
   pParams = findActionParamByRelatedVar(ac,VAR_KeyPassphrase);
   if (pParams != NULL)
   {
      strcpy(KeyPassPhrase, pParams->value); 
      if(strlen(pParams->value) <= WL_MIN_PSK_LEN || strlen(pParams->value) >= WL_MAX_PSK_LEN )
      {
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE; 
      }
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
#ifdef FOLLOW_TR098
   for ( i=0; i<4; i++ ) 
   {
      INIT_INSTANCE_ID_STACK(&iidStack);
      PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
      PUSH_INSTANCE_ID(&iidStack, instanceOfService);
      PUSH_INSTANCE_ID(&iidStack, i+1); 

      if ((ret = cmsObj_get(MDMOID_LAN_WLAN_WEP_KEY, &iidStack, 0, (void **) &wlwepKeyObj)) != CMSRET_SUCCESS)
      {
         break;
      }

      cmsMem_free(wlwepKeyObj->WEPKey);
      wlwepKeyObj->WEPKey = cmsMem_strdup(ac->params[i].value);  

      ret = cmsObj_set(wlwepKeyObj, &iidStack);

      cmsObj_free((void **) &wlwepKeyObj);

      if ( ret  != CMSRET_SUCCESS )
      {
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }
   }
#else
   for ( i=0; i<4; i++ ) 
   {
      INIT_INSTANCE_ID_STACK(&iidStack);
      PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
      PUSH_INSTANCE_ID(&iidStack, instanceOfService);
      PUSH_INSTANCE_ID(&iidStack, 1); 
      PUSH_INSTANCE_ID(&iidStack, i+1); 

      if(valueSize == WL_KEY64_SIZE_HEX)
      {
         _WlKey64CfgObject *wepkey64Obj=NULL;


         if ((ret = cmsObj_get(MDMOID_WL_KEY64_CFG, &iidStack, 0, (void **) &wepkey64Obj)) != CMSRET_SUCCESS)
         {
            break;
         }


         cmsMem_free(wepkey64Obj->wlKey64);
         wepkey64Obj->wlKey64 = cmsMem_strdup(ac->params[i].value);  
         ret = cmsObj_set(wepkey64Obj, &iidStack);

         cmsObj_free((void **) &wepkey64Obj);
      }
      else
      {
         _WlKey128CfgObject *wepkey128Obj=NULL;


         if ((ret = cmsObj_get(MDMOID_WL_KEY128_CFG, &iidStack, 0, (void **) &wepkey128Obj)) != CMSRET_SUCCESS)
         {

            break;
         }

         cmsMem_free(wepkey128Obj->wlKey128);
         wepkey128Obj->wlKey128 = cmsMem_strdup(ac->params[i].value);  

         ret = cmsObj_set(wepkey128Obj, &iidStack);

         cmsObj_free((void **) &wepkey128Obj);
      }
      if ( ret  != CMSRET_SUCCESS )
      {
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }
   }

#endif
   if( ret != CMSRET_SUCCESS)
   {

      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }
   else
   {
      INIT_INSTANCE_ID_STACK(&iidStack);
      PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
      PUSH_INSTANCE_ID(&iidStack, instanceOfService);
      PUSH_INSTANCE_ID(&iidStack, 1); 

   /*Wireless PreSharedKey MDM Data model don't follow TR098 Data Modules*/
#ifdef FOLLOW_TR098
      if ((ret = cmsObj_get(MDMOID_LAN_WLAN_PRE_SHARED_KEY, &iidStack, 0, (void **) &wlPreSharedKeyObj)) == CMSRET_SUCCESS)
      {
         cmsMem_free(wlPreSharedKeyObj->keyPassphrase);
         wlPreSharedKeyObj->keyPassphrase = cmsMem_strdup(KeyPassPhrase);
         ret = cmsObj_set(wlPreSharedKeyObj, &iidStack);
         cmsObj_free((void **) &wlPreSharedKeyObj);
      }
#else
      _WlVirtIntfCfgObject *wlVirtIntfCfgObj=NULL;

      if ((ret = cmsObj_get(MDMOID_WL_VIRT_INTF_CFG, &iidStack, 0, (void **) &wlVirtIntfCfgObj)) == CMSRET_SUCCESS)
      {
         cmsMem_free(wlVirtIntfCfgObj->wlWpaPsk);
         wlVirtIntfCfgObj->wlWpaPsk = cmsMem_strdup(PreSharedKey);
         ret = cmsObj_set(wlVirtIntfCfgObj, &iidStack);
         cmsObj_free((void **) &wlVirtIntfCfgObj);
      }

#endif
      else
      {
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE;
      }
   }
   if( ret != CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   cmsMgm_saveConfigToFlash();

#ifndef FOLLOW_TR098
   DoNothingJustWorkAroundWirelessNonStandardTR98Support();
#endif

   return TRUE;
}

int GetPreSharedKey(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
   int errorinfo = 0;
   CmsRet ret;

   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
#ifdef FOLLOW_TR098
   LanWlanPreSharedKeyObject *wlPreSharedKeyObj = NULL;
#endif

   int instanceOfLANDevice;
   int instanceOfService;

   instanceOfLANDevice = psvc->device->instance;
   instanceOfService = psvc->instance;

   INIT_INSTANCE_ID_STACK(&iidStack);
   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);
   PUSH_INSTANCE_ID(&iidStack, 1); 

   /*Wireless PreSharedKey MDM Data model don't follow TR098 Data Modules*/
#ifdef FOLLOW_TR098
   if ((ret = cmsObj_get(MDMOID_LAN_WLAN_PRE_SHARED_KEY, &iidStack, 0, (void **) &wlPreSharedKeyObj)) == CMSRET_SUCCESS)
   {
      errorinfo |= OutputCharValueToAC(ac, VAR_PreSharedKey, wlPreSharedKeyObj->preSharedKey);
      errorinfo |= OutputCharValueToAC(ac, VAR_KeyPassphrase, wlPreSharedKeyObj->keyPassphrase);
      cmsObj_free((void **) &wlPreSharedKeyObj);
   }
#else
   _WlVirtIntfCfgObject *wlVirtIntfCfgObj=NULL;

   if ((ret = cmsObj_get(MDMOID_WL_VIRT_INTF_CFG, &iidStack, 0, (void **) &wlVirtIntfCfgObj)) == CMSRET_SUCCESS)
   {
      errorinfo |= OutputCharValueToAC(ac, VAR_PreSharedKey, wlVirtIntfCfgObj->wlWpaPsk);
      cmsObj_free((void **) &wlVirtIntfCfgObj);
   }
#endif
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }


   errorinfo |= OutputCharValueToAC(ac, VAR_PreSharedKeyIndex, "1");   
   errorinfo |= OutputCharValueToAC(ac, VAR_AssociatedDeviceMACAddress, "");   

   if(errorinfo)
   {
      soap_error( uclient, errorinfo );
      return FALSE;
   }
   return TRUE;
}

int SetPreSharedKey(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
#ifndef FOLLOW_TR098
   char PreSharedKey[WL_WPA_PSK_SIZE_MAX];
#endif
   char KeyPassPhrase[WL_WPA_PSK_SIZE_MAX];
   struct Param *pParams;
   CmsRet ret;

   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
#ifdef FOLLOW_TR098
   LanWlanPreSharedKeyObject *wlPreSharedKeyObj = NULL;
#endif

   int instanceOfLANDevice;
   int instanceOfService;

   instanceOfLANDevice = psvc->device->instance;
   instanceOfService = psvc->instance;
   

   pParams = findActionParamByRelatedVar(ac,VAR_KeyPassphrase);
   if (pParams != NULL)
   {
      strcpy(KeyPassPhrase, pParams->value); 
      if((strlen(pParams->value) > WL_MAX_PSK_LEN) || (strlen(pParams->value) < WL_MIN_PSK_LEN))
      {
         soap_error( uclient, SOAP_ACTIONFAILED );
         return FALSE; 
      }
   }
   else
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   INIT_INSTANCE_ID_STACK(&iidStack);
   PUSH_INSTANCE_ID(&iidStack, instanceOfLANDevice);
   PUSH_INSTANCE_ID(&iidStack, instanceOfService);
   PUSH_INSTANCE_ID(&iidStack, 1); 

   /*Wireless PreSharedKey MDM Data model don't follow TR098 Data Modules*/
#ifdef FOLLOW_TR098
   if ((ret = cmsObj_get(MDMOID_LAN_WLAN_PRE_SHARED_KEY, &iidStack, 0, (void **) &wlPreSharedKeyObj)) == CMSRET_SUCCESS)
   {

      cmsMem_free(wlPreSharedKeyObj->keyPassphrase);
      wlPreSharedKeyObj->keyPassphrase = cmsMem_strdup(KeyPassPhrase);
      ret = cmsObj_set(wlPreSharedKeyObj, &iidStack);
      cmsObj_free((void **) &wlPreSharedKeyObj);
   }
#else
   _WlVirtIntfCfgObject *wlVirtIntfCfgObj=NULL;

   if ((ret = cmsObj_get(MDMOID_WL_VIRT_INTF_CFG, &iidStack, 0, (void **) &wlVirtIntfCfgObj)) == CMSRET_SUCCESS)
   {

      cmsMem_free(wlVirtIntfCfgObj->wlWpaPsk);
      wlVirtIntfCfgObj->wlWpaPsk = cmsMem_strdup(PreSharedKey);
      ret = cmsObj_set(wlVirtIntfCfgObj, &iidStack);
      cmsObj_free((void **) &wlVirtIntfCfgObj);
   }

#endif
   if(ret != CMSRET_SUCCESS)
   {
      soap_error( uclient, SOAP_ACTIONFAILED );
      return FALSE;
   }

   cmsMgm_saveConfigToFlash();

#ifndef FOLLOW_TR098
   DoNothingJustWorkAroundWirelessNonStandardTR98Support();
#endif

   return TRUE;
}

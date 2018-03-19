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
//  Filename:       lanwlancfgparams.c
//
******************************************************************************/
#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "lanwlancfgparams.h"

char *BasicEncryptionModes_allowedValueList[] = { "None",
                                              "WEPEncryption",
                                              NULL };

char *BasicAuthenticationMode_allowedValueList[] = { "None",
                                              "EAPAuthentication",
                                              NULL };

char *BeaconType_allowedValueList[] = {  "None",
                                         "Basic",
                                         "WPA",
                                         "11i",
                                         "BasicandWPA",
                                         "Basicand11i",
                                         "WPAand11i",
                                         "BasicandWPAand11i",
                                          NULL };
char *WPAEncryptionModes_allowedValueList[] = {  "WEPEncryption",
                                         "TKIPEncryption",
                                         "WEPandTKIPEncryption",
                                         "AESEncryption",
                                         "WEPandAESEncryption",
                                         "TKIPandAESEncryption",
                                         "WEPandTKIPandAESEncryption",
                                          NULL };
char *WPAAuthenticationMode_allowedValueList[] = {  "PSKAuthentication",
                                         "EAPAuthentication",
                                          NULL };

static VarTemplate StateVariables[] = {
    { "Enable", "1", VAR_EVENTED|VAR_BOOL},
    { "Status", "", VAR_EVENTED|VAR_STRING},
    { "BSSID", "", VAR_EVENTED|VAR_STRING},
    { "MaxBitRate", "", VAR_EVENTED|VAR_STRING},
    { "Channel", "", VAR_EVENTED|VAR_ULONG},
    { "SSID", "", VAR_EVENTED|VAR_STRING},
    { "BeaconType", "", VAR_EVENTED|VAR_STRING|VAR_LIST ,(allowedValue) { BeaconType_allowedValueList } },    
    { "MACAddressControlEnabled", "", VAR_EVENTED|VAR_BOOL },    
    { "Standard",  "", VAR_EVENTED|VAR_STRING },
    { "WEPKeyIndex", "", VAR_EVENTED|VAR_ULONG},
    { "KeyPassphrase", "", VAR_EVENTED|VAR_STRING},    
    { "WEPEncryptionLevel", "", VAR_EVENTED|VAR_STRING},
    { "BasicEncryptionModes", "", VAR_EVENTED|VAR_STRING|VAR_LIST ,(allowedValue) { BasicEncryptionModes_allowedValueList } },
    { "BasicAuthenticationMode", "", VAR_EVENTED|VAR_STRING|VAR_LIST ,(allowedValue) { BasicAuthenticationMode_allowedValueList }},
    { "WPAEncryptionModes", "", VAR_EVENTED|VAR_STRING|VAR_LIST ,(allowedValue) { WPAEncryptionModes_allowedValueList }},
    { "WPAAuthenticationMode", "", VAR_EVENTED|VAR_STRING|VAR_LIST ,(allowedValue) { WPAAuthenticationMode_allowedValueList }},  
    { "IEEE11iEncryptionModes", "", VAR_EVENTED|VAR_STRING},    
    { "IEEE11iAuthenticationMode", "", VAR_EVENTED|VAR_STRING},    
    { "PossibleChannels", "", VAR_EVENTED|VAR_STRING},
    { "BasicDataTransmitRates", "", VAR_EVENTED|VAR_STRING},
    { "OperationalDataTransmitRates", "", VAR_EVENTED|VAR_STRING},
    { "PossibleDataTransmitRates", "", VAR_EVENTED|VAR_STRING},
    { "InsecureOOBAccessEnabled", "", VAR_EVENTED|VAR_BOOL},
    { "BeaconAdvertisementEnabled", "", VAR_EVENTED|VAR_BOOL},
    { "RadioEnabled", "", VAR_EVENTED|VAR_BOOL},
    { "AutoRateFallBackEnabled", "", VAR_EVENTED|VAR_BOOL},
    { "LocationDesccription", "", VAR_EVENTED|VAR_STRING},
    { "RegulatoryDomain", "", VAR_EVENTED|VAR_STRING},    
    { "TotalIPSKFailures", "", VAR_EVENTED|VAR_ULONG},    
    { "TotalIntergrityFailures", "", VAR_EVENTED|VAR_ULONG},
    { "ChannelsInUse", "", VAR_EVENTED|VAR_STRING},
    { "DeviceOperationMode", "", VAR_EVENTED|VAR_STRING},
    { "DistanceFromRoot", "", VAR_EVENTED|VAR_ULONG},
    { "PeerBSSID", "", VAR_EVENTED|VAR_STRING},    
    { "AuthenticationServiceMode", "", VAR_EVENTED|VAR_STRING},    
    { "TotalBytesSent", "", VAR_EVENTED|VAR_ULONG},
    { "TotalBytesReceived", "", VAR_EVENTED|VAR_ULONG},
    { "TotalPacketsSent", "", VAR_EVENTED|VAR_ULONG},
    { "TotalPacketsReceived", "", VAR_EVENTED|VAR_ULONG},
    { "TotalAssociations", "", VAR_EVENTED|VAR_ULONG},   
    { "AssociatedDeviceMACAddress", "", VAR_EVENTED|VAR_STRING},   
    { "AssociatedDeviceIPAddress", "", VAR_EVENTED|VAR_STRING},       
    { "AssociatedDeviceAuthenticationState", "", VAR_EVENTED|VAR_BOOL},       
    { "WEPKey", "", VAR_EVENTED|VAR_STRING},
    { "PreSharedKey", "", VAR_EVENTED|VAR_STRING},   
    { "PreSharedKeyIndex", "", VAR_ULONG},
    { NULL }
};

static Action _GetInfo = {
    "GetInfo", GetWlanInfo,
   (Param []) {
        { "NewEnable", VAR_Enable, VAR_OUT},
        { "NewStatus", VAR_Status, VAR_OUT},    
        { "NewMaxBitRate", VAR_MaxBitRate, VAR_OUT},            
        { "NewChannel",  VAR_Channel, VAR_OUT },
        { "NewSSID", VAR_SSID, VAR_OUT },
        { "NewBeaconType", VAR_BeaconType, VAR_OUT },
        { "NewMACAddressControlEnabled", VAR_MACAddressControlEnabled, VAR_OUT },        
        { "NewStandard", VAR_Standard, VAR_OUT },        
        { "NewBSSID", VAR_BSSID, VAR_OUT },
        { "NewBasicEncryptionModes", VAR_BasicEncryptionModes, VAR_OUT },
        { "NewBasicAuthenticationMode", VAR_BasicAuthenticationMode, VAR_OUT },
        { 0 }
    }
};

static Action _GetStatistics = {
    "GetStatistics", GetWlanStatistics,
   (Param []) {
        { "NewTotalBytesSent", VAR_TotalBytesSent, VAR_OUT},
        { "NewTotalBytesReceived", VAR_TotalBytesReceived, VAR_OUT},    
        { "NewTotalPacketsSent", VAR_TotalPacketsSent, VAR_OUT},            
        { "NewTotalPacketsReceived",  VAR_TotalPacketsReceived, VAR_OUT },
        { 0 }
    }
};

static Action _GetBytesStatistics = {
    "GetByteStatistics", GetWlanByteStatistics,
   (Param []) {
        { "NewTotalBytesSent", VAR_TotalBytesSent, VAR_OUT},
        { "NewTotalBytesReceived", VAR_TotalBytesReceived, VAR_OUT},    
        { 0 }
    }
};

static Action _GetPacketStatistics = {
    "GetPacketStatistics", GetWlanPacketStatistics,
   (Param []) {   
        { "NewTotalPacketsSent", VAR_TotalPacketsSent, VAR_OUT},            
        { "NewTotalPacketsReceived",  VAR_TotalPacketsReceived, VAR_OUT },
        { 0 }
    }
};


static Action _SetEnable = {
    "SetEnable", SetEnable,
   (Param []) { 
       {"NewEnable", VAR_Enable, VAR_IN},  
       { 0 }
    }
};

static Action _SetConfig = {
    "SetConfig", SetConfig,
   (Param []) { 
       {"NewMaxBitRate", VAR_MaxBitRate, VAR_IN}, 
       {"NewChannel", VAR_Channel, VAR_IN}, 
       {"NewSSID", VAR_SSID, VAR_IN}, 
       {"NewBeaconType", VAR_BeaconType, VAR_IN}, 
       {"NewMacAddressControlEnabled", VAR_MACAddressControlEnabled, VAR_IN}, 
       {"NewBasicEncryptionModes", VAR_BasicEncryptionModes, VAR_IN}, 
       {"NewBasicAuthenticationMode", VAR_BasicAuthenticationMode, VAR_IN},        
       
       { 0 }
    }
};

static Action _SetSSID = {
    "SetSSID", SetSSID,
   (Param []) { 
       {"NewSSID", VAR_SSID, VAR_IN}, 
       { 0 }
    }
};

static Action _GetSSID = {
    "GetSSID", GetSSID,
   (Param []) { 
       {"NewSSID", VAR_SSID, VAR_OUT}, 
       { 0 }
    }
};

static Action _GetBSSID = {
    "GetBSSID", GetBSSID,
   (Param []) { 
       {"NewSSID", VAR_BSSID, VAR_OUT}, 
       { 0 }
    }
};

static Action _SetChannel = {
    "SetChannel", SetChannel,
   (Param []) { 
       {"NewChannel", VAR_Channel, VAR_IN}, 
       { 0 }
    }
};

static Action _GetChannelInfo = {
    "GetChannelInfo", GetChannelInfo,
   (Param []) { 
       {"NewChannel", VAR_Channel, VAR_OUT}, 
       {"NewPossibleChannels", VAR_PossibleChannels, VAR_OUT},
       { 0 }
    }
};

static Action _GetBeaconType = {
    "GetBeaconType", GetBeaconType,
   (Param []) { 
       {"NewBeaconType", VAR_BeaconType, VAR_OUT}, 
       { 0 }
    }
};

static Action _SetBeaconType  = {
    "SetBeaconType", SetBeaconType,
   (Param []) { 
       {"NewBeaconType", VAR_BeaconType, VAR_IN}, 
       { 0 }
    }
};
/*
static Action _SetBeaconAdvertisement = {
    "SetBeaconAdvertisement", SetBeaconAdvertisement,
   (Param []) { 
       {"NewBeaconAdvertisementEnabled", VAR_BeaconAdvertisementEnabled, VAR_IN}, 
       { 0 }
    }
};

static Action _GetBeaconAdvertisement = {
    "GetBeaconAdvertisement", GetBeaconAdvertisement,
   (Param []) { 
       {"NewBeaconAdvertisementEnabled", VAR_BeaconAdvertisementEnabled, VAR_OUT}, 
       { 0 }
    }
};
*/
static Action _SetRadioMode = {
    "SetRadioMode", SetRadioMode,
   (Param []) { 
       {"NewRadioEnabled", VAR_RadioEnabled, VAR_IN}, 
       { 0 }
    }
};

static Action _GetRadioMode = {
    "GetRadioMode", GetRadioMode,
   (Param []) { 
       {"NewRadioEnabled", VAR_RadioEnabled, VAR_OUT}, 
       { 0 }
    }
};
/*
static Action _SetDataTransmitRates = {
    "SetDataTransmitRates", SetDataTransmitRates,
   (Param []) { 
       {"NewBasicDataTransmitRates", VAR_BasicDataTransmitRates, VAR_IN}, 
       {"NewOperationalDataTransmitRates", VAR_OperationalDataTransmitRates, VAR_IN},
       { 0 }
    }
};

static Action _GetDataTransmitRatesInfo = {
    "GetDataTransmitRatesInfo", GetDataTransmitRatesInfo,
   (Param []) { 
       {"NewBasicDataTransmitRates", VAR_BasicDataTransmitRates, VAR_OUT}, 
       {"NewOperationalDataTransmitRates", VAR_OperationalDataTransmitRates, VAR_OUT},
       {"NewPossibleDataTransmitRates", VAR_PossibleDataTransmitRates, VAR_OUT},       
       { 0 }
    }
};

static Action _SetInsecureOutOfBandAccessMode = {
    "SetInsecureOutOfBandAccessMode", SetInsecureOutOfBandAccessMode,
   (Param []) { 
       {"NewInsecureOOBAccessEnabled", VAR_InsecureOOBAccessEnabled, VAR_IN}, 
       { 0 }
    }
};

static Action _GetInsecureOutOfBandAccessMode = {
    "GetInsecureOutOfBandAccessMode", GetInsecureOutOfBandAccessMode,
   (Param []) { 
       {"NewInsecureOOBAccessEnabled", VAR_InsecureOOBAccessEnabled, VAR_OUT}, 
       { 0 }
    }
};
*/
static Action _GetTotalAssociations = {
    "GetTotalAssociations", GetTotalAssociations,
   (Param []) { 
       {"NewTotalAssociations", VAR_TotalAssociations, VAR_OUT}, 
       { 0 }
    }
};

static Action _GetGenericAssociatedDeviceInfo = {
    "GetGenericAssociatedDeviceInfo", GetGenericAssociatedDeviceInfo,
   (Param []) { 
       {"NewAssociatedDeviceIndex", VAR_TotalAssociations, VAR_IN}, 
       {"NewAssociatedDeviceMACAddress", VAR_AssociatedDeviceMACAddress, VAR_OUT}, 
       {"NewAssociatedDeviceIPAddress", VAR_AssociatedDeviceIPAddress, VAR_OUT}, 
       {"NewAssociatedDeviceAuthenticationState", VAR_AssociatedDeviceAuthenticationState, VAR_OUT}, 
       { 0 }
    }
};

static Action _GetSpecificAssociatedDeviceInfo = {
    "GetSpecificAssociatedDeviceInfo", GetSpecificAssociatedDeviceInfo,
   (Param []) { 
       {"NewAssociatedDeviceMACAddress", VAR_AssociatedDeviceMACAddress, VAR_IN}, 
       {"NewAssociatedDeviceIPAddress", VAR_AssociatedDeviceIPAddress, VAR_OUT}, 
       {"NewAssociatedDeviceAuthenticationState", VAR_AssociatedDeviceAuthenticationState, VAR_OUT}, 
       { 0 }
    }
};

static Action _SetSecurityKeys = {
    "SetSecurityKeys", SetSecurityKeys,
   (Param []) { 
       {"NewWEPKey0", VAR_WEPKey, VAR_IN}, 
       {"NewWEPKey1", VAR_WEPKey, VAR_IN}, 
       {"NewWEPKey2", VAR_WEPKey, VAR_IN}, 
       {"NewWEPKey3", VAR_WEPKey, VAR_IN}, 
       {"NewPreSharedKey", VAR_PreSharedKey, VAR_IN},    
       {"NewKeyPassphrase", VAR_KeyPassphrase, VAR_IN}, 
       { 0 }
    }
};


static Action _GetSecurityKeys = {
    "GetSecurityKeys", GetSecurityKeys,
   (Param []) { 
       {"NewWEPKey0", VAR_WEPKey, VAR_OUT}, 
       {"NewWEPKey1", VAR_WEPKey, VAR_OUT}, 
       {"NewWEPKey2", VAR_WEPKey, VAR_OUT}, 
       {"NewWEPKey3", VAR_WEPKey, VAR_OUT}, 
       {"NewPreSharedKey", VAR_PreSharedKey, VAR_OUT},    
       {"NewKeyPassphrase", VAR_KeyPassphrase, VAR_OUT}, 
       { 0 }
    }
};

static Action _SetDefaultWEPKeyIndex = {
    "SetDefaultWEPKeyIndex", SetDefaultWEPKeyIndex,
   (Param []) { 
       {"NewDefaultWEPKeyIndex", VAR_WEPKeyIndex, VAR_IN}, 
       { 0 }
    }
};

static Action _GetDefaultWEPKeyIndex = {
    "GetDefaultWEPKeyIndex", GetDefaultWEPKeyIndex,
   (Param []) { 
       {"NewDefaultWEPKeyIndex", VAR_WEPKeyIndex, VAR_OUT}, 
       { 0 }
    }
};

static Action _SetBasBeaconSecurityProperties = {
    "SetBasBeaconSecurityProperties", SetBasBeaconSecurityProperties,
   (Param []) { 
       {"NewBasicEncryptionModes", VAR_BasicEncryptionModes, VAR_IN}, 
       {"NewBasicAuthenticationMode", VAR_BasicAuthenticationMode, VAR_IN}, 
       { 0 }
    }
};

static Action _GetBasBeaconSecurityProperties = {
    "GetBasBeaconSecurityProperties", GetBasBeaconSecurityProperties,
   (Param []) { 
       {"NewBasicEncryptionModes", VAR_BasicEncryptionModes, VAR_OUT}, 
       {"NewBasicAuthenticationMode", VAR_BasicAuthenticationMode, VAR_OUT}, 
       { 0 }
    }
};

static Action _SetWPABeaconSecurityProperties = {
    "SetWPABeaconSecurityProperties", SetWPABeaconSecurityProperties,
   (Param []) { 
       {"NewWPAEncryptionModes", VAR_WPAEncryptionModes, VAR_IN}, 
       {"NewWPAAuthenticationMode", VAR_WPAAuthenticationMode, VAR_IN}, 
       { 0 }
    }
};

static Action _GetWPABeaconSecurityProperties = {
    "GetWPABeaconSecurityProperties", GetWPABeaconSecurityProperties,
   (Param []) { 
       {"NewWPAEncryptionModes", VAR_WPAEncryptionModes, VAR_OUT}, 
       {"NewWPAAuthenticationMode", VAR_WPAAuthenticationMode, VAR_OUT}, 
       { 0 }
    }
};

static Action _SetPreSharedKey = {
    "SetPreSharedKey", SetPreSharedKey,
   (Param []) { 
       {"NewPreSharedKeyIndex", VAR_PreSharedKeyIndex, VAR_IN}, 
       {"NewAssociatedDeviceMACAddress", VAR_AssociatedDeviceMACAddress, VAR_IN}, 
       {"NewPreSharedKey", VAR_PreSharedKey, VAR_IN},        
       {"NewKeyPassphrase", VAR_KeyPassphrase, VAR_IN},        
       { 0 }
    }
};

static Action _GetPreSharedKey = {
    "GetPreSharedKey", GetPreSharedKey,
   (Param []) { 
       {"NewPreSharedKeyIndex", VAR_PreSharedKeyIndex, VAR_IN}, 
       {"NewAssociatedDeviceMACAddress", VAR_AssociatedDeviceMACAddress, VAR_OUT}, 
       {"NewPreSharedKey", VAR_PreSharedKey, VAR_OUT},        
       {"NewKeyPassphrase", VAR_KeyPassphrase, VAR_OUT},        
       { 0 }
    }
};

static PAction Actions[] = {
    &_GetInfo,
    &_SetEnable,
    &_SetConfig,
    &_SetSSID,
    &_GetSSID,
    &_GetBSSID,
    &_SetChannel,
    &_GetChannelInfo,
    &_GetBeaconType,
    &_SetBeaconType,
//    &_SetBeaconAdvertisement,
//    &_GetBeaconAdvertisement,
    &_SetRadioMode,
    &_GetRadioMode,
//    &_SetDataTransmitRates,
//    &_GetDataTransmitRatesInfo,
//    &_SetInsecureOutOfBandAccessMode,
//    &_GetInsecureOutOfBandAccessMode,
    &_GetStatistics,
    &_GetBytesStatistics,
    &_GetPacketStatistics,
    &_GetDefaultWEPKeyIndex,
    &_SetDefaultWEPKeyIndex,
    &_GetTotalAssociations,
    &_GetSpecificAssociatedDeviceInfo,
    &_GetGenericAssociatedDeviceInfo,
    &_SetBasBeaconSecurityProperties,
    &_GetBasBeaconSecurityProperties,
    &_SetWPABeaconSecurityProperties,
    &_GetWPABeaconSecurityProperties,
    &_GetSecurityKeys,
    &_SetSecurityKeys,
    &_GetPreSharedKey,
    &_SetPreSharedKey,    
    NULL
};

ServiceTemplate Template_WLANConfig = {
    "WLANConfiguration:",
    NULL,
    NULL,
    NULL,   /* SVCXML */
    ARRAYSIZE(StateVariables)-1, StateVariables,
    Actions,
    0,
    "urn:dslforum-org:serviceId:WLANConfiguration",
    NULL
};


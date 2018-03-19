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
//  Filename:       layer2bridgeparams.c
//
******************************************************************************/
#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "layer2bridgeparams.h"

static VarTemplate StateVariables[] = { 
    { "MaxBridgeEntries", "", VAR_USHORT }, 
    { "MaxFilterEntries", "" ,VAR_USHORT},
    { "MaxMarkingEntries", "", VAR_USHORT},
    { "BridgeNumberOfEntries", "", VAR_USHORT},    
    { "BridgeKey", "", VAR_USHORT},        
    { "BridgeEnable", "", VAR_BOOL},
    { "BridgeStatus", "", VAR_STRING},            
    { "BridgeName", "", VAR_STRING},            
    { "VLANID", "", VAR_USHORT},                
    { "FilterNumberOfEntries", "", VAR_USHORT},
    { "FilterKey", "", VAR_USHORT},
    { "FilterEnable", "", VAR_USHORT},
    { "FilterStatus", "", VAR_BOOL},
    { "FilterBridgeReference", "", VAR_SHORT},                
    { "ExclusivityOrder", "", VAR_USHORT},
    { "FilterInterface", "", VAR_STRING},                
    { "VLANIDFilter", "", VAR_SHORT},
    { "AdmitOnlyVLANTagged", "", VAR_BOOL},
    { "EthertypeFilterList", "", VAR_STRING},                
    { "EthertypeFilterExclude", "", VAR_BOOL},
    { "SourceMACAddressFilterList", "", VAR_STRING},                
    { "SourceMACAddressFilterExclude", "", VAR_BOOL},
    { "DestMACAddressFilterList", "", VAR_STRING},                
    { "DestMACAddressFilterExclude", "", VAR_BOOL},
    { "SourceMACFromVendorClassIDFilter", "", VAR_STRING},                
    { "SourceMACFromVendorClassIDFilterExclude", "", VAR_BOOL},                    
    { "DestMACFromVendorClassIDFilter", "", VAR_STRING},                
    { "DestMACFromVendorClassIDFilterExclude", "", VAR_BOOL},                    
    { "SourceMACFromClientIDFilter", "", VAR_STRING},                
    { "SourceMACFromClientIDFilterExclude", "", VAR_BOOL},                    
    { "DestMACFromClientIDFilter", "", VAR_STRING},                
    { "DestMACFromClientIDFilterExclude", "", VAR_BOOL},                    
    { "SourceMACFromUserClassIDFilter", "", VAR_STRING},                
    { "SourceMACFromUserClassIDFilterExclude", "", VAR_BOOL},                    
    { "DestMACFromUserClassIDFilter", "", VAR_STRING},                
    { "DestMACFromUserClassIDFilterExclude", "", VAR_BOOL},                    
    { "MarkingNumberOfEntries", "", VAR_USHORT},                    
    { "MarkingKey", "", VAR_USHORT},                        
    { "MarkingEnable", "", VAR_BOOL},
    { "MarkingStatus", "", VAR_STRING},                        
    { "MarkingBridgeReference", "", VAR_SHORT},                        
    { "MarkingInterface", "", VAR_STRING},    
    { "VLANIDUntag", "", VAR_BOOL},                        
    { "VLANIDMark", "", VAR_SHORT},                            
    { "EthernetPriorityMark", "", VAR_SHORT},                            
    { "EthernetPriorityOverride", "", VAR_BOOL},
    { "AvailableInterfaceNumberOfEntries", "", VAR_USHORT},                            
    { "AvailableInterfaceKey", "", VAR_USHORT},                            
    { "InterfaceType", "", VAR_STRING},
    { "InterfaceReference", "", VAR_STRING},                            
    { NULL }
};

static Action _GetInfo = { 
   "GetInfo", GetLayer2Bridge_Info,
   (Param []) {
      {"NewMaxBridgeEntries", VAR_MaxBridgeEntries, VAR_OUT},
      {"NewMaxFilterEntries", VAR_MaxFilterEntries, VAR_OUT},
      {"NewMaxMarkingEntries", VAR_MaxMarkingEntries, VAR_OUT},      
      {"NewBridgeNumberOfEntries", VAR_BridgeNumberOfEntries, VAR_OUT},      
      {"NewFilterNumberOfEntries", VAR_FilterNumberOfEntries, VAR_OUT},      
      {"NewMarkingNumberOfEntries", VAR_MarkingNumberOfEntries, VAR_OUT},      
      {"NewAvailableInterfaceNumberOfEntries", VAR_AvailableInterfaceNumberOfEntries, VAR_OUT},      
      { 0 }
   }
};

static Action _AddBridgeEntry = { 
   "AddBridgeEntry", AddBridgeEntry,
   (Param []) {
      {"NewBridgeEnable", VAR_BridgeEnable, VAR_IN},
      {"NewBridgeName", VAR_BridgeName, VAR_IN},
      {"NewVLANID", VAR_VLANID, VAR_IN},      
      {"NewBridgeKey", VAR_BridgeKey, VAR_OUT},
      { 0 }
   }
};

static Action _DeleteBridgeEntry = { 
   "DeleteBridgeEntry", DeleteBridgeEntry,
   (Param []) {
      {"NewBridgeKey", VAR_BridgeKey, VAR_IN},
      { 0 }
   }
};

static Action _GetSpecificBridgeEntry = { 
   "GetSpecificBridgeEntry", GetSpecificBridgeEntry,
   (Param []) {
      {"NewBridgeKey", VAR_BridgeKey, VAR_IN},
      {"NewBridgeEnable", VAR_BridgeEnable, VAR_OUT},
      {"NewBridgeStatus", VAR_BridgeStatus, VAR_OUT},      
      {"NewVLANID", VAR_VLANID, VAR_OUT},      
      { 0 }
   }
};


static Action _GetGenericBridgeEntry = { 
   "GetGenericBridgeEntry", GetGenericBridgeEntry,
   (Param []) {
      {"NewBridgeIndex", VAR_BridgeNumberOfEntries, VAR_IN},   
      {"NewBridgeKey", VAR_BridgeKey, VAR_OUT},
      {"NewBridgeEnable", VAR_BridgeEnable, VAR_OUT},
      {"NewBridgeStatus", VAR_BridgeStatus, VAR_OUT},      
      {"NewVLANID", VAR_VLANID, VAR_OUT},      
      { 0 }
   }
};

static Action _SetBridgeEntryEnable = { 
   "SetBridgeEntryEnable", SetBridgeEntryEnable,
   (Param []) {
      {"NewBridgeKey", VAR_BridgeKey, VAR_IN},
      {"NewBridgeEnable", VAR_BridgeEnable, VAR_IN},
      { 0 }
   }
};

static Action _GetSpecificAvailableInterfaceEntry = { 
   "GetSpecificAvailableInterfaceEntry", GetSpecificAvailableInterfaceEntry,
   (Param []) {
      {"NewAvailableInterfaceKey", VAR_AvailableInterfaceKey, VAR_IN},
      {"NewInterfaceType", VAR_InterfaceType, VAR_OUT},
      {"NewInterfaceReference", VAR_InterfaceReference, VAR_OUT},
      { 0 }
   }
};

static Action _GetGenericAvailableInterfaceEntry = { 
   "GetGenericAvailableInterfaceEntry", GetGenericAvailableInterfaceEntry,
   (Param []) {
      {"NewAvailableInterfaceIndex", VAR_AvailableInterfaceNumberOfEntries, VAR_IN},   
      {"NewAvailableInterfaceKey", VAR_AvailableInterfaceKey, VAR_OUT},
      {"NewInterfaceType", VAR_InterfaceType, VAR_OUT},
      {"NewInterfaceReference", VAR_InterfaceReference, VAR_OUT},
      { 0 }
   }
};

static PAction Actions[] = {
    &_GetInfo,
    &_AddBridgeEntry,
    &_DeleteBridgeEntry,
    &_GetSpecificBridgeEntry,    
    &_GetGenericBridgeEntry,
    &_SetBridgeEntryEnable,
    &_GetSpecificAvailableInterfaceEntry,
    &_GetGenericAvailableInterfaceEntry,    
    NULL
};


ServiceTemplate Template_Layer2Bridging = {
    "Layer2Bridging:1",
    NULL, 
    NULL,
    NULL,   /* SVCXML */
    ARRAYSIZE(StateVariables)-1, 
    StateVariables,
    Actions,
    0,
    NULL,
    NULL
};


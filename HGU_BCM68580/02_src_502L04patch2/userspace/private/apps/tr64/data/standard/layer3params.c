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
//  Filename:       layer3params.c
//
******************************************************************************/
#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "layer3params.h"


static VarTemplate StateVariables[] = { 
   { "DefaultConnectionService", "", VAR_EVENTED|VAR_STRING }, 
    { "ForwardNumerOfEntries", "0" ,VAR_EVENTED|VAR_USHORT},
    { "Enable", "1", VAR_EVENTED|VAR_BOOL},
    { "Status", "Disabled", VAR_EVENTED|VAR_STRING},    
    { "Type", "", VAR_EVENTED|VAR_STRING},            
    { "DestIPAddress",  "", VAR_EVENTED|VAR_STRING },
    { "DestSubnetMask", "", VAR_EVENTED|VAR_STRING },
    { "SourceIPAddress", "", VAR_EVENTED|VAR_STRING },
    { "SourceSubnetMask", "", VAR_EVENTED|VAR_STRING },
    { "GatewayIPAddress", "", VAR_EVENTED|VAR_STRING },
    { "Interface", "", VAR_EVENTED|VAR_STRING },
    { "ForwardingMetric", "", VAR_EVENTED|VAR_LONG },    
    { "MTU", "", VAR_USHORT },
    { NULL }
};

static Action _GetDefaultConnectionService = { 
   "GetDefaultConnectionService", GetDefaultConnectionService,
   (Param []) {
      {"NewDefaultConnectionService", VAR_DefaultConnectionService, VAR_OUT},
      { 0 }
   }
};

static Action _SetDefaultConnectionService = { 
   "SetDefaultConnectionService", SetDefaultConnectionService,
   (Param []) {
      {"NewDefaultConnectionService", VAR_DefaultConnectionService, VAR_IN},
      { 0 }
   }
};
static Action _GetForwardNumberOfEntries = {
    "GetForwardNumberOfEntries", GetForwardNumberOfEntries,
   (Param []) {
       {"NewForwardNumberOfEntries", VAR_ForwardNumberOfEntries, VAR_OUT},
       { 0 }
    }
};

static Action _AddForwardingEntry = {
    "AddForwardingEntry", AddForwardingEntry,
        
   (Param []) {
//       {"NewType", VAR_Type, VAR_IN},
       {"NewDestIPAddress", VAR_DestIPAddress, VAR_IN},
       {"NewDestSubnetMask", VAR_DestSubnetMask, VAR_IN},
//       {"NewSourceIPAddress", VAR_SourceIPAddress, VAR_IN},
//       {"NewSourceSubnetMask", VAR_SourceSubnetMask, VAR_IN},
       {"NewGatewayIPAddress", VAR_GatewayIPAddress, VAR_IN},
       {"NewInterface", VAR_Interface, VAR_IN},
//       {"NewForwardingMetric", VAR_ForwardingMetric, VAR_IN},
//       {"NewMTU", VAR_MTU, VAR_IN},
       { 0 }
    }
};

static Action _DeleteForwardingEntry = {
    "DeleteForwardingEntry", DeleteForwardingEntry,
   (Param []) {
       {"NewDestIPAddress", VAR_DestIPAddress, VAR_IN},
       {"NewDestSubnetMask", VAR_DestSubnetMask, VAR_IN},
//     {"NewSourceIPAddress", VAR_SourceIPAddress, VAR_IN},
//     {"NewSourceSubnetMask", VAR_SourceSubnetMask, VAR_IN},       
       { 0 }
    }
};

static Action _GetSpecificForwardingEntry = {
    "GetSpecificForwardingEntry", GetSpecificForwardingEntry,
   (Param []) {
       {"NewDestIPAddress", VAR_DestIPAddress, VAR_IN},
       {"NewDestSubnetMask", VAR_DestSubnetMask, VAR_IN},
//       {"NewSourceIPAddress", VAR_SourceIPAddress, VAR_IN},
//       {"NewSourceSubnetMask", VAR_SourceSubnetMask, VAR_IN},   
       {"NewGatewayIPAddress", VAR_GatewayIPAddress, VAR_OUT},
       {"NewEnable", VAR_Enable, VAR_OUT},
       {"NewStatus", VAR_Status, VAR_OUT},    
//       {"NewType", VAR_Type, VAR_OUT},
       {"NewInterface", VAR_Interface, VAR_OUT}, 
//       {"NewForwardingMetric", VAR_ForwardingMetric, VAR_OUT},
//       {"NewMTU", VAR_MTU, VAR_OUT},       
       { 0 }
    }
};

static Action _GetGenericForwardingEntry = {
    "GetGenericForwardingEntry", GetGenericForwardingEntry,
   (Param []) {
       {"NewForwardingIndex", VAR_ForwardNumberOfEntries, VAR_IN},
       {"NewEnable", VAR_Enable, VAR_OUT},
       {"NewStatus", VAR_Status, VAR_OUT},    
  //     {"NewType", VAR_Type, VAR_OUT},
       {"NewDestIPAddress", VAR_DestIPAddress, VAR_OUT},
       {"NewDestSubnetMask", VAR_DestSubnetMask, VAR_OUT},
//       {"NewSourceIPAddress", VAR_SourceIPAddress, VAR_OUT},
//       {"NewSourceSubnetMask", VAR_SourceSubnetMask, VAR_OUT},
       {"NewGatewayIPAddress", VAR_GatewayIPAddress, VAR_OUT},
       {"NewInterface", VAR_Interface, VAR_OUT}, 
//       {"NewForwardingMetric", VAR_ForwardingMetric, VAR_OUT},
//       {"NewMTU", VAR_MTU, VAR_OUT},       
       { 0 }
    }
};

static Action _SetForwardingEntryEnable = {
    "SetForwardingEntryEnable", SetForwardingEntryEnable,
   (Param []) {
       {"NewDestIPAddress", VAR_DestIPAddress, VAR_IN},
       {"NewDestSubnetMask", VAR_DestSubnetMask, VAR_IN},
//       {"NewSourceIPAddress", VAR_SourceIPAddress, VAR_IN},
//       {"NewSourceSubnetMask", VAR_SourceSubnetMask, VAR_IN},   
       {"NewEnable", VAR_Enable, VAR_IN},  
       { 0 }
    }
};

static PAction Actions[] = {
    &_GetDefaultConnectionService,
    &_SetDefaultConnectionService,
    &_GetForwardNumberOfEntries,
    &_AddForwardingEntry,
    &_DeleteForwardingEntry,
    &_GetSpecificForwardingEntry,
    &_GetGenericForwardingEntry,
    &_SetForwardingEntryEnable,
    NULL
};


ServiceTemplate Template_Layer3Forwarding = {
    "Layer3Forwarding:1",
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




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
//  Filename:       wanethifcfgparams.c
//
******************************************************************************/
#ifdef INCLUDE_WANETHERNETCONFIG
#ifdef DMP_ETHERNETWAN_1
#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "wanethifcfgparams.h"

char *wanethMaxBitRate_allowedValueList[] = { "10",
                                              "100",
                                              "1000",
                                              "Auto", 
                                              NULL };

static VarTemplate StateVariables[] = { 
   { "Enable", "1", VAR_BOOL }, 
   { "Status", "", VAR_STRING }, 
   { "MACAddress", "", VAR_STRING|VAR_LIST,(allowedValue) { wanethMaxBitRate_allowedValueList } }, 
   { "MaxBitRate", "", VAR_STRING },    
   { "Stats.BytesSent", "", VAR_ULONG },    
   { "Stats.BytesReceived", "", VAR_ULONG },    
   { "Stats.PacketsSent", "", VAR_ULONG },    
   { "Stats.PacketsReceived", "", VAR_ULONG },    
   { 0 } 
};


static Action _SetEnable = {
    "SetEnable", SetETHInterfaceEnable,
   (Param []) {
       {"NewEnable", VAR_Enable, VAR_IN},
       { 0 }
    }
};

static Action _GetInfo = {
    "GetInfo", WANETHInterfaceConfig_GetVar,
   (Param []) {
       {"NewEnable", VAR_Enable, VAR_OUT},
       {"NewStatus", VAR_Status, VAR_OUT},
       {"NewMACAddress", VAR_MACAddress, VAR_OUT},   
       {"NewMaxBitRate", VAR_MaxBitRate, VAR_OUT},   
                                  
       { 0 }
    }
};


static Action _SetMaxBitRate = {
    "SetMaxBitRate", SetMaxBitRate,
   (Param []) {
       {"NewMaxBitRate", VAR_MaxBitRate, VAR_IN},
       { 0 }
    }
};

static Action _GetStatistics = {
    "GetStatistics", GetStatisticsWANETH,
   (Param []) {
       {"NewBytesSent", VAR_BytesSent, VAR_OUT},
       {"NewBytesReceived", VAR_BytesReceived, VAR_OUT},
       {"NewPacketsSent", VAR_PacketsSent, VAR_OUT},
       {"NewPacketsReceived", VAR_PacketsReceived, VAR_OUT},
       { 0 }
    }
};

static PAction Actions[] = {
   &_SetEnable,
   &_GetInfo,
   &_SetMaxBitRate,
   &_GetStatistics,
   NULL
};

ServiceTemplate Template_WANETHInterfaceConfig = {
   "WANETHInterfaceConfig:1",
   NULL,
   NULL,    /* state variable handler */
   NULL,  /* SVCXML */
   ARRAYSIZE(StateVariables)-1, 
   StateVariables,
   Actions,
   0,
   "urn:dslforum-org:serviceId:WANETHInterfaceConfig",
   NULL
};
#endif
#endif


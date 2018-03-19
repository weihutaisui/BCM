/*****************************************************************************
//
// Copyright (c) 2006-2012 Broadcom Corporation
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
//  Filename:       deviceconfigparams.c
//
******************************************************************************/
#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "deviceconfigparams.h"


static VarTemplate StateVariables[] = { 
    { "PersistentData", "", VAR_EVENTED|VAR_STRING },   
    { "ConfigFile", "" ,VAR_EVENTED|VAR_STRING},
    { "A_ARG_TYPE_Status", "ChangesApplied", VAR_EVENTED|VAR_STRING},
#ifdef TEST
    /* DeviceSpy doesn't do VAR_UUID, use string now for testing purpose */
    { "A_ARG_TYPE_UUID", "", VAR_EVENTED|VAR_UUID},    
#else
    { "A_ARG_TYPE_UUID", "", VAR_EVENTED|VAR_STRING},    
#endif
    { NULL }
};


static Action _Configuration_Started = {
    "ConfigurationStarted", ConfigurationStarted,
   (Param []) {
       {"NewSessionId", VAR_AUUID, VAR_IN},
       { 0 }
    }
};

static Action _Configuration_Finished = {
    "ConfigurationFinished", ConfigurationFinished,
   (Param []) {
       {"NewStatus", VAR_AStatus, VAR_OUT},
       { 0 }
    }
};

static Action _SetConfigFile = {
    "SetConfiguration", SetConfigFile,
   (Param []) {
       {"NewConfigFile", VAR_ConfigFile, VAR_IN},
       { 0 }
    }
};

static Action _GetConfigFile = {
    "GetConfiguration", GetConfigFile,
   (Param []) {
       {"NewConfigFile", VAR_ConfigFile, VAR_OUT},
       { 0 }
    }
};

static Action _FactoryReset = {
    "FactoryReset", Tr64FactoryReset,
   (Param []) {
       { 0 }
    }
};

static Action _Reboot = {
    "Reboot", Tr64Reboot,
   (Param []) {
       { 0 }
    }
};


static PAction Actions[] = {
#ifdef LATER
   &_Get_PersistentData,
   &_Set_PeristentData,
   &_Get_Configuration,
   &_Set_Configuration,
#endif
   &_GetConfigFile,
   &_SetConfigFile,
   &_Configuration_Started,
   &_Configuration_Finished,
   &_FactoryReset,
   &_Reboot,
   NULL
};


ServiceTemplate Template_DeviceConfig = {
    "DeviceConfig:1",
    DeviceConfig_Init, 
    DeviceConfig_GetVar,
    NULL,   /* SVCXML */
    ARRAYSIZE(StateVariables)-1, 
    StateVariables,
    Actions,
    0,
    "urn:dslforum-org:service:DeviceConfig",
    NULL
};

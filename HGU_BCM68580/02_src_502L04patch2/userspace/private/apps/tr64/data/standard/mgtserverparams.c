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
//  Filename:       mgtserverparams.c
//
******************************************************************************/
#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "mgtserverparams.h"


static VarTemplate StateVariables[] = { 
    { "URL", "", VAR_EVENTED|VAR_STRING },   
    { "Password", "" ,VAR_EVENTED|VAR_STRING},
    { "PeriodicInformaEnable", "", VAR_EVENTED|VAR_BOOL},
    { "PeriodicInformInterval", "", VAR_EVENTED|VAR_LONG},    
    { "PeriodicInformTime", "", VAR_EVENTED|VAR_STRING},            
    { "ParameterKey",  "", VAR_EVENTED|VAR_STRING },
    { "ParameterHash", "", VAR_EVENTED|VAR_STRING },
    { "ConnectionRequestURL", "", VAR_EVENTED|VAR_STRING },
    { "ConnectionRequestUsername", "", VAR_EVENTED|VAR_STRING },
    { "ConnectionRequestPassword", "", VAR_EVENTED|VAR_STRING },
    { "UpgradesManaged", "", VAR_EVENTED|VAR_BOOL },
    { "KickURL", "", VAR_EVENTED|VAR_STRING },
    { "DownloadProgressURL", "", VAR_EVENTED|VAR_STRING },
    { NULL }
};



static Action _GetIfno = {
    "GetInfo", MgtServer_GetVar,
   (Param []) {
       {"NewURL", VAR_URL, VAR_OUT},
       {"NewPeriodicInformEnable", VAR_PeriodicInformEnable, VAR_OUT},
       {"NewPeriodicInformInterval", VAR_PeriodicInformInterval, VAR_OUT},    
       {"NewPeriodicInformTime", VAR_PeriodicInformTime, VAR_OUT},
       {"NewParameterKey", VAR_ParameterKey, VAR_OUT},
       {"NewParameterHash", VAR_ParameterHash, VAR_OUT},
       {"NewConnectionRequestURL", VAR_ConnectionRequestURL, VAR_OUT},
       {"NewConnectionRequestUsername", VAR_ConnectionRequestUsername, VAR_OUT},
       {"NewUpgradesManaged", VAR_UpgradesManaged, VAR_OUT},
       { 0 }
    }
};

static Action _SetManagementServerURL = {
    "SetManagementServerURL", SetManagementServerURL,
   (Param []) {
       {"NewURL", VAR_URL, VAR_IN},
       { 0 }
    }
};

static Action _SetUpgradesManagement = {
    "SetUpgradesManagement", SetUpgradesManagement,
   (Param []) {
       {"NewUpgradesManaged", VAR_UpgradesManaged, VAR_IN},
       { 0 }
    }
};

static Action _SetManagementServerPassword = {
    "SetManagementServerPassword", SetManagementServerPassword,
   (Param []) {
       {"NewPassword", VAR_Password, VAR_IN},
       { 0 }
    }
};

static Action _SetPeriodicInform = {
    "SetPeriodicInform", SetPeriodicInform,
   (Param []) {
       {"NewPeriodicInformEnable", VAR_PeriodicInformEnable, VAR_IN},
       {"NewPeriodicInformInterval", VAR_PeriodicInformInterval, VAR_IN},
       {"NewPeriodicInformTime", VAR_PeriodicInformTime, VAR_IN},
       { 0 }
    }
};

static Action _SetConnectionRequestAuthentication = {
    "SetConnectionRequestAuthentication", SetConnectionRequestAuthentication,
   (Param []) {
       {"NewConnectionRequestUsername", VAR_ConnectionRequestUsername, VAR_IN},
       {"NewConnectionRequestPassword", VAR_ConnectionRequestPassword, VAR_IN},
       { 0 }
    }
};

static PAction Actions[] = {
    &_GetIfno,
    &_SetManagementServerURL,    
    &_SetManagementServerPassword,
    &_SetUpgradesManagement,
    &_SetPeriodicInform,
    &_SetConnectionRequestAuthentication,
    NULL
};


ServiceTemplate Template_MgtServer = {
    "ManagementServer:1",
    NULL, 
    NULL,
    NULL,   /* SVCXML */
    ARRAYSIZE(StateVariables)-1, 
    StateVariables,
    Actions,
    0,
    "urn:dslforum-org:service:ManagementServer",
    NULL
};


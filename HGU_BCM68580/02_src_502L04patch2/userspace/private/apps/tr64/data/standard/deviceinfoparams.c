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
//  Filename:       deviceinfoparams.c
//
******************************************************************************/
#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "deviceinfoparams.h"


static VarTemplate StateVariables[] = { 
    { "ManufacturerName", "", VAR_EVENTED|VAR_STRING },   
    { "ManufacturerOUI", "" ,VAR_EVENTED|VAR_STRING},
    { "ModelName", "", VAR_EVENTED|VAR_STRING},
    { "Description", "", VAR_EVENTED|VAR_STRING},
    { "ProductClass", "", VAR_EVENTED|VAR_STRING},
    { "SerialNumber", "", VAR_EVENTED|VAR_STRING},
    { "SoftwareVersion",  "", VAR_EVENTED|VAR_STRING },
    { "HardwareVersion", "", VAR_EVENTED|VAR_STRING },
    { "SpecVersion", "", VAR_EVENTED|VAR_STRING },
    { "ProvisioningCode", "", VAR_EVENTED|VAR_STRING },
    { "UpTime", "", VAR_EVENTED|VAR_STRING },
    { "DeviceLog", "", VAR_EVENTED|VAR_STRING },
    { NULL }
};



static Action _GetIfno = {
    "GetInfo", DeviceInfo_GetVar,
   (Param []) {
       {"NewManufacturerName", VAR_Manufacturer, VAR_OUT},
       {"NewManufacturerOUI", VAR_ManufacturerOUI, VAR_OUT},
       {"NewModelName", VAR_ModelName, VAR_OUT},	
       {"NewDescription", VAR_Description, VAR_OUT},	
       {"NewProductClass", VAR_ProductClass, VAR_OUT},
       {"NewSerialNumber", VAR_SerialNumber, VAR_OUT},
       {"NewSoftwareVersion", VAR_SoftwareVersion, VAR_OUT},
       {"NewHardwareVersion", VAR_HardwareVersion, VAR_OUT},
       {"NewSpecVersion", VAR_SpecVersion, VAR_OUT},
       {"NewProvisioningCode", VAR_ProvisioningCode, VAR_OUT},
       {"NewUpTime", VAR_UpTime, VAR_OUT},
       { 0 }
    }
};
static Action _SetProvisioningCode = {
    "SetProvisioningCode", SetProvisioningCode,
   (Param []) {
	   {"NewProvisioningCode", VAR_ProvisioningCode, VAR_IN},
       { 0 }
    }
};

static Action _GetDeviceLog = {
    "GetDeviceLog", GetDeviceLog,
   (Param []) {
	   {"NewDeviceLog", VAR_DeviceLog, VAR_OUT},
       { 0 }
    }
};

static PAction Actions[] = {
    &_GetIfno,
    &_SetProvisioningCode,
    &_GetDeviceLog,
    NULL
};


ServiceTemplate Template_DeviceInfo = {
    "DeviceInfo:1",
    NULL, 
    NULL,
    NULL,   /* SVCXML */
    ARRAYSIZE(StateVariables)-1, 
    StateVariables,
    Actions,
    0,
    "urn:dslforum-org:service:DeviceInfo",
    NULL
};


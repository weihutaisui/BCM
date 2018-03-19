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
//  Filename:       ippingparams.c
//
******************************************************************************/
#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "ippingparams.h"

static VarTemplate StateVariables[] = {
    { "DiagnosticsState", "", VAR_STRING},
    { "Interface", "", VAR_STRING},
    { "Host", "", VAR_STRING},
    { "NumberOfRepetitions", "", VAR_ULONG},
    { "Timeout", "", VAR_ULONG},
    { "DataBlockSize", "", VAR_ULONG},
    { "DSCP", "", VAR_ULONG},    
    { "SuccessCount", "", VAR_ULONG },    
    { "FailureCount",  "", VAR_ULONG },
    { "AverageResponseTime", "", VAR_ULONG},
    { "MinimumResponseTime", "", VAR_ULONG},    
    { "MaximumResponseTime", "", VAR_ULONG},
    { NULL }
};

static Action _GetInfo = {
    "GetInfo", IPPing_GetVar,
   (Param []) {
        { "NewDiagnosticsState", VAR_DiagnosticsState, VAR_OUT},
        { "NewInterface", VAR_Interface, VAR_OUT},    
        { "NewHost", VAR_Host, VAR_OUT},            
        { "NewNumberOfRepetitions",  VAR_NumberOfRepetitions, VAR_OUT },
        { "NewTimeout", VAR_Timeout, VAR_OUT },
        { "NewDataBlockSize", VAR_DataBlockSize, VAR_OUT },
        { "NewDSCP", VAR_DSCP, VAR_OUT },        
        { "NewSuccessCount", VAR_SuccessCount, VAR_OUT },        
        { "NewFailureCount", VAR_FailureCount, VAR_OUT },
        { "NewAverageResponseTime", VAR_AverageResponseTime, VAR_OUT },
        { "NewMinimumResponseTime", VAR_MinimumResponseTime, VAR_OUT },
        { "NewMaximumResponseTime", VAR_MaximumResponseTime, VAR_OUT },
        { 0 }
    }
};

static Action _SetDiagnosticsState = {
    "SetDiagnosticsState", SetDiagnosticsStateIPPing,
   (Param []) { 
       {"NewDiagnosticsState", VAR_DiagnosticsState, VAR_IN},  
       { 0 }
    }
};

static Action _SetInterface = {
    "SetInterface", SetInterfaceIPPing,
   (Param []) { 
       {"NewInterface", VAR_Interface, VAR_IN},       
       { 0 }
    }
};

static Action _SetHost = {
    "SetHost", SetHostIPPing,
   (Param []) { 
       {"NewHost", VAR_Host, VAR_IN}, 
       { 0 }
    }
};

static Action _NumberOfRepetitions = {
    "NumberOfRepetitions", NumberOfRepetitionsIPPing,
   (Param []) { 
       {"NewNumberOfRepetitions", VAR_NumberOfRepetitions, VAR_IN}, 
       { 0 }
    }
};

static Action _SetTimeout = {
    "SetTimeout", SetTimeoutIPPing,
   (Param []) { 
       {"NewTimeout", VAR_Timeout, VAR_IN}, 
       { 0 }
    }
};

static Action _SetDataBlockSize = {
    "SetDataBlockSize", SetDataBlockSizeIPPing,
   (Param []) { 
       {"NewDataBlockSize", VAR_DataBlockSize, VAR_IN}, 
       { 0 }
    }
};


static Action _SetDSCP = {
    "SetDSCP", SetDSCPIPPing,
   (Param []) { 
       {"NewDSCP", VAR_DSCP, VAR_IN}, 
       { 0 }
    }
};

static PAction Actions[] = {
    &_GetInfo,
    &_SetDiagnosticsState,
    &_SetInterface,
    &_SetHost,
    &_NumberOfRepetitions,
    &_SetTimeout,
    &_SetDataBlockSize,
    &_SetDSCP,
    NULL
};

ServiceTemplate Template_IPPingConfig = {
    "IPPingDiagnostics:1",
    NULL,
    NULL,   /* IPPing_GetVar,?? */
    NULL,   /* SVCXML */
    ARRAYSIZE(StateVariables)-1, StateVariables,
    Actions,
    0,
    "urn:dslforum-org:serviceId:IPPingDiagnostics",
    NULL
};


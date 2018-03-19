/*
 *
 * <:copyright-BRCM:2012:proprietary:standard
 * 
 *    Copyright (c) 2012 Broadcom 
 *    All Rights Reserved
 * 
 *  This program is the proprietary software of Broadcom and/or its
 *  licensors, and may only be used, duplicated, modified or distributed pursuant
 *  to the terms and conditions of a separate, written license agreement executed
 *  between you and Broadcom (an "Authorized License").  Except as set forth in
 *  an Authorized License, Broadcom grants no license (express or implied), right
 *  to use, or waiver of any kind with respect to the Software, and Broadcom
 *  expressly reserves all rights in and to the Software and all intellectual
 *  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 *  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 *  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 * 
 *  Except as expressly set forth in the Authorized License,
 * 
 *  1. This program, including its structure, sequence and organization,
 *     constitutes the valuable trade secrets of Broadcom, and you shall use
 *     all reasonable efforts to protect the confidentiality thereof, and to
 *     use this information only in connection with your use of Broadcom
 *     integrated circuit products.
 * 
 *  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
 *     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
 *     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
 *     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
 *     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
 *     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
 *     PERFORMANCE OF THE SOFTWARE.
 * 
 *  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
 *     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
 *     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
 *     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 *     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
 *     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
 *     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
 *     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
 *     LIMITED REMEDY.
 * :>
 *
 * $Id: wancommon.c,v 1.26.20.2 2003/10/31 21:31:35 mthawani Exp $
 */

#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "igd.h"
#include "wancommon.h"
#include "bcmutils.h"

static char *WANAccessType_allowedValueList[] = { "DSL", "POTS", "Cable", "Ethernet", "Other", NULL };
static char *PhysicalLinkStatus_allowedValueList[] = { "Up", "Down", "Initializing", "Unavailable", NULL };

static VarTemplate StateVariables[] = {
    { "WANAccessType", "DSL", VAR_STRING|VAR_LIST,  (allowedValue) { WANAccessType_allowedValueList } },
    { "Layer1UpstreamMaxBitRate", "", VAR_ULONG },
    { "Layer1DownstreamMaxBitRate", "", VAR_ULONG },
    { "PhysicalLinkStatus", "", VAR_EVENTED|VAR_STRING|VAR_LIST,  (allowedValue) { PhysicalLinkStatus_allowedValueList } },
    { "EnabledForInternet", "1", VAR_EVENTED|VAR_BOOL },
    { "TotalBytesSent", "", VAR_ULONG },
    { "TotalBytesReceived", "", VAR_ULONG },
    { "TotalPacketsSent", "", VAR_ULONG },
    { "TotalPacketsReceived", "", VAR_ULONG },
    { NULL }
};

static Action _GetCommonLinkProperties = {
    "GetCommonLinkProperties", GetCommonLinkProperties,
   (Param []) {
       {"NewWANAccessType", VAR_WANAccessType, VAR_OUT},
       {"NewLayer1UpstreamMaxBitRate", VAR_Layer1UpstreamMaxBitRate, VAR_OUT},
       {"NewLayer1DownstreamMaxBitRate", VAR_Layer1DownstreamMaxBitRate, VAR_OUT},
       {"NewPhysicalLinkStatus", VAR_PhysicalLinkStatus, VAR_OUT},
       { 0 }
    }
};

static Action _GetTotalBytesReceived = { 
    "GetTotalBytesReceived", GetTotalBytesReceived,
        (Param []) {
            {"NewTotalBytesReceived", VAR_TotalBytesReceived, VAR_OUT},
            { 0 }
        }
};


static Action _GetTotalBytesSent = { 
    "GetTotalBytesSent", GetTotalBytesSent,
        (Param []) {
            {"NewTotalBytesSent", VAR_TotalBytesSent, VAR_OUT},
            { 0 }
        }
};

static Action _GetTotalPacketsReceived = { 
    "GetTotalPacketsReceived", GetTotalPacketsReceived,
        (Param []) {
            {"NewTotalPacketsReceived", VAR_TotalPacketsReceived, VAR_OUT},
            { 0 }
        }
};

static Action _GetTotalPacketsSent = { 
    "GetTotalPacketsSent", GetTotalPacketsSent,
        (Param []) {
            {"NewTotalPacketsSent", VAR_TotalPacketsSent, VAR_OUT},
            { 0 }
        }
};

static Action _GetEnabledForInternet = { 
    "GetEnabledForInternet", GetEnabledForInternet,
        (Param []) {
            {"NewEnabledForInternet", VAR_EnabledForInternet, VAR_OUT},
            { 0 }
        }
};

static Action _SetEnabledForInternet = { 
   /* enableForInternet is not supported in CMS core lib */
    "SetEnabledForInternet", SetEnabledForInternet,
        (Param []) {
            {"NewEnabledForInternet", VAR_EnabledForInternet, VAR_IN},
            { 0 }
        }
};

static PAction Actions[] = {
    &_GetCommonLinkProperties,
    &_GetTotalBytesSent,
    &_GetTotalBytesReceived,
    &_GetTotalPacketsReceived,
    &_GetTotalPacketsSent,
#if 0
    /* cms lib doesn't support this parameter */
    &_GetEnabledForInternet,
    &_SetEnabledForInternet,
#endif
    NULL
};

ServiceTemplate Template_WANCommonInterfaceConfig = {
    "WANCommonInterfaceConfig:1",
    NULL,
    WANCommonInterfaceConfig_GetVar,
    NULL,   /* SVCXML */
    ARRAYSIZE(StateVariables)-1, StateVariables,
    Actions,
    0,
    "urn:dslforum-org:serviceId:WANCommonIFC",
    NULL
};


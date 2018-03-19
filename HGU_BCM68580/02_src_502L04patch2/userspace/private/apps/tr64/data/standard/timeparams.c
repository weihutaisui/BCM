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
//  Filename:       timeparams.c
//
******************************************************************************/
#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "timeparams.h"


static VarTemplate StateVariables[] = { 
   { "NTPServer1", "", VAR_STRING }, 
   { "NTPServer2", "", VAR_STRING }, 
   { "NTPServer3", "", VAR_STRING }, 
   { "NTPServer4", "", VAR_STRING }, 
   { "NTPServer5", "", VAR_STRING }, 
   { "CurrentLocalTime", "", VAR_STRING }, 
   { "LocalTimeZone", "", VAR_STRING }, 
   { "LocalTimeZoneName", "", VAR_STRING }, 
   { "DaylightSavingsUsed", "", VAR_BOOL }, 
   { "DaylightSavingsStart", "", VAR_STRING }, 
   { "DaylightSavingsEnd", "", VAR_STRING }, 
   { NULL }
};

static Action _GetIfno = {
    "GetInfo", TimeServer_GetVar,
   (Param []) {
       {"NewNTPServer1", VAR_NTPServer1, VAR_OUT},
       {"NewNTPServer2", VAR_NTPServer2, VAR_OUT},
       {"NewNTPServer3", VAR_NTPServer3, VAR_OUT},
       {"NewNTPServer4", VAR_NTPServer4, VAR_OUT},
       {"NewNTPServer5", VAR_NTPServer5, VAR_OUT},
       {"NewCurrentLocalTime", VAR_CurrentLocalTime, VAR_OUT},
       {"NewLocalTimeZone", VAR_LocalTimeZone, VAR_OUT},    
       {"NewLocalTimeZoneName", VAR_LocalTimeZoneName, VAR_OUT},
       {"NewDaylightSavingsUsed", VAR_DaylightSavingsUsed, VAR_OUT},
       {"NewDaylightSavingStart", VAR_DaylightSavingsStart, VAR_OUT},
       {"NewDaylightSavingEnd", VAR_DaylightSavingsEnd, VAR_OUT},
       { 0 }
    }
};

static Action _SetNTPServers = { 
   "SetNTPServers", SetNTPServers,
   (Param []) {
      {"NewNTPServer1", VAR_NTPServer1, VAR_IN},
      {"NewNTPServer2", VAR_NTPServer2, VAR_IN},
      {"NewNTPServer3", VAR_NTPServer3, VAR_IN},
      {"NewNTPServer4", VAR_NTPServer4, VAR_IN},
      {"NewNTPServer5", VAR_NTPServer5, VAR_IN},

      { 0 }
   }
};

static Action _SetLocalTimeZone = { 
   "SetLocalTimeZone", SetLocalTimeZone,
   (Param []) {
      {"NewLocalTimeZone", VAR_LocalTimeZone, VAR_IN},
      {"NewLocalTimeZoneName", VAR_LocalTimeZoneName, VAR_IN},
      {"NewDaylightSavingsUsed", VAR_DaylightSavingsUsed, VAR_IN},
      {"NewDaylightSavingsStart", VAR_DaylightSavingsStart, VAR_IN},
      {"NewDaylightSavingsEnd", VAR_DaylightSavingsEnd, VAR_IN},
      { 0 }
   }
};

static PAction Actions[] = {
    &_GetIfno,
    &_SetNTPServers,
    &_SetLocalTimeZone,
    NULL
};

ServiceTemplate Template_TimeServer = {
    "Time:1",
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

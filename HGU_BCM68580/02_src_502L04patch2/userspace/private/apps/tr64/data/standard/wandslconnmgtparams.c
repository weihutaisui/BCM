
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
//  Filename:       wandslconnmgtparams.c
//
******************************************************************************/
#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "wandslconnmgtparams.h"

char *PossibleLinkTypesDslConnMgmt_allowedValueList[] = { "EoA(RFC2684B)",
                                               "IPoA(RFC2684R)",
                                               "PPPoA",
                                               "PPPoE",
                                               "CIP",
                                               "Unconfigured",
                                               NULL };

char *PossibleConnectionTypesDslConnMgmt_allowedValueList[] = { "IP_Routed",
                                               "IP_Bridged",
                                               "Unconfigured",
                                               NULL };


static VarTemplate StateVariables[] = {
   { "WANConnectionDevice", "", VAR_STRING },
   { "WANConnectionService", "", VAR_STRING },       
   { "DestinationAddress", "", VAR_STRING }, 
   { "LinkType", "", VAR_STRING|VAR_LIST, (allowedValue) { PossibleLinkTypesDslConnMgmt_allowedValueList } }, 
   { "ConnectionType", "", VAR_STRING|VAR_LIST ,(allowedValue) { PossibleConnectionTypesDslConnMgmt_allowedValueList }}, 
   { "Name", "", VAR_STRING }, 
   { "WANConnectionServiceNumberOfEntries", "", VAR_STRING }, 
   { 0 } 
};

static Action _GetWANConnectionServiceNumberOfEntries = { 
   "GetWANConnectionServiceNumberOfEntries", GetWANConnectionServiceNumberOfEntries,
   (Param [])    {
      {"NewWANConnectionServiceNumberOfEntries", VAR_WANConnectionServiceNumberOfEntries, VAR_OUT},
      { 0 }
   }
};

static Action _GetGenericConnectionServiceEntry = { 
   "GetGenericConnectionServiceEntry", GetGenericConnectionServiceEntry,
   (Param [])    {
      {"NewConnectionServiceIndex", VAR_WANConnectionServiceNumberOfEntries, VAR_IN},
      {"NewWANConnectionDevice", VAR_WANConnectionDevice, VAR_OUT},
      {"NewWANConnectionService", VAR_WANConnectionService, VAR_OUT},
      {"NewName", VAR_Name, VAR_OUT},
      { 0 }
   }
};

static Action _GetSpecificConnectionServiceEntry = { 
   "GetSpecificConnectionServiceEntry", GetSpecificConnectionServiceEntry,
   (Param [])    {
      {"NewWANConnectionService", VAR_WANConnectionService, VAR_IN},
      {"NewWANConnectionDevice", VAR_WANConnectionDevice, VAR_OUT},
      {"NewName", VAR_Name, VAR_OUT},
      { 0 }
   }
};


static Action _AddConnectionDeviceAndService = { 
   "AddConnectionDeviceAndService", AddConnectionDeviceAndService,
   (Param [])    {
      {"NewDestinationAddress", VAR_DestinationAddress, VAR_IN},
      {"NewLinkType", VAR_LinkType, VAR_IN},
      {"NewConnectionType", VAR_ConnectionType, VAR_IN},
      {"NewName", VAR_Name, VAR_IN},
      {"NewWANConnectionDevice", VAR_WANConnectionDevice, VAR_OUT},
      {"NewWANConnectionService", VAR_WANConnectionService, VAR_OUT},
      { 0 }
   }
};

#ifdef LGD_TODO
static Action _AddConnectionService = { 
   "AddConnectionService", AddConnectionService,
   (Param [])    {
      {"NewWANConnectionDevice", VAR_WANConnectionDevice, VAR_IN},
      {"NewConnectionType", VAR_ConnectionType, VAR_IN},
      {"NewName", VAR_Name, VAR_IN},
      {"NewWANConnectionService", VAR_WANConnectionService, VAR_OUT},
      { 0 }
   }
};
#endif

static Action _DeleteConnectionService = { 
   "DeleteConnectionService", DeleteConnectionService,
   (Param [])    {
      {"NewWANConnectionService", VAR_WANConnectionService, VAR_IN},
      { 0 }
   }
};

static PAction Actions[] = {
   &_GetWANConnectionServiceNumberOfEntries,
   &_GetGenericConnectionServiceEntry,
   &_GetSpecificConnectionServiceEntry,
   &_AddConnectionDeviceAndService,
#ifdef LGD_TODO
   &_AddConnectionService,
#endif
   &_DeleteConnectionService,
   NULL
};


ServiceTemplate Template_WANDSLConnMgt = {
   "WANDSLConnectionManagement:1",
   NULL,
   NULL,    /* state variable handler */
   NULL,  /* SVCXML */
   ARRAYSIZE(StateVariables)-1, 
   StateVariables,
   Actions,
   0,
   "urn:dslforum-org:service:WANDSLConnectionManagement",
   NULL
};

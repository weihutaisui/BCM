
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
//  Filename:       wandsllinkconfigparams.c
//
******************************************************************************/
#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "wandsllinkconfig.h"

struct iftab iftable[] = {
   { "nas_", 1 },
   { "ipa_", 2 },
   { "ppp_", 3 },
   { "ppp_", 4 },
   { "nas_", 5 },
   { "unconfigured", 6 },
   { NULL, 0 }
};

char *PossibleLinkTypes_allowedValueList[] = { "EoA(RFC2684B)",
                                               "IPoA(RFC2684R)",
                                               "PPPoA",
                                               "PPPoE",
                                               "CIP",
                                               "Unconfigured",
                                               NULL };

char *LinkStatus_allowedValueList[] = { "Down",
                                        "Up",
                                        "Initializing",
                                        "Unavailable",
                                        NULL };

char *Encapsulation_allowedValueList[] = { "LLC",
                                        "VCMUX",
                                        NULL };

char *ATMQoS_allowedValueList[] = { "UBR",
                                    "CBR",
                                    "GFR",
                                    "VBR-nrt",
                                    "VBR-rt",
                                    "UBR+",
                                    "ABR",
                                     NULL };

static VarTemplate StateVariables[] = {
   { "Enable", "", VAR_BOOL },
   { "LinkStatus", "Unavailable", VAR_EVENTED|VAR_STRING|VAR_LIST,(allowedValue) { LinkStatus_allowedValueList } },       
   { "LinkType", "Unconfigured", VAR_STRING|VAR_LIST, (allowedValue) { PossibleLinkTypes_allowedValueList } }, 
   { "AutoConfig", "1", VAR_EVENTED|VAR_BOOL }, 
   { "ModulationType", "", VAR_STRING }, 
   { "DestinationAddress", "", VAR_STRING }, 
   { "ATMEncapsulation", "", VAR_STRING|VAR_LIST, (allowedValue) { Encapsulation_allowedValueList } }, 
   { "FCSPreserved", "", VAR_BOOL }, 
   { "VCSearchList", "", VAR_STRING },
   { "ATMAAL", "", VAR_STRING }, 
   { "ATMTransmittedBlocks", "", VAR_ULONG }, 
   { "ATMReceivedBlocks", "", VAR_ULONG }, 
   { "ATMQoS", "", VAR_STRING|VAR_LIST, (allowedValue) { ATMQoS_allowedValueList }  }, 
   { "ATMPeakCellRate", "", VAR_ULONG }, 
   { "ATMMaximumBurstSize", "", VAR_STRING }, 
   { "ATMSustainableCellRate", "", VAR_STRING }, 
   { "AAL5CRCErrors", "", VAR_ULONG }, 
   { "ATMCRCErrors", "", VAR_ULONG }, 
   { "ATMHECErrors", "", VAR_ULONG }, 
   { 0 } 
};

static Action _GetInfo = { 
   "GetInfo", WANDSLLinkConfig_GetInfo,
   (Param [])    {
      {"NewEnable", VAR_Enable, VAR_OUT},
      {"NewLinkType", VAR_LinkType, VAR_OUT},          
      {"NewLinkStatus", VAR_LinkStatus, VAR_OUT},      
      {"NewDestinationAddress", VAR_DestinationAddress, VAR_OUT},      
      {"NewATMEncapsulation", VAR_ATMEncapsulation, VAR_OUT},      
      {"NewATMQoS", VAR_ATMQoS, VAR_OUT},      
      {"NewATMPeakCellRate", VAR_ATMPeakCellRate, VAR_OUT},      
      {"NewATMMaximumBurstSize", VAR_ATMMaximumBurstSize, VAR_OUT},      
      {"NewATMSustainableCellRate", VAR_ATMSustainableCellRate, VAR_OUT},      
      { 0 }
   }
};

static Action _SetEnable = { 
   "SetEnable", SetDSLEnable,
   (Param [])    {
      {"NewEnable", VAR_Enable, VAR_IN},
      { 0 }
   }
};


static Action _GetDSLLinkInfo = { 
   "GetDSLLinkInfo", WANDSLLinkConfig_GetDSLLinkInfo,
   (Param [])    {
      {"NewLinkType", VAR_LinkType, VAR_OUT},
      { 0 }
   }
};
/* It is not required in Phase 1
static Action _GetAutoConfig = { 
   "GetAutoConfig", GetAutoConfig,
   (Param [])    {
      {"NewAutoConfig", VAR_AutoConfig, VAR_OUT},
      { 0 }
   }
};

static Action _GetModulationType = { 
   "GetModulationType", GetModulationType,
   (Param [])    {
      {"NewModulationType", VAR_ModulationType, VAR_OUT},
      { 0 }
   }
};
*/
static Action _SetDestinationAddress = { 
   "SetDestinationAddress", SetDestinationAddress,
   (Param [])    {
      {"NewDestinationAddress", VAR_DestinationAddress, VAR_IN},
      { 0 }
   }
};

static Action _GetDestinationAddress = { 
   "GetDestinationAddress", GetDestinationAddress,
   (Param [])    {
      {"NewDestinationAddress", VAR_DestinationAddress, VAR_OUT},
      { 0 }
   }
};

static Action _SetATMEncapsulation = { 
   "SetATMEncapsulation", SetATMEncapsulation,
   (Param [])    {
      {"NewATMEncapsulation", VAR_ATMEncapsulation, VAR_IN},
      { 0 }
   }
};

static Action _GetATMEncapsulation = { 
   "GetATMEncapsulation", GetATMEncapsulation,
   (Param [])    {
      {"NewATMEncapsulation", VAR_ATMEncapsulation, VAR_OUT},
      { 0 }
   }
};

static Action _SetATMQoS = { 
   "SetATMQoS", SetATMQoS,
   (Param [])    {
      {"NewATMQoS", VAR_ATMQoS, VAR_IN},
      {"NewATMPeakCellRate", VAR_ATMPeakCellRate, VAR_IN},
      {"NewATMMaximumBurstSize", VAR_ATMMaximumBurstSize, VAR_IN},
      {"NewATMSustainableCellRate", VAR_ATMSustainableCellRate, VAR_IN},      
      { 0 }
   }
};

static Action _SetDSLLinkType = { 
   "SetDSLLinkType", SetDSLLinkType,
   (Param [])    {
      {"NewLinkType", VAR_LinkType, VAR_IN},
      { 0 }
   }
};

#ifdef LGD_TODO  
static Action _GetStatistics = { 
   "GetStatistics", GetStatisticsWANDSL,
   (Param [])    {
      {"NewATMTransmittedBlocks", VAR_ATMTransmittedBlocks, VAR_OUT},
      {"NewATMReceivedBlocks", VAR_ATMReceivedBlocks, VAR_OUT},
      {"NewAAL5CRCErrors", VAR_AAL5CRCErrors, VAR_OUT},
      {"NewATMCRCErrors", VAR_ATMCRCErrors, VAR_OUT},
      { 0 }
   }
};
#endif

static PAction Actions[] = {
   &_SetEnable,
   &_GetInfo,
   &_GetDSLLinkInfo,
   &_SetDestinationAddress,
   &_GetDestinationAddress,
   &_SetATMEncapsulation,
   &_GetATMEncapsulation,
   &_SetATMQoS,
   &_SetDSLLinkType,

   /*
     Currently the system MDM datastruct don't support the 3 paramters:
     ATMTransmittedBlocks/ATMReceivedBlocks/ATMCRCErrors
   */
#ifdef LGD_TODO  
   &_GetStatistics,
#endif
   NULL
};


ServiceTemplate Template_WANDSLLinkConfig = {
   "WANDSLLinkConfig:1",
   NULL,
   NULL,    /* state variable handler */
   NULL,  /* SVCXML */
   ARRAYSIZE(StateVariables)-1, 
   StateVariables,
   Actions,
   0,
   "urn:dslforum-org:serviceId:WANDSLLinkConfig",
   NULL
};

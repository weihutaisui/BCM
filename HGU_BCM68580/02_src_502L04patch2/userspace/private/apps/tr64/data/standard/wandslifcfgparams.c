
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
//  Filename:       wandslifcfgparams.c
//
******************************************************************************/
#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "wandslifcfgparams.h"


static VarTemplate StateVariables[] = { 
   { "Enable", "1", VAR_BOOL }, 
   { "Status", "", VAR_STRING }, 
   { "UpstreamCurrRate", "", VAR_ULONG }, 
   { "DownstreamCurrRate", "", VAR_ULONG }, 
   { "UpstreamMaxRate", "",VAR_ULONG }, 
   { "DownstreamMaxRate", "", VAR_ULONG }, 
   { "UpstreamNoiseMargin", "", VAR_LONG }, 
   { "DownstreamNoiseMargin", "", VAR_LONG },    
   { "UpstreamAttenuation", "", VAR_LONG },    
   { "DownstreamAttenuation", "", VAR_LONG },    
   { "UpstreamPower", "", VAR_LONG },    
   { "DownstreamPower", "", VAR_LONG },    
   { "ATURVendor", "", VAR_STRING },  
   { "ATURCountry", "", VAR_ULONG },  
   { "ATURANSIStd", "", VAR_ULONG },
   { "ATURANSIRev", "", VAR_ULONG },
   { "ATUCVendor", "", VAR_STRING },  
   { "ATUCCountry", "", VAR_ULONG },  
   { "ATUCANSIStd", "", VAR_ULONG },
   { "ATUCANSIRev", "", VAR_ULONG },   
   { "TotalStart", "", VAR_ULONG },      
   { "ShowTimeStart", "", VAR_ULONG },         
   { "LastShowTimeStart", "", VAR_ULONG },
   { "CurrentDayStart", "", VAR_ULONG },         
   { "QuarterHourStart", "", VAR_ULONG },     

   { "Stats.Total.ReceiveBlocks", "", VAR_ULONG },     
   { "Stats.Total.TransmitBlocks", "", VAR_ULONG },        
   { "Stats.Total.CellDelin", "", VAR_ULONG },        
   { "Stats.Total.LinkRetrain", "", VAR_ULONG },     
   { "Stats.Total.InitErrors", "", VAR_ULONG },        
   { "Stats.Total.InitTimeouts", "", VAR_ULONG },
   { "Stats.Total.LossOfFraming", "", VAR_ULONG },     
   { "Stats.Total.ErroredSecs", "", VAR_ULONG },     
   { "Stats.Total.SeverelyErroredSecs", "", VAR_ULONG },     
   { "Stats.Total.FECErrors", "", VAR_ULONG },     
   { "Stats.Total.ATUCFECErrors", "", VAR_ULONG },     
   { "Stats.Total.HECErrors", "", VAR_ULONG },     
   { "Stats.Total.ATUCHECErrors", "", VAR_ULONG },     
   { "Stats.Total.CRCErrors", "", VAR_ULONG },        
   { "Stats.Total.ATUCCRCErrors", "", VAR_ULONG },     

   { "Stats.ShowTime.ReceiveBlocks", "", VAR_ULONG },     
   { "Stats.ShowTime.TransmitBlocks", "", VAR_ULONG },        
   { "Stats.ShowTime.CellDelin", "", VAR_ULONG },        
   { "Stats.ShowTime.LinkRetrain", "", VAR_ULONG },     
   { "Stats.ShowTime.InitErrors", "", VAR_ULONG },        
   { "Stats.ShowTime.InitTimeouts", "", VAR_ULONG },
   { "Stats.ShowTime.LossOfFraming", "", VAR_ULONG },     
   { "Stats.ShowTime.ErroredSecs", "", VAR_ULONG },     
   { "Stats.ShowTime.SeverelyErroredSecs", "", VAR_ULONG },     
   { "Stats.ShowTime.FECErrors", "", VAR_ULONG },     
   { "Stats.ShowTime.ATUCFECErrors", "", VAR_ULONG },     
   { "Stats.ShowTime.HECErrors", "", VAR_ULONG },     
   { "Stats.ShowTime.ATUCHECErrors", "", VAR_ULONG },     
   { "Stats.ShowTime.CRCErrors", "", VAR_ULONG },        
   { "Stats.ShowTime.ATUCCRCErrors", "", VAR_ULONG },        
   { 0 } 
};

#if 0 //remove this action firstly, set atm to disabled will make cpe hang on 6338 chip 
static Action _SetEnable = {
    "SetEnable", SetDSLInterfaceEnable,
   (Param []) {
       {"NewEnable", VAR_Enable, VAR_IN},
       { 0 }
    }
};
#endif
static Action _GetInfo = {
    "GetInfo", WANDSLInterfaceConfig_GetVar,
   (Param []) {
       {"NewEnable", VAR_Enable, VAR_OUT},
       {"NewStatus", VAR_Status, VAR_OUT},
       {"NewUpstreamCurrRate", VAR_UpstreamCurrRate, VAR_OUT},   
       {"NewDownstreamCurrRate", VAR_DownstreamCurrRate, VAR_OUT},   
       {"NewUpstreamMaxRate", VAR_UpstreamMaxRate, VAR_OUT},   
       {"NewDownstreamMaxRate", VAR_DownstreamMaxRate, VAR_OUT},   
       {"NewUpstreamNoiseMargin", VAR_UpstreamNoiseMargin, VAR_OUT},   
       {"NewDownstreamNoiseMargin", VAR_DownstreamNoiseMargin, VAR_OUT},   
       {"NewUpstreamAttenuation", VAR_UpstreamAttenuation, VAR_OUT},   
       {"NewDownstreamAttenuation", VAR_DownstreamAttenuation, VAR_OUT},   
       {"NewUpstreamPower", VAR_UpstreamPower, VAR_OUT},   
       {"NewDownstreamPower", VAR_DownstreamPower, VAR_OUT},                                   
       {"NewATURVendor", VAR_ATURVendor, VAR_OUT},  
       {"NewATURCountry", VAR_ATURCountry, VAR_OUT},  
       {"NewATUCVendor", VAR_ATUCVendor, VAR_OUT},  
       {"NewATUCCountry", VAR_ATUCCountry, VAR_OUT},  
       {"NewTotalStart", VAR_TotalStart, VAR_OUT},  
       { 0 }
    }
};

static Action _GetStatisticsTotal = {
    "GetStatisticsTotal", GetStatisticsTotal,
   (Param []) {
       {"NewReceiveBlocks", VAR_ReceiveBlocks, VAR_OUT},
       {"NewTransmitBlocks", VAR_TransmitBlocks, VAR_OUT},
       {"NewCellDelin", VAR_CellDelin, VAR_OUT},   
       {"NewLinkRetrain", VAR_LinkRetrain, VAR_OUT},   
       {"NewInitErrors", VAR_InitErrors, VAR_OUT},   
       {"NewInitTimeouts", VAR_InitTimeouts, VAR_OUT},   
       {"NewLossOfFraming", VAR_LossOfFraming, VAR_OUT},   
       {"NewErroredSecs", VAR_ErroredSecs, VAR_OUT},   
       {"NewSeverelyErroredSecs", VAR_SeverelyErroredSecs, VAR_OUT},   
       {"NewFECErrors", VAR_FECErrors, VAR_OUT},   
       {"NewATUCFECErrors", VAR_ATUCFECErrors, VAR_OUT},   
       {"NewHECErrors", VAR_HECErrors, VAR_OUT},  
       {"NewATUCHECErrors", VAR_ATUCHECErrors, VAR_OUT},  
       {"NewCRCErrors", VAR_CRCErrors, VAR_OUT},  
       {"NewATUCCRCErrors", VAR_ATUCCRCErrors, VAR_OUT},  
       { 0 }
    }
};

static Action _GetStatisticsShowTime = {
    "GetStatisticsShowTime", GetStatisticsShowTime,
   (Param []) {
       {"NewReceiveBlocks", VAR_ReceiveBlocks, VAR_OUT},
       {"NewTransmitBlocks", VAR_TransmitBlocks, VAR_OUT},
       {"NewCellDelin", VAR_CellDelin, VAR_OUT},   
       {"NewLinkRetrain", VAR_LinkRetrain, VAR_OUT},   
       {"NewInitErrors", VAR_InitErrors, VAR_OUT},   
       {"NewInitTimeouts", VAR_InitTimeouts, VAR_OUT},   
       {"NewLossOfFraming", VAR_LossOfFraming, VAR_OUT},   
       {"NewErroredSecs", VAR_ErroredSecs, VAR_OUT},   
       {"NewSeverelyErroredSecs", VAR_SeverelyErroredSecs, VAR_OUT},   
       {"NewFECErrors", VAR_FECErrors, VAR_OUT},   
       {"NewATUCFECErrors", VAR_ATUCFECErrors, VAR_OUT},   
       {"NewHECErrors", VAR_HECErrors, VAR_OUT},  
       {"NewATUCHECErrors", VAR_ATUCHECErrors, VAR_OUT},  
       {"NewCRCErrors", VAR_CRCErrors, VAR_OUT},  
       {"NewATUCCRCErrors", VAR_ATUCCRCErrors, VAR_OUT},  
       { 0 }
    }
};

static PAction Actions[] = {
#if 0 //remove this action firstly, set atm to disabled will make cpe hang on 6338 chip 
   &_SetEnable,
#endif
   &_GetInfo,
   &_GetStatisticsTotal,
   &_GetStatisticsShowTime,
   NULL
};

ServiceTemplate Template_WANDSLInterfaceConfig = {
   "WANDSLInterfaceConfig:1",
   NULL,
   NULL,    /* state variable handler */
   NULL,  /* SVCXML */
   ARRAYSIZE(StateVariables)-1, StateVariables,
   Actions,
   0,
   "urn:dslforum-org:serviceId:WANDSLInterfaceConfig",
   NULL
};


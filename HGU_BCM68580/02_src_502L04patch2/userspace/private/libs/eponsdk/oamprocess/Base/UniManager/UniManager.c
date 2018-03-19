/*
* <:copyright-BRCM:2013:proprietary:epon
* 
*    Copyright (c) 2013 Broadcom 
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
*/


////////////////////////////////////////////////////////////////////////////////
/// \file UniManager.c
/// \brief Manages UNI specific Configuration and detection of UNI status
///
///
////////////////////////////////////////////////////////////////////////////////
// generic includes
#include "Build.h"
#include "Stream.h"
#include "Teknovus.h"

// base includes
#include "OntConfigDb.h"
#include "OnuOs.h"
#include "UniConfigDb.h"
#include "UniManager.h"

// interface includes
#ifdef CTC_OAM_BUILD
#include "CtcStats.h"
#include "CtcAlarms.h"
#endif

// app includes
#if MCAST
#include "McastCommon.h"
#endif
#include "OntmTimer.h"

#define LinkDownPollTime    (6000 / TkOsMsPerTick) //6 seconds


UniMgrPrtSt IntPortSt[MAX_UNI_PORTS];

typedef enum
    {
    LinkDownPollStart,
    LinkDownPolling,
    LinkDownPollEnd
    }LinkDownPollState;

typedef enum
    {
    RuleProtProtect,
    RuleProtReserve //as in reserve the space for the protection rule
    } RuleProtType;


////////////////////////////////////////////////////////////////////////////////
/// \brief sets state of Forced Capabilities
///
 // Parameters:
/// \param port     port of which to return Forced Capabilities for
/// \param caps     forced caps to set
///
/// \return
/// state of Forced Capabilities
////////////////////////////////////////////////////////////////////////////////
//extern
void UniMgrApplyForcedCaps(TkOnuEthPort port, EthPortCapability caps)
    {
    if (UniCfgDbGetForceModeCaps(port) != caps)
        {
        UniCfgDbSetForceModeCaps(port,caps);
        IntPortSt[port] = UmRenegotiate;
        }
    }  // UniMgrApplyForcedCaps



////////////////////////////////////////////////////////////////////////////////
/// \brief sets state of length error discard
///
 // Parameters:
/// \param port     port of which to set len err discard for
/// \param mode     enable/disable indicator
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void UniMgrApplyLengthErrorDiscard(TkOnuEthPort port, BOOL mode)
    {
    if (UniCfgDbGetLenError(port) != mode)
        {
        UniCfgDbSetLenError(port,mode);
        IntPortSt[port] = UmRenegotiate;
        }
    }  // UniMgrApplyLengthErrorDiscard


////////////////////////////////////////////////////////////////////////////////
/// \brief Renegotiate port
///
/// Configures
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void UniMgrRestartPort(TkOnuEthPort port)
    {
    IntPortSt[port] = UmRenegotiate;
    }  // UniMgrRestartPort

////////////////////////////////////////////////////////////////////////////////
/// \brief Validates downstream configuration from the UNI perspective
///
 // Parameters:
/// \param cfgPtr     configuration pointer
///
/// \return
/// TRUE if valid, FALSE if invalid
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL UniMgrValidateDownstreamConfig(U8 BULK * cfgPtr)
    {
    Stream cfg;
    StreamInit(&cfg,cfgPtr);
    if (StreamReadU8(&cfg) <= UniCfgDbGetActivePortCount())
        {
        return TRUE;
        }
    return FALSE;
    }  // UniMgrValidateDownstreamConfig


////////////////////////////////////////////////////////////////////////////////
/// \brief returns state of Port
///
 // Parameters:
/// \param port     port of which to return link status for
///
/// \return
/// link status of port
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL UniMgrFindPortStatus(TkOnuEthPort port)
    {    
    return FALSE;
    }




#if STATS_COLLECT
////////////////////////////////////////////////////////////////////////////////
/// \brief Get Stat for port
///
///
/// \return
/// stat value
////////////////////////////////////////////////////////////////////////////////
//extern
U32 UniMgrGetPortStat(U8 port, U16 statId)
    {
    return 0;
    }
#endif


// UniManager.c

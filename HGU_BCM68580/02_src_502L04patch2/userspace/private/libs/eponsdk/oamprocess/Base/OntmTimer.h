/*
* <:copyright-BRCM:2012:proprietary:epon
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
*/
#if !defined(OntmTimer_h)
#define OntmTimer_h
////////////////////////////////////////////////////////////////////////////////
/// \file   OntmTimer.c
/// \brief  Manages Polled timers
///
////////////////////////////////////////////////////////////////////////////////

#include "TkOsTypes.h"
#include "TkOs.h"


#define OntmTimerModuleMsk          0xf000
#define OntmTimerIdMsk              0xff00
#define OntmTimerPayloadMsk         0x00ff

#define OntmTimerMakeId(id, payload)\
    ((OntmTimerId)(((id) & OntmTimerIdMsk) | ((payload) & OntmTimerPayloadMsk)))


typedef enum
    {
    OntmTimerNull,

    OamTimer                     = 0x1000,
    OamTimerMaxRate              = 0x1100,
    OamTimerPortVlanReconfigure  = 0x1200,

    StatsTimer                   = 0x2000,
    StatsTimerPoll               = 0x2100,

    EncryptTimer                 = 0x3000,
    EncryptSecond                = 0x3100,
    EncryptCheck                 = 0x3200,
    EncryptMkaHello              = 0x3300,
    EncryptMkaLifetime           = 0x3400,

    IgmpTimer                    = 0x4000,
    IgmpQueryUp                  = 0x4100,
    IgmpQueryDn                  = 0x4200,

    EapolTimer                   = 0x5000,
    EapolStartTimeout            = 0x5100,

    EnableTrafficTimer           = 0x6000,

    SharedTimer                  = 0x7000,
    Shared100msTimer             = 0x7100,
    Shared1sTimer                = 0x7200,

    RstpTimerPoll                = 0x8000,
    RstpCfgWriteSchedule         = 0x8100,

    PonTimer                     = 0x9000,
    PonAlmHandleTimer            = 0x9100,

    PonHoldoverTimer             = 0xA000,

    OamCtcTimer                  = 0xB000,
    OamCtcPowerSaveTimer         = 0xB100,

    OamDpoeTimer                 = 0xC000,
    OamDpoeResetTimer            = 0xC100,
    OamDpoeTxPowerTimer          = 0xC200,

    OamBcmTimer                  = 0xD000,
    OamBcmResetTimer             = 0xD100,
    
    maxTimer = 0xFFFF
    } OntmTimerId;


////////////////////////////////////////////////////////////////////////////////
/// OntmTimerInit:  Initialize ONTM Timers
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OntmTimerInit (void);

////////////////////////////////////////////////////////////////////////////////
/// TimerActOntm:  Routine to handle ontm timer expiry
///
 // Parameters:
/// \param param    timer ID for the queue
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void TimerActOntm (TkOsTimerActionParam param);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Get the current aggregate PBI time
///
/// \return 32 bit aggregated PBI time
////////////////////////////////////////////////////////////////////////////////
extern
U32 OntmTimerPbiTimeGet(void);


////////////////////////////////////////////////////////////////////////////////
/// OntMgrPollTimer:  Check for ONT Manager timer expiration
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OntMgrPollTimer (void);


////////////////////////////////////////////////////////////////////////////////
/// OntmTimerCreate:  Create a timer
///
 // Parameters:
/// \param time     Time (in ticks) when timer expires
/// \param id       Timer value to return
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OntmTimerCreate (TkOsTimerInterval time, OntmTimerId id);


////////////////////////////////////////////////////////////////////////////////
/// OntmTimerCreateRepeating:  Create a repeating timer
///
 // Parameters:
/// \param time     Time (in ticks) when timer expires
/// \param id       Timer value to return
///
/// \return
///None
////////////////////////////////////////////////////////////////////////////////
extern
void OntmTimerCreateRepeating (TkOsTimerInterval time, OntmTimerId id);


////////////////////////////////////////////////////////////////////////////////
/// OntmTimerDestroy:  Destroy a timer
///
/// Parameters:
/// \param time     Time (in ticks) when timer expires
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void OntmTimerDestroy(OntmTimerId id);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Start a new timer after clearing any old timers with the same ID
///
/// \param id       ID of the timer
/// \param time     Time (in ticks) when timer expires
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OntmTimerRestart(OntmTimerId id, TkOsTimerInterval time);


#endif // OntmTimer.h

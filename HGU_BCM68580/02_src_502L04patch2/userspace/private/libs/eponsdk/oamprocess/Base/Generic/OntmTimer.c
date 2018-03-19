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
////////////////////////////////////////////////////////////////////////////////
/// \file   OntmTimer.c
/// \brief  Manages Polled timers
///
////////////////////////////////////////////////////////////////////////////////

#include "OntmTimer.h"
#include "TkOsTypes.h"

#include "OamIeee.h"
#include "PonMgrEncrypt.h"
#include "UniManager.h"
#include "OptCtrl.h"
#if defined(DPOE_OAM_BUILD)  || defined(BCM_OAM_BUILD)
#include "Stats.h"
#endif
#include "PonManager.h"
#include "OamProcessInit.h"


#define OntmTimerQDepth 16
U8 ontmTimerQSpace[sizeof(TkOsMsgQOverhead) +
                   (OntmTimerQDepth * sizeof(TkOsMsg))];
TkOsMsgQ ontmTimerQ;
static U16 ontTimerbootTime;
static U16 lastTime;
static U32 currentTime;

extern OamHandlerCB *oamVenderHandlerHead;


////////////////////////////////////////////////////////////////////////////////
/// OntmTimerBootTime:  get ONTM boot time
///
 // Parameters:
/// \param None
///
/// \return
/// Boot time
////////////////////////////////////////////////////////////////////////////////
//extern
U16 OntmTimerBootTime (void)
    {
    return ontTimerbootTime;
    } // OntmTimerBootTime


////////////////////////////////////////////////////////////////////////////////
/// OntmTimerInit:  Initialize ONTM Timers
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void OntmTimerInit (void)
    {
    lastTime = 0;
    currentTime = 0;
    ontTimerbootTime = 0;
    ontmTimerQ = TkOsMsgQueueCreate (ontmTimerQSpace, OntmTimerQDepth, NULL);
    OntmTimerCreateRepeating (100, Shared100msTimer);
    OntmTimerCreateRepeating (1000, Shared1sTimer);
    } // OntmTimerInit


////////////////////////////////////////////////////////////////////////////////
/// TimerActOntm:  Routine to handle ontm timer expiry
///
 // Parameters:
/// \param param    timer ID for the queue
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void TimerActOntm (TkOsTimerActionParam param)
    {
    TkOsMsgSend (ontmTimerQ, (TkOsMsg)param);
    } // TimerActOntm


////////////////////////////////////////////////////////////////////////////////
/// AllSharedTimer:  Deal all the shared timer
///
 // Parameters:
/// \param param    timer ID for the queue
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void AllSharedTimer (OntmTimerId timerId)
    {
    switch(timerId)
        {
        case Shared100msTimer:
            OamHandleTimer (OamTimerMaxRate);            
            TIMER_100MS_HANDLE();
            break;

        case Shared1sTimer:
#if defined(DPOE_OAM_BUILD)  || defined(BCM_OAM_BUILD)
            StatsTimerHandle1s();
#endif
            TIMER_1S_HANDLE();

#if INCLUDE_PPPOE
            OnuPPPoETimerHandle();
#endif
            break;

        default:
            break;
        }
    } // AllSharedTimer


////////////////////////////////////////////////////////////////////////////////
/// \brief  Get the current aggregate PBI time
///
/// \return 32 bit aggregated PBI time
////////////////////////////////////////////////////////////////////////////////
//extern
U32 OntmTimerPbiTimeGet(void)
    {
    return currentTime;
    }


////////////////////////////////////////////////////////////////////////////////
/// OntMgrPollTimer:  Check for ONT Manager timer expiration
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void OntMgrPollTimer (void)
    {
    OntmTimerId FAST timerId;

    // just to avoid the warning Suspicous cast in directly
    // casting a pointer to enumeration
    U32 FAST tmp;
    tmp = (U32)TkOsMsgReceive (ontmTimerQ);

    timerId = (OntmTimerId)tmp;
    if (timerId != 0)
        {
        switch (timerId & OntmTimerModuleMsk)
            {
            case SharedTimer:
                AllSharedTimer(timerId);
                break;

            case EncryptTimer :
                EncryptHandleTimer (timerId);
                break;

            case OamCtcTimer:
                POLL_TIMER_HANDLE(OuiCtc, timerId);
                break;

            case OamDpoeTimer:
                POLL_TIMER_HANDLE(OuiDpoe, timerId);

            case OamBcmTimer:
                POLL_TIMER_HANDLE(OuiTeknovus,timerId);

            default :
                break;
            } // switch timer ID
        }

    } // OntMgrPollTimer


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
//extern
void OntmTimerCreate (TkOsTimerInterval time, OntmTimerId id)
    {
    TkOsTimerCreate (time, TimerActOntm, (TkOsTimerActionParam) id);
    } // OntmTimerCreate


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
//extern
void OntmTimerCreateRepeating (TkOsTimerInterval time, OntmTimerId id)
    {
    TkOsTimerCreateRepeating (time, TimerActOntm,
                                    (TkOsTimerActionParam) id);
    } // OntmTimerCreateRepeating

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
void OntmTimerDestroy(OntmTimerId id)
    {
    TkOsTimerDestroyAll((TkOsTimerActionParam)id);
    } // OntmTimerDestroy


////////////////////////////////////////////////////////////////////////////////
/// \brief  Start a new timer after clearing any old timers with the same ID
///
/// \param id       ID of the timer
/// \param time     Time (in ticks) when timer expires
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
void OntmTimerRestart(OntmTimerId id, TkOsTimerInterval time)
    {
    OntmTimerDestroy(id);
    OntmTimerCreate(time, id);
    }


// end of OntmTimer.c

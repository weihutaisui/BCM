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
/// \file   OamCtcPowerSave.c
/// \brief  China Telecom 3.0 power save module implementation
////////////////////////////////////////////////////////////////////////////////

// generic includes
#include <string.h>
#include "CtcOam.h"
#include "bcm_epon_cfg.h"
#include "Oam.h"
#include "Teknovus.h"
#include "MultiByte.h"

// base includes
#include "OntmTimer.h"

// interface includes
#include "OamCtcPowerSave.h"

// app includes
#include "CtcAlarms.h"
#include "PowerSave.h"

typedef struct
    {
    BOOL earlyWake;
    BOOL sleepRx;
    U32 sleepTime;
    U32 wakeTime;
    U16 nextTime;
    MultiByte64 refreshTime;
    } PowerSaveCfg;


static PowerSaveCfg curCfg;


// convert 16ns TQ to PBI ticks (100us)
#define TqToPbi(time)   ((U16)((((time) * 16U) / 1000U) / 100U))
// convert 16ns TQ to timer ticks (5ms)
#define TqToTicks(time) (((((time) * 16U) / 1000U) / 1000U) / TkOsMsPerTick)


////////////////////////////////////////////////////////////////////////////////
/// \brief  Exit power saving mode
///
/// \param reason   Failure code inidcating reason for exiting power save
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcPowerSaveExit(OamCtcSleepFailureCode reason)
    {
    CtcAlmCont cont;

    cont.raised = TRUE;
    cont.almId = OamCtcSleepStatusUpdate;
    cont.inst = 0;
    cont.value = reason;
    (void)CtcAlmMsg(&cont);

    curCfg.sleepTime = 0;
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Do we need to wake early?
///
/// \return TRUE if early wake is needed
////////////////////////////////////////////////////////////////////////////////
static
BOOL OamCtcPowerSaveEarlyWake(void)
    {
    LinkIndex link;

    for (link = 0; link < TkOnuNumTxLlids; link++)
        {
        //TODO: check queues for data? (FifoManager needs to provide access?)
        }

    return FALSE;
    }


////////////////////////////////////////////////////////////////////////////////
//extern
void OamCtcPowerSaveCapGet(OamVarContainer BULK * reply)
    {
    OamCtcTlvPowerSavingCap BULK * cap =
        (OamCtcTlvPowerSavingCap BULK *)reply->value;
    
    //janeqiu: get laser sleep capability from gpio ?
    cap->sleepModeCap = OamCtcSleepModeCapTx_TRx;
    cap->sleepModeCap |= OamCtcSleepModeCapTRx;
    cap->earlyWakeCap = OamCtcEarlyWakeupCapSupport;
    reply->length = sizeof(OamCtcTlvPowerSavingCap);
    }


////////////////////////////////////////////////////////////////////////////////
//extern
void OamCtcPowerSaveCfgGet(OamVarContainer BULK * reply)
    {
    OamCtcTlvPowerSavingCfg BULK * cfg =
        (OamCtcTlvPowerSavingCfg BULK *)reply->value;

    cfg->earlyWake = curCfg.earlyWake ?
        OamCtcEarlyWakeEnable : OamCtcEarlyWakeDisable;
    memcpy(&cfg->maxDuration, &curCfg.refreshTime.u48.val, sizeof(U48));
    reply->length = sizeof(OamCtcTlvPowerSavingCfg);
    }


////////////////////////////////////////////////////////////////////////////////
//extern
void OamCtcPowerSaveCfgSet(const OamVarContainer BULK * src,
                           OamVarContainer BULK * reply)
    {
    OamCtcTlvPowerSavingCfg BULK * cfg =
        (OamCtcTlvPowerSavingCfg BULK *)src->value;

    if (cfg->earlyWake == OamCtcEarlyWakeNotSupport)
        {
        reply->length = OamVarErrActBadParameters;
        return;
        }

    curCfg.earlyWake = (cfg->earlyWake == OamCtcEarlyWakeEnable);
    memcpy(&curCfg.refreshTime.u48.val, &cfg->maxDuration, sizeof(U48));
    }


////////////////////////////////////////////////////////////////////////////////
//extern
void OamCtcSleepCtrlAct(const OamVarContainer BULK * src,
                        OamVarContainer BULK * reply)
    {
    OamCtcTlvActSleepCtrl BULK * sleep =
        (OamCtcTlvActSleepCtrl BULK *)src->value;

    UNUSED(reply);

    curCfg.sleepRx = (sleep->sleepCtrlMode == OamCtcSleepModeTRx);
    curCfg.sleepTime = OAM_NTOHL(sleep->sleepDuration);
    curCfg.wakeTime = OAM_NTOHL(sleep->wakeDuration);
    OntmTimerDestroy(OamCtcPowerSaveTimer);

    switch (sleep->sleepCtrlFlag)
        {
        case OamCtcSleepFlagEnter:
        case OamCtcSleepFlagModify:
            curCfg.nextTime = TkOsSysTick(); 
            OntmTimerCreate(
                (TkOsTimerInterval)TqToTicks(curCfg.refreshTime.u64),
                OamCtcPowerSaveTimer);
            break;

        case OamCtcSleepFlagLeave:
            OamCtcPowerSaveExit(SleepFailDisable);
            break;

        default:
            break;
        }
    }


////////////////////////////////////////////////////////////////////////////////
//extern
void OamCtcPowerSaveTimerExpire(void)
    {
    OamCtcPowerSaveExit(SleepFailTimeout);
    }


////////////////////////////////////////////////////////////////////////////////
//extern
void OamCtcPowerSavePoll(void)
    {
    U32 now = TkOsSysTick(); 

    if (OamCtcPowerSaveEarlyWake())
        {
        PowerSaveWake();
        OamCtcPowerSaveExit(SleepFailEarlyWake);
        }
    else if (U16LessThan(now, curCfg.nextTime) && (curCfg.sleepTime > 0))
        {
        PowerSaveOpts opts = PowerSaveTx;

        if (curCfg.sleepRx)
            {
            opts |= PowerSaveRx;
            }
        if (curCfg.earlyWake)
            {
            opts |= PowerSaveEarlyWake;
            }

        PowerSaveSleep(now + TqToPbi(curCfg.sleepTime), opts);
        curCfg.nextTime += TqToPbi(curCfg.sleepTime + curCfg.wakeTime);
        }
    else
        {
        // don't need to wake up and not time to go to sleep - do nothing
        }
    }


////////////////////////////////////////////////////////////////////////////////
//extern
void OamCtcPowerSaveInit(void)
    {
    memset(&curCfg, 0, sizeof(PowerSaveCfg));
    }


// end of OamCtcPowerSave.c

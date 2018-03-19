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
/// \file OsAstEvt.c
/// \brief Os assert event common processing
///
/// The common processing for os assert event.
///
////////////////////////////////////////////////////////////////////////////////
#include "Build.h"
#include "OsAstEvt.h"
#include "OntConfigDb.h"
#include "OamIeee.h"
#include "OamIeeeEvent.h"
#include "AlarmCodes.h"
#include "TkOamMem.h"

typedef enum
    {
    OsAstDestDefault,
    OsAstDestAllLinks,
    OsAstDestMasterOnly
    } OsAstDest;


const OsAstDest CODE almDest[AlmNumConditions] =
    {
    ////////////////////////////////////////////////////////////////////////////
    /// per-ONT alarms
    ////////////////////////////////////////////////////////////////////////////

    OsAstDestDefault,       // AlmOntPowerLoss
    OsAstDestDefault,       // AlmOntBootInvalid
    OsAstDestDefault,       // AlmOntSelfTestErr
    OsAstDestDefault,       // AlmOntFlashErasing
    OsAstDestDefault,       // AlmOntFirmwareDownload
    OsAstDestDefault,       // AlmOntAuthInfoUnavailable
    OsAstDestDefault,       // AlmOntAuthenticated
    OsAstDestDefault,       // AlmOntGpioLinkFault
    OsAstDestDefault,       // AlmOntGpioCritical
    OsAstDestDefault,       // AlmOntGpioDyingGasp
    OsAstDestDefault,       // AlmOntGpioOther
    OsAstDestDefault,       // AlmOnt1GDnRegistered
    OsAstDestDefault,       // AlmOnt1GDnActivity
    OsAstDestDefault,       // AlmOnt10GDnRegistered
    OsAstDestDefault,       // AlmOnt10GDnActivity
    OsAstDestDefault,       // AlmOnt1GUpRegistered
    OsAstDestDefault,       // AlmOnt1GUpActivity
    OsAstDestDefault,       // AlmOnt10GUpRegistered
    OsAstDestDefault,       // AlmOnt10GUpActivity
    OsAstDestDefault,       // AlmOntSingleLinkReg
    OsAstDestDefault,       // AlmOntMultiLinkReg
    OsAstDestDefault,       // AlmOntFecUp
    OsAstDestDefault,       // AlmOntFecDn
    OsAstDestDefault,       // AlmSff8472Threshold
    OsAstDestDefault,       // AlmBatteryMissing
    OsAstDestDefault,       // AlmBatteryFailure
    OsAstDestDefault,       // AlmBatteryVoltageLow
    OsAstDestDefault,       // AlmOntPonActPri
    OsAstDestDefault,       // AlmOntPonActSec
    OsAstDestDefault,       // AlmOntLearnEvent
    OsAstDestDefault,       // AlmOntWdt
    OsAstDestDefault,       // AlmOntReset
    OsAstDestDefault,       // AlmOntRestore
    OsAstDestDefault,       // AlmOntProtSwitch

    ////////////////////////////////////////////////////////////////////////////
    /// EPON port alarms
    ////////////////////////////////////////////////////////////////////////////

    OsAstDestDefault,       // AlmEponLos
    OsAstDestDefault,       // AlmEponLostSync
    OsAstDestDefault,       // AlmEponStatThreshold
    OsAstDestDefault,       // AlmEponActLed
    OsAstDestDefault,       // AlmEponStandbyLos
    OsAstDestDefault,       // AlmEponLaserShutdownTemp
    OsAstDestDefault,       // AlmEponLaserShutdownPerm
    OsAstDestDefault,       // AlmEponLaserOn
    OsAstDestDefault,       // AlmPwrMonTempHigh   
    OsAstDestDefault,       // AlmPwrMonTempLow      
    OsAstDestDefault,       // AlmPwrMonVccHigh     
    OsAstDestDefault,       // AlmPwrMonVccLow       
    OsAstDestDefault,       // AlmPwrMonTxBiasHigh   
    OsAstDestDefault,       // AlmPwrMonTxBiasLow  
    OsAstDestDefault,       // AlmPwrMonTxPowerHigh  
    OsAstDestDefault,       // AlmPwrMonTxPowerLow  
    OsAstDestDefault,       // AlmPwrMonRxPowerHigh 
    OsAstDestDefault,       // AlmPwrMonRxPowerLow

    ////////////////////////////////////////////////////////////////////////////
    /// Ethernet port alarms
    ////////////////////////////////////////////////////////////////////////////

    OsAstDestDefault,       // AlmPortLearnTblOverflow
    OsAstDestDefault,       // AlmPortLoopback
    OsAstDestDefault,       // AlmPortStatThreshold
    OsAstDestDefault,       // AlmPortLinkDown
    OsAstDestDefault,       // AlmPortBlocked
    OsAstDestDefault,       // AlmPortTraffic
	OsAstDestDefault,       // AlmPortLinkUp
    OsAstDestDefault,       // AlmPortDisabled
	OsAstDestDefault,       // AlmPortAutoNegFailure
    ////////////////////////////////////////////////////////////////////////////
    /// per-LLID alarms
    ////////////////////////////////////////////////////////////////////////////

    OsAstDestDefault,       // AlmLinkNoGates
    OsAstDestDefault,       // AlmLinkUnregistered
    OsAstDestDefault,       // AlmLinkOamTimeout
    OsAstDestDefault,       // AlmLinkOamDiscComplete,
    OsAstDestDefault,       // AlmLinkStatThreshold
    OsAstDestDefault,       // AlmLinkKeyExchange
    OsAstDestDefault,       // AlmLinkLoopback
    OsAstDestDefault,       // AlmLinkAllDown
    OsAstDestDefault,       // AlmLinkDisabled
    OsAstDestMasterOnly,    // AlmLinkRegStart
    OsAstDestMasterOnly,    // AlmLinkRegSucc
    OsAstDestMasterOnly,    // AlmLinkRegFail
    OsAstDestMasterOnly,    // AlmLinkRegTimeOut
    OsAstDestDefault,       // AlmLinkOamCtcDiscComplete,

    ////////////////////////////////////////////////////////////////////////////
    /// Queue alarms
    ////////////////////////////////////////////////////////////////////////////

    OsAstDestDefault,        // AlmQueueStatThreshold

    ////////////////////////////////////////////////////////////////////////////
    /// Other alarms
    ////////////////////////////////////////////////////////////////////////////

    OsAstDestDefault        // AlmOtherGpioInput

    };  // almAllLink


////////////////////////////////////////////////////////////////////////////////
/// \brief  Notify about events
///
/// \param cond     Event idintifier
/// \param inst     Instance of the event
/// \param stat     Associated statistic (if any)
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void OsAstEventNotify(AlmCondition cond, U8 inst, StatId stat)
    {
    LinkIndex link = GetLinkForAlarm(cond, inst);

    // TODO: handle dying gasp when ONU does not terminate OAM
    
    if (TxFrame == NULL)
        {//the tx buffer has not been initialized, just return
        return;
        }

    OamEventPrep(link);
    OamIeeeEventSend(cond, inst, stat);


    // TODO: send other notifications?
    }


////////////////////////////////////////////////////////////////////////////////
//extern
void OsAssertEvent (TkOsAstRec rec)
    {
    U16 FAST idx = OnuOsAssertMap(rec.id);
    U8 FAST first;
    U8 FAST last;

    if (idx < OsAstAlmNums)
        {
        first = rec.inst;
        last = first + 1;

        if (almDest[OnuOsAssertMap(rec.id)] == OsAstDestAllLinks)
            {
            first = 0;
            last = OntCfgDbGetNumUserLinks();
            }

        for (rec.inst = first; rec.inst < last; ++rec.inst)
            {
            if (rec.raise)
                {
                OnuOsAssertSet(rec.id, rec.inst);
                }
            else
                {
                OnuOsAssertClr(rec.id, rec.inst);
                }
            OsAstEventNotify((AlmCondition)idx, rec.inst, (StatId)rec.stat);
            }
        }
    } // OsAssertEvent


// end of OasAstEvt.c

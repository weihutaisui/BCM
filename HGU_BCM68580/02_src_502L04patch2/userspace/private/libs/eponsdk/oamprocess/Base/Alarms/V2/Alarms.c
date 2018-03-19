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
/// \file Alarms.h
/// \brief Alarm conditions and reporting
///
/// The Alarms code handles alarm conditioning, hierarchical masking, and
/// notification of alarm conditions.
///
////////////////////////////////////////////////////////////////////////////////

#include "Teknovus.h"
#include "Alarms.h"
#include "OsAstMsgQ.h"
#include "OsAstEvt.h"
#include "cms_boardioctl.h"
#include "OamOnu.h"

typedef Bool (* OptMonGetFunc)(U16 * value);

AlmMonThd powerMonAlmThreshod[(OamAlmCodePowerMonRxPowerLow - OamAlmCodePowerMonTempHigh) + 1] = { {0, 0, 0} };

Bool bBipErrRaised = FALSE;

static OptMonGetFunc func[] =
    {
    (OptMonGetFunc)optical_temp,
    (OptMonGetFunc)optical_vcc,
    (OptMonGetFunc)optical_bias,
    (OptMonGetFunc)optical_txpower,
    (OptMonGetFunc)optical_rxpower
    };


const OnuAssertId asserts[AlmNumConditions] =
    {
    OsAstAlmOntPowerLoss,
    OsAstAlmOntBootInvalid,
    OsAstAlmOntSelfTestErr,
    OsAstAlmOntFlashBusy,
    OsAstAlmOntSysBusy,
    OsAstAlmOntFirmwareDownload,
    OsAstAlmOntAuthInfoUnvld,
    OsAstAlmOntAuthenticated,
    OsAstAlmOntGpioLinkFault,
    OsAstAlmOntGpioCritical,
    OsAstAlmOntGpioDyingGasp,
    OsAstAlmOntGpioOther,
    OsAstAlmOnt1GDnRegistered,
    OsAstAlmOnt1GDnActivity,
    OsAstAlmOnt10GDnRegistered,
    OsAstAlmOnt10GDnActivity,
    OsAstAlmOnt1GUpRegistered,
    OsAstAlmOnt1GUpActivity,
    OsAstAlmOnt10GUpRegistered,
    OsAstAlmOnt10GUpActivity,
    OsAstAlmOntSingleLinkReg,
    OsAstAlmOntMultiLinkReg,
    OsAstAlmOntFecUp,
    OsAstAlmOntFecDn,
    OsAstAlmOntSff8472Thd,
    OsAstAlmOntBatteryLost,
    OsAstAlmOntBatteryFail,
    OsAstAlmOntBatteryVoltLow,
    OsAstAlmOntPonPriSig,
    OsAstAlmOntPonSecSig,
    OsAstAlmOntLearnEvent,
    OsAstAlmOntWdt,
    OsAstAlmOntReset,
    OsAstAlmOntRestore,
    OsAstAlmOntProtSwitch,
    OsAstAlmOntRogueOnu,

    OsAstAlmEponLos,
    OsAstAlmEponLostSync,
    OsAstAlmEponStatThreshold,
    OsAstAlmEponActLed,
    OsAstAlmEponStandbyLos,
    OsAstAlmEponLaserShutdownTemp,
    OsAstAlmEponLaserShutdownPerm,
    OsAstAlmEponLaserOn,
    OsAstPwrMonTempHigh,
    OsAstPwrMonTempLow,
    OsAstPwrMonVccHigh,
    OsAstPwrMonVccLow,
    OsAstPwrMonTxBiasHigh,
    OsAstPwrMonTxBiasLow,
    OsAstPwrMonTxPowerHigh,
    OsAstPwrMonTxPowerLow,
    OsAstPwrMonRxPowerHigh,
    OsAstPwrMonRxPowerLow,

    OsAstAlmPortLearnTblOverflow,
    OsAstAlmPortLoopback,
    OsAstAlmPortStatThreshold,
    OsAstAlmPortLinkDown,
    OsAstAlmPortBlocked,
    OsAstAlmPortTraffic,
    OsAstAlmPortLinkUp,
    OsAstAlmPortDisabled,
    OsAstAlmPortAutoNegFailure,

    OsAstAlmLinkNoGates,
    OsAstAlmLinkUnregistered,
    OsAstAlmLinkOamTimeout,
    OsAstAlmLinkOamDiscComplete,
    OsAstAlmLinkStatThreshold,
    OsAstAlmLinkKeyExchange,
    OsAstAlmLinkLoopback,
    OsAstAlmLinkAllDown,
    OsAstAlmLinkDisabled,
    OsAstAlmLinkRegStart,
    OsAstAlmLinkRegSucc,
    OsAstAlmLinkRegFail,
    OsAstAlmLinkRegTimeOut,
    OsAstAlmLinkCtcOamDiscComplete,

    OsAstAlmQueueStatThd,

    OsAstAlmGpioInput
    };

typedef struct tagLedCtrl
    {
    U8 inst;
    OnuAssertId assertId;
    BOARD_LED_NAME ledName;
    U8             priority;
    BOARD_LED_STATE state;
    }EponLedCtrl;

typedef struct tagLedState
    {
    U8 priority;
    BOARD_LED_STATE state;
    }EponLedState;

EponLedCtrl ledControl[] = 
    {
    // AlmOntPowerLoss
#ifdef CONFIG_BCM_PON
    {0, OsAstAlmOntPowerLoss,           kLedEpon, 6, kLedStateOff},
    {0, OsAstAlmEponLos,                kLedEpon, 6, kLedStateOff},
    {0, OsAstAlmLinkDisabled,           kLedEpon, 5, kLedStateSlowBlinkContinues},
    {0, OsAstAlmLinkRegStart,           kLedEpon, 2, kLedStateFastBlinkContinues},
    {0, OsAstAlmLinkRegSucc,            kLedEpon, 1, kLedStateOn},
    {0, OsAstAlmLinkRegFail,            kLedEpon, 1, kLedStateSlowBlinkContinues},
#endif
    };

BOARD_LED_STATE ledState[kLedEnd];

void AlarmLedPoll(void)
    {
    U8  i;
    BOARD_LED_NAME led;
    EponLedState  ledToSet[kLedEnd] = { {0} };

    for (i = 0; i < sizeof(ledControl)/sizeof(EponLedCtrl); i++)
        {
        if (OnuOsAssertGet(ledControl[i].assertId, ledControl[i].inst))
            {
            if (ledControl[i].priority > ledToSet[ledControl[i].ledName].priority)
                {
                ledToSet[ledControl[i].ledName].priority = ledControl[i].priority;
                ledToSet[ledControl[i].ledName].state = ledControl[i].state;
                }
            }
        }

    for (led = 0; led < kLedEnd; led++)
        {
        if (ledToSet[led].state != ledState[led])
            {
            if (0 == devCtl_boardIoctl(BOARD_IOCTL_LED_CTRL, 0, NULL, led, 
                ledToSet[led].state, NULL))
                {
                ledState[led] = ledToSet[led].state;
                }
            }
        }
    }

void AlarmInit (void)
    {
    memset(&ledState[0], 0, sizeof(ledState));
    }

////////////////////////////////////////////////////////////////////////////////
/// \brief  Returns LLID statistic alarm condition
///
/// \param  stat    Statistic identifier of interest
/// \param  inst    Instance of the stats
///
/// \return Condition of the LLID stat
////////////////////////////////////////////////////////////////////////////////
//extern
Bool AlarmLlidStatCondGet(StatId stat, U8 inst)
    {
    return OnuOsAssertGet(OnuAssertStatId(stat), OsAstStatLinkInst(inst));
    }

////////////////////////////////////////////////////////////////////////////////
/// AlarmGetCondition:  Is the given alarm currently raised?
///
 // Parameters:
/// \param cond     Alarm condition to query
/// \param inst     Instance of alarm condition
///
/// \return
/// Whether the given alarm is currently raised
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL AlarmGetCondition(AlmCondition cond, U8 inst)
    {
    return OnuOsAssertGet(asserts[cond], inst);
    } // AlarmGetCondition


////////////////////////////////////////////////////////////////////////////////
/// AlarmSetCondition:  indicates condition has occurred
///
 // Parameters:
/// \param cond Condition that has occurred
/// \param inst Instance of that condition
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void AlarmSetCondition (AlmCondition cond, U8 inst)
    {
    OsAstMsgQSet(asserts[cond], inst, 0);
    } // AlarmSetCondition


////////////////////////////////////////////////////////////////////////////////
/// AlarmClearCondition:  indicates condition has gone away
///
 // Parameters:
/// \param cond Condition that has gone away
/// \param inst Instance of that condition
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void AlarmClearCondition (AlmCondition cond, U8 inst)
    {
    OsAstMsgQClr(asserts[cond], inst, 0);
    } // AlarmClearCondition



////////////////////////////////////////////////////////////////////////////////
//extern
LinkIndex GetLinkForAlarm(AlmCondition cond, U8 inst)
    {
    if (((cond >= AlmPortFirst) && (cond < AlmPortLast)) ||
        ((cond >= AlmEponFirst) && (cond < AlmEponLast)) ||
		(cond == AlmOntSff8472Thd) ||
		(cond == AlmPortStatThreshold) ||
		(cond == AlmEponStatThreshold) ||
		(cond == AlmLinkStatThreshold) ||
		(inst >= TkOnuNumTxLlids))
        {
        return 0;
        }
    return inst;
    }


////////////////////////////////////////////////////////////////////////////////
//extern
U8 GetInstForAlarm(AlmCondition cond, U8 inst)
    {
    if (cond == AlmEponStatThreshold ||
        cond == AlmLinkStatThreshold ||
        cond == AlmPortStatThreshold
        )
        {
        if (inst >= OsAstStatLinkInstBase)
            {
            return (inst - OsAstStatLinkInstBase);
            }
        else if (inst >= OsAstStatPortInstBase)
            {
            return (inst - OsAstStatPortInstBase);
            }
        else
            {
            return 0;
            }
        }
    
    return inst;
    }


////////////////////////////////////////////////////////////////////////////////
/// AlarmLinkFaultPresent:  determines if a link fault is present
///
/// Presence of "link fault" is declared in the flags field of all
/// OAM packets.  This routine provides that value based on all
/// system alarms.
///
 // Parameters:
/// \param link Link to query
///
/// \return
/// TRUE if link fault is present
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL AlarmLinkFaultPresent (LinkIndex link)
    {
    return OnuOsAssertGet(OsAstAlmLinkNoGates, link) |
           OnuOsAssertGet(OsAstAlmEponLostSync, link) |
           OnuOsAssertGet(OsAstAlmOntGpioLinkFault, link);
    } // AlarmLinkFaultPresent



////////////////////////////////////////////////////////////////////////////////
/// AlarmDyingGasp:  determines if we're about to fail
///
/// Presence of "dying gasp" is declared in the flags field of all
/// OAM packets.  This routine provides that value based on all
/// system alarms.
///
 // Parameters:
/// None
///
/// \return
/// TRUE if the DG flag should be set
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL AlarmDyingGasp (LinkIndex link)
    {
    return OnuOsAssertGet(OsAstAlmOntPowerLoss, link) |
           OnuOsAssertGet(OsAstAlmOntGpioDyingGasp, link);
    } // AlarmDyingGasp



////////////////////////////////////////////////////////////////////////////////
/// AlarmCriticalEvent:  determines critical event is present
///
/// Presence of "critical event" is declared in the flags field of all
/// OAM packets.  This routine provides that value based on all
/// system alarms.
///
 // Parameters:
/// None
///
/// \return
/// TRUE if the CE flag should be set
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL AlarmCriticalEvent (LinkIndex link)
    {
    return OnuOsAssertGet(OsAstAlmOntAuthInfoUnvld, link) |
           OnuOsAssertGet(OsAstAlmOntGpioCritical, link);
    } // AlarmCriticalEvent

////////////////////////////////////////////////////////////////////////////////
/// \brief  Set EPON Alarm threshold value
///
/// \param id   alarm
/// \param val  Pointer to threshold value
///
/// \return
///  None
////////////////////////////////////////////////////////////////////////////////
//extern
static Bool AlarmEponThdValSet(OamTkAlarmCode alarmId, const void BULK * val)
    {
    if (0 == ((alarmId - OamAlmCodePowerMonTempHigh) % 2))
        {
        if (((AlmMonThd *)val)->AlmRaise < ((AlmMonThd *)val)->AlmClear)
            {
            return FALSE;
            }
        }
    else
        {
        if (((AlmMonThd *)val)->AlmRaise > ((AlmMonThd *)val)->AlmClear)
            {
            return FALSE;
            }
        }

    memcpy(&powerMonAlmThreshod[alarmId - OamAlmCodePowerMonTempHigh], 
            val, 
            sizeof(AlmMonThd));

    return TRUE;
    }

////////////////////////////////////////////////////////////////////////////////
/// OamAlarmSetEponThdVal:  Set OAM alarm for Epon
///
 // Parameters:
/// \param src  OAM container into which value is to be requested
/// \param cont OAM container into which value is to be returned
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamAlarmSetEponThdVal (const OamVarContainer BULK* src,
                            OamVarContainer BULK* cont)
    {
    OamTkAlarmCode alarmCode;
    AlmMonThd * thd = (AlmMonThd *)src->value;

    alarmCode = *((OamTkAlarmCode BULK*)(src->value));
    if ((alarmCode < OamAlmCodePowerMonTempHigh) || 
        (alarmCode > OamAlmCodePowerMonRxPowerLow))
        {
        cont->length = OamVarErrAttrUnsupported;
        return;
        }
    
    if (FALSE == AlarmEponThdValSet(alarmCode, thd))
        {
        cont->length = OamVarErrActBadParameters;
        return;
        }

    cont->length = OamVarErrNoError;
    } // OamStatsSetEponThdVal

////////////////////////////////////////////////////////////////////////////////
/// \brief  Get EPON Alarm threshold value
///
/// \param id   alarm
/// \param val  Pointer to threshold value
///
/// \return
///  None
////////////////////////////////////////////////////////////////////////////////
//extern
static void AlarmEponThdValGet(OamTkAlarmCode alarmCode, const void BULK * val)
    {
    powerMonAlmThreshod[alarmCode - OamAlmCodePowerMonTempHigh].alarmId = alarmCode;
    memcpy((AlmMonThd *)val, 
            &powerMonAlmThreshod[alarmCode - OamAlmCodePowerMonTempHigh], 
            sizeof(AlmMonThd));
    }

////////////////////////////////////////////////////////////////////////////////
/// OamAlarmGetEponThdVal:  Get OAM alarm threshold for Epon
///
 // Parameters:
/// \param src  OAM container into which value is to be requested
/// \param cont OAM container into which value is to be returned
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////

void OamAlarmGetEponThdVal (const OamVarContainer BULK* src,
                            OamVarContainer BULK* cont)
    {
    OamTkAlarmCode alarmCode;
    AlmMonThd * thd = (AlmMonThd *)cont->value;

    alarmCode = *((OamTkAlarmCode BULK*)(src->value));
    if ((alarmCode < OamAlmCodePowerMonTempHigh) || 
        (alarmCode > OamAlmCodePowerMonRxPowerLow))
        {
        cont->length = OamVarErrAttrUnsupported;
        return;
        }

    cont->length = sizeof(AlmMonThd);
    
    AlarmEponThdValGet(alarmCode, thd);
    } // OamStatsSetEponThdVal

////////////////////////////////////////////////////////////////////////////////
/// AlarmThresholdNotifyTimer:  Call this function to check the alarm threshold and raise an alarm.
///
 // Parameters:
/// None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern

void AlarmThresholdNotifyTimer(void)
    {
    U16  value;
    U8   almCode;
    U8   almId;
    U8   i;

    for(i = 0; i < 5; i++)
        {
        if (func[i](&value))
            {
            // High threshold alarm
            almCode = OamAlmCodePowerMonTempHigh + (2*i);
            almId = AlmPwrMonTempHigh + (2*i);

            if ((value >= powerMonAlmThreshod[almCode - OamAlmCodePowerMonTempHigh].AlmRaise) &&
                (0 != powerMonAlmThreshod[almCode - OamAlmCodePowerMonTempHigh].AlmRaise))
                {
                AlarmSetCondition((AlmCondition)almId,0);
                }
            if ((value <= powerMonAlmThreshod[almCode - OamAlmCodePowerMonTempHigh].AlmClear) &&
                (0 != powerMonAlmThreshod[almCode - OamAlmCodePowerMonTempHigh].AlmClear))
                {
                AlarmClearCondition((AlmCondition)almId,0);
                }

			// Low threshold alarm
            almCode = OamAlmCodePowerMonTempHigh + (2*i) +1;
            almId = AlmPwrMonTempHigh + (2*i) + 1;

            if ((value <= powerMonAlmThreshod[almCode - OamAlmCodePowerMonTempHigh].AlmRaise) &&
                (0 != powerMonAlmThreshod[almCode - OamAlmCodePowerMonTempHigh].AlmRaise))
                {
                AlarmSetCondition((AlmCondition)almId,0);
                }
            if ((value >= powerMonAlmThreshod[almCode - OamAlmCodePowerMonTempHigh].AlmClear) &&
                (0 != powerMonAlmThreshod[almCode - OamAlmCodePowerMonTempHigh].AlmClear))
                {
                AlarmClearCondition((AlmCondition)almId,0);
                }
            }
        }
    }
// end Alarms.c

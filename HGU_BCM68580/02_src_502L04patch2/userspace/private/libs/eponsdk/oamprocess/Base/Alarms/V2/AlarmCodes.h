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


#if !defined(AlarmCodes_h)
#define AlarmCodes_h
////////////////////////////////////////////////////////////////////////////////
/// \file Alarms.h
/// \brief Alarm conditions and reporting
///
/// The Alarms code handles alarm conditioning, hierarchical masking, and
/// notification of alarm conditions.
///
/// Terminology in this module:
///
/// Condition: presence of a state or event that can cause an alarm
///
/// Conditioning: process of "debouncing" a condition to see if it should
/// really become an alarm.  Also called "soaking".  Some conditions have to
/// persist or reoccur before alarms are declared.  Alarms may require soaking
/// before they clear, just as conditions require soaking before they become
/// alarms.
///
/// Alarm: Noteworthy and usually bad thing that requires attention.  Conditions
/// grow up to become alarms if they survive conditioning
///
/// Masking: The hiding of one alarm by the presence of another.  For instance,
/// a loss of signal alarm might mask all other low-level comms link alarms,
/// since if there's no signal the other alarms are irrelevant and perhaps even
/// expected.
////////////////////////////////////////////////////////////////////////////////

#if defined(__cplusplus)
extern "C" {
#endif

/// possible conditions that can occur
typedef enum
    {
    AlmFirst,
    // per-ONT alarms
    AlmOntFirst = AlmFirst,
    AlmOntPowerLoss = AlmOntFirst,
    AlmOntBootInvalid,
    AlmOntSelfTestErr,
    AlmOntFlashBusy,
    AlmOntSysBusy,
    AlmOntFirmwareDownload,

    AlmOntAuthInfoUnvld,
    AlmOntAuthenticated,

    AlmOntGpioLinkFault,
    AlmOntGpioCritical,
    AlmOntGpioDyingGasp,
    AlmOntGpioOther,

    AlmOnt1GDnRegistered,
    AlmOnt1GDnActivity,
    AlmOnt10GDnRegistered,
    AlmOnt10GDnActivity,
    AlmOnt1GUpRegistered,
    AlmOnt1GUpActivity,
    AlmOnt10GUpRegistered,
    AlmOnt10GUpActivity,
    AlmOntSingleLinkReg,
    AlmOntMultiLinkReg,

    AlmOntFecUp,
    AlmOntFecDn,

    AlmOntSff8472Thd,

    AlmOntBatteryLost,
    AlmOntBatteryFail,
    AlmOntBatteryVoltLow,

    AlmOntPonActPri,
    AlmOntPonActSec,
    AlmOntLearnEvent,
    AlmOntWdt,
    AlmOntReset,
    AlmOntRestore,
    AlmOntProtSwitch,
    AlmOntRogueOnu,
    AlmOntLast,

    // EPON port alarms
    AlmEponFirst = AlmOntLast,
    AlmEponLos = AlmEponFirst,
    AlmEponLostSync,
    AlmEponStatThreshold,
    AlmEponActLed,
    AlmEponStandbyLos,
    AlmEponLaserShutdownTemp,
    AlmEponLaserShutdownPerm,
    AlmEponLaserOn,
    AlmPwrMonTempHigh,
    AlmPwrMonTempLow,
    AlmPwrMonVccHigh,
    AlmPwrMonVccLow,
    AlmPwrMonTxBiasHigh,
    AlmPwrMonTxBiasLow,
    AlmPwrMonTxPowerHigh,
    AlmPwrMonTxPowerLow,
    AlmPwrMonRxPowerHigh,
    AlmPwrMonRxPowerLow,
    AlmEponLast,

    // Ethernet port alarms
    AlmPortFirst = AlmEponLast,
    AlmPortLearnTblOverflow = AlmPortFirst,
    AlmPortLoopback,
    AlmPortStatThreshold,
    AlmPortLinkDown,
    AlmPortBlocked,
    AlmPortTraffic,
    AlmPortLinkUp,
    AlmPortDisabled,
    AlmPortAutoNegFailure,
    AlmPortLast,

    // per-LLID alarms
    AlmLinkFirst = AlmPortLast,
    AlmLinkNoGates = AlmLinkFirst,
    AlmLinkUnregistered,
    AlmLinkOamTimeout,
    AlmLinkOamDiscComplete,
    AlmLinkStatThreshold,
    AlmLinkKeyExchange,
    AlmLinkLoopback,
    AlmLinkAllDown,
    AlmLinkDisabled,
    AlmLinkRegStart,
    AlmLinkRegSucc,
    AlmLinkRegFail,
    AlmLinkRegTimeOut,
    AlmLinkCtcOamDiscComplete,
    AlmLinkLast,

    // per queue alarms
    AlmQueueFirst = AlmLinkLast,
    AlmQueueStatThreshold = AlmQueueFirst,
    AlmQueueLast,

    // Other alarms
    AlmOtherFirst = AlmQueueLast,
    AlmOtherGpioInput = AlmOtherFirst,
    AlmOtherLast,

    AlmNumConditions = AlmOtherLast
    } AlmCondition;

#if defined(__cplusplus)
}
#endif

#endif // Alarms.h


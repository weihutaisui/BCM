//****************************************************************************
//
// Copyright (c) 2005-2010 Broadcom Corporation
//
// This program is the proprietary software of Broadcom Corporation and/or
// its licensors, and may only be used, duplicated, modified or distributed
// pursuant to the terms and conditions of a separate, written license
// agreement executed between you and Broadcom (an "Authorized License").
// Except as set forth in an Authorized License, Broadcom grants no license
// (express or implied), right to use, or waiver of any kind with respect to
// the Software, and Broadcom expressly reserves all rights in and to the
// Software and all intellectual property rights therein.  IF YOU HAVE NO
// AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY,
// AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE
// SOFTWARE.  
//
// Except as expressly set forth in the Authorized License,
//
// 1.     This program, including its structure, sequence and organization,
// constitutes the valuable trade secrets of Broadcom, and you shall use all
// reasonable efforts to protect the confidentiality thereof, and to use this
// information only in connection with your use of Broadcom integrated circuit
// products.
//
// 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
// "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS
// OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
// RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL
// IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR
// A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET
// ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME
// THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
//
// 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM
// OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
// INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY
// RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM
// HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN
// EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1,
// WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY
// FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
//
//****************************************************************************
//  $Id$
//
//  Filename:       BatterySoftTimer.h
//  Author:         Kevin O'Neal
//  Creation Date:  July 25, 2005
//
//****************************************************************************

#include "BatterySoftTimer.h"
#include "MessageLog.h"

#include "BatteryThread.h"
#include "BatteryNonVolSettings.h"
#include "upsAlarms.h"

#if (SNMP_SUPPORT)
#include "upsTrap.h"
#include "upsMib.h"
#include "BcmSnmpAgent.h"
#endif


BatterySoftTimerList::BatterySoftTimerList(BcmBatteryThread *pThread) :
  BcmSoftTimerList ("Battery Soft Timers")
{
  fpThread = pThread;
}
    
void BatterySoftTimerList::UpdateTimers ()
{
  fpThread->UpdateTimers();
}


BatteryPollSoftTimer::BatteryPollSoftTimer () :
  BcmSoftTimer ("Battery Poll Soft Timer")
{
}

void BatteryPollSoftTimer::Fire ()
{
  BcmBatteryThread::Singleton().PollBattery();
  
  // Re-arm with NV setting.
  Arm (BcmBatteryNonVolSettings::GetSingletonInstance()->PollIntervalSeconds() * 1000, kOneShot);
}


BatteryStateSoftTimer::BatteryStateSoftTimer () :
  BcmSoftTimer ("Battery State Soft Timer")
{
}

void BatteryStateSoftTimer::Fire ()
{
  BcmBatteryThread::Singleton().BatteryController().UpdateState();
  BcmBatteryThread::Singleton().CheckForPowerSourceTransition();
}


#if (SNMP_SUPPORT)

upsAlarmShutdownImminentSoftTimer::upsAlarmShutdownImminentSoftTimer (BcmSnmpAgent *pAgent) :
  BcmSoftTimer ("upsAlarmShutdownImminent")
{
  fpAlarmTable = (upsAlarmTable *)pAgent->FindTable(BcmObjectId(kOID_upsAlarmTable));
  BcmBatteryThread::Singleton().AddTimer(this);
}

void upsAlarmShutdownImminentSoftTimer::Fire ()
{
  if (fpAlarmTable)
    fpAlarmTable->RaiseAlarm(kOID_upsAlarmShutdownImminent);
}

upsShutdownAfterDelaySoftTimer::upsShutdownAfterDelaySoftTimer (BcmSnmpAgent *pAgent) :
  BcmSoftTimer ("upsShutdownAfterDelay"),
  fUpsAlarmShutdownImminentSoftTimer(pAgent)
{
  fpAlarmTable = (upsAlarmTable *)pAgent->FindTable(BcmObjectId(kOID_upsAlarmTable));
  BcmBatteryThread::Singleton().AddTimer(this);
}

bool upsShutdownAfterDelaySoftTimer::Arm (unsigned long TimeoutMS, TimerMode Mode)
{
  bool Armed = BcmSoftTimer::Arm(TimeoutMS, Mode);
  
  // Also start a timer for the upsAlarmShutdownImminent which will come
  // 5 seconds before this happens.    
  unsigned long ImminentTimeout = 0;
  
  if (TimeoutMS > 5000)
    ImminentTimeout = TimeoutMS - 5000;
    
  fUpsAlarmShutdownImminentSoftTimer.Arm (ImminentTimeout, kOneShot);
  
  if (Armed)
  {
    if (fpAlarmTable)
      fpAlarmTable->RaiseAlarm(kOID_upsAlarmShutdownPending);
  }
                          
  return Armed;
}

bool upsShutdownAfterDelaySoftTimer::Disarm ()
{
  bool Disarmed = BcmSoftTimer::Disarm();
  
  if (Disarmed)
  {
    fUpsAlarmShutdownImminentSoftTimer.Disarm();
  
    if (fpAlarmTable)
    {
      fpAlarmTable->ClearAlarm(kOID_upsAlarmShutdownImminent);
      fpAlarmTable->ClearAlarm(kOID_upsAlarmShutdownPending);
    }
  }
    
  return Disarmed;
}

void upsShutdownAfterDelaySoftTimer::Fire ()
{
  // Clearing these alarms is not really necessary here since they get
  // cleared in Disarm, and Disarm is called prior to Fire in
  // BcmSoftTimerList::ProcessTimers.  Still, re-clearing won't hurt...
  if (fpAlarmTable)
  {
    fpAlarmTable->ClearAlarm(kOID_upsAlarmShutdownPending);
    fpAlarmTable->ClearAlarm(kOID_upsAlarmShutdownImminent);
  }
  
  // PR 8588: Raise various alarms before shutting off the battery just
  // in case we are on battery power.
  if (fpAlarmTable)
  {
    fpAlarmTable->RaiseAlarm(kOID_upsAlarmUpsOutputOff);
    fpAlarmTable->RaiseAlarm(kOID_upsAlarmOutputOffAsRequested);
    
    // If we are on battery power, we need to raise the 'awaiting power' alarm.
    // This is somewhat pointless since we are going to shut down immediately
    // and nobody is ever going to see this alarm, but just for the sake of
    // compliance...
    if (BcmBatteryThread::Singleton().BatteryController().OperatingOnBattery() == true)
      fpAlarmTable->RaiseAlarm(kOID_upsAlarmAwaitingPower);
  }

  if (BcmBatteryThread::Singleton().BatteryController().BatteryEnableOutput(false) == true)
  {
    /*
    Moved this up above so the alarms get raised before we shut down should
    we be on battery power.
    
    if (fpAlarmTable)
    {
      fpAlarmTable->RaiseAlarm(kOID_upsAlarmUpsOutputOff);
      fpAlarmTable->RaiseAlarm(kOID_upsAlarmOutputOffAsRequested);
    }
    */
  }
}

// Necessary to code to broken PacketCable 1.5 CTP.  Tells us if 
// this timer has ever been previously armed.
bool upsShutdownAfterDelaySoftTimer::EverArmed () const
{
  // If we have ever been armed then fTimeToFire will contain the
  // firing time established by the previous arming.  If we have never
  // been armed then it will be 0.
  if (fTimeToFire == 0)
    return false;
    
  return true;
}


upsStartupAfterDelaySoftTimer::upsStartupAfterDelaySoftTimer (BcmSnmpAgent *pAgent) :
  BcmSoftTimer ("upsStartupAfterDelay")
{
  fpAlarmTable = (upsAlarmTable *)pAgent->FindTable(BcmObjectId(kOID_upsAlarmTable));
  BcmBatteryThread::Singleton().AddTimer(this);
}

void upsStartupAfterDelaySoftTimer::Fire ()
{
  if (BcmBatteryThread::Singleton().BatteryController().BatteryEnableOutput(true) == true)
  {
    if (fpAlarmTable)
    {
      fpAlarmTable->ClearAlarm(kOID_upsAlarmUpsOutputOff);
      fpAlarmTable->ClearAlarm(kOID_upsAlarmOutputOffAsRequested);
    }
  }
}


upsRebootWithDurationSoftTimer::upsRebootWithDurationSoftTimer (BcmSnmpAgent *pAgent) :
  BcmSoftTimer ("upsRebootWithDuration")
{
  fpAlarmTable = (upsAlarmTable *)pAgent->FindTable(BcmObjectId(kOID_upsAlarmTable));
  fDuration = 0;
  BcmBatteryThread::Singleton().AddTimer(this);
}

bool upsRebootWithDurationSoftTimer::Arm (unsigned long TimeoutMS, TimerMode Mode)
{
  fDuration = TimeoutMS;

  // PR 9760: 2 stage timer to allow SNMP reply to get out.  First just arm
  // ourselves with 0 ms delay so the SNMP thread can get the reply out.
  // Then we'll do the 'real' arming afterwards.
  if (BcmBatteryThread::Singleton().BatteryController().BatteryOutputEnabled() == true)
  {
    // Also raise alarms here, so they can get up before we shut down should
    // we be on battery power.
    if (fpAlarmTable)
    {
      fpAlarmTable->RaiseAlarm(kOID_upsAlarmUpsOutputOff);
      fpAlarmTable->RaiseAlarm(kOID_upsAlarmOutputOffAsRequested);
      
      // If we are on battery power, we need to raise the 'awaiting power' alarm.
      // This is somewhat pointless since we are going to shut down immediately
      // and nobody is ever going to see this alarm, but just for the sake of
      // compliance...
      if (BcmBatteryThread::Singleton().BatteryController().OperatingOnBattery() == true)
        fpAlarmTable->RaiseAlarm(kOID_upsAlarmAwaitingPower);
    }
    
    return BcmSoftTimer::Arm(0, kOneShot);
  }
  
  // Call BatteryEnableOutput prior to calling BcmSoftTimer::Arm to avoid
  // possible race conditions when arming with 0 second delay.  The problem
  // is, if we have 0 delay, we're going to fire as soon as we arm.
  // If we arm first, it's possible we could fire (thus enabling output)
  // before we get to our EnableOutput(false) call, which would leave us
  // disabled when all is said and done.
  if (BcmBatteryThread::Singleton().BatteryController().BatteryEnableOutput(false) == true)
  {
    /*
    Moved to above...
    if (fpAlarmTable)
    {
      fpAlarmTable->RaiseAlarm(kOID_upsAlarmUpsOutputOff);
      fpAlarmTable->RaiseAlarm(kOID_upsAlarmOutputOffAsRequested);
    }
    */
  }

  return BcmSoftTimer::Arm(fDuration, Mode);
}

bool upsRebootWithDurationSoftTimer::Disarm ()
{
  return BcmSoftTimer::Disarm();
}

void upsRebootWithDurationSoftTimer::Fire ()
{
  // PR 9760: 2 stage timer to allow SNMP reply to get out.
  if (BcmBatteryThread::Singleton().BatteryController().BatteryOutputEnabled() == true)
  {
    BcmBatteryThread::Singleton().BatteryController().BatteryEnableOutput(false);
    Arm (fDuration, fMode);
    return;  
  }

  if (BcmBatteryThread::Singleton().BatteryController().BatteryEnableOutput(true) == true)
  {
    if (fpAlarmTable)
    {
      fpAlarmTable->ClearAlarm(kOID_upsAlarmUpsOutputOff);
      fpAlarmTable->ClearAlarm(kOID_upsAlarmOutputOffAsRequested);
    }
  }
}

// Necessary to code to broken PacketCable 1.5 CTP.  Tells us if 
// this timer has ever been previously armed.
bool upsRebootWithDurationSoftTimer::EverArmed () const
{
  // If we have ever been armed then fTimeToFire will contain the
  // firing time established by the previous arming.  If we have never
  // been armed then it will be 0.
  if (fTimeToFire == 0)
    return false;
    
  return true;
}


upsTrapOnBatterySoftTimer::upsTrapOnBatterySoftTimer (BcmSnmpAgent *pAgent) :
  BcmSoftTimer ("upsTrapOnBattery ")
{
  fpAgent = pAgent;
  fName += fpAgent->GetName();
}

void upsTrapOnBatterySoftTimer::Fire ()
{
  BcmTrap *pTrap = new upsTrapOnBattery(fpAgent);
  fpAgent->SendTrap(pTrap);
  delete (pTrap);
  
  Arm (60000, kOneShot);
}

#endif

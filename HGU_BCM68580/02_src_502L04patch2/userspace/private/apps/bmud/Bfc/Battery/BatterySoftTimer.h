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

#ifndef BATTERYSOFTTIMER_H
#define BATTERYSOFTTIMER_H

#include "SoftTimer.h"

class BcmBatteryThread;


class BatterySoftTimerList : public BcmSoftTimerList
{
public:
    BatterySoftTimerList(BcmBatteryThread *pThread);
    
    void UpdateTimers ();
    
protected:
    BcmBatteryThread *fpThread;
};

class BatteryPollSoftTimer : public BcmSoftTimer
{
public:
    BatteryPollSoftTimer ();

    virtual void Fire ();
};

class BatteryStateSoftTimer : public BcmSoftTimer
{
public:
    BatteryStateSoftTimer ();

    virtual void Fire ();
};

#if (SNMP_SUPPORT)

/* UPS-MIB related timers.  Should these get their own module? */

class BcmSnmpAgent;
class upsAlarmTable;


class upsAlarmShutdownImminentSoftTimer : public BcmSoftTimer
{
public:
    upsAlarmShutdownImminentSoftTimer (BcmSnmpAgent *pAgent);
    
    virtual void Fire ();
    
protected:
    upsAlarmTable *fpAlarmTable;
};

class upsShutdownAfterDelaySoftTimer : public BcmSoftTimer
{
public:
    upsShutdownAfterDelaySoftTimer (BcmSnmpAgent *pAgent);
    
    virtual bool Arm (unsigned long TimeoutMS, TimerMode Mode);
    virtual bool Disarm ();
    
    virtual void Fire ();
    
    // Necessary to code to broken PacketCable 1.5 CTP.  Tells us if 
    // this timer has ever been previously armed.
    virtual bool EverArmed () const;
    
protected:
    upsAlarmTable *fpAlarmTable;
    upsAlarmShutdownImminentSoftTimer fUpsAlarmShutdownImminentSoftTimer;
};

class upsStartupAfterDelaySoftTimer : public BcmSoftTimer
{
public:
    upsStartupAfterDelaySoftTimer (BcmSnmpAgent *pAgent);

    virtual void Fire ();
    
protected:
    upsAlarmTable *fpAlarmTable;
};

class upsRebootWithDurationSoftTimer : public BcmSoftTimer
{
public:
    upsRebootWithDurationSoftTimer (BcmSnmpAgent *pAgent);
    
    virtual bool Arm (unsigned long TimeoutMS, TimerMode Mode);
    virtual bool Disarm ();

    virtual void Fire ();
    
    // Necessary to code to broken PacketCable 1.5 CTP.  Tells us if 
    // this timer has ever been previously armed.
    virtual bool EverArmed () const;
    
protected:
    upsAlarmTable *fpAlarmTable;
    unsigned long fDuration;
};

class upsTrapOnBatterySoftTimer : public BcmSoftTimer
{
public:
    upsTrapOnBatterySoftTimer (BcmSnmpAgent *pAgent);
    
    virtual void Fire ();
    
protected:
    BcmSnmpAgent *fpAgent;
};

#endif

#endif

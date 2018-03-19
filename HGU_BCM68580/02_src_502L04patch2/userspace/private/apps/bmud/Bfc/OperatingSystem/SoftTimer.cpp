//****************************************************************************
//
// Copyright (c) 2007 Broadcom Corporation
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
//  Filename:       SoftTimer.h
//  Author:         Kevin O'Neal
//  Creation Date:  July 25, 2005
//
//  This module implements an OS independent soft timer.  It is not derived
//  from BcmTimer and works differently in that it does not consume any OS
//  timer resources and does not dispatch a BcmEvent when the timer fires.
//  Instead, this timer may be used in conjunction with any kind of mechanism
//  in a thread which allows you to specify a timeout (ie message queue, event,
//  socket set select, etc).  Here's an example of how this might work:
//
//  void BcmMyThread::ThreadMain(void)
//  {
//    unsigned int MsgCode;
//    void *pMsgBuf = NULL;
//
//    while (1)
//    {
//      if (pfMsgQueue->Receive (MsgCode, pMsgBuf, BcmMessageQueue::kTimeout, 
//                               fMyTimerList.GetTimeout()) == true)
//      {
//        // Received an event, handle it.
//        switch (MsgCode)
//        {
//          // whatever...
//        }
//      }
//      
//      // Process any soft timers which are due.
//      fMyTimerList.ProcessTimers();
//    }
// }
//
//  The call to BcmSoftTimerList::GetTimeout will iterate over the list of 
//  timers and calculate the timeout necessary to expire when the first
//  timer is set to fire.  If the list is empty then this method will return
//  kAsLongAsPossible (0xFFFFFFFF), meaning that the thread will wait as long 
//  as possible (about 50 days) before timing out - this being as close as
//  we can get to 'forever'.
//
//  The call to BcmSoftTimerList::ProcessTimers will iterate over the list of
//  timers and call the virtual method BcmSoftTimer::Fire for any that are
//  due.  Note that in order to do anything interesting, a derived BcmSoftTimer
//  must be created which overrides the Fire method to take action.
//
//****************************************************************************

#include "SoftTimer.h"

#include "SystemTime.h"
#include "MessageLog.h"

#include "OperatingSystemFactory.h"
#include "MutexSemaphore.h"


BcmSoftTimer::BcmSoftTimer (const char *pName)
{
  fArmed = false;
  fMode = kOneShot;
  fTimeToFire = 0;
  fTimeout = 0;
  fpList = NULL;

  if (pName)
    fName = pName;
}

BcmSoftTimer::~BcmSoftTimer ()
{
  Disarm();

  if (fpList)
    fpList->RemoveTimer(this);
}

const BcmString &BcmSoftTimer::Name () const
{
  return fName;
}

bool BcmSoftTimer::Arm (unsigned long TimeoutMS, TimerMode Mode)
{
  Counter64 C64; // Structure, not object

  fMode = Mode;
  
  fTimeout = TimeoutMS;
  
  // Build our 64 bit time to fire value.  Use 64 bit counters here to 
  // avoid rollover problems.
  SystemTimeMS64(&C64);
  
  fTimeToFire.Set (C64.Lo, C64.Hi);
  fTimeToFire += TimeoutMS;
  
  fArmed = true;
  
  if (fpList)
    fpList->UpdateTimers();
    
  return true;
}

bool BcmSoftTimer::Disarm ()
{
  fArmed = false;
  
  if (fpList)
    fpList->UpdateTimers();
  
  return true;
}

bool BcmSoftTimer::Armed () const
{
  return fArmed;
}

BcmSoftTimer::TimerMode BcmSoftTimer::Mode () const
{
  return fMode;
}

unsigned long BcmSoftTimer::TimeoutMS () const
{
  return fTimeout;
}

unsigned long BcmSoftTimer::FiresInMS () const
{
  BcmCounter64 TimeToFire = fTimeToFire;
  Counter64 Now;
  
  SystemTimeMS64(&Now);
  
  if (TimeToFire < BcmCounter64(Now))
    TimeToFire = 0;
  else
    TimeToFire -= BcmCounter64(Now);
    
  return TimeToFire.Low32();
}

void BcmSoftTimer::Fire ()
{
  // Base class implementation does nothing useful.  To be useful a derived
  // class is needed which does something here!
  gLogMessageRaw << "BcmSoftTimer " << fName << " - FIRE!" << endl;
}

ostream &BcmSoftTimer::Print(ostream &outStream) const
{
  #if (!BCM_REDUCED_IMAGE_SIZE)
  
  if (Armed() == false)
    outStream << "  "  << Name() << ": DISARMED" << endl;
  else
    outStream << "  " << Name() << ": ARMED, will fire in " << FiresInMS() << " mS." << endl;
  
  #endif
    
  return outStream;
}



BcmSoftTimerList::BcmSoftTimerList (const char *pName)
{
  fpMutex = BcmOperatingSystemFactory::NewMutexSemaphore("BcmSoftTimerList mutex");
  
  if (fpMutex == NULL)
    gLogMessageRaw << "WARNING: failed to create mutex for " << pName << endl;

  if (pName)
    fName = pName;
}

BcmSoftTimerList::~BcmSoftTimerList ()
{
  // Create a mutex auto-lock object.  This will automatically release
  // the lock when it goes out of scope (ie when we return).
  BcmMutexSemaphoreAutoLock Lock(fpMutex);

  // Iterate over all timers and clear their list pointer, then clear our list.
  TimerList::iterator Pos = fTimers.begin();
  
  while (Pos != fTimers.end())
  {
    (*Pos)->fpList = NULL;
    Pos++;
  }
  
  fTimers.clear();
}

const BcmString &BcmSoftTimerList::Name () const
{
  return fName;
}

unsigned long BcmSoftTimerList::GetTimeout () const
{
  // Create a mutex auto-lock object.  This will automatically release
  // the lock when it goes out of scope (ie when we return).
  BcmMutexSemaphoreAutoLock Lock(fpMutex);

  bool Empty = true;
  BcmCounter64 NextFire(0xFFFFFFFF, 0xFFFFFFFF);
  Counter64 Now;
  BcmSoftTimer *pTimer;
  TimerList::const_iterator Pos = fTimers.begin();
  
  while (Pos != fTimers.end())
  {
    pTimer = *Pos;
    
    if (pTimer->Armed())
    {
      Empty = false;
      
      if (pTimer->fTimeToFire < NextFire)
        NextFire = pTimer->fTimeToFire;
    }
      
    Pos++;
  }

  // If we have no armed timers, return 'as long as possible'  
  if (Empty == true)
    return kAsLongAsPossible;
    
  SystemTimeMS64(&Now);
    
  // If time to fire is less than now, return 0 to fire right away
  if (NextFire < BcmCounter64(Now))
    return 0;
  
  // OK, now we have a 64 bit value of the soonest time to fire.  Just subtract
  // off SystemTimeMS64 and return the low 32 bits.  Since we only support a
  // 32 bit timeout in BcmSoftTimer::Arm, we know this value will never be more
  // than 32 bits.
  NextFire -= BcmCounter64(Now);
  
  return NextFire.Low32();
}

void BcmSoftTimerList::ProcessTimers ()
{
  // Create a mutex auto-lock object.  This will automatically release
  // the lock when it goes out of scope (ie when we return).
  BcmMutexSemaphoreAutoLock Lock(fpMutex);

  Counter64 Now;
  BcmSoftTimer *pTimer;
  TimerList::iterator Pos = fTimers.begin();
  
  SystemTimeMS64(&Now);
  
  while (Pos != fTimers.end())
  {
    pTimer = *Pos;
    
    if (pTimer->Armed())
    {
      SystemTimeMS64(&Now);
      
      if (pTimer->fTimeToFire <= BcmCounter64(Now))
      {
        // If this is a periodic timer, reset it.
        if (pTimer->Mode() == BcmSoftTimer::kPeriodic)
        {
          pTimer->fTimeToFire = Now;
          pTimer->fTimeToFire += pTimer->fTimeout;
        }        
        
        // Otherwise disarm it, but don't remove it from the list.
        else
          pTimer->Disarm();
        
        // Fire the timer.
        pTimer->Fire();
      }
    }
    
    Pos++;
  }
}

bool BcmSoftTimerList::AddTimer (BcmSoftTimer *pTimer)
{
  // Create a mutex auto-lock object.  This will automatically release
  // the lock when it goes out of scope (ie when we return).
  BcmMutexSemaphoreAutoLock Lock(fpMutex);

  // Don't add if this is already in the list (or any other list for that
  // matter)
  if (pTimer->fpList != NULL)
    return false;
    
  fTimers.push_back(pTimer);
  pTimer->fpList = this;
  
  UpdateTimers();
  
  return false;
}

bool BcmSoftTimerList::RemoveTimer (BcmSoftTimer *pTimer)
{
  // Create a mutex auto-lock object.  This will automatically release
  // the lock when it goes out of scope (ie when we return).
  BcmMutexSemaphoreAutoLock Lock(fpMutex);

  if (pTimer->fpList != this)
    return false;
    
  pTimer->fpList = NULL;
  fTimers.remove(pTimer);
  
  // Should I disarm this timer?  Nah...
  
  UpdateTimers();
  
  return true;
}

void BcmSoftTimerList::UpdateTimers ()
{
  // This method is called whenever anything about the timers changes.
  // That is, a timer is added, removed, armed, or disarmed.
  // Base class implementation here does nothing.  Derived class might
  // wish to signal the owning thread so that it can break out of a 
  // wait state and recheck the timeout.
}

ostream &BcmSoftTimerList::Print(ostream &outStream) const
{
  TimerList::const_iterator Pos = fTimers.begin();
  
  outStream << Name() << endl;
  
  while (Pos != fTimers.end())
  {
    outStream << *(*Pos);
    Pos++;
  }

  return outStream;
}



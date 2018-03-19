//****************************************************************************
//
//  Copyright (c) 2005  Broadcom Corporation
//  All Rights Reserved
//  No portions of this material may be reproduced in any form without the
//  written permission of:
//          Broadcom Corporation
//          16251 Laguna Canyon Road
//          Irvine, California 92618
//  All information contained in this document is Broadcom Corporation
//  company private, proprietary, and trade secret.
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
//  0xFFFFFFFF, meaning that the thread will wait as long as possible
//  before timing out.
//
//  The call to BcmSoftTimerList::ProcessTimers will iterate over the list of
//  timers and call the virtual method BcmSoftTimer::Fire for any that are
//  due.  Note that in order to do anything interesting, a derived BcmSoftTimer
//  must be created which overrides the Fire method to take action.
//
//****************************************************************************

#ifndef SOFTTIMER_H
#define SOFTTIMER_H

#include "BcmString.h"
#include "Counter64.h"
#include <list>

// Forward references:
class BcmSoftTimerList;
class BcmMutexSemaphore;

class BcmSoftTimer
{
public:
    BcmSoftTimer (const char *pName = NULL);
    virtual ~BcmSoftTimer ();
    
    const BcmString &Name () const;
   
    typedef enum
    {
      kOneShot = 0,
      kPeriodic
    } TimerMode;
    
    virtual bool Arm (unsigned long TimeoutMS, TimerMode Mode);
    virtual bool Disarm ();
    
    bool Armed () const;
    TimerMode Mode () const;
    unsigned long TimeoutMS () const;
    unsigned long FiresInMS() const;
    
    /// Print timer info.
    ///
    /// \param
    ///      outStream - the ostream to which the packet should be printed.
    ///
    /// \return
    ///      A reference to the ostream to support operation chaining.
    ///
    ostream &Print(ostream &outStream) const;
    
protected:
    virtual void Fire ();
    
protected:
    BcmString fName;
    bool fArmed;
    TimerMode fMode;
    BcmSoftTimerList *fpList;
    unsigned long fTimeout;
    
    // This is a 64 bit counter just to save the headache of dealing
    // with rollover of the SystemTimeMS clock on which we rely.
    BcmCounter64 fTimeToFire;
    
friend class BcmSoftTimerList;
};

inline ostream & operator << (ostream &outStream, const BcmSoftTimer &Timer)
{
  return Timer.Print(outStream);
}


typedef list <BcmSoftTimer *> TimerList;

class BcmSoftTimerList
{
public:
    BcmSoftTimerList (const char *pName = NULL);
    virtual ~BcmSoftTimerList ();
    
    const BcmString &Name () const;
        
    // Possible return values from GetTimeout
    enum
    {
      kRightNow = 0,
      kAsLongAsPossible = 0xFFFFFFFF
    };

    unsigned long GetTimeout () const;
    
    void ProcessTimers ();
    
    bool AddTimer (BcmSoftTimer *pTimer);
    bool RemoveTimer (BcmSoftTimer *pTimer);
    
    /// Print timer list info.
    ///
    /// \param
    ///      outStream - the ostream to which the packet should be printed.
    ///
    /// \return
    ///      A reference to the ostream to support operation chaining.
    ///
    ostream &Print(ostream &outStream) const;
    
protected:
    virtual void UpdateTimers ();
    
protected:
    BcmMutexSemaphore *fpMutex;
    BcmString fName;
    TimerList fTimers;
    
friend class BcmSoftTimer;
};

inline ostream & operator << (ostream &outStream, const BcmSoftTimerList &List)
{
  return List.Print(outStream);
}

#endif

//****************************************************************************
//
// Copyright (c) 1999-2009 Broadcom Corporation
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
//  Filename:       Timer.h
//  Author:         David Pullen
//  Creation Date:  Feb 18, 1999
//
//****************************************************************************

#ifndef TIMER_H
#define TIMER_H

//********************** Include Files ***************************************

#include "typedefs.h"

#include "MessageLog.h"

//********************** Global Types ****************************************

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Forward Declarations ********************************

// I need to tell the compiler that this exists, but it doesn't need to know
// anything about it yet.
class BcmEvent;

//********************** Class Declaration ***********************************


/** \ingroup OsWrappers
*
*   This is the abstract base class for operating system timers.  A timer
*   causes the OS to send an event (BcmEvent) to the thread after the
*   specified timeout interval has expired; thus BcmTimer has-a BcmEvent.
*
*   Timers are an OS component that keep track of elapsed time, and are
*   generally run off of the main system clock.  When the specified time
*   interval has elapsed, the OS internally sends an event to the thread
*   that owns it.  Because timers are associated with events, BcmTimer
*   has a BcmEvent, and can be used with a BcmEventSet object.
*
*   Timers can be used in a one-shot or a repeating mode.
*
*   NOTE:  Unlike other OS wrapper objects, where some of the methods are
*   meant to be used by other threads, all of the methods of BcmTimer can
*   only be operated on by the thread that created/owns it.  This is
*   because the event is only valid in the context of the owning thread,
*   and in some operating systems a timer must be started by the thread
*   that owns it.  For this reason, it must never be accessible outside of
*   the thread.
*/
class BcmTimer
{
public:

    /// Default Constructor.  Sets everything to a quiescent state; note that
    /// the event to trigger is not created here - it must be created by the
    /// derived class.
    ///
    /// \param
    ///      pName - the text name that is to be given to the object, for
    ///              debugging purposes.
    ///
    BcmTimer(const char *pName = NULL);

    /// Destructor.  Frees up any memory/objects allocated, cleans up internal
    /// state.  The most-derived class must call Stop() in order to ensure that
    /// the timer isn't running.
    ///
    /// pfEventToTrigger is deleted here.
    ///
    virtual ~BcmTimer();

    /// Mode values to be used in the call to Start().
    typedef enum
    {
        /// Specifies that this is a one-shot timer - when it expires, it
        /// triggers the event, then sits idle.  You can call Restart() to get
        /// it to run again with the same parameters.
        kOnce,

        /// Specifies that this is a periodic timer - when it expires, it
        /// triggers the event, and continues running.
        kRepeat

    } TimerMode;

    /// Starts the timer running (restarting the timer if it is already running).
    /// When the timer expires, the OS will send the specified event to you.
    ///
    /// The timer will automatically restart if the kRepeat flag is set; 
    /// otherwise, it just sits idle once it expires.
    ///
    /// This is a pure virtual method, and must be provided by the OS-specific
    /// subclass.
    ///
    /// \param
    ///      timeoutMS - the number of milliseconds that the timer should count
    ///                  before expiring.
    /// \param
    ///      mode - set this to kRepeat to get the timer to automatically restart
    ///             after it expires.
    ///
    /// \retval
    ///      true if the timer was started successfully.
    /// \retval
    ///      false if there was a problem starting the timer.
    ///
    virtual bool Start(unsigned long timeoutMS, TimerMode mode = kOnce) = 0;

    /// Stops the timer and restarts it with the timeout value and mode that
    /// were sent in on the last call to Start().  If Start() has not been
    /// called, then the timer is not restarted.
    ///
    /// This is useful because it eliminates extra calls to the object, and you
    /// don't have to know what the previous values were in order to restart it.
    ///
    /// \retval
    ///      true if the timer was restarted successfully.
    /// \retval
    ///      false if the timer was not restarted (Start() has not bee called
    ///          previously, or there was a problem stopping and/or starting it).
    ///
    virtual bool Restart(void);

    /// Stops a timer that is running (preventing it from sending the event).
    /// There may be a race condition where the timer expires (handled by the OS
    /// at interrupt time) as this function is executing, which will cause the
    /// event to be triggered.
    ///
    /// This is a pure virtual method, and must be provided by the OS-specific
    /// subclass.
    ///
    /// \retval
    ///      true if the timer was successfully stopped.
    /// \retval
    ///      false if there was a problem.
    ///
    virtual bool Stop(void) = 0;

    /// Accessor to know if the existing timer is currently active or not.
    ///
    /// \retval
    ///      true if the timer is currently active
    /// \retval
    ///      false if the timer is not currently active
    virtual bool IsActive(void) = 0;

    /// Causes the calling thread to be blocked until the timer expires.  If the
    /// timer was not started, then it returns false immediately.
    ///
    /// NOTE:  If a thread stops the timer while another thread is waiting for
    ///        it to expire, then that thread will continue to be blocked waiting
    ///        forever!
    ///
    /// \retval
    ///      true if the timer expired.
    /// \retval
    ///      false if the timer was not started.
    ///
    bool Wait(void);

    /// Returns the event that is triggered when the timer expires.  This is
    /// primarily used by BcmEventSet when the timer is added to the list of
    /// events that are to be waited on.
    ///
    /// \return
    ///      The timer's event.
    ///
    inline BcmEvent *GetEvent(void) const;

    /// An alternate way to get the pointer to the event.  You can use it this
    /// way:
    ///
    /// \code
    ///   BcmTimer *pTimer;
    ///   BcmEvent *pEvent;
    ///
    ///   pEvent = (*pTimer);
    /// \endcode
    ///
    /// \return
    ///      The event associated with the timer.
    ///
    inline operator BcmEvent * () const;

    /// Simple accessor for the name of this object.
    ///
    /// \return
    ///      The pointer to the name string.
    ///
    inline const char *Name(void) const;

protected:

    /// The event that will be triggered by the timer when it expires.  This must
    /// be created in the constructor of the OS-specific derived class, but it is
    /// destroyed by the destructor of this class.
    BcmEvent *pfEventToTrigger;

    /// Tells whether or not Start() has been called.
    bool fHasBeenStarted;

    /// Tells whether or not the timer is currently running.  Note that this may
    /// not be accurate, especially in the case of one-shot timers, because the
    /// OS does not call the timer object to let it know that the event has been
    /// triggered so that this member can be set to false.  This member should
    /// just be viewed as an indication of the last user interaction with the
    /// timer.
    bool fIsRunning;

    /// The last value specified in the call to Start(), to be used in Restart().
    unsigned long fLastTimeoutMS;

    /// The last value specified in the call to Start(), to be used in Restart().
    TimerMode fLastTimerMode;

    /// My assigned name.
    char *pfName;

    /// Controls message logging.
    BcmMessageLogSettings fMessageLogSettings;

private:

    /// Copy Constructor.  Not supported.
    BcmTimer(BcmTimer &otherInstance);

    /// Assignment operator.  Not supported.
    BcmTimer & operator = (BcmTimer &otherInstance);

};


//********************** Inline Method Implementations ***********************


/// Returns the event that is triggered when the timer expires.  This is
/// primarily used by BcmEventSet when the timer is added to the list of
/// events that are to be waited on.
///
/// \return
///      The timer's event.
///
inline BcmEvent *BcmTimer::GetEvent(void) const
{
    return pfEventToTrigger;
}


/// An alternate way to get the pointer to the event.  You can use it this
/// way:
///
/// \code
///   BcmTimer *pTimer;
///   BcmEvent *pEvent;
///
///   pEvent = (*pTimer);
/// \endcode
///
/// \return
///      The event associated with the timer.
///
inline BcmTimer::operator BcmEvent * () const
{
    return pfEventToTrigger; 
}


/// Simple accessor for the name of this object.
///
/// \return
///      The pointer to the name string.
///
inline const char *BcmTimer::Name(void) const
{
    return pfName;
}


#endif



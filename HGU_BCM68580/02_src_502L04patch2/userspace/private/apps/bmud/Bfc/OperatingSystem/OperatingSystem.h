//****************************************************************************
//
// Copyright (c) 1999-2008 Broadcom Corporation
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
//  Filename:       OperatingSystem.h
//  Author:         David Pullen
//  Creation Date:  Feb 11, 1999
//
//****************************************************************************

#ifndef OPERATINGSYSTEM_H
#define OPERATINGSYSTEM_H

//********************** Include Files ***************************************

#include "typedefs.h"

#include "MessageLog.h"

//********************** Global Types ****************************************

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Forward Declarations ********************************

class BcmThread;

// These are the abstract base classes for the OS components.
class BcmEvent;
class BcmEventSet;
class BcmTimer;
class BcmMutexSemaphore;
class BcmCountingSemaphore;
class BcmMessageQueue;

//********************** Class Declaration ***********************************

/**   \ingroup OsWrappers
*
*     The BcmOperatingSystem class contains the core OS state and methods
*     that are associated with threads but which cannot be contained in
*     BcmThread if it is to be OS-agnostic.  BcmOperatingSystem derived
*     classes contain the OS-specific implementations for controlling a
*     thread's state, as well as methods to create OS-specific instances of
*     other objects (BcmEvent, BcmTimer, etc.).  This class is the thread's
*     main hook into the Operating System.
*/
class BcmOperatingSystem
{
public:

    typedef enum
    {
        kNotStarted,            /// The thread object has been created, but is
                                /// not yet running.
        kInitializing,          /// The thread has been spawned, and is creating
                                /// its OS objects or doing other initialization.
        kRunning,               /// The thread is running in its ThreadMain, and
                                /// is schedulable.
        kSuspended,             /// The thread is running in its ThreadMain, but
                                /// is non-schedulable (suspended).
        kDeinitializing,        /// The thread has exited its ThreadMain, and is
                                /// destroying its OS object or doing other
                                /// teardown.
        kTerminated,            /// The thread has finished deinitializing, is no
                                /// longer running, and can be re-started.
        kInoperable             /// The thread object is being destroyed and can
                                /// no longer be operated on.
    } ThreadState;

    /// To be completed...don't know whether I should use a number 0..255, or just
    /// abstract names like this.  How many different values are really needed?
    /// How much granularity?
    ///
    /// Win32 and pSOS are radically different; how do I resolve this?
    ///
    /// I think I'll just provide symbolic names and leave it to the OS-specific
    /// classes to make sense of it.
    typedef enum
    {
        kIdlePriority = 1,      /// Should be reserved for threads that just need
                                /// whatever CPU cycles are left over (like Console).
                                /// Note that this is different (and higher) than
                                /// the operating system's built-in idle thread
                                /// priority, which runs only when nothing else
                                /// is running.

        kLowPriority,           /// These 5 should be used for most application
        kLowNormalPriority,     /// threads.  This should given enough levels
        kMediumLowNormalPriority,
        kNormalPriority,        /// of differentiation for most application needs.
        kMediumHighNormalPriority,
        kHighNormalPriority,
        kHighPriority,

        kTimeCriticalPriority,  /// Should be reserved for threads that absolutely
                                /// must run when scheduled (like VoIP data
                                /// related threads); similar to an ISR, but with
                                /// interrupts enabled.

        kNumberOfPriorities
    } ThreadPriority;

    /// These are used by BcmThread in the ThreadEntrypoint() method; they tell
    /// it whether it should delete any existing OS objects first (on restart
    /// or terminate), and whether it should skip running the thread main method
    /// (on terminate).
    typedef enum
    {
        kFirstStartup,
        kRestart,
        kTerminate
    } ThreadEntrypointMode;

    /// Default constructor.
    ///
    BcmOperatingSystem(void);

    /// Destructor.  Cleans things up and kills the thread if it is running.
    ///
    virtual ~BcmOperatingSystem();

    /// This is a helper method that allows any of the OS Wrapper objects (event,
    /// timer, etc.) to create its name string for debugging purposes.  The name
    /// string is either a copy of the string that is passed in, or a unique
    /// name generated by an incrementing number if the string is empty or NULL.
    ///
    /// The returned string has been "newed" from the heap, and is owned by the
    /// caller, who must delete it when it is no longer needed.
    ///
    /// This method will generally be called by the base class constructor for
    /// BcmEvent, BcmTimer, etc.
    ///
    /// \param
    ///      pName - pointer to the name string to be used.
    /// \param
    ///      pClassName - name of the class that is calling this method (e.g.
    ///                   "Event" or "Timer").
    ///
    /// \return
    ///      A pointer to the string that was created.
    ///
    static char *GenerateName(const char *pName, const char *pClassName);

    /// -----------------------------------------------------------------------
    /// EXTERNAL THREAD CONTROL METHODS:
    ///      These methods allow any thread to control any other thread's
    ///      state (including its own state).
    /// -----------------------------------------------------------------------
    
    /// Causes the thread to stop executing (blocked) until Resume() is called.
    /// A thread can suspend itself or another thread, though it cannot resume
    /// itself - some other thread must release it to run again.
    ///
    /// This only works if the thread's current state is kRunning.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem.
    ///
    virtual bool Suspend(void) = 0;

    /// Causes a thread that had been blocked to be released from its suspended
    /// state.  Note that it may or may not begin running immediately upon being
    /// released, depending on the thread's priority level.
    ///
    /// This only works if the thread's current state is kSuspended.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem.
    ///
    virtual bool Resume(void) = 0;
    
protected:

    /// NOTE:  I'm having a hard time implementing these two methods in a uniform
    ///        way on all operating systems.  In particular, Win32 doesn't allow
    ///        one thread to do these to another thread cleanly.  It only allows
    ///        a thread to terminate itself.
    ///
    ///        For now, I will disallow the use of these methods (by making them
    ///        protected) until someone can prove that they are necessary.  Then
    ///        I'll worry about how to implement them in Win32.

    /// Causes a thread to be terminated in its tracks.  This is generally not
    /// recommended because it may not give the thread a chance to free memory
    /// and other resources that were allocated.  You should only call this under
    /// catostrophic conditions where the system needs to be shut down or there
    /// is a rogue thread that won't respond to events, etc.
    ///
    /// Unfinished...this may not be true.  I suspect that this can be done
    /// gracefully by setting an internal state and restarting the thread, which
    /// causes it to de-init and exit.
    ///
    /// This only works if the thread's current state is kRunning or kSuspended.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem (what would cause this?).
    ///
    ///virtual bool Terminate(void) = 0;
    
    /// Allows a thread that has exited to be restarted.  In general, we don't
    /// recommend this - instead, delete the original thread, allowing it to
    /// exit gracefully, then create a new one.
    ///
    /// Unfinished...this may not be true.  I suspect that this can be done
    /// gracefully by setting an internal state and restarting the thread, which
    /// causes it to de-init, then start over again.
    ///
    /// This only works if the thread's current state is kRunning or kSuspended.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem (what would cause this?).
    ///
    ///virtual bool Restart(void) = 0;

public:

    /// Sets the priority of the thread to the value specified.
    ///
    /// This only works if the thread's current state is kRunning or kSuspended.
    ///
    /// \param
    ///      priority - the new thread priority to use.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem.
    ///
    virtual bool SetPriority(ThreadPriority priority) = 0;

    /// Increases the priority of the thread by one level, if possible.
    ///
    /// This only works if the thread's current state is kRunning or kSuspended.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem.
    ///
    bool IncreasePriority(void);

    /// Decreases the priority of the thread by one level, if possible.
    ///
    /// This only works if the thread's current state is kRunning or kSuspended.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem.
    ///
    bool DecreasePriority(void);

    /// Returns the current thread priority.
    ///
    /// \return
    ///      The current thread priority.
    ///
    inline ThreadPriority GetPriority(void) const;

public:

    /// PR11084 - Bits for use in SetCpuAffinity(); for now, we only support 2
    /// CPUs.
    enum
    {
        kThreadAffinityPrimaryCpu = 0x01,
        kThreadAffinitySecondCpu  = 0x02,
    };

    /// For a CPU that has multiple cores (such as BMIPS4350), and an OS that
    /// supports SMP (such as eCos), this method allows you to set the CPU
    /// affinity for the thread.
    ///
    /// By default, a thread has its affinity set to the "primary" or master
    /// CPU.  It can be useful to move a thread to the "secondary" CPU, though
    /// doing so introduces another level of reentrancy issues that have to
    /// be addressed, so this should be done only after careful consideration.
    ///
    /// A thread can be configured to run on multiple CPUs; again, this should
    /// only be done after careful consideration.
    ///
    /// \note Not all OSs support SMP; this base class provides a dummy
    ///       implementation for these OSs which prints a warning and returns
    ///       false.
    ///
    /// Added for PR11084.
    ///
    /// \param
    ///     cpuAffinityMask - a bitmask indicating which CPUs the thread should
    ///                       be allowed to run on.  Bit 0 is always the primary
    ///                       CPU.
    ///
    /// \retval
    ///     true if the CPU affinity was set.
    /// \retval
    ///     false if the CPU affinity was not set (CPU doesn't have multiple
    ///     cores, OS doesn't support SMP, invalid bitmask, etc).
    ///
    virtual bool SetCpuAffinity(uint32 cpuAffinityMask);

public:

    /// -----------------------------------------------------------------------
    /// INTERNAL THREAD CONTROL METHODS:
    ///      These methods allow a thread to control its own state.  They should
    ///      not be called by one thread to control another thread.
    /// -----------------------------------------------------------------------
    
    /// This method spawns the thread, vectoring in to the thread entrypoint
    /// method.
    ///
    /// This method should only be called by the BcmThread derived class, usually
    /// at the end of the constructor.  It may also be called by the
    /// BcmOperatingSystem class under some circumstances.
    ///
    /// \param
    ///      pThread - pointer to the thread that is to be started.
    /// \param
    ///      stackSize - the number of bytes to be allocated for the thread's
    ///                  stack.
    /// \param
    ///      usesFPU - tells whether or not the thread uses the Floating Point
    ///                Unit.  If the thread is known not to use the FPU, then
    ///                setting this to false can reduce the overhead in
    ///                thread context switching (on some operating systems).
    ///
    /// \retval
    ///      true if successful (the thread is running).
    /// \retval
    ///      false if there was a problem.
    ///
    virtual bool BeginThread(BcmThread *pThread,
                             unsigned long stackSize = 8 * 1024,
                             bool usesFPU = true) = 0;

    /// This method allows a thread to put itself to sleep for the specified
    /// number of milliseconds, giving up control of the processor.
    ///
    /// Note that a timeout value of 0 may not cause the thread to sleep at all.
    ///
    /// \param
    ///      timeoutMS - the number of milliseconds to sleep.
    ///
    virtual void Sleep(unsigned long timeoutMS) = 0;

    /// This method ensures that the thread is allowed to run without being
    /// preempted.  Depending on the OS, this may boost the thread's priority,
    /// and may event turn off interrupts.
    ///
    /// The thread MUST call AllowPreemption() within a reasonable amount of time
    /// so as not to starve other threads for too long.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem.
    ///
    virtual bool SurpressPreemption(void) = 0;

    /// This is the opposite of SurpressPreemption(); it restores the thread's
    /// state back to the way it was.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem.
    ///
    virtual bool AllowPreemption(void) = 0;

    /// This lets the thread update the state information for events that are
    /// beyond the control of this class (like kInitializing, etc.).
    ///
    /// \param
    ///      newState - the thread's new state.
    ///
    void SetThreadState(ThreadState newState);

    /// This lets the thread retrieve its state information.
    ///
    /// \return
    ///     The current thread state.
    ///
    inline ThreadState GetThreadState(void) const;

    /// -----------------------------------------------------------------------
    /// FACTORY METHODS:
    ///      These methods create operating system components (events,
    ///      semaphores, etc.).
    /// -----------------------------------------------------------------------

    /// This method creates an OS-specific event for the current thread.  The
    /// event that is returned is owned by (and must be deleted by) the caller
    /// when it is no longer needed.
    ///
    /// \param
    ///      pName - the text name that is to be given to the object, for
    ///              debugging purposes.
    ///
    /// \return
    ///      Pointer to the event that was created, or NULL if there was a
    ///      problem creating the event.
    ///
    virtual BcmEvent *NewEvent(const char *pName = NULL) = 0;

    /// This method creates an OS-specific event set for the current thread.
    /// The event set that is returned is owned by (and must be deleted by) the
    /// caller when it is no longer needed.
    ///
    /// \param
    ///      pName - the text name that is to be given to the object, for
    ///              debugging purposes.
    ///
    /// \return
    ///      Pointer to the event set that was created, or NULL if there was a
    ///      problem creating the event set.
    ///
    virtual BcmEventSet *NewEventSet(const char *pName = NULL) = 0;

    /// This method creates an OS-specific timer for the current thread.  The
    /// timer that is returned is owned by (and must be deleted by) the caller
    /// when it is no longer needed.
    ///
    /// \param
    ///      pName - the text name that is to be given to the object, for
    ///              debugging purposes.
    ///
    /// \return
    ///      Pointer to the timer that was created, or NULL if there was a
    ///      problem creating the timer.
    ///
    virtual BcmTimer *NewTimer(const char *pName = NULL) = 0;

    /// This method creates an OS-specific mutex semaphore for the current
    /// thread.  The semaphore that is returned is owned by (and must be deleted
    /// by) the caller when it is no longer needed.
    ///
    /// \param
    ///      pName - the text name that is to be given to the object, for
    ///              debugging purposes.
    ///
    /// \return
    ///      Pointer to the semaphore that was created, or NULL if there was a
    ///      problem creating the semaphore.
    ///
    virtual BcmMutexSemaphore *NewMutexSemaphore(const char *pName = NULL) = 0;
    
    /// This method creates an OS-specific counting semaphore for the current
    /// thread.  The semaphore that is returned is owned by (and must be deleted
    /// by) the caller when it is no longer needed.
    ///
    /// \param
    ///      isBounded - set this to true if you want the semaphore to have
    ///                  a maximum count beyond which it cannot be incremented.
    ///                  Set it to false if you don't want a maximum count.
    /// \param
    ///      maxCount - the max count to be used, if isBounded is true.  This
    ///                 is ignored if isBounded is false.
    /// \param
    ///      initialCount - the count that the semaphore should start off with.
    /// \param
    ///      pName - the text name that is to be given to the object, for
    ///              debugging purposes.
    ///
    /// \return
    ///      Pointer to the semaphore that was created, or NULL if there was a
    ///      problem creating the semaphore.
    ///
    virtual BcmCountingSemaphore *NewCountingSemaphore(bool isBounded,
                                                       unsigned int maxCount,
                                                       unsigned int initialCount,
                                                       const char *pName = NULL) = 0;

    /// This method creates an OS-specific message queue for the current thread.
    /// The message queue that is returned is owned by (and must be deleted by)
    /// the caller when it is no longer needed.
    ///
    /// \param
    ///      pName - the text name that is to be given to the object, for
    ///              debugging purposes.
    ///
    /// \return
    ///      Pointer to the message queue that was created, or NULL if there was
    ///      a problem creating the message queue.
    ///
    virtual BcmMessageQueue *NewMessageQueue(const char *pName = NULL) = 0;

protected:

    /// Tells the current state of the thread.
    ThreadState fCurrentThreadState;

    /// Tells the current priority of the thread.
    ThreadPriority fCurrentPriority;

    /// The thread to whom I belong.  Derived classes may need to use this in
    /// certain methods (typically Restart() and Terminate()).
    BcmThread *pfThread;

    /// Controls message logging.
    BcmMessageLogSettings fMessageLogSettings;

private:

    /// Copy constructor.  Not supported.
    BcmOperatingSystem(BcmOperatingSystem &otherInstance);

    /// Assignment operator.  Not supported.
    BcmOperatingSystem & operator = (BcmOperatingSystem &otherInstance);

};


//********************** Inline Method Implementations ***********************


/// Returns the current thread priority.
///
/// \return
///      The current thread priority.
///
inline BcmOperatingSystem::ThreadPriority BcmOperatingSystem::GetPriority(void) const
{
    return fCurrentPriority;
}


/// This lets the thread retrieve its state information.
///
/// \return
///     The current thread state.
///
inline BcmOperatingSystem::ThreadState BcmOperatingSystem::GetThreadState(void) const
{
    return fCurrentThreadState;
}


// Special doxygen documentation tag for the OS Wrapper objects.
/** \defgroup OsWrappers BFC OS Wrappers Component

This document describes Broadcom Corporation's Operating System Abstraction
Layer Classes (OS Wrappers), which is a core component of the BFC System.
These classes provide all of the common resources of most modern embedded
operating systems.  All components of the BFC System (and derived applications)
use these abstract base classes when making calls to the operating system,
rather than calling the OS directly or calling the OS-specific derived classes.

It is important to note that we are not attempting to implement an operating
system!  These classes simply provide a uniform interface to various operating
systems, hiding the gory details of a given OS from the application.  The goal
of these classes is to make the application OS-agnostic, allowing faster and
more robust porting of applications to new operating systems.

An application based on the BFC framework will have one
BcmOperatingSystemFactory, and usually at least one BcmThread.  The BcmThread(s)
each have a BcmOperatingSystem instance, which will be used to create other OS
objects as needed.

\image html OsWrappers.png "High Level OS Wrappers Relationship Diagram"

One note about time:  Different operating systems tend to use different units of
time - Win32 uses milliseconds, VxWorks uses "ticks", etc.  Most protocols, and
most programmers, really prefer to specify time in units of milliseconds.  Thus,
in all calls to these classes where a time value is required, the units of time
are specified in milliseconds.  Each OS-specific derived class then translates
this value into the native timebase as needed.

<H2>Scenario diagrams for BcmThread</H2>

Note:  The horizontal dashed lines in the scenario diagrams indicate a context
switch to a different thread.

<H3>Scenario:  Starting a thread</H3>

This scenario shows an example of one thread (main in this case) creating and
starting another thread (the console thread).

\image html OsWrappers_Scenario_Thread1_Objects.png "Object relationship diagram for starting a thread"

Note that the object constructor for BcmConsoleThread runs in the context of the
thread that creates it; BcmThread::Initialize() is the first time the object has
a chance to run in the context of the new thread.  Also note that the main
thread is blocked until Initialize() completes.

\image html OsWrappers_Scenario_Thread1.png "Scenario diagram for starting a thread"

<H3>Scenario:  A thread exiting after completing work</H3>

In some cases, and thread can be created (as shown above), will run and do some
work, then will complete the work and exit.

\image html OsWrappers_Scenario_Thread2.png "Scenario diagram for a thread exiting"

It is important to note that the object associated with the thread still exists,
even though the thread itself has exited.  The object must be deleted by some
external entity in order to release the memory for the object.

<H3>Scenario:  Stopping a thread by deleting it</H3>

While a thread can exit on its own (as shown above), it is more common for the
thread to continue running until told to exit.  This is usually done within the
destructor of the thread object.  The thread must implement some mechanism for
signalling the need to exit (often an event or a message) which is used in the
destructor.  Then, the destructor must wait for the thread to exit before
continuing with normal object destruction.

\image html OsWrappers_Scenario_Thread3.png "Scenario diagram for deleting a thread"

<H2>Scenario diagram for BcmEvent</H2>

In this scenario, thread 1 is waiting for one of several event to occur (via a
BcmEventSet object).  Thread 2 sends the event to thread 1, which wakes it up
and allows it to run.

\image html OsWrappers_Scenario_Event_Objects.png "Object relationship diagram for events"

\image html OsWrappers_Scenario_Event.png "Scenario diagram for events"

<H2>Scenario diagram for BcmTimer</H2>

In this scenario, thread 1 has a timer that is started so that it will expire in
1000 ms.  The thread iswaiting for one of several event to occur (via a
BcmEventSet object), including the event associated with the timer.  The timer
expires and causes the thread to wake up and run.

\image html OsWrappers_Scenario_Timer_Objects.png "Object relationship diagram for timers"

\image html OsWrappers_Scenario_Timer.png "Scenario diagram for timers"

<H2>Scenario diagram for BcmMutexSemaphore</H2>

Thread1 contends with Thread2 for access to a shared object, using a mutex
semaphore to control access.

\image html OsWrappers_Scenario_Mutex_Objects.png "Object relationship diagram for mutex semaphores"

\image html OsWrappers_Scenario_Mutex.png "Scenario diagram for mutex semaphores"

Note, in the scenario shown above, that the shared object manages the mutex on
behalf of the threads that use it, thus eliminating the need for each thread to
get a reference to the mutex and manipulate it themselves.  Each thread can just
call the methods of the shared object as though it weren't shared.

This also makes it easier for the threads to deal with the case where there are
multiple shared objects - the threads only have to keep track of the objects
themselves, and each shared object is responsible for managing its own mutex
instance.

<H2>Scenario diagram for BcmMessageQueue</H2>

Thread2 sends a message to Thread1, who is waiting for messages on its queue.

\image html OsWrappers_Scenario_MessageQueue_Objects.png "Object relationship diagram for message queues"

\image html OsWrappers_Scenario_MessageQueue.png "Scenario diagram for message queues"

If a message were already waiting in the queue, then the middle part of the
scenario (where Thread2 is running) would be skipped, and Thread1 would return
from the call to Receive() immediately.

*/

#endif



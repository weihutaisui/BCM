//****************************************************************************
//
// Copyright © 2007 Broadcom Corporation
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
//  Filename:       
//  Author:         
//  Creation Date:  
//
//****************************************************************************
//
//  Filename:       LnxOperatingSystem.h
//  Author:         John McQueen
//  Creation Date:  June 30, 2000
//
//****************************************************************************
//  Description:
//      This is the Linux Operating System class implementation.
//
//****************************************************************************

#ifndef LNXOPERATINGSYSTEM_H
#define LNXOPERATINGSYSTEM_H

//********************** Include Files ***************************************

#include <pthread.h>
#include <semaphore.h>

// My base class.
#include "OperatingSystem.h"

//********************** Global Types ****************************************

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Forward Declarations ********************************

//********************** Class Declaration ***********************************


class BcmLnxOperatingSystem : public BcmOperatingSystem
{
public:

    // Default Constructor.
    //
    // Parameters:  None.
    //
    // Returns:  N/A
    //
    BcmLnxOperatingSystem(void);

    // Destructor.
    //
    // Parameters:  N/A
    //
    // Returns:  N/A
    //
    virtual ~BcmLnxOperatingSystem();

    // This is a helper method that is used by several objects; pSOS lets you
    // specify a 4 character (not ASCII-Z) name for all OS objects.  This
    // method extracts that name from any string (which is usually the object
    // name).
    //
    // Note that if pFullStringName is shorter than 4 bytes, or is NULL, then
    // this method fills the unused characters in pPsosString with 0's.
    //
    // Parameters:
    //      pFullStringName - the full object name to be converted.
    //      pPsosString - the 4 byte pSOS name to be produced.
    //
    // Returns:  Nothing.
    //

    static void ConvertStringToLnxName(const char *pFullStringName,
                                        char pLnxString[4]);

    // This is a helper method that converts time values from milliseconds to
    // ticks (used by all of the Linux calls).
    //
    // Note that the conversion can result in overflow or underflow; i.e. if
    // the tick precision is 10ms and a value of 5ms is specified, the result
    // would underflow to 0.  In this case, we will clamp to 1 tick.
    //
    // If the value would overflow (larger than 0xffffffff ticks), then this
    // method returns false rather than clamping.  This is so that you don't
    // get a false sense of security and set the timer to a smaller value than
    // requested.
    //
    // Parameters:
    //      timeMs - the time value (in milliseconds) to be converted.
    //      timeTicks - the converted time value (in ticks).  This is only
    //                  valid if the return value is true!
    //
    // Returns:
    //      true if the converted value is correct.
    //      false if the converted value would have been clamped, and is not
    //          correct.
    //
    static bool ConvertMsToTicks(unsigned long timeMs, unsigned long &timeTicks);



    // -----------------------------------------------------------------------
    // EXTERNAL THREAD CONTROL METHODS:
    //      These methods allow any thread to control any other thread's
    //      state (including its own state).
    // -----------------------------------------------------------------------
    
    // Causes the thread to stop executing (blocked) until Resume() is called.
    // A thread can suspend itself or another thread, though it cannot resume
    // itself - some other thread must release it to run again.
    //
    // This only works if the thread's current state is kRunning.
    //
    // Parameters:  None.
    //
    // Returns:
    //      true if successful.
    //      false if there was a problem.
    //
    virtual bool Suspend(void);
    
    // Causes a thread that had been blocked to be released from its suspended
    // state.  Note that it may or may not begin running immediately upon being
    // released, depending on the thread's priority level.
    //
    // This only works if the thread's current state is kSuspended.
    //
    // Parameters:  None.
    //
    // Returns:
    //      true if successful.
    //      false if there was a problem.
    //
    virtual bool Resume(void);
    
protected:

    // NOTE:  I'm having a hard time implementing these two methods in a uniform
    //        way on all operating systems.  In particular, Win32 doesn't allow
    //        one thread to do these to another thread cleanly.  It only allows
    //        a thread to terminate itself.
    //
    //        For now, I will disallow the use of these methods (by making them
    //        protected) until someone can prove that they are necessary.  Then
    //        I'll worry about how to implement them in Win32.

    // Causes a thread to be terminated in its tracks.  This is generally not
    // recommended because it may not give the thread a chance to free memory
    // and other resources that were allocated.  You should only call this under
    // catostrophic conditions where the system needs to be shut down or there
    // is a rogue thread that won't respond to events, etc.
    //
    // Unfinished...this may not be true.  I suspect that this can be done
    // gracefully by setting an internal state and restarting the thread, which
    // causes it to de-init and exit.
    //
    // This only works if the thread's current state is kRunning or kSuspended.
    //
    // Parameters:  None.
    //
    // Returns:
    //      true if successful.
    //      false if there was a problem (what would cause this?).
    //
    virtual bool Terminate(void);
    
    // Allows a thread that has exited to be restarted.  In general, we don't
    // recommend this - instead, delete the original thread, allowing it to
    // exit gracefully, then create a new one.
    //
    // Unfinished...this may not be true.  I suspect that this can be done
    // gracefully by setting an internal state and restarting the thread, which
    // causes it to de-init, then start over again.
    //
    // This only works if the thread's current state is kRunning or kSuspended.
    //
    // Parameters:  None.
    //
    // Returns:
    //      true if successful.
    //      false if there was a problem (what would cause this?).
    //
    virtual bool Restart(void);

public:

    // Sets the priority of the thread to the value specified.
    //
    // This only works if the thread's current state is kRunning or kSuspended.
    //
    // Parameters:
    //      priority - the new thread priority to use.
    //
    // Returns:
    //      true if successful.
    //      false if there was a problem.
    //
    virtual bool SetPriority(ThreadPriority priority);

    // -----------------------------------------------------------------------
    // INTERNAL THREAD CONTROL METHODS:
    //      These methods allow a thread to control its own state.  They should
    //      not be called by one thread to control another thread.
    // -----------------------------------------------------------------------
    
    // This method spawns the thread, vectoring in to the thread entrypoint
    // methods.
    //
    // This method should generally only be called by the BcmThread derived
    // class constructor after it has had a chance to initialize.
    //
    // Parameters:
    //      *pThread - pointer to the thread that is to be started.
    //      stackSize - the number of bytes to be allocated for the thread's
    //                  stack.
    //      usesFPU - tells whether or not the thread uses the Floating Point
    //                Unit.  If the thread is known not to use the FPU, then
    //                setting this to false can reduce the overhead in
    //                thread context switching (on some operating systems).
    //
    // Returns:  Nothing.
    //
    virtual bool BeginThread(BcmThread *pThread,
                             unsigned long stackSize = 8 * 1024,
                             bool usesFPU = true);

    // This method allows a thread to put itself to sleep for the specified
    // number of milliseconds, giving up control of the processor.
    //
    // Note that a timeout value of 0 may not cause the thread to sleep at all.
    //
    // Parameters:
    //      timeoutMS - the number of milliseconds to sleep.
    //
    // Returns:  Nothing.
    //
    virtual void Sleep(unsigned long timeoutMS);

    // This method allows a thread to put itself to sleep for the specified
    // number of milliseconds, giving up control of the processor.
    //
    // Note that a timeout value of 0 may not cause the thread to sleep at all.
    //
    // This one is called by the OperatingSystem Factory class.
    //
    // Parameters:
    //      timeoutMS - the number of milliseconds to sleep.
    //
    // Returns:  Nothing.
    //
    static void SleepImpl(unsigned long timeoutMS);

    // This method ensures that the thread is allowed to run without being
    // preempted.  Depending on the OS, this may boost the thread's priority,
    // and may event turn off interrupts.
    //
    // The thread MUST call AllowPreemption() within a reasonable amount of time
    // so as not to starve other threads for too long.
    //
    // Parameters:  None.
    //
    // Returns:
    //      true if successful.
    //      false if there was a problem.
    //
    virtual bool SurpressPreemption(void);

    // This is the opposite of SurpressPreemption(); it restores the thread's
    // state back to the way it was.
    //
    // Parameters:  None.
    //
    // Returns:
    //      true if successful.
    //      false if there was a problem.
    //
    virtual bool AllowPreemption(void);

    // -----------------------------------------------------------------------
    // FACTORY METHODS:
    //      These methods create operating system components (events,
    //      semaphores, etc.).
    // -----------------------------------------------------------------------

    // This method creates an OS-specific event for the current thread.  The
    // event that is returned is owned by (and must be deleted by) the caller
    // when it is no longer needed.
    //
    // Parameters:
    //      pName - the text name that is to be given to the object, for
    //              debugging purposes.
    //
    // Returns:
    //      Pointer to the event that was created, or NULL if there was a
    //      problem creating the event.
    //
    virtual BcmEvent *NewEvent(const char *pName = NULL);

    // This method creates an OS-specific event set for the current thread.
    // The event set that is returned is owned by (and must be deleted by) the
    // caller when it is no longer needed.
    //
    // Parameters:
    //      pName - the text name that is to be given to the object, for
    //              debugging purposes.
    //
    // Returns:
    //      Pointer to the event set that was created, or NULL if there was a
    //      problem creating the event set.
    //
    virtual BcmEventSet *NewEventSet(const char *pName = NULL);

    // This method creates an OS-specific timer for the current thread.  The
    // timer that is returned is owned by (and must be deleted by) the caller
    // when it is no longer needed.
    //
    // Parameters:
    //      pName - the text name that is to be given to the object, for
    //              debugging purposes.
    //
    // Returns:
    //      Pointer to the timer that was created, or NULL if there was a
    //      problem creating the timer.
    //
    virtual BcmTimer *NewTimer(const char *pName = NULL);

    // This method creates an OS-specific mutex semaphore for the current
    // thread.  The semaphore that is returned is owned by (and must be deleted
    // by) the caller when it is no longer needed.
    //
    // Parameters:
    //      pName - the text name that is to be given to the object, for
    //              debugging purposes.
    //
    // Returns:
    //      Pointer to the semaphore that was created, or NULL if there was a
    //      problem creating the semaphore.
    //
    virtual BcmMutexSemaphore *NewMutexSemaphore(const char *pName = NULL);
    
    // This method creates an OS-specific counting semaphore for the current
    // thread.  The semaphore that is returned is owned by (and must be deleted
    // by) the caller when it is no longer needed.
    //
    // Parameters:
    //      isBounded - set this to true if you want the semaphore to have
    //                  a maximum count beyond which it cannot be incremented.
    //                  Set it to false if you don't want a maximum count.
    //      maxCount - the max count to be used, if isBounded is true.  This
    //                 is ignored if isBounded is false.
    //      initialCount - the count that the semaphore should start off with.
    //      pName - the text name that is to be given to the object, for
    //              debugging purposes.
    //
    // Returns:
    //      Pointer to the semaphore that was created, or NULL if there was a
    //      problem creating the semaphore.
    //
    virtual BcmCountingSemaphore *NewCountingSemaphore(bool isBounded,
                                                       unsigned int maxCount,
                                                       unsigned int initialCount,
                                                       const char *pName = NULL);

    // This method creates an OS-specific message queue for the current thread.
    // The message queue that is returned is owned by (and must be deleted by)
    // the caller when it is no longer needed.
    //
    // Parameters:
    //      pName - the text name that is to be given to the object, for
    //              debugging purposes.
    //
    // Returns:
    //      Pointer to the message queue that was created, or NULL if there was
    //      a problem creating the message queue.
    //
    virtual BcmMessageQueue *NewMessageQueue(const char *pName = NULL);

    // This method sets the priority of the current thread to the highest
	// within the subsystem, so no other threads in the subsystem would be able
	// to preempt the current one.
    //
    // Parameters: None
    //
    // Returns:
    //      old priority of the current task.
    //
	static int DisablePreemption(void);

    // This method restores the old priority of the current thread. So other 
	// threads in the subsystem would be able to preempt the current one.
    //
    // Parameters: 
	//		oldPriority:	the old priority of the current task.
    //
    // Returns: None
    //
	static void EnablePreemption(int oldPriority);

protected:

private:

    // Returns the next available event bit for pSOS events.  If all of the
    // available events have been used, then it asserts (pSOS only allows 16
    // user events per thread; pSOS 2.5.0 may allow 32).
    //
    // This method is generally only called by NewEvent (and related methods)
    // to get the event bit for the object that is being created.
    //
    // Parameters:  None.
    //
    // Returns:
    //      The Linux event bit.
    //
    uint32 GetNextEventBit(void);

    // Gives the specified event bit back to the thread; this is called by the
    // destructor of BcmPSOSEvent.
    //
    // Parameters:
    //      event - the event bit to be returned for reuse.
    //
    // Returns:  Nothing.
    //
    void ReturnEventBit(uint32 event);

    // This declares BcmLnxEvent as a friend of this class, giving it full
    // access to ReturnEventBit(), even though it is declares as private.
    friend class BcmLnxEvent;

private:

    // Stores the semaphore suspend and resume.
    sem_t fSuspendSem;

    // Stores the Linux task id for this thread.
    pthread_t fThreadId;

    // Stores the Linux attributes for this thread.
    pthread_attr_t fThreadAttr;

    // The priorities used for suppression preemption.
    ThreadPriority fCurrentThreadPriority;
    ThreadPriority fOriginalThreadPriority;
    bool fPreemptionSuppressed;

    // Stores the bit mask for the next available event bit.
    uint32 fAvailableEventBits;

private:

    // Copy Constructor.  Not supported.
    //
    // Parameters:
    //      otherInstance - reference to the object to copy.
    //
    // Returns:  N/A
    //
    BcmLnxOperatingSystem(BcmLnxOperatingSystem &otherInstance);

    // Assignment operator.  Not supported.
    //
    // Parameters:
    //      otherInstance - reference to the object to copy.
    //
    // Returns:
    //      A reference to "this" so that operator = can be chained.
    //
    BcmLnxOperatingSystem & operator = (BcmLnxOperatingSystem &otherInstance);

};


//********************** Inline Method Implementations ***********************

#endif



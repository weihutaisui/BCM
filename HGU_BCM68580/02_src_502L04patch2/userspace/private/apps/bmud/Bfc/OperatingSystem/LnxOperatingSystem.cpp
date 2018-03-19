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
//  Filename:       LnxOperatingSystem.cpp
//  Author:         John McQueen
//  Creation Date:  June 30, 2000
//
//****************************************************************************
//  Description:
//      This is the Linux Operating System class implementation.
//
//****************************************************************************

//********************** Include Files ***************************************

// My api and definitions...
#include "LnxOperatingSystem.h"

// I use this to create a thread-less counting semaphore.
#include "OperatingSystemFactory.h"

// Operating system objects that I can create.
#include "LnxEvent.h"
#include "LnxEventSet.h"
#include "LnxTimer.h"
#include "LnxMutexSemaphore.h"
#include "LnxCountingSemaphore.h"
#include "LnxMessageQueue.h"

#include "Thread.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

//********************** Local Types *****************************************

//********************** Local Constants *************************************
 
// These are the Linux thread priority values that correspond to the portable
// ThreadPriority values.  Linux priorities go from 1 to 99.
static const unsigned long gPriorityConversionTable[BcmOperatingSystem::kNumberOfPriorities] =
{
     0,
    10,

    50,  // I could choose a lower value, but I don't think it's necessary.
    60,
    75,  // Space them apart by 5 to allow for future expansion...(it
    80,  // doesn't matter whether the spacing is 5 or 1 as far as Linux
    85,  // is concerned).
    90,
    95,

    99   // This is the highest value that application tasks can use.
};

//********************** Local Variables *************************************

//********************** Local Functions *************************************

static void ThreadEntrypoint(unsigned long *threadParm);

//********************** Global Functions ************************************
extern "C" 
{
	void RegisterThreadInfo(char *name);
}
//********************** Class Method Implementations ************************


// Default Constructor.  Initializes the state of the object...
//
// Parameters:  None.
//
// Returns:  N/A
//
BcmLnxOperatingSystem::BcmLnxOperatingSystem(void)
{
    // Override the class name given by my parent.
    fMessageLogSettings.SetModuleName("BcmLnxOperatingSystem");

    fMessageLogSettings.SetInstanceName("Linux OS");

    fMessageLogSettings.Register();

    // This is filled in when BeginThread() is called.
    fThreadId = 0;

    // pSOS 2.2.2 only allows the lower 16 bits to be used by the application.
    // Can I use more with pSOS 2.5.0?
    //
    // The docs only state the 16-bit limitation in one place (for timers, tm_*
    // functions).  Since a timer uses a BcmPSOSEvent, I'll have to limit all
    // events to use the lower 16 bits, even if the thread doesn't use timers.
    fAvailableEventBits = 0x0000ffff;

    // We start of with Preemption not suppressed and Priorities set to normal.
    fPreemptionSuppressed = false;
    fCurrentThreadPriority = kNormalPriority;
    fOriginalThreadPriority = kNormalPriority;

    // Initialize my semaphore for Suspend and Resume.
    sem_init(&fSuspendSem, 0, 0);
}


// Destructor.  Frees up any memory/objects allocated, cleans up internal
// state.
//
// Parameters:  N/A
//
// Returns:  N/A
//
BcmLnxOperatingSystem::~BcmLnxOperatingSystem()
{
    fMessageLogSettings.Deregister();

    fThreadId = 0;
    fAvailableEventBits = 0;
    sem_destroy(&fSuspendSem);
}


// This is a helper method that is used by several objects; vxWorks lets you
// specify a 4 character (not ASCII-Z) name for all OS objects.  This
// method extracts that name from any string (which is usually the object
// name).
//
// Note that if pFullStringName is shorter than 4 bytes, or is NULL, then
// this method fills the unused characters in pLnxString with 0's.
//
// Parameters:
//      pFullStringName - the full object name to be converted.
//      pLnxString - the 4 byte vxWorks name to be produced.
//
// Returns:  Nothing.
//
void BcmLnxOperatingSystem::ConvertStringToLnxName(const char *pFullStringName,
                                                            char pLnxString[4])
{
    // If the string is NULL, then make it an empty string.
    if (pFullStringName == NULL)
    {
        pFullStringName = "";
    }

    // Copy bytes, possibly propogating the \0 string terminator if necessary.
    for (int offset = 0; offset < 4; offset++)
    {
        pLnxString[offset] = *pFullStringName;

        if (*pFullStringName != '\0')
        {
            pFullStringName++;
        }
    }
}

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
bool BcmLnxOperatingSystem::ConvertMsToTicks(unsigned long timeMs, unsigned long &timeTicks)
{
    // Translate the ms timeout value to ticks, as required by vxWorks.  The
    // equation is:
    //      ticks = ms * KC_TICKS2SEC / 1000

    // Need to be careful - if the ms value is big, then we need to divide
    // first, then multiply (preventing overflow).  If the ms value is small,
    // then we need to multiply first, then divide (preserving as much precision
    // as possible).
    // TPS - This should no longer be called.
    // Need to remove this once it is verified that this is no longer needed.
#ifdef NOOP
    if (timeMs > (0xffffffff / KC_TICKS2SEC))
    {
        timeMs /= 1000;
        timeTicks = timeMs * KC_TICKS2SEC;

        // Check for overflow.
        if (timeTicks < timeMs)
        {
            return false;
        }
    }
    else
    {
        timeTicks = timeMs * KC_TICKS2SEC;

        // Check for overflow.
        if (timeTicks < timeMs)
        {
            return false;
        }
        
        timeTicks /= 1000;
    }

    // Need to be careful - if the timeout value (in ticks) is now 0 because of
    // truncation due to integer math, then I need to force it to be 1 tick.
    if (timeTicks == 0)
    {
        timeTicks = 1;
    }

#endif
    return true;
}


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
bool BcmLnxOperatingSystem::Suspend(void)
{ 
    
    // Quick sanity check...
    if (fThreadId == 0)
    {
        // Log the error...
        gErrorMsg(fMessageLogSettings, "Suspend")
            << "Task id is invalid (0)!  Can't suspend it!" << endl;

        return false;
    }

    // Make sure the state is correct.
    if (fCurrentThreadState != kRunning)
    {
        // Log the error...
        gErrorMsg(fMessageLogSettings, "Suspend")
            << "The thread state is not running!  Can't suspend it!" << endl;

        return false;
    }

    // I may need to protect this against preemption...
    fCurrentThreadState = kSuspended;

    // Call the Linux function to do the work.  If a thread is suspending 
    // itself, then this will not return until some other thread resumes it.
    sem_wait(&fSuspendSem);

    return true; 
}


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
bool BcmLnxOperatingSystem::Resume(void)
{ 
    int rc = 0;

    // Quick sanity check...
    if (fThreadId == 0)
    {
        // Log the error...
        gErrorMsg(fMessageLogSettings, "Resume")
            << "Task id is invalid (0)!  Can't resume it!" << endl;

        return false;
    }

    // Make sure the task state is correct.
    if (fCurrentThreadState != kSuspended)
    {
        // Log the error...
        gErrorMsg(fMessageLogSettings, "Resume")
            << "The thread state is not suspended!  Can't resume it!" << endl;

        return false;
    }

    // Do this first, so that the thread's state will be correct if it is a
    // higher priority and gets a chance to run as soon as I resume it.
    fCurrentThreadState = kRunning;

    // Call the Linux function to do the work.  If this is successful, then the
    // task is running.
    rc = sem_post(&fSuspendSem);

    if (rc != 0)
    {
        // Set this back since the resume failed.
        fCurrentThreadState = kSuspended;

        // Log the error...
        gErrorMsg(fMessageLogSettings, "Resume")
            << "Failed to resume the thread!  " << (void *) rc << endl;

        return false;
    }
    return true; 
}


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
bool BcmLnxOperatingSystem::Terminate(void)
{ 
    // Quick sanity check...
    if ((fThreadId == 0) || (pfThread == NULL))
    {
        // Log the error...
        gErrorMsg(fMessageLogSettings, "Terminate")
            << "Task id is 0 or thread pointer is NULL!  Can't terminate it!" << endl;

        return false;
    }

    // Make sure the task state is correct.
    if ((fCurrentThreadState != kSuspended) &&
        (fCurrentThreadState != kRunning))
    {
        // Log the error...
        gErrorMsg(fMessageLogSettings, "Terminate")
            << "Thread isn't running or suspended!  Can't terminate it!" << endl;

        return false;
    }

    // Set the thread's state so that it knows that it needs to terminate
    // (deleting all OS resources, etc.).
    pfThread->SetThreadEntrypointMode(kTerminate);

    // Delete the thread.
    pthread_exit(0);

    return true; 
}


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
bool BcmLnxOperatingSystem::Restart(void)
{ 
    // TPS - Restart is not used by any code.  I've leaving the code
    // here commented out, but no one calls this anyway.
    // Need to remove this once it is verified that this is never called.
    gErrorMsg(fMessageLogSettings, "Restart")
        << "Restart not implemented for linux!" << endl;
#ifdef NOOP
    int rc;
    unsigned int arguments[4];

    // Quick sanity check...
    if ((fThreadId == 0) || (pfThread == NULL))
    {
        // Log the error...
        gErrorMsg(fMessageLogSettings, "Restart")
            << "Task id is 0 or thread pointer is NULL!  Can't restart it!" << endl;

        return false;
    }

    // Make sure the task state is correct.
    if ((fCurrentThreadState != kSuspended) &&
        (fCurrentThreadState != kRunning))
    {
        // Log the error...
        gErrorMsg(fMessageLogSettings, "Restart")
            << "Thread isn't running or suspended!  Can't restart it!" << endl;

        return false;
    }

    // Set the thread's state so that it knows that it needs to restart
    // (deleting all OS resources, then creating them again, etc.).
    pfThread->SetThreadEntrypointMode(kRestart);

    // Set up the parameters that are to be sent to the task entrypoint.
    memset(arguments, 0, sizeof(arguments));
    arguments[0] = (unsigned long) pfThread;
    
    // Restart the thread.  It will then delete the appropriate resources, 
    // then start over as though this were the first call.
    rc = t_restart(fThreadId, arguments);

    if (rc != 0)
    {
        // Log the error...
        gErrorMsg(fMessageLogSettings, "Restart")
            << "Failed to restart the thread!  " << (void *) rc << endl;

        return false;
    }

#endif
    //return true; 
    return false; 
}


// Sets the priority of the thread to the value specified.
//
// This only works if the thread's current state is kRunning or kSuspended.
//
// Note - For linux, it must be noted that after the priority has been 
//        changed, it will not preempt a lower priority thread.  In other
//        words, you set a thread to a higher priority, and it is waiting
//        on data supplied by an interrupt.  When the interrupt comes, the
//        thread will be made ready, and will execute before lower priority
//        threads, but it must wait for the currently running, possibly 
//        lower priority thread, to suspend.
//
// Parameters:
//      priority - the new thread priority to use.
//
// Returns:
//      true if successful.
//      false if there was a problem.
//
bool BcmLnxOperatingSystem::SetPriority(ThreadPriority priority)
{ 
    int rc = 0;
    int newPriority;
    int thdpolicy;
    struct sched_param thdparam;

    // Quick sanity check...
    if (fThreadId == 0)
    {
        // Log the error...
        gErrorMsg(fMessageLogSettings, "SetPriority")
            << "Task id is invalid (0)!  Can't set priority!" << endl;

        return false;
    }

    // Make sure the task state is correct.
    if ((fCurrentThreadState != kSuspended) &&
        (fCurrentThreadState != kRunning))
    {
        // Log the error...
        gErrorMsg(fMessageLogSettings, "SetPriority")
            << "The thread state is not suspended or running!  Can't set priority!" << endl;

        return false;
    }

    // Store this for future reference.
    fCurrentThreadPriority = priority;

    // Convert the priority units from the portable values to the Lnx-specific
    // values.
    newPriority = gPriorityConversionTable[priority];

    // Call the Linux functions to do the work.
    // Get the current settings.
    rc = pthread_getschedparam(fThreadId, &thdpolicy, &thdparam);

    // Set the priority.
    thdparam.sched_priority = newPriority;
    rc = pthread_setschedparam(fThreadId, thdpolicy, &thdparam);

    if (rc != 0)
    {
        // Log the error...
        gErrorMsg(fMessageLogSettings, "SetPriority")
            << "Failed to set thread priority!  " << (void *) rc << endl;

        return false;
    }

    // Log the change...
    fCurrentPriority = priority;

	// Comment this our later on ...
//    gAlwaysMsg(fMessageLogSettings, "SetPriority")
//        << pfThread->Name() << " (tid=0x" << hex << fThreadId << ") has a priority of " << dec << newPriority << endl;
    return true;
}


// -----------------------------------------------------------------------
// INTERNAL THREAD CONTROL METHODS:
//      These methods allow a thread to control its own state.  They should
//      not be called by one thread to control another thread.
// -----------------------------------------------------------------------

// This is the function that is called by BeginThread() via the Lnx t_start()
// API routine.  My parameter will always be a pointer to a BcmThread object,
// and my job is to call its ThreadEntrypoint() method.
//
// This is necessary because t_start can't call a class method; it must call a
// normal C function.
//
// Parameters:
//      threadParm - pointer to the BcmThread instance to be called.
//      semaphoreParm - pointer to the BcmCountingSemaphore instance to be used
//                      as the parameter to the thread's entrypoint.
//
// Returns:  Nothing.
//
static void ThreadEntrypoint(unsigned long *threadParm)
{ 
    BcmLnxOperatingSystem pOpSys;
    // Extract the parameters to their true types.
    BcmThread *pThread = (BcmThread *) threadParm[0];
    BcmCountingSemaphore *pSemaphore = (BcmCountingSemaphore *) threadParm[1];

    // This should be replace with a semaphore get.
    pOpSys.Sleep(50); 
    // The thread pointer must NOT be NULL, but the semaphore can be NULL.
    if (pThread == NULL)
    {
        // UNFINISHED - may want to do this differently...
        //
        // Log the error...note that this is not suppressable, since I don't
        // have a BcmMessageLogSettings instance.  I'm using the message log
        // ostream directly.
        if (pgMessageLogStream != NULL)
        {
            *pgMessageLogStream << "BcmLnxOperatingSystem::ThreadEntrypoint:  ERROR - thread pointer is NULL!  Can't start the thread!" << endl;
        }
    }
    else
    {
		
		RegisterThreadInfo((char *)pThread->Name());
		// Vector into the BcmThread object.  When it returns, the thread can
        // exit.
        pThread->ThreadEntrypoint(pSemaphore);
    }

    // Delete myself...
    pthread_exit(0);
}


// This method spawns the thread, vectoring in to the thread entrypoint
// methods.
//
// This method should only be called by the BcmThread derived class, usually
// at the end of the constructor.  It may also be called by the
// BcmOperatingSystem class under some circumstances.
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
bool BcmLnxOperatingSystem::BeginThread(BcmThread *pThread, 
                                         unsigned long stackSize,
                                         bool usesFPU)
{
    int rc;
    struct sched_param tparam;
    unsigned int arguments[4];
    BcmCountingSemaphore *pSemaphore;
    bool result;

    // Quick sanity check...
    if (pThread == NULL)
    {
        // Log the error...
        gErrorMsg(fMessageLogSettings, "BeginThread")
            << "Thread pointer is NULL!  Can't begin the thread..." << endl;

        return false;
    }

    // Make sure the thread is in the right state.
    if ((fCurrentThreadState != kNotStarted) &&
        (fCurrentThreadState != kTerminated))
    {
        // Log the error...
        gErrorMsg(fMessageLogSettings, "BeginThread")
            << "Thread state is incorrect!  Can't be started..." << endl;

        return false;
    }

    // Create a counting semaphore, using the OS Factory's static method to do
    // this so that I can get a semaphore that isn't associated with a thread.
    // It isn't bounded and the initial count is 0 (so that I will wait until
    // the thread releases it).
    pSemaphore = BcmOperatingSystemFactory::NewCountingSemaphore(false, 0, 0,
                                                                 "Thread Initialization Semaphore");

    if (pSemaphore == NULL)
    {
        // Log the error.
        gErrorMsg(fMessageLogSettings, "BeginThread")
            << "Failed to create counting semaphore!" << endl;

        return false;
    }

    // Store this for future reference.
    pfThread = pThread;

    // Set the state.
    fCurrentThreadState = kNotStarted;

    // Make sure the thread has the right mode for startup.
    pThread->SetThreadEntrypointMode(kFirstStartup);

    // Set the Linux parameters for creating a task.  Note that the initial task
    // priority is quite high!  This allows the task to run immediately and
    // initialize itself.  Once initialized, the task will set its own priority
    // down to the level supplied in the constructor.
    tparam.sched_priority = gPriorityConversionTable[kTimeCriticalPriority];
    pthread_attr_init(&fThreadAttr);
    pthread_attr_setschedpolicy(&fThreadAttr, SCHED_FIFO);
    pthread_attr_setschedparam(&fThreadAttr, &tparam);
    pthread_attr_setscope(&fThreadAttr, PTHREAD_SCOPE_SYSTEM);
    pthread_attr_setstacksize(&fThreadAttr, stackSize);
	// I have seen different default values for inheritsched, so set it!
	pthread_attr_setinheritsched(&fThreadAttr, PTHREAD_EXPLICIT_SCHED);

    // Set up the parameters that are to be sent to the task entrypoint.
    memset(arguments, 0, sizeof(arguments));
    arguments[0] = (unsigned long) pThread;
    arguments[1] = (unsigned long) pSemaphore;

    // Get Linux to create core resources for the task.
    rc = pthread_create(&fThreadId, &fThreadAttr, (void * (*) (void *))ThreadEntrypoint, arguments);

    if (rc != 0)
    {
        // Log the error...
        gErrorMsg(fMessageLogSettings, "BeginThread")
            << "Failed to create and start the thread!  " << (void *) rc << endl;

        // Clean up the core Lnx resources.
        fCurrentThreadState = kNotStarted;
        pfThread = NULL;
        fThreadId = 0;
        delete pSemaphore;

        return false;
    }
	pthread_detach(fThreadId);

    // Wait for the task to initialize itself before returning.
    result = pSemaphore->Get();

    if (result == false)
    {
        // Log the problem.
        gErrorMsg(fMessageLogSettings, "BeginThread")
            << "Failed to wait for the semaphore!  Thread may not be fully initialized..." << endl;
    }

    // Now I can get rid of the semaphore I created.
    delete pSemaphore;

    return true;
}


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
void BcmLnxOperatingSystem::Sleep(unsigned long timeoutMS)
{
	BcmLnxOperatingSystem::SleepImpl(timeoutMS);
}


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
void BcmLnxOperatingSystem::SleepImpl(unsigned long timeoutMS)
{
    unsigned long beginTimeMS, endTimeMS;
	unsigned long timeElapsedMS;

	if(timeoutMS == 0)
	{
		usleep(0);
		return;
	}

    endTimeMS = beginTimeMS = SystemTimeMS();

    // Call the sleep function.
	timeElapsedMS = 0;
    while (timeElapsedMS < timeoutMS) 
	{
		// Sleep for the time left
		// The Linux usleep function deals with microseconds.
		// Therefore we must mulitply timeoutMS by 1000.
		usleep((timeoutMS - timeElapsedMS) *1000);
		// What time is it now
	    endTimeMS = SystemTimeMS();
		// Have we slept long enough
		if(endTimeMS >= beginTimeMS)
		{
			timeElapsedMS = endTimeMS - beginTimeMS;
		}
		else
		{
			timeElapsedMS = (0xffffffff - beginTimeMS + endTimeMS);
		}
    }
}


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
bool BcmLnxOperatingSystem::SurpressPreemption(void)
{
    // There isn't a way to do this directly, that I can find, in Linux.
    // I have to do this indirectly by setting the thread's priority
    // to kTimeCriticalPriority.  The old priority is saved so it can
    // be restored later.  A flag is set so that this isn't called 
    // multiple times.

    if (fPreemptionSuppressed == false)
    {
        fOriginalThreadPriority = fCurrentThreadPriority;
        fPreemptionSuppressed = SetPriority(kTimeCriticalPriority);
    }
    
    return fPreemptionSuppressed;
}


// This is the opposite of SurpressPreemption(); it restores the thread's
// state back to the way it was.
//
// Parameters:  None.
//
// Returns:
//      true if successful.
//      false if there was a problem.
//
bool BcmLnxOperatingSystem::AllowPreemption(void)
{
    bool result = false;

    // If preemption has been suppressed, then restore the original
    // priority.
    if (fPreemptionSuppressed == true)
    {
        result = SetPriority(fOriginalThreadPriority);
        if(result);
        {
            fPreemptionSuppressed = false;
        }
    }
    return result;
}


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
BcmEvent *BcmLnxOperatingSystem::NewEvent(const char *pName)
{
    BcmLnxEvent *pEvent;
    uint32 eventBit;

    if (fThreadId == 0)
    {
        // Log the error...
        gErrorMsg(fMessageLogSettings, "NewEvent")
            << "Task id is invalid (0)!  Can't create an event for it!" << endl;

        return NULL;
    }

    eventBit = GetNextEventBit();

    if (eventBit == 0)
    {
        // Log the error...
        gErrorMsg(fMessageLogSettings, "NewEvent")
            << "No more event bits available!  Can't create an event." << endl;

        return NULL;
    }

    pEvent = new BcmLnxEvent(pName);

    return pEvent;
}


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
BcmEventSet *BcmLnxOperatingSystem::NewEventSet(const char *pName)
{
    BcmLnxEventSet *pEventSet;

    if (fThreadId == 0)
    {
        // Log the error...
        gErrorMsg(fMessageLogSettings, "NewEventSet")
            << "Task id is invalid (0)!  Can't create an event set for it!" << endl;

        return NULL;
    }

    pEventSet = new BcmLnxEventSet(pName);

    return pEventSet;
}


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
BcmTimer *BcmLnxOperatingSystem::NewTimer(const char *pName)
{
    BcmLnxTimer *pTimer;

    if (fThreadId == 0)
    {
        // Log the error...
        gErrorMsg(fMessageLogSettings, "NewTimer")
            << "Task id is invalid (0)!  Can't create a timer for it!" << endl;

        return NULL;
    }

    pTimer = new BcmLnxTimer(pName);

    return pTimer;
}


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
BcmMutexSemaphore *BcmLnxOperatingSystem::NewMutexSemaphore(const char *pName)
{
    BcmLnxMutexSemaphore *pMutex;

    if (fThreadId == 0)
    {
        // Log the error...
        gErrorMsg(fMessageLogSettings, "NewMutexSemaphore")
            << "Task id is invalid (0)!  Can't create a mutex for it!" << endl;

        return NULL;
    }

    pMutex = new BcmLnxMutexSemaphore(pName);

    return pMutex;
}


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
BcmCountingSemaphore *BcmLnxOperatingSystem::NewCountingSemaphore(bool isBounded,
                                                                   unsigned int maxCount,
                                                                   unsigned int initialCount,
                                                                   const char *pName)
{
    BcmEvent *pEvent;
    BcmLnxCountingSemaphore *pSemaphore;

    if (fThreadId == 0)
    {
        // Log the error...
        gErrorMsg(fMessageLogSettings, "NewCountingSemaphore")
            << "Task id is invalid (0)!  Can't create a counting semaphore for it!" << endl;

        return NULL;
    }

    // First create the event object that is associated with the semaphore (thus
    // reserving an event bit for the it).  Then create the semaphore object.
    // It will delete the event object when it is deleted, and it will also
    // handle pEvent being NULL if there was a problem.
    pEvent = NewEvent("Counting Semaphore's Event");

    pSemaphore = new BcmLnxCountingSemaphore(isBounded, maxCount, initialCount,
                                              pEvent, pName);

    return pSemaphore;
}


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
BcmMessageQueue *BcmLnxOperatingSystem::NewMessageQueue(const char *pName)
{
    BcmLnxMessageQueue *pMessageQueue;

    if (fThreadId == 0)
    {
        // Log the error...
        gErrorMsg(fMessageLogSettings, "NewMessageQueue")
            << "Task id is invalid (0)!  Can't create a message queue for it!" << endl;

        return NULL;
    }

    pMessageQueue = new BcmLnxMessageQueue(pName);

    return pMessageQueue;
}


// JRM - Look at vxWorks implementation.
// Returns the next available event bit for PSOS events.  If all of the
// available events have been used, then it asserts (Vx only allows 16
// user events per thread; PSOS 2.5.0 may allow 32).
//
// This method is generally only called by NewEvent (and related methods)
// to get the event bit for the object that is being created.
//
// Parameters:  None.
//
// Returns:
//      The Lnx event bit.
//
uint32 BcmLnxOperatingSystem::GetNextEventBit(void)
{
    uint32 event;

    // Find the first available bit.
    event = 0x01;
    while (((fAvailableEventBits & event) == 0) && (event != 0))
    {
        event <<= 1;
    }

    // Make sure this yielded a valid bit.
    if (event == 0)
    {
        // Log the error...
        gErrorMsg(fMessageLogSettings, "GetNextEventBit")
             << "No more event bits available for this thread!" << endl;
    }

    // Mark the bit as unavailable.
    fAvailableEventBits &= ~event;

    // Return the found bit.
    return event;
}


// Gives the specified event bit back to the thread; this is called by the
// destructor of BcmLnxEvent.
//
// Parameters:
//      event - the event bit to be returned for reuse.
//
// Returns:  Nothing.
//
void BcmLnxOperatingSystem::ReturnEventBit(uint32 event)
{
    // Make sure this bit was actually allocated for use.
    if ((fAvailableEventBits & event) != 0)
    {
        // Log the error...
        gErrorMsg(fMessageLogSettings, "ReturnEventBit")
            << "Specified event bit already available!  Can't return it!" << endl;

        return;
    }

    // Just mask it back in, making sure that it's in range (the lower 16
    // bits).
    fAvailableEventBits |= (event & 0xffff);
}

// This method sets the priority of the current thread to the highest
// within the subsystem, so no other threads in the subsystem would be able
// to preempt the current one.
//
// Parameters: None
//
// Returns:
//      old priority of the current task.
//
int BcmLnxOperatingSystem::DisablePreemption(void)
{
    int rc = 0;
    int thdpolicy;
    int oldPriority;
    struct sched_param thdparam;
	pthread_t threadId = pthread_self();

    // Get the current settings.
    rc = pthread_getschedparam(threadId, &thdpolicy, &thdparam);
	oldPriority = thdparam.sched_priority;

    // Set the priority.
    thdparam.sched_priority = 99;
    rc = pthread_setschedparam(threadId, thdpolicy, &thdparam);

    if (rc != 0)
    {
        // Log the error...
        gLogMessageRaw << "Failed to boost thread priority!  " << (void *) rc << endl;
    }

    return oldPriority;
}

// This method restores the old priority of the current thread. So other 
// threads in the subsystem would be able to preempt the current one.
//
// Parameters: 
//		oldPriority:	the old priority of the current task.
//
// Returns: None
//
void BcmLnxOperatingSystem::EnablePreemption(int oldPriority)
{
    int rc = 0;
    int thdpolicy;
    struct sched_param thdparam;
	pthread_t threadId = pthread_self();

    // Get the current settings.
    rc = pthread_getschedparam(threadId, &thdpolicy, &thdparam);

    // Set the priority.
    thdparam.sched_priority = oldPriority;
    rc = pthread_setschedparam(threadId, thdpolicy, &thdparam);
    if (rc != 0)
    {
        // Log the error...
        gLogMessageRaw << "Failed to lower thread priority!  " << (void *) rc << endl;
    }
}


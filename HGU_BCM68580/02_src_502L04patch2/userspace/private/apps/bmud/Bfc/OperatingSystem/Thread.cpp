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
//  Filename:       Thread.cpp
//  Author:         David Pullen
//  Creation Date:  Feb 22, 1999
//
//****************************************************************************
//  Description:
//      This is the base class from which all application threads are derived.
//      It handles most of the guts of creating and starting a new thread in an
//      OS-agnostic way, and provides common storage and functionality required
//      by all threads.
//
//      A thread is a client of the operating system components, but is agnostic
//      with respect to any particular OS.  As a result, there are no OS-specific
//      derived classes of BcmThread; there are only application-specific derived
//      classes.
//
//      Threads maintain their independence from any particular OS by only making
//      use of the abstract base classes of objects like BcmEvent, BcmTimer, etc.
//
//****************************************************************************

//********************** Include Files ***************************************

// My api and definitions...
#include "Thread.h"

// The OS factory class.
#include "OperatingSystemFactory.h"

// For notifying other threads of significant events (thread initialized, thread
// terminated, etc.).
#include "CountingSemaphore.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

//********************** Local Types *****************************************

//********************** Local Constants *************************************

//********************** Local Variables *************************************

//********************** Class Method Implementations ************************


// Constructor.  Initializes the state of the object, acquiring a
// BcmOperatingSystem instance for later use, and setting up required state
// information.
//
// Parameters:
//      pThreadName - the name of this thread (for debugging purposes).
//      startSuspended - set to true if the thread should suspend itself
//                       after initializing, or false if it should run.
//      initialPriority - the thread priority that should be used.
//
// Returns:  N/A
//
BcmThread::BcmThread(const char *pThreadName, bool startSuspended,
                     BcmOperatingSystem::ThreadPriority initialPriority) :
    fMessageLogSettings("BcmThread")
{
    // Store these for later use.
    fStartSuspended = startSuspended;
    fInitialPriority = initialPriority;
    fThreadEntrypointMode = BcmOperatingSystem::kFirstStartup;
    
    pfThreadName = BcmOperatingSystem::GenerateName(pThreadName, "Thread");

    // Set my instance name.
    fMessageLogSettings.SetInstanceName(pfThreadName);

    fMessageLogSettings.Register();

    // Get my instance of the operating system.
    pfOperatingSystem = BcmOperatingSystemFactory::NewOperatingSystem();

    // Sanity check...
    if (pfOperatingSystem == NULL)
    {
        gFatalErrorMsg(fMessageLogSettings, "BcmThread") << "Failed to create OS instance!" << endl;

        assert(0);
    }

    pfOperatingSystem->SetThreadState(BcmOperatingSystem::kNotStarted);
}


// Destructor.  If the thread is still running, it will be stopped, allowing
// OS objects to be destroyed (note - this is actually done by the derived
// class when it calls WaitForThread()).
//
// Note that WaitForThread() MUST be called by the destructor of the derived
// class rather than here!  This is because once the base class destructor
// is called, the data members and virtual methods in the derived class are
// no longer valid!
//
// Parameters:  N/A
//
// Returns:  N/A
//
BcmThread::~BcmThread()
{
    fMessageLogSettings.Deregister();

    // Quick sanity check of my state.  If the thread hasn't been shut down
    // properly, then all sorts of nastiness could ensue when it runs using
    // code/data that no longer exist!
    if (pfOperatingSystem->GetThreadState() != BcmOperatingSystem::kInoperable)
    {
        gFatalErrorMsg(fMessageLogSettings, "~BcmThread")
             << "Thread has not stopped running!  Derived class failed to call WaitForThread()!" << endl;
    
        assert(0);
    }

    // Get rid of my operating system instance.
    delete pfOperatingSystem;

    // Free the memory for the thread name.
    delete pfThreadName;
    pfThreadName = NULL;
    
    // Zero out some members.
    pfOperatingSystem = NULL;
}


// NOTE:  This method should only be called by BcmOperatingSystem (and
//        derived classes)!
//
// This is the main thread entrypoint that is called by the
// BcmOperatingSystem once the thread has started.  It handles basic thread
// setup and configuration, calling the Initialize(), ThreadMain(), and
// Deinitialize() methods.  It sets the initial thread priority, and
// suspends the thread after call Initialize() (if specified in the
// constructor).
//
// When this method exits, the thread has been cleaned up and stopped.
//
// Parameters:
//      pSemaphore - pointer to an optional counting semaphore.  The thread
//                   will call Release() once it has finished initializing.
//
// Returns:  Nothing.
//
void BcmThread::ThreadEntrypoint(BcmCountingSemaphore *pSemaphore)
{
    bool result;

    // If my mode is set to restart or terminate, then I need to destroy any
    // existing OS objects first.
    if ((fThreadEntrypointMode == BcmOperatingSystem::kRestart) ||
        (fThreadEntrypointMode == BcmOperatingSystem::kTerminate))
    {
        // Let the derived class destroy its OS classes and otherwise tear down
        // before the thread stops running for good.
        pfOperatingSystem->SetThreadState(BcmOperatingSystem::kDeinitializing);
        Deinitialize();
    }

    // If my mode is set to first startup or restart, then I need to do "normal"
    // thread startup processing.
    if ((fThreadEntrypointMode == BcmOperatingSystem::kFirstStartup) ||
        (fThreadEntrypointMode == BcmOperatingSystem::kRestart))
    {
        // Let the derived class initialize itself, creating its OS classes,
        // etc.
        pfOperatingSystem->SetThreadState(BcmOperatingSystem::kInitializing);
        result = Initialize();

        // If it had a problem, then don't run the main thread; instead, just
        // let the derived class tear down and then exit.
        if (result)
        {
            // I need to do this first, so that I can set my priority and then
            // possibly suspend myself (these are valid only when the state is
            // Running or Suspended).
            pfOperatingSystem->SetThreadState(BcmOperatingSystem::kRunning);

            // Set my priority.
            //
            // PR7368 - we need to defer this until after we release the
            // semaphore.  If my new thread priority is lower than normal,
            // and another thread is running continuously (usually some network
            // driver), then we'll never make it to the pSemaphore->Release()
            // call, and I'll never be able to let the spawning thread run
            // again.
            //pfOperatingSystem->SetPriority(fInitialPriority);

            // If the caller specified a counting semaphore, then I need to call
            // Release() in order to notify him that I have finished
            // initializing.
            if (pSemaphore != NULL)
            {
                // Protect this against preemption; I know that the thread that
                // created and spawned me is waiting on this semaphore.  When
                // I release it, he may start running and delete the semaphore
                // before I have a chance to exit the call to Release(), which
                // could cause a memory exception.
                pfOperatingSystem->SurpressPreemption();

                result = pSemaphore->Release();

                pfOperatingSystem->AllowPreemption();

                if (result == false)
                {
                    // Log the error...
                    gErrorMsg(fMessageLogSettings, "ThreadEntrypoint")
                        << "Failed to release semaphore to let parent thread run!" << endl;
                }
            }

            // Set my priority.
            //
            // PR7368 - now that we have released the semaphore, we can reduce
            // the thread priority.
            pfOperatingSystem->SetPriority(fInitialPriority);

            if (fStartSuspended)
            {
                // Suspend myself.
                pfOperatingSystem->Suspend();
            }

            ThreadMain();
        }
        
        // PR 14276:  If Initialize fails, we must now release the calling
        // thread's semaphore in order to keep the calling thread from hanging
        // up forever.
        else
        {
            // Log the error...
            gErrorMsg(fMessageLogSettings, "ThreadEntrypoint")
                << "Initialize call failed!  Thread will not be started." << endl;
        
            // If the caller specified a counting semaphore, then I need to call
            // Release() in order to notify him that I have finished
            // initializing.
            if (pSemaphore != NULL)
            {
                // Protect this against preemption; I know that the thread that
                // created and spawned me is waiting on this semaphore.  When
                // I release it, he may start running and delete the semaphore
                // before I have a chance to exit the call to Release(), which
                // could cause a memory exception.
                pfOperatingSystem->SurpressPreemption();

                result = pSemaphore->Release();

                pfOperatingSystem->AllowPreemption();

                if (result == false)
                {
                    // Log the error...
                    gErrorMsg(fMessageLogSettings, "ThreadEntrypoint")
                        << "Failed to release semaphore to let parent thread run!" << endl;
                }
            }
        }

        // Let the derived class destroy its OS classes and otherwise tear down
        // before the thread stops running for good.
        pfOperatingSystem->SetThreadState(BcmOperatingSystem::kDeinitializing);
        Deinitialize();
    }

    // Now we're done.
    pfOperatingSystem->SetThreadState(BcmOperatingSystem::kTerminated);
}


// This method MUST be called by the destructor of the most derived class!
// It does all of the real work to ensure that the thread has exited
// correctly and has had a chance to delete all of its OS objects.
//
// Parameters:  None.
//
// Returns:  Nothing.
//
void BcmThread::WaitForThread(void)
{
    // If the thread has already stopped, then just bail.
    if (pfOperatingSystem->GetThreadState() == BcmOperatingSystem::kInoperable)
    {
        gInfoMsg(fMessageLogSettings, "WaitForThread") << "Thread already stopped..." << endl;

        return;
    }

    if (pfOperatingSystem->GetThreadState() == BcmOperatingSystem::kInitializing)
    {
        gInfoMsg(fMessageLogSettings, "WaitForThread") << "Waiting for thread to initialize before I stop it..." << endl;
    
        // If the thread is initializing, then I need to wait for it to finish
        // before I can continue.
        while (pfOperatingSystem->GetThreadState() == BcmOperatingSystem::kInitializing)
        {
            pfOperatingSystem->Sleep(50);
        }

        gInfoMsg(fMessageLogSettings, "WaitForThread") << "Thread is initialized..." << endl;
    }

    // If the thread is currently suspended, resume it so that it can do exit
    // processing.  It is up to the derived class to figure out how to tell the
    // thread to exit.
    if (pfOperatingSystem->GetThreadState() == BcmOperatingSystem::kSuspended)
    {
        pfOperatingSystem->Resume();
    }

    // Wait for the thread to exit.
    {
        unsigned int waitCount = 0;

        gInfoMsg(fMessageLogSettings, "WaitForThread") << "Waiting for thread to terminate..." << endl;

        // Wait for the termination to complete.
        while (pfOperatingSystem->GetThreadState() != BcmOperatingSystem::kTerminated)
        {
            waitCount++;

            if ((waitCount * 50) > 1000)
            {
                waitCount = 0;

                gAlwaysMsg(fMessageLogSettings, "WaitForThread") << "Waiting for thread to terminate..." << endl;
            }

            pfOperatingSystem->Sleep(50);
        }
    
        gInfoMsg(fMessageLogSettings, "WaitForThread") << "Thread is terminated..." << endl;
    }

    // Make sure nothing else can be done to me.
    pfOperatingSystem->SetThreadState(BcmOperatingSystem::kInoperable);
}
    

// ========================================================================
// These methods are called by the BcmThread base class (ThreadEntrypoint())
// once the thread has been spawned.
// ========================================================================

// Thread constructor - this is the first method called after the thread has
// been spawned, and is where the thread should create all OS objects.  This
// has to be done here, rather than in the object's constructor, because
// some OS objects must be created in the context of the thread that will
// use them.  The object's constructor is still running in the context of
// the thread that created this object.
//
// The default implementation simply returns true, allowing derived classes
// that don't have any initialization to use the default.
//
// Parameters:  None.
//
// Returns:
//      true if successful and ThreadMain() should be called.
//      false if there was a problem (couldn't create an OS object, etc.)
//          and ThreadMain() should not be called.
//
bool BcmThread::Initialize(void)
{
    // By default, we return true so that the derived class doesn't have to
    // provide an implementation if there is no thread initialization
    // necessary.  This allows the thread to begin running.
    return true;
}


// Thread destructor - this is the last method called when the thread is
// exiting, and is where the thread should delete all of the OS objects that
// it created.
//
// The default implementation does nothing, allowing derived classes that
// don't have any deinitialization to use the default.
//
// Parameters:  None.
//
// Returns:  Nothing.
//
void BcmThread::Deinitialize(void)
{
    // Nothing to do!
}



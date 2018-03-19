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
//  Filename:       OperatingSystemFactory.cpp
//  Author:         David Pullen
//  Creation Date:  Feb 23, 1999
//
//****************************************************************************
//  Description:
//      This class is responsible for creating the correct type of
//      BcmOperatingSystem, based on compiler or other environment settings.
//
//      This class just has a single static method; you don't need to create
//      an instance of this class.
//
//****************************************************************************

//********************** Include Files ***************************************

// My api and definitions...
#include "OperatingSystemFactory.h"

// The types of BcmOperatingSystems that I know how to create.
#if defined(WIN32)

#include "Win32OperatingSystem.h"
#include "Win32MutexSemaphore.h"
#include "Win32CountingSemaphore.h"

#elif defined(PSOS)       

#include "PSOSOperatingSystem.h"
#include "PSOSMutexSemaphore.h"
#include "PSOSCountingSemaphore.h"

#elif defined(TARGETOS_vxWorks)

#include "vxOperatingSystem.h"
#include "vxMutexSemaphore.h"
#include "vxCountingSemaphore.h"

#include "taskLib.h"     

#include <exception>

//#elif defined(__QNX__)        // old school QNX 4 (not supported, this is ancient CMTS stuff)
#elif defined(TARGETOS_Qnx6)    // Qnx Neutrino v6.x (the new school)

#include "QnxOperatingSystem.h"
#include "QnxMutexSemaphore.h"
#include "QnxCountingSemaphore.h"

#elif defined(__linux__)       

#include "LnxOperatingSystem.h"
#include "LnxMutexSemaphore.h"
#include "LnxCountingSemaphore.h"

#elif defined(TARGETOS_eCos)

#include "ecosOperatingSystem.h"
#include "ecosMutexSemaphore.h"
#include "ecosCountingSemaphore.h"

#include <cyg/kernel/kapi.h>

#else

#error Unknown Operating System - unable to build OperatingSystemFactory.cpp!

#endif

//********************** Local Types *****************************************

//********************** Local Constants *************************************

//********************** Local Variables *************************************

//********************** Local Functions *************************************

#if defined(PSOS)

static void PsosTaskStartCallout(void *pNull, CO_INFO *pCalloutInfo);

#elif defined(TARGETOS_vxWorks)

// Let this be global rather than static so that people can find this function
// in the map file when they see the address of the crash.
void VxWorksTerminateCallout(void);
    
#endif

//********************** Class Method Implementations ************************


// Main() should call this function before there are any calls to other
// operating system facilities (including other functions of this class). 
// This function may initialize critical elements that allow the operating
// environment to work properly.
//
// Parameters:  None.
//
// Returns:  Nothing.
//
void BcmOperatingSystemFactory::InitializeOperatingSystem(void)
{
    #if defined(WIN32)

        // anything needed here?

    #elif defined(PSOS)

    // DPullen - register my own task start callout routine.  This will let
    // me seed the stacks of all tasks that are created (except IDLE, ROOT,
    // and possibly PNAD), and do other interesting task start processing.
    ULONG dummy;
    co_register(CO_START, PsosTaskStartCallout, NULL, &dummy);

    // Loop over all tasks already created, and seed them manually.
    CO_INFO calloutInfo;

    // IDLE, ROOT and PNAD are always the first 3 tasks created, so their task
    // ID's are consistent.
    calloutInfo.tid = 0x00010000;
    PsosTaskStartCallout(NULL, &calloutInfo);

    calloutInfo.tid = 0x00020000;
    PsosTaskStartCallout(NULL, &calloutInfo);

    calloutInfo.tid = 0x00030000;
    PsosTaskStartCallout(NULL, &calloutInfo);

    #elif defined(TARGETOS_vxWorks)
    
    // Because we can't disable exceptions in the VxWorks compiler environment,
    // there are various modules that might throw exceptions that will be
    // completely uncaught (such as operator new throwing a bad_alloc exception
    // when we run out of memory).
    //
    // Unhandled exceptions cause std::terminate() to be called, which will just
    // call abort(), which causes the VxWorks thread to be suspended.  This
    // generally causes the system to become inoperable, but not to restart or
    // do anything useful.
    //
    // What we really want is for the system to crash and reboot, so we will
    // install our own terminate() function that does this.
    set_terminate(VxWorksTerminateCallout);

    //#elif defined(__QNX__)
    #elif defined(TARGETOS_Qnx6)

        // nothing needed here

    #elif defined(__linux__)

        // anything needed here?

    #elif defined(TARGETOS_eCos)
    
        // Anything needed here?

    #endif
}


// This is the method that you should call to get your operating system
// instance, thus allowing you to spawn yourself (if you are a BcmThread
// subclass) and create other operating system objects (BcmEvent, etc.).
//
// Note that you become the owner of the object returned, and you must
// delete it when you no longer need it.
//
// Parameters:  None.
//
// Returns:
//      A pointer to your instance of the operating system.
//
BcmOperatingSystem *BcmOperatingSystemFactory::NewOperatingSystem(void)
{
    // Create the correct derived class.
    #if defined(WIN32)

        return new BcmWin32OperatingSystem();

    #elif defined(PSOS)

        return new BcmPSOSOperatingSystem();

    #elif defined(TARGETOS_vxWorks)
    
        return new BcmVxOperatingSystem();

    //#elif defined(__QNX__)
    #elif defined(TARGETOS_Qnx6)

        return new BcmQnxOperatingSystem();
        
    #elif defined(__linux__)
        
        return new BcmLnxOperatingSystem();

    #elif defined(TARGETOS_eCos)
    
        return new BcmEcosOperatingSystem();

    #endif
}


// A mutex semaphore is the only OS component that doesn't need to be
// associated with a particular thread.
//
// Because a mutex tends to be associated with a data object that is shared
// between threads, we want the data object to be able to create the mutex
// without having to be tightly coupled with a thread object (and its
// BcmOperatingSystem instance).  This method allows non-thread objects to
// create their own mutexes.
//
// Note that you become the owner of the object returned, and you must
// delete it when you no longer need it.
//
// Parameters:
//      pName - the text name that is to be given to the object, for
//              debugging purposes.
//
// Returns:
//      A pointer to the mutex that was created, or NULL if there was a
//      problem creating it.
//
BcmMutexSemaphore *BcmOperatingSystemFactory::NewMutexSemaphore(const char *pName)
{
    // Create the correct derived class.
    #if defined(WIN32)

        return new BcmWin32MutexSemaphore(pName);

    #elif defined(PSOS)

        return new BcmPSOSMutexSemaphore(pName);

    #elif defined(TARGETOS_vxWorks)
    
        return new BcmVxMutexSemaphore(pName);

    //#elif defined(__QNX__)
    #elif defined(TARGETOS_Qnx6)

        return new BcmQnxMutexSemaphore(pName);

    #elif defined(__linux__)

         return new BcmLnxMutexSemaphore(pName);        

    #elif defined(TARGETOS_eCos)
    
        return new BcmEcosMutexSemaphore(pName);

    #endif
}


// In most cases, a counting semaphore is associated with a particular
// thread (and is addable to a BcmEventSet), but in some cases, it is useful
// to have a thread-less method to handle event notification.
//
// This method creates an OS-specific counting semaphore that isn't
// associated with a particular thread.  Unlike counting semaphores that are
// created by BcmOperatingSystem, this semaphore does not have an associated
// BcmEvent.
//
// The semaphore that is returned is owned by (and must be deleted by) the
// caller when it is no longer needed.
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
BcmCountingSemaphore *BcmOperatingSystemFactory::NewCountingSemaphore(bool isBounded,
                                                                      unsigned int maxCount,
                                                                      unsigned int initialCount,
                                                                      const char *pName)
{
    // Create the correct derived class.
    #if defined(WIN32)
    
        return new BcmWin32CountingSemaphore(isBounded, maxCount, initialCount,
                                             pName);

    #elif defined(PSOS)
    
        // Pass in NULL for the BcmEvent pointer so that it doesn't get associated
        // with a particular thread resource.
        return new BcmPSOSCountingSemaphore(isBounded, maxCount, initialCount,
                                            NULL, pName);
    #elif defined(TARGETOS_vxWorks)
    
        return new BcmVxCountingSemaphore(isBounded, maxCount, initialCount,
                                            NULL, pName);

    //#elif defined(__QNX__)
    #elif defined(TARGETOS_Qnx6)
        
        return new BcmQnxCountingSemaphore(isBounded, maxCount, initialCount,
                                            NULL, pName);

    #elif defined(__linux__)
        
        return new BcmLnxCountingSemaphore(isBounded, maxCount, initialCount,
                                            NULL, pName);

    #elif defined(TARGETOS_eCos)
    
        return new BcmEcosCountingSemaphore(isBounded, maxCount, initialCount,
                                            NULL, pName);

    #endif
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
void BcmOperatingSystemFactory::ThreadSleep(unsigned long timeoutMS)
{
    // Vector to the correct implementation.
    #if defined(WIN32)
    
        BcmWin32OperatingSystem::SleepImpl(timeoutMS);

    #elif defined(PSOS)
    
        BcmPSOSOperatingSystem::SleepImpl(timeoutMS);

    #elif defined(TARGETOS_vxWorks)
    
        // DPullen - optimization for typical case; many drivers will do a burst
        // of work and then sleep for 0 ms to allow other tasks to run.
        if (timeoutMS == 0)
        {
            taskDelay(0);
        }
        else
        {
            BcmVxOperatingSystem::SleepImpl(timeoutMS);
        }

    //#elif defined(__QNX__)
    #elif defined(TARGETOS_Qnx6)

        BcmQnxOperatingSystem::SleepImpl(timeoutMS);

    #elif defined(__linux__)
        
        BcmLnxOperatingSystem::SleepImpl(timeoutMS);

    #elif defined(TARGETOS_eCos)
    
        // DPullen - optimization for typical case; many drivers will do a burst
        // of work and then sleep for 0 ms to allow other tasks to run.
        if (timeoutMS == 0)
        {
            cyg_thread_yield();
        }
        else
        {
            BcmEcosOperatingSystem::SleepImpl(timeoutMS);
        }

    #endif
}


/// This method returns the OS-specific thread ID of the thread from which
/// the call to this method was made.
///
/// \return
///      The OS-specific thread ID of the thread from which the call to 
///      this method was made.
///
unsigned long BcmOperatingSystemFactory::CurrentThread(void)
{
    // Vector to the correct implementation.
    #if defined(WIN32)
    
        gLogMessageRaw << "WARNING: BcmOperatingSystemFactory::CurrentThread not implemented for Win32!" << endl;
        return 0; // Not implemented

    #elif defined(PSOS)
    
        unsigned long TaskId;
        
        t_ident(NULL, 0, &TaskId);
        return TaskId;

    #elif defined(TARGETOS_vxWorks)
        
        unsigned long TaskId;
        
        t_ident(NULL, 0, &TaskId);
        return TaskId;

    //#elif defined(__QNX__)
    #elif defined(TARGETOS_Qnx6)

        return pthread_self();

    #elif defined(__linux__)
        
        return pthread_self();

    #elif defined(TARGETOS_eCos)
    
        return cyg_thread_self();

    #endif
}


// This helper method translates the ThreadPriority enum in the base class
// to a real priority value for this operating system.  This value can be
// passed to other modules that don't use the OS Wrapper classes to spawn
// threads, but that want to base their priorities on the ones we have
// chosen.
//
// Parameters:
//      threadPriority - the ThreadPriority enum value to translate.
//
// Returns:
//      The real priority value for this operating system.
//
int BcmOperatingSystemFactory::ConvertThreadPriority(BcmOperatingSystem::ThreadPriority threadPriority)
{
    // Vector to the correct implementation.
    #if defined(WIN32)
    
        return BcmWin32OperatingSystem::ConvertThreadPriority(threadPriority);

    #elif defined(PSOS)
    
        return BcmPSOSOperatingSystem::ConvertThreadPriority(threadPriority);

    #elif defined(TARGETOS_vxWorks)
    
        return BcmVxOperatingSystem::ConvertThreadPriority(threadPriority);

    //#elif defined(__QNX__)
    #elif defined(TARGETOS_Qnx6)

        return BcmQnxOperatingSystem::ConvertThreadPriority(threadPriority);

        return 0;

    #elif defined(__linux__)

        // UNFINISHED - needs to be implemented?
        //return BcmlinuxOperatingSystem::ConvertThreadPriority(threadPriority);

        return 0;

    #elif defined(TARGETOS_eCos)
    
        return BcmEcosOperatingSystem::ConvertThreadPriority(threadPriority);

    #endif
}


#if defined(PSOS)

// Seed the stack of the task that is passed in, and do other interesting task
// start processing.
static void PsosTaskStartCallout(void * /*pNull*/, CO_INFO *pCalloutInfo)
{
    ULONG *pStack, *pStackTop;
    int rc;
    struct tinfo *pTaskInfo;

    // This structure is rather large, and can blow the stack for tasks with
    // small stacks.  Thus, we will create it on the heap to avoid this.  This
    // is safe, since this function is called in the context of the thread
    // that is being started (or in the context of ROOT after C++ runtime
    // initialization when the system is being started).
    pTaskInfo = new (struct tinfo);
    
    if (pTaskInfo != NULL)
    {
        // Query info about the task.
        rc = t_info(pCalloutInfo->tid, pTaskInfo);

        if (rc == 0)
        {
            // Pull out the current supervisor stack pointer, and skip over
            // enough stack so that we don't clobber things.
            pStack = pTaskInfo->ssp;
            pStack--;
            pStack -= 128;

            // Calculate where the top of the stack is based on the initial
            // stack pointer and the stack size.
            pStackTop = pTaskInfo->issp - (pTaskInfo->ss_size / 4);

            // Walk down through the stack, setting values to something
            // noticeable.
            while (pStack >= pStackTop)
            {
                *pStack = 0xdeadbeef;
                pStack--;
            }
        }

        #if ENABLE_CALL_TRACING
        
        // Set up a new Call Trace flag for this thread, if enabled.  This
        // allows us to enable/disable Call Tracing for each thread, which
        // minimizes the clutter on the console.
        //PsosRegisterThreadCallTrace(pTaskInfo->name, pCalloutInfo->tid);

        #endif
        
        delete pTaskInfo;
    }
}

#elif defined(TARGETOS_vxWorks)

// Public function that causes the system to crash.  It overrides the default
// terminate() function, which would just suspend the thread.
void VxWorksTerminateCallout(void)
{
    char *pVoid = NULL;

    BcmMessageLogSettings messageLogSettings("BcmOperatingSystemFactory", "Singleton");

    gFatalErrorMsg(messageLogSettings, "VxWorksTerminateCallout")
        << "Somebody called terminate(), possibly as a result of an unhandled C++ exception.  Deliberately crashing the system so that we restart..." << endl;

    // Give this a little time to flush out.
    BcmOperatingSystemFactory::ThreadSleep(250);

    // Force a memory exception.
    pVoid[0] = 0;
}
    
#endif



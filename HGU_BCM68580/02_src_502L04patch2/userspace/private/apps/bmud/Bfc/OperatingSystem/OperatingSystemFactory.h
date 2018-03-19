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
//  Filename:       OperatingSystemFactory.h
//  Author:         David Pullen
//  Creation Date:  Feb 23, 1999
//
//****************************************************************************

#ifndef OPERATINGSYSTEMFACTORY_H
#define OPERATINGSYSTEMFACTORY_H

//********************** Include Files ***************************************

#include "OperatingSystem.h"

//********************** Global Types ****************************************

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Forward Declarations ********************************

class BcmMutexSemaphore;
class BcmCountingSemaphore;

//********************** Class Declaration ***********************************


/**   \ingroup OsWrappers
*
*     This class is responsible for creating the correct type of
*     BcmOperatingSystem, based on compiler or other environment settings.
*
*     It can also be used to create reentrancy protection objects (like
*     BcmMutexSemaphore and BcmCountingSemaphore) without having to do so in
*     the context of the BcmThread; this is important in cases where a data
*     object will be accessed by multiple threads at the same time.  The data
*     object needs to be able to create a BcmMutexSemaphore that it will lock
*     and unlock whenever one of it's methods is called.
*
*     This class can also be used to cause the current thread to sleep,
*     yielding the processor to other threads.  Again, this is useful for
*     non-BcmThread objects; for example, a h/w interface object might need to
*     write a value to a register, then wait a while before reading another
*     register.
*
*     This class just has static methods; you don't need to create an
*     instance of this class to use it.
*/
class BcmOperatingSystemFactory
{
public:

    /// Main() should call this function before there are any calls to other
    /// operating system facilities (including other functions of this class). 
    /// This function may initialize critical elements that allow the operating
    /// environment to work properly.
    ///
    static void InitializeOperatingSystem(void);

    /// This is the method that you should call to get your operating system
    /// instance, thus allowing you to spawn yourself (if you are a BcmThread
    /// subclass) and create other operating system objects (BcmEvent, etc.).
    ///
    /// Note that you become the owner of the object returned, and you must
    /// delete it when you no longer need it.
    ///
    /// \return
    ///      A pointer to your instance of the operating system, or NULL if there
    ///      was a problem creating it.
    ///
    static BcmOperatingSystem *NewOperatingSystem(void);

    /// A mutex semaphore is one of the few OS components that don't need to be
    /// associated with a particular thread.
    ///
    /// Because a mutex tends to be associated with a data object that is shared
    /// between threads, we want the data object to be able to create the mutex
    /// without having to be tightly coupled with a thread object (and its
    /// BcmOperatingSystem instance).  This method allows non-thread objects to
    /// create their own mutexes.
    ///
    /// Note that you become the owner of the object returned, and you must
    /// delete it when you no longer need it.
    ///
    /// \param
    ///      pName - the text name that is to be given to the object, for
    ///              debugging purposes.
    ///
    /// \return
    ///      A pointer to the mutex that was created, or NULL if there was a
    ///      problem creating it.
    ///
    static BcmMutexSemaphore *NewMutexSemaphore(const char *pName = NULL);

    /// In most cases, a counting semaphore is associated with a particular
    /// thread (and is addable to a BcmEventSet), but in some cases, it is useful
    /// to have a thread-less method to handle event notification.
    ///
    /// This method creates an OS-specific counting semaphore that isn't
    /// associated with a particular thread.  Unlike counting semaphores that are
    /// created by BcmOperatingSystem, this semaphore does not have an associated
    /// BcmEvent.
    ///
    /// The semaphore that is returned is owned by (and must be deleted by) the
    /// caller when it is no longer needed.
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
    static BcmCountingSemaphore *NewCountingSemaphore(bool isBounded,
                                                      unsigned int maxCount,
                                                      unsigned int initialCount,
                                                      const char *pName = NULL);

    /// This method allows a thread to put itself to sleep for the specified
    /// number of milliseconds, giving up control of the processor.
    ///
    /// Note that a timeout value of 0 may not cause the thread to sleep at all.
    ///
    /// \param
    ///      timeoutMS - the number of milliseconds to sleep.
    ///
    static void ThreadSleep(unsigned long timeoutMS);
    
    /// This method returns the OS-specific thread ID of the thread from which
    /// the call to this method was made.
    ///
    /// \return
    ///      The OS-specific thread ID of the thread from which the call to 
    ///      this method was made.
    ///
    static unsigned long CurrentThread(void);

    /// This helper method translates the ThreadPriority enum in the base class
    /// to a real priority value for this operating system.  This value can be
    /// passed to other modules that don't use the OS Wrapper classes to spawn
    /// threads, but that want to base their priorities on the ones we have
    /// chosen.
    ///
    /// \param
    ///      threadPriority - the ThreadPriority enum value to translate.
    ///
    /// \return
    ///      The real priority value for this operating system.
    ///
    static int ConvertThreadPriority(BcmOperatingSystem::ThreadPriority threadPriority);

protected:

    /// I have to declare all of these as protected in order to keep this class
    /// from being used inappropriately.  The compiler will generate these
    /// methods for you if you don't declare them yourself!

    /// Default Constructor.  Not supported.
    BcmOperatingSystemFactory(void);

    /// Destructor.  Not supported.
    ~BcmOperatingSystemFactory();

private:

    /// Copy Constructor.  Not supported.
    BcmOperatingSystemFactory(BcmOperatingSystemFactory &otherInstance);

    /// Assignment operator.  Not supported.
    BcmOperatingSystemFactory & operator = (BcmOperatingSystemFactory &otherInstance);

};


//********************** Inline Method Implementations ***********************

#endif



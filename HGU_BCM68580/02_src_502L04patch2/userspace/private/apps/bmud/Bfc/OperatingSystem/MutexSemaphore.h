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
//  Filename:       MutexSemaphore.h
//  Author:         David Pullen
//  Creation Date:  Feb 18, 1999
//
//****************************************************************************

#ifndef MUTEXSEMAPHORE_H
#define MUTEXSEMAPHORE_H

#if defined (__cplusplus)
extern "C" 
{
#endif

void mutexPrint ();

#if defined (__cplusplus)
}
#endif

#if defined (__cplusplus)

#if (BFC_INCLUDE_CONSOLE_SUPPORT)
// Defining this directive adds a command mutex_debug to the root directory
// which will display a list of all BcmMutexSemaphore objects and whether
// each one is locked or not.  This can be useful for debugging mutex
// deadlocks.  By default we will turn this on unless there is no console.
#if !defined (TARGETOS_Linux)
#define MUTEX_DEBUG
#endif
#endif

#ifdef MUTEX_DEBUG
#include <list>
#endif

//********************** Include Files ***************************************

// For bool and other portable types.
#include "typedefs.h"

//********************** Global Types ****************************************

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Class Declaration ***********************************


/** \ingroup OsWrappers
*
*   This is the base class for all types of operating system mutual
*   exclusion semaphores.
*
*   This class is used to prevent multiple threads from accessing shared
*   data at the same time.  When one thread wants to read or write the
*   shared data, it tries to lock the mutex.  If the mutex is already
*   locked by another thread, then the thread must wait for it to be
*   unlocked before performing its operations.  Alternatively, it can time
*   out and give up on trying to access the data.
*
*   Note that, rather than making the threads explicitly lock the mutex,
*   the shared data object itself should contain and handle it on behalf of
*   the calling threads.  This way, the mutex is guaranteed to be enforced
*   and handled uniformly, and it only needs to be done in one place (the
*   data object), rather than in each thread that accesses the data.
*
*   This is one of the few OS components that doesn't have an associated
*   event, and that can't be added to a BcmEventSet.  It is used for
*   processing data, and either the thread gets it, or it doesn't.  The
*   thread can't request the lock and check for it later (this would block
*   other threads).
*
*   Mutex Semaphores can be created both by BcmOperatingSystem and
*   BcmOperatingSystemFactory; this allows data objects (who probably don't
*   have access to the BcmOperatingSystem instance of a thread) to create a
*   mutex for their own use.
*/
class BcmMutexSemaphore
{
public:

    /// Default Constructor.  Stores the name assigned to the object.
    ///
    /// \param
    ///      pName - the text name that is to be given to the object, for
    ///              debugging purposes.
    ///
    BcmMutexSemaphore(const char *pName = NULL);

    /// Destructor.  Frees up any memory/objects allocated, cleans up internal
    /// state.
    ///
    virtual ~BcmMutexSemaphore();

    /// Values that can be used in the call to Lock().
    typedef enum
    {
        kTimeout,
        kForever
    } LockMode;
    
    /// Tries to gain access to the shared resource.  If it has already been
    /// locked, then the thread will block (based on the timeout parameters).
    ///
    /// By default, if you call this with no parameters, then it will not
    /// time out waiting for the lock.  If you call it with kTimeout and 0 as
    /// the timeoutMS value, then it will return immediately if the lock is not
    /// available.
    ///
    /// This is a pure virtual method; derived classes must override this to
    /// provide the OS-specific implementation for locking a semaphore.
    ///
    /// \param
    ///      mode - tells whether or not you want to time out if the lock is not
    ///             granted in the specified amount of time.
    /// \param
    ///      timeoutMS - the amount of time to wait for the lock.  This is
    ///                  ignored if mode is not set to kTimeout.
    ///
    /// \retval
    ///      true if the semaphore was successfully locked.
    /// \retval
    ///      false if the semaphore was not locked (timed out waiting for it
    ///          to be unlocked, etc.).
    ///
    virtual bool Lock(LockMode mode = kForever, unsigned long timeoutMS = 0) = 0;

    /// Unlocks access to the shared resource, allowing other threads to lock it.
    ///
    /// This is a pure virtual method; derived classes must override this to
    /// provide the OS-specific implementation for unlocking a semaphore.
    ///
    /// \retval
    ///      true if the semaphore was unlocked.
    /// \retval
    ///      false if there was a problem (OS-specific).
    ///
    virtual bool Unlock(void) = 0;
    
    /// Indicates if this mutex is locked.
    ///
    /// The base class implementation of this method always returns false since 
    /// the base class doesn't know if it is locked or not.  Derived OS 
    /// specific objects may override this method to provide this information.
    ///
    /// \retval
    ///      true if the semaphore is locked.
    /// \retval
    ///      false if the semaphore is unlocked.
    ///
    virtual bool Locked(void) const;
    
    /// Simple accessor for the ID of the thread which currently has the
    /// mutex locked.  If not locked, this method will return 0.
    ///
    /// The base class implementation of this method always returns 0 since 
    /// the base class doesn't know the owning thread ID.  Derived OS 
    /// specific objects may override this method to provide this information.
    ///
    /// \return
    ///      The thread ID of the thread which currently has the mutex locked.
    ///
    virtual unsigned long ThreadId(void) const;
    
    /// Simple accessor for the name of this object.
    ///
    /// \return
    ///      The pointer to the name string.
    ///
    inline const char *Name(void) const;
    
    #ifdef MUTEX_DEBUG
    static void PrintMutexInfo(void);
    #endif

protected:

    /// My assigned name.
    char *pfName;
    
    #ifdef MUTEX_DEBUG
    static list <BcmMutexSemaphore *> *fpMutices;
    #endif

private:

    /// Copy Constructor.  Not supported.
    BcmMutexSemaphore(BcmMutexSemaphore &otherInstance);

    /// Assignment operator.  Not supported.
    BcmMutexSemaphore & operator = (BcmMutexSemaphore &otherInstance);

};


//********************** Inline Method Implementations ***********************


/// Simple accessor for the name of this object.
///
/// \return
///      The pointer to the name string.
///
inline const char *BcmMutexSemaphore::Name(void) const
{
    return pfName;
}


// Here is a simple object to manage a mutex semaphore.  All it does is lock
// the mutex in the constructor and unlock it in the destructor.  The point
// of this is so we can create an instance of one of these objects on the stack 
// at the start of a function or method, and it will lock the mutex at that 
// time.  Then when we return from the function or method the object goes out
// of scope and is deleted, thus unlocking the mutex.  This avoids the risk of
// returning without unlocking the mutex (coding error), as well as being more 
// convenient to use since you don't have to call unlock before returning.
// Like so:
//
// int MyClass::MyMethod ()
// {
//   BcmMutexSemaphoreAutoLock Lock(fpMyMutex);
//
//   if (something)
//     return 1;
//   
//   else
//     return 2;
//
//   return 0;
// }
//
// The Lock object here will lock the mutex at the beginning and unlock it
// whenever we return.  Thus eliminating the risk of the common coding error
// where we lock a mutex then return without unlocking, thus producing a
// deadlock.  Also makes for cleaner code than the following:
//
// int MyClass::MyMethod ()
// {
//   if (fpMyMutex)
//     fpMyMutex->Lock();
//
//   if (something)
//   {
//     if (fpMyMutex)
//       fpMyMutex->Unlock();
//
//     return 1;
//   }
//   
//   else
//   {
//     if (fpMyMutex)
//       fpMyMutex->Unlock();
//
//     return 2;
//   }
//
//   if (fpMyMutex)
//     fpMyMutex->Unlock();
//
//   return 0;
// }
//

class BcmMutexSemaphoreLock
{
  public:
    BcmMutexSemaphoreLock (BcmMutexSemaphore *pMutex, const char *pIdStr = "");
   ~BcmMutexSemaphoreLock ();
   
  protected:
    BcmMutexSemaphore *fpMutex;
};

// A debug-enhanced version of the auto-lock.  This should not be used in 
// production code but can be useful for development and debugging.  It will
// print a message to the console whenever mutex protection kicks in - which
// is to say, if the mutex will block when the object is created.
class BcmMutexSemaphoreDebugLock : public BcmMutexSemaphoreLock
{
  public:
    BcmMutexSemaphoreDebugLock (BcmMutexSemaphore *pMutex, const char *pIdStr = "");
};

// We can choose whether to use the debug instrumented version or the
// standard version of the mutex lock object by the value of this macro.
#define BcmMutexSemaphoreAutoLock BcmMutexSemaphoreLock

#endif

#endif

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
//  Filename:       MutexSemaphore.cpp
//  Author:         David Pullen
//  Creation Date:  Feb 19, 1999
//
//****************************************************************************
//  Description:
//      This is the base class for all types of operating system mutual
//      exclusion semaphores.  These are used to serialize access to shared
//      resources (memory, hardware, etc.) by more than one thread.
//
//****************************************************************************

//********************** Include Files ***************************************

// My api and definitions...
#include "MutexSemaphore.h"

#include "OperatingSystem.h"
#include "OperatingSystemFactory.h"

#ifdef MUTEX_DEBUG
extern "C" BcmMutexSemaphore *pgStlAllocMutex;
list <BcmMutexSemaphore *> *BcmMutexSemaphore::fpMutices = NULL;

// mutex to protect the mutex list 
static BcmMutexSemaphore * pgMutexDebugListMutex = BcmOperatingSystemFactory::NewMutexSemaphore("Mutex debug list mutex");
#endif

// "C" callable API
void mutexPrint()
{
  #ifdef MUTEX_DEBUG
  BcmMutexSemaphore::PrintMutexInfo();
  #endif
}

//********************** Local Types *****************************************

//********************** Local Constants *************************************

//********************** Local Variables *************************************

//********************** Class Method Implementations ************************

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
// deadlock.
//

BcmMutexSemaphoreLock::BcmMutexSemaphoreLock (BcmMutexSemaphore *pMutex, const char *pIdStr)
{
  fpMutex = pMutex;
  
  if (fpMutex)
    fpMutex->Lock();
}

BcmMutexSemaphoreLock::~BcmMutexSemaphoreLock ()
{
  if (fpMutex)
    fpMutex->Unlock();
    
  // Might as well clear this pointer...
  fpMutex = NULL;
}

// A debug-enhanced version of the auto-lock.  This should not be used in 
// production code but can be useful for development and debugging.  It will
// print a message to the console whenever mutex protection kicks in - which
// is to say, if the mutex will block when the object is created.
#include "SystemTime.h"
BcmMutexSemaphoreDebugLock::BcmMutexSemaphoreDebugLock (BcmMutexSemaphore *pMutex, const char *pIdStr)
  : BcmMutexSemaphoreLock (NULL)
{
  fpMutex = pMutex;

  if (fpMutex)
  {
    unsigned long Owner = fpMutex->ThreadId();
    unsigned long Caller = BcmOperatingSystemFactory::CurrentThread();
    bool Blocked = false;
    
    if ((Owner != Caller) && (Owner != 0))
      Blocked = true;
    
    if (Blocked == true)
      gLogMessageRaw << SystemTimeMS() << ": MUTEX WAIT - " << fpMutex->Name() << " - Owner = " << inHex(Owner) << "; Waiting = " << inHex(Caller) << " " << pIdStr << endl;
    
    fpMutex->Lock();
    
    if (Blocked)
      gLogMessageRaw << SystemTimeMS() << ": MUTEX LOCK - " << fpMutex->Name() << " - New Owner = " << inHex(fpMutex->ThreadId()) << " " << pIdStr  << "; Old owner = " << inHex(Owner) << endl;
  }
}

// Default Constructor.  Stores the name assigned to the object.
//
// Parameters:
//      pName - the text name that is to be given to the object, for
//              debugging purposes.
//
// Returns:  N/A
//
BcmMutexSemaphore::BcmMutexSemaphore(const char *pName)
{
    // Call the helper method to create my object name.
    pfName = BcmOperatingSystem::GenerateName(pName, "Mutex");
    
    #ifdef MUTEX_DEBUG
    // Push myself onto the static mutex list
    if( pgMutexDebugListMutex )
    {
        BcmMutexSemaphoreAutoLock Lock(pgMutexDebugListMutex);

        // Special case: watch out for the global STL alloc mutex!
        if ((fpMutices == NULL) && (pgStlAllocMutex != NULL) && (this != pgStlAllocMutex))
        {
            fpMutices = new list<BcmMutexSemaphore *>;
        }

        // Push myself onto the static mutex list
        if( pgMutexDebugListMutex )
        {
            BcmMutexSemaphoreAutoLock Lock(pgMutexDebugListMutex);

            if (fpMutices)
                fpMutices->push_back(this);
        }
    }
    #endif
}


// Destructor.  Frees up any memory/objects allocated, cleans up internal
// state.
//
// Parameters:  N/A
//
// Returns:  N/A
//
BcmMutexSemaphore::~BcmMutexSemaphore()
{
    #ifdef MUTEX_DEBUG
    // Push myself onto the static mutex list
    if( pgMutexDebugListMutex )
    {
        BcmMutexSemaphoreAutoLock Lock(pgMutexDebugListMutex);

        // Find myself in the list and remove me.
        if (fpMutices)
          fpMutices->remove(this);
    }
    #endif

    // Delete the memory associated with the name.
    delete pfName;
    pfName = NULL;
}


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
bool BcmMutexSemaphore::Locked(void) const
{
  #ifdef MUTEX_DEBUG
  gLogMessageRaw << "WARNING: BcmMutexSemaphore::Locked base class called!" << endl;
  #endif
  
  return false;
}


/// Simple accessor for the ID of the thread which currently has the
/// mutex locked.  If not locked, this method will return 0.
///
/// \return
///      The thread ID of the thread which currently has the mutex locked.
///    
unsigned long BcmMutexSemaphore::ThreadId(void) const
{
    return 0;
}


#ifdef MUTEX_DEBUG
void BcmMutexSemaphore::PrintMutexInfo(void)
{
    BcmMutexSemaphore *pMutex;

    // Push myself onto the static mutex list
    if( pgMutexDebugListMutex )
    {
        BcmMutexSemaphoreAutoLock Lock(pgMutexDebugListMutex);

        // Iterate over the list and print if each object is locked.
        list<BcmMutexSemaphore *>::iterator Pos = fpMutices->begin();
        while (Pos != fpMutices->end())
        {
            pMutex = *Pos;

            if (pMutex->Locked())
                gLogMessageRaw << "  LOCKED: " << inHex(pMutex) << "  " << pMutex->Name() << " by thread " << inHex(pMutex->ThreadId()) << endl;
            else
                gLogMessageRaw << "UNLOCKED: " << inHex(pMutex) << "  " << pMutex->Name() << endl;

            Pos++;
        }
    }
}
#endif

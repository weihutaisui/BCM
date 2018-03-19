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

//********************** Include Files ***************************************
#include <sys/time.h>
#include <pthread.h>
#include <errno.h>

#include "MessageLog.h"
#include "LnxMutexSemaphore.h"

//********************** Local Types *****************************************

//********************** Local Constants *************************************

//********************** Local Variables *************************************

//********************** Class Method Implementations ************************


// ----------------------------------------------------------------------------
// Initializing Constructor.  Stores the name assigned to the object, and
// creates the Mutex with the specified parameters.
//
// Parameters:
//      pName - the text name that is to be given to the object, for
//              debugging purposes.
//
// Returns:  N/A
// ----------------------------------------------------------------------------
BcmLnxMutexSemaphore::BcmLnxMutexSemaphore(const char *pName) :
    BcmMutexSemaphore(pName)
{
    // Set this stuff up.
	fOwnerThread = (pthread_t)0;

	// Initialize the mutex attributes
	pthread_mutexattr_init(&fLnxMutexAttr);
	// Set the mutex type to 'error checking'
	if(pthread_mutexattr_settype (&fLnxMutexAttr, PTHREAD_MUTEX_ERRORCHECK_NP))
	{
		gLogMessageRaw 
			<< "BcmLnxMutexSemaphore::BcmLnxMutexSemaphore:  Error - "
			<< "Failed to set mutex attibutes!  tid = " << hex << pthread_self() << endl;
	}
	// Initialize the POSIX mutex
	pthread_mutex_init(&fLnxMutexLock, &fLnxMutexAttr);
	// For a 'error checking' mutex, __m_count is not used by pthread library.
	// It might be safe just to use that...
	fMutexCount = 0;
}


// ----------------------------------------------------------------------------
// Destructor.  Frees up any memory/objects allocated
//
// Parameters:  N/A
//
// Returns:  N/A
//
// ----------------------------------------------------------------------------
BcmLnxMutexSemaphore::~BcmLnxMutexSemaphore()
{
	// 'Error Checking' mutex can only be unlocked by the owning thread!
	if(pthread_mutex_unlock(&fLnxMutexLock))
	{
		gLogMessageRaw 
			<< "BcmLnxMutexSemaphore::~BcmLnxMutexSemaphore:  Error - "
			<< "Failed to unlock mutex!  tid = " << hex << pthread_self() << endl;
	}
	else
	{
		// Now destroy the mutex
		pthread_mutex_destroy(&fLnxMutexLock);
		// Now destroy the mutex attribute
		pthread_mutexattr_destroy(&fLnxMutexAttr);

		fOwnerThread = (pthread_t)0;
		fMutexCount = 0;
	}
}


// ----------------------------------------------------------------------------
// Tries to get access to the mutex, If the mutex is available, then
// the thread will block (based on the timeout  parameters).
//
// By default, if you call this with no parameters, then it will not
// time out waiting for the mutex.  If you call it with kTimeout and 0
// as the timeoutMS value, then it will return immediately if the mutex
// is not available (a nonblocking attempt at aquiring the mutex).
//
// Parameters:
//      mode - tells whether or not you want to time out if the Mutex is
//             not granted in the specified amount of time.
//      timeoutMS - the amount of time to wait for the Mutex.  This is
//                  ignored if mode is not set to kTimeout.
//
// Returns:
//      true if the Mutex was successfully gotten.
//      false if the Mutex was not available (timed out waiting for it
//          to be released, etc.).
//
// ToDo: Check priority inheritance!!!
// ----------------------------------------------------------------------------
bool BcmLnxMutexSemaphore::Lock(LockMode mode, unsigned long timeoutMS)
{
	bool locked = false;
	int retcode;
	struct timeval now;
    struct timespec timeout;
	
	// Check the mode
	if(mode == kTimeout)
	{
		// Try to lock the mutex, no waiting!
		if(timeoutMS == 0)
		{
			retcode = pthread_mutex_trylock(&fLnxMutexLock);
		}
		else
		{
			// Try to lock the mutex, give up after timeoutMS!
			retcode = gettimeofday(&now, NULL);
			if (retcode!=0) 
			{
				gLogMessageRaw << "gettimeofday() returned %d " << retcode << endl;
			}
			else
			{
				// timeout is the absolute future time
				timeout.tv_nsec = now.tv_usec * 1000 + (timeoutMS%1000)*1000000;
				timeout.tv_sec = now.tv_sec + (timeoutMS/1000);
				if (timeout.tv_nsec > 1000000000) 
				{
					timeout.tv_nsec -=  1000000000;
					timeout.tv_sec ++;
				}
				// This function was introduced in the POSIX.1d revision of the POSIX standard.
				// It returns either after mutex is locked or timeout has occurred.
				// It only supports timed or error checking mutex!!!
				retcode = pthread_mutex_timedlock (&fLnxMutexLock, &timeout);
			}
		}
	}
	else
	{
		// Try to lock the mutex, never give up!
		retcode = pthread_mutex_lock(&fLnxMutexLock);
	}

	switch(retcode)
	{
	case 0:
		fOwnerThread = pthread_self();
		fMutexCount++;
		locked = true;
		break;
	case EDEADLK:
		// Recursive locking
		fMutexCount++;
		locked = true;
//		gLogMessageRaw 
//			<< "BcmLnxMutexSemaphore::Lock:  Info - "
//			<< "Recursive lock fMutexCount = " << dec << fMutexCount << " " << hex << pthread_self() << endl;
		break;
	case ETIMEDOUT:
		gLogMessageRaw 
			<< "BcmLnxMutexSemaphore::Lock:  Warning - "
			<< "Timed out to lock mutex!  tid = " << hex << pthread_self() << endl;
		break;
	case EBUSY:
		gLogMessageRaw 
			<< "BcmLnxMutexSemaphore::Lock:  Warning - "
			<< "Failed to try to lock mutex!  tid = " << hex << pthread_self() << endl;
		break;
	default:
		gLogMessageRaw 
			<< "BcmLnxMutexSemaphore::Lock:  Error - "
			<< "mode = " << dec << mode << " timeoutMS = " << timeoutMS 
			<< " retcode = " << dec << retcode << " tid = " << hex << pthread_self() << endl;
		break;
	}

	return locked;
}
// ----------------------------------------------------------------------------
// Releases the Mutex, If LockCount goes to 0 and a thread is waiting for the
// Mutex, then it will be released to run.
//
// Parameters:  None.
//
// Returns:
//      true if the Mutex was released.
//      false if there was a problem (OS-specific).
// ----------------------------------------------------------------------------
bool BcmLnxMutexSemaphore::Unlock(void)
{
	int retcode;
	bool unlocked = true;
	//gLogMessageRaw << "Unlocking... " << hex << pthread_self() << endl;
	// Since we are using an 'error checking' mutex, first check if the unlocking thread
	// is the owner, then check the recursive counter
	if(fOwnerThread != pthread_self())
	{
		unlocked = false;
		gLogMessageRaw 
			<< "BcmLnxMutexSemaphore::Unlock:  Error - "
			<< "Mutex not locked or not owned by the thread! The owner is " << hex << fOwnerThread << endl;
	}
	else
	{
		fMutexCount--;
		if(fMutexCount < 0)
		{
			gLogMessageRaw 
				<< "BcmLnxMutexSemaphore::Unlock:  Error - "
				<< "fMutexCount is negative!!! tid = " << hex << pthread_self() << endl;
		}
		else if(fMutexCount == 0)
		{
			retcode = pthread_mutex_unlock(&fLnxMutexLock);
			if(retcode)
			{
				gLogMessageRaw 
					<< "BcmLnxMutexSemaphore::Unlock:  Error - "
					<< "Failed to unlock mutex!  tid = " << hex << pthread_self() << endl;
				// This is impossible!!!
				if(retcode == EPERM)
				{
					gLogMessageRaw 
						<< "BcmLnxMutexSemaphore::Unlock:  Error - "
						<< "Impossible: Mutex not locked or not owned by the thread! The owner is " << hex << fOwnerThread << endl;
				}
				unlocked = false;		
			}
//			else
//			{
//				gLogMessageRaw
//					<< "BcmLnxMutexSemaphore::Unlock:  Info - " << "successfully unlocked!" << endl;
//			}
		}
//		else
//		{
//			gLogMessageRaw
//				<< "BcmLnxMutexSemaphore::Unlock:  Info - " << "fMutexCount = " << fMutexCount << endl;
//		}

	}
	//gLogMessageRaw << "Unlocking...done! " << hex << pthread_self() << endl;
	return unlocked;
}


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
unsigned long BcmLnxMutexSemaphore::ThreadId(void) const
{
    return fOwnerThread;
}

// ============================================================================
// ======== END ========== END ========== END ========== END ========== END ===
// ============================================================================



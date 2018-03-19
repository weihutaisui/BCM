//****************************************************************************
//
//  Copyright (c) 1999  Broadcom Corporation
//  All Rights Reserved
//  No portions of this material may be reproduced in any form without the
//  written permission of:
//          Broadcom Corporation
//          16251 Laguna Canyon Road
//          Irvine, California 92618
//  All information contained in this document is Broadcom Corporation
//  company private, proprietary, and trade secret.
//
//  LnxCountingSemaphore.cpp - Mark Rush 10/23/2001
//
//****************************************************************************

//********************** Include Files ***************************************
#include <unistd.h> //for testing MDR

#include "LnxCountingSemaphore.h"
#include <errno.h>

//********************** Local Types *****************************************

//********************** Local Constants *************************************

//********************** Local Variables *************************************

//********************** Class Method Implementations ************************


// ----------------------------------------------------------------------------
// Initializing Constructor.  Stores the name assigned to the object, and
// creates the counting semaphore with the specified parameters.
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
// Returns:  N/A
// ----------------------------------------------------------------------------
BcmLnxCountingSemaphore::BcmLnxCountingSemaphore(bool isBounded, 
                                                     unsigned int maxCount,
                                                     unsigned int initialCount,
                                                   BcmEvent *pEvent,
                                                   const char *pName) :
    BcmCountingSemaphore(pName),
    fIsBounded (isBounded),
    fMaxCount (maxCount)
{
    int retcode;

	// Override the class name given by my parent.
    fMessageLogSettings.SetModuleName("BcmLnxCountingSemaphore");

    // Make sure that the initial count is less than or equal to the max.
    if (initialCount > maxCount)
        initialCount = maxCount;

	retcode = sem_init(&fSemaphore, 0, initialCount);
	if(retcode == EINVAL)
	{
        gErrorMsg(fMessageLogSettings, "BcmLnxCountingSemaphore")
            << "initialCount is greater than SEM_VALUE_MAX" << endl;
	}
	else if(retcode != 0)
	{
        gErrorMsg(fMessageLogSettings, "BcmLnxCountingSemaphore")
            << "retcode =" << retcode << endl;
	}

	fSemInitialized = true;
}


// ----------------------------------------------------------------------------
// Destructor.  Frees up any memory/objects allocated, cleans up internal
// state.
//
// Parameters:  N/A
//
// Returns:  N/A
//
// ----------------------------------------------------------------------------
BcmLnxCountingSemaphore::~BcmLnxCountingSemaphore()
{
    // destroy the semaphore.
    if(sem_destroy (&fSemaphore) != 0)
	{
        gErrorMsg(fMessageLogSettings, "~BcmLnxCountingSemaphore")
            << "Semaphore is busy! errno = " << errno <<  endl;
	}

	fSemInitialized = false;
}


// ----------------------------------------------------------------------------
// Tries to get access to the semaphore, reducing the its count by 1.  If
// the count is 0, then the thread will block (based on the timeout
// parameters).
//
// By default, if you call this with no parameters, then it will not
// time out waiting for the semaphore.  If you call it with kTimeout and 0
// as the timeoutMS value, then it will return immediately even if the semaphore
// is not available.
//
// Parameters:
//      mode - tells whether or not you want to time out if the semaphore is
//             not granted in the specified amount of time.
//      timeoutMS - the amount of time to wait for the semaphore.  This is
//                  ignored if mode is not set to kTimeout.
//
// Returns:
//      true if the semaphore was successfully gotten.
//      false if the semaphore was not available (timed out waiting for it
//          to be released, etc.).
//
// ----------------------------------------------------------------------------
bool BcmLnxCountingSemaphore::Get(GetMode mode, unsigned long timeoutMS)
{
	bool gotit = false;
	int retcode;
	struct timeval now;
    struct timespec timeout;

	// Check the mode
	if (mode == kTimeout)
	{
		// Try to lock the semaphore, no waiting!
		if(timeoutMS == 0)
		{
			retcode = sem_trywait(&fSemaphore);
		}
		else
		{
			// Try to lock the semaphore, give up after timeoutMS!
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
				// This function needs __USE_XOPEN2K
				retcode = sem_timedwait (&fSemaphore, &timeout);
			}
			
		}
	}
	else 
	{
		// Try to lock the semaphore, never give up!
		retcode = sem_wait(&fSemaphore);
	}

	switch(retcode)
	{
	case 0:
		gotit = true;
		break;
	case -1:
        gWarningMsg(fMessageLogSettings, "Get")
            << "Failed to get semaphore! " << endl;
		break;
	case ETIMEDOUT:
        gWarningMsg(fMessageLogSettings, "Get")
            << "Timed out to get semaphore! " << endl;
		break;
	default:
        gErrorMsg(fMessageLogSettings, "Get")
            << "retcode =" << retcode << endl;
		break;
	}

	return gotit;
}
// ----------------------------------------------------------------------------
// Releases the semaphore, increasing its count by 1.  If the semaphore has
// a ceiling to the count, then it will not be increased beyond it.  If the
// count is 0 and a thread is waiting for the semaphore, then it will be
// released to run.
//
// Parameters:  None.
//
// Returns:
//      true if the semaphore was released.
//      false if there was a problem (OS-specific).
// ----------------------------------------------------------------------------
bool BcmLnxCountingSemaphore::Release(void)
{
	sem_post(&fSemaphore);
	return true;
}

// This method lets a thread read the current count of the semaphore.
//
// This is a pure virtual method; derived classes must override this to
// provide the OS-specific implementation for querying a semaphores count.
//
// Parameters:  None.
//
// Returns:
//      The semaphores current count.
//
unsigned int BcmLnxCountingSemaphore::GetCount(void) const
{
	int count = SEM_VALUE_MAX;
    if ( fSemInitialized )
    {
		sem_getvalue((sem_t *)&fSemaphore, &count);
    }
    else
    {
        gErrorMsg(fMessageLogSettings, "GetCount")
            << "Semaphore not initialized!" << endl;
    }
	return count;
}


// ============================================================================
// ======== END ========== END ========== END ========== END ========== END ===
// ============================================================================



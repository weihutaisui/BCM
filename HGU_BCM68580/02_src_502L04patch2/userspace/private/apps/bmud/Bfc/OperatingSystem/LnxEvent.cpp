//****************************************************************************
//
//  $Id: LnxEvent.cpp 1.4 2006/08/02 19:50:24Z taskiran Release $
//  Copyright (c) 2001  Broadcom Corporation
//  All Rights Reserved
//  No portions of this material may be reproduced in any form without the
//  written permission of:
//          Broadcom Corporation
//          16251 Laguna Canyon Road
//          Irvine, California 92618
//  All information contained in this document is Broadcom Corporation
//  company private, proprietary, and trade secret.
//
//****************************************************************************

//********************** Include Files ***************************************

#include <errno.h>

#include "LnxEvent.h"
#include "LnxEventSet.h"
#include "LnxTimeout.h"
#include <unistd.h>
#include <pthread.h>

sigset_t BcmLnxEvent::fSigmask;
sigset_t BcmLnxEvent::fSigmaskNoAlarm;
bool BcmLnxEvent::fRunOnce = false;
pthread_key_t BcmLnxEvent::fKeyEvtInstance;
pthread_key_t BcmLnxEvent::fKeyEvtSetInstance;
pthread_key_t BcmLnxEvent::fKeyEvtSetQuantity;
pthread_mutex_t BcmLnxEvent::fLnxMutexLock = PTHREAD_MUTEX_INITIALIZER;

//********************** Local Types *****************************************

//********************** Local Constants *************************************

//********************** Local Variables *************************************

//********************** Class Method Implementations ************************

// ----------------------------------------------------------------------------
// Initializing/default Constructor.  
//
// Parameters:
//      pName - the text name that is to be given to the object, for
//              debugging purposes.
//
//  Notes:
//      When the event occurs, the thread that created the Event object
//      will be the one that is notified. 
//
// Returns:  N/A
// ----------------------------------------------------------------------------
BcmLnxEvent::BcmLnxEvent(const char *pName) :
    BcmEvent(pName)
{
    // Override the class name given by my parent.
    fMessageLogSettings.SetModuleName("BcmLnxEvent");
    
    fOwnerProcessId = getpid();
    fOwnerThreadId = pthread_self();
    fState = les_waiting;
	fEvtTimedout = false;
	fEvtOccurred = false;
    fMyTimeout = BcmLinuxTimeout::GetMyTimeout();
    fTimeoutId = 0;

	// Entering the critical section
	pthread_mutex_lock(&fLnxMutexLock);
	// Initialize the key once
	if(!fRunOnce)
	{
		fRunOnce = true;
		// Initialize the key
		pthread_key_create(&fKeyEvtInstance, NULL);
		pthread_key_create(&fKeyEvtSetInstance, NULL);
		pthread_key_create(&fKeyEvtSetQuantity, NULL);
		
		// Install the signal mask for finite wait
		sigemptyset (&fSigmask);
		sigaddset (&fSigmask, SIGUSR1);
		sigaddset (&fSigmask, SIGALRM);
		
		// Initialize the signal mask for infinite wait
		sigemptyset (&fSigmaskNoAlarm);
		sigaddset (&fSigmaskNoAlarm, SIGUSR1);
		
		// Install the signal handler
		struct sigaction action;
		action.sa_handler = BcmLnxEvent::EventSigHandler;
		sigemptyset (&action.sa_mask);
		sigaddset (&action.sa_mask, SIGALRM);
		action.sa_flags = SA_RESTART;
		
		sigaction (SIGUSR1, &action, NULL);
	}
	// Leaving this ciritical section
	pthread_mutex_unlock(&fLnxMutexLock);
}


// ----------------------------------------------------------------------------
// Destructor.  
//
// Parameters:  N/A
//
// Returns:  N/A
// ----------------------------------------------------------------------------
BcmLnxEvent::~BcmLnxEvent()
{
    // stop timeout
    if (fTimeoutId)
    {
        fMyTimeout = BcmLinuxTimeout::GetMyTimeout();
		fMyTimeout->CancelTimeout (fTimeoutId);
    }
}

// ----------------------------------------------------------------------------
// This is the method that thread X will call when it wants to "post" the
// event, telling the waiting thread Y (who created the event) that the event 
// has occurred.  Note that
// if the event has been posted and you send the event again, both of these
// are collapsed down to a single event.  It does not count the number of
// times an event has been posted.
//
// Parameters:  None.
//
// Returns:
//      true if successful.
//      false if there was a problem.
//
// ----------------------------------------------------------------------------
bool BcmLnxEvent::Send(void)
{
    bool retval = true;
        
	pthread_t tid;
	int oldPriority = 0; // Initialize to 0 to turn off warning
	int thdpolicy;
	struct sched_param thdparam;
	
	tid = pthread_self();
		
	// In case the event owner thread is posting to himself, block the ALRM signal
    if(fOwnerThreadId == tid)
	{
		sigset_t oldSigmask;
		sigset_t sigmask;
		
        sigemptyset (&sigmask);
        sigaddset (&sigmask, SIGALRM);
		pthread_sigmask (SIG_BLOCK, &sigmask, &oldSigmask);
		// At this point, SIGALRM handler can not run, it's safe to check fEvtTimedout.
		if(fEvtTimedout == false)
			fState = les_occurred;	
		pthread_sigmask (SIG_UNBLOCK, &sigmask, NULL);
	}
	else // Other threads are posting events to the event owner thread
	{
		// Get the current priority
		pthread_getschedparam(tid, &thdpolicy, &thdparam);
		// Save the old priority
		oldPriority = thdparam.sched_priority;
		// Set the new priority, hardcoded value for now, FIX ME!
		thdparam.sched_priority = 99;
		pthread_setschedparam(tid, thdpolicy, &thdparam);

		// At this point, the event owner thread can not run, neither does its signal handler.
		// So it's safe to do this
		if(fEvtTimedout == false)
			fState = les_occurred;	
	}
	
	// signal that event has occurred
	int result = pthread_kill (fOwnerThreadId, BcmEventSigNum);

	if (result)
	{
		gErrorMsg(fMessageLogSettings, "Send")
			<< "Failed to send the event!, errno= " << dec << (int) errno << "\n";

		retval = false;
	}

    if(fOwnerThreadId != tid)
	{
		// Restore the old priority
		thdparam.sched_priority = oldPriority;
		pthread_setschedparam(tid, thdpolicy, &thdparam);
	}
    return retval;
}



// ----------------------------------------------------------------------------
// This is the method that a thread will call when it wants to wait for
// an event to be sent.  Optionally, the thread can 
//      1. wait forever for the event, 
//      2. timeout if the event isn't sent within a certain number of
//         milliseconds, or 
//      3. return immediately regardless of whether or not the event has been sent
//
// This is the Linux-specific implementation for waiting for an event.
//
// Parameters:
//      mode - the wait mode (forever or timeout).
//      timeoutMS - the number of milliseconds to wait for the event to be
//                  sent; only used if mode is set to timeout.
//
// Returns:
//      true if the event occurred.
//      false if the event didn't occur (timed out waiting or something).
// ----------------------------------------------------------------------------
bool BcmLnxEvent::Wait(WaitMode mode, unsigned long timeoutMS)
{
	sigset_t oldSigmask;
	sigset_t *pSigmask = &fSigmaskNoAlarm;

    bool occurred = (fState == les_occurred);                // look at state right now
    if (occurred)
    {   // event has occurred - go ahead & return
        fState = les_waiting;      // event resolution reported, so reset it
    }
    else 
    {   
		if ((mode == kTimeout))
		{
			if(timeoutMS == 0)
				return occurred;
			else
				pSigmask = &fSigmask;
		}

		// Now block the signals interested
        // Then wait for event occurrence
		pthread_sigmask (SIG_BLOCK, pSigmask, &oldSigmask);
		if ((mode == kForever))
		{
			// block SIGALRM when suspended
			sigaddset (&oldSigmask, SIGALRM);
		}
		// Arm the time after blocking SIGALRM
		if ((mode == kTimeout))
		{
			fEvtTimedout = false;
			fMyTimeout = BcmLinuxTimeout::GetMyTimeout();
			fTimeoutId = fMyTimeout->AddTimeout (timeoutMS, BcmLinuxTimeout::ltc_event, this);
		}
		// Remember the instance
        pthread_setspecific(fKeyEvtInstance, this);

		// Safe here because signals are blocked
		fEvtOccurred = fEvtTimedout = false;
		// At this point, check the fState, which may have changed before we block the signals!
		if(fState == les_occurred)
		{
			fEvtOccurred = true;
		}
		// Wait for event occurrence
		while((fEvtOccurred == false) && (fEvtTimedout == false))
		{
			// Wait for signal indicating event or timeout
			sigsuspend (&oldSigmask); 
		}
		// No longer waiting, so clear the instance field
        pthread_setspecific(fKeyEvtInstance, NULL);
		// Unblock those signals
		pthread_sigmask (SIG_UNBLOCK, pSigmask, NULL);

        if (fState == les_occurred)
        {
            // We've received the event so cancel the timeout.
            // If it's already timed out, the Timeout code will handle
            // the double call with no issues.
            // Must wait until now to do the timeout.  If we do it when
            // we first know about the event (in Send) we could try to 
            // cancel using the wrong thread.  Very bad.
            if (fTimeoutId)
            {
                fMyTimeout = BcmLinuxTimeout::GetMyTimeout();
                fMyTimeout->CancelTimeout (fTimeoutId);
                fTimeoutId = 0;
            }
        }
        occurred = (fState == les_occurred);                // look at state right now
		// At this point, the timeout has either happened or been cancelled
		// So there is no race with the ALRM handler, safe to do this:
		fEvtTimedout = false;
		// If somebody posts the event right here, then it has no effect!
		fState = les_waiting;
		// If somebody posts the event right here, then there is a pending event
    }
    
    return occurred;
}

// ----------------------------------------------------------------------------
//  Called by master clock when a timeout has occurred - that is, a thread has 
//  been waiting for an Event to happen, and it didn't
// ----------------------------------------------------------------------------
void BcmLnxEvent::Timeout (void)
{
	pthread_t tid;
	int oldPriority;
	int thdpolicy;
	struct sched_param thdparam;
	
	tid = pthread_self();
	// Get the current priority
	pthread_getschedparam(tid, &thdpolicy, &thdparam);
	// Save the old priority
	oldPriority = thdparam.sched_priority;
	// Set the new priority
	thdparam.sched_priority = 99;
	pthread_setschedparam(tid, thdpolicy, &thdparam);

	// Nobody can preempt me now
	if(fState == les_waiting)
	    fEvtTimedout = true;
	
    // We've timed out, the timer should no longer exist,
    // so 0 LnxEvent copy of the Id.
    fTimeoutId = 0;

	// Restore the old priority
	thdparam.sched_priority = oldPriority;
	pthread_setschedparam(tid, thdpolicy, &thdparam);
}

// ----------------------------------------------------------------------------
// signal handler - invoked when event occurs
// there is really nothing for me to do - elsewhere I just wait for this signal
// ----------------------------------------------------------------------------
void BcmLnxEvent::EventSigHandler (int signal_number)
{
	// Get the instance from the thread-specific storage 
	BcmLnxEvent *pEvtInstance = (BcmLnxEvent *) pthread_getspecific(fKeyEvtInstance);
	BcmLnxEventSet *pEvtSetInstance = (BcmLnxEventSet *) pthread_getspecific(fKeyEvtSetInstance);

	// Sanity checks
	if((pEvtSetInstance != NULL) && (pEvtInstance != NULL))
	{
        cout << "A thread is waiting on both an event and an eventset, impossible!!!" << endl;
	}

	// Timed out waiting for an event
	if(pEvtInstance != NULL)
	{
		if((pEvtInstance->fEvtTimedout != true) && (pEvtInstance->fState == les_occurred))
		{
			pEvtInstance->fEvtOccurred = true;
		}
	}

	// Timed out waiting for an eventset
	if(pEvtSetInstance != NULL)
	{
		BcmEventSet::Quantity *pQuantity = (BcmEventSet::Quantity *)pthread_getspecific(fKeyEvtSetQuantity);
		pEvtSetInstance->CheckNow(*pQuantity);
	}
}

//pthread_t BcmLnxEvent::GetThreadId(void)
//{
//	return fOwnerThreadId;
//}

// ============================================================================
// ======== END ========== END ========== END ========== END ========== END ===
// ============================================================================



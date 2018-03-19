//****************************************************************************
//
//  $Id: LnxMutexEvent.cpp 1.4 2006/08/02 19:50:35Z taskiran Release $
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
// NOTICE !!!
//  LnxMutexEvent is a class specifically designed for the use of the current
//LnxMutex class.  The problem this emplimentation overcomes, is that LnxEvent
//ends up instantiating a class 'BcmCommandTable', which instantiates a LnxMutex,
//so on and so on.  LnxMutexEvent does not do this, but has all the other
//functionality of LnxEvent. If LnxMutexSemaphore.cpp is ever changed back to
//be just a wrapper for OS mutex's again, then this class will not be needed
//anymore. MDR
//********************** Include Files ***************************************

#include <errno.h>

#include "LnxMutexEvent.h"
#include "LnxTimeout.h"
#include <unistd.h>
#include <pthread.h>

bool LnxMutexEvent::fSigMaskInitted = false;
sigset_t LnxMutexEvent::fSigmask;

//#if (BCM_SETTOP_SUPPORT)
//extern "C"
//{
//	extern pthread_t dhcp_thread_id;
//	extern void sigHandler(int);
//}
//#endif

//********************** Local Types *****************************************

//********************** Local Constants *************************************

//********************** Local Variables *************************************

//********************** Class Method Implementations ************************

extern void set_alarm_handler (void);


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
LnxMutexEvent::LnxMutexEvent(const char *pName)
{
    
    fOwnerProcessId = getpid();
    fOwnerThreadId = pthread_self();
    fState = les_idle;
    fMyTimeout = BcmLinuxTimeout::GetMyTimeout();
    fTimeoutId = 0;

    if (!fSigMaskInitted)   
    {   // initialize static signal mask

        sigemptyset (&fSigmask);
        sigaddset (&fSigmask, SIGUSR2);
        sigaddset (&fSigmask, SIGALRM);

        fSigMaskInitted = true;
    }

    signal( BcmEventSigNum, LnxMutexEvent::EventSigHandler );

}


// ----------------------------------------------------------------------------
// Destructor.  
//
// Parameters:  N/A
//
// Returns:  N/A
// ----------------------------------------------------------------------------
LnxMutexEvent::~LnxMutexEvent()
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
bool LnxMutexEvent::Send(void)
{
// TEMP TEMP TEMP    printf("LnxMutexEvent::Send \n");
    bool retval = true;
        
    fState = les_occurred;

    // signal that event has occurred
    int result = pthread_kill (fOwnerThreadId, BcmEventSigNum);

    if (result)
        retval = false;

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
bool LnxMutexEvent::Wait(WaitMode mode, unsigned long timeoutMS)
{

    bool occurred = (fState == les_occurred);                // look at state right now
    if (occurred)
    {   // event has occurred - go ahead & return
        fState = les_idle;      // event resolution reported, so reset it
    }
    else if ((mode == kTimeout) && (timeoutMS==0))
    {   // event has not occurred, and caller doesn't want to wait for it
        if (fState == les_timedout)
            fState = les_idle;      // event resolution reported, so reset it
    }
    else 
    {        // event has not occurred, wait for it (bounded or unbounded)
        if (mode == kTimeout)
        {
            fMyTimeout = BcmLinuxTimeout::GetMyTimeout();
            fTimeoutId = fMyTimeout->AddTimeout (timeoutMS, BcmLinuxTimeout::ltc_event, this);
        }
        
        // wait for event occurrence
        fState = les_waiting;
        for (bool done = false; !done;)
        {
            pthread_sigmask (SIG_BLOCK, NULL, &fSigmask);
            
            sigsuspend (&fSigmask); // wait for signal indicating event or timeout
            // got some signal
            // I have to make sure that the signal was really meant for me
            if (fState != les_waiting)   
                done = true;
        }   
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
        fState = les_idle;      // done waiting on state, reset it
    }
    
    return occurred;
}

// ----------------------------------------------------------------------------
//  Called by master clock when a timeout has occurred - that is, a thread has 
//  been waiting for an Event to happen, and it didn't
// ----------------------------------------------------------------------------
void LnxMutexEvent::Timeout (void)
{

    fState = les_timedout;
	
    // We've timed out, the timer should no longer exist,
    // so 0 LnxEvent copy of the Id.
    fTimeoutId = 0;
//    int result = pthread_kill (fOwnerThreadId, BcmEventSigNum);
    pthread_kill (fOwnerThreadId, BcmEventSigNum);
}

// ----------------------------------------------------------------------------
// signal handler - invoked when event occurs
// there is really nothing for me to do - elsewhere I just wait for this signal
// ----------------------------------------------------------------------------
void LnxMutexEvent::EventSigHandler (int signal_number)
{
    // have to reset myself as signal handler?
    signal( BcmEventSigNum, LnxMutexEvent::EventSigHandler );

//#if (BCM_SETTOP_SUPPORT)
//	if(pthread_self() == dhcp_thread_id)
//	{
//		// Never returns!
//		sigHandler(signal_number);
//	}
//#endif    
}



// ============================================================================
// ======== END ========== END ========== END ========== END ========== END ===
// ============================================================================



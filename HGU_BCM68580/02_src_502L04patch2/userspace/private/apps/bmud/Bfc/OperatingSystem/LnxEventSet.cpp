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
//  $Id: LnxEventSet.cpp 1.3 2006/08/02 19:50:27Z taskiran Release $
//****************************************************************************

//********************** Include Files ***************************************

#include <signal.h>
#include "LnxEventSet.h"
#include "LnxEvent.h"
#include "LnxTimeout.h"

//********************** Local Types *****************************************

//********************** Local Constants *************************************

//********************** Local Variables *************************************

//********************** Class Method Implementations ************************


// ----------------------------------------------------------------------------
// Default Constructor.  Stores the name assigned to the object.
//
// Parameters:
//      pName - the text name that is to be given to the object, for
//              debugging purposes.
//
// Returns:  N/A
// ----------------------------------------------------------------------------
BcmLnxEventSet::BcmLnxEventSet(const char *pName) :
    BcmEventSet(pName)
{
    // Override the class name given by my parent.
    fMessageLogSettings.SetModuleName("BcmLnxEventSet");
    
    fMyTimeout = BcmLinuxTimeout::GetMyTimeout();
    fTimeoutId = 0;
    fEvtSetOccurred = false;
    fEvtSetTimedout = false;
}


// ----------------------------------------------------------------------------
// Destructor.  Frees up any memory/objects allocated, cleans up internal
// state.
//
// Parameters:  N/A
//
// Returns:  N/A
// ----------------------------------------------------------------------------
BcmLnxEventSet::~BcmLnxEventSet()
{
    // Call RemoveAll() to clear out any remaining events.
    if (fTimeoutId)
    {
        fMyTimeout = BcmLinuxTimeout::GetMyTimeout();
        fMyTimeout->CancelTimeout (fTimeoutId);
    }
    RemoveAll();
}


// ----------------------------------------------------------------------------
// Check on the occurrence of events in the set
//
// Linux flavor notes:
// When a thread constructs an Event, that thread will be notified if/when
// the Event occurs. The thread gets the notification by receiving a signal.
// This class works under the assumption that the same thread
// that constructed the Events later called BcmLnxEventSet::Wait(). 
// I will intercept Event notifications by waiting on a signal. 
//
// If the caller wants to wait on just any Event, it is pretty straightforward
// for me. However, if the caller wants to wait on ALL Events, I need to keep
// track of which have happened.
//
// Parameters:
//      quantity - tells whether the thread should be released when any
//                 event is sent, or if it should wait for all of them.
//      mode - tells whether to wait forever, or to timeout.
//      timeoutMS - tells how long to wait if mode is kTimeout.
//
// Returns:
//      true if one or more of the events occurred.
//      false if none of the events occurred (timeout).
// ----------------------------------------------------------------------------
bool BcmLnxEventSet::Wait(Quantity quantity,
                           BcmEvent::WaitMode mode,
                           unsigned long timeoutMS)
{
    // farm this out to appropriate sub-functions
    if ((mode == BcmEvent::kTimeout)&& (!timeoutMS))
        return CheckNow (quantity);
    else 
        return WaitAwhile (quantity, mode, timeoutMS);
}



// ----------------------------------------------------------------------------
// check if any or all events are ready
// immediate return, no matter how many Events I get
//
// Parameters:
//      quantity: 
//            kAny: return true if any event has occurred
//            kAll: return true if all events have occurred
//
// Returns:
//      true if "quantity" satisfied
//      false otherwise
//
// ----------------------------------------------------------------------------
bool BcmLnxEventSet::CheckNow (Quantity quantity)
{
    bool success;
    
    if (quantity == kAny)
        success = anyOccurred();
    else
	{
        success = allOccurred();
		if(success)
			cout << "all occurred!" << endl;
	}

	if(!fEvtSetTimedout)
		fEvtSetOccurred = success;
    return success;
}




// ----------------------------------------------------------------------------
// get event reports, wait (maybe forever) if necessary
//
// Parameters:
//      quantity: 
//            kAny: return true if any event has occurred
//            kAll: return true if all events have occurred
//      mode: wait forever or a certain timeout
//      timeoutms: the amount of time to wait (if any)
//
// Returns:
//      true if "quantity" satisfied
//      false otherwise
//
// ----------------------------------------------------------------------------
bool BcmLnxEventSet::WaitAwhile (BcmEventSet::Quantity quantity, BcmEvent::WaitMode mode, 
                                 unsigned long timeoutms)
{
    bool satisfied;

    // maybe I don't need to wait at all because the condition is already satisfied
    satisfied = CheckNow (quantity);

    if (!satisfied)
    {
        if (mode == BcmEvent::kForever)
            satisfied = InfiniteWait (quantity);
        else
            satisfied = FiniteWait (quantity, timeoutms);

    }
    return satisfied;
}


// ----------------------------------------------------------------------------
// Wait until the specified condition (any event or all events) has occurred
// return true
// ----------------------------------------------------------------------------
bool BcmLnxEventSet::InfiniteWait (Quantity quantity)
{
    sigset_t sigmask;

	pthread_sigmask (SIG_BLOCK, &BcmLnxEvent::fSigmaskNoAlarm, &sigmask);

    pthread_setspecific(BcmLnxEvent::fKeyEvtSetQuantity, &quantity);
    pthread_setspecific(BcmLnxEvent::fKeyEvtSetInstance, this);
	fEvtSetTimedout = fEvtSetOccurred = false;
    CheckNow (quantity);
    while ((fEvtSetTimedout == false) && (fEvtSetOccurred == false))
    {
//		sigdelset(&sigmask, SIGUSR1);
		// wait for signal indicating event or timeout
        sigsuspend (&sigmask); 
    }
    pthread_setspecific(BcmLnxEvent::fKeyEvtSetQuantity, NULL);
    pthread_setspecific(BcmLnxEvent::fKeyEvtSetInstance, NULL);
	pthread_sigmask (SIG_UNBLOCK, &BcmLnxEvent::fSigmaskNoAlarm, NULL);
    return fEvtSetOccurred;    // I know, done has to be true to exit... 
}



// ----------------------------------------------------------------------------
// Called by LnxTimeout when my timer expires
// ----------------------------------------------------------------------------
void BcmLnxEventSet::Timeout (void)
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

	if(!fEvtSetOccurred)
	    fEvtSetTimedout = true;
	
    // We've timed out, the timer should no longer exist,
    // so 0 LnxEvent copy of the Id.
    fTimeoutId = 0;

	// Restore the old priority
	thdparam.sched_priority = oldPriority;
	pthread_setschedparam(tid, thdpolicy, &thdparam);
}



// ----------------------------------------------------------------------------
// Wait for the specified condition (any event or all events) 
// Stop waiting after specified interval
// return: 
//      true = condition satisfied
//      false = timeout
// ----------------------------------------------------------------------------
bool BcmLnxEventSet::FiniteWait (Quantity quantity, unsigned long timeoutms)
{
    sigset_t sigmask;

	pthread_sigmask (SIG_BLOCK, &BcmLnxEvent::fSigmask, &sigmask);

	// Arm the timer
    fMyTimeout = BcmLinuxTimeout::GetMyTimeout();
    fTimeoutId = fMyTimeout->AddTimeout (timeoutms, BcmLinuxTimeout::ltc_evtset, this);

	// Get the instance and arguments
    pthread_setspecific(BcmLnxEvent::fKeyEvtSetQuantity, &quantity);
    pthread_setspecific(BcmLnxEvent::fKeyEvtSetInstance, this);
	fEvtSetTimedout = fEvtSetOccurred = false;
    CheckNow (quantity);
    while ((fEvtSetTimedout == false) && (fEvtSetOccurred == false))
    {
		sigdelset(&sigmask, SIGUSR1);
		// wait for signal indicating event or timeout
        sigsuspend (&sigmask); 
    }
    pthread_setspecific(BcmLnxEvent::fKeyEvtSetQuantity, NULL);
    pthread_setspecific(BcmLnxEvent::fKeyEvtSetInstance, NULL);
	
	pthread_sigmask (SIG_UNBLOCK, &BcmLnxEvent::fSigmask, NULL);

	if(fEvtSetOccurred)
	{
        fMyTimeout = BcmLinuxTimeout::GetMyTimeout();
        fMyTimeout->CancelTimeout (fTimeoutId);
        fTimeoutId = 0;
	}
    return fEvtSetOccurred;    // I know, done has to be true to exit... 
}



// ----------------------------------------------------------------------------
// This method is called by Add() in order to allow the derived class to do
// its OS-specific stuff.
//
// Parameters:
//      pEvent - pointer to the event to add.
//
// Returns:
//      true if successful.
//      false if there was a problem (too many events, etc.).
// ----------------------------------------------------------------------------
bool BcmLnxEventSet::DerivedAdd(BcmEvent *pEvent)
{
    // Add this event to my collection
    BcmLnxEvent * qP = (BcmLnxEvent * )pEvent;
    bool occurred = false;
    fMyEvents[qP] = occurred;

    return true;
}


// ----------------------------------------------------------------------------
// This method is called by Remove() in order to allow the derived class to
// do its OS-specific stuff.
//
// Parameters:
//      pEvent - pointer to the event to remove.
//
// Returns:
//      true if successful.
//      false if the event isn't a member of the set, other problem.
// ----------------------------------------------------------------------------
bool BcmLnxEventSet::DerivedRemove(BcmEvent *pEvent)
{
    // Remove this event from my collection
    BcmLnxEvent * qP = (BcmLnxEvent * )pEvent;
    fMyEvents.erase(qP);

    return true;
}


// ----------------------------------------------------------------------------
// This method is called by RemoveAll() in order to allow the derived class
// to do its OS-specific stuff.
//
// Parameters:  None.
//
// Returns:
//      true if successful.
//      false if the event set was already empty.
// ----------------------------------------------------------------------------
bool BcmLnxEventSet::DerivedRemoveAll(void)
{
    // empty my collection
    if( !fMyEvents.empty() )
        fMyEvents.erase( fMyEvents.begin(), fMyEvents.end() );

    return true;
}


// ----------------------------------------------------------------------------
// This method is called by Occurred() in order to allow the derived class
// to do its OS-specific stuff.
//
// Parameters:
//      pEvent - the event to check for being set.
//      clearEvent - if true, resets the event; otherwise, leaves it set.
//
// Returns:
//      true if the event is set.
//      false if the event is not set.
// ----------------------------------------------------------------------------
bool BcmLnxEventSet::DerivedOccurred(BcmEvent *pEvent, bool clearEvent)
{
    bool retval = false;
    BcmLnxEvent * qP = (BcmLnxEvent * ) pEvent;
    MapEventsIds::iterator mapI = fMyEvents.find(qP);
    if (mapI != fMyEvents.end())
    {
        bool * itDid = &((*mapI).second);    // pointer to element in my map
        retval = *itDid;
        if (clearEvent)
            *itDid = false;
     }

    return retval;
}


// ----------------------------------------------------------------------------
// This method returns the Event BitSet to the caller.
// Currently only used for print debug, so not implemented for now
// \retval
//      The EventBitSet 
//
// ----------------------------------------------------------------------------
uint32 BcmLnxEventSet::Derived(void)
{
    return 0;
}

// ----------------------------------------------------------------------------
// This method looks real-time at the events pending and checks to see if a particular
// event is pending instead of having to do a Wait() and store the value to be checked later.
//
// \retval
//      true if the event is set.
// \retval
//      false if the event is not set.
//
// ----------------------------------------------------------------------------
bool BcmLnxEventSet::PeekOccurred(BcmEvent *pEvent)
{
    return DerivedOccurred(pEvent, false);
}

// ----------------------------------------------------------------------------
// Returns:
//      true if all of my events have occurred
//      false otherwise
// ----------------------------------------------------------------------------
bool BcmLnxEventSet::allOccurred(void)
{
    bool allDid = true;

    MapEventsIds::iterator mapI = fMyEvents.begin();
    // look through all my events, unless I find one that hasn't occurred
    for ( ; (mapI != fMyEvents.end());   ++mapI)
    {
        bool * itdid = &((*mapI).second);    // pointer to element in my map
        if (*itdid == false)
        {   // I don't have a record that this one occurred
            // note that once I see that it has occurred, if I check it again,
            // it will say it has not occurred
            // check it again
            BcmLnxEvent * eP = *(&((*mapI).first)); // get ptr to Event
            *itdid = eP->Wait(BcmEvent::kTimeout, 0);  // see if it occurred
            
            allDid &= *itdid;    
        }
    }

    return allDid;
}

// ----------------------------------------------------------------------------
// Returns:
//      true if any of my events have occurred
//      false otherwise
// ----------------------------------------------------------------------------
bool BcmLnxEventSet::anyOccurred(void)
{
    bool anyDid = false;

    MapEventsIds::iterator mapI = fMyEvents.begin();
    // look through all my events, unless I find one that hasn't occurred
    for ( ; (mapI != fMyEvents.end());   ++mapI)
    {
        bool * itdid = &((*mapI).second);    // pointer to element in my map
        if (*itdid == false)
        {   // I don't have a record that this one occurred
            // note that once I see that it has occurred, if I check it again,
            // it will say it has not occurred
            // check it again
            BcmLnxEvent * eP = *(&((*mapI).first)); // get ptr to Event
            *itdid = eP->Wait(BcmEvent::kTimeout, 0);  // see if it occurred
            
        }
        anyDid |= *itdid;    
    }

    return anyDid;
}

// ============================================================================
// ======== END ========== END ========== END ========== END ========== END ===
// ============================================================================


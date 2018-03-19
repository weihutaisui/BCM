//****************************************************************************
//
//  Copyright (c) 2000  Broadcom Corporation
//  All Rights Reserved
//  No portions of this material may be reproduced in any form without the
//  written permission of:
//          Broadcom Corporation
//          16251 Laguna Canyon Road
//          Irvine, California 92618
//  All information contained in this document is Broadcom Corporation
//  company private, proprietary, and trade secret.
//
//  $Id: LnxEventSet.h 1.4 2006/08/02 19:50:30Z taskiran Release $
//****************************************************************************

#ifndef LNXEVENTSET_H
#define LNXEVENTSET_H

//********************** Include Files ***************************************

// My base class...
#include "EventSet.h"

class BcmLnxEvent;
class BcmLinuxTimeout;
#include <map>

//********************** Global Types ****************************************

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Class Declaration ***********************************


class BcmLnxEventSet : public BcmEventSet
{
public:

    // Default Constructor.  Stores the name assigned to the object.
    //
    // Parameters:
    //      pName - the text name that is to be given to the object, for
    //              debugging purposes.
    //
    // Returns:  N/A
    //
    BcmLnxEventSet(const char *pName = NULL);

    // Destructor.  Frees up any memory/objects allocated, cleans up internal
    // state.
    //
    // Parameters:  N/A
    //
    // Returns:  N/A
    //
    ~BcmLnxEventSet();

    // Blocks the calling thread until the conditions specified in the
    // parameters are satisfied.  By default, the thread will be blocked until
    // any of the events in the set is sent.  Optionally, the thread can be
    // released from waiting after a timeout, and it can wait until all of the
    // events in the set are set.
    //
    // Most threads will use the default parameters in their event handling
    // loop.
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
    //
    bool Wait(Quantity quantity = kAny,
                      BcmEvent::WaitMode mode = BcmEvent::kForever, 
                      unsigned long timeoutMS = 0);

    // Called by LnxTimeout when my timer expires
    void Timeout (void);


private:


    // This method is called by Add() in order to allow the derived class to do
    // its OS-specific stuff.
    //
    // Parameters:
    //      pEvent - pointer to the event to add.
    //
    // Returns:
    //      true if successful.
    //      false if there was a problem (too many events, etc.).
    //
    bool DerivedAdd(BcmEvent *pEvent);

    // This method is called by Remove() in order to allow the derived class to
    // do its OS-specific stuff.
    //
    // Parameters:
    //      pEvent - pointer to the event to remove.
    //
    // Returns:
    //      true if successful.
    //      false if the event isn't a member of the set, other problem.
    //
    bool DerivedRemove(BcmEvent *pEvent);

    // This method is called by RemoveAll() in order to allow the derived class
    // to do its OS-specific stuff.
    //
    // Parameters:  None.
    //
    // Returns:
    //      true if successful.
    //      false if the event set was already empty.
    //
    bool DerivedRemoveAll(void);

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
    //
    bool DerivedOccurred(BcmEvent *pEvent, bool clearEvent);

    /// This method returns the Event BitSet to the caller.
    ///
    /// \retval
    ///      The EventBitSet 
    ///
    uint32 Derived(void);

    /// This method looks real-time at the events pending and checks to see if a particular
    /// event is pending instead of having to do a Wait() and store the value to be checked later.
    ///
    /// \retval
    ///      true if the event is set.
    /// \retval
    ///      false if the event is not set.
    ///
    bool PeekOccurred(BcmEvent *pEvent);

    // Returns:
    //      true if all of my events have occurred
    //      false otherwise
    bool allOccurred(void);

    // Returns:
    //      true if any of my events have occurred
    //      false otherwise
    bool anyOccurred(void);
    
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
    bool WaitAwhile (BcmEventSet::Quantity quantity, BcmEvent::WaitMode mode, 
                                     unsigned long timeoutms);

    // check if any or all events are ready
    // immediate return, no matter how many Events have occurred
    //
    // Parameters:
    //      quantity: 
    //            kAny: return true if any event has occurred
    //            kAll: return true if all events have occurred
    //
    // Returns:
    //      true if "quantity" satisfied
    //      false otherwise
    bool CheckNow (Quantity quantity);

    // Wait until the specified condition (any event or all events) has occurred
    // return true
    bool InfiniteWait (Quantity quantity);

    // Wait for the specified condition (any event or all events) 
    // Stop waiting after specified interval
    // Parameters:
    //      quantity: 
    //            kAny: return true if any event has occurred
    //            kAll: return true if all events have occurred
    //      timeoutms: the amount of time to wait (if any)
    // return: 
    //      true = condition satisfied
    //      false = timeout
    bool FiniteWait (Quantity quantity, unsigned long timeoutms);

    typedef struct 
    {
        BcmLnxEvent * evPtr;
        bool occurred;
    } tEvInfo;
    // a collection of the Events I hold and their id codes
    typedef map< BcmLnxEvent *, bool, less<BcmLnxEvent *> > MapEventsIds;
	MapEventsIds fMyEvents;

    ulong fTimeoutId;
    BcmLinuxTimeout * fMyTimeout; 

	// Flag to tell the waiting thread one or more events have been posted
	bool fEvtSetOccurred;
	// Flag to tell the waiting thread time is up
	bool fEvtSetTimedout;

	// Open access to my friend
    friend class BcmLnxEvent;
};


//********************** Inline Method Implementations ***********************

#endif



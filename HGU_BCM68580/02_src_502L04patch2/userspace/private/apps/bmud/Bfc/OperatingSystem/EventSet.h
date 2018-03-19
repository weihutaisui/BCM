//****************************************************************************
//
// Copyright (c) 1999-2009 Broadcom Corporation
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
//  Filename:       EventSet.h
//  Author:         David Pullen
//  Creation Date:  March 1, 1999
//
//****************************************************************************

#ifndef EVENTSET_H
#define EVENTSET_H

//********************** Include Files ***************************************

// The portable types.
#include "typedefs.h"

// For storing the events.
#include <vector>

// I need this for some of the enums that are in BcmEvent.                     
#include "Event.h"

//********************** Global Types ****************************************

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Forward Declarations ********************************

//********************** Class Declaration ***********************************


/** \ingroup OsWrappers
*
*   This is the abstract base class for event sets; an event set is a
*   container class that allows individual sources of events (like BcmEvent,
*   BcmTimer, and BcmMessageQueue) to be grouped together an operated on
*   simultaneously.  This is necessary because BcmEvent, BcmTimer, and
*   others only allow you to operate on that particular instance.  The
*   primary use of this class is to allow a thread to wait for one or more
*   of a set of events to occur.  The API is similar to the BcmEvent API,
*   but with some additions to handle multiple events.
*
*   Note that, unlike BcmEvent and BcmMessageQueue, this class is NOT meant
*   to be used as an interface between multiple threads.  It is simply a
*   container class that helps a thread manage its set of events.
*
*   In all of the methods of this class where there is a BcmEvent parameter,
*   you can substitute a BcmTimer, BcmMessageQueue, or any other class that
*   provides an explicit type conversion to BcmEvent.
*
*   Any class that has an associated BcmEvent (such as BcmTimer and
*   BcmMessageQueue) can be added to the event set.  For example, this
*   allows a thread to wait for an event to be sent, a message to be
*   received, or a timer to expire, which ever comes first.  A thread can
*   also wait for a combination of events before returning, such as a
*   received message and a timer expiration.
*
*   Note that all of the events stored here must be created/owned by the
*   thread that created/owns the event set.  This is because the events
*   are only valid in the context of the thread that created them.
*/
class BcmEventSet
{
public:

    /// Default Constructor.  Stores the name assigned to the object.
    ///
    /// \param
    ///      pName - the text name that is to be given to the object, for
    ///              debugging purposes.
    ///
    BcmEventSet(const char *pName = NULL);

    /// Destructor.  Frees up any memory/objects allocated, cleans up internal
    /// state.
    ///
    virtual ~BcmEventSet();

    /// Adds the given event to the set.  If the event is already in the set,
    /// then it will not be added again.
    ///
    /// \param
    ///      pEvent - pointer to the event to add.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem (too many events, etc.).
    ///
    bool Add(BcmEvent *pEvent);

    /// Removes the given event from the set.  If the event wasn't a member of
    /// the set, then false is returned.
    ///
    /// \param
    ///      pEvent - pointer to the event to remove.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if the event isn't a member of the set, other problem.
    ///
    bool Remove(BcmEvent *pEvent);

    /// Clears the event set, removing all of the events.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if the event set was already empty.
    ///
    bool RemoveAll(void);

    /// This method causes the event set to call BcmEvent::Send() for each event
    /// in the set.  Note that some events are tied to other objects (like timers
    /// and message queues) and that sending these events may not make sense.  It
    /// is up to the client to make sure that the events that are in the set
    /// should be sent.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem.
    ///
    bool Send(void);

    typedef enum
    {
        kAll = 0,
        kAny
    } Quantity;

    /// Blocks the calling thread until the conditions specified in the
    /// parameters are satisfied.  By default, the thread will be blocked until
    /// any of the events in the set is sent.  Optionally, the thread can be
    /// released from waiting after a timeout, and it can wait until all of the
    /// events in the set are set.
    ///
    /// Most threads will use the default parameters in their event handling
    /// loop.
    ///
    /// \param
    ///      quantity - tells whether the thread should be released when any
    ///                 event is sent, or if it should wait for all of them.
    /// \param
    ///      mode - tells whether to wait forever, or to timeout.
    /// \param
    ///      timeoutMS - tells how long to wait if mode is kTimeout.
    ///
    /// \retval
    ///      true if one or more of the events occurred.
    /// \retval
    ///      false if none of the events occurred (timeout).
    ///
    virtual bool Wait(Quantity quantity = kAny,
                      BcmEvent::WaitMode mode = BcmEvent::kForever, 
                      unsigned long timeoutMS = 0) = 0;

    /// This method will be used by a client thread to see if the specified
    /// event has been triggered.  This is usually done immediately after the
    /// call to Wait() returns.
    ///
    /// By default, the event will be cleared after calling this method so that
    /// future calls will return false (unless the event is sent again).  If
    /// clearEvent is set to true, then the event will not be cleared, and
    /// future calls to Occurred() will return true.
    ///
    /// \param
    ///      pEvent - the event to check for being set.
    /// \param
    ///      clearEvent - if true, resets the event; otherwise, leaves it set.
    /// \param
    ///      peekEvent - do we want to check the event real-time?
    ///
    /// \retval
    ///      true if the event is set.
    /// \retval
    ///      false if the event is not set.
    ///
    bool Occurred(BcmEvent *pEvent, bool clearEvent = true, bool peekEvent = false);

    /// Simple accessor for the name of this object.
    ///
    /// \return
    ///      The pointer to the name string.
    ///
    inline const char *Name(void);

    /// This is our print method to dump the entire event set.
    ///
    void Print(void);

protected:

    /// This method is called by Add() in order to allow the derived class to do
    /// its OS-specific stuff.
    ///
    /// \param
    ///      pEvent - pointer to the event to add.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem (too many events, etc.).
    ///
    virtual bool DerivedAdd(BcmEvent *pEvent) = 0;

    /// This method is called by Remove() in order to allow the derived class to
    /// do its OS-specific stuff.
    ///
    /// \param
    ///      pEvent - pointer to the event to remove.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if the event isn't a member of the set, other problem.
    ///
    virtual bool DerivedRemove(BcmEvent *pEvent) = 0;

    /// This method is called by RemoveAll() in order to allow the derived class
    /// to do its OS-specific stuff.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if the event set was already empty.
    ///
    virtual bool DerivedRemoveAll(void) = 0;

    /// This method is called by Occurred() in order to allow the derived class
    /// to do its OS-specific stuff.
    ///
    /// \param
    ///      pEvent - the event to check for being set.
    /// \param
    ///      clearEvent - if true, resets the event; otherwise, leaves it set.
    ///
    /// \retval
    ///      true if the event is set.
    /// \retval
    ///      false if the event is not set.
    ///
    virtual bool DerivedOccurred(BcmEvent *pEvent, bool clearEvent) = 0;

    /// This method returns the Event BitSet to the caller.
    ///
    /// \retval
    ///      The EventBitSet 
    ///
    virtual uint32 Derived(void) = 0;

    /// This method looks real-time at the events pending and checks to see if a particular
    /// event is pending instead of having to do a Wait() and store the value to be checked later.
    ///
    /// \retval
    ///      true if the event is set.
    /// \retval
    ///      false if the event is not set.
    ///
    virtual bool PeekOccurred(BcmEvent *pEvent) = 0;

protected:

    /// Controls message logging.
    BcmMessageLogSettings fMessageLogSettings;

private:

    /// The set of events that has been added to this class.
    vector<BcmEvent *> fEventList;

    /// My assigned name.
    char *pfName;

private:

    /// Copy Constructor.  Not supported.
    BcmEventSet(BcmEventSet &otherInstance);

    /// Assignment operator.  Not supported.
    BcmEventSet & operator = (BcmEventSet &otherInstance);

};


//********************** Inline Method Implementations ***********************

/// Simple accessor for the name of this object.
///
/// \return
///      The pointer to the name string.
///
inline const char *BcmEventSet::Name(void)
{
    return pfName;
}

#endif



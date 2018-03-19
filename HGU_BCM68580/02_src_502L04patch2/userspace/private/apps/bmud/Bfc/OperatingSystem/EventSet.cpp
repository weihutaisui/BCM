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
//  Filename:       EventSet.cpp
//  Author:         David Pullen
//  Creation Date:  March 1, 1999
//
//****************************************************************************
//  Description:
//      This is the abstract base class for event sets; an event set is a
//      logical grouping of individual events (like BcmEvent, BcmTimer, and
//      BcmMessageQueue), and it allows you to act on the set of events all
//      at once.  This is most useful when waiting for one or more of a set
//      of events to occur.
//
//      Note that, unlike BcmEvent and BcmMessageQueue, this class is NOT meant
//      to be used as an interface between multiple threads.  It is simply a
//      container class that helps a thread manage its set of events.
//
//      In all of the methods of this class where there is a BcmEvent parameter,
//      you can substitute a BcmTimer, BcmMessageQueue, or any other class that
//      provides an explicit type conversion to BcmEvent.
//      
//****************************************************************************

//********************** Include Files ***************************************

// My api and definitions...
#include "EventSet.h"

#include "OperatingSystem.h"

// Some STL angorithms.
#include <algorithm>
#if defined (TARGETOS_eCos)
#include "ecosEvent.h"
#elif defined (TARGETOS_Linux)
#include "LnxEvent.h"
#endif


//********************** Local Types *****************************************

//********************** Local Constants *************************************

//********************** Local Variables *************************************

//********************** Forward Declaration *********************************

//********************** Class Method Implementations ************************


// Default Constructor.  Stores the name assigned to the object.
//
// Parameters:
//      pName - the text name that is to be given to the object, for
//              debugging purposes.
//
// Returns:  N/A
//
BcmEventSet::BcmEventSet(const char *pName) :
    fMessageLogSettings("BcmEventSet")
{
    // Call the helper method to create my object name.
    pfName = BcmOperatingSystem::GenerateName(pName, "EventSet");

    // Set my instance name.
    fMessageLogSettings.SetInstanceName(pfName);

    fMessageLogSettings.Register();

    // Make the vector reserve enough memory for at least 32 items so as to
    // minimize future resizing.
    fEventList.reserve(32);
}


// Destructor.  Frees up any memory/objects allocated, cleans up internal
// state.
//
// Parameters:  N/A
//
// Returns:  N/A
//
BcmEventSet::~BcmEventSet()
{
    fMessageLogSettings.Deregister();

    // Assume that the derived class has already called RemoveAll().

    // Clean up other members.
    delete pfName;
    pfName = NULL;
}


// Adds the given event to the set.  If the event is already in the set,
// then it will not be added again.
//
// Parameters:
//      pEvent - pointer to the event to add.
//
// Returns:
//      true if successful.
//      false if there was a problem (too many events, etc.).
//
bool BcmEventSet::Add(BcmEvent *pEvent)
{
    vector<BcmEvent *>::iterator myIterator;

    // Make sure it's a valid event.
    if (pEvent == NULL)
    {
        gErrorMsg(fMessageLogSettings, "Add") << "Event parameter is NULL!" << endl;

        return false;
    }

    // Make sure it isn't already in the list, if the list isn't empty.
    if (!fEventList.empty())
    {
        // Find my event.
        myIterator = find(fEventList.begin(), fEventList.end(), pEvent);

        if (myIterator != fEventList.end())
        {
            return true;
        }
    }

    // Add it to the list.
    fEventList.push_back(pEvent);

    // Let the derived class do its thing.
    return DerivedAdd(pEvent);
}


// Removes the given event from the set.  If the event wasn't a member of
// the set, then false is returned.
//
// Parameters:
//      pEvent - pointer to the event to remove.
//
// Returns:
//      true if successful.
//      false if the event isn't a member of the set, other problem.
//
bool BcmEventSet::Remove(BcmEvent *pEvent)
{
    vector<BcmEvent *>::iterator myIterator;

    // Make sure it's a valid event.
    if (pEvent == NULL)
    {
        gErrorMsg(fMessageLogSettings, "Remove") << "Event parameter is NULL!" << endl;

        return false;
    }

    // Only do this if the list isn't empty.
    if (!fEventList.empty())
    {
        // Find my event.
        myIterator = find(fEventList.begin(), fEventList.end(), pEvent);

        if (myIterator != fEventList.end())
        {
            // Remove it from the vector.
            fEventList.erase(myIterator);
            
            // Let the derived class do its thing.
            return DerivedRemove(pEvent);
        }
    }

    // Didn't find it!
    gWarningMsg(fMessageLogSettings, "Remove") << "Specified event isn't in the set!" << endl;

    return false;
}


// Clears the event set, removing all of the events.
//
// Parameters:  None.
//
// Returns:
//      true if successful.
//      false if the event set was already empty.
//
bool BcmEventSet::RemoveAll(void)
{
    if (fEventList.empty())
    {
        return false;
    }

    // Clear the vector.
    fEventList.clear();

    // Let the derived class do its thing.
    return DerivedRemoveAll();
}


// This method causes the event set to call BcmEvent::Send() for each event
// in the set.  Note that some events are tied to other objects (like timers
// and message queues) and that sending these events may not make sense.  It
// is up to the client to make sure that the events that are in the set
// should be sent.
//
// Parameters:  None.
//
// Returns:
//      true if successful.
//      false if there was a problem.
//
bool BcmEventSet::Send(void)
{
    vector<BcmEvent *>::iterator myIterator;
    bool result;

    // Iterate through each item in the vector.
    for (myIterator = fEventList.begin(); myIterator < fEventList.end(); myIterator++)
    {
        // Send the event.
        result = (*myIterator)->Send();

        if (result == false)
        {
            gErrorMsg(fMessageLogSettings, "Send") << "Failed to send the message!" << endl;

            return false;
        }
    }

    return true;
}


// This method will be used by a client thread to see if the specified
// event has been triggered.  This is usually done immediately after the
// call to Wait() returns.
//
// By default, the event will be cleared after calling this method so that
// future calls will return false (unless the event is sent again).  If
// clearEvent is set to true, then the event will not be cleared, and
// future calls to Occurred() will return true.
//
// Parameters:
//      pEvent - the event to check for being set.
//      clearEvent - if true, resets the event; otherwise, leaves it set.
//
// Returns:
//      true if the event is set.
//      false if the event is not set.
//
bool BcmEventSet::Occurred(BcmEvent *pEvent, bool clearEvent, bool peekEvent)
{
    vector<BcmEvent *>::iterator myIterator;

    // Make sure it's a valid event.
    if (pEvent == NULL)
    {
        gErrorMsg(fMessageLogSettings, "Occurred") << "Event parameter is NULL!" << endl;

        return false;
    }

    // Only do this if the list isn't empty.
    if (!fEventList.empty())
    {
        // Find my event.
        myIterator = find(fEventList.begin(), fEventList.end(), pEvent);

        if (myIterator != fEventList.end())
        {                      
            // Do we need to pull this real-time or use the event that was pulled and stored
            // from the last Wait() call?
            if ( peekEvent == true )
            {
                return PeekOccurred(pEvent);
            }
            else
            {
                // See if it was triggered.
                return DerivedOccurred(pEvent, clearEvent);
            }
        }
    }

    // Didn't find it!
    gWarningMsg(fMessageLogSettings, "Occurred") << "Specified event isn't in the set!" << endl;
    
    return false;
}


void BcmEventSet::Print(void)
{
    vector<BcmEvent *>::iterator myIterator;
    unsigned long bitSet = 0;
    static bool oneTime = true;


    // Iterate through each item in the vector.
    for (myIterator = fEventList.begin(); myIterator < fEventList.end(); myIterator++)
    {
        if (oneTime == true)
        {
            gAlwaysMsg(fMessageLogSettings, "Print") << "Events Set: " <<  inHex( (*myIterator)->EventBit() ) << endl;
        }

        bitSet += (*myIterator)->EventBit();
    }

    gAlwaysMsg(fMessageLogSettings, "Print") << "Event Set Bitmask   : " <<  inHex( bitSet ) << endl;
    gAlwaysMsg(fMessageLogSettings, "Print") << "OS Event Set Bitmask: " <<  inHex( Derived() ) << endl;

    oneTime = false;
}
    
                                            

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
//****************************************************************************
//  $Id$
//
//  Filename:       MessageQueue.cpp
//  Author:         David Pullen
//  Creation Date:  Dec 30, 1999
//
//****************************************************************************
//  Description:
//
//      NOTE:  I'm undecided at this point as to whether or not I should use
//             templates for the message parameters, object pointers (derived
//             from an abstract BcmMessage base class), or just a void pointer.
//             They all have their drawbacks, so I may just go with the simplest
//             one (void pointer).
//
//      This is the abstract base class (and wrapper) for operating system
//      message queues.  These are used when one thread wants to send data to
//      another thread (as opposed to events and semaphores, which simply
//      indicate that something happened).  See the BcmMessageQueue scenario
//      diagram in OSWrapper.vsd for more information on how this class should
//      be used.
//
//      This message queue object is thread-safe, meaning that it provides
//      access and contention control against being modified by multiple threads
//      at the same time.  Other types of queues (such as STL and other home-brew
//      objects/structures) may be more efficient, but must provide their own
//      contention control mechanisms (mutexes).
//
//      Note that a message queue generally operates as a FIFO queue, though it
//      does provide a method to allow messages to be sent at the front of the
//      queue.
//
//      When using a message queue one thread will act as the producer, calling
//      Send(), and another will act as the consumer, calling Receive().  There
//      can be multiple producers for a single message queue, and if controlled
//      carefully, there can even be multiple consumers.  Note, however, that
//      if there are multiple consumers, then it may only be possible for one
//      of them to use the queue in the context of an event set.
//
//****************************************************************************

//********************** Include Files ***************************************

// My api and definitions...
#include "MessageQueue.h"

// Other objects of interest.
#include "Event.h"
#include "OperatingSystem.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

//********************** Local Types *****************************************

//********************** Local Constants *************************************

//********************** Local Variables *************************************

//********************** Class Method Implementations ************************


// Default Constructor.  Stores the name assigned to the object.
//
// Parameters:
//      pName - the text name that is to be given to the object, for
//              debugging purposes.
//
// Returns:  N/A
//
BcmMessageQueue::BcmMessageQueue(const char *pName) :
    fMessageLogSettings("BcmMessageQueue")
{
    // The derived class must create and store this.  I will delete it.
    pfEventToTrigger = NULL;

    // Call the helper method to create my object name.
    pfName = BcmOperatingSystem::GenerateName(pName, "MessageQueue");

    // Set my instance name.
    fMessageLogSettings.SetInstanceName(pfName);

    fMessageLogSettings.Register();
}


// Destructor.  Frees up any memory/objects allocated, cleans up internal
// state.
//
// Warning about possible memory leaks:
//
//      If the message queue has messages in it, then any memory associated
//      with that data will not be freed.  This is because the message queue
//      has no way to know whether the data is just a number, is a pointer
//      to a buffer, or is a pointer to an object (whose destructor needs)
//      to be called.
//
// Parameters:  N/A
//
// Returns:  N/A
//
BcmMessageQueue::~BcmMessageQueue()
{
    fMessageLogSettings.Deregister();

    // Assume that the derived class has removed all of the messages from the
    // queue and done other high-level cleanup.

    // Get rid of the event.
    delete pfEventToTrigger;
    pfEventToTrigger = NULL;

    // Delete the memory associated with the name.
    delete pfName;
    pfName = NULL;
}


// This methods allows the calling thread to wait for a message to be queued
// without forcing it to remove the message from the queue.
//
// Note that this method returns true immediately if there are messages
// already waiting in the queue.  It returns false if the timeout expired
// before a message was queued.
//
// Parameters:
//      mode - the wait mode (forever or timeout).
//      timeoutMs - the number of milliseconds to wait for a message to be
//                  sent; only used if mode is set to timeout.
//
// Returns:
//      true if a message was queued.
//      false if the timeout expired, or some other OS-specific problem
//          occurred.
//
bool BcmMessageQueue::Wait(WaitMode mode, unsigned long timeoutMs)
{
    if (pfEventToTrigger == NULL)
    {
        gErrorMsg(fMessageLogSettings, "Wait") << "My event is NULL!  Can't wait..." << endl;

        return false;
    }

    // Just vector to the event's method.
    return pfEventToTrigger->Wait((BcmEvent::WaitMode) mode, timeoutMs);
}



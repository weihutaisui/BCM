//****************************************************************************
//
//  Copyright (c) 1999, 2000  Broadcom Corporation
//  All Rights Reserved
//  No portions of this material may be reproduced in any form without the
//  written permission of:
//          Broadcom Corporation
//          16215 Alton Parkway
//          Irvine, California 92619
//  All information contained in this document is Broadcom Corporation
//  company private, proprietary, and trade secret.
//
//****************************************************************************
//  $Id: LnxMessageQueue.cpp 1.3 2006/08/02 19:50:32Z taskiran Release $
//
//  Filename:       QNXMessageQueue.cpp
//  Author:         David Pullen
//  Creation Date:  Jan 4, 2000
//
//****************************************************************************
//  Description:
//      This is the Linux implementation of the Message Queue wrapper class.
//		One last thing... I/ve not been able to test the time-out functionality
//		using the LnxEvent class yet. As soon as that class is finished I'll 
//		come back to this.  MDR 
//
//****************************************************************************

//********************** Include Files ***************************************

// My api and definitions...
#include "LnxMessageQueue.h"

// Other objects of interest.
#include "LnxEvent.h"
#include "LnxMutexSemaphore.h"

#include <assert.h>
#include <iostream>

//********************** Local Types *****************************************

//********************** Local Constants *************************************

//********************** Local Variables *************************************

//********************** Class Method Implementations ************************


// ----------------------------------------------------------------------------
// Default Constructor.  Creates a Linux message queue and sets up other
// needed members.
//
// Parameters:
//      pName - the text name that is to be given to the object, for
//              debugging purposes.
//
// Returns:  N/A
// ----------------------------------------------------------------------------
BcmLnxMessageQueue::BcmLnxMessageQueue(const char *pName) :
    BcmMessageQueue(pName)
{
    // Override the class name given by my parent.
    fMessageLogSettings.SetModuleName("BcmLnxMessageQueue");

    // Create the event that will be used.  This will be deleted by my base
    // class.
    pfEventToTrigger = new BcmLnxEvent("Message Queue's Event");

    if (pfEventToTrigger == NULL)
    {
        gFatalErrorMsg(fMessageLogSettings, "BcmLnxMessageQueue")
            << "Failed to create event!" << endl;

        assert(0);
    }

    // Create the mutex that will be used.
    pfMutex = new BcmLnxMutexSemaphore("Message Queue's Mutex");

    if (pfMutex == NULL)
    {
        gFatalErrorMsg(fMessageLogSettings, "BcmLnxMessageQueue")
            << "Failed to create mutex!" << endl;

        assert(0);
    }
}


// ----------------------------------------------------------------------------
// Destructor.  Frees up any memory/objects allocated, cleans up internal
// state.
//
// Warning about possible memory leaks:
//
//      If the message queue has messages in it, then any memory associated
//      with that data will not be freed.  This is because the message queue
//      has no way to know whether the data is just a number, is a pointer
//      to a buffer, or is a pointer to an object (whose destructor needs
//      to be called).
//
// Parameters:  N/A
//
// Returns:  N/A
// ----------------------------------------------------------------------------
BcmLnxMessageQueue::~BcmLnxMessageQueue()
{
    // Check to see if there are any messages still in the queue.  This will
    // cause a memory leak!
    if (NumberOfMessages() > 0)
    {
        // Log the error.
        gErrorMsg(fMessageLogSettings, "~BcmLnxMessageQueue")
            << "Message queue isn't empty!  Possible memory leak..." << endl;
    }
    
    // I only need to delete the mutex; the event is deleted by my base class.
    delete pfMutex;
    pfMutex = NULL;
}


// ----------------------------------------------------------------------------
// This is the method that a thread will call when it wants to send a
// message to the queue.  The message is appended to the end of the queue.
// The structure, format, and contents of the message are defined by the
// threads that use it.
//
// The first parameter is a message code that is agreed upon by the sender
// and receiver.  These will generally be abstracted as enums that the
// producers and consumers all have access to.
//
// The second parameter is a generic pointer who's type should be inferred
// from the message code.  It can be used to hold any numerical value, or
// it can be a pointer to an object or struct who's memory is being passed
// from one thread to another.  Of course, if pointers to memory or objects
// are being passed, then the producer and consumer must agree on whether
// or not it needs to be freed/destroyed, and which thread must do it.
//
// Parameters:
//      messageCode - the code that identifies the message being sent.
//      pMessage - a generic pointer to be used for sending data.  This can
//                 optionally be omitted if it is not necessary (e.g. if the
//                 messageCode alone conveys enough information).
//
// Returns:
//      true if successful.
//      false if there was a problem (OS-specific).  This generally means
//          that the message was not send, and the data needs to be dealt
//          with by the called (freeing memory, etc.).
// ----------------------------------------------------------------------------
bool BcmLnxMessageQueue::Send(unsigned int messageCode, void *pMessage)
{
    // Just vector to the common implementation method.
    return SendImpl(false, messageCode, pMessage);
}


// ----------------------------------------------------------------------------
// This method is similar to Send(), except that it prepends the message to
// the front of the queue, rather than appending it to the end.  All of the
// same parameters apply.
//
// Parameters:
//      messageCode - the code that identifies the message being sent.
//      pMessage - a generic pointer to be used for sending data.  This can
//                 optionally be omitted if it is not necessary (e.g. if the
//                 messageCode alone conveys enough information).
//
// Returns:
//      true if successful.
//      false if there was a problem (OS-specific).  This generally means
//          that the message was not send, and the data needs to be dealt
//          with by the called (freeing memory, etc.).
// ----------------------------------------------------------------------------
bool BcmLnxMessageQueue::SendUrgent(unsigned int messageCode, void *pMessage)
{
    // Just vector to the common implementation method.
    return SendImpl(true, messageCode, pMessage);
}


// ----------------------------------------------------------------------------
// This is the method that a thread will call when it wants to try to
// receive a message from the queue.  If a message is waiting, then it will
// read the one from the front of the queue and return immediately.
//
// If one is not available, then the thread can optionally return
// immediately (with an appropriate return code), or it can choose to wait
// for a message to be sent.  The wait can either time out after a specified
// number of milliseconds, or can wait indefinitely.
//
// By default, the timeout parameters are set such that the caller will wait
// forever for a message to be sent.
//
// Parameters:
//      messageCode - reference to a variable into which the message's
//                    message code should be placed.
//      pMessage - reference to a data pointer into which the data portion
//                 of the message should be placed.  This must be type-casted
//                 and interprited based on the messageCode.
//      mode - the wait mode (forever or timeout).
//      timeoutMs - the number of milliseconds to wait for a message to be
//                  sent; only used if mode is set to timeout.
//
// Returns:
//      true if the message was read.
//      false if the message wasn't read (timed out waiting, or some other
//          OS-specific problem occurred).
// ----------------------------------------------------------------------------
bool BcmLnxMessageQueue::Receive(unsigned int &messageCode, void *&pMessage,
                                   WaitMode mode, unsigned long timeoutMs)
{
    // Just vector to the common implementation method.
    return ReceiveImpl(true, messageCode, pMessage, mode, timeoutMs);
}


// ----------------------------------------------------------------------------
// This method is similar to Receive(), except that it does not remove the
// message from the queue.  Also, it does not allow the caller to wait for
// a message to be queued.
//
// Parameters:
//      messageCode - reference to a variable into which the message's
//                    message code should be placed.
//      pMessage - reference to a data pointer into which the data portion
//                 of the message should be placed.  This must be type-casted
//                 and interprited based on the messageCode.
//
// Returns:
//      true if the message was read.
//      false if there wasn't a message in the queue (or some other
//          OS-specific problem occurred).
// ----------------------------------------------------------------------------
bool BcmLnxMessageQueue::Peek(unsigned int &messageCode, void *&pMessage)
{
    // Just vector to the common implementation method.  Set the wait parameters
    // so that we return immediately regardless of whether or not there is a
    // message waiting.
    return ReceiveImpl(false, messageCode, pMessage, kTimeout, 0);
}


// ----------------------------------------------------------------------------
// This method returns the number of messages that are currently waiting in
// the message queue.
//
// Parameters:  None.
//
// Returns:
//      The number of messages in the queue.
// ----------------------------------------------------------------------------
unsigned int BcmLnxMessageQueue::NumberOfMessages(void) const
{
    // Call the STL method to query the number of items that are in the queue.
    return fQueue.size();
}


// ----------------------------------------------------------------------------
// This method returns the maximum number of messages that the message queue
// can hold.  Some operating systems place limits on the size of queues.
//
// Parameters:  None.
//
// Returns:
//      The maximum number of messages that can be queued.
// ----------------------------------------------------------------------------
unsigned int BcmLnxMessageQueue::MaximumMessages(void) const
{
    // Call the STL method to query the max number of items that can be stored
    // in the queue.
    return fQueue.max_size();
}


// ----------------------------------------------------------------------------
// This method contains the common implementation code for both of the
// Send methods; the difference between urgent and normal sending is minor
// enough that most of the code can be reused.
//
// Parameters:
//      isUrgent - tells whether this is being called from Send() or
//                 SendUrgent().
//      messageCode - the code that identifies the message being sent.
//      pMessage - a generic pointer to be used for sending data.  This can
//                 optionally be omitted if it is not necessary (e.g. if the
//                 messageCode alone conveys enough information).
//
// Returns:
//      true if successful.
//      false if there was a problem (OS-specific).  This generally means
//          that the message was not send, and the data needs to be dealt
//          with by the called (freeing memory, etc.).
// ----------------------------------------------------------------------------
bool BcmLnxMessageQueue::SendImpl(bool isUrgent, unsigned int messageCode,
                                    void *pMessage)
{
    bool result;
    QueueElement queueElement;

    // Set up the queue element that will be added to the queue.
    queueElement.messageCode = messageCode;
    queueElement.pMessage = pMessage;

    // Lock access to the queue.
    result = pfMutex->Lock();

    if (result == false)
    {
        gErrorMsg(fMessageLogSettings, "SendImpl")
             << "Failed to lock the mutex!" << endl;

        return false;
    }

    // Add the packet to the end or beginning of the queue, depending on the
    // urgent flag.
    if (isUrgent == true)
    {
        fQueue.push_front(queueElement);
    }
    else
    {
        fQueue.push_back(queueElement);
    }

    // Notify the event.
    result = pfEventToTrigger->Send();

    if (result == false)
    {
        // This is a non-fatal error.
        gErrorMsg(fMessageLogSettings, "SendImpl")
             << "Failed to send the event!" << endl;
    }

    // Unlock access to the queue.
    result = pfMutex->Unlock();

    if (result == false)
    {
        // This is a non-fatal error.
        gErrorMsg(fMessageLogSettings, "SendImpl")
             << "Failed to unlock the mutex!" << endl;
    }

    return true;
}


// ----------------------------------------------------------------------------
// This method contains the common implementation code for both Receive and
// Peek; the difference between them is minor enough that most of the code
// can be reused.
//
// Parameters:
//      dequeueMessage - tells whether or not to remove the message from the
//                       queue.
//      messageCode - reference to a variable into which the message's
//                    message code should be placed.
//      pMessage - reference to a data pointer into which the data portion
//                 of the message should be placed.  This must be type-casted
//                 and interprited based on the messageCode.
//      mode - the wait mode (forever or timeout).
//      timeoutMs - the number of milliseconds to wait for a message to be
//                  sent; only used if mode is set to timeout.
//
// Returns:
//      true if the message was read.
//      false if there wasn't a message in the queue (or some other
//          OS-specific problem occurred).
// ----------------------------------------------------------------------------
bool BcmLnxMessageQueue::ReceiveImpl(bool dequeueMessage, 
                                       unsigned int &messageCode,
                                       void *&pMessage, WaitMode mode,
                                       unsigned long timeoutMs)
{
    bool result;
    QueueElement queueElement;

    // Lock access to the queue.
    result = pfMutex->Lock();

    if (result == false)
    {
        gErrorMsg(fMessageLogSettings, "ReceiveImpl")
             << "Failed to lock the mutex!" << endl;

        return false;
    }

    // If the queue is empty, then I need to unlock access to the queue and
    // wait for the event to be posted.
    if (fQueue.empty())
    {
        // Clear the event in case it was previously posted.
        pfEventToTrigger->Clear();

        // Unlock the mutex.
        result = pfMutex->Unlock();

        if (result == false)
        {
            gErrorMsg(fMessageLogSettings, "ReceiveImpl")
                 << "Failed to unlock the mutex!" << endl;

            return false;
        }

        // Wait for the event to be posted.
        result = pfEventToTrigger->Wait((BcmEvent::WaitMode) mode, timeoutMs);

        // If the event wasn't triggered (we timed out waiting), then just bail.
        if (result == false)
        {
            gInfoMsg(fMessageLogSettings, "ReceiveImpl")
                << "Timed out waiting for a message." << endl;

            return false;
        }

        // Re-lock the mutex so that I can operate on the queue.
        result = pfMutex->Lock();

        if (result == false)
        {
            gErrorMsg(fMessageLogSettings, "ReceiveImpl")
                 << "Failed to lock the mutex!" << endl;

            return false;
        }
    }

    // Read the element from the front of the queue.
    queueElement = fQueue.front();

    // If we are not peeking at the queue, then remove the first element.
    if (dequeueMessage == true)
    {
        fQueue.pop_front();
    }

    // Unlock access to the queue.
    result = pfMutex->Unlock();

    if (result == false)
    {
        // This is a non-fatal error.
        gErrorMsg(fMessageLogSettings, "ReceiveImpl")
             << "Failed to unlock the mutex!" << endl;
    }

    // Set the return values.
    messageCode = queueElement.messageCode;
    pMessage = queueElement.pMessage;

    return true;
}

// END END END END END END END END END END END END END END END END END END END
// END END END END END END END END END END END END END END END END END END END
// END END END END END END END END END END END END END END END END END END END



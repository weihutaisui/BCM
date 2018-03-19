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
//  $Id: LnxMessageQueue.h 1.4 2006/08/02 19:50:33Z taskiran Release $
//
//  Filename:       LnxMessageQueue.h
//  Author:         Mark Rush
//  Creation Date:  Oct. 23, 2001
//
//****************************************************************************
//  Description:
//      This is the Linux implementation of the Message Queue wrapper class.
//
//		In the interests of time, I will implement this using the same method
//		as the vxWorks implementation.  We will go back later and see if the 
//		Linux Message Queue is appropriate.  One possible problem is that the
//		Linux Message Queue is designed for interprocess communication.
//      So I am implementing this with an STL deque, a mutex to protect
//      against access by multiple threads, and an event to allow the queue to
//      be waited on.
//		One last thing... I/ve not been able to test the time-out functionality
//		using the LnxEvent class yet.  MDR
//
//****************************************************************************

#ifndef LNXMESSAGEQUEUE_H
#define LNXMESSAGEQUEUE_H

//********************** Include Files ***************************************

///////MDR NECESSARY?????? in Linux?

// following line necessary because QNX defines a macro called Send           
#undef Send
// following line necessary because QNX defines a macro called Receive
#undef Receive
// My base class.
#include "MessageQueue.h"

// Used as the implementation of the queue.
#include <deque>

//********************** Global Types ****************************************

// This is what will be stored in the STL queue.  It corresponds to the API for
// Send().
typedef struct
{
    unsigned int messageCode;
    void *pMessage;
} QueueElement;

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Forward Declarations ********************************

class BcmMutexSemaphore;

//********************** Class Declaration ***********************************


class BcmLnxMessageQueue : public BcmMessageQueue
{
public:

    // Default Constructor.  Creates a QNX message queue and sets up other
    // needed members.
    //
    // Parameters:
    //      pName - the text name that is to be given to the object, for
    //              debugging purposes.
    //
    // Returns:  N/A
    //
    BcmLnxMessageQueue(const char *pName = NULL);

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
    //
    virtual ~BcmLnxMessageQueue();

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
    //
    bool Send(unsigned int messageCode, void *pMessage = NULL);

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
    //
    bool SendUrgent(unsigned int messageCode, void *pMessage = NULL);

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
    //
    bool Receive(unsigned int &messageCode, void *&pMessage,
                         WaitMode mode = kForever, unsigned long timeoutMs = 0);

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
    //
    bool Peek(unsigned int &messageCode, void *&pMessage);

    // This method returns the number of messages that are currently waiting in
    // the message queue.
    //
    // Parameters:  None.
    //
    // Returns:
    //      The number of messages in the queue.
    //
    unsigned int NumberOfMessages(void) const;

    // This method returns the maximum number of messages that the message queue
    // can hold.  Some operating systems place limits on the size of queues.
    //
    // Parameters:  None.
    //
    // Returns:
    //      The maximum number of messages that can be queued.
    //
    unsigned int MaximumMessages(void) const;

protected:

private:

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
    //
    bool SendImpl(bool isUrgent, unsigned int messageCode, void *pMessage);

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
    //
    bool ReceiveImpl(bool dequeueMessage, unsigned int &messageCode, 
                     void *&pMessage, WaitMode mode, unsigned long timeoutMs);

private:

    // The queue.
    deque<QueueElement> fQueue;

    // Locks access to the queue.
    BcmMutexSemaphore *pfMutex;

private:

    // Copy Constructor.  Not supported.
    //
    // Parameters:
    //      otherInstance - reference to the object to copy.
    //
    // Returns:  N/A
    //
    BcmLnxMessageQueue(BcmLnxMessageQueue &otherInstance);

    // Assignment operator.  Not supported.
    //
    // Parameters:
    //      otherInstance - reference to the object to copy.
    //
    // Returns:
    //      A reference to "this" so that operator = can be chained.
    //
    BcmLnxMessageQueue & operator = (BcmLnxMessageQueue &otherInstance);

};


//********************** Inline Method Implementations ***********************

#endif



//****************************************************************************
//
//  Copyright (c) 2000-2003  Broadcom Corporation
//  All Rights Reserved
//  No portions of this material may be reproduced in any form without the
//  written permission of:
//          Broadcom Corporation
//          16215 Alton Parkway
//          Irvine, California 92618
//  All information contained in this document is Broadcom Corporation
//  company private, proprietary, and trade secret.
//
//****************************************************************************
//  $Id$
//
//  Filename:       PortableSwMessageQueue.h
//  Author:         David Pullen
//  Creation Date:  May 28, 2003
//
//****************************************************************************

#ifndef PortableSwMessageQueue_H
#define PortableSwMessageQueue_H

//********************** Include Files ***************************************

// My base class.
#include "MessageQueue.h"

// Used as the implementation of the queue.
#include "deque.h"

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


/** \ingroup OsWrappers
*
*   This is a portable, OS-agnostic, software-based implementation of the
*   Message Queue wrapper class.  This used to be the Win32 implementation,
*   but we found that there are more operating systems that don't provide
*   the message queue support that we need, and there really isn't much
*   about this class that is specific to Win32 (except creation of the
*   mutex and event object).
*
*   In order to improve re-use across multiple operating systems, I
*   refactored the Win32 Message Queue code down to a common base class
*   and left the Win32-specific stuff in the Win32 class.
*
*   This implementation uses an STL deque to implement storage for the
*   message code and data, with a mutex to protect access to the queue.
*   As with other OS-specific implementations, an event is used to signal
*   that a message has been queued.
*/
class BcmPortableSwMessageQueue : public BcmMessageQueue
{
public:

    /// Default Constructor.  Creates a message queue and sets up other
    /// needed members.
    ///
    /// \param
    ///      pEvent - pointer to the event that should be posted when a message
    ///               is sent in the queue.  This is OS-specific, so it must be
    ///               passed to me by the derived class.
    /// \param
    ///      pName - the text name that is to be given to the object, for
    ///              debugging purposes.
    ///
    BcmPortableSwMessageQueue(BcmEvent *pEvent, const char *pName = NULL);

    /// Destructor.  Frees up any memory/objects allocated, cleans up internal
    /// state.
    ///
    /// Warning about possible memory leaks:
    ///
    ///      If the message queue has messages in it, then any memory associated
    ///      with that data will not be freed.  This is because the message queue
    ///      has no way to know whether the data is just a number, is a pointer
    ///      to a buffer, or is a pointer to an object (whose destructor needs
    ///      to be called).
    ///
    virtual ~BcmPortableSwMessageQueue();

    /// This is the method that a thread will call when it wants to send a
    /// message to the queue.  The message is appended to the end of the queue.
    /// The structure, format, and contents of the message are defined by the
    /// threads that use it.
    ///
    /// The first parameter is a message code that is agreed upon by the sender
    /// and receiver.  These will generally be abstracted as enums that the
    /// producers and consumers all have access to.
    ///
    /// The second parameter is a generic pointer who's type should be inferred
    /// from the message code.  It can be used to hold any numerical value, or
    /// it can be a pointer to an object or struct who's memory is being passed
    /// from one thread to another.  Of course, if pointers to memory or objects
    /// are being passed, then the producer and consumer must agree on whether
    /// or not it needs to be freed/destroyed, and which thread must do it.
    ///
    /// \param
    ///      messageCode - the code that identifies the message being sent.
    /// \param
    ///      pMessage - a generic pointer to be used for sending data.  This can
    ///                 optionally be omitted if it is not necessary (e.g. if the
    ///                 messageCode alone conveys enough information).
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem (OS-specific).  This generally means
    ///          that the message was not send, and the data needs to be dealt
    ///          with by the called (freeing memory, etc.).
    ///
    virtual bool Send(unsigned int messageCode, void *pMessage = NULL);

    /// This method is similar to Send(), except that it prepends the message to
    /// the front of the queue, rather than appending it to the end.  All of the
    /// same parameters apply.
    ///
    /// \param
    ///      messageCode - the code that identifies the message being sent.
    /// \param
    ///      pMessage - a generic pointer to be used for sending data.  This can
    ///                 optionally be omitted if it is not necessary (e.g. if the
    ///                 messageCode alone conveys enough information).
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem (OS-specific).  This generally means
    ///          that the message was not send, and the data needs to be dealt
    ///          with by the called (freeing memory, etc.).
    ///
    virtual bool SendUrgent(unsigned int messageCode, void *pMessage = NULL);

    /// This is the method that a thread will call when it wants to try to
    /// receive a message from the queue.  If a message is waiting, then it will
    /// read the one from the front of the queue and return immediately.
    ///
    /// If one is not available, then the thread can optionally return
    /// immediately (with an appropriate return code), or it can choose to wait
    /// for a message to be sent.  The wait can either time out after a specified
    /// number of milliseconds, or can wait indefinitely.
    ///
    /// By default, the timeout parameters are set such that the caller will wait
    /// forever for a message to be sent.
    ///
    /// \param
    ///      messageCode - reference to a variable into which the message's
    ///                    message code should be placed.
    /// \param
    ///      pMessage - reference to a data pointer into which the data portion
    ///                 of the message should be placed.  This must be type-casted
    ///                 and interprited based on the messageCode.
    /// \param
    ///      mode - the wait mode (forever or timeout).
    /// \param
    ///      timeoutMs - the number of milliseconds to wait for a message to be
    ///                  sent; only used if mode is set to timeout.
    ///
    /// \retval
    ///      true if the message was read.
    /// \retval
    ///      false if the message wasn't read (timed out waiting, or some other
    ///          OS-specific problem occurred).
    ///
    virtual bool Receive(unsigned int &messageCode, void *&pMessage,
                         WaitMode mode = kForever, unsigned long timeoutMs = 0);

    /// This method is similar to Receive(), except that it does not remove the
    /// message from the queue.  Also, it does not allow the caller to wait for
    /// a message to be queued.
    ///
    /// \param
    ///      messageCode - reference to a variable into which the message's
    ///                    message code should be placed.
    /// \param
    ///      pMessage - reference to a data pointer into which the data portion
    ///                 of the message should be placed.  This must be type-casted
    ///                 and interprited based on the messageCode.
    ///
    /// \retval
    ///      true if the message was read.
    /// \retval
    ///      false if there wasn't a message in the queue (or some other
    ///          OS-specific problem occurred).
    ///
    virtual bool Peek(unsigned int &messageCode, void *&pMessage);

    /// This method returns the number of messages that are currently waiting in
    /// the message queue.
    ///
    /// \return
    ///      The number of messages in the queue.
    ///
    virtual unsigned int NumberOfMessages(void) const;

    /// This method returns the maximum number of messages that the message queue
    /// can hold.  Some operating systems place limits on the size of queues.
    ///
    /// \return
    ///      The maximum number of messages that can be queued.
    ///
    virtual unsigned int MaximumMessages(void) const;

protected:

private:

    /// This method contains the common implementation code for both of the
    /// Send methods; the difference between urgent and normal sending is minor
    /// enough that most of the code can be reused.
    ///
    /// \param
    ///      isUrgent - tells whether this is being called from Send() or
    ///                 SendUrgent().
    /// \param
    ///      messageCode - the code that identifies the message being sent.
    /// \param
    ///      pMessage - a generic pointer to be used for sending data.  This can
    ///                 optionally be omitted if it is not necessary (e.g. if the
    ///                 messageCode alone conveys enough information).
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem (OS-specific).  This generally means
    ///          that the message was not send, and the data needs to be dealt
    ///          with by the called (freeing memory, etc.).
    ///
    bool SendImpl(bool isUrgent, unsigned int messageCode, void *pMessage);

    /// This method contains the common implementation code for both Receive and
    /// Peek; the difference between them is minor enough that most of the code
    /// can be reused.
    ///
    /// \param
    ///      dequeueMessage - tells whether or not to remove the message from the
    ///                       queue.
    /// \param
    ///      messageCode - reference to a variable into which the message's
    ///                    message code should be placed.
    /// \param
    ///      pMessage - reference to a data pointer into which the data portion
    ///                 of the message should be placed.  This must be type-casted
    ///                 and interprited based on the messageCode.
    /// \param
    ///      mode - the wait mode (forever or timeout).
    /// \param
    ///      timeoutMs - the number of milliseconds to wait for a message to be
    ///                  sent; only used if mode is set to timeout.
    ///
    /// \retval
    ///      true if the message was read.
    /// \retval
    ///      false if there wasn't a message in the queue (or some other
    ///          OS-specific problem occurred).
    ///
    bool ReceiveImpl(bool dequeueMessage, unsigned int &messageCode, 
                     void *&pMessage, WaitMode mode, unsigned long timeoutMs);

private:

    /// The queue.
    deque<QueueElement> fQueue;

    /// Locks access to the queue.
    BcmMutexSemaphore *pfMutex;

private:

    /// Default Constructor.  Not supported.
    BcmPortableSwMessageQueue(void);

    /// Copy Constructor.  Not supported.
    BcmPortableSwMessageQueue(BcmPortableSwMessageQueue &otherInstance);

    /// Assignment operator.  Not supported.
    BcmPortableSwMessageQueue & operator = (BcmPortableSwMessageQueue &otherInstance);

};


//********************** Inline Method Implementations ***********************

#endif



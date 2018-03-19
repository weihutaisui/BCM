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
//  Filename:       MessageQueue.h
//  Author:         David Pullen
//  Creation Date:  Feb 17, 1999
//
//****************************************************************************

#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

//********************** Include Files ***************************************

// For bool and other portable types.
#include "typedefs.h"

#include "MessageLog.h"

//********************** Global Types ****************************************

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Forward Declarations ********************************

class BcmEvent;

//********************** Class Declaration ***********************************


/** \ingroup OsWrappers
*
*   This is the abstract base class (and wrapper) for operating system
*   message queues.
*
*   A message queue is an OS component that allows one or more threads to
*   send messages to another thread.  The messages contain information not
*   available from a BcmEvent or a BcmCountingSemaphore; they both indicate
*   that something happened (the semaphore also indicates the number of
*   times it happened), but they don't indicate what happens, and can't
*   carry any additional data.  A message consists of a message code, which
*   is generally one of a set of enum values that indicates the event or
*   command, and an optional data element, who's type should be inferred
*   from the message code.  The sender and receiver threads must agree on
*   the message codes and the format of the message data.
*
*   This message queue implementation is thread-safe, meaning that it
*   provides access and contention control against being modified by
*   multiple threads at the same time.  Other types of queues (such as STL
*   and other home-brew objects/structures) may be more efficient, but must
*   provide their own contention control mechanisms (mutexes).
*
*   Note that a message queue generally operates as a FIFO queue, though it
*   does provide a method to allow messages to be sent at the front of the
*   queue.
*
*   Message queues provide methods to allow a "producer" thread to send a
*   message (placing it either at the back or front of the queue); they
*   also allow the "consumer" thread, which created the queue, to wait for
*   a message to be sent, and to read the message, optionally leaving it in
*   the queue.
*
*   There can be multiple producers for a single message queue, and if
*   controlled carefully, there can even be multiple consumers.  Note,
*   however, that if there are multiple consumers, then it may only be
*   possible for one of them to use the queue in the context of a
*   BcmEventSet.
*
*   Because message queues have a BcmEvent associated with them, they can
*   be added to a BcmEventSet.
*/
class BcmMessageQueue
{
public:

    /// Default Constructor.  Stores the name assigned to the object.
    ///
    /// \param
    ///      pName - the text name that is to be given to the object, for
    ///              debugging purposes.
    ///
    BcmMessageQueue(const char *pName = NULL);

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
    virtual ~BcmMessageQueue();

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
    /// This is a pure virtual method; derived classes must override this to
    /// provide the OS-specific implementation.
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
    virtual bool Send(unsigned int messageCode, void *pMessage = NULL) = 0;

    /// This method is similar to Send(), except that it prepends the message to
    /// the front of the queue, rather than appending it to the end.  All of the
    /// same parameters apply.
    ///
    /// This is a pure virtual method; derived classes must override this to
    /// provide the OS-specific implementation.
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
    virtual bool SendUrgent(unsigned int messageCode, void *pMessage = NULL) = 0;

    /// Values that can be used in the call to Receive() and Wait().
    typedef enum
    {
        kTimeout,
        kForever
    } WaitMode;

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
    /// This is a pure virtual method; derived classes must override this to
    /// provide the OS-specific implementation.
    ///
    /// \note  When using this method, make sure the second parameter is
    ///        declared as a 'void *'; failure to do so will cause the compiler
    ///        to generate unexpected code, and the value returned will often
    ///        be 0 instead of the value that was actually associated with the
    ///        message.
    ///
    /// \code
    /// unsigned int messageCode;
    /// void *pMessage = NULL;
    ///
    /// pMessageQueue->Receive(messageCode, pMessage);
    ///
    /// if (messageCode == kProcessOctetBuffer)
    /// {
    ///     BcmOctetBuffer *pObuf = (BcmOctetBuffer *) pMessage;
    ///
    ///     // Do something with pObuf.
    /// }
    /// \endcode
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
                         WaitMode mode = kForever, unsigned long timeoutMs = 0) = 0;

    /// This method is similar to Receive(), except that it does not remove the
    /// message from the queue.  Also, it does not allow the caller to wait for
    /// a message to be queued.
    ///
    /// This is a pure virtual method; derived classes must override this to
    /// provide the OS-specific implementation.
    ///
    /// \note  When using this method, make sure the second parameter is
    ///        declared as a 'void *'; failure to do so will cause the compiler
    ///        to generate unexpected code, and the value returned will often
    ///        be 0 instead of the value that was actually associated with the
    ///        message.
    ///
    /// \code
    /// unsigned int messageCode;
    /// void *pMessage = NULL;
    ///
    /// pMessageQueue->Peek(messageCode, pMessage);
    ///
    /// if (messageCode == kProcessOctetBuffer)
    /// {
    ///     BcmOctetBuffer *pObuf = (BcmOctetBuffer *) pMessage;
    ///
    ///     // Do something with pObuf.
    /// }
    /// \endcode
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
    virtual bool Peek(unsigned int &messageCode, void *&pMessage) = 0;

    /// This methods allows the calling thread to wait for a message to be queued
    /// without forcing it to remove the message from the queue.
    ///
    /// Note that this method returns true immediately if there are messages
    /// already waiting in the queue.  It returns false if the timeout expired
    /// before a message was queued.
    ///
    /// \param
    ///      mode - the wait mode (forever or timeout).
    /// \param
    ///      timeoutMs - the number of milliseconds to wait for a message to be
    ///                  sent; only used if mode is set to timeout.
    ///
    /// \retval
    ///      true if a message was queued.
    /// \retval
    ///      false if the timeout expired, or some other OS-specific problem
    ///          occurred.
    ///
    virtual bool Wait(WaitMode mode = kForever, unsigned long timeoutMs = 0);

    /// This method returns the number of messages that are currently waiting in
    /// the message queue.
    ///
    /// This is a pure virtual method; derived classes must override this to
    /// provide the OS-specific implementation.
    ///
    /// \return
    ///      The number of messages in the queue.
    ///
    virtual unsigned int NumberOfMessages(void) const = 0;

    /// This method returns the maximum number of messages that the message queue
    /// can hold.  Some operating systems place limits on the size of queues.
    ///
    /// This is a pure virtual method; derived classes must override this to
    /// provide the OS-specific implementation.
    ///
    /// \return
    ///      The maximum number of messages that can be queued.
    ///
    virtual unsigned int MaximumMessages(void) const = 0;

    /// Simple accessor for the name of this object.
    ///
    /// \return
    ///      The pointer to the name string.
    ///
    inline const char *Name(void) const;

    /// Returns the event that is triggered when a message is sent.  This is
    /// primarily used by BcmEventSet when the message queue is added to the list
    /// of events that are to be waited on.
    ///
    /// \return
    ///      The message queue's event.
    ///
    inline BcmEvent *GetEvent(void) const;

    /// An alternate way to get the pointer to the event.  You can use it this
    /// way:
    ///
    /// \code
    ///   BcmMessageQueue *pMsgQueue;
    ///   BcmEvent *pEvent;
    ///
    ///   pEvent = (*pMsgQueue);
    /// \endcode
    ///
    /// \return
    ///      The event associated with the message queue.
    ///
    inline operator BcmEvent * () const;

protected:

    /// The event associated with the message queue.
    BcmEvent *pfEventToTrigger;

    /// My assigned name.
    char *pfName;

    /// Controls message logging.
    BcmMessageLogSettings fMessageLogSettings;

private:

    /// Copy Constructor.  Not supported.
    BcmMessageQueue(BcmMessageQueue &otherInstance);

    /// Assignment operator.  Not supported.
    BcmMessageQueue & operator = (BcmMessageQueue &otherInstance);

};


//********************** Inline Method Implementations ***********************


/// Simple accessor for the name of this object.
///
/// \return
///      The pointer to the name string.
///
inline const char *BcmMessageQueue::Name(void) const
{
    return pfName;
}


/// Returns the event that is triggered when a message is sent.  This is
/// primarily used by BcmEventSet when the message queue is added to the list
/// of events that are to be waited on.
///
/// \return
///      The message queue's event.
///
inline BcmEvent *BcmMessageQueue::GetEvent(void) const
{
    return pfEventToTrigger;
}


/// An alternate way to get the pointer to the event.  You can use it this
/// way:
///
/// \code
///   BcmMessageQueue *pMsgQueue;
///   BcmEvent *pEvent;
///
///   pEvent = (*pMsgQueue);
/// \endcode
///
/// \return
///      The event associated with the message queue.
///
inline BcmMessageQueue::operator BcmEvent * () const
{
    return pfEventToTrigger; 
}


#endif



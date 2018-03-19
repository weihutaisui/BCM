//****************************************************************************
//
//  $Id: LnxMutexEvent.h 1.4 2006/08/02 19:50:37Z taskiran Release $
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
//****************************************************************************

#ifndef LNXMUTEXEVENT_H
#define LNXMUTEXEVENT_H

//********************** Include Files ***************************************

// My base classes...
#include "Event.h"
class BcmLinuxTimeout;
#include <signal.h>
#include <pthread.h>

//********************** Global Types ****************************************

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Class Declaration ***********************************

class LnxMutexEvent
{
public:

    // Initializing/default Constructor.  
    // Parameters:
    //      pName - the text name that is to be given to the object, for
    //              debugging purposes.
    // Returns:  N/A
    //
    LnxMutexEvent(const char *pName = NULL);

    // Destructor.  Destroys the QNX event by closing the handle.
    //
    // Parameters:  N/A
    //
    // Returns:  N/A
    //
    ~LnxMutexEvent();

    // This is the method that a thread will call when it wants to "post" the
    // event, telling the waiting thread that the event has occurred.  Note that
    // if the event has been posted and you send the event again, both of these
    // are collapsed down to a single event.  It does not count the number of
    // times an event has been posted.
    //
    // This is the Linux-specific implementation for sending an event.
    //
    // Parameters:  None.
    //
    // Returns:
    //      true if successful.
    //      false if there was a problem.
    //
    bool Send(void);
    // Values that can be used in the call to Wait().
    typedef enum
    {
        kTimeout,
        kForever
    } WaitMode;

    // This is the method that a thread will call when it wants to wait for
    // an event to be sent.  Optionally, the thread can wait forever for the
    // event, timeout if the event isn't sent within a certain number of
    // milliseconds, or return immediately regardless of whether or not the
    // event has been sent.
    //
    // By default, if you call this method with no parameters, then you will
    // wait forever for the event.
    //
    // This is the Linux-specific implementation for waiting for an event.
    //
    // Parameters:
    //      mode - the wait mode (forever or timeout).
    //      timeoutMS - the number of milliseconds to wait for the event to be
    //                  sent; only used if mode is set to timeout.
    //
    // Returns:
    //      if EventOperation 
    //          true if the event occurred.
    //          false if the event didn't occur (timed out waiting).
    //      else TimerOperation
    //          true if timeout occurred.
    //          false if some kind of error
    //
    bool Wait(WaitMode mode = kForever, unsigned long timeoutMS = 0);

    //  Called by master clock when a timeout has occurred - that is, a thread has 
    //  been waiting for an Event to happen, and it didn't
    void Timeout (void);

    // don't mess with this, you nosy swine!
    static sigset_t fSigmask;

private:

    BcmLinuxTimeout * fMyTimeout;

    // handler for the signal that I use to indicate Events
    static void EventSigHandler (int signum);

    enum
    {
        les_idle,
        les_waiting,
        les_occurred,
        les_timedout
    }
    fState;             // the essence of my event
	typedef int pid_t;
    pid_t fOwnerProcessId;    // my owner's process id
    pthread_t fOwnerThreadId;    // my owner's process id
    #define BcmEventSigNum SIGUSR2
    unsigned long fTimeoutId;

    static bool fSigMaskInitted;

};


//********************** Inline Method Implementations ***********************


#endif



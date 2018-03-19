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
//  Filename:       Timer.cpp
//  Author:         David Pullen
//  Creation Date:  Feb 18, 1999
//
//****************************************************************************
//  Description:
//      This is the abstract base class for operating system timers.  A timer
//      causes the OS to send an event (BcmEvent) to the thread after the
//      specified timeout interval has expired; thus BcmTimer has-a BcmEvent.
//
//      NOTE:  For some operating systems, a timer must be started by the thread
//             that expects to be notified.  Thus, when a thread creates a
//             timer, it must ensure that the timer will not be used by any
//             other thread.
//
//****************************************************************************

//********************** Include Files ***************************************

// My api and definitions...
#include "Timer.h"

// Other objects of interest.
#include "Event.h"
#include "OperatingSystem.h"

//********************** Local Types *****************************************

//********************** Local Constants *************************************

//********************** Local Variables *************************************

//********************** Class Method Implementations ************************


// Default Constructor.  Sets everything to a quiescent state; note that
// the event to trigger is not created here - it must be created by the
// derived class.
//
// Parameters:
//      pName - the text name that is to be given to the object, for
//              debugging purposes.
//
// Returns:  N/A
//
BcmTimer::BcmTimer(const char *pName) :
    fMessageLogSettings("BcmTimer")
{
    // Call the helper method to create my object name.
    pfName = BcmOperatingSystem::GenerateName(pName, "Timer");
    
    // Set my instance name.
    fMessageLogSettings.SetInstanceName(pfName);

    fMessageLogSettings.Register();

    // Defer creation of the event to the derived class - they know what type
    // of event to create (and how to create it).
    pfEventToTrigger = NULL;

    // Set the rest of these to good default values.
    fHasBeenStarted = false;
    fIsRunning = false;
    fLastTimeoutMS = 0;
    fLastTimerMode = kOnce;
}


// Destructor.  Frees up any memory/objects allocated, cleans up internal
// state.  The most-derived class must call Stop() in order to ensure that
// the timer isn't running.
//
// pfEventToTrigger is deleted here.
//
// Parameters:  N/A
//
// Returns:  N/A
//
BcmTimer::~BcmTimer()
{
    fMessageLogSettings.Deregister();

    // Assume that the timer is stopped by now.  Delete the event object.
    delete pfEventToTrigger;
    pfEventToTrigger = NULL;

    // Delete the memory associated with the name.
    delete pfName;
    pfName = NULL;
}


// Stops the timer and restarts it with the timeout value and mode that
// were sent in on the last call to Start().  If Start() has not been
// called, then the timer is not restarted.
//
// This is useful because it eliminates extra calls to the object, and you
// don't have to know what the previous values were in order to restart it.
//
// Parameters:  None.
//
// Returns:
//      true if the timer was restarted successfully.
//      false if the timer was not restarted (Start() has not bee called
//          previously, or there was a problem stopping and/or starting it).
//
bool BcmTimer::Restart(void)
{
    // See if Start() has been called; if not, then we can't do this.
    if (!fHasBeenStarted)
    {
        gInfoMsg(fMessageLogSettings, "Restart") << "Timer not started.  Can't restart..." << endl;

        return false;
    }

    // Start the timer with the previous values.  I don't need to stop it 
    // before calling Start (the derived class will do so if necessary).
    return Start(fLastTimeoutMS, fLastTimerMode);
}


// Causes the calling thread to be blocked until the timer expires.  If the
// timer was not started, then it returns false immediately.
//
// Parameters:  None.
//
// Returns:
//      true if the timer expired.
//      false if the timer was not started.
//
bool BcmTimer::Wait(void)
{
    if (!fIsRunning)
    {
        gInfoMsg(fMessageLogSettings, "Wait") << "Timer not running.  Can't wait for it..." << endl;

        return false;
    }

    // Wait forever for the event to be triggered.  We don't need to have the
    // event time out on the wait, because the timer will generate the timeout.
    return pfEventToTrigger->Wait();
}




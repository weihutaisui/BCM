//****************************************************************************
//
//  $Id: LnxTimer.cpp 1.4 2006/08/02 19:50:49Z taskiran Release $
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
//****************************************************************************

//********************** Include Files ***************************************

#include "LnxTimer.h"
#include "LnxEvent.h"
#include "LnxTimeout.h"

//********************** Local Types *****************************************

//********************** Local Constants *************************************

//********************** Local Variables *************************************

//********************** Class Method Implementations ************************


// ----------------------------------------------------------------------------
// Default Constructor.  Creates the event that is to be triggered.
//
// Parameters:
//      pName - the text name that is to be given to the object, for
//              debugging purposes.
//
// Returns:  N/A
// ----------------------------------------------------------------------------
BcmLnxTimer::BcmLnxTimer(const char *pName) :
    BcmTimer(pName)
{
    // Override the class name given by my parent.
    fMessageLogSettings.SetModuleName("BcmLnxTimer");

    pfEventToTrigger  = new BcmLnxEvent("Timer's Event");    
    fMyTimeout = BcmLinuxTimeout::GetMyTimeout();
    fTimeoutId = 0;
    fThreadId = pthread_self();
}


// ----------------------------------------------------------------------------
// Destructor.  Stops the timer if it was running.
//
// Parameters:  N/A
//
// Returns:  N/A
// ----------------------------------------------------------------------------
BcmLnxTimer::~BcmLnxTimer()
{
    Stop();
}


// ----------------------------------------------------------------------------
// Starts the timer running (restarting the timer if it is already running).
// When the timer expires, the OS will send the specified event to you.
//
// The timer will automatically restart if the kRepeat flag is set; 
// otherwise, it just sits idle once it expires.
//
// Parameters:
//      timeoutMS - the number of milliseconds that the timer should count
//                  before expiring.
//      mode - set this to kRepeat to get the timer to automatically restart
//             after it expires.
//
// Returns:
//      true if the timer was started successfully.
//      false if there was a problem starting the timer.
//
// ----------------------------------------------------------------------------
bool BcmLnxTimer::Start(unsigned long timeoutMS, TimerMode mode)
{

    unsigned long threadId;

//    printf("BcmLnxTimer::Start %d ms this=%x \n", timeoutMS, this);

    threadId = pthread_self();
    if (fThreadId != threadId)
    {
        gErrorMsg(fMessageLogSettings, "Start")
            << "Calling thread does not own the timer! "
            << "One thread can't start another thread's timers!" 
            << endl;

        return false;
    }
    fHasBeenStarted = true;
    fLastTimeoutMS = timeoutMS;
    fLastTimerMode = mode;

    Stop();     // make sure its not going right now

    fMyTimeout = BcmLinuxTimeout::GetMyTimeout();
    fTimeoutId = fMyTimeout->AddTimeout (timeoutMS, BcmLinuxTimeout::ltc_timer, this);
    fIsRunning = true;

    return true;
}



// ----------------------------------------------------------------------------
// Called by the LnxTimeout facility when the timer has expired
// ----------------------------------------------------------------------------
void BcmLnxTimer::Timeout(void)
{
//    printf("BcmLnxTimer::Timeout this=%x \n", this);
    
    fIsRunning = false;
    fTimeoutId = 0;
    pfEventToTrigger->Send();
    if (fLastTimerMode == kRepeat)
        Restart();
}



// ----------------------------------------------------------------------------
// Stops a timer that is running (preventing it from sending the event).
// There may be a race condition where the timer expires (handled by the OS
// at interrupt time) as this function is executing, which will cause the
// event to be triggered.
//
// Parameters:  None.
//
// Returns:
//      true if the timer was successfully stopped.
//      false if there was a problem.
// ----------------------------------------------------------------------------
bool BcmLnxTimer::Stop(void)
{
    unsigned long threadId;

//    printf("BcmLnxTimer::Stop  \n");

    threadId = pthread_self();
    if (fThreadId != threadId)
    {
        gErrorMsg(fMessageLogSettings, "Stop")
            << "Calling thread does not own the timer!"
            << "One thread can't stop another thread's timers!" 
            << endl;

        return false;
    }
    
    bool success = true;     // assume commendable success
    if (fIsRunning)
    {
        fMyTimeout = BcmLinuxTimeout::GetMyTimeout();
        success = fMyTimeout->CancelTimeout (fTimeoutId);
        fTimeoutId = 0;
        fIsRunning = false;
    }

    return success;
}


// ============================================================================
// ======== END ========== END ========== END ========== END ========== END ===
// ============================================================================



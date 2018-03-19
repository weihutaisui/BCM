//****************************************************************************
//
//  Copyright (c) 2000  Broadcom Corporation
//  All Rights Reserved
//  No portions of this material may be reproduced in any form without the
//  written permission of:
//          Broadcom Corporation
//          16251 Laguna Canyon Road
//          Irvine, California 92618
//  All information contained in this document is Broadcom Corporation
//  company private, proprietary, and trade secret.
//****************************************************************************

#ifndef LNXTIMER_H
#define LNXTIMER_H

//********************** Include Files ***************************************

// My base class...
#include "Timer.h"

class BcmLinuxTimeout;

//********************** Global Types ****************************************

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Class Declaration ***********************************


class BcmLnxTimer : public BcmTimer
{
public:

    // Default Constructor.  Creates the event that is to be triggered.
    //
    // Parameters:
    //      pName - the text name that is to be given to the object, for
    //              debugging purposes.
    //
    // Returns:  N/A
    //
    BcmLnxTimer(const char *pName = NULL);

    // Destructor.  Stops the timer if it was running.
    //
    // Parameters:  N/A
    //
    // Returns:  N/A
    //
    ~BcmLnxTimer();

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
    bool Start(unsigned long timeoutMS, TimerMode mode = kOnce);

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
    //
    bool Stop(void);

    // Called by the LnxTimeout facility when the timer has expired
    void Timeout(void);

    /// Accessor to know if the existing timer is currently active or not.
    ///
    /// \retval
    ///      true if the timer is currently active
    /// \retval
    ///      false if the timer is not currently active
    virtual inline bool IsActive(void);

private:

    BcmLinuxTimeout * fMyTimeout;
    ulong fTimeoutId;
    ulong fThreadId;
};


//********************** Inline Method Implementations ***********************

/// Accessor to know if the existing timer is currently active or not.
///
/// \retval
///      true if the timer is currently active
/// \retval
///      false if the timer is not currently active
inline bool BcmLnxTimer::IsActive()
{ 
    return (fIsRunning); 
}

#endif



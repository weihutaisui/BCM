//****************************************************************************
//
//  Copyright (c) 2004  Broadcom Corporation
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
//  Filename:       NonVolSettingsAccessLock.h
//  Author:         David Pullen
//  Creation Date:  March 23, 2004
//
//****************************************************************************

#ifndef NonVolSettingsAccessLock_H
#define NonVolSettingsAccessLock_H

//********************** Include Files ***************************************

//********************** Global Types ****************************************

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Forward Declarations ********************************

//********************** Class Declaration ***********************************


/** \ingroup BfcNonVol
*
*   This class implements a locking mechanism to control access to various
*   Non-Volatile settings objects.  It is possible for multiple threads to
*   try and read/modify/write the settings at the same time, sometimes in
*   conflicting ways.
*
*   Rather than adding a mutex lock/unlock within each accessor method of all
*   the settings objects (which would bloat the code size significantly, and
*   would take forever), we have this external lock object that can (and
*   should) be used by clients when they do something significant to nonvol
*   settings.
*
*   To handle the locking, there is a single mutex that is created during
*   system startup.  This mutex is locked in the constructor of this class,
*   and is unlocked in the destructor.  To use it, just create an instance
*   of this class on the stack; when you leave the function/scope, the object
*   will be destroyed and the mutex will be unlocked.
*
*   For example:
\code
void UpdateNonvol(void)
{
    BcmMyNonVolSettings *pSettings = BcmMyNonVolSettings::GetSingletonInstance();
    BcmNonVolDevice *pDevice = BcmNonVolDevice::GetSingletonInstance();

    // Get a lock on the nonvol settings so that I have exclusive access, then
    // do the update.
    {
        BcmNonVolSettingsAccessLock nonvolLock;

        pSettings->SetNumberOfEntries(100);
    }

    // The lock will be released when the scope exits, allowing others to use
    // nonvol.  The nonvol device will acquire the lock when it tries to write
    // the settings to a buffer, so I should release the lock before calling it.
    pDevice->Write(*pSettings);
}
\endcode
*
* This class is implemented as part of PR6478.
*
*/
class BcmNonVolSettingsAccessLock
{
public:

    /// Default Constructor.  Initializes the state of the object, locking
    /// the mutex semaphore.
    ///
    BcmNonVolSettingsAccessLock(void);

    /// Destructor.  Unlocks the mutex semaphore.
    ///
    ~BcmNonVolSettingsAccessLock();

private:

    /// Copy Constructor.  Not supported.
    BcmNonVolSettingsAccessLock(const BcmNonVolSettingsAccessLock &otherInstance);

    /// Assignment operator.  Not supported.
    BcmNonVolSettingsAccessLock & operator = (const BcmNonVolSettingsAccessLock &otherInstance);

};


//********************** Inline Method Implementations ***********************


#endif



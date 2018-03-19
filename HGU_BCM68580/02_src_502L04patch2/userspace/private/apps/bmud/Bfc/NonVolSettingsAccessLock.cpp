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
//  Filename:       NonVolSettingsAccessLock.cpp
//  Author:         David Pullen
//  Creation Date:  March 23, 2004
//
//****************************************************************************

//********************** Include Files ***************************************

// My api and definitions...
#include "NonVolSettingsAccessLock.h"

#include "OperatingSystemFactory.h"
#include "MutexSemaphore.h"

//********************** Local Types *****************************************

//********************** Local Constants *************************************

//********************** Local Variables *************************************

//********************** Local Functions *************************************

static BcmMutexSemaphore *pgNonvolLockMutex = NULL;

//********************** Class Method Implementations ************************


BcmNonVolSettingsAccessLock::BcmNonVolSettingsAccessLock(void)
{
    CallTrace("BcmNonVolSettingsAccessLock", "Constructor");

    // Create the mutex if it doesn't already exist.
    //
    /// \todo Can't destroy this mutex at system shutdown, since it isn't a
    ///       static object (it must be a pointer because of polymorphism).
    ///       Need to see if we can find a way to destroy it.
    if (pgNonvolLockMutex == NULL)
    {
        pgNonvolLockMutex = BcmOperatingSystemFactory::NewMutexSemaphore("NonVolSettingsAccessLock Mutex");
    }

    pgNonvolLockMutex->Lock();
}


BcmNonVolSettingsAccessLock::~BcmNonVolSettingsAccessLock()
{
    CallTrace("BcmNonVolSettingsAccessLock", "Destructor");

    pgNonvolLockMutex->Unlock();
}



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
//  Filename:       CountingSemaphore.cpp
//  Author:         David Pullen
//  Creation Date:  Dec 15, 1999
//
//****************************************************************************
//  Description:
//      This is the base class for all types of operating system counting
//      semaphores.
//
//****************************************************************************

//********************** Include Files ***************************************

// My api and definitions...
#include "CountingSemaphore.h"

#include "OperatingSystem.h"
#include "Event.h"

//********************** Local Types *****************************************

//********************** Local Constants *************************************

//********************** Local Variables *************************************

//********************** Class Method Implementations ************************


// Default Constructor.  Stores the name associated with the object.
//
// Parameters:
//      pName - the text name that is to be given to the object, for
//              debugging purposes.
//
// Returns:  N/A
//
BcmCountingSemaphore::BcmCountingSemaphore(const char *pName) :
    fMessageLogSettings("BcmCountingSemaphore")
{
    // Call the helper method to create my object name.
    pfName = BcmOperatingSystem::GenerateName(pName, "CountingSemaphore");

    // Set my instance name.
    fMessageLogSettings.SetInstanceName(pfName);

    fMessageLogSettings.Register();

    // Defer creation of the event to the derived class - they know what type
    // of event to create (and how to create it).
    pfEventToTrigger = NULL;
}


// Destructor.  Frees up any memory/objects allocated, cleans up internal
// state.
//
// Parameters:  N/A
//
// Returns:  N/A
//
BcmCountingSemaphore::~BcmCountingSemaphore()
{
    fMessageLogSettings.Deregister();

    // Delete the event object.
    delete pfEventToTrigger;
    pfEventToTrigger = NULL;

    // Delete the memory associated with the name.
    delete pfName;
    pfName = NULL;
}




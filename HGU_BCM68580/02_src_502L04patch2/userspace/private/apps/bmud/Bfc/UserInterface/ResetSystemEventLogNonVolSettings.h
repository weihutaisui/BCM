//****************************************************************************
//
//  Copyright (c) 2003  Broadcom Corporation
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
//  Filename:       ResetSystemEventLogNonVolSettings.h
//  Author:         Sean Dunlap
//  Creation Date:  October 31, 2005 (Boo!)
//
//****************************************************************************
//  Description:
//
//      This is a system event log for reset events. 
//
//****************************************************************************

#ifndef ResetSystemEventLogNonVolSettings_H
#define ResetSystemEventLogNonVolSettings_H

//********************** Include Files ***************************************

//********************** Include Files ***************************************

// My base class.
#include "SystemEventLogNonVolSettings.h"

//********************** Global Types ****************************************

//********************** Global Variables ************************************

//********************** Class Declaration ***********************************

class BcmResetSystemEventLogNonVolSettings : public BcmSystemEventLogNonVolSettings
{
public:

    // Default Constructor.  Initializes the state of the object, setting all
    // fields to good default values.
    //
    // Parameters:  None.
    //
    // Returns:  N/A
    //
    BcmResetSystemEventLogNonVolSettings();

    // Destructor.  Frees up any memory/objects allocated, cleans up internal
    // state.
    //
    // Parameters:  N/A
    //
    // Returns:  N/A
    //
    virtual ~BcmResetSystemEventLogNonVolSettings();

    /// Returns the pointer to the singleton.  Most objects
    /// in the system will use this method rather than being passed a pointer to
    /// the object.  The singleton pointer will be set up in the base-class
    /// constructor.
    ///
    /// \note   This can return NULL if a singleton has not been set up for the
    ///         system, so you must check for this condition.
    ///
    /// \note   You must not delete this object!
    ///
    /// \note   You should not store a pointer to the object that is returned,
    ///         since it may be deleted and replaced with a new one.
    ///
    /// \return
    ///      A pointer to the program store device that should be used by the
    ///      system.
    ///
    static BcmResetSystemEventLogNonVolSettings *GetSingletonInstance(void);

    
public:

    void LogResetEvent( const char * pEventString );

private:

    /// This is the pointer to the singleton instance of this class, which will
    /// be returned by GetSingletonInstance().
    static BcmResetSystemEventLogNonVolSettings *pfSingletonInstance;

    // Copy Constructor.  Not supported.
    BcmResetSystemEventLogNonVolSettings(const BcmResetSystemEventLogNonVolSettings &otherInstance);

    // Assignment operator.  Not supported.
    BcmResetSystemEventLogNonVolSettings & operator = (const BcmResetSystemEventLogNonVolSettings &otherInstance);
};

//********************** Inline Method Implementations ***********************

#endif


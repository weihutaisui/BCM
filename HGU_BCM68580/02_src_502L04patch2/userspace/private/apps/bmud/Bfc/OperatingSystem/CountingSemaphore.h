//****************************************************************************
//
//  Copyright (c) 1999, 2000  Broadcom Corporation
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
//  Filename:       CountingSemaphore.h
//  Author:         David Pullen
//  Creation Date:  Dec 15, 1999
//
//****************************************************************************

#ifndef COUNTINGSEMAPHORE_H
#define COUNTINGSEMAPHORE_H

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
*   This is the base class for all types of operating system counting
*   semaphores.
*
*   A counting semaphore can be used for many different purposes; it has
*   characteristics of both events and mutexes.  It can act as a type of
*   event, where it keeps track of the number of times the event occurred;
*   it can be used to control access to a set of shared resources, where
*   the semaphore allows a lock for each of N resources, but no more.
*
*   Counting semaphores have the most options of all the wrapper objects.
*   They can be pre-set with a count; they can have a maximum count or can
*   be unbounded; and they can have a BcmEvent instance (if associated with
*   a BcmThread) or can be created without one (if associated with a data
*   object).
*
*   Like mutex semaphores, counting semaphores can be created both by
*   BcmOperatingSystem and BcmOperatingSystemFactory; this allows data
*   objects (who probably don't have access to the BcmOperatingSystem
*   instance of a thread) to create one for their own use.
*
*   Note, however, that a counting semaphore can only be used in conjunction
*   with a BcmEventSet if it is created by BcmOperatingSystem.
*   BcmOperatingSystemFactory creates the semaphore outside of the context
*   of a thread, so there is no BcmEvent associated with it.  You can still
*   wait on it by calling Get() with the appropriate timeout parameters,
*   you just can't add it to an event set.
*/
class BcmCountingSemaphore
{
public:

    /// Default Constructor.  Stores the name associated with the object.
    ///
    /// \param
    ///      pName - the text name that is to be given to the object, for
    ///              debugging purposes.
    ///
    BcmCountingSemaphore(const char *pName = NULL);

    /// Destructor.  Frees up any memory/objects allocated, cleans up internal
    /// state.
    ///
    virtual ~BcmCountingSemaphore();

    /// Values that can be used in the call to Get().
    typedef enum
    {
        kTimeout,
        kForever
    } GetMode;

    /// Tries to get access to the semaphore, reducing the its count by 1.  If
    /// the count is 0, then the thread will block (based on the timeout
    /// parameters).
    ///
    /// By default, if you call this with no parameters, then it will not
    /// time out waiting for the semaphore.  If you call it with kTimeout and 0
    /// as the timeoutMS value, then it will return immediately if the semaphore
    /// is not available.
    ///
    /// This is a pure virtual method; derived classes must override this to
    /// provide the OS-specific implementation for getting a semaphore.
    ///
    /// \param
    ///      mode - tells whether or not you want to time out if the semaphore is
    ///             not granted in the specified amount of time.
    /// \param
    ///      timeoutMS - the amount of time to wait for the semaphore.  This is
    ///                  ignored if mode is not set to kTimeout.
    ///
    /// \retval
    ///      true if the semaphore was successfully gotten.
    /// \retval
    ///      false if the semaphore was not available (timed out waiting for it
    ///          to be released, etc.).
    ///
    virtual bool Get(GetMode mode = kForever, unsigned long timeoutMS = 0) = 0;

    /// Releases the semaphore, increasing its count by 1.  If the semaphore has
    /// a ceiling to the count, then it will not be increased beyond it.  If the
    /// count is 0 and a thread is waiting for the semaphore, then it will be
    /// released to run.
    ///
    /// This is a pure virtual method; derived classes must override this to
    /// provide the OS-specific implementation for unlocking a semaphore.
    ///
    /// \retval
    ///      true if the semaphore was released.
    /// \retval
    ///      false if there was a problem (OS-specific).
    ///
    virtual bool Release(void) = 0;

    /// This method lets a thread read the current count of the semaphore.
    ///
    /// This is a pure virtual method; derived classes must override this to
    /// provide the OS-specific implementation for querying a semaphores count.
    ///
    /// \return
    ///      The semaphores current count.
    ///
    virtual unsigned int GetCount(void) const = 0;

    /// Returns the event that is triggered when the semaphore is released.  This
    /// is primarily used by BcmEventSet when the timer is added to the list of
    /// events that are to be waited on.
    ///
    /// \return
    ///      The semaphore's event.
    ///
    inline BcmEvent *GetEvent(void) const;

    /// An alternate way to get the pointer to the event.  You can use it this
    /// way:
    ///
    /// \code
    ///   BcmCountingSemaphore *pSemaphore;
    ///   BcmEvent *pEvent;
    ///
    ///   pEvent = (*pSemaphore);
    /// \endcode
    ///
    /// \return
    ///      The event associated with the semaphore.
    ///
    inline operator BcmEvent * () const;

    /// Simple accessor for the name of this object.
    ///
    /// \return
    ///      The pointer to the name string.
    ///
    inline const char *Name(void) const;

protected:

    /// The event that will be triggered when the semaphore is released.  This
    /// must be created in the constructor of the OS-specific derived class, but
    /// it is destroyed by the destructor of this class.
    BcmEvent *pfEventToTrigger;

    /// My assigned name.
    char *pfName;

    /// Controls message logging.
    BcmMessageLogSettings fMessageLogSettings;

private:

    /// Copy Constructor.  Not supported.
    BcmCountingSemaphore(BcmCountingSemaphore &otherInstance);

    /// Assignment operator.  Not supported.
    BcmCountingSemaphore & operator = (BcmCountingSemaphore &otherInstance);

};


//********************** Inline Method Implementations ***********************


/// Returns the event that is triggered when the semaphore is released.  This
/// is primarily used by BcmEventSet when the timer is added to the list of
/// events that are to be waited on.
///
/// \return
///      The semaphore's event.
///
inline BcmEvent *BcmCountingSemaphore::GetEvent(void) const
{
    return pfEventToTrigger;
}


/// An alternate way to get the pointer to the event.  You can use it this
/// way:
///
/// \code
///   BcmCountingSemaphore *pSemaphore;
///   BcmEvent *pEvent;
///
///   pEvent = (*pSemaphore);
/// \endcode
///
/// \return
///      The event associated with the semaphore.
///
inline BcmCountingSemaphore::operator BcmEvent * () const
{
    return pfEventToTrigger; 
}


/// Simple accessor for the name of this object.
///
/// \return
///      The pointer to the name string.
///
inline const char *BcmCountingSemaphore::Name(void) const
{
    return pfName;
}


#endif



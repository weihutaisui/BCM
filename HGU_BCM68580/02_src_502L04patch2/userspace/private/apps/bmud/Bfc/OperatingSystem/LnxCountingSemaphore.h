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
//  $Id: LnxCountingSemaphore.h 1.4 2006/08/02 19:50:22Z taskiran Release $
//
//  Filename:       LnxCountingSemaphore.h
//  Author:         Mark Rush
//  Creation Date:  Oct. 19, 2001
//
//****************************************************************************
//  Description:
//      This is the Linux implementation for counting semaphores.
// The pthreads library implements POSIX 1003.1b semaphores
//
//****************************************************************************

#ifndef LNXCOUNTINGSEMAPHORE_H
#define LNXCOUNTINGSEMAPHORE_H

//********************** Include Files ***************************************

#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>
#include "CountingSemaphore.h"

//********************** Global Types ****************************************

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Class Declaration ***********************************


class BcmLnxCountingSemaphore : public BcmCountingSemaphore
{
public:

    // Initializing Constructor.  Stores the name assigned to the object, and
    // creates the counting semaphore with the specified parameters.
    //
    // Parameters:
    //      isBounded - set this to true if you want the semaphore to have
    //                  a maximum count beyond which it cannot be incremented.
    //                  Set it to false if you don't want a maximum count.
    //      maxCount - the max count to be used, if isBounded is true.  This
    //                 is ignored if isBounded is false.
    //      initialCount - the count that the semaphore should start off with.
    //      pEvent - an Event to notify when Release() is called
    //      pName - the text name that is to be given to the object, for
    //              debugging purposes.
    //
    // Returns:  N/A
    //
    BcmLnxCountingSemaphore(bool isBounded, unsigned int maxCount,
                             unsigned int initialCount,
                             BcmEvent *pEvent = NULL,
                             const char *pName = NULL);

    // Destructor.  Frees up any memory/objects allocated, cleans up internal
    // state.
    //
    // Parameters:  N/A
    //
    // Returns:  N/A
    //
    ~BcmLnxCountingSemaphore();

    // Tries to get access to the semaphore, reducing its count by 1.  If
    // the count is 0, then the thread will block (based on the timeout
    // parameters).
    //
    // By default, if you call this with no parameters, then it will not
    // time out waiting for the semaphore.  If you call it with kTimeout and 0
    // as the timeoutMS value, then it will return immediately if the semaphore
    // is not available.
    //
    // Parameters:
    //      mode - tells whether or not you want to time out if the semaphore is
    //             not granted in the specified amount of time.
    //      timeoutMS - the amount of time to wait for the semaphore.  This is
    //                  ignored if mode is not set to kTimeout.
    //
    // Returns:
    //      true if the semaphore was successfully gotten.
    //      false if the semaphore was not available (timed out waiting for it
    //          to be released, etc.).
    //
    virtual bool Get(GetMode mode = kForever, unsigned long timeoutMS = 0);

    // Releases the semaphore, increasing its count by 1.  If the semaphore has
    // a ceiling to the count, then it will not be increased beyond it.  If the
    // count is 0 and a thread is waiting for the semaphore, then it will be
    // released to run.
    //
    // Parameters:  None.
    //
    // Returns:
    //      true if the semaphore was released.
    //      false if there was a problem (OS-specific).
    //
    virtual bool Release(void);

    // This method lets a thread read the current count of the semaphore.
    //
    // Parameters:  None.
    //
    // Returns:
    //      The semaphores current count.
    //
    virtual unsigned int GetCount(void) const;


private:

    bool fIsBounded;
    unsigned int fMaxCount;

	bool fSemInitialized;

	// The POSIX semaphore
	sem_t fSemaphore;
};


//********************** Inline Method Implementations ***********************

#endif



//****************************************************************************
//
// Copyright (c) 2007 Broadcom Corporation
//
// This program is the proprietary software of Broadcom Corporation and/or
// its licensors, and may only be used, duplicated, modified or distributed
// pursuant to the terms and conditions of a separate, written license
// agreement executed between you and Broadcom (an "Authorized License").
// Except as set forth in an Authorized License, Broadcom grants no license
// (express or implied), right to use, or waiver of any kind with respect to
// the Software, and Broadcom expressly reserves all rights in and to the
// Software and all intellectual property rights therein.  IF YOU HAVE NO
// AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY,
// AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE
// SOFTWARE.  
//
// Except as expressly set forth in the Authorized License,
//
// 1.     This program, including its structure, sequence and organization,
// constitutes the valuable trade secrets of Broadcom, and you shall use all
// reasonable efforts to protect the confidentiality thereof, and to use this
// information only in connection with your use of Broadcom integrated circuit
// products.
//
// 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
// "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS
// OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
// RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL
// IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR
// A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET
// ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME
// THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
//
// 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM
// OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
// INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY
// RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM
// HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN
// EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1,
// WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY
// FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
//
//****************************************************************************
//  $Id: LnxMutexSemaphore.h 1.6 2007/09/21 19:55:28Z koneal Release $
//
//  Filename:       LnxMutexSemaphore.h
//  Author:         Mark Rush
//  Creation Date:  Oct. 19, 2001
//
//****************************************************************************
//  Description:
//      This is the Linux implementation for mutex semaphores.
//
//****************************************************************************

#ifndef LNXMUTEXSEMAPHORE_H
#define LNXMUTEXSEMAPHORE_H

//********************** Include Files ***************************************
#include <pthread.h>
#include "MutexSemaphore.h"

//********************** Global Types ****************************************

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Class Declaration ***********************************


class BcmLnxMutexSemaphore : public BcmMutexSemaphore
{
public:

    // Initializing Constructor.  Stores the name assigned to the object, and
    // creates the counting semaphore with the specified parameters.
    //
    // Parameters:
    //      pName - the text name that is to be given to the object, for
    //              debugging purposes.
    //
    // Returns:  N/A
    //
    BcmLnxMutexSemaphore(const char *pName = NULL);

    // Destructor.  Frees up any memory/objects allocated, cleans up internal
    // state.
    //
    // Parameters:  N/A
    //
    // Returns:  N/A
    //
    ~BcmLnxMutexSemaphore();

    // Tries to get access to the mutex, If the mutex is available, then
	// the thread will block (based on the timeout  parameters).
    //
    // By default, if you call this with no parameters, then it will not
    // time out waiting for the mutex.  If you call it with kTimeout and 0
    // as the timeoutMS value, then it will return immediately if the mutex
    // is not available (a nonblocking attempt at aquiring the mutex).
    //
    // Parameters:
    //      mode - tells whether or not you want to time out if the semaphore is
    //             not granted in the specified amount of time.
    //      timeoutMS - the amount of time to wait for the semaphore.  This is
    //                  ignored if mode is not set to kTimeout.
    //
    // Returns:
    //      true if the mutex was successfully gotten.
    //      false if the mutex was not available (timed out waiting for it
    //          to be released, etc.).
    //
    virtual bool Lock(LockMode mode = kForever, unsigned long timeoutMS = 0);

    // Releases the mutex.
    //
    // Parameters:  None.
    //
    // Returns:
    //      true if the semaphore was released.
    //      false if there was a problem (OS-specific).
    //
    virtual bool Unlock(void);

    /// Simple accessor for the ID of the thread which currently has the
    /// mutex locked.  If not locked, this method will return 0.
    ///
    /// The base class implementation of this method always returns 0 since 
    /// the base class doesn't know the owning thread ID.  Derived OS 
    /// specific objects may override this method to provide this information.
    ///
    /// \return
    ///      The thread ID of the thread which currently has the mutex locked.
    ///
    virtual unsigned long ThreadId(void) const;

private:

	// Who has the lock, for debugging, __m_owner in pthread_mutex_t is not the id!
	pthread_t fOwnerThread;	
	// The POSIX mutex
    pthread_mutex_t fLnxMutexLock;
	// The Mutex attribute 
	pthread_mutexattr_t fLnxMutexAttr;
	// The Mutex count
	// I am reluctant to use __m_count in pthread_mutex_t
	int fMutexCount;
};


//********************** Inline Method Implementations ***********************

#endif



//****************************************************************************
//
// Copyright (c) 1999-2009 Broadcom Corporation
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
//  $Id$
//
//  Filename:       Event.h
//  Author:         David Pullen
//  Creation Date:  Feb 17, 1999
//
//****************************************************************************

#ifndef EVENT_H
#define EVENT_H

//********************** Include Files ***************************************

// For bool and other portable types.
#include "typedefs.h"

#include "MessageLog.h"

//********************** Global Types ****************************************

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Forward Declarations ********************************

//********************** Class Declaration ***********************************


/** \ingroup OsWrappers
*
*   This is the abstract base class (and wrapper) for operating system
*   events.
*
*   BcmEvents are generally used as a means of simple communication between
*   threads, when one thread needs to tell another thread that a particular
*   event has occurred.  BcmEvents contain no other information about the
*   event, such as how many times it occurred, when it occurred, who sent
*   it, or any other data associated with the event.
*
*   An event must be created by the thread that wants to receive the
*   notification.  This is because the event is valid only in the context
*   of the thread that created it.  The sender of the event (who calls
*   Send()) can be any thread, including the thread that created the event.
*/
class BcmEvent
{
public:

    /// Default Constructor.  Stores the name assigned to the object.
    ///
    /// \param
    ///      pName - the text name that is to be given to the object, for
    ///              debugging purposes.
    ///
    BcmEvent(const char *pName = NULL);

    /// Destructor.  Frees up any memory and other resources that were allocated.
    ///
    virtual ~BcmEvent();

    /// This is the method that a thread will call when it wants to "post" the
    /// event, telling the waiting thread that the event has occurred.  Note that
    /// if the event has been posted and you send the event again, both of these
    /// are collapsed down to a single event.  It does not count the number of
    /// times an event has been posted.
    ///
    /// This is a pure virtual method; derived classes must override this to
    /// provide the OS-specific implementation for sending an event.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem.
    ///
    virtual bool Send(void) = 0;

    /// Values that can be used in the call to Wait().
    typedef enum
    {
        kTimeout,
        kForever
    } WaitMode;
    
    /// This is the method that a thread will call when it wants to wait for
    /// an event to be sent.  Optionally, the thread can wait forever for the
    /// event, timeout if the event isn't sent within a certain number of
    /// milliseconds, or return immediately regardless of whether or not the
    /// event has been sent.
    ///
    /// By default, if you call this method with no parameters, then you will
    /// wait forever for the event.
    ///
    /// This is a pure virtual method; derived classes must override this to
    /// provide the OS-specific implementation for waiting for an event.
    ///
    /// \param
    ///      mode - the wait mode (forever or timeout).
    /// \param
    ///      timeoutMS - the number of milliseconds to wait for the event to be
    ///                  sent; only used if mode is set to timeout.
    ///
    /// \retval
    ///      true if the event occurred.
    /// \retval
    ///      false if the event didn't occur (timed out waiting).
    ///
    virtual bool Wait(WaitMode mode = kForever, unsigned long timeoutMS = 0) = 0;

    /// This method clears any pending (stale) event that may already have been
    /// processed.  It is commonly called just before calling Wait().
    ///
    /// This is equivalent to calling Wait(kTimeout, 0) and ignoring the result.
    ///
    virtual void Clear(void);

    /// Simple accessor for the name of this object.
    ///
    /// \return
    ///      The pointer to the name string.
    ///
    inline const char *Name(void) const;

    /// This method returns the OS event bit for the associated event.
    ///
    /// \retval
    ///      The event bit.
    ///
    virtual uint32 EventBit(void) const;

protected:

    /// My assigned name.
    char *pfName;

    /// Controls message logging.
    BcmMessageLogSettings fMessageLogSettings;

private:

    /// Copy Constructor.  Not supported.
    BcmEvent(BcmEvent &otherInstance);

    /// Assignment operator.  Not supported.
    BcmEvent & operator = (BcmEvent &otherInstance);

};


//********************** Inline Method Implementations ***********************


/// Simple accessor for the name of this object.
///
/// \return
///      The pointer to the name string.
///
inline const char *BcmEvent::Name(void) const
{
    return pfName;
}

#endif



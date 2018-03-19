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
//  Filename:       Event.cpp
//  Author:         David Pullen
//  Creation Date:  Feb 17, 1999
//
//****************************************************************************
//  Description:
//      This is the abstract base class (and wrapper) for operating system
//      events.  These are used when one thread wants to notify another thread
//      that something has happened.  See the BcmEvent scenario diagram in 
//      OSWrapper.vsd for more information on how this class should be used.
//
//****************************************************************************

//********************** Include Files ***************************************

// My api and definitions...
#include "Event.h"

#include "OperatingSystem.h"

//********************** Local Types *****************************************

//********************** Local Constants *************************************

//********************** Local Variables *************************************

//********************** Class Method Implementations ************************


// Default Constructor.  Stores the name assigned to the object.
//
// Parameters:
//      pName - the text name that is to be given to the object, for
//              debugging purposes.
//
// Returns:  N/A
//
BcmEvent::BcmEvent(const char *pName) :
    fMessageLogSettings("BcmEvent")
{
    // Call the helper method to create my object name.
    pfName = BcmOperatingSystem::GenerateName(pName, "Event");

    // Set my instance name.
    fMessageLogSettings.SetInstanceName(pfName);

    fMessageLogSettings.Register();
}


// Destructor.  Frees up any memory and other resources that were allocated.
//
// Parameters:  N/A
//
// Returns:  N/A
//
BcmEvent::~BcmEvent()
{
    fMessageLogSettings.Deregister();

    // Delete the memory associated with the name.
    delete pfName;
    pfName = NULL;
}


// This method clears any pending (stale) event that may already have been
// processed.  It is commonly called just before calling Wait().
//
// This is equivalent to calling Wait(kTimeout, 0) and ignoring the result.
//
// Parameters:  None.
//
// Returns:  Nothing.
//
void BcmEvent::Clear(void)
{
    gInfoMsg(fMessageLogSettings, "Clear") << "Clearing the event..." << endl;

    // True to the above comment, call Wait() and ignore the result.
    Wait(kTimeout, 0);
}


// This is a stub method for the Event base class.  It should never be called.
//
// Parameters:  None.
//
// Returns:     Bit 0 as an error but outputs an error as well.
//
uint32 BcmEvent::EventBit(void) const
{
    gErrorMsg(fMessageLogSettings, "EventBit") << "EventBit not defined by the inherited object!  This should never happen! " << endl;

    return(0);
}



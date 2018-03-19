//****************************************************************************
//
// Copyright (c) 2000-2009 Broadcom Corporation
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
//  Filename:       BfcAppNonVolSettings.cpp (was CmAppNonVolSettings.cpp)
//  Author:         David Pullen
//  Creation Date:  Feb 10, 2000
//
//****************************************************************************
//  Description:
//      These are the Non-Volatile settings for the core BFC application.  They
//      control the startup behavior for the BFC core.
//
//****************************************************************************

//********************** Include Files ***************************************

// My api and definitions...
#include "BfcAppNonVolSettings.h"
#include "MergedNonVolDefaults.h"

#include <string.h>
#include <iostream>
#include <iomanip>

//********************** Local Types *****************************************

//********************** Local Constants *************************************

// Dynamic rev = 0.1
// Permanent rev = 0.1
const uint8 kCurrentVersionMajor[2] =
{
    // Dynamic, Permanent
    0, 0
};

const uint8 kCurrentVersionMinor[2] =
{
    // Dynamic, Permanent
    1, 1
};

//********************** Local Variables *************************************

//********************** Class Method Implementations ************************


// Set this to NULL initially.  This will be set up in my constructor.
BcmBfcAppNonVolSettings *BcmBfcAppNonVolSettings::pgSingletonInstance = NULL;


// Default Constructor.  Initializes the state of the object, setting all
// fields to good default values.
//
// Parameters:  None.
//
// Returns:  N/A
//
BcmBfcAppNonVolSettings::BcmBfcAppNonVolSettings(void) :
    BcmNonVolSettings("BFC Application NonVol Settings")
{
    // Override the class name in the message log settings that was set by my
    // base class.
    fMessageLogSettings.SetModuleName("BcmBfcAppNonVolSettings");

    // Just let this do the work.
    ResetDefaults(kDynamicSection);
    ResetDefaults(kPermanentSection);

    // Set myself as the singleton.
    if (pgSingletonInstance == NULL)
    {
        gInfoMsg(fMessageLogSettings, "BcmBfcAppNonVolSettings")
            << "Setting up the singleton pointer." << endl;
        pgSingletonInstance = this;
    }
    else
    {
        gWarningMsg(fMessageLogSettings, "BcmBfcAppNonVolSettings")
            << "Singleton pointer is not NULL!  There are multiple instances!  Leaving the singleton pointer alone..." << endl;
    }
}


// Destructor.  Frees up any memory/objects allocated, cleans up internal
// state.
//
// Parameters:  N/A
//
// Returns:  N/A
//
BcmBfcAppNonVolSettings::~BcmBfcAppNonVolSettings()
{
    // Clear the singleton pointer.
    if (pgSingletonInstance == this)
    {
        gInfoMsg(fMessageLogSettings, "~BcmBfcAppNonVolSettings")
            << "Clearing the singleton pointer." << endl;

        pgSingletonInstance = NULL;
    }
    else
    {
        gWarningMsg(fMessageLogSettings, "~BcmBfcAppNonVolSettings")
            << "I'm not the singleton instance!  Leaving the singleton pointer alone..." << endl;
    }
}


// Set all values to a known good state.
//
// Note that the derived class should set the version numbers to the desired
// values in case they were changed (by ReadFrom(), etc).
//
// This method may be called if ReadFrom() returns false in order to ensure
// that there are valid settings in the object.
//
// Parameters:
//      section - the section that should be reset.
//
// Returns:  Nothing.
//
void BcmBfcAppNonVolSettings::ResetDefaults(NonVolSection section)
{
    // Set the magic numbers.  This is ASCII-hex for 'CMAp'.
    //
    // Unfortunately, this magic number was chosen poorly; there really isn't
    // anything about this that is CM-specific, but once a magic number is
    // selected, it really needs to be kept.
    fMagicNumber = 0x434d4170;

    // Set the current version number.
    fVersionMajor[section] = kCurrentVersionMajor[section];
    fVersionMinor[section] = kCurrentVersionMinor[section];

    if (section == kPermanentSection)
    {
        // By default, go for fast bootup.  The user doesn't have the option
        // of skiping drivers or stopping at the console.
        fEnableDisableBitmask = kSkipDriverInitPromptDisable | kStopAtAppPromptDisable;
    }
    
    if (section == kDynamicSection)
    {
        fSerialConsoleMode = kDefaultValue_SerialConsoleMode; // PR 11916
    }
}


/// Returns the Is-Manufactured bool.  For a Composite object, it will 
/// check all of its contained settings groups and return true only if all 
/// of them return true.
///
/// The criteria for whether a group is manufactured is specific to the
/// group.  Generally, a group should return 'true' only if all critical
/// settings have been changed from their default values to proper
/// deployable values.  Of course there is no way for the base class to
/// know this, so the default behavior for the base class is to always
/// return 'true'.  Derived classes must override this method to check
/// critical settings and return a meaningful value.  It is entirely
/// possible that a given derived object may wish to use the base class
/// implementation, if there are no settings which must be manufactured
/// on a per-device basis.  This method is used at boot-time to determine
/// if a device is adequately manufactured, and if not, to take appropriate
/// action.
///
/// \param
///      section - the section that is being queried.
///
/// \param
///      hint - if the section is not manufactured, this argument will
///             be filled in with a hint as to what settings need to be
///             specified to complete manufacturing.
///
/// \retval
///      true if this settings group was upgraded.
/// \retval
///      false if this settings group was not upgraded.
///
bool BcmBfcAppNonVolSettings::IsManufactured(NonVolSection section, BcmString &hint) const
{
    // No dynamic settings need manufacturing.
    if (section == kDynamicSection)
    {
        return true;
    }
    
    // We never want auto stop at console to be true for a manufactured
    // device
    if (AutoStopAtConsole() == true)
    {
        if (hint.length())
          hint += ", ";
        hint += "Stop at console enabled";
        
        return false;
    }

    return true;
}


// Returns the pointer to the singleton instance for this class.  Most
// objects in the system will use this method rather than being passed a
// pointer to it.  The singleton pointer will be set up in the base-class
// constructor.
//
// NOTES:  This can return NULL if a singleton has not been set up for the
//         system, so you must check for this condition.
//
//         You must not delete this object!
//
//         You should not store a pointer to the object that is returned,
//         since it may be deleted and replaced with a new one.
//
// Parameters:  None.
//
// Returns:
//      A pointer to the instance that should be used by the system.
//
BcmBfcAppNonVolSettings *BcmBfcAppNonVolSettings::GetSingletonInstance(void)
{
    if (pgSingletonInstance == NULL)
    {
        gLogMessageRaw
            << "BcmBfcAppNonVolSettings::GetSingletonInstance:  WARNING - the singleton instance is NULL, and someone is accessing it!" << endl;
    }
    
    return pgSingletonInstance;
}


// Here's where I write my members to the buffer in a flat format, in the
// order described at the top of this file.  All multibyte values are
// written in network byte order.
//
// Parameters:
//      outputBuffer - the buffer into which the settings should be written.
//      section - the section that should be written.
//
// Returns:
//      true if successful.
//      false if there was a problem (usually the buffer is too small).
//
bool BcmBfcAppNonVolSettings::WriteToImpl(BcmOctetBuffer &outputBuffer,
                                         NonVolSection section) const
{
    // Append the settings one at a time; if any of these fails, then the buffer
    // is probably too small.
    if (section == kPermanentSection)
    {
        if (!outputBuffer.NetworkAddToEnd(fEnableDisableBitmask))
        {
            // Log the error.
            gErrorMsg(fMessageLogSettings, "WriteToImpl") 
                << "Buffer too small to hold all of the settings!" << endl;

            return false;
        }
    }
    
    // Dynamic
    else
    {
        int Console = (int)fSerialConsoleMode; // cast the enum
        
        if (!outputBuffer.NetworkAddToEnd(Console)) // PR 11916
        {
            // Log the error.
            gErrorMsg(fMessageLogSettings, "WriteToImpl") 
                << "Buffer too small to hold console oper mode!" << endl;

            return false;
        }
    }

    // If we got this far, then it worked!
    return true;
}


// Here's where I read my members from the buffer in a flat format, in the
// order described at the top of this file.  All multibyte values are read
// in network byte order.
//
// The currentOffset parameter tells the derived class where the app data
// starts in the buffer.  It can be used directly in the calls to Read() or
// NetworkRead() in BcmOctetBuffer (which expects an offset value).
//
// Parameters:
//      inputBuffer - the buffer from which the settings should be read.
//      currentOffset - the offset into the buffer from which the settings
//                      should be read.  This is necessary to allow the
//                      derived class to skip the header.
//      section - the section that should be read.
//
// Returns:
//      true if successful.
//      false if there was a problem (version number not supported, not
//          enough bytes in the buffer, etc.).
//
bool BcmBfcAppNonVolSettings::ReadFromImpl(const BcmOctetBuffer &inputBuffer, 
                                          unsigned int &currentOffset,
                                          NonVolSection section)
{
    // For this object, we skipped 0.0 for both dynamic and permanent, so this
    // should never be read.  If it is 0.0, then this is a problem, so we bail
    // with comment.
    if ((fVersionMajor[section] == 0) && (fVersionMinor[section] == 0))
    {
        gErrorMsg(fMessageLogSettings, "ReadFromImpl")
            << "Read unsupported version (0.0)!  These settings are not valid!" << endl;

        return false;
    }

    // Read the settings one at a time; if any of these fails, then the buffer
    // is probably too small.
    if (section == kPermanentSection)
    {
        /* Version 0.1 */
        
        if (!inputBuffer.NetworkRead(currentOffset, fEnableDisableBitmask))
        {
            // Log the error.
            gErrorMsg(fMessageLogSettings, "ReadFromImpl") 
                << "Buffer too small; doesn't have all of the settings!" << endl;

            return false;
        }
        
        // Do whatever validation is necessary for the values.
    }
    
    else // dynamic
    {
        int Console;
        
        // For the Dynamic Settings, we skipped 0.0, so we should never read
        // this.  If it is 0.0, then this is a problem, so we bail with comment.
        if ((fVersionMajor == 0) && (fVersionMinor == 0))
        {
            gErrorMsg(fMessageLogSettings, "ReadFromImpl")
                << "Read unsupported dynamic version (0.0)!  These settings are not valid!" << endl;

            return false;
        }

        /* Rev 0.1 (PR 11916) */
        
        if (!inputBuffer.NetworkRead(currentOffset, Console))
        {
            // Log the error.
            gErrorMsg(fMessageLogSettings, "ReadFromImpl") 
                << "Buffer too small; doesn't have the console mode setting!" << endl;

            return false;
        }
        else
            fSerialConsoleMode = (ConsoleMode)Console;
    }
    
    // Call the helper code in the base class to check for version mismatch.
    CheckForVersionUpgrade(kCurrentVersionMajor[section],
                           kCurrentVersionMinor[section], section);

    // If we got this far, then if worked!
    return true;
}



// Causes me to do formatted output of my settings to the specified ostream,
// after the base class does its stuff.
//
// Parameters:
//      outputStream - the ostream to be used.
//
// Returns:
//      The reference to the ostream that was passed in.
//
ostream & BcmBfcAppNonVolSettings::PrintImpl(ostream & outputStream) const
{
    #if !(BCM_REDUCED_IMAGE_SIZE)

    const char *ConsoleStr[] = { "disabled", "readOnly", "readWrite" };
    
    // Dump the settings out one at a time.
    outputStream 
        << "\nCM Application Permanent NonVol Settings:\n\n"
        << setw(38) << "Automatically stop at console = " << AutoStopAtConsole() << '\n'
        << setw(38) << "'Skip driver init' Prompt Enabled = " << DriverInitPrompt() << '\n'    
        << setw(38) << "'Stop at console' Prompt Enabled = " << StopAtConsolePrompt() << '\n';
        
    outputStream
        << "\nCM Application Dynamic NonVol Settings:\n\n"
        // Printing console mode is somewhat pointless, since in order to print
        // it we must be in R/W mode.  Still...
        << setw(38) << "Serial console mode = " << ConsoleStr[(int)SerialConsoleMode()] << '\n'; // PR 11916

    #else

    outputStream
        << "\nNonvol Dump:\n\n"
        << AutoStopAtConsole() << '\n'
        << DriverInitPrompt() << '\n'
        << StopAtConsolePrompt() << '\n'
        << SerialConsoleMode() << '\n';

    #endif
    
    return outputStream;
}




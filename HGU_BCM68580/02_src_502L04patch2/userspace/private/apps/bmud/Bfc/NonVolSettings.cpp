//****************************************************************************
//
// Copyright (c) 2000-2010 Broadcom Corporation
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
//  Filename:       NonVolSettings.cpp
//  Author:         David Pullen
//  Creation Date:  Feb 9, 2000
//
//****************************************************************************
//  Description:
//      This is the abstract base class for all kinds of application-specific
//      Non-Volatile settings objects.  An application will define its own
//      class derived from this one, giving it the values and ordering, as well
//      as methods for converting to/from a flat buffer.
//
//      Each NonVol Settings object has a split personality, containing both
//      Permanent and Dynamic settings.  The section that is being operated on
//      is specified as a parameter to most of the methods.  We went with this
//      approach rather than splitting it out to 2 objects because the settings
//      really are related, they are just stored in different locations so that
//      the permanent settings can be protected from write failures.
//
//      Note that the settings are independant of the device to which they are
//      being stored, allowing the same application settings to be used on
//      multiple target platforms and devices!
//
//      The derived class will generally be placed in the application directory,
//      since it is application-specific; if it is used in multiple apps, then
//      it may be appropriate to place it in a common directory.
//
//      This class provides the common API, as well as some helper methods, for
//      all of the settings objects, providing a common interface for all of the
//      possible types of Non-Volatile devices.
//
//      All NonVolSettings have the following 8-byte header layout:
//
//        +--------+-----------+----------+----------+------------------------+
//        | Length | Magic Num | VerMajor | VerMinor | {App settings data...} |
//        +--------+-----------+----------+----------+------------------------+
//             2         4           1          1                n
//
//      The length is set by the base class after the derived class has written
//      all of its data.  On input, it is used bound the number of bytes that
//      are available for reading.  It includes all bytes, including the length
//      field itself, and will always be 8 or larger.
//
//      The magic and version numbers are set by the derived class in the
//      constructor, or are read from the device.  The Magic Number allows the
//      derived class to be sure that it was what wrote the settings (so that
//      it doesn't read in another object's settings).
//
//      The Checksum or other information is handled by the specific
//      NonVolDevice rather than by the settings.
//
//****************************************************************************

//********************** Include Files ***************************************

// My api and definitions...
#include "NonVolSettings.h"

#include <iomanip>
#include <string.h>
#include <ctype.h>

//********************** Local Types *****************************************

//********************** Local Constants *************************************

//********************** Local Variables *************************************

//********************** Class Method Implementations ************************


// Default Constructor.  Initializes the state of the object, setting the
// magic and version numbers to 0.
//
// Parameters:
//      pInstanceName - the name to be used with the message log settings.
//
// Returns:  N/A
//
BcmNonVolSettings::BcmNonVolSettings(const char *pInstanceName) :
    fMessageLogSettings("BcmNonVolSettings")
{
    CallTrace("BcmNonVolSettings", "BcmNonVolSettings");

    // Assume that this is not "unrecognized", that it is defaulted, and that
    // it is not a Composite.
    fIsRecognized = true;
    fIsDefault[kDynamicSection] = true;
    fIsDefault[kPermanentSection] = true;
    fIsUpgraded[kDynamicSection] = false;
    fIsUpgraded[kPermanentSection] = false;
    fIsComposite = false;

    // Set the instance name passed by the derived class.
    fMessageLogSettings.SetInstanceName(pInstanceName);

    // Tell my settings to register itself with its command table.
    fMessageLogSettings.Register();
    
    pfParent = NULL;

    // Set protected and private members to a known state.
    fMagicNumber = 0;

    fVersionMajor[0] = 0;
    fVersionMinor[0] = 0;
    
    fVersionMajor[1] = 0;
    fVersionMinor[1] = 0;

    fPreviousVersionMajor[0] = 0;
    fPreviousVersionMinor[0] = 0;
    
    fPreviousVersionMajor[1] = 0;
    fPreviousVersionMinor[1] = 0;
    
    // Set the 'native' versions to an invalid value just in case a derived
    // class doesn't call CheckForVersionUpgrade (eg: BcmUnrecognizedNonVolSettings)
    fNativeVersionMajor[0] = kNativeVersionUnknown;
    fNativeVersionMinor[0] = kNativeVersionUnknown;
    
    fNativeVersionMajor[1] = kNativeVersionUnknown;
    fNativeVersionMinor[1] = kNativeVersionUnknown;

    pfExtraGroupData[0] = NULL;
    pfExtraGroupData[1] = NULL;
}


// Destructor.  Frees up any memory/objects allocated, cleans up internal
// state.
//
// Parameters:  N/A
//
// Returns:  N/A
//
BcmNonVolSettings::~BcmNonVolSettings()
{
    CallTrace("BcmNonVolSettings", "~BcmNonVolSettings");

    // Tell my settings to deregister itself from its command table.
    fMessageLogSettings.Deregister();
    
    delete pfExtraGroupData[0];
    pfExtraGroupData[0] = NULL;

    delete pfExtraGroupData[1];
    pfExtraGroupData[1] = NULL;

    // Forget my parent.
    pfParent = NULL;
}


// This allows some client (usually some other non-vol settings instance)
// set another settings instance as my parent.  Usually, one settings
// instance will act as a parent container for a bunch of subsystem-specific
// settings instances.
//
// If you call this with NULL, then the parent will be forgotten.
//
// Parameters:
//      pParent - pointer to the instance that you want to be my parent.
//
// Returns:  Nothing.
//
void BcmNonVolSettings::SetParent(BcmNonVolSettings *pParent)
{
    CallTrace("BcmNonVolSettings", "SetParent");

    // Just store this.
    pfParent = pParent;
}


// Returns the instance name for this object.
//
// Parameters:  None.
//
// Returns:
//      The instance name.
//
const char *BcmNonVolSettings::GetInstanceName(void) const
{
    const char *pInstanceName = fMessageLogSettings.GetInstanceName();

    CallTrace("BcmNonVolSettings", "GetInstanceName");

    if (pInstanceName == NULL)
    {
        pInstanceName = "{Unknown Settings object}";
    }

    return pInstanceName;
}


// Returns the Is-Recognized bool.
//
// Parameters:  None.
//
// Returns:
//      true if this settings group is known.
//      false if this settings group is unrecognized.
//
bool BcmNonVolSettings::IsRecognized(void) const
{
    CallTrace("BcmNonVolSettings", "IsRecognized");

    return fIsRecognized;
}


/// Returns a pointer to the nonvol object corresponding to the specified
/// magic number value.  This is most useful for getting access to
/// unrecognized nonvol group objects, which don't have a singleton
/// instance.
///
/// \note
///     This can return NULL if there is no object with specified magic
///     number!
///
/// \param
///     magicNumber - the magic number of the nonvol instance to be returned.
///
/// \return
///     A pointer to the nonvol instance corresponding to the specified
///     magic number, or NULL if no object was found.
///
const BcmNonVolSettings *BcmNonVolSettings::Find(uint32 magicNumber) const
{
    return NULL;
}


// Returns the Is-Default bool.
//
// Parameters:
//      section - the section that is being queried.
//
// Returns:
//      true if this settings group contains only default data (and is not
//          really valid).
//      false if this settings group contains valid data read from the
//          device.
//
bool BcmNonVolSettings::IsDefault(NonVolSection section) const
{
    const char *pSection = "Dynamic";

    CallTrace("BcmNonVolSettings", "IsDefault");

    if (section == kPermanentSection)
    {
        pSection = "Permanent";
    }

    if (fIsDefault[section])
    {
        gAlwaysMsg(fMessageLogSettings, "IsDefault")
            << pSection << " settings are default!" << endl;
    }
    
    return fIsDefault[section];
}


// Returns the Is-Upgraded bool.  For a Composite object, it will check all
// of its contained settings groups and return false only if all of them
// return false.
//
// A group is upgraded when it reads non-vol settings that correspond to a
// version that is earlier than what is currently supported in the code.
//
// Parameters:
//      section - the section that is being queried.
//
// Returns:
//      true if this settings group was upgraded.
//      false if this settings group was not upgraded.
//
bool BcmNonVolSettings::IsUpgraded(NonVolSection section) const
{
    CallTrace("BcmNonVolSettings", "IsUpgraded");

    return fIsUpgraded[section];
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
bool BcmNonVolSettings::IsManufactured(NonVolSection section, BcmString &hint) const
{
    return true;
}


//  Returns the Is-Composite bool.
//
// Parameters:  None.
//
// Returns:
//      true if this is a Composite object.
//      false if this is not a Composite object.
//
bool BcmNonVolSettings::IsComposite(void) const
{
    CallTrace("BcmNonVolSettings", "IsComposite");

    return fIsComposite;
}


// For the base class, this does nothing.  The Composite derived class will
// override this and use it to delete any unrecognized settings groups that
// it contains.
//
// Parameters:  None.
//
// Returns:
//      true if unrecognized settings groups were flushed.
//      false if there were no unrecognized settings groups to flush.
//
bool BcmNonVolSettings::FlushUnrecognized(void)
{
    CallTrace("BcmNonVolSettings", "FlushUnrecognized");

    // Nothing to do here.
    return false;
}



// For the base class, this does nothing.  The Composite derived class will
// override this and use it to delete any non-critical dynamic data that
// it contains.
//
// Parameters:  None.
//
//
void BcmNonVolSettings::FlushNonVitalDynamicInformation(void)
{
    CallTrace("BcmNonVolSettings", "FlushNonVitalDynamicInformation");
}

// If there is any extra data stored for the specified section, then this
// deletes it.  PR925
//
// Parameters:
//      section - the section whose extra data is to be deleted.
//
// Returns:  Nothing.
//
void BcmNonVolSettings::FlushExtraData(NonVolSection section)
{
    CallTrace("BcmNonVolSettings", "FlushExtraData");

    delete pfExtraGroupData[section];
    pfExtraGroupData[section] = NULL;
    
    // Reset the version of the group to my native version, provided we know
    // what our native version is.
    if ((fNativeVersionMajor[section] != kNativeVersionUnknown) 
    &&  (fNativeVersionMinor[section] != kNativeVersionUnknown))
    {
        fVersionMajor[section] = fNativeVersionMajor[section];
        fVersionMinor[section] = fNativeVersionMinor[section];
    }
}


// This is the main entrypoint to get a settings object to write itself to
// a flat buffer, so that it can be sent to a NonVol device.  This method
// does the common header value setup required (storing the magic and version
// numbers).
//
// Then it calls WriteToImpl(), which gives the derived class a chance to
// append its data to the buffer.
//
// Parameters:
//      outputBuffer - the buffer into which the settings should be written.
//      section - the section that should be written.
//
// Returns:
//      true if successful.
//      false if there was a problem (usually the buffer is too small).
//
bool BcmNonVolSettings::WriteTo(BcmOctetBuffer &outputBuffer, NonVolSection section) const
{
    uint16 groupLength = 0;
    unsigned int offset = outputBuffer.BytesUsed();
    bool result;

    CallTrace("BcmNonVolSettings", "WriteTo");

    // Assume that the client who called us has pre-set (or cleared) the output
    // buffer, possibly with some header info, so we won't empty it.

    // Add the header elements to the buffer.  If any of those fail, then the
    // buffer is probably too small, so return failure.  Note that I don't know
    // the real length of the Group at this point, so I need to put a
    // placeholder in and the overwrite it later when the derived class returns.
    if (!outputBuffer.NetworkAddToEnd(groupLength) ||
        !outputBuffer.NetworkAddToEnd(fMagicNumber) ||
        !outputBuffer.NetworkAddToEnd(fVersionMajor[section]) ||
        !outputBuffer.NetworkAddToEnd(fVersionMinor[section]))
    {
        // Log this!
        gErrorMsg(fMessageLogSettings, "WriteTo") 
            << "Buffer too small to hold the Group Header!" << endl;

        // Unwind any changes.
        outputBuffer.SetBytesUsed(offset);

        result = false;
    }

    // Now give the derived class a chance to do its thing.
    else if (WriteToImpl(outputBuffer, section))
    {
        // Because it is possible for extra data to have been read but not
        // consumed (if the version that's running is earlier than the version
        // that wrote the data), we need to write this data back out.
        if ((pfExtraGroupData[section] != NULL) &&
            !outputBuffer.Append(*(pfExtraGroupData[section])))
        {
            gErrorMsg(fMessageLogSettings, "WriteTo") 
                << "Buffer too small to hold all of the Group data!  Failed to write 'extra' group data." << endl;

            result = false;
        }
        else
        {
            // Calculate the number of bytes used by this Group.
            unsigned int realGroupLength = outputBuffer.BytesUsed() - offset;

            // Make sure this isn't too big.
            if ((realGroupLength & 0xffff0000) != 0)
            {
                gErrorMsg(fMessageLogSettings, "WriteTo")
                    << "Group data is too long (can only be 64k bytes)!" << endl;

                result = false;
            }
            else
            {
                groupLength = (uint16) (realGroupLength & 0xffff);

                // Now overwrite the real Group Length.
                result = outputBuffer.NetworkOverwrite(offset, groupLength);
            }
        }
    }
    else
    {
        result = false;
    }

    if (result == false)
    {
        // Unwind any changes to the buffer.
        outputBuffer.SetBytesUsed(offset);
    }

    return result;
}


// This is the main entrypoint to get a settings object to read itself from
// a flat buffer, which was filled from some NonVol device.  This method
// does the common header stuff (reading the magic and version numbers).
//
// Then it calls ReadFromImpl(), which gives the derived class a chance to
// parse its data from the buffer.
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
bool BcmNonVolSettings::ReadFrom(const BcmOctetBuffer &inputBuffer,
                                 unsigned int &currentOffset, NonVolSection section)
{
    // If we got this far, then assume that the device has already done
    // verification of the checksum, length, etc., and that the input buffer
    // contains valid data.  I need to validate that the magic number is
    // correct.
    uint32 magicNumber;
    uint16 groupLength = 0, bytesNotConsumed;
    unsigned int originalOffset = currentOffset;

    CallTrace("BcmNonVolSettings", "ReadFrom");

    // Assume that we are no longer "upgraded".  This will be verified by the
    // derived class when it calls CheckForVersionUpgrade.
    fIsUpgraded[section] = false;

    // Read my header elements from the buffer.  If any of those fail, then the
    // buffer is probably too small, so return failure.
    if (!inputBuffer.NetworkRead(currentOffset, groupLength) ||
        !inputBuffer.NetworkRead(currentOffset, magicNumber) ||
        !inputBuffer.NetworkRead(currentOffset, fVersionMajor[section]) ||
        !inputBuffer.NetworkRead(currentOffset, fVersionMinor[section]))
    {
        // Log this!
        gErrorMsg(fMessageLogSettings, "ReadFrom") 
            << "Couldn't read the Group Header from the buffer!" << endl;

        // Unwind any changes.
        currentOffset = originalOffset;

        ResetDefaults(section);

        return false;
    }

    // Now validate the magic number.
    if (magicNumber != fMagicNumber)
    {
        gErrorMsg(fMessageLogSettings, "ReadFrom") 
            << "Magic Number isn't correct!  Expected " << (void *) fMagicNumber
            << "; read " << (void *) magicNumber << "..." << endl;

        // Unwind any changes.
        currentOffset = originalOffset;

        ResetDefaults(section);

        return false;
    }

    // Overlay an octet buffer on the data for the group, skipping the header.
    // This ensures that the derived class will not read beyond the end of the
    // group data.
    BcmOctetBuffer overlayBuffer(0);
    unsigned int overlayOffset = 0;

    overlayBuffer.Overlay(inputBuffer, currentOffset, groupLength - 8);

    // Now give the derived class a chance to do its thing.
    if (!ReadFromImpl(overlayBuffer, overlayOffset, section))
    {
        gErrorMsg(fMessageLogSettings, "ReadFrom")
            << GetInstanceName() << " failed to read all of its settings from the buffer!" << endl;
    
        // Unwind any changes.
        currentOffset = originalOffset;

        ResetDefaults(section);

        return false;
    }

    // Now adjust the offset to account for the number of bytes consumed by the
    // derived class.
    currentOffset += overlayOffset;

    // If all of that worked, then we need to do one final thing.  It is
    // possible that the settings were written by a later version of the code,
    // and that it added to the settings.  In order to preserve forward
    // compatibility, I need to store the settings in the group that were not
    // consumed by the derived class.  This will allow me to write them out
    // blindly next time.
    bytesNotConsumed = (uint16) (groupLength - currentOffset - originalOffset);
    if (bytesNotConsumed > 0)
    {
        delete pfExtraGroupData[section];
        pfExtraGroupData[section] = new BcmOctetBuffer(bytesNotConsumed);

        if (!pfExtraGroupData[section]->Append(inputBuffer, currentOffset, bytesNotConsumed))
        {
            gErrorMsg(fMessageLogSettings, "ReadFrom")
                << "Failed to copy unused bytes from Group Data!  Too many bytes." << endl;

            // Unwind any changes.
            currentOffset = originalOffset;

            delete pfExtraGroupData[section];
            pfExtraGroupData[section] = NULL;

            ResetDefaults(section);

            return false;
        }
    
        gInfoMsg(fMessageLogSettings, "ReadFrom")
            << "Derived class did not consume all of the bytes from the Group Data." << endl;

        if ((pgMessageLogStream != NULL) &&
            fMessageLogSettings.InfoMsgLogging())
        {
            pfExtraGroupData[section]->Print(*pgMessageLogStream, true);
        }
    }
    else
    {
        // If all bytes were consumed, then make sure we 'forget' any bytes that
        // were previously stored.
        delete pfExtraGroupData[section];
        pfExtraGroupData[section] = NULL;
    }

    // At this point, we're no longer defaulted.
    fIsDefault[section] = false;

    return true;
}


// Does formatted output of the settings to the specified ostream.
//
// Parameters:
//      outputStream - the ostream to be used.
//
// Returns:
//      The reference to the ostream that was passed in.
//
ostream & BcmNonVolSettings::Print(ostream &outputStream) const
{
    const char *pInstanceName = GetInstanceName();
    unsigned int nameLength, offset;
    char magicNumberString[5];
    BcmString Hint;

    CallTrace("BcmNonVolSettings", "Print");

    nameLength = strlen(pInstanceName) + 3;

    // Dump my settings...
    outputStream << "\n+" << setfill('-') << setw(nameLength) << "+" << "\n|"
                 << setfill(' ') << setw(nameLength) << "|" << "\r| "
                 << pInstanceName << "\n+"
                 << setfill('-') << setw(nameLength) << "+" << "\n" << setfill(' ')
                 << setw(30) << "Magic Number = " << (void *) fMagicNumber << " '";

    // Print the magic number in ASCII if possible.  Some of them are hex
    // representations of ASCII words.
    magicNumberString[0] = (char) ((fMagicNumber >> 24) & 0xff);
    magicNumberString[1] = (char) ((fMagicNumber >> 16) & 0xff);
    magicNumberString[2] = (char) ((fMagicNumber >> 8) & 0xff);
    magicNumberString[3] = (char) ((fMagicNumber) & 0xff);
    magicNumberString[4] = '\0';

    for (offset = 0; offset < 4; offset++)
    {
        if (!isprint(magicNumberString[offset]))
        {
            magicNumberString[offset] = '.';
        }
    }

    outputStream << magicNumberString << "'\n"
                 << setw(30) << "Version (Permanent) = " << (int) fVersionMajor[kPermanentSection] << "." << (int) fVersionMinor[kPermanentSection];
                 
    // Conditionally print native and previous versions
    if (((fNativeVersionMajor[kPermanentSection] != fVersionMajor[kPermanentSection]) && (fNativeVersionMajor[kPermanentSection] != kNativeVersionUnknown))
    ||  ((fNativeVersionMinor[kPermanentSection] != fVersionMinor[kPermanentSection]) && (fNativeVersionMinor[kPermanentSection] != kNativeVersionUnknown))
    ||   (fPreviousVersionMajor[kPermanentSection] != 0)
    ||   (fPreviousVersionMinor[kPermanentSection] != 0))
    {
        outputStream  << " (Native = " << (int) fNativeVersionMajor[kPermanentSection] << "." << (int) fNativeVersionMinor[kPermanentSection] 
                      << "; Previous = " << (int) fPreviousVersionMajor[kPermanentSection] << "." << (int) fPreviousVersionMinor[kPermanentSection] << ")" << endl;
    }
    else
    {
        outputStream << endl;
    }
        
    outputStream << setw(30) << "Version (Dynamic) = " << (int) fVersionMajor[kDynamicSection] << "." << (int) fVersionMinor[kDynamicSection];
    
    // Conditionally print native and previous versions
    if (((fNativeVersionMajor[kDynamicSection] != fVersionMajor[kDynamicSection]) && (fNativeVersionMajor[kDynamicSection] != kNativeVersionUnknown)) 
    ||  ((fNativeVersionMinor[kDynamicSection] != fVersionMinor[kDynamicSection]) && (fNativeVersionMinor[kDynamicSection] != kNativeVersionUnknown)) 
    ||   (fPreviousVersionMajor[kDynamicSection] != 0)
    ||   (fPreviousVersionMinor[kDynamicSection] != 0))
    {
        outputStream << " (Native = " << (int) fNativeVersionMajor[kDynamicSection] << "." << (int) fNativeVersionMinor[kDynamicSection]
                     << "; Previous = " << (int) fPreviousVersionMajor[kDynamicSection] << "." << (int) fPreviousVersionMinor[kDynamicSection] << ")" << endl;
    }
    else
    {
        outputStream << endl;
    }
                 
    outputStream << setw(30) << "Is Default (Permanent) = " << fIsDefault[kPermanentSection] << '\n'
                 << setw(30) << "Is Default (Dynamic) = " << fIsDefault[kDynamicSection] << '\n'
                 << endl;
    
    // Manufacturing state:             
    outputStream << setw(30) << "Is Manufactured (Permanent) = " << IsManufactured(kPermanentSection, Hint) << '\n';
    if (Hint.length())
    {
      outputStream << setw(30) << "(NOT SET: " << Hint << ")\n";
      Hint = "";
    }
    outputStream << setw(30) << "Is Manufactured (Dynamic) = " << IsManufactured(kDynamicSection, Hint) << '\n';
    if (Hint.length())
      outputStream << setw(30) << "(NOT SET: " << Hint << ")\n";

    // Now let the derived class do its thing.
    PrintImpl(outputStream);

    // If there is any extra group data stored, then print it.
    if (pfExtraGroupData[kPermanentSection] != NULL)
    {
        outputStream 
            << "\nExtra Permanent Settings data (stored because of downgrade):\n"
            << *pfExtraGroupData[kPermanentSection] << '\n';
    }
    if (pfExtraGroupData[kDynamicSection] != NULL)
    {
        outputStream 
            << "\nExtra Dynamic Settings data (stored because of downgrade):\n"
            << *pfExtraGroupData[kDynamicSection] << '\n';
    }

    return outputStream;
}


// This is a helper method that checks for differences between the currently
// supported version and the version that was read, displaying an
// appropriate message.  If the current version is newer, then it "upgrades"
// the version numbers that were read.
//
// Parameters:
//      currentVersionMajor - the major part of the version number that is
//                            currently supported.
//      currentVersionMinor - the minor part of the version number that is
//                            currently supported.
//      section - the section that is being checked.
//
// Returns:  Nothing.
//
void BcmNonVolSettings::CheckForVersionUpgrade(uint8 currentVersionMajor,
                                               uint8 currentVersionMinor,
                                               NonVolSection section)
{
    CallTrace("BcmNonVolSettings", "CheckForVersionUpgrade");

    // Assume that we aren't upgraded.
    fIsUpgraded[section] = false;
    
    // The currentVersion args here identify the version of this group
    // supported natively by THIS code version.  Remember them.
    fNativeVersionMajor[section] = currentVersionMajor;
    fNativeVersionMinor[section] = currentVersionMinor;

    // If we read an earlier version of the settings, then we just "upgraded"
    // it to our current version, so we need to change the version.  If we
    // read a later version, then we don't "downgrade" it, we just don't use
    // any extra settings.
    if ((fVersionMajor[section] < currentVersionMajor) ||
        ((fVersionMajor[section] == currentVersionMajor) &&
         (fVersionMinor[section] < currentVersionMinor)))
    {
        gWarningMsg(fMessageLogSettings, "ReadFromImpl")
            << "Read older version of the settings (" << (int) fVersionMajor[section]
            << "." << (int) fVersionMinor[section] << "); they have been upgraded to version "
            << (int) currentVersionMajor << "."
            << (int) currentVersionMinor << ", preserving original settings." << endl;

        fPreviousVersionMajor[section] = fVersionMajor[section];
        fPreviousVersionMinor[section] = fVersionMinor[section];

        fVersionMajor[section] = currentVersionMajor;
        fVersionMinor[section] = currentVersionMinor;

        fIsUpgraded[section] = true;
    }
    else if ((fVersionMajor[section] > currentVersionMajor) ||
             ((fVersionMajor[section] == currentVersionMajor) &&
              (fVersionMinor[section] > currentVersionMinor)))
    {
        gWarningMsg(fMessageLogSettings, "ReadFromImpl")
            << "Read newer version of the settings (" << (int) fVersionMajor[section]
            << "." << (int) fVersionMinor[section] << "); current version is "
            << (int) currentVersionMajor << "."
            << (int) currentVersionMinor << ", newer settings will be preserved, but will not be used." << endl;
    }
}


// Helper function that check to see if the version that was read (stored
// in fVersionMajor and fVersionMinor) is greater than or equal to the
// specified version; for example, this method returns true if:
//
//      fVersionMajor.fVersionMinor >= versionMajor.versionMinor
//
// Parameters:
//      versionMajor - the version to be compared against.
//      versionMinor - the version to be compared against.
//      section - the section whose version number should be compared.
//
// Returns:
//      true if the current version is greater than or equal to the
//          specified verison.
//      false if the current version is less.
//
bool BcmNonVolSettings::MyVersionIsGreaterThanOrEqualTo(uint8 versionMajor,
                                                        uint8 versionMinor,
                                                        NonVolSection section) const
{
    CallTrace("BcmNonVolSettings", "MyVersionIsGreaterThanOrEqualTo");

    // Handle the case where the major version number is bigger.  In this case,
    // we ignore the minor version number.
    //
    // For example:
    //    2.0 >= 1.5
    //
    if (fVersionMajor[section] > versionMajor)
    {
        return true;
    }

    // Handle the case where the major versions are equal; in this case, the
    // minor version is the deciding factor.
    //
    // For example:
    //      1.5 >= 1.3
    //
    if ((fVersionMajor[section] == versionMajor) &&
        (fVersionMinor[section] >= versionMinor))
    {
        return true;
    }

    // Otherwise, either the major version is less, or it is equal and the minor
    // version is less.
    //
    // For example:
    //      1.3 < 1.5
    //      0.9 < 1.5
    //
    return false;
}


// Helper function that check to see if the version that was read (stored
// in fVersionMajor and fVersionMinor) is less than or equal to the
// specified version; for example, this method returns true if:
//
//      fVersionMajor.fVersionMinor <= versionMajor.versionMinor
//
// Parameters:
//      versionMajor - the version to be compared against.
//      versionMinor - the version to be compared against.
//      section - the section whose version number should be compared.
//
// Returns:
//      true if the current version is less than or equal to the
//          specified verison.
//      false if the current version is greater.
//
bool BcmNonVolSettings::MyVersionIsLessThanOrEqualTo(uint8 versionMajor,
                                                     uint8 versionMinor,
                                                     NonVolSection section) const
{
    CallTrace("BcmNonVolSettings", "MyVersionIsLessThanOrEqualTo");

    // Handle the case where the major version number is bigger.  In this case,
    // we ignore the minor version number.
    //
    // For example:
    //    1.5 <= 2.0
    //
    if (fVersionMajor[section] < versionMajor)
    {
        return true;
    }

    // Handle the case where the major versions are equal; in this case, the
    // minor version is the deciding factor.
    //
    // For example:
    //      1.3 <= 1.5
    //
    if ((fVersionMajor[section] == versionMajor) &&
        (fVersionMinor[section] <= versionMinor))
    {
        return true;
    }

    // Otherwise, either the major version is less, or it is equal and the minor
    // version is less.
    //
    // For example:
    //      2.3 > 1.5
    //      3.9 > 1.5
    //
    return false;
}


// Helper function that check to see if the previous version that 
// was read (stored in fVersionMajor and fVersionMinor) is less 
// than or equal to the specified version; for example, this 
// method returns true if:
//
//   fPreviousVersionMajor.fVersionMinor <= versionMajor.versionMinor
//
// Parameters:
//      versionMajor - the version to be compared against.
//      versionMinor - the version to be compared against.
//      section - the section whose version number should be compared.
//
// Returns:
//      true if the previous version is less than or equal to the
//          specified verison.
//      false if the previous version is greater.
//
bool BcmNonVolSettings::MyPrevVersionIsLessThanOrEqualTo(uint8 versionMajor,
                                                         uint8 versionMinor,
                                                         NonVolSection section) const
{
    CallTrace("BcmNonVolSettings", "MyPrevVersionIsLessThanOrEqualTo");

    // Handle the case where the major version number is bigger.  In this case,
    // we ignore the minor version number.
    //
    // For example:
    //    1.5 <= 2.0
    //
    if (fPreviousVersionMajor[section] < versionMajor)
    {
        return true;
    }

    // Handle the case where the major versions are equal; in this case, the
    // minor version is the deciding factor.
    //
    // For example:
    //      1.3 <= 1.5
    //
    if ((fPreviousVersionMajor[section] == versionMajor) &&
        (fPreviousVersionMinor[section] <= versionMinor))
    {
        return true;
    }

    // Otherwise, either the major version is less, or it is equal and the minor
    // version is less.
    //
    // For example:
    //      2.3 > 1.5
    //      3.9 > 1.5
    //
    return false;
}


// Helper function that is used to detect if we had a recent upgrade. This
// is performed if the previous major and minor values are zero, if they
// are we did not have an upgrade since they would never get updated.
//
// Parameters:
//      None
//
// Returns:
//      true if we detected an upgrade
//      false if we did not detect and upgrade
//
bool BcmNonVolSettings::DetectUpgrade(NonVolSection section) const
{
    CallTrace("BcmNonVolSettings", "DetectUpgrade");

    if ( (fPreviousVersionMajor[section] == 0) && (fPreviousVersionMinor[section] == 0) )
    {
        return false;
    }

    return true;
}


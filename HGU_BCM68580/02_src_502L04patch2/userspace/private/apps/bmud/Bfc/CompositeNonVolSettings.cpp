//****************************************************************************
//
//  Copyright (c) 2000  Broadcom Corporation
//  All Rights Reserved
//  No portions of this material may be reproduced in any form without the
//  written permission of:
//          Broadcom Corporation
//          16215 Alton Parkway
//          Irvine, California 92619
//  All information contained in this document is Broadcom Corporation
//  company private, proprietary, and trade secret.
//
//****************************************************************************
//  $Id$
//
//  Filename:       CompositeNonVolSettings.cpp
//  Author:         David Pullen
//  Creation Date:  Nov 1, 2000
//
//****************************************************************************
//  Description:
//      This is a specialization of the NonVolSettings base class to handle
//      the usage pattern called "Compositing".  Its purpose is to store a list
//      of settings objects, and to iterate over that list when any of the
//      base class public methods are called.  The client doesn't need to know
//      that this is a composite class.
//
//      Generally, an application will derive a child from this class in order
//      to group all of the settings it needs together in a single object.
//
//      Note that it is perfectly legal, and may be useful, for a composite
//      class to be contained in another composite, but for this particular
//      class, it will usually be more desirable to have just a single composite
//      as the parent.
//
//      The composite class generally will not have any setting data of its own.
//
//****************************************************************************

//********************** Include Files ***************************************

// My api and definitions...
#include "CompositeNonVolSettings.h"

#include "UnrecognizedNonVolSettings.h"

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
BcmCompositeNonVolSettings::BcmCompositeNonVolSettings(const char *pInstanceName) :
    BcmNonVolSettings(pInstanceName)
{
    CallTrace("BcmCompositeNonVolSettings", "BcmCompositeNonVolSettings");

    fMessageLogSettings.SetModuleName("BcmCompositeNonVolSettings");

    fIsComposite = true;
}


// Destructor.  Frees up any memory/objects allocated, cleans up internal
// state.
//
// Parameters:  N/A
//
// Returns:  N/A
//
BcmCompositeNonVolSettings::~BcmCompositeNonVolSettings()
{
    CallTrace("BcmCompositeNonVolSettings", "~BcmCompositeNonVolSettings");

    // Get rid of anything that's in my STL containers.
    while (!fSettingsList.empty())
    {
        delete *(fSettingsList.begin());

        fSettingsList.erase(fSettingsList.begin());
    }
}


// Adds the specified settings group to the Composite.  This means that it
// will be iterated over and operated on.
//
// This object becomes the owner of the object, and will delete it when
// the composite object is deleted.
//
// Parameters:
//      pSettings - pointer to the settings group object to be added.
//
// Returns:
//      true if successful.
//      false if there was a problem - usually because of duplicate magic
//          number.
//
bool BcmCompositeNonVolSettings::AddSettingsGroup(BcmNonVolSettings *pSettings)
{
    vector<BcmNonVolSettings *>::const_iterator myIterator;

    CallTrace("BcmCompositeNonVolSettings", "AddSettingsGroup");

    // Make sure the magic number is unique.
    myIterator = fSettingsList.begin();
    while (myIterator < fSettingsList.end())
    {
        if (pSettings->GetMagicNumber() == (*myIterator)->GetMagicNumber())
        {
            gErrorMsg(fMessageLogSettings, "AddSettingsGroup")
                << "Duplicate magic number detected!  Can't add settings object!  " 
                << (void *) pSettings->GetMagicNumber() << endl;

            return false;
        }

        myIterator++;
    }

    // Set myself as the parent, which allows the recursive linkage to be
    // established (when one of the contained settings is written, then entire
    // composite gets written).
    pSettings->SetParent(this);

    // Add the object to the end of the list.
    fSettingsList.push_back(pSettings);

    return true;
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
const BcmNonVolSettings *BcmCompositeNonVolSettings::Find(uint32 magicNumber) const
{
    vector<BcmNonVolSettings *>::const_iterator myIterator;

    CallTrace("BcmCompositeNonVolSettings", "Find");

    // Search for an object with the specified magic number.
    myIterator = fSettingsList.begin();
    while (myIterator < fSettingsList.end())
    {
        // If this one is a composite, recurse into it.
        if ((*myIterator)->IsComposite())
        {
            const BcmNonVolSettings *pSettings;

            pSettings = (*myIterator)->Find(magicNumber);

            if (pSettings != NULL)
            {
                return pSettings;
            }
        }

        // If this one matches, return it.
        else if (magicNumber == (*myIterator)->GetMagicNumber())
        {
            return (*myIterator);
        }

        myIterator++;
    }

    // None were found, return NULL.
    return NULL;
}


// Returns the Is-Default bool.  For a Composite object, it will check all
// of its contained settings groups and return false only if all of them
// return false.
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
bool BcmCompositeNonVolSettings::IsDefault(NonVolSection section) const
{
    vector<BcmNonVolSettings *>::const_iterator myIterator;
    bool result = false;

    CallTrace("BcmCompositeNonVolSettings", "IsDefault");

    // Iterate over my containers, calling each contained object one at a time.
    myIterator = fSettingsList.begin();
    while (myIterator < fSettingsList.end())
    {
        // See if this object is default.
        if ((*myIterator)->IsDefault(section))
        {
            result = true;
        }

        myIterator++;
    }

    return result;
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
bool BcmCompositeNonVolSettings::IsUpgraded(NonVolSection section) const
{
    vector<BcmNonVolSettings *>::const_iterator myIterator;

    CallTrace("BcmCompositeNonVolSettings", "IsUpgraded");

    // Iterate over my containers, calling each contained object one at a time.
    myIterator = fSettingsList.begin();
    while (myIterator < fSettingsList.end())
    {
        // See if this object is default.  If so, then I can stop.
        if ((*myIterator)->IsUpgraded(section))
        {
            return true;
        }

        myIterator++;
    }

    // If we checked all of them, then they aren't default.
    return false;
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
bool BcmCompositeNonVolSettings::IsManufactured(NonVolSection section, BcmString &hint) const
{
    vector<BcmNonVolSettings *>::const_iterator myIterator;
    BcmString myHint;
    bool Manufactured = true;

    CallTrace("BcmCompositeNonVolSettings", "IsManufactured");

    // Iterate over my containers, calling each contained object one at a time.
    myIterator = fSettingsList.begin();
    while (myIterator < fSettingsList.end())
    {
        // See if this object is manufactured.  If not, then I can stop.
        myHint = "";
        if ((*myIterator)->IsManufactured(section, myHint) == false)
            Manufactured = false;
            
        if ((myHint.length() != 0) && (hint.length() != 0))
          hint += ", ";
            
        hint += myHint;

        myIterator++;
    }

    // If we checked all of them, then they are manufactured.
    return Manufactured;
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
bool BcmCompositeNonVolSettings::FlushUnrecognized(void)
{
    vector<BcmNonVolSettings *>::iterator myIterator;
    bool anyDeleted = false;

    CallTrace("BcmCompositeNonVolSettings", "FlushUnrecognized");

    // Iterate over my containers, calling each contained object one at a time.
    myIterator = fSettingsList.begin();
    while (myIterator < fSettingsList.end())
    {
        // See if this object is unrecognized.  If so, then get rid of it.
        if ((*myIterator)->IsRecognized() == false)
        {
            // Erase the item.  Note that this is a vector, so I don't need to
            // increment the iterator.
            delete (*myIterator);
            fSettingsList.erase(myIterator);

            anyDeleted = true;
        }
        else
        {
            // If this item is a Composite, then I should recurse into it.
            if ((*myIterator)->IsComposite() &&
                (*myIterator)->FlushUnrecognized())
            {
                anyDeleted = true;
            }

            // I only increment the iterator if I didn't erase the item.
            myIterator++;
        }
    }

    return anyDeleted;
}


// Iterates over all contained objects, and calls FlushExtraData() for each
// of them.  PR925
//
// Parameters:
//      section - the section whose extra data is to be deleted.
//
// Returns:  Nothing.
//
void BcmCompositeNonVolSettings::FlushExtraData(NonVolSection section)
{
    vector<BcmNonVolSettings *>::iterator myIterator;

    CallTrace("BcmCompositeNonVolSettings", "FlushExtraData");

    // Iterate over my containers, calling each contained object one at a time.
    myIterator = fSettingsList.begin();
    while (myIterator < fSettingsList.end())
    {
        // See if this object is recognized.  If not, skip it (unrecognized
        // objects store all of their data as "extra", so you must use the
        // FlushUnrecognized to do this).
        // If this item is a Composite, then I should recurse into it.
        if (((*myIterator)->IsRecognized() == true) ||
            (*myIterator)->IsComposite())
        {
            (*myIterator)->FlushExtraData(section);
        }
    
        // Go to the next one.
        ++myIterator;
    }
}


// Allows the derived class to set its values to a known good state.  This
// MUST be implemented by the derived class.
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
void BcmCompositeNonVolSettings::ResetDefaults(NonVolSection section)
{
    vector<BcmNonVolSettings *>::iterator myIterator;

    CallTrace("BcmCompositeNonVolSettings", "ResetDefaults");

    // Iterate over my containers, calling each contained object one at a time.
    myIterator = fSettingsList.begin();
    while (myIterator < fSettingsList.end())
    {
        (*myIterator)->ResetDefaults(section);
        myIterator++;
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
bool BcmCompositeNonVolSettings::WriteTo(BcmOctetBuffer &outputBuffer, 
                                         NonVolSection section) const
{
    vector<BcmNonVolSettings *>::const_iterator myIterator;

    CallTrace("BcmCompositeNonVolSettings", "WriteTo");

    // Iterate over my containers, calling each contained object one at a time.
    //
    // If there was a failure along the way, should I bail immediately and
    // return false, or should I continue?  For now, I'll bail immediately.
    myIterator = fSettingsList.begin();
    while (myIterator < fSettingsList.end())
    {
        if ((*myIterator)->WriteTo(outputBuffer, section) == false)
        {
            return false;
        }

        myIterator++;
    }

    return true;
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
bool BcmCompositeNonVolSettings::ReadFrom(const BcmOctetBuffer &inputBuffer,
                                          unsigned int &currentOffset, 
                                          NonVolSection section)
{
    vector<BcmNonVolSettings *>::iterator myIterator;
    uint32 magicNumber = 0;
    uint16 groupLength;
    unsigned int myOffset;
    BcmOctetBuffer overlayBuffer(0);
    unsigned int overlayOffset;
    bool processed;

    CallTrace("BcmCompositeNonVolSettings", "ReadFrom");

    // UNFINISHED - what if one of the groups was not present?  I may need to
    // do something special for that group (calculate defaults from some other
    // value and write to the device).

    // Iterate over my containers, calling each contained object one at a time.
    // This is a bit trickier than the other methods; the ordering of the
    // groups is not guaranteed, and can even change from one write to the next
    // read, so rather than doing just a regular iteration, I need to try each
    // one until there is success.
    //
    // If there was a failure along the way, should I bail immediately and
    // return false, or should I continue?  If all of the known settings objects
    // report a failure, then I need to check all of the "unknown" settings
    // objects.  If none of them wants to claim it, then I need to create a new
    // unknown settings object and store the data with it so that it isn't lost.
    // We don't report a failure unless there is something critical (not enough
    // bytes in the buffer).
    //
    // Keep going until we have processed all of the groups in the buffer.
    while (currentOffset < inputBuffer.BytesUsed())
    {
        // Read part of the group header so that I can see the size (for
        // overlaying the buffer) and the magic number (for finding the object
        // that is responsible for it).
        myOffset = currentOffset;
        if (!inputBuffer.NetworkRead(myOffset, groupLength) ||
            !inputBuffer.NetworkRead(myOffset, magicNumber))
        {
            // Log this!
            gErrorMsg(fMessageLogSettings, "ReadFrom") 
                << "Couldn't read the Group Header from the buffer!" << endl;

            // Unwind any changes?
            //currentOffset = originalOffset;

            //ResetDefaults(section);

            return false;
        }

        // Do a quick sanity check on the length to make sure there are enough
        // bytes.
        if (groupLength > (inputBuffer.BytesUsed() - currentOffset))
        {
            // Log this!
            gErrorMsg(fMessageLogSettings, "ReadFrom") 
                << "The Group Length is bigger than the number of bytes in the buffer!" << endl;

            // Unwind any changes?
            //currentOffset = originalOffset;

            //ResetDefaults(section);

            return false;
        }

        // Overlay a new buffer on the input buffer so that only the bytes for
        // the group are visible to the object that I call.
        overlayBuffer.Overlay(inputBuffer, currentOffset, groupLength);
        overlayOffset = 0;

        // Find the object that has a matching magic number.
        processed = false;
        myIterator = fSettingsList.begin();
        while (myIterator < fSettingsList.end())
        {
            // If the magic number matches, then make it read the settings from
            // the group.
            if ((*myIterator)->GetMagicNumber() == magicNumber)
            {
                // If this fails, then what?  The CRC check on the buffer worked,
                // so it can't be corruption.  One possibility is that someone
                // else chose the same magic number, and the data was formatted
                // incorrectly.  Another is that there was a resource failure.
                // Any way you slice it, we're kinda stuck.  I guess we should
                // return a failure and let some higher layer thing deal with it.
                // Should we unwind changes and reset to defaults?  Dunno...
                if ((*myIterator)->ReadFrom(overlayBuffer, overlayOffset, section) == false)
                {
                    // Log this!
                    gErrorMsg(fMessageLogSettings, "ReadFrom") 
                        << "A contained Settings object failed to parse the Group settings!" << endl;

                    // Unwind any changes?
                    //currentOffset = originalOffset;

                    // Reset this group to defaults to undo any partial parsing
                    // that was done.
                    (*myIterator)->ResetDefaults(section);

                    // Continue anyway, so that we don't kill all groups that
                    // follow this one.
                    //return false;
                }

                processed = true;
                break;
            }

            myIterator++;
        }

        // If nobody claimed the group, then I need to create a new object that
        // will store it.
        if (!processed)
        {
            BcmUnrecognizedNonVolSettings *pSettings = new BcmUnrecognizedNonVolSettings("Unrecognized NonVol Settings Group");

            if (pSettings->ReadFrom(overlayBuffer, overlayOffset, section) == false)
            {
                // Log this!
                gErrorMsg(fMessageLogSettings, "ReadFrom") 
                    << "Failed to store unrecognized NonVol Settings Group!" << endl;

                delete pSettings;

                // Unwind any changes?
                //currentOffset = originalOffset;

                //ResetDefaults(section);

                return false;
            }
            else
            {
                char magicNumberString[5];
                unsigned int offset;

                // Print the magic number in ASCII if possible.  Some of them are hex
                // representations of ASCII words.
                magicNumberString[0] = (char) ((magicNumber >> 24) & 0xff);
                magicNumberString[1] = (char) ((magicNumber >> 16) & 0xff);
                magicNumberString[2] = (char) ((magicNumber >> 8) & 0xff);
                magicNumberString[3] = (char) ((magicNumber) & 0xff);
                magicNumberString[4] = '\0';

                for (offset = 0; offset < 4; offset++)
                {
                    if (!isprint(magicNumberString[offset]))
                    {
                        magicNumberString[offset] = '.';
                    }
                }

                gWarningMsg(fMessageLogSettings, "ReadFrom")
                    << "Read an unrecognized settings group from the buffer (magic number " 
                    << (void *) magicNumber << " '" << magicNumberString 
                    << "'); storing in raw form for compatibility..." << endl;
            }

            fSettingsList.push_back(pSettings);
        }

        // Adjust the current offset to skip over the group.
        currentOffset += groupLength;
    }

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
ostream & BcmCompositeNonVolSettings::Print(ostream &outputStream) const
{
    vector<BcmNonVolSettings *>::const_iterator myIterator;

    CallTrace("BcmCompositeNonVolSettings", "Print");

    // Iterate over my containers, calling each contained object one at a time.
    //
    // If there was a failure along the way, should I bail immediately and
    // return false, or should I continue?  For now, I'll bail immediately.
    myIterator = fSettingsList.begin();
    while (myIterator < fSettingsList.end())
    {
        (*myIterator)->Print(outputStream);

        myIterator++;
    }

    return outputStream;
}


// This is where the derived class writes its members to the buffer in a
// flat format.  It is up to the derived class to determine what order,
// what data size, whether to use network or host byte order, etc.
//
// This MUST be implemented by the derived class.
//
// Parameters:
//      outputBuffer - the buffer into which the settings should be written.
//      section - the section that should be written.
//
// Returns:
//      true if successful.
//      false if there was a problem (usually the buffer is too small).
//
bool BcmCompositeNonVolSettings::WriteToImpl(BcmOctetBuffer & /*outputBuffer*/, 
                                             NonVolSection /*section*/) const
{
    CallTrace("BcmCompositeNonVolSettings", "WriteToImpl");

    // Log this as an error!  This method should never be called for this class!
    gErrorMsg(fMessageLogSettings, "WriteToImpl")
        << "This method should never be called for BcmCompositeNonVolSettings!  This is a programming error!" << endl;

    return false;
}


// This is where the derived class reads its members from the buffer in a
// flat format.  It is up to the derived class to determine what ordering,
// etc., should be used.
//
// The currentOffset parameter tells the derived class where the app data
// starts in the buffer.  It can be used directly in the calls to Read() or
// NetworkRead() in BcmOctetBuffer (which expects an offset value).
//
// This MUST be implemented by the derived class.
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
bool BcmCompositeNonVolSettings::ReadFromImpl(const BcmOctetBuffer & /*inputBuffer*/, 
                                              unsigned int & /*currentOffset*/, 
                                              NonVolSection /*section*/)
{
    CallTrace("BcmCompositeNonVolSettings", "ReadFromImpl");

    // Log this as an error!  This method should never be called for this class!
    gErrorMsg(fMessageLogSettings, "ReadFromImpl")
        << "This method should never be called for BcmCompositeNonVolSettings!  This is a programming error!" << endl;

    return false;
}


// Allows the derived class to do formatted output of its settings to the
// specified ostream, after the base class does its stuff.
//
// Parameters:
//      outputStream - the ostream to be used.
//
// Returns:
//      The reference to the ostream that was passed in.
//
ostream & BcmCompositeNonVolSettings::PrintImpl(ostream & outputStream) const
{
    CallTrace("BcmCompositeNonVolSettings", "PrintImpl");

    #if !(BCM_REDUCED_IMAGE_SIZE)

    // Log this as an error!  This method should never be called for this class!
    gErrorMsg(fMessageLogSettings, "PrintImpl")
        << "This method should never be called for BcmCompositeNonVolSettings!  This is a programming error!" << endl;

    #endif
    
    return outputStream;
}




// For the base class, this does nothing.  The Composite derived class will
// override this and use it to delete any non-critical dynamic data that
// it contains.  
// It is a fail safe, such that if the dynamic nonvol evers gets
// full, we can flush out non-critical dynamic information (such as event logs) and
// save some space.

//
// Parameters:  None.
//
//
void BcmCompositeNonVolSettings::FlushNonVitalDynamicInformation(void)
{
    vector<BcmNonVolSettings *>::iterator myIterator;

    CallTrace("BcmCompositeNonVolSettings", "FlushNonVitalDynamicInformation");


    // Iterate over my containers, calling each contained object one at a time.
    myIterator = fSettingsList.begin();
    while (myIterator < fSettingsList.end())
    {
        (*myIterator)->FlushNonVitalDynamicInformation();
        myIterator++;

    }
}

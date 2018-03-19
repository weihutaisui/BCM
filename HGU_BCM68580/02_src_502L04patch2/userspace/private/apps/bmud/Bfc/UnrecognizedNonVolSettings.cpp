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
//  Filename:       UnrecognizedNonVolSettings.cpp
//  Author:         David Pullen
//  Creation Date:  Nov 1, 2000
//
//****************************************************************************
//  Description:
//      This is a specialization of the base class whose sole purpose is to
//      store the group settings data that is not recognized by other known
//      settings objects.  This allows us to preserve groups of data that were
//      written by other applications, so that their settings will still be
//      there when they run again.
//
//****************************************************************************

//********************** Include Files ***************************************

// My api and definitions...
#include "UnrecognizedNonVolSettings.h"

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
BcmUnrecognizedNonVolSettings::BcmUnrecognizedNonVolSettings(const char *pInstanceName) :
    BcmNonVolSettings(pInstanceName)
{
    CallTrace("BcmUnrecognizedNonVolSettings", "BcmUnrecognizedNonVolSettings");

    fMessageLogSettings.SetModuleName("BcmUnrecognizedNonVolSettings");

    // Prime this to false so that we will read data from a buffer, but won't
    // write anything until something is read.
    fIsInitialized = false;

    // Also make note that we are unrecognized.
    fIsRecognized = false;
}


// Destructor.  Frees up any memory/objects allocated, cleans up internal
// state.
//
// Parameters:  N/A
//
// Returns:  N/A
//
BcmUnrecognizedNonVolSettings::~BcmUnrecognizedNonVolSettings()
{
    CallTrace("BcmUnrecognizedNonVolSettings", "~BcmUnrecognizedNonVolSettings");

    // Nothing to do.
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
bool BcmUnrecognizedNonVolSettings::IsDefault(NonVolSection section) const
{
    CallTrace("BcmUnrecognizedNonVolSettings", "IsDefault");

    // Never claim to be defaulted for any section.
    return false;
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
void BcmUnrecognizedNonVolSettings::ResetDefaults(NonVolSection section)
{
    CallTrace("BcmUnrecognizedNonVolSettings", "ResetDefaults");

    // Do I need to do anything?  Not sure yet...

    // At this point, if we are resetting an unrecognized setting, then we
    // should get rid of the data.  This is because we don't return false for
    // any reason during Read() or Write(), so it would only be called if the
    // Section had a CRC failure or some other critical failure, so we probably
    // want to get rid of this errant data.
    delete pfExtraGroupData[section];
    pfExtraGroupData[section] = NULL;
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
bool BcmUnrecognizedNonVolSettings::WriteTo(BcmOctetBuffer &outputBuffer, 
                                            NonVolSection section) const
{
    CallTrace("BcmUnrecognizedNonVolSettings", "WriteTo");

    // If I have not been initialized with settings data for this section, then
    // I don't do anything.
    if ((fIsInitialized == false) || (pfExtraGroupData[section] == NULL))
    {
        return true;
    }

    // Otherwise, I let the base class do the real work.
    return BcmNonVolSettings::WriteTo(outputBuffer, section);
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
bool BcmUnrecognizedNonVolSettings::ReadFrom(const BcmOctetBuffer &inputBuffer,
                                             unsigned int &currentOffset, 
                                             NonVolSection section)
{
    CallTrace("BcmUnrecognizedNonVolSettings", "ReadFrom");

    // If both sections have not been initialized, then use the opportunity to
    // read and store the magic number.  This allows me to become initialized.
    // Once initialized, I will only accept data for this magic number, and will
    // write the data back out.
    if (fIsInitialized == false)
    {
        // The magic number is past the group size field.
        unsigned int offset = sizeof(uint16);

        fIsInitialized = inputBuffer.NetworkRead(offset, fMagicNumber);
    }

    // Either way, once we get here, we need to let the base class do the work.
    // It will either accept or reject the group based on my magic number.
    return BcmNonVolSettings::ReadFrom(inputBuffer, currentOffset, section);
}


// Does formatted output of the settings to the specified ostream.
//
// Parameters:
//      outputStream - the ostream to be used.
//
// Returns:
//      The reference to the ostream that was passed in.
//
ostream & BcmUnrecognizedNonVolSettings::Print(ostream &outputStream) const
{
    CallTrace("BcmUnrecognizedNonVolSettings", "Print");

    // If I have not been initialized, then I need to be quiet, not printing
    // anything.
    if (fIsInitialized == false)
    {
        return outputStream;
    }

    // Otherwise, let the base class do its thing.
    return BcmNonVolSettings::Print(outputStream);
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
bool BcmUnrecognizedNonVolSettings::WriteToImpl(BcmOctetBuffer & /*outputBuffer*/, 
                                                NonVolSection /*section*/) const
{
    CallTrace("BcmUnrecognizedNonVolSettings", "WriteToImpl");

    // I don't need to do anything.  All of the data is considered "extra"
    // since I don't recognize it.  Let the base class handle this.
    return true;
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
bool BcmUnrecognizedNonVolSettings::ReadFromImpl(const BcmOctetBuffer & /*inputBuffer*/, 
                                                 unsigned int & /*currentOffset*/, 
                                                 NonVolSection /*section*/)
{
    CallTrace("BcmUnrecognizedNonVolSettings", "ReadFromImpl");

    // I don't need to do anything.  All of the data is considered "extra"
    // since I don't recognize it.  Let the base class handle this.
    return true;
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
ostream & BcmUnrecognizedNonVolSettings::PrintImpl(ostream & outputStream) const
{
    CallTrace("BcmUnrecognizedNonVolSettings", "PrintImpl");

    // Nothing to do...since the unrecognized data is stored as "extra data" by
    // the base class, and the extra data is printed by the base class, then
    // we don't need to do it.
    return outputStream;
}



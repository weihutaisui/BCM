//****************************************************************************
//
//  Copyright (c) 2000-2003  Broadcom Corporation
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
//  Filename:       UnrecognizedNonVolSettings.h
//  Author:         David Pullen
//  Creation Date:  Nov 1, 2000
//
//****************************************************************************

#ifndef UNRECOGNIZEDNONVOLSETTINGS_H
#define UNRECOGNIZEDNONVOLSETTINGS_H

//********************** Include Files ***************************************

// My base class.
#include "NonVolSettings.h"

//********************** Global Types ****************************************

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Class Declaration ***********************************

/** \ingroup BfcNonVol
*
*      This is a specialization of the base class whose sole purpose is to
*      store the group settings data that is not recognized by other known
*      settings objects.  This allows us to preserve groups of data that were
*      written by other applications, so that their settings will still be
*      there when they run again.
*/
class BcmUnrecognizedNonVolSettings : public BcmNonVolSettings
{
public:

    /// Default Constructor.  Initializes the state of the object, setting the
    /// magic and version numbers to 0.
    ///
    /// \param
    ///      pInstanceName - the name to be used with the message log settings.
    ///
    BcmUnrecognizedNonVolSettings(const char *pInstanceName = NULL);

    /// Destructor.  Frees up any memory/objects allocated, cleans up internal
    /// state.
    ///
    virtual ~BcmUnrecognizedNonVolSettings();

    /// Returns the Is-Default bool.  For a Composite object, it will check all
    /// of its contained settings groups and return false only if all of them
    /// return false.
    ///
    /// \param
    ///      section - the section that is being queried.
    ///
    /// \retval
    ///      true if this settings group contains only default data (and is not
    ///          really valid).
    /// \retval
    ///      false if this settings group contains valid data read from the
    ///          device.
    ///
    virtual bool IsDefault(NonVolSection section) const;

    /// Allows the derived class to set its values to a known good state.  This
    /// MUST be implemented by the derived class.
    ///
    /// Note that the derived class should set the version numbers to the desired
    /// values in case they were changed (by ReadFrom(), etc).
    ///
    /// This method may be called if ReadFrom() returns false in order to ensure
    /// that there are valid settings in the object.
    ///
    /// \param
    ///      section - the section that should be reset.
    ///
    virtual void ResetDefaults(NonVolSection section);

    /// This is the main entrypoint to get a settings object to write itself to
    /// a flat buffer, so that it can be sent to a NonVol device.  This method
    /// does the common header value setup required (storing the magic and version
    /// numbers).
    ///
    /// Then it calls WriteToImpl(), which gives the derived class a chance to
    /// append its data to the buffer.
    ///
    /// \param
    ///      outputBuffer - the buffer into which the settings should be written.
    /// \param
    ///      section - the section that should be written.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem (usually the buffer is too small).
    ///
    virtual bool WriteTo(BcmOctetBuffer &outputBuffer, NonVolSection section) const;

    /// This is the main entrypoint to get a settings object to read itself from
    /// a flat buffer, which was filled from some NonVol device.  This method
    /// does the common header stuff (reading the magic and version numbers).
    ///
    /// Then it calls ReadFromImpl(), which gives the derived class a chance to
    /// parse its data from the buffer.
    ///
    /// The currentOffset parameter tells the derived class where the app data
    /// starts in the buffer.  It can be used directly in the calls to Read() or
    /// NetworkRead() in BcmOctetBuffer (which expects an offset value).
    ///
    /// \param
    ///      inputBuffer - the buffer from which the settings should be read.
    /// \param
    ///      currentOffset - the offset into the buffer from which the settings
    ///                      should be read.  This is necessary to allow the
    ///                      derived class to skip the header.
    /// \param
    ///      section - the section that should be read.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem (version number not supported, not
    ///          enough bytes in the buffer, etc.).
    ///
    virtual bool ReadFrom(const BcmOctetBuffer &inputBuffer,
                          unsigned int &currentOffset, NonVolSection section);

    /// Does formatted output of the settings to the specified ostream.
    ///
    /// \param
    ///      outputStream - the ostream to be used.
    ///
    /// \return
    ///      The reference to the ostream that was passed in.
    ///
    virtual ostream & Print(ostream &outputStream) const;

protected:

    /// This is where the derived class writes its members to the buffer in a
    /// flat format.  It is up to the derived class to determine what order,
    /// what data size, whether to use network or host byte order, etc.
    ///
    /// This MUST be implemented by the derived class.
    ///
    /// \param
    ///      outputBuffer - the buffer into which the settings should be written.
    /// \param
    ///      section - the section that should be written.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem (usually the buffer is too small).
    ///
    virtual bool WriteToImpl(BcmOctetBuffer &outputBuffer, NonVolSection section) const;

    /// This is where the derived class reads its members from the buffer in a
    /// flat format.  It is up to the derived class to determine what ordering,
    /// etc., should be used.
    ///
    /// The currentOffset parameter tells the derived class where the app data
    /// starts in the buffer.  It can be used directly in the calls to Read() or
    /// NetworkRead() in BcmOctetBuffer (which expects an offset value).
    ///
    /// This MUST be implemented by the derived class.
    ///
    /// \param
    ///      inputBuffer - the buffer from which the settings should be read.
    /// \param
    ///      currentOffset - the offset into the buffer from which the settings
    ///                      should be read.  This is necessary to allow the
    ///                      derived class to skip the header.
    /// \param
    ///      section - the section that should be read.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem (version number not supported, not
    ///          enough bytes in the buffer, etc.).
    ///
    virtual bool ReadFromImpl(const BcmOctetBuffer &inputBuffer, 
                              unsigned int &currentOffset, NonVolSection section);

    /// Allows the derived class to do formatted output of its settings to the
    /// specified ostream, after the base class does its stuff.
    ///
    /// \param
    ///      outputStream - the ostream to be used.
    ///
    /// \return
    ///      The reference to the ostream that was passed in.
    ///
    virtual ostream & PrintImpl(ostream & outputStream) const;

protected:

    bool fIsInitialized;

private:

    /// Copy Constructor.  Not supported.
    BcmUnrecognizedNonVolSettings(const BcmUnrecognizedNonVolSettings &otherInstance);

    /// Assignment operator.  Not supported.
    BcmUnrecognizedNonVolSettings & operator = (const BcmUnrecognizedNonVolSettings &otherInstance);

};


//********************** Inline Method Implementations ***********************


/// This allows you to do the following:
///
///    cout << settingsInstance << endl;
///
inline ostream & operator << (ostream &outputStream, const BcmUnrecognizedNonVolSettings &settingsInstance)
{
    return settingsInstance.Print(outputStream);
}


#endif



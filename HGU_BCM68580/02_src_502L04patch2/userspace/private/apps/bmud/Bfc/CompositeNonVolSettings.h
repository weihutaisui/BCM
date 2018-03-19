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
//  Filename:       CompositeNonVolSettings.h
//  Author:         David Pullen
//  Creation Date:  Nov 1, 2000
//
//****************************************************************************

#ifndef COMPOSITENONVOLSETTINGS_H
#define COMPOSITENONVOLSETTINGS_H

//********************** Include Files ***************************************

// My base class.
#include "NonVolSettings.h"

#include <vector>

//********************** Global Types ****************************************

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Class Declaration ***********************************


/** \ingroup BfcNonVol
*
*      This is a specialization of the NonVolSettings base class to handle
*      the usage pattern called "Compositing".  Its purpose is to store a list
*      of settings objects, and to iterate over that list when any of the
*      base class public methods are called.  The client doesn't need to know
*      that this is a composite class.
*
*      Generally, an application will derive a child from this class in order
*      to group all of the settings it needs together in a single object.
*
*      Note that it is perfectly legal, and may be useful, for a composite
*      class to be contained in another composite, but for this particular
*      class, it will usually be more desirable to have just a single composite
*      as the parent.
*
*      The composite class generally will not have any setting data of its own.
*/
class BcmCompositeNonVolSettings : public BcmNonVolSettings
{
public:

    /// Default Constructor.  Initializes the state of the object, setting the
    /// magic and version numbers to 0.
    ///
    /// \param
    ///      pInstanceName - the name to be used with the message log settings.
    ///
    BcmCompositeNonVolSettings(const char *pInstanceName = NULL);

    /// Destructor.  Frees up any memory/objects allocated, cleans up internal
    /// state.
    ///
    virtual ~BcmCompositeNonVolSettings();

    /// Adds the specified settings group to the Composite.  This means that it
    /// will be iterated over and operated on.
    ///
    /// This object becomes the owner of the object, and will delete it when
    /// the composite object is deleted.
    ///
    /// \param
    ///      pSettings - pointer to the settings group object to be added.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem - usually because of duplicate magic
    ///          number.
    ///
    bool AddSettingsGroup(BcmNonVolSettings *pSettings);
    
    inline int NumSettingsGroups (void) const;
    
    inline BcmNonVolSettings *GetSettingsGroup (int GroupNum) const;

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
    virtual const BcmNonVolSettings *Find(uint32 magicNumber) const;

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

    /// Returns the Is-Upgraded bool.  For a Composite object, it will check all
    /// of its contained settings groups and return false only if all of them
    /// return false.
    ///
    /// A group is upgraded when it reads non-vol settings that correspond to a
    /// version that is earlier than what is currently supported in the code.
    ///
    /// \param
    ///      section - the section that is being queried.
    ///
    /// \retval
    ///      true if this settings group was upgraded.
    /// \retval
    ///      false if this settings group was not upgraded.
    ///
    virtual bool IsUpgraded(NonVolSection section) const;
    
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
    virtual bool IsManufactured(NonVolSection section, BcmString &hint) const;

    /// For the base class, this does nothing.  The Composite derived class will
    /// override this and use it to delete any unrecognized settings groups that
    /// it contains.
    ///
    /// \retval
    ///      true if unrecognized settings groups were flushed.
    /// \retval
    ///      false if there were no unrecognized settings groups to flush.
    ///
    virtual bool FlushUnrecognized(void);


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
    virtual void FlushNonVitalDynamicInformation(void);

    /// Iterates over all contained objects, and calls FlushExtraData() for each
    /// of them.  PR925
    ///
    /// \param
    ///      section - the section whose extra data is to be deleted.
    ///
    virtual void FlushExtraData(NonVolSection section);

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

private:    // DPullen - changed this from protected to private.  I added code
            // to this class to help with error checking, only to see that it
            // was never being called by derived classes, so I decided to force
            // the issue and let the compiler tell me who was accessing this
            // data member, and change it.  At this point, there really is no
            // need for anyone to access this directly.

    /// These are the settings objects that I contain.
    vector<BcmNonVolSettings *> fSettingsList;

private:

    /// Copy Constructor.  Not supported.
    BcmCompositeNonVolSettings(const BcmCompositeNonVolSettings &otherInstance);

    /// Assignment operator.  Not supported.
    BcmCompositeNonVolSettings & operator = (const BcmCompositeNonVolSettings &otherInstance);

};


//********************** Inline Method Implementations ***********************


inline int BcmCompositeNonVolSettings::NumSettingsGroups (void) const
{
  return fSettingsList.size();
}
    
inline BcmNonVolSettings *BcmCompositeNonVolSettings::GetSettingsGroup (int GroupNum) const
{
  if (GroupNum >= NumSettingsGroups())
    return NULL;
    
  return fSettingsList[GroupNum];
}


/// This allows you to do the following:
///
///    cout << settingsInstance << endl;
///
inline ostream & operator << (ostream &outputStream, const BcmCompositeNonVolSettings &settingsInstance)
{
    return settingsInstance.Print(outputStream);
}


#endif



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
//  Filename:       NonVolSettings.h
//  Author:         David Pullen
//  Creation Date:  Feb 9, 2000
//
//****************************************************************************

#ifndef NONVOLSETTINGS_H
#define NONVOLSETTINGS_H

//********************** Include Files ***************************************

// For the portable types.
#include "typedefs.h"

#include "OctetBuffer.h"

#include "MessageLog.h"

#include "BcmString.h"

// PR6478 - make it easy for clients who use nonvol settings objects to use
// the new access lock object.
#include "NonVolSettingsAccessLock.h"

#include <iostream>

//********************** Global Types ****************************************

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Class Declaration ***********************************


/** \ingroup BfcNonVol
*
*      This is the abstract base class for all kinds of application-specific
*      Non-Volatile settings objects.  An application will define its own
*      class derived from this one, giving it the values and ordering, as well
*      as methods for converting to/from a flat buffer.
*
*      Each NonVol Settings object has a split personality, containing both
*      Permanent and Dynamic settings.  The section that is being operated on
*      is specified as a parameter to most of the methods.  We went with this
*      approach rather than splitting it out to 2 objects because the settings
*      really are related, they are just stored in different locations so that
*      the permanent settings can be protected from write failures.
*
*      Note that the settings are independant of the device to which they are
*      being stored, allowing the same application settings to be used on
*      multiple target platforms and devices!
*
*      The derived class will generally be placed in the application directory,
*      since it is application-specific; if it is used in multiple apps, then
*      it may be appropriate to place it in a common directory.
*
*      This class provides the common API, as well as some helper methods, for
*      all of the settings objects, providing a common interface for all of the
*      possible types of Non-Volatile devices.
*
*      All NonVolSettings have the following 8-byte header layout:
*
\verbatim
        +--------+-----------+----------+----------+------------------------+
        | Length | Magic Num | VerMajor | VerMinor | {App settings data...} |
        +--------+-----------+----------+----------+------------------------+
             2         4           1          1                n
\endverbatim
*
*      The length is set by the base class after the derived class has written
*      all of its data.  On input, it is used bound the number of bytes that
*      are available for reading.  It includes all bytes, including the length
*      field itself, and will always be 8 or larger.
*
*      The magic and version numbers are set by the derived class in the
*      constructor, or are read from the device.  The Magic Number allows the
*      derived class to be sure that it was what wrote the settings (so that
*      it doesn't read in another object's settings).
*
*      The Checksum or other information is handled by the specific
*      NonVolDevice rather than by the settings.
*/
class BcmNonVolSettings
{
public:

    /// Parameter to some of the methods; selects which section (Permanent or
    /// Dynamic) will be operated on.
    typedef enum
    {
        kDynamicSection = 0,
        kPermanentSection
    } NonVolSection;

    /// Default Constructor.  Initializes the state of the object, setting the
    /// magic and version numbers to 0.
    ///
    /// \param
    ///      pInstanceName - the name to be used with the message log settings.
    ///
    BcmNonVolSettings(const char *pInstanceName = NULL);

    /// Destructor.  Frees up any memory/objects allocated, cleans up internal
    /// state.
    ///
    virtual ~BcmNonVolSettings();

    /// This allows some client (usually some other non-vol settings instance)
    /// set another settings instance as my parent.  Usually, one settings
    /// instance will act as a parent container for a bunch of subsystem-specific
    /// settings instances.
    ///
    /// If you call this with NULL, then the parent will be forgotten.
    ///
    /// \param
    ///      pParent - pointer to the instance that you want to be my parent.
    ///
    void SetParent(BcmNonVolSettings *pParent);

    /// Returns a pointer to my parent class, if one is set.  This can return
    /// NULL.
    ///
    /// \return
    ///      A pointer to my parent settings object.
    ///
    inline BcmNonVolSettings *GetParent(void) const;

    /// Returns the magic number for this object.
    ///
    /// \return
    ///      The magic number.
    ///
    inline uint32 GetMagicNumber(void) const;
    
    /// Returns the magic number for this object.
    ///
    /// \param
    ///     section - permanent or dynamic
    /// \param
    ///     major - reference to receive the major version
    /// \param
    ///     minor - reference to receive the minor version
    ///
    inline void GetVersion (NonVolSection section, int &major, int &minor) const;

    /// Returns the previous version for this object, store in the event that
    /// an upgrade occurred.
    ///
    /// \param
    ///     section - permanent or dynamic
    /// \param
    ///     major - reference to receive the major version
    /// \param
    ///     minor - reference to receive the minor version
    ///
    inline void GetPreviousVersion (NonVolSection section, int &major, int &minor) const;

    /// Returns the instance name for this object.
    ///
    /// \return
    ///      The instance name.
    ///
    const char *GetInstanceName(void) const;

    /// Returns the Is-Recognized bool.
    ///
    /// \retval
    ///      true if this settings group is known.
    /// \retval
    ///      false if this settings group is unrecognized.
    ///
    bool IsRecognized(void) const;

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

    /// Returns the Is-Composite bool.
    ///
    /// \retval
    ///      true if this is a Composite object.
    /// \retval
    ///      false if this is not a Composite object.
    ///
    bool IsComposite(void) const;

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

    /// If there is any extra data stored for the specified section, then this
    /// deletes it.  PR925
    ///
    /// \param
    ///      section - the section whose extra data is to be deleted.
    ///
    virtual void FlushExtraData(NonVolSection section);

    /// Provides access to the extra data that is stored for the specified
    /// section.
    ///
    /// \note
    ///     This can return NULL if there is no extra data for the specified
    ///     section!
    ///
    /// \param
    ///     section - the section whose extra data is to be retrieved.
    ///
    /// \return
    ///     A pointer to the octet buffer containing the extra data for the
    ///     specified section, or NULL if there is no extra data for the section.
    ///
    inline const BcmOctetBuffer *ExtraData(NonVolSection section) const;

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
    virtual void ResetDefaults(NonVolSection section) = 0;

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

    /// Helper function that check to see if the version that was read (stored
    /// in fVersionMajor and fVersionMinor) is greater than or equal to the
    /// specified version; for example, this method returns true if:
    ///
    /// \code
    ///      fVersionMajor.fVersionMinor >= versionMajor.versionMinor
    /// \endcode
    ///
    /// \param
    ///      versionMajor - the version to be compared against.
    /// \param
    ///      versionMinor - the version to be compared against.
    /// \param
    ///      section - the section whose version number should be compared.
    ///
    /// \retval
    ///      true if the current version is greater than or equal to the
    ///          specified verison.
    /// \retval
    ///      false if the current version is less.
    ///
    bool MyVersionIsGreaterThanOrEqualTo(uint8 versionMajor,
                                         uint8 versionMinor,
                                         NonVolSection section) const;

    /// Helper function that check to see if the version that was read (stored
    /// in fVersionMajor and fVersionMinor) is less than or equal to the
    /// specified version; for example, this method returns true if:
    ///
    /// \code
    ///      fVersionMajor.fVersionMinor <= versionMajor.versionMinor
    /// \endcode
    ///
    /// \param
    ///      versionMajor - the version to be compared against.
    /// \param
    ///      versionMinor - the version to be compared against.
    /// \param
    ///      section - the section whose version number should be compared.
    ///
    /// \retval
    ///      true if the current version is less than or equal to the
    ///          specified verison.
    /// \retval
    ///      false if the current version is greater.
    ///
    bool MyVersionIsLessThanOrEqualTo(uint8 versionMajor,
                                      uint8 versionMinor,
                                      NonVolSection section) const;

    /// Helper function that check to see if the previous version that 
    /// was read (stored in fVersionMajor and fVersionMinor) is less 
    /// than or equal to the specified version; for example, this 
    /// method returns true if:
    ///
    /// \code
    ///      fPreviousVersionMajor.fVersionMinor <= versionMajor.versionMinor
    /// \endcode
    ///
    /// \param
    ///      versionMajor - the version to be compared against.
    /// \param
    ///      versionMinor - the version to be compared against.
    /// \param
    ///      section - the section whose version number should be compared.
    ///
    /// \retval
    ///      true if the previous version is less than or equal to the
    ///          specified verison.
    /// \retval
    ///      false if the previous version is greater.
    ///
    bool MyPrevVersionIsLessThanOrEqualTo(uint8 versionMajor,
                                          uint8 versionMinor,
                                          NonVolSection section) const;

    /// Helper function that is used to detect if we had a recent upgrade. This
    /// is performed if the previous major and minor values are zero, if they
    /// are we did not have an upgrade since they would never get updated.
    ///
    /// \retval
    ///      true if we detected an upgrade.
    /// \retval
    ///      false if we did not detect and upgrade.
    ///
    bool DetectUpgrade(NonVolSection section) const;

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
    virtual bool WriteToImpl(BcmOctetBuffer &outputBuffer, NonVolSection section) const = 0;

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
                              unsigned int &currentOffset, NonVolSection section) = 0;

    /// Allows the derived class to do formatted output of its settings to the
    /// specified ostream, after the base class does its stuff.
    ///
    /// \param
    ///      outputStream - the ostream to be used.
    ///
    /// \return
    ///      The reference to the ostream that was passed in.
    ///
    virtual ostream & PrintImpl(ostream & outputStream) const = 0;

    /// This is a helper method that checks for differences between the currently
    /// supported version and the version that was read, displaying an
    /// appropriate message.  If the current version is newer, then it "upgrades"
    /// the version numbers that were read.
    ///
    /// \param
    ///      currentVersionMajor - the major part of the version number that is
    ///                            currently supported.
    /// \param
    ///      currentVersionMinor - the minor part of the version number that is
    ///                            currently supported.
    /// \param
    ///      section - the section that is being checked.
    ///
    void CheckForVersionUpgrade(uint8 currentVersionMajor,
                                uint8 currentVersionMinor,
                                NonVolSection section);

protected:

    /// Various flags to make note of state and identify information.
    bool fIsRecognized;
    bool fIsDefault[2];
    bool fIsUpgraded[2];
    bool fIsComposite;

    /// This allows this class to be contained in a parent settings group, so
    /// that when someone tells me to write my settings out, I can cause the
    /// entire settings to be written.
    BcmNonVolSettings *pfParent;

    /// This is an id (magic number) that is unique to the application.  It lets
    /// the app determine whether or not the non-vol settings were written by
    /// this application or a different one.  This is necessary because the CRC
    /// may check out correctly, and the version number may seem reasonable, but
    /// the structure of the settings may not match this app.
    ///
    /// We use the same magic number for both permanent and dynamic settings.
    uint32 fMagicNumber;

    /// Application-specific version numbers (such as 1.0, 1.1, etc.).  These
    /// can be set by the app as desired, and are for its own use in determining
    /// what fields may or may not be present, etc.  It is up to the app as to
    /// whether or not backward compatibility with older versions will be
    /// supported.
    uint8 fVersionMajor[2];
    uint8 fVersionMinor[2];

    /// Storage for the version PRIOR to an upgrade
    uint8 fPreviousVersionMajor[2];
    uint8 fPreviousVersionMinor[2];
    
    /// Constant to identify unknown version for native version, below.
    enum
    {
        kNativeVersionUnknown = 0x100
    };
    
    /// Storage for the native version of THIS code version
    uint16 fNativeVersionMajor[2];
    uint16 fNativeVersionMinor[2];

    /// These are used to hold Group settings data that is not read or consumed
    /// by the derived class.  This preserves forward compatibility in case an
    /// older version of the settings reads data written by a newer version.  We
    /// need to be able to write this unused data back out so that it is not
    /// lost.
    BcmOctetBuffer *pfExtraGroupData[2];

    /// Controls message logging output.
    BcmMessageLogSettings fMessageLogSettings;

private:

    /// Copy Constructor.  Not supported.
    BcmNonVolSettings(const BcmNonVolSettings &otherInstance);

    /// Assignment operator.  Not supported.
    BcmNonVolSettings & operator = (const BcmNonVolSettings &otherInstance);

};


//********************** Inline Method Implementations ***********************


/// This allows you to do the following:
///
///    cout << settingsInstance << endl;
///
inline ostream & operator << (ostream &outputStream, const BcmNonVolSettings &settingsInstance)
{
    return settingsInstance.Print(outputStream);
}


inline BcmNonVolSettings *BcmNonVolSettings::GetParent(void) const
{
    return pfParent;
}


inline uint32 BcmNonVolSettings::GetMagicNumber(void) const
{
    return fMagicNumber;
}


inline void BcmNonVolSettings::GetVersion (NonVolSection section, int &major, int &minor) const
{
    major = fVersionMajor[section];
    minor = fVersionMinor[section];
}


inline void BcmNonVolSettings::GetPreviousVersion (NonVolSection section, int &major, int &minor) const
{
    major = fPreviousVersionMajor[section];
    minor = fPreviousVersionMinor[section];
}


inline const BcmOctetBuffer *BcmNonVolSettings::ExtraData(NonVolSection section) const
{
    return pfExtraGroupData[section];
}


// Special doxygen documentation tag for the BFC Core objects.
/** \defgroup BfcNonVol BFC Non-Volatile Settings (i.e. NonVol)

This document describes NonVol Settings as implemented by the BFC Core framework,
and extended by numerous BFC applications and components.

<H2>Introduction</H2>

Communications devices often need to store information so that it persists across
power cycles.  Such information is usually called non-volatile settings.

Some of the information is set at manufacturing time:
  - MAC addresses
  - Certificates
  - Calibration data
  - Serial number

Other data may be initialized at manufacturing time, but can be changed at
runtime:
  - Enable/disable encryption
  - User name/password
  - Change the WiFi SSID


<H2>Details</H2>

<H3>NonVol Section</H3>

<H3>NonVol Group</H3>

<H3>NonVol Device</H3>

<H2>Features/Benefits/Requirements</H2>

<H3>Preserve Binary Footprint</H3>

<H4>Formatting of Data Items</H4>

<H3>Upgrades</H3>

<H3>Downgrades</H3>

<H3>Multi-System Support</H3>

*/

#endif



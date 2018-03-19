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
//  Filename:       BfcAppNonVolSettings.h (was CmAppNonVolSettings.h)
//  Author:         David Pullen
//  Creation Date:  Feb 10, 2000
//
//****************************************************************************

#ifndef BfcAppNonVolSettings_H
#define BfcAppNonVolSettings_H

//********************** Include Files ***************************************

// My base class.
#include "NonVolSettings.h"

//********************** Global Types ****************************************

//********************** Global Constants ************************************

// The presence or absence of the following #define controls how the console
// mode NV setting will affect telnet & SSH.
//#define CONSOLE_DISABLE_ALSO_DISABLES_TELNET_AND_SSH

//********************** Global Variables ************************************

//********************** Class Declaration ***********************************


/** \ingroup BfcCore BfcNonVol
*
*   These are the Non-Volatile settings for the core BFC application.  They
*   control the startup behavior for the BFC core.
*/
class BcmBfcAppNonVolSettings : public BcmNonVolSettings
{
public:

    /// Default Constructor.  Initializes the state of the object, setting all
    /// fields to good default values.
    ///
    BcmBfcAppNonVolSettings(void);

    /// Destructor.  Frees up any memory/objects allocated, cleans up internal
    /// state.
    ///
    virtual ~BcmBfcAppNonVolSettings();

    /// Set all values to a known good state.
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

    /// Returns the pointer to the singleton instance for this class.  Most
    /// objects in the system will use this method rather than being passed a
    /// pointer to it.  The singleton pointer will be set up in the base-class
    /// constructor.
    ///
    /// \note   This can return NULL if a singleton has not been set up for the
    ///         system, so you must check for this condition.
    ///
    /// \note   You must not delete this object!
    ///
    /// \note   You should not store a pointer to the object that is returned,
    ///         since it may be deleted and replaced with a new one.
    ///
    /// \return
    ///      A pointer to the instance that should be used by the system.
    ///
    static BcmBfcAppNonVolSettings *GetSingletonInstance(void);

public:

    /// Accessors for the auto-stop-at-console enabled bool.
    ///
    /// \param
    ///      enable - set to true to enable this feature, false to disable.
    ///
    /// \retval
    ///      true if enabled.
    /// \retval
    ///      false if disabled.
    ///
    inline bool AutoStopAtConsole(void) const;
    inline void AutoStopAtConsole(bool enable);

    /// Accessors for the driver init prompt enabled bool.
    ///
    /// \param
    ///      enable - set to true to enable this feature, false to disable.
    ///
    /// \retval
    ///      true if enabled.
    /// \retval
    ///      false if disabled.
    ///
    inline bool DriverInitPrompt(void) const;
    inline void DriverInitPrompt(bool enable);

    /// Accessors for the stop at console prompt enabled bool.
    ///
    /// \param
    ///      enable - set to true to enable this feature, false to disable.
    ///
    /// \retval
    ///      true if enabled.
    /// \retval
    ///      false if disabled.
    ///
    inline bool StopAtConsolePrompt(void) const;
    inline void StopAtConsolePrompt(bool enable);
    
    /// Accessors for serial console operational mode (PR 11916).
    ///
    /// \mode
    ///      kConsoleDisabled - console input & output are disabled
    ///      kConsoleReadOnly - console output is enabled but input is disabled
    ///      kConsoleReadWrite - console input & output are enabled
    ///
    /// \retval
    ///      kConsoleDisabled - console input & output are disabled
    /// \retval
    ///      kConsoleReadOnly - console output is enabled but input is disabled
    /// \retval
    ///      kConsoleReadWrite - console input & output are enabled
    ///
    typedef enum
    {
      kConsoleDisabled = 0,
      kConsoleReadOnly,
      kConsoleReadWrite
    } ConsoleMode;
    
    inline ConsoleMode SerialConsoleMode(void) const;
    inline void SerialConsoleMode (ConsoleMode mode);

protected:

    /// Here's where I write my members to the buffer in a flat format, in the
    /// order described at the top of this file.  All multibyte values are
    /// written in network byte order.
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

    /// Here's where I read my members from the buffer in a flat format, in the
    /// order described at the top of this file.  All multibyte values are read
    /// in network byte order.
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
    virtual bool ReadFromImpl(const BcmOctetBuffer &inputBuffer, 
                              unsigned int &currentOffset, NonVolSection section);

    /// Causes me to do formatted output of my settings to the specified ostream,
    /// after the base class does its stuff.
    ///
    /// \param
    ///      outputStream - the ostream to be used.
    ///
    /// \return
    ///      The reference to the ostream that was passed in.
    ///
    virtual ostream & PrintImpl(ostream & outputStream) const;

private:

    // Permanent settings.

    /// Bitmask containing some enable/disable bits.
    uint8 fEnableDisableBitmask;

    /// The bits for the bitmask.
    enum
    {
        /// If set, the app will automatically stop at the console without user
        /// intervention.
        kAutoConsoleEnable = 0x01,

        /// If set, the "skip driver initialization" prompt will not be
        /// displayed.  This means that you can't skip loading the drivers that
        /// are enabled in nonvol, but it speeds up boot time by 1.5 seconds.
        kSkipDriverInitPromptDisable = 0x02,

        /// If set, the "stop at console" prompt will not be displayed.  This
        /// means that you can't stop the CM app from beginning the downstream
        /// scan, but is speeds up boot time by 2 seconds.  Note that if you
        /// enable the auto-console bit, then it will still stop automatically.
        kStopAtAppPromptDisable = 0x04
    };
    
    /* Dynamic settings. */
    
    // PR 11916
    ConsoleMode fSerialConsoleMode;

private:

    /// This is the singleton instance of this class.
    static BcmBfcAppNonVolSettings *pgSingletonInstance;

private:

    /// Copy Constructor.  Not supported.
    BcmBfcAppNonVolSettings(const BcmBfcAppNonVolSettings &otherInstance);

    /// Assignment operator.  Not supported.
    BcmBfcAppNonVolSettings & operator = (const BcmBfcAppNonVolSettings &otherInstance);

};


//********************** Inline Method Implementations ***********************


/// Accessors for the auto-stop-at-console enabled bool.
///
/// \param
///      enable - set to true to enable this feature, false to disable.
///
/// \retval
///      true if enabled.
/// \retval
///      false if disabled.
///
inline bool BcmBfcAppNonVolSettings::AutoStopAtConsole(void) const
{
    if (fEnableDisableBitmask & kAutoConsoleEnable)
    {
        return true;
    }

    return false;
}


inline void BcmBfcAppNonVolSettings::AutoStopAtConsole(bool enable)
{
    if (enable)
    {
        fEnableDisableBitmask |= kAutoConsoleEnable;
    }
    else
    {
        fEnableDisableBitmask &= ~kAutoConsoleEnable;
    }
}


/// Accessors for the driver init prompt enabled bool.
///
/// \param
///      enable - set to true to enable this feature, false to disable.
///
/// \retval
///      true if enabled.
/// \retval
///      false if disabled.
///
inline bool BcmBfcAppNonVolSettings::DriverInitPrompt(void) const
{
    // This is a disable bit, so I return the opposite.
    if (fEnableDisableBitmask & kSkipDriverInitPromptDisable)
    {
        return false;
    }

    return true;
}


inline void BcmBfcAppNonVolSettings::DriverInitPrompt(bool enable)
{
    // This is a disable bit, so I set the opposite.
    if (enable == false)
    {
        fEnableDisableBitmask |= kSkipDriverInitPromptDisable;
    }
    else
    {
        fEnableDisableBitmask &= ~kSkipDriverInitPromptDisable;
    }
}


/// Accessors for the stop at console prompt enabled bool.
///
/// \param
///      enable - set to true to enable this feature, false to disable.
///
/// \retval
///      true if enabled.
/// \retval
///      false if disabled.
///
inline bool BcmBfcAppNonVolSettings::StopAtConsolePrompt(void) const
{
    // This is a disable bit, so I return the opposite.
    if (fEnableDisableBitmask & kStopAtAppPromptDisable)
    {
        return false;
    }

    return true;
}


inline void BcmBfcAppNonVolSettings::StopAtConsolePrompt(bool enable)
{
    // This is a disable bit, so I set the opposite.
    if (enable == false)
    {
        fEnableDisableBitmask |= kStopAtAppPromptDisable;
    }
    else
    {
        fEnableDisableBitmask &= ~kStopAtAppPromptDisable;
    }
}

/// Accessors for console operational mode (PR 11916).
///
/// \mode
///      kConsoleDisabled - console input & output are disabled
///      kConsoleReadOnly - console output is enabled but input is disabled
///      kConsoleReadWrite - console input & output are enabled
///
/// \retval
///      kConsoleDisabled - console input & output are disabled
/// \retval
///      kConsoleReadOnly - console output is enabled but input is disabled
/// \retval
///      kConsoleReadWrite - console input & output are enabled
///
inline BcmBfcAppNonVolSettings::ConsoleMode BcmBfcAppNonVolSettings::SerialConsoleMode(void) const
{
  return fSerialConsoleMode;
}

inline void BcmBfcAppNonVolSettings::SerialConsoleMode (BcmBfcAppNonVolSettings::ConsoleMode mode)
{
  fSerialConsoleMode = mode;
}


/// This allows you to do the following:
///
///    cout << settingsInstance << endl;
///
inline ostream & operator << (ostream &outputStream, const BcmBfcAppNonVolSettings &settingsInstance)
{
    return settingsInstance.Print(outputStream);
}

#endif



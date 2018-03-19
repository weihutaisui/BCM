//****************************************************************************
//
// Copyright (c) 2005-2013 Broadcom Corporation
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
//  Filename:       BatteryNonVolSettings.h
//  Author:         Kevin O'Neal
//  Creation Date:  January 13, 2005
//
//****************************************************************************
//  Description:
//      These are the Non-Volatile settings for battery settings. This is 
//      derived from the abstract base NonVolSettings class, which provides 
//      hooks for loading and storing the settings.
//
//      Note that the settings are independant of the device to which they are
//      being stored, allowing the same application settings to be used on
//      multiple target platforms and devices!
//
//****************************************************************************

#ifndef BATTERYNONVOLSETTINGS_H
#define BATTERYNONVOLSETTINGS_H

// "C" callable API's to get ifTimeout values
#if defined (__cplusplus)
extern "C"
{
#endif

int DocsisCmPowerMgmtTimeoutSeconds ();
int EthernetPowerMgmtTimeoutSeconds ();
int HpnaPowerMgmtTimeoutSeconds ();
int UsbPowerMgmtTimeoutSeconds ();
int BluetoothPowerMgmtTimeoutSeconds ();
int WiFiPowerMgmtTimeoutSeconds ();
int WiFi2PowerMgmtTimeoutSeconds ();
int MocaPowerMgmtTimeoutSeconds ();

#if defined (__cplusplus)
}
#endif

#if defined (__cplusplus)

//********************** Include Files ***************************************

//********************** Include Files ***************************************
// My base class.
#include "NonVolSettings.h"
#include "BcmString.h"
#include "battery.h"

//********************** Global Types ****************************************

//********************** Global Variables ************************************

//********************** Class Declaration ***********************************

/** \ingroup BfcNonVol
*/
class BcmBatteryNonVolSettings : public BcmNonVolSettings
{
public:

    // Default Constructor.  Initializes the state of the object, setting all
    // fields to good default values.
    //
    // Parameters:  None.
    //
    // Returns:  N/A
    //
    BcmBatteryNonVolSettings(void);

    // Destructor.  Frees up any memory/objects allocated, cleans up internal
    // state.
    //
    // Parameters:  N/A
    //
    // Returns:  N/A
    //
    virtual ~BcmBatteryNonVolSettings();

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
    virtual void ResetDefaults(NonVolSection section);

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
    static BcmBatteryNonVolSettings *GetSingletonInstance(void);
    
    /* Dynamic section accessors */
    
    // Get/set the period which is used by the battery thread to poll the
    // driver for the latest battery information.
    int PollIntervalSeconds() const;
    bool PollIntervalSeconds (int Seconds);
    
    // Get/set the number of seconds after the detection of a power source
    // change before the battery thread publishes an event.  Note that this
    // value can only be as precise as PollIntervalSeconds, above.
    int PowerSourceEventSeconds () const;
    bool PowerSourceEventSeconds (int Seconds);
    
    // Get/set the maximum percentage to which the batteries will be charged.
    int MaxChargePercent () const;
    bool MaxChargePercent (int Percent);
    
    // Rev 0.2 settings: UPS MIB things
    const BcmString &UpsIdentName () const;
    bool UpsIdentName (const BcmString &Name);
    
    const BcmString &UpsIdentAttachedDevices () const;
    bool UpsIdentAttachedDevices (const BcmString &Devices);
    
    int UpsConfigLowBattTime () const;
    bool UpsConfigLowBattTime (int Time);
    
    // Rev 0.3: replacement threshold (PR 8745)
    int ReplacementThresholdPercent () const;
    bool ReplacementThresholdPercent (int Percent);
    
    // Rev 0.4: interface shutdown timing (PR 9257)
    bool IfTypePowerMgmtTimeoutSeconds (int BrcmIanaIfType, int *pSeconds, int Instance=0) const;
    bool IfTypePowerMgmtTimeoutSeconds (int BrcmIanaIfType, int Seconds, int Instance=0);
    
    int DocsisCmPowerMgmtTimeoutSeconds () const;
    bool DocsisCmPowerMgmtTimeoutSeconds (int Seconds);
    
    int EthernetPowerMgmtTimeoutSeconds () const;
    bool EthernetPowerMgmtTimeoutSeconds (int Seconds);
    
    int HpnaPowerMgmtTimeoutSeconds () const;
    bool HpnaPowerMgmtTimeoutSeconds (int Seconds);
    
    int UsbPowerMgmtTimeoutSeconds () const;
    bool UsbPowerMgmtTimeoutSeconds (int Seconds);
    
    int BluetoothPowerMgmtTimeoutSeconds () const;
    bool BluetoothPowerMgmtTimeoutSeconds (int Seconds);
    
    // The 'instance' argument here is used for DBC wifi.  Instance values
    // are 0 or 1.
    bool WiFiPowerMgmtTimeoutSeconds (int *pSeconds, int Instance = 0) const;
    bool WiFiPowerMgmtTimeoutSeconds (int Seconds, int Instance = 0);
    
    // Convenience methods to get/set an individual wifi timeout
    int WiFiPowerMgmtTimeoutSeconds () const;
    int WiFi2PowerMgmtTimeoutSeconds () const;
    bool WiFi2PowerMgmtTimeoutSeconds (int Seconds);

    int MocaPowerMgmtTimeoutSeconds () const;
    bool MocaPowerMgmtTimeoutSeconds (int Seconds);

    // Rev 0.5: low charge threshold (PR 9726)
    int LowChargePercent () const;
    bool LowChargePercent (int Percent);
    
	// Rev 0.6: 3378 rewrite
	bool LifeTestingEnabled() const;
	bool LifeTestingEnabled(bool enable);

	uint32 LifeTestPeriod() const;			// days
	bool LifeTestPeriod(uint32 period);		// days

	const char *LifeTestTOD() const;		// "hh:mm"
	bool LifeTestTOD( const BcmString &tod);// "hh:mm"

	//*********** 0.6 utility functions ********************
	// returns an index (0/1) to the non-vol battery information for the specified GUID
	// return -1 if no such battery exists
	int FindBatteryInfo(uint64 guid);

	// given an index (0/1), returns the structure referenced by that index
	// returns NULL if the index is <0 or >1
	TNonVolBatteryInfo *GetBattery(int index);

	uint16 SeriesResistance(int nvIndex) const;
	bool SeriesResistance(int nvIndex, uint16 milliOhms);

	uint32 LifeTestCount(int nvIndex) const;
	bool LifeTestCount(int nvIndex, uint32 count);

	time_t LastTestTime(int nvIndex) const;
	bool LastTestTime(int nvIndex, time_t when);

	time_t NextTestTime(int nvIndex) const;
	bool NextTestTime(int nvIndex, time_t when);

	//*********** 0.7 Accessors ********************
	bool PackVoltageSmoothingSteps(uint16 steps);
	uint16 PackVoltageSmoothingSteps() const;

	bool BoardVoltageSmoothingSteps(uint16 steps);
	uint16 BoardVoltageSmoothingSteps() const;

	bool PackTemperatureSmoothingSteps(uint16 steps);
	uint16 PackTemperatureSmoothingSteps() const;

	bool BoardTemperatureSmoothingSteps(uint16 steps);
	uint16 BoardTemperatureSmoothingSteps() const;

        //*********** 0.8 Accessors ***** SWCM-25406 ****
	uint32 AllowedFaults() const;
	bool AllowedFaults(uint32 mask);

    /* Permanent section accessors */
    // Rev 0.2 permanent settings: nominal discharge time
    int NominalDischargeSecondsPerBattery () const;
    bool NominalDischargeSecondsPerBattery (int Seconds);
    
    // Rev 0.3 permanent settings: reset on poll PR 9120 (9099)
    bool ResetControllerOnPoll () const;
    bool ResetControllerOnPoll (bool ResetOnPoll);
    
protected:

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
    virtual bool WriteToImpl(BcmOctetBuffer &outputBuffer, NonVolSection section) const;

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
    virtual bool ReadFromImpl(const BcmOctetBuffer &inputBuffer, 
                              unsigned int &currentOffset, NonVolSection section);

    // Allows the derived class to do formatted output of its settings to the
    // specified ostream, after the base class does its stuff.
    //
    // Parameters:
    //      outputStream - the ostream to be used.
    //
    // Returns:
    //      The reference to the ostream that was passed in.
    //
    virtual ostream & PrintImpl(ostream & outputStream) const;
    
protected:

    // Used to keep Reset() from printing warnings when it is called from the
    // constructor.
    bool fConstructorIsRunning;

public:


private:
   // This is the singleton instance of this class.
    static BcmBatteryNonVolSettings *pfSingletonInstance;

private:
    
    // Copy Constructor.  Not supported.
    //
    // Parameters:
    //      otherInstance - reference to the object to copy.
    //
    // Returns:  N/A
    //
    BcmBatteryNonVolSettings(const BcmBatteryNonVolSettings &otherInstance);

    // Assignment operator.  Not supported.
    //
    // Parameters:
    //      otherInstance - reference to the object to copy.
    //
    // Returns:
    //      A reference to "this" so that operator = can be chained.
    //
    BcmBatteryNonVolSettings & operator = (const BcmBatteryNonVolSettings &otherInstance);

	void DefaultBatteryInfo();
	void CreateDefaultBatteryInfo(int index, uint64 guid);

private:
    
    // Settings for the Permanent Group:
    // ---------------------------------
    
    /* Rev 0.2 setting */
    
    int32 fNominalDischargeTime;
    
    /* Rev 0.3 setting, PR 9120 (9099) */
    
    bool fResetControllerOnPoll;
    
    // Settings for the Dynamic Group:
    // ------------------------------
    int32 fPollIntervalSeconds;
    int32 fPowerSourceEventSeconds;
    int32 fMaxChargePercent;
    
    /* Rev 0.2 settings, from UPS MIB */
    
    BcmString fUpsIdentName;
    BcmString fUpsIdentAttachedDevices;
    int32 fUpsConfigLowBattTime;
    
    /* Rev 0.3 setting, PR 8745 */
    
    int fReplacementThresholdPercent;
    
    /* Rev 0.4: interface power mgmt timing (PR 9257) */
    
    int32 fDocsisCmPowerMgmtTimeoutSeconds;
    int32 fEthernetPowerMgmtTimeoutSeconds;
    int32 fHpnaPowerMgmtTimeoutSeconds;
    int32 fUsbPowerMgmtTimeoutSeconds;
    int32 fBluetoothPowerMgmtTimeoutSeconds;
    int32 fWiFiPowerMgmtTimeoutSeconds[2]; // Second instance is for rev 0.8
    
    /* Rev 0.5: low charge threshold (PR 9726) */
    
    int32 fLowChargePercent;

	/* Rev 0.6: 3378 rewrite*/
	bool 	fLifeTestingEnabled;
	uint32  fLifeTestPeriod;		// days between life tests
	BcmString fLifeTestTOD;
	TNonVolBatteryInfo	fBatteryInfo[2];

	/*Rev 0.7: Smoothing steps (SWCM11533) */
	uint16	fPackVoltageSmoothingSteps;
	uint16	fBoardVoltageSmoothingSteps;
	uint16	fPackTemperatureSmoothingSteps;
	uint16	fBoardTemperatureSmoothingSteps;

    /* Rev 0.8: Second wifi timeout for DBC wifi (fWiFiPowerMgmtTimeoutSeconds[1]) */
    // see fWiFiPowerMgmtTimeoutSeconds above

    /* Rev 0.9: MoCA power mgmt timeout */
        int32 fMocaPowerMgmtTimeoutSeconds;

    /* Rev 0.10: H/W Fault coverage (SWCM-25406) */
	uint16  fAllowedHWFaults;
};

//********************** Inline Method Implementations ***********************


// This allows you to do the following:
//
//    cout << settingsInstance << endl;
//
inline ostream & operator << (ostream &outputStream, const BcmBatteryNonVolSettings &settingsInstance)
{
    return settingsInstance.Print(outputStream);
}


#endif

#endif

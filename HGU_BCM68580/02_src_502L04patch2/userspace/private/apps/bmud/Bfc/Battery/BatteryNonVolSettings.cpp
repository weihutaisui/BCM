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
//  Filename:       BatteryNonVolSettings.cpp
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
 
//********************** Include Files ***************************************

// My api and definitions...
#include "BatteryNonVolSettings.h"
#include "BatteryThread.h"
#include "battery.h"
#include <iomanip>

#include "MergedNonVolDefaults.h"
#include "IANAifType.h"

// PR 14641: Set LPV default charge level to 90% (2-cell batteries)
#ifndef kBatteryNumberOfCells
	#define kBatteryNumberOfCells 3
#endif
//PR15585 - allow bsp's to determine default charge percentage
#ifndef kDefaultBatteryChargePercent
	// PR22444 - 5.5.2 stream defaults to 90%
	#define kDefaultBatteryChargePercent 90
#endif

//********************** Local Types *****************************************

//********************** Local Constants *************************************

// Dynamic rev = 0.7
// Permanent rev = 0.3
const uint8 kCurrentVersionMajor[2] =
{
    // Dynamic, Permanent
    0, 0
};

const uint8 kCurrentVersionMinor[2] =
{
    // Dynamic, Permanent
    10, 3
};


//********************** Local Variables *************************************

//********************** "C" callable API's***********************************

int DocsisCmPowerMgmtTimeoutSeconds ()
{
  BcmBatteryNonVolSettings *pSettings = BcmBatteryNonVolSettings::GetSingletonInstance();

  if (pSettings == NULL)
    return 0;

  return pSettings->DocsisCmPowerMgmtTimeoutSeconds();
}

int EthernetPowerMgmtTimeoutSeconds ()
{
  BcmBatteryNonVolSettings *pSettings = BcmBatteryNonVolSettings::GetSingletonInstance();

  if (pSettings == NULL)
    return 0;

  return pSettings->EthernetPowerMgmtTimeoutSeconds();
}

int HpnaPowerMgmtTimeoutSeconds ()
{
  BcmBatteryNonVolSettings *pSettings = BcmBatteryNonVolSettings::GetSingletonInstance();

  if (pSettings == NULL)
    return 0;

  return pSettings->HpnaPowerMgmtTimeoutSeconds();
}

int UsbPowerMgmtTimeoutSeconds ()
{
  BcmBatteryNonVolSettings *pSettings = BcmBatteryNonVolSettings::GetSingletonInstance();

  if (pSettings == NULL)
    return 0;

  return pSettings->UsbPowerMgmtTimeoutSeconds();
}

int BluetoothPowerMgmtTimeoutSeconds ()
{
  BcmBatteryNonVolSettings *pSettings = BcmBatteryNonVolSettings::GetSingletonInstance();

  if (pSettings == NULL)
    return 0;

  return pSettings->BluetoothPowerMgmtTimeoutSeconds();
}

int WiFiPowerMgmtTimeoutSeconds ()
{
  BcmBatteryNonVolSettings *pSettings = BcmBatteryNonVolSettings::GetSingletonInstance();

  if (pSettings == NULL)
    return 0;

  return pSettings->WiFiPowerMgmtTimeoutSeconds();
}

int WiFi2PowerMgmtTimeoutSeconds ()
{
  BcmBatteryNonVolSettings *pSettings = BcmBatteryNonVolSettings::GetSingletonInstance();

  if (pSettings == NULL)
    return 0;

  return pSettings->WiFi2PowerMgmtTimeoutSeconds();
}

int MocaPowerMgmtTimeoutSeconds ()
{
  BcmBatteryNonVolSettings *pSettings = BcmBatteryNonVolSettings::GetSingletonInstance();

  if (pSettings == NULL)
    return 0;

  return pSettings->MocaPowerMgmtTimeoutSeconds();
}


//********************** Class Method Implementations ************************

// Set this to NULL initially.  This will be set up in my constructor.
BcmBatteryNonVolSettings *BcmBatteryNonVolSettings::pfSingletonInstance = NULL;


// Default Constructor.  Initializes the state of the object, setting all
// fields to good default values.
//
// Parameters:  None.
//
// Returns:  N/A
//
BcmBatteryNonVolSettings::BcmBatteryNonVolSettings(void) :
    BcmNonVolSettings("Battery Management NonVol Settings")
{
    // Override the class name in the message log settings that was set by my
    // base class.
    fMessageLogSettings.SetModuleName("BcmBatteryNonVolSettings");
	fMessageLogSettings.SetEnabledFields(BcmMessageLogSettings::kTimestampField | BcmMessageLogSettings::kFunctionNameField);

    fConstructorIsRunning = true;

     // Just let this do the work.
    ResetDefaults(kDynamicSection);
    ResetDefaults(kPermanentSection);

    fConstructorIsRunning = false;

    // Set myself as the singleton.
    if (pfSingletonInstance == NULL)
    {
        gInfoMsg(fMessageLogSettings, "BcmBatteryNonVolSettings")
            << "Setting up the singleton pointer." << endl;
        pfSingletonInstance = this;
    }
    else
    {
        gWarningMsg(fMessageLogSettings, "BcmBatteryNonVolSettings")
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
BcmBatteryNonVolSettings::~BcmBatteryNonVolSettings()
{
 
    // Clear the singleton pointer.
    if (pfSingletonInstance == this)
    {
        gInfoMsg(fMessageLogSettings, "~BcmBatteryNonVolSettings")
            << "Clearing the singleton pointer." << endl;

        pfSingletonInstance = NULL;
    }
    else
    {
        gWarningMsg(fMessageLogSettings, "~BcmBatteryNonVolSettings")
            << "I'm not the singleton instance!  Leaving the singleton pointer alone..." << endl;
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
void BcmBatteryNonVolSettings::ResetDefaults(NonVolSection section)
{
    // Set the magic numbers.  This is ASCII-hex for "BATT"
    fMagicNumber = 0x42415454;

    // Set the current version number.
    fVersionMajor[section] = kCurrentVersionMajor[section];
    fVersionMinor[section] = kCurrentVersionMinor[section];

    if (section == kPermanentSection)
    {
      // 0.2 settings
      NominalDischargeSecondsPerBattery (kDefaultValue_NominalDischargeSecondsPerBattery);
      
      // 0.3 settings PR 9120 (9099)
      ResetControllerOnPoll (kDefaultValue_ResetBatteryControllerOnPoll);
    }
    else
    {
      // 0.1 settings
      PollIntervalSeconds(5);
      PowerSourceEventSeconds(0); // Changed from 30 to 0 seconds to accomodate PacketCable ATPs
      MaxChargePercent(kDefaultBatteryChargePercent);
      // 0.2
      BcmString Empty = "";
      UpsIdentName (Empty);
      UpsIdentAttachedDevices (Empty);
      UpsConfigLowBattTime (30); // What's a reasonable default value for this?
      
      // 0.3
      ReplacementThresholdPercent (55);
      
      // 0.4
      DocsisCmPowerMgmtTimeoutSeconds (kDefaultValue_DocsisCmPowerMgmtTimeoutSeconds);
      EthernetPowerMgmtTimeoutSeconds (kDefaultValue_EthernetPowerMgmtTimeoutSeconds);
      HpnaPowerMgmtTimeoutSeconds (kDefaultValue_HpnaPowerMgmtTimeoutSeconds);
      UsbPowerMgmtTimeoutSeconds (kDefaultValue_UsbPowerMgmtTimeoutSeconds);
      BluetoothPowerMgmtTimeoutSeconds (kDefaultValue_BluetoothPowerMgmtTimeoutSeconds);
      WiFiPowerMgmtTimeoutSeconds (kDefaultValue_WiFiPowerMgmtTimeoutSeconds);
      
      // 0.5
      LowChargePercent (kDefaultValue_BatteryLowChargePercent);

	  // 0.6
	  LifeTestingEnabled(false);
	  LifeTestPeriod(180);
	  LifeTestTOD("22:00");
	  DefaultBatteryInfo();

	  // 0.7
	  PackVoltageSmoothingSteps(1);
	  BoardVoltageSmoothingSteps(1);
	  PackTemperatureSmoothingSteps(1);
	  BoardTemperatureSmoothingSteps(1);

    // 0.8
    WiFiPowerMgmtTimeoutSeconds (kDefaultValue_WiFi2PowerMgmtTimeoutSeconds, 1);

    // 0.9
    MocaPowerMgmtTimeoutSeconds (kDefaultValue_MocaPowerMgmtTimeoutSeconds);

    // 0.10
	  AllowedFaults(kDOI_FAULT);  // DOI is non-maskable
    }
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
BcmBatteryNonVolSettings *BcmBatteryNonVolSettings::GetSingletonInstance(void)
{
    if (pfSingletonInstance == NULL)
    {
        gLogMessageRaw
            << "BcmBatteryNonVolSettings::GetSingletonInstance:  WARNING - the singleton instance is NULL, and someone is accessing it!" << endl;
    }
    
    return pfSingletonInstance;
}


/* Permanent section accessors */
    
/* Dynamic section accessors */

// Get/set the period which is used by the battery thread to poll the
// driver for the latest battery information.
int BcmBatteryNonVolSettings::PollIntervalSeconds() const
{
  return fPollIntervalSeconds;
}

bool BcmBatteryNonVolSettings::PollIntervalSeconds (int Seconds)
{
  fPollIntervalSeconds = Seconds;
  
  // Update our battery timers now, assuming we've been instantiated.
  // That is, don't do this if this call is being made from a ResetDefaults
  // call at boot time.
  if (pfSingletonInstance)
    BcmBatteryThread::Singleton().UpdateTimers();
    
  return true;
}

 
// Get/set the number of seconds after the detection of a power source
// change before the battery thread publishes an event.  Note that this
// value can only be as precise as PollIntervalSeconds, above.
int BcmBatteryNonVolSettings::PowerSourceEventSeconds () const
{
  return fPowerSourceEventSeconds;
}

bool BcmBatteryNonVolSettings::PowerSourceEventSeconds (int Seconds)
{
  fPowerSourceEventSeconds = Seconds;
  return true;
}

// Get/set the maximum percentage to which the batteries will be charged.
int BcmBatteryNonVolSettings::MaxChargePercent () const
{
  return fMaxChargePercent;
}

bool BcmBatteryNonVolSettings::MaxChargePercent (int Percent)
{
  fMaxChargePercent = Percent;
  BcmBatteryThread::Singleton().BatteryController().SetGlobalPercentCharge(Percent);
  return true;
}

const BcmString &BcmBatteryNonVolSettings::UpsIdentName () const
{
  return fUpsIdentName;
}

bool BcmBatteryNonVolSettings::UpsIdentName (const BcmString &Name)
{
  fUpsIdentName = Name;
  return true;
}

const BcmString &BcmBatteryNonVolSettings::UpsIdentAttachedDevices () const
{
  return fUpsIdentAttachedDevices;
}

bool BcmBatteryNonVolSettings::UpsIdentAttachedDevices (const BcmString &Devices)
{
  fUpsIdentAttachedDevices = Devices;
  return true;
}

int BcmBatteryNonVolSettings::UpsConfigLowBattTime () const
{
  return fUpsConfigLowBattTime;
}

bool BcmBatteryNonVolSettings::UpsConfigLowBattTime (int Time)
{
  fUpsConfigLowBattTime = Time;
  
  // Check for low battery now.  But ONLY IF we have been instantiated!
  if (pfSingletonInstance)
    BcmBatteryThread::Singleton().BatteryController().CheckForLowBattery(false);
  
  return true;
}

// Rev 0.3 dynamic settings
int BcmBatteryNonVolSettings::ReplacementThresholdPercent () const
{
  return fReplacementThresholdPercent;
}

bool BcmBatteryNonVolSettings::ReplacementThresholdPercent (int Percent)
{
  fReplacementThresholdPercent = Percent;
  return true;
}

/* Rev 0.4: interface shutdown timing (PR 9257) */

// This is a generic accessor that operates based on IANA ifType value.  Note
// that for some interface types (ex: HPNA, Bluetooth) there is no IANA number,
// so in that case we'll use the BRCM extended values (> 1000) from IANAifType.h
bool BcmBatteryNonVolSettings::IfTypePowerMgmtTimeoutSeconds (int BrcmIanaIfType, int *pSeconds, int Instance) const
{
  bool Retval = true;
  
  switch (BrcmIanaIfType)
  {
    case CONSTVAL_IANAIFTYPE_DOCSCABLEMACLAYER:
      *pSeconds = DocsisCmPowerMgmtTimeoutSeconds();
      break;
      
    case CONSTVAL_IANAIFTYPE_ETHERNETCSMACD:
      *pSeconds = EthernetPowerMgmtTimeoutSeconds();
      break;
      
    case CONSTVAL_IANAIFTYPE_BRCMHPNA:
      *pSeconds = HpnaPowerMgmtTimeoutSeconds();
      break;
      
    case CONSTVAL_IANAIFTYPE_USB:
      *pSeconds = UsbPowerMgmtTimeoutSeconds();
      break;
      
    case CONSTVAL_IANAIFTYPE_BRCMBLUETOOTH:
      *pSeconds = BluetoothPowerMgmtTimeoutSeconds();
      break;
      
    case CONSTVAL_IANAIFTYPE_IEEE80211:
      Retval = WiFiPowerMgmtTimeoutSeconds(pSeconds, Instance);
      break;

    case CONSTVAL_IANAIFTYPE_BRCMMOCAINTERFACE:
      *pSeconds = MocaPowerMgmtTimeoutSeconds();
      break;
    
    default:
      Retval = false;
      break;
  }
  
  return Retval;
}

bool BcmBatteryNonVolSettings::IfTypePowerMgmtTimeoutSeconds (int BrcmIanaIfType, int Seconds, int Instance)
{
  bool Retval = true;
  
  switch (BrcmIanaIfType)
  {
    case CONSTVAL_IANAIFTYPE_DOCSCABLEMACLAYER:
      Retval = DocsisCmPowerMgmtTimeoutSeconds(Seconds);
      break;
      
    case CONSTVAL_IANAIFTYPE_ETHERNETCSMACD:
      Retval = EthernetPowerMgmtTimeoutSeconds(Seconds);
      break;
      
    case CONSTVAL_IANAIFTYPE_BRCMHPNA:
      Retval = HpnaPowerMgmtTimeoutSeconds(Seconds);
      break;
      
    case CONSTVAL_IANAIFTYPE_USB:
      Retval = UsbPowerMgmtTimeoutSeconds(Seconds);
      break;
      
    case CONSTVAL_IANAIFTYPE_BRCMBLUETOOTH:
      Retval = BluetoothPowerMgmtTimeoutSeconds(Seconds);
      break;
      
    case CONSTVAL_IANAIFTYPE_IEEE80211:
      Retval = WiFiPowerMgmtTimeoutSeconds(Seconds, Instance);
      break;

    case CONSTVAL_IANAIFTYPE_BRCMMOCAINTERFACE:
      Retval = MocaPowerMgmtTimeoutSeconds(Seconds);
      break;
    
    default:
      Retval = false;
      break;
  }
  
  return Retval;
}

int BcmBatteryNonVolSettings::DocsisCmPowerMgmtTimeoutSeconds () const
{
  #if !kDefaultValue_BatteryIfTimeoutSupport
  return 0;
  #endif

  return fDocsisCmPowerMgmtTimeoutSeconds;
}

bool BcmBatteryNonVolSettings::DocsisCmPowerMgmtTimeoutSeconds (int Seconds)
{
  #if !kDefaultValue_BatteryIfTimeoutSupport
  return false;
  #endif

  if (Seconds < -1)
    return false;
    
  fDocsisCmPowerMgmtTimeoutSeconds = Seconds;
  return true;
}

int BcmBatteryNonVolSettings::EthernetPowerMgmtTimeoutSeconds () const
{
  #if !kDefaultValue_BatteryIfTimeoutSupport
  return 0;
  #endif

  return fEthernetPowerMgmtTimeoutSeconds;
}

bool BcmBatteryNonVolSettings::EthernetPowerMgmtTimeoutSeconds (int Seconds)
{
  #if !kDefaultValue_BatteryIfTimeoutSupport
  return false;
  #endif

  if (Seconds < -1)
    return false;
    
  fEthernetPowerMgmtTimeoutSeconds = Seconds;
  return true;
}

int BcmBatteryNonVolSettings::HpnaPowerMgmtTimeoutSeconds () const
{
  #if !kDefaultValue_BatteryIfTimeoutSupport
  return 0;
  #endif

  return fHpnaPowerMgmtTimeoutSeconds;
}

bool BcmBatteryNonVolSettings::HpnaPowerMgmtTimeoutSeconds (int Seconds)
{
  #if !kDefaultValue_BatteryIfTimeoutSupport
  return false;
  #endif

  if (Seconds < -1)
    return false;
    
  fHpnaPowerMgmtTimeoutSeconds = Seconds;
  return true;
}

int BcmBatteryNonVolSettings::UsbPowerMgmtTimeoutSeconds () const
{
  #if !kDefaultValue_BatteryIfTimeoutSupport
  return 0;
  #endif

  return fUsbPowerMgmtTimeoutSeconds;
}

bool BcmBatteryNonVolSettings::UsbPowerMgmtTimeoutSeconds (int Seconds)
{
  #if !kDefaultValue_BatteryIfTimeoutSupport
  return false;
  #else

  if (Seconds < -1)
    return false;
    
  fUsbPowerMgmtTimeoutSeconds = Seconds;
  return true;
  #endif
}

int BcmBatteryNonVolSettings::BluetoothPowerMgmtTimeoutSeconds () const
{
  #if !kDefaultValue_BatteryIfTimeoutSupport
  return 0;
  #endif

  return fBluetoothPowerMgmtTimeoutSeconds;
}

bool BcmBatteryNonVolSettings::BluetoothPowerMgmtTimeoutSeconds (int Seconds)
{
  #if !kDefaultValue_BatteryIfTimeoutSupport
  return false;
  #endif

  if (Seconds < -1)
    return false;
    
  fBluetoothPowerMgmtTimeoutSeconds = Seconds;
  return true;
}

int BcmBatteryNonVolSettings::MocaPowerMgmtTimeoutSeconds () const
{
  #if !kDefaultValue_BatteryIfTimeoutSupport
  return 0;
  #endif

  return fMocaPowerMgmtTimeoutSeconds;
}

bool BcmBatteryNonVolSettings::MocaPowerMgmtTimeoutSeconds (int Seconds)
{
  #if !kDefaultValue_BatteryIfTimeoutSupport
  return false;
  #endif

  if (Seconds < -1)
    return false;
    
  fMocaPowerMgmtTimeoutSeconds = Seconds;
  return true;
}

// Convenience methods to return an individual wifi timeout
int BcmBatteryNonVolSettings::WiFiPowerMgmtTimeoutSeconds () const
{
  #if !kDefaultValue_BatteryIfTimeoutSupport
  return 0;
  #endif

  int Timeout = 0;

  WiFiPowerMgmtTimeoutSeconds (&Timeout, 0);
  return Timeout;
}

int BcmBatteryNonVolSettings::WiFi2PowerMgmtTimeoutSeconds () const
{
  #if !kDefaultValue_BatteryIfTimeoutSupport
  return 0;
  #endif

  int Timeout = 0;

  WiFiPowerMgmtTimeoutSeconds (&Timeout, 1);
  return Timeout;
}

bool BcmBatteryNonVolSettings::WiFiPowerMgmtTimeoutSeconds (int *pSeconds, int Instance) const
{
  #if !kDefaultValue_BatteryIfTimeoutSupport
  *pSeconds = 0;
  return false;
  #endif

  if ((Instance < 0) || (Instance > 1))
    return false;

  if (pSeconds == NULL)
    return false;

  *pSeconds = fWiFiPowerMgmtTimeoutSeconds[Instance];
  return true;
}

bool BcmBatteryNonVolSettings::WiFiPowerMgmtTimeoutSeconds (int Seconds, int Instance)
{
  #if !kDefaultValue_BatteryIfTimeoutSupport
  return false;
  #endif

  if ((Instance < 0) || (Instance > 1))
    return false;

  if (Seconds < -1)
    return false;
    
  fWiFiPowerMgmtTimeoutSeconds[Instance] = Seconds;
  return true;
}

bool BcmBatteryNonVolSettings::WiFi2PowerMgmtTimeoutSeconds (int Seconds)
{
  #if !kDefaultValue_BatteryIfTimeoutSupport
  return false;
  #endif

  return WiFiPowerMgmtTimeoutSeconds(Seconds, 1);
}

// Rev 0.5: low charge threshold (PR 9726)
int BcmBatteryNonVolSettings::LowChargePercent () const
{
  return fLowChargePercent;
}

bool BcmBatteryNonVolSettings::LowChargePercent (int Percent)
{
  if ((Percent < 0) || (Percent > 100))
    return false;
    
  fLowChargePercent = Percent;
  return true;
}

bool BcmBatteryNonVolSettings::LifeTestingEnabled() const
{
	return fLifeTestingEnabled;
}
bool BcmBatteryNonVolSettings::LifeTestingEnabled(bool enable)
{
	fLifeTestingEnabled = enable;
	return true;
}
uint32 BcmBatteryNonVolSettings::LifeTestPeriod() const
{			
	// days
	return fLifeTestPeriod;
}
bool BcmBatteryNonVolSettings::LifeTestPeriod(uint32 period)
{		
	// days
	fLifeTestPeriod = period;
	return true;
}
const char *BcmBatteryNonVolSettings::LifeTestTOD() const
{
	return fLifeTestTOD.c_str();
}
bool BcmBatteryNonVolSettings::LifeTestTOD( const BcmString &tod)
{
	char str[6];
	strncpy(str, tod.c_str(), 5);
	str[5] = '\0';
	for (int ix = 0; ix < 5; ix++) {
		if (ix == 2) continue;	// don't bother to check the colon
		if (!isdigit(str[ix])) return false;
	}
	char temp[3];
	int val;
	strncpy(temp, str, 2);
	val = atoi(temp);
	if (val > 24) return false;
	strncpy(temp, &str[3], 2);
	val = atoi(temp);
	if (val > 59) return false;
	fLifeTestTOD = BcmString(str);
	return true;
}
uint32 BcmBatteryNonVolSettings::AllowedFaults() const
{
	return (uint32)fAllowedHWFaults;
}
bool BcmBatteryNonVolSettings::AllowedFaults(uint32 mask)
{
	  fAllowedHWFaults = (uint16)(mask & (kCOV_FAULT | kCOI_FAULT               // charge faults
									                    | kDOI_FAULT | kDHF_FAULT               // discharge faults
								                      | kIOV_FAULT | kVWE_FAULT | kBOV_FAULT  // general faults
								                      | BFC_FAULTS));
    if(!(fAllowedHWFaults & kDOI_FAULT))
    {
      printf("Adding non-maskable DOI fault to the specified mask!\n");
      fAllowedHWFaults |= kDOI_FAULT;   // DOI is non-maskable
    }
    BcmBatteryThread::Singleton().BatteryController().SetFaultMask(fAllowedHWFaults);
	return true;
}
// returns an index (0/1) to the non-vol battery information for the specified GUID
// if neither slot has that battery, tries to find an open slot.  If there is no
// open slot, picks one to be overwritten
int BcmBatteryNonVolSettings::FindBatteryInfo(uint64 guid)
{
	if((fBatteryInfo[0].flags & kBATTERYNVINFOVALID) && (fBatteryInfo[0].guid == guid))
	{
        gInfoMsg(fMessageLogSettings, "FindBatteryInfo")
            << "Matched GUID to slot 0." << endl;
		return 0;
	}
	if((fBatteryInfo[1].flags & kBATTERYNVINFOVALID) && (fBatteryInfo[1].guid == guid))
	{
		gInfoMsg(fMessageLogSettings, "FindBatteryInfo")
			<< "Matched GUID to slot 1." << endl;
		return 1;
	}
	// no match...find an open slot and return that
	gInfoMsg(fMessageLogSettings, "FindBatteryInfo: NoMatch...searching for an empty location") << endl;
	if(!(fBatteryInfo[0].flags & kBATTERYNVINFOVALID)) {
		CreateDefaultBatteryInfo(0, guid);
		gInfoMsg(fMessageLogSettings, "FindBatteryInfo")
			<< "using empty slot 0." << endl;
		return 0;
	}
	if(!(fBatteryInfo[1].flags & kBATTERYNVINFOVALID)) {
		CreateDefaultBatteryInfo(1, guid);
		gInfoMsg(fMessageLogSettings, "FindBatteryInfo")
			<< "using empty slot 1." << endl;
		return 1;
	}
	// no empty slots...pick one to be overwritten
	gInfoMsg(fMessageLogSettings, "FindBatteryInfo: No empty locations") << endl;
	TBatteryControllerInfo tempInfo;
	BcmBatteryThread::Singleton().BatteryController().GetBatteryControllerInfo(tempInfo);
	gInfoMsg(fMessageLogSettings, "FindBatteryInfo: Controller info copied") << endl;
	if (tempInfo.Batteries[0].updateInfo.Status & kBatStatusValid)
	{
		gInfoMsg(fMessageLogSettings, "FindBatteryInfo") << "Battery 0 is valid." << endl;
		if( tempInfo.Batteries[0].guid != guid)
		{
			gInfoMsg(fMessageLogSettings, "FindBatteryInfo") << "Battery 0 is NOT the requested GUID." << endl;
			if(fBatteryInfo[0].guid == tempInfo.Batteries[0].guid) {
				CreateDefaultBatteryInfo(1, guid);
				gInfoMsg(fMessageLogSettings, "FindBatteryInfo") << "Choosing slot 1 because slot 0 is being used by battery 0." << endl;
				return 1;
			} else {
				CreateDefaultBatteryInfo(0, guid);
				gInfoMsg(fMessageLogSettings, "FindBatteryInfo") << "Choosing slot 0 because slot 1 is being used by battery 0." << endl;
				return 0;
			}
		}
	}
	if (tempInfo.Batteries[1].updateInfo.Status & kBatStatusValid)
	{
		gInfoMsg(fMessageLogSettings, "FindBatteryInfo") << "Battery 1 is valid." << endl;
		if( tempInfo.Batteries[1].guid != guid)
		{
			gInfoMsg(fMessageLogSettings, "FindBatteryInfo") << "Battery 1 is NOT the requested GUID." << endl;
			if(fBatteryInfo[0].guid == tempInfo.Batteries[1].guid) {
				CreateDefaultBatteryInfo(1, guid);
				gInfoMsg(fMessageLogSettings, "FindBatteryInfo") << "Choosing slot 1 because slot 0 is being used by battery 1." << endl;
				return 1;
			} else {
				CreateDefaultBatteryInfo(0, guid);
				gInfoMsg(fMessageLogSettings, "FindBatteryInfo") << "Choosing slot 0 because slot 1 is being used by battery 1." << endl;
				return 0;
			}
		}
	}
	// doesn't really matter...any other existing battery doesn't match either
	// entry...just return entry index 0;
	int index = clock() & 1;
	gInfoMsg(fMessageLogSettings, "FindBatteryInfo") << "Arbitrarily choosing slot " << index << endl;
	CreateDefaultBatteryInfo(index, guid);
	gInfoMsg(fMessageLogSettings, "FindBatteryInfo") << "Returning: " << index << endl;
	return index;
}
// given an index (0/1), returns the structure referenced by that index
// returns NULL if the index is <0 or >1
TNonVolBatteryInfo *BcmBatteryNonVolSettings::GetBattery(int index)
{
	if (index < 0 || index > 1) return NULL;
	return &fBatteryInfo[index];
}

uint16 BcmBatteryNonVolSettings::SeriesResistance(int nvIndex) const
{
	if(nvIndex >= 0 && nvIndex <= 1) {
		return fBatteryInfo[nvIndex].seriesResistance;
	}
	return 0;
}

bool BcmBatteryNonVolSettings::SeriesResistance(int nvIndex, uint16 milliOhms)
{
	if(nvIndex >= 0 && nvIndex <= 1) {
		fBatteryInfo[nvIndex].seriesResistance = milliOhms;
		return true;
	}
	return false;
}

uint32 BcmBatteryNonVolSettings::LifeTestCount(int nvIndex) const
{
	if (nvIndex >= 0 && nvIndex <= 1) {
		return fBatteryInfo[nvIndex].LifeTestCount;
	}
	return 0;
}

bool BcmBatteryNonVolSettings::LifeTestCount(int nvIndex, uint32 count)
{
	if (nvIndex >= 0 && nvIndex <= 1) {
		fBatteryInfo[nvIndex].LifeTestCount = count;
		return true;
	}
	return false;
}

time_t BcmBatteryNonVolSettings::LastTestTime(int nvIndex) const
{
	if (nvIndex >= 0 && nvIndex <= 1) {
		return fBatteryInfo[nvIndex].LastLifeTestStarted;
	}
	return 0;
}

bool BcmBatteryNonVolSettings::LastTestTime(int nvIndex, time_t when)
{
	if (nvIndex >= 0 && nvIndex <= 1) {
		fBatteryInfo[nvIndex].LastLifeTestStarted = when;
		return true;
	}
	return false;
}

time_t BcmBatteryNonVolSettings::NextTestTime(int nvIndex) const
{
	if (nvIndex >= 0 && nvIndex <= 1) {
		return fBatteryInfo[nvIndex].NextUpdate;
	}
	// if index is bad, reschedule for Jan 1, 2300 at 10PM
	struct tm t;
	memset(&t, 0, sizeof(struct tm));
	t.tm_hour = 21;
	t.tm_mday = 1;
	t.tm_year = 400;
	return mktime(&t);
}

bool BcmBatteryNonVolSettings::NextTestTime(int nvIndex, time_t when)
{
	if (nvIndex >= 0 && nvIndex <= 1) {
		fBatteryInfo[nvIndex].NextUpdate = when;
		return true;
	}
	return false;
}

bool BcmBatteryNonVolSettings::PackVoltageSmoothingSteps(uint16 steps)
{
	fPackVoltageSmoothingSteps = (uint16)BcmBatteryThread::Singleton().BatteryController().SmoothingSteps(BcmBatteryController::kPackVoltageSmoothingSteps, steps);
	return fPackVoltageSmoothingSteps == steps;
}
uint16 BcmBatteryNonVolSettings::PackVoltageSmoothingSteps() const
{
	return fPackVoltageSmoothingSteps;
}
bool BcmBatteryNonVolSettings::BoardVoltageSmoothingSteps(uint16 steps)
{
	fBoardVoltageSmoothingSteps = (uint16)BcmBatteryThread::Singleton().BatteryController().SmoothingSteps(BcmBatteryController::kBoardVoltageSmoothingSteps, steps);
	return fBoardVoltageSmoothingSteps == steps;
}
uint16 BcmBatteryNonVolSettings::BoardVoltageSmoothingSteps() const
{
	return	fBoardVoltageSmoothingSteps;
}
bool BcmBatteryNonVolSettings::PackTemperatureSmoothingSteps(uint16 steps)
{
	fPackTemperatureSmoothingSteps = (uint16)BcmBatteryThread::Singleton().BatteryController().SmoothingSteps(BcmBatteryController::kPackTempSmoothingSteps, steps);
	return fPackTemperatureSmoothingSteps == steps;
}
uint16 BcmBatteryNonVolSettings::PackTemperatureSmoothingSteps() const
{
	return	fPackTemperatureSmoothingSteps;
}
bool BcmBatteryNonVolSettings::BoardTemperatureSmoothingSteps(uint16 steps)
{
	fBoardTemperatureSmoothingSteps = (uint16)BcmBatteryThread::Singleton().BatteryController().SmoothingSteps(BcmBatteryController::kBoardTempSmoothingSteps, steps);
	return fBoardTemperatureSmoothingSteps == steps;
}
uint16 BcmBatteryNonVolSettings::BoardTemperatureSmoothingSteps() const
{
	return	fBoardTemperatureSmoothingSteps;
}

void BcmBatteryNonVolSettings::CreateDefaultBatteryInfo(int which, uint64 guid)
{
	gInfoMsg(fMessageLogSettings, "CreateDefaultBatteryInfo") << "Building nonvol block for index: " << which << ", guid: " << hex << guid << dec << endl;
	TNonVolBatteryInfo *info = &fBatteryInfo[which];
	info->guid = guid;
	info->LifeTestCount = 0;
	info->LastLifeTestStarted = 1; // Using 1 instead of 0 because some functions consider 0 == Now.
	info->NextUpdate = time(NULL);
	info->seriesResistance = 500;
	info->wHCorrectionRatio = 1.0;
	info->EstLifeRemaining = 100;
	info->DesignCapacity = 2200;
	info->totalSeconds   = 0;
	info->flags = 0;		// mark as invalid
	gInfoMsg(fMessageLogSettings, "CreateDefaultBatteryInfo") << "Done building nonvol block" << endl;
}

// Rev 0.2 permanent settings: nominal discharge time
int BcmBatteryNonVolSettings::NominalDischargeSecondsPerBattery () const
{
  // Upon further reflection I think it's better to just return the compile-time
  // value here.  This allows us to update this value if we come up with a 
  // SW change that increases battery life.
  return kDefaultValue_NominalDischargeSecondsPerBattery;

#if 0
  return fNominalDischargeTime;
#endif
}

bool BcmBatteryNonVolSettings::NominalDischargeSecondsPerBattery (int Seconds)
{
  fNominalDischargeTime = Seconds;
  return true;
}


// PR 9120 (9099)
bool BcmBatteryNonVolSettings::ResetControllerOnPoll () const
{
  return fResetControllerOnPoll;
}

bool BcmBatteryNonVolSettings::ResetControllerOnPoll (bool ResetOnPoll)
{
  fResetControllerOnPoll = ResetOnPoll;
  return true;
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
bool BcmBatteryNonVolSettings::WriteToImpl(BcmOctetBuffer &outputBuffer, 
                                            NonVolSection section) const
{
    if (section == kDynamicSection)
    {
      /* Rev 0.1 */
      
      if ((outputBuffer.NetworkAddToEnd(fPollIntervalSeconds) == false)
      ||  (outputBuffer.NetworkAddToEnd(fPowerSourceEventSeconds) == false)
      ||  (outputBuffer.NetworkAddToEnd(fMaxChargePercent) == false))
      {
        gErrorMsg(fMessageLogSettings, "WriteToImpl") << "Failed to write rev 0.1 dynamic settings to the buffer!" << endl;
        return false;
      }
      
      /* Added for 0.2: UPS MIB things */
      
      // Write the length of upsIdentName, then the value itself
      if ((outputBuffer.NetworkAddToEnd((uint32)fUpsIdentName.size()) == false)
      ||  (fUpsIdentName.WriteTo(outputBuffer) == false)
      // Write the length of upsIdentAttachedDevices, then the value itself
      ||  (outputBuffer.NetworkAddToEnd((uint32)fUpsIdentAttachedDevices.size()) == false)
      ||  (fUpsIdentAttachedDevices.WriteTo(outputBuffer) == false)
      ||  (outputBuffer.NetworkAddToEnd(fUpsConfigLowBattTime) == false))
      {
        gErrorMsg(fMessageLogSettings, "WriteToImpl") << "Failed to write rev 0.2 dynamic settings to the buffer!" << endl;
        return false;
      }
      
      /* Added for 0.3: ReplacementThresholdPercent */
      
      if (outputBuffer.NetworkAddToEnd(fReplacementThresholdPercent) == false)
      {
        gErrorMsg(fMessageLogSettings, "WriteToImpl") << "Failed to write rev 0.3 dynamic settings to the buffer!" << endl;
        return false;
      }
      
      /* Added for 0.4: Per-Interface type power mgmt timeouts (PR 9257) */
      
      if ((outputBuffer.NetworkAddToEnd(fDocsisCmPowerMgmtTimeoutSeconds) == false)
      ||  (outputBuffer.NetworkAddToEnd(fEthernetPowerMgmtTimeoutSeconds) == false)
      ||  (outputBuffer.NetworkAddToEnd(fHpnaPowerMgmtTimeoutSeconds) == false)
      ||  (outputBuffer.NetworkAddToEnd(fUsbPowerMgmtTimeoutSeconds) == false)
      ||  (outputBuffer.NetworkAddToEnd(fBluetoothPowerMgmtTimeoutSeconds) == false)
      ||  (outputBuffer.NetworkAddToEnd(fWiFiPowerMgmtTimeoutSeconds[0]) == false))
      {
        gErrorMsg(fMessageLogSettings, "WriteToImpl") << "Failed to write rev 0.4 dynamic settings to the buffer!" << endl;
        return false;
      }
      
      /* Added for 0.5: low charge threshold (PR 9726) */
      
      if (outputBuffer.NetworkAddToEnd(fLowChargePercent) == false)
      {
        gErrorMsg(fMessageLogSettings, "WriteToImpl") << "Failed to write rev 0.5 dynamic settings to the buffer!" << endl;
        return false;
      }

	  /* Added for 0.6: 3378 rewrite */
	  if ((outputBuffer.AddToEnd(fLifeTestingEnabled) == false) 
	  ||  (outputBuffer.NetworkAddToEnd(fLifeTestPeriod) == false)
	  ||  (outputBuffer.AddToEnd( (uint8 *) &fBatteryInfo[0], sizeof(TNonVolBatteryInfo)) == false)
	  ||  (outputBuffer.AddToEnd( (uint8 *) &fBatteryInfo[1], sizeof(TNonVolBatteryInfo)) == false))
	  {
		  gErrorMsg(fMessageLogSettings, "WriteToImpl") << "Failed to write rev 0.6 dynamic settings to the buffer!" << endl;
		  return false;
	  }

	  /* Added for 0.7 */
	  if ((outputBuffer.NetworkAddToEnd(fPackVoltageSmoothingSteps) == false) 
	  ||  (outputBuffer.NetworkAddToEnd(fBoardVoltageSmoothingSteps) == false)
	  ||  (outputBuffer.NetworkAddToEnd(fPackTemperatureSmoothingSteps) == false)
	  ||  (outputBuffer.NetworkAddToEnd(fBoardTemperatureSmoothingSteps) == false))
	  {
		  gErrorMsg(fMessageLogSettings, "WriteToImpl") << "Failed to write rev 0.7 dynamic settings to the buffer!" << endl;
		  return false;
	  }

      /* Added for 0.8: wifi 2 (DBC) power mgmt timeout */
      
      if (outputBuffer.NetworkAddToEnd(fWiFiPowerMgmtTimeoutSeconds[1]) == false)
      {
        gErrorMsg(fMessageLogSettings, "WriteToImpl") << "Failed to write rev 0.8 dynamic settings to the buffer!" << endl;
        return false;
      }

      /* Added for 0.9: MoCA power mgmt timeout */

      if (outputBuffer.NetworkAddToEnd(fMocaPowerMgmtTimeoutSeconds) == false)
      {
        gErrorMsg(fMessageLogSettings, "WriteToImpl") << "Failed to write rev 0.9 dynamic settings to the buffer!" << endl;
        return false;
      }

      /* Added for 0.10: Fault coverage (SWCM-25406) */

      if (outputBuffer.NetworkAddToEnd(fAllowedHWFaults) == false)
      {
        gErrorMsg(fMessageLogSettings, "WriteToImpl") << "Failed to write rev 0.10 dynamic settings to the buffer!" << endl;
        return false;
      }
	}
    else // kPermanentSection
    {
      /* Rev 0.1: Nothing to write */
      
      /* Added for 0.2: fNominalDischargeTime */
      if (outputBuffer.NetworkAddToEnd(fNominalDischargeTime) == false)
      {
        gErrorMsg(fMessageLogSettings, "WriteToImpl") << "Failed to write rev 0.2 permanent settings to the buffer!" << endl;
        return false;
      }
      
      /* Added for 0.3: Reset controller on poll PR 9120 (9099) */
      if (outputBuffer.NetworkAddToEnd(fResetControllerOnPoll) == false)
      {
        gErrorMsg(fMessageLogSettings, "WriteToImpl") << "Failed to write rev 0.3 permanent settings to the buffer!" << endl;
        return false;
      }
    }
   
    // If we got this far, then it worked!
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
bool BcmBatteryNonVolSettings::ReadFromImpl(const BcmOctetBuffer &inputBuffer, 
                                             unsigned int &currentOffset, 
                                             NonVolSection section)
{
    uint32 Length;

    // For this object, we skipped 0.0 for both dynamic and permanent, so this
    // should never be read.  If it is 0.0, then this is a problem, so we bail
    // with comment.
    if ((fVersionMajor[section] == 0) && (fVersionMinor[section] == 0))
    {
        gErrorMsg(fMessageLogSettings, "ReadFromImpl")
            << "Read unsupported version (0.0)!  These settings are not valid!" << endl;

        return false;
    }

    if (section == kDynamicSection)
    {
      /* Rev 0.1 */
      
      if ((inputBuffer.NetworkRead(currentOffset, fPollIntervalSeconds) == false)
      ||  (inputBuffer.NetworkRead(currentOffset, fPowerSourceEventSeconds) == false)
      ||  (inputBuffer.NetworkRead(currentOffset, fMaxChargePercent) == false))
      {
        gErrorMsg(fMessageLogSettings, "ReadFromImpl") << "Failed to read rev 0.1 dynamic settings from the buffer!" << endl;
        return false;
      }
      
      // Added for version 0.2: UPS MIB things
      if (MyVersionIsGreaterThanOrEqualTo(0, 2, section))
      {
        // Read the length of the upsIdentName, then the value itself
        if ((inputBuffer.NetworkRead(currentOffset, Length) == false)
        ||  (fUpsIdentName.ReadFrom(inputBuffer, currentOffset, Length) == false)
        // Read the length of the upsIdentAttachedDevices, then the value itself
        ||  (inputBuffer.NetworkRead(currentOffset, Length) == false)
        ||  (fUpsIdentAttachedDevices.ReadFrom(inputBuffer, currentOffset, Length) == false)
        ||  (inputBuffer.NetworkRead(currentOffset, fUpsConfigLowBattTime) == false))
        {
          gErrorMsg(fMessageLogSettings, "ReadFromImpl") << "Failed to read rev 0.2 dynamic settings from the buffer!" << endl;
          return false;
        }
      }
      
      // Added for version 0.3: fReplacementThresholdPercent
      if (MyVersionIsGreaterThanOrEqualTo(0, 3, section))
      {
        if (inputBuffer.NetworkRead(currentOffset, fReplacementThresholdPercent) == false)
        {
          gErrorMsg(fMessageLogSettings, "ReadFromImpl") << "Failed to read rev 0.3 dynamic settings from the buffer!" << endl;
          return false;
        }
      }
      
      // Added for version 0.4: per-interface power mgmt timeouts (PR 9257)
      if (MyVersionIsGreaterThanOrEqualTo(0, 4, section))
      {
        if ((inputBuffer.NetworkRead(currentOffset, fDocsisCmPowerMgmtTimeoutSeconds) == false)
        ||  (inputBuffer.NetworkRead(currentOffset, fEthernetPowerMgmtTimeoutSeconds) == false)
        ||  (inputBuffer.NetworkRead(currentOffset, fHpnaPowerMgmtTimeoutSeconds) == false)
        ||  (inputBuffer.NetworkRead(currentOffset, fUsbPowerMgmtTimeoutSeconds) == false)
        ||  (inputBuffer.NetworkRead(currentOffset, fBluetoothPowerMgmtTimeoutSeconds) == false)
        ||  (inputBuffer.NetworkRead(currentOffset, fWiFiPowerMgmtTimeoutSeconds[0]) == false))
        {
          gErrorMsg(fMessageLogSettings, "ReadFromImpl") << "Failed to read rev 0.4 dynamic settings from the buffer!" << endl;
          return false;
        }
      }
      
      // Added for 0.5: low charge threshold (PR 9726)
      if (MyVersionIsGreaterThanOrEqualTo(0, 5, section))
      {
        if (inputBuffer.NetworkRead(currentOffset, fLowChargePercent) == false)
        {
          gErrorMsg(fMessageLogSettings, "ReadFromImpl") << "Failed to read rev 0.5 dynamic settings from the buffer!" << endl;
          return false;
        }
      }
	  /* Added for 0.6: 3378 rewrite */
      if (MyVersionIsGreaterThanOrEqualTo(0, 6, section))
      {
		  if ((inputBuffer.Read(currentOffset, fLifeTestingEnabled) == false) 
		  ||  (inputBuffer.NetworkRead(currentOffset, fLifeTestPeriod) == false)
		  ||  (inputBuffer.Read( currentOffset, (uint8 *)&fBatteryInfo[0], sizeof(TNonVolBatteryInfo)) == false)
		  ||  (inputBuffer.Read( currentOffset, (uint8 *)&fBatteryInfo[1], sizeof(TNonVolBatteryInfo)) == false))
		  {
			  gErrorMsg(fMessageLogSettings, "ReadFromImpl") << "Failed to read rev 0.6 dynamic settings from the buffer!" << endl;
			  return false;
		  }
	  }
	  /* Added for 0.7*/
      if (MyVersionIsGreaterThanOrEqualTo(0, 7, section))
      {
		  if ((inputBuffer.NetworkRead(currentOffset, fPackVoltageSmoothingSteps) == false) 
		  ||  (inputBuffer.NetworkRead(currentOffset, fBoardVoltageSmoothingSteps) == false)
		  ||  (inputBuffer.NetworkRead(currentOffset, fPackTemperatureSmoothingSteps) == false)
		  ||  (inputBuffer.NetworkRead(currentOffset, fBoardTemperatureSmoothingSteps) == false))
		  {
				  gErrorMsg(fMessageLogSettings, "ReadFromImpl") << "Failed to read rev 0.7 dynamic settings from the buffer!" << endl;
				  return false;
		  }
	  }
      /* Added for 0.8*/
      if (MyVersionIsGreaterThanOrEqualTo(0, 8, section))
      {
        if (inputBuffer.NetworkRead(currentOffset, fWiFiPowerMgmtTimeoutSeconds[1]) == false)
        {
          gErrorMsg(fMessageLogSettings, "ReadFromImpl") << "Failed to read rev 0.8 dynamic settings from the buffer!" << endl;
          return false;
        }
      }
      /* Added for 0.9*/
      if (MyVersionIsGreaterThanOrEqualTo(0, 9, section))
      {
        if (inputBuffer.NetworkRead(currentOffset, fMocaPowerMgmtTimeoutSeconds) == false)
        {
          gErrorMsg(fMessageLogSettings, "ReadFromImpl") << "Failed to read rev 0.9 dynamic settings from the buffer!" << endl;
          return false;
        }
      }
      /* Added for 0.10*/
      if (MyVersionIsGreaterThanOrEqualTo(0, 10, section))
      {
        if (inputBuffer.NetworkRead(currentOffset, fAllowedHWFaults) == false)
        {
          gErrorMsg(fMessageLogSettings, "ReadFromImpl") << "Failed to read rev 0.10 dynamic settings from the buffer!" << endl;
          return false;
        }
      }
    }
    else // kPermanentSection
    {
      /* Rev 0.1: nothing to read */
      
      // Added for version 0.2: nominal discharge time
      if (MyVersionIsGreaterThanOrEqualTo(0, 2, section))
      {
        if (inputBuffer.NetworkRead(currentOffset, fNominalDischargeTime) == false)
        {
          gErrorMsg(fMessageLogSettings, "ReadFromImpl") << "Failed to read rev 0.2 permanent settings from the buffer!" << endl;
          return false;
        }
      }
      
      // Added for version 0.3: reset controller on poll PR 9120 (9099)
      if (MyVersionIsGreaterThanOrEqualTo(0, 3, section))
      {
        if (inputBuffer.NetworkRead(currentOffset, fResetControllerOnPoll) == false)
        {
          gErrorMsg(fMessageLogSettings, "ReadFromImpl") << "Failed to read rev 0.3 permanent settings from the buffer!" << endl;
          return false;
        }
      }
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
ostream & BcmBatteryNonVolSettings::PrintImpl(ostream & outputStream) const
{
    outputStream << 
      "\n\nBroadcom Battery Management Permanent NonVol Settings:\n" << endl;
      outputStream << setw(35) << "Nominal discharge time: " << NominalDischargeSecondsPerBattery() << " seconds per battery" << endl;
      //outputStream << "Reset controller on poll = " << ResetControllerOnPoll() << endl;
      
    outputStream << 
      "\n\nBroadcom Battery Management Dynamic NonVol Settings:\n" << endl;
      
    outputStream << setw(35) << "Poll interval seconds: " << PollIntervalSeconds() << endl;
    
    // With PR 9257 (per-interface delay) this is no longer used for anything.
    // We must keep it around in flash to avoid confusing the NV, but since it's
    // no longer used we will conceal it from the UI.
    // outputStream << "Power source event seconds = " << PowerSourceEventSeconds() << endl;
    
    outputStream << setw(35) <<  "MaxChargePercent: " << MaxChargePercent() << endl;
    
    outputStream << setw(35) <<  "upsIdentName: " << UpsIdentName() << endl;
    outputStream << setw(35) <<  "upsIdentAttachedDevices: " << UpsIdentAttachedDevices() << endl;
    outputStream << setw(35) <<  "upsConfigLowBattTime: " << UpsConfigLowBattTime() << " minutes" << endl;
    outputStream << setw(35) <<  "lowChargePercent: " << LowChargePercent() << " percent" << endl;
    
    outputStream << setw(35) <<  "ReplacementThresholdPercent: " << ReplacementThresholdPercent() << " percent" << endl;
    
    #if kDefaultValue_BatteryIfTimeoutSupport
    outputStream << setw(35) <<  "DocsisCm power mgmt timeout: " << DocsisCmPowerMgmtTimeoutSeconds() << " seconds" << endl;
    outputStream << setw(35) <<  "Ethernet power mgmt timeout: " << EthernetPowerMgmtTimeoutSeconds() << " seconds" << endl;
    outputStream << setw(35) <<  "Hpna power mgmt timeout: " << HpnaPowerMgmtTimeoutSeconds() << " seconds" << endl;
    outputStream << setw(35) <<  "Usb power mgmt timeout: " << UsbPowerMgmtTimeoutSeconds() << " seconds" << endl;
    outputStream << setw(35) <<  "Bluetooth power mgmt timeout: " << BluetoothPowerMgmtTimeoutSeconds() << " seconds" << endl;
    outputStream << setw(35) <<  "WiFi[0] power mgmt timeout: " << WiFiPowerMgmtTimeoutSeconds() << " seconds" << endl;
    outputStream << setw(35) <<  "WiFi[1] power mgmt timeout: " << WiFi2PowerMgmtTimeoutSeconds() << " seconds" << endl;
    outputStream << setw(35) <<  "Moca power mgmt timeout: " << MocaPowerMgmtTimeoutSeconds() << " seconds" << endl;
    #endif
      
	 if (BcmBatteryThread::Singleton().BatteryController().BspSupportsLifeTesting())
	 {
		 outputStream << setw(35) <<  "Life testing enable: " << LifeTestingEnabled() << endl; 
		 outputStream << setw(35) <<  "Life test period: " << LifeTestPeriod() << " days" << endl; 
		 outputStream << setw(35) <<  "Life test TOD: " << LifeTestTOD() << " (GMT)" << endl; 
		 for (int ix = 0; ix < MAX_BATTERIES; ix++) {       // changed from "2" to MAX_BATTERIES by PR24004
			 const TNonVolBatteryInfo *nvi = (const TNonVolBatteryInfo *)&fBatteryInfo[ix];
			 outputStream << endl;
			 if (nvi->flags & kBATTERYNVINFOVALID) {
				 outputStream << setw(35) << "Battery pack: " << ix << " Information is valid - " << endl; 
				 outputStream << setw(40) << "GUID = " << "0x" << setfill('0') << setw(16) << hex << nvi->guid << setfill(' ') << dec << endl;
				 outputStream << setw(40) << "Design capacity = " << nvi->DesignCapacity << " mAh" << endl;
				 outputStream << setw(40) << "Est. life remaining = " << (int)(nvi->EstLifeRemaining) << "%" << endl;
				 outputStream << setw(40) << "Max WattHours = " << nvi->maxWh << endl;
				 outputStream << setw(40) << "WattHour correction = " << nvi->wHCorrectionRatio << endl;
				 outputStream << setw(40) << "Dchg minutes @100% = " << (nvi->totalSeconds/60.0) << " minutes" << endl;
				 outputStream << setw(40) << "Series resistance = " << nvi->seriesResistance << " milliOhms" << endl;
				 outputStream << setw(40) << "Life test count = " << nvi->LifeTestCount << endl;
				 char outbuf[128];
				 strcpy(outbuf,ctime((const time_t *)&nvi->LastLifeTestStarted));
				 outbuf[strlen(outbuf)-1] = '\0';	// remove the newline generated by ctime()
				 outputStream << setw(40) << "Last life test started = " << outbuf << " GMT" << endl;
				 strcpy(outbuf,ctime((const time_t *)&nvi->NextUpdate));
				 outbuf[strlen(outbuf)-1] = '\0';	// remove the newline generated by ctime()
				 outputStream << setw(40) << "Next life test = " << outbuf << " GMT" << endl;
			 } else {
				 outputStream << setw(35) << "Battery pack: " << ix << " Information is not valid\n" << endl; 
			 }
		 }
	 }
	 outputStream << setw(35) <<  "Pack voltage smoothing: " << (int)PackVoltageSmoothingSteps() << " steps" << endl;
	 outputStream << setw(35) <<  "Board voltage smoothing: " << (int)BoardVoltageSmoothingSteps() << " steps" << endl;
	 outputStream << setw(35) <<  "Pack temperature smoothing: " << (int)PackTemperatureSmoothingSteps() << " steps" << endl;
	 outputStream << setw(35) <<  "Board temperature smoothing: " << (int)BoardTemperatureSmoothingSteps() << " steps" << endl;
	 outputStream << setw(35) <<  "Allowed faults: 0x" << hex << AllowedFaults() << dec << endl;

    return outputStream;
}
 
void BcmBatteryNonVolSettings::DefaultBatteryInfo()
{
	gInfoMsg(fMessageLogSettings, "DefaultBatteryInfo") << "Defaulting battery info" << endl;
    
	memset(&fBatteryInfo[0], 0, sizeof(TNonVolBatteryInfo));
	memset(&fBatteryInfo[1], 0, sizeof(TNonVolBatteryInfo));
	//fBatteryInfo[0].flags = fBatteryInfo[1].flags = 0;
	//fBatteryInfo[0].guid = fBatteryInfo[1].guid = 0ULL;
}

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
//  Filename:       BatteryController.cpp
//  Author:         Tom Johnson
//  Creation Date:  December 18, 2007
//
//****************************************************************************
//  Description:
//      High-level interface to the battery driver
//
//****************************************************************************
#include "BatteryController.h"
#include "BatteryThread.h"
#include "BatteryNonVolSettings.h"
#include "MessageLog.h"
#include "battery.h"
#include "upsAlarms.h"
#include "OperatingSystemFactory.h"
#include "MutexSemaphore.h"
#include "MergedNonVolDefaults.h"
#include "NonVolDevice.h"
#include "SystemTime.h"
#include "VendorSpecificBattery.h"
#ifdef BFC_INCLUDE_THERMAL_MONITOR_SUPPORT
	#include "ThermalMonitorThread.h"
#endif
#if (POWER_MGMT)
	#include "PowerManagementThread.h"
#endif

#if defined (TARGETOS_Linux)
#include <math.h>
#endif

#include <iomanip>

// the 2.0 rev battery subsystem does not support more than 2 batteries...period.
//#undef MAX_BATTERIES      // PR24004 - see battery.h
//#define MAX_BATTERIES 2

#ifndef kBatterySenseNumerator
	#define kBatterySenseNumerator 2
#endif
#ifndef kBatterySenseDenominator
	#define kBatterySenseDenominator 1
#endif
#ifndef kMinPackResistance
	#define kMinPackResistance 0.3
#endif
#ifndef kMaxPackResistance
	#define kMaxPackResistance 0.7
#endif
#ifndef kDefaultPackResistance
	#define kDefaultPackResistance 0.5
#endif

#ifndef kLifeTestMinVoltsPerCell
	#define kLifeTestMinVoltsPerCell 3.0
#endif
#ifndef kLifeTestMaxVoltsPerCell
	#define kLifeTestMaxVoltsPerCell 4.2
#endif

#define kThermalDecisionMessages 0x00000040

#ifndef kDefaultBatteryChargePercent
	#define kDefaultBatteryChargePercent 90
#endif

// Allow for 36 hours of samples during life test
// assuming a sample rate of 5 seconds
#define LTDischargeTableElements (36*60*60/5)

extern "C"
{
    // This is the HAL's debug control variable.
    extern uint16 DebugZone; 
}

#if 1
	#define PRT(s...) printf(s)
#else
	#define PRT(s...)
#endif

BcmMessageLogSettings *fMessageLogSettings;

static const uint8 crc8_tab[] = {
	0,94,188,226,97,63,221,131,194,156,126,32,163,253,31,65,
	157,195,33,127,252,162,64,30,95,1,227,189,62,96,130,220,
	35,125,159,193,66,28,254,160,225,191,93,3,128,222,60,98,
	190,224,2,92,223,129,99,61,124,34,192,158,29,67,161,255,
	70,24,250,164,39,121,155,197,132,218,56,102,229,187,89,7,
	219,133,103,57,186,228,6,88,25,71,165,251,120,38,196,154,
	101,59,217,135,4,90,184,230,167,249,27,69,198,152,122,36,
	248,166,68,26,153,199,37,123,58,100,134,216,91,5,231,185,
	140,210,48,110,237,179,81,15,78,16,242,172,47,113,147,205,
	17,79,173,243,112,46,204,146,211,141,111,49,178,236,14,80,
	175,241,19,77,206,144,114,44,109,51,209,143,12,82,176,238,
	50,108,142,208,83,13,239,177,240,174,76,18,145,207,45,115,
	202,148,118,40,171,245,23,73,8,86,180,234,105,55,213,139,
	87,9,235,181,54,104,138,212,149,203,41,119,244,170,72,22,
	233,183,85,11,136,214,52,106,43,117,151,201,74,20,246,168,
	116,42,200,150,21,75,169,247,182,232,10,84,215,137,107,53
};
time_t				   sysStartTime;
static bool bequite = true;

BcmBatteryController::BcmBatteryController()
{
	fpMutex = BcmOperatingSystemFactory::NewMutexSemaphore("BcmBatteryController mutex");

	if (fpMutex == NULL)
	  gLogMessageRaw << "WARNING: failed to create mutex for BcmBatteryController" << endl;

	pfMutex = BcmOperatingSystemFactory::NewMutexSemaphore("BcmBatteryThread mutex");

	if (pfMutex == NULL)
	  gLogMessageRaw << "WARNING: failed to create mutex for BcmBatteryThread" << endl;

	fInitialized = false;
	fLastUpdate = 0;
	fVersion = "Unknown";
	//fBuildDateTime = 0;
	fChargingEnabled = true;
	fOutputEnabled = false;
	//fChargingInhibited = false;
	fPackTemperatureSmoothingSteps = 1;
	fBoardTemperatureSmoothingSteps = 1;
	fPackVoltageSmoothingSteps = 1;
	fPackLowVoltageFlagLimit = 10.7 * kBatteryNumberOfCells / 3.0;
	fBoardVoltageSmoothingSteps = 1;
	SetDiagTemperature(TEMP_MEASUREMENT_NOT_SUPPORTED);
	fDiagBatteryErrors[0] = fDiagBatteryErrors[1] = -1;
	fAttachedDevices = "";
	fNumBatteries = 0;
	fNumUCS = 0;
	fNumberOfAttachedDevices = 0;
	fControllerState = 0;
	memset (&fPackStateFlags[0], 0, sizeof(struct _PackFlags));
	memset (&fPackStateFlags[1], 0, sizeof(struct _PackFlags));
	memset (&fLTInfo, 0, sizeof(TLifeTestInfo));

	// Initialize driver structure
	memset (&fInfo, 0, sizeof(TBatteryControllerInfo));
	fInfo.controllerSpecific.average_board_temperature = TEMP_MEASUREMENT_NOT_SUPPORTED;
	fInfo.Batteries[0].controllerSpecific.average_pack_temperature = TEMP_MEASUREMENT_NOT_SUPPORTED;
	fInfo.Batteries[1].controllerSpecific.average_pack_temperature = TEMP_MEASUREMENT_NOT_SUPPORTED;

	memset (&fMVLimits, 0, sizeof(TMVLimits));
	memset (&fDriverCalls,0,sizeof(TBatteryDriverCalls));
}

BcmBatteryController::~BcmBatteryController()
{
	DeInitialize();

	// Delete my mutex.
	delete pfMutex;
	pfMutex = NULL;
}

BcmBatteryController &BcmBatteryController::operator = (const BcmBatteryController &Controller)
{
	// Create a mutex auto-lock object.  This will automatically release
  // the lock when it goes out of scope (ie when we return).
  BcmMutexSemaphoreAutoLock Lock(fpMutex);

  // Copy the controller info structure from the supplied
  // object over our own.
  Controller.GetBatteryControllerInfo(fInfo);
  
  // Copy other members
  fInitialized = Controller.fInitialized;
  fLastUpdate = Controller.fLastUpdate;
  fVersion = Controller.fVersion;
  //fBuildDateTime = Controller.fBuildDateTime;
  fChargingEnabled = Controller.fChargingEnabled;
  //fChargingInhibited = Controller.fChargingInhibited;
  fOutputEnabled = Controller.fOutputEnabled;
  fPackTemperatureSmoothingSteps = Controller.fPackTemperatureSmoothingSteps;
  fBoardTemperatureSmoothingSteps = Controller.fBoardTemperatureSmoothingSteps;
  fPackVoltageSmoothingSteps = Controller.fPackVoltageSmoothingSteps;
  fPackLowVoltageFlagLimit = Controller.fPackLowVoltageFlagLimit;
  fBoardVoltageSmoothingSteps = Controller.fBoardVoltageSmoothingSteps;
  fDiagBLTTemperature = Controller.fDiagBLTTemperature;
  fDiagPLTTemperature[0] = Controller.fDiagPLTTemperature[0];
  fDiagPLTTemperature[1] = Controller.fDiagPLTTemperature[1];
  fDiagBatteryErrors[0] = Controller.fDiagBatteryErrors[0];
  fDiagBatteryErrors[1] = Controller.fDiagBatteryErrors[1];
  fAttachedDevices = Controller.fAttachedDevices;
  fNumberOfAttachedDevices = Controller.fNumberOfAttachedDevices;
  fNumBatteries = Controller.fNumBatteries;
  fNumUCS = Controller.fNumUCS;
  fControllerState = Controller.fControllerState;
  memcpy (&fPackStateFlags[0], &Controller.fPackStateFlags[0], sizeof(struct _PackFlags));
  memcpy (&fPackStateFlags[1], &Controller.fPackStateFlags[1], sizeof(struct _PackFlags));
  memcpy (&fMVLimits, &Controller.fMVLimits, sizeof(TMVLimits));
  memcpy (&fLTInfo, &Controller.fLTInfo, sizeof(TLifeTestInfo));

  return *this;
}

int BcmBatteryController::NumberSeriesCellsSupported()
{
	return (int)bspNumSeriesCellsSupported();
}

void BcmBatteryController::GetBatteryControllerInfo (TBatteryControllerInfo &Info) const
{
	memcpy (&Info, &fInfo, sizeof(TBatteryControllerInfo));
}

bool BcmBatteryController::Initialize ()
{
	if (fInitialized) DeInitialize();
	sysStartTime = GetSystemStartTime();

	fInfo.PollSeconds = BcmBatteryNonVolSettings::GetSingletonInstance()->PollIntervalSeconds();
	fInfo.ChargePercent = BcmBatteryNonVolSettings::GetSingletonInstance()->MaxChargePercent();

	// SWCM-21130
	if(fInfo.PollSeconds > 120) fInfo.PollSeconds = 5;
	if(fInfo.ChargePercent > 100) fInfo.ChargePercent = kDefaultBatteryChargePercent;
	SmoothingSteps(kBoardTempSmoothingSteps,BcmBatteryNonVolSettings::GetSingletonInstance()->BoardTemperatureSmoothingSteps());
	SmoothingSteps(kBoardVoltageSmoothingSteps,BcmBatteryNonVolSettings::GetSingletonInstance()->BoardVoltageSmoothingSteps());
	SmoothingSteps(kPackTempSmoothingSteps,BcmBatteryNonVolSettings::GetSingletonInstance()->PackTemperatureSmoothingSteps());
	SmoothingSteps(kPackVoltageSmoothingSteps,BcmBatteryNonVolSettings::GetSingletonInstance()->PackVoltageSmoothingSteps());
//	gLogMessageRaw << "setting smoothing to: packTemp = " << fPackTemperatureSmoothingSteps << ", boardTemp = " << fBoardTemperatureSmoothingSteps
//				   << ", pack voltage = " << fPackVoltageSmoothingSteps << ", board voltage = " << fBoardVoltageSmoothingSteps << endl;
	UpdateMaxHistory(fInfo.PollSeconds);
	bspBatInit(&fInfo, &fDriverCalls);
	if( fDriverCalls.pfMainsVoltages) {
		fDriverCalls.pfMainsVoltages(&fMVLimits);
	}
	SetFaultMask(BcmBatteryNonVolSettings::GetSingletonInstance()->AllowedFaults());
	fVersion = fInfo.Version;
	// always assume charging and discharging are enabled.
	fChargingEnabled = true;
	//fChargingInhibited = false;
	fOutputEnabled = true;
	fInitialized = true;
	fMessageLogSettings = &BcmBatteryThread::Singleton().fMessageLogSettings;
	fMessageLogSettings->SetEnabledFields(BcmMessageLogSettings::kTimestampField | BcmMessageLogSettings::kFunctionNameField);
//	fMessageLogSettings->SetEnabledSeverities(BcmMessageLogSettings::kInformationalMessages | BcmMessageLogSettings::kFatalErrorMessages | BcmMessageLogSettings::kErrorMessages | BcmMessageLogSettings::kWarningMessages);

	if ( ALLOW_UNTRIMMED_PARTS == TRUE )
	{
		cout << "WARNING: Code is compiled with ALLOW_UNTRIMMED_PARTS set to TRUE" << endl;
		cout << "WARNING: Production code MUST NOT have this setting set to TRUE" << endl;
		cout << "WARNING: Production code MUST have ALLOW_UNTRIMMED_PARTS set to FALSE" << endl;
		cout << "WARNING: Setting ALLOW_UNTRIMMED_PARTS to TRUE is done at your own risk" << endl;
	}
	else
	{
		if( fDriverCalls.pfPartIsTrimmed && fDriverCalls.pfPartIsTrimmed() == FALSE )
		{
			cout << "Battery Controller is disabled: Chip is not trimmed (calibrated)" << endl;
			cout << "Consult the bmud Makefile to temporarily change this behavior" << endl;
		}
	}

	return true;
}

bool BcmBatteryController::DeInitialize ()
{
	memset (&fInfo, 0, sizeof(TBatteryControllerInfo));
	memset (&fDriverCalls,0,sizeof(TBatteryDriverCalls));
	if(fLTInfo.LTData) {
		delete [] fLTInfo.LTData;
		fLTInfo.LTData = NULL;
	}
	fInitialized = false;
	return bspBatDeinit(&fInfo);
}

bool BcmBatteryController::LifeTestingEnabled() const
{
	return BcmBatteryNonVolSettings::GetSingletonInstance()->LifeTestingEnabled();
}

void BcmBatteryController::CheckUpdateNextLifeTest()
{
	static bool RuleAApplied = false, RuleBApplied = false;

	time_t Now, newStartTime = GetSystemStartTime();
	uint32 upTimeSecs = 0, ltPeriodSecs = 0; 
	time (&Now);
	uint32 ltPeriodDays = BcmBatteryNonVolSettings::GetSingletonInstance()->LifeTestPeriod();
	if(ltPeriodDays > 365) ltPeriodDays = 180;	// SWCM-21130
	ltPeriodSecs = ltPeriodDays * 86400;
	//gInfoMsg(*fMessageLogSettings, "CheckUpdateNextLifeTest") << "Enter...(@" << ctime(&Now) << ")" << endl;
	if( !RuleAApplied && (newStartTime != sysStartTime) )
	{
		// Rule A: start time changed - check each pack's next test time.  It it is
		// more than 10X LifeTestPeriods in the past, adjust to time() + (test_time - time_t(0))
		// This handles the case where a battery was discovered/sceduled prior to TOD completing
		//gLogMessageRaw << "CheckUpdateNextLifeTest: Rule A - sysStartTime changed from" << ctime(&sysStartTime) << " to " << ctime(&newStartTime) << endl;
		for( int ix = 0; ix <= 1; ix++)
		{
			TNonVolBatteryInfo *nvi = BcmBatteryNonVolSettings::GetSingletonInstance()->GetBattery(ix);
			if (nvi->flags & kBATTERYNVINFOVALID)
			{
				time_t nextUpdate = nvi->NextUpdate;
				float diff = difftime(Now, nextUpdate);
				//gLogMessageRaw << "CheckUpdateNextLifeTest: Rule A - Battery " << ix << ": information is valid, diff = " << diff << "s." << endl;
				// diff will be negative if nextUpdate is in the future relative to Now
				if (diff > (ltPeriodSecs * 10))
				{
					// nextUpdate should always be relative to time(0) in this instance, but if it represents more than
					// ltPeriodSecs, we'll rescedule for <Period> in the future.  This could happen if the modem was
					// operating for a long time on a system that didn't support TOD and was moved to one that does
					//gInfoMsg(*fMessageLogSettings, "CheckUpdateNextLifeTest") << "CheckUpdateNextLifeTest: Battery " << ix << ": diff > 10X..." << endl;
					if (difftime(nextUpdate,(time_t)ltPeriodSecs) > ltPeriodSecs) 
						RescheduleByNVI(ix, ltPeriodSecs/86400);
					else
					{
						if ((uint32)nextUpdate < 86400) nextUpdate = (time_t)86400;
						RescheduleByNVI(ix, ((uint32)nextUpdate+86399)/86400);
					}

					gInfoMsg(*fMessageLogSettings, "CheckUpdateNextLifeTest") << "Rescheduling nv[" << ix << "] nextUpdate from " << ctime(&nextUpdate) << " to " << ctime(&nvi->NextUpdate) << endl;
				}
			}
		}
		RuleAApplied = true;
	}

	upTimeSecs = Now - newStartTime;
	if (!RuleBApplied && (upTimeSecs > ltPeriodSecs ))
	{
		// Rule B: system has been up for more than LifeTestPeriod days.  
		// check each pack's next test time.  It it is due any later than 
		// Now() + (LifeTestPeriodSeconds * 10), reschedule +1 day from now.
		// This handles the case where a battery was discovered/scheduled while
		// TOD was operating and TOD is no longer being used.
		//gInfoMsg(*fMessageLogSettings, "CheckUpdateNextLifeTest") << "CheckUpdateNextLifeTest: sysUpTime seconds (" << upTimeSecs << ") exceeds ltPeriodSecs (" << ltPeriodSecs << ")" << endl;
		for( int ix = 0; ix <= 1; ix++)
		{
			TNonVolBatteryInfo *nvi = BcmBatteryNonVolSettings::GetSingletonInstance()->GetBattery(ix);
			if (nvi->flags & kBATTERYNVINFOVALID)
			{
				time_t nextUpdate = nvi->NextUpdate;
				float diff = difftime(nextUpdate, Now);
				// diff will be negative if nextUpdate is in the past relative to Now
				if (diff > (ltPeriodSecs * 10))
				{
					RescheduleByNVI(ix,1);
					gInfoMsg(*fMessageLogSettings, "CheckUpdateNextLifeTest") << "Rescheduling nv[" << ix << "] nextUpdate from " << ctime(&nextUpdate) << " to " << ctime(&nvi->NextUpdate) << endl;
				}
			}
		}
		RuleBApplied = true;
	}
	//gInfoMsg(*fMessageLogSettings, "CheckUpdateNextLifeTest") << "Exit..." << endl;
}

bool BcmBatteryController::Update()
{
	if (!fInitialized) return true;
	if( fDriverCalls.pfPartIsTrimmed && fDriverCalls.pfPartIsTrimmed() == FALSE && ALLOW_UNTRIMMED_PARTS == FALSE )
	{
		return true;
	}
	TBatteryControllerInfo xInfo;		// use a local version to update in case this fails
	memcpy(&xInfo, &fInfo, sizeof(TBatteryControllerInfo));

	// Create a mutex auto-lock object.  This will automatically release
	// the lock when it goes out of scope (ie when we return).
	BcmMutexSemaphoreAutoLock Lock(pfMutex);
	if( bspBatUpdate(&xInfo) )
	{
		SmoothInputVoltage(xInfo);
		CalculateBoardTemperature(xInfo);	// this must be done before the packs
		SmoothPackVoltages(xInfo);
		SmoothPackTemperatures(xInfo);
		memcpy(&fInfo, &xInfo, sizeof(TBatteryControllerInfo));
	}
	else
	{
		gInfoMsg(*fMessageLogSettings, "Update") << "BSP returned FALSE from update call" << endl;
		return true;
	}

	for( int ix = 0; ix < MAX_BATTERIES; ix++ )
	{
		if( fInfo.Batteries[ix].updateInfo.Status & kBatStatusPresenceChanged )
		{
			if( fInfo.Batteries[ix].updateInfo.Status & kBatStatusPresent )
			{
				if( fInfo.Support & kSupportsBfcPackValidation )
				{
				#ifdef VENDOR_SUPPLIED_PACK_VALIDATION_FUNCTION
					VENDOR_SUPPLIED_PACK_VALIDATION_FUNCTION(fInfo,ix);
				#else
					ValidateBattery(fInfo,ix);
				#endif
				}
				if( BatteryValid(ix) 
				 && (fInfo.Support & kSupportsLifeTesting)
				 && (CheckNonVolInfo(ix) >= 0) ) 
					RescheduleAtLeast(ix,1);
			}
		}
		if (fInfo.Support & kSupportsLifeTesting) CheckLifeTest(ix);
	}
	history.AddHistory(fInfo);
	CheckAlerts();
	if (fInfo.Support & kSupportsLifeTesting && LifeTestingEnabled()) 
	{
		CheckUpdateNextLifeTest();
		ServiceLifeTest();
	}
	fLastUpdate = time(0);
	return true;
}

BatteryControllerState BcmBatteryController::State () const
{
	return fInfo.State;
}

int	 BcmBatteryController::ControllerState() const
{
	return fControllerState; 
}


bool IsChargeState(BatteryControllerState state)
{
	return (state >= kBCPrecharge) && (state <= kBCTopoff);
}

bool IsDischargeState( BatteryControllerState state )
{
	return (state >= kBCDischarge) && (state <= kBCForcedDischarge);
}

bool BcmBatteryController::IsInDischargeState() const
{
	return IsDischargeState(fInfo.State);
}

bool BcmBatteryController::OperatingOnBattery () const
{
	return (fInfo.State == kBCDischarge) || (fInfo.State == kBCEternalSleep);
}

int BcmBatteryController::PercentChargeRemaining () const
{
	float result = 0;	// indicates no valid, known charge state batteries are present
	int foundBatts = 0;
	if (fInfo.NumBatteries == 0) {
		return 0;		// return 0% when no batteries are present
	}
	for (int ix = 0; ix < MAX_BATTERIES; ix++ ) 
	{
		uint16 status = fInfo.Batteries[ix].updateInfo.Status;
		if( (status & (kBatStatusValid | kBatStatusUCS)) == kBatStatusValid)
		{
			result += VToPct(fInfo.Batteries[ix].dischargeTable, fInfo.Batteries[ix].controllerSpecific.average_pack_voltage);
			foundBatts++;
		}
		//else
		//	gInfoMsg(*fMessageLogSettings, "PctChargeRemaining") << "Battery " << ix << " is either not valid or has unknown charge state" << endl;
	}
	if(foundBatts == 0) return 0;	// return 0 when all packs are either invalid or have unknown charge states
	result = result/foundBatts;		// overall % = average of all batteries that are present
	if (result < 0) result = 0;
	if (result > 100) result = 100;
	return (int)result;
}

int BcmBatteryController::InputVoltage () const
{
	return (int)(fInfo.InputVoltage);
}

/// Indicates the current temperature at or near the battery in degrees C.  
///  This is always the value upsTemperature.
/// \returns
///     The temperature at or near the battery, expressed in degrees C.
///     Returns TEMP_MEASUREMENT_NOT_SUPPORTED if not measurable.
///
/***** 
  There are 2 types of thermal limits packs need to be concerned with:  ChargeSuspension and DischargeSuspension
  For each of these, there are both a high and a low limit.  The only restriction on these programmable
  limits are that the low limit MUST be 4 or more degrees less than the high limit (due to the 3degC hysteresis).
 
  If a diagnostic temperature is set, we ALWAYS use this temperature
  Otherwise if the packs don't support pack thermistors, or if no pack is present, we will ALWAYS use the
		board temperature.  (Board temperature should default to the Calibrated BLT.  We will use the TSC ONLY if it is
		present and the BLT doesn't have a valid temperature).
  Assuming neither of the above are true, we need to figure out which pack's temperature to return.
		If only one pack is valid, we'll use that pack's average temperature.
		Otherwise:
			If a pack is currently selected and is being actively charged or discharged, we'll choose that pack's average temperature.
		Otherwise:
			we'll start by accumulating temperatures and the amounts by which these temperatures exceed the various
			limits.  Once this is done, if neither pack exceeded any limits, we'll pick the temperature of the pack that is
			furthest from the CHARGE limit. 
		Otherwise we'll pick the pack that exceeds the fewest limits, 
		Finally, if both packs exceed the same number of limits, we'll pick the pack that exceeded its CHARGE limit by the least
*****/
int BcmBatteryController::Temperature () const
{
	//if (fDiagTemperature != TEMP_MEASUREMENT_NOT_SUPPORTED)
	//	return fDiagTemperature;							// return diag temp if specified
	
	// default is the pseudo pack temperature
	int pseudoTemp = (int)fInfo.controllerSpecific.average_pseudo_pack_temperature;
	int chosenPack = -1;
	
	if ((fInfo.Support & kSupportsPackThermistors) && (NumberOfValidatedBatteries() > 0) ) {
		struct _ptInfo {
		  bool valid;
		  int temperature;
		  int chg_delta, dchg_delta;
		  int numLimitsExceeded;
		} ptInfo[2];
		TBatteryThermalInfo tempThermalInfo;

		BatteryControllerState state = fInfo.State;
		int 	tt;
		
		for (int ix = 0; ix < 2; ix++)
		{
			memset(&ptInfo[ix], 0, sizeof(struct _ptInfo));

			ptInfo[ix].valid = (((fInfo.Batteries[ix].updateInfo.Status >> kPackStatusValidityShift) & kPackStatusValidityMask) == kPackStatusValidityValid);
			tt = ptInfo[ix].temperature = (int)PackTemperature(ix);
			if(ptInfo[ix].valid && (tt != TEMP_MEASUREMENT_NOT_SUPPORTED)) {
				// always use the temperature of a currently charging/discharging pack
				if ((IsChargeState(state) || IsDischargeState(state)) 
				&& fInfo.Batteries[ix].updateInfo.Status & kBatStatusSelected)
				{
					gInfoMsg(*fMessageLogSettings, "Temperature") << "Returning temperature of selected pack (" << ix << ")" << endl;
					return tt;	// early out!
				}
				
				// collect deltas and # limits exceeded
				if(fInfo.Support & kSupportsPerPackEnables)
					memcpy(&tempThermalInfo, &fInfo.Batteries[ix].thermalInfo, sizeof(TBatteryThermalInfo));
				else 
				{
					tempThermalInfo.Coff_Lo = kDefaultValue_BatteryChargeSuspensionLoTemp;
					tempThermalInfo.Coff_Hi = kDefaultValue_BatteryChargeSuspensionHiTemp;
					tempThermalInfo.Doff_Lo = kDefaultValue_BatteryDischargeSuspensionLoTemp;
					tempThermalInfo.Doff_Hi = kDefaultValue_BatteryDischargeSuspensionHiTemp;
				}
				if(abs(tt - tempThermalInfo.Coff_Lo) <= abs(tempThermalInfo.Coff_Hi - tt))
					ptInfo[ix].chg_delta = tt - tempThermalInfo.Coff_Lo;
				else
					ptInfo[ix].chg_delta = tempThermalInfo.Coff_Hi - tt;
				if(abs(tt - tempThermalInfo.Doff_Lo) <= abs(tempThermalInfo.Doff_Hi - tt))
					ptInfo[ix].dchg_delta = tt - tempThermalInfo.Doff_Lo;
				else
					ptInfo[ix].dchg_delta = tempThermalInfo.Doff_Hi - tt;
				
				if (ptInfo[ix].chg_delta < 0) ptInfo[ix].numLimitsExceeded++;
				if (ptInfo[ix].dchg_delta < 0) ptInfo[ix].numLimitsExceeded++;
			}
		}
		if(!bequite && (fMessageLogSettings->GetEnabledSeverities() & kThermalDecisionMessages))
		{
			uint8 settings = fMessageLogSettings->GetEnabledFields();
			gInfoMsg(*fMessageLogSettings, "Temperature") << "ptInfo:" << endl;
			fMessageLogSettings->SetEnabledFields(0);
			for(int ix = 0; ix < 2; ix++)
			{
				gInfoMsg(*fMessageLogSettings,"") << "  ptInfo[" << ix << "]:" << endl;
				gInfoMsg(*fMessageLogSettings,"") << "      valid: " << boolalpha << ptInfo[ix].valid << endl;
				if(ptInfo[ix].valid)
				{
					gInfoMsg(*fMessageLogSettings,"") << "       temp: " << ptInfo[ix].temperature << endl;
					gInfoMsg(*fMessageLogSettings,"") << "   chgdelta: " << ptInfo[ix].chg_delta << endl;
					gInfoMsg(*fMessageLogSettings,"") << "  dchgdelta: " << ptInfo[ix].dchg_delta << endl;
					gInfoMsg(*fMessageLogSettings,"") << "    xlimits: " << ptInfo[ix].numLimitsExceeded << endl;
				}
			}
			fMessageLogSettings->SetEnabledFields(settings);
		}

		// if only one pack is valid, use that pack's temperature
		if(ptInfo[0].valid && !ptInfo[1].valid)	chosenPack = 0;
		else if(ptInfo[1].valid && !ptInfo[0].valid) chosenPack = 1;

		if(chosenPack < 0)
		{
			// if NEITHER pack exceeded any limits, use the one that is furthest from a exceeding the CHARGE limit
			if ((ptInfo[0].numLimitsExceeded + ptInfo[1].numLimitsExceeded) == 0 ) 
				chosenPack = (ptInfo[0].chg_delta >= ptInfo[1].chg_delta) ? 0 : 1;
			if(chosenPack < 0)
			{
				// if ONE pack exceeded fewer limits, use that pack's temperature
				if(ptInfo[0].numLimitsExceeded < ptInfo[1].numLimitsExceeded ) 
					chosenPack = 0;
				else if(ptInfo[1].numLimitsExceeded < ptInfo[0].numLimitsExceeded ) 
					chosenPack = 1;
				if(chosenPack < 0)
				{
					// when both packs exceeded the same number of limits, report the pack that was CLOSEST to the charge limit
					chosenPack = (ptInfo[0].chg_delta >= ptInfo[1].chg_delta) ? 0 : 1;
				}
			}
		}
		if(chosenPack >= 0)
		{
			gInfoMsg(*fMessageLogSettings, "Temperature") << "Choosing temperature from pack " << chosenPack << endl;
			return ptInfo[chosenPack].temperature;
		}
	}
	else if(fDiagPLTTemperature[0] != TEMP_MEASUREMENT_NOT_SUPPORTED || fDiagPLTTemperature[1] != TEMP_MEASUREMENT_NOT_SUPPORTED)
	{
		gInfoMsg(*fMessageLogSettings, "Temperature") << "returning diag temperature" << endl;
		if(fDiagPLTTemperature[0] != TEMP_MEASUREMENT_NOT_SUPPORTED) return fDiagPLTTemperature[0];
		return fDiagPLTTemperature[1];
	}

	gInfoMsg(*fMessageLogSettings, "Temperature") << "Choosing pseudo pack temperature" << endl;
	return pseudoTemp;
}

int BcmBatteryController::NominalDischargeTimePerBattery () const		// in MINUTES!
{
	const TBatDischargeTable *table;
	table = kBatteryNumberOfCells == 2	? &default2cell2200mAhDischargeTable 
										: &default3cell2200mAhDischargeTable;
	float maxWattHours = PctToWh(table,100);
	return (int)((maxWattHours * 60)/kNOMINAL_DCHG_WATTS);
}

bool BcmBatteryController::UpdateState ()
{
	// Create a mutex auto-lock object.  This will automatically release
	// the lock when it goes out of scope (ie when we return).
	BcmMutexSemaphoreAutoLock Lock(pfMutex);

	if(fInitialized) fInfo.State = bspCurrentState();
	return true;
}

int BcmBatteryController::NumberOfPresentBatteries () const
{
	return fInfo.NumBatteries;
}

int BcmBatteryController::NumberUCSBatteries () const
{
	int result = 0;
	for(int ix = 0; ix < MAX_BATTERIES; ix++) {		// PR24004
		if( fInfo.Batteries[ix].updateInfo.Status & kBatStatusUCS ) result++;
	}
	return result;
}

int BcmBatteryController::NumberOfValidBatteries () const
{
	return NumberOfPresentBatteries() - NumberUCSBatteries();
}

int BcmBatteryController::NumberOfValidatedBatteries() const
{
	int result = 0;
	for(int ix = 0; ix < MAX_BATTERIES; ix++) {		// PR24004
		if( ((fInfo.Batteries[ix].updateInfo.Status >> kPackStatusValidityShift) & kPackStatusValidityMask) == kPackStatusValidityValid ) {
			result++;
		}
	}
	return result;
}

int BcmBatteryController::NumberOfValidatedBatteriesWithoutErrors() const
{
	int result = 0;
	for(int ix = 0; ix < MAX_BATTERIES; ix++) {
		if(((fInfo.Batteries[ix].updateInfo.Status >> kPackStatusValidityShift) & kPackStatusValidityMask) == kPackStatusValidityValid) {
			if(BatteryErrors(ix) == 0) {
				result++;
			}
		}
	}
	return result;
}

unsigned long BcmBatteryController::BatteryErrors (int Battery) const
{
//	return (fInfo.Batteries[Battery].updateInfo.Status >> kPackStatusErrorShift) & kPackStatusErrorMask;
	if (fDiagBatteryErrors[Battery] >= 0)
		return fDiagBatteryErrors[Battery];
	else
		return fInfo.Batteries[Battery].updateInfo.Errors;
}

int BcmBatteryController::BatteryChargeCapacity (int Battery) const
{
	int result = 0;
	if (BatteryValid(Battery))
	{
		result = (int)fInfo.Batteries[Battery].dischargeTable->designCapacity;
	}
	return result;
}

int BcmBatteryController::BatteryActualCapacity( int Battery) const
{
	int result = 0;
	if (BatteryValid(Battery))
	{
		result = (int)fInfo.Batteries[Battery].dischargeTable->actualCapacity;
	}
	return result;
}

void BcmBatteryController::DumpBatteryDischargeTable(int whichBat)
{
	if(whichBat < MAX_BATTERIES)	// PR24004
	{
	#if !(BCM_REDUCED_IMAGE_SIZE)
		DumpDischargeTable(fInfo.Batteries[whichBat].dischargeTable);
	//#else
		//gLogMessageRaw << "Sorry...reduced image size" << endl;
	#endif
	}
	else
		gLogMessageRaw << "Specified battery not installed";
}

int BcmBatteryController::BatteryFullChargeVoltage (int Battery) const
{
	if(Battery < MAX_BATTERIES)		// PR24004
		return (int)(PctToV(fInfo.Batteries[Battery].dischargeTable, 100.0) * 1000);
	return 0;
}

int BcmBatteryController::BatteryDepletedVoltage (int Battery) const
{
	if(Battery < MAX_BATTERIES)		// PR24004
		return (int)(PctToV(fInfo.Batteries[Battery].dischargeTable, 0) * 1000);
	return 0;
}

// returns voltage in millivolts!
int BcmBatteryController::BatteryMeasuredVoltage (int Battery) const
{
	if(Battery < MAX_BATTERIES)		// PR24004
		return (int)(fInfo.Batteries[Battery].controllerSpecific.average_pack_voltage * 1000);
	return 0;
}

int BcmBatteryController::BatteryPercentCharge (int Battery) const
{
	if(Battery < MAX_BATTERIES)		// PR24004
	{
		int pct = (int)VToPct( fInfo.Batteries[Battery].dischargeTable, 
							fInfo.Batteries[Battery].controllerSpecific.average_pack_voltage);

		//gInfoMsg(*fMessageLogSettings, "BatteryPercentCharge") << "Battery " << Battery << " v = " << fInfo.Batteries[Battery].controllerSpecific.average_pack_voltage 
		//	<< ", Pct = " << pct << endl;
		
		return pct;
	}
	return 0;
}

void BcmBatteryController::SetGlobalPercentCharge (int pct)
{
	if (pct > 100) pct = 100;
	if( pct < 0)   pct = 0;
	if( fDriverCalls.pfSetChargePercentageAll)
		fDriverCalls.pfSetChargePercentageAll(&fInfo, pct);
}

int BcmBatteryController::BatteryEstimatedMinutesRemaining (int Battery) const
{
	if (!BatteryPresent(Battery) || !BatteryValid(Battery)) 
	{
		//gInfoMsg(*fMessageLogSettings, "BatteryEstimatedMinutesRemaining") << "Battery " << Battery << " is either not present or not valid - returning 0" << endl; 
		return 0;
	}
	float packVolts = fInfo.Batteries[Battery].controllerSpecific.average_pack_voltage;
	float pctChg = VToPct( fInfo.Batteries[Battery].dischargeTable, packVolts);
	float wattHours = PctToWh( fInfo.Batteries[Battery].dischargeTable, pctChg);
	float battWatts = kNOMINAL_DCHG_WATTS;
	if( (fInfo.Support & kSupportsDischargeCurrent) && IsDischargeState(fInfo.State ) )
	{
		if(fInfo.BatteryCurrent < -0.100)
		{
			battWatts = fabs(fInfo.BatteryCurrent * packVolts)/1000;
			//cout <<  "bat " <<  Battery << " v = " << packVolts << ", i = " << fInfo.BatteryCurrent << ", wH = " << wattHours << ", w = " << battWatts << ", % = " << pctChg << endl; 
		}
		//else
		//{
		//	cout << "bypassing recalculation of batteryWatts because IBAT > -100mA" << endl;
		//}
	}
	//gInfoMsg(*fMessageLogSettings, "BatteryEstimatedMinutesRemaining") << "Battery " << Battery << " v = " << packVolts << ", wH = " << wattHours << ", w = " << battWatts << ", % = " << pctChg << endl; 
	//cout << "EMR[" << Battery << "] = " << (int)((wattHours * 60)/battWatts) << endl;
	return (int)((wattHours * 60)/battWatts);
}

bool BcmBatteryController::BatteryPresent (int Battery) const
{
	return fInfo.Batteries[Battery].updateInfo.Status & kBatStatusPresent;
}

bool BcmBatteryController::BatteryValid (int Battery) const
{
	return (fInfo.Batteries[Battery].updateInfo.Status & kBatStatusValid) != 0;
}

bool BcmBatteryController::BatteryBad (int Battery) const
{
	int replacementThresholdPct = BcmBatteryNonVolSettings::GetSingletonInstance()->ReplacementThresholdPercent();
	if(replacementThresholdPct < 0 || replacementThresholdPct > 100) replacementThresholdPct = 55;	// reset default if corrupted
	if(((fInfo.Batteries[Battery].updateInfo.Status >> kPackStatusValidityShift) & kPackStatusValidityMask) == kPackStatusValidityInvalid)
	{
		gInfoMsg(*fMessageLogSettings, "BatteryBad") << "Pack is bad because it is invalid." << endl;
		return true; 
	}
	else if(BatteryErrors(Battery) > 0)
	{
		gInfoMsg(*fMessageLogSettings, "BatteryBad") << "Pack is bad because it is has charge errors." << endl;
		return true; 
	}
	else if(SupportsLifeTesting() 
		 && LifeTestingEnabled() 
		 && (fInfo.Batteries[Battery].dischargeTable->actualCapacity / fInfo.Batteries[Battery].dischargeTable->designCapacity) < (replacementThresholdPct / 100.0))
	{
		gInfoMsg(*fMessageLogSettings, "BatteryBad") << "Pack is bad because it's capacity has degraded below the replacement threshold." << endl;
		return true; 
	}
	return false;
}

bool BcmBatteryController::BatterySelected (int Battery) const
{
	return fInfo.Batteries[Battery].updateInfo.Status & kBatStatusSelected;
}

bool BcmBatteryController::BatteryFullyCharged (int Battery) const
{
	return fInfo.Batteries[Battery].updateInfo.Status & kBatStatusFull;
}

bool BcmBatteryController::BatteryChargeLow (int Battery) const
{
//	gLogMessageRaw << "BatteryChargeLow:: avgPackV = " << fInfo.Batteries[Battery].controllerSpecific.average_pack_voltage << ", flagLimit = " << fPackLowVoltageFlagLimit << endl;
	return fInfo.Batteries[Battery].controllerSpecific.average_pack_voltage < fPackLowVoltageFlagLimit;
}

bool BcmBatteryController::BatteryChargeLowPercent (int Battery) const
{
	int pct = BcmBatteryNonVolSettings::GetSingletonInstance()->LowChargePercent();
	if(pct < 0 || pct > 100) pct = kDefaultValue_BatteryLowChargePercent;	// guard agains corrupt non-vol
	return BatteryPercentCharge(Battery) <= pct;
}

bool BcmBatteryController::BatteryChargeDepleted (int Battery) const
{
	return fInfo.Batteries[Battery].updateInfo.Status & kBatStatusDepleted;
}

int BcmBatteryController::EstimatedMinutesRemaining (bool NormalizeToLow) const
{
	int result = 0;
	for(int ix = 0; ix < MAX_BATTERIES; ix++) {
		result += BatteryEstimatedMinutesRemaining(ix);
	}
	return result;
}

int BcmBatteryController::BatteryCurrent(void) const
{
	return fInfo.BatteryCurrent;	// mA
}

bool BcmBatteryController::BatteryEnableCharging (bool Enable, bool Thermal, bool Silent)
{
	EnableChargeAll(Enable, Thermal ? kReasonThermal : kReasonManual);
	return true;
}

bool BcmBatteryController::BatterySuspendCharging (bool Suspend)
{
	bspSuspendCharging(Suspend);
	return true;
}

bool BcmBatteryController::BatterySupportsPackThermistors(void)
{
	return fInfo.Support & kSupportsPackThermistors;
}

bool BcmBatteryController::BatteryEnableOutput (bool Enable, bool Thermal, bool silent)
{
	EnableDischargeAll(Enable, Thermal ? kReasonThermal : kReasonManual);
	return true;
}

// A low battery condition will be reported if estimated minutes remaining
// falls below the NV setting for low battery threshold (or, in UPS MIB
// parlance, if upsEstimatedMinutesRemaining < upsConfigLowBattTime)
void BcmBatteryController::CheckForLowBattery(bool avoidHysteresis)
{
	if (NumberOfValidBatteries() == 0) return;
	
	int Threshold = BcmBatteryNonVolSettings::GetSingletonInstance()->UpsConfigLowBattTime();
	if ((EstimatedMinutesRemaining(false) <= Threshold)
	&&  ((fControllerState & kBIT_BatteryLow) == 0))
	{
		if ((OperatingOnBattery() == true)
		||  (avoidHysteresis == false))
		{
			UpdateState(kBIT_BatteryLow, true, BcmBatteryThread::kBatteryLow, BcmBatteryThread::kBatteryNotLow);
		}
	}
	else if ((EstimatedMinutesRemaining(false) > Threshold)
	&&  ((fControllerState & kBIT_BatteryLow) != 0))
	{
		if ((OperatingOnBattery() == false)
		||  (avoidHysteresis == false))
		{
			UpdateState(kBIT_BatteryLow, false, BcmBatteryThread::kBatteryLow, BcmBatteryThread::kBatteryNotLow);
		}
	}
}

uint32 BcmBatteryController::BatteryLifeTestCount(int whichBat) const
{
	if ((whichBat >= 0) 
	 && (whichBat < MAX_BATTERIES) 		// PR24004
	 && (fInfo.Support & kSupportsLifeTesting)
	 && BatteryValid(whichBat)) {
		TNonVolBatteryInfo *nvi = BcmBatteryNonVolSettings::GetSingletonInstance()->GetBattery(fInfo.Batteries[whichBat].nvIndex);
		if(nvi->flags & kBATTERYNVINFOVALID) {
			return nvi->LifeTestCount;
		}
	}
	return 0;
}

time_t BcmBatteryController::BatteryLastLifeTest(int whichBat) const
{
	if ((whichBat >= 0) 
	 && (whichBat < MAX_BATTERIES) 		// PR24004
	 && (fInfo.Support & kSupportsLifeTesting)
	 && BatteryValid(whichBat)) {
		TNonVolBatteryInfo *nvi = BcmBatteryNonVolSettings::GetSingletonInstance()->GetBattery(fInfo.Batteries[whichBat].nvIndex);
		if(nvi->flags & kBATTERYNVINFOVALID) {
			return nvi->LastLifeTestStarted;
		}
	}
	return 0;
}

BcmString BcmBatteryController::BatteryLastLifeTestString(int whichBat) const
{
    time_t LastLifeTest = BatteryLastLifeTest(whichBat);
    if (LastLifeTest) {
		struct tm *t = gmtime(&LastLifeTest);
		char buff[64];
		strftime(buff, 64, "%m/%d/%Y %H:%M:%S", t);
		return buff;
	}
	return "None";
}

time_t BcmBatteryController::BatteryNextLifeTest(int whichBat) const
{
	if ((whichBat >= 0) 
	 && (whichBat < MAX_BATTERIES)
	 && (fInfo.Support & kSupportsLifeTesting)
	 && BatteryValid(whichBat)) {
		TNonVolBatteryInfo *nvi = BcmBatteryNonVolSettings::GetSingletonInstance()->GetBattery(fInfo.Batteries[whichBat].nvIndex);
		if(nvi->flags & kBATTERYNVINFOVALID) {
			return nvi->NextUpdate;
		}
	}
	return 0;
}

BcmString BcmBatteryController::BatteryNextLifeTestString(int whichBat) const
{
	BcmString result = "";
	if ((whichBat >= 0) 
	 && (whichBat < MAX_BATTERIES) 		// PR24004
	 && (fInfo.Support & kSupportsLifeTesting)
	 && BatteryValid(whichBat)) {
		TNonVolBatteryInfo *nvi = BcmBatteryNonVolSettings::GetSingletonInstance()->GetBattery(fInfo.Batteries[whichBat].nvIndex);
		if(nvi->flags & kBATTERYNVINFOVALID) {
			struct tm *t = gmtime(&nvi->NextUpdate);
			char buff[64];
			strftime(buff, 64, "%m/%d/%Y %H:%M:%S", t);
			result = buff;
		}
	}
	return result;
}

bool BcmBatteryController::BatteryNextLifeTest(int whichBat, const BcmString &dateStr)
{
	int err = 1;
	if ((whichBat >= 0) 
	 && (whichBat < MAX_BATTERIES) 		// PR24004
	 && (fInfo.Support & kSupportsLifeTesting)
	 && BatteryValid(whichBat)) {
		TNonVolBatteryInfo *nvi = BcmBatteryNonVolSettings::GetSingletonInstance()->GetBattery(fInfo.Batteries[whichBat].nvIndex);
		err = 2;
		if(nvi->flags & kBATTERYNVINFOVALID) {
			struct tm t;
			char buffDate[9], buffTime[9], temp[5];
			int  tempVal;
			strncpy(buffDate,dateStr.c_str(),8); buffDate[8] = '\0';
			strncpy(buffTime,&(dateStr.c_str())[9],8); buffTime[8] = '\0';
			gInfoMsg(*fMessageLogSettings, "BatteryNextLifeTest") << "Datestr = " << buffDate << ", Timestr = " << buffTime << endl;
			for (int ix = 0; ix < 8; ix++)
			{
				err = 3;
				if (!isdigit(buffDate[ix])) goto BatteryNextLifeTestErr;
				if (ix == 2 || ix == 5) continue;
				err = 31;
				if (!isdigit(buffTime[ix])) goto BatteryNextLifeTestErr;
			}
			memset(&t, 0, sizeof(struct tm));

			err = 4;
			strncpy(temp,&buffDate[0],2); temp[2] = '\0';
			tempVal = atoi(temp);
			if (tempVal < 1 || tempVal > 12) goto BatteryNextLifeTestErr;
			t.tm_mon = tempVal - 1;

			err = 5;
			strncpy(temp,&buffDate[2],2); temp[2] = '\0';
			tempVal = atoi(temp);
			if (tempVal < 1 || tempVal > 31)  goto BatteryNextLifeTestErr;
			t.tm_mday = tempVal;

			err = 6;
			strncpy(temp,&buffDate[4],4); temp[4] = '\0';
			tempVal = atoi(temp);
			if (tempVal < 0) tempVal = 0;
			if (tempVal > 1900) tempVal -= 1900;
			if (tempVal > 137)   goto BatteryNextLifeTestErr;
			t.tm_year = tempVal;

			err = 7;
			strncpy(temp,&buffTime[0],2); temp[2] = '\0';
			tempVal = atoi(temp);
			if (tempVal > 24)  goto BatteryNextLifeTestErr;
			t.tm_hour = tempVal;

			err = 8;
			strncpy(temp,&buffTime[3],2); temp[2] = '\0';
			tempVal = atoi(temp);
			if (tempVal > 59)  goto BatteryNextLifeTestErr;
			t.tm_min = tempVal;

			err = 9;
			strncpy(temp,&buffTime[6],2); temp[2] = '\0';
			tempVal = atoi(temp);
			if (tempVal > 59)  goto BatteryNextLifeTestErr;
			t.tm_sec = tempVal;

			gInfoMsg(*fMessageLogSettings, "BatteryNextLifeTest") << "Finished parsing string, writing to nonvol" << endl;
			nvi->NextUpdate = mktime(&t);
#if !(BCM_REDUCED_IMAGE_SIZE)
			gInfoMsg(*fMessageLogSettings, "BatteryNextLifeTest") << "Next life test = " << ctime((const time_t *)&nvi->NextUpdate);
#endif
			return true;
		}
	}
BatteryNextLifeTestErr:
	gInfoMsg(*fMessageLogSettings, "BatteryNextLifeTest") << "Error " << err << " detected parsing string \"" << dateStr << "\"" << endl;
	return false;
}

uint64 BcmBatteryController::GetGUID(int whichBat) const
{
	if ((whichBat >= 0) 
	 && (whichBat < MAX_BATTERIES) 		// PR24004
	 && (fDriverCalls.pfReadGUID)
	 && BatteryValid(whichBat))
		return fInfo.Batteries[whichBat].guid;
	else
		return 0ULL;
}

uint16 BcmBatteryController::GetFaults() const
{
	if( fDriverCalls.pfGetHWFaults)
		return fDriverCalls.pfGetHWFaults();
	return 0;
}

void BcmBatteryController::SetFaultMask(uint32 faultmask)
{
	if( fDriverCalls.pfGetHWFaults)
	{
		faultmask |= kDOI_FAULT;		// DOI can't be turned off
		fDriverCalls.pfSetHWFaultMask(faultmask);
	}
}

void BcmBatteryController::ClearAllFaults() const
{
	if( fDriverCalls.pfClearHWFaults)
		fDriverCalls.pfClearHWFaults(0xffff);
}
void BcmBatteryController::ClearChargeErrors(int whichBat) const
{
	if( fDriverCalls.pfClearChargeFaults)
		fDriverCalls.pfClearChargeFaults(whichBat);
}

bool BcmBatteryController::BatteryEPROMRead (int Battery, char * pBuffer, int BufferSize)
{
	if (!fInitialized) return false;
	// Create a mutex auto-lock object.  This will automatically release
	// the lock when it goes out of scope (ie when we return).
	BcmMutexSemaphoreAutoLock Lock(fpMutex);

	// Battery value should be [0 .. MAX_BATTERIES-1]	// PR24004
	if((Battery < 0) || (Battery > MAX_BATTERIES-1))	
	{
	  gLogMessageRaw << "WARNING: Parameter Battery not in range when calling BcmBatteryController::BatteryEPROMRead().\n" << endl;
	  return false;
	}
	int fullPages = (BufferSize / 32);
	int remainder = (BufferSize % 32);
	if( fullPages >= 4 )
	{
		fullPages = 4;
		remainder = 0;
	}
	int pagesToRead = fullPages;
	if( remainder > 0 )
		pagesToRead++;

	int  ix;
	for( ix =0; ix < pagesToRead; ix++ )
	{
		// no need to actually read cached pages...
		if (fInfo.Batteries[Battery].epromInfo.ValidFlags & (1 << ix)) continue;

		if (bspBatReadOWI(&fInfo, Battery, ix) == FALSE ) {
			gLogMessageRaw << "WARNING: batteryEPROMRead() failed." << endl;
			return false;
		}
	}
	// all required full pages have been read - transfer the data from the fInfo structure
	for( ix = 0; ix < fullPages; ix++ )
		memcpy(&pBuffer[ix*32], fInfo.Batteries[Battery].epromInfo.pageCache[ix], 32);
	// in case the request was for slightly more than an integer number of pages, get the remaining bytes
	if( remainder > 0 )
		memcpy(&pBuffer[fullPages * 32], fInfo.Batteries[Battery].epromInfo.pageCache[fullPages], remainder);

	return true;
}

const TBatDischargeTable *BcmBatteryController::BatteryDischargeTable(int Battery) const
{
	if (Battery == 0 || Battery == 1) 
		return fInfo.Batteries[Battery].dischargeTable;
	return (TBatDischargeTable *)0;
}

const TNonVolBatteryInfo *BcmBatteryController::BatteryNonVolInfo( int Battery ) const
{
	if ((Battery == 0 || Battery == 1) && BatteryValid(Battery))
	{
		TNonVolBatteryInfo *nvi = BcmBatteryNonVolSettings::GetSingletonInstance()->GetBattery(fInfo.Batteries[Battery].nvIndex);
		if(nvi->flags & kBATTERYNVINFOVALID) 
		{
			return nvi;
		}
	}
	return (TNonVolBatteryInfo *)0;
}

BcmString BcmBatteryController::GetBatteryEpromParameter(int Battery, BatteryEpromParameter Param)
{
	BcmString result = "";
	if(!fInitialized) return result;

	int whichPage = 0, whichString = 0;
	switch(Param) {
	case kBatteryEpromRevision:	whichPage = 0; whichString = 0; break;
	case kBatteryPackSupplier:	whichPage = 0; whichString = 1; break;
	case kBatteryModelNumber:	whichPage = 0; whichString = 2; break;
	case kBatterySerialNumber:	whichPage = 0; whichString = 3; break;
	case kBatteryPcbRev:		whichPage = 0; whichString = 4; break;	// note: no ending NULL!!!
	case kBatteryCellSupplier:	whichPage = 1; whichString = 0; break;
	case kBatteryCellCapacity:	whichPage = 1; whichString = 1; break;
	case kBatteryCellDateCode:	whichPage = 1; whichString = 2; break;
	case kBatteryPackDateCode:	whichPage = 1; whichString = 3; break;	// changed from 4 on PR17915
	default: break;
	}
	if(!(fInfo.Batteries[Battery].epromInfo.ValidFlags & (1 << whichPage)))
		if( bspBatReadOWI(&fInfo, Battery, whichPage) == FALSE ) return result;
	char *pptr = (char *)fInfo.Batteries[Battery].epromInfo.pageCache[whichPage];
	if(Param == kBatteryPcbRev)
		result += pptr[31];
	else
	{
		while(whichString--)
		{
			pptr += strlen(pptr)+1;
		}
		result += pptr;
	}
	return result;
}

bool BcmBatteryController::BatteryForcedDischarge (int Battery) const
{
	if(Battery >= MAX_BATTERIES)		// PR24004
	{
		return false;
	}
	if(this->fDriverCalls.pfStartForcedDischarge)
	{
		return fDriverCalls.pfStartForcedDischarge(&fInfo, Battery, 0);
	}
	else
		return false;
}

bool BcmBatteryController::BatteryForcedDischarge (int Battery, float toVolts) const
{
	if(Battery >= MAX_BATTERIES)		// PR24004
	{
		return false;
	}
	if(this->fDriverCalls.pfStartForcedDischarge)
	{
		if( fInfo.Batteries[Battery].dischargeTable )
		{
			gInfoMsg(*fMessageLogSettings,"BatteryForcedDischarge") << "Commanding discharge to " << toVolts << "V)" << endl; 
			return fDriverCalls.pfStartForcedDischarge(&fInfo, Battery, toVolts);
		}
	}
	return false;
}

bool BcmBatteryController::BatteryStopForcedDischarge (void) const
{
	if(this->fDriverCalls.pfStopForcedDischarge)
		return fDriverCalls.pfStopForcedDischarge();
	else
		return false;
}

bool BcmBatteryController::BatteryForcedCharge (int Battery)
{
	if(Battery >= MAX_BATTERIES)		// PR24004
	{
		return false;
	}
	if(this->fDriverCalls.pfStartForcedCharge)
	{
		float V = PctToV( fInfo.Batteries[Battery].dischargeTable, fInfo.ChargePercent );
		return BatteryForcedCharge(Battery, V);
	}
	else
		return false;
}

bool BcmBatteryController::BatteryForcedCharge (int Battery, float toVolts)
{
	if(Battery >= MAX_BATTERIES)		// PR24004
	{
		return false;
	}
	if(this->fDriverCalls.pfStartForcedCharge)
	{
		gInfoMsg(*fMessageLogSettings,"BatteryForcedCharge") << "Commanding charge to " << toVolts << "V)" << endl; 
		return fDriverCalls.pfStartForcedCharge(&fInfo, Battery, toVolts);
	}
	return false;
}

bool BcmBatteryController::BatteryForcedChargePct (int Battery, float toPct)
{
	if(Battery >= MAX_BATTERIES)		// PR24004
	{
		return false;
	}
	if(this->fDriverCalls.pfStartForcedCharge)
	{
		gInfoMsg(*fMessageLogSettings,"BatteryForcedCharge") << "Commanding charge to " << toPct << "%)" << endl;
		if (BatteryValid(Battery))
		{
			return BatteryForcedCharge(Battery, PctToV(fInfo.Batteries[Battery].dischargeTable, toPct));
		}
	}
	return false;
}


bool BcmBatteryController::BatteryStopCharge (void)
{
	if(this->fDriverCalls.pfStopCharge)
	{
		bool result = fDriverCalls.pfStopCharge();
		if(fDriverCalls.pfEnableChargingAll)
		{
			fDriverCalls.pfEnableChargingAll(FALSE);
		}
		if(this->fDriverCalls.pfSetChargePercentageAll)
		{
			fDriverCalls.pfSetChargePercentageAll(&fInfo, fInfo.ChargePercent);
		}
		if(fDriverCalls.pfEnableChargingAll)
		{
			fDriverCalls.pfEnableChargingAll(TRUE);
		}
		return result;
	}
	else
		return false;
}

int BcmBatteryController::PackTemperature(int whichPack) const
{
	if (fDiagPLTTemperature[whichPack] != TEMP_MEASUREMENT_NOT_SUPPORTED)
		return fDiagPLTTemperature[whichPack];
	return (int)fInfo.Batteries[whichPack].controllerSpecific.average_pack_temperature;
}

float BcmBatteryController::BoardTemperature() const
{
	if (fDiagBLTTemperature != TEMP_MEASUREMENT_NOT_SUPPORTED)
		return fDiagBLTTemperature;
	float result = fInfo.controllerSpecific.average_board_temperature;
	#ifdef BFC_INCLUDE_THERMAL_MONITOR_SUPPORT
		result = (float)BcmThermalMonitorThread::Singleton().ThermalMonitor().Temperature();
	#endif
	return result;
}

float BcmBatteryController::BoardLevelThermistorTemperature() const	// returns AVERAGE calibrated BLT
{
	if (fDiagBLTTemperature != TEMP_MEASUREMENT_NOT_SUPPORTED)
		return fDiagBLTTemperature;
	return fInfo.controllerSpecific.average_board_temperature;
}



//****************************  PRIVATE METHODS  *******************************
void BcmBatteryController::SmoothInputVoltage(TBatteryControllerInfo &info)
{
	float t = (float)(info.InputVoltage)/1000.0;
	float avgv = info.controllerSpecific.average_board_voltage;
	if (info.controllerSpecific.num_board_voltage_samples < fBoardVoltageSmoothingSteps) {
		info.controllerSpecific.num_board_voltage_samples++;
	}
	info.controllerSpecific.average_board_voltage = (t + (avgv * (info.controllerSpecific.num_board_voltage_samples-1)))
		/ info.controllerSpecific.num_board_voltage_samples;
}

void BcmBatteryController::SmoothPackVoltages(TBatteryControllerInfo &info)
{
	int maskflags = kBatStatusPresent | kBatStatusValid | kBatStatusUCS;
	int reqdflags = kBatStatusPresent | kBatStatusValid;
	for( int ix = 0; ix < MAX_BATTERIES; ix++ )
	{
		TBatteryInfo *bi = &info.Batteries[ix];
		if((bi->updateInfo.Status & maskflags) == reqdflags)
		{
			// pack is present, valid, and has a known charge state
			float t = bi->updateInfo.MeasuredVoltage/1000.0;
			float avgv = bi->controllerSpecific.average_pack_voltage;
			if (bi->controllerSpecific.num_voltage_samples < fPackVoltageSmoothingSteps) {
				bi->controllerSpecific.num_voltage_samples++;
			}
			bi->controllerSpecific.average_pack_voltage = (t + (avgv * (bi->controllerSpecific.num_voltage_samples-1)))
				/ bi->controllerSpecific.num_voltage_samples; 
		}
		else
		{
			bi->controllerSpecific.num_voltage_samples = 0;
			bi->controllerSpecific.average_pack_voltage = 0;
		}
	}
}
void BcmBatteryController::SmoothPackTemperatures(TBatteryControllerInfo &info)
{
	for( int ix = 0; ix < MAX_BATTERIES; ix++ )
	{
		TBatteryInfo *bi = &info.Batteries[ix];
		if((bi->updateInfo.Status & (kBatStatusPresent | kBatStatusValid)) == (kBatStatusPresent | kBatStatusValid))
		{
			float t;
			if (info.Support & kSupportsPackThermistors)
				t = bi->updateInfo.Temperature;
			else
				t = info.controllerSpecific.average_pseudo_pack_temperature;
			
			if (t != TEMP_MEASUREMENT_NOT_SUPPORTED) 
			{
				float avgv = bi->controllerSpecific.average_pack_temperature;
				if (bi->controllerSpecific.num_temperature_samples < fPackTemperatureSmoothingSteps) {
					bi->controllerSpecific.num_temperature_samples++;
				}
				bi->controllerSpecific.average_pack_temperature = (t + (avgv * (bi->controllerSpecific.num_temperature_samples-1)))
					/ bi->controllerSpecific.num_temperature_samples; 
			}
			else
			{
				bi->controllerSpecific.num_temperature_samples = 0;
				bi->controllerSpecific.average_pack_temperature = TEMP_MEASUREMENT_NOT_SUPPORTED;
			}

		}
		else
		{
			bi->controllerSpecific.num_temperature_samples = 0;
			bi->controllerSpecific.average_pack_temperature = TEMP_MEASUREMENT_NOT_SUPPORTED;
		}
	}
}
void BcmBatteryController::CalculateBoardTemperature(TBatteryControllerInfo &info)
{
	float pseudoPackTemperature = TEMP_MEASUREMENT_NOT_SUPPORTED;
	// first, calibrate the raw BLT temperature
	if (info.BLTTemperature != TEMP_MEASUREMENT_NOT_SUPPORTED) {
		info.CalBLTTemperature = fBlt2BoardCal.CalibrateTemperature(info.BLTTemperature);
		pseudoPackTemperature = fBlt2PackCal.CalibrateTemperature(info.BLTTemperature);
	}
	else {
		info.CalBLTTemperature = TEMP_MEASUREMENT_NOT_SUPPORTED;
	}

	// for board temperatures, the most desireable source is the TSC ( if available).
	// Second-most desireable is the board-level thermistor (also if available)
	float BoardTemp = info.CalBLTTemperature;
	#ifdef BFC_INCLUDE_THERMAL_MONITOR_SUPPORT
		BoardTemp = (float)BcmThermalMonitorThread::Singleton().ThermalMonitor().Temperature();
		pseudoPackTemperature = fTsc2PackCal.CalibrateTemperature(BoardTemp);
		BoardTemp = fTsc2BoardCal.CalibrateTemperature(BoardTemp);
	#endif
	// now smooth the temperature
	if (BoardTemp != TEMP_MEASUREMENT_NOT_SUPPORTED) 
	{
		// average together calibrated board temperatures (either BLT-->Board or TSC-->Board)
		float t = BoardTemp;
		float avgv = info.controllerSpecific.average_board_temperature;
		if (info.controllerSpecific.num_board_temperature_samples < fBoardTemperatureSmoothingSteps) {
			info.controllerSpecific.num_board_temperature_samples++;
		}
		info.controllerSpecific.average_board_temperature = (t + (avgv * (info.controllerSpecific.num_board_temperature_samples-1)))
		/ info.controllerSpecific.num_board_temperature_samples; 

		// average together pseudo pack temperatures (BLT/TSC-->pack) in case there are no packs or pack thermistors aren't supported
		t = pseudoPackTemperature;
		avgv = info.controllerSpecific.average_pseudo_pack_temperature;
		if (info.controllerSpecific.num_pseudo_pack_temperature_samples < fBoardTemperatureSmoothingSteps) {
			info.controllerSpecific.num_pseudo_pack_temperature_samples++;
		}
		info.controllerSpecific.average_pseudo_pack_temperature = (t + (avgv * (info.controllerSpecific.num_pseudo_pack_temperature_samples-1)))
		/ info.controllerSpecific.num_pseudo_pack_temperature_samples; 
	}
	else
	{
		info.controllerSpecific.num_pseudo_pack_temperature_samples = 0;
		info.controllerSpecific.average_pseudo_pack_temperature = TEMP_MEASUREMENT_NOT_SUPPORTED;
	}
}

// read
int BcmBatteryController::SmoothingSteps(int stepType)
{
	int result = 0;
	switch(stepType)
	{
	case kBoardTempSmoothingSteps:
		result = fBoardTemperatureSmoothingSteps;
		break;
	case kPackTempSmoothingSteps:
		result = fPackTemperatureSmoothingSteps;
		break;
	case kPackVoltageSmoothingSteps:
		result = fPackVoltageSmoothingSteps;
		break;
	case kBoardVoltageSmoothingSteps:
		result = fBoardVoltageSmoothingSteps;
		break;
	}
	//gLogMessageRaw << "SmoothingSteps: type = " << stepType << ", returning " << result << endl;
	return result;
}
// write
int BcmBatteryController::SmoothingSteps(int stepType, int val)
{
	//gLogMessageRaw << "SmoothingSteps: type = " << stepType << ", val = " << val << endl;
	switch(stepType)
	{
	case kBoardTempSmoothingSteps:
		if(val <= 0 || val > 100) val = 1;		// reset to default in case of corrupted non-vol
		fBoardTemperatureSmoothingSteps = val;
		if (val < fInfo.controllerSpecific.num_board_temperature_samples)
			fInfo.controllerSpecific.num_board_temperature_samples = val;
		if (val < fInfo.controllerSpecific.num_pseudo_pack_temperature_samples)
			fInfo.controllerSpecific.num_pseudo_pack_temperature_samples = val;
		break;
	case kPackTempSmoothingSteps:
		if(val <= 0 || val > 100) val = 1;		// reset to default in case of corrupted non-vol
		fPackTemperatureSmoothingSteps = val;
		if (val < fInfo.Batteries[0].controllerSpecific.num_temperature_samples)
			fInfo.Batteries[0].controllerSpecific.num_temperature_samples = val;
		if (val < fInfo.Batteries[1].controllerSpecific.num_temperature_samples)
			fInfo.Batteries[1].controllerSpecific.num_temperature_samples = val;
		break;
	case kPackVoltageSmoothingSteps:
		if(val <= 0 || val > 100) val = 1;		// reset to default in case of corrupted non-vol
		fPackVoltageSmoothingSteps = val;
		if (val < fInfo.Batteries[0].controllerSpecific.num_voltage_samples)
			fInfo.Batteries[0].controllerSpecific.num_voltage_samples = val;
		if (val < fInfo.Batteries[1].controllerSpecific.num_voltage_samples)
			fInfo.Batteries[1].controllerSpecific.num_voltage_samples = val;
		break;
	case kBoardVoltageSmoothingSteps:		
		if(val <= 0 || val > 100) val = 1;		// reset to default in case of corrupted non-vol
		fBoardVoltageSmoothingSteps = val;
		if (val < fInfo.controllerSpecific.num_board_voltage_samples)
			fInfo.controllerSpecific.num_board_voltage_samples = val;
		break;
	}
	return SmoothingSteps(stepType);
}


// Called ONLY when the presence status has changed, the driver indicates support for
// external pack validation, and the vendor hasn't supplied it's own validation procedure.  
// external validation requires ALL of the following:
// 		OneWire pages 0 and 1 MUST be present
// 		If driver supports reading the GUID, 
// 			Driver MUST be able to successfully read the GUID and GUID CRC MUST match
// 		#cells MUST match the BSP's capabilities
// 		if #cells = 2:
// 			 OneWire pages 2 and 3 MUST be present and MUST match pages 0/1
// 			 full 16-bit page0/1 checksum MUST match
// 		otherwise
// 			 low-order byte of page 0/1 checksum MUST match
void BcmBatteryController::ValidateBattery(TBatteryControllerInfo &info, int which)
{
	TBatteryInfo *bi = &info.Batteries[which];
	bool valid = false;		// assume battery pack is NOT valid
	// defaults:
	// checksums invalid, GUID is OK, pages don't match, 1st generation, 3S1P
	TPackValidation vs = {0,0,TRUE,FALSE,1,3,1};

	
	// sanity checks - ALL must be true - only validate packs:
	// a) where the presence status has changed
	// b) where the BSP supports external pack validation, and
	if( !((bi->updateInfo.Status & kBatStatusPresenceChanged)
	      &&(info.Support & kSupportsBfcPackValidation)))
		return;
	// c) where external pack validation routines are available
	if( (fDriverCalls.pfValidatePack == NULL)
	 || (fDriverCalls.pfCompletePackValidation == NULL)) 
		return;
	// d) only verify packs that are present and NOT yet valid
	if( !(bi->updateInfo.Status & kBatStatusPresent)
	||   (bi->updateInfo.Status & kBatStatusValid ))
		return;

	gInfoMsg(*fMessageLogSettings, "ValidateBattery") << "Validating pack " << which << endl;
	// read pages 0/1 of the eprom - inability to read these results or failure of the
	// low order byte of the checksum results in immediate disqualification
	uint16 actcs;
	bool pageReadOK = true;
	pageReadOK = (bspBatReadOWI(&info,which,0) == TRUE);
	pageReadOK &= (bspBatReadOWI(&info,which,1) == TRUE);
	if( !pageReadOK)
	{
		gInfoMsg(*fMessageLogSettings, "ValidateBattery") << "One or both of pages 0/1 failed to read...invalidating pack" << endl;
		fDriverCalls.pfValidatePack(which,FALSE);
		return;
	}
	actcs = ((uint16)(bi->epromInfo.pageCache[1][30]) << 8) | bi->epromInfo.pageCache[1][31];
	uint16 calccs = CalcEpromChecksum(bi->epromInfo,0);
	gInfoMsg(*fMessageLogSettings, "ValidateBattery") << "Page 0/1 actcs = " << hex << actcs << ", calc = " << calccs << dec << endl;
	if ((actcs & 0xff) != (calccs & 0xff))
	{
		gInfoMsg(*fMessageLogSettings, "ValidateBattery") << "Pages 0/1 8-bit CS bad" << endl;
		fDriverCalls.pfValidatePack(which,FALSE);
		return;
	}
	else
		vs.page1CS_valid = 1;
	vs.page1CS_valid |= (actcs == calccs ? 2 : 0);
	gInfoMsg(*fMessageLogSettings, "ValidateBattery") << "Page 0/1 CS validity = " << (int)(vs.page1CS_valid) << endl;

	pageReadOK = (bspBatReadOWI(&info,which,2) == TRUE);
	pageReadOK &= (bspBatReadOWI(&info,which,3) == TRUE);
	if( !pageReadOK)
	{
		gInfoMsg(*fMessageLogSettings, "ValidateBattery") << "One or both of pages 2/3 failed to read...invalidating pack" << endl;
		fDriverCalls.pfValidatePack(which,FALSE);
		return;
	}
	actcs = ((uint16)(bi->epromInfo.pageCache[3][30]) << 8) | bi->epromInfo.pageCache[3][31];
	calccs = CalcEpromChecksum(bi->epromInfo,2);
	gInfoMsg(*fMessageLogSettings, "ValidateBattery") << "Page 2/3 actcs = " << hex << actcs << ", calc = " << calccs << dec << endl;
	vs.page3CS_valid = (actcs == calccs ? 3 : 0);
	gInfoMsg(*fMessageLogSettings, "ValidateBattery") << "Page 2/3 CS validity = " << (int)(vs.page3CS_valid) << endl;

	if( !VendorSpecificValidateBattery(bi, &vs) )
	{
		if( (bi->epromInfo.pageCache[1][27] & 0xf0) == 0x30 ) vs.parallelChains = bi->epromInfo.pageCache[1][27] & 0x0f;
		if( (bi->epromInfo.pageCache[1][28] & 0xf0) == 0x30 ) 
		{	
			vs.seriesCells = bi->epromInfo.pageCache[1][28] & 0x0f;
			vs.generation = 2;
			gInfoMsg(*fMessageLogSettings, "ValidateBattery") << "Generation is now 2" << endl;
			if( bi->epromInfo.pageCache[1][24] == 0xa7 
			&&  bi->epromInfo.pageCache[1][25] == 0xa7 ) {
				gInfoMsg(*fMessageLogSettings, "ValidateBattery") << "Generation is now 3" << endl;
				vs.generation = 3;
			}
		}
	}
	gInfoMsg(*fMessageLogSettings, "ValidateBattery") << "Valid flags = " << (uint16)bi->epromInfo.ValidFlags << endl;
	if( bi->epromInfo.ValidFlags == (uint8)0x0f )
	{
		bool pm0 = memcmp(bi->epromInfo.pageCache[0],bi->epromInfo.pageCache[2],32) == 0;
		bool pm1 = memcmp(bi->epromInfo.pageCache[1],bi->epromInfo.pageCache[3],32) == 0;
		if (pm0 && pm1) {
			vs.pagesMatch = 1;
			gInfoMsg(*fMessageLogSettings, "ValidateBattery") << "PM = " << boolalpha << (bool)vs.pagesMatch << endl;
		}
		else
			gInfoMsg(*fMessageLogSettings, "ValidateBattery") << "PM 0/2 = " << boolalpha << pm0 << ", PM 1/3 = " << pm1 << endl;
	}
	else
		gInfoMsg(*fMessageLogSettings, "ValidateBattery") << "Not all 4 pages valid, not checking page match" << endl;
	

	if(fDriverCalls.pfReadGUID != NULL) {
		vs.guidOK = (fDriverCalls.pfReadGUID(&info,which) == TRUE ) && VerifyGUIDCRC(bi->guid);
		if( vs.guidOK ) {
			gInfoMsg(*fMessageLogSettings, "ValidateBattery") << "Pack GUID " << hex << bi->guid << dec << endl;
		}
	}
	else {
		gInfoMsg(*fMessageLogSettings, "ValidateBattery") << "Reading GUID not supported - defaulting to OK." << endl;
		vs.guidOK = TRUE;
	}
	if(!vs.guidOK)
	{
		gInfoMsg(*fMessageLogSettings, "ValidateBattery") << "Unable to read pack GUID or GUID CRC check failed" << endl;
		fDriverCalls.pfValidatePack(which,FALSE);
		return;
	}
	// all basic validation information has now been collected...make early valid/invalid checks
	valid = vs.seriesCells == bspNumSeriesCellsSupported();
	if (!valid)
	{
		gInfoMsg(*fMessageLogSettings, "ValidateBattery") << "#series cells (" << (int)vs.seriesCells << ") does not match compile environment of " << bspNumSeriesCellsSupported() << endl;
		cout << "===============WARNING! WARNING! WARNING! WARNING! WARNING! WARNING!=============" << endl;
		cout << "Battery pack has different number of series cells than compile environment ( " << (int)vs.seriesCells << " vs " << bspNumSeriesCellsSupported() << ")" << endl;
		cout << "===============WARNING! WARNING! WARNING! WARNING! WARNING! WARNING!=============" << endl;
	}
	if (valid)
	{
		// pages only have to match on generation 2
		if (vs.generation == 2 && !vs.pagesMatch)
		{
			valid = false;
			gInfoMsg(*fMessageLogSettings, "ValidateBattery") << "Pages don't match for generation 2" << endl;
		}
	}
	if (valid)
	{
		if (vs.generation > 1 && (vs.page1CS_valid != 3 || vs.page3CS_valid != 3))
		{
			valid = false;
			gInfoMsg(*fMessageLogSettings, "ValidateBattery") << "Generation > 1 and both full checksums aren't good" << endl;
		}
	}
	if (valid)
	{
		gInfoMsg(*fMessageLogSettings, "ValidateBattery") << "Complete initial validation...determining pack capacity..." << endl;
		DeterminePackCapacity(which, vs);
		gInfoMsg(*fMessageLogSettings, "ValidateBattery") << "Calling BSP to complete validation." << endl;
		valid = fDriverCalls.pfCompletePackValidation(&info,which,&vs);
	}
	else
	{
		fDriverCalls.pfValidatePack(which,FALSE);
		bi->dischargeTable->designCapacity = bi->dischargeTable->actualCapacity = (float)bi->EpromCapacity;
	}

	if (valid)
	{
		fInfo.Batteries[which].updateInfo.Status |= kBatStatusValid;
		fInfo.Batteries[which].updateInfo.Status &= ~(kBatStatusChgDisabled | kBatStatusDchgDisabled);
	}
	else
	{
		fInfo.Batteries[which].updateInfo.Status &= ~(kBatStatusValid | kBatStatusChgDisabled | kBatStatusDchgDisabled);
	}

	gInfoMsg(*fMessageLogSettings, "ValidateBattery") << "Valid = " << boolalpha << valid << endl;
}

uint16 BcmBatteryController::CalcEpromChecksum(TEpromInfo &info, int startPage)
{
	if ((info.ValidFlags & (3 << startPage)) != (3 << startPage)) return 0xffff;	// return an invalid checksum
	uint16 result = 0;
	for( int page = startPage; page < (startPage + 2); page++ )
	{
		uint8 *pageData = info.pageCache[page];
		for( int whichByte = 0; whichByte < 32; whichByte++ )
		{
			if (page == (startPage + 1) && whichByte >= 30) break;		// last 2 bytes are treated as 0
			result += (uint16)(*pageData++);
		}
	}
	return result;
}

// returns -1 if no GUID is available, 0 if a new entry was created, and 1 if pre-existing information was extracted
int BcmBatteryController::CheckNonVolInfo(int which)
{
	BcmBatteryNonVolSettings *nv = BcmBatteryNonVolSettings::GetSingletonInstance();
	TBatteryInfo 		*bi = &fInfo.Batteries[which];
	TNonVolBatteryInfo	*nvi = NULL;
	int			  		nonVolIndex;
	int					result = 1;		// the default is GUID found and information extracted
	// steps:
	// 1) Get the nonvol slot for this battery pack based on GUID
	if (bi->guid == 0ULL) return -1;	// there is no GUID available
	// the nonvol method FindBatteryInfo will ALWAYS return a valid index
	gInfoMsg(*fMessageLogSettings, "CheckNonVolInfo") << "Calling FindBatteryInfo with GUID = 0x" << hex << bi->guid << dec << endl;
	nonVolIndex = nv->FindBatteryInfo(bi->guid);
	gInfoMsg(*fMessageLogSettings, "CheckNonVolInfo") << "Returned index = " << nonVolIndex << endl;
	// if the nonvol slot is valid, simply decompress the existing discharge table
	bi->nvIndex = nonVolIndex;
	nvi = nv->GetBattery(nonVolIndex);
	if (fInfo.Support & kSupportsBatteryNonvol) {
		if(nvi->flags & kBATTERYNVINFOVALID) {
			gInfoMsg(*fMessageLogSettings, "CheckNonVolInfo") << "NonVolInfo is valid - extracting information\n" << endl;
			if (!bi->dischargeTable) 
				bi->dischargeTable = new TBatDischargeTable; 
			DecompressDischargeTable(nvi, bi->dischargeTable);
			// reset the charge parameters based on the new table
			if( fDriverCalls.pfSetChargePercentageSingle)
				fDriverCalls.pfSetChargePercentageSingle(&fInfo,which,fInfo.ChargePercent);
			result = 1;
		}
		else
		{
			gInfoMsg(*fMessageLogSettings, "CheckNonVolInfo") << "NonVolInfo is invalid - creating valid information" << endl;
			// non-vol was not valid...we have some work to do
			// bi->dischargeTable->batZ  		   = (uint16)(kDefaultPackResistance*1000);
			nvi->wHCorrectionRatio = 1.0;
			nvi->NextUpdate        = time(0);
			CompressDischargeTable(bi->dischargeTable, nvi);
			nvi->flags |= kBATTERYNVINFOVALID;
			result = 0;
		}
	}
	return result;
}

bool BcmBatteryController::VerifyGUIDCRC(uint64 GUID)
{
	uint8 targetCRC = (uint8)((GUID >> 56) & 0xff);
	int byteCounter;
	uint8 tempCRC = 0;
	for (byteCounter = 0; byteCounter < 7; byteCounter++) {
		uint8 temp = (uint8)((GUID >> (8*byteCounter)) & 0xff);
		tempCRC = crc8_tab[tempCRC ^ temp];
	}
	return targetCRC == tempCRC;
}

void BcmBatteryController::RescheduleByNVI(int whichNVI, int days)
{
#ifndef VENDOR_SUPPLIED_LIFE_TESTING
	struct tm t;
	int tod_hr = 22, tod_min = 0;
	char tod[16];
	strncpy(tod, BcmBatteryNonVolSettings::GetSingletonInstance()->LifeTestTOD(), 15);
	char *tok = strtok(tod,":");
	if (tok)
	{
		tod_hr = atoi(tok);
		tok = strtok(0,":");
		if (tok)
			tod_min = atoi(tok);
	}
	time_t now = time(0);
	time_t nextTestTime = now;
	if (days > 0) {
		memcpy(&t,gmtime(&now),sizeof(struct tm));
		t.tm_mday += days;
		t.tm_hour = tod_hr;
		t.tm_min = tod_min;
		t.tm_sec = 0;
		nextTestTime = mktime(&t);
	}
	gInfoMsg(*fMessageLogSettings, "Reschedule") << "Rescheduling for " << ctime(&nextTestTime) << endl;
	BcmBatteryNonVolSettings::GetSingletonInstance()->NextTestTime(whichNVI, nextTestTime);
#endif
}

void BcmBatteryController::Reschedule(int which, int days)
{
#ifndef VENDOR_SUPPLIED_LIFE_TESTING
	int nvIndex = fInfo.Batteries[which].nvIndex;
	//gLogMessageRaw << "Reschedule: Using " << nvIndex << " for battery " << which << endl;
	RescheduleByNVI(nvIndex,days);
#endif
}

void BcmBatteryController::RescheduleAtLeast(int which, int days)
{
#ifndef VENDOR_SUPPLIED_LIFE_TESTING
	int nvIndex = fInfo.Batteries[which].nvIndex;
	//gLogMessageRaw << "RescheduleAtLeast: Using " << nvIndex << " for battery " << which << endl;

	time_t nextTest = BcmBatteryNonVolSettings::GetSingletonInstance()->NextTestTime(nvIndex);
	time_t Now;
	time(&Now);
	float delta = difftime(nextTest, Now);
	//gLogMessageRaw << "RescheduleAtLeast: Next test is scheduled for " << ctime(&nextTest) << " Now = " << ctime(&Now) << ", delta = " << delta << endl;
	if (delta < (days * 86400))
	{
		//gLogMessageRaw << "RescheduleAtLeast: Rescheduling for " << days << "in the future" << endl;
		RescheduleByNVI(nvIndex,days);
	}
#endif
}

void BcmBatteryController::DeterminePackCapacity(int which, TPackValidation &vs)
{
	// by default, we'll use the design capacity of the current discharge table
	fInfo.Batteries[which].EpromCapacity = (uint16)fInfo.Batteries[which].dischargeTable->designCapacity;
	int packCapacity = (int)fInfo.Batteries[which].EpromCapacity;
	gInfoMsg(*fMessageLogSettings, "DeterminePackCapacity") << "Table design capacity = " << packCapacity << endl;

	// eprom has already been read.  verify this, then update the pack capacity based on data in the eprom
	if((fInfo.Batteries[which].epromInfo.ValidFlags & 3) != 3)
	{
		if( !bspBatReadOWI(&fInfo,which,0) || !bspBatReadOWI(&fInfo,which,1) )
		{
			gInfoMsg(*fMessageLogSettings, "DeterminePackCapacity") << "Failed to read 1 or more EPROM pages." << endl;
			return;
		}
	}

	// capacity is always the 2nd string on page 1
	char *str = (char *)fInfo.Batteries[which].epromInfo.pageCache[1];
	str = &str[strlen(str)+1];
	packCapacity = atoi(str);
	if (!VendorSpecificDeterminePackCapacity(&fInfo.Batteries[which], &packCapacity))
	{
		if (packCapacity == 0) { 
			cout << "DeterminePackCapacity: using default table capacity of " << (int)fInfo.Batteries[which].dischargeTable->designCapacity << " instead of bogus reported capacity of \"" << str << "\"" << endl;
			packCapacity = (int)fInfo.Batteries[which].dischargeTable->designCapacity;
		}
	}
	gInfoMsg(*fMessageLogSettings, "DeterminePackCapacity") << "Pack capacity from EPROM = " << packCapacity << endl;
	fInfo.Batteries[which].EpromCapacity = (uint16)packCapacity;

	// find the best default discharge table based on pack's eprom-stated capacity
	const TBatDischargeTable *newTable = FindBestDefaultDischargeTable(packCapacity, vs.seriesCells, vs.parallelChains);
	if (newTable)
	{
		CopyDischargeTable(fInfo.Batteries[which].dischargeTable, newTable);
	}
	ScaleDischargeTable(fInfo.Batteries[which].dischargeTable, packCapacity);
	return;
}

void BcmBatteryController::SignalLowPower()
{
	BcmBatteryThread::Singleton().PowerManagementLevel(BcmBatteryThread::kLowPowerMaximumReduction, true);
}

void BcmBatteryController::SignalNormalPower()
{
	BcmBatteryThread::Singleton().PowerManagementLevel(BcmBatteryThread::kFullPowerNoReduction);
}

static const char *State2CharP(int state)
{
	switch (state)
	{
	  case kBCInit:
		  return "INIT"; break;
	  case kBCIdle:
		  return "IDLE"; break;
	  case kBCChargeInit:
		  return "CHARGE INIT"; break;
	  case kBCSleep:
		  return "SLEEP"; break;
	  case kBCDischarge:
		  return "DISCHARGE"; break;
	  case kBCForcedDischarge:
		  return "FORCED DISCHARGE"; break;
	  case kBCPrecharge:
		  return "PRECHARGE"; break;
	  case kBCFastCharge:
		  return "FAST CHARGE"; break;
	  case kBCTopoff:
		  return "TRICKLE CHARGE"; break;
	  case kBCChargeSuspended:
		  return "CHARGE SUSPENDED"; break;
	  case kBCEternalSleep:
		  return "ETERNAL SLEEP"; break;
	  default:
		break;
	}
	return "UNKNOWN";
}

static char faultResultStr[80] = "";

static const char *Faults2CharP(uint16 faultWord, bool allowShorthand)
{
	if (allowShorthand && (faultWord == 0xffff)) return "All";
	if (!faultWord) return "None";

	faultResultStr[0] = '\0';
	for(int ix = 0; ix < 16 && faultWord; ix++, faultWord >>= 1) {
		if(!(faultWord & 1)) continue;
		switch (ix) {
		case 0:	 strcat(faultResultStr, "COV|"); break;
		case 1:	 strcat(faultResultStr, "COI|"); break;
		case 2:	 strcat(faultResultStr, "CUI|"); break;
		case 3:	 strcat(faultResultStr, "PVR|"); break;
		case 4:	 strcat(faultResultStr, "DOI|"); break;
		case 5:	 strcat(faultResultStr, "DHF|"); break;
		case 6:	 strcat(faultResultStr, "IVP|"); break;
		case 7:	 strcat(faultResultStr, "IIR|"); break;
		case 8:	 strcat(faultResultStr, "IOV|"); break;
		case 9:	 strcat(faultResultStr, "VWE|"); break;
		case 10: strcat(faultResultStr, "UIE|"); break;
		case 11: strcat(faultResultStr, "BOV|"); break;
		case 12: strcat(faultResultStr, "BFC0|"); break;
		case 13: strcat(faultResultStr, "BFC1|"); break;
		case 14: strcat(faultResultStr, "BFC2|"); break;
		case 15: strcat(faultResultStr, "BFC3|"); break;
		default:
			break;
		}
	}
	if (strlen(faultResultStr) > 0) {
		faultResultStr[strlen(faultResultStr)-1] = '\0';
	}
	return faultResultStr;
}

#if (!BCM_REDUCED_IMAGE_SIZE) || (BCM_TELNET_SUPPORT)
static const char *Bool2CharP(bool val)
{
	const char *TrueStr = "true";
	const char *FalseStr = "false";
	if (val) return TrueStr; else return FalseStr;
}
#endif

#if (!BCM_REDUCED_IMAGE_SIZE) || (BCM_TELNET_SUPPORT)
static const char *Status2Str(uint32 status)
{
	static char outstr[64];
	//int temp = (status >> kPackStatusValidityShift) & kPackStatusValidityMask;
	strcpy(outstr," (");
#if 0
	switch(temp)
	{
	case kPackStatusValidityMissing: strcpy(outstr," (Missing"); break;
	case kPackStatusValidityUnknown: strcpy(outstr," (Unknown"); break;
	case kPackStatusValidityValid: strcpy(outstr," (Valid"); break;
	case kPackStatusValidityInvalid: strcpy(outstr," (Invalid"); break;
	}
#endif
	if(status & kBatStatusDchgDisabled) strcat(outstr, "dd,");
	if(status & kBatStatusChgDisabled) strcat(outstr, "cd,");
	if(status & kBatStatusSelected) strcat(outstr, "sel,");
	if(status & kBatStatusFull) strcat(outstr, "full,");
	if(status & kBatStatusDepleted) strcat(outstr, "depl,");
	if(status & kBatStatusUCS) strcat(outstr, "ucs,");
	if(status & kBatStatusValid) strcat(outstr, "valid,");
	if(status & kBatStatusPresent) strcat(outstr, "present,");
	outstr[strlen(outstr)-1] = ')';
	return (const char *)outstr;
}
#endif

ostream &BcmBatteryController::Print(ostream &outStream) const
{
  #if (!BCM_REDUCED_IMAGE_SIZE) || (BCM_TELNET_SUPPORT)
	if( ALLOW_UNTRIMMED_PARTS == TRUE )
	{
		cout << "WARNING: Code is compiled with ALLOW_UNTRIMMED_PARTS set to TRUE" << endl;
		cout << "WARNING: Production code MUST NOT have this setting set to TRUE" << endl;
		cout << "WARNING: Production code MUST have ALLOW_UNTRIMMED_PARTS set to FALSE" << endl;
		cout << "WARNING: Setting ALLOW_UNTRIMMED_PARTS to TRUE is done at your own risk" << endl;
	}
	else
	{
		if( fDriverCalls.pfPartIsTrimmed && fDriverCalls.pfPartIsTrimmed() == FALSE )
		{
			outStream << "Battery Controller is disabled: Chip is not trimmed (calibrated)" << endl;
			outStream << "Consult the bmud Makefile to temporarily change this behavior" << endl;
			return outStream;
		}
	}
	ios::fmtflags prevFlags = outStream.flags();
	int prevwidth = outStream.width();
	char prevfill = outStream.fill();

	bequite = true;
  outStream << "Battery Controller Information:" << endl;
  
  #if (POWER_MGMT)
  outStream << "    PowerSave capability INCLUDED" << endl;
  #else
  outStream << "    (NO PowerSave capability)" << endl;
  #endif
  outStream << "    Sense Ratio:          " << kBatterySenseNumerator << "/" << kBatterySenseDenominator << endl;
  outStream << "    Initialized:          " << Bool2CharP(fInitialized) << endl;
  outStream << "    Charging enabled:     " << Bool2CharP(fChargingEnabled) << endl;
  outStream << "    Output enabled:       " << Bool2CharP(fOutputEnabled) << endl;
  outStream << "    Driver:               " << Version() << endl;
  outStream << "    Built:                " << ctime(&fInfo.BuildTime); // No endl, ctime attaches \n
  outStream << "    State:                " << State2CharP(State()) << endl;
  outStream << "    Faults Detected:      " << Faults2CharP(fInfo.Faults, false) << endl;
  outStream << "    Faults Allowed:       " << Faults2CharP(fInfo.FaultsAllowed, true) << endl;
  
  if (OperatingOnBattery() == true)
  outStream << "    Current power source: battery" << endl;
  else
  outStream << "    Current power source: utility" << endl;
  
  outStream << "    Input voltage:        " << InputVoltage() << " mV" << endl;
  if( fInfo.Support & kSupportsAdapterCurrent )
	  outStream << "    Adapter current:      " << abs(fInfo.AdapterCurrent) << " mA" << endl;
  
  if( fInfo.Support & kSupportsDischargeCurrent )
	  outStream << "    Discharge current:    " << fInfo.BatteryCurrent << " mA" << endl;

  if (fDiagBLTTemperature != TEMP_MEASUREMENT_NOT_SUPPORTED)
      outStream << "    BLT DIAG Temperature: " << fDiagBLTTemperature << " degrees C" << endl;
  else
  {
	#ifdef BFC_INCLUDE_THERMAL_MONITOR_SUPPORT
  outStream << "    Board Temperature:    " << BcmThermalMonitorThread::Singleton().ThermalMonitor().Temperature() << endl;
	#else
	  if(fInfo.CalBLTTemperature != TEMP_MEASUREMENT_NOT_SUPPORTED )
  outStream << "    Board Temperature:    " << fInfo.CalBLTTemperature << " degrees C" << endl;
	  else
  outStream << "    Board Temperature:    Unknown" << endl;
	#endif
  }

     if(NumberOfValidatedBatteries() == 0)
	 {
  outStream << "    Chg Temp thresholds:  " << kDefaultValue_BatteryChargeSuspensionLoTemp << " / " << kDefaultValue_BatteryChargeSuspensionHiTemp << endl;
  outStream << "    Dchg Temp thresholds: " << kDefaultValue_BatteryDischargeSuspensionLoTemp << " / " << kDefaultValue_BatteryDischargeSuspensionHiTemp << endl;
	 }
  outStream << "    LPwr Temp threshold:  " << kDefaultValue_BatteryLowPowerModeHiTemp << endl;
  outStream << "    Max charge level:     " << fInfo.ChargePercent << "%" << endl;
  outStream << "    Current charge level: " << PercentChargeRemaining() << "%" << endl;
  //outStream << "    Nom dchg mins/Bat:    " << NominalDischargeTimePerBattery() << " minutes" << endl;
  outStream << "    Est. time remaining:  " << EstimatedMinutesRemaining() << " minutes" << endl;
  outStream << "    Attached devices:     " << AttachedDevices() << endl;
  outStream << "    Number of Batteries:  " << NumberOfPresentBatteries() << endl;
  
  for (int Loop = 0; Loop < MAX_BATTERIES; Loop++)
  {
	  uint32 status = fInfo.Batteries[Loop].updateInfo.Status;
	  if (BatteryPresent(Loop) == false)
	     continue;
      
    outStream << "    Battery " << (char)('A' + Loop) << ":" << endl;
	outStream << "        Status:               0x" << hex << setw(4) << setfill('0') << status << dec << Status2Str(status) << endl;
	outStream.width(prevwidth);	outStream.fill(prevfill);

    if (fDiagBatteryErrors[Loop] == -1)
		outStream << "        Errors:               " << BatteryErrors(Loop) << endl;
    else
		outStream << "        DIAGNOSTIC Errors:    " << BatteryErrors(Loop) << endl;
    outStream << "        Capacity:             " << BatteryChargeCapacity(Loop) << "mAH" << endl;
    outStream << "        Actual Capacity:      " << BatteryActualCapacity(Loop) << "mAH" << endl;
    outStream << "        Full charge voltage:  " << BatteryFullChargeVoltage(Loop) << "mV" << endl;
    outStream << "        Depleted voltage:     " << BatteryDepletedVoltage(Loop) << "mV" << endl;
	if( BatteryChargeStateUnknown(Loop)) {
		outStream << "        Measured voltage:     unknown" << endl;
		outStream << "        Current charge level: unknown" << endl;
		outStream << "        Est time remaining:   unknown" << endl;
	}
	else {
		outStream << "        Measured voltage:     " << BatteryMeasuredVoltage(Loop) << "mV" << endl;
		outStream << "        Current charge level: " << BatteryPercentCharge(Loop) << "%" << endl;
		outStream << "        Est time remaining:   " << BatteryEstimatedMinutesRemaining(Loop) << " minutes" << endl;
	}
	int packTemp = PackTemperature(Loop);
	if ((fInfo.Support & kSupportsPackThermistors) && (packTemp != TEMP_MEASUREMENT_NOT_SUPPORTED))
	{
		outStream << "        Pack temperature:     " << packTemp << " degC" << endl;
		outStream << "        Chg Temp thresholds:  " << fInfo.Batteries[Loop].thermalInfo.Coff_Lo << " / " << fInfo.Batteries[Loop].thermalInfo.Coff_Hi << endl;
		outStream << "        Dchg Temp thresholds: " << fInfo.Batteries[Loop].thermalInfo.Doff_Lo << " / " << fInfo.Batteries[Loop].thermalInfo.Doff_Hi << endl;
		if(fInfo.Support & kSupportsPerPackEnables)
		{
			char text[64] = "", *tp = &text[1];
			outStream << "        Thermal inhibits:     ";
			if(fInfo.Batteries[Loop].updateInfo.Status & (kBatStatusChgDisabled | kBatStatusDchgDisabled))
			{
				if (fInfo.Batteries[Loop].updateInfo.Status & kBatStatusChgDisabled)
					strcpy(text,",Charge");
				if (fInfo.Batteries[Loop].updateInfo.Status & kBatStatusDchgDisabled)
					strcat(text,",Discharge");
			}
			else
				strcpy(text,",none");
			outStream << tp << endl;
		}
	}
	else
	{
		packTemp = (int)fInfo.controllerSpecific.average_pseudo_pack_temperature;
		if( packTemp == TEMP_MEASUREMENT_NOT_SUPPORTED )
			outStream << "        Pack temperature:     Not found" << endl;
		else
			outStream << "        Pack temp. (via BLT): " << packTemp << " degC" << endl;

		outStream << "        Chg Temp thresholds:  " << kDefaultValue_BatteryChargeSuspensionLoTemp << " / " << kDefaultValue_BatteryChargeSuspensionHiTemp << endl;
		outStream << "        Dchg Temp thresholds: " << kDefaultValue_BatteryDischargeSuspensionLoTemp << " / " << kDefaultValue_BatteryDischargeSuspensionHiTemp << endl;
	}
	outStream << "        Valid:                " << Bool2CharP(BatteryValid(Loop)) << endl;
	outStream << "        Unknown Voltage:      " << Bool2CharP(BatteryChargeStateUnknown(Loop)) << endl;
	outStream << "        Bad:                  " << Bool2CharP(BatteryBad(Loop)) << endl;
	outStream << "        Selected:             " << Bool2CharP(BatterySelected(Loop)) << endl;
	outStream << "        Charge Full:          " << Bool2CharP(BatteryFullyCharged(Loop)) << endl;
	outStream << "        Charge Low (time):    " << Bool2CharP(BatteryChargeLow(Loop)) << endl;
	outStream << "        Charge Low (percent): " << Bool2CharP(BatteryChargeLowPercent(Loop)) << endl;
	outStream << "        Charge Depleted:      " << Bool2CharP(BatteryChargeDepleted(Loop)) << endl;
	outStream << "        Life Test State:      " << BatteryLifeTestState(Loop) << endl;
	outStream << "        Life Test Count:      " << BatteryLifeTestCount(Loop) << endl;
	outStream << "        Last Life Test:       " << BatteryLastLifeTestString(Loop) << endl;
	outStream << "        Next Life Test:       " << BatteryNextLifeTestString(Loop) << endl;
  }
  // PR16016 - Changes needed to increase thermal data understanding
  int temperature = fInfo.BLTTemperature;	// get RAW BLT
  if (fDiagBLTTemperature != TEMP_MEASUREMENT_NOT_SUPPORTED)
	outStream << "    BLT Temperature (DIAG):   " << fDiagBLTTemperature << " degC" << endl;
  else
  {
	  if( temperature != TEMP_MEASUREMENT_NOT_SUPPORTED )
		  outStream << "    BLT Temperature (Raw/Cal):" << temperature << "/" << fInfo.CalBLTTemperature <<  " degC" << endl;
	  else
		  outStream << "    BLT Temperature:          n/a" << endl;
	  #ifdef BFC_INCLUDE_THERMAL_MONITOR_SUPPORT
		  outStream << "    TSC Temperature:          " << BcmThermalMonitorThread::Singleton().ThermalMonitor().Temperature() <<  " degC" << endl;
	  #endif
  }
  temperature = Temperature();
  if(temperature != TEMP_MEASUREMENT_NOT_SUPPORTED) 
  {
	  if( !(fInfo.Support & kSupportsPackThermistors))
	  {
		  outStream << "    C/D Decision temperature: " << temperature <<  " degC" << endl;
	  }
	  outStream << "    MIB reported temperature: " << temperature <<  " degC" << endl;
  }
  else 
  {
	  if( !(fInfo.Support & kSupportsPackThermistors))
	  {
		  outStream << "    C/D Decision temperature: n/a" << endl;
	  }
	  outStream << "    MIB reported temperature: n/a" << endl;
  }
  bequite = false;
  outStream.flags(prevFlags);
  outStream.width(prevwidth);
  outStream.fill(prevfill);
  #endif
  
  return outStream;
}

void BcmBatteryController::DiagnoseState() const
{
	// diagnoses why the Pico is in Idle or Sleep
	int state = State();
	gLogMessageRaw << "Pico is in " << State2CharP(state) << ": ";
	if (state != kBCIdle && state != kBCSleep) {
		gLogMessageRaw << "this is probably OK" << endl;
		return;
	}
	gLogMessageRaw << endl;
	if (state == kBCIdle) {
		gLogMessageRaw << "    Number of packs detected is: " << NumberOfPresentBatteries() << endl;
		gLogMessageRaw << "    Number of valid packs is:    " << NumberOfValidBatteries() << endl;
		if( fInfo.Faults & fInfo.FaultsAllowed & GEN_FAULTS ) 
			gLogMessageRaw << "    There are general faults:    " << Faults2CharP(fInfo.Faults & fInfo.FaultsAllowed & GEN_FAULTS, false) << endl;
		if( (fInfo.ChargerStatus & (kBatStatusChgDisabled | kBatStatusDchgDisabled)) == (kBatStatusChgDisabled | kBatStatusDchgDisabled))
			gLogMessageRaw << "    Both charging and discharging are disabled" << endl;
		gLogMessageRaw << "    Input (adapter) voltage is:  " << fInfo.InputVoltage << "mV - this should be checked against limits" << endl;
	} else if( state == kBCSleep ) {
		if( fInfo.Faults & fInfo.FaultsAllowed & CHG_FAULTS ) 
			gLogMessageRaw << "    There are charge faults:     " << Faults2CharP(fInfo.Faults & fInfo.FaultsAllowed & CHG_FAULTS, false) << endl;
		if( fInfo.Faults & fInfo.FaultsAllowed & DCHG_FAULTS ) 
			gLogMessageRaw << "    There are discharge faults:  " << Faults2CharP(fInfo.Faults & fInfo.FaultsAllowed & DCHG_FAULTS, false) << endl;
		gLogMessageRaw << "    Input (adapter) voltage is:  " << fInfo.InputVoltage << "mV - this should be checked against limits" << endl;
		for(int ix = 0; ix < MAX_BATTERIES; ix++) {			// PR24004
			if( BatteryPresent(ix) ){
				uint16 vinit = fInfo.Batteries[ix].VINIT;
				gLogMessageRaw << "    Battery " << ix << ":  measured voltage = " << fInfo.Batteries[ix].updateInfo.MeasuredVoltage << "mV" << endl;
				gLogMessageRaw << "               VINIT voltage    = " << vinit << "mV" << endl;
				gLogMessageRaw << "               error count      = " << BatteryErrors(ix) << endl;
			}
		}
	}
}


int BcmBatteryController::AddAttachedDevice (const char *pDevice)
{
  // Create a mutex auto-lock object.  This will automatically release
  // the lock when it goes out of scope (ie when we return).
  BcmMutexSemaphoreAutoLock Lock(fpMutex);

  if (NumberOfAttachedDevices() > 0)
    fAttachedDevices += ":";
    
  fAttachedDevices += pDevice;
  fNumberOfAttachedDevices++;
  
  return NumberOfAttachedDevices();
}

const BcmString &BcmBatteryController::AttachedDevices () const
{
  return fAttachedDevices;
}

int BcmBatteryController::NumberOfAttachedDevices () const
{
  return fNumberOfAttachedDevices;
}

bool BcmBatteryController::BatteryChargeStateUnknown( int battery) const
{
	return (fInfo.Batteries[battery].updateInfo.Status & kBatStatusUCS) != 0;
}

void BcmBatteryController::CheckAlerts()
{
	static bool FirstPoll = true;
	int tempPresent = NumberOfPresentBatteries();
	int tempUCS = NumberUCSBatteries();

	if (fNumBatteries != tempPresent
	 || fNumUCS != tempUCS )
	{
		//gLogMessageRaw << "-------->Resetting first poll..." << endl;
		//gLogMessageRaw << "                   NumPresent: (" << fNumBatteries << "/" << tempPresent << ")" << endl;
		//gLogMessageRaw << "                       NumUCS: (" << fNumUCS << "/" << tempUCS << ")" << endl;
	  FirstPoll = true;
	}
	fNumBatteries = tempPresent;
	fNumUCS = tempUCS;

	CheckBatteryAlerts(FirstPoll);
	CheckVoltageAlerts(FirstPoll);
	CheckTemperatureAlerts(FirstPoll);
	CheckHardwareAlerts();
	FirstPoll = false;
}

void BcmBatteryController::CheckChargerVoltageLow(bool avoidHysteresis)
{
	// Special case: if we have no batteries, do nothing here.  Don't raise
	// low battery condition, or clear it.
	if (NumberOfValidBatteries() <= 0) return;
	//if (NumberOfPresentBatteries() == 0)
	//  return;

	int Threshold = BcmBatteryNonVolSettings::GetSingletonInstance()->UpsConfigLowBattTime();
	int estMin = EstimatedMinutesRemaining(false);

	//gInfoMsg(*fMessageLogSettings, "CheckVoltage") << "Threshold = " << Threshold << ", estMin = " << estMin << endl;
	if ((estMin <= Threshold) && !(fControllerState & kBIT_BatteryLow))
	{
	  if ( OperatingOnBattery() || !avoidHysteresis )
	  {
		  gInfoMsg(*fMessageLogSettings, "CheckVoltage") << "Setting kBatteryLow state bit" << endl;
		  UpdateState(kBIT_BatteryLow, true, BcmBatteryThread::kBatteryLow, BcmBatteryThread::kBatteryNotLow);
	  }
	}
	else if ((estMin > Threshold) && ((fControllerState & kBIT_BatteryLow) != 0))
	{
	  if ( !OperatingOnBattery() || !avoidHysteresis )
	  {
		  gInfoMsg(*fMessageLogSettings, "CheckVoltage") << "Clearing kBatteryLow state bit" << endl;
		  UpdateState(kBIT_BatteryLow, false, BcmBatteryThread::kBatteryLow, BcmBatteryThread::kBatteryNotLow);
	  }
	}
}

void BcmBatteryController::CheckChargerVoltageDepleted( bool avoidHysteresis )
{
//	if (NumberOfValidBatteries() <= 0) return;
//	if (NumberOfPresentBatteries() == 0)  return;

	bool depleted = true;

	// Check for depletion.  We are depleted if ALL batteries are depleted or missing.
	// So what we're really checking for here is non-depletion since we
	// start assuming we are depleted.
	for (int Loop = 0; Loop < MAX_BATTERIES; Loop++)
	{
	  if (BatteryPresent(Loop) == false)  continue;
	  // If even one battery is not depleted, then depleted = false      
	  if (BatteryChargeDepleted(Loop) == false) {
		  depleted = false;
		  break;
	  }
	}

    // It has been decreed that having no batteries should not be considered 
    // 'depleted', whether they are UCS or not.
	if (NumberOfValidBatteries() == 0 /* && NumberUCSBatteries() > 0 */) {
		depleted = false;
	}

	if (depleted && !(fControllerState & kBIT_BatteryDepleted) )
	{
//	  if (OperatingOnBattery() ||  !avoidHysteresis )
//	  {
		  UpdateState(kBIT_BatteryDepleted, true, BcmBatteryThread::kBatteryDepleted, BcmBatteryThread::kBatteryNotDepleted);

		// Now would be a good time to write non-vol since we are seconds away
		// from shutting down.  Note that the check for AvoidHysteresis is to 
		// avoid writing NV if we have just booted up with a depleted battery.
		if (OperatingOnBattery())
		{
		  BcmBatteryNonVolSettings *pSettings = BcmBatteryNonVolSettings::GetSingletonInstance();
		  BcmNonVolDevice *pNonVolDevice = BcmNonVolDevice::GetSingletonInstance();

		  if ((pSettings != NULL) && (pNonVolDevice != NULL))
		  {
			gLogMessageRaw << "Writing dynamic NV since we are about to shut down." << endl;
			pNonVolDevice->Write (*pSettings, BcmNonVolSettings::kDynamicSection, BcmNonVolDevice::kWriteModeAsync);
		  }
		}
//	  }
	}
	else if (!depleted	&&  (fControllerState & kBIT_BatteryDepleted))
	{
//	  if (!OperatingOnBattery() ||  !avoidHysteresis )
//	  {
		  UpdateState(kBIT_BatteryDepleted, false, BcmBatteryThread::kBatteryDepleted, BcmBatteryThread::kBatteryNotDepleted);
//	  }
	}
}

void BcmBatteryController::CheckChargerPctLow(bool avoidHysteresis)
{
	int pct = 0;
	if (NumberOfValidBatteries() <= 0) return;
	for(int ix = 0; ix < MAX_BATTERIES; ix++) {		// PR24004
		if(BatteryValid(ix)) pct += BatteryPercentCharge(ix);
	}
	pct /= NumberOfValidBatteries();
	if (pct < 0) pct = 0;
	if (pct > 100) pct = 100;

	int Threshold = BcmBatteryNonVolSettings::GetSingletonInstance()->LowChargePercent();
	if(Threshold < 0 || Threshold > 100) Threshold = kDefaultValue_BatteryLowChargePercent;	// guard agains corrupt non-vol
	//gInfoMsg(*fMessageLogSettings, "CheckPctLow") << "Threshold = " << Threshold << ", pct = " << pct << endl;
	if (pct <= Threshold && !(fControllerState & kBIT_BatteryLowPercent))
	{
		if( OperatingOnBattery() || !avoidHysteresis )
		{
			gInfoMsg(*fMessageLogSettings, "CheckPctLow") << "Setting kBatteryLowPercent" << endl;
			UpdateState(kBIT_BatteryLowPercent, true, BcmBatteryThread::kBatteryLowPercent, BcmBatteryThread::kBatteryNotLowPercent);
		}
	}
	else if (pct > (Threshold+2) && (fControllerState & kBIT_BatteryLowPercent))
	{
		if( OperatingOnBattery() || !avoidHysteresis )
		{
			gInfoMsg(*fMessageLogSettings, "CheckPctLow") << "Clearing kBatteryLowPercent" << endl;
			UpdateState(kBIT_BatteryLowPercent, false, BcmBatteryThread::kBatteryLowPercent, BcmBatteryThread::kBatteryNotLowPercent);
		}
	}
}

void BcmBatteryController::CheckBadBatteries()
{
	bool Bad = false;
	for (int Loop = 0; Loop < MAX_BATTERIES; Loop++) 		// PR24004
	{
	  if (BatteryPresent(Loop))// && !BatteryChargeStateUnknown(Loop))
	  {
		//gInfoMsg(*fMessageLogSettings, "CheckBadBatteries") << "Battery " << Loop << " is present"<< endl;
		if (BatteryBad(Loop))
		{
		  gInfoMsg(*fMessageLogSettings, "CheckBadBatteries") << "Battery " << Loop << " is bad"<< endl;
		  Bad = true;
		}
	  }
	}
	UpdateState(kBIT_BatteryBad, Bad, BcmBatteryThread::kBatteryBad, BcmBatteryThread::kBatteryOK);
}

// missing/present transitions, and bad batteries  
void BcmBatteryController::CheckBatteryAlerts(bool firstPoll)
{
	UpdateState(kBIT_BatteryMissing, NumberOfPresentBatteries() <= 0, BcmBatteryThread::kBatteryMissing, BcmBatteryThread::kBatteryPresent);
	CheckBadBatteries();
}

// checks for missing/present input voltage, low/depleted batteries
void BcmBatteryController::CheckVoltageAlerts(bool firstPoll)
{
	//gInfoMsg(*fMessageLogSettings,"CheckVoltageAlerts") << "State = " << State2CharP(fInfo.State) << endl;
	CheckChargerVoltageLow(firstPoll);
	CheckChargerPctLow(firstPoll);
	CheckChargerVoltageDepleted(firstPoll);
}

#define CHARGE_HYSTERESIS 	 3
#define DISCHARGE_HYSTERESIS 3
#define LOW_POWER_HYSTERESIS 3
void BcmBatteryController::CheckPerPackTemperatureExtremes(bool *globalChgInhibit, bool *globalDchgInhibit)
{
	struct _ptInfo {
	  bool valid;
	  int temperature;
	  int chg_delta, dchg_delta;
	  bool exceedsChgLimit, exceedsDchgLimit, withinChgHysteresis, withinDchgHysteresis;
	} ptInfo[2], *pti;
	TBatteryThermalInfo *ti;
	bool quite = bequite;
	uint8 settings = fMessageLogSettings->GetEnabledFields();
	fMessageLogSettings->SetEnabledFields(0);
	for(int ix = 0; ix < 2; ix++) 
	{
		memset(&ptInfo[ix], 0, sizeof(struct _ptInfo));
		ptInfo[ix].valid = (fInfo.Batteries[ix].updateInfo.Status & kPackStatusValidityValid) != 0;
		if(ptInfo[ix].valid)
		{
			ti = &fInfo.Batteries[ix].thermalInfo;
			bequite = true;
			ptInfo[ix].temperature = (int)PackTemperature(ix);
			bequite = quite;
			if( abs(ptInfo[ix].temperature - ti->Coff_Lo) < abs(ti->Coff_Hi - ptInfo[ix].temperature) )
				ptInfo[ix].chg_delta = ptInfo[ix].temperature - ti->Coff_Lo;
			else
				ptInfo[ix].chg_delta = ti->Coff_Hi - ptInfo[ix].temperature;
			if( abs(ptInfo[ix].temperature - ti->Doff_Lo) < abs(ti->Doff_Hi - ptInfo[ix].temperature) )
				ptInfo[ix].dchg_delta = ptInfo[ix].temperature - ti->Doff_Lo;
			else
				ptInfo[ix].dchg_delta = ti->Doff_Hi - ptInfo[ix].temperature;
	
			ptInfo[ix].exceedsChgLimit = (ptInfo[ix].chg_delta < 0);
			ptInfo[ix].exceedsDchgLimit = (ptInfo[ix].dchg_delta < 0);
			ptInfo[ix].withinChgHysteresis = (ptInfo[ix].temperature >= (ti->Coff_Lo + CHARGE_HYSTERESIS))
										  && (ptInfo[ix].temperature <= (ti->Coff_Hi - CHARGE_HYSTERESIS));
			ptInfo[ix].withinDchgHysteresis = (ptInfo[ix].temperature >= (ti->Doff_Lo + DISCHARGE_HYSTERESIS))
										   && (ptInfo[ix].temperature <= (ti->Doff_Hi - DISCHARGE_HYSTERESIS));
			if(ptInfo[ix].exceedsChgLimit)
				fDriverCalls.pfSetPackChargeInhibit(&fInfo.Batteries[ix],ix,TRUE);
			else if( ptInfo[ix].withinChgHysteresis )
				fDriverCalls.pfSetPackChargeInhibit(&fInfo.Batteries[ix],ix,FALSE);

			if(ptInfo[ix].exceedsDchgLimit)
				fDriverCalls.pfSetPackDischargeInhibit(&fInfo.Batteries[ix],ix,TRUE);
			else if( ptInfo[ix].withinDchgHysteresis )
				fDriverCalls.pfSetPackDischargeInhibit(&fInfo.Batteries[ix],ix,FALSE);
		}
	}
	if(!bequite && (fMessageLogSettings->GetEnabledSeverities() & kThermalDecisionMessages))
	{
		// dump both ptInfos
		gInfoMsg(*fMessageLogSettings, "CheckPerPackTemperatureExtremes") << "ptInfo:" << endl;
		for(int ix = 0; ix < 2; ix++)
		{
			gInfoMsg(*fMessageLogSettings, "") << "  ptInfo[" << ix << "]:" << endl;
			gInfoMsg(*fMessageLogSettings, "") << "      valid: " << boolalpha << ptInfo[ix].valid << endl;
			if(ptInfo[ix].valid)
			{
				gInfoMsg(*fMessageLogSettings, "") << "       temp: " << ptInfo[ix].temperature << endl;
				gInfoMsg(*fMessageLogSettings, "") << "   chgdelta: " << ptInfo[ix].chg_delta << endl;
				gInfoMsg(*fMessageLogSettings, "") << "  dchgdelta: " << ptInfo[ix].dchg_delta << endl;
				gInfoMsg(*fMessageLogSettings, "") << "  xChgLimit: " << boolalpha << ptInfo[ix].exceedsChgLimit << endl;
				gInfoMsg(*fMessageLogSettings, "") << " xDchgLimit: " << boolalpha << ptInfo[ix].exceedsDchgLimit << endl;
				gInfoMsg(*fMessageLogSettings, "") << "  wChgLimit: " << boolalpha << ptInfo[ix].withinChgHysteresis << endl;
				gInfoMsg(*fMessageLogSettings, "") << " wDchgLimit: " << boolalpha << ptInfo[ix].withinDchgHysteresis << endl;
			}
		}
	}
	if(NumberOfValidatedBatteries() == 1)
	{
		// only one pack is valid:
		pti = &ptInfo[ptInfo[0].valid ? 0 : 1];		// choose information for the valid pack
		if(fMessageLogSettings->GetEnabledSeverities() & kThermalDecisionMessages)
			gLogMessageRaw << "CheckPerPackTemperatureExtremes: Basing decision on single pack (" << (ptInfo[0].valid ? 0 : 1) << ")" << endl;
		if(pti->exceedsChgLimit) 
		{
			if(fMessageLogSettings->GetEnabledSeverities() & kThermalDecisionMessages)
				gLogMessageRaw << "CheckPerPackTemperatureExtremes: Setting charge inhibit" << endl;
			*globalChgInhibit = true;
		}
		else if(pti->withinChgHysteresis)
		{
			if(fMessageLogSettings->GetEnabledSeverities() & kThermalDecisionMessages)
				gLogMessageRaw << "CheckPerPackTemperatureExtremes: Clearing charge inhibit" << endl;
			*globalChgInhibit = false;
		}
		if(pti->exceedsDchgLimit) 
		{
			if(fMessageLogSettings->GetEnabledSeverities() & kThermalDecisionMessages)
				gLogMessageRaw << "CheckPerPackTemperatureExtremes: Setting discharge inhibit" << endl;
			*globalDchgInhibit = true;
		}
		else if(pti->withinDchgHysteresis)
		{
			if(fMessageLogSettings->GetEnabledSeverities() & kThermalDecisionMessages)
				gLogMessageRaw << "CheckPerPackTemperatureExtremes: Clearing discharge inhibit" << endl;
			*globalDchgInhibit = false;
		}
	}
	else
	{
		// both packs valid:
		if(fMessageLogSettings->GetEnabledSeverities() & kThermalDecisionMessages)
			gLogMessageRaw << "CheckPerPackTemperatureExtremes: Basing decision on both packs" << endl;
		if( ptInfo[0].exceedsChgLimit && ptInfo[1].exceedsChgLimit ) 
		{
			if(fMessageLogSettings->GetEnabledSeverities() & kThermalDecisionMessages)
				gLogMessageRaw << "CheckPerPackTemperatureExtremes: Setting charge inhibit (" << boolalpha << ptInfo[0].exceedsChgLimit << ", " << ptInfo[1].exceedsChgLimit << ")" << endl;
			*globalChgInhibit = true;
		}
		else if(ptInfo[0].withinChgHysteresis || ptInfo[1].withinChgHysteresis)
		{
			if(fMessageLogSettings->GetEnabledSeverities() & kThermalDecisionMessages)
				gLogMessageRaw << "CheckPerPackTemperatureExtremes: Clearing charge inhibit (" << boolalpha << ptInfo[0].withinChgHysteresis << ", " << ptInfo[1].withinChgHysteresis << ")" << endl;
			*globalChgInhibit = false;
		}
		if( ptInfo[0].exceedsDchgLimit && ptInfo[1].exceedsDchgLimit ) 
		{
			if(fMessageLogSettings->GetEnabledSeverities() & kThermalDecisionMessages)
				gLogMessageRaw << "CheckPerPackTemperatureExtremes: Setting discharge inhibit (" << boolalpha << ptInfo[0].exceedsDchgLimit << ", " << ptInfo[1].exceedsDchgLimit << ")" << endl;
			*globalDchgInhibit = true;
		}
		else if(ptInfo[0].withinDchgHysteresis || ptInfo[1].withinDchgHysteresis)
		{
			if(fMessageLogSettings->GetEnabledSeverities() & kThermalDecisionMessages)
				gLogMessageRaw << "CheckPerPackTemperatureExtremes: Clearing discharge inhibit (" << boolalpha << ptInfo[0].withinDchgHysteresis << ", " << ptInfo[1].withinDchgHysteresis << ")" << endl;
			*globalDchgInhibit = false;
		}
	}
	fMessageLogSettings->SetEnabledFields(settings);
}

void BcmBatteryController::CheckPackTemperatureExtremes(bool *globalChgInhibit, bool *globalDchgInhibit)
{
	bequite = false;
	float t = Temperature();
	bequite = true;
	if (t != TEMP_MEASUREMENT_NOT_SUPPORTED) {
		if((NumberOfValidatedBatteries() > 0) && (fInfo.Support & kSupportsPerPackEnables)) {
			bequite = false;
			CheckPerPackTemperatureExtremes(globalChgInhibit,globalDchgInhibit);
			bequite = true;
		}
		else
		{
			if( t <= kDefaultValue_BatteryChargeSuspensionLoTemp 
			 || t >= kDefaultValue_BatteryChargeSuspensionHiTemp )
			{
				*globalChgInhibit = true;
			}
			else if( t >= (kDefaultValue_BatteryChargeSuspensionLoTemp + CHARGE_HYSTERESIS)  
				  && t <= (kDefaultValue_BatteryChargeSuspensionHiTemp - CHARGE_HYSTERESIS))
			{
				*globalChgInhibit = false;
			}
			if( t <= kDefaultValue_BatteryDischargeSuspensionLoTemp 
			 || t >= kDefaultValue_BatteryDischargeSuspensionHiTemp )
			{
				*globalDchgInhibit = true;
			}
			else if( t >= (kDefaultValue_BatteryDischargeSuspensionLoTemp + DISCHARGE_HYSTERESIS)  
				  && t <= (kDefaultValue_BatteryDischargeSuspensionHiTemp - DISCHARGE_HYSTERESIS))
			{
				*globalDchgInhibit = false;
			}
		}
	}
	gInfoMsg(*fMessageLogSettings, "CheckPackTemperatureExtremes") << "gblChgInhibit = " << boolalpha << *globalChgInhibit << ", gblDchgInhibit = " << *globalDchgInhibit << endl;
}

// checks for various temperature limits being exceeded
void BcmBatteryController::CheckTemperatureAlerts(bool firstPoll)
{
#ifdef VENDOR_TEMPERATURE_EVENTS_OVERRIDE_FUNCTION
	VENDOR_TEMPERATURE_EVENTS_OVERRIDE_FUNCTION();
#else
	float t = Temperature();
	bool inhibitCharging = (fControllerState & kBIT_ChargeTemperatureDisabled) != 0;
	bool inhibitDischarging = (fControllerState & kBIT_DischargeTemperatureDisabled) != 0;
	if (t != TEMP_MEASUREMENT_NOT_SUPPORTED) {
		CheckPackTemperatureExtremes(&inhibitCharging, &inhibitDischarging);
	}

	// check for high-temperature, low power condition
	t = BoardTemperature();
	bool hiTempLowPower = (fControllerState & kBIT_HiTempLowPower) != 0;
	if (t != TEMP_MEASUREMENT_NOT_SUPPORTED) {
		if( t >= kDefaultValue_BatteryLowPowerModeHiTemp )
		{
			//gLogMessageRaw << "CheckTemperatureAlerts: Board temperature exceeds low power temperature" << endl;
			hiTempLowPower = true;
			inhibitCharging = true;
		}
		else if( t <= (kDefaultValue_BatteryLowPowerModeHiTemp - LOW_POWER_HYSTERESIS))
		{
			hiTempLowPower = false;
		}
	}
	EnableChargeAll(!inhibitCharging, kReasonThermal);
	EnableDischargeAll(!inhibitDischarging, kReasonThermal);

	// finally, set state bits and raise events as appropriate
	bool newtempOOB = inhibitCharging || inhibitDischarging || hiTempLowPower;
	bool oldtempOOB = (fControllerState & TEMPERATURE_OOB) != 0;
	UpdateState(kBIT_HiTempLowPower, hiTempLowPower, BcmBatteryThread::kLowPowerHiTemperature, BcmBatteryThread::kNotLowPowerHiTemperature);
	UpdateState(kBIT_ChargeTemperatureDisabled, inhibitCharging, BcmBatteryThread::kBatteryChargeTemperatureDisabled, BcmBatteryThread::kBatteryChargeTemperatureNotDisabled);
	UpdateState(kBIT_DischargeTemperatureDisabled, inhibitDischarging, BcmBatteryThread::kBatteryDischargeTemperatureDisabled, BcmBatteryThread::kBatteryDischargeTemperatureNotDisabled);
	if( newtempOOB != oldtempOOB) {
		BcmBatteryThread::Singleton().RaiseEvent(newtempOOB ? BcmBatteryThread::kBatteryTemperatureOOB : BcmBatteryThread::kBatteryTemperatureNotOOB);
	}
#endif
}

void BcmBatteryController::CheckHardwareAlerts()
{
	if(SupportsHardwareFaults()) {
		uint16 allowedFaults = (uint16)BcmBatteryNonVolSettings::GetSingletonInstance()->AllowedFaults();
		uint16 actualFaults = GetFaults();
		UpdateState(kBIT_HardwareFaultDetected, (allowedFaults & actualFaults) != 0, 
					BcmBatteryThread::kHardwareFaultDetected, BcmBatteryThread::kHardwareFaultNotDetected);
	}
}

bool BcmBatteryController::LowPowerRequiredDueToHighTemperature()
{
	return (fControllerState & kBIT_HiTempLowPower) != 0;
}

void BcmBatteryController::UpdateState(int stateBit, bool newState, int eventIfTrue, int eventIfFalse)
{
	bool oldstate = (fControllerState & stateBit) != 0;
	if (oldstate && !newState) {
		// state WAS set and is now to be cleared
		fControllerState &= ~(stateBit);
		BcmBatteryThread::Singleton().RaiseEvent(eventIfFalse);
	}
	else if (!oldstate && newState) {
		// state WAS clear and is now to be set
		fControllerState |= stateBit;
		BcmBatteryThread::Singleton().RaiseEvent(eventIfTrue);
	}
}

// this is called to enable/disable charging on ALL batteries.
void BcmBatteryController::EnableChargeAll(bool enable, int reason)
{
	static int disableReasons = 0;

	// if the BSP doesn't support global charge enables just return
	if(!fDriverCalls.pfEnableChargingAll) return;

	int oldReasons = disableReasons;
	if (enable)
		disableReasons &= ~reason;
	else
		disableReasons |= reason;
	fChargingEnabled = (disableReasons == 0);

	if(oldReasons != disableReasons)
	{
		if( !fChargingEnabled ) {
			fDriverCalls.pfEnableChargingAll(FALSE);
			if (reason == kReasonThermal && fLTInfo.LTState != kLTState_Idle ) 
				AbortLifeTest();
		}
		else
		{
			gInfoMsg(*fMessageLogSettings, "EnableChargeAll") << "Reenabling battery charge" << endl;
			fDriverCalls.pfEnableChargingAll(TRUE);
		}
	}
}

// this is called to enable/disable discharging on ALL batteries.
void BcmBatteryController::EnableDischargeAll(bool enable, int reason)
{
	static bool disableMessagePrinted = false;
	static int  disableReasons = 0;

	// if the BSP doesn't support global charge enables just return
	if(!fDriverCalls.pfEnableDischargeAll) return;
	int oldReasons = disableReasons;

	if (enable)
		disableReasons &= ~reason;
	else
		disableReasons |= reason;
	fOutputEnabled = (disableReasons == 0);

	if(oldReasons != disableReasons)
	{
		if( !fOutputEnabled ) {
			if ((disableReasons & kReasonThermal) && !disableMessagePrinted)
			{
					gLogMessageRaw << "Disabling battery discharge due to thermal limit being exceeded.  Temperature = " << Temperature() << " degC." << endl;
					disableMessagePrinted = true;
			}
			fDriverCalls.pfEnableDischargeAll(FALSE);
			if (reason == kReasonThermal && fLTInfo.LTState != kLTState_Idle ) 
				AbortLifeTest();
		}
		else
		{
			fDriverCalls.pfEnableDischargeAll(TRUE);
			gInfoMsg(*fMessageLogSettings, "EnableDischargeAll") << "Reenabling battery discharge" << endl;
			disableMessagePrinted = false;
		}
	}
}

void BcmBatteryController::AbortLifeTest()
{
#ifdef VENDOR_SUPPLIED_LIFE_TESTING
	VENDOR_ABORT_LIFE_TEST();
#else
	if( fLTInfo.LTState != kLTState_Idle) {
		fLTInfo.LTError = true;
		CompleteLifeTest();
	}
#endif
}

bool BcmBatteryController::MeasureHWImpedance(unsigned int bat, float &z) const
{
	if (bat >= MAX_BATTERIES || fDriverCalls.pfMakeHWResistanceMeasurement == NULL) { 		// PR24004
		return false;
	}
	return fDriverCalls.pfMakeHWResistanceMeasurement(bat, &z);
}

void BcmBatteryController::RaiseAlert(int alarm)
{
	BcmBatteryThread::Singleton().RaiseEvent(alarm);
}

void BcmBatteryController::ForceWatchdogTimeout()
{
	if( fDriverCalls.pfForceWatchdogTimeout != NULL )
		fDriverCalls.pfForceWatchdogTimeout();
}

void BcmBatteryController::ForceLifeTest(int whichBat)
{
	if(whichBat < MAX_BATTERIES)		// PR24004
	{
	#ifdef VENDOR_SUPPLIED_LIFE_TESTING
		VENDOR_FORCE_LIFE_TEST(whichBat);
	#else
		gInfoMsg(*fMessageLogSettings, "ForceLifeTest") << "Forcing life test on battery " << whichBat << " (nvIndex " << fInfo.Batteries[whichBat].nvIndex << ")" << endl;
		AbortLifeTest();
		BcmBatteryNonVolSettings::GetSingletonInstance()->NextTestTime(fInfo.Batteries[whichBat].nvIndex, (time_t)0);
		CheckLifeTest(whichBat);
	#endif
	}
	else
		gLogMessageRaw << "Invalid battery.";
}

int BcmBatteryController::BatteryLifeTestState(int whichBat) const
{
	if (whichBat == fLTInfo.LTBattery) {
		return fLTInfo.LTState;
	}
	return kLTState_Idle;
}

int BcmBatteryController::LifeTestState()
{
#ifdef VENDOR_SUPPLIED_LIFE_TESTING
	return VENDOR_LIFE_TEST_STATE();
#else
	return fLTInfo.LTState;
#endif
}

uint16 BcmBatteryController::RawControllerState()
{
	return fInfo.RawControllerState;
}

bool BcmBatteryController::AllBatteriesCharged()
{
	if(BatteryValid(0) && !BatteryFullyCharged(0)) {
		return false;
	}
	if(BatteryValid(1) && !BatteryFullyCharged(1)) {
		return false;
	}
	return true;	// all valid batteries are charged
}

void BcmBatteryController::UpdateMaxHistory(uint32 pollSecs)
{
	fInfo.PollSeconds = pollSecs;
	if (pollSecs == 0) pollSecs = 1;	// prevent a divide-by-zero exception
	int samples = 60/pollSecs;
	history.SetMaxHistory(samples + 1);
}

//*************************** BATTERY LIFE TESTING *******************************
#ifndef VENDOR_SUPPLIED_LIFE_TESTING
	static int TemperatureCompensateTime(int seconds, float temperature)
	{
		// for now, we'll just return the seconds...later - when we know what to do here - we'll actually
		// perform some sort of compensation.
		return seconds;
	}
	
	static float TemperatureCompensate( float volts, float degC )
	{
		// for now, we'll just return the voltage...later - when we know what to do here - we'll actually
		// perform some sort of compensation.
		return volts;
	}
	
	void BcmBatteryController::BuildNewDischargeTable()
	{
		// allocate a bunch of automatic variables to make the code easier to read
		int which = fLTInfo.LTBattery;
		float rSeries = fLTInfo.Impedance;
		int ltPolls = fLTInfo.LTDataIndex;
		float pollSeconds = (float)(fLTInfo.LTSampleSeconds);
		LTDataEntry *lde = fLTInfo.LTData;
		float averageTemperature = fLTInfo.LTAccumTemperature/(float)ltPolls;
		int	  totalSeconds = TemperatureCompensateTime((int)(pollSeconds * ltPolls), averageTemperature);

		totalSeconds += 0;		// gets rid of compiler warning when building a slim image!
	
		gInfoMsg(*fMessageLogSettings, "BuildNewDischargeTable") << "rSeries = " << rSeries << endl;
		gInfoMsg(*fMessageLogSettings, "BuildNewDischargeTable") << "polls   = " << ltPolls << endl;
		gInfoMsg(*fMessageLogSettings, "BuildNewDischargeTable") << "pollsec = " << pollSeconds << endl;
		gInfoMsg(*fMessageLogSettings, "BuildNewDischargeTable") << "avgTemp = " << averageTemperature << endl;
		gInfoMsg(*fMessageLogSettings, "BuildNewDischargeTable") << "totSecs = " << totalSeconds << endl << endl;
	
		// find last valid entry (i.e. 3V/cell)
		int ix, minDchgmV = (int)((bspNumSeriesCellsSupported() * kLifeTestMinVoltsPerCell) * 1000);
		if( fDriverCalls.pfDcvbOnMillivolts)
		{
			minDchgmV = fDriverCalls.pfDcvbOnMillivolts(fLTInfo.LTBattery);
		}
		int lastPoll = ltPolls - 1;
		int deltamV = abs(lde[lastPoll].millivolts - minDchgmV);
		for( ix = ltPolls-2; ix > 0; ix-- )
		{
			int newDeltamV = abs(lde[ix].millivolts - minDchgmV);
			if (newDeltamV < deltamV)
			{
				gInfoMsg(*fMessageLogSettings, "BuildNewDischargeTable") << "Correcting for last poll: prev_deltaMv = " << deltamV << ", new_deltaMv = " << newDeltamV << ", new_lastPoll = " << ix << endl;
				lastPoll = ix;
				deltamV = newDeltamV;
			}
			else
			{
				gInfoMsg(*fMessageLogSettings, "BuildNewDischargeTable") << "Finished correcting for last poll: prev_deltaMv = " << deltamV << ", new_deltaMv = " << newDeltamV << ", lastPoll = " << lastPoll << endl;
				ltPolls = lastPoll;
				break;
			}
		}
		//recalculate average temperature and total seconds based on [potentially] new # polls
		averageTemperature = fLTInfo.LTAccumTemperature/(float)ltPolls;
		totalSeconds = TemperatureCompensateTime((int)(pollSeconds * ltPolls), averageTemperature);
		gInfoMsg(*fMessageLogSettings, "BuildNewDischargeTable") << "After correcting for last valid poll:" << endl;
		gInfoMsg(*fMessageLogSettings, "BuildNewDischargeTable") << "   polls   = " << ltPolls << endl;
		gInfoMsg(*fMessageLogSettings, "BuildNewDischargeTable") << "   totSecs = " << totalSeconds << endl << endl;

		// correct sample V for I*Rseries
		for( ix = 0; ix < ltPolls; ix++ )
		{
			float prevMv = lde[ix].millivolts;
            prevMv += 0.0;  // gets rid of compiler warning when building a slim image!
			lde[ix].millivolts += (unsigned short)(fabs((float)lde[ix].milliamps)*rSeries);
			if (ix < 10)
			{
				gInfoMsg(*fMessageLogSettings, "BuildNewDischargeTable") 
						<< "milliamps = " << (float)lde[ix].milliamps
						<< ", rSeries = " << rSeries 
						<< ", prev millivolts = " << prevMv
						<< ", new millivolts = " << lde[ix].millivolts << endl;
			}
		}
		// determine total wH and mAh used
		float totWh = 0.0, totmAh = 0.0, watts[2] = {0.0,0.0}, mA[2] = {0.0,0.0};
		mA[1] = fabs((float)lde[0].milliamps);
		watts[1] = ((float)lde[0].millivolts * mA[1])/1e6;
		for( ix = 1; ix < ltPolls; ix++ )
		{
			mA[0] = mA[1]; watts[0] = watts[1];
			mA[1] = fabs((float)lde[ix].milliamps);
			watts[1] = ((float)lde[ix].millivolts * mA[1])/1e6;
			totmAh += ((mA[0] + mA[1])/2 * pollSeconds)/3600.0;
			totWh  += ((watts[0] + watts[1])/2 * pollSeconds)/3600.0;
		}
		// correct for the 95% charge
		gInfoMsg(*fMessageLogSettings, "BuildNewDischargeTable") << "   Prior to 95% charge correction:" << endl;
		gInfoMsg(*fMessageLogSettings, "BuildNewDischargeTable") << "      total watt hours     = " << totWh << endl;
		gInfoMsg(*fMessageLogSettings, "BuildNewDischargeTable") << "      total milliamp hours = " << totmAh << endl;
		totWh = totWh * 100.0/95.0;
		totmAh = totmAh * 100.0/95.0;
		gInfoMsg(*fMessageLogSettings, "BuildNewDischargeTable") << "   After 95% charge correction:" << endl;
		gInfoMsg(*fMessageLogSettings, "BuildNewDischargeTable") << "      total watt hours     = " << totWh << endl;
		gInfoMsg(*fMessageLogSettings, "BuildNewDischargeTable") << "      total milliamp hours = " << totmAh << endl;

		// determine polls and pct wH/pct
		float ppp = (float)ltPolls/95.0;	// polls-per-percent (we only charged to 95%)
		float whpp = totWh/100.0;			// watt-hours-per-percent (totWh already corrected for 95% charge)
		float vbatmvppct = (float)abs(lde[(int)ppp].millivolts - lde[0].millivolts);
		if( ((float)lde[0].millivolts + 5*vbatmvppct) > ((bspNumSeriesCellsSupported() * kLifeTestMaxVoltsPerCell) * 1000) ) 
		{
			gInfoMsg(*fMessageLogSettings, "BuildNewDischargeTable") << "   modifying mvppct to avoid exceeding max vbat:" << endl;
			gInfoMsg(*fMessageLogSettings, "BuildNewDischargeTable") << "      lde[0].millivolts = " << lde[0].millivolts << endl;
			gInfoMsg(*fMessageLogSettings, "BuildNewDischargeTable") << "      5*vbatmvppct      = " << 5*vbatmvppct << endl;
			gInfoMsg(*fMessageLogSettings, "BuildNewDischargeTable") << "      maxvbatmv         = " << (int)((bspNumSeriesCellsSupported() * kLifeTestMaxVoltsPerCell) * 1000) << endl;
			// constrain to avoid exceeded maximum possible pack voltage anywhere in upper 5% of the table
			vbatmvppct = (((bspNumSeriesCellsSupported() * kLifeTestMaxVoltsPerCell) * 1000) - (float)lde[0].millivolts)/5;
			gInfoMsg(*fMessageLogSettings, "BuildNewDischargeTable") << "      5*new vbatmvppct  = " << 5*vbatmvppct << endl;
		}
		// allocate the discharge table
		TBatDischargeTable *dt = new TBatDischargeTable;
		dt->designCapacity = fInfo.Batteries[which].dischargeTable->designCapacity;
		dt->actualCapacity = totmAh;
		dt->totalMinutes = (totWh*60.0)/fLTInfo.LTLPWatts;
		gInfoMsg(*fMessageLogSettings, "BuildNewDischargeTable") << "   Using lp watts of " << fLTInfo.LTLPWatts 
																 << "W, and totalWh of " << totWh << "Wh, setting total minutes = " << dt->totalMinutes << endl;
		dt->maxWh = totWh;
		dt->seriesCells = fInfo.Batteries[which].dischargeTable->seriesCells;
		dt->parallelChains = fInfo.Batteries[which].dischargeTable->parallelChains;
		dt->batZ = (uint16)(fLTInfo.Impedance * 1000.0);
		gInfoMsg(*fMessageLogSettings, "BuildNewDischargeTable") << "..." << endl;
		gInfoMsg(*fMessageLogSettings, "BuildNewDischargeTable") << "   total discharge time = " << totalSeconds << " seconds" << endl;
		gInfoMsg(*fMessageLogSettings, "BuildNewDischargeTable") << "   Wh/%                 = " << whpp << endl;
		gInfoMsg(*fMessageLogSettings, "BuildNewDischargeTable") << "   polls/%              = " << ppp << endl;

		// fill in voltages and wH remaining
		for( ix = 0; ix <= 100; ix++ )
		{
			if(ix < 5)
			{
				// back into the first 5 entries based on delta between entries 5 and 6
				dt->table[ix].voltage = ((float)(lde[0].millivolts) + (5-ix)*vbatmvppct)/1000.0;
			}
			else
			{
				dt->table[ix].voltage = (float)(lde[(int)(ppp*(ix-5))].millivolts) / 1000.0;
			}
			dt->table[ix].whRemaining = totWh - (whpp * ix);
		}
		// copy the new discharge table over the old one and free the temporary copy
		gInfoMsg(*fMessageLogSettings, "BuildNewDischargeTable") << "OldDischargeTable:" << endl;
		DumpDischargeTable(fInfo.Batteries[which].dischargeTable);
		CopyDischargeTable(fInfo.Batteries[which].dischargeTable, dt);
		gInfoMsg(*fMessageLogSettings, "BuildNewDischargeTable") << "New DischargeTable:" << endl;
		DumpDischargeTable(	fInfo.Batteries[which].dischargeTable );
		delete dt;
	}
#endif	// VENDOR_SUPPLIED_LIFE_TESTING

void BcmBatteryController::CheckLifeTest(int whichBattery )
{
#ifdef VENDOR_SUPPLIED_LIFE_TESTING
	VENDOR_PACK_REQUIRES_LIFE_TEST(battery);
#else
    if (!LifeTestingEnabled()) 
        return;
    if ((fInfo.Support & kSupportsLifeTesting) == 0) 
        return;
	if( (fLTInfo.LTState != kLTState_Idle) && (fLTInfo.LTBattery == whichBattery))
		return;									// life testing already in progress for this battery
    if( !BatteryValid(whichBattery) )
	{
		//gInfoMsg(*fMessageLogSettings, "CheckLifeTest") << "Battery " << whichBattery << " isn't valid." << endl;
		return;
	}
	int nvIdx = fInfo.Batteries[whichBattery].nvIndex;
	BcmBatteryNonVolSettings *nv = BcmBatteryNonVolSettings::GetSingletonInstance();
	//gInfoMsg(*fMessageLogSettings, "CheckLifeTest") << "Battery " << whichBattery << "'s NV index = " << nvIdx << "." << endl;
	TNonVolBatteryInfo *batNVI = nv->GetBattery(nvIdx);
	//gInfoMsg(*fMessageLogSettings, "CheckLifeTest") << "Battery " << whichBattery << "'s GUID = 0x" << hex << fInfo.Batteries[whichBattery].guid << ", NV GUID = 0x" << batNVI->guid << "." << dec << endl;
    if (!(batNVI->flags & kBATTERYNVINFOVALID) || (batNVI->guid != fInfo.Batteries[whichBattery].guid)) 
	{
		//gInfoMsg(*fMessageLogSettings, "CheckLifeTest") << "Battery " << whichBattery << "'s NV info isn't valid." << endl;
		return;
	}
//#if (ENABLE_INFO_MSG == 1)
//	time_t now = time(0);
//	gInfoMsg(*fMessageLogSettings, "CheckLifeTest") << "Now = " << ctime(&now) << ", battery " << whichBattery << " due at " << ctime(&batNVI->NextUpdate) << endl;
//#endif
    if (difftime(time(0), batNVI->NextUpdate) < 0)
	{
//		gInfoMsg(*fMessageLogSettings, "CheckLifeTest") << "Battery " << whichBattery << " isn't due for a life test." << endl;
		return;
	}

	// battery is present and valid, lifetesting is enabled and supported, battery non-vol info seems correct,
	// and battery appears to need lifetesting
	if( (fLTInfo.LTState != kLTState_Idle)		// life testing already in progress (apparently on the other battery...see above)
	 || (fInfo.State != kBCSleep)               // or if Pico is not in sleep
	 || !AllBatteriesCharged())                // or if any packs are not already charged
	{
		gInfoMsg(*fMessageLogSettings, "CheckLifeTest") << "Life testing already underway, can't be started, or not all batteries are charged." << endl;
		Reschedule(whichBattery,1);
	}
	else
	{
		// it is OK to kick off life testing...
		gInfoMsg(*fMessageLogSettings, "CheckLifeTest") << "Kicking off life testing for battery " << whichBattery << "." << endl;
		nv->LifeTestCount(nvIdx, nv->LifeTestCount(nvIdx)+1); 
		nv->LastTestTime(nvIdx, time(0)); 
		fLTInfo.LTBattery = whichBattery;
		fLTInfo.LTState = kLTState_Begin;
	}
#endif
}

void BcmBatteryController::ServiceLifeTest()
{
#ifdef VENDOR_SUPPLIED_LIFE_TESTING
	VENDOR_SUPPLIED_LIFE_TESTER();
#else
	if ((fLTInfo.LTState >= kLTState_PTD) && (fLTInfo.LTState <= kLTState_Discharge) 
	&&  (!BatteryValid(fLTInfo.LTBattery) || !BatteryPresent(fLTInfo.LTBattery)))
		AbortLifeTest();
	else
	{
		switch (fLTInfo.LTState) {
		case kLTState_Idle: 		 break;
		case kLTState_Begin:         BeginLifeTest(); break;
		case kLTState_PTD:           MonitorPretestDischarge(); break;
		case kLTState_StartCharge:	 StartLifeTestCharge(); break;
		case kLTState_HWIMP:         MeasureHWImpedance(); break;
		case kLTState_MonitorCharge: MonitorCharge(); break;
		case kLTState_StartSWIMP:    StartSwImpedanceTest(); break;
		case kLTState_ComplSWIMP:    CompleteSwImpedanceTest(); break;
		case kLTState_Discharge:     MonitorDischarge(); break;
		case kLTState_Complete:      CompleteLifeTest(); break;
		default:
			gErrorMsg(*fMessageLogSettings, "ServiceLifeTest") << "Undefined life test state: " << fLTInfo.LTState << ", aborting the life test engine." << endl;
			AbortLifeTest();
		}
	}
#endif
}

#ifndef VENDOR_SUPPLIED_LIFE_TESTING
	void BcmBatteryController::BeginLifeTest()
	{
		fLTInfo.LTError = false;
		fLTInfo.LTData = new LTDataEntry[LTDischargeTableElements];  // one entry every 5 seconds for 12 hours
		if (!fLTInfo.LTData) {
			gErrorMsg(*fMessageLogSettings,"BeginLifeTest") << "Failed to allocate life test buffer" << endl;
			fLTInfo.LTError = true;
		}
		else
		{
			fLTInfo.LTDataIndex = 0;
			memset(fLTInfo.LTData,0,LTDischargeTableElements * sizeof(LTDataEntry));
	
			BcmBatteryNonVolSettings *nv = BcmBatteryNonVolSettings::GetSingletonInstance();
			float sampleMs = 5000.0 * fInfo.Batteries[fLTInfo.LTBattery].dischargeTable->designCapacity/2200.0;
			fLTInfo.LTSampleSeconds = (uint16)((sampleMs + 999)/1000);
			fLTInfo.prevSampleSeconds = nv->PollIntervalSeconds();
			gInfoMsg(*fMessageLogSettings,"BeginLifeTest") << "Setting poll interval during life test to " << sampleMs << " milliseconds" << endl;
			nv->PollIntervalSeconds(fLTInfo.LTSampleSeconds);
			gInfoMsg(*fMessageLogSettings,"BeginLifeTest") << "Disabling charge on both batteries." << endl;
			EnableChargeAll(false, kReasonLT); 

			gInfoMsg(*fMessageLogSettings,"BeginLifeTest") << "Starting a forced discharge on battery " << fLTInfo.LTBattery << endl;
			// start a forced discharge
			if (VToPct( fInfo.Batteries[fLTInfo.LTBattery].dischargeTable, fInfo.Batteries[fLTInfo.LTBattery].updateInfo.MeasuredVoltage) > 85)
			{
				float v = PctToV(fInfo.Batteries[fLTInfo.LTBattery].dischargeTable, 80);
				fLTInfo.LTError = !(BatteryForcedDischarge(fLTInfo.LTBattery, v));
				if (fLTInfo.LTError) {
					gErrorMsg(*fMessageLogSettings,"BeginLifeTest") << "Unable to start forced discharge" << endl;
				}
				else
				{
					gInfoMsg(*fMessageLogSettings,"BeginLifeTest") << "No problem starting forced discharge.  Setting next state = PreTestDischarge." << endl;
					fLTInfo.LTState = kLTState_PTD;
				}
			}
			else
			{
				// skip the pretest discharge and go directly to the forced charge
				gErrorMsg(*fMessageLogSettings,"BeginLifeTest") << "Skipping pretest discharge and going directly to forced charge" << endl;
				fLTInfo.LTState = kLTState_StartCharge;
			}
		}
		if (fLTInfo.LTError)
		{
			gErrorMsg(*fMessageLogSettings,"BeginLifeTest") << "Aborting Life Test due to earlier error" << endl;
			AbortLifeTest();
		}
		else
			gInfoMsg(*fMessageLogSettings, "BeginLifeTest") << "Life test sucessfully started on battery " << fLTInfo.LTBattery << "." << endl;
	}
	
	void BcmBatteryController::MonitorPretestDischarge()
	{
		// wait for the discharge to complete.  If sucessful, either
		// start a HW impedance test or start the charge
		if( fInfo.ChargerStatus & BS_ForcedDchgIP )
		{
			float volts = fInfo.Batteries[fLTInfo.LTBattery].updateInfo.LifeTestVoltage / 1000.0;

			volts += 0;		// gets rid of compiler warning when building a slim image!

			gInfoMsg(*fMessageLogSettings, "MonitorPretestDischarge") << "Still discharging (" << volts << "V)..." << endl;
			return;
		}
		if( !(fInfo.ChargerStatus & BS_ForcedDchgCompl ))
		{
			gErrorMsg(*fMessageLogSettings,"PreLifeTestDischarge") << "Pretest discharge failed or was aborted" << endl;
			AbortLifeTest();
		}
		else
		{
			gInfoMsg(*fMessageLogSettings, "MonitorPretestDischarge") << "Completed." << endl;
			fLTInfo.LTState = kLTState_StartCharge;
		}
	}
	
	void BcmBatteryController::StartLifeTestCharge()
	{
		// pre-test discharge is completed.  Let's start a forced charge to 95%
		// Start the charge cycle
		float v = PctToV(fInfo.Batteries[fLTInfo.LTBattery].dischargeTable, 95);
		fLTInfo.LTError = !BatteryForcedCharge(fLTInfo.LTBattery, v);
		if (fLTInfo.LTError)
		{
			gErrorMsg(*fMessageLogSettings,"StartLifeTestCharge") << "Unable to start forced charge...aborting life test" << endl;
			AbortLifeTest();
		}
		else
		{
			EnableChargeAll(true,kReasonLT);	// re-enable charging
			fLTInfo.LTState = kLTState_HWIMP;
		}
	}

	void BcmBatteryController::MeasureHWImpedance()
	{
		fLTInfo.RequiresSWImpedanceTest = true;
		if( fDriverCalls.pfMakeHWResistanceMeasurement != NULL )
		{
			gInfoMsg(*fMessageLogSettings,"MeasureHWImp") << "Calling bsp to make HW resistance measurement" << endl;
			fLTInfo.RequiresSWImpedanceTest = (fDriverCalls.pfMakeHWResistanceMeasurement( fLTInfo.LTBattery, &fLTInfo.Impedance) == FALSE);
			gInfoMsg(*fMessageLogSettings,"MeasureHWImp") << "Completed HWIMP call w/ " << fLTInfo.Impedance << " ohms" << endl;
		}
		if (!fLTInfo.RequiresSWImpedanceTest && ((fLTInfo.Impedance < kMinPackResistance) || (fLTInfo.Impedance > kMaxPackResistance))) 
		{
			gInfoMsg(*fMessageLogSettings,"MeasureHWImpedance") << "Measured impedance of " << fLTInfo.Impedance 
				<< " ohms is outside acceptable limits." << endl;
			fLTInfo.RequiresSWImpedanceTest = true;
		}
		if(fLTInfo.RequiresSWImpedanceTest)
			gInfoMsg(*fMessageLogSettings,"MeasureHWImp") << "Software resistance test will be used" << endl;
		else
			gInfoMsg(*fMessageLogSettings,"MeasureHWImp") << "Hardware resistance = " << fLTInfo.Impedance << endl;
	
		gInfoMsg(*fMessageLogSettings, "MeasureHWImp") << "Transitioning to charge monitor." << endl;
		fLTInfo.LTState = kLTState_MonitorCharge;
	}
	
	void BcmBatteryController::MonitorCharge()
	{
		// wait for the charge to complete.  If sucessful, start the "real" discharge
		//gInfoMsg(*fMessageLogSettings, "MonitorCharge") << "Charger status = 0x" << hex << fInfo.ChargerStatus << dec << endl;

		if( fInfo.ChargerStatus & BS_ForcedChgIP )
		{
			float volts = fInfo.Batteries[fLTInfo.LTBattery].updateInfo.LifeTestVoltage / 1000.0;
			float current = fInfo.BatteryCurrent;

			volts += 0;		// gets rid of compiler warning when building a slim image!
			current += 0;

			gInfoMsg(*fMessageLogSettings, "MonitorCharge") << "Still charging (" << volts << " V, " << current << " mA, state = " << fInfo.State << ")..." << endl;
			return;
		}
		if( !(fInfo.ChargerStatus & BS_ForcedChgCompl ))
		{
			gErrorMsg(*fMessageLogSettings,"MonitorCharge") << "Forced charge failed or was aborted...aborting life test" << endl;
			AbortLifeTest();
		}
		else
		{
			// Start the discharge cycle
			//float v = PctToV(fInfo.Batteries[fLTInfo.LTBattery].dischargeTable, 0);
			float v = bspNumSeriesCellsSupported() * kLifeTestMinVoltsPerCell;
			if( fDriverCalls.pfDcvbOnMillivolts)
			{
				// the additional 0.01V correction is to ensure forced discharge completes due to DEPLETED status
				v = (float)fDriverCalls.pfDcvbOnMillivolts(fLTInfo.LTBattery)/1000.0 - 0.01;
			}
			gInfoMsg(*fMessageLogSettings,"MonitorCharge") << "Forced charge completed...starting forced discharge" << endl;

			// reset default charge percentages
			fDriverCalls.pfSetChargePercentageAll(&fInfo, fInfo.ChargePercent);
			EnableChargeAll(false,kReasonLT);

			BcmOperatingSystemFactory::ThreadSleep(10);	// give Pico a chance to go back to sleep
			fLTSWImped_mV = (float)fInfo.Batteries[fLTInfo.LTBattery].updateInfo.LifeTestVoltage;
			fLTInfo.LTError = !BatteryForcedDischarge(fLTInfo.LTBattery, v);
			if (fLTInfo.LTError) 
			{
				gErrorMsg(*fMessageLogSettings,"MonitorCharge") << "Unable to start forced discharge...aborting life test" << endl;
				AbortLifeTest();
			}
			else
			{
				fLTInfo.LTState = kLTState_StartSWIMP;
			}
		}
	}
	
	void BcmBatteryController::StartSwImpedanceTest()
	{
		gInfoMsg(*fMessageLogSettings,"StartSWImped") << "Obtaining low-power watts..." << endl;
		if(!(fInfo.ChargerStatus & BS_ForcedDchgIP))
		{
			gErrorMsg(*fMessageLogSettings,"StartSWImped") << "Forced discharge failed or was aborted...aborting life test" << endl;
			AbortLifeTest();
		}
		else
		{
		//#if (POWER_MGMT)
		//	gInfoMsg(*fMessageLogSettings,"StartSwImpedanceTest") << "Informing power manager to immediately accept all power-related events." << endl;
		//	BcmPowerManagementThread::Singleton().ForceAcceptBatteryEvents(true);
		//#endif
			CollectLTData();
		//	SignalLowPower();
			fLTInfo.LTState = kLTState_ComplSWIMP;
		}
	}
	
	void BcmBatteryController::CompleteSwImpedanceTest()
	{
		gInfoMsg(*fMessageLogSettings,"CompleteSwImpedanceTest") << "Completing obtaining low-power watts." << endl;
		if(!(fInfo.ChargerStatus & BS_ForcedDchgIP))
		{
			gErrorMsg(*fMessageLogSettings,"CompleteSwImpedanceTest") << "Forced discharge failed or was aborted...aborting life test" << endl;
			AbortLifeTest();
		}
		else
		{
			//LTDataEntry *prevData = &fLTInfo.LTData[fLTInfo.LTDataIndex-1];
			CollectLTData();
			LTDataEntry *newData = &fLTInfo.LTData[fLTInfo.LTDataIndex-1];
			fLTInfo.LTLPWatts = ((float)newData->millivolts * (float)newData->milliamps)/1e6;
			if (fLTInfo.RequiresSWImpedanceTest)
			{
				gInfoMsg(*fMessageLogSettings,"CompleteSwImpedanceTest") << "Calculating SW impedance..." << endl;
				gInfoMsg(*fMessageLogSettings,"CompleteSwImpedanceTest") << "prevV = " << fLTSWImped_mV << "mV, newData = " << newData->millivolts << "mV" << endl;
				gInfoMsg(*fMessageLogSettings,"CompleteSwImpedanceTest") << "newData = " << newData->milliamps << "mA" << endl;
				float deltaV = (float)(fLTSWImped_mV - newData->millivolts) / 1000.0;
				float deltaI = fabs((float)(newData->milliamps) / 1000.0);
				gInfoMsg(*fMessageLogSettings,"CompleteSwImpedanceTest") << "DeltaV = " << deltaV << "V, deltaI = " << deltaI << "A" << endl;
				fLTInfo.Impedance = deltaV/deltaI;
				fLTInfo.Impedance -= SERIES_SENSE_RESISTOR;
				if (fLTInfo.Impedance < kMinPackResistance)
                {    
					gInfoMsg(*fMessageLogSettings,"CompleteSwImpedanceTest") << "Calculated impedance of " << fLTInfo.Impedance 
						<< " ohms is less than minimim.  Using " << kMinPackResistance << "instead." << endl;
					fLTInfo.Impedance = kMinPackResistance;
				}
				else if (fLTInfo.Impedance > kMaxPackResistance)
				{
					gInfoMsg(*fMessageLogSettings,"CompleteSwImpedanceTest") << "Calculated impedance of " << fLTInfo.Impedance 
						<< " ohms is greater than maximum.  Using " << kDefaultPackResistance << " instead." << endl;
					fLTInfo.Impedance = kDefaultPackResistance;
				}
				gInfoMsg(*fMessageLogSettings,"CompleteSwImpedanceTest") << "Final calculated impedance is " << fLTInfo.Impedance << " ohms" << endl;
			}
			float maxmV = PctToV(fInfo.Batteries[fLTInfo.LTBattery].dischargeTable,100) * 1000.0;
			if( (newData->millivolts + (fLTInfo.Impedance * newData->milliamps)) > maxmV)
			{
				gInfoMsg(*fMessageLogSettings,"CompleteSwImpedanceTest") << "Result impedance causes excessive maxV.  maxV = " << maxmV
					<< ", newMV = " << newData->millivolts << ", Z = " << fLTInfo.Impedance << ", newMA = " << newData->milliamps << endl;
				fLTInfo.Impedance = (maxmV - newData->millivolts) / newData->milliamps;
				gInfoMsg(*fMessageLogSettings,"CompleteSwImpedanceTest") << "Result impedance causes excessive maxV, setting to " 
					<< fLTInfo.Impedance << " instead." << endl;
			}
			//SignalNormalPower();
			//#if (POWER_MGMT)
			//	gInfoMsg(*fMessageLogSettings,"CompleteSwImpedanceTest") << "Informing power manager to react to power-related events using default behavior." << endl;
			//	BcmPowerManagementThread::Singleton().ForceAcceptBatteryEvents(false);
			//#endif
			fLTInfo.LTState = kLTState_Discharge;
		}
	}
	
	void BcmBatteryController::MonitorDischarge()
	{
		gInfoMsg(*fMessageLogSettings, "MonitorDischarge") << "Charger status = 0x" << hex << fInfo.ChargerStatus << dec << endl;
		if( fInfo.ChargerStatus & BS_ForcedDchgIP )
		{
			gInfoMsg(*fMessageLogSettings,"MonitorDischarge") << "Capturing data" << endl;
			CollectLTData();
			if ( fLTInfo.LTDataIndex >= LTDischargeTableElements )
			{
				gErrorMsg(*fMessageLogSettings,"MonitorDischarge") << "Out of room in the discharge monitor table...aborting life test" << endl;
				AbortLifeTest();
			}
		}
		else
		{
			if( !(fInfo.ChargerStatus & BS_ForcedDchgCompl ))
			{
				gErrorMsg(*fMessageLogSettings,"MonitorDischarge") << "Forced discharge failed or was aborted" << endl;
				fLTInfo.LTError = true;
			}
			CompleteLifeTest();
		}
	}
	
	void BcmBatteryController::CompleteLifeTest()
	{
		BcmBatteryNonVolSettings *nv = BcmBatteryNonVolSettings::GetSingletonInstance();
		fLTInfo.LTState = kLTState_Idle;
		BatteryStopForcedDischarge();		// depending on where we were called from, this may also be necessary
		if (fLTInfo.LTError)
		{
			gErrorMsg(*fMessageLogSettings,"CompleteLifeTest") << "Life test failed or was aborted...rescheduling for tomorrow" << endl;
			Reschedule(fLTInfo.LTBattery, 1);
		}
		else
		{
			gInfoMsg(*fMessageLogSettings,"CompleteLifeTest") << "Life test has completed..." << endl;
			uint16 impedance = (uint16)(fLTInfo.Impedance * 1000.0);
			int    nvIndex = fInfo.Batteries[fLTInfo.LTBattery].nvIndex;
			TNonVolBatteryInfo	*nvi = nv->GetBattery(nvIndex);
			gInfoMsg(*fMessageLogSettings,"CompleteLifeTest") << "...Nonvol index = " << nvIndex << endl;	
			gInfoMsg(*fMessageLogSettings,"CompleteLifeTest") << "...building new discharge table..." << endl;	
			BuildNewDischargeTable();
			nv->SeriesResistance(nvIndex, impedance);
			gInfoMsg(*fMessageLogSettings,"CompleteLifeTest") << "...compressing new discharge table..." << endl;	
			CompressDischargeTable(fInfo.Batteries[fLTInfo.LTBattery].dischargeTable, nvi);
			gInfoMsg(*fMessageLogSettings,"CompleteLifeTest") << "...rescheduling battery" << endl;	
			Reschedule(fLTInfo.LTBattery, nv->LifeTestPeriod());
		}
		if (fLTInfo.LTData != NULL) {
			delete [] fLTInfo.LTData;
			fLTInfo.LTData = NULL;
		}
		uint16 prevZone = DebugZone;
		DebugZone = 0xc002;
		fDriverCalls.pfSetChargePercentageAll(&fInfo, fInfo.ChargePercent);
		// clear charge errors coming out of life test - this may or may not actually be necessary
		if( fDriverCalls.pfClearChargeFaults) 
			fDriverCalls.pfClearChargeFaults(fLTInfo.LTBattery);
		EnableChargeAll(true, kReasonLT);
		DebugZone = prevZone;
		gInfoMsg(*fMessageLogSettings,"CompleteLifeTest") << "Resetting poll interval to " << fLTInfo.prevSampleSeconds << " seconds" << endl;	
		nv->PollIntervalSeconds(fLTInfo.prevSampleSeconds);
	}
	
	void BcmBatteryController::CollectLTData()
	{
		float temperature = (float)PackTemperature(fLTInfo.LTBattery);
		float voltage = (float)fInfo.Batteries[fLTInfo.LTBattery].updateInfo.LifeTestVoltage / 1000.0;
		float current = fabs((float)fInfo.BatteryCurrent / 1000.0);
		float watts = voltage * current;

		// gInfoMsg(*fMessageLogSettings,"CollectLTData") << "Idx = " << fLTInfo.LTDataIndex << ", Precomp: t = " << temperature << ", v = " << voltage << "V, i = " << current << "A, w = " << watts << "W" << endl;
		// temperature compensate voltage, then adjust current based on the new voltage to achieve the same power
		voltage = TemperatureCompensate(voltage, temperature);
		current = watts/voltage;
		gInfoMsg(*fMessageLogSettings,"CollectLTData") << "Idx = " << fLTInfo.LTDataIndex << ", Postcomp: t = " << temperature << ", v = " << voltage << "V, i = " << current << "A, w = " << watts << "W" << endl;
	
		// now store all values for later use in building the discharge table
		fLTInfo.LTAccumTemperature += temperature;
		fLTInfo.LTData[fLTInfo.LTDataIndex].millivolts = (uint16)(voltage * 1000);
		fLTInfo.LTData[fLTInfo.LTDataIndex].milliamps  = (uint16)(current * 1000);
		fLTInfo.LTDataIndex++;
	}
#endif // #ifndef VENDOR_SUPPLIED_LIFE_TESTING

	//*********************** ENGINEERING ACCESSIBLE ONLY **************************
	bool BcmBatteryController::ForceReadGUID(int whichPack, uint64 &guid)
	{
		// Battery value should be [0 .. MAX_BATTERIES-1]
		if((whichPack < 0) || (whichPack >= MAX_BATTERIES))
		{
			gLogMessageRaw << "WARNING: Parameter whichPack not in range when calling BcmBatteryController::ForceReadGUID().\n" << endl;
			return false;
		}
		TBatteryControllerInfo info;
		if(fDriverCalls.pfReadGUID != NULL)
		{
			if(fDriverCalls.pfReadGUID(&info,whichPack) == TRUE )
			{
				guid = info.Batteries[whichPack].guid;
				return true;
			}
			else
				gLogMessageRaw << "Error: ForceReadGUID: pfReadGUID returned false" << endl; 
		}
		else
			gLogMessageRaw << "Error: ForceReadGUID: pfReadGUID not supported" << endl; 

		return false;
	}

	bool BcmBatteryController::ForceReadEPROM(int whichPack, BcmString &eprom)
	{
		if (!fInitialized) return false;
		// Create a mutex auto-lock object.  This will automatically release
		// the lock when it goes out of scope (ie when we return).
		BcmMutexSemaphoreAutoLock Lock(fpMutex);
	
		// Battery value should be [0 .. MAX_BATTERIES-1]
		if((whichPack < 0) || (whichPack >= MAX_BATTERIES))
		{
			gLogMessageRaw << "WARNING: Parameter whichPack not in range when calling BcmBatteryController::ForceReadEPROM().\n" << endl;
			return false;
		}
		int ix;
		TBatteryControllerInfo info;
		for( ix =0; ix < 4; ix++ )
		{
			if (bspBatReadOWI(&info, whichPack, ix) == FALSE ) 
			{
				gLogMessageRaw << "WARNING: batteryEPROMRead() failed." << endl;
				return false;
			}
		}
		byte 	buffer[128];
		char	epromString[750] = "";		// 128 * 5 = 640 (space-delimited "0x"-prefixed ACSII-hex)
		char	temp[32];
		for( ix = 0; ix < 4; ix++ )
			memcpy(&buffer[ix*32], info.Batteries[whichPack].epromInfo.pageCache[ix], 32);
		for( ix = 0; ix < 128; ix++)
		{
			sprintf(temp, " 0x%02x", buffer[ix]);
			strcat(epromString, temp);
		}
		eprom = &epromString[1];	// removes leading space
		return true;
	}


//*********************** BATTERY HISTORY CLASS IMPLEMENTATION******************
TBatteryHistory::TBatteryHistory()
{
	history = (HistoryElement *)0;
	Init(13);		// presumes a 5-second poll interval
}

TBatteryHistory::~TBatteryHistory()
{
	if (history) delete [] history;
}

void TBatteryHistory::Init(unsigned int newMaxHistory)
{
	if (history) delete [] history;
	entryPtr = count = 0;
	maxElements = newMaxHistory;
	history = new HistoryElement[newMaxHistory];
}

void TBatteryHistory::AddHistory(const TBatteryControllerInfo &info)
{
	time_t now = time(0);
	if(info.Batteries[0].updateInfo.Status & kBatStatusValid)
		history[entryPtr].vbat[0] = info.Batteries[0].updateInfo.MeasuredVoltage/1000.0;
	else
		history[entryPtr].vbat[0] = 0.0;
	if(info.Batteries[1].updateInfo.Status & kBatStatusValid)
		history[entryPtr].vbat[1] = info.Batteries[1].updateInfo.MeasuredVoltage/1000.0;
	else
		history[entryPtr].vbat[1] = 0.0;
	history[entryPtr].onBattery = ((info.State == kBCDischarge) || (info.State == kBCForcedDischarge));
	history[entryPtr].secs = now - GetSystemStartTime();
	if (count < maxElements) count++;
	if(++entryPtr >= maxElements) entryPtr = 0;
}

void TBatteryHistory::SetMaxHistory(unsigned int newMaxElements)
{
	Init(newMaxElements);
}

void TBatteryHistory::PrintHistory() const
{
	char str[128];
	float accumA = 0.0, accumB = 0.0;
	unsigned int startPtr = 0;
	if (count >= maxElements) startPtr = entryPtr;

	cout << "Battery sample history for previous 60 seconds:" << endl;
	cout << "------------------------------------------------" << endl;
	for(unsigned int i = count; i > 0; i-- ) {
		HistoryElement *h = &history[startPtr];
		accumA += h->vbat[0]; accumB += h->vbat[1];
		sprintf(str, "%9ld: VA=%6.3f  VB=%6.3f  OnBattery=%s", h->secs, h->vbat[0], h->vbat[1], (h->onBattery ? "true" : "false")); 
		cout << str << endl; 
		if (++startPtr >= maxElements) startPtr = 0;
	}
	cout << "------------------------------------------------" << endl;
	if (count > 0)
		sprintf(str, " AVERAGES: VA=%6.3f  VB=%6.3f", accumA/count, accumB/count);
	else
		sprintf(str, " AVERAGES: VA=  0.000  VB=  0.000");
	cout << str << endl << endl;
}

void BcmBatteryController::BlockPolling(void)
{
	pfMutex->Lock();
}

void BcmBatteryController::UnblockPolling(void)
{
	pfMutex->Unlock();
}


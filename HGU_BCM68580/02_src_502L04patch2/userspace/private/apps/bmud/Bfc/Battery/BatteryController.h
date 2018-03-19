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
//  Filename:       BatteryController.h
//  Author:         Tom Johnson
//  Creation Date:  December 18, 2007
//
//****************************************************************************
//  Description:
//      High-level interface to the battery driver
//
//****************************************************************************
#ifndef BATTERY_CONTROLLER_H
#define BATTERY_CONTROLLER_H

#include "BfcHal.h"
#include "battery.h"
#include "BcmString.h"
#include "BatteryNonVolSettings.h"
#include <iostream>
#include <time.h>
#include "TemperatureCalibrator.h"

// PR22444 - all 5.5.2 boards default to 90% charge
#define kDefaultChargePercentage 90

// Forward reference...
class BcmMutexSemaphore;

class TBatteryHistory
{
private:
	typedef struct
	{
		float vbat[2];
		bool  onBattery;
		long  secs;
	} HistoryElement;
	unsigned int 	count, maxElements,	entryPtr;
	HistoryElement  *history;
	void Init(unsigned int newMaxHistory);
public:
	TBatteryHistory();
	virtual ~TBatteryHistory();
	void AddHistory(const TBatteryControllerInfo &info);
	void SetMaxHistory(unsigned int maxElements);
	void PrintHistory() const;
};

class BcmBatteryController
{
public:
	BcmBatteryController();
	~BcmBatteryController();

    BcmBatteryController &operator = (const BcmBatteryController &Controller);
	void GetBatteryControllerInfo (TBatteryControllerInfo &Info) const;

	int NumberSeriesCellsSupported();
	bool Initialize ();
	bool Initialized () const {return fInitialized;}
	bool DeInitialize ();
	const BcmString &Version () const {return fVersion;}
	bool Update();
	time_t BuildDateTime () const {return fInfo.BuildTime;}
	BatteryControllerState State () const;
	bool OperatingOnBattery () const;
	bool IsInDischargeState() const;
	int PercentChargeRemaining () const;
	int InputVoltage () const;
	int Temperature () const;
	int NominalDischargeTimePerBattery () const;
	int	 ControllerState() const;		// states of the various bits below
	// bit definitions for ControllerState()
	enum {
		// general stuff
		kBIT_OperatingOnBattery 	= (1 << 0),
		kBIT_BatteryChargeInhibited	= (1 << 1),
		kBIT_BatteryOutputDisabled 	= (1 << 2),
		kBIT_BatteryLow			 	= (1 << 3),
		kBIT_BatteryLowPercent	 	= (1 << 4),
		kBIT_BatteryDepleted	 	= (1 << 5),
		kBIT_BatteryBad				= (1 << 6),
		kBIT_Battery_A_Bad		 	= (1 << 7),
		kBIT_Battery_B_Bad		 	= (1 << 8),
		kBIT_BatteryMissing		 	= (1 << 9),
		kBIT_ChargeTemperatureDisabled 		= (1 << 10),
		kBIT_DischargeTemperatureDisabled 	= (1 << 11),
		kBIT_HiTempLowPower			= (1 << 12),
		// hardware faults
		kBIT_HardwareFaultDetected  = (1 << 21),
		#define TEMPERATURE_OOB (kBIT_ChargeTemperatureDisabled | kBIT_DischargeTemperatureDisabled | kBIT_HiTempLowPower)
	};
	
	bool UpdateState ();
	int AddAttachedDevice (const char *pDevice);
	time_t LastUpdate () const {return fLastUpdate;}
	const BcmString &AttachedDevices () const;
	int NumberOfAttachedDevices () const;
	bool BspSupportsLifeTesting () const { return fInfo.Support & kSupportsLifeTesting; }
	int NumberOfPresentBatteries () const;
	int NumberOfValidBatteries() const;
	int NumberOfValidatedBatteries() const;	// used when deciding whether to measure pack temperatures
	int NumberUCSBatteries() const;
	int NumberOfValidatedBatteriesWithoutErrors() const;
	unsigned long BatteryErrors (int Battery) const;
	int BatteryChargeCapacity (int Battery) const;
	int BatteryActualCapacity( int Battery) const;
	int BatteryFullChargeVoltage (int Battery) const;
	int BatteryDepletedVoltage (int Battery) const;
	int BatteryMeasuredVoltage (int Battery) const;
	int BatteryPercentCharge (int Battery) const;
	void SetGlobalPercentCharge (int pct);
	int BatteryEstimatedMinutesRemaining (int Battery) const;
	bool BatteryPresent (int Battery) const;
	bool BatteryValid (int Battery) const;
	bool BatteryBad (int Battery) const;
	bool BatterySelected (int Battery) const;
	bool BatteryFullyCharged (int Battery) const;
	bool BatteryChargeLow (int Battery) const;
	bool BatteryChargeLowPercent (int Battery) const;
	bool BatteryChargeDepleted (int Battery) const;
	bool BatteryChargeStateUnknown( int battery) const;
	int EstimatedMinutesRemaining (bool NormalizeToLow = true) const;
	int BatteryCurrent(void) const;
	bool BatteryChargingEnabled () const {return fChargingEnabled;}
	bool BatteryEnableCharging (bool Enable, bool Thermal = false, bool Silent = false);
	bool BatterySuspendCharging (bool Suspend);
	bool BatterySupportsPackThermistors(void);
	bool BatteryEnableChargingSingle (int battery, bool Enable);
	bool BatteryOutputEnabled () const {return fOutputEnabled;}
	bool BatteryEnableOutput (bool Enable, bool Thermal = false, bool silent = false);
	bool BatteryEPROMRead (int Battery, char * pBuffer, int BufferSize);
	const TBatDischargeTable *BatteryDischargeTable(int Battery) const;
	const TNonVolBatteryInfo *BatteryNonVolInfo( int Battery ) const;
	bool LowPowerRequiredDueToHighTemperature();
	void CheckForLowBattery(bool avoidHysteresis);
	bool SupportsHardwareFaults() const { return (fInfo.Support & kSupportsHWFaults) != 0; }
	bool SupportsLifeTesting() const { return (fInfo.Support & kSupportsLifeTesting) != 0; }
	bool SupportsBatteryNonVol() const { return (fInfo.Support & kSupportsBatteryNonvol) != 0; }
	bool LifeTestingEnabled() const;
	uint32 BatteryLifeTestCount(int whichBat) const;
	time_t BatteryLastLifeTest(int whichBat) const;
	BcmString BatteryLastLifeTestString(int whichBat) const;
	time_t BatteryNextLifeTest(int whichBat) const;
	BcmString BatteryNextLifeTestString(int whichBat) const;
	bool BatteryNextLifeTest(int whichBat, const BcmString &dateStr);
	int BatteryLifeTestState(int whichBat) const;
	int LifeTestState();
	void AbortLifeTest();
	uint64 GetGUID(int whichBat) const;
	uint16 GetAllowedFaultsMask() const;
	uint16 GetFaults() const;
	void SetFaultMask(uint32 faultmask);
	void ClearAllFaults() const;
	void DiagnoseState() const;
	void ClearChargeErrors(int whichBat) const;
	typedef enum
	{
	  kBatteryEpromRevision,    // Page 0 parameters
	  kBatteryPackSupplier,
	  kBatteryModelNumber,
	  kBatterySerialNumber,
	  kBatteryPcbRev,
	  kBatteryCellSupplier,    // Page 1 parameters
	  kBatteryCellCapacity,
	  kBatteryCellDateCode,
	  kBatteryPackDateCode,
	  kEpromPageMask
	} BatteryEpromParameter;
	BcmString GetBatteryEpromParameter(int Battery, BatteryEpromParameter Param);
	bool BatteryForcedDischarge (int Battery) const;
	bool BatteryForcedDischarge (int Battery, float toVolts) const;
	bool BatteryStopForcedDischarge (void) const;
	bool BatteryForcedCharge (int Battery);
	bool BatteryForcedCharge (int Battery, float toVolts);
	bool BatteryForcedChargePct (int Battery, float toPct);
	bool BatteryStopCharge (void);
	int PackTemperature(int whichPack) const;		// AVERAGE pack temperature
	float BoardTemperature() const;					// INSTANTANEOUS board temperature
	float BoardLevelThermistorTemperature() const;	// returns AVERAGE calibrated BLT
    ostream &Print(ostream &outStream) const;

	void DischargeTimeScalingFactor(float &scalingFactor) {}	// preserved for historical reasons
	void PrintHistory () const {history.PrintHistory();}
	void UpdateMaxHistory(uint32 pollSecs);

	void BlockPolling(void);
	void UnblockPolling(void);

protected:
	BcmMutexSemaphore 					*fpMutex;
	BcmMutexSemaphore 					*pfMutex;
	TBatteryControllerInfo				fInfo;
	TMVLimits							fMVLimits;
	TBatteryDriverCalls					fDriverCalls;
	TBLTtoBoardTemperatureCalibrator	fBlt2BoardCal;
	TBLTtoPackTemperatureCalibrator		fBlt2PackCal;
#ifdef BFC_INCLUDE_THERMAL_MONITOR_SUPPORT
	TTSCtoBoardTemperatureCalibrator	fTsc2BoardCal;
	TTSCtoPackTemperatureCalibrator		fTsc2PackCal;
#endif	
	bool 	fInitialized;
	time_t	fLastUpdate;
	BcmString	fVersion;
	bool 	fChargingEnabled;
	bool 	fOutputEnabled;
	int		fDiagBLTTemperature;
	int		fDiagPLTTemperature[2];
//	int 	fDiagTemperature;
	int 	fDiagBatteryErrors[2];
	int 	fPackTemperatureSmoothingSteps;
	int 	fBoardTemperatureSmoothingSteps;
	int 	fPackVoltageSmoothingSteps;
	int		fBoardVoltageSmoothingSteps;
	BcmString fAttachedDevices;
	int		fNumberOfAttachedDevices;
	int		fNumBatteries, fNumUCS;	// we remember these from poll-to-poll
	float 	fPackLowVoltageFlagLimit;

#ifndef VENDOR_SUPPLIED_LIFE_TESTING
	enum {
		kLTState_Idle = 0,      
		kLTState_Begin,     
		kLTState_PTD,       
		kLTState_StartCharge,    
		kLTState_HWIMP,     
		kLTState_MonitorCharge,    
		kLTState_StartSWIMP,
		kLTState_ComplSWIMP,
		kLTState_Discharge, 
		kLTState_Complete
	};
	
	typedef struct {
		uint16 	millivolts;
		uint16 	milliamps;
	} LTDataEntry;
	
	typedef struct {
		int			LTState;			// one of the above enum values
		int			LTBattery;			// the battery being tested
		bool		LTError;
		bool		RequiresSWImpedanceTest;
		uint16		LTSampleSeconds;
		uint16		prevSampleSeconds;
		int			LTDataIndex;
		LTDataEntry	*LTData;
		float		LTAccumTemperature;	// accumulates temperature for use in calculating average temperature 
		float		Impedance;
		float		LTLPWatts;
	} TLifeTestInfo;
	TLifeTestInfo	fLTInfo;
	float           fLTSWImped_mV;
#endif
	TBatteryHistory	history;
	friend class BcmBatteryThread;
    friend class BcmBatteryCommandTable;
	friend class BcmBatteryATECommandTable;
	friend class BcmBatteryNonVolSettings;
	friend class batteryEngrEntryBridge;
private:
	void SetDiagTemperature(int temp) {
		fDiagBLTTemperature = fDiagPLTTemperature[0] = fDiagPLTTemperature[1] = temp;
	}
	bool VerifyGUIDCRC(uint64 GUID);
	void SmoothInputVoltage(TBatteryControllerInfo &info);
	void SmoothPackVoltages(TBatteryControllerInfo &info);
	void SmoothPackTemperatures(TBatteryControllerInfo &info);
	void CalculateBoardTemperature(TBatteryControllerInfo &info);
	void CheckChargerVoltageLow(bool avoidHysteresis);
	void CheckChargerVoltageDepleted(bool avoidHysteresis);
	void CheckChargerPctLow(bool avoidHysteresis);
	void CheckBadBatteries();
	void CheckAlerts();
	void CheckBatteryAlerts(bool firstPoll);
	void CheckTemperatureAlerts(bool firstPoll);
	void CheckVoltageAlerts(bool firstPoll);
	void CheckHardwareAlerts();
	void CheckPackTemperatureExtremes(bool *globalChgInhibit, bool *globalDchgInhibit);		// SWCM-13890
	void CheckPerPackTemperatureExtremes(bool *globalChgInhibit, bool *globalDchgInhibit);	// ditto
	void EnableAlert( int whichAlert, int state );
	void ValidateBattery(TBatteryControllerInfo &info, int which);
	uint16 CalcEpromChecksum(TEpromInfo &info, int startPage);
	int CheckNonVolInfo(int which);
	void DeterminePackCapacity(int which, TPackValidation &vs);
	bool AllBatteriesCharged();
	void CheckLifeTest(int whichBattery );
	void CheckUpdateNextLifeTest();
	void ServiceLifeTest();
	void SignalLowPower();
	void SignalNormalPower();
	void UpdateState(int stateBit, bool newState, int eventIftrue, int eventIfFalse);

	enum {
		kBoardTempSmoothingSteps,
		kPackTempSmoothingSteps,
		kPackVoltageSmoothingSteps,
		kBoardVoltageSmoothingSteps,
	};
	int SmoothingSteps(int stepType, int val);
	int SmoothingSteps(int stepType);

	struct _PackFlags{
		bool	temperaturechargeinhibit;
		bool	temperaturedischargeinhibit;
		bool	lifettestchargeinhibit;
		bool	lifettestdischargeinhibit;
	} fPackStateFlags[2];
	int		fControllerState;
	void RaiseAlert(int alarm);
	// these bits are used in EnableChargeAll and EnableDischargeAll:
	enum {
		kReasonThermal = (1 << 0),
		kReasonManual  = (1 << 1),
		kReasonLT      = (1 << 2)
	};
	void EnableDischargeAll(bool enable, int reason);
	void EnableChargeAll(bool enable, int reason);
	void ForceWatchdogTimeout();
	void ForceLifeTest(int whichBat);
	uint16 RawControllerState();
	bool MeasureHWImpedance(unsigned int bat, float &z) const;
	void DumpBatteryDischargeTable(int whichBat);

#ifndef VENDOR_SUPPLIED_LIFE_TESTING
	void BeginLifeTest();
	void MonitorPretestDischarge();
	void StartLifeTestCharge();
	void MeasureHWImpedance();
	void MonitorCharge();
	void StartSwImpedanceTest();
	void CompleteSwImpedanceTest();
	void MonitorDischarge();
	void CompleteLifeTest();
	void CollectLTData();
	void BuildNewDischargeTable();
#endif
	void Reschedule(int which, int days);
	void RescheduleAtLeast(int which, int days);
	void RescheduleByNVI(int whichNVI, int days);

	// engineering accessable functions
	bool ForceReadGUID(int whichPack, uint64 &guid);
	bool ForceReadEPROM(int whichPack, BcmString &eprom);
};
//********************** Inline Method Implementations ***********************

inline ostream & operator << (ostream &outStream, const BcmBatteryController &Controller)
{
  return Controller.Print(outStream);
}

#endif // BATTERY_CONTROLLER_H


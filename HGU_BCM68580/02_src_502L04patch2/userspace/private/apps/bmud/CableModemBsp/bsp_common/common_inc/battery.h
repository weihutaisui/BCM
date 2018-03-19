//****************************************************************************
//
// Copyright (c) 2008-2013 Broadcom Corporation
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
//  Filename:       battery.h
//  Author:         Kevin O'Neal
//  Creation Date:  February 3, 2005
//
//****************************************************************************
//  Description:
//      Header file with public battery controller APIs
//
//****************************************************************************

#ifndef BATTERY_H
#define BATTERY_H

#include "board.h"
#include "MergedNonVolDefaults.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "bcmtypes.h"
#include <time.h> // for time_t type

// this is one that may not be defined
#ifndef uint64
	#define uint64 unsigned long long
#endif

	// various default temperature limits - may be overridden in MergedNonvolDefaults.h
#ifndef kDefaultValue_BatteryChargeSuspensionHiTemp
	#define kDefaultValue_BatteryChargeSuspensionHiTemp 45
#endif
#ifndef kDefaultValue_BatteryChargeSuspensionLoTemp
	#define kDefaultValue_BatteryChargeSuspensionLoTemp 0
#endif
#ifndef kDefaultValue_BatteryDischargeSuspensionHiTemp
	#define kDefaultValue_BatteryDischargeSuspensionHiTemp 60
#endif
#ifndef kDefaultValue_BatteryDischargeSuspensionLoTemp
	#define kDefaultValue_BatteryDischargeSuspensionLoTemp -10
#endif
#ifndef kDefaultValue_BatteryLowPowerModeHiTemp
	#define kDefaultValue_BatteryLowPowerModeHiTemp     80 // degrees C
#endif

#define TEMP_MEASUREMENT_NOT_SUPPORTED 0x7fff

// PR24004
#ifndef MAX_BATTERIES
	#define MAX_BATTERIES 2
#endif

#define SENSE_RATIO ((float)kBatterySenseNumerator/(float)kBatterySenseDenominator)
#define SERIES_SENSE_RESISTOR (0.1 / SENSE_RATIO)

typedef struct {
	float 	measuredTemperature;	// deg C
	float	calibratedTemperature;	// deg C
} TTemperatureCalTableEntry;

// Enumerated values for BatteryControllerInfo.State
typedef enum
{
  kBCInit = 0,
  kBCIdle,
  kBCSleep,
  kBCChargeInit,
  kBCPrecharge,
  kBCFastCharge,
  kBCTopoff,
  kBCChargeSuspended,
  kBCDischarge,
  kBCEternalSleep,
  kBCForcedDischarge,
  kBCTrickleCharge		// deprecated
} BatteryControllerState;

#if 1
// Charger status values
#define BS_STATUS_VALID	 	0x0001
#define BS_LOW_BAT_WARNING	0x0002
#define BS_IMMINENT_FAILURE 0x0004
#define BS_BatAPresent		0x0008
#define BS_BatBPresent		0x0010
#define BS_ForcedDchgIP		0x0020
#define BS_ForcedDchgCompl	0x0040
#define BS_ForcedChgIP		0x0080
#define BS_ForcedChgCompl	0x0100
#define BS_AC_ABSENT		0x0200
#define BS_SecureBootIP 	0x0400
#define BS_PWR_ON_RESET	 	0x0800
#define BS_STATUS_SPI_ERROR	0x1000
#endif

// all of these are in ADC counts.  You can use the VinPrecision field in the
// TBatteryControllerInfo structure to convert to mV
typedef struct
{
	uint16 infoValid;	// 0 = not valid, 1 = valid
	uint16 MV_CHG_ON;	// ADC: minimum mains voltage required to charge
	uint16 MV_CHG_OFF;	// ADC: voltage below which charging is disabled/inhibited
	uint16 MV_DCHG_ON;	// ADC: mains voltage below which we go to discharge
	uint16 MV_DCHG_OFF;	// ADC: mains voltage above which we leave discharge
	uint16 MVSYS_MIN;	// ADC: absolute minimum system voltage
	uint16 MVSYS_MAX;	// ADC: absolute maximum system voltage
	uint16 MVMAX_IDLE;	// ADC: voltage above which we won't leave idle
} TMVLimits;

typedef struct 
{
	float 	voltage;
	float 	whRemaining;
} TBatDchgTableEntry;

// each battery will have a discharge table
typedef struct
{
	float				designCapacity;	// mAh
	float				actualCapacity; // mAh
	float				totalMinutes;
	float				maxWh;
	uint16				seriesCells;
	uint16				parallelChains;
	uint16				batZ;			// in milli-ohms
	TBatDchgTableEntry 	table[101];
} TBatDischargeTable;

typedef struct {
	uint8	ValidFlags;			// bits [3..0]...one per page
	uint8	pageCache[4][32];
} TEpromInfo;

// a copy of this is kept in NonVol for each battery. Order is important
// to minimize storage
typedef struct {
	uint8	flags;				// bit 7 = (0 = nvinfo is invalid, 1 = nvinfo is valid)
				#define kBATTERYNVINFOVALID 0x80
	uint8	EstLifeRemaining;	// % (100..0) = most recent mAh/Capacity_mAh
	uint16	DesignCapacity;		// mAh from either EPROM or a default value
	float	maxWh;				// used to build the wattHours column of the discharge table
	uint64	guid;
	uint32	LifeTestCount;		// Number of times Life Test was started
	time_t	LastLifeTestStarted;// date when the last Life Test was started
	time_t	NextUpdate;         // Date when the next life test is scheduled
	float	wHCorrectionRatio;	// applied to wH - initially capacity/2200, later 1 (following life test)
	uint32	totalSeconds;		// total seconds represented by in the compresed discharge table		
	uint16	seriesResistance;	// in milliOhms
	uint16	dchgVoltages[101];	// this is the volts column of the discharge table
} TNonVolBatteryInfo;

// battery status bits as seen by the controller.  BSP's are responsible
// for emulating any bits that are in different positions in their private
// interface
enum	
{
	kBatStatusPresent			= (1 << 0),		// this battery is present
	kBatStatusValid				= (1 << 1),		// this battery is valid
	kBatStatusUCS				= (1 << 2),		// this battery has an unknown charge state
	kBatStatusFull				= (1 << 3),		// this battery is full
	kBatStatusDepleted			= (1 << 4),		// this battery is depleted
	kBatStatusSelected			= (1 << 5),		// this battery is currently selected
	kBatStatusChgDisabled		= (1 << 6),		// per-pack charging disabled
	kBatStatusDchgDisabled		= (1 << 7),		// per-pack discharging disabled
	// see #defines below for charge errors and pack status
	kBatStatusPresenceChanged	= (1 << 15),	// kBatStatusPresent has changed since the last update
};

// some handy definitions
#define kPackStatusValidityShift 	11
#define kPackStatusValidityMask		0x03
#define kPackStatusValidityMissing	0
#define kPackStatusValidityUnknown	1
#define kPackStatusValidityValid	2
#define kPackStatusValidityInvalid	3

// this is per-pack info that may change from poll to poll
// we make a separate struct so it is easy to zero it out on each poll
typedef struct {
	uint16 				Status;      		  // bitmask with bits from above
	uint16 				Errors;      		  // Number of charger errors
	float 				MeasuredVoltage;      // mV
	float				LifeTestVoltage;	  // mV - raw voltage prior to any discharge correction
	float 				Temperature;	 	  // current pack temperature (if supported)
} TBatteryUpdateInfo;

// SWCM-13890
typedef struct {
	uint16 ThermB;	// kelvin - default = 3380
	uint16 ThermR0;	// Ohms - default = 10000
	uint16 ThermT0;	// degC - default = 25
	int    Coff_Lo;	// degC - default = kDefaultValue_BatteryChargeSuspensionLoTemp
	int    Coff_Hi;	// degC - default = kDefaultValue_BatteryChargeSuspensionHiTemp
	int    Doff_Lo;	// degC - default = kDefaultValue_BatteryDischargeSuspensionLoTemp
	int    Doff_Hi;	// degC - default = kDefaultValue_BatteryDischargeSuspensionHiTemp
} TBatteryThermalInfo;

// ALL per-battery information
typedef struct {
	uint8				nvIndex;
	// this is per-pack info that doesn't change very much (if ever)
	uint16				FCVTERM;			  // may change every 90 days
	uint16				VINIT;				  // ditto
	uint16			  	EpromCapacity;        // mAH - directly from the battery
	uint64				guid;				  // the GUID as read from non-vol (a copy is in non-vol)
	float				mAhPercent;           // Charge level as % of max mAH
//	uint16  			FullVoltage;          // mV
//	uint16  			DepletedVoltage;      // mV
	TBatteryUpdateInfo	updateInfo;
	// this is the per-pack discharge table (null if unused)
	TBatDischargeTable	*dischargeTable;
	// now the per-pack EPROM contents
	TEpromInfo			epromInfo;
	// now the per-pack thermal information
	TBatteryThermalInfo	thermalInfo;		  // SWCM-13890
	// the following is initialized & controlled ONLY by the controller - NEVER the driver!
	struct {
		float  average_pack_temperature;
		float  average_pack_voltage;
		int	   num_temperature_samples;
		int	   num_voltage_samples;
	} controllerSpecific;
} TBatteryInfo;

enum {
	kSupportsLifeTesting 			 = (1 << 0),
	kSupportsTwoCellBatteries		 = (1 << 1),
	kSupportsPackThermistors		 = (1 << 2),
	kSupportsPerPackDischargeTable	 = (1 << 3),
	kSupportsBatteryNonvol			 = (1 << 4),
	kSupportsBfcPackValidation		 = (1 << 5),
	kSupportsDischargeCurrent		 = (1 << 6),	// ALL platforms support CHARGE current
	kSupportsAdapterCurrent			 = (1 << 7),
	kSupportsHWFaults				 = (1 << 8),
	kSupportsPerPackEnables			 = (1 << 9),
	// all other values (10..31) are reserved for future use
};

enum HardwareFaultBits {
    kCOV_FAULT		= 0x0001,
    kCOI_FAULT		= 0x0002,
    kCUI_FAULT 		= 0x0004,	// currently not used
    kPVR_FAULT 		= 0x0008,	// currently not used
    #define CHG_FAULTS 0x000f

	kDOI_FAULT 		= 0x0010,
	kDHF_FAULT 		= 0x0020,
	kIVP_FAULT 		= 0x0040,	// currently not used
	kIIR_FAULT 		= 0x0080,	// currently not used
    #define DCHG_FAULTS 0x00f0

	kIOV_FAULT 		= 0x0100,
	kVWE_FAULT 		= 0x0200,
	kUIE_FAULT 		= 0x0400,   // currently not used
	kBOV_FAULT 		= 0x0800,	
    #define GEN_FAULTS  0x0f00

	#define BFC_FAULTS  0xf000	// defined, set, and cleared ONLY by the BFC
};

// data passed around by the BatteryController
typedef struct _BatteryControllerInfo {
	char					*Version;			// this are filled in by the BSP at Init time and are never modified thereafter
	time_t					BuildTime;			// "
	uint32					Support;			// Indicates what sorts of features the driver supports - see bit defines above
	uint32					ChargePercent;		// this is the value originally passed into the controller at INIT time
	uint32					PollSeconds;		// this is the poll period - especially useful on some BSP's when starting up
	uint16 					Flags; 				// For special instructions at update time
	uint16					VbatPrecision;   	// mV per ADC
	uint16					VbatOffset;			// mV represented by 0 ADC counts - only needed by the 3385, this will be 0 for everyone else
	uint16					VinPrecision;		// mV per ADC
	uint16					VinOffset;			// mV represented by 0 ADC counts - only needed by the 3385, this will be 0 for everyone else
	uint16					InputVoltage; 		// mV
	short					AdapterCurrent;		// mA, if available.  If not measurable returns 0
	short					BatteryCurrent;		// mA
	short 					BLTTemperature;		// UNCORRECTED DegC, if available.  If not measurable returns TEMPERATURE_MEAS_NOT_SUPPORTED 
	short					FanTemperature;		// THERMC on the 3385 - not used by anyone else
	uint16					RawControllerState;	// mostly just for informative purposes
	BatteryControllerState 	State; 				// from enum above
	uint16					ChargerStatus;		// see Charger status values above - all other BSP's have to emulate to this structure
	uint16					Faults;				// IEEE1625 faults that are currently detected
	uint16					FaultsAllowed;		// IEEE1625 faults currently allowed mask
	uint16					WatchdogResets;		// only used on platforms that support the microcode Watchdog
	uint16					WatchdogReloads;
	float 					NominalDchgWLowPwr;	// The nominal power draw when in low power
	int 					NumBatteries; 		// 0 ... 2 batteries CURRENTLY present in the system
	float					CalBLTTemperature;	// Calibrated BLT temperature calculated by controller
	// the following is initialized & controlled ONLY by the controller - NEVER the driver!
	struct {
		float  average_board_temperature;
		float  average_board_voltage;
		float  average_pseudo_pack_temperature;
		int	   num_board_voltage_samples;
		int	   num_board_temperature_samples;
		int    num_pseudo_pack_temperature_samples;
	} controllerSpecific;
	TBatteryInfo			Batteries[2]; 		// array, 1 for each of a maximum of 2 batteries - changed from MAX_BATTEROES by PR24004
} TBatteryControllerInfo;

// the following structure is filled in by the controller based on general validation of the EPROM (when external
// pack validation is being used).  If this initial validation passes, the structure is passed to the BSP via the
// pfValidatePack call to allow the BSP to complete validation
typedef struct _PackValidation {
	byte page1CS_valid;		// bit 0 = low byte validity, bit 1 = upper byte validity
	byte page3CS_valid;		// bit 0 = low byte validity, bit 1 = upper byte validity
	byte guidOK;			// either a) GUID isn't supported or b) GUID has been read and validated
	byte pagesMatch;		// page 0 is identical to page 2, and page 1 id identical to page 3
	byte generation;		// 1 - Comcast original, 2 = CC w/ BRCM config, 3 = New BRCM (page 2/3 are binary)
	byte seriesCells;
	byte parallelChains;
} TPackValidation;

// the following structure is used by BSP's that allow adjustment of various parameters based on pack-specific criteria
// the order and size of the parameters is IDENTICAL to generation 3 pack-specific information stored in eprom
// ref "Rechargable Battery Pack System Requirements - 2600mAh Three Cell LiIon Battery Pack" version 1.0.1, May 19, 2009
typedef struct {
	uint16 doiLimitA;	// ma
	uint16 doiLimitB;	// ma
	uint16 doiLimitC;	// ma
	uint16 doiTimeA;	// ms
	uint16 doiTimeB;	// ms
	uint16 pqiChg;		// ma
	uint16 fciChg;		// ma
	uint16 tciTerm;		// ma
	uint16 pqvTerm;		// mv
	uint16 vdepleted;	// mv (dcvbon)
	uint16 mavV;		// mv
	char   coff_hi;		// degC
	char   coff_lo;		// degC
	char   doff_hi;		// degC
	char   doff_lo;		// degC
	uint16 thermB;		// degK
	uint16 thermR0;		// Ohms
	uint16 thermT0;		// degC
	// these next parameters are NOT in generation 3 packs
	uint16  fciTerm;	// ma
	uint16 	coiLimit;	// ma
    uint16  dcvb_off;   // mv
    uint16  low_warn;   // mv
} TPackParams;

// The controller will fill in this structure with null function pointers at 
// construction, then will pass it to the driver in the bspBatInit() call. Any 
// calls that a driver doesn't want to handle should be left alone.
// In this manner, drivers that can't support one or more features - including
// new features added in the future - don't need any modifications whatsoever.
typedef struct _BatteryDriverCalls {
	BOOL (*pfEnableChargingAll) ( BOOL Enable );
	BOOL (*pfSetChargePercentageAll) ( TBatteryControllerInfo *info, int Percent );
	BOOL (*pfSetChargePercentageSingle) ( TBatteryControllerInfo *info, int battery, int Percent );
	BOOL (*pfStartForcedCharge) ( TBatteryControllerInfo *info, int battery, float toVolts );
	BOOL (*pfStopCharge) ( void );
	BOOL (*pfEnableDischargeAll) ( BOOL Enable );
	BOOL (*pfStartForcedDischarge) ( const TBatteryControllerInfo *info, int battery, float toVolts );
	BOOL (*pfStopForcedDischarge) ( void );
	BOOL (*pfReadGUID) (TBatteryControllerInfo *info, int which );
	BOOL (*pfMakeHWResistanceMeasurement) ( int which, float *result );
	BOOL (*pfValidatePack) (int which, BOOL validity);	// sets PIco pack validity to true/false - NO decision making!
	uint16 (*pfGetHWFaults) ( void );					// allows ONLY faults whose bits are SET in the mask
	BOOL (*pfSetHWFaultMask) (uint32 mask);				// allows ONLY faults whose bits are SET in the mask
	uint16 (*pfGetHWFaultMask) ( void );
	BOOL (*pfClearHWFaults) (uint32 mask);  			// clears ALL faults whose bits are SET in the mask
	void (*pfForceWatchdogTimeout)( void );				// forces a microcontroller watchdog timeout
	void (*pfMainsVoltages) (TMVLimits *limits);		// fills in the mains voltage limits
	BOOL (*pfAdjustCellConfig)(TBatteryControllerInfo *info, int whichBat, TPackParams *chgParams);
	void (*pfClearChargeFaults)(int whichBat);
	BOOL (*pfCompletePackValidation)(TBatteryControllerInfo *info, int whichBat, TPackValidation *vs);
	void (*pfSetPackChargeInhibit)(TBatteryInfo *info, int whichBat, BOOL inhibit);
	void (*pfSetPackDischargeInhibit)(TBatteryInfo *info, int whichBat, BOOL inhibit);
	BOOL (*pfPartIsTrimmed)( void );
	int  (*pfDcvbOnMillivolts)(int whichBat);
} TBatteryDriverCalls;

// the following are interfaces ALL drivers MUST support!
extern BOOL bspBatInit(TBatteryControllerInfo *info, TBatteryDriverCalls *calls);
extern BOOL bspBatDeinit(TBatteryControllerInfo *info);
extern uint32 bspBatApmIsr(void);
extern BOOL bspBatUpdate(TBatteryControllerInfo *info);
extern BOOL bspBatReadOWI(TBatteryControllerInfo *info, int which, int page);
extern BOOL bspUtilPowerIsPresent();
extern BatteryControllerState bspCurrentState();
extern int bspNumSeriesCellsSupported();
extern void bspSuspendCharging(BOOL Suspend);

// the following are utility functions defined in bsp_common/common_src/batteryCommon.c
extern float BltAdc2DegC( uint16 adcVal, uint16 limitingADC );
extern float PctToV( const TBatDischargeTable *table, float pct );
extern float VToPct( const TBatDischargeTable *table, float volts );
extern float PctToWh( const TBatDischargeTable *table, float pct );
extern float VToWh( const TBatDischargeTable *table, float volts ); 
extern TBatDischargeTable *CreateDischargeTable(const TBatDischargeTable *fromTable);
extern void CopyDischargeTable( TBatDischargeTable *toTable, const TBatDischargeTable *fromTable);
extern void CompressDischargeTable(const TBatDischargeTable *fromTable, TNonVolBatteryInfo *nvi);
extern void DecompressDischargeTable(const TNonVolBatteryInfo *nvi, TBatDischargeTable *toTable);
extern void DumpDischargeTable( const TBatDischargeTable *table );
extern void NDumpDischargeTable( const TBatDischargeTable *table, int entries );
extern const TBatDischargeTable *FindBestDefaultDischargeTable(int capacity, int seriesCells, int parallelChains);
extern void ScaleDischargeTable(TBatDischargeTable *inTable, int epromCapacity);

// the default discharge tables are defined in batteryDischargeTables.h:
extern const TBatDischargeTable default3cell2200mAhDischargeTable;
extern const TBatDischargeTable default2cell2200mAhDischargeTable;
extern const TBatDischargeTable default2S2PDischargeTable;
extern const TBatDischargeTable defaultMitac2600mAh3S1PDischargeTable;
extern const TBatDischargeTable defaultGetac2250mAh3S1PDischargeTable;
extern const TBatDischargeTable defaultGetac2600mAh2S1PDischargeTable;
extern const TBatDischargeTable *defaultDischargeTables[];	// array of the above tables

#ifdef __cplusplus
}
#endif

#endif

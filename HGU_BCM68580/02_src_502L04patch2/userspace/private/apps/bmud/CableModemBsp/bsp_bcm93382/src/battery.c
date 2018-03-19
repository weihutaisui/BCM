//****************************************************************************
//
// Copyright (c) 2009-2010 Broadcom Corporation
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
//  Filename:       battery.c
//  Author:         Kevin O'Neal
//  Creation Date:  February 10, 2005
//
//****************************************************************************
//  Description:
//      Implements battery management driver for the 3368 platform
//
//****************************************************************************

#include "3382_map.h"
#include "3382_BatteryRegs.h"
#include "bcmos.h"
#include "SystemTime.h"
#include "battery.h"
#include "MergedNonVolDefaults.h"

#undef MAX_BATTERIES
#define MAX_BATTERIES 2
#ifndef kBatteryNumberOfCells
	#define kBatteryNumberOfCells 3
#endif
#ifndef kBatterySenseNumerator
	#define kBatterySenseNumerator 1
#endif
#ifndef kBatterySenseDenominator
	#define kBatterySenseDenominator 1
#endif
#ifndef kDefaultPackResistance
	#define kDefaultPackResistance 0.4
#endif
#ifndef kMIN_DETECTION_POLLS
	#define kMIN_DETECTION_POLLS 2
#endif
#ifndef USE_ONLY_TRIMMED_PARTS
	#define USE_ONLY_TRIMMED_PARTS TRUE
#endif

// use appropriate picocode depending on charger type as defined
// by the kBatteryChargerCircuitType in *NonVolDefaults.h
#if (kBatteryChargerCircuitType == LINEAR_CHARGER)
	// All other chips use identical code to the 3382
	#include "batteryMicrocodeLinear_3382.h"
#else
	// All other chips use identical code to the 3382
	#include "batteryMicrocode_3382.h"
#endif

// the following are correct defaults for Broadcom reference designs.  Others must define these
// in the appropriate platform-specific nonvoldefault.h file:
#ifndef VBAT_ATTEN_UPPER_KOHMS
	#define VBAT_ATTEN_UPPER_KOHMS 33.0					
#endif
#ifndef VBAT_ATTEN_LOWER_KOHMS
	#if (kBatteryNumberOfCells == 3)
		#define VBAT_ATTEN_LOWER_KOHMS 3.24
	#else
		#define VBAT_ATTEN_LOWER_KOHMS 5.11
	#endif
#endif

// maximum  charge current allowed based on board and/or adapter constraints
// this is correct for all BRCM 3378/79/80 refernce designs.  Others may have
// to redefine this in an appropriate platform-specific nonvoldefaults.h file:
#ifndef MAX_CHARGE_CURRENT_MA
	#define MAX_CHARGE_CURRENT_MA 1100
#endif

// whether a pack that requires more than MAX_CHARGE_CURRENT_MA should be allowed
// to charge at a lower C-rate or should be invalidated.  All BRCM reference
// designs invalidate the pack if it requires more than the allowed charge 
// current.  Vendors may redefine this in an appropriate platform-specific 
// nonvoldefaults.h file:  
// 		0 = ALLOW CHARGING AT LOWER THAN CALCULATED C-RATE
// 		1 = INVALIDATE
#ifndef INVALIDATE_PACK_WHEN_EXCESS_CHARGE_CURRENT_IS_REQD
	#define INVALIDATE_PACK_WHEN_EXCESS_CHARGE_CURRENT_IS_REQD 1
#endif

#define VREF 1.2
// full-scale voltage calculation:
#define VBAT_FS ((float)((VREF * (VBAT_ATTEN_UPPER_KOHMS+VBAT_ATTEN_LOWER_KOHMS))/VBAT_ATTEN_LOWER_KOHMS))
// how we convert from VBAT ADCs to volts and visa-versa
#define ADC_TO_MILLIVOLTS(adc) ((uint32)((((float)adc*VBAT_FS)/1024.0)*1000))
#define MILLIVOLTS_TO_ADC(millivolts) ((uint32)(ceil(1024.0*((float)millivolts/1000.0)/VBAT_FS)))

// how we convert from VIN ADCs to and from millivolts
#ifdef VENDOR_FULL_SCALE_VIN
	#define FS_VIN VENDOR_FULL_SCALE_VIN
#else
	#if (kBatteryNumberOfCells == 3)
		#define FS_VIN 21.030
	#else
		#define FS_VIN 16.824
	#endif
#endif
#define ADC_VIN_V_FACTOR	   ((float)1024/(float)(FS_VIN * 1000))	// mV

// and how we convert from IBAT ADC values to and from milliamps
#define ADC_IBAT_A_FACTOR      ((float)1024/(float)1920)	// mA
															// 
															// 
#define VINIT_DELTA			   ((float)210)					// mV

//#define SCALE_VOLTAGE_FROM_PICO(p) ((p * kBatteryNumberOfCells) / 3)
//#define SCALE_VOLTAGE_TO_PICO(v) ((v * 3) / kBatteryNumberOfCells)
#define SENSE_RATIO ((float)kBatterySenseNumerator/(float)kBatterySenseDenominator)
#define SERIES_SENSE_RESISTOR (0.1 / SENSE_RATIO)

// the following defines the absolute maximum charge voltage allowed per cell
// it is given to the Pico at initialization time as an ADC value based on
// the total number of series cells supported
#ifndef BAT_MAX_MV_PER_CELL
	#define BAT_MAX_MV_PER_CELL	4164
#endif
// this is defined by cell manufacturer and is used to clip the above value
// at initialization time
#define ABS_MAX_MV_PER_CELL	4200

#if ((BAT_MAX_MV_PER_CELL * kBatteryNumberOfCells) <= (ABS_MAX_MV_PER_CELL * kBatteryNumberOfCells))
	#define MAX_CHG_VOLTS (BAT_MAX_MV_PER_CELL * kBatteryNumberOfCells/1000.0)
#else
	#define MAX_CHG_VOLTS (ABS_MAX_MV_PER_CELL * kBatteryNumberOfCells/1000.0)
#endif

//*************
// VENDOR EXTENSION PROVISIONS
//
// There are two functions that may be overridden by vendor-supplied functions
// VendorBatteryInit() can be used to replace default initialization values,
// VendorBatteryUpdate() can be used to replace most of the update function.
// To use them, set the #defines properly:
// 		#define VENDOR_BATTERY_INIT_FUNCTION    // if VendorBatteryInit is supplied
// 		#define VENDOR_BATTERY_UPDATE_FUNCTION  // if VendorBatteryUpdate is supplied
// Finally, put the funcion definition(s) (the body) in a private ".c" file and
// make sure it is added to the vendor make file.
//*************
// these are in a Vendor's replacement file
#ifdef VENDOR_BATTERY_INIT_FUNCTION
	extern void VendorBatteryInit(TBatteryControllerInfo *info, TBatteryDriverCalls *calls);
#endif
#ifdef VENDOR_BATTERY_UPDATE_FUNCTION
	extern BOOL VendorBatteryUpdate(TBatteryControllerInfo *info, TBMUUpdateInfo *updateInfo);
#endif

// uncomment this to get additional debug messages when ZONE_TEST1 is enabled
// #define PRINT_BATTERY_UPDATE_INFO
#ifdef PRINT_BATTERY_UPDATE_INFO
	#define HDP(z, s...) HalDebugPrint(z, s)
#else
	#define HDP(z, s...)
#endif

static int CmdErrno = 0;
static int presenceCounter[2] = {kMIN_DETECTION_POLLS-1,kMIN_DETECTION_POLLS-1};
static int currentChargePercent = 80;
static BOOL chargingSuspended = FALSE;
static BOOL allowDebugPrint = TRUE;
static BOOL bmuDisabled = FALSE;
//*********************  Private methods **************************************
static BOOL PartIsTrimmed()
{
	BOOL result = TRUE;
	if( USE_ONLY_TRIMMED_PARTS == TRUE)
	{
		volatile ApmBaseApmBaseApmBase *apm = (volatile ApmBaseApmBaseApmBase *)APM_BASE;
		uint32 apm_otp_lo = apm->RegOtp0;
		HDP (ZONE_TEST1, "APM OTP LO = 0x%08lx\n", apm_otp_lo);
		apm_otp_lo = (apm_otp_lo >> 6) & 0xfff;
		result = (apm_otp_lo != 0 ? TRUE : FALSE);
	}
	return result;
}

static void getupdateinfo(TBMUUpdateInfo *updateInfo)
{
	int size = sizeof(TBMUUpdateInfo)/sizeof(uint32);
	volatile uint32 *pSrc = (volatile uint32 *)&BMU->DMEM.updateInfo;
	uint32 *pDst = (uint32 *)updateInfo;
	while(size--) {
		*pDst++ = *pSrc++;
	}
	BMU->DMEM.updateInfo.BMUStatus = 0;
}

#if (!BCM_REDUCED_IMAGE_SIZE) && !defined(VENDOR_BATTERY_UPDATE_FUNCTION) && defined( PRINT_BATTERY_UPDATE_INFO)
static const char *RawState2String(uint32 state)
{
	switch (state)
	{
	case STATE_INIT: 		return "INIT"; break;
	case STATE_CHARGE_INIT: return "CHG_INIT"; break;
	case STATE_IDLE: 		return "IDLE"; break;
	case STATE_SLEEP: 		return "SLEEP"; break;
	case STATE_CHG_SUSPEND: return "CHG_SUSP"; break;
	case STATE_PREQUAL: 	return "PREQUAL"; break;
	case STATE_FAST_CHARGE: return "FAST_CHG"; break;
	case STATE_TOPOFF: 		return "TOPOFF"; break;
	case STATE_DISCHARGE: 	return "DISCHARGE"; break;
	case STATE_DYING: 		return "ESLEEP"; break;
	}
	return "UNKNOWN";
}
#endif

static BatteryControllerState State2State(uint32 picoState)
{
	BatteryControllerState result = kBCInit;
	switch (picoState)
	{
	case STATE_INIT: 		result = kBCInit; break;
	case STATE_CHARGE_INIT: result = kBCChargeInit; break;
	case STATE_IDLE: 		result = kBCIdle; break;
	case STATE_SLEEP: 		result = kBCSleep; break;
	case STATE_CHG_SUSPEND: result = kBCChargeSuspended; break;
	case STATE_PREQUAL: 	result = kBCPrecharge; break;
	case STATE_FAST_CHARGE: result = kBCFastCharge; break;
	case STATE_TOPOFF: 		result = kBCTopoff; break;
	case STATE_DISCHARGE: 	result = kBCDischarge; break;
	case STATE_DYING: 		result = kBCEternalSleep; break;
	}
	return result;
}

#define PACK_THERM_Ro 10000		// deprecated since each pack contains its own thermal constants and limits (SWCM-13890)
#define PACK_THERM_To 298.15	// "
#define PACK_THERM_B 3380.0		// "
#define PACK_THERM_LADDER_RESISTANCE 10000
float bspPackTemperature(TBatteryThermalInfo *thermInfo, uint16 thermADC, long ibatADC )
{
	//if( kBatteryNumberOfCells != 2 )
	//	return TEMP_MEASUREMENT_NOT_SUPPORTED;

	// the algorithm used compensates for battery current across the current sense resistor
	float result = 25.0;
	float R, cTherm;

	if( thermADC > 1023 ) 
		thermADC = 1023; //brrrrrr
	if( thermADC == 0 ) 
		thermADC = 1; 	//sweatin' to the oldies and preventing a divide-by-zero

	cTherm = (float)thermADC - ((float)(ibatADC)/(8*SENSE_RATIO));
	if(cTherm < 0) cTherm = 0;
	HDP (ZONE_TEST1, "Calculating Pack Temp with ADC = 0x%08x, ibat ADC = %ld\n", thermADC, ibatADC);
	R = (PACK_THERM_LADDER_RESISTANCE * cTherm) / (1024 - cTherm);
	result = 1 / ((log(R / (float)thermInfo->ThermR0) / (float)thermInfo->ThermB) + (1 / ((float)thermInfo->ThermT0 + 275.15))) - 273.15; 
	HDP (ZONE_TEST1, "Calculated Pack R = %.3f ohms, temperature = %.3f degC\n", R, result);
	return result;
}

BOOL SendSimpleCommand( int command, int qual, uint32 *cmdData )
{
	uint32 cmd = (qual << CMD_QUAL_SHIFT) | (command << CMD_SHIFT);
	// wait for any in-process command to complete
	while( BMU->DMEM.controlInfo.MipsCmd & CMD_VALID) Sleep(1);
	// send the target command
	if (cmdData) {
		BMU->DMEM.controlInfo.CmdDataMSW = ((*cmdData)>>16) & 0xffff;
		BMU->DMEM.controlInfo.CmdDataLSW = (*cmdData) & 0xffff;
	}
	BMU->DMEM.controlInfo.MipsCmd = (cmd | CMD_VALID);
	// wait for this command to complete
	while( BMU->DMEM.controlInfo.MipsCmd & CMD_VALID) Sleep(1);
	// return the value in cmdData MSW/LSW and an indication of success
	if(cmdData) {
		*cmdData = (BMU->DMEM.controlInfo.CmdDataMSW << 16) | BMU->DMEM.controlInfo.CmdDataLSW;
	}
	CmdErrno = (BMU->DMEM.controlInfo.MipsCmd >> CMD_ERR_SHIFT) & CMD_ERR_MASK;
	return CmdErrno == EC_NO_ERROR ? TRUE : FALSE;
}

//*********************  Private Functions called via Pointers *****************
static BOOL pvtEnableChargingAll ( BOOL Enable )
{
	if (Enable)
	{
		HDP(ZONE_TEST1,"Clearing Pico global charge disable\n");
		BMU->DMEM.controlInfo.BMUControl &= ~CHARGE_DISABLED;
	}
	else
	{
			HDP(ZONE_TEST1,"Setting Pico global charge disable\n");
			BMU->DMEM.controlInfo.BMUControl |= CHARGE_DISABLED;
	}
	return TRUE;
}

static void	SetChargeParams(TBatteryControllerInfo *info, int battery, float targetVolts)
{
	TBatteryInfo *bi = &info->Batteries[battery];
	uint32 irdrop, pqvterm, vinit, vterm = (uint32)(targetVolts * 1000);
	float scaleFactor = (float)bi->EpromCapacity/2200.0;
	float minChgReqd = 500.0 * scaleFactor;

	if(minChgReqd > MAX_CHARGE_CURRENT_MA) minChgReqd = MAX_CHARGE_CURRENT_MA;

	if(allowDebugPrint)
	{
		HalDebugPrint (ZONE_TEST1, "TargetV = %.3f, VBAT_FS = %f\n", targetVolts, VBAT_FS);
		HalDebugPrint (ZONE_TEST1, "...cap = %d, sf = %0.2f, mcr = %0.2f, sr = %0.3f\n", 
					   bi->EpromCapacity, scaleFactor, minChgReqd, SERIES_SENSE_RESISTOR);
	}

	// add 25mV for charge termination IR drop and 21mV for charge relaxation
	//vterm += (25 + 21);

	// add IR drop at end of charge and charge relaxation voltage
	irdrop = ceil(minChgReqd * SERIES_SENSE_RESISTOR);
	vterm += (irdrop + 50);
	if(allowDebugPrint)
	{
		HalDebugPrint (ZONE_TEST1, "...adding %ld mv to vterm for charge corrections (ir=%ld)\n", irdrop+50, irdrop);
	}

	if( vterm > (uint32)(MAX_CHG_VOLTS * 1000))
	{
		if(allowDebugPrint)
		{
			HalDebugPrint (ZONE_TEST1, "...Clipping calculated VCHG (%ld mV) to %ld mV\n", vterm, (uint32)(MAX_CHG_VOLTS * 1000));
		}
		vterm = (uint32)(MAX_CHG_VOLTS * 1000);
	}
	vinit = (vterm >= VINIT_DELTA) ? vterm - VINIT_DELTA : 0;
	pqvterm = 3000 * kBatteryNumberOfCells;		// 3V/cell
	info->Batteries[battery].FCVTERM = (uint16)vterm;
	info->Batteries[battery].VINIT = (uint16)vinit;
	if(allowDebugPrint)
	{
		HalDebugPrint (ZONE_TEST1, "...Desired mV - PQVTERM = %ld, VINIT = %ld, FCVTERM = %ld\n", pqvterm, vinit, vterm);
	}
	vinit = MILLIVOLTS_TO_ADC(vinit);
	vterm = MILLIVOLTS_TO_ADC(vterm) + 1;
	pqvterm = MILLIVOLTS_TO_ADC(pqvterm);
	if(allowDebugPrint)
	{
		HalDebugPrint (ZONE_TEST1, "...ADC Values - PQVTERM = %ld, VINIT = %ld, FCVTERM = %ld\n", pqvterm, vinit, vterm);
		HalDebugPrint (ZONE_TEST1, "...Actual mV -  PQVTERM = %ld, VINIT = %ld, FCVTERM = %ld\n", ADC_TO_MILLIVOLTS(pqvterm), ADC_TO_MILLIVOLTS(vinit), ADC_TO_MILLIVOLTS(vterm));
	}
	BMU->DMEM.initInfo.batChargeParams[battery].BAT_PQVTERM = pqvterm;
	BMU->DMEM.initInfo.batChargeParams[battery].BAT_VINIT = vinit;
	BMU->DMEM.initInfo.batChargeParams[battery].BAT_FCVTERM = vterm;
	BMU->DMEM.initInfo.batChargeParams[battery].BAT_PQVCHG = vterm;
	BMU->DMEM.initInfo.batChargeParams[battery].BAT_TCVTERM = vterm;
}

static BOOL pvtStopCharge (void)
{
	return SendSimpleCommand(CmdStopCharge,0,0);
}

static BOOL pvtSetChargePercentageSingle ( TBatteryControllerInfo *info, int battery, int Percent )
{
	float  Vpct = PctToV(info->Batteries[battery].dischargeTable, (float)Percent);
	uint32 picoState;
	if(allowDebugPrint)
	{
		HalDebugPrint (ZONE_TEST1, "Setting battery %d charge percentage to %d (%.3fV)\n", battery, Percent, Vpct);
	}
	SetChargeParams(info, battery, Vpct);

	// see if we need to stop an in-process charge on this pack to cause the new values to get loaded
	picoState = BMU->DMEM.updateInfo.PicoState;
	if (picoState >= STATE_CHARGE_INIT 
	 && picoState <= STATE_CHG_SUSPEND 
	 && (BMU->DMEM.updateInfo.batteryInfo[battery].BatStatus & BXS_SELECTED)) {
		if(allowDebugPrint)
		{
			HalDebugPrint (ZONE_TEST1, "...Stopping charge so new values are picked up\n");
		}
		pvtStopCharge();
	}
	return TRUE;
}

static BOOL pvtSetChargePercentageAll ( TBatteryControllerInfo *info, int Percent )
{
	int ix;
	BOOL result = TRUE;
	for( ix = 0; result == TRUE && ix < MAX_BATTERIES; ix++ )
		result = pvtSetChargePercentageSingle(info, ix, Percent);
	currentChargePercent = Percent;
	return result;
}

static BOOL pvtStartForcedCharge ( TBatteryControllerInfo *info, int battery, float toVolts )
{
	HDP(ZONE_TEST1,"Commanding forced charge to %fV\n", toVolts);
	SetChargeParams(info, battery, toVolts);
	return SendSimpleCommand(CmdForcedCharge,battery,0);
}

static BOOL pvtEnableDischargeAll ( BOOL Enable )
{
	if (Enable)
	{
		HDP(ZONE_TEST1,"Clearing Pico global discharge disable\n");
		BMU->DMEM.controlInfo.BMUControl &= ~DISCHARGE_DISABLED;
	}
	else
	{
		HDP(ZONE_TEST1,"Setting Pico global charge disable\n");
		BMU->DMEM.controlInfo.BMUControl |= DISCHARGE_DISABLED;
	}
	return TRUE;
}

static BOOL pvtStartForcedDischarge ( const TBatteryControllerInfo *info, int battery, float toVolts )
{
	uint32 adc = MILLIVOLTS_TO_ADC((uint32)(toVolts * 1000));
	HDP(ZONE_TEST1,"Commanding forced discharge to %fV (%ld ADC)\n", toVolts, adc);
	return SendSimpleCommand(CmdForcedDischarge,battery,&adc);
}

static BOOL pvtStopForcedDischarge ( void )
{
	return SendSimpleCommand(CmdStopForcedDchg,0,0);
}

static BOOL pvtReadGUID (TBatteryControllerInfo *info, int which )
{
	HalDebugPrint(ZONE_TEST1,"Reading GUID as requested...\n");
	if( SendSimpleCommand(CmdReadGUID,which,0) == TRUE )
	{
		info->Batteries[which].guid = (((uint64)BMU->SMEM.Param32_H) << 32) | (uint64)BMU->SMEM.Param32_L;
		return TRUE;
	}
	HalDebugPrint(ZONE_TEST1, "pvtReadGUID returned an error of %d\n", CmdErrno);
	return FALSE;
}

static BOOL pvtMakeHWResistanceMeasurement ( int which, float *result )
{
#if 0
	uint32 cmdData;
	if( SendSimpleCommand(CmdCalcResistance,which,&cmdData) == TRUE )
	{
		float ICOS = (float)BMU->LS.ImpedanceICos;
		float ISIN = (float)BMU->LS.ImpedanceISin;
		float VCOS = (float)BMU->LS.ImpedanceVCos;
		float VSIN = (float)BMU->LS.ImpedanceVSin;
		int saturation = cmdData & 1;
		if (!saturation ) {
			// The battery impedance is  Zbat = (BMU_LS_BAT_IMP_V_COS + jBMU_LS_BAT_IMP_V_SIN) / (BMU_LS_BAT_IMP_I_COS +jBMU_LS_BAT_IMP_I_SIN)  x vi_scale
			// where vi_scale = (max_FS_VBAT) x .1 x 6.25 / 1.2
			#define vi_scale (VBAT_FS * SERIES_SENSE_RESISTOR * 6.25 / 1.2)
			*result = sqrt(VCOS*VCOS + VSIN*VSIN)/sqrt(ICOS*ICOS + ISIN*ISIN) * vi_scale;
			return TRUE;
		}
		else
		{
			HalDebugPrint(ZONE_TEST1, "pvtMakeHWResistanceMeasurement returned no error, but saturation flag was set\n");
			return FALSE;
		}
	}
	HalDebugPrint(ZONE_TEST1, "pvtMakeHWResistanceMeasurement returned an error of %d\n", CmdErrno);
#else
	HalDebugPrint(ZONE_TEST1, "pvtMakeHWResistanceMeasurement returning FALSE\n");
#endif
	return FALSE;
}

// make a pack valid/invalid based on the validity value
static BOOL pvtValidatePack (int which, BOOL validity)
{
	if (validity == TRUE)
		return SendSimpleCommand(CmdBatteryIsValid,which,0);
	else
		return SendSimpleCommand(CmdBatteryIsInvalid,which,0);
}

static BOOL pvtSetHWFaultMask (uint32 mask)
{
	BMU->DMEM.updateInfo.HWFaultsAllowed = mask;
	return TRUE;
}

static uint16 pvtGetFaultMask( void)
{
	return (uint16)BMU->DMEM.updateInfo.HWFaultsAllowed;
}

static uint16 pvtGetHWFaults( void )
{
	return (uint16)BMU->DMEM.updateInfo.HWFaults;
}

static BOOL pvtClearHWFaults (uint32 mask)
{
	BMU->DMEM.updateInfo.HWFaults &= ~mask;
	return TRUE;
}

static void pvtClearChargeFaults( int which )
{
	SendSimpleCommand(CmdClearChargeErrors,which,0);
}

static void pvtForceWatchdogTimeout( void )
{
	BMU->DMEM.controlInfo.BMUControl |= INHIBIT_WATCHDOG;
}

static void pvtMainsVoltages( TMVLimits *limits )
{
	limits->infoValid   = 1;
	limits->MV_CHG_ON   = 692;
	limits->MV_CHG_OFF  = 672;
	limits->MV_DCHG_ON  = 488;
	limits->MV_DCHG_OFF = 585;
	limits->MVSYS_MIN   = 244;
	limits->MVSYS_MAX   = 853;
	limits->MVMAX_IDLE  = 805;
}

static BOOL pvtAdjustCellConfig(TBatteryControllerInfo *info, int whichBat, TPackParams *packParams)
{
	volatile TBatChargeParams *ptr = &BMU->DMEM.initInfo.batChargeParams[whichBat];

	if(packParams->doiLimitC > 8000) packParams->doiLimitC = 8000;	// hardware limit of 8A

	ptr->BAT_PQICHG 	   = (uint32)(((float)packParams->pqiChg)*ADC_IBAT_A_FACTOR) + 1;
	ptr->BAT_FCICHG 	   = (uint32)(((float)packParams->fciChg)*ADC_IBAT_A_FACTOR) + 1;
	ptr->BAT_FCITERM 	   = (uint32)(((float)packParams->fciTerm)*ADC_IBAT_A_FACTOR) + 1;
	ptr->BAT_TCICHG 	   = ptr->BAT_FCICHG;     // by default, TC uses same charge current as FC
	ptr->BAT_TCITERM 	   = (uint32)(((float)packParams->tciTerm)*ADC_IBAT_A_FACTOR);
	ptr->BAT_COI_LIMIT     = (uint32)(((float)packParams->coiLimit)*ADC_IBAT_A_FACTOR) + 1;
	ptr->BAT_DOI_LIMIT_A   = (uint32)(((float)packParams->doiLimitA)*ADC_IBAT_A_FACTOR) + 1;
	ptr->BAT_DOI_LIMIT_B   = (uint32)(((float)packParams->doiLimitB)*ADC_IBAT_A_FACTOR) + 1;
	ptr->BAT_DOI_LIMIT_C   = (uint32)(((float)packParams->doiLimitC)*ADC_IBAT_A_FACTOR) + 1;
    ptr->BAT_DOI_TIMEOUT_A = (uint32)packParams->doiTimeA;
    ptr->BAT_DOI_TIMEOUT_B = (uint32)packParams->doiTimeB;
    ptr->BAT_PQVTERM       = (uint32)MILLIVOLTS_TO_ADC(packParams->pqvTerm);
    ptr->BAT_DCVB_ON       = (uint32)MILLIVOLTS_TO_ADC(packParams->vdepleted);
    ptr->BAT_DCVB_OFF      = (uint32)MILLIVOLTS_TO_ADC(packParams->dcvb_off); 
    ptr->BAT_MAX_BAT_CHG   = (uint32)MILLIVOLTS_TO_ADC(packParams->mavV);
    ptr->BAT_LOW_WARN      = (uint32)MILLIVOLTS_TO_ADC(packParams->low_warn);
	HalDebugPrint(ZONE_TEST1,"AdjustCellConfig on battery %d:\n", whichBat);
	HalDebugPrint(ZONE_TEST1,"   PQICHG  = 0x%04lx (%d mA)\n", ptr->BAT_PQICHG, (int)(ptr->BAT_PQICHG/ADC_IBAT_A_FACTOR));
    HalDebugPrint(ZONE_TEST1,"   PQVTERM = 0x%04lx (%d mV)\n", ptr->BAT_PQVTERM, (int)ADC_TO_MILLIVOLTS(ptr->BAT_PQVTERM));
	HalDebugPrint(ZONE_TEST1,"   FCICHG  = 0x%04lx (%d mA)\n", ptr->BAT_FCICHG, (int)(ptr->BAT_FCICHG/ADC_IBAT_A_FACTOR));
	HalDebugPrint(ZONE_TEST1,"   FCITERM = 0x%04lx (%d mA)\n", ptr->BAT_FCITERM, (int)(ptr->BAT_FCITERM/ADC_IBAT_A_FACTOR));
	HalDebugPrint(ZONE_TEST1,"   TCICHG  = 0x%04lx (%d mA)\n", ptr->BAT_TCICHG, (int)(ptr->BAT_TCICHG/ADC_IBAT_A_FACTOR));
	HalDebugPrint(ZONE_TEST1,"   TCITERM = 0x%04lx (%d mA)\n", ptr->BAT_TCITERM, (int)(ptr->BAT_TCITERM/ADC_IBAT_A_FACTOR));
	HalDebugPrint(ZONE_TEST1,"   COI LIM = 0x%04lx (%d mA)\n", ptr->BAT_COI_LIMIT, (int)(ptr->BAT_COI_LIMIT/ADC_IBAT_A_FACTOR));
	HalDebugPrint(ZONE_TEST1,"   DOI A   = 0x%04lx (%d mA)/%dms\n", ptr->BAT_DOI_LIMIT_A, (int)(ptr->BAT_DOI_LIMIT_A/ADC_IBAT_A_FACTOR), (int)ptr->BAT_DOI_TIMEOUT_A);
	HalDebugPrint(ZONE_TEST1,"   DOI B   = 0x%04lx (%d mA)/%dms\n", ptr->BAT_DOI_LIMIT_B, (int)(ptr->BAT_DOI_LIMIT_B/ADC_IBAT_A_FACTOR), (int)ptr->BAT_DOI_TIMEOUT_B);
	HalDebugPrint(ZONE_TEST1,"   DOI C   = 0x%04lx (%d mA)\n", ptr->BAT_DOI_LIMIT_C, (int)(ptr->BAT_DOI_LIMIT_C/ADC_IBAT_A_FACTOR));
	HalDebugPrint(ZONE_TEST1,"   DCVB_ON = 0x%04lx (%d mV)\n", ptr->BAT_DCVB_ON, (int)ADC_TO_MILLIVOLTS(ptr->BAT_DCVB_ON));
	HalDebugPrint(ZONE_TEST1,"   DCVB_OFF= 0x%04lx (%d mV)\n", ptr->BAT_DCVB_OFF, (int)ADC_TO_MILLIVOLTS(ptr->BAT_DCVB_OFF));
	HalDebugPrint(ZONE_TEST1,"   MAX_CHG = 0x%04lx (%d mV)\n", ptr->BAT_MAX_BAT_CHG, (int)ADC_TO_MILLIVOLTS(ptr->BAT_MAX_BAT_CHG));
	HalDebugPrint(ZONE_TEST1,"   LOW_WARN= 0x%04lx (%d mV)\n", ptr->BAT_LOW_WARN, (int)ADC_TO_MILLIVOLTS(ptr->BAT_LOW_WARN));
	return TRUE;
}

static void pvtAdjustPORValidityTimeout(uint32 pollSecs)
{
	if (pollSecs < 2)
	{
		pollSecs = 2;
	}
	BMU->DMEM.initInfo.POR_VALID_SECS = (pollSecs * 4) + 2;
}


static void pvtLoadPicocode( BOOL printDebugInfo )
{
	int loop;
	if (printDebugInfo) HalDebugPrint (ZONE_TEST1, "Copying picocode to BMU IMEM @ %p.\n", BMU_IMEM);
	for (loop = 0; loop < (sizeof(batteryPicocode) / sizeof(uint32)); loop++)
	{
		if (printDebugInfo) HalDebugPrint (ZONE_TEST1, ".");
		BMU_IMEM[loop] = batteryPicocode[loop];
	}
	if (printDebugInfo) HalDebugPrint (ZONE_TEST1, "\n");
}

static BOOL pvtVerifyPicocode( BOOL printDebugInfo )
{
	int loop;

	if (printDebugInfo) HalDebugPrint (ZONE_TEST1, "Verifying picocode.\n");
	for (loop = 0; loop < (sizeof(batteryPicocode) / sizeof(uint32)); loop++)
	{
		if (BMU_IMEM[loop] != batteryPicocode[loop])
		{
			if (printDebugInfo) HalDebugPrint (ZONE_TEST1, "Picocode verification failed!\n");
			return FALSE;
		}
		if (printDebugInfo) HalDebugPrint (ZONE_TEST1, ".");
	}
	if (printDebugInfo) HalDebugPrint (ZONE_TEST1, "\n");
	return TRUE;
}

static BOOL pvtStartPico(TBatteryControllerInfo *info, TBatteryDriverCalls *calls)
{
	int maxSleep = 1000;
    BOOL result = TRUE;
	presenceCounter[0] = presenceCounter[1] = (kMIN_DETECTION_POLLS-1);
    BMU->LS.SoftReset = PICO_HALT;
	BMU->DMEM.controlInfo.BMUControl = 0;		// set BMUControl to zero - this is where Pico will handshake
	BMU->LS.SoftReset = PICO_RUN_FROM_RAM;		// let Pico start up <------------ comment this line out to test "Pico Failed To Start" error
	while( !(BMU->DMEM.controlInfo.BMUControl & STARTUP_HANDSHAKE)) 
	{
		Sleep(1);
		if(maxSleep-- <= 0 ) {
			break;
		}
	}
	if (maxSleep > 0) {
		HalDebugPrint(ZONE_TEST1,"StartPico:  Pico started.. overriding defaults...\n");
		BMU->DMEM.initInfo.SenseNumerator = kBatterySenseNumerator;
		BMU->DMEM.initInfo.SenseDenominator = kBatterySenseDenominator;
		BMU->DMEM.initInfo.MaxIDCurrent = (610 * kBatterySenseNumerator)/kBatterySenseDenominator;
		BMU->DMEM.initInfo.batChargeParams[0].BAT_MAX_BAT_CHG = MILLIVOLTS_TO_ADC(MAX_CHG_VOLTS * 1000)+1;		// allow a 1 ADC count overage
        BMU->DMEM.initInfo.batChargeParams[1].BAT_MAX_BAT_CHG = MILLIVOLTS_TO_ADC(MAX_CHG_VOLTS * 1000)+1;		// allow a 1 ADC count overage
		BMU->DMEM.initInfo.BatSelectDelay = 7;	// PR23030
        allowDebugPrint = FALSE;
        pvtSetChargePercentageAll ( info, info->ChargePercent );
		pvtSetHWFaultMask(0);					/// TODO:  for now!!!
		pvtAdjustPORValidityTimeout(info->PollSeconds);
        allowDebugPrint = TRUE;

		#ifdef VENDOR_BATTERY_INIT_FUNCTION
			// Call vendor init function if one is provided 
			VendorBatteryInit(info,calls);
		#endif
		// Release the Pico
		//Sleep(1000);		// <------------uncomment this line to test LTE
		BMU->DMEM.controlInfo.BMUControl &= ~(STARTUP_HANDSHAKE);	// release the hounds!
		info->Batteries[0].epromInfo.ValidFlags = 0;
		info->Batteries[0].updateInfo.Status = 0;
		info->Batteries[1].epromInfo.ValidFlags = 0;
		info->Batteries[1].updateInfo.Status = 0;
		HalDebugPrint(ZONE_TEST1,"...completed overriding defaults - checking for Pico load-time error\n");
		if(BMU->DMEM.updateInfo.BMUStatus & LTE)
		{
			HalDebugPrint(ZONE_TEST1,"ERROR...Pico reports load-time error...\n");
			result = FALSE;
		}
		else
		{
				HalDebugPrint(ZONE_TEST1,"Pico startup completed successfully\n");
		}
	}
	else 
	{
		HalDebugPrint(ZONE_TEST1,"ERROR...Pico failed to start...\n");
		result = FALSE;
	}
	if(result == FALSE)
	{
		HalDebugPrint(ZONE_TEST1,"...deselecting all batteries\n");
		BMU->LS.SoftReset = PICO_HALT;
		BMU->CP.BatteryEnable = 0x10;		// this isn't actually necessary when an LTE error has been detected, but it won't hurt anything either.
	}
	return result;
}

static uint32 pvtMeasure(int which)
{
	uint32 status = (&BMU->LS.VbatStatus)[which];
	while( !(status & 0x10)) {
		status = (&BMU->LS.VbatStatus)[which];
	}
	return ((&BMU->LS.VbatAvg)[which]) >> 4;
}

static void pvtMimicWarmRestart(void)
{
	uint32 batCount = 0;
	uint32 val = 0;
	// performs the same tasks as the bootloader
	BMU->LS.SoftReset = PICO_HALT;
	if((BMU->CP.ActiveBat & 0x00000003) != 0) {
		HalDebugPrint(ZONE_TEST1,"\nb33\n");
		return;		// battery already selected...nothing to do
	}
	// intialize hardware
	BMU->CP.ChargerMode = (1 << 2);	// SLEEP/PWM Disabled
	BMU->CP.RrSgl[0] = 0;	// single input
	BMU->CP.RrSgl[1] = 1;	// round-robin
	BMU->CP.SlewWindowSizes = 0;	// don't slew window sizes
	BMU->LS.VbatAvgCtrl = 2;		// 4 samples/average
	BMU->LS.RidAAvgCtrl = 2;
	BMU->LS.RidBAvgCtrl = 2;
	val = BMU->LS.RidAStatus;		// read once to clear
	val = BMU->LS.RidBStatus;

	if( pvtMeasure(4) < 0x3e0 ) batCount |= 1;
	if( pvtMeasure(5) < 0x3e0 ) batCount |= 2;
	if( batCount == 0) {
		HalDebugPrint(ZONE_TEST1,"\nb00\n");
		return;		// no batteries present
	}
	if(batCount != 3) {
		BMU->CP.BatteryEnable = batCount | 0x10;
		HalDebugPrint(ZONE_TEST1,"\nb1%1ld\n", batCount);
		return;		// a single battery present...select it
	}
	// both batteries present...measure each in turn
	BMU->CP.BatteryEnable = 0x11;
	Sleep(3);
	val = pvtMeasure(0);
	BMU->CP.BatteryEnable = 0x12;
	Sleep(3);
	if( pvtMeasure(0) < val ) {
		BMU->CP.BatteryEnable = 0x11;
		Sleep(3);
		HalDebugPrint(ZONE_TEST1,"\nb21\n");
	}
	else
		HalDebugPrint(ZONE_TEST1,"\nb22\n");
}

static BOOL pvtCheckWatchdog(TBatteryControllerInfo *info)
{
	uint32 wdVal = BMU->DMEM.updateInfo.PicoWatchdog & 0xffff;
	BMU->DMEM.updateInfo.PicoWatchdog = 0;
	if (wdVal != WATCHDOG_MAGIC_VALUE ) {
		HalDebugPrint(ZONE_TEST1,"*********Pico Watchdog timeout detected!!!!\n");
		info->WatchdogResets++;
		pvtMimicWarmRestart();		// places Pico in reset and minics action of bootrom
		if(pvtVerifyPicocode(FALSE) == FALSE) {
			info->WatchdogReloads++;
			BMU->LS.SoftReset = PICO_HALT;
			pvtLoadPicocode(FALSE);
			if(pvtVerifyPicocode(FALSE) == FALSE) {
				HalDebugPrint(ZONE_TEST1,"*********Pico Watchdog Reload unsuccessful!!!\n");
				return FALSE;
			}
		}
		return pvtStartPico(info, (TBatteryDriverCalls *)0);
	}
	return TRUE;
}

BOOL pvtCompletePackValidation(TBatteryControllerInfo *info, int whichBat, TPackValidation *vs)
{
    BOOL valid = TRUE;		// assume success
    TBatteryInfo *bi = &info->Batteries[whichBat];
    if (vs->generation > 1)
    {
        TPackParams tpp;
        float minChgReqd;
        // the discharge table may have been changed prior to getting called.  
        // The following will set the charge parameters based on this [potentially] new table...
        // ...they may get modified below, and pvtAdjustCellConfig will take care of that
        pvtSetChargePercentageSingle(info,whichBat,currentChargePercent);
        if (vs->generation == 2) {
            float scaleFactor;
            HalDebugPrint(ZONE_TEST1,"Processing 2nd generation pack\n");
            scaleFactor     = (float)bi->EpromCapacity/2200.0;
            minChgReqd      = 500.0 * scaleFactor;
            tpp.doiLimitA	= 1200 * scaleFactor;		    // ma
            tpp.doiLimitB	= 2000 * scaleFactor;		    // ma
            tpp.doiLimitC	= 4200 * scaleFactor;		    // ma
            tpp.doiTimeA	= 2000;				 		    // ms
            tpp.doiTimeB	= 200;				 		    // ms
            tpp.pqiChg		= (uint16)(minChgReqd * 0.4);   // ma
            tpp.fciChg		= (uint16)minChgReqd;		    // ma
            tpp.tciTerm		= (int)(minChgReqd * 0.1);	    // ma
            tpp.pqvTerm		= 3000 * kBatteryNumberOfCells; // mv
            tpp.vdepleted	= 3000 * kBatteryNumberOfCells;	// mv (dcvbon)
            tpp.mavV		= 4193 * kBatteryNumberOfCells;	// mv
            tpp.coff_hi		= 45;						    // degC
            tpp.coff_lo		= 0;						    // degC
            tpp.doff_hi		= 60;						    // degC
            tpp.doff_lo		= -10;						    // degC
            tpp.thermB		= 3380;						    // degK
            tpp.thermR0		= 10000;					    // Ohms
            tpp.thermT0		= 25;						    // degC
        }
        else {	// 3rd generation pack - we read all parameters from the pack then make decisions
            HalDebugPrint(ZONE_TEST1,"Processing 3rd generation pack\n");
            memcpy(&tpp, &bi->epromInfo.pageCache[2][0], 32);
            minChgReqd = tpp.pqiChg > tpp.fciChg ? tpp.pqiChg : tpp.fciChg;
        #if 0
            HalDebugPrint(ZONE_TEST1,"   tpp.doiLimitA = %d\n",tpp.doiLimitA);
            HalDebugPrint(ZONE_TEST1,"   tpp.doiLimitB = %d\n",tpp.doiLimitB); 
            HalDebugPrint(ZONE_TEST1,"   tpp.doiLimitC = %d\n",tpp.doiLimitC);
            HalDebugPrint(ZONE_TEST1,"   tpp.doiTimeA  = %d\n",tpp.doiTimeA);
            HalDebugPrint(ZONE_TEST1,"   tpp.doiTimeB  = %d\n",tpp.doiTimeB);
            HalDebugPrint(ZONE_TEST1,"   tpp.pqiChg    = %d\n",tpp.pqiChg);
            HalDebugPrint(ZONE_TEST1,"   tpp.fciChg    = %d\n",tpp.fciChg);  
            HalDebugPrint(ZONE_TEST1,"   tpp.tciTerm   = %d\n",tpp.tciTerm);  
            HalDebugPrint(ZONE_TEST1,"   tpp.pqvTerm   = %d\n",tpp.pqvTerm); 
            HalDebugPrint(ZONE_TEST1,"   tpp.vdepleted = %d\n",tpp.vdepleted);
            HalDebugPrint(ZONE_TEST1,"   tpp.mavV      = %d\n",tpp.mavV);
            HalDebugPrint(ZONE_TEST1,"   tpp.coff_hi   = %d\n",tpp.coff_hi);
            HalDebugPrint(ZONE_TEST1,"   tpp.coff_lo   = %d\n",tpp.coff_lo); 
            HalDebugPrint(ZONE_TEST1,"   tpp.doff_hi   = %d\n",tpp.doff_hi); 
            HalDebugPrint(ZONE_TEST1,"   tpp.doff_lo   = %d\n",tpp.doff_lo); 
            HalDebugPrint(ZONE_TEST1,"   tpp.thermB    = %d\n",tpp.thermB); 
            HalDebugPrint(ZONE_TEST1,"   tpp.thermR0   = %d\n",tpp.thermR0);  
            HalDebugPrint(ZONE_TEST1,"   tpp.thermT0   = %d\n",tpp.thermT0); 
        #endif
            // determine whether or not parameters make sense (we'll check charge current below)
            if( (tpp.doiLimitC <= tpp.doiLimitB) 
            ||  (tpp.doiLimitB <= tpp.doiLimitA)
            ||  (tpp.doiTimeB >= tpp.doiTimeA)
            ||  (tpp.tciTerm >= tpp.fciChg)
            ||  (tpp.pqvTerm > tpp.mavV)
            ||  (tpp.vdepleted < (3000 * kBatteryNumberOfCells))
            ||  (tpp.mavV > (4200 * kBatteryNumberOfCells))
            ||  (tpp.coff_hi <= tpp.coff_lo)
            ||  (tpp.doff_hi <= tpp.doff_lo))
            {
                valid = FALSE;
            }
        }
        HalDebugPrint(ZONE_TEST1,"MinChgReqd = %.1f, Max charge current = %d, invalidate on undercharge = %d\n", minChgReqd, MAX_CHARGE_CURRENT_MA, INVALIDATE_PACK_WHEN_EXCESS_CHARGE_CURRENT_IS_REQD);
        if(minChgReqd > MAX_CHARGE_CURRENT_MA) {
            if (!INVALIDATE_PACK_WHEN_EXCESS_CHARGE_CURRENT_IS_REQD) 
            {
                HalDebugPrint(ZONE_TEST1,"Allowing lower than normal charge rate on pack %d\n", whichBat);
                minChgReqd = MAX_CHARGE_CURRENT_MA;
            }
            else
            {
                HalDebugPrint(ZONE_TEST1,"Invalidating pack %d because calculated charge current exceeds maximum and this is not allowed\n", whichBat);
                valid = FALSE;
            }
        }
        if (valid) {
            tpp.coiLimit	= (uint16)(minChgReqd * 1.2);// ma
            tpp.fciTerm		= (uint16)(minChgReqd * 0.7);// ma
            if(tpp.fciChg >= minChgReqd) tpp.fciChg = minChgReqd;
            if(tpp.pqiChg >= minChgReqd) tpp.pqiChg = minChgReqd;
            tpp.dcvb_off    = 3333 * kBatteryNumberOfCells;    // mv (dcvb_off)
            tpp.low_warn    = 3567 * kBatteryNumberOfCells;    // mv
		#if 0
            HalDebugPrint(ZONE_TEST1,"   tpp.dcvb_off  = %d\n",tpp.dcvb_off); 
            HalDebugPrint(ZONE_TEST1,"   tpp.low_warn  = %d\n",tpp.low_warn); 
            HalDebugPrint(ZONE_TEST1,"   tpp.coiLimit  = %d\n",tpp.coiLimit);  
            HalDebugPrint(ZONE_TEST1,"   tpp.fciTerm   = %d\n",tpp.fciTerm); 
            HalDebugPrint(ZONE_TEST1,"   tpp.fciChg   = %d\n",tpp.fciChg); 
            HalDebugPrint(ZONE_TEST1,"   tpp.pqiChg   = %d\n",tpp.pqiChg); 
		#endif
			// copy thermal info into the per-pack info block
			bi->thermalInfo.ThermB  = tpp.thermB;
			bi->thermalInfo.ThermR0 = tpp.thermR0;
			bi->thermalInfo.ThermT0 = tpp.thermT0;
			bi->thermalInfo.Coff_Lo = tpp.coff_lo;
			bi->thermalInfo.Coff_Hi = tpp.coff_hi;
			bi->thermalInfo.Doff_Lo = tpp.doff_lo;
			bi->thermalInfo.Doff_Hi = tpp.doff_hi;
            valid = pvtAdjustCellConfig(info,whichBat,&tpp);
        }
    }
	else
	{
		HalDebugPrint(ZONE_TEST1,"Invalidating pack because this BSP doesn't support 1st generation packs!\n");
		valid = FALSE;
	}

    if( !pvtValidatePack(whichBat,valid) )	valid = FALSE;
    return valid;
}

// NOTE for these next two, a value of TRUE INHIBITS and a value of FALSE ALLOWS!
void pvtSetPackChargeInhibit(TBatteryInfo *info, int whichBat, BOOL inhibit)
{
	if(inhibit == TRUE) {
		BMU->DMEM.updateInfo.batteryInfo[whichBat].BatStatus |= BXS_CHG_DISABLED;
		info->updateInfo.Status |= kBatStatusChgDisabled;
	} else {
		BMU->DMEM.updateInfo.batteryInfo[whichBat].BatStatus &= ~BXS_CHG_DISABLED;
		info->updateInfo.Status &= ~kBatStatusChgDisabled;
	}
}
void pvtSetPackDischargeInhibit(TBatteryInfo *info, int whichBat, BOOL inhibit)
{
	if(inhibit == TRUE) {
		BMU->DMEM.updateInfo.batteryInfo[whichBat].BatStatus |= BXS_DCHG_DISABLED;
		info->updateInfo.Status |= kBatStatusDchgDisabled;
	} else {
		BMU->DMEM.updateInfo.batteryInfo[whichBat].BatStatus &= ~BXS_DCHG_DISABLED;
		info->updateInfo.Status &= ~kBatStatusDchgDisabled;
	}
}

BOOL pvtBmuIsDisabled( void )
{
	return bmuDisabled;
}

//*********************  PUBLIC API  ******************************************
BOOL bspBatInit(TBatteryControllerInfo *info, TBatteryDriverCalls *calls)
{
	const char FormatString[] = "BCM3382 BMU Picocode rev %s";
	TBatDischargeTable *defaultTable = kBatteryNumberOfCells == 2
		? (TBatDischargeTable *)&default2cell2200mAhDischargeTable 
		: (TBatDischargeTable *)&default3cell2200mAhDischargeTable ;

    BMU->LS.BG_OTP_REGS = kUSE_OTP_TRIM;	// PR21670: make sure OTP value is used for VREF

	HalDebugPrint (ZONE_TEST1, "BCM3382 batteryInit, enter\n");  
#if 0
	HalDebugPrint (ZONE_TEST1, "       updateInfo  = %p\n", &BMU->DMEM.updateInfo);  
	HalDebugPrint (ZONE_TEST1, "       controlInfo = %p\n", &BMU->DMEM.controlInfo);  
	HalDebugPrint (ZONE_TEST1, "       initInfo    = %p\n", &BMU->DMEM.initInfo);  
	HalDebugPrint (ZONE_TEST1, "       ioData      = %p\n", &BMU->DMEM.ioData);  
	HalDebugPrint (ZONE_TEST1, "       picoPrivate = %p\n", &BMU->DMEM.picoPrivate);  
#endif
	if( PartIsTrimmed() == FALSE)
	{
		HalDebugPrint (ZONE_TEST1, "Part is not trimmed...disabling battery functionality\n");
		BMU->LS.SoftReset = PICO_HALT;			// place core into reset
		BMU->CP.BatteryEnable = 0x10;			// deselect all packs
		BMU->CP.ChargerMode = (1 << 3); 		// stop discharge
		BMU->CP.ChargerMode = 0;				// place hardware in sleep
		Sleep(3);
		BMU->CP.AcDisable = 0;
		bmuDisabled = TRUE;
		info->Version = (char *)malloc(strlen("n/a") + 1);
		sprintf(info->Version, "n/a");
		//HalDebugPrint (ZONE_TEST1, "End Part is not trimmed.\n");
		calls->pfBmuIsDisabled = pvtBmuIsDisabled;
		return TRUE;							// pretend everything went OK
	}
	HalDebugPrint (ZONE_TEST1, "  ChgPct = %ld\n", info->ChargePercent);
	//DumpDischargeTable(defaultTable);

	info->Version = (char *)malloc(strlen(FormatString) + strlen(PICO_CODE_REV) + 1);
	if( info->Version) {
		sprintf (info->Version, FormatString, PICO_CODE_REV);
		HalDebugPrint(ZONE_TEST1,"%s\n", info->Version);
	}
	info->BuildTime = PICO_BUILD_TIME_T;
	
	info->Support = (kSupportsLifeTesting      | kSupportsTwoCellBatteries 
				   | kSupportsPackThermistors  | kSupportsPerPackDischargeTable
				   | kSupportsBatteryNonvol    | kSupportsBfcPackValidation
				   | kSupportsDischargeCurrent | kSupportsHWFaults
				   | kSupportsPerPackEnables );


	// Always malloc per-pack discharge tables and clean up during bspBatDeinit (below)
	info->Batteries[0].dischargeTable = CreateDischargeTable(defaultTable);
	info->Batteries[1].dischargeTable = CreateDischargeTable(defaultTable);
	info->Batteries[0].dischargeTable->batZ = (uint16)(kDefaultPackResistance * 1000);
	info->Batteries[1].dischargeTable->batZ = (uint16)(kDefaultPackResistance * 1000);

	// initialize other constant information
	info->VbatPrecision = (uint16)ADC_TO_MILLIVOLTS(1);		// in millivolts!
	info->VinPrecision = (uint16)(1 / ADC_VIN_V_FACTOR);	// in millivolts!
	info->NominalDchgWLowPwr = kNOMINAL_DCHG_WATTS;			// defined in board's NonVolDefaults.h

	calls->pfEnableChargingAll = pvtEnableChargingAll;
	calls->pfSetChargePercentageAll = pvtSetChargePercentageAll;
	calls->pfSetChargePercentageSingle = pvtSetChargePercentageSingle;
	calls->pfStartForcedCharge = pvtStartForcedCharge;
	calls->pfStopCharge = pvtStopCharge;
	calls->pfEnableDischargeAll = pvtEnableDischargeAll;
	calls->pfStartForcedDischarge = pvtStartForcedDischarge;
	calls->pfStopForcedDischarge = pvtStopForcedDischarge;
	calls->pfReadGUID = pvtReadGUID;
	calls->pfMakeHWResistanceMeasurement = pvtMakeHWResistanceMeasurement;
	calls->pfValidatePack = pvtValidatePack;
	calls->pfSetHWFaultMask = pvtSetHWFaultMask;
	calls->pfGetHWFaultMask = pvtGetFaultMask;
	calls->pfGetHWFaults = pvtGetHWFaults;
	calls->pfClearHWFaults = pvtClearHWFaults;
	calls->pfForceWatchdogTimeout = pvtForceWatchdogTimeout;
	calls->pfMainsVoltages = pvtMainsVoltages;
	calls->pfAdjustCellConfig = pvtAdjustCellConfig;
	calls->pfClearChargeFaults = pvtClearChargeFaults;
	calls->pfCompletePackValidation = pvtCompletePackValidation;
	calls->pfSetPackChargeInhibit = pvtSetPackChargeInhibit;			// per-pack enable support
	calls->pfSetPackDischargeInhibit = pvtSetPackDischargeInhibit;

	// load the Pico
	BMU->LS.SoftReset = PICO_HALT;		// place core into reset
	pvtLoadPicocode(TRUE);
	if( pvtVerifyPicocode(TRUE) == FALSE ) return FALSE;
	HalDebugPrint (ZONE_TEST1, "Picocode verified.  Taking BMU pico out of soft reset.\n");
	return pvtStartPico(info, calls);
}

BOOL bspBatDeinit(TBatteryControllerInfo *info)
{
	if( info->Batteries[0].dischargeTable ) free(info->Batteries[0].dischargeTable);
	if( info->Batteries[1].dischargeTable ) free(info->Batteries[1].dischargeTable);
	info->Batteries[0].dischargeTable = info->Batteries[1].dischargeTable = (TBatDischargeTable *)0;
	return TRUE;
}

BOOL bspBatUpdate(TBatteryControllerInfo *info)
{
	uint32			currentState;
	TBMUUpdateInfo	updateInfo;
#ifndef VENDOR_BATTERY_UPDATE_FUNCTION
	int 			ix, numBats;
	long			ibatADC;
#endif

	if( bmuDisabled ) 
	{
		//HDP(ZONE_TEST1,"bspBatUpdate: BMU is disabled...returning\n");
		return TRUE;	// pretend everything is OK
	}
	if (!info) return FALSE;	// sanity check

	HDP (ZONE_TEST1, "bspBatUpdate:\n");

	if( pvtCheckWatchdog(info) == FALSE ) 
	{
			HDP(ZONE_TEST1, "Check watchdog failed - just returning!\n");	
			return FALSE;
	}

	// get stuff that may change as a consequence of the poll...
	info->RawControllerState = currentState = BMU->DMEM.updateInfo.PicoState;

	// get current info - we may overwrite VBAT latter as a consequence of inhibiting the charge
	getupdateinfo(&updateInfo);

	if (!(updateInfo.BMUStatus & STATUS_VALID)) 
	{
		//HDP(ZONE_TEST1, "Status not valid (0x%04x) - just returning!\n", updateInfo.BMUStatus);	
		return FALSE;
	}
	// HDP(ZONE_TEST1, "Status OK - continuing\n");	

	// Convert state
	if(currentState == STATE_DISCHARGE && (updateInfo.BMUStatus & FDCHG_IN_PROGRESS))
	{
		HDP (ZONE_TEST1, "FDIP bit set in BMUStatus\n");
		info->State = kBCForcedDischarge;
	}
	else
		info->State = State2State(currentState);

#ifdef VENDOR_BATTERY_UPDATE_FUNCTION
	return VendorBatteryUpdate(info, &updateInfo);
#else
	// battery current may be any of charge current, discharge current, or idle current
	ibatADC = BMU->SMEM.RecentIBat;
	info->BatteryCurrent = ibatADC / ADC_IBAT_A_FACTOR;
	//if we're charging, we must stop the charge to get valid battery voltage)
	if ((currentState >= STATE_PREQUAL) && (currentState <= STATE_TOPOFF)) 
	{
		// now, suspend the charge
		BMU->DMEM.controlInfo.BMUControl |= CHG_SUSPENDED;
		// wait for a few milliseconds to allow charging to settle out (just in case)
		Sleep(3);
		// then update the battery voltage/current - we'll transform from ADC to V later
		updateInfo.batteryInfo[0].Vbat = BMU->DMEM.updateInfo.batteryInfo[0].Vbat;
		updateInfo.batteryInfo[1].Vbat = BMU->DMEM.updateInfo.batteryInfo[1].Vbat;
	}
	if(!chargingSuspended) {
		// re-allow charging - a nop if we never inhibited it
		BMU->DMEM.controlInfo.BMUControl &= ~CHG_SUSPENDED;
	}

	// fill in additional stuff
	info->ChargerStatus  = updateInfo.BMUStatus;
	info->Faults 		 = updateInfo.HWFaults;
	info->FaultsAllowed  = updateInfo.HWFaultsAllowed;
	info->BLTTemperature = (short)BltAdc2DegC(updateInfo.RecentVTherm, 1023);
	info->InputVoltage   = updateInfo.RecentVin / ADC_VIN_V_FACTOR;

#if (!BCM_REDUCED_IMAGE_SIZE) && !defined(VENDOR_BATTERY_UPDATE_FUNCTION) && defined( PRINT_BATTERY_UPDATE_INFO)
	HDP (ZONE_TEST1, "    state   = %ld (%s)\n", currentState, RawState2String(currentState));
#endif
	HDP (ZONE_TEST1, "    status  = 0x%04x\n", info->ChargerStatus);
	HDP (ZONE_TEST1, "    control = 0x%04lx\n", BMU->DMEM.controlInfo.BMUControl);
	HDP (ZONE_TEST1, "    faults  = 0x%04x\n", info->Faults);
	HDP (ZONE_TEST1, "    allowed = 0x%04x\n", info->FaultsAllowed);
	HDP (ZONE_TEST1, "    WD R/R  = %d/%d\n", info->WatchdogResets, info->WatchdogReloads);
	HDP (ZONE_TEST1, "    IIN     = %d\n", info->AdapterCurrent);
	HDP (ZONE_TEST1, "    IBAT    = %d (from %ld)\n", info->BatteryCurrent, ibatADC);
	HDP (ZONE_TEST1, "    THERM   = %d\n", info->BLTTemperature);
	HDP (ZONE_TEST1, "    VIN     = %d\n", info->InputVoltage);

	for( ix = 0, numBats = 0; ix < MAX_BATTERIES; ix++ )
	{
		uint16 prevStatus = info->Batteries[ix].updateInfo.Status;
		uint16 tempStatus = updateInfo.batteryInfo[ix].BatStatus;
		BOOL statusChanged = ((tempStatus & kBatStatusPresent) == (prevStatus & kBatStatusPresent)) ? FALSE : TRUE;
		info->Batteries[ix].updateInfo.Status = tempStatus & 0x7fff;
		info->Batteries[ix].updateInfo.Status |= (statusChanged ? kBatStatusPresenceChanged : 0);
		info->Batteries[ix].updateInfo.Errors = (tempStatus >> BXS_ERROR_SHIFT) & BXS_ERROR_MASK;

		if( (tempStatus & kBatStatusPresent))
		{
			HDP (ZONE_TEST1, "    Battery %c:\n", (ix == 0 ? 'A' : 'B'));
			HDP (ZONE_TEST1, "        Prev Status: 0x%04x\n", prevStatus);
			HDP (ZONE_TEST1, "        Curr Status: 0x%04x\n", info->Batteries[ix].updateInfo.Status);
			if (!statusChanged && !(tempStatus & kBatStatusValid)) {
				 statusChanged = TRUE;							// forces an invalidation of eprom pages below
				 info->Batteries[ix].updateInfo.Status |= kBatStatusPresenceChanged;
				 if( prevStatus & kBatStatusValid ) {
					// a battery was removed and re-installed so quickly we didn't catch it in our statusChanged test above
					// we need to make this look like a newly inserted pack so the higher-level code will validate it
					// (If the pack was removed and replaced so quickly that even the Pico didn't catch it (<1ms), it doesn't actually
					// matter)
					HDP (ZONE_TEST1, "Battery became invalid while presence remained constant...we probably missed a removal/insertion event.\n");
					info->Batteries[ix].updateInfo.Temperature = TEMP_MEASUREMENT_NOT_SUPPORTED;
					info->Batteries[ix].updateInfo.MeasuredVoltage = 0.0;
					presenceCounter[ix] = kMIN_DETECTION_POLLS-1;	// forces a zeroing of updateInfo.Status below
				}
			}
			if( presenceCounter[ix] > 0 ) {
				presenceCounter[ix] -= 1;
				info->Batteries[ix].updateInfo.Status = 0;
				HDP (ZONE_TEST1, "        Battery detected but detection counter not yet exhausted\n");
			}
			else {
				if( tempStatus & kBatStatusValid )
				{    
					info->Batteries[ix].updateInfo.Temperature = bspPackTemperature(&info->Batteries[ix].thermalInfo, updateInfo.batteryInfo[ix].RidADC, ibatADC );
					info->Batteries[ix].updateInfo.LifeTestVoltage = ADC_TO_MILLIVOLTS(updateInfo.batteryInfo[ix].Vbat);
					info->Batteries[ix].updateInfo.MeasuredVoltage = info->Batteries[ix].updateInfo.LifeTestVoltage;
					if ((currentState == STATE_DISCHARGE)
					||  (currentState == STATE_DYING))
					{
						// Only do this for the selected battery, and only when discharging it.
						if (tempStatus & BXS_SELECTED)
						{
							float Z = info->Batteries[0].dischargeTable->batZ/1000.0;
							float tempV = fabs(Z * (float)info->BatteryCurrent + 0.5);
							info->Batteries[ix].updateInfo.MeasuredVoltage += tempV;	// add drop across series resistance
							HalDebugPrint (ZONE_TEST1, "  Performing battery normalization, adding %fmV (Z = %f, current = %dmA), new V = %fmV\n",
									  tempV, Z, info->BatteryCurrent, info->Batteries[ix].updateInfo.MeasuredVoltage );
						}
					}
					HDP (ZONE_TEST1, "        VBAT = %ld / %ld (%ld ADC)\n", (uint32)info->Batteries[ix].updateInfo.LifeTestVoltage, 
																		 (uint32)info->Batteries[ix].updateInfo.MeasuredVoltage, 
																		  updateInfo.batteryInfo[ix].Vbat);
					HDP (ZONE_TEST1, "        TEMP = %f\n", info->Batteries[ix].updateInfo.Temperature);
				}
				numBats++;
			}
		}
		else
		{
			info->Batteries[ix].updateInfo.Temperature = TEMP_MEASUREMENT_NOT_SUPPORTED;
			info->Batteries[ix].updateInfo.MeasuredVoltage = 0.0;
			presenceCounter[ix] = kMIN_DETECTION_POLLS-1;
		}
		if (statusChanged) 	// force a re-read of the EPROM
		{
			info->Batteries[ix].epromInfo.ValidFlags = 0;
		}
	}
	HDP (ZONE_TEST1, "    #Packs  = %d\n", numBats);
	info->NumBatteries = numBats;
	HDP (ZONE_TEST1, "bspBatUpdate: Done.\n");
	return TRUE;
#endif
}

BOOL bspBatReadOWI(TBatteryControllerInfo *info, int which, int page)
{
	uint32 cmdData = (uint32)page;
	if (page < 0 || page > 3) return FALSE;
	HalDebugPrint(ZONE_TEST1,"bspBatReadOWI: Reading page %d from pack %d...\n", page, which);
	if(SendSimpleCommand(CmdReadEPROM,which,&cmdData))
	{
		uint8 *dptr = info->Batteries[which].epromInfo.pageCache[page];
		uint32 *sptr = (uint32 *)BMU->DMEM.ioData;
		uint32 temp;
		int ix;
		for( ix = 0; ix < 16; ix++) {
			temp = *sptr++;
			*dptr++ = (uint8)((temp >> 8) & 0xff);
			*dptr++ = (uint8)(temp & 0xff);
		}
		info->Batteries[which].epromInfo.ValidFlags |= (uint8)(1 << page);
		HalDebugPrint(ZONE_TEST1,"bspBatReadOWI: OWI read sucessful.\n");
		return TRUE;
	}
	HalDebugPrint(ZONE_TEST1,"bspBatReadOWI: OWI Read returned an error of %d.\n", CmdErrno);
	info->Batteries[which].epromInfo.ValidFlags &= (uint8)(~(1 << page));
	return FALSE;
}

BOOL bspUtilPowerIsPresent(void)
{
	return (BMU->DMEM.updateInfo.BMUStatus & AC_ABSENT) == 0 ? TRUE : FALSE;
}

BatteryControllerState bspCurrentState(void)
{
	uint16 state = BMU->DMEM.updateInfo.PicoState;
	if((state == STATE_DISCHARGE) && (BMU->DMEM.updateInfo.BMUStatus & FDCHG_IN_PROGRESS)) 
		return kBCForcedDischarge;
	else 
		return State2State(state);
}

int bspNumSeriesCellsSupported(void)
{
	return kBatteryNumberOfCells;
}

void bspSuspendCharging(BOOL Suspend)
{
	chargingSuspended = Suspend;		// let the update function in on this
	if (Suspend)
		BMU->DMEM.controlInfo.BMUControl |= CHG_SUSPENDED;
	else
		BMU->DMEM.controlInfo.BMUControl &= ~CHG_SUSPENDED;
}

#undef HDP



//****************************************************************************
//
// Copyright (c) 2010 Broadcom Corporation
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
//  Filename:       3382_BatteryRegs.h
//  Author:         Tom Johnson
//  Creation Date:  20-January-2010
//
//****************************************************************************
//
//  Description:
//      This file defines addresses of the 3382 battery interface in a 
// 		3378-compatible manner.  This allows common coding between the
// 		3378, 3380, and 3382 BSPs.  Thus, any changes in the 3378/3380/3382 battery.c
// 		that causes a change in 3378_map.h should also be duplicated here
//
//****************************************************************************

#ifndef __BCM3382_BATTERYREGS_H
#define __BCM3382_BATTERYREGS_H

#include "bcmtypes.h"

#if __cplusplus
extern "C" {
#endif



/* BMU (battery control picocore) interface */
typedef struct {
	uint32	BatStatus;
	// +-------------------------------------------------------------------------------+
	// | 15 | 14 | 13 | 12 | 11 | 10 | 09 | 08 | 07 | 06 | 05 | 04 | 03 | 02 | 01 | 00 |
	// +-------------------------------------------------------------------------------+
	// |    UNUSED    | STATUS  |   ERROR CNT  | DD | CD |  S |  D |  F |  U |  V |  P |
	// +-------------------------------------------------------------------------------+
	//  status should be masked with 0x180
		#define BXS_PRESENT 	 0x01
		#define BXS_VALID		 0x02
		#define BXS_UCS			 0x04
		#define BXS_FULL		 0x08
		#define BXS_DEPLETED	 0x10
		#define BXS_SELECTED	 0x20
		#define BXS_CHG_DISABLED 0x40
		#define BXS_DCHG_DISABLED 0x80
		#define BXS_ERROR_SHIFT  	8
		#define BXS_ERROR_MASK   	0x07
		#define BXS_PACK_STATUS_SHIFT 11
		#define BXS_PACK_STATUS_MASK  0x03
			#define BXS_PACK_STATUS_MISSING		0
			#define BXS_PACK_STATUS_UNKNOWN		1
			#define BXS_PACK_STATUS_VALID		2
			#define BXS_PACK_STATUS_INVALID		3
	uint32	RidADC;
	uint32	Vbat;
	uint32	IDI;
} TBatInfo;

typedef struct _BMUUpdateInfo {
	uint32		BMUStatus;
		#define STATUS_VALID	 	0x001
		#define LOW_BAT_WARNING	 	0x002
		#define IMMINENT_FAILURE 	0x004
		// bits 3 and 4 are "present" bits for each battery
		#define PresentMask			0x03
		#define PresentShift		3
		// next four are forced charge/discharge in progress/completed status bits
		#define FDCHG_IN_PROGRESS	0x020
		#define FDCHG_COMPLETE 	  	0x040
		#define FCHG_IN_PROGRESS	0x080
		#define	FCHG_IN_COMPLETE	0x100
		#define AC_ABSENT		 	0x200
		// this next bit tells the outside world if we are running code loaded by
		// either the secure boot rom (3378) or the "normal" boot rom (both).  
		// WARNING WARNING It MUST stay in this location!!!!!
		#define SecureBootInProgress 0x400
		#define PWR_ON_RESET	 	0x800
		// LoadTimeoutError - timed out awaiting release
		#define LTE				    0x1000

	uint32		RecentVTherm;
	uint32		RecentVin;   
	uint32		HWFaults;
		// charge faults
		#define COV_FAULT 0x00000001	// charger over voltage
		#define COI_FAULT 0x00000002	// charger over current
		#define CUI_FAULT 0x00000004	// charger under current (future use)
		#define PVR_FAULT 0x00000008	// voltage doesn't rise in prequal 
		// discharge faults
		#define DOI_FAULT 0x00000010	// discharge over current
		#define DHF_FAULT 0x00000020	// general discharge hardware fault
		// general faults
		#define IOV_FAULT 0x00000100	// adapter over voltage
		#define VWE_FAULT 0x00000200	// battery voltage without enable
		#define UIE_FAULT 0x00000400	// unexpected IBAT when enabled
		#define BOV_FAULT 0x00000800	// battery over voltage
		// BFC faults
		#define BES_FAULT 0x00001000 	// battery enables shorted together
	uint32		HWFaultsAllowed;
	uint32		PicoState;
		#define STATE_INIT	      		0
		#define STATE_POST_INIT	      	1
		#define STATE_IDLE	      		2
		#define STATE_SLEEP	      		3
		#define STATE_CHARGE_INIT      	4
		#define STATE_PREQUAL	      	5
		#define STATE_FAST_CHARGE      	6
		#define STATE_TOPOFF	      	7
		#define STATE_CHG_SUSPEND      	8
		#define STATE_DISCHARGE	      	9
		#define STATE_DYING	      	   10
	TBatInfo	batteryInfo[2];
	uint32		reserved[9];
	uint32 		PicoWatchdog;
		#define WATCHDOG_MAGIC_VALUE	0x3c3c
} TBMUUpdateInfo;

enum MipsCommands {
	CmdGR909New 		 = 1,
	CmdReadEPROM		 = 2,
	CmdReadGUID          = 3,
	CmdClearFaults       = 4,
	CmdForcedDischarge   = 5,
	CmdStopForcedDchg    = 6,
	CmdForcedCharge      = 7,
	CmdStopCharge        = 8,
	CmdClearChargeErrors = 9,
	CmdBatteryIsValid    = 10,
	CmdBatteryIsInvalid  = 11,
	CmdSetBatChgDisFlag  = 12,
	CmdSetBatDchgDisFlag = 13,
	CmdLoadFuelMeter     = 14,
	CmdSetChargeParams   = 15,
	CmdCalcResistance    = 16,
};

enum MipsCommandErrorCodes {
	EC_NO_ERROR			= 0,
	EC_INVALID_CMD		= 1,
	EC_INVALID_ADDR		= 2,
	EC_PARAM_ERROR		= 3,
	EC_UNABLE			= 4,
	EC_INVALID_BATT		= 5,
	EC_CMD_ABORTED 		= 6,
	EC_GUID_ERROR  		= 7,
	EC_NO_OWI_PRESENCE	= 8,
};

typedef struct {
	uint32	BAT_VINIT;			// default = 3.977V/cell
	uint32	BAT_PQVCHG;			// default = 4.047V/cell
	uint32	BAT_PQICHG;			// default = 200mA
	uint32	BAT_PQVTERM;		// default = 3V/cell
	uint32	BAT_FCICHG;			// default = 500mA
	uint32	BAT_FCITERM;		// default = 350mA
	uint32	BAT_FCVTERM;		// default = 4.047V/cell
	uint32	BAT_TCVTERM;		// default = 4.047V/cell
	uint32	BAT_TCITERM;		// default = 50mA
	uint32	BAT_TCICHG;			// default = 500mA
	uint32	BAT_COI_LIMIT;		// default = 600mA
	uint32	CC_K0K1IGAIN;		// default = K1 = 7, K0 = 4, IGAIN = 1
	uint32	CV_K0K1IGAIN;		// default = K1 = 4, K0 = 0, IGAIN = 4
	uint32	BAT_DOI_LIMIT_A;	// default = 1.2A
	uint32	BAT_DOI_LIMIT_B;	// default = 2.0A
	uint32	BAT_DOI_LIMIT_C;	// default = 4.2A
	uint32	BAT_DOI_TIMEOUT_A;	// default = 2sec
	uint32	BAT_DOI_TIMEOUT_B;	// default = 200ms
	uint32	BAT_DCVB_ON;		// default = 3V/cell
	uint32	BAT_DCVB_OFF;		// default = 3.333V/cell
	uint32	BAT_MAX_BAT_CHG;	// default = 4.193V/cell
	uint32	BAT_LOW_WARN;		// default = 3.567V/cell
	uint32	reserved[2];
} TBatChargeParams;

typedef struct {
	uint32		BMUControl;
		#define CHG_SUSPENDED 		0x0001
		#define DISCHARGE_DISABLED 	0x0002
		#define CHARGE_DISABLED		0x0004
		#define INHIBIT_ID_CHK		0x0008
		#define INHIBIT_WATCHDOG	0x0020
		#define STARTUP_HANDSHAKE	0x2000
	uint32		MipsCmd;
	// +---------------------------------------------------------------+
	// | 15| 14| 13| 12| 11| 10| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	// +---------------------------------------------------------------+
	// |         QUAL      |     CEC       |       COMMAND     |INT| V |
	// +---------------------------------------------------------------+
		#define CMD_VALID	   		1
		#define CMD_INTERRUPT  		2
		#define CMD_SHIFT			2
		#define CMD_ERR_SHIFT  		7
		#define CMD_ERR_MASK		0x0f
		#define CMD_QUAL_SHIFT 		11
		#define CMD_QUAL_MASK		0x1f
	uint32		CmdDataLSW;
	uint32		CmdDataMSW;
	uint32		CmdExtra1;
	uint32		CmdExtra2;
	uint32		reserved[2];
} TBMUControlInfo;

typedef struct {
	uint32		PCTimeout;		// millisecs >> 15 - default = 4 minutes
	uint32		FCTimeout;		// millisecs >> 15 - default = 8 hours
	uint32		TCTimeout;		// millisecs >> 15 - default = 8 hours
	uint32		DCStartStopMS;	// default = 3
	uint32		MaxChgErrors;	// default = 3
	uint32		BatSelectDelay;	// default = 3ms
	uint32		SenseNumerator;	// default = 1
	uint32		SenseDenominator; // default = 1
	uint32		MaxIDCurrent;	// default = 610mA
	uint32		PVRLimit;		// default = 1 count/second
	uint32		NumTCCounts;	// default = 10
	uint32		BOV_FAULT_LIMIT;// default = 4.25V/cell		
	uint32		IOV_FAULT_LIMIT;// default = 14.015V (12V adapter)
	uint32		MVCHG_ON;		// default = 11.369V (12V adapter)
	uint32		MVCHG_OFF;		// default = 11.041V (12V adapter)
	uint32		MVMAX_IDLE;		// default = 13.226V (12V adapter)
	uint32		MVMIN_SYS;		// default = 4V (12V adapter)
	uint32		MVLOW_MIN;		// default = 8.018V (12V adapter)
	uint32		MVLOW_MAX;		// default = 9.611V (12V adapter)
	uint32		IMP_I_OFFSET;	// default = 250mA		
	uint32		IMP_I_SCALE;	// default = 100mA		
	uint32		IMP_FCW;		// default = 950Hz
	uint32		IMP_MCL;		// default = 100 cycles
	uint32 		POR_VALID_SECS;	// default = 17 secs (3 Polls + 2 secs)
	uint32		BMU_OTP_CAL;	// default = 0
	uint32		reserved[7];
	TBatChargeParams	batChargeParams[2];
} TBMUInitInfo;

typedef struct Bmu16BitSharedDataMemoryRegisters // Offsets 0xfff9d000 - 0xfff9d3fc
{
	TBMUUpdateInfo	updateInfo;
	TBMUControlInfo controlInfo;
	TBMUInitInfo	initInfo;
	uint32			ioData[16];
	uint32			picoPrivate[128];
} Bmu16BitSharedDataMemoryRegisters;

typedef struct Bmu32BitSharedDataMemoryRegisters
{
	long		RecentIAdapter;	// 3368 ONLY
	long		RecentIBat;
	uint32		Param32_L;
	uint32		Param32_H;
	uint32		reserved;
	long		BatFuelgauge[2];
	uint32		picoPrivate[12];
	uint32		picoStack[109];
} Bmu32BitSharedDataMemoryRegisters;

typedef struct BmuLoadStoreRegisters // Offsets 0xfff9d800 - 0xfff9d8f4
{
	uint32		SoftReset;
		#define PICO_RUN_FROM_ROM	0x00
		#define PICO_RUN_FROM_RAM	0x04
		#define PICO_HALT 			0x0c
	uint32		WakeStatus;
	uint32		WakeMask;
	uint32		PwmStatus;
	uint32		BootStatus;
	uint32		Spare0;
	uint32		Spare1;
	uint32		PwmMinMax;
	uint32		rsvd1;
	uint32		ChargeControl1;
	uint32		rsvd2[2];
	uint32		VbatAvgCtrl;
	uint32		IbatAvgCtrl;
	uint32		CalAvgCtrl;
	uint32		ThermAvgCtrl;
	uint32		RidAAvgCtrl;
	uint32		RidBAvgCtrl;
	uint32		VinAvgCtrl;
	uint32		IBatCoarseAvgCtrl;
	uint32		VbatInst;
	uint32		IbatInst;
	uint32		CalInst;
	uint32		ThermInst;
	uint32		RidAInst;
	uint32		RidBInst;
	uint32		VinInst;
	uint32		IBatCoarseInst;
	uint32		VbatAvg;
	uint32		IbatAvg;
	uint32		CalAvg;
	uint32		ThermAvg;
	uint32		RidAAvg;
	uint32		RidBAvg;
	uint32		VinAvg;
	uint32		IBatCoarseAvg;
	uint32		VbatStatus;
	uint32		IbatStatus;
	uint32		CalStatus;
	uint32		ThermStatus;
	uint32		RidAStatus;
	uint32		RidBStatus;
	uint32		VinStatus;
	uint32		IBatCoarseStatus;
	uint32		ChargeMeter[2];
	uint32		rsvd3[2];
	uint32		FMValid;
	uint32		IIEnergyMeter[2];
	uint32		VIEnergyMeter[2];
	uint32		ImpedanceICos;
	uint32		ImpedanceISin;
	uint32		ImpedanceVCos;
	uint32		ImpedanceVSin;
	uint32		SerialTestInterface[2];
	uint32		OtpLow;
	uint32		OtpHigh;
	uint32		IntegStartValue;
	uint32		BG_OTP;
		// ctat_otp in bits 13:8, ptat_otp in bits 5:0 
	uint32		BG_OTP_REGS;
		#define kUSE_OTP_TRIM 0x10000	// 0x10000 = use BG_OTP_REGS, 0x00000 = use BG_OTP
		// ctat in bits 13:8, ptat in bits 5:0 
	uint32		GPO;
	uint32		GPI;
	uint32		GPO_OEN;
} BmuLoadStoreRegisters;

typedef struct BmuCoProcessorRegisters // Offsets 0xfff9cc00 - 0xfff9cd8c
{
	uint32		TimerMS;
	uint32		TimerUs;
	uint32		rsvd1[2];
	uint32		Interrupt;
	uint32		rsvd2[2];
	uint32		AcDisable;
	uint32		rsvd3[3];
	uint32		BatteryEnable;
	uint32		rsvd4[2];
	uint32		ActiveBat;
	uint32		rsvd5;
	uint32		ChargerMode;
	uint32		rsvd6[3];
	uint32		IbatDisThresh;
	uint32		VbatDisThresh;
	uint32		VbatChgThresh;
	uint32		IbatChgThresh;
	uint32		rsvd7[2];
	uint32		FuelMeterCtrl;
	uint32		rsvd8[3];
	uint32		FuelMeterLoad;
	uint32		rsvd9;
	uint32		AnaPD;
	uint32		RrSgl[2];
	uint32		SglMeasSel[2];
	uint32		SlewWindowSizes;
	uint32		rsvd10[4];
	uint32		VbatCorrection;
	uint32		IbatCorrection;
	uint32		IGainShift;
	uint32		rsvd11;
	uint32		PulseStartTime;
	uint32		HiLowSpeedPWM;
	uint32		CMStartValue;
	uint32		EMStartValue;
	uint32		OwiControl;
	uint32		OwiConfig;
	uint32		OwiStatus;
	uint32		ImpedanceModStart;
	uint32		ImpedanceBusy;
	uint32		rsvd12[4];
	uint32		ImpedanceExp;
	uint32		ImpedanceAccumSat;
	uint32		ImpedanceDone;
	uint32		ImpedanceShiftDone;
	uint32		ImpedanceStart;
	uint32		ImpedanceVScale;
	uint32		ImpedanceIScale;
	uint32		ImpedanceFcw;
	uint32		ImpedanceMeasCycleLen;
} BmuCoProcessorRegisters;

typedef struct BmuBlockRegisters // Offsets 0xb5401000 - 0xb5401d0c
{
  Bmu16BitSharedDataMemoryRegisters  DMEM;    	// (0xb4a01000 - 0xb4a013fc)
  Bmu32BitSharedDataMemoryRegisters  SMEM;    	// (0xb4a01400 - 0xb4a015fc)
  uint32							 rsvd1[128];// (0xb4a01600 - 0xb4a017fc)
  BmuLoadStoreRegisters              LS;        // (0xb4a01800 - 0xb4a01908)
  uint32							 rsvd2[189];// (0xb4a0190c - 0xb4a01bfc)
  BmuCoProcessorRegisters            CP;        // (0xb4a01c00 - 0xb4a01d0c)
} BmuBlockRegisters;

#define BMU ((volatile BmuBlockRegisters * const) BMU_BASE)
#define BMU_IMEM ((volatile uint32 * const) APM_PICO_IMEM_BASE) // BMU instruction memory (where the microcode lives)

#if __cplusplus
}
#endif

#endif

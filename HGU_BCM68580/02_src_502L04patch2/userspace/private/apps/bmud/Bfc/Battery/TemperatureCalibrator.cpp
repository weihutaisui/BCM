//****************************************************************************
//
// Copyright (c) 2008-2009 Broadcom Corporation
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
//  Filename:       TemperatureCalibrator.cpp
//  Author:         Tom Johnson
//  Creation Date:  September 8, 2008
//
//****************************************************************************
//  Description:
//      Temperature calibration class.
//
//****************************************************************************
#include "TemperatureCalibrator.h"
#include "DefaultTemperatureCalibrationData.h"
#include "MergedNonVolDefaults.h"

#if 0
// Here's an example vendor-supplied table:
const TTemperatureCalTableEntry myBLT2BoardTempCalTable[] = {
	{-10,-15},
	{0,-5},
	{17,12},
	{25,19},
	{43,37}
};

// remember to place the following in your NonVolDefaults.h:
#define VENDOR_SUPPLIED_BLT_TO_BOARD_TEMPERATURE_CAL_TABLE myBLT2BoardTempCalTable
#define VENDOR_SUPPLIED_BLT_TO_BOARD_TEMPERATURE_CAL_TABLE_NUM_ENTRIES 5

#endif

#ifdef VENDOR_SUPPLIED_BLT_TO_BOARD_TEMPERATURE_CAL_TABLE
	extern const TTemperatureCalTableEntry VENDOR_SUPPLIED_BLT_TO_BOARD_TEMPERATURE_CAL_TABLE[];
#endif
#ifdef VENDOR_SUPPLIED_BLT_TO_PACK_TEMPERATURE_CAL_TABLE
	extern const TTemperatureCalTableEntry VENDOR_SUPPLIED_BLT_TO_PACK_TEMPERATURE_CAL_TABLE[];
#endif
#ifdef VENDOR_SUPPLIED_TSC_TO_BOARD_TEMPERATURE_CAL_TABLE
	extern const TTemperatureCalTableEntry VENDOR_SUPPLIED_TSC_TO_BOARD_TEMPERATURE_CAL_TABLE[];
#endif
#ifdef VENDOR_SUPPLIED_TSC_TO_PACK_TEMPERATURE_CAL_TABLE
	extern const TTemperatureCalTableEntry VENDOR_SUPPLIED_TSC_TO_PACK_TEMPERATURE_CAL_TABLE[];
#endif

TTemperatureCalibrator::TTemperatureCalibrator()
{
	calTable = NULL;
	numEntries = 0;
}

TTemperatureCalibrator::~TTemperatureCalibrator()
{
}

float TTemperatureCalibrator::CalibrateTemperature(float inputTemperature)
{
	if( calTable == NULL || numEntries == 0) return inputTemperature;
	if( numEntries == 1 || inputTemperature <= calTable[0].measuredTemperature)
	{
		// fixed offset
		return inputTemperature + 
			(calTable[0].calibratedTemperature - calTable[0].measuredTemperature);
	}
	// see if we fall off the table
	if( inputTemperature >= calTable[numEntries-1].measuredTemperature )
	{
		// fixed offset
		return inputTemperature + 
			(calTable[numEntries-1].calibratedTemperature - calTable[numEntries-1].measuredTemperature);
	}
	// real work to do...find the bounding entries
	int upperIdx, lowerIdx;
	for (upperIdx = 1; upperIdx < numEntries; upperIdx++) {
		if( calTable[upperIdx].measuredTemperature >= inputTemperature )
			break;
	}
	if( calTable[upperIdx].measuredTemperature == inputTemperature )
	{
		// fixed offset
		return inputTemperature + 
			(calTable[upperIdx].calibratedTemperature - calTable[upperIdx].measuredTemperature);
	}
	lowerIdx = upperIdx-1;
	// interpolate
	float pct = (inputTemperature - calTable[lowerIdx].measuredTemperature)/
				(calTable[upperIdx].measuredTemperature - calTable[lowerIdx].measuredTemperature);
	return (calTable[lowerIdx].calibratedTemperature + 
				(pct * (calTable[upperIdx].calibratedTemperature - calTable[lowerIdx].calibratedTemperature)));
}


TBLTtoBoardTemperatureCalibrator::TBLTtoBoardTemperatureCalibrator()
{
	#ifdef VENDOR_SUPPLIED_BLT_TO_BOARD_TEMPERATURE_CAL_TABLE
		calTable = VENDOR_SUPPLIED_BLT_TO_BOARD_TEMPERATURE_CAL_TABLE;
		numEntries = VENDOR_SUPPLIED_BLT_TO_BOARD_TEMPERATURE_CAL_TABLE_NUM_ENTRIES;
	#else
		calTable = gDefaultBltToBoardTemperatureCalibrationData;
		numEntries = sizeof(gDefaultBltToBoardTemperatureCalibrationData)/sizeof(TTemperatureCalTableEntry);
	#endif
}

TBLTtoPackTemperatureCalibrator::TBLTtoPackTemperatureCalibrator()
{
	#ifdef VENDOR_SUPPLIED_BLT_TO_PACK_TEMPERATURE_CAL_TABLE
		calTable = VENDOR_SUPPLIED_BLT_TO_PACK_TEMPERATURE_CAL_TABLE;
		numEntries = VENDOR_SUPPLIED_BLT_TO_PACK_TEMPERATURE_CAL_TABLE_NUM_ENTRIES;
	#else
		calTable = gDefaultBltToPackTemperatureCalibrationData;
		numEntries = sizeof(gDefaultBltToPackTemperatureCalibrationData)/sizeof(TTemperatureCalTableEntry);
	#endif
}

#ifdef BFC_INCLUDE_THERMAL_MONITOR_SUPPORT
TTSCtoBoardTemperatureCalibrator::TTSCtoBoardTemperatureCalibrator()
{
	#ifdef VENDOR_SUPPLIED_TSC_TO_BOARD_TEMPERATURE_CAL_TABLE
		calTable = VENDOR_SUPPLIED_TSC_TO_BOARD_TEMPERATURE_CAL_TABLE;
		numEntries = VENDOR_SUPPLIED_TSC_TO_BOARD_TEMPERATURE_CAL_TABLE_NUM_ENTRIES;
	#else
		calTable = gDefaultTscToBoardTemperatureCalibrationData;
		numEntries = sizeof(gDefaultTscToBoardTemperatureCalibrationData)/sizeof(TTemperatureCalTableEntry);
	#endif
}

TTSCtoPackTemperatureCalibrator::TTSCtoPackTemperatureCalibrator()
{
	#ifdef VENDOR_SUPPLIED_TSC_TO_PACK_TEMPERATURE_CAL_TABLE
		calTable = VENDOR_SUPPLIED_TSC_TO_PACK_TEMPERATURE_CAL_TABLE;
		numEntries = VENDOR_SUPPLIED_TSC_TO_PACK_TEMPERATURE_CAL_TABLE_NUM_ENTRIES;
	#else
		calTable = gDefaultTscToPackTemperatureCalibrationData;
		numEntries = sizeof(gDefaultTscToPackTemperatureCalibrationData)/sizeof(TTemperatureCalTableEntry);
	#endif
}
#endif	// BFC_INCLUDE_THERMAL_MONITOR_SUPPORT




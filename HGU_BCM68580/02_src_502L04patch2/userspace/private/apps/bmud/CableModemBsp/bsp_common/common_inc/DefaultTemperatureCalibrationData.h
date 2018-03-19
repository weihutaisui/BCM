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
//  Filename:       DefaultTemperatureCalibrationData.h
//  Author:         Kevin O'Neal
//  Creation Date:  May 4, 2006
//
//****************************************************************************
//  Description:
//      Default temperature calibration data which maps a temperature as
//      measured by a temperature probe vs. the reported temperature from
//      the thermistor.  This data is used to normalize temperature when the
//      thermistor is physically located on the board at a point other than
//      the location where we want to measure the temperature.
//
//      The default values in this file map the thermistor temperature as 
//      being equal to the measured temperature, and as such really doesn't
//      do anything meaningful.  This file exists primarily to serve as an 
//      example for vendors who need to provide this kind of data.
//
//      PR 11350
//
//****************************************************************************


#ifndef DEFAULT_TEMPERATURE_CALIBRATION_DATA_H
#define DEFAULT_TEMPERATURE_CALIBRATION_DATA_H

const TTemperatureCalTableEntry gDefaultBltToPackTemperatureCalibrationData[] = 
{
  // input thermistor °C, output temperature °C (this default table is 1:1)
  {-10, -10},
    {0,  0},
   {10,  10},
   {20,  20},
   {30,  30},
   {40,  40},
   {50,  50},
   {60,  60},
   {70,  70},
   {80,  80}
};

const TTemperatureCalTableEntry gDefaultBltToBoardTemperatureCalibrationData[] = 
{
	// input thermistor °C, output temperature °C (this default table is 1:1)
  {-10, -10},
    {0,  0},
   {10,  10},
   {20,  20},
   {30,  30},
   {40,  40},
   {50,  50},
   {60,  60},
   {70,  70},
   {80,  80}
};

#ifdef BFC_INCLUDE_THERMAL_MONITOR_SUPPORT
const TTemperatureCalTableEntry gDefaultTscToBoardTemperatureCalibrationData[] = 
{
	// input thermistor °C, output temperature °C
    {0,  0}		// 1:1
};
const TTemperatureCalTableEntry gDefaultTscToPackTemperatureCalibrationData[] = 
{
	// input thermistor °C, output temperature °C
    {0,  0}		// 1:1
};
#endif	// BFC_INCLUDE_THERMAL_MONITOR_SUPPORT

#endif	// DEFAULT_TEMPERATURE_CALIBRATION_DATA_H

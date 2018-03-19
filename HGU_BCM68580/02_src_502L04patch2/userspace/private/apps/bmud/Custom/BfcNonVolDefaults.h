/***********************************************************************
 *
 *  Copyright (c) 2010-2013  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2012:proprietary:standard

 This program is the proprietary software of Broadcom and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").  Except as set forth in
 an Authorized License, Broadcom grants no license (express or implied), right
 to use, or waiver of any kind with respect to the Software, and Broadcom
 expressly reserves all rights in and to the Software and all intellectual
 property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,

 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use
    all reasonable efforts to protect the confidentiality thereof, and to
    use this information only in connection with your use of Broadcom
    integrated circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
    PERFORMANCE OF THE SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
    LIMITED REMEDY.
:>
 *
 ************************************************************************/

#ifndef BfcNonVolDefaults_H
#define BfcNonVolDefaults_H

// PR 11916:  Default value for BcmBfcAppNonVolSettings::SerialConsoleMode
// which controls whether the serial console is disabled, read-only, or
// read-write.  Enum values which may be used here are as follows:
// kConsoleDisabled, kConsoleReadOnly, kConsoleReadWrite
#define kDefaultValue_SerialConsoleMode kConsoleReadWrite

// +-------------------------+
// | Battery NonVol Settings |
// +-------------------------+
#define kDefaultValue_NominalDischargeSecondsPerBattery 43200 // 12 hours @ 100% charge
#define kNOMINAL_DCHG_WATTS	   ((float)2.2067)				  // watts (PR 15640)
#define kDefaultValue_ResetBatteryControllerOnPoll false // PR 9120 (9099)

#define SWITCHING_CHARGER 0
#define LINEAR_CHARGER 1
#define kBatteryChargerCircuitType SWITCHING_CHARGER // PR 9177

// PR 9257: Per-interface shutdown timing
#define kDefaultValue_DocsisCmPowerMgmtTimeoutSeconds     0
#define kDefaultValue_EthernetPowerMgmtTimeoutSeconds     0
#define kDefaultValue_HpnaPowerMgmtTimeoutSeconds         0
#define kDefaultValue_UsbPowerMgmtTimeoutSeconds          0
#define kDefaultValue_BluetoothPowerMgmtTimeoutSeconds    0
#define kDefaultValue_WiFiPowerMgmtTimeoutSeconds         0

#define kDefaultValue_WiFi2PowerMgmtTimeoutSeconds        0
#define kDefaultValue_MocaPowerMgmtTimeoutSeconds         0

// PR 9726: threshold for declaring a battery low in terms of % charge instead
// of estimated minutes remaining
#define kDefaultValue_BatteryLowChargePercent 25

// PR 9964: programmable values for thermal charge suspension thresholds
// Defaults are 0 and 45 degrees C.  However this value may need to be tweaked
// per design to account for variations in physical location of the thermistor
// versus physical location of the battery itself.
#define kDefaultValue_BatteryChargeSuspensionLoTemp 0  // degrees C
#define kDefaultValue_BatteryChargeSuspensionHiTemp 45 // degrees C

// PR 9967: programmable value for high temp threshold upon which we enter
// low power mode to try and cool things down.
#define kDefaultValue_BatteryLowPowerModeHiTemp     80 // degrees C (changed from 50 by PR20352)

// PR 12319: programmable values for thermal discharge suspension thresholds
// Defaults are 0 and 45 degrees C.  However this value may need to be tweaked
// per design to account for variations in physical location of the thermistor
// versus physical location of the battery itself.
#define kDefaultValue_BatteryDischargeSuspensionLoTemp -10  // degrees C
#define kDefaultValue_BatteryDischargeSuspensionHiTemp  60 // degrees C

// PR 11350: New way of calibrating temperature which uses arbitrary
// temperature points rather than forcing the use of (-10°C..80°C) in 10°C
// increments.  Instead just point to a header file with the data.
#define kTemperatureCalibrationDataFile "DefaultTemperatureCalibrationData.h"

// Default number of cells for battery is 3.
#define kBatteryNumberOfCells 3

// PR 15538: Default adapter current limits for charging (mA)
#define kChargerOffAdapterCurrent 1200
#define kChargerOnAdapterCurrent   503


//********************** Global Variables ************************************

//********************** Forward Declarations ********************************

//********************** Class Declaration ***********************************

//********************** Inline Method Implementations ***********************


#endif




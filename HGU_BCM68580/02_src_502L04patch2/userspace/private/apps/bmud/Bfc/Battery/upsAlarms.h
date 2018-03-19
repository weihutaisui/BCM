//****************************************************************************
//
// Copyright (c) 2005-2009 Broadcom Corporation
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
//  Filename:       upsAlarms.h
//  Author:         Kevin O'Neal
//  Creation Date:  January 13, 2005
//
//****************************************************************************
//  Description:
//      UPS Alarms header file.
//
//****************************************************************************

#ifndef UPSALARMS_H
#define UPSALARMS_H

// "One or more batteries have been determined to require replacement."
#define kOID_upsAlarmBatteryBad           BcmObjectId("1.3.6.1.2.1.33.1.6.3.1")

// "The UPS is drawing power from the batteries."
#define kOID_upsAlarmOnBattery            BcmObjectId("1.3.6.1.2.1.33.1.6.3.2")
        
// "The remaining battery run-time is less than or equal to upsConfigLowBattTime."
#define kOID_upsAlarmLowBattery           BcmObjectId("1.3.6.1.2.1.33.1.6.3.3")

// "The UPS will be unable to sustain the present load when and if the utility power is lost."        
#define kOID_upsAlarmDepletedBattery      BcmObjectId("1.3.6.1.2.1.33.1.6.3.4")

// "A temperature is out of tolerance."        
#define kOID_upsAlarmTempBad              BcmObjectId("1.3.6.1.2.1.33.1.6.3.5")

// "An input condition is out of tolerance."        
#define kOID_upsAlarmInputBad             BcmObjectId("1.3.6.1.2.1.33.1.6.3.6")

// "An output condition (other than OutputOverload) is out of tolerance."        
#define kOID_upsAlarmOutputBad            BcmObjectId("1.3.6.1.2.1.33.1.6.3.7")

// "The output load exceeds the UPS output capacity."        
#define kOID_upsAlarmOutputOverload       BcmObjectId("1.3.6.1.2.1.33.1.6.3.8")

// "The Bypass is presently engaged on the UPS."        
#define kOID_upsAlarmOnBypass             BcmObjectId("1.3.6.1.2.1.33.1.6.3.9")

// "The Bypass is out of tolerance."        
#define kOID_upsAlarmBypassBad            BcmObjectId("1.3.6.1.2.1.33.1.6.3.10x")

// "The UPS has shutdown as requested, i.e., the output is off."        
#define kOID_upsAlarmOutputOffAsRequested BcmObjectId("1.3.6.1.2.1.33.1.6.3.11")

// "The entire UPS has shutdown as commanded."        
#define kOID_upsAlarmUpsOffAsRequested    BcmObjectId("1.3.6.1.2.1.33.1.6.3.12")

// "An uncorrected problem has been detected within the UPS charger subsystem."        
#define kOID_upsAlarmChargerFailed        BcmObjectId("1.3.6.1.2.1.33.1.6.3.13")

// "The output of the UPS is in the off state."        
#define kOID_upsAlarmUpsOutputOff         BcmObjectId("1.3.6.1.2.1.33.1.6.3.14")

// "The UPS system is in the off state."        
#define kOID_upsAlarmUpsSystemOff         BcmObjectId("1.3.6.1.2.1.33.1.6.3.15")

// "The failure of one or more fans in the UPS has been detected."        
#define kOID_upsAlarmFanFailure           BcmObjectId("1.3.6.1.2.1.33.1.6.3.16")

// "The failure of one or more fuses has been detected."        
#define kOID_upsAlarmFuseFailure          BcmObjectId("1.3.6.1.2.1.33.1.6.3.17")

// "A general fault in the UPS has been detected."        
#define kOID_upsAlarmGeneralFault         BcmObjectId("1.3.6.1.2.1.33.1.6.3.18")

// "The result of the last diagnostic test indicates a failure."        
#define kOID_upsAlarmDiagnosticTestFailed BcmObjectId("1.3.6.1.2.1.33.1.6.3.19")

// "A problem has been encountered in the communications between the agent and the UPS."        
#define kOID_upsAlarmCommunicationsLost   BcmObjectId("1.3.6.1.2.1.33.1.6.3.20")

// "The UPS output is off and the UPS is awaiting the return of input power."        
#define kOID_upsAlarmAwaitingPower        BcmObjectId("1.3.6.1.2.1.33.1.6.3.21")

// "A upsShutdownAfterDelay countdown is underway."        
#define kOID_upsAlarmShutdownPending      BcmObjectId("1.3.6.1.2.1.33.1.6.3.22")
        
// "The UPS will turn off power to the load in less than 5 seconds this may be either a timed shutdown or a low battery shutdown."
#define kOID_upsAlarmShutdownImminent     BcmObjectId("1.3.6.1.2.1.33.1.6.3.23")
        
// "A test is in progress, as initiated and indicated by the Test Group."
#define kOID_upsAlarmTestInProgress       BcmObjectId("1.3.6.1.2.1.33.1.6.3.24")

/* The following are BRCM enterprise alarms */

// "The value of upsEstimatedChargeRemaining is less than or equal
//  to the value of battChargerLowChargeThreshold."
#define kOID_bcmAlarmLowBatteryCharge           BcmObjectId("1.3.6.1.4.1.4413.2.2.2.1.10.4.1")

// "The temperature of the device has exceeded operational limits
//  for safe battery charging, and battery charging has been suspeded."
#define kOID_bcmAlarmTempBadChargeSuspended     BcmObjectId("1.3.6.1.4.1.4413.2.2.2.1.10.4.2")

// "The temperature of the device has exceeded operational limits,
//  and the device has been placed in low power mode in an to attempt
//  to reduce heat production."
#define kOID_bcmAlarmTempBadPowerReduced        BcmObjectId("1.3.6.1.4.1.4413.2.2.2.1.10.4.3")

// "There are no batteries installed in the device and the device will
//  not be capable of sustaining power in the event of utility power
//  loss."
#define kOID_bcmAlarmBatteryMissing             BcmObjectId("1.3.6.1.4.1.4413.2.2.2.1.10.4.4")

// "The temperature of the device has exceeded operational limits
// for safe battery discharging, and battery discharging has been 
// inhibited."
#define kOID_bcmAlarmTempBadDischargeInhibited  BcmObjectId("1.3.6.1.4.1.4413.2.2.2.1.10.4.5")

// "The battery controller has detected a hardware fault condition"
#define kOID_bcmAlarmHardwareFault  			BcmObjectId("1.3.6.1.4.1.4413.2.2.2.1.10.4.6")

// "the system has been placed into low power mode"
#define kOID_bcmAlarmLowPower  			        BcmObjectId("1.3.6.1.4.1.4413.2.2.2.1.10.4.7")

#endif

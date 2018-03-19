/*
* <:copyright-BRCM:2013:proprietary:epon
* 
*    Copyright (c) 2013 Broadcom 
*    All Rights Reserved
* 
*  This program is the proprietary software of Broadcom and/or its
*  licensors, and may only be used, duplicated, modified or distributed pursuant
*  to the terms and conditions of a separate, written license agreement executed
*  between you and Broadcom (an "Authorized License").  Except as set forth in
*  an Authorized License, Broadcom grants no license (express or implied), right
*  to use, or waiver of any kind with respect to the Software, and Broadcom
*  expressly reserves all rights in and to the Software and all intellectual
*  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
*  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
*  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
* 
*  Except as expressly set forth in the Authorized License,
* 
*  1. This program, including its structure, sequence and organization,
*     constitutes the valuable trade secrets of Broadcom, and you shall use
*     all reasonable efforts to protect the confidentiality thereof, and to
*     use this information only in connection with your use of Broadcom
*     integrated circuit products.
* 
*  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
*     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
*     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
*     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
*     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
*     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
*     PERFORMANCE OF THE SOFTWARE.
* 
*  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
*     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
*     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
*     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
*     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
*     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
*     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
*     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
*     LIMITED REMEDY.
* :> 
*/


#if !defined(Alarms_h)
#define Alarms_h
////////////////////////////////////////////////////////////////////////////////
/// \file Alarms.h
/// \brief Alarm conditions and reporting
///
/// The Alarms code handles alarm conditioning, hierarchical masking, and
/// notification of alarm conditions.
///
/// Terminology in this module:
///
/// Condition: presence of a state or event that can cause an alarm
///
/// Conditioning: process of "debouncing" a condition to see if it should
/// really become an alarm.  Also called "soaking".  Some conditions have to
/// persist or reoccur before alarms are declared.  Alarms may require soaking
/// before they clear, just as conditions require soaking before they become
/// alarms.
///
/// Alarm: Noteworthy and usually bad thing that requires attention.  Conditions
/// grow up to become alarms if they survive conditioning
///
/// Masking: The hiding of one alarm by the presence of another.  For instance,
/// a loss of signal alarm might mask all other low-level comms link alarms,
/// since if there's no signal the other alarms are irrelevant and perhaps even
/// expected.
////////////////////////////////////////////////////////////////////////////////

#include "Teknovus.h"
#include "AlarmCodes.h"
#include "Oam.h"


#if defined(__cplusplus)
extern "C" {
#endif

typedef struct
    {
    U8          alarmId;
    // alarm raise threshold
    U32        	AlmRaise;
    U32        	AlmClear;
    }PACK AlmMonThd;


////////////////////////////////////////////////////////////////////////////////
/// AlarmSetMask:  indicates Mask has setted
///
 // Parameters:
/// \param cond Condition that has occurred
/// \param mask Mask the alarm or not
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void AlarmSetMask (AlmCondition cond, BOOL mask);



////////////////////////////////////////////////////////////////////////////////
/// AlarmSetCondition:  indicates condition has occurred
///
/// Detailed description
///
 // Parameters:
/// \param cond Condition that has occurred
/// \param inst Instance of that condition
/// \param stat Statistic to which condition applies (if stat alarm)
///
/// \return
/// Description of return value
////////////////////////////////////////////////////////////////////////////////
extern
void AlarmSetCondition (AlmCondition cond, U8 inst) REENTRANT;



////////////////////////////////////////////////////////////////////////////////
/// AlarmClearCondition:  indicates condition has gone away
///
 // Parameters:
/// \param cond Condition that has gone away
/// \param inst Instance of that condition
/// \param stat Statistic to which condition applies (if stat alarm)
///
/// \return
/// Description of return value
////////////////////////////////////////////////////////////////////////////////
extern
void AlarmClearCondition (AlmCondition cond, U8 inst) REENTRANT;


////////////////////////////////////////////////////////////////////////////////
/// AlarmAddLink:  Add a new link with no alarms
///
 // Parameters:
/// \param inst     Link to add
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void AlarmAddLink (U8 inst);



////////////////////////////////////////////////////////////////////////////////
/// AlarmDelLink:  Delete a link and all alarms
///
 // Parameters:
/// \param inst     Link to delete
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void AlarmDelLink (U8 inst);


////////////////////////////////////////////////////////////////////////////////
/// AlarmGetCondition:  Is the given alarm currently raised?
///
 // Parameters:
/// \param cond     Alarm condition to query
/// \param inst     Instance of alarm condition
///
/// \return
/// Whether the given alarm is currently raised
////////////////////////////////////////////////////////////////////////////////
extern
BOOL AlarmGetCondition(AlmCondition cond, U8 inst);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Find the link an alarm should be reported on
///
/// \param cond     Alarm to report
/// \param inst     Instance of the alarm
///
/// \return
/// Index of link to report alarm on
////////////////////////////////////////////////////////////////////////////////
extern
LinkIndex GetLinkForAlarm(AlmCondition cond, U8 inst);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Find the instance where an alarm is from
///
/// \param cond     Alarm to report
/// \param inst     Instance modified by threshold alarm
///
/// \return
/// Orignal instance of the alarm
////////////////////////////////////////////////////////////////////////////////
extern
U8 GetInstForAlarm(AlmCondition cond, U8 inst);



////////////////////////////////////////////////////////////////////////////////
/// AlarmLinkFaultPresent:  determines if a link fault is present
///
/// Presence of "link fault" is declared in the flags field of all
/// OAM packets.  This routine provides that value based on all
/// system alarms.
///
 // Parameters:
/// \param link Link to query
///
/// \return
/// TRUE if link fault is present
////////////////////////////////////////////////////////////////////////////////
extern
BOOL AlarmLinkFaultPresent (LinkIndex link);


////////////////////////////////////////////////////////////////////////////////
/// AlarmDyingGasp:  determines if we're about to fail
///
/// Presence of "dying gasp" is declared in the flags field of all
/// OAM packets.  This routine provides that value based on all
/// system alarms.
///
 // Parameters:
/// \param None
///
/// \return
/// TRUE if the DG flag should be set
////////////////////////////////////////////////////////////////////////////////
extern
BOOL AlarmDyingGasp (LinkIndex link);



////////////////////////////////////////////////////////////////////////////////
/// AlarmCriticalEvent:  determines critical event is present
///
/// Presence of "critical event" is declared in the flags field of all
/// OAM packets.  This routine provides that value based on all
/// system alarms.
///
 // Parameters:
/// \param None
///
/// \return
/// TRUE if the CE flag should be set
////////////////////////////////////////////////////////////////////////////////
extern
BOOL AlarmCriticalEvent (LinkIndex link);

////////////////////////////////////////////////////////////////////////////////
/// \brief  Returns LLID statistic alarm condition
///
/// \param  stat    Statistic identifier of interest
/// \param  inst    Instance of the stats
///
/// \return Condition of the LLID stat
////////////////////////////////////////////////////////////////////////////////
extern
Bool AlarmLlidStatCondGet(StatId stat, U8 inst);

////////////////////////////////////////////////////////////////////////////////
/// AlarmLedPoll:  poll alarm led task to offer it an opportunity to run
///
 // Parameters:
/// None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void AlarmLedPoll (void);


////////////////////////////////////////////////////////////////////////////////
/// AlarmInit:  initialization for Alarms module
///
/// Detailed description
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void AlarmInit (void);

////////////////////////////////////////////////////////////////////////////////
/// OamAlarmSetEponThdVal:  Set OAM alarm for Epon
///
 // Parameters:
/// \param src  OAM container into which value is to be requested
/// \param cont OAM container into which value is to be returned
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern

void OamAlarmSetEponThdVal (const OamVarContainer BULK* src,
                            OamVarContainer BULK* cont);

////////////////////////////////////////////////////////////////////////////////
/// OamAlarmGetEponThdVal:  Get OAM alarm threshold for Epon
///
 // Parameters:
/// \param src  OAM container into which value is to be requested
/// \param cont OAM container into which value is to be returned
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OamAlarmGetEponThdVal (const OamVarContainer BULK* src,
                            OamVarContainer BULK* cont);

////////////////////////////////////////////////////////////////////////////////
/// AlarmThresholdNotifyTimer:  Call this function to check the alarm threshold and raise an alarm.
///
 // Parameters:
/// None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void AlarmThresholdNotifyTimer(void);

#if defined(__cplusplus)
}
#endif

#endif // Alarms.h

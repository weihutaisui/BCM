//****************************************************************************
//
// Copyright (c) 2007 Broadcom Corporation
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
//  Filename:       SystemTime.h
//  Author:         David Pullen
//  Creation Date:  July 17, 2000
//
//****************************************************************************

#ifndef SYSTEMTIME_H
#define SYSTEMTIME_H

//********************** Include Files ***************************************


#include <time.h>
#include "Counter64.h"

#if defined (__cplusplus)

#include "typedefs.h"


extern "C"
{
#endif

//********************** Global Types ****************************************

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Class Declaration ***********************************


/** \ingroup OsWrappers
*   \file SystemTime.h
*
*   This file provides an OS-agnostic way to retrieve the current system
*   time (in milliseconds) as an unsigned long value.  There is also a
*   function to query the timebase of the system, which tells you the
*   accuracy of time values (e.g. 1ms, 10ms, etc.).
*
*   There is a new API that allows you to set the system time.  There is
*   always some OS-specific code involved in this (no C runtime support,
*   unlike getting the time of day).
*/

/// This function returns the number of milliseconds that have elapsed since
/// the system started.  It can be used as a snapshot of the system clock for
/// calculating the number of milliseconds that have elapsed.
///
/// Of course, the accuracy of this value depends on the system timebase, which
/// can be read via the SystemTimebaseMS() function.
///
/// Note that the counter can wrap around to 0.
///
/// \return
///      The system millisecond counter.
///
unsigned long SystemTimeMS(void);

/// 64 bit version of SystemTimeMS.
void SystemTimeMS64 (Counter64 *pMS64);

/// This function allows a client to query the fundamental time base of this
/// particular system.  This corresponds to the frequency of the clock tick,
/// and has an impact on the accuracy of timeout values (they will be
/// quantized to this resolution).
///
/// In methods like Sleep(), when a timeout value is specified, if that value
/// is less than this, then it can either be truncated to 0 or ceiling'ed up
/// to this value.
///
/// Thus, for a system that has a 10ms tick, specifying a timeout value of
/// 11ms may be the same as specifying a 10ms timeout value.
///
/// \return
///      The number of milliseconds in each clock tick on the current system.
///
unsigned long SystemTimebaseMS(void);

// 'bool' type not cool w/ straight "C" so exclude these prototypes when 
// included from "C" files.
#if defined (__cplusplus)

/// This function sets the system time based on the time_t value that is passed
/// in.  The value usually comes from the result of a Time Of Day query to a
/// server, and MUST be in UTC/GMT calendar time format.  
///
/// IMPORTANT: UTC/GMT calendar time format accepted by this function is as 
/// returned by an RFC868 TOD query, which is number of seconds since 1/1/1900 
/// GMT, not 1/1/1970 as is the usual convention for time functions like time, 
/// localtime, ctime, etc.
///
/// \param
///      timeOfDay - the time of day to be set (seconds since 1/1/1900 GMT).
/// \param
///      utcOffset - the UTC time offset for the local time zone.
///
/// \retval
///      true if successful.
/// \retval
///      false if there was a problem.
///
bool SetTimeOfDay(time_t timeOfDay, int utcOffset, bool applyGmtOffset=true);

/// Set the system time based on a standard time_t which counts seconds since
/// 1/1/1970 GMT.  This sets the absolute current time of the system and does
/// not accept any arguments for UTC offset, daylight savings time, etc.  So
/// the following code would produce no system time change:
/// time_t Now;
/// time (&Now);
/// SetSystemTime (Now);
///
/// \param
///      systemTime - Current time in seconds since 1/1/1970
///
/// \retval
///      true if successful.
/// \retval
///      false if there was a problem.
///
#ifndef WIN32
bool SetSystemTime (time_t systemTime);
#endif

#endif

/// Get the number of seconds that the system clock was adjusted by the
/// last time SetTimeOfDay() was called.
///
/// \return
///      Difference between the old system time and the new system time since
///      the last time SetTimeOfDay() or SetSystemTime() was called.  Could be 
///      positive, negative, or zero.
///
int SystemTimeGetLastDelta(void);

/// Get the time_t value at which the system was started.  If the time is
/// changed via SetTimeOfDay, this value will be updated.
///
/// \return
///      Time that the system was started in time_t format (number of seconds
///      since Midight UTC, 1/1/1970)
time_t GetSystemStartTime(void);


//********************** Inline Method Implementations ***********************

#if defined (__cplusplus)
}
#endif

#endif



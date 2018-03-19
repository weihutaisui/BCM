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
//  Filename:       SystemTime.cpp
//  Author:         David Pullen
//  Creation Date:  July 17, 2000
//
//****************************************************************************
//  Description:
//      This file provides an OS-agnostic way to retrieving the current system
//      time (in milliseconds) as an unsigned long value.  There is also a
//      function to query the timebase of the system, which tells you the
//      accuracy of time values (e.g. 1ms, 10ms, etc.).
//
//****************************************************************************

//********************** Include Files ***************************************

// My api and definitions...
#include "SystemTime.h"

#include "MessageLog.h"

// OS specific stuff.
#if defined(WIN32)

#include <windows.h>

// For the system time services.  NOTE:  You must add winmm.lib to the list of
// link libraries for the project (alt-f7, "Link" tab, Category "General", 
// "Object/library modules" field).
#include <mmsystem.h>

#elif defined(PSOS)

#include "sys_conf.h"
#include <psos.h>

#elif defined(TARGETOS_vxWorks)

#include <vxWorks.h>
#include <tickLib.h>

#include "vxsys_conf.h"

//#elif defined(__QNX__)
#elif defined(TARGETOS_Qnx6)

#include <sys/neutrino.h>
#include <time.h>

#elif defined(__linux__)

extern "C"
{
    uint32 TimeMs(void);
    extern int bcmDrvFd;
}
#include <asm/param.h>
#include <time.h>

#elif defined(TARGETOS_eCos)

// This is needed for various eCos types, as well as the API prototypes.
#include <cyg/kernel/kapi.h>


#else

#error Unknown Operating System - unable to build SystemTime.cpp!

#endif

//********************** Local Types *****************************************

//********************** Local Constants *************************************

// In SetTimeOfDay, use following offset to convert from UTC (GMT) to calendar
// time.  The following is GMT for midnight Jan 1 1970, which is:
//     number of seconds from midnight Jan 1 1900 to midnight Jan 1 1970
//
// Calendar time starts at midnight Jan 1 1970, so subtract this value
// from the given UTC to get calendar time.
#define GMT_CAL_TIME_OFFSET	2208988800UL

//********************** Local Variables *************************************

static int fLastDelta = 0;

static time_t fSystemStartTime = 0;

#if defined(__linux__)
static bool fDoOnce = true;
#endif

//********************** Local Functions *************************************

static inline unsigned long Win32SystemTimeMS(void);
static inline unsigned long Win32SystemTimebaseMS(void);
static inline bool Win32SetTimeOfDay(time_t timeOfDay,bool applyGmtOffset);
static inline bool Win32SetSystemTime(time_t timeOfDay);

static inline unsigned long PsosSystemTimeMS(void);
static inline unsigned long PsosSystemTimebaseMS(void);
static inline bool PsosSetTimeOfDay(time_t timeOfDay,bool applyGmtOffset);
static inline bool PsosSetSystemTime(time_t timeOfDay);

static inline unsigned long VxSystemTimeMS(void);
static inline unsigned long VxSystemTimebaseMS(void);
static inline bool VxSetTimeOfDay(time_t timeOfDay,bool applyGmtOffset);
static inline bool VxSetSystemTime(time_t timeOfDay);

static inline unsigned long QnxSystemTimeMS(void);
static inline unsigned long QnxSystemTimebaseMS(void);
static inline bool QnxSetTimeOfDay(time_t timeOfDay,bool applyGmtOffset);
static inline bool QnxSetSystemTime(time_t timeOfDay);

static inline unsigned long LinuxSystemTimeMS(void);
static inline unsigned long LinuxSystemTimebaseMS(void);
static inline bool LinuxSetTimeOfDay(time_t timeOfDay,bool applyGmtOffset);
static inline bool LinuxSetSystemTime(time_t timeOfDay);

static inline unsigned long EcosSystemTimeMS(void);
static inline unsigned long EcosSystemTimebaseMS(void);
static inline bool EcosSetTimeOfDay(time_t timeOfDay,bool applyGmtOffset);
static inline bool EcosSetSystemTime(time_t timeOfDay);

//********************** Class Method Implementations ************************


// This function returns the number of milliseconds that have elapsed since
// the system started.  It can be used as a snapshot of the system clock for
// calculating the number of milliseconds that have elapsed.
//
// Of course, the accuracy of this value depends on the system timebase, which
// can be read via the SystemTimebaseMS() function.
//
// Note that the counter can wrap around to 0.
//
// Parameters:  None.
//
// Returns:
//      The system millisecond counter.
//
unsigned long SystemTimeMS(void)
{
    // Vector to the correct implementation.
    #if defined(WIN32)
    
        return Win32SystemTimeMS();

    #elif defined(PSOS)
    
        return PsosSystemTimeMS();

    #elif defined(TARGETOS_vxWorks)
    
        return VxSystemTimeMS();

    //#elif defined(__QNX__)
    #elif defined(TARGETOS_Qnx6)
    
        return QnxSystemTimeMS();

    #elif defined(__linux__)

        return LinuxSystemTimeMS();

    #elif defined(TARGETOS_eCos)

        return EcosSystemTimeMS();

    #endif
}

// The 32 bit millisecond counter will roll over every 0x100000000 mS, which
// is 4294967.296 seconds (49.71 days)
#define ROLLOVER_SECONDS 4294967.296

// Calculate how many times our 32 bit millisecond counter has rolled over,
// based on the value of the mS counter and the second counter.  The resulting
// 64 bit value will be the rollovers in the upper 32 bits and the current
// mS counter in the lower 32 bits.  This calculation is more complicated than 
// one might expect...
void Calculate64BitTime (time_t UpTimeSecs, unsigned long UpTimeMS, Counter64 *pMS64)
{
  unsigned long Rollovers;
  unsigned long RolloverWindowBegin;
  unsigned long RolloverWindowEnd;

  
  // Calculate how many times our 32 bit millisecond counter has rolled over
  // based on the value of the 32 bit second counter.  This part is easy.
  Rollovers = (unsigned long)(UpTimeSecs / ROLLOVER_SECONDS);

  // The difficult part of this is that the counter will generally roll over
  // in the middle of a 'second'.  We want our answer to always be accurate
  // to the millisecond so we need to handle this one-second window during
  // which the rollover occurs.  So figure out what that window is.  The
  // initial value for RolloverWindowBegin and RolloverWindowEnd will be
  // in units of seconds.
  
  RolloverWindowBegin = (unsigned long)((Rollovers + 1) * ROLLOVER_SECONDS);
  RolloverWindowEnd   = RolloverWindowBegin + 1;
  
  // If we are not within the one-second window then we are finished.  If we
  // are within the window, we have more work to do.
  if (UpTimeSecs == (time_t)RolloverWindowBegin)
  {
    // Now calculate the millisecond values for the beginning and end of the
    // rollover window.  Note that because we will be truncating the 'end'
    // value to 32 bits, we will always have a large value for the 'begin'
    // and a small value for the 'end'.  For example, the first rollover
    // occurs between 4294967 and 4294968 seconds, which in milliseconds is
    // 0FFFFFED8 ... 1000002BF.  But since we are truncating to 32 bits,
    // this gives begin = FFFFFED8 and end = 000002BF.
    RolloverWindowBegin *= 1000;
    RolloverWindowEnd   *= 1000;
    RolloverWindowEnd   -= 1; // Inclusive upper boundary
    
    // Are we within the mS rollover window?  Note the special case check for
    // the rollover beginning at 0 (this specifically happens on the 375th
    // rollover).
    if (RolloverWindowBegin != 0)
    {
      if ((UpTimeMS >= RolloverWindowBegin) || (UpTimeMS <= RolloverWindowEnd))
      {
        // If the current time is <= the end of the window then we need to
        // increment the rollover.  In our example, this means between 0 and 2BF.
        if (UpTimeMS <= RolloverWindowEnd)
          Rollovers++;
      }
    }
  }
  
  // Load the 64 bit value into the provided counter.
  if (pMS64)
  {
    pMS64->Hi = (unsigned long)Rollovers;
    pMS64->Lo = UpTimeMS;
  }
}


/// 64 bit version of SystemTimeMS.
void SystemTimeMS64 (Counter64 *pMS64)
{
  unsigned long UpTimeMS = SystemTimeMS();
  time_t UpTimeSecs;
  
  
  // This calculation will yield the number of seconds we have been up.
  time (&UpTimeSecs);
  UpTimeSecs -= GetSystemStartTime();
  
  Calculate64BitTime (UpTimeSecs, UpTimeMS, pMS64);
}


// This function allows a client to query the fundamental time base of this
// particular system.  This corresponds to the frequency of the clock tick,
// and has an impact on the accuracy of timeout values (they will be
// quantized to this resolution).
//
// In methods like Sleep(), when a timeout value is specified, if that value
// is less than this, then it can either be truncated to 0 or ceiling'ed up
// to this value.
//
// Thus, for a system that has a 10ms tick, specifying a timeout value of
// 11ms may be the same as specifying a 10ms timeout value.
//
// Parameters:  None.
//
// Returns:
//      The number of milliseconds in each clock tick on the current system.
//
unsigned long SystemTimebaseMS(void)
{
    // Vector to the correct implementation.
    #if defined(WIN32)
    
        return Win32SystemTimebaseMS();

    #elif defined(PSOS)
    
        return PsosSystemTimebaseMS();

    #elif defined(TARGETOS_vxWorks)
    
        return VxSystemTimebaseMS();

    //#elif defined(__QNX__)
    #elif defined(TARGETOS_Qnx6)
    
        return QnxSystemTimebaseMS();

    #elif defined (__linux__)

        return LinuxSystemTimebaseMS();

    #elif defined (TARGETOS_eCos)

        return EcosSystemTimebaseMS();

    #endif
}


// This function sets the system time based on the time_t value that is passed
// in.  The value usually comes from the result of a Time Of Day query to a
// server, and MUST be in UTC/GMT calendar time format.
//
// Parameters:
//      timeOfDay - the time of day to be set.
//      utcOffset - the UTC time offset for the local time zone.
//
// Returns:
//      true if successful.
//      false if there was a problem.
//
bool SetTimeOfDay(time_t timeOfDay, int utcOffset, bool applyGmtOffset)
{
	bool statusVal;

    time_t currentTime;

    // Store the current time before we change it.  We need to keep track of the
    // changes in time of day.
    time(&currentTime);

    // Adjust the time specified for local time via the offset.
    timeOfDay += utcOffset;

    // Vector to the correct implementation.
    #if defined(WIN32)
    
        statusVal = Win32SetTimeOfDay(timeOfDay,applyGmtOffset);

    #elif defined(PSOS)
    
        statusVal = PsosSetTimeOfDay(timeOfDay,applyGmtOffset);

    #elif defined(TARGETOS_vxWorks)
    
		statusVal =  VxSetTimeOfDay(timeOfDay,applyGmtOffset);

    //#elif defined(__QNX__)
    #elif defined(TARGETOS_Qnx6)
    
		statusVal =  QnxSetTimeOfDay(timeOfDay,applyGmtOffset);

    #elif defined(__linux__)
    
		statusVal =  LinuxSetTimeOfDay(timeOfDay,applyGmtOffset);

    #elif defined(TARGETOS_eCos)
    
		statusVal =  EcosSetTimeOfDay(timeOfDay,applyGmtOffset);

    #endif
	
	// Update our current calendar time value.
	time(&timeOfDay);

	// Store the amount by which we changed the time.
	fLastDelta = timeOfDay - currentTime;
    
    // Adjust system start time.
#if defined(__linux__)
    if(fDoOnce)
	{
		fSystemStartTime = timeOfDay - (LinuxSystemTimeMS() / 1000);
		fDoOnce = false;
	}
	else
#endif
    fSystemStartTime += fLastDelta;
    
    // Display the new time in ASCII.
	gLogMessageRaw << "Current system time -> " << ctime(&timeOfDay) << endl;
    
    // Display the new time in ASCII.
	gLogMessageRaw << "System start time -> " << ctime(&fSystemStartTime) << endl;

	return statusVal;
}


// Set the system time based on a standard time_t which counts seconds since
// 1/1/1970 GMT.  This sets the absolute current time of the system and does
// not accept any arguments for UTC offset, daylight savings time, etc.  So
// the following code would produce no system time change:
// time_t Now;
// time (&Now);
// SetSystemTime (Now);
//
// Parameters:
//      systemTime - Current time in seconds since 1/1/1970
//
// Returns:
//      true if successful.
//      false if there was a problem.
//
#ifndef WIN32
bool SetSystemTime (time_t systemTime)
{
	bool statusVal;

    time_t currentTime;

    // Store the current time before we change it.  We need to keep track of the
    // changes in time of day.
    time(&currentTime);

    // Vector to the correct implementation.
    #if defined(WIN32)
    
        statusVal = Win32SetSystemTime(systemTime);

    #elif defined(PSOS)
    
        statusVal = PsosSetSystemTime(systemTime);

    #elif defined(TARGETOS_vxWorks)
    
		statusVal =  VxSetSystemTime(systemTime);

    //#elif defined(__QNX__)
    #elif defined(TARGETOS_Qnx6)
    
		statusVal =  QnxSetSystemTime(systemTime);

    #elif defined(__linux__)
    
		statusVal =  LinuxSetSystemTime(systemTime);

	#elif defined(TARGETOS_eCos)
    
		statusVal =  EcosSetSystemTime(systemTime);

    #endif
	
	// Update our current system time value.
	time(&systemTime);

	// Display the new time in ASCII.
	gLogMessageRaw << "Current system time -> " << ctime(&systemTime) << endl;

	// Store the amount by which we changed the time.
	fLastDelta = systemTime - currentTime;
    
    // Adjust system start time.
    fSystemStartTime += fLastDelta;
    
    // Display the new time in ASCII.
	gLogMessageRaw << "System start time -> " << ctime(&fSystemStartTime) << endl;

	return statusVal;
}
#endif

// Get the number of seconds that the system clock was adjusted by the
// last time SetTimeOfDay() or SetSystemTime() was called.
//
// Parameters:  None.
//
// Returns:
//      Difference between the old system time and the new system time since
//      the last time SetTimeOfDay() or SetSystemTime() was called.  Could be 
//      positive, negative, or zero.
//
int SystemTimeGetLastDelta (void)
{
    return fLastDelta;
}


time_t GetSystemStartTime(void) 
{
	return fSystemStartTime;
}


//============================================================================


// This is the Win32-specific implementation for the system time.
//
// Parameters:  None.
//
// Returns:
//      The system millisecond counter.
//
static inline unsigned long Win32SystemTimeMS(void)
{
    unsigned long currentTime = 0;

    #if defined(WIN32)
    
        currentTime = (unsigned long) timeGetTime();

    #endif

    return currentTime;
}


// This is the Win32-specific implementation for the system timebase.
//
// Parameters:  None.
//
// Returns:
//      The number of milliseconds in each clock tick on the current system.
//
static inline unsigned long Win32SystemTimebaseMS(void)
{
    // This is true for Win95/98, but may not be true for WinNT.
    // Need to query this for the current system.
    //
    // There doesn't appear to be a way to query the current tick
    // resolution, though you can set it.  Go figure...
    return 1;
}


// This is the Win32-specific implementation for setting the time of day.  It
// just simulates operation, since we really don't want to muck with the real-
// time clock.
//
// Parameters:
//      timeOfDay - the time of day to be set.
//
// Returns:
//      true if successful.
//      false if there was a problem.
//
static inline bool Win32SetTimeOfDay(time_t /*timeOfDay*/, bool /*applyGmtOffset*/)
{
    return true;
}

// This function is identical to QnxSetTimeOfDay except that it does not
// compensate for the TOD timebase of 1/1/1900, it uses the standard 1/1/1970
static inline bool Win32SetSystemTime(time_t /*timeOfDay*/)
{
    return true;
}


//============================================================================


// This is the pSOS-specific implementation for the system time.
//
// Parameters:  None.
//
// Returns:
//      The system millisecond counter.
//
static inline unsigned long PsosSystemTimeMS(void)
{
    unsigned long currentTime = 0;

    #if defined(PSOS)
    
        unsigned long ticksHi, ticksLo;

        // I need to get the current system tick counter, then convert to ms.
        // This has time as a 64-bit number.  Do I want to try to preserve this?
        // now, I'll just use the lower 32 bits.
        tm_getticks(&ticksHi, &ticksLo);
    
		  // NOTE:  We do a calculation of the currentTime (which is in Milliseconds). 
		  //  We will only get a granularity of whatever the system tick time is.  
		  //  In our current case the system Ticks are 10Ms so the currentTime will go
		  //  in steps of 10Ms.  There is a potential problem with the following line of
		  //  code if the KC_TICKS2SEC ever gets about 1000.  I don't think that this will ever
		  //  happen though.
        currentTime = (ticksLo * (1000 / KC_TICKS2SEC));
    
    #endif

    return currentTime;
}


// This is the pSOS-specific implementation for the system timebase.
//
// Parameters:  None.
//
// Returns:
//      The number of milliseconds in each clock tick on the current system.
//
static inline unsigned long PsosSystemTimebaseMS(void)
{
    #if defined(PSOS)
    
        // This value is set in KC_TICKS2SEC, and needs to be converted to ms.
        return (1000 / KC_TICKS2SEC);

    #else
        
        return 0;

    #endif
}


// This is the pSOS-specific implementation for setting the time of day.
//
// Parameters:
//      timeOfDay - the time of day to be set.
//
// Returns:
//      true if successful.
//      false if there was a problem.
//
static inline bool PsosSetTimeOfDay(time_t timeOfDay,bool applyGmtOffset)
{
    #if defined(PSOS)
    
        struct tm localTime;
        unsigned long rc, pSOSdate, pSOStime, pSOSticks;

       
        // Adjust from UTC (GMT) to calendar time.
        if (applyGmtOffset == true)
        {
            timeOfDay -= GMT_CAL_TIME_OFFSET;
        }

        // Break out the calendar time.
        localtime_r(&timeOfDay, &localTime);

        // Reformat the calendar time to match what pSOS needs.
        pSOSdate = (((localTime.tm_year + 1900) & 0xffff) << 16) |
                   (((localTime.tm_mon + 1) & 0xff) << 8) |
                   (localTime.tm_mday & 0xff);

        pSOStime = ((localTime.tm_hour & 0xff) << 16) |
                   ((localTime.tm_min & 0xff) << 8) |
                   (localTime.tm_sec & 0xff);

        // We don't need any precision here, so just set it to 0.
        pSOSticks = 0;
    
        // Set the pSOS system time based on the reformatted calendar time.
        rc = tm_set(pSOSdate, pSOStime, pSOSticks);

        if (rc != 0)
        {
            return false;
        }

        return true;

    #else
    
        return false;

    #endif
}

// This function is identical to PsosSetTimeOfDay except that it does not
// compensate for the TOD timebase of 1/1/1900, it uses the standard 1/1/1970
static inline bool PsosSetSystemTime(time_t timeOfDay)
{
    #if defined(PSOS)
    
        struct tm localTime;
        unsigned long rc, pSOSdate, pSOStime, pSOSticks;

        // Break out the calendar time.
        localtime_r(&timeOfDay, &localTime);

        // Reformat the calendar time to match what pSOS needs.
        pSOSdate = (((localTime.tm_year + 1900) & 0xffff) << 16) |
                   (((localTime.tm_mon + 1) & 0xff) << 8) |
                   (localTime.tm_mday & 0xff);

        pSOStime = ((localTime.tm_hour & 0xff) << 16) |
                   ((localTime.tm_min & 0xff) << 8) |
                   (localTime.tm_sec & 0xff);

        // We don't need any precision here, so just set it to 0.
        pSOSticks = 0;
    
        // Set the pSOS system time based on the reformatted calendar time.
        rc = tm_set(pSOSdate, pSOStime, pSOSticks);

        if (rc != 0)
        {
            return false;
        }

        return true;

    #else
    
        return false;

    #endif
}


//============================================================================


// This is the VxWorks-specific implementation for the system time.
//
// Parameters:  None.
//
// Returns:
//      The system millisecond counter.
//
static inline unsigned long VxSystemTimeMS(void)
{
    unsigned long currentTime = 0;

    #if defined(TARGETOS_vxWorks)
    
        // I need to get the current system tick counter, then convert to ms.
        currentTime = tickGet();
          
		  // NOTE:  We do a calculation of the currentTime (which is in Milliseconds). 
		  //  We will only get a granularity of whatever the system tick time is.  
		  //  In our current case the system Ticks are 10Ms so the currentTime will go
		  //  in steps of 10Ms.  There is a potential problem with the following line of
		  //  code if the KC_TICKS2SEC ever gets about 1000.  I don't think that this will ever
		  //  happen though.
        currentTime = (currentTime * (1000 / KC_TICKS2SEC));

    #endif

    return currentTime;
}


// This is the VxWorks-specific implementation for the system timebase.
//
// Parameters:  None.
//
// Returns:
//      The number of milliseconds in each clock tick on the current system.
//
static inline unsigned long VxSystemTimebaseMS(void)
{
    #if defined(TARGETOS_vxWorks)
    
        // This value is set in KC_TICKS2SEC, and needs to be converted to ms.
        return (1000 / KC_TICKS2SEC);

    #else
        
        return 0;

    #endif
}


// This is the VxWorks-specific implementation for setting the time of day.
//
// Parameters:
//      timeOfDay - the time of day to be set.
//
// Returns:
//      true if successful.
//      false if there was a problem.
//
static inline bool VxSetTimeOfDay(time_t timeOfDay,bool applyGmtOffset)
{
    #if defined(TARGETOS_vxWorks)
    
        struct timespec l_TimeSpec;

        // Adjust from UTC (GMT) to calendar time.
        if (applyGmtOffset == true)
        {
            timeOfDay -= GMT_CAL_TIME_OFFSET;
        }

        l_TimeSpec.tv_sec  = timeOfDay;          
        l_TimeSpec.tv_nsec = 0;

        // JRM - It seems that if we set the system time once we already have a
        // DHCP lease that the setting of the time screws up the tDhcpTask. A
        // TSR with WindRiver has been opened to address this issue.
        #if 1
            /* Set up vxWorks clock */
            if (clock_settime(CLOCK_REALTIME, &l_TimeSpec) == ERROR)
            {
                return false;
            }
        #endif

        return true;

    #else
    
        return false;

    #endif
}

// This function is identical to VxSetTimeOfDay except that it does not
// compensate for the TOD timebase of 1/1/1900, it uses the standard 1/1/1970
static inline bool VxSetSystemTime(time_t timeOfDay)
{
    #if defined(TARGETOS_vxWorks)
    
        struct timespec l_TimeSpec;

        l_TimeSpec.tv_sec  = timeOfDay;          
        l_TimeSpec.tv_nsec = 0;

        // JRM - It seems that if we set the system time once we already have a
        // DHCP lease that the setting of the time screws up the tDhcpTask. A
        // TSR with WindRiver has been opened to address this issue.
        #if 1
            /* Set up vxWorks clock */
            if (clock_settime(CLOCK_REALTIME, &l_TimeSpec) == ERROR)
            {
                return false;
            }
        #endif

        return true;

    #else
    
        return false;

    #endif
}


//============================================================================

//#if defined(__QNX__)
//extern unsigned int tm_now();  
//#endif

// This is the QNX-specific implementation for the system time.
//
// Parameters:  None.
//
// Returns:
//      The system millisecond counter.
//
static inline unsigned long QnxSystemTimeMS(void)
{
    unsigned long currentTime = 0;

    //#if defined(__QNX__)
    #if defined(TARGETOS_Qnx6)
    
    unsigned long long  sysTimeNanoSecs;

    // I need to get the current system tick counter, then convert to ms.
    // the system maintains time in nanoseconds 
    ClockTime(CLOCK_REALTIME, NULL, &sysTimeNanoSecs);

    currentTime = (unsigned long)(sysTimeNanoSecs/1000000);
    #endif

    return currentTime;
}


// This is the QNX-specific implementation for the system timebase.
//
// Parameters:  None.
//
// Returns:
//      The number of milliseconds in each clock tick on the current system.
//
static inline unsigned long QnxSystemTimebaseMS(void)
{
    unsigned long msecTick=0;

#if defined(TARGETOS_Qnx6)

    struct _clockperiod clk;

    // Get the clockperiod msec in each clock tick (Qnx provides this in nsec, so convert to msec)
    // the equation is:
    //      msec(tick) = ClockPeriod(nanosec) * 1 (msec) / 1000000 (nanosec)
    //      round up 1 if  ClockPeriod(nanosec) % 1000000 > 0
    //
    ClockPeriod( CLOCK_REALTIME, NULL, &clk,0);

    msecTick = clk.nsec/1000000;
    if ((clk.nsec % 1000000) > 0)
        msecTick++;
#endif

    return msecTick;
}


static inline bool QnxSetTimeOfDay(time_t timeOfDay, bool applyGmtOffset)
{

#if defined(TARGETOS_Qnx6)

    // Adjust from UTC (GMT) to calendar time.
    if (applyGmtOffset)
    {
        timeOfDay -= GMT_CAL_TIME_OFFSET;
    } 

    struct timespec timeSpec;

    timeSpec.tv_sec = timeOfDay;
    timeSpec.tv_nsec = 0;
    clock_settime(CLOCK_REALTIME, &timeSpec);

#endif

    return true;
}

// This function is identical to QnxSetTimeOfDay except that it does not
// compensate for the TOD timebase of 1/1/1900, it uses the standard 1/1/1970
static inline bool QnxSetSystemTime(time_t timeOfDay)
{

#if defined(TARGETOS_Qnx6)

    struct timespec timeSpec;

    timeSpec.tv_sec = timeOfDay;
    timeSpec.tv_nsec = 0;
    clock_settime(CLOCK_REALTIME, &timeSpec);

#endif

    return true;
}


// This is the Linux-specific implementation for the system time.
//
// Parameters:  None.
//
// Returns:
//      The system millisecond counter.
//
static inline unsigned long LinuxSystemTimeMS(void)
{
    unsigned long currentTime = 0;

    #if defined(__linux__)

    currentTime = TimeMs();

   #if (BCM_SETTOP_SUPPORT)
    // Check to see if we were able to get time from driver.
    // If not, get it using realtime.  This would only occur
    // during testing, and the driver is not loaded.
    if (bcmDrvFd == -1)
    {
        timespec nowtime;
// TPS - TODO clock_gettime isn't in uClibc - must make replacement.
//        clock_gettime(CLOCK_REALTIME, &nowtime);
        currentTime = nowtime.tv_sec * 1000;
        currentTime += nowtime.tv_nsec/1000000;
    }
   #endif

    #endif

    return currentTime;
}


// This is the Linux-specific implementation for the system timebase.
//
// Parameters:  None.
//
// Returns:
//      The number of milliseconds in each clock tick on the current system.
//
static inline unsigned long LinuxSystemTimebaseMS(void)
{
    #if defined(__linux__)
    
    // CLK_TCK is the number of ticks / second
    // so (CLK_TCK / 1000) is the number of ticks / millisecond
    // so 1000 / CLK_TCK is the number of ms/tick
    // return (1000 / CLK_TCK);
    // CLK_TCK seems to be undefined in new kernel, so...
    // ticks = (msecs * HZ) / 1000
    // msecs/tick = 1000 / HZ

    return (1000 / HZ);

    #else
        
        return 0;

    #endif
}

// This function sets the Linux system time, compensating for 
// TOD timebase of 1/1/1900. (Linux uses the standard 1/1/1970)
static inline bool LinuxSetTimeOfDay(time_t timeOfDay, bool applyGmtOffset)
{
    #if defined(__linux__)

        // Adjust from UTC (GMT) to calendar time.
        if (applyGmtOffset)
        {
            timeOfDay -= GMT_CAL_TIME_OFFSET;
        } 
    
		if ((stime(&timeOfDay)) == 0)
            return true;
    #endif
   
        return false;
}

// This function is identical to LinuxSetTimeOfDay except that it does not
// compensate for the TOD timebase of 1/1/1900, it uses the standard 1/1/1970
static inline bool LinuxSetSystemTime(time_t timeOfDay)
{
    #if defined(__linux__)
    
    if ((stime(&timeOfDay)) == 0)
        return true;

    #endif
    
    return false;
}


//============================================================================


// This is the eCos-specific implementation for the system time.
//
// Parameters:  None.
//
// Returns:
//      The system millisecond counter.
//
static inline unsigned long EcosSystemTimeMS(void)
{
    unsigned long curr_time_millisec = 0;

    #if defined(TARGETOS_eCos)
        
	    // get current time in ticks.
        cyg_tick_count_t  curr_time_ticks = cyg_current_time();

		// convert ticks to millisecond units.
		curr_time_millisec = (unsigned long)( curr_time_ticks * EcosSystemTimebaseMS() );

        if( curr_time_ticks == 0 )
        {
        	gLogMessageRaw << "\ncyg_current_time() returned 0" << endl;
        }
        else if( curr_time_millisec == 0 )
        {
        	gLogMessageRaw << "\nEcosSystemTimebaseMS() returned 0" << endl;
        }
        

    #endif

    return curr_time_millisec;
}


// This is the eCos-specific implementation for the system timebase.
//
// Parameters:  None.
//
// Returns:
//      The number of milliseconds in each clock tick on the current system.
//
static inline unsigned long EcosSystemTimebaseMS(void)
{
    #if defined(TARGETOS_eCos)
    
        // This value is set in KC_TICKS2SEC, and needs to be converted to ms.
//        return (1000 / KC_TICKS2SEC);
	  	return 10;
    #else
        
        return 0;

    #endif
}


// This is the eCos-specific implementation for setting the time of day.
//
// Parameters:
//      timeOfDay - the time of day to be set.
//
// Returns:
//      true if successful.
//      false if there was a problem.
//
static inline bool EcosSetTimeOfDay(time_t timeOfDay,bool applyGmtOffset)
{
    #if defined(TARGETOS_eCos)
    
        // Adjust from UTC (GMT) to calendar time.
        if (applyGmtOffset)
        {
            timeOfDay -= GMT_CAL_TIME_OFFSET;
        } 
    
        // Set the pSOS system time based on the reformatted calendar time.
        // Note we must translate eCos return values here.
        int eCosRet = cyg_libc_time_settime( timeOfDay );
        
        if (eCosRet == 0)
          return true;
        else
          return false;

    #else
    
        return false;

    #endif
}

// This function is identical to EcosSetTimeOfDay except that it does not
// compensate for the TOD timebase of 1/1/1900, it uses the standard 1/1/1970
static inline bool EcosSetSystemTime(time_t timeOfDay)
{
    #if defined(TARGETOS_eCos)
    
        // Set the pSOS system time based on the reformatted calendar time.
        // Note we must translate eCos return values here.
        int eCosRet = cyg_libc_time_settime( timeOfDay );
        
        if (eCosRet == 0)
          return true;
        else
          return false;

    #else
    
        return false;

    #endif
}



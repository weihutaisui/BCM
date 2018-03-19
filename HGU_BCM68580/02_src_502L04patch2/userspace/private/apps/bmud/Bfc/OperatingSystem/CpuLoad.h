//****************************************************************************
//
// Copyright (c) 2009 Broadcom Corporation
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
//  Filename:       CpuLoad.h
//  Author:         Kevin O'Neal
//  Creation Date:  August 20, 2009
//
//****************************************************************************
//  Description:
//      This file provides C callable APIs for estimating CPU utilization.
//
//****************************************************************************

#ifndef CPULOAD_H
#define CPULOAD_H

#define CPULOAD_SUPPORT  1

/*

Depending on whether CPU loading is supported, these will either be inline
functions which all just return -1, or macros defined to call OS-specific
implementation functions.  Here's what they do.

/// This function returns the CPU utilization estimated over the number of
/// milliseconds specified by the argument.  The function will block for the
/// length of time requested, then return the estimated CPU load as a 
/// percentage.
///
/// The function will return -1 if CPU utilization is not supported or
/// can't be performed for some reason.
///
/// \param
///      PeriodMsec - Number of milliseconds over which to perform the estimate
///
/// \return
///      CPU load estimate as a percentage; -1 if there is an error
///
int CpuloadMeasure (unsigned long PeriodMsec);

/// This function begins a CPU load estimate measurement.  The results of the
/// measurement can subsequently be retrieved by calling CpuloadGet().
///
/// \return
///      0 if the estimate starts successfully
///     -1 if the estimate fails to start (ie estimate already in progress or
///        CPU utilization not supported)
///
int CpuloadStart ();

/// This function returns the CPU utilization estimated since a previous call
/// to CpuloadStart was made.
///
/// The function will return -1 if CPU utilization is not supported or
/// can't be performed for some reason.
///
/// \param
///      pMilliseconds - pointer to a location to return the number of
///         milliseconds since the estimate began.  This may be called with
///         a NULL value if the caller doesn't care about the duration.
///
/// \return
///      CPU load estimate as a percentage; -1 if there is an error (ie 
///      estimate not started or CPU utilization not supported)
///
int CpuloadGet (unsigned long *pMilliseconds);

*/

#if defined (__cplusplus)
extern "C" {
#endif

#if defined(TARGETOS_eCos) && CPULOAD_SUPPORT

  // These are in ecosBfcApplication.cpp
  int eCosCpuloadMeasure (unsigned long PeriodMsec);
  int eCosCpuloadStart ();
  int eCosCpuloadGet (unsigned long *pMilliseconds);
  void eCosCpuloadTick ();
  
  #define CpuLoadMeasure(Milliseconds) eCosCpuloadMeasure(Milliseconds)
  #define CpuLoadStart eCosCpuloadStart
  #define CpuLoadGet(pMilliseconds) eCosCpuloadGet(pMilliseconds)
  #define CpuLoadTick() eCosCpuloadTick()

#else

  // For other OS's, CPU utilization estimation is not supported.
  inline int CpuLoadMeasure(unsigned long milliseconds) { return -1; }
  inline int CpuLoadStart () { return -1; }
  inline int CpuLoadGet (unsigned long *pMilliseconds) { if (pMilliseconds) *pMilliseconds = 0; return -1; }
  inline void CpuLoadTick () { }

#endif

#if defined (__cplusplus)
}
#endif

#endif

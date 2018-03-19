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
 *:>
 */
#if !defined(Timer_h)
#define Timer_h
////////////////////////////////////////////////////////////////////////////////
/// \file Timer.h
/// \brief Routines to manipulate a timer
///
/// This is the base layer driver, now the Polled timer use the Arc timer 0, TK Os timer use the
/// Arc timer 1, the DelayMs() use the IC coarse timer, the IC timer 0 and IC timer 1 is left
/// unused and should add base layer interface here for future requirements.
///
////////////////////////////////////////////////////////////////////////////////

#include "Platform.h"

#if defined(__cplusplus)
extern "C" {
#endif


////////////////////////////////////////////////////////////////////////////////
/// StartPolledTimer:  Set timer to overflow in the given number of ticks
///
 // Parameters:
/// \param ticks    Number of ticks for which to wait
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void StartPolledTimer(U32 ticks);


////////////////////////////////////////////////////////////////////////////////
/// StopPolledTimer:  Stops the polled timer
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void StopPolledTimer(void);


////////////////////////////////////////////////////////////////////////////////
/// PolledTimerExpired:  TRUE when timer has run its course
///
/// Again, a macro implementation as it just tests an SFR bit
///
/// \param None
///
/// \return
/// TRUE if PolledTimer has expired, else FALSE
////////////////////////////////////////////////////////////////////////////////
extern
BOOL PolledTimerExpired(void);


////////////////////////////////////////////////////////////////////////////////
/// InitPolledTimer:  Set up Polled timer for 16-bit one-shot incrementing mode
///
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void InitPolledTimer(void);


////////////////////////////////////////////////////////////////////////////////
/// DelayMs:  spin specified number of milliseconds
///
/// \param msec     Time to spin
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void DelayMs(U16 msec);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Wait at least specified number of nanoseconds
///
/// \param nsec     Number of nanoseconds to wait
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void DelayNs(U32 nsec);


////////////////////////////////////////////////////////////////////////////////
/// InitTkOsTimer:  Set up Tk Os timer for 16-bit one-shot incrementing mode
///
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void InitTkOsTimer(void);


////////////////////////////////////////////////////////////////////////////////
/// StartTkOsTimer:  Set Tk Os timer to overflow in the given number of ticks
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void StartTkOsTimer(U32 ticks);


#if defined(__cplusplus)
}
#endif

#endif // Timer.h


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


#if !defined(TkOsTypes_h)
#define TkOsTypes_h
////////////////////////////////////////////////////////////////////////////////
/// \file TkOsTypes.h
/// \brief Defines types for TkOs.h
///
/// This file contains platform-specific type definitions for the overhead
/// necessary to implement the functions declared in TkOs.h
///
////////////////////////////////////////////////////////////////////////////////

#include "Teknovus.h"

#if defined(__cplusplus)
extern "C" {
#endif


////////////////////////////////////////////////////////////////////////////////
// System
////////////////////////////////////////////////////////////////////////////////

typedef U32 TkOsTick;

////////////////////////////////////////////////////////////////////////////////
// Variable length ("Byte") allocation
////////////////////////////////////////////////////////////////////////////////

typedef struct TkOsBytePoolBlockHdr_tag  * ByteBlockPtr;
typedef struct
    {
    ByteBlockPtr free;
    } PACK TkOsBytePoolOverhead;

typedef TkOsBytePoolOverhead  * TkOsBytePool;


////////////////////////////////////////////////////////////////////////////////
// Fixed length ("Block") allocation
////////////////////////////////////////////////////////////////////////////////

typedef struct TkOsBlockPoolBlockHdr_tag
    {
    struct TkOsBlockPoolBlockHdr_tag  * next;
    } PACK TkOsBlockPoolBlockHdr;

typedef struct
    {
    TkOsBlockPoolBlockHdr  * free;
    } PACK TkOsBlockPoolOverhead;

typedef TkOsBlockPoolOverhead  * TkOsBlockPool;


////////////////////////////////////////////////////////////////////////////////
// Message Queues
////////////////////////////////////////////////////////////////////////////////

typedef struct
    {
    U8 msgCount;
    U8 maxMsgs;
    U8 head;
    U8 tail;
    } PACK TkOsMsgQOverhead;

typedef TkOsMsgQOverhead  * TkOsMsgQ;

////////////////////////////////////////////////////////////////////////////////
// Threads
////////////////////////////////////////////////////////////////////////////////

typedef struct
    {
    U8 threadId;
    } PACK TkOsThreadOverhead;


////////////////////////////////////////////////////////////////////////////////
// Timers
////////////////////////////////////////////////////////////////////////////////

/* userspace timer, 1 tick means 1 ms */
#define TkOsMsPerTick       1
#define TkOsMsToTicks(ms)   ((ms) / TkOsMsPerTick)
#define TicksPer10Ms        (10 / TkOsMsPerTick)

/// Maximum time, in system ticks, of a TkOsTimer
typedef TkOsTick TkOsTimerInterval;

typedef U16 TkOsTimerActionParam;

/// Action taken when timer expires
typedef void (CODE* TkOsTimerAction) (TkOsTimerActionParam param);


typedef struct TkOsTimerOverhead_tag
    {
    struct TkOsTimerOverhead_tag  * next;
    TkOsTick expiryTime;
    TkOsTimerInterval interval;
    TkOsTimerAction action;
    TkOsTimerActionParam param;
    BOOL repeat;
    int fd;
    } PACK TkOsTimerOverhead;

typedef TkOsTimerOverhead  * TkOsTimer;

#define TkOsNumTimers   32


typedef U8 TkOsMutex;



#if defined(__cplusplus)
}
#endif

#endif // TkOsTypes.h

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
//**************************************************************************
//    Filename: bcmos.h
//    Author:   Dannie Gay
//    Creation Date: 21-feb-99
//
//**************************************************************************
//    Description:
//
//	Header file for os specific definitions/includes
//
//**************************************************************************
//    Revision History:
//
//		0.1  initial revision
//
//**************************************************************************

#ifndef bcmos_h
#define bcmos_h

#if __cplusplus
extern "C" {
#endif

#ifndef __KERNEL__
#include <stdarg.h>// ptmod
#include <string.h>// ptmod
#include <stdio.h> // ptmod
#include <errno.h>
#include <malloc.h>
#endif

//linux include stuff:.....
#include "types.h"
#include "zone.h"
#include "bcmtypes.h"
//#include "cblmodem.h"
//#include "bcm_map.h"
//#include "timerUtils.h"
#include "math.h"
#include "limits.h"
#include <unistd.h>
#include <netinet/in.h>

#ifdef __KERNEL__
#include <asm/io.h>
#include <linux/slab.h>
#include <linux/mm.h>

#define log10 	bcmlog10	//function in bcmmath.c
#define sqrt	bcmsqrt		//function in bcmmath.c
#define calloc( count, size )	kmalloc( (count * size), GFP_KERNEL)
#define printf printk

extern double bcmlog10(double x);
extern double bcmsqrt(double x);
void bootPrint( char *pChr );
extern int bcm_vsprintf(char *buf, const char *fmt, va_list args);
#define LOCAL static
#endif

#define	EBADS	0x5009		//in EventLog.cpp

#define HAL_THREAD_PRIORITY_TABLE_LENGTH  8
#define HAL_THREAD_PRIORITY   230
#define HAL_THREAD_STACK_SIZE 8192
#define HAL_THREAD_TX_STACK_SIZE 8192
#define HAL_THREAD_RX_STACK_SIZE 8192

// max sizes for quick look up tables
#define MAX_MUTEX_NUM     100
#define MAX_SEMAPHORE_NUM 100
#define MAX_THREAD_NUM    32
#define MAX_EVENT_NUM     32

//klmod_linux: various console output defines
#ifndef __KERNEL__
#define HalPrint(s...)              printf(s)
#else
#define HalPrint(s...)              HALMSG(s)
#endif

#if (BCM_DEBUG_MSG)
#define DEBUGMSG(zn, s...) printf(s)
#else
#define DEBUGMSG(zn, s...)
#endif

#if (BCM_REDUCED_IMAGE_SIZE)

#define LogRawBuffer(s, p, b, w) 
#define HalDebugPrint(zn, s...)     
#define HalDebugPrintCrit(zn, s...)  DEBUGMSG(zn, s )          
#define MacPrint(x...)	            

#else

void LogRawBuffer (char * str, uint8 *pBuffer, int BufLen, BOOL wacky);
#define HalDebugPrint(zn, s...)      DEBUGMSG(zn, s )
#define HalDebugPrintCrit(zn, s...)  DEBUGMSG(zn, s )     
#ifndef __KERNEL__
#define MacPrint(x...)	             printf(x)
#else
#define MacPrint(x...)	             printk(x)
#endif
#endif

//klmod_linux: OS specific defines
//void Sleep(int x);
#define Sleep(x) usleep(x*1000)

/********************************************************************/
/* Address conversion macros                                        */
/********************************************************************/
#define K0_TO_K1(x)     ((unsigned int)(x)|0xA0000000)    /* kseg0 to kseg1 */
#define K1_TO_K0(x)     ((unsigned int)(x)&0x9FFFFFFF)    /* kseg1 to kseg0 */
#define K0_TO_PHYS(x)   ((unsigned int)(x)&0x1FFFFFFF)    /* kseg0 to physical */
#define K1_TO_PHYS(x)   ((unsigned int)(x)&0x1FFFFFFF)    /* kseg1 to physical */
#if defined(__KERNEL__)
/* PHYS_TO_K0 is defined in include/asm-mips/addrspace.h */
#undef PHYS_TO_K0
#define PHYS_TO_K0(x)   ((unsigned int)(x)|0x80000000)    /* physical to kseg0 */
#endif
#define PHYS_TO_K1(x)   ((unsigned int)(x)|0xA0000000)    /* physical to kseg1 */

#define  CacheToNonCacheVirtAddress    K0_TO_K1
#define  NonCacheToCacheVirtAddress    K1_TO_K0
#define  CacheVirtToPhyAddress         K0_TO_PHYS
#define  NonCacheVirtToPhyAddress      K1_TO_PHYS
#define  PhyToNonCacheVirtAddress      PHYS_TO_K1
#define  PhyToCacheVirtAddress         PHYS_TO_K0

// alignment macro
#define	 BCM_ALIGN(addr, boundary) ((addr + boundary - 1) & ~(boundary - 1))
#define  	BCM_ALIGN_ADJUST( boundary, objSize ) ((boundary/objSize) + ((boundary % objSize) ?  1 : 0))

void WaitLoop(unsigned long msecs);
void WaitLoopMicro(unsigned long usecs);
void Delayms(unsigned short);
uint32 BcmMuCreate(BcmHandle * mu);
uint32 BcmMuLock(BcmHandle mu);
uint32 BcmMuUnlock(BcmHandle mu);
uint32 BcmSemCreate(BcmHandle * sem);
uint32 BcmSemLock(BcmHandle sem);
uint32 BcmSemUnlock(BcmHandle sem);
uint32 BcmCreateEvent(uint32 tid);
int32 BcmWaitForEvent(uint32 event, uint32 * rEvent, uint32 msecs, uint32 tid);
int32 BcmSendEvent(uint32 event, uint32 tid);
int32 BcmClearEvent(uint32 event, uint32 taskId);
int32 BcmCreateThread(char * tname, uint32 tprio, uint32 tstack, uint32 * tid);
int32 BcmStartThread(uint32 tid, uint32 (*tfunc)(void *), void * targs);
uint32 BcmStartTimerPeriodic(int32 ticks, uint32 event, uint32 tid);
uint32 BcmStartTimerOneShot(int32 ticks, uint32 event, uint32 tid);
uint32 BcmCancelTimer(uint32 * t_handle);
int32 BcmStopAllThreads(void);
void BcmThreadInfo(void);
int Print(char *FmtStr, ...);

//interrupt.c
void BcmHalInterruptDisable(uint32 interruptId);
void BcmHalInterruptEnable(uint32 interruptId);
int BcmHalCpuInterruptDisable(void);
void BcmHalCpuInterruptEnable( ULONG );
uint32 BcmHalMapInterrupt(uint32 (*pfunc)(void), uint32 interruptId);

#define BcmHalInterruptDone   BcmHalInterruptEnable

typedef struct
{
    uint32 lockCount;
    uint32 waitCount;

    uint32 lockOwnerTaskId;

    BcmHandle semaphore;

} BcmMutexLiteData;

typedef BcmMutexLiteData * BcmMutexLite;

uint32 BcmMuLiteCreate(BcmMutexLite *mu);
uint32 BcmMuLiteDelete(BcmMutexLite mu);

#ifndef __KERNEL__ // The kernel mutex code does not work at user level.
uint32 BcmMuLiteLock(BcmMutexLite mu);
uint32 BcmMuLiteUnlock(BcmMutexLite mu);
#else
#define BcmMuLiteLock(/*BcmMutexLite*/ mu)                                              \
{                                                                                       \
    ULONG oldLevel;                                                                     \
                                                                                        \
    /* Because the 'mu' parameter is likely to be a global variable, the compiler    */ \
    /* will refuse to register optimize it (it will recalculate the address over and */ \
    /* over in this macro).  This essentially tells the compiler to register optimize*/ \
    /* the address.                                                                  */ \
    BcmMutexLiteData *pMutexData = mu;                                                  \
                                                                                        \
    /* Same kind of thing with taskIdCurrent; we can read it once and keep it local. */ \
    /* uint32 taskId = (uint32) taskIdCurrent;                                        */ \
    uint32 taskId = (uint32) current->pid;                                              \
                                                                                        \
    oldLevel = BcmHalCpuInterruptDisable();                                             \
                                                                                        \
    /* If somebody currently has the lock, and it isn't me, then I need to */           \
    /* wait for the lock.                                                  */           \
    if ((pMutexData->lockCount > 0) && (pMutexData->lockOwnerTaskId != taskId))         \
    {                                                                                   \
        /* Note that I use a while loop rather than an if statement; this is because */ \
        /* there is a possible race condition during the unlock where we can         */ \
        /* decrement the count to 0 before unlocking the semaphore; if the unlocking */ \
        /* task is interrupted then the locking task can blow through this while loop*/ \
        /* and get the lock.  Thus, when task is waiting on the semaphore, it needs  */ \
        /* to check the lock state again after waking up.                            */ \
        do                                                                              \
        {                                                                               \
            /* Increment the count of tasks waiting for the lock, then restore */       \
            /* interrupts. */                                                           \
            pMutexData->waitCount++;                                                    \
            BcmHalCpuInterruptEnable(oldLevel);                                         \
                                                                                        \
            /* Now go to sleep, yielding the processor until somebody unlocks the */    \
            /* mutex and releases the counting semaphore. */                            \
            BcmSemLock(pMutexData->semaphore);                                          \
                                                                                        \
            /* Lock out interrupts and decrement the count of tasks waiting. */         \
            oldLevel = BcmHalCpuInterruptDisable();                                     \
            pMutexData->waitCount--;                                                    \
        }                                                                               \
        while (pMutexData->lockCount > 0);                                              \
    }                                                                                   \
                                                                                        \
    /* PR5524 - Else, if someone else is waiting for the semaphore (this only   */      \
    /* happens if the other thread is a lower priority than me), then let them  */      \
    /* run and get the lock.                                                    */      \
    else if (pMutexData->waitCount > 0)                                                 \
    {                                                                                   \
        BcmHalCpuInterruptEnable(oldLevel);                                             \
                                                                                        \
        Sleep(1);                                                                       \
                                                                                        \
        oldLevel = BcmHalCpuInterruptDisable();                                         \
    }                                                                                   \
                                                                                        \
    /* Increment the lock count and set myself as the lock owner. */                    \
    pMutexData->lockCount++;                                                            \
    pMutexData->lockOwnerTaskId = taskId;                                               \
                                                                                        \
    BcmHalCpuInterruptEnable(oldLevel);                                                 \
}


#define BcmMuLiteUnlock(/*BcmMutexLite*/ mu)                                            \
{                                                                                       \
    /* Because the 'mu' parameter is likely to be a global variable, the compiler    */ \
    /* will refuse to register optimize it (it will recalculate the address over and */ \
    /* over in this macro).  This essentially tells the compiler to register optimize*/ \
    /* the address. */                                                                  \
    BcmMutexLiteData *pMutexData = mu;                                                  \
                                                                                        \
    /* Note that I don't need to disable interrupts around this.  If I am interrupted*/ \
    /* everything will work out ok, since either the thread will go into the wait    */ \
    /* loop, and I'll wake them up, or it will get the lock and I won't need to wake */ \
    /* anybody up.                                                                   */ \
    pMutexData->lockCount--;                                                            \
    if (pMutexData->waitCount > 0)                                                      \
    {                                                                                   \
        BcmSemUnlock(pMutexData->semaphore);                                            \
                                                                                        \
        /* PR5524 - How lame is this - the VxWorks counting semaphore implementation */ \
        /* doesn't work correctly.  If a thread is waiting for the semaphore (even a */ \
        /* higher priority thread!), it doesn't get a chance to run.  As a result, I */ \
        /* have to get the current thread to sleep a bit which should let the slacker*/ \
        /* OS give the waiting thread a chance to go (if it is equal or higher       */ \
        /* priority).                                                                */ \
        Sleep(0);                                                                       \
    }                                                                                   \
}

#endif // __KERNEL__


// DPullen - replaced the duplicate prototypes of the BcmAlloc API with an
// include of bcmAlloc.h, which is the only place where the API should be.
#include "bcmAlloc.h"


void BcmCacheInvalidate (void *adrs, size_t bytes);

#if !(BCM_REDUCED_IMAGE_SIZE)
void LogRawBuffer (char * str, UCHAR *pBuffer, int BufLen, BOOL wacky);
//int DEBUGMSG(int zone, char *FmtStr, ...);
#endif
BOOL HalMapInterrupt(IN ULONG InterruptNumber,IN ULONG InterruptVector, OUT DWORD *InterruptId);
int HALMSG(char *FmtStr, ...);

uint32	BcmIntLock(void);
void	BcmIntUnlock(uint32 flags);

//**********************Special Section*********************************
// This section is dedicated to pSOS defines that are used in the code
// but cannot be found in VxWorks
//**********************************************************************
//redefine C preprocessor for packed, Vxworks support diff syntax
#define __packed__
#define GNU_PACKED		__attribute__ ((packed))

typedef enum parm_type
    {
    FLAG = 1, IP = 2, HEX = 3, DECIMAL = 4, CHAR = 5, STRING = 6
    } PARM_TYPE;

#if __cplusplus
}
#endif

#endif


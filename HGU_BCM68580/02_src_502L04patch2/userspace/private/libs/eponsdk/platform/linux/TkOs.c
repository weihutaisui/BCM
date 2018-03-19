/*
* <:copyright-BRCM:2012:proprietary:epon
* 
*    Copyright (c) 2012 Broadcom 
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


////////////////////////////////////////////////////////////////////////////////
/// \file TkOs.c
/// \brief Teknovus OS interface
///
/// This file implements TkOs functionality for the CMX RTX RTOS.
///
////////////////////////////////////////////////////////////////////////////////

/// Functions uncalled by any variant
#define UNCALLED 0

#include <stdlib.h> 
#include <sys/time.h> 
#include <unistd.h> 
#include <sys/timerfd.h>

#include "Teknovus.h"
#include "TkOs.h"
#include "Timer.h"
#include "TkOsTypes.h"
#include "TkOsAlSem.h"
#include "cms_log.h"

////////////////////////////////////////////////////////////////////////////////
// Fixed length ("Block") allocation
//
// A block pool is a group of fixed-length blocks of memory.  As a result of
// the fixed length, block pool allocation is faster than byte pool allocation
// and there's less overhead.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// TkOsBlockPoolCreate:  Creates new block pool
///
/// Creates a new memory pool for fixed-length allocation.
///
/// The pool has internal storage overhead which must be taken into account
/// when reserving the memory beginning startAddr, if having a precise number
/// of blocks is important.
///
 // Parameters:
/// \param startAddr    Start address of heap.  (The implementation may not
///     use this information.)
/// \param  size        Size, in bytes, of a memory block
/// \param  numBlocks   Number of blocks in this pool
///
/// \return
/// Byte pool identifier used to make requests of this pool
////////////////////////////////////////////////////////////////////////////////
//extern
TkOsBlockPool TkOsBlockPoolCreate_ (void * startAddr,
                                   unsigned int size,
                                   unsigned int numBlocks
                                   )
{
    TkOsBlockPoolBlockHdr * FAST block;

    ((TkOsBlockPool)startAddr)->free = (TkOsBlockPoolBlockHdr *)
        ((TkOsBlockPool)startAddr + 1);

    block = ((TkOsBlockPool)startAddr)->free;

    while (--numBlocks != 0)
    {
        block->next = (TkOsBlockPoolBlockHdr *)
            ((U8*)block + sizeof(*block) + size);
        block = block->next;
    }
    block->next = NULL;


    return (TkOsBlockPool)startAddr;
} // TkOsBlockPoolCreate


////////////////////////////////////////////////////////////////////////////////
/// TkOsBlockAlloc:  Allocates block from a block pool
///
/// All blocks in a block pool are the same size.
///
 // Parameters:
/// \param pool     Block pool from which to allocate
///
/// \return
/// Pointer to block returned; NULL if no block available
////////////////////////////////////////////////////////////////////////////////
//extern
//#pragma disable
void * TkOsBlockAlloc (TkOsBlockPool pool)
{
    TkOsBlockPoolBlockHdr *result;
    result = pool->free;
    if (result != NULL)
    {
        pool->free = result->next;

        // remember pool from whence we came
        result->next = (TkOsBlockPoolBlockHdr *)pool;
        // skip over header and point into user data area
        result++;
    }
    return result;
} // TkOsBlockAlloc



////////////////////////////////////////////////////////////////////////////////
/// TkOsBlockFree:  Return block to block pool
///
/// Frees the block and returns it to the pool for future use
///
 // Parameters:
/// \param block    block to free
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
//#pragma disable
void TkOsBlockFree (void * block)
{
    TkOsBlockPoolBlockHdr  * FAST b;

    b = (TkOsBlockPoolBlockHdr*)block - 1;
    TkOsBlockPool FAST pool = (TkOsBlockPool)(b->next);

    b->next = pool->free;
    pool->free = b;

} // TkOsBlockFree



////////////////////////////////////////////////////////////////////////////////
// Message Queues
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
/// TkOsMsgQueueCreate_:  Creates message queue
///
///
/// Overhead: sizeof(TkOsMsgQOverhead)
///
 // Parameters:
/// \param startAddr    Start address of space for message queue
/// \param maxMsgs      Maximum messages that can be enqueued in this queue
///
/// \return
/// Message Queue object
////////////////////////////////////////////////////////////////////////////////
//extern
TkOsMsgQ TkOsMsgQueueCreate_ (void * startAddr,
                            U8 maxMsgs
                            )
    {
    ((TkOsMsgQ)startAddr)->maxMsgs = maxMsgs;
    ((TkOsMsgQ)startAddr)->msgCount = 0;
    ((TkOsMsgQ)startAddr)->head = 0;
    ((TkOsMsgQ)startAddr)->tail = 0;

    return (TkOsMsgQ)startAddr;
    } // TkOsMsgQueueCreate_



////////////////////////////////////////////////////////////////////////////////
/// TkOsMsgSend:  Send a message to a message queue
///
 // Parameters:
/// \param q    Queue to which to send message
/// \param msg  message to place into queue
///
/// \return
/// TRUE if send successful; otherwise FALSE
////////////////////////////////////////////////////////////////////////////////
//extern
//#pragma disable
BOOL TkOsMsgSend (TkOsMsgQ q, TkOsMsg msg)
    {
    BOOL FAST result = FALSE;

    if (q->msgCount < q->maxMsgs)
        {
        ((TkOsMsg*)(q + 1))[q->tail] = msg;

        q->tail = q->tail + 1;
        if (q->tail == q->maxMsgs)
            {
            q->tail = 0;
            }

        q->msgCount++;
        result = TRUE;
        }

    return result;
    } // TkOsMsgSend


////////////////////////////////////////////////////////////////////////////////
/// TkOsMsgAvailable:  Checks to see if one or more messages is present
/// inside a queue
///
///
 // Parameters:
/// \param q    Queue to check.
///
/// \return
/// Message, or NULL if an error occurred
////////////////////////////////////////////////////////////////////////////////
//extern
Bool TkOsMsgAvailable (TkOsMsgQ q)
    {
    if (q->msgCount)
        {
        return TRUE;
        }
    return FALSE;
    }



////////////////////////////////////////////////////////////////////////////////
/// TkOsMsgReceive:  Receive a message from a queue
///
/// Gets the next message from the queue.  If no message is available, the
/// thread will block until a message arrives.
///
 // Parameters:
/// \param q    Queue from which to receive.
///
/// \return
/// Message, or NULL if an error occurred
////////////////////////////////////////////////////////////////////////////////
//extern
//#pragma disable
TkOsMsg TkOsMsgReceive (TkOsMsgQ q)
    {
    TkOsMsg result = 0;

    if (q->msgCount != 0)
        {
        result = ((TkOsMsg*)(q + 1))[q->head];

        q->head = q->head + 1;
        if (q->head == q->maxMsgs)
            {
            q->head = 0;
            }

        q->msgCount--;
        }

    return result;
    } // TkOsMsgReceive


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Timers
////////////////////////////////////////////////////////////////////////////////

// reserve some space for our timer overhead
static U8 BULK tkOsTimerPoolSpace[sizeof(TkOsBlockPoolOverhead) +
                             (TkOsNumTimers * (sizeof(TkOsTimerOverhead) +
                             sizeof(void*)))];
static TkOsBlockPool BULK tkOsTimerPool;

static TkOsTimer FAST activeTimers;


void TkOsDumpActiveTimers (void)
    {
    TkOsTimer FAST p;
    cmsLog_debug("active timer list:\n");
    p = activeTimers;    
    while (NULL != p) 
        {
        cmsLog_debug("param=%04x ", p->param);
        p = p->next;   
        }
    cmsLog_debug("end list\n"); 
    }

////////////////////////////////////////////////////////////////////////////////
/// TkOsSysTick:  gets current system time
///
 // Parameters:
/// \param None
///
/// \return
/// Current system tick
////////////////////////////////////////////////////////////////////////////////
//extern
//#pragma disable
TkOsTick TkOsSysTick (void)
{
    TkOsTick ms;
    struct timespec current;
    if(clock_gettime(CLOCK_MONOTONIC, &current) == -1)
    {
        cmsLog_error("TkOsSysTick: failed to get time!\n");
        return 0;
    }        
    ms = (current.tv_sec*1000) + (current.tv_nsec/1000000);
    return ms;
        
} // TkOsSysTick



////////////////////////////////////////////////////////////////////////////////
/// TimeLessThan:  returns t1 < t2, considering wraparound
///
 // Parameters:
/// \param t1   timestamp 1
/// \param t2   timestamp 2
///
/// \return
/// TRUE if t1 < t2
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL TimeLessThan (TkOsTick t1, TkOsTick t2)
    {
    return (t1 - t2) >= 0x80000000L;
    } // TimeLessThan

///////////////////////////////////////////////////////////////////////////////
/// TimerEnqueue:  Adds a timer to priority-sorted timer list
///
 // Parameters:
/// \param timer    Timer to add
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static void TimerEnqueue (TkOsTimer timer) 
{
    TkOsTimer p;
    TkOsTimer q;

    q = NULL;
    p = activeTimers;
    while ((p != NULL) && TimeLessThan (p->expiryTime, timer->expiryTime))
    {
        q = p;
        p = p->next;
    }

    // insert in front of p
    timer->next = p;
    if (q != NULL)
    {
        q->next = timer;
    }
    else
    {
        activeTimers = timer;
    }

} // TimerEnqueue



////////////////////////////////////////////////////////////////////////////////
/// TimerDequeue:  Remove timer from active list
///
 // Parameters:
/// \param timer    Timer to remove
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//lint -e{818} suppress timer should be declared as pointing to const
static
void TimerDequeue (TkOsTimer timer)
{
    TkOsTimer FAST p;
    TkOsTimer FAST q;

    q = NULL;
    p = activeTimers;
    while ((p != NULL) && (p != timer))
    {
        q = p;
        p = p->next;
    }

    if (p != NULL)
    { // found it
        if (q != NULL)
        {
            q->next = timer->next;
        }
        else
        {
            activeTimers = timer->next;
        }
    }
} // TimerDequeue



////////////////////////////////////////////////////////////////////////////////
/// TkOsTimerCreate:  Creates a one-shot timer
///
/// The timer begins running immediately upon creation; there is no
/// "start timer" function.
///
 // Parameters:
/// \param time     time, in system ticks, until the timer expires
/// \param action   Function called when timer expires.
/// \param repeat   TRUE if timer repeatedly fires; FALSE if it's one-shot
///
/// \return
/// Timer ID
////////////////////////////////////////////////////////////////////////////////
//extern
TkOsTimer TkOsTimerCreate (TkOsTimerInterval time,
                           TkOsTimerAction action,
                           TkOsTimerActionParam param)
{
    TkOsTimer FAST result;
    int fd;
    struct itimerspec newtimespec;
    
    if ((result = TkOsBlockAlloc (tkOsTimerPool)) != NULL)
    { 
        result->expiryTime = TkOsSysTick() + time;
        result->interval = time;
        result->action = action;
        result->param = param;
        result->repeat = FALSE;
    }
    else
    {
        cmsLog_error("fatal error: no resource to create timer !\n");
        return NULL;
    }

    memset(&newtimespec, 0, sizeof(newtimespec));
    
    fd = timerfd_create(CLOCK_MONOTONIC, 0);
     
    if( fd != -1 )
    {
        /* conversion from ms to s */
        newtimespec.it_value.tv_sec = time/1000;
        /* conversion from ms to ns */
        newtimespec.it_value.tv_nsec = (time%1000)*1000*1000;    
        if(0 == timerfd_settime(fd, 0, &newtimespec, NULL))
        { 
            result->fd = fd;
            TimerEnqueue(result);   
            return result;
        }
        
    }

    return NULL;
} 



////////////////////////////////////////////////////////////////////////////////
/// TkOsTimerCreateRepeating:  Creates a repeatign timer
///
/// The timer begins running immediately upon creation; there is no
/// "start timer" function.
///
 // Parameters:
/// \param time     time, in system ticks, until the timer expires
/// \param action   Function called when timer expires.
/// \param param    Parameter to pass to the callback function.
///
/// \return
/// Timer ID
////////////////////////////////////////////////////////////////////////////////
//extern
TkOsTimer TkOsTimerCreateRepeating (TkOsTimerInterval time,
                           TkOsTimerAction action,
                           TkOsTimerActionParam param)
{
    TkOsTimer FAST result;
    int fd;
    struct itimerspec newtimespec;

    if ((result = TkOsBlockAlloc (tkOsTimerPool)) != NULL)
    { 
        result->expiryTime = TkOsSysTick() + time;
        result->interval = time;
        result->action = action;
        result->param = param;
        result->repeat = TRUE;
       
    }

    fd = timerfd_create(CLOCK_MONOTONIC, 0);
   
    if( fd != -1 )
    {
         /* conversion from ms to s */
         newtimespec.it_interval.tv_sec = (time / 1000);
          /* conversion from ms to ns */
         newtimespec.it_interval.tv_nsec = (time % 1000) * 1000 * 1000;
         newtimespec.it_value.tv_sec = newtimespec.it_interval.tv_sec; 
         newtimespec.it_value.tv_nsec = newtimespec.it_interval.tv_nsec;
         if(0 == timerfd_settime(fd, 0, &newtimespec, NULL))
         {   
             result->fd = fd; 
             TimerEnqueue(result);
             cmsLog_debug("TkOs: created repeat timer %x\n", param);
             cmsLog_debug("interval: %d, %03d", newtimespec.it_interval.tv_sec,
                                          newtimespec.it_interval.tv_nsec);
             return result;
         }
    }
    else
    {
         int errsv;
         errsv = errno; 
         cmsLog_error("fatal error: unable to create timer fd! error=%s\n", strerror(errsv));
    }

    return NULL;
} 



////////////////////////////////////////////////////////////////////////////////
/// TkOsTimerDestroy:  destroy timer
///
/// Destroy a timer, ensuring that it will no longer expire.  If the timer
/// has already expired, the effects of its actions (say, putting a message
/// into a message queue) cannot be undone.
///
 // Parameters:
/// \param timer    Timer to destroy
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void TkOsTimerDestroy (TkOsTimer timer)
{
   struct itimerspec oldtimespec;
   struct itimerspec newtimespec;

   
   if(0 == timerfd_gettime(timer->fd, &oldtimespec))
   {
       newtimespec.it_interval.tv_sec = 0;
       newtimespec.it_interval.tv_nsec = 0;
       newtimespec.it_value.tv_sec = 0;
       newtimespec.it_value.tv_nsec = 0;
       if(0 == timerfd_settime(timer->fd, TFD_TIMER_ABSTIME, &newtimespec, NULL))
       {
           TimerDequeue(timer);
           close(timer->fd); 
           TkOsBlockFree(timer); 
       }    
   } 

   return;
}



////////////////////////////////////////////////////////////////////////////////
/// TkOsTimerDestroyAll:  destroy all timers with specified ID
///
/// Destroy a set of timers, ensuring that they will no longer expire.  If the
/// timers have already expired, the effects of their actions (say, putting a
/// message into a message queue) cannot be undone.
///
/// Parameters:
/// \param id   Timers to destroy
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void TkOsTimerDestroyAll(TkOsTimerActionParam id)
{
    TkOsTimer FAST q;
    TkOsTimer FAST p = activeTimers;

    while (p != NULL)
    {
        q = p;
        p = p->next;
        if (q->param == id)
        {
            TkOsTimerDestroy(q);
        }
     }
} // TkOsTimerDestroyAll

    
////////////////////////////////////////////////////////////////////////////////
/// TkOsTimerInit:  Init TkOsTimers
///
/// Performs initialization needed to support timers
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void TkOsTimerInit (void)
{
    tkOsTimerPool = TkOsBlockPoolCreate (tkOsTimerPoolSpace,
                                        sizeof(TkOsTimerOverhead),
                                        TkOsNumTimers, NULL);

    activeTimers = NULL; 
} 



////////////////////////////////////////////////////////////////////////////////
//TkOsInitTimerFdSet:
//set fdset with the fds in active timer list. and returns the maximum fd 
///////////////////////////////////////////////////////////////////////////////

int TkOsTimerInitFdSet(fd_set *pReadFds)
{
    TkOsTimer q;
    TkOsTimer p = activeTimers;
    int max_fd = 0;

    while (p != NULL)
    {
        q = p;
        p = p->next;
        
        if(q->fd > max_fd)
        {
            max_fd = q->fd;
        }

        FD_SET(q->fd, pReadFds);
    }
    return max_fd;
}

////////////////////////////////////////////////////////////////////////////////
/// TkOsCheckTimerExpiry:  Checks timer list for expired timers
///
/// Detailed description
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//static
void TkOsTimerCheckExpiry (fd_set *pReadFds)
{
    TkOsTimer FAST p, q;
    U64 expire_count;
    
    q = activeTimers;

    while (q != NULL)
    {
        p = NULL;
        if(FD_ISSET(q->fd, pReadFds))
        {
            //cmsLog_debug("expire param=%x\n", q->param);
            read(q->fd, &expire_count, sizeof(U64)); 
            q->action(q->param);
            
            if (q->repeat == FALSE)
            {     
                p = q;               
            }

        }
        
        q = q->next;       
        
        if(p != NULL)
        {
            TkOsTimerDestroy(p);
            p = NULL;    
        }
    } 
}


////////////////////////////////////////////////////////////////////////////////
// System
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
/// TkOsInit:  initialize TkOs
///
/// Initialize TkOs internals.  This function must be called before any other
/// TkOs function may be used.
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void TkOsInit (void)
    {
    //InitPolledTimer();
    TkOsTimerInit();
    } // TkOsInit



////////////////////////////////////////////////////////////////////////////////
/// TkOsStart:  Start the system
///
/// this call will start the scheduler executing.  This call never returns.
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void TkOsStart (void)
    {
    //StartTkOsTimer(MsecToTicks(TkOsMsPerTick));
    } // TkOsStart


// end of TkOs.c


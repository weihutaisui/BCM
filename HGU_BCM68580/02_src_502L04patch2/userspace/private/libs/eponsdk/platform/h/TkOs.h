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


#if !defined(TkOs_h)
#define TkOs_h
////////////////////////////////////////////////////////////////////////////////
/// \file TkOs.h
/// \brief Teknovus OS interface
///
/// These functions provide an abstraction layer from the underlying RTOS.
/// The intent is to be as minimalist as possible to aid porting from one
/// RTOS to another, as well as avoiding reliance on features that may not
/// be available from different kernels.
///
////////////////////////////////////////////////////////////////////////////////

#if defined(__cplusplus)
extern "C" {
#endif

#include "Build.h"    
#include "Teknovus.h"
#include "TkOsTypes.h"
#include "TkOsAlThread.h"


#if TK_OS_DEBUG
#define TkOsDebugParam(param) ,param
#define TkOsDebugString(param) param
#else
#define TkOsDebugParam(param) 
#define TkOsDebugString(param)
#endif


////////////////////////////////////////////////////////////////////////////////
// Variable length ("Byte") allocation
//
// A byte pool is a general-purpose heap from which you can allocate variable-
// length memory objects.
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
/// TkOsBytePoolCreate_:  Creates new byte pool
///
/// Creates a new memory pool for variable-length allocation.
///
/// The pool has internal storage overhead which must be taken into account
/// when allocating the startAddr and size, if an exact "size" is important.
///
/// Overhead: sizeof(TkOsBytePoolOverhead)
///
/// This overhead does not count any per-chunk overhead; as with most memory
/// allocators, fewer than "size" bytes of memory can successfully be allocated
/// from the pool.
///
 // Parameters:
///	\param startAddr	Start address of heap.
/// \param	size		Size, in bytes, of heap
/// \param  name	    Name of block pool, do not send a non static string 
/// 
/// \return 
/// Byte pool identifier used to make requests of this pool
////////////////////////////////////////////////////////////////////////////////
extern
TkOsBytePool TkOsBytePoolCreate_ (void * startAddr, unsigned int size
								 TkOsDebugParam(const U8* name));

#define TkOsBytePoolCreate(startAddr,size,name)								 \
												TkOsBytePoolCreate_(		 \
 												startAddr,					 \
												size /* No , see macro def*/ \
												TkOsDebugParam(name))		

////////////////////////////////////////////////////////////////////////////////
/// TkOsByteAlloc_:  Allocates memory from a byte pool
///
/// Allocates a new block of memory from the given byte pool.  The returned
/// pointer is guaranteed to be aligned properly for any type.
///
 // Parameters:
///	\param pool		Byte pool from which to allocate
/// \param size		Size of chunk to allocate
/// 
/// \return 
/// Pointer to allocated memory.  NULL if no memory available.
////////////////////////////////////////////////////////////////////////////////
extern
void * TkOsByteAlloc (TkOsBytePool pool, unsigned int size);



////////////////////////////////////////////////////////////////////////////////
/// TkOsByteFree:  Return memory to byte pool
///
/// Frees previously allocated memory
///
 // Parameters:
/// \param mem		Memory to return (as granted by TkOsByteAlloc)
/// 
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void TkOsByteFree (void * mem);



////////////////////////////////////////////////////////////////////////////////
// Fixed length ("Block") allocation
//
// A block pool is a group of fixed-length blocks of memory.  As a result of
// the fixed length, block pool allocation is faster than byte pool allocation
// and there's less overhead.
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
/// TkOsBlockPoolCreate_:  Creates new block pool
///
/// Creates a new memory pool for fixed-length allocation.
///
/// The pool has internal storage overhead which must be taken into account
/// when reserving the memory beginning startAddr, if having a precise number 
/// of blocks is important.
///
/// Overhead: sizeof(TkOsBlockPoolOverhead) + sizeof(void*) * numBlocks
///
 // Parameters:
///	\param startAddr	Start address of heap.  (The implementation may not
///		use this information.)
/// \param	size		Size, in bytes, of a memory block
/// \param	numBlocks	Number of blocks in this pool
/// \param  name	    Name of block pool, do not send a non static string 
/// \return 
/// Byte pool identifier used to make requests of this pool
////////////////////////////////////////////////////////////////////////////////
extern
TkOsBlockPool TkOsBlockPoolCreate_(void * startAddr,
								   unsigned int size,
								   unsigned int numBlocks
								   TkOsDebugParam(const U8* name)
								   );


#define TkOsBlockPoolCreate(startAddr,size,numBlocks,name)				\
												TkOsBlockPoolCreate_(	\
 												startAddr,				\
												size,					\
					/* No comma see macro def*/	numBlocks				\
												TkOsDebugParam(name))		
											

////////////////////////////////////////////////////////////////////////////////
/// TkOsBlockAlloc:  Allocates block from a block pool
///
/// All blocks in a block pool are the same size.
///
 // Parameters:
///	\param pool		Block pool from which to allocate
/// 
/// \return 
/// Pointer to block returned; NULL if no block available
////////////////////////////////////////////////////////////////////////////////
extern
void * TkOsBlockAlloc (TkOsBlockPool pool);



////////////////////////////////////////////////////////////////////////////////
/// TkOsBlockAllocChecked: Allocates block from block pool if size ok
///
/// All blocks in a block pool are the same size, but the blocks may be
/// used to carry messages of different sizes. This function guards against
/// allocating a block for message size larger than block size.
///
//  Parameters:
///	\param pool		Block pool from which to allocate
///	\param size		Size of the message that the allocated block is intended to
///					carry
///
/// \return
/// Pointer to block returned; NULL if no block available or size too big
////////////////////////////////////////////////////////////////////////////////
extern
void * TkOsBlockAllocChecked(TkOsBlockPool pool, U32 size);



////////////////////////////////////////////////////////////////////////////////
/// TkOsBlockFree:  Return block to block pool
///
/// Frees the block and returns it to the pool for future use
///
 // Parameters:
/// \param block	block to free
/// 
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void TkOsBlockFree (void * block);



////////////////////////////////////////////////////////////////////////////////
// Message Queues
////////////////////////////////////////////////////////////////////////////////

/// TkOs message type
///
/// TkOs messages are fixed size, limited to the size of a pointer.  
/// Typically, that pointer will point to a larger structure containing 
/// many bytes of data, perhaps allocated from a TkOs[Byte|Block]Pool.  
/// Simple tasks may be able to get by with just a few bytes worth of message, 
/// though, in which case the message value may not really be a pointer.
typedef U32 TkOsMsg;


////////////////////////////////////////////////////////////////////////////////
/// TkOsMsgQueueCreate_:  Creates message queue
///
///
/// Overhead: sizeof(TkOsMsgQOverhead)
///
 // Parameters:
///	\param startAddr	Start address of space for message queue
///	\param maxMsgs		Maximum messages that can be enqueued in this queue
/// \param name			Name of the message queue.  Note this must be a globally
/// allocated piece of data and not stack local. 
/// 
/// \return 
/// Message Queue object
////////////////////////////////////////////////////////////////////////////////
extern
TkOsMsgQ TkOsMsgQueueCreate_ (void * startAddr,
							U8 maxMsgs
							TkOsDebugParam(const U8* name)
							);

#define TkOsMsgQueueCreate(startAddr,maxMsgs,name) TkOsMsgQueueCreate_(		\
 												startAddr,				    \
					/* No comma see macro def*/ maxMsgs						\
												TkOsDebugParam(name))			
												

////////////////////////////////////////////////////////////////////////////////
/// TkOsMsgSend:  Send a message to a message queue
///
 // Parameters:
///	\param q	Queue to which to send message
/// \param msg	message to place into queue
/// 
/// \return 
/// TRUE if send successful; otherwise FALSE
////////////////////////////////////////////////////////////////////////////////
extern
BOOL TkOsMsgSend (TkOsMsgQ q, TkOsMsg msg);


////////////////////////////////////////////////////////////////////////////////
/// TkOsMsgSendWaiting: Send a message to a queue with wait
///
//  Parameters:
///	\param q	Queue to which to send message
/// \param msg	message to place into queue
/// \param ticks number of timer ticks to wait	
///
/// \return 
/// TRUE if send successful; otherwise FALSE
////////////////////////////////////////////////////////////////////////////////
extern
BOOL TkOsMsgSendWaiting(TkOsMsgQ q, TkOsMsg msg, TkOsTick ticks);


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
extern
Bool TkOsMsgAvailable (TkOsMsgQ q);


////////////////////////////////////////////////////////////////////////////////
/// TkOsMsgReceive:  Receive a message from a queue
///
/// Gets the next message from the queue.  If no message is available, the
/// thread will block until a message arrives.
///
 // Parameters:
///	\param q 	Queue from which to receive.
/// 
/// \return 
/// Message, or NULL if an error occurred
////////////////////////////////////////////////////////////////////////////////
extern
TkOsMsg TkOsMsgReceive (TkOsMsgQ q);



////////////////////////////////////////////////////////////////////////////////
/// TkOsMsgReceiveTimed: Non-blocking receive from a queue 
///
/// Gets the next message from the queue.  If no message is available, the
/// thread will block until a message is received or a timeout occurs.
///
//  Parameters:
///	\param q 		Queue from which to receive.
/// \param timeout	Timeout value in system ticks 
///
/// \return
/// Message, or NULL if an error occurred
////////////////////////////////////////////////////////////////////////////////
extern
TkOsMsg TkOsMsgReceiveTimed(TkOsMsgQ q, TkOsTick wait);



////////////////////////////////////////////////////////////////////////////////
// Threads
////////////////////////////////////////////////////////////////////////////////

/// Thread Priority
///
/// Lower-valued priorities correspond to higher priority threads.  That is,
/// the highest priority thread is "0" and the lowest would be 255.  Wacky,
/// but priority has a long RTOS tradition of being backwards.  It will be 
/// easier to map this layer to another RTOS if we don't reverse the sense
/// of priority comparison.
typedef U8 ThreadPriority;

typedef
	void (CODE* ThreadBody) (void);
								 



////////////////////////////////////////////////////////////////////////////////
// Timers
////////////////////////////////////////////////////////////////////////////////



void TkOsDumpActiveTimers (void);

////////////////////////////////////////////////////////////////////////////////
/// TkOsSysTick:  gets current system time
///
 // Parameters:
/// \param None
/// 
/// \return 
/// Current system tick
////////////////////////////////////////////////////////////////////////////////
extern
TkOsTick TkOsSysTick (void);



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
extern
BOOL TimeLessThan (TkOsTick t1, TkOsTick t2);




////////////////////////////////////////////////////////////////////////////////
/// TkOsTimerCreate:  Creates a one-shot timer
///
/// The timer begins running immediately upon creation; there is no 
/// "start timer" function.
///
 // Parameters:
///	\param time 	time, in system ticks, until the timer expires
/// \param action  	Function called when timer expires.
/// \param repeat	TRUE if timer repeatedly fires; FALSE if it's one-shot
/// 
/// \return 
/// Timer ID 
////////////////////////////////////////////////////////////////////////////////
extern
TkOsTimer TkOsTimerCreate (TkOsTimerInterval time,
						   TkOsTimerAction action,
						   TkOsTimerActionParam param);



////////////////////////////////////////////////////////////////////////////////
/// TkOsTimerCreateRepeating:  Creates a repeatign timer
///
/// The timer begins running immediately upon creation; there is no 
/// "start timer" function.
///
 // Parameters:
///	\param time 	time, in system ticks, until the timer expires
/// \param action  	Function called when timer expires.
/// \param param    Parameter to pass to the callback function.	
/// 
/// \return 
/// Timer ID 
////////////////////////////////////////////////////////////////////////////////
//extern
TkOsTimer TkOsTimerCreateRepeating (TkOsTimerInterval time,
						   TkOsTimerAction action,
						   TkOsTimerActionParam param);



////////////////////////////////////////////////////////////////////////////////
/// TkOsTimerDestroy:  destroy timer
///
/// Destroy a timer, ensuring that it will no longer expire.  If the timer
/// has already expired, the effects of its actions (say, putting a message
/// into a message queue) cannot be undone.  A one-shot timer that is known 
/// to have expired (say, because you received a message from its action proc)
/// need not be destroyed.
///
 // Parameters:
///	\param timer	Timer to destroy
/// 
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void TkOsTimerDestroy (TkOsTimer timer);


////////////////////////////////////////////////////////////////////////////////
/// TkOsTimerDestroyAll:  destroy all timers with specified ID
///
/// Destroy a set of timers, ensuring that they will no longer expire.  If the
/// timers have already expired, the effects of their actions (say, putting a
/// message into a message queue) cannot be undone.
///
/// Parameters:
///	\param id	Timers to destroy
/// 
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void TkOsTimerDestroyAll(TkOsTimerActionParam id);


////////////////////////////////////////////////////////////////////////////////
/// TkOsThreadYield : let other tasks at same pri run. 
///
//  Parameters:
/// 
///
/// \return Pointer to the allocated semaphore; else NULL
////////////////////////////////////////////////////////////////////////////////
void TkOsThreadYield(void);



////////////////////////////////////////////////////////////////////////////////
// System
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//TkOsTimerInitFdSet:
//set fdset with the fds in active timer list. and returns the maximum fd 
///////////////////////////////////////////////////////////////////////////////

int TkOsTimerInitFdSet(fd_set *pReadFds);

////////////////////////////////////////////////////////////////////////////////
/// TkOsTimerCheckExpiry:  Checks timer list for expired timers
///
/// Detailed description
///
 // Parameters:
///	\param None
/// 
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////
void TkOsTimerCheckExpiry (fd_set *pReadFds);



////////////////////////////////////////////////////////////////////////////////
/// TkOsInit:  initialize TkOs
///
/// Initialize TkOs internals.  This function must be called before any other
/// TkOs function may be used. 
///
 // Parameters:
///	\param None
/// 
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void TkOsInit (void);



////////////////////////////////////////////////////////////////////////////////
/// TkOsStart:  Start the system
///
/// This call will start the scheduler executing.  This call never returns.
///
 // Parameters:
///	\param None
/// 
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void TkOsStart (void);




#if defined(__cplusplus)
}
#endif

#endif // TkOs.h

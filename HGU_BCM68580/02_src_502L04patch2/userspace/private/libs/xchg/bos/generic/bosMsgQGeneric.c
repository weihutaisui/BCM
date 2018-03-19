/****************************************************************************
*
*  <:copyright-BRCM:2008:proprietary:standard
*  
*     Copyright (c) 2008 Broadcom 
*     All Rights Reserved
*  
*   This program is the proprietary software of Broadcom and/or its
*   licensors, and may only be used, duplicated, modified or distributed pursuant
*   to the terms and conditions of a separate, written license agreement executed
*   between you and Broadcom (an "Authorized License").  Except as set forth in
*   an Authorized License, Broadcom grants no license (express or implied), right
*   to use, or waiver of any kind with respect to the Software, and Broadcom
*   expressly reserves all rights in and to the Software and all intellectual
*   property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
*   NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
*   BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
*  
*   Except as expressly set forth in the Authorized License,
*  
*   1. This program, including its structure, sequence and organization,
*      constitutes the valuable trade secrets of Broadcom, and you shall use
*      all reasonable efforts to protect the confidentiality thereof, and to
*      use this information only in connection with your use of Broadcom
*      integrated circuit products.
*  
*   2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*      AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*      WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
*      RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
*      ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
*      FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
*      COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
*      TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
*      PERFORMANCE OF THE SOFTWARE.
*  
*   3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
*      ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
*      INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
*      WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
*      IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
*      OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
*      SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
*      SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
*      LIMITED REMEDY.
*  :>
****************************************************************************
*
*  Filename: bosMsgQGeneric.c
*
****************************************************************************
*  Description:
*
*
****************************************************************************/
/**
*
*  @file    bosMsgQGeneric.c
*
*  @brief   Contains OS independant definitions for the BOS MsgQ module.
*
****************************************************************************/

/* ---- Include Files ---------------------------------------------------- */

#include <bosCfg.h>

#if BOS_CFG_MSGQ

#include <bosError.h>
#include <bosMsgQ.h>
#include <bosMsgQPrivate.h>

#include <bosTask.h>
#include <bosMutex.h>
#include <bosSem.h>
#include <bosEvent.h>
#include <bosLog.h>

#include <string.h>

#if BOS_CFG_TIME_FOR_TIMED_FUNCTION
#  if BOS_CFG_TIME
#     include <bosTime.h>
#  else
#     error "Using System time for Timed function requires Time module support (not present)"
#  endif
#endif


/**
 * @addtogroup bosMsgQ
 * @{
 */

/* ---- Public Variables ------------------------------------------------- */

#if BOS_CFG_ERROR_FUNCTION_STRINGS

const char *gBosMsgQFunctionName[ BOS_MSGQ_NUM_FUNC_ID ];

#endif   /* BOS_CFG_ERROR_FUNCTION_STRINGS */

/* ---- Private Constants and Types -------------------------------------- */

/* Message queue element */
typedef struct QELEMENT
{
   BOS_MSGQ_MSG       msg;
   struct QELEMENT   *next;
} QELEMENT;

/* Message queue */
typedef struct MSGQ
{
   QELEMENT      *freeChain;                   /* Linked list of free buffer */
   QELEMENT      *pendingChain;                /* Linked list of pending buffers */
   QELEMENT       bufferPool[BOS_CFG_MSGQ_MAX_MSG_PER_MSGQ];  /* Pool of buffers */
   BOS_SEM        sema4;
   BOS_EVENT      event;
   BOS_BOOL       isEventBased;
   BOS_BOOL       inUse;
   unsigned int   noFreeMsgCount;
} MSGQ;


/* ---- Private Variables ------------------------------------------------ */

static BOS_BOOL   gBosMsgQInitialized = BOS_FALSE;

/* Mutexes used by the message queue module to protect critical sections */
static BOS_MUTEX  gMsgQMutex;

/* Message queues */
static MSGQ       gMsgQ[ BOS_CFG_MSGQ_MAX_MSGQS ];


/* ---- Private Function Prototypes -------------------------------------- */

/* Functions required for accessing the message queue */
static void InitMsgQueue( int qIndex, unsigned int qSize );
static QELEMENT *Dequeue( QELEMENT** chain );
static void Enqueue( QELEMENT** chain, QELEMENT *bufp );
static BOS_STATUS MsgQCreate
(
   const char    *name,
   unsigned int   qSize,
   BOS_TASK_ID   *eventTaskId,
   BOS_MSGQ      *qid
);


/* ---- Functions -------------------------------------------------------- */




#if BOS_CFG_ERROR_FUNCTION_STRINGS

/***************************************************************************/
/**
*  Initializes the contents of @a gBosMsgQFunctionName, which is used to
*  translate function IDs into character strings.
*/

void bosMsgQInitFunctionNames( void )
{
   gBosMsgQFunctionName[ BOS_MSGQ_INIT_FUNC_ID ]          = "bosMsgQInit";
   gBosMsgQFunctionName[ BOS_MSGQ_TERM_FUNC_ID ]          = "bosMsgQTerm";
   gBosMsgQFunctionName[ BOS_MSGQ_CREATE_FUNC_ID ]        = "bosMsgQCreate";
   gBosMsgQFunctionName[ BOS_MSGQ_DESTROY_FUNC_ID ]       = "bosMsgQDestroy";
   gBosMsgQFunctionName[ BOS_MSGQ_SEND_FUNC_ID ]          = "bosMsgQSend";
   gBosMsgQFunctionName[ BOS_MSGQ_RECV_FUNC_ID ]          = "bosMsgQReceive";
   gBosMsgQFunctionName[ BOS_MSGQ_TIMED_RECV_FUNC_ID ]    = "bosMsgQTimedReceive";

   bosErrorAddFunctionStrings( BOS_MSGQ_MODULE, gBosMsgQFunctionName, BOS_MSGQ_NUM_FUNC_ID );

} /* bosMsgQInitFunctioNames */

#endif   /* BOS_CFG_ERROR_FUNCTION_STRINGS */

/***************************************************************************/

#if BOS_CFG_RESET_SUPPORT

BOS_STATUS bosMsgQTimedReceiveR( BOS_MSGQ *qid, BOS_TIME_MS timeout, BOS_MSGQ_MSG *qMsg )
{
   BOS_STATUS retval;
#if BOS_CFG_TIME_FOR_TIMED_FUNCTION 
   BOS_TIME_MS start;
   BOS_TIME_MS now;
   BOS_TIME_MS delta = 0;
   BOS_TIME_MS localTimeout;
   BOS_TIME_MS timeRemaining = timeout;
   bosTimeGetMs(&start);
#endif
   
   while ( !bosResourceIsResetPending() )
   {
#if BOS_CFG_TIME_FOR_TIMED_FUNCTION 
      timeRemaining = timeout - delta;
      if (timeRemaining > BOS_CFG_RESET_GRANULARITY )
      {
         localTimeout = BOS_CFG_RESET_GRANULARITY;
      }
      else
      {
         localTimeout = timeRemaining;
      }
      retval = bosMsgQTimedReceive( qid, localTimeout, qMsg );
      if (retval == BOS_STATUS_TIMEOUT)
      {
         bosTimeGetMs(&now);
         bosTimeCalcDeltaMs(&start, &now, &delta);
         if (timeout > delta)
         {
            continue;
         }
         else
         {
            return BOS_STATUS_TIMEOUT;   
         }
      }
      else
      {
         return retval;  /* BOS_STATUS_OK or BOS_STATUS_ERR */
      }
#else
      if (( timeout != BOS_WAIT_FOREVER ) && ( timeout <= BOS_CFG_RESET_GRANULARITY ))
      {
         /* Timeout is less than the reset granularity, queueReceive normally */
         return ( bosMsgQTimedReceive( qid, timeout, qMsg ));
      }
      else
      {
         /* Timeout is more than reset granularity, decrement timeout remaining
         ** and timeout for the maximum allowed before checking for a reset */
         if ( timeout != BOS_WAIT_FOREVER )
         {
            timeout -= BOS_CFG_RESET_GRANULARITY;
         }
         retval = bosMsgQTimedReceive( qid, BOS_CFG_RESET_GRANULARITY, qMsg );
         if ( retval != BOS_STATUS_TIMEOUT )
         {
            /* Anything but the early timeout is returned to the caller */
            return (retval);
         }
      }
#endif
   }
   return ( BOS_STATUS_RESET );

} /* bosQueueReceiveR */

#endif   /* BOS_CFG_RESET_SUPPORT */


/******************************************************************************/

BOS_STATUS bosMsgQInit( void )
{
   BOS_ASSERT( !gBosMsgQInitialized );

   bosMsgQInitFunctionNames();

   /* initialize the message queues */
   memset( &gMsgQ[0], 0, sizeof( gMsgQ ));

   /* create mutex for critical section protection */
   bosMutexCreate( "MsgQ", &gMsgQMutex );

   gBosMsgQInitialized = BOS_TRUE;

   return ( BOS_STATUS_OK );

} /* bosMsgQInit */

/******************************************************************************/

BOS_STATUS bosMsgQTerm( void )
{
   BOS_ASSERT( gBosMsgQInitialized );

   gBosMsgQInitialized = BOS_FALSE;

   /* delete mutex for critical section protection */
   bosMutexDestroy( &gMsgQMutex );

   memset( &gMsgQMutex, 0, sizeof( gMsgQMutex ) );

   return ( BOS_STATUS_OK );

} /* bosMsgQTerm */

/******************************************************************************/

BOS_STATUS bosMsgQSend( BOS_MSGQ *qid, const BOS_MSGQ_MSG *qMsg)
{
   QELEMENT   *qElement;
   BOS_MSGQ    qIndex;
   MSGQ       *msgQ;
   BOS_STATUS  status = BOS_STATUS_ERR;

   BOS_ASSERT( qid != NULL );
   BOS_ASSERT( qMsg );

   /* Message queue IDs are 1-indexed. Convert this to a 0-index base so that we can
    * use this to directly index the array of message queues */
   qIndex   = *qid - 1;
   msgQ     = &gMsgQ[qIndex];


   /* Critical section */
   bosMutexAcquire( &gMsgQMutex );
   {
      /* Get a free queue element */
      qElement = Dequeue( &msgQ->freeChain );

      if ( qElement != NULL )
      {
         /* Copy the message into the queue element */
         memcpy( qElement->msg.buffer, qMsg->buffer, sizeof( BOS_MSGQ_MSG ));

         /* Queue the message */
         Enqueue( &msgQ->pendingChain, qElement );

         status = BOS_STATUS_OK;
      }
   }
   bosMutexRelease( &gMsgQMutex );


   if ( status == BOS_STATUS_OK )
   {
      if ( msgQ->isEventBased )
      {
         bosEventSend( &msgQ->event );
      }
      else
      {
         bosSemGive( &msgQ->sema4 );
      }
   }
   else
   {
      msgQ->noFreeMsgCount++;
      bosLogErr("Free message buffer pool empty - queue id %d", (unsigned int)*qid);
   }

   return ( status );

} /* bosMsgQSend */

/******************************************************************************/

BOS_STATUS bosMsgQReceive( BOS_MSGQ *qid, BOS_MSGQ_MSG *qMsg)
{

   return ( bosMsgQTimedReceive( qid, BOS_WAIT_FOREVER, qMsg ) );

} /* bosMsgQReceive */

/******************************************************************************/

BOS_STATUS bosMsgQTimedReceive( BOS_MSGQ *qid, BOS_TIME_MS timeoutMsec,
                                 BOS_MSGQ_MSG *qMsg)
{
   BOS_STATUS        status = BOS_STATUS_OK;
   QELEMENT         *qElement;
   BOS_MSGQ          qIndex;
   MSGQ             *msgQ;

   BOS_ASSERT( qid != NULL );
   BOS_ASSERT( qMsg );

   /* Message queue IDs are 1-indexed. Convert this to a 0-index base so that we can
    * use this to directly index the array of message queues */
   qIndex   = *qid - 1;
   msgQ     = &gMsgQ[qIndex];

   if ( msgQ->isEventBased )
   {
      /* Critical section */
      bosMutexAcquire( &gMsgQMutex );

      /* If the message queue is empty, then we need to unlock access to the
      ** queue and wait for the event to be posted. */
      if ( msgQ->pendingChain == NULL )
      {
         /* Clear the event in case it was previously posted. */
         bosEventClear( &msgQ->event );

         /* Unlock access to the queue. */
         bosMutexRelease( &gMsgQMutex );

         /* Wait for the event to be posted. */
         status = bosEventTimedReceive( &msgQ->event, timeoutMsec );
         if ( status == BOS_STATUS_EXIT )
         {
            return( status );
         }

         /* Re-lock the message queue. */
         bosMutexAcquire( &gMsgQMutex );
      }

      bosMutexRelease( &gMsgQMutex );

   }
   else
   {
      /* Wait for someone to post a message. */
      status = bosSemTimedTake( &msgQ->sema4, timeoutMsec );
   }

   if ( status != BOS_STATUS_OK )
   {
      return ( status );
   }

   bosMutexAcquire( &gMsgQMutex );
   {
      /* Dequeue the message */
      qElement = Dequeue( &msgQ->pendingChain );
      BOS_ASSERT( qElement );

      /* Get the message */
      memcpy( qMsg->buffer, qElement->msg.buffer, sizeof( BOS_MSGQ_MSG ));

      /* Free the queue element */
      Enqueue( &msgQ->freeChain, qElement );
   }
   bosMutexRelease( &gMsgQMutex );


   return ( BOS_STATUS_OK );

} /* bosMsgQTimedReceive */

/******************************************************************************/

BOS_STATUS bosMsgQCreate( const char *name, unsigned int qSize, BOS_MSGQ *qid )
{
   return ( MsgQCreate( name, qSize, NULL, qid ) );
}


BOS_STATUS bosMsgQCreateEventBased
(
   const char    *name,
   unsigned int   qSize,
   BOS_TASK_ID   *eventTaskId,
   BOS_MSGQ      *qid
)
{
   return ( MsgQCreate( name, qSize, eventTaskId, qid ) );
}

static BOS_STATUS MsgQCreate
(
   const char    *name,
   unsigned int   qSize,
   BOS_TASK_ID   *eventTaskId,
   BOS_MSGQ      *qid
)
{
   int         qIndex;
   char        msgQName[32];

   BOS_ASSERT( name );
   BOS_ASSERT( qid != NULL );

   /* Verify that we can accomodate the specified number of messages in the
    * message queue */
   BOS_ASSERT( qSize <= BOS_CFG_MSGQ_MAX_MSG_PER_MSGQ );


   /* Critical section */
   bosMutexAcquire( &gMsgQMutex );
   {
      /* Find free message queue object */
      for ( qIndex = 0; qIndex < BOS_CFG_MSGQ_MAX_MSGQS; qIndex++ )
      {
         if ( gMsgQ[qIndex].inUse == BOS_FALSE )
         {
            gMsgQ[qIndex].inUse = BOS_TRUE;
            break;
         }
      }
   }
   bosMutexRelease( &gMsgQMutex );

   BOS_ASSERT( qIndex < BOS_CFG_MSGQ_MAX_MSGQS );

   /* Initialize the message queue to accomodate for 'qSize' number of messages */
   InitMsgQueue( qIndex, qSize );

   if ( eventTaskId == NULL )
   {
      /*
      ** Use a sema4 as the synchronization primitive for this message Q.
      */

      sprintf( msgQName, "gMsgQ%d", qIndex );
      bosSemCreate( msgQName, 0, BOS_CFG_MSGQ_MAX_MSG_PER_MSGQ, &gMsgQ[qIndex].sema4 );

      gMsgQ[qIndex].isEventBased = BOS_FALSE;
   }
   else
   {
      /*
      ** Use an event as the synchronization primitive for this message Q.
      ** This allows this message Q to be added to BOS event-sets.
      */

      bosEventCreate( eventTaskId, &gMsgQ[qIndex].event );

      gMsgQ[qIndex].isEventBased = BOS_TRUE;
   }


   gMsgQ[qIndex].noFreeMsgCount = 0;


   /* The returned queue ID is just an index into the array of message queues.
    * The first index will be 1 (not 0), so that the caller doesn't think
    * we are returning a NULL queue ID. */
   *qid = ++qIndex;

   /* Add a log here that will always show up (use error log) no matter what
   ** as we may need this information to correlate to other error.
   **
   ** Going forward, only the queue identifier is kept and used in this module.
   */
   bosLogInfo("Created message queue \"%s\", depth %d, id %u",
         name, qSize, (unsigned int)*qid);

   return ( BOS_STATUS_OK );

} /* bosMsgQCreate */


/******************************************************************************/

BOS_STATUS bosMsgQDestroy( BOS_MSGQ *qid )
{
   /* Message queue IDs are 1-indexed. Convert this to a 0-index base so that we can
    * use this to directly index the array of message queues */
   MSGQ       *msgQ = &gMsgQ[ *qid - 1 ];

   /* Critical section */
   bosMutexAcquire( &gMsgQMutex );
   {
      if ( msgQ->isEventBased )
      {
         bosEventDestroy( &msgQ->event );
      }
      else
      {
         bosSemDestroy( &msgQ->sema4 );
      }

      msgQ->inUse = BOS_FALSE;
   }
   bosMutexRelease( &gMsgQMutex );

   return ( BOS_STATUS_OK );

} /* bosMsgQDestroy */


/***************************************************************************/

BOS_STATUS bosMsgQGetEvent( BOS_MSGQ *qid, BOS_EVENT **event )
{
   /* Message queue IDs are 1-indexed. Convert this to a 0-index base so that we can
    * use this to directly index the array of message queues */
   MSGQ       *msgQ = &gMsgQ[ *qid - 1 ];

   if ( msgQ->isEventBased )
   {
      *event = &msgQ->event;
   }
   else
   {
      BOS_ASSERT( 0 );
   }

   return ( BOS_STATUS_OK );
}


/***************************************************************************/

BOS_STATUS bosMsgQDisplayStats( BOS_MSGQ *qid )
{
   /* Message queue IDs are 1-indexed. Convert this to a 0-index base so that we can
    * use this to directly index the array of message queues */
   MSGQ       *msgQ = &gMsgQ[ *qid - 1 ];

   bosLogErr("No Free Buffer Count: %d", msgQ->noFreeMsgCount);

   /* Clear stats each time this function is called. */
   msgQ->noFreeMsgCount = 0;

   return ( BOS_STATUS_OK );
}



/**
 * @addtogroup bosTimerInternal
 * @{
 */


/*
*****************************************************************************
** FUNCTION:   Dequeue
**
** PURPOSE:    This function dequeues a buffer from a linked list of buffers
**
** PARAMETERS: chain - input - a pointer to the linked list of buffers
**
** RETURNS:    A pointer to the dequeued buffer.  NULL if no buffers are
**             available
**
** NOTE:       This function is NOT thread-safe. It is the caller's
**             responsibility to provide mutual exclusion protection.
*****************************************************************************
*/
static QELEMENT *Dequeue( QELEMENT** chain )
{
   QELEMENT *p;

   /* get buffer from linked list */
   p = *chain;
   if (p)
   {
      *chain = p->next;
   }

   /* return pointer */
   return ( p );

} /* Dequeue */

/*
*****************************************************************************
** FUNCTION:   Enqueue
**
** PURPOSE:    This function queues a buffer into a linked list of buffers
**
** PARAMETERS: chain - input - a pointer to the linked list of buffers
**             bufp  - input - a pointer to the buffer to be queued
**
** RETURNS:    Nothing
**
** NOTE:       This function is NOT thread-safe. It is the caller's
**             responsibility to provide mutual exclusion protection.
*****************************************************************************
*/
static void Enqueue( QELEMENT** chain, QELEMENT *bufp )
{
   /* Add to the tail */
   if( *chain )
   {
      QELEMENT* qbufp = *chain;
      while( qbufp->next )
      {
         qbufp = qbufp->next;
      }
      qbufp->next = bufp;
   }
   else
   {
      *chain = bufp;
   }

   bufp->next = NULL;

} /* Enqueue */

/*
*****************************************************************************
** FUNCTION:   InitMsgQueue
**
** PURPOSE:    This function initializes the buffer pool and linked lists
**             associated with a specific message queue.
**
** PARAMETERS: qIndex - index into global array of message queues, 'gMsgQ'
**             qSize  - number of messages in the message queue to initialize
**
** RETURNS:    Nothing
**
** NOTE:
*****************************************************************************
*/
static void InitMsgQueue( int qIndex, unsigned int qSize )
{
   unsigned int   i;
   MSGQ          *msgQ = &gMsgQ[qIndex];

   /* Critical section */
   bosMutexAcquire( &gMsgQMutex );
   {
      msgQ->freeChain      = NULL;
      msgQ->pendingChain   = NULL;

      for( i = 0; i < qSize; i++ )
      {
         Enqueue( &msgQ->freeChain, &msgQ->bufferPool[i] );
      }
   }
   bosMutexRelease( &gMsgQMutex );

} /* InitMsgQueue */


#endif   /* BOS_CFG_MSGQ */


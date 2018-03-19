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
*  Filename: bosEventLinuxUser.c
*
****************************************************************************
*  Description:
*
*
****************************************************************************/
/**
*
*  @file    bosEventLinuxUser.c
*
*  @brief   LinuxUser implementation of the BOS Event Module
*
****************************************************************************/

/* ---- Include Files ---------------------------------------------------- */

#include <bosCfg.h>     /* include BOS config file */

#if BOS_CFG_EVENT

#include <string.h>
#include <bosTypes.h>
#include <bosError.h>
#include <bosErrorLinuxUser.h>
#include <bosEvent.h>
#include <bosEventPrivate.h>
#include <bosTask.h>
#include <bosCritSect.h>
#include <string.h>
#include <bosLog.h>
#include <bosLog.h>
#include <bosLinuxUser.h>
#include <bosMutex.h>
#include <bosSem.h>

#if !BOS_CFG_TASK
#error bos Event module requires Task module support (not present)
#endif

/* ---- Public Variables ------------------------------------------------- */
/* ---- Private Constants and Types -------------------------------------- */


/* Local task control block. For each task, we need to store the event bits
 * that are currently in use by the application. Each task is limited to
 * receiving 32 events. 
 * NOTE: The above comment is taken from the VxWorks version of bosEvent */
typedef struct
{
   /* NULL taskId = "not used"      */
   BOS_TASK_ID       taskId;

   /* Semaphore used for signalling */
   BOS_SEM           semId;

   /* Indicates free events that are available for use by the application. */
   BOS_UINT32        availableEventBits;

   /* Bits that task is waiting to receive. Any event will unblock task. */
   BOS_UINT32        anyEventBits;

   /* Bits that task is waiting to receive. All events must be receive to unblock. */
   BOS_UINT32        allEventBits;

   /* Event bits that have been sent to task. */
   BOS_UINT32        setEventBits;

   /* Indicates if the task is currently blocked, waiting to receive events. */
   BOS_BOOL          bTaskWaiting;

} BOS_EVENT_TCB;


/* ---- Private Variables ------------------------------------------------ */

static   BOS_BOOL gBosEventInitialized = BOS_FALSE;

/* Local task control block for events */
static BOS_EVENT_TCB gEventTcb[ BOS_CFG_TASK_MAX_TASKS ];

/* Mutex for the event module */
static BOS_MUTEX  gEvtMutex;


/* ---- Private Function Prototypes -------------------------------------- */

static BOS_STATUS GetTaskIndex( BOS_EVENT *event, unsigned int *tIndex );
static BOS_STATUS GetEventBits( BOS_EVENT *event, BOS_UINT32 *eventBits );
static BOS_STATUS NewEvent( BOS_EVENT *event, BOS_TASK_ID *taskId );
static BOS_STATUS DeleteEvent( BOS_EVENT *event );
static BOS_STATUS CheckEventStatusLinuxUser( unsigned long status, BOS_EVENT_FUNC_ID funcId );
static BOS_STATUS EventTimedReceiveLinuxUser
(
   unsigned int               taskIndex,
   BOS_UINT32                 pattern,
   BOS_EVENT_SET_WAIT_MODE    mode,
   BOS_TIME_MS                timeoutMsec,
   BOS_UINT32                *eventsReceived
);

/* helpful debug function */
#if 0 
void PrintEvtTCBTable()
{
   /* Enter critical section */
   bosMutexAcquire( &gEvtMutex );
   int i = 0;
   bosLog("****************************************************");
   bosLog("*************** Event TCB Table ********************");
   bosLog("****************************************************");
   bosLog("Index Tid     Avail      AnyW       AllW       SetB      TaskW");
   for (i = 0; i < BOS_CFG_TASK_MAX_TASKS; i++)
   {
      bosLog("%02d %04x %08x %08x %08x %08x %d",
         i,
         gEventTcb[i].taskId, 
         gEventTcb[i].availableEventBits, 
         gEventTcb[i].anyEventBits, 
         gEventTcb[i].allEventBits, 
         gEventTcb[i].setEventBits, 
         gEventTcb[i].bTaskWaiting);
   }
   bosLog("****************************************************");
   bosMutexRelease( &gEvtMutex );
}
#endif



/* ---- Functions -------------------------------------------------------- */

/*****************************************************************************/

BOS_STATUS bosEventInit( void )
{
   BOS_STATUS  status;

   BOS_ASSERT( !gBosEventInitialized );

   bosEventInitFunctionNames();

   /* create mutex for critical section protection */
   status = bosMutexCreate( "EVMU", &gEvtMutex );
   if ( status != BOS_STATUS_OK )
   {
      bosLogErr("ERROR: bosEventInit() - Unable to create mutex");
      BOS_ASSERT(0);
   }

   gBosEventInitialized = BOS_TRUE;
   memset(gEventTcb, 0, sizeof(gEventTcb));

   return BOS_STATUS_OK;

} /* bosEventInit */

/***************************************************************************/

BOS_STATUS bosEventTerm( void )
{
   BOS_STATUS  status;

   BOS_ASSERT( gBosEventInitialized );

   /* delete mutex for critical section protection */
   status = bosMutexDestroy( &gEvtMutex );
   if ( status != BOS_STATUS_OK )
   {
      bosLogErr("ERROR: bosEventTerm() - Unable to delete mutex");
      BOS_ASSERT(0);
   }
   memset(&gEvtMutex, 0, sizeof(BOS_MUTEX));

   gBosEventInitialized = BOS_FALSE;
   return BOS_STATUS_OK;

} /* bosEventTerm */

/***************************************************************************/
BOS_STATUS bosEventRegisterTask( BOS_TASK_ID *taskId )
{
   int         i;
   BOS_STATUS  status = BOS_STATUS_ERR;
   BOS_SEM     semId;
   BOS_STATUS  rc;

   /* Create semaphor used for signaling */
   rc = bosSemCreate( "EVSM", BOS_CFG_SEM_INIT_COUNT, BOS_CFG_SEM_MAX_COUNT, &semId );
   if ( rc != BOS_STATUS_OK )
   {
      bosLogErr("bosEventRegisterTask() - Unable to create semaphor");
      BOS_ASSERT(0);
   }

   /* Enter critical section */
   bosMutexAcquire( &gEvtMutex );

   /* Find a free TCB entry, and set the initial task state */
   for ( i = 0; i < BOS_CFG_TASK_MAX_TASKS; i++ )
   {
      if ( gEventTcb[i].taskId == 0 )
      {
         /* For VxWorks, all 32-bits can be used. */
         gEventTcb[i].availableEventBits  = 0xffffffff;
         gEventTcb[i].taskId              = *taskId;
         gEventTcb[i].semId               = semId;
         gEventTcb[i].allEventBits        = 0;
         gEventTcb[i].anyEventBits        = 0;
         gEventTcb[i].setEventBits        = 0;
         gEventTcb[i].bTaskWaiting        = BOS_FALSE;
         status = BOS_STATUS_OK;
         break;
      }
   }

   /* Leave critical section */
   bosMutexRelease( &gEvtMutex );

   if ( status != BOS_STATUS_OK )
   {
      bosLogErr("bosEventRegisterTask() - No free entries in TCB list");

      rc = bosSemDestroy( &semId );
      if ( rc != BOS_STATUS_OK )
      {
         bosLogErr("bosEventRegisterTask() - Unable to delete semaphore");
         BOS_ASSERT(0);
      }
   }

   return ( status );

} /* bosEventRegisterTask */

/***************************************************************************/

BOS_STATUS bosEventUnRegisterTask( BOS_TASK_ID *taskId )
{
   int         i;
   BOS_STATUS  status = BOS_STATUS_ERR;
   BOS_STATUS  rc;

   /* Enter critical section */
   bosMutexAcquire( &gEvtMutex );

   /* Find a the TCB entry corresponding to <taskId> */
   for ( i = 0; i < BOS_CFG_TASK_MAX_TASKS; i++ )
   {
      if ( gEventTcb[i].taskId == *taskId )
      {
         /* Found the TCB, delete the semaphor */
         rc = bosSemDestroy( &gEventTcb[i].semId );
         if ( rc != BOS_STATUS_OK )
         {
            bosLogErr("bosEventRegisterTask() - Unable to delete semaphor");
            BOS_ASSERT(0);
         }

         /* clear the task state */
         memset( &gEventTcb[i], 0, sizeof( BOS_EVENT_TCB ) );

         status = BOS_STATUS_OK;
         break;
      }
   }

   /* Leave critical section */
   bosMutexRelease( &gEvtMutex );

   if ( status != BOS_STATUS_OK )
   {
      bosLogErr("bosEventUnRegisterTask() - task does not exist");
   }

   return ( status );

} /* bosEventUnRegisterTask */

/***************************************************************************/

BOS_STATUS bosEventCreate( BOS_TASK_ID *taskId, BOS_EVENT *event )
{
   return ( NewEvent( event, taskId ));
} /* bosEventCreate */

/***************************************************************************/

BOS_STATUS bosEventDestroy( BOS_EVENT *event )
{
   return ( DeleteEvent( event ));
} /* bosEventDestroy */

/***************************************************************************/

BOS_STATUS bosEventTimedReceive( BOS_EVENT *event, BOS_TIME_MS timeoutMsec )
{
   BOS_UINT32        evtBit;
   unsigned int      taskIndex;
   BOS_EVENT_TCB    *evtTcb;
   BOS_UINT32        eventsReceived;
   BOS_TASK_ID       taskId;
   BOS_TASK_ID       evtTaskId;

   BOS_ASSERT( event != NULL );

   /* Get the task Id and event-bits */
   GetTaskIndex( event, &taskIndex );
   GetEventBits( event, &evtBit );
   
   /* check calling taskId, only self received is supported */
   bosTaskGetMyTaskId( &taskId );
   
   /* Enter critical section */
   bosMutexAcquire( &gEvtMutex );
   evtTcb = &gEventTcb[ taskIndex ];
   evtTaskId = evtTcb->taskId; 
   /* Leave critical section */
   bosMutexRelease( &gEvtMutex );
   
   if ( taskId != evtTaskId)
   {
      bosLogErr("ERROR: bosEventTimedReceive - calling taskId (0x%x) does not "
                "correspond to event's taskId (0x%x)",
                (unsigned int)taskId,
                (unsigned int)evtTaskId);
      return ( BOS_STATUS_ERR );
   }
   return ( EventTimedReceiveLinuxUser( taskIndex,
                                      evtBit,
                                      BOS_EVENT_SET_WAIT_MODE_ANY,
                                      timeoutMsec,
                                      &eventsReceived ) );
}


/***************************************************************************/

BOS_STATUS bosEventSend( BOS_EVENT *event )
{
   BOS_STATUS        status = BOS_STATUS_OK;
   BOS_UINT32        evtBit;
   unsigned int      taskIndex;
   BOS_SEM           evtSemId;
   BOS_EVENT_TCB    *evtTcb;

   (void) evtSemId;

   BOS_ASSERT( event != NULL );
   /* Get the task Id and event-bits */
   GetTaskIndex( event, &taskIndex );
   GetEventBits( event, &evtBit );

   /* Enter critical section */
   bosMutexAcquire( &gEvtMutex );
   evtTcb   = &gEventTcb[ taskIndex ];
   evtSemId = evtTcb->semId;

   BOS_TASK_ID tid;
   bosTaskGetMyTaskId(&tid);
   
   /* Create new 'set' events bit-field. */
   evtTcb->setEventBits |= evtBit;
   if ( evtTcb->bTaskWaiting )
   {
      /*
         ** There is already a task waiting for events, check if we should
         ** unblock it...
      */
      if (  (( evtTcb->anyEventBits & evtTcb->setEventBits ) != 0 )
         || (( evtTcb->allEventBits & evtTcb->setEventBits ) == evtTcb->allEventBits ) )
      {
         /* Unblock the waiting task. */
         if ( BOS_STATUS_OK != bosSemGive( &evtTcb->semId ) )
         {
            status = BOS_STATUS_ERR;
         }
         else
         {
            evtTcb->bTaskWaiting = BOS_FALSE;
            status = BOS_STATUS_OK;
         }
      }
   }
   /* Leave critical section */
   bosMutexRelease( &gEvtMutex );

   if ( status != BOS_STATUS_OK )
   {
      status = CheckEventStatusLinuxUser( BOS_STATUS_ERR, BOS_EVENT_SEND_FUNC_ID );
   }
   return ( status );
}


/***************************************************************************/

BOS_STATUS bosEventSetCreate( BOS_TASK_ID *taskId, BOS_EVENT_SET *eventSet )
{
   int         i;
   BOS_STATUS  status = BOS_STATUS_ERR;

   /* Find the TCB corresponding to <taskId> */
   bosMutexAcquire( &gEvtMutex );
   {
      for ( i = 0; i < BOS_CFG_TASK_MAX_TASKS; i++ )
      {
         if ( gEventTcb[i].taskId == *taskId )
         {
            break;
         }
      }
   }
   bosMutexRelease( &gEvtMutex );

   if ( i != BOS_CFG_TASK_MAX_TASKS )
   {
      /* Initially, no events are present. */
      eventSet->eventBits        = 0;
      eventSet->eventsReceived   = 0;
      eventSet->taskIndex        = i;
      status = BOS_STATUS_OK;
   }
   return ( status );
}

/***************************************************************************/

BOS_STATUS bosEventSetDestroy( BOS_EVENT_SET *eventSet )
{
   /* Clear out any remaining events. */
   bosEventSetRemoveAllEvents( eventSet );
   memset( eventSet, 0, sizeof( *eventSet ) );
   return ( BOS_STATUS_OK );
}


/***************************************************************************/

BOS_STATUS bosEventSetAddEvent( BOS_EVENT_SET *eventSet, BOS_EVENT *event )
{
   if ( eventSet->taskIndex != event->taskIndex )
   {
      bosLogErr("ERROR: bosEventSetAddEvent - eventSet taskIndex (0x%x) does not "
                "correspond to event's taskIndex (0x%x)",
                (unsigned int) eventSet->taskIndex,
                (unsigned int) event->taskIndex );
      return ( BOS_STATUS_ERR );
   }
   /* Add this event's bit to the set. */
   eventSet->eventBits |= event->eventBit;
   return( BOS_STATUS_OK );
}


/***************************************************************************/

BOS_STATUS bosEventSetRemoveEvent( BOS_EVENT_SET *eventSet, BOS_EVENT *event )
{
   /* Remove this event's bit from the set. */
   eventSet->eventBits &= ~( event->eventBit );
   return( BOS_STATUS_OK );
}


/***************************************************************************/

BOS_STATUS bosEventSetRemoveAllEvents( BOS_EVENT_SET *eventSet )
{
   /* Clear all of the bits. */
   eventSet->eventBits = 0;
   return ( BOS_STATUS_OK );
}


/***************************************************************************/

BOS_STATUS bosEventSetTimedReceive
(
   BOS_EVENT_SET             *eventSet,
   BOS_EVENT_SET_WAIT_MODE    mode,
   BOS_TIME_MS                timeoutMsec
)
{
   BOS_TASK_ID    taskId;
   BOS_TASK_ID    evtSetTaskId;

   BOS_ASSERT( eventSet != NULL );
   bosTaskGetMyTaskId( &taskId );
   /* Enter critical section */
   bosMutexAcquire( &gEvtMutex );
   evtSetTaskId = gEventTcb[eventSet->taskIndex].taskId;
   /* Leave critical section */
   bosMutexRelease( &gEvtMutex );
   
   if ( taskId != evtSetTaskId )
   {
      bosLogErr( "ERROR: bosEventSetTimedReceive - calling taskId (0x%x) does not "
                "correspond to event set's taskId (0x%x) taskIndex = (0x%x)",
                (unsigned int)taskId,
                (unsigned int)evtSetTaskId, (unsigned int)eventSet->taskIndex );
      return ( BOS_STATUS_ERR );
   }


   return ( EventTimedReceiveLinuxUser( eventSet->taskIndex,
                                      eventSet->eventBits,
                                      mode,
                                      timeoutMsec,
                                      &eventSet->eventsReceived ) );
}


/***************************************************************************/

BOS_BOOL bosEventSetIsEventSet( BOS_EVENT_SET *eventSet, BOS_EVENT *event )
{
   BOS_BOOL result;

   /* See if the bit was set the last time we returned from bosEventSetReceive(). */
   if ( eventSet->eventsReceived & event->eventBit )
   {
      result = BOS_TRUE;
      /* Clear the bit. */
      eventSet->eventsReceived &= ~( event->eventBit );
   }
   else
   {
      result = BOS_FALSE;
   }
   return ( result );
}


/*
** Static helper functions
*/

/***************************************************************************/
/**
*  Create a new event object
*
*  @param   event      (out)  created event
*  @param   taskId     (in)   target task identifier. The created event will be sent
*                             to this task when bosEventSend is invoked.
*/
static BOS_STATUS NewEvent( BOS_EVENT *event, BOS_TASK_ID *taskId )
{
   int            i;
   BOS_STATUS     status = BOS_STATUS_ERR;
   unsigned long  eventBit = 0;
   int            eventNum = 0;

   BOS_ASSERT( event != NULL );

   /* Enter critical section */
   bosMutexAcquire( &gEvtMutex );

   /* Find the TCB corresponding to <taskId> */
   for ( i = 0; i < BOS_CFG_TASK_MAX_TASKS; i++ )
   {
      if ( gEventTcb[i].taskId == *taskId )
      {
         /* Found the TCB. Get the next free event for this task. */
         eventBit = 0x01;
         while ((( gEventTcb[i].availableEventBits & eventBit ) == 0 ) && ( eventBit != 0 ))
         {
            eventBit <<= 1;
            eventNum++;
         }

         /* Make sure this yielded a valid bit */
         if ( eventBit != 0 )
         {
            /* Mark the bit as unavailable */
            gEventTcb[i].availableEventBits &= ~eventBit;
            status = BOS_STATUS_OK;
         }

         break;
      }
   }

   /* Leave critical section */
   bosMutexRelease( &gEvtMutex );

   if ( status != BOS_STATUS_OK )
   {
      bosLogErr("newEvent() - no more free event for this task");
      memset( event, 0, sizeof( *event ) );
   }
   else
   {
      /* The event object is actually a bit-field consisting of an index into
       * the TCB and the actual event-bit. */
      event->eventBit   = eventBit;
      event->taskIndex  = i;
   }

   return ( status );

} /* NewEvent */

/***************************************************************************/
/**
*  Delete an event object
*
*  @param   event     (in)   event to delete
*/
static BOS_STATUS DeleteEvent( BOS_EVENT *event )
{
   BOS_UINT32     eventBit;
   unsigned int   taskIndex;

   BOS_ASSERT( event != NULL );

   /* Get the task-index and event value */
   GetTaskIndex( event, &taskIndex );
   GetEventBits( event, &eventBit );

   /* Enter critical section */
   bosMutexAcquire( &gEvtMutex );

   gEventTcb[ taskIndex ].availableEventBits |= eventBit;

   /* Leave critical section */
   bosMutexRelease( &gEvtMutex );

   return ( BOS_STATUS_OK );

} /* DeleteEvent */

/***************************************************************************/
/**
*  Retrieves the TCB index associated with an event
*
*  @param   event      (in)   event handle
*  @param   taskIndex  (out)  index into TCB array associated with the
*                             task that will receive this event
*/
static BOS_STATUS GetTaskIndex( BOS_EVENT *event, unsigned int *tIndex )
{
   *tIndex = event->taskIndex;

   return ( BOS_STATUS_OK );
}


/***************************************************************************/
/**
*  Retrieves the event value (bit-field) associated with an event handle
*
*  @param   event      (in)   event handle
*  @param   eventBits  (out)  event-bits associated with the event handle
*/
static BOS_STATUS GetEventBits( BOS_EVENT *event, BOS_UINT32 *eventBits )
{
   *eventBits = event->eventBit;

   return ( BOS_STATUS_OK );
}


/***************************************************************************/
/**
*  Translate a Linux User error code into a BOS error code and report any
*  errors encountered.
*
*  @param   err      (in)  Linux User Error code.
*  @param   funcId   (in)  ID of the function that we're checking for.
*/

static BOS_STATUS CheckEventStatusLinuxUser( unsigned long status, BOS_EVENT_FUNC_ID funcId )
{
   return bosErrorCheckStatusLinuxUser( status,
                                   BOS_MAKE_ERROR_SOURCE( BOS_EVENT_MODULE, funcId ));

} /* CheckEventStatusLinuxUser */

/***************************************************************************/
/**
*  Wait for an event to be sent to the calling task.
*
*  @param   evtTcb         (mod) Event TCB state.
*           pattern        (in)  Bit pattern that will cause the calling task to be
*                                woken up.
*           mode           (in)  Wait for any event, or all events.
*           timeout        (in)  Time to wait for conditions to be met.
*           eventsReceived (out) Events that occurred.
*
*  @return  BOS_STATUS_OK if the event was received, BOS_STATUS_TIMEOUT if
*           the timeout period expired, or an error code if the event could
*           not be received for another reason.
*/

static BOS_STATUS EventTimedReceiveLinuxUser
(
   unsigned int               taskIndex,
   BOS_UINT32                 pattern,
   BOS_EVENT_SET_WAIT_MODE    mode,
   BOS_TIME_MS                timeoutMsec,
   BOS_UINT32                *eventsReceived
)
{
   BOS_STATUS        status = BOS_STATUS_OK;
   BOS_SEM           *evtSemId;
   int               rc;
   BOS_EVENT_TCB    *evtTcb;

   /* Enter critical section */
   bosMutexAcquire( &gEvtMutex );
   evtTcb = &gEventTcb[ taskIndex ];
   
   /* check if task is unregistered between calls */
   if (evtTcb->taskId == 0) 
   {
      bosLogErr("ERROR: bosEventTimedReceive() - , task is unregistered.");
      /* Leave critical section */
      bosMutexRelease( &gEvtMutex );
      return BOS_STATUS_ERR;
   }
   
   evtSemId          = &(evtTcb->semId);
   *eventsReceived   = 0;
   
   /*
   ** Check if the requested event bits have already been set...
   */

   if ( mode == BOS_EVENT_SET_WAIT_MODE_ANY )
   {
      if ( ( pattern & evtTcb->setEventBits ) != 0 )
      {
         *eventsReceived = evtTcb->setEventBits;
      }
   }
   else if ( mode == BOS_EVENT_SET_WAIT_MODE_ALL )
   {
      if (  ( pattern != 0 )
         && (( pattern & evtTcb->setEventBits ) == pattern ) )
      {
         *eventsReceived = evtTcb->setEventBits;
      }
   }

   if ( *eventsReceived != 0 )
   {
      /* Requested events are already set, we don't need to block, simply
      ** update TCB state, and return. */
      evtTcb->setEventBits &= ~( pattern );
      /* Leave critical section */
      bosMutexRelease( &gEvtMutex );
      return ( BOS_STATUS_OK );
   }


   /*
   ** The requested events are not yet set. Store the 'any' or 'all'
   ** event bits, and then we'll block waiting for these events to occur.
   */
   if ( mode == BOS_EVENT_SET_WAIT_MODE_ANY )
   {
      evtTcb->anyEventBits = pattern;
      evtTcb->allEventBits = 0;
   }
   else if ( mode == BOS_EVENT_SET_WAIT_MODE_ALL )
   {
      evtTcb->anyEventBits = 0;
      evtTcb->allEventBits = pattern;
   }

   evtTcb->bTaskWaiting = BOS_TRUE;
   /* Leave critical section */
   bosMutexRelease( &gEvtMutex );

   /*
   ** Block waitint for events to occur.
   */
   rc = bosSemTimedTake( evtSemId, timeoutMsec );

   /* Enter critical section */
   bosMutexAcquire( &gEvtMutex );
   /* check if the semaphor had timed out */
   if ( rc == BOS_STATUS_TIMEOUT )
   {
      /* Semaphor timed out. Check both the event and semaphore states in case
      ** the semaphore was given just after we timed out but were still in
      ** the WAITING state. */

      if ( evtTcb->bTaskWaiting )
      {
         /*
         ** We are still in the WAITING state, change back to NONE
         */
         evtTcb->bTaskWaiting = BOS_FALSE;
      }
      else
      {
         /*
         ** A sender changed the state right when we timed-out. To keep the
         ** semaphore in sync with the state, we will take it now (non-blocking)
         ** and return that status instead of a timeout. This is because
         ** the sender only gave the semaphore believing that someone was
         ** in the WAITING state to consume the sem with a semTake. If the sem
         ** is left in the given state, the event state will get out of sync.
         ** ie. A second evSend would change to the state to GIVEN, while the
         ** sem is also in a given state.
         */
         rc = bosSemTimedTake( evtSemId, 0 );
      }
   }
   if ( rc == BOS_STATUS_OK )
   {
      *eventsReceived       = evtTcb->setEventBits;
      evtTcb->setEventBits &= ~( pattern );
   }
   /* Enter critical section */
   bosMutexRelease( &gEvtMutex );

   if ( rc != BOS_STATUS_OK && rc != BOS_STATUS_TIMEOUT )
   {
      bosLogErr("ERROR: bosEventTimedReceive() - error");
      status = CheckEventStatusLinuxUser( BOS_STATUS_ERR, BOS_EVENT_RECEIVE_FUNC_ID );
   }
   else
   {
      status = rc;
   }
   return ( status );
}  /* EventTimedReceiveLinuxUser */

#endif /* #if BOS_CFG_EVENT */


/****************************************************************************
*
* <:copyright-BRCM:2002:proprietary:standard
* 
*    Copyright (c) 2002 Broadcom 
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
*****************************************************************************/
/**
*
*  @file    bosTaskGeneric.c
*
*  @brief   Contains OS independant definitions for the BOS Task module.
*
****************************************************************************/

/* ---- Include Files ---------------------------------------------------- */

#include <bosCfg.h>

#if BOS_CFG_TASK

#include <bosLog.h>

#include <bosError.h>
#include <bosTask.h>
#include <bosTaskPrivate.h>
#include <string.h>     /* for memcpy */
#include <bosMutex.h>


#if BOS_CFG_RESET_SUPPORT

#include <bosSem.h>
#include <bosEvent.h>
#include <bosSleep.h>

#endif   /* BOS_CFG_RESET_SUPPORT */

/**
 * @addtogroup bosTask
 * @{
 */

/* ---- Public Variables ------------------------------------------------- */

#if BOS_CFG_ERROR_FUNCTION_STRINGS

const char *gBosTaskFunctionName[ BOS_TASK_NUM_FUNC_ID ];

#endif   /* BOS_CFG_ERROR_FUNCTION_STRINGS */


/* ---- Private Constants and Types -------------------------------------- */

#define BOS_TASK_RESURRECT       0

#define BOS_TCB_MUTEX            "aoTM"
#define BOS_APPTASK_NAME         "aoAP"

/* Maximum number of tasks in the system */
#define MAX_TCB                 BOS_CFG_TASK_MAX_TASKS


#define PUBLIC
#define PRIVATE static


/* ---- Reset-specific defines --- */

#if BOS_CFG_RESET_SUPPORT

#define BOS_TASKRESET_SEM        "aoTR"
#define BOS_APPRESET_SEM         "aoAR"
#define BOS_APPRESETCOMPLETE_SEM "aoRC"
#define BOS_ROOTTASK_NAME        "aoRT"

/* BOS reset task states */
typedef enum
{
   BOS_TASKSTATE_NORMAL,         /* The task is in the normal running state   */

   BOS_TASKSTATE_UNINTERRUPTIBLE,/* Un-interruptible tasks are not notified
                                  * of application resets                     */

   BOS_TASKSTATE_SUSPENDED,      /* The task is suspended                     */

   BOS_TASKSTATE_RESET_NTFY,     /* The task has been notified of an application
                                  * reset. BOS is waiting for an ACK.   */

   BOS_TASKSTATE_RESET_ACKED,    /* The task has ACKed the application reset
                                  * notify and is blocked, waiting to be placed
                                  * into shutdown state                       */

   BOS_TASKSTATE_SHUTDOWN,       /* The task is running in shutdown mode. It
                                  * should promptly shutdown and exit from
                                  * it's main task function.                  */

   BOS_TASKSTATE_DONE,           /* The task is suspended after completing
                                  * the task de-initialization function.
                                  * It is waiting to be re-spawned.           */

} BOS_TASKSTATE;


/* Reset detection states */
typedef enum
{
   BOS_RESETDETECT_DISABLED = 0,
   BOS_RESETDETECT_ENABLED,
   BOS_RESETDETECT_INHIBITED
} BOS_RESETDETECTMODE;


/* Reset states */
typedef enum
{
   BOS_RESETSTATE_NORESET = 0,
   BOS_RESETSTATE_RESETTING,
   BOS_RESETSTATE_POSTPONED
} BOS_RESETSTATE;


/* Synchronization event object types used by each task */
typedef enum _BOS_TASK_EVENT
{
   BOS_TASKSHUTDOWN_EVENT,    /* Tasks in the reset-ack state wait on this event
                               * until they are placed into shutdown mode.    */
   BOS_RESET_ACK_EVENT,       /* Tasks individually resetting other tasks wait
                               * on this event until the resetting task acks. */
   BOS_RESURRECT_EVENT,       /* Tasks which are "done" wait on this event until
                               * they are re-spawned. */
   BOS_RESUME_EVENT,          /* Suspended tasks wait on this event until they
                               * are resumed. */
   BOS_TASKSTART_EVENT,       /* Tasks started suspended wait on this event until
                               * they are 'started' by TaskStart() */
   BOS_MAX_EVENTS
} BOS_TASK_EVENT;

#endif   /* BOS_CFG_RESET_SUPPORT */


/* Task control block */
typedef struct
{
   char                 name[ BOS_CFG_TASK_MAX_TASK_NAME_LEN ];   /* Task name                     */
   BOS_TASK_INIT_FNC    init;             /* Task init function            */
   BOS_TASK_ENTRY       main;             /* Task main function            */
   BOS_TASK_DEINIT_FNC  deinit;           /* Task deinit function          */
   BOS_TASK_ID          taskid;           /* Task handle                   */
   BOS_BOOL             inuse;            /* Indicates if TCB is free      */

#if BOS_CFG_RESET_SUPPORT
   BOS_TASKSTARTMODE    startMode;        /* Start SUSPENDED or RUNNING    */
   BOS_TASKSTATE        state;            /* Current task state            */
   BOS_TASKSTATE        savedState;       /* Saved task state, restored
                                           * after a suspended task is
                                           * resumed.                      */
   BOS_EVENT    events[ BOS_MAX_EVENTS ]; /* Task event handles            */
   BOS_BOOL             reset;
#endif

   BOS_TASK_ID          resetSrcTaskid;   /* Source of an individual reset */
} BOSZTCB;

/* Application task state */
typedef struct
{
   const char          *name;         /* App task name                 */
   int                  stackSize;     /* App task stack size           */
   BOS_APP_INIT_FNC     init;          /* App task init func            */
   BOS_APP_DEINIT_FNC   deinit;        /* App task de-init func         */
   BOS_TASK_ARG         taskArg;       /* App task main func argument   */
   BOS_TASK_ID          taskId;        /* App task handle               */
   BOS_TASK_CLASS       taskClass;     /* App task priority             */
} BOSZAPPTASK;


/* Application OS control block */
typedef struct
{
   BOSZTCB              tcb[ MAX_TCB ];   /* TCB list                           */
   BOS_MUTEX            tcbMutexId;       /* Mutex to protect TCB access        */
   BOSZAPPTASK          appTask;          /* App task state                     */
   BOS_TASK_ID          rootTaskId;       /* Root task handle                   */

#if BOS_CFG_RESET_SUPPORT
   BOS_SEM              appResetSemId;    /* Wait for an app reset              */
   BOS_SEM              appResetCompletedSemId;   /* Wait for reset completion          */
   BOS_SEM              taskResetSemId;   /* Wait for reset ACKS from all tasks */
   BOS_RESETSTATE       resetState;       /* reset in progress/outstanding/etc  */
   BOS_RESETDETECTMODE  resetDetectMode;  /* Reset detection mode               */
#endif

} BOSZCB;


/* Local flag to indicate that the TCB mutex was initialized */
static BOS_BOOL bTcbMutexInit     = BOS_FALSE;

#if BOS_CFG_RESET_SUPPORT

/* Local flags to indicate that the app semaphores were initialized */
static BOS_BOOL bAppResetSemInit         = BOS_FALSE;
static BOS_BOOL bAppResetCompletSemInit  = BOS_FALSE;
static BOS_BOOL bTaskResetSemInit        = BOS_FALSE;

#endif


/* ---- Private Variables ------------------------------------------------ */

/**
 * Table used to translate reserved BOS_TASK_CLASS classes into native priorities.
 */

static BOS_TASK_PRIORITY gTaskPriorities[ BOS_TASK_NUM_CLASSES ] =
{
   BOS_CFG_TASK_HIGH_VAL,
   BOS_CFG_TASK_MED_HIGH_VAL,
   BOS_CFG_TASK_MED_VAL,
   BOS_CFG_TASK_MED_LOW_VAL,
   BOS_CFG_TASK_LOW_VAL
};


PRIVATE BOSZCB boszcb;


/* ---- Private Function Prototypes -------------------------------------- */

/* TCB function prototypes */
PRIVATE BOS_STATUS   AllocTCB( BOSZTCB **tcb );
PRIVATE BOS_STATUS   FreeTCB( BOSZTCB *tcb );
PRIVATE BOS_STATUS   GetTCB( BOS_TASK_ID *taskid, BOSZTCB **tcb );
#if BOS_CFG_RESET_SUPPORT
#if BOS_TASK_RESURRECT
PRIVATE BOS_STATUS   GetNamedTCB( const char *name, BOSZTCB **tcb );
#endif /* BOS_TASK_RESURRECT */
#endif

#if !BOS_CFG_RESET_SUPPORT
/* App task main template function (with no reset support) */
PRIVATE void         AppTemplateTaskMain( BOS_TASK_ARG taskArg );
#endif

#if BOS_CFG_RESET_SUPPORT

PRIVATE const char  *GetTaskName( BOS_TASK_ID *taskid );
PRIVATE const char  *GetTaskNameSelf( void );
PRIVATE BOS_STATUS   SetTaskState( BOS_TASK_ID *taskid, BOS_TASKSTATE state );
PRIVATE BOS_STATUS   SetTaskStateSelf( BOS_TASKSTATE state );
PRIVATE BOS_STATUS   SaveTaskState( BOS_TASK_ID *taskid );
PRIVATE BOS_STATUS   GetTaskState( BOS_TASK_ID *taskid, BOS_TASKSTATE *state );
PRIVATE BOS_STATUS   RestoreTaskState( BOS_TASK_ID *taskid );
PRIVATE BOS_BOOL     IsResetPendingCheck( BOS_BOOL isResource );
#if BOS_TASK_RESURRECT
PRIVATE BOS_STATUS   TaskEnterDone( void );
PRIVATE BOS_STATUS   TaskResurrect( BOSZTCB *tcb );
#endif /* BOS_TASK_RESURRECT */
PRIVATE BOS_STATUS   TaskEnterShutdown( void );
PRIVATE BOS_STATUS   TaskResetNotifyAll( void );
PRIVATE void         AppResetRootTask( BOS_TASK_ARG taskArg );
PRIVATE BOS_STATUS   SignalEvent( BOS_TASK_EVENT eventid, BOS_TASK_ID *taskid );
PRIVATE BOS_STATUS   WaitForEvent( BOS_TASK_EVENT eventid );
PRIVATE void         AppResetTemplateTaskMain( BOS_TASK_ARG taskArg );
PRIVATE BOS_STATUS   GetTaskStateSelf( BOS_TASKSTATE *state );

#endif   /* BOS_CFG_RESET_SUPPORT */


/*
** Sem create and delete macros with error checks
*/

/* Check if there was an error before creating the sem */
#define BOS_SEMCREATE( errcode, name, semid, bInitialized )    \
   if ( errcode == BOS_STATUS_OK )                             \
   {                                                           \
      errcode = bosSemCreate( name, BOS_CFG_SEM_INIT_COUNT,    \
                              BOS_CFG_SEM_MAX_COUNT, semid );  \
      bInitialized = BOS_TRUE;                                 \
   }

/* Check if a sem exists before deleting */
#define BOS_SEMDELETE( bInitialized, semid )    \
   if ( bInitialized == BOS_TRUE )              \
   {                                            \
      bosSemDestroy( semid );                   \
   }


/*
** Mutex create and delete macros with error checks
*/

/* Check if there was an error before creating the mutex */
#define BOS_MUCREATE( errcode, name, mutexid, bInitialized )   \
   if ( errcode == BOS_STATUS_OK )                             \
   {                                                           \
      errcode = bosMutexCreate( name, mutexid );               \
      bInitialized = BOS_TRUE;                                 \
   }

/* Check if a mutex exists before deleting */
#define BOS_MUDELETE( bInitialized, mutexid )      \
   if ( bInitialized == BOS_TRUE )                 \
   {                                               \
      bosMutexDestroy( mutexid );                  \
   }


#if BOS_CFG_RESET_SUPPORT

/*
** Event create and delete macros with error checks
*/

/* Check if there was an error before creating the event */
#define BOS_EVENTCREATE( errcode, eventid, bInitialized, taskid )                \
   if ( errcode == BOS_STATUS_OK )                                               \
   {                                                                             \
      errcode = bosEventCreate( taskid, eventid );                               \
      bInitialized = BOS_TRUE;                                                   \
   }                                                                             \
   else                                                                          \
   {                                                                             \
      bosLogErr("Not creating event: " #eventid );                               \
   }

/* Check if a event exists before deleting */
#define BOS_EVENTDELETE( eventid, bInitialized )      \
   if ( bInitialized )                                \
   {                                                  \
      bosEventDestroy( eventid );                     \
   }


/* Generic error condition check and LOG macro */
#define BOS_TASK_LOG_ERR_CONDITION( cond )   \
      if ( cond )                            \
      {                                      \
         bosLogErr( #cond );                 \
      }


#endif   /* BOS_CFG_RESET_SUPPORT */


/* ---- Functions -------------------------------------------------------- */

#if BOS_CFG_ERROR_FUNCTION_STRINGS

/***************************************************************************/
/**
*  Initializes the contents of @a gBosTaskFunctionName, which is used to
*  translate function IDs into character strings.
*/

void bosTaskInitFunctionNames( void )
{
   gBosTaskFunctionName[ BOS_TASK_INIT_FUNC_ID ]             = "bosTaskInit";
   gBosTaskFunctionName[ BOS_TASK_TERM_FUNC_ID ]             = "bosTaskTerm";
   gBosTaskFunctionName[ BOS_TASK_CREATE_FUNC_ID ]           = "bosTaskCreate";
   gBosTaskFunctionName[ BOS_TASK_DESTROY_FUNC_ID ]          = "bosTaskDestroy";
   gBosTaskFunctionName[ BOS_TASK_SUSPEND_FUNC_ID ]          = "bosTaskSuspend";
   gBosTaskFunctionName[ BOS_TASK_RESUME_FUNC_ID ]           = "bosTaskResume";
   gBosTaskFunctionName[ BOS_TASK_EXISTS_FUNC_ID ]           = "bosTaskExists";
   gBosTaskFunctionName[ BOS_TASK_GET_PRIORITY_FUNC_ID ]     = "bosTaskGetPriority";
   gBosTaskFunctionName[ BOS_TASK_SET_PRIORITY_FUNC_ID ]     = "bosTaskSetPriority";
   gBosTaskFunctionName[ BOS_TASK_GET_MY_TASK_ID_FUNC_ID ]   = "bosTaskGetMyTaskId";
   gBosTaskFunctionName[ BOS_TASK_GET_ID_FROM_NAME_FUNC_ID ] = "bosTaskGetIdFromName";
   gBosTaskFunctionName[ BOS_TASK_GET_NAME_FROM_ID_FUNC_ID ] = "bosTaskGetNameFromId";

   bosErrorAddFunctionStrings( BOS_TASK_MODULE, gBosTaskFunctionName, BOS_TASK_NUM_FUNC_ID );

} /* bosTaskInitFunctioNames */

#endif   /* BOS_CFG_ERROR_FUNCTION_STRINGS */

/***************************************************************************/

BOS_STATUS bosTaskSetClassPriorities( BOS_TASK_CLASS_PRIORITY_TABLE *priorities )
{
   memcpy(  &gTaskPriorities[0],
            &priorities->priorityLevel[0],
            sizeof( gTaskPriorities ));

   return ( BOS_STATUS_OK );
}

/***************************************************************************/

BOS_TASK_PRIORITY bosTaskCalcNativePriority( BOS_TASK_CLASS taskClass )
{
   if ( BOS_TASK_CLASS_IS_RESERVED( taskClass ))
   {
      /*
       * The priority is one of the predefined enumerated ones, use the
       * lookup table to translate.
       */

      return gTaskPriorities[ BOS_TASK_CLASS_TO_INDEX( taskClass )];
   }

   /*
    * The priority wasn't one of the reserved ones. Assume that it's an
    * OS native task priority.
    */

   return (BOS_TASK_PRIORITY)taskClass;

}

/***************************************************************************/

BOS_TASK_CLASS bosTaskCalcTaskClass( BOS_TASK_PRIORITY priority )
{
   unsigned int   i;

   for ( i = 0; i < BOS_ARRAY_LEN( gTaskPriorities ); i++ )
   {
      if ( gTaskPriorities[ i ] == priority )
      {
         return BOS_TASK_INDEX_TO_CLASS( i );
      }
   }

   /*
    * Not one of the predefined ones, must be user defined.
    */

   return (BOS_TASK_CLASS)priority;

}

/** @} */





/***************************************************************************/


/**
 * @addtogroup bosTaskReset
 * @{
 */

/***************************************************************************/
PUBLIC BOS_STATUS bosTaskCreateMain
(
   const char            *name,
   int                   stackSize,
   BOS_TASK_CLASS        taskClass,
   BOS_TASK_INIT_FNC     taskInit,
   BOS_TASK_ENTRY        taskMain,
   BOS_TASK_DEINIT_FNC   taskDeinit,
   BOS_TASK_EXTENDED_ARG *argument,
   BOS_TASK_ID           *taskId,
   BOS_BOOL              bUnInterruptible,
   BOS_TASKSTARTMODE     taskStartMode
)
{
   BOSZTCB       *tcb;
   BOS_STATUS     retval;

#if BOS_CFG_RESET_SUPPORT
   BOS_BOOL       bEventInitialized[ BOS_MAX_EVENTS ];
#endif

#if !BOS_CFG_RESET_SUPPORT
   (void) bUnInterruptible;
#endif
#if !BOS_CFG_RESET_SUPPORT
   (void) taskStartMode;
#endif

   bosLogInfo("Enter TaskCreate %s", name);

   /* Enter critical section */
   bosMutexAcquire( &boszcb.tcbMutexId );

#if BOS_CFG_RESET_SUPPORT

   /* Creation of new tasks is not allowed during application reset since
    * the newly created task will never be notified of the reset
    * (so task creation is only allowed during NORESET or POSTPONED). */
   if ( boszcb.resetState == BOS_RESETSTATE_RESETTING )
   {
      bosLogErr("Can't create task (%s) during a reset!", name);
      bosMutexRelease( &boszcb.tcbMutexId );
      return ( BOS_STATUS_RESET );
   }
#endif

   /*
   ** Create the task from scratch
   */
   retval = AllocTCB( &tcb );
   if ( retval != BOS_STATUS_OK )
   {
      bosMutexRelease( &boszcb.tcbMutexId );
      return ( retval );
   }

   strncpy( tcb->name, name, BOS_CFG_TASK_MAX_TASK_NAME_LEN - 1 );
   tcb->name[BOS_CFG_TASK_MAX_TASK_NAME_LEN - 1] = '\0';
   tcb->init = taskInit;
   tcb->main = taskMain;
   tcb->deinit = taskDeinit;

#if BOS_CFG_RESET_SUPPORT

   tcb->startMode = taskStartMode;
   tcb->reset = BOS_TRUE;

   /* Need to determine if this is an Un-interruptible task. */
   if ( bUnInterruptible )
   {
      tcb->state = BOS_TASKSTATE_UNINTERRUPTIBLE;
   }
   else
   {
      tcb->state = BOS_TASKSTATE_NORMAL;
   }

#endif

   /*
   ** Spawn the task.
   */
   bosLogInfo("TaskCreate - spawn new task %s", name);

   /* Pass in tcb.taskid since it must be valid as soon as the task is created
    * because it will be used by the task to check it's own state */
   retval = bosTaskCreate( name,
                           stackSize,
                           taskClass,
#if BOS_CFG_RESET_SUPPORT
                           AppResetTemplateTaskMain,
#else
                           AppTemplateTaskMain,
#endif
                           argument,
                           &tcb->taskid );

#if BOS_CFG_RESET_SUPPORT

   /*
   ** Create tasks's event synchronization objects
   */
   BOS_EVENTCREATE( retval, &tcb->events[ BOS_TASKSHUTDOWN_EVENT ],
                    bEventInitialized[ BOS_TASKSHUTDOWN_EVENT ], &tcb->taskid );
   BOS_EVENTCREATE( retval, &tcb->events[ BOS_RESURRECT_EVENT ],
                    bEventInitialized[ BOS_RESURRECT_EVENT ], &tcb->taskid );
   BOS_EVENTCREATE( retval, &tcb->events[ BOS_RESUME_EVENT ],
                    bEventInitialized[ BOS_RESUME_EVENT ], &tcb->taskid );
   BOS_EVENTCREATE( retval, &tcb->events[ BOS_TASKSTART_EVENT ],
                    bEventInitialized[ BOS_TASKSTART_EVENT ], &tcb->taskid );
   BOS_EVENTCREATE( retval, &tcb->events[ BOS_RESET_ACK_EVENT ],
                    bEventInitialized[ BOS_RESET_ACK_EVENT ], &tcb->taskid );

   if ( retval != BOS_STATUS_OK )
   {
      bosLogErr("TaskCreate: Initialization error, cleaning up task state");

      /* Cleanup anything created before returning */
      BOS_EVENTDELETE( &tcb->events[ BOS_TASKSHUTDOWN_EVENT ],
                       bEventInitialized[ BOS_TASKSHUTDOWN_EVENT ]  );
      BOS_EVENTDELETE( &tcb->events[ BOS_RESURRECT_EVENT ],
                       bEventInitialized[ BOS_RESURRECT_EVENT ]     );
      BOS_EVENTDELETE( &tcb->events[ BOS_RESUME_EVENT ],
                       bEventInitialized[ BOS_RESUME_EVENT ]        );
      BOS_EVENTDELETE( &tcb->events[ BOS_TASKSTART_EVENT ],
                       bEventInitialized[ BOS_TASKSTART_EVENT ]     );
      BOS_EVENTDELETE( &tcb->events[ BOS_RESET_ACK_EVENT ],
                       bEventInitialized[ BOS_RESET_ACK_EVENT ]     );

      FreeTCB( tcb );
   }
   else
#endif
   {
      /* If the create succeeded, set the callers taskid */
      *taskId = tcb->taskid;
   }

   /* Leave critical section */
   bosMutexRelease( &boszcb.tcbMutexId );

   bosLogInfo("Exit TaskCreate");
   return ( retval );
}

#if BOS_CFG_RESET_SUPPORT
/***************************************************************************/
PUBLIC BOS_STATUS bosTaskSetNoReset ( BOS_TASK_ID *taskId )
{
   BOS_STATUS retval = BOS_STATUS_OK;
   BOSZTCB *t;

   /* Enter critical section */
   bosMutexAcquire( &boszcb.tcbMutexId );

   retval = GetTCB( taskId, &t );
   if ( retval != BOS_STATUS_OK )
   {
      /* Leave critical section */
      bosMutexRelease( &boszcb.tcbMutexId );
      return ( retval );
   }

   t->reset = BOS_FALSE;

   /* Leave critical section */
   bosMutexRelease( &boszcb.tcbMutexId );
   return ( retval );
}

/***************************************************************************/
PUBLIC BOS_STATUS bosTaskDestroyR( BOS_TASK_ID *taskId )
{
   BOSZTCB      *tcb;
   BOS_STATUS   status;

   bosLog("Enter bosTaskDestoryR, taskId = %08x", *taskId);

   /* Enter critical section */
   bosMutexAcquire( &boszcb.tcbMutexId );

   /* Self-identify ( taskid is zero ) */
   if ( taskId == 0 )
   {
      status = bosTaskGetMyTaskId( taskId );
      if ( status != BOS_STATUS_OK )
      {
         bosMutexRelease( &boszcb.tcbMutexId );
         return ( status );
      }
   }

   status = GetTCB( taskId, &tcb );

   if ( status != BOS_STATUS_OK )
   {
      bosMutexRelease( &boszcb.tcbMutexId );
      return ( status );
   }

   /*
   ** Free the local storage associated with the task
   */

   BOS_EVENTDELETE( &tcb->events[ BOS_TASKSHUTDOWN_EVENT ], BOS_TRUE );
   BOS_EVENTDELETE( &tcb->events[ BOS_RESURRECT_EVENT ]   , BOS_TRUE );
   BOS_EVENTDELETE( &tcb->events[ BOS_RESUME_EVENT ]      , BOS_TRUE );
   BOS_EVENTDELETE( &tcb->events[ BOS_TASKSTART_EVENT ]   , BOS_TRUE );
   BOS_EVENTDELETE( &tcb->events[ BOS_RESET_ACK_EVENT ]   , BOS_TRUE );

   FreeTCB( tcb );

   /* Leave critical section */
   bosMutexRelease( &boszcb.tcbMutexId );

   /* Delete the task */
   return ( bosTaskDestroy( taskId ));
}


/***************************************************************************/
PUBLIC BOS_STATUS bosTaskStart( BOS_TASK_ID *taskId )
{
   return ( SignalEvent( BOS_TASKSTART_EVENT, taskId ));
}


/***************************************************************************/
PUBLIC BOS_STATUS bosTaskSuspendR( BOS_TASK_ID *taskId )
{
   BOS_STATUS     retval;
   BOS_TASK_ID    myTaskId;

   retval = bosTaskGetMyTaskId( &myTaskId );

   /* Only self-suspension is currently supported */
   if ( *taskId != myTaskId )
   {
      return ( BOS_STATUS_ERR );
   }

   if ( bosTaskIsResetPending() )
   {
      /* Reset pending, return immediately */
      return ( BOS_STATUS_RESET );
   }

   /*
   ** No pending reset, suspend the task
   */

   /* Self-identify ( taskid is zero ) */
   retval = bosTaskGetMyTaskId( taskId );
   if ( retval != BOS_STATUS_OK )
   {
      return ( retval );
   }

   /* Enter critical section */
   bosMutexAcquire( &boszcb.tcbMutexId );

   /* Save task state to be restored after suspension */
   SaveTaskState( taskId );

   /* Set new task state */
   SetTaskState( taskId, BOS_TASKSTATE_SUSPENDED );

   /* Leave critical section */
   bosMutexRelease( &boszcb.tcbMutexId );

   /* Suspend task */
   retval = WaitForEvent( BOS_RESUME_EVENT );
   if ( retval != BOS_STATUS_OK )
   {
      /* Enter critical section */
      bosMutexAcquire( &boszcb.tcbMutexId );

      /* Suspend failed, revert state and return error */
      RestoreTaskState( taskId );

      /* Leave critical section */
      bosMutexRelease( &boszcb.tcbMutexId );
      return ( retval );
   }

   /* Suspend succeeded, check for pending reset after being resumed */
   if ( bosTaskIsResetPending() )
   {
      /* Reset pending, return RESET */
      retval = BOS_STATUS_RESET;
   }

   return ( retval );
}


/***************************************************************************/
PUBLIC BOS_STATUS bosTaskResumeR( BOS_TASK_ID *taskId )
{
   BOS_STATUS retval;
   BOS_TASKSTATE state;

   /* Enter critical section */
   bosMutexAcquire( &boszcb.tcbMutexId );

   /* Verify that the task is currently suspended */
   retval = GetTaskState( taskId, &state );
   if ( retval != BOS_STATUS_OK )
   {
      bosMutexRelease( &boszcb.tcbMutexId );
      return ( retval );
   }

   if ( state != BOS_TASKSTATE_SUSPENDED )
   {
      bosLogErr("TaskResume() attempting to resume a task not in SUSPENDED state!");
      bosMutexRelease( &boszcb.tcbMutexId );
      return ( BOS_STATUS_ERR );
   }

   retval = RestoreTaskState( taskId );

   /* Leave critical section */
   bosMutexRelease( &boszcb.tcbMutexId );

   if ( retval != BOS_STATUS_OK )
   {
      return ( retval );
   }

   /* Resume task */
   return ( SignalEvent( BOS_RESUME_EVENT, taskId ));
}


/***************************************************************************/
PUBLIC BOS_BOOL bosTaskIsResetPending( void )
{
   return ( IsResetPendingCheck( BOS_FALSE ) );
}


/***************************************************************************/
PUBLIC BOS_STATUS bosTaskResetNotify( BOS_TASK_ID *taskid )
{
   BOS_STATUS retval;
   BOSZTCB *t;

   bosLogInfo("BOS: Enter TaskResetNotify");

   /* Enter critical section */
   bosMutexAcquire( &boszcb.tcbMutexId );

   /*
   ** Notify all tasks that are eligible of the pending reset
   */
   retval = GetTCB( taskid, &t );
   if ( retval != BOS_STATUS_OK )
   {
      /* Leave critical section */
      bosMutexRelease( &boszcb.tcbMutexId );
      return ( retval );
   }

   if ( t->inuse )
   {
      switch ( t->state )
      {
         case BOS_TASKSTATE_NORMAL:
         {
            SetTaskState( &t->taskid, BOS_TASKSTATE_RESET_NTFY );
            bosLogInfo("TaskResetNotify informed task %s",
                            GetTaskName( &t->taskid ) );
         }
         break;

         case BOS_TASKSTATE_SUSPENDED:
         {
            /* Do not resume un-interruptible tasks, or notify them of the reset.
             * It is the parent task's responsibility to deal with un-interruptible
             * tasks during an application reset. */
            if ( t->savedState != BOS_TASKSTATE_UNINTERRUPTIBLE )
            {
               bosLogInfo("BOS: TaskResetNotify informed and resumed task %s",
                            GetTaskName( &t->taskid ) );

               /* Directly resume the task, and set the task state. Calling
                * TaskResume would result in deadlock, since we have already
                * taken the same mutex used by TaskResume */
               t->state = BOS_TASKSTATE_RESET_NTFY;
               bosEventSend( &t->events[ BOS_RESUME_EVENT ] );
            }
         }
         break;

         case BOS_TASKSTATE_UNINTERRUPTIBLE:
         {
            /* Do not interrupt */
            bosLogErr("TaskResetNotify can't reset an uninterruptible task!");

            /* Leave critical section */
            bosMutexRelease( &boszcb.tcbMutexId );

            return ( BOS_STATUS_ERR );
         }
         break;

         default:
         {
            /* Leave critical section */
            bosMutexRelease( &boszcb.tcbMutexId );

            return ( BOS_STATUS_OK );
         }
         break;
      }
   }

   /* Set this task as the source of the reset */
   retval = bosTaskGetMyTaskId( &t->resetSrcTaskid );
   if ( retval != BOS_STATUS_OK )
   {
      /* Leave critical section */
      bosMutexRelease( &boszcb.tcbMutexId );
      return ( retval );
   }

   /* Leave critical section */
   bosMutexRelease( &boszcb.tcbMutexId );

   /*
   ** Wait for the task notified of reset to acknowledge
   */
   bosLogInfo("Task 0x%x notified of reset, waiting for ack...", *taskid );
   retval = WaitForEvent( BOS_RESET_ACK_EVENT );

   if ( retval != BOS_STATUS_OK )
   {
      bosLogErr("TaskResetNotify(): BIG TROUBLE in little china! "
                "Can't wait on event while waiting for reset ack!" );
   }
   else
   {
      bosLogInfo("BOS: Task 0x%x acknowledged reset.", *taskid);
   }

   bosLogInfo("Exit TaskResetNotify");
   return ( BOS_STATUS_OK );
}


/***************************************************************************/
PUBLIC BOS_STATUS bosTaskResetAck( void )
{
   BOS_STATUS retval;
   BOS_TASK_ID taskid;
   BOSZTCB *t;

   bosLogInfo("Enter TaskResetAck (task=%s)", GetTaskNameSelf());

   retval = bosTaskGetMyTaskId( &taskid );
   if ( retval != BOS_STATUS_OK )
   {
      return ( retval );
   }
   retval = GetTCB( &taskid, &t );
   if ( retval != BOS_STATUS_OK )
   {
      return ( retval );
   }

   /* Enter critical section */
   bosMutexAcquire( &boszcb.tcbMutexId );

   retval = SetTaskStateSelf( BOS_TASKSTATE_RESET_ACKED );

   /* Leave critical section */
   bosMutexRelease( &boszcb.tcbMutexId );

   if ( retval != BOS_STATUS_OK )
   {
      return ( retval );
   }

   /*
   ** Notify the reset source that we have acknowledged the reset condition
   */
   if ( t->resetSrcTaskid )
   {
      SignalEvent( BOS_RESET_ACK_EVENT, &t->resetSrcTaskid );
      t->resetSrcTaskid = 0;
   }
   else
   {
      /* Notify the "root" reset task that we have acked the reset_ntfy */
      retval = bosSemGive( &boszcb.taskResetSemId );
   }

   if ( retval != BOS_STATUS_OK )
   {
      return ( retval );
   }


   /* Wait until told to resume by the root task */
   retval = WaitForEvent( BOS_TASKSHUTDOWN_EVENT );

   bosLogInfo("Exit TaskResetAck (task=%s)", GetTaskNameSelf());

   if ( retval != BOS_STATUS_OK )
   {
      return ( retval );
   }

   /* Enter critical section */
   bosMutexAcquire( &boszcb.tcbMutexId );

   retval = SetTaskStateSelf( BOS_TASKSTATE_SHUTDOWN );

   /* Leave critical section */
   bosMutexRelease( &boszcb.tcbMutexId );

   return ( retval );
}


/***************************************************************************/
PUBLIC BOS_STATUS bosTaskProceedToShutdown( BOS_TASK_ID *taskid )
{
   BOS_STATUS retval;
   BOS_TASKSTATE state;
   BOS_BOOL bSignalTask;

   bosLogInfo("Enter TaskProceedToShutdown 0x%x (%s)", *taskid, GetTaskName( taskid ));

   /* Enter critical section */
   bosMutexAcquire( &boszcb.tcbMutexId );

   retval = GetTaskState( taskid, &state );
   if ( retval != BOS_STATUS_OK )
   {
      /* Leave critical section */
      bosMutexRelease( &boszcb.tcbMutexId );
      return ( retval );
   }

   switch ( state )
   {
      case BOS_TASKSTATE_RESET_ACKED:
      case BOS_TASKSTATE_RESET_NTFY:
      {
         /* Signal the task to go to shutdown */
         bosLogInfo("TaskProceedToShutdown() signaling task (%s)", GetTaskName( taskid ));
         bSignalTask = BOS_TRUE;
      }
      break;

      case BOS_TASKSTATE_DONE:
      {
         /* Task already done, don't signal the task */
         bosLogWarning("TaskProceedToShutdown() not signaling task (%s) (task already DONE)", GetTaskName( taskid ));
         bSignalTask = BOS_FALSE;
      }
      break;

      default:
      {
         /* Other states will do nothing: don't signal the task */
         bosLogWarning("TaskProceedToShutdown() not signaling task (%s) (not in RESET_ACKED or NTFY)", GetTaskName( taskid ));
         bSignalTask = BOS_FALSE;
      }
      break;
   }

   /* Leave critical section */
   bosMutexRelease( &boszcb.tcbMutexId );

   if ( retval != BOS_STATUS_OK )
   {
      return ( retval );
   }

   bosLogInfo("BOS: Exit TaskProceedToShutdown (%s)", GetTaskName( taskid ));

   if ( bSignalTask )
   {
      /* Wake up the task to perform its shutdown operations */
      return ( SignalEvent( BOS_TASKSHUTDOWN_EVENT, taskid ));
   }
   else
   {
      return ( BOS_STATUS_OK );
   }
}


/***************************************************************************/
PUBLIC BOS_STATUS bosTaskWaitForDone( BOS_TASK_ID *taskid )
{
   BOS_STATUS   retval;
   BOSZTCB     *tcb;

   bosLogInfo("Enter TaskWaitForDone - %s waiting on task 0x%x (%s)",
                   GetTaskNameSelf(), *taskid, GetTaskName( taskid ));

   while ( 1 )
   {
      /* Enter critical section */
      bosMutexAcquire( &boszcb.tcbMutexId );

      retval = GetTCB( taskid, &tcb );

      /* Leave critical section */
      bosMutexRelease( &boszcb.tcbMutexId );

      if ( retval == BOS_STATUS_ERR )
      {
         break;
      }
      else
      {
         bosSleep( 100 );
      }
   }

   bosLogInfo("Exit TaskWaitForDone - %s done waiting on task 0x%x (%s)",
         GetTaskNameSelf(), *taskid, GetTaskName( taskid ) );

   return ( BOS_STATUS_OK );
}


#endif

/***************************************************************************/
PUBLIC BOS_STATUS bosAppInit( void )
{
   BOS_STATUS retval;

   bosLogInfo("Enter bosAppInit");

   /*
   ** Initialize control block
   */
   memset( &boszcb, 0, sizeof( boszcb ) );

#if BOS_CFG_RESET_SUPPORT
   boszcb.resetState = BOS_RESETSTATE_NORESET;
#endif


   /*
   ** Create bosAppReset semaphores & mutexes
   */
   retval = bosMutexCreate( BOS_TCB_MUTEX, &boszcb.tcbMutexId );
   bTcbMutexInit = BOS_TRUE;

#if BOS_CFG_RESET_SUPPORT
   BOS_SEMCREATE( retval, BOS_APPRESET_SEM, &boszcb.appResetSemId,
                  bAppResetSemInit  );
   BOS_SEMCREATE( retval, BOS_APPRESETCOMPLETE_SEM, &boszcb.appResetCompletedSemId,
                  bAppResetCompletSemInit  );
   BOS_SEMCREATE( retval, BOS_TASKRESET_SEM, &boszcb.taskResetSemId,
                  bTaskResetSemInit  );
#endif

   if ( retval != BOS_STATUS_OK )
   {
      bosLogErr("bosAppInit: Initialization error, cleaning up semaphores/mutexes");

      /* Cleanup anything created before returning */
#if BOS_CFG_RESET_SUPPORT
      BOS_SEMDELETE( bAppResetSemInit, &boszcb.appResetSemId );
      BOS_SEMDELETE( bAppResetCompletSemInit, &boszcb.appResetCompletedSemId );
      BOS_SEMDELETE( bTaskResetSemInit, &boszcb.taskResetSemId );
#endif
      BOS_MUDELETE(  bTcbMutexInit, &boszcb.tcbMutexId );

      return ( retval );
   }

   bosLogInfo("Exit bosAppInit");
   return( retval );
}



/***************************************************************************/
PUBLIC void bosAppStart( BOS_APP_INIT_FNC appInit, BOS_APP_DEINIT_FNC appDeinit )
{
   bosLogInfo("Enter bosStartApp");

   boszcb.appTask.init = appInit;
   boszcb.appTask.deinit = appDeinit;
   boszcb.appTask.stackSize = (16*1024);
   boszcb.appTask.taskClass = BOS_CFG_APP_TASK_PRIORITY;
   boszcb.appTask.taskArg = 0;
   boszcb.appTask.name = BOS_APPTASK_NAME;

#if BOS_CFG_RESET_SUPPORT
   /* Spawn the root task to asynchronously spawn the app task */
   bosTaskCreate( BOS_ROOTTASK_NAME,
                    (4*1024),
                    BOS_CFG_ROOT_TASK_PRIORITY,
                    AppResetRootTask,
                    0,
                    &boszcb.rootTaskId );
#else
   /* Spawn the app task - task init and deinit functions are not required since
    *                      we only spawn the app task once.  Thus, appInit is
    *                      treated as the task main function */
   bosTaskCreateEx(  boszcb.appTask.name,
                     boszcb.appTask.stackSize,
                     boszcb.appTask.taskClass,
                     NULL,
                     appInit,
                     NULL,
                     boszcb.appTask.taskArg,
                     &boszcb.appTask.taskId );
#endif

   bosLogInfo("Exit bosStartApp");

}

#if BOS_CFG_RESET_SUPPORT


/***************************************************************************/
PUBLIC void bosAppReset( void )
{
   /* Enter critical section */
   bosMutexAcquire( &boszcb.tcbMutexId );

   switch ( boszcb.resetDetectMode )
   {
      case BOS_RESETDETECT_ENABLED:
      {
         bosLogInfo("AppReset() - Signalling root task of reset.");

         /* Reset detection is enabled, disable detection to avoid a duplicate
         ** reset signal, and then notify the aoRootTask of the reset */
         boszcb.resetDetectMode = BOS_RESETDETECT_DISABLED;
         boszcb.resetState = BOS_RESETSTATE_RESETTING;
         bosSemGive( &boszcb.appResetSemId );

         /* Leave critical section */
         bosMutexRelease( &boszcb.tcbMutexId );

         /* Wait until reset finished! It makes no sense to continue earlier! */
         bosLogInfo("AppReset() - Wait for root task to complete reset.");
         bosSemTake(&boszcb.appResetCompletedSemId);

         return;
      }
      break;

      case BOS_RESETDETECT_DISABLED:
      {
         /* Ignore the reset if detection is disabled */
         bosLogWarning("AppReset() - Ignoring reset, reset detection is disabled.");
      }
      break;

      case BOS_RESETDETECT_INHIBITED:
      {
         /* Postpone handling the reset if detection is inhibited */
         bosLogWarning("AppReset() - Postponing reset until detection is enabled, reset detection is currently inhibited.");
         boszcb.resetState = BOS_RESETSTATE_POSTPONED;
      }
      break;
   }

   /* Leave critical section */
   bosMutexRelease( &boszcb.tcbMutexId );
}



/***************************************************************************/
PUBLIC void bosAppResetDetectionEnable( void )
{
   /* Enter critical section */
   bosMutexAcquire( &boszcb.tcbMutexId );

   switch ( boszcb.resetDetectMode )
   {
      case BOS_RESETDETECT_ENABLED:
      {
         bosLogWarning("AppResetDetectionEnable() - Doing nothing, detection already enabled.");
      }
      break;

      case BOS_RESETDETECT_DISABLED:
      {
         bosLogInfo("AppResetDetectionEnable() - Enabled reset detection.");
         boszcb.resetDetectMode = BOS_RESETDETECT_ENABLED;
      }
      break;

      case BOS_RESETDETECT_INHIBITED:
      {
         boszcb.resetDetectMode = BOS_RESETDETECT_ENABLED;

         if ( boszcb.resetState != BOS_RESETSTATE_POSTPONED )
         {
            bosLogInfo("AppResetDetectionEnable() - Enabled reset detection, no resets were postponed.");
         }
         else
         {
            /* A reset was postponed, signal it now */
            bosLogInfo("AppResetDetectionEnable() - Enabled reset detection, continuing a postponed reset...");
            boszcb.resetState = BOS_RESETSTATE_NORESET;
            bosAppReset();
         }
      }
      break;
   }

   /* Leave critical section */
   bosMutexRelease( &boszcb.tcbMutexId );
}


/***************************************************************************/
PUBLIC BOS_STATUS bosAppResetDetectionInhibit( void )
{
   BOS_STATUS retval = BOS_STATUS_OK;

   /* Enter critical section */
   bosMutexAcquire( &boszcb.tcbMutexId );

   if ( boszcb.resetState == BOS_RESETSTATE_RESETTING )
   {
      bosLogWarning("AppResetDetectionInhibit() - Can't inhibit reset detection, reset already in progress.");
      retval = BOS_STATUS_RESET;
   }
   else
   {
      boszcb.resetDetectMode = BOS_RESETDETECT_INHIBITED;
      bosLogInfo("AppResetDetectionInhibit() - Reset detection has been inhibited.");
   }

   /* Leave critical section */
   bosMutexRelease( &boszcb.tcbMutexId );

   return ( retval );
}

/** @} */

#endif   /* BOS_CFG_RESET_SUPPORT */


/* ---- Private Functions ------------------------------------------------ */

/**
 * @addtogroup bosTaskResetInternal
 * @{
 */

/***************************************************************************/
/**
*  Allocate a new task control block
*
*  @param   tcb   (out) returned pointer to the allocated TCB
*
*  @return  BOS_STATUS_OK if successful, error code on failure
*
*  @note    It is the caller's responsibility to insure concurrency protection
*/
PRIVATE BOS_STATUS AllocTCB( BOSZTCB **tcb )
{
   BOSZTCB *t;
   int i;

   for ( i = 0; i < MAX_TCB; i++ )
   {
      t = &boszcb.tcb[i];

      if ( !t->inuse )
      {
         t->inuse = 1;
         *tcb = t;
         return ( BOS_STATUS_OK );
      }
   }

   *tcb = NULL;
   return ( BOS_STATUS_ERR );
}


/***************************************************************************/
/**
*  De-allocate a task control block
*
*  @param   tcb   (in)  pointer to TCB to free
*
*  @return  BOS_STATUS_OK if successful, error code on failure
*
*  @note    It is the caller's responsibility to insure concurrency protection
*/
PRIVATE BOS_STATUS FreeTCB( BOSZTCB *tcb )
{
   memset( tcb, 0, sizeof( BOSZTCB ));

   return ( BOS_STATUS_OK );

}


/***************************************************************************/
/**
*  To retrieve a task control block associated with a task id
*
*  @param   taskid   (in)  task identifier
*  @param   tcb      (out) returned pointer to the TCB
*
*  @return  BOS_STATUS_OK if successful, error code on failure
*
*  @note    It is the caller's responsibility to insure concurrency protection
*/
PRIVATE BOS_STATUS GetTCB( BOS_TASK_ID *taskid, BOSZTCB **tcb )
{
   BOSZTCB *t;
   int i;

   for ( i = 0; i < MAX_TCB; i++ )
   {
      t = &boszcb.tcb[i];

      if ( t->inuse && ( t->taskid == *taskid ) )
      {
         *tcb = t;
         return ( BOS_STATUS_OK );
      }
   }

   *tcb = NULL;
   return ( BOS_STATUS_ERR );
}


#if BOS_CFG_RESET_SUPPORT

#if BOS_TASK_RESURRECT
/***************************************************************************/
/**
*  To retrieve the task control block associated with a task name
*
*  @param   name  (in)  task name
*  @param   tcb   (out) returned pointer to the TCB
*
*  @return  BOS_STATUS_OK if successful, error code on failure
*
*  @note    It is the caller's responsibility to insure concurrency protection
*/
PRIVATE BOS_STATUS GetNamedTCB( const char *name, BOSZTCB **tcb )
{
   BOSZTCB *t;
   int i;

   for ( i = 0; i < MAX_TCB; i++ )
   {
      t = &boszcb.tcb[i];

      /* Max TCB name length. */
      if ( t->inuse && !strncmp( t->name, name, BOS_CFG_TASK_MAX_TASK_NAME_LEN - 1 ))
      {
         *tcb = t;
         return ( BOS_STATUS_OK );
      }
   }

   *tcb = NULL;
   return ( BOS_STATUS_ERR );
}
#endif /* BOS_TASK_RESURRECT */



/***************************************************************************/
/**
*  Get the name of specified task
*
*  @param   taskid   (in)  task identifier
*
*  @return  name or "NULL"
*
*/
PRIVATE const char *GetTaskName( BOS_TASK_ID *taskid )
{
   BOS_STATUS   retval;
   BOSZTCB     *tcb;

   retval = GetTCB( taskid, &tcb );
   if ( retval == BOS_STATUS_OK )
   {
      return ( tcb->name );
   }
   else
   {
      return ( "NULL" );
   }
}


/***************************************************************************/
/*
*  Get the name of the calling task
*
*  @return  name or "NULL"
*
*/
PRIVATE const char *GetTaskNameSelf( void )
{
   BOS_TASK_ID   taskid;

   bosTaskGetMyTaskId( &taskid );

   return ( GetTaskName( &taskid ) );
}


/***************************************************************************/
/**
*  The calling task will wait for the specified event to occur
*
*  @param   eventid  (in)  the event type to wait for
*
*  @return  BOS_STATUS_OK if successful, error code on failure
*/
PRIVATE BOS_STATUS WaitForEvent( BOS_TASK_EVENT eventid )
{
   BOS_TASK_ID   taskid;
   BOSZTCB     *tcb = NULL;
   BOS_STATUS   retval;

   /* Enter critical section */
   bosMutexAcquire( &boszcb.tcbMutexId );

   /* Get my TCB */
   retval = bosTaskGetMyTaskId( &taskid );
   if ( retval == BOS_STATUS_OK )
   {
      retval = GetTCB( &taskid, &tcb );
   }

   /* Leave critical section */
   bosMutexRelease( &boszcb.tcbMutexId );

   if ( retval == BOS_STATUS_OK )
   {
      /* Wait until the event is received */
      retval = bosEventReceive( &tcb->events[ eventid ] );
   }

   return ( retval );
}


/***************************************************************************/
/**
*  To send an event to a task
*
*  @param   eventid  (in)  the event type to send
*  @param   taskid   (in)  the task to send the event to
*
*  @return  BOS_STATUS_OK if successful, error code on failure
*/
PRIVATE BOS_STATUS SignalEvent( BOS_TASK_EVENT eventid, BOS_TASK_ID *taskid )
{
   BOSZTCB     *tcb;
   BOS_STATUS   retval;

   /* Enter critical section */
   bosMutexAcquire( &boszcb.tcbMutexId );

   retval = GetTCB( taskid, &tcb );

   /* Leave critical section */
   bosMutexRelease( &boszcb.tcbMutexId );

   if ( retval == BOS_STATUS_OK )
   {
      /* Send an event to the specified task */
      retval = bosEventSend( &tcb->events[ eventid ] );
   }

   return ( retval );
}


/***************************************************************************/
/**
*  Set the task state of the specified task
*
*  @param   taskid   (in)  task id of the task to change the state of
*  @param   state    (in)  new task state for the specified task
*
*  @return  BOS_STATUS_OK if successful, error code on failure
*
*  @note    It is the caller's responsibility to insure concurrency protection
*/
PRIVATE BOS_STATUS SetTaskState( BOS_TASK_ID *taskid, BOS_TASKSTATE state )
{
   BOS_STATUS retval;
   BOSZTCB *tcb;

   retval = GetTCB( taskid, &tcb );

   if ( retval == BOS_STATUS_OK )
   {
      tcb->state = state;
   }

   return ( retval );
}


/***************************************************************************/
/**
*  Set the task state of the calling task
*
*  @param   state (in)  new task state for the calling task
*
*  @return  BOS_STATUS_OK if successful, error code on failure
*
*  @note    It is the caller's responsibility to insure concurrency protection
*/
PRIVATE BOS_STATUS SetTaskStateSelf( BOS_TASKSTATE state )
{
   BOS_STATUS retval;
   BOS_TASK_ID taskid;

   retval = bosTaskGetMyTaskId( &taskid );

   if ( retval == BOS_STATUS_OK )
   {
      retval = SetTaskState( &taskid, state );
   }

   return ( retval );
}


/***************************************************************************/
/**
*  Get the task state of the specified task
*
*  @param   taskid   (in)  get state of this task
*  @param   state    (out) current state for the specified task
*
*  @return  BOS_STATUS_OK if successful, error code on failure
*
*  @note    It is the caller's responsibility to insure concurrency protection
*/
PRIVATE BOS_STATUS GetTaskState( BOS_TASK_ID *taskid, BOS_TASKSTATE *state )
{
   BOS_STATUS retval;
   BOSZTCB *tcb;

   if ( !state )
   {
      return ( BOS_STATUS_ERR );
   }

   retval = GetTCB( taskid, &tcb );

   if ( retval == BOS_STATUS_OK )
   {
      *state = tcb->state;
   }

   return ( retval );
}


/***************************************************************************/
/**
*  Get the task state of the calling task
*
*  @param   state (out) current state for the calling task
*
*  @return  BOS_STATUS_OK if successful, error code on failure
*/
PRIVATE BOS_STATUS GetTaskStateSelf( BOS_TASKSTATE *state )
{
   BOS_STATUS retval;
   BOS_TASK_ID taskid;

   retval = bosTaskGetMyTaskId( &taskid );

   if ( retval == BOS_STATUS_OK )
   {
      /* Enter critical section */
      bosMutexAcquire( &boszcb.tcbMutexId );

      retval = GetTaskState( &taskid, state );

      /* Leave critical section */
      bosMutexRelease( &boszcb.tcbMutexId );
   }

   return ( retval );
}


/***************************************************************************/
/**
*  Save the state of a task (restored later by RestoreTaskState)
*
*  @param   taskid   (in)  task id of the task to save the state
*
*  @return  BOS_STATUS_OK if successful, error code on failure
*
*  @note    It is the caller's responsibility to insure concurrency protection
*/
PRIVATE BOS_STATUS SaveTaskState( BOS_TASK_ID *taskid )
{
   BOS_STATUS retval;
   BOSZTCB *tcb;

   retval = GetTCB( taskid, &tcb );

   if ( retval != BOS_STATUS_OK )
   {
      return ( retval );
   }

   return ( GetTaskState( taskid, &tcb->savedState ) );
}


/***************************************************************************/
/**
*  Restore the state of a task (saved by SaveTaskState)
*
*  @param   taskid   (in)  task id of the task to restore the state
*
*  @return  BOS_STATUS_OK if successful, error code on failure
*
*  @note    It is the caller's responsibility to insure concurrency protection
*/
PRIVATE BOS_STATUS RestoreTaskState( BOS_TASK_ID *taskid )
{
   BOS_STATUS retval;
   BOSZTCB *tcb;

   retval = GetTCB( taskid, &tcb );

   if ( retval != BOS_STATUS_OK )
   {
      return ( retval );
   }

   return ( SetTaskState( taskid, tcb->savedState ) );
}


/***************************************************************************/
/**
*  Checks if the calling task has been notified of a pending reset
*
*  @return  BOS_STATUS_OK if successful, error code on failure
*/
PUBLIC BOS_BOOL bosResourceIsResetPending( void )
{
   return ( IsResetPendingCheck( BOS_TRUE ) );
}


/***************************************************************************/
/**
*  Checks if the calling task has been notified of a pending reset
*
*  @param   isResource  (in)
*
*  @return  BOS_STATUS_OK if successful, error code on failure
*/
PRIVATE BOS_BOOL IsResetPendingCheck( BOS_BOOL isResource )
{
   BOS_TASKSTATE  state = BOS_TASKSTATE_NORMAL;
   BOS_STATUS      status;

   status = GetTaskStateSelf( &state );
   if ( status != BOS_STATUS_OK )
   {
      return ( BOS_FALSE );
   }

   /* Check state against current 'wake-up' condition states */
   if ( state == BOS_TASKSTATE_RESET_NTFY )
   {
      /* Interrupt the task because of a reset condition */
      return ( BOS_TRUE );
   }
   else if ( isResource && ( state == BOS_TASKSTATE_UNINTERRUPTIBLE ) )
   {
      /* Uninterruptible Tasks should not use interruptible resources */
      bosLogErr( "Uninterruptible task trying to use an interruptible resource!" );
      BOS_ASSERT( 0 );
   }

   /* Task does not need to be interrupted */
   return ( BOS_FALSE );
}

/***************************************************************************/
/**
*
*  This is a blocking call that informs all tasks of an application
*  reset and waits for each of them to ACK the reset.
*
*  @return  BOS_STATUS_OK if successful, error code on failure
*/
PRIVATE BOS_STATUS TaskResetNotifyAll( void )
{
   BOS_STATUS retval;
   BOSZTCB *t;
   int i;
   int nResetTasks = 0;

   bosLogInfo("Enter TaskResetNotifyAll");

   /* Enter critical section */
   bosMutexAcquire( &boszcb.tcbMutexId );

   /*
   ** Notify all tasks that are eligible of the pending reset
   */
   for ( i = 0; i < MAX_TCB; i++ )
   {
      t = &boszcb.tcb[i];

      if ( t->inuse )
      {
         switch ( t->state )
         {
            case BOS_TASKSTATE_NORMAL:
            {
               if( t->reset )
               {
                  SetTaskState( &t->taskid, BOS_TASKSTATE_RESET_NTFY );
                  bosLogInfo("TaskResetNotifyAll informed task %s", GetTaskName( &t->taskid ));
                  nResetTasks++;
               }
            }
            break;

            case BOS_TASKSTATE_SUSPENDED:
            {
               /* Do not resume un-interruptible tasks, or notify them of the reset.
                * It is the parent task's responsibility to deal with un-interruptible
                * tasks during an application reset. */
               if ( t->savedState != BOS_TASKSTATE_UNINTERRUPTIBLE && t->reset )
               {
                  bosLogInfo("TaskResetNotifyAll informed and resumed task %s", GetTaskName( &t->taskid ));

                  /* Directly resume the task, and set the task state. Calling
                   * TaskResume would result in deadlock, since we have already
                   * taken the same mutex used by TaskResume */
                  t->state = BOS_TASKSTATE_RESET_NTFY;
                  bosEventSend( &t->events[ BOS_RESUME_EVENT ] );
                  nResetTasks++;
               }
            }
            break;

            case BOS_TASKSTATE_UNINTERRUPTIBLE:
            default:
            {
               /* Do not interrupt */
            }
            break;
         }
      }
   }

   /* Leave critical section */
   bosMutexRelease( &boszcb.tcbMutexId );

   /*
   ** Wait for all tasks notified of reset to acknowledge
   */
   bosLogInfo("%d tasks notified of reset, waiting for acks...", nResetTasks);
   for ( i = 0; i < nResetTasks; i++ )
   {
      retval = bosSemTake( &boszcb.taskResetSemId );

      if ( retval != BOS_STATUS_OK )
      {
         bosLogErr( "TaskResetNotifyAll(): Failed to take %p with %d", &boszcb.taskResetSemId, (int)retval);
      }
   }

   bosLogInfo(": All %d tasks acknowledged reset.", nResetTasks);

   bosLogInfo("Exit TaskResetNotifyAll");
   return ( BOS_STATUS_OK );
}


/***************************************************************************/
/**
*  This is called invoked when a taskMain has completed before entering the
*  taskDeinit.
*
*  @return  BOS_STATUS_OK if successful, error code on failure
*/
PRIVATE BOS_STATUS TaskEnterShutdown( void )
{
   BOS_STATUS retval;
   BOS_TASK_ID taskid;
   BOS_TASKSTATE state;

   bosLogInfo("Enter TaskEnterShutdown (task=%s)", GetTaskNameSelf());

   /* Enter critical section */
   bosMutexAcquire( &boszcb.tcbMutexId );

   retval = bosTaskGetMyTaskId( &taskid );
   if ( retval != BOS_STATUS_OK )
   {
      return ( retval );
   }

   GetTaskState( &taskid, &state );

   if ( state == BOS_TASKSTATE_NORMAL )
   {
      /* A running task has exited its main. Change its state to shutdown */
      SetTaskState( &taskid, BOS_TASKSTATE_SHUTDOWN );
   }
   else if ( state == BOS_TASKSTATE_RESET_NTFY )
   {
      /* The task was notified of a reset, but didn't ack it.
      ** We must ack the reset, since the notifier is waiting for this ack.
      ** We must assume it was in the process of exiting on its own so we will
      ** allow it to proceed with its shutdown. This is accomplished by sending
      ** the shutdown event before acking, so that reset ack will not block. */
      bosTaskProceedToShutdown( &taskid );
      bosTaskResetAck();
   }

   /* Leave critical section */
   bosMutexRelease( &boszcb.tcbMutexId );

   bosLogInfo("Exit TaskEnterShutdown (task=%s)", GetTaskNameSelf());
   return ( BOS_STATUS_OK );
}


#if BOS_TASK_RESURRECT
/***************************************************************************/
/**
*  This is a blocking call invoked when a task has completed execution. The
*  calling task will wait until it is re-spawned.
*
*  @return  BOS_STATUS_OK if successful, error code on failure
*/
PRIVATE BOS_STATUS TaskEnterDone( void )
{
   BOS_STATUS retval;

   bosLogInfo("Enter TaskEnterDone (task=%s)", GetTaskNameSelf());

   /* Change own task's state to reset_done */
   retval = SetTaskStateSelf( BOS_TASKSTATE_DONE );
   if ( retval != BOS_STATUS_OK )
   {
      return ( retval );
   }

   bosLogInfo("Exit TaskEnterDone (task=%s)", GetTaskNameSelf());

   /* Wait until told to resume by the root task */
   return ( WaitForEvent( BOS_RESURRECT_EVENT ));
}


/***************************************************************************/
/**
*  To re-spawn a task that was previously reset
*
*  @param   tcb   (in)  Task control block of the task to resurrect
*
*  @return  BOS_STATUS_OK if successful, error code on failure
*
*  @note    It is the caller's responsibility to insure concurrency protection
*/
PRIVATE BOS_STATUS TaskResurrect( BOSZTCB *tcb )
{
   /* Change task's state to normal */
   tcb->state = BOS_TASKSTATE_NORMAL;

   /* Resume the previously suspended task */
   return ( bosEventSend( &tcb->events[ BOS_RESURRECT_EVENT ] ));
}
#endif /* BOS_TASK_RESURRECT */


/***************************************************************************/
/**
*  Task main template for all application tasks with reset support
*
*  @param   taskArg (in)   argument passed to main task function
*/
PRIVATE void AppResetTemplateTaskMain( BOS_TASK_ARG taskArg )
{
   BOS_STATUS           retval;
   BOS_TASK_ID          taskid;
   BOSZTCB             *t;
   BOS_TASK_INIT_FNC    initFnc;
   BOS_TASK_ENTRY       mainFnc;
   BOS_TASK_DEINIT_FNC  deinitFnc;
   BOS_TASKSTARTMODE    taskStartMode;


   /* Enter critical section */
   bosMutexAcquire( &boszcb.tcbMutexId );

   retval = bosTaskGetMyTaskId( &taskid );
   if ( retval != BOS_STATUS_OK )
   {
      bosLogErr("HUGE trouble in little China!!! Couldn't start bosTaskMain! taskid=0x%x", taskid);
      return;
   }

   retval = GetTCB( &taskid, &t );
   if ( retval != BOS_STATUS_OK )
   {
      /* Leave critical section */
      bosMutexRelease( &boszcb.tcbMutexId );

      bosLogErr("HUGE trouble in big China!!! Couldn't start bosTaskMain! taskid=0x%x", taskid);
      return;
   }

   initFnc = t->init;
   mainFnc = t->main;
   deinitFnc = t->deinit;
   taskStartMode = t->startMode;

   /* Leave critical section */
   bosMutexRelease( &boszcb.tcbMutexId );

   if ( taskStartMode == BOS_TASKSTART_SUSPENDED )
   {
      bosLogInfo("bosTaskMain() task %s started suspended", GetTaskNameSelf());
      WaitForEvent( BOS_TASKSTART_EVENT );
      bosLogInfo("BOS: bosTaskMain() task %s started (from suspended)", GetTaskNameSelf());
   }
   if ( initFnc )
   {
      (*initFnc)( taskArg );
   }
   (*mainFnc)( taskArg );
   TaskEnterShutdown();
   if ( deinitFnc )
   {
      (*deinitFnc)( taskArg );
   }

   /* Destroy yourself */
   bosTaskDestroyR(&taskid);
}

#endif   /* BOS_CFG_RESET_SUPPORT */

#if !BOS_CFG_RESET_SUPPORT
/***************************************************************************/
/**
*  Task main template for all application tasks
*
*  @param   taskArg (in)   argument passed to main task function
*/
PRIVATE void AppTemplateTaskMain( BOS_TASK_ARG taskArg )
{
   BOS_STATUS           retval;
   BOS_TASK_ID          taskid;
   BOSZTCB             *t;
   BOS_TASK_INIT_FNC    initFnc;
   BOS_TASK_ENTRY       mainFnc;
   BOS_TASK_DEINIT_FNC  deinitFnc;


   /* Enter critical section */
   bosMutexAcquire( &boszcb.tcbMutexId );

   retval = bosTaskGetMyTaskId( &taskid );
   if ( retval != BOS_STATUS_OK )
   {
      bosLogErr("HUGE trouble in little China!!! Couldn't start bosTaskMain! taskid=0x%x", taskid);
      return;
   }

   retval = GetTCB( &taskid, &t );
   if ( retval != BOS_STATUS_OK )
   {
      /* Leave critical section */
      bosMutexRelease( &boszcb.tcbMutexId );

      bosLogErr("HUGE trouble in big China!!! Couldn't start bosTaskMain! taskid=0x%x", taskid);
      return;
   }

   initFnc = t->init;
   mainFnc = t->main;
   deinitFnc = t->deinit;

   /* Free the TCB here since the TCB is not longer required when reset
    * support is off */
   FreeTCB( t );

   /* Leave critical section */
   bosMutexRelease( &boszcb.tcbMutexId );


   /* Invoke task's init, main, and deinit functions */
   if ( initFnc )
   {
      (*initFnc)( taskArg );
   }
   (*mainFnc)( taskArg );

   if ( deinitFnc )
   {
      (*deinitFnc)( taskArg );
   }
}
#endif

#if BOS_CFG_RESET_SUPPORT


/***************************************************************************/
/**
*  Main function for the root task. The root task spawns the
*  application task, waits for an application reset, informs
*  all tasks of the reset, waits for an ACK from each task, resumes
*  the application task, and then waits for the reset to complete.
*
*  @param   taskArg  (in)  not used
*
*  @return  BOS_STATUS_OK if successful, error code on failure
*/
PRIVATE void AppResetRootTask( BOS_TASK_ARG taskArg )
{
      BOS_STATUS status;

      bosLogInfo("AppResetRootTask() - Is it morning already? Spawning app task...");

      /*
      ** Wait forever for an application reset
      ** NOTE: it IS valid for a reset to occur during the appTask.init, this
      ** is why we must initialize the app asynchronously and wait for the reset
      ** signal immediately
      */

      /* This be done in the bos, but until we resolve the clear dhcp IP
       * problem, ccInit() will enable reset detection before provisioning */
#if (BOS_OS_LINUXKERNEL || BOS_OS_LINUXUSER)
      /* Reset detection should be enabled by default */
      bosAppResetDetectionEnable();
#endif

      /* Wait forever for an application reset (native semtake) */
      status = bosSemTake( &boszcb.appResetSemId );
      if ( status == BOS_STATUS_EXIT )
      {
         bosLogInfo("Exiting task aoRT");
         return;
      }

      bosLogInfo("bosAppRootTask() - RESET NOW");

      /* Notify all tasks of the reset, blocking until all have acked */
      TaskResetNotifyAll();

      /* Application reset is complete */
      boszcb.resetState = BOS_RESETSTATE_NORESET;

      /* Need this call to unregister the event TCB associated with the root task */
      bosTaskDestroy(&boszcb.rootTaskId);

      bosLogInfo("bosAppRootTask() - Reset completed, good night...");

      /* Reset complete. Notify the caller. */
      bosSemGive( &boszcb.appResetCompletedSemId );
}


#if !BOS_OS_LINUXKERNEL /* To disable compile warning */
/***************************************************************************/
/**
*  Disable reset detection (resets will be ignored)
*
*/
PRIVATE void AppResetDetectionDisable( void )
{
   /* Enter critical section */
   bosMutexAcquire( &boszcb.tcbMutexId );

   switch ( boszcb.resetDetectMode )
   {
      case BOS_RESETDETECT_DISABLED:
      {
         bosLogWarning("AppResetDetectionDisable() - Reset detection is already disabled.");
      }
      break;

      case BOS_RESETDETECT_ENABLED:
      {
         bosLogInfo("AppResetDetectionDisable() - Reset detection has been disabled.");
      }
      break;

      case BOS_RESETDETECT_INHIBITED:
      {
         bosLogInfo("AppResetDetectionDisable() - Reset detection has been disabled.");
      }
      break;
   }
   boszcb.resetDetectMode = BOS_RESETDETECT_DISABLED;

   /* Leave critical section */
   bosMutexRelease( &boszcb.tcbMutexId );
}
#endif /* !BOS_OS_LINUXKERNEL */

/** @} */

#endif   /* BOS_CFG_RESET_SUPPORT */

#endif   /* BOS_CFG_TASK */

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
*  Filename: bosTaskLinuxUser.c
*
****************************************************************************
*  Description:
*
*
****************************************************************************/
/**
*
*  @file    bosTaskLinuxUser.c
*
*  @brief   LinuxUser implementation of the BOS Task Module
*
****************************************************************************/

/* ---- Include Files ---------------------------------------------------- */


#include <bosCfg.h>
#include <bosSleep.h>

#if BOS_CFG_TASK

#include <bosTypes.h>
#include <bosError.h>
#include <bosErrorLinuxUser.h>
#include <bosTask.h>
#include <bosTaskPrivate.h>
#include <bosLog.h>

#if BOS_CFG_EVENT
#include <bosEventPrivate.h>
#endif

#include <str.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>

/* ---- Public Variables ------------------------------------------------- */
/* ---- Private Constants and Types -------------------------------------- */

/* All the pthread-related functions return a value different
** than zero if an error occurred. */
#define PT_RETURN_ON_ERROR( status, funcId )                   \
{                                                              \
   if ( (status) != 0 )                                        \
   {                                                           \
      bosLogErr("%s [%d]: Error: %d", __func__, __LINE__, status ); \
      return CheckTaskStatusLinuxUser( errno, (funcId) );      \
   }                                                           \
}

/* Task entry parameter structure */
typedef struct
{
   char name[ BOS_CFG_TASK_MAX_TASK_NAME_LEN ];
   BOS_TASK_ENTRY taskEntry;
   void           *argument;
} BOS_TASK_ENTRY_PARAM;


/* ---- Private Variables ------------------------------------------------ */

/**
 * Indicates that the Task module has been initialized.
 */

static   BOS_BOOL gBosTaskInitialized = BOS_FALSE;


/* ---- Private Function Prototypes -------------------------------------- */

static BOS_STATUS CheckTaskStatusLinuxUser( int err, BOS_TASK_FUNC_ID funcId );
static void* bosTaskCommonEntry( BOS_TASK_ENTRY_PARAM *entryParam );

/* ---- Functions -------------------------------------------------------- */

/**
 * @addtogroup bosTask
 * @{
 */

/***************************************************************************/

BOS_STATUS bosTaskInit( void )
{
   BOS_ASSERT( !gBosTaskInitialized );

   gBosTaskInitialized = BOS_TRUE;

   return BOS_STATUS_OK;

} /* bosTaskInit */

/***************************************************************************/

BOS_STATUS bosTaskTerm( void )
{
   BOS_ASSERT( gBosTaskInitialized );

   gBosTaskInitialized = BOS_FALSE;

   return BOS_STATUS_OK;

} /* bosTaskTerm */

/***************************************************************************/

BOS_STATUS bosTaskCreate
(
   const char            *name,
   int                   stackSize,
   BOS_TASK_CLASS        taskClass,
   BOS_TASK_ENTRY        taskEntry,
   BOS_TASK_EXTENDED_ARG *argument,
   BOS_TASK_ID           *taskId
)
{
   int status;
   pthread_attr_t attributes;
   struct sched_param schedparams;
   BOS_TASK_ENTRY_PARAM *entryParam;

   BOS_ASSERT( gBosTaskInitialized );

   errno = 0;
   status = pthread_attr_init( &attributes );
   PT_RETURN_ON_ERROR( status, BOS_TASK_CREATE_FUNC_ID );

   status = pthread_attr_setinheritsched( &attributes, 0 );
   PT_RETURN_ON_ERROR( status, BOS_TASK_CREATE_FUNC_ID );

   status = pthread_attr_setschedpolicy( &attributes, SCHED_FIFO );
   PT_RETURN_ON_ERROR( status, BOS_TASK_CREATE_FUNC_ID );

   switch(taskClass)
   {
      case BOS_TASK_CLASS_HIGH:      schedparams.sched_priority = BOS_CFG_TASK_HIGH_VAL; break;
      case BOS_TASK_CLASS_MED_HIGH:  schedparams.sched_priority = BOS_CFG_TASK_MED_HIGH_VAL; break;
      case BOS_TASK_CLASS_MED:       schedparams.sched_priority = BOS_CFG_TASK_MED_VAL; break;
      case BOS_TASK_CLASS_MED_LOW:   schedparams.sched_priority = BOS_CFG_TASK_MED_LOW_VAL; break;
      case BOS_TASK_CLASS_LOW:       schedparams.sched_priority = BOS_CFG_TASK_LOW_VAL; break;
      default:                       schedparams.sched_priority = taskClass; break;
   }
   status = pthread_attr_setschedparam( &attributes, &schedparams );
   PT_RETURN_ON_ERROR( status, BOS_TASK_CREATE_FUNC_ID );

   entryParam = malloc(sizeof(BOS_TASK_ENTRY_PARAM));
   if ( !entryParam )
   {
      bosLogErr("%s: entryParam is NULL", __func__);
      return BOS_STATUS_ERR;
   }

   /* Populate entry param structure */
   snprintf(entryParam->name, BOS_CFG_TASK_MAX_TASK_NAME_LEN, "%s%s", BOS_TASK_NAME_PREFIX, name);
   entryParam->taskEntry = taskEntry;
   entryParam->argument = (argument != NULL) ? (void*)argument->argument : (void*)0;

   while(1)
   {
      status = pthread_create( (pthread_t*)taskId,
                               &attributes,
                               (void*)bosTaskCommonEntry,
                               entryParam);
      if(!status || errno != EINTR)
      {
         break;
      }

      bosSleep(100);
   }
   if ( status != 0 )
   {
      /* Could not create thread. Entry param memory */
      free(entryParam);
   }
   PT_RETURN_ON_ERROR( status, BOS_TASK_CREATE_FUNC_ID );

#if BOS_CFG_EVENT
      /*
       * Register the task with the Event module.
       */
      bosEventRegisterTask( taskId );
#endif

   /* Set the task priority and scheduling class */
   pthread_setschedparam( *(pthread_t*)taskId, SCHED_FIFO, &schedparams);

   bosLogInfo("Created task %s", name);

   return( BOS_STATUS_OK );

} /* bosTaskCreate */

/***************************************************************************/

BOS_STATUS bosTaskDestroy ( BOS_TASK_ID *taskId )
{
#if BOS_CFG_EVENT
   /*
    * Un-register the task with the Event module. This needs to be performed
    * first since taskDelete will not return if the caller is committing suicide.
    */
   bosEventUnRegisterTask( taskId );
#endif


   if( *taskId == pthread_self()) /* The task is attempting to destroy itself */
   {
      pthread_exit("Exiting thread");
   }
   else /* Destroy the task with this particular ID */
   {
      if (pthread_cancel( *taskId ) !=0 )
      {
         return(BOS_STATUS_ERR);
      }
   }
   pthread_testcancel(); /* Set cancellation point */
   pthread_join(*taskId, NULL); /* Acknowledge the child's death */

   return( BOS_STATUS_OK );

} /* bosTaskDestroy */

/***************************************************************************/

BOS_STATUS bosTaskSuspend( BOS_TASK_ID *taskId )
{
   bosLogWarning("bosTaskSuspend not implemented for Linux User mode");
   return( BOS_STATUS_ERR );

} /* bosTaskSuspend */

/***************************************************************************/

BOS_STATUS bosTaskResume( BOS_TASK_ID *taskId )
{
   bosLogWarning("bosTaskResume not implemented for Linux User mode");
   return( BOS_STATUS_ERR );

} /* bosTaskResume */

/***************************************************************************/

BOS_STATUS bosTaskExists( const BOS_TASK_ID *taskId )
{
   bosLogWarning("bosTaskExists not implemented for Linux User mode");
   return( BOS_STATUS_ERR );

} /* bosTaskExists */

/***************************************************************************/

BOS_STATUS bosTaskGetPriority( const BOS_TASK_ID *taskId, BOS_TASK_CLASS *taskClass )
{
   int status;
   struct sched_param schedparams;
   int policy;

   status = pthread_getschedparam( *taskId, &policy, &schedparams );
   PT_RETURN_ON_ERROR( status, BOS_TASK_GET_PRIORITY_FUNC_ID );

   *taskClass = bosTaskCalcTaskClass( schedparams.sched_priority );

   return( BOS_STATUS_OK );

} /* bosTaskGetPriority */

/***************************************************************************/

BOS_STATUS bosTaskSetPriority( BOS_TASK_ID *taskId, BOS_TASK_CLASS taskClass )
{
   /* NOTE: bosTaskSetPriority is implemented only for the calling thread.
   ** taskId parameter is not used. */

   int status;
   pthread_attr_t attributes;
   struct sched_param schedparams;

   schedparams.sched_priority = taskClass;
   status = pthread_attr_setschedparam( &attributes, &schedparams );
   PT_RETURN_ON_ERROR( status, BOS_TASK_SET_PRIORITY_FUNC_ID );

   return( BOS_STATUS_OK );

} /* bosTaskSetPriority */

/***************************************************************************/

BOS_STATUS bosTaskGetMyTaskId( BOS_TASK_ID *taskId )
{
   *taskId = pthread_self();

   return BOS_STATUS_OK;

} /* bosTaskGetMyTaskId */

/***************************************************************************/

BOS_STATUS bosTaskGetIdFromName( const char *tName, BOS_TASK_ID *taskId )
{
   /* Not implemented */
   return BOS_STATUS_ERR;

} /* bosTaskGetIdFromName */

/***************************************************************************/

BOS_STATUS bosTaskGetNameFromId
(
   const BOS_TASK_ID   *taskId,
   char                *dstTaskName,
   int                  maxLen
)
{
   /* Not implemented */
   return BOS_STATUS_ERR;

} /* bosTaskGetNameFromId */

/** @} */

/**
 * @addtogroup bosTaskInternal
 * @{
 */

/***************************************************************************/
/**
*  Translate a LinuxUser error code into a BOS error code and report any
*  errors encountered.
*
*  @param   err      (in)  LinuxUser Error code.
*  @param   funcId   (in)  ID of the function that we're checking for.
*/

BOS_STATUS CheckTaskStatusLinuxUser( int err, BOS_TASK_FUNC_ID funcId )
{
   return bosErrorCheckStatusLinuxUser( err,
                                      BOS_MAKE_ERROR_SOURCE( BOS_TASK_MODULE, funcId ));

} /* CheckTaskStatusLinuxUser */

/** @} */

/***************************************************************************/

static void* bosTaskCommonEntry(BOS_TASK_ENTRY_PARAM *entryParam)
{
   BOS_TASK_ENTRY taskEntry = entryParam->taskEntry;
   void *argument = entryParam->argument;

   /* Set task name */
   prctl(PR_SET_NAME, entryParam->name, 0, 0, 0);

   /* Free entry param */
   free(entryParam);

   taskEntry(argument);

   return 0;
}

#endif /* BOS_CFG_TASK */


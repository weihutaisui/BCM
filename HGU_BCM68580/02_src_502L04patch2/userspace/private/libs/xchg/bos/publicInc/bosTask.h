/***************************************************************************
*
* <:copyright-BRCM:2014:DUAL/GPL:standard
* 
*    Copyright (c) 2014 Broadcom 
*    All Rights Reserved
* 
* Unless you and Broadcom execute a separate written software license
* agreement governing use of this software, this software is licensed
* to you under the terms of the GNU General Public License version 2
* (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
* with the following added to such license:
* 
*    As a special exception, the copyright holders of this software give
*    you permission to link this software with independent modules, and
*    to copy and distribute the resulting executable under terms of your
*    choice, provided that you also meet, for each linked independent
*    module, the terms and conditions of the license of that module.
*    An independent module is a module which is not derived from this
*    software.  The special exception does not apply to any modifications
*    of the software.
* 
* Not withstanding the above, under no circumstances may you combine
* this software in any way with any other Broadcom software provided
* under a license other than the GPL, without Broadcom's express prior
* written consent.
* 
* :>
*
*****************************************************************************/
/**
*
*  @file    bosTask.h
*
*  @brief   Contains the BOS Task module interface
*
****************************************************************************/
/**
*  @defgroup   bosTask   BOS Task
*
*  @brief      Provides task module interface
*
*  The BOS task interface provides the ability to create, destroy, and control
*  the set of tasks the exist within an application.
*
****************************************************************************/

#ifndef BOSTASK_H
#define BOSTASK_H

#ifndef BOSCFG_H
#include <bosCfg.h>     /* include application-specific BOS config file */
#endif

#ifndef BOSTYPES_H
#include "bosTypes.h"
#endif

#ifndef BOSERROR_H
#include "bosError.h"
#endif

#if !BOS_CFG_TASK
#error bos Task module not selected in bosCfg.h
#endif

#if BOS_OS_ECOS
#  include <../eCos/bosTaskECOS.h>
#elif BOS_OS_PSOS
#  include <../pSOS/bosTaskPSOS.h>
#elif BOS_OS_VXWORKS
#  include <../VxWorks/bosTaskVxWorks.h>
#elif BOS_OS_WIN32
#  include <../Win32/bosTaskWin32.h>
#elif BOS_OS_LINUXKERNEL
#  include <../LinuxKernel/bosTaskLinuxKernel.h>
#elif BOS_OS_LINUXUSER
#  include <../LinuxUser/bosTaskLinuxUser.h>
#else
#  error Unknown OS configuration from bosCfg.h
#endif
#if defined( __cplusplus )
extern  "C"
{
#endif

/*
*****************************************************************************
** CONSTANTS
*****************************************************************************
*/

/*
*****************************************************************************
** TYPE DEFINITIONS
*****************************************************************************
*/

/* BOS reset-task start modes */
typedef enum
{
   BOS_TASKSTART_RUNNING,
   BOS_TASKSTART_SUSPENDED

} BOS_TASKSTARTMODE;

/* BOS test affinity */
typedef enum
{
   BOS_TASK_AFFINITY_TP0    = 0,
   BOS_TASK_AFFINITY_TP1    = 1,
   BOS_TASK_AFFINITY_ALL_TP = 2
   
} BOS_TASK_AFFINITY;

/** Task entry argument */
typedef void* BOS_TASK_ARG;

/* BOS task extended argument */
typedef struct
{
   BOS_TASK_AFFINITY affinity;    /* Task affinity */
   BOS_TASK_ARG      argument;    /* Task argument */

} BOS_TASK_EXTENDED_ARG;

/** This is the BOS task fuction prototype - one void* argument returning void */
typedef void (*BOS_TASK_INIT_FNC)( BOS_TASK_ARG argument );
typedef void (*BOS_TASK_ENTRY) ( BOS_TASK_ARG argument );
typedef void (*BOS_TASK_DEINIT_FNC)( BOS_TASK_ARG argument );

/* Application module init and de-init function prototypes. These must be the
 * same as the task init and de-init prototypes. */
typedef BOS_TASK_INIT_FNC     BOS_APP_INIT_FNC;
typedef BOS_TASK_DEINIT_FNC   BOS_APP_DEINIT_FNC;

/**
 * BOS task priority levels.
 * BOS_TASK_xxx is translated to the real underlying OS value BOS_CFG_TASK_xxx_VAL
 * as specificed in the bosCfg.h/bosCfgCustom.h header files.
 *
 * It's important that these reserved numbers do not overlap with "native"
 * task priorities.
 *
 * pSos  task priorities   range from   0 to 255.
 * Win32 thread priorities range from -15 to  15.
 */
typedef enum
{
   BOS_TASK_CLASS_START = 0x8000,   /**< Start of reserved task classes.   */

   BOS_TASK_CLASS_HIGH,             /**< High priority task.               */
   BOS_TASK_CLASS_MED_HIGH,         /**< Medium-High priority task.        */
   BOS_TASK_CLASS_MED,              /**< Medium priority task.             */
   BOS_TASK_CLASS_MED_LOW,          /**< Medium-Low priority task.         */
   BOS_TASK_CLASS_LOW,              /**< Low priority task.                */

   BOS_TASK_CLASS_END,              /**< End of reserved task classes.     */

   /* External task class defns begin at the end of the predefined classes */
   BOS_TASK_CLASS_EXT1 = BOS_TASK_CLASS_END,
   BOS_TASK_CLASS_EXT2,
   BOS_TASK_CLASS_EXT3,
   BOS_TASK_CLASS_EXT4,
   BOS_TASK_CLASS_EXT5,
   BOS_TASK_CLASS_EXT6,
   BOS_TASK_CLASS_EXT7,
   BOS_TASK_CLASS_EXT8,
   BOS_TASK_CLASS_EXT9,
   BOS_TASK_CLASS_EXT10,
   BOS_TASK_CLASS_EXT11,
   BOS_TASK_CLASS_EXT12,
   BOS_TASK_CLASS_EXT13,
   BOS_TASK_CLASS_EXT14,
   BOS_TASK_CLASS_EXT15,
   BOS_TASK_CLASS_EXT16

} BOS_TASK_CLASS;


#ifndef BOS_CFG_TASK_NUM_EXT_CLASSES
#  define BOS_CFG_TASK_NUM_EXT_CLASSES 0
#endif


/**< Number of reserved classes.                                        */
#define BOS_TASK_NUM_INT_CLASSES (BOS_TASK_CLASS_END - BOS_TASK_CLASS_START + 1)
#define BOS_TASK_NUM_CLASSES     (BOS_TASK_NUM_INT_CLASSES + \
                                  BOS_CFG_TASK_NUM_EXT_CLASSES)

/**
 * Determines if a particular task class is one of the reserved ones above.
 */

#define  BOS_TASK_CLASS_IS_RESERVED(taskClass)  \
   (((taskClass) > BOS_TASK_CLASS_START ) && ((taskClass) < (BOS_TASK_CLASS_START + BOS_TASK_NUM_CLASSES) ))

/**
 * The various implementations of bosTaskCreate use a lookup table to translate
 * one of the BOS_TASK_CLASS enumerations into a natiev priority. This macro
 * is used to translate a BOS_TASK_CLASS into an index starting at zero
 * for BOS_TASK_CLASS_HIGH thru BOS_TASK_NUM_CLASSES - 1 for BOS_TASK_CLASS_LOW.
 */

#define  BOS_TASK_CLASS_TO_INDEX(taskClass)  \
   ((taskClass) - BOS_TASK_CLASS_START - 1)

/**
 * Complement of BOS_TASK_CLASS_TO_INDEX.
 */

#define  BOS_TASK_INDEX_TO_CLASS(idx) \
   ( BOS_TASK_CLASS_START + (idx) + 1 )

/**
 * Function index, used with the error handler to report which function
 * an error originated from.
 *
 * These constants would typically only be used if you provide your own
 * error handler.
 *
 * @note If you change anything in this enumeration, please ensure that
 *       you make the corresponding updates in the bosTaskInitFunctionNames
 *       routine.
 */

typedef enum
{
   BOS_TASK_INIT_FUNC_ID,              /**< bosTaskInit                    */
   BOS_TASK_TERM_FUNC_ID,              /**< bosTeaskTerm                   */
   BOS_TASK_CREATE_FUNC_ID,            /**< bosTaskCreate                  */
   BOS_TASK_DESTROY_FUNC_ID,           /**< bosTaskDestroy                 */
   BOS_TASK_SUSPEND_FUNC_ID,           /**< bosTaskSuspend                 */
   BOS_TASK_RESUME_FUNC_ID,            /**< bosTaskResume                  */
   BOS_TASK_EXISTS_FUNC_ID,            /**< bosTaskExists                  */
   BOS_TASK_GET_PRIORITY_FUNC_ID,      /**< bosTaskGetPriority             */
   BOS_TASK_SET_PRIORITY_FUNC_ID,      /**< bosTaskSetPriority             */
   BOS_TASK_GET_MY_TASK_ID_FUNC_ID,    /**< bosTaskGetMyTaskId             */
   BOS_TASK_GET_ID_FROM_NAME_FUNC_ID,  /**< bosTaskGetIdFromName           */
   BOS_TASK_GET_NAME_FROM_ID_FUNC_ID,  /**< bosTaskNameFromId              */

   BOS_TASK_NUM_FUNC_ID    /**< Number of functions in the Task module.    */

} BOS_TASK_FUNC_ID;

/* Priority level structure for EMTA threads. This structure contains absolute task
 * priority levels for various classes of EMTA tasks. The absolute task priority
 * levels are supplied to BOS when it is initialized. This allows the task
 * levels to be set by an external entity that understands how they should be
 * set in order to be compatible with the overall system architecture */
typedef struct
{
   BOS_UINT32 priorityLevel[ BOS_TASK_NUM_CLASSES ];

} BOS_TASK_CLASS_PRIORITY_TABLE;

/* This prefix is added to all threads created by BOS */
#define BOS_TASK_NAME_PREFIX "voice-"

/* ---- Variable Externs ------------------------------------------------- */

/**
 * Contains string versions of the task function names. This table is
 * indexed using values from the BOS_TASK_FUNC_ID enumeration.
 */

extern const char *gBosTaskFunctionName[];

/* ---- Function Prototypes ---------------------------------------------- */

#if ( BOS_CFG_RESET_SUPPORT )

   #if ( !BUILDING_BOS)
      /* If reset support is enabled, application code (non-OS) will use the
      ** re-defined function prototypes for the reset-aware versions of the following
      ** blocking functions */
      #define bosTaskDestroy        bosTaskDestroyR
      #define bosTaskSuspend        bosTaskSuspendR
      #define bosTaskResume         bosTaskResumeR
   #else
      /* need to declare the function prototype here to avoid compilation problems
       * in VxWorks */
      BOS_STATUS bosTaskDestroyR( BOS_TASK_ID *taskId );
      BOS_STATUS bosTaskSuspendR( BOS_TASK_ID *taskId );
      BOS_STATUS bosTaskResumeR( BOS_TASK_ID *taskId );
   #endif

#endif   /* BOS_CFG_RESET_SUPPORT */

/***************************************************************************/
/**
*  Initializes the BOS Task module.
*
*  @return  BOS_STATUS_OK if the initialization was successful, or an error
*           code if some type of error occured.
*/

BOS_STATUS bosTaskInit( void );

/***************************************************************************/
/**
*  Terminates the BOS Task module.
*
*  @return  BOS_STATUS_OK if the termination was successful, or an error
*           code if some type of error occured.
*/

BOS_STATUS bosTaskTerm( void );

/***************************************************************************/
/**
*  Set BOS's internal Class Priority table
*
*  @param   priorities  (in) Pointer to task prioirty table.
*/

BOS_STATUS bosTaskSetClassPriorities( BOS_TASK_CLASS_PRIORITY_TABLE *priorities );

/***************************************************************************/
/**
*  Create a task.
*
*  @param   name        (in)  Pointer to task string descriptor.
*  @param   stackSize   (in)  Stack size to allocate - in bytes. If this value is
 *                     set to 0, the default value for the configured
 *                     underlying OS will be used.
*  @param   taskClass   (in)  The Broadcom OS supports an enumerated range
 *                     (BOS_TASK_CLASS) of task priorities. These numeric
 *                     values associated with these priorities are
 *                     application specific, and are defined in the
 *                     <bosCfg.h> configuration file. If a value other than
 *                     one of the predefined ennumerated ones is used, it is
 *                     treated as an absolute priority value to be passed
 *                     directly to the OS task creation call.
 *
*  @param   taskEntry   (in)  A pointer to the task entry point function.
*  @param   argument    (in)  Value passed into task entry point function.
*  @param   taskId      (out) Used to uniquely identify created task in all
 *                      future system calls.
 *
*  @return  BOS_STATUS_OK if successful, error code on failure
*
*  @note    Any additional creation parameters, such as election of the underlying
 *       OS scheduling method (round robin timeslicing, etc.) are nominated in
 *       the application-specific configuration header <bosCfgCustom.h>.
 */
BOS_STATUS bosTaskCreate
(
   const char            *name,
   int                   stackSize,
   BOS_TASK_CLASS        taskClass,
   BOS_TASK_ENTRY        taskEntry,
   BOS_TASK_EXTENDED_ARG *argument,
   BOS_TASK_ID           *taskId
);

/***************************************************************************/
/**
*  Destroy a task.
*
*  @param   taskId (mod)   A pointer to the task identifier generated on creation.
*
*  @return  BOS_STATUS_OK if successful, error code on failure
*
 */
BOS_STATUS bosTaskDestroy ( BOS_TASK_ID *taskId );

/***************************************************************************/
/**
*  Suspends a task.
 *
*  @param   taskId (mod) A pointer to the task identifier generated on creation.
*
*  @return  BOS_STATUS_OK if successful, error code on failure
*
*  @note    This function is not nestable; in other words, once the task has
 *             been suspended for the first time, further calls are ignored.
 */
BOS_STATUS bosTaskSuspend ( BOS_TASK_ID *taskId );

/***************************************************************************/
/**
*  Resumes a task.
 *
*  @param   taskId   (mod) A pointer to the task identifier generated on creation.
*
*  @return  BOS_STATUS_OK if successful, error code on failure
*
 */
BOS_STATUS bosTaskResume( BOS_TASK_ID *taskId );

/***************************************************************************/
/**
*  Verifies a task's existence.
 *
*  @param   taskId   (in)  A pointer to the task identifier generated on creation.
*
*  @return  BOS_STATUS_OK if successful, error code on failure
*
 */
BOS_STATUS bosTaskExists ( const BOS_TASK_ID *taskId );

/***************************************************************************/
/**
*  Determine the currently-assigned task priority class.
 *
*  @param   taskId   (in)  A pointer to the task identifier generated on creation.
*  @param   taskClass(out) Variable of ennumerated type BOS_TASK_CLASS
*
*  @return  BOS_STATUS_OK if successful, error code on failure
 */
BOS_STATUS bosTaskGetPriority( const BOS_TASK_ID *taskId, BOS_TASK_CLASS *taskClass );


/***************************************************************************/
/**
*  Set a new task priority class .
 *
*  @param   taskId   (mod) A pointer to the task identifier generated on creation.
*  @param   taskClass(in)  Variable of ennumerated type BOS_TASK_CLASS
*
*  @return  BOS_STATUS_OK if successful, error code on failure
 */
BOS_STATUS bosTaskSetPriority( BOS_TASK_ID *taskId, BOS_TASK_CLASS taskClass );

/***************************************************************************/
/**
*  Get task Id associated with the calling task.
 *
*  @param   taskId   (out) ID of calling task.
*
*  @return  BOS_STATUS_OK if successful, error code on failure
 */
BOS_STATUS bosTaskGetMyTaskId ( BOS_TASK_ID *taskId );

/***************************************************************************/
/**
*  Given the task name, determine the task ID.
 *
*  @param   taskStringName (in)  String name of task with unknown ID.
*  @param   taskId         (out) ID of task with name taskStringName
*
*  @return  BOS_STATUS_OK if successful, error code on failure
 */
BOS_STATUS bosTaskGetIdFromName ( const char *taskStringName, BOS_TASK_ID *taskId );

/***************************************************************************/
/**
*  Given the task ID, determine the task name.
 *
*  @param   taskStringName (in)  String name of task with unknown ID.
*  @param   taskId         (out) ID of task with name taskStringName
*
*  @return  BOS_STATUS_OK if successful, error code on failure
 */
BOS_STATUS bosTaskGetNameFromId ( const BOS_TASK_ID *taskId, char *taskStringName, int maxLen );

/***************************************************************************/
/**
*  Translate a priority class into the native OS priority value.
*
*  @param   taskClass   (in)  BOS_TASK_CLASS enum or actual priority
*
*  @return  Native task priority associated with @a taskClass.
*/
BOS_TASK_PRIORITY bosTaskCalcNativePriority( BOS_TASK_CLASS taskClass );

/***************************************************************************/
/**
*  Given an OS priority, calculate the BOS_TASK_CLASS that this priority
*  represents.
*
*  @param   priority (in)  Priority to convert.
*
*  @return  A BOS_TASK_CLASS which represents this priority, or if no
*           equivalent BOS_TASK_CLASS exists, the native priority.
*/
BOS_TASK_CLASS    bosTaskCalcTaskClass( BOS_TASK_PRIORITY priority );

/***************************************************************************/
/**
*  Create a task and allocate the appropriate resources
*
*  @return  BOS_STATUS_OK if successful, error code on failure
*
*  @note    bUnInterruptible and taskStartMode are ignored when reset support
*           is disabled.
*
*           Uninterruptible tasks do not allocate or use OS resources,
*           and therefore are not informed of application resets.
*/
BOS_STATUS
bosTaskCreateMain
(
   const char            *name,             /**< (in) name of the task (4 characters max)   */
   int                   stackSize,        /**< (in) task stack size (bytes)               */
   BOS_TASK_CLASS        taskClass,        /**< (in) task priority class                   */
   BOS_TASK_INIT_FNC     taskInit,         /**< (in) task initialization function          */
   BOS_TASK_ENTRY        taskMain,         /**< (in) task main function                    */
   BOS_TASK_DEINIT_FNC   taskDeinit,       /**< (in) task de-initialization function       */
   BOS_TASK_EXTENDED_ARG *argument,         /**< (in) the task argument passed to <taskInit>
                                           **       <taskMain>, and <taskDeinit>          */
   BOS_TASK_ID           *taskId,           /**< (out) task identifier of newly create task */
   BOS_BOOL              bUnInterruptible, /**< (in) indicates if task is uninterruptible  */
   BOS_TASKSTARTMODE     taskStartMode     /**< (in) taskStartMode - Mode to start the task
                                                  (AOS_TASKSTARTMODE_RUNNING | SUSPENDED) */
);

/***************************************************************************/
/**
*  Create a task in the extended version (TCBs are allocated)
*
*  @param   name        (in)  name of the task (4 characters max)
*  @param   stackSize   (in)  task stack size (bytes)
*  @param   taskClass   (in)  task priority class
*  @param   taskInit    (in)  task initialization function
*  @param   taskMain    (in)  task main function
*  @param   taskDeinit  (in)  task de-initialization function
*  @param   argument    (in)  the task argument passed to <taskMain>
*  @param   taskId      (out)  task identifier
*
*  @return  BOS_STATUS_OK if successful, error code on failure
*/
#define bosTaskCreateEx(nm, sz, cl, i, m, d, arg, id)   \
         bosTaskCreateMain(nm, sz, cl, i, m, d, arg, id, BOS_FALSE, BOS_TASKSTART_RUNNING)


/***************************************************************************/
/**
*  Create a task that can be reset (in RUNNING mode)
*
*  @param   name        (in)  name of the task (4 characters max)
*  @param   stackSize   (in)  task stack size (bytes)
*  @param   taskClass   (in)  task priority class
*  @param   taskInit    (in)  task initialization function
*  @param   taskMain    (in)  task main function
*  @param   taskDeinit  (in)  task de-initialization function
*  @param   argument    (in)  the task argument passed to <taskMain>
*  @param   taskId      (out)  task identifier
*
*  @return  BOS_STATUS_OK if successful, error code on failure
*/

#if BOS_CFG_RESET_SUPPORT
   #define bosTaskResetCreate(nm, sz, cl, i, m, d, arg, id)   \
            bosTaskCreateMain(nm, sz, cl, i, m, d, arg, id, BOS_FALSE, BOS_TASKSTART_RUNNING)
#else
   /* this function is not supported if reset is disabled */
   #define bosTaskResetCreate    \
               err_bosTaskResetCreate_not_supported
#endif


/***************************************************************************/
/**
*  Create a task that can be reset (in SUSPENDED mode)
*
*  @param   name        (in)  name of the task (4 characters max)
*  @param   stackSize   (in)  task stack size (bytes)
*  @param   taskClass   (in)  task priority class
*  @param   taskInit    (in)  task initialization function
*  @param   taskMain    (in)  task main function
*  @param   taskDeinit  (in)  task de-initialization function
*  @param   argument    (in)  the task argument passed to <taskMain>
*  @param   taskId      (out)  task identifier
*
*  @return  BOS_STATUS_OK if successful, error code on failure
*
*  @note    You must call bosTaskStart() to start the task
*/

#if BOS_CFG_RESET_SUPPORT
   #define bosTaskResetCreateSuspended(nm, sz, cl, i, m, d, arg, id)   \
            bosTaskCreateMain(nm, sz, cl, i, m, d, arg, id, BOS_FALSE, BOS_TASKSTART_SUSPENDED)
#else
   /* this function is not supported if reset is disabled */
   #define bosTaskResetCreateSuspended    \
               err_bosTaskResetCreateSuspended_not_supported
#endif


/***************************************************************************/
/**
*  Create an un-interruptible task that can be reset. Uninterruptible tasks do
*  not allocate or use OS resources, and therefore are not informed of
*  application resets.
*
*  @param   name        (in)  name of the task (4 characters max)
*  @param   stackSize   (in)  task stack size (bytes)
*  @param   taskClass   (in)  task priority class
*  @param   taskInit    (in)  task initialization function
*  @param   taskMain    (in)  task main function
*  @param   taskDeinit  (in)  task de-initialization function
*  @param   argument    (in)  the task argument passed to <taskMain>
*  @param   taskId      (out)  task identifier
*
*  @return  BOS_STATUS_OK if successful, error code on failure
*/

#if BOS_CFG_RESET_SUPPORT
   #define bosTaskResetCreateUnInterruptible(nm, sz, cl, i, m, d, arg, id)   \
            bosTaskCreateMain(nm, sz, cl, i, m, d, arg, id, BOS_TRUE, BOS_TASKSTART_RUNNING)
#else
   /* this function is not supported if reset is disabled */
   #define bosTaskResetCreateUnInterruptible \
               err_bosTaskResetCreateUnInterruptible_not_supported
#endif


/***************************************************************************/
/**
*  Start a task which was created in suspended mode
*
*  @param   taskId   (in)  task identifier
*
*  @return  BOS_STATUS_OK if successful, error code on failure
*/

#if BOS_CFG_RESET_SUPPORT
   BOS_STATUS bosTaskStart( BOS_TASK_ID *taskId );
#else
   /* this function is not supported if reset is disabled */
   #define bosTaskStart          err_bosTaskStart_not_supported
#endif


/***************************************************************************/
/**
*  Checks if the calling task has been notified of a pending reset
*
*  @return  BOS_STATUS_OK if successful, error code on failure
*/
#if BOS_CFG_RESET_SUPPORT
   BOS_BOOL bosTaskIsResetPending( void );
#else
   /* since reset is disabled, we do not have any pending resets */
   #define bosTaskIsResetPending()    BOS_FALSE
#endif


/***************************************************************************/
/**
*  This is a blocking call that informs a task of a reset and waits for the
*  task to ACK the reset.
*
*  @param   taskId   (in)  task identifier
*
*  @return  BOS_STATUS_OK if successful, error code on failure
*/

#if BOS_CFG_RESET_SUPPORT
   BOS_STATUS bosTaskResetNotify( BOS_TASK_ID *taskid );
#else
   /* this function is not supported if reset is disabled */
   #define bosTaskResetNotify    err_bosTaskResetNotify_not_supported
#endif


/***************************************************************************/
/**
*  This function should be invoked by tasks to acknowledge that they have been
*  notified that a reset is pending. Calling this function will cause the
*  calling task to block until it is placed into the reset-active state.
*
*  @return  BOS_STATUS_OK if successful, error code on failure
*/

#if BOS_CFG_RESET_SUPPORT
   BOS_STATUS bosTaskResetAck( void );
#else
   /* this function should not be called when reset is disabled */
   #define bosTaskResetAck()        BOS_ASSERT( 0 )
#endif


/***************************************************************************/
/**
*  This function will change the state of a task to 'shutdown' (from reset-ack).
*  Tasks are blocked in the reset-ack state. Changing the task's state to
*  'shutdown' will cause the task to resume operation. In this state, tasks
*  should perform operations required for task shutdown before exiting their
*  main task function.
*
*  @param   taskId   (in)  task identifier
*
*  @return  BOS_STATUS_OK if successful, error code on failure
*/
#if BOS_CFG_RESET_SUPPORT
   BOS_STATUS bosTaskProceedToShutdown( BOS_TASK_ID *taskid );
#else
   /* this function should not be called when reset is disabled */
   #define bosTaskProceedToShutdown( taskid )   BOS_ASSERT( 0 )
#endif


/***************************************************************************/
/**
*  This function will block until the specified task is "done". A task is
*  considered done once it has exited from it's main task function and
*  completed execution of the task de-init function.
*
*  @param   taskId   (in)  task identifier
*
*  @return  BOS_STATUS_OK if successful, error code on failure
*/
#if BOS_CFG_RESET_SUPPORT
   BOS_STATUS bosTaskWaitForDone( BOS_TASK_ID *taskid );
#else
   /* this function should not be called when reset is disabled */
   #define bosTaskWaitForDone( taskid )   BOS_ASSERT( 0 )
#endif


/***************************************************************************/
/**
*  This function is responsible for spawning the application, allocating
*  resources, and initializing local storage.  If reset support is enabled,
*  the application initialization function is invoked asynchronously.  In
*  addition, the application de-initialization is invoked when application
*  reset is initiated.  This function should wait for tasks to complete the
*  reset, and de-allocate resources.
*
*  @param   appInit  (in)  application initialization function
*  @param   appDeinit(in)  application deinitialization function
*
*  @return  BOS_STATUS_OK if successful, error code on failure
*/
void bosAppStart( BOS_APP_INIT_FNC appInit, BOS_APP_DEINIT_FNC appDeinit );


/***************************************************************************/
/**
*  Initiate an application reset
*
*  @return  BOS_STATUS_OK if successful, error code on failure
*
*  @note    The application reset is performed asynchronously, this function
*           returns immediately
*/

#if BOS_CFG_RESET_SUPPORT
   void bosAppReset( void );
#else
   /* this function is not supported if reset is disabled */
   #define bosAppReset           err_bosAppReset_not_supported
#endif


/***************************************************************************/
/**
*  Enable reset detection
*/

#if BOS_CFG_RESET_SUPPORT
   void bosAppResetDetectionEnable( void );
#else
   /* this function is not supported if reset is disabled */
   #define bosAppResetDetectionEnable  \
               err_bosAppResetDetectionEnable_not_supported
#endif


/***************************************************************************/
/*
*  Inhibit reset detection (resets won't be processed until enabled)
*
*  @return  BOS_STATUS_OK if successful, error code on failure
*/

#if BOS_CFG_RESET_SUPPORT
   BOS_STATUS bosAppResetDetectionInhibit( void );
#else
   /* this function is not supported if reset is disabled */
   #define bosAppResetDetectionInhibit \
               err_bosAppResetDetectionInhibit_not_supported
#endif


/***************************************************************************/
/**
*  Initializes the BOS Task module.
*
*  @return  BOS_STATUS_OK if the initialization was successful, or an error
*           code if some type of error occured.
*/
BOS_STATUS bosAppInit( void );


/***************************************************************************/
/**
*  Determine if calling task has been notified of a pending reset
*
*  @return  BOS_TRUE or BOS_FALSE
*/

#if BOS_CFG_RESET_SUPPORT
   BOS_BOOL bosResourceIsResetPending( void );
#else
   /* this function is not supported if reset is disabled */
   #define bosResourceIsResetPending   \
               err_bosResourceIsResetPending_not_supported
#endif

#if BOS_CFG_RESET_SUPPORT
/***************************************************************************/
/**
*  Set the task such that it is not counted for during application reset.
 *
*  @param   taskId         (in) ID of task to set
*
*  @return  BOS_STATUS_OK if successful, error code on failure
 */
BOS_STATUS bosTaskSetNoReset ( BOS_TASK_ID *taskId );
#endif

#if defined( __cplusplus )
}
#endif

#endif   /* BOSTASK_H */

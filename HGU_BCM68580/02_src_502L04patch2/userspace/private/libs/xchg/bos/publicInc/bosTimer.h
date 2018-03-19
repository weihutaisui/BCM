/****************************************************************************
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
*  @file    bosTimer.h
*
*  @brief   Contains the BOS Timer interface.
*
****************************************************************************/
/**
*  @defgroup   bosTimer   BOS Timer
*
*  @brief   Provides operating system timers.
*
*           Timers are an OS component that keep track of elapsed time, and are
*           run off of the main system clock.  When the specified time
*           interval has elapsed, the OS abstraction internally sends an event
*           to the thread that owns it. This will unblock the task that is
*           waiting for the timer to expire.
*
*           Timers can be used in a one-shot or a repeating mode.
*
*           NOTE:  Unlike other BOS primitives, where some of the functions are
*           meant to be used by other tasks, all of the functions of BOS timers
*           can only be operated on by the task that created/owns it.  This is
*           because the event is only valid in the context of the owning task,
*           and in some operating systems a timer must be started by the task
*           that owns it. For this reason, it must never be accessible outside of
*           the task.
*
*
****************************************************************************/

#if !defined( BOSTIMER_H )
#define BOSTIMER_H            /**< Include Guard                           */

/* ---- Include Files ---------------------------------------------------- */

#if !defined( BOSCFG_H )
#  include <bosCfg.h>
#endif

#if !defined( BOSTYPES_H )
#  include <bosTypes.h>
#endif

#if !defined( BOSEVENT_H )
#  include <bosEvent.h>
#endif


#if !BOS_CFG_TIMER
#error   BOS timer module is not selected in bosCfg.h
#endif

#if BOS_OS_ECOS
#  include <../eCos/bosTimerECOS.h>
#elif BOS_OS_PSOS
#  include <../pSOS/bosTimerPSOS.h>
#elif BOS_OS_VXWORKS
#  include <../VxWorks/bosTimerVxWorks.h>
#elif BOS_OS_WIN32
#  include <../Win32/bosTimerWin32.h>
#elif BOS_OS_LINUXKERNEL
#  include <../LinuxKernel/bosTimerLinuxKernel.h>
#elif BOS_OS_LINUXUSER
#  include <../LinuxUser/bosTimerLinuxUser.h>
#else
#  error Unknown OS configuration from bosCfg.h
#endif

#if defined( __cplusplus )
extern "C"
{
#endif

/**
 * @addtogroup bosTimer
 * @{
 */

/* ---- Constants and Types ---------------------------------------------- */

/**
 * Function index, used with the error handler to report which function
 * an error originated from.
 *
 * These constants would typically only be used if you provide your own
 * error handler.
 *
 * @note If you change anything in this enumeration, please ensure that
 *       you make the corresponding updates in the bosTimerInitFunctionNames
 *       routine.
 */

typedef enum
{
   BOS_TIMER_INIT_FUNC_ID,          /**< bosTimerInit                      */
   BOS_TIMER_TERM_FUNC_ID,          /**< bosTimerTerm                      */
   BOS_TIMER_CREATE_FUNC_ID,        /**< bosTimerCreate                    */
   BOS_TIMER_DESTROY_FUNC_ID,       /**< bosTimerDestroy                   */
   BOS_TIMER_START_FUNC_ID,         /**< bosTimerStart                     */
   BOS_TIMER_RESTART_FUNC_ID,       /**< bosTimerRestart                   */
   BOS_TIMER_STOP_FUNC_ID,          /**< bosTimerStop                      */
   BOS_TIMER_WAIT_FUNC_ID,          /**< bosTimerWait                      */

   BOS_TIMER_NUM_FUNC_ID   /**< Number of functions in the Timer module.   */

} BOS_TIMER_FUNC_ID;


typedef enum
{
   /* One-shot timer - when it expires, it triggers the event, then sits idle.
   ** You can call bosTimerRestart to get it to run again with the same
   ** parameters. */
   BOS_TIMER_MODE_ONCE,

   /* Periodic timer - when it expires, it triggers the event, and continues
   ** running. */
   BOS_TIMER_MODE_REPEAT

} BOS_TIMER_MODE;


/* ---- Variable Externs ------------------------------------------------- */

/**
 * Contains string versions of the timer function names. This table is
 * indexed using values from the BOS_TIMER_FUNC_ID enumeration.
 */

extern const char *gBosTimerFunctionName[];

/* ---- Function Prototypes ---------------------------------------------- */

/***************************************************************************/
/**
*  Initializes the BOS Timer module.
*
*  @return  BOS_STATUS_OK if the initialization was successful, or an error
*           code if some type of error occured.
*/

BOS_STATUS bosTimerInit( void );

/***************************************************************************/
/**
*  Terminates the BOS Timer module.
*
*  @return  BOS_STATUS_OK if the termination was successful, or an error
*           code if some type of error occured.
*/

BOS_STATUS bosTimerTerm( void );

/***************************************************************************/
/**
*  Creates a timer object.
*
*  @param   timer (out) Timer object to initialize.
*
*  @return  BOS_STATUS_OK if the timer was created successfully, or an
*           error code if the timer could not be created.
*/

BOS_STATUS bosTimerCreate( BOS_TIMER *timer );

/***************************************************************************/
/**
*  Destroys a previously created timer object.
*
*  @param   timer (mod) Timer object to destroy.
*
*  @return  BOS_STATUS_OK if the timer was destroyed successfully, or an
*           error code if the timer could not be destroyed.
*/

BOS_STATUS bosTimerDestroy( BOS_TIMER *timer );

/***************************************************************************/
/**
*  Starts the timer running. The timer will automatically restart if the mode
*  is set to repeating.
*
*  @param   timer       (mod) Timer object to start.
*           timeoutMsec (in)  Milliseconds that timer should count before expiring.
*           mode        (in)  Timer mode (one-shot or periodic).
*
*  @return  BOS_STATUS_OK if the timer was started, or an error code if
*           the timer could not be started.
*
*/

BOS_STATUS bosTimerStart
(
   BOS_TIMER        *timer,
   unsigned int      timeoutMsec,
   BOS_TIMER_MODE    mode
);

/***************************************************************************/
/**
*  Stops the timer and restarts it with the timeout value and mode that were
*  sent in on the last call to bosTimerStart(). If bosTimerStart() has not been
*  called, then the timer is no restarted.
*
*  This is useful because it eliminates extra calls to the object, and you don't
*  have to know what the previous values were in order to restart it.
*
*  @param   timer       (mod) Timer object to re-start.
*
*  @return  BOS_STATUS_OK if the timer was started, or an error code if
*           the timer could not be started.
*
*/

BOS_STATUS bosTimerRestart( BOS_TIMER *timer );

/***************************************************************************/
/**
*  Stops the timer that is running (preventing it from sending the event).
*  There may be a race condition where the timer expires (handled by the OS
*  at interrupt time) as this function is executing, which will cause the
*  event to be triggered.
*
*  @param   timer       (mod) Timer object to stop
*
*  @return  BOS_STATUS_OK if the timer was stopped, or an error code if
*           the timer could not be stopped.
*
*/

BOS_STATUS bosTimerStop( BOS_TIMER *timer );

/***************************************************************************/
/**
*  Causes the calling thread to be blocked until the timer expires.
*
*  @param   timer       (mod) Timer object to wait on.
*
*  @return  BOS_STATUS_OK if the timer expired, or an error code if
*           the timer was not started.
*
*/

BOS_STATUS bosTimerWait( BOS_TIMER *timer );


/***************************************************************************/
/**
*  Retrieves the internal event object used by the timer as a signalling
*  primitive. This is typically used to add a timer to an event-set object.
*  Event-sets are a container of event objects, and other OS primitives that
*  use events as their synchronization primitive. They allow users to wait
*  for one, or all or a set of events to occur, e.g. for an event to
*  be sent, or a timer to expire; or an event to be send and a timer to expire
*  before unblocking.
*
*  @param   timer       (mod) Timer object to wait on.
*
*  @return  BOS_STATUS_OK if the timer expired, or an error code if
*           the timer was not started.
*
*/

BOS_STATUS bosTimerGetEvent( BOS_TIMER *timer, BOS_EVENT **event );


/** @} */

#if defined( __cplusplus )
}
#endif

#endif /* BOSTIMER_H */

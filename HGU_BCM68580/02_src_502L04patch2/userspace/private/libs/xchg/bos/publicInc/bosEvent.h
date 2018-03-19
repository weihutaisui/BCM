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
****************************************************************************
*
*    Filename: bosEvent.h
*
****************************************************************************
*    Description:
*
*     BOS events are generally used as a means of simple communication between
*     tasks, when one task needs to tell another task that a particular
*     event has occurred.  BOS events contain no other information about the
*     event, such as how many times it occurred, when it occurred, who sent
*     it, or any other data associated with the event.
*
*     When an event is created, you must specify the task that wants to receive
*     the notification.  This is because the event is valid only in the context
*     of the receiving task.  The sender of the event (who calls bosEventSend())
*     can be any task, including the task that created the event.
*
*     BOS event sets are a container class that allows individual sources of
*     events (like BOS events, BOS timer, etc) to be grouped together and
*     operated on simultaneously.  This is necessary because BOS events,
*     BOS timers, and others only allow you to operate on that particular
*     instance.  The primary use of this object is to allow a task to wait for
*     one or more of a set of events to occur.
*
*     Any BOS primitive that has an associated BOS event (such as BOS timer)
*     can be added to the event set.  For example, this allows a task to wait
*     for an event to be sent, or a timer to expire, which ever comes first.
*     A task can also wait for a combination of events before returning, such as
*     a received message and a timer expiration.
*
****************************************************************************/

#ifndef BOSEVENT_H
#define BOSEVENT_H

#ifndef BOSCFG_H
#include <bosCfg.h>     /* include application-specific BOS config file */
#endif

#ifndef BOSTYPES_H
#include "bosTypes.h"
#endif

#ifndef BOSTASK_H
#include "bosTask.h"
#endif

#if !BOS_CFG_EVENT
#error bos Event module not selected in bosCfg.h
#endif

#if BOS_OS_ECOS
#  include <../eCos/bosEventECOS.h>
#elif BOS_OS_PSOS
#  include <../pSOS/bosEventPSOS.h>
#elif BOS_OS_VXWORKS
#  include <../VxWorks/bosEventVxWorks.h>
#elif BOS_OS_WIN32
#  include <../Win32/bosEventWin32.h>
#elif BOS_OS_LINUXKERNEL
#  include <../LinuxKernel/bosEventLinuxKernel.h>
#elif BOS_OS_LINUXUSER
#  include <../LinuxUser/bosEventLinuxUser.h>
#else
#  error Unknown OS configuration from bosCfg.h
#endif


#ifdef          __cplusplus
extern  "C" {                           /* C declarations in C++ */
#endif

/**
 * @addtogroup bosEvent
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
 *       you make the corresponding updates in the bosEventInitFunctionNames
 *       routine.
 */

typedef enum
{
   /* BOS event functions. */
   BOS_EVENT_INIT_FUNC_ID,                   /**< bosEventInit                */
   BOS_EVENT_TERM_FUNC_ID,                   /**< bosEventTerm                */
   BOS_EVENT_CREATE_FUNC_ID,                 /**< bosEventCreate              */
   BOS_EVENT_DESTROY_FUNC_ID,                /**< bosEventDestroy             */
   BOS_EVENT_RECEIVE_FUNC_ID,                /**< bosEventReceive             */
   BOS_EVENT_TIMED_RECEIVE_FUNC_ID,          /**< bosEventTimedReceive        */
   BOS_EVENT_SEND_FUNC_ID,                   /**< bosEventSend                */

   /* BOS event set functions. */
   BOS_EVENT_SET_CREATE_FUNC_ID,             /**< bosEventSetCreate           */
   BOS_EVENT_SET_DESTROY_FUNC_ID,            /**< bosEventSetDestroy          */
   BOS_EVENT_SET_ADD_EVENT_FUNC_ID,          /**< bosEventSetAddEvent         */
   BOS_EVENT_SET_REMOVE_EVENT_FUNC_ID,       /**< bosEventSetRemoveEvent      */
   BOS_EVENT_SET_REMOVE_ALL_EVENTS_FUNC_ID,  /**< bosEventSetRemoveAllEvents  */
   BOS_EVENT_SET_TIMED_RECEIVE_FUNC_ID,      /**< bosEventSetTimedReceive     */
   BOS_EVENT_SET_RECEIVE_FUNC_ID,            /**< bosEventSetReceive          */
   BOS_EVENT_SET_IS_EVENT_SET_FUNC_ID,       /**< bosEventSetIsEventSet       */

   BOS_EVENT_NUM_FUNC_ID   /**< Number of functions in the Event module.   */

} BOS_EVENT_FUNC_ID;


typedef enum
{
   /* Wait for ANY event to be sent before returning from bosEventSetReceive(). */
   BOS_EVENT_SET_WAIT_MODE_ANY,

   /* Wait for ALL events to be sent before returning from bosEventSetReceive(). */
   BOS_EVENT_SET_WAIT_MODE_ALL,

} BOS_EVENT_SET_WAIT_MODE;


/* ---- Variable Externs ------------------------------------------------- */

/**
 * Contains string versions of the event function names. This table is
 * indexed using values from the BOS_EVENT_FUNC_ID enumeration.
 */

extern const char *gBosEventFunctionName[];

/* ---- Function Prototypes ---------------------------------------------- */

#if ( BOS_CFG_RESET_SUPPORT )

   #if ( !BUILDING_BOS )
      /* If reset support is enabled, application code (non-OS) will use the
      ** re-defined function prototypes for the reset-aware versions of the following
      ** blocking functions */
      #define bosEventReceive          bosEventReceiveR
      #define bosEventTimedReceive     bosEventTimedReceiveR
      #define bosEventSetReceive       bosEventSetReceiveR
      #define bosEventSetTimedReceive  bosEventSetTimedReceiveR
   #else
      /****************************************************************************/
      /**
      *  Wait for an event to be sent to the calling task
      *
      *  @param   event    (in)  event to wait for
      *           timeout  (in)  timeout - event timeout delay (in ticks)
      *
      *  @return  BOS_STATUS_OK or failure
      *
      *  @note    bosEventReceiveR() is the reset equivalent of bosEventReceive().
      *           It will return BOS_STATUS_RESET if the calling task has been
      *           notified of a reset.
      */
      BOS_STATUS bosEventTimedReceiveR( BOS_EVENT *event, BOS_TIME_MS timeout );
      BOS_STATUS bosEventSetTimedReceiveR( BOS_EVENT_SET *event, BOS_EVENT_SET_WAIT_MODE mode, BOS_TIME_MS timeout );
   #endif

#endif   /* BOS_CFG_RESET_SUPPORT */

/***************************************************************************/
/**
*  Initializes the BOS Event module.
*
*  @return  BOS_STATUS_OK if the initialization was successful, or else an
*           error code.
*
*/
BOS_STATUS bosEventInit( void );

/***************************************************************************/
/**
*  Terminates the BOS Event module.
*
*  @return  BOS_STATUS_OK if the termination was successful, or else an
*           error code.
*
*/
BOS_STATUS bosEventTerm( void );

/***************************************************************************/
/**
*  Creates a event synchronization object.
*
*  @param   taskId   (in)  Id of task that event will be sent to.
*  @param   event    (out) Event object to initialize.
*
*  @return  BOS_STATUS_OK if the event was created successfully, or an
*           error code if the event could not be created.
*/
BOS_STATUS bosEventCreate( BOS_TASK_ID *taskId, BOS_EVENT *event );

/***************************************************************************/
/**
*  Destroys a previously created event object.
*
*  @param   event (mod) Event object to destroy.
*
*  @return  BOS_STATUS_OK if the event was destroyed successfully, or an
*           error code if the event could not be destroyed.
*/
BOS_STATUS bosEventDestroy( BOS_EVENT *event );

/***************************************************************************/
/**
*  Wait for an event to be sent to the calling task.
*
*  @param   event (mod)    Event object to wait for.
*
*  @return  BOS_STATUS_OK if the event was received, or an error code if the
*           event could not be received for another reason.
*
*/
BOS_STATUS bosEventReceive( BOS_EVENT *event );

/***************************************************************************/
/**
*  Wait for an event to be sent to the calling task.
*
*  @param   event (mod)    Event object to wait for.
*           timeout  (in)  Time to wait for event.
*
*  @return  BOS_STATUS_OK if the event was received, BOS_STATUS_TIMEOUT if
*           the timeout period expired, or an error code if the event could
*           not be received for another reason.
*
*  @note:   Calling bosEventTimedReceive( event, BOS_WAIT_FOREVER ) is
*           equivalent to calling bosEventReceive( event ).
*/
BOS_STATUS bosEventTimedReceive( BOS_EVENT *event, BOS_TIME_MS timeout );

#define bosEventReceiveR( event )   bosEventTimedReceiveR( event, BOS_WAIT_FOREVER )


/***************************************************************************/
/**
*  Post an event. This will cause the task blocked waiting to receive the event
*  to wake up.
*
*  @param   event (mod) Event object to send.
*
*  @return  BOS_STATUS_OK if the event was sent, or an error code if
*           the event could not be sent.
*/
BOS_STATUS bosEventSend( BOS_EVENT *event );

/***************************************************************************/
/**
*  Clears any pending (stale) event that may already have been processed.  It
*  is commonly called just before calling bosEventReceive().
*
*  This is equivalent to calling bosEventReceive(event, 0) and ignoring the result.
*
*  @param   event (mod) Event object to clear.
*
*  @return  BOS_STATUS_OK if the event was cleared, or an error code if
*           the event could not be cleared.
*/
BOS_STATUS bosEventClear( BOS_EVENT *event );



/* --------------------------------------------------------------------------
** Event Set API.
*/

/***************************************************************************/
/**
*  Creates a event set synchronization object.
*
*  @param   taskId   (in)  Id of task that events will be sent to.
*  @param   eventSet (out) Event set object to initialize.
*
*  @return  BOS_STATUS_OK if the event set was created successfully, or an
*           error code if the event set could not be created.
*/
BOS_STATUS bosEventSetCreate( BOS_TASK_ID *taskId, BOS_EVENT_SET *eventSet );

/***************************************************************************/
/**
*  Destroys a previously created event set object.
*
*  @param   eventSet (mod) Event set object to destroy.
*
*  @return  BOS_STATUS_OK if the event set was destroyed successfully, or an
*           error code if the event set could not be destroyed.
*
*  @note    This only destroys the event set, NOT the events contained within
*           the set. It is the user's responsibility to delete the individual
*           events.
*/
BOS_STATUS bosEventSetDestroy( BOS_EVENT_SET *eventSet );

/***************************************************************************/
/**
*  Adds an event to an event-set.
*
*  @param   eventSet (mod) Event-set to add event to.
*           event    (in)  Event to be added.
*
*  @return  BOS_STATUS_OK if the event was added successfully, or an
*           error code if the event could not be added.
*/
BOS_STATUS bosEventSetAddEvent( BOS_EVENT_SET *eventSet, BOS_EVENT *event );

/***************************************************************************/
/**
*  Removes an event from an event-set.
*
*  @param   eventSet (mod) Event-set to remove event from.
*           event    (in)  Event to be removed.
*
*  @return  BOS_STATUS_OK if the event was removed successfully, or an
*           error code if the event could not be removed.
*/
BOS_STATUS bosEventSetRemoveEvent( BOS_EVENT_SET *eventSet, BOS_EVENT *event );

/***************************************************************************/
/**
*  Removes all events from an event-set.
*
*  @param   eventSet (mod) Event-set to remove events from.
*
*  @return  BOS_STATUS_OK if the events were removed successfully, or an
*           error code if the events could not be removed.
*/
BOS_STATUS bosEventSetRemoveAllEvents( BOS_EVENT_SET *eventSet );

/***************************************************************************/
/**
*  Wait for an event to be sent to the calling task.
*
*  @param   eventSet (mod) Event set object to wait for.
*           mode     (in)  Wait for all events, or any event.
*
*  @return  BOS_STATUS_OK if an event was received, or an error code if an
*           event could not be received for another reason.
*
*/
BOS_STATUS bosEventSetReceive
(
   BOS_EVENT_SET             *eventSet,
   BOS_EVENT_SET_WAIT_MODE    mode
);

/***************************************************************************/
/**
*  Wait for an event to be sent to the calling task.
*
*  @param   eventSet (mod) Event set object to wait for.
*           mode     (in)  Wait for all events, or any event.
*           timeout  (in)  Time to wait for event.
*
*  @return  BOS_STATUS_OK if the event was received, BOS_STATUS_TIMEOUT if
*           the timeout period expired, or an error code if the event could
*           not be received for another reason.
*
*  @note:   Calling bosEventSetTimedReceive( event, BOS_WAIT_FOREVER ) is
*           equivalent to calling bosEventSetReceive( event ).
*/
BOS_STATUS bosEventSetTimedReceive
(
   BOS_EVENT_SET             *eventSet,
   BOS_EVENT_SET_WAIT_MODE    mode,
   BOS_TIME_MS                timeout
);

#define bosEventSetReceiveR( eventSet, mode )   \
            bosEventSetTimedReceiveR( (eventSet), (mode), BOS_WAIT_FOREVER )

/***************************************************************************/
/**
** This function should be used by a task to determine if the specified
** event has been triggered.  This is usually done immediately after a
** call to bosEventSetReceive() returns.
*
*  @param   eventSet (mod) Event set object.
*           event    (in)  The event to check for being set.
*
*  @return  BOS_TRUE if the event was received, or BOS_FALSE if the
*           event was not received.
*/
BOS_BOOL bosEventSetIsEventSet( BOS_EVENT_SET *eventSet, BOS_EVENT *event );


#ifdef          __cplusplus
}                               /* End of C declarations */
#endif

#endif   /* BOSEVENT_H */


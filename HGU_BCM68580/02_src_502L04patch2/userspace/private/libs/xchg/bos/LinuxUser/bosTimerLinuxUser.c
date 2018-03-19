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
*  @file    bosTimerECOS.c
*
*  @brief   ECOS implementation of the BOS Timer Module
*
****************************************************************************/

/* ---- Include Files ---------------------------------------------------- */

#include <bosCfg.h>

#if BOS_CFG_TIMER

#include <stdio.h>
#include <string.h>
#include <bosError.h>
#include <bosErrorLinuxUser.h>
#include <bosLog.h>
#include <bosSleep.h>
#include <bosTimer.h>
#include <bosTimerPrivate.h>
#include <bosLinuxUser.h>
#include <bosEvent.h>


/* ---- Public Variables ------------------------------------------------- */
/* ---- Private Constants and Types -------------------------------------- */
/* ---- Private Variables ------------------------------------------------ */

static   BOS_BOOL gBosTimerInitialized = BOS_FALSE;


/* ---- Private Function Prototypes -------------------------------------- */

static void bosTimerLinuxUserMain( BOS_TASK_ARG taskArg );
static void bosTimerTaskInitCB( BOS_TASK_ARG taskArg );
static void bosTimerTaskDeinitCB( BOS_TASK_ARG taskArg );


/* ---- Functions -------------------------------------------------------- */

/***************************************************************************/

BOS_STATUS bosTimerInit( void )
{
   BOS_ASSERT( !gBosTimerInitialized );

   bosLogInfo("bosTimerInit");

   bosTimerInitFunctionNames();

   gBosTimerInitialized = BOS_TRUE;

   return BOS_STATUS_OK;

} /* bosTimerInit */

/***************************************************************************/

BOS_STATUS bosTimerTerm( void )
{
   BOS_ASSERT( gBosTimerInitialized );

   gBosTimerInitialized = BOS_FALSE;

   return BOS_STATUS_OK;

} /* bosTimerTerm */

/***************************************************************************/

BOS_STATUS bosTimerCreate( BOS_TIMER *timer )
{
   BOS_STATUS            status;
   BOS_TASK_EXTENDED_ARG argument = { BOS_TASK_AFFINITY_ALL_TP, timer };

   bosLogInfo("bosTimerCreate");

   BOS_ASSERT( gBosTimerInitialized );

   memset( timer, 0, sizeof( *timer ) );

   timer->lastTimeoutMsec  = BOS_WAIT_FOREVER;
 
   status = bosTaskGetMyTaskId( &timer->taskId );
   BOS_ASSERT( status == BOS_STATUS_OK );

   status = bosEventCreate( &timer->taskId, &timer->event );
   BOS_ASSERT( status == BOS_STATUS_OK );


   /* Get the kernel to create the alarm object, associated with the realtime
   ** clock.  We need to specify the callout function (and data) that will be
   ** called every time the alarm occurs. */
   status = bosTaskCreateEx( BOS_TIMER_LINUXUSER_TASK_NAME,
                           BOS_TIMER_LINUXUSER_TASK_STACK,
                           BOS_TIMER_LINUXUSER_TASK_PRIORITY,
                           bosTimerTaskInitCB,
                           bosTimerLinuxUserMain,
                           bosTimerTaskDeinitCB,
                           &argument,
                           &(timer->timerId));

   return ( BOS_STATUS_OK );

} /* bosTimerCreate */

/***************************************************************************/

BOS_STATUS bosTimerDestroy( BOS_TIMER *timer )
{
   BOS_ASSERT( gBosTimerInitialized );
   BOS_ASSERT( timer  != NULL );

   if ( timer->bIsRunning )
   {
      bosTimerStop( timer );
   }

   bosTaskDestroy( &timer->timerId );

   bosEventDestroy( &timer->event );


   return ( BOS_STATUS_OK );

} /* bosTimerDestroy */

/***************************************************************************/

BOS_STATUS bosTimerStart
(
   BOS_TIMER        *timer,
   unsigned int      timeoutMsec,
   BOS_TIMER_MODE    mode
)
{
   BOS_TASK_ID    taskId;

   bosLogInfo("bosTimerStart");
   /* Error checks. */
   bosTaskGetMyTaskId( &taskId );
   if ( taskId != timer->taskId )
   {
      bosLogErr("bosTimerStart - Calling task does not own timer");
      return ( BOS_STATUS_ERR );
   }

   /* If the timer is already running, then stop it before restarting it. */
   if ( timer->bIsRunning )
   {
      bosTimerStop( timer );
   }

   /* Store these for use by the Restart() method in the base class. */
   timer->lastTimeoutMsec  = timeoutMsec;
   timer->lastTimerMode    = mode;

   /* Clear any pending event notifications. */
   bosEventClear( &timer->event );

   /* Start the alarm. */
   bosEventSend( &timer->eventStart );

   return ( BOS_STATUS_OK );

} /* bosTimerStart */

/***************************************************************************/

BOS_STATUS bosTimerRestart( BOS_TIMER *timer )
{
   bosLogInfo("bosTimerRestart");
   /* See if Start() has been called; if not, then we can't do this. */
   if ( !timer->bIsRunning )
   {
      bosLogErr("bosTimerRestart - Timer not started. Can't restart...");
      return ( BOS_STATUS_ERR );
   }

   /* Start the timer with the previous values. */
   return ( bosTimerStart( timer, timer->lastTimeoutMsec, timer->lastTimerMode ) );

} /* bosTimerRestart */

/***************************************************************************/

BOS_STATUS bosTimerStop( BOS_TIMER *timer )
{
   BOS_TASK_ID    taskId;

   bosLogInfo("bosTimerStop");

   /* Error checks. */
   bosTaskGetMyTaskId( &taskId );
   if ( taskId != timer->taskId )
   {
      bosLogErr("bosTimerStop - Calling task does not own timer");
      return ( BOS_STATUS_ERR );
   }


   if ( timer->bIsRunning )
   {
      timer->lastTimeoutMsec  = BOS_WAIT_FOREVER;
      bosEventSend( &timer->eventStop );
   }

   return ( BOS_STATUS_OK );

} /* bosTimerStop */

/***************************************************************************/

BOS_STATUS bosTimerWait( BOS_TIMER *timer )
{
   BOS_TASK_ID    taskId;

   bosLogInfo("bosTimerWait");

   /* Error checks... */
   if ( !timer->bIsRunning )
   {
      bosLogErr("bosTimerWait - Timer not running. Can't wait for it...");
      return ( BOS_STATUS_ERR );
   }

   bosTaskGetMyTaskId( &taskId );
   if ( taskId != timer->taskId )
   {
      bosLogErr("bosTimerWait - Calling task does not own timer");
      return ( BOS_STATUS_ERR );
   }


#if BOS_CFG_RESET_SUPPORT
   /* Wait on the timer event that will be posted by the timer callout.
   ** Note that we need to call the reset-able version of EventReceive so
   ** that we will unblock if there is an application reset. */
   return ( bosEventReceiveR( &timer->event ) );
#else
   /* Wait on the timer event. */
   return ( bosEventReceive( &timer->event ) );
#endif

} /* bosTimerWait */


/***************************************************************************/

BOS_STATUS bosTimerGetEvent( BOS_TIMER *timer, BOS_EVENT **event )
{
   bosLogInfo("bosTimerGetEvent");

   *event = &timer->event;

   return ( BOS_STATUS_OK );
}



static void bosTimerTaskInitCB( BOS_TASK_ARG taskArg )
{
   BOS_TIMER *timer = (BOS_TIMER *)taskArg;

   BOS_ASSERT( bosEventSetCreate( &timer->timerId, &timer->apiEventSet ) == BOS_STATUS_OK);
   BOS_ASSERT( bosEventCreate( &timer->timerId, &timer->eventStart ) == BOS_STATUS_OK);
   BOS_ASSERT( bosEventCreate( &timer->timerId, &timer->eventStop ) == BOS_STATUS_OK);
   BOS_ASSERT( bosEventCreate( &timer->timerId, &timer->eventRestart ) == BOS_STATUS_OK);

   BOS_ASSERT( bosEventSetAddEvent( &timer->apiEventSet, &timer->eventStart ) == BOS_STATUS_OK );
   BOS_ASSERT( bosEventSetAddEvent( &timer->apiEventSet, &timer->eventStop ) == BOS_STATUS_OK );
   BOS_ASSERT( bosEventSetAddEvent( &timer->apiEventSet, &timer->eventRestart ) == BOS_STATUS_OK );

}

static void bosTimerTaskDeinitCB( BOS_TASK_ARG taskArg )
{
   BOS_TIMER *timer = (BOS_TIMER *)taskArg;

   /* Cleanup */
   BOS_ASSERT( bosEventSetRemoveAllEvents( &timer->apiEventSet ) == BOS_STATUS_OK);

   BOS_ASSERT( bosEventDestroy( &timer->eventStart ) == BOS_STATUS_OK);
   BOS_ASSERT( bosEventDestroy( &timer->eventStop ) == BOS_STATUS_OK);
   BOS_ASSERT( bosEventDestroy( &timer->eventRestart ) == BOS_STATUS_OK);

   BOS_ASSERT( bosEventSetDestroy( &timer->apiEventSet ) == BOS_STATUS_OK);
}

/**
 * @addtogroup bosTimerInternal
 * @{
 */

/***************************************************************************/
/**
*  eCos alarm handler function. This will be invoked each time the alarm
*  timer expires.
*
*  @param   alarmHandle (in)  Alarm id.
*  @param   data        (in)  User-specified data.
*/

static void bosTimerLinuxUserMain( void *data )
{
   BOS_STATUS status;
   BOS_TIMER  *timer = (BOS_TIMER *) data;

   while(BOS_TRUE)
   {
      /* Signal that the timer expired by sending the event. */
      status = bosEventSetTimedReceive( &timer->apiEventSet, BOS_EVENT_SET_WAIT_MODE_ANY, timer->lastTimeoutMsec );

      /* check the reason why we are awaken */ 
      if(status == BOS_STATUS_RESET )
      {
         /* this timer will be destroyed, clear any pending events */
         bosEventClear( &timer->event );
         break;
      }

      else if(status == BOS_STATUS_TIMEOUT /* && timer->bIsRunning == BOS_TRUE */ )
      {
         /* timer is running and timeout */
         bosEventSend( &timer->event );
      }
      else if(status == BOS_STATUS_OK )
      {
         /* get event from timer api interface */
         if( bosEventSetIsEventSet( &timer->apiEventSet, &timer->eventStart ) )
	 {
            /* timer started or restart*/
            timer->bIsRunning      = BOS_TRUE;
	 }
         else if( bosEventSetIsEventSet( &timer->apiEventSet, &timer->eventStop ) )
	 {
            timer->bIsRunning      = BOS_FALSE;
	 }
      }
   }
}

#endif   /* BOS_CFG_TIMER */

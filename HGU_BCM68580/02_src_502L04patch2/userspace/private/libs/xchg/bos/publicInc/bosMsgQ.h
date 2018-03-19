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
****************************************************************************
*
*  Filename: bosMsgQ.h
*
****************************************************************************
*  Description:
*
*
****************************************************************************/
/**
*
*  @file    bosMsgQ.h
*
*  @brief   Contains the BOS Message Queue interface.
*
****************************************************************************/
/**
*  @defgroup   bosMsgQ   BOS Message Queue
*
*  @brief      Provides message queue.
*
*  The BOS message queue interface contains implementation of message queues.
*
****************************************************************************/

#if !defined( BOSMSGQ_H )
#define BOSMSGQ_H              /**< Include Guard                           */

/* ---- Include Files ---------------------------------------------------- */

#if !defined( BOSCFG_H )
#  include <bosCfg.h>
#endif

#if !defined( BOSTYPES_H )
#  include <bosTypes.h>
#endif

#if !defined( BOSTASK_H )
#  include <bosTask.h>
#endif

#if !defined( BOSEVENT_H )
#  include <bosEvent.h>
#endif



#if !BOS_CFG_MSGQ
#error   BOS Message Queue module is not selected in bosCfg.h
#endif

#if BOS_OS_PSOS
#  include <../pSOS/bosMsgQPSOS.h>
#elif BOS_OS_VXWORKS
#  include <../VxWorks/bosMsgQVxWorks.h>
#elif BOS_OS_WIN32
#  include <../Win32/bosMsgQWin32.h>
#elif BOS_OS_LINUXKERNEL
#  include <../LinuxKernel/bosMsgQLinuxKernel.h>
#elif BOS_OS_LINUXUSER
#  include <../LinuxUser/bosMsgQLinuxUser.h>
#else
#  error Unknown OS configuration from bosCfg.h
#endif

#if defined( __cplusplus )
extern "C"
{
#endif

/**
 * @addtogroup bosMsgQ
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
 *       you make the corresponding updates in the bosMsgQInitFunctionNames
 *       routine.
 */

typedef enum
{
   BOS_MSGQ_INIT_FUNC_ID,           /**< bosMsgQInit                       */
   BOS_MSGQ_TERM_FUNC_ID,           /**< bosMsgQTerm                       */
   BOS_MSGQ_CREATE_FUNC_ID,         /**< bosMsgQCreate                     */
   BOS_MSGQ_DESTROY_FUNC_ID,        /**< bosMsgQDestroy                    */
   BOS_MSGQ_SEND_FUNC_ID,           /**< bosMsgQSend                       */
   BOS_MSGQ_RECV_FUNC_ID,           /**< bosMsgQReceive                    */
   BOS_MSGQ_TIMED_RECV_FUNC_ID,     /**< bosMsgQTimedReceive               */

   BOS_MSGQ_NUM_FUNC_ID   /**< Number of functions in the Message Queue module */

} BOS_MSGQ_FUNC_ID;

/**
 * Message queue element that is sent or received to/from the queue
 * NOTE: While VxWorks has no limit on the message size within a queue, PSOS
 *       has a limit of 16 bytes msgBuf[4].  To maintain a consistent
 *       interface acorss different operating systems, all message sizes are
 *       restricted to 16 bytes */
#ifndef BOS_MSGQ_MSG_MAX_SIZE
#  define BOS_MSGQ_MSG_MAX_SIZE    8 //4
#endif

typedef struct BOS_MSGQ_MSG
{
   BOS_UINT32 buffer[BOS_MSGQ_MSG_MAX_SIZE];
} BOS_MSGQ_MSG;

/* ---- Variable Externs ------------------------------------------------- */

/**
 * Contains string versions of the message queue function names. This table is
 * indexed using values from the BOS_MSGQ_FUNC_ID enumeration.
 */

extern const char *gBosMsgQFunctionName[];

/* ---- Function Prototypes ---------------------------------------------- */

#if ( BOS_CFG_RESET_SUPPORT )

   #if ( !BUILDING_BOS)
      /* If reset support is enabled, application code (non-OS) will use the
      ** re-defined function prototypes for the reset-aware versions of the following
      ** blocking functions */
      #define bosMsgQTimedReceive         bosMsgQTimedReceiveR
      #define bosMsgQReceive              bosMsgQReceiveR
   #else
      /*****************************************************************************/
      /**
      *  Receives a message from a specified oridinary message queue.  This
      *  function will wait infinitely until a message is available in the queue
      *
      *  @param   qid         (in)  Message queue identifier
      *  @param   timeoutMsec (in)  Number of milliseconds to wait for a
      *                             message to be posted at the queue.
      *  @param   qMsg        (out) Message buffer containing the retrieved message
      *
      *  @return  BOS_STATUS_OK if a message was retrieved successfully from the
      *           queue, or an error code if no message was obtained.
      *
      *  @note    bosMsgQTimedReceiveR() is the reset equivalent of bosMsgQTimedReceiveR().
      *           It will return BOS_STATUS_RESET if the calling task has been
      *           notified of a reset.
      */
      BOS_STATUS bosMsgQTimedReceiveR( BOS_MSGQ *qid, BOS_TIME_MS timeout, BOS_MSGQ_MSG *qMsg );
   #endif

#endif   /* BOS_CFG_RESET_SUPPORT */

/***************************************************************************/
/**
*  Initializes the BOS Message Queue module.
*
*  @return  BOS_STATUS_OK if the initialization was successful, or an error
*           code if some type of error occured.
*/

BOS_STATUS bosMsgQInit( void );

/***************************************************************************/
/**
*  Terminates the BOS Message Qeue module.
*
*  @return  BOS_STATUS_OK if the termination was successful, or an error
*           code if some type of error occured.
*/

BOS_STATUS bosMsgQTerm( void );

/***************************************************************************/
/**
*  Creates an ordinary FIFO message queue of size (qSize).
*
*  @param   name        (in)  4-character long messasge queue name
*  @param   qSize       (in)  Number of messages in the queue
*  @param   qid         (out) Message queue identifier
*
*  @return  BOS_STATUS_OK if the message queue was created successfully, or
*           an error code if the message queue could not be created.
**
** @ note   This message queue will use a sema4 as it's internal synchronization
**          primitive. This means that any task can call either bosMsgQSend
**          and bosMsgQReceive.
*/
BOS_STATUS bosMsgQCreate
(
   const char    *name,
   unsigned int   qSize,
   BOS_MSGQ      *qid
);

/***************************************************************************/
/**
*  Creates an "event-based" FIFO message queue of size (qSize).
*
*  @param   name        (in)  4-character long messasge queue name
*  @param   qSize       (in)  Number of messages in the queue
*  @parm    eventTaskId (in)  Id of task that will call bosMsgQReceive
*  @param   qid         (out) Message queue identifier
*
*  @return  BOS_STATUS_OK if the message queue was created successfully, or
*           an error code if the message queue could not be created.
**
** @ note   This message queue will use an event as it's internal synchronization
**          primitive. This means that the message queue can be added to an
**          BOS event-set object. Event-sets are container objects for BOS
**          events and other BOS primitives that use events as their
**          synchronization primitive. They allow tasks to wait for one or
**          all of a set of events to occur. The message queue's internal event
**          object can be retrieved by calling bosMsgQGetEvent(). However, only
**          the task specified by 'eventTaskId' may call and bosMsgQReceive().
**          Any task may call bosMsgQSend().
*/
BOS_STATUS bosMsgQCreateEventBased
(
   const char    *name,
   unsigned int   qSize,
   BOS_TASK_ID   *eventTaskId,
   BOS_MSGQ      *qid
);

/***************************************************************************/
/**
*  Destroys a message queue
*
*  @param   qid         (in)  Message queue identifier
*
*  @return  BOS_STATUS_OK if the message queue was successfully deleted,
*           or an error code if the deletion failed.
*/
BOS_STATUS bosMsgQDestroy( BOS_MSGQ *qid );

/***************************************************************************/
/**
*  Sends a message to a specified oridinary message queue.
*
*  @param   qid         (in)  Message queue identifier
*  @param   qMsg        (in)  Message buffer containing the message to send
*
*  @return  BOS_STATUS_OK if the message was sent successfully to the
*           queue, or an error code if the message was not sent.
*/
BOS_STATUS bosMsgQSend( BOS_MSGQ *qid, const BOS_MSGQ_MSG *qMsg);

/***************************************************************************/
/**
*  Receives a message from a specified oridinary message queue.  This
*  function will wait infinitely until a message is available in the queue
*
*  @param   qid         (in)  Message queue identifier
*  @param   qMsg        (out) Message buffer containing the retrieved message
*
*  @return  BOS_STATUS_OK if a message was retrieved successfully from the
*           queue, or an error code if no message was obtained.
*/
BOS_STATUS bosMsgQReceive
(
   BOS_MSGQ      *qid,
   BOS_MSGQ_MSG  *qMsg
);

/***************************************************************************/
/**
*  Receives a message from a specified oridinary message queue.  This
*  function will wait for @a timeoutMsec milliseconds for a message, before
*  returning with BOS_STATUS_TIMEOUT
*
*  @param   qid         (in)  Message queue identifier
*  @param   timeoutMsec (in)  Number of milliseconds to wait for a
*                             message to be posted at the queue.
*  @param   qMsg        (out) Message buffer containing the retrieved message
*
*  @return  BOS_STATUS_OK if a message was retrieved successfully from the
*           queue, or an error code if no message was obtained.
*/
BOS_STATUS bosMsgQTimedReceive
(
   BOS_MSGQ      *qid,
   BOS_TIME_MS    timeoutMsec,
   BOS_MSGQ_MSG  *qMsg
);

#define bosMsgQReceiveR( q, msg )   bosMsgQTimedReceiveR( q, BOS_WAIT_FOREVER, msg )


/***************************************************************************/
/**
*  Retrieves the internal event object used by the mesage Q as a signalling
*  primitive. This is typically used to add a message Q to an event-set object.
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

BOS_STATUS bosMsgQGetEvent( BOS_MSGQ *msgQ, BOS_EVENT **event );


/***************************************************************************/
/**
*  Displays statistics associated with the message queue.
*
*  @param   qid         (in)  Message queue identifier
*
*  @return  BOS_STATUS_OK.
*
*/

BOS_STATUS bosMsgQDisplayStats( BOS_MSGQ *qid );

/** @} */

#if defined( __cplusplus )
}
#endif

#endif /* BOSMSGQ_H */

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
*  Filename: bosMsgQLinuxUser.c
*
****************************************************************************
*  Description:
*
*
****************************************************************************/
/**
*
*  @file    bosMsgQLinuxUser.c
*
*  @brief   LinuxUser implementation of the BOS Message Queue
*
****************************************************************************/

/* ---- Include Files ---------------------------------------------------- */

#include <bosCfg.h>

#if BOS_CFG_MSGQ

#include <bosError.h>
#include <bosErrorLinuxUser.h>
#include <bosMsgQ.h>
#include <bosMsgQPrivate.h>
#include <str.h>
#include <bosLog.h>

#include <sys/msg.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <string.h>

#if BOS_CFG_TIME_FOR_TIMED_FUNCTION
#  if BOS_CFG_TIME
#     include <bosTime.h>
#  else
#     error "Using System time for Timed function requires Time module support (not present)"
#  endif
#endif

/* ---- Public Variables ------------------------------------------------- */
/* ---- Private Constants and Types -------------------------------------- */
#if 0

#define BOS_Q_WAIT_TIME    10000  /* in microseconds */

typedef enum
{
   BOS_TIMER_MESSAGE = 1,
   BOS_USER_MESSAGE,
} BOS_QMSG_TYPE;

typedef struct BOS_LINUXUSER_MSGBUF
{
   long           mtype;      /* type of received/sent message */
   BOS_MSGQ_MSG   mtext;   /* text of the message */
} BOS_LINUXUSER_MSGBUF;


/* ---- Private Variables ------------------------------------------------ */

static   BOS_BOOL gBosMsgQInitialized = BOS_FALSE;

/* ---- Private Function Prototypes -------------------------------------- */

static BOS_STATUS CheckMsgQStatusLinuxUser( unsigned long err, BOS_MSGQ_FUNC_ID funcId );

/* ---- Functions -------------------------------------------------------- */

/***************************************************************************/

BOS_STATUS bosMsgQInit( void )
{
   BOS_ASSERT( !gBosMsgQInitialized );

   bosMsgQInitFunctionNames();

   gBosMsgQInitialized = BOS_TRUE;

   return BOS_STATUS_OK;

} /* bosMsgQInit */

/***************************************************************************/

BOS_STATUS bosMsgQTerm( void )
{
   BOS_ASSERT( gBosMsgQInitialized );

   gBosMsgQInitialized = BOS_FALSE;

   return BOS_STATUS_OK;

} /* bosMsgQTerm */

/***************************************************************************/

BOS_STATUS bosMsgQCreate( const char *name, unsigned int qSize, BOS_MSGQ *qid )
{
   int q;
   BOS_STATUS status = BOS_STATUS_OK;

   BOS_ASSERT( qid != NULL );

   q = msgget(IPC_PRIVATE, 0);
   if(q < 0)
   {
      status = BOS_STATUS_ERR;
   }
   else
   {
      /* queue ID returned from msgget can be 0.
      Since we don't want to use 0 as a valid OS_QID queue id,
      we increment it before teturning to the caller. */
      BOS_LOG(("Created queue %s \n", name));
      *qid = q + 1;
   }

   if ( status != BOS_STATUS_OK )
   {
      return CheckMsgQStatusLinuxUser( errno, BOS_MSGQ_CREATE_FUNC_ID );
   }

   return( BOS_STATUS_OK );

} /* bosMsgQCreate */

/***************************************************************************/

BOS_STATUS bosMsgQDestroy( BOS_MSGQ *qid )
{
   int result;
   BOS_MSGQ queueId;

   BOS_ASSERT( qid != NULL );

   queueId = (*qid) - 1;

   result = msgctl( queueId, IPC_RMID, NULL );

   if ( result == 0 )
   {
      return( BOS_STATUS_OK );
   }
   else
   {
      return CheckMsgQStatusLinuxUser( BOS_STATUS_ERR, BOS_MSGQ_DESTROY_FUNC_ID );
   }


} /* bosMsgQDestroy */

/***************************************************************************/

BOS_STATUS bosMsgQSend( BOS_MSGQ *qid, const BOS_MSGQ_MSG *qMsg)
{
   long        result;
   BOS_MSGQ    queueId;
   BOS_LINUXUSER_MSGBUF    buf;

   BOS_ASSERT( qid  != NULL );
   BOS_ASSERT( qMsg != NULL );

   /* Decrement queue ID because it was incremented in bosMsgQCreate */
   queueId = (*qid) - 1;

   buf.mtype = BOS_USER_MESSAGE;
   memcpy( &buf.mtext, qMsg->buffer, sizeof(*qMsg) );

   result = msgsnd( queueId, &buf, sizeof(buf.mtext), IPC_NOWAIT);

   if(result != 0)
   {
      CheckMsgQStatusLinuxUser( errno, BOS_MSGQ_SEND_FUNC_ID );
   }

   return( BOS_STATUS_OK);

} /* bosMsgQSend */

/***************************************************************************/

BOS_STATUS bosMsgQReceive( BOS_MSGQ *qid, BOS_MSGQ_MSG *qMsg)
{
   BOS_STATUS status;

   status = bosMsgQTimedReceive( qid, 0, qMsg );

   return( status );

} /* bosMsgQReceive */

/***************************************************************************/

BOS_STATUS bosMsgQTimedReceive( BOS_MSGQ *qid, BOS_TIME_MS timeoutMsec,
                                 BOS_MSGQ_MSG *qMsg)
{
   int         bytesRecvd;
   int         msgflag;
   BOS_STATUS  status;
   BOS_MSGQ    queueId;
   BOS_LINUXUSER_MSGBUF buf;

   BOS_ASSERT( qid  != NULL );
   BOS_ASSERT( qMsg != NULL );

   msgflag = IPC_NOWAIT;
   status = BOS_STATUS_OK;

   /* Decrement queue ID because it was incremented in bcmOsQueueCreate */
   queueId = (*qid) - 1;

   if ( timeoutMsec == BOS_WAIT_FOREVER )
   {
      msgflag = 0;
   }

   if( timeoutMsec > 0 )
   {
#if BOS_CFG_TIME_FOR_TIMED_FUNCTION   
      BOS_TIME_MS start;
      BOS_TIME_MS now;
      BOS_TIME_MS delta = 0;
      bosTimeGetMs(&start);
      bytesRecvd = msgrcv( queueId, &buf, sizeof(buf.mtext), 0, IPC_NOWAIT );
      while (( mutexStatus != 0 ) && ( timeoutMsec > delta )
      {
         usleep( BOS_Q_WAIT_TIME );            /* BOS_Q_WAIT_TIME is in microseconds */
         bytesRecvd = msgrcv( queueId, &buf, sizeof(buf.mtext), 0, IPC_NOWAIT );
         bosTimeGetMs(&now);
         bosTimeCalcDeltaMs(&start, &now, &delta);
      } (timeoutMsec > delta);
#else
      long wait_msec = (long)( timeoutMsec );
      bytesRecvd = msgrcv( queueId, &buf, sizeof(buf.mtext), 0, IPC_NOWAIT );

      while ( bytesRecvd <= 0 && wait_msec > 0 )
      {
         usleep( BOS_Q_WAIT_TIME );
         wait_msec -= ( BOS_Q_WAIT_TIME/1000 );
         bytesRecvd = msgrcv( queueId, &buf, sizeof(buf.mtext), 0, IPC_NOWAIT );
      }
#endif      
      if ( bytesRecvd <= 0 )
      {
         if (errno == ENOMSG)
         {
            status = BOS_STATUS_TIMEOUT;
         }
         else
         {
            status = BOS_STATUS_ERR;
         }
      }
   }
   else
   {
      bytesRecvd = msgrcv( queueId, &buf, sizeof(buf.mtext), 0, msgflag);
   }

   if( status == BOS_STATUS_OK )
   {
      if( bytesRecvd == sizeof(buf.mtext) )
      {
         /* message received -- copy it to user */
         memcpy( qMsg->buffer, &buf.mtext, sizeof(buf.mtext) );
      }
      else
      {
         status = BOS_STATUS_ERR;
      }
   }
   else
   {
      return CheckMsgQStatusLinuxUser( errno, BOS_MSGQ_TIMED_RECV_FUNC_ID );

   }

   return ( status );

} /* bosMsgQTimedReceive */

/**
 * @addtogroup bosMsgQInternal
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

static BOS_STATUS CheckMsgQStatusLinuxUser( unsigned long err, BOS_MSGQ_FUNC_ID funcId )
{
   return bosErrorCheckStatusLinuxUser( err,
                                   BOS_MAKE_ERROR_SOURCE( BOS_MSGQ_MODULE, funcId ));

} /* CheckMsgQStatusLinuxUser */

/** @} */
#endif
#endif   /* BOS_CFG_MSGQ */


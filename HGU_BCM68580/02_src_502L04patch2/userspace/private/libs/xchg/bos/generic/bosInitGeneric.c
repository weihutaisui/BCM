/****************************************************************************
*
* <:copyright-BRCM:2009:proprietary:standard
* 
*    Copyright (c) 2009 Broadcom 
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
*  @file    bosInit.c
*
*  @brief   Generic implementation of the BOS Init Module
*
****************************************************************************/

/* ---- Include Files ---------------------------------------------------- */

#include <bosCfg.h>

#if BOS_CFG_INIT

#include <bosInit.h>

#if BOS_CFG_FILE
#  include <bosFile.h>
#endif

#include <bosError.h>
#include <bosLog.h>

#if BOS_CFG_SLEEP
#  include <bosSleep.h>
#endif
#if BOS_CFG_MUTEX
#  include <bosMutex.h>
#endif
#if BOS_CFG_SEM
#  include <bosSem.h>
#endif
#if BOS_CFG_EVENT
#  include <bosEvent.h>
#endif
#if BOS_CFG_TASK
#  include <bosTask.h>
#endif
#if BOS_CFG_SOCKET
#  include <bosSocket.h>
#endif
#if BOS_CFG_MSGQ
#  include <bosMsgQ.h>
#endif
#if BOS_CFG_TIME
#  include <bosTime.h>
#endif
#if BOS_CFG_CRITSECT
#  include <bosCritSect.h>
#endif
#if BOS_CFG_UTIL
#  include <bosUtil.h>
#endif
#if BOS_CFG_TIMER
#  include <bosTimer.h>
#endif
#if BOS_CFG_IP_ADDRESS
#  include <bosIpAddr.h>
#endif

/* ---- Public Variables ------------------------------------------------- */
/* ---- Private Constants and Types -------------------------------------- */
/* ---- Private Variables ------------------------------------------------ */

static   BOS_BOOL gBosInitialized = BOS_FALSE;

/* ---- Private Function Prototypes -------------------------------------- */

#define  RETURN_IF_NOT_OK(x)  \
   if (( status = (x)) != BOS_STATUS_OK ) \
   { \
      bosLogErr("%s: %s failed", __FUNCTION__, #x); \
      return status; \
   }

/* ---- Functions -------------------------------------------------------- */

/***************************************************************************/

BOS_STATUS bosInit( void )
{
   BOS_STATUS  status;

   bosLogInfo("BOS: Enter bosInit");

   BOS_ASSERT( !gBosInitialized );

   RETURN_IF_NOT_OK( bosErrorInit() ); /* MUST be first */

#if BOS_CFG_SLEEP
   RETURN_IF_NOT_OK( bosSleepInit() );
#endif
#if BOS_CFG_SEM
   RETURN_IF_NOT_OK( bosSemInit() );
#endif
#if BOS_CFG_MUTEX
   RETURN_IF_NOT_OK( bosMutexInit() );
#endif
#if BOS_CFG_EVENT
   RETURN_IF_NOT_OK( bosEventInit() );
#endif
#if BOS_CFG_TASK
   RETURN_IF_NOT_OK( bosTaskInit() );
#endif
#if BOS_CFG_SOCKET
   RETURN_IF_NOT_OK( bosSocketInit() );
#endif
#if BOS_CFG_MSGQ
   RETURN_IF_NOT_OK( bosMsgQInit() );
#endif
#if BOS_CFG_TIME
   RETURN_IF_NOT_OK( bosTimeInit() );
#endif

#if BOS_CFG_CRITSECT
   RETURN_IF_NOT_OK( bosCritSectInit() );
#endif
#if BOS_CFG_UTIL
   RETURN_IF_NOT_OK( bosUtilInit() );
#endif
#if BOS_CFG_TIMER
   RETURN_IF_NOT_OK( bosTimerInit() );
#endif

#if BOS_CFG_LOG
   RETURN_IF_NOT_OK( bosLogInit() );
#endif

#if BOS_CFG_IP_ADDRESS
   RETURN_IF_NOT_OK( bosIpAddrInit() );
#endif

#if BOS_CFG_FILE
   RETURN_IF_NOT_OK( bosFileInit() );
#endif

   RETURN_IF_NOT_OK( bosAppInit() );


   gBosInitialized = BOS_TRUE;

   bosLogInfo("BOS: Exit bosInit");
   return BOS_STATUS_OK;

} /* bosInit */

/***************************************************************************/

BOS_STATUS bosTerm( void )
{
   BOS_STATUS  status;

   BOS_ASSERT( gBosInitialized );

#if BOS_CFG_LOG
   RETURN_IF_NOT_OK( bosLogTerm() );
#endif

#if BOS_CFG_TIMER
   RETURN_IF_NOT_OK( bosTimerTerm() );
#endif
#if BOS_CFG_UTIL
   RETURN_IF_NOT_OK( bosUtilTerm() );
#endif
#if BOS_CFG_CRITSECT
   RETURN_IF_NOT_OK( bosCritSectTerm() );
#endif
#if BOS_CFG_TIME
   RETURN_IF_NOT_OK( bosTimeTerm() );
#endif
#if BOS_CFG_MSGQ
   RETURN_IF_NOT_OK( bosMsgQTerm() );
#endif
#if BOS_CFG_SOCKET
   RETURN_IF_NOT_OK( bosSocketTerm() );
#endif
#if BOS_CFG_TASK
   RETURN_IF_NOT_OK( bosTaskTerm() );
#endif
#if BOS_CFG_EVENT
   RETURN_IF_NOT_OK( bosEventTerm() );
#endif
#if BOS_CFG_MUTEX
   RETURN_IF_NOT_OK( bosMutexTerm() );
#endif
#if BOS_CFG_SEM
   RETURN_IF_NOT_OK( bosSemTerm() );
#endif
#if BOS_CFG_SLEEP
   RETURN_IF_NOT_OK( bosSleepTerm() );
#endif
#if BOS_CFG_IP_ADDRESS
   RETURN_IF_NOT_OK( bosIpAddrTerm() );
#endif
#if BOS_CFG_FILE
   RETURN_IF_NOT_OK( bosFileTerm() );
#endif

   RETURN_IF_NOT_OK( bosErrorTerm() ); /* MUST be last */

   gBosInitialized = BOS_FALSE;

   return BOS_STATUS_OK;

} /* bosTerm */

/***************************************************************************/

BOS_BOOL bosIsInit( void )
{
   return gBosInitialized;

} /* bosIsInit */

#endif   /* BOS_CFG_INIT */


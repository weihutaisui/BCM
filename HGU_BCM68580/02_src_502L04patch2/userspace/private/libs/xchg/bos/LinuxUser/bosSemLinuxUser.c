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
*  Filename: bosSemLinuxUser.c
*
****************************************************************************
*  Description:
*
*
****************************************************************************/
/**
*
*  @file    bosSemLinuxUser.c
*
*  @brief   LinuxUser implementation of the BOS Semaphore Module
*
****************************************************************************/

/* ---- Include Files ---------------------------------------------------- */

#include <bosCfg.h>

#if BOS_CFG_SEM

#include <bosError.h>
#include <bosErrorLinuxUser.h>
#include <bosSem.h>
#include <bosSemPrivate.h>
#include <unistd.h>

#if BOS_CFG_TIME_FOR_TIMED_FUNCTION
#  if BOS_CFG_TIME
#     include <bosTime.h>
#  else
#     error "Using System time for Timed function requires Time module support (not present)"
#  endif
#endif


/* ---- Public Variables ------------------------------------------------- */
/* ---- Private Constants and Types -------------------------------------- */

#define SEM_WAIT_TIME 10000  /* in microseconds */

/* ---- Private Variables ------------------------------------------------ */

static   BOS_BOOL gBosSemInitialized = BOS_FALSE;

/* ---- Private Function Prototypes -------------------------------------- */

static BOS_STATUS CheckSemStatusLinuxUser( int err, BOS_SEM_FUNC_ID funcId );

/* ---- Functions -------------------------------------------------------- */

/***************************************************************************/

BOS_STATUS bosSemInit( void )
{
   BOS_ASSERT( !gBosSemInitialized );

   bosSemInitFunctionNames();

   gBosSemInitialized = BOS_TRUE;

   return BOS_STATUS_OK;

} /* bosSemInit */

/***************************************************************************/

BOS_STATUS bosSemTerm( void )
{
   BOS_ASSERT( gBosSemInitialized );

   gBosSemInitialized = BOS_FALSE;

   return BOS_STATUS_OK;

} /* bosSemTerm */

/***************************************************************************/

BOS_STATUS bosSemCreate
(
   const char *name,
   int         initCount,
   int         maxCount,
   BOS_SEM    *sem
)
{
    BOS_STATUS status;
   (void)name;
   (void)maxCount;

   if ( sem_init(sem, 0, initCount) != 0 )
   {
      status = BOS_STATUS_ERR;
   }
   else
   {
      status = BOS_STATUS_OK;
   }

   if ( status != BOS_STATUS_OK )
   {
      return CheckSemStatusLinuxUser( errno, BOS_SEM_CREATE_FUNC_ID );
   }

   return( BOS_STATUS_OK );

} /* bosSemCreate */

/***************************************************************************/

BOS_STATUS bosSemDestroy( BOS_SEM *sem )
{
   BOS_ASSERT( gBosSemInitialized );
   BOS_ASSERT( sem != NULL );

   if ( sem_destroy(sem) != 0 )
   {
      return CheckSemStatusLinuxUser( errno, BOS_SEM_DESTROY_FUNC_ID );
   }

   return( BOS_STATUS_OK );

} /* bosSemDestroy */

/***************************************************************************/

BOS_STATUS bosSemTake( BOS_SEM *sem )
{
   BOS_ASSERT( gBosSemInitialized );
   BOS_ASSERT( sem != NULL );

   if ( sem_wait(sem) != 0 )
   {
      return CheckSemStatusLinuxUser( errno, BOS_SEM_TAKE_FUNC_ID );
   }

   return ( BOS_STATUS_OK );

} /* bosSemTake */

/***************************************************************************/

BOS_STATUS bosSemTimedTake( BOS_SEM *sem, BOS_TIME_MS timeoutMsec )
{
   int   semStatus;
   int   status = BOS_STATUS_OK;

   BOS_ASSERT( gBosSemInitialized );
   BOS_ASSERT( sem != NULL );

   if ( timeoutMsec == BOS_WAIT_FOREVER )
   {
      if ( sem_wait(sem) != 0 )
      {
         status = BOS_STATUS_ERR;
      }
   }
   else
   {
#if BOS_CFG_TIME_FOR_TIMED_FUNCTION   
      BOS_TIME_MS start;
      BOS_TIME_MS now;
      BOS_TIME_MS delta = 0;
      bosTimeGetMs(&start);
      semStatus = sem_trywait( sem );
      while (( semStatus != 0 ) && ( timeoutMsec > delta ))
      {
         usleep( SEM_WAIT_TIME );            /* SEM_WAIT_TIME is in microseconds */
         semStatus = sem_trywait( sem );
         bosTimeGetMs(&now);
         bosTimeCalcDeltaMs(&start, &now, &delta);
      };
#else
      long wait_msec = (long)( timeoutMsec );
      semStatus = sem_trywait(sem);

      while ( ( semStatus != 0 ) && ( wait_msec > 0 ) )
      {
         usleep( SEM_WAIT_TIME );            /* SEM_WAIT_TIME is in microseconds */
         wait_msec -= (SEM_WAIT_TIME/1000);
         semStatus  = sem_trywait(sem);
      }
#endif
      if ( semStatus != 0 )
      {
         status = BOS_STATUS_TIMEOUT;
      }
   }

   /* Don't print out error code through CheckSemStatusLinuxUser
   ** when a timeout occurrs. Just return error code. */
   if ( (status != BOS_STATUS_OK) && (status != BOS_STATUS_TIMEOUT) )
   {
      return CheckSemStatusLinuxUser( errno, BOS_SEM_TIMED_TAKE_FUNC_ID );
   }
   else
   {
      return( status );
   }

} /* bosSemTimedTake */

/***************************************************************************/

BOS_STATUS bosSemGive( BOS_SEM *sem )
{
   if ( sem_post(sem) != 0 )
   {
      return CheckSemStatusLinuxUser( errno, BOS_SEM_GIVE_FUNC_ID );
   }

   return BOS_STATUS_OK;

} /* bosSemGive */

/**
 * @addtogroup bosSemInternal
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

static BOS_STATUS CheckSemStatusLinuxUser( int err, BOS_SEM_FUNC_ID funcId )
{
   return bosErrorCheckStatusLinuxUser( err,
                                      BOS_MAKE_ERROR_SOURCE( BOS_SEM_MODULE, funcId ));

} /* CheckSemStatusLinuxUser */

/** @} */

#endif   /* BOS_CFG_SEM */


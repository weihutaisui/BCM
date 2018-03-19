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
*  Filename: bosMutexLinuxUser.c
*
****************************************************************************
*  Description:
*
*
****************************************************************************/
/**
*
*  @file    bosMutexLinuxUser.c
*
*  @brief   LinuxUser implementation of the BOS Mutex Module
*
****************************************************************************/

/* ---- Include Files ---------------------------------------------------- */

#include <bosCfg.h>
#include <unistd.h>

#if BOS_CFG_MUTEX
#include <pthread.h>
#include <bosError.h>
#include <bosErrorLinuxUser.h>
#include <bosMutex.h>
#include <bosMutexPrivate.h>


#if BOS_CFG_TIME_FOR_TIMED_FUNCTION
#  if BOS_CFG_TIME
#     include <bosTime.h>
#  else
#     error "Using System time for Timed function requires Time module support (not present)"
#  endif
#endif


/* ---- Public Variables ------------------------------------------------- */
/* ---- Private Constants and Types -------------------------------------- */

#define MUTEX_WAIT_TIME 10000  /* in microseconds */

/* ---- Private Variables ------------------------------------------------ */

static   BOS_BOOL gBosMutexInitialized = BOS_FALSE;

/* ---- Private Function Prototypes -------------------------------------- */

static BOS_STATUS CheckMutexStatusLinuxUser( int err, BOS_MUTEX_FUNC_ID funcId );

/* ---- Functions -------------------------------------------------------- */

/***************************************************************************/

BOS_STATUS bosMutexInit( void )
{
   BOS_ASSERT( !gBosMutexInitialized );

   bosMutexInitFunctionNames();

   gBosMutexInitialized = BOS_TRUE;

   return BOS_STATUS_OK;

} /* bosMutexInit */

/***************************************************************************/

BOS_STATUS bosMutexTerm( void )
{
   BOS_ASSERT( gBosMutexInitialized );

   gBosMutexInitialized = BOS_FALSE;

   return BOS_STATUS_OK;

} /* bosMutexTerm */

/***************************************************************************/

BOS_STATUS bosMutexCreate( const char *name, BOS_MUTEX *mutex )
{
   pthread_mutexattr_t attrib;

   BOS_ASSERT( gBosMutexInitialized );

   *mutex = malloc( sizeof(pthread_mutex_t) );
   pthread_mutexattr_init(&attrib);
   pthread_mutexattr_settype(&attrib, PTHREAD_MUTEX_RECURSIVE_NP);
   pthread_mutex_init( *mutex, &attrib );

   return ( BOS_STATUS_OK );

} /* bosMutexCreate */

/***************************************************************************/

BOS_STATUS bosMutexDestroy( BOS_MUTEX *mutex )
{
   BOS_ASSERT( gBosMutexInitialized );
   BOS_ASSERT( mutex != NULL );
   BOS_ASSERT( *mutex != NULL );

   if ( pthread_mutex_destroy( *mutex ) != 0 )
   {
      return CheckMutexStatusLinuxUser( errno, BOS_MUTEX_DESTROY_FUNC_ID );
   }

   free( *mutex );

   return BOS_STATUS_OK;

} /* bosMutexDestroy */

/***************************************************************************/

BOS_STATUS bosMutexAcquire( BOS_MUTEX *mutex )
{
   BOS_ASSERT( gBosMutexInitialized );
   BOS_ASSERT( mutex != NULL );
   BOS_ASSERT( *mutex != NULL );

   if ( pthread_mutex_lock( *mutex ) != 0 )
   {
      return CheckMutexStatusLinuxUser( errno, BOS_MUTEX_ACQUIRE_FUNC_ID );
   }

   return BOS_STATUS_OK;

} /* bosMutexAcquire */

/***************************************************************************/

BOS_STATUS bosMutexTimedAcquire( BOS_MUTEX *mutex, BOS_TIME_MS timeoutMsec )
{
   int   mutexStatus;
   int   status = BOS_STATUS_OK;

   BOS_ASSERT( gBosMutexInitialized );
   BOS_ASSERT( mutex != NULL );
   BOS_ASSERT( *mutex != NULL );

   if ( timeoutMsec == BOS_WAIT_FOREVER )
   {
      if ( pthread_mutex_lock( *mutex ) != 0 )
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
      mutexStatus = pthread_mutex_trylock( *mutex );
      while (( mutexStatus != 0 ) && ( timeoutMsec > delta ))
      {
         usleep( MUTEX_WAIT_TIME );            /* MUTEX_WAIT_TIME is in microseconds */
         mutexStatus = pthread_mutex_trylock( *mutex );
         bosTimeGetMs(&now);
         bosTimeCalcDeltaMs(&start, &now, &delta);
      };
#else
      long wait_msec = (long)( timeoutMsec );

      mutexStatus = pthread_mutex_trylock( *mutex );
      while ( ( mutexStatus != 0 ) && ( wait_msec > 0 ) )
      {
         usleep( MUTEX_WAIT_TIME );            /* MUTEX_WAIT_TIME is in microseconds */
         wait_msec -= ( MUTEX_WAIT_TIME/1000 );
         mutexStatus = pthread_mutex_trylock( *mutex );
      }
#endif
      if ( mutexStatus != 0 )
      {
         status = BOS_STATUS_TIMEOUT;
      }
   }

   /* Don't print out error code through CheckMutexStatusLinuxUser
   ** when a timeout occurrs. Just return error code. */
   if ( (status != BOS_STATUS_OK) && (status != BOS_STATUS_TIMEOUT) )
   {
      return CheckMutexStatusLinuxUser( errno, BOS_MUTEX_TIMED_ACQUIRE_FUNC_ID );
   }
   else
   {
      return( status );
   }

} /* bosMutexTimedAcquire */

/***************************************************************************/

BOS_STATUS bosMutexRelease( BOS_MUTEX *mutex )
{
   BOS_ASSERT( gBosMutexInitialized );
   BOS_ASSERT( mutex != NULL );
   BOS_ASSERT( *mutex != NULL );

   if ( pthread_mutex_unlock( *mutex ) != 0 )
   {
      return CheckMutexStatusLinuxUser( errno, BOS_MUTEX_RELEASE_FUNC_ID );
   }

   return BOS_STATUS_OK;

} /* bosMutexRelease */


/**
 * @addtogroup bosMutexInternal
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

static BOS_STATUS CheckMutexStatusLinuxUser( int err, BOS_MUTEX_FUNC_ID funcId )
{
   return bosErrorCheckStatusLinuxUser( err,
                                      BOS_MAKE_ERROR_SOURCE( BOS_MUTEX_MODULE, funcId ));

} /* CheckMutexStatusLinuxUser */

/** @} */

#endif   /* BOS_CFG_MUTEX */


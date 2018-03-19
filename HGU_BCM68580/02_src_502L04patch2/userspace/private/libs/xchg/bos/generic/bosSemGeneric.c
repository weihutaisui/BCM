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
*  Filename: bosSemGeneric.c
*
****************************************************************************
*  Description:
*
*
****************************************************************************/
/**
*
*  @file    bosSemGeneric.c
*
*  @brief   Contains OS independant definitions for the BOS Sem module.
*
****************************************************************************/

/* ---- Include Files ---------------------------------------------------- */

#include <bosCfg.h>

#if BOS_CFG_SEM

#include <bosError.h>
#include <bosSem.h>
#include <bosSemPrivate.h>

#if BOS_CFG_RESET_SUPPORT
#include <bosTask.h>
#endif

#if BOS_CFG_TIME_FOR_TIMED_FUNCTION
#  if BOS_CFG_TIME
#     include <bosTime.h>
#  else
#     error "Using System time for Timed function requires Time module support (not present)"
#  endif
#endif

/**
 * @addtogroup bosSem
 * @{
 */

/* ---- Public Variables ------------------------------------------------- */

#if BOS_CFG_ERROR_FUNCTION_STRINGS

const char *gBosSemFunctionName[ BOS_SEM_NUM_FUNC_ID ];

#endif   /* BOS_CFG_ERROR_FUNCTION_STRINGS */

/* ---- Private Constants and Types -------------------------------------- */
/* ---- Private Variables ------------------------------------------------ */
/* ---- Private Function Prototypes -------------------------------------- */
/* ---- Functions -------------------------------------------------------- */

#if BOS_CFG_ERROR_FUNCTION_STRINGS

/***************************************************************************/
/**
*  Initializes the contents of @a gBosSemFunctionName, which is used to
*  translate function IDs into character strings.
*/

void bosSemInitFunctionNames( void )
{
   gBosSemFunctionName[ BOS_SEM_INIT_FUNC_ID ]        = "bosSemInit";
   gBosSemFunctionName[ BOS_SEM_TERM_FUNC_ID ]        = "bosSemTerm";
   gBosSemFunctionName[ BOS_SEM_CREATE_FUNC_ID ]      = "bosSemCreate";
   gBosSemFunctionName[ BOS_SEM_DESTROY_FUNC_ID ]     = "bosSemDestroy";
   gBosSemFunctionName[ BOS_SEM_GIVE_FUNC_ID ]        = "bosSemGive";
   gBosSemFunctionName[ BOS_SEM_TAKE_FUNC_ID ]        = "bosSemTake";
   gBosSemFunctionName[ BOS_SEM_TIMED_TAKE_FUNC_ID ]  = "bosSemTimedTake";

   bosErrorAddFunctionStrings( BOS_SEM_MODULE, gBosSemFunctionName, BOS_SEM_NUM_FUNC_ID );

} /* bosSemInitFunctioNames */

#endif   /* BOS_CFG_ERROR_FUNCTION_STRINGS */

/***************************************************************************/

#if BOS_CFG_RESET_SUPPORT

BOS_STATUS bosSemTimedTakeR( BOS_SEM *sem, BOS_TIME_MS timeout )
{
   BOS_STATUS retval;
#if BOS_CFG_TIME_FOR_TIMED_FUNCTION 
   BOS_TIME_MS start;
   BOS_TIME_MS now;
   BOS_TIME_MS delta = 0;
   BOS_TIME_MS localTimeout;
   BOS_TIME_MS timeRemaining = timeout;
   bosTimeGetMs(&start);
#endif

   while ( !bosResourceIsResetPending() )
   {
#if BOS_CFG_TIME_FOR_TIMED_FUNCTION 
      timeRemaining = timeout - delta;
      if (timeRemaining > BOS_CFG_RESET_GRANULARITY )
      {
         localTimeout = BOS_CFG_RESET_GRANULARITY;
      }
      else
      {
         localTimeout = timeRemaining;
      }
      retval =  bosSemTimedTake( sem, localTimeout );
      if (retval == BOS_STATUS_TIMEOUT)
      {
         bosTimeGetMs(&now);
         bosTimeCalcDeltaMs(&start, &now, &delta);
         if (timeout > delta)
         {
            continue;            
         }
         else
         {
            return BOS_STATUS_TIMEOUT;   
         }
      }
      else
      {
         return retval;  /* BOS_STATUS_OK or BOS_STATUS_ERR */
      }
#else
      if (( timeout != BOS_WAIT_FOREVER ) && ( timeout <= BOS_CFG_RESET_GRANULARITY ))
      {
         /* Timeout is less than the reset granularity, semTake normally */
         return ( bosSemTimedTake( sem, timeout ) );
      }
      else
      {
         /* Timeout is more than reset granularity, decrement timeout remaining
         ** and timeout for the maximum allowed before checking for a reset */
         if ( timeout != BOS_WAIT_FOREVER )
         {
            timeout -= BOS_CFG_RESET_GRANULARITY;
         }
         retval = bosSemTimedTake( sem, BOS_CFG_RESET_GRANULARITY );
         if ( retval != BOS_STATUS_TIMEOUT )
         {
            /* Anything but the early timeout is returned to the caller */
            return (retval);
         }
      }
#endif
   }

   return ( BOS_STATUS_RESET );

} /* bosSemTimedTakeR */

#endif   /* BOS_CFG_RESET_SUPPORT */

/** @} */

#endif   /* BOS_CFG_SEM */


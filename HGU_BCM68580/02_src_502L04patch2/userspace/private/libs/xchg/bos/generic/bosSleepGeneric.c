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
*  @file    bosSleepGeneric.c
*
*  @brief   Contains OS independant definitions for the BOS Sleep module.
*
****************************************************************************/

/* ---- Include Files ---------------------------------------------------- */

#include <bosCfg.h>

#if BOS_CFG_SLEEP

#include <bosError.h>
#include <bosSleep.h>
#include <bosSleepPrivate.h>

#if BOS_CFG_RESET_SUPPORT
#include <bosTask.h>
#endif

/**
 * @addtogroup bosSleep
 * @{
 */

/* ---- Public Variables ------------------------------------------------- */

#if BOS_CFG_ERROR_FUNCTION_STRINGS

const char *gBosSleepFunctionName[ BOS_SLEEP_NUM_FUNC_ID ];

#endif   /* BOS_CFG_ERROR_FUNCTION_STRINGS */

/* ---- Private Constants and Types -------------------------------------- */
/* ---- Private Variables ------------------------------------------------ */
/* ---- Private Function Prototypes -------------------------------------- */
/* ---- Functions -------------------------------------------------------- */

#if BOS_CFG_ERROR_FUNCTION_STRINGS

/***************************************************************************/
/**
*  Initializes the contents of @a gBosSleepFunctionName, which is used to
*  translate function IDs into character strings.
*/

void bosSleepInitFunctionNames( void )
{
   gBosSleepFunctionName[ BOS_SLEEP_INIT_FUNC_ID ]    = "bosSleepInit";
   gBosSleepFunctionName[ BOS_SLEEP_TERM_FUNC_ID ]    = "bosSleepTerm";
   gBosSleepFunctionName[ BOS_SLEEP_SLEEP_FUNC_ID ]   = "bosSleep";

   bosErrorAddFunctionStrings( BOS_SLEEP_MODULE, gBosSleepFunctionName, BOS_SLEEP_NUM_FUNC_ID );

} /* bosSleepInitFunctioNames */

#endif   /* BOS_CFG_ERROR_FUNCTION_STRINGS */

/***************************************************************************/

#if BOS_CFG_RESET_SUPPORT

BOS_STATUS bosSleepR( BOS_TIME_MS sleeptime )
{
   BOS_STATUS retval;

   while ( !bosTaskIsResetPending() )
   {
      if ( sleeptime <= BOS_CFG_RESET_GRANULARITY )
      {
         /* Sleeptime is less than the reset granularity, simply sleep normally */
         return ( bosSleep( sleeptime ) );
      }
      else
      {
         /* Sleeptime is more than reset granularity, decrement sleeptime remaining
         ** and sleep for the maximum allowed before checking for a reset */
         if ( sleeptime != BOS_WAIT_FOREVER )
         {
            sleeptime -= BOS_CFG_RESET_GRANULARITY;
         }
         retval = bosSleep( BOS_CFG_RESET_GRANULARITY );
         if ( retval != BOS_STATUS_OK )
         {
            /* Anything but the OK to this chunk of the sleep is returned to the caller */
            return (retval);
         }
      }
   }

   return ( BOS_STATUS_RESET );
}

#endif   /* BOS_CFG_RESET_SUPPORT */

/** @} */

#endif   /* BOS_CFG_SLEEP */


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
*  @file    bosTimerGeneric.c
*
*  @brief   Contains OS independant definitions for the BOS Timer module.
*
****************************************************************************/

/* ---- Include Files ---------------------------------------------------- */

#include <bosCfg.h>

#if BOS_CFG_TIMER

#include <bosError.h>
#include <bosTimer.h>
#include <bosTimerPrivate.h>

#if BOS_CFG_RESET_SUPPORT
#include <bosTask.h>
#endif

/**
 * @addtogroup bosTimer
 * @{
 */

/* ---- Public Variables ------------------------------------------------- */

#if BOS_CFG_ERROR_FUNCTION_STRINGS

const char *gBosTimerFunctionName[ BOS_TIMER_NUM_FUNC_ID ];

#endif   /* BOS_CFG_ERROR_FUNCTION_STRINGS */

/* ---- Private Constants and Types -------------------------------------- */
/* ---- Private Variables ------------------------------------------------ */
/* ---- Private Function Prototypes -------------------------------------- */
/* ---- Functions -------------------------------------------------------- */

#if BOS_CFG_ERROR_FUNCTION_STRINGS

/***************************************************************************/
/**
*  Initializes the contents of @a gBosTimerFunctionName, which is used to
*  translate function IDs into character strings.
*/

void bosTimerInitFunctionNames( void )
{
   gBosTimerFunctionName[ BOS_TIMER_INIT_FUNC_ID ]          = "bosTimerInit";
   gBosTimerFunctionName[ BOS_TIMER_TERM_FUNC_ID ]          = "bosTimerTerm";
   gBosTimerFunctionName[ BOS_TIMER_CREATE_FUNC_ID ]        = "bosTimerCreate";
   gBosTimerFunctionName[ BOS_TIMER_DESTROY_FUNC_ID ]       = "bosTimerDestroy";
   gBosTimerFunctionName[ BOS_TIMER_START_FUNC_ID ]         = "bosTimerStart";
   gBosTimerFunctionName[ BOS_TIMER_RESTART_FUNC_ID ]       = "bosTimerRestart";
   gBosTimerFunctionName[ BOS_TIMER_STOP_FUNC_ID ]          = "bosTimerStop";
   gBosTimerFunctionName[ BOS_TIMER_WAIT_FUNC_ID ]          = "bosTimerWait";

   bosErrorAddFunctionStrings( BOS_TIMER_MODULE, gBosTimerFunctionName, BOS_TIMER_NUM_FUNC_ID );

} /* bosTimerInitFunctioNames */

#endif   /* BOS_CFG_ERROR_FUNCTION_STRINGS */

/***************************************************************************/


/** @} */

#endif   /* BOS_CFG_TIMER */


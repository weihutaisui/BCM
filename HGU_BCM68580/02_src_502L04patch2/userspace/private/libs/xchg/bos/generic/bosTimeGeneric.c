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
*  Filename: bosTimeGeneric.c
*
****************************************************************************
*  Description:
*
*
****************************************************************************/
/**
*
*  @file    bosTimeGeneric.c 
*
*  @brief   Contains OS independant definitions for the BOS Time module.
*
****************************************************************************/

/* ---- Include Files ---------------------------------------------------- */

#include <bosCfg.h>

#if BOS_CFG_TIME

#include <bosError.h>
#include <bosTime.h>
#include <bosTimePrivate.h>

/**
 * @addtogroup bosTime
 * @{
 */

/* ---- Public Variables ------------------------------------------------- */

#if BOS_CFG_ERROR_FUNCTION_STRINGS

const char *gBosTimeFunctionName[ BOS_TIME_NUM_FUNC_ID ];

#endif   /* BOS_CFG_ERROR_FUNCTION_STRINGS */

/* ---- Private Constants and Types -------------------------------------- */
/* ---- Private Variables ------------------------------------------------ */
/* ---- Private Function Prototypes -------------------------------------- */
/* ---- Functions -------------------------------------------------------- */

#if BOS_CFG_ERROR_FUNCTION_STRINGS

/***************************************************************************/
/**
*  Initializes the contents of @a gBosTimeFunctionName, which is used to
*  translate function IDs into character strings.
*/

void bosTimeInitFunctionNames( void )
{
   gBosTimeFunctionName[ BOS_TIME_INIT_FUNC_ID ]            = "bosTimeInit";
   gBosTimeFunctionName[ BOS_TIME_TERM_FUNC_ID ]            = "bosTimeTerm";
   gBosTimeFunctionName[ BOS_TIME_GET_TIME_MS_FUNC_ID ]     = "bosTimeGetMs";
   gBosTimeFunctionName[ BOS_TIME_DELTA_TIME_MS_FUNC_ID ]   = "bosTimeCalcDeltaMs";
   gBosTimeFunctionName[ BOS_TIME_SET_WALL_TIME_ID ]        = "bosTimeSetWallTime";
   gBosTimeFunctionName[ BOS_TIME_GET_GMTIME ]              = "bosTimeGetGmTime";
   gBosTimeFunctionName[ BOS_TIME_GET_LOCALTIME ]           = "bosTimeGetLocalTime";
   gBosTimeFunctionName[ BOS_TIME_MKTIME ]                  = "bosTimeMkTime";
   gBosTimeFunctionName[ BOS_TIME_GET_TIME_OF_DAY_ID ]      = "bosTimeGetTimeOfDay";

   bosErrorAddFunctionStrings( BOS_TIME_MODULE, gBosTimeFunctionName, BOS_TIME_NUM_FUNC_ID );

} /* bosTimeInitFunctioNames */

#endif   /* BOS_CFG_ERROR_FUNCTION_STRINGS */

/** @} */

#endif   /* BOS_CFG_TIME */


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
*  @file    boxErrorGeneric.c 
*
*  @brief   Contains OS independant error related functions.
*
****************************************************************************/

/* ---- Include Files ---------------------------------------------------- */

#include <bosError.h>
#include <bosCfg.h>
#include <str.h>
#include <string.h>

#if BOS_CFG_MUTEX
#  include <bosMutex.h>
#endif

#if BOS_CFG_TASK
#  include <bosTask.h>
#endif

/* ---- Public Variables ------------------------------------------------- */

#if BOS_CFG_ERROR_FUNCTION_STRINGS

typedef struct
{
   int            numFuncIds; /**< Number of entries pointed to by @a funcStr.   */
   const char   **funcEntry;  /**< Array of function pointers to string names.   */

} BOS_ERROR_MODULE;

BOS_ERROR_MODULE gBosErrorModule[ BOS_NUM_MODULES ];

#endif   /* BOS_CFG_ERROR_FUNCTION_STRINGS */

/* ---- Private Constants and Types -------------------------------------- */
/* ---- Private Variables ------------------------------------------------ */
/* ---- Private Function Prototypes -------------------------------------- */
/* ---- Functions -------------------------------------------------------- */

/**
 * @addtogroup bosError
 * @{
 */
 
#if BOS_CFG_ERROR_FUNCTION_STRINGS

/***************************************************************************/
/**
*  Registers some function strings with the error module. These function
*  strings are used to translate a module ID/function ID into a readable
*  ASCII string.
*
*  @param   moduleId    
*  @param   funcStr     
*  @param   numFuncIds  (in)
*
*  @return  Description of return value.
*/

BOS_STATUS bosErrorAddFunctionStrings
(
   BOS_MODULE moduleId,    /**< (in) Module ID that function strings are being registered for.  */
   const char **funcEntry, /**< (in) Array of pointers to strings for each function.            */ 
   int numFuncIds          /**< (in) Number of entries in the array of pointers.                */
)
{
   BOS_ASSERT( moduleId < BOS_NUM_MODULES );

   gBosErrorModule[ moduleId ].funcEntry  = funcEntry;
   gBosErrorModule[ moduleId ].numFuncIds = numFuncIds;

   return BOS_STATUS_OK;

} /* bosErrorAddFunctionStrings */

#endif   /* BOS_CFG_ERROR_FUNCTION_STRINGS */

/***************************************************************************/
/**
*  Translates an error source into a human readable ASCII string.
*
*  @param   errSrc      (in)  Numeric error source.
*  @param   paramName   (in)  Description
*
*  @return  Description of return value.
*/

void bosErrorFunctionStr( BOS_ERROR_SOURCE errSrc, char *outStr, size_t maxLen )
{
   BOS_MODULE        module = BOS_ERROR_MODULE( errSrc );
   BOS_FUNCTION_ID   funcId = BOS_ERROR_FUNC_ID( errSrc );
   
#if BOS_CFG_ERROR_FUNCTION_STRINGS
   const char       *funcStr = NULL;
   const char      **funcEntry;
   int               numFuncIds;

   if ( module < BOS_NUM_MODULES )
   {
      if (( funcEntry = gBosErrorModule[ module ].funcEntry ) != NULL )
      {
         numFuncIds = gBosErrorModule[ module ].numFuncIds;

         if ( funcId < numFuncIds )
         {
            funcStr = funcEntry[ funcId ];
         }
      }
   }

   if ( funcStr != NULL )
   {
      strPrintf( outStr, maxLen, "%.*s", maxLen, funcStr );
   }
   else
#endif   /* BOS_CFG_ERROR_FUNCTION_STRINGS */ 
   {
      strPrintf( outStr, maxLen, "Module:0x%02x, Function:0x%02x", module, funcId );
   }

} /* bosErrorFunctionStr */

/***************************************************************************/
/**
*  Initializes the BOS Error module.
*
*  @return  BOS_STATUS_OK if the initialization was successful, or an error
*           code if some type of error occured.
*/

BOS_STATUS bosErrorInit( void )
{
   memset( gBosErrorModule, 0, sizeof( gBosErrorModule ));

   return BOS_STATUS_OK;
}

/***************************************************************************/
/**
*  Terminates the BOS Error module.
*
*  @return  BOS_STATUS_OK if the termination was successful, or an error
*           code if some type of error occured.
*/

BOS_STATUS bosErrorTerm( void )
{
   return BOS_STATUS_OK;
}

/** @} */


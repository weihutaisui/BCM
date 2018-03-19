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
*  @file    strMaxCpy.c 
*
*  @brief   Implements strMaxCpy, a bounded string copy routine.
*
****************************************************************************/

/* ---- Include Files ---------------------------------------------------- */

#include "str.h"
#include <string.h>

/* ---- Public Variables ------------------------------------------------- */
/* ---- Private Constants and Types -------------------------------------- */
/* ---- Private Variables ------------------------------------------------ */
/* ---- Private Function Prototypes -------------------------------------- */
/* ---- Functions -------------------------------------------------------- */

/**
 * @addtogroup str
 * @{
 */

/***************************************************************************/
/**
*  Copies the source to the destination, but makes sure that the 
*  destination string (including terminating null), doesn't exceed 
*  maxLen.
*
*  @param   dst      (out) Place to store the string copy.
*  @param   src      (in)  String to copy.
*  @param   maxLen   (in)  Maximum number of characters to copy into @a dst.
*
*  @return  A pointer to the destination string.
*/

char *strMaxCpy( char *dst, const char *src, size_t maxLen )
{
   if ( maxLen < 1 )
   {
      /*
       * There's no room in the buffer?
       *
       * VxWorks complains if the cast isn't present (it considers "" to 
       * be const char *
       */

      return (char *)"";
   }

   if ( maxLen == 1 )
   {
      /*
       * There's only room for the terminating null character
       */

      dst[ 0 ] = '\0';
      return dst;
   }

   /*
    * The Visual C++ version of strncpy writes to every single character
    * of the destination buffer, so we use a length one character smaller
    * and write in our own null (if required).
    */

   strncpy( dst, src, maxLen - 1 );
   if (( strlen( src ) + 1 ) >= maxLen )
   {
      /*
       * The string exactly fits, or probably overflows the buffer.
       * Write in the terminating null character since strncpy doesn't in
       * this particular case.
       *
       * We don't do this arbitrarily so that the caller can use a sentinel 
       * in the very end of the buffer to detect buffer overflows.
       */

      dst[ maxLen - 1 ] = '\0';
   }

   return dst;

} /* strMaxCpy */

/** @} */


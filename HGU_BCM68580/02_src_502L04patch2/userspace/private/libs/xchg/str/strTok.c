/****************************************************************************
*
*     <:copyright-BRCM:2001:proprietary:standard
*     
*        Copyright (c) 2001 Broadcom 
*        All Rights Reserved
*     
*      This program is the proprietary software of Broadcom and/or its
*      licensors, and may only be used, duplicated, modified or distributed pursuant
*      to the terms and conditions of a separate, written license agreement executed
*      between you and Broadcom (an "Authorized License").  Except as set forth in
*      an Authorized License, Broadcom grants no license (express or implied), right
*      to use, or waiver of any kind with respect to the Software, and Broadcom
*      expressly reserves all rights in and to the Software and all intellectual
*      property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
*      NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
*      BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
*     
*      Except as expressly set forth in the Authorized License,
*     
*      1. This program, including its structure, sequence and organization,
*         constitutes the valuable trade secrets of Broadcom, and you shall use
*         all reasonable efforts to protect the confidentiality thereof, and to
*         use this information only in connection with your use of Broadcom
*         integrated circuit products.
*     
*      2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*         AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*         WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
*         RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
*         ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
*         FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
*         COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
*         TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
*         PERFORMANCE OF THE SOFTWARE.
*     
*      3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
*         ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
*         INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
*         WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
*         IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
*         OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
*         SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
*         SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
*         LIMITED REMEDY.
*     :>
*****************************************************************************/
/**
*
*  @file    strToken.c
*
*  @brief   Implements strToken, a thread-safe version of the standard ANSI
*           strtok function.
*
****************************************************************************/

/* ---- Include Files ---------------------------------------------------- */

#include "str.h"


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
*  Tokenizes a string. This function is conceptually similiar to ANSI C strtok(),
*  but allows strToken() to be used by different strings or callers at the same
*  time. Each call modifies '*string' by substituting a NULL character for the
*  first delimiter that is encountered, and updates 'string' to point to the char
*  after the delimiter. Leading delimiters are skipped.
*
*  @param   string      (mod) Ptr to string ptr, updated by token.
*  @param   delimiters  (in)  Set of delimiter characters.
*  @param   tokdelim    (out) Character that delimits the returned token. (May
**                            be set to NULL if token delimiter is not required).
*
* @return  Pointer to the next token found. NULL when no more tokens are found.
*
*/
char* strTokDelim( char **string, const char *delimiters, char *tokdelim )
{
   char *str;
   unsigned long map[8];
   int count;
   char *nextoken;

   if ( tokdelim != NULL )
   {
      /* Prime the token delimiter */
      *tokdelim = '\0';
   }

   /* Clear control map */
   for (count = 0; count < 8; count++)
   {
      map[count] = 0;
   }

   /* Set bits in delimiter table */
   do
   {
      map[*delimiters >> 5] |= (1 << (*delimiters & 31));
   }
   while (*delimiters++);

   str = *string;

   /* Find beginning of token (skip over leading delimiters). Note that
   ** there is no token iff this loop sets str to point to the terminal
   ** null (*str == '\0')
   */
   while (( (map[*str >> 5] & (1 << (*str & 31))) && *str) || (*str == ' '))
   {
      str++;
   }

   nextoken = str;

   /* Find the end of the token. If it is not the end of the string,
   ** put a null there.
   */
   for ( ; *str ; str++ )
   {
      if ( map[*str >> 5] & (1 << (*str & 31)) )
      {
         if ( tokdelim != NULL )
         {
            *tokdelim = *str;
         }

         *str++ = '\0';
         break;
      }
   }

   *string = str;

   /* Determine if a token has been found. */
   if ( nextoken == (char *) str )
   {
      return NULL;
   }
   else
   {
      return nextoken;
   }
}

/** @} */

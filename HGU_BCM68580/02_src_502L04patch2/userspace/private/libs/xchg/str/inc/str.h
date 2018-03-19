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
*  @file    str.h
*
*  @brief   String Manipulation routines.
*
*  Definitions for a variety of string manipulation routine. These routines
*  are all bounded, to ensure that memory overwrites don't occur.
*
****************************************************************************/
/**
*  @defgroup   str   String Manipulation
*
*  @brief   Provides a variety of string manipulation routines.
*/

#if !defined( STR_H )
#define STR_H                 /**< Include Guard                           */

#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>

/* ---- Include Files ---------------------------------------------------- */

/**
 * @addtogroup str
 * @{
 */

/* ---- Constants and Types ---------------------------------------------- */

/**
 * Pointer to a function which outputs a single character. This function
 * is called by the strXPrintf()/vStrXPrintf() functions to output a
 * character.
 */

#if defined(__cplusplus)
extern "C" {
#endif

typedef int (*strXPrintfFunc)( void *outParm, int ch );

/* ---- Variable Externs ------------------------------------------------- */
/* ---- Function Prototypes ---------------------------------------------- */

char *strMaxCpy( char *dst, const char *src, size_t maxLen );
char *strMaxCat( char *dst, const char *src, size_t maxLen );
char *strCatSize( char *dst, const char *src, size_t len );

int strPrintf( char *outStr, int maxLen, const char *fmt, ... );
int vStrPrintf( char *outStr, int maxLen, const char *fmt, va_list args );

int strXPrintf( strXPrintfFunc func, void *userParm, const char *fmt, ... );
int vStrXPrintf( strXPrintfFunc func, void *userParm, const char *fmt, va_list args );

char* strCpyLower( char *dst, const char *src, size_t maxLen );
char* strCpyUpper( char *dst, const char *src, size_t maxLen );

int strCmpNoCase( const char *str1, const char *str2 );
int strCmpNoCaseSize( const char *str1, const char *str2, size_t n );

#define strTok( str, delim )   strTokDelim( str, delim, NULL )
char* strTokDelim( char **string, const char *delimiters, char *tokdelim );

int strScanf(const char *format, ...);
int strFScanf(FILE *stream, const char *format, ...);
int strSScanf(char *s, const char *format, ...);

#if defined(__cplusplus)
}
#endif

/** @} */

#endif /* STR_H */


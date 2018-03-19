/*
 *
 * Copyright (c) 2003-2012  Broadcom Corporation
 * All Rights Reserved
 *
 * <:label-BRCM:2012:proprietary:standard
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
 *
 * $Id: typedefs.h,v 1.49.8.1 2003/10/16 23:10:58 mthawani Exp $
 */

#ifndef _TYPEDEFS_H_
#define _TYPEDEFS_H_

/*----------------------- define TRUE, FALSE, NULL, bool ----------------*/
#ifdef __cplusplus

#ifndef FALSE
#define FALSE	false
#endif
#ifndef TRUE
#define TRUE	true
#endif

#else /* !__cplusplus */

#if defined(_WIN32) || defined(__klsi__)

typedef	unsigned char	bool;

#else

#if defined(MACOSX) && defined(KERNEL)
#include <IOKit/IOTypes.h>
#else
typedef	int	bool;
#endif

#endif

#ifndef FALSE
#define FALSE	0
#endif
#ifndef TRUE
#define TRUE	1

#ifndef NULL
#define	NULL 0
#endif

#endif

#endif /* __cplusplus */

#ifdef vxworks
#include <private/cplusLibP.h>
#else	/* vxworks */
#ifndef OFF
#define	OFF	0
#endif
#endif	/* vxworks */

#ifndef ON
#define	ON	1
#endif

/*----------------------- define uchar, ushort, uint, ulong ----------------*/

typedef unsigned char uchar;

#if defined(_WIN32) || defined(PMON) || defined(PSOS) || defined(__klsi__) || defined(__MRC__) || defined(_CFE_)

typedef unsigned short	ushort;

typedef unsigned int	uint;
typedef unsigned long	ulong;

#else

/* pick up ushort & uint from standard types.h */
#if defined(linux) && defined(__KERNEL__)
#include <linux/types.h>	/* sys/types.h and linux/types.h are oil and water */
#else
#include <sys/types.h>	
#if !defined(TARGETENV_sun4) && !defined(linux)
typedef unsigned long	ulong;
#endif /* TARGETENV_sun4 */
#endif
#if defined(vxworks) || defined(PMON)
typedef unsigned int	uint;
typedef unsigned long long       uint64;
#endif

#endif /* WIN32 || PMON || .. */

/*----------------------- define [u]int8/16/32/64 --------------------------*/

#if defined(__klsi__)

typedef signed char	int8;
typedef signed short	int16;
typedef signed int	int32;

typedef unsigned char	uint8;
typedef unsigned short	uint16;
typedef unsigned int	uint32;

typedef struct {
	int32	hi;
	int32	lo;
} int64;

typedef struct {
	uint32	hi;
	uint32	lo;
} uint64;

#else /* !klsi */

typedef signed char	int8;
typedef signed short	int16;
typedef signed int	int32;

typedef unsigned char	uint8;
typedef unsigned short	uint16;
typedef unsigned int	uint32;

typedef float		float32;
typedef double		float64;

/*
 * abstracted floating point type allows for compile time selection of
 * single or double precision arithmetic.  Compiling with -DFLOAT32
 * selects single precision; the default is double precision.
 */

#if defined(FLOAT32)
typedef float32 float_t;
#else /* default to double precision floating point */
typedef float64 float_t;
#endif /* FLOAT32 */

#ifdef _MSC_VER	/* Microsoft C */
typedef signed __int64	int64;
typedef unsigned __int64 uint64;

#elif defined(__GNUC__) && !defined(__STRICT_ANSI__) && !defined(vxworks)
/* gcc understands signed/unsigned 64 bit types, but complains in ANSI mode */
typedef signed long long int64;
typedef unsigned long long uint64;

#elif defined(__ICL) && !defined(__STDC__)
/* ICL accepts unsigned 64 bit type only, and complains in ANSI mode */
typedef unsigned long long uint64;

#endif /* _MSC_VER */

#endif /* klsi */

/*----------------------- define PTRSZ, INLINE --------------------------*/

#define	PTRSZ	sizeof (char*)

#ifndef INLINE

#ifdef _MSC_VER

#define INLINE __inline

#elif __GNUC__

#define INLINE __inline__

#else

#define INLINE

#endif /* _MSC_VER */

#endif /* INLINE */

#endif /* _TYPEDEFS_H_ */

//**************************************************************************** 
// 
// Copyright (c) 2008 Broadcom Corporation
//
// This program is the proprietary software of Broadcom Corporation and/or
// its licensors, and may only be used, duplicated, modified or distributed
// pursuant to the terms and conditions of a separate, written license
// agreement executed between you and Broadcom (an "Authorized License").
// Except as set forth in an Authorized License, Broadcom grants no license
// (express or implied), right to use, or waiver of any kind with respect to
// the Software, and Broadcom expressly reserves all rights in and to the
// Software and all intellectual property rights therein.  IF YOU HAVE NO
// AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY,
// AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE
// SOFTWARE.  
//
// Except as expressly set forth in the Authorized License,
//
// 1.     This program, including its structure, sequence and organization,
// constitutes the valuable trade secrets of Broadcom, and you shall use all
// reasonable efforts to protect the confidentiality thereof, and to use this
// information only in connection with your use of Broadcom integrated circuit
// products.
//
// 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
// "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS
// OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
// RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL
// IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR
// A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET
// ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME
// THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
//
// 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM
// OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
// INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY
// RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM
// HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN
// EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1,
// WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY
// FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
// 
//**************************************************************************** 
// 
//  $Id$
// 
//  Filename:       bcmtypes.h 
//  Author:         [author] 
//  Creation Date:  [date] 
// 
//**************************************************************************** 
//  Description: 
//       
//       
//       
//       
// 
//**************************************************************************** 
//
// bcmtypes.h - misc useful typedefs
//
#ifndef BCMTYPES_H
#define BCMTYPES_H

#ifdef WIFI_TYPEDEFS_H
#include <typedefs.h>
#else

// sys/types.h defines UCHAR, USHORT, UINT, and ULONG (under pSOS).  It's
// safer to include it here.
#if (defined( __KERNEL__) || defined(TARGETOS_Linux))
  #include "types.h"
#else
  #include <sys/types.h>
#endif

#if defined( TARGETOS_eCos ) || defined( TARGETOS_Qnx6 ) || defined( TARGETOS_Linux )
  #include "types.h"
#endif

// These are also defined in typedefs.h in the application area, so I need to
// protect against re-definition.
#ifndef TYPEDEFS_H

typedef unsigned char   byte;
typedef unsigned char   uint8;
typedef unsigned short  uint16;
typedef unsigned long   uint32;
typedef long long       int64;
typedef signed char     int8;
typedef signed short    int16;
typedef signed long     int32;

typedef unsigned long   BcmHandle;

#endif
#ifndef uint64
	#define uint64 unsigned long long
#endif

typedef unsigned long   semaphore_t;

typedef unsigned long   *PULONG,DWORD,*PDWORD;
typedef signed long     LONG,*PLONG;

typedef unsigned int    *PUINT;
typedef signed int      INT;

typedef unsigned short  *PUSHORT;
typedef signed short    SHORT,*PSHORT,WORD,*PWORD;

typedef unsigned char   *PUCHAR;
typedef signed char     *PCHAR;

typedef void            *PVOID;

typedef unsigned char   BOOLEAN, *PBOOL, *PBOOLEAN;

typedef unsigned char   BYTE,*PBYTE;

#ifndef TARGETOS_vxWorks
//The following has been defined in Vxworks internally: vxTypesOld.h
//redefine under vxworks will cause error

typedef signed int      *PINT;

#ifndef NUMBER_TYPES_ALREADY_DEFINED
#define NUMBER_TYPES_ALREADY_DEFINED
typedef signed char     INT8;
typedef signed short    INT16;
typedef signed long     INT32;
typedef signed long     sint32;
#define SINT32 sint32
typedef signed long long SINT64;

typedef unsigned char   UINT8;
typedef unsigned short  UINT16;
typedef unsigned long   UINT32;
typedef unsigned long long UINT64;
#endif

#ifndef BASE_TYPE_ALREADY_DEFINED
#define BASE_TYPE_ALREADY_DEFINED
typedef unsigned char   UCHAR;
typedef unsigned short  USHORT;
typedef unsigned int    UINT;
typedef unsigned long   ULONG;
#endif

typedef void            VOID;
typedef unsigned char   BOOL;

#else 
// This doesn't really belong here, but it avoids a lot of weirdness elsewhere.  msieweke
#include <types/vxTypesOld.h>
#endif  /* TARGETOS_vxWorks */


// These are also defined in typedefs.h in the application area, so I need to
// protect against re-definition.
#ifndef TYPEDEFS_H

#define MAX_INT16 32767
#define MIN_INT16 -32768

// Useful for true/false return values.  This uses the
// Taligent notation (k for constant).
typedef enum
{
    kFalse = 0,
    kTrue = 1
} Bool;

#endif

/* macros to protect against unaligned accesses */

/* first arg is an address, second is a value */
#define PUT16( a, d ) { 		\
  *((byte *)a) = (byte)((d)>>8); 	\
  *(((byte *)a)+1) = (byte)(d); 	\
}

#define PUT32( a, d ) { 		\
  *((byte *)a) = (byte)((d)>>24); 	\
  *(((byte *)a)+1) = (byte)((d)>>16); 	\
  *(((byte *)a)+2) = (byte)((d)>>8); 	\
  *(((byte *)a)+3) = (byte)(d); 	\
}

/* first arg is an address, returns a value */
#define GET16( a ) ( 			\
  (*((byte *)a) << 8) |			\
  (*(((byte *)a)+1))	 		\
)

#define GET32( a ) ( 			\
  (*((byte *)a) << 24)     |		\
  (*(((byte *)a)+1) << 16) | 		\
  (*(((byte *)a)+2) << 8)  | 		\
  (*(((byte *)a)+3))	 		\
)

/* Register Macros to handle shadow register writing for 
   bitfields.  This macro handles read modify writes.    */
#define ReadModWrField(reg, type, field, value) \
{                                               \
	type local_##type;				   	        \
	local_##type.Reg32 = reg.Reg32;             \
	local_##type.Bits.field = value;            \
	reg.Reg32 = local_##type.Reg32;             \
}


/* Register Macros to handle shadow register writing for 
   bitfields.  This macro handles write onlys.           */
#define WrField(reg, type, field, value)        \
{                                               \
	type local_##type;				   	        \
	local_##type.Bits.field = value;            \
	reg.Reg32 = local_##type.Reg32;             \
}


/* Register Macros to handle shadow register writing for 
   bitfields.  This macro handles read onlys.           */
#define ReadField(reg, type, field, value)      \
{                                               \
	type local_##type;				   	        \
	local_##type.Reg32 = reg.Reg32;             \
	value = local_##type.Bits.field;            \
}


/* Register Macros to handle shadow register writing for 
   8 bit bitfields.  This macro handles write onlys.           */
#define WrField8(reg, type, field, value)        \
{                                               \
	type local_##type;				   	        \
	local_##type.Bits.field = value;            \
	reg.Reg8 = local_##type.Reg8;             \
}


/* Register Macros to handle shadow register writing for 
   bitfields.  This macro handles read onlys.           */
#define ReadField8(reg, type, field, value)      \
{                                               \
	type local_##type;				   	        \
	local_##type.Reg8 = reg.Reg8;             \
	value = local_##type.Bits.field;            \
}



/* General bit enable */
enum
{
    ONE  = 1,
    ZERO = 0
};


#ifndef YES
#define YES 1
#endif

#ifndef NO
#define NO  0
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#endif /* WIFI_TYPEDEFS_H */

#endif

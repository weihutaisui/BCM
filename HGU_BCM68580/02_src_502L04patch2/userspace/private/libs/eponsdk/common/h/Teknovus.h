/*
* <:copyright-BRCM:2013:proprietary:epon
* 
*    Copyright (c) 2013 Broadcom 
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
*/
    
#if !defined(Teknovus_h)
#define Teknovus_h

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ArchCfg.h"
#include "bcm_epon_common.h"
/*
    \file Teknovus.h 
    \brief Customary local conventions and utilities
   
    This header contains Teknovus-specific idiosyncracies for our
   	coding preferences
   
*/
#include "Build.h"
#include "eponctl_types.h"

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(CPU_ARCH_PPC) || defined(OS_LINUX)
#define PACK __attribute__((__packed__))
#elif defined(CPU_ARCH_ARM) || defined(CPU_ARCH_X86)
#define PACK 
#else
#error "Must define CPU_ARCH_XXXX"
#endif

#if defined(OS_WIN32) || defined(OS_VXWORKS) || defined(OS_LINUX)
#else
#error "Must define OS_xxxx"
#endif


#if  __BYTE_ORDER == __BIG_ENDIAN
#define CPU_ENDIAN_BIG
#elif __BYTE_ORDER == __LITTLE_ENDIAN
#define CPU_ENDIAN_LITTLE
#else
#error cannot detect endianess
#endif



#define MaxU16Val 0xFFFF
////////////////////////////////////////////////////////////////////////////////
/// \brief  Masks and shifts a field out of a value
///
/// \param field    The field to extract (defines mask and shift)
/// \param var      The value to extract the field from
///
/// \return
/// The extracted field
////////////////////////////////////////////////////////////////////////////////
#define TkGetField(field, var)  (((U32)(var) & field##Msk) >> field##Sft)


////////////////////////////////////////////////////////////////////////////////
/// \brief  Applies a mask and shift to a field value
///
/// \param field    The field to create (defines mask and shift)
/// \param var      The value to put in the field
///
/// \return
/// The value put into the field
////////////////////////////////////////////////////////////////////////////////
#define TkPutField(field, var)  (((U32)(var) << field##Sft) & field##Msk)


////////////////////////////////////////////////////////////////////////////////
/// \brief  Sets a field in a value
///
/// \param field    The field to set (defines mask and shift)
/// \param fieldVal The new value to set the field to
/// \param regVal   The original value to set the field in
///
/// \return
/// The original value with the new field value
////////////////////////////////////////////////////////////////////////////////
#define TkSetField(field, fieldVal, regVal)              \
    (((regVal) & ~(field##Msk)) | TkPutField(field, fieldVal))


#define TkClearBit(val, bit) ((val) & ~(1UL << (bit)))
#define TkSetBit(val, bit) ((val) | (1UL << (bit)))

#define TkMaxValField(field) TkGetField(field, field##Msk)
#define FullMsk32 0xFFFFFFFFUL
#define MakeMsk32(Msb,Lsb) (((FullMsk32)>>((31-(Msb))+(Lsb))) << (Lsb))
#define MakeSft32(Msb,Lsb) (Lsb)

#define LsbMask(numBits)        ((1UL << (numBits)) - 1)
#define GetBits(value, offset, width)   (((value) >> (offset)) & LsbMask(width))
#define __GetBit(b, p) ((0x##b##UL & (1UL << (4*p))) >> (3*p))
#define TkBits(b) \
    __GetBit(b,0) | __GetBit(b,1) | __GetBit(b,2) | __GetBit(b,3) |     \
    __GetBit(b,4) | __GetBit(b,5) | __GetBit(b,6) | __GetBit(b,7)


////////////////////////////////////////////////////////////////////////////////
/// \brief Test whether the given bits are set in a value
///
/// \param val  The value to test
/// \param bits The bits to test for
///
/// \return
/// TRUE if all bits are set in the value, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
#define TestBitsSet(val, bits)  (((val) & (bits)) == (bits))


////////////////////////////////////////////////////////////////////////////////
/// \brief Test whether any of the given bits are set in a value
///
/// \param val  The value to test
/// \param bits The bits to test for
///
/// \return
/// TRUE if any of the bits are set in the value, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
#define TestBitsAny(val, bits)  (((val) & (bits)) != 0)


////////////////////////////////////////////////////////////////////////////////
/// \brief Test if t1 is less than t2, accounting for over/underflow
///
/// \param t1   First value
/// \param t2   Second value
///
/// \return
/// TRUE if t1 < t2, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
#define U16LessThan(t1, t2)     ((U16)(((U16)(t1) - (U16)(t2))) >= 0x8000)
#define U32LessThan(t1, t2)     ((U32)(((U32)(t1) - (U32)(t2))) >= 0x80000000)


////////////////////////////////////////////////////////////////////////////////
/// \brief  Convert a pointer to a U24 to a U32 value
///
/// \param  p   Pointer to U24
///
/// \return U32 value of U24
////////////////////////////////////////////////////////////////////////////////
#define U24ToU32(p)                     \
    ((((U32)((U8*)(p))[0]) << 16) +     \
     (((U32)((U8*)(p))[1]) << 8) +      \
     ((U32)((U8*)(p))[2]))


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the difference between p1 and p2 in bytes
///
/// \param p1   First pointer
/// \param p2   Second pointer
///
/// \return
/// p1 - p2 in bytes
////////////////////////////////////////////////////////////////////////////////
#define PointerDiff(p1, p2)     (((U8*)(p1)) - ((U8*)(p2)))


////////////////////////////////////////////////////////////////////////////////
/// \brief Add length bytes to pointer p
///
/// \param p    The pointer
/// \param len  Number of bytes to add to pointer
///
/// \return
/// p + len bytes
////////////////////////////////////////////////////////////////////////////////
#define PointerAdd(p, len)      (((U8*)(p)) + (len))


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the minimum of a and b
///
/// \param a    First value
/// \param b    Second value
///
/// \return
/// minimum of a and b
////////////////////////////////////////////////////////////////////////////////
#define Min(a, b)              (((a) < (b)) ? (a) : (b))


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the maximum of a and b
///
/// \param  a   First value
/// \param  b   Second value
///
/// \return maximum of a and b
////////////////////////////////////////////////////////////////////////////////
#define Max(a, b)              (((a) > (b)) ? (a) : (b))


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the ceiling of a / b
///
/// \param a    First value
/// \param b    Second value
///
/// \return
/// ceiling of a / b
////////////////////////////////////////////////////////////////////////////////
#define Ceil(a, b)              (((a) + ((b) - 1)) / (b))

////////////////////////////////////////////////////////////////////////////////
/// \brief  Perform safe subtraction of unsigned values
///
/// \param a    First value
/// \param b    Second value
///
/// \return a - b, or 0 if b > a
////////////////////////////////////////////////////////////////////////////////
#define SafeSub(a, b)              (((a) > (b)) ? ((a) - (b)) : 0)
#define ArrayLength(arr) (sizeof(arr) / sizeof(arr[0]))
#define EnumLessThan(a, b)              (((a) >= 0) && ((a) < (b)))
#if !defined(NULL)
#define NULL  			((void*)0)
#endif

// TODO: to tweak

////////////////////////////////////////////////////////////////////////////////
/// \brief unsigned 32-bit Byte swapping
///
/// This macro will swap the byte order for a 32-bit value from Little Endian
/// to Big Endian or vice versa
///
/// \param x    32-bit value with byte order 0x01020304
///
/// \return     32-bit value with byte order 0x04030201
///
////////////////////////////////////////////////////////////////////////////////
#define BswapU32(x)         ((((U32)(x) << 24) & 0xFF000000) | \
                            (((U32)(x)  >> 24) & 0x000000FF) | \
                            (((U32)(x)  << 8)  & 0x00FF0000) | \
                            (((U32)(x)  >> 8)  & 0x0000FF00) )


////////////////////////////////////////////////////////////////////////////////
/// \brief unsigned 16-bit Byte swapping
///
/// This macro will swap the byte order for a 16-bit value from Little Endian
/// to Big Endian or vice versa
///
/// \param x    16-bit value with byte order 0x0102
///
/// \return     16-bit value with byte order 0x0201
///
////////////////////////////////////////////////////////////////////////////////
#define BswapU16(x)         ((((U16)(x) >> 8)  & 0x00FF) | \
                            (((U16) (x) << 8)  & 0xFF00))

////////////////////////////////////////////////////////////////////////////////
/// \brief HostToXY
///
/// These macros will always translate the given (Y=32) or (Y=16)-bit values
/// from the host format to Big (X=BE) or Little (X=LE) Endian, based on the
/// definition of CPU_ENDIAN_BIG
/// \param y Value to convert

///
/// \return
/// the input in BE or LE as dictated by the macro.
////////////////////////////////////////////////////////////////////////////////
#ifdef CPU_ENDIAN_BIG
#define HostToLE32(y)       (BswapU32(y))
#define HostToBE32(y)       (y)
#define HostToLE16(y)       (BswapU16(y))
#define HostToBE16(y)       (y)
#endif

typedef enum eRc
    {
    RcOk,
    RcFail,
	RcFound,
   	RcNotFound,
	RcEmptyFound,
    RcBadParam,
	RcTimedOut,
	RcDoubleAlloc,
	RcTooManyLlids,
	RcNoLlidToRegister,
	RcNotProvisioned,
	RcNoOamBw,
	RcWrongMode,
	RcOnuReturnedErr,
	RcMissingTlv,
	RcBadOnuResponse,
	RcNoResource,

    RcEnumForceS8    = 0x7F
    } eRc;

#define RESP_ID  	1234
#define OLT_ID   	4567
#define ALARM_ID 	4890
#define CMS_MSG_ID   	6000

#if defined(__cplusplus)
}
#endif

#endif /*  Teknovus */

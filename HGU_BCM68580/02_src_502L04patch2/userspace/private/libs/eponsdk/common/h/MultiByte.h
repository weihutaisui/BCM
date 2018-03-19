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

#ifndef MultiByte_h
#define MultiByte_h

#include "Teknovus.h"

/// provides access to bytes of a U16
typedef union
    {
    U16 u16;
    U8  array[2];
#if defined(CPU_ENDIAN_BIG)
    struct { U8 msb; U8 lsb; } bytes;
#elif defined(CPU_ENDIAN_LITTLE)
    struct { U8 lsb; U8 msb; } bytes;
#else
#error Must define CPU_ENDIAN_BIG or _LITTLE
#endif
    } MultiByte16;

/// provides access to words/bytes of a U32
typedef union
    {
    U32 u32;
    U8  array[4];
    U16 warray[2];
#if defined(CPU_ENDIAN_BIG)
    struct { MultiByte16 msw; MultiByte16 lsw; } words;
#elif defined(CPU_ENDIAN_LITTLE)
    struct { MultiByte16 lsw; MultiByte16 msw; } words;
#else
#error Must define CPU_ENDIAN_BIG or _LITTLE
#endif
    } MultiByte32;

// Pack only needed for structures not a common multiple of U16 or U32
/// provides access to words/bytes of a U48
typedef union
    {
    U48 u48;
    U8  array[6];
    U16 warray[3];
#if defined(CPU_ENDIAN_BIG)
    struct { MultiByte16 msw; MultiByte32 lsw; } words;
#elif defined(CPU_ENDIAN_LITTLE)
    struct { MultiByte32 lsw; MultiByte16 msw; } words;
#else
#error Must define CPU_ENDIAN_BIG or _LITTLE
#endif
    } PACK MultiByte48;

/// provides access to words/bytes of a U48
typedef union
    {
    U64 u64;
    U32 dword[2];
    U16 warray[4];
    U8  array[8];
#if defined(CPU_ENDIAN_BIG)
    struct { MultiByte32 msw; MultiByte32 lsw; } words;
    struct { U16 hiWord; U48 val; } u48;
#elif defined(CPU_ENDIAN_LITTLE)
    struct { MultiByte32 lsw; MultiByte32 msw; } words;
    struct { U48 val; U16 hiWord; } u48;
#else
#error Must define CPU_ENDIAN_BIG or _LITTLE
#endif
    } MultiByte64;

typedef union
    {
    U64 qword[2];
    U32 dword[4];
    U16 word[8];
    U8  byte[16];
    } MultiByte128;
#endif

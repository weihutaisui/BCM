//
// <:copyright-BRCM:2014:proprietary:epon
// 
//    Copyright (c) 2014 Broadcom 
//    All Rights Reserved
// 
//  This program is the proprietary software of Broadcom and/or its
//  licensors, and may only be used, duplicated, modified or distributed pursuant
//  to the terms and conditions of a separate, written license agreement executed
//  between you and Broadcom (an "Authorized License").  Except as set forth in
//  an Authorized License, Broadcom grants no license (express or implied), right
//  to use, or waiver of any kind with respect to the Software, and Broadcom
//  expressly reserves all rights in and to the Software and all intellectual
//  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
//  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
//  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
// 
//  Except as expressly set forth in the Authorized License,
// 
//  1. This program, including its structure, sequence and organization,
//     constitutes the valuable trade secrets of Broadcom, and you shall use
//     all reasonable efforts to protect the confidentiality thereof, and to
//     use this information only in connection with your use of Broadcom
//     integrated circuit products.
// 
//  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
//     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
//     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
//     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
//     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
//     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
//     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
//     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
//     PERFORMANCE OF THE SOFTWARE.
// 
//  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
//     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
//     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
//     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
//     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
//     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
//     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
//     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
//     LIMITED REMEDY.
// :>
//

////////////////////////////////////////////////////////////////////////////////
/// \file Md5.c
/// \brief MD5 message digest algorithm
///
/// This file implements the MD5 one-way hash as per RFC1321
/// The implementation is limited to messages of at most 512 MBytes
/// (2^32 bits) and is not reentrant.
///
////////////////////////////////////////////////////////////////////////////////

#include <string.h>
#if !defined(_ARC)
//#include <intrins.h>  // _lrol_
#endif
#include "Teknovus.h"
#include "Md5.h"
#include "MultiByte.h"

#define Md5BlockSize	64


typedef struct 
	{
    U32 abcd[4];			// MD5 register set
    U8  buf[Md5BlockSize];	// saved partial block from previous Append()
    MultiByte32 length;	 	// message length in bytes
	} Md5State;

Md5State md5State;  // current state of MD5 algorithm.   Since this is
					// global, only one MD5 calculation can be in progress
					// at once.	 Make this a parameter and pass it around
					// everywhere to have reentrant code.


/// 32-bit rotate left
#if defined(_ARC)
#define RotateLeft(x, n) (((x)<<(n))|((x) >> (32-(n))))
#else
#define RotateLeft(x, n) (((x)<<(n))|((x) >> (32-(n)))) //_lrol_(x, n)
#endif

// Auxiliary functions for each round
typedef
	U32 (*Md5Aux) (U32, U32, U32);

static
U32 F(U32 x, U32 y, U32 z)
	{
	return ((x & y) | (~x & z));
	}
static
U32 G(U32 x, U32 y, U32 z) 
	{
	return ((x & z) | (y & ~z));
	}
static
U32 H(U32 x, U32 y, U32 z)
	{
	return x ^ y ^ z;
	}
static
U32 I(U32 x, U32 y, U32 z) 
	{
	return (y ^ (x | ~z));
	}

////////////////////////////////////////////////////////////////////////////////
/// RoundAux:  Call the auxiliary function for this round
///
/// Keil C can only call through function pointers if the parameters all fit
/// in registers.  That's not the case with 3x 32-bit words.  So, we dispatch
/// through a case statement instead of a function pointer.
///
 // Parameters:
///	\param num		Function "number" to call
///	\param x, y, z	Params for aux function
/// 
/// \return 
/// result of aux function
////////////////////////////////////////////////////////////////////////////////
static 
U32 RoundAux (U8 num, U32 x, U32 y, U32 z)
	{
	switch (num)
		{
		case 1: return G (x, y, z);
		case 2: return H (x, y, z);
		case 3: return I (x, y, z);
		case 0:
		default:
			return F (x, y, z);
		}
	}

/// Per-round control data
typedef struct
	{
	U8		f;			// auxiliary function number
	U8		k[16];  	// indices into current block
	U8		rot[16];   	// number of bits to rotate
	U32		T[16];		// "T" constants to add
	} PACK Md5RoundParams;

/// parameters that define each MD5 round
const Md5RoundParams CODE Md5Round[4] = 
	{

	{ // round 1
	0,
	{ 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15 },
	{ 7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22},
	{ // round constants 
	0xd76aa478,
	0xe8c7b756,
	0x242070db,
	0xc1bdceee,

	0xf57c0faf,
	0x4787c62a,
	0xa8304613,
	0xfd469501,

	0x698098d8,
	0x8b44f7af,
	0xffff5bb1,
	0x895cd7be,

	0x6b901122,
	0xfd987193,
	0xa679438e,
	0x49b40821
	}

	}, // round 1

	{ // round 2
	1,
	{  1,  6, 11,  0,  5, 10, 15,  4, 9, 14,  3,  8, 13, 2,  7, 12 },
	{  5,  9, 14, 20,  5,  9, 14, 20, 5,  9, 14, 20,  5, 9, 14, 20},

	{  // T constants
	0xf61e2562,
	0xc040b340,
	0x265e5a51,
	0xe9b6c7aa,

	0xd62f105d,
	0x02441453,
	0xd8a1e681,
	0xe7d3fbc8,

	0x21e1cde6,
	0xc33707d6,
	0xf4d50d87,
	0x455a14ed,

	0xa9e3e905,
	0xfcefa3f8,
	0x676f02d9,
	0x8d2a4c8a
	}

	}, // round 2

	{ // round 3
	2,
	{ 5,  8, 11, 14, 1,  4,  7, 10, 13,  0,  3,  6, 9, 12, 15,  2 },
	{ 4, 11, 16, 23, 4, 11, 16, 23,  4, 11, 16, 23, 4 ,11, 16, 23},

	{  // T constants
	0xfffa3942,
	0x8771f681,
	0x6d9d6122,
	0xfde5380c,

	0xa4beea44,
	0x4bdecfa9,
	0xf6bb4b60,
	0xbebfbc70,

	0x289b7ec6,
	0xeaa127fa,
	0xd4ef3085,
	0x04881d05,

	0xd9d4d039,
	0xe6db99e5,
	0x1fa27cf8,
	0xc4ac5665
	}

	},

	{ // round 4
	3,
	{ 0,  7, 14,  5, 12,  3, 10,  1, 8, 15,  6, 13, 4, 11,  2,  9 },
	{ 6, 10, 15, 21,  6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21},

	{  // T constants
	0xf4292244, 
	0x432aff97, 
	0xab9423a7, 
	0xfc93a039,

	0x655b59c3, 
	0x8f0ccc92, 
	0xffeff47d, 
	0x85845dd1,

	0x6fa87e4f, 
	0xfe2ce6e0, 
	0xa3014314, 
	0x4e0811a1,

	0xf7537e82, 
	0xbd3af235, 
	0x2ad7d2bb, 
	0xeb86d391
	}

	} // round 4 

	};

////////////////////////////////////////////////////////////////////////////////
/// DoMd5Round:  executes one round of MD5
///
 // Parameters:
///	\param round 		Descriptor for round control data
/// \param src			data to process
/// 
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////
static
void DoMd5Round (const Md5RoundParams* round,
			     const U32* src)
	{
	U32 FAST t;
	U8 FAST i;
	U8 FAST idxAdd;

	U8 CODE * FAST k = (U8 *)round->k;
	U8 CODE * FAST rot = (U8 *)round->rot;
	U32 CODE * FAST T = (U32 *)round->T;
	U8 FAST f = round->f;

	idxAdd = 4;
	for (i = 0; i < 16; ++i)
		{
#define reg(off)	md5State.abcd[(idxAdd + (off)) & 3]
		t = reg(0) + 
			RoundAux (f, reg(1), reg(2), reg(3)) + 
			src[*k] + 
			*T;
		++k;
		++T;

		reg(0) = RotateLeft(t, *rot) + reg(1);    
		++rot;
		--idxAdd;
		}


	} // Md5Round


////////////////////////////////////////////////////////////////////////////////
/// FixEndian:  fix endianness for big-endian U32 processing
///
/// Could probably invert the whole algorithm with some study, but
/// this is easier to get right.
///
 // Parameters:
///	\param dst	 src, byte-reversed in 32-bit words
/// \param src	 src data
/// 
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////
static
void FixEndian (U8 numU32, U8 BULK* dst, const U8* src)
    {
    U8 FAST i;

    for (i = 0; i < numU32; ++i)
        {
#if defined(CPU_ENDIAN_BIG)
        *dst++ = src[3];
        *dst++ = src[2];
        *dst++ = src[1];
        *dst++ = src[0];
#elif defined(CPU_ENDIAN_LITTLE)
        *dst++ = src[0];
        *dst++ = src[1];
        *dst++ = src[2];
        *dst++ = src[3];
#else
#error Must define CPU_ENDIAN_BIG or _LITTLE
#endif
        src += 4;
        }
    } // FixEndian



////////////////////////////////////////////////////////////////////////////////
/// Md5ProcessBlock:  Process data through MD5
///
 // Parameters:
///	\param src 	Next 64-byte block of data to process
/// 
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////
static
void Md5ProcessBlock (const U8* src)
	{
	U32 BULK abcd[4];
	U32 BULK dst[16];

	U8 FAST i;

	// remember original values of register set
	memcpy (abcd, md5State.abcd, 4 * sizeof(U32));

	FixEndian (16, (U8 BULK*)dst, src);

	for (i = 0; i < 4; ++i)
		{
		DoMd5Round (&Md5Round[i], dst);
		}

	// add original values back into current register set
    md5State.abcd[0] += abcd[0];
    md5State.abcd[1] += abcd[1];
    md5State.abcd[2] += abcd[2];
    md5State.abcd[3] += abcd[3];

	} // Md5ProcessBlock


////////////////////////////////////////////////////////////////////////////////
/// Md5Start:  initialize the algorithm
///
/// This function is called before processing data through the MD5 hash.
///
 // Parameters:
/// None.  The state of the algorithm is global, which means only one
/// MD5 calculation can be in progress at one time.  We're squeezing every
/// byte out of code space we can for the 8051.
/// 
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////
void Md5Start (void)
	{
    md5State.length.u32 = 0;

    md5State.abcd[0] = 0x67452301;
    md5State.abcd[1] = 0xefcdab89;
    md5State.abcd[2] = 0x98badcfe;
    md5State.abcd[3] = 0x10325476;
	} // Md5Start



////////////////////////////////////////////////////////////////////////////////
/// Md5Append:  process some bytes through MD5
///
/// This function may be called multiple times, after Md5Start, to process
/// the message in blocks.
///
 // Parameters:
/// \param buf  Bytes to process
/// \param size Number of bytes in buf
/// 
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////
void Md5Append(const U8* p, U16 size)
	{
    U8 FAST saved;
    if (size == 0)
		{
		return;
		}

	saved = md5State.length.words.lsw.bytes.lsb % Md5BlockSize;
	md5State.length.u32 += size;

    // Process any initial partial block from previous calls
    if (saved > 0) 
    	{
		if (((U16)saved + size) >= Md5BlockSize) 
			{ // can fill out full block
			U16 FAST toCopy = Md5BlockSize - saved;

			memcpy(md5State.buf + saved, p, toCopy);

			Md5ProcessBlock (md5State.buf);

			saved = 0;
			p += toCopy;
			size -= toCopy;
			}
    	}

    // Process full blocks while we can
	while (size >= Md5BlockSize)
		{
		Md5ProcessBlock (p);
		p += Md5BlockSize;
		size -= Md5BlockSize;
		}

    // Save any final partial block
    if (size > 0)
		{
		memcpy(md5State.buf + saved, p, size);
		}

	} // Md5Append



////////////////////////////////////////////////////////////////////////////////
/// Md5Finish:  complete the algorithm
///
/// Wraps up the MD5 algorithm and returns the digest value
///
 // Parameters:
/// \param digest   Buffer to receive the 16-byte MD5 digest value
/// 
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////

static const U8 CODE pad[Md5BlockSize] = 
	{
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

//extern
void Md5Finish(U8 digest[Md5HashSize])
    {
    U8 lenBits[8];
    MultiByte32 FAST bitLen;
    U8 FAST i;
    U16 FAST saved = md5State.length.u32 % Md5BlockSize;

    // Save the length before padding, in bits, little-endian
    bitLen.u32 = md5State.length.u32 << 3; // byte count to bit count
    for (i = 0; i < 4; ++i)
        {
#if defined(CPU_ENDIAN_BIG)
        lenBits[i] = bitLen.array[3 - i];
#elif defined(CPU_ENDIAN_LITTLE)
        lenBits[i] = bitLen.array[i];
#else
#error Must define CPU_ENDIAN_BIG or _LITTLE
#endif
        lenBits[i+4] = 0;
        }

    // Pad to 56 bytes in final block
    // 120 = 56 + 64, to avoid negative integers
    Md5Append (pad, (U16)((saved < 56) ? (56 - saved) : (120 - saved)));

    // Append the length to finish out the final block
    Md5Append (lenBits, 8);

	// copy final registers (little endian!) to digest
	FixEndian (4, digest, (U8*)md5State.abcd);
	} // Md5Finish

// end Md5.c

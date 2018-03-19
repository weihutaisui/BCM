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

#if !defined(Md5_h)
#define Md5_h
////////////////////////////////////////////////////////////////////////////////
/// \file Md5.h
/// \brief MD5 message digest algorithm
///
/// This file implements the MD5 one-way hash as per RFC1321.  
/// The implementation is limited to messages of at most 512 MBytes
/// (2^32 bits) and is not reentrant.
///
////////////////////////////////////////////////////////////////////////////////

#include "Teknovus.h"

#if defined(__cplusplus)
extern "C" {
#endif


#define Md5HashSize		16


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
extern
void Md5Start (void);



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
extern
void Md5Append (const U8* buf, U16 size);



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
extern
void Md5Finish (U8* digest);




#if defined(__cplusplus)
}
#endif

#endif // Md5.h



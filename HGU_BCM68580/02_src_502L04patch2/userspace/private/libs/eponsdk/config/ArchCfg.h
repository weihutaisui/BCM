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



#if !defined(ArchCfg_h)
#define ArchCfg_h

#if defined(__cplusplus)
extern "C" {
#endif

/// define as needed for specific hardware platform, again,
/// sometimes on the compiler command line rather than in this file

//OS_WIN32 OS_VXWORKS OS_LINUX

#define OS_LINUX

/////////////////////

////////////////////
#define OAM_SWAP64(val) \
	((U64)((((U64)(val) & 0x00000000000000ffULL) << 56) | \
	          (((U64)(val) & 0x000000000000ff00ULL) << 40) | \
	          (((U64)(val) & 0x0000000000ff0000ULL) << 24) | \
	          (((U64)(val) & 0x00000000ff000000ULL) <<  8) | \
	          (((U64)(val) & 0x000000ff00000000ULL) >>  8) | \
	          (((U64)(val) & 0x0000ff0000000000ULL) >> 24) | \
	          (((U64)(val) & 0x00ff000000000000ULL) >> 40) | \
	          (((U64)(val) & 0xff00000000000000ULL) >> 56)))

#if  __BYTE_ORDER == __BIG_ENDIAN
#define OAM_NTOHLL(x)	((U64)(x))
#define OAM_HTONLL(x)	((U64)(x))
#elif __BYTE_ORDER == __LITTLE_ENDIAN
#define OAM_NTOHLL(x)	OAM_SWAP64(x)
#define OAM_HTONLL(x)	OAM_SWAP64(x)
#else
#error cannot detect endianess
#endif

#define OAM_NTOHS(x)  ntohs(x)	
#define OAM_NTOHL(x)  ntohl(x)

#define OAM_HTONS(x)  htons(x)
#define OAM_HTONL(x)  htonl(x)

#define TkSwap32(x) htonl(x)
#define TkSwap16(x) htons(x)

#define TkMakeU8(x)  (U8)(*(U8 *)(x))
#define TkMakeU16(x) (U16)((*(U8 *)(x)<<8)|*((U8 *)(x)+1))
#define TkMakeU32(x) (U32)((*(U8 *)(x)<<24)|(*((U8 *)(x)+1)<<16)|(*((U8 *)(x)+2)<<8)|(*((U8 *)(x)+3)))

#if defined(__cplusplus)
}
#endif

#endif

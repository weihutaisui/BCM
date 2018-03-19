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

/*
*  \file Utils.h
*  \brief Epon utils
* 
*/
#ifndef Utils_h
#define Utils_h

#if defined(__cplusplus)
extern "C" {
#endif

#include "Teknovus.h"
#include "Oam.h"


/*******************************************************************************
 * BufInfo Stuff
 ******************************************************************************/

typedef struct 
	{
	U8   *start;    // Pointer to the start of the buffer
	U8   *curr;	  // Pointer to the current position in the buffer
	U16  len;	  // Total Length of buffer
//	BOOL (*onBufFullCall)(void *buf, const U8 *from, U16 len);
	} BufInfo;

void InitBufInfo (BufInfo *buf, U16 size, U8 *start);
BOOL BufSkip(BufInfo *buf, U16 len);
U16  BufGetUsed(const BufInfo *buf);
BOOL BufRead(BufInfo *buf, U8 *to, U16 len);
BOOL BufReadU8(BufInfo *buf, U8 *val);
BOOL BufReadU16(BufInfo *buf, U16 *val);
U16  BufGetRemainingSize(const BufInfo *buf);
BOOL BufWrite(BufInfo *buf, const U8 *from, U16 len);
BOOL BufWriteU16(BufInfo *buf, U16 val);

/*******************************************************************************
 * Miscellaneous Stuff
 ******************************************************************************/

inline void Tk2BufU8  (U8 * buf, U8 val);
inline void Tk2BufU16 (U8 * buf, U16 val);
inline void Tk2BufU32 (U8 * buf, U32 val);

void BufDump(char * title, U8 * buf, U16 len);

/*******************************************************************************
 * OAM Handling Routines
 ******************************************************************************/
BOOL AddOamTlv(BufInfo *bufInfo, OamVarBranch branch, U16 leaf, U8 len,
   const U8 *value);

BOOL AddSetRespDesc(BufInfo *bufInfo, OamVarBranch branch, U16 leaf,
   OamVarErrorCode ackVal);

BOOL FormatBranchLeaf(BufInfo *bufInfo, OamVarBranch branch, 
                              OamAttrLeaf leaf);
BOOL OamAddAttrLeaf(BufInfo *bufInfo, U16 leaf);

BOOL GetNextOamVar(BufInfo *pBufInfo, tGenOamVar *pOamVar, U8 *tlvRet);

BOOL GetEventTlv(BufInfo *pBufInfo, OamEventTlv **pOamEventTlv,int *tlvError);

U8   SearchBranchLeaf(void *oamResp, OamVarBranch branch, U16 leaf, tGenOamVar *pOamVar);

void RuleDebug(U8 *buf,U32 len);

void conditonBubbleSort(OamNewRuleCondition* pData,S32 count);

#if defined(__cplusplus)
}
#endif

#endif /* Utils_h */

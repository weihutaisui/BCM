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
* \file TkOamCommon.h
* \brief the TK extension OAM common functions  
*
*/
#ifndef TKOAMCOMMON_H
#define TKOAMCOMMON_H
	
#include "TkPlatform.h"
#include "Utils.h"

#if defined(__cplusplus)
	extern "C" {
#endif

#define VidNotTagFrame       -1
#define TagOamVidMask        0x0FFF        

#define OnuHostIfPhyIfSft	 5
#define OnuHostIfLinkMask	 0x1F

#define OamReservedFlagShift 8
#define OamReservedFlagMask  0xFF00
#define OamFlagMask          0x00FF

#define OamFlagLinkMask      0xF8
#define OamFlagLinkShift     3

#define OamFlagSrcIfMask     0x06
#define OamFlagSrcIfShift    1


#define MIN_BCM_LINK_NUM   1
#define MAX_BCM_LINK_NUM 8
#define BCM_LINK_DEFAULT_NUM 1
#define BCM_LINK_DEFAULT_QUEUE_NUM 0
#define BCM_PORT_DEFAULT_QUEUE_NUM 0
#define MIN_BCM_LINK_Q_NUM   1
#define MAX_BCM_LINK_Q_NUM   8
#define MIN_BCM_Q_SIZE   1
#define MIN_BCM_PORT_NUM   1
#define MAX_BCM_PORT_NUM   4
#define MIN_BCM_PORT_Q_NUM   0
#define MAX_BCM_PORT_Q_NUM   8
#define INTERNAL_MIN_BCM_PORT_Q_NUM   1


#if defined(CPU_ARCH_ARM) || defined(CPU_ARCH_X86)
#pragma pack (1)
#endif

typedef union
{
    U8 portId;
    U8 linkId;
    OamNameQueue queueId;
} PACK OamObjIndex;

typedef struct {
  TkOsMsgQId   qReqId;
  TkOsSem      semId; 
  TkOsThread   ThreadId;
} LinkConfig;

#if defined(CPU_ARCH_ARM) || defined(CPU_ARCH_X86)
/* Restore packing to previous setting */
#pragma pack ()
#endif

typedef void (* OamExtSendProc)(U8 * pDataBuf, U32 dataLen);

typedef void (* OamExtGetMacProc)(MacAddr * pMacAddr);  

/* Teknovus OUI */
extern const IeeeOui TeknovusOui;

/* CTC OUI */
extern const IeeeOui CTCOui;

////////////////////////////////////////////////////////////////////////////////
/// GetSourceForFlag:  Gets the source associated with the provided OAM flag
///
/// Parameters:
/// \param flag	OAM flags
/// 
/// \return 
/// The source (interface + link) associated with the given flag
////////////////////////////////////////////////////////////////////////////////

U8 GetSourceForFlag(U16 flags);

void AttachOamFlag(U8 source, U16 *flags);
    
////////////////////////////////////////////////////////////////////////////////
/// OamEthSend:  Sends an ethernet frame to a specified link
///
// Parameters:
///	\param linkId: the link to send the ethernet/oam frame;
///	\param dstAddr: the mac address of the ethernet destination
/// \param pDataBuf: the buffer pointer to the ethernet frame
/// \param dataLen: valid data lenth of the ethernet frame
/// 
/// \return 
/// STATUS.  ERROR on failure and OK on success.
////////////////////////////////////////////////////////////////////////////////
    
STATUS TkOamEthSend (U8 linkId, MacAddr *dstAddr, U8 *pDataBuf, U32 dataLen);

    

#if defined(__cplusplus)
}
#endif

#endif


/*
* <:copyright-BRCM:2015:proprietary:epon
* 
*    Copyright (c) 2015 Broadcom 
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


#if !defined(CucOam_h)
#define CucOam_h
////////////////////////////////////////////////////////////////////////////////
/// \file CucOam.c
/// \brief China Unicom extended OAM generic module
/// \todo
/// Finish this header.
////////////////////////////////////////////////////////////////////////////////

#include "Teknovus.h"

#if defined(__cplusplus)
extern "C" {
#endif


////////////////////////////////////////////////////////////////////////////////
/// OamCucAttr - OAM TLV Leaf Attribute Identifiers
///
///
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    OamCucAttrOnuMacAgingTime   = 0x000E,
    OamCucAttrOnuCfgCnt         = 0x000F,
    OamCucAttrOnuMacLimit       = 0x001C,
    OamCucAttrOnuPortModeConfig = 0x001D,
    OamCucAttrOnuPortModeStatus = 0x001E,
    OamCucAttrOnuCap4           = 0x00A2,
    OamCucAttrOnuProtParam      = 0x00A6,
    OamCucAttrU16               = 0x7FFF
    } PACK OamCucAttr;

typedef enum
{
    OamCucPhy10MHalf            = 1,
    OamCucPhy10MFull            = 2,
    OamCucPhy100MHalf           = 3,
    OamCucPhy100MFull           = 4,
    OamCucPhy1G                 = 5,
} PACK OamCucPhyType;


#if defined(__cplusplus)
}
#endif
#endif // End of File CucOam.h

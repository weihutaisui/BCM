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


#if !defined(Mpcp_h)
#define Mpcp_h
////////////////////////////////////////////////////////////////////////////////
/// \file Mpcp.h
/// \brief Definitions for the IEEE 802.3ah Multi-Point Control Protocol
///
/// This header defines structures and constants useful for handling
/// MPCP messages
///
/// These definitions match draft 1.414 of IEEE 802.3ah
///
////////////////////////////////////////////////////////////////////////////////

#include "Teknovus.h"

#if defined(__cplusplus)
extern "C" {
#endif

/// Global broadcast physical LLID value
#define MpcpBroadcastLlid      0x7fff
#define MpcpBroadcastLlid1G    0x7fff
#define MpcpBroadcastLlid10G   0x7ffe
#define MpcpLlidSleBit         0x8000

// Hardware index values for broadcast.  Note that these can only be used in
// specific instances.
#define MpcpBroadcastHwIdx1G   (MpcpBroadcastLlid1G & 0xff)
#define MpcpBroadcastHwIdx10G  (MpcpBroadcastLlid10G & 0xff)

#define MpcpEthertype 0x8808


/// All MPCP messages have a 32-bit timestamp field, which
/// counts time in units of 16 bit times.
typedef U32 MpcpTimestamp;

/// MPCP commonly measures time intervals with a 16-bit width,
/// again measured in units of 16 bit times
typedef U16 MpcpInterval16;



#define MpcpGateInfoCapabilityMsk (MpcpGateInfo1GCapable|MpcpGateInfo10GCapable)
#define MpcpGateInfoCapabilitySft 0
#define MpcpGateInfoWindowMsk     (MpcpGateInfo1GWindow|MpcpGateInfo10GWindow)
#define MpcpGateInfoWindowSft     4



#define MpcpRegReqFlagRegister     1
#define MpcpRegReqFlagDeregister   3

#define MpcpMaxAllowedLaserOn     32
#define MpcpMaxAllowedLaserOff    32


////////////////////////////////////////////////////////////////////////////////
// Register
////////////////////////////////////////////////////////////////////////////////
#define MpcpRegFlagReregister       1
#define MpcpRegFlagDeallocate       2
#define MpcpRegFlagSuccess          3
#define MpcpRegFlagNack             4

#define MpcpRegAckFlagsSuccess      0x01
#define MpcpRegAckFlagsFailure      0x00


typedef U16 MpcpPhyLlid;

#if defined(__cplusplus)
}
#endif

#endif // Mpcp.h

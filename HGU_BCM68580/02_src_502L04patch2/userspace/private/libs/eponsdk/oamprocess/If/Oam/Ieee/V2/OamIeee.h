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


////////////////////////////////////////////////////////////////////////////////
/// \file   OamIeee.h
/// \brief  IEEE OAM for 10G
///
///
////////////////////////////////////////////////////////////////////////////////

#if !defined(OamIeee_h)
#define OamIeee_h


#if defined(__cplusplus)
extern "C" {
#endif

#include "Teknovus.h"
#include "Oam.h"
#include "Ethernet.h"
#include "OamUtil.h"
#include "OamIeeeCommon.h"
#include "OntmTimer.h"

typedef struct
{
    U8      master;
    U8      needToStartEncrypt;
    MacAddr masterMacAddr;
    OamId   oamId;
} PACK OamIeeeCfg;

extern const MacAddr OamMcMacAddr;


////////////////////////////////////////////////////////////////////////////////
/// SetOamRate:  Set the OamRate
///
 // Parameters:
///
///
/// \return
///
////////////////////////////////////////////////////////////////////////////////
extern
void SetOamRate (const OamUpOamRate BULK *ptr);


////////////////////////////////////////////////////////////////////////////////
/// GetOamRate:  Get the OamRate
///
 // Parameters:
///
///
/// \return
///
////////////////////////////////////////////////////////////////////////////////
extern
U8 GetOamRate (OamUpOamRate BULK *ptr);


////////////////////////////////////////////////////////////////////////////////
/// OamSetLoopbackState:  sets loopback state of link
///
 // Parameters:
/// \param link     Link to set into loopback
/// \param action   New loopback state for link
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OamSetLoopbackState (LinkIndex link, OamInfoStateAction action);


////////////////////////////////////////////////////////////////////////////////
/// OamGetLoopbackState:  Gets loopback state of link
///
 // Parameters:
/// \param link   Link to get loopback info
///
/// \return
/// Loopback state
////////////////////////////////////////////////////////////////////////////////
extern
OamInfoStateAction OamGetLoopbackState (LinkIndex link);


////////////////////////////////////////////////////////////////////////////////
/// OamGetOamDiscoveryState:  Gets Oam discovery state of link
///
 // Parameters:
/// \param link   Link to getOam discovery info
///
/// \return
/// Oam discovery state
////////////////////////////////////////////////////////////////////////////////
extern
OamDiscoveryState OamGetOamDiscoveryState (LinkIndex link);


////////////////////////////////////////////////////////////////////////////////
/// OamIeeeUpVlanStripSet: Set the control variable stripUpStreamVlan.
///
/// \param strip the value of OamIeeeUpVlanStripSet.
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OamIeeeUpVlanStripSet(BOOL strip);


////////////////////////////////////////////////////////////////////////////////
/// OamIeeeUpVlanStripGet: Get the control variable stripUpStreamVlan.
///
/// \param None
///
/// \return
/// The value of stripUpStreamVlan
////////////////////////////////////////////////////////////////////////////////
extern
BOOL OamIeeeUpVlanStripGet(void);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Get an Oam Id based on what is configured
///
/// \return Pointer to OAM ID
////////////////////////////////////////////////////////////////////////////////
extern
OamId BULK* OamIeeeGetOamId(void);

extern 
BOOL OamSendNotificationToCms(LinkIndex link, BOOL in_service);

////////////////////////////////////////////////////////////////////////////////
/// OamLinkReset:  Resets link in case of link fault, etc
///
/// For simpler OAM failures where you try to resync.  For serious faults
/// tearing down the whole link, you probably want OamLinkInit () instead.
///
 // Parameters:
/// \param link     link to reset
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OamLinkReset (LinkIndex link);



////////////////////////////////////////////////////////////////////////////////
/// OamLinkStart:  Start OAM at the link
///
///
///
// Parameters:
/// \param link     link to start
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OamLinkStart (LinkIndex link);
////////////////////////////////////////////////////////////////////////////////
/// OamLinkInit:  initialize a link (cold)
///
/// As if the link had never been up -- the initial Passive Wait
///
 // Parameters:
/// \param link     link to init
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OamLinkInit (LinkIndex link);


////////////////////////////////////////////////////////////////////////////////
/// OamInit:  Initialization of OAM module
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OamIeeeInit (void);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Process OAM msgs
///
/// \param link     Link on which message arrived
///
/// \pre    Global frame parser must point the to flags field of the OAM frame
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void OamProcess (LinkIndex link, U8 *pCurPos);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Write any vendor specific info TLVs to the curent frame
///
/// \param link     Index of link frame is destined to
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void OamInfoVendorGet(LinkIndex link);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Send size bytes to given destination
///
/// Assumes pktBuffer has already been formatted with packet to send.  This
/// routine will assure minimum Ethernet size (64 bytes) and pad with
/// zeros.
///
/// \param link     Index of link on which to transmit packet
/// \param size     Number of bytes in packet (excluding DA/SA)
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OamSendToLink (LinkIndex link, U16 size);


////////////////////////////////////////////////////////////////////////////////
/// OamHandleTimer:  Process OAM timer expiry
///
 // Parameters:
/// \param timerId  ID of timer that expired
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OamHandleTimer (OntmTimerId timerId);


#if defined(__cplusplus)
}
#endif

#endif // end OamIeee.h

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

#if !defined(CtcEncryption_h)
#define CtcEncryption_h
////////////////////////////////////////////////////////////////////////////////
/// \file CtcEncryption.h
/// \brief China Telecom encryption module
///
////////////////////////////////////////////////////////////////////////////////
#include "Teknovus.h"
#include "CtcOam.h"

////////////////////////////////////////////////////////////////////////////////
/// OamCtcProcess100MsTimer -  Handle 100ms timer events
///
/// The China Telecom encryption module triggers a few events of of the standard
/// 100ms timer.  If the link is waiting for a new key to be excepted by the OLT
/// the firmware will check the hardware for the arival of encrypted frames with
/// the new key.  If such a frame is found a "Key Change Detected" event is
/// triggered.  If the link times out while waiting for a key change detect a
/// "Key Switch Timer Expire" event is triggered.  If the link is in the idle
/// state and receives no notification from the OLT to switch keys a "Key
/// Progress Timer Expire Event" is triggered.  Events are immediatly processed.
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
extern
void OamCtcProcess100MsTimer (void);



////////////////////////////////////////////////////////////////////////////////
/// OamCtcHandleChurning - Handle churing OAMPDU
///
/// This function handles incoming China Telecom churing OAMPDU messages.  The
/// only type that is handled by the ONU is the key request message.  This will
/// trigger a key change received event for the given link.
///
/// \param link Index of link
/// \param pdu  Pointer to incoming OAMPDU
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
extern
void OamCtcHandleChurning (LinkIndex link, const OamCtcChurningPdu BULK *pdu);



////////////////////////////////////////////////////////////////////////////////
/// OamCtcEncryptionInit - Initialize the encryption module
///
/// This function initializes the China Telecom encryption module.  By default
/// all links are brought up with encryption disabled in the idle state not
/// generating progress timer events.  The progress timer threshold is set to 15
/// seconds, the key switch timer threshold is set to 200 milliseconds, and the
/// retry threshold is set to 3.
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
extern
void OamCtcEncryptionInit (void);



////////////////////////////////////////////////////////////////////////////////
/// OamCtcEncryptResetLink - Reset encryption setting on a link
///
/// This function should be called when a link is deregistered.  It will reset
/// the encryption status on the given link.
///
 // Parameters:
/// \param link Logical link ID
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
extern
void OamCtcEncryptResetLink (LinkIndex link);

#endif

// End of File CtcDiscovery.h


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
/// \file OamOnu.h
/// \brief Main interface to the ONU OAM module
///
////////////////////////////////////////////////////////////////////////////////


#if !defined(OamOnu_h)
#define OamOnu_h


#include "Teknovus.h"
#include "Oui.h"
#include "PonMgrEncrypt.h"
#include "Oam.h"


#if defined(OAM_CLI) && OAM_CLI
////////////////////////////////////////////////////////////////////////////////
/// \brief  Dump an OAM frame
///
/// This function will conditionally ignore info frames based on exFrameDump
///
/// \param  frame   Pointer to frame
/// \param  size    Number of bytes in frame
/// \param  link    Link index of frame
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void OamDumpFrame(const EthernetFrame BULK* frame, U16 size, LinkIndex link);
#endif


////////////////////////////////////////////////////////////////////////////////
/// \brief  Send a key exchange OAM to the OLT
///
/// \param link     Link index for key
/// \param keyNum   Key index to exchange
/// \param oam      Type of OAM to send
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OamSendKeyExchange(OuiVendor oam, LinkIndex link, U8 keyNum, BOOL keyGen);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Enable extended loopback
///
/// \param port     Port to loopback
/// \param loop     Loopback action parameters
/// \param reply    Location for action reply
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void ActLoopEnable (TkOnuEthPort port,
                    OamExtLoopback const BULK* loop,
                    OamVarContainer BULK* reply);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Disable extended loopback
///
/// \param port     Port to loopback
/// \param loop     Loopback action parameters
/// \param reply    Location for action reply
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void ActLoopDisable (TkOnuEthPort port,
                     OamExtLoopback const BULK* loop,
                     OamVarContainer BULK* reply);


#if OAM_FULLSUPPORT
////////////////////////////////////////////////////////////////////////////////
/// \brief  Poll the OAM module
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void OamOnuPoll(void);
#endif

////////////////////////////////////////////////////////////////////////////////
/// \brief  Register OAM module FDS groups
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void OamOnuFdsGroupReg(void);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Initialize the OAM module
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void OamOnuInit(void);

////////////////////////////////////////////////////////////////////////////////
extern
BOOL optical_temp(U16 *value);

////////////////////////////////////////////////////////////////////////////////
extern
BOOL optical_vcc(U16 *value);

////////////////////////////////////////////////////////////////////////////////
extern
BOOL optical_bias(U16 *value);


////////////////////////////////////////////////////////////////////////////////
extern
BOOL optical_rxpower(U16 *value);

////////////////////////////////////////////////////////////////////////////////
extern
BOOL optical_txpower(U16 *value);

#endif // OamOnu.h

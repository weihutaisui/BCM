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
/// \file EncryptCommon.c
/// \brief Encryption functionality
///
/// This module manages the encryption configuration.  This file contains
/// shared implementations.  Hardware specific functionality is defined in
/// specific versions.
///
////////////////////////////////////////////////////////////////////////////////


#include <string.h>
#include "Build.h"
#include "Teknovus.h"
#include "PonMgrEncrypt.h"
#include "Oam.h"
#include "OamUtil.h"
#include "OamOnu.h"
#if MKA
#include "Mka.h"
#endif
#include "cms_log.h"

#define KeyExpiryDefault        0   // seconds; 0 is off
#define KeyOamDefault           OuiTeknovus
#define EncryptTimerIntvl       (1000 / TkOsMsPerTick)  // 1 s in ticks

KeyTimerInfo keyTimerInfo[TkOnuNumRxLlids];

////////////////////////////////////////////////////////////////////////////////
/// EncryptSetKeyTimeout:  set key timeout value
///
/// \param link     Link to affect
/// \param seconds  Number of seconds for key timeout
/// \param oam      OAM type to use for key exchange
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void EncryptSetKeyTimeout (LinkIndex link, U16 seconds, OuiVendor oam)
    {
    keyTimerInfo[link].keyExpiryTime = seconds;
    keyTimerInfo[link].oam = oam;

    if (seconds > 0)
        { // generate (nearly) immediate update after change
        keyTimerInfo[link].keyTimer = 1;
        }
    } // EncryptSetKeyTimeout



////////////////////////////////////////////////////////////////////////////////
/// EncryptGetKeyTimeout:  get key timeout value
///
/// Parameters:
/// \param link     Link to query
///
/// \return
/// Number of seconds between key refresh
////////////////////////////////////////////////////////////////////////////////
//extern
U16 EncryptGetKeyTimeout (LinkIndex link)
    {
    return keyTimerInfo[link].keyExpiryTime;
    } // EncryptGetKeyTimeout


////////////////////////////////////////////////////////////////////////////////
/// NextKey:  choose next key number for link
///
/// Parameters:
/// \param link     Link that needs new key
///
/// \return
/// Key number for link
////////////////////////////////////////////////////////////////////////////////
static
U8 NextKey (LinkIndex link)
    {
    return (EncryptKeyInUse(link) == 0) ? 1 : 0;
    } // NextKey



////////////////////////////////////////////////////////////////////////////////
/// EncryptUpdateKey:  Send updated key message for given link
///
/// Parameters:
/// \param link     Link for new key
/// \param keyIdx     current key index in ONU hw
/// \param KeyGen     flag indicates whether generate a new key
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void EncryptUpdateKey (LinkIndex link, U8 keyIdx, Bool keyGen)
    {
    if (EncryptIsLinkEncrypted(link) &&
        (EncryptModeGet(link) != EncryptModeTripleChurn) &&
        !EncryptIsBiDir(link))
        {
        U8 FAST keyCurIdx = (keyGen == TRUE) ? NextKey(link) : keyIdx;

        OamSendKeyExchange(keyTimerInfo[link].oam, link, keyCurIdx, keyGen);

        //for switch over check
        keyTimerInfo[link].keyCurIdx = keyCurIdx;
        keyTimerInfo[link].keyTimer = keyTimerInfo[link].keyExpiryTime;
        //EncryptSwitchOverOp(link);
        }
    } // EncryptUpdateKey

////////////////////////////////////////////////////////////////////////////////
/// \brief  Handle encryption module timer expiry
///
/// \param  timerId     ID of the timer that expired
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
//extern
void EncryptHandleTimer (OntmTimerId timerId)
    {
    switch (timerId & OntmTimerIdMsk)
        {
        case EncryptSecond :
            EncryptPriHandler();
            break;
        
#if MKA
        case EncryptMkaHello:
        case EncryptMkaLifetime:
            MkaHandleTimer(timerId);
            break;
#endif
        default :
            break;
        }
    } // EncryptHandleTimer


void EncryptLinkReset(LinkIndex link)
    {
    EncryptSetKeyTimeout (link, KeyExpiryDefault, OuiTeknovus); // set default timeout
    keyTimerInfo[link].keyState = EncryptKeySend;
    keyTimerInfo[link].keyTimer = 0;
    EncryptLinkSet(link, EncryptModeDisable, EncryptOptNone);

    EncryptKeyInfoReset(link);
    }


////////////////////////////////////////////////////////////////////////////////
/// EncryptInit:  initialize encryption module
///
/// Parameters:
/// None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void EncryptInit (void)
    {
    U8 FAST i;

    for (i = 0; i < TkOnuNumRxLlids; ++i)
        { // set default timeout
        EncryptLinkReset(i);
        }
    OntmTimerCreateRepeating (EncryptTimerIntvl, EncryptSecond);
    } // EncryptInit


// end EncryptCommon.c

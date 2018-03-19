/*
* <:copyright-BRCM:2012:proprietary:epon
* 
*    Copyright (c) 2012 Broadcom 
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
/// \file PonMgrEncrypt.c
/// \brief Version 2 encryption implementation
///
/// Version 2 encryption is used to configure encrpytion hardware for all
/// products based off of the 4701 chip (55030).
///
////////////////////////////////////////////////////////////////////////////////


// generic includes
#include <stdio.h>
#include "bcm_epon_cfg.h"
#include "Stream.h"
#include "Teknovus.h"
#include "OamOnu.h"
#include "TkOamMem.h"

// base includes
#include "PonManager.h"
#include "PonMgrEncrypt.h"
#include "PonConfigDb.h"

#include "Alarms.h"
#include "eponctl_api.h"
#include "cms_log.h"
#include "EponDevInfo.h"

// driver includes
#include "Pbi.h"

typedef struct
    {
    EncryptMode     mode;
    EncryptOptions  opts;
    } PACK EncryptCfg;

EncryptCfg  encryptCfg;
U16 encryptLinks;  // bitmap of encrypted links

KeyTimerKeyInfo keyTimerKeyInfo[TkOnuNumRxLlids];
extern KeyTimerInfo keyTimerInfo[TkOnuNumRxLlids];
static U8 KeyExSwitchOver = 1;
static U8 keyExSwitchOverInfo[TkOnuNumRxLlids];
static MacAddr BULK oltMac; 


#define KeyResendTime            1  // 1s
#define KeyCheckTime           1 // 2s


////////////////////////////////////////////////////////////////////////////////
/// \brief  Is bidirectional encryption enabled on this link?
///
/// \return
/// TRUE if bidirectional encryption is enabled, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
BOOL EncryptIsBiDir(LinkIndex link)
    {
    return TestBitsSet(encryptCfg.opts, EncryptOptBiDir) &&
        TestBitsSet(encryptLinks, 1U << link);
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Is a link encrypted
///
/// \return
/// TRUE is link is encrypted, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
BOOL EncryptIsLinkEncrypted(LinkIndex link)
    {
    return TestBitsSet(encryptLinks, 1U << link);
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Gets the current encryption mode
///
/// \param link     Link to get mode for
///
/// \return
/// encryption mode
////////////////////////////////////////////////////////////////////////////////
EncryptMode EncryptModeGet(LinkIndex link)
    {
    return TestBitsSet(encryptLinks, 1U << link) ?
        encryptCfg.mode : EncryptModeDisable;
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Gets the current encryption options
///
/// \param link     Link to get options for
///
/// \return
/// encryption options
////////////////////////////////////////////////////////////////////////////////
EncryptOptions EncryptOptsGet(LinkIndex link)
    {
    return TestBitsSet(encryptLinks, 1U << link) ?
        encryptCfg.opts : EncryptOptNone;
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Get the Downstream encryption key bytes
///
/// \param none
///
/// \return
/// Downstream encryption key bytes
////////////////////////////////////////////////////////////////////////////////
U8 EncryptDnKeySize(void)
    {
    return (PonCfgDbGetDnRate() != LaserRate10G) ? 1 : 3;
    }
////////////////////////////////////////////////////////////////////////////////
/// \brief  Gets the downstream encryption key number bitmap
///
/// \param None
///
/// \return
/// Downstream encryption key number bitmap
////////////////////////////////////////////////////////////////////////////////
//extern
U32 EncryptDnKeyNumBitmapGet(void)
    {
    if (PonCfgDbGetDnRate() == LaserRate10G)
        {
        // FIXME WGMAI
        return 0; //XifRxEncryptKeyNumBitmapGet();
        }
    else
        {
        return 0; //LifRxEncryptKeyNumBitmapGet();
        }
    } // EncryptDnKeyNumBitmapGet


////////////////////////////////////////////////////////////////////////////////
/// \brief  Gets the downstream encryption enable state bitmap
///
/// \param None
///
/// \return
/// Downstream encryption enable state bitmap
////////////////////////////////////////////////////////////////////////////////
//extern
U32 EncryptDnEnBitmapGet(void)
    {
    if (PonCfgDbGetDnRate() == LaserRate10G)
        {// FIXME WGMAI
        return 0; //XifRxEncryptEnBitmapGet();
        }
    else
        {
        return 0; //LifRxEncryptEnBitmapGet();
        }
    } // EncryptDnEnBitmapGet


////////////////////////////////////////////////////////////////////////////////
/// EncryptKeyInUse - Get the active encryption key
///
/// This function gets the active key for a given link.  It is used to detect a
/// key switch over.
///
/// Parameters:
/// \param link Link to check
///
/// \return
/// Active encryption key index
////////////////////////////////////////////////////////////////////////////////
//extern
U8 EncryptKeyInUse (LinkIndex link)
    {
    U8 keyInUse;
    
    eponStack_CtlGetKeyInuse(link, &keyInUse); 
    return keyInUse;
    } // EncryptKeyInUse


////////////////////////////////////////////////////////////////////////////////
/// GenerateKey:  create new random key
///
/// Parameters:
/// \param key  Buffer to store new key
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void GenerateKey (U32 BULK* key)
    {
    U8 FAST i;

    for( i = 0; i < (AesKeyBytes / sizeof(U32)); i++)
    {
        key[i] = ((U32)PbiRand16() << 16) | PbiRand16();
    }
    } // GenerateKey


////////////////////////////////////////////////////////////////////////////////
/// \brief  Get the SCI field of the 802.1ae security header for a link
///
/// This function gets an AES 802.1ae SCI for a given link. For 802.1ae AES
/// security there has secTag, in the secTag, the SCI field comprise of MAC
/// address and port ID, now, this will be set defaultly to LLID MAC and LLID
/// index
///
/// \param  link    Index of link
/// \param  dir    encryption direction
/// \param  sci     Pointer to buffer to hold SCI
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
//extern
void EncryptSciGet(LinkIndex link, Direction dir, AesSci BULK* sci)
    {
    U16 llid;
    // form sci from MAC and LLID index
    if (dir == Dnstream)
        {
        memcpy(&(sci->mac), &oltMac, sizeof(MacAddr));
        }
    else
        {
        TkOsGetIfMac(link, &sci->mac);
        }

    eponStack_CtlGetLLID(link, &llid);
    sci->port = OAM_HTONS(llid);
    }   // Encrypt8021aeSciSet


////////////////////////////////////////////////////////////////////////////////
/// \brief  Construct a new encryption key
///
/// \param key      Buffer to place the key in
/// \param link     Link that needs new key
/// \param keyNum   Index for the new key
/// \param KeyGen     flag indicates whether generate a new key
///
/// \return
/// Size in bytes of the new key
////////////////////////////////////////////////////////////////////////////////
U8 EncryptMakeNewKey(U8 BULK* frame, LinkIndex link, U8 keyNum, Bool kenGen)
    {
    U32     BULK key[AesKeyBytes / sizeof(U32)];
    U32     BULK sci[sizeof(AesSci) / sizeof(U32)];
    Stream  FAST aes;
    U8      FAST i;

    if (kenGen == TRUE)
        {
        GenerateKey( key);
        for (i = 0; i < (AesKeyBytes / sizeof (U32)); i ++)
            {
            keyTimerKeyInfo[link].aesKeyRec[i] = key[i];
            }
        }
    else
        {
        for (i = 0; i < (AesKeyBytes / sizeof (U32)); i ++)
            {
            key[i] = keyTimerKeyInfo[link].aesKeyRec[i];
            }
        }
	
    EncryptSciGet(link, Dnstream, (AesSci*)sci);

    // copy key & SCI to frame
    StreamInit(&aes, frame);
    for (i = 0; i < (AesKeyBytes / sizeof(U32)); i++)
        {
        StreamWriteU32(&aes, key[i]);
        }

    for (i = 0; i < (sizeof(sci) / sizeof(U32)); i++)
        {
        sci[i] = OAM_NTOHL(sci[i]);
        }

    eponStack_CtlSetKeyData(link, Dnstream, keyNum, 
		keyTimerKeyInfo[link].aesKeyRec, AesKeyBytes, sci);

    return AesKeyBytes;
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Apply new encryption configuration
///
/// \param mode New encryption mode
/// \param opts New encryption options
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void EncryptCfgApply(EncryptMode mode, EncryptOptions opts)
    {
    encryptCfg.mode = mode;
    encryptCfg.opts = opts;
    }


///////////////////////////////////////////////////////////////////////////////
/// \brief  Set encryption configuration on a link
///
/// \param  link    Link to configure encryption
/// \param  mode    Encryption mode to set
/// \param  opts    Encryption options to set
///
/// \return TRUE if configuration was successful
////////////////////////////////////////////////////////////////////////////////
BOOL EncryptLinkSet(LinkIndex link, EncryptMode mode, EncryptOptions opts)
    {
    // update OLT's MAC
    if (RxFrame)
    	{
        memcpy(&oltMac, &(RxFrame->sa), sizeof(MacAddr));
    	}
	
    if (mode == EncryptModeDisable)
        {
        encryptLinks &= ~(1U << link);
        
        if (!encryptLinks)
            {
            EncryptCfgApply(mode, opts);
            }
        }
    else
    	{
        if (((mode != encryptCfg.mode) || (opts != encryptCfg.opts)) &&
            (encryptLinks&(1U << link)))
            {
            return FALSE;
            }
     
        encryptLinks |= (1U << link);
        EncryptCfgApply(mode, opts);
    	}
    
    eponStack_CtlSetKeyMode(link,  mode, opts);

    return TRUE;
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Get the Downstream encryption key bytes
///
/// \param link     Link for packet number
/// \param expiryTime     expiry time for key exchange
///
/// \return
/// Downstream encryption key bytes
////////////////////////////////////////////////////////////////////////////////
void EncryptModeModify(LinkIndex link, U16 expiryTime)
    {
    //workaround for 1/1 with Tk3723.
    //Tk 3723 may use HMI 140 to control encryption.
    //it only send OAM with key expiry time but no mode.
    //Xenu enable encryption agaist a valid expiry time
    if (PonCfgDbGetDnRate() != LaserRate10G)
        {
        EncryptLinkSet(link, 
                            ((expiryTime == 0) ? EncryptModeDisable : EncryptModeAes), 
                            EncryptOptNone);
        }
    }


///////////////////////////////////////////////////////////////////////////////
/// \brief  Switch over operation
///
/// \param link     Link for packet number
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern 
void EncryptSwitchOverOp(LinkIndex link)
    {
    if (KeyExSwitchOver > 0)
        {
        if (keyExSwitchOverInfo[link] > 0)
            {//switch over this time
            keyTimerInfo[link].keyState = EncryptKeyCheck;
            keyTimerInfo[link].keyTimer = KeyCheckTime;
            }
        //no need to switch
        }
    }

////////////////////////////////////////////////////////////////////////////////
/// EncryptKeyExchange:  Update keys for all links in service
///
/// Parameters:
/// \param link     Link for new key
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void EncryptKeyExchange (LinkIndex link)
    {
    if ((keyTimerInfo[link].keyTimer > 0) && (--keyTimerInfo[link].keyTimer == 0))
        {
        EncryptUpdateKey (link,  EncryptKeyInUse(link), TRUE);

        //update switch over information
        keyExSwitchOverInfo[link] = KeyExSwitchOver;
	EncryptSwitchOverOp(link);
        }
    } // EncryptKeyExchange


////////////////////////////////////////////////////////////////////////////////
/// EncryptCheckKeys:  Check keys for all links in service
///
/// Parameters:
/// \param link     Link for new key
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void EncryptCheckKeys (LinkIndex link)
    {
    if (--keyTimerInfo[link].keyTimer == 0)
        {
        keyExSwitchOverInfo[link]--;
        if (EncryptKeyInUse(link) != keyTimerInfo[link].keyCurIdx)
            { // switch didn't happen, maybe OLT didn't receive new key
            if (!AlarmGetCondition (AlmLinkKeyExchange, link))
                {
                AlarmSetCondition (AlmLinkKeyExchange, link);
                }
            //for system limitation, alarm will be sent by next process circle
            //but we resend key before alarm, it's not correct.
            //just start a timer for resending key, then the alarm can go first.
            keyTimerInfo[link].keyState = EncryptKeyResend;
            keyTimerInfo[link].keyTimer = KeyResendTime;
            }
        else
            {
            //the key switch is done now, clear alarm and reset exchange timer
            if (AlarmGetCondition (AlmLinkKeyExchange, link))
                {
                AlarmClearCondition (AlmLinkKeyExchange, link);
                }
            
            keyTimerInfo[link].keyState = EncryptKeySend;
            keyTimerInfo[link].keyTimer = keyTimerInfo[link].keyExpiryTime - KeyCheckTime;
            keyExSwitchOverInfo[link] = KeyExSwitchOver;
            }
        }
    }

void EncryptKeyInfoReset(LinkIndex link)
    {
    memset(&keyTimerKeyInfo[link], 0, sizeof(KeyTimerKeyInfo));
    }

static void EncryptKeyResendThenCheck(LinkIndex link)
    {
    OamSendKeyExchange(keyTimerInfo[link].oam, link,
                    keyTimerInfo[link].keyCurIdx, FALSE);
    keyTimerInfo[link].keyState = EncryptKeyCheck;
    keyTimerInfo[link].keyTimer = KeyCheckTime;
    }
////////////////////////////////////////////////////////////////////////////////
/// EncryptPriHandler:  timer handler for AES encryption key exchange
///
/// Parameters:
/// None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern 
void EncryptPriHandler (void)
    {
    U8 link;

    for (link = 0; link < TkOnuNumTxLlids; ++link)
        {
        if ((keyTimerInfo[link].keyExpiryTime == 0) ||
			(!EncryptIsLinkEncrypted(link)))
            {
            //EncryptLinkCheck(link);
            //keyTimerInfo[link].keyTimer = 0;
            //keyTimerInfo[link].keyState = EncryptKeySend;
            continue;
            }
        
        switch (keyTimerInfo[link].keyState)
            {
            case EncryptKeySend:
                EncryptKeyExchange(link);
                break;
                
            case EncryptKeyCheck:
                EncryptCheckKeys(link);
                break;
                
            case EncryptKeyResend:
                EncryptKeyResendThenCheck(link);
                break;
                
            default:
                break;
            }
        }
    } // UpdateAllKeys
// end PonMgrEncrypt.c

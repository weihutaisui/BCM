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
*//*
* <:copyright-BRCM:2012:proprietary:epon
* 
*    Copyright (c) 2012 Broadcom Corporation
*    All Rights Reserved
* 
*  This program is the proprietary software of Broadcom Corporation and/or its
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
#if !defined(PonMgrEncrypt_h)
#define PonMgrEncrypt_h
////////////////////////////////////////////////////////////////////////////////
/// \file PonMgrEncrypt.h
/// \brief Public encryption API
///
/// This module manages the encryption hardware.  It does not itself contain
/// any crypto algorithms; rather it manages key exchange with the OLT and
/// provisions the security hardware in the chip.
///
////////////////////////////////////////////////////////////////////////////////


#include "OntmTimer.h"
#include "Security.h"
#include "Oui.h"
#include "MultiByte.h"
#include "eponctl_api.h"

#if defined(__cplusplus)
extern "C" {
#endif

// define AES key and SCI for 802.1ae security algorithm
typedef struct
    {
    MacAddr mac;
    U16     port;
    } PACK AesSci;


typedef union
    {
    AesSci  sci;
    U8      byte[8];
    U32     dword[2];
    } PACK AesSciInfo;


typedef struct
    {
    MultiByte128    key;    // AES 128 bytes key
    AesSciInfo      sci;    // the SCI field for 802.1ae AES algorithm
    } PACK AesKeyInfo;


//key exchange state
typedef enum
    {
    EncryptKeyNone = 0, 
    EncryptKeySend, 
    EncryptKeyCheck, 
    EncryptKeyResend, 
    EncryptKeyMax
    } PACK AesEncryptState;


/// key timer control info
typedef struct
    {
    U16         keyExpiryTime;
    U16         keyTimer;
    U8         keyCurIdx;
    AesEncryptState         keyState;
    OuiVendor   oam; // which OAM to use when sending key exchange messages
    } PACK KeyTimerInfo;

typedef struct
    {
    U32  aesKeyRec[4];
    } PACK KeyTimerKeyInfo;

#define AesInitialPacketNumber  1
#define AesPktNumThreshold      0xC0000000UL


////////////////////////////////////////////////////////////////////////////////
/// \brief  Is bidirectional encryption enabled on this link?
///
/// \param link link of interest
///
/// \return
/// TRUE if bidirectional encryption is enabled, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
BOOL EncryptIsBiDir(LinkIndex link);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Gets the current encryption mode
///
/// \param link     Link to get mode for
///
/// \return
/// encryption mode
////////////////////////////////////////////////////////////////////////////////
extern 
EncryptMode EncryptModeGet(LinkIndex link);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Gets the current encryption options
///
/// \param link     Link to get options for
///
/// \return
/// encryption options
////////////////////////////////////////////////////////////////////////////////
extern
EncryptOptions EncryptOptsGet(LinkIndex link);


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
extern
void EncryptSetKeyTimeout (LinkIndex link, U16 seconds, OuiVendor oam);


////////////////////////////////////////////////////////////////////////////////
/// EncryptGetKeyTimeout:  get key timeout value
///
/// \param link     Link to query
///
/// \return
/// Number of seconds between key refresh
////////////////////////////////////////////////////////////////////////////////
extern
U16 EncryptGetKeyTimeout (LinkIndex link);




////////////////////////////////////////////////////////////////////////////////
/// \brief  Is a link encrypted
///
/// \return
/// TRUE is link is encrypted, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
BOOL EncryptIsLinkEncrypted(LinkIndex link);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Get the Downstream encryption key bytes
///
/// \param none
///
/// \return
/// Downstream encryption key bytes
////////////////////////////////////////////////////////////////////////////////
extern 
U8 EncryptDnKeySize(void);

////////////////////////////////////////////////////////////////////////////////
/// \brief  Gets the downstream encryption key number bitmap
///
/// \param None
///
/// \return
/// Downstream encryption key number bitmap
////////////////////////////////////////////////////////////////////////////////
extern
U32 EncryptDnKeyNumBitmapGet(void);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Gets the downstream encryption enable state bitmap
///
/// \param None
///
/// \return
/// Downstream encryption enable state bitmap
////////////////////////////////////////////////////////////////////////////////
extern
U32 EncryptDnEnBitmapGet(void);


////////////////////////////////////////////////////////////////////////////////
/// EncryptKeyInUse - Get the active encryption key
///
/// This function gets the active key for a given link.  It is used to detect a
/// key switch over.
///
/// \param link Link to check
///
/// \return
/// Active encryption key index
////////////////////////////////////////////////////////////////////////////////
extern
U8 EncryptKeyInUse (LinkIndex link);


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
extern
U8 EncryptMakeNewKey(U8 BULK* key, LinkIndex link, U8 keyNum, Bool keyGen);

////////////////////////////////////////////////////////////////////////////////
/// EncryptUpdateKey:  Send updated key message for given link
///
/// \param link     Link for new key
/// \param keyIdx     current key index in ONU hw
/// \param KeyGen     flag indicates whether generate a new key
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void EncryptUpdateKey (LinkIndex link, U8 keyIdx, Bool keyGen);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Handle encryption module timer expiry
///
/// \param  timerId     ID of the timer that expired
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void EncryptHandleTimer (OntmTimerId timerId);


///////////////////////////////////////////////////////////////////////////////
/// \brief  Set encryption configuration on a link
///
/// \param  link    Link to configure encryption
/// \param  mode    Encryption mode to set
/// \param  opts    Encryption options to set
///
/// \return TRUE if configuration was successful
////////////////////////////////////////////////////////////////////////////////
extern 
BOOL EncryptLinkSet(LinkIndex link, EncryptMode mode, EncryptOptions opts);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Get the Downstream encryption key bytes
///
/// \param link     Link for packet number
/// \param expiryTime     expiry time for key exchange
///
 /// \return none
////////////////////////////////////////////////////////////////////////////////
extern 
void EncryptModeModify(LinkIndex link, U16 expiryTime);


///////////////////////////////////////////////////////////////////////////////
/// \brief  Switch over operation
///
/// \param link     Link for packet number
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern 
void EncryptSwitchOverOp(LinkIndex link);


////////////////////////////////////////////////////////////////////////////////
/// EncryptPriHandler:  timer handler for AES encryption key exchange
///
/// Parameters:
/// None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern 
void EncryptPriHandler (void);


////////////////////////////////////////////////////////////////////////////////
/// EncryptKeyInfoReset:  reset the key info
///
/// Parameters:
/// None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void EncryptKeyInfoReset(LinkIndex link);


////////////////////////////////////////////////////////////////////////////////
/// EncryptLinkReset:  initialize encryption module
///
/// None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void EncryptLinkReset(LinkIndex link);


////////////////////////////////////////////////////////////////////////////////
/// EncryptInit:  initialize encryption module
///
/// None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void EncryptInit (void);


#if defined(__cplusplus)
}
#endif


#endif // Encryption.h

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
/// \file CtcEncryption.c
/// \brief China Telecom extended OAM encryption module
/// \author Jason Armstrong
/// \date March 7, 2006
///
///
////////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include "Teknovus.h"
#include "Build.h"
#include "Oam.h"
#include "OamUtil.h"
#include "Alarms.h"
#include "CtcOam.h"
#include "CtcOnuOam.h"
#include "CtcEncryption.h"
#include "PonMgrEncrypt.h"
#include "Pbi.h"
#include "EponDevInfo.h"
#include "OamIeee.h"
#include "cms_log.h"
#include "eponctl_api.h"

#define RANDOM_IGNORE_KEY_REQUEST       0

#define LlidDnSecKeyRamSize         4
#define LlidDnSecOddKeyRamOffset    2


////////////////////////////////////////////////////////////////////////////////
/// The following are the defaults for the three counters that are needed to
/// drive a links encryption state machine.
////////////////////////////////////////////////////////////////////////////////
#define DefaultKeyRetry         0
#define DefaultProgressTimer    150
#define DefaultSwitchTimer      2


////////////////////////////////////////////////////////////////////////////////
/// OamCtcCounter - Generic counter type
///
/// This record is part of a generic counter module.  It can be used to run some
/// sort of counter or timer up to a given threshold.  The counter may be
/// enabled and disabled.
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    U16                     threshold;      //< Counter limit
    U16                     count;          //< Current counter value
    BOOL                    enabled;        //< Enable/disable flag
    } OamCtcCounter;



////////////////////////////////////////////////////////////////////////////////
/// OamCtcEncryptionMode - Encryption scheme
///
/// The following enumeration defines all of the proposed encryption schemes
/// supported by China Telecom ONUs and OLTs.  At the moment only triple
/// churning is supported by the specification.
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    CtcEncryptNone,
    CtcEncryptTripleChurn,
    CtcEncryptChurning,
    CtcEncryptAes128,
    CtcEncryptEponAes128Down,
    CtcEncryptEponAes128Bidir
    } OamCtcEncryptMode;



////////////////////////////////////////////////////////////////////////////////
/// OamCtcEncryptState - Encryption state machine state
///
/// Every link runs a state machine for their encryption status.  Each state
/// machine contains four state.  The stale state is entered when an encryption
/// key has expired.  The machine is in the idle state when the key is valid
/// and no new key requests have been received.  The generate state is entered
/// when a key request is received which puts the machine in the send state
/// where new keys are sent to the OLT.
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    CtcEncryptStale,                        //< Key has expired
    CtcEncryptIdle,                         //< Key is fresh, all is good
    CtcEncryptGenerate,                     //< Ket request received
    CtcEncryptSend,                         //< Waiting for key acknowledge
    CtcEncryptHalt                          //< Terminate state machine
    } OamCtcEncryptState;



////////////////////////////////////////////////////////////////////////////////
/// OamCtcEncryptEvent - Encryption state machine event
///
/// There is a set of inputs that will trigger certain events in a links
/// encryption state machine.  The event generally cause the machine to change
/// states.
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    CtcNoEvent,                             //< Not to much going on
    CtcKeyProgressTimerExpire,              //< Key has expired
    CtcKeySwitchTimerExpire,                //< Key change acknowledge expired
    CtcKeyChangeReceived,                   //< Key request received
    CtcKeyChangeDetected                    //< Key change acknowledged
    } OamCtcEncryptEvent;



////////////////////////////////////////////////////////////////////////////////
/// OamCtcEncryption - Link encryption infomation
///
/// This record contains all of the configurable date (an some constant data
/// just for convience sake) for a logical links encryption status.
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    OamCtcEncryptMode       mode;           //< Current encryption mode
    OamCtcEncryptState      state;          //< Active key exchange state
    OamCtcEncryptEvent      last;           //< Last key exchange event
    U32                     key[CtcNumKeyStages]; //< Last generated key
    U8                      index;          //< In use key index
    U8			lastUpdateIndex;
    U8                      oltRequestIndex;
    OamCtcCounter           progress;       //< Progress key timer
    OamCtcCounter           newKey;         //< New key switch timer
    OamCtcCounter           retry;          //< Key switch retry counter
    } OamCtcEncryption;



////////////////////////////////////////////////////////////////////////////////
/// ctcEncryption - Per link encryption configuration
///
/// All encryption information for every link is contained in this vector.
////////////////////////////////////////////////////////////////////////////////
static
OamCtcEncryption BULK ctcEncryption[TkOnuNumRxLlids];



////////////////////////////////////////////////////////////////////////////////
/// OamCtcNextKey - Get the next available key index
///
/// This function returns the next available key index on a link.  Link key
/// indexes are labeled from 0 to 1.
///
 // Parameters:
/// \param link Logical link ID
///
/// \return
/// Next available key index
////////////////////////////////////////////////////////////////////////////////
static
U8 OamCtcNextKey(LinkIndex link)
    {
    return ctcEncryption[link].oltRequestIndex;
    } // OamCtcNextKey


////////////////////////////////////////////////////////////////////////////////
/// OamCtcStartCounter - Reset and start a counter
///
/// This function resets the count to 0 and enables the counter.  Counters are
/// not self driven.  They must be incremented by another driver function.
///
 // Parameters:
/// \param counter Pointer to counter
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcStartCounter (OamCtcCounter BULK *counter)
    {
    counter->enabled = TRUE;
    counter->count = 0;
    } // OamCtcStartCounter



////////////////////////////////////////////////////////////////////////////////
/// OamCtcStopCounter - Stop a counter
///
/// This function disables a counter.  The current count is not affected.
///
 // Parameters:
/// \param counter Pointer to counter
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcStopCounter (OamCtcCounter BULK *counter)
    {
    counter->enabled = FALSE;
    } // OamCtcStopCounter



////////////////////////////////////////////////////////////////////////////////
/// OamCtcTickCounter - Increment a counter
///
/// This function increments a counter if it is enabled.  It will return TRUE
/// if the counter has overflowed or false if the counter is disabled or under
/// the threshold.
///
 // Parameters:
/// \param counter Pointer to counter
///
/// \return
/// TRUE if counter is enabled and overflowed
////////////////////////////////////////////////////////////////////////////////
static
BOOL OamCtcTickCounter (OamCtcCounter BULK *counter)
    {
    if ((counter->enabled == TRUE) && (counter->threshold != 0))
        {
        counter->count++;
        return counter->count >= counter->threshold;
        }
    return FALSE;
    } // OamCtcTickCounter



////////////////////////////////////////////////////////////////////////////////
/// OamCtcGenerateKey - Generate a new encryption key
///
/// This function loads a new encryption key from the random number generator.
/// The key is save to the link infomation but not loaded into the hardware.
/// See OamCtcLoadKey for more information.
///
 // Parameters:
/// \param link Logical link ID
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcGenerateKey (LinkIndex link)
    {
    U8 FAST i;

    //##########################################################################
    // This code is generating the new 72 bit key.  I'm not sure why the x part
    // p part nomenclature is still around or why it was even there in the first
    // place for that mater.  Here is one of those places where this could be OK
    // for 24 bit or 72.  Sure the message is too long but the OLT could just
    // ignore the extra data.  PMC may complain but if memory serves they only
    // pad frames with 0 when the standard absolutely requires it and sometimes
    // not even then.
    //
    // The IV is only generated once, I'm not sure if that is correct or not.
    //##########################################################################

    // There are some issue that OLT didn't not receive the previous key, 
    // and request new key with same in-use index,
    // now, ONU should not generate a new key but use previous key for current in-use index
    if ((ctcEncryption[link].lastUpdateIndex <= 1) &&
		(ctcEncryption[link].lastUpdateIndex == OamCtcNextKey(link)))
    	{//it happened
    	return;
    	}

    for (i = 0; i < CtcNumKeyStages; ++i)
        {
        ctcEncryption[link].key[i] = (U32) (((U32) PbiRand16 () << 16) |
                                            (U32) PbiRand16 ());
        }
    } // OamCtcGenerateKey



////////////////////////////////////////////////////////////////////////////////
/// OamCtcDnKeyInUse - Return the active encryption key
///
/// This function returns the downstream encryption key that is currently being
/// used on all downstream traffic to a given link.
///
 // Parameters:
/// \param link Logical link ID
///
/// \return
/// Active encryption key
////////////////////////////////////////////////////////////////////////////////
static
U8 OamCtcDnKeyInUse (LinkIndex link)
    {
    U8 key;    
    eponStack_CtlGetKeyInuse(link, &key);
    return key; 
    } // OamCtcDnKeyInUse



////////////////////////////////////////////////////////////////////////////////
/// OamCtcKeyChangeOccured - Detect if the new key is being used
///
/// This fucntion will return TRUE if the hardware has detected that the OLT is
/// using the new key index.  It will only check if the link is in the send
/// state.
///
 // Parameters:
/// \param link Logical link ID
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
static
BOOL OamCtcKeyChangeOccured (LinkIndex link)
    {
    return (ctcEncryption[link].state == CtcEncryptSend) &&
        ((ctcEncryption[link].retry.threshold == 0) ||
         (OamCtcNextKey (link) == OamCtcDnKeyInUse (link)));
    } // OamCtcKeyChangeOccured



////////////////////////////////////////////////////////////////////////////////
/// OamCtcLoadKey - Commit new key to hardware
///
/// This function loads the newly generated key into the hardware.  The key must
/// be added into ever stage of the tripple churing algorithm, manually rolling
/// the bytes.  Once the key has been loaded it is then activated.
///
 // Parameters:
/// \param link Logical link ID
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcLoadKey (LinkIndex link)
    {
    eponStack_CtlSetKeyData(link, Dnstream, OamCtcNextKey (link), 
                            ctcEncryption[link].key, sizeof(U32)*EncryptDnKeySize(), NULL);
    } // OamCtcLoadKey


////////////////////////////////////////////////////////////////////////////////
/// \brief Creates a 1G CTC key from a U32 key
///
/// This function simply takes the 24 LSB of the U32 key
///
/// Parameters:
/// \param outKey The 1G CTC Key to build from the U32 key
/// \param inKey The U32 key
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
static
void Build1GKeyFromU32 (OamCtcEncryptKey1G BULK * outKey, U32 inKey)
    {
    outKey->byte[2] = (U8) (inKey >>  0);
    outKey->byte[1] = (U8) (inKey >>  8);
    outKey->byte[0] = (U8) (inKey >> 16);
    } // Build1GKeyFromU32


////////////////////////////////////////////////////////////////////////////////
/// OamCtcTransmitKey - Send a new churning key to the OLT
///
/// This function sends a newly generated churning key to the OLT.  It is called
/// after the OLT has made a key request and the new key has been generated.
///
/// Patch:
/// In 1/1 Mode, latter 24x2 bits of key should be zero padding, otherwise, OLT 
/// consider it as 10G key.
/// 
 // Parameters:
/// \param link Logical link ID
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcTransmitKey (LinkIndex link)
    {
    OamCtcChurning10GPdu  BULK *pdu;
    U32 BULK pduSize;
    U8 FAST keyNum = EncryptDnKeySize();

    pduSize = (keyNum == CtcNumKeyStages) ? sizeof(OamCtcChurning10GPdu) : sizeof(OamCtcChurningPdu);

    //###########################################################
    // This is the OAM format we have specified.  If CTC doesn't like it then it
    // will need to be changed.  Although if just the structure changes it may
    // be possible to save this one.
    //
    // Below (and some above) is the print message that will dump the CLI
    // commands for the OLT.  This will probably work out well to check the
    // endianess of the OLT and ONU and verify they are in sync.
    //###########################################################
    pdu = (OamCtcChurning10GPdu BULK *)oamParser.reply.cur;
	
    pdu->ext = OamCtcChurning;
    pdu->opcode = ChurningNewKey;
    pdu->keyIndex = OamCtcNextKey(link);

    while ((keyNum--) > 0)
        {
        Build1GKeyFromU32 (&(pdu->key.key[keyNum]), ctcEncryption[link].key[keyNum]);
        }

    StreamSkip(&oamParser.reply, pduSize);

    OamTransmit();
    ctcEncryption[link].lastUpdateIndex = OamCtcNextKey(link);
    } // OamCtcTransmitKey



////////////////////////////////////////////////////////////////////////////////
/// OamCtcRaiseStaleKeyAlarm - Raise alarm when key request times out
///
/// This function is called when the progress timer expires, signaling a failure
/// on the OLT to send a key request.  This causes an alarm to be sent up to the
/// OLT to inform it of its failure.
///
 // Parameters:
/// \param link Logical link ID
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcRaiseStaleKeyAlarm (LinkIndex link)
    {
    UNUSED (link);
    } // OamCtcRaiseStaleKeyAlarm



////////////////////////////////////////////////////////////////////////////////
/// OamCtcSyncActiveKey - Syncronize active key for no retry
///
/// Because of the way Immenstar designed their encryption state machine, that
/// is without key retry.  The state machine must now syncronize itself with the
/// hardware since the state machine will transition to from the send state to
/// the idle state without waiting for key change confirmation.
///
 // Parameters:
/// \param link Logical link ID
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcSyncActiveKey (LinkIndex link)
    {
    ctcEncryption[link].index = OamCtcDnKeyInUse (link);
    } // OamCtcSyncActiveKey



////////////////////////////////////////////////////////////////////////////////
/// OamCtcEncryptExecuteState - Excute state specific commands
///
/// When a state transition occurs on a link a series must be executed for that
/// state.  This function handles the execution of those commands.  This
/// function should be called right after a state transition and never called
/// again until another transition occurs.
///
 // Parameters:
/// \param link Logical link ID
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcEncryptExecuteState (LinkIndex link)
    {
    switch (ctcEncryption[link].state)
        {
        case CtcEncryptStale:
            {
            OamCtcRaiseStaleKeyAlarm (link);
            ctcEncryption[link].state = CtcEncryptIdle;
            // As per the CTC specification, the transition from the stale key
            // state to the idle state is an eventless transition.  The easiest
            // way to pull this off is to fall through here.
            } //lint -fallthrough
        case CtcEncryptIdle:
            {
            OamCtcStopCounter (&ctcEncryption[link].newKey);
            OamCtcStartCounter (&ctcEncryption[link].progress);
            break;
            }
        case CtcEncryptGenerate:
            {
            OamCtcStopCounter (&ctcEncryption[link].progress);
            OamCtcSyncActiveKey (link);
            OamCtcGenerateKey (link);
            OamCtcLoadKey (link);
            OamCtcStartCounter (&ctcEncryption[link].retry);
            ctcEncryption[link].state = CtcEncryptSend;
            // Transitioning from the generate key state to the send key state
            // is another eventless transition.
            }//lint -fallthrough
        case CtcEncryptSend:
            {
            OamCtcTransmitKey (link);
            OamCtcStartCounter (&ctcEncryption[link].newKey);
            break;
            }
        case CtcEncryptHalt:
            {
            OamCtcStopCounter (&ctcEncryption[link].newKey);
            OamCtcStopCounter (&ctcEncryption[link].progress);
            break;
            }
        default:
            {
            break;
            }
        }
    } // OamCtcEncryptExecuteState



////////////////////////////////////////////////////////////////////////////////
/// OamCtcEncryptProcessEvent - Process CTC encryption event
///
/// There are four events that can cause a links encryption state.  A "Key
/// Progress Timer Expire" occurs when the link is in the idle state and the OLT
/// failed to transmit a key request.  A "Key Switch Timer Expire" is triggered
/// when the link is the send state and OLT fails to acknowledge the new key by
/// sending traffic with the new key.  If the retry counter overflows the link
/// is put in the stale state and an alarm is sent to the OLT.  A "Key Change
/// Received" event occurs when the ONU recieves a new key request from the OLT
/// for the given link.  A "Key Change Detected" is the final event in a key
/// change signifying that the OLT has been verified to be using the new key.
/// The link is returned to the idle state.
///
 // Parameters:
/// \param link Logical link ID
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcEncryptProcessEvent (LinkIndex link)
    {
    switch (ctcEncryption[link].last)
        {
        case CtcKeyProgressTimerExpire:
            {
            ctcEncryption[link].state = CtcEncryptStale;
            break;
            }
        case CtcKeySwitchTimerExpire:
            {
            if (OamCtcTickCounter (&ctcEncryption[link].retry))
                {
                ctcEncryption[link].state = CtcEncryptStale;
                }
            else
                {
                ctcEncryption[link].state = CtcEncryptSend;
                }
            break;
            }
        case CtcKeyChangeReceived:
            {
            ctcEncryption[link].state = CtcEncryptGenerate;
            break;
            }
        case CtcKeyChangeDetected:
            {
            ctcEncryption[link].state = CtcEncryptIdle;
            break;
            }
        default:
            {
            break;
            }
        }
    OamCtcEncryptExecuteState (link);
    } // OamCtcEncryptProcessEvent



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
//extern
void OamCtcProcess100MsTimer (void)
    {
    LinkIndex FAST i;
	
    for (i = 0; i < TkOnuNumRxLlids; ++i)
        {
        if (OamCtcKeyChangeOccured (i))
            {
            ctcEncryption[i].last = CtcKeyChangeDetected;
            OamCtcEncryptProcessEvent (i);
            }
        else if (OamCtcTickCounter(&ctcEncryption[i].progress))
            {
            ctcEncryption[i].last = CtcKeyProgressTimerExpire;
            OamCtcEncryptProcessEvent (i);
            }
        else if (OamCtcTickCounter(&ctcEncryption[i].newKey))
            {
            ctcEncryption[i].last = CtcKeySwitchTimerExpire;
            OamCtcEncryptProcessEvent (i);
            }
        }
    } // OamCtcProcess100MsTimer


////////////////////////////////////////////////////////////////////////////////
//extern
void OamCtcHandleChurning (LinkIndex link, const OamCtcChurningPdu BULK *pdu)
    {
    cmsLog_notice("opcode=0x%x", pdu->opcode); 

    // We have received a CTC churning message of some flavor which means the
    // OLT is CTC compatible and wishes to operate in triple churning mode.  The
    // hardware needs to be enabled.
    ctcEncryption[link].mode = CtcEncryptTripleChurn;
    EncryptLinkSet(link, EncryptModeTripleChurn, EncryptOptNone);

    switch (pdu->opcode)
        {
        case ChurningKeyRequest:
            ctcEncryption[link].last = CtcKeyChangeReceived;
            ctcEncryption[link].oltRequestIndex = (pdu->keyIndex == 1)?0:1;
            OamCtcEncryptProcessEvent (link);
            break;
        case ChurningNewKey:
            // The ONU should only transmit churning PDUs of this type.  Some
            // error handling should be added here because receiving a new key
            // is evidence of something very bad happening.
        default:
            break;
        }
    } // OamCtcHandleChurning


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
//extern
void OamCtcEncryptResetLink (LinkIndex link)
    {
    if (link < TkOnuNumRxLlids)
        {
        ctcEncryption[link].index = 0;
        ctcEncryption[link].lastUpdateIndex = 2; //invalid index
        ctcEncryption[link].state = CtcEncryptHalt;
        ctcEncryption[link].mode = CtcEncryptNone;
        ctcEncryption[link].progress.threshold = DefaultProgressTimer;
        ctcEncryption[link].newKey.threshold = DefaultSwitchTimer;
        ctcEncryption[link].retry.threshold = DefaultKeyRetry;
        OamCtcEncryptExecuteState (link);
        }
    } // OamCtcEncryptResetLink



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
//extern
void OamCtcEncryptionInit (void)
    {
    LinkIndex FAST link;

    for (link = 0; link < TkOnuNumRxLlids; ++link)
        {
        OamCtcEncryptResetLink (link);
        }
    } // OamCtcEncryptionInit


// End of File CtcEncryption.c

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
:>
*/
////////////////////////////////////////////////////////////////////////////////
/// \file OamIeee.c
/// \brief Handle OAM Ieee messages
///
///
////////////////////////////////////////////////////////////////////////////////
#define _FILE_ OamIeee_c

#include <stdio.h>
#include <string.h>     // memcpy
#include "Build.h"
#include "Teknovus.h"
#include "Oam.h"
#include "OamIeee.h"
#include "Oui.h"
#include "bcm_epon_cfg.h"
#include "Ethernet.h"
#include "TkOamMem.h"
#include "OamGet.h"
#include "OsAstMsgQ.h"
#if OAM_TEKNOVUS
#include "OamSys.h"
#endif

#include "EponDevInfo.h"
#include "TkDebug.h"
#if CLOCK_TRANSPORT
#include "ClockTransport.h"
#endif
#include "OamEpon.h"
#include "OamTekEvent.h"
#include "OamOnu.h"
#include "OamTekNeg.h"
#include "OntConfigDb.h"
#include "OamProcessInit.h"

// internal
#include "OamIeeeInt.h"
#include "PonManager.h"

#include "cms_log.h"
#include "eponctl_api.h"
#include "EthFrameUtil.h"
#ifdef BRCM_CMS_BUILD
#include "cms_msg.h"
#endif

// OAM timer constants
#define   OamTicksPerSec                10

#define   OamMaxCreditsPerTick          25
#define   OamDefaultCreditsPerTick       3
#define   OamDefaultTicksPerMinRate     10
#ifndef DESKTOP_LINUX
#define   OamAllowedTicksUntilLinkFail  (10*OamDefaultTicksPerMinRate)
#else
#define   OamAllowedTicksUntilLinkFail  OamDefaultTicksPerMinRate+2  // for test
#endif
#define   OamInfoIntInterval            12500000UL  // 100ms

BOOL hbIntInstall = FALSE;

extern void *msgHandle;

static U8 FAST oamCreditsPerTick;
static U8 FAST oamTicksPerMinRate;

OamIeeeCfg oamIeeeCfg;

const MacAddr CODE OamMcMacAddr =
    { {0x01, 0x80, 0xc2, 0x00, 0x00, 0x02} };/* lint !e708  union initialization */


// This variable is used for indicating whether ont needs to strip the upstream
// traffic's Vlan or not. It is a global control variable, so it is valid for
// all UNI port. FALSE: Not strip; TURE : Strip. the default value is FALSE.
static BOOL stripUpStreamVlan = FALSE;

extern OamHandlerCB *oamVenderHandlerHead;
void DumpOamMessage(LinkIndex link, OamOpcode opcode, U8 *Frame, U16 size, Direction dir);

#if HARDCODED_PERSONALITY
////////////////////////////////////////////////////////////////////////////////
/// OamIeeeCreateDefaultPersonality:  initialize LIF NVS record (debug)
///
// Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void OamIeeeCreateDefaultPersonality (void)
{
    // Don't need to initialize, it should have been done already
    oamIeeeCfg.master = 0;
    memset (&oamIeeeCfg.masterMacAddr, 0x00, sizeof(oamIeeeCfg.masterMacAddr));
    oamIeeeCfg.needToStartEncrypt = 0;
    memcpy (&oamIeeeCfg.oamId.vendor, &TeknovusOui, sizeof(TeknovusOui));
    oamIeeeCfg.oamId.product = OAM_HTONS(0x6838);
    oamIeeeCfg.oamId.version = OAM_HTONS(0x0001);
} // OamIeeeCreateDefaultPersonality
#endif


////////////////////////////////////////////////////////////////////////////////
/// SetOamRate:  Set the OamRate
///
// Parameters:
///
///
/// \return
///
////////////////////////////////////////////////////////////////////////////////
//extern
void SetOamRate (const OamUpOamRate *ptr)
{
    if (ptr->maxRate < (OamMaxCreditsPerTick+1))
    {
        oamCreditsPerTick = ptr->maxRate;
        oamTicksPerMinRate = ptr->minRate;
    }
} //SetOamRate


////////////////////////////////////////////////////////////////////////////////
/// GetOamRate:  Get the OamRate
///
// Parameters:
///
///
/// \return
///
////////////////////////////////////////////////////////////////////////////////
//extern
U8 GetOamRate (OamUpOamRate *ptr)
    {
    ptr->maxRate = oamCreditsPerTick;
    ptr->minRate = oamTicksPerMinRate;
    return sizeof(OamUpOamRate);
    } // GetOamRate


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
//extern
void OamSetLoopbackState (LinkIndex link, OamInfoStateAction action)
    {
    linkOamData[link].loopbackState = action;
    } // OamSetLoopbackState


////////////////////////////////////////////////////////////////////////////////
/// OamGetLoopbackState:  Gets loopback state of link
///
// Parameters:
/// \param link   Link to get loopback info
///
/// \return
/// Loopback state
////////////////////////////////////////////////////////////////////////////////
//extern
OamInfoStateAction OamGetLoopbackState (LinkIndex link)
    {
    return linkOamData[link].loopbackState;
    } // OamGetLoopbackState


////////////////////////////////////////////////////////////////////////////////
/// OamGetOamDiscoveryState:  Gets Oam discovery state of link
///
// Parameters:
/// \param link   Link to getOam discovery info
///
/// \return
/// Oam discovery state
////////////////////////////////////////////////////////////////////////////////
//extern
OamDiscoveryState OamGetOamDiscoveryState (LinkIndex link)
{
    return linkOamData[link].discState;
} // OamGetOamDiscoveryState


////////////////////////////////////////////////////////////////////////////////
/// OamIeeeUpVlanStripSet: Set the control variable stripUpStreamVlan.
///
/// \param strip the value of OamIeeeUpVlanStripSet.
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamIeeeUpVlanStripSet(BOOL strip)
{
    stripUpStreamVlan = strip;
} // OamIeeeUpVlanStripSet


////////////////////////////////////////////////////////////////////////////////
/// OamIeeeUpVlanStripGet: Get the control variable stripUpStreamVlan.
///
/// \param None
///
/// \return
/// The value of stripUpStreamVlan
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL OamIeeeUpVlanStripGet(void)
    {
    return stripUpStreamVlan;
    } // OamIeeeUpVlanStripGet


////////////////////////////////////////////////////////////////////////////////
/// OamIeeeHeartClear:  Clear Oam Hear beat counter.
///
/// \param link     Link on which counter reset
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void OamIeeeHeartClear (LinkIndex link)
    {
    linkOamData[link].ticksUntilLinkFail = OamAllowedTicksUntilLinkFail;
    } // OamIeeeHeartClear


///////////////////////////////////////////////////////////////////////////////
//extern
OamId BULK* OamIeeeGetOamId(void)
    {
    return &oamIeeeCfg.oamId;
    } // OamIeeeGetOamId


////////////////////////////////////////////////////////////////////////////////
/// OamPutLinkInService:  Puts link in service
///
// Parameters:
/// \param link     Link that has completed discovery
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void OamPutLinkInService (LinkIndex link, OamDiscoveryState state)
    {
    cmsLog_notice("OAM: L:%x in service - OAM %x\n", link, state);

    linkOamData[link].minRate = (state == OamDiscNoOam) ?
                                0 :  // don't send heartbeat at all if no OAM
                                oamTicksPerMinRate;   // in service; use provisioned rate
    
    linkOamData[link].discState = state;
    linkOamData[link].loopbackState = OamInfoStateActionForward;

    linkOamData[link].txCredits = oamCreditsPerTick * OamTicksPerSec;

    EncryptUpdateKey (link, 0, FALSE);    // Set up new crypto key

    } // OamPutLinkInService





////////////////////////////////////////////////////////////////////////////////
/// OamInfoNegotiate:  checks remote info, updates local info
///
/// Decides whether or not the OAM configuration is happy
///
// Parameters:
/// \param remote   OAM Info TLV from remote system
///
/// \return
/// TRUE if we are satisfied with the results; FALSE if we need to keep going
////////////////////////////////////////////////////////////////////////////////
static
BOOL OamInfoNegotiate (LinkIndex link, OamInfoTlv const BULK* remote)
    {
    BOOL    FAST satisfied = FALSE;
    U16     FAST pduSize = OAM_NTOHS(remote->pduConfiguration) & OamInfoPduConfigSizeMsk;

    // negotiate max PDU size
    if (pduSize <= linkOamData[link].maxPduSize)
        {
        linkOamData[link].maxPduSize = pduSize;
        cmsLog_debug("OAM: L:%x reduces PDU size to %u\n", link, pduSize);
        satisfied = TRUE;
        }
    return satisfied;
    } // OamInfoNegotiate


////////////////////////////////////////////////////////////////////////////////
/// OamInfoGetStateFlags:  figures out state flags for the given link
///
// Parameters:
/// \param link     Link to query
///
/// \return
/// OAM info flags for this link
////////////////////////////////////////////////////////////////////////////////
static
U8 OamInfoGetStateFlags (LinkIndex link)
    {
    U8 FAST flags = 0;

    // multiplexer action
    if ((linkOamData[link].loopbackState != OamInfoStateActionForward) ||
        (linkOamData[link].discState <= OamDiscSatisfied))
        {
        flags = OamInfoStateMuxDiscard;
        }

    // current forwarding action
    flags |= linkOamData[link].loopbackState << OamInfoStateActionSft;

#if OAM_VERSION < 200
    // stable/unstable
    if (linkOamData[link].discState >= OamDiscSatisfied)
        {
        flags |= OamInfoStateFlagStable;
        }
#elif OAM_VERSION <= 320
    // state flags unused for initial negotation
#else
#error "Unsupported OAM OAM_VERSION"
#endif

    return flags;
    } // OamInfoGetStateFlags


////////////////////////////////////////////////////////////////////////////////
/// \brief  Write the local & remote info TLVs to the curent frame
///
/// \param  link    Index of link frame is destined to
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamInfoLocalRemoteGet(LinkIndex link)
    {
    OamInfoTlv BULK* FAST info = (OamInfoTlv BULK*)oamParser.reply.cur;

    info->type = OamTlvLocalInfo;
    info->length = sizeof(OamInfoTlv);

    info->state.flags = OamInfoGetStateFlags (link);

    info->version = OamInfoVersion;

    info->pduConfiguration = OAM_NTOHS(linkOamData[link].maxPduSize);

    memcpy(&info->id, OamIeeeGetOamId(), sizeof(OamId));

#if OAM_VERSION < 300
    info->configuration = OamInfoConfigLoopCapable;
#elif OAM_VERSION < 330
    info->configuration = OamInfoConfigLoopCapable |
                          OamInfoConfigVarResp;

    if(OamTlvNull == linkOamData[link].lastLocalInfo.type)
        {
        /*revision should start from 0*/
        info->revision = 0;
        }
    else
        {
        U16 lastRevision = linkOamData[link].lastLocalInfo.revision;
        info->revision = lastRevision;
        /*Every time local TLV changes, revision should plus 1.*/
        if(0 != memcmp(info, &linkOamData[link].lastLocalInfo,
            sizeof(OamInfoTlv)))
            {
            lastRevision = OAM_NTOHS(lastRevision) + 1;
            info->revision = OAM_NTOHS(lastRevision);
            }
        }

    linkOamData[link].lastLocalInfo = *info;
#else
#error Unknown OAM version OAM_VERSION
#endif

    StreamSkip(&oamParser.reply, sizeof(OamInfoTlv));

    // move to second TLV - remote info
    if (linkOamData[link].lastRemoteInfo.length > 0)
        { // we've seen info TLV from remote OAM peer
        info++;
        memcpy (info, &linkOamData[link].lastRemoteInfo, sizeof(OamInfoTlv));
        info->type = OamTlvRemoteInfo;
        StreamSkip(&oamParser.reply, sizeof(OamInfoTlv));
        }
    }


////////////////////////////////////////////////////////////////////////////////
//extern
void OamInfoVendorGet(LinkIndex link)
    {
    U8 len = 0;
    
    if (linkOamData[link].discState == OamDiscComplete)
        {
#ifdef CTC_OAM_BUILD
		VENDOR_OAM_INFO_TLV_GET(OuiCtc, link, oamParser.reply.cur, &len);
		StreamSkip(&oamParser.reply, len);
#endif

        VENDOR_OAM_INFO_TLV_GET(OuiTeknovus, link, oamParser.reply.cur, &len);        
        StreamSkip(&oamParser.reply, len);        
//        StreamSkip(&oamParser.reply,
//                   OamGetTeknovusInfoTlv(link, oamParser.reply.cur));
        }
    else
        {
        // DPoE TLV SHOULD NOT be included after discovery completes
        VENDOR_OAM_INFO_TLV_GET(OuiDpoe, link, oamParser.reply.cur, &len);
        StreamSkip(&oamParser.reply, len);
        }
    }


////////////////////////////////////////////////////////////////////////////////
/// OamSendInfo:  Send info frame
///
// Parameters:
/// \param link     Link on which to send frame
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void OamSendInfo (LinkIndex link)
{
    StreamWriteU8(&oamParser.reply, OamOpInfo);
    OamInfoLocalRemoteGet(link);
    OamInfoVendorGet(link);
    // Pad to indicate termination
    StreamSkip(&oamParser.reply,
               OamTerminateBranch((OamVarContainer BULK *)oamParser.reply.cur));
    // send the message; one TLV unless we've heard from the other side
    // otherwise two TLVs
    OamTransmit();
} // OamSendInfo


////////////////////////////////////////////////////////////////////////////////
/// \brief  Handles OAM info packet
///
/// \param  link    Link on which frame was received
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamHandleInfo (LinkIndex link)
    {
    OamInfoTlv BULK *    FAST tlv;
    OamInfoTlv BULK *    FAST oltInfo = NULL;
    OamDiscoveryState    FAST state = linkOamData[link].discState;
    BOOL BULK isRegInfo = FALSE;
    U8 BULK*  RxFrameEnd = (U8 BULK*)RxFrame + rxFrameLength;
    
    // If we are not ready to handle OAM, just ignore the INFO
    if ((state == OamDiscNoOam) || (oamParser.dest.pift != PortIfPon))
        {
        return;
        }
    
    linkOamData[link].informationRx++;   
    
    /* check if missing OAM header filling */
    StreamWriteU8(&oamParser.reply, OamOpInfo);

    // scan TLVs in Info frame
    tlv = (OamInfoTlv BULK*)oamParser.src.cur;

    while (tlv->type != OamTlvNull)
        {
        if((U8 BULK*)tlv + offsetof(OamInfoTlv, length) >= RxFrameEnd)
            {
            cmsLog_error("malformed OAM frame detected\n");
            return;
            }
        if((tlv->length == 0) || (((U8 BULK*)tlv + tlv->length) > RxFrameEnd))
            {
            cmsLog_error("malformed OAM frame detected\n");
            return;
            }
        switch (tlv->type)
            {
            // Note that what we call "remote" info is their "local" info,
            // and will be labeled as such
            case OamTlvLocalInfo :
                oltInfo = tlv;
                StreamCopy(&oamParser.reply, (U8*)tlv, tlv->length);
                // The local info (local to the OLT that is) may tell us more
                // infomation about the OLT than the organization specific info
                // in some cases.  Those cases being PMC OLTs which after all of
                // these years still haven't figured out how to grant properly.
                PonMgrSyncReportOptions(&(tlv->id.vendor));                
                break;

            case OamTlvRemoteInfo:
                StreamCopy(&oamParser.reply, (U8*)tlv, tlv->length);
                break;

            case OamTlvOrgSpecific :
                {
                U32 OuiVal = U24ToU32(&((OamInfoVendorTlv*)tlv)->oui);
                switch (OuiVal)
                    {
                    case TekOuiVal:
                    case CtcOuiVal:
                    case DpoeOuiVal:
                        VENDOR_OAM_INFO_TLV_HANDLE(oamValToOamVendor(OuiVal), link, (OamInfoTlv BULK*)tlv);
                        break;

                    default:
                        break;
                    }
                }
                break;

            default : // ignore all other TLV types
                break;
            }
        //non-standard info packet reach end of buffer   
        if((((U8 BULK*)tlv + tlv->length) == RxFrameEnd))
            {
            break;
            }
        //add length to current pointer to reach next tlv
        tlv = (OamInfoTlv BULK*)((U8 BULK*)tlv + tlv->length);
        }

    if(isRegInfo)
        {
        OamTransmit();
        }

    if (oltInfo == NULL)
        {
        cmsLog_debug("OAM: No remote info TLV in info frame\n");
        return;
        }
    else
        {
        memcpy (&linkOamData[link].lastRemoteInfo,
                oltInfo, sizeof(OamInfoTlv));
        }

    switch (state)
        {
        case OamDiscPassiveWait :
            // once we hear from an active peer, we're allowed to transmit
            if (OamInfoNegotiate (link, oltInfo))
                {
                state = OamDiscSatisfied;
                }
            else
                {
                state = OamDiscUnsatisfied;
                }
            linkOamData[link].minRate = oamTicksPerMinRate;
            break;

        case OamDiscUnsatisfied:
            if (OamInfoNegotiate (link, oltInfo))
                {
                state = OamDiscSatisfied;
                }
            break;

        case OamDiscSatisfied:
            state = OamDiscUnsatisfied;
            if (OamInfoNegotiate (link, oltInfo))
                {
                // negotation in OAM PDU flags field
                if (TkGetField (OamMsgFlagLocalDisc,
                                linkOamData[link].lastRemoteFlags) ==
                    OamMsgFlagDiscComplete)
                    {
                    state = OamDiscComplete;
                    OamPutLinkInService (link, state);
                    OsAstMsgQSet(OsAstAlmLinkOamDiscComplete, link, 0);
                    }
                }
            break;

        case OamDiscComplete:
            // no purpose but as a keepalive heartbeat message
            break;

        default :
            break;
        }
    
    if (linkOamData[link].discState != state)
        {
        cmsLog_notice("link %x oamDiscState 0x%x", link, state);
        }
    
    linkOamData[link].discState = state;

    // Interleave the INFO frames. This allows us to better load balance
    // the OAM processing.
    // This has also the nice effect of better fir with the CTC discovery
    // process where INFO OAMPDUs are considered to "respond" to each other.
    // This is an incorrect reading of the IEEE spec (where INFO OAMPDUs are
    // independant/asynchronous). Note that CTC spec is not clear about its
    // description of the OAM extended discovery.
    // Note that directly responding to an INFO OAMPDU (i.e. use the reception
    // of an INFO as a trigger to send an INFO) is not acceptable. Even thought
    // it would perfectly comply with IEEE spec (which says each end needs to
    // send OAM frame at least every sec), it would in fact reveal a weakness of
    // the CTC discovery. If the ONU "responds" to the OLT INFO frame, then its
    // timer is always about to expire when it receive the INFO from the OLT.
    // So any kind of delay in the processing (say an external CPU), could make
    // the ONU timer expire before it sent the "response". This INFO frame sent
    // would then be analyzed by OLT as a failure to comply with CTC discovery.
    // And since the CTC discovery does not incorporate any retry, the link
    // would be left as is.
    // So here we tweak the per link counter to make sure that the next INFO
    // frame will not be sent after 1/2 of the normal period (nominally .5sec)
    // Note that we don't change the period itself.
    // This will make the INFO OAMPDU perfectly interleaved, smoothing the
    // processing load at the ONU but more importantly gives the ONU 1/2 period
    // to send and INFO (or "answer") before the timer expires.
    if (linkOamData[link].ticksSinceSend < (linkOamData[link].minRate / 2))
        {
        linkOamData[link].ticksSinceSend = linkOamData[link].minRate / 2;
        }

    } // OamHandleInfo


////////////////////////////////////////////////////////////////////////////////
/// \brief  Handle loopback frame type
///
/// \param link     Link on which frame arrived
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void OamHandleLoopback (LinkIndex link)
    {
    OamLoopCmd loopCmd = (OamLoopCmd)StreamReadU8(&oamParser.src);

    switch (loopCmd)
        {
        case OamLoopCmdEnable :
            OamSetLoopbackState (link, OamInfoStateActionLoopback);

#if OAM_NTT_AS
            NttLoopbackEnable ();
#else
            OamEponLinkLoopbackEnable (link);
#endif
            break;

        case OamLoopCmdDisable :
            OamSetLoopbackState (link, OamInfoStateActionForward);

#if OAM_NTT_AS
            NttLoopbackDisable ();
#else
            OamEponLinkLoopbackDisable (link);
#endif
            break;

        default :
            break;
        }

    OamHeaderFill(link);
    OamSendInfo (link);
    } // OamHandleLoopback


////////////////////////////////////////////////////////////////////////////////
/// \brief  Handle standard OAM var req
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void OamHandleStdVarReq (void)
    {
    StreamWriteU8(&oamParser.reply, OamOpVarResponse);
    if (OamHandleGet())
        {
        OamTransmit();
        }
    } // OamHandleStdVarReq


////////////////////////////////////////////////////////////////////////////////
/// \brief  Handle vendor-specific OAM frames
///
/// \param  link    Link of frame reception
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamHandleVendor (LinkIndex link)
    {
    IeeeOui BULK * FAST pOui = (IeeeOui BULK*)oamParser.src.cur;
    U32 ouiVal;

    linkOamData[link].orgSpecificRx++;

    // write opcode
    StreamWriteU8(&(oamParser.reply), OamOpVendorOui);
    // write OUI
    StreamWriteOui(&oamParser.reply, pOui);
    StreamSkip(&oamParser.src, sizeof(IeeeOui));

    ouiVal = U24ToU32(pOui);
    cmsLog_notice("pOui = 0x%06x", ouiVal);
    switch (ouiVal)
        {
        case TekOuiVal:
        case CtcOuiVal:
        case DpoeOuiVal:
        case PmcOuiVal:
        case KtOuiVal:
        case DasanOuiVal:            
            VENDOR_OAM_HANDLE(oamValToOamVendor(ouiVal), link, (U8 BULK *) (pOui + 1));
            break;
            
        default:
            break;
        }
    } // OamHandleVendor


////////////////////////////////////////////////////////////////////////////////
/// OamLinkStart:  Start OAM at link
///
///
///
// Parameters:
/// \param link     link to start
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamLinkStart (LinkIndex link)
{
    OamPerLinkData *oam = &linkOamData[link];

    oam->discState = OamDiscPassiveWait;
} // OamLinkStart

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
//extern
void OamLinkReset (LinkIndex link)
{
    OamPerLinkData *oam = &linkOamData[link];

    memset (oam, 0, sizeof(OamPerLinkData));

    oam->discState = OamDiscPassiveWait;
    oam->loopbackState = OamInfoStateActionDiscard;
    oam->maxPduSize = MaxPktSize;
    oam->ticksUntilLinkFail = OamAllowedTicksUntilLinkFail;

    // other functions reset when the link goes down
    OsAstMsgQClr (OsAstAlmLinkOamDiscComplete, link, 0);

    VENDOR_LINK_RESET_HANDLE(link, 0);
} // OamLinkReset


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
//extern
void OamLinkInit (LinkIndex link)
{
    VENDOR_LINK_INIT(link);

    OamLinkReset (link);
#ifndef DESKTOP_LINUX
    linkOamData[link].discState = OamDiscNoOam;
#endif
    linkOamData[link].eventSeq = 0;

    cmsLog_debug("OamLinkInit %x", link);
} // OamLinkInit



////////////////////////////////////////////////////////////////////////////////
/// OamReset:  Reset OAM layer
///
// Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void OamReset (void)
    {
    U8 FAST i;

    for (i = 0; i < TkOnuNumTxLlids; ++i)
        {
        OamLinkInit (i);
        }
    } // OamReset


////////////////////////////////////////////////////////////////////////////////
/// OamInit:  Initialization of OAM module
///
// Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamIeeeInit (void)
    {
    //if (PersRead (PersRecIdOamIeeeCfg, &oamIeeeCfg, sizeof(oamIeeeCfg)) == 0)
        {
#if HARDCODED_PERSONALITY
        OamIeeeCreateDefaultPersonality ();
#endif
        }

    oamCreditsPerTick = OamDefaultCreditsPerTick;
    oamTicksPerMinRate = OamDefaultTicksPerMinRate;

    VENDOR_OAM_PRE_INIT();
    
    OamReset ();
    } // OamInit



////////////////////////////////////////////////////////////////////////////////
//extern
void OamHandle(LinkIndex link)
    {
    OamOpcode opcode = (OamOpcode)StreamReadU8(&oamParser.src);

    DumpOamMessage(link, opcode, (U8 *)RxFrame, rxFrameLength, Dnstream);

    switch (opcode)
        {
        case OamOpInfo :
            OamHandleInfo(link);
            break;

        case OamOpEventNotification :
            break;

        case OamOpVarRequest:
            OamHandleStdVarReq();
            break;

        case OamOpLoopback :
            OamHandleLoopback (link);
            break;

        case OamOpVendorOui :
            OamHandleVendor (link);
            break;

        case OamLegacyOpVendorExt : // legacy IOP based on draft 1.2
            {
            switch (StreamReadU8(&(oamParser.src)))
                {
                case OamLegacyOpPingRequest :
                    memcpy ((U8 BULK *) TxFrame,
                            (U8 BULK *) RxFrame,
                            rxFrameLength);
                    StreamWriteU8(&oamParser.reply, OamLegacyOpPingResponse);
                    OamTransmit();
                    break;

                default:
                    break;
                }
            }
            break;

        case OamOpVarResponse : // shouldn't get these
        default :
            linkOamData[link].unSuptCodesRx++;
            cmsLog_notice("OAM: Bad OAM opcode %02x\n", opcode);
            break;
        }
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Common processing when an OAM frame is received
///
/// \param link     Link on which message arrived
///
/// \pre    Global frame parser must point to the flags field of the OAM frame
///
/// \post   Global frame parser will point to the opcode field of the OAM frame
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamProcessCommon (U8 link)
    {
    U8 subType = StreamReadU8(&oamParser.src);    
    U16 flags = StreamReadU16(&oamParser.src);

    UNUSED(subType);
    /* reset hearbeat timeout */
    OamIeeeHeartClear(link);
    linkOamData[link].framesRx++;

/*    if (PersInterop (PersInteropStrictOamFlags)) */
        { /*  check flags field for all OAM messages */
        if ((linkOamData[link].discState == OamDiscComplete) &&
            (TkGetField(OamMsgFlagLocalDisc, flags) !=
            OamMsgFlagDiscComplete) &&
            (!PonMgrIsPmcOlt())

           )
            { /* peer flags invalid; reset link */
            cmsLog_error("received unexpected oam request:");
            DumpRawFrame(RxFrame, rxFrameLength);
            cmsLog_error("OAM: link %u peer flags %u; reset\n", link, flags);
            OamLinkInit (link);
            }
        }
    /* save their local flags, which are our remote flags */
    if (TkGetField (OamMsgFlagLocalDisc, flags) != OamMsgFlagDiscInvalid)
        {
        linkOamData[link].lastRemoteFlags = flags;
        }
    } /* OamProcessCommon */


////////////////////////////////////////////////////////////////////////////////
//extern
void OamProcess (LinkIndex link, U8 *pCurPos)
    {
    StreamInit(&oamParser.src, pCurPos);    
    
    OamProcessCommon (link);

    OamHeaderFill(link);
    oamParser.endReply = oamParser.reply.cur + MaxOamSpace(link);

    OamHandle(link);
    } // OamProcess


////////////////////////////////////////////////////////////////////////////////
//extern
void OamSendToLink (LinkIndex link, U16 size)
    {
    OamMsg BULK * FAST msg = (OamMsg*)(TxFrame + 1);
    OamPerLinkData BULK *    FAST linkData;
    if (link < TkOnuNumTxLlids)
        { 
        linkData = &linkOamData[link];
        if (linkData->discState < OamDiscComplete)
            {
            if ((linkData->discState == OamDiscNoOam) ||
                (msg->opcode != OamOpInfo))
                { 
                // still syncing; ignore other messages
                cmsLog_debug("link[%x] Drop Op %x in disc\n", link, msg->opcode);
                return;
                }
            }
        else
            { // only count credits once discovered
            if (oamCreditsPerTick != 0) // Otherwise Unlimited if 0
                {
                if (linkData->txCredits == 0)
                    {
                    cmsLog_error("link[%x] op %x no tx credits\n", link, msg->opcode);
                    return;
                    }
#ifndef DESKTOP_LINUX
                linkData->txCredits--;
#endif
                }
            
            switch(msg->opcode)
                {
                case OamOpInfo:
                    linkData->informationTx++;
                    break;
                
                case OamOpVendorOui:
                    linkData->orgSpecificTx++;
                    break;

                default:
                    break;
                }     

            linkData->framesTx++;

            }

        linkData->ticksSinceSend = 0;


        size = FrameSendUp (&OamMcMacAddr, link, size);

        DumpOamMessage(link, msg->opcode, (U8 *)TxFrame, size, Upstream);
        }
    } // OamSendToLink


////////////////////////////////////////////////////////////////////////////////
/// OamTimerOneLink:  Timer actions for a single link
///
// Parameters:
/// \param link     Link to check
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void OamTimerOneLink (LinkIndex link)
    {
    U8                      FAST creditsToFull;
    OamPerLinkData BULK *    FAST oam = &linkOamData[link];

    // accumulate tx credits
    creditsToFull = (oamCreditsPerTick * OamTicksPerSec) - oam->txCredits;
    oam->txCredits += (creditsToFull < oamCreditsPerTick) ?
                      creditsToFull : oamCreditsPerTick;

    if (oam->minRate > 0)
        { 
#ifdef DESKTOP_LINUX
		if(oam->ticksUntilLinkFail <= 1){
			return;
		}
#endif			
        oam->ticksUntilLinkFail--;
        if (oam->ticksUntilLinkFail == 0)
            { 
            cmsLog_error("OAM: Heartbeat timeout L:%x\n", link);
            if (oam->discState <= OamDiscPassiveWait)
                { 
                oam->ticksUntilLinkFail = OamAllowedTicksUntilLinkFail;
                }
            else
                {
                OsAstMsgQSet (OsAstAlmLinkOamTimeout, link, 0);
                OsAstMsgQSpin ();
                OamLinkReset (link);
                OsAstMsgQClr (OsAstAlmLinkOamTimeout, link, 0);
                }
            }

        oam->ticksSinceSend++;
       
        if (oam->ticksSinceSend >= oam->minRate)
            {
#if LINK_LOOPBACK_SUPPORT
            if (!LinkInLoopback (link))
                {
                thowawayOamMsg = TRUE;
                }
#endif
            OamHeaderFill(link);
            OamSendInfo (link);
            }
        }

    } // OamTimerOneLink


////////////////////////////////////////////////////////////////////////////////
/// OamHandleTimer:  Process OAM timer expiry
///
// Parameters:
/// \param timerId  ID of timer that expired
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHandleTimer (OntmTimerId timerId)
    {
    LinkIndex FAST link = timerId & 0xff;

    switch ((OntmTimerId)(timerId & 0xff00))
        {
        case OamTimerMaxRate :
            for (link = 0; link < OntCfgDbGetNumUserLinks(); ++link)
            {
            
            
            OamTimerOneLink(link);
            }
            break;


        default:
            cmsLog_notice("OAM: Unknown OAM timer %04x L:%x\n", timerId, link);
            break;
        }
    } 


// end OamIeee.c

/*
 *<:copyright-BRCM:2013:proprietary:epon 
 *
 *   Copyright (c) 2013 Broadcom 
 *   All Rights Reserved
 *
 * This program is the proprietary software of Broadcom and/or its
 * licensors, and may only be used, duplicated, modified or distributed pursuant
 * to the terms and conditions of a separate, written license agreement executed
 * between you and Broadcom (an "Authorized License").  Except as set forth in
 * an Authorized License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and Broadcom
 * expressly reserves all rights in and to the Software and all intellectual
 * property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 * NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 * BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1. This program, including its structure, sequence and organization,
 *    constitutes the valuable trade secrets of Broadcom, and you shall use
 *    all reasonable efforts to protect the confidentiality thereof, and to
 *    use this information only in connection with your use of Broadcom
 *    integrated circuit products.
 *
 * 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
 *    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
 *    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
 *    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
 *    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
 *    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
 *    PERFORMANCE OF THE SOFTWARE.
 *
 * 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
 *    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
 *    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
 *    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 *    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
 *    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
 *    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
 *    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
 *    LIMITED REMEDY.
:>
 */ 

////////////////////////////////////////////////////////////////////////////////
/// \file OntmOamSet.c
/// \brief Support for OAM "Set" frames
///
/// This code handles setting of attributes as found in OAM frames
///
////////////////////////////////////////////////////////////////////////////////

// generic includes
#include "Build.h"
#include <string.h> // memcpy
#include "Teknovus.h"
#include "cms_log.h"

// interface includes
#include "Oam.h"
#include "OamEpon.h"
#include "OamIeee.h"
#include "OamIeeeAutoNeg.h"
#include "OamIeeeFec.h"
#include "OamIeeeMac.h"
#include "OamIeeePhy.h"
#if OAM_POLICER
#include "OamPolicer.h"
#endif
#include "OamSet.h"
#if OAM_SHAPER
#include "OamShaper.h"
#endif
#include "OamStats.h"
#include "OamUtil.h"
#include "Alarms.h"

#ifdef BCM_OAM_BUILD
#include "OamTekUserRules.h"
#include "OamTekSec.h"
#include "OamGeneralActions.h"
#include "OamQueue.h"
#include "OamShaper.h"
#include "OamSys.h"
#include "OamPortOperations.h"
#include "OamPolicer.h"
#endif

// app includes
#if CLOCK_TRANSPORT
#include "ClockTransport.h"
#endif

#include "Holdover.h"
#include "Laser.h"

// platform includes
#include "EponDevInfo.h"

#define ConvOam2Rstp(val)      ((val)/100)


////////////////////////////////////////////////////////////////////////////////
/// \brief  Sets attributes for Ethernet port
///
/// \param  port    Port to affect
/// \param  src     Source container for attribute
/// \param  reply   Location for response
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamSetAttrEth (TkOnuEthPort port,
                    OamVarContainer BULK* src,
                    OamVarContainer BULK* reply)
    {
    switch (OAM_NTOHS(src->leaf))
        {
        // standard
        // MAC
        case OamAttrMacDuplexStatus :
            OamIeeeUniMacDuplexStatusSet(port, src);
            break;

        case OamAttrMacEnableStatus :
            if (!OamIeeeMacEnableStatusSet(port, src))
                {
                reply->length = OamVarErrActBadParameters;
                }
            break;

        // Auto-negotiation
        //this is the auto-negotiation advertisement
        case OamAttrAutoNegAdTech :
            if (!OamIeeeAdTechSet(port,src,reply))
                {
                reply->length = OamVarErrActBadParameters;
                }
            break;

        //this is the auto-negotiation vs forced mode provisioning
        case OamAttrAutoNegAutoCfg :
            OamIeeeAutoCfgSet(port, src);
            break;

        case OamAttrMacCtrlFuncsSupported :
            OamIeeeMacCtrlFuncsSet(port, src);
            break;

#ifdef BCM_OAM_BUILD
        // extended attributes
        case OamExtAttrDynLearnTblSize :
            OamHdlDynLearnTableSet(port,src,reply);
            break;

        case OamExtAttrMinMacLimit :
            OamHdlDynLearnMinSet(port, src, reply);
            break;

        case OamExtAttrDynLearnAgeLimit :
            OamHdlDynLearnAgeSet(port, src, reply);
            break;

        case OamExtAttrDynLearningMode :
            OamHdlDynLearnModeSet(port,src,reply);            
            break;

        case OamExtAttrPortStatThreshold :
            OamTekStatsSetEthThdVal (port, (OamVarContainer BULK*)src, reply);
            break;

        case OamExtAttrPortCapability:
            OamHdlPortCapabilitySet(port, src, reply);
            break;

        case OamExtAttrPriEnqueuing:
			// ignore
            break;

        case OamExtAttrCosTranslation:
			// ignore
            break;

        case OamExtAttrIngressPolicing:
			OamBcmIngressPolicingSet(src, reply);
            break;

        case OamExtAttrEgressShaping:
            TkOnuSetEgressShaping (TRUE, port, src, reply);
            break;
            
#endif
        default :
            reply->length = OamVarErrAttrUnsupported;
            break;
        }
    } // OamSetAttrEth

////////////////////////////////////////////////////////////////////////////////
/// \brief  Set attribute for EPON port
///
/// \param  src     Source container for attribute
/// \param  reply   Location for response
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamSetAttrEpon (OamVarContainer BULK* src, OamVarContainer BULK* reply)
    {
#ifdef BCM_OAM_BUILD
    LinkIndex FAST link = OamContextLink();
#endif
    switch (OAM_NTOHS(src->leaf))
        {
#ifdef BCM_OAM_BUILD        
        case OamExtAttrOnuFailsafe:
            OamHdlFailsafeSet((OamExtFailsafes BULK *)(&src->value[0]));
            break;
            
        // encryption
        case OamExtAttrEncryptMode:
            OamTekSecEncryptModeSet(src, reply);
            break;
            
        case OamExtAttrEncryptKeyExpiryTime :
            {
            U16 expiryTime =  ContGetU16(src);
            
            EncryptSetKeyTimeout(link, expiryTime, OuiTeknovus);
            EncryptModeModify(link, expiryTime);
            }
            break;

        case OamExtAttrPortStatThreshold :
            OamTekStatsSetEponThdVal ((OamVarContainer BULK*)src, reply);
            break;

        case OamExtAttrLinkStatThreshold :
            OamTekStatsSetLinkThdVal(link, (OamVarContainer BULK*)src, reply);
            break;

        // reporting thresholds for this link
        case OamExtAttrReportThresholds :
            OamHdlEponReportThreshSet(src,reply);
            break;

        case OamExtAttrVlanEthertype :
        // ignore
            break;

        case OamExtAttrOnuAggregateLimit :
            OamHdlAggMacLimitSet(src,reply);
            break;

        case OamExtAttrEgressShaping:
            TkOnuSetEgressShaping (FALSE, 0, src, reply);
            break;
            
        case OamExtAttrFECMode:
            OamHdlEponExtFecSet(link,src,reply);
            break;

        case OamExtAttrLinkState:
            // Onu is notified that a link is enabled/disabled
            OamHdlEponLinkStateSet(src, reply);
            break;

        case OamExtAttrOnuHoldover :
            OamTekEponHoldoverSet(src, reply);
            break;


        case OamExtAttrOnuPSState:
//            PsSetState(ContGetU16(src));
            break;

#endif

        case OamAttrFecMode:
            OamIeeeFecModeSet(src);
            break;

        case OamExtAttrAlarmIdThreshold:
            OamAlarmSetEponThdVal ((OamVarContainer BULK*)src, reply);
            break;

        default :
            reply->length = OamVarErrAttrUnsupported;
            break;
        }
    } // OamSetAttrEpon



////////////////////////////////////////////////////////////////////////////////
/// \brief  Set attribute for port
///
/// \param  src     Source container for attribute
/// \param  reply   Location for response
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamSetAttr(OamVarContainer BULK* src, OamVarContainer BULK* reply)
{
    U8 FAST port = OamContextPort();

    //OntmDebug (OntmDebugStd, ("OAM : set attr %04x port %bu\n",
    //    OAM_NTOHS(src->leaf), port));

    reply->length = OamVarErrNoError;
    if (port == 0)
    {
         OamSetAttrEpon (src, reply);
    }
    else
    {
         OamSetAttrEth (port - 1, src, reply);
    }
} // OamSetAttr

////////////////////////////////////////////////////////////////////////////////
/// OamActEth:  Execute OAM Actions for Ethernet ports (only)
///
 // Parameters:
/// \param port     Port to affect
/// \param src      Source container for action parameters
/// \param reply    Location for response
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamActEth (TkOnuEthPort port,
                OamVarContainer const BULK* src,
                OamVarContainer BULK* reply)
    {
    reply->length = OamVarErrNoError; // let's be optimistic

    switch (OAM_NTOHS(src->leaf))
        {
        // standard
        case OamActPhyAdminControl :
            {
#if !OAM_NTT_AS
                if (ctcIeeeOamProcessing)
                {
                    if((ContGetU32(src) !=  OamCtcActionDisable)  
                        && (ContGetU32(src)  != OamCtcActionEnable) )
                    {
                        reply->length = OamVarErrActBadParameters;
                        break;
                    }
                }
                else
#endif
                {
                    if((ContGetU8(src) != OamStateEnabled) && (ContGetU8(src) != OamStateDisabled) )
                    {
                        reply->length = OamVarErrActBadParameters;
                        break;
                    }
                }
                OamIeeePhyAdminControl(port, src);
            }
            break;

        // AutoNeg
        case OamActAutoRenegotiate :
            OamIeeeAutoRenegotiate(port, src);
            break;

        case OamActAutoAdminCtrl :
            OamIeeeAutoAdminCtrl(port, src);
            break;

#ifdef BCM_OAM_BUILD
        case OamExtActClearDynLearnTbl :
//            TkOnuUluClearDynamicTable(port);
            break;

        case OamExtActAddStaticEntry :
            // ignore
            break;

        case OamExtActDelStaticEntry :
            // ignore
            break;

        case OamExtActClearUpFilterTbl :
            // ignore            ConfigUpRuleClear (port, RuleUseFilter);
            break;

        case OamExtActClrUpUserRules :
            reply->length = ClearRulesByPort();
            break;
        case OamExtActClrUpClass :
//            ConfigUpRuleClear (port, RuleUseClassify);
            break;

        case OamExtActLoopbackEnable :
            OamHdlLpbkEnAct(port,src,reply);
            break;

        case OamExtActLoopbackDisable :
            OamHdlLpbkDisAct(port,src,reply);
            break;
#endif
        default :
            reply->length = OamVarErrAttrUnsupported;
            break;

        }

    } // OamActEth


////////////////////////////////////////////////////////////////////////////////
//extern
void OamAct(const OamVarContainer BULK* src, OamVarContainer BULK* reply)
    {
    TkOnuEthPort  port = OamContextPort();
    cmsLog_notice("leaf = 0x%x,port=%d", OAM_NTOHS(src->leaf),port);

    reply->length = OamVarErrNoError; // let's be optimistic

    switch (OAM_NTOHS(src->leaf))
        {
        // standard
        // actions that apply only to Ethernet ports
        case OamActAutoRenegotiate :
        case OamActAutoAdminCtrl :
#ifdef BCM_OAM_BUILD
        case OamExtActAddStaticEntry :
        case OamExtActDelStaticEntry :
        case OamExtActClearUpFilterTbl :
        case OamExtActClrUpClass :
        case OamExtActClrUpUserRules :
        case OamExtActLoopbackEnable :
        case OamExtActLoopbackDisable :
#endif            
        case OamActPhyAdminControl :
            if (port == 0)
            {
                reply->length = OamVarErrActBadParameters;
            }
            else
            {
                OamActEth (port - 1, src, reply);
            }
            break;

#ifdef BCM_OAM_BUILD
        case OamExtActClearDynLearnTbl :
            { // Ignore
            if (port == 0)
                {
                reply->length = OamVarErrActBadParameters;
                }
            else
                {
                OamActEth (port - 1, src, reply);
                }
            }
            break;

        case OamExtActClearDnFilterTbl :
//            ConfigDnRuleClear (RuleUseFilter); ignore
            break;

        case OamExtActNewAddRule:
            reply->length = AddOamRule (port, RULE_NEWFORMAT,
                (OamNewRuleSpec BULK *)src->value, (OamVarContainer *)src);
            break;

        case OamExtActSetOamRate:
            OamHdlGenActOamRateSet(src,reply);
            break;

        case OamExtActGetOamRate:
            OamHdlGenActOamRateGet((OamVarContainer *)src, reply);
            break;
            
        case OamExtActNewDelRule:
            reply->length = DelOamRule (port, RULE_NEWFORMAT,
                (OamNewRuleSpec BULK*)src->value);
            break;

        case OamExtActClrDnUserRules :
            reply->length = ClearRulesByPort();
            break;

        case OamExtActClrDnClass :
//             ConfigDnRuleClear (RuleUseClassify); ignore
            break;

        case OamExtActSetQueueConfig :
            OamBcmQueueSet(src, reply);
            break;

        case OamExtActGetQueueConfig :
            OamBcmQueueGet(reply);
            break;

        case OamExtActEraseNvs :
            // ignore NvsErase ();
            break;

        case OamExtActOnuEnableUserTraffic:
            OamHdlGenActOnuEnableUserTraffic((OamVarContainer *)src, reply);
            break;

        case OamExtActOnuDisableUserTraffic:
            OamHdlGenActOnuDisableUserTraffic((OamVarContainer *)src, reply);
            break;

        case OamExtActBcLaserPowerOff:
            OamHdlOptCtrlLaserShutdown((OamVarContainer *)src, reply);
            reply->length = OamVarErrNoError;
            break;
        case OamExtActResetOnu:
            OntmTimerCreate(100, OamBcmResetTimer);
            break;
#endif

#if OAM_POLICER
        case OamExtActEnablePolicer:
            OamPolicerEnable (src, reply);
            break;

        case OamExtActDisablePolicer:
            OamPolicerDisable (src, reply);
            break;
#endif

        case OamExtActClearStats :
#ifdef BCM_OAM_BUILD                
#if STATS_COLLECT
            OamTekStatsClearAll();
#endif
#endif
            break;

        default :
            reply->length = OamVarErrAttrUnsupported;
            break;

        }
    } // OamAct

// end OamSet.c


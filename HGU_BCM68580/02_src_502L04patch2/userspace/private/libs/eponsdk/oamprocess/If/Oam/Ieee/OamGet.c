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
/// \file OntmOamGet.c
/// \brief Support for OAM "Get" operatios
///
/// This code handles setting of attributes as found in OAM frames
///
////////////////////////////////////////////////////////////////////////////////


#include "Build.h"
#include "Ethernet.h"
#include "Oam.h"
#include "OamTeknovus.h"
#if OAM_POLICER
#include "OamPolicer.h"
#endif
#if OAM_SHAPER
#include "OamShaper.h"
#endif
#include "OamUtil.h"
#include "EponDevInfo.h"
#include "OamStats.h"
#include "Teknovus.h"
#include <string.h>
#include "OamGet.h"

#include "OamIeeeAutoNeg.h"
#include "OamIeeeFec.h"
#include "OamIeeeMac.h"
#include "OamIeeePhy.h"
#include "cms_log.h"
#include "Alarms.h"
#include "OamEpon.h"

#ifdef BCM_OAM_BUILD
#include "OamTekUserRules.h"
#include "OamTekSec.h"
#include "OamShaper.h"
#include "OamSys.h"

#if STATS_COLLECT
#include "OamPortOperations.h"
#endif

#endif

#define ConvRstp2Oam(val)      ((val)*100)



////////////////////////////////////////////////////////////////////////////////
//extern
void OamGetAttrEth (TkOnuEthPort port,
                    OamVarContainer BULK* cont,
                    OamVarDesc BULK* src)
    {
    switch (OAM_NTOHS(src->leaf))
        {
        case OamAttrMacId :
            ContPutU8 (cont, port);
            break;

        case OamAttrMacFramesTxOk :
        case OamAttrMacSingleCollFrames :
        case OamAttrMacMultipleCollFrames :
        case OamAttrMacFramesRxOk :
        case OamAttrMacFcsErr :
        case OamAttrMacAlignErr :
        case OamAttrMacOctetsTxOk :
        case OamAttrMacLateCollisions :
        case OamAttrMacExcessiveCollisions :
        case OamAttrMacFramesLostMacTxErr :
        case OamAttrMacOctetsRxOk :
        case OamAttrMacFramesLostMacRxErr :
        case OamAttrMacMcastFramesTxOk :
        case OamAttrMacBcastFramesTxOk :
        case OamAttrMacMcastFramesRxOk :
        case OamAttrMacBcastFramesRxOk :
        case OamAttrMacInRangeLenErr :
        case OamAttrMacOutOfRangeLenErr :
        case OamAttrMacFrameTooLong :

        case OamAttrMacCtrlFramesTx :
        case OamAttrMacCtrlFramesRx :
        case OamAttrMacCtrlUnsupportedOpRx :
        case OamAttrMacCtrlPauseDelay :
        case OamAttrMacCtrlPauseTx :
        case OamAttrMacCtrlPauseRx :

        case OamExtAttrRxUnicastFrames :
        case OamExtAttrTxUnicastFrames :
        case OamExtAttrRxFrameTooShort :

        /* frame size bins, as per RFC 1757 */
        case OamExtAttrRxFrame64 :
        case OamExtAttrRxFrame65_127 :
        case OamExtAttrRxFrame128_255 :
        case OamExtAttrRxFrame256_511 :
        case OamExtAttrRxFrame512_1023 :
        case OamExtAttrRxFrame1024_1518 :
        case OamExtAttrRxFrame1519Plus :

        case OamExtAttrTxFrame64 :
        case OamExtAttrTxFrame65_127 :
        case OamExtAttrTxFrame128_255 :
        case OamExtAttrTxFrame256_511 :
        case OamExtAttrTxFrame512_1023 :
        case OamExtAttrTxFrame1024_1518 :
        case OamExtAttrTxFrame1519Plus :

        case OamExtAttrTxFramesDropped :
        case OamExtAttrTxBytesDropped :
        case OamExtAttrTxBytesDelayed :
        case OamExtAttrTxDelay :

        case OamExtAttrRxFramesDropped :
        case OamExtAttrRxBytesDropped :
        case OamExtAttrRxBytesDelayed :
        case OamExtAttrRxDelay :
#ifdef BCM_OAM_BUILD              
#if STATS_COLLECT
            OamPortStatsGet(port, src, cont);
#endif
#endif
            break;

        case OamAttrMacFramesDeferred :
        case OamAttrMacCarrierSenseErr :
        case OamAttrMacFrExcessiveDeferral :
            ContPutU8 (cont, 0);
            break;
        case OamExtAttrTxDelayThreshold :
        case OamExtAttrRxDelayThreshold :
            ContPutU8 (cont, 30);
            break;
        case OamAttrMacPromiscuousStatus :
            break;

        case OamAttrMacMcastAddrList :
            break;

        case OamAttrPhyAdminState :
            OamIeeePhyAdminStateGet(port, cont);
            break;

        case OamAttrMacEnableStatus :
            OamIeeeMacEnableStatusGet(port, cont);
            break;


        case OamAttrMacTxEnable :
            break;

        case OamAttrMacMcastRxStatus :
            break;

        case OamAttrMacAddr :
            OamIeeeMacAddrGet(port, cont);
            break;


        /* PHY
           This actually has little to do with PHY type this is the attribute
           that correlates to port speed */
        case OamAttrPhyType :
            OamIeeeUniPhyTypeGet(port, cont);
            break;

        /* MAC - later additions */
        case OamAttrMacCapabilities :
            break;

        case OamAttrMacDuplexStatus :
            OamIeeeUniMacDuplexStatusGet(port, cont);
            break;

        /* MAU */
        case OamAttrMauMediaAvail :
            OamIeeeMauMediaAvailGet(port, cont);
            break;

        /* Auto-negotiation */
        case OamAttrAutoNegId :
            /* OamHdlAutoNegId(port, cont, src); */
            break;

        case OamAttrAutoNegAdminState :
            OamIeeeAdminStateGet(port, cont);
            break;

        case OamAttrAutoNegRemoteSig :
            /* OamHdlAutoNegRemoteSig(port, cont, src); */
            break;

        case OamAttrAutoNegAutoCfg :
            OamIeeeAutoCfgGet(port, cont);
            break;

        case OamAttrAutoNegLocalTech :
            OamIeeeLocalTechGet(port, cont);
            break;

        case OamAttrAutoNegAdTech :
            OamIeeeAdTechGet(port, cont);
            break;

        case OamAttrAutoNegRxTech :
            /* OamIeeeRxTechGet(port, cont); */
            break;

        case OamAttrAutoNegLocalSelectAble :
        case OamAttrAutoNegAdSelectAble :
        case OamAttrAutoNegRxSelectAble :
            /* OamHdlAutoNegSelectAble(port, cont, src); */
            break;

        /* DTE MAC Control */
        case OamAttrMacCtrlId :
            /* OamHdlMacControlId(port, cont, src); */
            break;

        case OamAttrMacCtrlFuncsSupported :
            OamIeeeMacCtrlFuncsGet(port, cont);
            break;
            
        case OamExtAttrLenErrorDiscard :
            /* OamHdlLenErrorDiscard(port, cont, src); */
            break;
            
        case OamExtAttrMdiCrossover :
            /* OamHdlMdiCrossover(port, cont, src); */
            break;
            
        case OamExtAttrMtu:
            /* OamHdlTekMtuGet (port, cont); */
            break;

#ifdef BCM_OAM_BUILD
        case OamExtAttrEgressShaping:
            TkOnuGetEgressShaping (TRUE, port, cont);
            break;

        case OamExtAttrIngressPolicing:
            OamBcmIngressPolicingGet(cont);
            break;

        case OamExtAttrFirmwareVer :
            OamHdlFirmwareVersion(0, cont, src);
            break;

        case OamExtAttrExtendedId :
            OamHdlExtendedId(port, cont, src);
            break;

        case OamExtAttrJedecId:
            OamHdlJedecId(port, cont, src);
            break;

        case OamExtAttrChipId:
            OamHdlChipId(port, cont, src);
            break;

        case OamExtAttrChipVersion:
            OamHdlExtAttrChipVersion(port, cont, src);
            break;

        case OamExtAttrPortCapability:
            OamHdlPortCapability(port, cont, src);
            break;

        case OamExtAttrDynLearnAgeLimit :
            OamHdlDynLearnAgeGet(port, cont);
            break;
        case OamNewExtAttrUpFilterTbl :
            GetOamRules (cont,
                         (RULE_ALLVOLATILE | RULE_NEWFORMAT),
                         OamNewExtAttrUpFilterTbl,
                         RuleUseNone);           /* Look for all rules */
            break;
        case OamExtAttrUpFilterTbl :
            GetOamRules (cont,
                         (RULE_ALLVOLATILE | RULE_OLDFORMAT),
                         OamExtAttrUpFilterTbl,
                         RuleUseNone);           /* Look for all rules     */
            break;
            
        case OamExtAttrPortStatThreshold:
            OamTekStatsGetEthThdVal(port, (OamVarContainer BULK*)src, cont);
            break;

        /* bridging */
        case OamExtAttrDynLearnTblSize :
            OamHdlExtAttrDynLearnTableSize(port, cont, src);
            break;

        case OamExtAttrMinMacLimit:
            OamHdlExtAttrMinMacLimit(port, cont, src);
            break;

        case OamExtAttrDynMacTbl :
            OamHdlExtAttrPortDynEntries(cont, src);
            break;
            

        case OamExtAttrDynLearningMode:
            OamHdlExtAttrDynLearnMode(port, cont, src);
            break;            
#endif

        default :
            cont->length = OamVarErrAttrUnsupported;
            break;
        }
    } /* OamGetAttrEth */



////////////////////////////////////////////////////////////////////////////////
//extern
void OamGetAttrEpon(OamVarContainer BULK* cont, OamVarDesc BULK* src)
    {
#ifdef BCM_OAM_BUILD
    LinkIndex FAST link = OamContextLink();
#endif
    /*lint --e{64} type mismatch standard/extended leaf */
    switch (OAM_NTOHS(src->leaf))
        {
        /* standard attributes */
        /* MAC */
        case OamAttrMacDuplexStatus :
            OamIeeePonMacDuplexStatusGet(cont);
            break;

        case OamAttrMacFramesTxOk :
        case OamAttrMacFramesRxOk :
        case OamAttrMacFcsErr :
        case OamAttrMacOctetsTxOk :
        case OamAttrMacFramesLostMacTxErr :
        case OamAttrMacOctetsRxOk :
        case OamAttrMacFramesLostMacRxErr :

        case OamAttrMacCtrlFramesTx :
        case OamAttrMacCtrlFramesRx :
        case OamAttrMacCtrlUnsupportedOpRx :
        case OamAttrMacCtrlPauseDelay :
        case OamAttrMacCtrlPauseTx :
        case OamAttrMacCtrlPauseRx :

        case OamAttrMacSingleCollFrames :
        case OamAttrMacMultipleCollFrames :
        case OamAttrMacAlignErr :
        case OamAttrMacLateCollisions :
        case OamAttrMacExcessiveCollisions :
        case OamAttrMacMcastFramesTxOk :
        case OamAttrMacBcastFramesTxOk :
        case OamAttrMacMcastFramesRxOk :
        case OamAttrMacBcastFramesRxOk :
        case OamAttrMacInRangeLenErr :
        case OamAttrMacOutOfRangeLenErr :
        case OamAttrMacFrameTooLong :

        case OamExtAttrRxUnicastFrames :
        case OamExtAttrTxUnicastFrames :
        case OamExtAttrRxFrameTooShort :

        /* frame size bins, as per RFC 1757 */
        case OamExtAttrRxFrame64 :
        case OamExtAttrRxFrame65_127 :
        case OamExtAttrRxFrame128_255 :
        case OamExtAttrRxFrame256_511 :
        case OamExtAttrRxFrame512_1023 :
        case OamExtAttrRxFrame1024_1518 :
        case OamExtAttrRxFrame1519Plus :

        case OamExtAttrRxFramesDropped :
        case OamExtAttrRxBytesDropped :
        case OamExtAttrRxBytesDelayed :
        case OamExtAttrRxDelay :

        case OamExtAttrTxFrame64 :
        case OamExtAttrTxFrame65_127 :
        case OamExtAttrTxFrame128_255 :
        case OamExtAttrTxFrame256_511 :
        case OamExtAttrTxFrame512_1023 :
        case OamExtAttrTxFrame1024_1518 :
        case OamExtAttrTxFrame1519Plus :

        case OamExtAttrTxFramesDropped :
        case OamExtAttrTxBytesDropped :
        case OamExtAttrTxBytesDelayed :
        case OamExtAttrTxDelay :
        case OamExtAttrTxBytesUnused :
        case OamAttrOamEmulCrc8Err:

        case OamAttrOamLocalErrFrameSecsEvent:

#if OAM_VERSION < 320
        /* No MPCP attributes prior to version 320 */
#elif OAM_VERSION == 320
        case OamAttrMpcpMACCtrlFramesTx:
        case OamAttrMpcpMACCtrlFramesRx:
        case OamAttrMpcpTxRegAck:
        case OamAttrMpcpTxRegRequest:
        case OamAttrMpcpTxReport:
        case OamAttrMpcpRxGate:
        case OamAttrMpcpRxRegister:
        case OamAttrMpcpDiscoveryWindowTx:
        case OamAttrMpcpDiscoveryTimeout:
        case OamAttrMpcpTxGate:
        case OamAttrMpcpTxRegister:
        case OamAttrMpcpRxRegAck:
        case OamAttrMpcpRxRegRequest:
        case OamAttrMpcpRxReport:
        case OamAttrPhySymbolErrDuringCarrier :
#else
#error "Unsupported OAM OAM_VERSION"
#endif
            OamHdlEponStats(src, cont);
            break;

        /* Power Monitoring Statistics, these aren't "true" statistics in that
           they do not need to be gathered and therefore do not need to be
           wasting data space.  We can just directly read them when requested. */
        case OamExtAttrPowerMonTemperature:
        case OamExtAttrPowerMonVcc:
        case OamExtAttrPowerMonTxBias:
        case OamExtAttrPowerMonTxPower:
        case OamExtAttrPowerMonRxPower:
            OamHdlEponPowerMonitorStats(cont, src);
            break;

        /* FEC */
        case OamAttrFecCorrectedBlocks:
            OamIeeeFecCorrectedBlocksGet(cont);
            break;

        case OamAttrFecUncorrectableBlocks:
            OamIeeeFecUncorrectableBlocksGet(cont);
            break;

        case OamAttrFecAbility:
            OamIeeeFecAbilityGet(cont);
            break;

        case OamAttrFecMode:
            OamIeeeFecModeGet(cont);
            break;

        /* addtional per-LLID  statistics */
        case OamExtAttrTxDelayThreshold :
        case OamExtAttrRxDelayThreshold :
            ContPutU8 (cont, 30);
            break;

        case OamAttrMacFramesDeferred :
        case OamAttrMacCarrierSenseErr :
        case OamAttrMacFrExcessiveDeferral :
            ContPutU8 (cont, 0);
            break;

        case OamAttrMacPromiscuousStatus :
        case OamAttrMacMcastAddrList :
            break;

        /* PHY */
        case OamAttrPhyType :
            OamIeeePonPhyTypeGet(cont);
            break;

#ifdef BCM_OAM_BUILD
        /* encryption */
        case OamExtAttrEncryptKeyExpiryTime :
            OamTekSecKeyExpiryGet(cont);
            break;

        case OamExtAttrEncryptMode:
            OamTekSecEncryptModeGet(cont);
            break;

        case OamExtAttrOnuFailsafe:
            OamHdlFailsafeGet (OAM_NTOHS(src->leaf), cont, src);
            break;
            
        case OamExtAttrFirmwareVer :
            OamHdlFirmwareVersion(0, cont, src);
            break;

        case OamExtAttrExtendedId :
            OamHdlExtendedId(0, cont, src);
            break;

        case OamExtAttrOnuAggregateLimit :
            OamHdlAggMacLimitGet(cont);
            break;

        case OamNewExtAttrDnFilterTbl :
            GetOamRules (cont,
                         (RULE_ALLVOLATILE | RULE_NEWFORMAT),
                         OamNewExtAttrDnFilterTbl,
                         RuleUseNone);           /* Look for all rules */
            break;
 
        case OamExtAttrDnFilterTbl :
            GetOamRules (cont,
                         (RULE_ALLVOLATILE | RULE_OLDFORMAT),
                         OamExtAttrDnFilterTbl,
                         RuleUseNone);           /* Look for all rules    */
            break;

        case OamExtAttrEgressShaping:
            TkOnuGetEgressShaping (FALSE, 0, cont);
            break;     
            
        case OamExtAttrReportThresholds:
            OamHdlEponReportThresholds (cont);
            break;

        case OamExtAttrFECMode:
            OamHdlEponExtFecGet(cont, src);
            break;
        case OamExtAttrAlarmIdThreshold:
            OamAlarmGetEponThdVal ((OamVarContainer BULK*)src, cont);
            break;
            
        case OamExtAttrPortStatThreshold :
            OamTekStatsGetEponThdVal ((OamVarContainer BULK*)src, cont);
            break;

        case OamExtAttrLinkStatThreshold :
            OamTekStatsGetLinkThdVal (link, (OamVarContainer BULK*)src, cont);
            break;
            
        case OamExtAttrOnuHoldover :
            OamTekEponHoldoverGet(src, cont);
            break;          
#endif

        default :
            cont->length = OamVarErrAttrUnsupported;
            break;
        }
    } /* OamGetAttrEpon */



////////////////////////////////////////////////////////////////////////////////
/// \brief  Get value of one attribute
///
/// \param src      Source attribute
/// \param cont     Location for response
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamGetAttr(OamVarDesc BULK* src, OamVarContainer BULK* cont)
    {
    if ((OamContextType() == OamCtxtLink) ||
        (OamContextType() == OamCtxtLinkQueue) ||
        ((OamContextType() == OamCtxtPort) && (OamContextPort() == 0)))
        {
        OamGetAttrEpon (cont, src);
        }
    else
        {
        OamGetAttrEth (OamContextPort() - 1, cont, src);
        }
    } /* OamGetAttr */


////////////////////////////////////////////////////////////////////////////////
/// \brief  get attribute for bridge
///
/// \param src      Source container for attribute
/// \param reply    Location for response
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamGetBridgeAttr (const OamVarDesc BULK* src,
                       OamVarContainer BULK* cont)
    {
#if RSTP && OAM_RSTP
    U8 FAST bridge = OamContextBridge ();
#endif

    
        {
        cont->length = OamVarErrNoError;

        switch (OAM_NTOHS(src->leaf))
            {
#if RSTP && OAM_RSTP
            case OamExtAttrRstpBridgeMode:
                ContPutU8(cont, RstpGetBridgeMode(bridge));
                break;

            case OamAttrBridgeAddress:
            case OamAttrBridgePortAddresses:
                cont->length = sizeof(MacAddr);
                memcpy((MacAddr BULK*)&cont->value[0],
                    &(RstpGetBridgeId(bridge)->macAddr), sizeof(MacAddr));
                break;

            case OamAttrBridgeNumPorts:
                ContPutU8(cont, RstpGetBridgeNumPorts(bridge));
                break;

            case OamAttrRstpBridgeIdentifier:
                cont->length = sizeof(RstpBridgeId);
                memcpy((RstpBridgeId BULK*)&cont->value[0],
                    RstpGetBridgeId(bridge), sizeof(RstpBridgeId));
                break;

            case OamAttrRstpTimeSinceTopologyChange:
            case OamAttrRstpTopologyChangeCount:
                ContPutU8(cont, 0);
                break;

            case OamAttrRstpDesignatedRoot:
                {
                U8 FAST rootPort = RstpGetRootPort(bridge);

                cont->length = sizeof(RstpBridgeId);
                if (rootPort < RstpPortCount)
                    {
                    memcpy((RstpBridgeId BULK*)&cont->value[0],
                        &(RstpGetPortPriVector(bridge, rootPort)->rootBridgeId),
                        sizeof(RstpBridgeId));
                    }
                else
                    {
                    memcpy((RstpBridgeId BULK*)&cont->value[0],
                        RstpGetBridgeId(bridge), sizeof(RstpBridgeId));
                    }
                }
                break;

            case OamAttrRstpRootPathCost:
                {
                U8 FAST rootPort  = RstpGetRootPort(bridge);

                if (rootPort < RstpPortCount)
                    {
                    ContPutU32(cont,
                        RstpGetPortPriVector(bridge, rootPort)->rootPathCost);
                    }
                else
                    {
                    ContPutU32(cont, 0);
                    }
                }
                break;

            case OamAttrRstpRootPort:
                if (RstpGetRootPort(bridge) < RstpPortCount)
                    {
                    ContPutU8(cont, RstpGetRootPort(bridge) + 1);
                    }
                else
                    {
                    ContPutU8(cont, 0);
                    }
                break;

            case OamAttrRstpMaxAge:
            case OamAttrRstpBridgeMaxAge:
                ContPutU16(cont, ConvRstp2Oam(RstpGetBridgeMaxAge(bridge)));
                break;

            case OamAttrRstpHelloTime:
            case OamAttrRstpBridgeHelloTime:
                ContPutU16(cont, ConvRstp2Oam(RstpGetBridgeHelloTime(bridge)));
                break;

            case OamAttrRstpForwardDelay:
            case OamAttrRstpBridgeForwardDelay:
                ContPutU16(cont,
                    ConvRstp2Oam(RstpGetBridgeForwardDelay(bridge)));
                break;

            case OamAttrRstpHoldTime:
                ContPutU8(cont, RstpGetBridgeHoldTime(bridge));
                break;

            case OamAttrRstpBridgePriority:
                ContPutU16(cont, RstpGetBridgePriority(bridge));
                break;
#endif
#if OAM_LUE
            case OamExtAttrDynLearnAgeLimit:
                OamLueAgeTimeGet (src, cont);
                break;

            case OamExtAttrRule:
                OamLueRuleGet (cont, src);
                break;

            case OamExtAttrSearchConfig:
                OamLueSearchGet (src, cont);
                break;

            case OamExtAttrLearningConfig:
                OamLueLearningGet(src,cont);
                break;
#endif
            default :
                cont->length = OamVarErrAttrUnsupported;
                break;
            }
        }
    } /* OamGetBridgeAttr */



#if RSTP && OAM_RSTP
////////////////////////////////////////////////////////////////////////////////
/// OamGetAttrBridgePort:  Get Bridge related attribute
///
 // Parameters:
/// \param port     Ethernet port number
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamGetAttrBridgePort (TkOnuEthPort port,
                           const OamVarDesc BULK* src,
                           OamVarContainer BULK* cont)
    {
    U8 FAST bridge = OamContextBridge();

    switch (src->leaf)
        {
        case OamAttrRstpTopologyChange:
            ContPutU8(cont, 0);
            break;

        case OamExtAttrRstpPortOperEdge:
            ContPutU8(cont, (U8)RstpGetPortOperEdge(bridge, port));
            break;

        case OamAttrRstpPortIdentifier:
            ContPutU16(cont, RstpGetPortId(bridge, port));
            break;

        case OamAttrRstpPortPriority:
            ContPutU8(cont, RstpGetPortPriority(bridge, port));
            break;

        case OamAttrRstpPathCost:
            ContPutU32(cont, RstpGetPortRootPathCost(bridge, port));
            break;

        case OamAttrRstpPortDesignatedRoot:
            cont->length = sizeof(RstpBridgeId);
            memcpy((RstpBridgeId BULK*)&cont->value[0],
                &(RstpGetPortPriVector(bridge, port)->rootBridgeId),
                sizeof(RstpBridgeId));
            break;

        case OamAttrRstpDesignatedCost:
            ContPutU32(cont,
                RstpGetPortPriVector(bridge, port)->rootPathCost);
            break;

        case OamAttrRstpDesignatedBridge:
            cont->length = sizeof(RstpBridgeId);
            memcpy((RstpBridgeId BULK*)&cont->value[0],
                &(RstpGetPortPriVector(bridge, port)->desigBridgeId),
                sizeof(RstpBridgeId));
            break;

        case OamAttrRstpDesignatedPort:
            ContPutU16(cont,
                RstpGetPortPriVector(bridge, port)->desigPortId);
            break;

        case OamAttrRstpPortState:
            switch(RstpGetPortState(bridge, port))
                {
                case RstpStateDiscarding:
                    ContPutU8(cont, OamRstpPortBlocking);
                    break;

                case RstpStateLearning:
                    ContPutU8(cont, OamRstpPortLearning);
                    break;

                case RstpStateForwarding:
                    ContPutU8(cont, OamRstpPortForwarding);
                    break;

                default:
                    ContPutU8(cont, OamRstpPortDisabled);
                    break;
                }
            break;

        default:
            cont->length = OamVarErrAttrUnsupported;
            break;
        }
    } /* OamGetAttrBridgePort */
#endif

////////////////////////////////////////////////////////////////////////////////
/// \brief  Get Bridge related attribute
///
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamGetBridgePortAttr (const OamVarDesc BULK* src,
                           OamVarContainer BULK* cont)
    {
#if RSTP && OAM_RSTP
    U8 FAST bridge = OamContextBridge();
    U8 FAST port = OamContextPort();

    if (port == 0xFF)
        {
        for (port = 0; port < RstpGetBridgeNumPorts(bridge); port++)
            {
            /* Create bridge port object */
            cont->branch = OamBranchNameBinding;
            cont->leaf = OamExtNameBridgePort;
            cont->length = sizeof(U16);
            ContPutU16(cont, (U16)(port + 1));
            StreamSkip(&oamParser.reply, OamContSize(cont));
            cont = NextCont(cont);
            /* Handle bridge port OAM */
            CreatCont(src, cont);
            OamGetAttrBridgePort(port, src, cont);
            if ((port + 1) < RstpGetBridgeNumPorts(bridge))
                {
                StreamSkip(&oamParser.reply, OamContSize(cont));
                cont = NextCont(cont);
                }
            }
        }
    else if ((port > 0) && (port <= RstpGetBridgeNumPorts(bridge)))
        {
        OamGetAttrBridgePort(port - 1, src, cont);
        }
    else
        {
        cont->length = OamVarErrActBadParameters;
        }
#else
    UNUSED(src);

    cont->length = OamVarErrAttrUnsupported;
#endif
    } /* OamGetBridgePortAttr */



////////////////////////////////////////////////////////////////////////////////
/// \brief  Add Teknovus sequence TLV (if necessary)
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamTekAddSeqTlv(void)
    {
    OamVarContainer BULK * cont = (OamVarContainer BULK *)oamParser.reply.cur;

    if (oamParser.isLastItem && (oamParser.seqNum == 0))
        {
        return;
        }

    cont->branch = OamBranchAttribute;
    cont->leaf = OamExtAttrSequenceNumber;
    if (oamParser.isLastItem)
        {
        ContPutU16(cont, oamParser.seqNum | 0x8000);
        }
    else
        {
        ContPutU16(cont, oamParser.seqNum);
        }

    StreamSkip(&oamParser.reply, OamContSize(cont));
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Check registration on a variable descriptor
///
/// \param  src     Variable descriptor to check
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamGetVarRegCheck(const OamVarDesc BULK * src)
    {
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Process a variable descriptor
///
/// \param  src     Variable descriptor to process
///
/// \pre    oamParser.reply MUST point to the next variable container in the
///         response frame
/// \post   oamParser.reply WILL point to the end of the last variable
///         container written to the response frame
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamGetVarProcess(const OamVarDesc BULK * src)
    {
    OamVarContainer BULK * reply = (OamVarContainer BULK *)oamParser.reply.cur;

    if (OamReplyFull(oamParser.reply.cur))
        { /* reply overflow */
       	if(oamParser.isLastItem)
       		{
        	StreamSkip(&oamParser.src, (U16)PointerDiff(src, oamParser.src.cur));
			oamParser.isLastItem = FALSE;
       		}
        return;
        }

    /* process this source request */
    reply->branch = src->branch;
    reply->leaf = src->leaf;
    
    cmsLog_notice("branch=0x%x, leaf=0x%x", src->branch, OAM_NTOHS(src->leaf));

    switch (src->branch)
        {
        case OamBranchNameBinding:
            OamTekContextSet((OamVarContainer BULK*)src, reply);
            /* In the case of all ports, the contexts will be written by the
               handler */
            if ((OamContextType() != OamCtxtBridgePort) ||
                (OamContextPort() != AllPorts))
                {
                StreamSkip(&oamParser.reply, OamContSize(reply));
                }
            break;
        
        case OamBranchAttribute:
            switch (OamContextType())
                {
                case OamCtxtBridge:
                    OamGetBridgeAttr (src, reply);
                    break;

                case OamCtxtBridgePort:
                    OamGetBridgePortAttr (src, reply);
                    break;

                default:
                    OamGetAttr ((OamVarDesc *)src, reply);
                    break;
                } /* switch context */

            StreamSkip(&oamParser.reply,
                       OamContSize(
                           (OamVarContainer BULK *)oamParser.reply.cur));
            break;

        default :
            break;
        }
    } /* OamGetVarProcess */


////////////////////////////////////////////////////////////////////////////////
/// \brief  Parse variable descriptors in a Teknovus OAM get requested
///
/// \param  varAction   Action to perform for each variable descriptor
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamGetParse(void (*varAction)(const OamVarDesc BULK *))
    {
    OamVarDesc BULK * src = (OamVarDesc BULK *)oamParser.src.cur;

    while (src->branch != OamBranchTermination)
        {
        if ((U8*)src >= PointerAdd(RxFrame, rxFrameLength))
            { /* incoming packet used up */
            break;
            }

        varAction(src);

        switch (src->branch)
            {
            case OamBranchNameBinding:
                src = (OamVarDesc BULK*) NextCont((OamVarContainer BULK*)src);
                break;

#if KT_DASAN_PMC_OAM
            case OamDasanBranchLegacy:
#endif
            case OamBranchAttribute:
                if ((OAM_NTOHS(src->leaf) == OamExtAttrPortStatThreshold) ||
                    (OAM_NTOHS(src->leaf) == OamExtAttrLinkStatThreshold))
                    {
                    src = (OamVarDesc BULK *)NextCont(
                        (OamVarContainer const BULK*)src);
                    }
                else
                    {
                    src++;
                    }
                break;

            default :
                src++;
                break;
            }
        }
    } /* OamGetParse */


////////////////////////////////////////////////////////////////////////////////
//extern
BOOL OamHandleGet(void)
    {
    OamGetParse(OamGetVarRegCheck);
    OamGetParse(OamGetVarProcess);
    /* add seqeunce TLV (if necessary) */
    OamTekAddSeqTlv();
    /* null terminate response */
    StreamWriteU8(&oamParser.reply, OamBranchTermination);

    return TRUE;

    } /* OamHandleGet */


/* end OamGet.c */

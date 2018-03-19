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
/// \file OamEpon.c
/// \brief Support for OAM "Get" operatios
///
/// This code handles setting of attributes as found in OAM frames
///
////////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include "Build.h"
#include "Alarms.h"
#include "CtcOnuUtils.h"
#include "CtcOamDiscovery.h"
#if REGISTER_NACK_BACKOFF_SUPPORT
#include "CtcAuthentication.h"
#endif
#include "PonMgrEncrypt.h"
#include "Ethernet.h"
#include "Holdover.h"
#include "EponDevInfo.h"
#include "Oam.h"
#include "OamIeee.h"
#include "OamUtil.h"
#include "PonManager.h"
#if STATS_COLLECT
#include "OamStats.h"
#endif
#include "SysInfo.h"
#include "Teknovus.h"
#include "OamEpon.h"
#include "OamOnu.h"
#include "Stream.h"

#include "PonManager.h"
#include "MacLearning.h"
#include "eponctl_api.h"
#ifdef BCM_OAM_BUILD
#include "OamOptCtrl.h"
#endif


#define OamPersMaxTlvLength    (OamMaxTlvLength - sizeof(U16))
#define DefaultBridgeName                  "br0"
#define TekReportCount         1


const OamAttrLeaf CODE ponPortStatId[11] =
    {
    OamAttrLeafIllegal,
    OamAttrLeafIllegal,
    (OamAttrLeaf)OamExtAttrRxFrameTooShort,
    OamAttrPhySymbolErrDuringCarrier,
    OamAttrOamEmulCrc8Err,
    OamAttrMacFcsErr,
    OamAttrMacFramesRxOk,
    OamAttrMacOctetsRxOk,
    OamAttrMacFramesTxOk,
    OamAttrMacOctetsTxOk,
    OamAttrMpcpMACCtrlFramesTx
    };


typedef struct
    {
    U8              time;    // Unit is 10ms
    U8              flags;
    } PACK OamTekHoldoverParams;


////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponStats :  Handle EPON stats
///
// Parameters:
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponStats (const OamVarDesc BULK* src,
    OamVarContainer BULK* cont)
    {
    StatId FAST stat;

    cont->length = OamVarErrAttrUnsupported;
    stat = OamAttrToStatId ((OamExtAttrLeaf)(OAM_NTOHS(src->leaf)));
    
    if (stat < StatIdNumStats)
        {
        switch(OamContextType())
            {
            case OamCtxtPort:
#ifdef BCM_OAM_BUILD                
                OamTekStatsGetEpon (stat, cont);
#endif
                break;

            case OamCtxtLink:
#ifdef BCM_OAM_BUILD                
                OamTekStatsGetLink(OamContextLink(), stat, cont);
#endif
                break;

            case OamCtxtLinkQueue:
                // Unsupport now!
                //OamStatsGetFifo(FifoChanEpon, stat, cont);
                break;

            default:
                break;
            }
        }
    }//OamHdlEponStats



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponPowerMonitorStats :  Handle power monitoring stats
///
// Parameters:
/// \param port     Port to query
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponPowerMonitorStats (
    OamVarContainer BULK* cont,
    const OamVarDesc BULK* src)
    {
    MultiByte64 BULK * pwrReg = (MultiByte64 BULK *) (cont->value);
    BOOL FAST rdSuccess = FALSE;
    memset((void*)pwrReg,0,sizeof(MultiByte64));

    switch (OAM_NTOHS(src->leaf))
        {
        case OamExtAttrPowerMonTemperature:
			rdSuccess = optical_temp(&pwrReg->warray[3]);    
            if(rdSuccess)
                {
                memset (pwrReg,
                    (((pwrReg->array[6] & 0x80) != 0)? 0xFF : 0),
                    sizeof(MultiByte64)-sizeof(U16));
                }
            break;
        case OamExtAttrPowerMonVcc:
            rdSuccess = optical_vcc(&pwrReg->warray[3]);
            break;
        case OamExtAttrPowerMonTxBias:
            rdSuccess = optical_bias(&pwrReg->warray[3]);
            break;
        case OamExtAttrPowerMonTxPower:
            rdSuccess = optical_txpower(&pwrReg->warray[3]);
            break;
        case OamExtAttrPowerMonRxPower:
            rdSuccess = optical_rxpower(&pwrReg->warray[3]);
            break;
        default:
            break;
        }

    cont->length = rdSuccess?
        sizeof(MultiByte64) : OamVarErrActBadParameters;

    } // OamHdlEponPowerMonitorStats




////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponFecStats :  Handle FEC stats
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponFecStats (
    OamExtAttrLeaf attr,
    OamVarContainer BULK* cont,
    OamVarDesc BULK* src)
    {
    UNUSED(src);

    switch((OamAttrLeaf)attr)

        {
        case OamAttrFecCorrectedBlocks:
            //ContPutU32 (cont, FecCorrectedBlocks ());
            break;

        case OamAttrFecUncorrectableBlocks:
            //ContPutU32 (cont, FecUncorrectableBlocks ());
            break;

        default:
            break;
        }
    } // OamHdlEponFecStats



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponFecAbility :  Handle FEC Ability
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponFecAbility (
    OamExtAttrLeaf attr,
    OamVarContainer BULK* cont,
    OamVarDesc BULK* src)
    {
    ContPutU32 (cont, FecAbilitySupported);
    UNUSED(attr);
    UNUSED(src);
    } // OamHdlEponFecAbility


////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponLenErrDiscard :  Handle length error discard
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponLenErrDiscard (
    OamExtAttrLeaf attr,
    OamVarContainer BULK* cont,
    OamVarDesc BULK* src)
    {
#if OAM_FULLSUPPORT
    ContPutU8 (cont, Tk3701DluLenErrDiscard());
#else
    UNUSED(attr);
    UNUSED(src);
    ContPutU8 (cont, 0);
#endif
    } // OamHdlEponLenErrDiscard



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponDnNewFilterTable :  Handle downstream filters
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponDnNewFilterTable (
    OamExtAttrLeaf attr,
    OamVarContainer BULK* cont,
    OamVarDesc BULK* src)
    {
    //intentionally blank
    UNUSED(attr);
    UNUSED(cont);
    UNUSED(src);
#if !OAM_FULLSUPPORT
    cont->length = OamVarErrAttrUnsupported;
#endif
    } // OamHdlEponDnNewFilterTable



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponDnFilterTable :  Handle downstream filters
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponDnFilterTable (
    OamExtAttrLeaf attr,
    OamVarContainer BULK* cont,
    OamVarDesc BULK* src)
    {
    //intentionally blank
    UNUSED(attr);
    UNUSED(cont);
    UNUSED(src);
    } // OamHdlEponDnFilterTable



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponReportThresholds :  Handle report thresholds
///
// Parameters:
/// \param cont     Location for result
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponReportThresholds (OamVarContainer BULK* cont)
    {
    U16 bcap[8] = {0};
    U8 count = 0;
    PonMgrRptMode mode = 0;
    LinkIndex link = OamContextLink();
    U16 *threshold = (U16*)cont->value;
    
    if (eponStack_CtlGetReportMode(&mode) != 0)
        {
        cont->length = OamVarErrAttrUndetermined;
        return;
        }
    
    if (mode!= RptModeFrameAligned)
        {
        cont->length = OamVarErrActBadParameters;
        return;
        }
    
    PonMgrGetBurstCap(link, &bcap[0], &count);
    cont->length = 2 * sizeof(U16);
    *threshold = bcap[0];
    threshold++;
    *threshold = 0;
    } // OamHdlEponReportThresholds



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponCVlanEthertype :  Handle alternative customer VLAN prototype ID
///
// Parameters:
/// \param cont     Location for result
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponCVlanEthertype (OamVarContainer BULK* cont)
    {
    OamExtVlanEthertype BULK * FAST vlan;

    cont->length = sizeof(OamExtVlanEthertype);
    vlan = (OamExtVlanEthertype BULK *) (cont->value);
    UNUSED(vlan);
    //vlan->ethertype = LueVlanEthertypeGet(LueVlanCustomer,
    //    (BOOL BULK*)&vlan->upEtype,
    //    (BOOL BULK*)&vlan->dnEtype);
    } // OamHdlEponCVlanEthertype



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponSVlanEthertype :  Handle alternative service VLAN prototype ID
///
// Parameters:
/// \param cont     Location for result
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponSVlanEthertype (OamVarContainer BULK* cont)
    {
    OamExtVlanEthertype BULK * FAST vlan;

    cont->length = sizeof(OamExtVlanEthertype);
    vlan = (OamExtVlanEthertype BULK *) (cont->value);
    UNUSED(vlan);
    //vlan->ethertype = LueVlanEthertypeGet(LueVlanService,
    //    (BOOL BULK*)&vlan->upEtype,
    //    (BOOL BULK*)&vlan->dnEtype);
    } // OamHdlEponSVlanEthertype



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponOnuIgmpVlan :  Handle IGMP VLAN
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponIgmpVlan (
    OamExtAttrLeaf attr,
    OamVarContainer BULK* cont,
    OamVarDesc BULK* src)
    {
    UNUSED(attr);
    UNUSED(cont);
    UNUSED(src);
    } // OamHdlEponIgmpVlan



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponPortVlanPolicy :  Handle Port VLAN Policy
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponPortVlanPolicy (
    OamExtAttrLeaf attr,
    OamVarContainer BULK* cont,
    OamVarDesc BULK* src)
    {
#if OAM_FULLSUPPORT
#if OAM_SWITCH
    TkOnuGetPortVlanPolicy (0, cont);
#endif //OAM_SWITCH
#else
    UNUSED(attr);
    ContPutU8 (cont, 0);
    UNUSED(src);
#endif
    } // OamHdlEponPortVlanPolicy



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponPortVlanMembership :  Handle Port VLAN membership
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponPortVlanMembership (
    OamExtAttrLeaf attr,
    OamVarContainer BULK* cont,
    OamVarDesc BULK* src)
    {
#if OAM_FULLSUPPORT
#if OAM_SWITCH
    TkOnuGetPortVlanMembership (0, cont);
#endif //OAM_SWITCH
#else
    UNUSED(attr);
    ContPutU8 (cont, 0);
    UNUSED(src);
#endif
    } // OamHdlEponPortVlanMembership

////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponDownBurstTolerance :  Handle downstream burst tolerance
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponDownBurstTolerance (
    OamExtAttrLeaf attr,
    OamVarContainer BULK* cont,
    OamVarDesc BULK* src)
    {
#if OAM_FULLSUPPORT
#if OAM_SWITCH
    ContPutU8 (cont, MacLearnGetDownBurstToll ());
#endif //OAM_SWITCH
#else
    UNUSED(attr);
    ContPutU8 (cont, 0);
    UNUSED(src);
#endif
    } // OamHdlEponDownBurstTolerance



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponArpReplicationDest :  Handle ARP Replication
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponArpReplicationDest (
    OamExtAttrLeaf attr,
    OamVarContainer BULK* cont,
    OamVarDesc BULK* src)
    {
#if ARP_REPLICATION
    UNUSED(attr);
    UNUSED(src);
    ContPutU16 (cont, ArpDestinationGet ((U8)0));
#else
    UNUSED(attr);
    UNUSED(cont);
    UNUSED(src);
#endif
    } // OamHdlEponArpReplicationDest



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponLacpPassthrough :  Handle LACP pass through
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponLacpPassthrough (
    OamExtAttrLeaf attr,
    OamVarContainer BULK* cont,
    OamVarDesc BULK* src)
    {
#if OAM_FULLSUPPORT
#if LACP_PASSTHROUGH
    ContPutU16(cont, LacpDnDestGet (link));
#endif //OAM_SWITCH
#else
    UNUSED(attr);
    UNUSED(cont);
    UNUSED(src);
#endif
    } // OamHdlEponLacpPassthrough



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponNvsScratchpad :  Handle NVS scratchpad
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponNvsScratchpad (
    OamExtAttrLeaf attr,
    OamVarContainer BULK* cont,
    OamVarDesc BULK* src)
    {
    UNUSED(attr);
    UNUSED(src);
    //cont->length = (U8)NvsRead(NvsRecIdScratchpad,(&cont->value[0]), 128);
    if (cont->length == 0)
        {
        // No data here, represent 0 as 0x80
        cont->length = OamVarErrNoError;
        }
    else if (cont->length == 128)
        {
        // Represent 128 bytes as 0
        cont->length = 0;
        }
    else
        {
        // for lint
        }
    } // OamHdlEponNvsScratchpad



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponPortCap :  Handle EPON port capability
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponPortCap (
    OamExtAttrLeaf attr,
    OamVarContainer BULK* cont,
    OamVarDesc BULK* src)
    {
#if OAM_FULLSUPPORT
    GetEponPortCapability (cont);
#endif
    UNUSED(attr);
    UNUSED(cont);
    UNUSED(src);
    } //  OamHdlEponPortCap



////////////////////////////////////////////////////////////////////////////////
/// \brief  Get holdover config
///
/// \param src      Source descriptor
/// \param cont     Location for result
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamTekEponHoldoverGet (OamVarDesc BULK* src,
    OamVarContainer BULK* reply)
    {
    OamTekHoldoverParams BULK* holdover =
        (OamTekHoldoverParams BULK*)reply->value;
    UNUSED(src);

    holdover->time = (U8)(HoldoverGetTime() / 10);
    holdover->flags = HoldoverGetFlags();
    reply->length = sizeof(OamTekHoldoverParams);
    } // OamHdlEponOnuHoldover


////////////////////////////////////////////////////////////////////////////////
/// \brief  Set holdover config
///
/// \param src      Source container
/// \param cont     Location for result
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamTekEponHoldoverSet (OamVarContainer BULK* src,
    OamVarContainer BULK* reply)
    {
    OamTekHoldoverParams BULK* holdover =
        (OamTekHoldoverParams BULK*)src->value;
    UNUSED(reply);

    HoldoverSetParams((U16)holdover->time * 10, 
                      (rdpa_epon_holdover_flags)holdover->flags);
    } // OamHdlEponOnuHoldover


////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponMcastGroupInfo :  Handle Mutlicast group info
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponMcastGroupInfo (
    OamExtAttrLeaf attr,
    OamVarContainer BULK* cont,
    OamVarDesc BULK* src)
    {
#if IPV6_SUPPORT
    cont = GetMcastGroupTable (cont);
#endif
    UNUSED(attr);
    UNUSED(cont);
    UNUSED(src);
    } // OamHdlEponMcastGroupInfo



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponMcastDomainConfig :  Handle Mutlicast domain config
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponMcastDomainConfig (
    OamExtAttrLeaf attr,
    OamVarContainer BULK* cont,
    OamVarDesc BULK* src)
    {
#if IPV6_SUPPORT
    McastGetDomainConfig (cont);
#endif
    UNUSED(attr);
    UNUSED(cont);
    UNUSED(src);
    } // OamHdlEponMcastDomainConfig



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponMcastGlobalConfig :  Handle Mutlicast domain config
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponMcastGlobalConfig (
    OamExtAttrLeaf attr,
    OamVarContainer BULK* cont,
    OamVarDesc BULK* src)
    {
#if IPV6_SUPPORT
    cont->length = McastGetGlobProv (cont);
#endif
    UNUSED(attr);
    UNUSED(cont);
    UNUSED(src);
    } // OamHdlEponMcastGlobalConfig


////////////////////////////////////////////////////////////////////////////////
/// OamHdlExtAttrLaserStatus:  Get the status(burst mode or continous mode)
///
// Parameters:
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlExtAttrLaserStatus(OamVarContainer BULK* cont,
    OamVarDesc BULK* src)
    {
    UNUSED(src);
    cont->length = sizeof(EponTxLaserStatus);
    if (PonMgrGetLaserBurstStatus() == LaserTxModeContinuous)
        {
        cont->value[0] = 1;
        }
    else
        {
        cont->value[0] = 0;
        }
    } // OamHdlExtAttrLaserStatus


////////////////////////////////////////////////////////////////////////////////
/// OamHdlExtAttrEponPortAdmin:  Get the state (enable/disable) of PON port
///
// Parameters:
/// \param cont     Location for result
/// \param src  Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlExtAttrEponPortAdmin(OamVarContainer BULK* cont,
    OamVarDesc BULK* src)
    {
    UNUSED(src);
    cont->length = sizeof(OamEponAdmin);
    //cont->value[0] = PonMgrGetEponPortAdmin();
    } // OamHdlExtAttrEponPortAdmin


////////////////////////////////////////////////////////////////////////////////
/// OamHdlExtAttrOamVersion:  Get the Oam oui and version for the oam running
///
// Parameters:
/// \param cont     Location for result
/// \param src  Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
//lint -e{415} access of out-of-bounds pointer
void OamHdlExtAttrOamVersion(OamVarContainer BULK* cont,
    OamVarDesc BULK* src)
    {
    OamVersion BULK * BULK verdorOamVersion;
    UNUSED(src);
    // fill the Teknovus OUI and version
    verdorOamVersion = (OamVersion BULK *)cont->value;
    memcpy((char *)verdorOamVersion->oui,(char *)&TeknovusOui,
        sizeof(TeknovusOui));
    verdorOamVersion->version = (U16)OAM_VERSION;
    verdorOamVersion++;
	
#ifdef CTC_OAM_BUILD
    // fill the Ctc OUI and version
    memcpy((char *)verdorOamVersion->oui,(char *)&CtcOui,sizeof(CtcOui));
    verdorOamVersion->version = (U16)CtcOamNegotiatedVersion();
    cont->length = sizeof(OamVersion)<<1;
#endif
    } // OamHdlExtAttrOamVersion


////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponExtFecGet:  Get the fec state
///
// Parameters:
/// \param cont     Location for result
/// \param src  Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponExtFecGet(OamVarContainer BULK* cont,
    OamVarDesc BULK* src)
    {
    FecPara fec_config;
    OamExtFECMode *fecMode = (OamExtFECMode BULK *)cont->value;
    
    UNUSED(src);
    fec_config.link = OamContextLink();
    fec_config.rx = 0;
    fec_config.tx = 0;
    if(eponStack_CtlCfgFec(EponGetOpe, &fec_config) != 0)
        {
        cont->length = OamVarErrAttrUnsupported;
        return;
        }
    
    fecMode->rxFEC= (fec_config.rx != 0)? 1 : 0;
    fecMode->txFEC= (fec_config.tx != 0)? 1 : 0;
    cont->length = sizeof (OamExtFECMode);  
    } // OamHdlEponExtFecGet


////////////////////////////////////////////////////////////////////////////////
////
////                                Set functions
////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponLenErrSet:  Handle length error discard set
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponLenErrSet(const OamVarContainer BULK* src,
    OamVarContainer BULK* reply)
    {
#if OAM_FULLSUPPORT
    Tk3701DluLenErrDiscardEnable (ContGetU8(src));
#else
    UNUSED(src);
    reply->length = OamVarErrAttrUnsupported;
#endif
    } // OamHdlEponLenErrSet



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponReportThreshSet:  Handle report threshold set
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponReportThreshSet(const OamVarContainer BULK* src,
    OamVarContainer BULK* reply)
    {
    LinkIndex FAST link = OamContextLink();
    Stream FAST strm;
    U16 FAST thresh;
    PonMgrRptMode mode;

    if (eponStack_CtlGetReportMode(&mode) != 0)
        {
        reply->length = OamVarErrAttrUndetermined;
        return;
        }
    
    if (mode!= RptModeFrameAligned)
        {
        reply->length = OamVarErrActBadParameters;
        return;
        }

    StreamInit(&strm, (U8 *)src->value);
    thresh = StreamReadU16(&strm);
    PonMgrSetBurstCap(link, &thresh, TekReportCount);
    } // OamHdlEponReportThreshSet



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponCVlanEthertypeSet:  Handle customer VLAN Ethertype set
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponCVlanEthertypeSet(const OamVarContainer BULK* src,
    OamVarContainer BULK* reply)
    {
    OamExtVlanEthertype BULK* FAST vlan;
    UNUSED(reply);

    vlan = (OamExtVlanEthertype BULK *)(src->value);
    UNUSED(vlan);
    //LueVlanEthertypeSet(LueVlanCustomer, (Ethertype)(vlan->ethertype),
    //    (0 != (vlan->upEtype)), (0 != (vlan->dnEtype)));
    } // OamHdlEponCVlanEthertypeSet



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponSVlanEthertypeSet:  Handle service VLAN Ethertype set
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponSVlanEthertypeSet(const OamVarContainer BULK* src,
    OamVarContainer BULK* reply)
    {
    OamExtVlanEthertype BULK* FAST vlan;
    UNUSED(reply);

    vlan = (OamExtVlanEthertype BULK *)(src->value);
    UNUSED(vlan);
    //LueVlanEthertypeSet(LueVlanService, (Ethertype)(vlan->ethertype),
    //    (0 != (vlan->upEtype)), (0 != (vlan->dnEtype)));
    } // OamHdlEponSVlanEthertypeSet



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponOnuIgmpVlanSet:  Handle IGMP VLAN set
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponOnuIgmpVlanSet(const OamVarContainer BULK* src,
    OamVarContainer BULK* reply)
    {
#if OAM_FULLSUPPORT
#if IPV6_SUPPORT
    if (McastGetSnoopMode())
        {
        reply->length = OamVarErrActBadParameters;
        return;
        }
#endif
    if(!IgmpSetVlanCfg(src))
        {
        reply->length = OamVarErrActBadParameters;
        }
#else
    UNUSED(src);
    reply->length = OamVarErrAttrUnsupported;
#endif
    } // OamHdlEponOnuIgmpVlanSet


////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponPortVlanPolicySet:  Handle Port VLAN policy set
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponPortVlanPolicySet(const OamVarContainer BULK* src,
    OamVarContainer BULK* reply)
    {
#if OAM_SWITCH
    TkOnuSetPortVlanPolicy (0, src, reply);
#else
    UNUSED(src);
    reply->length = OamVarErrAttrUnsupported;
#endif
    } // OamHdlEponPortVlanPolicy



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponPortVlanMembershipSet:  Handle Port VLAN membership set
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponPortVlanMembershipSet(const OamVarContainer BULK* src,
    OamVarContainer BULK* reply)
    {
#if OAM_SWITCH
    src = TkOnuSetPortVlanMembership (0, src, reply);
#else
    UNUSED(src);
    reply->length = OamVarErrAttrUnsupported;
#endif
    } // OamHdlEponPortVlanMembershipSet



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponDownBurstSet:  Handle downstream burst tolerance
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponDownBurstSet(const OamVarContainer BULK* src,
    OamVarContainer BULK* reply)
    {
#if OAM_FULLSUPPORT
    MacLearnSetDownBurstToll (ContGetU8 (src));
#else
    UNUSED(src);
    if (0 != ContGetU8(src))
        {
        reply->length = OamVarErrActBadParameters;
        }
#endif
    } // OamHdlEponDownBurstSet



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponLacpSet:  Handle LACP configuration
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponLacpSet(const OamVarContainer BULK* src,
    OamVarContainer BULK* reply)
    {
#if LACP_PASSTHROUGH
    LacpDnDestSet (link, ContGetU16(src));
#else
    UNUSED(src);
    reply->length = OamVarErrAttrUnsupported;
#endif
    } // OamHdlEponLacpSet



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponExtFecSet:  Handle FEC configuration
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponExtFecSet(
    LinkIndex FAST link,
    const OamVarContainer BULK* src,
    OamVarContainer BULK* reply)
    {
    FecPara fec_config;
    OamExtFECMode *fecMode = (OamExtFECMode BULK*)src->value;
    
    fec_config.link = link;
    fec_config.rx = (0 != fecMode->rxFEC)?1:0;
    fec_config.tx = (0 != fecMode->txFEC)?1:0;
    
    if (eponStack_CtlCfgFec(EponSetOpe, &fec_config) != 0)
        {
        reply->length = OamVarErrActBadParameters;
        }
    } // OamHdlEponExtFecSet


////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponNvsScratchpadSet:  Handle NVS scratchpad writes
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponNvsScratchpadSet(const OamVarContainer BULK* src,
    OamVarContainer BULK* reply)
    {
    UNUSED(reply);
    //
    //   If the source length is 0, this is really encoded 128.
    //   Otherwise, if the source length is > 0x80, this means 0.
    //   In the last case, use the actual number of source length.
    //
    
    //(void)NvsWrite(NvsRecIdScratchpad, (&src->value[0]),
    //    (src->length == 0) ? 128 :
    //    ((src->length >= OamVarErrNoError) ? 0 : src->length));
    } // OamHdlEponNvsScratchpadSet



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponArpReplicationSet:  Handle ARP replication setup
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponArpReplicationSet(const OamVarContainer BULK* src,
    OamVarContainer BULK* reply)
    {
#if ARP_REPLICATION
    ArpDestinationSet (0, ContGetU16(src));
    ArpCommitChanges ();
    reply->length = OamVarErrNoError;
#else
    UNUSED(src);
    reply->length = OamVarErrAttrUnsupported;
#endif
    } // OamHdlEponArpReplicationSet



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponLinkStateSet:  Handle link state setup
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponLinkStateSet(const OamVarContainer BULK* src,
    OamVarContainer BULK* reply)
    {
    LinkIndex FAST link = OamContextLink();
    // Onu is notified that a link is enabled/disabled
    switch (((OamExtLinkState BULK*)src->value)->state)
        {
        case LinkStateDisable:
            AlarmSetCondition(AlmLinkDisabled, (U8) link);
            break;

        case LinkStateEnable:
            AlarmClearCondition(AlmLinkDisabled, (U8) link);
            break;

        default:
            reply->length = OamVarErrActBadParameters;
            break;
        }
    } // OamHdlEponLinkStateSet



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponMcastDomainSet:  Handle multicast domain
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponMcastDomainSet(const OamVarContainer BULK* src,
    OamVarContainer BULK* reply)
    {
#if IPV6_SUPPORT
    if(!McastDomainAdd(src))
        {
        reply->length = OamVarErrActNoResources;
        }
#else
    UNUSED(src);
    reply->length = OamVarErrAttrUnsupported;
#endif
    } // OamHdlEponMcastDomainSet



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponMcastGlobalSet:  Handle multicast global settings
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponMcastGlobalSet(const OamVarContainer BULK* src,
    OamVarContainer BULK* reply)
    {
#if IPV6_SUPPORT
    if(!McastSetGlobProv (src))
        {
        reply->length = OamVarErrActNoResources;
        }
#else
    UNUSED(src);
    reply->length = OamVarErrAttrUnsupported;
#endif
    } // OamHdlEponMcastGlobalSet



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponMcastGroupSet:  Handle multicast group sets
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponMcastGroupSet(const OamVarContainer BULK* src,
    OamVarContainer BULK* reply)
    {
#if IPV6_SUPPORT
    OamMcastAddDelGroupInfo BULK *addDelInfo;
    U8 FAST numGroups = src->value[0];

    addDelInfo = (OamMcastAddDelGroupInfo BULK *)&(src->value[1]);

    while (numGroups && (reply->length == OamVarErrNoError))
        {
        addDelInfo = OamMcastAddDelGroup (OAM_NTOHS(src->leaf), addDelInfo, reply);
        --numGroups;
        }

#else
    UNUSED(src);
    reply->length = OamVarErrAttrUnsupported;
#endif
    }


#ifdef CTC_OAM_BUILD

////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponCtcLoidGet:  Handle CTC Loid
///
// Parameters:
/// \param cont     Location for result
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponCtcLoidGet(OamVarContainer BULK* cont)
    {
    OamCtcAuthLoid BULK *val = (OamCtcAuthLoid BULK *)(&cont->value[0]);

    cont->length = sizeof(OamCtcAuthLoid);
    UNUSED(val);
    //CtcAuthLoidGet (&val->loid[0]);
    } // OamHdlEponCtcLoidGet


////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponCtcLoidSet:  Handle CTC Loid
///
// Parameters:
/// \param src      Source descriptor
/// \param cont     Location for result
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponCtcLoidSet(const OamVarContainer BULK* src,
    OamVarContainer BULK* cont)
    {
    UNUSED(cont);
    OamCtcAuthLoid BULK *val = (OamCtcAuthLoid BULK*)(&src->value[0]);
    UNUSED(val);
    //CtcAuthLoidSet (&val->loid[0]);
    } // OamHdlEponCtcLoidSet


////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponCtcPwdGet:  Handle CTC password
///
// Parameters:
/// \param cont     Location for result
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponCtcPwdGet(OamVarContainer BULK* cont)
    {
    OamCtcAuthPwd BULK *val = (OamCtcAuthPwd BULK *)(&cont->value[0]);

    cont->length = sizeof(OamCtcAuthPwd);
    UNUSED(val);
    //CtcAuthPwdGet (&val->pwd[0]);
    } // OamHdlEponCtcPwdGet


////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponCtcPwdSet:  Handle CTC Loid
///
// Parameters:
/// \param src      Source descriptor
/// \param cont     Location for result
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponCtcPwdSet(const OamVarContainer BULK* src,
    OamVarContainer BULK* cont)
    {
    UNUSED(cont);
    OamCtcAuthPwd BULK *val = (OamCtcAuthPwd BULK*)(&src->value[0]);
    UNUSED(val);
    //CtcAuthPwdSet (&val->pwd[0]);
    } // OamHdlEponCtcPwdSet


////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponCtcLoidAuthResultGet:  Handle CTC loid auth result
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponCtcLoidAuthResultGet(OamVarContainer BULK* cont)
    {
    ContPutU8(cont, (U8)OamCtcGetAuthFailure());
    } // OamHdlEponCtcLoidAuthResultGet



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponCtcLoidAuthResultSet:  Handle CTC loid auth result
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponCtcLoidAuthResultSet(const OamVarContainer BULK* src,
    OamVarContainer BULK* cont)
    {
    UNUSED(cont);
    OamLoidAuthStauts BULK *val = (OamLoidAuthStauts BULK *)(&src->value[0]);

    OamCtcSetAuthFailure(0 != (val->result));
    } // OamHdlEponCtcLoidAuthResultSet
#endif


#if defined(EXT_CPU)
////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponBackupBatteryGet:  Handle backup battery
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponBackupBatteryGet(const OamVarContainer BULK* cont)
    {
    PersOnuOptions BULK onuOpt;

    // FIXME WGMAI
//    OntCfgGetOnuOptions(&onuOpt);
    ContPutU8(cont,(U8)(onuOpt.backupBattery));
    } // OamHdlEponBackupBatteryGet



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponBackupBatterySet:  Handle backup battery
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponBackupBatterySet(const OamVarContainer BULK* src,
    OamVarContainer BULK* reply)
    {
    PersOnuOptions BULK onuOpt;

    // FIXME WGMAI
//    OntCfgGetOnuOptions(&onuOpt);

    onuOpt.backupBattery = (0 != ContGetU8(src));
    if(!OntCfgSetOnuOptions(&onuOpt))
        {
        reply->length = OamVarErrActBadParameters;
        }
    } // OamHdlEponBackupBatterySet



////////////////////////////////////////////////////////////////////////////////
/// OamHdlExtAttrEponPortInfo:  Get the Link info for the oam running
///
// Parameters:
/// \param cont Location for result
/// \param src  Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlExtAttrEponPortInfo(OamVarContainer BULK* cont,
    OamVarDesc BULK* src)
    {
    U8 FAST i;
    OamEponPortInfo BULK *info;
    UNUSED(src);

    info = (OamEponPortInfo BULK *)&(cont->value[0]);
    info->los = (U8)(PonMgrLos());
    info->linkNum = OntCfgDbGetNumUserLinks();
    cont->length = (sizeof(OamEponPortInfo) - sizeof(OamEponLinkInfo)) +
        (info->linkNum * sizeof(OamEponLinkInfo));

    for (i = 0; i < info->linkNum; i++)
        {
        info->linkInfo[i].state     = OntmMpcpRegState(i);
        info->linkInfo[i].phyLlid   = PonMgrGetPhyLlid(i);
        }
    } // OamHdlExtAttrEponPortInfo



////////////////////////////////////////////////////////////////////////////////
/// OamHdlExtAttrPonCfgInfo:  Get the Pon config info for the oam running
///
// Parameters:
/// \param cont Location for result
/// \param src  Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlExtAttrPonCfgInfo(OamVarContainer BULK* cont,
    OamVarDesc BULK* src)
    {
    OamPonCfgInfo BULK *info;
    UNUSED(src);

    cont->length = sizeof(OamPonCfgInfo);

    info = (OamPonCfgInfo BULK *)&(cont->value[0]);
    info->pon   = PonCfgDbGetActivePon();
    info->upSpd = (OamPonSpeed)PonCfgDbGetUpRate();
    info->dnSpd = (OamPonSpeed)PonCfgDbGetDnRate();
    } // OamHdlExtAttrPonCfgInfo
#endif


////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponMpcpTimeoutGet:  Handle MPCP timeout settings
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponMpcpTimeoutGet(LinkIndex link,
    const OamVarContainer BULK* cont)
    {
    //ContPutU16(cont, OntmMpcpTimeOutGet(link));
    } // OamHdlEponMpcpTimeoutGet



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponMpcpTimeoutSet:  Handle MPCP timeout settings
///
// Parameters:
/// \param port     Port to query
/// \param attr     Attribute to retrieve
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlEponMpcpTimeoutSet(LinkIndex link,
    const OamVarContainer BULK* src,
    OamVarContainer BULK* reply)
    {
    UNUSED(reply);
    OamMpcpTimeOut BULK *val = (OamMpcpTimeOut BULK *)(&src->value[0]);
    UNUSED(val);
    //OntmMpcpTimeOutSet(link, val->time);
    } // OamHdlEponMpcpTimeoutSet


static void GetOltMac(MacAddr *oltMac)
    {
    U8 *oamHead = oamParser.src.start;

    // Move pointer to DA MAC
    oamHead -= 8;
    memcpy(oltMac, oamHead, 6); 
    }
////////////////////////////////////////////////////////////////////////////////
/// OamEponLinkLoopbackEnable:  Enable loopback per link
///
// Parameters:
/// \param link     Link to loop back
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL OamEponLinkLoopbackEnable (LinkIndex link)
    {
    BOOL loopback = TRUE;
    MacAddr linkMac;
    char    ifName[10]={0};
    BOOL ret = TRUE;
    MacAddr oltMac;
    
    memcpy(&linkMac, &EponDevBaseMacAddr, sizeof(MacAddr));
    linkMac.u8[5] += link;
#ifdef EPON_SFU   
    sprintf(ifName, "%s.%d", eponRealIfname, link+1);
#else
    if (EponDevGetOamSel() == OAM_BCM_SUPPORT) /* BCM OAM mode */
        sprintf(ifName, "%s.0", eponVeipIfname[EROUTER_VEIP_IDX]);
    else 
        sprintf(ifName, "%s.%d", eponRealIfname, link+1);        
#endif
    GetOltMac(&oltMac);    
    
    if(eponStack_CtlLinkLoopback(EponSetOpe, link, &loopback) != 0)
        {
        ret = FALSE;
        }
    else if(!MacLearnAddEntry(DefaultBridgeName, ifName, &linkMac, EntryTypeStatic))
        {
        loopback = FALSE;
        eponStack_CtlLinkLoopback(EponSetOpe, link, &loopback);
        ret = FALSE;
        }
    else
        {
          (void)MacLearnAddEntry(DefaultBridgeName, ifName, &oltMac, EntryTypeStatic);
//        AlarmSetCondition (AlmLinkLoopback, link);        
        }

    return ret;
    } // OamEponLinkLoopbackEnable



////////////////////////////////////////////////////////////////////////////////
/// OamEponLinkLoopbackDisable:  Disable loopback per link
///
// Parameters:
/// \param link     Link to remove from loopback loop back
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL OamEponLinkLoopbackDisable (LinkIndex link)
    {
    BOOL loopback = FALSE;
    MacAddr linkMac;
    char    ifName[10]={0};
    BOOL ret = TRUE;
    MacAddr oltMac;
    
    memcpy(&linkMac, &EponDevBaseMacAddr, sizeof(MacAddr));
    linkMac.u8[5] += link;
#ifdef EPON_SFU   
    sprintf(ifName, "%s.%d", eponRealIfname, link+1);
#else
    if (EponDevGetOamSel() == OAM_BCM_SUPPORT) /* BCM OAM mode */
        sprintf(ifName, "%s.0", eponVeipIfname[EROUTER_VEIP_IDX]);
    else 
        sprintf(ifName, "%s.%d", eponRealIfname, link+1);        
#endif

    GetOltMac(&oltMac);    

    (void)MacLearnDelEntry(DefaultBridgeName, ifName, &linkMac);
    (void)MacLearnDelEntry(DefaultBridgeName, ifName, &oltMac);    
    if(eponStack_CtlLinkLoopback(EponSetOpe, link, &loopback) != 0)
        {
        ret = FALSE;
        }
    else
        {            
//        AlarmClearCondition (AlmLinkLoopback, link);        
        }       

    return ret;
    } // OamEponLinkLoopbackDisable



////////////////////////////////////////////////////////////////////////////////
/// OamHdlMpcpForwardSet :  Handle mpcp forward set
///
/// Parameters:
/// \param src      Source descriptor
/// \param reply    Location for result
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlMpcpForwardSet (const OamVarContainer BULK* src,
    OamVarContainer BULK* reply)
    {
    UNUSED(reply);
    //OntmMpcpMpcpForwardSet(0 != (src->value[0]));
    } // OamHdlMpcpForwardSet


////////////////////////////////////////////////////////////////////////////////
/// OamHdlMpcpForwardGet :  Handle mpcp forward request
///
/// Parameters:
/// \param cont     Location for result
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlMpcpForwardGet (OamVarContainer BULK* cont)
    {
    //ContPutU8(cont, (U8)OntmMpcpMpcpForwardGet());
    } // OamHdlMpcpForwardGet

#ifdef BCM_OAM_BUILD
////////////////////////////////////////////////////////////////////////////////
/// OamHdlOptCtrlLaserShutdown :  Handle mpcp forward request
///
/// Parameters:
/// \param cont     Location for result
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void OamHdlOptCtrlLaserShutdown(OamVarContainer BULK* src,
                           OamVarContainer BULK* reply)
{
    UNUSED(reply);
    OamTekLaserShutDown(ContGetU16(src));
}
#endif

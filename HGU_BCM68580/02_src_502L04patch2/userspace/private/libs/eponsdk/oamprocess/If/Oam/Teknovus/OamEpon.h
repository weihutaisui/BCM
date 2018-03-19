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
/// \file OamEpon.h
/// \brief TK EPON operations functionality
///
///
///
////////////////////////////////////////////////////////////////////////////////

#if !defined(OamEpon_h)
#define OamEpon_h


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
                        OamVarContainer BULK* cont);



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
extern
void OamHdlEponPowerMonitorStats (
                        OamVarContainer BULK* cont,
                        const OamVarDesc BULK* src);




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
extern
void OamHdlEponFecStats (
                        OamExtAttrLeaf attr,
                        OamVarContainer BULK* cont,
                        OamVarDesc BULK* src);



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
extern
void OamHdlEponFecAbility (
                        OamExtAttrLeaf attr,
                        OamVarContainer BULK* cont,
                        OamVarDesc BULK* src);



///////////////////////////////////////////////////////////////////////////////
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
extern
void OamHdlEponLenErrDiscard (
                        OamExtAttrLeaf attr,
                        OamVarContainer BULK* cont,
                        OamVarDesc BULK* src);



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
extern
void OamHdlEponDnNewFilterTable (
                        OamExtAttrLeaf attr,
                        OamVarContainer BULK* cont,
                        OamVarDesc BULK* src);



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
extern
void OamHdlEponDnFilterTable (
                        OamExtAttrLeaf attr,
                        OamVarContainer BULK* cont,
                        OamVarDesc BULK* src);



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponReportThresholds :  Handle report thresholds
///
 // Parameters:
/// \param cont     Location for result
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
extern
void OamHdlEponReportThresholds (OamVarContainer BULK* cont);



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponCVlanEthertype :  Handle alternative customer VLAN prototype ID
///
 // Parameters:
/// \param cont     Location for result
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
extern
void OamHdlEponCVlanEthertype (OamVarContainer BULK* cont);



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponSVlanEthertype :  Handle alternative service VLAN prototype ID
///
 // Parameters:
/// \param cont     Location for result
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
extern
void OamHdlEponSVlanEthertype (OamVarContainer BULK* cont);



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponIgmpVlan :  Handle IGMP VLAN
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
extern
void OamHdlEponIgmpVlan (
                        OamExtAttrLeaf attr,
                        OamVarContainer BULK* cont,
                        OamVarDesc BULK* src);



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
extern
void OamHdlEponPortVlanPolicy (
                        OamExtAttrLeaf attr,
                        OamVarContainer BULK* cont,
                        OamVarDesc BULK* src);



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
extern
void OamHdlEponPortVlanMembership (
                        OamExtAttrLeaf attr,
                        OamVarContainer BULK* cont,
                        OamVarDesc BULK* src);



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponAggregateLimit :  Handle port learning limit
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
extern
void OamHdlEponAggregateLimit (
                        OamExtAttrLeaf attr,
                        OamVarContainer BULK* cont,
                        OamVarDesc BULK* src);



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponFloodUnknown :  Handle flood unknown
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
extern
void OamHdlEponFloodUnknown (
                        OamExtAttrLeaf attr,
                        OamVarContainer BULK* cont,
                        OamVarDesc BULK* src);



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
extern
void OamHdlEponDownBurstTolerance (
                        OamExtAttrLeaf attr,
                        OamVarContainer BULK* cont,
                        OamVarDesc BULK* src);



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
extern
void OamHdlEponArpReplicationDest (
                        OamExtAttrLeaf attr,
                        OamVarContainer BULK* cont,
                        OamVarDesc BULK* src);



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
extern
void OamHdlEponLacpPassthrough (
                        OamExtAttrLeaf attr,
                        OamVarContainer BULK* cont,
                        OamVarDesc BULK* src);



////////////////////////////////////////////////////////////////////////////////
/// OamHdlNEponvsScratchpad :  Handle NVS scratchpad
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
extern
void OamHdlEponNvsScratchpad (
                        OamExtAttrLeaf attr,
                        OamVarContainer BULK* cont,
                        OamVarDesc BULK* src);



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
extern
void OamHdlEponPortCap (
                        OamExtAttrLeaf attr,
                        OamVarContainer BULK* cont,
                        OamVarDesc BULK* src);



////////////////////////////////////////////////////////////////////////////////
/// \brief  Get holdover config
///
/// \param src      Source descriptor
/// \param cont     Location for result
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OamTekEponHoldoverGet (OamVarDesc BULK* src,
                            OamVarContainer BULK* reply);



////////////////////////////////////////////////////////////////////////////////
/// \brief  Set holdover config
///
/// \param src      Source container
/// \param cont     Location for result
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OamTekEponHoldoverSet (OamVarContainer BULK* src,
                            OamVarContainer BULK* reply);


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
extern
void OamHdlEponMcastGroupInfo (
                        OamExtAttrLeaf attr,
                        OamVarContainer BULK* cont,
                        OamVarDesc BULK* src);



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
extern
void OamHdlEponMcastDomainConfig (
                        OamExtAttrLeaf attr,
                        OamVarContainer BULK* cont,
                        OamVarDesc BULK* src);



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
extern
void OamHdlEponMcastGlobalConfig (
                        OamExtAttrLeaf attr,
                        OamVarContainer BULK* cont,
                        OamVarDesc BULK* src);


////////////////////////////////////////////////////////////////////////////////
/// OamHdlExtAttrLaserStatus:  Get the status(normal, alwasys on,
///                            or disable) of Tx laser
///
 // Parameters:
/// \param cont     Location for result
/// \param src      Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
extern
void OamHdlExtAttrLaserStatus(OamVarContainer BULK* cont,
                                 OamVarDesc BULK* src);


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
extern
void OamHdlExtAttrEponPortAdmin(OamVarContainer BULK* cont,
                                 OamVarDesc BULK* src);



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
extern
void OamHdlExtAttrOamVersion(OamVarContainer BULK* cont,
                                 OamVarDesc BULK* src);


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
extern
void OamHdlEponExtFecGet(OamVarContainer BULK* cont,
    OamVarDesc BULK* src);


////////////////////////////////////////////////////////////////////////////////
/// OamHdlExtAttrEponStatus:  Get the Epon status
///
// Parameters:
/// \param cont     Location for result
/// \param src  Source descriptor
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
extern
void OamHdlExtAttrEponStatus(OamVarContainer BULK* cont,
                                const OamVarDesc BULK* src);



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
extern
void OamHdlEponLenErrSet(const OamVarContainer BULK* src,
                                         OamVarContainer BULK* reply);



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
extern
void OamHdlEponReportThreshSet(const OamVarContainer BULK* src,
                                         OamVarContainer BULK* reply);



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
extern
void OamHdlEponCVlanEthertypeSet(const OamVarContainer BULK* src,
                            OamVarContainer BULK* reply);



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
extern
void OamHdlEponSVlanEthertypeSet(const OamVarContainer BULK* src,
                            OamVarContainer BULK* reply);



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
extern
void OamHdlEponOnuIgmpVlanSet(const OamVarContainer BULK* src,
                                         OamVarContainer BULK* reply);



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
extern
void OamHdlEponPortVlanPolicySet(const OamVarContainer BULK* src,
                                         OamVarContainer BULK* reply);



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
extern
void OamHdlEponPortVlanMembershipSet(const OamVarContainer BULK* src,
                                         OamVarContainer BULK* reply);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Handle local switching set
///
/// \param src      Source descriptor
/// \param reply    Location for result
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
extern
void OamHdlEponLocalSwitchingSet(const OamVarContainer BULK* src,
                                 OamVarContainer BULK* reply);



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
extern
void OamHdlEponDownBurstSet(const OamVarContainer BULK* src,
                                         OamVarContainer BULK* reply);



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
extern
void OamHdlEponLacpSet(const OamVarContainer BULK* src,
                                         OamVarContainer BULK* reply);



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
extern
void OamHdlEponExtFecSet (
                        LinkIndex FAST link,
                        const OamVarContainer BULK* src,
                        OamVarContainer BULK* reply);


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
extern
void OamHdlEponNvsScratchpadSet(const OamVarContainer BULK* src,
                                         OamVarContainer BULK* reply);



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
extern
void OamHdlEponArpReplicationSet(const OamVarContainer BULK* src,
                                         OamVarContainer BULK* reply);



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
extern
void OamHdlEponLinkStateSet(const OamVarContainer BULK* src,
                                         OamVarContainer BULK* reply);



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
extern
void OamHdlEponMcastDomainSet(const OamVarContainer BULK* src,
                                         OamVarContainer BULK* reply);



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
extern
void OamHdlEponMcastGlobalSet(const OamVarContainer BULK* src,
                                         OamVarContainer BULK* reply);



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
extern
void OamHdlEponMcastGroupSet(const OamVarContainer BULK* src,
                                         OamVarContainer BULK* reply);

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
extern
void OamHdlEponCtcLoidGet(OamVarContainer BULK* cont);


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
extern
void OamHdlEponCtcLoidSet(const OamVarContainer BULK* src,
                          OamVarContainer BULK* cont);


////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponCtcPwdGet:  Handle CTC password
///
 // Parameters:
/// \param cont     Location for result
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
extern
void OamHdlEponCtcPwdGet(OamVarContainer BULK* cont);


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
extern
void OamHdlEponCtcPwdSet(const OamVarContainer BULK* src,
                         OamVarContainer BULK* cont);


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
extern
void OamHdlEponCtcLoidAuthResultGet(OamVarContainer BULK* cont);



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
extern
void OamHdlEponCtcLoidAuthResultSet(const OamVarContainer BULK* src,
                                    OamVarContainer BULK* cont);

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
extern
void OamHdlEponBackupBatteryGet(const OamVarContainer BULK* cont);


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
extern
void OamHdlEponBackupBatterySet(const OamVarContainer BULK* src,
                                OamVarContainer BULK* reply);

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
extern
void OamHdlExtAttrEponPortInfo(OamVarContainer BULK* cont,
                                 OamVarDesc BULK* src);



////////////////////////////////////////////////////////////////////////////////
/// OamHdlExtAttrEponPortInfo:  Get the Pon config info for the oam running
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
                                 OamVarDesc BULK* src);
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
extern
void OamHdlEponMpcpTimeoutGet(LinkIndex link,
                              const OamVarContainer BULK* cont);



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
extern
void OamHdlEponMpcpTimeoutSet(LinkIndex link,
                              const OamVarContainer BULK* src,
                              OamVarContainer BULK* reply);


////////////////////////////////////////////////////////////////////////////////
/// OamEponLinkLoopbackEnable:  Enable loopback per link
///
 // Parameters:
/// \param link     Link to loop back
///
/// \return
/// TRUE/FALSE
////////////////////////////////////////////////////////////////////////////////
extern
BOOL OamEponLinkLoopbackEnable (LinkIndex link);



////////////////////////////////////////////////////////////////////////////////
/// OamEponLinkLoopbackDisable:  Disable loopback per link
///
 // Parameters:
/// \param link     Link to remove from loopback loop back
///
/// \return
/// TRUE/FALSE
////////////////////////////////////////////////////////////////////////////////
extern
BOOL OamEponLinkLoopbackDisable (LinkIndex link);


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
extern
void OamHdlMpcpForwardSet (const OamVarContainer BULK* src,
                        OamVarContainer BULK* reply);


////////////////////////////////////////////////////////////////////////////////
/// OamHdlMpcpForwardGet :  Handle mpcp forward request
///
/// Parameters:
/// \param cont     Location for result
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
extern
void OamHdlMpcpForwardGet (OamVarContainer BULK* cont);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Handle personality item get
///
/// \param  src     OAM container from request
/// \param  cont    Location for result
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void OamHdlEponPersItemGet(const OamVarContainer BULK* src,
                           OamVarContainer BULK* cont);



////////////////////////////////////////////////////////////////////////////////
/// OamHdlEponPersItemSet:  Handle personality item get
///
 // Parameters:
/// \param cont     Location for result
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
extern
void OamHdlEponPersItemSet(const OamVarContainer BULK* src,
                            OamVarContainer BULK* cont);



extern
void OamHdlOptCtrlLaserShutdown(OamVarContainer BULK* src,
                           OamVarContainer BULK* reply);

#endif // end OamEpon_h



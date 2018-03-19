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


#if !defined(McastMgmt_h)
#define McastMgmt_h
////////////////////////////////////////////////////////////////////////////////
/// \file McastMgmt.h
/// \brief IP Multicast management module
///
/// Manages the IP multicast module
///
////////////////////////////////////////////////////////////////////////////////

#include "Teknovus.h"
#include "PortCapability.h"
#include "TkDebug.h"
#include "OntmTimer.h"
#include "McastDb.h"
#include "McastTypes.h"
#include "McastVlan.h"

#if defined(__cplusplus)
extern "C" {
#endif


#define McastGrpInfoBufSize                   200
#define McastProxyFrameLength                 100

extern McastGrpInfo BULK * BULK mcastMgmtGroupInfo;

#define McastBitMap(bitIdx)         (0x1ULL << (bitIdx))
#define McastBitsOrEq(val, bitsMap) ((val) |= (bitsMap))
#define McastBitsAndEqNot(val, bitsMap) \
                                    ((val) &= ~(bitsMap))

#define McastBitsAndEq(val, bitsMap) \
                                    ((val) &= (bitsMap))


//##############################################################################
//                      Multicast Management Funtcions
//##############################################################################

#define McastMgmtValidPort(port)  (((port) < McastPortMaxNum) ? TRUE : FALSE)


////////////////////////////////////////////////////////////////////////////////
/// \brief Reset the multicast group info buffer.
///
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastMgmtGroupInfoBufReset(void);


////////////////////////////////////////////////////////////////////////////////
/// \brief Set the multicast mode configuration
///
/// \param cfg The mode configuration
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastMgmtVlanActCbHdlReg(McastVlanActCb upCb, McastVlanActCb dnCb);


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the multicast admin state
extern
BOOL McastAdminState(void);


////////////////////////////////////////////////////////////////////////////////
/// \brief set the port multicast vlans.
///
/// \param port Port to set
/// \param cfg Pointer to the multicast vlan configuration buffer
///
/// \return
/// TRUE if success, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
BOOL McastMgmtVlanSet(TkOnuEthPort port, const McastVlanPortCfg * cfg);


////////////////////////////////////////////////////////////////////////////////
/// \brief Reset the multicast management module
///
/// \Param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastMgmtModeCfg(const McastModeCfg BULK * modeCfg);


////////////////////////////////////////////////////////////////////////////////
/// \brief Reset the multicast management module
///
/// \Param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastMgmtReset(void);


////////////////////////////////////////////////////////////////////////////////
/// \brief Reset the multicast management module
///
/// \Param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastMgmtConfig(const McastParamCfg * cfg);


////////////////////////////////////////////////////////////////////////////////
/// \brief Re-Install the multicast system rules
///
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastMgmtSysRuleReInstall(void);

//##############################################################################
//                    Multicast Group Management Functions
//##############################################################################


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the masked mac address from group address(16 bytes).
///        For v4, the masked mac is the low 23 bits with other bits are zero.
///        For v6, the masked mac is the low 32 bits with other bits are zero.
///
/// \param v4       V4 or V6 multicast mac address
/// \param mac      Pointer to the mac buffer
/// \param grpAddr  Pointer to group address
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastMgmtMaskMacAddrGet(BOOL v4, MacAddr * mac, const U8 * grpAddr);


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the mac address from group address(16 bytes).
///
/// \param v4       V4 or V6 multicast mac address
/// \param mac      Pointer to the mac buffer
/// \param grpAddr  Pointer to group address
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastMgmtMacAddrGet(BOOL v4, MacAddr * mac, const U8 * grpAddr);


////////////////////////////////////////////////////////////////////////////////
/// \brief Set the mac address into group address(16 bytes) from ip address.
///
/// \param v4       V4 or V6 multicast mac address
/// \param grpAddr  Pointer to the group address
/// \param ip       Pointer to ip address
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastMgmtMacAddrSet(BOOL v4, McastGrpAddr * grpAddr, const U8 * ip);


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the ipv4 address from the group entry
///
/// \param ipv4     Pointer to the ipv4 buffer
/// \param grpAddr  Pointer to group address
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastMgmtIpv4AddrGet(IpAddr * ipv4, const U8 * grpAddr);


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the ipv6 address from the group entry
///
/// \param ipv6     Pointer to the ipv6 buffer
/// \param grpAddr  Pointer to group address
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastMgmtIpv6AddrGet(IpV6Addr * ipv6, const U8 * grpAddr);


////////////////////////////////////////////////////////////////////////////////
/// \brief Set the ip address into group address(16 bytes).
///
/// \param v4       V4 or V6 multicast mac address
/// \param grpAddr  Pointer to the group address
/// \param ip       Pointer to ip address
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastMgmtIpAddrSet(BOOL v4, McastGrpAddr * grpAddr, const U8 * ip);


////////////////////////////////////////////////////////////////////////////////
/// \brief Set to non vlan mode
///
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastMgmtVlanModeClr(void);


////////////////////////////////////////////////////////////////////////////////
/// \brief Add a multicast group for the port
///
/// \param port    Port to add
/// \param grpInfo Group information
///
/// \return
/// TRUE if success, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
BOOL McastMgmtGrpAdd(TkOnuEthPort port, const McastGrpInfo BULK * grpInfo);


////////////////////////////////////////////////////////////////////////////////
/// \brief Delete a multicast group from the port
///
/// \param port    Port to delete
/// \param grpInfo Group information
///
/// \return
/// TRUE if success, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
BOOL McastMgmtGrpDel(TkOnuEthPort port, const McastGrpInfo BULK * grpInfo);


////////////////////////////////////////////////////////////////////////////////
/// \brief Clear all the multicast group from the port
///
/// \param port    Port to clear
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastMgmtGrpClr(TkOnuEthPort port);


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the grpIdx multicast group from the port
///
/// \param port        Port to clear
/// \param startEntry  Group entry to begin
///
/// \return
/// Group Entry Pointer
////////////////////////////////////////////////////////////////////////////////
extern
McastGrpEntry * McastMgmtGrpEntryGet(TkOnuEthPort port,
                                     McastGrpEntry * startEntry);


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the grpIdx multicast group from the port
///
/// \param port        Port to clear
/// \param startEntry  Group entry to begin
/// \param grpInfo     Group information buffer
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
McastGrpEntry * McastMgmtGrpGet(TkOnuEthPort port,
                                McastGrpEntry * startEntry,
                                McastGrpInfo BULK * grpInfo);


//##############################################################################
//                      Multicast Protocol Funtcions
//##############################################################################


////////////////////////////////////////////////////////////////////////////////
/// \brief Handle the downstream multicast frame
///
/// \param link Link from which the frame is recived
/// \param vid  Vid
/// \param frm  Pointer to the multicast frame
/// \param len  The length of the multicast frame
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastDnHandle(LinkIndex link, U16 vid,
                         const McastFrame BULK * frm, U16 len);


////////////////////////////////////////////////////////////////////////////////
/// \brief Handle the upstream multicast frame
///
/// \param port Port from which the frame is recived
/// \param vid  Vid
/// \param frm  Pointer to the multicast frame
/// \param len  The length of the multicast frame
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastUpHandle(TkOnuEthPort port, U16 vid,
                        const McastFrame BULK * frm, U16 len);


////////////////////////////////////////////////////////////////////////////////
/// \brief Handle Timer expiry events
///
/// \param timerId Timer that has expired
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastHandleTimer (OntmTimerId timerId);


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the ssm source from ssm map
///
/// \param ssmMap Ssm map
///
/// \return
/// Ssm source number
////////////////////////////////////////////////////////////////////////////////
extern
U8 McastSsmSrcNum(U32 ssmMap);


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the ssm source value
///
/// \param srcId Ssm source id.
///
/// \return
/// The pointer to the source.
////////////////////////////////////////////////////////////////////////////////
extern
SsmSrcEntry * McastSsmSrcGet(SsmSrcId srcId);


////////////////////////////////////////////////////////////////////////////////
/// \brief Check the ipv4 group mac address
///
/// \Param mac Pointer to the mac address
///
/// \return
/// TRUE if valid, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
BOOL McastGrpMacV4AddrCheck(const U8 BULK * mac);


////////////////////////////////////////////////////////////////////////////////
/// \brief Check the ipv6 group mac address
///
/// \Param mac Pointer to the mac address
///
/// \return
/// TRUE if valid, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
BOOL McastGrpMacV6AddrCheck(const U8 BULK * mac);


////////////////////////////////////////////////////////////////////////////////
/// \brief Check the group mac address
///
/// \Param mac Pointer to the mac address
///
/// \return
/// TRUE if valid, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
BOOL McastGrpMacAddrCheck(const U8 BULK * mac);


////////////////////////////////////////////////////////////////////////////////
/// \brief Check the ipv4 group da ip address
///
/// \Param ipv4 Pointer to the ipv4 da ip address
///
/// \return
/// TRUE if valid, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
BOOL McastGrpDaIpV4AddrCheck(const U8 BULK * ipv4);


////////////////////////////////////////////////////////////////////////////////
/// \brief Check the ipv6 group da ip address
///
/// \Param ipv6 Pointer to the ipv6 da ip address
///
/// \return
/// TRUE if valid, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
BOOL McastGrpDaIpV6AddrCheck(const IpV6Addr BULK * ipv6);

#if defined(__cplusplus)
}
#endif

#endif // McastMgmt.h


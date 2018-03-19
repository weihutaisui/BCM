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


#if !defined(McastBaseRule_h)
#define McastBaseRule_h
////////////////////////////////////////////////////////////////////////////////
/// \file McastBaseRule.h
/// \brief IP Multicast Base, Driver Adapter Layer
///
/// In this layer, it provids the methods for adding/deleting Ip multicast group
/// and grabs the Ip multicast management frame into CPU.
///
////////////////////////////////////////////////////////////////////////////////

#include "Teknovus.h"
#include "Ethernet.h"
#include "Ip.h"
#include "PortCapability.h"
#include "Lue.h"
#include "McastDb.h"


#if defined(__cplusplus)
extern "C" {
#endif

typedef enum
    {
    SwDomainVlan,
    SwDomainSrcIp,

    SwDomainNum
    } SwitchDomainType;

typedef struct
    {
    SwitchDomainType  type;
    LueFieldSelect    field1;
    U64               value1;
    LueFieldSelect    field2;
    U64               value2;
    U16               domainId;
    } SwitchDomainInfo;

typedef enum
    {
    McastBaseCfgOptNone         = 0,
    McastBaseCfgOptHostCtrl     = (1 << 0),

    McastBaseCfgOptInvalid      = 0x80
    } McastBaseCfgOpt;

typedef struct
    {
    BOOL            ipv4;        // should support ipv4
    BOOL            ipv6;        // should support ipv6
    McastVlanType   vlanType;    // CVLAN or SVLAN
    McastBaseCfgOpt opt;
    } McastBaseCfg;

//##############################################################################
//                      Switch Domain Operation
//##############################################################################

////////////////////////////////////////////////////////////////////////////////
/// \brief Set a switch domain
///
/// \param domainInfo Information about this switch domain
/// \param install    Install or Uninstall
///
/// \return
/// TRUE if success, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
BOOL McastSwDomainCfg(const SwitchDomainInfo * domainInfo, BOOL install);

//##############################################################################
//                      Multicast Vlan Operation
//##############################################################################

////////////////////////////////////////////////////////////////////////////////
/// \brief Set rules to strip multicast vlan for the port
///
/// \param port Port to install
/// \param strip Strip or not
///
/// \return
/// TRUE if success, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
BOOL McastBaseVlanStrip(TkOnuEthPort port, BOOL strip);


////////////////////////////////////////////////////////////////////////////////
/// \brief Set rules for the multicast vlan and vlan switch
///
/// \param port Port to install
/// \param vid1 multi Vid
/// \param vid2 user Vid
/// \param add  Add or delete
///
/// \return
/// TRUE if success, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
BOOL McastBaseVlan(TkOnuEthPort port, VlanTag mVlan, VlanTag uVlan, BOOL add);

//##############################################################################
//                      Multicast Group APIs
//##############################################################################

////////////////////////////////////////////////////////////////////////////////
/// \brief Add a port to a group in ONU hardware binary table
///
/// This function adds a port to a group in the ONU hardware table.  Groups are
/// added with an domain(vid or ssmId) qualifier depending on the group
/// management module configuration.  If the table entry already exists
/// the port will be added to it, if not the entry will be created.
///
/// \param port Port to add to group
/// \param mac  Group MAC address
/// \param domain Domain for this group
///
/// \return
/// TRUE if add successfully
////////////////////////////////////////////////////////////////////////////////
extern
BOOL McastGrpBinAddMac(TkOnuEthPort port, const MacAddr BULK *mac, U16 domain);


////////////////////////////////////////////////////////////////////////////////
/// \brief Delete a port from group in ONU hardware table
///
/// This function deletes a port from a group in the ONU hardware table. If the
/// port was the last member of the group the entry will be removed from
/// hardware.
///
/// \param port Port to add to group
/// \param ip Group MAC address
/// \param domain Domain for this group
///
/// \return
/// TRUE if delete successfully
////////////////////////////////////////////////////////////////////////////////
extern
BOOL McastGrpBinDelMac (TkOnuEthPort port, const MacAddr BULK *mac, U16 domain);


////////////////////////////////////////////////////////////////////////////////
/// \brief Add a port to a group in ONU hardware table
///
/// This function adds a port to a group in the ONU hardware table.  Groups are
/// added with a domain(vid or ssmId) qualifier depending on the group
/// management module configuration.  If the table entry already exists
/// the port will be added to it. If not, the entry will be created.
///
/// \param port      Port to add to group
/// \param ipLowBits The low 32 bits of ipv4/ipv6 address
/// \param domain    Domain for this group
///
/// \return
/// TRUE if add successfully
////////////////////////////////////////////////////////////////////////////////
extern
BOOL McastGrpBinAddIp (TkOnuEthPort port, U32 ipLowBits, U16 domain);


////////////////////////////////////////////////////////////////////////////////
/// \brief Delete a port from group in ONU hardware table
///
/// This function deletes a port from a group in the ONU hardware table. If the
/// port was the last member of the group the entry will be removed from
/// hardware.
///
/// \param port      Port to add to group
/// \param ipLowBits The low 32 bits of ipv4/ipv6 address
/// \param domain    Domain for this group
///
/// \return
/// TRUE if delete successfully
////////////////////////////////////////////////////////////////////////////////
extern
BOOL McastGrpBinDelIp (TkOnuEthPort port, U32 ipLowBits, U16 domain);


////////////////////////////////////////////////////////////////////////////////
/// \brief Re-install the multicast system rules.
///
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastBaseRuleReInstall(void);


////////////////////////////////////////////////////////////////////////////////
/// \brief Configure the multicast base module
///
/// \param cfg Configuration of multicast base module
///
/// \return
/// TRUE if sucessful, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
void McastBaseConfig(const McastBaseCfg BULK * BULK cfg);


////////////////////////////////////////////////////////////////////////////////
/// \brief Initialize the multicast module data.
///
////////////////////////////////////////////////////////////////////////////////
extern
void McastBaseInit(void);


#if defined(__cplusplus)
}
#endif

#endif // McastBase.h




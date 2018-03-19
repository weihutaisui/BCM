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
/// \file OamShared.h
/// \brief OAM handlers shared between Teknovus and DPoE OAM
///
////////////////////////////////////////////////////////////////////////////////


#if !defined(OamShared_h)
#define OamShared_h


#include "Teknovus.h"
#include "Oui.h"
#include "Sff8472.h"
#include "OamUtil.h"
#include "MultiByte.h"

extern const OamRuleSpec CODE learnedMacRule;


////////////////////////////////////////////////////////////////////////////////
/// \brief  Get optical monitoring stat
///
/// \param buf  Buffer to write stat to
/// \param stat Stat to retrieve
///
/// \return
/// Length of data
////////////////////////////////////////////////////////////////////////////////
extern
U8 OamPowerMonStatGet(MultiByte64 BULK* buf, Sff8472Value stat);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Gets phy type matching current speed of port
///
/// \param port     Port to check
///
/// \return
/// PHY type code
////////////////////////////////////////////////////////////////////////////////
extern
OamPhyType GetEthPhyType (TkOnuEthPort port);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Convert MDIO autoneg attrs to attribute list
///
/// \param cont     Location for response
/// \param techs    Attributes
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void AutoNegTechMdioToList16 (OamVarContainer BULK* cont, U16 techs);


////////////////////////////////////////////////////////////////////////////////
/// AutoNegCapToPortConfig:  Convert auto-neg capability attr to PortCfg
///
 // Parameters:
/// \param cont     Source container for attribute
/// \param reply    Location for response
///
/// \return
/// PortCfg type matching specified auto-neg capabilities
////////////////////////////////////////////////////////////////////////////////
extern
void GetPortCapability (TkOnuEthPort port,
                        OamVarContainer BULK* cont);


////////////////////////////////////////////////////////////////////////////////
/// AppendGigeAutoNegTechMdio16:  Convert gig MDIO autoneg attrs to OAM attr list
///
 // Parameters:
/// \param cont     Location for response
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void AppendGigEAutoNegTechMdio16 (OamVarContainer BULK* cont, U16 techs);


////////////////////////////////////////////////////////////////////////////////
/// AutoNegCapToPortConfig:  Convert auto-neg capability attr to PortCfg
///
 // Parameters:
/// \param cont     Source container for attribute
/// \param reply    Location for response
///
/// \return
/// PortCfg type matching specified auto-neg capabilities
////////////////////////////////////////////////////////////////////////////////
extern
TkOnuEthPortCfg AutoNegCapToPortConfig (OamVarContainer BULK* cont,
                                        OamVarContainer BULK* reply);


////////////////////////////////////////////////////////////////////////////////
/// AutoNegCapToPortConfig:  Convert auto-neg capability attr to PortCfg
///
 // Parameters:
/// \param cont     Source container for attribute
/// \param reply    Location for response
///
/// \return
/// PortCfg type matching specified auto-neg capabilities
////////////////////////////////////////////////////////////////////////////////
extern
BOOL SetPortCapability (TkOnuEthPort port,
                        OamVarContainer const BULK* cont);


////////////////////////////////////////////////////////////////////////////////
/// EthConfigSet:  Sets Ethernet port config bits
///
 // Parameters:
/// \param port     Port to affect
/// \param cfg      Config bits to add to current config
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void EthConfigSet (TkOnuEthPort port, TkOnuEthPortCfg cfg);


////////////////////////////////////////////////////////////////////////////////
/// EthConfigClr:  Clears Ethernet port config bits
///
 // Parameters:
/// \param port     Port to affect
/// \param cfg      Config bits to remove from current config
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void EthConfigClr (TkOnuEthPort port, TkOnuEthPortCfg cfg);


////////////////////////////////////////////////////////////////////////////////
/// EthPortEnable:  Enable specified Ethernet port
///
 // Parameters:
/// \param port     Port to enable
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void EthPortEnable (TkOnuEthPort port);


////////////////////////////////////////////////////////////////////////////////
/// EthPortDisable:  Disable specified Ethernet port
///
 // Parameters:
/// \param port     Port to disable
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void EthPortDisable (TkOnuEthPort port);


////////////////////////////////////////////////////////////////////////////////
/// IsOamMaster:  Is interface being referred to the OAM Master?
///
 // Parameters:
/// \param port     Port to ascertain (1-based number)
///
/// \return
/// TRUE if OAM Master, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
BOOL IsOamMaster (TkOnuEthPort port);


////////////////////////////////////////////////////////////////////////////////
/// OamMcastAddDelGroup:  Administratively Add or Delete Multicast Group
///
 // Parameters:
/// \param leaf     Leaf indicating whether this is a Add or Del OAM
/// \param src      Container with the Add/Delete OAM request
/// \param reply    Response Container
///
/// \return
/// Pointer to End of Group Information (start of next Group)
////////////////////////////////////////////////////////////////////////////////
extern
OamMcastAddDelGroupInfo BULK * OamMcastAddDelGroup (
    U16 leaf,
    OamMcastAddDelGroupInfo BULK * src,
    OamVarContainer BULK *reply);


////////////////////////////////////////////////////////////////////////////////
/// ActGpioGet:  Gets GPIO register value
///
 // Parameters:
/// \param srcGpio  GPIO request structure for register to read
/// \param reply    OAM container for reply
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void ActGpioGet (OamGpioValue const BULK* srcGpio,
                 OamVarContainer BULK* reply);


////////////////////////////////////////////////////////////////////////////////
/// ActGpioSet:  Sets GPIO register value
///
 // Parameters:
/// \param port     Port to query
/// \param srcMdio  GPIO request structure for register to read
/// \param reply    OAM container for reply
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void ActGpioSet (OamGpioValue const BULK* srcGpio,
                 OamVarContainer BULK* reply);


////////////////////////////////////////////////////////////////////////////////
/// ActMdioGet:  Gets MDIO register value
///
 // Parameters:
/// \param port     Port to query
/// \param srcMdio  MDIO request structure for register to read
/// \param reply    OAM container for reply
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void ActMdioGet (Tk3701MdioPhy mdio,
                 OamMdioValue const BULK* srcMdio,
                 OamMdioValue BULK* dstMdio);


////////////////////////////////////////////////////////////////////////////////
/// ActMdioSet:  Sets MDIO register value
///
 // Parameters:
/// \param port     Port to query
/// \param srcMdio  MDIO request structure for register to read
/// \param reply    OAM container for reply
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void ActMdioSet (Tk3701MdioPhy mdio,
                 OamMdioValue const BULK* srcMdio,
                 OamMdioValue BULK* dstMdio);


////////////////////////////////////////////////////////////////////////////////
///  OamSetOnuStaticMacTbl: Add a list of static mac addresses to the table
///
 // Parameters:
/// \param src  Source container for configuration
/// \param cont Container to place information
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OamSetOnuStaticMacTbl (OamVarContainer BULK *src,
    OamVarContainer BULK *cont);



////////////////////////////////////////////////////////////////////////////////
/// \brief  Get statically learned MAC addresses
///
/// Creates multiple sequential containers, starting at the given container,
///
/// \param  cont    Location for response
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void OamGetOnuStaticMacTbl (OamVarContainer BULK* cont);



////////////////////////////////////////////////////////////////////////////////
/// OamSetOnuMacLimit - Set learning table size and mode
///
/// This function configures the learning mode and the learning table size.  If
/// the ONU is in MAC access control mode then the mode is included.  If not
/// only the learning table size is included in the source message.
///
 // Parameters:
/// \param src  Source container for configuration
/// \param cont Container to place information
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OamSetOnuMacLimit (OamVarContainer BULK *src, OamVarContainer BULK *cont);



////////////////////////////////////////////////////////////////////////////////
/// \brief  Get learning table size and mode
///
/// This function fetches the learning mode and the learning table size.  If the
/// ONU is in MAC access control mode then the mode is included.  If not only
/// the learning table size is fetched.
///
/// \param  cont    Container to place information
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void OamGetOnuMacLimit (OamVarContainer BULK *cont);


#endif // OamShared.h

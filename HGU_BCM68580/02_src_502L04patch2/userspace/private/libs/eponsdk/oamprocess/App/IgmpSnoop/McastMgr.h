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

#if !defined(McastMgr_h)
#define McastMgr_h
////////////////////////////////////////////////////////////////////////////////
/// \file McastMgr.h
/// \brief Multicast group installation and removal module
/// \author Jason Armstrong
/// \date January 20, 2011
///
///
////////////////////////////////////////////////////////////////////////////////

#include "Teknovus.h"
#include "Ethernet.h"
#include "Ip.h"

typedef enum
    {
    McastNoVid = 0x0,
    McastCVlan = 0x1,
    McastSVlan = 0x2
    } McastVlan;


typedef U16                 McastIndex;
#define McastIndexNull      LueBinIndexNull
#define McastDirection      Dnstream



//##############################################################################
// L2 Multicast Functions
//##############################################################################



////////////////////////////////////////////////////////////////////////////////
/// \brief Add a port to a group in ONU hardware table
///
/// This function adds a port to a group in the ONU hardware table.  Groups are
/// added with an optional VLAN tag qualifier depending on the group management
/// module configuration.  If the table entry already exists the port will be
/// added to it, if not the entry will be created.
///
/// \param port Port to add to group
/// \param mac Group MAC address
/// \param vid Group VID or 0 if no VID.
///
/// \return
/// TRUE if port was added to group successfully
////////////////////////////////////////////////////////////////////////////////
extern
BOOL McastAddMac (Port port, const MacAddr BULK *mac, VlanId vid);



////////////////////////////////////////////////////////////////////////////////
/// \brief Delete a port from group in ONU hardware table
///
/// This function deletes a port from a group in the ONU hardware table. If the
/// port was the last member of the group the entry will be removed from
/// hardware.
///
/// \param port Port to add to group
/// \param ip Group MAC address
/// \param vid Group VID or 0 if no VID.
///
/// \return
/// TRUE if port was deleted group successfully
////////////////////////////////////////////////////////////////////////////////
extern
BOOL McastDelMac (Port port, const MacAddr BULK *mac, VlanId vid);



////////////////////////////////////////////////////////////////////////////////
/// \brief Get the next active multicast group
///
/// This function looks for the next valid mcast entry starting from the given
/// given starting point.  The starting point is included in the search.  If no
/// more entries remain the function will return McastIndexNull.  If a valid
/// entry is found the contents will be saved and the index of the next starting
/// point will be returned.
///
/// \param port Port whose membership list is being scanned
/// \param start Search starting index
/// \param mac Found MAC address container
/// \param vid VID for found entry
///
/// \return
/// Next starting index
////////////////////////////////////////////////////////////////////////////////
extern
U16 McastGetNextMac (Port port,
                     McastIndex start,
                     MacAddr BULK *mac,
                     VlanId BULK *vid);



////////////////////////////////////////////////////////////////////////////////
/// \brief Clear a port from group in ONU hardware table
///
/// This function clears a port from a group in the ONU hardware table. If the
/// port was the last member of the group the entry will be removed from
/// hardware.
///
/// \param port Port to add to group
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastClearMac (Port port);



//##############################################################################
// IP Multicast Functions
//##############################################################################



////////////////////////////////////////////////////////////////////////////////
/// \brief Add a port to a group in ONU hardware table
///
/// This function adds a port to a group in the ONU hardware table.  Groups are
/// added with an optional VLAN tag qualifier depending on the group management
/// module configuration.  If the table entry already exists the port will be
/// added to it, if not the entry will be created.
///
/// \param port Port to add to group
/// \param ip Group IP address
/// \param vid Group VID or 0 if no VID.
///
/// \return
/// TRUE if port was added to group successfully
////////////////////////////////////////////////////////////////////////////////
extern
BOOL McastAddIp (Port port, IpAddr ip, VlanId vid);



////////////////////////////////////////////////////////////////////////////////
/// \brief Delete a port from group in ONU hardware table
///
/// This function deletes a port from a group in the ONU hardware table. If the
/// port was the last member of the group the entry will be removed from
/// hardware.
///
/// \param port Port to add to group
/// \param ip Group IP address
/// \param vid Group VID or 0 if no VID.
///
/// \return
/// TRUE if port was deleted group successfully
////////////////////////////////////////////////////////////////////////////////
extern
BOOL McastDelIp (Port port, IpAddr ip, VlanId vid);



////////////////////////////////////////////////////////////////////////////////
/// \brief Get the next active multicast group
///
/// This function looks for the next valid mcast entry starting from the given
/// given starting point.  The starting point is included in the search.  If no
/// more entries remain the function will return McastIndexNull.  If a valid
/// entry is found the contents will be saved and the index of the next starting
/// point will be returned.
///
/// \param port Port whose membership list is being scanned
/// \param start Search starting index
/// \param ip Found IP address container
/// \param vid VID for found entry
///
/// \return
/// Next starting index
////////////////////////////////////////////////////////////////////////////////
extern
U16 McastGetNextIp (Port port,
                    McastIndex start,
                    IpAddr BULK *ip,
                    VlanId BULK *vid);



////////////////////////////////////////////////////////////////////////////////
/// \brief Clear a port from group in ONU hardware table
///
/// This function clears a port from a group in the ONU hardware table. If the
/// port was the last member of the group the entry will be removed from
/// hardware.
///
/// \param port Port to clear
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastClearIp (Port port);



//##############################################################################
// Initialization Functions
//##############################################################################



////////////////////////////////////////////////////////////////////////////////
/// \brief Enable/disable VLAN processing
///
/// This function enables of disables processing of VLAN tags.  It is highly
/// recommended that the multicast table be cleared when changing VLAN state
/// as existing entries may not match correctly.
///
/// \param enable Enable VLAN processing
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastVlanEnable (BOOL enable);



////////////////////////////////////////////////////////////////////////////////
/// \brief Is VLAN processing enabled
///
/// This function returns the VLAN processing enable state, TRUE if VLAN tags
/// are being processed, FALSE if they are ignored.
///
/// \return
/// Current VLAN processing state
////////////////////////////////////////////////////////////////////////////////
extern
BOOL McastVlanGetEnable (void);



////////////////////////////////////////////////////////////////////////////////
/// \brief Initialize group installation module
///
/// This function initializes hardware for downstream multicast forwarding.  The
/// multicast module can be configured to qualify groups based on MAC
/// destination address and an optional VLAN ID which can be retreived from the
/// CVLAN, SVLAN, or both with SVLAN taking higher priority.
///
/// \param vlanConfig Group qualification configuration
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastInit (McastVlan vlanConfig);



#endif // End of file McastMgr.h

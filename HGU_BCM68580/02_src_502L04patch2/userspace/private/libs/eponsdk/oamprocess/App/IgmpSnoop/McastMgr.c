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
/// \file McastMgr.c
/// \brief Multicast group installation and removal module
/// \author Jason Armstrong
/// \date January 20, 2011
///
///
////////////////////////////////////////////////////////////////////////////////

#include "Teknovus.h"
#include "Ethernet.h"
#include "UniManager.h"
#include "Lue.h"
#include "McastMgr.h"



static LueTrackingId BULK mcastTrackingId = LueTrackIdInvalid;
static const LueInstance CODE McastIpInstance =
    { McastDirection, LueInstBinary, 0 };
static const LueInstance CODE McastMacInstance =
    { McastDirection, LueInstBinary, 1 };


#define McastResultPri                  4
#define McastPortDiscValue(port)        (1 << (port))
#define McastPortDiscLMask(port)        (63 - (port))
#define McastPortDiscRMask(port)        (port)
#define McastPushDaClause()         LueCmdPushClause (LueFieldInL2Da,       \
                                                      LueOpEqual,           \
                                                      0x010000000000ULL,    \
                                                      23, 40)
#define McastEntryPortMapGet(inst, entry)   ((U8)(entry)->learnDomain)
#define McastEntryPortMapSet(inst, entry, portMap)  \
    ((entry)->learnDomain = (portMap))


//##############################################################################
// General Entry Functions
//##############################################################################



////////////////////////////////////////////////////////////////////////////////
/// \brief Compare two entrys
///
/// This function compares two multicast entries and returns TRUE if they are
/// equal.  Port destination map is not included in the comparison, the same
/// group on different ports will return as matched.
///
/// \param instance Binary table search instance
/// \param entry1 First entry to compare
/// \param entry2 Second entry to compare
///
/// \return
/// TRUE if the entries are equal
////////////////////////////////////////////////////////////////////////////////
static
BOOL McastEntryCompare (LueInstance BULK *instance,
                        const LueBinEntry BULK *entry1,
                        const LueBinEntry BULK *entry2)
    {
    if (LueBinFieldSelectGet (instance) == LueBinFieldDa)
        {
        return
            (entry1->entry.da.switchDomain == entry2->entry.da.switchDomain) &&
            (memcmp (&entry1->entry.da.macAddr,
                     &entry2->entry.da.macAddr,
                     sizeof(MacAddr)) == 0);
        }
    else if (LueBinFieldSelectGet (instance) == LueBinFieldSsmSrcId)
        {
        return
            (entry1->entry.ssm.ipDa == entry2->entry.ssm.ipDa) &&
            (entry1->entry.ssm.switchDomain == entry2->entry.ssm.switchDomain);
        }
    else
        {
        return FALSE;
        }
    } // McastEntryCompare


////////////////////////////////////////////////////////////////////////////////
/// \brief Add an entry or new port to a group in ONU hardware table
///
/// This function adds a port to a group in the ONU hardware table.  Groups are
/// added with an optional VLAN tag qualifier depending on the group management
/// module configuration.  If the table entry already exists the port will be
/// added to it, if not the entry will be created.
///
/// \param instance Binary table search instance
/// \param entry New entry to add
///
/// \return
/// TRUE if port was added to group successfully
////////////////////////////////////////////////////////////////////////////////
static
BOOL McastAdd (LueInstance BULK *instance, LueBinEntry BULK *entry)
    {
    LueBinEntry BULK old;

    LueCmdStart ();
    LueCmdUseInst (instance);
    (void)LueCmdPushEntry (entry->learnDomain,
                           entry->isStatic,
                           entry->rightMask,
                           &entry->entry);
    if ((LueCmdQuery (&old) == TRUE) &&
        McastEntryCompare (instance, entry, &old))
        {
        McastEntryPortMapSet (instance, entry,
                              (McastEntryPortMapGet (instance, entry) |
                               McastEntryPortMapGet (instance, &old)));
        (void)LueBinDeleteEntry (instance, &old, LueBinOptNone);
        }
    LueCmdStart ();
    (void)LueCmdPushEntry (entry->learnDomain,
                           entry->isStatic,
                           entry->rightMask,
                           &entry->entry);
    (void)LueCmdPushResult (LueActionSetPortDiscard,
                            McastEntryPortMapGet (instance, entry));

    if (LueCmdAdd (mcastTrackingId, McastResultPri, FALSE) == NULL)
        {
        LueBinCopyBalance (instance);
        return (LueCmdAdd (mcastTrackingId, McastResultPri, FALSE) != NULL);
        }
    return TRUE;
    } // McastAdd



////////////////////////////////////////////////////////////////////////////////
/// \brief Delete a port from group in ONU hardware table
///
/// This function deletes a port from a group in the ONU hardware table. If the
/// port was the last member of the group the entry will be removed from
/// hardware.
///
/// \param instance Binary table search instance
/// \param entry Group entry to delete
///
/// \return
/// TRUE if port was deleted group successfully
////////////////////////////////////////////////////////////////////////////////
static
BOOL McastDel (LueInstance BULK *instance, LueBinEntry BULK *entry)
    {
    LueBinEntry BULK old;
    U8 FAST oldMap;
    U8 FAST newMap;

    LueCmdStart ();
    LueCmdUseInst (instance);
    (void)LueCmdPushEntry (entry->learnDomain,
                           entry->isStatic,
                           entry->rightMask,
                           &entry->entry);
    if ((LueCmdQuery (&old) == TRUE) &&
        McastEntryCompare (instance, entry, &old))
        {
        newMap = McastEntryPortMapGet (instance, entry);
        oldMap = McastEntryPortMapGet (instance, &old);
        if ((newMap & oldMap) == 0)
            {
            return FALSE;
            }
        (void)LueBinDeleteEntry (instance, &old, LueBinOptNone);
        if ((newMap ^ oldMap) == 0)
            {
            return TRUE;
            }
        McastEntryPortMapSet (instance, entry, (newMap ^ oldMap));
        }
    else
        {
        return FALSE;
        }
    LueCmdStart ();
    (void)LueCmdPushEntry (entry->learnDomain,
                           entry->isStatic,
                           entry->rightMask,
                           &entry->entry);
    (void)LueCmdPushResult (LueActionSetPortDiscard,
                            McastEntryPortMapGet (instance, entry));

    if (LueCmdAdd (mcastTrackingId, McastResultPri, FALSE) == NULL)
        {
        LueBinCopyBalance (instance);
        return (LueCmdAdd (mcastTrackingId, McastResultPri, FALSE) != NULL);
        }
    return TRUE;
    } // McastDel



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
BOOL McastAddMac (Port port, const MacAddr BULK *mac, VlanId vid)
    {
    LueBinEntry BULK entry;

    entry.isStatic = TRUE;
    entry.rightMask = 4;

    entry.entry.da.macAddr = *mac;
    entry.entry.da.switchDomain = vid;
    entry.entry.da.linkId = 0;

    McastEntryPortMapSet (&McastMacInstance, &entry, (U16)(1U << port.inst));

    return McastAdd (&McastMacInstance, &entry);
    } // McastAddMac



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
BOOL McastDelMac (Port port, const MacAddr BULK *mac, VlanId vid)
    {
    LueBinEntry BULK entry;

    entry.isStatic = TRUE;
    entry.rightMask = 4;

    entry.entry.da.macAddr = *mac;
    entry.entry.da.switchDomain = vid;
    entry.entry.da.linkId = 0;

    McastEntryPortMapSet (&McastMacInstance, &entry, (U16)(1U << port.inst));

    return McastDel (&McastMacInstance, &entry);
    } // McastDelMac



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
U16 McastGetNextMac (Port port,
                     McastIndex start,
                     MacAddr BULK *mac,
                     VlanId BULK *vid)
    {
    LueBinEntry BULK entry;

    start = LueBinGetNextEntry (&McastMacInstance, start, &entry);
    while (start != LueBinIndexNull)
        {
        if ((McastEntryPortMapGet (&McastMacInstance, &entry) &
             (1U << port.inst)) != 0)
            {
            memcpy (mac, &entry.entry.da.macAddr, sizeof(MacAddr));
            *vid = entry.entry.da.switchDomain;
            return start;
            }
        start = LueBinGetNextEntry (&McastMacInstance, start, &entry);
        }
    return start;
    } // McastGetNext



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
void McastClearMac (Port port)
    {
    McastIndex FAST index;
    MacAddr BULK mac;
    VlanTag FAST vid;

    index = McastGetNextMac (port, 0, &mac, &vid);
    while (index != McastIndexNull)
        {
        (void)McastDelMac (port, &mac, vid);
        index = McastGetNextMac (port, 0, &mac, &vid);
        }
    }//McastClearMac



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
BOOL McastAddIp (Port port, IpAddr ip, VlanId vid)
    {
    LueBinEntry BULK entry;

    if(LueBinFieldSelectGet(&McastIpInstance) != LueBinFieldSsmSrcId)
        {
        return FALSE;
        }

    entry.isStatic = TRUE;
    entry.rightMask = 0;

    entry.entry.ssm.ipDa = ip;
    entry.entry.ssm.ssm = 0;
    entry.entry.ssm.switchDomain = vid;

    McastEntryPortMapSet (&McastIpInstance, &entry, (U16)(1U << port.inst));

    return McastAdd (&McastIpInstance, &entry);
    } // McastAddIp



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
BOOL McastDelIp (Port port, IpAddr ip, VlanId vid)
    {
    LueBinEntry BULK entry;

    if(LueBinFieldSelectGet(&McastIpInstance) != LueBinFieldSsmSrcId)
        {
        return FALSE;
        }

    entry.isStatic = TRUE;
    entry.rightMask = 0;

    entry.entry.ssm.ipDa = ip;
    entry.entry.ssm.ssm = 0;
    entry.entry.ssm.switchDomain = vid;

    McastEntryPortMapSet (&McastIpInstance, &entry, (U16)(1U << port.inst));

    return McastDel (&McastIpInstance, &entry);
    } // McastDelIp



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
U16 McastGetNextIp (Port port,
                    McastIndex start,
                    IpAddr BULK *ip,
                    VlanId BULK *vid)
    {
    LueBinEntry BULK entry;

    if(LueBinFieldSelectGet(&McastIpInstance) != LueBinFieldSsmSrcId)
        {
        return LueBinIndexNull;
        }

    start = LueBinGetNextEntry (&McastIpInstance, start, &entry);
    while (start != LueBinIndexNull)
        {
        if ((McastEntryPortMapGet (&McastIpInstance, &entry) &
             (1U << port.inst)) != 0)
            {
            *ip = entry.entry.ssm.ipDa;
            *vid = entry.entry.ssm.switchDomain;
            return start;
            }
        start = LueBinGetNextEntry (&McastIpInstance, start, &entry);
        }
    return start;
    } // McastGetNextIp



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
void McastClearIp (Port port)
    {
    McastIndex FAST index;
    IpAddr BULK ip;
    VlanTag FAST vid;

    if(LueBinFieldSelectGet(&McastIpInstance) != LueBinFieldSsmSrcId)
        {
        return;
        }

    index = McastGetNextIp (port, 0, &ip, &vid);
    while (index != McastIndexNull)
        {
        (void)McastDelIp (port, ip, vid);
        index = McastGetNextIp (port, 0, &ip, &vid);
        }
    } // McastClearIp



//##############################################################################
// Initialization Functions
//##############################################################################



static BOOL mcastVlanEnable;        //< Are VLAN tags matched in with group



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
void McastVlanEnable (BOOL enable)
    {
    if (enable != mcastVlanEnable)
        {
        LueCmdSetInst (McastDirection, LueInstCommon, 0);
        LueCmdStart ();
        (void)McastPushDaClause ();
        (void)LueCmdPushResult (LueActionSetSwitchDomain, 0);

        if (enable)
            {
            (void)LueCmdDel (mcastTrackingId, 13, FALSE);
            }
        else
            {
            (void)LueCmdAdd (mcastTrackingId, 13, FALSE);
            }

        mcastVlanEnable = enable;
        }
    } // McastVlanEnable



////////////////////////////////////////////////////////////////////////////////
/// \brief Is VLAN processing enabled
///
/// This function returns the VLAN processing enable state, TRUE if VLAN tags
/// are being processed, FALSE if they are ignored.
///
/// \return
/// Current VLAN processing state
////////////////////////////////////////////////////////////////////////////////
BOOL McastVlanGetEnable (void)
    {
    return mcastVlanEnable;
    } // McastVlanGetEnable



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
void McastInit (McastVlan vlanConfig)
    {
    // Grab a tracking ID for the first call or wipe out existing rules if the
    // module is being reinitialized.
    if (mcastTrackingId == LueTrackIdInvalid)
        {
        //mcastTrackingId = LueTrackIdCreate ();
        }
    else
        {
        (void)LueRuleDelByGroup (mcastTrackingId);
        }

    //--------------------------------------------------------------------------
    // Common rules
    //--------------------------------------------------------------------------
    LueCmdSetInst (McastDirection, LueInstCommon, 0);

    // If CVLAN tagged and multicast copy VID to switching domain
    if ((vlanConfig & McastCVlan) != 0)
        {
        LueCmdStart ();
        (void)LueCmdPushClause (LueFieldInL2CVlan, LueOpFieldExist, 0ULL, 0, 0);
        (void)LueCmdPushResult (LueActionCpySwitchDomain,
                                LueResultCopy(LueFieldInL2CVlan, 0));
        (void)LueCmdAdd (mcastTrackingId, 15, FALSE);
        }

    // If SVLAN tagged and multicast copy VID to switching domain
    if ((vlanConfig & McastSVlan) != 0)
        {
        LueCmdStart ();
        (void)LueCmdPushClause (LueFieldInL2SVlan, LueOpFieldExist, 0ULL, 0, 0);
        (void)LueCmdPushResult (LueActionCpySwitchDomain,
                                LueResultCopy(LueFieldInL2SVlan, 0));
        (void)LueCmdAdd (mcastTrackingId, 14, FALSE);
        }

    //--------------------------------------------------------------------------
    // Search intialization
    //--------------------------------------------------------------------------
    LueBinFieldSelectSet (&McastMacInstance, LueBinFieldDa);
    LueBinFieldSelectSet (&McastIpInstance, LueBinFieldSsmSrcId);
    mcastVlanEnable = TRUE;
    } // McastInit



// End of file McastMgr.c

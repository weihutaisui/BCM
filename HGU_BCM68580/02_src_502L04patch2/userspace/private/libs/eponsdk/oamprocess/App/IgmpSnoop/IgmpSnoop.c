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
/// \file IgmpSnoop.c
/// \brief IGMP Snooping for TK37xx ONUs
///
/// This file contains code and data structures for snooping IGMP frames in
/// the ONU data path, and adjusting forwarding rules for corresponding
/// Ethernet multicast frames appropriately.
///
////////////////////////////////////////////////////////////////////////////////

#include <string.h>     // memcpy

#include "Teknovus.h"
#include "Ip.h"
#include "Igmp.h"
#include "OamUserRules.h"
#include "EponDevInfo.h"
#include "Tk37Onu.h"
#include "OntMgr.h"
#include "OntmConfig.h"
#include "OntmMpcp.h"
#include "NvsRecordId.h" // NvsRecIdIgmp
#include "NvsTask.h" // NvsRead/NvsWrite
#include "FifoMap.h"
#include "EthFrameUtil.h"
#include "IgmpSnoop.h"
#include "MldSnoop.h"
#include "IgmpRules.h"
#include "UserPortMap.h"
#include "PolledTimer.h"
#if !OAM_NTT_AS
#include "CtcOnuOam.h"
#else
// I know this ugly. To be cleaned later. Presently could not care less...
BOOL CtcMcastIsFastLeaveEnabled (void) {return FALSE;}
void EthFrameAddDefaultVlanTag (TkOnuEthPort port, VlanTag vid)
 { UNUSED(port); UNUSED(vid); }
BOOL ValidPortVid (TkOnuEthPort port,VlanTag vid)
 { UNUSED(port); UNUSED(vid); return TRUE; }
void EthFrameDelVlanTag (TkOnuEthPort port) { UNUSED(port); }
VlanTag CtcVlanGetPortDefaultVid (TkOnuEthPort port) { return (VlanTag)port; }
#endif // !OAM_NTT_AS

/* #define QueryTimePad     2   // 10 ms ticks */

#define MaxLmqc             12  // 32767 ticks / (255 * 10) timeout interval
#define DefaultLmqc          2  // per RFC 2236
#define MaxRobustness       12  // match max LMQC

/* Igmp timer unit is 100ms, not 10ms */
#define DefaultLastMemQueryInterval     (10)  // 1 second
#define DefaultLastMemQueryResponse     (80)
#define DefaultQueryInterval            (1250)  // 125 second

#define IgmpLargeTimerValue ((MaxLmqc * 255) + 1)

#define IgmpDebugLevel  OntmDebugLo


//lint -e720 BOOLean test of assignment

#if IPV6_SUPPORT
static IgmpFwd BULK * igmpDn;
extern McastGroup BULK mcastGroupTable[MaxMcastGroups];
#else
IgmpFwd BULK igmpDn[IgmpMaxGroups];
#endif

// Declared in OntOamCommon.c
extern U16 BULK oamOuisReceived;

static bit doneDluInit = 0;        // indicates if DLU has been initialized
static bit igmpVlanCfgValid = 0;   // is IGMP VLAN config valid?

U8 BULK ethPortIgmpCount[TkOnuNumUserPorts]; // # groups port is part of

#if IPV6_SUPPORT
extern U8 FAST activeGroupCount; // # of groups currently active
#define activeIgmpGroupCount activeGroupCount
#else
U8 FAST activeIgmpGroupCount;
#endif

U8 BULK ethPortIgmpRateLimit[TkOnuNumUserPorts]; // max frames/sec per port
                                                 // 0 == no limit
U8 BULK currentIgmpCredits[TkOnuNumUserPorts]; // current credits per port
U8 BULK igmpRateLimitCounter; // counts timer calls

NvsIgmpRecord   BULK igmpCurrCfg;   // Current IGMP configuration
static IgmpVlanRecord   BULK igmpVlanCfg;   // Current IGMP VLAN Configuration

IgmpLeaveForwarding BULK igmpLeaveForwarding;

#if KT_DASAN_PMC_OAM
extern U8 BULK dasanIgmpGroupCount;
extern BOOL BULK dasanIgmpDisabled;
#endif

U8 BULK curIgmpMaxGroups;



////////////////////////////////////////////////////////////////////////////////
/// EntryInUse:  Test for IGMP entry being in use
///
 // Parameters:
/// \param entry    Forwarding table entry to query
///
/// \return
/// TRUE if entry is in use; FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
static
BOOL EntryInUse(IgmpFwd const BULK* entry)
    {
    return entry->ruleId != CfgRuleNotFound;
    } // EntryInUse



////////////////////////////////////////////////////////////////////////////////
/// AllocEntry:  Finds free or existing group in IGMP tracking table
///
 // Parameters:
/// \param  None
///
/// \return
/// Matching entry in table; out of bounds if not found
////////////////////////////////////////////////////////////////////////////////
static
IgmpFwd BULK* AllocEntry (void)
    {
    U8              FAST i;
    IgmpFwd BULK*   FAST entry = igmpDn;

    i = curIgmpMaxGroups;
    do
        {
        if (!EntryInUse(entry))
            { // found it
            activeIgmpGroupCount++;
            return entry;
            }

        entry++;
        }
    while ((--i) > 0);

    return NULL;
    } // AllocEntry



////////////////////////////////////////////////////////////////////////////////
/// FreeEntry:  Sets entry to unused state
///
 // Parameters:
/// \param entry    Entry to free
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void FreeEntry (IgmpFwd BULK* entry)
    {
    memset (entry, 0, sizeof(IgmpFwd));
    entry->vid = IgmpNoVid;
    entry->ruleId = CfgRuleNotFound;
    } // FreeEntry



////////////////////////////////////////////////////////////////////////////////
/// FindGroup:  Finds a given group in an IGMP tracking table
///
 // Parameters:
/// \param vid      VID qualifier for the group to find
/// \param addr     IP group address to find
///
/// \return
/// Matching entry in table; NULL if not found
////////////////////////////////////////////////////////////////////////////////
static
IgmpFwd BULK* FindGroup (U16 vid, IpAddr BULK* addr)
    {
    U8              FAST count = activeIgmpGroupCount;
    IgmpFwd BULK*   FAST entry = igmpDn;

    while (count > 0)
        {
        if (EntryInUse(entry))
            {
            count--;
            //if host control mode and fast leave mode. Only check the the
            //lower 23 bits of the IP address.
            if (!IgmpTrueSnoop())
                {
                if ((entry->ipAddr & 0x7fffffUL) == ((*addr) & 0x7fffffUL))
                    {
                     return entry;
                    }
                }
            else if (0 == memcmp (&(entry->ipAddr), addr, sizeof(IpAddr)))
                { // potential match
                if (((igmpCurrCfg.grpFwdQual & IgmpAddGroupByVid) == 0) ||
                    (vid == entry->vid))
                    {
                    return entry;
                    }
                }
            else { /* lint */ }
            }

        entry++;
        }

    return NULL;
    } // FindGroup



#if BCM53125

////////////////////////////////////////////////////////////////////////////////
/// IgmpQState: return queue needed for group based on port states
///
 // Parameters:
/// \param entry    Forwarding table entry to query
///
/// \return
/// Queue number to use. TkOnuNumDnQueues if none
////////////////////////////////////////////////////////////////////////////////
//extern
TkOnuDnQset IgmpQState (IgmpFwd const BULK* entry)
    {
    MultiByte16 FAST queues;
    U8 FAST i;

    //
    // All downstream IGMP goes to the WAN (Gbe port which is port 0)
    //
    queues.bytes.lsb = TkOnuNumDnQueues;
    queues.bytes.msb = TkOnuNumDnQueues;

    for (i = 0; i < TkOnuNumUserPorts; i++)
        {
        if (entry->uniUse[i] != 0)
            {
            queues.bytes.msb = PortNthDnQueue(0, igmpCurrCfg.portCfg[0].dnQ);
            break;
            }
        }

    return (TkOnuDnQset)queues.u16;
    } // IgmpQState

#else

////////////////////////////////////////////////////////////////////////////////
/// IgmpQState: return queue needed for group based on port states
///
 // Parameters:
/// \param entry    Forwarding table entry to query
///
/// \return
/// Queue number to use. TkOnuNumDnQueues if none
////////////////////////////////////////////////////////////////////////////////
//extern
TkOnuDnQset IgmpQState (IgmpFwd const BULK* entry)
    {
    MultiByte16 FAST queues;

    queues.bytes.msb = TkOnuNumDnQueues;
    queues.bytes.lsb = TkOnuNumDnQueues;

    if (entry->uniUse[0] > 0)
        {
        queues.bytes.msb = PortNthDnQueue(0, igmpCurrCfg.portCfg[0].dnQ);
        }

    if (entry->uniUse[1] > 0)
        {
        queues.bytes.lsb = PortNthDnQueue(1, igmpCurrCfg.portCfg[1].dnQ);
        }

    return (TkOnuDnQset)queues.u16;
    } // IgmpQState
#endif



////////////////////////////////////////////////////////////////////////////////
/// DnFwdSet:  Set downstream forwarding rule
///
 // Parameters:
/// \param rule     Rule to change
/// \param id       Rule id for this group
/// \param queue    Queue for rule
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void DnFwdSet (Tk3701DluRule BULK* rule, Tk3701DluRuleId id, TkOnuDnQset qset)
    {
    (void)TkOnuRuleSetQDest (rule, qset);   // update dest queue in rule

    if (IgmpAutoFwd())
        {
        TkSetTrafficRule (rule, TRUE);  // Set Clear Discard bit
        }

    Tk3701DluSetRule (id, rule);    // add rule to DLU
    } // DnFwdSet



////////////////////////////////////////////////////////////////////////////////
/// GetIgmpVidEntry:  Get IGMP VID entry
///
 // Parameters:
/// \param vid  VID, whose entry is desired
///
/// \return
/// Index of the entry IGMP VLAN config or 0xFF if not found.
////////////////////////////////////////////////////////////////////////////////
static
U8 GetIgmpVidEntry(U16 vid)
    {
    U8  FAST i;
    U8  FAST result = 0xFF;


    if (igmpVlanCfgValid)
        {
        // Treat frames with no VLAN tag as frames with VID=0
        if (vid == IgmpNoVid)
            {
            vid = 0;
            }
        vid &= 0xFFF;

        for (i = 0; i < igmpVlanCfg.nvsConfig.numVlans; ++i)
            {
            if (vid == igmpVlanCfg.nvsConfig.vlanCfg[i].eponVid)
                {
                result = i;
                break;
                }
            }
        }

    return result;
    } // GetIgmpVidEntry



////////////////////////////////////////////////////////////////////////////////
/// RemoveDnEntry:  remove given down table entry
///
/// Validation should be performed by callers
///
/// WARNING: This function should NOT be called for a group that is already
///          free.
///
 // Parameters:
/// \param entry    Entry to remove
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void RemoveDnEntry (IgmpFwd BULK* entry)
    {
    U8 FAST vlanIndex = GetIgmpVidEntry(entry->vid);

    // decrement the active groups count
    activeIgmpGroupCount--;
    if (vlanIndex < TkOnuMaxIgmpVlan)
        {
        --(igmpVlanCfg.activeCount[vlanIndex]);
        }

    if (EntryInUse(entry))
        {
        RuleDnFree (entry->ruleId);
        }

    // reset entry to default values
    FreeEntry (entry);
    } // RemoveDnEntry



////////////////////////////////////////////////////////////////////////////////
/// IgmpAddDnGroup:  Add given IP multicast group to downstream table
///
 // Parameters:
/// \param port     Port which is joining group
/// \param vid      VID for which this group needs to be added
/// \param frame    IGMP frame with information about the group to add
///
/// \return
/// TRUE if Add succeeded; FALSE if failed
////////////////////////////////////////////////////////////////////////////////
static
BOOL IgmpAddDnGroup (TkOnuEthPort port, U16 vid, IgmpFrame const BULK* frame)
    {
    IgmpFwd BULK*   FAST entry;
    BOOL            FAST withinPortLimit;
    BOOL            FAST result = FALSE;

    if (ethPortIgmpCount[port] > igmpCurrCfg.portCfg[port].groupLimit)
        {
        ethPortIgmpCount[port] = 0;
        }
    
    // store the flag that says if the group count for this port
    // is within the limits specified in Pers Flash
    withinPortLimit = (ethPortIgmpCount[port] <
                        igmpCurrCfg.portCfg[port].groupLimit);

    entry = FindGroup (vid, &(frame->groupAddr));
    if (entry != NULL)
        { // found this group addr
        result = TRUE; // forward Report msg

        switch (entry->portState[port])
            {
            case IgmpStateUnused : // new join from this port
                OntmDebug (IgmpDebugLevel,
                    ("IGMP: add dn port %bu group %08lx\n",
                        port, frame->groupAddr));

                //printf("IGMP: add dn port %bu group %08lx\n",
                 //       port, frame->groupAddr);

                if (withinPortLimit)
                    {
                    entry->portState[port] = IgmpStateActive;
                    ++(ethPortIgmpCount[port]); // bump group count
                    // increment UNI use count for the port
                    ++(entry->uniUse[TkOnuGetUniPort(port)]);

                    // update forwarding rule to include this port
                    TkOnuUserPortAddMcast (&(frame->groupAddr), port, vid);
                    UpdateRuleDest(entry);
                    }
                else
                    {
                    result = FALSE; // don't forward
                    }
                break;

            case IgmpStatePending : // query response
                OntmDebug (IgmpDebugLevel,
                    ("IGMP: query resp port %bu group %08lx at %p\n",
                        port, frame->groupAddr, entry));
                // group moves to the reported state
                entry->portState[port] = IgmpStateReported;
                break;

            default : // already active or reported; ignore
                break;
            }
        } // group addr found
    else
        { // entry not found
        // This is a new entry for this VID. Check to see if we can allow
        // the join, based on VID
        U8 FAST vlanIndex = GetIgmpVidEntry (vid);

        //
        //  Validate VID against VLAN provisioning if it has been configured
        //
        if (igmpVlanCfgValid && ((vlanIndex >= TkOnuMaxIgmpVlan) ||
            (igmpVlanCfg.activeCount[vlanIndex] >=
            igmpVlanCfg.nvsConfig.vlanCfg[vlanIndex].maxGroups)))
            {
            withinPortLimit = FALSE;
            }

        if (withinPortLimit)
            {
            entry = AllocEntry ();
            if (entry != NULL)
                {
                OntmDebug (IgmpDebugLevel,
                    ("IGMP: add dn port %bu group %08lx at %p\n",
                        port, frame->groupAddr, entry));

                entry->ipAddr = frame->groupAddr;
                entry->vid = vid;

                ++(igmpVlanCfg.activeCount[vlanIndex]);

                // TODO:
                // based on V3 and MLD frame, expect to add other
                // information that is relevant to figuring out how
                // many clauses are required to establish a rule
                // that would allow this group to pass through the
                // system

                entry->ruleId = IgmpAllocDnRule (entry);
                if (entry->ruleId != CfgRuleNotFound)
                    { // if rule allocated
                    OntmDebug (IgmpDebugLevel,
                       ("IGMP: add dn rule Id %bu\n", entry->ruleId));

                    entry->portState[port] = IgmpStateActive;
                    ++(ethPortIgmpCount[port]); // bump group count
                    // increment UNI use count for the port
                    ++(entry->uniUse[TkOnuGetUniPort(port)]);

                    // create forwarding rule for this group
                    TkOnuUserPortAddMcast (&(frame->groupAddr), port, vid);
                    CreateFwdRule(entry);

                    result = TRUE; // return success
                    } // if rule allocated
                else
                    {
                    // Alloc entry would have bumped the active group count,
                    // we must undo that if we are not going to use this entry
                    // because of lack of rules.
                    RemoveDnEntry (entry);
                    }
                } // if allocated new entry
            }
        } // group addr not found

    return result;
    } // IgmpAddDnGroup



////////////////////////////////////////////////////////////////////////////////
/// IgmpUpdateRule:  Update or remove group rule based on queue usage
///
/// If no port needs this queue, it is deleted
///
 // Parameters:
/// \param entry    Entry to move
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void IgmpUpdateRule (IgmpFwd *entry)
    {
    TkOnuDnQset FAST qset;

    // check to see if the queue needs to move from being a multicast
    // queue to being a unicast queue
    qset = IgmpQState (entry);

    if (TkOnuValidQset(qset))
        { // some port still needs the group
        // modify rule to include current queue
        UpdateRuleDest(entry);
        }
    else
        { // no queue; no port needs the group
        OntmDebug (IgmpDebugLevel,
                    ("IGMP: remove addr %08lx at %p\n", entry->ipAddr, entry));
        RemoveDnEntry(entry);
        }

    } // IgmpUpdateRule




////////////////////////////////////////////////////////////////////////////////
/// IgmpClearEntry:  Clear an IGMP table entry
///
 // Parameters:
/// \param port     Port which is to be cleaned
/// \param entry    Entry to clean
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void IgmpClearEntry (TkOnuEthPort port, IgmpFwd BULK* FAST entry)
    {
    IpAddr FAST ipAddr;

    --(ethPortIgmpCount[port]); // decrement group count for the port
    // decrement UNI use count for the port
    --(entry->uniUse[TkOnuGetUniPort(port)]);

    entry->portState[port] = IgmpStateUnused;
    entry->timerVal[port] = 0; // if timer is active, stop it

#if CTC_PROXY
    entry->lastMemQueryCounter[port] = 0;
#endif

    ipAddr = entry->ipAddr; // stash the IP before we zero it out
    IgmpUpdateRule(entry); // update the rule as necessary
    DelayMs (2); // wait for pending traffic to clear
    TkOnuUserPortRemoveMcast (&ipAddr, port, entry->vid);
    } // IgmpClearEntry



////////////////////////////////////////////////////////////////////////////////
/// IgmpRemoveDnGroup:  Remove group from forwarding
///
 // Parameters:
/// \param port     Port which is leaving group
/// \param vid      VID for which group needs to be removed
/// \param igmp     IGMP frame with info on group to remove
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void IgmpRemoveDnGroup (TkOnuEthPort port, U16 vid, IgmpFrame const BULK* igmp)
    {
    IgmpFwd BULK* FAST entry = FindGroup (vid, &(igmp->groupAddr));

    if (entry != NULL)
        {
        OntmDebug (IgmpDebugLevel,
            ("IGMP Rm A:%08lx V:%04x at %p\n", *igmp, vid, entry));

        if (entry->portState[port] > IgmpStateUnused)
            { // this port participates in group
            IgmpClearEntry (port, entry);
            } // if port state != IGMP state Unused
        } // if addr found
    } // IgmpRemoveDnGroup



#if CTC_PROXY
////////////////////////////////////////////////////////////////////////////////
static U16 CheckSum (const U8 *addr, U32 count)
    {
    U32 BULK sum = 0;

    // Main summing loop
    while (count > 1)
        {
        sum = sum + *((U16 *) addr);
        addr += 2;
        count -= 2;
        }

    // Add left-over byte, if any
    if (count > 0)
        {
        sum = sum + *((U8 *) addr);
        }

    // Fold 32-bit sum to 16 bits
    while ((sum >> 16) != 0)
        {
        sum = (sum & 0xFFFF) + (sum >> 16);
        }

    return (U16)~sum;
    } // CheckSum



////////////////////////////////////////////////////////////////////////////////
static void GetMulticastMacFromIpAddr (MacAddr *MulticastMacAddr, U32 Ip)
    {
    memset (MulticastMacAddr, 0, sizeof (MacAddr));

    MulticastMacAddr->u8[0] = 0x01;
    MulticastMacAddr->u8[1] = 0x0;
    MulticastMacAddr->u8[2] = 0x5E;
    MulticastMacAddr->u8[5] = Ip & 0xFF;
    MulticastMacAddr->u8[4] = (Ip>> 8)& 0xFF;
    // Take only 23 bits of IP, not 24!
    MulticastMacAddr->u8[3] = (Ip>> 16)& 0x7F;
    } // GetMulticastMacFromIpAddr



////////////////////////////////////////////////////////////////////////////////
static void BuildAndTxQuery (TkOnuEthPort port, const IgmpFwd BULK *entry)
    {
    IgmpFrame BULK *pIgmp;  // 4 bytes option
    IpHeader BULK *pIpHead;

    memset (TxFrame, 0, 60);
    GetMulticastMacFromIpAddr ((MacAddr *)TxFrame, entry->ipAddr); // DA
    memcpy ((U8 BULK *) TxFrame + 6, EponDevBaseMacAddr, 6);   // SA
    *(U16 *)((U8 BULK *) TxFrame + 12) = EthertypeIp;

    pIgmp = (IgmpFrame BULK *)((U8 BULK *) TxFrame + 38);
    pIpHead = (IpHeader BULK *)((U8 BULK *) TxFrame + 14);

    pIpHead->sourceAddr = 0x00000000;   // S IP - not care
    pIpHead->destAddr = entry->ipAddr;  // D IP
    pIpHead->versionHdrLen = 0x46;
    pIpHead->tos = 0xC0;
    pIpHead->length = 32;
    pIpHead->id = 0;
    pIpHead->fragmentOffset = 0;
    pIpHead->ttl = 1;
    pIpHead->protocol = IpProtocolIgmp;
    *(U32 *)((U8 BULK *) TxFrame + 34) = 0x94040000;// option

    pIpHead->checksum = 0;
    // calculate the IP header checksum
    pIpHead->checksum = CheckSum ((U8 *)pIpHead, sizeof (IpHeader) + 4);

    pIgmp->type = IgmpV2V3Query;
    pIgmp->groupAddr = entry->ipAddr;
    pIgmp->maxResponseTime = DefaultLastMemQueryResponse;
    pIgmp->checksum = 0;

    // calculate the IGMP checksum
    pIgmp->checksum = CheckSum ((U8 *)pIgmp, sizeof (IgmpFrame));

    BufferDn ((EthernetFrame BULK *)TxFrame,
              (TkOnuDnDest)(TkOnuGetUniPort(port) + 1), 60);

    } // BuildAndTxQuery



#define IgmpAllRoutersIgmpAddr 0xE0000002

////////////////////////////////////////////////////////////////////////////////
static void BuildAndFwdLeave (TkOnuEthPort port, const IgmpFwd BULK *entry)
    {
    U8 BULK VlanOff;
    IgmpFrame BULK *pIgmp;
    IpHeader BULK *pIpHead;

    memset (TxFrame, 0, 60);
    VlanOff = 0;
    if (NvsRecIdExists (NvsRecIdCtcIgmp))  // CTC IGMP mode
        {
        VlanOff = 4;
        }

    pIgmp = (IgmpFrame BULK *)((U8 BULK *) TxFrame + 38 + VlanOff);   // +VLAN
    pIpHead = (IpHeader BULK *)((U8 BULK *) TxFrame + 14 + VlanOff);   // +VLAN

    // DA
    GetMulticastMacFromIpAddr ((MacAddr *)TxFrame, IgmpAllRoutersIgmpAddr);

    // SA
    GetMulticastMacFromIpAddr ((MacAddr *)((U8 BULK *) TxFrame+6),
                               entry->ipAddr);

    if (VlanOff > 0)    // VLAN
        {
        *(U16 *)((U8 BULK *) TxFrame + 12) = EthertypeVlan;

        // VLAN Tag
        *(U16 *)((U8 BULK *) TxFrame + 14) = CtcVlanGetPortDefaultVid (port);
        }

    *(U16 *)((U8 BULK *) TxFrame + 12 + VlanOff) = EthertypeIp;
    pIpHead->sourceAddr = entry->ipAddr;        // S IP - not care
    pIpHead->destAddr = IgmpAllRoutersIgmpAddr; // D IP
    pIpHead->versionHdrLen = 0x46;
    pIpHead->tos = 0xC0;
    pIpHead->length = 32;
    pIpHead->id = 0;
    pIpHead->fragmentOffset = 0;
    pIpHead->ttl = 1;
    pIpHead->protocol = IpProtocolIgmp;
    *(U32 *)((U8 BULK *) TxFrame + 34) = 0x94040000;// option
    pIpHead->checksum = 0;
    // calculate the IP header checksum
    pIpHead->checksum = CheckSum ((U8 *)pIpHead, sizeof (IpHeader) + 4);

    pIgmp->type = IgmpV2Leave;
    pIgmp->groupAddr = entry->ipAddr;
    pIgmp->checksum = 0;
    // calculate the IGMP checksum
    pIgmp->checksum = CheckSum ((U8 *)pIgmp, sizeof (IgmpFrame));

    BufferUp ((EthernetFrame BULK *)TxFrame, 0, 60);
    } // BuildAndFwdLeave
#endif



////////////////////////////////////////////////////////////////////////////////
/// IgmpProcessLeave:  Processes an upstream Leave message
///
 // Parameters:
/// \param port     Port which is leaving group
/// \param vid      VID for which group needs to be removed
/// \param igmp     IGMP frame with info on group to remove
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void IgmpProcessLeave (TkOnuEthPort port, U16 vid, IgmpFrame const BULK* igmp)
    {
    IgmpFwd BULK* FAST entry = FindGroup (vid, &(igmp->groupAddr));

    if (entry != NULL)
        {
        OntmDebug (IgmpDebugLevel,
            ("IGMP Leave: A:%08lx V:%04x at %p\n", *igmp, vid, entry));

        switch (entry->portState[port])
            {
            case IgmpStateReported:
                // When we receive the leave, it could override the previous
                // report that could have potentially originated from a
                // different client.
                // To avoid the group from being deleted and cause a glitch
                // for the client who is interested in this group, we move
                // the group back to the pending state and extend the timer
                // to an arbitrarily large value. The OLT can then re-initiate
                // the query cycle with an appropriate max response time and
                // let the network determine if the group is active or not.

            case IgmpStateActive:
                // If the group is active and we receive a leave, we pass the
                // leave on to the OLT. We go Pending, and set a large timer
                // in expectation of a query from the network Querier. That
                // query will shorten the large timer to the proper value. If
                // no network querier exists, at least the group will eventually
                // stop being forwarded.

                entry->portState[port] = IgmpStatePending;

                // set timer to a value greater than the maximum possible
                // value that can be received in a Query message
#if CTC_PROXY
                if (!(NvsRecIdExists (NvsRecIdCtcIgmp)) ||
                        !PersCtcProxyIgmpLeave ())
                    {
                    entry->timerVal[port] = IgmpLargeTimerValue;
                    }
                else
                    {
                    entry->timerVal[port] = DefaultLastMemQueryInterval;
                    // Configured Last Member Query Count
                    entry->lastMemQueryCounter[port] = igmpCurrCfg.lmqCount;

                    // last member query cycle
                    // FIXME: where did this magic number 0x5A come from?
                    entry->ifLastMemQuery[port] = 0x5a;

                    // build and send query message to the port
                    BuildAndTxQuery (port, entry);
                    }
#else
                    entry->timerVal[port] = IgmpLargeTimerValue;
#endif
                break;

            // case IgmpStatePending:
                // If the group is in pending state when we receive the leave,
                // we remain in the pending state. Any client in the same port
                // interested in the group would send a report thereby causing
                // the group to move to the reported state. Failure by any
                // other client to report would cause the group to be deleted
                // in due course.

            default:
                break; // do nothing
            } // end switch
        } // if entry found
    } // IgmpProcessLeave



////////////////////////////////////////////////////////////////////////////////
/// SetEntryPending:  Set forwarding table to pending query reply
///
 // Parameters:
/// \param entry        Entry to make pending
/// \param timerVal     Timer Count to set for this group
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void SetEntryPending (IgmpFwd BULK* entry, U16 timerVal)
    {
    U8 FAST i;

    // for each port, check if port state for this group needs to be
    // updated.
    for (i = 0; i < TkOnuNumUserPorts; ++i)
        {
        if (entry->portState[i] != IgmpStateUnused)
            { // If group is in use for this port
            if (entry->portState[i] == IgmpStateActive)
                {
                entry->portState[i] = IgmpStatePending;

#if CTC_PROXY

                if (NvsRecIdExists (NvsRecIdCtcIgmp))  // CTC IGMP mode
                    {
                    entry->ifLastMemQuery[i] = 0;    // query request from OLT
                    if (CtcMcastIsFastLeaveEnabled ()) // Fast Leave
                        {
                        entry->lastMemQueryCounter[i] = 0;
                        }
                    else
                        {
                        entry->lastMemQueryCounter[i] = DefaultLmqc;
                        }
                    }
#endif
                }

            // change the timer value only if the timer is new or if the
            // new timer value being set is smaller than the current value
            //
            // timerVal is checked against (entry->timerVal[i] - 1) and not
            // entry->timerVal[i] so that we could check for 0 timer values
            // as well. 0 - 1 would be 0xFFFF, which is a big value
            if (timerVal <= (entry->timerVal[i] - 1))
                {
                entry->timerVal[i] = timerVal;
                }
            } // Group is active for this port
        } // for each port
    } // SetEntryPending



////////////////////////////////////////////////////////////////////////////////
/// IgmpSetDnTimers:  Set downstream response timer for given group
///
 // Parameters:
/// \param maxTime  Maximum allowed response time, in 1/10ths of a second
/// \param vid      VID for this group
/// \param addr     Group address pending reply
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void IgmpSetDnTimers (U8 maxTime, U16 vid, IpAddr BULK* addr)
    {
    U8              FAST index;
    IgmpFwd BULK*   FAST entry;
    U16             FAST timeout = maxTime;


    OntmDebug (IgmpDebugLevel, ("IGMP: set timer group %08lx:%04x for %bu\n",
                *addr, vid, maxTime));

    //
    // Is this a group specific query
    //
    if (*addr > 0)
        {
        entry = FindGroup (vid, addr);

        if (entry != NULL)
            {
            // catch the case where maxTime causes timout to be 0.
            if (timeout == 0)
                {
                timeout = DefaultLmqc;
                }

            //
            // found the addr; one single entry
            //

#if CTC_PROXY
            if (!NvsRecIdExists (NvsRecIdCtcIgmp))  // non CTC IGMP mode
                {
#endif
                //
                // For the non Fast Leave configuration; use LMQC
                // else Fast Leave; use default to avoid 
                // immediate termination, also there are cases where
                // maxTime is not set so set to 1.
                //
                timeout *= (U16)(igmpCurrCfg.lmqCount > 0) ?
                               igmpCurrCfg.lmqCount : DefaultLmqc;


#if CTC_PROXY
                }
            else  // CTC IGMP mode
                {
                if (CtcMcastIsFastLeaveEnabled ()) // Fast Leave
                    {
                    timeout *= DefaultLmqc;
                    }
                else    // Non fast leave
                    {
                    timeout *= (U16)(igmpCurrCfg.lmqCount > 0) ?
                                igmpCurrCfg.lmqCount : DefaultLmqc;
                    }

                }
#endif

            SetEntryPending (entry, timeout);
            }
        }
    else
        { // else all-zeroes address; query all groups

#if CTC_PROXY
        if (!NvsRecIdExists (NvsRecIdCtcIgmp))  // non CTC IGMP mode
            {
#endif
            timeout *= (U16)igmpCurrCfg.robustnessCount;
#if CTC_PROXY
            }
        else
            {
            timeout = (U16)(DefaultQueryInterval * igmpCurrCfg.robustnessCount);
            }
#endif

        entry = igmpDn;

        index = activeIgmpGroupCount;
        while (index > 0)
            {
            if (EntryInUse(entry))
                { // set pending only for active groups with matching vid
                index--;
                if (vid == entry->vid)
            {
                SetEntryPending (entry, timeout);
                    }
                }
            entry++;
            }
        }
    } // IgmpSetDnTimers



#if UNCALLED
////////////////////////////////////////////////////////////////////////////////
/// IgmpDnDest: Determine downstream port destinations for IGMP
///
/// This routine is a placeholder for logic that has to figure out
/// VLAN-correct forwarding for IGMP based on VLAN of arrival of message.
/// Until we do multi-VID support on the ONU, this routine devolves to
/// forwarding to any active port.
///
 // Parameters:
/// \param link     Link on which IGMP frame was received
///
/// \return
/// Downstream Queues to which to send IGMP frames
////////////////////////////////////////////////////////////////////////////////
static
TkOnuDnDest IgmpDnDest (LinkIndex link)
    {
    UNUSED(link);

    return PortsActive ();
    } // IgmpDnDest
#endif



////////////////////////////////////////////////////////////////////////////////
/// ValidIgmpIpAddr:  Check if the address is a valid IGMP IP address
///
 // Parameters:
/// \param ipAddr   IP Address to validate
///
/// \return
/// TRUE if valid address, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL ValidIgmpIpAddr(IpAddr const BULK *ipAddr)
    {
    // Valid IGMP address range is 224.0.0.0 to 239.255.255.255
    // Check for the MSB of the IPv4 address to be 0xE0
    // 224.0.0.0 to 224.0.0.255 cannot be used for user groups
    if ((0xE0 == (0xF0 & *((U8 const BULK *) ipAddr))) &&
        (*ipAddr > 0xE00000FFL))
        {
        return TRUE;
        }

    return FALSE;
    } // ValidIgmpIpAddr



////////////////////////////////////////////////////////////////////////////////
/// ValidIgmpVid:  Check if the VID is OK to process
///
 // Parameters:
/// \param vid  VID on which this IGMP frame was received
///
/// \return
/// Number of groups allowed for this VID
////////////////////////////////////////////////////////////////////////////////
static
U8 ValidIgmpVid (U16 vid)
    {
    U8  FAST index;
    U8  FAST result = curIgmpMaxGroups; // some very big number

    if (igmpVlanCfgValid)
        {
        if ((index = GetIgmpVidEntry(vid)) < TkOnuMaxIgmpVlan)
            {
            result = igmpVlanCfg.nvsConfig.vlanCfg[index].maxGroups;
            }
        else if (!igmpVlanCfg.nvsConfig.ignoreUnmanaged)
            {
            result = 0;
            }
        else { /* lint */ }
        }

    return result;
    } // ValidIgmpVid



////////////////////////////////////////////////////////////////////////////////
/// ValidIgmpFrame:  Check if the frame is a valid IGMP frame
///
 // Parameters:
/// \param vid      VID on which frame was received
/// \param igmp     IGMP frame to validate
///
/// \return
/// TRUE if valid frame, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
static
BOOL ValidIgmpFrame (U16 vid, IgmpFrame const *igmp)
    {
    // Validate IP Address in the IGMP frame
    //
    // NOTE: This is not a complete check to validate an IGMP frame since
    //       IGMP General Queries have an all-zeroes IP address in them.
    //       Neither does it do stuff like check for checksum, as required
    //       in IGMPv3
    return (ValidIgmpIpAddr((IpAddr const BULK *) &(igmp->groupAddr)) &&
            (ValidIgmpVid(vid) > 0));
    } // ValidIgmpFrame



////////////////////////////////////////////////////////////////////////////////
/// IgmpDnHandle:  Handle downstream IGMP frames
///
 // Parameters:
/// \param link     Link on which IGMP frame was received
/// \param vid      VID on which this IGMP frame was received
/// \param igmp     IGMP frame to process
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void IgmpDnHandle (LinkIndex link, U16 vid, IgmpFrame const BULK* igmp)
    {
    U8 FAST portDstInfo;    // which ports are enabled

    if (enableAllTraffic)
        { // None of the links is ready yet.
        return;
        }

    // Figure out which ports should receive downstream IGMP traffic
    portDstInfo = TkOnuIgmpDnDestGet (link, vid);

    // If broadcast link is not associated with the port, check to see if
    // any of the unicast links are associsted with any of the user ports
    // and if so, forward IGMP traffic to them.
    if (portDstInfo == TkOnuDnProc)
        {
        U8 FAST port;
        U8 FAST i = numToRegister;

        while (i > 0)
            {
            if (IsLinkInService (i-1))
                {
                port = FindPortForLink (i-1);
                if (port != TkOnuDnNullPort)
                    {
                    ++port; // FindPortForLink returns zero based port #
                    portDstInfo |= port;
                    }

                if (portDstInfo == TkOnuDnPortAll)
                    {
                    break;
                    }
                }
            --i;
            }
        }

    if ((portDstInfo != TkOnuDnNullPort) && (ValidIgmpVid(vid) > 0))
        {
#if CTC_PROXY
        // If in CTC mode with Tag Stripping enabled, delete outermost VLAN
        // Tag if one exists.
#if !OAM_SWITCH
        // This is implemented differently on the switch
        EthFrameDelVlanTag ((TkOnuEthPort) portDstInfo);
#endif
#endif
        // forward, then process, to minimize snooping delay
        FrameForwardDn ((TkOnuDnDest)portDstInfo);

        OntmDebug (IgmpDebugLevel, ("IGMP: dn link %bu vid %x type %02bx\n",
                        link, vid, igmp->type));

        switch (igmp->type)
            {
            case IgmpV1Report :
            case IgmpV2ReportV1 :
            case IgmpV2Report :
            case IgmpV2Leave :
                // ignoring joins from upstream for now
                break;

            case IgmpV1Query :
                IgmpSetDnTimers (IgmpV1MaxResponseTime, vid,
                                (IpAddr BULK *) &(igmp->groupAddr));
                break;

            case IgmpV2V3Query :
                IgmpSetDnTimers (igmp->maxResponseTime, vid,
                                (IpAddr BULK *) &(igmp->groupAddr));
                break;

            default :
                break;
            }
        } // if frame should be processed

    } // IgmpDnHandle



////////////////////////////////////////////////////////////////////////////////
/// IgmpUpHandle:  Handle upstream IGMP frames
///
 // Parameters:
/// \param port Port on which IGMP frame was received
/// \param vid  VID on which this IGMP frame was received
/// \param igmp IGMP frame to process
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void IgmpUpHandle (TkOnuEthPort port, U16 vid, IgmpFrame const BULK* igmp)
    {
    BOOL        FAST allowedGroups;
    BOOL        FAST fwd = FALSE; // is it OK to fwd the frame to the OLT

#if BCM53125
    //
    // The frame came in from the IMP port and it does not have a link
    // associated with it. Use the GE port instead.
    //
    LinkIndex   FAST txLink = FindLinkForPort (0, 0);
#else
    LinkIndex   FAST txLink = FindLinkForPort (TkOnuGetUniPort(port), 0);
#endif

    OntmDebug (IgmpDebugLevel, ("IGMP: Port %bu VID %x Up Type %02bx\n",
                port, vid, igmp->type));

    allowedGroups = ValidIgmpFrame (vid, igmp) && ValidPortVid (port, vid);

    // Process the frame only if there is a valid link available to send the
    // frame upstream and if the frame is a valid IGMP frame
    if (txLink < TkOnuNumTxLlids)
        {
        if (allowedGroups)
            {
            switch (igmp->type)
                {
                case IgmpV1Report :
                case IgmpV2ReportV1 :
                case IgmpV2Report :
                    fwd = IgmpTrueSnoop() ?
                        IgmpAddDnGroup (port, vid, igmp) : TRUE;
                    break;

                case IgmpV2Leave :
#if CTC_PROXY
                    if (!NvsRecIdExists (NvsRecIdCtcIgmp)) // not CTC IGMP mode
                        {
                        if (igmpCurrCfg.lmqCount == 0)
                            { // fast leave enabled
                            IgmpRemoveDnGroup (port, vid, igmp);
                            }
                        else // if (IgmpTrueSnoop())
                            {
                            IgmpProcessLeave (port, vid, igmp);
                            }

                        fwd = TRUE;
                        }
                    else
                        {
                        if (CtcMcastIsFastLeaveEnabled ())
                            { // fast leave enabled
                            // delete the multicast entry
                            IgmpRemoveDnGroup (port, vid, igmp);
                            fwd = TRUE;
                            // CTC special delete
                            }
                        else
                            { // non-fast leave
                            IgmpProcessLeave (port, vid, igmp);
                            fwd = (PersCtcProxyIgmpLeave ()) ?
                                    FALSE : // according the last member query
                                            // result to forward or not the
                                            // leave message to OLT
                                    TRUE;
                            }
                        }
#else
                    if (igmpCurrCfg.lmqCount == 0)
                        { // fast leave enabled
                        IgmpRemoveDnGroup (port, vid, igmp);
                        }
                    else if (IgmpTrueSnoop())
                        {
                        IgmpProcessLeave (port, vid, igmp);
                        }
                    fwd = TRUE;
#endif
                    break;

                case IgmpV1Query :
                case IgmpV2V3Query :
                    // ignoring upstream mc flows for now
                    // do not forward queries -- TT660
                    // Helps against DOS-ing the OLT
                    // IgmpSetUpTimers();
                    break;

                default:
                    // do not forward illegal opcodes
                    break;
                } // end switch
            } // if valid frame
        else
            {
            // forward the frame if it is a leave frame and either:
            // a) leave forwarding is set to forward any unknown
            // b) leave forwarding is set to forward zero and group addr is zero
            fwd = ((igmp->type == IgmpV2Leave) &&
                   ((igmpLeaveForwarding == IgmpLeaveForwardingAnyUnknown) ||
                    ((igmpLeaveForwarding == IgmpLeaveForwardingZero) &&
                     (igmp->groupAddr == 0))));
            }
        } // if valid link

    // apply IGMP rate limiting if limit != 0
    if (fwd && (ethPortIgmpRateLimit[port] > 0))
        {
        if (currentIgmpCredits[port] > 0)
            { // we have credits, use one
            --(currentIgmpCredits[port]);
            }
        else
            { // we have no credits, drop the frame
            fwd = FALSE;
            }
        }

    if (fwd)
        {
        // assumes rxFrameLength and RxFrame
        EthFrameAddDefaultVlanTag (port, vid);

        FrameForwardUp (txLink);
        }
    } // IgmpUpHandle



////////////////////////////////////////////////////////////////////////////////
/// CheckDnQueryResponse:  Checks to see if query response was received
///
 // Parameters:
/// \param port         Port for which Dn Query Response needs to be checked
/// \param entry        Entry to make pending
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void CheckDnQueryResponse (TkOnuEthPort port, IgmpFwd BULK* entry)
    {
    switch (entry->portState[port])
        {
        case IgmpStatePending:
            // no response; the group should be removed
            IgmpClearEntry (port, entry);
            break;

        case IgmpStateReported:
            // If a port has already reported, it needs to move
            // back to active state
            entry->portState[port] = IgmpStateActive;
            break;

        default:
            // do nothing
            break;
        } // end switch

    } // CheckDnQueryResponse



////////////////////////////////////////////////////////////////////////////////
/// IgmpHandleTimer:  Handle Timer expiry events
///
 // Parameters:
/// \param  timerId     Timer that has expired
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void IgmpHandleTimer (OntmTimerId timerId)
    {
    TkOnuEthPort    FAST port;
    U8              FAST count = activeIgmpGroupCount;
    U16             FAST timerVal;
    IgmpFwd BULK *  FAST entry = igmpDn;

    UNUSED(timerId);

    // for all active IGMP groups, check if the timer is active. If yes,
    // decrement the counter. If the decremented counter reaches zero,
    // the timer for this group has expired and we need to call the
    // CheckDnQueryResponse function for this group

    while (count > 0)
        {
        if (EntryInUse(entry))
            {
            count--;
            for (port = 0; port < TkOnuNumUserPorts; ++port)
                {
                timerVal = entry->timerVal[port]; // cache for smaller code
                if (timerVal > 0)
                    { // timer is active

                    // decrement the timer and check for expiry
                    --timerVal;

                    entry->timerVal[port] = timerVal; // update the timer
#if CTC_PROXY
                    if (!(NvsRecIdExists (NvsRecIdCtcIgmp)) ||
                            !PersCtcProxyIgmpLeave ())
                        {
                        if (0 == timerVal)
                            { // timer has expired
                            OntmDebug (IgmpDebugLevel,
                                    ("IGMP: Timer %08lx:%04x port %bu\n",
                                     entry->ipAddr, entry->vid, port));

                            CheckDnQueryResponse (port, entry);
                            }
                        }
                    else
                        {
                        if (0 == timerVal)
                            { // timer has expired
                            OntmDebug (IgmpDebugLevel,
                                    ("IGMP: Timer %08lx:%04x port %bu\n",
                                     entry->ipAddr, entry->vid, port));

                            if (IgmpStatePending == entry->portState[port])
                                {// no response
                                if (0 == entry->lastMemQueryCounter[port])
                                    {
                                    if (0x5a == entry->ifLastMemQuery[port])
                                        {
                                        // If it's the last member query
                                        // cycle then forward one leave
                                        // message to the OLT
                                        BuildAndFwdLeave (port, entry);
                                        }

                                    IgmpClearEntry (port, entry);
                                    }
                                else
                                    {
                                    --entry->lastMemQueryCounter[port];
                                    entry->timerVal[port] =
                                        DefaultLastMemQueryInterval;
                                    // resend query message to the port
                                    BuildAndTxQuery (port, entry);
                                    }
                                }
                            else if (IgmpStateReported == entry->portState[port])
                                {// a port has already reported
                                entry->portState[port] = IgmpStateActive;
                                entry->lastMemQueryCounter[port] = 0;
                                entry->timerVal[port] = 0;
                                }
                            else { /* lint */ }
                            }
                        }
#else
                    if (0 == timerVal)
                        { // timer has expired
                        OntmDebug (IgmpDebugLevel,
                                ("IGMP: Timer %08lx:%04x port %bu\n",
                                 entry->ipAddr, entry->vid, port));

                        CheckDnQueryResponse (port, entry);
                        }
#endif
                    } // end timerVal > 0
                else if (0 == timerVal)
                    {
                    //
                    //  Handle the case where timerVal has been changed to
                    //  zero outside this handler.
                    //
                    CheckDnQueryResponse (port, entry);
                    }

                } // for all ports
            } // if group is active

        entry++;
        } // end while

    if (igmpRateLimitCounter > 0)
        {
        --igmpRateLimitCounter;
        }
    else
        {
        memcpy(&currentIgmpCredits[0], &ethPortIgmpRateLimit[0],
               sizeof(NvsIgmpRateLimits));

        igmpRateLimitCounter = 9;
        }
    } // IgmpHandleTimer



////////////////////////////////////////////////////////////////////////////////
/// IgmpGetVlanCfg:  Get IGMP VLAN configuration
///
 // Parameters:
/// \param reply    Pointer to the OAM reply container
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void IgmpGetVlanCfg (OamVarContainer BULK *reply)
    {
    memcpy((NvsIgmpVlanRecord BULK *) reply->value,
            &(igmpVlanCfg.nvsConfig), sizeof(NvsIgmpVlanRecord));
    reply->length = (2 * sizeof(U8)) + // ignoreUnmanaged and numVlans
                    (igmpVlanCfg.nvsConfig.numVlans * sizeof(IgmpVlanCfg));
    } // IgmpGetVlanCfg




////////////////////////////////////////////////////////////////////////////////
/// ValidIgmpVlanCfg:  Validate IGMP VLAN configuration record
///
 // Parameters:
/// \param cfg  Pointer to the new IGMP VLAN Configuration record
///
/// \return
/// TRUE if valid, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
static
BOOL ValidIgmpVlanCfg (NvsIgmpVlanRecord const BULK *cfg)
    {
    U8  FAST i;

    if (cfg->numVlans <= TkOnuMaxIgmpVlan)
        {
        for (i = 0; i < cfg->numVlans; ++i)
            {
            if ((cfg->vlanCfg[i].eponVid & 0xF000) > 0)
                {
                return FALSE;
                }
            }
        return TRUE;
        }

    return FALSE;
    } // ValidIgmpVlanCfg



////////////////////////////////////////////////////////////////////////////////
/// IgmpSetGroupLimits:  Write IGMP configuration into NVS
///
 // Parameters:
/// \param pRec Pointer to an IGMP Configuration record
///
/// \return
/// TRUE if successful, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
static
BOOL IgmpSetGroupLimits(NvsIgmpRecord BULK *pRec)
    {
    // write to the NVS
    return (NvsWrite(NvsRecIdIgmp, (void *) pRec, sizeof(NvsIgmpRecord)));
    } // IgmpSetGroupLimits



////////////////////////////////////////////////////////////////////////////////
/// IgmpGetGroupLimits:  Get IGMP Configuration
///
 // Parameters:
/// \param cfg  Pointer to buffer to hold config
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
// extern
void IgmpGetGroupLimits (OamIgmpConfig BULK* cfg)
    {
    U8  FAST port;

    cfg->robustnessCount = igmpCurrCfg.robustnessCount;
    cfg->lmqCount = igmpCurrCfg.lmqCount;
    cfg->numPorts = TkOnuUserPortCount ();

    for (port = 0; port < cfg->numPorts; ++port)
        {
        cfg->port[port].numGroups   = igmpCurrCfg.portCfg[port].groupLimit;
        cfg->port[port].dnQueue     = igmpCurrCfg.portCfg[port].dnQ;
        }

    // Get extended information
    memcpy((IgmpExtendedInfo BULK *) &(cfg->port[port]),
            (IgmpExtendedInfo BULK *) &(igmpCurrCfg.grpFwdQual),
            sizeof(IgmpExtendedInfo));
    } // IgmpGetGroupLimits



////////////////////////////////////////////////////////////////////////////////
/// IgmpClearDynVidRules:  Remove Dynamic VID rules installed by this module
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void IgmpClearDynVidRules (void)
    {
    U8 FAST i;

    for (i = 0; i < TkOnuMaxIgmpVlan; ++i)
        {
        if (igmpVlanCfg.rule[i] != CfgRuleNotFound)
            {
            RuleDnFree (igmpVlanCfg.rule[i]);
            igmpVlanCfg.rule[i] = CfgRuleNotFound;
            }
        }
    } // IgmpClearDynVidRules



////////////////////////////////////////////////////////////////////////////////
/// IgmpClearDynFwdRules:  Remove Dynamic group forward rules
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void IgmpClearDynFwdRules (void)
    {
    U8 FAST i; U8 FAST port;

    for (i = 0; (i < curIgmpMaxGroups) && (activeIgmpGroupCount > 0); ++i)
        {
        if (EntryInUse(&(igmpDn[i])))
            {
            /*clear user port hardware Mcast forwarding entry*/
            for (port = 0; port < TkOnuUserPortCount(); ++port)
                {
                if (igmpDn[i].portState[port] != IgmpStateUnused)
                    {
                    TkOnuUserPortRemoveMcast (&(igmpDn[i].ipAddr), port,
                                              igmpDn[i].vid);
                    }
                }

            RemoveDnEntry (&(igmpDn[i]));
            }
        }

    // clear out the port usage count
    memset(ethPortIgmpCount, 0, sizeof(ethPortIgmpCount));
    } // IgmpClearDynFwdRules



////////////////////////////////////////////////////////////////////////////////
/// IgmpClearDynRules:  Remove Dynamic rules installed by the IGMP module
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void IgmpClearDynRules (void)
    {
    // clear out the IGMP dynanic forward rules
    IgmpClearDynFwdRules ();

    // clear rules installed to filter out VIDs
    IgmpClearDynVidRules ();
    } // IgmpClearDynRules



////////////////////////////////////////////////////////////////////////////////
/// IgmpClearDluRules:  Remove DLU rules for IGMP snooping
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void IgmpClearDluRules (void)
    {
    //
    // NOTE:
    // Make sure we clear the downstream chain rules in descending direction
    // to avoid partial chain rules that can cause traffic disruption.
    //

    Tk3701DluClrRule (DluSnoopIgmpRule + 1);
    Tk3701DluClrRule (DluSnoopIgmpRule);

    Tk3701DluClrRule (DluIgmpRangeRule + 1);
    Tk3701DluClrRule (DluIgmpRangeRule);

    // clear the DLU init bit
    doneDluInit = 0;
    } // IgmpClearDluRules



////////////////////////////////////////////////////////////////////////////////
/// IgmpRemoveUlu:  Remove ULU rules for IGMP snooping for a given port
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void IgmpRemoveUlu (void)
    {
    TkOnuEthPort FAST port;
    TkOnuEthPort FAST numPorts = TkOnuUserPortCount ();

    for (port = 0; port < numPorts; ++port)
        {
        IgmpClearUluRules (port);
        }
    } // IgmpRemoveUlu


////////////////////////////////////////////////////////////////////////////////
/// IgmpConfigDlu:  Configure DLU rules for IGMP snooping
///
 // Parameters:
/// \param numGroups    Number of groups to configure
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void IgmpConfigDlu (U8 numGroups)
    {
    if ((numGroups > 0) && !doneDluInit)
        {
        Tk3701DluRule   BULK ipRule;
        Tk3701DluRule   BULK igmpRule;

        memcpy(&ipRule, &DluIsIp, sizeof(Tk3701DluRule));
        memcpy(&igmpRule, &DluGrabIgmp, sizeof(Tk3701DluRule));

        if ((igmpCurrCfg.snoopOpt & IgmpDisableDnAutoSnoop) > 0)
            {
            // Turn OFF the forwarding option in the rules
            SetRuleAct (&ipRule, OamNewRuleActNoOp, FALSE);
            SetRuleAct (&igmpRule, OamNewRuleActNoOp, FALSE);
            }

        Tk3701DluSetRule (DluSnoopIgmpRule, &ipRule);
        Tk3701DluSetRule (DluSnoopIgmpRule + 1, &igmpRule);

        Tk3701DluSetRule (DluIgmpRangeRule, &DluIgmpDiscardLo);
        Tk3701DluSetRule (DluIgmpRangeRule + 1, &DluIgmpDiscardHi);

        // if possible setup rules to discard IPMC frames from selected VIDs
        (void)IpmcFilterVid (&igmpVlanCfg);

        doneDluInit = 1;
        }
    } // IgmpConfigDlu




////////////////////////////////////////////////////////////////////////////////
/// SuspendIgmp:  Suspend IGMP traffic
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void SuspendIgmp (void)
    {
    // set all ELU/DLU rules to discard
    IgmpClearUluRules (0);
    IgmpClearUluRules (1);
    IgmpClearDluRules ();
    } // SuspendIgmp



////////////////////////////////////////////////////////////////////////////////
/// ResumeIgmp:  Resume IGMP traffic
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void ResumeIgmp (void)
    {
    TkOnuEthPort    FAST userPort;
    TkOnuEthPort    FAST numPorts = TkOnuUserPortCount();
    U8              FAST count;

    for (userPort = 0; userPort < numPorts; ++userPort)
        {
        count = igmpCurrCfg.portCfg[userPort].groupLimit;

        IgmpConfigInitDlu (count);
        IgmpConfigInitUlu (userPort, count);
        }
    } // ResumeIgmp



////////////////////////////////////////////////////////////////////////////////
/// IgmpConfigLue:  Setup ELU and DLU rules for IGMP
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
#define IgmpConfigLue ResumeIgmp



////////////////////////////////////////////////////////////////////////////////
/// IgmpSetVlanCfg:  Set IGMP VLAN configuration
///
 // Parameters:
/// \param src  Pointer to OAM container with new configuration
///
/// \return
/// TRUE if successful, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL IgmpSetVlanCfg (OamVarContainer BULK *src)
    {
    if (ValidIgmpVlanCfg ((NvsIgmpVlanRecord BULK *) src->value))
        {
        OntmDebug (IgmpDebugLevel, ("Wr VLAN cfg to NVS\n"));

        memcpy(&(igmpVlanCfg.nvsConfig),
                (NvsIgmpVlanRecord BULK *) src->value,
                src->length);

        // write to the NVS
        if (NvsWrite(NvsRecIdIgmpVlan, (void *) &(igmpVlanCfg.nvsConfig),
                            sizeof(NvsIgmpVlanRecord)))
            {
            doneDluInit = 0;
            igmpVlanCfgValid = (!(igmpVlanCfg.nvsConfig.ignoreUnmanaged) ||
                                (igmpVlanCfg.nvsConfig.numVlans > 0)) ? 1 : 0;
            IgmpConfigLue();

            return TRUE;
            }
        }

    return FALSE;
    } // IgmpSetVlanCfg



////////////////////////////////////////////////////////////////////////////////
/// IgmpConfigInit:  Initialize rules for IGMP
///
/// \param size     Size of IGMP record
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void IgmpConfigInit (U16 size)
    {
    if (size == 0)
        { // record not found in the NVS, check the Personality
        U8 FAST port;

        igmpCurrCfg.robustnessCount = PersIgmpRobustnessCount();
        igmpCurrCfg.lmqCount = PersIgmpLmqCount();

        for (port = 0; port < TkOnuUserPortCount(); ++port)
            {
#if KT_DASAN_PMC_OAM
            dasanIgmpGroupCount = PersIgmpPortLimit(port);
#endif
            igmpCurrCfg.portCfg[port].groupLimit = PersIgmpPortLimit(port);

            igmpCurrCfg.portCfg[port].dnQ = PersIgmpPortDnQ(port);
            }
        }

    if (size < sizeof(NvsIgmpRecord)) // old record or no config in NVS
        {
        igmpCurrCfg.grpFwdQual  = (IgmpGrpFwdQualifier)PersIgmpGrpFwdQual ();
        igmpCurrCfg.snoopOpt    = (IgmpSnoopOption)PersIgmpSnoopOpt ();

        // If everything fails, fall back to forwarding by L2 DA
        if (igmpCurrCfg.grpFwdQual == 0)
            {
            igmpCurrCfg.grpFwdQual = IgmpAddGroupByL2Da;
            }
        }

    igmpVlanCfgValid = 0;

    if (NvsRead(NvsRecIdIgmpVlan, (void *) &(igmpVlanCfg.nvsConfig),
                sizeof(NvsIgmpVlanRecord)) == 0)
        { // record not found in the NVS, zero out VLAN config
        memset(&(igmpVlanCfg.nvsConfig), 0, sizeof(NvsIgmpVlanRecord));
        }
    else
        { // NVS record exists
        if (!(igmpVlanCfg.nvsConfig.ignoreUnmanaged) ||
            (igmpVlanCfg.nvsConfig.numVlans > 0))
            {
            igmpVlanCfgValid = 1;
            }
        }

    if (NvsRead(NvsRecIdIgmpUnknownLeaveFwd, &igmpLeaveForwarding,
                 sizeof(IgmpLeaveForwarding)) == 0)
        {
        igmpLeaveForwarding = IgmpLeaveForwardingStandard;
        }

    if (NvsRead(NvsRecIdIgmpRateLimit, &ethPortIgmpRateLimit[0],
                 sizeof(NvsIgmpRateLimits)) == 0)
        { // no NVS record, zero out config
        memset(&ethPortIgmpRateLimit[0], 0, sizeof(NvsIgmpRateLimits));
        }
    else
        { // NVS record exists
        memcpy(&currentIgmpCredits[0], &ethPortIgmpRateLimit[0],
               sizeof(NvsIgmpRateLimits));
        }

    igmpRateLimitCounter = 9;

    IgmpConfigLue();
    } // IgmpConfigInit



////////////////////////////////////////////////////////////////////////////////
/// IgmpGetPortmap:  Given an IGMP entry, get the map of subscribed ports
///
/// WARNING:    This function would work only when the number of ports is less
///             than or equal to 8
///
 // Parameters:
/// \param index     Index to look at
///
/// \return
/// U8 Portmap of the ports subscribed
////////////////////////////////////////////////////////////////////////////////
static
U8 IgmpGetPortmap (U8 index)
    {
    U8 FAST port;
    U8 FAST numPorts;
    U8 FAST result = 0;

    numPorts = TkOnuUserPortCount();

    for (port = 0; port < numPorts; ++port)
        { // figure out which port(s) has this group active
        if (igmpDn[index].portState[port] != IgmpStateUnused)
            {
            result |= (1UL << port); //lint !e734 Loss of precision
            }
        }

    return result;
    } // IgmpGetPortmap



////////////////////////////////////////////////////////////////////////////////
/// IgmpGetNextActiveGroup:  Starting at an index, get next active group
///
 // Parameters:
/// \param index     Index to start from
/// \param groupInfo Pointer to memory where group info is stored
///
/// \return
/// Index to be used as starting point for next call;
/// >= IgmpMaxGroups when no more active groups exist
////////////////////////////////////////////////////////////////////////////////
//extern
U8 IgmpGetNextActiveGroup (U8 index, OnuIgmpGroupInfo BULK *groupInfo)
    {
    U8 FAST result = (curIgmpMaxGroups + 1); // init with bad return value

    if (activeIgmpGroupCount == 0)
        {
        return result;
        }

    // find next active group starting with index
    while (index < curIgmpMaxGroups)
        {
        if (EntryInUse(&igmpDn[index]))
            { // return value
            groupInfo->portMap = IgmpGetPortmap(index);
            groupInfo->ipAddr = igmpDn[index].ipAddr;
            groupInfo->vid = (igmpDn[index].vid == IgmpNoVid) ?
                                0 : igmpDn[index].vid;
            result = ++index;
            break;
            }

        ++index;
        } // for each slot in the IGMP table

    return result;
    } // IgmpGetNextActiveGroup



////////////////////////////////////////////////////////////////////////////////
/// IgmpGetActiveGroupCount:  Get count of groups that are currently active
///
 // Parameters:
/// \param port     Port for which group count needs to be returned
///
/// \return
/// Number of groups currently active for the specified port
////////////////////////////////////////////////////////////////////////////////
//extern
U8 IgmpGetActiveGroupCount (TkOnuEthPort port)
    {
    return ethPortIgmpCount[port];
    } // IgmpGetActiveGroupCount



////////////////////////////////////////////////////////////////////////////////
/// OamIgmpClrGroups:  Clear all IGMP groups associated with a given port
///
 // Parameters:
/// \param port     Port for which group need to be deleted
///
/// \return
/// TRUE if operation was successful, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL OamIgmpClrGroups (TkOnuEthPort port)
    {
    U8              FAST count = activeIgmpGroupCount;
    IgmpFwd BULK*   FAST entry = igmpDn;

    while (count > 0)
        {
        if (EntryInUse(entry))
            {
            if (entry->portState[port] > 0)
                {
                IgmpClearEntry (port, entry);
                }

            --count;
            }

        ++entry;
        }

    return TRUE;
    } // OamIgmpClrGroups



////////////////////////////////////////////////////////////////////////////////
/// OamIgmpDelGroup:  Forcefully delete an IGMP group
///
 // Parameters:
/// \param portMap  Bitmap of ports from which group is to be deleted
/// \param vid      VID for which group needs to be deleted
/// \param addr     The IGMP group that needs to be deleted
///
/// \return
/// TRUE if delete was successful, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL OamIgmpDelGroup (U8 portMap, U16 vid, IpAddr BULK* addr)
    {
    IgmpFwd BULK*   FAST entry;
    TkOnuEthPort    FAST port;
    BOOL            FAST result = FALSE;

    entry = FindGroup (vid, addr);
    if (entry != NULL)
        { // if there is an entry for this group
        result = TRUE;
        for (port = 0; port < TkOnuNumUserPorts; ++port, (portMap >>= 1))
            {
            // check to see if this port was specified in the delete request
            if ((portMap & 0x1) > 0)
                {
                // check if the entry exists and the port is receiving traffic
                // from this group
                if (entry->portState[port] != IgmpStateUnused)
                    {
                    IgmpClearEntry (port, entry);
                    }
                else
                    {
                    result = FALSE;
                    }
                } // if port in delete request
            } // for each port
        } // if there is an entry for this group

    return result;
    } // OamIgmpDelGroup




////////////////////////////////////////////////////////////////////////////////
/// OamIgmpAddGroup:  Forcefully add an IGMP group
///
 // Parameters:
/// \param portMap  Bitmap of port(s) to which to add the group
/// \param vid      VID for which group needs to be added
/// \param addr     The IGMP group that needs to be added
///
/// \return
/// TRUE if add was successful, FALSE otherwise (incl. if group already active)
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL OamIgmpAddGroup (U8 portMap, U16 vid, IpAddr const BULK* addr)
    {
    TkOnuEthPort    FAST port;
    BOOL            FAST result;
    IgmpFrame       BULK igmp;

    result = ValidIgmpIpAddr(addr) && (ValidIgmpVid(vid) > 0);

    igmp.groupAddr = *addr;

    for (port = 0; (port < TkOnuNumUserPorts) && result; ++port, (portMap >>= 1))
        {
        // check to see if this port was specified in the delete request
        if ((portMap & 0x1) > 0)
            {
            // add the group for this port
            result = IgmpAddDnGroup(port, vid, &igmp);
            }
        }

    return result;
    } // OamIgmpAddGroup



////////////////////////////////////////////////////////////////////////////////
/// ClearIgmpLue:  Clear all LUE entries relating to IGMP
///
 // Parameters:
/// \param newCount     Number of IGMP Groups to support
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
// extern
void ClearIgmpLue (U8 newCount)
    {
    IgmpRemoveUlu ();
    IgmpClearDluRules ();

    if (newCount == 0)
        { // clear out all the rules only if snooping has been disabled
        // Clear dynamic rules
        IgmpClearDynRules ();
        }

    // clear the DLU init flag
    doneDluInit = 0;
    } // ClearIgmpLue



////////////////////////////////////////////////////////////////////////////////
/// IgmpReconfigure:  Reconfigure IGMP snooping in the ONU
///
 // Parameters:
/// \param cont     Pointer to the container containing OAM information
///
/// \return
/// TRUE if reconfigure was successful, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
// extern
BOOL IgmpReconfigure(OamVarContainer const BULK *cont)
    {
    NvsIgmpRecord   BULK newCfg;
    U8              FAST i;
    U8              FAST newGrpCount = 0;
    BOOL            FAST result = FALSE;
    OamIgmpConfig   BULK * FAST pOamConfig = (OamIgmpConfig BULK *)cont->value;

    // clear out newCfg to make sure that we get sane values for Port 1
    // parameters, just in case there is only one port in the Host Interface
    // Message.
    // The Host Interface Message better not have a port count greater than 2
    memset(&newCfg, 0, sizeof(NvsIgmpRecord));

    // Don't check for count of 0. However users must be aware that count of 0
    // is non-standard.
    // LMQC == 0 is legal, and means Fast Leave
    if ((pOamConfig->robustnessCount > MaxRobustness) ||
        (pOamConfig->lmqCount > MaxLmqc))
        {
        return result;
        }

    newCfg.robustnessCount = pOamConfig->robustnessCount;
    newCfg.lmqCount = pOamConfig->lmqCount;

    // If there is more information in the message than sizeof(OamIgmpConfig),
    // there is information about how to qualify Group Forwarding and IGMP
    // Snooping Options
    // Check if the group Forwarding Qualifier and Snooping Options are
    // consistent with this platform
    i = (3 * sizeof(U8)) + (pOamConfig->numPorts * sizeof(OamIgmpPortConfig));

    if (cont->length > i)
        {
        memcpy(&(newCfg.grpFwdQual),
                &(pOamConfig->port[pOamConfig->numPorts]),
                sizeof(IgmpExtendedInfo)); //lint !e419 data overrun for memcpy
        }
    else
        {
        newCfg.grpFwdQual = IgmpAddGroupByL2Da;
        newCfg.snoopOpt = IgmpDefaultSnooping;
        }

    if (IgmpValidGrpFwdQual(newCfg.grpFwdQual) &&
        IgmpValidSnoopOpt(newCfg.snoopOpt))
        {
        for (i = 0; i < pOamConfig->numPorts; ++i)
            {
            newCfg.portCfg[i].groupLimit = pOamConfig->port[i].numGroups;
            newGrpCount += pOamConfig->port[i].numGroups;
            newCfg.portCfg[i].dnQ = pOamConfig->port[i].dnQueue;
            }

        // write to the NVS if everything went OK
        if (result = IgmpSetGroupLimits(&newCfg))
            { // is OK to reconfigure

            // get rid of all the rules and put new ones back based on
            // new config getting rid of the rules will also help drop
            // any traffic flowing into the queues that might be the
            // subject of reconfiguration
            ClearIgmpLue (newGrpCount);


            // setup IGMP rules
            IgmpConfigInit (NvsReadForced(NvsRecIdIgmp,
                                          (void *) &igmpCurrCfg,
                                          sizeof(NvsIgmpRecord)));
            }
        }

    return result;
    } // IgmpReconfigure



////////////////////////////////////////////////////////////////////////////////
/// RestoreMcastRules:  Restore multicast rules
///
/// Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
void RestoreMcastRules(void)
    {
#if IPV6_SUPPORT
    if (McastGetSnoopMode())
        {
        McastRebuildGroupRules (FALSE);
        }
    else
#endif
        {
        RestoreIgmpRules();
        }
    } // RestoreMcastRules



////////////////////////////////////////////////////////////////////////////////
/// RestoreIgmpRules:  Restore IGMP rules which were cleared during fiber-pull
///
/// Called because of the softreset() function which does a complete
/// reinit of the ONU hardware, in particular of the DLU and its rule tracker.
/// To keep IGMP rules over this reset during LOS, we have to preserve the
/// internal IGMP table and reconstruct the programming after the reset.
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
void RestoreIgmpRules (void)
    {
    IgmpFwd BULK* FAST entry;
    U8 FAST count = curIgmpMaxGroups;  // loop over all groups, rather than active
                                    // helps to audit "empty" entries in this
                                    // non-speed-critical routine

    activeIgmpGroupCount = 0; // count being recomputed. Need to start from 0

    entry = igmpDn;
    while (count-- > 0)
        {
        if (EntryInUse(entry))
            { // entry was in use before softreset
            // DLU rule table was re-init'd, and rule allocator reset
            // must reallocate all active rules
            if ((entry->ruleId = IgmpAllocDnRule (entry)) != CfgRuleNotFound)
                { // if rule allocated
                CreateFwdRule (entry);
                ++activeIgmpGroupCount;
                }
            else
                { // error - can't realloc rule to restore entry
                FreeEntry(entry);
                }
            } // if entry was valid

        entry++;
        }
    } // RestoreIgmpRules



////////////////////////////////////////////////////////////////////////////////
/// IgmpLeaveForwardingSet: Set IGMP leave forwarding behavior
///
/// Parameters:
/// \param fwd      behavior to set
////////////////////////////////////////////////////////////////////////////////
//extern
OamVarErrorCode IgmpUnknownLeaveFwdSet(IgmpLeaveForwarding fwd)
    {
    if (fwd >= IgmpLeaveForwardingCount)
        {
        return OamVarErrActBadParameters;
        }

    igmpLeaveForwarding = fwd;
    return NvsWrite(NvsRecIdIgmpUnknownLeaveFwd, &igmpLeaveForwarding,
                    sizeof(IgmpLeaveForwarding)) ?
        OamVarErrNoError : OamVarErrAttrUndetermined;
    } // IgmpUnknownLeaveFwdSet



////////////////////////////////////////////////////////////////////////////////
/// IgmpLeaveForwardingGet: Get IGMP leave forwarding behavior
///
/// \return
/// The currently provisioned behavior
////////////////////////////////////////////////////////////////////////////////
//extern
IgmpLeaveForwarding IgmpUnknownLeaveFwdGet (void)
    {
    return igmpLeaveForwarding;
    } // IgmpUnknownLeaveFwdGet



////////////////////////////////////////////////////////////////////////////////
// IgmpConfigRateLimit: Configure IGMP rate limiting
//
// Parameters:
// \param cont  Pointer to the container containing OAM information
//
// \return
// TRUE if provisioning was applied successfully, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL IgmpConfigRateLimit (TkOnuEthPort port, OamVarContainer const BULK *cont)
    {
    NvsIgmpRateLimits   BULK newCfg;
    BOOL                FAST result = TRUE;
    U8                  BULK * FAST pOamCfg = (U8 BULK *)cont->value;

    result = port < TkOnuNumUserPorts;

    if (result)
        {
        memcpy(&newCfg, &ethPortIgmpRateLimit[0], sizeof(NvsIgmpRateLimits));
        newCfg.limit[port] = pOamCfg[0];
        result = NvsWrite(NvsRecIdIgmpRateLimit,
                          &newCfg,
                          sizeof(NvsIgmpRateLimits));
        }

    if (result)
        {
        memcpy(&ethPortIgmpRateLimit[0], &newCfg, sizeof(NvsIgmpRateLimits));
        }

    return result;
    } // IgmpConfigRateLimit



////////////////////////////////////////////////////////////////////////////////
// IgmpGetRateLimit: returns the current rate limit for the specified port
//
// Parameters:
// \param port  The port to retrieve the rate limit for
//
// \return
// The current IGMP rate limit for the specified port
////////////////////////////////////////////////////////////////////////////////
//extern
U8 IgmpGetRateLimit (TkOnuEthPort port)
    {
    return ethPortIgmpRateLimit[port];
    } // IgmpGetRateLimit




////////////////////////////////////////////////////////////////////////////////
/// IgmpInit:  Initialize IGMP module
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern bit softResetInProgress;

//extern
void IgmpInit (void)
    {
    doneDluInit = 0;

    curIgmpMaxGroups = IgmpMaxGroups;

#if IPV6_SUPPORT
    if (sizeof(mcastGroupTable) < (curIgmpMaxGroups * sizeof(IgmpFwd)))
        {
        return;
        }

    igmpDn = (IgmpFwd BULK *) mcastGroupTable;
#endif

    memset (igmpVlanCfg.rule, CfgRuleNotFound,
            sizeof(igmpVlanCfg.rule));//lint !e569 Loss of info

    IgmpConfigInit(NvsRead(NvsRecIdIgmp,
                           (void *) &igmpCurrCfg,
                           sizeof(NvsIgmpRecord)));

    if (!softResetInProgress)
        {
        U8 FAST i;

        activeIgmpGroupCount = 0; // no groups active

        for (i = 0; i < TkOnuMaxIgmpVlan; ++i)
            {
            igmpVlanCfg.activeCount[i] = 0;
            }

        // With CfgRuleNotFound in the ruleId field of an IGMP entry indicating
        // the entry not being in use, we need to initialize this field in all
        // entries to that value
        for (i = 0; i < curIgmpMaxGroups; ++i)
            {
            FreeEntry (&igmpDn[i]);
            }
        } // end if not soft reset

    } // IgmpInit



// end IgmpSnoop.c


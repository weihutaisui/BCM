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
/// \file McastBase.c
/// \brief IP Multicast Base, Driver Adapter Layer
///
/// In this layer, it provids the methods for adding/deleting Ip multicast group
/// and grabs the Ip multicast management frame into CPU.
///
////////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include "Teknovus.h"
#include "Ip.h"
#include "FifCmd.h"
#include "Lue.h"
#include "McastDb.h"
#include "McastBaseRule.h"


#define McastSnoopPri                  1
#define McastFrameBlockPri             2
#define McastSsmSourceCtrlPri          2
#define McastVlanPri                   3
#define McastGrpBinEntryPri            3
#define McastPortDiscardMapPri         3
#define McastTrafficBlockPri           4
#define McastEnableSwDomainPri         5
#define McastDisableSwDomainPri        6


#define McastPortDiscValue(port)       (1ULL << (port))
#define McastPortDiscLMask(port)       (63 - (port))
#define McastPortDiscRMask(port)       (port)

#define McastV4MacPrefixRMask          23
#define McastV6MacPrefixRMask          32
#define McastPortRxQueue(port)         \
         ((U16)FifoGetQueueOffset(FifCmdGetSysQueue(port, FifUseGenProcRx)))

#define GroupEntryPortMapGet(inst, entry)   ((U8)(entry)->learnDomain)
#define GroupEntryPortMapSet(inst, entry, portMap)  \
                            ((entry)->learnDomain = (portMap))

static
LueTrackingId BULK mcastTrackingId = LueTrackIdInvalid;
static
LueTrackingId BULK mcastTrackingSysId = LueTrackIdInvalid;


static const LueInstance CODE McastIpInstance =
    { Dnstream, LueInstBinary, 0 };
static const LueInstance CODE McastMacInstance =
    { Dnstream, LueInstBinary, 1 };

static McastBaseCfg BULK mcastBaseCfg;
static BOOL BULK stripMcastVlan[McastPortMaxNum];


////////////////////////////////////////////////////////////////////////////////
/// \brief Build Multicast Mac Clause
///
/// \param v4     For Ipv4 or Ipv6 multicast
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void BuildMcastMacClause(BOOL v4)
    {
    (void)LueCmdPushClause (LueFieldInL2Da,
                            LueOpEqual,
                            ((v4) ? McastDataIpv4DaMacLow :
                                    McastDataIpv6DaMacLow),
                            0,
                            ((v4) ? McastV4MacPrefixRMask :
                                    McastV6MacPrefixRMask));
    } // BuildMcastMacClause


////////////////////////////////////////////////////////////////////////////////
/// \brief Build Multicast Frame Clause
///
/// \param v4     For Ipv4 or Ipv6 multicast
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void BuildMcastFrameClause(BOOL v4)
    {
    if(v4)
        {
        (void)LueCmdPushClause(LueFieldInIpProtocol, LueOpEqual,
                                      (U64)IpProtocolIgmp, 0, 0);
        }
    else
        {
        (void)LueCmdPushClause (LueFieldInIpProtocol, LueOpEqual,
                            (U64)IpProtocolIcmp6Mld, 0, 0);
        BuildMcastMacClause(FALSE);
        }
    } // BuildMcastFrameClause


//##############################################################################
//                      Filter Multicast Traffic And Frame
//##############################################################################

////////////////////////////////////////////////////////////////////////////////
/// \brief Build Multicast frame filter rule
///
/// \param v4 For Ipv4 or Ipv6 multicast
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void BuildMcastFrameFilterRule(BOOL v4)
    {
    LueCmdStart();
    BuildMcastFrameClause(v4);
    (void)LueCmdPushResult(LueActionDiscardSet, 0);
    } // BuildMcastFrameFilterRule


////////////////////////////////////////////////////////////////////////////////
/// \brief Build Multicast traffic filter rule
///
/// \param v4     For Ipv4 or Ipv6 multicast
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void BuildMcastTrafficFilterRule(BOOL v4)
    {
    LueCmdStart();
    BuildMcastMacClause(v4);
    (void)LueCmdPushResult(LueActionSetQueue, McastInvalidQueue);
    (void)LueCmdPushResult(LueActionDiscardSet, 0);
    } // BuildMcastTrafficFilterRule


////////////////////////////////////////////////////////////////////////////////
/// \brief Set multicast frame filter rules
///
/// \param v4     For Ipv4 or Ipv6 multicast
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void FilterMcastFrame(BOOL v4)
    {
    BuildMcastFrameFilterRule(v4);
	(void)LueCmdAdd(mcastTrackingSysId, McastFrameBlockPri, FALSE);
    } // FilterMcastFrame


////////////////////////////////////////////////////////////////////////////////
/// \brief Set multicast traffic filter rule
///
/// \param v4    For Ipv4 or Ipv6 multicast
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void FilterMcastTraffic(BOOL v4)
    {
    BuildMcastTrafficFilterRule(v4);
	(void)LueCmdAdd(mcastTrackingSysId, McastTrafficBlockPri, FALSE);
    } // FilterMcastTraffic


////////////////////////////////////////////////////////////////////////////////
/// \brief Filter the multicast
///
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void FilterMcast(void)
    {
    U8 FAST i;
    // Downstream
    LueCmdSetInst(Dnstream, LueInstCommon, 0);
    // Ipv4 and Ipv6 traffic
    FilterMcastTraffic(TRUE);
    FilterMcastTraffic(FALSE);
    // Igmp and Mld frame
    for(i = 0; i < McastPortMaxNum; i++)
        {
        LueCmdSetInst(Dnstream, LueInstEgress, i);
        FilterMcastFrame(TRUE);
        FilterMcastFrame(FALSE);
        }

    // Upstream
    LueCmdSetInst(Upstream, LueInstEgress, LuePonPortIndex);
    // Igmp and Mld frame
    FilterMcastFrame(TRUE);
    FilterMcastFrame(FALSE);
    } // FilterMcast


//##############################################################################
//                      Switch Domain Operation
//##############################################################################

////////////////////////////////////////////////////////////////////////////////
/// \brief Build zero switch domain rule by default
///
/// \param v4 For Ipv4 or Ipv6 multicast
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void BuildZeroSwDomainRule(BOOL v4)
    {
    LueCmdStart();
    BuildMcastMacClause(v4);
    (void)LueCmdPushResult(LueActionSetSwitchDomain, 0);
    } // BuildZeroSwDomainRule


////////////////////////////////////////////////////////////////////////////////
/// \brief Build Vlan Switch domain rule
///
/// \param v4 For Ipv4 or Ipv6 multicast
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void BuildVlanSwDomainRule(BOOL v4)
    {
    LueCmdStart();
    BuildMcastMacClause(v4);
    if(mcastBaseCfg.vlanType == McastVlanCVlan)
        {
        (void)LueCmdPushClause(LueFieldInL2CVlan,
                           LueOpFieldExist, 0ULL, 0, 0);
        (void)LueCmdPushResult(LueActionCpySwitchDomain,
                        LueResultCopy(LueFieldInL2CVlan, 0));
        }
    else
        {
        (void)LueCmdPushClause(LueFieldInL2SVlan,
                           LueOpFieldExist, 0ULL, 0, 0);

        (void)LueCmdPushResult(LueActionCpySwitchDomain,
                        LueResultCopy(LueFieldInL2SVlan, 0));
        }
    } // BuildVlanSwDomainRule


////////////////////////////////////////////////////////////////////////////////
/// \brief Install or Uninstall default zero switch domain rule.
///
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void ZeroSwDomainCfg(void)
    {
    LueCmdSetInst (Dnstream, LueInstCommon, 0);
    if(mcastBaseCfg.ipv4)
        {
        // Ipv4
        BuildZeroSwDomainRule(TRUE);
		(void)LueCmdAdd(mcastTrackingSysId, McastDisableSwDomainPri, FALSE);
        }

    if(mcastBaseCfg.ipv6)
        {
        // Ipv6
        BuildZeroSwDomainRule(FALSE);
		(void)LueCmdAdd(mcastTrackingSysId, McastDisableSwDomainPri, FALSE);
        }
    } // ZeroSwDomainCfg


////////////////////////////////////////////////////////////////////////////////
/// \brief Set the multicast vlan as the switch  domain
///
/// \param install Install or Uninstall
///
/// \return
/// TRUE if success, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
static
BOOL VlanSwDomainCfg(BOOL install)
    {
    LueHandle FAST handle = NULL;
    LueCmdSetInst (Dnstream, LueInstCommon, 0);

    // Ipv4
    if(mcastBaseCfg.ipv4)
        {
        BuildVlanSwDomainRule(TRUE);
        if(install)
            {
            handle = LueCmdAdd(mcastTrackingId,
                               McastEnableSwDomainPri, FALSE);
            if(handle == NULL)
                {
                return FALSE;
                }
            }
        else
            {
            (void)LueCmdDel(mcastTrackingId,
                               McastEnableSwDomainPri, FALSE);
            }
        }

    // Ipv6
    if(mcastBaseCfg.ipv6)
        {
        BuildVlanSwDomainRule(FALSE);
        if(install)
            {
            if(LueCmdAdd(mcastTrackingId,
                         McastEnableSwDomainPri, FALSE) == NULL)
                {
                if(handle != NULL)
                    {
                    (void)LueRuleDelByHandle(handle);
                    }
                return FALSE;
                }
            }
        else
            {
            (void)LueCmdDel(mcastTrackingId,
                               McastEnableSwDomainPri, FALSE);
            }
        }
    return TRUE;
    } // VlanSwDomainCfg


////////////////////////////////////////////////////////////////////////////////
/// \brief Set the multicast vlan as the switch  domain
///
/// \param install Install or Uninstall
///
/// \return
/// TRUE if success, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
static
BOOL SrcIpSwDomainCfg(const SwitchDomainInfo * domainInfo, BOOL install)
    {
    LueCmdSetInst (Dnstream, LueInstCommon, 0);
    LueCmdStart();
    if(domainInfo->field1 != LueFieldNumFields)
        {
        (void)LueCmdPushClause(domainInfo->field1,
                               LueOpEqual, domainInfo->value1, 0, 0);
        }
    if(domainInfo->field2 != LueFieldNumFields)
        {
        (void)LueCmdPushClause(domainInfo->field2,
                               LueOpEqual, domainInfo->value2, 0, 0);
        }
    (void)LueCmdPushResult(LueActionSetSwitchDomain, domainInfo->domainId);

    if(install)
        {
        return (LueCmdAdd(mcastTrackingId,
                          McastEnableSwDomainPri, FALSE) != NULL);
        }
    else
        {
        (void)LueCmdDel(mcastTrackingId, McastEnableSwDomainPri, FALSE);
        return TRUE;
        }
    } // SrcIpSwDomainCfg


////////////////////////////////////////////////////////////////////////////////
/// \brief Set a switch domain
///
/// \param domainInfo Information about this switch domain
/// \param install    Install or Uninstall
///
/// \return
/// TRUE if success, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL McastSwDomainCfg(const SwitchDomainInfo * domainInfo, BOOL install)
    {
    BOOL FAST ret = TRUE;
    switch(domainInfo->type)
        {
        case SwDomainVlan:
            ret = VlanSwDomainCfg(install);
            break;

        case SwDomainSrcIp:
            ret = SrcIpSwDomainCfg(domainInfo, install);
            break;

        default:
            break;
        }
    return ret;
    } // McastSwDomainCfg


//##############################################################################
//                      Snooping IGMP/MLD Frames
//##############################################################################

////////////////////////////////////////////////////////////////////////////////
/// \brief Build Mcast frame capture rule
///
/// \param v4 For Ipv4 or Ipv6 multicast
/// \param queue Queue to put
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void BuildFrameCaptureRule(BOOL v4, U16 queue)
    {
    LueCmdStart();
    BuildMcastFrameClause(v4);
    (void)LueCmdPushResult(LueActionSetQueue, queue);
    (void)LueCmdPushResult(LueActionVlanDelClr, 0);
    (void)LueCmdPushResult(LueActionVlanAddClr, 0);
    (void)LueCmdPushResult(LueActionDiscardClr, 0);
    } // BuildFrameCaptureRule


////////////////////////////////////////////////////////////////////////////////
/// \brief Install the rules for capture multicast frames
///
/// \param up     Upstream or Downstream
/// \param v4     For Ipv4 or Ipv6 multicast
/// \param queu   Queue number for the captured frames
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void CaptureMcastRule(BOOL up, BOOL v4, U16 queue)
    {
    BuildFrameCaptureRule(v4, queue);
	(void)LueCmdAdd(mcastTrackingSysId, McastSnoopPri, up);
    } // CaptureMcastRule


////////////////////////////////////////////////////////////////////////////////
/// \brief Capture the downstream IGMP/MLD frames into CPU
///
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void CaptureMcastDn(void)
    {
    Port BULK ponPort = {PortIfPon, LueProcPortIndex};
    TkOnuQueue BULK cpuQueue = McastPortRxQueue(ponPort);

    LueCmdSetInst (Dnstream, LueInstEgress, LueProcPortIndex);
    if(mcastBaseCfg.ipv4)
        {// IGMP
        CaptureMcastRule(FALSE, TRUE, cpuQueue);
        }
    if(mcastBaseCfg.ipv6)
        {// MLD
        CaptureMcastRule(FALSE, FALSE, cpuQueue);
        }
    } // CaptureMcastDn


////////////////////////////////////////////////////////////////////////////////
/// \brief Capture the upstream IGMP/MLD frames into CPU
///
/// \param port   Port to be captrued
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void CaptureMcastUp(TkOnuEthPort port)
    {
    Port BULK uniPort = {PortIfUni, port};
    TkOnuQueue BULK cpuQueue = McastPortRxQueue(uniPort);

    LueCmdSetInst (Upstream, LueInstEgress, port);
    if(mcastBaseCfg.ipv4)
        {// IGMP
        CaptureMcastRule(TRUE, TRUE, cpuQueue);
        }

    if(mcastBaseCfg.ipv6)
        {// MLD
        CaptureMcastRule(TRUE, FALSE, cpuQueue);
        }
    } // CaptureMcastUp


////////////////////////////////////////////////////////////////////////////////
/// \brief Install the rules for capturing the downstream/upstream IGMP/MLD 
///        frames into CPU
///
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void CaptureMcastFrame(void)
    {
    U8 FAST portIdx;
    for(portIdx = 0; portIdx < McastPortMaxNum; portIdx++)
        {
        CaptureMcastUp(portIdx);
        }
    CaptureMcastDn();
    } // CaptureMcastFrame



//##############################################################################
//                      Multicast Vlan Operation
//##############################################################################

////////////////////////////////////////////////////////////////////////////////
/// \brief Build multicast vlan strip rule
///
/// \param v4  Ipv4 or Ipv6
///
/// \return
/// TRUE if success, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
static
void BuildMcastVlanStripRule(BOOL v4)
    {
    LueCmdStart();
    BuildMcastMacClause(v4);
    if(mcastBaseCfg.vlanType == McastVlanCVlan)
        {
        (void)LueCmdPushClause(LueFieldInL2CVlan,
                           LueOpFieldExist, 0ULL, 0, 0);
        }
    else
        {
        (void)LueCmdPushClause(LueFieldInL2SVlan,
                           LueOpFieldExist, 0ULL, 0, 0);
        }
    (void)LueCmdPushResult(LueActionVlanDelSet, 0);
    } // BuildMcastVlanStripRule


////////////////////////////////////////////////////////////////////////////////
/// \brief Set rules to strip multicast vlan for the port
///
/// \param port Port to install
/// \param strip Strip or not
///
/// \return
/// TRUE if success, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL McastBaseVlanStrip(TkOnuEthPort port, BOOL strip)
    {
    LueHandle FAST handle = NULL;
    LueCmdSetInst(Dnstream, LueInstEgress, port);
    if(strip != stripMcastVlan[port])
        {
        if(mcastBaseCfg.ipv4)
            {
            // Ipv4
            BuildMcastVlanStripRule(TRUE);
            if(strip)
                {
                handle = LueCmdAdd(mcastTrackingId, McastVlanPri, FALSE);
                if(handle == NULL)
                    {
                    return FALSE;
                    }
                }
            else
                {
                (void)LueCmdDel(mcastTrackingId, McastVlanPri, FALSE);
                }
            }
        if(mcastBaseCfg.ipv6)
            {
            // ipv6
            BuildMcastVlanStripRule(FALSE);
            if(strip)
                {
                if(LueCmdAdd(mcastTrackingId, McastVlanPri, FALSE) == NULL)
                    {
                    if(handle != NULL)
                        {
                        (void)LueRuleDelByHandle(handle);
                        }
                    return FALSE;
                    }
                }
            else
                {
                (void)LueCmdDel(mcastTrackingId, McastVlanPri, FALSE);
                }
            }
        }
    stripMcastVlan[port] = strip;
    return TRUE;
    } // McastBaseVlanStrip


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
//extern
BOOL McastBaseVlan(TkOnuEthPort port, VlanTag mVlan, VlanTag uVlan, BOOL add)
    {
    BOOL FAST ret = TRUE;

    LueCmdSetInst(Dnstream, LueInstEgress, port);
    LueCmdStart();
    (void)LueCmdPushClause(LueFieldInL2CVlan, LueOpEqual,
                           (U64)McastVlanGetVid(mVlan),
                           McastVidLeftMask,
                           McastVidRightMask);

    if(McastVlanGetVid(uVlan) != McastVlanNonVid)
        { // With switch Vlan
        (void)LueCmdPushResult(LueActionVlanAddSet, 0);
        (void)LueCmdPushResult(LueActionVlanDelSet, 0);
        (void)LueCmdPushResult(LueActionSetVidCfi, McastVlanGetVid(uVlan));
        //(void)LueCmdPushResult(LueActionCpyCos,
        //                            (LueFieldInL2CVlan << 8)|13);
        (void)LueCmdPushResult(LueActionCpyTpid,
                                      (LueFieldInL2CVlan << 8)|16);
        }
    if(!TestBitsSet(mcastBaseCfg.opt, McastBaseCfgOptHostCtrl))
        {
        // For Snooping mode, clear discard here!
        (void)LueCmdPushResult(LueActionDiscardClr, 0);
        }

    if(add)
        {
        ret = (LueCmdAdd(mcastTrackingId, McastVlanPri, FALSE) != NULL);
        }
    else
        {
        (void)LueCmdDel(mcastTrackingId, McastVlanPri, FALSE);
        }

    return ret;
    } // McastBaseVlan


////////////////////////////////////////////////////////////////////////////////
/// \brief Build multicast port discard check rule
///
/// \param port  Port to build
/// \param v4    For Ipv4 or Ipv6 multicast
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void BuildPortDiscardCheckRule(TkOnuEthPort port, BOOL v4)
    {
    U16 BULK dnQueue = (U16)McastDbDnQueueGet(port);

    LueCmdStart();
    BuildMcastMacClause(v4);
    (void)LueCmdPushClause(LueFieldVarPortDiscard, LueOpEqual,
                           (U64)McastPortDiscValue(port),
                           McastPortDiscLMask(port),
                           McastPortDiscRMask(port));
    if(mcastBaseCfg.vlanType == McastVlanCVlan)
        {
        (void)LueCmdPushClause(LueFieldInL2CVlan,
                           LueOpFieldExist, 0ULL, 0, 0);
        }
    else
        {
        (void)LueCmdPushClause(LueFieldInL2SVlan,
                           LueOpFieldExist, 0ULL, 0, 0);
        }
    (void)LueCmdPushResult(LueActionSetQueue, dnQueue);
    if(TestBitsSet(mcastBaseCfg.opt, McastBaseCfgOptHostCtrl))
        {
        // For host control mode, clear discard here!
        (void)LueCmdPushResult(LueActionDiscardClr, 0);
        }
    } // BuildPortDiscardCheckRule


////////////////////////////////////////////////////////////////////////////////
/// \brief Install or Uninstall port discard rules
///
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void CheckPortDiscard(void)
    {
    U8 FAST i;
    for(i = 0; i < McastPortMaxNum; i++)
        {
        LueCmdSetInst(Dnstream, LueInstEgress, i);
        if(mcastBaseCfg.ipv4)
            {
            // Ipv4
            BuildPortDiscardCheckRule(i, TRUE);
			(void)LueCmdAdd(mcastTrackingSysId, 
				            McastPortDiscardMapPri, FALSE);
            }

        if(mcastBaseCfg.ipv6)
            {
            // Ipv6
            BuildPortDiscardCheckRule(i, FALSE);
            (void)LueCmdAdd(mcastTrackingSysId, 
				            McastPortDiscardMapPri, FALSE);
            }
        }
    } // CheckPortDiscard


//##############################################################################
//                      Multicast Group APIs
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
BOOL GroupBinEntryCompare (const LueInstance BULK *instance,
                           const LueBinEntry BULK *entry1,
                           const LueBinEntry BULK *entry2)
    {
    LueBinField FAST searchField;
    searchField = LueBinFieldSelectGet (instance);
    if (searchField == LueBinFieldDa)
        {
        return
            (entry1->entry.da.switchDomain == entry2->entry.da.switchDomain) &&
            (memcmp (&entry1->entry.da.macAddr,
                     &entry2->entry.da.macAddr,
                     sizeof(MacAddr)) == 0);
        }
    else if (searchField == LueBinFieldSsmSrcId)
        {
        return
            (entry1->entry.ssm.ipDa == entry2->entry.ssm.ipDa) &&
            (entry1->entry.ssm.switchDomain == entry2->entry.ssm.switchDomain);
        }
    else
        {
        return FALSE;
        }
    } // GroupBinEntryCompare


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
/// TRUE if add successfully
////////////////////////////////////////////////////////////////////////////////
static
BOOL GroupBinAdd (const LueInstance BULK *instance, LueBinEntry BULK *entry)
    {
    LueBinEntry BULK old;

    LueCmdStart ();
    LueCmdUseInst (instance);
    (void)LueCmdPushEntry (entry->learnDomain,
                           entry->isStatic,
                           entry->rightMask,
                           &entry->entry);
    if ((LueCmdQuery (&old) == TRUE) &&
        GroupBinEntryCompare (instance, entry, &old))
        {
        GroupEntryPortMapSet (instance, entry,
                              (GroupEntryPortMapGet (instance, entry) |
                               GroupEntryPortMapGet (instance, &old)));
        (void)LueBinDeleteEntry (instance, &old, LueBinOptNone);
        LueBinCopyBalance (instance);
        }
    LueCmdStart ();
    (void)LueCmdPushEntry (entry->learnDomain,
                           entry->isStatic,
                           entry->rightMask,
                           &entry->entry);
    (void)LueCmdPushResult (LueActionOrPortDiscard,
                            GroupEntryPortMapGet(instance, entry));


    if (LueCmdAdd (mcastTrackingId,
                   McastGrpBinEntryPri, FALSE) == NULL)
        {
        LueBinCopyBalance (instance);
        return (LueCmdAdd (mcastTrackingId,
                   McastGrpBinEntryPri, FALSE) != NULL);
        }
    return TRUE;
    } // GroupBinAdd


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
/// TRUE if delete successfully
////////////////////////////////////////////////////////////////////////////////
static
BOOL GroupBinDel (const LueInstance BULK *instance, LueBinEntry BULK *entry)
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
        GroupBinEntryCompare (instance, entry, &old))
        {
        newMap = GroupEntryPortMapGet (instance, entry);
        oldMap = GroupEntryPortMapGet (instance, &old);
        if ((newMap & oldMap) == 0)
            {
            return FALSE;
            }
        (void)LueBinDeleteEntry (instance, &old, LueBinOptNone);
        LueBinCopyBalance (instance);
        if ((newMap ^ oldMap) == 0)
            {
            return TRUE;
            }
        GroupEntryPortMapSet (instance, entry, (newMap ^ oldMap));
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
    (void)LueCmdPushResult (LueActionOrPortDiscard,
                            GroupEntryPortMapGet(instance, entry));

    if (LueCmdAdd (mcastTrackingId, McastGrpBinEntryPri, FALSE) == NULL)
        {
        LueBinCopyBalance (instance);
        return (LueCmdAdd (mcastTrackingId,
                           McastGrpBinEntryPri, FALSE) != NULL);
        }
    return TRUE;
    } // GroupBinDel


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
//extern
BOOL McastGrpBinAddMac(TkOnuEthPort port, const MacAddr BULK *mac, U16 domain)
    {
    LueBinEntry BULK entry;

    entry.isStatic = TRUE;
    entry.rightMask = 4;

    entry.entry.da.macAddr = *mac;
    entry.entry.da.switchDomain = domain;
    entry.entry.da.linkId = 0;

    GroupEntryPortMapSet (&McastMacInstance, &entry, (U16)(1U << port));

    return GroupBinAdd (&McastMacInstance, &entry);
    } // McastGrpBinAddMac


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
//extern
BOOL McastGrpBinDelMac (TkOnuEthPort port, const MacAddr BULK *mac, U16 domain)
    {
    LueBinEntry BULK entry;

    entry.isStatic = TRUE;
    entry.rightMask = 4;

    entry.entry.da.macAddr = *mac;
    entry.entry.da.switchDomain = domain;
    entry.entry.da.linkId = 0;

    GroupEntryPortMapSet (&McastMacInstance, &entry, (U16)(1U << port));

    return GroupBinDel (&McastMacInstance, &entry);
    } // McastGrpBinDelMac



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
//extern
BOOL McastGrpBinAddIp (TkOnuEthPort port, U32 ipLowBits, U16 domain)
    {
    LueBinEntry BULK entry;

    entry.isStatic = TRUE;
    entry.rightMask = 0;

    entry.entry.ssm.ipDa = ipLowBits;
    entry.entry.ssm.ssm = 0;
    entry.entry.ssm.switchDomain = domain;

    GroupEntryPortMapSet (&McastIpInstance, &entry, (U16)(1U << port));

    return GroupBinAdd (&McastIpInstance, &entry);
    } // McastGrpBinAddIp


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
//extern
BOOL McastGrpBinDelIp (TkOnuEthPort port, U32 ipLowBits, U16 domain)
    {
    LueBinEntry BULK entry;

    entry.isStatic = TRUE;
    entry.rightMask = 0;

    entry.entry.ssm.ipDa = ipLowBits;
    entry.entry.ssm.ssm = 0;
    entry.entry.ssm.switchDomain = domain;

    GroupEntryPortMapSet (&McastIpInstance, &entry, (U16)(1U << port));

    return GroupBinDel (&McastIpInstance, &entry);
    } // McastGrpBinDelIpv4


////////////////////////////////////////////////////////////////////////////////
/// \brief Re-install the multicast system rules.
///
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void McastBaseRuleReInstall(void)
	{
	if(mcastTrackingSysId != LueTrackIdInvalid)
		{
		// Clear the old rules
		(void)LueRuleDelByGroup (mcastTrackingSysId);

		// Add the new rules
		if(mcastBaseCfg.ipv4 || mcastBaseCfg.ipv6)
			{
			if(LueBinFieldSelectGet(&McastMacInstance)
                                != LueBinFieldDa)
	            {
	            LueBinFieldSelectSet(&McastMacInstance,
	                                LueBinFieldDa);
	            }
	        if(LueBinFieldSelectGet(&McastIpInstance)
	                                != LueBinFieldSsmSrcId)
	            {
	            LueBinFieldSelectSet(&McastIpInstance,
	                                LueBinFieldSsmSrcId);
	            }
			FilterMcast();
			CaptureMcastFrame();
			ZeroSwDomainCfg();
			CheckPortDiscard();
			}
		}
	} // McastBaseRuleReInstall


////////////////////////////////////////////////////////////////////////////////
/// \brief Configure the multicast base module
///
/// \param cfg Configuration of multicast base module
///
/// \return
/// TRUE if sucessful, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
//extern
void McastBaseConfig(const McastBaseCfg BULK * BULK cfg)
    {
    if(mcastTrackingId == LueTrackIdInvalid)
    	{
		mcastTrackingId = LueTrackIdCreate ();
		}
	else
		{
		(void)LueRuleDelByGroup (mcastTrackingId);
		}

	if(mcastTrackingSysId == LueTrackIdInvalid)
    	{
		mcastTrackingSysId = LueTrackIdCreate ();
		}
	else
		{
		(void)LueRuleDelByGroup (mcastTrackingSysId);
		}
	
    memcpy(&mcastBaseCfg, cfg, sizeof(McastBaseCfg));
	McastBaseRuleReInstall();
    } // McastBaseConfig


////////////////////////////////////////////////////////////////////////////////
/// \brief Initialize the multicast module data.
///
////////////////////////////////////////////////////////////////////////////////
//extern
void McastBaseInit(void)
    {
    memset(&mcastBaseCfg, 0, sizeof(McastBaseCfg));
    memset(stripMcastVlan,   0, sizeof(stripMcastVlan));
    } // McastBaseInit

// end of McastBaseRule.c


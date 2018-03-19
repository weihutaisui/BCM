/*
<:copyright-BRCM:2002-2003:proprietary:epon

   Copyright (c) 2002-2003 Broadcom 
   All Rights Reserved

 This program is the proprietary software of Broadcom and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").  Except as set forth in
 an Authorized License, Broadcom grants no license (express or implied), right
 to use, or waiver of any kind with respect to the Software, and Broadcom
 expressly reserves all rights in and to the Software and all intellectual
 property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,

 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use
    all reasonable efforts to protect the confidentiality thereof, and to
    use this information only in connection with your use of Broadcom
    integrated circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
    PERFORMANCE OF THE SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
    LIMITED REMEDY.
:>*/
////////////////////////////////////////////////////////////////////////////////
/// \file CtcMcast.c
/// \brief China Telecom multicast control module
///
////////////////////////////////////////////////////////////////////////////////
#include "Mcast.h"
#include "CtcOamDiscovery.h"
#include "CtcVlan.h"
#include "CtcMcast.h"

#ifdef EPON_SFU 
extern int netlinkSnoopFd;

U8 CtcToHostForGroupType(U8 ctcType)
{
    U8 hostType;
    
    switch (ctcType)
    {
    case CtcMcastGrpDaMacOnly:
        hostType = McastGrpDaMacOnly;
        break;
    case CtcMcastGrpDaMacVid:
        hostType = McastGrpDaMacVid;
        break;    
    case CtcMcastGrpDaIpv4Vid:
        hostType = McastGrpDaIpv4Vid;
        break;
    case CtcMcastGrpDaIpv6Vid:
        hostType = McastGrpDaIpv6Vid;
        break;
    default:
        hostType = McastGrpTypeInvalid;
        break;
    }
    
    return hostType;
}

U8 HostToCtcForGroupType(U8 hostType)
{
    U8 ctcType;
    
    switch (hostType)
    {
    case McastGrpDaMacOnly:
        ctcType = CtcMcastGrpDaMacOnly;
        break;
    case McastGrpDaMacVid:
        ctcType = CtcMcastGrpDaMacVid;
        break;    
    case McastGrpDaIpv4Vid:
        ctcType = CtcMcastGrpDaIpv4Vid;
        break;
    case McastGrpDaIpv6Vid:
        ctcType = CtcMcastGrpDaIpv6Vid;
        break;
    default:
        ctcType = CtcMcastGrpTypeInvalid;
        break;
    }
    
    return ctcType;
}
#endif

int CtcMcastParseFdbEntry(IptvEntry *pEntry)
{
#ifdef EPON_SFU 
    if (pEntry->userId < 1)
    {
        cmsLog_error("userId should start at 1!\n");
        return 0;
    }

    pEntry->userId = pEntry->userId - 1;
 
    McastVlanActionOnPortGet(pEntry);
    
#endif 
    return 1;
}

int CtcMcastFindFdbEntryIndex(U8 ctltype, U8 *entry, U16 *foundIndex)
{
#ifdef EPON_SFU 
    int i;   
    for (i = 0; i < McastFdbEntryNumGet(); i++) 
    {
        switch (ctltype)
        {
            case McastGrpDaMacOnly:
            case McastGrpDaMacVid:
            {
                mcastEntry_1* mcastEntry = (mcastEntry_1*)entry;
                if ((McastFdbUserIdGet(i) + 1 == OAM_NTOHS(mcastEntry->userId))  
                        && (McastFdbVlanGet(i) == OAM_NTOHS(mcastEntry->vlanTag))  
                        && (McastFdbDaCmp(i, mcastEntry->da.u8) == 0) )
                {
                    *foundIndex = i;
                    return 1;
                }
                
                break;
            } 
            case McastGrpDaIpv4Vid:
            {
                mcastEntry_3* mcastEntry = (mcastEntry_3*)entry;
                if ((McastFdbUserIdGet(i) + 1 == OAM_NTOHS(mcastEntry->userId))
                        && (McastFdbVlanGet(i) == OAM_NTOHS(mcastEntry->vlanTag))  
                        && (McastFdbGroupIpCmp(i, mcastEntry->groupIp, IpFamilyIpV4) == 0) )
                {
                    *foundIndex = i;
                    return 1;                    
                }
                
                break;
            }
            case McastGrpDaIpv6Vid:
            {
                mcastEntry_4* mcastEntry = (mcastEntry_4*)entry;
                if ((McastFdbUserIdGet(i) + 1 == OAM_NTOHS(mcastEntry->userId))
                        && (McastFdbVlanGet(i) == OAM_NTOHS(mcastEntry->vlanTag))  
                        && (McastFdbGroupIpCmp(i, mcastEntry->groupIp, IpFamilyIpV6) == 0) )
                {
                    *foundIndex = i;
                    return 1;                    
                }
                
                break;
            }            
        } 
    }
#endif
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Get grpIdx group entry from the port
///
/// \Param port      Port to get
/// \param ctrlEntry Group entry buffer
///
/// \return
/// The size of this entry, zero means reaching end or error
////////////////////////////////////////////////////////////////////////////////
U8 CtcMcastCtrlGrpGet(OamVarContainer *cont)
{
#ifdef EPON_SFU    
    OamCtcTlvMcastCtrl *pMcastCtrl = NULL;
    U8  *pEntry    = NULL;
    U8  entrySize  = 0;
    U8  ctrlType   = McastCtrlTypeGet();
    U8  numEntries = McastFdbEntryNumGet();
    U32 index      = 0;
    
    #ifdef CTC_MCAST_DEBUG
    McastDumpConfigInfo(0xFF);
    #endif
	
    cmsLog_notice("numEntries:%d",  numEntries );	
    
    entrySize = McastFdbEntrySizeGet(McastCtrlTypeGet());
    
    if (numEntries) 
    {
        cont->length = sizeof (OamCtcTlvMcastCtrlHdr);
        
        while (TRUE) 
        {
            if (cont->length == sizeof (OamCtcTlvMcastCtrlHdr))
            {
                pMcastCtrl = (OamCtcTlvMcastCtrl BULK *)(&cont->value[0]);
                pMcastCtrl->action = OamCtcMcastGroupList;
                pMcastCtrl->ctrlType = HostToCtcForGroupType(ctrlType);
                pMcastCtrl->numEntries = numEntries - index;
            }

            pEntry = pMcastCtrl->entry;
            for (; index < numEntries; index++)
            {
                 // CTC suerId starts at 1
                ((mcastEntry_1*)pEntry)->userId = OAM_HTONS(McastFdbUserIdGet(index) + 1);

                switch (ctrlType)
                {
                    case McastGrpDaMacOnly:
                    case McastGrpDaMacVid:
                    {
                        mcastEntry_1* entry = (mcastEntry_1*)pEntry;
                        entry->vlanTag = OAM_HTONS(McastFdbVlanGet(index));
                        McastFdbDaCpy(index, entry->da.u8);
                        
                        break;
                    }
                    case McastGrpDaIpv4Vid:
                    {
                        mcastEntry_3* entry = (mcastEntry_3*)pEntry;
                        entry->vlanTag = OAM_HTONS(McastFdbVlanGet(index));
                        McastFdbIpCpy(index, entry->groupIp, IpFamilyIpV4);
                        
                        break;
                    }                    
                    case McastGrpDaIpv6Vid:
                    {
                        mcastEntry_4* entry = (mcastEntry_4*)pEntry;
                        entry->vlanTag = OAM_HTONS(McastFdbVlanGet(index));
                        McastFdbIpCpy(index, entry->groupIp, IpFamilyIpV6);
                        
                        break;
                    }    
                }
                
                pEntry += entrySize;
                cont->length += entrySize;
                
                if (index >= numEntries -1)
                {
                    return TRUE;
                }
                
                if (cont->length + entrySize > 0x80)
                {
                    StreamSkip(&oamParser.reply, OamContSize(cont));
                    cont = OamCtcNextCont (cont);	        
                    cont->length = sizeof (OamCtcTlvMcastCtrlHdr);
                    index++;
                    
                    break;
                }
            }
           
        }
    }
    else
    {
        cont->branch = OamCtcBranchExtAttribute;
        cont->leaf = OAM_HTONS(OamCtcAttrMcastCtrl);
        pMcastCtrl = (OamCtcTlvMcastCtrl  *)(&cont->value[0]);
        pMcastCtrl->action = OamCtcMcastGroupList;
        
        if (ctrlType == McastGrpTypeInvalid)
        {
            pMcastCtrl->ctrlType = CtcMcastGrpDaMacOnly;
        }
        else
        {
            pMcastCtrl->ctrlType = HostToCtcForGroupType(ctrlType);
        }
        pMcastCtrl->numEntries = 0;
        cont->length = sizeof (OamCtcTlvMcastCtrlHdr);   
    }
#else
    printf("%s: this is SFU feature. not supported in HGU\n", __FUNCTION__);
    cont->length = OamVarErrAttrUnsupported;
#endif	
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Add a group entry from the port
///
/// \Param port    Port to add
/// \Param grpType Group Type to add
/// \Param ctrlEntry  Point to group entry
///
/// \return
/// 0 if success, failed otherwise
////////////////////////////////////////////////////////////////////////////////
//extern
int CtcMcastCtrlGrpSet(OamVarContainer *src, OamVarContainer *cont)
{
#ifdef EPON_SFU
    U32 i             =  0;
    U32 ret           =  0;
    U16 foundIndex    =  0;   
    U16 index         =  0;
    int leftEntryLen  =  0;
    int lookUpMethod  = -1;
    int masgelen      = src->length;
    U8  entrySize     =  0;
    U8  mcastCtrlType =  0; 
    IptvEntry fdbEntry;
    OamCtcTlvMcastCtrl *pMcastCtrl = (OamCtcTlvMcastCtrl *)src->value;
    int numEntries    = pMcastCtrl->numEntries;
    U8  *pEntry       = pMcastCtrl->entry;
 
    cmsLog_notice("action:%d, ctrlType:%d, numEntries:%d", 
		pMcastCtrl->action, pMcastCtrl->ctrlType, pMcastCtrl->numEntries);
	
    if (McastModeGet() != McastHostMode)
    {
        cmsLog_error("Error: The multicast mode is not McastHostMode \n");
        return -1;
    }

    if (pMcastCtrl->ctrlType > CtcMcastGrpDaMacSaIpv6)
    {
        cmsLog_error("Invalid Multicast Control Type \n");
        return -1;
    }

    /*now rdpa only supports these multicast control types: 
        00: GDA mac only
        01: GDA mac + vlan
        03: GDA IPv4 + vlan
        04: GDA IPv6 + vlan
    */
    if ((pMcastCtrl->ctrlType == CtcMcastGrpDaMacSaIpv4) || (pMcastCtrl->ctrlType == CtcMcastGrpDaMacSaIpv6))
    {
        cmsLog_error("Multicast control type %d is not supported now\n", pMcastCtrl->ctrlType);
        return -1;
    }    

    mcastCtrlType = CtcToHostForGroupType(pMcastCtrl->ctrlType);

    if ((entrySize = McastFdbEntrySizeGet(mcastCtrlType)) == 0xFF)
    {
        cmsLog_error("error entry size: %d\n", entrySize);
        return -1;
    }
    
    leftEntryLen = masgelen - 3*sizeof(pMcastCtrl->action);//offset action, ctrlType and numEntries 
    
    for (i = 0; i < pMcastCtrl->numEntries; i++)
    {
        if (McastCheckFdbEntry(mcastCtrlType, pEntry) != 0)
        {
            cmsLog_error("invalid parameters at entry %d\n", i);
            return -1;
        }
        pEntry += entrySize;

        leftEntryLen -= entrySize;
        
        if (leftEntryLen == 0)
        {
            numEntries = i + 1;       
            break;
        }
    }

    pEntry = pMcastCtrl->entry;

    lookUpMethod = McastConvertControlType(mcastCtrlType);
    if (lookUpMethod < 0)
    {
        cmsLog_error("not supported multicast control type");
        return -1;
    }
    
    switch (pMcastCtrl->action)
    {
        case OamCtcMcastGroupAdd:
        {
            if ((numEntries + McastFdbEntryNumGet()) > (MAX_MCASTS_PER_PORT * UniCfgDbGetActivePortCount()))
            {
                cmsLog_error("Max number of Mcast entries supported per port = %d \n", MAX_MCASTS_PER_PORT);
                return -1;
            }
            
            if (McastCtrlTypeGet() != mcastCtrlType)
            {
                for (i = 0; i < UniCfgDbGetActivePortCount(); i++)
                {
                    if((ret = McastDelFdbEntriesOnPort(i, McastFdbOpDelTmp)) != 0)
                    {
                        return ret;
                    }
                }
                
                McastFdbEntryNumSet(0);

                rdpaCtl_IptvEntryFlush();

                ret = rdpaCtl_IptvLookupMethodSet(lookUpMethod);
                if (ret)
                {
                    cmsLog_error("failed to change iptv lookup method: %d\n", pMcastCtrl->ctrlType);
                    return ret;
                }
                
                McastCtrlTypeSet(mcastCtrlType);
            }
            
            for (i = 0; i < numEntries; i++)
            {
                if (CtcMcastFindFdbEntryIndex(mcastCtrlType, pEntry, &foundIndex) == 0) 
                {                  
                    if((ret = McastParseFdbEntry(mcastCtrlType, pEntry, &fdbEntry)) != 0)
                    {
                        cmsLog_error("not supported control type: %d\n", pMcastCtrl->ctrlType);
                        return ret;
                    }

                    if(!CtcMcastParseFdbEntry(&fdbEntry))
                    {
                        cmsLog_error("have a wrong userId");
                        return -1;
                    }

                    if((ret = McastAddOrRemoveFdbEntry(mcastCtrlType, &fdbEntry,
                                                            &index, McastFdbOpAdd)) != 0)
                    {
                        cmsLog_error("failed to add iptv entry");
                        return -1;
                    }  

                    if ((lookUpMethod == RDPA_IOCTL_IPTV_METHOD_MAC_VID) || (lookUpMethod == RDPA_IOCTL_IPTV_METHOD_GROUP_IP_SOURCE_IP_VID))
                    {   
                        if (fdbEntry.vlanTag != 0)
                        {
                            McastHandleVlanRule(fdbEntry.userId, fdbEntry.vlanTag, (pMcastCtrl->action != OamCtcMcastGroupAdd));
                        }
                    }
                }
                
                pEntry += entrySize;
            }
            
            break;
        }
        
        case OamCtcMcastGroupDel:
        {
            for (i = 0; i < numEntries; i++) 
            {
                mcastEntry_1* mcastEntry = (mcastEntry_1*)pEntry;
                
                if (CtcMcastFindFdbEntryIndex(mcastCtrlType, pEntry, &foundIndex))
                {
                    if ((ret = McastParseFdbEntry(mcastCtrlType, pEntry, &fdbEntry)) != 0)
                    {
                        cmsLog_error("not supported control type: %d\n", pMcastCtrl->ctrlType);
                        return ret;
                    }
                    
                    if (!CtcMcastParseFdbEntry(&fdbEntry))
                    {
                        cmsLog_error("have a wrong userId");
                        return -1;
                    }

                    if ((ret = McastAddOrRemoveFdbEntry(mcastCtrlType, &fdbEntry, &foundIndex, McastFdbOpDel)) != 0)
                    {
                        cmsLog_error("failed to add iptv entry");
                        return ret;
                    }
                    
                    if ((lookUpMethod == RDPA_IOCTL_IPTV_METHOD_MAC_VID) || (lookUpMethod == RDPA_IOCTL_IPTV_METHOD_GROUP_IP_SOURCE_IP_VID))
                    {   
                        if (fdbEntry.vlanTag != 0)
                        {
                            McastHandleVlanRule(fdbEntry.userId, fdbEntry.vlanTag, (pMcastCtrl->action != OamCtcMcastGroupAdd));
                        }
                    }
                }
                else
                {
                    // not found, print error
                    cmsLog_error("Multicast entry 0x%06x%06x not found\n", OAM_NTOHS(mcastEntry->da.lowHi.hi),
                                        OAM_NTOHL(mcastEntry->da.lowHi.low));
                    
                    if (McastFdbEntryNumGet() == 0) //directly return error when mcast lib is empty
                    {
                        return -1;
                    }
                }
                pEntry += entrySize;
            }
            
            break;
        }
        
        case OamCtcMcastGroupClear:
        {
            for (i = 0; i < UniCfgDbGetActivePortCount(); i++)
            {
                if ((ret = McastDelFdbEntriesOnPort(i, McastFdbOpDelTmp)) != 0)
                {
                    return ret;
                }
            }

            McastFdbEntryNumSet(0);

            break;
        }
        
        default:
            break;

    }
#else
    printf("%s: this is SFU feature. not supported in HGU\n", __FUNCTION__);
#endif
    return 0;
} 

////////////////////////////////////////////////////////////////////////////////
/// \brief Set the multicast vlan tag process for the port
///
/// \Param port Port to set
/// \param cfg  Configuration to set
///
/// \return
/// 0 if successful, fail otherwise
////////////////////////////////////////////////////////////////////////////////
int CtcMcastTagOpSet(TkOnuEthPort port, const CtcMcastTagOpCfg * cfg)
{
    int ret = 0;
#ifdef EPON_SFU  
    ret = McastTagOpSet(port, cfg->switchVlanNum, cfg->op, cfg->entry);
#else
    printf("%s: this is SFU feature. not supported in HGU\n", __FUNCTION__);
#endif    
    return ret;
} 

////////////////////////////////////////////////////////////////////////////////
/// \brief Set the multicast vlan tag process for the port
///
/// \Param port Port to get
/// \param cfg  Pointer to the configuration buffer to get
///
/// \return
/// The length of the configuration
////////////////////////////////////////////////////////////////////////////////
U8 CtcMcastTagOpGet(TkOnuEthPort port, CtcMcastTagOpCfg * cfg)
{
    U8 len  = 0;
#ifdef EPON_SFU  
    U8  i   = 0;

    cfg->op = McastPortVlanOpGet(port);
    len += sizeof(cfg->op);
    
#ifdef CTC_MCAST_DEBUG
    McastDumpConfigInfo(port);
#endif

    if (cfg->op == McastVlanActTrans)
    {
        cfg->switchVlanNum = McastPortVlanSwitchNumGet(port);
        len += sizeof(cfg->switchVlanNum);

        for (i = 0; i < cfg->switchVlanNum; i++)
        {   
            cfg->entry[i].eponVid = OAM_HTONS(McastPortEntryMcastVlanGet(port, i));
            len += sizeof(VlanTag);
            cfg->entry[i].iptvVid = OAM_HTONS(McastPortEntryUserVlanGet(port, i));
            len += sizeof(VlanTag);             
        }  
    }
#else
    printf("%s: this is SFU feature. not supported in HGU\n", __FUNCTION__);
#endif

    return len;
}

/*analogous to setCtcMcastVlanData in 6828 solution.
    only call this function in IGMP/MLD snooping mode. 
    the port is already converted by OamCtcPortToEth.
*/
int CtcMcastVlanSet(TkOnuEthPort port, U8 len,
                    const CtcMcastVlanOpCfg * cfg, int TriggeredByMcGroup)      
{
    int ret     = 0; 
#ifdef EPON_SFU
    U8  vidNum  = 0;
    U8  vlanOp  = cfg->op;
    U16 *vlanId = (U16 *)cfg->vlan;

    if (TriggeredByMcGroup == FALSE && McastModeGet() == McastHostMode)
    {
        /* Nothing to do as this does not apply for CTC Host Controllable Mode */
        return -1;
    }

    if (len != 0)
    {   
        vidNum = (len - sizeof(cfg->op))/sizeof(VlanTag);
        ret = McastVlanTagSet(port, vidNum, vlanId, vlanOp);
    }
#endif
    return ret;
} 

////////////////////////////////////////////////////////////////////////////////
/// \brief Get the multicast vlanfor the port
///
/// \Param port Port to get
/// \param cfg  Pointer to the configuration buffer to get
///
/// \return
/// The length of the configuration
////////////////////////////////////////////////////////////////////////////////
U8 CtcMcastVlanGet(TkOnuEthPort port, CtcMcastVlanOpCfg * cfg)
{
    U8 FAST len = 0;    
#ifdef EPON_SFU    
    int FAST i;
    VlanTag *pVid = cfg->vlan;

    cfg->op = McastVlanOpList;
    len += sizeof(cfg->op);
    
    #ifdef CTC_MCAST_DEBUG
    McastDumpConfigInfo(port);
    #endif    
    
    for (i = 0; i < McastPortVidNumGet(port); i++)
    {
        pVid[i] = OAM_HTONS(McastPortVidGet(port, i));
        len += sizeof(VlanTag);
    }
#else
    printf("%s: this is SFU feature. not supported in HGU\n", __FUNCTION__);
#endif
    return len;
} 

////////////////////////////////////////////////////////////////////////////////
/// \brief Set the multicast vlan
///
/// \Param port Port to set
/// \Param max  Maximum number of groups for the port
///
/// \return
/// TRUE if successful, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
//extern
int CtcMcastGroupMaxSet(TkOnuEthPort port, U8 max)
{
    int ret    = 0;
#ifdef EPON_SFU    
    U8  oldMax = 0;
#endif
    if (!CtcMcastIsSnoopingMode( ))
    {
        cmsLog_error("Group Num Max only support for Snooping mode.\n");
        return -1;
    }
    
    if (max <= MAX_MCASTS_PER_PORT)
    {
#ifdef EPON_SFU    
        oldMax = McastGroupMaxGet(port);
        McastGroupMaxSet(port, max);
        if (!McastSetMaxGroupsPortList())
        {
            McastGroupMaxSet(port, oldMax);
            ret = -1;
        }
#endif        
    }
    else
    {
        ret = -1;
    }
    
    return ret; 
} 

////////////////////////////////////////////////////////////////////////////////
/// \brief Get the multicast vlan
///
/// \Param port Port to get
///
/// \return
/// Maximum number of groups for the port
////////////////////////////////////////////////////////////////////////////////
//extern
U8 CtcMcastGroupMaxGet(TkOnuEthPort port)
{
#ifdef EPON_SFU
    return McastGroupMaxGet(port); 
#else
    return 0;
#endif
} 

////////////////////////////////////////////////////////////////////////////////
/// \brief Set the fast leave admin control
///
/// \Param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
int CtcMcastFastLeaveStateSet(CtcMcastFastLeaveState admin)
{
#ifdef BRCM_CMS_BUILD
#ifdef EPON_SFU 
#if defined(DMP_X_BROADCOM_COM_IGMP_1) || defined(DMP_X_BROADCOM_COM_MLD_1)
    CmsRet ret = CMSRET_SUCCESS;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
#endif
#ifdef DMP_X_BROADCOM_COM_MLD_1
    MldCfgObject *mldObj = NULL;
#endif
#ifdef DMP_X_BROADCOM_COM_IGMP_1
    IgmpCfgObject *igmpObj = NULL;
    
    if ((ret = cmsLck_acquireLockWithTimeout(5000)) != CMSRET_SUCCESS)
    {
        cmsLog_error("could not acquire lock oam , ret=%d", ret);
        return -1;
    }
    
    if ((ret = cmsObj_get(MDMOID_IGMP_CFG, &iidStack, 0, (void **) &igmpObj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get igmpObj, ret=%d", ret);
        cmsLck_releaseLock();
        return -1;
    }
    
    if (admin == CtcMcastFastLeaveEnable)
    {
        igmpObj->igmpFastLeaveEnable = 1;
    }
    else
    {
        igmpObj->igmpFastLeaveEnable = 0;
    }
    
    cmsObj_set(igmpObj, &iidStack);    
    cmsObj_free((void **) &igmpObj);    
    cmsLck_releaseLock();
#endif

#ifdef DMP_X_BROADCOM_COM_MLD_1
    if ((ret = cmsLck_acquireLockWithTimeout(5000)) != CMSRET_SUCCESS)
    {
        cmsLog_error("could not acquire lock oam , ret=%d", ret);
        return -1;
    }
    
    if ((ret = cmsObj_get(MDMOID_MLD_CFG, &iidStack, 0, (void **) &mldObj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get mldObj, ret=%d", ret);
        cmsLck_releaseLock();
        return -1;
    }
    
    if (admin == CtcMcastFastLeaveEnable)
    {
        mldObj->mldFastLeaveEnable = 1;
    }
    else
    {
        mldObj->mldFastLeaveEnable = 0;
    }
    
    cmsObj_set(mldObj, &iidStack);    
    cmsObj_free((void **) &mldObj);    
    cmsLck_releaseLock();
#endif
    cmsLog_notice("fast leave state %d \n", admin);
#else
    printf("%s: this is SFU feature. not supported in HGU\n", __FUNCTION__);
#endif
#endif
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Get the fast leave state
///
/// \Param None
///
/// \return
/// The current fast leave state
////////////////////////////////////////////////////////////////////////////////
//extern
CtcMcastFastLeaveState CtcMcastFastLeaveStateGet(void)
{
#ifdef BRCM_CMS_BUILD
    U32 fastLeaveState = 2;
    
#ifdef EPON_SFU    
#ifdef DMP_X_BROADCOM_COM_IGMP_1
    CmsRet ret               = CMSRET_SUCCESS;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    IgmpCfgObject *igmpObj   = NULL;    
    
    if ((ret = cmsLck_acquireLockWithTimeout(50000)) != CMSRET_SUCCESS )
    {
        cmsLog_error("could not acquire lock oam , ret=%d", ret);
        return CtcMcastFastLeaveDisable;
    }
    
    if ((ret = cmsObj_get(MDMOID_IGMP_CFG, &iidStack, 0, (void **) &igmpObj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get igmpObj, ret=%d", ret);
        cmsLck_releaseLock();
        return CtcMcastFastLeaveDisable;
    }
    
    if (igmpObj->igmpFastLeaveEnable)
    {
        fastLeaveState = CtcMcastFastLeaveEnable;
    }
    else
    {
        fastLeaveState = CtcMcastFastLeaveDisable;
    }
    
    cmsObj_free((void **) &igmpObj);
    
    cmsLck_releaseLock();
#else
    fastLeaveState = CtcMcastFastLeaveDisable;
#endif
#else
    printf("%s: this is SFU feature. not supported in HGU\n", __FUNCTION__);
#endif    
    return fastLeaveState;
#else
    return CtcMcastFastLeaveDisable;
#endif
} 

////////////////////////////////////////////////////////////////////////////////
/// \brief Set IP multicast control method: igmp/mld snooping or host controllable mode
///
/// \Param mode The new mode to set
///
/// \return
/// 0: success; other value: fail
////////////////////////////////////////////////////////////////////////////////
int CtcMcastSwitchSetMode (OamCtcMcastMode mode)
{
#if defined(EPON_SFU) && (defined(DMP_X_BROADCOM_COM_IGMP_1) || defined(DMP_X_BROADCOM_COM_MLD_1))
    int ret             = 0;
    int enableSnooping  = 0;
	U8  port            = 0;
    cmsLog_notice("mode = %d", mode);

    if (mode > OamCtcMcastHostMode)
    {
        cmsLog_error("Invalid Multicast Mode (%d)", mode);
        return -1;
    }
	
    if (McastModeGet() == mode)
    {
        return 0;
    }
    
    ret = McastConfigClear();
    
    if (ret)
    {
        cmsLog_error("Multicast Config Clear Failed");
        return -1;
    }

    if (mode == OamCtcMcastIgmpMode)
    {
        enableSnooping = 1;
        
        bcm_mcast_api_unregister(netlinkSnoopFd, 1);

        McastModeSet(McastIgmpMode);
        
        rdpaCtl_IptvEntryFlush();
        
        ret = rdpaCtl_IptvLookupMethodSet(RDPA_IOCTL_IPTV_METHOD_GROUP_IP_SOURCE_IP_VID);
        if (ret)
        {
            cmsLog_error("failed to change iptv lookup method: %d\n", RDPA_IOCTL_IPTV_METHOD_GROUP_IP_SOURCE_IP_VID);
            return -1;
        }
    } 
    else
    {
        enableSnooping = 0;
        
        bcm_mcast_api_register(netlinkSnoopFd, 1); 

        McastSetUpStreamVlanTag();
        
        for (port = 0; port < UniCfgDbGetActivePortCount(); port++) 
        {
            McastVlanDefaultUpdate(port);        
        }    
    }
    
	McastModeSet(mode);
        
    McastEnableSnooping(enableSnooping);
    
    CtcMcastFastLeaveStateSet(CtcMcastFastLeaveDisable);

#else
        printf("%s: this is SFU feature. not supported in HGU\n", __FUNCTION__);
#endif
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Get the IP multicast control method
///
/// \Param None
///
/// \return
/// The current IP multicast control method
////////////////////////////////////////////////////////////////////////////////
//extern
U8 CtcMcastSwitchGetMode(void)
{   
#ifdef EPON_SFU
    return McastModeGet();
#else
    return 0;
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Return whether it is host control mode
///
/// \Param None
///
/// \return
/// TRUE if Yes, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
//extern
U8 CtcMcastIsHostCtrlMode(void)
{
#ifdef EPON_SFU
    return (McastModeGet() == McastHostMode)? 1 : 0;
#else
    return 0;
#endif
} 

////////////////////////////////////////////////////////////////////////////////
/// \brief Return whether it is snooping mode
///
/// \Param None
///
/// \return
/// TRUE if Yes, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
//extern
U8 CtcMcastIsSnoopingMode(void)
{
#ifdef EPON_SFU
    return (McastModeGet() == McastIgmpMode)? 1 : 0;
#else
    return 1;
#endif
} 

////////////////////////////////////////////////////////////////////////////////
/// \brief Initialize the CTC multicast module
///
/// \Param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void CtcMcastInit(void)
{   
    McastInit();

    McastIngressFiltersSet(rdpa_if_id(rdpa_if_wan0), RDPA_FILTER_IGMP, rdpa_forward_action_host);

    McastIngressFiltersSet(rdpa_if_id(rdpa_if_wan0), RDPA_FILTER_MLD, rdpa_forward_action_host);
    
    CtcMcastSwitchSetMode(McastIgmpMode);
    
    CtcMcastFastLeaveStateSet(CtcMcastFastLeaveDisable); 
} 

// End of File CtcMcast.c

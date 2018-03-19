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
/// \file Mcast.c
/// \brief multicast control module
///
////////////////////////////////////////////////////////////////////////////////
#include "Mcast.h"

#ifdef EPON_SFU
BCM_COMMON_DECLARE_LL(gMulticastRuleLL);
static U32 defRuleInstalled[MAX_UNI_PORTS] = {0};
static mcastConfig_t gMcastConfig;
extern int netlinkSnoopFd;
extern int (*eponsdk_igmp_process_input_func)(t_BCM_MCAST_PKT_INFO *pkt_info);
extern int (*eponsdk_mld_process_input_func)(t_BCM_MCAST_PKT_INFO *pkt_info);

#ifdef MCAST_DEBUG
static void McastDumpPortConfig(U8 port)
{
    int i;
    printf("    port %d config:\n", port);
    printf("        max groups: %d; \n", gMcastConfig.mcastPortCfg[port].maxGroups);
    printf("        num vids: %d\n",   gMcastConfig.mcastPortCfg[port].numVids);                      
    printf("            ");    
    for(i=0; i<gMcastConfig.mcastPortCfg[port].numVids; i++)
        printf("%d ", gMcastConfig.mcastPortCfg[port].vids[i]);
    printf("\n");
    printf("        tag oper mode: %s\n", (gMcastConfig.mcastPortCfg[port].tagStripped == 0)? "keep" : 
                        ((gMcastConfig.mcastPortCfg[port].tagStripped == 1)? "detag" : "translate"));
    printf("        nbr vlan switches: %d\n", gMcastConfig.mcastPortCfg[port].numVlanSwitches);
    for(i=0; i<gMcastConfig.mcastPortCfg[port].numVlanSwitches; i++)
    {
        printf("            mcast vlan: %d; user vlan: %d\n", 
                    gMcastConfig.mcastPortCfg[port].entry[i].mcastVlan,
                    gMcastConfig.mcastPortCfg[port].entry[i].userVlan);
    }    
}
static void McastDumpIptvEntries(U8 num)
{
    int i;
    for (i=0; i<num; i++)
    {
        printf("        user id: %d vlantag: %d index: %d\n", 
                                    gMcastConfig.fdbEntry[i].userId,
                                    gMcastConfig.fdbEntry[i].vlanTag,
                                    gMcastConfig.fdbEntry[i].index);
    }
}

static char* mcastCtltype[] = 
{
    "GDA MAC Only",
    "GDA MAC + VLAN",
    "GDA MAC + Source IPv4",
    "Group IPv4 + VLAN",    
    "Group IPv6 + VLAN",
    "GDA MAC + Source IPv6",
};

static char* mcastIptvLookupMethod[] = 
{
    "GDA MAC Only",
    "GDA MAC + VLAN",
    "Group IP",    
    "Group IP  + Srouce IP",
    "Group IP  + Srouce IP + VLAN",
};

/* Dump internal data of gMcastConfig
    port: 0xFF, for all ports
            0~3 (max port): only dump info of one port
*/
void McastDumpConfigInfo(U8 port)
{
    int i;
    
    printf("    dump mcast config info  \n");
    printf("    mcast mode: %s mode\n", 
                                gMcastConfig.mcastMode == OamCtcMcastHostMode? "host controllable" : "igmp snooping");
    if (port != 0xFF)
    {
        McastDumpPortConfig(port);
    }
    else
    {
        for (i = 0; i < UniCfgDbGetActivePortCount(); i++)
        {
            McastDumpPortConfig(i);
        }
    }
    
    printf("    multicast control type: %s\n", mcastCtltype[gMcastConfig.ctrlType]);
    printf("    nbr iptv entries: %d\n", gMcastConfig.numEntries);
    McastDumpIptvEntries(gMcastConfig.numEntries);
}
#endif
 
static int McastFindMcastVidInVlanSwitches(int port, VlanTag tag, U16 *foundIndex)
{
    int i   = 0;
    U16 vid = 0;

    for (i = 0; i < gMcastConfig.mcastPortCfg[port].numVlanSwitches; i++)
    {
        vid = GET_VID(gMcastConfig.mcastPortCfg[port].entry[i].mcastVlan);
        if (vid == GET_VID(tag))
        {
            *foundIndex = i;
            return 1;
        }
    }

    return 0;
}

static int McastFindFdbEntryFreeIndex(U16 *index)
{
    int i;

    for (i=0; i < MAX_MCAST_FDB_ENTRIES; i++)
    {
        if (gMcastConfig.fdbEntry[i].index == INVALID_INDEX)
        {
            *index = i;
            return 0;
        }
    }    

    return -1;
}

void McastVlanDefaultClear(U8 port)
{ 
    U32  numTags = 0;
    char realDevName[IF_NAME_SIZE] = {0};
    char vlanDevName[IF_NAME_SIZE] = {0};
    
    snprintf(realDevName, IF_NAME_SIZE, "%s%d", "eth", port);  
    snprintf(vlanDevName, IF_NAME_SIZE, "%s%d.0", "eth", port);
    
    if (defRuleInstalled[port] != 0)
    {
        vlanCtl_init();
        
        vlanCtl_initTagRule();
        vlanCtl_filterOnRxRealDevice(eponRealIfname);
        vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_IS_MULTICAST);
        vlanCtl_setReceiveVlanDevice(vlanDevName);

        for (numTags = 0; numTags < 3; numTags++) 
        {
            vlanCtl_removeTagRuleByFilter(realDevName, VLANCTL_DIRECTION_TX, numTags);
        }
    
        defRuleInstalled[port] = 0;
        vlanCtl_cleanup();
    } 
}

static void McastSetMcMaxGroups(void)
{
#ifdef BRCM_CMS_BUILD

#if (defined(DMP_X_BROADCOM_COM_IGMP_1) || defined(DMP_X_BROADCOM_COM_MLD_1))   
    CmsRet ret               = CMSRET_SUCCESS;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
#endif

#ifdef DMP_X_BROADCOM_COM_MLD_1
    MldCfgObject *mldObj     = NULL;
#endif

#ifdef DMP_X_BROADCOM_COM_IGMP_1
    IgmpCfgObject *igmpObj   = NULL;

    if ((ret = cmsLck_acquireLockWithTimeout(5000)) != CMSRET_SUCCESS )
    {
        cmsLog_error("could not acquire lock oam , ret=%d", ret);
        return;
    }

    if ((ret = cmsObj_get(MDMOID_IGMP_CFG, &iidStack, 0, (void **) &igmpObj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get igmpObj, ret=%d", ret);
        cmsLck_releaseLock();
        return;
    }

    igmpObj->igmpMaxGroups = MAX_MCASTS_PER_PORT * UniCfgDbGetActivePortCount();        
    cmsObj_set(igmpObj, &iidStack);    
    cmsObj_free((void **) &igmpObj);    
    cmsLck_releaseLock();
#endif

#ifdef DMP_X_BROADCOM_COM_MLD_1
    if ((ret = cmsLck_acquireLockWithTimeout(5000)) != CMSRET_SUCCESS )
    {
        cmsLog_error("could not acquire lock oam , ret=%d", ret);
        return;
    }

    if ((ret = cmsObj_get(MDMOID_MLD_CFG, &iidStack, 0, (void **) &mldObj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get mldObj, ret=%d", ret);
        cmsLck_releaseLock();
        return;
    }

    mldObj->mldMaxGroups = MAX_MCASTS_PER_PORT * UniCfgDbGetActivePortCount();        
    cmsObj_set(mldObj, &iidStack);    
    cmsObj_free((void **) &mldObj);    
    cmsLck_releaseLock();

#endif
#endif
}

void McastSaveVlanRule(vlan_rule_data *pRule, char *devName, 
    vlanCtl_direction_t direction, U32 numTags, U32 ruleIdx) 
{
    pRule->valid = 1;
    pRule->nbrOfTags = numTags;
    pRule->dir = direction;
    pRule->tagRuleId = ruleIdx;
    strcpy(pRule->devName, devName);
}

int McastDeleteVlanRule(vlan_rule_data * pRule) 
{
    int ret =0;
    
    if (pRule->valid)
    {
        pRule->valid = 0;        
        cmsLog_debug("%s: dev: %s, tags: %d\n", __FUNCTION__, pRule->devName, pRule->nbrOfTags);
        ret =vlanCtl_removeTagRule((char *)pRule->devName, pRule->dir, 
            pRule->nbrOfTags, pRule->tagRuleId);
    }

    return ret;
}

//just remove iptv entries belonging to this port temporarily
int McastDelFdbEntriesOnPort(U8 port, McastFdbOp fdbOp)
{
    int ret = 0;
    int i   = 0;

    for (i=0; i<gMcastConfig.numEntries; i++)
    {
        if ((gMcastConfig.fdbEntry[i].userId == port) 
                && (gMcastConfig.fdbEntry[i].index != INVALID_INDEX))
        {
            ret = McastAddOrRemoveFdbEntry(gMcastConfig.ctrlType, &(gMcastConfig.fdbEntry[i]),
                                           &gMcastConfig.fdbEntry[i].index, fdbOp);
            if (ret)
            {
                cmsLog_error("Failed to remove iptv entry %d\n", gMcastConfig.fdbEntry[i].index);
                return ret;
            }
            
            gMcastConfig.fdbEntry[i].index = INVALID_INDEX;
        }
    }
    return 0;
}

/*check whether the entry of this vid is in the mcastPortCfg array
    or not
*/
int McastFindVidOnPort(int port, U16 tag, U16 *foundIndex)
{
    int i   = 0;
    U16 vid = 0;

    for (i = 0; i < gMcastConfig.mcastPortCfg[port].numVids; i++)
    {
        vid = GET_VID(gMcastConfig.mcastPortCfg[port].vids[i]);
        if (vid == GET_VID(tag))
        {
            *foundIndex = i;
            return 1;
        }
    }

    return 0;
}

/*for upstream IGMP/MLD control packet, push vlan tag with     
    vid = port number + 1;
*/
int McastSetUpStreamVlanTag(void)
{
    int i        = 0;
    int ret      = 0;  
    U32 numTags  = 0;
    U32 ruleId   = 0;
    vlan_rule_data *pRule;
    char realDevName[IFNAMESIZ];  
    vlanCtl_direction_t direction = VLANCTL_DIRECTION_TX;

    vlanCtl_init();

    for (i = 0; i < UniCfgDbGetActivePortCount(); i++)
    {
        /* Get the Real Dev Name, eth0, eth1...*/
        snprintf(realDevName, IFNAMESIZ, "%s%d", "eth", i);

        vlanCtl_initTagRule();
        vlanCtl_filterOnRxRealDevice(realDevName);
        vlanCtl_setReceiveVlanDevice(eponVlanIfname);
        vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_IS_MULTICAST);
        vlanCtl_cmdPushVlanTag();
        vlanCtl_cmdSetTagVid(McastPortToUserVid(i), 0);   

        for (numTags = 0; numTags < MAX_VLANCTL_TAGS; numTags++)
        {
            vlanCtl_insertTagRule(eponRealIfname, direction,numTags, 
                                  VLANCTL_POSITION_BEFORE,
                                  VLANCTL_DONT_CARE, &ruleId);
            
            pRule = &gMcastConfig.usMcastTagRules[i][numTags];
            McastSaveVlanRule(pRule, eponRealIfname, direction, numTags, ruleId);
        }
    }
    
    vlanCtl_cleanup();
    
    return ret;
}

/*enable or disable igmp/mld snooping*/
#if defined(DMP_X_BROADCOM_COM_IGMP_1) || defined(DMP_X_BROADCOM_COM_MLD_1)
int McastEnableSnooping(int enable)
{
#ifdef BRCM_CMS_BUILD
    CmsRet ret                 = CMSRET_SUCCESS;
    InstanceIdStack iidStack   = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidStack2  = EMPTY_INSTANCE_ID_STACK;
    LanIpIntfObject *ipIntfObj =NULL;
    LanDevObject *lanDeviceObj =NULL;
#ifdef DMP_X_BROADCOM_COM_IGMP_1
    IgmpSnoopingCfgObject *igmpSnoopingObj = NULL;
#endif
#ifdef DMP_X_BROADCOM_COM_MLD_1
    _MldSnoopingCfgObject *mldSnoopingObj  = NULL;
#endif

    INIT_INSTANCE_ID_STACK(&iidStack);
    INIT_INSTANCE_ID_STACK(&iidStack2);

    if ((ret = cmsLck_acquireLockWithTimeout(15*MSECS_IN_SEC)) != CMSRET_SUCCESS )
    {
        cmsLog_error("could not acquire lock oam , ret=%d", ret);
        return -1;
    }

    if ((ret = cmsObj_getNext(MDMOID_LAN_IP_INTF, &iidStack, (void **) &ipIntfObj)) == CMSRET_SUCCESS)
    {
        iidStack2 = iidStack;
        ret = cmsObj_getAncestor(MDMOID_LAN_DEV, MDMOID_LAN_IP_INTF, &iidStack2, (void **) &lanDeviceObj);

        /* we only need the instance id, not the lanDeviceObj  */
        cmsObj_free((void **)&lanDeviceObj);

#ifdef DMP_X_BROADCOM_COM_IGMP_1
        if ((ret = cmsObj_get(MDMOID_IGMP_SNOOPING_CFG, &iidStack2, 0, (void **) &igmpSnoopingObj)) 
            != CMSRET_SUCCESS)
        {
            cmsLog_error("could not get igmpSnoopingCfgObject, ret=%d", ret);
            cmsLck_releaseLock();
            return -1;
        }

        if (enable)
        {
            igmpSnoopingObj->enable = 1;
        }
        else
        {
            igmpSnoopingObj->enable = 0;
        }

        cmsObj_set(igmpSnoopingObj, &iidStack2);

        cmsObj_free((void **) &igmpSnoopingObj);
#endif

#ifdef DMP_X_BROADCOM_COM_MLD_1
        if ((ret = cmsObj_get(MDMOID_MLD_SNOOPING_CFG, &iidStack2, 0, (void **) &mldSnoopingObj)) 
            != CMSRET_SUCCESS)
        {
            cmsLog_error("could not get mldSnoopingObj, ret=%d", ret);
            cmsLck_releaseLock();
            return -1;
        }

        if (enable)
        {
            mldSnoopingObj->enable = 1;
        }
        else
        {
            mldSnoopingObj->enable = 0;
        }

        cmsObj_set(mldSnoopingObj, &iidStack2);

        cmsObj_free((void **) &mldSnoopingObj);
#endif
        cmsObj_free((void **) &ipIntfObj);
    }

    cmsLck_releaseLock();
#endif
    return 0;
}
#endif


int McastParseFdbEntry(U8 ctltype, U8 * pEntry, IptvEntry* fdbEntry)
{
    int ret = 0;
    
    if (!pEntry || !fdbEntry)
    {
        return -1;
    }
    
    memset(fdbEntry, 0, sizeof(IptvEntry));  
    
    switch(ctltype)
    {
        case McastGrpDaMacOnly:
        case McastGrpDaMacVid:
        {
            mcastEntry_1* entry = (mcastEntry_1*)pEntry;
            memcpy(fdbEntry->da.mac, entry->da.mac, sizeof(MacAddr));
            fdbEntry->vlanTag =  OAM_NTOHS(entry->vlanTag);
            fdbEntry->userId = OAM_NTOHS(entry->userId);
            break;
        }        
        case McastGrpDaIpv4Vid:
        {
            mcastEntry_3* entry = (mcastEntry_3*)pEntry;
            memcpy(fdbEntry->groupIp.ipv4Addr, entry->groupIp, 6);
            fdbEntry->vlanTag =  OAM_NTOHS(entry->vlanTag);
            fdbEntry->userId = OAM_NTOHS(entry->userId);
            break;
        }
        case McastGrpDaIpv6Vid:
        {
            mcastEntry_4* entry = (mcastEntry_4*)pEntry;
            memcpy(fdbEntry->groupIp.ipv6Addr, entry->groupIp, 16);
            fdbEntry->vlanTag =  OAM_NTOHS(entry->vlanTag);
            fdbEntry->userId = OAM_NTOHS(entry->userId);
            break;
        }
        case McastGrpDaSaIpv4Vid:
        {
            mcastEntry_6* entry = (mcastEntry_6*)pEntry;
            memcpy(fdbEntry->groupIp.ipv4Addr, &entry->groupIp[10], 6);
            memcpy(fdbEntry->sourceIp.ipv4Addr, &entry->sourceIp[10], 6);

            fdbEntry->vlanTag =  OAM_NTOHS(entry->vlanTag);
            fdbEntry->userId = OAM_NTOHS(entry->userId);
            break;
        }
        case McastGrpDaSaIpv6Vid:
        {
            mcastEntry_6* entry = (mcastEntry_6*)pEntry;
            memcpy(fdbEntry->groupIp.ipv6Addr, entry->groupIp, 16);
            memcpy(fdbEntry->sourceIp.ipv6Addr, entry->sourceIp, 16);
            fdbEntry->vlanTag =  OAM_NTOHS(entry->vlanTag);
            fdbEntry->userId = OAM_NTOHS(entry->userId);
            break;
        }
        case McastGrpDaMacSaIpv6: 
        default:
        {
            ret = -1;
            break;
        }
    }

    return ret;
}

int McastCheckFdbEntry(U8 ctltype, U8 * pEntry)
{
    int ret = 0;
    
    if (!pEntry)
    {
        return -1;   
    }
    
    switch(ctltype)
    {
        case McastGrpDaMacOnly:
        case McastGrpDaMacVid:
        {
            mcastEntry_1* entry = (mcastEntry_1*)pEntry;
            if(entry->da.u8[0] == 0x01)
                ret = 0; // IPv4
            else if((entry->da.u8[0] == 0x33) && (entry->da.u8[1] == 0x33))
                ret = 0; // IPv6
            else
                ret = -1;
            break;
        }    
        case McastGrpDaIpv4Vid:
        {
            mcastEntry_3* entry = (mcastEntry_3*)pEntry;
            ret = ((entry->groupIp[2] & 0xF0) == 0xE0)?0:-1;
            break;
        }        
        case McastGrpDaIpv6Vid:
        {
            mcastEntry_4* entry = (mcastEntry_4*)pEntry;
            ret = (entry->groupIp[0] == 0xFF)?0:-1;
            break;
        }       
        case McastGrpDaIpv4:
        case McastGrpDaSaIpv4:
        case McastGrpDaSaIpv4Vid:
        {
            mcastEntry_6* entry = (mcastEntry_6*)pEntry;
            ret = ((entry->groupIp[12] & 0xF0) == 0xE0)?0:-1;
            break;
        }        
        case McastGrpDaIpv6:
        case McastGrpDaSaIpv6:
        case McastGrpDaSaIpv6Vid:
        {
            mcastEntry_6* entry = (mcastEntry_6*)pEntry;
            ret = (entry->groupIp[0] == 0xFF)?0:-1;
            break;
        }        
        case McastGrpDaMacSaIpv6: 
        default:
        {
            ret = -1;
            break;
        }
    }

    return ret;
}

U8 McastFdbEntrySizeGet(U8 ctrlType)
{
    U8 size = 0;

    switch(ctrlType)
    {
        case McastGrpDaMacOnly:
        case McastGrpDaMacVid:
        {
             size =  sizeof(mcastEntry_1);   
             break;
        }     
        case McastGrpDaMacSaIpv4:
        {
            size =  sizeof(mcastEntry_2);   
            break;
        }     
        case McastGrpDaIpv4Vid:
        {
            size =  sizeof(mcastEntry_3);   
            break;
        }  
        case McastGrpDaIpv6Vid:
        {
            size =  sizeof(mcastEntry_4);   
            break;
        }    
        case McastGrpDaMacSaIpv6:
        {
            size =  sizeof(mcastEntry_5);   
            break;
        }   
        case McastGrpDaSaIpv4Vid:
        case McastGrpDaSaIpv6Vid:
        {
            size =  sizeof(mcastEntry_6);   
            break;
        }  
        default:
        {
            size = 0xFF;
            break;
        }
    }
    
    return size;
}

int McastConfigClear(void)
{
    int i       = 0;
    int j       = 0;
    int numTags = 0;
    vlan_rule_data *pRule;

    vlanCtl_init();

    McastSetMcMaxGroups();

    for (i = 0; i < UniCfgDbGetActivePortCount(); i++) 
    {
        gMcastConfig.mcastPortCfg[i].maxGroups =  MAX_GROUPS_DEF_VALUE;

        if (gMcastConfig.mcastPortCfg[i].tagStripped == McastVlanActTrans)
        {
            gMcastConfig.mcastPortCfg[i].numVlanSwitches = 0;
        }
        
        gMcastConfig.mcastPortCfg[i].tagStripped = McastVlanActNone;

        for (j = 0; j  < gMcastConfig.mcastPortCfg[i].numVids; j++) 
        {
            for (numTags = 1; numTags < MAX_VLANCTL_TAGS; numTags++)
            {   
                pRule = &gMcastConfig.mcastPortCfg[i].vidRules[j][numTags];
                McastDeleteVlanRule(pRule);
            }
        }
        gMcastConfig.mcastPortCfg[i].numVids = 0;
    }
    
    for (i = 0; i < UniCfgDbGetActivePortCount(); i++)
    {
        for (j = 0; j < MAX_VLANCTL_TAGS; j++)
        {
            pRule = &gMcastConfig.usMcastTagRules[i][j];
            McastDeleteVlanRule(pRule);
        }
    }

    vlanCtl_cleanup();

    for (i = 0; i < UniCfgDbGetActivePortCount(); i++)
    {
        McastDelFdbEntriesOnPort(i, McastFdbOpDel);
        McastVlanDefaultClear(i);
    }
    
    gMcastConfig.numEntries = 0;
    gMcastConfig.ctrlType   = McastGrpTypeInvalid;

    for (i=0; i< MAX_MCAST_FDB_ENTRIES; i++)
    {
        gMcastConfig.fdbEntry[i].index = INVALID_INDEX;
    }
    
    return 0;
}

BOOL McastSetMaxGroupsPortList(void)
{
#ifdef BRCM_CMS_BUILD
    CmsRet ret = CMSRET_SUCCESS;

#if defined(DMP_X_BROADCOM_COM_MCAST_1) && (defined(DMP_X_BROADCOM_COM_IGMP_1) || defined(DMP_X_BROADCOM_COM_MLD_1))
    {
    McastCfgObject *mcastObj = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    U16 port                 = 0;
    U16 totalLen             = 0;
    char buf[16]             = {0};
    char tmpStr[MAX_MC_GROUP_PORT_LIST_STR_LEN] = {0};

    if( (ret = cmsLck_acquireLockWithTimeout(5000)) != CMSRET_SUCCESS )
    {
        cmsLog_error("could not acquire lock oam , ret=%d", ret);
        return FALSE;
    }
    
    ret = cmsObj_get(MDMOID_MCAST_CFG, &iidStack, 0, (void **) &mcastObj);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("could not get mcastCfgObject, ret=%d", ret);
        cmsLck_releaseLock();        
        return FALSE;
    }

    CMSMEM_FREE_BUF_AND_NULL_PTR(mcastObj->mcastMaxGroupsPortList);
    
    for (port = 0; port < UniCfgDbGetActivePortCount(); port++)
    {
        memset(buf, 0, sizeof(buf));
        if (gMcastConfig.mcastPortCfg[port].maxGroups != 0)
        {
            sprintf(buf,"eth%d.0 %d,", port, 
                        gMcastConfig.mcastPortCfg[port].maxGroups);
            totalLen = strlen(buf) + strlen(tmpStr);
            
            if (totalLen < MAX_MC_GROUP_PORT_LIST_STR_LEN)
            {
                strcat(tmpStr, buf);
            }
        }
    }

    mcastObj->mcastMaxGroupsPortList = cmsMem_alloc(MAX_MC_GROUP_PORT_LIST_STR_LEN, ALLOC_ZEROIZE);
    if(strlen(tmpStr) != 0)
    {
        /* Copy all string except last ',' */
        strncpy(mcastObj->mcastMaxGroupsPortList, tmpStr, strlen(tmpStr) - 1);
    }
    else
    {
        sprintf(mcastObj->mcastMaxGroupsPortList, " ");
    }

    ret = cmsObj_set(mcastObj, &iidStack);
    cmsObj_free((void **) &mcastObj); 
    cmsLck_releaseLock();
    }
#endif

    return (ret == CMSRET_SUCCESS);
#else
    return TRUE;
#endif
}

int McastConvertControlType(int ctlType)
{
    int lookUpMethod;
    
    switch(ctlType)
    {
        case McastGrpDaMacOnly:
        {
            lookUpMethod = RDPA_IOCTL_IPTV_METHOD_MAC;
            break;
        }  
        case McastGrpDaMacVid:
        {
            lookUpMethod = RDPA_IOCTL_IPTV_METHOD_MAC_VID;
            break;
        }
        case McastGrpDaIpv4:
        case McastGrpDaIpv6:
        {
            lookUpMethod = RDPA_IOCTL_IPTV_METHOD_GROUP_IP;
            break;
        }
        case McastGrpDaSaIpv4:
        case McastGrpDaSaIpv6:
        {
            lookUpMethod = RDPA_IOCTL_IPTV_METHOD_GROUP_IP_SOURCE_IP;
            break;
        }   
        case McastGrpDaIpv4Vid:
        case McastGrpDaIpv6Vid:
        case McastGrpDaSaIpv4Vid:
        case McastGrpDaSaIpv6Vid:
        {
            lookUpMethod = RDPA_IOCTL_IPTV_METHOD_GROUP_IP_SOURCE_IP_VID;
            break;
        }
        default:
        {
            lookUpMethod = -1;
            break;
        }
    }
    
    return lookUpMethod;
}

static int McastOptFdbEntryList(int type, struct in6_addr *grpIp,
    t_BCM_MCAST_PKT_INFO *pktInfo, U16 *index, void *repIp, McastFdbOp fdbOp)
{
    BCM_MULTICAST_RULE_ENTRY *newEntry  = BCM_COMMON_LL_GET_HEAD(gMulticastRuleLL);
    BCM_MULTICAST_RULE_ENTRY *tempEntry = NULL; 
    int match = 0;

    while (newEntry)
    {
        if (pktInfo->rxdev_ifi == newEntry->pktInfo.rxdev_ifi)
        {
            if (type == BCM_MCAST_MSG_IGMP_PKT) 
            {
                if (grpIp->s6_addr32[3] == newEntry->grpIp.v6.s6_addr32[3])
                    match = 1;
                else if ((grpIp->s6_addr32[3] & OAM_HTONL(0x7fffff)) == (newEntry->grpIp.v6.s6_addr32[3] & OAM_HTONL(0x7fffff)))
                {
                    /* if OLT reported this entery is L2, store this info by clearing the MSB byte */
                    //newEntry->grpIp.v6.s6_addr32[3] = newEntry->grpIp.v6.s6_addr32[3] & OAM_HTONL(0x7fffff);
                    grpIp->s6_addr32[3] = grpIp->s6_addr32[3] & OAM_HTONL(0x7fffff);
                    match = 1;
                }
            }
            else if (type == BCM_MCAST_MSG_MLD_PKT) 
            {
                if (!memcmp(grpIp, &newEntry->grpIp.v6, sizeof(struct in6_addr)))
                    match = 1;
                else if (grpIp->s6_addr32[3] == newEntry->grpIp.v6.s6_addr32[3])
                {
                    /* if OLT reported this entery is L2, store this info by clearing the 12 MSB bytes */
                    /*
                    newEntry->grpIp.v6.s6_addr32[0] = 0;
                    newEntry->grpIp.v6.s6_addr32[1] = 0;
                    newEntry->grpIp.v6.s6_addr32[2] = 0;
                    */ 
                    grpIp->s6_addr32[0] = grpIp->s6_addr32[1] = grpIp->s6_addr32[2] = 0;
                    match = 1;
                }
            }

            if (match)
            {
                if (*index == INVALID_INDEX)
                {   
                    *index = newEntry->index;
                }
                else
                {
                    newEntry->index = *index;
                    newEntry->pktInfo.tci = pktInfo->tci;
                }

                memcpy(pktInfo, &newEntry->pktInfo, sizeof(t_BCM_MCAST_PKT_INFO));

                if (type == BCM_MCAST_MSG_IGMP_PKT)
                {
                    memcpy(repIp, &newEntry->srcIp, sizeof(struct in_addr));
                }
                else
                {
                    memcpy(repIp, &newEntry->srcIp, sizeof(struct in6_addr));  
                }
                
                if (fdbOp ==  McastFdbOpDel)
                {
                    tempEntry = newEntry;

                    newEntry = BCM_COMMON_LL_GET_NEXT(newEntry);
                    BCM_COMMON_LL_REMOVE(&gMulticastRuleLL, tempEntry);
                    CMSMEM_FREE_BUF_AND_NULL_PTR(tempEntry);
                }
                else if (fdbOp ==  McastFdbOpDelTmp)
                {
                    newEntry->pktInfo.tci = 0;
                }

                return 0;
            }
        }
        newEntry = BCM_COMMON_LL_GET_NEXT(newEntry);
    }
    
    return -1;
}

static int McastFindFdbEntryInList(int type, struct in6_addr *grpIp, int rxDevIndex, int shouldRemove)
{
    BCM_MULTICAST_RULE_ENTRY *newEntry = BCM_COMMON_LL_GET_HEAD(gMulticastRuleLL);
    BCM_MULTICAST_RULE_ENTRY *tempEntry = NULL;
    int match = 0;

    while (newEntry)
    {
        if (rxDevIndex == newEntry->pktInfo.rxdev_ifi)
        {
            if (type == BCM_MCAST_MSG_IGMP_PKT) 
            {
                if (grpIp->s6_addr32[3] == newEntry->grpIp.v6.s6_addr32[3])
                {
                    match = 1;
                }
                else if ((grpIp->s6_addr32[3] & OAM_HTONL(0x7fffff)) == (newEntry->grpIp.v6.s6_addr32[3] & OAM_HTONL(0x7fffff)))
                {
                    grpIp->s6_addr32[3] = grpIp->s6_addr32[3] & OAM_HTONL(0x7fffff);
                    match = 1;
                }
            }
            else if (type == BCM_MCAST_MSG_MLD_PKT) 
            {
                if (!memcmp(grpIp, &newEntry->grpIp.v6, sizeof(struct in6_addr)))
                {
                    match = 1;
                }
                else if (grpIp->s6_addr32[3] == newEntry->grpIp.v6.s6_addr32[3])
                {
                    grpIp->s6_addr32[0] = grpIp->s6_addr32[1] = grpIp->s6_addr32[2] = 0;
                    match = 1;
                }
            }

            if (match)
            {
                if (shouldRemove)
                {
                    tempEntry = newEntry;
                    
                    newEntry = BCM_COMMON_LL_GET_NEXT(newEntry);
                    BCM_COMMON_LL_REMOVE(&gMulticastRuleLL, tempEntry);
                    CMSMEM_FREE_BUF_AND_NULL_PTR(tempEntry);
                }
                
                return 1;
            }
        }
        newEntry = BCM_COMMON_LL_GET_NEXT(newEntry);
    }
    
    return 0;
}

int McastHandleVlanRule(U16 port, U16 vlanTag, int shouldRemove)
{
    int  ret     = 0;
    U8   vlanOp  = 0;
    U16  vlanId  = 0;
    U32  ifIndex = 0;
    char ifName[IF_NAME_SIZE];
    BCM_MULTICAST_RULE_ENTRY *newEntry = BCM_COMMON_LL_GET_HEAD(gMulticastRuleLL);
    
    if (shouldRemove)
    {   
        memset(ifName, 0, IFNAMESIZ);
        snprintf(ifName, IFNAMESIZ, "eth%d.0", port);
        ifIndex = if_nametoindex(ifName);
        
        while (newEntry)
        {
            if (ifIndex == newEntry->pktInfo.rxdev_ifi)
            {
                if (vlanTag == newEntry->pktInfo.tci)
                {
                    return ret;
                }
            }
            newEntry = BCM_COMMON_LL_GET_NEXT(newEntry);        
        }
   
        vlanOp = McastVlanOpDel;
    }
    else
    {
        vlanOp = McastVlanOpAdd;   
    }

    vlanId = OAM_NTOHS(vlanTag);
    ret = McastVlanTagSet((U8)port, 1, &vlanId, vlanOp);
    if (ret != 0)
    {
        cmsLog_error("Could not configure a new vlanctl rule\n");
        return ret;
    }
    
    McastVlanDefaultUpdate((U8)port);
        
    return ret;
}

static int McastDeleteIptvByIndex(U16 port, U16 index)
{
    U32 channelIndex = 0;
    int ret          = 0;
    
    channelIndex = gMcastConfig.fdbEntry[index].channelIndex;
    
    if (index == INVALID_INDEX)
    {
        cmsLog_error("Del IPTV Channel Failed index = INVALID_INDEX\n");
        return CMSRET_INTERNAL_ERROR;
    }

    if (channelIndex == 0)
    {
        cmsLog_error("Del IPTV Channel Failed channelIndex= %d\n",channelIndex);
        return CMSRET_INTERNAL_ERROR;
    } 
    else
    {
        ret = rdpaCtl_IptvEntryRemove(channelIndex, port);
        if (ret)
        {
            cmsLog_error("Del IPTV Channel Failed channelIndex= %U\n",channelIndex);
            return CMSRET_INTERNAL_ERROR;
        }

        gMcastConfig.fdbEntry[index].channelIndex = 0;

        if (index < (gMcastConfig.numEntries - 1))
        {
            memcpy(&gMcastConfig.fdbEntry[index], 
                &gMcastConfig.fdbEntry[gMcastConfig.numEntries - 1], 
                sizeof(IptvEntry));
        }        
        gMcastConfig.numEntries--;
    }

    return CMSRET_SUCCESS;
}


static int McastUpdateIptvByLeave(int type, int rxDevIndex, struct in6_addr *grpIp)
{
    int  lookUpMethod = 0;
    int  port         = 0;
    int  ret          = 0;
    int  shouldRemove = 1;
    U16  i            = 0;
    char ifName[IF_NAME_SIZE];
    
    memset(ifName, 0, IF_NAME_SIZE);
    if ( NULL == if_indextoname(rxDevIndex, ifName))
    {
        cmsLog_error("Could not get ifname for vlanctl rule\n");
        return CMSRET_INTERNAL_ERROR;
    }
    sscanf(ifName, "eth%d.", &port);

    lookUpMethod = McastConvertControlType(gMcastConfig.ctrlType);
    
    switch (lookUpMethod)
    {
        case RDPA_IOCTL_IPTV_METHOD_MAC:        
        case RDPA_IOCTL_IPTV_METHOD_MAC_VID:
        { 
            for (i = 0; i < gMcastConfig.numEntries; i++) 
            {
                if ((gMcastConfig.fdbEntry[i].userId  == port)
                    && ((gMcastConfig.fdbEntry[i].da.lowHi.low & OAM_HTONL(0x7fffff)) == (grpIp->s6_addr32[3]& OAM_HTONL(0x7fffff))))
                {
                    ret = McastDeleteIptvByIndex(port, i);
                    if (ret == CMSRET_SUCCESS)
                    {
                        McastFindFdbEntryInList(type, grpIp, rxDevIndex, shouldRemove);

                        if (gMcastConfig.fdbEntry[i].vlanTag != 0)
                        {
                            McastHandleVlanRule(port, gMcastConfig.fdbEntry[i].vlanTag, shouldRemove);
                        }
                    }
                }
            }
            
            break;
        }       
        case RDPA_IOCTL_IPTV_METHOD_GROUP_IP_SOURCE_IP:
        case RDPA_IOCTL_IPTV_METHOD_GROUP_IP_SOURCE_IP_VID:
        case RDPA_IOCTL_IPTV_METHOD_GROUP_IP:
        {
            if (type == BCM_MCAST_MSG_IGMP_PKT)
            {
                for (i = 0; i < gMcastConfig.numEntries; i++)
                {     
                    if ((gMcastConfig.fdbEntry[i].userId == port) 
                        && (memcmp(&gMcastConfig.fdbEntry[i].groupIp.ipv4Addr[2], &grpIp->s6_addr32[3], sizeof(grpIp->s6_addr32[3])) == 0) )
                    {
                        ret = McastDeleteIptvByIndex(port, i);
                        if (ret == CMSRET_SUCCESS)
                        {
                            McastFindFdbEntryInList(type, grpIp, rxDevIndex, shouldRemove);
                        
                            if (gMcastConfig.fdbEntry[i].vlanTag != 0)
                            {
                                McastHandleVlanRule(port, gMcastConfig.fdbEntry[i].vlanTag, shouldRemove);
                            }
                        }
                    }
                }
            }
            else
            {
                for (i = 0; i < gMcastConfig.numEntries; i++)
                {
                    if ((gMcastConfig.fdbEntry[i].userId == port) 
                        && (memcmp(gMcastConfig.fdbEntry[i].groupIp.ipv6Addr, grpIp, sizeof(struct in6_addr)) == 0) )
                    {
                        ret = McastDeleteIptvByIndex(port, i);
                        if (ret == CMSRET_SUCCESS)
                        {
                            McastFindFdbEntryInList(type, grpIp, rxDevIndex, shouldRemove);
                        
                            if (gMcastConfig.fdbEntry[i].vlanTag != 0)
                            {
                                McastHandleVlanRule(port, gMcastConfig.fdbEntry[i].vlanTag, shouldRemove);
                            }
                        }
                    }
                }
            }            
            break;
        }      
        default:
        {
            break; 
        }
    }
    
    return CMSRET_SUCCESS;
}

static int McastUpdateIptvByHost(int rxDevIndex, struct in6_addr *grpIp, struct in6_addr *srcIp, U16 vlanId, 
                             U8 ipFamily, McastFdbOp fdbOp, McastVlanAction *vlanAction, U16 fdbIndex)
{
    int ret          = 0;
    int lookUpMethod = 0;
    U16 headByte     = 0; 
    U16 vlanTransId  = 0;
    U32 port         = 0;
    U32 addrIpv4     = 0;
    U32 daAddr       = 0;
    U32 channelIndex = 0; 
    char ifName[IF_NAME_SIZE];

    rdpa_drv_ioctl_iptv_key_t         requestKey;
    rdpa_drv_ioctl_iptv_vlan_action_t vlanTransOp = RDPA_IOCTL_IPTV_VLAN_TRANPARENT;

    memset(ifName, 0, IF_NAME_SIZE);
    if ( NULL == if_indextoname(rxDevIndex, ifName))
    {
        cmsLog_error("Could not get ifname for vlanctl rule\n");
        return CMSRET_INTERNAL_ERROR;
    }
    sscanf(ifName, "eth%d.", &port);

    lookUpMethod = McastConvertControlType(gMcastConfig.ctrlType);

    if (fdbOp == McastFdbOpAdd)
    {
        if (vlanAction != NULL)
        {   
            switch (vlanAction->action)
            {
                case McastVlanActDel:
                {
                    vlanTransOp = RDPA_IOCTL_IPTV_VLAN_UNTAG;                  
                    break;
                }               
                case McastVlanActTrans:
                {

                    vlanTransId  = vlanAction->vlanId;
                    vlanTransOp  = RDPA_IOCTL_IPTV_VLAN_TRANSLATION;
                    break;
                }                
                case McastVlanActNone:
                default:
                {
                    vlanTransOp = RDPA_IOCTL_IPTV_VLAN_TRANPARENT;
                    break;
                }
            }
        }
        
        memset(&requestKey, 0, sizeof(rdpa_drv_ioctl_iptv_key_t));
    
        if (lookUpMethod == iptv_lookup_method_group_ip_src_ip_vid ||
            lookUpMethod == iptv_lookup_method_mac_vid)
        {
            requestKey.vid = vlanId;
        }

        /* L2 multicast */
        if (lookUpMethod == iptv_lookup_method_mac_vid || 
            lookUpMethod == iptv_lookup_method_mac)    
        {
            if (ipFamily == IpFamilyIpV4)
            {
                memcpy(&addrIpv4,&grpIp->s6_addr32[3],sizeof(struct in_addr));
                daAddr = OAM_NTOHL(addrIpv4);
                headByte = daAddr & 0xff000000;

                if (headByte == 0)
                {
                    requestKey.group.mac[0] = 0x1;         
                    requestKey.group.mac[1] = 0x0;         
                    requestKey.group.mac[2] = 0x5e;         
                    requestKey.group.mac[3] = (daAddr&0x7f0000)>>16; 
                    requestKey.group.mac[4] = (daAddr&0xff00)>>8; 
                    requestKey.group.mac[5] = daAddr&0xff; 
                }
                else
                {
                    cmsLog_error("L2 multicast supported for Host control mode only");
                    return CMSRET_INTERNAL_ERROR;
                }
            }
            else
            {
                headByte = grpIp->s6_addr[0] & 0xff;

                if (headByte == 0)
                {
                    requestKey.group.mac[0] = 0x33;         
                    requestKey.group.mac[1] = 0x33;         
                    requestKey.group.mac[2] = grpIp->s6_addr[12];
                    requestKey.group.mac[3] = grpIp->s6_addr[13];
                    requestKey.group.mac[4] = grpIp->s6_addr[14];
                    requestKey.group.mac[5] = grpIp->s6_addr[15];
                }
                else
                {
                    cmsLog_error("L2 multicast supported for Host control mode only");
                    return CMSRET_INTERNAL_ERROR;
                }
            }
        }
        else
        {
            /* Retrieve group and source IP addresses. */
            if (ipFamily == IpFamilyIpV4)
            {
                /* IGMP */
                memcpy(&addrIpv4,&grpIp->s6_addr32[3],sizeof(struct in_addr));
                requestKey.ip_family   = RDPACTL_IP_FAMILY_IPV4;
                requestKey.group.ipv4  = OAM_NTOHL(addrIpv4);
                memcpy(&addrIpv4,&srcIp->s6_addr32[3],sizeof(struct in_addr));
                requestKey.src_ip.ipv4 = OAM_NTOHL(addrIpv4);
            }
            else
            {
                /* MLD. */
                requestKey.ip_family = RDPACTL_IP_FAMILY_IPV6;
                memcpy(requestKey.group.ipv6,  grpIp, sizeof(struct in6_addr));
                memcpy(requestKey.src_ip.ipv6, srcIp, sizeof(struct in6_addr));
            }
        }

        ret = rdpaCtl_IptvEntryAdd(port, &requestKey, vlanTransOp, vlanTransId, &channelIndex);
        if (ret)
        {
            cmsLog_error("Add IPTV Channel Failed\n");
            return CMSRET_INTERNAL_ERROR;
        }
        
        if (channelIndex)
        {
            gMcastConfig.fdbEntry[fdbIndex].channelIndex = channelIndex;
        }
    }
    else
    {
        channelIndex = gMcastConfig.fdbEntry[fdbIndex].channelIndex;
       
        if (channelIndex)
        {
            ret = rdpaCtl_IptvEntryRemove(channelIndex, port);
            if (ret)
            {
                cmsLog_error("Del IPTV Channel Failed channelIndex= %U\n",channelIndex);
                return CMSRET_INTERNAL_ERROR;
            }
        }
        
        gMcastConfig.fdbEntry[fdbIndex].channelIndex = 0;
    }

    return CMSRET_SUCCESS;
}

static int McastGetWansInBridge(int ifIndex, t_BCM_MCAST_WAN_INFO_ARRAY *wanInfo)
{
    U32 count     = BRIDGE_MAX_IFS;
    int socketFd  = 0;
    int rc        = 0;
    int i         = 0;
    int ifindices[BRIDGE_MAX_IFS];
    struct ifreq ifr;

    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd < 0)
    {
        return socketFd;
    }

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_ifindex = ifIndex;

    rc = ioctl(socketFd, SIOCGIFNAME, &ifr);
    close(socketFd);
    if ( rc < 0 )
    {
        return rc;
    }

    rc = br_util_get_bridge_wan_interfaces(ifr.ifr_name, ifindices, &count);
    if ( rc < 0 )
    {
      return rc;
    }

    for (i = 0; i < count; i++)
    {
       wanInfo[i]->ifi = ifindices[i];
       wanInfo[i]->if_ops = BCM_MCAST_IF_BRIDGED;
    }

    return 0;
} 

static int McastUpdateIptvBySnooping(int type, struct in6_addr *grpIp, struct in6_addr *srcIp, struct in6_addr *repIp, t_BCM_MCAST_PKT_INFO *pktInfo, int filterMode)
{
    int rc  = 0;
    int ret = CMSRET_SUCCESS;
    t_BCM_MCAST_WAN_INFO_ARRAY wanInfo;
    
    memset(&wanInfo, 0, sizeof(t_BCM_MCAST_WAN_INFO_ARRAY));
    
    McastGetWansInBridge(pktInfo->parent_ifi, &wanInfo);
    
    switch (type)
    {
#ifdef SUPPORT_IGMP
        case BCM_MCAST_MSG_IGMP_PKT:
        {
            rc = bcm_mcast_api_update_igmp_snoop(netlinkSnoopFd, pktInfo->parent_ifi, pktInfo->rxdev_ifi, pktInfo->tci,
            pktInfo->lanppp, (struct in_addr *)&grpIp->s6_addr32[3], (struct in_addr *)&grpIp->s6_addr32[3],
            (struct in_addr *)&srcIp->s6_addr32[3], (struct in_addr *)repIp, pktInfo->repMac, 0, filterMode, &wanInfo, -1, 0);
            if ( rc < 0 )
            {
               ret = CMSRET_INTERNAL_ERROR;
            }
            break;
        }
#endif
#ifdef SUPPORT_MLD
        case BCM_MCAST_MSG_MLD_PKT:
        {
            rc = bcm_mcast_api_update_mld_snoop(netlinkSnoopFd, pktInfo->parent_ifi, pktInfo->rxdev_ifi, pktInfo->tci,
                pktInfo->lanppp, grpIp, srcIp, repIp, pktInfo->repMac, 0, filterMode, &wanInfo);
            if ( rc < 0 )
            {
                ret = CMSRET_INTERNAL_ERROR;
            }
            break;
        }
#endif
        default:
        {
            ret = CMSRET_INTERNAL_ERROR;
            break;
        }
    }

    return ret;
}


int McastVlanTagSet(U8 port, U8 vidNum, const U16 *pVid, U8 vlanOp)      
{
#ifdef EPON_SFU
    int i          = 0;
    U16 foundIndex = 0;
    U16 index      = 0;
    U16 tag        = 0;
    U32 curIndex   = 0;
    U32 numTags    = 0;
    U32 ruleId     = 0;

    char realDevName[IFNAMESIZ] = {0};
    char vlanDevName[IFNAMESIZ] = {0};                    
    vlanCtl_direction_t direction = VLANCTL_DIRECTION_TX;
    vlan_rule_data *pRule;

    cmsLog_notice("port: %d, vidNum: %d; op:%d; strip mode: %d\n",
                                port, vidNum, vlanOp, 
                                gMcastConfig.mcastPortCfg[port].tagStripped);

    snprintf(realDevName, IFNAMESIZ, "%s%d", "eth", port);  
    snprintf(vlanDevName, IFNAMESIZ, "%s%d.0", "eth", port);
   
    vlanCtl_init();

    cmsLog_notice("num vids: %d\n", vidNum);
    switch (vlanOp)
    {
        case McastVlanOpAdd: 
        {
            if ((vidNum + gMcastConfig.mcastPortCfg[port].numVids) > MAX_MCASTS_PER_PORT)  //only have 32 slots
            {
                vlanCtl_cleanup();
                return -1;
            }
            for (i = 0; i < vidNum; i++) 
            {
                /*for vid that is added for this port, setup vlanctl rule. 
                and multicast tag operation mode need to be considered
                as well
            */
                if (McastFindVidOnPort(port, OAM_NTOHS(pVid[i]), &foundIndex) == 0) 
                {
                    curIndex = gMcastConfig.mcastPortCfg[port].numVids;
                    vlanCtl_initTagRule();
                    vlanCtl_filterOnTagVid(GET_VID(OAM_NTOHS(pVid[i])), 0);
                    vlanCtl_filterOnRxRealDevice(eponRealIfname);
                    vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_IS_MULTICAST);
                    vlanCtl_setReceiveVlanDevice(vlanDevName);
    
                    if (gMcastConfig.mcastPortCfg[port].tagStripped == McastVlanActTrans) 
                    {
                        if (McastFindMcastVidInVlanSwitches(port, OAM_NTOHS(pVid[i]), &index)) 
                        {
                            tag = GET_VID(gMcastConfig.mcastPortCfg[port].entry[index].userVlan);
                            vlanCtl_cmdSetTagVid(tag, 0);
                        }
                    } 
                    else if (gMcastConfig.mcastPortCfg[port].tagStripped == McastVlanActDel)
                    {
                        vlanCtl_cmdPopVlanTag();
                    }
    
                    for (numTags = 1; numTags < 3; numTags++) 
                    {   
                        cmsLog_notice("insert vlan rule: %s, dir=%s\n", 
                                                                realDevName, 
                                                                (direction==VLANCTL_DIRECTION_TX)? "tx" : "rx");
                        vlanCtl_insertTagRule(realDevName, direction,
                                                                numTags, 
                                                                VLANCTL_POSITION_BEFORE, 
                                                                VLANCTL_DONT_CARE, 
                                                                &ruleId);

                        pRule = &gMcastConfig.mcastPortCfg[port].vidRules[curIndex][numTags];
                        McastSaveVlanRule(pRule, realDevName, direction, numTags, ruleId);
                    }
                    gMcastConfig.mcastPortCfg[port].vids[curIndex] = OAM_NTOHS(pVid[i]);
                    gMcastConfig.mcastPortCfg[port].numVids++;
                }
            }
            break;
        }
        case McastVlanOpDel:
        {
            for (i = 0; i < vidNum; i++) 
            {
                if (McastFindVidOnPort(port, OAM_NTOHS(pVid[i]), &foundIndex)) 
                {
                    for (numTags = 1; numTags < MAX_VLANCTL_TAGS; numTags++)
                    {   
                        pRule = &gMcastConfig.mcastPortCfg[port].vidRules[foundIndex][numTags];
                        McastDeleteVlanRule(pRule);
                    }
    
                    curIndex = gMcastConfig.mcastPortCfg[port].numVids - 1;
                    gMcastConfig.mcastPortCfg[port].vids[foundIndex] = gMcastConfig.mcastPortCfg[port].vids[curIndex];
                    memcpy(&gMcastConfig.mcastPortCfg[port].vidRules[foundIndex],
                           &gMcastConfig.mcastPortCfg[port].vidRules[curIndex],
                           sizeof(vlan_rule_data)*MAX_VLANCTL_TAGS);
                    gMcastConfig.mcastPortCfg[port].numVids--;
                }
            }
            break;
        }
        case McastVlanOpClr:                
        {
            if (gMcastConfig.mcastPortCfg[port].numVids > 0) 
            {
                for (i = 0; i < gMcastConfig.mcastPortCfg[port].numVids; i++) 
                {
                    for (numTags = 1; numTags < MAX_VLANCTL_TAGS; numTags++)
                    {   
                        pRule = &gMcastConfig.mcastPortCfg[port].vidRules[i][numTags];
                        McastDeleteVlanRule(pRule);
                    }
                }
            }
            gMcastConfig.mcastPortCfg[port].numVids = 0;
            break;
        }
        default:
        {
            break;
        }
    }   
    vlanCtl_cleanup();
    
#else
    printf("%s: this is SFU feature. not supported in HGU\n", __FUNCTION__);
#endif    
    return 0;
}

void McastVlanDefaultUpdate(U8 port)
{ 
    U32  numTags = 0;
    U32  ruleId  = 0;
    char realDevName[IF_NAME_SIZE] = {0};
    char vlanDevName[IF_NAME_SIZE] = {0};
    snprintf(realDevName, IF_NAME_SIZE, "%s%d", "eth", port);  
    snprintf(vlanDevName, IF_NAME_SIZE, "%s%d.0", "eth", port);

    vlanCtl_init();
    
    vlanCtl_initTagRule();
    vlanCtl_filterOnRxRealDevice(eponRealIfname);
    vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_IS_MULTICAST);
    vlanCtl_setReceiveVlanDevice(vlanDevName);
    
    if ((gMcastConfig.mcastPortCfg[port].numVids == 0) && (defRuleInstalled[port] == 0))
    {
        // Need to install the default rules
        for (numTags = 0; numTags < 3; numTags++) 
        {
            vlanCtl_insertTagRule(realDevName, VLANCTL_DIRECTION_TX,
                                  numTags, VLANCTL_POSITION_BEFORE, 
                                  VLANCTL_DONT_CARE, &ruleId);
        }
        defRuleInstalled[port] = 1;
    }
    else if ((gMcastConfig.mcastPortCfg[port].numVids != 0) 
              && (defRuleInstalled[port] == 1))
    {
        for (numTags = 0; numTags < 3; numTags++) 
        {
            vlanCtl_removeTagRuleByFilter(realDevName, VLANCTL_DIRECTION_TX, numTags);
        }
        defRuleInstalled[port] = 0;
    }
 
    vlanCtl_cleanup();
}

int McastFastLeaveEnabled(void)
{
#ifdef BRCM_CMS_BUILD
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    IgmpCfgObject *igmpObj   = NULL;
    int fastLeaveEnable      = 0; 
    int ret                  = 0;

    if( (ret = cmsLck_acquireLockWithTimeout(5000)) != CMSRET_SUCCESS )
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
    
    fastLeaveEnable = igmpObj->igmpFastLeaveEnable;
    cmsObj_free((void **) &igmpObj);

    cmsLck_releaseLock();

    return fastLeaveEnable;
#else
    return 0;
#endif
}

int McastAddReportToList(int type, struct in6_addr *grpIp, t_BCM_MCAST_PKT_INFO *pktInfo, void *repIp)
{
#ifdef BRCM_CMS_BUILD
    BCM_MULTICAST_RULE_ENTRY * newEntry;
    int shouldRemove = 0;
    int ret          = CMSRET_SUCCESS;

    ret = McastFindFdbEntryInList(type, grpIp, pktInfo->rxdev_ifi, shouldRemove);
    if (ret)
    {
        cmsLog_notice("Trying to add an existing entry. ret = %d\n",ret);
        return CMSRET_INTERNAL_ERROR;
    }

    newEntry = cmsMem_alloc(sizeof(BCM_MULTICAST_RULE_ENTRY), ALLOC_ZEROIZE);
    if (newEntry == NULL)
    {
        cmsLog_error("failed to allocate new memory\n");
        return CMSRET_INTERNAL_ERROR;
    }

    memcpy(&newEntry->pktInfo, pktInfo, sizeof(t_BCM_MCAST_PKT_INFO)); 
    memcpy(&newEntry->grpIp, grpIp, sizeof(struct in6_addr));
    memcpy(&newEntry->srcIp, repIp, sizeof(struct in6_addr));  

    BCM_COMMON_LL_APPEND(&gMulticastRuleLL, newEntry);

    return ret;
#else
    return 0;
#endif
}

int McastSetReport(int type, struct in6_addr *grpIp, struct in6_addr *srcIp, McastVlanAction *vlanAction, t_BCM_MCAST_PKT_INFO *pktInfo, U16 *index, McastFdbOp fdbOp)
{
#if defined(SUPPORT_IGMP) || defined(SUPPORT_MLD)
    int ret        = CMSRET_SUCCESS;
    int filterMode = 0;
    U16 fdbIndex   = *index;
    struct in6_addr repIp;

    memset(&repIp, 0, sizeof(repIp));
    ret = McastOptFdbEntryList(type, grpIp, pktInfo, index, &repIp, fdbOp);
    if (ret)
    {
        cmsLog_error("Could not get the L2 entry. ret = %d\n",ret);
        return CMSRET_INTERNAL_ERROR;
    }

    if (fdbOp != McastFdbOpAdd)
    {
        filterMode = BCM_MCAST_SNOOP_EX_CLEAR;
    }
    else
    {
        filterMode = BCM_MCAST_SNOOP_EX_ADD;
    }
    
    cmsLog_debug("brifi(%d),portifi(%d),vid(%d),grpaddr(%x),srcaddr(%x),filter(%d)\n", pktInfo->parent_ifi, 
    pktInfo->rxdev_ifi, pktInfo->tci,grpIp->s6_addr32[3],srcIp->s6_addr32[3],filterMode);
    
    if (fdbIndex == INVALID_INDEX)
    {
        cmsLog_error("INVALID FDB INDEX\n");
        return CMSRET_INTERNAL_ERROR;
    }
    
    if (type == BCM_MCAST_MSG_IGMP_PKT)
    {
        ret = McastUpdateIptvByHost(pktInfo->rxdev_ifi, grpIp, srcIp, pktInfo->tci, IpFamilyIpV4, fdbOp, vlanAction, fdbIndex);
    }
    else
    {
        ret = McastUpdateIptvByHost(pktInfo->rxdev_ifi, grpIp, srcIp, pktInfo->tci, IpFamilyIpV6, fdbOp, vlanAction, fdbIndex);
    }    

    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("Error while setting snooping info. ret = %d\n",ret);
        return ret;
    }

    return ret;
#else
    return CMSRET_INTERNAL_ERROR;
#endif
}

int McastAddOrRemoveFdbEntry(U8 ctrType, IptvEntry *pEntry, U16 *index, McastFdbOp fdbOp)
{
    int ret  = 0;
    int type = 0;
    U16 delIndex = *index;
    struct in6_addr grpIp;
    struct in6_addr srcIp;
    t_BCM_MCAST_PKT_INFO pktInfo;
    char ifname[IF_NAME_SIZE];

    memset(&grpIp, 0, sizeof(struct in6_addr));
    memset(&srcIp, 0, sizeof(struct in6_addr));

    if (fdbOp == McastFdbOpAdd)
    {
        ret = McastFindFdbEntryFreeIndex(index);
        if (ret != 0)
        {
            cmsLog_error("no free entry!\n");
            return ret;
        }
    }
    
    snprintf(ifname, IFNAMESIZ, "eth%d.0", pEntry->userId);
    pktInfo.rxdev_ifi = if_nametoindex(ifname);
    pktInfo.tci = pEntry->vlanTag;

    switch(ctrType)
    {
        case  McastGrpDaMacOnly:
        case  McastGrpDaMacVid: 
        {
            grpIp.s6_addr32[3] = pEntry->da.lowHi.low;

            if (pEntry->da.lowHi.hi == 0x3333)
                type = BCM_MCAST_MSG_MLD_PKT;
            else
                type = BCM_MCAST_MSG_IGMP_PKT;

            break;
        }
        case McastGrpDaIpv4:
        case McastGrpDaIpv4Vid:
        {
            memcpy(&grpIp.s6_addr32[3], &pEntry->groupIp.ipv4Addr[2], sizeof(struct in_addr));
            type = BCM_MCAST_MSG_IGMP_PKT;          
            break;
        }
        case McastGrpDaIpv6:
        case McastGrpDaIpv6Vid:
        {
            memcpy(&grpIp, pEntry->groupIp.ipv6Addr, sizeof(struct in6_addr));
            type = BCM_MCAST_MSG_MLD_PKT;            
            break;
        }        
        case McastGrpDaSaIpv4:
        case McastGrpDaSaIpv4Vid:
        {
            memcpy(&grpIp.s6_addr32[3], &pEntry->groupIp.ipv4Addr[2], sizeof(struct in_addr));
            memcpy(&srcIp.s6_addr32[3], &pEntry->sourceIp.ipv4Addr[2], sizeof(struct in_addr));
            type = BCM_MCAST_MSG_IGMP_PKT;            
            break;
        }       
        case McastGrpDaSaIpv6:
        case McastGrpDaSaIpv6Vid:
        {
            memcpy(&grpIp, pEntry->groupIp.ipv6Addr, sizeof(struct in6_addr));
            memcpy(&srcIp, pEntry->sourceIp.ipv6Addr, sizeof(struct in6_addr));
            type = BCM_MCAST_MSG_MLD_PKT;            
            break;
        } 
        default:
        {
            cmsLog_error("ctrType %d is not supported\n", ctrType);          
            return -1;
        }
    }
    
    ret = McastSetReport(type, &grpIp, &srcIp, &pEntry->vlanAction, &pktInfo, index, fdbOp);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("Error while updating snooping info\n");       
        return ret;
    }

    if (fdbOp == McastFdbOpAdd)
    {
        pEntry->channelIndex = gMcastConfig.fdbEntry[*index].channelIndex;
        memcpy(&gMcastConfig.fdbEntry[gMcastConfig.numEntries], pEntry, sizeof(IptvEntry));
        gMcastConfig.fdbEntry[gMcastConfig.numEntries].index = *index;
        gMcastConfig.numEntries++;    
    }
    else 
    {
        if (delIndex < (gMcastConfig.numEntries - 1))
        {
            memcpy(&gMcastConfig.fdbEntry[delIndex], 
                   &gMcastConfig.fdbEntry[gMcastConfig.numEntries - 1], 
                   sizeof(IptvEntry));                        
        }
        gMcastConfig.numEntries--;
    }
    
    return 0;
}

int McastIgmpProcessInput(t_BCM_MCAST_PKT_INFO *pktInfo)
{
    t_IGMPv12_REPORT *igmp_hdr;
    struct in6_addr grpIp;
    struct in6_addr srcIp;
    int ret   = CMSRET_SUCCESS;

    memset(&grpIp, 0, sizeof(struct in6_addr));
    memset(&srcIp, 0, sizeof(struct in6_addr));
    igmp_hdr = (t_IGMPv12_REPORT *)&pktInfo->pkt[0];
    
    switch (igmp_hdr->type)
    {
        case IGMP_MEMBERSHIP_QUERY:           
        {
            break;
        }
        case IGMP_V1_MEMBERSHIP_REPORT:
        case IGMP_V2_MEMBERSHIP_REPORT:
        {
            grpIp.s6_addr32[3] = igmp_hdr->group.s_addr;
            ret = McastAddReportToList(BCM_MCAST_MSG_IGMP_PKT, &grpIp, pktInfo, &pktInfo->ipv4rep);            
            break;
        }        
        case IGMP_V2_MEMBERSHIP_LEAVE:
        {
#if !defined(DPOE_OAM_BUILD)        
            grpIp.s6_addr32[3] = igmp_hdr->group.s_addr;

            cmsLog_debug("IGMP_V2_MEMBERSHIP_LEAVE, FastLeaveEnabled()=%d\n", McastFastLeaveEnabled());

            if (McastFastLeaveEnabled())
            {
                ret = McastUpdateIptvByLeave(BCM_MCAST_MSG_IGMP_PKT, pktInfo->rxdev_ifi, &grpIp);
            }
#endif            
            break;
        }        
        case IGMP_V3_MEMBERSHIP_REPORT:
        {
            t_IGMPv3_REPORT *report3 = (t_IGMPv3_REPORT *)igmp_hdr;
            grpIp.s6_addr32[3] = report3->group->group.s_addr;
            
            if (report3->group->type == MODE_TO_EXCLUDE)
            {
                ret = McastAddReportToList(BCM_MCAST_MSG_IGMP_PKT, &grpIp, pktInfo, &pktInfo->ipv4rep);
            }
#if !defined(DPOE_OAM_BUILD)            
            else if (report3->group->type == MODE_TO_INCLUDE)
            {
                cmsLog_debug("IGMP_V3_MEMBERSHIP_REPORT, FastLeaveEnabled()=%d\n", McastFastLeaveEnabled());

                if (McastFastLeaveEnabled())
                {
                    ret = McastUpdateIptvByLeave(BCM_MCAST_MSG_IGMP_PKT, pktInfo->rxdev_ifi, &grpIp);                  
                }
            } 
#endif
            break;
        }        
        default:               
        {
            break;
        }
    }
    
    return ret;
}

int McastMldProcessInput(t_BCM_MCAST_PKT_INFO *pktInfo)
{
    t_MLDv1_REPORT *mldv1;
    struct in6_addr srcIp;
    int ret   = CMSRET_SUCCESS;

    memset(&srcIp, 0, sizeof(struct in6_addr));
    mldv1 = (t_MLDv1_REPORT *)&pktInfo->pkt[0];
    switch (mldv1->icmp6_hdr.icmp6_type) 
    {
        case ICMPV6_MLD_V1V2_QUERY:           
        {
            break;
        }   
        case ICMPV6_MLD_V1_DONE:
        {  
#if !defined(DPOE_OAM_BUILD)
            cmsLog_debug("ICMPV6_MLD_V1_DONE, FastLeaveEnabled()=%d\n", McastFastLeaveEnabled());

            if (McastFastLeaveEnabled())
            {
                ret = McastUpdateIptvByLeave(BCM_MCAST_MSG_MLD_PKT, pktInfo->rxdev_ifi, &mldv1->grp_addr);              
            }
#endif
            break;
        }        
        case ICMPV6_MLD_V1_REPORT:
        {
            ret = McastAddReportToList(BCM_MCAST_MSG_MLD_PKT, &mldv1->grp_addr, pktInfo, &pktInfo->ipv6rep);           
            break;
        }        
        case ICMPV6_MLD_V2_REPORT:
        {
            t_MLDv2_REPORT *mldv2 = (t_MLDv2_REPORT *)mldv1;
            
            if (mldv2->group->type == MODE_TO_EXCLUDE)
            {
                ret = McastAddReportToList(BCM_MCAST_MSG_MLD_PKT, &mldv2->group->group, pktInfo, &pktInfo->ipv6rep);
            }
#if !defined(DPOE_OAM_BUILD)        
            else if (mldv2->group->type == MODE_TO_INCLUDE)
            {
                cmsLog_debug("ICMPV6_MLD_V2_REPORT, FastLeaveEnabled()=%d\n", McastFastLeaveEnabled());

                if (McastFastLeaveEnabled())
                {
                    ret = McastUpdateIptvByLeave(BCM_MCAST_MSG_MLD_PKT, pktInfo->rxdev_ifi, &mldv2->group->group);
                }
            }
#endif           
            break;
        }        
        default:            
        {
            break;
        }
    }
    
    return ret;
}

int McastTagOpSet(U8 port, U8 switchVlanNum, U8 tagOp, const McastVlanEntry *pEntry)
{
    int ret        = 0;        
    int i          = 0;
    U32 numTags    = 0;
    U32 ruleId     = 0;
    U16 foundIndex = 0;
    U16 tag        = 0;
    U16 *VlanList  = NULL;
    char realDevName[IF_NAME_SIZE];
    char vlanDevName[IF_NAME_SIZE];
    vlanCtl_direction_t direction = VLANCTL_DIRECTION_TX;
    vlan_rule_data *pRule;

    snprintf(realDevName, IF_NAME_SIZE, "eth%d", port);  
    snprintf(vlanDevName, IF_NAME_SIZE, "eth%d.0", port);

    cmsLog_notice("op: %d; switch entries: %d\n", tagOp, switchVlanNum);
    
    if (tagOp > McastVlanActTrans)
    {
        cmsLog_error("Error: Invalid tagStripped value (%d)", tagOp);
        return -1;
    }
    
    if (switchVlanNum > MAX_MCAST_SWITCHING_ENTRIES_PER_PORT)
    {
        cmsLog_error("Error: Number of VLAN switches(%d) exceeded Max supported \n", switchVlanNum);
        return -1;
    }

    if (gMcastConfig.mcastMode == McastIgmpMode)
    {
        vlanCtl_init();
        
        if (gMcastConfig.mcastPortCfg[port].tagStripped == McastVlanActTrans) 
        {
            
            for (i = 0; i < gMcastConfig.mcastPortCfg[port].numVlanSwitches; i++)
            {
                tag = GET_VID(gMcastConfig.mcastPortCfg[port].entry[i].mcastVlan);              
                if (McastFindVidOnPort(port, tag, &foundIndex))  
                {
                    for (numTags = 1; numTags < MAX_VLANCTL_TAGS; numTags++)
                    {
                        pRule = &gMcastConfig.mcastPortCfg[port].vidRules[foundIndex][numTags];
                        McastDeleteVlanRule(pRule);
                    }
                    
                    vlanCtl_initTagRule();
                    vlanCtl_filterOnRxRealDevice(eponRealIfname);
                    vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_IS_MULTICAST);
                    vlanCtl_filterOnTagVid(tag, 0);
                    vlanCtl_setReceiveVlanDevice(vlanDevName);     
                    
                    for (numTags = 1; numTags < MAX_VLANCTL_TAGS; numTags++) 
                    {   
                         vlanCtl_insertTagRule(realDevName, direction, numTags, 
                                               VLANCTL_POSITION_BEFORE, VLANCTL_DONT_CARE, 
                                               &ruleId);
                         
                         pRule = &gMcastConfig.mcastPortCfg[port].vidRules[foundIndex][numTags];
                         McastSaveVlanRule(pRule, realDevName, direction, numTags, ruleId);
                    }                    
                }        
            }
        }
        else if ((gMcastConfig.mcastPortCfg[port].tagStripped == McastVlanActDel)
                 && (tagOp != McastVlanActDel))
        {
            for (i = 0; i < gMcastConfig.mcastPortCfg[port].numVids; i++)
            {
                for (numTags = 1; numTags < MAX_VLANCTL_TAGS; numTags++)
                {
                    pRule = &gMcastConfig.mcastPortCfg[port].vidRules[i][numTags];
                    McastDeleteVlanRule(pRule);
                }

                vlanCtl_initTagRule();
                vlanCtl_filterOnRxRealDevice(eponRealIfname);
                vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_IS_MULTICAST);
                tag = GET_VID(gMcastConfig.mcastPortCfg[port].vids[i]);
                vlanCtl_filterOnTagVid(tag, 0);
                vlanCtl_setReceiveVlanDevice(vlanDevName);

                for (numTags = 1; numTags < MAX_VLANCTL_TAGS; numTags++)
                {   
                     vlanCtl_insertTagRule(realDevName, direction,numTags,
                                           VLANCTL_POSITION_BEFORE, VLANCTL_DONT_CARE,
                                           &ruleId);
                     pRule = &gMcastConfig.mcastPortCfg[port].vidRules[i][numTags];
                     McastSaveVlanRule(pRule, realDevName, direction, numTags, ruleId);
                }
            }           
        }
        
        if (tagOp == McastVlanActTrans)
        {   
            gMcastConfig.mcastPortCfg[port].numVlanSwitches = switchVlanNum;
            for (i = 0; i < switchVlanNum; i++)
            {
                gMcastConfig.mcastPortCfg[port].entry[i].mcastVlan = OAM_NTOHS(pEntry[i].eponVid);
                gMcastConfig.mcastPortCfg[port].entry[i].userVlan = OAM_NTOHS(pEntry[i].iptvVid);  
                if (McastFindVidOnPort(port, OAM_NTOHS(pEntry[i].eponVid), &foundIndex))
                {
                    for (numTags = 1; numTags < MAX_VLANCTL_TAGS; numTags++)
                    {
                        pRule = &gMcastConfig.mcastPortCfg[port].vidRules[foundIndex][numTags];
                        McastDeleteVlanRule(pRule);
                    }
                    
                    vlanCtl_initTagRule();
                    vlanCtl_filterOnRxRealDevice(eponRealIfname);
                    vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_IS_MULTICAST);
                    vlanCtl_filterOnTagVid(GET_VID(OAM_NTOHS(pEntry[i].eponVid)), 0);
                    vlanCtl_setReceiveVlanDevice(vlanDevName);
                    vlanCtl_cmdSetTagVid(GET_VID(OAM_NTOHS(pEntry[i].iptvVid)), 0);

                    for (numTags = 1; numTags < MAX_VLANCTL_TAGS; numTags++) 
                    {   
                         vlanCtl_insertTagRule(realDevName, direction, numTags,
                                               VLANCTL_POSITION_BEFORE, VLANCTL_DONT_CARE,
                                               &ruleId);
                         pRule = &gMcastConfig.mcastPortCfg[port].vidRules[foundIndex][numTags];
                         McastSaveVlanRule(pRule, realDevName, direction, numTags, ruleId);
                    }                    
                }                
            }
        }
        else if ((tagOp == McastVlanActDel )  
                    && (gMcastConfig.mcastPortCfg[port].tagStripped != McastVlanActDel))
        {
            for (i = 0; i < gMcastConfig.mcastPortCfg[port].numVids; i++)
            {
                for (numTags = 1; numTags < MAX_VLANCTL_TAGS; numTags++) 
                {
                    pRule = &gMcastConfig.mcastPortCfg[port].vidRules[i][numTags];
                    McastDeleteVlanRule(pRule);
                }
                
                vlanCtl_initTagRule();
                vlanCtl_filterOnRxRealDevice(eponRealIfname);
                vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_IS_MULTICAST);
                tag = GET_VID(gMcastConfig.mcastPortCfg[port].vids[i]);
                vlanCtl_filterOnTagVid(tag, 0);
                vlanCtl_setReceiveVlanDevice(vlanDevName);
                vlanCtl_cmdPopVlanTag();

                for (numTags = 1; numTags < MAX_VLANCTL_TAGS; numTags++) 
                {   
                     vlanCtl_insertTagRule(realDevName, direction, numTags,
                                           VLANCTL_POSITION_BEFORE, VLANCTL_DONT_CARE,
                                           &ruleId);
                     pRule = &gMcastConfig.mcastPortCfg[port].vidRules[i][numTags];
                     McastSaveVlanRule(pRule, realDevName, direction, numTags, ruleId);
                }                
            }        
        }   

        vlanCtl_cleanup();

        if ((tagOp == McastVlanActTrans) && (switchVlanNum > 0))
        { 
            VlanList = (U16 *)malloc(sizeof(U16) * switchVlanNum);
            memset(VlanList, 0, sizeof(U16) * switchVlanNum);
        
            for (i = 0; i <  switchVlanNum; i++)
            {
                VlanList[i] = pEntry[i].eponVid;
            }

            McastVlanTagSet(port, switchVlanNum, VlanList, McastVlanOpAdd);

            free(VlanList);
        }
       
        gMcastConfig.mcastPortCfg[port].tagStripped = tagOp; 
    }
    
    /*handle tag operation change in host controllable mode for each port
       basic idea: the action of change of tag operation mode invalids all the 
       iptv entries for this port. so we remove iptv entries which belongs to 
       this port. 
    */
    if (gMcastConfig.mcastMode == McastHostMode)
    {
        if ((ret = McastDelFdbEntriesOnPort(port, McastFdbOpDelTmp)) != 0)
            return ret;
        
        gMcastConfig.mcastPortCfg[port].tagStripped = tagOp;
        /*save new config of vlan switch*/
        if (tagOp == McastVlanActTrans)
        {
            gMcastConfig.mcastPortCfg[port].numVlanSwitches = switchVlanNum;
            for (i = 0; i < switchVlanNum; i++)
            {
                gMcastConfig.mcastPortCfg[port].entry[i].mcastVlan = OAM_NTOHS(pEntry[i].eponVid);
                gMcastConfig.mcastPortCfg[port].entry[i].userVlan = OAM_NTOHS(pEntry[i].iptvVid);
            }
        }
        
        /*now iptv entries, if any, with new tag operation mode should be added back*/        
        for (i=0; i < gMcastConfig.numEntries; i++)
        {
            if (((U16)(port) == gMcastConfig.fdbEntry[i].userId) 
               && (gMcastConfig.fdbEntry[i].index == INVALID_INDEX))
            {
                if ((ret = McastAddOrRemoveFdbEntry(gMcastConfig.ctrlType, 
                                                    &gMcastConfig.fdbEntry[i], 
                                                    &gMcastConfig.fdbEntry[i].index,
                                                    McastFdbOpAdd)) != 0)
                {
                    cmsLog_error("failed to add iptv entry");
                    return -1;
                }
            }
        }  
    }
    
    return ret;
}

U8 McastTransVlanOnPortGet(U8 vlan, U8 port)
{
    int i = 0;
    
    for (i = 0; i < gMcastConfig.mcastPortCfg[port].numVlanSwitches; i++)
    {
        if (gMcastConfig.mcastPortCfg[port].entry[i].mcastVlan == vlan)
        {
            return gMcastConfig.mcastPortCfg[port].entry[i].userVlan;
        }
    }
    
    return 0;
}

void McastVlanActionOnPortGet(IptvEntry *pEntry)
{
    int i = 0;
    pEntry->vlanAction.action = gMcastConfig.mcastPortCfg[pEntry->userId].tagStripped;
    pEntry->vlanAction.vlanId = 0;

    if (pEntry->vlanAction.action == McastVlanActTrans)
    {
        for (i = 0; i < gMcastConfig.mcastPortCfg[pEntry->userId].numVlanSwitches; i++)
        {
            if (gMcastConfig.mcastPortCfg[pEntry->userId].entry[i].mcastVlan == pEntry->vlanTag)
            {
                pEntry->vlanAction.vlanId  = gMcastConfig.mcastPortCfg[pEntry->userId].entry[i].userVlan;
                return;
            }
        }
        
        pEntry->vlanAction.action = McastVlanActNone;
    }
}

U8 McastModeGet(void)
{   
    return gMcastConfig.mcastMode;
}

void McastModeSet(U8 mcastMode)
{   
    gMcastConfig.mcastMode = mcastMode;
}

U16 McastFdbUserIdGet(int index)
{
    return gMcastConfig.fdbEntry[index].userId;
}

U16 McastFdbVlanGet(int index)
{
    return gMcastConfig.fdbEntry[index].vlanTag;
}

U8 McastFdbEntryNumGet(void)
{
    return gMcastConfig.numEntries;
}

void  McastFdbEntryNumSet(U8 num)
{
    gMcastConfig.numEntries = num;
}

void McastFdbDaCpy(int index, U8 *daMac)
{
    memcpy(daMac, gMcastConfig.fdbEntry[index].da.u8, 6);
    
    return;
}

void McastFdbIpCpy(int index, U8 *ipAddr, U8 ipVer)
{
    if (ipVer == IpFamilyIpV4)
    {
        memcpy(ipAddr, gMcastConfig.fdbEntry[index].groupIp.ipv4Addr, 6);
    }
    else
    {
        memcpy(ipAddr, gMcastConfig.fdbEntry[index].groupIp.ipv6Addr, 16);
    }

    return;
}

int McastFdbDaCmp(int index, U8 *daMac)
{
    return memcmp(gMcastConfig.fdbEntry[index].da.u8, daMac, 6);
}

int McastFdbGroupIpCmp(int index, U8 *ipAddr, U8 ipVer)
{
    if (ipVer == IpFamilyIpV4)
    {
        return memcmp(gMcastConfig.fdbEntry[index].groupIp.ipv4Addr, ipAddr, 6);
    }
    else
    {
        return memcmp(gMcastConfig.fdbEntry[index].groupIp.ipv6Addr, ipAddr, 16);
    }
}

int McastFdbSourceIpCmp(int index, U8 *ipAddr, U8 ipVer)
{
    if (ipVer == IpFamilyIpV4)
    {
        return memcmp(gMcastConfig.fdbEntry[index].sourceIp.ipv4Addr, ipAddr, 6);
    }
    else
    {
        return memcmp(gMcastConfig.fdbEntry[index].sourceIp.ipv6Addr, ipAddr, 16);
    }
}

U8 McastCtrlTypeGet(void)
{
    return gMcastConfig.ctrlType;
}

void McastCtrlTypeSet(U8 ctrlType)
{
    gMcastConfig.ctrlType = ctrlType;
}

void McastPortVlanOpSet(U8 port, U8 op)
{
    gMcastConfig.mcastPortCfg[port].tagStripped = op;  
}

U8 McastPortVlanOpGet(U8 port)
{
    return gMcastConfig.mcastPortCfg[port].tagStripped;
}

U8 McastPortVlanSwitchNumGet(U8 port)
{
    return gMcastConfig.mcastPortCfg[port].numVlanSwitches;
}

U16 McastPortEntryMcastVlanGet(U8 port,int index)
{
    return gMcastConfig.mcastPortCfg[port].entry[index].mcastVlan;
}

U16 McastPortEntryUserVlanGet(U8 port,int index)
{
    return gMcastConfig.mcastPortCfg[port].entry[index].userVlan;
}

U8 McastPortVidNumGet(U8 port)
{
    return gMcastConfig.mcastPortCfg[port].numVids;
}

U16 McastPortVidGet(U8 port, int index)
{
    return gMcastConfig.mcastPortCfg[port].vids[index];
}

U8 McastGroupMaxGet(U8 port)
{
    return gMcastConfig.mcastPortCfg[port].maxGroups;
}

void McastGroupMaxSet(U8 port, U8 max)
{
    gMcastConfig.mcastPortCfg[port].maxGroups = max;
} 

#ifdef DPOE_OAM_BUILD
U16 McastFdbPortUsedGet(U8 port, int index)
{
    return gMcastConfig.fdbEntry[index].portUsed[port];
}

void McastFdbPortUsedInc(U8 port, int index)
{
    gMcastConfig.fdbEntry[index].portUsed[port]++;
}

void McastFdbPortUsedDec(U8 port, int index)
{
    gMcastConfig.fdbEntry[index].portUsed[port]--;
}

void McastFdbClearByIndex(U8 index)
{
    memset(&gMcastConfig.fdbEntry[index], 0, sizeof(IptvEntry));
    gMcastConfig.fdbEntry[index].index = INVALID_INDEX;
}
#endif // endif for #ifdef EPON_SFU

#endif

int McastIngressFiltersSet(rdpa_ports port, rdpa_filter filterType, rdpa_forward_action action)
{
#ifdef EPON_SFU
    int rc;
    rdpa_filter_key_t entry_key;
    rdpa_filter_ctrl_t entry_ctrl;

    /* Check if the filter type is invalid */
    if (filterType != RDPA_FILTER_IGMP 
        && filterType != RDPA_FILTER_MLD
        && filterType != RDPA_FILTER_ICMPV6
        && filterType != RDPA_FILTER_MCAST_IPV4
        && filterType != RDPA_FILTER_MCAST_IPV6
        && filterType != RDPA_FILTER_IP_MCAST_CONTROL)
    {
        cmsLog_error("Mcast Ingress Filters is invalid");
        return -1;
    }
    
    entry_key.filter = filterType;
    entry_key.ports = port;
    entry_ctrl.enabled = TRUE;
    entry_ctrl.action = action;

    rc = rdpaCtl_filter_entry_create(&entry_key, &entry_ctrl);
    if (rc)
    {
        cmsLog_error("Mcast Ingress Filters Set Error");
        return -1;
    }
#endif

    return 0;
}

void McastInit(void)
{
#ifdef EPON_SFU
    int i = 0;

    memset(&gMcastConfig, 0, sizeof(gMcastConfig));
    gMcastConfig.mcastMode = McastInvalidMode;
    
    for (i=0; i< MAX_MCAST_FDB_ENTRIES; i++)
    {
        gMcastConfig.fdbEntry[i].index = INVALID_INDEX;
    }
    
    eponsdk_igmp_process_input_func = McastIgmpProcessInput;
    eponsdk_mld_process_input_func  = McastMldProcessInput; 
#endif
}

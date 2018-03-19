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
/// \file CtcVlan.c
/// \brief China Telecom VLAN mode configuration module
/// \author Lingyong Chen
/// \date November 2, 2010
///
///
////////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include "PortCapability.h"
#include "CtcVlan.h"
#include "VlanConfig.h"

#include "TkPlatform.h"
#include "rdpactl_api.h"
#include "ethswctl_api.h"
#include "EponDevInfo.h"
#include "cms_log.h"
#include <bridgeutil.h>


#define Single_LLID_Flow 0
#define PRI_TAG_NUM  61
#define PRI_TAG_NUM_VID  62

typedef struct  {
    U8 num_translate_entries;
    CtcVlanTranslatate xlate_entries[MAX_PERMITTED_VLANS];
} CtcVlanXlateEntries;

typedef struct  {
    U8 num_aggr_entries;
    EthernetVlanData aggregatedVlan;
    EthernetVlanData aggregate_vlans[MAX_PERMITTED_VLANS];
} CtcVlanN21AggrEntries;


typedef struct  {
    U8 mode;
    U16 default_tag_tpid;
    U16 default_tag_tci;
    U8 num_aggr_tables;
    union {
        CtcVlanTrunkEntries trunk_rules;
        CtcVlanXlateEntries xlate_tables;
        CtcVlanN21AggrEntries aggrN21_tables[MAX_PERMITTED_AG_GROUP];
    }vlanRules;
    U32 num_vlanctl_rules;
    vlan_rule_data vlanctl_rules[MAX_VLANCTL_RULE_IDS];
} CtcVlanRules;

typedef struct{
    U8 num;
    VlanTag vlans[MAX_PERMITTED_VLANS*MAX_PERMITTED_AG_GROUP];
    }CtcVlanList;

static CtcVlanRules uni_vlan_cfg[MAX_UNI_PORTS];
static CtcVlanList uni_vlanlist_cfg[MAX_UNI_PORTS];

static void getCtcVlanData(int port, U8 BULK *vlan, U32 *pOutLen);

static void CtcVlanListCfg(U32 uni_port,BOOL add) 
{
    int i;
    int fordmap = 1<<uni_port;
    int untagmap = add?0:fordmap;

    for(i=0;i<uni_vlanlist_cfg[uni_port].num;i++)
    {
        bcm_vlan_port_set(0, uni_vlanlist_cfg[uni_port].vlans[i], fordmap, untagmap);
    }
}

#ifdef EPON_SFU
static void save_rule(U32 uni_port, char *devName, 
    vlanCtl_direction_t direction, unsigned int num_tags, unsigned int rule_idx) 
{
    int rule_num = uni_vlan_cfg[uni_port].num_vlanctl_rules; 
    uni_vlan_cfg[uni_port].vlanctl_rules[rule_num].nbrOfTags = num_tags;
    uni_vlan_cfg[uni_port].vlanctl_rules[rule_num].dir = direction;
    uni_vlan_cfg[uni_port].vlanctl_rules[rule_num].tagRuleId = rule_idx;
    strcpy(uni_vlan_cfg[uni_port].vlanctl_rules[rule_num].devName, devName);
    uni_vlan_cfg[uni_port].num_vlanctl_rules++;
}

////////////////////////////////////////////////////////////////////////////////
/// CtcVlanSetTransparent - Enable transparent mode
///
/// This function sets the current CTC VLAN mode to transparent.
///
 // Parameters:
/// \param port Port number, including EPON
///
/// \return
/// True if operation was successful
////////////////////////////////////////////////////////////////////////////////
static
void CtcVlanSetTransparent (TkOnuEthPort uni_port)
    {
    unsigned int num_tags, ruleId = 0;
    vlanCtl_direction_t direction = VLANCTL_DIRECTION_TX;
    char realDevName[16];
    char vlanDevName[16];
	
    /*  Hard coding the vlan interface name for now. TBD to get this name */
    snprintf(realDevName, sizeof(vlanDevName), "%s", uniRealIfname[uni_port]);
    snprintf(vlanDevName, sizeof(vlanDevName), "%s.0", realDevName);

    /* Transparently send upstream packets */
    vlanCtl_initTagRule();
    vlanCtl_filterOnRxRealDevice(realDevName);
    vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_ALL);
	vlanCtl_cmdSetSkbMarkPort(Single_LLID_Flow);
    num_tags = 0;
    vlanCtl_insertTagRule(eponRealIfname, direction, num_tags,
        VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, eponRealIfname, direction, num_tags, ruleId);
    num_tags = 1;
    vlanCtl_insertTagRule(eponRealIfname, direction, num_tags,
        VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, eponRealIfname, direction, num_tags, ruleId);
    num_tags = 2;
    vlanCtl_insertTagRule(eponRealIfname, direction, num_tags,
        VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, eponRealIfname, direction, num_tags, ruleId);
	
    /* Transparently send downstream packets */
    vlanCtl_initTagRule();
    vlanCtl_filterOnRxRealDevice(eponRealIfname);
    vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_IS_UNICAST|BCM_VLAN_FILTER_FLAGS_IS_BROADCAST );    
    num_tags = 0;
    vlanCtl_insertTagRule(realDevName, direction,
       num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, realDevName, direction, num_tags, ruleId);
    num_tags = 1;
    vlanCtl_insertTagRule(realDevName, direction,
       num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, realDevName, direction, num_tags, ruleId);
    num_tags = 2;
    vlanCtl_insertTagRule(realDevName, direction,
       num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, realDevName, direction, num_tags, ruleId);
    return;
    } // CtcVlanSetTransparent



////////////////////////////////////////////////////////////////////////////////
/// CtcVlanSetTag - Enable VLAN tag mode
///
/// This function sets the current CTC VLAN mode to tag.
///
 // Parameters:
/// \param port Port number, including EPON
///
/// \return
/// True if operation was successful
////////////////////////////////////////////////////////////////////////////////
void CtcVlanSetTag (TkOnuEthPort uni_port, CtcOamVlanTag  *pTagInfo)
    {
    unsigned int num_tags, ruleId = 0;
    vlanCtl_direction_t direction = VLANCTL_DIRECTION_TX;
    char realDevName[16];
    char vlanDevName[16];
	
    cmsLog_debug("type:0x%x, tag:0x%x", OAM_NTOHS(pTagInfo->tag[0].type), OAM_NTOHS(pTagInfo->tag[0].tag));
	
    /*  Hard coding the vlan interface name for now. TBD to get this name */
    snprintf(realDevName, sizeof(vlanDevName), "%s", uniRealIfname[uni_port]);
    snprintf(vlanDevName, sizeof(vlanDevName), "%s.0", realDevName);
    
    uni_vlan_cfg[uni_port].default_tag_tpid = OAM_NTOHS(pTagInfo->tag[0].type);
    uni_vlan_cfg[uni_port].default_tag_tci = OAM_NTOHS(pTagInfo->tag[0].tag);
    
    /* Add the VLAN tag for upstream untagged packets */
    vlanCtl_initTagRule();
    vlanCtl_filterOnTxVlanDevice(eponVlanIfname);
    vlanCtl_filterOnRxRealDevice(realDevName);
    vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_ALL);
    vlanCtl_cmdPushVlanTag();
    vlanCtl_cmdSetSkbMarkPort(Single_LLID_Flow);
    vlanCtl_cmdSetTagVid(GET_VID(OAM_NTOHS(pTagInfo->tag[0].tag)), 0);
    vlanCtl_cmdSetTagPbits(GET_PBITS(OAM_NTOHS(pTagInfo->tag[0].tag)), 0);
    vlanCtl_cmdSetEtherType(EthertypeCvlan);
    num_tags = 0;
    vlanCtl_insertTagRule(eponRealIfname, direction, num_tags,
        VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, eponRealIfname, direction, num_tags, ruleId);

    /* Set the VLAN tag for upstream p-tagged packets */
    vlanCtl_initTagRule();
    vlanCtl_filterOnTxVlanDevice(eponVlanIfname);
    vlanCtl_filterOnTagVid(0, 0);
    vlanCtl_filterOnRxRealDevice(realDevName);
    vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_ALL);
    vlanCtl_cmdSetSkbMarkPort(Single_LLID_Flow);
    vlanCtl_cmdSetTagVid(GET_VID(OAM_NTOHS(pTagInfo->tag[0].tag)), 0);
    vlanCtl_cmdSetTagPbits(GET_PBITS(OAM_NTOHS(pTagInfo->tag[0].tag)), 0);
    num_tags = 1;
    vlanCtl_insertTagRule(eponRealIfname, direction, num_tags,
        VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, eponRealIfname, direction, num_tags, ruleId);

    uni_vlanlist_cfg[uni_port].vlans[uni_vlanlist_cfg[uni_port].num++] = 0;


    /* Disacrd upstream packets with VLAN tag */
    vlanCtl_initTagRule();
    vlanCtl_filterOnTxVlanDevice(eponVlanIfname);
    vlanCtl_filterOnRxRealDevice(realDevName);
    vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_ALL);
    vlanCtl_cmdDropFrame();
    num_tags = 1;
    vlanCtl_insertTagRule(eponRealIfname, direction,
        num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, eponRealIfname, direction, num_tags, ruleId);
    num_tags = 2;
    vlanCtl_insertTagRule(eponRealIfname, direction,
        num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, eponRealIfname, direction, num_tags, ruleId);


    /* Untag DS pkts with the default vlan tag */
    vlanCtl_initTagRule();
    vlanCtl_setReceiveVlanDevice(vlanDevName);
    vlanCtl_filterOnTagVid(GET_VID(OAM_NTOHS(pTagInfo->tag[0].tag)), 0);
    vlanCtl_filterOnRxRealDevice(eponRealIfname);
    vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_IS_UNICAST|BCM_VLAN_FILTER_FLAGS_IS_BROADCAST );        
    vlanCtl_cmdPopVlanTag();
    num_tags = 1;
    vlanCtl_insertTagRule(realDevName, direction,
       num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, realDevName, direction, num_tags, ruleId);
    num_tags = 2;
    vlanCtl_insertTagRule(realDevName, direction,
       num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, realDevName, direction, num_tags, ruleId);

    /* Discard downstream if the tag is not configured tag */
    /* Discard downstream if untagged */
    vlanCtl_initTagRule();
    vlanCtl_setReceiveVlanDevice(vlanDevName);
    vlanCtl_filterOnRxRealDevice(eponRealIfname);
    vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_IS_UNICAST|BCM_VLAN_FILTER_FLAGS_IS_BROADCAST );    
    vlanCtl_cmdDropFrame();
    num_tags = 0;
    vlanCtl_insertTagRule(realDevName, direction,
       num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, realDevName, direction, num_tags, ruleId);
    num_tags = 1;
    vlanCtl_insertTagRule(realDevName, direction,
       num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, realDevName, direction, num_tags, ruleId);
    num_tags = 2;
    vlanCtl_insertTagRule(realDevName, direction,
       num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, realDevName, direction, num_tags, ruleId);
	
    return;
    } // CtcVlanSetTag



////////////////////////////////////////////////////////////////////////////////
/// CtcVlanSetTranslation - Enable translation mode
///
/// This function sets the current CTC VLAN mode to transparent.
///
 // Parameters:
/// \param port Port number, including EPON
///
/// \return
/// True if operation was successful
////////////////////////////////////////////////////////////////////////////////
static
void CtcVlanSetTranslation (TkOnuEthPort uni_port, 
                            CtcOamVlanTranslate *pVlanXlateEntry, 
                            U16 size)
{
    U32 entries;
    unsigned int i;
    unsigned int num_tags, ruleId = 0;
    vlanCtl_direction_t direction = VLANCTL_DIRECTION_TX;
    char realDevName[16];
    char vlanDevName[16];
	
    /*  Hard coding the vlan interface name for now. TBD to get this name */
    snprintf(realDevName, sizeof(vlanDevName), "%s", uniRealIfname[uni_port]);
    snprintf(vlanDevName, sizeof(vlanDevName), "%s.0", realDevName);
	
     uni_vlan_cfg[uni_port].default_tag_tpid = OAM_NTOHS(pVlanXlateEntry->defaultVlan.type);
     uni_vlan_cfg[uni_port].default_tag_tci = OAM_NTOHS(pVlanXlateEntry->defaultVlan.tag);
 
     entries = ((size - 4) / (sizeof(EthernetVlanData) * 2)) ;
     if (entries > MAX_PERMITTED_VLANS) {
         cmsLog_error("entries exceeds MAX_PERMITTED_VLANS\n");
         return;
     }
     uni_vlan_cfg[uni_port].vlanRules.xlate_tables.num_translate_entries = entries;

    /* Add the default tag for US untagged traffic (ethx -> epon0) */
    vlanCtl_initTagRule();
    vlanCtl_filterOnTxVlanDevice(eponVlanIfname);
    vlanCtl_filterOnRxRealDevice(realDevName);
    vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_ALL);
    vlanCtl_cmdPushVlanTag();
    vlanCtl_cmdSetSkbMarkPort(Single_LLID_Flow);
    vlanCtl_cmdSetTagVid(GET_VID(OAM_NTOHS(pVlanXlateEntry->defaultVlan.tag)), 0);
    vlanCtl_cmdSetTagPbits(GET_PBITS(OAM_NTOHS(pVlanXlateEntry->defaultVlan.tag)), 0);
    vlanCtl_cmdSetEtherType(EthertypeCvlan);
    num_tags = 0;
    vlanCtl_insertTagRule(eponRealIfname, direction,
        num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, eponRealIfname, direction, num_tags, ruleId);

    /* Set the VLAN tag for upstream p-tagged packets */
    vlanCtl_initTagRule();
    vlanCtl_filterOnTxVlanDevice(eponVlanIfname);
    vlanCtl_filterOnTagVid(0, 0);
    vlanCtl_filterOnRxRealDevice(realDevName);
    vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_ALL);
    vlanCtl_cmdSetSkbMarkPort(Single_LLID_Flow);
    vlanCtl_cmdSetTagVid(GET_VID(OAM_NTOHS(pVlanXlateEntry->defaultVlan.tag)), 0);
    vlanCtl_cmdSetTagPbits(GET_PBITS(OAM_NTOHS(pVlanXlateEntry->defaultVlan.tag)), 0);
    num_tags = 1;
    vlanCtl_insertTagRule(eponRealIfname, direction, num_tags,
        VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, eponRealIfname, direction, num_tags, ruleId);

    uni_vlanlist_cfg[uni_port].vlans[uni_vlanlist_cfg[uni_port].num++] = 0;

    /* for US tagged, translate the tag if it is in the fromVID column of translation table. Else discard */
    for (i=0; i< entries; i++) {
        /* Translate the tag for US traffic from  ethx -> epon0  */
        vlanCtl_initTagRule();
        vlanCtl_setReceiveVlanDevice(eponVlanIfname);
        vlanCtl_filterOnTagVid(GET_VID(OAM_NTOHS(pVlanXlateEntry->vlanTranslateArry[i].fromVid.tag)), 0);
        vlanCtl_filterOnRxRealDevice(realDevName);
        vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_ALL);
        vlanCtl_cmdSetSkbMarkPort(Single_LLID_Flow);
        vlanCtl_cmdSetTagVid(GET_VID(OAM_NTOHS(pVlanXlateEntry->vlanTranslateArry[i].toVid.tag)), 0);
        num_tags = 1;
        vlanCtl_insertTagRule(eponRealIfname, direction,
            num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
        save_rule(uni_port, eponRealIfname, direction, num_tags, ruleId);
        num_tags = 2;
        vlanCtl_insertTagRule(eponRealIfname, direction,
            num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
        save_rule(uni_port, eponRealIfname, direction, num_tags, ruleId);


        /* Translate the tag for DS traffic from epon0 -> ethX.0 */
        vlanCtl_initTagRule();
        vlanCtl_setReceiveVlanDevice(vlanDevName);
        vlanCtl_filterOnTagVid(GET_VID(OAM_NTOHS(pVlanXlateEntry->vlanTranslateArry[i].toVid.tag)), 0);
        vlanCtl_filterOnRxRealDevice(eponRealIfname);
        vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_IS_UNICAST|BCM_VLAN_FILTER_FLAGS_IS_BROADCAST );    
        vlanCtl_cmdSetTagVid(GET_VID(OAM_NTOHS(pVlanXlateEntry->vlanTranslateArry[i].fromVid.tag)), 0);
        num_tags = 1;
        vlanCtl_insertTagRule(realDevName, direction,
            num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
        save_rule(uni_port, realDevName, direction, num_tags, ruleId);
        num_tags = 2;
        vlanCtl_insertTagRule(realDevName, direction,
            num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
        save_rule(uni_port, realDevName, direction, num_tags, ruleId);
        uni_vlan_cfg[uni_port].vlanRules.xlate_tables.xlate_entries[i].fromVid.tag = 
            OAM_NTOHS(pVlanXlateEntry->vlanTranslateArry[i].fromVid.tag);
        uni_vlan_cfg[uni_port].vlanRules.xlate_tables.xlate_entries[i].fromVid.type = 
            OAM_NTOHS(pVlanXlateEntry->vlanTranslateArry[i].fromVid.type);
        uni_vlan_cfg[uni_port].vlanRules.xlate_tables.xlate_entries[i].toVid.tag = 
            OAM_NTOHS(pVlanXlateEntry->vlanTranslateArry[i].toVid.tag);
        uni_vlan_cfg[uni_port].vlanRules.xlate_tables.xlate_entries[i].toVid.type = 
            OAM_NTOHS(pVlanXlateEntry->vlanTranslateArry[i].toVid.type);

        
        uni_vlanlist_cfg[uni_port].vlans[uni_vlanlist_cfg[uni_port].num++] = GET_VID(OAM_NTOHS(pVlanXlateEntry->vlanTranslateArry[i].fromVid.tag));
    }

    /*discard upstream packets that vlan is not in the list*/
    vlanCtl_initTagRule();
    vlanCtl_filterOnTxVlanDevice(eponVlanIfname);
    vlanCtl_filterOnRxRealDevice(realDevName);
    vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_ALL);
    vlanCtl_cmdDropFrame();
    num_tags = 1;
    vlanCtl_insertTagRule(eponRealIfname, direction,
        num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, eponRealIfname, direction, num_tags, ruleId);
    num_tags = 2;
    vlanCtl_insertTagRule(eponRealIfname, direction,
        num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, eponRealIfname, direction, num_tags, ruleId);

    /* for DS, translate the tagged if VLAN is in the toVid column of translation table. Else discard if not default Vlan. 
                     If default Vlan, strip the tag and forward */
    vlanCtl_initTagRule();
    vlanCtl_setReceiveVlanDevice(vlanDevName);
    vlanCtl_filterOnTagVid(GET_VID(OAM_NTOHS(pVlanXlateEntry->defaultVlan.tag)), 0);
    vlanCtl_filterOnRxRealDevice(eponRealIfname);
    vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_IS_UNICAST|BCM_VLAN_FILTER_FLAGS_IS_BROADCAST );    
    vlanCtl_cmdPopVlanTag();
    num_tags = 1;
    vlanCtl_insertTagRule(realDevName, direction,
       num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, realDevName, direction, num_tags, ruleId);
    num_tags = 2;
    vlanCtl_insertTagRule(realDevName, direction,
       num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, realDevName, direction, num_tags, ruleId);

    /* Discard DS untagged.  */
    vlanCtl_initTagRule();
    vlanCtl_setReceiveVlanDevice(vlanDevName);
    vlanCtl_filterOnRxRealDevice(eponRealIfname);
    vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_IS_UNICAST|BCM_VLAN_FILTER_FLAGS_IS_BROADCAST );    
    vlanCtl_cmdDropFrame();
    num_tags = 0;
    vlanCtl_insertTagRule(realDevName, direction,
       num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, realDevName, direction, num_tags, ruleId);
    num_tags = 1;
    vlanCtl_insertTagRule(realDevName, direction,
       num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, realDevName, direction, num_tags, ruleId);
    num_tags = 2;
    vlanCtl_insertTagRule(realDevName, direction,
       num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, realDevName, direction, num_tags, ruleId);	
} // CtcVlanSetTranslation



////////////////////////////////////////////////////////////////////////////////
/// CtcVlanSetAggregation - Enable aggregation mode
///
/// This function sets the current CTC VLAN mode to aggregation.
///
 // Parameters:
/// \param port Port number, including EPON
///
/// \return
/// True if operation was successful
////////////////////////////////////////////////////////////////////////////////
static
void CtcVlanSetAggregation (TkOnuEthPort uni_port, 
                            CtcOamVlanN21Translate *pVlanN21XlateEntry)
{
    U32 entries;
    unsigned int i, j;
    unsigned int num_tags, ruleId = 0;
    vlanCtl_direction_t direction = VLANCTL_DIRECTION_TX;
    CtcOamVlanN21Table *pVlanN21Table = (CtcOamVlanN21Table *)pVlanN21XlateEntry->vlanAggrTable;
    char realDevName[16];
    char vlanDevName[16];
	
    /*  Hard coding the vlan interface name for now. TBD to get this name */
    snprintf(realDevName, sizeof(vlanDevName), "%s", uniRealIfname[uni_port]);
    snprintf(vlanDevName, sizeof(vlanDevName), "%s.0", realDevName);

    uni_vlan_cfg[uni_port].num_aggr_tables = OAM_NTOHS(pVlanN21XlateEntry->numVlanAggrTables);
    uni_vlan_cfg[uni_port].default_tag_tpid = OAM_NTOHS(pVlanN21XlateEntry->defaultVlan.type);
    uni_vlan_cfg[uni_port].default_tag_tci = OAM_NTOHS(pVlanN21XlateEntry->defaultVlan.tag);

    /* Tag the upstream untagged. */
    /* for US, translate the tagged if VLAN is one of the VLANs being aggregated. Else discard */
    /* for DS, translate the tagged if VLAN is an aggregated VLAN. Else discard if not default Vlan. 
                     If default Vlan, strip the tag and forward */
    /* Discard DS untagged.  */


    /* Add the default tag for US untagged traffic (ethx -> epon0) */
    vlanCtl_initTagRule();
    vlanCtl_filterOnTxVlanDevice(eponVlanIfname);
    vlanCtl_filterOnRxRealDevice(realDevName);
    vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_ALL);
    vlanCtl_cmdPushVlanTag();
    vlanCtl_cmdSetSkbMarkPort(Single_LLID_Flow);
    vlanCtl_cmdSetTagVid(GET_VID(OAM_NTOHS(pVlanN21XlateEntry->defaultVlan.tag)), 0);
    vlanCtl_cmdSetTagPbits(GET_PBITS(OAM_NTOHS(pVlanN21XlateEntry->defaultVlan.tag)), 0);
    vlanCtl_cmdSetEtherType(EthertypeCvlan);
    num_tags = 0;
    vlanCtl_insertTagRule(eponRealIfname, direction,
        num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, eponRealIfname, direction, num_tags, ruleId);

    /* Set the VLAN tag for upstream p-tagged packets */
    vlanCtl_initTagRule();
    vlanCtl_filterOnTxVlanDevice(eponVlanIfname);
    vlanCtl_filterOnTagVid(0, 0);
    vlanCtl_filterOnRxRealDevice(realDevName);
    vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_ALL);
    vlanCtl_cmdSetSkbMarkPort(Single_LLID_Flow);
    vlanCtl_cmdSetTagVid(GET_VID(OAM_NTOHS(pVlanN21XlateEntry->defaultVlan.tag)), 0);
    vlanCtl_cmdSetTagPbits(GET_PBITS(OAM_NTOHS(pVlanN21XlateEntry->defaultVlan.tag)), 0);
    num_tags = 1;
    vlanCtl_insertTagRule(eponRealIfname, direction, num_tags,
        VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, eponRealIfname, direction, num_tags, ruleId);

    uni_vlanlist_cfg[uni_port].vlans[uni_vlanlist_cfg[uni_port].num++] = 0;
    
#if defined(BCM_PON_XRDP)
    vlanCtl_setDefaultAction(uniRealIfname[uni_port], VLANCTL_DIRECTION_RX, 1, VLANCTL_ACTION_DROP, NULL);
    vlanCtl_setDefaultAction(uniRealIfname[uni_port], VLANCTL_DIRECTION_RX, 2, VLANCTL_ACTION_DROP, NULL);

    for (j = 0; j < uni_vlan_cfg[uni_port].num_aggr_tables; j++) {
        int rule_created;
        char uniAggVlanDevName[16];
        char eponAggVlanDevName[16];
        snprintf(uniAggVlanDevName, sizeof(uniAggVlanDevName), "%s.1.%d", realDevName, GET_VID(OAM_NTOHS(pVlanN21Table->aggregatedVlan.tag)));
        snprintf(eponAggVlanDevName, sizeof(eponAggVlanDevName), "%s.2.%d", eponRealIfname, GET_VID(OAM_NTOHS(pVlanN21Table->aggregatedVlan.tag)));

        entries = OAM_NTOHS(pVlanN21Table->numAggrEntries);

        uni_vlan_cfg[uni_port].vlanRules.aggrN21_tables[j].num_aggr_entries = entries;
        uni_vlan_cfg[uni_port].vlanRules.aggrN21_tables[j].aggregatedVlan.tag = 
            OAM_NTOHS(pVlanN21Table->aggregatedVlan.tag);
        uni_vlan_cfg[uni_port].vlanRules.aggrN21_tables[j].aggregatedVlan.type = 
            OAM_NTOHS(pVlanN21Table->aggregatedVlan.type);

        for (i = 0, rule_created = 0; !rule_created && i < MAX_UNI_PORTS; i++)
        {
            int k;
            for (k = 0; !rule_created && (k < uni_vlan_cfg[i].num_aggr_tables); k++)
            {
                if((GET_VID(OAM_NTOHS(pVlanN21Table->aggregatedVlan.tag)) ==
                    uni_vlan_cfg[i].vlanRules.aggrN21_tables[k].aggregatedVlan.tag)
                    && (uni_port != i))
                {
                    rule_created = 1;
                }
            }
        }

        if (!rule_created)
        {
            vlanCtl_createVlanInterfaceByName(eponRealIfname, eponAggVlanDevName, 0, 1);
            
            vlanCtl_initTagRule();
            vlanCtl_filterOnTagVid(GET_VID(OAM_NTOHS(pVlanN21Table->aggregatedVlan.tag)), 0);
            vlanCtl_setReceiveVlanDevice(eponAggVlanDevName);
            num_tags = 1;
            vlanCtl_insertTagRule(eponRealIfname, VLANCTL_DIRECTION_RX,
                num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
            save_rule(uni_port, eponRealIfname, VLANCTL_DIRECTION_RX, num_tags, ruleId);
        }

        vlanCtl_createVlanInterfaceByName(uniRealIfname[uni_port], uniAggVlanDevName, 0, 1);

        /* Translate the tag for US traffic from  ethx -> epon0  */
        vlanCtl_initTagRule();
        vlanCtl_setReceiveVlanDevice(uniAggVlanDevName);
        vlanCtl_filterOnRxRealDevice(realDevName);
        vlanCtl_filterOnTxVlanDevice(eponAggVlanDevName);
        vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_ALL);
        vlanCtl_cmdSetSkbMarkPort(Single_LLID_Flow);
        vlanCtl_cmdSetTagVid(GET_VID(OAM_NTOHS(pVlanN21Table->aggregatedVlan.tag)), 0);
        num_tags = 1;
        vlanCtl_insertTagRule(eponRealIfname, direction,
            num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
        save_rule(uni_port, eponRealIfname, direction, num_tags, ruleId);
        num_tags = 2;
        vlanCtl_insertTagRule(eponRealIfname, direction,
            num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
        save_rule(uni_port, eponRealIfname, direction, num_tags, ruleId);

        for (i = 0; i < entries; i++) {
            vlanCtl_initTagRule();
            vlanCtl_filterOnTagVid(GET_VID(OAM_NTOHS(pVlanN21Table->vlanAggrArry[i].tag)), 0);
            vlanCtl_setReceiveVlanDevice(uniAggVlanDevName);
            num_tags = 1;
            vlanCtl_insertTagRule(realDevName, VLANCTL_DIRECTION_RX,
                num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
            save_rule(uni_port, realDevName, VLANCTL_DIRECTION_RX, num_tags, ruleId);

            /* Reverse translation rule for DS is automatic in FAP. If needed in MIPS, 
                                  DA MAC filtering in vlanctl will have to be implemented */

            /* Save the aggregation rules */
            uni_vlan_cfg[uni_port].vlanRules.aggrN21_tables[j].aggregate_vlans[i].tag = 
                OAM_NTOHS(pVlanN21Table->vlanAggrArry[i].tag);
            uni_vlan_cfg[uni_port].vlanRules.aggrN21_tables[j].aggregate_vlans[i].type = 
                OAM_NTOHS(pVlanN21Table->vlanAggrArry[i].type);

            uni_vlanlist_cfg[uni_port].vlans[uni_vlanlist_cfg[uni_port].num++] = GET_VID(OAM_NTOHS(pVlanN21Table->vlanAggrArry[i].tag));
        }

        if (entries >= 1) {
            /* Need a rule for Reverse VLAN translation of broadcast. Use one of the aggregate VLANs for brodcast */
            vlanCtl_initTagRule();
            vlanCtl_setReceiveVlanDevice(eponAggVlanDevName);
            vlanCtl_filterOnTxVlanDevice(uniAggVlanDevName);
            vlanCtl_filterOnTagVid(GET_VID(OAM_NTOHS(pVlanN21Table->aggregatedVlan.tag)), 0);
            vlanCtl_filterOnRxRealDevice(eponRealIfname);
            vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_IS_UNICAST|BCM_VLAN_FILTER_FLAGS_IS_BROADCAST);
#if defined(CONFIG_BCM_VLAN_AGGREGATION)
            vlanCtl_cmdDeaggrVlanTag();
#else			
            vlanCtl_cmdSetTagVid(GET_VID(OAM_NTOHS(pVlanN21Table->vlanAggrArry[0].tag)), 0);
            vlanCtl_cmdSetTagPbits(GET_PBITS(OAM_NTOHS(pVlanN21Table->vlanAggrArry[i].tag)), 0);
#endif			
            num_tags = 1;
            vlanCtl_insertTagRule(realDevName, direction,
                num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
            save_rule(uni_port, realDevName, direction, num_tags, ruleId);
            num_tags = 2;
            vlanCtl_insertTagRule(realDevName, direction,
                num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
            save_rule(uni_port, realDevName, direction, num_tags, ruleId);
        }

        /* TBD: Change hard-coded values */
        pVlanN21Table = (CtcOamVlanN21Table *)(((U8 *)pVlanN21Table) + 6 + (4 * entries));
        vlanCtl_createVlanFlows(uniAggVlanDevName,eponAggVlanDevName);   
        vlanCtl_createVlanFlows(eponAggVlanDevName,uniAggVlanDevName);
    }
#else /* BCM_PON_XRDP */
    for (j=0; j < uni_vlan_cfg[uni_port].num_aggr_tables; j++) {     

        entries = OAM_NTOHS(pVlanN21Table->numAggrEntries);

        uni_vlan_cfg[uni_port].vlanRules.aggrN21_tables[j].num_aggr_entries = entries;
        uni_vlan_cfg[uni_port].vlanRules.aggrN21_tables[j].aggregatedVlan.tag = 
            OAM_NTOHS(pVlanN21Table->aggregatedVlan.tag);
        uni_vlan_cfg[uni_port].vlanRules.aggrN21_tables[j].aggregatedVlan.type = 
            OAM_NTOHS(pVlanN21Table->aggregatedVlan.type);

        for (i=0; i< entries; i++) {
            /* Translate the tag for US traffic from  ethx -> epon0  */
            vlanCtl_initTagRule();
            vlanCtl_setReceiveVlanDevice(vlanDevName);
            vlanCtl_filterOnTagVid(GET_VID(OAM_NTOHS(pVlanN21Table->vlanAggrArry[i].tag)), 0);
            vlanCtl_filterOnRxRealDevice(realDevName);
            vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_ALL);
            vlanCtl_cmdSetSkbMarkPort(Single_LLID_Flow);
            vlanCtl_cmdSetTagVid(GET_VID(OAM_NTOHS(pVlanN21Table->aggregatedVlan.tag)), 0);
            num_tags = 1;
            vlanCtl_insertTagRule(eponRealIfname, direction,
                num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
            save_rule(uni_port, eponRealIfname, direction, num_tags, ruleId);
            num_tags = 2;
            vlanCtl_insertTagRule(eponRealIfname, direction,
                num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
            save_rule(uni_port, eponRealIfname, direction, num_tags, ruleId);

            /* Reverse translation rule for DS is automatic in FAP. If needed in MIPS, 
                                  DA MAC filtering in vlanctl will have to be implemented */

            /* Save the aggregation rules */
            uni_vlan_cfg[uni_port].vlanRules.aggrN21_tables[j].aggregate_vlans[i].tag = 
                OAM_NTOHS(pVlanN21Table->vlanAggrArry[i].tag);
            uni_vlan_cfg[uni_port].vlanRules.aggrN21_tables[j].aggregate_vlans[i].type = 
                OAM_NTOHS(pVlanN21Table->vlanAggrArry[i].type);

            uni_vlanlist_cfg[uni_port].vlans[uni_vlanlist_cfg[uni_port].num++] = GET_VID(OAM_NTOHS(pVlanN21Table->vlanAggrArry[i].tag));
        }

        if (entries >= 1) {
            /* Need a rule for Reverse VLAN translation of broadcast. Use one of the aggregate VLANs for brodcast */
            vlanCtl_initTagRule();
            vlanCtl_setReceiveVlanDevice(vlanDevName);
            vlanCtl_filterOnTagVid(GET_VID(OAM_NTOHS(pVlanN21Table->aggregatedVlan.tag)), 0);
            vlanCtl_filterOnRxRealDevice(eponRealIfname);
            vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_IS_UNICAST|BCM_VLAN_FILTER_FLAGS_IS_BROADCAST );                
#if defined(CONFIG_BCM_VLAN_AGGREGATION)
            vlanCtl_cmdDeaggrVlanTag();
#else			
            vlanCtl_cmdSetTagVid(GET_VID(OAM_NTOHS(pVlanN21Table->vlanAggrArry[0].tag)), 0);
            vlanCtl_cmdSetTagPbits(GET_PBITS(OAM_NTOHS(pVlanN21Table->vlanAggrArry[i].tag)), 0);
#endif			
            num_tags = 1;
            vlanCtl_insertTagRule(realDevName, direction,
                num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
            save_rule(uni_port, realDevName, direction, num_tags, ruleId);
            num_tags = 2;
            vlanCtl_insertTagRule(realDevName, direction,
                num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
            save_rule(uni_port, realDevName, direction, num_tags, ruleId);
        }

        /* TBD: Change hard-coded values */
        pVlanN21Table = (CtcOamVlanN21Table *)(((U8 *)pVlanN21Table) + 6 + (4 * entries ));
    }
#endif /* BCM_PON_XRDP */
    /* Untag DS pkts with the default vlan tag */
    vlanCtl_initTagRule();
    vlanCtl_setReceiveVlanDevice(vlanDevName);
    vlanCtl_filterOnTagVid(GET_VID(OAM_NTOHS(pVlanN21XlateEntry->defaultVlan.tag)), 0);
    vlanCtl_filterOnRxRealDevice(eponRealIfname);
    vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_IS_UNICAST|BCM_VLAN_FILTER_FLAGS_IS_BROADCAST );    
    vlanCtl_cmdPopVlanTag();
    num_tags = 1;
    vlanCtl_insertTagRule(realDevName, direction,
       num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, realDevName, direction, num_tags, ruleId);
    num_tags = 2;
    vlanCtl_insertTagRule(realDevName, direction,
       num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, realDevName, direction, num_tags, ruleId);

    /* Discard US tagged pkt if VLAN is not in the aggregated list */
    vlanCtl_initTagRule();
    vlanCtl_filterOnTxVlanDevice(eponVlanIfname);
    vlanCtl_filterOnRxRealDevice(realDevName);
    vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_ALL);
    vlanCtl_cmdDropFrame();
    num_tags = 1;
    vlanCtl_insertTagRule(eponRealIfname, direction,
        num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, eponRealIfname, direction, num_tags, ruleId);
    num_tags = 2;
    vlanCtl_insertTagRule(eponRealIfname, direction,
        num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, eponRealIfname, direction, num_tags, ruleId);

    /* Discard DS untagged and vlan is not in the aggregated list */
    vlanCtl_initTagRule();
    vlanCtl_setReceiveVlanDevice(vlanDevName);
    vlanCtl_filterOnRxRealDevice(eponRealIfname);
    vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_IS_UNICAST|BCM_VLAN_FILTER_FLAGS_IS_BROADCAST );
    vlanCtl_cmdDropFrame();
    num_tags = 0;
    vlanCtl_insertTagRule(realDevName, direction,
       num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, realDevName, direction, num_tags, ruleId);
    num_tags = 1;
    vlanCtl_insertTagRule(realDevName, direction,
       num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, realDevName, direction, num_tags, ruleId);
    num_tags = 2;
    vlanCtl_insertTagRule(realDevName, direction,
       num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, realDevName, direction, num_tags, ruleId);
	
} // CtcVlanSetAggregation



////////////////////////////////////////////////////////////////////////////////
/// CtcVlanSetTrunk - Enable trunk mode
///
/// This function sets the current CTC VLAN mode to trunk.
///
 // Parameters:
/// \param port Port number, including EPON
///
/// \return
/// True if operation was successful
////////////////////////////////////////////////////////////////////////////////
static
void CtcVlanSetTrunk (TkOnuEthPort uni_port, 
                      CtcOamVlanTrunk *pVlanTrunkEntry, 
                      U16 size)
{
    U32 entries;
    unsigned int i;
    unsigned int num_tags, ruleId = 0;
    vlanCtl_direction_t direction = VLANCTL_DIRECTION_TX;
    char realDevName[16];
    char vlanDevName[16];
	
    /*  Hard coding the vlan interface name for now. TBD to get this name */
    snprintf(realDevName, sizeof(vlanDevName), "%s", uniRealIfname[uni_port]);
    snprintf(vlanDevName, sizeof(vlanDevName), "%s.0", realDevName);
    entries = (size / sizeof(EthernetVlanData)) - 1 ;
   

    /* Save the vlan tag mode info */
    uni_vlan_cfg[uni_port].default_tag_tpid = OAM_NTOHS(pVlanTrunkEntry->defaultVlan.type);
    uni_vlan_cfg[uni_port].default_tag_tci = OAM_NTOHS(pVlanTrunkEntry->defaultVlan.tag);
    uni_vlan_cfg[uni_port].vlanRules.trunk_rules.num_permitted_vlans = entries;

    /* Tag the upstream untagged. Allow tagged upstream only if VLAN is permitted */
    /* Discard DS untagged. Allow DS tagged if VLAN is permitted */

    /* Add the default tag for US untagged traffic (ethx -> epon0) */
    vlanCtl_initTagRule();
    vlanCtl_filterOnTxVlanDevice(eponVlanIfname);
    vlanCtl_filterOnRxRealDevice(realDevName);
    vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_ALL);
    vlanCtl_cmdPushVlanTag();
    vlanCtl_cmdSetSkbMarkPort(Single_LLID_Flow);
    vlanCtl_cmdSetTagVid(GET_VID(OAM_NTOHS(pVlanTrunkEntry->defaultVlan.tag)), 0);
    vlanCtl_cmdSetTagPbits(GET_PBITS(OAM_NTOHS(pVlanTrunkEntry->defaultVlan.tag)), 0);
    vlanCtl_cmdSetEtherType(EthertypeCvlan);
    num_tags = 0;
    vlanCtl_insertTagRule(eponRealIfname, direction,
        num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, eponRealIfname, direction, num_tags, ruleId);

    /* Set the VLAN tag for upstream p-tagged packets */
    vlanCtl_initTagRule();
    vlanCtl_filterOnTxVlanDevice(eponVlanIfname);
    vlanCtl_filterOnTagVid(0, 0);
    vlanCtl_filterOnRxRealDevice(realDevName);
    vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_ALL);
    vlanCtl_cmdSetSkbMarkPort(Single_LLID_Flow);
    vlanCtl_cmdSetTagVid(GET_VID(OAM_NTOHS(pVlanTrunkEntry->defaultVlan.tag)), 0);
    vlanCtl_cmdSetTagPbits(GET_PBITS(OAM_NTOHS(pVlanTrunkEntry->defaultVlan.tag)), 0);
    num_tags = 1;
    vlanCtl_insertTagRule(eponRealIfname, direction, num_tags,
        VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, eponRealIfname, direction, num_tags, ruleId);

    uni_vlanlist_cfg[uni_port].vlans[uni_vlanlist_cfg[uni_port].num++] = 0;


    /* Pop tag for DS default tag traffic (epon0 -> ethx) */
    vlanCtl_initTagRule();
    vlanCtl_setReceiveVlanDevice(vlanDevName);
    vlanCtl_filterOnTagVid(GET_VID(OAM_NTOHS(pVlanTrunkEntry->defaultVlan.tag)), 0);
    vlanCtl_filterOnRxRealDevice(eponRealIfname);
    vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_IS_UNICAST|BCM_VLAN_FILTER_FLAGS_IS_BROADCAST );    
    vlanCtl_cmdPopVlanTag();
    num_tags = 1;
    vlanCtl_insertTagRule(realDevName, direction,
       num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, realDevName, direction, num_tags, ruleId);
    num_tags = 2;
    vlanCtl_insertTagRule(realDevName, direction,
       num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, realDevName, direction, num_tags, ruleId);

    for (i=0; i< entries; i++) {
        /* Allow US tagged only of VLAN is permitted  */
        vlanCtl_initTagRule();
        vlanCtl_setReceiveVlanDevice(eponVlanIfname);
        vlanCtl_filterOnTagVid(GET_VID(OAM_NTOHS(pVlanTrunkEntry->vlanTrunkArry[i].tag)), 0);
        vlanCtl_filterOnRxRealDevice(realDevName);
        vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_ALL);
        vlanCtl_cmdSetSkbMarkPort(Single_LLID_Flow);
        num_tags = 1;
        vlanCtl_insertTagRule(eponRealIfname, direction,
            num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
        save_rule(uni_port, eponRealIfname, direction, num_tags, ruleId);
        num_tags = 2;
        vlanCtl_insertTagRule(eponRealIfname, direction,
            num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
        save_rule(uni_port, eponRealIfname, direction, num_tags, ruleId);

        vlanCtl_initTagRule();
        vlanCtl_setReceiveVlanDevice(vlanDevName);
        vlanCtl_filterOnTagVid(GET_VID(OAM_NTOHS(pVlanTrunkEntry->vlanTrunkArry[i].tag)), 0);
        vlanCtl_filterOnRxRealDevice(eponRealIfname);
        vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_IS_UNICAST|BCM_VLAN_FILTER_FLAGS_IS_BROADCAST );    
        num_tags = 1;
        vlanCtl_insertTagRule(realDevName, direction,
            num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
        save_rule(uni_port, realDevName, direction, num_tags, ruleId);
        num_tags = 2;
        vlanCtl_insertTagRule(realDevName, direction,
            num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
        save_rule(uni_port, realDevName,direction, num_tags, ruleId);
        uni_vlan_cfg[uni_port].vlanRules.trunk_rules.permitted_vlans[i].tag = OAM_NTOHS(pVlanTrunkEntry->vlanTrunkArry[i].tag);
        uni_vlan_cfg[uni_port].vlanRules.trunk_rules.permitted_vlans[i].type = OAM_NTOHS(pVlanTrunkEntry->vlanTrunkArry[i].type);

        uni_vlanlist_cfg[uni_port].vlans[uni_vlanlist_cfg[uni_port].num++] = GET_VID(OAM_NTOHS(pVlanTrunkEntry->vlanTrunkArry[i].tag));
    }

    /* for US tagged, discard traffic if the VLAN is not permitted */
    vlanCtl_initTagRule();
    vlanCtl_filterOnTxVlanDevice(eponVlanIfname);
    vlanCtl_filterOnRxRealDevice(realDevName);
    vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_ALL);
    vlanCtl_cmdDropFrame();
    num_tags = 1;
    vlanCtl_insertTagRule(eponRealIfname, direction,
        num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, eponRealIfname, direction, num_tags, ruleId);
    num_tags = 2;
    vlanCtl_insertTagRule(eponRealIfname, direction,
        num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, eponRealIfname, direction, num_tags, ruleId);	

    /* Discard DS untagged.  */
    vlanCtl_initTagRule();
    vlanCtl_setReceiveVlanDevice(vlanDevName);
    vlanCtl_filterOnRxRealDevice(eponRealIfname);
    vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_IS_UNICAST|BCM_VLAN_FILTER_FLAGS_IS_BROADCAST );    
    vlanCtl_cmdDropFrame();
    num_tags = 0;
    vlanCtl_insertTagRule(realDevName, direction,
       num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, realDevName, direction, num_tags, ruleId);
    num_tags = 1;
    vlanCtl_insertTagRule(realDevName, direction,
       num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, realDevName, direction, num_tags, ruleId);
    num_tags = 2;
    vlanCtl_insertTagRule(realDevName, direction,
       num_tags, VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &ruleId);
    save_rule(uni_port, realDevName, direction, num_tags, ruleId);
	
    } // CtcVlanSetTrunk
#endif


////////////////////////////////////////////////////////////////////////////////
/// CtcVlanTranslateCfgCheck - Check if the CTC VLAN Translate config is valid
/// 
 // Parameters:
/// \param vlan CTC VLAN translation mode data
/// \param size Size of optional data in bytes
///
/// \return
/// TRUE if configuration is valid, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
static
BOOL CtcVlanTranslateCfgCheck (EthernetVlanData   *provData, U16 size)
{
	U8					  i;
	U8					  j;
	U8					  entries;
	CtcVlanTranslatate		  *vlanData;
	EthernetVlanData 		  *DefaultVlan;

 	entries = ((size / sizeof(EthernetVlanData)) - 1) / 2;
	vlanData = (CtcVlanTranslatate   *) &(provData[1]);

	DefaultVlan = (EthernetVlanData   *)&(provData[0]);
	if(OAM_NTOHS(DefaultVlan->type) == 0 && OAM_NTOHS(DefaultVlan->tag) == 0)
	{
		// bad oam 
		return FALSE;
	}

    
        if (size != (sizeof (EthernetVlanData) +
                        (entries * 2 * sizeof(EthernetVlanData))))
        {
            // Malformed VLAN Translate request
            return FALSE;
        }

	if (!entries)
	{
		return TRUE;
	}

	
	// Weed out duplicate Input VLAN configuration
	for (i = 0; i < (entries - 1); ++i)
	{
		for (j = (i + 1); j < entries; ++j)
		{
			if (OAM_NTOHS(vlanData[i].fromVid.tag) == OAM_NTOHS(vlanData[j].fromVid.tag))
			{
				return FALSE;
			}
		}
	}

	return TRUE;
} // CtcVlanTranslateCfgCheck


 ////////////////////////////////////////////////////////////////////////////////
/// CtcVlanAggregationCfgCheck - Check if the CTC VLAN aggregation config is valid
/// 
 // Parameters:
/// \param vlan CTC VLAN translation mode data
/// \param size Size of optional data in bytes
///
/// \return
/// TRUE if configuration is valid, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
static
BOOL CtcVlanAggregationCfgCheck (U8  const * provData, U16 size)
{
	U8   i;
	U8   j;
	U8   Cvidnum = 0;
	CtcOamVlanN21Table   *vlanData;
	CtcOamVlanN21Translate   *OamCtcVlanAgg = 
			(CtcOamVlanN21Translate   *)provData;
 

    //check default VLAN
        if (size  < sizeof(EthernetVlanData) + sizeof(U16))
        {
            return FALSE;
        }

	//check group number
	if (OAM_NTOHS(OamCtcVlanAgg->numVlanAggrTables) >  MAX_PERMITTED_AG_GROUP)
	{
		return FALSE;
	}

    vlanData = (CtcOamVlanN21Table  *)(OamCtcVlanAgg->vlanAggrTable );
	for (i = 0;i < OAM_NTOHS(OamCtcVlanAgg->numVlanAggrTables);i++)
	{
 		Cvidnum += OAM_NTOHS(vlanData->numAggrEntries);
		//check per group entry number and SVID if valid
		if ((OAM_NTOHS(vlanData->numAggrEntries) > MAX_PERMITTED_VLANS) || 
				(OAM_NTOHS(vlanData->aggregatedVlan.type) != EthertypeCvlan))
		{
			return FALSE;
		}
		for (j = 0;j < OAM_NTOHS(vlanData->numAggrEntries);j++)
		{
			//check entry CVID if valid
			if (OAM_NTOHS(vlanData->vlanAggrArry[j].type) != EthertypeCvlan)
			{
				return FALSE;
			}
		}
        
		vlanData =   (CtcOamVlanN21Table  *)((U8   *)vlanData + sizeof(U16) + (sizeof(EthernetVlanData) *
										(OAM_NTOHS(vlanData->numAggrEntries) +1)));
	}
	//all CVID can not more than MAX_PERMITTED_VLANS
	if (Cvidnum > MAX_PERMITTED_VLANS)
	{
		return FALSE;
	}
       return TRUE;
}//CtcVlanAggregationCfgCheck


 
////////////////////////////////////////////////////////////////////////////////
/// CtcVlanTrunkCheck - Check if the CTC VLAN Trunk config is valid
/// 
 // Parameters:
/// \param size Size of optional data in bytes
///
/// \return
/// TRUE if configuration is valid, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
static
BOOL CtcVlanTrunkCheck (U16 size)
{
    U8  entries;

    entries = (size / sizeof(EthernetVlanData)) - 1 ;

    if (size != (sizeof (EthernetVlanData) +
        (entries * sizeof(EthernetVlanData))))
    {
    // Malformed VLAN Trunk request
        return FALSE;
    }
    if (entries > MAX_PERMITTED_VLANS) {
        cmsLog_error("entries (%d) exceeds MAX_PERMITTED_VLANS",entries);     
        return FALSE;
    }

    return TRUE;
}//CtcVlanTrunkCheck


#if defined(EPON_SFU) && defined(BCM_PON_XRDP)
#define BR_AGGR_PREFIX "br_aggr"
static
void CtcVlanAggrCheck(TkOnuEthPort uni_port,
                     OamCtcVlanMode old_mode,
                     const U8 BULK *provData)
{
    char uniAggVlanDevName[16];
    char eponAggVlanDevName[16];
    unsigned int i;

    if (old_mode == OamCtcVlanAggregation)
    {
        for (i = 0; i < uni_vlan_cfg[uni_port].num_aggr_tables; i++) {     
            snprintf(uniAggVlanDevName, sizeof(uniAggVlanDevName), "%s.1.%d", uniRealIfname[uni_port],
                     uni_vlan_cfg[uni_port].vlanRules.aggrN21_tables[i].aggregatedVlan.tag);
            snprintf(eponAggVlanDevName, sizeof(eponAggVlanDevName), "%s.2.%d", eponRealIfname,
                     uni_vlan_cfg[uni_port].vlanRules.aggrN21_tables[i].aggregatedVlan.tag);
            vlanCtl_init();
            vlanCtl_deleteVlanFlows(uniAggVlanDevName, eponAggVlanDevName);
            vlanCtl_deleteVlanFlows(eponAggVlanDevName, uniAggVlanDevName);
            vlanCtl_cleanup();
        }
    }

    vlanCtl_init();
    /* Remove the previous VLAN rules */
    for (i = 0; i < uni_vlan_cfg[uni_port].num_vlanctl_rules; i++) {
        vlanCtl_removeTagRule((char *)uni_vlan_cfg[uni_port].vlanctl_rules[i].devName, uni_vlan_cfg[uni_port].vlanctl_rules[i].dir,
            uni_vlan_cfg[uni_port].vlanctl_rules[i].nbrOfTags, uni_vlan_cfg[uni_port].vlanctl_rules[i].tagRuleId);
    }
    uni_vlan_cfg[uni_port].num_vlanctl_rules = 0;

    if (old_mode == OamCtcVlanAggregation)
    {
        char vlanDevName[16];
        snprintf(vlanDevName, sizeof(vlanDevName), "%s.0", uniRealIfname[uni_port]);
        for (i = 0; i < uni_vlan_cfg[uni_port].num_aggr_tables; i++)
        {
            char cmdStr[64];
            int j;
            int br_delete;

            snprintf(uniAggVlanDevName, sizeof(uniAggVlanDevName), "%s.1.%d", uniRealIfname[uni_port],
                     uni_vlan_cfg[uni_port].vlanRules.aggrN21_tables[i].aggregatedVlan.tag);
            snprintf(eponAggVlanDevName, sizeof(eponAggVlanDevName), "%s.2.%d", eponRealIfname,
                     uni_vlan_cfg[uni_port].vlanRules.aggrN21_tables[i].aggregatedVlan.tag);

            snprintf(cmdStr, sizeof(cmdStr), "ifconfig %s down", uniAggVlanDevName);
            cmsLog_debug("%s", cmdStr);
            system(cmdStr);

            snprintf(cmdStr, sizeof(cmdStr), "brctl delif %s.%d %s", BR_AGGR_PREFIX, 
                     uni_vlan_cfg[uni_port].vlanRules.aggrN21_tables[i].aggregatedVlan.tag, uniAggVlanDevName);
            cmsLog_debug("%s", cmdStr);
            system(cmdStr);

            for (j = 0, br_delete = 1; br_delete && (j < MAX_UNI_PORTS); j++)
            {
                int k;
                for (k = 0; br_delete && (k < uni_vlan_cfg[j].num_aggr_tables); k++)
                {
                    if ((uni_vlan_cfg[uni_port].vlanRules.aggrN21_tables[i].aggregatedVlan.tag ==
                        uni_vlan_cfg[j].vlanRules.aggrN21_tables[k].aggregatedVlan.tag)
                        && (uni_port != j))
                    {
                        br_delete = 0;
                    }
                }
            }
            if (br_delete)
            {
                snprintf(cmdStr, sizeof(cmdStr), "ifconfig %s down", eponAggVlanDevName);
                cmsLog_debug("%s", cmdStr);
                system(cmdStr);

                snprintf(cmdStr, sizeof(cmdStr), "brctl delif %s.%d %s", BR_AGGR_PREFIX, 
                         uni_vlan_cfg[uni_port].vlanRules.aggrN21_tables[i].aggregatedVlan.tag, eponAggVlanDevName);
                cmsLog_debug("%s", cmdStr);
                system(cmdStr);

                snprintf(cmdStr, sizeof(cmdStr), "ifconfig %s.%d down", BR_AGGR_PREFIX,
                         uni_vlan_cfg[uni_port].vlanRules.aggrN21_tables[i].aggregatedVlan.tag);
                cmsLog_debug("%s", cmdStr);
                system(cmdStr);

                snprintf(cmdStr, sizeof(cmdStr), "brctl delbr %s.%d", BR_AGGR_PREFIX,
                         uni_vlan_cfg[uni_port].vlanRules.aggrN21_tables[i].aggregatedVlan.tag);
                cmsLog_debug("%s", cmdStr);
                system(cmdStr);

                vlanCtl_deleteVlanInterface(eponAggVlanDevName);
            }
            vlanCtl_deleteVlanInterface(uniAggVlanDevName);
        }
        vlanCtl_setDefaultAction(uniRealIfname[uni_port], VLANCTL_DIRECTION_RX, 1, VLANCTL_ACTION_ACCEPT, vlanDevName);
        vlanCtl_setDefaultAction(uniRealIfname[uni_port], VLANCTL_DIRECTION_RX, 2, VLANCTL_ACTION_ACCEPT, vlanDevName);
        uni_vlan_cfg[uni_port].num_aggr_tables = 0;
    }
}


static
void CtcVlanAggrBrAdd(TkOnuEthPort uni_port, const U8 BULK *provData)
{
    CtcOamVlanN21Translate *pVlanN21XlateEntry = (CtcOamVlanN21Translate *)provData;
    CtcOamVlanN21Table *pVlanN21Table = (CtcOamVlanN21Table *)pVlanN21XlateEntry->vlanAggrTable;
    U8 uniAggVlanDevName[16];
    U8 eponAggVlanDevName[16];
    U8 vlanDevName[16];
    U8 cmdStr[64];
    U32 entries;
    int i,j,br_add;
	    
    snprintf(vlanDevName, sizeof(vlanDevName), "%s.0", uniRealIfname[uni_port]);

    for (i = 0; i < OAM_NTOHS(((CtcOamVlanN21Translate *)provData)->numVlanAggrTables); i++)
    {
        entries = OAM_NTOHS(pVlanN21Table->numAggrEntries);
        snprintf(uniAggVlanDevName, sizeof(uniAggVlanDevName), "%s.1.%d", uniRealIfname[uni_port], 
                 GET_VID(OAM_NTOHS(pVlanN21Table->aggregatedVlan.tag)));
        snprintf(eponAggVlanDevName, sizeof(eponAggVlanDevName), "%s.2.%d", eponRealIfname,
                 GET_VID(OAM_NTOHS(pVlanN21Table->aggregatedVlan.tag)));

        for (j = 0, br_add = 1; br_add && (j < MAX_UNI_PORTS); j++)
        {
            int k;
            for (k = 0; br_add && (k < uni_vlan_cfg[j].num_aggr_tables); k++)
            {
                if ((GET_VID(OAM_NTOHS(pVlanN21Table->aggregatedVlan.tag)) ==
                    uni_vlan_cfg[j].vlanRules.aggrN21_tables[k].aggregatedVlan.tag)
                    && (uni_port != j))
                {
                    br_add = 0;
                }
            }
        }
        
        if (br_add)
        {
            snprintf(cmdStr, sizeof(cmdStr), "brctl addbr %s.%d", BR_AGGR_PREFIX, 
                     GET_VID(OAM_NTOHS(pVlanN21Table->aggregatedVlan.tag)));
            cmsLog_debug("%s", cmdStr);
            system(cmdStr);

            snprintf(cmdStr, sizeof(cmdStr), "brctl addif %s.%d %s", BR_AGGR_PREFIX,
                     GET_VID(OAM_NTOHS(pVlanN21Table->aggregatedVlan.tag)), eponAggVlanDevName);
            cmsLog_debug("%s", cmdStr);
            system(cmdStr);

            snprintf(cmdStr, sizeof(cmdStr), "ifconfig %s up", eponAggVlanDevName);
            cmsLog_debug("%s", cmdStr);
            system(cmdStr);
        }

        snprintf(cmdStr, sizeof(cmdStr), "brctl addif %s.%d %s", BR_AGGR_PREFIX,
                 GET_VID(OAM_NTOHS(pVlanN21Table->aggregatedVlan.tag)), uniAggVlanDevName);
        cmsLog_debug("%s", cmdStr);
        system(cmdStr);

        snprintf(cmdStr, sizeof(cmdStr), "ifconfig %s up", uniAggVlanDevName);
        cmsLog_debug("%s", cmdStr);
        system(cmdStr);

        snprintf(cmdStr, sizeof(cmdStr), "ifconfig %s.%d up", BR_AGGR_PREFIX,
                 GET_VID(OAM_NTOHS(pVlanN21Table->aggregatedVlan.tag)));
        cmsLog_debug("%s", cmdStr);
        system(cmdStr);

        pVlanN21Table = (CtcOamVlanN21Table *)(((U8 *)pVlanN21Table) + 6 + (4 * entries ));
    }
}
#endif

BOOL CtcVlanSetMode (TkOnuEthPort uni_port,
                     OamCtcVlanMode mode,
                     const U8 BULK *provData,
                     U16 size)
{
#if defined(EPON_SFU) && defined(BCM_PON_XRDP)
    OamCtcVlanMode old_mode = CtcVlanGetMode(uni_port);
#endif

    if ((uni_port >= UniCfgDbGetActivePortCount()))
    {
        cmsLog_error("Invalid port: Given uni_port = %d", uni_port);
        return FALSE;
    }

    if (uniRealIfname[uni_port][0] == '\0')
    {
        cmsLog_error("Invalid port name: Given uni_port = %d", uni_port);
        return FALSE;
    }
	
    if (mode > OamCtcVlanInit)
    {
        cmsLog_error("Invalid Vlan Mode = %d", mode);
        return FALSE;
    }

    cmsLog_debug("uni_port = %d, mode = %d, size = 0x%x", uni_port, mode, size);


    
    //check parameter 
    if ((mode == OamCtcVlanTranslation) &&
        (((size / sizeof(EthernetVlanData)) > (MAX_PERMITTED_VLANS*2 + 1)) ||
        (!CtcVlanTranslateCfgCheck ((EthernetVlanData BULK *)provData, size-1))))
    {
        return FALSE;
    }
    else if ((mode == OamCtcVlanAggregation) && 
        (!CtcVlanAggregationCfgCheck(provData,size-1)))
    {
        return FALSE;
    }
    else if ((mode == OamCtcVlanTrunk) && (!CtcVlanTrunkCheck(size-1)))
    {
        return FALSE;
    }
    else if ((mode == OamCtcVlanTransparent) && (size != 1))
    {
        return FALSE;
    }
    else if ((mode == OamCtcVlanTag) && (size != 1 + sizeof(EthernetVlanData)))
    {
        return FALSE;
    }
    else
    {}

    //Return directly if configuration is the same as existing one
    {
        U32 len = 0;
        unsigned char tmp[OamMaxTlvLength];
        getCtcVlanData(uni_port,tmp, &len);
        
        if((mode == CtcVlanGetMode(uni_port))
            &&(len == size)
            &&((OamCtcVlanInit == mode)||
                (OamCtcVlanTransparent == mode)||
                (!memcmp(tmp,provData,len))))
            {
                return TRUE;
            }
    }


    /* Set the new mode */
    uni_vlan_cfg[uni_port].mode = (mode == OamCtcVlanInit ? OamCtcVlanTransparent : mode);

    CtcVlanListCfg(uni_port,FALSE);
    memset (&uni_vlanlist_cfg[uni_port], 0, sizeof(CtcVlanList));
    
#ifdef EPON_SFU	
    //Clear old flows	
    {
        char cmdStr[64];
        snprintf(cmdStr, sizeof(cmdStr), "vlanctl --delete-flows %s.0 %s", uniRealIfname[uni_port], eponVlanIfname);
        cmsLog_debug("%s", cmdStr);
        system(cmdStr);
        snprintf(cmdStr, sizeof(cmdStr), "vlanctl --delete-flows %s %s.0", eponVlanIfname, uniRealIfname[uni_port]);
        cmsLog_debug("%s", cmdStr);
        system(cmdStr);
    }

#if defined(BCM_PON_XRDP)
    CtcVlanAggrCheck(uni_port, old_mode, provData);
#else
    {
        unsigned int i;
        vlanCtl_init();
        /* Remove the previous VLAN rules */
        for (i = 0; i < uni_vlan_cfg[uni_port].num_vlanctl_rules; i++) {
            vlanCtl_removeTagRule((char *)uni_vlan_cfg[uni_port].vlanctl_rules[i].devName, uni_vlan_cfg[uni_port].vlanctl_rules[i].dir, 
                uni_vlan_cfg[uni_port].vlanctl_rules[i].nbrOfTags, uni_vlan_cfg[uni_port].vlanctl_rules[i].tagRuleId);
        }
        uni_vlan_cfg[uni_port].num_vlanctl_rules = 0;
     }
#endif
    /* Set the new mode and rules */
    switch (mode) {
        case OamCtcVlanInit:        
            memset (&(uni_vlan_cfg[uni_port]), 0, sizeof(CtcVlanRules));
            uni_vlan_cfg[uni_port].mode = OamCtcVlanInit;
            break;
			
        case OamCtcVlanTransparent:        
            CtcVlanSetTransparent(uni_port);
            break;

        case OamCtcVlanTag:
            CtcVlanSetTag(uni_port, (CtcOamVlanTag *)provData);
            break;

        case OamCtcVlanTranslation:
            CtcVlanSetTranslation(uni_port, 
                (CtcOamVlanTranslate *)provData, size);
            break;

        case OamCtcVlanAggregation:
            CtcVlanSetAggregation(uni_port, (CtcOamVlanN21Translate *)provData);
            break;

        case OamCtcVlanTrunk:
            CtcVlanSetTrunk(uni_port, (CtcOamVlanTrunk *)provData, size);
            break;
        default:
            cmsLog_error("unkown vlan mode 0x%x", mode);
            break;
    }
	
    vlanCtl_cleanup();

    /* Apply the new LAN-WAN rules*/   
    {
        char cmdStr[64];
        snprintf(cmdStr, sizeof(cmdStr), "vlanctl --create-flows %s.0 %s", uniRealIfname[uni_port], eponVlanIfname);
        cmsLog_debug("%s", cmdStr);
        system(cmdStr);
        snprintf(cmdStr, sizeof(cmdStr), "vlanctl --create-flows %s %s.0", eponVlanIfname, uniRealIfname[uni_port]);
        cmsLog_debug("%s", cmdStr);
        system(cmdStr);
    }

#if defined(BCM_PON_XRDP)
    if (mode == OamCtcVlanAggregation)
        CtcVlanAggrBrAdd(uni_port, provData);
#endif
    
#endif

    if(mode == OamCtcVlanTransparent)
        bcm_port_transparent_set(0,bcm_enet_map_oam_idx_to_phys_port(uni_port),1);
    else
        bcm_port_transparent_set(0,bcm_enet_map_oam_idx_to_phys_port(uni_port),0);

    bcm_port_vlan_isolation_set(0,bcm_enet_map_oam_idx_to_phys_port(uni_port),0,0);

    //update VLAN list
    CtcVlanListCfg(uni_port,TRUE);

    //flush existing MAC address
    {
        br_util_flush_fdb("br0", uniRealIfname[uni_port], BR_UTIL_FDB_TYPE_DYNAMIC);
    }


    return TRUE;
}




static void getCtcVlanData(int port, U8 BULK *vlan, U32 *pOutLen)
{
    unsigned int size = 0, i, j;
    CtcOamVlanEntry *pVlan = (CtcOamVlanEntry *)vlan;

    size = sizeof(uni_vlan_cfg[port].mode);

    switch (uni_vlan_cfg[port].mode) {
        case OamCtcVlanTag:
            {
                EthernetVlanData *pTag = (EthernetVlanData *)pVlan->CtcOamVlanData;

                pTag->type = OAM_HTONS(uni_vlan_cfg[port].default_tag_tpid);
                pTag->tag = OAM_HTONS(uni_vlan_cfg[port].default_tag_tci);

                size += sizeof(EthernetVlanData);
            }
            break;

        case OamCtcVlanTranslation:
            {
                CtcOamVlanTranslate *pVlanXlateEntry = (CtcOamVlanTranslate *)pVlan->CtcOamVlanData;

                pVlanXlateEntry->defaultVlan.type = OAM_HTONS(uni_vlan_cfg[port].default_tag_tpid);
                pVlanXlateEntry->defaultVlan.tag = OAM_HTONS(uni_vlan_cfg[port].default_tag_tci);
                size += sizeof(EthernetVlanData);

                //printf("Number of Entries = %d \n", (unsigned int)uni_vlan_cfg[port].vlanRules.xlate_tables.num_translate_entries);
                for (i=0; i < uni_vlan_cfg[port].vlanRules.xlate_tables.num_translate_entries; i++) {
                    pVlanXlateEntry->vlanTranslateArry[i].fromVid.tag = 
                        OAM_HTONS(uni_vlan_cfg[port].vlanRules.xlate_tables.xlate_entries[i].fromVid.tag);
                    pVlanXlateEntry->vlanTranslateArry[i].fromVid.type = 
                        OAM_HTONS(uni_vlan_cfg[port].vlanRules.xlate_tables.xlate_entries[i].fromVid.type);
                    pVlanXlateEntry->vlanTranslateArry[i].toVid.tag = 
                        OAM_HTONS(uni_vlan_cfg[port].vlanRules.xlate_tables.xlate_entries[i].toVid.tag);
                    pVlanXlateEntry->vlanTranslateArry[i].toVid.type = 
                        OAM_HTONS(uni_vlan_cfg[port].vlanRules.xlate_tables.xlate_entries[i].toVid.type);
                }
                size += (sizeof(CtcVlanTranslatate) 
                    * uni_vlan_cfg[port].vlanRules.xlate_tables.num_translate_entries);
            }
            break;

        case OamCtcVlanAggregation:
            {
                CtcOamVlanN21Translate *pVlanN21XlateEntry = (CtcOamVlanN21Translate *)pVlan->CtcOamVlanData;
                CtcOamVlanN21Table *pVlanN21Table = (CtcOamVlanN21Table *)pVlanN21XlateEntry->vlanAggrTable;
                unsigned int tmpSize;

                pVlanN21XlateEntry->numVlanAggrTables = OAM_HTONS((U16)uni_vlan_cfg[port].num_aggr_tables);
                size += sizeof(pVlanN21XlateEntry->numVlanAggrTables);

                pVlanN21XlateEntry->defaultVlan.type = OAM_HTONS(uni_vlan_cfg[port].default_tag_tpid);
                pVlanN21XlateEntry->defaultVlan.tag = OAM_HTONS(uni_vlan_cfg[port].default_tag_tci);
                size += sizeof(EthernetVlanData);

                //printf("Number of Tables = %d \n", (unsigned int)uni_vlan_cfg[port].num_aggr_tables);
                for (j=0; j < uni_vlan_cfg[port].num_aggr_tables; j++) {   
                    tmpSize = 0;
                    pVlanN21Table->numAggrEntries = OAM_HTONS((U16)uni_vlan_cfg[port].vlanRules.aggrN21_tables[j].num_aggr_entries);
                    tmpSize += sizeof(pVlanN21Table->numAggrEntries);

                    pVlanN21Table->aggregatedVlan.tag =  
                        OAM_HTONS(uni_vlan_cfg[port].vlanRules.aggrN21_tables[j].aggregatedVlan.tag);
                    pVlanN21Table->aggregatedVlan.type =  
                        OAM_HTONS(uni_vlan_cfg[port].vlanRules.aggrN21_tables[j].aggregatedVlan.type);
                    tmpSize += sizeof(EthernetVlanData);

                    //printf("Number of Entries = %d \n", (unsigned int)pVlanN21Table->numAggrEntries);
                    for (i = 0; i < OAM_NTOHS(pVlanN21Table->numAggrEntries); i++) {
                        pVlanN21Table->vlanAggrArry[i].tag = 
                            OAM_HTONS(uni_vlan_cfg[port].vlanRules.aggrN21_tables[j].aggregate_vlans[i].tag);
                        pVlanN21Table->vlanAggrArry[i].type = 
                            OAM_HTONS(uni_vlan_cfg[port].vlanRules.aggrN21_tables[j].aggregate_vlans[i].type);
                    }
                    tmpSize += (sizeof(EthernetVlanData) * OAM_NTOHS(pVlanN21Table->numAggrEntries));
                    pVlanN21Table = (CtcOamVlanN21Table *)(((U8 *)pVlanN21Table) + tmpSize);
                    size += tmpSize;
                }
            }
            break;

        case OamCtcVlanTrunk:
            {
                CtcOamVlanTrunk *pVlanTrunkEntry = (CtcOamVlanTrunk *)pVlan->CtcOamVlanData;
                EthernetVlanData *pVlanTrunkTable = (EthernetVlanData *)pVlanTrunkEntry->vlanTrunkArry;

                pVlanTrunkEntry->defaultVlan.type = OAM_HTONS(uni_vlan_cfg[port].default_tag_tpid);
                pVlanTrunkEntry->defaultVlan.tag = OAM_HTONS(uni_vlan_cfg[port].default_tag_tci);
                size += sizeof(EthernetVlanData);

                for (i = 0; i < uni_vlan_cfg[port].vlanRules.trunk_rules.num_permitted_vlans; i++) {
                    pVlanTrunkTable[i].type = OAM_HTONS(uni_vlan_cfg[port].vlanRules.trunk_rules.permitted_vlans[i].type);
                    pVlanTrunkTable[i].tag = OAM_HTONS(uni_vlan_cfg[port].vlanRules.trunk_rules.permitted_vlans[i].tag);
                    size += sizeof(EthernetVlanData);
                }
            }
            break;

        default:
            break;
    }

    *pOutLen = size;
	
    return;
}

////////////////////////////////////////////////////////////////////////////////
/// CtcVlanGetMode - Get the currently provisioned VLAN mode
///
/// This function returns the currently provisioned CTC VLAN mode.
///
 // Parameters:
/// \param port Port number, including EPON
///
/// \return
/// Active VLAN mode
////////////////////////////////////////////////////////////////////////////////
//extern
OamCtcVlanMode CtcVlanGetMode (TkOnuEthPort uni_port)
    {
    cmsLog_debug("mode = %d", uni_vlan_cfg[uni_port].mode);
    return uni_vlan_cfg[uni_port].mode;
    } // CtcVlanGetMode


U16 CtcVlanGetData (TkOnuEthPort port, U8 BULK *vlan)
    {
       cmsLog_debug("port = %d", port);
       U32 pOutLen = 0;

        getCtcVlanData(port, vlan, &pOutLen);
        cmsLog_debug("pOutLen = %d", pOutLen);
        return pOutLen;
    }

////////////////////////////////////////////////////////////////////////////////
/// CtcVlanInit - Initialize CTC VLAN mode
///
/// This function starts up the CTC VLAN provisioning if there are CTC Port
/// VLAN records in NVS.
///
 // Parameters:
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void CtcVlanInit (void)
    {
    int i;
    cmsLog_debug("enter");
    memset (uni_vlan_cfg, 0, sizeof(uni_vlan_cfg));
    for(i=0; i< UniCfgDbGetActivePortCount();i++)
        uni_vlan_cfg[i].mode = OamCtcVlanInit;
    } // CtcVlanInit
    
// End of File CtcVlan.c


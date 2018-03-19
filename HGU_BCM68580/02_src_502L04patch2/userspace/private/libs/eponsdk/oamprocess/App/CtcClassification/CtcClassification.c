/*
 *<:copyright-BRCM:2013:proprietary:epon
 *
 *   Copyright (c) 2013 Broadcom 
 *   All Rights Reserved
 *
 * This program is the proprietary software of Broadcom and/or its
 * licensors, and may only be used, duplicated, modified or distributed pursuant
 * to the terms and conditions of a separate, written license agreement executed
 * between you and Broadcom (an "Authorized License").  Except as set forth in
 * an Authorized License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and Broadcom
 * expressly reserves all rights in and to the Software and all intellectual
 * property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 * NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 * BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1. This program, including its structure, sequence and organization,
 *    constitutes the valuable trade secrets of Broadcom, and you shall use
 *    all reasonable efforts to protect the confidentiality thereof, and to
 *    use this information only in connection with your use of Broadcom
 *    integrated circuit products.
 *
 * 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
 *    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
 *    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
 *    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
 *    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
 *    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
 *    PERFORMANCE OF THE SOFTWARE.
 *
 * 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
 *    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
 *    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
 *    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 *    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
 *    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
 *    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
 *    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
 *    LIMITED REMEDY.
 * :>
 */
////////////////////////////////////////////////////////////////////////////////
/// \file CtcClassification.c
/// \brief China Telecom classification module
///
/// Implement for SFU, and MDU etc.
///
/// For SFU, ONU can do it individually.
///
/// For MDU, it needs to be implemented in the ONU and the switch.
///     When external CPU receives the CTC Classification OAM, it will divide
///     the ctc classification into two parts, one for switch, and another
///     for ONU.
///
///     As ONU can't see the UNIs in the switch and can't delete first vlan and
///     modify the second vlan. External CPU need to add one VLAN and modify the
///     original VLAN's cos(if necessary), then create a new ctc classification
///     rules for ONU based on the what it did on the original packages.
///
///     In order to make the CTC classification interfaces unique, external CPU
///     must use CTC classification format to configure ONU.
///
///     ONU provides APIs for external CPU to enable/disable the VLAN strip, and
///     get the state of the VLAN strip.
///
/// Note: this strip control variable will be global for all ports(ONUs), not a
///       single port, rule.
////////////////////////////////////////////////////////////////////////////////

//generic includes
#include "cms_util.h"
#include "rdpactl_api.h"
#include <string.h>

#include "Teknovus.h"
#include "Build.h"
#include "PortCapability.h"

//base includes
#include "OntConfigDb.h"

//app includes
#include "OamIeee.h"
#include "CtcOnuOam.h"
#include "CtcMLlid.h"
#include "CtcClassification.h"
#include "Ethernet.h"
#include "CtcEthControl.h"
#include "TkPlatform.h"
#include "EponDevInfo.h"
#include "skb_defines.h"


#define MAX_QOS_CLASS_RULES_PER_PORT 8
#define MAX_CLASS_RULES_PER_PORT 8
#define MAX_CLAUSES_PER_RULE 8
#define NEW_CLASS_RULES_INDEX_FLAG       0x8000
#define NEW_CLASS_RULES_INDEX_MASK       0xF000
#define DIRTY_CLASS_RULES_INDEX_FLAG     0x4000
#define DIRTY_CLASS_RULES_INDEX_MASK     0xF000

#define MIN_PRECEDENCE            1
#define MAX_PRECEDENCE            255

#define FLOW_ID_BASE          48


typedef struct
{
    U8 prec;
    U8 length;
    U8 queueMapped;
    U8 pri;
    U8 numClause;
    CtcRuleClause clause[MAX_CLAUSES_PER_RULE];
} PACK classRule_t;  //alignment should be the same as oam container

#define MAX_VLANCTL_RULES_PER_CLASS_RULE   3

typedef struct
{
    U8 action;
    U8 numRules;
    classRule_t rule[MAX_CLASS_RULES_PER_PORT];
    vlan_rule_data vlanctl_rules[MAX_CLASS_RULES_PER_PORT][MAX_VLANCTL_RULES_PER_CLASS_RULE];
    U32 flow_id[MAX_CLASS_RULES_PER_PORT];
} classConfig_t;

static classConfig_t class_cfg[MAX_UNI_PORTS];

static int findRuleIndexForposRuleId(int uni_port, OamCtcRule *pRule, int tag_num);

////////////////////////////////////////////////////////////////////////////////
/// NextClause - Get the start of the next cluse
///
/// This function fetches a pointer to the start of the clause following the
/// supplied clause.
///
/// \param cls Clause to jump
///
/// \return
/// Clause after the given clause
////////////////////////////////////////////////////////////////////////////////
static
CtcRuleClause * NextClause(CtcRuleClause * cls)
    {
    if(cls->field <= CtcRuleFieldL4DestPort)
        {
        return (CtcRuleClause *)(&cls->clause6 + 1);
        }
    else
        {
        return (CtcRuleClause *)(&cls->clause16 + 1);
        }
    } // NextClause


////////////////////////////////////////////////////////////////////////////////
/// NextRule - Get the start of the next rule
///
/// This function fetches a pointer to the start of the rule following the
/// supplied rule.
///
/// \param rule Rule to jump
///
/// \return
/// Rule after the given rule
////////////////////////////////////////////////////////////////////////////////
static
OamCtcRule *NextRule (OamCtcRule *rule)
    {
    return (OamCtcRule *)((U8 *)rule + rule->length + 2);
    } // NextRule


static void create_rdpa_ic_rule(rdpactl_classification_rule_t *rdpaRule, 
                                   U32 uni_port, 
                                   classRule_t *pCtcRule,
                                   int prty)
{
   int i;
   uint16_t  outer_vid = 0;
   uint16_t  etype = 0;
   uint32_t  dst_ipv4 = 0;
   uint32_t  src_ipv4 = 0;
   uint16_t  src_port = 0;
   uint16_t  dst_port = 0;
   uint32_t  ipv6_label = 0;
   uint32_t  physical_port;

   physical_port = bcm_enet_map_oam_idx_to_phys_port(uni_port);   
   cmsLog_debug("uni_port = %d, physical_port = %d", uni_port, physical_port);
   
   memset(rdpaRule, 0, sizeof(rdpactl_classification_rule_t));
   rdpaRule->type = RDPACTL_IC_TYPE_QOS;
   rdpaRule->action = 1;
   rdpaRule->dir = rdpactl_dir_us;
   rdpaRule->prty = MAX_CLASS_RULES_PER_PORT*(physical_port+1) - prty - 1;
   rdpaRule->field_mask = RDPACTL_IC_MASK_INGRESS_PORT;
   rdpaRule->ingress_port_id = RDPACTL_IF_LAN0 + physical_port;
   rdpaRule->queue_id =  (1 << RDPACTL_QUEUEID_BITS_NUMBER) | pCtcRule->queueMapped;
   rdpaRule->dscp_remark = -1;
   rdpaRule->opbit_remark = pCtcRule->pri == 0xff ? -1 : pCtcRule->pri;
   rdpaRule->ipbit_remark =-1;

   for (i = 0; i < pCtcRule->numClause; i++) 
   {
       CtcRuleClause *pClause = &pCtcRule->clause[i];
       switch (pClause->field) {
           case OamCtcFieldDaMac:
               rdpaRule->field_mask |= RDPACTL_IC_MASK_DST_MAC;
               memcpy(rdpaRule->dst_mac, pClause->clause6.value, sizeof(rdpaRule->dst_mac));
               break;

           case OamCtcFieldSaMac:
               rdpaRule->field_mask |= RDPACTL_IC_MASK_SRC_MAC;
               memcpy(rdpaRule->src_mac, pClause->clause6.value, sizeof(rdpaRule->src_mac));
               break;

           case OamCtcFieldVlanPri:
               if ((pClause->clause6.value[5] >= 0) && (pClause->clause6.value[5] <= 7)) {
                   rdpaRule->field_mask |= RDPACTL_IC_MASK_OUTER_PBIT;
                   memcpy(&rdpaRule->outer_pbits, &pClause->clause6.value[5], sizeof(rdpaRule->outer_pbits));
                }
               break;

           case OamCtcFieldVlanId:
                
               rdpaRule->field_mask |= RDPACTL_IC_MASK_OUTER_VID;
               memcpy(&outer_vid, &pClause->clause6.value[4], sizeof(rdpaRule->outer_vid));
               rdpaRule->outer_vid = OAM_NTOHS(outer_vid);
               break;

           case OamCtcFieldEthertype:
               rdpaRule->field_mask |= RDPACTL_IC_MASK_ETHER_TYPE;
               memcpy(&etype, &pClause->clause6.value[4], sizeof(rdpaRule->etype));
               rdpaRule->etype = OAM_NTOHS(etype);
               break;

           case OamCtcFieldDestIp:
               rdpaRule->field_mask |= RDPACTL_IC_MASK_DST_IP;
               rdpaRule->ip_family = RDPACTL_IP_FAMILY_IPV4;
               memcpy(&dst_ipv4, &pClause->clause6.value[2], sizeof(rdpaRule->dst_ip.ipv4));
               rdpaRule->dst_ip.ipv4 = OAM_NTOHL(dst_ipv4);
               break;

           case OamCtcFieldSrcIp:
               rdpaRule->field_mask |= RDPACTL_IC_MASK_SRC_IP;
               rdpaRule->ip_family = RDPACTL_IP_FAMILY_IPV4;
               memcpy(&src_ipv4, &pClause->clause6.value[2], sizeof(rdpaRule->src_ip.ipv4));
               rdpaRule->src_ip.ipv4 = OAM_NTOHL(src_ipv4);
               break;

           case OamCtcFieldIpType:
               rdpaRule->field_mask |= RDPACTL_IC_MASK_IP_PROTOCOL;
               rdpaRule->protocol = pClause->clause6.value[5];
               break;

           case OamCtcFieldIpTos:
           case OamCtcFieldIpPrec:
               rdpaRule->field_mask |= RDPACTL_IC_MASK_DSCP;
               rdpaRule->dscp = pClause->clause6.value[5];
               break;

           case OamCtcFieldL4SrcPort:
               rdpaRule->field_mask |= RDPACTL_IC_MASK_SRC_PORT;
               memcpy(&src_port, &pClause->clause6.value[4], sizeof(rdpaRule->src_port));
               rdpaRule->src_port = OAM_NTOHS(src_port);
               break;

           case OamCtcFieldL4DestPort:
               rdpaRule->field_mask |= RDPACTL_IC_MASK_DST_PORT;
               memcpy(&dst_port, &pClause->clause6.value[4], sizeof(rdpaRule->dst_port));
               rdpaRule->dst_port = OAM_NTOHS(dst_port);
               break;

           case OamCtcFieldIPv4v6Version:
               rdpaRule->field_mask |= RDPACTL_IC_MASK_L3_PROTOCOL;
               rdpaRule->version |= 
                   ((pClause->clause16.value[15] == CtcIpV4Version)? 
                   RDPA_L3_PROTOCOL_IPV4 : RDPA_L3_PROTOCOL_IPV6);
               break;

           case OamCtcFieldIPv6FlowLable:
               rdpaRule->field_mask |= RDPACTL_IC_MASK_IPV6_FLOW_LABEL;
               memcpy(&ipv6_label, &pClause->clause16.value[12], sizeof(rdpaRule->ipv6_label));
               rdpaRule->ipv6_label = OAM_NTOHL(ipv6_label);
               break;

           case OamCtcFieldIPv6DestAddr:
               rdpaRule->field_mask |= RDPACTL_IC_MASK_DST_IP;
               rdpaRule->ip_family = RDPACTL_IP_FAMILY_IPV6;
               memcpy(rdpaRule->dst_ip.ipv6, pClause->clause16.value, sizeof(rdpaRule->dst_ip.ipv6));
               break;

           case OamCtcFieldIPv6SrcAddr:
               rdpaRule->field_mask |= RDPACTL_IC_MASK_SRC_IP;
               rdpaRule->ip_family = RDPACTL_IP_FAMILY_IPV6;
               memcpy(rdpaRule->src_ip.ipv6, pClause->clause16.value, sizeof(rdpaRule->src_ip.ipv6));
               break;

           default:
               break;
       }
    }
}

static void save_qos_rule(vlan_rule_data *pRule, char *devName, 
    vlanCtl_direction_t direction, unsigned int num_tags, unsigned int rule_idx) 
{
    pRule->valid = 1;
    pRule->nbrOfTags = num_tags;
    pRule->dir = direction;
    pRule->tagRuleId = rule_idx;
    strcpy(pRule->devName, devName);
}

static Bool is_new_class_entry(U16 flag)
{
    return ((flag&NEW_CLASS_RULES_INDEX_MASK) 
               == NEW_CLASS_RULES_INDEX_FLAG);
}

static Bool is_dirty_class_entry(U16 flag)
{
    return ((flag&DIRTY_CLASS_RULES_INDEX_MASK) 
               == DIRTY_CLASS_RULES_INDEX_FLAG);
}

static int find_class_entry(U32 uni_port, U8 rulePrec, U8 *found_index)
{
    U8 i;

    for (i = 0; i < class_cfg[uni_port].numRules; i++)
    {
        if (class_cfg[uni_port].rule[i].prec == rulePrec)
        {
            *found_index = i;
            return 0;
        }
    }

    return -1;
}

static void del_ebtables_rule(int uni_port, U8 index)
{
    if (0 != class_cfg[uni_port].flow_id[index])
    {
        CtcRuleClause *pClause = NULL;
        classRule_t *pRule = &class_cfg[uni_port].rule[index];
        int j;
        char cmdStr[BUFLEN_512];
        char smacStr[BUFLEN_48];
        char dmacStr[BUFLEN_48];
        char ifStr[IFNAMESIZ];
        U32 flowIdMask = 0;


        cmdStr[0] = '\0';
        smacStr[0] = '\0'; 
        dmacStr[0] = '\0';
        ifStr[0] = '\0'; 

        pClause = (CtcRuleClause *)pRule->clause;
        for (j = 0; j < pRule->numClause; j++)
        {
            switch (pClause->field)
            {
            case OamCtcFieldDaMac:
                 sprintf(dmacStr, "--dst %02x:%02x:%02x:%02x:%02x:%02x",
                    pClause->clause6.value[0],pClause->clause6.value[1],
                    pClause->clause6.value[2],pClause->clause6.value[3],
                    pClause->clause6.value[4],pClause->clause6.value[5]);
                break;
            case OamCtcFieldSaMac:
                sprintf(smacStr, "--src %02x:%02x:%02x:%02x:%02x:%02x",
                    pClause->clause6.value[0],pClause->clause6.value[1],
                    pClause->clause6.value[2],pClause->clause6.value[3],
                    pClause->clause6.value[4],pClause->clause6.value[5]);
                break;
            default:
                cmsLog_debug("Not supported  by ebtables, ignored");
            break;
            }
        }

        sprintf(ifStr, "-i %s.0", uniRealIfname[uni_port]);
        flowIdMask = SKBMARK_SET_FLOW_ID(flowIdMask,class_cfg[uni_port].flow_id[index]);
        sprintf(cmdStr, "ebtables  -D FORWARD %s %s %s  -j mark --mark-or 0x%x ", ifStr, smacStr, dmacStr, flowIdMask);
        
        cmsLog_debug("%s", cmdStr);
        system(cmdStr);
        class_cfg[uni_port].flow_id[index] = 0;
    }
}

static void delete_class_rule(int uni_port, U8 index) 
{
     int i;
     rdpactl_classification_rule_t rdpaRule;

     create_rdpa_ic_rule(&rdpaRule, uni_port, 
               (classRule_t *)&class_cfg[uni_port].rule[index],
               index);
     rdpaCtl_del_classification_rule(&rdpaRule);

     vlanCtl_init();
     for (i=0; i < MAX_VLANCTL_TAGS; i++)
     {
         if (class_cfg[uni_port].vlanctl_rules[index][i].valid)
         {  
             class_cfg[uni_port].vlanctl_rules[index][i].valid = 0;
             vlanCtl_removeTagRule((char *)class_cfg[uni_port].vlanctl_rules[index][i].devName, 
                 class_cfg[uni_port].vlanctl_rules[index][i].dir, 
                 class_cfg[uni_port].vlanctl_rules[index][i].nbrOfTags, 
                 class_cfg[uni_port].vlanctl_rules[index][i].tagRuleId);
         }
     }
     vlanCtl_cleanup();
    
     del_ebtables_rule(uni_port,index);
}


static void move_all_class_entries(int uni_port, U8 index, BOOL forward)
{
    int i;

    if(forward)
    {   
        for(i = index; i < class_cfg[uni_port].numRules; i++)
        {
            memcpy(&class_cfg[uni_port].rule[i-1],
               &class_cfg[uni_port].rule[i], sizeof(classRule_t));     
            memset(&class_cfg[uni_port].rule[i],0x0, sizeof(classRule_t));  
            memcpy(&class_cfg[uni_port].vlanctl_rules[i-1],
                &class_cfg[uni_port].vlanctl_rules[i], 
                MAX_VLANCTL_RULES_PER_CLASS_RULE*sizeof(vlan_rule_data));   
            memset(&class_cfg[uni_port].vlanctl_rules[i],0x0, 
                MAX_VLANCTL_RULES_PER_CLASS_RULE*sizeof(vlan_rule_data));  

            class_cfg[uni_port].flow_id[i-1]= class_cfg[uni_port].flow_id[i];
            class_cfg[uni_port].flow_id[i] = 0;
        }
    }
    else
    {   
        for(i = class_cfg[uni_port].numRules; i > index; i--)
        {            
            memcpy(&class_cfg[uni_port].rule[i],
               &class_cfg[uni_port].rule[i-1], sizeof(classRule_t));    
            memset(&class_cfg[uni_port].rule[i-1],0x0, sizeof(classRule_t));    
            memcpy(&class_cfg[uni_port].vlanctl_rules[i],
                &class_cfg[uni_port].vlanctl_rules[i-1], 
                MAX_VLANCTL_RULES_PER_CLASS_RULE*sizeof(vlan_rule_data));   
            memset(&class_cfg[uni_port].vlanctl_rules[i-1],0x0, 
                MAX_VLANCTL_RULES_PER_CLASS_RULE*sizeof(vlan_rule_data));   

            class_cfg[uni_port].flow_id[i]= class_cfg[uni_port].flow_id[i-1];
            class_cfg[uni_port].flow_id[i-1] = 0;
        }
    }
}
static void insert_class_entry_at(int uni_port, OamCtcRule *pRule, 
                                    U8 index, U16 *newClassRulesIndex)
{
    int i;

    /* Move all entries backward */
    if(index < (MAX_QOS_CLASS_RULES_PER_PORT-1))
        {
        move_all_class_entries(uni_port, index, FALSE);
        if(pRule->prec == class_cfg[uni_port].rule[index+1].prec)
            class_cfg[uni_port].rule[index+1].prec++;
        }
    /* Check precedence conflict */
    for(i = index+1; i < class_cfg[uni_port].numRules; i++)
    {
        if(class_cfg[uni_port].rule[i].prec == 
            class_cfg[uni_port].rule[i+1].prec)
            class_cfg[uni_port].rule[i+1].prec++;
    }
    /* Update index */
    for(i = class_cfg[uni_port].numRules; i > index; i--)
    {   
        newClassRulesIndex[i] = newClassRulesIndex[i-1];    
    }
    newClassRulesIndex[index] = index | NEW_CLASS_RULES_INDEX_FLAG;
}

/* TBD: Use the appropriate data structure for inserting/deleting rules by precedence */
static int get_new_class_entry_index(int uni_port, OamCtcRule *pRule, 
                        U8 *add_index, U16 *newClassRulesIndex)
{
    int i;

    /* If the table is full */
    if(class_cfg[uni_port].numRules >= MAX_QOS_CLASS_RULES_PER_PORT)            
    {
        if(pRule->prec 
           > class_cfg[uni_port].rule[class_cfg[uni_port].numRules-1].prec)
        { /* New entry is lower than lowest entry, we don't add it */
            return -1;
        }
        /* Delete lowest rule */
        if(!is_new_class_entry(
              newClassRulesIndex[MAX_QOS_CLASS_RULES_PER_PORT - 1]))
        {
            delete_class_rule(uni_port, MAX_QOS_CLASS_RULES_PER_PORT - 1);
        }
        class_cfg[uni_port].numRules--;
    }
    if(class_cfg[uni_port].numRules == 0)
    {
        *add_index = 0;
        newClassRulesIndex[0] = NEW_CLASS_RULES_INDEX_FLAG;
    }
    else
    {
        U16 idx = 0;
        for(i = 0; i < class_cfg[uni_port].numRules; i++)
        {
            if (class_cfg[uni_port].rule[i].prec < pRule->prec)
            { 
                idx++;
            }         
            else
            {
                break;
            }
        }
        /* Insert new rule */
        insert_class_entry_at((int)uni_port, pRule, 
                              (U8)idx, newClassRulesIndex);            
       
        if((class_cfg[uni_port].rule[class_cfg[uni_port].numRules].prec == 0)
            && (idx != class_cfg[uni_port].numRules))
        { /* Delete lowest rule */
            if(!is_new_class_entry(
              newClassRulesIndex[class_cfg[uni_port].numRules]))
            {
                delete_class_rule(uni_port, class_cfg[uni_port].numRules);
            }            
            class_cfg[uni_port].numRules--;
        }
        
        *add_index = (U8)idx;
    }
        	
    class_cfg[uni_port].numRules++;
    
    cmsLog_debug("uni_port = %d, rulePrec = %d, add_index = %d", uni_port, pRule->prec, *add_index);	
    return 0;
}

static void flush_dirty_class_rules(int uni_port, U16*newClassRulesIndex)
{
    int i;


    for(i = class_cfg[uni_port].numRules - 1; i >= 0; i--)
    {
        if(is_new_class_entry(newClassRulesIndex[i]))
            continue;                     
        if(newClassRulesIndex[i] != i)
        { /* Delete it then re-add it later*/
            delete_class_rule(uni_port, i);
            newClassRulesIndex[i] |= NEW_CLASS_RULES_INDEX_FLAG;
        }
    }
}

static void update_class_rules(int uni_port, U16 *newClassRulesIndex)
{
    vlan_rule_data *pVlanRule;
    rdpactl_classification_rule_t rdpaRule;
    U8 addVlanctlRule,vlanPresent;
    int i,j;
    vlanCtl_direction_t direction = VLANCTL_DIRECTION_TX;    
    char realDevName[IFNAMESIZ];
    CtcRuleClause *pClause = NULL;
    classRule_t *pRule = NULL;
    unsigned int num_tags, ruleId = 0, posRuleId = 0, rule_index, position;
    unsigned int etherTypeFilter;
    U8 ebtablesRuleNeeded;
    U8 ebtablesRuleAdded;
    char cmdStr[BUFLEN_512];
    char smacStr[BUFLEN_48];
    char dmacStr[BUFLEN_48];
    char ifStr[IFNAMESIZ] ;
    U32 flowIdMask;
    U32 flowId;
    U8 prty;

    snprintf(realDevName, sizeof(realDevName), "%s", uniRealIfname[uni_port]);

    vlanCtl_init();

    for (i = 0; i < class_cfg[uni_port].numRules; i++)
    {
        if(!is_new_class_entry(newClassRulesIndex[i]))
            continue;

        addVlanctlRule = 1;
        vlanPresent = 0;
        num_tags = 0;
        etherTypeFilter = BCM_VLAN_DONT_CARE;

        pRule = &class_cfg[uni_port].rule[i];
        create_rdpa_ic_rule(&rdpaRule, uni_port, pRule, i); 

        ebtablesRuleNeeded = 0;
        ebtablesRuleAdded = 0;
        smacStr[0] = '\0';
        dmacStr[0] = '\0';
        cmdStr[0] = '\0';
        ifStr[0] = '\0'; 
        flowId = 0;
        flowIdMask = 0;
        while (num_tags < MAX_VLANCTL_TAGS)
        {
            vlanCtl_initTagRule();
            vlanCtl_setReceiveVlanDevice(eponVlanIfname);
            vlanCtl_filterOnRxRealDevice(realDevName);
            pClause = (CtcRuleClause *)pRule->clause;

            for (j = 0; j < pRule->numClause; j++)
            {
                CtcMatchValue6* matchVal = (CtcMatchValue6*)pClause->clause6.value;
                switch (pClause->field)
                {
                    case OamCtcFieldVlanPri:
                        if ((pClause->clause6.value[5] >= 0) && (pClause->clause6.value[5] <= 7))
                        {
                           vlanCtl_filterOnTagPbits((unsigned int)pClause->clause6.value[5], 0);
                        }
                        vlanPresent = 1;
                        break;

                    case OamCtcFieldVlanId:
                        vlanCtl_filterOnTagVid(GET_VID(OAM_NTOHS(matchVal->word[2])), 0);
                        vlanPresent = 1;
                        break;

                    case OamCtcFieldEthertype:
                        etherTypeFilter = (unsigned int)OAM_NTOHS(matchVal->word[2]);
                        break;

                    case OamCtcFieldIPv4v6Version:
                        etherTypeFilter = 
                            ((unsigned int)pClause->clause16.value[15] == CtcIpV4Version)? 
                            EthertypeIp : EthertypeIpv6;
                        break;

                    case OamCtcFieldIpType:
                        vlanCtl_filterOnIpProto((unsigned int)pClause->clause6.value[5]);
                        break;

                    case OamCtcFieldIpTos:
                    case OamCtcFieldIpPrec:
                        vlanCtl_filterOnDscp((unsigned int)pClause->clause6.value[5]);
                        break;

                    case OamCtcFieldDaMac:
                        sprintf(dmacStr, "--dst %02x:%02x:%02x:%02x:%02x:%02x", 
                            pClause->clause6.value[0],pClause->clause6.value[1],
                            pClause->clause6.value[2],pClause->clause6.value[3],
                            pClause->clause6.value[4],pClause->clause6.value[5]);
                        ebtablesRuleNeeded = 1;
                        break;

                    case OamCtcFieldSaMac:
                        sprintf(smacStr, "--src %02x:%02x:%02x:%02x:%02x:%02x", 
                            pClause->clause6.value[0],pClause->clause6.value[1],
                            pClause->clause6.value[2],pClause->clause6.value[3],
                            pClause->clause6.value[4],pClause->clause6.value[5]);
                        ebtablesRuleNeeded = 1;
                        break;

                    default:
                        addVlanctlRule = 0;
                        break;
                }

                pClause = NextClause(pClause);
            }

            if(ebtablesRuleNeeded)
            {
                if(!ebtablesRuleAdded)
                {
                    flowId = FLOW_ID_BASE +  uni_port*MAX_CLASS_RULES_PER_PORT + i;  
                    flowIdMask = SKBMARK_SET_FLOW_ID(flowIdMask,flowId);
                    sprintf(ifStr, "-i %s.0", uniRealIfname[uni_port]);
                    sprintf(cmdStr, "ebtables  -A FORWARD %s %s %s  -j mark --mark-or 0x%x ", ifStr, smacStr, dmacStr, flowIdMask);
                    cmsLog_debug("%s", cmdStr);
                    system(cmdStr);
                    ebtablesRuleAdded = 1;
                    class_cfg[uni_port].flow_id[i] = flowId;
                }
                vlanCtl_filterOnSkbMarkFlowId(class_cfg[uni_port].flow_id[i]);
            }

            if (addVlanctlRule) 
            {
                if((vlanPresent) && (!num_tags))
                    num_tags = 1;
    
                // for rule with tags >= 1, we should set tagethertype but not ethertype.
                if (etherTypeFilter != BCM_VLAN_DONT_CARE)
                {
                    //remove all the ethertype filter.
                    vlanCtl_filterOnEthertype(BCM_VLAN_DONT_CARE);
                    vlanCtl_filterOnTagEtherType(BCM_VLAN_DONT_CARE, 0);
                    vlanCtl_filterOnTagEtherType(BCM_VLAN_DONT_CARE, 1);
                    vlanCtl_filterOnTagEtherType(BCM_VLAN_DONT_CARE, 2);
                    switch (num_tags)
                    {
                        case 1:
                        case 2:
                        case 3:
                            // 1, 2 or 3 tags, just filter on tagethertype
                            vlanCtl_filterOnTagEtherType(etherTypeFilter, (num_tags-1));
                            break;
                        case 0:
                            //un-tag, just filter on ethertype
                            vlanCtl_filterOnEthertype(etherTypeFilter);
                        default:
                            break;
                    }
                }

                vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_IS_UNICAST | BCM_VLAN_FILTER_FLAGS_IS_BROADCAST);
                vlanCtl_cmdSetSkbMarkQueue(pRule->queueMapped);
                if ((pRule->pri >= 0) && (pRule->pri <= 7))
                {
                    vlanCtl_cmdSetTagPbits(pRule->pri, 0);
                }
                vlanCtl_setVlanRuleTableType(BCM_VLAN_RULE_TYPE_QOS);
                
                
                rule_index = findRuleIndexForposRuleId(uni_port, (OamCtcRule *)pRule, num_tags);
                posRuleId = VLANCTL_DONT_CARE;
                position = VLANCTL_POSITION_BEFORE;
                
                if (rule_index != -1)
                {
                    posRuleId = class_cfg[uni_port].vlanctl_rules[rule_index][num_tags].tagRuleId;
                    position = VLANCTL_POSITION_AFTER;
                }
                vlanCtl_insertTagRule(eponRealIfname, direction,num_tags, position, posRuleId, &ruleId);
                pVlanRule = &class_cfg[uni_port].vlanctl_rules[i][num_tags];
                save_qos_rule(pVlanRule, eponRealIfname, direction, num_tags, ruleId);
            }
                
            num_tags ++;
        }
		
        rdpaCtl_add_classification_rule(&rdpaRule, &prty);
    }
    vlanCtl_cleanup();
}
////////////////////////////////////////////////////////////////////////////////
/// CtcClassificationInit - Initialize CTC Classification module
///
/// This function initialize the CTC Classification module on
/// all of the user ports.
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void CtcClassificationInit (void)
    {
    memset(class_cfg, 0, sizeof(class_cfg));
    } // CtcClassificationInit

static int findRuleIndexForposRuleId(int uni_port, OamCtcRule *pRule, int tag_num)
{
    unsigned int i, next_low_prec = 0;
    int rule_index = -1;

    for (i = 0; i < class_cfg[uni_port].numRules; i++) {
        if ((class_cfg[uni_port].vlanctl_rules[i][tag_num].valid)&&
            (class_cfg[uni_port].rule[i].prec < pRule->prec) && 
            (class_cfg[uni_port].rule[i].prec > next_low_prec) )
        {
            next_low_prec = class_cfg[uni_port].rule[i].prec;
            rule_index = i;
        }
    }

    return rule_index;
}


////////////////////////////////////////////////////////////////////////////////
/// CtcRuleFind - Find if the rule has been installed.
///
/// This function find if teh rule has been installed.
///
/// \param port Ethernet port number
/// \param rule The rule to find
///
/// \return
/// TRUE: Installed; or FALSE;
////////////////////////////////////////////////////////////////////////////////
static
Bool CtcRuleFind (TkOnuEthPort port, OamCtcRule *pRule)
    {
    U8 ruleIndex;
    
    for(ruleIndex = 0; ruleIndex < class_cfg[port].numRules; ruleIndex++)
        {
        U8 *ruleStart = (U8*)&class_cfg[port].rule[ruleIndex];
        
        if(memcmp((void *)ruleStart, (void *)pRule,
            sizeof(OamCtcRuleNoClause)) == 0)
            {
            U8 cIndex;
            CtcRuleField *pField = (CtcRuleField *)pRule->clause;

            for (cIndex = 0; cIndex < pRule->numClause; cIndex++)
                {
                U8 *clause = 
                        (U8*)&class_cfg[port].rule[ruleIndex].clause[cIndex];
                
                if (*pField <= CtcRuleFieldL4DestPort)
                    {
                    if(memcmp((void *)pField, (void *)clause,
                        sizeof(CtcRuleClause6)) != 0)
                        {
                        return FALSE;
                        }
                    pField = pField + sizeof(CtcRuleClause6);
                    }
                else
                    {
                    if(memcmp((void *)pField, (void *)clause,
                        sizeof(CtcRuleClause16)) != 0)
                        {
                        return FALSE;
                        }
                    pField = pField + sizeof(CtcRuleClause16);
                    }
                }
           
            cmsLog_debug("ctc find rule:%u installed already", ruleIndex);
            return TRUE;
            }
        }

    return FALSE;
    } // CtcRuleFind


////////////////////////////////////////////////////////////////////////////////
/// CtcClassificationAdd - Add the CTC classification rules
///
/// This function processes a CTC classification add action.  Processing
/// includes validation of the rules and install them into the hardware.
///
/// \param port    Ethernet port number
/// \param ctcRule Pointer to the CTC Rule
///
/// \return
/// return code
////////////////////////////////////////////////////////////////////////////////
//extern
CtcRuleRetCode CtcClassificationAdd (TkOnuEthPort uni_port,
                                      OamCtcTlvClassMarking *pClass)
    {
    int i, j;
    CtcRuleOp op;
    CtcRuleClause *pClause;
    OamCtcRule *pRuleTmp, *pRule = pClass->rule;
    char realDevName[IFNAMESIZ];
    char vlanDevName[IFNAMESIZ];
    U16 newClassRulesIndex[MAX_CLASS_RULES_PER_PORT] = {0};

    cmsLog_debug("uni_port = %d, numRules = %d", uni_port, pClass->numRules);

    /*  Hard coding the vlan interface name for now. TBD to get this name */
    snprintf(realDevName, sizeof(vlanDevName), "%s", uniRealIfname[uni_port]);
    snprintf(vlanDevName, sizeof(vlanDevName), "%s.0", realDevName);

     /* Validate if we can support the given rules */
    pRuleTmp = pRule;
    for (i = 0; i < pClass->numRules; i++) 
    {
        if ((pRuleTmp->queueMapped < 0) || (pRuleTmp->queueMapped > 7)) {
            cmsLog_error("Rule %d Given Dest Queue is out of bounds", i);
            return CtcRuleRetCodeBadParameters;
        }

        if ((pRuleTmp->prec < MIN_PRECEDENCE) || (pRuleTmp->prec > MAX_PRECEDENCE)) {
            cmsLog_error("Rule %d Given precedence is out of bounds", i);
            return CtcRuleRetCodeBadParameters;
        }

        cmsLog_debug("Rule %d: prec = %d, len = 0x%x, queue = %d, pri = %d, numClause = %d", 
            i, pRuleTmp->prec, pRuleTmp->length, pRuleTmp->queueMapped, pRuleTmp->pri, pRuleTmp->numClause);

        pClause = (CtcRuleClause *)pRuleTmp->clause;
        
        for (j = 0; j < pRuleTmp->numClause; j++) 
        {
            cmsLog_debug("field = %d", pClause->field);
            switch (pClause->field) {
                case OamCtcFieldIPv6DestPrefix:
                case OamCtcFieldIPv6SrcPrefix:
                case OamCtcFieldIPv6NextHeader:
                    cmsLog_error("The given field (%d) is not yet supported\n", pClause->field);
                    return CtcRuleRetCodeBadParameters;

                case OamCtcFieldVlanId:
                case OamCtcFieldEthertype:
                case OamCtcFieldIpType:
                case OamCtcFieldIpTos:
                case OamCtcFieldIPv4v6Version:
                    break;

                case OamCtcFieldVlanPri:
                    if(pClause->clause6.value[0] != 0 || pClause->clause6.value[1] != 0 
                        || pClause->clause6.value[2] != 0 || pClause->clause6.value[3] != 0 
                        || pClause->clause6.value[4] != 0|| pClause->clause6.value[5] >7 )
                    {
                        cmsLog_error("The given cos (%d) is not  supported\n", pClause->clause6.value[5]);
                        return CtcRuleRetCodeBadParameters;
                    }
                    break;  

                case OamCtcFieldDaMac:
                case OamCtcFieldSaMac:
                    cmsLog_debug("The given field (%d) is supported by ebtables\n", pClause->field);
                    break;
                    
                default:
                    cmsLog_error("The given field (%d) is not supported by vlanctl, ingore in slow path\n", pClause->field);
                    break;
            }
           
            if (pClause->field <= CtcRuleFieldL4DestPort)
            {
                 op = pClause->clause6.op;
            }
            else
            {
                 op = pClause->clause16.op;
            }
            cmsLog_debug("op = %d", op);
            if (op != CtcRuleOpEqual) 
            {
                 cmsLog_error("Only OpEqual is supported right now");
                 return CtcRuleRetCodeBadParameters;
            }
             
            pClause = NextClause(pClause);
        }
 
        pRuleTmp = NextRule(pRuleTmp);
    }

    /* Init the result array */
    for(j = 0; j < class_cfg[uni_port].numRules; j++)
    {
       newClassRulesIndex[j] = j;
    }
 
     /* Configure the rules */
    for (i = 0; i < pClass->numRules; i++)
    {
        U8 add_index;

        /* check whether the rule installed already */
        if (CtcRuleFind (uni_port, pRule))
            continue;
        
        if (get_new_class_entry_index(uni_port, pRule, &add_index, newClassRulesIndex) == 0)
        {
            memcpy((void*)&class_cfg[uni_port].rule[add_index], pRule, sizeof(OamCtcRuleNoClause));
            pClause = (CtcRuleClause *)pRule->clause;
            for (j = 0; j < pRule->numClause; j++)
            {
                if (pClause->field <= CtcRuleFieldL4DestPort)
                {
                    class_cfg[uni_port].rule[add_index].clause[j].clause6 = 
                        pClause->clause6;
                }
                else
                {
                    class_cfg[uni_port].rule[add_index].clause[j].clause16 = 
                        pClause->clause16;
                }
                pClause = NextClause(pClause);
            }
            pRule = NextRule(pRule);
        }
    }
    
    /* Delete all changed items from RDPA */
    flush_dirty_class_rules(uni_port, newClassRulesIndex);
    
    /* Update RDPA data */
    update_class_rules(uni_port, newClassRulesIndex);
	 	
    return CtcRuleRetCodeOk;
    } // CtcClassificationAdd


////////////////////////////////////////////////////////////////////////////////
/// CtcClassificationDel - Delete CTC Classification rules
///
/// This function processes a CTC classification delete action.  A delete action
/// consists of a list of precedences to delete.  The format is first validated
/// and then the rules at each precedence level listed are deleted.
///
/// \param port     Ethernet port number
/// \param ctcRule  Pointer to the CTC Rule
///
/// \return
/// return code
///////////////////////////////////////////////////////////////////////////////
//extern
CtcRuleRetCode CtcClassificationDel (TkOnuEthPort uni_port,
                                    CtcTlvClassMarkingDelete BULK* pClass)
    {
    int i, j;
    U16 newClassRulesIndex[MAX_CLASS_RULES_PER_PORT] = {0};
    U16 oldNumRules;
	
    cmsLog_debug("uni_port = %d, numPrec = %d", uni_port, pClass->numPrec);

    /* Init the result array */
    for(j = 0; j < class_cfg[uni_port].numRules; j++)
    {
       newClassRulesIndex[j] = j;
    }

    oldNumRules = class_cfg[uni_port].numRules;
	
    for (i = 0; i < pClass->numPrec; i++)
    {
        U8 found_index;
        if (find_class_entry(uni_port, pClass->precToDelete[i], &found_index) == 0)
        {
            cmsLog_debug("found_index = %d", found_index);			           
            delete_class_rule(uni_port, found_index);
            move_all_class_entries(uni_port,found_index+1,TRUE);            
            newClassRulesIndex[found_index] |= DIRTY_CLASS_RULES_INDEX_FLAG;
            class_cfg[uni_port].numRules--;
        }
    }

    /* Re-order the rules */
    for(i = oldNumRules - 1; i >= 0; i--)
    {
        if(!is_dirty_class_entry(newClassRulesIndex[i]))
            continue;

        /* Update index */
        for(j = i; j < (oldNumRules - 1); j++)
        {   
            newClassRulesIndex[j] = newClassRulesIndex[j+1];    
        }
        oldNumRules--;
    }

    /* Delete all changed items from RDPA */
    flush_dirty_class_rules(uni_port, newClassRulesIndex);

    /* Update RDPA data */
    update_class_rules(uni_port, newClassRulesIndex);

    return CtcRuleRetCodeOk;
    } // CtcClassificationDel


////////////////////////////////////////////////////////////////////////////////
/// CtcClassificationClr - Delete all rules for port
///
/// This function deletes all rules at all precedence levels for a given port.
///
/// \param port Ethernet port number
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void CtcClassificationClr (TkOnuEthPort uni_port)
    {
    int i;

    cmsLog_debug("uni_port = %d, numRules = %d", uni_port, class_cfg[uni_port].numRules);


    for (i = class_cfg[uni_port].numRules - 1; i >= 0; i--)
    {
        delete_class_rule(uni_port, i);
        class_cfg[uni_port].numRules --;
    }

    } // CtcClassificationClr


////////////////////////////////////////////////////////////////////////////////
/// CtcClassificationGet - Process CTC classification get request
///
/// This function fills a CTC get request response with the current CTC rule
/// set.  It returns the number of bytes that have been added to the TLV.  If no
/// rules have been allocated the response will be empty save the action and the
/// rule count.
///
/// \param port     Ethernet port number
/// \param ctcRule  Pointer to the CTC Rule
///
/// \return
/// Size of TLV in bytes
////////////////////////////////////////////////////////////////////////////////
//extern
OamVarContainer * CtcClassificationGet (TkOnuEthPort uni_port,
                                            OamVarContainer *cont)
{
    int i, j;
    CtcRuleClause *pClause;
    OamCtcTlvClassMarking *pClass = (OamCtcTlvClassMarking *)&(cont->value[0]);
    OamCtcRule *pRule = (OamCtcRule *)pClass->rule;
	
    cmsLog_debug("uni_port = %d, numRules = %d", uni_port, class_cfg[uni_port].numRules);

    pClass->action = OamCtcClassRuleList;
    pClass->numRules = 0;
    cont->length = 2;

    for (i = 0; i < class_cfg[uni_port].numRules; i++)
    {
        if ((cont->length + class_cfg[uni_port].rule[i].length + 2) >= 128)
        {
            StreamSkip(&oamParser.reply, OamContSize((OamVarContainer *)oamParser.reply.cur));
            cont = OamCtcNextCont (cont);
            pClass = (OamCtcTlvClassMarking *)&(cont->value[0]);
            pRule = (OamCtcRule *)pClass->rule;
            pClass->action = OamCtcClassRuleList;
            pClass->numRules = 0;
            cont->length = 2;
        }

        pClass->numRules++;
        memcpy((void *)pRule, (void *)&class_cfg[uni_port].rule[i], 
            sizeof(OamCtcRuleNoClause));
        
        pClause = (CtcRuleClause *)pRule->clause;
        for (j = 0; j < pRule->numClause; j++) 
        {
            if (class_cfg[uni_port].rule[i].clause[j].field <= CtcRuleFieldL4DestPort)
            {
                memcpy((void *)pClause, 
                    (void *)&class_cfg[uni_port].rule[i].clause[j].clause6, 
                    sizeof(CtcRuleClause6));
            }
            else
            {
                memcpy((void *)pClause, 
                    (void *)&class_cfg[uni_port].rule[i].clause[j].clause16, 
                    sizeof(CtcRuleClause16));
            }
            pClause = NextClause(pClause);
        }
        cont->length += class_cfg[uni_port].rule[i].length + 2;
        pRule = NextRule(pRule);
    }

    return cont;
}
// CtcClassificationGet


////////////////////////////////////////////////////////////////////////////////
/// CtcClassificationLinkRuleClr - Delete all rules of the link
///
/// This function deletes all rules of the link.
///
/// \param link  link number
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void CtcClassificationLinkRuleClr (LinkIndex link)
    {
    } // CtcClassificationLinkRuleClr


////////////////////////////////////////////////////////////////////////////////
/// CtcClassificationAdjust - Adjust all rules installed
///
/// This function recalculates the rule queue for all rules, and deletes the
/// old rules, then install new rules.
///
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void CtcClassificationAdjust (void)
    {
    } // CtcClassificationAdjust


////////////////////////////////////////////////////////////////////////////////
/// CtcClassificationClrAllRules - clera all rules except the default rule
///
/// This function clears all the rules configured on the ONU.
///
/// \param NONE
///
/// \return
/// NONE
////////////////////////////////////////////////////////////////////////////////
//extern
void CtcClassificationClrAllRules (void)
    {
    TkOnuEthPort portIdx;
    for(portIdx = 0; portIdx < UniCfgDbGetActivePortCount(); portIdx++)
        {
        CtcClassificationClr(portIdx);
        }
    } // CtcClassificationClrAllRules


// End of file CtcClassification.c


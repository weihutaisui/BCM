
/*
* <:copyright-BRCM:2013:DUAL/GPL:standard
* 
*    Copyright (c) 2013 Broadcom 
*    All Rights Reserved
* 
* Unless you and Broadcom execute a separate written software license
* agreement governing use of this software, this software is licensed
* to you under the terms of the GNU General Public License version 2
* (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
* with the following added to such license:
* 
*    As a special exception, the copyright holders of this software give
*    you permission to link this software with independent modules, and
*    to copy and distribute the resulting executable under terms of your
*    choice, provided that you also meet, for each linked independent
*    module, the terms and conditions of the license of that module.
*    An independent module is a module which is not derived from this
*    software.  The special exception does not apply to any modifications
*    of the software.
* 
* Not withstanding the above, under no circumstances may you combine
* this software in any way with any other Broadcom software provided
* under a license other than the GPL, without Broadcom's express prior
* written consent.
* 
* :>
*/

/*
 *******************************************************************************
 * File Name  : rdpa_cmd_tm.c
 *
 * Description: This file contains the FAP Traffic Manager configuration API.
 *
 * The FAP Driver maintains two sets of TM configuration parameters: One for the
 * AUTO mode, and another for the MANUAL mode. The AUTO mode settings are
 * managed by the Ethernet driver based on the auto-negotiated PHY rates.
 * The MANUAL settings should be used by the user to configure the FAP TM.
 *
 * The mode can be set dynamically. Changing the mode does not apply the
 * corresponding settings into the FAP, it simply selects the current mode.
 * The settings corresponding to the current mode will take effect only when
 * explicitly applied to the FAP(s). This allows the caller to have a complete
 * configuration before activating it.
 *
 *******************************************************************************
 */

#include <linux/module.h>
#include <linux/jiffies.h>
#include <linux/delay.h>
#include <linux/bcm_log.h>
#include "bcmenet.h"
#include "bcmtypes.h"
#include "bcmnet.h"
#include "rdpa_types.h"
#include "rdpa_api.h"
#include "rdpa_ag_port.h"
#include "rdpa_drv.h"
#include "rdpa_cmd_ic.h"

#define __BDMF_LOG__

#define CMD_IC_LOG_ID_RDPA_CMD_DRV BCM_LOG_ID_RDPA_CMD_DRV

#if defined(__BDMF_LOG__)
#define CMD_IC_LOG_ERROR(fmt, args...) 										\
    do {                                                            				\
        if (bdmf_global_trace_level >= bdmf_trace_level_error)      				\
            bdmf_trace("ERR: %s#%d: " fmt "\n", __FUNCTION__, __LINE__, ## args);	\
    } while(0)
#define CMD_IC_LOG_INFO(fmt, args...) 										\
    do {                                                            				\
        if (bdmf_global_trace_level >= bdmf_trace_level_info)      					\
            bdmf_trace("INF: %s#%d: " fmt "\n", __FUNCTION__, __LINE__, ## args);	\
    } while(0)
#define CMD_IC_LOG_DEBUG(fmt, args...) 										\
    do {                                                            				\
        if (bdmf_global_trace_level >= bdmf_trace_level_debug)      					\
            bdmf_trace("DBG: %s#%d: " fmt "\n", __FUNCTION__, __LINE__, ## args);	\
    } while(0)
#else
#define CMD_IC_LOG_ERROR(fmt, arg...) BCM_LOG_ERROR(fmt, arg...)
#define CMD_IC_LOG_INFO(fmt, arg...) BCM_LOG_INFO(fmt, arg...)
#define CMD_IC_LOG_DEBUG(fmt, arg...) BCM_LOG_DEBUG(fmt, arg...)
#endif

static int ic_get_fieldmask(rdpactl_classification_rule_t *rule, uint32_t *fieldmask, int *size)
{
    *fieldmask = 0;

    if (rule->field_mask & RDPACTL_IC_MASK_SRC_IP)
        *fieldmask |= RDPA_IC_MASK_SRC_IP;

    if (rule->field_mask & RDPACTL_IC_MASK_DST_IP)
        *fieldmask |= RDPA_IC_MASK_DST_IP;

    if (rule->field_mask & RDPACTL_IC_MASK_IPV6_FLOW_LABEL)
        *fieldmask |= RDPA_IC_MASK_IPV6_FLOW_LABEL;

    if (rule->field_mask & RDPACTL_IC_MASK_OUTER_TPID)
        *fieldmask |= RDPA_IC_MASK_OUTER_TPID;

    if (rule->field_mask & RDPACTL_IC_MASK_INNER_TPID)
        *fieldmask |= RDPA_IC_MASK_INNER_TPID;

    if (rule->field_mask & RDPACTL_IC_MASK_SRC_PORT)
        *fieldmask |= RDPA_IC_MASK_SRC_PORT;

    if (rule->field_mask & RDPACTL_IC_MASK_DST_PORT)
        *fieldmask |= RDPA_IC_MASK_DST_PORT;

    if (rule->field_mask & RDPACTL_IC_MASK_OUTER_VID)
        *fieldmask |= RDPA_IC_MASK_OUTER_VID;

    if (rule->field_mask & RDPACTL_IC_MASK_INNER_VID)
        *fieldmask |= RDPA_IC_MASK_INNER_VID;

    if (rule->field_mask & RDPACTL_IC_MASK_DST_MAC)
        *fieldmask |= RDPA_IC_MASK_DST_MAC;

    if (rule->field_mask & RDPACTL_IC_MASK_SRC_MAC)
        *fieldmask |= RDPA_IC_MASK_SRC_MAC;

    if (rule->field_mask & RDPACTL_IC_MASK_ETHER_TYPE)
        *fieldmask |= RDPA_IC_MASK_ETHER_TYPE;

    if (rule->field_mask & RDPACTL_IC_MASK_IP_PROTOCOL)
        *fieldmask |= RDPA_IC_MASK_IP_PROTOCOL;

    if (rule->field_mask & RDPACTL_IC_MASK_DSCP)
        *fieldmask |= RDPA_IC_MASK_DSCP;

    if (rule->field_mask & RDPACTL_IC_MASK_SSID)
        *fieldmask |= RDPA_IC_MASK_SSID;

    if (rule->field_mask & RDPACTL_IC_MASK_OUTER_PBIT)
        *fieldmask |= RDPA_IC_MASK_OUTER_PBIT;

    if (rule->field_mask & RDPACTL_IC_MASK_INNER_PBIT)
        *fieldmask |= RDPA_IC_MASK_INNER_PBIT;

    if (rule->field_mask & RDPACTL_IC_MASK_INGRESS_PORT)
        *fieldmask |= RDPA_IC_MASK_INGRESS_PORT;

    if (rule->field_mask & RDPACTL_IC_MASK_NUM_OF_VLANS)
        *fieldmask |= RDPA_IC_MASK_NUM_OF_VLANS;

    if (rule->field_mask & RDPACTL_IC_MASK_L3_PROTOCOL)
        *fieldmask |= RDPA_IC_MASK_L3_PROTOCOL;

    if (rule->field_mask & RDPACTL_IC_MASK_GENERIC_1)
        *fieldmask |= RDPA_IC_MASK_GENERIC_1;

    if (rule->field_mask & RDPACTL_IC_MASK_GENERIC_2)
        *fieldmask |= RDPA_IC_MASK_GENERIC_2;

    if (rule->field_mask & RDPACTL_IC_MASK_INGRESS_WANFLOW)
        *fieldmask |= RDPA_IC_MASK_GEM_FLOW;

    if (size) *size = 0;  //Sarah: TBD calculate mask size
    return 0;
}

static rdpa_if ic_get_rdpa_port(uint8_t port)
{
    if ((RDPACTL_IF_LAN0 <= port) && (RDPACTL_IF_LAN7 >= port))
        return (port - RDPACTL_IF_LAN0 + rdpa_if_lan0);
    else
        return rdpa_wan_type_to_if(rdpa_wan_epon); /* Ilyal : As far as i know, it's used only by EPON code at the moment. 
                                                  So we can either use hard coded rdpa_if_wan0 or hardcoded  epon in get_if. */
}

static int delete_ic(bdmf_object_handle ingress_class_obj)
{
    bdmf_error_t rc;

    CMD_IC_LOG_INFO("delete_ic");

    if ((rc = bdmf_destroy(ingress_class_obj))) {
        CMD_IC_LOG_ERROR("bdmf_destroy() failed:rc(%d)", rc);
        return RDPA_DRV_SH_DESTROY;
    }

    return 0;
}

static int find_ic(rdpactl_classification_rule_t *rule, bdmf_object_handle *ingress_class_obj, bdmf_number *ic_idx, uint8_t *prty)
{
    bdmf_object_handle obj = NULL;
    rdpa_ic_cfg_t  cfg;
    rdpa_traffic_dir dir;
    uint32_t field_mask;

    *ingress_class_obj = NULL;
    ic_get_fieldmask(rule, &field_mask, NULL);

    while ((obj = bdmf_get_next(rdpa_ingress_class_drv(), obj, NULL)))
    {
        rdpa_ingress_class_dir_get(obj, &dir);
        if (dir != (rdpa_traffic_dir)(rule->dir))  continue;

        rdpa_ingress_class_cfg_get(obj, &cfg);
        if (cfg.type != (rdpa_ic_type)rule->type || cfg.field_mask != field_mask) continue;

        if (cfg.gen_rule_cfg1.type != rule->gen_rule_cfg1.type ||
            cfg.gen_rule_cfg1.offset != rule->gen_rule_cfg1.offset ||
            cfg.gen_rule_cfg1.mask != rule->gen_rule_cfg1.mask ||
            cfg.gen_rule_cfg2.type != rule->gen_rule_cfg2.type ||
            cfg.gen_rule_cfg2.offset != rule->gen_rule_cfg2.offset ||
            cfg.gen_rule_cfg2.mask != rule->gen_rule_cfg2.mask) 
            continue;

        *prty = cfg.prty;
        if (cfg.prty != rule->prty)
            CMD_IC_LOG_ERROR("rule priority %d ignored, using %d instead", rule->prty, cfg.prty);

        rdpa_ingress_class_index_get(obj, ic_idx);
        *ingress_class_obj = obj;
        return 1;
    }

    return 0;
}

static int add_ic(rdpactl_classification_rule_t *rule, bdmf_object_handle *ingress_class_obj)
{
    rdpa_ic_cfg_t cfg;
    int rc;
    BDMF_MATTR(ingress_class_attrs, rdpa_ingress_class_drv());

    CMD_IC_LOG_INFO("dir:%d, field_mask:0x%x", rule->dir, rule->field_mask);

    cfg.type = (rdpa_ic_type)rule->type;
    ic_get_fieldmask(rule, &cfg.field_mask, NULL);
    cfg.prty = rule->prty;
    cfg.port_mask = 0;
    cfg.acl_mode = RDPA_ACL_MODE_BLACK;

    cfg.gen_rule_cfg1.type = rule->gen_rule_cfg1.type;
    cfg.gen_rule_cfg1.offset = rule->gen_rule_cfg1.offset;
    cfg.gen_rule_cfg1.mask = rule->gen_rule_cfg1.mask;
    cfg.gen_rule_cfg2.type = rule->gen_rule_cfg2.type;
    cfg.gen_rule_cfg2.offset = rule->gen_rule_cfg2.offset;
    cfg.gen_rule_cfg2.mask = rule->gen_rule_cfg2.mask;

    rc = rdpa_ingress_class_dir_set(ingress_class_attrs, rule->dir);
    rc = rc ? : rdpa_ingress_class_cfg_set(ingress_class_attrs, &cfg);
    rc = rc ? : bdmf_new_and_set(rdpa_ingress_class_drv(), NULL, ingress_class_attrs, ingress_class_obj);
    if (rc < 0)
    {
        CMD_IC_LOG_ERROR("Failed to create ingress_class object, rc=%d", rc);
        return -1;
    }

    return 0;
}

static void ic_vlan_action_find(rdpa_vlan_action_cfg_t *action, 
    rdpa_traffic_dir dir, 
    bdmf_object_handle *vlan_action_obj)
{
    bdmf_object_handle obj = NULL;
    rdpa_vlan_action_cfg_t action_tmp;
    rdpa_traffic_dir dir_tmp;

    *vlan_action_obj = NULL;

    while ((obj = bdmf_get_next(rdpa_vlan_action_drv(), obj, NULL)))
    {
        rdpa_vlan_action_dir_get(obj, &dir_tmp);
        if (dir != dir_tmp)
            continue;

        rdpa_vlan_action_action_get(obj, &action_tmp);
        if (!memcmp(action, &action_tmp, sizeof(action_tmp)))
        {
            *vlan_action_obj = obj;
            return;
        }
    }
}


static int ic_vlan_action_add(rdpa_vlan_action_cfg_t *action, rdpa_traffic_dir dir, bdmf_object_handle *vlan_action_obj)
{
    int rc;
    BDMF_MATTR(vlan_action_attr, rdpa_vlan_action_drv());

    rc = rdpa_vlan_action_dir_set(vlan_action_attr, dir);
    rc = rc ? : rdpa_vlan_action_action_set(vlan_action_attr, action);
    /* index will be picked automatically by rdpa */
    rc = rc ? : bdmf_new_and_set(rdpa_vlan_action_drv(), NULL, vlan_action_attr, vlan_action_obj);

    return rc; 
}


static int create_ic_flow_result(uint8_t action, rdpactl_classification_rule_t *rule, rdpa_ic_info_t *flow, rdpa_traffic_dir dir)
{
    int rc = 0;

    flow->result.qos_method = rule->qos_method;

    //for EPON upstream, it's LLID
    flow->result.wan_flow = rule->wan_flow;

    flow->result.action = rule->action;

    flow->result.forw_mode = rule->forw_mode;

    flow->result.egress_port = ic_get_rdpa_port(rule->egress_port);

    flow->result.queue_id = rule->queue_id & (~RDPACTL_WANFLOW_MASK); 

    //vlan action parse
    if (rule->vlan_action.cmd != RDPACTL_VLAN_CMD_TRANSPARENT)
    {
        bdmf_object_handle vlan_action_obj;
        ic_vlan_action_find((rdpa_vlan_action_cfg_t*)&rule->vlan_action, dir, &vlan_action_obj);
        if (!vlan_action_obj)//no same vlan_action existed, should create a new one.
        {
            if (action)
                rc = ic_vlan_action_add((rdpa_vlan_action_cfg_t*)&rule->vlan_action, dir, &vlan_action_obj);
        }
        else
        {
            bdmf_number idx;
            rdpa_vlan_action_index_get(vlan_action_obj, &idx);
        }

        if (!rc)
            flow->result.vlan_action = vlan_action_obj;
    }

    flow->result.opbit_remark = (rule->opbit_remark != -1); 
    flow->result.opbit_val =(rule->opbit_remark == -1) ? 0 : rule->opbit_remark; 

    flow->result.ipbit_remark = (rule->ipbit_remark != -1); 
    flow->result.ipbit_val =(rule->ipbit_remark == -1) ? 0 : rule->ipbit_remark; 

    flow->result.dscp_remark = (rule->dscp_remark != -1); 
    flow->result.dscp_val =(rule->dscp_remark == -1) ? 0 : rule->dscp_remark; 

    flow->result.policer = NULL;  //Sarah: TBD

    if(rule->service_queue_info & RDPACTL_SERVICEACT_Q_MASK)
    {
        flow->result.action_vec |= rdpa_ic_action_service_q;
        flow->result.service_q_id = 
            rule->service_queue_info & RDPACTL_SERVICEQUEUE_MASK;
    }

    return rc;
}

static void create_ic_flow_key(rdpactl_classification_rule_t *rule, rdpa_ic_info_t *flow)
{
    if ((rule->field_mask & RDPACTL_IC_MASK_SRC_IP) && rule->ip_family == RDPACTL_IP_FAMILY_IPV4)
    {
        flow->key.src_ip.family = bdmf_ip_family_ipv4;
        flow->key.src_ip.addr.ipv4 = rule->src_ip.ipv4;
    }

    if ((rule->field_mask & RDPACTL_IC_MASK_SRC_IP) && rule->ip_family == RDPACTL_IP_FAMILY_IPV6)
    {
        flow->key.src_ip.family = bdmf_ip_family_ipv6;
        memcpy(flow->key.src_ip.addr.ipv6.data, rule->src_ip.ipv6, sizeof(rule->src_ip.ipv6));
    }

    if ((rule->field_mask & RDPACTL_IC_MASK_DST_IP) && rule->ip_family == RDPACTL_IP_FAMILY_IPV4)
    {
        flow->key.dst_ip.family = bdmf_ip_family_ipv4;
        flow->key.dst_ip.addr.ipv4 = rule->dst_ip.ipv4;
    }

    if ((rule->field_mask & RDPACTL_IC_MASK_DST_IP) && rule->ip_family == RDPACTL_IP_FAMILY_IPV6)
    {
        flow->key.dst_ip.family = bdmf_ip_family_ipv6;
        memcpy(flow->key.dst_ip.addr.ipv6.data, rule->dst_ip.ipv6, sizeof(rule->dst_ip.ipv6));
    }

    if (rule->field_mask & RDPACTL_IC_MASK_IPV6_FLOW_LABEL)
        flow->key.ipv6_flow_label = rule->ipv6_label;

    if (rule->field_mask & RDPACTL_IC_MASK_OUTER_TPID)
        flow->key.outer_tpid =  rule->outer_tpid;

    if (rule->field_mask & RDPACTL_IC_MASK_INNER_TPID)
        flow->key.inner_tpid = rule->inner_tpid;

    if (rule->field_mask & RDPACTL_IC_MASK_SRC_PORT)
        flow->key.src_port = rule->src_port;

    if (rule->field_mask & RDPACTL_IC_MASK_DST_PORT)
        flow->key.dst_port = rule->dst_port;

    if (rule->field_mask & RDPACTL_IC_MASK_OUTER_VID)
        flow->key.outer_vid =  rule->outer_vid;

    if (rule->field_mask & RDPACTL_IC_MASK_INNER_VID)
        flow->key.inner_vid = rule->inner_vid;

    if (rule->field_mask & RDPACTL_IC_MASK_DST_MAC)
        memcpy(&flow->key.dst_mac, rule->dst_mac, sizeof(flow->key.dst_mac));

    if (rule->field_mask & RDPACTL_IC_MASK_SRC_MAC)
        memcpy(&flow->key.src_mac, rule->src_mac, sizeof(flow->key.src_mac));

    if (rule->field_mask & RDPACTL_IC_MASK_ETHER_TYPE)
        flow->key.etype = rule->etype;

    if (rule->field_mask & RDPACTL_IC_MASK_IP_PROTOCOL)
        flow->key.protocol = rule->protocol;

    if (rule->field_mask & RDPACTL_IC_MASK_DSCP)
        flow->key.dscp = rule->dscp;

    if (rule->field_mask & RDPACTL_IC_MASK_INGRESS_PORT)
        flow->key.ingress_port = (rule->dir == rdpactl_dir_ds) ? rule->ingress_port_id : ic_get_rdpa_port(rule->ingress_port_id);

    if (rule->field_mask & RDPACTL_IC_MASK_OUTER_PBIT)
        flow->key.outer_pbits = rule->outer_pbits;

    if (rule->field_mask & RDPACTL_IC_MASK_INNER_PBIT)
        flow->key.inner_pbits = rule->inner_pbits;

    if (rule->field_mask & RDPACTL_IC_MASK_NUM_OF_VLANS)
        flow->key.number_of_vlans = rule->number_of_vlans;

    /* version field */
    if (rule->field_mask & RDPACTL_IC_MASK_L3_PROTOCOL)
        flow->key.l3_protocol = rule->version;

    if (rule->field_mask & RDPACTL_IC_MASK_GENERIC_1)
    {
        flow->key.generic_key_1 = rule->gen_rule_key_1;
        flow->key.generic_mask = rule->generic_mask;
    }

    if (rule->field_mask & RDPACTL_IC_MASK_GENERIC_2)
        flow->key.generic_key_2 = rule->gen_rule_key_2;    

    if (rule->field_mask & RDPACTL_IC_MASK_INGRESS_WANFLOW)
        flow->key.gem_flow = rule->ingress_wan_flow;    
}

static int create_ic_flow(uint8_t action, rdpactl_classification_rule_t *rule, rdpa_ic_info_t *flow, rdpa_traffic_dir dir)
{
    memset(flow, 0, sizeof(rdpa_ic_info_t));

    create_ic_flow_key(rule, flow);

    return create_ic_flow_result(action, rule, flow, dir);
}

/* Compare ALL ic flow fields */
static int rdpactl_compare_ic_flows(rdpa_ic_info_t *a, rdpa_ic_info_t *b)
{
#define COMPARE(field) do \
    { \
        if (a->field != b->field) \
        { \
            CMD_IC_LOG_DEBUG("mismatch at %s %ld %ld offset 0x%x", #field, (long)a->field, (long)b->field, \
                (int)offsetof(rdpa_ic_info_t, field)); \
            return -1; \
        } \
    } while (0)

    if (bdmf_ip_cmp(&a->key.src_ip, &b->key.src_ip))
        return -1;
    if (bdmf_ip_cmp(&a->key.dst_ip, &b->key.dst_ip))
        return -1;
    COMPARE(key.src_port);
    COMPARE(key.dst_port);
    COMPARE(key.protocol);
    COMPARE(key.outer_vid);
    COMPARE(key.inner_vid);
    if (memcmp(&a->key.dst_mac, &b->key.dst_mac, sizeof(a->key.dst_mac)))
        return -1;
    if (memcmp(&a->key.src_mac, &b->key.src_mac, sizeof(a->key.src_mac)))
        return -1;
    COMPARE(key.etype);
    COMPARE(key.dscp);
    COMPARE(key.ssid);
    COMPARE(key.ingress_port);
    COMPARE(key.gem_flow);
    COMPARE(key.outer_pbits);
    COMPARE(key.inner_pbits);
    COMPARE(key.number_of_vlans);
    COMPARE(key.ipv6_flow_label);
    COMPARE(key.outer_tpid);
    COMPARE(key.inner_tpid);
    COMPARE(key.l3_protocol);
    COMPARE(key.generic_key_1);
    COMPARE(key.generic_key_2);    
    COMPARE(result.qos_method);
    COMPARE(result.wan_flow);
    COMPARE(result.action);
    COMPARE(result.policer);
    COMPARE(result.forw_mode);
    COMPARE(result.egress_port);
    COMPARE(result.queue_id);
    COMPARE(result.vlan_action);
    COMPARE(result.opbit_remark);
    COMPARE(result.opbit_val);
    COMPARE(result.ipbit_remark);
    COMPARE(result.ipbit_val);
    COMPARE(result.dscp_remark);
    COMPARE(result.dscp_val);
    COMPARE(result.ecn_val);
    COMPARE(result.pbit_to_gem_table);

    return 0;
}


/* lookup if we have an existing flow with the same key/result besides vlan_action. Assume vlan_action in flow is not
   set */
static void rdpactl_match_ic_flow(bdmf_object_handle ingress_class_obj, rdpa_ic_info_t *flow, bdmf_index *flow_idx)
{
    rdpa_ic_info_t flow_tmp;

    *flow_idx = BDMF_INDEX_UNASSIGNED;
    while (rdpa_ingress_class_flow_get_next(ingress_class_obj, flow_idx) != BDMF_ERR_NO_MORE)
    {
        rdpa_ingress_class_flow_get(ingress_class_obj, *flow_idx, &flow_tmp);
        flow_tmp.result.vlan_action = NULL;
        if (!rdpactl_compare_ic_flows(&flow_tmp, flow))
        {
            CMD_IC_LOG_DEBUG("IC flow match found idx(%d)", (int)*flow_idx);
            return;
        }
    }

    *flow_idx = BDMF_INDEX_UNASSIGNED;
    CMD_IC_LOG_DEBUG("IC flow no match found");
}


/*******************************************************************************/
/* global routines                                                             */
/*******************************************************************************/

/*******************************************************************************
 *
 * Function: rdpa_cmd_ic_ioctl
 *
 * IOCTL interface to the RDPA INGRESS CLASSIFIER API.
 *
 *******************************************************************************/
int rdpa_cmd_ic_ioctl(unsigned long arg)
{
    rdpa_drv_ioctl_ic_t *userIc_p = (rdpa_drv_ioctl_ic_t *)arg;
    rdpa_drv_ioctl_ic_t ic;
    rdpactl_classification_rule_t rule;
    int ret = 0;
    bdmf_error_t rc = BDMF_ERR_OK;
    uint8_t prty = 0xFF;

    copy_from_user(&ic, userIc_p, sizeof(rdpa_drv_ioctl_ic_t));
    copy_from_user(&rule, ic.param.rule, sizeof(rdpactl_classification_rule_t));

    CMD_IC_LOG_DEBUG("RDPA IC CMD(%d)", ic.cmd);

    bdmf_lock();

    switch(ic.cmd)
    {
    case RDPA_IOCTL_IC_CMD_ADD_CLASSIFICATION_RULE: {
        bdmf_object_handle ingress_class_obj;
        int rc, icAdd = 0;
        bdmf_number ic_idx = 0;
        bdmf_index ic_flow_idx;
        rdpa_ic_info_t  ic_flow;
        bdmf_index flow_idx = BDMF_INDEX_UNASSIGNED;
        
        CMD_IC_LOG_DEBUG("RDPA_IOCTL_IC_CMD_ADD_CLASSIFICATION_RULE: field(0x%x) port_id(%d)", rule.field_mask, rule.ingress_port_id);
        
        if (!find_ic(&rule, &ingress_class_obj, &ic_idx, &prty)) 
        {
            rc = add_ic(&rule, &ingress_class_obj);
            if (rc)
            {
                CMD_IC_LOG_ERROR("add  ic rule fail for field 0x%x, rc=%d", rule.field_mask, rc);
                ret = RDPA_DRV_IC_ERROR;
                goto ioctl_exit;
            }
            icAdd = 1;
            prty = rule.prty;
        }
        else
        {
            CMD_IC_LOG_DEBUG("find exist ic:%d, prty:%d", (int)ic_idx, prty);
        }
        
        copy_to_user(&(userIc_p->param.prty), &prty, sizeof(uint8_t));
        
        rc = create_ic_flow(1, &rule, &ic_flow, rule.dir);
        if (rc)
        {
            CMD_IC_LOG_ERROR("create ic flow fail, rc=%d", rc);
            ret = RDPA_DRV_IC_ERROR;
            goto ioctl_exit;
        }
        
        rdpactl_match_ic_flow(ingress_class_obj, &ic_flow, &flow_idx);
        if (flow_idx != BDMF_INDEX_UNASSIGNED)
            break;
        
        rc = rdpa_ingress_class_flow_add(ingress_class_obj, &ic_flow_idx, &ic_flow);
        if (rc)
        {
            CMD_IC_LOG_ERROR("add  ic flow error, rc=%d", rc);
        
            if (icAdd) 
                delete_ic(ingress_class_obj);
            else
                bdmf_put(ingress_class_obj);
        
            ret = (rc == BDMF_ERR_ALREADY ? 0 : RDPA_DRV_IC_FLOW_ERROR);
            goto ioctl_exit;
        }
        else
            CMD_IC_LOG_INFO("Created ic flow: ic_idx %d, flow_idx %d", (int)ic_idx, (int)ic_flow_idx);
        
        if (!icAdd) bdmf_put(ingress_class_obj);
        
        copy_to_user(&(userIc_p->param.prty), &prty, sizeof(uint8_t));
        break;
        }

    case RDPA_IOCTL_IC_CMD_DEL_CLASSIFICATION_RULE: {
        bdmf_object_handle ingress_class_obj = NULL;
        bdmf_index ic_flow_idx;
        rdpa_ic_info_t  ic_flow;
        bdmf_number ic_idx;
        int rc;
        bdmf_number nflows;
        
        CMD_IC_LOG_DEBUG("RDPA_IOCTL_IC_CMD_DEL_CLASSIFICATION_RULE: field(0x%x) port_id(%d)", rule.field_mask, rule.ingress_port_id);
        
        if (!find_ic(&rule, &ingress_class_obj, &ic_idx, &prty))
        {
            CMD_IC_LOG_ERROR("Not found qos ic");
            ret = RDPA_DRV_IC_NOT_FOUND;
            goto ioctl_exit;
        }
        
        memset(&ic_flow, 0, sizeof(rdpa_ic_info_t));
        create_ic_flow_key(&rule, &ic_flow);       
        rc = rdpa_ingress_class_flow_find(ingress_class_obj, &ic_flow_idx, &ic_flow);
        if (rc)
        {
            CMD_IC_LOG_ERROR("Cannot find ic flow, rc=%d\n", rc);
            bdmf_put(ingress_class_obj);
            goto ioctl_exit;
        }
        if(ic_flow.result.vlan_action) 
            bdmf_put(ic_flow.result.vlan_action);
        CMD_IC_LOG_INFO("Delete flow: ic_idx %d, flow_idx %d",(int)ic_idx, (int)ic_flow_idx);          
        rc = rdpa_ingress_class_flow_delete(ingress_class_obj, ic_flow_idx);
        if (rc)
        {
            CMD_IC_LOG_ERROR("Cannot delete ingress_class flow: ic_idx %d, flow_idx %d", (int)ic_idx, (int)ic_flow_idx); 
            bdmf_put(ingress_class_obj);
            ret = RDPA_DRV_IC_FLOW_ERROR;
            goto ioctl_exit;
        }
        
        rdpa_ingress_class_nflow_get(ingress_class_obj, &nflows);
        if (nflows == 0) 
        {
            bdmf_put(ingress_class_obj);
            //Sarah: TBD: if qos ic is not created here			
            delete_ic(ingress_class_obj);
        }
        else 
            bdmf_put(ingress_class_obj);
        
        break;
        }

    case RDPA_IOCTL_IC_CMD_ADD: {
        bdmf_object_handle ingress_class_obj;
        int rc, icAdd = 0;
        bdmf_number ic_idx = 0;
        
        CMD_IC_LOG_DEBUG("RDPA_IOCTL_IC_CMD_ADD: type(0x%x)field(0x%x) port_id(%d)", rule.type,rule.field_mask, rule.ingress_port_id);
        
        if (!find_ic(&rule, &ingress_class_obj, &ic_idx, &prty)) 
        {
            rc = add_ic(&rule, &ingress_class_obj);
            if (rc)
            {
                CMD_IC_LOG_ERROR("add  ic  fail, rc=%d", rc);
                ret = RDPA_DRV_IC_ERROR;
                goto ioctl_exit;
            }
            icAdd = 1;
        }
        else
        
            if (!icAdd) 
                bdmf_put(ingress_class_obj);
        break;
        }

    case RDPA_IOCTL_IC_CMD_DEL: {
        bdmf_object_handle ingress_class_obj = NULL;
        bdmf_number ic_idx;
        int rc;
        bdmf_number nflows;
        
        CMD_IC_LOG_DEBUG("RDPA_IOCTL_IC_CMD_DEL: type(0x%x)field(0x%x) port_id(%d)",rule.type, rule.field_mask, rule.ingress_port_id);
        
        if (!find_ic(&rule, &ingress_class_obj, &ic_idx, &prty))
        {
            CMD_IC_LOG_ERROR("Not found  ic");
            ret = RDPA_DRV_IC_NOT_FOUND;
            goto ioctl_exit;
        }
        
        rdpa_ingress_class_nflow_get(ingress_class_obj, &nflows);
        if (nflows == 0) 
        {
            bdmf_put(ingress_class_obj);
            rc = delete_ic(ingress_class_obj);
        }
        else 
            bdmf_put(ingress_class_obj);
        
        break;
        }

    default:
        CMD_IC_LOG_ERROR("Invalid IOCTL cmd %d", ic.cmd);
        ret = RDPA_DRV_ERROR;
    }

ioctl_exit:
    if (ret) {
        CMD_IC_LOG_ERROR("rdpa_cmd_ic_ioctl() OUT: FAILED: field(0x%x) rc(%d)", rule.field_mask, rc);
    }

    bdmf_unlock();
    return ret;
}

#define MAX_GEN_RULES   4

bdmf_object_handle ic_objs[MAX_GEN_RULES];

static uint8_t get_prty(void)
{
    uint8_t i;

    for (i = 0; i < MAX_GEN_RULES; i++)
    {
        if (ic_objs[i] == NULL)
            break;
    }

    return i;

}

static int parse_index(int32_t index, uint8_t *prty, bdmf_index *flow_idx, bdmf_object_handle *obj)
{
    *prty = index >> 10;
    *flow_idx = index & 0x3ff;

    if (*prty < 0 || *prty >= MAX_GEN_RULES)
    {
        CMD_IC_LOG_ERROR("Invalid index: %d", index);
        return -1;
    }

    *obj = ic_objs[*prty];
    if (*obj == NULL)
    {
        CMD_IC_LOG_ERROR("IC object not found for index: %d", index);
        return -1;
    }

    return 0;
}

int rdpa_cmd_ds_wan_udp_filter_ioctl(unsigned long arg)
{
    rdpa_drv_ioctl_ds_wan_udp_filter_t *user_ds_wan_udp_filter_p = (rdpa_drv_ioctl_ds_wan_udp_filter_t *)arg;
    rdpa_drv_ioctl_ds_wan_udp_filter_t ds_wan_udp_filter;
    int ret = 0;

    copy_from_user(&ds_wan_udp_filter, user_ds_wan_udp_filter_p, sizeof(rdpa_drv_ioctl_ds_wan_udp_filter_t));

    CMD_IC_LOG_DEBUG("RDPA DS_WAN_UDP_FILTER CMD: %d", ds_wan_udp_filter.cmd);

    bdmf_lock();

    switch(ds_wan_udp_filter.cmd)
    {
    case RDPA_IOCTL_DS_WAN_UDP_FILTER_CMD_ADD:
        {
            bdmf_object_handle ingress_class_obj;
            rdpactl_classification_rule_t rule;
            rdpa_ic_info_t ic_flow;
            bdmf_number ic_idx;
            bdmf_index flow_idx;
            int rc, is_add = 0;

            CMD_IC_LOG_DEBUG("RDPA_IOCTL_DS_WAN_UDP_FILTER_CMD_ADD");

            memset(&rule, 0, sizeof(rdpactl_classification_rule_t));
            rule.dscp_remark = -1;
            rule.opbit_remark = -1;
            rule.ipbit_remark = -1;
            rule.type = RDPACTL_IC_TYPE_IP_FLOW;
            rule.dir = rdpactl_dir_ds;
            rule.action = rdpa_forward_action_drop;
            rule.forw_mode = rdpa_forwarding_mode_flow;
            rule.qos_method = rdpa_qos_method_flow;
            rule.field_mask = RDPACTL_IC_MASK_GENERIC_1;
            rule.gen_rule_cfg1.type = RDPA_OFFSET_L4;
            rule.gen_rule_cfg1.offset = ds_wan_udp_filter.filter.offset;
            rule.gen_rule_key_1 = ds_wan_udp_filter.filter.value;
            rule.generic_mask = ds_wan_udp_filter.filter.mask;

            if (!find_ic(&rule, &ingress_class_obj, &ic_idx, &rule.prty)) 
            {
                rule.prty = get_prty();
                rc = add_ic(&rule, &ingress_class_obj);
                if (rc)
                {
                    CMD_IC_LOG_ERROR("Failed to create DS WAN UDP filter, rc=%d", rc);
                    ret = RDPA_DRV_IC_ERROR;
                    goto ioctl_exit;
                }
                else
                {
                    ic_objs[rule.prty] = ingress_class_obj;
                    is_add = 1;
                }
            }

            rc = create_ic_flow(1, &rule, &ic_flow, rule.dir);
            if (rc)
            {
                CMD_IC_LOG_ERROR("Failed to create IC flow, rc=%d", rc);
                ret = RDPA_DRV_IC_FLOW_ERROR;
                goto flow_error;
            }

            rdpactl_match_ic_flow(ingress_class_obj, &ic_flow, &flow_idx);
            if (flow_idx != BDMF_INDEX_UNASSIGNED)
            {
                CMD_IC_LOG_ERROR("IC flow already exists, flow_idx=%d", (int)flow_idx);
                ret = RDPA_DRV_IC_FLOW_ERROR;
                goto flow_error;
            }

            rc = rdpa_ingress_class_flow_add(ingress_class_obj, &flow_idx, &ic_flow);
            if (rc)
            {
                CMD_IC_LOG_ERROR("Failed to add IC flow, rc=%d", rc);
                ret = RDPA_DRV_IC_FLOW_ERROR;
                goto flow_error;
            }

            ds_wan_udp_filter.filter.index = (int32_t)rule.prty << 10 | (int32_t)flow_idx;

            copy_to_user(&user_ds_wan_udp_filter_p->filter.index, &ds_wan_udp_filter.filter.index, sizeof(ds_wan_udp_filter.filter.index));

flow_error:
            if (ret)
            {
                if (is_add) 
                    delete_ic(ingress_class_obj);
                else
                    bdmf_put(ingress_class_obj);
            }

            break;
        }
    case RDPA_IOCTL_DS_WAN_UDP_FILTER_CMD_DELETE:
        {
            uint8_t prty;
            bdmf_index flow_idx;
            bdmf_number nflows;
            bdmf_object_handle ingress_class_obj;
            int rc;

            CMD_IC_LOG_DEBUG("RDPA_IOCTL_DS_WAN_UDP_FILTER_CMD_DELETE: index=%d", ds_wan_udp_filter.filter.index);

            rc = parse_index(ds_wan_udp_filter.filter.index, &prty, &flow_idx, &ingress_class_obj);
            if (rc)
            {
                ret = RDPA_DRV_IC_ERROR;
                goto ioctl_exit;
            }

            rc = rdpa_ingress_class_flow_delete(ingress_class_obj, flow_idx);
            if (rc)
            {
                CMD_IC_LOG_ERROR("Cannot delete ingress_class flow: index=%d", ds_wan_udp_filter.filter.index);
                ret = RDPA_DRV_IC_FLOW_ERROR;
                goto ioctl_exit;
            }

            rdpa_ingress_class_nflow_get(ingress_class_obj, &nflows);
            if (nflows == 0) 
            {
                ic_objs[prty] = NULL;
                delete_ic(ingress_class_obj);
            }
            else
                bdmf_put(ingress_class_obj);

            break;
        }

    case RDPA_IOCTL_DS_WAN_UDP_FILTER_CMD_GET:
        {
            uint8_t prty;
            bdmf_index flow_idx;
            bdmf_object_handle ingress_class_obj;
            rdpa_ic_cfg_t cfg;
            rdpa_ic_info_t flow_info;
            rdpa_stat_t flow_stat;
            int rc;

            CMD_IC_LOG_DEBUG("RDPA_IOCTL_DS_WAN_UDP_FILTER_CMD_GET: index=%d", ds_wan_udp_filter.filter.index);

            rc = parse_index(ds_wan_udp_filter.filter.index, &prty, &flow_idx, &ingress_class_obj);
            if (rc)
            {
                ret = RDPA_DRV_IC_ERROR;
                goto ioctl_exit;
            }

            rc = rdpa_ingress_class_cfg_get(ingress_class_obj, &cfg);
            if (rc)
            {
                CMD_IC_LOG_ERROR("Cannot get IC configuration: prty %d, flow_idx %d rc %d", (int)prty, (int)flow_idx, rc); 
                ret = RDPA_DRV_IC_ERROR;
                goto ioctl_exit;
            }

            rc = rdpa_ingress_class_flow_get(ingress_class_obj, flow_idx, &flow_info);
            if (rc)
            {
                CMD_IC_LOG_ERROR("Cannot find ingress_class flow: prty %d, flow_idx %d rc %d", (int)prty, (int)flow_idx, rc); 
                ret = RDPA_DRV_IC_FLOW_ERROR;
                goto ioctl_exit;
            }

            rdpa_ingress_class_flow_stat_get(ingress_class_obj, flow_idx, &flow_stat);
            if (rc)
            {
                CMD_IC_LOG_ERROR("Cannot find ingress_class flow stat: prty %d, flow_idx %d rc %d", (int)prty, (int)flow_idx, rc); 
                ret = RDPA_DRV_IC_FLOW_ERROR;
                goto ioctl_exit;
            }

            ds_wan_udp_filter.filter.offset = cfg.gen_rule_cfg1.offset;
            ds_wan_udp_filter.filter.value = flow_info.key.generic_key_1;
            ds_wan_udp_filter.filter.mask = flow_info.key.generic_mask;
            ds_wan_udp_filter.filter.hits = flow_stat.packets;

            copy_to_user(user_ds_wan_udp_filter_p, &ds_wan_udp_filter, sizeof(rdpa_drv_ioctl_ds_wan_udp_filter_t));

            break;
        }
    default:
        {
            CMD_IC_LOG_ERROR("Invalid Command: %d", ds_wan_udp_filter.cmd);

            ret = -1;
        }
    }

ioctl_exit:
    bdmf_unlock();

    return ret;
}

/*******************************************************************************
 *
 * Function: rdpa_cmd_ic_init
 *
 * Initializes the RDPA IC API.
 *
 *******************************************************************************/
void rdpa_cmd_ic_init(void)
{
    CMD_IC_LOG_DEBUG("RDPA IC INIT");
}

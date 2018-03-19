/*
 * <:copyright-BRCM:2013:proprietary:standard
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

#include "bdmf_dev.h"
#include "rdd.h"
#ifndef G9991
#include "rdd_tunnels_parsing.h"
#endif
#include "rdd_tuple_lkp.h"
#include "rdd_platform.h"
#include "rdp_drv_natc.h"
#include "rdpa_ip_class_int.h"
#include "rdpa_rdd_map.h"
#include "rdd_ip_class.h"
#include "rdpa_api.h"
#include "rdpa_int.h"
#include "data_path_init.h"
#include "rdd_stubs.h"
#include "rdpa_port_int.h"

#ifdef __CYGWIN__
#include <cygwin/in.h>
#ifndef IPPROTO_GRE
#define IPPROTO_GRE 74
#endif
#endif

extern struct rdp_v6_subnets rdp_v6_subnets[rdpa_ds_lite_max_tunnel_id+1];

const bdmf_attr_enum_table_t rdpa_l4_filter_index_enum_table = {
    .type_name = "rdpa_l4_filter_index", .help = "L4 filter index (not supported yet)",
    .values = {
        {NULL, 0}
    }
};

/* ip flow key type to NATC mask map */
uint16_t rdpa_ip_class_key_to_mask_map[] = {0xc000, 0x8000, 0xef00};

#ifndef G9991
static uint32_t ds_lite_flows_num;
static uint32_t l2gre_flows_num;
#endif
extern natc_tbl_config_t g_natc_tbl_cfg[];
extern struct bdmf_object *ip_class_object;
#ifndef G9991
extern rdd_module_t tunnels_parsing;
#endif

extern int ip_class_prepare_rdd_ip_flow_params(rdpa_ip_flow_info_t *const info,
    rdd_fc_context_t *rdd_ip_flow_ctx, bdmf_boolean is_new_flow);

#define RDD_NATC_TABLE_SIZE_PER_DIRECTION  (RDD_CONTEXT_TABLE_SIZE/2)
#define GET_NATC_INDEX_FROM_BDMF_INDEX(index) (index < RDD_NATC_TABLE_SIZE_PER_DIRECTION ? index : index - RDD_NATC_TABLE_SIZE_PER_DIRECTION)
#define GET_TABLE_ACCORDING_TO_INDEX(index) (index < RDD_NATC_TABLE_SIZE_PER_DIRECTION ? tuple_lkp_ds_tbl : tuple_lkp_us_tbl)
#define SET_BDMF_INDEX_FROM_NATC_INDEX(index, dir) (dir ==  rdpa_dir_ds ? index : index + RDD_NATC_TABLE_SIZE_PER_DIRECTION)

static int _ip_class_read_rdd_ip_flow(uint32_t bdmf_idx, rdpa_ip_flow_info_t *info)
{
    uint8_t *keyword, *result, sub_tbl_id = 0;
    bdmf_boolean valid = 0;
    rdd_fc_context_t rdd_ip_flow_ctx = {};
    int rc = BDMF_ERR_OK;
    uint32_t index = GET_NATC_INDEX_FROM_BDMF_INDEX(bdmf_idx);
    uint8_t tbl_idx = GET_TABLE_ACCORDING_TO_INDEX(bdmf_idx);

    memset(info, 0, sizeof(*info));

    keyword = bdmf_alloc(g_natc_tbl_cfg[tbl_idx].key_len);
    result = bdmf_alloc(g_natc_tbl_cfg[tbl_idx].res_len);
    if (!keyword || !result)
    {
        rc = BDMF_ERR_INTERNAL;
        goto exit;
    }

    rc = drv_natc_key_entry_get(tbl_idx, index, &valid, keyword);
    if (rc)
        goto exit;
    rdd_ip_class_key_entry_decompose(&(info->key), &sub_tbl_id, keyword);
    if (sub_tbl_id)
    {
        rc = BDMF_ERR_NOENT;
        goto exit;
    }
    info->key.dir = tbl_idx;
    info->key.ingress_if = rdpa_port_vport_to_rdpa_if(info->key.ingress_if);

    rc = drv_natc_result_entry_get(tbl_idx, index, result);
    if (rc)
    {
        goto exit;
    }

    rdd_ip_class_result_entry_decompose(&rdd_ip_flow_ctx, result);

    info->result.qos_method = rdd_ip_flow_ctx.qos_method;
    info->result.action = rdd_ip_flow_ctx.actions_vector & rdpa_fc_action_forward ?
        (rdd_ip_flow_ctx.fwd_action == RDD_FC_FWD_ACTION_CPU ?
            rdpa_forward_action_host : rdpa_forward_action_drop) : rdpa_forward_action_forward;
    info->result.trap_reason = rdd_ip_flow_ctx.trap_reason;
    info->result.dscp_value = rdd_ip_flow_ctx.ip_version ? /* is_ipv6: compute TOS field */
        ((rdd_ip_flow_ctx.dscp_value << ECN_IN_TOS_SHIFT) | rdd_ip_flow_ctx.ecn_value) :
        rdd_ip_flow_ctx.dscp_value;
    info->result.nat_port = rdd_ip_flow_ctx.nat_port;
    if (!rdd_ip_flow_ctx.ip_version) /* if ipv4 */
        info->result.nat_ip.addr.ipv4 = rdd_ip_flow_ctx.nat_ip.addr.ipv4;
    if (rdd_ip_flow_ctx.to_lan)
        info->result.port = rdpa_port_vport_to_rdpa_if(rdd_ip_flow_ctx.tx_flow);
    else
    {
        if (rdpa_is_gbe_mode() || rdpa_is_epon_ae_mode())
        {
            info->result.wan_flow = 0;
            info->result.port = rdpa_port_vport_to_rdpa_if(rdd_ip_flow_ctx.tx_flow);
        }
        else
        {
            info->result.port = rdpa_if_wan0; /* FIXME: MULTI-WAN XPON */
            info->result.wan_flow = rdd_ip_flow_ctx.tx_flow;
        }
    }
    info->result.drop_eligibility = rdd_ip_flow_ctx.drop_eligibility;
    info->result.ssid = rdd_ip_flow_ctx.ssid;
    info->result.queue_id = BDMF_INDEX_UNASSIGNED;
    if (info->key.dir == rdpa_dir_ds)
    {
        _rdpa_egress_tm_queue_id_by_lan_port_qm_queue(info->result.port, rdd_ip_flow_ctx.traffic_class,
            &info->result.queue_id);
    }
    else
    {
        if (rdpa_if_is_wlan(info->result.port))
        {
           info->result.wl_metadata = rdd_ip_flow_ctx.wl_metadata;
            if (rdd_ip_flow_ctx.wfd.nic_ucast.is_wfd)
            {
                info->result.wl_accel_type = RDPA_WL_ACCEL_WFD;
                if (rdd_ip_flow_ctx.wfd.nic_ucast.is_chain)
                    info->result.queue_id = rdd_ip_flow_ctx.wfd.nic_ucast.priority;
                else
                    info->result.queue_id = rdd_ip_flow_ctx.wfd.dhd_ucast.priority;
            }
            else
            {
                info->result.wl_accel_type = RDPA_WL_ACCEL_DHD_OFFLOAD;
                info->result.queue_id = rdd_ip_flow_ctx.rnr.priority;
            }
        }
        else
        {
            int wan_flow = info->result.wan_flow;

            info->result.wl_accel_type = RDPA_WL_ACCEL_NONE;

            if (rdpa_if_is_lan(info->result.port)) /*check if exit port lan or wan*/
            {
                _rdpa_egress_tm_queue_id_by_lan_port_qm_queue(info->result.port, rdd_ip_flow_ctx.traffic_class,
                    &info->result.queue_id);
            }
            else
            {
                _rdpa_egress_tm_queue_id_by_wan_flow_qm_queue(&wan_flow, rdd_ip_flow_ctx.traffic_class,
                    &info->result.queue_id);
            }
            info->result.wan_flow = wan_flow;
        }
    }

    if (rdd_ip_flow_ctx.actions_vector & rdpa_fc_action_policer)
    {
        int policer_idx = rdd_ip_flow_ctx.policer_id;
        rdpa_policer_key_t key = { .dir = info->key.dir, .index = policer_idx };
        bdmf_object_handle policer;
        int rc;

        rc = rdpa_policer_get(&key, &policer);
        if (rc)
            BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Cannot find policer %d\n", policer_idx);
        bdmf_put(policer);

        info->result.policer_obj = policer;
    }

    if (rdpa_if_is_wan(info->result.port) &&  
       (rdd_ip_flow_ctx.actions_vector & rdpa_fc_action_dslite_tunnel))
    {
        info->result.ds_lite_src = rdp_v6_subnets[rdd_ip_flow_ctx.ds_lite_hdr_index].src;
        info->result.ds_lite_dst = rdp_v6_subnets[rdd_ip_flow_ctx.ds_lite_hdr_index].dst;
    }

    info->result.ovid_offset = rdd_ip_flow_ctx.ovid_offset;
    info->result.opbit_action = rdd_ip_flow_ctx.opbit_action;
    info->result.ipbit_action = rdd_ip_flow_ctx.ipbit_action;
    info->result.l2_header_offset = rdd_ip_flow_ctx.l2_hdr_offset;
    info->result.l2_header_size = rdd_ip_flow_ctx.l2_hdr_size;
    info->result.l2_header_number_of_tags = rdd_ip_flow_ctx.l2_hdr_number_of_tags;
    info->result.action_vec = rdd_ip_flow_ctx.actions_vector;
    info->result.action_vec &= ~rdpa_fc_action_policer; /* policer action is used only by RDD */
    info->result.service_q_id = BDMF_INDEX_UNASSIGNED;
    if (rdd_ip_flow_ctx.service_queue_enabled)
    {
        info->result.action_vec |= rdpa_fc_action_service_q;
        info->result.service_q_id = rdd_ip_flow_ctx.service_queue_id;
    }
    memcpy(info->result.l2_header, rdd_ip_flow_ctx.l2_header, rdd_ip_flow_ctx.l2_hdr_size);

exit:
    if (keyword)
        bdmf_free(keyword);
    if (result)
        bdmf_free(result);
    return rc;
}

int ip_class_attr_flow_delete_ex(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index bdmf_idx)
{
    int rc = BDMF_ERR_OK;
    uint32_t index = GET_NATC_INDEX_FROM_BDMF_INDEX(bdmf_idx);
    uint8_t tbl_idx = GET_TABLE_ACCORDING_TO_INDEX(bdmf_idx);

    rc = drv_natc_entry_delete(tbl_idx, index, 1);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "IP flow deletion failed, error=%d\n", rc);

    return 0;
}

void remove_all_flows(struct bdmf_object *mo)
{
    uint8_t *keyword = bdmf_alloc(g_natc_tbl_cfg[0].key_len);
    uint8_t tbl_id = 0;
    bdmf_boolean valid = 0;
    int i, rc;

    for (i = 0; i < RDD_NATC_TABLE_SIZE_PER_DIRECTION; i++)
    {
        drv_natc_key_entry_get(tuple_lkp_ds_tbl, i, &valid, keyword);
        rdd_ip_class_key_entry_decompose(NULL, &tbl_id, keyword);
        if (valid && !tbl_id)
        {
            rc = ip_class_attr_flow_delete(mo, NULL, i);
            if (rc)
                bdmf_trace("DS IP flow deletion failed, error=%d\n", rc);
        }

        drv_natc_key_entry_get(tuple_lkp_us_tbl, i, &valid, keyword);
        rdd_ip_class_key_entry_decompose(NULL, &tbl_id, keyword);
        if (valid && !tbl_id)
        {
            rc = ip_class_attr_flow_delete(mo, NULL, i + RDD_NATC_TABLE_SIZE_PER_DIRECTION);
            if (rc)
                bdmf_trace("US IP flow deletion failed, error=%d\n", rc);
        }
    }

    bdmf_free(keyword);
}

int ip_class_attr_flow_read(struct bdmf_object *mo, struct bdmf_attr *ad,
    bdmf_index bdmf_idx, void *val, uint32_t size)
{
    rdpa_ip_flow_info_t *info = (rdpa_ip_flow_info_t *)val;

    /* read the ip flow data from the RDD */
    return _ip_class_read_rdd_ip_flow(bdmf_idx, info);
}

int ip_class_attr_flow_write(struct bdmf_object *mo, struct bdmf_attr *ad,
    bdmf_index bdmf_idx, const void *val, uint32_t size)
{
    int rc = BDMF_ERR_OK;
    rdpa_ip_flow_info_t *info = (rdpa_ip_flow_info_t *)val;
    rdd_fc_context_t rdd_ip_flow_ctx = {};
    uint8_t result[NATC_MAX_ENTRY_LEN] = {};
    uint32_t index = GET_NATC_INDEX_FROM_BDMF_INDEX(bdmf_idx);
    uint8_t tbl_idx = GET_TABLE_ACCORDING_TO_INDEX(bdmf_idx);

    if (mo->state != bdmf_state_active)
        return BDMF_ERR_INVALID_OP;

    /* read the ip flow data from the RDD */
    rc = drv_natc_result_entry_get(tbl_idx, index, result);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_PERM, "Modifying ip flow retrieve failed, error %d\n", rc);
    rdd_ip_class_result_entry_decompose(&rdd_ip_flow_ctx, result);

    /* prepare ip flow data to modify in RDD */
    rc = ip_class_prepare_rdd_ip_flow_params(info, &rdd_ip_flow_ctx, 0);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_PERM, "Modifying ip flow parameters failed, error %d\n", rc);

    /* modify the ip flow in the RDD */
    rdd_ip_class_result_entry_compose(&rdd_ip_flow_ctx, result, info->key.dir);
    rc = drv_natc_result_entry_add(tbl_idx, index, result);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Modifying ip flow failed, error %d\n", rc);

    return 0;
}

static int ip_flow_key_validate(rdpa_ip_flow_key_t *key, rdd_fc_context_t *fc_ctx)
{
    if (key->prot != IPPROTO_TCP && key->prot != IPPROTO_UDP)
    {
        if (key->prot == IPPROTO_GRE)
        {
            if (key->src_port)
                BDMF_TRACE_RET(BDMF_ERR_PARM, "Source port specified for GRE protocol (%d)\n", key->prot);
        }
        else if (key->prot != IPPROTO_ESP && (key->src_port || key->dst_port))
        {
            BDMF_TRACE_RET(BDMF_ERR_PARM, "%s port specified for non TCP/UDP protocol (%d)\n",
                key->src_port ? "Source" : "Destination", key->prot);
        }
    }
    else
    {
        if (!key->src_port || !key->dst_port)
            BDMF_TRACE_RET(BDMF_ERR_PARM, "Missing %s port\n", key->dst_port ? "Source" : "Destination");
    }

    if (key->dst_ip.family != bdmf_ip_family_ipv4)
    {
        if (fc_ctx->actions_vector & rdpa_fc_action_nat)
            BDMF_TRACE_RET(BDMF_ERR_PARM, "Nat action is not valid for ipv6 connections\n");
        else if (key->dir == rdpa_dir_ds && (fc_ctx->actions_vector & rdpa_fc_action_dslite_tunnel))
            BDMF_TRACE_RET(BDMF_ERR_PARM, "Tunnel action is not valid for ipv6 DS connections\n");
    }
    return 0;
}

int ip_class_attr_flow_add_ex(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index *index, const void *val,
    uint32_t size)
{
    rdpa_ip_flow_info_t *info = (rdpa_ip_flow_info_t *)val;
    int rc;
#ifndef G9991
    bdmf_boolean tunnels_parsing_enable;
    uint8_t quad_idx;
#endif
    rdd_ip_flow_t rdd_ip_flow = {};
    uint8_t keyword[NATC_MAX_ENTRY_LEN] = {}, result[NATC_MAX_ENTRY_LEN] = {};

    rc = ip_class_prepare_rdd_ip_flow_params(info, &rdd_ip_flow.context_entry, 1);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_PERM, "Modifying ip flow parameters failed, error %d\n", rc);

    rc = ip_flow_key_validate(&info->key, &rdd_ip_flow.context_entry);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_PERM, "ip flow parameters validation failed, error %d\n", rc);

    /* remapping ingress_if and sending to rdd for compose */
    rc = rdd_ip_class_key_entry_compose(&info->key, keyword, (uint8_t)rdpa_port_rdpa_if_to_vport(info->key.ingress_if));
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_PERM, "Generating ip flow key entry failed, error %d\n", rc);

#ifndef G9991
    tunnels_parsing_enable = (rdd_ip_flow.context_entry.actions_vector &
                              (rdpa_fc_action_dslite_tunnel | rdpa_fc_action_l2gre_tunnel)) ? 1 : 0;

    if (info->key.dir == rdpa_dir_ds && tunnels_parsing_enable &&
        ds_lite_flows_num == 0 && l2gre_flows_num == 0)
    {
        rdd_tunnels_parsing_enable(&tunnels_parsing, 1);
    }

    if ((rdd_ip_flow.context_entry.actions_vector & rdpa_fc_action_dslite_tunnel) &&
        (ds_lite_flows_num == 0))
    {
        for (quad_idx = 0; !rc && quad_idx < NUM_OF_RNR_QUAD; quad_idx++)
            ag_drv_rnr_quad_parser_ip_protocol3_set(quad_idx, IPPROTO_IPIP);
    }
#endif
    if (info->key.dst_ip.family != bdmf_ip_family_ipv4)
        rdd_ip_flow.context_entry.nat_ip.addr.ipv4 = *(uint32_t *)&(info->key.dst_ip.addr.ipv6.data[12]);
    rdd_ip_flow.context_entry.ip_version = info->key.dst_ip.family;
    rdd_connection_checksum_delta_calc(&info->key, &rdd_ip_flow.context_entry);
    rdd_ip_class_result_entry_compose(&rdd_ip_flow.context_entry, result, info->key.dir);
    rc = drv_natc_key_result_entry_add(info->key.dir, keyword, result, &rdd_ip_flow.entry_index);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_PERM, "Adding connection to nat cache failed %d\n", rc);
    *index = SET_BDMF_INDEX_FROM_NATC_INDEX(rdd_ip_flow.entry_index, info->key.dir);
    return 0;
}

int ip_class_attr_flow_find(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index *index,
    void *val, uint32_t size)
{
    bdmf_error_t rc = BDMF_ERR_OK;
    rdpa_ip_flow_info_t *info = (rdpa_ip_flow_info_t *)val;
    uint8_t keyword[NATC_MAX_ENTRY_LEN] = {};
    uint32_t hash_index;

    /* remapping ingress_if and sending to rdd for compose */
    rc = rdd_ip_class_key_entry_compose(&info->key, keyword, (uint8_t)rdpa_port_rdpa_if_to_vport(info->key.ingress_if));
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_PERM, "Generating ip flow key entry failed, error %d\n", rc);
    rc = drv_natc_key_idx_get(info->key.dir, keyword, &hash_index, (uint32_t *)index);
    *index = SET_BDMF_INDEX_FROM_NATC_INDEX(*index, info->key.dir);
    return rc;
}

int ip_class_attr_flow_stat_read(struct bdmf_object *mo, struct bdmf_attr *ad,
    bdmf_index bdmf_idx, void *val, uint32_t size)
{
    rdpa_stat_t *stat = (rdpa_stat_t *)val;
    uint64_t hit_count = 0, byte_count = 0;
    bdmf_error_t rc = BDMF_ERR_OK;
    uint32_t index = GET_NATC_INDEX_FROM_BDMF_INDEX(bdmf_idx);
    uint8_t tbl_idx = GET_TABLE_ACCORDING_TO_INDEX(bdmf_idx);

    /* if the entry is in the NAT cache, then the statistics is also kept there */
    rc = drv_natc_entry_counters_get(tbl_idx, index, &hit_count, &byte_count);
    if (rc)
        return rc;

    /* XXX: need to change interface to work with 64bit counters */
    stat->packets = (uint32_t)hit_count;
    stat->bytes = (uint32_t)byte_count;
    return BDMF_ERR_OK;
}

int ip_class_attr_flow_status_read(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index bdmf_idx, void *val,
    uint32_t size)
{
    uint8_t rc;
    uint8_t *result = bdmf_alloc(g_natc_tbl_cfg[0].res_len);
    uint32_t index = GET_NATC_INDEX_FROM_BDMF_INDEX(bdmf_idx);
    uint8_t tbl_idx = GET_TABLE_ACCORDING_TO_INDEX(bdmf_idx);

    *(uint8_t *)val = 0;
    rc = drv_natc_result_entry_get(tbl_idx, index, result);
    bdmf_free(result);

    return rc;
}

/* "fc_bypass" attribute "write" callback */
int ip_class_attr_fc_bypass_write_ex(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    const void *val, uint32_t size)
{
    uint32_t mask = *(uint32_t *)val;
    ip_class_drv_priv_t *ip_class = (ip_class_drv_priv_t *)bdmf_obj_data(mo);

    /* Configure Pure IP mode */
    rdd_3_tupples_ip_flows_enable(mask & RDPA_IP_CLASS_MASK_PURE_IP ? 1 : 0);
    ip_class->fc_bypass_mask = mask;

    return 0;
}

/* "key mask" attribute "write" callback */
int ip_class_attr_key_type_write_ex(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    const void *val, uint32_t size)
{    
    bdmf_error_t rc;
    
    uint32_t key_type = *(uint32_t *)val;
    ip_class_drv_priv_t *ip_class = (ip_class_drv_priv_t *)bdmf_obj_data(mo);

    ip_class->ip_key_type = key_type;
    
    rc = drv_natc_set_key_mask(rdpa_ip_class_key_to_mask_map[key_type]);
    
    return rc;
}

int ip_class_attr_routed_mac_write_ex(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    const void *val, uint32_t size)
{
    ip_class_drv_priv_t *ip_class = (ip_class_drv_priv_t *)bdmf_obj_data(mo);
    bdmf_mac_t *mac = (bdmf_mac_t *)val;
    int rc;

    /* disable MAC entry before setting new one */
    rc = drv_rnr_quad_parser_da_filter_without_mask_set(ROUTER_DA_MAC_FILTER_GROUP, index, mac->b, 0);
    rc =  rc ? rc : drv_rnr_quad_parser_da_filter_without_mask_set(ROUTER_DA_MAC_FILTER_GROUP, index, mac->b, bdmf_mac_is_zero(mac) ? 0 : 1);
    if (rc)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_INTERNAL, mo, "Cannot set MAC %pM in Parser, error = %d\n", mac, rc);

    memcpy(&ip_class->routed_mac[index], mac, sizeof(bdmf_mac_t));
    return BDMF_ERR_OK;
}

void ip_class_pre_init_ex(struct bdmf_object *mo)
{
    ip_class_drv_priv_t *ip_class = (ip_class_drv_priv_t *)bdmf_obj_data(mo);
    
    ip_class->ip_key_type = RDPA_IP_CLASS_5TUPLE;
}

void ip_class_post_init_ex(struct bdmf_object *mo)
{
    ip_class_drv_priv_t *ip_class = (ip_class_drv_priv_t *)bdmf_obj_data(mo);
    
    drv_natc_set_key_mask(rdpa_ip_class_key_to_mask_map[ip_class->ip_key_type]);
}

void ip_class_destroy_ex(struct bdmf_object *mo)
{
}

int  ip_class_attr_l4_filter_stat_read(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    void *val, uint32_t size)
{
    return BDMF_ERR_NOT_SUPPORTED;
}

int ip_class_attr_l4_filter_cfg_write(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    const void *val, uint32_t size)
{
    rdpa_l4_filter_cfg_t *l4_filter_cfg = (rdpa_l4_filter_cfg_t *)val;
    rdd_action rdd_filter_action;

    /* Map action */
    if (l4_filter_cfg->action == rdpa_forward_action_drop)
        return BDMF_ERR_NOT_SUPPORTED;

    rdd_filter_action =
        (l4_filter_cfg->action == rdpa_forward_action_forward) ? ACTION_FORWARD : ACTION_TRAP;
 
    if (l4_filter_cfg->protocol == IPPROTO_ESP)
    {
        rdd_esp_filter_set(rdd_filter_action);
        return 0;
    }

    return BDMF_ERR_NOT_SUPPORTED;
}

int ip_class_attr_l4_filter_cfg_read(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index, void *val,
    uint32_t size)
{
    return BDMF_ERR_NOT_SUPPORTED;
}

int ip_flow_can_change_on_fly_params_ex(const rdpa_ip_flow_info_t *const info, rdd_fc_context_t *rdd_ip_flow_ctx)
{
    if (rdd_ip_flow_ctx->ssid != info->result.ssid ||
        (rdd_ip_flow_ctx->tx_flow != (info->key.dir == rdpa_dir_us ? info->result.wan_flow :
            rdpa_port_rdpa_if_to_vport(info->result.port))) ||
        (rdd_ip_flow_ctx->l2_hdr_offset != info->result.l2_header_offset + RDD_PACKET_HEADROOM_OFFSET)) 
    {
        return BDMF_ERR_PARM;
    }
    return 0;
}

void ip_class_prepare_new_rdd_ip_flow_params_ex(rdpa_ip_flow_info_t *const info,
    rdd_fc_context_t *rdd_ip_flow_ctx)
{
    rdd_ip_flow_ctx->phy_egress_port = (uint8_t)info->result.phy_port;
    rdd_ip_flow_ctx->ssid = info->result.ssid;

    rdd_ip_flow_ctx->tx_flow = (info->key.dir == rdpa_dir_ds) ? rdpa_port_rdpa_if_to_vport(info->result.port) :
        ((rdpa_if_is_lan_or_wifi(info->result.port)) ? rdpa_port_rdpa_if_to_vport(info->result.port) : info->result.wan_flow);
    rdd_ip_flow_ctx->to_lan = (rdpa_port_rdpa_if_to_vport(info->result.port) == rdpa_if_wan0) ? 0 : 1; /* FIXME: MULTI-WAN XPON */

    rdd_ip_flow_ctx->drop_eligibility = info->result.drop_eligibility;
    rdd_ip_flow_ctx->l2_hdr_offset = info->result.l2_header_offset + RDD_PACKET_HEADROOM_OFFSET;
}

void ip_class_rdd_ip_flow_cpu_vport_cfg_ex(rdpa_ip_flow_info_t *const info, rdd_fc_context_t *rdd_ip_flow_ctx)
{
    rdd_ip_flow_ctx->wl_metadata = info->result.wl_metadata;
    if (info->result.wfd.nic_ucast.is_wfd)
    {
        /* Use info->result.queue_id as skb mark */
        if (info->result.wfd.nic_ucast.is_chain)
        {
            rdd_ip_flow_ctx->wfd.nic_ucast.is_chain = 1;
            rdd_ip_flow_ctx->wfd.nic_ucast.priority = info->result.wfd.nic_ucast.priority;
        }
        else
        {
            rdd_ip_flow_ctx->wfd.dhd_ucast.is_chain = 0;
            rdd_ip_flow_ctx->wfd.dhd_ucast.priority = info->result.wfd.dhd_ucast.priority;
        }
    }
    else
    {
        rdd_ip_flow_ctx->rnr.priority = info->result.queue_id;
    }
}


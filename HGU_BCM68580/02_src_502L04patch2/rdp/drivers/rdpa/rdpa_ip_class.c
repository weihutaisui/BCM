/*
* <:copyright-BRCM:2013-2015:proprietary:standard
*
*    Copyright (c) 2013-2015 Broadcom
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
 :>
*/

#include "bdmf_dev.h"
#include "rdpa_api.h"
#include "rdd.h"
#include "rdpa_int.h"
#ifndef G9991
#include "rdd_tunnels_parsing.h"
#endif
#ifndef XRDP
#include "rdd_data_structures.h"
#include "rdpa_egress_tm_inline.h"
#ifndef LEGACY_RDP
#include "rdd_ip_flow.h"
#include "rdd_l4_filters.h"
#include "rdpa_rdd_map.h"
#else
#include "rdd_ih_defs.h"
#include "rdpa_rdd_inline.h"
#endif
#else
#include "rdpa_egress_tm_inline.h"
#include "rdpa_rdd_map.h"
#include "rdpa_rdd_inline.h"
#include "rdd_defs.h"
#include "rdd_ip_class.h"
#include "rdpa_filter.h"
#include "rdd_stubs.h"
#endif
#include "rdpa_ip_class_int.h"
#include "rdpa_port_int.h"
#include "rdpa_common.h"

#ifdef __CYGWIN__
#include <cygwin/in.h>
#ifndef IPPROTO_GRE
#define IPPROTO_GRE 74
#endif
#endif

struct bdmf_object *ip_class_object;

#define RDPA_FC_VARIABLE_ACTIONS (rdpa_fc_action_forward | rdpa_fc_action_dscp_remark | \
    rdpa_fc_action_gre_remark | rdpa_fc_action_opbit_remark | rdpa_fc_action_ipbit_remark | rdpa_fc_action_policer)

const bdmf_attr_enum_table_t rdpa_gre_type_enum_table =
{
    .type_name = "gre_tunnel_type", .help = "GRE l2 or l3",
    .values = {
        {"L2oGRE", rdpa_tunnel_l2gre},
        {"L3oGRE", rdpa_tunnel_l3gre},
        {NULL, 0}
    }
};

struct rdp_v6_subnets rdp_v6_subnets[rdpa_ds_lite_max_tunnel_id+1];

static DEFINE_BDMF_FASTLOCK(ip_class_lock);

extern int ip_class_attr_fc_bypass_write_ex(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    const void *val, uint32_t size);
extern int ip_class_attr_key_type_write_ex(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    const void *val, uint32_t size);    
int ip_class_attr_routed_mac_write_ex(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    const void *val, uint32_t size);
extern void ip_class_pre_init_ex(struct bdmf_object *mo);
extern void ip_class_post_init_ex(struct bdmf_object *mo);
extern void ip_class_destroy_ex(struct bdmf_object *mo);
extern int  ip_class_attr_l4_filter_stat_read(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    void *val, uint32_t size);
extern int ip_class_attr_l4_filter_cfg_write(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    const void *val, uint32_t size);
extern int ip_class_attr_l4_filter_cfg_read(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index, void *val,
    uint32_t size);

extern const bdmf_attr_enum_table_t rdpa_l4_filter_index_enum_table;

static inline int match_v6_subnet(const bdmf_ipv6_t *src, const bdmf_ipv6_t *dst, int subnet_idx)
{
    return !memcmp(src, &rdp_v6_subnets[subnet_idx].src, sizeof(bdmf_ipv6_t)) &&
        !memcmp(dst, &rdp_v6_subnets[subnet_idx].dst, sizeof(bdmf_ipv6_t));
}

int get_v6_subnet(const bdmf_ipv6_t *src, const bdmf_ipv6_t *dst)
{
    int i, free_idx = -1;

    /* lookup already assigned subnet */
    for (i = 0; i <= rdpa_ds_lite_max_tunnel_id; i++)
    {
        if (match_v6_subnet(src, dst, i))
            return i;

        if (rdp_v6_subnets[i].refcnt == 0)
            free_idx = i;
    }

    /* none found - assign new subnet on free slot */
    if (free_idx == -1)
        return -1; /* NO_FREE_SUBNETS */

    rdp_v6_subnets[free_idx].src = *src;
    rdp_v6_subnets[free_idx].dst = *dst;

#ifndef G9991
    rdd_ds_lite_tunnel_cfg(&rdp_v6_subnets[free_idx].src, &rdp_v6_subnets[free_idx].dst);
#endif

    return free_idx;
}

/* same custom user reasons can't be use by IP_CLASS/INGRESS_CLASS in parallel */
static bdmf_attr_enum_table_t rdpa_ip_flow_trap_reason_enum_table =
{
    .type_name = "flow_trap_reason",
    .values =
    {
        {"no_trap", 0},
        {"conn_trap0", rdpa_cpu_rx_reason_udef_0},
        {"conn_trap1", rdpa_cpu_rx_reason_udef_1},
        {"conn_trap2", rdpa_cpu_rx_reason_udef_2},
        {"conn_trap3", rdpa_cpu_rx_reason_udef_3},
        {"conn_trap4", rdpa_cpu_rx_reason_udef_4},
        {"conn_trap5", rdpa_cpu_rx_reason_udef_5},
        {"conn_trap6", rdpa_cpu_rx_reason_udef_6},
        {"conn_trap7", rdpa_cpu_rx_reason_udef_7},
        {NULL, 0}
    }
};

static const bdmf_attr_enum_table_t rdpa_fc_bypass_fields_enum_table =
{
    .type_name = "fc_bypass", .help = "FlowCache Bypass Modes",
    .values = {
        {"pure_mac", RDPA_IP_CLASS_PURE_MAC},
        {"multicast_ip", RDPA_IP_CLASS_MC_IP},
        {"pure_ip", RDPA_IP_CLASS_PURE_IP},
        {"us_wlan", RDPA_IP_CLASS_US_WLAN},
        {"bypass_all", RDPA_IP_CLASS_BP_ALL},
        {NULL, 0}
    }
};

static const bdmf_attr_enum_table_t rdpa_key_type_fields_enum_table =
{
    .type_name = "rdpa_key_type", .help = "Ip Flow Key mask",
    .values = {
        {"five_tuple", RDPA_IP_CLASS_5TUPLE},
        {"six_tuple", RDPA_IP_CLASS_6TUPLE},
        {"three_tuple", RDPA_IP_CLASS_3TUPLE},
        {NULL, 0}
    }
};


/* Vlan offset enum values */
static bdmf_attr_enum_table_t rdpa_vlan_offset_enum_table =
{
    .type_name = "vlan_offset",
    .values =
    {
        {"offset_12", rdpa_vlan_offset_12},
        {"offset_16", rdpa_vlan_offset_16},
        {NULL, 0}
    }
};

/* PBIT remark action enum values */
static bdmf_attr_enum_table_t rdpa_pbit_action_enum_table =
{
    .type_name = "pbit_remark_action",
    .values =
    {
        {"dscp_copy", rdpa_pbit_act_dscp_copy},
        {"outer_copy", rdpa_pbit_act_outer_copy},
        {"inner_copy", rdpa_pbit_act_inner_copy},
        {NULL, 0}
    }
};

const bdmf_attr_enum_table_t rdpa_fc_act_vect_enum_table =
{
    .type_name = "fc_action_vector", .help = "Vector of actions, relevant for Flow Cache only",
    .values =
    {
        {"no_fwd", rdpa_fc_act_forward},
        {"reserved", rdpa_fc_act_reserved},
        {"ttl", rdpa_fc_act_ttl},
        {"dscp", rdpa_fc_act_dscp_remark},
        {"nat", rdpa_fc_act_nat},
        {"gre", rdpa_fc_act_gre_remark},
        {"opbit", rdpa_fc_act_opbit_remark},
        {"ipbit", rdpa_fc_act_ipbit_remark},
        {"dslite_tunnel", rdpa_fc_act_dslite_tunnel},
        {"l2gre_tunnel", rdpa_fc_act_l2gre_tunnel},
        {"pppoe", rdpa_fc_act_pppoe},
        {"service_q", rdpa_fc_act_service_q},
        {"llc_snap_set_len", rdpa_fc_act_llc_snap_set_len},
        {"spdsvc", rdpa_fc_act_spdsvc},
        {NULL, 0}
    }
};

/***************************************************************************
 * RDD interface
 ***************************************************************************/
/***************************************************************************
 * ip_class object type
 **************************************************************************/

/* following parameters cannot change on the fly */
static int ip_flow_can_change_on_fly_params(const rdpa_ip_flow_info_t *const info, rdd_fc_context_t *rdd_ip_flow_ctx)
{
    int rc;

    rc = ip_flow_can_change_on_fly_params_ex(info, rdd_ip_flow_ctx);
    if (rc)
        return rc;

    if ((rdd_ip_flow_ctx->trap_reason != info->result.trap_reason) ||
        (rdd_ip_flow_ctx->nat_port != info->result.nat_port) ||
        (rdd_ip_flow_ctx->ip_version == bdmf_ip_family_ipv6 ?
        0 : (rdd_ip_flow_ctx->nat_ip.addr.ipv4 != info->result.nat_ip.addr.ipv4)) ||
        (rdd_ip_flow_ctx->ip_version != (info->key.dst_ip.family == bdmf_ip_family_ipv6)) ||
        (rdpa_if_is_wifi(info->result.port) &&
            (rdd_ip_flow_ctx->wl_metadata != info->result.wl_metadata ||
            (info->result.wl_accel_type == RDPA_WL_ACCEL_WFD && !rdd_ip_flow_ctx->wfd.nic_ucast.is_wfd) ||
            (info->result.wl_accel_type == RDPA_WL_ACCEL_DHD_OFFLOAD && rdd_ip_flow_ctx->rnr.is_wfd))) ||
        (rdd_ip_flow_ctx->ovid_offset != info->result.ovid_offset) ||
        (rdd_ip_flow_ctx->l2_hdr_number_of_tags != info->result.l2_header_number_of_tags) ||
        (rdd_ip_flow_ctx->l2_hdr_size != info->result.l2_header_size) ||
        ((rdd_ip_flow_ctx->actions_vector & ~RDPA_FC_VARIABLE_ACTIONS) !=
        (info->result.action_vec & ~RDPA_FC_VARIABLE_ACTIONS)) ||
        (memcmp(rdd_ip_flow_ctx->l2_header, info->result.l2_header, info->result.l2_header_size)))
    {
        return BDMF_ERR_PARM;
    }
    return 0;
}

/* converts from RDPA ip flow parameters to RDD ip flow parameters*/
int ip_class_prepare_rdd_ip_flow_params(rdpa_ip_flow_info_t *const info,
    rdd_fc_context_t *rdd_ip_flow_ctx, bdmf_boolean is_new_flow)
{
    int rc_id = 0, priority = 0;
    int rc = 0;

    /* check for service_q and modify action_vec before setting to runner*/
    if (info->result.action_vec & rdpa_fc_action_service_q)
    {
        rdd_ip_flow_ctx->service_queue_enabled = 1;
        rdd_ip_flow_ctx->service_queue_id = info->result.service_q_id;
    }
    else
        rdd_ip_flow_ctx->service_queue_enabled = 0;
    info->result.action_vec &= ~rdpa_fc_action_service_q;

    if (is_new_flow) /* when new flow is created all the parameters should be set */
    {
        /* prepare RDD context */
        if (info->result.action_vec & rdpa_fc_action_forward && info->result.action == rdpa_forward_action_host &&
            !info->result.trap_reason)
        {
            BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Undefined trap reason\n");
        }

        ip_class_prepare_new_rdd_ip_flow_params_ex(info, rdd_ip_flow_ctx);

        rdd_ip_flow_ctx->trap_reason = info->result.trap_reason;

        rdd_ip_flow_ctx->nat_port = info->result.nat_port;
#if defined(LEGACY_RDP) || defined(XRDP)
        rdd_ip_flow_ctx->drop_eligibility = info->result.drop_eligibility;
#endif

        rdd_ip_flow_ctx->ip_version = info->key.dst_ip.family;
        rdd_ip_flow_ctx->nat_ip.addr.ipv4 = (rdd_ip_flow_ctx->ip_version == bdmf_ip_family_ipv6) ?
            0 : info->result.nat_ip.addr.ipv4;

        rdd_ip_flow_ctx->ovid_offset = info->result.ovid_offset;

        rdd_ip_flow_ctx->l2_hdr_number_of_tags = info->result.l2_header_number_of_tags;
        rdd_ip_flow_ctx->l2_hdr_size = info->result.l2_header_size;
        rdd_ip_flow_ctx->actions_vector = info->result.action_vec;
        memcpy(rdd_ip_flow_ctx->l2_header, info->result.l2_header, info->result.l2_header_size);

        if (info->result.action_vec & rdpa_fc_action_dslite_tunnel)
        {
            /* DS-Lite */
            rdd_ip_flow_ctx->actions_vector |= rdpa_fc_action_dslite_tunnel;

            if (info->key.dir == rdpa_dir_us)
            {
                int v6_subnet_idx;

                if (bdmf_ipv6_is_zero(&info->result.ds_lite_src) || bdmf_ipv6_is_zero(&info->result.ds_lite_dst))
                    BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Undefined tunnel addresses\n");

                v6_subnet_idx = get_v6_subnet(&info->result.ds_lite_src, &info->result.ds_lite_dst);
                if (v6_subnet_idx < 0) /* will continue in software accel. */
                    BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "All IPv6 tunnel subnets are in use in firmware\n");

                rdd_ip_flow_ctx->ds_lite_hdr_index = v6_subnet_idx;
            }
        }
    }
    else
    {
        rc = ip_flow_can_change_on_fly_params(info, rdd_ip_flow_ctx);
        if (rc)
            return rc;

        rdd_ip_flow_ctx->actions_vector = info->result.action_vec;
    }

    rdd_ip_flow_ctx->qos_method = info->result.qos_method;

    if (info->result.action != rdpa_forward_action_forward)
        rdd_ip_flow_ctx->fwd_action = rdpa_fwd_act2rdd_fc_fwd_act[info->result.action];

    rdd_ip_flow_ctx->dscp_value = info->result.dscp_value;

    rdd_ip_flow_ctx->opbit_action = info->result.opbit_action;
    rdd_ip_flow_ctx->ipbit_action = info->result.ipbit_action;

    if (info->result.qos_method != rdpa_qos_method_pbit)
    {
        if (rdpa_if_is_lan(info->result.port)) /* WAN->LAN / WiFi->LAN / LAN->LAN */
        {
            rc = _rdpa_egress_tm_lan_port_queue_to_rdd(info->result.port, info->result.queue_id,
                &rc_id, &priority);
        }
        else if (!rdpa_if_is_cpu_port(info->result.port)) /* WAN */
        {
            /* tx_flow already configured for ds */
            int channel = 0;
            rc = _rdpa_egress_tm_wan_flow_queue_to_rdd(info->result.wan_flow, info->result.queue_id,
                &channel, &rc_id, &priority);
            if ((rdpa_is_epon_or_xepon_mode() || rdpa_is_epon_ae_mode()) && !rc)
#ifndef XRDP
                rdd_ip_flow_ctx->wan_flow_index = channel;
#else
                rdd_ip_flow_ctx->tx_flow = channel;
#endif
        }
    }

    if (rdpa_if_is_cpu_port(info->result.port))
    {
        rc = 0;
        rc_id = 0;
        priority = 0; /* cpu vport does not have egress_tm */
        info->result.wfd.nic_ucast.is_wfd = info->result.wl_accel_type == RDPA_WL_ACCEL_WFD;
        ip_class_rdd_ip_flow_cpu_vport_cfg_ex(info, rdd_ip_flow_ctx);
    }

    if (rc)
    {
        if (rdpa_if_is_wan(info->result.port))
        {
            BDMF_TRACE_RET(rc, "ip_class: egress queue %u is not configured on WAN flow %d\n",
                info->result.queue_id, (int)info->result.wan_flow);
        }
        else
        {
            BDMF_TRACE_RET(rc, "ip_class: egress queue %u is not configured on port %s\n",
                info->result.queue_id, bdmf_attr_get_enum_text_hlp(&rdpa_if_enum_table, info->result.port));
        }
    }

    rdd_ip_flow_ctx->rate_controller = rc_id;
    rdd_ip_flow_ctx->traffic_class = priority;

    BDMF_TRACE_INFO("ip_class: rc_id=%d traffic_class=%d\n", rc_id, priority);

    if (info->result.policer_obj)
    {
        bdmf_number policer_idx;
        int rc;
        rdpa_traffic_dir dir;

        rc = rdpa_policer_index_get(info->result.policer_obj, &policer_idx);
        if (rc)
            BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Cannot find policer %d\n", (int)policer_idx);

        rdpa_policer_dir_get(info->result.policer_obj, &dir);
        if (dir != info->key.dir)
            BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Given policer is in the opposite direction\n");

        rdd_ip_flow_ctx->policer_id = policer_idx;
        rdd_ip_flow_ctx->actions_vector |= rdpa_fc_action_policer;
    }
    else /* policer object destroyed */
        rdd_ip_flow_ctx->actions_vector &= ~rdpa_fc_action_policer;

    return 0;
}

/** This optional callback is called called at object init time
 *  before initial attributes are set.
 *  If function returns error code !=0, object creation is aborted
 */
static int ip_class_pre_init(struct bdmf_object *mo)
{
    int i;

    ip_class_drv_priv_t *ip_class = (ip_class_drv_priv_t *)bdmf_obj_data(mo);

    ip_class_pre_init_ex(mo);

    for (i = 0; i < RDPA_MAX_ROUTED_MAC; i++)
        memset(&ip_class->routed_mac[i], 0, sizeof(bdmf_mac_t));

    memset(ip_class->gre_tunnels, 0, sizeof(rdpa_gre_tunnel_cfg_t) * RDPA_MAX_GRE_TUNNELS);

    ip_class->num_flows = 0;
    DLIST_INIT(&ip_class->ip_flow_index_list);

    return BDMF_ERR_OK;
}

/** This optional callback is called at object init time
 * after initial attributes are set.
 * Its work is:
 * - make sure that all necessary attributes are set and make sense
 * - allocate dynamic resources if any
 * - assign object name if not done in pre_init
 * - finalise object creation
 * If function returns error code !=0, object creation is aborted
 */
static int ip_class_post_init(struct bdmf_object *mo)
{
    ip_class_drv_priv_t *ip_class = (ip_class_drv_priv_t *)bdmf_obj_data(mo);
#if !defined(XRDP)
    rdd_ic_context_t context = {};
    int rc;
#endif
    /* get the ip class method from the system */
    ip_class->op_method = _rdpa_system_init_cfg_get()->ip_class_method;

    /* if working with flow cache, configure internal l2 flow in RDD */
    if (ip_class->op_method != rdpa_method_none)
    {
#if !defined(XRDP)
        /* configure internal ingress context to RDD */
        context.action = rdpa_forward_action_forward;
        context.rate_shaper = BDMF_INDEX_UNASSIGNED;
        context.policer = BDMF_INDEX_UNASSIGNED;
        context.subnet_id = RDD_SUBNET_FLOW_CACHE;

        rc = rdd_ic_context_cfg(rdpa_dir_ds, RDPA_FC_DS_IC_RESULT_ID, &context);
        if (rc)
            BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Internal ds eth flow RDD configuration failed, error %d\n", rc);
#endif
        /* Configure Pure IP mode: Enabled */
        rdd_3_tupples_ip_flows_enable(1);

        ip_class->fc_bypass_mask |= RDPA_IP_CLASS_MASK_PURE_IP;
    }
    
    ip_class_post_init_ex(mo);
    
    ip_class_object = mo;
    /* set object name */
    snprintf(mo->name, sizeof(mo->name), "ip_class");
    /* ToDo: configure the filters  in RDD and IH */
    return 0;
}

static void ip_class_destroy(struct bdmf_object *mo)
{
    ip_class_destroy_ex(mo);

    remove_all_flows(mo);

    /* ToDo: do more cleanups here */
    ip_class_object = NULL;
}

/** find ip_class object */
static int ip_class_get(struct bdmf_type *drv, struct bdmf_object *owner, const char *discr, struct bdmf_object **pmo)
{
    if (!ip_class_object)
        return BDMF_ERR_NOENT;
    *pmo = ip_class_object;
    return 0;
}


#if defined LEGACY_RDP && defined XRDP
/* "gre_tunnels" attribute "read" callback */
static int ip_class_attr_gre_tunnel_cfg_read(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index, void *val,
    uint32_t size)
{
    ip_class_drv_priv_t *ip_class = (ip_class_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_gre_tunnel_cfg_t *gre_tunnel_cfg = (rdpa_gre_tunnel_cfg_t *)val;

    /*GRE tunnel source mac is the validity for the entry */
    if (bdmf_mac_is_zero(&ip_class->gre_tunnels[index].src_mac))
        return BDMF_ERR_NOENT;

    *gre_tunnel_cfg = ip_class->gre_tunnels[index];

    return 0;
}

static int ip_class_attr_gre_tunnel_cfg_write(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    const void *val, uint32_t size)
{
    ip_class_drv_priv_t *ip_class = (ip_class_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_gre_tunnel_cfg_t *gre_tunnel_cfg = (rdpa_gre_tunnel_cfg_t *)val;

    /*TODO: RDD to support ipv6 */
    rdd_gre_tunnel_cfg(index, &gre_tunnel_cfg->src_mac, &gre_tunnel_cfg->dst_mac, gre_tunnel_cfg->src_ip.addr.ipv4, gre_tunnel_cfg->dst_ip.addr.ipv4);

    ip_class->gre_tunnels[index] = *gre_tunnel_cfg;
    ip_class->gre_tunnels[index].ref_cnt = 0;

    return 0;
}
#endif

/* "fc_bypass" attribute "write" callback */
static int ip_class_attr_fc_bypass_write(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    const void *val, uint32_t size)
{
    return ip_class_attr_fc_bypass_write_ex(mo, ad, index, val, size);
}


/* "key_type" attribute "write" callback */
static int ip_class_attr_key_type_write(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    const void *val, uint32_t size)
{
    return ip_class_attr_key_type_write_ex(mo, ad, index, val, size);
}


/* "routed_mac" attribute "write" callback */
static int ip_class_attr_routed_mac_write(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    const void *val, uint32_t size)
{
    return ip_class_attr_routed_mac_write_ex(mo, ad, index, val, size);
}

/* "routed_mac" attribute "read" callback */
static int ip_class_attr_routed_mac_read(struct bdmf_object *mo, struct bdmf_attr *ad,
    bdmf_index index, void *val, uint32_t size)
{
    ip_class_drv_priv_t *ip_class = (ip_class_drv_priv_t *)bdmf_obj_data(mo);
    bdmf_mac_t *mac = (bdmf_mac_t *)val;

    *mac = ip_class->routed_mac[index];
    return 0;
}

/* "flush" attribute "write" callback */
static int ip_class_attr_flush_write(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index, const void *val,
    uint32_t size)
{
    remove_all_flows(mo);
    return BDMF_ERR_OK;
}

#define ip_class_indexes_dump(ip_class) _ip_class_indexes_dump(__FUNCTION__, __LINE__, ip_class)
static void _ip_class_indexes_dump(const char *func, int line, ip_class_drv_priv_t *ip_class)
{
    ip_flow_index_list_entry_t *entry;

    if (bdmf_global_trace_level < bdmf_trace_level_debug)
        return;

    bdmf_trace("%s:%d, Total %d indecies\n====================================\n", func, line, ip_class->num_flows);
    DLIST_FOREACH(entry, &ip_class->ip_flow_index_list, list)
        bdmf_trace("%d => ", (int)entry->index);
    bdmf_trace("\n");
}

int ip_class_attr_flow_add(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index *index, const void *val,
    uint32_t size)
{
    int rc;
    ip_flow_index_list_entry_t *entry;
    ip_class_drv_priv_t *ip_class = (ip_class_drv_priv_t *)bdmf_obj_data(mo);

    if (ip_class->num_flows >= 16*1024)
        BDMF_TRACE_RET(BDMF_ERR_NORES, "Too many connections\n");

    rc = ip_class_attr_flow_add_ex(mo, ad, index, val, size);
    if (rc)
        return rc;

    entry = (ip_flow_index_list_entry_t *)bdmf_calloc(sizeof(ip_flow_index_list_entry_t));
    if (!entry)
    {
        ip_class_attr_flow_delete_ex(mo, ad, *index);
        return BDMF_ERR_NOMEM;
    }

    entry->index = *index;

    bdmf_fastlock_lock(&ip_class_lock);
    ip_class->num_flows++;

    DLIST_INSERT_HEAD(&ip_class->ip_flow_index_list, entry, list);
    bdmf_fastlock_unlock(&ip_class_lock);
    ip_class_indexes_dump(ip_class);

    return 0;
}

int ip_class_attr_flow_delete(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index)
{
    int rc;
    ip_flow_index_list_entry_t *entry;
    ip_class_drv_priv_t *ip_class = (ip_class_drv_priv_t *)bdmf_obj_data(mo);

    rc = ip_class_attr_flow_delete_ex(mo, ad, index);
    if (rc)
        return rc;

    bdmf_fastlock_lock(&ip_class_lock);

    DLIST_FOREACH(entry, &ip_class->ip_flow_index_list, list)
    {
        if (entry->index == index)
        {
            DLIST_REMOVE(entry, list);
            bdmf_free(entry);
            break;
        }
    }
    ip_class->num_flows--;
    bdmf_fastlock_unlock(&ip_class_lock);
    BUG_ON(ip_class->num_flows < 0);
    ip_class_indexes_dump(ip_class);

    return 0;
}

static int ip_class_attr_flow_get_next(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index *index)
{
    ip_flow_index_list_entry_t *entry;
    ip_class_drv_priv_t *ip_class = (ip_class_drv_priv_t *)bdmf_obj_data(mo);

    if (!index)
        return BDMF_ERR_INTERNAL;

    if (DLIST_EMPTY(&ip_class->ip_flow_index_list))
        return BDMF_ERR_NO_MORE;

    if (*index == BDMF_INDEX_UNASSIGNED)
    {
        entry = DLIST_FIRST(&ip_class->ip_flow_index_list);
        *index = entry->index;
        return 0;
    }

    DLIST_FOREACH(entry, &ip_class->ip_flow_index_list, list)
    {
        if (entry->index == *index)
            break;
    }
    if (entry)
        entry = DLIST_NEXT(entry, list);
    if (entry)
    {
        *index = entry->index;
        return 0;
    }
    return BDMF_ERR_NO_MORE;
}

/*  ip_flow_key aggregate type */
struct bdmf_aggr_type ip_flow_key_type = {
    .name = "ip_flow_key", .struct_name = "rdpa_ip_flow_key_t",
    .help = "IP Flow Key",
    .fields = (struct bdmf_attr[])
    {
        { .name = "src_ip", .help = "Source IPv4/IPv6 address", .size = sizeof(bdmf_ip_t),
            .type = bdmf_attr_ip_addr, .offset = offsetof(rdpa_ip_flow_key_t, src_ip)
        },
        { .name = "dst_ip", .help = "Destination IPv4/IPv6 address", .size = sizeof(bdmf_ip_t),
            .type = bdmf_attr_ip_addr, .offset = offsetof(rdpa_ip_flow_key_t, dst_ip)
        },
        { .name = "prot", .help = "IP protocol",
            .type = bdmf_attr_number, .size = sizeof(uint8_t), .offset = offsetof(rdpa_ip_flow_key_t, prot),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "src_port", .help = "Source port", .size = sizeof(uint16_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_ip_flow_key_t, src_port),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "dst_port", .help = "Destination port", .size = sizeof(uint16_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_ip_flow_key_t, dst_port),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "dir", .help = "Traffic direction",
            .type = bdmf_attr_enum, .ts.enum_table = &rdpa_traffic_dir_enum_table,
            .size = sizeof(rdpa_traffic_dir), .offset = offsetof(rdpa_ip_flow_key_t, dir)
        },
        { .name = "ingress_if", .help = "Ingress interface", 
            .type = bdmf_attr_enum, .ts.enum_table = &rdpa_if_enum_table,
            .size = sizeof(rdpa_if), .offset = offsetof(rdpa_ip_flow_key_t, ingress_if)
        },
        BDMF_ATTR_LAST
    },
};
DECLARE_BDMF_AGGREGATE_TYPE(ip_flow_key_type);

/*  ip_flow_result aggregate type */
struct bdmf_aggr_type ip_flow_result_type =
{
    .name = "ip_flow_result", .struct_name = "rdpa_ip_flow_result_t",
    .help = "IP Flow Result",
    .fields = (struct bdmf_attr[])
    {
        { .name = "qos_method", .help = "QoS classification method",
            .type = bdmf_attr_enum, .ts.enum_table = &rdpa_qos_method_enum_table,
            .size = sizeof(rdpa_qos_method), .offset = offsetof(rdpa_ip_flow_result_t , qos_method)
        },
        { .name = "action", .help = "Forwarding action",
            .type = bdmf_attr_enum, .ts.enum_table = &rdpa_forward_action_enum_table,
            .size = sizeof(rdpa_forward_action), .offset = offsetof(rdpa_ip_flow_result_t, action)
        },
        { .name = "trap_reason", .help = "Trap reason",
            .type = bdmf_attr_enum, .ts.enum_table = &rdpa_ip_flow_trap_reason_enum_table,
            .size = sizeof(rdpa_cpu_reason), .offset = offsetof(rdpa_ip_flow_result_t, trap_reason),
        },
        { .name = "dscp_value", .help = "DSCP value", .size = sizeof(rdpa_dscp),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_ip_flow_result_t, dscp_value),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "nat_port", .help = "NAT port", .size = sizeof(uint16_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_ip_flow_result_t, nat_port),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "nat_ip", .help = "NAT IPv4/IPv6 address", .size = sizeof(bdmf_ip_t),
            .type = bdmf_attr_ip_addr, .offset = offsetof(rdpa_ip_flow_result_t, nat_ip)
        },
        { .name = "dslite_src", .help = "DS-Lite source tunnel address", .size = sizeof(bdmf_ipv6_t),
            .type = bdmf_attr_ipv6_addr, .offset = offsetof(rdpa_ip_flow_result_t, ds_lite_src)
        },
        { .name = "dslite_dst", .help = "DS-Lite destination tunnel address", .size = sizeof(bdmf_ipv6_t),
            .type = bdmf_attr_ipv6_addr, .offset = offsetof(rdpa_ip_flow_result_t, ds_lite_dst)
        },
        { .name = "policer", .help = "Policer ID", .size = sizeof(bdmf_object_handle), .ts.ref_type_name = "policer",
            .type = bdmf_attr_object, .offset = offsetof(rdpa_ip_flow_result_t, policer_obj)
        },
        { .name = "port", .help = "Egress port",
            .type = bdmf_attr_enum, .ts.enum_table = &rdpa_if_enum_table,
            .size = sizeof(rdpa_if), .offset = offsetof(rdpa_ip_flow_result_t, port),
        },
        { .name = "ssid", .help = "If port is CPU vport for wlan",
            .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_ip_flow_result_t, ssid),
            .flags = BDMF_ATTR_UNSIGNED, .min_val = 0, .max_val = 15
        },
        { .name = "phys_port", .help = "physical port",
            .type = bdmf_attr_enum, .ts.enum_table = &rdpa_emac_enum_table,
            .size = sizeof(rdpa_emac), .offset = offsetof(rdpa_ip_flow_result_t, phy_port),
        },
        { .name = "queue_id", .help = "Egress queue id", .size = sizeof(uint32_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_ip_flow_result_t, queue_id),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "wan_flow", .help = "US gem flow or DSL status", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_ip_flow_result_t, wan_flow),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "ovid_offset", .help = "Outer VID offset",
            .type = bdmf_attr_enum, .ts.enum_table = &rdpa_vlan_offset_enum_table,
            .size = sizeof(rdpa_vlan_offset), .offset = offsetof(rdpa_ip_flow_result_t, ovid_offset),
        },
        { .name = "opbit_action", .help = "Packet based outer pbit remarking action",
            .type = bdmf_attr_enum, .ts.enum_table = &rdpa_pbit_action_enum_table,
            .size = sizeof(rdpa_vlan_offset), .offset = offsetof(rdpa_ip_flow_result_t, opbit_action),
        },
        { .name = "ipbit_action", .help = "Packet based inner pbit remarking action",
            .type = bdmf_attr_enum, .ts.enum_table = &rdpa_pbit_action_enum_table,
            .size = sizeof(rdpa_vlan_offset), .offset = offsetof(rdpa_ip_flow_result_t, ipbit_action),
        },
        { .name = "l2_offset", .help = "Offset of L2 header", .size = sizeof(int8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_ip_flow_result_t, l2_header_offset)
        },
        { .name = "l2_head_size", .help = "Size of L2 header in bytes", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_ip_flow_result_t, l2_header_size),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "l2_num_tags", .help = "L2 header number of tags", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_ip_flow_result_t, l2_header_number_of_tags),
        },
        { .name = "action_vec", .help = "Vector of actions to perfrom on the flow", .size = sizeof(uint16_t),
            .type = bdmf_attr_enum_mask, .ts.enum_table = &rdpa_fc_act_vect_enum_table,
            .offset = offsetof(rdpa_ip_flow_result_t, action_vec)
        },
        { .name = "l2_header", .help = "L2 header in egress", .size = RDPA_L2_HEADER_SIZE,
            .type = bdmf_attr_buffer, .offset = offsetof(rdpa_ip_flow_result_t, l2_header)
        },
        { .name = "wl_accel_type", .help = "WLAN acceleration type", .type = bdmf_attr_enum,
            .ts.enum_table = &rdpa_wl_accel_enum_table,
            .offset = offsetof(rdpa_ip_flow_result_t, wl_accel_type),
        },
        { .name = "wl_metadata", .help = "WL Metadata (in use for some WiFi acceleration techniques)",
            .size = sizeof(uint32_t), .type = bdmf_attr_number, .flags = BDMF_ATTR_HEX_FORMAT,
            .offset = offsetof(rdpa_ip_flow_result_t, wl_metadata)
        },
        { .name = "service_queue_id", .help = "service queue id", .size = sizeof(bdmf_index),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_ip_flow_result_t, service_q_id),
            .flags = BDMF_ATTR_HAS_DISABLE, .disable_val = BDMF_INDEX_UNASSIGNED,
        },
        { .name = "drop_eligibility", .help = "Drop eligibility indicator[1:0] 00/01= disable 10=non drop eligible(WRED high priority), 11=drop eligible(WRED low priority)",
            .size = sizeof(uint8_t), .type = bdmf_attr_number, .offset = offsetof(rdpa_ip_flow_result_t, drop_eligibility),
            .flags = BDMF_ATTR_UNSIGNED
        },
#ifdef LEGACY_RDP
        { .name = "gre_tunnel", .help = "GRE tunnel index to be used in this flow",
          .size = sizeof(uint8_t), .type = bdmf_attr_number, .offset = offsetof(rdpa_ip_flow_result_t, gre_tunnel_idx),
          .flags = BDMF_ATTR_UNSIGNED
        },
#endif
        BDMF_ATTR_LAST
    },
};
DECLARE_BDMF_AGGREGATE_TYPE(ip_flow_result_type);

/*  ip_flow_info aggregate type */
struct bdmf_aggr_type ip_flow_info_type = {
    .name = "ip_flow_info", .struct_name = "rdpa_ip_flow_info_t",
    .help = "Fast IP Connection Info (key+result)",
    .fields = (struct bdmf_attr[]) {
        { .name = "key", .help = "IP flow key", .type = bdmf_attr_aggregate,
            .ts.aggr_type_name = "ip_flow_key", .offset = offsetof(rdpa_ip_flow_info_t, key)
        },
        { .name = "result", .help = "IP flow result", .type = bdmf_attr_aggregate,
            .ts.aggr_type_name = "ip_flow_result", .offset = offsetof(rdpa_ip_flow_info_t, result)
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE(ip_flow_info_type);

/*  l4_filter_cfg aggregate type : l4 filter configuration */
/* not used for XRDP but must be decleared */
struct bdmf_aggr_type l4_filter_cfg_type = {
    .name = "l4_filter_cfg", .struct_name = "rdpa_l4_filter_cfg_t",
    .help = "L4 Protocol Filter Configuration",
    .fields = (struct bdmf_attr[]) {
        { .name = "action", .help = "Filter action", .type = bdmf_attr_enum,
            .ts.enum_table = &rdpa_forward_action_enum_table,
            .size = sizeof(rdpa_filter_action), .offset = offsetof(rdpa_l4_filter_cfg_t, action)
        },
        { .name = "protocol", .help = "IP protocol", .type = bdmf_attr_number,
            .flags = BDMF_ATTR_HEX_FORMAT | BDMF_ATTR_HAS_DISABLE, .disable_val = RDPA_INVALID_PROTOCOL,
            .size = sizeof(uint8_t), .offset = offsetof(rdpa_l4_filter_cfg_t, protocol)
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE(l4_filter_cfg_type);

#ifdef LEGACY_RDP
/*  gre_tunnel_cfg aggregate type : gre tunnel configuration */
static struct bdmf_aggr_type gre_tunnel_cfg_type = {
    .name = "gre_tunnel_cfg", .struct_name = "rdpa_gre_tunnel_cfg_t",
    .help = "GRE Tunnel Configuration",
    .fields = (struct bdmf_attr[]) {
        { .name = "gre_type", .help = "GRE L2 or L3", .type = bdmf_attr_enum,
            .ts.enum_table = &rdpa_gre_type_enum_table,
            .size = sizeof(rdpa_tunnel_gre_type), .offset = offsetof(rdpa_gre_tunnel_cfg_t, gre_type)
        },
        { .name = "src_mac", .help = "Tunnel Source MAC", .type = bdmf_attr_ether_addr,
            .size = sizeof(bdmf_mac_t), .offset = offsetof(rdpa_gre_tunnel_cfg_t, src_mac)
        },
        { .name = "dst_mac", .help = "Tunnel Destination MAC", .type = bdmf_attr_ether_addr,
            .size = sizeof(bdmf_mac_t), .offset = offsetof(rdpa_gre_tunnel_cfg_t, dst_mac)
        },
        { .name = "src_ip", .help = "Tunnel Source IPv4/IPv6 address", .size = sizeof(bdmf_ip_t),
            .type = bdmf_attr_ip_addr, .offset = offsetof(rdpa_gre_tunnel_cfg_t, src_ip)
        },
        { .name = "dst_ip", .help = "Tunnel Destination IPv4/IPv6 address", .size = sizeof(bdmf_ip_t),
            .type = bdmf_attr_ip_addr, .offset = offsetof(rdpa_gre_tunnel_cfg_t, dst_ip)
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE(gre_tunnel_cfg_type);
#endif

/* Object attribute descriptors */
static struct bdmf_attr ip_class_attrs[] = {
    { .name = "nflows", .help = "number of configured 5-tuple based IP flows",
        .type = bdmf_attr_number, .flags = BDMF_ATTR_READ | BDMF_ATTR_CONFIG,
        .size = sizeof(uint32_t), .offset = offsetof(ip_class_drv_priv_t, num_flows)
    },
    { .name = "flow", .help = "5-tuple based IP flow entry",
        .type = bdmf_attr_aggregate, .ts.aggr_type_name = "ip_flow_info", .array_size = RDPA_MAX_IP_FLOW,
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG | BDMF_ATTR_NOLOCK,
        .read = ip_class_attr_flow_read, .write = ip_class_attr_flow_write,
        .add = ip_class_attr_flow_add, .del = ip_class_attr_flow_delete,
        .find = ip_class_attr_flow_find, .get_next = ip_class_attr_flow_get_next
    },
    { .name = "flow_stat", .help = "5-tuple based IP flow entry statistics",
        .type = bdmf_attr_aggregate, .ts.aggr_type_name = "rdpa_stat", .array_size = RDPA_MAX_IP_FLOW,
        .flags = BDMF_ATTR_READ | BDMF_ATTR_STAT | BDMF_ATTR_NOLOCK,
        .read = ip_class_attr_flow_stat_read, .get_next = ip_class_attr_flow_get_next
    },
    { .name = "flush", .help = "Flush flows", .size = sizeof(bdmf_boolean),
        .flags = BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG,
        .type = bdmf_attr_boolean, .write = ip_class_attr_flush_write
    },
    { .name = "l4_filter", .help = "L4 filter configuration",
        .type = bdmf_attr_aggregate, .ts.aggr_type_name = "l4_filter_cfg",
        .index_type = bdmf_attr_enum, .index_ts.enum_table = &rdpa_l4_filter_index_enum_table,
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG, .array_size = RDPA_MAX_L4_FILTERS,
        .read = ip_class_attr_l4_filter_cfg_read, .write = ip_class_attr_l4_filter_cfg_write
    },
    { .name = "l4_filter_stat", .help = "L4 filter statistics",
        .index_type = bdmf_attr_enum, .index_ts.enum_table = &rdpa_l4_filter_index_enum_table,
        .type = bdmf_attr_number, .size = sizeof(uint32_t), .data_type_name = "uint32_t",
        .array_size = RDPA_MAX_L4_FILTERS,
        .flags = BDMF_ATTR_READ | BDMF_ATTR_STAT, .read = ip_class_attr_l4_filter_stat_read
    },
    { .name = "routed_mac", .help = "Router MAC address", .type = bdmf_attr_ether_addr,
        .array_size = RDPA_MAX_ROUTED_MAC,
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG | BDMF_ATTR_NO_NULLCHECK,
        .size = sizeof(bdmf_mac_t), .offset = offsetof(ip_class_drv_priv_t, routed_mac),
        .read = ip_class_attr_routed_mac_read, .write = ip_class_attr_routed_mac_write
    },
    { .name = "fc_bypass", .help = "FlowCache Bypass Modes", .type = bdmf_attr_enum_mask,
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG,
        .ts.enum_table = &rdpa_fc_bypass_fields_enum_table,
        .size = sizeof(rdpa_fc_bypass), .offset = offsetof(ip_class_drv_priv_t , fc_bypass_mask),
        .write = ip_class_attr_fc_bypass_write, .data_type_name = "rdpa_fc_bypass"
    },
    { .name = "key_type", .help = "IP class key type", .type = bdmf_attr_enum,
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG,
        .ts.enum_table = &rdpa_key_type_fields_enum_table,
        .size = sizeof(rdpa_key_type), .offset = offsetof(ip_class_drv_priv_t , ip_key_type),
        .write = ip_class_attr_key_type_write, .data_type_name = "rdpa_key_type"
    },
#if defined LEGACY_RDP && defined XRDP
    { .name = "gre_tunnels", .help = "gre tunnel configuration",
        .type = bdmf_attr_aggregate, .ts.aggr_type_name = "gre_tunnel_cfg",
        .index_type = bdmf_attr_number,
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG, .array_size = RDPA_MAX_GRE_TUNNELS,
        .read = ip_class_attr_gre_tunnel_cfg_read, .write = ip_class_attr_gre_tunnel_cfg_write
    },
#endif
    BDMF_ATTR_LAST
};


static int ip_class_drv_init(struct bdmf_type *drv);
static void ip_class_drv_exit(struct bdmf_type *drv);

struct bdmf_type ip_class_drv = {
    .name = "ip_class",
    .parent = "system",
    .description = "IP Flow Classifier",
    .drv_init = ip_class_drv_init,
    .drv_exit = ip_class_drv_exit,
    .pre_init = ip_class_pre_init,
    .post_init = ip_class_post_init,
    .destroy = ip_class_destroy,
    .get = ip_class_get,
    .extra_size = sizeof(ip_class_drv_priv_t),
    .aattr = ip_class_attrs,
    .max_objs = 1,
};
DECLARE_BDMF_TYPE(rdpa_ip_class, ip_class_drv);

/* Init/exit module. Cater for GPL layer */
static int ip_class_drv_init(struct bdmf_type *drv)
{
#ifdef BDMF_DRIVER_GPL_LAYER
    f_rdpa_ip_class_drv = rdpa_ip_class_drv;
    f_rdpa_ip_class_get = rdpa_ip_class_get;
#endif
    return 0;
}

static void ip_class_drv_exit(struct bdmf_type *drv)
{
#ifdef BDMF_DRIVER_GPL_LAYER
    f_rdpa_ip_class_drv = NULL;
    f_rdpa_ip_class_get = NULL;
#endif
}

/***************************************************************************
 * Functions declared in auto-generated header
 **************************************************************************/

/** Get ip_class object by key
 * \return  Object handle or NULL if not found
 */
int rdpa_ip_class_get(bdmf_object_handle *_obj_)
{
    if (!ip_class_object)
        return BDMF_ERR_NOENT;
    bdmf_get(ip_class_object);
    *_obj_ = ip_class_object;
    return 0;
}


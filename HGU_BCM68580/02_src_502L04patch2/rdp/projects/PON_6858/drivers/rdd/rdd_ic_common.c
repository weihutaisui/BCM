/*
    <:copyright-BRCM:2016:DUAL/GPL:standard
    
       Copyright (c) 2016 Broadcom 
       All Rights Reserved
    
    Unless you and Broadcom execute a separate written software license
    agreement governing use of this software, this software is licensed
    to you under the terms of the GNU General Public License version 2
    (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
    with the following added to such license:
    
       As a special exception, the copyright holders of this software give
       you permission to link this software with independent modules, and
       to copy and distribute the resulting executable under terms of your
       choice, provided that you also meet, for each linked independent
       module, the terms and conditions of the license of that module.
       An independent module is a module which is not derived from this
       software.  The special exception does not apply to any modifications
       of the software.
    
    Not withstanding the above, under no circumstances may you combine
    this software in any way with any other Broadcom software provided
    under a license other than the GPL, without Broadcom's express prior
    written consent.
    
:>
*/

#include "rdd.h"
#include "rdd_tcam_ic.h"
#include "rdd_data_structures_auto.h"
#include "rdd_ic_common.h"
#include "rdd.h"

void rdd_ic_result_entry_compose(uint16_t index, const rdd_ic_context_t *ctx, uint8_t *entry)
{
    RDD_RULE_BASED_CONTEXT_ENTRY_DTS context_entry = {};

    context_entry.queue = ctx->priority;
    context_entry.to_lan = ctx->to_lan;
    context_entry.flow_or_port =  context_entry.to_lan ? ctx->egress_port : ctx->tx_flow;
    context_entry.rule = index;
    context_entry.pbit_to_q = ctx->qos_method == rdpa_qos_method_pbit;
    context_entry.tc_to_q = (ctx->forw_mode != rdpa_forwarding_mode_flow)
        && (ctx->qos_method == rdpa_qos_method_flow);
    context_entry.pbit_to_gem = ctx->gem_mapping_mode;
    context_entry.flow_based_forward = 
        (ctx->forw_mode == rdpa_forwarding_mode_flow);
    if (context_entry.pbit_to_gem == rdpa_qos_method_pbit)
        context_entry.gem_mapping_table = ctx->gem_mapping_table;

    context_entry.pbit_remark = (ctx->opbit_remark | ctx->ipbit_remark);

    context_entry.cntr_id = ctx->cntr_id;
    context_entry.policer_id = ctx->policer;
    context_entry.qos_rule_wan_flow_overrun = ctx->qos_rule_wan_flow_overrun;
    context_entry.opbit_remark_mode = ctx->opbit_remark;
    context_entry.ipbit_remark_mode = ctx->ipbit_remark;
    context_entry.outer_pbit = ctx->opbit_val;	 	 
    context_entry.inner_pbit = ctx->ipbit_val;
#if !defined(BCM63158)
    context_entry.include_mcast = ctx->include_mcast;
#endif
    /* action is added dynamically according to policer params */    
    context_entry.dei_or_iptv_common_actions = 0;

    if (-1 == ctx->policer)
        context_entry.policer_enable = 0;
    else
        context_entry.policer_enable = 1;

    context_entry.dscp_value = ctx->dscp_val;
    context_entry.dscp_remark = ctx->dscp_remark;
    context_entry.vlan_action = ctx->is_vlan_action; /* avoid fw vlan action incase of transparnet for all egress ports */

    switch (ctx->action)
    {
    case    rdpa_forward_action_forward:
        context_entry.no_fwd = 0;
        break;
    case    rdpa_forward_action_host:
        context_entry.no_fwd = 1;
        context_entry.no_fwd_action = 1;
        context_entry.trap_reason = ctx->trap_reason;
        break;
    case    rdpa_forward_action_drop:
    case    rdpa_forward_action_none:
    default:
        context_entry.no_fwd = 1;
		context_entry.no_fwd_action = 0;
        break;
    }

    memcpy(entry, &context_entry, sizeof(RDD_RULE_BASED_CONTEXT_ENTRY_DTS));
}

void rdd_ic_result_entry_decompose(uint8_t *entry, uint16_t *index, rdd_ic_context_t *ctx)
{
    RDD_RULE_BASED_CONTEXT_ENTRY_DTS context_entry = {};

    memcpy(&context_entry, entry, sizeof(RDD_RULE_BASED_CONTEXT_ENTRY_DTS));

    if (ctx)
    {
        ctx->to_lan = context_entry.to_lan;
        ctx->priority = context_entry.queue;
        ctx->tx_flow = ctx->to_lan ? 0 : context_entry.flow_or_port;
        ctx->egress_port = ctx->to_lan ? context_entry.flow_or_port : RDD_WAN0_VPORT;
        ctx->dscp_val = context_entry.dscp_value;
        ctx->dscp_remark = context_entry.dscp_remark;
        ctx->qos_method = (context_entry.pbit_to_q == 1)
            ? rdpa_qos_method_pbit : rdpa_qos_method_flow;
        ctx->gem_mapping_mode= context_entry.pbit_to_gem;
        ctx->gem_mapping_table = context_entry.gem_mapping_table;
        ctx->forw_mode = (ctx->forw_mode == 1) ? rdpa_forwarding_mode_flow : rdpa_forwarding_mode_pkt;

        ctx->policer = context_entry.policer_id;
        ctx->cntr_id = context_entry.cntr_id;
        ctx->qos_rule_wan_flow_overrun = context_entry.qos_rule_wan_flow_overrun;
        ctx->opbit_remark = context_entry.opbit_remark_mode;
        ctx->ipbit_remark = context_entry.ipbit_remark_mode;
        ctx->opbit_val = context_entry.outer_pbit;	 	 
        ctx->ipbit_val = context_entry.inner_pbit;
#if !defined(BCM63158)
        ctx->include_mcast = context_entry.include_mcast;
#endif

        if (context_entry.no_fwd)
        {
            if (context_entry.no_fwd_action)
                ctx->action = rdpa_forward_action_host;
            else
                ctx->action = rdpa_forward_action_drop;
        }
        else
            ctx->action = rdpa_forward_action_forward;
    }
    *index = context_entry.rule;
}


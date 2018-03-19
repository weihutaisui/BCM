/*
   <:copyright-BRCM:2014:DUAL/GPL:standard

      Copyright (c) 2014 Broadcom
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

#include "rdd_ip_class.h"

typedef struct
{
    uint32_t       natc_control;
    RDD_FLOW_CACHE_CONTEXT_ENTRY_DTS flow_cache_context;
} natc_result_entry_t;


static void _map_rdpa_to_rdd_actions(uint16_t src_actions_vector, uint16_t *dst_actions_vector, rdpa_traffic_dir conn_dir)
{
    *dst_actions_vector = 0;

    if (src_actions_vector & (1 << rdpa_fc_act_forward))
        *dst_actions_vector |= (1 << ACTION_ID_TRAP);
    if (src_actions_vector & (1 << rdpa_fc_act_policer))
        *dst_actions_vector |= (1 << ACTION_ID_POLICER);
    if (src_actions_vector & (1 << rdpa_fc_act_ttl))
        *dst_actions_vector |= (1 << ACTION_ID_TTL);
    if (src_actions_vector & (1 << rdpa_fc_act_dscp_remark))
        *dst_actions_vector |= (1 << ACTION_ID_DSCP);
    if (src_actions_vector & (1 << rdpa_fc_act_nat))
    {
        if (conn_dir == rdpa_dir_ds)
            *dst_actions_vector |= (1 << ACTION_ID_NAT_DS);
        else
            *dst_actions_vector |= (1 << ACTION_ID_NAT_US);
    }
    if (src_actions_vector & (1 << rdpa_fc_act_gre_remark))
        *dst_actions_vector |= (1 << ACTION_ID_GRE);
    if (src_actions_vector & (1 << rdpa_fc_act_opbit_remark))
    {
        if (conn_dir == rdpa_dir_ds)
            *dst_actions_vector |= (1 << ACTION_ID_OPBITS_DS);
        else
            *dst_actions_vector |= (1 << ACTION_ID_OPBITS_US);
    }
    if (src_actions_vector & (1 << rdpa_fc_act_ipbit_remark))
    {
        if (conn_dir == rdpa_dir_ds)
            *dst_actions_vector |= (1 << ACTION_ID_IPBITS_DS);
        else
            *dst_actions_vector |= (1 << ACTION_ID_IPBITS_US);
    }
    if (src_actions_vector & (1 << rdpa_fc_act_dslite_tunnel))
    {
        if (conn_dir == rdpa_dir_ds)
            *dst_actions_vector |= (1 << ACTION_ID_DEFAULT);
        else
            *dst_actions_vector |= (1 << ACTION_ID_DS_LITE);
    }

    if (src_actions_vector & (1 << rdpa_fc_act_l2gre_tunnel))
    {
        if (conn_dir == rdpa_dir_ds)
            *dst_actions_vector |= (1 << ACTION_ID_DEFAULT);
        else
            *dst_actions_vector |= (1 << ACTION_ID_GRE_TUNNEL);
    }

    if (src_actions_vector & (1 << rdpa_fc_act_pppoe))
        *dst_actions_vector |= (1 << ACTION_ID_PPPOE);

    if (src_actions_vector & (1 << rdpa_fc_act_llc_snap_set_len))
        *dst_actions_vector |= (1 << ACTION_ID_LLC_SNAP_SET_LEN);

    if (src_actions_vector & (1 << rdpa_fc_act_spdsvc))
        *dst_actions_vector |= (1 << ACTION_ID_SPDSVC);
}

static void _map_rdd_to_rdpa_actions(uint16_t src_actions_vector, uint16_t *dst_actions_vector)
{
    *dst_actions_vector = 0;

    if (src_actions_vector & (1 << ACTION_ID_TRAP))
        *dst_actions_vector |= (1 << rdpa_fc_act_forward);
    if (src_actions_vector & (1 << ACTION_ID_POLICER))
        *dst_actions_vector |= (1 << rdpa_fc_act_policer);
    if (src_actions_vector & (1 << ACTION_ID_TTL))
        *dst_actions_vector |= (1 << rdpa_fc_act_ttl);
    if (src_actions_vector & (1 << ACTION_ID_DSCP))
        *dst_actions_vector |= (1 << rdpa_fc_act_dscp_remark);
    if ((src_actions_vector & (1 << ACTION_ID_NAT_DS)) || (src_actions_vector & (1 << ACTION_ID_NAT_US)))
        *dst_actions_vector |= (1 << rdpa_fc_act_nat);
    if (src_actions_vector & (1 << ACTION_ID_GRE))
        *dst_actions_vector |= (1 << rdpa_fc_act_gre_remark);
    if ((src_actions_vector & (1 << ACTION_ID_OPBITS_DS)) || (src_actions_vector & (1 << ACTION_ID_OPBITS_US)))
        *dst_actions_vector |= (1 << rdpa_fc_act_opbit_remark);
    if ((src_actions_vector & (1 << ACTION_ID_IPBITS_DS)) || (src_actions_vector & (1 << ACTION_ID_IPBITS_US)))
        *dst_actions_vector |= (1 << rdpa_fc_act_ipbit_remark);
    if (src_actions_vector & (1 << ACTION_ID_DS_LITE))
        *dst_actions_vector |= (1 << rdpa_fc_act_dslite_tunnel);
    if (src_actions_vector & (1 << ACTION_ID_GRE_TUNNEL))
        *dst_actions_vector |= (1 << rdpa_fc_act_l2gre_tunnel);
    if (src_actions_vector & (1 << ACTION_ID_PPPOE))
        *dst_actions_vector |= (1 << rdpa_fc_act_pppoe);
    if (src_actions_vector & (1 << ACTION_ID_LLC_SNAP_SET_LEN))
        *dst_actions_vector |= (1 << rdpa_fc_act_llc_snap_set_len);
    if (src_actions_vector & (1 << ACTION_ID_SPDSVC))
        *dst_actions_vector |= (1 << rdpa_fc_act_spdsvc);
}

void rdd_ip_class_result_entry_compose(rdd_fc_context_t *ctx, uint8_t *entry, rdpa_traffic_dir dir)
{
    natc_result_entry_t context_entry = {};
    uint16_t dst_actions_vector, drop_eligibility;

    _map_rdpa_to_rdd_actions(ctx->actions_vector, &dst_actions_vector, dir);

    RDD_BTRACE("dst_actions_vector 0x%x, ctx %p\n", dst_actions_vector, ctx);

    context_entry.flow_cache_context.to_lan = ctx->to_lan;
    context_entry.flow_cache_context.actions_vector = dst_actions_vector;
    context_entry.flow_cache_context.fwd_action = ctx->fwd_action;
    context_entry.flow_cache_context.cpu_reason = ctx->trap_reason - rdpa_cpu_rx_reason_udef_0;
    context_entry.flow_cache_context.service_queue = ctx->service_queue_id;
        
    /* Make sure trap_type = 0 in case of configure trap action */
    if (!(dst_actions_vector & (1 << ACTION_ID_TRAP)) )
    {
        context_entry.flow_cache_context.service_queue_mode_miss_union = ctx->service_queue_enabled;
    }
    context_entry.flow_cache_context.dscp_value = ctx->dscp_value;
    context_entry.flow_cache_context.ecn_value = ctx->ecn_value;
    context_entry.flow_cache_context.nat_port = ctx->nat_port;
    context_entry.flow_cache_context.ip_checksum_delta = ctx->ip_checksum_delta;
    context_entry.flow_cache_context.l4_checksum_delta = ctx->l4_checksum_delta;
    context_entry.flow_cache_context.nat_ip = ctx->nat_ip.addr.ipv4;
    context_entry.flow_cache_context.physical_egress_port = ctx->phy_egress_port;
    context_entry.flow_cache_context.ip_version = ctx->ip_version;
    context_entry.flow_cache_context.ssid = ctx->ssid;
    context_entry.flow_cache_context.qos_mapping_mode = ctx->qos_method;
    context_entry.flow_cache_context.outer_vid_offset = ctx->ovid_offset;
    context_entry.flow_cache_context.outer_pbit_remap_action = ctx->opbit_action;
    context_entry.flow_cache_context.inner_pbit_remap_action = ctx->ipbit_action;
    context_entry.flow_cache_context.policer_id = ctx->policer_id;
    context_entry.flow_cache_context.l2_offset = ctx->l2_hdr_offset;
    context_entry.flow_cache_context.l2_size = ctx->l2_hdr_size - RDD_LAYER2_HEADER_MINIMUM_LENGTH;
    context_entry.flow_cache_context.l2_header_number_of_tags = ctx->l2_hdr_number_of_tags;
    context_entry.flow_cache_context.queue = ctx->traffic_class;
    context_entry.flow_cache_context.tx_flow = ctx->tx_flow;

    if (ctx->to_lan && (ctx->tx_flow >= RDD_CPU_VPORT_FIRST && ctx->tx_flow <= RDD_CPU_VPORT_LAST))
    {
        uint8_t metadata_0;
        uint16_t metadata_1;

        /* Today support only WLAN */
        context_entry.flow_cache_context.egress_cpu_vport = 1;
        if (ctx->wfd.nic_ucast.is_wfd)
        {
            context_entry.flow_cache_context.tc = ctx->wfd.nic_ucast.wfd_prio;
            context_entry.flow_cache_context.is_wfd = 1;
            if (ctx->wfd.nic_ucast.is_chain)
            {
                uint8_t iq_prio = (ctx->wfd.nic_ucast.priority & 0x8) >> 3;

                metadata_0 = (1 << 3) | (ctx->wfd.nic_ucast.priority & 0x7);
                metadata_1 = (iq_prio << 8) | ctx->wfd.nic_ucast.chain_idx;
            }
            else
            {
                metadata_0 = ctx->wfd.dhd_ucast.priority;
                metadata_1 = ctx->wfd.dhd_ucast.flowring_idx;
            }
        }
        else
        {
            context_entry.flow_cache_context.is_wfd = 0;
            metadata_0 = ctx->rnr.priority;
            metadata_1 = ctx->rnr.flowring_idx;
        }
    context_entry.flow_cache_context.cpu_params_union = metadata_0 << 10 | metadata_1;
    }
    else
        context_entry.flow_cache_context.tc = 0; /* TODO */

    context_entry.flow_cache_context.l2_header = ctx->l2_header[0];
    context_entry.flow_cache_context.l2_header_1 = ctx->l2_header[1];
    context_entry.flow_cache_context.l2_header_2 = ctx->l2_header[2];
    context_entry.flow_cache_context.l2_header_3 = ctx->l2_header[3];
    context_entry.flow_cache_context.l2_header_4 = ctx->l2_header[4];
    context_entry.flow_cache_context.l2_header_5 = ctx->l2_header[5];
    context_entry.flow_cache_context.l2_header_6 = ctx->l2_header[6];
    context_entry.flow_cache_context.l2_header_7 = ctx->l2_header[7];
    context_entry.flow_cache_context.l2_header_8 = ctx->l2_header[8];
    context_entry.flow_cache_context.l2_header_9 = ctx->l2_header[9];
    context_entry.flow_cache_context.l2_header_10 = ctx->l2_header[10];
    context_entry.flow_cache_context.l2_header_11 = ctx->l2_header[11];
    context_entry.flow_cache_context.l2_header_12 = ctx->l2_header[12];
    context_entry.flow_cache_context.l2_header_13 = ctx->l2_header[13];
    context_entry.flow_cache_context.l2_header_14 = ctx->l2_header[14];
    context_entry.flow_cache_context.l2_header_15 = ctx->l2_header[15];
    context_entry.flow_cache_context.l2_header_16 = ctx->l2_header[16];
    context_entry.flow_cache_context.l2_header_17 = ctx->l2_header[17];
    context_entry.flow_cache_context.l2_header_18 = ctx->l2_header[18];
    context_entry.flow_cache_context.l2_header_19 = ctx->l2_header[19];
    context_entry.flow_cache_context.l2_header_20 = ctx->l2_header[20];
    context_entry.flow_cache_context.l2_header_21 = ctx->l2_header[21];
    context_entry.flow_cache_context.l2_header_22 = ctx->l2_header[22];
    context_entry.flow_cache_context.l2_header_23 = ctx->l2_header[23];
    context_entry.flow_cache_context.l2_header_24 = ctx->l2_header[24];
    context_entry.flow_cache_context.l2_header_25 = ctx->l2_header[25];
    context_entry.flow_cache_context.l2_header_26 = ctx->l2_header[26];
    context_entry.flow_cache_context.l2_header_27 = ctx->l2_header[27];
    context_entry.flow_cache_context.l2_header_28 = ctx->l2_header[28];
    context_entry.flow_cache_context.l2_header_29 = ctx->l2_header[29];

    drop_eligibility = ctx->drop_eligibility;
    context_entry.flow_cache_context.drop_eligibility_en = (drop_eligibility >> 1) & 0x1;
    context_entry.flow_cache_context.drop_eligibility = drop_eligibility & 0x1;

    memcpy(entry, &context_entry, sizeof(natc_result_entry_t));

#ifdef FIRMWARE_LITTLE_ENDIAN
    SWAPBYTES(entry, sizeof(natc_result_entry_t));
#endif
}


void rdd_ip_class_result_entry_decompose(rdd_fc_context_t *ctx, uint8_t *entry)
{
    natc_result_entry_t context_entry;
    uint8_t drop_eligibility;

    RDD_BTRACE("ctx %p, entry %p\n", ctx, entry);

    memset(ctx, 0, sizeof(rdd_fc_context_t));
#ifdef FIRMWARE_LITTLE_ENDIAN
    SWAPBYTES(entry, sizeof(natc_result_entry_t));
#endif
    memcpy(&context_entry, entry, sizeof(natc_result_entry_t));

    _map_rdd_to_rdpa_actions(context_entry.flow_cache_context.actions_vector, &ctx->actions_vector);
    ctx->fwd_action = context_entry.flow_cache_context.fwd_action;
    ctx->trap_reason = context_entry.flow_cache_context.cpu_reason + rdpa_cpu_rx_reason_udef_0;
    ctx->service_queue_enabled = context_entry.flow_cache_context.service_queue_mode_miss_union;
    ctx->service_queue_id = context_entry.flow_cache_context.service_queue;
    ctx->qos_method = context_entry.flow_cache_context.qos_mapping_mode;
    ctx->ip_version = context_entry.flow_cache_context.ip_version;
    ctx->nat_port =context_entry.flow_cache_context.nat_port;
    ctx->nat_ip.addr.ipv4 = context_entry.flow_cache_context.nat_ip;
    ctx->ovid_offset = context_entry.flow_cache_context.outer_vid_offset;
    ctx->opbit_action = context_entry.flow_cache_context.outer_pbit_remap_action;
    ctx->ipbit_action = context_entry.flow_cache_context.inner_pbit_remap_action;
    ctx->dscp_value = context_entry.flow_cache_context.dscp_value;
    ctx->ecn_value = context_entry.flow_cache_context.ecn_value;
    ctx->policer_id = context_entry.flow_cache_context.policer_id;
    ctx->phy_egress_port = context_entry.flow_cache_context.physical_egress_port;
    ctx->ssid = context_entry.flow_cache_context.ssid;
    ctx->l2_hdr_offset = context_entry.flow_cache_context.l2_offset;
    ctx->l2_hdr_size = context_entry.flow_cache_context.l2_size + RDD_LAYER2_HEADER_MINIMUM_LENGTH;
    ctx->l2_hdr_number_of_tags = context_entry.flow_cache_context.l2_header_number_of_tags;
    ctx->traffic_class = context_entry.flow_cache_context.queue;
    ctx->tx_flow = context_entry.flow_cache_context.tx_flow;
    ctx->ip_checksum_delta = context_entry.flow_cache_context.ip_checksum_delta;
    ctx->l4_checksum_delta = context_entry.flow_cache_context.l4_checksum_delta;
    ctx->to_lan = context_entry.flow_cache_context.to_lan;

    if (ctx->to_lan && (ctx->tx_flow >= RDD_CPU_VPORT_FIRST && ctx->tx_flow <= RDD_CPU_VPORT_LAST))
    {
        uint8_t metadata_0;
        uint16_t metadata_1;

        /* Today support only WLAN */
        ctx->wfd.nic_ucast.wfd_prio = context_entry.flow_cache_context.tc;
        ctx->wfd.nic_ucast.is_wfd = context_entry.flow_cache_context.is_wfd;
        metadata_0 = (context_entry.flow_cache_context.cpu_params_union & 0x3c00) >> 10;
        metadata_1 = context_entry.flow_cache_context.cpu_params_union & 0x3ff;
        ctx->wfd.nic_ucast.is_chain = metadata_0 >> 3;
        ctx->wfd.nic_ucast.priority = metadata_0 & 0x7;
        if (ctx->wfd.nic_ucast.is_chain)
        {
            uint8_t iq_prio = (metadata_1 >> 8) & 0x1;

            ctx->wfd.nic_ucast.priority |= iq_prio << 3;
            ctx->wfd.nic_ucast.chain_idx = metadata_1 & 0xff;
        }
        else
        {
            ctx->wfd.dhd_ucast.flowring_idx = metadata_1;
        }
    }

    ctx->l2_header[0] = context_entry.flow_cache_context.l2_header;
    ctx->l2_header[1] = context_entry.flow_cache_context.l2_header_1;
    ctx->l2_header[2] = context_entry.flow_cache_context.l2_header_2;
    ctx->l2_header[3] = context_entry.flow_cache_context.l2_header_3;
    ctx->l2_header[4] = context_entry.flow_cache_context.l2_header_4;
    ctx->l2_header[5] = context_entry.flow_cache_context.l2_header_5;
    ctx->l2_header[6] = context_entry.flow_cache_context.l2_header_6;
    ctx->l2_header[7] = context_entry.flow_cache_context.l2_header_7;
    ctx->l2_header[8] = context_entry.flow_cache_context.l2_header_8;
    ctx->l2_header[9] = context_entry.flow_cache_context.l2_header_9;
    ctx->l2_header[10] = context_entry.flow_cache_context.l2_header_10;
    ctx->l2_header[11] = context_entry.flow_cache_context.l2_header_11;
    ctx->l2_header[12] = context_entry.flow_cache_context.l2_header_12;
    ctx->l2_header[13] = context_entry.flow_cache_context.l2_header_13;
    ctx->l2_header[14] = context_entry.flow_cache_context.l2_header_14;
    ctx->l2_header[15] = context_entry.flow_cache_context.l2_header_15;
    ctx->l2_header[16] = context_entry.flow_cache_context.l2_header_16;
    ctx->l2_header[17] = context_entry.flow_cache_context.l2_header_17;
    ctx->l2_header[18] = context_entry.flow_cache_context.l2_header_18;
    ctx->l2_header[19] = context_entry.flow_cache_context.l2_header_19;
    ctx->l2_header[20] = context_entry.flow_cache_context.l2_header_20;
    ctx->l2_header[21] = context_entry.flow_cache_context.l2_header_21;
    ctx->l2_header[22] = context_entry.flow_cache_context.l2_header_22;
    ctx->l2_header[23] = context_entry.flow_cache_context.l2_header_23;
    ctx->l2_header[24] = context_entry.flow_cache_context.l2_header_24;
    ctx->l2_header[25] = context_entry.flow_cache_context.l2_header_25;
    ctx->l2_header[26] = context_entry.flow_cache_context.l2_header_26;
    ctx->l2_header[27] = context_entry.flow_cache_context.l2_header_27;
    ctx->l2_header[28] = context_entry.flow_cache_context.l2_header_28;
    ctx->l2_header[29] = context_entry.flow_cache_context.l2_header_29;
    drop_eligibility = context_entry.flow_cache_context.drop_eligibility_en << 1;
    drop_eligibility |= context_entry.flow_cache_context.drop_eligibility;
    ctx->drop_eligibility = drop_eligibility;
}


void rdd_ip_class_natc_cfg(uint8_t tbl_idx, uint32_t key_addr_hi, uint32_t key_addr_lo, uint32_t res_addr_hi, uint32_t res_addr_lo)
{    
    RDD_NATC_TBL_CONFIGURATION_MISS_CACHE_ENABLE_WRITE_G(1, RDD_NATC_TBL_CFG_ADDRESS_ARR, tbl_idx);
    RDD_NATC_TBL_CONFIGURATION_KEY_SIZE_WRITE_G(NATC_TABLE_KEY_SIZE, RDD_NATC_TBL_CFG_ADDRESS_ARR, tbl_idx);
    RDD_NATC_TBL_CONFIGURATION_CONTEXT_SIZE_WRITE_G(NATC_TABLE_RES_SIZE, RDD_NATC_TBL_CFG_ADDRESS_ARR, tbl_idx);
    
    RDD_NATC_TBL_CONFIGURATION_KEY_ADDR_HIGH_WRITE_G(key_addr_hi, RDD_NATC_TBL_CFG_ADDRESS_ARR, tbl_idx);
    RDD_NATC_TBL_CONFIGURATION_KEY_ADDR_LOW_WRITE_G(key_addr_lo, RDD_NATC_TBL_CFG_ADDRESS_ARR, tbl_idx);
       
    RDD_NATC_TBL_CONFIGURATION_RES_ADDR_HIGH_WRITE_G(res_addr_hi, RDD_NATC_TBL_CFG_ADDRESS_ARR, tbl_idx);
    RDD_NATC_TBL_CONFIGURATION_RES_ADDR_LOW_WRITE_G(res_addr_lo, RDD_NATC_TBL_CFG_ADDRESS_ARR, tbl_idx);        
}

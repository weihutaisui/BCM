/*
   Copyright (c) 2013 Broadcom Corporation
   All Rights Reserved

    <:label-BRCM:2013:DUAL/GPL:standard
    
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
#include "rdd_bridge.h"
#include "rdd_ag_bridge.h"
#include "rdd_ag_processing.h"



/******************************************************************************/
/*                                                                            */
/*                            F/W tables configuration helpers                */
/*                                                                            */
/******************************************************************************/


/* module init */
static void _rdd_bridge_fw_module_init(const rdd_module_t *module)
{
    const rdd_bridge_module_param_t *params = module->params;   

    /* Module init. Set up module configuration */
    RDD_BRIDGE_CFG_RES_OFFSET_WRITE_G(module->res_offset, module->cfg_ptr, params->module_id);
    RDD_BRIDGE_CFG_CONTEXT_OFFSET_WRITE_G(module->context_offset, module->cfg_ptr, params->module_id);
}

/* Module params init*/
static void _rdd_bridge_fw_module_params_init(const rdd_module_t *module)
{
    const rdd_bridge_module_param_t *params = module->params;   

    /* Module init. Set up module params configuration */
    RDD_BRIDGE_CFG_BRIDGE_RESULTS_AVAILABLE_WRITE_G(params->bridge_lkps_ready, module->cfg_ptr, params->module_id);
    RDD_BRIDGE_CFG_VLAN_AGGREGATION_WRITE_G(
       params->bridge_module_actions.vlan_aggregation_action,
       module->cfg_ptr,
       params->module_id);
    RDD_BRIDGE_CFG_BRIDGE_FW_FAILED_WRITE_G(
       params->bridge_module_actions.bridge_fw_failed_action,
       module->cfg_ptr,
       params->module_id);
    RDD_BRIDGE_CFG_HIT_WRITE_G(
       params->bridge_module_actions.hit,
       module->cfg_ptr,
       params->module_id);                       
}

/******************************************************************************/
/*                                                                            */
/*                            External interface                              */
/*                                                                            */
/******************************************************************************/

/****************************************************************************************
 * module->init callback
 *****************************************************************************************/
int rdd_bridge_module_init(const rdd_module_t *module)
{
    _rdd_bridge_fw_module_init(module);
    _rdd_bridge_fw_module_params_init(module);
    return BDMF_ERR_OK;
}

void rdd_bridge_ports_init(void)
{
   int idx;

   for (idx = 0; idx < RDD_VPORT_CFG_TABLE_SIZE; idx++)
   {
      RDD_VPORT_CFG_ENTRY_SA_LOOKUP_MISS_ACTION_WRITE_G(ACTION_TRAP, RDD_VPORT_CFG_TABLE_ADDRESS_ARR, idx);
      RDD_VPORT_CFG_ENTRY_SA_LOOKUP_EN_WRITE_G(1, RDD_VPORT_CFG_TABLE_ADDRESS_ARR, idx);
      RDD_VPORT_CFG_ENTRY_DA_LOOKUP_MISS_ACTION_WRITE_G(ACTION_TRAP, RDD_VPORT_CFG_TABLE_ADDRESS_ARR, idx);
      RDD_VPORT_CFG_ENTRY_DA_LOOKUP_EN_WRITE_G(1, RDD_VPORT_CFG_TABLE_ADDRESS_ARR, idx);
      RDD_VPORT_CFG_ENTRY_EGRESS_ISOLATION_EN_WRITE_G(0, RDD_VPORT_CFG_TABLE_ADDRESS_ARR, idx);
      RDD_VPORT_CFG_ENTRY_INGRESS_ISOLATION_EN_WRITE_G(0, RDD_VPORT_CFG_TABLE_ADDRESS_ARR, idx);
      RDD_VPORT_CFG_ENTRY_BRIDGE_AND_VLAN_LOOKUP_METHOD_WRITE_G(0, RDD_VPORT_CFG_TABLE_ADDRESS_ARR, idx);
      RDD_VPORT_CFG_ENTRY_LS_FC_CFG_WRITE_G(0, RDD_VPORT_CFG_TABLE_ADDRESS_ARR, idx);
      RDD_VPORT_CFG_ENTRY_EGRESS_ISOLATION_MAP_WRITE_G(0, RDD_VPORT_CFG_TABLE_ADDRESS_ARR, idx);
      RDD_VPORT_CFG_ENTRY_PROTOCOL_FILTERS_DIS_WRITE_G(0, RDD_VPORT_CFG_TABLE_ADDRESS_ARR, idx);
      RDD_VPORT_CFG_ENTRY_DISCARD_PRTY_WRITE_G(0, RDD_VPORT_CFG_TABLE_ADDRESS_ARR, idx);
      rdd_ag_processing_vport_cfg_ex_table_ingress_filter_profile_set(idx, 0x3f);
   }
}


/********************************************************************
 * RDD integration
 ********************************************************************/

/***************************************************************************
 * Low-overhead FDB access functions
 * ToDo: implement
 **************************************************************************/

rdpa_forward_action rdd_action2rdpa_forward_action(rdd_action_t rdd_action)
{
    rdpa_forward_action fa = rdpa_forward_action_host;

    switch (rdd_action)
    {
    case ACTION_FORWARD:
        fa = rdpa_forward_action_forward;
        break;

    case ACTION_DROP:
        fa = rdpa_forward_action_drop;
        break;

    case ACTION_MULTICAST:
       fa = rdpa_forward_action_flood;
       break;

    default:
        break;
    }
    return fa;
}

rdd_action_t rdpa_forward_action2rdd_action(rdpa_forward_action fa)
{
    rdd_action ra;

    switch (fa)
    {
    case rdpa_forward_action_forward:
        ra = ACTION_FORWARD;
        break;

    case rdpa_forward_action_drop:
        ra = ACTION_DROP;
        break;

    case rdpa_forward_action_flood:
       ra = ACTION_MULTICAST;
       break;

    case rdpa_forward_action_host:
    default:
        ra = ACTION_TRAP;
        break;
    }
    return ra;
}

void rdd_bridge_bridge_and_vlan_ctx_hash_ctx_compose(RDD_BRIDGE_AND_VLAN_LKP_RESULT_DTS *bridge_and_vlan_lkp_result, 
    uint8_t *int_ctx, uint8_t *ext_ctx)
{
    RDD_BTRACE("== Composing hash context for vlan and port (bridge_and_vlan_lkp_result) resolution:\n\t"
        "port_isolation_map_15_0 = 0x%x, port_isolation_map_23_16 = 0x%x, port_isolation_map_31_24 = 0x%x\n\t"
        "wan_vid = %d, ingress_filter_profile = 0x%x, protocol_filters_dis 0x%x\n\t"
        "sa_lookup_en = %d, sa_lookup_miss_action = %d, da_lookup_en = %d, da_lookup_miss_action = %d\n\t"
        "aggregation_en = %d, arl_lookup_method = %d\n",
        bridge_and_vlan_lkp_result->port_isolation_map_15_0, bridge_and_vlan_lkp_result->port_isolation_map_23_16,
        bridge_and_vlan_lkp_result->port_isolation_map_31_24, 
        bridge_and_vlan_lkp_result->wan_vid, bridge_and_vlan_lkp_result->ingress_filter_profile,
        bridge_and_vlan_lkp_result->protocol_filters_dis,
        bridge_and_vlan_lkp_result->sa_lookup_en, bridge_and_vlan_lkp_result->sa_lookup_miss_action,
        bridge_and_vlan_lkp_result->da_lookup_en, bridge_and_vlan_lkp_result->da_lookup_miss_action,
        bridge_and_vlan_lkp_result->aggregation_en, bridge_and_vlan_lkp_result->arl_lookup_method);

    /* Internal context: Port Isolation MAP 24-31 + ingress filters profile */
    int_ctx[2] = bridge_and_vlan_lkp_result->port_isolation_map_31_24;
    int_ctx[1] = bridge_and_vlan_lkp_result->ingress_filter_profile;
    int_ctx[0] = 0;

    /* External context 3_5: Isolation Map 0-23 (swapped) */
    ext_ctx[4] = bridge_and_vlan_lkp_result->port_isolation_map_15_0 & 0xFF;
    ext_ctx[5] = (bridge_and_vlan_lkp_result->port_isolation_map_15_0 >> 8) & 0xFF;
    ext_ctx[3] = bridge_and_vlan_lkp_result->port_isolation_map_23_16;

    /* External context 0_1: WAN VID + Disabled Protocols mask */
    ext_ctx[0] = bridge_and_vlan_lkp_result->wan_vid & 0xFF;
    ext_ctx[1] = (bridge_and_vlan_lkp_result->wan_vid >> 8) |
        (bridge_and_vlan_lkp_result->protocol_filters_dis << 4);

    /* External context 2: flags + SA/DA lookup miss actions */
    ext_ctx[2] =
        (bridge_and_vlan_lkp_result->aggregation_en << BRIDGE_AND_VLAN_LKP_RESULT_AGGREGATION_EN_F_OFFSET_MOD8) |
        (bridge_and_vlan_lkp_result->arl_lookup_method << BRIDGE_AND_VLAN_LKP_RESULT_ARL_LOOKUP_METHOD_F_OFFSET_MOD8) |
        (bridge_and_vlan_lkp_result->sa_lookup_en << BRIDGE_AND_VLAN_LKP_RESULT_SA_LOOKUP_EN_F_OFFSET_MOD8) |
        (bridge_and_vlan_lkp_result->da_lookup_en << BRIDGE_AND_VLAN_LKP_RESULT_DA_LOOKUP_EN_F_OFFSET_MOD8) |
        ((bridge_and_vlan_lkp_result->sa_lookup_miss_action & 0x3) << BRIDGE_AND_VLAN_LKP_RESULT_SA_LOOKUP_MISS_ACTION_F_OFFSET_MOD8) |
        ((bridge_and_vlan_lkp_result->da_lookup_miss_action & 0x3) << BRIDGE_AND_VLAN_LKP_RESULT_DA_LOOKUP_MISS_ACTION_F_OFFSET_MOD8);

    RDD_BTRACE("== Result:\n\t*** Internal context: int_ctx[0] = %x, int_ctx[1] = %x, int_ctx[2] = %x ***\n"
        "\t*** External context: ext_ctx[0] = %x, ext_ctx[1] = %x, ext_ctx[2] = %x, ext_ctx[3] = %x, "
        "ext_ctx[4] = %x, ext_ctx[5] = %x ***\n",
        int_ctx[0], int_ctx[1], int_ctx[2], 
        ext_ctx[0], ext_ctx[1], ext_ctx[2], ext_ctx[3], ext_ctx[4], ext_ctx[5]);

    return;
}

void rdd_bridge_bridge_and_vlan_ctx_hash_ctx_decompose(RDD_BRIDGE_AND_VLAN_LKP_RESULT_DTS *bridge_and_vlan_lkp_result, 
    uint8_t *int_ctx, uint8_t *ext_ctx)
{
    RDD_BTRACE("== De-composing hash context for vlan and port (bridge_and_vlan_lkp_result):\n\t"
        "*** Internal context: int_ctx[0] = %x, int_ctx[1] = %x, int_ctx[2] = %x ***\n"
        "\t*** External context: ext_ctx[0] = %x, ext_ctx[1] = %x, ext_ctx[2] = %x, ext_ctx[3] = %x, "
        "ext_ctx[4] = %x, ext_ctx[5] = %x ***\n",
        int_ctx[0], int_ctx[1], int_ctx[2], 
        ext_ctx[0], ext_ctx[1], ext_ctx[2], ext_ctx[3], ext_ctx[4], ext_ctx[5]);

    /* Internal context: Port Isolation MAP 24-31 + ingress filters profile */
    bridge_and_vlan_lkp_result->port_isolation_map_31_24 = int_ctx[2];
    bridge_and_vlan_lkp_result->ingress_filter_profile = int_ctx[1];

    /* External context 3_5: Isolation Map 0-23 (swapped) */
    bridge_and_vlan_lkp_result->port_isolation_map_15_0 = (ext_ctx[5] << 8) | ext_ctx[4];
    bridge_and_vlan_lkp_result->port_isolation_map_23_16 = ext_ctx[3];

    /* External context 0_1: WAN VID + Disabled Protocols mask */
    bridge_and_vlan_lkp_result->wan_vid = (ext_ctx[1] & 0xF) << 8 | ext_ctx[0];
    bridge_and_vlan_lkp_result->protocol_filters_dis = (ext_ctx[1] & 0xF0) >> 4;

    /* External context 2: flags + SA/DA lookup miss actions */
    bridge_and_vlan_lkp_result->aggregation_en = FIELD_GET(ext_ctx[2],
        BRIDGE_AND_VLAN_LKP_RESULT_AGGREGATION_EN_F_OFFSET_MOD8, 1); 
    bridge_and_vlan_lkp_result->arl_lookup_method = FIELD_GET(ext_ctx[2],
        BRIDGE_AND_VLAN_LKP_RESULT_ARL_LOOKUP_METHOD_F_OFFSET_MOD8, 1);
    bridge_and_vlan_lkp_result->sa_lookup_en = FIELD_GET(ext_ctx[2],
        BRIDGE_AND_VLAN_LKP_RESULT_SA_LOOKUP_EN_F_OFFSET_MOD8, 1);
    bridge_and_vlan_lkp_result->da_lookup_en = FIELD_GET(ext_ctx[2],
        BRIDGE_AND_VLAN_LKP_RESULT_DA_LOOKUP_EN_F_OFFSET_MOD8, 1);
    bridge_and_vlan_lkp_result->sa_lookup_miss_action = FIELD_GET(ext_ctx[2],
        BRIDGE_AND_VLAN_LKP_RESULT_SA_LOOKUP_MISS_ACTION_F_OFFSET_MOD8, 2);
    bridge_and_vlan_lkp_result->da_lookup_miss_action = FIELD_GET(ext_ctx[2],
        BRIDGE_AND_VLAN_LKP_RESULT_DA_LOOKUP_MISS_ACTION_F_OFFSET_MOD8, 2);

    RDD_BTRACE("== Result:\n\t"
        "port_isolation_map_15_0 = 0x%x, port_isolation_map_23_16 = 0x%x, port_isolation_map_31_24 = 0x%x\n\t"
        "wan_vid = %d, ingress_filter_profile = 0x%x, protocol_filters_dis 0x%x\n\t"
        "sa_lookup_en = %d, sa_lookup_miss_action = %d, da_lookup_en = %d, da_lookup_miss_action = %d\n\t"
        "aggregation_en = %d, arl_lookup_method = %d\n",
        bridge_and_vlan_lkp_result->port_isolation_map_15_0, bridge_and_vlan_lkp_result->port_isolation_map_23_16,
        bridge_and_vlan_lkp_result->port_isolation_map_31_24, 
        bridge_and_vlan_lkp_result->wan_vid, bridge_and_vlan_lkp_result->ingress_filter_profile,
        bridge_and_vlan_lkp_result->protocol_filters_dis,
        bridge_and_vlan_lkp_result->sa_lookup_en, bridge_and_vlan_lkp_result->sa_lookup_miss_action,
        bridge_and_vlan_lkp_result->da_lookup_en, bridge_and_vlan_lkp_result->da_lookup_miss_action,
        bridge_and_vlan_lkp_result->aggregation_en, bridge_and_vlan_lkp_result->arl_lookup_method);

    return;
}

void map_rdd_arl_data_to_ext_ctx(RDD_BRIDGE_ARL_LKP_RESULT_DTS *rdd_arl_data, 
    uint8_t* ext_ctx)
{
    RDD_BTRACE("Mapping rdd arl data to external hash context: "
        "lan_vid_8lsb = %d, lan_vid_msb_and_da_no_fwd_action = %d"
        " vport = %d\n",
        rdd_arl_data->lan_vid_8lsb, rdd_arl_data->lan_vid_msb_or_da_no_fwd_action, rdd_arl_data->vport);

    memset(ext_ctx, 0, 3);
    ext_ctx[0] = rdd_arl_data->lan_vid_8lsb;
    ext_ctx[1] = rdd_arl_data->lan_vid_msb_or_da_no_fwd_action
        | (rdd_arl_data->da_match_action_fwd << BRIDGE_ARL_LKP_RESULT_DA_MATCH_ACTION_FWD_F_OFFSET);
    ext_ctx[2] = rdd_arl_data->vport 
        | (rdd_arl_data->sa_match_action << BRIDGE_ARL_LKP_RESULT_SA_MATCH_ACTION_F_OFFSET_MOD8);

    RDD_BTRACE("Result External Context: [0] = %d, [1] = %d, [2] = %d\n", ext_ctx[0], ext_ctx[1], ext_ctx[2]);
}

void map_ext_ctx_to_rdd_arl_data(uint8_t* ext_ctx, RDD_BRIDGE_ARL_LKP_RESULT_DTS *rdd_arl_data)
{
    RDD_BTRACE("Mapping hash external context to rdd arl data: [0] = %d, [1] = %d, [2] = %d\n", 
        ext_ctx[0], ext_ctx[1], ext_ctx[2]);

    rdd_arl_data->lan_vid_8lsb = ext_ctx[0];
    rdd_arl_data->da_match_action_fwd = (ext_ctx[1] >> BRIDGE_ARL_LKP_RESULT_DA_MATCH_ACTION_FWD_F_OFFSET) & 0x1;
    if (rdd_arl_data->da_match_action_fwd) /* lan_vid*/
        rdd_arl_data->lan_vid_msb_or_da_no_fwd_action = ext_ctx[1] & 0xF;
    else /* match action drop/trap*/
        rdd_arl_data->lan_vid_msb_or_da_no_fwd_action = ext_ctx[1] & 0x1;
    rdd_arl_data->vport = ext_ctx[2] & 0x3F;
    rdd_arl_data->sa_match_action = (ext_ctx[2] >> BRIDGE_ARL_LKP_RESULT_SA_MATCH_ACTION_F_OFFSET_MOD8) & 0x3;

    RDD_BTRACE("Result rdd arl data to external hash context: "
        "lan_vid_8lsb = %d, lan_vid_msb_and_da_no_fwd_action = %d"
        " vport = %d\n",
        rdd_arl_data->lan_vid_8lsb, rdd_arl_data->lan_vid_msb_or_da_no_fwd_action, rdd_arl_data->vport);
}


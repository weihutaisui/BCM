/*
    <:copyright-BRCM:2013:DUAL/GPL:standard
    
       Copyright (c) 2013 Broadcom 
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


/******************************************************************************/
/*                                                                            */
/*                            Global Variables                                */
/*                                                                            */
/******************************************************************************/

extern RDD_WAN_TX_POINTERS_TABLE_DTS                       *wan_tx_pointers_table_ptr;

BL_LILAC_RDD_ERROR_DTE rdd_us_wan_flow_config ( uint32_t                      xi_wan_flow,
                                                RDD_WAN_CHANNEL_ID            xi_wan_channel,
                                                uint32_t                      xi_wan_port_or_fstat,
                                                BL_LILAC_RDD_TX_CRC_CALC_DTE  xi_crc_calc,
                                                int                           xi_ptm_bonding,
                                                uint8_t                       xi_pbits_to_queue_table_index,
                                                uint8_t                       xi_traffic_class_to_queue_table_index)
{
    RDD_US_WAN_FLOW_TABLE_DTS  *wan_flow_table_ptr;
    RDD_US_WAN_FLOW_ENTRY_DTS  *wan_flow_entry_ptr;

    wan_flow_table_ptr = ( RDD_US_WAN_FLOW_TABLE_DTS * )(DEVICE_ADDRESS( RUNNER_PRIVATE_1_OFFSET ) + US_WAN_FLOW_TABLE_ADDRESS );

    wan_flow_entry_ptr = &( wan_flow_table_ptr->entry[ xi_wan_flow ] );

    RDD_US_WAN_FLOW_ENTRY_WAN_PORT_ID_OR_FSTAT_WRITE ( xi_wan_port_or_fstat , wan_flow_entry_ptr );
    RDD_US_WAN_FLOW_ENTRY_CRC_CALC_WRITE ( xi_crc_calc, wan_flow_entry_ptr );
    RDD_US_WAN_FLOW_ENTRY_PTM_BONDING_WRITE( xi_ptm_bonding, wan_flow_entry_ptr );
    RDD_US_WAN_FLOW_ENTRY_WAN_CHANNEL_ID_WRITE ( xi_wan_channel, wan_flow_entry_ptr );

    return ( BL_LILAC_RDD_OK );
}


BL_LILAC_RDD_ERROR_DTE rdd_ingress_classification_context_get ( rdpa_traffic_dir                      xi_direction,
                                                                uint32_t                              xi_context_id,
                                                                rdd_ingress_classification_context_t  *xo_context )
{
    return ( BL_LILAC_RDD_OK );
}

BL_LILAC_RDD_ERROR_DTE rdd_us_pbits_to_qos_entry_config ( uint8_t                    xi_wan_mapping_table_index,
                                                          uint32_t                   xi_pbits,
                                                          BL_LILAC_RDD_QUEUE_ID_DTE  xi_queue,
                                                          uint8_t                    xi_rate_controller )
{
    return ( BL_LILAC_RDD_OK );
}

BL_LILAC_RDD_ERROR_DTE rdd_dscp_to_pbits_global_config ( uint32_t  xi_dscp,
                                                         uint32_t  xi_pbits )
{
    return ( BL_LILAC_RDD_OK );
}

BL_LILAC_RDD_ERROR_DTE rdd_dscp_to_pbits_dei_global_config ( uint32_t  xi_dscp,
                                                             uint32_t  xi_pbits,
                                                             uint32_t  xi_dei )
{
    RDD_GLOBAL_DSCP_TO_PBITS_DEI_TABLE_DTS  *global_dscp_to_pbits_dei_table_ptr;
    RDD_DSCP_TO_PBITS_DEI_ENTRY_DTS         *dscp_to_pbits_dei_entry_ptr;

    global_dscp_to_pbits_dei_table_ptr = ( RDD_GLOBAL_DSCP_TO_PBITS_DEI_TABLE_DTS * )(DEVICE_ADDRESS( RUNNER_COMMON_0_OFFSET ) + GLOBAL_DSCP_TO_PBITS_DEI_TABLE_ADDRESS );

    dscp_to_pbits_dei_entry_ptr = &( global_dscp_to_pbits_dei_table_ptr->entry[ xi_dscp ] );

    RDD_DSCP_TO_PBITS_DEI_ENTRY_PBITS_WRITE ( xi_pbits, dscp_to_pbits_dei_entry_ptr );
    RDD_DSCP_TO_PBITS_DEI_ENTRY_DEI_WRITE ( xi_dei, dscp_to_pbits_dei_entry_ptr );

    return ( BL_LILAC_RDD_OK );
}

BL_LILAC_RDD_ERROR_DTE rdd_ds_pbits_to_qos_entry_config ( BL_LILAC_RDD_BRIDGE_PORT_DTE  xi_bridge_port,
                                                          uint32_t                      xi_pbits,
                                                          BL_LILAC_RDD_QUEUE_ID_DTE     xi_qos )
{
    return ( BL_LILAC_RDD_OK );
}

BL_LILAC_RDD_ERROR_DTE rdd_ingress_classification_context_config ( rdpa_traffic_dir                             xi_direction,
                                                                   uint32_t                                     xi_context_id,
                                                                   const rdd_ingress_classification_context_t  *xi_context )
{
    return ( BL_LILAC_RDD_OK );
}

BL_LILAC_RDD_ERROR_DTE rdd_ds_wan_flow_config ( uint32_t                                        xi_wan_flow,
                                                rdpa_cpu_reason                                 xi_cpu_reason,
                                                BL_LILAC_RDD_DOWNSTREAM_FLOW_CLASSIFY_MODE_DTE  xi_flow_classify_mode,
                                                uint8_t                                         xi_ingress_flow )
{
    return ( BL_LILAC_RDD_OK );
}

BL_LILAC_RDD_ERROR_DTE rdd_ingress_classification_rule_cfg_add ( rdpa_traffic_dir                        xi_direction,
                                                                 int32_t                                 xi_rule_cfg_priority,
                                                                 rdpa_ic_type                            xi_rule_cfg_type,
                                                                 rdpa_ic_fields                          xi_rule_cfg_key_mask,
                                                                 /* rdpa_ingress_class_rule_type            xi_rule_cfg_type, */
                                                                 /* rdpa_ingress_class_rule_mask            xi_rule_cfg_key_mask, */
                                                                 rdpa_forward_action                     xi_rule_hit_action,
                                                                 rdpa_forward_action                     xi_rule_miss_action,
                                                                 rdd_ingress_classification_lookup_mode  *xo_rule_cfg_lookup_mode,
									                             int generic_rule_cfg_idx1,
                                                                 int generic_rule_cfg_idx2 )
{
    return ( BL_LILAC_RDD_OK );
}

BL_LILAC_RDD_ERROR_DTE rdd_ingress_classification_rule_cfg_delete ( rdpa_traffic_dir  xi_direction,
                                                                    int32_t           xi_rule_cfg_priority )
{
    return ( BL_LILAC_RDD_OK );
}

BL_LILAC_RDD_ERROR_DTE rdd_ingress_classification_rule_delete ( rdpa_traffic_dir           xi_direction,
                                                                uint32_t                   xi_rule_cfg_priority,
                                                                /* rdpa_classification_key_t  *xi_rule_key */
                                                                rdpa_ic_key_t     *xi_rule_key )
{
    return ( BL_LILAC_RDD_OK );
}

BL_LILAC_RDD_ERROR_DTE rdd_ingress_classification_rule_add ( rdpa_traffic_dir           xi_direction,
                                                             uint32_t                   xi_rule_cfg_priority,
                                                             /* rdpa_classification_key_t  *xi_rule_key, */
                                                             rdpa_ic_key_t     *xi_rule_key,
                                                             uint32_t                   xi_context_id )
{
    return ( BL_LILAC_RDD_OK );
}

BL_LILAC_RDD_ERROR_DTE rdd_ds_traffic_class_to_queue_entry_config ( BL_LILAC_RDD_BRIDGE_PORT_DTE  xi_bridge_port,
                                                                    uint8_t                       xi_traffic_class,
                                                                    BL_LILAC_RDD_QUEUE_ID_DTE     xi_queue )
{
    return ( BL_LILAC_RDD_OK );
}

BL_LILAC_RDD_ERROR_DTE rdd_us_traffic_class_to_queue_entry_config ( uint8_t                    xi_wan_mapping_table_index,
                                                                    uint8_t                    xi_traffic_class,
                                                                    BL_LILAC_RDD_QUEUE_ID_DTE  xi_queue,
                                                                    uint8_t                    xi_rate_controller )
{
   return ( BL_LILAC_RDD_OK );
}

BL_LILAC_RDD_ERROR_DTE rdd_ingress_classification_rule_cfg_modify ( rdpa_traffic_dir     xi_direction,
                                                                    int32_t              xi_rule_cfg_priority,
                                                                    rdpa_forward_action  xi_rule_hit_action,
                                                                    rdpa_forward_action  xi_rule_miss_action )
{
    return ( BL_LILAC_RDD_OK );
}

BL_LILAC_RDD_ERROR_DTE rdd_us_ingress_classification_default_flows_config ( BL_LILAC_RDD_EMAC_ID_DTE      xi_emac_id,
                                                                            uint32_t                      xi_context_id )
{
    return ( BL_LILAC_RDD_OK );
}

BL_LILAC_RDD_ERROR_DTE rdd_us_pbits_to_wan_flow_entry_config ( uint8_t  xi_wan_mapping_table,
                                                               uint8_t  xi_pbits,
                                                               uint8_t  xi_wan_flow )
{
    return ( BL_LILAC_RDD_OK );
}

void rdd_ingress_classification_generic_rule_cfg(rdpa_traffic_dir dir,
    int gen_rule_cfg_idx, rdpa_ic_gen_rule_cfg_t *gen_rule_cfg)
{
}

void rdd_rate_limit_overhead_cfg(uint8_t  xi_rate_limit_overhead)
{
}


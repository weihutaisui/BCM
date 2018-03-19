/*
   Copyright (c) 2015 Broadcom Corporation
   All Rights Reserved

    <:label-BRCM:2015:DUAL/GPL:standard
    
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

#ifndef _RDD_BRIDGE_H
#define _RDD_BRIDGE_H

#include "rdd_data_structures_auto.h"

typedef rdd_bridge_flow  rdd_bridge_module;
typedef rdd_action rdd_action_t;


typedef struct rdd_bridge_default_actions
{
    bdmf_boolean vlan_aggregation_action;                           
    bdmf_boolean bridge_fw_failed_action;
    bdmf_boolean hit;                        
} rdd_bridge_default_actions_t;

/** Bridge table parameters */
typedef struct rdd_bridge_module_param
{
    rdd_bridge_default_actions_t bridge_module_actions;             /* Default actions for module flow*/
    bdmf_boolean bridge_lkps_ready;                                 /* TRUE = bridge_id & da lookup results are expected to be valid in module  */
    bdmf_boolean aggregation_en;                                    /* TRUE = aggergation is enabled for bridge */
    rdd_bridge_module module_id;                                    /* FW module id */
} rdd_bridge_module_param_t;


/*****************************************************************************************
 * This module is responsible for bridge functionality: 
 * 1) Ingress isolation 
 * 2) SA/DA lookup 
 * 3) forwarding decision
 * 4) Vlan aggregation
 * 5) Egress isolation 
 *****************************************************************************************/

int rdd_bridge_module_init(const rdd_module_t *module);

rdpa_forward_action rdd_action2rdpa_forward_action(rdd_action_t rdd_action);

rdd_action_t rdpa_forward_action2rdd_action(rdpa_forward_action fa);

void rdd_bridge_bridge_and_vlan_ctx_hash_ctx_compose(RDD_BRIDGE_AND_VLAN_LKP_RESULT_DTS *bridge_id_lkp_result,
    uint8_t *int_ctx, uint8_t *ext_ctx);
void rdd_bridge_bridge_and_vlan_ctx_hash_ctx_decompose(RDD_BRIDGE_AND_VLAN_LKP_RESULT_DTS *bridge_and_vlan_lkp_result, 
    uint8_t *int_ctx, uint8_t *ext_ctx);

void map_rdd_arl_data_to_ext_ctx(RDD_BRIDGE_ARL_LKP_RESULT_DTS *rdd_arl_data, 
   uint8_t* ext_ctx);

void map_ext_ctx_to_rdd_arl_data(uint8_t* ext_ctx,
    RDD_BRIDGE_ARL_LKP_RESULT_DTS *rdd_arl_data);

void rdd_bridge_ports_init(void);

#endif /* _RDD_BRIDGE_H */

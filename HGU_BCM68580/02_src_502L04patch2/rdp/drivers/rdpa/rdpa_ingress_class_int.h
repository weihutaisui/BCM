/*
    <:copyright-BRCM:2015:DUAL/GPL:standard
    
       Copyright (c) 2015 Broadcom 
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

/*
 * rdpa_ingress_class_int.h
 */

#ifndef RDPA_INGRESS_CLASS_INT_H_
#define RDPA_INGRESS_CLASS_INT_H_

/* RDPA ingress classifier - internal interface */

/* rule entry.
 * This structure is allocated per configured rule, holds the rule entry index
 */
struct ingress_classifier
{
    DLIST_ENTRY(ingress_classifier) list;
    rdpa_ic_key_t  entry_key;
    bdmf_index index;
};

/* rule list */
DLIST_HEAD(ic_key_list, ingress_classifier);

/* ingress_classification object private data. Exposed in this file because
 * it is referenced by platform interface */
typedef struct
{
    bdmf_index index;           /* class index */
    rdpa_traffic_dir dir;       /* US / DS */
    rdpa_ic_cfg_t  cfg;         /* classification configuration */
    uint32_t num_flows;         /* Number of configured rules */
    struct ic_key_list rules;   /* Holds list of rules entry indexes */
    int gen_rule_idx1;          /* generic rule #2 configuration index */
    int gen_rule_idx2;          /* generic rule #2 configuration index */
    rdpa_forward_action hit_action;     /* Hit action */
    rdpa_forward_action miss_action;    /* Miss action */
} ic_drv_priv_t;

extern const bdmf_attr_enum_table_t rdpa_ic_type_enum_table;

extern bdmf_error_t rdpa_ic_rdd_rule_cfg_add(const ic_drv_priv_t *priv, rdd_ic_lkp_mode_t *lookup_mode);

extern bdmf_error_t rdpa_ic_rdd_rule_cfg_delete(const ic_drv_priv_t *priv);

extern bdmf_error_t rdpa_ic_rdd_rule_cfg_modify(const ic_drv_priv_t *priv, uint32_t smallest_prty,
    rdpa_forward_action hit_action, rdpa_forward_action miss_action);

extern int ingress_class_attr_port_action_write_ex(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index, 
    const void *val, uint32_t size);

#ifndef XRDP
extern bdmf_error_t rdpa_ic_rdd_rule_add(const ic_drv_priv_t *priv, bdmf_index index, const rdpa_ic_key_t *key,
    const rdd_ic_context_t *context);
#else
extern bdmf_error_t rdpa_ic_rdd_rule_add(const ic_drv_priv_t *priv, bdmf_index index, const rdpa_ic_key_t *key,
    rdd_ic_context_t *context);
#endif

extern bdmf_error_t rdpa_ic_rdd_rule_get(const ic_drv_priv_t *priv, bdmf_index index, const rdpa_ic_key_t *key,
    rdd_ic_context_t *context);

extern int rdpa_ic_result_add(uint32_t context_id, rdpa_traffic_dir dir, rdpa_ic_result_t *result,
    bdmf_boolean iptv, rdpa_ic_type ic_type);

extern void rdpa_ic_result_delete(uint32_t context_id, rdpa_traffic_dir dir);

extern bdmf_error_t rdpa_ic_rdd_rule_delete(const ic_drv_priv_t *priv, const rdpa_ic_key_t *key, bdmf_index index);

#ifndef XRDP
extern bdmf_error_t rdpa_ic_rdd_rule_modify(const ic_drv_priv_t *priv, bdmf_index index, const rdpa_ic_key_t *key,
    const rdd_ic_context_t *context);
#else
extern bdmf_error_t rdpa_ic_rdd_rule_modify(const ic_drv_priv_t *priv, bdmf_index index, const rdpa_ic_key_t *key,
    rdd_ic_context_t *context);
#endif

extern bdmf_error_t rdpa_ic_rdd_generic_rule_cfg(const ic_drv_priv_t *priv, bdmf_index index_in_table, rdpa_ic_gen_rule_cfg_t *cfg);

/* Check if context is allocated */
extern bdmf_boolean rdpa_ic_rdd_context_index_is_busy(rdpa_traffic_dir dir, uint32_t ctx_idx);

/* Get context */
extern bdmf_error_t rdpa_ic_rdd_context_get(rdpa_traffic_dir dir, uint32_t ctx_id, rdd_ic_context_t  *ctx);

/* context cfg */
#ifndef XRDP
extern bdmf_error_t rdpa_ic_rdd_context_cfg(rdpa_traffic_dir dir, uint32_t ctx_idx, const rdd_ic_context_t *ctx);
#else
extern bdmf_error_t rdpa_ic_rdd_context_cfg(rdpa_traffic_dir dir, uint32_t ctx_idx, rdd_ic_context_t *ctx);
extern bdmf_error_t rdpa_ic_rdd_context_delete(rdpa_traffic_dir dir, uint32_t ctx_idx, rdd_ic_context_t *ctx);
#endif

/* Set port_action */
#ifndef XRDP
extern bdmf_error_t rdpa_ic_rdd_port_action_cfg(rdpa_traffic_dir dir, uint32_t ctx_idx, rdpa_if port, const rdd_ic_context_t *ctx);
#else
extern bdmf_error_t rdpa_ic_rdd_port_action_cfg(rdpa_traffic_dir dir, uint32_t ctx_idx, rdpa_if port, rdd_ic_context_t *ctx);
extern bdmf_error_t rdpa_ic_rdd_port_action_del(uint32_t ctx_idx, rdpa_if port);

extern int port_action_ic_write(bdmf_index rule, bdmf_object_handle vlan_action, rdpa_if port, bdmf_boolean drop, 
  const ic_drv_priv_t *priv, const rdpa_ic_key_t *key);
#endif

/* configure MAC in RDD level*/
int configure_rdd_mac(bdmf_mac_t *mac, int is_add);

extern int is_same_vlan_action_per_port(rdd_ic_context_t *ctx, bdmf_boolean skip_transparent);

/* Configure classifier context table */
int rdpa_ic_result_vlan_action_set(rdpa_traffic_dir dir, bdmf_object_handle vlan_action_obj,
    rdpa_if egress_port, rdd_ic_context_t *ctx, bdmf_boolean is_iptv, bdmf_boolean is_init);

void rdpa_ic_result_delete(uint32_t context_id, rdpa_traffic_dir dir);

bdmf_error_t ingress_class_attr_flow_stat_read_ex(rdpa_traffic_dir dir, uint16_t context_id, uint32_t *cntr);

uint8_t rdpa_rdd_ic_context_ds_vlan_command_get_ex(const rdd_ic_context_t *context, rdpa_if port);
void rdpa_rdd_ic_context_ds_vlan_command_set_ex(rdd_ic_context_t *context, rdpa_if port, uint8_t command);

void rdpa_ic_is_vlan_action_set_ex(rdpa_traffic_dir dir, rdd_ic_context_t *ctx);

#endif /* RDPA_INGRESS_CLASS_INT_H_ */


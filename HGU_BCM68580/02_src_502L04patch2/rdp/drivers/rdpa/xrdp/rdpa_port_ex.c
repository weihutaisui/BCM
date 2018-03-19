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

#include <bdmf_dev.h>
#include "rdpa_api.h"
#include "rdpa_int.h"
#include "rdd_stubs.h"
#include "rdd_init.h"
#include "rdpa_rdd_inline.h"
#include "rdpa_egress_tm_inline.h"
#include "rdpa_port_int.h"
#include "rdpa_cpu_ex.h"
#include "rdpa_vlan_ex.h"
#include "rdpa_bridge_ex.h"
#include "rdpa_ingress_class_int.h"
#include "rdd.h"
#include "rdp_drv_bbh_tx.h"
#include "rdp_drv_bbh_rx.h"
#include "rdp_drv_rnr.h"
#include "rdp_drv_qm.h"
#include "rdp_drv_dis_reor.h"
#include "rdd_data_structures_auto.h"
#include "rdd_runner_proj_defs.h"
#include "rdp_drv_proj_cntr.h"
#include "rdd_scheduling.h"
#include "rdd_mirroring.h"
#include "rdd_cpu_rx.h"
#include "rdd_ag_processing.h"
#if !defined(BCM63158)
#include "rdd_bridge.h"
#endif
#ifdef INGRESS_FILTERS
#include "rdpa_filter_ex.h"
#endif


extern rdd_bb_id rdpa_emac_to_bb_id_rx[rdpa_emac__num_of];
extern rdd_vport_id_t rx_flow_to_vport[RX_FLOW_CONTEXTS_NUMBER];
extern rdpa_port_stat_t accumulate_port_stat[rdpa_if__number_of];
extern rdpa_emac rdpa_if_to_port_emac_map[rdpa_if__number_of];
extern void rdpa_rx_def_flow_rdd_ic_context_idx_get(bdmf_index index, RDD_RULE_BASED_CONTEXT_ENTRY_DTS *entry, uint16_t *ctx_idx);
rdd_vport_id_t rdpa_if_to_rdd_vport(rdpa_if port, rdpa_wan_type wan_type);

#define USE_CNPL_HW_COUNTER(port) (rdpa_if_is_lan_or_cpu(port) || (rdpa_if_is_wan(port) && (rdpa_is_gbe_mode() || rdpa_is_epon_ae_mode())))
#define FW_LOOPBACK_EN(cfg_ptr) ((cfg_ptr)->op == rdpa_loopback_op_local && (cfg_ptr)->type == rdpa_loopback_type_fw)

#define BBH_TIMER_MIN (BBH_FREQUENCY / 1000) /* freq * 1ms */
#define BBH_TIMER_MAX (0xFFFFFF) /* BBH timer is 24 bits length */
#define BBH_TIMER_CONFIG_BUDGET (4*1000) /* (4KB) */

static int port_rdd_mirror_cfg(port_drv_priv_t *port, rdpa_port_mirror_cfg_t *mirror_cfg);
static bbh_id_e rdpa_port_to_bbh_id(port_drv_priv_t *port);

int update_port_bridge_and_vlan_lookup_method_ex(rdpa_if port)
{
    rdd_vport_id_t vport = rdpa_port_rdpa_if_to_vport(port);
    bdmf_object_handle port_obj = NULL, vlan_obj = NULL;
#ifdef CONFIG_RNR_BRIDGE
    bdmf_object_handle bridge_obj;
#endif
    bdmf_boolean ingress_isolation_en;
    int rc;
    uint8_t method = BRIDGE_AND_VLAN_CTX_LOOKUP_METHOD_VPORT_ONLY;

    rc = rdd_ag_processing_vport_cfg_table_ingress_isolation_en_get(vport, &ingress_isolation_en);
    if (rc)
       return rc;
    if (ingress_isolation_en)
    {
        method = BRIDGE_AND_VLAN_CTX_LOOKUP_METHOD_VPORT_VID;
        goto exit;
    }

#ifdef CONFIG_RNR_BRIDGE
    bridge_obj =  _rdpa_port_get_linked_bridge(port);
  
    /* if a Q-bridge is linked to this port we must use VID for lookup */ 
    /* Workaround for when a Q-Bridge with no VLANs is linked to a port for untagged switching. */
    /* We add a port+vid=0 to HASH table*/
    if (bridge_obj)
    {
        bridge_drv_priv_t *bridge = (bridge_drv_priv_t *)bdmf_obj_data(bridge_obj);

        if (bridge->cfg.type == rdpa_bridge_802_1q)
        {
            method = BRIDGE_AND_VLAN_CTX_LOOKUP_METHOD_VPORT_VID;             
            goto exit;
        }
    }
   
    /* If any of the VLANs owned by this port are linked to a Q-bridge we must use port+VID for lookup */
    rc = rdpa_port_get(port, &port_obj);
    if (rc)
    {
        BDMF_TRACE_RET(rc, "port object %s not found, error = %d\n", 
            bdmf_attr_get_enum_text_hlp(&rdpa_if_enum_table, port), rc);
    }

    /* iterate on VLAN childs and check if any is linked to Q-bridge */
    while ((vlan_obj = bdmf_get_next_child(port_obj, "vlan", vlan_obj)) != NULL)
    {
        vlan_drv_priv_t *vlan = (vlan_drv_priv_t *)bdmf_obj_data(vlan_obj);

        /* If any of VIDs is enabled, change lookup method */
        if (vlan_is_any_vid_enabled(vlan))
        {
            method = BRIDGE_AND_VLAN_CTX_LOOKUP_METHOD_VPORT_VID;
            goto exit;
        }
    }
#endif
    
exit:
    if (vlan_obj)
        bdmf_put(vlan_obj);
    if (port_obj)
        bdmf_put(port_obj);
    
    return rdd_ag_processing_vport_cfg_table_bridge_and_vlan_lookup_method_set(vport, method);
}

static uint32_t rdpa_port_tx_counter_entry_get(rdpa_if port)
{
    if (rdpa_if_is_wan(port)) 
        return 0;

    return RDD_TM_FLOW_CNTR_TABLE_SIZE + rdpa_port_rdpa_if_to_vport(port);
}

uint32_t rdpa_port_rx_flow_src_port_get(rdpa_if port, int set_lan_indication)
{
    uint32_t rx_flow = 0;
#ifndef G9991
    rdd_vport_id_t vport = rdpa_port_rdpa_if_to_vport(port);
#endif

    if (rdpa_if_is_wan(port) && rdpa_wan_if_to_wan_type(port) != rdpa_wan_gbe) 
        rx_flow = 0; /* XXX: Should be GEM for GPON? */
    else
    {
#ifndef G9991
        if (rdpa_if_is_cpu_port(port))
            rx_flow = BB_ID_CPU0 + vport - RDD_CPU0_VPORT;
        else
        {
            if ((rdpa_is_fttdp_mode() || (!rdpa_if_is_lag_and_switch(port) && rdpa_is_ext_switch_mode())) && !rdpa_if_is_wan(port))
#else
        {
            if (((rdpa_is_fttdp_mode() && port != rdpa_if_lan29) || (!rdpa_if_is_lag_and_switch(port) && rdpa_is_ext_switch_mode())) && !rdpa_if_is_wan(port))
#endif
#ifndef BCM63158
                rx_flow = port - rdpa_if_lan0;
#else
                rx_flow = vport;
#endif
            else
                rx_flow = rdpa_emac_to_bb_id_rx[rdpa_if_to_port_emac_map[port]];
        }

        if (set_lan_indication)
            rx_flow += RDD_NUM_OF_WAN_FLOWS;
    }

    return rx_flow;
}

static uint32_t rdpa_port_rx_flow_index_get(rdpa_if port)
{
    return rdpa_port_rx_flow_src_port_get(port, 1);
}

void port_flow_add(rdpa_if port)
{
    uint32_t cntr_id;
    bdmf_index flow_index = 0;
    rdd_vport_id_t vport = rdpa_port_rdpa_if_to_vport(port);

    if (!USE_CNPL_HW_COUNTER(port))
        return;

    flow_index = rdpa_port_rx_flow_index_get(port);

    /* Remap in case of GBE */
    if (rdpa_if_is_wan(port) && rdpa_is_gbe_mode())
        rx_flow_to_vport[flow_index] = vport;

    rdpa_cntr_id_alloc(RX_FLOW_CNTR_GROUP_ID, &cntr_id);
    rdd_rx_flow_cfg(flow_index, FLOW_DEST_ETH_ID, vport, cntr_id);

    rdpa_cntr_id_alloc(TX_FLOW_CNTR_GROUP_ID, &cntr_id);
    rdd_tm_flow_cntr_cfg(rdpa_port_tx_counter_entry_get(port), cntr_id);
}

void port_flow_del(port_drv_priv_t *port)
{
    uint32_t tx_cntr_entry_index;
    bdmf_index flow_index = 0;
    rdd_ic_context_t context = {.cntr_id = TCAM_IPTV_DEF_CNTR_GROUP_INVLID_CNTR};

    if (!USE_CNPL_HW_COUNTER(port->index))
        return;

    flow_index = rdpa_port_rx_flow_index_get(port->index);
    rdpa_cntr_id_dealloc(RX_FLOW_CNTR_GROUP_ID, NONE_CNTR_SUB_GROUP_ID, flow_index);
    rdd_rx_flow_del(flow_index);

    if (port->default_cfg_exist)
    {
        rdpa_cntr_id_dealloc(TCAM_IPTV_DEF_CNTR_GROUP_ID, DEF_FLOW_CNTR_SUB_GROUP_ID, flow_index);
        rdd_rx_default_flow_cfg(flow_index, 0, &context);
        port->def_flow_index = BDMF_INDEX_UNASSIGNED;
        port->default_cfg_exist = 0;
        return;
    }

    tx_cntr_entry_index = rdpa_port_tx_counter_entry_get(port->index);
    rdpa_cntr_id_dealloc(TX_FLOW_CNTR_GROUP_ID, NONE_CNTR_SUB_GROUP_ID, tx_cntr_entry_index);
    rdd_tm_flow_cntr_cfg(tx_cntr_entry_index, TX_FLOW_CNTR_GROUP_INVLID_CNTR);
}

#if !defined(BCM63158)
int port_def_flow_cntr_add(port_drv_priv_t *port)
{
    rdpa_traffic_dir dir = rdpa_if_is_wan(port->index) ? rdpa_dir_ds : rdpa_dir_us;
    bdmf_index rx_flow_index = 0;
    int rc, ctx_idx;
    rdd_ic_context_t context = {};
    
    if (!USE_CNPL_HW_COUNTER(port->index))
        BDMF_TRACE_RET(BDMF_ERR_PARM, "Default flow should be set to gem\n");

    rx_flow_index = rdpa_port_rx_flow_index_get(port->index);

    if (!port->default_cfg_exist)
    {
        /* for post init */
        rdpa_cntr_id_dealloc(TCAM_IPTV_DEF_CNTR_GROUP_ID, DEF_FLOW_CNTR_SUB_GROUP_ID, rx_flow_index);
        port->def_flow_index = BDMF_INDEX_UNASSIGNED;
        return 0;
    }

    rc = classification_ctx_index_get(dir, 0, &ctx_idx);
    if (rc < 0)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Can't get free context index\n");

    if (dir == rdpa_dir_us)
        rc = rdpa_map_to_rdd_classifier(rdpa_dir_us, &port->default_cfg, &context, 0, 1, RDPA_IC_TYPE_FLOW, 0);
    else
        rc = rdpa_map_to_rdd_classifier(rdpa_dir_ds, &port->default_cfg, &context, 0, 0, RDPA_IC_TYPE_FLOW, 0);
    if (rc)
        return rc;

    rdpa_cntr_id_alloc(TCAM_IPTV_DEF_CNTR_GROUP_ID, &context.cntr_id);
    rdd_rx_default_flow_cfg(rx_flow_index, ctx_idx, &context);
    rc = rdpa_ic_rdd_context_cfg(dir, ctx_idx, &context);
    if (!rc)
        port->def_flow_index = ctx_idx;

    return rc;
}
#else
int port_def_flow_cntr_add(port_drv_priv_t *port)
{
    /* TBD. */
    return 0;
}
#endif

extern int rdpa_if_to_rdd_vport_set[rdpa_if__number_of];
extern rdd_vport_id_t rdpa_if_to_rdd_vport_map[rdpa_if__number_of];
extern int rdd_vport_to_rdpa_if_set[RDD_VPORT_LAST + 1];
extern rdpa_if rdd_vport_to_rdpa_if_map[RDD_VPORT_LAST + 1];
extern rdpa_if physical_port_to_rdpa_if[rdpa_physical_none];

rdd_vport_id_t rdpa_port_rdpa_if_to_vport(rdpa_if port)
{
    BUG_ON(port >= rdpa_if__number_of);

    if (port == rdpa_if_any)
        return RDD_VPORT_ID_ANY;
        
    if (!rdpa_if_to_rdd_vport_set[port])
        return RDD_VPORT_LAST + 1;

    return rdpa_if_to_rdd_vport_map[port];
}

rdpa_if rdpa_port_vport_to_rdpa_if(rdd_vport_id_t rdd_vport)
{
    if (!rdd_vport_to_rdpa_if_set[rdd_vport])
        return rdpa_if_none;

    return rdd_vport_to_rdpa_if_map[rdd_vport];
}

extern rdpa_system_init_cfg_t *sys_init_cfg;

static void _port_cpu_obj_set(port_drv_priv_t *port, struct bdmf_object *cpu_obj)
{
    uint8_t cpu_obj_idx = (uint8_t)BDMF_INDEX_UNASSIGNED;

    if (cpu_obj)
    {
        cpu_drv_priv_t *cpu_data = (cpu_drv_priv_t *)bdmf_obj_data(cpu_obj);
        cpu_obj_idx = (uint8_t)cpu_data->index;
    }

    rdd_cpu_vport_cpu_obj_set(rdpa_port_rdpa_if_to_vport(port->index), cpu_obj_idx);
}

static void port_rx_exception_cfg(port_drv_priv_t *port, uint32_t flag, bdmf_boolean is_set);
static int _port_ls_fc_cfg_ex(port_drv_priv_t *port, rdpa_port_dp_cfg_t *cfg);
static int _proto_filters_update(port_drv_priv_t *port, uint32_t proto_filters);

int port_post_init_ex(struct bdmf_object *mo)
{
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_port_mirror_cfg_t *mirror_cfg = &port->mirror_cfg;
#ifdef INGRESS_FILTERS
    rdpa_filter_ctrl_t ingress_filters_init_ctrl[RDPA_FILTERS_QUANT] = {};
#endif

    int rc = 0, i;

#if !defined(G9991) && !defined(XRDP_EMULATION)
    if (port->cfg.emac == rdpa_emac_none &&
        ((rdpa_if_is_lan(port->index) && !rdpa_is_ext_switch_mode()) || 
         (rdpa_if_is_wan(port->index) && port->wan_type == rdpa_wan_gbe)))
    {
        BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, mo, "EMAC is not set\n");
    }
#endif

    if (port->wan_type == rdpa_wan_gbe && sys_init_cfg->gbe_wan_emac != port->cfg.emac)
    {
        BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, mo,
            "Incorrect configuration of emac %s for WAN port port: %s (expected %s)!\n",
            bdmf_attr_get_enum_text_hlp(&rdpa_emac_enum_table, port->cfg.emac),
            bdmf_attr_get_enum_text_hlp(&rdpa_if_enum_table, port->index),
            bdmf_attr_get_enum_text_hlp(&rdpa_emac_enum_table, sys_init_cfg->gbe_wan_emac));
    }

    /* Only SA/DA lookup should be configured in fttdp mode */
    if (!rdpa_is_fttdp_mode())
        rc = rdpa_cfg_sa_da_lookup(port, &port->cfg, port->cfg.sal_enable, 0);
    else if (rdpa_if_is_lag_and_switch(port->index) && port->index != rdpa_if_switch)
        rc = rdpa_update_da_sa_searches(port->index, 0);
    if (rc)
        return rc;

    /* Physical port id for external switch */
    if (rdpa_if_is_lan(port->index))
    {
        if (rdpa_is_ext_switch_mode() && port->cfg.physical_port != rdpa_physical_none)
        {
            rdd_vport_id_t rdd_port = rdpa_if_to_rdd_vport(port->index, port->wan_type);
            rc = rdd_broadcom_switch_ports_mapping_table_config(rdd_port, port->cfg.physical_port);
            if (rc)
                BDMF_TRACE_RET_OBJ(rc, mo, "rdd_broadcom_switch_ports_mapping_table_config failed\n");
        }

        else if (port->cfg.physical_port == rdpa_physical_none)
        {
#if defined(BCM63158) /* On 63158 the table physical_port_to_rdpa_if is hard mapped */
            /* Make sure this rdpa-if is assigned a physical port */
            rdpa_physical_port physical_port = rdpa_physical_port0;
            for (; physical_port < rdpa_physical_none; physical_port++)
            {
                if (physical_port_to_rdpa_if[physical_port] == port->index)
                {
                    port->cfg.physical_port = physical_port;
                    break;
                }
            }
            if (physical_port >= rdpa_physical_none)
            {
                rc = BDMF_ERR_PARM;
                BDMF_TRACE_RET_OBJ(rc, mo, "rdpa_if_lanX is not mapped to physical_port\n");
            }
#else
            /* Backward compatibility */
            port->cfg.physical_port = (rdpa_physical_port)(port->cfg.emac);

            /* Can break rdpa_physical_port_to_rdpa_if if input is not 1:1 physical<->rdpa mapping */
            physical_port_to_rdpa_if[port->cfg.physical_port] = port->index;
#endif
        }
#ifdef G9991
        if (rdpa_is_fttdp_mode())
            rdd_g9991_vport_to_emac_mapping_cfg(port->index - rdpa_if_lan0, port->cfg.physical_port);
#endif
    }
    else if (port->wan_type == rdpa_wan_gbe && port->cfg.physical_port == rdpa_physical_none)
    {
        /* Backward compatibility */
        port->cfg.physical_port = (rdpa_physical_port)(port->cfg.emac);
#if !defined(BCM63158) /* On 63158 the table physical_port_to_rdpa_if is hard mapped */
        /* Can break rdpa_physical_port_to_rdpa_if if input is not 1:1 physical<->rdpa mapping */
        physical_port_to_rdpa_if[port->cfg.physical_port] = port->index;
#endif
    }
    /* Update all_ports mask. Notify RDD if necessary */
    rc = port_update_all_ports_set(mo, 1);
    if (rc)
        return rc;

    rdpa_if_to_rdd_vport_map[port->index] = rdpa_if_to_rdd_vport(port->index, port->wan_type);
    rdpa_if_to_rdd_vport_set[port->index] = 1;
    rdd_vport_to_rdpa_if_map[rdpa_if_to_rdd_vport_map[port->index]] = port->index;
    rdd_vport_to_rdpa_if_set[rdpa_if_to_rdd_vport_map[port->index]] = 1;

    /* Set up ingress filters if configured */
    for (i = 0; i < RDPA_FILTERS_QUANT; i++)
    {
        if (!port->ingress_filters[i].enabled)
            continue;
#ifdef INGRESS_FILTERS
        rc = ingress_filter_entry_set((rdpa_filter)i, mo, ingress_filters_init_ctrl,
            &port->ingress_filters[i], &port->ingress_filters_profile);
#endif
        if (rc)
            break;
    }

    if (!rc)
        rc = rdpa_cfg_sa_da_lookup(port, &port->cfg, port->cfg.sal_enable, 1);

    /* If failed to configure rdd, revert cfg to default*/
    if (rc)
    {
        if (is_triple_tag_detect())
            port->cfg.sal_enable = 0;
        else
            port->cfg.sal_enable = 1;

        port->cfg.dal_enable = 1;
        port->cfg.sal_miss_action = rdpa_forward_action_host;
        port->cfg.dal_miss_action = rdpa_forward_action_host;
    }

    rc = _port_ls_fc_cfg_ex(port, &port->cfg);
    if (rc)
    {
        /*If failed to configure rdd, revert ls cfg to default*/
        port->cfg.ls_fc_enable = ((port->index == rdpa_if_wlan0) || (port->index == rdpa_if_wlan1) || (port->index == rdpa_if_wlan1)) ? 1 : 0;
    }

    if (rdpa_if_is_lan(port->index))
        rc = port_vlan_isolation_cfg_ex(port, &port->vlan_isolation, 1);

    /* If failed to configure rdd or tried to configure a non lan port, revert isolation cfg to default */
    if (rc || (!rdpa_if_is_lan(port->index)))
    {
        port->vlan_isolation.ds = 0;
        port->vlan_isolation.us = 0;
    }

    port_flow_add(port->index);

    if (rdpa_if_is_wan(port->index) && rdpa_is_gbe_mode())
    {
#if !defined(BCM63158)
        port_rx_exception_cfg(port, EXCEPTION_PORT_WAN_GBE, 1);
#endif    
        rdd_tx_flow_enable(0, rdpa_dir_us, 1);
    }
    else if (rdpa_if_is_wan(port->index) && rdpa_is_epon_ae_mode())
    {
        rdd_tx_flow_enable(0, rdpa_dir_us, 1);
    }
    else
        rdd_tx_flow_enable(rdpa_port_rdpa_if_to_vport(port->index), rdpa_dir_ds, 1);

    if (port->default_cfg_exist)
        rc = port_def_flow_cntr_add(port);

    if (rdpa_if_is_wan(port->index))
        rc = rc ? rc : rdd_us_budget_allocation_timer_set();

    if (!rc)
        _port_cpu_obj_set(port, port->cpu_obj);

    if (!rc && (mirror_cfg->rx_dst_port || mirror_cfg->tx_dst_port))
        rc = port_rdd_mirror_cfg(port, mirror_cfg);

    if (!rc)
        rc = _proto_filters_update(port, port->proto_filters);

    if (rc)
    {
        /* rollback */
        rdpa_if_to_rdd_vport_set[port->index] = 0;
        rdd_vport_to_rdpa_if_set[rdpa_if_to_rdd_vport_map[port->index]] = 0;
    }
#ifdef DISPATCHER_UNIFIED_MULTICAST_BBMSG
    rdd_ag_processing_vport_cfg_table_viq_set(rdpa_port_rdpa_if_to_vport(port->index), rdpa_port_to_bbh_id(port));
    if (rdpa_if_is_wan(port->index))
        GROUP_MWRITE_8(RDD_DIRECT_PROCESSING_WAN_VIQ_EXCLUSIVE_ADDRESS_ARR, 0, rdpa_port_to_bbh_id(port) + DISP_REOR_VIQ_BBH_RX0_EXCL);
#endif

#ifdef G9991
    if (rdpa_if_is_lag_and_switch(port->index) && port->index != rdpa_if_switch) {
        rdd_g9991_control_sid_set(port->cfg.control_sid - rdpa_if_lan0, port->cfg.emac - rdpa_emac0);
        port->cfg.physical_port = port->cfg.emac - rdpa_emac0;
    }
#endif

    return rc;
}

int port_attr_def_flow_write(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index, const void *val,
    uint32_t size)
{
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_ic_result_t *cfg = (rdpa_ic_result_t *)val;
    int rc = 0;
#if !defined(BCM63158)
    rdpa_traffic_dir dir = rdpa_if_is_wan(port->index) ? rdpa_dir_ds : rdpa_dir_us;
    bdmf_index rx_flow_index = 0;
    rdd_ic_context_t context = {.cntr_id = TCAM_IPTV_DEF_CNTR_GROUP_INVLID_CNTR};
#endif

    if (rdpa_if_is_lag_and_switch(port->index))
        BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, mo, "can't set default flow on lag ports port\n");

    if (rdpa_if_is_wan(port->index) && rdpa_wan_if_to_wan_type(port->index) != rdpa_wan_gbe)
        BDMF_TRACE_RET(BDMF_ERR_PARM, "Default flow should be set to gem\n");

    if (cfg == NULL)
    {
#if !defined(BCM63158)
        /* FIXME!! Wen.. not sure what the following does.. does it apply to 63158? */
        rx_flow_index = rdpa_port_rx_flow_index_get(port->index);
        rdd_rx_default_flow_cfg(rx_flow_index, port->def_flow_index, &context);

        rdpa_cntr_id_dealloc(TCAM_IPTV_DEF_CNTR_GROUP_ID, DEF_FLOW_CNTR_SUB_GROUP_ID, rx_flow_index);

        classification_ctx_index_put(dir, port->def_flow_index);
#endif
        port->def_flow_index = BDMF_INDEX_UNASSIGNED;
        memset(&port->default_cfg, 0, sizeof(rdpa_ic_result_t));

        return rc;
    }

    port->default_cfg_exist = 1;
    memcpy(&port->default_cfg, cfg, sizeof(rdpa_ic_result_t));

    if (mo->state == bdmf_state_active)
        rc = port_def_flow_cntr_add(port);

    return rc;
}

#if !defined(BCM63158)
int port_attr_def_flow_read(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index, void *val, uint32_t size)
{
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_ic_result_t *cfg = (rdpa_ic_result_t *)val;
    rdpa_traffic_dir dir = rdpa_if_is_wan(port->index) ? rdpa_dir_ds : rdpa_dir_us;
    RDD_RULE_BASED_CONTEXT_ENTRY_DTS entry;
    bdmf_index flow_index;
    rdd_ic_context_t context = {};
    uint16_t ctx_idx;    

    if (port->def_flow_index == BDMF_INDEX_UNASSIGNED)
        return BDMF_ERR_NOENT;

    flow_index = rdpa_port_rx_flow_index_get(port->index);
    rdpa_rx_def_flow_rdd_ic_context_idx_get(flow_index, &entry, &ctx_idx);
    rdpa_ic_rdd_context_get(dir, ctx_idx, &context);
    return rdpa_map_from_rdd_classifier(dir, cfg, &context, 0);
}
#else
int port_attr_def_flow_read(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index, void *val, uint32_t size)
{
    rdpa_ic_result_t *cfg = (rdpa_ic_result_t *)val;
    /* memset to prevent val_to_s errors when displaying default ic fields */
    memset(val, 0, sizeof(rdpa_ic_result_t));
    cfg->qos_method = rdpa_qos_method_flow;
    cfg->action = rdpa_forward_action_host;
    cfg->forw_mode = rdpa_forwarding_mode_flow;
    cfg->egress_port = rdpa_if_none;

    return 0;
}
#endif

int mac_lkp_cfg_validate_ex(rdpa_mac_lookup_cfg_t *mac_lkp_cfg, port_drv_priv_t *port, int ls_fc_enable)
{
    if (mac_lkp_cfg->sal_enable)
    {
        if (mac_lkp_cfg->sal_miss_action == rdpa_forward_action_flood)
            BDMF_TRACE_RET(BDMF_ERR_PARM, "Flooding can be configured as SA lookup miss action only\n");
        if (mac_lkp_cfg->sal_miss_action == rdpa_forward_action_forward)
            BDMF_TRACE_RET(BDMF_ERR_PARM, "Forward action is not allowed for SA lookup.\n");
    }
    if (mac_lkp_cfg->dal_enable)
    {
        if (mac_lkp_cfg->dal_miss_action == rdpa_forward_action_forward && rdpa_if_is_wan(port->index))
            BDMF_TRACE_RET(BDMF_ERR_PARM, "Forward action is not allowed for DA lookup on wan port.\n");
        if (mac_lkp_cfg->dal_miss_action == rdpa_forward_action_flood && ls_fc_enable)
        {
            BDMF_TRACE_RET(BDMF_ERR_PARM, "Flooding upon DA miss cannot be "
                "configured for ports with ls_fc_enable set\n");
        }
    }

    return BDMF_ERR_OK;
}

int rdpa_cfg_sa_da_lookup_ex(port_drv_priv_t *port, rdpa_port_dp_cfg_t *cfg)
{
#ifdef CONFIG_RNR_BRIDGE
    rdd_vport_id_t rdd_port = rdpa_port_rdpa_if_to_vport(port->index);
    rdd_vport_cfg_entry_t vport_entry = {};
    rdpa_mac_lookup_cfg_t vlan_mac_lkp_cfg;
    int rc;

    BDMF_TRACE_DBG("Going to rdpa_cfg_sa_da_lookup_ex, index=%d, vport=%d\n", (int)port->index, rdd_port);
    rc = rdd_ag_processing_vport_cfg_entry_get(rdd_port, &vport_entry);
    if (rc)
        return rc;
    vport_entry.da_lookup_en = cfg->dal_enable;
    vport_entry.sa_lookup_en = cfg->sal_enable;
    vport_entry.da_lookup_miss_action = rdpa_forward_action2rdd_action(cfg->dal_miss_action);
    vport_entry.sa_lookup_miss_action = rdpa_forward_action2rdd_action(cfg->sal_miss_action);

    rc = rdd_ag_processing_vport_cfg_entry_set(rdd_port, &vport_entry);
    if (rc)
        return rc;

    /* Work-around for linking port to 802.1Q bridge */
    rdpa_port_dp_cfg2mac_lkp_cfg(cfg, &vlan_mac_lkp_cfg);
    return port_update_hash_port_and_vid0(port, rdpa_vlan_mac_lkp_cfg_modify_cb, &vlan_mac_lkp_cfg);
#else
    return BDMF_ERR_OK;
#endif
}

int rdpa_update_da_sa_searches(rdpa_if port, bdmf_boolean dal)
{
    return BDMF_ERR_OK;
}

void update_port_tag_size(rdpa_emac emac, drv_rnr_prop_tag_size_t new_tag_size)
{
    rnr_quad_parser_core_configuration_prop_tag_cfg prop_cfg = {};
    uint8_t i;

    /* TODO: should be to all profiles? */
    prop_cfg.size_profile_0 = new_tag_size;
    prop_cfg.size_profile_1 = new_tag_size;
    prop_cfg.size_profile_2 = new_tag_size;

    /* TODO: should be for all rnr quads? */
    for (i = 0; i < NUM_OF_RNR_QUADS; ++i)
        ag_drv_rnr_quad_parser_core_configuration_prop_tag_cfg_set(i, &prop_cfg);
}

void update_broadcom_tag_size(void)
{
    rnr_quad_parser_core_configuration_prop_tag_cfg prop_cfg = {};
    uint8_t i;

    /* profile 1 is upstream */
    prop_cfg.size_profile_1 = 4;

    /* TODO: should be for all rnr quads? */
    for (i = 0; i < NUM_OF_RNR_QUADS; ++i)
        ag_drv_rnr_quad_parser_core_configuration_prop_tag_cfg_set(i, &prop_cfg);
}

int rdpa_if_to_rdpa_physical_port(rdpa_if port, rdpa_physical_port *physical_port)
{
    if (port <= rdpa_if_lan6)
        *physical_port = port + rdpa_physical_port0;
    else
        BDMF_TRACE_RET(BDMF_ERR_PERM, "can't map rdpa_if % d to rdpa_physical_port\n", port);

    return BDMF_ERR_OK;
}

extern bbh_id_e rdpa_emac_to_bbh_id_e[rdpa_emac__num_of];
static bbh_id_e rdpa_port_to_bbh_id(port_drv_priv_t *port)
{
    if (port->cfg.emac == rdpa_emac_none && !rdpa_if_is_wan(port->index))
        return BBH_ID_NULL;

    BUG_ON(rdpa_if_is_lan(port->index) && port->cfg.emac == rdpa_emac_none);
    BUG_ON(rdpa_is_gbe_mode() && port->cfg.emac == rdpa_emac_none);

    if (rdpa_if_is_lan(port->index) || (rdpa_is_gbe_mode() && port->cfg.emac == rdpa_gbe_wan_emac()))
        return rdpa_emac_to_bbh_id_e[rdpa_if_to_port_emac_map[port->index]];

    return BBH_ID_PON;
}

int port_attr_mtu_size_read(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    void *val, uint32_t size)
{
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo);
    uint16_t mtu;
    uint8_t min_pkt_size;
    bbh_id_e bbh_id;

    bbh_id = rdpa_port_to_bbh_id(port);
    if (bbh_id == BBH_ID_NULL)
        return BDMF_ERR_INTERNAL;

    /* Since we assume that all selections will have same configuration, enough to read only first selection */
    drv_bbh_rx_pkt_size_get(bbh_id, 0, &min_pkt_size, &mtu);

    *(uint32_t *)val = mtu;

    return BDMF_ERR_OK;
}

int port_attr_mtu_size_write(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    const void *val, uint32_t size)
{
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo);
    uint32_t mtu_size = *(uint32_t *)val;
    uint16_t tmp;
    uint8_t min_pkt_size;
    bbh_id_e bbh_id;
    int i, rc = 0;

    bbh_id = rdpa_port_to_bbh_id(port);

    if (bbh_id == BBH_ID_NULL)
        return BDMF_ERR_INTERNAL;

    for (i = 0; i < 4; i++) /* Same MTU size to all packet selections */
    {
        drv_bbh_rx_pkt_size_get(bbh_id, i, &min_pkt_size, &tmp);
        rc = rc ? rc : drv_bbh_rx_pkt_size_set(bbh_id, i, min_pkt_size, (uint16_t)mtu_size);
    }

    return rc ? BDMF_ERR_INTERNAL : BDMF_ERR_OK;
}

bdmf_error_t rdpa_port_tm_discard_prty_cfg_ex(struct bdmf_object *mo, rdpa_port_tm_cfg_t *tm_cfg)
{
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo);
    int rc = 0;
#ifndef BCM63158 
    rdd_vport_id_t rdd_port = rdpa_port_rdpa_if_to_vport(port->index);
    rdd_vport_cfg_entry_t vport_entry = {};

    BDMF_TRACE_DBG("Going to rdpa_port_tm_discard_prty_cfg_ex, index=%d, vport=%d\n", (int)port->index, rdd_port);

    rc = rdd_ag_processing_vport_cfg_entry_get(rdd_port, &vport_entry);
    if (rc)
        return rc;
    vport_entry.discard_prty = tm_cfg->discard_prty;

    rc = rdd_ag_processing_vport_cfg_entry_set(rdd_port, &vport_entry);
#endif    
    port->tm_cfg.discard_prty = tm_cfg->discard_prty;	
    return rc;
}

#ifdef G9991 /*read multicast/broadcast counters for g9991 project*/
static bdmf_error_t read_mcst_bcst_stat_from_hw(port_drv_priv_t *port, rdd_vport_pm_counters_t *rdd_vport_mc_bc_counters)
{
    uint32_t vport_id;
    uint32_t cntr_id;
    uint32_t cntr_arr[MAX_NUM_OF_COUNTERS_PER_READ] = {};
    int rc = 0;

    vport_id = rdpa_port_rdpa_if_to_vport(port->index);
    cntr_id =  vport_id + PORT_MCST_BCST_GROUP_RX_MULTICAST_PKT;
    rc = drv_cntr_counter_read(PORT_MCST_BCST_GROUP_ID, cntr_id, cntr_arr);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Could not read PORT_MCST_BCST_GTOUP_ID for entry %d. err: %d\n", cntr_id, rc);

    rdd_vport_mc_bc_counters->rx_multicast_pkt = cntr_arr[0];

    cntr_id = vport_id + PORT_MCST_BCST_GROUP_RX_BROADCAST_PKT;
    rc = drv_cntr_counter_read(PORT_MCST_BCST_GROUP_ID, cntr_id, cntr_arr);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Could not read PORT_MCST_BCST_GTOUP_ID for entry %d. err: %d\n", cntr_id, rc);

    rdd_vport_mc_bc_counters->rx_broadcast_pkt  = cntr_arr[0];

    cntr_id =  vport_id + PORT_MCST_BCST_GROUP_TX_MULTICAST_PKT;
    rc = drv_cntr_counter_read(PORT_MCST_BCST_GROUP_ID, cntr_id, cntr_arr);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Could not read PORT_MCST_BCST_GTOUP_ID for entry %d. err: %d\n", cntr_id, rc);

    rdd_vport_mc_bc_counters->tx_multicast_pkt = cntr_arr[0];

    cntr_id = vport_id + PORT_MCST_BCST_GROUP_TX_BROADCAST_PKT;
    rc = drv_cntr_counter_read(PORT_MCST_BCST_GROUP_ID, cntr_id, cntr_arr);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Could not read PORT_MCST_BCST_GTOUP_ID for entry %d. err: %d\n", cntr_id, rc);

    rdd_vport_mc_bc_counters->tx_broadcast_pkt  = cntr_arr[0];

    return rc;
}
#endif

static int read_stat_from_hw(port_drv_priv_t *port, rdd_vport_pm_counters_t *rdd_port_counters,
    bbh_rx_counters_t *bbh_rx_counters, bbh_tx_debug_counters *bbh_tx_counters)
{
    uint32_t flow_index = 0, tx_cntr_entry_index;
    uint32_t cntr_id;
    uint32_t cntr_arr[MAX_NUM_OF_COUNTERS_PER_READ] = {};
    int rc = 0;
    bbh_id_e bbh_id;
    uint16_t cntr_wan_drop_pkt;

    /* BBH counters */
    bbh_id = rdpa_port_to_bbh_id(port); 

    if (bbh_id != BBH_ID_NULL)
        drv_bbh_rx_get_stat(bbh_id, bbh_rx_counters);

    if (rdpa_if_is_wan(port->index) && rdpa_wan_if_to_wan_type(port->index) != rdpa_wan_gbe && !rdpa_is_epon_ae_mode())
    {
        ag_drv_bbh_tx_debug_counters_get(BBH_TX_WAN_ID, bbh_tx_counters);
        /* read wan0 drop counter*/
#if !defined(BCM63158)
        rc = drv_cntr_varios_counter_get(COUNTER_RX_WAN_PORT_DROP, &cntr_wan_drop_pkt);
#else
        cntr_wan_drop_pkt = 0;
#endif
        if (rc)
            BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Could not read DROP_GROUP_RX_WAN_PORT counter for flow. err: %d\n", rc);
        rdd_port_counters->rx_drop_pkt = (uint32_t)cntr_wan_drop_pkt;
        return rc;
    }

    /* RX PM counters */
    flow_index = rdpa_port_rx_flow_index_get(port->index);
    cntr_id = rdd_rx_flow_cntr_id_get(flow_index);

    if (cntr_id == RX_FLOW_CNTR_GROUP_INVLID_CNTR)
        BDMF_TRACE_RET(BDMF_ERR_PARM, "can't get counter id. RX flow index - %d\n", flow_index);

    rc = drv_cntr_counter_read(RX_FLOW_CNTR_GROUP_ID, cntr_id, cntr_arr);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Could not read CNTR counters for flow %d. err: %d\n", flow_index, rc);

    rdd_port_counters->rx_packets = cntr_arr[0];
    rdd_port_counters->rx_bytes = cntr_arr[1];
    rdd_port_counters->rx_drop_pkt = cntr_arr[2];

    /* TX PM counters */
    tx_cntr_entry_index = rdpa_port_tx_counter_entry_get(port->index);
    cntr_id = rdd_tm_flow_cntr_id_get(tx_cntr_entry_index);

    if (cntr_id == TX_FLOW_CNTR_GROUP_INVLID_CNTR)
        BDMF_TRACE_RET(BDMF_ERR_PARM, "can't get counter id. TX flow index - %d\n", tx_cntr_entry_index);

    rc = drv_cntr_counter_read(TX_FLOW_CNTR_GROUP_ID, cntr_id, cntr_arr);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Could not read CNTR counters for entry %d. err: %d\n", tx_cntr_entry_index, rc);

    rdd_port_counters->tx_packets = cntr_arr[0];
    rdd_port_counters->tx_bytes = cntr_arr[1];

#ifdef G9991
    rc = read_mcst_bcst_stat_from_hw(port, rdd_port_counters);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Could not read MC BC counters for port %d. err: %d\n", rdpa_port_rdpa_if_to_vport(port->index), rc);

#endif
    return rc;
}

int port_attr_stat_read(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index, void *val, uint32_t size)
{
    rdpa_port_stat_t *stat = (rdpa_port_stat_t *)val;
    bbh_rx_counters_t bbh_rx_counters = {};
    bbh_tx_debug_counters bbh_tx_counters = {};
    rdd_vport_pm_counters_t rdd_port_counters = {};
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo);
    int rc = 0;

    if (port->index >= rdpa_if_lan_max)
        return BDMF_ERR_PARM;
    
    if (rdpa_if_is_lag_and_switch(port->index))
    {
        /* ToDo */
        return rc;
    }

    if (rdpa_if_is_cpu_port(port->index))
    {
        /* ToDo: need to accumalte Ring counters */
        return rc;
    }

    rc = read_stat_from_hw(port, &rdd_port_counters, &bbh_rx_counters, &bbh_tx_counters);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "can't clear PM port counters, error = %d\n", rc);

    accumulate_port_stat[port->index].rx_discard_min_length += bbh_rx_counters.pm_counters.too_short;
    accumulate_port_stat[port->index].rx_discard_max_length += bbh_rx_counters.pm_counters.too_long;
    accumulate_port_stat[port->index].rx_discard_2 += bbh_rx_counters.pm_counters.no_sbpm_sbn;
    accumulate_port_stat[port->index].bbh_drop_2 += bbh_rx_counters.pm_counters.no_sdma_cd;
    accumulate_port_stat[port->index].bbh_drop_3 += bbh_rx_counters.pm_counters.disp_cong;
    accumulate_port_stat[port->index].rx_crc_error_pkt += bbh_rx_counters.pm_counters.crc_err;
    accumulate_port_stat[port->index].discard_pkt += rdd_port_counters.rx_drop_pkt;

    if (rdpa_if_is_wan(port->index) && rdpa_wan_if_to_wan_type(port->index) != rdpa_wan_gbe && !rdpa_is_epon_ae_mode())
    {
        accumulate_port_stat[port->index].rx_valid_pkt += bbh_rx_counters.pm_counters.inpkt;
        accumulate_port_stat[port->index].tx_valid_pkt += bbh_tx_counters.ddrpkt + bbh_tx_counters.srampkt;
     }
    else
    {
        accumulate_port_stat[port->index].rx_valid_pkt += rdd_port_counters.rx_packets;
        accumulate_port_stat[port->index].rx_valid_bytes += rdd_port_counters.rx_bytes;
        accumulate_port_stat[port->index].tx_valid_pkt += rdd_port_counters.tx_packets;
        accumulate_port_stat[port->index].tx_valid_bytes += rdd_port_counters.tx_bytes;
#ifdef G9991
        accumulate_port_stat[port->index].rx_broadcast_pkt += rdd_port_counters.rx_broadcast_pkt;
        accumulate_port_stat[port->index].rx_multicast_pkt += rdd_port_counters.rx_multicast_pkt;
        accumulate_port_stat[port->index].tx_broadcast_pkt += rdd_port_counters.tx_broadcast_pkt;
        accumulate_port_stat[port->index].tx_multicast_pkt += rdd_port_counters.tx_multicast_pkt;
#endif
    }

    accumulate_port_stat[port->index].tx_discard = 0; 

    memcpy(stat, &accumulate_port_stat[port->index], sizeof(*stat));

    return BDMF_ERR_OK;
}

/* "stat" attribute "write" callback */
int port_attr_stat_write(struct bdmf_object *mo, struct bdmf_attr *ad, 
    bdmf_index index, const void *val, uint32_t size)
{
    int rc = 0;
    uint32_t flow_index = 0, cntr_id, tx_cntr_entry_index;
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo);

    /* clear RX counter */
    flow_index = rdpa_port_rx_flow_index_get(port->index);
    cntr_id = rdd_rx_flow_cntr_id_get(flow_index);

    if (cntr_id == RX_FLOW_CNTR_GROUP_INVLID_CNTR)
        BDMF_TRACE_RET(BDMF_ERR_PARM, "can't get counter id. RX flow index - %d\n", flow_index);

    /* clear TX counter */
    tx_cntr_entry_index = rdpa_port_tx_counter_entry_get(port->index);
    cntr_id = rdd_tm_flow_cntr_id_get(tx_cntr_entry_index);

    if (cntr_id == TX_FLOW_CNTR_GROUP_INVLID_CNTR)
        BDMF_TRACE_RET(BDMF_ERR_PARM, "can't get counter id. TX flow index - %d\n", tx_cntr_entry_index);

    memset(&accumulate_port_stat[port->index], 0, sizeof(accumulate_port_stat[port->index]));

    return rc;
}

int port_attr_wan_type_write_ex(port_drv_priv_t *port, rdpa_wan_type wan_type)
{
    return 0;
}

static int _port_ls_fc_cfg_ex(port_drv_priv_t *port, rdpa_port_dp_cfg_t *cfg)
{
    return rdd_ag_processing_vport_cfg_table_ls_fc_cfg_set(rdpa_port_rdpa_if_to_vport(port->index), cfg->ls_fc_enable);
}

int port_ls_fc_cfg_ex(struct bdmf_object *mo, rdpa_port_dp_cfg_t *cfg)
{
#if !defined(BCM63158)
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo);

    /* If object is not active, rdpa_if_to_vport mapping is not valid*/
    if (mo->state != bdmf_state_active)
        return BDMF_ERR_OK;

    if (cfg->dal_miss_action == rdpa_forward_action_flood && cfg->ls_fc_enable)
        BDMF_TRACE_RET(BDMF_ERR_PARM, "Flooding upon DA miss cannot be configured for ports with ls_fc_enable set\n");

    return _port_ls_fc_cfg_ex(port, cfg);
#endif
    return BDMF_ERR_OK;
}

static int _port_first_queue_in_egress_tm(bdmf_object_handle mirror_dst_port, uint32_t *queue_id)
{
    bdmf_number egress_tm_index;
    rdpa_tm_queue_cfg_t queue_cfg;
    rdpa_tm_level_type level;
    int i, rc = BDMF_ERR_OK;
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mirror_dst_port);
    bdmf_object_handle egress_tm =  port->tm_cfg.sched;
  
    if (egress_tm == NULL) 
        BDMF_TRACE_RET(BDMF_ERR_NOENT, "Egress TM not configured for mirror destination port (%d)\n", (int)port->index);
    
    rc = rdpa_egress_tm_level_get(egress_tm, &level);
    if (level == rdpa_tm_level_egress_tm)
    {
         BDMF_TRACE_RET(BDMF_ERR_NOT_SUPPORTED, 
             "Mirroring is not supported for mirror destination port (%d) with two level scheduling\n", 
             (int)port->index);
    }
    rc = rc ? rc : rdpa_egress_tm_index_get(egress_tm, &egress_tm_index);
    if (rc)
        BDMF_TRACE_RET(rc, "No egress TM is configured for mirror destination port (%d)\n", (int)port->index);
    for (i = 0; i < RDPA_DFT_NUM_EGRESS_QUEUES; i++)
    {
        rc = rdpa_egress_tm_queue_cfg_get(egress_tm, i, &queue_cfg);
        if (rc)
            continue;		
        if (queue_cfg.queue_id != BDMF_INDEX_UNASSIGNED)
        {
            *queue_id = queue_cfg.queue_id;
            return BDMF_ERR_OK;
        }
    }

    return BDMF_ERR_NOENT;
}

static int _port_mirror_rdpa_to_rdd_queue_cfg(port_drv_priv_t *port, bdmf_object_handle mirror_dst_port,
    uint16_t *qm_queue)
{
    int rc, rc_id, _qm_queue;
    rdpa_if port_index;
    uint32_t queue_id = 0;

    if (mirror_dst_port == NULL) 
        return 0;
    rdpa_port_index_get(mirror_dst_port, &port_index);

    if (port->index == port_index)
        BDMF_TRACE_RET(BDMF_ERR_PARM, "Destination mirroring port (%d) is the same as configured port\n", (int)port_index);
    if (!rdpa_if_is_lan(port_index))
        BDMF_TRACE_RET(BDMF_ERR_PARM, "Destination mirroring port (%d) is WAN\n", (int)port_index);
    rc = _port_first_queue_in_egress_tm(mirror_dst_port, &queue_id);
    rc = rc ? rc : _rdpa_egress_tm_lan_port_queue_to_rdd(port_index, queue_id, &rc_id, &_qm_queue);
    if (rc)
       BDMF_TRACE_RET(rc, "No valid queue is configured for mirroring destination port (%d)\n", (int)port_index);
    *qm_queue = (uint16_t)_qm_queue;
    return 0;
}

static void port_rx_exception_cfg(port_drv_priv_t *port, uint32_t flag, bdmf_boolean is_set)
{
    uint32_t rx_flow_id;

    if (is_set)
        port->exception_flags |= flag;
    else
        port->exception_flags &= ~flag;

    for (rx_flow_id = 0; rx_flow_id < RX_FLOW_CONTEXTS_NUMBER; rx_flow_id++)
    {
        if (rx_flow_to_vport[rx_flow_id] == rdpa_port_rdpa_if_to_vport(port->index))
            rdd_rx_flow_exception_cfg(rx_flow_id, port->exception_flags ? 1 : 0);
    }
}

static int port_rdd_mirror_cfg(port_drv_priv_t *port, rdpa_port_mirror_cfg_t *mirror_cfg)
{
    int rc = 0;
    rdd_mirroring_cfg_t rdd_mirroring_cfg = 
    {.rx_dst_queue = QM_NUM_QUEUES + 1,
     .tx_dst_queue = QM_NUM_QUEUES + 1,
     .src_tx_bbh_id = rdpa_emac_to_bbh_id_e[rdpa_if_to_port_emac_map[port->index]] };

    rdd_mirroring_cfg.lan = rdpa_if_is_lan(port->index);

    rc = _port_mirror_rdpa_to_rdd_queue_cfg(port, mirror_cfg->rx_dst_port,
                                       &(rdd_mirroring_cfg.rx_dst_queue));
    rc = rc ? rc : _port_mirror_rdpa_to_rdd_queue_cfg(port, mirror_cfg->tx_dst_port,
                                       &(rdd_mirroring_cfg.tx_dst_queue));
    if (rc)
        return rc;

    if (rdd_mirroring_cfg.rx_dst_queue <= QM_NUM_QUEUES)
    {
        rdpa_if rx_dst_port_idx;

        rc = rdpa_port_index_get(mirror_cfg->rx_dst_port, &rx_dst_port_idx);
        if (!rc)
        {
            rdd_mirroring_cfg.rx_dst_vport = rdpa_if_to_rdd_vport_map[rx_dst_port_idx];
        }
    }

    if (rc)
        return rc;

    if (rdd_mirroring_cfg.tx_dst_queue <= QM_NUM_QUEUES)
    {
        rdpa_if tx_dst_port_idx;

        rc = rdpa_port_index_get(mirror_cfg->tx_dst_port, &tx_dst_port_idx);
        if (!rc)
        {
            rdd_mirroring_cfg.tx_dst_vport = rdpa_if_to_rdd_vport_map[tx_dst_port_idx];
        }
    }

    if (rc) 
        return rc;

    rdd_mirroring_set(&rdd_mirroring_cfg);
    rdd_rx_mirroring_cfg(rdpa_port_rdpa_if_to_vport(port->index), !!mirror_cfg->rx_dst_port);
    port_rx_exception_cfg(port, EXCEPTION_RX_MIRROR, !!mirror_cfg->rx_dst_port);

    return BDMF_ERR_OK;
}
int port_mirror_cfg_ex(struct bdmf_object *mo, port_drv_priv_t *port, rdpa_port_mirror_cfg_t *mirror_cfg)
{
    if (mo->state == bdmf_state_active)
    {
        return port_rdd_mirror_cfg(port, mirror_cfg);
    }
    return BDMF_ERR_OK;
}

int port_attr_loopback_write_ex(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, const void *val, uint32_t size)
{
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_port_loopback_t *lb_req = (rdpa_port_loopback_t *)val;
    int rc_id, qm_queue, rc = 0;

    if (lb_req->op == rdpa_loopback_op_remote)
        BDMF_TRACE_RET(BDMF_ERR_PARM, "Remote loopback not supported\n");

    if (lb_req->type == rdpa_loopback_type_mac || lb_req->type == rdpa_loopback_type_phy)
        BDMF_TRACE_RET(BDMF_ERR_PARM, "RDPA supports only firmware based loopback\n");

    if (port->cfg.emac == rdpa_emac_none)
        BDMF_TRACE_RET(BDMF_ERR_PARM, "Loopback is supported only on LAN interfaces\n");

    if (lb_req->type == port->loopback_cfg.type && lb_req->op == port->loopback_cfg.op && lb_req->queue == port->loopback_cfg.queue)
        return 0;

    rc = rc ? rc : _rdpa_egress_tm_lan_port_queue_to_rdd(port->index, lb_req->queue, &rc_id, &qm_queue);
    if (rc)
       BDMF_TRACE_RET(rc, "Invalid queue ID\n");

    rdd_loopback_queue_set(rdpa_port_rdpa_if_to_vport(port->index), qm_queue);

    rdd_loopback_cfg(rdpa_port_rdpa_if_to_vport(port->index), FW_LOOPBACK_EN(lb_req));
    port_rx_exception_cfg(port, EXCEPTION_PORT_FW_LOOPBACK, FW_LOOPBACK_EN(lb_req));

    /* save in private data */
    port->loopback_cfg.type = lb_req->type;
    port->loopback_cfg.op = lb_req->op;
    port->loopback_cfg.queue = lb_req->queue;

    return 0;
}

int port_vlan_isolation_cfg_ex(port_drv_priv_t *port, 
    rdpa_port_vlan_isolation_t *vlan_isolation_cfg, bdmf_boolean is_active)
{
    int rc = 0;

#ifdef CONFIG_RNR_BRIDGE
    rdd_vport_id_t vport;

    /* rdpa_if to vport mapping is valid only for active port object */
    if (!is_active)
        return 0;

    vport = rdpa_port_rdpa_if_to_vport(port->index);

    rc = rdd_ag_processing_vport_cfg_table_egress_isolation_en_set(vport, 
        (vlan_isolation_cfg->ds ? 1 : 0));
    rc = rc ? rc : rdd_ag_processing_vport_cfg_table_ingress_isolation_en_set(vport, 
        (vlan_isolation_cfg->us ? 1 : 0));
    if (rc)
    {
        BDMF_TRACE_RET(rc, "Failed to configure VLAN isolation for port %s, rc = %d\n",
            bdmf_attr_get_enum_text_hlp(&rdpa_if_enum_table, port->index), rc);
    }
    return update_port_bridge_and_vlan_lookup_method_ex(port->index);
#endif
    
    return rc;
}

int port_flow_control_cfg_ex(port_drv_priv_t *port, rdpa_port_flow_ctrl_t *flow_ctrl)
{
    int rc = 0;
#if !defined(BCM63158) /* flow control not relevant for 63158 chip */
    bdmf_number policer_idx;
    policer_drv_priv_t *policer;

    rdd_vport_id_t vport_idx = rdpa_port_rdpa_if_to_vport(port->index);
    /* When policer deleted all flow control values initialized */
    if (!flow_ctrl->policer)
    {
        port->flow_ctrl.policer = NULL;
        port->flow_ctrl.traffic_types = 0;
        rc = rc ? rc : rdd_ag_processing_vport_cfg_table_flow_control_all_traffic_set(vport_idx, 0);
        rc = rc ? rc : rdd_ag_processing_vport_cfg_table_flow_control_broadcast_set(vport_idx, 0);
        rc = rc ? rc : rdd_ag_processing_vport_cfg_table_flow_control_multicast_set(vport_idx, 0);
        rc = rc ? rc : rdd_ag_processing_vport_cfg_table_flow_control_unknown_da_set(vport_idx, 0);
        return rc;
    }
    /* Write the policer index to VPORT_CFG table */
    policer = (policer_drv_priv_t *)bdmf_obj_data(flow_ctrl->policer);
    if (bdmf_attr_get_as_num(flow_ctrl->policer, rdpa_policer_attr_index, &policer_idx))
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Cannot find policer\n");
    rc = rdd_ag_processing_vport_cfg_table_policer_idx_set(vport_idx, policer_idx);
    
    /* Write the traffic types to VPORT_CFG table */
    if (flow_ctrl->traffic_types & RDPA_FLOW_CONTROL_MASK_ALL_TRAFFIC)
    {
        if (flow_ctrl->traffic_types != RDPA_FLOW_CONTROL_MASK_ALL_TRAFFIC)
            BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "all_traffic should be configured without any other traffic type!\n");
        rc = rc ? rc : rdd_ag_processing_vport_cfg_table_flow_control_all_traffic_set(vport_idx, 1);
        rc = rc ? rc : rdd_ag_processing_vport_cfg_table_flow_control_broadcast_set(vport_idx, 0);
        rc = rc ? rc : rdd_ag_processing_vport_cfg_table_flow_control_multicast_set(vport_idx, 0);
        rc = rc ? rc : rdd_ag_processing_vport_cfg_table_flow_control_unknown_da_set(vport_idx, 0);
        
        /* Configure the BBH Flow Control timeout */
        rc = port_set_bbh_timer_clock_ex(port, policer);
    }
    else
    {
        rc = rc ? rc : rdd_ag_processing_vport_cfg_table_flow_control_all_traffic_set(vport_idx, 0);
        rc = rc ? rc : rdd_ag_processing_vport_cfg_table_flow_control_broadcast_set(vport_idx,
                        ((flow_ctrl->traffic_types) & RDPA_FLOW_CONTROL_MASK_BROADCAST) >> RDPA_FLOW_CONTROL_BROADCAST);
        rc = rc ? rc : rdd_ag_processing_vport_cfg_table_flow_control_multicast_set(vport_idx, 
                        ((flow_ctrl->traffic_types) & RDPA_FLOW_CONTROL_MASK_MULTICAST) >> RDPA_FLOW_CONTROL_MULTICAST);
        rc = rc ? rc : rdd_ag_processing_vport_cfg_table_flow_control_unknown_da_set(vport_idx, 
                        ((flow_ctrl->traffic_types) & RDPA_FLOW_CONTROL_MASK_UNKNOWN_DA) >> RDPA_FLOW_CONTROL_UNKNOWN_DA);
    }
    
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_PARM, " Write flow control configuration failed!\n");
    /* Save configuration */
    port->flow_ctrl.traffic_types = flow_ctrl->traffic_types;
    port->flow_ctrl.policer = flow_ctrl->policer;
#endif    
    return rc;
}

int port_set_bbh_timer_clock_ex(port_drv_priv_t *port, policer_drv_priv_t *policer)
{
    uint32_t timer;
    bbh_id_e bbh_id;
    /* Configure the BBH FC timeout */
    bbh_id = rdpa_port_to_bbh_id(port);
    if (bbh_id == BBH_ID_NULL)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "port to bbh_id failed\n");
    
    if (policer->cfg.commited_rate)        
        timer = BBH_FREQUENCY / policer->cfg.commited_rate;
    else
        BDMF_TRACE_RET(BDMF_ERR_PARM, " Policer commited rate shouldn't be 0!\n");
    timer = timer * BBH_TIMER_CONFIG_BUDGET;
    timer = (timer < BBH_TIMER_MIN) ? BBH_TIMER_MIN : timer;
    timer = (timer > BBH_TIMER_MAX) ? BBH_TIMER_MAX : timer;
    if (ag_drv_bbh_rx_flow_ctrl_timer_set(bbh_id, (uint32_t)timer))
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Flow control timer configuration failed\n");
    return 0;
}

/* NOTE : Reverse mapping - rdd_vport to rdpa_if is NOT used
 * if required in future, it is better that we provide it
 * from the array rdd_vport_to_rdpa_if_map[] instead of reversing the logic */

rdd_vport_id_t rdpa_if_to_rdd_vport(rdpa_if port, rdpa_wan_type wan_type)
{
    rdd_vport_id_t vport;

    switch (port)
    {
#if defined(BCM63158)
    case rdpa_if_wan0 ... rdpa_if_wan_max:
        {
            switch (wan_type)
            {
            case rdpa_wan_gpon:
                vport = RDD_GPON_WAN_VPORT;
                break;
            case rdpa_wan_dsl:
                vport = RDD_DSL_WAN_VPORT;
                break;
            case rdpa_wan_gbe:
                vport = RDD_ETH_WAN_VPORT;
                break;
            default:
                BDMF_TRACE_ERR("Can't map rdpa_if %d wan_type %d to rdd vport\n", port, wan_type);
                vport = RDD_WAN0_VPORT;
                break;
            }
        }
        break;
#else
    case rdpa_if_wan0:
        vport = RDD_WAN0_VPORT;
        break;
    case rdpa_if_wan1:
        vport = RDD_WAN1_VPORT;
        break;
#if defined(RDD_WAN2_VPORT)
    case rdpa_if_wan2:
        vport = RDD_WAN2_VPORT;
        break;
#endif /* RDD_WAN2_VPORT */
#endif
    case rdpa_if_lan0 ... rdpa_if_lan_max:
        vport = (port - rdpa_if_lan0) + RDD_LAN0_VPORT;
        break;
    case rdpa_if_cpu_first ... rdpa_if_cpu_last:
        vport = RDD_CPU_VPORT_FIRST + port - rdpa_if_cpu_first;
        break;
    case rdpa_if_lag0 ... rdpa_if_lag4:
        vport = RDD_VIRTUAL_VPORT;
        break;
    case rdpa_if_switch:
        vport = RDD_VIRTUAL_VPORT;
        break;
    default:
        if (port != rdpa_if_none)
            BDMF_TRACE_ERR("Can't map rdpa_if %d to rdd vport\n", port);
        vport = RDD_WAN0_VPORT;
        break;
    }

    return vport;
}

int rdpa_port_lag_link_ex(port_drv_priv_t *lag_port)
{
    return BDMF_ERR_NOT_SUPPORTED;
}

void rdpa_port_lag_unlink_ex(port_drv_priv_t *lag_port)
{
    /* XXX: Temporary not supported */
}

int rdpa_port_bond_link_ex(rdpa_physical_port physical_port)
{
    return BDMF_ERR_NOT_SUPPORTED;
}

int rdpa_port_bond_unlink_ex(rdpa_physical_port physical_port)
{
    return BDMF_ERR_NOT_SUPPORTED;
}

int rdpa_port_get_egress_tm_channel_from_port_ex(rdpa_if port, int *channel_id)
{
    if (rdpa_if_is_wan(port))
        *channel_id = port - rdpa_if_wan0;
    else
    {
#ifndef G9991
        *channel_id = rdpa_port_rdpa_if_to_emac(port);
#else /* XRDP - G9991 */
        *channel_id = port - rdpa_if_lan0;
#endif
    }
    return 0;
}

int port_attr_cpu_obj_write_ex(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index, const void *val,
    uint32_t size)
{
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo);
    bdmf_object_handle cpu_obj = *(bdmf_object_handle *)val;

    if (mo->state != bdmf_state_init)
    {
        /* VPORT is already set, configure */
        _port_cpu_obj_set(port, cpu_obj);
    }
    port->cpu_obj = cpu_obj;
    return 0;
}

int port_attr_cpu_meter_write_ex(struct bdmf_object *mo, rdpa_traffic_dir dir, rdpa_if intf, bdmf_index meter_idx)
{
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo);
    rdd_vport_id_t vport = rdpa_port_rdpa_if_to_vport(port->index);
    rdd_cpu_rx_meter rdd_meter = (meter_idx != BDMF_INDEX_UNASSIGNED) ? meter_idx : CPU_RX_METER_DISABLE;

    if (vport == RDD_VPORT_LAST)
    {
        BDMF_TRACE_RET_OBJ(BDMF_ERR_INTERNAL, mo, "Can't translate to vport\n");
    }

    GROUP_MWRITE_8(RDD_CPU_VPORT_TO_METER_TABLE_ADDRESS_ARR, vport, rdd_meter);

    return BDMF_ERR_OK;
}

#ifdef CONFIG_RNR_BRIDGE
/* Workaround for when a Q-Bridge with no VLANs is linked to a port for untagged switching. We add a port+vid=0 to
 * HASH table */
/* So So ugly Delete as soon as possible!!!! */
int port_update_hash_port_and_vid0(port_drv_priv_t *port, void *modify_cb, void *modify_ctx)
{
    rdpa_vlan_hash_entry_modify_cb_t cb = (rdpa_vlan_hash_entry_modify_cb_t)modify_cb;
    bdmf_object_handle bridge_obj = NULL;
    int rc;

    bridge_obj = _rdpa_port_get_linked_bridge(port->index);
    if (bridge_obj)
    {
        bridge_drv_priv_t *bridge = (bridge_drv_priv_t *)bdmf_obj_data(bridge_obj);
        if (bridge->cfg.type == rdpa_bridge_802_1q)
        {
            rc = rdpa_vlan_hash_entry_modify(port->index, 0, cb, modify_ctx); 
            if (rc)
                return rc;
        }
    }

    return 0;
}
#endif

int port_attr_ingress_filter_write_ex(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    const void *val, uint32_t size)
{
#ifdef INGRESS_FILTERS
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_filter_ctrl_t *ctrl = (rdpa_filter_ctrl_t *)val;

    if (mo->state == bdmf_state_active)
    {
        return ingress_filter_entry_set((rdpa_filter)index, mo, port->ingress_filters, ctrl,
            &port->ingress_filters_profile);
    }
    return 0;
#else
    return BDMF_ERR_NOT_SUPPORTED;
#endif
}

uint32_t disabled_proto_mask_get(uint32_t proto_filters_mask)
{
    uint32_t dis_proto_mask = 0;

    if (proto_filters_mask & (1 << rdpa_proto_filter_any))
        return 0;

    dis_proto_mask = ~proto_filters_mask;
    dis_proto_mask &= (1 << rdpa_proto_filter_last) - 1;

    return dis_proto_mask;
}

static int _proto_filters_update(port_drv_priv_t *port, uint32_t proto_filters)
{
    uint32_t dis_proto_mask = disabled_proto_mask_get(proto_filters);
    rdd_vport_id_t rdd_vport = rdpa_port_rdpa_if_to_vport(port->index);
    int rc;

    rc = rdd_ag_processing_vport_cfg_table_protocol_filters_dis_set(rdd_vport, dis_proto_mask);

#ifdef CONFIG_RNR_BRIDGE
    /* Work-around for linking port to 802.1Q bridge */
    rc = rc ? rc : port_update_hash_port_and_vid0(port, rdpa_vlan_proto_filters_modify_cb, &proto_filters);
#endif
    return rc;
}

int port_attr_proto_filters_write_ex(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    const void *val, uint32_t size)
{
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo);
    uint32_t proto_filters = *(uint32_t *)val;
    int rc = 0;
   
    if (mo->state == bdmf_state_active)
        rc = _proto_filters_update(port, proto_filters);
    if (!rc)
        port->proto_filters = proto_filters;

    return rc;
}

#ifdef CONFIG_RNR_BRIDGE
static int _rdpa_port_add_port_and_vid0_to_hash(port_drv_priv_t *port, struct bdmf_object *bridge_obj)
{
    rdpa_mac_lookup_cfg_t mac_lkp_cfg;

    rdpa_port_dp_cfg2mac_lkp_cfg(&port->cfg, &mac_lkp_cfg);
    return rdpa_vlan_add_single_port_and_vid(port->index, 0, bridge_obj, port->proto_filters, &mac_lkp_cfg,
        port->ingress_filters_profile);
}
#endif

int _rdpa_port_set_linked_bridge_ex(port_drv_priv_t *port, bdmf_object_handle bridge_obj)
{
#ifdef CONFIG_RNR_BRIDGE
    bridge_drv_priv_t *bridge;

    /* Workaround for when a Q-Bridge with no VLANs is linked to a port for untagged switching.
     * When configured, we add a port+vid=0 to HASH table.
     * XXX: Since this is a work-around, we don't check whether port has already VID with VLAN-0 configured */
    if (bridge_obj) /* Link to new bridge */
    {
        bridge = (bridge_drv_priv_t *)bdmf_obj_data(bridge_obj);

        if (bridge->cfg.type == rdpa_bridge_802_1q)
            return _rdpa_port_add_port_and_vid0_to_hash(port, bridge_obj);
    }
    else /* Unlink */
    {
        if (!port->bridge_obj)
            return 0; 

        bridge = (bridge_drv_priv_t *)bdmf_obj_data(port->bridge_obj);
        if (bridge->cfg.type == rdpa_bridge_802_1q)
            return rdpa_vlan_hash_entry_delete(port->index, 0);
    }
#endif
    return 0;
}


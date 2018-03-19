/*
 * <:copyright-BRCM:2015:proprietary:standard
 *
 *    Copyright (c) 2015 Broadcom
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
#include "rdpa_api.h"
#include "rdpa_int.h"
#include "rdp_drv_rnr.h"
#include "rdd_tcam_ic.h"
#include "rdd_init.h"
#include "rdpa_system_ex.h"
#include "rdp_drv_cntr.h"
#include "rdd_cpu_rx.h"
#include "rdd_ag_cpu_rx.h"
#include "rdp_drv_bbh_tx.h"
#include "rdp_drv_bbh_rx.h"
#include "rdp_drv_dis_reor.h"
#include "rdp_drv_rnr.h"
#include "rdp_drv_qm.h"
#include "rdp_drv_psram.h"
#include "rdp_drv_bac_if.h"
#ifdef CONFIG_BCM_GPON_TODD
#include <gpon_tod_gpl.h>
#endif

extern int triple_tag_detect_ref_count;
extern int num_wan;
extern int num_lan;
extern struct bdmf_object *system_object;

static uint8_t cpu_reason_to_tc[rdpa_cpu_reason__num_of] = { };

static bdmf_boolean auto_gate;

int system_attr_clock_gate_write(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    const void *val, uint32_t size)
{
     int wait_counter = 0xC8;

     auto_gate = *(bdmf_boolean *)val;

     /* Clock-gate enable/disable for BBH-TX blocks */
     drv_bbh_tx_config_clock_autogate(auto_gate, wait_counter);
     /* Clock-gate enable/disable for BBH-RX blocks */
     drv_bbh_rx_config_clock_autogate(auto_gate, wait_counter);
     /* Clock-gate enable/disable for BACIF blocks */
     drv_bac_if_config_clock_autogate(auto_gate, wait_counter);
     /* Clock-gate enable/disable for Runner blocks */
     drv_rnr_config_clock_autogate(auto_gate, wait_counter);
     /* Clock-gate enable/disable for QM block */
     drv_qm_config_clock_autogate(auto_gate, wait_counter);
     /* Clock-gate enable/disable for DISPATCHER block */
     drv_dis_reor_config_clock_autogate(auto_gate, wait_counter);
     /* Clock-gate enable/disable for PSRAM block */
     drv_psram_config_clock_autogate(auto_gate, wait_counter);

     return 0;
}


int system_attr_clock_gate_read(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    void *val, uint32_t size)
{
#if defined(BCM63158)
     *(bdmf_boolean *)val = auto_gate;
     return 0;
#else
     return BDMF_ERR_NOT_SUPPORTED;
#endif
}

#ifdef CONFIG_CPU_REDIRECT_MODE_SUPPORT
static uint8_t _map_cpu_redirect_mode(rdpa_rx_redirect_cpu_t rdpa_redirect_mode)
{
    uint8_t rdd_mode = CPU_REDIRECT_TYPE_NONE;
    switch (rdpa_redirect_mode)
    {
    case rdpa_rx_redirect_to_cpu_disabled:
        rdd_mode = CPU_REDIRECT_TYPE_NONE;
        break;
    case rdpa_rx_redirect_to_cpu_all:
        rdd_mode = CPU_REDIRECT_TYPE_ALL;
        break;
    case rdpa_rx_redirect_to_cpu_trap_flows:
        rdd_mode = CPU_REDIRECT_TYPE_FLOW;
        break;
    }
    return rdd_mode;
}
#endif

int system_attr_cfg_write_ex(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    const void *val, uint32_t size)
{
    rdpa_system_cfg_t *cfg = (rdpa_system_cfg_t *)val;

    if (cfg->force_dscp_to_pbit_us || cfg->force_dscp_to_pbit_ds)
        RDD_BYTE_1_BITS_WRITE_G(1, RDD_FORCE_DSCP_ADDRESS_ARR, 0);
    else
        RDD_BYTE_1_BITS_WRITE_G(0, RDD_FORCE_DSCP_ADDRESS_ARR, 0);

#ifdef CONFIG_CPU_REDIRECT_MODE_SUPPORT
    {
        uint8_t rdd_mode = _map_cpu_redirect_mode(cfg->cpu_redirect_mode);
        RDD_CPU_REDIRECT_MODE_ENTRY_MODE_WRITE_G(rdd_mode, RDD_CPU_REDIRECT_MODE_ADDRESS_ARR, 0);
    }
#else
    if (cfg->cpu_redirect_mode != rdpa_rx_redirect_to_cpu_disabled)
    {
        BDMF_TRACE_RET_OBJ(BDMF_ERR_NOT_SUPPORTED, mo, "RX Redirect to CPU is not supported\n");
    }
#endif

    return 0;
}

/* TPID Detect */
static int __tpid_detect_cfg(struct bdmf_object * const mo,
    rdpa_tpid_detect_t tpid_detect,
    drv_parser_qtag_profile_t profile,
    const rdpa_tpid_detect_cfg_t * const tpid_detect_cfg)
{
    uint8_t i;
    system_drv_priv_t *priv = (system_drv_priv_t *)bdmf_obj_data(mo);
    uint16_t ethertype_0[NUM_OF_RNR_QUAD];
    uint16_t ethertype_1[NUM_OF_RNR_QUAD];
    rdpa_tpid_detect_t etype_index;
    bdmf_error_t rc = BDMF_ERR_OK;

    /* Handle user-defined */
    if ((tpid_detect == rdpa_tpid_detect_udef_1) ||
        (tpid_detect == rdpa_tpid_detect_udef_2))
    {
        for (i = 0; !rc && i < NUM_OF_RNR_QUAD; i++)
            rc = ag_drv_rnr_quad_parser_core_configuration_qtag_ethtype_get(i, &ethertype_0[i], &ethertype_1[i]);
        if (rc)
            BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, mo, "Failed to get user-defined TPIDs Detect values\n");

        if (tpid_detect == rdpa_tpid_detect_udef_1)
        {
            for (i = 0; i < NUM_OF_RNR_QUAD; i++)
                ethertype_0[i] = tpid_detect_cfg->val_udef;
        }
        else
        {
            for (i = 0; i < NUM_OF_RNR_QUAD; i++)
                ethertype_1[i] = tpid_detect_cfg->val_udef;
        }

        for (i = 0; !rc && i < NUM_OF_RNR_QUAD; i++)
            rc = ag_drv_rnr_quad_parser_core_configuration_qtag_ethtype_set(i, ethertype_0[i], ethertype_1[i]);

        if (rc)
            BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, mo, "Failed to set user-defined TPID Detect values\n");

        /* Update data */
        priv->tpids_detect[tpid_detect].val_udef = tpid_detect_cfg->val_udef;
    }

    etype_index = tpid_detect;

    /* Call PARSER API */
    for (i = 0; i < NUM_OF_RNR_QUAD; i++)
    {
        rc = rc ? rc : drv_rnr_quad_parser_configure_outer_qtag(i, profile, tpid_detect_cfg->otag_en, etype_index);
        rc = rc ? rc : drv_rnr_quad_parser_configure_inner_qtag(i, profile, tpid_detect_cfg->itag_en, etype_index);
        rc = rc ? rc : drv_rnr_quad_parser_configure_3rd_qtag(i, profile, tpid_detect_cfg->triple_en, etype_index);
    }

    if (rc)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, mo, "Failed to configure TPID Detect: %u\n", tpid_detect);

    /* Handle the SA operations ref counter */
    if (priv->tpids_detect[tpid_detect].triple_en)
    {
        if (!tpid_detect_cfg->triple_en)
            triple_tag_detect_ref_count--;
    }
    else
    {
        if (tpid_detect_cfg->triple_en)
            triple_tag_detect_ref_count++;
    }

    /* Update data */
    priv->tpids_detect[tpid_detect].otag_en = tpid_detect_cfg->otag_en;
    priv->tpids_detect[tpid_detect].itag_en = tpid_detect_cfg->itag_en;
    priv->tpids_detect[tpid_detect].triple_en = tpid_detect_cfg->triple_en;

    return rc;
}

int _tpid_detect_cfg(struct bdmf_object * const mo, rdpa_tpid_detect_t tpid_detect,
    const rdpa_tpid_detect_cfg_t * const tpid_detect_cfg)
{
    return __tpid_detect_cfg(mo, tpid_detect, DRV_PARSER_QTAG_PROFILE_0, tpid_detect_cfg) ||
        __tpid_detect_cfg(mo, tpid_detect, DRV_PARSER_QTAG_PROFILE_1, tpid_detect_cfg) ||
        __tpid_detect_cfg(mo, tpid_detect, DRV_PARSER_QTAG_PROFILE_2, tpid_detect_cfg);
}

int system_post_init_enumerate_emacs(struct bdmf_object *mo)
{
    system_drv_priv_t *system = (system_drv_priv_t *)bdmf_obj_data(mo);

    num_wan = 1;
    num_lan = __bitcount(system->init_cfg.enabled_emac);

    if (system->init_cfg.gbe_wan_emac != rdpa_emac_none)
    {
        if (system->init_cfg.enabled_emac & (1 << system->init_cfg.gbe_wan_emac))
            num_lan--;
        system->init_cfg.enabled_emac |= (1 << system->init_cfg.gbe_wan_emac);
    }
    return 0;
}

int system_pre_init_ex(struct bdmf_object *mo)
{
    system_drv_priv_t *system = (system_drv_priv_t *)bdmf_obj_data(mo);
    system->qm_cfg.number_of_ds_queues = QM_QUEUE_DS_DEFAULT_QUANTITY;
    system->qm_cfg.number_of_us_queues = QM_QUEUE_US_DEFAULT_QUANTITY;
    system->qm_cfg.number_of_service_queues =  QM_QUEUE_SERVICE_QUANTITY;
    memset(cpu_reason_to_tc, RDPA_CPU_TC_LOW, rdpa_cpu_reason__num_of);
    return 0;
}

int system_post_init_ex(struct bdmf_object *mo)
{
    system_drv_priv_t *system = (system_drv_priv_t *)bdmf_obj_data(mo);
    int i;

    /* full flow cache mode */
    if (system->init_cfg.ip_class_method == rdpa_method_fc)
        rdd_full_flow_cache_cfg(1);

    for (i = 0; i < rdpa_cpu_reason__num_of; i++)
        rdd_ag_cpu_rx_cpu_reason_to_tc_set(i, cpu_reason_to_tc[i]);

    if (system->init_cfg.switching_mode == rdpa_vlan_aware_switching)
    {
        BDMF_TRACE_RET(BDMF_ERR_NOT_SUPPORTED,
            "rdpa_vlan_aware_switching is deprecated, "
            "please set vlan learning and swithching mode per configured bridge\n");
    }

#if defined(BCM63158)
    /* SF2 mapping */
    rdd_broadcom_switch_ports_mapping_table_config(RDD_LAN0_VPORT, 0);
    rdd_broadcom_switch_ports_mapping_table_config(RDD_LAN1_VPORT, 1);
    rdd_broadcom_switch_ports_mapping_table_config(RDD_LAN2_VPORT, 2);
    rdd_broadcom_switch_ports_mapping_table_config(RDD_LAN3_VPORT, 3);
    rdd_broadcom_switch_ports_mapping_table_config(RDD_LAN4_VPORT, 4);
    rdd_broadcom_switch_ports_mapping_table_config(RDD_LAN5_VPORT, 6);

    rdd_lookup_ports_mapping_table_init(RDD_WAN0_VPORT, RDD_WAN0_VPORT);
    rdd_lookup_ports_mapping_table_init(RDD_LAN0_VPORT, RDD_LAN0_VPORT);
    rdd_lookup_ports_mapping_table_init(RDD_LAN1_VPORT, RDD_LAN1_VPORT);
    rdd_lookup_ports_mapping_table_init(RDD_LAN2_VPORT, RDD_LAN2_VPORT);
    rdd_lookup_ports_mapping_table_init(RDD_LAN3_VPORT, RDD_LAN3_VPORT);
    rdd_lookup_ports_mapping_table_init(RDD_LAN4_VPORT, RDD_LAN4_VPORT);
    rdd_lookup_ports_mapping_table_init(RDD_LAN5_VPORT, RDD_LAN5_VPORT);
    /* TBD. Add vport_any for speed service and vport_pci for WL */
#endif

    /* Enable clock autogating by default */
#if defined(BCM63158)
    /* Enable clock autogating by default */
    auto_gate = 1;
#else
    auto_gate = 0;
#endif
    system_attr_clock_gate_write(NULL, NULL, 0, &auto_gate, 0);

    return 0;
}

int system_post_init_wan(rdpa_wan_type wan_type, rdpa_emac wan_emac)
{
    int rc;

    if (wan_type == rdpa_wan_gbe)
        rc = system_data_path_init_gbe(wan_emac);
#if defined(BCM63158)
    else if (wan_type == rdpa_wan_dsl)
        rc = system_data_path_init_dsl();
#endif
    else
        rc = system_data_path_init_fiber(wan_type);
    if (rc)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_INTERNAL, system_object, "Failed system data path init fiber rc=%d\n", rc);

    return 0;
}

/* US drop statistics */
struct bdmf_aggr_type system_us_stat_type =
{
    .name = "system_us_stat", .struct_name = "rdpa_system_us_stat_t",
    .help = "System US Drop Statistics", .extra_flags = BDMF_ATTR_UNSIGNED,
    .fields = (struct bdmf_attr[])
    {
        { .name = "reserved", .help = "Reserved US counters for future use", .size = sizeof(uint16_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_system_us_stat_t, empty)
        },
        /* TODO */
        BDMF_ATTR_LAST
    },
};
DECLARE_BDMF_AGGREGATE_TYPE(system_us_stat_type);

/* DS drop statistics */
struct bdmf_aggr_type system_ds_stat_type =
{
    .name = "system_ds_stat", .struct_name = "rdpa_system_ds_stat_t",
    .help = "System Drop Statistics (Various Drop counters)", .extra_flags = BDMF_ATTR_UNSIGNED,
    .fields = (struct bdmf_attr[])
    {
        { .name = "connection_action_drop", .help = "Connection Action Drop", .size = sizeof(uint16_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_system_ds_stat_t, connection_action)
        },
        { .name = "cpu_rx_ring_congestion", .help = "CPU RX Ring congestion", .size = sizeof(uint16_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_system_ds_stat_t, cpu_rx_ring_congestion)
        },
        { .name = "cpu_recycle_ring_congestion", .help = "CPU Recycle Ring congestion", .size = sizeof(uint16_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_system_ds_stat_t, cpu_recycle_ring_congestion)
        },
        { .name = "cpu_rx_psram_drop", .help = "Drop due to PSRAM pasring error in CPU RX path",
            .size = sizeof(uint16_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_system_ds_stat_t, cpu_rx_psram_drop)
        },
        { .name = "iptv_hash_lkp_miss_drop", .help = "Drop due to IPTV Hash lookup miss (DA \\ DIP)",
            .size = sizeof(uint16_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_system_ds_stat_t, iptv_hash_lkp_miss_drop)
        },
        { .name = "iptv_sip_vid_lkp_miss_drop", .help = "Drop due to IPTV channel SIP \\ VID lookup miss",
            .size = sizeof(uint16_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_system_ds_stat_t, iptv_sip_vid_lkp_miss_drop)
        },
        { .name = "iptv_invalid_ctx_entry_drop", .help = "Drop due to IPTV channel invalid ctx entry",
            .size = sizeof(uint16_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_system_ds_stat_t, iptv_invalid_ctx_entry_drop)
        },
        { .name = "cpu_tx_copy_no_fpm", .help = "Drop due no FPM when CPU TX copy",
            .size = sizeof(uint16_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_system_ds_stat_t, cpu_tx_copy_no_fpm)
        },
        { .name = "cpu_tx_copy_no_sbpm", .help = "Drop due no SBPM when CPU TX copy",
            .size = sizeof(uint16_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_system_ds_stat_t, cpu_tx_copy_no_sbpm)
        },
        { .name = "eth_flow_drop_action", .help = "Drop due flow drop action",
            .size = sizeof(uint16_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_system_ds_stat_t, flow_drop_action)
        },
        { .name = "iptv_first_repl_disp_nack_drop", .help = "Drop due to unavilable dispatcher buffer - first replication",
            .size = sizeof(uint16_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_system_ds_stat_t, iptv_first_repl_disp_nack_drop)
        },
        { .name = "iptv_other_repl_disp_nack_drop", .help = "Drop due to unavilable dispatcher buffer - other replication",
            .size = sizeof(uint16_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_system_ds_stat_t, iptv_other_repl_disp_nack_drop)
        },
        { .name = "iptv_fpm_nack_drop", .help = "Drop due to exhaustion of FPM buffers",
            .size = sizeof(uint16_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_system_ds_stat_t, iptv_fpm_nack_drop)
        },
        { .name = "rx_mirror_cpu_mcast_exception", .help = "Drop due RX mirroring or CPU/WLAN Multicast exception",
            .size = sizeof(uint16_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_system_ds_stat_t, rx_mirror_cpu_mcast_exception)
        },
        { .name = "ingress_resources_congestion", .help = "Drop due resources ingress congestion",
            .size = sizeof(uint16_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_system_ds_stat_t, ingress_resources_congestion)
        },
        { .name = "egress_resources_congestion", .help = "Drop due resources egress congestion",
            .size = sizeof(uint16_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_system_ds_stat_t, egress_resources_congestion)
        },
        BDMF_ATTR_LAST
    },
};
DECLARE_BDMF_AGGREGATE_TYPE(system_ds_stat_type);

int system_attr_stat_read(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    void *val, uint32_t size)
{
    rdpa_system_stat_t *stat = (rdpa_system_stat_t *)val;
    int rc;

    memset(stat, 0, sizeof(rdpa_system_stat_t));

    rc = drv_cntr_varios_counter_get(COUNTER_DROP_CONNECTION_ACTION_DROP_ID, &stat->ds.connection_action);
    rc = rc ? rc : drv_cntr_varios_counter_get(COUNTER_CPU_RX_FEED_RING_CONGESTION, &stat->ds.cpu_rx_ring_congestion);
    rc = rc ? rc : drv_cntr_varios_counter_get(COUNTER_CPU_RECYCLE_RING_CONGESTION,
        &stat->ds.cpu_recycle_ring_congestion);
    rc = rc ? rc : drv_cntr_varios_counter_get(COUNTER_CPU_RX_PSRAM_DROP, &stat->ds.cpu_rx_psram_drop);
    rc = rc ? rc : drv_cntr_varios_counter_get(COUNTER_IPTV_HASH_LKP_MISS_DROP, &stat->ds.iptv_hash_lkp_miss_drop);
    rc = rc ? rc : drv_cntr_varios_counter_get(COUNTER_IPTV_SIP_VID_LKP_MISS_DROP,
        &stat->ds.iptv_sip_vid_lkp_miss_drop);
    rc = rc ? rc : drv_cntr_varios_counter_get(COUNTER_IPTV_INVALID_CTX_ENTRY_DROP,
        &stat->ds.iptv_invalid_ctx_entry_drop);
    rc = rc ? rc : drv_cntr_varios_counter_get(COUNTER_CPU_TX_COPY_NO_FPM, &stat->ds.cpu_tx_copy_no_fpm);
    rc = rc ? rc : drv_cntr_varios_counter_get(COUNTER_CPU_TX_COPY_NO_SBPM, &stat->ds.cpu_tx_copy_no_sbpm);
    rc = rc ? rc : drv_cntr_varios_counter_get(COUNTER_ETHERNET_FLOW_DROP_ACTION, &stat->ds.flow_drop_action);
    rc = rc ? rc : drv_cntr_varios_counter_get(COUNTER_IPTV_FIRST_REPL_DISP_ALLOC_NACK_DROP,
        &stat->ds.iptv_first_repl_disp_nack_drop);
    rc = rc ? rc : drv_cntr_varios_counter_get(COUNTER_IPTV_OTHER_REPL_DISP_ALLOC_NACK_DROP,
        &stat->ds.iptv_other_repl_disp_nack_drop);
    rc = rc ? rc : drv_cntr_varios_counter_get(COUNTER_IPTV_FPM_ALLOC_NACK_DROP, &stat->ds.iptv_fpm_nack_drop);
    rc = rc ? rc : drv_cntr_varios_counter_get(COUNTER_SBPM_ALLOC_EXCEPTION_DROP,
        &stat->ds.rx_mirror_cpu_mcast_exception);
    rc = rc ? rc : drv_cntr_varios_counter_get(COUNTER_DROP_RESOURCE_CONGESTION_INGRESS,
        &stat->ds.ingress_resources_congestion);
    rc = rc ? rc : drv_cntr_varios_counter_get(COUNTER_DROP_RESOURCE_CONGESTION_EGRESS,
        &stat->ds.egress_resources_congestion);    
        
    stat->us.empty = 0;
    return rc;
}

int system_attr_tod_read(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    void *val, uint32_t size)
{
    rdpa_system_tod_t *system_tod = (rdpa_system_tod_t *)val;
    rdpa_wan_type wan_type = rdpa_wan_if_to_wan_type(rdpa_wan_type_to_if(rdpa_wan_gpon));
#ifdef CONFIG_BCM_GPON_TODD
    gpon_todd_tstamp_t tstamp;
    uint64_t ts;
#endif

    memset(system_tod, 0, sizeof(rdpa_system_tod_t));

    if (wan_type == rdpa_wan_gpon || wan_type == rdpa_wan_xgpon)
    {
#ifdef CONFIG_BCM_GPON_TODD
        gpon_todd_get_tod(&tstamp, &ts);
        system_tod->sec_ms = tstamp.sec_ms;
        system_tod->sec_ls = tstamp.sec_ls;
        system_tod->nsec = tstamp.nsec;
        system_tod->ts48_nsec = ts;
#endif

        return 0;
    }

    return BDMF_ERR_NOT_SUPPORTED;
}

int system_attr_cpu_reason_to_tc_read_ex(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    void *val, uint32_t size)
{
    rdpa_cpu_reason reason = (rdpa_cpu_reason)index;
    uint8_t *tc = (uint8_t *)val;

    *tc = cpu_reason_to_tc[reason];
    return 0;
}

int system_attr_cpu_reason_to_tc_write_ex(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    const void *val, uint32_t size)
{
    rdpa_cpu_reason reason = (rdpa_cpu_reason)index;
    uint8_t tc = *(uint8_t *)val;

    if (tc > 7)
        return BDMF_ERR_PARM;
    cpu_reason_to_tc[reason] = tc;
    rdd_ag_cpu_rx_cpu_reason_to_tc_set(reason, cpu_reason_to_tc[reason]);
    return 0;
}


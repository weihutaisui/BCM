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
* :>
*/


#include "bdmf_dev.h"
#include "rdpa_api.h"
#include "rdpa_int.h"
#include "rdpa_common.h"
#include "rdd_init.h"
#ifndef LEGACY_RDP
#ifndef XRDP
#include "rdd_ic.h"
#if !defined(WL4908)
#include "rdd_multicast_processing.h"
#endif
#endif
#endif
#ifndef XRDP
#include "rdd_tm.h"
#define QM_QUEUE_MAX_DYNAMIC_QUANTITY 0
#else
#include "rdd_tcam_ic.h"
#include "rdd_common.h"
#endif


#include "rdp_version.h"
#include "rdpa_system_ex.h"

static int _get_rcs_version(char *ver);
static void _get_fw_version(system_drv_priv_t *system);

struct bdmf_object *system_object;
int triple_tag_detect_ref_count = 0;
int num_wan = 0;
int num_lan = 0;

bdmf_object_handle ds_transparent_vlan_action;
bdmf_object_handle us_transparent_vlan_action;

/** This optional callback is called called at object init time
 *  before initial attributes are set.
 *  If function returns error code !=0, object creation is aborted
 */
static int system_pre_init(struct bdmf_object *mo)
{
    /* set object name */
    system_drv_priv_t *system = (system_drv_priv_t *)bdmf_obj_data(mo);

    system_object = mo;

    system->sw_version.rdpa_version_major = RDP_VERSION_MAJOR;
    system->sw_version.rdpa_version_minor = RDP_VERSION_MINOR;
    system->sw_version.rdpa_version_branch = RDP_VERSION_BRANCH;
    system->sw_version.rdpa_version_sw_revision = _get_rcs_version(RDP_VERSION_SW);
    system->cfg.headroom_size = RDPA_DS_LITE_HEADROOM_SIZE;
#ifdef G9991
    system->cfg.headroom_size = 0;
#endif
    system->init_cfg.switching_mode = rdpa_switching_none;
    system->init_cfg.ip_class_method = rdpa_method_fc;
    system->init_cfg.runner_ext_sw_cfg.enabled = 0;
    system->init_cfg.runner_ext_sw_cfg.emac_id = rdpa_emac_none;
    system->init_cfg.runner_ext_sw_cfg.type = rdpa_brcm_none;

    system->init_cfg.enabled_emac = RDPA_PORT_ALL_EMACS;
    system->init_cfg.gbe_wan_emac = rdpa_emac_none;
    system->init_cfg.dpu_split_scheduling_mode = 0;
#if defined(BCM6858)
    /* 6858 tables size are 1K (can't be changed)  */
    system->init_cfg.iptv_table_size = rdpa_table_1024_entries;
#else
    system->init_cfg.iptv_table_size = rdpa_table_256_entries;
#endif
    system->cfg.car_mode = 0;
    system->cfg.ic_dbg_stats = 0;
    system->cfg.force_dscp_to_pbit_us = 0;
    system->cfg.force_dscp_to_pbit_ds = 0;

    system->cfg.mtu_size = RDPA_MTU;
    system->cfg.inner_tpid = 0x8100;
    system->cfg.outer_tpid = 0x88a8;
    system->cfg.add_always_tpid = 0x0;
    system->cfg.options = 0;

    system->dp_bitmask[rdpa_dir_ds] = 0;
    system->dp_bitmask[rdpa_dir_us] = 0;

    memset(system->tpids_detect, 0, sizeof(system->tpids_detect));

    return system_pre_init_ex(mo);
}

static int _get_rcs_version(char *ver)
{
    int v;

    sscanf(ver, "$Change: %d $", &v);
    return v;
}

#ifndef XRDP
extern char *rdpa_version_fw_a;
extern char *rdpa_version_fw_b;
extern char *rdpa_version_fw_c;
extern char *rdpa_version_fw_d;
#else
extern char *rdpa_version_fw_0;
extern char *rdpa_version_fw_1;
extern char *rdpa_version_fw_2;
extern char *rdpa_version_fw_3;
#endif

#ifndef BCM6846
#define NUM_OF_IMAGES 4 /* TODO: need to parametrize the amount for 6858 */
#else
#define NUM_OF_IMAGES 3
#endif
static void _get_fw_version(system_drv_priv_t *system)
{
    char *versions[NUM_OF_IMAGES] = {
#ifndef XRDP
        rdpa_version_fw_a,
        rdpa_version_fw_b,
        rdpa_version_fw_c,
        rdpa_version_fw_d
#else
        rdpa_version_fw_0,
        rdpa_version_fw_1,
        rdpa_version_fw_2,
#ifndef BCM6846
        rdpa_version_fw_3
#endif
#endif
    };
    char *p;
    int i;

    p = system->sw_version.rdpa_version_firmware_revision;

    for (i = 0; i < NUM_OF_IMAGES; i++)
    {
        sprintf(p, "%d", _get_rcs_version(versions[i]));
        p += strlen(p);
        if (i < (NUM_OF_IMAGES - 1))
            strcat(p++, ".");
    }
}

#if !defined(WL4908) && !defined(BCM63158)
static int system_post_init_def_vlan_actions_cfg(struct bdmf_object *mo)
{
    int rc;
    BDMF_MATTR(ds_vlan_action_attr, rdpa_vlan_action_drv());
    BDMF_MATTR(us_vlan_action_attr, rdpa_vlan_action_drv());
    rdpa_vlan_action_cfg_t action = {};

    /* create ds transparent vlan action*/
    rc = rdpa_vlan_action_index_set(ds_vlan_action_attr, RDPA_DS_TRANSPARENT_VLAN_ACTION);

    rc = rc ? rc : rdpa_vlan_action_dir_set(ds_vlan_action_attr, rdpa_dir_ds);

    rc = rc ? rc : rdpa_vlan_action_action_set(ds_vlan_action_attr, &action);

    rc = rc ? rc : bdmf_new_and_set(rdpa_vlan_action_drv(), mo, ds_vlan_action_attr, &ds_transparent_vlan_action);
    if (rc < 0)
        BDMF_TRACE_RET_OBJ(rc, mo, "Failed to create ds transparent vlan action\n");

    /* create us transparent vlan action*/
    rc = rc ? rc : rdpa_vlan_action_index_set(us_vlan_action_attr, RDPA_US_TRANSPARENT_VLAN_ACTION);

    rc = rc ? rc : rdpa_vlan_action_dir_set(us_vlan_action_attr, rdpa_dir_us);

    rc = rc ? rc : rdpa_vlan_action_action_set(us_vlan_action_attr, &action);

    rc = rc ? rc : bdmf_new_and_set(rdpa_vlan_action_drv(), mo, us_vlan_action_attr, &us_transparent_vlan_action);

    return rc;
}
#endif

static int system_post_init_tpid_detect_cfg(struct bdmf_object *mo)
{
    system_drv_priv_t *system = (system_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_tpid_detect_cfg_t tpid_detect_cfg = {};
    uint8_t cntr;
    int rc = 0;

    for (cntr = rdpa_tpid_detect_0x8100; cntr < rdpa_tpid_detect__num_of; ++cntr)
    {
        if (((cntr == rdpa_tpid_detect_0x8100) && (memcmp(&system->tpids_detect[cntr], &tpid_detect_cfg,
            sizeof(rdpa_tpid_detect_cfg_t)) == 0)) ||
            ((cntr == rdpa_tpid_detect_0x88A8) && (memcmp(&system->tpids_detect[cntr], &tpid_detect_cfg,
            sizeof(rdpa_tpid_detect_cfg_t)) == 0)))
        {
            /* Set: Pre-Defined */
            tpid_detect_cfg.otag_en = 1;
            tpid_detect_cfg.itag_en = 1;
        }
        else
        {
            /* Set: configuration from object data */
            tpid_detect_cfg.otag_en = system->tpids_detect[cntr].otag_en;
            tpid_detect_cfg.itag_en = system->tpids_detect[cntr].itag_en;
        }

        tpid_detect_cfg.triple_en = system->tpids_detect[cntr].triple_en;
        tpid_detect_cfg.val_udef = system->tpids_detect[cntr].val_udef;

        /* Configure the init configuration from object info */
        rc = rc ? rc : _tpid_detect_cfg(mo, cntr, &tpid_detect_cfg);

        /* Reset the temp configuration structure */
        memset(&tpid_detect_cfg, 0, sizeof(rdpa_tpid_detect_cfg_t));
    }

    return rc;
}

static int system_post_init_cpu_host_cfg(struct bdmf_object *mo)
{
    BDMF_MATTR(cpu_attrs, rdpa_cpu_drv());
    bdmf_object_handle cpu;

    rdpa_cpu_index_set(cpu_attrs, rdpa_cpu_host);
    return bdmf_new_and_set(rdpa_cpu_drv(), mo, cpu_attrs, &cpu);
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
static int system_post_init(struct bdmf_object *mo)
{
    system_drv_priv_t *system = (system_drv_priv_t *)bdmf_obj_data(mo);
    int rc;

    /* set object name */
    snprintf(mo->name, sizeof(mo->name), "system");
    system_object = mo;

    _get_fw_version(system);

    rc = system_post_init_enumerate_emacs(mo);
    if (rc)
        BDMF_TRACE_RET_OBJ(rc, mo, "Failed to enumerate emacs\n");

    rc = system_data_path_init();
    if (rc)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_INTERNAL, mo, "Failed system data path init rc=%d\n", rc);

#if !defined(WL4908) && !defined(BCM63158)
    rc = system_post_init_def_vlan_actions_cfg(mo);
    if (rc < 0)
        BDMF_TRACE_RET_OBJ(rc, mo, "Failed to create us transparent vlan action\n");

    rdd_ic_debug_mode_enable(system->cfg.ic_dbg_stats);
#endif

    rc = system_post_init_cpu_host_cfg(mo);
    if (rc < 0)
        BDMF_TRACE_RET_OBJ(rc, mo, "Failed to create cpu object\n");

    rc = system_post_init_ex(mo);
    if (rc < 0)
        BDMF_TRACE_RET_OBJ(rc, mo, "system_post_init_ex failed\n");

    rc = system_post_init_tpid_detect_cfg(mo);
    if (rc < 0)
        BDMF_TRACE_RET_OBJ(rc, mo, "Failed to configure TPID detect\n");

    rc = system_attr_cfg_write_ex(mo, NULL, 0, &system->cfg, sizeof(system->cfg));
    if (rc)
        BDMF_TRACE_RET_OBJ(rc, mo, "Failed to set platform-specific run-time configuration\n");
#ifndef WL4908
    rdd_rate_limit_overhead_cfg(system->cfg.rate_limit_overhead);
#endif    
    return 0;
}

static void system_pre_destroy(struct bdmf_object *mo)
{
    rdpa_tpid_detect_cfg_t tpid_detect_cfg = {};
    uint8_t cntr;
    int rc;

    if (mo->state != bdmf_state_active)
        return;

    /* TPID Detect: Disable all */
    for (cntr = rdpa_tpid_detect_0x8100; cntr < rdpa_tpid_detect__num_of; ++cntr)
    {
        rc = _tpid_detect_cfg(mo, cntr, &tpid_detect_cfg);
        if (rc)
            break;
    }
    rdd_exit();
}

static void system_destroy(struct bdmf_object *mo)
{
    system_object = NULL;
}

/*
 * system attribute access
 */

/* us qos scheduling method  */
const bdmf_attr_enum_table_t rdpa_vlan_switching_enum_table =
{
    .type_name = "rdpa_vlan_switching",
    .values = {
        {"vlan_aware", rdpa_vlan_aware_switching},
        {"mac_based", rdpa_mac_based_switching},
        {"none", rdpa_switching_none},
        {NULL, 0}
    }
};

/* iptv table size enum values */
const bdmf_attr_enum_table_t rdpa_iptv_table_size_enum_table =
{
    .type_name = "rdpa_iptv_table_size", .help = "IPTV table size modes",
    .values = {
        {"256", rdpa_table_256_entries},
        {"1024", rdpa_table_1024_entries},
        {NULL, 0}
    }
};

const bdmf_attr_enum_table_t rdpa_runner_ext_sw_type_table =
{
    .type_name = "rdpa_runner_ext_sw_type",
    .values = {
        {"type0", rdpa_brcm_hdr_opcode_0},
        {"type1", rdpa_brcm_hdr_opcode_1},
        {"fttdp", rdpa_brcm_fttdp},
        {"none", rdpa_brcm_none},
        {NULL, 0}
    }
};

static const bdmf_attr_enum_table_t rdpa_tpid_detect_enum_table =
{
    .type_name = "rdpa_tpid_detect_t", .help = "TPID Detect",
    .values = {
        {"0x8100", rdpa_tpid_detect_0x8100},
        {"0x88A8", rdpa_tpid_detect_0x88A8},
        {"0x9100", rdpa_tpid_detect_0x9100},
        {"0x9200", rdpa_tpid_detect_0x9200},
        {"udef_1", rdpa_tpid_detect_udef_1},
        {"udef_2", rdpa_tpid_detect_udef_2},
        {NULL, 0}
    }
};

/** Debug feature: redirect packets to CPU */
static const bdmf_attr_enum_table_t rdpa_rx_redirect_cpu_enum_table =
{
    .type_name = "rdpa_rx_redirect_cpu_t", .help = "Redirect packets to CPU (debug feature).",
    .values = {
        {"disable", rdpa_rx_redirect_to_cpu_disabled},
        {"trap_all", rdpa_rx_redirect_to_cpu_all},
        {"trap_flows", rdpa_rx_redirect_to_cpu_trap_flows},
        {NULL, 0}
    }
};

/*  system aggregate type */
struct bdmf_aggr_type system_ruuner_ext_sw_type =
{
    .name = "runner_ext_sw_cfg", .struct_name = "rdpa_runner_ext_sw_cfg",
    .help = "Runner external switch configuration",
    .fields = (struct bdmf_attr[]) {
        { .name = "enabled", .help = "Is external switch exist",
            .type = bdmf_attr_boolean,
            .size = sizeof(bdmf_boolean), .offset = offsetof(rdpa_runner_ext_sw_cfg_t, enabled)
        },
        { .name = "emac_id", .help = "Physical connected emac",
            .type = bdmf_attr_enum, .ts.enum_table = &rdpa_emac_enum_table,
            .size = sizeof(rdpa_emac), .offset = offsetof(rdpa_runner_ext_sw_cfg_t, emac_id)
        },
        { .name = "type", .help = "External switch type",
            .type = bdmf_attr_enum, .ts.enum_table = &rdpa_runner_ext_sw_type_table,
            .size = sizeof(rdpa_ext_sw_type), .offset = offsetof(rdpa_runner_ext_sw_cfg_t, type)
        },
        BDMF_ATTR_LAST
    },
};
DECLARE_BDMF_AGGREGATE_TYPE(system_ruuner_ext_sw_type);

struct bdmf_aggr_type system_init_config_type =
{
    .name = "system_init_config", .struct_name = "rdpa_system_init_cfg_t",
    .help = "Initial System Configuration",
    .fields = (struct bdmf_attr[]) {
        { .name = "enabled_emac", .help = "Enabled emacs bitmask", .size = sizeof(int),
            .type = bdmf_attr_enum_mask, .ts.enum_table = &rdpa_emac_enum_table,
            .offset = offsetof(rdpa_system_init_cfg_t, enabled_emac)
        },
        { .name = "wan_emac", .help = "WAN emac in GBE mode", .size = sizeof(rdpa_emac),
            .type = bdmf_attr_enum, .ts.enum_table = &rdpa_wan_emac_enum_table,
            .offset = offsetof(rdpa_system_init_cfg_t, gbe_wan_emac)
        },
        { .name = "switching_mode", .help = "system vlan switching mode",
#ifdef XRDP
            .flags = BDMF_ATTR_DEPRECATED,
#endif
            .type = bdmf_attr_enum, .ts.enum_table = &rdpa_vlan_switching_enum_table,
            .offset = offsetof(rdpa_system_init_cfg_t, switching_mode)
        },
        { .name = "ip_class_method", .help = "ip class operational mode",
            .type = bdmf_attr_enum, .ts.enum_table = &rdpa_ip_class_method_enum_table,
            .offset = offsetof(rdpa_system_init_cfg_t, ip_class_method)
        },
        { .name = "runner_ext_sw", .help = "Runner external switch configuration",
            .size = sizeof(rdpa_runner_ext_sw_cfg_t),
            .type = bdmf_attr_aggregate, .ts.aggr_type_name = "runner_ext_sw_cfg",
            .offset = offsetof(rdpa_system_init_cfg_t, runner_ext_sw_cfg)
        },
        { .name = "us_ddr_queue_enable", .help = "WAN TX queues DDR offload enable",
            .type = bdmf_attr_boolean,
            .offset = offsetof(rdpa_system_init_cfg_t, us_ddr_queue_enable)
        },
        { .name = "dpu_split_scheduling_mode", .help = "DPU split scheduling mode",
            .type = bdmf_attr_boolean,
            .offset = offsetof(rdpa_system_init_cfg_t, dpu_split_scheduling_mode)
        },
        { .name = "iptv_table_size", .help = "IPTV table size (256 entries / 1024 entries)",
            .type = bdmf_attr_enum, .ts.enum_table = &rdpa_iptv_table_size_enum_table,
            .offset = offsetof(rdpa_system_init_cfg_t, iptv_table_size)
        },
        BDMF_ATTR_LAST
    },
};
DECLARE_BDMF_AGGREGATE_TYPE(system_init_config_type);

struct bdmf_aggr_type system_qm_config_type =
{
    .name = "system_qm_config", .struct_name = "rdpa_qm_cfg_t",
    .help = "Initial Queue Configuration",
    .fields = (struct bdmf_attr[]) {
        { .name = "number_of_ds_queues", .help = "defines the number of egress queues in DS",
                           .type = bdmf_attr_number, .size = sizeof(uint16_t),
                           .min_val = 0, .max_val = QM_QUEUE_MAX_DYNAMIC_QUANTITY,
                           .offset = offsetof(rdpa_qm_cfg_t, number_of_ds_queues)
        },
        { .name = "number_of_us_queues", .help = "defines the number of egress queues in US",
                           .type = bdmf_attr_number, .size = sizeof(uint16_t),
                           .min_val = 0, .max_val = QM_QUEUE_MAX_DYNAMIC_QUANTITY,
                           .offset = offsetof(rdpa_qm_cfg_t, number_of_us_queues)
        },
        { .name = "number_of_service_queues", .help = "defines the number of service queues in DS",
                           .type = bdmf_attr_number, .size = sizeof(uint16_t),
                           .min_val = 0, .max_val = QM_QUEUE_MAX_DYNAMIC_QUANTITY,
                           .offset = offsetof(rdpa_qm_cfg_t, number_of_service_queues)
        },
        BDMF_ATTR_LAST
    },
};
DECLARE_BDMF_AGGREGATE_TYPE(system_qm_config_type);

struct bdmf_aggr_type system_tpid_detect_cfg_type =
{
    .name = "system_tpid_detect_cfg", .struct_name = "rdpa_tpid_detect_cfg_t",
    .help = "TPID Detect",
    .fields = (struct bdmf_attr[]) {
        { .name = "val_udef", .help = "Value, User-Defined",
            .type = bdmf_attr_number,
            .size = sizeof(uint16_t), .offset = offsetof(rdpa_tpid_detect_cfg_t, val_udef),
            .flags = BDMF_ATTR_HEX_FORMAT | BDMF_ATTR_UNSIGNED
        },
        { .name = "otag_enabled", .help = "Outer tag, Enabled flag",
            .type = bdmf_attr_boolean,
            .size = sizeof(bdmf_boolean), .offset = offsetof(rdpa_tpid_detect_cfg_t, otag_en)
        },
        { .name = "itag_enabled", .help = "Inner tag, Enabled flag",
            .type = bdmf_attr_boolean,
            .size = sizeof(bdmf_boolean), .offset = offsetof(rdpa_tpid_detect_cfg_t, itag_en)
        },
        { .name = "triple_enabled", .help = "triple tag (most inner), Enabled flag",
            .type = bdmf_attr_boolean,
            .size = sizeof(bdmf_boolean), .offset = offsetof(rdpa_tpid_detect_cfg_t, triple_en)
        },
        BDMF_ATTR_LAST
    },
};
DECLARE_BDMF_AGGREGATE_TYPE(system_tpid_detect_cfg_type);

struct bdmf_aggr_type system_config_type =
{
    .name = "system_config", .struct_name = "rdpa_system_cfg_t",
    .help = "System Configuration that can be changed in run-time",
    .fields = (struct bdmf_attr[]) {
        { .name = "car_mode", .help = "Is CAR mode enabled",
            .type = bdmf_attr_boolean, .size = sizeof(bdmf_boolean), .offset = offsetof(rdpa_system_cfg_t, car_mode)
        },
        { .name = "headroom_size", .help = "Headroom size ", .size = sizeof(int),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_system_cfg_t, headroom_size)
        },
        { .name = "mtu_size", .help = "MTU size",
            .type = bdmf_attr_number, .size = sizeof(int), .offset = offsetof(rdpa_system_cfg_t, mtu_size),
        },
        { .name = "inner_tpid", .help = "Inner TPID", .type = bdmf_attr_number,
            .size = sizeof(uint16_t), .offset = offsetof(rdpa_system_cfg_t, inner_tpid),
            .flags = BDMF_ATTR_HEX_FORMAT | BDMF_ATTR_UNSIGNED
        },
        { .name = "outer_tpid", .help = "Outer TPID", .type = bdmf_attr_number,
            .size = sizeof(uint16_t), .offset = offsetof(rdpa_system_cfg_t, outer_tpid),
            .flags = BDMF_ATTR_HEX_FORMAT | BDMF_ATTR_UNSIGNED
        },
        { .name = "add_always_tpid", .help = "Always TPID", .type = bdmf_attr_number,
            .size = sizeof(uint16_t), .offset = offsetof(rdpa_system_cfg_t, add_always_tpid),
            .flags = BDMF_ATTR_HEX_FORMAT | BDMF_ATTR_UNSIGNED
        },
        { .name = "ic_dbg_stats", .help = "Enable Ingress class debug statistics",
            .type = bdmf_attr_boolean, .size = sizeof(bdmf_boolean), .offset = offsetof(rdpa_system_cfg_t, ic_dbg_stats)
        },
        { .name = "force_dscp_to_pbit_us", .help = "Force DSCP to Pbit mapping for upstream",
            .type = bdmf_attr_boolean, .size = sizeof(bdmf_boolean),
            .offset = offsetof(rdpa_system_cfg_t, force_dscp_to_pbit_us)
        },
        { .name = "force_dscp_to_pbit_ds", .help = "Force DSCP to Pbit mapping for downstream",
            .type = bdmf_attr_boolean, .size = sizeof(bdmf_boolean),
            .offset = offsetof(rdpa_system_cfg_t, force_dscp_to_pbit_ds)
        },

        { .name = "options", .help = "reserved", .type = bdmf_attr_number,
            .size = sizeof(uint32_t), .offset = offsetof(rdpa_system_cfg_t, options),
            .flags = BDMF_ATTR_HEX_FORMAT | BDMF_ATTR_UNSIGNED
        },
        { .name = "rate_limit_overhead", .help = "Rate limit overhead", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_system_cfg_t, rate_limit_overhead)
        },
        { .name = "rx_cpu_redirect", .help = "RX CPU Redirect Mode (Debug)", .size = sizeof(rdpa_rx_redirect_cpu_t),
            .type = bdmf_attr_enum, .ts.enum_table = &rdpa_rx_redirect_cpu_enum_table,
            .offset = offsetof(rdpa_system_cfg_t, cpu_redirect_mode)
        },

        BDMF_ATTR_LAST
    },
};
DECLARE_BDMF_AGGREGATE_TYPE(system_config_type);

/* Channel selector string to internal value */
static int system_fw_ver_val_to_s(struct bdmf_object *mo,
    struct bdmf_attr *ad, const void *val, char *sbuf, uint32_t size)
{
    system_drv_priv_t *system = (system_drv_priv_t *)bdmf_obj_data(system_object);

    sprintf(sbuf, "%s", system->sw_version.rdpa_version_firmware_revision);
    return 0;
}

struct bdmf_aggr_type system_sw_version =
{
    .name = "sw_version", .struct_name = "rdpa_sw_version_t",
    .help = "SW Version",
    .fields = (struct bdmf_attr[])
    {
        { .name = "rdpa_version_major", .help = "version's major", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_sw_version_t, rdpa_version_major)
        },
        { .name = "rdpa_version_minor", .help = "version's minor", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_sw_version_t, rdpa_version_minor)
        },
        { .name = "rdpa_version_branch", .help = "version's branch", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_sw_version_t, rdpa_version_branch)
        },
        { .name = "rdpa_version_sw_revision", .help = "rdd version", .size = sizeof(uint32_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_sw_version_t, rdpa_version_sw_revision)
        },
        { .name = "rdpa_version_firmware_revision", .help = "fw version", .size = sizeof(RDPA_FW_VER_LEN),
            .type = bdmf_attr_string, .offset = offsetof(rdpa_sw_version_t, rdpa_version_firmware_revision),
            .val_to_s = system_fw_ver_val_to_s
        },
        BDMF_ATTR_LAST
    },
};
DECLARE_BDMF_AGGREGATE_TYPE(system_sw_version);

/* System drop statistics */
struct bdmf_aggr_type system_stat_type =
{
    .name = "system_stat", .struct_name = "rdpa_system_stat_t",
    .help = "System Drop Statistics",
    .fields = (struct bdmf_attr[])
    {
        { .name = "us", .help = "US drop statistics", .type = bdmf_attr_aggregate,
            .ts.aggr_type_name = "system_us_stat", .offset = offsetof(rdpa_system_stat_t, us)
        },
        { .name = "ds", .help = "DS drop statistics", .type = bdmf_attr_aggregate,
            .ts.aggr_type_name = "system_ds_stat", .offset = offsetof(rdpa_system_stat_t, ds)
        },
        BDMF_ATTR_LAST
    },
};
DECLARE_BDMF_AGGREGATE_TYPE(system_stat_type);


/*
 * system attribute access
 */

/* "cfg" attribute "write" callback */
static int system_attr_cfg_write(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    const void *val, uint32_t size)
{
    system_drv_priv_t *system = (system_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_system_cfg_t *cfg = (rdpa_system_cfg_t *)val;
    int rc;

    if (mo->state != bdmf_state_active)
        goto exit;

    rc = system_attr_cfg_write_ex(mo, ad, index, val, size);
    if (rc)
        return rc;

#ifndef WL4908
    rdd_ic_debug_mode_enable(cfg->ic_dbg_stats);
    rdd_rate_limit_overhead_cfg(cfg->rate_limit_overhead);
#endif
exit:
    /* Save configuration */
    system->cfg = *cfg;
    return 0;
}

/* "qm_cfg" attribute "write" callback */
static int system_attr_qm_write(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    const void *val, uint32_t size)
{
    system_drv_priv_t *system = (system_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_qm_cfg_t *qm_cfg = (rdpa_qm_cfg_t *)val;

    if (mo->state == bdmf_state_active)
        return BDMF_ERR_NOT_SUPPORTED;

    /* Save configuration */
    system->qm_cfg = *qm_cfg;
    return 0;
}


static int system_attr_tpid_detect_read(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    void *val, uint32_t size)
{
    system_drv_priv_t *system = (system_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_tpid_detect_cfg_t *tpid_detect = (rdpa_tpid_detect_cfg_t *)val;

    if (!system->tpids_detect[index].otag_en && !system->tpids_detect[index].itag_en &&
        !system->tpids_detect[index].triple_en)
        return BDMF_ERR_NOENT;

    *tpid_detect = system->tpids_detect[index];

    return 0;
}

static int system_attr_tpid_detect_write(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    const void *val, uint32_t size)
{
    system_drv_priv_t *system = (system_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_tpid_detect_t tpid_detect = (rdpa_tpid_detect_t) index;
    rdpa_tpid_detect_cfg_t *tpid_detect_cfg = (rdpa_tpid_detect_cfg_t *)val;
    int rc = 0;

    if (mo->state == bdmf_state_active)
    {
        rc = _tpid_detect_cfg(mo, tpid_detect, tpid_detect_cfg);
        if (rc)
            BDMF_TRACE_RET_OBJ(BDMF_ERR_INTERNAL, mo, "Error configuring tpid detection\n");
    }

    /* Update the attr in object */
    system->tpids_detect[index] = *tpid_detect_cfg;

    return rc;
}

/*
 * drop_precedence attribute access
 */
/** "drop_precedence" attribute's "write" callback */
int system_attr_dp_write(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    const void *val, uint32_t size)
{
    system_drv_priv_t *system = (system_drv_priv_t *)bdmf_obj_data(mo);
    bdmf_boolean drop = *(bdmf_boolean *)val;
    rdpa_dp_key_t *key = (rdpa_dp_key_t *)index;
    uint16_t dp_table = system->dp_bitmask[key->dir];
    uint16_t pbit_dei;

    pbit_dei = ((key->pbit << 1) | key->dei);

    if (drop)
        dp_table |= 1 << pbit_dei;
    else
        dp_table &= ~(1 << pbit_dei);

    if (mo->state == bdmf_state_active)
        rdd_drop_precedence_cfg(key->dir, dp_table);

    system->dp_bitmask[key->dir] = dp_table;

    return 0;
}

int system_attr_dp_read(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    void *val, uint32_t size)
{
    system_drv_priv_t *system = (system_drv_priv_t *)bdmf_obj_data(mo);
    bdmf_boolean *drop = (bdmf_boolean *)val;
    rdpa_dp_key_t *key = (rdpa_dp_key_t *)index;
    uint16_t pbit_dei;

    pbit_dei = (key->pbit << 1) | key->dei;
    *drop = system->dp_bitmask[key->dir] & (1 << pbit_dei) ? 1 : 0;
    if (!*drop)
        return BDMF_ERR_NOENT;

    return 0;
}

int system_attr_dp_get_next(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index *index)
{
    rdpa_dp_key_t *entry = (rdpa_dp_key_t *)index;

    if (*index == BDMF_INDEX_UNASSIGNED)
    {
        entry->dir = rdpa_dir_ds;
        entry->dei = 0;
        entry->pbit = 0;
        return 0;
    }

    if (entry->pbit == 7 && entry->dei == 1)
    {
        if (entry->dir == rdpa_dir_us)
            return BDMF_ERR_NO_MORE;

        entry->dir = rdpa_dir_us;
        entry->dei = 0;
        entry->pbit = 0;
        return 0;
    }

    if (entry->dei == 0)
    {
        entry->dei = 1;
    }
    else
    {
        entry->dei = 0;
        entry->pbit++;
    }

    return 0;
}

/* dp_key aggregate type */
struct bdmf_aggr_type dp_key_type =
{
    .name = "dp_key", .struct_name = "rdpa_dp_key_t",
    .help = "drop precedence pbit/dei per direction value ",
    .fields = (struct bdmf_attr[])
    {
        { .name = "dir", .help = "Traffic Direction",
            .type = bdmf_attr_enum, .ts.enum_table = &rdpa_traffic_dir_enum_table,
            .size = sizeof(rdpa_traffic_dir), .offset = offsetof(rdpa_dp_key_t, dir)
        },
        { .name = "pbit", .help = "Pbit value",
            .type = bdmf_attr_number, .min_val = 0, .max_val = 7,
            .offset = offsetof(rdpa_dp_key_t , pbit), .size = sizeof(rdpa_pbit),
        },
        { .name = "dei", .help = "Dei value",
            .type = bdmf_attr_number, .min_val = 0, .max_val = 1,
            .offset = offsetof(rdpa_dp_key_t , dei), .size = sizeof(uint8_t),
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE(dp_key_type);

/* 'system_tod' aggregate type */
struct bdmf_aggr_type system_tod_type =
{
    .name = "system_tod",
    .struct_name = "rdpa_system_tod_t",
    .help = "Time Of Day",
    .fields = (struct bdmf_attr[])
    {
        {
            .name = "tod_sec_ms", .help = "TOD: Seconds, MS bits", .type = bdmf_attr_number,
            .size = sizeof(uint16_t), .flags = BDMF_ATTR_UNSIGNED,
            .offset = offsetof(rdpa_system_tod_t, sec_ms)
        },
        {
            .name = "tod_sec_ls", .help = "TOD: Seconds, LS bits", .type = bdmf_attr_number,
            .size = sizeof(uint32_t), .flags = BDMF_ATTR_UNSIGNED,
            .offset = offsetof(rdpa_system_tod_t, sec_ls)
        },
        {
            .name = "tod_nsec", .help = "TOD: Nanoseconds", .type = bdmf_attr_number,
            .size = sizeof(uint32_t), .flags = BDMF_ATTR_UNSIGNED,
            .offset = offsetof(rdpa_system_tod_t, nsec)
        },
        {
            .name = "ts48_nsec", .help = "Timestamp: Nanoseconds", .type = bdmf_attr_number,
            .size = sizeof(uint64_t), .flags = BDMF_ATTR_UNSIGNED,
            .offset = offsetof(rdpa_system_tod_t, ts48_nsec)
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE(system_tod_type);

/* Object attribute descriptors */
static struct bdmf_attr system_attrs[] =
{
    { .name = "init_cfg", .help = "Initial System Configuration", .offset = offsetof(system_drv_priv_t, init_cfg),
        .type = bdmf_attr_aggregate, .ts.aggr_type_name = "system_init_config",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE_INIT | BDMF_ATTR_CONFIG | BDMF_ATTR_MANDATORY
    },
    { .name = "cfg", .help = "System Configuration that can be changes in run-time",
        .offset = offsetof(system_drv_priv_t, cfg),
        .type = bdmf_attr_aggregate, .ts.aggr_type_name = "system_config",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG,
        .write = system_attr_cfg_write
    },
    { .name = "sw_version", .help = "software version",
        .type = bdmf_attr_aggregate, .ts.aggr_type_name = "sw_version",
        .offset = offsetof(system_drv_priv_t, sw_version), .flags = BDMF_ATTR_READ
    },
    { .name = "clock_gate", .help = "Enable/Disable clock auto-gating feature",
        .type = bdmf_attr_boolean, .size = sizeof(bdmf_boolean),
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE,
        .read = system_attr_clock_gate_read,
        .write = system_attr_clock_gate_write
    },
    { .name = "stat", .help = "Drop statistics",
        .type = bdmf_attr_aggregate, .ts.aggr_type_name = "system_stat",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_STAT | BDMF_ATTR_NO_AUTO_GEN,
        .read = system_attr_stat_read
    },
    { .name = "drop_precedence", .help = "Drop precedence flow entry",
        .type = bdmf_attr_boolean, .size = sizeof(bdmf_boolean),
        .array_size = 32, .index_type = bdmf_attr_aggregate, .index_ts.aggr_type_name = "dp_key",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG,
        .offset = offsetof(system_drv_priv_t, dp_bitmask),
        .write = system_attr_dp_write,
        .read = system_attr_dp_read,
        .get_next = system_attr_dp_get_next,
    },
    { .name = "tpid_detect", .help = "TPID Detect",
        .array_size = rdpa_tpid_detect__num_of,
        .index_type = bdmf_attr_enum, .index_ts.enum_table = &rdpa_tpid_detect_enum_table,
        .type = bdmf_attr_aggregate, .ts.aggr_type_name = "system_tpid_detect_cfg",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG,
        .offset = offsetof(system_drv_priv_t, tpids_detect),
        .write = system_attr_tpid_detect_write, .read = system_attr_tpid_detect_read,
    },
    { .name = "tod", .help = "Time Of Day",
        .type = bdmf_attr_aggregate, .ts.aggr_type_name = "system_tod",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_CONFIG,
        .read = system_attr_tod_read
    },
    { .name = "cpu_reason_to_tc", .help = "CPU Reason to TC global configuration",
        .array_size = rdpa_cpu_reason__num_of,
        .index_type = bdmf_attr_enum, .index_ts.enum_table = &rdpa_cpu_reason_enum_table,
        .type = bdmf_attr_number, .size = sizeof(uint8_t),
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG,
        .write = system_attr_cpu_reason_to_tc_write_ex, .read = system_attr_cpu_reason_to_tc_read_ex,
    },
    { .name = "qm_cfg", .help = "Configuration for dynamic Queue allocation", .offset = offsetof(system_drv_priv_t, qm_cfg),
        .type = bdmf_attr_aggregate, .ts.aggr_type_name = "system_qm_config",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE_INIT | BDMF_ATTR_CONFIG,
        .write = system_attr_qm_write
    },
    BDMF_ATTR_LAST
};


static int system_drv_init(struct bdmf_type *drv);
static void system_drv_exit(struct bdmf_type *drv);

struct bdmf_type system_drv =
{
    .name = "system",
    .description = "RDPA system",
    .drv_init = system_drv_init,
    .drv_exit = system_drv_exit,
    .pre_init = system_pre_init,
    .post_init = system_post_init,
    .pre_destroy = system_pre_destroy,
    .destroy = system_destroy,
    .extra_size = sizeof(system_drv_priv_t),
    .aattr = system_attrs,
    .max_objs = 1,
    .flags = BDMF_DRV_FLAG_ROOT,
};
DECLARE_BDMF_TYPE(rdpa_system, system_drv);

/* Init/exit module. Cater for GPL layer */
static int system_drv_init(struct bdmf_type *drv)
{
#ifdef BDMF_DRIVER_GPL_LAYER
    f_rdpa_system_drv = rdpa_system_drv;
    f_rdpa_system_get = rdpa_system_get;
#endif
    return 0;
}

static void system_drv_exit(struct bdmf_type *drv)
{
#ifdef BDMF_DRIVER_GPL_LAYER
    f_rdpa_system_drv = NULL;
    f_rdpa_system_get = NULL;
#endif
}

/** Get system object by key
 * \param[out] system_obj     Object handle
 * \return  0=OK or error <0
 */
int rdpa_system_get(bdmf_object_handle *system_obj)
{
    if (system_object)
    {
        bdmf_get(system_object);
        *system_obj = system_object;
        return 0;
    }
    return BDMF_ERR_NOENT;
}

/*************************************************************************
 * Functions in use by other drivers internally
 ************************************************************************/

bdmf_boolean rdpa_is_car_mode(void)
{
    system_drv_priv_t *system = (system_drv_priv_t *)bdmf_obj_data(system_object);
    rdpa_system_cfg_t *cfg = &(system->cfg);
    return cfg->car_mode;
}

bdmf_boolean rdpa_ic_dbg_stats_enabled(void)
{
    system_drv_priv_t *system = (system_drv_priv_t *)bdmf_obj_data(system_object);
    rdpa_system_cfg_t *cfg = &(system->cfg);
    return cfg->ic_dbg_stats;
}

bdmf_boolean rdpa_is_gpon_or_xgpon_mode(void)
{
    rdpa_wan_type wan_type = rdpa_wan_if_to_wan_type(rdpa_wan_type_to_if(rdpa_wan_gpon));

    return wan_type == rdpa_wan_gpon || wan_type == rdpa_wan_xgpon;
}

bdmf_boolean rdpa_is_epon_or_xepon_mode(void)
{
    rdpa_wan_type wan_type = rdpa_wan_if_to_wan_type(rdpa_wan_type_to_if(rdpa_wan_epon));
    int is_ae_enable = rdpa_is_epon_ae_mode();

    return (wan_type == rdpa_wan_epon || wan_type == rdpa_wan_xepon) && !is_ae_enable;
}

bdmf_boolean rdpa_is_gbe_mode(void)
{
    return rdpa_wan_if_to_wan_type(rdpa_wan_type_to_if(rdpa_wan_gbe)) == rdpa_wan_gbe;
}

bdmf_boolean rdpa_is_dsl_mode(void)
{
    return rdpa_wan_if_to_wan_type(rdpa_wan_type_to_if(rdpa_wan_dsl)) == rdpa_wan_dsl;
}

bdmf_boolean rdpa_is_fttdp_mode(void)
{
    const rdpa_system_init_cfg_t *init_cfg = _rdpa_system_init_cfg_get();

    return (init_cfg->runner_ext_sw_cfg.enabled && init_cfg->runner_ext_sw_cfg.type == rdpa_brcm_fttdp);
}

bdmf_boolean rdpa_is_ext_switch_mode(void)
{
    const rdpa_system_init_cfg_t *init_cfg = _rdpa_system_init_cfg_get();

    return init_cfg->runner_ext_sw_cfg.enabled &&
        (init_cfg->runner_ext_sw_cfg.type == rdpa_brcm_hdr_opcode_0 ||
            init_cfg->runner_ext_sw_cfg.type == rdpa_brcm_hdr_opcode_1);
}

rdpa_emac rdpa_gbe_wan_emac(void)
{
    const rdpa_system_init_cfg_t *init_cfg = _rdpa_system_init_cfg_get();

    if (rdpa_wan_if_to_wan_type(rdpa_wan_type_to_if(rdpa_wan_gbe)) == rdpa_wan_gbe)
        return init_cfg->gbe_wan_emac;
    return rdpa_emac__num_of;
}

bdmf_boolean is_triple_tag_detect(void)
{
    return (bdmf_boolean)triple_tag_detect_ref_count;
}

int _rdpa_system_num_wan_get(void)
{
    return num_wan;
}

int _rdpa_system_num_lan_get(void)
{
    return num_lan;
}

const rdpa_system_init_cfg_t *_rdpa_system_init_cfg_get(void)
{
    system_drv_priv_t *system = (system_drv_priv_t *)bdmf_obj_data(system_object);

#if !defined(XRDP) || defined(BCM6858)
    system->init_cfg.iptv_table_size = rdpa_table_1024_entries;
#endif
    return &system->init_cfg;
}

const rdpa_qm_cfg_t *_rdpa_system_qm_cfg_get(void)
{
    system_drv_priv_t *system = (system_drv_priv_t *)bdmf_obj_data(system_object);

    return &system->qm_cfg;
}

const rdpa_system_cfg_t *_rdpa_system_cfg_get(void)
{
    system_drv_priv_t *system = (system_drv_priv_t *)bdmf_obj_data(system_object);

    return &system->cfg;
}


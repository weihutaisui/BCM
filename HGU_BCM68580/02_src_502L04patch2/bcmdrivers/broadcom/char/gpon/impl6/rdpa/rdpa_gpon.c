
/*
* <:copyright-BRCM:2015:proprietary:gpon
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

/*
 * rdpa_gpon.c
 *
 *  Created on: Aug 23, 2012
 *      Author: igort
 */

#include <shared_utils.h>
#include <bdmf_dev.h>
#include <rdpa_api.h>
#include "rdpa_gpon.h"
#include "ngpon_sm.h"
#include "rdpa_ag_gpon.h"
#include "rdpa_gpon_cfg.h"
#ifdef CONFIG_BCM_XRDP
#include "xrdp_drv_bbh_rx_ag.h"
#include "xrdp_drv_bbh_tx_ag.h"
#include "rdp_drv_bbh_tx.h"
#endif
#if (defined(CONFIG_BCM96838) || defined(CONFIG_BCM96848))
#include "rdp_drv_bbh.h"
#include <rdd_runner_defs_auto.h>
#endif
#ifndef BDMF_SYSTEM_SIM
#include "rogue_drv.h"
#include <linux/interrupt.h>
#include <access_macros.h>
#ifdef USE_BDMF_SHELL
#include <bdmf_shell.h>
#endif
#include <rdpa_mw_cpu_queue_ids.h>
#include <rdp_cpu_ring.h>
#include "pmd.h"
#include "gpon_tod_gpl.h"
#include "board.h"
#include "pon_sm_common.h"
/*
 *  Internal Functions declaration
 */

#ifdef CONFIG_BCM_XRDP
extern int rdpa_wan_tx_bbh_flush_status_get(uint8_t tcont_id, bdmf_boolean *bbh_flush_done_p);
#endif

static void p_isr_ploam(void);
static void p_isr_pon(rdpa_pon_transmit pon_unit);
static void _pon_rx_packet(rdpa_cpu_rx_info_t *info);
static void _rdpa_gpon_tasklet_handler(unsigned long tasklet_arg);
static int initialize_pon(void);
static void _rdpa_indication_callback(PON_API_MESSAGE_OPCODES_DTE message_id, PON_USER_INDICATION_DTE user_indication_message);
static void _rdpa_sc_sc_callback(uint8_t *buffer, uint8_t *flag);
static void _rdpa_gpon_sr_dba_callback(uint32_t runner_queue_id, uint32_t *runner_ddr_occupancy);
static int _rdpa_pon_flush_tcont_fe_bbh(uint8_t tcont_id);
#ifdef CONFIG_BCM_XRDP
static int _rdpa_tcont_enable_set(uint8_t tcont_id,  bdmf_boolean enabled);
#endif
static int rdpa_gpon_irq_connect(void);
static bdmf_boolean rdpa_gem_is_ds_cfg_set(bdmf_index index);
static bdmf_boolean rdpa_gpon_is_alloc_exist(bdmf_index index);
#ifdef USE_BDMF_SHELL
static void register_gpon_shell_commands(void);
static void unregister_gpon_shell_commands(void);
#endif
static int _pon_rx_isr_wrapper(int irq, void *priv);
#endif
static void default_ic(rdpa_pon_indication indication_type,
    rdpa_callback_indication indication_parameters);
#ifdef CONFIG_BCM_GPON_TODD
extern void (*f_gpon_todd_get_tod)(gpon_todd_tstamp_t* tstamp, uint64_t *ts);
extern void (*f_gpon_todd_get_tod_info)(uint32_t* sframe_num_ls,
    uint32_t* sframe_num_ms, gpon_todd_tstamp_t* tstamp_n);
extern void (*f_gpon_todd_set_tod_info)(uint32_t sframe_num_ls,
    uint32_t sframe_num_ms, const gpon_todd_tstamp_t* tstamp_n);
extern void (*f_gpon_todd_reg_1pps_start_cb)(gpon_todd_1pps_ctrl_cb_t onepps_start_cb);
extern void (*f_gpon_tod_get_sfc)(uint32_t *sfc_ls, uint32_t *sfc_ms);
#endif

#ifdef BDMF_DRIVER_GPL_LAYER
extern bdmf_type_handle (*f_rdpa_gpon_drv)(void);
#endif

/*
 *  GPON Object types
 */

#ifndef BDMF_SYSTEM_SIM
struct rdpa_gpon_tasklet_context
{
    struct tasklet_struct gpon_tasklet;
    volatile bdmf_boolean rx_pon_int_source;
    volatile bdmf_boolean tx_pon_int_source;
    volatile bdmf_boolean ploam_int_source;
    volatile bdmf_boolean pmd_int_source;
};

struct rdpa_gpon_tasklet_context gpon_tasklet_info;

static DEFINE_SPINLOCK(tasklet_lock);
#endif

#define RDPA_CPU_RX_QUEUE_GPON_SIZE 32
#define RDPA_PLOAM_PACKET_SIZE 30
#define RDPA_PLOAM_BUFFER_SIZE 24
#define RDPA_PLOAM_SIZE 12
#define RDPA_GPON_DEFAULT_GEM_BLOCK_SIZE 48
#define PLOAM_ENCRYPTION_KEY_SIZE_BYTES 16
#define PLOAM_ENCRYPTION_KEY_SIZE (PLOAM_ENCRYPTION_KEY_SIZE_BYTES*sizeof(uint8_t))
#define RDPA_PMD_DV_SETUP_PATTERN 0xffffff
#define RDPA_PMD_DV_HOLD_PATTERN 0xffffff
#define ALLOC_ID_RANGE_LOW 256
#define ALLOC_ID_RANGE_HIGH 4095
#define GPON_MAX_TCONT 32

#define RDPA_MAX_PREAMBLE_LEN 0x10

#define TCONT_ID_TO_RDPA_WAN_FLOW(tcont_id) (tcont_id == (GPON_MAX_TCONT - 1))? 0 : tcont_id + 1

static struct bdmf_object *gpon_object;
static PON_ACCESS accesses_array;
static int access_read_num;

typedef void (*rdpa_indication_callback)(rdpa_pon_indication indication_type,
    rdpa_callback_indication indication_parameters);

/* GPON object private data */
typedef struct {
    bdmf_index onu_id;                 /**< ONU Index (FFU) */
    rdpa_link_activate_t link_activate; /**< Link Activaton */
    rdpa_gpon_link_cfg_t link_cfg;  /**< Link configuration */
    rdpa_pon_link_state link_state; /** Link state*/
    rdpa_onu_sn_t onu_sn;           /**< ONU serial number */
    rdpa_onu_password_t onu_password;/**< ONU password */
    rdpa_indication_callback user_ic; /**< user indication callback */
    rdpa_gpon_overhead_cfg_t overhead_cfg; /**< overhead configuration */
    uint8_t transceiver_power_level; /**< transceiver power level */
    uint32_t equalization_delay; /**< equalization delay */
    uint8_t encryption_key[PLOAM_ENCRYPTION_KEY_SIZE_BYTES];  /**< encryption key */
    uint32_t tcont_counter_assigment; /**< TCONT counter assigmant */
    rdpa_dba_interval_t dba_interval; /**< DBA interval */
    rdpa_rogue_onu_t rogue_onu; /**< Rogue ONU configuration */
    rdpa_misc_tx_t misc_tx; /** MISC transmit configuration */
    uint32_t gem_block_size; /**< Gem block size */
    rdpa_gpon_mcast_enc_key_param_t mcast_enc_key; /**< Mcast encryption key */
    rdpa_gpon_stack_mode_t stack_mode; /**< Stack mode: gpon/xgpon/ngpon2_10g/ngpon2_2_2g */
    rdpa_gpon_bw_record_cfg_t bw_record_cfg; /**< BW recording Setting */
    rdpa_gpon_burst_prof_t burst_prof[NGPON_BURST_PROFILE_INDEX_HIGH + 1];
} gpon_drv_priv_t;

PON_SM_CALLBACK_FUNC pon_sm_cb;

PON_MAC_MODE stack_mode = GPON_MODE;


/*
 * ENUM tables
 */

static bdmf_attr_enum_table_t pon_admin_state_enum_table = {
    .type_name = "rdpa_pon_admin_state", .help = "PON administrative state",
    .values = {
        {"not_ready", rdpa_pon_admin_state_not_ready},
        {"inactive", rdpa_pon_admin_state_inactive},
        {"active", rdpa_pon_admin_state_active},
        {NULL, 0}
    }
};

static bdmf_attr_enum_table_t pon_sub_state_enum_table = {
    .type_name = "rdpa_pon_sub_state", .help = "PON sub state",
    .values = {
        {"unknown", rdpa_pon_oper_state_none},
        {"standby", rdpa_pon_oper_state_standby},
        {"operational", rdpa_pon_oper_state_operational},
        {"stop", rdpa_pon_oper_state_stop},
        /* -------------------------------------- */
        {"no_sync", rdpa_pon_o1_no_sync_sub_state},
        {"profile_learn", rdpa_pon_o1_profile_learn_sub_state},
        /* -------------------------------------- */
        {"tuned", rdpa_pon_o5_accociated_sub_state},
        {"pending", rdpa_pon_o5_pending_sub_state},
        /* -------------------------------------- */
        {"no_sync", rdpa_pon_o8_no_sync_sub_state},
        {"profile_learn", rdpa_pon_o8_profile_learn_sub_state},
        {NULL, 0}
    }
};

static bdmf_attr_enum_table_t pon_link_operational_state_enum_table = {
    .type_name = "rdpa_pon_link_operational_state", .help = "PON link operational state",
    .values = {
        {"init o1", rdpa_pon_init_o1},
        {"standby o2", rdpa_pon_standby_o2},
        {"serial num o3", rdpa_pon_serial_num_o3},
        {"ranging o4", rdpa_pon_ranging_o4},
        {"operational o5", rdpa_pon_operational_o5},
        {"popup o6", rdpa_pon_popup_o6},
        {"emergency stop o7", rdpa_pon_emergency_stop_o7},
        {"ds tuning o8", rdpa_pon_ds_tuning_o8},
        {"us tuning o9", rdpa_pon_us_tuning_o9},
        {"serial num o2_3", rdpa_pon_serial_num_o2_3},
        {NULL, 0}
    }
};

static bdmf_attr_enum_table_t rdpa_polarity_enum_table = {
    .type_name = "rdpa_polarity",
    .values = {
        {"active_high", rdpa_polarity_active_high},
        {"active_low", rdpa_polarity_active_low},
        {NULL, 0}
    }
};

static bdmf_attr_enum_table_t ploam_type_enum_table = {
    .type_name = "ploam_type",
    .values = {
        {"dgasp", gpon_ploam_type_dgasp},
        {"pee", gpon_ploam_type_pee},
        {"pst", gpon_ploam_type_pst},
        {NULL, 0}
    }
};

static bdmf_attr_enum_table_t dba_interval_enum_table = {
    .type_name = "rdpa_dba_interval_t",
    .values = {
        {"disable", gpon_dba_disable},
        {"1msec", gpon_dba_interval_1_msec},
        {"2msec", gpon_dba_interval_2_msec},
        {"3msec", gpon_dba_interval_3_msec},
        {"4msec", gpon_dba_interval_4_msec},
        {"5msec", gpon_dba_interval_5_msec},
        {"6msec", gpon_dba_interval_6_msec},
        {"7msec", gpon_dba_interval_7_msec},
        {"8msec", gpon_dba_interval_8_msec},
        {"9msec", gpon_dba_interval_9_msec},
        {"10msec", gpon_dba_interval_10_msec},
        {"125usec", gpon_dba_interval_125_usec},
        {"250usec", gpon_dba_interval_250_usec},
        {"500usec", gpon_dba_interval_500_usec},
        {NULL, 0}
    }
};

static bdmf_attr_enum_table_t rogue_mode_enum_table = {
    .type_name = "rogue_detection_mode",
    .values = {
        {"tx_monitor", rdpa_rogue_tx_monitor_mode},
        {"tx_fault", rdpa_rogue_tx_fault_mode},
        {NULL, 0}
    }
};

static bdmf_attr_enum_table_t link_activate_enum_table = {
    .type_name = "rdpa_link_activate_t",
    .values = {
        {"deactivate",  rdpa_link_deactivate},
        {"activate_O1", rdpa_link_activate_O1},
        {"activate_O7", rdpa_link_activate_O7},
        {NULL, 0}
    }
};

const bdmf_attr_enum_table_t rdpa_gpon_disc_prio_enum_table =
{
    .type_name = "rdpa_discard_prty", .help = "Discard priority",
    .values = {
        {"low", rdpa_discard_prty_low},
        {"high", rdpa_discard_prty_high},
        {NULL, 0}
    }
};

const bdmf_attr_enum_table_t rdpa_gpon_enc_ring_enum_table =
{
    .type_name = "rdpa_enc_ring", .help = "Encryption ring",
    .values = {
        {"no_enc", rdpa_no_enc},
        {"ucast",  rdpa_unicast_both_dir_enc},
        {"bcast",  rdpa_broadcast_enc},
        {"ucast_ds",  rdpa_unicast_ds_enc},
        {NULL, 0}
    }
};

const bdmf_attr_enum_table_t rdpa_gpon_flow_type_enum_table =
{
    .type_name = "rdpa_gpon_flow_type", .help = "Flow Type",
    .values = {
        {"omci", rdpa_gpon_flow_type_omci},
        {"eth",  rdpa_gpon_flow_type_ethernet},
        {NULL, 0}
    }
};

static bdmf_attr_enum_table_t rdpa_stack_mode_enum_table = {
    .type_name = "rdpa_gpon_stack_mode_t",
    .values = {
        {"xgpon",  rdpa_stack_mode_xgpon},
        {"ngpon2_10g", rdpa_stack_mode_ngpon2_10g},
        {"ngpon2_2_5g", rdpa_stack_mode_ngpon2_2_5g},
        {"xgs", rdpa_stack_mode_xgs},
        {"gpon", rdpa_stack_mode_gpon},
        {NULL, 0}
    }
};


/*
 * aggregate descriptors
 */

/*  rdpa_gpon_link_state_t aggregate type */
static struct bdmf_aggr_type gpon_link_state_type = {
    .name = "gpon_link_state", .struct_name = "rdpa_pon_link_state",
    .help = "GPON link state",
    .fields = (struct bdmf_attr[]) {
        { .name = "admin_state", .help = "Administrative state",
            .type = bdmf_attr_enum, .ts.enum_table = &pon_admin_state_enum_table,
            .offset = offsetof(rdpa_pon_link_state, admin_state), .size = sizeof(rdpa_pon_admin_state),
        },
        { .name = "sub_state", .help = "Sub state",
            .type = bdmf_attr_enum, .ts.enum_table = &pon_sub_state_enum_table,
            .offset = offsetof(rdpa_pon_link_state, sub_state), .size = sizeof(rdpa_pon_sub_state),
        },
        { .name = "link_operational_state", .help = "Link operational state (o1 - o7)",
            .type = bdmf_attr_enum, .ts.enum_table = &pon_link_operational_state_enum_table,
            .offset = offsetof(rdpa_pon_link_state, link_oper_state),
            .size = sizeof(rdpa_pon_link_operational_state),
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE_EXT(gpon_link_state_type);

/*  gpon_link_cfg aggregate type */
static struct bdmf_aggr_type gpon_link_cfg_type = {
    .name = "gpon_link_cfg", .struct_name = "rdpa_gpon_link_cfg_t",
    .help = "GPON Link Configuration",
    .fields = (struct bdmf_attr[]) {
        { .name = "to1_timeout", .help = "T01 timeout", .size = sizeof(uint32_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_gpon_link_cfg_t, to1_timeout)
        },
        { .name = "to2_timeout", .help = "T02 timeout", .size = sizeof(uint32_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_gpon_link_cfg_t, to2_timeout)
        },
        { .name = "ber_interval", .help = "BER interval", .size = sizeof(uint32_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_gpon_link_cfg_t, ber_interval)
        },
        { .name = "min_response_time", .help = "Min response time", .size = sizeof(uint32_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_gpon_link_cfg_t, min_response_time)
        },
        { .name = "tx_data_polarity", .help = "Data polarity", .size = sizeof(rdpa_polarity),
            .offset = offsetof(rdpa_gpon_link_cfg_t, tx_data_polarity),
            .type = bdmf_attr_enum, .ts.enum_table = &rdpa_polarity_enum_table
        },
        { .name = "dv_setup_pattern", .help = "Transceiver DV setup pattern", .size = sizeof(uint32_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_gpon_link_cfg_t, transceiver_dv_setup_pattern),
            .flags = BDMF_ATTR_HEX_FORMAT
        },
        { .name = "dv_hold_pattern", .help = "Transceiver DV hold pattern", .size = sizeof(uint32_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_gpon_link_cfg_t, transceiver_dv_hold_pattern),
            .flags = BDMF_ATTR_HEX_FORMAT
        },
        { .name = "dv_polarity", .help = "Transceiver DV polarity", .size = sizeof(rdpa_polarity),
            .offset = offsetof(rdpa_gpon_link_cfg_t, transceiver_dv_polarity),
            .type = bdmf_attr_enum, .ts.enum_table = &rdpa_polarity_enum_table
        },
        { .name = "power_calibration_mode", .help = "Transceiver power calibration mode",
            .size = sizeof(bdmf_boolean),
            .type = bdmf_attr_boolean, .offset = offsetof(rdpa_gpon_link_cfg_t, transceiver_power_calibration_mode)
        },
        { .name = "power_calibration_pattern", .help = "Transceiver power calibration pattern",
            .size = sizeof(uint32_t), .type = bdmf_attr_number,
            .offset = offsetof(rdpa_gpon_link_cfg_t, transceiver_power_calibration_pattern),
            .flags = BDMF_ATTR_HEX_FORMAT
        },
        { .name = "power_calibration_size", .help = "Transceiver power calibration size",
            .size = sizeof(uint32_t), .type = bdmf_attr_number,
            .offset = offsetof(rdpa_gpon_link_cfg_t, transceiver_power_calibration_size)
        },
        { .name = "rx_din_polarity", .help = "RX Data Polarity", .size = sizeof(rdpa_polarity),
            .offset = offsetof(rdpa_gpon_link_cfg_t, rx_din_polarity),
            .type = bdmf_attr_enum, .ts.enum_table = &rdpa_polarity_enum_table
        },
        { .name = "ber_threshold_for_sd_assertion", .help = "BER threshold for SD assertion",
            .size = sizeof(uint32_t), .type = bdmf_attr_number,
            .offset = offsetof(rdpa_gpon_link_cfg_t, ber_threshold_for_sd_assertion)
        },
        { .name = "ber_threshold_for_sf_assertion", .help = "BER threshold for SF assertion",
            .size = sizeof(uint32_t), .type = bdmf_attr_number,
            .offset = offsetof(rdpa_gpon_link_cfg_t, ber_threshold_for_sf_assertion)
        },
        { .name = "number_of_psyncs_for_lof_assertion", .help = "Number of PSYNCs for LOF assertion",
            .size = sizeof(uint32_t), .type = bdmf_attr_number,
            .offset = offsetof(rdpa_gpon_link_cfg_t, number_of_psyncs_for_lof_assertion)
        },
        { .name = "number_of_psyncs_for_lof_clear", .help = "Number of PSYNCs for LOF clear",
            .size = sizeof(uint32_t), .type = bdmf_attr_number,
            .offset = offsetof(rdpa_gpon_link_cfg_t, number_of_psyncs_for_lof_clear)
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE_EXT(gpon_link_cfg_type);

/*  onu_sn aggregate type */
static struct bdmf_aggr_type onu_sn_type = {
    .name = "onu_sn", .struct_name = "rdpa_onu_sn_t",
    .help = "ONU Serial Number",
    .fields = (struct bdmf_attr[]) {
        { .name = "vendor_id", .help = "Vendor id", .size = sizeof(uint32_t),
            .type = bdmf_attr_buffer, .offset = offsetof(rdpa_onu_sn_t, vendor_id),
        },
        { .name = "vendor_specific", .help = "Vendor-specific id", .size = sizeof(uint32_t),
            .type = bdmf_attr_buffer, .offset = offsetof(rdpa_onu_sn_t, vendor_specific),
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE_EXT(onu_sn_type);

/*  gpon_link_stat aggregate type */
static struct bdmf_aggr_type gpon_link_stat_type = {
    .name = "gpon_link_stat", .struct_name = "rdpa_gpon_stat_t",
    .help = "GPON Link Statistics", .extra_flags = BDMF_ATTR_UNSIGNED,
    .fields = (struct bdmf_attr[]) {
        { .name = "bip_errors", .help = "BIP error counter", .size = sizeof(uint32_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_gpon_stat_t, bip_errors),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "crc_errors", .help = "PLOAM CRC error counter", .size = sizeof(uint32_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_gpon_stat_t, crc_errors),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "rx_ploam_onu_id", .help = "Valid ONU id PLOAM counter", .size = sizeof(uint32_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_gpon_stat_t, rx_onu_id),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "rx_broadcast_ploam", .help = "Broadcast PLOAM counter", .size = sizeof(uint32_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_gpon_stat_t, rx_broadcast),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "rx_unknown", .help = "Rx unknown PLOAM counter", .size = sizeof(uint32_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_gpon_stat_t, rx_unknown),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "tx_illegal_access", .help = "Tx illegal access counter", .size = sizeof(uint16_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_gpon_stat_t, tx_illegal_access),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "tx_idle_ploam", .help = "Tx idle PLOAM counter", .size = sizeof(uint32_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_gpon_stat_t, tx_idle),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "tx_ploam", .help = "Tx PLOAM counter", .size = sizeof(uint32_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_gpon_stat_t, tx_ploam),
            .flags = BDMF_ATTR_UNSIGNED
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE_EXT(gpon_link_stat_type);

static struct bdmf_aggr_type burst_prof =
{
    .name = "burst_prof", .struct_name = "rdpa_gpon_burst_prof_t",
    .help = "Burst profile",
    .fields = (struct bdmf_attr[])
    {
        { .name = "profile_enable", .help = "Enabled",
            .type = bdmf_attr_boolean,
            .offset = offsetof(rdpa_gpon_burst_prof_t, profile_enable)
        },
        { .name = "profile_index", .help = "Index",
            .type = bdmf_attr_number, .size = sizeof(uint8_t),
            .offset = offsetof(rdpa_gpon_burst_prof_t, profile_index)
        },
        { .name = "profile_version", .help = "Version",
            .type = bdmf_attr_number, .size = sizeof(uint8_t),
            .offset = offsetof(rdpa_gpon_burst_prof_t, profile_version)
        },
        { .name = "fec_ind", .help = "FEC indication",
            .type = bdmf_attr_number, .size = sizeof(uint8_t),
            .offset = offsetof(rdpa_gpon_burst_prof_t, fec_ind)
        },
        { .name = "delimiter_len", .help = "Delimiter length",
            .type = bdmf_attr_number, .size = sizeof(uint8_t),
            .offset = offsetof(rdpa_gpon_burst_prof_t, delimiter_len)
        },
        { .name = "fec_type", .help = "FEC type (NGPON2)",
            .type = bdmf_attr_number, .size = sizeof(uint8_t),
            .offset = offsetof(rdpa_gpon_burst_prof_t, fec_type)
        },
        { .name = "delimiter", .help = "Delimiter",
            .type = bdmf_attr_number, .size = sizeof(uint64_t),
            .offset = offsetof(rdpa_gpon_burst_prof_t, delimiter),
            .flags = BDMF_ATTR_HEX_FORMAT
        },
        { .name = "preamble_len", .help = "Preamble length",
            .type = bdmf_attr_number, .size = sizeof(uint8_t),
            .offset = offsetof(rdpa_gpon_burst_prof_t, preamble_len)
        },
        { .name = "preamble_repeat_counter", .help = "Preamble count",
            .type = bdmf_attr_number, .size = sizeof(uint8_t),
            .offset = offsetof(rdpa_gpon_burst_prof_t, preamble_repeat_counter),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "preamble", .help = "Preamble",
            .type = bdmf_attr_number, .size = sizeof(uint64_t),
            .offset = offsetof(rdpa_gpon_burst_prof_t, preamble),
            .flags = BDMF_ATTR_HEX_FORMAT
        },
        { .name = "pon_tag", .help = "PON TAG",
            .type = bdmf_attr_number, .size = sizeof(uint64_t),
            .offset = offsetof(rdpa_gpon_burst_prof_t, pon_tag),
            .flags = BDMF_ATTR_HEX_FORMAT
        },
        { .name = "specific_line_rate", .help = "Line rate 2.5G/10G",
            .type = bdmf_attr_number, .size = sizeof(uint8_t),
            .offset = offsetof(rdpa_gpon_burst_prof_t, specific_line_rate)
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE_EXT(burst_prof);

/*  rdpa_gpon_overhead_cfg_t aggregate type */
static struct bdmf_aggr_type gpon_overhead_cfg_type = {
    .name = "gpon_overhead_cfg", .struct_name = "rdpa_gpon_overhead_cfg_t",
    .help = "GPON Overhead Configuration",
    .fields = (struct bdmf_attr[]) {
        { .name = "overhead", .help = "GPON Overhead", .size = 16,
            .type = bdmf_attr_buffer, .offset = offsetof(rdpa_gpon_overhead_cfg_t, overhead)
        },
        { .name = "overhead_len", .help = "GPON Overhead length", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_gpon_overhead_cfg_t, overhead_len)
        },
        { .name = "overhead_repetition", .help = "GPON Overhead repetition", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_gpon_overhead_cfg_t, overhead_repetition)
        },
        { .name = "overhead_repetition_len", .help = "GPON Overhead repetition length", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_gpon_overhead_cfg_t, overhead_repetition_len)
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE_EXT(gpon_overhead_cfg_type);

/*  FEC stat aggregate type */
static struct bdmf_aggr_type fec_stat_type =
{
    .name = "fec_stat", .struct_name = "rdpa_fec_stat_t",
    .help = "FEC Statistics", .extra_flags = BDMF_ATTR_UNSIGNED,
    .fields = (struct bdmf_attr[]) {
        { .name = "corrected_bytes", .help = "FEC corrected bytes", .size = sizeof(uint32_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_fec_stat_t, corrected_bytes),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "corrected_codewords", .help = "FEC corrected codewords", .size = sizeof(uint32_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_fec_stat_t, corrected_codewords),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "uncorrectable_codewords", .help = "FEC uncorrectable codewords", .size = sizeof(uint32_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_fec_stat_t, uncorrectable_codewords),
            .flags = BDMF_ATTR_UNSIGNED
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE_EXT(fec_stat_type);

/*  PST parameters aggregate type */
static struct bdmf_aggr_type pst_params_type =
{
    .name = "pst_params", .struct_name = "pst_params_t",
    .help = "PST parameters",
    .fields = (struct bdmf_attr[]) {
        { .name = "k1_value", .help = "K1", .size = sizeof(uint32_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_pst_params_t, k1_value)
        },
        { .name = "k2_value", .help = "K2", .size = sizeof(uint32_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_pst_params_t, k2_value)
        },
        { .name = "line_number", .help = "Line number", .size = sizeof(uint32_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_pst_params_t, line_number)
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE_EXT(pst_params_type);

/*  Send PLOAM aggregate type */
static struct bdmf_aggr_type send_ploam_type =
{
    .name = "send_ploam", .struct_name = "rdpa_send_ploam_params_t",
    .help = "Send PLOAM",
    .fields = (struct bdmf_attr[]) {
        { .name = "ploam_type", .help = "PLOAM type", .size = sizeof(rdpa_ploam_type_t),
            .type = bdmf_attr_enum,  .ts.enum_table = &ploam_type_enum_table,
            .offset = offsetof(rdpa_send_ploam_params_t, ploam_type)
        },
        { .name = "pst_params", .help = "PST Params", .size = sizeof(rdpa_pst_params_t),
            .type = bdmf_attr_aggregate, .ts.aggr_type_name = "pst_params",
            .offset = offsetof(rdpa_send_ploam_params_t, pst_params)
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE_EXT(send_ploam_type);

/*  ROGUE ONU aggregate type */
static struct bdmf_aggr_type rogue_type =
{
    .name = "rogue_onu", .struct_name = "rdpa_rogue_onu_t",
    .help = "rogue_onu",
    .fields = (struct bdmf_attr[]) {
        { .name = "enable", .help = "Rogue ONU State",
            .type = bdmf_attr_boolean, .offset = offsetof(rdpa_rogue_onu_t, enable)
        },
        { .name = "mode", .help = "Rogue ONU Detection mode", .size = sizeof(rdpa_rogue_mode_t),
            .type = bdmf_attr_enum, .ts.enum_table = &rogue_mode_enum_table,
            .offset = offsetof(rdpa_rogue_onu_t, mode)
        },
        { .name = "clock_cycle", .help = "Rogue ONU window_size in clock cycles", .size = sizeof(uint32_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_rogue_onu_t, clock_cycle)
        },
        { .name = "gpio_pin", .help = "Rogue ONU the wanted GPIO for this feature", .size = sizeof(uint32_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_rogue_onu_t, gpio_pin)
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE_EXT(rogue_type);

/*  MISC transmit aggregate type */
static struct bdmf_aggr_type misc_tx_type =
{
    .name = "misc_tx", .struct_name = "rdpa_misc_tx_t",
    .help = "misc transmit",
    .fields = (struct bdmf_attr[]) {
        { .name = "enable", .help = "transmit state", .size = sizeof(bdmf_boolean),
          .type = bdmf_attr_boolean, .offset = offsetof(rdpa_misc_tx_t, enable),
        },
        { .name = "overhead", .help = "Overhead", .size = 16*sizeof(char),
          .type = bdmf_attr_buffer, .offset = offsetof(rdpa_misc_tx_t, prodc),
        },
        { .name = "overhead_len", .help = "Overhead length", .size = sizeof(uint8_t),
          .type = bdmf_attr_number, .offset = offsetof(rdpa_misc_tx_t, prcl),
          .flags = BDMF_ATTR_HEX_FORMAT
        },
        { .name = "overhead_repetition", .help = "Overhead repetition content", .size = sizeof(uint8_t),
          .type = bdmf_attr_number, .offset = offsetof(rdpa_misc_tx_t, brc),
          .flags = BDMF_ATTR_HEX_FORMAT
        },
        { .name = "overhead_repetition_len", .help = "Overahead repetition length", .size = sizeof(uint8_t),
          .type = bdmf_attr_number, .offset = offsetof(rdpa_misc_tx_t, prl),
          .flags = BDMF_ATTR_HEX_FORMAT
        },
        { .name = "msstart", .help = "Misc start", .size = sizeof(uint16_t),
          .type = bdmf_attr_number, .offset = offsetof(rdpa_misc_tx_t, msstart),
          .flags = BDMF_ATTR_HEX_FORMAT
        },
        { .name = "msstop", .help = "Misc stop", .size = sizeof(uint16_t),
          .type = bdmf_attr_number, .offset = offsetof(rdpa_misc_tx_t, msstop),
          .flags = BDMF_ATTR_HEX_FORMAT
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE_EXT(misc_tx_type);

/*  gem_ds_cfg aggregate type */
static struct bdmf_aggr_type gem_ds_cfg_type = {
    .name = "gem_ds_cfg", .struct_name = "rdpa_gpon_gem_ds_cfg_t",
    .help = "GEM DS Configuration",
    .fields = (struct bdmf_attr[]) {
        { .name = "port", .help = "Flow destination",
          .size = sizeof(uint16_t), .type = bdmf_attr_number,
          .offset = offsetof(rdpa_gpon_gem_ds_cfg_t, port)
        },
        { .name = "discard_prty", .help = "Discard priority",
          .size = sizeof(rdpa_discard_prty), .type = bdmf_attr_enum,
          .ts.enum_table = &rdpa_gpon_disc_prio_enum_table,
          .offset = offsetof(rdpa_gpon_gem_ds_cfg_t, discard_prty)
        },
        { .name = "encryption", .help = "Encryption",
          .type = bdmf_attr_boolean,
          .offset = offsetof(rdpa_gpon_gem_ds_cfg_t, encryption)
        },
        { .name = "crc", .help = "CRC",
          .type = bdmf_attr_boolean,
          .offset = offsetof(rdpa_gpon_gem_ds_cfg_t, crc)
        },
        { .name = "enc-ring", .help = "Encryption_ring",
          .size = sizeof(rdpa_enc_ring_t), .type = bdmf_attr_enum,
          .ts.enum_table = &rdpa_gpon_enc_ring_enum_table,
          .offset = offsetof(rdpa_gpon_gem_ds_cfg_t, enc_ring)
        },
        { .name = "flow_type", .help = "Flow type",
          .size = sizeof(rdpa_gpon_flow_type), .type = bdmf_attr_enum,
          .ts.enum_table = &rdpa_gpon_flow_type_enum_table,
          .offset = offsetof(rdpa_gpon_gem_ds_cfg_t, flow_type)
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE_EXT(gem_ds_cfg_type);

/*  tcont_stat aggregate type */
static struct bdmf_aggr_type tcont_stat_type = {
    .name = "tcont_stat", .struct_name = "rdpa_tcont_stat_t",
    .help = "TCONT Statistics", .extra_flags = BDMF_ATTR_UNSIGNED,
    .fields = (struct bdmf_attr[]) {
        { .name = "idle_gems", .help = "Idle GEM counter", .size = sizeof(uint32_t),
          .type = bdmf_attr_number, .offset = offsetof(rdpa_tcont_stat_t, idle_gem_counter)
        },
        { .name = "non_idle_gems", .help = "Non idle GEM counter", .size = sizeof(uint32_t),
          .type = bdmf_attr_number, .offset = offsetof(rdpa_tcont_stat_t, non_idle_gem_counter)
        },
        { .name = "packets", .help = "Packet counter", .size = sizeof(uint32_t),
          .type = bdmf_attr_number, .offset = offsetof(rdpa_tcont_stat_t, packet_counter)
        },
        { .name = "valid_accesses", .help = "Valid access counter", .size = sizeof(uint32_t),
          .type = bdmf_attr_number, .offset = offsetof(rdpa_tcont_stat_t, valid_access_counter)
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE_EXT(tcont_stat_type);

/*  pon_id_param aggregate type */
static struct bdmf_aggr_type pon_id_param = {
    .name = "pon_id_param", .struct_name = "rdpa_gpon_pon_id_param_t",
    .help = "Three elements that were read from latest PON ID PLOAM message",
    .fields = (struct bdmf_attr[]) {
        { .name = "pon_id_type", .help = "PON-ID type", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_gpon_pon_id_param_t, pon_id_type),
            .flags = BDMF_ATTR_HEX_FORMAT
        },
        { .name = "pon_identifier", .help = "PON-Identifier", .size = 7*sizeof(uint8_t),
            .type = bdmf_attr_buffer, .offset = offsetof(rdpa_gpon_pon_id_param_t, pon_identifier),
        },
        { .name = "tol", .help = "TOL - Transmit Optical Level", .size = 2*sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_gpon_pon_id_param_t, tx_optical_level),
            .flags = BDMF_ATTR_HEX_FORMAT
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE_EXT(pon_id_param);


static struct bdmf_aggr_type mcast_enc_key_param = {
    .name = "mcast_enc_key_param", .struct_name = "rdpa_gpon_mcast_enc_key_param_t",
    .help = "Multicast encryption key generated by OLT",
    .fields = (struct bdmf_attr[]) {
        { .name = "key1", .help = "first multicast encryption key", .size = 16*sizeof(uint8_t),
            .type = bdmf_attr_buffer, .offset = offsetof(rdpa_gpon_mcast_enc_key_param_t, key_1),
        },
        { .name = "key2", .help = "second multicast encryption key", .size = 16*sizeof(uint8_t),
            .type = bdmf_attr_buffer, .offset = offsetof(rdpa_gpon_mcast_enc_key_param_t, key_2),
            .flags = BDMF_ATTR_HEX_FORMAT
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE_EXT(mcast_enc_key_param);

/*  bw_record_cfg aggregate type */
static struct bdmf_aggr_type bw_record_cfg =
{
    .name = "bw_record_cfg", .struct_name = "rdpa_gpon_bw_record_cfg_t",
    .help = "BW Record Setting",
    .fields = (struct bdmf_attr[])
    {
        { .name = "rcd_stop", .help = "RCD stop",
            .type = bdmf_attr_boolean,
            .offset = offsetof(rdpa_gpon_bw_record_cfg_t, rcd_stop)
        },
        { .name = "record_type", .help = "Record Type",
            .size = sizeof(uint32_t), .type = bdmf_attr_number,
            .offset = offsetof(rdpa_gpon_bw_record_cfg_t, record_type)
        },
        { .name = "alloc_id", .help = "Alloc ID",
            .size = sizeof(uint32_t), .type = bdmf_attr_number,
            .offset = offsetof(rdpa_gpon_bw_record_cfg_t, alloc_id)
        },
        { .name = "enable", .help = "Enable BW recording",
            .type = bdmf_attr_boolean,
            .offset = offsetof(rdpa_gpon_bw_record_cfg_t, enable)
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE_EXT(bw_record_cfg);

/*  bw_record_result aggregate type */
static struct bdmf_aggr_type bw_record_result_gpon =
{
    .name = "bw_record_result_gpon", .struct_name = "rdpa_gpon_bw_record_result_gpon_t",
    .help = "BW Record Result",
    .fields = (struct bdmf_attr[])
    {
        { .name = "alloc_id", .help = "Alloc ID",
            .size = sizeof(uint32_t), .type = bdmf_attr_number,
            .offset = offsetof(rdpa_gpon_bw_record_result_gpon_t, alloc_id)
        },
        { .name = "flag_pls", .help = "PLS",
            .type = bdmf_attr_boolean,
            .offset = offsetof(rdpa_gpon_bw_record_result_gpon_t, flag_pls)
        },
        { .name = "flag_ploam", .help = "flag_ploam",
            .type = bdmf_attr_boolean,
            .offset = offsetof(rdpa_gpon_bw_record_result_gpon_t, flag_ploam)
        },
        { .name = "flag_fec", .help = "FEC",
            .type = bdmf_attr_boolean,
            .offset = offsetof(rdpa_gpon_bw_record_result_gpon_t, flag_fec)
        },
        { .name = "flag_dbru", .help = "dbru",
            .size = sizeof(uint32_t), .type = bdmf_attr_number,
            .offset = offsetof(rdpa_gpon_bw_record_result_gpon_t, flag_dbru)
        },
        { .name = "crc_valid", .help = "CRC",
            .type = bdmf_attr_boolean,
            .offset = offsetof(rdpa_gpon_bw_record_result_gpon_t, crc_valid)
        },
        { .name = "sstart", .help = "Start",
            .size = sizeof(uint32_t), .type = bdmf_attr_number,
            .offset = offsetof(rdpa_gpon_bw_record_result_gpon_t, sstart)
        },
        { .name = "sstop", .help = "Stop",
            .size = sizeof(uint32_t), .type = bdmf_attr_number,
            .offset = offsetof(rdpa_gpon_bw_record_result_gpon_t, sstop)
        },
        { .name = "sf_counter", .help = "SF counter",
            .size = sizeof(uint32_t), .type = bdmf_attr_number,
            .offset = offsetof(rdpa_gpon_bw_record_result_gpon_t, sf_counter)
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE_EXT(bw_record_result_gpon);

/*  bw_record_result_ngpon aggregate type */
static struct bdmf_aggr_type bw_record_result_ngpon =
{
    .name = "bw_record_result_ngpon", .struct_name = "rdpa_gpon_bw_record_result_ngpon_t",
    .help = "BW Record Result",
    .fields = (struct bdmf_attr[])
    {
        { .name = "starttime", .help = "Start Time",
            .size = sizeof(uint16_t), .type = bdmf_attr_number,
            .offset = offsetof(rdpa_gpon_bw_record_result_ngpon_t, starttime)
        },
        { .name = "allocid", .help = "Alloc ID",
            .size = sizeof(uint16_t), .type = bdmf_attr_number,
            .offset = offsetof(rdpa_gpon_bw_record_result_ngpon_t, allocid)
        },
        { .name = "sfc_ls", .help = "SFC LS",
            .size = sizeof(uint16_t), .type = bdmf_attr_number,
            .offset = offsetof(rdpa_gpon_bw_record_result_ngpon_t, sfc_ls)
        },
        { .name = "hec_ok", .help = "HEC OK",
            .type = bdmf_attr_boolean,
            .offset = offsetof(rdpa_gpon_bw_record_result_ngpon_t, hec_ok)
        },
        { .name = "bprofile", .help = "B-Profile",
            .size = sizeof(uint8_t), .type = bdmf_attr_number,
            .offset = offsetof(rdpa_gpon_bw_record_result_ngpon_t, bprofile)
        },
        { .name = "fwi", .help = "FWI",
            .type = bdmf_attr_boolean,
            .offset = offsetof(rdpa_gpon_bw_record_result_ngpon_t, fwi)
        },
        { .name = "ploamu", .help = "PloamU",
            .type = bdmf_attr_boolean,
            .offset = offsetof(rdpa_gpon_bw_record_result_ngpon_t, ploamu)
        },
        { .name = "dbru", .help = "dbru",
            .type = bdmf_attr_boolean,
            .offset = offsetof(rdpa_gpon_bw_record_result_ngpon_t, dbru)
        },
        { .name = "grantsize", .help = "Grant Size",
            .size = sizeof(uint16_t), .type = bdmf_attr_number,
            .offset = offsetof(rdpa_gpon_bw_record_result_ngpon_t, grantsize)
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE_EXT(bw_record_result_ngpon);


/** This optional callback is called called at object init time
 *  before initial attributes are set.
 *  If function returns error code !=0, object creation is
 *  aborted */
static int gpon_pre_init(struct bdmf_object *mo)
{
    gpon_drv_priv_t *priv = (gpon_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_gpon_link_cfg_t *link_cfg = &(priv->link_cfg);

#ifndef BDMF_SYSTEM_SIM
    uint16_t OpticsTypeFlag = 0 ;
#endif

    link_cfg->to1_timeout = RDPA_TO1_TIMEOUT;
    link_cfg->to2_timeout = RDPA_TO2_TIMEOUT;
    link_cfg->ber_interval = RDPA_BER_INTERVAL;
    link_cfg->min_response_time = RDPA_MIN_RESPONSE_TIME;
    link_cfg->tx_data_polarity = RDPA_TX_DATA_POLARITY_MODE;
    link_cfg->transceiver_dv_setup_pattern = RDPA_DV_SETUP_PATTERN;
    link_cfg->transceiver_dv_hold_pattern = RDPA_DV_HOLD_PATTERN;
    link_cfg->transceiver_dv_polarity = RDPA_DV_POLARITY;
    link_cfg->transceiver_power_calibration_mode = RDPA_POWER_CALIBRATION_MODE;
    link_cfg->transceiver_power_calibration_pattern = RDPA_POWER_CALIBRATION_PATTERN;
    link_cfg->transceiver_power_calibration_size = RDPA_POWER_CALIBRATION_SIZE;
    link_cfg->rx_din_polarity = RDPA_RX_DIN_POLARITY_MODE;
    link_cfg->ber_threshold_for_sd_assertion = RDPA_SD_THRESHOLD;
    link_cfg->ber_threshold_for_sf_assertion = RDPA_SF_THRESHOLD;
#ifdef CONFIG_BCM96858
    if (stack_mode != GPON_MODE)
    {
        link_cfg->number_of_psyncs_for_lof_assertion = 2;
        link_cfg->number_of_psyncs_for_lof_clear = 3;
    }
    else
    {
        link_cfg->number_of_psyncs_for_lof_assertion = RDPA_RX_LOF_ASSERTION;
        link_cfg->number_of_psyncs_for_lof_clear = RDPA_RX_LOF_CLEAR;
    }
#else
    link_cfg->number_of_psyncs_for_lof_assertion = RDPA_RX_LOF_ASSERTION;
    link_cfg->number_of_psyncs_for_lof_clear = RDPA_RX_LOF_CLEAR;
#endif

#ifndef BDMF_SYSTEM_SIM
    BpGetGponOpticsType(&OpticsTypeFlag);
    if (OpticsTypeFlag == BP_GPON_OPTICS_TYPE_PMD)
    {
#ifdef CONFIG_BCM96858
        link_cfg->tx_data_polarity = 1;

        if (stack_mode != GPON_MODE)
        {
        	link_cfg->transceiver_dv_polarity = 0;
        }
#endif
        /*link_cfg->transceiver_dv_setup_pattern = RDPA_PMD_DV_SETUP_PATTERN;*/
        link_cfg->transceiver_dv_hold_pattern = RDPA_PMD_DV_HOLD_PATTERN;
        pmd_dev_assign_gpon_callback(_pmd_gpon_isr_wrapper);
    }
#endif

    memset(&priv->onu_password, 0, sizeof(rdpa_onu_password_t));
    memset(&priv->onu_sn, 0, sizeof(rdpa_onu_sn_t));
    priv->link_activate = rdpa_link_deactivate;
    priv->link_state.link_oper_state = rdpa_pon_init_o1;
    memset(&priv->encryption_key, 0, sizeof(uint8_t)*PLOAM_ENCRYPTION_KEY_SIZE_BYTES);
    priv->user_ic = default_ic;
    priv->tcont_counter_assigment = 0;
    priv->dba_interval = gpon_dba_interval_2_msec;
    priv->gem_block_size = RDPA_GPON_DEFAULT_GEM_BLOCK_SIZE;
    memset(&priv->rogue_onu, 0, sizeof(rdpa_rogue_onu_t));
    memset(&priv->misc_tx, 0, sizeof(rdpa_misc_tx_t));
    memset(&priv->mcast_enc_key,0,sizeof(rdpa_gpon_mcast_enc_key_param_t));
    priv->stack_mode = rdpa_stack_mode_gpon;
    memset(&priv->bw_record_cfg, 0, sizeof(rdpa_gpon_bw_record_cfg_t));
    priv->bw_record_cfg.rcd_stop = BDMF_TRUE;
    memset(&priv->burst_prof, 0, sizeof(priv->burst_prof));
    accesses_array.ngpon_access = bdmf_alloc(NGPON_NUMBER_OF_ACCESS_IN_MAP*sizeof(NGPON_ACCESS));
    accesses_array.gpon_access = bdmf_alloc(GPON_NUMBER_OF_ACCESS_IN_MAP*sizeof(GPON_ACCESS));

#ifdef USE_BDMF_SHELL
    register_gpon_shell_commands();
#endif

    return 0;
}

/** This optional callback is called at object init time
 * after initial attributes are set.
 * Its work is:
 * - make sure that all necessary attributes are set and make
 *   sense
 * - allocate dynamic resources if any
 * - assign object name if not done in pre_init
 * - finalise object creation
 * If function returns error code !=0, object creation is
 * aborted
 */
static int gpon_post_init(struct bdmf_object *mo)
{
    int rc = 0;
#ifndef BDMF_SYSTEM_SIM
    gpon_drv_priv_t *priv = (gpon_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_gpon_stack_mode_t  pon_mac_mode_2_rdpa_stack_mode[] = {
       rdpa_stack_mode_xgpon,       /* 0 - NGPON_MODE_XGPON */
       rdpa_stack_mode_gpon,        /* 1 - NGPON_MODE_ILLEGAL */
       rdpa_stack_mode_ngpon2_10g,  /* 2 - NGPON_MODE_NGPON2_10G */
       rdpa_stack_mode_ngpon2_2_5g, /* 3 - NGPON_MODE_NGPON2_2_5G */
       rdpa_stack_mode_xgs,         /* 4 - NGPON_MODE_XGS */
       rdpa_stack_mode_gpon,        /* 5 - GPON_MODE      */
    };
#endif

    gpon_object = mo;
    snprintf(mo->name, sizeof(mo->name), "gpon");

#ifndef BDMF_SYSTEM_SIM
    pon_sm_cb.pon_assign_user_callback((EVENT_AND_ALARM_CALLBACK_DTE)_rdpa_indication_callback,
        (DBR_PROCESS_RUNNER_DATA_CALLBACK_DTE) _rdpa_gpon_sr_dba_callback,
        (SC_SC_RUNNER_DATA_CALLBACK_DTE)_rdpa_sc_sc_callback, (FLUSH_TCONT_CALLBACK_DTE)_rdpa_pon_flush_tcont_fe_bbh);

    rdpa_gpon_irq_connect();

    rc = initialize_pon();
    if (rc < 0)
        return rc;

    rc = pon_sm_cb.pon_configure_onu_serial_number((*(PON_SERIAL_NUMBER_DTE *)(&priv->onu_sn)));
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "serial number configuration failed, error = %d\n", rc);

    rc = pon_sm_cb.pon_configure_onu_password((*(PON_PASSWORD_DTE *)(&priv->onu_password)));
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "password configuration failed, error = %d\n", rc);

    if (priv->link_activate)
    {
        rdpa_cpu_int_enable(rdpa_cpu_host, RDPA_PLOAM_CPU_RX_QUEUE_ID);
        rc = pon_sm_cb.pon_link_activate(priv->link_activate == rdpa_link_activate_O7 ? 1 : 0);
        if (rc)
        {
            rdpa_cpu_int_disable(rdpa_cpu_host, RDPA_PLOAM_CPU_RX_QUEUE_ID);
            BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "link activating failed, error = %d\n", rc);
        }
    }

    if (priv->gem_block_size != RDPA_GPON_DEFAULT_GEM_BLOCK_SIZE)
    {
        rc = pon_sm_cb.pon_set_gem_block_size(priv->gem_block_size);
        if (rc)
            BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "gpon block size configuration failed, error = %d\n", rc);
    }

    /* Enable SR-DBA */
    if (priv->dba_interval != gpon_dba_disable)
    {
        rc = pon_sm_cb.pon_dba_sr_process_initialize(priv->gem_block_size, (uint32_t)(priv->dba_interval));
        if (rc)
            BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Enable SR DBA failed, error = %d\n", rc);
    }
#endif
    priv->stack_mode = pon_mac_mode_2_rdpa_stack_mode[stack_mode];

    return rc;
}

static void gpon_destroy(struct bdmf_object *mo)
{
    if (gpon_object != mo)
        return;

#ifdef USE_BDMF_SHELL
    unregister_gpon_shell_commands();
#endif

    /* ToDo: do cleanups here */
    gpon_object = NULL;
    bdmf_free(accesses_array.ngpon_access);
    bdmf_free(accesses_array.gpon_access);
}

#ifndef BDMF_SYSTEM_SIM
/* Map rdpa tcont index to rdd wan channel; tcont index shifted by 1 on gpon so tcont 1-8 can use 32 RC's */
static int gpon_tcont_to_rdd_wan_channel(int rdpa_tcont_index)
{
    if (rdpa_tcont_index)
        return rdpa_tcont_index - 1;
    return GPON_MAX_TCONT - 1;
}
#endif

/*
 * Attribute access functions
 */

/* "link_activate" attribute "write" callback. */
static int gpon_attr_link_activate_write(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, const void *val, uint32_t size)
{
    gpon_drv_priv_t *priv = (gpon_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_link_activate_t *activate = (rdpa_link_activate_t *)val;
#ifndef BDMF_SYSTEM_SIM
    int rc = 0;
#endif

    if (priv->link_activate == *activate)
        return 0;

#ifndef BDMF_SYSTEM_SIM
    if (mo->state == bdmf_state_active)
    {
        if (*activate == rdpa_link_deactivate)
        {
            rc = pon_sm_cb.pon_link_deactivate();
            if (rc)
                BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "pon link deactivate failed: gpon_error = %d\n", rc);
                rdpa_cpu_int_disable(rdpa_cpu_host, RDPA_PLOAM_CPU_RX_QUEUE_ID);
            }
        else
        {
            rdpa_cpu_int_enable(rdpa_cpu_host, RDPA_PLOAM_CPU_RX_QUEUE_ID);
            rc = pon_sm_cb.pon_link_activate(*activate == rdpa_link_activate_O7 ? 1 : 0);
            if (rc)
            {
            rdpa_cpu_int_disable(rdpa_cpu_host, RDPA_PLOAM_CPU_RX_QUEUE_ID);
                BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "link activating failed: gpon error = %d\n", rc);
            }
        }
     }
#endif

    priv->link_activate = *activate;

    return 0;
}

#define GPON_GEN_SUB_STATES_NUM (rdpa_pon_o8_profile_learn_sub_state+1)   /* From bcmdrivers/opensource/char/rdpa_gpl_ext/impl1/include/rdpa_gpon.h, enum rdpa_pon_sub_state */

static rdpa_pon_link_operational_state xgpon_map_state_for_presentation(LINK_OPERATION_STATES_DTE state, LINK_SUB_STATE_DTE sub_state)
{

  /*
   * For XGPON the mapping is determined by the state and for O1 the sub state shall also be considered, therefore the table contains
   * entries for max num of sub-states (see rdpa_pon_sub_state enum in rdpa_gpon.h)
   */
  static rdpa_pon_link_operational_state state_sub_state_table[OPERATION_NGPON2_STATES_NUMBER][GPON_GEN_SUB_STATES_NUM] = {
    /* OPERATION_STATE_O1 */
    {rdpa_pon_init_o1, rdpa_pon_init_o1, rdpa_pon_init_o1, rdpa_pon_init_o1, rdpa_pon_init_o1, rdpa_pon_serial_num_o2_3, rdpa_pon_init_o1, rdpa_pon_init_o1, rdpa_pon_init_o1, rdpa_pon_init_o1},

    /* OPERATION_STATE_O2 */
    {rdpa_pon_serial_num_o2_3,rdpa_pon_serial_num_o2_3,rdpa_pon_serial_num_o2_3,rdpa_pon_serial_num_o2_3,rdpa_pon_serial_num_o2_3,rdpa_pon_serial_num_o2_3,rdpa_pon_serial_num_o2_3,rdpa_pon_serial_num_o2_3,rdpa_pon_serial_num_o2_3,rdpa_pon_serial_num_o2_3},

    /* OPERATION_STATE_O3 */
    {rdpa_pon_serial_num_o2_3, rdpa_pon_serial_num_o2_3, rdpa_pon_serial_num_o2_3, rdpa_pon_serial_num_o2_3, rdpa_pon_serial_num_o2_3, rdpa_pon_serial_num_o2_3, rdpa_pon_serial_num_o2_3, rdpa_pon_serial_num_o2_3, rdpa_pon_serial_num_o2_3, rdpa_pon_serial_num_o2_3},

    /* OPERATION_STATE_O4 */
    {rdpa_pon_ranging_o4, rdpa_pon_ranging_o4, rdpa_pon_ranging_o4, rdpa_pon_ranging_o4, rdpa_pon_ranging_o4, rdpa_pon_ranging_o4, rdpa_pon_ranging_o4, rdpa_pon_ranging_o4, rdpa_pon_ranging_o4},

    /* OPERATION_STATE_O5 */
    {rdpa_pon_operational_o5, rdpa_pon_operational_o5, rdpa_pon_operational_o5, rdpa_pon_operational_o5, rdpa_pon_operational_o5, rdpa_pon_operational_o5, rdpa_pon_operational_o5, rdpa_pon_operational_o5, rdpa_pon_operational_o5, rdpa_pon_operational_o5},

    /* OPERATION_STATE_O6 */
    {rdpa_pon_popup_o6, rdpa_pon_popup_o6, rdpa_pon_popup_o6, rdpa_pon_popup_o6, rdpa_pon_popup_o6, rdpa_pon_popup_o6, rdpa_pon_popup_o6, rdpa_pon_popup_o6, rdpa_pon_popup_o6, rdpa_pon_popup_o6},

    /* OPERATION_STATE_O7 */
    {rdpa_pon_emergency_stop_o7, rdpa_pon_emergency_stop_o7, rdpa_pon_emergency_stop_o7, rdpa_pon_emergency_stop_o7, rdpa_pon_emergency_stop_o7, rdpa_pon_emergency_stop_o7, rdpa_pon_emergency_stop_o7, rdpa_pon_emergency_stop_o7, rdpa_pon_emergency_stop_o7, rdpa_pon_emergency_stop_o7}

    /* States O8 and O9 shall not appear in XGPON mode - protected by Activation State Machine */
  } ;

  if (sub_state >  rdpa_pon_o8_profile_learn_sub_state)
  {
    sub_state = LINK_SUB_STATE_NONE;
  }

  return state_sub_state_table[state][sub_state];

}


static rdpa_pon_link_operational_state xgs_map_state_for_presentation(LINK_OPERATION_STATES_DTE state)
{
  static rdpa_pon_link_operational_state state_sub_state_table[OPERATION_NGPON2_STATES_NUMBER] = {
    rdpa_pon_init_o1,
    rdpa_pon_serial_num_o2_3,
    rdpa_pon_serial_num_o2_3,
    rdpa_pon_ranging_o4,
    rdpa_pon_operational_o5,
    rdpa_pon_popup_o6,
    rdpa_pon_emergency_stop_o7
  } ;

  return state_sub_state_table[state];

}

static rdpa_pon_link_operational_state ngpon2_map_state_for_presentation(LINK_OPERATION_STATES_DTE state)
{
  static rdpa_pon_link_operational_state state_sub_state_table[OPERATION_NGPON2_STATES_NUMBER] = {
    rdpa_pon_init_o1,
    rdpa_pon_serial_num_o2_3,
    rdpa_pon_serial_num_o2_3,
    rdpa_pon_ranging_o4,
    rdpa_pon_operational_o5,
    rdpa_pon_popup_o6,
    rdpa_pon_emergency_stop_o7,
    rdpa_pon_ds_tuning_o8,
    rdpa_pon_us_tuning_o9
  } ;

  return state_sub_state_table[state];

}



/* "link_operational_state" attribute "read" callback */
static int gpon_attr_link_state_read(struct bdmf_object *mo, struct bdmf_attr *ad,
    bdmf_index index, void *val, uint32_t size)
{
    int rc = 0;
    gpon_drv_priv_t *priv = (gpon_drv_priv_t *)bdmf_obj_data(mo);

#ifndef BDMF_SYSTEM_SIM
    rdpa_pon_link_state *link_state = (rdpa_pon_link_state *)val;
    LINK_SUB_STATE_DTE  sub_state;
    LINK_OPERATION_STATES_DTE state;

    rc = pon_sm_cb.pon_get_link_status((LINK_STATE_DTE *)&link_state->admin_state,
        (LINK_SUB_STATE_DTE *)&sub_state, (LINK_OPERATION_STATES_DTE *)&state);

    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "fail in getting link state! gpon_error = %d\n", rc);

    switch (priv->stack_mode)
    {
       case rdpa_stack_mode_xgpon:
         link_state->link_oper_state = xgpon_map_state_for_presentation (state, sub_state);
         break;

       case rdpa_stack_mode_ngpon2_10g:
       case rdpa_stack_mode_ngpon2_2_5g:
         link_state->link_oper_state = ngpon2_map_state_for_presentation (state);
         break;

      case rdpa_stack_mode_xgs:
         link_state->link_oper_state = xgs_map_state_for_presentation (state);
         break;

      case rdpa_stack_mode_gpon:
         link_state->link_oper_state = state;  /* no need in conversion */
         break;

      default:
         link_state->link_oper_state = state;
         break;
    }
    link_state->sub_state = sub_state ;
#endif

    return rc;
}

/* "link_cfg" attribute "write" callback. */
static int gpon_attr_link_cfg_write(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, const void *val, uint32_t size)
{
    gpon_drv_priv_t *priv = (gpon_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_gpon_link_cfg_t *cfg = (rdpa_gpon_link_cfg_t *)val;

#ifndef BDMF_SYSTEM_SIM
    if (mo->state == bdmf_state_active)
        pon_sm_cb.pon_configure_link_params(cfg);
#endif
    priv->link_cfg = *cfg;

    return 0;
}

/* "link_cfg" attribute "read" callback. */
static int gpon_attr_link_cfg_read(struct bdmf_object *mo, struct bdmf_attr *ad,
    bdmf_index index, void *val, uint32_t size)
{
    int rc = 0;
    rdpa_gpon_link_cfg_t *cfg = (rdpa_gpon_link_cfg_t *)val;

#ifndef BDMF_SYSTEM_SIM
    rc = pon_sm_cb.pon_get_link_parameters(cfg);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "can't get link parameters, error = %d\n", rc);
#endif
        return 0;
}

/* "onu_id" attribute "read" callback. */
static int gpon_attr_onu_id_read(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    void *val, uint32_t size)
{
    int rc = 0;
    uint16_t onu_id = 0;

#ifndef BDMF_SYSTEM_SIM
    rc = pon_sm_cb.pon_get_onu_id(&onu_id);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "can't get onu id, error = %d\n", rc);

#endif
    (*(bdmf_index *)val) = (bdmf_index)onu_id;

    return rc;
}

/* "overhead" attribute "read" callback. */
static int gpon_attr_overhead_read(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    void *val, uint32_t size)
{
    int rc = 0;
#ifndef BDMF_SYSTEM_SIM
    rdpa_gpon_overhead_cfg_t *overhead_cfg = (rdpa_gpon_overhead_cfg_t *)val;

    rc = pon_sm_cb.pon_get_overhead_and_length((PON_TX_OVERHEAD_DTE *)overhead_cfg->overhead,
        &overhead_cfg->overhead_len, &overhead_cfg->overhead_repetition_len, &overhead_cfg->overhead_repetition);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "can't get overhead info, error = %d\n", rc);
#endif

    return rc;
}

/* "transceiver_power_level" attribute "read" callback. */
static int gpon_attr_transceiver_power_level_read(struct bdmf_object *mo, struct bdmf_attr *ad,
    bdmf_index index, void *val, uint32_t size)
{
    int rc = 0;
#ifndef BDMF_SYSTEM_SIM
    uint8_t *transceiver_power_level = (uint8_t *)val;

    rc = pon_sm_cb.pon_get_transceiver_power_level(transceiver_power_level);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "can't get transceiver power level, error = %d\n", rc);
#endif

    return rc;
}

/* "equalization_delay" attribute "read" callback. */
static int gpon_attr_equalization_delay_read(struct bdmf_object *mo, struct bdmf_attr *ad,
    bdmf_index index, void *val, uint32_t size)
{
    int rc = 0;
#ifndef BDMF_SYSTEM_SIM
    uint32_t *equalization_delay = (uint32_t *)val;

    rc = pon_sm_cb.pon_get_equalization_delay(equalization_delay);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "can't get equalization delay, error = %d\n", rc);
#endif

    return rc;
}

/* "encryption_key" attribute "read" callback. */
static int gpon_attr_encryption_key_read(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    void *val, uint32_t size)
{
    int rc = 0;
    uint32_t encryption_key[4] = {};
#ifndef BDMF_SYSTEM_SIM
    int i;

    for (i = 0; i < PLOAM_ENCRYPTION_KEY_SIZE / sizeof(encryption_key[0]) ; i++)
    {
        rc = pon_sm_cb.pon_get_aes_encryption_key(&encryption_key[i], i);
        if (rc)
        {
            if (rc == PON_ERROR_INVALID_STATE)
                BDMF_TRACE_RET(BDMF_ERR_STATE, "Invalid state for aes encryption key reading\n");

            BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "can't get aes encryption key, error = %d\n", rc);
        }
    }

#endif
    memcpy((uint32_t *)val, encryption_key, sizeof(encryption_key));

    return rc;
}

/* "onu_sn" attribute "write" callback. */
static int gpon_attr_onu_sn_write(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, const void *val, uint32_t size)
{
    gpon_drv_priv_t *priv = (gpon_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_onu_sn_t *onu_sn = (rdpa_onu_sn_t *)val;
#ifndef BDMF_SYSTEM_SIM
    int rc = 0;
#endif

#ifndef BDMF_SYSTEM_SIM
    if (mo->state == bdmf_state_active)
    {
        if (priv->link_activate)
            BDMF_TRACE_RET(BDMF_ERR_STATE, "Can't change serial number because link is active\n");

        rc = pon_sm_cb.pon_configure_onu_serial_number((*(PON_SERIAL_NUMBER_DTE *)onu_sn));
        if (rc)
            BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "serial number configuration failed\n");
    }
#endif
    memcpy(&priv->onu_sn, onu_sn, sizeof(rdpa_onu_sn_t));

    return 0;
}

/* "onu_password" attribute "read" callback */
static int gpon_attr_onu_sn_read(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, void *val, uint32_t size)
{
    gpon_drv_priv_t *priv = (gpon_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_onu_sn_t *onu_sn = (rdpa_onu_sn_t *)val;

    memcpy(onu_sn, &priv->onu_sn, sizeof(rdpa_onu_sn_t));

    return 0;
}

/* "onu_sn" attribute "write" callback. */
static int gpon_attr_password_write(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, const void *val, uint32_t size)
{
    gpon_drv_priv_t *priv = (gpon_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_onu_password_t *password = (rdpa_onu_password_t *)val;
#ifndef BDMF_SYSTEM_SIM
    int rc = 0;
#endif

#ifndef BDMF_SYSTEM_SIM
    if (mo->state == bdmf_state_active)
    {
        if (priv->link_activate)
            BDMF_TRACE_RET(BDMF_ERR_STATE, "Can't change password because link is active\n");
        rc = pon_sm_cb.pon_configure_onu_password((*(PON_PASSWORD_DTE *)password));
        if (rc)
            BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "password configuration failed\n");
    }
#endif
    memcpy(&priv->onu_password, password, sizeof(rdpa_onu_password_t));

    return 0;
}

/* "user_ic" attribute "write" callback */
static int gpon_attr_user_ic_write(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, const void *val, uint32_t size)
{
    gpon_drv_priv_t *gpon_data = (gpon_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_indication_callback user_ic = *(rdpa_indication_callback *)val;

    gpon_data->user_ic = user_ic;

    /* Set default indication callback if the received callback is NULL */
    if (!user_ic)
        gpon_data->user_ic = default_ic;

    return 0;
}

/* "tcont_counter_assigment" attribute "write" callback */
static int gpon_attr_tcont_counter_assign_write(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, const void *val, uint32_t size)
{
    int rc = 0;
#ifndef BDMF_SYSTEM_SIM
    uint8_t tcont_group_0;
    uint8_t tcont_group_1;
    uint8_t tcont_group_2;
    uint8_t tcont_group_3;

    gpon_drv_priv_t *priv = (gpon_drv_priv_t *)bdmf_obj_data(mo);
    uint32_t *base_tcont_id = (uint32_t *)val;

    /* Assign counters */
    tcont_group_0 = gpon_tcont_to_rdd_wan_channel((uint8_t)(*base_tcont_id));
    tcont_group_1 = gpon_tcont_to_rdd_wan_channel((uint8_t)(*base_tcont_id + 1));
    tcont_group_2 = gpon_tcont_to_rdd_wan_channel((uint8_t)(*base_tcont_id + 2));
    tcont_group_3 = gpon_tcont_to_rdd_wan_channel((uint8_t)(*base_tcont_id + 3));

    rc = pon_sm_cb.pon_tx_set_tcont_to_counter_group_association(tcont_group_0,
        tcont_group_1, tcont_group_2, tcont_group_3);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "gpon configuration error\n");

    priv->tcont_counter_assigment = *base_tcont_id;
#endif

    return rc;
}

/* "link_stat" attribute "read" callback */
static int gpon_attr_link_stat_read(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, void *val, uint32_t size)
{
#ifndef BDMF_SYSTEM_SIM
    rdpa_gpon_stat_t *stat = (rdpa_gpon_stat_t *)val;
    int rc = 0;
    PON_RX_PLOAM_COUNTERS_DTE rx_ploam_ctr;
    PON_RX_HEC_COUNTERS_DTE   rx_pm_hec_ctr;
    uint32_t fec_corrected_bytes = 0;
    uint32_t fec_corrected_codewords = 0;
    uint32_t fec_uncorrectable_codewords = 0;

    memset(&rx_ploam_ctr,0,sizeof(PON_RX_PLOAM_COUNTERS_DTE));
    memset(&rx_pm_hec_ctr,0,sizeof(PON_RX_HEC_COUNTERS_DTE));

    /* Get the RX counters */
    rc = pon_sm_cb.pon_get_bip_error_counter(&stat->bip_errors);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "error - (%d) in func :pon_get_bip_error_counter\n", rc);

    rc = pon_sm_cb.pon_get_rxpon_ploam_counters(&rx_ploam_ctr);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "error - (%d) in func :pon_get_rxpon_ploam_counters\n", rc);

    stat->rx_onu_id = rx_ploam_ctr.valid_onu_id_ploam_counter;
    stat->rx_broadcast = rx_ploam_ctr.valid_broadcast_ploam_counter;

    rc = pon_sm_cb.pon_get_ploam_crc_error_counter(CE_COUNTER_RDPA_REQUEST, &stat->crc_errors);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "error - (%d) in func :pon_get_ploam_crc_error_counter\n", rc);

    rc = pon_sm_cb.pon_get_rx_unkonw_ploam_counter(&stat->rx_unknown);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "error - (%d) in func :pon_get_rx_unkonw_ploam_counter\n", rc);

    /* Get the TX counters */
    rc = pon_sm_cb.pon_get_txpon_pm_counters(&stat->tx_illegal_access, &stat->tx_idle, &stat->tx_ploam);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "error - (%d) in func :pon_get_txpon_pm_counters\n", rc);

    rc = pon_sm_cb.pon_rx_get_fec_counters(&fec_corrected_bytes, &fec_corrected_codewords, &fec_uncorrectable_codewords);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "error - (%d) in func :pon_rx_get_fec_counters\n", rc);

    stat->fec_errors = fec_uncorrectable_codewords;

    rc = pon_sm_cb.pon_rx_get_hec_counters(&rx_pm_hec_ctr);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "error - (%d) in func :pon_rx_get_hec_counters\n", rc);

    stat->hec_errors = rx_pm_hec_ctr.hlend_hec_err_counter;
#endif /* BDMF_SYSTEM_SIM */

    return 0;
}

/* "ds_fec_status" attribute "read" callback */
static int gpon_attr_ds_fec_state_read(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, void *val, uint32_t size)
{
#ifndef BDMF_SYSTEM_SIM
    bdmf_boolean *state = (bdmf_boolean *)val;
    int rc = 0;
    bdmf_boolean lof_state, lcdg_state;

    rc = pon_sm_cb.pon_rx_get_receiver_status(&lof_state, state, &lcdg_state);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "error - (%d) in func :gpon_get_receiver_status\n", rc);
#endif

    return 0;
}


/* "fec_stat" attribute "read" callback */
static int gpon_attr_fec_stat_read(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, void *val, uint32_t size)
{
    int rc = 0;

#ifndef BDMF_SYSTEM_SIM
    bdmf_boolean lof_state;
    bdmf_boolean fec_state;
    bdmf_boolean lcdg_state;
    rdpa_fec_stat_t *stat = (rdpa_fec_stat_t *)val;

    /* Check the receiver status fec bit */
    rc = pon_sm_cb.pon_rx_get_receiver_status(&lof_state, &fec_state, &lcdg_state);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "can't read tranceiver status, error = %d\n", rc);

    /* Reset return counters */
    memset(stat, 0, sizeof(rdpa_fec_stat_t));

    /* If the DS FEC is disabled return */
    if (!fec_state)
        return 0;

    /* Read FEC counters */
    rc = pon_sm_cb.pon_rx_get_fec_counters(&stat->corrected_bytes, &stat->corrected_codewords, &stat->uncorrectable_codewords);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "can't read FEC counters, error = %d\n", rc);
#endif

    return rc;
}

/* "rogue_onu" attribute "read" callback */
static int gpon_attr_rogue_onu_read(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, void *val, uint32_t size)
{
    gpon_drv_priv_t *priv = (gpon_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_rogue_onu_t *rogue_onu = (rdpa_rogue_onu_t *)val;

    rogue_onu->enable = priv->rogue_onu.enable;
    rogue_onu->mode = priv->rogue_onu.mode;
    rogue_onu->clock_cycle = priv->rogue_onu.clock_cycle;
    rogue_onu->gpio_pin = priv->rogue_onu.gpio_pin;

    return 0;
}

/* "misc_tx" attribute "read" callback */
static int gpon_attr_misc_tx_read(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, void *val, uint32_t size)
{
    gpon_drv_priv_t *priv = (gpon_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_misc_tx_t *misc_tx = (rdpa_misc_tx_t *)val;

    misc_tx->enable = priv->misc_tx.enable;
    misc_tx->prl = priv->misc_tx.prl;
    misc_tx->prcl = priv->misc_tx.prcl;
    memcpy(misc_tx->prodc, priv->misc_tx.prodc, 16*sizeof(char));
    misc_tx->brc = priv->misc_tx.brc;
    misc_tx->msstart = priv->misc_tx.msstart;
    misc_tx->msstop = priv->misc_tx.msstop;

    return 0;
}

/* "stack mode" attribute "write" callback */
static int gpon_attr_stack_mode_write(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, const void *val, uint32_t size)
{
    int rc = 0;
    PON_MAC_MODE  rdpa_stack_mode_2_pon_mac_mode[5] = {
       NGPON_MODE_XGPON,      /* rdpa_stack_mode_xgpon */
       NGPON_MODE_NGPON2_10G, /* rdpa_stack_mode_ngpon2_10g */
       NGPON_MODE_NGPON2_2_5G,/* rdpa_stack_mode_ngpon2_2_5g */
       NGPON_MODE_XGS,        /* rdpa_stack_mode_xgs */
       GPON_MODE              /* rdpa_stack_mode_gpon */
    };
#ifndef BDMF_SYSTEM_SIM
    gpon_drv_priv_t *priv = (gpon_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_gpon_stack_mode_t *stack_mode = (rdpa_gpon_stack_mode_t *)val;
    PON_MAC_MODE  mac_mode = rdpa_stack_mode_2_pon_mac_mode[*stack_mode];
    pon_sm_cb.set_stack_mode(mac_mode);

    priv->stack_mode = *stack_mode;
#endif /* BDMF_SYSTEM_SIM */

    return rc;
}

/* "multicast encryption key" attribute "write" callback */
static int gpon_attr_mcast_enc_key_write(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, const void *val, uint32_t size)
{
    int rc = 0;
#ifndef BDMF_SYSTEM_SIM
    gpon_drv_priv_t *priv = (gpon_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_gpon_mcast_enc_key_param_t *mcast_enc_key = (rdpa_gpon_mcast_enc_key_param_t *)val;

    rc = pon_sm_cb.set_multicast_encryption_key(1, mcast_enc_key->key_1);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "internal error in func: mcast_enc_key_write\n");
    rc = pon_sm_cb.set_multicast_encryption_key(2, mcast_enc_key->key_2);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "internal error in func: mcast_enc_key_write\n");

    memcpy (&(priv->mcast_enc_key), mcast_enc_key, sizeof (*mcast_enc_key));
#endif /* BDMF_SYSTEM_SIM */

    return rc;
}

static int gpon_attr_ploam_write(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, const void *val, uint32_t size)
{
    int rc = 0;
#ifndef BDMF_SYSTEM_SIM
    PON_PST_INDICATION_PARAMETERS_DTE pst_params;
    rdpa_send_ploam_params_t *ploam = (rdpa_send_ploam_params_t *)val;
#endif

#ifndef BDMF_SYSTEM_SIM
    /* This attribute flag is set to BDMF_ATTR_NOLOCK to prevent
     * locking when dying gasp interrupt is running.
     * in all other cases we would like to keep syncronization so
     * we manually lock the bdmf */
    switch (ploam->ploam_type)
    {
    case gpon_ploam_type_dgasp:
       rc = pon_sm_cb.pon_send_dying_gasp_message();
       break;

    case gpon_ploam_type_pee:
       bdmf_lock();
       rc = pon_sm_cb.pon_send_pee_message();
       bdmf_unlock();
       break;

    case gpon_ploam_type_pst:
       pst_params.k1 = ploam->pst_params.k1_value;
       pst_params.k2 = ploam->pst_params.k2_value;
       pst_params.line_number = ploam->pst_params.line_number;
       bdmf_lock();
       rc = pon_sm_cb.pon_send_pst_message(pst_params);
       bdmf_unlock();
       break;
    }

    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Send PLOAM driver error\n");
#endif

    return rc;
}

static int gpon_attr_dba_write(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, const void *val, uint32_t size)
{
    int rc = 0;

#ifndef BDMF_SYSTEM_SIM
    gpon_drv_priv_t *priv = (gpon_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_dba_interval_t *dba_interval = (rdpa_dba_interval_t *)val;

    /* Can not change interval while report is running */
    if ((priv->dba_interval != gpon_dba_disable) && (*dba_interval != gpon_dba_disable))
        return BDMF_ERR_ALREADY;

    if (*dba_interval == gpon_dba_disable) /* Stop status report */
        rc = pon_sm_cb.pon_dba_sr_process_terminate();
    else /* start status report */
        rc = pon_sm_cb.pon_dba_sr_process_initialize(priv->gem_block_size, (uint32_t)(*dba_interval));

    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Failed to %s DBA status report\n", *dba_interval ?  "enable" : "disable");

    priv->dba_interval = *dba_interval;
#endif

    return rc;
}

/* "rogue_onu" attribute "write" callback */
static int gpon_attr_rogue_onu_write(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, const void *val, uint32_t size)
{
    int rc = 0;
#ifndef BDMF_SYSTEM_SIM
    gpon_drv_priv_t *priv = (gpon_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_rogue_onu_t *rogue_onu = (rdpa_rogue_onu_t *)val;

    rc = pon_sm_cb.rogue_onu_detect_cfg(rogue_onu, &priv->rogue_onu);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "internal error in func: rogue_cfg\n");

    priv->rogue_onu = *rogue_onu;
#endif /* BDMF_SYSTEM_SIM */

    return rc;
}

/* "misc_tx" attribute "write" callback */
static int gpon_attr_misc_tx_write(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, const void *val, uint32_t size)
{
    int rc = 0;
#ifndef BDMF_SYSTEM_SIM
    gpon_drv_priv_t *priv = (gpon_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_misc_tx_t *misc_tx = (rdpa_misc_tx_t *)val;

    if (priv->misc_tx.enable == misc_tx->enable)
    {
        BDMF_TRACE_RET(BDMF_ERR_PARM, "state is already %s\n",
            misc_tx->enable ? "enable" : "disable");
    }

    if (misc_tx->prcl > RDPA_MAX_PREAMBLE_LEN)
        return BDMF_ERR_RANGE;

    rc = pon_sm_cb.pon_tx_generate_misc_transmit(misc_tx->prodc, misc_tx->prcl,
        misc_tx->brc, misc_tx->prl, misc_tx->msstart, misc_tx->msstop, misc_tx->enable);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "internal error in func: misc_tx\n");

    priv->misc_tx.enable = misc_tx->enable;
    priv->misc_tx.prl = misc_tx->prl;
    priv->misc_tx.prcl = misc_tx->prcl;
    memcpy(priv->misc_tx.prodc, misc_tx->prodc, 16*sizeof(uint8_t));
    priv->misc_tx.brc = misc_tx->brc;
    priv->misc_tx.msstart = misc_tx->msstart;
    priv->misc_tx.msstop = misc_tx->msstop;
#endif

    return rc;
}

/* "gem_block_size" attribute "write" callback. */
static int gpon_attr_gem_block_size_write(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, const void *val, uint32_t size)
{
    gpon_drv_priv_t *priv = (gpon_drv_priv_t *)bdmf_obj_data(mo);
    uint32_t *gem_block_size = (uint32_t *)val;
    int rc = 0;

    if (*gem_block_size != 32 && *gem_block_size != 48 && *gem_block_size != 64)
        BDMF_TRACE_RET(BDMF_ERR_PARM, "size of %d is not a legal gpon block size\n", *gem_block_size);

    if (priv->gem_block_size == *gem_block_size)
        return 0;

    if (mo->state == bdmf_state_active)
    {
#ifndef BDMF_SYSTEM_SIM
        rc = pon_sm_cb.pon_set_gem_block_size(*gem_block_size);
        if (rc)
            BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "can't set new block size %d\n", *gem_block_size);
#endif
    }

    priv->gem_block_size = *gem_block_size;

    return rc;
}

/* "bw_record_cfg" attribute "write" callback */
static int gpon_attr_bw_record_cfg_write(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, const void *val, uint32_t size)
{
    int rc = 0;
#ifndef BDMF_SYSTEM_SIM
    gpon_drv_priv_t *priv = (gpon_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_gpon_bw_record_cfg_t *cfg = (rdpa_gpon_bw_record_cfg_t *)val;

    if (cfg->enable)
    {
        rc = pon_sm_cb.pon_start_bw_recording(cfg->rcd_stop,
            cfg->record_type, cfg->alloc_id);
    }
    else
        rc = pon_sm_cb.pon_stop_bw_recording();

    if (rc)
    {
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL,
            "Error %s BW recording, rc=%d\n", cfg->enable ? "start" : "stop", rc);
    }

    priv->bw_record_cfg = *cfg;
#endif

    return rc;
}

/* "burst_prof" attribute "read" callback. */
static int gpon_attr_burst_prof_read(struct bdmf_object *mo, struct bdmf_attr *ad,
    bdmf_index index, void *val, uint32_t size)
{
    int rc = 0;
    gpon_drv_priv_t *priv = (gpon_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_gpon_burst_prof_t *cfg = (rdpa_gpon_burst_prof_t *)val;

#ifndef BDMF_SYSTEM_SIM
    if ( priv->stack_mode == rdpa_stack_mode_gpon )
    {
        return BDMF_ERR_NOENT;
    }

    *cfg = priv->burst_prof[index];

    if ( (rc = pon_sm_cb.pon_get_burst_prof(cfg, index)) == PON_BURST_PROF_NOT_ENABLED_YET )
        return BDMF_ERR_NOENT;
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "can't get burst profile %ld, error = %d\n", index, rc);
#endif
    return rc;
}

/* "bw_record_result_gpon" attribute "read" callback */
static int gpon_attr_bw_record_result_gpon_read(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, void *val, uint32_t size)
{
    int rc = 0;
    gpon_drv_priv_t *priv = (gpon_drv_priv_t *)bdmf_obj_data(mo);
#ifndef BDMF_SYSTEM_SIM
    rdpa_gpon_bw_record_result_gpon_t *result = (rdpa_gpon_bw_record_result_gpon_t *)val;
#endif

    /* Silently returns BDMF_ERR_NOENT if DBA recording is disabled */
    if (!priv->bw_record_cfg.enable)
        return BDMF_ERR_NOENT;

#ifndef BDMF_SYSTEM_SIM
    /* Read the table only once */
    if (!index)
    {
        rc = pon_sm_cb.pon_get_bw_recording_data(&accesses_array, &access_read_num);
        if (rc)
        {
            access_read_num = 0;
            BDMF_TRACE_RET(BDMF_ERR_INTERNAL,
                "Fail to read BW recording result, rc = %d\n", rc);
        }
    }

    /* Silently returns BDMF_ERR_NOENT for NGPON and for for irrelevant index */
    if (accesses_array.access_type || index >= access_read_num)
        return BDMF_ERR_NOENT;

    result->alloc_id = accesses_array.gpon_access[index].map_fields.alloc_id;
    result->sstart = accesses_array.gpon_access[index].map_fields.sstart;
    result->sstop = accesses_array.gpon_access[index].map_fields.sstop;
    result->sf_counter = accesses_array.gpon_access[index].map_fields.sf_counter;
    result->flag_pls = accesses_array.gpon_access[index].map_fields.flag_pls;
    result->flag_ploam = accesses_array.gpon_access[index].map_fields.flag_ploam;
    result->flag_fec = accesses_array.gpon_access[index].map_fields.flag_fec;
    result->flag_dbru = accesses_array.gpon_access[index].map_fields.flag_dbru;
    result->crc_valid = accesses_array.gpon_access[index].map_fields.crc;
#endif

    return rc;
}

/* "bw_record_result_ngpon" attribute "read" callback */
static int gpon_attr_bw_record_result_ngpon_read(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, void *val, uint32_t size)
{
    int rc = 0;
    gpon_drv_priv_t *priv = (gpon_drv_priv_t *)bdmf_obj_data(mo);
#ifndef BDMF_SYSTEM_SIM
    rdpa_gpon_bw_record_result_ngpon_t *result = (rdpa_gpon_bw_record_result_ngpon_t *)val;
#endif

    /* Silently returns BDMF_ERR_NOENT if DBA recording is disabled */
    if (!priv->bw_record_cfg.enable)
        return BDMF_ERR_NOENT;

#ifndef BDMF_SYSTEM_SIM
    /* Read the table only once */
    if (!index)
    {
        rc = pon_sm_cb.pon_get_bw_recording_data(&accesses_array, &access_read_num);
        if (rc)
        {
            access_read_num = 0;
            BDMF_TRACE_RET(BDMF_ERR_INTERNAL,
                "Fail to read BW recording result, rc = %d\n", rc);
        }
    }

    /* Silently returns BDMF_ERR_NOENT for GPON and for for irrelevant index */
    if (!accesses_array.access_type || index >= access_read_num)
        return BDMF_ERR_NOENT;

    result->starttime = accesses_array.ngpon_access[index].starttime;
    result->allocid = accesses_array.ngpon_access[index].allocid;
    result->sfc_ls = accesses_array.ngpon_access[index].sfc_ls;
    result->hec_ok = accesses_array.ngpon_access[index].hec_ok;
    result->bprofile = accesses_array.ngpon_access[index].bprofile;
    result->fwi = accesses_array.ngpon_access[index].fwi;
    result->ploamu = accesses_array.ngpon_access[index].ploamu;
    result->dbru = accesses_array.ngpon_access[index].dbru;
    result->grantsize = accesses_array.ngpon_access[index].grantsize;
#endif

    return rc;
}

/* "gem_enable" attribute write callback. */
static int gpon_attr_gem_enable_write(struct bdmf_object *mo, struct bdmf_attr *ad,
    bdmf_index index, const void *val, uint32_t size)
{
    int rc = 0;
    bdmf_number gem_index;
    bdmf_object_handle gem = (bdmf_object_handle)index;
#ifndef BDMF_SYSTEM_SIM
    bdmf_boolean *port_id_filter = (bdmf_boolean *)val;
#endif

    if (gem->drv != rdpa_gem_drv())
        return BDMF_ERR_PARM;

    if (bdmf_attr_get_as_num(gem, rdpa_gem_attr_index, &gem_index) < 0)
        return BDMF_ERR_NOENT;

    if (mo->state == bdmf_state_active)
    {
#ifndef BDMF_SYSTEM_SIM
       /* Enable GEM only if configuration was already set */
       if (rdpa_gem_is_ds_cfg_set(gem_index))
       {
           rc = pon_sm_cb.pon_control_port_id_filter(gem_index, *port_id_filter);
           if (rc)
           {
               BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Can't change port id filter to %s\n",
                   (*port_id_filter == 1 ? "enable" : "disable"));
           }
       }
#endif
    }

    return rc;
}

/* "gem_enable" attribute read callback. */
static int gpon_attr_gem_enable_read(struct bdmf_object *mo, struct bdmf_attr *ad,
    bdmf_index index, void *val, uint32_t size)
{
    int rc = 0;
    bdmf_number gem_index;
    bdmf_object_handle gem = (bdmf_object_handle)index;
#ifndef BDMF_SYSTEM_SIM
    PON_PORT_ID_DTE port_id_filter;
    PON_PORT_ID_DTE port_id_mask;
    bool encryption_mode;
    bool filter_status;
    PON_FLOW_TYPE_DTE flow_type;
    PON_FLOW_PRIORITY_DTE flow_priority;
    bool crc;
#endif

    if (!gem)
       return BDMF_ERR_NOENT;

    if (gem->drv != rdpa_gem_drv())
        return BDMF_ERR_PARM;

    if (bdmf_attr_get_as_num(gem, rdpa_gem_attr_index, &gem_index) < 0)
        return BDMF_ERR_NOENT;

    if (mo->state == bdmf_state_active)
    {
#ifndef BDMF_SYSTEM_SIM
        rc = pon_sm_cb.pon_get_port_id_configuration((PON_FLOW_ID_DTE)gem_index,
            &filter_status, &port_id_filter, &port_id_mask, &encryption_mode, &flow_type,
            &flow_priority, &crc);
        if (rc)
            BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Can't read port id configuration\n");

        (*(bdmf_boolean *)val) = (bdmf_boolean)filter_status;
#endif
    }

    return rc;
}

/* "gem_encryption" attribute write callback. */
static int gpon_attr_gem_encryption_write(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, const void *val, uint32_t size)
{
    int rc = 0;
    bdmf_number gem_index;
    bdmf_object_handle gem = (bdmf_object_handle)index;
#ifndef BDMF_SYSTEM_SIM
    bdmf_boolean *encrypt = (bdmf_boolean *)val;
#endif

    if (gem->drv != rdpa_gem_drv())
        return BDMF_ERR_PARM;

    if (bdmf_attr_get_as_num(gem, rdpa_gem_attr_index, &gem_index) < 0)
        return BDMF_ERR_NOENT;

    if (mo->state == bdmf_state_active)
    {
#ifndef BDMF_SYSTEM_SIM
        /* Modify encryption only if ds_cfg was already set */
        if (rdpa_gem_is_ds_cfg_set(gem_index))
        {
            rc = pon_sm_cb.pon_modify_encryption_port_id_filter(gem_index, *encrypt);
            if (rc)
                BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "encryption modification failed, gpon error = %d\n", rc);
        }
#endif
    }

    return rc;
}

/* "gem_encryption" attribute read callback. */
static int gpon_attr_gem_encryption_read(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, void *val, uint32_t size)
{
    int rc = 0;
    bdmf_number gem_index;
    bdmf_object_handle gem = (bdmf_object_handle)index;
#ifndef BDMF_SYSTEM_SIM
    bool filter_status;
    PON_PORT_ID_DTE port_id_filter;
    PON_PORT_ID_DTE port_id_mask;
    bool encryption_mode;
    PON_FLOW_TYPE_DTE flow_type;
    PON_FLOW_PRIORITY_DTE flow_priority;
    bool crc;
#endif

    if (!gem)
       return BDMF_ERR_NOENT;

    if (gem->drv != rdpa_gem_drv())
        return BDMF_ERR_PARM;

    if (bdmf_attr_get_as_num(gem, rdpa_gem_attr_index, &gem_index) < 0)
        return BDMF_ERR_NOENT;

    if (mo->state == bdmf_state_active)
    {
#ifndef BDMF_SYSTEM_SIM
        rc = pon_sm_cb.pon_get_port_id_configuration((PON_FLOW_ID_DTE)gem_index,
            &filter_status, &port_id_filter, &port_id_mask, &encryption_mode,
            &flow_type, &flow_priority, &crc);
        if (rc)
            BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Can't read port id configuration\n");

        (*(bdmf_boolean *)val) = (bdmf_boolean)encryption_mode;
#endif
    }

    return rc;
}

/* "encryption_error_stat" attribute read callback */
static int gpon_attr_encryption_error_stat_read(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, void *val, uint32_t size)
{
    uint32_t rx_encryption_error = 0;
    uint32_t *stat = (uint32_t *)val;

#ifndef BDMF_SYSTEM_SIM
    if (pon_sm_cb.pon_get_encryption_error_counter(&rx_encryption_error))
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Can't read GPON Encryption errors counter\n");
#endif

    *stat = rx_encryption_error;

    return 0;
}

/* "gem_ds_cfg" attribute write callback. */
static int gpon_attr_gem_ds_cfg_write(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, const void *val, uint32_t size)
{
    int rc = 0;
    bdmf_number gem_index;
    bdmf_object_handle gem = (bdmf_object_handle)index;
#ifndef BDMF_SYSTEM_SIM
    rdpa_gpon_gem_ds_cfg_t *cfg = (rdpa_gpon_gem_ds_cfg_t *)val;
    bdmf_boolean cfg_gem = (cfg == NULL) ? 0 : 1;
    gpon_drv_priv_t *priv = (gpon_drv_priv_t *)bdmf_obj_data(mo);
    PON_FLOW_PRIORITY_DTE priority;
#endif

    if (gem->drv != rdpa_gem_drv())
        return BDMF_ERR_PARM;

    if (bdmf_attr_get_as_num(gem, rdpa_gem_attr_index, &gem_index) < 0)
        return BDMF_ERR_NOENT;

#ifndef BDMF_SYSTEM_SIM

    /* Configure GEM */
    if (cfg_gem)
    {
        if (mo->state == bdmf_state_active)
        {
            switch (cfg->flow_type)
            {
            case rdpa_gpon_flow_type_omci:
                priority = PON_FLOW_PRIORITY_OMCI;
                break;

            case rdpa_gpon_flow_type_ethernet:
            default:
               if (priv->stack_mode == rdpa_stack_mode_gpon)
               {
                   priority = cfg->discard_prty == rdpa_discard_prty_low ?
                       PON_FLOW_PRIORITY_LOW : PON_FLOW_PRIORITY_HIGH;
               }
               else
                   priority = PON_FLOW_PRIORITY_HIGH;
               break;
            }
            /* Configure port ID filter in GPON MAC */
            rc = pon_sm_cb.pon_configure_port_id_filter((PON_FLOW_ID_DTE)gem_index,
                cfg->port, GPON_PORT_ID_UNASSIGNED,
                priv->stack_mode == rdpa_stack_mode_gpon ? cfg->encryption : cfg->enc_ring,
                PON_FLOW_TYPE_ETH, priority, cfg->crc);
            if (rc)
            {
                BDMF_TRACE_RET(BDMF_ERR_INTERNAL,
                    "GPON MAC configure port filter failed: rc=%d\n", rc);
            }
        }
    }
#endif

    return rc;
}

/* "gem_ds_cfg" attribute read callback. */
static int gpon_attr_gem_ds_cfg_read(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, void *val, uint32_t size)
{
    bdmf_number gem_index;
    bdmf_object_handle gem = (bdmf_object_handle)index;
#ifndef BDMF_SYSTEM_SIM
    bool filter_status;
    PON_PORT_ID_DTE port_id_filter = 0;
    PON_PORT_ID_DTE port_id_mask;
    bool encryption_mode = 0;
    PON_FLOW_TYPE_DTE flow_type;
    PON_FLOW_PRIORITY_DTE flow_priority = 0;
    int rc;
    rdpa_gpon_gem_ds_cfg_t *cfg = (rdpa_gpon_gem_ds_cfg_t *)val;
    bool crc;
    gpon_drv_priv_t *priv = (gpon_drv_priv_t *)bdmf_obj_data(mo);
#endif

    if (!gem)
       return BDMF_ERR_NOENT;

    if (bdmf_attr_get_as_num(gem, rdpa_gem_attr_index, &gem_index) < 0)
        return BDMF_ERR_NOENT;

    if (mo->state == bdmf_state_active)
    {
#ifndef BDMF_SYSTEM_SIM
        rc = pon_sm_cb.pon_get_port_id_configuration((PON_FLOW_ID_DTE)gem_index,
            &filter_status, &port_id_filter, &port_id_mask, &encryption_mode,
            &flow_type, &flow_priority, &crc);
        if (rc)
            BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Can't read port id configuration\n");

        cfg->port = port_id_filter;
        cfg->discard_prty = (flow_priority == PON_FLOW_PRIORITY_UNASSIGNED || flow_priority == PON_FLOW_PRIORITY_LOW) ?
                            rdpa_discard_prty_low : rdpa_discard_prty_high;
        cfg->encryption = encryption_mode;
        if (priv->stack_mode != rdpa_stack_mode_gpon)
        {
          cfg->enc_ring = encryption_mode;   /* XGPON/NGPON stack actually returns enc_ring in OMCI MIB format where:
                                                0 - unencrypted,
                                                1 - unicast bidirectional encryption
                                                2 - broadcast (multicast) encryption
                                                3 - unicast downstream only
                                              */
        }
#endif
    }

    return 0;
}

/* "gem_enable" attribute get_next callback. */
static int gpon_attr_gem_enable_get_next(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index *index)
{
    bdmf_object_handle next_gem;
    bdmf_object_handle current_gem = *(bdmf_object_handle *)index;

    if (*index == BDMF_INDEX_UNASSIGNED)
        current_gem = NULL;

    next_gem = bdmf_get_next(rdpa_gem_drv(), current_gem, NULL);

    if (!next_gem)
       return BDMF_ERR_NO_MORE;

    *index = (bdmf_index)next_gem;
    return 0;
}

/* "gem_encryption" attribute get_next callback. */
static int gpon_attr_gem_encryption_get_next(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index *index)
{
    bdmf_object_handle next_gem = NULL;
    bdmf_object_handle current_gem = *(bdmf_object_handle *)index;

    if (*index == BDMF_INDEX_UNASSIGNED)
        current_gem = NULL;

    next_gem = bdmf_get_next(rdpa_gem_drv(), current_gem, NULL);

    if (!next_gem)
       return BDMF_ERR_NO_MORE;

    *index = (bdmf_index)next_gem;
    return 0;
}

/* "gem_ds_cfg" attribute get_next callback. */
static int gpon_attr_gem_ds_cfg_get_next(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index *index)
{
    bdmf_object_handle next_gem = NULL;
    bdmf_object_handle current_gem = *(bdmf_object_handle *)index;
    bdmf_number gem_index;

    if (*index == BDMF_INDEX_UNASSIGNED)
        current_gem = NULL;

    while(1)
    {
        next_gem = bdmf_get_next(rdpa_gem_drv(), current_gem, NULL);
        if (!next_gem)
            return BDMF_ERR_NO_MORE;

        bdmf_attr_get_as_num(next_gem, rdpa_gem_attr_index, &gem_index);

#ifndef BDMF_SYSTEM_SIM
        if (rdpa_gem_is_ds_cfg_set(gem_index))
        {
            *index = (bdmf_index)next_gem;
            return 0;
        }
#endif
        current_gem = next_gem;
    }

    return 0;
}

/* "tcont_alloc_id" attribute get_next callback. */
static int gpon_attr_tcont_alloc_id_get_next(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index *index)
{
    bdmf_object_handle next_tcont = NULL;
    bdmf_object_handle current_tcont = *(bdmf_object_handle *)index;
    bdmf_number tcont_index;

    if (*index == BDMF_INDEX_UNASSIGNED)
        current_tcont = NULL;

    while(1)
    {
        next_tcont = bdmf_get_next(rdpa_tcont_drv(), current_tcont, NULL);
        if (!next_tcont)
            return BDMF_ERR_NO_MORE;

        bdmf_attr_get_as_num(next_tcont, rdpa_tcont_attr_index, &tcont_index);

#ifndef BDMF_SYSTEM_SIM
        if (rdpa_gpon_is_alloc_exist(tcont_index))
        {
            *index = (bdmf_index)next_tcont;
            return 0;
        }
#endif
        current_tcont = next_tcont;
    }

    return 0;
}

/* "assign_ploam_flag" attribute get_next callback. */
static int gpon_attr_tcont_assign_ploam_flag_get_next(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index *index)
{
    bdmf_object_handle next_tcont = NULL;
    bdmf_object_handle current_tcont = *(bdmf_object_handle *)index;

    if (*index == BDMF_INDEX_UNASSIGNED)
        current_tcont = NULL;

    next_tcont = bdmf_get_next(rdpa_tcont_drv(), current_tcont, NULL);

    if (!next_tcont)
       return BDMF_ERR_NO_MORE;

    *index = (bdmf_index)next_tcont;
    return 0;
}

/* "tcont_stat" attribute get_next callback. */
static int gpon_attr_tcont_stat_get_next(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index *index)
{
    bdmf_object_handle next_tcont = NULL;
    bdmf_object_handle current_tcont = *(bdmf_object_handle *)index;
    bdmf_number tcont_index;
    gpon_drv_priv_t *priv = (gpon_drv_priv_t *)bdmf_obj_data(mo);

    if (*index == BDMF_INDEX_UNASSIGNED)
        current_tcont = NULL;

    while(1)
    {
        next_tcont = bdmf_get_next(rdpa_tcont_drv(), current_tcont, NULL);
        if (!next_tcont)
            return BDMF_ERR_NO_MORE;

        bdmf_attr_get_as_num(next_tcont, rdpa_tcont_attr_index, &tcont_index);

        if (priv->tcont_counter_assigment == tcont_index ||
            priv->tcont_counter_assigment + 1 == tcont_index ||
            priv->tcont_counter_assigment + 2 == tcont_index ||
            priv->tcont_counter_assigment + 3 == tcont_index)
        {
            *index = (bdmf_index)next_tcont;
            return 0;
        }

        current_tcont = next_tcont;
    }

    return 0;
}

/* "tcont_alloc_id" attribute write callback */
static int gpon_attr_tcont_alloc_id_write(struct bdmf_object *mo, struct bdmf_attr *ad,
    bdmf_index index, const void *val, uint32_t size)
{
    bdmf_object_handle tcont = (bdmf_object_handle)index;
    uint32_t *alloc_id = (uint32_t *)val;
    int rc = 0;
    bdmf_number tcont_index;

    if (tcont->drv != rdpa_tcont_drv())
        return BDMF_ERR_PARM;

    if (bdmf_attr_get_as_num(tcont, rdpa_tcont_attr_index, &tcont_index) < 0)
        return BDMF_ERR_NOENT;

    /* Alloc-ID range check [for tcont 0-OMCI flow, alloc id is ONU-ID by default] */
    if ((tcont_index != 0) &&
        (*alloc_id < ALLOC_ID_RANGE_LOW || *alloc_id > ALLOC_ID_RANGE_HIGH))
    {
        BDMF_TRACE_RET(BDMF_ERR_PARM, "Configure tcont %d alloc id %d out of range\n",
            (int)tcont_index, (int)(*alloc_id));
    }

    if (mo->state == bdmf_state_active)
    {
#ifndef BDMF_SYSTEM_SIM
        if (rdpa_gpon_is_alloc_exist(tcont_index))
            rc = pon_sm_cb.pon_modify_tcont(gpon_tcont_to_rdd_wan_channel(tcont_index), *alloc_id);
        else
            rc = pon_sm_cb.pon_configure_tcont(gpon_tcont_to_rdd_wan_channel(tcont_index), *alloc_id);

        if (rc)
        {
            BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Configure tcont %d alloc id %d failed\n",
                (int)tcont_index, (int)(*alloc_id));
        }
#endif
    }

    return rc;
}

/* "tcont_alloc_id" attribute delete callback */
static int gpon_attr_tcont_alloc_id_delete(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index)
{
    bdmf_object_handle tcont = (bdmf_object_handle)index;
    bdmf_number tcont_index;

    if (tcont->drv != rdpa_tcont_drv())
        return BDMF_ERR_PARM;

    if (bdmf_attr_get_as_num(tcont, rdpa_tcont_attr_index, &tcont_index) < 0)
        return BDMF_ERR_NOENT;

    if (mo->state == bdmf_state_active)
    {
#ifndef BDMF_SYSTEM_SIM
        if (rdpa_gpon_is_alloc_exist(tcont_index))
        {
            if (pon_sm_cb.pon_remove_tcont(gpon_tcont_to_rdd_wan_channel(tcont_index)))
            {
                BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Remove tcont %d failed\n",
                    (int)tcont_index);
            }
            _rdpa_pon_flush_tcont_fe_bbh(gpon_tcont_to_rdd_wan_channel(tcont_index));
        }
#endif
    }

    return 0;
}

/* "tcont_alloc_id" attribute read callback. */
static int gpon_attr_tcont_alloc_id_read(struct bdmf_object *mo, struct bdmf_attr *ad,
    bdmf_index index, void *val, uint32_t size)
{
    int rc = 0;
    bdmf_number tcont_index;
    bdmf_object_handle tcont = (bdmf_object_handle)index;
#ifndef BDMF_SYSTEM_SIM
    PON_TCONT_DTE tcont_cfg;
    uint32_t *alloc_id = (uint32_t *)val;
#endif

    if (!tcont)
       return BDMF_ERR_NOENT;

    if (tcont->drv != rdpa_tcont_drv())
        return BDMF_ERR_PARM;

    if (bdmf_attr_get_as_num(tcont, rdpa_tcont_attr_index, &tcont_index) < 0)
        return BDMF_ERR_NOENT;

    if (mo->state == bdmf_state_active)
    {
#ifndef BDMF_SYSTEM_SIM
        rc = pon_sm_cb.pon_get_tcont_configuration(gpon_tcont_to_rdd_wan_channel(tcont_index),
            &tcont_cfg);
        if (rc)
            BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Can't read tcont configuration\n");

        *alloc_id = (uint32_t)tcont_cfg.alloc_id;
#endif
    }


    return rc;
}

/* "tcont_stat" attribute read callback */
static int gpon_attr_tcont_stat_read(struct bdmf_object *mo, struct bdmf_attr *ad,
    bdmf_index index, void *val, uint32_t size)
{
    int rc = 0;
    bdmf_number tcont_index;
    bdmf_object_handle tcont = (bdmf_object_handle)index;
    gpon_drv_priv_t *priv = (gpon_drv_priv_t *)bdmf_obj_data(mo);
#ifndef BDMF_SYSTEM_SIM
    uint16_t requested_dbr_counter;
    uint16_t valid_access_counter;
    rdpa_tcont_stat_t *stat = (rdpa_tcont_stat_t *)val;
#endif

    if (!tcont)
       return BDMF_ERR_NOENT;

    if (tcont->drv != rdpa_tcont_drv())
        return BDMF_ERR_PARM;

    /* Silently returns BDMF_ERR_NOENT for unconfigured TCONT or TCONT not assigned */
    if ((bdmf_attr_get_as_num(tcont, rdpa_tcont_attr_index, &tcont_index) < 0) ||
        (unsigned)tcont_index >= RDPA_MAX_TCONT ||
        (priv->tcont_counter_assigment != tcont_index &&
         priv->tcont_counter_assigment + 1 != tcont_index &&
         priv->tcont_counter_assigment + 2 != tcont_index &&
         priv->tcont_counter_assigment + 3 != tcont_index))
    {
        return BDMF_ERR_NOENT;
    }

#ifndef BDMF_SYSTEM_SIM
    rc = pon_sm_cb.pon_get_tcont_queue_pm_counters((tcont_index - priv->tcont_counter_assigment),
        &stat->idle_gem_counter, &stat->non_idle_gem_counter, &stat->packet_counter,
        &requested_dbr_counter, &valid_access_counter);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "can't read tcont counters, error = %d\n", rc);

    stat->valid_access_counter = (uint32_t)valid_access_counter;
#endif

    return rc;
}

/* "tcont_assign_ploam_flag" attribute read callback */
static int gpon_attr_tcont_assign_ploam_flag_read(struct bdmf_object *mo, struct bdmf_attr *ad,
    bdmf_index index, void *val, uint32_t size)
{
    bdmf_number tcont_index;
    bdmf_object_handle tcont = (bdmf_object_handle)index;
#ifndef BDMF_SYSTEM_SIM
    bdmf_boolean *assign_ploam_flag = (bdmf_boolean *)val;
    PON_TCONT_DTE tcont_cfg;
#endif

    if (!tcont)
       return BDMF_ERR_NOENT;

    if (tcont->drv != rdpa_tcont_drv())
        return BDMF_ERR_PARM;

    if (bdmf_attr_get_as_num(tcont, rdpa_tcont_attr_index, &tcont_index) < 0)
        return BDMF_ERR_NOENT;

#ifndef BDMF_SYSTEM_SIM
    if (pon_sm_cb.pon_get_tcont_configuration(gpon_tcont_to_rdd_wan_channel(tcont_index), &tcont_cfg))
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "can't read assign_ploam_flag from gpon stack\n");

    *assign_ploam_flag = tcont_cfg.assign_alloc_valid_flag;
#endif

    return 0;
}

int _rdpa_tcont_enable_set(uint8_t tcont_id,  bdmf_boolean enabled)
{
  int rc = 0;

  bdmf_object_handle tcont_obj;
  bdmf_number tcont_index;
  uint8_t rdpa_tcont_id = TCONT_ID_TO_RDPA_WAN_FLOW(tcont_id) ;


  if ((rc = rdpa_tcont_get(rdpa_tcont_id, &tcont_obj)))
  {
    BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "_rdpa_tcont_enable_set() -> cannot get tcont object for tcont_id = %d rc = %d\n", (int) tcont_id, rc);
  }

  /* BDMF_TRACE_ERR("_rdpa_tcont_enable_set() -- rdpa_tcont_get() returned  %d\n", (int) rc); */

  if (tcont_obj)
  {
     rdpa_tcont_index_get(tcont_obj, &tcont_index);

     /*     BDMF_TRACE_ERR("_rdpa_tcont_enable_set() -- operating on tcont with index %d\n", (int) tcont_index); */



     rc = rdpa_tcont_enable_set(tcont_obj, enabled);

     bdmf_put(tcont_obj);

  }

  return rc;
}


/* flushes front End Buffer and BBH queues */
int _rdpa_pon_flush_tcont_fe_bbh(uint8_t tcont_id)
{
#define GPON_MAX_FLUSH_TRIES    20
    int i, rc = 0;
    bdmf_boolean flush_immediate = 0;
    bdmf_object_handle tcont_obj = NULL;
#ifdef CONFIG_BCM_XRDP
    bdmf_boolean flush_enable, fe_flush_done, bbh_flush_done;
#endif

    if (!rdpa_tcont_get(TCONT_ID_TO_RDPA_WAN_FLOW(tcont_id), &tcont_obj))
        _rdpa_tcont_enable_set(tcont_id, BDMF_FALSE) ;
       /* FE buffer flash */

    mdelay(10);

    rc = pon_sm_cb.pon_tx_set_flush_valid_bit(tcont_id, 0, 0);
    rc = rc ? rc : pon_sm_cb.pon_tx_set_flush_valid_bit(tcont_id, 1, flush_immediate);

    if (rc != PON_NO_ERROR)
    {
       BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "pon_tx_set_flush_valid_bit() failure rc = %d\n", rc);
    }

    /* Check if the the flush was done */
    for (i = 0; i < GPON_MAX_FLUSH_TRIES; i++)
    {
       uint8_t temp_tcont_id;
#ifdef CONFIG_BCM_XRDP
       uint8_t rdpa_tcont_id = TCONT_ID_TO_RDPA_WAN_FLOW(tcont_id) ;


       /* Check whether gpon front end flush has done */
       rc = pon_sm_cb.pon_tx_get_flush_valid_bit(&temp_tcont_id, &flush_enable, &flush_immediate,
               &fe_flush_done);

       if (rc)
          BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "pon_tx_get_flush_valid_bit() returns error %d\n", rc);

       if (temp_tcont_id != tcont_id)
          BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Unexpected tcont %u is being flushed\n", temp_tcont_id);

       /* Check whether WAN TX BBH is flushed */
       rc = rdpa_wan_tx_bbh_flush_status_get(rdpa_tcont_id, &bbh_flush_done);
       if (fe_flush_done &&  bbh_flush_done)
       {
           BDMF_TRACE_INFO("\nTCONT flush for TCONT=%d try %d  flush completed successfully\n",tcont_id, i);
           break;
       }
       mdelay(10);
#else
       bdmf_boolean flush_enable, flush_done;
       uint32_t bbh_flush_done, bbh_is_in_seg;

       rc = pon_sm_cb.pon_tx_get_flush_valid_bit(&temp_tcont_id, &flush_enable, &flush_immediate, &flush_done);

       /* get the bbh fifo counter */
       fi_bl_drv_bbh_get_gpon_bbh_fifo_clear(tcont_id, &bbh_flush_done);

       /* check if the bbh is transmitting to the tcont */
       fi_bl_drv_bbh_get_gpon_bbh_in_segmentation(tcont_id, &bbh_is_in_seg);

       if (rc)
       {
          BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "tcont_id:%d pon_tx_get_flush_valid_bit() -> %d\n", tcont_id, rc) ;
       }

       if (flush_done && bbh_flush_done && !bbh_is_in_seg)
       {
          BDMF_TRACE_INFO("\nTCONT flush for TCONT=%d try %d  flush completed successfully\n",tcont_id, i);
          break;
       }
#endif
    }

    if (i >=  GPON_MAX_FLUSH_TRIES)
    {
#ifdef CONFIG_BCM_XRDP
       BDMF_TRACE_ERR("\nTCONT flush stuck TCONT=%d Front End Flush Done %d BBH PD Empty indicator %d\n",tcont_id, fe_flush_done, bbh_flush_done);
#else
       BDMF_TRACE_ERR("\nTCONT flush stuck TCONT=%d\n",tcont_id);
#endif
       rc = BDMF_ERR_INTERNAL;
    }

    /* Wait for few micro seconds before de-asserting the flush,
       for the case in which BBH is still transmitting to the flushed tcont.  */
    mdelay(10);

    /* Exit flush mode */
    /* FE buffer flash end */
    pon_sm_cb.pon_tx_set_flush_valid_bit(tcont_id, 0, 0);
#undef GPON_MAX_FLUSH_TRIES

    if (tcont_obj)
    {
        _rdpa_tcont_enable_set(tcont_id, BDMF_TRUE) ;
        bdmf_put(tcont_obj);
    }

    return rc;
}




/* "flush_fe" attribute "write" callback */
/* This API flushes front End Buffer and BBH queues, but doesn't flush TCONT's queues
   It is user responsibility to set tcont queus to zero and flush them before flush FE & BBH.
   (Disable tcont should be used for this) */
static int gpon_attr_tcont_flush_write(struct bdmf_object *mo, struct bdmf_attr *ad,
    bdmf_index index, const void *val, uint32_t size)
{
    bdmf_object_handle tcont = (bdmf_object_handle)index;
    bdmf_boolean flush = *(bdmf_boolean *)val;
    bdmf_boolean tcont_enable;
    bdmf_number tcont_index;
    int rc;

    rdpa_tcont_index_get(tcont, &tcont_index);
    rdpa_tcont_enable_get(tcont, &tcont_enable);

    if (mo->state != bdmf_state_active || !flush || !tcont_enable)
        return 0;

#ifndef BDMF_SYSTEM_SIM
    rc = _rdpa_pon_flush_tcont_fe_bbh(gpon_tcont_to_rdd_wan_channel(tcont_index));
#endif /* #ifndef BDMF_SYSTEM_SIM */

    return 0;
}

/* "pon_id_param" attribute read callback */
static int gpon_attr_pon_id_param_read(struct bdmf_object *mo, struct bdmf_attr *ad,
    bdmf_index index, void *val, uint32_t size)
{
#ifndef BDMF_SYSTEM_SIM
    PON_ERROR_DTE rc;
    PON_ID_PARAMETERS pon_param;

    rc = pon_sm_cb.pon_get_pon_id_parameters(&pon_param);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Fail to read PON ID ploam paramters, error = %d\n", rc);
#endif

    ((rdpa_gpon_pon_id_param_t *)val)->pon_id_type = pon_param.pon_id_type;
    memcpy(((rdpa_gpon_pon_id_param_t *)val)->pon_identifier, pon_param.pon_identifier, sizeof(pon_param.pon_identifier));
    memcpy(((rdpa_gpon_pon_id_param_t *)val)->tx_optical_level, pon_param.tx_optical_level, sizeof(pon_param.tx_optical_level));

    return 0;
}

/* Object attribute descriptors */
static struct bdmf_attr gpon_attrs[] = {
    { .name = "link_activate", .help = "PON Link_activate / deactivate",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG,
        .type = bdmf_attr_enum, .ts.enum_table = &link_activate_enum_table,
        .offset = offsetof(gpon_drv_priv_t, link_activate),
        .write = gpon_attr_link_activate_write
    },
    { .name = "link_cfg", .help = "PON Link Configuration",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG,
        .type = bdmf_attr_aggregate, .ts.aggr_type_name = "gpon_link_cfg",
        .offset = offsetof(gpon_drv_priv_t, link_cfg), .write = gpon_attr_link_cfg_write,
        .read = gpon_attr_link_cfg_read
    },
    { .name = "pon_link_state", .help = "Link state",
        .type = bdmf_attr_aggregate, .ts.aggr_type_name = "gpon_link_state",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_CONFIG,
        .offset = offsetof(gpon_drv_priv_t, link_state), .read = gpon_attr_link_state_read
    },
    { .name = "onu_id", .help = "ONU ID",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_CONFIG,
        .type = bdmf_attr_number, .size = sizeof(bdmf_index),
        .offset = offsetof(gpon_drv_priv_t, onu_id), .read = gpon_attr_onu_id_read
    },
    { .name = "onu_sn", .help = "ONU serial number",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG,
        .type = bdmf_attr_aggregate, .ts.aggr_type_name = "onu_sn",
        .offset = offsetof(gpon_drv_priv_t, onu_sn), .write = gpon_attr_onu_sn_write,
        .read = gpon_attr_onu_sn_read
    },
    { .name = "password", .help = "ONU password",
        .type = bdmf_attr_buffer, .size = sizeof(rdpa_onu_password_t),
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG,
        .offset = offsetof(gpon_drv_priv_t, onu_password), .write = gpon_attr_password_write
    },
    { .name = "user_ic", .help = "User indication callback",
        .type = bdmf_attr_pointer, .size = sizeof(void *),
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG,
        .offset = offsetof(gpon_drv_priv_t, user_ic), .write = gpon_attr_user_ic_write
    },
    { .name = "link_stat", .help = "Link statistics",
        .type = bdmf_attr_aggregate, .ts.aggr_type_name = "gpon_link_stat",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_STAT,
        .read = gpon_attr_link_stat_read
    },
    { .name = "overhead_cfg", .help = "Overhead configuration",
        .type = bdmf_attr_aggregate, .ts.aggr_type_name = "gpon_overhead_cfg",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_CONFIG,
        .offset = offsetof(gpon_drv_priv_t, overhead_cfg), .read = gpon_attr_overhead_read
    },
    { .name = "transceiver_power_level", .help = "Transceiver power level configuration",
        .type = bdmf_attr_number,
        .flags = BDMF_ATTR_READ | BDMF_ATTR_CONFIG,
        .offset = offsetof(gpon_drv_priv_t, transceiver_power_level), .size = sizeof(uint8_t),
        .read = gpon_attr_transceiver_power_level_read
    },
    { .name = "equalization_delay", .help = "Equalization delay",
        .type = bdmf_attr_number, .flags = BDMF_ATTR_READ | BDMF_ATTR_CONFIG,
        .offset = offsetof(gpon_drv_priv_t, equalization_delay), .size = sizeof(uint32_t),
        .read = gpon_attr_equalization_delay_read
    },
    { .name = "encryption_key", .help = "Encryption key",
        .type = bdmf_attr_buffer, .flags = BDMF_ATTR_READ | BDMF_ATTR_CONFIG,
        .offset = offsetof(gpon_drv_priv_t, encryption_key), .size = PLOAM_ENCRYPTION_KEY_SIZE,
        .read = gpon_attr_encryption_key_read
    },
    { .name = "tcont_counter_assigment", .help = "TCONT counter assigment",
        .type = bdmf_attr_number,
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG,
        .offset = offsetof(gpon_drv_priv_t, tcont_counter_assigment), .size = sizeof(uint32_t),
        .write = gpon_attr_tcont_counter_assign_write
    },
    { .name = "ds_fec_state", .help = "DS FEC enable state",
       .size = sizeof(bdmf_boolean), .type = bdmf_attr_boolean,
       .flags = BDMF_ATTR_READ, .read = gpon_attr_ds_fec_state_read
    },
    { .name = "fec_stat", .help = "FEC statistics",
        .type = bdmf_attr_aggregate, .ts.aggr_type_name = "fec_stat",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_STAT,
        .read = gpon_attr_fec_stat_read
    },
    { .name = "send_ploam", .help = "Send ploam",
        .type = bdmf_attr_aggregate, .ts.aggr_type_name = "send_ploam",
        .flags = BDMF_ATTR_WRITE | BDMF_ATTR_NOLOCK,
        .write = gpon_attr_ploam_write
    },
    { .name = "dba_interval", .help = "DBA Status Report",
        .type = bdmf_attr_enum, .ts.enum_table = &dba_interval_enum_table,
        .flags = BDMF_ATTR_READ | BDMF_ATTR_CONFIG | BDMF_ATTR_WRITE,
        .offset = offsetof(gpon_drv_priv_t, dba_interval), .write = gpon_attr_dba_write
    },
    { .name = "rogue_onu", .help = "rogue onu",
        .type = bdmf_attr_aggregate, .ts.aggr_type_name = "rogue_onu",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_CONFIG | BDMF_ATTR_WRITE,
        .offset = offsetof(gpon_drv_priv_t, rogue_onu),
        .read = gpon_attr_rogue_onu_read, .write = gpon_attr_rogue_onu_write
    },
    { .name = "misc_tx", .help = "Generate tx burst depended data",
        .type = bdmf_attr_aggregate, .ts.aggr_type_name = "misc_tx",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_CONFIG | BDMF_ATTR_WRITE,
        .offset = offsetof(gpon_drv_priv_t, misc_tx),
        .read = gpon_attr_misc_tx_read, .write = gpon_attr_misc_tx_write
    },
    { .name = "gem_block_size", .help = "Gem block size",
      .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG,
        .type = bdmf_attr_number, .size = sizeof(uint32_t),
        .offset = offsetof(gpon_drv_priv_t, gem_block_size),
      .write = gpon_attr_gem_block_size_write
    },
    { .name = "gem_enable", .help = "Enable GEM", .size = sizeof(bdmf_boolean),
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG |
            BDMF_ATTR_NO_RANGE_CHECK,
        .type = bdmf_attr_boolean, .array_size = RDPA_MAX_GEM_FLOW,
        .index_type = bdmf_attr_object, .write = gpon_attr_gem_enable_write,
        .read = gpon_attr_gem_enable_read,
        .get_next = gpon_attr_gem_enable_get_next
    },
    { .name = "gem_ds_cfg", .help = "Downstream GEM configuration",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG |
         BDMF_ATTR_NO_NULLCHECK | BDMF_ATTR_NO_RANGE_CHECK,
        .type = bdmf_attr_aggregate,
        .ts.aggr_type_name = "gem_ds_cfg", .array_size = RDPA_MAX_GEM_FLOW,
        .index_type = bdmf_attr_object, .write = gpon_attr_gem_ds_cfg_write,
        .read = gpon_attr_gem_ds_cfg_read,
        .get_next = gpon_attr_gem_ds_cfg_get_next
    },
    { .name = "gem_encryption", .help = "Downstream GEM flow Encryption mode",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG |
            BDMF_ATTR_NO_RANGE_CHECK,
        .type = bdmf_attr_boolean, .array_size = RDPA_MAX_GEM_FLOW,
        .index_type = bdmf_attr_object, .write = gpon_attr_gem_encryption_write,
        .read = gpon_attr_gem_encryption_read,
        .get_next = gpon_attr_gem_encryption_get_next
    },
    { .name = "tcont_alloc_id", .help = "TCONT Alloc ID",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG |
            BDMF_ATTR_NO_RANGE_CHECK,
        .type = bdmf_attr_number, .size = sizeof(uint32_t),
        .array_size = RDPA_MAX_TCONT, .index_type = bdmf_attr_object,
        .write = gpon_attr_tcont_alloc_id_write,
        .read = gpon_attr_tcont_alloc_id_read,
        .del = gpon_attr_tcont_alloc_id_delete,
        .get_next = gpon_attr_tcont_alloc_id_get_next
    },
    { .name = "tcont_assign_ploam_flag",
        .help = "TCONT Assign alloc id message arrived",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_NO_RANGE_CHECK,
        .type = bdmf_attr_boolean, .array_size = RDPA_MAX_TCONT,
        .index_type = bdmf_attr_object,
        .read = gpon_attr_tcont_assign_ploam_flag_read,
        .get_next = gpon_attr_tcont_assign_ploam_flag_get_next
    },
    { .name = "tcont_stat", .help = "TCONT statistics",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_STAT | BDMF_ATTR_NO_RANGE_CHECK,
        .type = bdmf_attr_aggregate, .ts.aggr_type_name = "tcont_stat",
        .array_size = RDPA_MAX_TCONT, .index_type = bdmf_attr_object,
        .read = gpon_attr_tcont_stat_read,
        .get_next = gpon_attr_tcont_stat_get_next
    },
    { .name = "tcont_flush", .help = "Flush TCONT Front End Buffer and BBH Queue",
        .flags = BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG | BDMF_ATTR_NO_RANGE_CHECK,
        .type = bdmf_attr_boolean, .array_size = RDPA_MAX_TCONT,
        .index_type = bdmf_attr_object,
        .write = gpon_attr_tcont_flush_write
    },
    { .name = "encryption_error_stat", .help = "GEM Encryption errors statistics",
        .type = bdmf_attr_number, .size = sizeof(uint32_t),
        .flags = BDMF_ATTR_READ | BDMF_ATTR_STAT,
        .read = gpon_attr_encryption_error_stat_read,
    },
    { .name = "pon_id_parameters",
        .help = "PON ID PLOAM parameters: PON-ID type, PON-Identifier, Transmit optical level",
        .type = bdmf_attr_aggregate, .ts.aggr_type_name = "pon_id_param",
        .flags = BDMF_ATTR_READ, .read = gpon_attr_pon_id_param_read
    },
    { .name = "mcast_enc_key",
        .help = "Multicast Encryption keys: first key, second key",
        .type = bdmf_attr_aggregate, .ts.aggr_type_name = "mcast_enc_key_param",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG ,
        .offset = offsetof(gpon_drv_priv_t, mcast_enc_key),
        .write = gpon_attr_mcast_enc_key_write
    },
    { .name = "stack_mode", .help = "Stack mode",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG,
        .type = bdmf_attr_enum, .ts.enum_table = &rdpa_stack_mode_enum_table,
        .offset = offsetof(gpon_drv_priv_t, stack_mode),
        .write = gpon_attr_stack_mode_write
    },
    { .name = "bw_record_cfg", .help = "BW recording setting",
        .type = bdmf_attr_aggregate, .ts.aggr_type_name = "bw_record_cfg",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_CONFIG | BDMF_ATTR_WRITE,
        .offset = offsetof(gpon_drv_priv_t, bw_record_cfg),
        .write = gpon_attr_bw_record_cfg_write
    },
    { .name = "bw_record_result_gpon", .help = "BW recording Result - GPON",
        .type = bdmf_attr_aggregate, .ts.aggr_type_name = "bw_record_result_gpon",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_STAT,
        .read = gpon_attr_bw_record_result_gpon_read,
        .array_size = GPON_NUMBER_OF_ACCESS_IN_MAP, .index_type = bdmf_attr_number,
    },
    { .name = "bw_record_result_ngpon", .help = "BW recording Result - NGPON",
        .type = bdmf_attr_aggregate, .ts.aggr_type_name = "bw_record_result_ngpon",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_STAT,
        .read = gpon_attr_bw_record_result_ngpon_read,
        .array_size = NGPON_NUMBER_OF_ACCESS_IN_MAP, .index_type = bdmf_attr_number,
    },
    { .name = "burst_prof", .help = "Burst profile",
        .type = bdmf_attr_aggregate, .ts.aggr_type_name = "burst_prof",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_CONFIG | BDMF_ATTR_NO_AUTO_GEN,
        .offset = offsetof(gpon_drv_priv_t, burst_prof),
        .read = gpon_attr_burst_prof_read,
        .array_size = NGPON_BURST_PROFILE_INDEX_HIGH + 1, .index_type = bdmf_attr_number,
    },
    BDMF_ATTR_LAST
};

/* Return gpon_object */
static int gpon_get(struct bdmf_type *drv,
    struct bdmf_object *owner, const char *discr,
    struct bdmf_object **pmo)
{
    if (!gpon_object)
        return BDMF_ERR_NOENT;
    *pmo = gpon_object;

    return 0;
}

/* Init/exit module. Cater for GPL layer */
static int gpon_drv_init(struct bdmf_type *drv)
{
#ifdef BDMF_DRIVER_GPL_LAYER
    f_rdpa_gpon_drv = rdpa_gpon_drv;
    f_rdpa_gpon_get = rdpa_gpon_get;
#endif
    return 0;
}

static void gpon_drv_exit(struct bdmf_type *drv)
{
#ifdef BDMF_DRIVER_GPL_LAYER
    f_rdpa_gpon_drv = NULL;
    f_rdpa_gpon_get = NULL;
#endif
}

struct bdmf_type gpon_drv =
{
    .name = "gpon",
    .parent = "system",
    .description = "GPON Link / ONU",
    .drv_init = gpon_drv_init,
    .drv_exit = gpon_drv_exit,
    .pre_init = gpon_pre_init,
    .post_init = gpon_post_init,
    .destroy = gpon_destroy,
    .get = gpon_get,
    .extra_size = sizeof(gpon_drv_priv_t),
    .aattr = gpon_attrs,
    .max_objs = 1,
};
DECLARE_BDMF_TYPE_EXT(rdpa_gpon, gpon_drv);

/*
 * Functions declared in auto-generated header
 */

int rdpa_gpon_get(bdmf_object_handle *gpon_obj)
{
    if (!gpon_object)
        return BDMF_ERR_NOENT;
    bdmf_get(gpon_object);
    *gpon_obj = gpon_object;
    return 0;
}


/*
 * GPON Internal functions
 */

#ifndef BDMF_SYSTEM_SIM
static int initialize_pon(void)
{
    bdmf_object_handle cpu = NULL;
    bdmf_object_handle system_obj = NULL;
    int rc = 0;
    gpon_drv_priv_t *gpon = (gpon_drv_priv_t *)bdmf_obj_data(gpon_object);
    rdpa_cpu_rxq_cfg_t rxq_cfg = {};

    bdmf_phys_addr_t ring_base;
    RING_CB_FUNC ringCb;

    /* Configure ONU link parameters */
    pon_sm_cb.pon_configure_link_params(&gpon->link_cfg);

    /* Reset PON link */
    rc = pon_sm_cb.pon_link_reset();
    if (rc)
        return BDMF_ERR_INTERNAL;

#ifndef CONFIG_BCM_XRDP
    /* unmask GPON interrupts in IC */
    bdmf_int_enable(RDPA_IC_GPON_TX_IRQ);
    bdmf_int_enable(RDPA_IC_GPON_RX_IRQ);
#endif

    /* Configure reason to CPU queue. */
    rc = rdpa_system_get(&system_obj);
    if (rc < 0)
        BDMF_TRACE_RET(rc, "Failed to get System object");

    rc = rdpa_cpu_get(rdpa_cpu_host, &cpu);
    if (rc < 0)
    {
        bdmf_put(system_obj);
        BDMF_TRACE_RET(rc, "Failed to get CPU object");
    }

    bdmf_lock();
    /* Create PLOAM cpu queue */
    rc = rdpa_cpu_rxq_cfg_get(cpu, RDPA_PLOAM_CPU_RX_QUEUE_ID, &rxq_cfg);
    if (rc < 0)
    {
        BDMF_TRACE_ERR("Failed to read CPU Rx queue ID %d", RDPA_PLOAM_CPU_RX_QUEUE_ID);
        goto unlock_exit;
    }

    rxq_cfg.isr_priv = RDPA_PLOAM_CPU_RX_QUEUE_ID;
    rxq_cfg.rx_isr = (void (*)(long))_ploam_isr_callback;

    ringCb.data_dump = rdp_packet_dump;
#ifndef XRDP
    ringCb.databuf_alloc = rdp_databuf_alloc_cache;
    ringCb.databuf_free = rdp_databuf_free_cache;
#endif
    ringCb.buff_mem_context = kmem_cache_create("PloamBufMem",
        PLOAM_PACKET_SIZE + RDPA_PKT_MIN_ALIGN, 0, SLAB_HWCACHE_ALIGN, NULL);

    rc = rdp_cpu_ring_create_ring(RDPA_PLOAM_CPU_RX_QUEUE_ID, rdpa_ring_data,
        PLOAM_BUFFER_QUANTUM, &ring_base, (PLOAM_PACKET_SIZE + RDPA_PKT_MIN_ALIGN), &ringCb);
    if (rc < 0)
        goto unlock_exit;

    rxq_cfg.size = PLOAM_BUFFER_QUANTUM;
    rxq_cfg.ring_head = (void *)ring_base;
    rxq_cfg.rxq_stat = rdp_cpu_rxq_stat_cb;
    rxq_cfg.rx_dump_data_cb = rdp_cpu_dump_data_cb;

    rc = rdpa_cpu_rxq_cfg_set(cpu, RDPA_PLOAM_CPU_RX_QUEUE_ID, &rxq_cfg);
    if (rc < 0)
    {
        BDMF_TRACE_ERR("Failed to configure PLOAM CPU Rx queue %d", rc);
        goto unlock_exit;
    }

    bdmf_unlock();

    /* XXX: Temporary removed until per-cpu queue statistics support is added */
    /*
    rc = rdpa_cpu_reason_stat_external_cb_set(cpu, rdp_cpu_reason_stat_cb);
    if (rc < 0)
        printk("Cannot configure CPU external reason statistics callback (%d)\n", rc);
    */

#ifdef CONFIG_BCM_XRDP
    /* Map reason OAM to special TC, and TC to RDPA_PLOAM_CPU_RX_QUEUE_ID.
       For simplicity, use TC = RDPA_PLOAM_CPU_RX_QUEUE_ID */
    rc = rdpa_system_cpu_reason_to_tc_set(system_obj, rdpa_cpu_rx_reason_oam, RDPA_PLOAM_CPU_RX_QUEUE_ID);
    rc = rc ? rc : rdpa_cpu_tc_to_rxq_set(cpu, RDPA_PLOAM_CPU_RX_QUEUE_ID, RDPA_PLOAM_CPU_RX_QUEUE_ID);
#else
    {
        rdpa_cpu_reason_index_t reason;
        rdpa_cpu_reason_cfg_t   reason_cfg = {};

        reason.dir = rdpa_dir_ds;
        reason.reason = rdpa_cpu_rx_reason_oam;
        reason_cfg.queue = RDPA_PLOAM_CPU_RX_QUEUE_ID;
        reason_cfg.meter = BDMF_INDEX_UNASSIGNED;

        rc = rc ? rc : rdpa_cpu_reason_cfg_set(cpu, &reason, &reason_cfg);

        /* Need to configure for upstream too because IH don't parse packets smaller than 64,
         * but FW still use the "wan" bit from parser result. */
        reason.dir = rdpa_dir_us;
        rc = rc ? rc : rdpa_cpu_reason_cfg_set(cpu, &reason, &reason_cfg);
    }
#endif

    rdpa_cpu_int_enable(rdpa_cpu_host, RDPA_PLOAM_CPU_RX_QUEUE_ID);

    /* Assign Default TCONT counters */
    pon_sm_cb.pon_tx_set_tcont_to_counter_group_association(
        gpon_tcont_to_rdd_wan_channel(gpon->tcont_counter_assigment),
        gpon_tcont_to_rdd_wan_channel(gpon->tcont_counter_assigment+1),
        gpon_tcont_to_rdd_wan_channel(gpon->tcont_counter_assigment+2),
        gpon_tcont_to_rdd_wan_channel(gpon->tcont_counter_assigment+3));

    /* 6858 sp_en_reg register removed. Assuming all relevant SP are always enabled. */
#ifndef CONFIG_BCM_XRDP
    fi_bl_drv_bpm_sp_enable(DRV_BPM_SP_GPON, DRV_BPM_ENABLE);
#endif

    bdmf_put(cpu);
    bdmf_put(system_obj);
    return rc;

unlock_exit:
    bdmf_unlock();
    bdmf_put(cpu);
    bdmf_put(system_obj);
    return rc;
}

static void _rdpa_indication_callback(PON_API_MESSAGE_OPCODES_DTE message_id,
    PON_USER_INDICATION_DTE user_indication_message)
{
    rdpa_pon_indication indication_type = rdpa_indication_none;
    rdpa_callback_indication indication_parameters;
    gpon_drv_priv_t *gpon = (gpon_drv_priv_t *)bdmf_obj_data(gpon_object);

    /* Process the message received from the GPON stack */
    switch (message_id)
    {
    case PON_API_MESSAGE_OPCODE_GPON_SM_TRANSITION:
         switch (user_indication_message.indication_data.onu_state_transition.onu_new_sub_state)
         {
         case LINK_SUB_STATE_STANDBY:
         case LINK_SUB_STATE_OPERATIONAL:
         case LINK_SUB_STATE_STOP:
              indication_parameters.state_transition_parameters.new_state =
                  user_indication_message.indication_data.onu_state_transition.onu_new_sub_state;
              break;

         default: /* CE_LINK_SUB_STATE_NONE */
              indication_parameters.state_transition_parameters.new_state = rdpa_pon_oper_state_none;
              break;
         }
         switch (user_indication_message.indication_data.onu_state_transition.onu_old_sub_state)
         {
         case LINK_SUB_STATE_STANDBY:
         case LINK_SUB_STATE_OPERATIONAL:
         case LINK_SUB_STATE_STOP:
              indication_parameters.state_transition_parameters.old_state =
                  user_indication_message.indication_data.onu_state_transition.onu_old_sub_state;
              break;

         default: /* CE_LINK_SUB_STATE_NONE */
              indication_parameters.state_transition_parameters.old_state = rdpa_pon_oper_state_none;
              break;
         }
         indication_type = rdpa_indication_state_transition;
         break;
    case PON_API_MESSAGE_OPCODE_GPON_LINK_STATE_TRANSITION:
         if (user_indication_message.indication_data.onu_link_state_transition.onu_new_link_state < OPERATION_NGPON2_STATES_NUMBER)
         {
             indication_parameters.link_state_transition_parameters.new_state =
                 user_indication_message.indication_data.onu_link_state_transition.onu_new_link_state;
         }
         else
         {
             indication_parameters.link_state_transition_parameters.new_state = 0 ;  /* default - O1 state */
         }

         if (user_indication_message.indication_data.onu_link_state_transition.onu_old_link_state < OPERATION_NGPON2_STATES_NUMBER)
         {
             indication_parameters.link_state_transition_parameters.old_state =
                 user_indication_message.indication_data.onu_link_state_transition.onu_old_link_state;
         }
         else
         {
             indication_parameters.link_state_transition_parameters.old_state = 0 ;  /* default - O1 state */
         }
         indication_type = rdpa_indication_link_state_transition;
         break;

    case PON_API_MESSAGE_OPCODE_PST_MESSAGE:
         indication_type = rdpa_indication_pst_msg;
         memcpy(indication_parameters.pst_ploam, &user_indication_message.indication_data.pst.pst_msg,
             RDPA_PLOAM_LENGTH);
         break;
    case PON_API_MESSAGE_OPCODE_BIP_ERROR:
         indication_type = rdpa_indication_err;
         indication_parameters.bip8_errors = user_indication_message.indication_data.bip_error.bip_error;
         break;
    case PON_API_MESSAGE_OPCODE_OAM_EVENT:
         switch (user_indication_message.indication_id)
         {
         case PON_INDICATION_PEE_OLT:
              indication_type = rdpa_indication_pee;
              break;
         case GPON_INDICATION_MEM:
              indication_type = rdpa_indication_mem;
              memcpy(indication_parameters.unkown_ploam_mem,
                  &user_indication_message.indication_data.gpon_mem.unknown_msg, RDPA_PLOAM_LENGTH);
              break;
         case NGPON_INDICATION_MEM:
              indication_type = rdpa_indication_mem;
              memcpy(indication_parameters.unkown_ploam_mem,
                     &user_indication_message.indication_data.ngpon_mem.unknown_msg, RDPA_PLOAM_LENGTH);  /* ????????? TODOSR */
              break;
         case PON_INDICATION_DACT:
              indication_type = rdpa_indication_dact;
              break;
         case PON_INDICATION_DIS:
              indication_type = rdpa_indication_dis;
              if (user_indication_message.indication_data.dis.alarm == 1)
                  indication_parameters.dis_status = rdpa_indication_on;
              else
                  indication_parameters.dis_status = rdpa_indication_off;
              break;
         case PON_INDICATION_LOF:
              indication_type = rdpa_indication_lof;
              if (user_indication_message.indication_data.lof.alarm == 1)
                  indication_parameters.lof_status = rdpa_indication_on;
              else
                  indication_parameters.lof_status = rdpa_indication_off;
              break;
        case PON_INDICATION_OPERATIONAL_LOF:
            indication_type = rdpa_indication_lof;
            indication_parameters.lof_status = rdpa_indication_on;
            break;
         case PON_INDICATION_LCDG:
              indication_type = rdpa_indication_lcdg;
              if (user_indication_message.indication_data.lcdg.alarm == 1)
                  indication_parameters.lcdg_status = rdpa_indication_on;
              else
                  indication_parameters.lcdg_status = rdpa_indication_off;
              break;
         case PON_INDICATION_SF:
             indication_type = rdpa_indication_sf;
             if (user_indication_message.indication_data.sf.alarm == 1)
                 indication_parameters.sf_status = rdpa_indication_on;
             else
                 indication_parameters.sf_status = rdpa_indication_off;
             break;
         case PON_INDICATION_SD:
             indication_type = rdpa_indication_sd;
             if (user_indication_message.indication_data.sd.alarm == 1)
                 indication_parameters.sd_status = rdpa_indication_on;
             else
                 indication_parameters.sd_status = rdpa_indication_off;
             break;
         default:
              return;
         }
         break;
    case PON_API_MESSAGE_OPCODE_RANGING_START:
         indication_type = rdpa_indication_ranging_start;
         break;
    case PON_API_MESSAGE_OPCODE_RANGING_STOP:
         indication_type = rdpa_indication_ranging_stop;
         switch (user_indication_message.indication_data.ranging_stop.ranging_stop_reason)
         {
         case PON_RANGING_STOP_REASON_TIMEOUT_EXPIRED:
              indication_parameters.ranging_stop = rdpa_ranging_stop_to1_timeout;
              break;
         case PON_RANGING_STOP_REASON_RANGING_END:
              indication_parameters.ranging_stop = rdpa_ranging_stop_successful_ranging;
              break;
         case PON_RANGING_STOP_REASON_DISABLE_SN:
              indication_parameters.ranging_stop = rdpa_ranging_stop_disable_onu;
              break;
         case PON_RANGING_STOP_REASON_DEACTIVATE_PON_ID:
              indication_parameters.ranging_stop = rdpa_ranging_stop_deactivate_onu;
              break;
         case PON_RANGING_STOP_REASON_ALARM_ON:
              indication_parameters.ranging_stop = rdpa_ranging_stop_link_folt;
              break;
         case PON_RANGING_STOP_REASON_LINK_DEACTIVATE:
              indication_parameters.ranging_stop = rdpa_ranging_stop_link_deactivate;
              break;
         case PON_RANGING_STOP_REASON_SN_UNMATCH:
              indication_parameters.ranging_stop = rdpa_ranging_stop_unmatch_sn;
              break;
         default:
              indication_parameters.ranging_stop = rdpa_ranging_stop_link_folt;
              break;
         }
         break;
    case PON_API_MESSAGE_OPCODE_ENCRYPT_PORT_ID:
         indication_type = rdpa_indication_encrypted_port_id_msg;
         indication_parameters.encrypted_port_id_parameters.port_id =
             user_indication_message.indication_data.configure_port.port_id;
         indication_parameters.encrypted_port_id_parameters.encryption =
             user_indication_message.indication_data.configure_port.encryption_mode;
         break;
    case PON_API_MESSAGE_OPCODE_CONFIGURE_OMCI_PORT_ID:
         indication_type = rdpa_indication_cfg_port_id_msg;
         indication_parameters.configure_port_id_parameters.port_id =
             user_indication_message.indication_data.configure_omci_port.port_id;
         if (user_indication_message.indication_data.configure_omci_port.port_control == 1)
             indication_parameters.configure_port_id_parameters.status = rdpa_indication_on;
         else
             indication_parameters.configure_port_id_parameters.status = rdpa_indication_off;
         break;
    case PON_API_MESSAGE_OPCODE_ASSIGN_ALLOC_ID_MESSAGE:
         indication_type = rdpa_indication_assign_alloc_id_msg;
         indication_parameters.assign_alloc_id_parameters.alloc_id =
             user_indication_message.indication_data.assign_alloc_id.alloc_id;
         if (user_indication_message.indication_data.assign_alloc_id.assign_flag == 1)
             indication_parameters.assign_alloc_id_parameters.status = rdpa_indication_on;
         else
             indication_parameters.assign_alloc_id_parameters.status = rdpa_indication_off;
         break;
    case PON_API_MESSAGE_OPCODE_ASSIGN_ONU_ID_MESSAGE:
         indication_type = rdpa_indication_assign_onu_id_msg;
         indication_parameters.onu_id = user_indication_message.indication_data.assign_onu_id.onu_id;
         break;
    case PON_API_MESSAGE_OPCODE_KEEPALIVE_TIMEOUT:
         return;
         break;
    case PON_API_MESSAGE_OPCODE_AES_KEY_SWITCHING_TIME:
        indication_type = rdpa_indication_key_switched;
        indication_parameters.key_switch_parameters.current_superframe =
            user_indication_message.indication_data.aes.current_superframe;
        indication_parameters.key_switch_parameters.switch_over_superframe =
            user_indication_message.indication_data.aes.key_switching_time;
        break;
    case PON_API_MESSAGE_OPCODE_AES_ERROR:
        indication_type = rdpa_indication_key_switched_out_of_sync;
        break;
    case PON_API_MESSAGE_OPCODE_ROGUE_ONU:
        indication_type = rdpa_indication_rouge_onu;
        indication_parameters.rogue_onu.status = user_indication_message.indication_data.rogue_onu.alarm;
        indication_parameters.rogue_onu.type = user_indication_message.indication_data.rogue_onu.rogue_type;
        break;
    case PON_API_MESSAGE_OPCODE_PMD_ALARM:
        indication_type = rdpa_indication_pmd_alarm;
        indication_parameters.pmd_alarm.esc_be = user_indication_message.indication_data.pmd_alarm.esc_be;
        indication_parameters.pmd_alarm.esc_rogue = user_indication_message.indication_data.pmd_alarm.esc_rogue;
        indication_parameters.pmd_alarm.esc_mod_over_current = user_indication_message.indication_data.pmd_alarm.esc_mod_over_current;
        indication_parameters.pmd_alarm.esc_mod_over_current = user_indication_message.indication_data.pmd_alarm.esc_mod_over_current;
        indication_parameters.pmd_alarm.esc_mpd_fault = user_indication_message.indication_data.pmd_alarm.esc_mpd_fault;
        indication_parameters.pmd_alarm.esc_eye_safety = user_indication_message.indication_data.pmd_alarm.esc_eye_safety;
        break;
    default:
        BDMF_TRACE_ERR("Unknown indication (%u)\n", message_id);
        return;
    }

    /* initiate user indication callback, if exists */
    if (gpon->user_ic)
        gpon->user_ic(indication_type, indication_parameters);
}

static void _rdpa_sc_sc_callback(uint8_t *buffer, uint8_t *flag)
{
/* Not sure this will be supported in 6858/Maple */
#ifndef CONFIG_BCM_XRDP
#ifndef BDMF_SYSTEM_SIM
    uint8_t  *sc_buffer_ptr;

    sc_buffer_ptr = (uint8_t *)(DEVICE_ADDRESS(RUNNER_PRIVATE_0_OFFSET) + SC_BUFFER_ADDRESS);
    memcpy(buffer, sc_buffer_ptr, 8);
    *flag = *(uint8_t *)(sc_buffer_ptr + 9);
#endif
#endif
}

static void _rdpa_gpon_sr_dba_callback(uint32_t tcont_id, uint32_t *runner_ddr_occupancy)
{
    rdpa_tcont_sr_dba_callback(tcont_id, runner_ddr_occupancy);
}

static int _pon_rx_isr_wrapper(int irq, void *priv)
{
    unsigned long flags;

    pon_sm_cb.pon_mask_rx_pon_interrupts();

    /* schedule a tasklet for farther processing */
    spin_lock_irqsave(&tasklet_lock, flags);
    gpon_tasklet_info.rx_pon_int_source = 1;
    spin_unlock_irqrestore(&tasklet_lock, flags);

    tasklet_hi_schedule(&gpon_tasklet_info.gpon_tasklet);

#ifndef CONFIG_BCM_XRDP
#if !defined(BDMF_SYSTEM_SIM)
    BcmHalInterruptEnable(irq);
#endif
#endif

    return BDMF_IRQ_HANDLED;
}

static int _pon_tx_isr_wrapper(int irq, void *priv)
{
    unsigned long flags;

    pon_sm_cb.pon_mask_tx_pon_interrupts();

    /* schedule a tasklet for farther processing */
    spin_lock_irqsave(&tasklet_lock, flags);
    gpon_tasklet_info.tx_pon_int_source = 1;
    spin_unlock_irqrestore(&tasklet_lock, flags);

    tasklet_hi_schedule(&gpon_tasklet_info.gpon_tasklet);

#ifndef CONFIG_BCM_XRDP
#if !defined(BDMF_SYSTEM_SIM)
    BcmHalInterruptEnable(irq);
#endif
#endif
    return BDMF_IRQ_HANDLED;
}

/* Ploam CPU Rx ISR callback */
void _ploam_isr_callback(void)
{
    unsigned long flags;

    rdpa_cpu_int_disable(rdpa_cpu_host, RDPA_PLOAM_CPU_RX_QUEUE_ID);
    spin_lock_irqsave(&tasklet_lock, flags);
    gpon_tasklet_info.ploam_int_source = 1;
    spin_unlock_irqrestore(&tasklet_lock, flags);
    tasklet_hi_schedule(&gpon_tasklet_info.gpon_tasklet);
}

/* PMD ISR callback */
void _pmd_gpon_isr_wrapper(void)
{
    unsigned long flags;

    /* schedule a tasklet for farther processing */
    spin_lock_irqsave(&tasklet_lock, flags);
    gpon_tasklet_info.pmd_int_source = 1;
    spin_unlock_irqrestore(&tasklet_lock, flags);

    tasklet_hi_schedule(&gpon_tasklet_info.gpon_tasklet);
}
EXPORT_SYMBOL(_pmd_gpon_isr_wrapper);

/* GPON tasklet handler */
static void _rdpa_gpon_tasklet_handler(unsigned long tasklet_arg)
{
    struct rdpa_gpon_tasklet_context tmp_gpon_tasklet;
    unsigned long flags;

    while ((gpon_tasklet_info.rx_pon_int_source == 1) || (gpon_tasklet_info.tx_pon_int_source == 1) ||
          (gpon_tasklet_info.ploam_int_source == 1) || (gpon_tasklet_info.pmd_int_source == 1))
    {
        spin_lock_irqsave(&tasklet_lock, flags);
        memcpy(&tmp_gpon_tasklet, &gpon_tasklet_info, sizeof(tmp_gpon_tasklet));
        gpon_tasklet_info.ploam_int_source = 0;
        gpon_tasklet_info.rx_pon_int_source = 0;
        gpon_tasklet_info.tx_pon_int_source = 0;
        spin_unlock_irqrestore(&tasklet_lock, flags);

        if (tmp_gpon_tasklet.rx_pon_int_source == 1)
            p_isr_pon(rdpa_pon_rx);

        if (tmp_gpon_tasklet.tx_pon_int_source == 1)
            p_isr_pon(rdpa_pon_tx);

        if (tmp_gpon_tasklet.ploam_int_source == 1)
            p_isr_ploam();
    }
}

static int rdpa_gpon_irq_connect(void)
{
    int rc = 0;

    spin_lock_init(&tasklet_lock);

    /* Initialize tasklet */
    tasklet_init(&gpon_tasklet_info.gpon_tasklet, _rdpa_gpon_tasklet_handler, (unsigned long)&gpon_tasklet_info);

    gpon_tasklet_info.ploam_int_source  = 0;
    gpon_tasklet_info.rx_pon_int_source = 0;
    gpon_tasklet_info.tx_pon_int_source = 0;
    gpon_tasklet_info.pmd_int_source    = 0;

#if defined(CONFIG_BCM96858)
    if (stack_mode != GPON_MODE)
    {
      rc = bdmf_int_connect(RDPA_IC_NGPON_RX_IRQ, 0, 0, _pon_rx_isr_wrapper, "pon_rx", NULL);
      if (rc)
          return rc;
      rc = bdmf_int_connect(RDPA_IC_NGPON_TX_1_IRQ, 0, 0, _pon_tx_isr_wrapper, "pon_tx_1", NULL);
      if (rc)
          return rc;
      rc = bdmf_int_connect(RDPA_IC_NGPON_TX_0_IRQ, 0, 0, _pon_tx_isr_wrapper, "pon_tx_0", NULL);
      if (rc)
          return rc;
    }
    else
#endif
    {
      rc = bdmf_int_connect(RDPA_IC_GPON_RX_IRQ, 0, 0, _pon_rx_isr_wrapper, "pon_rx", NULL);
      if (rc)
         return rc;
      rc = bdmf_int_connect(RDPA_IC_GPON_TX_IRQ, 0, 0, _pon_tx_isr_wrapper, "pon_tx", NULL);
      if (rc)
         return rc;
    }
    return 0;
}

static void _pon_rx_packet(rdpa_cpu_rx_info_t *info)
{
    PON_INTERRUPTS_MASK_DTE interrupt_source = {};
    int rc = 0;

    /* initialize interrupt mask */
    interrupt_source.ploam_received_interrupt = 1;
    if (info->reason != rdpa_cpu_rx_reason_oam)
    {
        BDMF_TRACE_ERR("wrong cpu trap reason, should be oam, but reason number is %d\n",
            info->reason);

        rdp_cpu_ring_free_mem(RDPA_PLOAM_CPU_RX_QUEUE_ID, (void*)(info->data));
        return;
    }

    /* initiate the pon isr */
    rc = pon_sm_cb.pon_isr_handler((uint8_t *)info->data + info->data_offset, interrupt_source);
    if (rc)
    {
        BDMF_TRACE_ERR("Ploam rx error.gpon isr fail. gpon_error = %d\n", (int)rc);
    }

    rdp_cpu_ring_free_mem(RDPA_PLOAM_CPU_RX_QUEUE_ID, (void*)(info->data));
}

static void p_isr_ploam(void)
{
    rdpa_cpu_rx_info_t info = {};
    int rc;

    /* clear interrupt in RDD */
    while (1)
    {
        rc = rdp_cpu_ring_get_packet(RDPA_PLOAM_CPU_RX_QUEUE_ID, &info);
        if (rc)
            break;
        _pon_rx_packet(&info);
    }

    rdpa_cpu_int_clear(rdpa_cpu_host, RDPA_PLOAM_CPU_RX_QUEUE_ID);
    rdpa_cpu_int_enable(rdpa_cpu_host, RDPA_PLOAM_CPU_RX_QUEUE_ID);
}

static void p_isr_pon(rdpa_pon_transmit pon_unit)
{
    PON_ERROR_DTE pon_error;
    PON_INTERRUPTS_MASK_DTE  interrupt_source = {};

    /* Tx pon interrupt */
    if (pon_unit == rdpa_pon_tx)
    {
        interrupt_source.txpon_interrupt = 1;
    }
    /* Rx pon interrupt */
    else
    {
        interrupt_source.rxpon_interrupt = 1;
    }

    pon_error = pon_sm_cb.pon_isr_handler(NULL, interrupt_source);
    if (pon_error != PON_NO_ERROR)
    {
        BDMF_TRACE_ERR("gpon isr. pon interrupt error. pon_error=%d\n", (int)pon_error);
    }

    /* Unmask the relevant Gpon interrupts */
    if (pon_unit == rdpa_pon_tx)
    {
       /* Unmask the txpon interrupts */
        pon_error = pon_sm_cb.pon_unmask_tx_pon_interrupts();
        if (pon_error != PON_NO_ERROR)
        {
            BDMF_TRACE_ERR("gpon isr. Unable to unmask Gpon Tx interrupts. pon_error=%d\n", (int)pon_error);
        }
    }
    /* Rx pon interrupt */
    else if (pon_unit == rdpa_pon_rx)
    {
        /* Unmask the rxpon interrupts */
        pon_error = pon_sm_cb.pon_unmask_rx_pon_interrupts();
        if (pon_error != PON_NO_ERROR)
        {
            BDMF_TRACE_ERR("gpon isr. Unable to unmask Gpon Rx interrupts. pon_error=%d\n", (int)pon_error);
         }
    }
}

#endif /*BDMF_SYSTEM_SIM*/

static void default_ic(rdpa_pon_indication indication_type, rdpa_callback_indication indication_parameters)
{
    /* help variables to hold info needed for the log message */
    char *indication  = "Unknown";
    char *fail_reason = "Unknown";
    char *tmp_str1    = "Unknown";
    char *tmp_str2    = "Unknown";
    static char *state_names_for_display[] = {
       "INIT_O1",
       "STANDBY_O2",
       "SERIAL_NUMBER_O3",
       "RANGING_O4",
       "OPERATION_O5",
       "POPUP_O6",
       "EMERGENCY_STOP_O7",
       "DS_TUNING_O8",
       "US_TUNING_O9",
       "SERIAL_NUMBER_O2_3"
    } ;

    static char *sub_state_names_for_display[] = {
      "NONE",
      "STANDBY",
      "OPERATIONAL",
      "STOP",
      "NO_SYNC_(O1)",
      "PROFILE_LEARN_(O1)",
      "TUNED",
      "PENDING",
      "NO_SYNC_(O8)",
      "PROFILE_LEARN_(O8)"
    } ;

    /* determine which indication has been received and issue an appropriate log message */
    /* using the receved indication parameters                                           */
    switch (indication_type)
    {
    case rdpa_indication_lof:
        indication = "LOF alarm (rdpa_indication_lof)";
        BDMF_TRACE_INFO("%s Status = %s\n", indication,
            (indication_parameters.lof_status == rdpa_indication_on ? "ON" : "OFF"));
        break;

    case rdpa_indication_err:
        indication = "ERR alarm (rdpa_indication_err)";
        BDMF_TRACE_INFO("%s Bip8 errors = %lu\n", indication,
            (long unsigned int)indication_parameters.bip8_errors);
        break;

    case rdpa_indication_sf:
        indication = "SF alarm (rdpa_indication_sf)";
        BDMF_TRACE_INFO("%s Status = %s\n", indication,
            (indication_parameters.sf_status == rdpa_indication_on ? "ON" : "OFF"));
        break;
    case rdpa_indication_sd:
        indication = "SD alarm (rdpa_indication_sd)";
        BDMF_TRACE_INFO("%s Status = %s\n", indication,
            (indication_parameters.sd_status == rdpa_indication_on ? "ON" : "OFF"));
        break;

    case rdpa_indication_lcdg:
        indication = "LCDG alarm (rdpa_indication_lcdg)";
        BDMF_TRACE_INFO("%s Status = %s\n", indication,
            (indication_parameters.lcdg_status == rdpa_indication_on ? "ON" : "OFF"));
        break;

    case rdpa_indication_dact:
        indication = "DACT alarm (rdpa_indication_dact)";
        BDMF_TRACE_INFO("%s\n", indication);
        break;

    case rdpa_indication_dis:
        indication = "DIS alarm (rdpa_indication_dis)";
        BDMF_TRACE_INFO("%s Status = %s\n", indication,
            (indication_parameters.dis_status == rdpa_indication_on ? "ON" : "OFF"));
        break;

    case rdpa_indication_pee:
        indication = "PEE alarm (rdpa_indication_pee)";
        BDMF_TRACE_INFO("%s\n", indication);
        break;

    case rdpa_indication_mem:
        indication = "MEM alarm (rdpa_indication_mem)";
        BDMF_TRACE_INFO("%s Message received = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
            indication, indication_parameters.unkown_ploam_mem[0],
            indication_parameters.unkown_ploam_mem[1], indication_parameters.unkown_ploam_mem[2],
            indication_parameters.unkown_ploam_mem[3], indication_parameters.unkown_ploam_mem[4],
            indication_parameters.unkown_ploam_mem[5], indication_parameters.unkown_ploam_mem[6],
            indication_parameters.unkown_ploam_mem[7], indication_parameters.unkown_ploam_mem[8],
            indication_parameters.unkown_ploam_mem[9], indication_parameters.unkown_ploam_mem[10],
            indication_parameters.unkown_ploam_mem[11]);
        break;

    case rdpa_indication_pst_msg:
        indication = "PST alarm (rdpa_indication_pst_msg)";
        BDMF_TRACE_INFO("%s\n", indication);
        break;

    case rdpa_indication_ranging_start:
        indication = "Ranging start (rdpa_indication_ranging_start)";
        BDMF_TRACE_INFO("%s\n", indication);
        break;

    case rdpa_indication_ranging_stop:
        indication = "Ranging stop (rdpa_indication_ranging_stop)";
        switch (indication_parameters.ranging_stop)
        {
        case rdpa_ranging_stop_to1_timeout:
            fail_reason = "TO1 Timeout";
            break;
        case rdpa_ranging_stop_link_folt:
            fail_reason = "Link fault";
            break;
        case rdpa_ranging_stop_deactivate_onu:
            fail_reason = "Deactivate ONU";
            break;
        case rdpa_ranging_stop_disable_onu:
            fail_reason = "Disable ONU";
            break;
        case rdpa_ranging_stop_successful_ranging:
            fail_reason = "Successful ranging";
            break;
        case rdpa_ranging_stop_unmatch_sn:
            fail_reason = "Unmatched serial number";
            break;
        case rdpa_ranging_stop_link_deactivate:
            fail_reason = "Link deactivate";
            break;
        }
        BDMF_TRACE_INFO("%s Reason = %s\n", indication, fail_reason);
        break;
    case rdpa_indication_assign_alloc_id_msg:
        indication = "Assign alloc ID message (rdpa_indication_assign_alloc_id_msg)";
        BDMF_TRACE_INFO("%s Alloc ID = %d, Status = %s\n", indication,
            indication_parameters.assign_alloc_id_parameters.alloc_id,
            (indication_parameters.assign_alloc_id_parameters.status == rdpa_indication_on ?
            "ALLOCATE" : "DE-ALLOCATE"));
        break;

    case rdpa_indication_assign_onu_id_msg:
        indication = "Assign ONU ID message (rdpa_indication_assign_onu_id_msg)";
        BDMF_TRACE_INFO("%s ONU ID = %d\n", indication, indication_parameters.onu_id);
        break;

    case rdpa_indication_cfg_port_id_msg:
        indication = "Configure port ID message (rdpa_indication_cfg_port_id_msg)";
        BDMF_TRACE_INFO("%s Port ID = %d, Status = %s\n", indication,
            indication_parameters.configure_port_id_parameters.port_id,
            (indication_parameters.configure_port_id_parameters.status == rdpa_indication_on ?
            "ASSIGN" : "REMOVE"));
        break;

    case rdpa_indication_encrypted_port_id_msg:
        indication = "Encrypted port ID message (rdpa_indication_encrypted_port_id_msg)";
        BDMF_TRACE_INFO("%s Port ID = %d, Encryption = %s\n", indication,
            indication_parameters.encrypted_port_id_parameters.port_id,
            (indication_parameters.encrypted_port_id_parameters.encryption == 0 ? "DISABLE" : "ENABLE"));
        break;

    case rdpa_indication_key_switched_out_of_sync:
        indication = "Key switch out of sync (rdpa_indication_key_switched_out_of_sync)";
        BDMF_TRACE_INFO("%s\n", indication);
        break;

    case rdpa_indication_key_switched:
        indication = "Key switch (rdpa_indication_key_switched)";
        BDMF_TRACE_INFO("%s, Current superframe = %ld, Switch over superframe = %ld\n", indication,
            (long int)indication_parameters.key_switch_parameters.current_superframe,
            (long int)indication_parameters.key_switch_parameters.switch_over_superframe);
        break;

    case rdpa_indication_state_transition:
        indication = "State transition (rdpa_indication_state_transition)";
        if (indication_parameters.state_transition_parameters.old_state < rdpa_pon_o8_profile_learn_sub_state)
        {
          tmp_str1 = sub_state_names_for_display[indication_parameters.state_transition_parameters.old_state];
        }
        else
        {
          tmp_str1 = "NONE" ;
        }

        if (indication_parameters.state_transition_parameters.new_state < rdpa_pon_o8_profile_learn_sub_state)
        {
          tmp_str2 = sub_state_names_for_display[indication_parameters.state_transition_parameters.new_state];
        }
        else
        {
          tmp_str2 = "NONE" ;
        }

        BDMF_TRACE_INFO("%s, old state = %s, new state = %s\n", indication, tmp_str1, tmp_str2);
        break;

    case rdpa_indication_link_state_transition:
        indication = "Link State transition (rdpa_indication_link_state_transition)";
        tmp_str1 = state_names_for_display[indication_parameters.link_state_transition_parameters.old_state];
        tmp_str2 = state_names_for_display[indication_parameters.link_state_transition_parameters.new_state];

        BDMF_TRACE_INFO("%s, old state = %s, new state = %s\n", indication, tmp_str1, tmp_str2);
        break;

    case rdpa_indication_rouge_onu:
        indication = "ROGUE_ONU alarm (rdpa_indication_rouge_onu)";
        BDMF_TRACE_INFO("%s Status = %s Type = %s\n", indication,
            (indication_parameters.rogue_onu.status == 1 ? "ON" : "OFF"),
            (indication_parameters.rogue_onu.type == rdpa_monitor_rouge_mode ?
            "rdpa_monitor_rouge_mode alarm" : "rdpa_fault_rouge_mode alarm"));
        break;

    case rdpa_indication_pmd_alarm:
        if (indication_parameters.pmd_alarm.esc_be)
        {
            indication = "PMD_ESC_BE alarm (rdpa_indication_pmd_alarm)";
            BDMF_TRACE_INFO("%s\n", indication);
        }

        if (indication_parameters.pmd_alarm.esc_rogue)
        {
            indication = "PMD_ESC_ROGUE_ALARM alarm (rdpa_indication_pmd_alarm)";
            BDMF_TRACE_INFO("%s\n", indication);
        }

        if (indication_parameters.pmd_alarm.esc_mod_over_current)
        {
            indication = "PMD_ESC_MOD_OVR_CRNT alarm (rdpa_indication_pmd_alarm)";
            BDMF_TRACE_INFO("%s\n", indication);
        }
        if (indication_parameters.pmd_alarm.esc_bias_over_current)
        {
            indication = "PMD_ESC_BIAS_OVR_CRNT alarm (rdpa_indication_pmd_alarm)";
            BDMF_TRACE_INFO("%s\n", indication);
        }

        if (indication_parameters.pmd_alarm.esc_mpd_fault)
        {
            indication = "PMD_ESC_ALARM_MPD_FAULT alarm (rdpa_indication_pmd_alarm)";
            BDMF_TRACE_INFO("%s\n", indication);
        }

        if (indication_parameters.pmd_alarm.esc_eye_safety)
        {
            indication = "PMD_ESC_ALARM_EYESAFE alarm (rdpa_indication_pmd_alarm)";
            BDMF_TRACE_INFO("%s\n", indication);
        }
        break;

    default:
        BDMF_TRACE_INFO("Unknown indication (%d)\n", indication_type);
        break;
    }
}

#ifndef BDMF_SYSTEM_SIM
static bdmf_boolean rdpa_gem_is_ds_cfg_set(bdmf_index index)
{
    int rc;
    bool filter_status;
    PON_PORT_ID_DTE port_id_filter;
    PON_PORT_ID_DTE port_id_mask;
    bool encryption_mode;
    PON_FLOW_TYPE_DTE flow_type;
    PON_FLOW_PRIORITY_DTE flow_priority;
    bool crc;

    rc = pon_sm_cb.pon_get_port_id_configuration((PON_FLOW_ID_DTE)index, &filter_status,
        &port_id_filter, &port_id_mask, &encryption_mode, &flow_type, &flow_priority, &crc);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Can't read port id configuration\n");

    return (port_id_filter == GPON_PORT_ID_UNASSIGNED ? 0 : 1);
}

static bdmf_boolean rdpa_gpon_is_alloc_exist(bdmf_index index)
{
    int rc;
    PON_TCONT_DTE tcont_cfg;

    rc = pon_sm_cb.pon_get_tcont_configuration(gpon_tcont_to_rdd_wan_channel(index),
        &tcont_cfg);

    return (rc == PON_ERROR_TCONT_NOT_ASSIGNED ? 0 : 1);
}
#endif

#ifdef USE_BDMF_SHELL
static void register_gpon_shell_commands(void)
{
    bdmfmon_handle_t driver_dir = bdmfmon_dir_find(NULL, "driver");

    if (!driver_dir)
    {
        bdmf_session_print(NULL, "Can't find driver directory\n");
        return;
    }

    pon_sm_cb.initialize_drv_pon_stack_shell(driver_dir);
}

static void unregister_gpon_shell_commands(void)
{
    bdmfmon_handle_t driver_dir = bdmfmon_dir_find(NULL, "driver");

    if (!driver_dir)
       return;

    pon_sm_cb.exit_drv_pon_stack_shell();
}
#endif

static struct bdmf_aggr_type *gpon_aggregates[] =
{
    &gpon_link_state_type,
    &gpon_link_cfg_type,
    &onu_sn_type,
    &gpon_link_stat_type,
    &gpon_overhead_cfg_type,
    &fec_stat_type,
    &pst_params_type,
    &send_ploam_type,
    &rogue_type,
    &misc_tx_type,
    &gem_ds_cfg_type,
    &tcont_stat_type,
    &pon_id_param,
    &mcast_enc_key_param,
    &bw_record_cfg,
    &bw_record_result_gpon,
    &bw_record_result_ngpon,
    &burst_prof,
};

int rdpa_gpon_declare_and_register_to_bdmf(void)
{
    int rc, i;

    /* Initialize BDMF aggregates */
    for (i = 0; i < sizeof(gpon_aggregates) / sizeof(gpon_aggregates[0]); i++)
    {
        if (!gpon_aggregates[i])
            continue;

        rc = bdmf_attr_aggregate_type_register(gpon_aggregates[i]);
        if (rc < 0)
        {
            BDMF_TRACE_RET(BDMF_ERR_INTERNAL,
                "GPON Aggregates registration to BDMF failed: rc=%d", rc);
        }
    }

    rc = bdmf_type_register(&gpon_drv);
    if (rc < 0)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "GPON object registration to BDMF failed: rc=%d", rc);

    return 0;
}

void rdpa_gpon_bdmf_unregister(void)
{
    int i;

    for (i = 0; i < sizeof(gpon_aggregates) / sizeof(gpon_aggregates[0]); i++)
    {
        if (!gpon_aggregates[i])
            continue;

        bdmf_attr_aggregate_type_unregister(gpon_aggregates[i]);
    }

    bdmf_type_unregister(&gpon_drv);
}

#define RATE_STR_LEN 2
/* Module init stack initialization */
int rdpa_gpon_init_stack(char *wan_type, char *wan_rate)
{
#ifndef BDMF_SYSTEM_SIM
    init_pon_module(&pon_sm_cb);

/* since the corresponding  RDPA_WAN_TYPE_VALUE_XXX are defined only in 6858's board.h */
#ifdef CONFIG_BCM_XRDP
    if (!strcmp (wan_type, RDPA_WAN_TYPE_VALUE_GPON))
    {
       stack_mode = GPON_MODE;
    }
    else if (!strcmp (wan_type, RDPA_WAN_TYPE_VALUE_XGPON1))
    {
       stack_mode = NGPON_MODE_XGPON;
    }
    else if (!strcmp (wan_type, RDPA_WAN_TYPE_VALUE_XGS))
    {
       stack_mode = NGPON_MODE_XGS;
    }
    else if (!strcmp (wan_type, RDPA_WAN_TYPE_VALUE_NGPON2))
    {
      if (!strncmp(wan_rate+RATE_STR_LEN,  RDPA_WAN_RATE_10G, RATE_STR_LEN))   /* Upstream - 10Gbps */
      {
         stack_mode = NGPON_MODE_NGPON2_10G;
      }
      else
      {
         stack_mode = NGPON_MODE_NGPON2_2_5G;
      }
    }
#else
    stack_mode = GPON_MODE;
#endif
    pon_sm_cb.set_stack_mode (stack_mode);
#endif

#ifdef BDMF_DRIVER_GPL_LAYER
#ifdef CONFIG_BCM_GPON_TODD
    f_gpon_todd_get_tod_info = pon_sm_cb.pon_todd_get_tod_info;
    f_gpon_todd_set_tod_info = pon_sm_cb.pon_todd_set_tod_info;
    f_gpon_todd_get_tod = pon_sm_cb.pon_todd_get_tod;
    f_gpon_todd_reg_1pps_start_cb = pon_sm_cb.pon_todd_reg_1pps_start_cb;
    f_gpon_tod_get_sfc = pon_sm_cb.pon_tod_get_sfc;
#endif
#endif

    return 0;
}

void rdpa_gpon_exit_stack(void)
{
#ifdef BDMF_DRIVER_GPL_LAYER
#ifdef CONFIG_BCM_GPON_TODD
    f_gpon_todd_get_tod_info = NULL;
    f_gpon_todd_get_tod = NULL;
    f_gpon_todd_reg_1pps_start_cb = NULL;
    f_gpon_todd_set_tod_info = NULL;
    f_gpon_tod_get_sfc = NULL;
#endif
#endif
}

uint32_t rdpa_calc_ds_omci_crc_or_mic (uint8_t *buffer, uint32_t length)
{
#ifndef BDMF_SYSTEM_SIM
  return pon_sm_cb.calc_ds_omci_crc_or_mic (buffer, length);
#else
  return 0;
#endif
}

uint32_t rdpa_calc_us_omci_crc_or_mic (uint8_t *buffer, uint32_t length)
{
#ifndef BDMF_SYSTEM_SIM
  return pon_sm_cb.calc_us_omci_crc_or_mic (buffer, length);
#else
  return 0;
#endif
}

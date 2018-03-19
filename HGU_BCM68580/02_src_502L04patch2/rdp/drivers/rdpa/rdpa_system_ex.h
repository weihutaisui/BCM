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

#ifndef _RDPA_SYSTEM_EX_H_
#define _RDPA_SYSTEM_EX_H_

/* system object private data */
typedef struct
{
    rdpa_sw_version_t sw_version;
    rdpa_system_init_cfg_t init_cfg;
    rdpa_system_cfg_t cfg;
    rdpa_qm_cfg_t qm_cfg;
    uint16_t dp_bitmask[2];
    rdpa_tpid_detect_cfg_t tpids_detect[rdpa_tpid_detect__num_of];
} system_drv_priv_t;

int system_attr_tod_read(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    void *val, uint32_t size);
int system_attr_dp_get_next(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index *index);
int system_attr_dp_read(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    void *val, uint32_t size);
int system_attr_dp_write(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    const void *val, uint32_t size);
int system_attr_stat_read(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index, 
    void *val, uint32_t size);

int system_pre_init_ex(struct bdmf_object *mo);
int system_post_init_ex(struct bdmf_object *mo);
int system_attr_cfg_write_ex(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index, 
    const void *val, uint32_t size);
int system_attr_clock_gate_write(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    const void *val, uint32_t size);
int system_attr_clock_gate_read(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    void *val, uint32_t size);

int system_data_path_init(void);
#if defined(__OREN__) || defined(XRDP)
int system_data_path_init_fiber(rdpa_wan_type wan_type);
#endif
#ifdef XRDP
int system_data_path_init_gbe(rdpa_emac wan_emac);
#endif

#ifdef BCM63158
int system_data_path_init_dsl(void);
#endif

int _tpid_detect_cfg(struct bdmf_object * const mo, rdpa_tpid_detect_t tpid_detect,
    const rdpa_tpid_detect_cfg_t * const tpid_detect_cfg);
int system_post_init_enumerate_emacs(struct bdmf_object *mo);

/** System-level drop statistics. */
#ifdef XRDP

typedef struct
{
    uint16_t empty;
    /* TODO */
} rdpa_system_us_stat_t;

typedef struct
{
    uint16_t connection_action;                 /**< Connection action == drop */
    uint16_t cpu_rx_ring_congestion;            /**< CPU RX Ring congestion */
    uint16_t cpu_recycle_ring_congestion;       /**< CPU Recycle Ring congestion */
    uint16_t cpu_rx_psram_drop;                 /**< Drop due to PSRAM pasring error in CPU RX path */
    uint16_t iptv_hash_lkp_miss_drop;           /**< Drop due to IPTV Hash lookup miss (DA \ DIP) */
    uint16_t iptv_sip_vid_lkp_miss_drop;        /**< Drop due to IPTV channel SIP \ VID lookup miss */
    uint16_t iptv_invalid_ctx_entry_drop;       /**< Drop due to IPTV channel invalid ctx entry */
    uint16_t cpu_tx_copy_no_fpm;                /**< Drop due to no FPM when CPU TX copy */
    uint16_t cpu_tx_copy_no_sbpm;               /**< Drop due to no SBPM when CPU TX copy */
    uint16_t flow_drop_action;                  /**< Flow action = drop */
    uint16_t iptv_first_repl_disp_nack_drop;    /**< Drop due to unavilable dispatcher buffer - first replication */
    uint16_t iptv_other_repl_disp_nack_drop;    /**< Drop due to unavilable dispatcher buffer - other replication */
    uint16_t iptv_fpm_nack_drop;                /**< Drop due to exhaustion of FPM buffers */
    uint16_t rx_mirror_cpu_mcast_exception;     /**< Drop due to RX mirroring or CPU/WLAN Mcast exception */
    uint16_t ingress_resources_congestion;      /**< Drop due to XRDP resources congestion ingress*/
    uint16_t egress_resources_congestion;       /**< Drop due to XRDP resources congestion egress*/
    uint16_t disabled_tx_flow;                  /**< tx flow is not defined */
} rdpa_system_ds_stat_t;

#else

/** US system-level drop statistics. */
typedef struct
{
    uint16_t eth_flow_action;                   /**< Flow action = drop */
    uint16_t sa_lookup_failure;                 /**< SA lookup failure */
    uint16_t da_lookup_failure;                 /**< DA lookup failure */
    uint16_t sa_action;                         /**< SA action == drop */
    uint16_t da_action;                         /**< DA action == drop */
    uint16_t forwarding_matrix_disabled;        /**< Disabled in forwarding matrix */
    uint16_t connection_action;                 /**< Connection action == drop */
    uint16_t parsing_exception;                 /**< Parsing exception */
    uint16_t parsing_error;                     /**< Parsing error */
    uint16_t local_switching_congestion;        /**< Local switching congestion */
    uint16_t vlan_switching;                    /**< VLAN switching */
    uint16_t tpid_detect;                       /**< Invalid TPID */
    uint16_t invalid_subnet_ip;                 /**< Invalid subnet */
    uint16_t acl_oui;                           /**< Dropped by OUI ACL */
    uint16_t acl;                               /**< Dropped by ACL */
} rdpa_system_us_stat_t;

/** DS system-level drop statistics */
typedef struct
{
    uint16_t eth_flow_action;                   /**< Flow action = drop */
    uint16_t sa_lookup_failure;                 /**< SA lookup failure */
    uint16_t da_lookup_failure;                 /**< DA lookup failure */
    uint16_t sa_action;                         /**< SA action == drop */
    uint16_t da_action;                         /**< DA action == drop */
    uint16_t forwarding_matrix_disabled;        /**< Disabled in forwarding matrix */
    uint16_t connection_action;                 /**< Connection action == drop */
    uint16_t policer;                           /**< Dropped by policer */
    uint16_t parsing_exception;                 /**< Parsing exception */
    uint16_t parsing_error;                     /**< Parsing error */
    uint16_t iptv_layer3;                       /**< IPTV L3 drop */
    uint16_t vlan_switching;                    /**< VLAN switching */
    uint16_t tpid_detect;                       /**< Invalid TPID */
    uint16_t dual_stack_lite_congestion;        /**< DSLite congestion */
    uint16_t invalid_subnet_ip;                 /**< Invalid subnet */
    uint16_t invalid_layer2_protocol;           /**< Invalid L2 protocol */
    uint16_t firewall;                          /**< Dropped by firewall */
    uint16_t dst_mac_non_router;                /**< DST MAC is not equal to the router's MAC */
} rdpa_system_ds_stat_t;
#endif /* !XRDP */

typedef struct
{
    rdpa_system_us_stat_t us;                   /**< Upstream drop statistics */
    rdpa_system_ds_stat_t ds;                   /**< Downstream drop statistics */
} rdpa_system_stat_t;

int system_attr_cpu_reason_to_tc_read_ex(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    void *val, uint32_t size);
int system_attr_cpu_reason_to_tc_write_ex(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    const void *val, uint32_t size);

#endif


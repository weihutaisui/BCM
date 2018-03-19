/******************************************************************************
 *
 * Copyright (c) 2009   Broadcom Corporation
 * All Rights Reserved
 *
 * <:label-BRCM:2009:proprietary:standard
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
 *
 ************************************************************************/

/***************************************************************************
 *
 *    Copyright (c) 2008-2009, Broadcom Corporation
 *    All Rights Reserved
 *    Confidential Property of Broadcom Corporation
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * Description: MoCA userland daemon
 *
 ***************************************************************************/

#define MOCAD_DEFTRAP_LEN  (8*1024)

#define MPS_PUB_KEYLEN 65


struct moca_pqos_table {
   macaddr_t               flow_id;
   macaddr_t               packet_da;
   macaddr_t               talker;
   uint32_t                lease_time;
   uint32_t                vlan_prio;
   uint32_t                vlan_id;
   uint32_t                dscp_moca;
   uint32_t                egress_node_mask;
   uint32_t                create_time;
   uint32_t                ingr_class_rule;
} __attribute__((packed,aligned(4)));


// This structure is used to implement the work-around for 
// incompatible bonded channel seed values. 
#define MOCAD_NUM_BONDED_SEED_RECOVERY_ERRORS 10
#define MOCAD_BONDED_SEED_RECOVERY_INTERVAL   (300)  // in seconds
struct mocad_bonded_seed_recovery {
   int32_t  error_code;
   uint32_t error_limit;
   uint32_t error_count;
};


struct mocad_ctx {
   uint32_t    useMocadHook;   // This needs to be the first item in the struct
   char        ifname[MoCAOS_IFNAMSIZE];
   MoCAOS_Handle os_handle;
   MoCAOS_DrvInfo kdrv_info;
   uint32_t    moca_sw_version_major;
   uint32_t    moca_sw_version_minor;
   uint32_t    moca_sw_version_rev;
   int         moca_running;
   int         restart;
   int         exit_on_mmp_mismatch;

   uint32_t    config_flags;
   struct moca_any_time any_time;
   struct moca_max_constellation max_constellation[MOCA_MAX_NODES];
   struct moca_egr_mc_addr_filter egr_mc_addr_filter[MOCA_MAX_EGR_MC_FILTERS];

   uint32_t    cof;
   int         disk_lof;
   int         disk_nondefseqnum;
   int         link_state;
   unsigned int core_uptime;   /* last time core came up */
   unsigned int core_downtime; /* last time core went down */
   unsigned int link_uptime;   /* last time link came up */
   unsigned int link_downtime; /* last time link went down */

   uint8_t        sock_in[MOCA_BUF_LEN];
   uint8_t        sock_out[MOCA_BUF_LEN];
   uint8_t        drv_out[MOCA_BUF_LEN];
   uint8_t        drv_in[MOCA_BUF_LEN];
   uint8_t        trap_buf[MOCA_BUF_LEN];
   uint32_t       trap_count;
   uint32_t       trap_watermark;

   char          *fw_file;
   void          *fw_img;
   unsigned int   fw_len;
   unsigned char *cpu_img[2];
   unsigned int   cpu_img_len[2];

   int         verbose;
   int         show_lab_printf;
   int         in_lab_printf;
   char        core_print_prefix[MoCAOS_IFNAMSIZE+4];
   int         lab_printf_wdog_count;
   struct moca_error_to_mask error_to_mask;

   /* local counters */
   uint64_t                   in_octets;
   uint64_t                   out_octets;
   struct moca_node_stats_ext last_node_stats_ext[MOCA_MAX_NODES];
   struct moca_gen_stats      gen_stats_base;
   struct moca_node_stats     node_stats_base[MOCA_MAX_NODES];
   struct moca_node_stats_ext node_stats_ext_base[MOCA_MAX_NODES];

   struct moca_pqos_table pqos_table[MOCA_MAX_PQOS_ENTRIES];
   uint32_t    pqos_time_wrap_flags;  /* Used to indicate whether or not a PQOS flow expires following a wrap of the timer */
#ifndef STANDALONE
   uint32_t       *cir_data[MOCA_MAX_NODES];
#endif
   struct moca_key_times key_times;   
   uint32_t     active_nodes;

#if defined(DSL_MOCA)
   void        *cmsMsgHandle;
   int          numUcFwdEntries;
   struct moca_uc_fwd ucFwdTable[128]; /* 128 matches MoCA_MAX_UC_FWD_ENTRIES */
#endif
   uint32_t    silentlyDropTraps;
   char *workdir;
   char *pidfile;
   uint32_t    sizeofcpu0strings; 
   unsigned char *cpu0strings;
   uint32_t    sizeofcpu1strings; 
   unsigned char *cpu1strings;
   uint32_t    sizeofcpu0lookup; 
   uint32_t *cpu0lookup;
   uint32_t    sizeofcpu1lookup; 
   uint32_t *cpu1lookup;

   struct {
      struct mmp_msg_hdr       hdr;
      union {
         struct moca_pqos_create_flow_out pqosc_rsp;
         struct moca_pqos_update_flow_out pqosu_rsp;
         struct moca_pqos_delete_flow_out pqosd_rsp;
         struct moca_pqos_query_out       pqosq_rsp;
         struct moca_pqos_list_out        pqosl_rsp;
         struct moca_pqos_status_out      pqoss_rsp;
         struct moca_dd_init_out          dd_rsp;
         struct moca_moca_reset_out       mr_rsp;
         uint32_t                         hostless_mode_rsp;
         uint32_t                         wakeup_node_rsp;
         uint32_t                         ps_cmd_rsp;
         uint32_t                         new_rf_band;
      } payload;
   } __attribute__((packed)) mocad_trap;

   // Not part of union because a response might require multiple traps from FW
   struct {  
      struct mmp_msg_hdr       hdr;
      struct moca_fmr_init_out fmr_rsp;    
   } __attribute__((packed)) fmr_trap;

   // Not part of union because a response might require multiple traps from FW
   struct {
      struct mmp_msg_hdr       hdr;
      struct moca_fmr_20_out   fmr20_rsp;
   } __attribute__((packed)) fmr20_trap;

   // Not part of union because a response might require multiple traps from FW
   struct {
      struct mmp_msg_hdr       hdr;
      struct moca_aca_out      aca_rsp;
   } __attribute__((packed)) aca_trap;

   uint32_t     ps_capabilities;
   int32_t      last_ps_event_code;
   uint32_t     fmr_count;
   uint32_t     fmr_rem_mask;
   uint32_t     fmr20_count;
   uint32_t     fmr20_rem_mask;
   uint32_t     aca_count;
   uint32_t     do_flags;
   uint32_t     hostless_mode;
   unsigned int pqosc_time;
   unsigned int pqosu_time;
   unsigned int pqosd_time;
   unsigned int pqosq_time;
   unsigned int pqosl_time;
   unsigned int pqoss_time;
   unsigned int dd_time;
   unsigned int fmr_time;
   unsigned int fmr20_time;
   unsigned int ps_cmd_time;
   unsigned int mr_time;
   unsigned int aca_time;
   unsigned int cpu_check_sec;
   unsigned int phy_clk_rate;
   unsigned int cpu_clk_rate;
   macaddr_t ecl_mcfilter[MOCA_MAX_ECL_MCFILTERS];

   uint8_t     deferred_traps[MOCAD_DEFTRAP_LEN];
   uint8_t     *deferred_start;
   uint8_t     *deferred_end;

   struct mocad_pre_init_config preconfig;

   struct moca_rf_calib_data rf_calib_data;
   struct moca_probe_2_results probe_2_results;
   struct moca_fw_version fw_version;

   struct mocad_timer *timer_head;

   // MPS
   unsigned char pub_key[MPS_PUB_KEYLEN];
   void *priv_key;
   void *key_ref;
   // MPS State: 0 - Unpaired, 1 - Paired, 2 - Reset, i.e. Unpaired and should use Defaults in moca wait
   uint8_t mps_state : 2;
   uint8_t privacy_defaults : 1;
   uint8_t privacy_changed;
   uint8_t mps_init_scan_tot_nets;
   uint8_t mps_init_scan_trig_nets;
   struct mocad_timer mps_timer;
   struct mocad_timer mps_unpaired_timer;
   struct mocad_timer mps_gen_key_timer;

   uint32_t reset_count;           // Number of times the MoCA firmware has been restarted since the last boot
   uint32_t link_up_count;         // Number of times the MoCA link has gone up since the last boot
   uint32_t link_down_count;       // Number of times the MoCA link has gone down since the last boot
   uint32_t topology_change_count; // Number of times the MoCA network topology has changed since the last link up event
   uint32_t assert_count;          // Number of times the MoCA firmware has asserted since the last boot
   uint32_t last_assert_num;       // This indicates the last assertion number or 0 if there have been no assertions since the last boot
   uint32_t wdog_count;            // Number of times the MoCA firmware has undergone a watchdog reset since the last boot
   uint32_t restart_history;       // Last 4 restart reasons, one byte per restart

   struct mocad_bonded_seed_recovery bonded_seed_recovery_table[MOCAD_NUM_BONDED_SEED_RECOVERY_ERRORS];
   uint32_t preconfig_seed_bonding;
   uint32_t last_bonded_seed_error_time;
   struct moca_last_mr_events last_mr_events;
};


#define L_DEBUG         (1 << 0)
#define L_VERBOSE       (1 << 1)
#define L_INFO          (1 << 2)
#define L_WARN          (1 << 3)
#define L_ERR           (1 << 4)
#define L_TRAP          (1 << 5)
#define L_MMP_MSG       (1 << 6)
#define L_RTT           (1 << 7)
#define L_CORE_WARN     (1 << 8)
#define L_POWER_STATE   (1 << 9)
#define L_RTT_FILE      (1 << 10)
#define L_PRINT_TRAP    (1 << 11)

#define L_CORE_TRACE    (1 << 29)
#define L_ALWAYS_NO_PFX (1 << 30)
#define L_ALWAYS        (1 << 31)

#define LINK_STATE_DOWN       0
#define LINK_STATE_UP         1
#define LINK_STATE_DEBUG      -1

#define PRIVACY_CHANGED_MANUAL 0x1
#define PRIVACY_CHANGED_MPS    0x2

void mocad_log(struct mocad_ctx *ctx, int lev, const char *fmt,...);

int mocad_wr_result(void *wr, uint32_t msg_type, uint32_t ie_type, uint32_t retcode);

int mocad_set_rf_band(void * vctx, uint32_t band);
int mocad_set_rlapm_en(void * vctx, uint32_t bool_val);
int mocad_set_rlapm_table_50(void * vctx, struct moca_rlapm_table_50 * out);
int mocad_set_rlapm_table_100(void * vctx, struct moca_rlapm_table_100 * out);
int mocad_set_rlapm_cap_50(void * vctx, uint32_t cap);
int mocad_set_rlapm_cap_100(void * vctx, uint32_t cap);
int mocad_set_moca_core_trace_enable(void *vctx, uint32_t bool_val);
int mocad_set_lof(void * vctx, uint32_t val);
int mocad_set_snr_margin_rs(void * vctx, struct moca_snr_margin_rs * in);
int mocad_set_snr_margin_ldpc(void * vctx, struct moca_snr_margin_ldpc * in);
int mocad_set_snr_margin_ldpc_pri_ch(void * vctx, struct moca_snr_margin_ldpc_pri_ch * in);
int mocad_set_snr_margin_ldpc_sec_ch(void * vctx, struct moca_snr_margin_ldpc_sec_ch * in);
int mocad_set_snr_margin_ldpc_pre5(void * vctx, struct moca_snr_margin_ldpc_pre5 * in);
int mocad_set_snr_margin_pre5_pri_ch(void * vctx, struct moca_snr_margin_pre5_pri_ch * in); 
int mocad_set_snr_margin_pre5_sec_ch(void * vctx, struct moca_snr_margin_pre5_sec_ch * in); 
int mocad_set_snr_margin_ofdma(void * vctx, struct moca_snr_margin_ofdma * in);
int mocad_set_primary_ch_offset(void * vctx, int32_t offset);
int mocad_set_secondary_ch_offset(void * vctx, int32_t offset);
int mocad_set_sapm_table_50(void * vctx, struct moca_sapm_table_50 * table);
int mocad_set_sapm_table_100(void * vctx, struct moca_sapm_table_100 * table);
int mocad_set_sapm_table_sec(void * vctx, struct moca_sapm_table_sec * table);
int mocad_set_network_password(void * vctx, struct moca_network_password * password);
int mocad_set_loopback_en(void * vctx, uint32_t enable);
int mocad_wr_get_response(void *wr, uint32_t msg_type, uint32_t ie_type, uint32_t ie_len);
int mocad_set_mr_seq_num(void * vctx, uint32_t mr_seq_num);
int mocad_set_brcmtag_enable(void * vctx, uint32_t enable);
int mocad_set_const_tx_params(void * vctx, struct moca_const_tx_params * params);
int mocad_set_nv_cal_enable(void * vctx, uint32_t nv_cal_enable);
int mocad_set_led_mode(void * vctx, uint32_t led_mode);

void mocad_random_password(unsigned char *out);
int mocad_validate_password(const unsigned char *in);
void mocad_set_anytime_network_password(struct mocad_ctx *ctx, const unsigned char *password);
int mocad_set_keys(void * vctx, struct moca_network_password * password);
int mocad_set_privacy_defaults(struct mocad_ctx *ctx);
int mocad_change_privacy_defaults(struct mocad_ctx *ctx, uint32_t new_val);

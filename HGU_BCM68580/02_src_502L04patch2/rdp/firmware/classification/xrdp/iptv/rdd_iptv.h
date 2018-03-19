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

#ifndef _RDD_IPTV_H
#define _RDD_IPTV_H

#include "rdd.h"
#include "rdd_common.h"
#include "rdpa_types.h"
#include "rdpa_wlan_mcast.h"

typedef struct iptv_params
{
    uint32_t key_offset;
    uint32_t hash_tbl_idx;;
} iptv_params_t;

typedef struct
{
#if defined(BCM63158)
    RDD_IPTV_GPE_BASED_RESULT_DTS gpe;
    uint8_t replications;
#endif
    uint16_t wlan_mcast_index;
    uint64_t egress_port_vector;
    uint16_t ic_context;
    rdpa_iptv_channel_key_t key;
    rdpa_wlan_mcast_fwd_table_t wlan_mcast_fwd_table;
#define mc_key_vid key.vid
#define mc_key_mac key.mcast_group.mac
#define mc_key_gr_ip key.mcast_group.l3.gr_ip
#define mc_key_src_ip key.mcast_group.l3.src_ip
#define mc_wlan_idx wlan_mcast_index
#define mc_egress_port_vector egress_port_vector
#define mc_ic_context ic_context
} rdd_iptv_entry_t;

#define IPTV_DDR_CTX_TBL (RDD_IPTV_DDR_CONTEXT_TABLE_SIZE * sizeof(RDD_IPTV_DDR_CONTEXT_ENTRY_DTS))

#define IPTV_CHANNEL_INDEX_GET(key_idx, ctx_idx) ((key_idx << RDD_IPTV_DDR_CONTEXT_TABLE_LOG2_SIZE) | (ctx_idx & ((1 << RDD_IPTV_DDR_CONTEXT_TABLE_LOG2_SIZE) - 1)))
#define IPTV_KEY_INDEX_GET(channel_idx)          (channel_idx >> RDD_IPTV_DDR_CONTEXT_TABLE_LOG2_SIZE)
#define IPTV_CTX_INDEX_GET(channel_idx)          (channel_idx & ((1 << RDD_IPTV_DDR_CONTEXT_TABLE_LOG2_SIZE) - 1))

int rdd_iptv_module_init(const rdd_module_t *module);
void rdd_iptv_lkp_method_set(rdpa_iptv_lookup_method method);
int rdd_iptv_ctx_table_ddr_init(uint16_t fpm_min_pool_size);
void rdd_iptv_ctx_table_ddr_destroy(void);
void rdd_iptv_ddr_context_entry_get(RDD_IPTV_DDR_CONTEXT_ENTRY_DTS *ddr_ctx_entry, uint32_t entry_idx);

void rdd_iptv_hash_key_entry_compose(rdpa_iptv_channel_key_t *key, RDD_IPTV_HASH_LKP_ENTRY_DTS *hash_key_entry);
void rdd_iptv_hash_key_entry_decompose(RDD_IPTV_HASH_LKP_ENTRY_DTS hash_key_entry, rdpa_iptv_channel_key_t *key);
void rdd_iptv_hash_result_entry_compose(uint32_t ctx_idx, RDD_IPTV_HASH_RESULT_ENTRY_DTS *hash_result_entry);
void rdd_iptv_hash_result_entry_decompose(RDD_IPTV_HASH_RESULT_ENTRY_DTS hash_result_entry, uint32_t *ctx_idx);

/* TODO: move to Rdd_mcast_resolution*/
int rdd_iptv_result_entry_add(rdd_iptv_entry_t *iptv_entry, uint8_t *ic_ctx, uint32_t *head_idx, uint32_t ctr,
    uint32_t *entry_idx);
void rdd_iptv_result_entry_modify(rdd_iptv_entry_t *iptv_entry, uint8_t *ic_ctx, uint32_t entry_idx);
int rdd_iptv_result_entry_get(uint32_t ctx_idx, rdd_iptv_entry_t *rdd_iptv_entry, uint8_t *ic_ctx);
int rdd_iptv_result_entry_find(rdpa_iptv_channel_key_t *key, uint32_t head_idx, uint32_t *entry_idx);
int rdd_iptv_result_entry_delete(uint32_t entry_idx, uint32_t *head_idx, uint32_t *cntr_id);
int rdd_iptv_result_entry_next_idx_get(uint32_t entry_idx, uint32_t *next_idx);
int rdd_iptv_cntr_idx_get(uint32_t entry_idx, uint32_t *cntr_idx);
#endif /* _RDD_IPTV_H */

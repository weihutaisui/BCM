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

#include "rdpa_iptv_ex.h"
#include "rdd_ic_common.h"
#include "rdd_iptv.h"
#include "rdd_data_structures_auto.h"
#include "rdp_drv_rnr.h"
#include "rdp_drv_hash.h"
#include "rdp_drv_qm.h"
#include "rdp_drv_proj_cntr.h"
#include "rdpa_port_int.h"

#define MAC_ADDR_LOW(addr)     ((uint64_t)(addr) & 0xFFFFFFFF)
#define MAC_ADDR_HIGH(addr)    ((uint64_t)(addr) >> 32)
#define IPV4_MCAST_MAC_ADDR_PREFIX  (0x01005E000000)
#define IPV4_MCAST_MAC_ADDR_MASK    (0xFFFFFF800000)
#define IPV6_MCAST_MAC_ADDR_PREFIX  (0x333300000000)
#define IPV6_MCAST_MAC_ADDR_MASK    (0xFFFF00000000)

#if !defined(BCM63158)
extern struct bdmf_object *iptv_object;
#endif

static void mcast_control_ip_filter_set(void)
{
    rnr_quad_parser_ip0 ipv4_filter, ipv6_filter;
    uint32_t quad_idx;

    /* Ipv4 multicast range - 224.0.0.0 to 224.0.0.255, mask = 255.255.255.0.
     * Runner expects to receive in host-order */
    ipv4_filter.ip_address = 0xe0000000;
    ipv4_filter.ip_address_mask = 0xFFFFFF00;
    ipv4_filter.ip_filter0_dip_en = 1;
    ipv4_filter.ip_filter0_valid = 1;

    /* Ipv6 multicast range - FF02:: to FF02:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF,
     * mask = FFFF::. By default, MS DW is used. */
    ipv6_filter.ip_address = 0xff020000;
    ipv6_filter.ip_address_mask = 0xffff0000;
    ipv6_filter.ip_filter0_dip_en = 1;
    ipv6_filter.ip_filter0_valid = 1;

    for (quad_idx = 0; quad_idx < NUM_OF_RNR_QUAD; quad_idx++) 
    {
        ag_drv_rnr_quad_parser_ip0_set(quad_idx, &ipv4_filter);
        ag_drv_rnr_quad_parser_ip1_set(quad_idx, &ipv6_filter);
    }
}

int rdpa_iptv_post_init_ex(void)
{
    uint16_t fpm_min_pool_size;
    bdmf_error_t rc;
#if !defined(BCM63158)
    iptv_drv_priv_t *iptv_cfg = (iptv_drv_priv_t *)bdmf_obj_data(iptv_object);
#endif
    rc = drv_qm_fpm_min_pool_get(&fpm_min_pool_size);
    if (rc)
        BDMF_TRACE_RET(rc, "Failed to get FPM min pool size\n");

    BDMF_TRACE_DBG("fpm min pool size is: %d\n", fpm_min_pool_size);
    rc = rdd_iptv_ctx_table_ddr_init(fpm_min_pool_size);
    if (rc)
        BDMF_TRACE_RET(rc, "Failed to initialize FPM pool\n");
#if !defined(BCM63158)
    rdd_multicast_filter_cfg(iptv_cfg->mcast_prefix_filter);
    rdd_iptv_status_cfg(1);
#endif
    mcast_control_ip_filter_set();

    return 0;
}

void rdpa_iptv_destroy_ex(void)
{
    rdd_iptv_ctx_table_ddr_destroy();
#if !defined(BCM63158)
    rdd_iptv_status_cfg(0);
#endif
}

int rdpa_iptv_cfg_rdd_update_ex(iptv_drv_priv_t *iptv_cfg, iptv_drv_priv_t *new_iptv_cfg, bdmf_boolean post_init)
{
    bdmf_error_t rc = BDMF_ERR_OK;

    /* Change only if not equal or init time */
    if (!rc && (iptv_cfg->lookup_method != new_iptv_cfg->lookup_method || post_init))
        rdd_iptv_lkp_method_set(new_iptv_cfg->lookup_method);

    return rc;
}

int rdpa_iptv_rdd_entry_search_ex(rdpa_iptv_channel_key_t *key, uint32_t *index)
{
    RDD_IPTV_HASH_LKP_ENTRY_DTS hash_key = {}; 
    RDD_IPTV_HASH_RESULT_ENTRY_DTS hash_ctx = {};
    uint32_t ctx_idx = 0;
    uint32_t head_ctx_idx = 0;
    hash_result_t hash_res = {};
    bdmf_error_t rc;
    bdmf_boolean skp;
    uint8_t cfg;

    /* lookup destination ip / mac in hash */
    rdd_iptv_hash_key_entry_compose(key, &hash_key);
    rc = drv_hash_find(HASH_TABLE_IPTV, (uint8_t *)&hash_key, &hash_res);
    if (rc)
        return rc;

    if (hash_res.match == HASH_MISS)
        return BDMF_ERR_NOENT;

    /* extract rdd iptv ctx index from hash result */
    rc = drv_hash_get_context(hash_res.match_index, HASH_TABLE_IPTV, NULL, (uint8_t *)&hash_ctx, &skp, &cfg);
    rdd_iptv_hash_result_entry_decompose(hash_ctx, &head_ctx_idx);
    if (rc)
        return rc;

    /* further lookup source ip and / or vid */
    rc = rdd_iptv_result_entry_find(key, head_ctx_idx, &ctx_idx);
    if (rc != BDMF_ERR_NOENT)
        *index = IPTV_CHANNEL_INDEX_GET(hash_res.match_index, ctx_idx);

    return rc;
}

int _rdpa_iptv_hash_key_get(uint32_t channel_index, rdd_iptv_entry_t *rdd_iptv_entry)
{
    int rc;
    RDD_IPTV_HASH_LKP_ENTRY_DTS hash_key = {};

    /* get dest ip / mac */
    rc = drv_hash_key_get(HASH_TABLE_IPTV, IPTV_KEY_INDEX_GET(channel_index), (uint8_t *)&hash_key);
    if (rc)
        return rc;

    rdd_iptv_hash_key_entry_decompose(hash_key, &rdd_iptv_entry->key);
    return 0;
}

int rdpa_iptv_rdd_entry_get_ex(uint32_t channel_index, rdd_iptv_entry_t *rdd_iptv_entry)
{
    int rc;
#if !defined(BCM63158)
    uint8_t ic_ctx[RDD_IPTV_RULE_BASED_RESULT_RULE_NUMBER] = {};
#else
    uint8_t *ic_ctx = (uint8_t *)&rdd_iptv_entry->gpe;
#endif
    rc = _rdpa_iptv_hash_key_get(channel_index, rdd_iptv_entry);
    if (rc)
        return rc;

    /* get src ip / mac and vid*/
    rc = rdd_iptv_result_entry_get(IPTV_CTX_INDEX_GET((uint32_t)channel_index), rdd_iptv_entry, ic_ctx);
    if (rc)
        return rc;

#if !defined(BCM63158)
    rdd_ic_result_entry_decompose(ic_ctx, &rdd_iptv_entry->ic_context, NULL);
#endif
    return rc;
}

#if !defined(BCM63158)
static void _rdpa_iptv_wlan_to_host_check_modify(rdd_vport_id_t vport_vector, rdpa_forward_action *action)
{
    iptv_drv_priv_t *iptv_cfg = (iptv_drv_priv_t *)bdmf_obj_data(iptv_object);
    uint64_t wlan_ports_mask;
    
    if (!iptv_cfg->wlan_to_host)
        return;

    wlan_ports_mask = (1LL << rdpa_port_rdpa_if_to_vport(rdpa_if_wlan0)) |
        (1LL << rdpa_port_rdpa_if_to_vport(rdpa_if_wlan1)) |
        (1LL << rdpa_port_rdpa_if_to_vport(rdpa_if_wlan2));
    if (vport_vector & wlan_ports_mask)
        *action = rdpa_forward_action_host;
}
#endif

int rdpa_iptv_rdd_entry_add_ex(rdd_iptv_entry_t *rdd_iptv_entry, uint32_t *channel_idx)
{
    uint32_t key_idx, ctx_idx, head_ctx_idx, cntr_id = 0;
    hash_result_t hash_res = {};
#if !defined(BCM63158)
    rdd_ic_context_t rdd_ic_ctx = {};
    uint8_t ic_ctx[RDD_IPTV_RULE_BASED_RESULT_RULE_NUMBER] = {};
#else
    uint8_t *ic_ctx = (uint8_t *)&rdd_iptv_entry->gpe;
#endif
    RDD_IPTV_HASH_LKP_ENTRY_DTS hash_key = {};
    RDD_IPTV_HASH_RESULT_ENTRY_DTS hash_ctx = {};
    bdmf_error_t rc;

    rdpa_cntr_id_alloc(TCAM_IPTV_DEF_CNTR_GROUP_ID, &cntr_id);

#if !defined(BCM63158)
    /* get ic context */
    rc = rdpa_ic_rdd_context_get(rdpa_dir_ds, rdd_iptv_entry->ic_context, &rdd_ic_ctx);
    if (rc)
        goto exit;

    /* Required explicitly for IPTV action vector */
    rdd_ic_ctx.forw_mode = rdpa_forwarding_mode_pkt;

    /* override action if wlan_to_host flag is set and there is wlan port in egress */
    _rdpa_iptv_wlan_to_host_check_modify(rdd_iptv_entry->egress_port_vector, &rdd_ic_ctx.action);

    rdd_ic_result_entry_compose(rdd_iptv_entry->ic_context, &rdd_ic_ctx, ic_ctx);
#endif
    /* lookup destination ip / mac in hash */
    rdd_iptv_hash_key_entry_compose(&rdd_iptv_entry->key, &hash_key);
    rc = drv_hash_find(HASH_TABLE_IPTV, (uint8_t *)&hash_key, &hash_res);
    if (rc)
        goto exit;

    /* check if first entry in linked list */
    if (hash_res.match == HASH_MISS)
    {
        key_idx = hash_res.first_free_idx;
        /* add entry to DDR */
        rc = rdd_iptv_result_entry_add(rdd_iptv_entry, ic_ctx, NULL, cntr_id, &ctx_idx);
        head_ctx_idx = ctx_idx;
        if (rc)
            goto exit;
        /* add hash key + result */
        rdd_iptv_hash_result_entry_compose(ctx_idx, &hash_ctx);
        rc = drv_hash_rule_add(HASH_TABLE_IPTV, (uint8_t *)&hash_key, NULL, (uint8_t *)&hash_ctx);
        if (rc)
        {
            rdd_iptv_result_entry_delete(ctx_idx, &head_ctx_idx, &cntr_id);
            goto exit;
        }
    }
    else
    {
        bdmf_boolean skp;
        uint8_t cfg;

        key_idx = hash_res.match_index;
        rc = drv_hash_get_context(hash_res.match_index, HASH_TABLE_IPTV, NULL, (uint8_t *)&hash_ctx, &skp, &cfg);
        rdd_iptv_hash_result_entry_decompose(hash_ctx, &head_ctx_idx);
        rc = rc ? rc : rdd_iptv_result_entry_add(rdd_iptv_entry, ic_ctx, &head_ctx_idx, cntr_id, &ctx_idx);
        if (rc)
            goto exit;
        /* added index is now the head of the linked list */
        if (head_ctx_idx == ctx_idx)
        {
            rdd_iptv_hash_result_entry_compose(ctx_idx, &hash_ctx);
            rc = drv_hash_modify_context(HASH_TABLE_IPTV, key_idx, NULL, (uint8_t *)&hash_ctx);
        }
    }

exit:
    if (!rc)
        *channel_idx = IPTV_CHANNEL_INDEX_GET(key_idx, ctx_idx);
    else
        rdpa_cntr_id_dealloc(TCAM_IPTV_DEF_CNTR_GROUP_ID, IPTV_CNTR_SUB_GROUP_ID, cntr_id);
    return rc;
}

int rdpa_iptv_result_entry_modify(uint32_t channel_idx, rdd_iptv_entry_t *rdd_iptv_entry)
{
#if !defined(BCM63158)
    uint8_t ic_ctx[RDD_IPTV_RULE_BASED_RESULT_RULE_NUMBER] = {};
    rdd_ic_context_t rdd_ic_ctx = {};
#else
    uint8_t *ic_ctx = (uint8_t *)&rdd_iptv_entry->gpe;
#endif
    bdmf_error_t rc = BDMF_ERR_OK;

#if !defined(BCM63158)
    /* get ic context */
    rc = rdpa_ic_rdd_context_get(rdpa_dir_ds, rdd_iptv_entry->ic_context, &rdd_ic_ctx);
    if (rc)
        return rc;

    /* Required explicitly for IPTV action vector */
    rdd_ic_ctx.forw_mode = rdpa_forwarding_mode_pkt;
    rdd_ic_result_entry_compose(rdd_iptv_entry->ic_context, &rdd_ic_ctx, ic_ctx);
#endif
    rdd_iptv_result_entry_modify(rdd_iptv_entry, ic_ctx, IPTV_CTX_INDEX_GET(channel_idx));

    return rc;
}

int rdpa_iptv_rdd_entry_delete_ex(uint32_t channel_idx)
{
    uint32_t head_ctx_idx, next_ctx_idx, cntr_id = 0;
    RDD_IPTV_HASH_RESULT_ENTRY_DTS hash_ctx = {};
    bdmf_error_t rc;
    bdmf_boolean skp;
    uint8_t cfg;

    /* get head ctx index from hash */
    rc = drv_hash_get_context(IPTV_KEY_INDEX_GET(channel_idx), HASH_TABLE_IPTV, NULL, (uint8_t *)&hash_ctx, &skp, &cfg);
    if (rc)
        return rc;
    rdd_iptv_hash_result_entry_decompose(hash_ctx, &head_ctx_idx);

    /* first element in the DDR ctx list need the update the hash result with the new head ctx idx */
    if (head_ctx_idx == IPTV_CTX_INDEX_GET(channel_idx))
    {
        rc = rdd_iptv_result_entry_next_idx_get(IPTV_CTX_INDEX_GET(channel_idx), &next_ctx_idx);
        if (rc)
            return rc;

        /* no more elements */
        if (next_ctx_idx == IPTV_CTX_ENTRY_IDX_NULL)
            rc = drv_hash_rule_remove_index(HASH_TABLE_IPTV, IPTV_KEY_INDEX_GET(channel_idx));
        else
        {
            rdd_iptv_hash_result_entry_compose(next_ctx_idx, &hash_ctx);
            rc = drv_hash_modify_context(HASH_TABLE_IPTV, IPTV_KEY_INDEX_GET(channel_idx), NULL, (uint8_t *)&hash_ctx);
        }
    }

    rc ? rc : rdd_iptv_result_entry_delete(IPTV_CTX_INDEX_GET(channel_idx), &head_ctx_idx, &cntr_id);
#ifndef RDP_SIM
    if (!rc)
        rdpa_cntr_id_dealloc(TCAM_IPTV_DEF_CNTR_GROUP_ID, IPTV_CNTR_SUB_GROUP_ID, cntr_id);
#endif
    return rc;
}

#if !defined(BCM63158)
int rdpa_iptv_ic_result_add_ex(mcast_result_entry_t *entry)
{
    rdd_ic_context_t ic_ctx = {};
    bdmf_error_t rc;

    rc = rdpa_map_to_rdd_classifier(rdpa_dir_ds, &entry->mcast_result, &ic_ctx, 1, 1, RDPA_IC_TYPE_FLOW, 0);
    return rc ? rc : rdpa_ic_rdd_context_cfg(rdpa_dir_ds, entry->mcast_result_idx, &ic_ctx);
}

void rdpa_iptv_ic_result_delete_ex(uint32_t mcast_result_idx, rdpa_traffic_dir dir)
{
    rdpa_ic_result_delete(mcast_result_idx, dir);
}
#endif

int rdpa_iptv_stat_read_ex(struct bdmf_attr *ad, bdmf_index index, void *val, uint32_t size)
{
    bdmf_error_t rc = BDMF_ERR_OK;
    uint16_t temp_cntr_val[3] = {};
    uint32_t cntr_arr[MAX_NUM_OF_COUNTERS_PER_READ] = {};

    rdpa_iptv_stat_t *stat = (rdpa_iptv_stat_t *)val;


    drv_cntr_counter_read(GENERAL_CNTR_GROUP_ID, GENERAL_COUNTER_RX_IPTV_VALID_PKT, cntr_arr);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "can't read COUNTER_RX_IPTV_VALID_PKT counter, error = %d\n", rc);

    stat->rx_valid_pkt = cntr_arr[0];

    rc = drv_cntr_varios_counter_get(COUNTER_IPTV_HASH_LKP_MISS_DROP, &temp_cntr_val[0]);
    rc = rc ? rc : drv_cntr_varios_counter_get(COUNTER_IPTV_SIP_VID_LKP_MISS_DROP, &temp_cntr_val[1]);
    rc = rc ? rc : drv_cntr_varios_counter_get(COUNTER_IPTV_INVALID_CTX_ENTRY_DROP, &temp_cntr_val[2]);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "can't read IPTV MISS counters, error = %d\n", rc);

    stat->discard_pkt = (uint32_t) temp_cntr_val[0] + (uint32_t) temp_cntr_val[1] + (uint32_t) temp_cntr_val[2];
    stat->rx_crc_error_pkt = 0;

    return rc;
}

/* "stat" attribute "write" callback */
int rdpa_iptv_stat_write_ex(struct bdmf_attr *ad, bdmf_index index, const void *val, uint32_t size)
{
    bdmf_error_t rc = BDMF_ERR_OK;

    /* TODO: implement when counters are ready */
    return rc;
}

bdmf_error_t rdpa_iptv_channel_rdd_pm_stat_get_ex(bdmf_index channel_index, rdpa_stat_t *pm_stat)
{
#ifndef RDP_SIM
    int rc;
    uint32_t cntr_id = 0;
    uint32_t rx_cntr_arr[MAX_NUM_OF_COUNTERS_PER_READ] = {};

    rc = rdd_iptv_cntr_idx_get(IPTV_CTX_INDEX_GET(channel_index), &cntr_id);
    rc = rc ? rc : drv_cntr_counter_read(TCAM_IPTV_DEF_CNTR_GROUP_ID, cntr_id, rx_cntr_arr);
    if (rc)
    {
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Failed to read iptv counter of channel 0x%x, rdd error %d\n",
            (uint32_t)channel_index, rc);
    }
    /* RDD doesn't supply the counter of received bytes, only packets. */
    pm_stat->packets = rx_cntr_arr[0];
    pm_stat->bytes = 0;

#else
    memset(pm_stat, 0, sizeof(rdpa_stat_t));
#endif
    return BDMF_ERR_OK;
}


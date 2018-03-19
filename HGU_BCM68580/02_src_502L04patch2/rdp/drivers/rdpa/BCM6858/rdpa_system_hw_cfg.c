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
#include "rdpa_api.h"
#include "rdpa_int.h"
#include "rdp_common.h"
#include "data_path_init.h"
#include "rdd_runner_defs_auto.h"
#include "rdd_platform.h"
#include "rdp_drv_fpm.h"
#ifndef RDP_SIM
#include "bcm_map_part.h"
#include "bcm_hwdefs.h"
#include "ru_types.h"
#include "board.h"
#include "bcm_misc_hw_init.h"
#include "rdp_drv_rnr.h"
extern const ru_block_rec *RU_ALL_BLOCKS[];
extern unsigned long BBH_TX_ADDRS[];
extern unsigned long BBH_RX_ADDRS[];
#if defined(CONFIG_BCM963158)
extern unsigned long XLIF_RX_IF_ADDRS[];
extern unsigned long XLIF_RX_FLOW_CONTROL_ADDRS[];
extern unsigned long XLIF_TX_IF_ADDRS[];
extern unsigned long XLIF_TX_FLOW_CONTROL_ADDRS[];
extern unsigned long DEBUG_BUS_ADDRS[];
extern unsigned long XLIF_EEE_ADDRS[];
extern unsigned long XLIF_Q_OFF_ADDRS[];
#endif
#endif

dpi_params_t dpi_params = {};

#ifdef RDP_SIM

int rdp_get_init_params(void)
{
    dpi_params.runner_freq = 1U<<9;
    dpi_params.rdp_ddr_pkt_base_virt = bdmf_alloc_rsv(FPM_DDR_SEGMENT_SIZE, NULL);
    dpi_params.rdp_ddr_rnr_tables_base_virt = bdmf_alloc_rsv(NATC_DDR_SEGMENT_SIZE, NULL);
    dpi_params.fpm_buf_size = FPM_BUF_SIZE_DEFAULT;

    return 0;
}
#else
extern int bcm_misc_g9991_debug_port_get(void);
extern uint32_t bcm_misc_g9991_phys_port_vec_get(void);
extern int bcm_misc_xfi_port_get(void);


void remap_ru_block_single(uint32_t block_index, ru_block_rec *ru_block, uint32_t phys_base);
uintptr_t bcm_io_block_virt_base_get(uint32_t block_index, uint32_t phys_base);

void remap_ru_blocks(void)
{
#if defined(CONFIG_BCM96858) || defined(CONFIG_BCM963158)
    bbh_id_e bbh_idx;
#endif
    int i;

    for (i = 0; RU_ALL_BLOCKS[i]; i++)
    {
#if defined(CONFIG_BCM963158)
        /* Skip the WAN blocks */
        if (RU_ALL_BLOCKS[i]->addr == BBH_TX_ADDRS                ||
            RU_ALL_BLOCKS[i]->addr == BBH_RX_ADDRS                ||
            RU_ALL_BLOCKS[i]->addr == XLIF_RX_IF_ADDRS            ||
            RU_ALL_BLOCKS[i]->addr == XLIF_RX_FLOW_CONTROL_ADDRS  ||
            RU_ALL_BLOCKS[i]->addr == XLIF_TX_IF_ADDRS            ||
            RU_ALL_BLOCKS[i]->addr == XLIF_TX_FLOW_CONTROL_ADDRS  ||
            RU_ALL_BLOCKS[i]->addr == DEBUG_BUS_ADDRS             ||
            RU_ALL_BLOCKS[i]->addr == XLIF_EEE_ADDRS              ||
            RU_ALL_BLOCKS[i]->addr == XLIF_Q_OFF_ADDRS)
            continue;
#endif
#if defined(CONFIG_BCM96858)
        /* Skip the WAN blocks */
        if (RU_ALL_BLOCKS[i]->addr == BBH_TX_ADDRS || 
            RU_ALL_BLOCKS[i]->addr == BBH_RX_ADDRS)
            continue;
#endif
        remap_ru_block_single(XRDP_IDX, (ru_block_rec *)RU_ALL_BLOCKS[i], XRDP_PHYS_BASE);
    }
#ifdef CONFIG_BCM96858
    /* go over BBH Tx and BBH Rx and map only XRDP instances */
    for (bbh_idx = BBH_ID_FIRST; bbh_idx < BBH_ID_PON; bbh_idx++)
    {
        BBH_RX_ADDRS[bbh_idx] += bcm_io_block_virt_base_get(XRDP_IDX, XRDP_PHYS_BASE);
        BBH_TX_ADDRS[bbh_idx] += bcm_io_block_virt_base_get(XRDP_IDX, XRDP_PHYS_BASE);
    }
    /* remap BBHWAN */
    BBH_RX_ADDRS[BBH_ID_PON] += bcm_io_block_virt_base_get(WANBBH_IDX, WANBBH_PHYS_BASE);
    BBH_TX_ADDRS[BBH_ID_PON] += bcm_io_block_virt_base_get(WANBBH_IDX, WANBBH_PHYS_BASE);
#endif
#ifdef CONFIG_BCM963158
    /* go over BBH Tx and BBH Rx and map only XRDP instances */
    for (bbh_idx = BBH_ID_0; bbh_idx <= BBH_ID_2; bbh_idx++)
    {
        BBH_RX_ADDRS[bbh_idx] += bcm_io_block_virt_base_get(XRDP_IDX, XRDP_PHYS_BASE);
    }
    BBH_TX_ADDRS[BBH_TX_ID_0] += bcm_io_block_virt_base_get(XRDP_IDX, XRDP_PHYS_BASE);

    /* remap BBHWAN */
    for (bbh_idx = BBH_ID_PON; bbh_idx <= BBH_ID_DSL; bbh_idx++)
    {
        BBH_RX_ADDRS[bbh_idx] += bcm_io_block_virt_base_get(WANBBH_IDX, WANBBH_PHYS_BASE);
    }
    for (bbh_idx = BBH_TX_ID_PON; bbh_idx <= BBH_TX_ID_DSL; bbh_idx++)
    {
        BBH_TX_ADDRS[bbh_idx] += bcm_io_block_virt_base_get(WANBBH_IDX, WANBBH_PHYS_BASE);
    }

    {
        xlif_id_e xlif_idx;
        /* Map XLIF */
        for (xlif_idx = XLIF_ID_CHANNEL_FIRST; xlif_idx < XLIF_ID_CHANNEL_NUM; xlif_idx++)
        {
            XLIF_RX_IF_ADDRS[xlif_idx]            += bcm_io_block_virt_base_get(XLIF_IDX, XLIF_PHYS_BASE);
            XLIF_RX_FLOW_CONTROL_ADDRS[xlif_idx]  += bcm_io_block_virt_base_get(XLIF_IDX, XLIF_PHYS_BASE);
            XLIF_TX_IF_ADDRS[xlif_idx]            += bcm_io_block_virt_base_get(XLIF_IDX, XLIF_PHYS_BASE);
            XLIF_TX_FLOW_CONTROL_ADDRS[xlif_idx]  += bcm_io_block_virt_base_get(XLIF_IDX, XLIF_PHYS_BASE);
            DEBUG_BUS_ADDRS[xlif_idx]             += bcm_io_block_virt_base_get(XLIF_IDX, XLIF_PHYS_BASE);
            XLIF_EEE_ADDRS[xlif_idx]              += bcm_io_block_virt_base_get(XLIF_IDX, XLIF_PHYS_BASE);
            XLIF_Q_OFF_ADDRS[xlif_idx]            += bcm_io_block_virt_base_get(XLIF_IDX, XLIF_PHYS_BASE);
        }
    }
#endif
}

extern unsigned long get_rdp_freq(unsigned int *rdp_freq);

int rdp_get_init_params(void)
{
    uint32_t rnr_size, fpm_pool_size;
    int rc = 0;

    /* fetch the reserved-memory from device tree */
    if (BcmMemReserveGetByName(FPMPOOL_BASE_ADDR_STR, &dpi_params.rdp_ddr_pkt_base_virt, &fpm_pool_size))
    {
        BDMF_TRACE_RET(BDMF_ERR_NOMEM, "Failed to get fpm_pool base address");
    }
    
     
    if (BcmMemReserveGetByName(RNRTBLS_BASE_ADDR_STR, &dpi_params.rdp_ddr_rnr_tables_base_virt, &rnr_size))
    {
        BDMF_TRACE_RET(BDMF_ERR_NOMEM, "Failed to get runner_tables base address");
    }

    /* remap all RU blocks to virtual space */
    remap_ru_blocks();

    if ((FPM_BUF_SIZE_256*FPM_BUF_MAX_BASE_BUFFS) >= RDPA_MTU)
        dpi_params.fpm_buf_size = FPM_BUF_SIZE_256;
    else if ((FPM_BUF_SIZE_512*FPM_BUF_MAX_BASE_BUFFS) >= RDPA_MTU)
        dpi_params.fpm_buf_size = FPM_BUF_SIZE_512;
    else if ((FPM_BUF_SIZE_1K*FPM_BUF_MAX_BASE_BUFFS) >= RDPA_MTU)
        dpi_params.fpm_buf_size = FPM_BUF_SIZE_1K;
    else
        dpi_params.fpm_buf_size = FPM_BUF_SIZE_2K;

    /* non jumbo packets, increase buffer size if more fpm memory available */
    if  (dpi_params.fpm_buf_size == FPM_BUF_SIZE_256)   
    {
        /* for >128M we can allow to increase basic token to 1K */
        if ((fpm_pool_size >> 20) >= 128)
        {
            dpi_params.fpm_buf_size = FPM_BUF_SIZE_2K;
        }
        /* for >64M we can allow to increase basic token to 1K */
        else if ((fpm_pool_size >> 20) >= 64)
        {
            dpi_params.fpm_buf_size = FPM_BUF_SIZE_1K;
        }
        else if ((fpm_pool_size >> 20) >= 32)
        {
            dpi_params.fpm_buf_size = FPM_BUF_SIZE_512;
        }   
    }

    bdmf_trace("fpm base =0x%pK fpm poolsize %d, MTU: %d, token: %d rnr_tables=0x%pK size=%d\n", dpi_params.rdp_ddr_pkt_base_virt, fpm_pool_size, RDPA_MTU,
        dpi_params.fpm_buf_size, dpi_params.rdp_ddr_rnr_tables_base_virt, rnr_size);
    if (fpm_pool_size < dpi_params.fpm_buf_size * TOTAL_FPM_TOKENS)
        BDMF_TRACE_RET(BDMF_ERR_NOMEM, "Token size too big considering fpm pool size\n");

    rc = get_rdp_freq(&dpi_params.runner_freq);
    if (rc)
    {
        BDMF_TRACE_RET(BDMF_ERR_NODEV, "Failed to get runner freq %d\n", rc);
    }

    return rc;
}
#endif

extern bbh_id_e rdpa_emac_to_bbh_id_e[rdpa_emac__num_of];
int system_data_path_init(void)
{
    const rdpa_system_init_cfg_t *system_init = _rdpa_system_init_cfg_get();
    const rdpa_system_cfg_t *system_cfg = _rdpa_system_cfg_get();
    const rdpa_qm_cfg_t *qm_cfg = _rdpa_system_qm_cfg_get();
    int rc;

    rc = rdp_get_init_params();
    if (rc)
    {
        BDMF_TRACE_RET(rc, "Failed to rdp_get_init_params");
    }

    /* TODO: add wan initialization using auto sensing */
    dpi_params.bbh_id_gbe_wan = system_init->gbe_wan_emac != rdpa_emac_none ?
        rdpa_emac_to_bbh_id_e[system_init->gbe_wan_emac] : BBH_ID_NULL;
    dpi_params.mtu_size = system_cfg->mtu_size;
    dpi_params.headroom_size = system_cfg->headroom_size;
    dpi_params.bcmsw_tag = system_init->runner_ext_sw_cfg.type;

    dpi_params.number_of_ds_queues = qm_cfg->number_of_ds_queues;
    dpi_params.number_of_us_queues = qm_cfg->number_of_us_queues;
    dpi_params.number_of_service_queues = qm_cfg->number_of_service_queues;

#if !defined(DSL63158)
    if (system_init->iptv_table_size == rdpa_table_1024_entries)
    {
#if defined(BCM6846) || defined(BCM6836)
        dpi_params.iptv_table_size =  hash_max_512_entries_per_engine;
        dpi_params.arl_table_size =  hash_max_256_entries_per_engine; 
#elif defined(BCM6858)
        dpi_params.iptv_table_size =  hash_max_256_entries_per_engine;
        dpi_params.arl_table_size =  hash_max_256_entries_per_engine; 
#else /* 6856 */
        dpi_params.iptv_table_size =  hash_max_256_entries_per_engine;
        dpi_params.arl_table_size =  hash_max_128_entries_per_engine; 
#endif
    }
    else /* iptv_table_size = rdpa_table_256_entries */
    {
#if defined(BCM6846) || defined(BCM6836)
        dpi_params.iptv_table_size =  hash_max_128_entries_per_engine;
        dpi_params.arl_table_size =  hash_max_512_entries_per_engine; 
#elif defined(BCM6858)  /* 1K for each table */
        dpi_params.iptv_table_size =  hash_max_256_entries_per_engine;
        dpi_params.arl_table_size =  hash_max_256_entries_per_engine; 
#else /* 6856 */
        dpi_params.iptv_table_size =  hash_max_64_entries_per_engine;
        dpi_params.arl_table_size =  hash_max_256_entries_per_engine; 
#endif
    }
#endif

#ifndef RDP_SIM
    dpi_params.xfi_port = bcm_misc_xfi_port_get();
#endif

#ifdef G9991
#ifndef RDP_SIM
    dpi_params.system_port = bcm_misc_g9991_debug_port_get();

    if (rdpa_is_gbe_mode() && system_init->gbe_wan_emac == dpi_params.system_port)
        dpi_params.system_port = rdpa_emac_none;

    dpi_params.g9991_port_vec = bcm_misc_g9991_phys_port_vec_get();
#else
    dpi_params.g9991_port_vec = (1 << rdpa_emac0) | (1 << rdpa_emac1) | (1 << rdpa_emac2) | (1 << rdpa_emac3);

    dpi_params.system_port = rdpa_emac_none;
#endif
    dpi_params.g9991_bbh_vec = dpi_params.g9991_port_vec;
#ifdef BCM6858
    if (dpi_params.g9991_port_vec & (1 << rdpa_emac0))
    {
        dpi_params.g9991_bbh_vec |= (1 << rdpa_emac4);
        dpi_params.g9991_bbh_vec &= ~(1 << rdpa_emac0);
    }

    if (dpi_params.g9991_port_vec & (1 << rdpa_emac4))
    {
        dpi_params.g9991_bbh_vec &= ~(1 << rdpa_emac4);
        dpi_params.g9991_bbh_vec |= (1 << rdpa_emac0);
    }
#endif

    if (asserted_bits_count_get(dpi_params.g9991_port_vec) > RDD_G9991_SID_TO_PHYSICAL_PORT_MASK_SIZE)
        BDMF_TRACE_RET(-1, "Failed: got wrong number of g9991 phys ports in vector 0x%x\n", dpi_params.g9991_port_vec);
#endif
    dpi_params.dpu_split_scheduling_mode = system_init->dpu_split_scheduling_mode; 

    rc = data_path_init(&dpi_params);
    if (rc)
        BDMF_TRACE_RET(-1, "Failed to data path init\n");
#ifndef RDP_SIM
    /*increase UBUS credits for QUAD0 towards MEMC where all CPU Rx/Tx processing is done*/
    ag_drv_rnr_quad_ext_flowctrl_config_token_val_set(0, DECODE_CFG_PID_MEMC, 8);
/* XXX: need to implement 6836 */
#if !defined(CONFIG_BCM96836) && !defined(CONFIG_BCM963158) && !defined(CONFIG_BCM96856)
    ubus_master_set_token_credits(MST_PORT_NODE_RQ0, DECODE_CFG_PID_MEMC, 8);
#endif
#endif
    return 0;
}

int system_data_path_init_fiber(rdpa_wan_type wan_type)
{
    switch (wan_type)
    {
    case rdpa_wan_gpon:
        data_path_init_fiber(MAC_TYPE_GPON);
        break;
    case rdpa_wan_epon:
        data_path_init_fiber(MAC_TYPE_EPON);
        break;
    case rdpa_wan_xgpon:
        data_path_init_fiber(MAC_TYPE_XGPON);
        break;
    case rdpa_wan_xepon:
        data_path_init_fiber(MAC_TYPE_XEPON);
        break;
    default:
        bdmf_trace("RDPA wan type is not supported");
        return -1;
    }

    return 0;
}

int system_data_path_init_gbe(rdpa_emac wan_emac)
{
    return data_path_init_gbe(wan_emac); 
}

#if defined(BCM63158)
int system_data_path_init_dsl(void)
{
    return data_path_init_dsl(MAC_TYPE_DSL); 
}
#endif

rdpa_bpm_buffer_size_t rdpa_bpm_buffer_size_get(void)
{
    return dpi_params.fpm_buf_size;
}


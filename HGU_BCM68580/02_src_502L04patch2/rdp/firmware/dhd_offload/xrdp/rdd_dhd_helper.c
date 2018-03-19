/*
   Copyright (c) 2014 Broadcom Corporation
   All Rights Reserved

    <:label-BRCM:2014:DUAL/GPL:standard

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

#include "rdd_dhd_helper.h"
#include "data_path_init.h"
#include "rdd_init.h"

RDD_DHD_HW_CONFIGURATION_DTS g_dhd_hw_config;

extern rdd_dhd_rx_post_ring_t g_dhd_rx_post_ring_priv[RDPA_MAX_RADIOS];
extern uintptr_t xrdp_virt2phys(const ru_block_rec *ru_block, uint8_t addr_idx);
extern RDD_FPM_GLOBAL_CFG_DTS g_fpm_hw_cfg;



/* Initialize entry in RxPost ring or template in SRAM with default and given parameters */
void rdd_rx_post_descr_init(uint32_t radio_idx, uint8_t *descr_ptr, uint32_t fpm_buffer_number)
{
    uint32_t req_id;
    uint64_t data_buf_ptr;
    uint64_t fpm_base_ptr;
    uint32_t addr_hi, addr_lo; 
    
    req_id = fpm_buffer_number | (1<<DHD_RX_POST_VALID_REQ_ID_OFFSET);                   
    fpm_base_ptr = g_fpm_hw_cfg.fpm_base_low + ((uint64_t)g_fpm_hw_cfg.fpm_base_high << 32);              
    data_buf_ptr = fpm_base_ptr + (fpm_buffer_number * (g_fpm_hw_cfg.fpm_token_size_asr_8 << 8)) + g_dhd_hw_config.ddr_sop_offset;

    GET_ADDR_HIGH_LOW(addr_hi, addr_lo, data_buf_ptr);
    
    /* 40 bits physical address */
    addr_lo = __swap4bytes(addr_lo);
    addr_hi = __swap4bytes((addr_hi & 0xff));
  
    switch (flow_ring_format[radio_idx])
    {           
        case FR_FORMAT_WI_WI64: /* Legacy Work Item */
        {

            RDD_DHD_RX_POST_DESCRIPTOR_MSG_TYPE_WRITE(DHD_MSG_TYPE_RX_POST, descr_ptr);
            RDD_DHD_RX_POST_DESCRIPTOR_IF_ID_WRITE(radio_idx, descr_ptr);
            RDD_DHD_RX_POST_DESCRIPTOR_COMMON_HDR_FLAGS_WRITE(0, descr_ptr);
            RDD_DHD_RX_POST_DESCRIPTOR_EPOCH_WRITE(0, descr_ptr);
            RDD_DHD_RX_POST_DESCRIPTOR_REQUEST_ID_WRITE(req_id, descr_ptr);
            RDD_DHD_RX_POST_DESCRIPTOR_META_BUF_LEN_WRITE(0, descr_ptr);
            RDD_DHD_RX_POST_DESCRIPTOR_DATA_LEN_WRITE(__swap2bytes(DHD_DATA_LEN), descr_ptr);
            RDD_DHD_RX_POST_DESCRIPTOR_METADATA_BUF_ADDR_HI_WRITE(0, descr_ptr);
            RDD_DHD_RX_POST_DESCRIPTOR_METADATA_BUF_ADDR_LOW_WRITE(0, descr_ptr);
            RDD_DHD_RX_POST_DESCRIPTOR_DATA_BUF_ADDR_HI_WRITE((addr_hi & 0xff), descr_ptr);
            RDD_DHD_RX_POST_DESCRIPTOR_DATA_BUF_ADDR_LOW_WRITE(addr_lo, descr_ptr);
            break;
        }

        case FR_FORMAT_WI_CWI32: /* Compact Work Item with 32b haddr */
        {               
            RDD_DHD_RX_POST_DESCRIPTOR_CWI32_REQUEST_ID_WRITE(req_id, descr_ptr);    
            RDD_DHD_RX_POST_DESCRIPTOR_CWI32_DATA_BUF_ADDR_LOW_WRITE(addr_lo, descr_ptr);
            break;
        }
        
        default:
        {
            bdmf_print("ERROR: flow ring format: %d not supported\n", flow_ring_format[radio_idx]);
            
            BUG();                
        }
    } /* switch item_type */
   
}


/* Set timer for packet aggregation and awake dhd timer task */
static int rdd_dhd_helper_aggregation_timer_set(uint8_t th)
{    
    RDD_BTRACE("th = %d\n", th);
    
    RDD_DHD_HW_CONFIGURATION_AGGR_TIMER_PERIOD_WRITE_G(th*1000, RDD_DHD_HW_CFG_ADDRESS_ARR, 0);
    
    WMB();
    /* wakeup timer task  */
    return ag_drv_rnr_regs_cfg_cpu_wakeup_set(get_runner_idx(dhd_tx_post_runner_image), DHD_TIMER_THREAD_NUMBER);
}


/* Initialize DHD helper object */
int rdd_dhd_hlp_cfg(uint32_t radio_idx, rdpa_dhd_init_cfg_t *init_cfg, int enable)
{       
    uint32_t i;
    bdmf_phys_addr_t phys_address;
    uint32_t addr_hi, addr_lo; 
    uint32_t lkp_tbl_size;
    
    RDD_BTRACE("radio_idx = %d, init_cfg = %p, enable = %d\n", radio_idx, init_cfg, enable);
    
    if (!enable)
    {
        /* DHD disabled for this radio => Re-calculate FPM  budget */
        set_fpm_budget(FPM_RES_WLAN, 0);
        return 0;
    }
        
    /* Invalidate lkp entries for specific radio - size of lookup table is 1/RDPA_MAX_RADIOS of total*/
    lkp_tbl_size = RDD_DHD_FLOW_RING_CACHE_LKP_TABLE_SIZE/RDPA_MAX_RADIOS;
    for (i = lkp_tbl_size*radio_idx; i < (lkp_tbl_size*radio_idx) + lkp_tbl_size; i++)
    {   
        RDD_DHD_FLOW_RING_CACHE_LKP_ENTRY_INVALID_WRITE_G(1 , RDD_DHD_FLOW_RING_CACHE_LKP_TABLE_ADDRESS_ARR, i);
    }

    RDD_DHD_POST_COMMON_RADIO_ENTRY_CACHE_CTX_NEXT_WRITE_IDX_WRITE_G(0, RDD_DHD_POST_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);
    
    /* Default value for aggregation thresholds (1 - means no aggregation )*/
    for (i = 0; i < RDPA_MAX_AC; i++)
    {
        RDD_DHD_POST_COMMON_RADIO_ENTRY_PER_AC_AGGREGATION_THRESHOLDS_WRITE_G(1, RDD_DHD_POST_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx, i);
        rdd_dhd_helper_aggregation_timeout_set(radio_idx, i, 60+i*20);  /* values get overwritten by DHD driver */
    }
    

    rdd_dhd_helper_aggregation_bypass_cpu_tx_set(radio_idx, 1);
    rdd_dhd_helper_aggregation_bypass_non_udp_tcp_set(radio_idx, 1);
    rdd_dhd_helper_aggregation_bypass_tcp_pktlen_set(radio_idx, 64);
  
    /* FR adresses */     
    phys_address = (bdmf_phys_addr_t) RDD_RSV_VIRT_TO_PHYS(init_cfg->rx_post_flow_ring_base_addr);         
    GET_ADDR_HIGH_LOW(addr_hi, addr_lo, phys_address);    
    RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY_RX_POST_FR_BASE_PTR_LOW_WRITE_G(addr_lo, RDD_DHD_COMPLETE_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);
    RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY_RX_POST_FR_BASE_PTR_HIGH_WRITE_G((addr_hi & 0xff), RDD_DHD_COMPLETE_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);


    phys_address = (bdmf_phys_addr_t) RDD_RSV_VIRT_TO_PHYS(init_cfg->rx_complete_flow_ring_base_addr);
    GET_ADDR_HIGH_LOW(addr_hi, addr_lo, phys_address);
    RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY_RX_COMPLETE_FR_BASE_PTR_LOW_WRITE_G(addr_lo, RDD_DHD_COMPLETE_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);
    RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY_RX_COMPLETE_FR_BASE_PTR_HIGH_WRITE_G((addr_hi & 0xff), RDD_DHD_COMPLETE_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);
    
    
    phys_address = (bdmf_phys_addr_t) RDD_RSV_VIRT_TO_PHYS(init_cfg->tx_complete_flow_ring_base_addr);
    GET_ADDR_HIGH_LOW(addr_hi, addr_lo, phys_address);
    RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY_TX_COMPLETE_FR_BASE_PTR_LOW_WRITE_G(addr_lo, RDD_DHD_COMPLETE_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);
    RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY_TX_COMPLETE_FR_BASE_PTR_HIGH_WRITE_G((addr_hi & 0xff), RDD_DHD_COMPLETE_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);    
    
    
    phys_address = (bdmf_phys_addr_t) init_cfg->tx_post_mgmt_arr_base_phys_addr;
    GET_ADDR_HIGH_LOW(addr_hi, addr_lo, phys_address);
    RDD_DHD_POST_COMMON_RADIO_ENTRY_TX_POST_MGMT_FR_BASE_PTR_LOW_WRITE_G(addr_lo, RDD_DHD_POST_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);
    RDD_DHD_POST_COMMON_RADIO_ENTRY_TX_POST_MGMT_FR_BASE_PTR_HIGH_WRITE_G((addr_hi & 0xff), RDD_DHD_POST_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);
    
    
    /* R2D indexies */
    /* rx_post indexes always at index 1 */
    phys_address = (bdmf_phys_addr_t) (((uint64_t)init_cfg->r2d_wr_arr_base_phys_addr + 2));
    GET_ADDR_HIGH_LOW(addr_hi, addr_lo, phys_address);
    RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY_RX_POST_FR_WR_IDX_PTR_LOW_WRITE_G(addr_lo, RDD_DHD_COMPLETE_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);
    RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY_RX_POST_FR_WR_IDX_PTR_HIGH_WRITE_G((addr_hi & 0xff), RDD_DHD_COMPLETE_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);
    
    phys_address = (bdmf_phys_addr_t) (((uint64_t)init_cfg->r2d_rd_arr_base_phys_addr + 2));
    GET_ADDR_HIGH_LOW(addr_hi, addr_lo, phys_address);
    RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY_RX_POST_FR_RD_IDX_PTR_LOW_WRITE_G(addr_lo, RDD_DHD_COMPLETE_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);
    RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY_RX_POST_FR_RD_IDX_PTR_HIGH_WRITE_G((addr_hi & 0xff), RDD_DHD_COMPLETE_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);
    
    phys_address = (bdmf_phys_addr_t) init_cfg->r2d_wr_arr_base_phys_addr;
    GET_ADDR_HIGH_LOW(addr_hi, addr_lo, phys_address);  
    RDD_DHD_POST_COMMON_RADIO_ENTRY_TX_POST_FR_WR_IDX_BASE_PTR_LOW_WRITE_G(addr_lo, RDD_DHD_POST_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);
    RDD_DHD_POST_COMMON_RADIO_ENTRY_TX_POST_FR_WR_IDX_BASE_PTR_HIGH_WRITE_G((addr_hi & 0xff), RDD_DHD_POST_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);
    
    phys_address = (bdmf_phys_addr_t) init_cfg->r2d_rd_arr_base_phys_addr;
    GET_ADDR_HIGH_LOW(addr_hi, addr_lo, phys_address);
    RDD_DHD_POST_COMMON_RADIO_ENTRY_TX_POST_FR_RD_IDX_BASE_PTR_LOW_WRITE_G(addr_lo, RDD_DHD_POST_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);
    RDD_DHD_POST_COMMON_RADIO_ENTRY_TX_POST_FR_RD_IDX_BASE_PTR_HIGH_WRITE_G((addr_hi & 0xff), RDD_DHD_POST_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);
    
    /* D2R indexies */
    /* rx_complete indexes always at index 1 */
    phys_address = (bdmf_phys_addr_t) ((uint64_t)init_cfg->d2r_wr_arr_base_phys_addr + 2);
    GET_ADDR_HIGH_LOW(addr_hi, addr_lo, phys_address);
    RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY_RX_COMPLETE_FR_WR_IDX_PTR_LOW_WRITE_G(addr_lo, RDD_DHD_COMPLETE_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);
    RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY_RX_COMPLETE_FR_WR_IDX_PTR_HIGH_WRITE_G((addr_hi & 0xff), RDD_DHD_COMPLETE_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);
    
    phys_address = (bdmf_phys_addr_t) ((uint64_t)init_cfg->d2r_rd_arr_base_phys_addr + 2);
    GET_ADDR_HIGH_LOW(addr_hi, addr_lo, phys_address);
    RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY_RX_COMPLETE_FR_RD_IDX_PTR_LOW_WRITE_G(addr_lo, RDD_DHD_COMPLETE_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);
    RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY_RX_COMPLETE_FR_RD_IDX_PTR_HIGH_WRITE_G((addr_hi & 0xff), RDD_DHD_COMPLETE_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);
    
    phys_address = (bdmf_phys_addr_t) init_cfg->d2r_wr_arr_base_phys_addr;
    GET_ADDR_HIGH_LOW(addr_hi, addr_lo, phys_address);
    RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY_TX_COMPLETE_FR_WR_IDX_PTR_LOW_WRITE_G(addr_lo, RDD_DHD_COMPLETE_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);
    RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY_TX_COMPLETE_FR_WR_IDX_PTR_HIGH_WRITE_G((addr_hi & 0xff), RDD_DHD_COMPLETE_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);
    
    phys_address = (bdmf_phys_addr_t) init_cfg->d2r_rd_arr_base_phys_addr;
    GET_ADDR_HIGH_LOW(addr_hi, addr_lo, phys_address);
    RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY_TX_COMPLETE_FR_RD_IDX_PTR_LOW_WRITE_G(addr_lo, RDD_DHD_COMPLETE_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);
    RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY_TX_COMPLETE_FR_RD_IDX_PTR_HIGH_WRITE_G((addr_hi & 0xff), RDD_DHD_COMPLETE_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);
    
    
    if (init_cfg->flow_ring_format == FR_FORMAT_WI_WI64)  /* Legacy Work Item */
    {
        /* Initialize template in SRAM for TxPost descriptor with default parameters */
        RDD_DHD_TX_POST_DESCRIPTOR_MSG_TYPE_WRITE_G(DHD_MSG_TYPE_TX_POST, RDD_DHD_TX_POST_FLOW_RING_BUFFER_ADDRESS_ARR, radio_idx);
        RDD_DHD_TX_POST_DESCRIPTOR_COMMON_HDR_FLAGS_WRITE_G(0, RDD_DHD_TX_POST_FLOW_RING_BUFFER_ADDRESS_ARR, radio_idx);
        RDD_DHD_TX_POST_DESCRIPTOR_EPOCH_WRITE_G(0, RDD_DHD_TX_POST_FLOW_RING_BUFFER_ADDRESS_ARR, radio_idx);
        RDD_DHD_TX_POST_DESCRIPTOR_SEG_CNT_WRITE_G(1, RDD_DHD_TX_POST_FLOW_RING_BUFFER_ADDRESS_ARR, radio_idx);
        RDD_DHD_TX_POST_DESCRIPTOR_METADATA_BUF_ADDR_HI_WRITE_G(0, RDD_DHD_TX_POST_FLOW_RING_BUFFER_ADDRESS_ARR, radio_idx);
        RDD_DHD_TX_POST_DESCRIPTOR_METADATA_BUF_ADDR_LOW_WRITE_G(0, RDD_DHD_TX_POST_FLOW_RING_BUFFER_ADDRESS_ARR, radio_idx);
        RDD_DHD_TX_POST_DESCRIPTOR_META_BUF_LEN_WRITE_G(0, RDD_DHD_TX_POST_FLOW_RING_BUFFER_ADDRESS_ARR, radio_idx);
        RDD_DHD_TX_POST_DESCRIPTOR_DATA_BUF_ADDR_HI_WRITE_G(0, RDD_DHD_TX_POST_FLOW_RING_BUFFER_ADDRESS_ARR, radio_idx);
                
        
        RDD_DHD_RX_POST_DESCRIPTOR_MSG_TYPE_WRITE_G(DHD_MSG_TYPE_RX_POST, RDD_DHD_RX_POST_FLOW_RING_BUFFER_ADDRESS_ARR, radio_idx);
        RDD_DHD_RX_POST_DESCRIPTOR_IF_ID_WRITE_G(radio_idx, RDD_DHD_RX_POST_FLOW_RING_BUFFER_ADDRESS_ARR, radio_idx);
        RDD_DHD_RX_POST_DESCRIPTOR_COMMON_HDR_FLAGS_WRITE_G(0, RDD_DHD_RX_POST_FLOW_RING_BUFFER_ADDRESS_ARR, radio_idx);
        RDD_DHD_RX_POST_DESCRIPTOR_EPOCH_WRITE_G(0, RDD_DHD_RX_POST_FLOW_RING_BUFFER_ADDRESS_ARR, radio_idx);
        RDD_DHD_RX_POST_DESCRIPTOR_REQUEST_ID_WRITE_G(0, RDD_DHD_RX_POST_FLOW_RING_BUFFER_ADDRESS_ARR, radio_idx);
        RDD_DHD_RX_POST_DESCRIPTOR_META_BUF_LEN_WRITE_G(0, RDD_DHD_RX_POST_FLOW_RING_BUFFER_ADDRESS_ARR, radio_idx);
        RDD_DHD_RX_POST_DESCRIPTOR_DATA_LEN_WRITE_G(__swap2bytes(DHD_DATA_LEN), RDD_DHD_RX_POST_FLOW_RING_BUFFER_ADDRESS_ARR, radio_idx);
        RDD_DHD_RX_POST_DESCRIPTOR_METADATA_BUF_ADDR_HI_WRITE_G(0, RDD_DHD_RX_POST_FLOW_RING_BUFFER_ADDRESS_ARR, radio_idx);
        RDD_DHD_RX_POST_DESCRIPTOR_METADATA_BUF_ADDR_LOW_WRITE_G(0, RDD_DHD_RX_POST_FLOW_RING_BUFFER_ADDRESS_ARR, radio_idx);
        RDD_DHD_RX_POST_DESCRIPTOR_DATA_BUF_ADDR_HI_WRITE_G(0, RDD_DHD_RX_POST_FLOW_RING_BUFFER_ADDRESS_ARR, radio_idx);
        RDD_DHD_RX_POST_DESCRIPTOR_DATA_BUF_ADDR_LOW_WRITE_G(0, RDD_DHD_RX_POST_FLOW_RING_BUFFER_ADDRESS_ARR, radio_idx);        
    }
    
    g_dhd_rx_post_ring_priv[radio_idx].wr_idx_addr = (uint16_t *)init_cfg->r2d_wr_arr_base_addr + 1;
        
    /* Update RX_post WR index both in SRAM and DDR. No need doorbell DHD that there are buffers available in RX Post.*/
    RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY_RX_POST_WR_IDX_WRITE_G(__swap2bytes(DHD_RX_POST_FLOW_RING_SIZE - 1), RDD_DHD_COMPLETE_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);
    

    /* direct interrupt to dongle */    
    RDD_DHD_POST_COMMON_RADIO_ENTRY_DHD_DOORBELL_LOW_WRITE_G(init_cfg->dongle_wakeup_register, RDD_DHD_POST_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);
    RDD_DHD_POST_COMMON_RADIO_ENTRY_DHD_DOORBELL_HIGH_WRITE_G(0, RDD_DHD_POST_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);
    
    RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY_DHD_DOORBELL_LOW_WRITE_G(init_cfg->dongle_wakeup_register, RDD_DHD_COMPLETE_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);
    RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY_DHD_DOORBELL_HIGH_WRITE_G(0, RDD_DHD_COMPLETE_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);


    /* Reset the rest of the fields */    
    RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY_TX_COMPLETE_RD_IDX_WRITE_G(0, RDD_DHD_COMPLETE_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);
    RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY_TX_COMPLETE_WR_IDX_WRITE_G(0, RDD_DHD_COMPLETE_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);        
    RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY_RX_COMPLETE_RD_IDX_WRITE_G(0, RDD_DHD_COMPLETE_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);
    RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY_RX_COMPLETE_WR_IDX_WRITE_G(0, RDD_DHD_COMPLETE_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);
            
    RDD_DHD_POST_COMMON_RADIO_ENTRY_ADD_LLCSNAP_HEADER_WRITE_G(init_cfg->add_llcsnap_header, RDD_DHD_POST_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);
    
    RDD_DHD_POST_COMMON_RADIO_ENTRY_FLOW_RING_FORMAT_WRITE_G(init_cfg->flow_ring_format, RDD_DHD_POST_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);
    RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY_FLOW_RING_FORMAT_WRITE_G(init_cfg->flow_ring_format, RDD_DHD_COMPLETE_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);
    
#ifdef RDP_SIM    
    RDD_DHD_DOORBELL_VALUE_WRITE_G(1<<(DONGLE_WAKEUP_REGISTER_0 + radio_idx), RDD_DHD_DOORBELL_VALUE_ADDRESS_ARR, radio_idx);
#else
    RDD_DHD_DOORBELL_VALUE_WRITE_G(0xdeadbee0, RDD_DHD_DOORBELL_VALUE_ADDRESS_ARR, radio_idx);    
#endif    
    phys_address = xrdp_virt2phys(&RU_BLK(UBUS_SLV), 0) + RU_REG_OFFSET(UBUS_SLV, RNR_INTR_CTRL_ITR);

    GET_ADDR_HIGH_LOW(addr_hi, addr_lo, phys_address);

    RDD_DDR_ADDRESS_HIGH_WRITE_G((addr_hi & 0xff), RDD_DHD_CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR_ADDRESS_ARR, 0);
    RDD_DDR_ADDRESS_LOW_WRITE_G(addr_lo, RDD_DHD_CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR_ADDRESS_ARR, 0);
    
    RDD_DHD_RING_SIZE_VALUE_WRITE_G(DHD_RX_COMPLETE_FLOW_RING_SIZE, RDD_DHD_RX_COMPLETE_RING_SIZE_ADDRESS_ARR, 0);
    RDD_DHD_RING_SIZE_VALUE_WRITE_G(DHD_TX_COMPLETE_FLOW_RING_SIZE, RDD_DHD_TX_COMPLETE_RING_SIZE_ADDRESS_ARR, 0);    
    RDD_DHD_RING_SIZE_VALUE_WRITE_G(DHD_RX_POST_FLOW_RING_SIZE, RDD_DHD_RX_POST_RING_SIZE_ADDRESS_ARR, 0);    
    

    /* enable timer with 1msec resolution */
     rdd_dhd_helper_aggregation_timer_set(1);
    
    /* Re-calculate FPM  budget */
    set_fpm_budget(FPM_RES_WLAN, 1);
    
    return 0;
}


static RDD_DHD_POST_COMMON_RADIO_ENTRY_DTS *get_radio_instance_data_ptr(uint32_t radio_idx)
{
    RDD_DHD_POST_COMMON_RADIO_DATA_DTS *radio_instance_post_data_ptr;

    radio_instance_post_data_ptr = (RDD_DHD_POST_COMMON_RADIO_DATA_DTS *)RDD_DHD_POST_COMMON_RADIO_DATA_PTR(get_runner_idx(dhd_tx_post_runner_image));
    
#if (RDPA_MAX_RADIOS == 1)
    return &radio_instance_post_data_ptr->entry;
#else
    return &radio_instance_post_data_ptr->entry[radio_idx];
#endif    
}


/* sets aggregation timeout for given AC */
int rdd_dhd_helper_aggregation_timeout_set(uint32_t radio_idx, int access_category, uint8_t aggregation_timeout)
{
    RDD_BTRACE("radio_idx = %d, access_category = %d, aggregation_timeout = %d\n", radio_idx, access_category,
        aggregation_timeout);
    
    /* per access category timeout value rather than global timer for all radios/access category */    
    RDD_DHD_POST_COMMON_RADIO_ENTRY_PER_AC_AGGREGATION_TIMEOUTS_WRITE_G(aggregation_timeout, RDD_DHD_POST_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx, access_category);
    RDD_DHD_POST_COMMON_RADIO_ENTRY_PER_AC_AGGREGATION_TIMEOUT_CNTRS_WRITE_G(0, RDD_DHD_POST_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx, access_category);
    
    return 0;
}

/* gets aggregation timeout for given AC */
int rdd_dhd_helper_aggregation_timeout_get(uint32_t radio_idx, int access_category, uint8_t *aggregation_timeout)
{
    RDD_BTRACE("radio_idx = %d, access_category = %d\n", radio_idx, access_category);
        
    /* per access category timeout value rather than global timer for all radios/access category   */
    RDD_DHD_POST_COMMON_RADIO_ENTRY_PER_AC_AGGREGATION_TIMEOUTS_READ(*aggregation_timeout, get_radio_instance_data_ptr(radio_idx), access_category);
      
    return 0;
}

/* sets aggregation size for given AC */
int rdd_dhd_helper_aggregation_size_set(uint32_t radio_idx, int access_category, uint8_t aggregation_size)
{    
    RDD_BTRACE("radio_idx = %d, access_category = %d, aggregation_size = %d\n", radio_idx, access_category,
        aggregation_size);

    RDD_DHD_POST_COMMON_RADIO_ENTRY_PER_AC_AGGREGATION_THRESHOLDS_WRITE_G(aggregation_size, RDD_DHD_POST_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx, access_category);
        
    return 0;
}

/* gets aggregation size for given AC */
int rdd_dhd_helper_aggregation_size_get(uint32_t radio_idx, int access_category, uint8_t *aggregation_size)
{     
    RDD_BTRACE("radio_idx = %d, access_category = %d, aggregation_size = %p\n", radio_idx, access_category,
        aggregation_size);

    RDD_DHD_POST_COMMON_RADIO_ENTRY_PER_AC_AGGREGATION_THRESHOLDS_READ(
        *aggregation_size, get_radio_instance_data_ptr(radio_idx), access_category);
        
    return 0;
}

/* sets aggregation bypass for CPU TX packet */
int rdd_dhd_helper_aggregation_bypass_cpu_tx_set(uint32_t radio_idx, bdmf_boolean enable)
{    
    RDD_BTRACE("radio_idx = %d, enable = %d\n", radio_idx, enable);
        
    RDD_DHD_POST_COMMON_RADIO_ENTRY_AGGREGATION_BYPASS_CPU_TX_WRITE_G(enable, RDD_DHD_POST_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);
            
    return 0;
}

/* gets aggregation bypass for CPU TX packet */
int rdd_dhd_helper_aggregation_bypass_cpu_tx_get(uint32_t radio_idx, bdmf_boolean *enable)
{    
    RDD_BTRACE("radio_idx = %d\n", radio_idx);		 
    
    RDD_DHD_POST_COMMON_RADIO_ENTRY_AGGREGATION_BYPASS_CPU_TX_READ(*enable, get_radio_instance_data_ptr(radio_idx));
        
    return 0;
}

/* sets aggregation bypass for non UDP/TCP packet */
int rdd_dhd_helper_aggregation_bypass_non_udp_tcp_set(uint32_t radio_idx, bdmf_boolean enable)
{
    RDD_BTRACE("radio_idx = %d, enable = %d\n", radio_idx, enable);
        
    RDD_DHD_POST_COMMON_RADIO_ENTRY_AGGREGATION_BYPASS_NON_UDP_TCP_WRITE_G(enable, RDD_DHD_POST_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);
            
    return 0;
}

/* gets aggregation bypass for non UDP/TCP packet */
int rdd_dhd_helper_aggregation_bypass_non_udp_tcp_get(uint32_t radio_idx, bdmf_boolean *enable)
{    
    RDD_BTRACE("radio_idx = %d\n", radio_idx);

    RDD_DHD_POST_COMMON_RADIO_ENTRY_AGGREGATION_BYPASS_NON_UDP_TCP_READ(*enable, get_radio_instance_data_ptr(radio_idx));
        
    return 0;
}

/* sets aggregation bypass for TCP packet with less or equal to the configued pkt_len */
int rdd_dhd_helper_aggregation_bypass_tcp_pktlen_set(uint32_t radio_idx, uint8_t pkt_len)
{
    RDD_BTRACE("radio_idx = %d, pkt_len = %d\n", radio_idx, pkt_len);
    
    /* dhd TX packet doesn't have eth header, so we are dealing with that 14 byte subtraction here */
    if (pkt_len < (DHD_TX_POST_PKT_AGGR_TCP_LEN_MIN + DHD_ETH_L2_HEADER_SIZE))
        pkt_len = 0;
    else
        pkt_len -= DHD_ETH_L2_HEADER_SIZE;

    
    RDD_DHD_POST_COMMON_RADIO_ENTRY_AGGREGATION_BYPASS_TCP_PKTLEN_WRITE_G(pkt_len, RDD_DHD_POST_COMMON_RADIO_DATA_ADDRESS_ARR, radio_idx);
            
    return 0;
}

/* gets aggregation bypass for TCP packet with less or equal to the configued pkt_len */
int rdd_dhd_helper_aggregation_bypass_tcp_pktlen_get(uint32_t radio_idx, uint8_t *pkt_len)
{
    uint8_t pkt_local;
    
    RDD_BTRACE("radio_idx = %d\n", radio_idx);		
    
    RDD_DHD_POST_COMMON_RADIO_ENTRY_AGGREGATION_BYPASS_TCP_PKTLEN_READ(pkt_local, get_radio_instance_data_ptr(radio_idx));
    
     if (pkt_local == 0)
        *pkt_len = 0;
    else
        *pkt_len = pkt_local + DHD_ETH_L2_HEADER_SIZE;
            
    return 0;
}


/* sets aggregation bypass for TCP packet with less or equal to the configued pkt_len */
int rdd_dhd_helper_fpm_thresholds_set(uint16_t low_th, uint16_t high_th)
{
    RDD_BTRACE("low_th = %d, high_th = %d\n", low_th, high_th);
    
    RDD_BYTES_2_BITS_WRITE_G(low_th, RDD_DHD_FPM_LOW_THRESHOLD_ADDRESS_ARR, 0);
    RDD_BYTES_2_BITS_WRITE_G(high_th, RDD_DHD_FPM_HIGH_THRESHOLD_ADDRESS_ARR, 0);
    
    RDD_DHD_HW_CONFIGURATION_FPM_LOW_THRESHOLD_WRITE_G(low_th, RDD_DHD_HW_CFG_ADDRESS_ARR, 0);   
    RDD_DHD_HW_CONFIGURATION_FPM_HIGH_THRESHOLD_WRITE_G(high_th, RDD_DHD_HW_CFG_ADDRESS_ARR, 0);        
    return 0;
}

/* gets aggregation bypass for TCP packet with less or equal to the configued pkt_len */
int rdd_dhd_helper_fpm_thresholds_get(uint16_t *low_th, uint16_t *high_th)
{
    RDD_BTRACE("get fpm thresholds\n");		
    
    RDD_BYTES_2_BITS_READ_G(*low_th, RDD_DHD_FPM_LOW_THRESHOLD_ADDRESS_ARR, 0);
    RDD_BYTES_2_BITS_READ_G(*high_th, RDD_DHD_FPM_HIGH_THRESHOLD_ADDRESS_ARR, 0);
            
    return 0;
}


/* Called from rdd_init and propagates fpm information to runner */
void rdd_dhd_hw_cfg(RDD_DHD_HW_CONFIGURATION_DTS *dhd_hw_config)
{
    uint8_t pool_num;
    
    RDD_BTRACE("dhd_hw_config = %p\n", dhd_hw_config);

    memcpy(&g_dhd_hw_config, dhd_hw_config, sizeof(RDD_DHD_HW_CONFIGURATION_DTS));
        
    RDD_DHD_HW_CONFIGURATION_DDR_SOP_OFFSET_WRITE_G(dhd_hw_config->ddr_sop_offset, RDD_DHD_HW_CFG_ADDRESS_ARR, 0);
    
    /* should map : 256 - pool0 (max); 512 -pool1; 1024 - pool2; 2048 - pool3*/
    if (g_fpm_hw_cfg.fpm_token_size_asr_8 == (2048 >> 8))
        pool_num = FPM_POOL_ID_ONE_BUFFER;
    else
        pool_num = g_fpm_hw_cfg.fpm_token_size_asr_8/2;
        
    RDD_DHD_HW_CONFIGURATION_RX_POST_FPM_POOL_WRITE_G(pool_num, RDD_DHD_HW_CFG_ADDRESS_ARR, 0);
        
}

/* Initialize CPU TX complete ring in runnner */
void rdd_complete_ring_init(uint32_t radio_idx, RING_DESCTIPTOR  *descriptor, bdmf_phys_addr_t phy_addr)
{
    uint32_t addr_hi, addr_lo; 
    
    GET_ADDR_HIGH_LOW(addr_hi, addr_lo, phy_addr);            
    RDD_CPU_RING_DESCRIPTOR_BASE_ADDR_LOW_WRITE_G(addr_lo, RDD_DHD_CPU_RECYCLE_RING_DESCRIPTOR_TABLE_ADDRESS_ARR, radio_idx);
    RDD_CPU_RING_DESCRIPTOR_BASE_ADDR_HIGH_WRITE_G((addr_hi & 0xff), RDD_DHD_CPU_RECYCLE_RING_DESCRIPTOR_TABLE_ADDRESS_ARR, radio_idx);
    
    RDD_CPU_RING_DESCRIPTOR_READ_IDX_WRITE_G(descriptor->shadow_read_idx, RDD_DHD_CPU_RECYCLE_RING_DESCRIPTOR_TABLE_ADDRESS_ARR, radio_idx);
    RDD_CPU_RING_DESCRIPTOR_WRITE_IDX_WRITE_G(descriptor->shadow_write_idx, RDD_DHD_CPU_RECYCLE_RING_DESCRIPTOR_TABLE_ADDRESS_ARR, radio_idx);
    RDD_CPU_RING_DESCRIPTOR_DROP_COUNTER_WRITE_G(0, RDD_DHD_CPU_RECYCLE_RING_DESCRIPTOR_TABLE_ADDRESS_ARR, radio_idx);    
    RDD_CPU_RING_DESCRIPTOR_SIZE_OF_ENTRY_WRITE_G(descriptor->size_of_entry, RDD_DHD_CPU_RECYCLE_RING_DESCRIPTOR_TABLE_ADDRESS_ARR, radio_idx);
    RDD_CPU_RING_DESCRIPTOR_NUMBER_OF_ENTRIES_WRITE_G(descriptor->num_of_entries, RDD_DHD_CPU_RECYCLE_RING_DESCRIPTOR_TABLE_ADDRESS_ARR, radio_idx);    
}


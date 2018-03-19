/*
   Copyright (c) 2013 Broadcom Corporation
   All Rights Reserved

    <:label-BRCM:2013:DUAL/GPL:standard

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


#include "bdmf_shell.h"
#include "rdd.h"
#include "dhd_defs.h"
#include "rdpa_dhd_helper_basic.h"
#include "rdp_drv_proj_cntr.h"
#include "rdd_dhd_helper.h"


#define MAKE_BDMF_SHELL_CMD_NOPARM(dir, cmd, help, cb) \
    bdmfmon_cmd_add(dir, cmd, cb, help, BDMF_ACCESS_ADMIN, NULL, NULL)

#define MAKE_BDMF_SHELL_CMD(dir, cmd, help, cb, parms...)   \
{                                                           \
    static bdmfmon_cmd_parm_t cmd_parms[]={                 \
        parms,                                              \
        BDMFMON_PARM_LIST_TERMINATOR                        \
    };                                                      \
    bdmfmon_cmd_add(dir, cmd, cb, help, BDMF_ACCESS_ADMIN, NULL, cmd_parms); \
}

static RDD_DHD_POST_COMMON_RADIO_ENTRY_DTS* get_dhd_post_radio_entry(uint32_t radio_idx)
{
    RDD_DHD_POST_COMMON_RADIO_DATA_DTS *radio_instance_table_ptr;   
    
    radio_instance_table_ptr = (RDD_DHD_POST_COMMON_RADIO_DATA_DTS *) RDD_DHD_POST_COMMON_RADIO_DATA_PTR(get_runner_idx(dhd_tx_post_runner_image));
    
#if (RDPA_MAX_RADIOS == 1)
    return &radio_instance_table_ptr->entry;
#else
    return &radio_instance_table_ptr->entry[radio_idx];
#endif
}

static RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY_DTS* get_dhd_complete_radio_entry(uint32_t radio_idx)
{
    RDD_DHD_COMPLETE_COMMON_RADIO_DATA_DTS *radio_instance_table_ptr;   
    
    radio_instance_table_ptr = (RDD_DHD_COMPLETE_COMMON_RADIO_DATA_DTS *) RDD_DHD_COMPLETE_COMMON_RADIO_DATA_PTR(get_runner_idx(dhd_complete_runner_image));
    
#if (RDPA_MAX_RADIOS == 1)
    return &radio_instance_table_ptr->entry;
#else
    return &radio_instance_table_ptr->entry[radio_idx];
#endif
}

static int _rdd_print_dhd_tx_post_desc(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{    
    RDD_DHD_POST_COMMON_RADIO_ENTRY_DTS *radio_instance_entry_ptr;
    void *desc_ptr, *base_ptr;

    uint32_t radio_idx, ring_id, desc_num;
    uint32_t addr_lo,addr_hi,val;
    rdpa_dhd_flring_cache_t *entry_ptr;

    radio_idx = (uint32_t)parm[0].value.unumber;
    ring_id = (uint32_t)parm[1].value.unumber;
    desc_num = (uint32_t)parm[2].value.unumber;

    radio_instance_entry_ptr = get_dhd_post_radio_entry(radio_idx);
    
    RDD_DHD_POST_COMMON_RADIO_ENTRY_TX_POST_MGMT_FR_BASE_PTR_LOW_READ(addr_lo, radio_instance_entry_ptr);
    RDD_DHD_POST_COMMON_RADIO_ENTRY_TX_POST_MGMT_FR_BASE_PTR_HIGH_READ(addr_hi, radio_instance_entry_ptr);
    
#ifdef PHYS_ADDR_64BIT
    base_ptr = (void*) (((uint64_t)addr_hi << 32) + addr_lo);
#else
    addr_hi = 0;
    base_ptr = (void*) (addr_lo | addr_hi);            
#endif
    
    entry_ptr = (rdpa_dhd_flring_cache_t *)(base_ptr) + ring_id;


    bdmf_session_print(session, "TX POST descriptor\n");
    
    switch (flow_ring_format[radio_idx])
    {           
        case FR_FORMAT_WI_WI64: /* Legacy Work Item */
        {
#ifdef PHYS_ADDR_64BIT
    desc_ptr = (RDD_DHD_TX_POST_DESCRIPTOR_DTS *)RDD_RSV_PHYS_TO_VIRT(((uint64_t)entry_ptr->base_addr_high << 32) + entry_ptr->base_addr_low) + desc_num;
#else
    desc_ptr = (RDD_DHD_TX_POST_DESCRIPTOR_DTS *)RDD_RSV_PHYS_TO_VIRT(entry_ptr->base_addr_low) + desc_num;
#endif
            RDD_DHD_TX_POST_DESCRIPTOR_MSG_TYPE_READ(val, desc_ptr);
            bdmf_session_print(session, "msg_type                 = 0x%4.4x\n", val);
            RDD_DHD_TX_POST_DESCRIPTOR_IF_ID_READ(val, desc_ptr);
            bdmf_session_print(session, "if_id                    = 0x%4.4x\n", val);
            RDD_DHD_TX_POST_DESCRIPTOR_REQUEST_ID_READ(val, desc_ptr);
            bdmf_session_print(session, "request_id               = 0x%8.8x\n", val);
            RDD_DHD_TX_POST_DESCRIPTOR_TX_ETH_HDR_0_READ(val, desc_ptr);
            bdmf_session_print(session, "tx_eth_hdr_0             = 0x%8.8x\n", val);
            RDD_DHD_TX_POST_DESCRIPTOR_TX_ETH_HDR_1_READ(val, desc_ptr);
            bdmf_session_print(session, "tx_eth_hdr_1             = 0x%8.8x\n", val);
            RDD_DHD_TX_POST_DESCRIPTOR_TX_ETH_HDR_2_READ(val, desc_ptr);
            bdmf_session_print(session, "tx_eth_hdr_2             = 0x%8.8x\n", val);
            RDD_DHD_TX_POST_DESCRIPTOR_TX_ETH_HDR_3_READ(val, desc_ptr);
            bdmf_session_print(session, "tx_eth_hdr_3             = 0x%4.4x\n", val);
            RDD_DHD_TX_POST_DESCRIPTOR_FLAGS_READ(val, desc_ptr);
            bdmf_session_print(session, "flags                    = 0x%4.4x\n", val);
            RDD_DHD_TX_POST_DESCRIPTOR_DATA_BUF_ADDR_LOW_READ(val, desc_ptr);
            bdmf_session_print(session, "data_buf_addr_low    	  = 0x%8.8x\n", __swap4bytes(val));
            RDD_DHD_TX_POST_DESCRIPTOR_DATA_BUF_ADDR_HI_READ(val, desc_ptr);
            bdmf_session_print(session, "data_buf_addr_hi         = 0x%8.8x\n", __swap4bytes(val));    
            RDD_DHD_TX_POST_DESCRIPTOR_DATA_LEN_READ(val, desc_ptr);
            bdmf_session_print(session, "data_len                 = 0x%4.4x\n", __swap2bytes(val));

            break;
        }

        case FR_FORMAT_WI_CWI32: /* Compact Work Item with 32b haddr */
        {               
#ifdef PHYS_ADDR_64BIT
    desc_ptr = (RDD_DHD_TX_POST_DESCRIPTOR_CWI32_DTS *)RDD_RSV_PHYS_TO_VIRT(((uint64_t)entry_ptr->base_addr_high << 32) + entry_ptr->base_addr_low) + desc_num;
#else
    desc_ptr = (RDD_DHD_TX_POST_DESCRIPTOR_CWI32_DTS *)RDD_RSV_PHYS_TO_VIRT(entry_ptr->base_addr_low) + desc_num;
#endif
            RDD_DHD_TX_POST_DESCRIPTOR_CWI32_IF_ID_READ(val, desc_ptr);
            bdmf_session_print(session, "if_id                    = 0x%4.4x\n", val);
            RDD_DHD_TX_POST_DESCRIPTOR_CWI32_REQUEST_ID_READ(val, desc_ptr);
            bdmf_session_print(session, "request_id               = 0x%8.8x\n", val);
            RDD_DHD_TX_POST_DESCRIPTOR_CWI32_TX_ETH_HDR_0_READ(val, desc_ptr);
            bdmf_session_print(session, "tx_eth_hdr_0             = 0x%8.8x\n", val);
            RDD_DHD_TX_POST_DESCRIPTOR_CWI32_TX_ETH_HDR_1_READ(val, desc_ptr);
            bdmf_session_print(session, "tx_eth_hdr_1             = 0x%8.8x\n", val);
            RDD_DHD_TX_POST_DESCRIPTOR_CWI32_TX_ETH_HDR_2_READ(val, desc_ptr);
            bdmf_session_print(session, "tx_eth_hdr_2             = 0x%8.8x\n", val);
            RDD_DHD_TX_POST_DESCRIPTOR_CWI32_TX_ETH_HDR_3_READ(val, desc_ptr);
            bdmf_session_print(session, "tx_eth_hdr_3             = 0x%4.4x\n", val);
            RDD_DHD_TX_POST_DESCRIPTOR_CWI32_FLAGS_READ(val, desc_ptr);
            bdmf_session_print(session, "flags                    = 0x%4.4x\n", val);
            RDD_DHD_TX_POST_DESCRIPTOR_CWI32_DATA_BUF_ADDR_LOW_READ(val, desc_ptr);
            bdmf_session_print(session, "data_buf_addr        	  = 0x%8.8x\n", __swap4bytes(val));            
            RDD_DHD_TX_POST_DESCRIPTOR_CWI32_DATA_LEN_READ(val, desc_ptr);
            bdmf_session_print(session, "data_len                 = 0x%4.4x\n", __swap2bytes(val));

            break;
        }        
    } /* switch item_type */
        
    return 0;
}

static int _rdd_print_dhd_tx_complete_desc(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY_DTS     *radio_instance_entry_ptr;
    void                                        *desc_ptr;
    uint32_t                                    desc_num, radio_idx; 
    bdmf_phys_addr_t                            base_ptr;
    uint32_t                                    addr_lo,addr_hi,val;

    radio_idx = (uint32_t)parm[0].value.unumber;
    desc_num = (uint32_t)parm[1].value.unumber;

    radio_instance_entry_ptr = get_dhd_complete_radio_entry(radio_idx);

    RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY_TX_COMPLETE_FR_BASE_PTR_LOW_READ(addr_lo, radio_instance_entry_ptr);
    RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY_TX_COMPLETE_FR_BASE_PTR_HIGH_READ(addr_hi, radio_instance_entry_ptr);

#ifdef PHYS_ADDR_64BIT
    base_ptr = ((uint64_t)addr_hi << 32) + addr_lo;
#else
    addr_hi = 0;
    base_ptr = addr_lo | addr_hi;            
#endif      
    
    bdmf_session_print(session, "TX COMPLETE descriptor\n");
    
    switch (flow_ring_format[radio_idx])
    {           
        case FR_FORMAT_WI_WI64: /* Legacy Work Item */
        {
            desc_ptr = (RDD_DHD_TX_COMPLETE_DESCRIPTOR_DTS *)RDD_RSV_PHYS_TO_VIRT(base_ptr) + desc_num;
            
            RDD_DHD_TX_COMPLETE_DESCRIPTOR_MSG_TYPE_READ(val, desc_ptr);
            bdmf_session_print(session, "msg_type                 = 0x%4.4x\n", val);
            RDD_DHD_TX_COMPLETE_DESCRIPTOR_IF_ID_READ(val, desc_ptr);
            bdmf_session_print(session, "if_id                    = 0x%4.4x\n", val);
            RDD_DHD_TX_COMPLETE_DESCRIPTOR_COMMON_HDR_FLAGS_READ(val, desc_ptr);
            bdmf_session_print(session, "common_hdr_flags         = 0x%4.4x\n", val);
            RDD_DHD_TX_COMPLETE_DESCRIPTOR_EPOCH_READ(val, desc_ptr);
            bdmf_session_print(session, "epoch                    = 0x%4.4x\n", val);
            RDD_DHD_TX_COMPLETE_DESCRIPTOR_REQUEST_ID_READ(val, desc_ptr);
            bdmf_session_print(session, "request_id               = 0x%8.8x\n", val);
            RDD_DHD_TX_COMPLETE_DESCRIPTOR_STATUS_READ(val, desc_ptr);
            bdmf_session_print(session, "status                   = 0x%4.4x\n", val);
            RDD_DHD_TX_COMPLETE_DESCRIPTOR_FLOW_RING_ID_READ(val, desc_ptr);
            bdmf_session_print(session, "flow_ring_id             = 0x%4.4x\n", __swap2bytes(val));
            RDD_DHD_TX_COMPLETE_DESCRIPTOR_DMA_DONE_MARK_READ(val, desc_ptr);
            bdmf_session_print(session, "dma_done_mark 	          = 0x%8.8x\n", val);
            break;
        }

        case FR_FORMAT_WI_CWI32: /* Compact Work Item with 32b haddr */
        {               
            desc_ptr = (RDD_DHD_TX_COMPLETE_DESCRIPTOR_CWI32_DTS *)RDD_RSV_PHYS_TO_VIRT(base_ptr) + desc_num;                        
            
            RDD_DHD_TX_COMPLETE_DESCRIPTOR_CWI32_REQUEST_ID_READ(val, desc_ptr);
            bdmf_session_print(session, "request_id               = 0x%8.8x\n", val);
            break;
        }        
    } /* switch item_type */
    
    return 0;
}

static int _rdd_print_dhd_rx_complete_desc(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY_DTS  *radio_instance_entry_ptr;
    void                                     *desc_ptr;    
    uint32_t                                 desc_num, radio_idx;
    bdmf_phys_addr_t                         base_ptr;
    uint32_t                                 addr_lo,addr_hi, val;

    radio_idx = (uint32_t)parm[0].value.unumber;
    desc_num = (uint32_t)parm[1].value.unumber;
    
    radio_instance_entry_ptr = get_dhd_complete_radio_entry(radio_idx);

    RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY_RX_COMPLETE_FR_BASE_PTR_LOW_READ(addr_lo, radio_instance_entry_ptr);
    RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY_RX_COMPLETE_FR_BASE_PTR_HIGH_READ(addr_hi, radio_instance_entry_ptr);
    
#ifdef PHYS_ADDR_64BIT
    base_ptr = ((uint64_t)addr_hi << 32) + addr_lo;
#else
    addr_hi = 0;
    base_ptr = addr_lo | addr_hi;            
#endif
       
    switch (flow_ring_format[radio_idx])
    {           
        case FR_FORMAT_WI_WI64: /* Legacy Work Item */
        {        
            desc_ptr = (RDD_DHD_RX_COMPLETE_DESCRIPTOR_DTS *)RDD_RSV_PHYS_TO_VIRT(base_ptr) + desc_num;
            
            bdmf_session_print(session, "RX COMPLETE descriptor\n");
            RDD_DHD_RX_COMPLETE_DESCRIPTOR_MSG_TYPE_READ(val, desc_ptr);
            bdmf_session_print(session, "msg_type                 = 0x%4.4x\n", val);
            RDD_DHD_RX_COMPLETE_DESCRIPTOR_IF_ID_READ(val, desc_ptr);
            bdmf_session_print(session, "if_id                    = 0x%4.4x\n", val);
            RDD_DHD_RX_COMPLETE_DESCRIPTOR_REQUEST_ID_READ(val, desc_ptr);
            bdmf_session_print(session, "request_id               = 0x%8.8x\n", val);
            RDD_DHD_RX_COMPLETE_DESCRIPTOR_COMMON_HDR_FLAGS_READ(val, desc_ptr);
            bdmf_session_print(session, "compl_msg_hdr_status     = 0x%4.4x\n", val);
            RDD_DHD_RX_COMPLETE_DESCRIPTOR_FLOW_RING_ID_READ(val, desc_ptr);
            bdmf_session_print(session, "flow_ring_id             = 0x%4.4x\n", __swap2bytes(val));
            RDD_DHD_RX_COMPLETE_DESCRIPTOR_DATA_LEN_READ(val, desc_ptr);
            bdmf_session_print(session, "data_len                 = 0x%4.4x\n", __swap2bytes(val));
            RDD_DHD_RX_COMPLETE_DESCRIPTOR_DATA_OFFSET_READ(val, desc_ptr);
            bdmf_session_print(session, "data_offset              = 0x%4.4x\n", __swap2bytes(val));
            RDD_DHD_RX_COMPLETE_DESCRIPTOR_FLAGS_READ(val, desc_ptr);
            bdmf_session_print(session, "flags                    = 0x%4.4x\n", val);
            RDD_DHD_RX_COMPLETE_DESCRIPTOR_RX_STATUS_0_READ(val, desc_ptr);
            bdmf_session_print(session, "rx_status_0              = 0x%8.8x\n", val);
            RDD_DHD_RX_COMPLETE_DESCRIPTOR_RX_STATUS_1_READ(val, desc_ptr);
            bdmf_session_print(session, "rx_status_1	          = 0x%8.8x\n", val);
            RDD_DHD_RX_COMPLETE_DESCRIPTOR_DMA_DONE_MARK_READ(val, desc_ptr);
            bdmf_session_print(session, "dma_done_mark 	          = 0x%8.8x\n", val);        
            break;
        }

        case FR_FORMAT_WI_CWI32: /* Compact Work Item with 32b haddr */
        {                           
            desc_ptr = (RDD_DHD_RX_COMPLETE_DESCRIPTOR_CWI32_DTS *)RDD_RSV_PHYS_TO_VIRT(base_ptr) + desc_num;
            bdmf_session_print(session, "RX COMPLETE descriptor\n");
            RDD_DHD_RX_COMPLETE_DESCRIPTOR_CWI32_IF_ID_READ(val, desc_ptr);
            bdmf_session_print(session, "if_id                    = 0x%4.4x\n", val);
            RDD_DHD_RX_COMPLETE_DESCRIPTOR_CWI32_REQUEST_ID_READ(val, desc_ptr);
            bdmf_session_print(session, "request_id               = 0x%8.8x\n", val);
            RDD_DHD_RX_COMPLETE_DESCRIPTOR_CWI32_DATA_LEN_READ(val, desc_ptr);
            bdmf_session_print(session, "data_len                 = 0x%4.4x\n", __swap2bytes(val));
            RDD_DHD_RX_COMPLETE_DESCRIPTOR_CWI32_DATA_OFFSET_READ(val, desc_ptr);
            bdmf_session_print(session, "data_offset              = 0x%4.4x\n", __swap2bytes(val));
            RDD_DHD_RX_COMPLETE_DESCRIPTOR_CWI32_FLAGS_READ(val, desc_ptr);
            bdmf_session_print(session, "flags                    = 0x%4.4x\n", val);      
            break;
        }        
    } /* switch item_type */
    
    return 0;
}

static int _rdd_print_dhd_rx_post_desc(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY_DTS  *radio_instance_entry_ptr;
    void                                     *desc_ptr;
    uint32_t                                 desc_num, radio_idx;
    bdmf_phys_addr_t                         base_ptr;
    uint32_t                                 addr_lo,addr_hi, val;
    
    
    radio_idx = (uint32_t)parm[0].value.unumber;
    desc_num = (uint32_t)parm[1].value.unumber;
   
    radio_instance_entry_ptr = get_dhd_complete_radio_entry(radio_idx);

    RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY_RX_POST_FR_BASE_PTR_LOW_READ(addr_lo, radio_instance_entry_ptr);
    RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY_RX_POST_FR_BASE_PTR_HIGH_READ(addr_hi, radio_instance_entry_ptr);
    
#ifdef PHYS_ADDR_64BIT
    base_ptr = ((uint64_t)addr_hi << 32) + addr_lo;
#else
    addr_hi = 0;
    base_ptr = addr_lo | addr_hi;            
#endif
            
    switch (flow_ring_format[radio_idx])
    {           
        case FR_FORMAT_WI_WI64: /* Legacy Work Item */
        {        
            desc_ptr = (RDD_DHD_RX_POST_DESCRIPTOR_DTS *)RDD_RSV_PHYS_TO_VIRT(base_ptr) + desc_num;
            
            bdmf_session_print(session, "RX POST descriptor\n");
            RDD_DHD_RX_POST_DESCRIPTOR_MSG_TYPE_READ(val, desc_ptr);
            bdmf_session_print(session, "msg_type                 = 0x%4.4x\n", val);
            RDD_DHD_RX_POST_DESCRIPTOR_IF_ID_READ(val, desc_ptr);
            bdmf_session_print(session, "if_id                    = 0x%4.4x\n", val);
            RDD_DHD_RX_POST_DESCRIPTOR_COMMON_HDR_FLAGS_READ(val, desc_ptr);
            bdmf_session_print(session, "common_hdr_flags         = 0x%4.4x\n", val);
            RDD_DHD_RX_POST_DESCRIPTOR_EPOCH_READ(val, desc_ptr);
            bdmf_session_print(session, "epoch                    = 0x%4.4x\n", val);    
            RDD_DHD_RX_POST_DESCRIPTOR_REQUEST_ID_READ(val, desc_ptr);
            bdmf_session_print(session, "request_id               = 0x%8.8x\n", val);    
            RDD_DHD_RX_POST_DESCRIPTOR_META_BUF_LEN_READ(val, desc_ptr);
            bdmf_session_print(session, "meta_buf_len             = 0x%4.4x\n", val);
            RDD_DHD_RX_POST_DESCRIPTOR_DATA_LEN_READ(val, desc_ptr);
            bdmf_session_print(session, "data_len                 = 0x%4.4x\n", __swap2bytes(val));
            RDD_DHD_RX_POST_DESCRIPTOR_METADATA_BUF_ADDR_LOW_READ(val, desc_ptr);
            bdmf_session_print(session, "metadata_buf_addr_low    = 0x%8.8x\n", val);
            RDD_DHD_RX_POST_DESCRIPTOR_METADATA_BUF_ADDR_HI_READ(val, desc_ptr);
            bdmf_session_print(session, "metadata_buf_addr_hi     = 0x%8.8x\n", val);    
            RDD_DHD_RX_POST_DESCRIPTOR_DATA_BUF_ADDR_LOW_READ(val, desc_ptr);
            bdmf_session_print(session, "data_buf_addr_low        = 0x%8.8x\n", __swap4bytes(val));    
            RDD_DHD_RX_POST_DESCRIPTOR_DATA_BUF_ADDR_HI_READ(val, desc_ptr);
            bdmf_session_print(session, "data_buf_addr_hi         = 0x%8.8x\n", __swap4bytes(val));        
            break;
        }

        case FR_FORMAT_WI_CWI32: /* Compact Work Item with 32b haddr */
        {                  
            desc_ptr = (RDD_DHD_RX_POST_DESCRIPTOR_CWI32_DTS *)RDD_RSV_PHYS_TO_VIRT(base_ptr) + desc_num;
            RDD_DHD_RX_POST_DESCRIPTOR_CWI32_REQUEST_ID_READ(val, desc_ptr);
            bdmf_session_print(session, "request_id               = 0x%8.8x\n", val);       
            RDD_DHD_RX_POST_DESCRIPTOR_CWI32_DATA_BUF_ADDR_LOW_READ(val, desc_ptr);
            bdmf_session_print(session, "data_buf_addr            = 0x%8.8x\n", __swap4bytes(val));          
            break;
        }        
    } /* switch item_type */
    
    

    return 0;
}


static int _rdd_print_dhd_general_counters(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    uint32_t radio_idx;
    uint32_t cntr_arr[MAX_NUM_OF_COUNTERS_PER_READ] = {};
    
    radio_idx = (uint32_t)parm[0].value.unumber;
    
    bdmf_session_print(session, "General DHD counters\n");


    bdmf_session_print(session, "Radio %d\n", radio_idx);                
    
    drv_cntr_counter_read(GENERAL_CNTR_GROUP_ID, GENERAL_COUNTER_DHD_TX_POST_PKTS_0 + radio_idx, cntr_arr);
    bdmf_session_print(session, "\tTX_POST packets = %d\n", cntr_arr[0]);
    
    drv_cntr_counter_read(GENERAL_CNTR_GROUP_ID, GENERAL_COUNTER_DHD_TX_COMPLETE_PKTS_0 + radio_idx, cntr_arr);
    bdmf_session_print(session, "\tTX_COMPLETE packets = %d\n", cntr_arr[0]);
    
    drv_cntr_counter_read(GENERAL_CNTR_GROUP_ID, GENERAL_COUNTER_DHD_RX_COMPLETE_PKTS_0 + radio_idx, cntr_arr);
    bdmf_session_print(session, "\tRX_COMPLETE packets = %d\n", cntr_arr[0]);        

    drv_cntr_counter_read(VARIOUS_CNTR_GROUP_ID, COUNTER_DHD_TX_FPM_USED_0 + radio_idx, cntr_arr);
    bdmf_session_print(session, "\tFPM in use = %d\n", cntr_arr[0]);
   
    return 0;
}


static int _rdd_print_dhd_drop_counters(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    uint32_t radio_idx, i, cntr_id;
    uint32_t cntr_arr[MAX_NUM_OF_COUNTERS_PER_READ] = {};
    
    radio_idx = (uint32_t)parm[0].value.unumber;
    
    bdmf_session_print(session, "SSID drop counters\n");

    bdmf_session_print(session, "Radio %d\n", radio_idx);                    
    for (i = 0; i < 16; i++)
    {
        cntr_id = COUNTER_DHD_TX_DROP_0_SSID_0 + radio_idx*16 + i;
        drv_cntr_counter_read(VARIOUS_CNTR_GROUP_ID, cntr_id, cntr_arr);
        bdmf_session_print(session, "\tSSID%d = %d\n", i, cntr_arr[0]);
    }

    bdmf_session_print(session, "AC Flow Ring Full counters\n");


    bdmf_session_print(session, "Radio %d\n", radio_idx);                
    for (i = 0; i < 5; i++)
    {
        cntr_id = COUNTER_DHD_TX_FR_FULL_0_AC_0 + radio_idx*5 + i;
        drv_cntr_counter_read(VARIOUS_CNTR_GROUP_ID, cntr_id, cntr_arr);
        bdmf_session_print(session, "\tAC%d = %d\n", i, cntr_arr[0]);
    }

    cntr_id = COUNTER_DHD_RX_DROP_0 + radio_idx;
    drv_cntr_counter_read(VARIOUS_CNTR_GROUP_ID, cntr_id, cntr_arr);
    bdmf_session_print(session, "FPM alloc failed for radio %d = %d\n", radio_idx, cntr_arr[0]);
    
    cntr_id = COUNTER_DHD_TX_FPM_DROP_0 + radio_idx;
    drv_cntr_counter_read(VARIOUS_CNTR_GROUP_ID, cntr_id, cntr_arr);
    bdmf_session_print(session, "FPM congestion drop for radio %d = %d\n", radio_idx, cntr_arr[0]);

    return 0;
}

static int _rdd_print_dhd_flow_rings_cache(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    uint32_t radio_idx,i;
    RDD_DHD_FLOW_RING_CACHE_LKP_ENTRY_DTS *cache_lkp_ptr, cache_lkp;
    RDD_DHD_FLOW_RING_CACHE_CTX_ENTRY_DTS *cache_ctx_ptr, ctx_entry;

    radio_idx = (uint32_t)parm[0].value.unumber;
    
    bdmf_session_print(session, "FlowRings Cache\n");

    cache_lkp_ptr = (RDD_DHD_FLOW_RING_CACHE_LKP_ENTRY_DTS *)RDD_DHD_FLOW_RING_CACHE_LKP_TABLE_PTR(get_runner_idx(dhd_tx_post_runner_image));
    cache_ctx_ptr = (RDD_DHD_FLOW_RING_CACHE_CTX_ENTRY_DTS *)RDD_DHD_FLOW_RING_CACHE_CTX_TABLE_PTR(get_runner_idx(dhd_tx_post_runner_image));

    bdmf_session_print(session, "Radio   Ring ID  Inv  Base        Size    Flags       SSID   RD_Idx  WR_Idx\n");
    bdmf_session_print(session, "=================================================================\n");

    cache_lkp_ptr += radio_idx*16;
    cache_ctx_ptr += radio_idx*16;
    
    for (i = 0; i < 16; i++, cache_lkp_ptr++, cache_ctx_ptr++)
    {
        /* Read Flowring cache entry */
        RDD_DHD_FLOW_RING_CACHE_LKP_ENTRY_FLOW_RING_ID_READ(cache_lkp.flow_ring_id, cache_lkp_ptr);        
        RDD_DHD_FLOW_RING_CACHE_LKP_ENTRY_INVALID_READ(cache_lkp.invalid, cache_lkp_ptr);
        bdmf_session_print(session, "%-5d   %-7d  %-3d  ", (i >> 4), cache_lkp.flow_ring_id, cache_lkp.invalid);

        /* Read FlowRing context entry */
        RDD_DHD_FLOW_RING_CACHE_CTX_ENTRY_FLOW_RING_BASE_LOW_READ(ctx_entry.flow_ring_base_low, cache_ctx_ptr);
        RDD_DHD_FLOW_RING_CACHE_CTX_ENTRY_SIZE_READ(ctx_entry.size, cache_ctx_ptr);
        RDD_DHD_FLOW_RING_CACHE_CTX_ENTRY_FLAGS_READ(ctx_entry.flags, cache_ctx_ptr);
        RDD_DHD_FLOW_RING_CACHE_CTX_ENTRY_SSID_READ(ctx_entry.ssid, cache_ctx_ptr);
        RDD_DHD_FLOW_RING_CACHE_CTX_ENTRY_RD_IDX_READ(ctx_entry.rd_idx, cache_ctx_ptr);
        RDD_DHD_FLOW_RING_CACHE_CTX_ENTRY_WR_IDX_READ(ctx_entry.wr_idx, cache_ctx_ptr);
        bdmf_session_print(session, "0x%-8x  %-5d   0x%-8x  %-4d  %-6d  %-6d\n",
            ctx_entry.flow_ring_base_low, ctx_entry.size, ctx_entry.flags, ctx_entry.ssid, ctx_entry.rd_idx, ctx_entry.wr_idx);
    }

    return 0;
}

static int _rdd_set_dhd_fpm_th(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    uint16_t  low_th, high_th;
       
    low_th = (uint32_t)parm[0].value.unumber;
    high_th = (uint32_t)parm[1].value.unumber;
    
    rdd_dhd_helper_fpm_thresholds_set(low_th, high_th);
    
    return 0;
}

static int _rdd_get_dhd_fpm_th(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    uint16_t  low_th, high_th;

    rdd_dhd_helper_fpm_thresholds_get(&low_th, &high_th);
    
    bdmf_session_print(session, "DHD FPM low threshold = %d, high threshold = %d\n", low_th, high_th);
    
    return 0;    
}


void rdd_dhd_helper_shell_cmds_init(bdmfmon_handle_t rdd_dir)
{
    MAKE_BDMF_SHELL_CMD(rdd_dir, "pdtxp", "print DHD TX POST descriptor", _rdd_print_dhd_tx_post_desc,
        BDMFMON_MAKE_PARM("radio_idx", "radio index", BDMFMON_PARM_NUMBER, 0),
        BDMFMON_MAKE_PARM("ring_id", "ring id", BDMFMON_PARM_NUMBER, 0),
        BDMFMON_MAKE_PARM("desc_id", "desc id", BDMFMON_PARM_NUMBER, 0));
    MAKE_BDMF_SHELL_CMD(rdd_dir, "pdrxp",   "print DHD RX POST descriptor", _rdd_print_dhd_rx_post_desc,
        BDMFMON_MAKE_PARM("radio_idx", "radio index", BDMFMON_PARM_NUMBER, 0),
        BDMFMON_MAKE_PARM("desc_id", "desc id", BDMFMON_PARM_NUMBER, 0));
    MAKE_BDMF_SHELL_CMD(rdd_dir, "pdtxc",   "print DHD TX COMPLETE descriptor", _rdd_print_dhd_tx_complete_desc,
        BDMFMON_MAKE_PARM("radio_idx", "radio index", BDMFMON_PARM_NUMBER, 0),
        BDMFMON_MAKE_PARM("desc_id", "desc id", BDMFMON_PARM_NUMBER, 0));
    MAKE_BDMF_SHELL_CMD(rdd_dir, "pdrxc",   "print DHD RX COMPLETE descriptor", _rdd_print_dhd_rx_complete_desc,
        BDMFMON_MAKE_PARM("radio_idx", "radio index", BDMFMON_PARM_NUMBER, 0),
        BDMFMON_MAKE_PARM("desc_id", "desc id", BDMFMON_PARM_NUMBER, 0));  
    MAKE_BDMF_SHELL_CMD(rdd_dir, "pddc",   "print DHD drop counters", _rdd_print_dhd_drop_counters,
        BDMFMON_MAKE_PARM("radio_idx", "radio index", BDMFMON_PARM_NUMBER, 0));
    MAKE_BDMF_SHELL_CMD(rdd_dir, "pdfrc",   "print DHD Flow Rings cache", _rdd_print_dhd_flow_rings_cache,
        BDMFMON_MAKE_PARM("radio_idx", "radio index", BDMFMON_PARM_NUMBER, 0));
    MAKE_BDMF_SHELL_CMD(rdd_dir, "pdgc",   "print DHD general counters", _rdd_print_dhd_general_counters,
        BDMFMON_MAKE_PARM("radio_idx", "radio index", BDMFMON_PARM_NUMBER, 0));
    MAKE_BDMF_SHELL_CMD(rdd_dir, "sdfl",   "set DHD FPM thresholds", _rdd_set_dhd_fpm_th,
        BDMFMON_MAKE_PARM("low_prio_limit", "low_prio_limit", BDMFMON_PARM_NUMBER, 0),
        BDMFMON_MAKE_PARM("high_prio_limit", "high_prio_limit", BDMFMON_PARM_NUMBER, 0));
    MAKE_BDMF_SHELL_CMD_NOPARM(rdd_dir, "gdfl",  "get DHD FPM thresholds", _rdd_get_dhd_fpm_th);
}


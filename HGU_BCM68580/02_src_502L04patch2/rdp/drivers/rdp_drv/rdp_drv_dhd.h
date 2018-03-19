/*
    <:copyright-BRCM:2015-2016:DUAL/GPL:standard
    
       Copyright (c) 2015-2016 Broadcom 
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
#ifndef _RDP_DRV_DHD_H_
#define _RDP_DRV_DHD_H_

#include "rdd_dhd_helper.h"


void rdp_drv_dhd_skb_fifo_tbl_init(void);
int rdp_drv_dhd_helper_flow_ring_flush(uint32_t radio_idx, uint32_t flow_ring_idx);
int rdp_drv_dhd_helper_flow_ring_disable(uint32_t radio_idx, uint32_t flow_ring_idx);
void rdp_drv_dhd_helper_wakeup_information_get(rdpa_dhd_wakeup_info_t *wakeup_info);
int rdp_drv_dhd_helper_dhd_complete_ring_create(uint32_t radio_idx, uint32_t ring_size);
int rdp_drv_dhd_helper_dhd_complete_ring_destroy(uint32_t radio_idx, uint32_t ring_size);
uint16_t rdp_drv_dhd_helper_ssid_tx_dropped_packets_get(uint32_t radio_idx, uint32_t ssid);
int rdp_drv_dhd_cpu_tx(const rdpa_dhd_tx_post_info_t *info, void *buffer, uint32_t  pkt_length);
int rdp_drv_dhd_helper_dhd_complete_message_get(rdpa_dhd_complete_data_t *dhd_complete_info);
int rdp_drv_rx_post_init(uint32_t radio_idx, rdpa_dhd_init_cfg_t *init_cfg);
int rdp_drv_dhd_rx_post_uninit(uint32_t radio_idx, rdpa_dhd_init_cfg_t *init_cfg);

#endif

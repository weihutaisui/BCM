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

#ifndef _RDD_DHD_HELPER_H
#define _RDD_DHD_HELPER_H



#include "rdd_dhd_helper_common.h"
#include "rdp_cpu_ring.h"
#include "rdp_drv_rnr.h"
#include "rdp_drv_proj_cntr.h"
#include "rdp_drv_fpm.h"
#include "rdp_drv_qm.h"
#include "rdp_mm.h"
#include "rdd_init.h"


#define RDD_CPU_TX_SKB_LIMIT_DEFAULT                        4096
#define RDD_CPU_TX_SKB_INDEX_MASK                           0x1FFF


void rdd_dhd_hw_cfg(RDD_DHD_HW_CONFIGURATION_DTS *dhd_hw_config);
void rdd_rx_post_descr_init(uint32_t radio_idx, uint8_t *descr_ptr, uint32_t fpm_buffer_number);
void rdd_complete_ring_init(uint32_t radio_idx, RING_DESCTIPTOR  *descriptor, bdmf_phys_addr_t phy_addr);
int rdd_dhd_helper_fpm_thresholds_set(uint16_t low_th, uint16_t high_th);
int rdd_dhd_helper_fpm_thresholds_get(uint16_t *low_th, uint16_t *high_th);

extern int flow_ring_format[RDPA_MAX_RADIOS];

/* Waking up TX complete or RX complete threads */
static inline void rdd_dhd_helper_wakeup(uint32_t radio_idx, bdmf_boolean is_tx_complete)
{
#ifndef RDP_SIM   
   if (is_tx_complete)
      ag_drv_rnr_regs_cfg_cpu_wakeup_set(get_runner_idx(dhd_complete_runner_image), DHD_TX_COMPLETE_0_THREAD_NUMBER + radio_idx);
   else
      ag_drv_rnr_regs_cfg_cpu_wakeup_set(get_runner_idx(dhd_complete_runner_image), DHD_RX_COMPLETE_0_THREAD_NUMBER + radio_idx);   
#else 
  if (is_tx_complete)
      rdp_cpu_runner_wakeup(get_runner_idx(dhd_complete_runner_image), DHD_TX_COMPLETE_0_THREAD_NUMBER + radio_idx);
   else
      rdp_cpu_runner_wakeup(get_runner_idx(dhd_complete_runner_image), DHD_RX_COMPLETE_0_THREAD_NUMBER + radio_idx);          
#endif   
}

void rdd_wlan_mcast_dft_init(bdmf_phys_addr_t dft_phys_addr);

#endif /* _RDD_DHD_HELPER_H */


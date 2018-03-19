/*
   Copyright (c) 2015 Broadcom Corporation
   All Rights Reserved

    <:label-BRCM:2015:DUAL/GPL:standard

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

#ifndef _BCM6858_GPON_RX_AMD2_AG_H_
#define _BCM6858_GPON_RX_AMD2_AG_H_

#include "access_macros.h"
#include "bdmf_interface.h"
#ifdef USE_BDMF_SHELL
#include "bdmf_shell.h"
#endif

int ag_drv_gpon_rx_amd2_tod_sf_up_set(uint32_t up_sf);
int ag_drv_gpon_rx_amd2_tod_sf_up_get(uint32_t *up_sf);
int ag_drv_gpon_rx_amd2_tod_up_val_set(uint32_t word_id, uint32_t tod_val);
int ag_drv_gpon_rx_amd2_tod_up_val_get(uint32_t gpon_rx_amd2_tod_up_val, uint32_t *tod_val);
int ag_drv_gpon_rx_amd2_up_arm_set(uint8_t uparm, uint8_t en, uint8_t rdarm);
int ag_drv_gpon_rx_amd2_up_arm_get(uint8_t *uparm, uint8_t *en, uint8_t *rdarm);
int ag_drv_gpon_rx_amd2_tod_cyc_in_sec_set(uint32_t cyc_in_sec);
int ag_drv_gpon_rx_amd2_tod_cyc_in_sec_get(uint32_t *cyc_in_sec);
int ag_drv_gpon_rx_amd2_tod_opps_duty_set(uint32_t duty_cycle, uint8_t opps_en);
int ag_drv_gpon_rx_amd2_tod_opps_duty_get(uint32_t *duty_cycle, uint8_t *opps_en);
int ag_drv_gpon_rx_amd2_nanos_per_clk_set(uint32_t ns_in_clk);
int ag_drv_gpon_rx_amd2_nanos_per_clk_get(uint32_t *ns_in_clk);
int ag_drv_gpon_rx_amd2_nspartinc_set(uint32_t nspinc);
int ag_drv_gpon_rx_amd2_nspartinc_get(uint32_t *nspinc);
int ag_drv_gpon_rx_amd2_nsp_in_nano_set(uint32_t nsp_in_nano);
int ag_drv_gpon_rx_amd2_nsp_in_nano_get(uint32_t *nsp_in_nano);
int ag_drv_gpon_rx_amd2_ts_up_val_set(uint32_t word_id, uint32_t ts_val);
int ag_drv_gpon_rx_amd2_ts_up_val_get(uint32_t word_id, uint32_t *ts_val);
int ag_drv_gpon_rx_amd2_tod_rd_val_get(uint32_t word_id, uint32_t *ts_val);
int ag_drv_gpon_rx_amd2_ts_rd_val_get(uint32_t word_id, uint32_t *ts_val);



#endif


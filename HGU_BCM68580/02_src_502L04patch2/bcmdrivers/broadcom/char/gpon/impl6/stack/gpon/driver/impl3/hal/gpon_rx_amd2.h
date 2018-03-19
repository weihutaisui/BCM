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

#ifndef _GPON_RX_AMD2_AG_H_
#define _GPON_RX_AMD2_AG_H_

#include "access_macros.h"
#include "bdmf_interface.h"
#ifdef USE_BDMF_SHELL
#include "bdmf_shell.h"
#endif
#include "rdp_common.h"

bdmf_error_t ag_drv_gpon_rx_amd2_tod_sf_up_set(uint32_t up_sf);
bdmf_error_t ag_drv_gpon_rx_amd2_tod_sf_up_get(uint32_t *up_sf);
bdmf_error_t ag_drv_gpon_rx_amd2_tod_up_val_set(uint32_t word_id, uint32_t tod_val);
bdmf_error_t ag_drv_gpon_rx_amd2_tod_up_val_get(uint32_t word_id, uint32_t *tod_val);
bdmf_error_t ag_drv_gpon_rx_amd2_up_arm_set(bdmf_boolean uparm, bdmf_boolean en, bdmf_boolean rdarm);
bdmf_error_t ag_drv_gpon_rx_amd2_up_arm_get(bdmf_boolean *uparm, bdmf_boolean *en, bdmf_boolean *rdarm);
bdmf_error_t ag_drv_gpon_rx_amd2_tod_cyc_in_sec_set(uint32_t cyc_in_sec);
bdmf_error_t ag_drv_gpon_rx_amd2_tod_cyc_in_sec_get(uint32_t *cyc_in_sec);
bdmf_error_t ag_drv_gpon_rx_amd2_tod_opps_duty_set(uint32_t duty_cycle, bdmf_boolean opps_en);
bdmf_error_t ag_drv_gpon_rx_amd2_tod_opps_duty_get(uint32_t *duty_cycle, bdmf_boolean *opps_en);
bdmf_error_t ag_drv_gpon_rx_amd2_nanos_per_clk_set(uint32_t ns_in_clk);
bdmf_error_t ag_drv_gpon_rx_amd2_nanos_per_clk_get(uint32_t *ns_in_clk);
bdmf_error_t ag_drv_gpon_rx_amd2_nspartinc_set(uint32_t nspinc);
bdmf_error_t ag_drv_gpon_rx_amd2_nspartinc_get(uint32_t *nspinc);
bdmf_error_t ag_drv_gpon_rx_amd2_nsp_in_nano_set(uint32_t nsp_in_nano);
bdmf_error_t ag_drv_gpon_rx_amd2_nsp_in_nano_get(uint32_t *nsp_in_nano);
bdmf_error_t ag_drv_gpon_rx_amd2_ts_up_val_set(uint32_t word_id, uint32_t ts_val);
bdmf_error_t ag_drv_gpon_rx_amd2_ts_up_val_get(uint32_t word_id, uint32_t *ts_val);
bdmf_error_t ag_drv_gpon_rx_amd2_tod_rd_val_get(uint32_t word_id, uint32_t *ts_val);
bdmf_error_t ag_drv_gpon_rx_amd2_ts_rd_val_get(uint32_t word_id, uint32_t *ts_val);

#ifdef USE_BDMF_SHELL
bdmfmon_handle_t ag_drv_gpon_rx_amd2_cli_init(bdmfmon_handle_t driver_dir);
#endif


#endif


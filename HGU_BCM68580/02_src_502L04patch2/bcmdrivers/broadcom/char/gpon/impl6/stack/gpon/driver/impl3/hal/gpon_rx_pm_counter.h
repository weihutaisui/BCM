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

#ifndef _GPON_RX_PM_COUNTER_AG_H_
#define _GPON_RX_PM_COUNTER_AG_H_

#include "access_macros.h"
#include "bdmf_interface.h"
#ifdef USE_BDMF_SHELL
#include "bdmf_shell.h"
#endif
#include "rdp_common.h"

bdmf_error_t ag_drv_gpon_rx_pm_counter_bwmaperr_get(uint32_t *bwmaperr);
bdmf_error_t ag_drv_gpon_rx_pm_counter_ploamerr_get(uint32_t *ploamerr);
bdmf_error_t ag_drv_gpon_rx_pm_counter_plenderr_get(uint32_t *plenderr);
bdmf_error_t ag_drv_gpon_rx_pm_counter_sfmismatch_get(uint32_t *sfmismatch);
bdmf_error_t ag_drv_gpon_rx_pm_counter_bip_get(uint32_t *bip);
bdmf_error_t ag_drv_gpon_rx_pm_counter_gemfrag_get(uint32_t *gemfrag);
bdmf_error_t ag_drv_gpon_rx_pm_counter_ethpack_get(uint32_t *ethpack);
bdmf_error_t ag_drv_gpon_rx_pm_counter_tdmfrag_get(uint32_t *tdmfrag);
bdmf_error_t ag_drv_gpon_rx_pm_counter_symerror_get(uint32_t *symerrors);
bdmf_error_t ag_drv_gpon_rx_pm_counter_biterror_get(uint32_t *biterrors);
bdmf_error_t ag_drv_gpon_rx_pm_counter_aeserror_get(uint32_t *aeserrcnt);
bdmf_error_t ag_drv_gpon_rx_pm_counter_tdmstats_get(uint32_t *rcvbytes);
bdmf_error_t ag_drv_gpon_rx_pm_counter_ethfrag_get(uint32_t *ethfrag);
bdmf_error_t ag_drv_gpon_rx_pm_counter_ethbytecnt_get(uint32_t *ethbytecnt);
bdmf_error_t ag_drv_gpon_rx_pm_counter_fecuccwcnt_get(uint16_t *fecuccwcnt);
bdmf_error_t ag_drv_gpon_rx_pm_counter_fecccwcnt_get(uint32_t *fecccwcnt);
bdmf_error_t ag_drv_gpon_rx_pm_counter_plonucnt_get(uint32_t *pl_onu_cnt);
bdmf_error_t ag_drv_gpon_rx_pm_counter_plbcstcnt_get(uint32_t *pl_bcst_cnt);
bdmf_error_t ag_drv_gpon_rx_pm_counter_bw_cnt_get(uint32_t tcont_idx, uint32_t *bw_cnt);

#ifdef USE_BDMF_SHELL
bdmfmon_handle_t ag_drv_gpon_rx_pm_counter_cli_init(bdmfmon_handle_t driver_dir);
#endif


#endif


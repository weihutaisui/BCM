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

#ifndef _GPON_TX_STATISTICS_COUNTERS_AG_H_
#define _GPON_TX_STATISTICS_COUNTERS_AG_H_

#include "access_macros.h"
#include "bdmf_interface.h"
#ifdef USE_BDMF_SHELL
#include "bdmf_shell.h"
#endif
#include "rdp_common.h"

bdmf_error_t ag_drv_gpon_tx_statistics_counters_iac_get(uint16_t *iacc);
bdmf_error_t ag_drv_gpon_tx_statistics_counters_rac_get(uint32_t tx_q, uint16_t *racc);
bdmf_error_t ag_drv_gpon_tx_statistics_counters_rdbc_get(uint32_t tx_q, uint16_t *rdbcc);
bdmf_error_t ag_drv_gpon_tx_statistics_counters_tpc_get(uint32_t tx_q, uint32_t *tpcc);
bdmf_error_t ag_drv_gpon_tx_statistics_counters_tgc_get(uint32_t tx_q, uint32_t *tgcc);
bdmf_error_t ag_drv_gpon_tx_statistics_counters_tic_get(uint32_t tx_q, uint32_t *tic);
bdmf_error_t ag_drv_gpon_tx_statistics_counters_ripc_get(uint32_t *ipcc);
bdmf_error_t ag_drv_gpon_tx_statistics_counters_rnpc_get(uint32_t *rnpcc);
bdmf_error_t ag_drv_gpon_tx_statistics_counters_rspc_get(uint16_t *rupcc, uint16_t *rrpcc);

#ifdef USE_BDMF_SHELL
bdmfmon_handle_t ag_drv_gpon_tx_statistics_counters_cli_init(bdmfmon_handle_t driver_dir);
#endif


#endif


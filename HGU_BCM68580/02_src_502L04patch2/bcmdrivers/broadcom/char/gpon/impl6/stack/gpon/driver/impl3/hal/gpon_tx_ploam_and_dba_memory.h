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

#ifndef _GPON_TX_PLOAM_AND_DBA_MEMORY_AG_H_
#define _GPON_TX_PLOAM_AND_DBA_MEMORY_AG_H_

#include "access_macros.h"
#include "bdmf_interface.h"
#ifdef USE_BDMF_SHELL
#include "bdmf_shell.h"
#endif
#include "rdp_common.h"

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_rplm_set(uint32_t word_idx, uint8_t rplmc);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_rplm_get(uint32_t word_idx, uint8_t *rplmc);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_iplm_set(uint32_t word_idx, uint8_t iplc);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_iplm_get(uint32_t word_idx, uint8_t *iplc);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_uplm_set(uint32_t word_idx, uint8_t uplmc);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_uplm_get(uint32_t word_idx, uint8_t *uplmc);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_nplm3_set(uint32_t word_idx, uint8_t nplmc);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_nplm3_get(uint32_t word_idx, uint8_t *nplmc);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_nplm2_set(uint32_t word_idx, uint8_t nplmc);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_nplm2_get(uint32_t word_idx, uint8_t *nplmc);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_nplm1_set(uint32_t word_idx, uint8_t nplmc);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_nplm1_get(uint32_t word_idx, uint8_t *nplmc);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_nplm0_set(uint32_t word_idx, uint8_t nplmc);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_nplm0_get(uint32_t word_idx, uint8_t *nplmc);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_idbr_set(uint32_t idbcc);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_idbr_get(uint32_t *idbcc);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_idbrbyte5_set(uint8_t idbr);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_idbrbyte5_get(uint8_t *idbr);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr0_set(uint32_t ndbc);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr0_get(uint32_t *ndbc);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr0byte5_set(uint8_t ndbcbyte5);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr0byte5_get(uint8_t *ndbcbyte5);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr1_set(uint32_t ndbc);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr1_get(uint32_t *ndbc);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr1byte5_set(uint8_t ndbcbyte5);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr1byte5_get(uint8_t *ndbcbyte5);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr2_set(uint32_t ndbc);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr2_get(uint32_t *ndbc);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr2byte5_set(uint8_t ndbcbyte5);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr2byte5_get(uint8_t *ndbcbyte5);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr3_set(uint32_t ndbc);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr3_get(uint32_t *ndbc);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr3byte5_set(uint8_t ndbcbyte5);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr3byte5_get(uint8_t *ndbcbyte5);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr4_set(uint32_t ndbc);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr4_get(uint32_t *ndbc);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr4byte5_set(uint8_t ndbcbyte5);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr4byte5_get(uint8_t *ndbcbyte5);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr5_set(uint32_t ndbc);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr5_get(uint32_t *ndbc);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr5byte5_set(uint8_t ndbcbyte5);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr5byte5_get(uint8_t *ndbcbyte5);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr6_set(uint32_t ndbc);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr6_get(uint32_t *ndbc);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr6byte5_set(uint8_t ndbcbyte5);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr6byte5_get(uint8_t *ndbcbyte5);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr7_set(uint32_t ndbc);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr7_get(uint32_t *ndbc);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr7byte5_set(uint8_t ndbcbyte5);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr7byte5_get(uint8_t *ndbcbyte5);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr8to39_set(uint32_t dbr_idx, uint32_t ndbc);
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr8to39_get(uint32_t dbr_idx, uint32_t *ndbc);

#ifdef USE_BDMF_SHELL
bdmfmon_handle_t ag_drv_gpon_tx_ploam_and_dba_memory_cli_init(bdmfmon_handle_t driver_dir);
#endif


#endif


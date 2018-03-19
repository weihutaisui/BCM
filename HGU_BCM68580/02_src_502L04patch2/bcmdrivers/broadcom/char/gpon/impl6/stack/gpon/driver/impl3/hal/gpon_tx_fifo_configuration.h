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

#ifndef _GPON_TX_FIFO_CONFIGURATION_AG_H_
#define _GPON_TX_FIFO_CONFIGURATION_AG_H_

#include "access_macros.h"
#include "bdmf_interface.h"
#ifdef USE_BDMF_SHELL
#include "bdmf_shell.h"
#endif
#include "rdp_common.h"

bdmf_error_t ag_drv_gpon_tx_fifo_configuration_pdp_set(uint32_t tx_q_idx, uint16_t pdbc, uint16_t pdsc);
bdmf_error_t ag_drv_gpon_tx_fifo_configuration_pdp_get(uint32_t tx_q_idx, uint16_t *pdbc, uint16_t *pdsc);
bdmf_error_t ag_drv_gpon_tx_fifo_configuration_pdpgrp_set(uint32_t tx_pd_idx, uint16_t pdbc_grp);
bdmf_error_t ag_drv_gpon_tx_fifo_configuration_pdpgrp_get(uint32_t tx_pd_idx, uint16_t *pdbc_grp);
bdmf_error_t ag_drv_gpon_tx_fifo_configuration_tqp_set(uint32_t tx_q_idx, uint16_t tqbc, uint16_t tqsc);
bdmf_error_t ag_drv_gpon_tx_fifo_configuration_tqp_get(uint32_t tx_q_idx, uint16_t *tqbc, uint16_t *tqsc);
bdmf_error_t ag_drv_gpon_tx_fifo_configuration_tdqpgrp_set(uint32_t tx_q_idx, uint16_t tdqb, uint16_t tdqs);
bdmf_error_t ag_drv_gpon_tx_fifo_configuration_tdqpgrp_get(uint32_t tx_q_idx, uint16_t *tdqb, uint16_t *tdqs);
bdmf_error_t ag_drv_gpon_tx_fifo_configuration_tdqp_set(uint16_t tdqpid, uint8_t tdq, uint8_t tdqpti);
bdmf_error_t ag_drv_gpon_tx_fifo_configuration_tdqp_get(uint16_t *tdqpid, uint8_t *tdq, uint8_t *tdqpti);
bdmf_error_t ag_drv_gpon_tx_fifo_configuration_lbp_set(uint8_t lbqn);
bdmf_error_t ag_drv_gpon_tx_fifo_configuration_lbp_get(uint8_t *lbqn);
bdmf_error_t ag_drv_gpon_tx_fifo_configuration_tth_set(uint16_t tdmmin, uint16_t tdmmax, bdmf_boolean tthen);
bdmf_error_t ag_drv_gpon_tx_fifo_configuration_tth_get(uint16_t *tdmmin, uint16_t *tdmmax, bdmf_boolean *tthen);

#ifdef USE_BDMF_SHELL
bdmfmon_handle_t ag_drv_gpon_tx_fifo_configuration_cli_init(bdmfmon_handle_t driver_dir);
#endif


#endif


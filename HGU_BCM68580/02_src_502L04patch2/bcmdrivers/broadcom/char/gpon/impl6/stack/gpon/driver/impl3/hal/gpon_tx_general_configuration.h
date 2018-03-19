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

#ifndef _GPON_TX_GENERAL_CONFIGURATION_AG_H_
#define _GPON_TX_GENERAL_CONFIGURATION_AG_H_

#include "access_macros.h"
#include "bdmf_interface.h"
#ifdef USE_BDMF_SHELL
#include "bdmf_shell.h"
#endif
#include "rdp_common.h"


/**************************************************************************************************/
/* twt2: Traffic_waiting_TCONT2 - Traffic waiting on TCONT type 2 indication bit                  */
/* twt3: Traffic_waiting_TCONT3 - Traffic waiting on TCONT type 3 indication                      */
/* twt4: Traffic_waiting_TCONT4 - Traffic waiting on TCONT type 4 indication                      */
/* twt5: Traffic_waiting_TCONT5 - Traffic waiting on TCONT type 5 indication                      */
/* rdi: Remote_defects_status - The RDI field is asserted by the software, if the OLT transmissio */
/*      n is received with defects at the ONU                                                     */
/**************************************************************************************************/
typedef struct
{
    bdmf_boolean twt2;
    bdmf_boolean twt3;
    bdmf_boolean twt4;
    bdmf_boolean twt5;
    bdmf_boolean rdi;
} gpon_tx_general_configuration_ind;

bdmf_error_t ag_drv_gpon_tx_general_configuration_onuid_set(uint8_t onuid);
bdmf_error_t ag_drv_gpon_tx_general_configuration_onuid_get(uint8_t *onuid);
bdmf_error_t ag_drv_gpon_tx_general_configuration_ind_set(const gpon_tx_general_configuration_ind *ind);
bdmf_error_t ag_drv_gpon_tx_general_configuration_ind_get(gpon_tx_general_configuration_ind *ind);
bdmf_error_t ag_drv_gpon_tx_general_configuration_prod_set(uint32_t word_idx, uint8_t prodc);
bdmf_error_t ag_drv_gpon_tx_general_configuration_prod_get(uint32_t word_idx, uint8_t *prodc);
bdmf_error_t ag_drv_gpon_tx_general_configuration_prods_set(uint8_t prcl, uint8_t prl);
bdmf_error_t ag_drv_gpon_tx_general_configuration_prods_get(uint8_t *prcl, uint8_t *prl);
bdmf_error_t ag_drv_gpon_tx_general_configuration_prrb_set(uint8_t brc);
bdmf_error_t ag_drv_gpon_tx_general_configuration_prrb_get(uint8_t *brc);
bdmf_error_t ag_drv_gpon_tx_general_configuration_delc_set(uint32_t deloc);
bdmf_error_t ag_drv_gpon_tx_general_configuration_delc_get(uint32_t *deloc);
bdmf_error_t ag_drv_gpon_tx_general_configuration_miscac1_set(uint16_t msstop, uint16_t msstart);
bdmf_error_t ag_drv_gpon_tx_general_configuration_miscac1_get(uint16_t *msstop, uint16_t *msstart);
bdmf_error_t ag_drv_gpon_tx_general_configuration_miscac2_set(bdmf_boolean ploamu, uint8_t dbru, bdmf_boolean fecu, uint8_t qu);
bdmf_error_t ag_drv_gpon_tx_general_configuration_miscac2_get(bdmf_boolean *ploamu, uint8_t *dbru, bdmf_boolean *fecu, uint8_t *qu);
bdmf_error_t ag_drv_gpon_tx_general_configuration_esall_set(bdmf_boolean esa);
bdmf_error_t ag_drv_gpon_tx_general_configuration_esall_get(bdmf_boolean *esa);
bdmf_error_t ag_drv_gpon_tx_general_configuration_qforcntr_set(uint8_t qforgrp0, uint8_t qforgrp1, uint8_t qforgrp2, uint8_t qforgrp3);
bdmf_error_t ag_drv_gpon_tx_general_configuration_qforcntr_get(uint8_t *qforgrp0, uint8_t *qforgrp1, uint8_t *qforgrp2, uint8_t *qforgrp3);
bdmf_error_t ag_drv_gpon_tx_general_configuration_plss_set(uint32_t plspat);
bdmf_error_t ag_drv_gpon_tx_general_configuration_plss_get(uint32_t *plspat);
bdmf_error_t ag_drv_gpon_tx_general_configuration_plsrs_set(uint8_t plsr);
bdmf_error_t ag_drv_gpon_tx_general_configuration_plsrs_get(uint8_t *plsr);
bdmf_error_t ag_drv_gpon_tx_general_configuration_bips_set(uint16_t bipenum, uint8_t bipm, bdmf_boolean bipev, bdmf_boolean bipef);
bdmf_error_t ag_drv_gpon_tx_general_configuration_bips_get(uint16_t *bipenum, uint8_t *bipm, bdmf_boolean *bipev, bdmf_boolean *bipef);
bdmf_error_t ag_drv_gpon_tx_general_configuration_crcoff_set(uint8_t crcoc);
bdmf_error_t ag_drv_gpon_tx_general_configuration_crcoff_get(uint8_t *crcoc);
bdmf_error_t ag_drv_gpon_tx_general_configuration_flush_set(uint8_t flq, bdmf_boolean flen, bdmf_boolean flimm);
bdmf_error_t ag_drv_gpon_tx_general_configuration_flush_get(uint8_t *flq, bdmf_boolean *flen, bdmf_boolean *flimm);
bdmf_error_t ag_drv_gpon_tx_general_configuration_flshdn_get(bdmf_boolean *fldn);
bdmf_error_t ag_drv_gpon_tx_general_configuration_rdyind_set(bdmf_boolean set, bdmf_boolean rdy);
bdmf_error_t ag_drv_gpon_tx_general_configuration_rdyind_get(bdmf_boolean *set, bdmf_boolean *rdy);
bdmf_error_t ag_drv_gpon_tx_general_configuration_dvslp_set(bdmf_boolean dvpol, bdmf_boolean dvexten);
bdmf_error_t ag_drv_gpon_tx_general_configuration_dvslp_get(bdmf_boolean *dvpol, bdmf_boolean *dvexten);
bdmf_error_t ag_drv_gpon_tx_general_configuration_dvstp_set(uint32_t dvstu);
bdmf_error_t ag_drv_gpon_tx_general_configuration_dvstp_get(uint32_t *dvstu);
bdmf_error_t ag_drv_gpon_tx_general_configuration_dvhld_set(uint32_t dvhld);
bdmf_error_t ag_drv_gpon_tx_general_configuration_dvhld_get(uint32_t *dvhld);
bdmf_error_t ag_drv_gpon_tx_general_configuration_dpen_set(bdmf_boolean dpen, bdmf_boolean prbs, uint8_t stplen, uint8_t hldlen);
bdmf_error_t ag_drv_gpon_tx_general_configuration_dpen_get(bdmf_boolean *dpen, bdmf_boolean *prbs, uint8_t *stplen, uint8_t *hldlen);
bdmf_error_t ag_drv_gpon_tx_general_configuration_dstp_set(uint32_t stppat);
bdmf_error_t ag_drv_gpon_tx_general_configuration_dstp_get(uint32_t *stppat);
bdmf_error_t ag_drv_gpon_tx_general_configuration_dhld_set(uint32_t hldpat);
bdmf_error_t ag_drv_gpon_tx_general_configuration_dhld_get(uint32_t *hldpat);
bdmf_error_t ag_drv_gpon_tx_general_configuration_rogue_onu_en_set(bdmf_boolean rogue_level_clr, bdmf_boolean rogue_diff_clr, bdmf_boolean rogue_dv_sel);
bdmf_error_t ag_drv_gpon_tx_general_configuration_rogue_onu_en_get(bdmf_boolean *rogue_level_clr, bdmf_boolean *rogue_diff_clr, bdmf_boolean *rogue_dv_sel);
bdmf_error_t ag_drv_gpon_tx_general_configuration_rogue_level_time_set(uint32_t window_size);
bdmf_error_t ag_drv_gpon_tx_general_configuration_rogue_level_time_get(uint32_t *window_size);
bdmf_error_t ag_drv_gpon_tx_general_configuration_rogue_diff_time_set(uint16_t window_size);
bdmf_error_t ag_drv_gpon_tx_general_configuration_rogue_diff_time_get(uint16_t *window_size);

#ifdef USE_BDMF_SHELL
bdmfmon_handle_t ag_drv_gpon_tx_general_configuration_cli_init(bdmfmon_handle_t driver_dir);
#endif


#endif


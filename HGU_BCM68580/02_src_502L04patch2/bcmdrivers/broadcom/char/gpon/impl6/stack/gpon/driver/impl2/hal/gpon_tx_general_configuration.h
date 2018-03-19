/*
* <:copyright-BRCM:2016:proprietary:gpon
* 
*    Copyright (c) 2016 Broadcom 
*    All Rights Reserved
* 
*  This program is the proprietary software of Broadcom and/or its
*  licensors, and may only be used, duplicated, modified or distributed pursuant
*  to the terms and conditions of a separate, written license agreement executed
*  between you and Broadcom (an "Authorized License").  Except as set forth in
*  an Authorized License, Broadcom grants no license (express or implied), right
*  to use, or waiver of any kind with respect to the Software, and Broadcom
*  expressly reserves all rights in and to the Software and all intellectual
*  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
*  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
*  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
* 
*  Except as expressly set forth in the Authorized License,
* 
*  1. This program, including its structure, sequence and organization,
*     constitutes the valuable trade secrets of Broadcom, and you shall use
*     all reasonable efforts to protect the confidentiality thereof, and to
*     use this information only in connection with your use of Broadcom
*     integrated circuit products.
* 
*  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
*     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
*     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
*     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
*     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
*     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
*     PERFORMANCE OF THE SOFTWARE.
* 
*  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
*     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
*     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
*     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
*     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
*     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
*     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
*     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
*     LIMITED REMEDY.
* :>
*/

#ifndef _BCM6858_GPON_TX_GENERAL_CONFIGURATION_AG_H_
#define _BCM6858_GPON_TX_GENERAL_CONFIGURATION_AG_H_

#include "access_macros.h"
#include "bcmtypes.h"
#ifdef USE_BDMF_SHELL
#include "bdmf_shell.h"
#endif
typedef struct
{
    uint8_t twt2;
    uint8_t twt3;
    uint8_t twt4;
    uint8_t twt5;
    uint8_t rdi;
} gpon_tx_general_configuration_ind;

int ag_drv_gpon_tx_general_configuration_onuid_set(uint8_t onuid);
int ag_drv_gpon_tx_general_configuration_onuid_get(uint8_t *onuid);
int ag_drv_gpon_tx_general_configuration_ind_set(const gpon_tx_general_configuration_ind *ind);
int ag_drv_gpon_tx_general_configuration_ind_get(gpon_tx_general_configuration_ind *ind);
int ag_drv_gpon_tx_general_configuration_prod_set(uint32_t word_idx, uint8_t prodc);
int ag_drv_gpon_tx_general_configuration_prod_get(uint32_t word_idx, uint8_t *prodc);
int ag_drv_gpon_tx_general_configuration_prods_set(uint8_t prcl, uint8_t prl);
int ag_drv_gpon_tx_general_configuration_prods_get(uint8_t *prcl, uint8_t *prl);
int ag_drv_gpon_tx_general_configuration_prrb_set(uint8_t brc);
int ag_drv_gpon_tx_general_configuration_prrb_get(uint8_t *brc);
int ag_drv_gpon_tx_general_configuration_delc_set(uint32_t deloc);
int ag_drv_gpon_tx_general_configuration_delc_get(uint32_t *deloc);
int ag_drv_gpon_tx_general_configuration_miscac1_set(uint16_t msstop, uint16_t msstart);
int ag_drv_gpon_tx_general_configuration_miscac1_get(uint16_t *msstop, uint16_t *msstart);
int ag_drv_gpon_tx_general_configuration_miscac2_set(uint8_t ploamu, uint8_t dbru, uint8_t fecu, uint8_t qu);
int ag_drv_gpon_tx_general_configuration_miscac2_get(uint8_t *ploamu, uint8_t *dbru, uint8_t *fecu, uint8_t *qu);
int ag_drv_gpon_tx_general_configuration_esall_set(uint8_t esa);
int ag_drv_gpon_tx_general_configuration_esall_get(uint8_t *esa);
int ag_drv_gpon_tx_general_configuration_qforcntr_set(uint8_t qforgrp0, uint8_t qforgrp1, uint8_t qforgrp2, uint8_t qforgrp3);
int ag_drv_gpon_tx_general_configuration_qforcntr_get(uint8_t *qforgrp0, uint8_t *qforgrp1, uint8_t *qforgrp2, uint8_t *qforgrp3);
int ag_drv_gpon_tx_general_configuration_plss_set(uint32_t plspat);
int ag_drv_gpon_tx_general_configuration_plss_get(uint32_t *plspat);
int ag_drv_gpon_tx_general_configuration_plsrs_set(uint8_t plsr);
int ag_drv_gpon_tx_general_configuration_plsrs_get(uint8_t *plsr);
int ag_drv_gpon_tx_general_configuration_bips_set(uint16_t bipenum, uint8_t bipm, uint8_t bipev, uint8_t bipef);
int ag_drv_gpon_tx_general_configuration_bips_get(uint16_t *bipenum, uint8_t *bipm, uint8_t *bipev, uint8_t *bipef);
int ag_drv_gpon_tx_general_configuration_crcoff_set(uint8_t crcoc);
int ag_drv_gpon_tx_general_configuration_crcoff_get(uint8_t *crcoc);
int ag_drv_gpon_tx_general_configuration_flush_set(uint8_t flq, uint8_t flen, uint8_t flimm);
int ag_drv_gpon_tx_general_configuration_flush_get(uint8_t *flq, uint8_t *flen, uint8_t *flimm);
int ag_drv_gpon_tx_general_configuration_flshdn_get(uint8_t *fldn);
int ag_drv_gpon_tx_general_configuration_rdyind_set(uint8_t set, uint8_t rdy);
int ag_drv_gpon_tx_general_configuration_rdyind_get(uint8_t *set, uint8_t *rdy);
int ag_drv_gpon_tx_general_configuration_dvslp_set(uint8_t dvpol, uint8_t dvexten);
int ag_drv_gpon_tx_general_configuration_dvslp_get(uint8_t *dvpol, uint8_t *dvexten);
int ag_drv_gpon_tx_general_configuration_dvstp_set(uint32_t dvstu);
int ag_drv_gpon_tx_general_configuration_dvstp_get(uint32_t *dvstu);
int ag_drv_gpon_tx_general_configuration_dvhld_set(uint32_t dvhld);
int ag_drv_gpon_tx_general_configuration_dvhld_get(uint32_t *dvhld);
int ag_drv_gpon_tx_general_configuration_dpen_set(uint8_t dpen, uint8_t prbs, uint8_t stplen, uint8_t hldlen);
int ag_drv_gpon_tx_general_configuration_dpen_get(uint8_t *dpen, uint8_t *prbs, uint8_t *stplen, uint8_t *hldlen);
int ag_drv_gpon_tx_general_configuration_dstp_set(uint32_t stppat);
int ag_drv_gpon_tx_general_configuration_dstp_get(uint32_t *stppat);
int ag_drv_gpon_tx_general_configuration_dhld_set(uint32_t hldpat);
int ag_drv_gpon_tx_general_configuration_dhld_get(uint32_t *hldpat);
int ag_drv_gpon_tx_general_configuration_rogue_onu_en_set(uint8_t rogue_level_clr, uint8_t rogue_diff_clr, uint8_t rogue_dv_sel);
int ag_drv_gpon_tx_general_configuration_rogue_onu_en_get(uint8_t *rogue_level_clr, uint8_t *rogue_diff_clr, uint8_t *rogue_dv_sel);
int ag_drv_gpon_tx_general_configuration_rogue_level_time_set(uint32_t window_size);
int ag_drv_gpon_tx_general_configuration_rogue_level_time_get(uint32_t *window_size);
int ag_drv_gpon_tx_general_configuration_rogue_diff_time_set(uint16_t window_size);
int ag_drv_gpon_tx_general_configuration_rogue_diff_time_get(uint16_t *window_size);

#ifdef USE_BDMF_SHELL
bdmfmon_handle_t ag_drv_gpon_tx_general_configuration_cli_init(bdmfmon_handle_t driver_dir);
#endif


#endif


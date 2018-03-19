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

#ifndef _BCM6858_NGPON_RX_GEN_AG_H_
#define _BCM6858_NGPON_RX_GEN_AG_H_

#include "access_macros.h"
#include "bcmtypes.h"
#ifdef USE_BDMF_SHELL
#include "bdmf_shell.h"
#endif
typedef struct
{
    uint8_t rx_enable;
    uint8_t desc_disable;
    uint8_t mac_mode;
    uint8_t loopback_enable;
    uint8_t tmf_enable;
    uint8_t rx_data_invert;
    uint8_t k_8b10b;
} ngpon_rx_gen_rcvrcfg;

int ag_drv_ngpon_rx_gen_rcvrstat_get(uint8_t *frame_sync_state, uint8_t *xgem_sync_state, uint8_t *ponid_consist_state, uint8_t *fwi_state);
int ag_drv_ngpon_rx_gen_rcvrcfg_set(const ngpon_rx_gen_rcvrcfg *rcvrcfg);
int ag_drv_ngpon_rx_gen_rcvrcfg_get(ngpon_rx_gen_rcvrcfg *rcvrcfg);
int ag_drv_ngpon_rx_gen_sync_cfg_set(uint8_t sync_loss_thr, uint8_t sync_acq_thr, uint8_t ponid_acq_thr, uint8_t ext_sync_holdover);
int ag_drv_ngpon_rx_gen_sync_cfg_get(uint8_t *sync_loss_thr, uint8_t *sync_acq_thr, uint8_t *ponid_acq_thr, uint8_t *ext_sync_holdover);
int ag_drv_ngpon_rx_gen_sync_stat_get(uint8_t *sync_cur_align, uint8_t *sync_match_ctr, uint8_t *sync_mismatch_ctr);
int ag_drv_ngpon_rx_gen_sync_psync_value_ms_set(uint32_t value);
int ag_drv_ngpon_rx_gen_sync_psync_value_ms_get(uint32_t *value);
int ag_drv_ngpon_rx_gen_sync_psync_value_ls_set(uint32_t value);
int ag_drv_ngpon_rx_gen_sync_psync_value_ls_get(uint32_t *value);
int ag_drv_ngpon_rx_gen_sync_frame_length_set(uint16_t value);
int ag_drv_ngpon_rx_gen_sync_frame_length_get(uint16_t *value);
int ag_drv_ngpon_rx_gen_random_seed_get(uint32_t *random_seed);
int ag_drv_ngpon_rx_gen_sync_ponid_ms_get(uint16_t *admin_label_msb, uint8_t *pit_rsvd, uint8_t *pit_odn_class, uint8_t *pit_re);
int ag_drv_ngpon_rx_gen_sync_ponid_ls_get(uint16_t *tol, uint8_t *dwlch_id, uint32_t *admin_label_lsb);
int ag_drv_ngpon_rx_gen_sync_sfc_get(uint32_t *ms_value, uint8_t *valid);
int ag_drv_ngpon_rx_gen_sync_sfc_ls_get(uint32_t *value);
int ag_drv_ngpon_rx_gen_sync_sfc_ind_set(uint32_t ms_value, uint8_t enable, uint8_t holdover);
int ag_drv_ngpon_rx_gen_sync_sfc_ind_get(uint32_t *ms_value, uint8_t *enable, uint8_t *holdover);
int ag_drv_ngpon_rx_gen_sync_sfc_ind_ls_set(uint32_t value);
int ag_drv_ngpon_rx_gen_sync_sfc_ind_ls_get(uint32_t *value);
int ag_drv_ngpon_rx_gen_version_get(uint8_t *lld_minor, uint8_t *lld_major, uint32_t *date);

#ifdef USE_BDMF_SHELL
bdmfmon_handle_t ag_drv_ngpon_rx_gen_cli_init(bdmfmon_handle_t driver_dir);
#endif


#endif


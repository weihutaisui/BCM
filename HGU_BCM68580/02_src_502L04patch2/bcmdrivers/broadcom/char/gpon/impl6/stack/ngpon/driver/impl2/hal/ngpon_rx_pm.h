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

#ifndef _BCM6858_NGPON_RX_PM_AG_H_
#define _BCM6858_NGPON_RX_PM_AG_H_

#include "access_macros.h"
#include "bcmtypes.h"
#ifdef USE_BDMF_SHELL
#include "bdmf_shell.h"
#endif
int ag_drv_ngpon_rx_pm_sync_lof_get(uint16_t *cnt);
int ag_drv_ngpon_rx_pm_bwmap_hec_err_get(uint16_t *cnt);
int ag_drv_ngpon_rx_pm_bwmap_invalid_burst_series_get(uint16_t *cnt);
int ag_drv_ngpon_rx_pm_hlend_hec_err_get(uint16_t *cnt);
int ag_drv_ngpon_rx_pm_del_lcdg_get(uint32_t *cnt);
int ag_drv_ngpon_rx_pm_sync_ponid_hec_err_get(uint16_t *cnt);
int ag_drv_ngpon_rx_pm_del_pass_pkt_get(uint32_t *cnt);
int ag_drv_ngpon_rx_pm_bwmap_correct_get(uint32_t *cnt);
int ag_drv_ngpon_rx_pm_bwmap_bursts_get(uint16_t *cnt);
int ag_drv_ngpon_rx_pm_xhp_pass_pkt_get(uint32_t *cnt);
int ag_drv_ngpon_rx_pm_dec_cant_allocate_get(uint16_t *cnt);
int ag_drv_ngpon_rx_pm_dec_invalid_key_idx_get(uint16_t *cnt);
int ag_drv_ngpon_rx_pm_plp_valid_onuid_get(uint32_t ngpon_rx_onu_id, uint16_t *cnt);
int ag_drv_ngpon_rx_pm_plp_valid_bcst_get(uint16_t *cnt);
int ag_drv_ngpon_rx_pm_plp_mic_err_get(uint16_t *cnt);
int ag_drv_ngpon_rx_pm_bwmap_hec_fix_get(uint16_t *cnt);
int ag_drv_ngpon_rx_pm_hlend_hec_fix_get(uint16_t *cnt);
int ag_drv_ngpon_rx_pm_xhp_hec_fix_get(uint16_t *cnt);
int ag_drv_ngpon_rx_pm_sync_sfc_hec_fix_get(uint16_t *cnt);
int ag_drv_ngpon_rx_pm_sync_ponid_hec_fix_get(uint16_t *cnt);
int ag_drv_ngpon_rx_pm_xgem_overrun_get(uint16_t *cnt);
int ag_drv_ngpon_rx_pm_bwmap_discard_dis_tx_get(uint16_t *cnt);
int ag_drv_ngpon_rx_pm_fec_bit_err_get(uint32_t *cnt);
int ag_drv_ngpon_rx_pm_fec_sym_err_get(uint32_t *cnt);
int ag_drv_ngpon_rx_pm_fec_cw_err_get(uint32_t *cnt);
int ag_drv_ngpon_rx_pm_fec_uc_cw_get(uint32_t *cnt);
int ag_drv_ngpon_rx_pm_bwmap_num_alloc_tcont_get(uint32_t ngpon_rx_onu_id, uint16_t *cnt);
int ag_drv_ngpon_rx_pm_bwmap_tot_bw_tcont_get(uint32_t ngpon_rx_onu_id, uint32_t *cnt);
int ag_drv_ngpon_rx_pm_fs_bip_err_get(uint16_t *cnt);
int ag_drv_ngpon_rx_pm_xhp_xgem_per_flow_get(uint32_t ngpon_rx_flow_id, uint32_t *cnt);
int ag_drv_ngpon_rx_pm_xhp_bytes_per_flow_get(uint32_t ngpon_rx_flow_id, uint32_t *cnt);

#ifdef USE_BDMF_SHELL
bdmfmon_handle_t ag_drv_ngpon_rx_pm_cli_init(bdmfmon_handle_t driver_dir);
#endif


#endif


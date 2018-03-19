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

#include "ngpon_common.h"
#include "ngpon_rx_pm.h"
int ag_drv_ngpon_rx_pm_sync_lof_get(uint16_t *cnt)
{
    uint32_t reg_sync_lof=0;

#ifdef VALIDATE_PARMS
    if(!cnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_PM, SYNC_LOF, reg_sync_lof);

    *cnt = RU_FIELD_GET(0, NGPON_RX_PM, SYNC_LOF, CNT, reg_sync_lof);

    return 0;
}

int ag_drv_ngpon_rx_pm_bwmap_hec_err_get(uint16_t *cnt)
{
    uint32_t reg_bwmap_hec_err=0;

#ifdef VALIDATE_PARMS
    if(!cnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_PM, BWMAP_HEC_ERR, reg_bwmap_hec_err);

    *cnt = RU_FIELD_GET(0, NGPON_RX_PM, BWMAP_HEC_ERR, CNT, reg_bwmap_hec_err);

    return 0;
}

int ag_drv_ngpon_rx_pm_bwmap_invalid_burst_series_get(uint16_t *cnt)
{
    uint32_t reg_bwmap_invalid_burst_series=0;

#ifdef VALIDATE_PARMS
    if(!cnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_PM, BWMAP_INVALID_BURST_SERIES, reg_bwmap_invalid_burst_series);

    *cnt = RU_FIELD_GET(0, NGPON_RX_PM, BWMAP_INVALID_BURST_SERIES, CNT, reg_bwmap_invalid_burst_series);

    return 0;
}

int ag_drv_ngpon_rx_pm_hlend_hec_err_get(uint16_t *cnt)
{
    uint32_t reg_hlend_hec_err=0;

#ifdef VALIDATE_PARMS
    if(!cnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_PM, HLEND_HEC_ERR, reg_hlend_hec_err);

    *cnt = RU_FIELD_GET(0, NGPON_RX_PM, HLEND_HEC_ERR, CNT, reg_hlend_hec_err);

    return 0;
}

int ag_drv_ngpon_rx_pm_del_lcdg_get(uint32_t *cnt)
{
    uint32_t reg_del_lcdg=0;

#ifdef VALIDATE_PARMS
    if(!cnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_PM, DEL_LCDG, reg_del_lcdg);

    *cnt = RU_FIELD_GET(0, NGPON_RX_PM, DEL_LCDG, CNT, reg_del_lcdg);

    return 0;
}

int ag_drv_ngpon_rx_pm_sync_ponid_hec_err_get(uint16_t *cnt)
{
    uint32_t reg_sync_ponid_hec_err=0;

#ifdef VALIDATE_PARMS
    if(!cnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_PM, SYNC_PONID_HEC_ERR, reg_sync_ponid_hec_err);

    *cnt = RU_FIELD_GET(0, NGPON_RX_PM, SYNC_PONID_HEC_ERR, CNT, reg_sync_ponid_hec_err);

    return 0;
}

int ag_drv_ngpon_rx_pm_del_pass_pkt_get(uint32_t *cnt)
{
    uint32_t reg_del_pass_pkt=0;

#ifdef VALIDATE_PARMS
    if(!cnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_PM, DEL_PASS_PKT, reg_del_pass_pkt);

    *cnt = RU_FIELD_GET(0, NGPON_RX_PM, DEL_PASS_PKT, CNT, reg_del_pass_pkt);

    return 0;
}

int ag_drv_ngpon_rx_pm_bwmap_correct_get(uint32_t *cnt)
{
    uint32_t reg_bwmap_correct=0;

#ifdef VALIDATE_PARMS
    if(!cnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_PM, BWMAP_CORRECT, reg_bwmap_correct);

    *cnt = RU_FIELD_GET(0, NGPON_RX_PM, BWMAP_CORRECT, CNT, reg_bwmap_correct);

    return 0;
}

int ag_drv_ngpon_rx_pm_bwmap_bursts_get(uint16_t *cnt)
{
    uint32_t reg_bwmap_bursts=0;

#ifdef VALIDATE_PARMS
    if(!cnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_PM, BWMAP_BURSTS, reg_bwmap_bursts);

    *cnt = RU_FIELD_GET(0, NGPON_RX_PM, BWMAP_BURSTS, CNT, reg_bwmap_bursts);

    return 0;
}

int ag_drv_ngpon_rx_pm_xhp_pass_pkt_get(uint32_t *cnt)
{
    uint32_t reg_xhp_pass_pkt=0;

#ifdef VALIDATE_PARMS
    if(!cnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_PM, XHP_PASS_PKT, reg_xhp_pass_pkt);

    *cnt = RU_FIELD_GET(0, NGPON_RX_PM, XHP_PASS_PKT, CNT, reg_xhp_pass_pkt);

    return 0;
}

int ag_drv_ngpon_rx_pm_dec_cant_allocate_get(uint16_t *cnt)
{
    uint32_t reg_dec_cant_allocate=0;

#ifdef VALIDATE_PARMS
    if(!cnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_PM, DEC_CANT_ALLOCATE, reg_dec_cant_allocate);

    *cnt = RU_FIELD_GET(0, NGPON_RX_PM, DEC_CANT_ALLOCATE, CNT, reg_dec_cant_allocate);

    return 0;
}

int ag_drv_ngpon_rx_pm_dec_invalid_key_idx_get(uint16_t *cnt)
{
    uint32_t reg_dec_invalid_key_idx=0;

#ifdef VALIDATE_PARMS
    if(!cnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_PM, DEC_INVALID_KEY_IDX, reg_dec_invalid_key_idx);

    *cnt = RU_FIELD_GET(0, NGPON_RX_PM, DEC_INVALID_KEY_IDX, CNT, reg_dec_invalid_key_idx);

    return 0;
}

int ag_drv_ngpon_rx_pm_plp_valid_onuid_get(uint32_t ngpon_rx_onu_id, uint16_t *cnt)
{
    uint32_t reg_plp_valid_onuid=0;

#ifdef VALIDATE_PARMS
    if(!cnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ngpon_rx_onu_id >= 2))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ngpon_rx_onu_id, NGPON_RX_PM, PLP_VALID_ONUID, reg_plp_valid_onuid);

    *cnt = RU_FIELD_GET(0, NGPON_RX_PM, PLP_VALID_ONUID, CNT, reg_plp_valid_onuid);

    return 0;
}

int ag_drv_ngpon_rx_pm_plp_valid_bcst_get(uint16_t *cnt)
{
    uint32_t reg_plp_valid_bcst=0;

#ifdef VALIDATE_PARMS
    if(!cnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_PM, PLP_VALID_BCST, reg_plp_valid_bcst);

    *cnt = RU_FIELD_GET(0, NGPON_RX_PM, PLP_VALID_BCST, CNT, reg_plp_valid_bcst);

    return 0;
}

int ag_drv_ngpon_rx_pm_plp_mic_err_get(uint16_t *cnt)
{
    uint32_t reg_plp_mic_err=0;

#ifdef VALIDATE_PARMS
    if(!cnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_PM, PLP_MIC_ERR, reg_plp_mic_err);

    *cnt = RU_FIELD_GET(0, NGPON_RX_PM, PLP_MIC_ERR, CNT, reg_plp_mic_err);

    return 0;
}

int ag_drv_ngpon_rx_pm_bwmap_hec_fix_get(uint16_t *cnt)
{
    uint32_t reg_bwmap_hec_fix=0;

#ifdef VALIDATE_PARMS
    if(!cnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_PM, BWMAP_HEC_FIX, reg_bwmap_hec_fix);

    *cnt = RU_FIELD_GET(0, NGPON_RX_PM, BWMAP_HEC_FIX, CNT, reg_bwmap_hec_fix);

    return 0;
}

int ag_drv_ngpon_rx_pm_hlend_hec_fix_get(uint16_t *cnt)
{
    uint32_t reg_hlend_hec_fix=0;

#ifdef VALIDATE_PARMS
    if(!cnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_PM, HLEND_HEC_FIX, reg_hlend_hec_fix);

    *cnt = RU_FIELD_GET(0, NGPON_RX_PM, HLEND_HEC_FIX, CNT, reg_hlend_hec_fix);

    return 0;
}

int ag_drv_ngpon_rx_pm_xhp_hec_fix_get(uint16_t *cnt)
{
    uint32_t reg_xhp_hec_fix=0;

#ifdef VALIDATE_PARMS
    if(!cnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_PM, XHP_HEC_FIX, reg_xhp_hec_fix);

    *cnt = RU_FIELD_GET(0, NGPON_RX_PM, XHP_HEC_FIX, CNT, reg_xhp_hec_fix);

    return 0;
}

int ag_drv_ngpon_rx_pm_sync_sfc_hec_fix_get(uint16_t *cnt)
{
    uint32_t reg_sync_sfc_hec_fix=0;

#ifdef VALIDATE_PARMS
    if(!cnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_PM, SYNC_SFC_HEC_FIX, reg_sync_sfc_hec_fix);

    *cnt = RU_FIELD_GET(0, NGPON_RX_PM, SYNC_SFC_HEC_FIX, CNT, reg_sync_sfc_hec_fix);

    return 0;
}

int ag_drv_ngpon_rx_pm_sync_ponid_hec_fix_get(uint16_t *cnt)
{
    uint32_t reg_sync_ponid_hec_fix=0;

#ifdef VALIDATE_PARMS
    if(!cnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_PM, SYNC_PONID_HEC_FIX, reg_sync_ponid_hec_fix);

    *cnt = RU_FIELD_GET(0, NGPON_RX_PM, SYNC_PONID_HEC_FIX, CNT, reg_sync_ponid_hec_fix);

    return 0;
}

int ag_drv_ngpon_rx_pm_xgem_overrun_get(uint16_t *cnt)
{
    uint32_t reg_xgem_overrun=0;

#ifdef VALIDATE_PARMS
    if(!cnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_PM, XGEM_OVERRUN, reg_xgem_overrun);

    *cnt = RU_FIELD_GET(0, NGPON_RX_PM, XGEM_OVERRUN, CNT, reg_xgem_overrun);

    return 0;
}

int ag_drv_ngpon_rx_pm_bwmap_discard_dis_tx_get(uint16_t *cnt)
{
    uint32_t reg_bwmap_discard_dis_tx=0;

#ifdef VALIDATE_PARMS
    if(!cnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_PM, BWMAP_DISCARD_DIS_TX, reg_bwmap_discard_dis_tx);

    *cnt = RU_FIELD_GET(0, NGPON_RX_PM, BWMAP_DISCARD_DIS_TX, CNT, reg_bwmap_discard_dis_tx);

    return 0;
}

int ag_drv_ngpon_rx_pm_fec_bit_err_get(uint32_t *cnt)
{
    uint32_t reg_fec_bit_err=0;

#ifdef VALIDATE_PARMS
    if(!cnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_PM, FEC_BIT_ERR, reg_fec_bit_err);

    *cnt = RU_FIELD_GET(0, NGPON_RX_PM, FEC_BIT_ERR, CNT, reg_fec_bit_err);

    return 0;
}

int ag_drv_ngpon_rx_pm_fec_sym_err_get(uint32_t *cnt)
{
    uint32_t reg_fec_sym_err=0;

#ifdef VALIDATE_PARMS
    if(!cnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_PM, FEC_SYM_ERR, reg_fec_sym_err);

    *cnt = RU_FIELD_GET(0, NGPON_RX_PM, FEC_SYM_ERR, CNT, reg_fec_sym_err);

    return 0;
}

int ag_drv_ngpon_rx_pm_fec_cw_err_get(uint32_t *cnt)
{
    uint32_t reg_fec_cw_err=0;

#ifdef VALIDATE_PARMS
    if(!cnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_PM, FEC_CW_ERR, reg_fec_cw_err);

    *cnt = RU_FIELD_GET(0, NGPON_RX_PM, FEC_CW_ERR, CNT, reg_fec_cw_err);

    return 0;
}

int ag_drv_ngpon_rx_pm_fec_uc_cw_get(uint32_t *cnt)
{
    uint32_t reg_fec_uc_cw=0;

#ifdef VALIDATE_PARMS
    if(!cnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_PM, FEC_UC_CW, reg_fec_uc_cw);

    *cnt = RU_FIELD_GET(0, NGPON_RX_PM, FEC_UC_CW, CNT, reg_fec_uc_cw);

    return 0;
}

int ag_drv_ngpon_rx_pm_bwmap_num_alloc_tcont_get(uint32_t ngpon_rx_onu_id, uint16_t *cnt)
{
    uint32_t reg_bwmap_num_alloc_tcont=0;

#ifdef VALIDATE_PARMS
    if(!cnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ngpon_rx_onu_id >= 12))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ngpon_rx_onu_id, NGPON_RX_PM, BWMAP_NUM_ALLOC_TCONT, reg_bwmap_num_alloc_tcont);

    *cnt = RU_FIELD_GET(0, NGPON_RX_PM, BWMAP_NUM_ALLOC_TCONT, CNT, reg_bwmap_num_alloc_tcont);

    return 0;
}

int ag_drv_ngpon_rx_pm_bwmap_tot_bw_tcont_get(uint32_t ngpon_rx_onu_id, uint32_t *cnt)
{
    uint32_t reg_bwmap_tot_bw_tcont=0;

#ifdef VALIDATE_PARMS
    if(!cnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ngpon_rx_onu_id >= 12))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ngpon_rx_onu_id, NGPON_RX_PM, BWMAP_TOT_BW_TCONT, reg_bwmap_tot_bw_tcont);

    *cnt = RU_FIELD_GET(0, NGPON_RX_PM, BWMAP_TOT_BW_TCONT, CNT, reg_bwmap_tot_bw_tcont);

    return 0;
}

int ag_drv_ngpon_rx_pm_fs_bip_err_get(uint16_t *cnt)
{
    uint32_t reg_fs_bip_err=0;

#ifdef VALIDATE_PARMS
    if(!cnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_PM, FS_BIP_ERR, reg_fs_bip_err);

    *cnt = RU_FIELD_GET(0, NGPON_RX_PM, FS_BIP_ERR, CNT, reg_fs_bip_err);

    return 0;
}

int ag_drv_ngpon_rx_pm_xhp_xgem_per_flow_get(uint32_t ngpon_rx_flow_id, uint32_t *cnt)
{
    uint32_t reg_xhp_xgem_per_flow=0;

#ifdef VALIDATE_PARMS
    if(!cnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ngpon_rx_flow_id >= 256))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ngpon_rx_flow_id, NGPON_RX_PM, XHP_XGEM_PER_FLOW, reg_xhp_xgem_per_flow);

    *cnt = RU_FIELD_GET(0, NGPON_RX_PM, XHP_XGEM_PER_FLOW, CNT, reg_xhp_xgem_per_flow);

    return 0;
}

int ag_drv_ngpon_rx_pm_xhp_bytes_per_flow_get(uint32_t ngpon_rx_flow_id, uint32_t *cnt)
{
    uint32_t reg_xhp_bytes_per_flow=0;

#ifdef VALIDATE_PARMS
    if(!cnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ngpon_rx_flow_id >= 256))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ngpon_rx_flow_id, NGPON_RX_PM, XHP_BYTES_PER_FLOW, reg_xhp_bytes_per_flow);

    *cnt = RU_FIELD_GET(0, NGPON_RX_PM, XHP_BYTES_PER_FLOW, CNT, reg_xhp_bytes_per_flow);

    return 0;
}


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
#include "ngpon_rx_gen.h"
int ag_drv_ngpon_rx_gen_rcvrstat_get(uint8_t *frame_sync_state, uint8_t *xgem_sync_state, uint8_t *ponid_consist_state, uint8_t *fwi_state)
{
    uint32_t reg_rcvrstat=0;

#ifdef VALIDATE_PARMS
    if(!frame_sync_state || !xgem_sync_state || !ponid_consist_state || !fwi_state)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_GEN, RCVRSTAT, reg_rcvrstat);

    *frame_sync_state = RU_FIELD_GET(0, NGPON_RX_GEN, RCVRSTAT, FRAME_SYNC_STATE, reg_rcvrstat);
    *xgem_sync_state = RU_FIELD_GET(0, NGPON_RX_GEN, RCVRSTAT, XGEM_SYNC_STATE, reg_rcvrstat);
    *ponid_consist_state = RU_FIELD_GET(0, NGPON_RX_GEN, RCVRSTAT, PONID_CONSIST_STATE, reg_rcvrstat);
    *fwi_state = RU_FIELD_GET(0, NGPON_RX_GEN, RCVRSTAT, FWI_STATE, reg_rcvrstat);

    return 0;
}

int ag_drv_ngpon_rx_gen_rcvrcfg_set(const ngpon_rx_gen_rcvrcfg *rcvrcfg)
{
    uint32_t reg_rcvrcfg=0;

#ifdef VALIDATE_PARMS
    if(!rcvrcfg)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, 1);
        return 1;
    }
    if((rcvrcfg->rx_enable >= _1BITS_MAX_VAL_) ||
       (rcvrcfg->desc_disable >= _1BITS_MAX_VAL_) ||
       (rcvrcfg->mac_mode >= _2BITS_MAX_VAL_) ||
       (rcvrcfg->loopback_enable >= _1BITS_MAX_VAL_) ||
       (rcvrcfg->tmf_enable >= _1BITS_MAX_VAL_) ||
       (rcvrcfg->rx_data_invert >= _1BITS_MAX_VAL_) ||
       (rcvrcfg->k_8b10b >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_rcvrcfg = RU_FIELD_SET(0, NGPON_RX_GEN, RCVRCFG, RX_ENABLE, reg_rcvrcfg, rcvrcfg->rx_enable);
    reg_rcvrcfg = RU_FIELD_SET(0, NGPON_RX_GEN, RCVRCFG, DESC_DISABLE, reg_rcvrcfg, rcvrcfg->desc_disable);
    reg_rcvrcfg = RU_FIELD_SET(0, NGPON_RX_GEN, RCVRCFG, MAC_MODE, reg_rcvrcfg, rcvrcfg->mac_mode);
    reg_rcvrcfg = RU_FIELD_SET(0, NGPON_RX_GEN, RCVRCFG, LOOPBACK_ENABLE, reg_rcvrcfg, rcvrcfg->loopback_enable);
    reg_rcvrcfg = RU_FIELD_SET(0, NGPON_RX_GEN, RCVRCFG, TMF_ENABLE, reg_rcvrcfg, rcvrcfg->tmf_enable);
    reg_rcvrcfg = RU_FIELD_SET(0, NGPON_RX_GEN, RCVRCFG, RX_DATA_INVERT, reg_rcvrcfg, rcvrcfg->rx_data_invert);
    reg_rcvrcfg = RU_FIELD_SET(0, NGPON_RX_GEN, RCVRCFG, K_8B10B, reg_rcvrcfg, rcvrcfg->k_8b10b);

    RU_REG_WRITE(0, NGPON_RX_GEN, RCVRCFG, reg_rcvrcfg);

    return 0;
}

int ag_drv_ngpon_rx_gen_rcvrcfg_get(ngpon_rx_gen_rcvrcfg *rcvrcfg)
{
    uint32_t reg_rcvrcfg=0;

#ifdef VALIDATE_PARMS
    if(!rcvrcfg)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_GEN, RCVRCFG, reg_rcvrcfg);

    rcvrcfg->rx_enable = RU_FIELD_GET(0, NGPON_RX_GEN, RCVRCFG, RX_ENABLE, reg_rcvrcfg);
    rcvrcfg->desc_disable = RU_FIELD_GET(0, NGPON_RX_GEN, RCVRCFG, DESC_DISABLE, reg_rcvrcfg);
    rcvrcfg->mac_mode = RU_FIELD_GET(0, NGPON_RX_GEN, RCVRCFG, MAC_MODE, reg_rcvrcfg);
    rcvrcfg->loopback_enable = RU_FIELD_GET(0, NGPON_RX_GEN, RCVRCFG, LOOPBACK_ENABLE, reg_rcvrcfg);
    rcvrcfg->tmf_enable = RU_FIELD_GET(0, NGPON_RX_GEN, RCVRCFG, TMF_ENABLE, reg_rcvrcfg);
    rcvrcfg->rx_data_invert = RU_FIELD_GET(0, NGPON_RX_GEN, RCVRCFG, RX_DATA_INVERT, reg_rcvrcfg);
    rcvrcfg->k_8b10b = RU_FIELD_GET(0, NGPON_RX_GEN, RCVRCFG, K_8B10B, reg_rcvrcfg);

    return 0;
}

int ag_drv_ngpon_rx_gen_sync_cfg_set(uint8_t sync_loss_thr, uint8_t sync_acq_thr, uint8_t ponid_acq_thr, uint8_t ext_sync_holdover)
{
    uint32_t reg_sync_cfg=0;

#ifdef VALIDATE_PARMS
    if((sync_loss_thr >= _4BITS_MAX_VAL_) ||
       (sync_acq_thr >= _4BITS_MAX_VAL_) ||
       (ponid_acq_thr >= _4BITS_MAX_VAL_) ||
       (ext_sync_holdover >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_sync_cfg = RU_FIELD_SET(0, NGPON_RX_GEN, SYNC_CFG, SYNC_LOSS_THR, reg_sync_cfg, sync_loss_thr);
    reg_sync_cfg = RU_FIELD_SET(0, NGPON_RX_GEN, SYNC_CFG, SYNC_ACQ_THR, reg_sync_cfg, sync_acq_thr);
    reg_sync_cfg = RU_FIELD_SET(0, NGPON_RX_GEN, SYNC_CFG, PONID_ACQ_THR, reg_sync_cfg, ponid_acq_thr);
    reg_sync_cfg = RU_FIELD_SET(0, NGPON_RX_GEN, SYNC_CFG, EXT_SYNC_HOLDOVER, reg_sync_cfg, ext_sync_holdover);

    RU_REG_WRITE(0, NGPON_RX_GEN, SYNC_CFG, reg_sync_cfg);

    return 0;
}

int ag_drv_ngpon_rx_gen_sync_cfg_get(uint8_t *sync_loss_thr, uint8_t *sync_acq_thr, uint8_t *ponid_acq_thr, uint8_t *ext_sync_holdover)
{
    uint32_t reg_sync_cfg=0;

#ifdef VALIDATE_PARMS
    if(!sync_loss_thr || !sync_acq_thr || !ponid_acq_thr || !ext_sync_holdover)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_GEN, SYNC_CFG, reg_sync_cfg);

    *sync_loss_thr = RU_FIELD_GET(0, NGPON_RX_GEN, SYNC_CFG, SYNC_LOSS_THR, reg_sync_cfg);
    *sync_acq_thr = RU_FIELD_GET(0, NGPON_RX_GEN, SYNC_CFG, SYNC_ACQ_THR, reg_sync_cfg);
    *ponid_acq_thr = RU_FIELD_GET(0, NGPON_RX_GEN, SYNC_CFG, PONID_ACQ_THR, reg_sync_cfg);
    *ext_sync_holdover = RU_FIELD_GET(0, NGPON_RX_GEN, SYNC_CFG, EXT_SYNC_HOLDOVER, reg_sync_cfg);

    return 0;
}

int ag_drv_ngpon_rx_gen_sync_stat_get(uint8_t *sync_cur_align, uint8_t *sync_match_ctr, uint8_t *sync_mismatch_ctr)
{
    uint32_t reg_sync_stat=0;

#ifdef VALIDATE_PARMS
    if(!sync_cur_align || !sync_match_ctr || !sync_mismatch_ctr)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_GEN, SYNC_STAT, reg_sync_stat);

    *sync_cur_align = RU_FIELD_GET(0, NGPON_RX_GEN, SYNC_STAT, SYNC_CUR_ALIGN, reg_sync_stat);
    *sync_match_ctr = RU_FIELD_GET(0, NGPON_RX_GEN, SYNC_STAT, SYNC_MATCH_CTR, reg_sync_stat);
    *sync_mismatch_ctr = RU_FIELD_GET(0, NGPON_RX_GEN, SYNC_STAT, SYNC_MISMATCH_CTR, reg_sync_stat);

    return 0;
}

int ag_drv_ngpon_rx_gen_sync_psync_value_ms_set(uint32_t value)
{
    uint32_t reg_sync_psync_value_ms=0;

#ifdef VALIDATE_PARMS
#endif

    reg_sync_psync_value_ms = RU_FIELD_SET(0, NGPON_RX_GEN, SYNC_PSYNC_VALUE_MS, VALUE, reg_sync_psync_value_ms, value);

    RU_REG_WRITE(0, NGPON_RX_GEN, SYNC_PSYNC_VALUE_MS, reg_sync_psync_value_ms);

    return 0;
}

int ag_drv_ngpon_rx_gen_sync_psync_value_ms_get(uint32_t *value)
{
    uint32_t reg_sync_psync_value_ms=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_GEN, SYNC_PSYNC_VALUE_MS, reg_sync_psync_value_ms);

    *value = RU_FIELD_GET(0, NGPON_RX_GEN, SYNC_PSYNC_VALUE_MS, VALUE, reg_sync_psync_value_ms);

    return 0;
}

int ag_drv_ngpon_rx_gen_sync_psync_value_ls_set(uint32_t value)
{
    uint32_t reg_sync_psync_value_ls=0;

#ifdef VALIDATE_PARMS
#endif

    reg_sync_psync_value_ls = RU_FIELD_SET(0, NGPON_RX_GEN, SYNC_PSYNC_VALUE_LS, VALUE, reg_sync_psync_value_ls, value);

    RU_REG_WRITE(0, NGPON_RX_GEN, SYNC_PSYNC_VALUE_LS, reg_sync_psync_value_ls);

    return 0;
}

int ag_drv_ngpon_rx_gen_sync_psync_value_ls_get(uint32_t *value)
{
    uint32_t reg_sync_psync_value_ls=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_GEN, SYNC_PSYNC_VALUE_LS, reg_sync_psync_value_ls);

    *value = RU_FIELD_GET(0, NGPON_RX_GEN, SYNC_PSYNC_VALUE_LS, VALUE, reg_sync_psync_value_ls);

    return 0;
}

int ag_drv_ngpon_rx_gen_sync_frame_length_set(uint16_t value)
{
    uint32_t reg_sync_frame_length=0;

#ifdef VALIDATE_PARMS
#endif

    reg_sync_frame_length = RU_FIELD_SET(0, NGPON_RX_GEN, SYNC_FRAME_LENGTH, VALUE, reg_sync_frame_length, value);

    RU_REG_WRITE(0, NGPON_RX_GEN, SYNC_FRAME_LENGTH, reg_sync_frame_length);

    return 0;
}

int ag_drv_ngpon_rx_gen_sync_frame_length_get(uint16_t *value)
{
    uint32_t reg_sync_frame_length=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_GEN, SYNC_FRAME_LENGTH, reg_sync_frame_length);

    *value = RU_FIELD_GET(0, NGPON_RX_GEN, SYNC_FRAME_LENGTH, VALUE, reg_sync_frame_length);

    return 0;
}

int ag_drv_ngpon_rx_gen_random_seed_get(uint32_t *random_seed)
{
    uint32_t reg_random_seed=0;

#ifdef VALIDATE_PARMS
    if(!random_seed)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_GEN, RANDOM_SEED, reg_random_seed);

    *random_seed = RU_FIELD_GET(0, NGPON_RX_GEN, RANDOM_SEED, RANDOM_SEED, reg_random_seed);

    return 0;
}

int ag_drv_ngpon_rx_gen_sync_ponid_ms_get(uint16_t *admin_label_msb, uint8_t *pit_rsvd, uint8_t *pit_odn_class, uint8_t *pit_re)
{
    uint32_t reg_sync_ponid_ms=0;

#ifdef VALIDATE_PARMS
    if(!admin_label_msb || !pit_rsvd || !pit_odn_class || !pit_re)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_GEN, SYNC_PONID_MS, reg_sync_ponid_ms);

    *admin_label_msb = RU_FIELD_GET(0, NGPON_RX_GEN, SYNC_PONID_MS, ADMIN_LABEL_MSB, reg_sync_ponid_ms);
    *pit_rsvd = RU_FIELD_GET(0, NGPON_RX_GEN, SYNC_PONID_MS, PIT_RSVD, reg_sync_ponid_ms);
    *pit_odn_class = RU_FIELD_GET(0, NGPON_RX_GEN, SYNC_PONID_MS, PIT_ODN_CLASS, reg_sync_ponid_ms);
    *pit_re = RU_FIELD_GET(0, NGPON_RX_GEN, SYNC_PONID_MS, PIT_RE, reg_sync_ponid_ms);

    return 0;
}

int ag_drv_ngpon_rx_gen_sync_ponid_ls_get(uint16_t *tol, uint8_t *dwlch_id, uint32_t *admin_label_lsb)
{
    uint32_t reg_sync_ponid_ls=0;

#ifdef VALIDATE_PARMS
    if(!tol || !dwlch_id || !admin_label_lsb)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_GEN, SYNC_PONID_LS, reg_sync_ponid_ls);

    *tol = RU_FIELD_GET(0, NGPON_RX_GEN, SYNC_PONID_LS, TOL, reg_sync_ponid_ls);
    *dwlch_id = RU_FIELD_GET(0, NGPON_RX_GEN, SYNC_PONID_LS, DWLCH_ID, reg_sync_ponid_ls);
    *admin_label_lsb = RU_FIELD_GET(0, NGPON_RX_GEN, SYNC_PONID_LS, ADMIN_LABEL_LSB, reg_sync_ponid_ls);

    return 0;
}

int ag_drv_ngpon_rx_gen_sync_sfc_get(uint32_t *ms_value, uint8_t *valid)
{
    uint32_t reg_sync_sfc=0;

#ifdef VALIDATE_PARMS
    if(!ms_value || !valid)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_GEN, SYNC_SFC, reg_sync_sfc);

    *ms_value = RU_FIELD_GET(0, NGPON_RX_GEN, SYNC_SFC, MS_VALUE, reg_sync_sfc);
    *valid = RU_FIELD_GET(0, NGPON_RX_GEN, SYNC_SFC, VALID, reg_sync_sfc);

    return 0;
}

int ag_drv_ngpon_rx_gen_sync_sfc_ls_get(uint32_t *value)
{
    uint32_t reg_sync_sfc_ls=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_GEN, SYNC_SFC_LS, reg_sync_sfc_ls);

    *value = RU_FIELD_GET(0, NGPON_RX_GEN, SYNC_SFC_LS, VALUE, reg_sync_sfc_ls);

    return 0;
}

int ag_drv_ngpon_rx_gen_sync_sfc_ind_set(uint32_t ms_value, uint8_t enable, uint8_t holdover)
{
    uint32_t reg_sync_sfc_ind=0;

#ifdef VALIDATE_PARMS
    if((ms_value >= _19BITS_MAX_VAL_) ||
       (enable >= _1BITS_MAX_VAL_) ||
       (holdover >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_sync_sfc_ind = RU_FIELD_SET(0, NGPON_RX_GEN, SYNC_SFC_IND, MS_VALUE, reg_sync_sfc_ind, ms_value);
    reg_sync_sfc_ind = RU_FIELD_SET(0, NGPON_RX_GEN, SYNC_SFC_IND, ENABLE, reg_sync_sfc_ind, enable);
    reg_sync_sfc_ind = RU_FIELD_SET(0, NGPON_RX_GEN, SYNC_SFC_IND, HOLDOVER, reg_sync_sfc_ind, holdover);

    RU_REG_WRITE(0, NGPON_RX_GEN, SYNC_SFC_IND, reg_sync_sfc_ind);

    return 0;
}

int ag_drv_ngpon_rx_gen_sync_sfc_ind_get(uint32_t *ms_value, uint8_t *enable, uint8_t *holdover)
{
    uint32_t reg_sync_sfc_ind=0;

#ifdef VALIDATE_PARMS
    if(!ms_value || !enable || !holdover)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_GEN, SYNC_SFC_IND, reg_sync_sfc_ind);

    *ms_value = RU_FIELD_GET(0, NGPON_RX_GEN, SYNC_SFC_IND, MS_VALUE, reg_sync_sfc_ind);
    *enable = RU_FIELD_GET(0, NGPON_RX_GEN, SYNC_SFC_IND, ENABLE, reg_sync_sfc_ind);
    *holdover = RU_FIELD_GET(0, NGPON_RX_GEN, SYNC_SFC_IND, HOLDOVER, reg_sync_sfc_ind);

    return 0;
}

int ag_drv_ngpon_rx_gen_sync_sfc_ind_ls_set(uint32_t value)
{
    uint32_t reg_sync_sfc_ind_ls=0;

#ifdef VALIDATE_PARMS
#endif

    reg_sync_sfc_ind_ls = RU_FIELD_SET(0, NGPON_RX_GEN, SYNC_SFC_IND_LS, VALUE, reg_sync_sfc_ind_ls, value);

    RU_REG_WRITE(0, NGPON_RX_GEN, SYNC_SFC_IND_LS, reg_sync_sfc_ind_ls);

    return 0;
}

int ag_drv_ngpon_rx_gen_sync_sfc_ind_ls_get(uint32_t *value)
{
    uint32_t reg_sync_sfc_ind_ls=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_GEN, SYNC_SFC_IND_LS, reg_sync_sfc_ind_ls);

    *value = RU_FIELD_GET(0, NGPON_RX_GEN, SYNC_SFC_IND_LS, VALUE, reg_sync_sfc_ind_ls);

    return 0;
}

int ag_drv_ngpon_rx_gen_version_get(uint8_t *lld_minor, uint8_t *lld_major, uint32_t *date)
{
    uint32_t reg_version=0;

#ifdef VALIDATE_PARMS
    if(!lld_minor || !lld_major || !date)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_GEN, VERSION, reg_version);

    *lld_minor = RU_FIELD_GET(0, NGPON_RX_GEN, VERSION, LLD_MINOR, reg_version);
    *lld_major = RU_FIELD_GET(0, NGPON_RX_GEN, VERSION, LLD_MAJOR, reg_version);
    *date = RU_FIELD_GET(0, NGPON_RX_GEN, VERSION, DATE, reg_version);

    return 0;
}


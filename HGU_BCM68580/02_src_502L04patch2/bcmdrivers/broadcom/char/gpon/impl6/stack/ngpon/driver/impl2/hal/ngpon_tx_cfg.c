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
#include "ngpon_tx_cfg.h"
int ag_drv_ngpon_tx_cfg_onu_tx_en_set(uint8_t transmitter_enable, uint8_t standard, uint8_t speed)
{
    uint32_t reg_onu_tx_en=0;

#ifdef VALIDATE_PARMS
    if((transmitter_enable >= _1BITS_MAX_VAL_) ||
       (standard >= _1BITS_MAX_VAL_) ||
       (speed >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_onu_tx_en = RU_FIELD_SET(0, NGPON_TX_CFG, ONU_TX_EN, TRANSMITTER_ENABLE, reg_onu_tx_en, transmitter_enable);
    reg_onu_tx_en = RU_FIELD_SET(0, NGPON_TX_CFG, ONU_TX_EN, STANDARD, reg_onu_tx_en, standard);
    reg_onu_tx_en = RU_FIELD_SET(0, NGPON_TX_CFG, ONU_TX_EN, SPEED, reg_onu_tx_en, speed);

    RU_REG_WRITE(0, NGPON_TX_CFG, ONU_TX_EN, reg_onu_tx_en);

    return 0;
}

int ag_drv_ngpon_tx_cfg_onu_tx_en_get(uint8_t *transmitter_enable, uint8_t *standard, uint8_t *speed)
{
    uint32_t reg_onu_tx_en=0;

#ifdef VALIDATE_PARMS
    if(!transmitter_enable || !standard || !speed)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_CFG, ONU_TX_EN, reg_onu_tx_en);

    *transmitter_enable = RU_FIELD_GET(0, NGPON_TX_CFG, ONU_TX_EN, TRANSMITTER_ENABLE, reg_onu_tx_en);
    *standard = RU_FIELD_GET(0, NGPON_TX_CFG, ONU_TX_EN, STANDARD, reg_onu_tx_en);
    *speed = RU_FIELD_GET(0, NGPON_TX_CFG, ONU_TX_EN, SPEED, reg_onu_tx_en);

    return 0;
}

int ag_drv_ngpon_tx_cfg_func_en_set(const ngpon_tx_cfg_func_en *func_en)
{
    uint32_t reg_func_en=0;

#ifdef VALIDATE_PARMS
    if(!func_en)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((func_en->scrmblr_en >= _1BITS_MAX_VAL_) ||
       (func_en->bip_en >= _1BITS_MAX_VAL_) ||
       (func_en->fec_en >= _1BITS_MAX_VAL_) ||
       (func_en->reserved1 >= _1BITS_MAX_VAL_) ||
       (func_en->reserved2 >= _1BITS_MAX_VAL_) ||
       (func_en->reserved3 >= _1BITS_MAX_VAL_) ||
       (func_en->enc_en >= _1BITS_MAX_VAL_) ||
       (func_en->reserved0 >= _1BITS_MAX_VAL_) ||
       (func_en->s >= _24BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_func_en = RU_FIELD_SET(0, NGPON_TX_CFG, FUNC_EN, SCRMBLR_EN, reg_func_en, func_en->scrmblr_en);
    reg_func_en = RU_FIELD_SET(0, NGPON_TX_CFG, FUNC_EN, BIP_EN, reg_func_en, func_en->bip_en);
    reg_func_en = RU_FIELD_SET(0, NGPON_TX_CFG, FUNC_EN, FEC_EN, reg_func_en, func_en->fec_en);
    reg_func_en = RU_FIELD_SET(0, NGPON_TX_CFG, FUNC_EN, RESERVED1, reg_func_en, func_en->reserved1);
    reg_func_en = RU_FIELD_SET(0, NGPON_TX_CFG, FUNC_EN, RESERVED2, reg_func_en, func_en->reserved2);
    reg_func_en = RU_FIELD_SET(0, NGPON_TX_CFG, FUNC_EN, RESERVED3, reg_func_en, func_en->reserved3);
    reg_func_en = RU_FIELD_SET(0, NGPON_TX_CFG, FUNC_EN, ENC_EN, reg_func_en, func_en->enc_en);
    reg_func_en = RU_FIELD_SET(0, NGPON_TX_CFG, FUNC_EN, RESERVED0, reg_func_en, func_en->reserved0);
    reg_func_en = RU_FIELD_SET(0, NGPON_TX_CFG, FUNC_EN, S, reg_func_en, func_en->s);

    RU_REG_WRITE(0, NGPON_TX_CFG, FUNC_EN, reg_func_en);

    return 0;
}

int ag_drv_ngpon_tx_cfg_func_en_get(ngpon_tx_cfg_func_en *func_en)
{
    uint32_t reg_func_en=0;

#ifdef VALIDATE_PARMS
    if(!func_en)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_CFG, FUNC_EN, reg_func_en);

    func_en->scrmblr_en = RU_FIELD_GET(0, NGPON_TX_CFG, FUNC_EN, SCRMBLR_EN, reg_func_en);
    func_en->bip_en = RU_FIELD_GET(0, NGPON_TX_CFG, FUNC_EN, BIP_EN, reg_func_en);
    func_en->fec_en = RU_FIELD_GET(0, NGPON_TX_CFG, FUNC_EN, FEC_EN, reg_func_en);
    func_en->reserved1 = RU_FIELD_GET(0, NGPON_TX_CFG, FUNC_EN, RESERVED1, reg_func_en);
    func_en->reserved2 = RU_FIELD_GET(0, NGPON_TX_CFG, FUNC_EN, RESERVED2, reg_func_en);
    func_en->reserved3 = RU_FIELD_GET(0, NGPON_TX_CFG, FUNC_EN, RESERVED3, reg_func_en);
    func_en->enc_en = RU_FIELD_GET(0, NGPON_TX_CFG, FUNC_EN, ENC_EN, reg_func_en);
    func_en->reserved0 = RU_FIELD_GET(0, NGPON_TX_CFG, FUNC_EN, RESERVED0, reg_func_en);
    func_en->s = RU_FIELD_GET(0, NGPON_TX_CFG, FUNC_EN, S, reg_func_en);

    return 0;
}

int ag_drv_ngpon_tx_cfg_dat_pol_set(uint8_t tx_data_out_polarity)
{
    uint32_t reg_dat_pol=0;

#ifdef VALIDATE_PARMS
    if((tx_data_out_polarity >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_dat_pol = RU_FIELD_SET(0, NGPON_TX_CFG, DAT_POL, TX_DATA_OUT_POLARITY, reg_dat_pol, tx_data_out_polarity);

    RU_REG_WRITE(0, NGPON_TX_CFG, DAT_POL, reg_dat_pol);

    return 0;
}

int ag_drv_ngpon_tx_cfg_dat_pol_get(uint8_t *tx_data_out_polarity)
{
    uint32_t reg_dat_pol=0;

#ifdef VALIDATE_PARMS
    if(!tx_data_out_polarity)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_CFG, DAT_POL, reg_dat_pol);

    *tx_data_out_polarity = RU_FIELD_GET(0, NGPON_TX_CFG, DAT_POL, TX_DATA_OUT_POLARITY, reg_dat_pol);

    return 0;
}

int ag_drv_ngpon_tx_cfg_tx_rng_dly_set(uint32_t tx_word_delay, uint8_t tx_frame_delay, uint8_t tx_bit_delay)
{
    uint32_t reg_tx_rng_dly=0;

#ifdef VALIDATE_PARMS
    if((tx_word_delay >= _18BITS_MAX_VAL_) ||
       (tx_frame_delay >= _4BITS_MAX_VAL_) ||
       (tx_bit_delay >= _5BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_tx_rng_dly = RU_FIELD_SET(0, NGPON_TX_CFG, TX_RNG_DLY, TX_WORD_DELAY, reg_tx_rng_dly, tx_word_delay);
    reg_tx_rng_dly = RU_FIELD_SET(0, NGPON_TX_CFG, TX_RNG_DLY, TX_FRAME_DELAY, reg_tx_rng_dly, tx_frame_delay);
    reg_tx_rng_dly = RU_FIELD_SET(0, NGPON_TX_CFG, TX_RNG_DLY, TX_BIT_DELAY, reg_tx_rng_dly, tx_bit_delay);

    RU_REG_WRITE(0, NGPON_TX_CFG, TX_RNG_DLY, reg_tx_rng_dly);

    return 0;
}

int ag_drv_ngpon_tx_cfg_tx_rng_dly_get(uint32_t *tx_word_delay, uint8_t *tx_frame_delay, uint8_t *tx_bit_delay)
{
    uint32_t reg_tx_rng_dly=0;

#ifdef VALIDATE_PARMS
    if(!tx_word_delay || !tx_frame_delay || !tx_bit_delay)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_CFG, TX_RNG_DLY, reg_tx_rng_dly);

    *tx_word_delay = RU_FIELD_GET(0, NGPON_TX_CFG, TX_RNG_DLY, TX_WORD_DELAY, reg_tx_rng_dly);
    *tx_frame_delay = RU_FIELD_GET(0, NGPON_TX_CFG, TX_RNG_DLY, TX_FRAME_DELAY, reg_tx_rng_dly);
    *tx_bit_delay = RU_FIELD_GET(0, NGPON_TX_CFG, TX_RNG_DLY, TX_BIT_DELAY, reg_tx_rng_dly);

    return 0;
}

int ag_drv_ngpon_tx_cfg_dv_cfg_set(const ngpon_tx_cfg_dv_cfg *dv_cfg)
{
    uint32_t reg_dv_cfg=0;

#ifdef VALIDATE_PARMS
    if(!dv_cfg)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((dv_cfg->dv_pol >= _1BITS_MAX_VAL_) ||
       (dv_cfg->dv_setup_len >= _3BITS_MAX_VAL_) ||
       (dv_cfg->dv_hold_len >= _3BITS_MAX_VAL_) ||
       (dv_cfg->dv_setup_pat_src >= _1BITS_MAX_VAL_) ||
       (dv_cfg->dv_hold_pat_src >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_dv_cfg = RU_FIELD_SET(0, NGPON_TX_CFG, DV_CFG, DV_POL, reg_dv_cfg, dv_cfg->dv_pol);
    reg_dv_cfg = RU_FIELD_SET(0, NGPON_TX_CFG, DV_CFG, DV_SETUP_LEN, reg_dv_cfg, dv_cfg->dv_setup_len);
    reg_dv_cfg = RU_FIELD_SET(0, NGPON_TX_CFG, DV_CFG, DV_HOLD_LEN, reg_dv_cfg, dv_cfg->dv_hold_len);
    reg_dv_cfg = RU_FIELD_SET(0, NGPON_TX_CFG, DV_CFG, DV_SETUP_PAT_SRC, reg_dv_cfg, dv_cfg->dv_setup_pat_src);
    reg_dv_cfg = RU_FIELD_SET(0, NGPON_TX_CFG, DV_CFG, DV_HOLD_PAT_SRC, reg_dv_cfg, dv_cfg->dv_hold_pat_src);

    RU_REG_WRITE(0, NGPON_TX_CFG, DV_CFG, reg_dv_cfg);

    return 0;
}

int ag_drv_ngpon_tx_cfg_dv_cfg_get(ngpon_tx_cfg_dv_cfg *dv_cfg)
{
    uint32_t reg_dv_cfg=0;

#ifdef VALIDATE_PARMS
    if(!dv_cfg)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_CFG, DV_CFG, reg_dv_cfg);

    dv_cfg->dv_pol = RU_FIELD_GET(0, NGPON_TX_CFG, DV_CFG, DV_POL, reg_dv_cfg);
    dv_cfg->dv_setup_len = RU_FIELD_GET(0, NGPON_TX_CFG, DV_CFG, DV_SETUP_LEN, reg_dv_cfg);
    dv_cfg->dv_hold_len = RU_FIELD_GET(0, NGPON_TX_CFG, DV_CFG, DV_HOLD_LEN, reg_dv_cfg);
    dv_cfg->dv_setup_pat_src = RU_FIELD_GET(0, NGPON_TX_CFG, DV_CFG, DV_SETUP_PAT_SRC, reg_dv_cfg);
    dv_cfg->dv_hold_pat_src = RU_FIELD_GET(0, NGPON_TX_CFG, DV_CFG, DV_HOLD_PAT_SRC, reg_dv_cfg);

    return 0;
}

int ag_drv_ngpon_tx_cfg_dv_setup_pat_set(uint32_t value)
{
    uint32_t reg_dv_setup_pat=0;

#ifdef VALIDATE_PARMS
#endif

    reg_dv_setup_pat = RU_FIELD_SET(0, NGPON_TX_CFG, DV_SETUP_PAT, VALUE, reg_dv_setup_pat, value);

    RU_REG_WRITE(0, NGPON_TX_CFG, DV_SETUP_PAT, reg_dv_setup_pat);

    return 0;
}

int ag_drv_ngpon_tx_cfg_dv_setup_pat_get(uint32_t *value)
{
    uint32_t reg_dv_setup_pat=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_CFG, DV_SETUP_PAT, reg_dv_setup_pat);

    *value = RU_FIELD_GET(0, NGPON_TX_CFG, DV_SETUP_PAT, VALUE, reg_dv_setup_pat);

    return 0;
}

int ag_drv_ngpon_tx_cfg_dv_hold_pat_set(uint32_t value)
{
    uint32_t reg_dv_hold_pat=0;

#ifdef VALIDATE_PARMS
#endif

    reg_dv_hold_pat = RU_FIELD_SET(0, NGPON_TX_CFG, DV_HOLD_PAT, VALUE, reg_dv_hold_pat, value);

    RU_REG_WRITE(0, NGPON_TX_CFG, DV_HOLD_PAT, reg_dv_hold_pat);

    return 0;
}

int ag_drv_ngpon_tx_cfg_dv_hold_pat_get(uint32_t *value)
{
    uint32_t reg_dv_hold_pat=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_CFG, DV_HOLD_PAT, reg_dv_hold_pat);

    *value = RU_FIELD_GET(0, NGPON_TX_CFG, DV_HOLD_PAT, VALUE, reg_dv_hold_pat);

    return 0;
}

int ag_drv_ngpon_tx_cfg_dat_pat_cfg_set(uint8_t dat_pat_type, uint8_t setup_len, uint8_t hold_len)
{
    uint32_t reg_dat_pat_cfg=0;

#ifdef VALIDATE_PARMS
    if((dat_pat_type >= _1BITS_MAX_VAL_) ||
       (setup_len >= _3BITS_MAX_VAL_) ||
       (hold_len >= _3BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_dat_pat_cfg = RU_FIELD_SET(0, NGPON_TX_CFG, DAT_PAT_CFG, DAT_PAT_TYPE, reg_dat_pat_cfg, dat_pat_type);
    reg_dat_pat_cfg = RU_FIELD_SET(0, NGPON_TX_CFG, DAT_PAT_CFG, SETUP_LEN, reg_dat_pat_cfg, setup_len);
    reg_dat_pat_cfg = RU_FIELD_SET(0, NGPON_TX_CFG, DAT_PAT_CFG, HOLD_LEN, reg_dat_pat_cfg, hold_len);

    RU_REG_WRITE(0, NGPON_TX_CFG, DAT_PAT_CFG, reg_dat_pat_cfg);

    return 0;
}

int ag_drv_ngpon_tx_cfg_dat_pat_cfg_get(uint8_t *dat_pat_type, uint8_t *setup_len, uint8_t *hold_len)
{
    uint32_t reg_dat_pat_cfg=0;

#ifdef VALIDATE_PARMS
    if(!dat_pat_type || !setup_len || !hold_len)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_CFG, DAT_PAT_CFG, reg_dat_pat_cfg);

    *dat_pat_type = RU_FIELD_GET(0, NGPON_TX_CFG, DAT_PAT_CFG, DAT_PAT_TYPE, reg_dat_pat_cfg);
    *setup_len = RU_FIELD_GET(0, NGPON_TX_CFG, DAT_PAT_CFG, SETUP_LEN, reg_dat_pat_cfg);
    *hold_len = RU_FIELD_GET(0, NGPON_TX_CFG, DAT_PAT_CFG, HOLD_LEN, reg_dat_pat_cfg);

    return 0;
}

int ag_drv_ngpon_tx_cfg_dat_pat_set(uint32_t value)
{
    uint32_t reg_dat_pat=0;

#ifdef VALIDATE_PARMS
#endif

    reg_dat_pat = RU_FIELD_SET(0, NGPON_TX_CFG, DAT_PAT, VALUE, reg_dat_pat, value);

    RU_REG_WRITE(0, NGPON_TX_CFG, DAT_PAT, reg_dat_pat);

    return 0;
}

int ag_drv_ngpon_tx_cfg_dat_pat_get(uint32_t *value)
{
    uint32_t reg_dat_pat=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_CFG, DAT_PAT, reg_dat_pat);

    *value = RU_FIELD_GET(0, NGPON_TX_CFG, DAT_PAT, VALUE, reg_dat_pat);

    return 0;
}

int ag_drv_ngpon_tx_cfg_dat_setup_pat_set(uint32_t tx_cfg_data_setup_pat_word, uint32_t value)
{
    uint32_t reg_dat_setup_pat=0;

#ifdef VALIDATE_PARMS
    if((tx_cfg_data_setup_pat_word >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_dat_setup_pat = RU_FIELD_SET(0, NGPON_TX_CFG, DAT_SETUP_PAT, VALUE, reg_dat_setup_pat, value);

    RU_REG_RAM_WRITE(0, tx_cfg_data_setup_pat_word, NGPON_TX_CFG, DAT_SETUP_PAT, reg_dat_setup_pat);

    return 0;
}

int ag_drv_ngpon_tx_cfg_dat_setup_pat_get(uint32_t tx_cfg_data_setup_pat_word, uint32_t *value)
{
    uint32_t reg_dat_setup_pat=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((tx_cfg_data_setup_pat_word >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, tx_cfg_data_setup_pat_word, NGPON_TX_CFG, DAT_SETUP_PAT, reg_dat_setup_pat);

    *value = RU_FIELD_GET(0, NGPON_TX_CFG, DAT_SETUP_PAT, VALUE, reg_dat_setup_pat);

    return 0;
}

int ag_drv_ngpon_tx_cfg_dat_hold_pat_set(uint32_t tx_cfg_data_hold_pat_word, uint32_t value)
{
    uint32_t reg_dat_hold_pat=0;

#ifdef VALIDATE_PARMS
    if((tx_cfg_data_hold_pat_word >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_dat_hold_pat = RU_FIELD_SET(0, NGPON_TX_CFG, DAT_HOLD_PAT, VALUE, reg_dat_hold_pat, value);

    RU_REG_RAM_WRITE(0, tx_cfg_data_hold_pat_word, NGPON_TX_CFG, DAT_HOLD_PAT, reg_dat_hold_pat);

    return 0;
}

int ag_drv_ngpon_tx_cfg_dat_hold_pat_get(uint32_t tx_cfg_data_hold_pat_word, uint32_t *value)
{
    uint32_t reg_dat_hold_pat=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((tx_cfg_data_hold_pat_word >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, tx_cfg_data_hold_pat_word, NGPON_TX_CFG, DAT_HOLD_PAT, reg_dat_hold_pat);

    *value = RU_FIELD_GET(0, NGPON_TX_CFG, DAT_HOLD_PAT, VALUE, reg_dat_hold_pat);

    return 0;
}

int ag_drv_ngpon_tx_cfg_tcont_to_onu_id_map_set(uint32_t ngpon_tx_onu_id_elem, uint8_t onu_id)
{
    uint32_t reg_tcont_to_onu_id_map=0;

#ifdef VALIDATE_PARMS
    if((ngpon_tx_onu_id_elem >= 44) ||
       (onu_id >= _2BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_tcont_to_onu_id_map = RU_FIELD_SET(0, NGPON_TX_CFG, TCONT_TO_ONU_ID_MAP, ONU_ID, reg_tcont_to_onu_id_map, onu_id);

    RU_REG_RAM_WRITE(0, ngpon_tx_onu_id_elem, NGPON_TX_CFG, TCONT_TO_ONU_ID_MAP, reg_tcont_to_onu_id_map);

    return 0;
}

int ag_drv_ngpon_tx_cfg_tcont_to_onu_id_map_get(uint32_t ngpon_tx_onu_id_elem, uint8_t *onu_id)
{
    uint32_t reg_tcont_to_onu_id_map=0;

#ifdef VALIDATE_PARMS
    if(!onu_id)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ngpon_tx_onu_id_elem >= 44))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ngpon_tx_onu_id_elem, NGPON_TX_CFG, TCONT_TO_ONU_ID_MAP, reg_tcont_to_onu_id_map);

    *onu_id = RU_FIELD_GET(0, NGPON_TX_CFG, TCONT_TO_ONU_ID_MAP, ONU_ID, reg_tcont_to_onu_id_map);

    return 0;
}

int ag_drv_ngpon_tx_cfg_tcont_flush_set(uint8_t flsh_q, uint8_t flsh_en, uint8_t flsh_imm)
{
    uint32_t reg_tcont_flush=0;

#ifdef VALIDATE_PARMS
    if((flsh_q >= _6BITS_MAX_VAL_) ||
       (flsh_en >= _1BITS_MAX_VAL_) ||
       (flsh_imm >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_tcont_flush = RU_FIELD_SET(0, NGPON_TX_CFG, TCONT_FLUSH, FLSH_Q, reg_tcont_flush, flsh_q);
    reg_tcont_flush = RU_FIELD_SET(0, NGPON_TX_CFG, TCONT_FLUSH, FLSH_EN, reg_tcont_flush, flsh_en);
    reg_tcont_flush = RU_FIELD_SET(0, NGPON_TX_CFG, TCONT_FLUSH, FLSH_IMM, reg_tcont_flush, flsh_imm);

    RU_REG_WRITE(0, NGPON_TX_CFG, TCONT_FLUSH, reg_tcont_flush);

    return 0;
}

int ag_drv_ngpon_tx_cfg_tcont_flush_get(uint8_t *flsh_q, uint8_t *flsh_en, uint8_t *flsh_imm)
{
    uint32_t reg_tcont_flush=0;

#ifdef VALIDATE_PARMS
    if(!flsh_q || !flsh_en || !flsh_imm)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_CFG, TCONT_FLUSH, reg_tcont_flush);

    *flsh_q = RU_FIELD_GET(0, NGPON_TX_CFG, TCONT_FLUSH, FLSH_Q, reg_tcont_flush);
    *flsh_en = RU_FIELD_GET(0, NGPON_TX_CFG, TCONT_FLUSH, FLSH_EN, reg_tcont_flush);
    *flsh_imm = RU_FIELD_GET(0, NGPON_TX_CFG, TCONT_FLUSH, FLSH_IMM, reg_tcont_flush);

    return 0;
}

int ag_drv_ngpon_tx_cfg_onu_id_to_plm_map_set(uint32_t ngpon_tx_onu_id_elem, uint16_t onu_id, uint8_t plm_num, uint8_t flbk_plm_num)
{
    uint32_t reg_onu_id_to_plm_map=0;

#ifdef VALIDATE_PARMS
    if((ngpon_tx_onu_id_elem >= 3) ||
       (onu_id >= _10BITS_MAX_VAL_) ||
       (plm_num >= _2BITS_MAX_VAL_) ||
       (flbk_plm_num >= _2BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_onu_id_to_plm_map = RU_FIELD_SET(0, NGPON_TX_CFG, ONU_ID_TO_PLM_MAP, ONU_ID, reg_onu_id_to_plm_map, onu_id);
    reg_onu_id_to_plm_map = RU_FIELD_SET(0, NGPON_TX_CFG, ONU_ID_TO_PLM_MAP, PLM_NUM, reg_onu_id_to_plm_map, plm_num);
    reg_onu_id_to_plm_map = RU_FIELD_SET(0, NGPON_TX_CFG, ONU_ID_TO_PLM_MAP, FLBK_PLM_NUM, reg_onu_id_to_plm_map, flbk_plm_num);

    RU_REG_RAM_WRITE(0, ngpon_tx_onu_id_elem, NGPON_TX_CFG, ONU_ID_TO_PLM_MAP, reg_onu_id_to_plm_map);

    return 0;
}

int ag_drv_ngpon_tx_cfg_onu_id_to_plm_map_get(uint32_t ngpon_tx_onu_id_elem, uint16_t *onu_id, uint8_t *plm_num, uint8_t *flbk_plm_num)
{
    uint32_t reg_onu_id_to_plm_map=0;

#ifdef VALIDATE_PARMS
    if(!onu_id || !plm_num || !flbk_plm_num)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ngpon_tx_onu_id_elem >= 3))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ngpon_tx_onu_id_elem, NGPON_TX_CFG, ONU_ID_TO_PLM_MAP, reg_onu_id_to_plm_map);

    *onu_id = RU_FIELD_GET(0, NGPON_TX_CFG, ONU_ID_TO_PLM_MAP, ONU_ID, reg_onu_id_to_plm_map);
    *plm_num = RU_FIELD_GET(0, NGPON_TX_CFG, ONU_ID_TO_PLM_MAP, PLM_NUM, reg_onu_id_to_plm_map);
    *flbk_plm_num = RU_FIELD_GET(0, NGPON_TX_CFG, ONU_ID_TO_PLM_MAP, FLBK_PLM_NUM, reg_onu_id_to_plm_map);

    return 0;
}

int ag_drv_ngpon_tx_cfg_tx_fifo_grp_cfg_set(uint32_t ngpon_tx_fifo_grp, uint16_t base, uint16_t size)
{
    uint32_t reg_tx_fifo_grp_cfg=0;

#ifdef VALIDATE_PARMS
    if((ngpon_tx_fifo_grp >= 4) ||
       (base >= _15BITS_MAX_VAL_) ||
       (size >= _15BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_tx_fifo_grp_cfg = RU_FIELD_SET(0, NGPON_TX_CFG, TX_FIFO_GRP_CFG, BASE, reg_tx_fifo_grp_cfg, base);
    reg_tx_fifo_grp_cfg = RU_FIELD_SET(0, NGPON_TX_CFG, TX_FIFO_GRP_CFG, SIZE, reg_tx_fifo_grp_cfg, size);

    RU_REG_RAM_WRITE(0, ngpon_tx_fifo_grp, NGPON_TX_CFG, TX_FIFO_GRP_CFG, reg_tx_fifo_grp_cfg);

    return 0;
}

int ag_drv_ngpon_tx_cfg_tx_fifo_grp_cfg_get(uint32_t ngpon_tx_fifo_grp, uint16_t *base, uint16_t *size)
{
    uint32_t reg_tx_fifo_grp_cfg=0;

#ifdef VALIDATE_PARMS
    if(!base || !size)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ngpon_tx_fifo_grp >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ngpon_tx_fifo_grp, NGPON_TX_CFG, TX_FIFO_GRP_CFG, reg_tx_fifo_grp_cfg);

    *base = RU_FIELD_GET(0, NGPON_TX_CFG, TX_FIFO_GRP_CFG, BASE, reg_tx_fifo_grp_cfg);
    *size = RU_FIELD_GET(0, NGPON_TX_CFG, TX_FIFO_GRP_CFG, SIZE, reg_tx_fifo_grp_cfg);

    return 0;
}

int ag_drv_ngpon_tx_cfg_tx_fifo_q_cfg_set(uint32_t ngpon_tx_fifo_q, uint16_t base, uint16_t size)
{
    uint32_t reg_tx_fifo_q_cfg=0;

#ifdef VALIDATE_PARMS
    if((ngpon_tx_fifo_q >= 8) ||
       (base >= _15BITS_MAX_VAL_) ||
       (size >= _15BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_tx_fifo_q_cfg = RU_FIELD_SET(0, NGPON_TX_CFG, TX_FIFO_Q_CFG, BASE, reg_tx_fifo_q_cfg, base);
    reg_tx_fifo_q_cfg = RU_FIELD_SET(0, NGPON_TX_CFG, TX_FIFO_Q_CFG, SIZE, reg_tx_fifo_q_cfg, size);

    RU_REG_RAM_WRITE(0, ngpon_tx_fifo_q, NGPON_TX_CFG, TX_FIFO_Q_CFG, reg_tx_fifo_q_cfg);

    return 0;
}

int ag_drv_ngpon_tx_cfg_tx_fifo_q_cfg_get(uint32_t ngpon_tx_fifo_q, uint16_t *base, uint16_t *size)
{
    uint32_t reg_tx_fifo_q_cfg=0;

#ifdef VALIDATE_PARMS
    if(!base || !size)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ngpon_tx_fifo_q >= 8))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ngpon_tx_fifo_q, NGPON_TX_CFG, TX_FIFO_Q_CFG, reg_tx_fifo_q_cfg);

    *base = RU_FIELD_GET(0, NGPON_TX_CFG, TX_FIFO_Q_CFG, BASE, reg_tx_fifo_q_cfg);
    *size = RU_FIELD_GET(0, NGPON_TX_CFG, TX_FIFO_Q_CFG, SIZE, reg_tx_fifo_q_cfg);

    return 0;
}

int ag_drv_ngpon_tx_cfg_pd_q_cfg_set(uint32_t ngpon_tx_pd_q, uint16_t base, uint16_t size)
{
    uint32_t reg_pd_q_cfg=0;

#ifdef VALIDATE_PARMS
    if((ngpon_tx_pd_q >= 8) ||
       (base >= _11BITS_MAX_VAL_) ||
       (size >= _11BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_pd_q_cfg = RU_FIELD_SET(0, NGPON_TX_CFG, PD_Q_CFG, BASE, reg_pd_q_cfg, base);
    reg_pd_q_cfg = RU_FIELD_SET(0, NGPON_TX_CFG, PD_Q_CFG, SIZE, reg_pd_q_cfg, size);

    RU_REG_RAM_WRITE(0, ngpon_tx_pd_q, NGPON_TX_CFG, PD_Q_CFG, reg_pd_q_cfg);

    return 0;
}

int ag_drv_ngpon_tx_cfg_pd_q_cfg_get(uint32_t ngpon_tx_pd_q, uint16_t *base, uint16_t *size)
{
    uint32_t reg_pd_q_cfg=0;

#ifdef VALIDATE_PARMS
    if(!base || !size)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ngpon_tx_pd_q >= 8))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ngpon_tx_pd_q, NGPON_TX_CFG, PD_Q_CFG, reg_pd_q_cfg);

    *base = RU_FIELD_GET(0, NGPON_TX_CFG, PD_Q_CFG, BASE, reg_pd_q_cfg);
    *size = RU_FIELD_GET(0, NGPON_TX_CFG, PD_Q_CFG, SIZE, reg_pd_q_cfg);

    return 0;
}

int ag_drv_ngpon_tx_cfg_pd_grp_cfg_set(uint32_t ngpon_tx_pd_grp, uint16_t base)
{
    uint32_t reg_pd_grp_cfg=0;

#ifdef VALIDATE_PARMS
    if((ngpon_tx_pd_grp >= 4) ||
       (base >= _11BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_pd_grp_cfg = RU_FIELD_SET(0, NGPON_TX_CFG, PD_GRP_CFG, BASE, reg_pd_grp_cfg, base);

    RU_REG_RAM_WRITE(0, ngpon_tx_pd_grp, NGPON_TX_CFG, PD_GRP_CFG, reg_pd_grp_cfg);

    return 0;
}

int ag_drv_ngpon_tx_cfg_pd_grp_cfg_get(uint32_t ngpon_tx_pd_grp, uint16_t *base)
{
    uint32_t reg_pd_grp_cfg=0;

#ifdef VALIDATE_PARMS
    if(!base)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ngpon_tx_pd_grp >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ngpon_tx_pd_grp, NGPON_TX_CFG, PD_GRP_CFG, reg_pd_grp_cfg);

    *base = RU_FIELD_GET(0, NGPON_TX_CFG, PD_GRP_CFG, BASE, reg_pd_grp_cfg);

    return 0;
}

int ag_drv_ngpon_tx_cfg_dbr_cfg_set(uint8_t dbr_src, uint8_t dbr_flush)
{
    uint32_t reg_dbr_cfg=0;

#ifdef VALIDATE_PARMS
    if((dbr_src >= _1BITS_MAX_VAL_) ||
       (dbr_flush >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_dbr_cfg = RU_FIELD_SET(0, NGPON_TX_CFG, DBR_CFG, DBR_SRC, reg_dbr_cfg, dbr_src);
    reg_dbr_cfg = RU_FIELD_SET(0, NGPON_TX_CFG, DBR_CFG, DBR_FLUSH, reg_dbr_cfg, dbr_flush);

    RU_REG_WRITE(0, NGPON_TX_CFG, DBR_CFG, reg_dbr_cfg);

    return 0;
}

int ag_drv_ngpon_tx_cfg_dbr_cfg_get(uint8_t *dbr_src, uint8_t *dbr_flush)
{
    uint32_t reg_dbr_cfg=0;

#ifdef VALIDATE_PARMS
    if(!dbr_src || !dbr_flush)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_CFG, DBR_CFG, reg_dbr_cfg);

    *dbr_src = RU_FIELD_GET(0, NGPON_TX_CFG, DBR_CFG, DBR_SRC, reg_dbr_cfg);
    *dbr_flush = RU_FIELD_GET(0, NGPON_TX_CFG, DBR_CFG, DBR_FLUSH, reg_dbr_cfg);

    return 0;
}

int ag_drv_ngpon_tx_cfg_dbr_set(uint32_t ngpon_tx_dbr, uint32_t bufocc)
{
    uint32_t reg_dbr=0;

#ifdef VALIDATE_PARMS
    if((ngpon_tx_dbr >= 40) ||
       (bufocc >= _24BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_dbr = RU_FIELD_SET(0, NGPON_TX_CFG, DBR, BUFOCC, reg_dbr, bufocc);

    RU_REG_RAM_WRITE(0, ngpon_tx_dbr, NGPON_TX_CFG, DBR, reg_dbr);

    return 0;
}

int ag_drv_ngpon_tx_cfg_dbr_get(uint32_t ngpon_tx_dbr, uint32_t *bufocc)
{
    uint32_t reg_dbr=0;

#ifdef VALIDATE_PARMS
    if(!bufocc)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ngpon_tx_dbr >= 40))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ngpon_tx_dbr, NGPON_TX_CFG, DBR, reg_dbr);

    *bufocc = RU_FIELD_GET(0, NGPON_TX_CFG, DBR, BUFOCC, reg_dbr);

    return 0;
}

int ag_drv_ngpon_tx_cfg_xgem_pyld_min_len_set(uint16_t value)
{
    uint32_t reg_xgem_pyld_min_len=0;

#ifdef VALIDATE_PARMS
    if((value >= _14BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_xgem_pyld_min_len = RU_FIELD_SET(0, NGPON_TX_CFG, XGEM_PYLD_MIN_LEN, VALUE, reg_xgem_pyld_min_len, value);

    RU_REG_WRITE(0, NGPON_TX_CFG, XGEM_PYLD_MIN_LEN, reg_xgem_pyld_min_len);

    return 0;
}

int ag_drv_ngpon_tx_cfg_xgem_pyld_min_len_get(uint16_t *value)
{
    uint32_t reg_xgem_pyld_min_len=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_CFG, XGEM_PYLD_MIN_LEN, reg_xgem_pyld_min_len);

    *value = RU_FIELD_GET(0, NGPON_TX_CFG, XGEM_PYLD_MIN_LEN, VALUE, reg_xgem_pyld_min_len);

    return 0;
}

int ag_drv_ngpon_tx_cfg_tcont_to_cnt_grp_map_set(uint8_t grp_0_q_num, uint8_t grp_1_q_num, uint8_t grp_2_q_num, uint8_t grp_3_q_num)
{
    uint32_t reg_tcont_to_cnt_grp_map=0;

#ifdef VALIDATE_PARMS
    if((grp_0_q_num >= _6BITS_MAX_VAL_) ||
       (grp_1_q_num >= _6BITS_MAX_VAL_) ||
       (grp_2_q_num >= _6BITS_MAX_VAL_) ||
       (grp_3_q_num >= _6BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_tcont_to_cnt_grp_map = RU_FIELD_SET(0, NGPON_TX_CFG, TCONT_TO_CNT_GRP_MAP, GRP_0_Q_NUM, reg_tcont_to_cnt_grp_map, grp_0_q_num);
    reg_tcont_to_cnt_grp_map = RU_FIELD_SET(0, NGPON_TX_CFG, TCONT_TO_CNT_GRP_MAP, GRP_1_Q_NUM, reg_tcont_to_cnt_grp_map, grp_1_q_num);
    reg_tcont_to_cnt_grp_map = RU_FIELD_SET(0, NGPON_TX_CFG, TCONT_TO_CNT_GRP_MAP, GRP_2_Q_NUM, reg_tcont_to_cnt_grp_map, grp_2_q_num);
    reg_tcont_to_cnt_grp_map = RU_FIELD_SET(0, NGPON_TX_CFG, TCONT_TO_CNT_GRP_MAP, GRP_3_Q_NUM, reg_tcont_to_cnt_grp_map, grp_3_q_num);

    RU_REG_WRITE(0, NGPON_TX_CFG, TCONT_TO_CNT_GRP_MAP, reg_tcont_to_cnt_grp_map);

    return 0;
}

int ag_drv_ngpon_tx_cfg_tcont_to_cnt_grp_map_get(uint8_t *grp_0_q_num, uint8_t *grp_1_q_num, uint8_t *grp_2_q_num, uint8_t *grp_3_q_num)
{
    uint32_t reg_tcont_to_cnt_grp_map=0;

#ifdef VALIDATE_PARMS
    if(!grp_0_q_num || !grp_1_q_num || !grp_2_q_num || !grp_3_q_num)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_CFG, TCONT_TO_CNT_GRP_MAP, reg_tcont_to_cnt_grp_map);

    *grp_0_q_num = RU_FIELD_GET(0, NGPON_TX_CFG, TCONT_TO_CNT_GRP_MAP, GRP_0_Q_NUM, reg_tcont_to_cnt_grp_map);
    *grp_1_q_num = RU_FIELD_GET(0, NGPON_TX_CFG, TCONT_TO_CNT_GRP_MAP, GRP_1_Q_NUM, reg_tcont_to_cnt_grp_map);
    *grp_2_q_num = RU_FIELD_GET(0, NGPON_TX_CFG, TCONT_TO_CNT_GRP_MAP, GRP_2_Q_NUM, reg_tcont_to_cnt_grp_map);
    *grp_3_q_num = RU_FIELD_GET(0, NGPON_TX_CFG, TCONT_TO_CNT_GRP_MAP, GRP_3_Q_NUM, reg_tcont_to_cnt_grp_map);

    return 0;
}

int ag_drv_ngpon_tx_cfg_plm_cfg_set(uint32_t tx_cfg_ploam_cfg_word, uint8_t prsstnt, uint8_t valid, uint8_t sw_en, uint8_t use_def)
{
    uint32_t reg_plm_cfg=0;

#ifdef VALIDATE_PARMS
    if((tx_cfg_ploam_cfg_word >= 3) ||
       (prsstnt >= _1BITS_MAX_VAL_) ||
       (valid >= _1BITS_MAX_VAL_) ||
       (sw_en >= _1BITS_MAX_VAL_) ||
       (use_def >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_plm_cfg = RU_FIELD_SET(0, NGPON_TX_CFG, PLM_CFG, PRSSTNT, reg_plm_cfg, prsstnt);
    reg_plm_cfg = RU_FIELD_SET(0, NGPON_TX_CFG, PLM_CFG, VALID, reg_plm_cfg, valid);
    reg_plm_cfg = RU_FIELD_SET(0, NGPON_TX_CFG, PLM_CFG, SW_EN, reg_plm_cfg, sw_en);
    reg_plm_cfg = RU_FIELD_SET(0, NGPON_TX_CFG, PLM_CFG, USE_DEF, reg_plm_cfg, use_def);

    RU_REG_RAM_WRITE(0, tx_cfg_ploam_cfg_word, NGPON_TX_CFG, PLM_CFG, reg_plm_cfg);

    return 0;
}

int ag_drv_ngpon_tx_cfg_plm_cfg_get(uint32_t tx_cfg_ploam_cfg_word, uint8_t *prsstnt, uint8_t *valid, uint8_t *sw_en, uint8_t *use_def)
{
    uint32_t reg_plm_cfg=0;

#ifdef VALIDATE_PARMS
    if(!prsstnt || !valid || !sw_en || !use_def)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((tx_cfg_ploam_cfg_word >= 3))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, tx_cfg_ploam_cfg_word, NGPON_TX_CFG, PLM_CFG, reg_plm_cfg);

    *prsstnt = RU_FIELD_GET(0, NGPON_TX_CFG, PLM_CFG, PRSSTNT, reg_plm_cfg);
    *valid = RU_FIELD_GET(0, NGPON_TX_CFG, PLM_CFG, VALID, reg_plm_cfg);
    *sw_en = RU_FIELD_GET(0, NGPON_TX_CFG, PLM_CFG, SW_EN, reg_plm_cfg);
    *use_def = RU_FIELD_GET(0, NGPON_TX_CFG, PLM_CFG, USE_DEF, reg_plm_cfg);

    return 0;
}

int ag_drv_ngpon_tx_cfg_rogue_onu_ctrl_set(uint8_t rogue_level_clr, uint8_t rogue_diff_clr, uint8_t rogue_dv_sel)
{
    uint32_t reg_rogue_onu_ctrl=0;

#ifdef VALIDATE_PARMS
    if((rogue_level_clr >= _1BITS_MAX_VAL_) ||
       (rogue_diff_clr >= _1BITS_MAX_VAL_) ||
       (rogue_dv_sel >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_rogue_onu_ctrl = RU_FIELD_SET(0, NGPON_TX_CFG, ROGUE_ONU_CTRL, ROGUE_LEVEL_CLR, reg_rogue_onu_ctrl, rogue_level_clr);
    reg_rogue_onu_ctrl = RU_FIELD_SET(0, NGPON_TX_CFG, ROGUE_ONU_CTRL, ROGUE_DIFF_CLR, reg_rogue_onu_ctrl, rogue_diff_clr);
    reg_rogue_onu_ctrl = RU_FIELD_SET(0, NGPON_TX_CFG, ROGUE_ONU_CTRL, ROGUE_DV_SEL, reg_rogue_onu_ctrl, rogue_dv_sel);

    RU_REG_WRITE(0, NGPON_TX_CFG, ROGUE_ONU_CTRL, reg_rogue_onu_ctrl);

    return 0;
}

int ag_drv_ngpon_tx_cfg_rogue_onu_ctrl_get(uint8_t *rogue_level_clr, uint8_t *rogue_diff_clr, uint8_t *rogue_dv_sel)
{
    uint32_t reg_rogue_onu_ctrl=0;

#ifdef VALIDATE_PARMS
    if(!rogue_level_clr || !rogue_diff_clr || !rogue_dv_sel)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_CFG, ROGUE_ONU_CTRL, reg_rogue_onu_ctrl);

    *rogue_level_clr = RU_FIELD_GET(0, NGPON_TX_CFG, ROGUE_ONU_CTRL, ROGUE_LEVEL_CLR, reg_rogue_onu_ctrl);
    *rogue_diff_clr = RU_FIELD_GET(0, NGPON_TX_CFG, ROGUE_ONU_CTRL, ROGUE_DIFF_CLR, reg_rogue_onu_ctrl);
    *rogue_dv_sel = RU_FIELD_GET(0, NGPON_TX_CFG, ROGUE_ONU_CTRL, ROGUE_DV_SEL, reg_rogue_onu_ctrl);

    return 0;
}

int ag_drv_ngpon_tx_cfg_rogue_level_time_set(uint32_t window_size)
{
    uint32_t reg_rogue_level_time=0;

#ifdef VALIDATE_PARMS
#endif

    reg_rogue_level_time = RU_FIELD_SET(0, NGPON_TX_CFG, ROGUE_LEVEL_TIME, WINDOW_SIZE, reg_rogue_level_time, window_size);

    RU_REG_WRITE(0, NGPON_TX_CFG, ROGUE_LEVEL_TIME, reg_rogue_level_time);

    return 0;
}

int ag_drv_ngpon_tx_cfg_rogue_level_time_get(uint32_t *window_size)
{
    uint32_t reg_rogue_level_time=0;

#ifdef VALIDATE_PARMS
    if(!window_size)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_CFG, ROGUE_LEVEL_TIME, reg_rogue_level_time);

    *window_size = RU_FIELD_GET(0, NGPON_TX_CFG, ROGUE_LEVEL_TIME, WINDOW_SIZE, reg_rogue_level_time);

    return 0;
}

int ag_drv_ngpon_tx_cfg_rogue_diff_time_set(uint16_t window_size)
{
    uint32_t reg_rogue_diff_time=0;

#ifdef VALIDATE_PARMS
    if((window_size >= _10BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_rogue_diff_time = RU_FIELD_SET(0, NGPON_TX_CFG, ROGUE_DIFF_TIME, WINDOW_SIZE, reg_rogue_diff_time, window_size);

    RU_REG_WRITE(0, NGPON_TX_CFG, ROGUE_DIFF_TIME, reg_rogue_diff_time);

    return 0;
}

int ag_drv_ngpon_tx_cfg_rogue_diff_time_get(uint16_t *window_size)
{
    uint32_t reg_rogue_diff_time=0;

#ifdef VALIDATE_PARMS
    if(!window_size)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_CFG, ROGUE_DIFF_TIME, reg_rogue_diff_time);

    *window_size = RU_FIELD_GET(0, NGPON_TX_CFG, ROGUE_DIFF_TIME, WINDOW_SIZE, reg_rogue_diff_time);

    return 0;
}

int ag_drv_ngpon_tx_cfg_lpb_q_num_set(uint8_t value)
{
    uint32_t reg_lpb_q_num=0;

#ifdef VALIDATE_PARMS
    if((value >= _6BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_lpb_q_num = RU_FIELD_SET(0, NGPON_TX_CFG, LPB_Q_NUM, VALUE, reg_lpb_q_num, value);

    RU_REG_WRITE(0, NGPON_TX_CFG, LPB_Q_NUM, reg_lpb_q_num);

    return 0;
}

int ag_drv_ngpon_tx_cfg_lpb_q_num_get(uint8_t *value)
{
    uint32_t reg_lpb_q_num=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_CFG, LPB_Q_NUM, reg_lpb_q_num);

    *value = RU_FIELD_GET(0, NGPON_TX_CFG, LPB_Q_NUM, VALUE, reg_lpb_q_num);

    return 0;
}

int ag_drv_ngpon_tx_cfg_xgem_hdr_opt_set(uint32_t value)
{
    uint32_t reg_xgem_hdr_opt=0;

#ifdef VALIDATE_PARMS
    if((value >= _18BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_xgem_hdr_opt = RU_FIELD_SET(0, NGPON_TX_CFG, XGEM_HDR_OPT, VALUE, reg_xgem_hdr_opt, value);

    RU_REG_WRITE(0, NGPON_TX_CFG, XGEM_HDR_OPT, reg_xgem_hdr_opt);

    return 0;
}

int ag_drv_ngpon_tx_cfg_xgem_hdr_opt_get(uint32_t *value)
{
    uint32_t reg_xgem_hdr_opt=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_CFG, XGEM_HDR_OPT, reg_xgem_hdr_opt);

    *value = RU_FIELD_GET(0, NGPON_TX_CFG, XGEM_HDR_OPT, VALUE, reg_xgem_hdr_opt);

    return 0;
}

int ag_drv_ngpon_tx_cfg_plm_0_stat_get(ngpon_tx_cfg_plm_0_stat *plm_0_stat)
{
    uint32_t reg_plm_0_stat=0;

#ifdef VALIDATE_PARMS
    if(!plm_0_stat)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_CFG, PLM_0_STAT, reg_plm_0_stat);

    plm_0_stat->occupy = RU_FIELD_GET(0, NGPON_TX_CFG, PLM_0_STAT, OCCUPY, reg_plm_0_stat);
    plm_0_stat->rd_adr = RU_FIELD_GET(0, NGPON_TX_CFG, PLM_0_STAT, RD_ADR, reg_plm_0_stat);
    plm_0_stat->wr_adr = RU_FIELD_GET(0, NGPON_TX_CFG, PLM_0_STAT, WR_ADR, reg_plm_0_stat);
    plm_0_stat->af = RU_FIELD_GET(0, NGPON_TX_CFG, PLM_0_STAT, AF, reg_plm_0_stat);
    plm_0_stat->f = RU_FIELD_GET(0, NGPON_TX_CFG, PLM_0_STAT, F, reg_plm_0_stat);
    plm_0_stat->ae = RU_FIELD_GET(0, NGPON_TX_CFG, PLM_0_STAT, AE, reg_plm_0_stat);
    plm_0_stat->e = RU_FIELD_GET(0, NGPON_TX_CFG, PLM_0_STAT, E, reg_plm_0_stat);

    return 0;
}

int ag_drv_ngpon_tx_cfg_line_rate_set(uint8_t value)
{
    uint32_t reg_line_rate=0;

#ifdef VALIDATE_PARMS
    if((value >= _2BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_line_rate = RU_FIELD_SET(0, NGPON_TX_CFG, LINE_RATE, VALUE, reg_line_rate, value);

    RU_REG_WRITE(0, NGPON_TX_CFG, LINE_RATE, reg_line_rate);

    return 0;
}

int ag_drv_ngpon_tx_cfg_line_rate_get(uint8_t *value)
{
    uint32_t reg_line_rate=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_CFG, LINE_RATE, reg_line_rate);

    *value = RU_FIELD_GET(0, NGPON_TX_CFG, LINE_RATE, VALUE, reg_line_rate);

    return 0;
}

int ag_drv_ngpon_tx_cfg_start_offset_set(uint16_t value)
{
    uint32_t reg_start_offset=0;

#ifdef VALIDATE_PARMS
    if((value >= _10BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_start_offset = RU_FIELD_SET(0, NGPON_TX_CFG, START_OFFSET, VALUE, reg_start_offset, value);

    RU_REG_WRITE(0, NGPON_TX_CFG, START_OFFSET, reg_start_offset);

    return 0;
}

int ag_drv_ngpon_tx_cfg_start_offset_get(uint16_t *value)
{
    uint32_t reg_start_offset=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_CFG, START_OFFSET, reg_start_offset);

    *value = RU_FIELD_GET(0, NGPON_TX_CFG, START_OFFSET, VALUE, reg_start_offset);

    return 0;
}

int ag_drv_ngpon_tx_cfg_frame_length_minus_1_set(uint32_t value)
{
    uint32_t reg_frame_length_minus_1=0;

#ifdef VALIDATE_PARMS
    if((value >= _18BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_frame_length_minus_1 = RU_FIELD_SET(0, NGPON_TX_CFG, FRAME_LENGTH_MINUS_1, VALUE, reg_frame_length_minus_1, value);

    RU_REG_WRITE(0, NGPON_TX_CFG, FRAME_LENGTH_MINUS_1, reg_frame_length_minus_1);

    return 0;
}

int ag_drv_ngpon_tx_cfg_frame_length_minus_1_get(uint32_t *value)
{
    uint32_t reg_frame_length_minus_1=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_CFG, FRAME_LENGTH_MINUS_1, reg_frame_length_minus_1);

    *value = RU_FIELD_GET(0, NGPON_TX_CFG, FRAME_LENGTH_MINUS_1, VALUE, reg_frame_length_minus_1);

    return 0;
}

int ag_drv_ngpon_tx_cfg_idle_xgem_pyld_len_set(uint16_t value)
{
    uint32_t reg_idle_xgem_pyld_len=0;

#ifdef VALIDATE_PARMS
    if((value >= _12BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_idle_xgem_pyld_len = RU_FIELD_SET(0, NGPON_TX_CFG, IDLE_XGEM_PYLD_LEN, VALUE, reg_idle_xgem_pyld_len, value);

    RU_REG_WRITE(0, NGPON_TX_CFG, IDLE_XGEM_PYLD_LEN, reg_idle_xgem_pyld_len);

    return 0;
}

int ag_drv_ngpon_tx_cfg_idle_xgem_pyld_len_get(uint16_t *value)
{
    uint32_t reg_idle_xgem_pyld_len=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_CFG, IDLE_XGEM_PYLD_LEN, reg_idle_xgem_pyld_len);

    *value = RU_FIELD_GET(0, NGPON_TX_CFG, IDLE_XGEM_PYLD_LEN, VALUE, reg_idle_xgem_pyld_len);

    return 0;
}

int ag_drv_ngpon_tx_cfg_af_err_filt_set(const ngpon_tx_cfg_af_err_filt *af_err_filt)
{
    uint32_t reg_af_err_filt=0;

#ifdef VALIDATE_PARMS
    if(!af_err_filt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((af_err_filt->big_brst_filt_en >= _1BITS_MAX_VAL_) ||
       (af_err_filt->big_grnt_filt_en >= _1BITS_MAX_VAL_) ||
       (af_err_filt->too_many_accs_filt_en >= _1BITS_MAX_VAL_) ||
       (af_err_filt->not_first_accs_plm_filt_en >= _1BITS_MAX_VAL_) ||
       (af_err_filt->late_accs_filt_en >= _1BITS_MAX_VAL_) ||
       (af_err_filt->wrng_gnt_size_filt_en >= _1BITS_MAX_VAL_) ||
       (af_err_filt->prof_filt_en >= _1BITS_MAX_VAL_) ||
       (af_err_filt->trim_too_big_grnt_en >= _1BITS_MAX_VAL_) ||
       (af_err_filt->start_time_filt_en >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_af_err_filt = RU_FIELD_SET(0, NGPON_TX_CFG, AF_ERR_FILT, BIG_BRST_FILT_EN, reg_af_err_filt, af_err_filt->big_brst_filt_en);
    reg_af_err_filt = RU_FIELD_SET(0, NGPON_TX_CFG, AF_ERR_FILT, BIG_GRNT_FILT_EN, reg_af_err_filt, af_err_filt->big_grnt_filt_en);
    reg_af_err_filt = RU_FIELD_SET(0, NGPON_TX_CFG, AF_ERR_FILT, TOO_MANY_ACCS_FILT_EN, reg_af_err_filt, af_err_filt->too_many_accs_filt_en);
    reg_af_err_filt = RU_FIELD_SET(0, NGPON_TX_CFG, AF_ERR_FILT, NOT_FIRST_ACCS_PLM_FILT_EN, reg_af_err_filt, af_err_filt->not_first_accs_plm_filt_en);
    reg_af_err_filt = RU_FIELD_SET(0, NGPON_TX_CFG, AF_ERR_FILT, LATE_ACCS_FILT_EN, reg_af_err_filt, af_err_filt->late_accs_filt_en);
    reg_af_err_filt = RU_FIELD_SET(0, NGPON_TX_CFG, AF_ERR_FILT, WRNG_GNT_SIZE_FILT_EN, reg_af_err_filt, af_err_filt->wrng_gnt_size_filt_en);
    reg_af_err_filt = RU_FIELD_SET(0, NGPON_TX_CFG, AF_ERR_FILT, PROF_FILT_EN, reg_af_err_filt, af_err_filt->prof_filt_en);
    reg_af_err_filt = RU_FIELD_SET(0, NGPON_TX_CFG, AF_ERR_FILT, TRIM_TOO_BIG_GRNT_EN, reg_af_err_filt, af_err_filt->trim_too_big_grnt_en);
    reg_af_err_filt = RU_FIELD_SET(0, NGPON_TX_CFG, AF_ERR_FILT, START_TIME_FILT_EN, reg_af_err_filt, af_err_filt->start_time_filt_en);

    RU_REG_WRITE(0, NGPON_TX_CFG, AF_ERR_FILT, reg_af_err_filt);

    return 0;
}

int ag_drv_ngpon_tx_cfg_af_err_filt_get(ngpon_tx_cfg_af_err_filt *af_err_filt)
{
    uint32_t reg_af_err_filt=0;

#ifdef VALIDATE_PARMS
    if(!af_err_filt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_CFG, AF_ERR_FILT, reg_af_err_filt);

    af_err_filt->big_brst_filt_en = RU_FIELD_GET(0, NGPON_TX_CFG, AF_ERR_FILT, BIG_BRST_FILT_EN, reg_af_err_filt);
    af_err_filt->big_grnt_filt_en = RU_FIELD_GET(0, NGPON_TX_CFG, AF_ERR_FILT, BIG_GRNT_FILT_EN, reg_af_err_filt);
    af_err_filt->too_many_accs_filt_en = RU_FIELD_GET(0, NGPON_TX_CFG, AF_ERR_FILT, TOO_MANY_ACCS_FILT_EN, reg_af_err_filt);
    af_err_filt->not_first_accs_plm_filt_en = RU_FIELD_GET(0, NGPON_TX_CFG, AF_ERR_FILT, NOT_FIRST_ACCS_PLM_FILT_EN, reg_af_err_filt);
    af_err_filt->late_accs_filt_en = RU_FIELD_GET(0, NGPON_TX_CFG, AF_ERR_FILT, LATE_ACCS_FILT_EN, reg_af_err_filt);
    af_err_filt->wrng_gnt_size_filt_en = RU_FIELD_GET(0, NGPON_TX_CFG, AF_ERR_FILT, WRNG_GNT_SIZE_FILT_EN, reg_af_err_filt);
    af_err_filt->prof_filt_en = RU_FIELD_GET(0, NGPON_TX_CFG, AF_ERR_FILT, PROF_FILT_EN, reg_af_err_filt);
    af_err_filt->trim_too_big_grnt_en = RU_FIELD_GET(0, NGPON_TX_CFG, AF_ERR_FILT, TRIM_TOO_BIG_GRNT_EN, reg_af_err_filt);
    af_err_filt->start_time_filt_en = RU_FIELD_GET(0, NGPON_TX_CFG, AF_ERR_FILT, START_TIME_FILT_EN, reg_af_err_filt);

    return 0;
}

int ag_drv_ngpon_tx_cfg_dying_gasp_set(uint8_t dg)
{
    uint32_t reg_dying_gasp=0;

#ifdef VALIDATE_PARMS
    if((dg >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_dying_gasp = RU_FIELD_SET(0, NGPON_TX_CFG, DYING_GASP, DG, reg_dying_gasp, dg);

    RU_REG_WRITE(0, NGPON_TX_CFG, DYING_GASP, reg_dying_gasp);

    return 0;
}

int ag_drv_ngpon_tx_cfg_dying_gasp_get(uint8_t *dg)
{
    uint32_t reg_dying_gasp=0;

#ifdef VALIDATE_PARMS
    if(!dg)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_CFG, DYING_GASP, reg_dying_gasp);

    *dg = RU_FIELD_GET(0, NGPON_TX_CFG, DYING_GASP, DG, reg_dying_gasp);

    return 0;
}

int ag_drv_ngpon_tx_cfg_fragmentation_disable_set(uint8_t frgment_dis)
{
    uint32_t reg_fragmentation_disable=0;

#ifdef VALIDATE_PARMS
    if((frgment_dis >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_fragmentation_disable = RU_FIELD_SET(0, NGPON_TX_CFG, FRAGMENTATION_DISABLE, FRGMENT_DIS, reg_fragmentation_disable, frgment_dis);

    RU_REG_WRITE(0, NGPON_TX_CFG, FRAGMENTATION_DISABLE, reg_fragmentation_disable);

    return 0;
}

int ag_drv_ngpon_tx_cfg_fragmentation_disable_get(uint8_t *frgment_dis)
{
    uint32_t reg_fragmentation_disable=0;

#ifdef VALIDATE_PARMS
    if(!frgment_dis)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_CFG, FRAGMENTATION_DISABLE, reg_fragmentation_disable);

    *frgment_dis = RU_FIELD_GET(0, NGPON_TX_CFG, FRAGMENTATION_DISABLE, FRGMENT_DIS, reg_fragmentation_disable);

    return 0;
}

int ag_drv_ngpon_tx_cfg_xgtc_header_ind_7_1_set(uint8_t bits_7_1)
{
    uint32_t reg_xgtc_header_ind_7_1=0;

#ifdef VALIDATE_PARMS
    if((bits_7_1 >= _7BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_xgtc_header_ind_7_1 = RU_FIELD_SET(0, NGPON_TX_CFG, XGTC_HEADER_IND_7_1, BITS_7_1, reg_xgtc_header_ind_7_1, bits_7_1);

    RU_REG_WRITE(0, NGPON_TX_CFG, XGTC_HEADER_IND_7_1, reg_xgtc_header_ind_7_1);

    return 0;
}

int ag_drv_ngpon_tx_cfg_xgtc_header_ind_7_1_get(uint8_t *bits_7_1)
{
    uint32_t reg_xgtc_header_ind_7_1=0;

#ifdef VALIDATE_PARMS
    if(!bits_7_1)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_CFG, XGTC_HEADER_IND_7_1, reg_xgtc_header_ind_7_1);

    *bits_7_1 = RU_FIELD_GET(0, NGPON_TX_CFG, XGTC_HEADER_IND_7_1, BITS_7_1, reg_xgtc_header_ind_7_1);

    return 0;
}



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
#include "ngpon_rx_enc.h"
int ag_drv_ngpon_rx_enc_key_valid_set(const ngpon_rx_enc_key_valid *key_valid)
{
    uint32_t reg_key_valid=0;

#ifdef VALIDATE_PARMS
    if(!key_valid)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, 1);
        return 1;
    }
    if((key_valid->key1_onu1 >= _1BITS_MAX_VAL_) ||
       (key_valid->key2_onu1 >= _1BITS_MAX_VAL_) ||
       (key_valid->key1_onu2 >= _1BITS_MAX_VAL_) ||
       (key_valid->key2_onu2 >= _1BITS_MAX_VAL_) ||
       (key_valid->key1_mcst >= _1BITS_MAX_VAL_) ||
       (key_valid->key2_mcst >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_key_valid = RU_FIELD_SET(0, NGPON_RX_ENC, KEY_VALID, KEY1_ONU1, reg_key_valid, key_valid->key1_onu1);
    reg_key_valid = RU_FIELD_SET(0, NGPON_RX_ENC, KEY_VALID, KEY2_ONU1, reg_key_valid, key_valid->key2_onu1);
    reg_key_valid = RU_FIELD_SET(0, NGPON_RX_ENC, KEY_VALID, KEY1_ONU2, reg_key_valid, key_valid->key1_onu2);
    reg_key_valid = RU_FIELD_SET(0, NGPON_RX_ENC, KEY_VALID, KEY2_ONU2, reg_key_valid, key_valid->key2_onu2);
    reg_key_valid = RU_FIELD_SET(0, NGPON_RX_ENC, KEY_VALID, KEY1_MCST, reg_key_valid, key_valid->key1_mcst);
    reg_key_valid = RU_FIELD_SET(0, NGPON_RX_ENC, KEY_VALID, KEY2_MCST, reg_key_valid, key_valid->key2_mcst);

    RU_REG_WRITE(0, NGPON_RX_ENC, KEY_VALID, reg_key_valid);

    return 0;
}

int ag_drv_ngpon_rx_enc_key_valid_get(ngpon_rx_enc_key_valid *key_valid)
{
    uint32_t reg_key_valid=0;

#ifdef VALIDATE_PARMS
    if(!key_valid)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_ENC, KEY_VALID, reg_key_valid);

    key_valid->key1_onu1 = RU_FIELD_GET(0, NGPON_RX_ENC, KEY_VALID, KEY1_ONU1, reg_key_valid);
    key_valid->key2_onu1 = RU_FIELD_GET(0, NGPON_RX_ENC, KEY_VALID, KEY2_ONU1, reg_key_valid);
    key_valid->key1_onu2 = RU_FIELD_GET(0, NGPON_RX_ENC, KEY_VALID, KEY1_ONU2, reg_key_valid);
    key_valid->key2_onu2 = RU_FIELD_GET(0, NGPON_RX_ENC, KEY_VALID, KEY2_ONU2, reg_key_valid);
    key_valid->key1_mcst = RU_FIELD_GET(0, NGPON_RX_ENC, KEY_VALID, KEY1_MCST, reg_key_valid);
    key_valid->key2_mcst = RU_FIELD_GET(0, NGPON_RX_ENC, KEY_VALID, KEY2_MCST, reg_key_valid);

    return 0;
}

int ag_drv_ngpon_rx_enc_ploam_load_set(uint8_t ploamik_onu1, uint8_t ploamik_onu2, uint8_t ploamik_bcst)
{
    uint32_t reg_ploam_load=0;

#ifdef VALIDATE_PARMS
    if((ploamik_onu1 >= _1BITS_MAX_VAL_) ||
       (ploamik_onu2 >= _1BITS_MAX_VAL_) ||
       (ploamik_bcst >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_ploam_load = RU_FIELD_SET(0, NGPON_RX_ENC, PLOAM_LOAD, PLOAMIK_ONU1, reg_ploam_load, ploamik_onu1);
    reg_ploam_load = RU_FIELD_SET(0, NGPON_RX_ENC, PLOAM_LOAD, PLOAMIK_ONU2, reg_ploam_load, ploamik_onu2);
    reg_ploam_load = RU_FIELD_SET(0, NGPON_RX_ENC, PLOAM_LOAD, PLOAMIK_BCST, reg_ploam_load, ploamik_bcst);

    RU_REG_WRITE(0, NGPON_RX_ENC, PLOAM_LOAD, reg_ploam_load);

    return 0;
}

int ag_drv_ngpon_rx_enc_ploam_load_get(uint8_t *ploamik_onu1, uint8_t *ploamik_onu2, uint8_t *ploamik_bcst)
{
    uint32_t reg_ploam_load=0;

#ifdef VALIDATE_PARMS
    if(!ploamik_onu1 || !ploamik_onu2 || !ploamik_bcst)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_ENC, PLOAM_LOAD, reg_ploam_load);

    *ploamik_onu1 = RU_FIELD_GET(0, NGPON_RX_ENC, PLOAM_LOAD, PLOAMIK_ONU1, reg_ploam_load);
    *ploamik_onu2 = RU_FIELD_GET(0, NGPON_RX_ENC, PLOAM_LOAD, PLOAMIK_ONU2, reg_ploam_load);
    *ploamik_bcst = RU_FIELD_GET(0, NGPON_RX_ENC, PLOAM_LOAD, PLOAMIK_BCST, reg_ploam_load);

    return 0;
}

int ag_drv_ngpon_rx_enc_key1_onu1_set(uint32_t key1_onu1_word_idx, uint32_t value)
{
    uint32_t reg_key1_onu1=0;

#ifdef VALIDATE_PARMS
    if((key1_onu1_word_idx >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_key1_onu1 = RU_FIELD_SET(0, NGPON_RX_ENC, KEY1_ONU1, VALUE, reg_key1_onu1, value);

    RU_REG_RAM_WRITE(0, key1_onu1_word_idx, NGPON_RX_ENC, KEY1_ONU1, reg_key1_onu1);

    return 0;
}

int ag_drv_ngpon_rx_enc_key1_onu1_get(uint32_t key1_onu1_word_idx, uint32_t *value)
{
    uint32_t reg_key1_onu1=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((key1_onu1_word_idx >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, key1_onu1_word_idx, NGPON_RX_ENC, KEY1_ONU1, reg_key1_onu1);

    *value = RU_FIELD_GET(0, NGPON_RX_ENC, KEY1_ONU1, VALUE, reg_key1_onu1);

    return 0;
}

int ag_drv_ngpon_rx_enc_key2_onu1_set(uint32_t key2_onu1_word_idx, uint32_t value)
{
    uint32_t reg_key2_onu1=0;

#ifdef VALIDATE_PARMS
    if((key2_onu1_word_idx >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_key2_onu1 = RU_FIELD_SET(0, NGPON_RX_ENC, KEY2_ONU1, VALUE, reg_key2_onu1, value);

    RU_REG_RAM_WRITE(0, key2_onu1_word_idx, NGPON_RX_ENC, KEY2_ONU1, reg_key2_onu1);

    return 0;
}

int ag_drv_ngpon_rx_enc_key2_onu1_get(uint32_t key2_onu1_word_idx, uint32_t *value)
{
    uint32_t reg_key2_onu1=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((key2_onu1_word_idx >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, key2_onu1_word_idx, NGPON_RX_ENC, KEY2_ONU1, reg_key2_onu1);

    *value = RU_FIELD_GET(0, NGPON_RX_ENC, KEY2_ONU1, VALUE, reg_key2_onu1);

    return 0;
}

int ag_drv_ngpon_rx_enc_key1_onu2_set(uint32_t key1_onu2_word_idx, uint32_t value)
{
    uint32_t reg_key1_onu2=0;

#ifdef VALIDATE_PARMS
    if((key1_onu2_word_idx >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_key1_onu2 = RU_FIELD_SET(0, NGPON_RX_ENC, KEY1_ONU2, VALUE, reg_key1_onu2, value);

    RU_REG_RAM_WRITE(0, key1_onu2_word_idx, NGPON_RX_ENC, KEY1_ONU2, reg_key1_onu2);

    return 0;
}

int ag_drv_ngpon_rx_enc_key1_onu2_get(uint32_t key1_onu2_word_idx, uint32_t *value)
{
    uint32_t reg_key1_onu2=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((key1_onu2_word_idx >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, key1_onu2_word_idx, NGPON_RX_ENC, KEY1_ONU2, reg_key1_onu2);

    *value = RU_FIELD_GET(0, NGPON_RX_ENC, KEY1_ONU2, VALUE, reg_key1_onu2);

    return 0;
}

int ag_drv_ngpon_rx_enc_key2_onu2_set(uint32_t key2_onu2_word_idx, uint32_t value)
{
    uint32_t reg_key2_onu2=0;

#ifdef VALIDATE_PARMS
    if((key2_onu2_word_idx >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_key2_onu2 = RU_FIELD_SET(0, NGPON_RX_ENC, KEY2_ONU2, VALUE, reg_key2_onu2, value);

    RU_REG_RAM_WRITE(0, key2_onu2_word_idx, NGPON_RX_ENC, KEY2_ONU2, reg_key2_onu2);

    return 0;
}

int ag_drv_ngpon_rx_enc_key2_onu2_get(uint32_t key2_onu2_word_idx, uint32_t *value)
{
    uint32_t reg_key2_onu2=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((key2_onu2_word_idx >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, key2_onu2_word_idx, NGPON_RX_ENC, KEY2_ONU2, reg_key2_onu2);

    *value = RU_FIELD_GET(0, NGPON_RX_ENC, KEY2_ONU2, VALUE, reg_key2_onu2);

    return 0;
}

int ag_drv_ngpon_rx_enc_ploamik_onu1_set(uint32_t ploam_ik_onu1_word_idx, uint32_t value)
{
    uint32_t reg_ploamik_onu1=0;

#ifdef VALIDATE_PARMS
    if((ploam_ik_onu1_word_idx >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_ploamik_onu1 = RU_FIELD_SET(0, NGPON_RX_ENC, PLOAMIK_ONU1, VALUE, reg_ploamik_onu1, value);

    RU_REG_RAM_WRITE(0, ploam_ik_onu1_word_idx, NGPON_RX_ENC, PLOAMIK_ONU1, reg_ploamik_onu1);

    return 0;
}

int ag_drv_ngpon_rx_enc_ploamik_onu1_get(uint32_t ploam_ik_onu1_word_idx, uint32_t *value)
{
    uint32_t reg_ploamik_onu1=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ploam_ik_onu1_word_idx >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ploam_ik_onu1_word_idx, NGPON_RX_ENC, PLOAMIK_ONU1, reg_ploamik_onu1);

    *value = RU_FIELD_GET(0, NGPON_RX_ENC, PLOAMIK_ONU1, VALUE, reg_ploamik_onu1);

    return 0;
}

int ag_drv_ngpon_rx_enc_ploamik_onu2_set(uint32_t ploam_ik_onu2_word_idx, uint32_t value)
{
    uint32_t reg_ploamik_onu2=0;

#ifdef VALIDATE_PARMS
    if((ploam_ik_onu2_word_idx >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_ploamik_onu2 = RU_FIELD_SET(0, NGPON_RX_ENC, PLOAMIK_ONU2, VALUE, reg_ploamik_onu2, value);

    RU_REG_RAM_WRITE(0, ploam_ik_onu2_word_idx, NGPON_RX_ENC, PLOAMIK_ONU2, reg_ploamik_onu2);

    return 0;
}

int ag_drv_ngpon_rx_enc_ploamik_onu2_get(uint32_t ploam_ik_onu2_word_idx, uint32_t *value)
{
    uint32_t reg_ploamik_onu2=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ploam_ik_onu2_word_idx >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ploam_ik_onu2_word_idx, NGPON_RX_ENC, PLOAMIK_ONU2, reg_ploamik_onu2);

    *value = RU_FIELD_GET(0, NGPON_RX_ENC, PLOAMIK_ONU2, VALUE, reg_ploamik_onu2);

    return 0;
}

int ag_drv_ngpon_rx_enc_ploamik_bcst_set(uint32_t ploam_ik_bcst_word_idx, uint32_t value)
{
    uint32_t reg_ploamik_bcst=0;

#ifdef VALIDATE_PARMS
    if((ploam_ik_bcst_word_idx >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_ploamik_bcst = RU_FIELD_SET(0, NGPON_RX_ENC, PLOAMIK_BCST, VALUE, reg_ploamik_bcst, value);

    RU_REG_RAM_WRITE(0, ploam_ik_bcst_word_idx, NGPON_RX_ENC, PLOAMIK_BCST, reg_ploamik_bcst);

    return 0;
}

int ag_drv_ngpon_rx_enc_ploamik_bcst_get(uint32_t ploam_ik_bcst_word_idx, uint32_t *value)
{
    uint32_t reg_ploamik_bcst=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ploam_ik_bcst_word_idx >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ploam_ik_bcst_word_idx, NGPON_RX_ENC, PLOAMIK_BCST, reg_ploamik_bcst);

    *value = RU_FIELD_GET(0, NGPON_RX_ENC, PLOAMIK_BCST, VALUE, reg_ploamik_bcst);

    return 0;
}

int ag_drv_ngpon_rx_enc_cmack2_onu1_0_set(uint32_t value)
{
    uint32_t reg_cmack2_onu1_0=0;

#ifdef VALIDATE_PARMS
#endif

    reg_cmack2_onu1_0 = RU_FIELD_SET(0, NGPON_RX_ENC, CMACK2_ONU1_0, VALUE, reg_cmack2_onu1_0, value);

    RU_REG_WRITE(0, NGPON_RX_ENC, CMACK2_ONU1_0, reg_cmack2_onu1_0);

    return 0;
}

int ag_drv_ngpon_rx_enc_cmack2_onu1_0_get(uint32_t *value)
{
    uint32_t reg_cmack2_onu1_0=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_ENC, CMACK2_ONU1_0, reg_cmack2_onu1_0);

    *value = RU_FIELD_GET(0, NGPON_RX_ENC, CMACK2_ONU1_0, VALUE, reg_cmack2_onu1_0);

    return 0;
}

int ag_drv_ngpon_rx_enc_cmack2_onu1_1_set(uint32_t value)
{
    uint32_t reg_cmack2_onu1_1=0;

#ifdef VALIDATE_PARMS
#endif

    reg_cmack2_onu1_1 = RU_FIELD_SET(0, NGPON_RX_ENC, CMACK2_ONU1_1, VALUE, reg_cmack2_onu1_1, value);

    RU_REG_WRITE(0, NGPON_RX_ENC, CMACK2_ONU1_1, reg_cmack2_onu1_1);

    return 0;
}

int ag_drv_ngpon_rx_enc_cmack2_onu1_1_get(uint32_t *value)
{
    uint32_t reg_cmack2_onu1_1=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_ENC, CMACK2_ONU1_1, reg_cmack2_onu1_1);

    *value = RU_FIELD_GET(0, NGPON_RX_ENC, CMACK2_ONU1_1, VALUE, reg_cmack2_onu1_1);

    return 0;
}

int ag_drv_ngpon_rx_enc_cmack2_onu1_2_set(uint32_t value)
{
    uint32_t reg_cmack2_onu1_2=0;

#ifdef VALIDATE_PARMS
#endif

    reg_cmack2_onu1_2 = RU_FIELD_SET(0, NGPON_RX_ENC, CMACK2_ONU1_2, VALUE, reg_cmack2_onu1_2, value);

    RU_REG_WRITE(0, NGPON_RX_ENC, CMACK2_ONU1_2, reg_cmack2_onu1_2);

    return 0;
}

int ag_drv_ngpon_rx_enc_cmack2_onu1_2_get(uint32_t *value)
{
    uint32_t reg_cmack2_onu1_2=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_ENC, CMACK2_ONU1_2, reg_cmack2_onu1_2);

    *value = RU_FIELD_GET(0, NGPON_RX_ENC, CMACK2_ONU1_2, VALUE, reg_cmack2_onu1_2);

    return 0;
}

int ag_drv_ngpon_rx_enc_cmack2_onu1_3_set(uint32_t value)
{
    uint32_t reg_cmack2_onu1_3=0;

#ifdef VALIDATE_PARMS
#endif

    reg_cmack2_onu1_3 = RU_FIELD_SET(0, NGPON_RX_ENC, CMACK2_ONU1_3, VALUE, reg_cmack2_onu1_3, value);

    RU_REG_WRITE(0, NGPON_RX_ENC, CMACK2_ONU1_3, reg_cmack2_onu1_3);

    return 0;
}

int ag_drv_ngpon_rx_enc_cmack2_onu1_3_get(uint32_t *value)
{
    uint32_t reg_cmack2_onu1_3=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_ENC, CMACK2_ONU1_3, reg_cmack2_onu1_3);

    *value = RU_FIELD_GET(0, NGPON_RX_ENC, CMACK2_ONU1_3, VALUE, reg_cmack2_onu1_3);

    return 0;
}

int ag_drv_ngpon_rx_enc_cmack2_onu2_0_set(uint32_t value)
{
    uint32_t reg_cmack2_onu2_0=0;

#ifdef VALIDATE_PARMS
#endif

    reg_cmack2_onu2_0 = RU_FIELD_SET(0, NGPON_RX_ENC, CMACK2_ONU2_0, VALUE, reg_cmack2_onu2_0, value);

    RU_REG_WRITE(0, NGPON_RX_ENC, CMACK2_ONU2_0, reg_cmack2_onu2_0);

    return 0;
}

int ag_drv_ngpon_rx_enc_cmack2_onu2_0_get(uint32_t *value)
{
    uint32_t reg_cmack2_onu2_0=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_ENC, CMACK2_ONU2_0, reg_cmack2_onu2_0);

    *value = RU_FIELD_GET(0, NGPON_RX_ENC, CMACK2_ONU2_0, VALUE, reg_cmack2_onu2_0);

    return 0;
}

int ag_drv_ngpon_rx_enc_cmack2_onu2_1_set(uint32_t value)
{
    uint32_t reg_cmack2_onu2_1=0;

#ifdef VALIDATE_PARMS
#endif

    reg_cmack2_onu2_1 = RU_FIELD_SET(0, NGPON_RX_ENC, CMACK2_ONU2_1, VALUE, reg_cmack2_onu2_1, value);

    RU_REG_WRITE(0, NGPON_RX_ENC, CMACK2_ONU2_1, reg_cmack2_onu2_1);

    return 0;
}

int ag_drv_ngpon_rx_enc_cmack2_onu2_1_get(uint32_t *value)
{
    uint32_t reg_cmack2_onu2_1=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_ENC, CMACK2_ONU2_1, reg_cmack2_onu2_1);

    *value = RU_FIELD_GET(0, NGPON_RX_ENC, CMACK2_ONU2_1, VALUE, reg_cmack2_onu2_1);

    return 0;
}

int ag_drv_ngpon_rx_enc_cmack2_onu2_2_set(uint32_t value)
{
    uint32_t reg_cmack2_onu2_2=0;

#ifdef VALIDATE_PARMS
#endif

    reg_cmack2_onu2_2 = RU_FIELD_SET(0, NGPON_RX_ENC, CMACK2_ONU2_2, VALUE, reg_cmack2_onu2_2, value);

    RU_REG_WRITE(0, NGPON_RX_ENC, CMACK2_ONU2_2, reg_cmack2_onu2_2);

    return 0;
}

int ag_drv_ngpon_rx_enc_cmack2_onu2_2_get(uint32_t *value)
{
    uint32_t reg_cmack2_onu2_2=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_ENC, CMACK2_ONU2_2, reg_cmack2_onu2_2);

    *value = RU_FIELD_GET(0, NGPON_RX_ENC, CMACK2_ONU2_2, VALUE, reg_cmack2_onu2_2);

    return 0;
}

int ag_drv_ngpon_rx_enc_cmack2_onu2_3_set(uint32_t value)
{
    uint32_t reg_cmack2_onu2_3=0;

#ifdef VALIDATE_PARMS
#endif

    reg_cmack2_onu2_3 = RU_FIELD_SET(0, NGPON_RX_ENC, CMACK2_ONU2_3, VALUE, reg_cmack2_onu2_3, value);

    RU_REG_WRITE(0, NGPON_RX_ENC, CMACK2_ONU2_3, reg_cmack2_onu2_3);

    return 0;
}

int ag_drv_ngpon_rx_enc_cmack2_onu2_3_get(uint32_t *value)
{
    uint32_t reg_cmack2_onu2_3=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_ENC, CMACK2_ONU2_3, reg_cmack2_onu2_3);

    *value = RU_FIELD_GET(0, NGPON_RX_ENC, CMACK2_ONU2_3, VALUE, reg_cmack2_onu2_3);

    return 0;
}

int ag_drv_ngpon_rx_enc_cmack2_bcst_0_set(uint32_t value)
{
    uint32_t reg_cmack2_bcst_0=0;

#ifdef VALIDATE_PARMS
#endif

    reg_cmack2_bcst_0 = RU_FIELD_SET(0, NGPON_RX_ENC, CMACK2_BCST_0, VALUE, reg_cmack2_bcst_0, value);

    RU_REG_WRITE(0, NGPON_RX_ENC, CMACK2_BCST_0, reg_cmack2_bcst_0);

    return 0;
}

int ag_drv_ngpon_rx_enc_cmack2_bcst_0_get(uint32_t *value)
{
    uint32_t reg_cmack2_bcst_0=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_ENC, CMACK2_BCST_0, reg_cmack2_bcst_0);

    *value = RU_FIELD_GET(0, NGPON_RX_ENC, CMACK2_BCST_0, VALUE, reg_cmack2_bcst_0);

    return 0;
}

int ag_drv_ngpon_rx_enc_cmack2_bcst_1_set(uint32_t value)
{
    uint32_t reg_cmack2_bcst_1=0;

#ifdef VALIDATE_PARMS
#endif

    reg_cmack2_bcst_1 = RU_FIELD_SET(0, NGPON_RX_ENC, CMACK2_BCST_1, VALUE, reg_cmack2_bcst_1, value);

    RU_REG_WRITE(0, NGPON_RX_ENC, CMACK2_BCST_1, reg_cmack2_bcst_1);

    return 0;
}

int ag_drv_ngpon_rx_enc_cmack2_bcst_1_get(uint32_t *value)
{
    uint32_t reg_cmack2_bcst_1=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_ENC, CMACK2_BCST_1, reg_cmack2_bcst_1);

    *value = RU_FIELD_GET(0, NGPON_RX_ENC, CMACK2_BCST_1, VALUE, reg_cmack2_bcst_1);

    return 0;
}

int ag_drv_ngpon_rx_enc_cmack2_bcst_2_set(uint32_t value)
{
    uint32_t reg_cmack2_bcst_2=0;

#ifdef VALIDATE_PARMS
#endif

    reg_cmack2_bcst_2 = RU_FIELD_SET(0, NGPON_RX_ENC, CMACK2_BCST_2, VALUE, reg_cmack2_bcst_2, value);

    RU_REG_WRITE(0, NGPON_RX_ENC, CMACK2_BCST_2, reg_cmack2_bcst_2);

    return 0;
}

int ag_drv_ngpon_rx_enc_cmack2_bcst_2_get(uint32_t *value)
{
    uint32_t reg_cmack2_bcst_2=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_ENC, CMACK2_BCST_2, reg_cmack2_bcst_2);

    *value = RU_FIELD_GET(0, NGPON_RX_ENC, CMACK2_BCST_2, VALUE, reg_cmack2_bcst_2);

    return 0;
}

int ag_drv_ngpon_rx_enc_cmack2_bcst_3_set(uint32_t value)
{
    uint32_t reg_cmack2_bcst_3=0;

#ifdef VALIDATE_PARMS
#endif

    reg_cmack2_bcst_3 = RU_FIELD_SET(0, NGPON_RX_ENC, CMACK2_BCST_3, VALUE, reg_cmack2_bcst_3, value);

    RU_REG_WRITE(0, NGPON_RX_ENC, CMACK2_BCST_3, reg_cmack2_bcst_3);

    return 0;
}

int ag_drv_ngpon_rx_enc_cmack2_bcst_3_get(uint32_t *value)
{
    uint32_t reg_cmack2_bcst_3=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_ENC, CMACK2_BCST_3, reg_cmack2_bcst_3);

    *value = RU_FIELD_GET(0, NGPON_RX_ENC, CMACK2_BCST_3, VALUE, reg_cmack2_bcst_3);

    return 0;
}

int ag_drv_ngpon_rx_enc_key1_mcst_set(uint32_t mcast_key1_word_idx, uint32_t value)
{
    uint32_t reg_key1_mcst=0;

#ifdef VALIDATE_PARMS
    if((mcast_key1_word_idx >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_key1_mcst = RU_FIELD_SET(0, NGPON_RX_ENC, KEY1_MCST, VALUE, reg_key1_mcst, value);

    RU_REG_RAM_WRITE(0, mcast_key1_word_idx, NGPON_RX_ENC, KEY1_MCST, reg_key1_mcst);

    return 0;
}

int ag_drv_ngpon_rx_enc_key1_mcst_get(uint32_t mcast_key1_word_idx, uint32_t *value)
{
    uint32_t reg_key1_mcst=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((mcast_key1_word_idx >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, mcast_key1_word_idx, NGPON_RX_ENC, KEY1_MCST, reg_key1_mcst);

    *value = RU_FIELD_GET(0, NGPON_RX_ENC, KEY1_MCST, VALUE, reg_key1_mcst);

    return 0;
}

int ag_drv_ngpon_rx_enc_key2_mcst_set(uint32_t mcast_key2_word_idx, uint32_t value)
{
    uint32_t reg_key2_mcst=0;

#ifdef VALIDATE_PARMS
    if((mcast_key2_word_idx >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_key2_mcst = RU_FIELD_SET(0, NGPON_RX_ENC, KEY2_MCST, VALUE, reg_key2_mcst, value);

    RU_REG_RAM_WRITE(0, mcast_key2_word_idx, NGPON_RX_ENC, KEY2_MCST, reg_key2_mcst);

    return 0;
}

int ag_drv_ngpon_rx_enc_key2_mcst_get(uint32_t mcast_key2_word_idx, uint32_t *value)
{
    uint32_t reg_key2_mcst=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((mcast_key2_word_idx >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, mcast_key2_word_idx, NGPON_RX_ENC, KEY2_MCST, reg_key2_mcst);

    *value = RU_FIELD_GET(0, NGPON_RX_ENC, KEY2_MCST, VALUE, reg_key2_mcst);

    return 0;
}

int ag_drv_ngpon_rx_enc_ploamik_lookup_get(uint32_t ploam_ik_lookup_word_idx, uint32_t *value)
{
    uint32_t reg_ploamik_lookup=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ploam_ik_lookup_word_idx >= 12))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ploam_ik_lookup_word_idx, NGPON_RX_ENC, PLOAMIK_LOOKUP, reg_ploamik_lookup);

    *value = RU_FIELD_GET(0, NGPON_RX_ENC, PLOAMIK_LOOKUP, VALUE, reg_ploamik_lookup);

    return 0;
}

int ag_drv_ngpon_rx_enc_cmack2_lookup_get(uint32_t cmac2_lookup_word_idx, uint32_t *value)
{
    uint32_t reg_cmack2_lookup=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((cmac2_lookup_word_idx >= 12))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, cmac2_lookup_word_idx, NGPON_RX_ENC, CMACK2_LOOKUP, reg_cmack2_lookup);

    *value = RU_FIELD_GET(0, NGPON_RX_ENC, CMACK2_LOOKUP, VALUE, reg_cmack2_lookup);

    return 0;
}


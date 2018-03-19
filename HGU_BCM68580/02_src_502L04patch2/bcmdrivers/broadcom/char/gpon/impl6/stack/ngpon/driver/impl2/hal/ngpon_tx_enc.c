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
#include "ngpon_tx_enc.h"
int ag_drv_ngpon_tx_enc_key_cfg_onu1_set(uint8_t key_1_valid, uint8_t key_2_valid, uint8_t key_sel)
{
    uint32_t reg_key_cfg_onu1=0;

#ifdef VALIDATE_PARMS
    if((key_1_valid >= _1BITS_MAX_VAL_) ||
       (key_2_valid >= _1BITS_MAX_VAL_) ||
       (key_sel >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_key_cfg_onu1 = RU_FIELD_SET(0, NGPON_TX_ENC, KEY_CFG_ONU1, KEY_1_VALID, reg_key_cfg_onu1, key_1_valid);
    reg_key_cfg_onu1 = RU_FIELD_SET(0, NGPON_TX_ENC, KEY_CFG_ONU1, KEY_2_VALID, reg_key_cfg_onu1, key_2_valid);
    reg_key_cfg_onu1 = RU_FIELD_SET(0, NGPON_TX_ENC, KEY_CFG_ONU1, KEY_SEL, reg_key_cfg_onu1, key_sel);

    RU_REG_WRITE(0, NGPON_TX_ENC, KEY_CFG_ONU1, reg_key_cfg_onu1);

    return 0;
}

int ag_drv_ngpon_tx_enc_key_cfg_onu1_get(uint8_t *key_1_valid, uint8_t *key_2_valid, uint8_t *key_sel)
{
    uint32_t reg_key_cfg_onu1=0;

#ifdef VALIDATE_PARMS
    if(!key_1_valid || !key_2_valid || !key_sel)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_ENC, KEY_CFG_ONU1, reg_key_cfg_onu1);

    *key_1_valid = RU_FIELD_GET(0, NGPON_TX_ENC, KEY_CFG_ONU1, KEY_1_VALID, reg_key_cfg_onu1);
    *key_2_valid = RU_FIELD_GET(0, NGPON_TX_ENC, KEY_CFG_ONU1, KEY_2_VALID, reg_key_cfg_onu1);
    *key_sel = RU_FIELD_GET(0, NGPON_TX_ENC, KEY_CFG_ONU1, KEY_SEL, reg_key_cfg_onu1);

    return 0;
}

int ag_drv_ngpon_tx_enc_key_cfg_onu2_set(uint8_t key_1_valid, uint8_t key_2_valid, uint8_t key_sel)
{
    uint32_t reg_key_cfg_onu2=0;

#ifdef VALIDATE_PARMS
    if((key_1_valid >= _1BITS_MAX_VAL_) ||
       (key_2_valid >= _1BITS_MAX_VAL_) ||
       (key_sel >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_key_cfg_onu2 = RU_FIELD_SET(0, NGPON_TX_ENC, KEY_CFG_ONU2, KEY_1_VALID, reg_key_cfg_onu2, key_1_valid);
    reg_key_cfg_onu2 = RU_FIELD_SET(0, NGPON_TX_ENC, KEY_CFG_ONU2, KEY_2_VALID, reg_key_cfg_onu2, key_2_valid);
    reg_key_cfg_onu2 = RU_FIELD_SET(0, NGPON_TX_ENC, KEY_CFG_ONU2, KEY_SEL, reg_key_cfg_onu2, key_sel);

    RU_REG_WRITE(0, NGPON_TX_ENC, KEY_CFG_ONU2, reg_key_cfg_onu2);

    return 0;
}

int ag_drv_ngpon_tx_enc_key_cfg_onu2_get(uint8_t *key_1_valid, uint8_t *key_2_valid, uint8_t *key_sel)
{
    uint32_t reg_key_cfg_onu2=0;

#ifdef VALIDATE_PARMS
    if(!key_1_valid || !key_2_valid || !key_sel)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_ENC, KEY_CFG_ONU2, reg_key_cfg_onu2);

    *key_1_valid = RU_FIELD_GET(0, NGPON_TX_ENC, KEY_CFG_ONU2, KEY_1_VALID, reg_key_cfg_onu2);
    *key_2_valid = RU_FIELD_GET(0, NGPON_TX_ENC, KEY_CFG_ONU2, KEY_2_VALID, reg_key_cfg_onu2);
    *key_sel = RU_FIELD_GET(0, NGPON_TX_ENC, KEY_CFG_ONU2, KEY_SEL, reg_key_cfg_onu2);

    return 0;
}

int ag_drv_ngpon_tx_enc_pyld_enc_key1_onu1_set(uint32_t ngpon_tx_payload_encryption_key1_onu1_word, uint32_t value)
{
    uint32_t reg_pyld_enc_key1_onu1=0;

#ifdef VALIDATE_PARMS
    if((ngpon_tx_payload_encryption_key1_onu1_word >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_pyld_enc_key1_onu1 = RU_FIELD_SET(0, NGPON_TX_ENC, PYLD_ENC_KEY1_ONU1, VALUE, reg_pyld_enc_key1_onu1, value);

    RU_REG_RAM_WRITE(0, ngpon_tx_payload_encryption_key1_onu1_word, NGPON_TX_ENC, PYLD_ENC_KEY1_ONU1, reg_pyld_enc_key1_onu1);

    return 0;
}

int ag_drv_ngpon_tx_enc_pyld_enc_key1_onu1_get(uint32_t ngpon_tx_payload_encryption_key1_onu1_word, uint32_t *value)
{
    uint32_t reg_pyld_enc_key1_onu1=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ngpon_tx_payload_encryption_key1_onu1_word >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ngpon_tx_payload_encryption_key1_onu1_word, NGPON_TX_ENC, PYLD_ENC_KEY1_ONU1, reg_pyld_enc_key1_onu1);

    *value = RU_FIELD_GET(0, NGPON_TX_ENC, PYLD_ENC_KEY1_ONU1, VALUE, reg_pyld_enc_key1_onu1);

    return 0;
}

int ag_drv_ngpon_tx_enc_pyld_enc_key2_onu1_set(uint32_t ngpon_tx_payload_encryption_key2_onu1_word, uint32_t value)
{
    uint32_t reg_pyld_enc_key2_onu1=0;

#ifdef VALIDATE_PARMS
    if((ngpon_tx_payload_encryption_key2_onu1_word >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_pyld_enc_key2_onu1 = RU_FIELD_SET(0, NGPON_TX_ENC, PYLD_ENC_KEY2_ONU1, VALUE, reg_pyld_enc_key2_onu1, value);

    RU_REG_RAM_WRITE(0, ngpon_tx_payload_encryption_key2_onu1_word, NGPON_TX_ENC, PYLD_ENC_KEY2_ONU1, reg_pyld_enc_key2_onu1);

    return 0;
}

int ag_drv_ngpon_tx_enc_pyld_enc_key2_onu1_get(uint32_t ngpon_tx_payload_encryption_key2_onu1_word, uint32_t *value)
{
    uint32_t reg_pyld_enc_key2_onu1=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ngpon_tx_payload_encryption_key2_onu1_word >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ngpon_tx_payload_encryption_key2_onu1_word, NGPON_TX_ENC, PYLD_ENC_KEY2_ONU1, reg_pyld_enc_key2_onu1);

    *value = RU_FIELD_GET(0, NGPON_TX_ENC, PYLD_ENC_KEY2_ONU1, VALUE, reg_pyld_enc_key2_onu1);

    return 0;
}

int ag_drv_ngpon_tx_enc_pyld_enc_key1_onu2_set(uint32_t ngpon_tx_payload_encryption_key1_onu2_word, uint32_t value)
{
    uint32_t reg_pyld_enc_key1_onu2=0;

#ifdef VALIDATE_PARMS
    if((ngpon_tx_payload_encryption_key1_onu2_word >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_pyld_enc_key1_onu2 = RU_FIELD_SET(0, NGPON_TX_ENC, PYLD_ENC_KEY1_ONU2, VALUE, reg_pyld_enc_key1_onu2, value);

    RU_REG_RAM_WRITE(0, ngpon_tx_payload_encryption_key1_onu2_word, NGPON_TX_ENC, PYLD_ENC_KEY1_ONU2, reg_pyld_enc_key1_onu2);

    return 0;
}

int ag_drv_ngpon_tx_enc_pyld_enc_key1_onu2_get(uint32_t ngpon_tx_payload_encryption_key1_onu2_word, uint32_t *value)
{
    uint32_t reg_pyld_enc_key1_onu2=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ngpon_tx_payload_encryption_key1_onu2_word >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ngpon_tx_payload_encryption_key1_onu2_word, NGPON_TX_ENC, PYLD_ENC_KEY1_ONU2, reg_pyld_enc_key1_onu2);

    *value = RU_FIELD_GET(0, NGPON_TX_ENC, PYLD_ENC_KEY1_ONU2, VALUE, reg_pyld_enc_key1_onu2);

    return 0;
}

int ag_drv_ngpon_tx_enc_pyld_enc_key2_onu2_set(uint32_t ngpon_tx_payload_encryption_key2_onu2_word, uint32_t value)
{
    uint32_t reg_pyld_enc_key2_onu2=0;

#ifdef VALIDATE_PARMS
    if((ngpon_tx_payload_encryption_key2_onu2_word >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_pyld_enc_key2_onu2 = RU_FIELD_SET(0, NGPON_TX_ENC, PYLD_ENC_KEY2_ONU2, VALUE, reg_pyld_enc_key2_onu2, value);

    RU_REG_RAM_WRITE(0, ngpon_tx_payload_encryption_key2_onu2_word, NGPON_TX_ENC, PYLD_ENC_KEY2_ONU2, reg_pyld_enc_key2_onu2);

    return 0;
}

int ag_drv_ngpon_tx_enc_pyld_enc_key2_onu2_get(uint32_t ngpon_tx_payload_encryption_key2_onu2_word, uint32_t *value)
{
    uint32_t reg_pyld_enc_key2_onu2=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ngpon_tx_payload_encryption_key2_onu2_word >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ngpon_tx_payload_encryption_key2_onu2_word, NGPON_TX_ENC, PYLD_ENC_KEY2_ONU2, reg_pyld_enc_key2_onu2);

    *value = RU_FIELD_GET(0, NGPON_TX_ENC, PYLD_ENC_KEY2_ONU2, VALUE, reg_pyld_enc_key2_onu2);

    return 0;
}

int ag_drv_ngpon_tx_enc_ploam_load_set(uint8_t ploamik_onu1, uint8_t ploamik_onu2, uint8_t ploamik_bcst)
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

    reg_ploam_load = RU_FIELD_SET(0, NGPON_TX_ENC, PLOAM_LOAD, PLOAMIK_ONU1, reg_ploam_load, ploamik_onu1);
    reg_ploam_load = RU_FIELD_SET(0, NGPON_TX_ENC, PLOAM_LOAD, PLOAMIK_ONU2, reg_ploam_load, ploamik_onu2);
    reg_ploam_load = RU_FIELD_SET(0, NGPON_TX_ENC, PLOAM_LOAD, PLOAMIK_BCST, reg_ploam_load, ploamik_bcst);

    RU_REG_WRITE(0, NGPON_TX_ENC, PLOAM_LOAD, reg_ploam_load);

    return 0;
}

int ag_drv_ngpon_tx_enc_ploam_load_get(uint8_t *ploamik_onu1, uint8_t *ploamik_onu2, uint8_t *ploamik_bcst)
{
    uint32_t reg_ploam_load=0;

#ifdef VALIDATE_PARMS
    if(!ploamik_onu1 || !ploamik_onu2 || !ploamik_bcst)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_ENC, PLOAM_LOAD, reg_ploam_load);

    *ploamik_onu1 = RU_FIELD_GET(0, NGPON_TX_ENC, PLOAM_LOAD, PLOAMIK_ONU1, reg_ploam_load);
    *ploamik_onu2 = RU_FIELD_GET(0, NGPON_TX_ENC, PLOAM_LOAD, PLOAMIK_ONU2, reg_ploam_load);
    *ploamik_bcst = RU_FIELD_GET(0, NGPON_TX_ENC, PLOAM_LOAD, PLOAMIK_BCST, reg_ploam_load);

    return 0;
}

int ag_drv_ngpon_tx_enc_ploamik_onu1_set(uint32_t ngpon_tx_ploam_ik_onu1_word, uint32_t value)
{
    uint32_t reg_ploamik_onu1=0;

#ifdef VALIDATE_PARMS
    if((ngpon_tx_ploam_ik_onu1_word >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_ploamik_onu1 = RU_FIELD_SET(0, NGPON_TX_ENC, PLOAMIK_ONU1, VALUE, reg_ploamik_onu1, value);

    RU_REG_RAM_WRITE(0, ngpon_tx_ploam_ik_onu1_word, NGPON_TX_ENC, PLOAMIK_ONU1, reg_ploamik_onu1);

    return 0;
}

int ag_drv_ngpon_tx_enc_ploamik_onu1_get(uint32_t ngpon_tx_ploam_ik_onu1_word, uint32_t *value)
{
    uint32_t reg_ploamik_onu1=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ngpon_tx_ploam_ik_onu1_word >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ngpon_tx_ploam_ik_onu1_word, NGPON_TX_ENC, PLOAMIK_ONU1, reg_ploamik_onu1);

    *value = RU_FIELD_GET(0, NGPON_TX_ENC, PLOAMIK_ONU1, VALUE, reg_ploamik_onu1);

    return 0;
}

int ag_drv_ngpon_tx_enc_ploamik_onu2_set(uint32_t ngpon_tx_ploam_ik_onu2_word, uint32_t value)
{
    uint32_t reg_ploamik_onu2=0;

#ifdef VALIDATE_PARMS
    if((ngpon_tx_ploam_ik_onu2_word >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_ploamik_onu2 = RU_FIELD_SET(0, NGPON_TX_ENC, PLOAMIK_ONU2, VALUE, reg_ploamik_onu2, value);

    RU_REG_RAM_WRITE(0, ngpon_tx_ploam_ik_onu2_word, NGPON_TX_ENC, PLOAMIK_ONU2, reg_ploamik_onu2);

    return 0;
}

int ag_drv_ngpon_tx_enc_ploamik_onu2_get(uint32_t ngpon_tx_ploam_ik_onu2_word, uint32_t *value)
{
    uint32_t reg_ploamik_onu2=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ngpon_tx_ploam_ik_onu2_word >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ngpon_tx_ploam_ik_onu2_word, NGPON_TX_ENC, PLOAMIK_ONU2, reg_ploamik_onu2);

    *value = RU_FIELD_GET(0, NGPON_TX_ENC, PLOAMIK_ONU2, VALUE, reg_ploamik_onu2);

    return 0;
}

int ag_drv_ngpon_tx_enc_ploamik_dflt_set(uint32_t ngpon_tx_ploam_ik_dflt_word, uint32_t value)
{
    uint32_t reg_ploamik_dflt=0;

#ifdef VALIDATE_PARMS
    if((ngpon_tx_ploam_ik_dflt_word >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_ploamik_dflt = RU_FIELD_SET(0, NGPON_TX_ENC, PLOAMIK_DFLT, VALUE, reg_ploamik_dflt, value);

    RU_REG_RAM_WRITE(0, ngpon_tx_ploam_ik_dflt_word, NGPON_TX_ENC, PLOAMIK_DFLT, reg_ploamik_dflt);

    return 0;
}

int ag_drv_ngpon_tx_enc_ploamik_dflt_get(uint32_t ngpon_tx_ploam_ik_dflt_word, uint32_t *value)
{
    uint32_t reg_ploamik_dflt=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ngpon_tx_ploam_ik_dflt_word >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ngpon_tx_ploam_ik_dflt_word, NGPON_TX_ENC, PLOAMIK_DFLT, reg_ploamik_dflt);

    *value = RU_FIELD_GET(0, NGPON_TX_ENC, PLOAMIK_DFLT, VALUE, reg_ploamik_dflt);

    return 0;
}

int ag_drv_ngpon_tx_enc_cmac_k2_onu1_0_set(uint32_t value)
{
    uint32_t reg_cmac_k2_onu1_0=0;

#ifdef VALIDATE_PARMS
#endif

    reg_cmac_k2_onu1_0 = RU_FIELD_SET(0, NGPON_TX_ENC, CMAC_K2_ONU1_0, VALUE, reg_cmac_k2_onu1_0, value);

    RU_REG_WRITE(0, NGPON_TX_ENC, CMAC_K2_ONU1_0, reg_cmac_k2_onu1_0);

    return 0;
}

int ag_drv_ngpon_tx_enc_cmac_k2_onu1_0_get(uint32_t *value)
{
    uint32_t reg_cmac_k2_onu1_0=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_ENC, CMAC_K2_ONU1_0, reg_cmac_k2_onu1_0);

    *value = RU_FIELD_GET(0, NGPON_TX_ENC, CMAC_K2_ONU1_0, VALUE, reg_cmac_k2_onu1_0);

    return 0;
}

int ag_drv_ngpon_tx_enc_cmac_k2_onu1_1_set(uint32_t value)
{
    uint32_t reg_cmac_k2_onu1_1=0;

#ifdef VALIDATE_PARMS
#endif

    reg_cmac_k2_onu1_1 = RU_FIELD_SET(0, NGPON_TX_ENC, CMAC_K2_ONU1_1, VALUE, reg_cmac_k2_onu1_1, value);

    RU_REG_WRITE(0, NGPON_TX_ENC, CMAC_K2_ONU1_1, reg_cmac_k2_onu1_1);

    return 0;
}

int ag_drv_ngpon_tx_enc_cmac_k2_onu1_1_get(uint32_t *value)
{
    uint32_t reg_cmac_k2_onu1_1=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_ENC, CMAC_K2_ONU1_1, reg_cmac_k2_onu1_1);

    *value = RU_FIELD_GET(0, NGPON_TX_ENC, CMAC_K2_ONU1_1, VALUE, reg_cmac_k2_onu1_1);

    return 0;
}

int ag_drv_ngpon_tx_enc_cmac_k2_onu1_2_set(uint32_t value)
{
    uint32_t reg_cmac_k2_onu1_2=0;

#ifdef VALIDATE_PARMS
#endif

    reg_cmac_k2_onu1_2 = RU_FIELD_SET(0, NGPON_TX_ENC, CMAC_K2_ONU1_2, VALUE, reg_cmac_k2_onu1_2, value);

    RU_REG_WRITE(0, NGPON_TX_ENC, CMAC_K2_ONU1_2, reg_cmac_k2_onu1_2);

    return 0;
}

int ag_drv_ngpon_tx_enc_cmac_k2_onu1_2_get(uint32_t *value)
{
    uint32_t reg_cmac_k2_onu1_2=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_ENC, CMAC_K2_ONU1_2, reg_cmac_k2_onu1_2);

    *value = RU_FIELD_GET(0, NGPON_TX_ENC, CMAC_K2_ONU1_2, VALUE, reg_cmac_k2_onu1_2);

    return 0;
}

int ag_drv_ngpon_tx_enc_cmac_k2_onu1_3_set(uint32_t value)
{
    uint32_t reg_cmac_k2_onu1_3=0;

#ifdef VALIDATE_PARMS
#endif

    reg_cmac_k2_onu1_3 = RU_FIELD_SET(0, NGPON_TX_ENC, CMAC_K2_ONU1_3, VALUE, reg_cmac_k2_onu1_3, value);

    RU_REG_WRITE(0, NGPON_TX_ENC, CMAC_K2_ONU1_3, reg_cmac_k2_onu1_3);

    return 0;
}

int ag_drv_ngpon_tx_enc_cmac_k2_onu1_3_get(uint32_t *value)
{
    uint32_t reg_cmac_k2_onu1_3=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_ENC, CMAC_K2_ONU1_3, reg_cmac_k2_onu1_3);

    *value = RU_FIELD_GET(0, NGPON_TX_ENC, CMAC_K2_ONU1_3, VALUE, reg_cmac_k2_onu1_3);

    return 0;
}

int ag_drv_ngpon_tx_enc_cmac_k2_onu2_0_set(uint32_t value)
{
    uint32_t reg_cmac_k2_onu2_0=0;

#ifdef VALIDATE_PARMS
#endif

    reg_cmac_k2_onu2_0 = RU_FIELD_SET(0, NGPON_TX_ENC, CMAC_K2_ONU2_0, VALUE, reg_cmac_k2_onu2_0, value);

    RU_REG_WRITE(0, NGPON_TX_ENC, CMAC_K2_ONU2_0, reg_cmac_k2_onu2_0);

    return 0;
}

int ag_drv_ngpon_tx_enc_cmac_k2_onu2_0_get(uint32_t *value)
{
    uint32_t reg_cmac_k2_onu2_0=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_ENC, CMAC_K2_ONU2_0, reg_cmac_k2_onu2_0);

    *value = RU_FIELD_GET(0, NGPON_TX_ENC, CMAC_K2_ONU2_0, VALUE, reg_cmac_k2_onu2_0);

    return 0;
}

int ag_drv_ngpon_tx_enc_cmac_k2_onu2_1_set(uint32_t value)
{
    uint32_t reg_cmac_k2_onu2_1=0;

#ifdef VALIDATE_PARMS
#endif

    reg_cmac_k2_onu2_1 = RU_FIELD_SET(0, NGPON_TX_ENC, CMAC_K2_ONU2_1, VALUE, reg_cmac_k2_onu2_1, value);

    RU_REG_WRITE(0, NGPON_TX_ENC, CMAC_K2_ONU2_1, reg_cmac_k2_onu2_1);

    return 0;
}

int ag_drv_ngpon_tx_enc_cmac_k2_onu2_1_get(uint32_t *value)
{
    uint32_t reg_cmac_k2_onu2_1=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_ENC, CMAC_K2_ONU2_1, reg_cmac_k2_onu2_1);

    *value = RU_FIELD_GET(0, NGPON_TX_ENC, CMAC_K2_ONU2_1, VALUE, reg_cmac_k2_onu2_1);

    return 0;
}

int ag_drv_ngpon_tx_enc_cmac_k2_onu2_2_set(uint32_t value)
{
    uint32_t reg_cmac_k2_onu2_2=0;

#ifdef VALIDATE_PARMS
#endif

    reg_cmac_k2_onu2_2 = RU_FIELD_SET(0, NGPON_TX_ENC, CMAC_K2_ONU2_2, VALUE, reg_cmac_k2_onu2_2, value);

    RU_REG_WRITE(0, NGPON_TX_ENC, CMAC_K2_ONU2_2, reg_cmac_k2_onu2_2);

    return 0;
}

int ag_drv_ngpon_tx_enc_cmac_k2_onu2_2_get(uint32_t *value)
{
    uint32_t reg_cmac_k2_onu2_2=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_ENC, CMAC_K2_ONU2_2, reg_cmac_k2_onu2_2);

    *value = RU_FIELD_GET(0, NGPON_TX_ENC, CMAC_K2_ONU2_2, VALUE, reg_cmac_k2_onu2_2);

    return 0;
}

int ag_drv_ngpon_tx_enc_cmac_k2_onu2_3_set(uint32_t value)
{
    uint32_t reg_cmac_k2_onu2_3=0;

#ifdef VALIDATE_PARMS
#endif

    reg_cmac_k2_onu2_3 = RU_FIELD_SET(0, NGPON_TX_ENC, CMAC_K2_ONU2_3, VALUE, reg_cmac_k2_onu2_3, value);

    RU_REG_WRITE(0, NGPON_TX_ENC, CMAC_K2_ONU2_3, reg_cmac_k2_onu2_3);

    return 0;
}

int ag_drv_ngpon_tx_enc_cmac_k2_onu2_3_get(uint32_t *value)
{
    uint32_t reg_cmac_k2_onu2_3=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_ENC, CMAC_K2_ONU2_3, reg_cmac_k2_onu2_3);

    *value = RU_FIELD_GET(0, NGPON_TX_ENC, CMAC_K2_ONU2_3, VALUE, reg_cmac_k2_onu2_3);

    return 0;
}

int ag_drv_ngpon_tx_enc_cmac_k2_dflt_0_set(uint32_t value)
{
    uint32_t reg_cmac_k2_dflt_0=0;

#ifdef VALIDATE_PARMS
#endif

    reg_cmac_k2_dflt_0 = RU_FIELD_SET(0, NGPON_TX_ENC, CMAC_K2_DFLT_0, VALUE, reg_cmac_k2_dflt_0, value);

    RU_REG_WRITE(0, NGPON_TX_ENC, CMAC_K2_DFLT_0, reg_cmac_k2_dflt_0);

    return 0;
}

int ag_drv_ngpon_tx_enc_cmac_k2_dflt_0_get(uint32_t *value)
{
    uint32_t reg_cmac_k2_dflt_0=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_ENC, CMAC_K2_DFLT_0, reg_cmac_k2_dflt_0);

    *value = RU_FIELD_GET(0, NGPON_TX_ENC, CMAC_K2_DFLT_0, VALUE, reg_cmac_k2_dflt_0);

    return 0;
}

int ag_drv_ngpon_tx_enc_cmac_k2_dflt_1_set(uint32_t value)
{
    uint32_t reg_cmac_k2_dflt_1=0;

#ifdef VALIDATE_PARMS
#endif

    reg_cmac_k2_dflt_1 = RU_FIELD_SET(0, NGPON_TX_ENC, CMAC_K2_DFLT_1, VALUE, reg_cmac_k2_dflt_1, value);

    RU_REG_WRITE(0, NGPON_TX_ENC, CMAC_K2_DFLT_1, reg_cmac_k2_dflt_1);

    return 0;
}

int ag_drv_ngpon_tx_enc_cmac_k2_dflt_1_get(uint32_t *value)
{
    uint32_t reg_cmac_k2_dflt_1=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_ENC, CMAC_K2_DFLT_1, reg_cmac_k2_dflt_1);

    *value = RU_FIELD_GET(0, NGPON_TX_ENC, CMAC_K2_DFLT_1, VALUE, reg_cmac_k2_dflt_1);

    return 0;
}

int ag_drv_ngpon_tx_enc_cmac_k2_dflt_2_set(uint32_t value)
{
    uint32_t reg_cmac_k2_dflt_2=0;

#ifdef VALIDATE_PARMS
#endif

    reg_cmac_k2_dflt_2 = RU_FIELD_SET(0, NGPON_TX_ENC, CMAC_K2_DFLT_2, VALUE, reg_cmac_k2_dflt_2, value);

    RU_REG_WRITE(0, NGPON_TX_ENC, CMAC_K2_DFLT_2, reg_cmac_k2_dflt_2);

    return 0;
}

int ag_drv_ngpon_tx_enc_cmac_k2_dflt_2_get(uint32_t *value)
{
    uint32_t reg_cmac_k2_dflt_2=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_ENC, CMAC_K2_DFLT_2, reg_cmac_k2_dflt_2);

    *value = RU_FIELD_GET(0, NGPON_TX_ENC, CMAC_K2_DFLT_2, VALUE, reg_cmac_k2_dflt_2);

    return 0;
}

int ag_drv_ngpon_tx_enc_cmac_k2_dflt_3_set(uint32_t value)
{
    uint32_t reg_cmac_k2_dflt_3=0;

#ifdef VALIDATE_PARMS
#endif

    reg_cmac_k2_dflt_3 = RU_FIELD_SET(0, NGPON_TX_ENC, CMAC_K2_DFLT_3, VALUE, reg_cmac_k2_dflt_3, value);

    RU_REG_WRITE(0, NGPON_TX_ENC, CMAC_K2_DFLT_3, reg_cmac_k2_dflt_3);

    return 0;
}

int ag_drv_ngpon_tx_enc_cmac_k2_dflt_3_get(uint32_t *value)
{
    uint32_t reg_cmac_k2_dflt_3=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_ENC, CMAC_K2_DFLT_3, reg_cmac_k2_dflt_3);

    *value = RU_FIELD_GET(0, NGPON_TX_ENC, CMAC_K2_DFLT_3, VALUE, reg_cmac_k2_dflt_3);

    return 0;
}


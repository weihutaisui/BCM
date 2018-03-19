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
#include "ngpon_tx_profile.h"
int ag_drv_ngpon_tx_profile_profile_ctrl_set(uint32_t tx_profile_ctrl, const ngpon_tx_profile_profile_ctrl *profile_ctrl)
{
    uint32_t reg_profile_ctrl=0;

#ifdef VALIDATE_PARMS
    if(!profile_ctrl)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, 1);
        return 1;
    }
    if((tx_profile_ctrl >= 4) ||
       (profile_ctrl->delimiter_len >= _4BITS_MAX_VAL_) ||
       (profile_ctrl->preamble_len >= _4BITS_MAX_VAL_) ||
       (profile_ctrl->fec_ind >= _1BITS_MAX_VAL_) ||
       (profile_ctrl->preamble_total_len >= _11BITS_MAX_VAL_) ||
       (profile_ctrl->prof_en >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_profile_ctrl = RU_FIELD_SET(0, NGPON_TX_PROFILE, PROFILE_CTRL, DELIMITER_LEN, reg_profile_ctrl, profile_ctrl->delimiter_len);
    reg_profile_ctrl = RU_FIELD_SET(0, NGPON_TX_PROFILE, PROFILE_CTRL, PREAMBLE_LEN, reg_profile_ctrl, profile_ctrl->preamble_len);
    reg_profile_ctrl = RU_FIELD_SET(0, NGPON_TX_PROFILE, PROFILE_CTRL, FEC_IND, reg_profile_ctrl, profile_ctrl->fec_ind);
    reg_profile_ctrl = RU_FIELD_SET(0, NGPON_TX_PROFILE, PROFILE_CTRL, PREAMBLE_TOTAL_LEN, reg_profile_ctrl, profile_ctrl->preamble_total_len);
    reg_profile_ctrl = RU_FIELD_SET(0, NGPON_TX_PROFILE, PROFILE_CTRL, PROF_EN, reg_profile_ctrl, profile_ctrl->prof_en);

    RU_REG_RAM_WRITE(0, tx_profile_ctrl, NGPON_TX_PROFILE, PROFILE_CTRL, reg_profile_ctrl);

    return 0;
}

int ag_drv_ngpon_tx_profile_profile_ctrl_get(uint32_t tx_profile_ctrl, ngpon_tx_profile_profile_ctrl *profile_ctrl)
{
    uint32_t reg_profile_ctrl=0;

#ifdef VALIDATE_PARMS
    if(!profile_ctrl)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((tx_profile_ctrl >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, tx_profile_ctrl, NGPON_TX_PROFILE, PROFILE_CTRL, reg_profile_ctrl);

    profile_ctrl->delimiter_len = RU_FIELD_GET(0, NGPON_TX_PROFILE, PROFILE_CTRL, DELIMITER_LEN, reg_profile_ctrl);
    profile_ctrl->preamble_len = RU_FIELD_GET(0, NGPON_TX_PROFILE, PROFILE_CTRL, PREAMBLE_LEN, reg_profile_ctrl);
    profile_ctrl->fec_ind = RU_FIELD_GET(0, NGPON_TX_PROFILE, PROFILE_CTRL, FEC_IND, reg_profile_ctrl);
    profile_ctrl->preamble_total_len = RU_FIELD_GET(0, NGPON_TX_PROFILE, PROFILE_CTRL, PREAMBLE_TOTAL_LEN, reg_profile_ctrl);
    profile_ctrl->prof_en = RU_FIELD_GET(0, NGPON_TX_PROFILE, PROFILE_CTRL, PROF_EN, reg_profile_ctrl);

    return 0;
}

int ag_drv_ngpon_tx_profile_preamble_0_set(uint32_t tx_profile_preamble_0, uint32_t value)
{
    uint32_t reg_preamble_0=0;

#ifdef VALIDATE_PARMS
    if((tx_profile_preamble_0 >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_preamble_0 = RU_FIELD_SET(0, NGPON_TX_PROFILE, PREAMBLE_0, VALUE, reg_preamble_0, value);

    RU_REG_RAM_WRITE(0, tx_profile_preamble_0, NGPON_TX_PROFILE, PREAMBLE_0, reg_preamble_0);

    return 0;
}

int ag_drv_ngpon_tx_profile_preamble_0_get(uint32_t tx_profile_preamble_0, uint32_t *value)
{
    uint32_t reg_preamble_0=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((tx_profile_preamble_0 >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, tx_profile_preamble_0, NGPON_TX_PROFILE, PREAMBLE_0, reg_preamble_0);

    *value = RU_FIELD_GET(0, NGPON_TX_PROFILE, PREAMBLE_0, VALUE, reg_preamble_0);

    return 0;
}

int ag_drv_ngpon_tx_profile_preamble_1_set(uint32_t tx_profile_preamble_1, uint32_t value)
{
    uint32_t reg_preamble_1=0;

#ifdef VALIDATE_PARMS
    if((tx_profile_preamble_1 >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_preamble_1 = RU_FIELD_SET(0, NGPON_TX_PROFILE, PREAMBLE_1, VALUE, reg_preamble_1, value);

    RU_REG_RAM_WRITE(0, tx_profile_preamble_1, NGPON_TX_PROFILE, PREAMBLE_1, reg_preamble_1);

    return 0;
}

int ag_drv_ngpon_tx_profile_preamble_1_get(uint32_t tx_profile_preamble_1, uint32_t *value)
{
    uint32_t reg_preamble_1=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((tx_profile_preamble_1 >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, tx_profile_preamble_1, NGPON_TX_PROFILE, PREAMBLE_1, reg_preamble_1);

    *value = RU_FIELD_GET(0, NGPON_TX_PROFILE, PREAMBLE_1, VALUE, reg_preamble_1);

    return 0;
}

int ag_drv_ngpon_tx_profile_delimiter_0_set(uint32_t tx_profile_delimeter_0, uint32_t value)
{
    uint32_t reg_delimiter_0=0;

#ifdef VALIDATE_PARMS
    if((tx_profile_delimeter_0 >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_delimiter_0 = RU_FIELD_SET(0, NGPON_TX_PROFILE, DELIMITER_0, VALUE, reg_delimiter_0, value);

    RU_REG_RAM_WRITE(0, tx_profile_delimeter_0, NGPON_TX_PROFILE, DELIMITER_0, reg_delimiter_0);

    return 0;
}

int ag_drv_ngpon_tx_profile_delimiter_0_get(uint32_t tx_profile_delimeter_0, uint32_t *value)
{
    uint32_t reg_delimiter_0=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((tx_profile_delimeter_0 >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, tx_profile_delimeter_0, NGPON_TX_PROFILE, DELIMITER_0, reg_delimiter_0);

    *value = RU_FIELD_GET(0, NGPON_TX_PROFILE, DELIMITER_0, VALUE, reg_delimiter_0);

    return 0;
}

int ag_drv_ngpon_tx_profile_delimiter_1_set(uint32_t tx_profile_delimeter_1, uint32_t value)
{
    uint32_t reg_delimiter_1=0;

#ifdef VALIDATE_PARMS
    if((tx_profile_delimeter_1 >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_delimiter_1 = RU_FIELD_SET(0, NGPON_TX_PROFILE, DELIMITER_1, VALUE, reg_delimiter_1, value);

    RU_REG_RAM_WRITE(0, tx_profile_delimeter_1, NGPON_TX_PROFILE, DELIMITER_1, reg_delimiter_1);

    return 0;
}

int ag_drv_ngpon_tx_profile_delimiter_1_get(uint32_t tx_profile_delimeter_1, uint32_t *value)
{
    uint32_t reg_delimiter_1=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((tx_profile_delimeter_1 >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, tx_profile_delimeter_1, NGPON_TX_PROFILE, DELIMITER_1, reg_delimiter_1);

    *value = RU_FIELD_GET(0, NGPON_TX_PROFILE, DELIMITER_1, VALUE, reg_delimiter_1);

    return 0;
}

int ag_drv_ngpon_tx_profile_fec_type_set(uint32_t tx_profile_fec_type, uint8_t type)
{
    uint32_t reg_fec_type=0;

#ifdef VALIDATE_PARMS
    if((tx_profile_fec_type >= 4) ||
       (type >= _2BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_fec_type = RU_FIELD_SET(0, NGPON_TX_PROFILE, FEC_TYPE, TYPE, reg_fec_type, type);

    RU_REG_RAM_WRITE(0, tx_profile_fec_type, NGPON_TX_PROFILE, FEC_TYPE, reg_fec_type);

    return 0;
}

int ag_drv_ngpon_tx_profile_fec_type_get(uint32_t tx_profile_fec_type, uint8_t *type)
{
    uint32_t reg_fec_type=0;

#ifdef VALIDATE_PARMS
    if(!type)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((tx_profile_fec_type >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, tx_profile_fec_type, NGPON_TX_PROFILE, FEC_TYPE, reg_fec_type);

    *type = RU_FIELD_GET(0, NGPON_TX_PROFILE, FEC_TYPE, TYPE, reg_fec_type);

    return 0;
}


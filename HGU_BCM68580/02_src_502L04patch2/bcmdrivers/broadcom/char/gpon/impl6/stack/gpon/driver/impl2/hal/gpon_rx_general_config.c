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

#include "gpon_common.h"
#include "gpon_rx_general_config.h"
int ag_drv_gpon_rx_general_config_rcvr_status_get(uint8_t *lof, uint8_t *fec_state, uint8_t *lcdg_state, uint8_t *bit_align)
{
    uint32_t reg_rcvr_status=0;

#ifdef VALIDATE_PARMS
    if(!lof || !fec_state || !lcdg_state || !bit_align)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_RX_GENERAL_CONFIG, RCVR_STATUS, reg_rcvr_status);

    *lof = RU_FIELD_GET(0, GPON_RX_GENERAL_CONFIG, RCVR_STATUS, LOF, reg_rcvr_status);
    *fec_state = RU_FIELD_GET(0, GPON_RX_GENERAL_CONFIG, RCVR_STATUS, FEC_STATE, reg_rcvr_status);
    *lcdg_state = RU_FIELD_GET(0, GPON_RX_GENERAL_CONFIG, RCVR_STATUS, LCDG_STATE, reg_rcvr_status);
    *bit_align = RU_FIELD_GET(0, GPON_RX_GENERAL_CONFIG, RCVR_STATUS, BIT_ALIGN, reg_rcvr_status);

    return 0;
}

int ag_drv_gpon_rx_general_config_rcvr_config_set(const gpon_rx_general_config_rcvr_config *rcvr_config)
{
    uint32_t reg_rcvr_config=0;

#ifdef VALIDATE_PARMS
    if(!rcvr_config)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, 1);
        return 1;
    }
    if((rcvr_config->des_disable >= _1BITS_MAX_VAL_) ||
       (rcvr_config->fec_disable >= _1BITS_MAX_VAL_) ||
       (rcvr_config->rx_disable >= _1BITS_MAX_VAL_) ||
       (rcvr_config->loopback_enable >= _1BITS_MAX_VAL_) ||
       (rcvr_config->fec_force >= _1BITS_MAX_VAL_) ||
       (rcvr_config->fec_st_disc >= _1BITS_MAX_VAL_) ||
       (rcvr_config->squelch_dis >= _1BITS_MAX_VAL_) ||
       (rcvr_config->sop_reset >= _1BITS_MAX_VAL_) ||
       (rcvr_config->din_polarity >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_rcvr_config = RU_FIELD_SET(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, DES_DISABLE, reg_rcvr_config, rcvr_config->des_disable);
    reg_rcvr_config = RU_FIELD_SET(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, FEC_DISABLE, reg_rcvr_config, rcvr_config->fec_disable);
    reg_rcvr_config = RU_FIELD_SET(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, RX_DISABLE, reg_rcvr_config, rcvr_config->rx_disable);
    reg_rcvr_config = RU_FIELD_SET(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, LOOPBACK_ENABLE, reg_rcvr_config, rcvr_config->loopback_enable);
    reg_rcvr_config = RU_FIELD_SET(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, FEC_FORCE, reg_rcvr_config, rcvr_config->fec_force);
    reg_rcvr_config = RU_FIELD_SET(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, FEC_ST_DISC, reg_rcvr_config, rcvr_config->fec_st_disc);
    reg_rcvr_config = RU_FIELD_SET(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, SQUELCH_DIS, reg_rcvr_config, rcvr_config->squelch_dis);
    reg_rcvr_config = RU_FIELD_SET(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, SOP_RESET, reg_rcvr_config, rcvr_config->sop_reset);
    reg_rcvr_config = RU_FIELD_SET(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, DIN_POLARITY, reg_rcvr_config, rcvr_config->din_polarity);

    RU_REG_WRITE(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, reg_rcvr_config);

    return 0;
}

int ag_drv_gpon_rx_general_config_rcvr_config_get(gpon_rx_general_config_rcvr_config *rcvr_config)
{
    uint32_t reg_rcvr_config=0;

#ifdef VALIDATE_PARMS
    if(!rcvr_config)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, reg_rcvr_config);

    rcvr_config->des_disable = RU_FIELD_GET(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, DES_DISABLE, reg_rcvr_config);
    rcvr_config->fec_disable = RU_FIELD_GET(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, FEC_DISABLE, reg_rcvr_config);
    rcvr_config->rx_disable = RU_FIELD_GET(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, RX_DISABLE, reg_rcvr_config);
    rcvr_config->loopback_enable = RU_FIELD_GET(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, LOOPBACK_ENABLE, reg_rcvr_config);
    rcvr_config->fec_force = RU_FIELD_GET(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, FEC_FORCE, reg_rcvr_config);
    rcvr_config->fec_st_disc = RU_FIELD_GET(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, FEC_ST_DISC, reg_rcvr_config);
    rcvr_config->squelch_dis = RU_FIELD_GET(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, SQUELCH_DIS, reg_rcvr_config);
    rcvr_config->sop_reset = RU_FIELD_GET(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, SOP_RESET, reg_rcvr_config);
    rcvr_config->din_polarity = RU_FIELD_GET(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, DIN_POLARITY, reg_rcvr_config);

    return 0;
}

int ag_drv_gpon_rx_general_config_lof_params_set(uint8_t delta, uint8_t alpha)
{
    uint32_t reg_lof_params=0;

#ifdef VALIDATE_PARMS
    if((delta >= _4BITS_MAX_VAL_) ||
       (alpha >= _4BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_lof_params = RU_FIELD_SET(0, GPON_RX_GENERAL_CONFIG, LOF_PARAMS, DELTA, reg_lof_params, delta);
    reg_lof_params = RU_FIELD_SET(0, GPON_RX_GENERAL_CONFIG, LOF_PARAMS, ALPHA, reg_lof_params, alpha);

    RU_REG_WRITE(0, GPON_RX_GENERAL_CONFIG, LOF_PARAMS, reg_lof_params);

    return 0;
}

int ag_drv_gpon_rx_general_config_lof_params_get(uint8_t *delta, uint8_t *alpha)
{
    uint32_t reg_lof_params=0;

#ifdef VALIDATE_PARMS
    if(!delta || !alpha)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_RX_GENERAL_CONFIG, LOF_PARAMS, reg_lof_params);

    *delta = RU_FIELD_GET(0, GPON_RX_GENERAL_CONFIG, LOF_PARAMS, DELTA, reg_lof_params);
    *alpha = RU_FIELD_GET(0, GPON_RX_GENERAL_CONFIG, LOF_PARAMS, ALPHA, reg_lof_params);

    return 0;
}

int ag_drv_gpon_rx_general_config_randomsd_get(uint32_t *randomsd)
{
    uint32_t reg_randomsd=0;

#ifdef VALIDATE_PARMS
    if(!randomsd)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_RX_GENERAL_CONFIG, RANDOMSD, reg_randomsd);

    *randomsd = RU_FIELD_GET(0, GPON_RX_GENERAL_CONFIG, RANDOMSD, RANDOMSD, reg_randomsd);

    return 0;
}


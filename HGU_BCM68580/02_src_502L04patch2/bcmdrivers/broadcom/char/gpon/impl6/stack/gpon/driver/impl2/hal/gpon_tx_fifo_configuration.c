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
#include "gpon_tx_fifo_configuration.h"
int ag_drv_gpon_tx_fifo_configuration_pdp_set(uint32_t tx_q_idx, uint16_t pdbc, uint16_t pdsc)
{
    uint32_t reg_pdp=0;

#ifdef VALIDATE_PARMS
    if((tx_q_idx >= 8) ||
       (pdbc >= _9BITS_MAX_VAL_) ||
       (pdsc >= _9BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_pdp = RU_FIELD_SET(0, GPON_TX_FIFO_CONFIGURATION, PDP, PDBC, reg_pdp, pdbc);
    reg_pdp = RU_FIELD_SET(0, GPON_TX_FIFO_CONFIGURATION, PDP, PDSC, reg_pdp, pdsc);

    RU_REG_RAM_WRITE(0, tx_q_idx, GPON_TX_FIFO_CONFIGURATION, PDP, reg_pdp);

    return 0;
}

int ag_drv_gpon_tx_fifo_configuration_pdp_get(uint32_t tx_q_idx, uint16_t *pdbc, uint16_t *pdsc)
{
    uint32_t reg_pdp=0;

#ifdef VALIDATE_PARMS
    if(!pdbc || !pdsc)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((tx_q_idx >= 8))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, tx_q_idx, GPON_TX_FIFO_CONFIGURATION, PDP, reg_pdp);

    *pdbc = RU_FIELD_GET(0, GPON_TX_FIFO_CONFIGURATION, PDP, PDBC, reg_pdp);
    *pdsc = RU_FIELD_GET(0, GPON_TX_FIFO_CONFIGURATION, PDP, PDSC, reg_pdp);

    return 0;
}

int ag_drv_gpon_tx_fifo_configuration_pdpgrp_set(uint32_t tx_pd_idx, uint16_t pdbc_grp)
{
    uint32_t reg_pdpgrp=0;

#ifdef VALIDATE_PARMS
    if((tx_pd_idx >= 4) ||
       (pdbc_grp >= _9BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_pdpgrp = RU_FIELD_SET(0, GPON_TX_FIFO_CONFIGURATION, PDPGRP, PDBC_GRP, reg_pdpgrp, pdbc_grp);

    RU_REG_RAM_WRITE(0, tx_pd_idx, GPON_TX_FIFO_CONFIGURATION, PDPGRP, reg_pdpgrp);

    return 0;
}

int ag_drv_gpon_tx_fifo_configuration_pdpgrp_get(uint32_t tx_pd_idx, uint16_t *pdbc_grp)
{
    uint32_t reg_pdpgrp=0;

#ifdef VALIDATE_PARMS
    if(!pdbc_grp)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((tx_pd_idx >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, tx_pd_idx, GPON_TX_FIFO_CONFIGURATION, PDPGRP, reg_pdpgrp);

    *pdbc_grp = RU_FIELD_GET(0, GPON_TX_FIFO_CONFIGURATION, PDPGRP, PDBC_GRP, reg_pdpgrp);

    return 0;
}

int ag_drv_gpon_tx_fifo_configuration_tqp_set(uint32_t tx_q_idx, uint16_t tqbc, uint16_t tqsc)
{
    uint32_t reg_tqp=0;

#ifdef VALIDATE_PARMS
    if((tx_q_idx >= 8) ||
       (tqbc >= _13BITS_MAX_VAL_) ||
       (tqsc >= _13BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_tqp = RU_FIELD_SET(0, GPON_TX_FIFO_CONFIGURATION, TQP, TQBC, reg_tqp, tqbc);
    reg_tqp = RU_FIELD_SET(0, GPON_TX_FIFO_CONFIGURATION, TQP, TQSC, reg_tqp, tqsc);

    RU_REG_RAM_WRITE(0, tx_q_idx, GPON_TX_FIFO_CONFIGURATION, TQP, reg_tqp);

    return 0;
}

int ag_drv_gpon_tx_fifo_configuration_tqp_get(uint32_t tx_q_idx, uint16_t *tqbc, uint16_t *tqsc)
{
    uint32_t reg_tqp=0;

#ifdef VALIDATE_PARMS
    if(!tqbc || !tqsc)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((tx_q_idx >= 8))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, tx_q_idx, GPON_TX_FIFO_CONFIGURATION, TQP, reg_tqp);

    *tqbc = RU_FIELD_GET(0, GPON_TX_FIFO_CONFIGURATION, TQP, TQBC, reg_tqp);
    *tqsc = RU_FIELD_GET(0, GPON_TX_FIFO_CONFIGURATION, TQP, TQSC, reg_tqp);

    return 0;
}

int ag_drv_gpon_tx_fifo_configuration_tdqpgrp_set(uint32_t tx_q_idx, uint16_t tdqb, uint16_t tdqs)
{
    uint32_t reg_tdqpgrp=0;

#ifdef VALIDATE_PARMS
    if((tx_q_idx >= 4) ||
       (tdqb >= _13BITS_MAX_VAL_) ||
       (tdqs >= _13BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_tdqpgrp = RU_FIELD_SET(0, GPON_TX_FIFO_CONFIGURATION, TDQPGRP, TDQB, reg_tdqpgrp, tdqb);
    reg_tdqpgrp = RU_FIELD_SET(0, GPON_TX_FIFO_CONFIGURATION, TDQPGRP, TDQS, reg_tdqpgrp, tdqs);

    RU_REG_RAM_WRITE(0, tx_q_idx, GPON_TX_FIFO_CONFIGURATION, TDQPGRP, reg_tdqpgrp);

    return 0;
}

int ag_drv_gpon_tx_fifo_configuration_tdqpgrp_get(uint32_t tx_q_idx, uint16_t *tdqb, uint16_t *tdqs)
{
    uint32_t reg_tdqpgrp=0;

#ifdef VALIDATE_PARMS
    if(!tdqb || !tdqs)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((tx_q_idx >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, tx_q_idx, GPON_TX_FIFO_CONFIGURATION, TDQPGRP, reg_tdqpgrp);

    *tdqb = RU_FIELD_GET(0, GPON_TX_FIFO_CONFIGURATION, TDQPGRP, TDQB, reg_tdqpgrp);
    *tdqs = RU_FIELD_GET(0, GPON_TX_FIFO_CONFIGURATION, TDQPGRP, TDQS, reg_tdqpgrp);

    return 0;
}

int ag_drv_gpon_tx_fifo_configuration_tdqp_set(uint16_t tdqpid, uint8_t tdq, uint8_t tdqpti)
{
    uint32_t reg_tdqp=0;

#ifdef VALIDATE_PARMS
    if((tdqpid >= _12BITS_MAX_VAL_) ||
       (tdq >= _6BITS_MAX_VAL_) ||
       (tdqpti >= _3BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_tdqp = RU_FIELD_SET(0, GPON_TX_FIFO_CONFIGURATION, TDQP, TDQPID, reg_tdqp, tdqpid);
    reg_tdqp = RU_FIELD_SET(0, GPON_TX_FIFO_CONFIGURATION, TDQP, TDQ, reg_tdqp, tdq);
    reg_tdqp = RU_FIELD_SET(0, GPON_TX_FIFO_CONFIGURATION, TDQP, TDQPTI, reg_tdqp, tdqpti);

    RU_REG_WRITE(0, GPON_TX_FIFO_CONFIGURATION, TDQP, reg_tdqp);

    return 0;
}

int ag_drv_gpon_tx_fifo_configuration_tdqp_get(uint16_t *tdqpid, uint8_t *tdq, uint8_t *tdqpti)
{
    uint32_t reg_tdqp=0;

#ifdef VALIDATE_PARMS
    if(!tdqpid || !tdq || !tdqpti)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_FIFO_CONFIGURATION, TDQP, reg_tdqp);

    *tdqpid = RU_FIELD_GET(0, GPON_TX_FIFO_CONFIGURATION, TDQP, TDQPID, reg_tdqp);
    *tdq = RU_FIELD_GET(0, GPON_TX_FIFO_CONFIGURATION, TDQP, TDQ, reg_tdqp);
    *tdqpti = RU_FIELD_GET(0, GPON_TX_FIFO_CONFIGURATION, TDQP, TDQPTI, reg_tdqp);

    return 0;
}

int ag_drv_gpon_tx_fifo_configuration_lbp_set(uint8_t lbqn)
{
    uint32_t reg_lbp=0;

#ifdef VALIDATE_PARMS
    if((lbqn >= _6BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_lbp = RU_FIELD_SET(0, GPON_TX_FIFO_CONFIGURATION, LBP, LBQN, reg_lbp, lbqn);

    RU_REG_WRITE(0, GPON_TX_FIFO_CONFIGURATION, LBP, reg_lbp);

    return 0;
}

int ag_drv_gpon_tx_fifo_configuration_lbp_get(uint8_t *lbqn)
{
    uint32_t reg_lbp=0;

#ifdef VALIDATE_PARMS
    if(!lbqn)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_FIFO_CONFIGURATION, LBP, reg_lbp);

    *lbqn = RU_FIELD_GET(0, GPON_TX_FIFO_CONFIGURATION, LBP, LBQN, reg_lbp);

    return 0;
}

int ag_drv_gpon_tx_fifo_configuration_tth_set(uint16_t tdmmin, uint16_t tdmmax, uint8_t tthen)
{
    uint32_t reg_tth=0;

#ifdef VALIDATE_PARMS
    if((tdmmin >= _15BITS_MAX_VAL_) ||
       (tdmmax >= _15BITS_MAX_VAL_) ||
       (tthen >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_tth = RU_FIELD_SET(0, GPON_TX_FIFO_CONFIGURATION, TTH, TDMMIN, reg_tth, tdmmin);
    reg_tth = RU_FIELD_SET(0, GPON_TX_FIFO_CONFIGURATION, TTH, TDMMAX, reg_tth, tdmmax);
    reg_tth = RU_FIELD_SET(0, GPON_TX_FIFO_CONFIGURATION, TTH, TTHEN, reg_tth, tthen);

    RU_REG_WRITE(0, GPON_TX_FIFO_CONFIGURATION, TTH, reg_tth);

    return 0;
}

int ag_drv_gpon_tx_fifo_configuration_tth_get(uint16_t *tdmmin, uint16_t *tdmmax, uint8_t *tthen)
{
    uint32_t reg_tth=0;

#ifdef VALIDATE_PARMS
    if(!tdmmin || !tdmmax || !tthen)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_FIFO_CONFIGURATION, TTH, reg_tth);

    *tdmmin = RU_FIELD_GET(0, GPON_TX_FIFO_CONFIGURATION, TTH, TDMMIN, reg_tth);
    *tdmmax = RU_FIELD_GET(0, GPON_TX_FIFO_CONFIGURATION, TTH, TDMMAX, reg_tth);
    *tthen = RU_FIELD_GET(0, GPON_TX_FIFO_CONFIGURATION, TTH, TTHEN, reg_tth);

    return 0;
}


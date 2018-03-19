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
#include "gpon_tx_interrupt_controller.h"
int ag_drv_gpon_tx_interrupt_controller_txisr0_set(uint32_t db_irq_idx, uint8_t db_irq_id)
{
    uint32_t reg_txisr0=0;

#ifdef VALIDATE_PARMS
    if((db_irq_id >= _2BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_READ(0, GPON_TX_INTERRUPT_CONTROLLER, TXISR0, reg_txisr0);

    FIELD_SET(reg_txisr0, (db_irq_idx % 16) *2, 0x2, db_irq_id);

    RU_REG_WRITE(0, GPON_TX_INTERRUPT_CONTROLLER, TXISR0, reg_txisr0);

    return 0;
}

int ag_drv_gpon_tx_interrupt_controller_txisr0_get(uint32_t db_irq_idx, uint8_t *db_irq_id)
{
    uint32_t reg_txisr0=0;

#ifdef VALIDATE_PARMS
    if(!db_irq_id)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_INTERRUPT_CONTROLLER, TXISR0, reg_txisr0);

    *db_irq_id = FIELD_GET(reg_txisr0, (db_irq_idx % 16) *2, 0x2);

    return 0;
}

int ag_drv_gpon_tx_interrupt_controller_txisr1_set(uint32_t db_irq_idx, uint8_t db_irq_id)
{
    uint32_t reg_txisr1=0;

#ifdef VALIDATE_PARMS
    if((db_irq_id >= _2BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_READ(0, GPON_TX_INTERRUPT_CONTROLLER, TXISR1, reg_txisr1);

    FIELD_SET(reg_txisr1, (db_irq_idx % 16) *2, 0x2, db_irq_id);

    RU_REG_WRITE(0, GPON_TX_INTERRUPT_CONTROLLER, TXISR1, reg_txisr1);

    return 0;
}

int ag_drv_gpon_tx_interrupt_controller_txisr1_get(uint32_t db_irq_idx, uint8_t *db_irq_id)
{
    uint32_t reg_txisr1=0;

#ifdef VALIDATE_PARMS
    if(!db_irq_id)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_INTERRUPT_CONTROLLER, TXISR1, reg_txisr1);

    *db_irq_id = FIELD_GET(reg_txisr1, (db_irq_idx % 16) *2, 0x2);

    return 0;
}

int ag_drv_gpon_tx_interrupt_controller_txisr2_set(uint32_t db_irq_idx, uint8_t db_irq_id)
{
    uint32_t reg_txisr2=0;

#ifdef VALIDATE_PARMS
    if((db_irq_id >= _2BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_READ(0, GPON_TX_INTERRUPT_CONTROLLER, TXISR2, reg_txisr2);

    FIELD_SET(reg_txisr2, (db_irq_idx % 16) *2, 0x2, db_irq_id);

    RU_REG_WRITE(0, GPON_TX_INTERRUPT_CONTROLLER, TXISR2, reg_txisr2);

    return 0;
}

int ag_drv_gpon_tx_interrupt_controller_txisr2_get(uint32_t db_irq_idx, uint8_t *db_irq_id)
{
    uint32_t reg_txisr2=0;

#ifdef VALIDATE_PARMS
    if(!db_irq_id)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_INTERRUPT_CONTROLLER, TXISR2, reg_txisr2);

    *db_irq_id = FIELD_GET(reg_txisr2, (db_irq_idx % 16) *2, 0x2);

    return 0;
}

int ag_drv_gpon_tx_interrupt_controller_txier0_set(uint32_t int_src_idx, uint8_t int_src_word)
{
    uint32_t reg_txier0=0;

#ifdef VALIDATE_PARMS
#endif

    RU_REG_READ(0, GPON_TX_INTERRUPT_CONTROLLER, TXIER0, reg_txier0);

    FIELD_SET(reg_txier0, (int_src_idx % 32) *1, 0x1, int_src_word);

    RU_REG_WRITE(0, GPON_TX_INTERRUPT_CONTROLLER, TXIER0, reg_txier0);

    return 0;
}

int ag_drv_gpon_tx_interrupt_controller_txier0_get(uint32_t int_src_idx, uint8_t *int_src_word)
{
    uint32_t reg_txier0=0;

#ifdef VALIDATE_PARMS
    if(!int_src_word)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_INTERRUPT_CONTROLLER, TXIER0, reg_txier0);

    *int_src_word = FIELD_GET(reg_txier0, (int_src_idx % 32) *1, 0x1);

    return 0;
}

int ag_drv_gpon_tx_interrupt_controller_txier1_set(uint32_t int_src_idx, uint8_t int_src_word)
{
    uint32_t reg_txier1=0;

#ifdef VALIDATE_PARMS
    if((int_src_word >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_READ(0, GPON_TX_INTERRUPT_CONTROLLER, TXIER1, reg_txier1);

    FIELD_SET(reg_txier1, (int_src_idx % 32) *1, 0x1, int_src_word);

    RU_REG_WRITE(0, GPON_TX_INTERRUPT_CONTROLLER, TXIER1, reg_txier1);

    return 0;
}

int ag_drv_gpon_tx_interrupt_controller_txier1_get(uint32_t int_src_idx, uint8_t *int_src_word)
{
    uint32_t reg_txier1=0;

#ifdef VALIDATE_PARMS
    if(!int_src_word)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_INTERRUPT_CONTROLLER, TXIER1, reg_txier1);

    *int_src_word = FIELD_GET(reg_txier1, (int_src_idx % 32) *1, 0x1);

    return 0;
}

int ag_drv_gpon_tx_interrupt_controller_txier2_set(uint32_t int_src_idx, uint8_t int_src_word)
{
    uint32_t reg_txier2=0;

#ifdef VALIDATE_PARMS
    if((int_src_word >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_READ(0, GPON_TX_INTERRUPT_CONTROLLER, TXIER2, reg_txier2);

    FIELD_SET(reg_txier2, (int_src_idx % 32) *1, 0x1, int_src_word);

    RU_REG_WRITE(0, GPON_TX_INTERRUPT_CONTROLLER, TXIER2, reg_txier2);

    return 0;
}

int ag_drv_gpon_tx_interrupt_controller_txier2_get(uint32_t int_src_idx, uint8_t *int_src_word)
{
    uint32_t reg_txier2=0;

#ifdef VALIDATE_PARMS
    if(!int_src_word)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_INTERRUPT_CONTROLLER, TXIER2, reg_txier2);

    *int_src_word = FIELD_GET(reg_txier2, (int_src_idx % 32) *1, 0x1);

    return 0;
}

int ag_drv_gpon_tx_interrupt_controller_txitr0_set(uint32_t int_src_idx, uint8_t data)
{
    uint32_t reg_txitr0=0;

#ifdef VALIDATE_PARMS
#endif

    RU_REG_READ(0, GPON_TX_INTERRUPT_CONTROLLER, TXITR0, reg_txitr0);

    FIELD_SET(reg_txitr0, (int_src_idx % 32) *1, 0x1, data);

    RU_REG_WRITE(0, GPON_TX_INTERRUPT_CONTROLLER, TXITR0, reg_txitr0);

    return 0;
}

int ag_drv_gpon_tx_interrupt_controller_txitr0_get(uint32_t int_src_idx, uint8_t *data)
{
    uint32_t reg_txitr0=0;

#ifdef VALIDATE_PARMS
    if(!data)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_INTERRUPT_CONTROLLER, TXITR0, reg_txitr0);

    *data = FIELD_GET(reg_txitr0, (int_src_idx % 32) *1, 0x1);

    return 0;
}

int ag_drv_gpon_tx_interrupt_controller_txitr1_set(uint32_t int_src_idx, uint8_t int_src_word)
{
    uint32_t reg_txitr1=0;

#ifdef VALIDATE_PARMS
#endif

    RU_REG_READ(0, GPON_TX_INTERRUPT_CONTROLLER, TXITR1, reg_txitr1);

    FIELD_SET(reg_txitr1, (int_src_idx % 32) *1, 0x1, int_src_word);

    RU_REG_WRITE(0, GPON_TX_INTERRUPT_CONTROLLER, TXITR1, reg_txitr1);

    return 0;
}

int ag_drv_gpon_tx_interrupt_controller_txitr1_get(uint32_t int_src_idx, uint8_t *int_src_word)
{
    uint32_t reg_txitr1=0;

#ifdef VALIDATE_PARMS
    if(!int_src_word)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_INTERRUPT_CONTROLLER, TXITR1, reg_txitr1);

    *int_src_word = FIELD_GET(reg_txitr1, (int_src_idx % 32) *1, 0x1);

    return 0;
}

int ag_drv_gpon_tx_interrupt_controller_txitr2_set(uint32_t int_src_idx, uint8_t int_src_word)
{
    uint32_t reg_txitr2=0;

#ifdef VALIDATE_PARMS
    if((int_src_word >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_READ(0, GPON_TX_INTERRUPT_CONTROLLER, TXITR2, reg_txitr2);

    FIELD_SET(reg_txitr2, (int_src_idx % 32) *1, 0x1, int_src_word);

    RU_REG_WRITE(0, GPON_TX_INTERRUPT_CONTROLLER, TXITR2, reg_txitr2);

    return 0;
}

int ag_drv_gpon_tx_interrupt_controller_txitr2_get(uint32_t int_src_idx, uint8_t *int_src_word)
{
    uint32_t reg_txitr2=0;

#ifdef VALIDATE_PARMS
    if(!int_src_word)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_INTERRUPT_CONTROLLER, TXITR2, reg_txitr2);

    *int_src_word = FIELD_GET(reg_txitr2, (int_src_idx % 32) *1, 0x1);

    return 0;
}


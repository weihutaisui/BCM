/*
   Copyright (c) 2015 Broadcom Corporation
   All Rights Reserved

    <:label-BRCM:2015:DUAL/GPL:standard

    Unless you and Broadcom execute a separate written software license
    agreement governing use of this software, this software is licensed
    to you under the terms of the GNU General Public License version 2
    (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
    with the following added to such license:

       As a special exception, the copyright holders of this software give
       you permission to link this software with independent modules, and
       to copy and distribute the resulting executable under terms of your
       choice, provided that you also meet, for each linked independent
       module, the terms and conditions of the license of that module.
       An independent module is a module which is not derived from this
       software.  The special exception does not apply to any modifications
       of the software.

    Not withstanding the above, under no circumstances may you combine
    this software in any way with any other Broadcom software provided
    under a license other than the GPL, without Broadcom's express prior
    written consent.

:>
*/

#include "rdp_common.h"
#include "drivers_common_ag.h"
#include "gpon_tx_interrupt_controller.h"
bdmf_error_t ag_drv_gpon_tx_interrupt_controller_txisr0_set(uint32_t db_irq_idx, uint8_t db_irq_id)
{
    uint32_t reg_txisr0=0;

#ifdef VALIDATE_PARMS
    if((db_irq_id >= _2BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    RU_REG_READ(0, GPON_TX_INTERRUPT_CONTROLLER, TXISR0, reg_txisr0);

    FIELD_SET(reg_txisr0, (db_irq_idx % 16) *2, 0x2, db_irq_id);

    RU_REG_WRITE(0, GPON_TX_INTERRUPT_CONTROLLER, TXISR0, reg_txisr0);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_interrupt_controller_txisr0_get(uint32_t db_irq_idx, uint8_t *db_irq_id)
{
    uint32_t reg_txisr0=0;

#ifdef VALIDATE_PARMS
    if(!db_irq_id)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_INTERRUPT_CONTROLLER, TXISR0, reg_txisr0);

    *db_irq_id = FIELD_GET(reg_txisr0, (db_irq_idx % 16) *2, 0x2);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_interrupt_controller_txisr1_set(uint32_t db_irq_idx, uint8_t db_irq_id)
{
    uint32_t reg_txisr1=0;

#ifdef VALIDATE_PARMS
    if((db_irq_id >= _2BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    RU_REG_READ(0, GPON_TX_INTERRUPT_CONTROLLER, TXISR1, reg_txisr1);

    FIELD_SET(reg_txisr1, (db_irq_idx % 16) *2, 0x2, db_irq_id);

    RU_REG_WRITE(0, GPON_TX_INTERRUPT_CONTROLLER, TXISR1, reg_txisr1);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_interrupt_controller_txisr1_get(uint32_t db_irq_idx, uint8_t *db_irq_id)
{
    uint32_t reg_txisr1=0;

#ifdef VALIDATE_PARMS
    if(!db_irq_id)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_INTERRUPT_CONTROLLER, TXISR1, reg_txisr1);

    *db_irq_id = FIELD_GET(reg_txisr1, (db_irq_idx % 16) *2, 0x2);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_interrupt_controller_txisr2_set(uint32_t db_irq_idx, uint8_t db_irq_id)
{
    uint32_t reg_txisr2=0;

#ifdef VALIDATE_PARMS
    if((db_irq_id >= _2BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    RU_REG_READ(0, GPON_TX_INTERRUPT_CONTROLLER, TXISR2, reg_txisr2);

    FIELD_SET(reg_txisr2, (db_irq_idx % 16) *2, 0x2, db_irq_id);

    RU_REG_WRITE(0, GPON_TX_INTERRUPT_CONTROLLER, TXISR2, reg_txisr2);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_interrupt_controller_txisr2_get(uint32_t db_irq_idx, uint8_t *db_irq_id)
{
    uint32_t reg_txisr2=0;

#ifdef VALIDATE_PARMS
    if(!db_irq_id)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_INTERRUPT_CONTROLLER, TXISR2, reg_txisr2);

    *db_irq_id = FIELD_GET(reg_txisr2, (db_irq_idx % 16) *2, 0x2);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_interrupt_controller_txier0_set(uint32_t int_src_idx, bdmf_boolean int_src_word)
{
    uint32_t reg_txier0=0;

#ifdef VALIDATE_PARMS
#endif

    RU_REG_READ(0, GPON_TX_INTERRUPT_CONTROLLER, TXIER0, reg_txier0);

    FIELD_SET(reg_txier0, (int_src_idx % 32) *1, 0x1, int_src_word);

    RU_REG_WRITE(0, GPON_TX_INTERRUPT_CONTROLLER, TXIER0, reg_txier0);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_interrupt_controller_txier0_get(uint32_t int_src_idx, bdmf_boolean *int_src_word)
{
    uint32_t reg_txier0=0;

#ifdef VALIDATE_PARMS
    if(!int_src_word)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_INTERRUPT_CONTROLLER, TXIER0, reg_txier0);

    *int_src_word = FIELD_GET(reg_txier0, (int_src_idx % 32) *1, 0x1);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_interrupt_controller_txier1_set(uint32_t int_src_idx, bdmf_boolean int_src_word)
{
    uint32_t reg_txier1=0;

#ifdef VALIDATE_PARMS
#endif

    RU_REG_READ(0, GPON_TX_INTERRUPT_CONTROLLER, TXIER1, reg_txier1);

    FIELD_SET(reg_txier1, (int_src_idx % 32) *1, 0x1, int_src_word);

    RU_REG_WRITE(0, GPON_TX_INTERRUPT_CONTROLLER, TXIER1, reg_txier1);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_interrupt_controller_txier1_get(uint32_t int_src_idx, bdmf_boolean *int_src_word)
{
    uint32_t reg_txier1=0;

#ifdef VALIDATE_PARMS
    if(!int_src_word)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_INTERRUPT_CONTROLLER, TXIER1, reg_txier1);

    *int_src_word = FIELD_GET(reg_txier1, (int_src_idx % 32) *1, 0x1);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_interrupt_controller_txier2_set(uint32_t int_src_idx, bdmf_boolean int_src_word)
{
    uint32_t reg_txier2=0;

#ifdef VALIDATE_PARMS
    if((int_src_word >= _1BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    RU_REG_READ(0, GPON_TX_INTERRUPT_CONTROLLER, TXIER2, reg_txier2);

    FIELD_SET(reg_txier2, (int_src_idx % 32) *1, 0x1, int_src_word);

    RU_REG_WRITE(0, GPON_TX_INTERRUPT_CONTROLLER, TXIER2, reg_txier2);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_interrupt_controller_txier2_get(uint32_t int_src_idx, bdmf_boolean *int_src_word)
{
    uint32_t reg_txier2=0;

#ifdef VALIDATE_PARMS
    if(!int_src_word)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_INTERRUPT_CONTROLLER, TXIER2, reg_txier2);

    *int_src_word = FIELD_GET(reg_txier2, (int_src_idx % 32) *1, 0x1);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_interrupt_controller_txitr0_set(uint32_t int_src_idx, bdmf_boolean data)
{
    uint32_t reg_txitr0=0;

#ifdef VALIDATE_PARMS
#endif

    RU_REG_READ(0, GPON_TX_INTERRUPT_CONTROLLER, TXITR0, reg_txitr0);

    FIELD_SET(reg_txitr0, (int_src_idx % 32) *1, 0x1, data);

    RU_REG_WRITE(0, GPON_TX_INTERRUPT_CONTROLLER, TXITR0, reg_txitr0);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_interrupt_controller_txitr0_get(uint32_t int_src_idx, bdmf_boolean *data)
{
    uint32_t reg_txitr0=0;

#ifdef VALIDATE_PARMS
    if(!data)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_INTERRUPT_CONTROLLER, TXITR0, reg_txitr0);

    *data = FIELD_GET(reg_txitr0, (int_src_idx % 32) *1, 0x1);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_interrupt_controller_txitr1_set(uint32_t int_src_idx, bdmf_boolean int_src_word)
{
    uint32_t reg_txitr1=0;

#ifdef VALIDATE_PARMS
#endif

    RU_REG_READ(0, GPON_TX_INTERRUPT_CONTROLLER, TXITR1, reg_txitr1);

    FIELD_SET(reg_txitr1, (int_src_idx % 32) *1, 0x1, int_src_word);

    RU_REG_WRITE(0, GPON_TX_INTERRUPT_CONTROLLER, TXITR1, reg_txitr1);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_interrupt_controller_txitr1_get(uint32_t int_src_idx, bdmf_boolean *int_src_word)
{
    uint32_t reg_txitr1=0;

#ifdef VALIDATE_PARMS
    if(!int_src_word)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_INTERRUPT_CONTROLLER, TXITR1, reg_txitr1);

    *int_src_word = FIELD_GET(reg_txitr1, (int_src_idx % 32) *1, 0x1);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_interrupt_controller_txitr2_set(uint32_t int_src_idx, bdmf_boolean int_src_word)
{
    uint32_t reg_txitr2=0;

#ifdef VALIDATE_PARMS
    if((int_src_word >= _1BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    RU_REG_READ(0, GPON_TX_INTERRUPT_CONTROLLER, TXITR2, reg_txitr2);

    FIELD_SET(reg_txitr2, (int_src_idx % 32) *1, 0x1, int_src_word);

    RU_REG_WRITE(0, GPON_TX_INTERRUPT_CONTROLLER, TXITR2, reg_txitr2);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_interrupt_controller_txitr2_get(uint32_t int_src_idx, bdmf_boolean *int_src_word)
{
    uint32_t reg_txitr2=0;

#ifdef VALIDATE_PARMS
    if(!int_src_word)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_INTERRUPT_CONTROLLER, TXITR2, reg_txitr2);

    *int_src_word = FIELD_GET(reg_txitr2, (int_src_idx % 32) *1, 0x1);

    return BDMF_ERR_OK;
}

#ifdef USE_BDMF_SHELL
enum
{
    BDMF_txisr0,
    BDMF_txisr1,
    BDMF_txisr2,
    BDMF_txier0,
    BDMF_txier1,
    BDMF_txier2,
    BDMF_txitr0,
    BDMF_txitr1,
    BDMF_txitr2,
};

typedef enum
{
    bdmf_address_txisr0,
    bdmf_address_txisr1,
    bdmf_address_txisr2,
    bdmf_address_txier0,
    bdmf_address_txier1,
    bdmf_address_txier2,
    bdmf_address_txitr0,
    bdmf_address_txitr1,
    bdmf_address_txitr2,
}
bdmf_address;

static int bcm_gpon_tx_interrupt_controller_cli_set(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_error_t err;

    switch(parm[0].value.unumber)
    {
    case BDMF_txisr0:
        err = ag_drv_gpon_tx_interrupt_controller_txisr0_set(parm[1].value.unumber, parm[2].value.unumber);
        break;
    case BDMF_txisr1:
        err = ag_drv_gpon_tx_interrupt_controller_txisr1_set(parm[2].value.unumber, parm[3].value.unumber);
        break;
    case BDMF_txisr2:
        err = ag_drv_gpon_tx_interrupt_controller_txisr2_set(parm[3].value.unumber, parm[4].value.unumber);
        break;
    case BDMF_txier0:
        err = ag_drv_gpon_tx_interrupt_controller_txier0_set(parm[4].value.unumber, parm[5].value.unumber);
        break;
    case BDMF_txier1:
        err = ag_drv_gpon_tx_interrupt_controller_txier1_set(parm[5].value.unumber, parm[6].value.unumber);
        break;
    case BDMF_txier2:
        err = ag_drv_gpon_tx_interrupt_controller_txier2_set(parm[6].value.unumber, parm[7].value.unumber);
        break;
    case BDMF_txitr0:
        err = ag_drv_gpon_tx_interrupt_controller_txitr0_set(parm[7].value.unumber, parm[8].value.unumber);
        break;
    case BDMF_txitr1:
        err = ag_drv_gpon_tx_interrupt_controller_txitr1_set(parm[8].value.unumber, parm[9].value.unumber);
        break;
    case BDMF_txitr2:
        err = ag_drv_gpon_tx_interrupt_controller_txitr2_set(parm[9].value.unumber, parm[10].value.unumber);
        break;
    default:
        err = BDMF_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

static int bcm_gpon_tx_interrupt_controller_cli_get(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_error_t err;

    switch(parm[0].value.unumber)
    {
    case BDMF_txisr0:
    {
        uint8_t db_irq_id;
        err = ag_drv_gpon_tx_interrupt_controller_txisr0_get(parm[1].value.unumber, &db_irq_id);
        bdmf_session_print(session, "db_irq_id = %u = 0x%x\n", db_irq_id, db_irq_id);
        break;
    }
    case BDMF_txisr1:
    {
        uint8_t db_irq_id;
        err = ag_drv_gpon_tx_interrupt_controller_txisr1_get(parm[1].value.unumber, &db_irq_id);
        bdmf_session_print(session, "db_irq_id = %u = 0x%x\n", db_irq_id, db_irq_id);
        break;
    }
    case BDMF_txisr2:
    {
        uint8_t db_irq_id;
        err = ag_drv_gpon_tx_interrupt_controller_txisr2_get(parm[1].value.unumber, &db_irq_id);
        bdmf_session_print(session, "db_irq_id = %u = 0x%x\n", db_irq_id, db_irq_id);
        break;
    }
    case BDMF_txier0:
    {
        bdmf_boolean int_src_word;
        err = ag_drv_gpon_tx_interrupt_controller_txier0_get(parm[1].value.unumber, &int_src_word);
        bdmf_session_print(session, "int_src_word = %u = 0x%x\n", int_src_word, int_src_word);
        break;
    }
    case BDMF_txier1:
    {
        bdmf_boolean int_src_word;
        err = ag_drv_gpon_tx_interrupt_controller_txier1_get(parm[1].value.unumber, &int_src_word);
        bdmf_session_print(session, "int_src_word = %u = 0x%x\n", int_src_word, int_src_word);
        break;
    }
    case BDMF_txier2:
    {
        bdmf_boolean int_src_word;
        err = ag_drv_gpon_tx_interrupt_controller_txier2_get(parm[1].value.unumber, &int_src_word);
        bdmf_session_print(session, "int_src_word = %u = 0x%x\n", int_src_word, int_src_word);
        break;
    }
    case BDMF_txitr0:
    {
        bdmf_boolean data;
        err = ag_drv_gpon_tx_interrupt_controller_txitr0_get(parm[1].value.unumber, &data);
        bdmf_session_print(session, "data = %u = 0x%x\n", data, data);
        break;
    }
    case BDMF_txitr1:
    {
        bdmf_boolean int_src_word;
        err = ag_drv_gpon_tx_interrupt_controller_txitr1_get(parm[1].value.unumber, &int_src_word);
        bdmf_session_print(session, "int_src_word = %u = 0x%x\n", int_src_word, int_src_word);
        break;
    }
    case BDMF_txitr2:
    {
        bdmf_boolean int_src_word;
        err = ag_drv_gpon_tx_interrupt_controller_txitr2_get(parm[1].value.unumber, &int_src_word);
        bdmf_session_print(session, "int_src_word = %u = 0x%x\n", int_src_word, int_src_word);
        break;
    }
    default:
        err = BDMF_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

static int bcm_gpon_tx_interrupt_controller_cli_test(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_test_method m = parm[0].value.unumber;
    bdmf_error_t err = BDMF_ERR_OK;

    {
        uint32_t db_irq_idx=gtmv(m, 4);
        uint8_t db_irq_id=gtmv(m, 2);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_interrupt_controller_txisr0_set( %u %u)\n", db_irq_idx, db_irq_id);
        if(!err) ag_drv_gpon_tx_interrupt_controller_txisr0_set(db_irq_idx, db_irq_id);
        if(!err) ag_drv_gpon_tx_interrupt_controller_txisr0_get( db_irq_idx, &db_irq_id);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_interrupt_controller_txisr0_get( %u %u)\n", db_irq_idx, db_irq_id);
        if(err || db_irq_id!=gtmv(m, 2))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t db_irq_idx=gtmv(m, 4);
        uint8_t db_irq_id=gtmv(m, 2);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_interrupt_controller_txisr1_set( %u %u)\n", db_irq_idx, db_irq_id);
        if(!err) ag_drv_gpon_tx_interrupt_controller_txisr1_set(db_irq_idx, db_irq_id);
        if(!err) ag_drv_gpon_tx_interrupt_controller_txisr1_get( db_irq_idx, &db_irq_id);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_interrupt_controller_txisr1_get( %u %u)\n", db_irq_idx, db_irq_id);
        if(err || db_irq_id!=gtmv(m, 2))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t db_irq_idx=gtmv(m, 4);
        uint8_t db_irq_id=gtmv(m, 2);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_interrupt_controller_txisr2_set( %u %u)\n", db_irq_idx, db_irq_id);
        if(!err) ag_drv_gpon_tx_interrupt_controller_txisr2_set(db_irq_idx, db_irq_id);
        if(!err) ag_drv_gpon_tx_interrupt_controller_txisr2_get( db_irq_idx, &db_irq_id);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_interrupt_controller_txisr2_get( %u %u)\n", db_irq_idx, db_irq_id);
        if(err || db_irq_id!=gtmv(m, 2))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t int_src_idx=gtmv(m, 5);
        bdmf_boolean int_src_word=gtmv(m, 1);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_interrupt_controller_txier0_set( %u %u)\n", int_src_idx, int_src_word);
        if(!err) ag_drv_gpon_tx_interrupt_controller_txier0_set(int_src_idx, int_src_word);
        if(!err) ag_drv_gpon_tx_interrupt_controller_txier0_get( int_src_idx, &int_src_word);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_interrupt_controller_txier0_get( %u %u)\n", int_src_idx, int_src_word);
        if(err || int_src_word!=gtmv(m, 1))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t int_src_idx=gtmv(m, 5);
        bdmf_boolean int_src_word=gtmv(m, 1);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_interrupt_controller_txier1_set( %u %u)\n", int_src_idx, int_src_word);
        if(!err) ag_drv_gpon_tx_interrupt_controller_txier1_set(int_src_idx, int_src_word);
        if(!err) ag_drv_gpon_tx_interrupt_controller_txier1_get( int_src_idx, &int_src_word);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_interrupt_controller_txier1_get( %u %u)\n", int_src_idx, int_src_word);
        if(err || int_src_word!=gtmv(m, 1))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t int_src_idx=gtmv(m, 5);
        bdmf_boolean int_src_word=gtmv(m, 1);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_interrupt_controller_txier2_set( %u %u)\n", int_src_idx, int_src_word);
        if(!err) ag_drv_gpon_tx_interrupt_controller_txier2_set(int_src_idx, int_src_word);
        if(!err) ag_drv_gpon_tx_interrupt_controller_txier2_get( int_src_idx, &int_src_word);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_interrupt_controller_txier2_get( %u %u)\n", int_src_idx, int_src_word);
        if(err || int_src_word!=gtmv(m, 1))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t int_src_idx=gtmv(m, 5);
        bdmf_boolean data=gtmv(m, 1);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_interrupt_controller_txitr0_set( %u %u)\n", int_src_idx, data);
        if(!err) ag_drv_gpon_tx_interrupt_controller_txitr0_set(int_src_idx, data);
        if(!err) ag_drv_gpon_tx_interrupt_controller_txitr0_get( int_src_idx, &data);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_interrupt_controller_txitr0_get( %u %u)\n", int_src_idx, data);
        if(err || data!=gtmv(m, 1))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t int_src_idx=gtmv(m, 5);
        bdmf_boolean int_src_word=gtmv(m, 1);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_interrupt_controller_txitr1_set( %u %u)\n", int_src_idx, int_src_word);
        if(!err) ag_drv_gpon_tx_interrupt_controller_txitr1_set(int_src_idx, int_src_word);
        if(!err) ag_drv_gpon_tx_interrupt_controller_txitr1_get( int_src_idx, &int_src_word);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_interrupt_controller_txitr1_get( %u %u)\n", int_src_idx, int_src_word);
        if(err || int_src_word!=gtmv(m, 1))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t int_src_idx=gtmv(m, 5);
        bdmf_boolean int_src_word=gtmv(m, 1);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_interrupt_controller_txitr2_set( %u %u)\n", int_src_idx, int_src_word);
        if(!err) ag_drv_gpon_tx_interrupt_controller_txitr2_set(int_src_idx, int_src_word);
        if(!err) ag_drv_gpon_tx_interrupt_controller_txitr2_get( int_src_idx, &int_src_word);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_interrupt_controller_txitr2_get( %u %u)\n", int_src_idx, int_src_word);
        if(err || int_src_word!=gtmv(m, 1))
            return err ? err : BDMF_ERR_IO;
    }
    return err;
}

static int bcm_gpon_tx_interrupt_controller_cli_address(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    uint32_t i;
    uint32_t j;
    uint32_t index1_start=0;
    uint32_t index1_stop;
    uint32_t index2_start=0;
    uint32_t index2_stop;
    bdmfmon_cmd_parm_t * bdmf_parm;
    const ru_reg_rec * reg;
    const ru_block_rec * blk;
    const char * enum_string = bdmfmon_enum_parm_stringval(session, 0, parm[0].value.unumber);

    if(!enum_string)
        return BDMF_ERR_INTERNAL;

    switch (parm[0].value.unumber)
    {
    case bdmf_address_txisr0 : reg = &RU_REG(GPON_TX_INTERRUPT_CONTROLLER, TXISR0); blk = &RU_BLK(GPON_TX_INTERRUPT_CONTROLLER); break;
    case bdmf_address_txisr1 : reg = &RU_REG(GPON_TX_INTERRUPT_CONTROLLER, TXISR1); blk = &RU_BLK(GPON_TX_INTERRUPT_CONTROLLER); break;
    case bdmf_address_txisr2 : reg = &RU_REG(GPON_TX_INTERRUPT_CONTROLLER, TXISR2); blk = &RU_BLK(GPON_TX_INTERRUPT_CONTROLLER); break;
    case bdmf_address_txier0 : reg = &RU_REG(GPON_TX_INTERRUPT_CONTROLLER, TXIER0); blk = &RU_BLK(GPON_TX_INTERRUPT_CONTROLLER); break;
    case bdmf_address_txier1 : reg = &RU_REG(GPON_TX_INTERRUPT_CONTROLLER, TXIER1); blk = &RU_BLK(GPON_TX_INTERRUPT_CONTROLLER); break;
    case bdmf_address_txier2 : reg = &RU_REG(GPON_TX_INTERRUPT_CONTROLLER, TXIER2); blk = &RU_BLK(GPON_TX_INTERRUPT_CONTROLLER); break;
    case bdmf_address_txitr0 : reg = &RU_REG(GPON_TX_INTERRUPT_CONTROLLER, TXITR0); blk = &RU_BLK(GPON_TX_INTERRUPT_CONTROLLER); break;
    case bdmf_address_txitr1 : reg = &RU_REG(GPON_TX_INTERRUPT_CONTROLLER, TXITR1); blk = &RU_BLK(GPON_TX_INTERRUPT_CONTROLLER); break;
    case bdmf_address_txitr2 : reg = &RU_REG(GPON_TX_INTERRUPT_CONTROLLER, TXITR2); blk = &RU_BLK(GPON_TX_INTERRUPT_CONTROLLER); break;
    default :
        return BDMF_ERR_NOT_SUPPORTED;
        break;
    }
    if((bdmf_parm = bdmfmon_find_named_parm(session,"index1")))
    {
        index1_start = bdmf_parm->value.unumber;
        index1_stop = index1_start + 1;
    }
    else
        index1_stop = blk->addr_count;
    if((bdmf_parm = bdmfmon_find_named_parm(session,"index2")))
    {
        index2_start = bdmf_parm->value.unumber;
        index2_stop = index2_start + 1;
    }
    else
        index2_stop = reg->ram_count + 1;
    if(index1_stop > blk->addr_count)
    {
        bdmf_session_print(session, "index1 (%u) is out of range (%u).\n", index1_stop, blk->addr_count);
        return BDMF_ERR_RANGE;
    }
    if(index2_stop > (reg->ram_count + 1))
    {
        bdmf_session_print(session, "index2 (%u) is out of range (%u).\n", index2_stop, reg->ram_count + 1);
        return BDMF_ERR_RANGE;
    }
    if(reg->ram_count)
        for (i = index1_start; i < index1_stop; i++)
        {
            bdmf_session_print(session, "index1 = %u\n", i);
            for (j = index2_start; j < index2_stop; j++)
                bdmf_session_print(session, 	 "(%5u) 0x%08X\n", j, (uint32_t)((uint32_t*)(blk->addr[i] + reg->addr) + j));
        }
    else
        for (i = index1_start; i < index1_stop; i++)
            bdmf_session_print(session, "(%3u) 0x%16lX\n", i, blk->addr[i]+reg->addr);
    return 0;
}

bdmfmon_handle_t ag_drv_gpon_tx_interrupt_controller_cli_init(bdmfmon_handle_t driver_dir)
{
    bdmfmon_handle_t dir;

    if ((dir = bdmfmon_dir_find(driver_dir, "gpon_tx_interrupt_controller"))!=NULL)
        return dir;
    dir = bdmfmon_dir_add(driver_dir, "gpon_tx_interrupt_controller", "gpon_tx_interrupt_controller", BDMF_ACCESS_ADMIN, NULL);

    {
        static bdmfmon_cmd_parm_t set_txisr0[]={
            BDMFMON_MAKE_PARM("db_irq_idx", "db_irq_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("db_irq_id", "db_irq_id", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_txisr1[]={
            BDMFMON_MAKE_PARM("db_irq_idx", "db_irq_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("db_irq_id", "db_irq_id", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_txisr2[]={
            BDMFMON_MAKE_PARM("db_irq_idx", "db_irq_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("db_irq_id", "db_irq_id", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_txier0[]={
            BDMFMON_MAKE_PARM("int_src_idx", "int_src_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("int_src_word", "int_src_word", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_txier1[]={
            BDMFMON_MAKE_PARM("int_src_idx", "int_src_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("int_src_word", "int_src_word", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_txier2[]={
            BDMFMON_MAKE_PARM("int_src_idx", "int_src_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("int_src_word", "int_src_word", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_txitr0[]={
            BDMFMON_MAKE_PARM("int_src_idx", "int_src_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("data", "data", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_txitr1[]={
            BDMFMON_MAKE_PARM("int_src_idx", "int_src_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("int_src_word", "int_src_word", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_txitr2[]={
            BDMFMON_MAKE_PARM("int_src_idx", "int_src_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("int_src_word", "int_src_word", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_enum_val_t selector_table[] = {
            { .name="txisr0", .val=BDMF_txisr0, .parms=set_txisr0 },
            { .name="txisr1", .val=BDMF_txisr1, .parms=set_txisr1 },
            { .name="txisr2", .val=BDMF_txisr2, .parms=set_txisr2 },
            { .name="txier0", .val=BDMF_txier0, .parms=set_txier0 },
            { .name="txier1", .val=BDMF_txier1, .parms=set_txier1 },
            { .name="txier2", .val=BDMF_txier2, .parms=set_txier2 },
            { .name="txitr0", .val=BDMF_txitr0, .parms=set_txitr0 },
            { .name="txitr1", .val=BDMF_txitr1, .parms=set_txitr1 },
            { .name="txitr2", .val=BDMF_txitr2, .parms=set_txitr2 },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "set", "set", bcm_gpon_tx_interrupt_controller_cli_set,
            BDMFMON_MAKE_PARM_SELECTOR("purpose", "purpose", selector_table, 0));
    }
    {
        static bdmfmon_cmd_parm_t set_default[]={
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_enum_val_t selector_table[] = {
            { .name="txisr0", .val=BDMF_txisr0, .parms=set_default },
            { .name="txisr1", .val=BDMF_txisr1, .parms=set_default },
            { .name="txisr2", .val=BDMF_txisr2, .parms=set_default },
            { .name="txier0", .val=BDMF_txier0, .parms=set_default },
            { .name="txier1", .val=BDMF_txier1, .parms=set_default },
            { .name="txier2", .val=BDMF_txier2, .parms=set_default },
            { .name="txitr0", .val=BDMF_txitr0, .parms=set_default },
            { .name="txitr1", .val=BDMF_txitr1, .parms=set_default },
            { .name="txitr2", .val=BDMF_txitr2, .parms=set_default },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "get", "get", bcm_gpon_tx_interrupt_controller_cli_get,
            BDMFMON_MAKE_PARM_SELECTOR("purpose", "purpose", selector_table, 0));
    }
    {
        static bdmfmon_enum_val_t enum_table_test_method[] = {
            { .name="low" , .val=bdmf_test_method_low },
            { .name="mid" , .val=bdmf_test_method_mid },
            { .name="high" , .val=bdmf_test_method_high },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "test", "test", bcm_gpon_tx_interrupt_controller_cli_test,
            BDMFMON_MAKE_PARM_ENUM("method", "low: 0000, mid: 1000, high: 1111", enum_table_test_method, 0));
    }
    {
        static bdmfmon_enum_val_t enum_table_address[] = {
            { .name="TXISR0" , .val=bdmf_address_txisr0 },
            { .name="TXISR1" , .val=bdmf_address_txisr1 },
            { .name="TXISR2" , .val=bdmf_address_txisr2 },
            { .name="TXIER0" , .val=bdmf_address_txier0 },
            { .name="TXIER1" , .val=bdmf_address_txier1 },
            { .name="TXIER2" , .val=bdmf_address_txier2 },
            { .name="TXITR0" , .val=bdmf_address_txitr0 },
            { .name="TXITR1" , .val=bdmf_address_txitr1 },
            { .name="TXITR2" , .val=bdmf_address_txitr2 },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "address", "address", bcm_gpon_tx_interrupt_controller_cli_address,
            BDMFMON_MAKE_PARM_ENUM("method", "method", enum_table_address, 0),
            BDMFMON_MAKE_PARM("index2", "onu_id/alloc_id/port_id/etc...", BDMFMON_PARM_NUMBER, BDMFMON_PARM_FLAG_OPTIONAL));
    }
    return dir;
}
#endif /* USE_BDMF_SHELL */


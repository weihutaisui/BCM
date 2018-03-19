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
#include "gpon_tx_fifo_configuration.h"
bdmf_error_t ag_drv_gpon_tx_fifo_configuration_pdp_set(uint32_t tx_q_idx, uint16_t pdbc, uint16_t pdsc)
{
    uint32_t reg_pdp=0;

#ifdef VALIDATE_PARMS
    if((tx_q_idx >= 8) ||
       (pdbc >= _9BITS_MAX_VAL_) ||
       (pdsc >= _9BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_pdp = RU_FIELD_SET(0, GPON_TX_FIFO_CONFIGURATION, PDP, PDBC, reg_pdp, pdbc);
    reg_pdp = RU_FIELD_SET(0, GPON_TX_FIFO_CONFIGURATION, PDP, PDSC, reg_pdp, pdsc);

    RU_REG_RAM_WRITE(0, tx_q_idx, GPON_TX_FIFO_CONFIGURATION, PDP, reg_pdp);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_fifo_configuration_pdp_get(uint32_t tx_q_idx, uint16_t *pdbc, uint16_t *pdsc)
{
    uint32_t reg_pdp=0;

#ifdef VALIDATE_PARMS
    if(!pdbc || !pdsc)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
    if((tx_q_idx >= 8))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, tx_q_idx, GPON_TX_FIFO_CONFIGURATION, PDP, reg_pdp);

    *pdbc = RU_FIELD_GET(0, GPON_TX_FIFO_CONFIGURATION, PDP, PDBC, reg_pdp);
    *pdsc = RU_FIELD_GET(0, GPON_TX_FIFO_CONFIGURATION, PDP, PDSC, reg_pdp);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_fifo_configuration_pdpgrp_set(uint32_t tx_pd_idx, uint16_t pdbc_grp)
{
    uint32_t reg_pdpgrp=0;

#ifdef VALIDATE_PARMS
    if((tx_pd_idx >= 4) ||
       (pdbc_grp >= _9BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_pdpgrp = RU_FIELD_SET(0, GPON_TX_FIFO_CONFIGURATION, PDPGRP, PDBC_GRP, reg_pdpgrp, pdbc_grp);

    RU_REG_RAM_WRITE(0, tx_pd_idx, GPON_TX_FIFO_CONFIGURATION, PDPGRP, reg_pdpgrp);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_fifo_configuration_pdpgrp_get(uint32_t tx_pd_idx, uint16_t *pdbc_grp)
{
    uint32_t reg_pdpgrp=0;

#ifdef VALIDATE_PARMS
    if(!pdbc_grp)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
    if((tx_pd_idx >= 4))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, tx_pd_idx, GPON_TX_FIFO_CONFIGURATION, PDPGRP, reg_pdpgrp);

    *pdbc_grp = RU_FIELD_GET(0, GPON_TX_FIFO_CONFIGURATION, PDPGRP, PDBC_GRP, reg_pdpgrp);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_fifo_configuration_tqp_set(uint32_t tx_q_idx, uint16_t tqbc, uint16_t tqsc)
{
    uint32_t reg_tqp=0;

#ifdef VALIDATE_PARMS
    if((tx_q_idx >= 8) ||
       (tqbc >= _13BITS_MAX_VAL_) ||
       (tqsc >= _13BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_tqp = RU_FIELD_SET(0, GPON_TX_FIFO_CONFIGURATION, TQP, TQBC, reg_tqp, tqbc);
    reg_tqp = RU_FIELD_SET(0, GPON_TX_FIFO_CONFIGURATION, TQP, TQSC, reg_tqp, tqsc);

    RU_REG_RAM_WRITE(0, tx_q_idx, GPON_TX_FIFO_CONFIGURATION, TQP, reg_tqp);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_fifo_configuration_tqp_get(uint32_t tx_q_idx, uint16_t *tqbc, uint16_t *tqsc)
{
    uint32_t reg_tqp=0;

#ifdef VALIDATE_PARMS
    if(!tqbc || !tqsc)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
    if((tx_q_idx >= 8))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, tx_q_idx, GPON_TX_FIFO_CONFIGURATION, TQP, reg_tqp);

    *tqbc = RU_FIELD_GET(0, GPON_TX_FIFO_CONFIGURATION, TQP, TQBC, reg_tqp);
    *tqsc = RU_FIELD_GET(0, GPON_TX_FIFO_CONFIGURATION, TQP, TQSC, reg_tqp);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_fifo_configuration_tdqpgrp_set(uint32_t tx_q_idx, uint16_t tdqb, uint16_t tdqs)
{
    uint32_t reg_tdqpgrp=0;

#ifdef VALIDATE_PARMS
    if((tx_q_idx >= 4) ||
       (tdqb >= _13BITS_MAX_VAL_) ||
       (tdqs >= _13BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_tdqpgrp = RU_FIELD_SET(0, GPON_TX_FIFO_CONFIGURATION, TDQPGRP, TDQB, reg_tdqpgrp, tdqb);
    reg_tdqpgrp = RU_FIELD_SET(0, GPON_TX_FIFO_CONFIGURATION, TDQPGRP, TDQS, reg_tdqpgrp, tdqs);

    RU_REG_RAM_WRITE(0, tx_q_idx, GPON_TX_FIFO_CONFIGURATION, TDQPGRP, reg_tdqpgrp);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_fifo_configuration_tdqpgrp_get(uint32_t tx_q_idx, uint16_t *tdqb, uint16_t *tdqs)
{
    uint32_t reg_tdqpgrp=0;

#ifdef VALIDATE_PARMS
    if(!tdqb || !tdqs)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
    if((tx_q_idx >= 4))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, tx_q_idx, GPON_TX_FIFO_CONFIGURATION, TDQPGRP, reg_tdqpgrp);

    *tdqb = RU_FIELD_GET(0, GPON_TX_FIFO_CONFIGURATION, TDQPGRP, TDQB, reg_tdqpgrp);
    *tdqs = RU_FIELD_GET(0, GPON_TX_FIFO_CONFIGURATION, TDQPGRP, TDQS, reg_tdqpgrp);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_fifo_configuration_tdqp_set(uint16_t tdqpid, uint8_t tdq, uint8_t tdqpti)
{
    uint32_t reg_tdqp=0;

#ifdef VALIDATE_PARMS
    if((tdqpid >= _12BITS_MAX_VAL_) ||
       (tdq >= _6BITS_MAX_VAL_) ||
       (tdqpti >= _3BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_tdqp = RU_FIELD_SET(0, GPON_TX_FIFO_CONFIGURATION, TDQP, TDQPID, reg_tdqp, tdqpid);
    reg_tdqp = RU_FIELD_SET(0, GPON_TX_FIFO_CONFIGURATION, TDQP, TDQ, reg_tdqp, tdq);
    reg_tdqp = RU_FIELD_SET(0, GPON_TX_FIFO_CONFIGURATION, TDQP, TDQPTI, reg_tdqp, tdqpti);

    RU_REG_WRITE(0, GPON_TX_FIFO_CONFIGURATION, TDQP, reg_tdqp);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_fifo_configuration_tdqp_get(uint16_t *tdqpid, uint8_t *tdq, uint8_t *tdqpti)
{
    uint32_t reg_tdqp=0;

#ifdef VALIDATE_PARMS
    if(!tdqpid || !tdq || !tdqpti)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_FIFO_CONFIGURATION, TDQP, reg_tdqp);

    *tdqpid = RU_FIELD_GET(0, GPON_TX_FIFO_CONFIGURATION, TDQP, TDQPID, reg_tdqp);
    *tdq = RU_FIELD_GET(0, GPON_TX_FIFO_CONFIGURATION, TDQP, TDQ, reg_tdqp);
    *tdqpti = RU_FIELD_GET(0, GPON_TX_FIFO_CONFIGURATION, TDQP, TDQPTI, reg_tdqp);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_fifo_configuration_lbp_set(uint8_t lbqn)
{
    uint32_t reg_lbp=0;

#ifdef VALIDATE_PARMS
    if((lbqn >= _6BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_lbp = RU_FIELD_SET(0, GPON_TX_FIFO_CONFIGURATION, LBP, LBQN, reg_lbp, lbqn);

    RU_REG_WRITE(0, GPON_TX_FIFO_CONFIGURATION, LBP, reg_lbp);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_fifo_configuration_lbp_get(uint8_t *lbqn)
{
    uint32_t reg_lbp=0;

#ifdef VALIDATE_PARMS
    if(!lbqn)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_FIFO_CONFIGURATION, LBP, reg_lbp);

    *lbqn = RU_FIELD_GET(0, GPON_TX_FIFO_CONFIGURATION, LBP, LBQN, reg_lbp);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_fifo_configuration_tth_set(uint16_t tdmmin, uint16_t tdmmax, bdmf_boolean tthen)
{
    uint32_t reg_tth=0;

#ifdef VALIDATE_PARMS
    if((tdmmin >= _15BITS_MAX_VAL_) ||
       (tdmmax >= _15BITS_MAX_VAL_) ||
       (tthen >= _1BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_tth = RU_FIELD_SET(0, GPON_TX_FIFO_CONFIGURATION, TTH, TDMMIN, reg_tth, tdmmin);
    reg_tth = RU_FIELD_SET(0, GPON_TX_FIFO_CONFIGURATION, TTH, TDMMAX, reg_tth, tdmmax);
    reg_tth = RU_FIELD_SET(0, GPON_TX_FIFO_CONFIGURATION, TTH, TTHEN, reg_tth, tthen);

    RU_REG_WRITE(0, GPON_TX_FIFO_CONFIGURATION, TTH, reg_tth);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_fifo_configuration_tth_get(uint16_t *tdmmin, uint16_t *tdmmax, bdmf_boolean *tthen)
{
    uint32_t reg_tth=0;

#ifdef VALIDATE_PARMS
    if(!tdmmin || !tdmmax || !tthen)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_FIFO_CONFIGURATION, TTH, reg_tth);

    *tdmmin = RU_FIELD_GET(0, GPON_TX_FIFO_CONFIGURATION, TTH, TDMMIN, reg_tth);
    *tdmmax = RU_FIELD_GET(0, GPON_TX_FIFO_CONFIGURATION, TTH, TDMMAX, reg_tth);
    *tthen = RU_FIELD_GET(0, GPON_TX_FIFO_CONFIGURATION, TTH, TTHEN, reg_tth);

    return BDMF_ERR_OK;
}

#ifdef USE_BDMF_SHELL
enum
{
    BDMF_pdp,
    BDMF_pdpgrp,
    BDMF_tqp,
    BDMF_tdqpgrp,
    BDMF_tdqp,
    BDMF_lbp,
    BDMF_tth,
};

typedef enum
{
    bdmf_address_pdp,
    bdmf_address_pdpgrp,
    bdmf_address_tqp,
    bdmf_address_tdqpgrp,
    bdmf_address_tdqp,
    bdmf_address_lbp,
    bdmf_address_tth,
}
bdmf_address;

static int bcm_gpon_tx_fifo_configuration_cli_set(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_error_t err;

    switch(parm[0].value.unumber)
    {
    case BDMF_pdp:
        err = ag_drv_gpon_tx_fifo_configuration_pdp_set(parm[1].value.unumber, parm[2].value.unumber, parm[3].value.unumber);
        break;
    case BDMF_pdpgrp:
        err = ag_drv_gpon_tx_fifo_configuration_pdpgrp_set(parm[1].value.unumber, parm[2].value.unumber);
        break;
    case BDMF_tqp:
        err = ag_drv_gpon_tx_fifo_configuration_tqp_set(parm[1].value.unumber, parm[2].value.unumber, parm[3].value.unumber);
        break;
    case BDMF_tdqpgrp:
        err = ag_drv_gpon_tx_fifo_configuration_tdqpgrp_set(parm[1].value.unumber, parm[2].value.unumber, parm[3].value.unumber);
        break;
    case BDMF_tdqp:
        err = ag_drv_gpon_tx_fifo_configuration_tdqp_set(parm[1].value.unumber, parm[2].value.unumber, parm[3].value.unumber);
        break;
    case BDMF_lbp:
        err = ag_drv_gpon_tx_fifo_configuration_lbp_set(parm[1].value.unumber);
        break;
    case BDMF_tth:
        err = ag_drv_gpon_tx_fifo_configuration_tth_set(parm[1].value.unumber, parm[2].value.unumber, parm[3].value.unumber);
        break;
    default:
        err = BDMF_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

static int bcm_gpon_tx_fifo_configuration_cli_get(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_error_t err;

    switch(parm[0].value.unumber)
    {
    case BDMF_pdp:
    {
        uint16_t pdbc;
        uint16_t pdsc;
        err = ag_drv_gpon_tx_fifo_configuration_pdp_get(parm[1].value.unumber, &pdbc, &pdsc);
        bdmf_session_print(session, "pdbc = %u = 0x%x\n", pdbc, pdbc);
        bdmf_session_print(session, "pdsc = %u = 0x%x\n", pdsc, pdsc);
        break;
    }
    case BDMF_pdpgrp:
    {
        uint16_t pdbc_grp;
        err = ag_drv_gpon_tx_fifo_configuration_pdpgrp_get(parm[1].value.unumber, &pdbc_grp);
        bdmf_session_print(session, "pdbc_grp = %u = 0x%x\n", pdbc_grp, pdbc_grp);
        break;
    }
    case BDMF_tqp:
    {
        uint16_t tqbc;
        uint16_t tqsc;
        err = ag_drv_gpon_tx_fifo_configuration_tqp_get(parm[1].value.unumber, &tqbc, &tqsc);
        bdmf_session_print(session, "tqbc = %u = 0x%x\n", tqbc, tqbc);
        bdmf_session_print(session, "tqsc = %u = 0x%x\n", tqsc, tqsc);
        break;
    }
    case BDMF_tdqpgrp:
    {
        uint16_t tdqb;
        uint16_t tdqs;
        err = ag_drv_gpon_tx_fifo_configuration_tdqpgrp_get(parm[1].value.unumber, &tdqb, &tdqs);
        bdmf_session_print(session, "tdqb = %u = 0x%x\n", tdqb, tdqb);
        bdmf_session_print(session, "tdqs = %u = 0x%x\n", tdqs, tdqs);
        break;
    }
    case BDMF_tdqp:
    {
        uint16_t tdqpid;
        uint8_t tdq;
        uint8_t tdqpti;
        err = ag_drv_gpon_tx_fifo_configuration_tdqp_get(&tdqpid, &tdq, &tdqpti);
        bdmf_session_print(session, "tdqpid = %u = 0x%x\n", tdqpid, tdqpid);
        bdmf_session_print(session, "tdq = %u = 0x%x\n", tdq, tdq);
        bdmf_session_print(session, "tdqpti = %u = 0x%x\n", tdqpti, tdqpti);
        break;
    }
    case BDMF_lbp:
    {
        uint8_t lbqn;
        err = ag_drv_gpon_tx_fifo_configuration_lbp_get(&lbqn);
        bdmf_session_print(session, "lbqn = %u = 0x%x\n", lbqn, lbqn);
        break;
    }
    case BDMF_tth:
    {
        uint16_t tdmmin;
        uint16_t tdmmax;
        bdmf_boolean tthen;
        err = ag_drv_gpon_tx_fifo_configuration_tth_get(&tdmmin, &tdmmax, &tthen);
        bdmf_session_print(session, "tdmmin = %u = 0x%x\n", tdmmin, tdmmin);
        bdmf_session_print(session, "tdmmax = %u = 0x%x\n", tdmmax, tdmmax);
        bdmf_session_print(session, "tthen = %u = 0x%x\n", tthen, tthen);
        break;
    }
    default:
        err = BDMF_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

static int bcm_gpon_tx_fifo_configuration_cli_test(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_test_method m = parm[0].value.unumber;
    bdmf_error_t err = BDMF_ERR_OK;

    {
        uint32_t tx_q_idx=gtmv(m, 3);
        uint16_t pdbc=gtmv(m, 9);
        uint16_t pdsc=gtmv(m, 9);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_fifo_configuration_pdp_set( %u %u %u)\n", tx_q_idx, pdbc, pdsc);
        if(!err) ag_drv_gpon_tx_fifo_configuration_pdp_set(tx_q_idx, pdbc, pdsc);
        if(!err) ag_drv_gpon_tx_fifo_configuration_pdp_get( tx_q_idx, &pdbc, &pdsc);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_fifo_configuration_pdp_get( %u %u %u)\n", tx_q_idx, pdbc, pdsc);
        if(err || pdbc!=gtmv(m, 9) || pdsc!=gtmv(m, 9))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t tx_pd_idx=gtmv(m, 2);
        uint16_t pdbc_grp=gtmv(m, 9);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_fifo_configuration_pdpgrp_set( %u %u)\n", tx_pd_idx, pdbc_grp);
        if(!err) ag_drv_gpon_tx_fifo_configuration_pdpgrp_set(tx_pd_idx, pdbc_grp);
        if(!err) ag_drv_gpon_tx_fifo_configuration_pdpgrp_get( tx_pd_idx, &pdbc_grp);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_fifo_configuration_pdpgrp_get( %u %u)\n", tx_pd_idx, pdbc_grp);
        if(err || pdbc_grp!=gtmv(m, 9))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t tx_q_idx=gtmv(m, 3);
        uint16_t tqbc=gtmv(m, 13);
        uint16_t tqsc=gtmv(m, 13);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_fifo_configuration_tqp_set( %u %u %u)\n", tx_q_idx, tqbc, tqsc);
        if(!err) ag_drv_gpon_tx_fifo_configuration_tqp_set(tx_q_idx, tqbc, tqsc);
        if(!err) ag_drv_gpon_tx_fifo_configuration_tqp_get( tx_q_idx, &tqbc, &tqsc);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_fifo_configuration_tqp_get( %u %u %u)\n", tx_q_idx, tqbc, tqsc);
        if(err || tqbc!=gtmv(m, 13) || tqsc!=gtmv(m, 13))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t tx_q_idx=gtmv(m, 2);
        uint16_t tdqb=gtmv(m, 13);
        uint16_t tdqs=gtmv(m, 13);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_fifo_configuration_tdqpgrp_set( %u %u %u)\n", tx_q_idx, tdqb, tdqs);
        if(!err) ag_drv_gpon_tx_fifo_configuration_tdqpgrp_set(tx_q_idx, tdqb, tdqs);
        if(!err) ag_drv_gpon_tx_fifo_configuration_tdqpgrp_get( tx_q_idx, &tdqb, &tdqs);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_fifo_configuration_tdqpgrp_get( %u %u %u)\n", tx_q_idx, tdqb, tdqs);
        if(err || tdqb!=gtmv(m, 13) || tdqs!=gtmv(m, 13))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint16_t tdqpid=gtmv(m, 12);
        uint8_t tdq=gtmv(m, 6);
        uint8_t tdqpti=gtmv(m, 3);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_fifo_configuration_tdqp_set( %u %u %u)\n", tdqpid, tdq, tdqpti);
        if(!err) ag_drv_gpon_tx_fifo_configuration_tdqp_set(tdqpid, tdq, tdqpti);
        if(!err) ag_drv_gpon_tx_fifo_configuration_tdqp_get( &tdqpid, &tdq, &tdqpti);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_fifo_configuration_tdqp_get( %u %u %u)\n", tdqpid, tdq, tdqpti);
        if(err || tdqpid!=gtmv(m, 12) || tdq!=gtmv(m, 6) || tdqpti!=gtmv(m, 3))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint8_t lbqn=gtmv(m, 6);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_fifo_configuration_lbp_set( %u)\n", lbqn);
        if(!err) ag_drv_gpon_tx_fifo_configuration_lbp_set(lbqn);
        if(!err) ag_drv_gpon_tx_fifo_configuration_lbp_get( &lbqn);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_fifo_configuration_lbp_get( %u)\n", lbqn);
        if(err || lbqn!=gtmv(m, 6))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint16_t tdmmin=gtmv(m, 15);
        uint16_t tdmmax=gtmv(m, 15);
        bdmf_boolean tthen=gtmv(m, 1);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_fifo_configuration_tth_set( %u %u %u)\n", tdmmin, tdmmax, tthen);
        if(!err) ag_drv_gpon_tx_fifo_configuration_tth_set(tdmmin, tdmmax, tthen);
        if(!err) ag_drv_gpon_tx_fifo_configuration_tth_get( &tdmmin, &tdmmax, &tthen);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_fifo_configuration_tth_get( %u %u %u)\n", tdmmin, tdmmax, tthen);
        if(err || tdmmin!=gtmv(m, 15) || tdmmax!=gtmv(m, 15) || tthen!=gtmv(m, 1))
            return err ? err : BDMF_ERR_IO;
    }
    return err;
}

static int bcm_gpon_tx_fifo_configuration_cli_address(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
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
    case bdmf_address_pdp : reg = &RU_REG(GPON_TX_FIFO_CONFIGURATION, PDP); blk = &RU_BLK(GPON_TX_FIFO_CONFIGURATION); break;
    case bdmf_address_pdpgrp : reg = &RU_REG(GPON_TX_FIFO_CONFIGURATION, PDPGRP); blk = &RU_BLK(GPON_TX_FIFO_CONFIGURATION); break;
    case bdmf_address_tqp : reg = &RU_REG(GPON_TX_FIFO_CONFIGURATION, TQP); blk = &RU_BLK(GPON_TX_FIFO_CONFIGURATION); break;
    case bdmf_address_tdqpgrp : reg = &RU_REG(GPON_TX_FIFO_CONFIGURATION, TDQPGRP); blk = &RU_BLK(GPON_TX_FIFO_CONFIGURATION); break;
    case bdmf_address_tdqp : reg = &RU_REG(GPON_TX_FIFO_CONFIGURATION, TDQP); blk = &RU_BLK(GPON_TX_FIFO_CONFIGURATION); break;
    case bdmf_address_lbp : reg = &RU_REG(GPON_TX_FIFO_CONFIGURATION, LBP); blk = &RU_BLK(GPON_TX_FIFO_CONFIGURATION); break;
    case bdmf_address_tth : reg = &RU_REG(GPON_TX_FIFO_CONFIGURATION, TTH); blk = &RU_BLK(GPON_TX_FIFO_CONFIGURATION); break;
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

bdmfmon_handle_t ag_drv_gpon_tx_fifo_configuration_cli_init(bdmfmon_handle_t driver_dir)
{
    bdmfmon_handle_t dir;

    if ((dir = bdmfmon_dir_find(driver_dir, "gpon_tx_fifo_configuration"))!=NULL)
        return dir;
    dir = bdmfmon_dir_add(driver_dir, "gpon_tx_fifo_configuration", "gpon_tx_fifo_configuration", BDMF_ACCESS_ADMIN, NULL);

    {
        static bdmfmon_cmd_parm_t set_pdp[]={
            BDMFMON_MAKE_PARM("tx_q_idx", "tx_q_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("pdbc", "pdbc", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("pdsc", "pdsc", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_pdpgrp[]={
            BDMFMON_MAKE_PARM("tx_pd_idx", "tx_pd_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("pdbc_grp", "pdbc_grp", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_tqp[]={
            BDMFMON_MAKE_PARM("tx_q_idx", "tx_q_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("tqbc", "tqbc", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("tqsc", "tqsc", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_tdqpgrp[]={
            BDMFMON_MAKE_PARM("tx_q_idx", "tx_q_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("tdqb", "tdqb", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("tdqs", "tdqs", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_tdqp[]={
            BDMFMON_MAKE_PARM("tdqpid", "tdqpid", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("tdq", "tdq", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("tdqpti", "tdqpti", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_lbp[]={
            BDMFMON_MAKE_PARM("lbqn", "lbqn", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_tth[]={
            BDMFMON_MAKE_PARM("tdmmin", "tdmmin", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("tdmmax", "tdmmax", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("tthen", "tthen", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_enum_val_t selector_table[] = {
            { .name="pdp", .val=BDMF_pdp, .parms=set_pdp },
            { .name="pdpgrp", .val=BDMF_pdpgrp, .parms=set_pdpgrp },
            { .name="tqp", .val=BDMF_tqp, .parms=set_tqp },
            { .name="tdqpgrp", .val=BDMF_tdqpgrp, .parms=set_tdqpgrp },
            { .name="tdqp", .val=BDMF_tdqp, .parms=set_tdqp },
            { .name="lbp", .val=BDMF_lbp, .parms=set_lbp },
            { .name="tth", .val=BDMF_tth, .parms=set_tth },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "set", "set", bcm_gpon_tx_fifo_configuration_cli_set,
            BDMFMON_MAKE_PARM_SELECTOR("purpose", "purpose", selector_table, 0));
    }
    {
        static bdmfmon_cmd_parm_t set_default[]={
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_pdp[]={
            BDMFMON_MAKE_PARM("tx_q_idx", "tx_q_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_pdpgrp[]={
            BDMFMON_MAKE_PARM("tx_pd_idx", "tx_pd_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_tqp[]={
            BDMFMON_MAKE_PARM("tx_q_idx", "tx_q_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_tdqpgrp[]={
            BDMFMON_MAKE_PARM("tx_q_idx", "tx_q_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_enum_val_t selector_table[] = {
            { .name="pdp", .val=BDMF_pdp, .parms=set_pdp },
            { .name="pdpgrp", .val=BDMF_pdpgrp, .parms=set_pdpgrp },
            { .name="tqp", .val=BDMF_tqp, .parms=set_tqp },
            { .name="tdqpgrp", .val=BDMF_tdqpgrp, .parms=set_tdqpgrp },
            { .name="tdqp", .val=BDMF_tdqp, .parms=set_default },
            { .name="lbp", .val=BDMF_lbp, .parms=set_default },
            { .name="tth", .val=BDMF_tth, .parms=set_default },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "get", "get", bcm_gpon_tx_fifo_configuration_cli_get,
            BDMFMON_MAKE_PARM_SELECTOR("purpose", "purpose", selector_table, 0));
    }
    {
        static bdmfmon_enum_val_t enum_table_test_method[] = {
            { .name="low" , .val=bdmf_test_method_low },
            { .name="mid" , .val=bdmf_test_method_mid },
            { .name="high" , .val=bdmf_test_method_high },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "test", "test", bcm_gpon_tx_fifo_configuration_cli_test,
            BDMFMON_MAKE_PARM_ENUM("method", "low: 0000, mid: 1000, high: 1111", enum_table_test_method, 0));
    }
    {
        static bdmfmon_enum_val_t enum_table_address[] = {
            { .name="PDP" , .val=bdmf_address_pdp },
            { .name="PDPGRP" , .val=bdmf_address_pdpgrp },
            { .name="TQP" , .val=bdmf_address_tqp },
            { .name="TDQPGRP" , .val=bdmf_address_tdqpgrp },
            { .name="TDQP" , .val=bdmf_address_tdqp },
            { .name="LBP" , .val=bdmf_address_lbp },
            { .name="TTH" , .val=bdmf_address_tth },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "address", "address", bcm_gpon_tx_fifo_configuration_cli_address,
            BDMFMON_MAKE_PARM_ENUM("method", "method", enum_table_address, 0),
            BDMFMON_MAKE_PARM("index2", "onu_id/alloc_id/port_id/etc...", BDMFMON_PARM_NUMBER, BDMFMON_PARM_FLAG_OPTIONAL));
    }
    return dir;
}
#endif /* USE_BDMF_SHELL */


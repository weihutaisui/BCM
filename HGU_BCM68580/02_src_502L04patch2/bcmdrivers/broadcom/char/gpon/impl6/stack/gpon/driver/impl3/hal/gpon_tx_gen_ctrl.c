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
#include "gpon_tx_gen_ctrl.h"
bdmf_error_t ag_drv_gpon_tx_gen_ctrl_tdly_set(uint16_t bdly, uint8_t fdly, uint8_t sdly)
{
    uint32_t reg_tx_general_transmitter_control_tdly=0;

#ifdef VALIDATE_PARMS
    if((bdly >= _15BITS_MAX_VAL_) ||
       (fdly >= _3BITS_MAX_VAL_) ||
       (sdly >= _3BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_tx_general_transmitter_control_tdly = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDLY, BDLY, reg_tx_general_transmitter_control_tdly, bdly);
    reg_tx_general_transmitter_control_tdly = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDLY, FDLY, reg_tx_general_transmitter_control_tdly, fdly);
    reg_tx_general_transmitter_control_tdly = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDLY, SDLY, reg_tx_general_transmitter_control_tdly, sdly);

    RU_REG_WRITE(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDLY, reg_tx_general_transmitter_control_tdly);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_gen_ctrl_tdly_get(uint16_t *bdly, uint8_t *fdly, uint8_t *sdly)
{
    uint32_t reg_tx_general_transmitter_control_tdly=0;

#ifdef VALIDATE_PARMS
    if(!bdly || !fdly || !sdly)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDLY, reg_tx_general_transmitter_control_tdly);

    *bdly = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDLY, BDLY, reg_tx_general_transmitter_control_tdly);
    *fdly = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDLY, FDLY, reg_tx_general_transmitter_control_tdly);
    *sdly = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDLY, SDLY, reg_tx_general_transmitter_control_tdly);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_gen_ctrl_ten_set(const gpon_tx_gen_ctrl_ten *ten)
{
    uint32_t reg_tx_general_transmitter_control_ten=0;

#ifdef VALIDATE_PARMS
    if(!ten)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
    if((ten->txen >= _1BITS_MAX_VAL_) ||
       (ten->scren >= _1BITS_MAX_VAL_) ||
       (ten->bipen >= _1BITS_MAX_VAL_) ||
       (ten->fecen >= _1BITS_MAX_VAL_) ||
       (ten->loopben >= _1BITS_MAX_VAL_) ||
       (ten->plsen >= _1BITS_MAX_VAL_) ||
       (ten->tdmen >= _1BITS_MAX_VAL_) ||
       (ten->misctxen >= _1BITS_MAX_VAL_) ||
       (ten->rngen >= _1BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_tx_general_transmitter_control_ten = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TEN, TXEN, reg_tx_general_transmitter_control_ten, ten->txen);
    reg_tx_general_transmitter_control_ten = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TEN, SCREN, reg_tx_general_transmitter_control_ten, ten->scren);
    reg_tx_general_transmitter_control_ten = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TEN, BIPEN, reg_tx_general_transmitter_control_ten, ten->bipen);
    reg_tx_general_transmitter_control_ten = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TEN, FECEN, reg_tx_general_transmitter_control_ten, ten->fecen);
    reg_tx_general_transmitter_control_ten = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TEN, LOOPBEN, reg_tx_general_transmitter_control_ten, ten->loopben);
    reg_tx_general_transmitter_control_ten = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TEN, PLSEN, reg_tx_general_transmitter_control_ten, ten->plsen);
    reg_tx_general_transmitter_control_ten = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TEN, TDMEN, reg_tx_general_transmitter_control_ten, ten->tdmen);
    reg_tx_general_transmitter_control_ten = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TEN, MISCTXEN, reg_tx_general_transmitter_control_ten, ten->misctxen);
    reg_tx_general_transmitter_control_ten = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TEN, RNGEN, reg_tx_general_transmitter_control_ten, ten->rngen);

    RU_REG_WRITE(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TEN, reg_tx_general_transmitter_control_ten);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_gen_ctrl_ten_get(gpon_tx_gen_ctrl_ten *ten)
{
    uint32_t reg_tx_general_transmitter_control_ten=0;

#ifdef VALIDATE_PARMS
    if(!ten)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TEN, reg_tx_general_transmitter_control_ten);

    ten->txen = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TEN, TXEN, reg_tx_general_transmitter_control_ten);
    ten->scren = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TEN, SCREN, reg_tx_general_transmitter_control_ten);
    ten->bipen = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TEN, BIPEN, reg_tx_general_transmitter_control_ten);
    ten->fecen = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TEN, FECEN, reg_tx_general_transmitter_control_ten);
    ten->loopben = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TEN, LOOPBEN, reg_tx_general_transmitter_control_ten);
    ten->plsen = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TEN, PLSEN, reg_tx_general_transmitter_control_ten);
    ten->tdmen = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TEN, TDMEN, reg_tx_general_transmitter_control_ten);
    ten->misctxen = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TEN, MISCTXEN, reg_tx_general_transmitter_control_ten);
    ten->rngen = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TEN, RNGEN, reg_tx_general_transmitter_control_ten);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_pol_set(bdmf_boolean doutpol)
{
    uint32_t reg_tx_general_transmitter_control_pol=0;

#ifdef VALIDATE_PARMS
    if((doutpol >= _1BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_tx_general_transmitter_control_pol = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_POL, DOUTPOL, reg_tx_general_transmitter_control_pol, doutpol);

    RU_REG_WRITE(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_POL, reg_tx_general_transmitter_control_pol);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_pol_get(bdmf_boolean *doutpol)
{
    uint32_t reg_tx_general_transmitter_control_pol=0;

#ifdef VALIDATE_PARMS
    if(!doutpol)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_POL, reg_tx_general_transmitter_control_pol);

    *doutpol = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_POL, DOUTPOL, reg_tx_general_transmitter_control_pol);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_gen_ctrl_nprep_set(uint8_t prep0, uint8_t prep1, uint8_t prep2, uint8_t prep3)
{
    uint32_t reg_tx_general_transmitter_control_nprep=0;

#ifdef VALIDATE_PARMS
    if((prep0 >= _2BITS_MAX_VAL_) ||
       (prep1 >= _2BITS_MAX_VAL_) ||
       (prep2 >= _2BITS_MAX_VAL_) ||
       (prep3 >= _2BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_tx_general_transmitter_control_nprep = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_NPREP, PREP0, reg_tx_general_transmitter_control_nprep, prep0);
    reg_tx_general_transmitter_control_nprep = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_NPREP, PREP1, reg_tx_general_transmitter_control_nprep, prep1);
    reg_tx_general_transmitter_control_nprep = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_NPREP, PREP2, reg_tx_general_transmitter_control_nprep, prep2);
    reg_tx_general_transmitter_control_nprep = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_NPREP, PREP3, reg_tx_general_transmitter_control_nprep, prep3);

    RU_REG_WRITE(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_NPREP, reg_tx_general_transmitter_control_nprep);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_gen_ctrl_nprep_get(uint8_t *prep0, uint8_t *prep1, uint8_t *prep2, uint8_t *prep3)
{
    uint32_t reg_tx_general_transmitter_control_nprep=0;

#ifdef VALIDATE_PARMS
    if(!prep0 || !prep1 || !prep2 || !prep3)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_NPREP, reg_tx_general_transmitter_control_nprep);

    *prep0 = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_NPREP, PREP0, reg_tx_general_transmitter_control_nprep);
    *prep1 = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_NPREP, PREP1, reg_tx_general_transmitter_control_nprep);
    *prep2 = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_NPREP, PREP2, reg_tx_general_transmitter_control_nprep);
    *prep3 = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_NPREP, PREP3, reg_tx_general_transmitter_control_nprep);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_tpvld_set(uint8_t npvld, bdmf_boolean uplvd)
{
    uint32_t reg_tx_general_transmitter_control_tpvld=0;

#ifdef VALIDATE_PARMS
    if((npvld >= _4BITS_MAX_VAL_) ||
       (uplvd >= _1BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_tx_general_transmitter_control_tpvld = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TPVLD, NPVLD, reg_tx_general_transmitter_control_tpvld, npvld);
    reg_tx_general_transmitter_control_tpvld = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TPVLD, UPLVD, reg_tx_general_transmitter_control_tpvld, uplvd);

    RU_REG_WRITE(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TPVLD, reg_tx_general_transmitter_control_tpvld);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_tpvld_get(uint8_t *npvld, bdmf_boolean *uplvd)
{
    uint32_t reg_tx_general_transmitter_control_tpvld=0;

#ifdef VALIDATE_PARMS
    if(!npvld || !uplvd)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TPVLD, reg_tx_general_transmitter_control_tpvld);

    *npvld = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TPVLD, NPVLD, reg_tx_general_transmitter_control_tpvld);
    *uplvd = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TPVLD, UPLVD, reg_tx_general_transmitter_control_tpvld);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_gen_ctrl_tdbvld_set(uint32_t dbav)
{
    uint32_t reg_tx_general_transmitter_control_tdbvld=0;

#ifdef VALIDATE_PARMS
#endif

    reg_tx_general_transmitter_control_tdbvld = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDBVLD, DBAV, reg_tx_general_transmitter_control_tdbvld, dbav);

    RU_REG_WRITE(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDBVLD, reg_tx_general_transmitter_control_tdbvld);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_gen_ctrl_tdbvld_get(uint32_t *dbav)
{
    uint32_t reg_tx_general_transmitter_control_tdbvld=0;

#ifdef VALIDATE_PARMS
    if(!dbav)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDBVLD, reg_tx_general_transmitter_control_tdbvld);

    *dbav = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDBVLD, DBAV, reg_tx_general_transmitter_control_tdbvld);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_gen_ctrl_tdbconf_set(uint8_t dbav, bdmf_boolean dbr_gen_en, uint8_t divrate, bdmf_boolean dbflush)
{
    uint32_t reg_tx_general_transmitter_control_tdbconf=0;

#ifdef VALIDATE_PARMS
    if((dbr_gen_en >= _1BITS_MAX_VAL_) ||
       (divrate >= _2BITS_MAX_VAL_) ||
       (dbflush >= _1BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_tx_general_transmitter_control_tdbconf = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF, DBAV, reg_tx_general_transmitter_control_tdbconf, dbav);
    reg_tx_general_transmitter_control_tdbconf = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF, DBR_GEN_EN, reg_tx_general_transmitter_control_tdbconf, dbr_gen_en);
    reg_tx_general_transmitter_control_tdbconf = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF, DIVRATE, reg_tx_general_transmitter_control_tdbconf, divrate);
    reg_tx_general_transmitter_control_tdbconf = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF, DBFLUSH, reg_tx_general_transmitter_control_tdbconf, dbflush);

    RU_REG_WRITE(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF, reg_tx_general_transmitter_control_tdbconf);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_gen_ctrl_tdbconf_get(uint8_t *dbav, bdmf_boolean *dbr_gen_en, uint8_t *divrate, bdmf_boolean *dbflush)
{
    uint32_t reg_tx_general_transmitter_control_tdbconf=0;

#ifdef VALIDATE_PARMS
    if(!dbav || !dbr_gen_en || !divrate || !dbflush)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF, reg_tx_general_transmitter_control_tdbconf);

    *dbav = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF, DBAV, reg_tx_general_transmitter_control_tdbconf);
    *dbr_gen_en = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF, DBR_GEN_EN, reg_tx_general_transmitter_control_tdbconf);
    *divrate = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF, DIVRATE, reg_tx_general_transmitter_control_tdbconf);
    *dbflush = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF, DBFLUSH, reg_tx_general_transmitter_control_tdbconf);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_gen_ctrl_tpclr_set(bdmf_boolean tpclrc)
{
    uint32_t reg_tx_general_transmitter_control_tpclr=0;

#ifdef VALIDATE_PARMS
    if((tpclrc >= _1BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_tx_general_transmitter_control_tpclr = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TPCLR, TPCLRC, reg_tx_general_transmitter_control_tpclr, tpclrc);

    RU_REG_WRITE(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TPCLR, reg_tx_general_transmitter_control_tpclr);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_gen_ctrl_tpclr_get(bdmf_boolean *tpclrc)
{
    uint32_t reg_tx_general_transmitter_control_tpclr=0;

#ifdef VALIDATE_PARMS
    if(!tpclrc)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TPCLR, reg_tx_general_transmitter_control_tpclr);

    *tpclrc = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TPCLR, TPCLRC, reg_tx_general_transmitter_control_tpclr);

    return BDMF_ERR_OK;
}

#ifdef USE_BDMF_SHELL
enum
{
    BDMF_tdly,
    BDMF_ten,
    BDMF_tx_general_transmitter_control_pol,
    BDMF_nprep,
    BDMF_tx_general_transmitter_control_tpvld,
    BDMF_tdbvld,
    BDMF_tdbconf,
    BDMF_tpclr,
};

typedef enum
{
    bdmf_address_tx_general_transmitter_control_tdly,
    bdmf_address_tx_general_transmitter_control_ten,
    bdmf_address_tx_general_transmitter_control_pol,
    bdmf_address_tx_general_transmitter_control_nprep,
    bdmf_address_tx_general_transmitter_control_tpvld,
    bdmf_address_tx_general_transmitter_control_tdbvld,
    bdmf_address_tx_general_transmitter_control_tdbconf,
    bdmf_address_tx_general_transmitter_control_tpclr,
}
bdmf_address;

static int bcm_gpon_tx_gen_ctrl_cli_set(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_error_t err;

    switch(parm[0].value.unumber)
    {
    case BDMF_tdly:
        err = ag_drv_gpon_tx_gen_ctrl_tdly_set(parm[1].value.unumber, parm[2].value.unumber, parm[3].value.unumber);
        break;
    case BDMF_ten:
    {
        gpon_tx_gen_ctrl_ten ten = { .txen=parm[1].value.unumber, .scren=parm[2].value.unumber, .bipen=parm[3].value.unumber, .fecen=parm[4].value.unumber, .loopben=parm[5].value.unumber, .plsen=parm[6].value.unumber, .tdmen=parm[7].value.unumber, .misctxen=parm[8].value.unumber, .rngen=parm[9].value.unumber};
        err = ag_drv_gpon_tx_gen_ctrl_ten_set(&ten);
        break;
    }
    case BDMF_tx_general_transmitter_control_pol:
        err = ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_pol_set(parm[1].value.unumber);
        break;
    case BDMF_nprep:
        err = ag_drv_gpon_tx_gen_ctrl_nprep_set(parm[1].value.unumber, parm[2].value.unumber, parm[3].value.unumber, parm[4].value.unumber);
        break;
    case BDMF_tx_general_transmitter_control_tpvld:
        err = ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_tpvld_set(parm[1].value.unumber, parm[2].value.unumber);
        break;
    case BDMF_tdbvld:
        err = ag_drv_gpon_tx_gen_ctrl_tdbvld_set(parm[1].value.unumber);
        break;
    case BDMF_tdbconf:
        err = ag_drv_gpon_tx_gen_ctrl_tdbconf_set(parm[1].value.unumber, parm[2].value.unumber, parm[3].value.unumber, parm[4].value.unumber);
        break;
    case BDMF_tpclr:
        err = ag_drv_gpon_tx_gen_ctrl_tpclr_set(parm[1].value.unumber);
        break;
    default:
        err = BDMF_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

static int bcm_gpon_tx_gen_ctrl_cli_get(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_error_t err;

    switch(parm[0].value.unumber)
    {
    case BDMF_tdly:
    {
        uint16_t bdly;
        uint8_t fdly;
        uint8_t sdly;
        err = ag_drv_gpon_tx_gen_ctrl_tdly_get(&bdly, &fdly, &sdly);
        bdmf_session_print(session, "bdly = %u = 0x%x\n", bdly, bdly);
        bdmf_session_print(session, "fdly = %u = 0x%x\n", fdly, fdly);
        bdmf_session_print(session, "sdly = %u = 0x%x\n", sdly, sdly);
        break;
    }
    case BDMF_ten:
    {
        gpon_tx_gen_ctrl_ten ten;
        err = ag_drv_gpon_tx_gen_ctrl_ten_get(&ten);
        bdmf_session_print(session, "txen = %u = 0x%x\n", ten.txen, ten.txen);
        bdmf_session_print(session, "scren = %u = 0x%x\n", ten.scren, ten.scren);
        bdmf_session_print(session, "bipen = %u = 0x%x\n", ten.bipen, ten.bipen);
        bdmf_session_print(session, "fecen = %u = 0x%x\n", ten.fecen, ten.fecen);
        bdmf_session_print(session, "loopben = %u = 0x%x\n", ten.loopben, ten.loopben);
        bdmf_session_print(session, "plsen = %u = 0x%x\n", ten.plsen, ten.plsen);
        bdmf_session_print(session, "tdmen = %u = 0x%x\n", ten.tdmen, ten.tdmen);
        bdmf_session_print(session, "misctxen = %u = 0x%x\n", ten.misctxen, ten.misctxen);
        bdmf_session_print(session, "rngen = %u = 0x%x\n", ten.rngen, ten.rngen);
        break;
    }
    case BDMF_tx_general_transmitter_control_pol:
    {
        bdmf_boolean doutpol;
        err = ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_pol_get(&doutpol);
        bdmf_session_print(session, "doutpol = %u = 0x%x\n", doutpol, doutpol);
        break;
    }
    case BDMF_nprep:
    {
        uint8_t prep0;
        uint8_t prep1;
        uint8_t prep2;
        uint8_t prep3;
        err = ag_drv_gpon_tx_gen_ctrl_nprep_get(&prep0, &prep1, &prep2, &prep3);
        bdmf_session_print(session, "prep0 = %u = 0x%x\n", prep0, prep0);
        bdmf_session_print(session, "prep1 = %u = 0x%x\n", prep1, prep1);
        bdmf_session_print(session, "prep2 = %u = 0x%x\n", prep2, prep2);
        bdmf_session_print(session, "prep3 = %u = 0x%x\n", prep3, prep3);
        break;
    }
    case BDMF_tx_general_transmitter_control_tpvld:
    {
        uint8_t npvld;
        bdmf_boolean uplvd;
        err = ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_tpvld_get(&npvld, &uplvd);
        bdmf_session_print(session, "npvld = %u = 0x%x\n", npvld, npvld);
        bdmf_session_print(session, "uplvd = %u = 0x%x\n", uplvd, uplvd);
        break;
    }
    case BDMF_tdbvld:
    {
        uint32_t dbav;
        err = ag_drv_gpon_tx_gen_ctrl_tdbvld_get(&dbav);
        bdmf_session_print(session, "dbav = %u = 0x%x\n", dbav, dbav);
        break;
    }
    case BDMF_tdbconf:
    {
        uint8_t dbav;
        bdmf_boolean dbr_gen_en;
        uint8_t divrate;
        bdmf_boolean dbflush;
        err = ag_drv_gpon_tx_gen_ctrl_tdbconf_get(&dbav, &dbr_gen_en, &divrate, &dbflush);
        bdmf_session_print(session, "dbav = %u = 0x%x\n", dbav, dbav);
        bdmf_session_print(session, "dbr_gen_en = %u = 0x%x\n", dbr_gen_en, dbr_gen_en);
        bdmf_session_print(session, "divrate = %u = 0x%x\n", divrate, divrate);
        bdmf_session_print(session, "dbflush = %u = 0x%x\n", dbflush, dbflush);
        break;
    }
    case BDMF_tpclr:
    {
        bdmf_boolean tpclrc;
        err = ag_drv_gpon_tx_gen_ctrl_tpclr_get(&tpclrc);
        bdmf_session_print(session, "tpclrc = %u = 0x%x\n", tpclrc, tpclrc);
        break;
    }
    default:
        err = BDMF_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

static int bcm_gpon_tx_gen_ctrl_cli_test(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_test_method m = parm[0].value.unumber;
    bdmf_error_t err = BDMF_ERR_OK;

    {
        uint16_t bdly=gtmv(m, 15);
        uint8_t fdly=gtmv(m, 3);
        uint8_t sdly=gtmv(m, 3);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_gen_ctrl_tdly_set( %u %u %u)\n", bdly, fdly, sdly);
        if(!err) ag_drv_gpon_tx_gen_ctrl_tdly_set(bdly, fdly, sdly);
        if(!err) ag_drv_gpon_tx_gen_ctrl_tdly_get( &bdly, &fdly, &sdly);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_gen_ctrl_tdly_get( %u %u %u)\n", bdly, fdly, sdly);
        if(err || bdly!=gtmv(m, 15) || fdly!=gtmv(m, 3) || sdly!=gtmv(m, 3))
            return err ? err : BDMF_ERR_IO;
    }
    {
        gpon_tx_gen_ctrl_ten ten = {.txen=gtmv(m, 1), .scren=gtmv(m, 1), .bipen=gtmv(m, 1), .fecen=gtmv(m, 1), .loopben=gtmv(m, 1), .plsen=gtmv(m, 1), .tdmen=gtmv(m, 1), .misctxen=gtmv(m, 1), .rngen=gtmv(m, 1)};
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_gen_ctrl_ten_set( %u %u %u %u %u %u %u %u %u)\n", ten.txen, ten.scren, ten.bipen, ten.fecen, ten.loopben, ten.plsen, ten.tdmen, ten.misctxen, ten.rngen);
        if(!err) ag_drv_gpon_tx_gen_ctrl_ten_set(&ten);
        if(!err) ag_drv_gpon_tx_gen_ctrl_ten_get( &ten);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_gen_ctrl_ten_get( %u %u %u %u %u %u %u %u %u)\n", ten.txen, ten.scren, ten.bipen, ten.fecen, ten.loopben, ten.plsen, ten.tdmen, ten.misctxen, ten.rngen);
        if(err || ten.txen!=gtmv(m, 1) || ten.scren!=gtmv(m, 1) || ten.bipen!=gtmv(m, 1) || ten.fecen!=gtmv(m, 1) || ten.loopben!=gtmv(m, 1) || ten.plsen!=gtmv(m, 1) || ten.tdmen!=gtmv(m, 1) || ten.misctxen!=gtmv(m, 1) || ten.rngen!=gtmv(m, 1))
            return err ? err : BDMF_ERR_IO;
    }
    {
        bdmf_boolean doutpol=gtmv(m, 1);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_pol_set( %u)\n", doutpol);
        if(!err) ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_pol_set(doutpol);
        if(!err) ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_pol_get( &doutpol);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_pol_get( %u)\n", doutpol);
        if(err || doutpol!=gtmv(m, 1))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint8_t prep0=gtmv(m, 2);
        uint8_t prep1=gtmv(m, 2);
        uint8_t prep2=gtmv(m, 2);
        uint8_t prep3=gtmv(m, 2);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_gen_ctrl_nprep_set( %u %u %u %u)\n", prep0, prep1, prep2, prep3);
        if(!err) ag_drv_gpon_tx_gen_ctrl_nprep_set(prep0, prep1, prep2, prep3);
        if(!err) ag_drv_gpon_tx_gen_ctrl_nprep_get( &prep0, &prep1, &prep2, &prep3);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_gen_ctrl_nprep_get( %u %u %u %u)\n", prep0, prep1, prep2, prep3);
        if(err || prep0!=gtmv(m, 2) || prep1!=gtmv(m, 2) || prep2!=gtmv(m, 2) || prep3!=gtmv(m, 2))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint8_t npvld=gtmv(m, 4);
        bdmf_boolean uplvd=gtmv(m, 1);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_tpvld_set( %u %u)\n", npvld, uplvd);
        if(!err) ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_tpvld_set(npvld, uplvd);
        if(!err) ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_tpvld_get( &npvld, &uplvd);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_tpvld_get( %u %u)\n", npvld, uplvd);
        if(err || npvld!=gtmv(m, 4) || uplvd!=gtmv(m, 1))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t dbav=gtmv(m, 32);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_gen_ctrl_tdbvld_set( %u)\n", dbav);
        if(!err) ag_drv_gpon_tx_gen_ctrl_tdbvld_set(dbav);
        if(!err) ag_drv_gpon_tx_gen_ctrl_tdbvld_get( &dbav);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_gen_ctrl_tdbvld_get( %u)\n", dbav);
        if(err || dbav!=gtmv(m, 32))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint8_t dbav=gtmv(m, 8);
        bdmf_boolean dbr_gen_en=gtmv(m, 1);
        uint8_t divrate=gtmv(m, 2);
        bdmf_boolean dbflush=gtmv(m, 1);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_gen_ctrl_tdbconf_set( %u %u %u %u)\n", dbav, dbr_gen_en, divrate, dbflush);
        if(!err) ag_drv_gpon_tx_gen_ctrl_tdbconf_set(dbav, dbr_gen_en, divrate, dbflush);
        if(!err) ag_drv_gpon_tx_gen_ctrl_tdbconf_get( &dbav, &dbr_gen_en, &divrate, &dbflush);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_gen_ctrl_tdbconf_get( %u %u %u %u)\n", dbav, dbr_gen_en, divrate, dbflush);
        if(err || dbav!=gtmv(m, 8) || dbr_gen_en!=gtmv(m, 1) || divrate!=gtmv(m, 2) || dbflush!=gtmv(m, 1))
            return err ? err : BDMF_ERR_IO;
    }
    {
        bdmf_boolean tpclrc=gtmv(m, 1);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_gen_ctrl_tpclr_set( %u)\n", tpclrc);
        if(!err) ag_drv_gpon_tx_gen_ctrl_tpclr_set(tpclrc);
        if(!err) ag_drv_gpon_tx_gen_ctrl_tpclr_get( &tpclrc);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_gen_ctrl_tpclr_get( %u)\n", tpclrc);
        if(err || tpclrc!=gtmv(m, 1))
            return err ? err : BDMF_ERR_IO;
    }
    return err;
}

static int bcm_gpon_tx_gen_ctrl_cli_address(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
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
    case bdmf_address_tx_general_transmitter_control_tdly : reg = &RU_REG(GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDLY); blk = &RU_BLK(GPON_TX_GEN_CTRL); break;
    case bdmf_address_tx_general_transmitter_control_ten : reg = &RU_REG(GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TEN); blk = &RU_BLK(GPON_TX_GEN_CTRL); break;
    case bdmf_address_tx_general_transmitter_control_pol : reg = &RU_REG(GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_POL); blk = &RU_BLK(GPON_TX_GEN_CTRL); break;
    case bdmf_address_tx_general_transmitter_control_nprep : reg = &RU_REG(GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_NPREP); blk = &RU_BLK(GPON_TX_GEN_CTRL); break;
    case bdmf_address_tx_general_transmitter_control_tpvld : reg = &RU_REG(GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TPVLD); blk = &RU_BLK(GPON_TX_GEN_CTRL); break;
    case bdmf_address_tx_general_transmitter_control_tdbvld : reg = &RU_REG(GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDBVLD); blk = &RU_BLK(GPON_TX_GEN_CTRL); break;
    case bdmf_address_tx_general_transmitter_control_tdbconf : reg = &RU_REG(GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF); blk = &RU_BLK(GPON_TX_GEN_CTRL); break;
    case bdmf_address_tx_general_transmitter_control_tpclr : reg = &RU_REG(GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TPCLR); blk = &RU_BLK(GPON_TX_GEN_CTRL); break;
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

bdmfmon_handle_t ag_drv_gpon_tx_gen_ctrl_cli_init(bdmfmon_handle_t driver_dir)
{
    bdmfmon_handle_t dir;

    if ((dir = bdmfmon_dir_find(driver_dir, "gpon_tx_gen_ctrl"))!=NULL)
        return dir;
    dir = bdmfmon_dir_add(driver_dir, "gpon_tx_gen_ctrl", "gpon_tx_gen_ctrl", BDMF_ACCESS_ADMIN, NULL);

    {
        static bdmfmon_cmd_parm_t set_tdly[]={
            BDMFMON_MAKE_PARM("bdly", "bdly", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("fdly", "fdly", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("sdly", "sdly", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_ten[]={
            BDMFMON_MAKE_PARM("txen", "txen", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("scren", "scren", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("bipen", "bipen", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("fecen", "fecen", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("loopben", "loopben", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("plsen", "plsen", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("tdmen", "tdmen", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("misctxen", "misctxen", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("rngen", "rngen", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_tx_general_transmitter_control_pol[]={
            BDMFMON_MAKE_PARM("doutpol", "doutpol", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_nprep[]={
            BDMFMON_MAKE_PARM("prep0", "prep0", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("prep1", "prep1", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("prep2", "prep2", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("prep3", "prep3", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_tx_general_transmitter_control_tpvld[]={
            BDMFMON_MAKE_PARM("npvld", "npvld", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("uplvd", "uplvd", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_tdbvld[]={
            BDMFMON_MAKE_PARM("dbav", "dbav", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_tdbconf[]={
            BDMFMON_MAKE_PARM("dbav", "dbav", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("dbr_gen_en", "dbr_gen_en", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("divrate", "divrate", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("dbflush", "dbflush", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_tpclr[]={
            BDMFMON_MAKE_PARM("tpclrc", "tpclrc", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_enum_val_t selector_table[] = {
            { .name="tdly", .val=BDMF_tdly, .parms=set_tdly },
            { .name="ten", .val=BDMF_ten, .parms=set_ten },
            { .name="tx_general_transmitter_control_pol", .val=BDMF_tx_general_transmitter_control_pol, .parms=set_tx_general_transmitter_control_pol },
            { .name="nprep", .val=BDMF_nprep, .parms=set_nprep },
            { .name="tx_general_transmitter_control_tpvld", .val=BDMF_tx_general_transmitter_control_tpvld, .parms=set_tx_general_transmitter_control_tpvld },
            { .name="tdbvld", .val=BDMF_tdbvld, .parms=set_tdbvld },
            { .name="tdbconf", .val=BDMF_tdbconf, .parms=set_tdbconf },
            { .name="tpclr", .val=BDMF_tpclr, .parms=set_tpclr },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "set", "set", bcm_gpon_tx_gen_ctrl_cli_set,
            BDMFMON_MAKE_PARM_SELECTOR("purpose", "purpose", selector_table, 0));
    }
    {
        static bdmfmon_cmd_parm_t set_default[]={
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_enum_val_t selector_table[] = {
            { .name="tdly", .val=BDMF_tdly, .parms=set_default },
            { .name="ten", .val=BDMF_ten, .parms=set_default },
            { .name="tx_general_transmitter_control_pol", .val=BDMF_tx_general_transmitter_control_pol, .parms=set_default },
            { .name="nprep", .val=BDMF_nprep, .parms=set_default },
            { .name="tx_general_transmitter_control_tpvld", .val=BDMF_tx_general_transmitter_control_tpvld, .parms=set_default },
            { .name="tdbvld", .val=BDMF_tdbvld, .parms=set_default },
            { .name="tdbconf", .val=BDMF_tdbconf, .parms=set_default },
            { .name="tpclr", .val=BDMF_tpclr, .parms=set_default },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "get", "get", bcm_gpon_tx_gen_ctrl_cli_get,
            BDMFMON_MAKE_PARM_SELECTOR("purpose", "purpose", selector_table, 0));
    }
    {
        static bdmfmon_enum_val_t enum_table_test_method[] = {
            { .name="low" , .val=bdmf_test_method_low },
            { .name="mid" , .val=bdmf_test_method_mid },
            { .name="high" , .val=bdmf_test_method_high },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "test", "test", bcm_gpon_tx_gen_ctrl_cli_test,
            BDMFMON_MAKE_PARM_ENUM("method", "low: 0000, mid: 1000, high: 1111", enum_table_test_method, 0));
    }
    {
        static bdmfmon_enum_val_t enum_table_address[] = {
            { .name="TX_GENERAL_TRANSMITTER_CONTROL_TDLY" , .val=bdmf_address_tx_general_transmitter_control_tdly },
            { .name="TX_GENERAL_TRANSMITTER_CONTROL_TEN" , .val=bdmf_address_tx_general_transmitter_control_ten },
            { .name="TX_GENERAL_TRANSMITTER_CONTROL_POL" , .val=bdmf_address_tx_general_transmitter_control_pol },
            { .name="TX_GENERAL_TRANSMITTER_CONTROL_NPREP" , .val=bdmf_address_tx_general_transmitter_control_nprep },
            { .name="TX_GENERAL_TRANSMITTER_CONTROL_TPVLD" , .val=bdmf_address_tx_general_transmitter_control_tpvld },
            { .name="TX_GENERAL_TRANSMITTER_CONTROL_TDBVLD" , .val=bdmf_address_tx_general_transmitter_control_tdbvld },
            { .name="TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF" , .val=bdmf_address_tx_general_transmitter_control_tdbconf },
            { .name="TX_GENERAL_TRANSMITTER_CONTROL_TPCLR" , .val=bdmf_address_tx_general_transmitter_control_tpclr },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "address", "address", bcm_gpon_tx_gen_ctrl_cli_address,
            BDMFMON_MAKE_PARM_ENUM("method", "method", enum_table_address, 0),
            BDMFMON_MAKE_PARM("index2", "onu_id/alloc_id/port_id/etc...", BDMFMON_PARM_NUMBER, BDMFMON_PARM_FLAG_OPTIONAL));
    }
    return dir;
}
#endif /* USE_BDMF_SHELL */


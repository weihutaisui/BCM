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
#include "gpon_tx_gen_ctrl.h"
int ag_drv_gpon_tx_gen_ctrl_tdly_set(uint16_t bdly, uint8_t fdly, uint8_t sdly)
{
    uint32_t reg_tx_general_transmitter_control_tdly=0;

#ifdef VALIDATE_PARMS
    if((bdly >= _15BITS_MAX_VAL_) ||
       (fdly >= _3BITS_MAX_VAL_) ||
       (sdly >= _3BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_tx_general_transmitter_control_tdly = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDLY, BDLY, reg_tx_general_transmitter_control_tdly, bdly);
    reg_tx_general_transmitter_control_tdly = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDLY, FDLY, reg_tx_general_transmitter_control_tdly, fdly);
    reg_tx_general_transmitter_control_tdly = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDLY, SDLY, reg_tx_general_transmitter_control_tdly, sdly);

    RU_REG_WRITE(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDLY, reg_tx_general_transmitter_control_tdly);

    return 0;
}

int ag_drv_gpon_tx_gen_ctrl_tdly_get(uint16_t *bdly, uint8_t *fdly, uint8_t *sdly)
{
    uint32_t reg_tx_general_transmitter_control_tdly=0;

#ifdef VALIDATE_PARMS
    if(!bdly || !fdly || !sdly)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDLY, reg_tx_general_transmitter_control_tdly);

    *bdly = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDLY, BDLY, reg_tx_general_transmitter_control_tdly);
    *fdly = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDLY, FDLY, reg_tx_general_transmitter_control_tdly);
    *sdly = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDLY, SDLY, reg_tx_general_transmitter_control_tdly);

    return 0;
}

int ag_drv_gpon_tx_gen_ctrl_ten_set(const gpon_tx_gen_ctrl_ten *ten)
{
    uint32_t reg_tx_general_transmitter_control_ten=0;

#ifdef VALIDATE_PARMS
    if(!ten)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, 1);
        return 1;
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
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
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

    return 0;
}

int ag_drv_gpon_tx_gen_ctrl_ten_get(gpon_tx_gen_ctrl_ten *ten)
{
    uint32_t reg_tx_general_transmitter_control_ten=0;

#ifdef VALIDATE_PARMS
    if(!ten)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
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

    return 0;
}

int ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_pol_set(uint8_t doutpol)
{
    uint32_t reg_tx_general_transmitter_control_pol=0;

#ifdef VALIDATE_PARMS
    if((doutpol >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_tx_general_transmitter_control_pol = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_POL, DOUTPOL, reg_tx_general_transmitter_control_pol, doutpol);

    RU_REG_WRITE(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_POL, reg_tx_general_transmitter_control_pol);

    return 0;
}

int ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_pol_get(uint8_t *doutpol)
{
    uint32_t reg_tx_general_transmitter_control_pol=0;

#ifdef VALIDATE_PARMS
    if(!doutpol)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_POL, reg_tx_general_transmitter_control_pol);

    *doutpol = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_POL, DOUTPOL, reg_tx_general_transmitter_control_pol);

    return 0;
}

int ag_drv_gpon_tx_gen_ctrl_nprep_set(uint8_t prep0, uint8_t prep1, uint8_t prep2, uint8_t prep3)
{
    uint32_t reg_tx_general_transmitter_control_nprep=0;

#ifdef VALIDATE_PARMS
    if((prep0 >= _2BITS_MAX_VAL_) ||
       (prep1 >= _2BITS_MAX_VAL_) ||
       (prep2 >= _2BITS_MAX_VAL_) ||
       (prep3 >= _2BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_tx_general_transmitter_control_nprep = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_NPREP, PREP0, reg_tx_general_transmitter_control_nprep, prep0);
    reg_tx_general_transmitter_control_nprep = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_NPREP, PREP1, reg_tx_general_transmitter_control_nprep, prep1);
    reg_tx_general_transmitter_control_nprep = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_NPREP, PREP2, reg_tx_general_transmitter_control_nprep, prep2);
    reg_tx_general_transmitter_control_nprep = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_NPREP, PREP3, reg_tx_general_transmitter_control_nprep, prep3);

    RU_REG_WRITE(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_NPREP, reg_tx_general_transmitter_control_nprep);

    return 0;
}

int ag_drv_gpon_tx_gen_ctrl_nprep_get(uint8_t *prep0, uint8_t *prep1, uint8_t *prep2, uint8_t *prep3)
{
    uint32_t reg_tx_general_transmitter_control_nprep=0;

#ifdef VALIDATE_PARMS
    if(!prep0 || !prep1 || !prep2 || !prep3)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_NPREP, reg_tx_general_transmitter_control_nprep);

    *prep0 = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_NPREP, PREP0, reg_tx_general_transmitter_control_nprep);
    *prep1 = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_NPREP, PREP1, reg_tx_general_transmitter_control_nprep);
    *prep2 = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_NPREP, PREP2, reg_tx_general_transmitter_control_nprep);
    *prep3 = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_NPREP, PREP3, reg_tx_general_transmitter_control_nprep);

    return 0;
}

int ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_tpvld_set(uint8_t npvld, uint8_t uplvd)
{
    uint32_t reg_tx_general_transmitter_control_tpvld=0;

#ifdef VALIDATE_PARMS
    if((npvld >= _4BITS_MAX_VAL_) ||
       (uplvd >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_tx_general_transmitter_control_tpvld = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TPVLD, NPVLD, reg_tx_general_transmitter_control_tpvld, npvld);
    reg_tx_general_transmitter_control_tpvld = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TPVLD, UPLVD, reg_tx_general_transmitter_control_tpvld, uplvd);

    RU_REG_WRITE(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TPVLD, reg_tx_general_transmitter_control_tpvld);

    return 0;
}

int ag_drv_gpon_tx_gen_ctrl_tx_general_transmitter_control_tpvld_get(uint8_t *npvld, uint8_t *uplvd)
{
    uint32_t reg_tx_general_transmitter_control_tpvld=0;

#ifdef VALIDATE_PARMS
    if(!npvld || !uplvd)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TPVLD, reg_tx_general_transmitter_control_tpvld);

    *npvld = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TPVLD, NPVLD, reg_tx_general_transmitter_control_tpvld);
    *uplvd = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TPVLD, UPLVD, reg_tx_general_transmitter_control_tpvld);

    return 0;
}

int ag_drv_gpon_tx_gen_ctrl_tdbvld_set(uint32_t dbav)
{
    uint32_t reg_tx_general_transmitter_control_tdbvld=0;

#ifdef VALIDATE_PARMS
#endif

    reg_tx_general_transmitter_control_tdbvld = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDBVLD, DBAV, reg_tx_general_transmitter_control_tdbvld, dbav);

    RU_REG_WRITE(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDBVLD, reg_tx_general_transmitter_control_tdbvld);

    return 0;
}

int ag_drv_gpon_tx_gen_ctrl_tdbvld_get(uint32_t *dbav)
{
    uint32_t reg_tx_general_transmitter_control_tdbvld=0;

#ifdef VALIDATE_PARMS
    if(!dbav)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDBVLD, reg_tx_general_transmitter_control_tdbvld);

    *dbav = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDBVLD, DBAV, reg_tx_general_transmitter_control_tdbvld);

    return 0;
}

int ag_drv_gpon_tx_gen_ctrl_tdbconf_set(uint8_t dbav, uint8_t dbr_gen_en, uint8_t divrate, uint8_t dbflush)
{
    uint32_t reg_tx_general_transmitter_control_tdbconf=0;

#ifdef VALIDATE_PARMS
    if((dbr_gen_en >= _1BITS_MAX_VAL_) ||
       (divrate >= _2BITS_MAX_VAL_) ||
       (dbflush >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_tx_general_transmitter_control_tdbconf = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF, DBAV, reg_tx_general_transmitter_control_tdbconf, dbav);
    reg_tx_general_transmitter_control_tdbconf = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF, DBR_GEN_EN, reg_tx_general_transmitter_control_tdbconf, dbr_gen_en);
    reg_tx_general_transmitter_control_tdbconf = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF, DIVRATE, reg_tx_general_transmitter_control_tdbconf, divrate);
    reg_tx_general_transmitter_control_tdbconf = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF, DBFLUSH, reg_tx_general_transmitter_control_tdbconf, dbflush);

    RU_REG_WRITE(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF, reg_tx_general_transmitter_control_tdbconf);

    return 0;
}

int ag_drv_gpon_tx_gen_ctrl_tdbconf_get(uint8_t *dbav, uint8_t *dbr_gen_en, uint8_t *divrate, uint8_t *dbflush)
{
    uint32_t reg_tx_general_transmitter_control_tdbconf=0;

#ifdef VALIDATE_PARMS
    if(!dbav || !dbr_gen_en || !divrate || !dbflush)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF, reg_tx_general_transmitter_control_tdbconf);

    *dbav = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF, DBAV, reg_tx_general_transmitter_control_tdbconf);
    *dbr_gen_en = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF, DBR_GEN_EN, reg_tx_general_transmitter_control_tdbconf);
    *divrate = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF, DIVRATE, reg_tx_general_transmitter_control_tdbconf);
    *dbflush = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TDBCONF, DBFLUSH, reg_tx_general_transmitter_control_tdbconf);

    return 0;
}

int ag_drv_gpon_tx_gen_ctrl_tpclr_set(uint8_t tpclrc)
{
    uint32_t reg_tx_general_transmitter_control_tpclr=0;

#ifdef VALIDATE_PARMS
    if((tpclrc >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_tx_general_transmitter_control_tpclr = RU_FIELD_SET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TPCLR, TPCLRC, reg_tx_general_transmitter_control_tpclr, tpclrc);

    RU_REG_WRITE(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TPCLR, reg_tx_general_transmitter_control_tpclr);

    return 0;
}

int ag_drv_gpon_tx_gen_ctrl_tpclr_get(uint8_t *tpclrc)
{
    uint32_t reg_tx_general_transmitter_control_tpclr=0;

#ifdef VALIDATE_PARMS
    if(!tpclrc)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TPCLR, reg_tx_general_transmitter_control_tpclr);

    *tpclrc = RU_FIELD_GET(0, GPON_TX_GEN_CTRL, TX_GENERAL_TRANSMITTER_CONTROL_TPCLR, TPCLRC, reg_tx_general_transmitter_control_tpclr);

    return 0;
}


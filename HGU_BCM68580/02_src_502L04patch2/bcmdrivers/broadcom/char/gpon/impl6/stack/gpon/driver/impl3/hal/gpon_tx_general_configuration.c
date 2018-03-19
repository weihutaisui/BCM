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
#include "gpon_tx_general_configuration.h"
bdmf_error_t ag_drv_gpon_tx_general_configuration_onuid_set(uint8_t onuid)
{
    uint32_t reg_onuid=0;

#ifdef VALIDATE_PARMS
#endif

    reg_onuid = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, ONUID, ONUID, reg_onuid, onuid);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, ONUID, reg_onuid);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_onuid_get(uint8_t *onuid)
{
    uint32_t reg_onuid=0;

#ifdef VALIDATE_PARMS
    if(!onuid)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, ONUID, reg_onuid);

    *onuid = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, ONUID, ONUID, reg_onuid);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_ind_set(const gpon_tx_general_configuration_ind *ind)
{
    uint32_t reg_ind=0;

#ifdef VALIDATE_PARMS
    if(!ind)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
    if((ind->twt2 >= _1BITS_MAX_VAL_) ||
       (ind->twt3 >= _1BITS_MAX_VAL_) ||
       (ind->twt4 >= _1BITS_MAX_VAL_) ||
       (ind->twt5 >= _1BITS_MAX_VAL_) ||
       (ind->rdi >= _1BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_ind = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, IND, TWT2, reg_ind, ind->twt2);
    reg_ind = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, IND, TWT3, reg_ind, ind->twt3);
    reg_ind = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, IND, TWT4, reg_ind, ind->twt4);
    reg_ind = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, IND, TWT5, reg_ind, ind->twt5);
    reg_ind = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, IND, RDI, reg_ind, ind->rdi);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, IND, reg_ind);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_ind_get(gpon_tx_general_configuration_ind *ind)
{
    uint32_t reg_ind=0;

#ifdef VALIDATE_PARMS
    if(!ind)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, IND, reg_ind);

    ind->twt2 = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, IND, TWT2, reg_ind);
    ind->twt3 = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, IND, TWT3, reg_ind);
    ind->twt4 = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, IND, TWT4, reg_ind);
    ind->twt5 = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, IND, TWT5, reg_ind);
    ind->rdi = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, IND, RDI, reg_ind);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_prod_set(uint32_t word_idx, uint8_t prodc)
{
    uint32_t reg_prod=0;

#ifdef VALIDATE_PARMS
    if((word_idx >= 16))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_prod = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, PROD, PRODC, reg_prod, prodc);

    RU_REG_RAM_WRITE(0, word_idx, GPON_TX_GENERAL_CONFIGURATION, PROD, reg_prod);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_prod_get(uint32_t word_idx, uint8_t *prodc)
{
    uint32_t reg_prod=0;

#ifdef VALIDATE_PARMS
    if(!prodc)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
    if((word_idx >= 16))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, word_idx, GPON_TX_GENERAL_CONFIGURATION, PROD, reg_prod);

    *prodc = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, PROD, PRODC, reg_prod);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_prods_set(uint8_t prcl, uint8_t prl)
{
    uint32_t reg_prods=0;

#ifdef VALIDATE_PARMS
    if((prcl >= _5BITS_MAX_VAL_) ||
       (prl >= _7BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_prods = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, PRODS, PRCL, reg_prods, prcl);
    reg_prods = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, PRODS, PRL, reg_prods, prl);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, PRODS, reg_prods);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_prods_get(uint8_t *prcl, uint8_t *prl)
{
    uint32_t reg_prods=0;

#ifdef VALIDATE_PARMS
    if(!prcl || !prl)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, PRODS, reg_prods);

    *prcl = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, PRODS, PRCL, reg_prods);
    *prl = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, PRODS, PRL, reg_prods);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_prrb_set(uint8_t brc)
{
    uint32_t reg_prrb=0;

#ifdef VALIDATE_PARMS
#endif

    reg_prrb = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, PRRB, BRC, reg_prrb, brc);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, PRRB, reg_prrb);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_prrb_get(uint8_t *brc)
{
    uint32_t reg_prrb=0;

#ifdef VALIDATE_PARMS
    if(!brc)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, PRRB, reg_prrb);

    *brc = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, PRRB, BRC, reg_prrb);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_delc_set(uint32_t deloc)
{
    uint32_t reg_delc=0;

#ifdef VALIDATE_PARMS
#endif

    reg_delc = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, DELC, DELOC, reg_delc, deloc);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, DELC, reg_delc);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_delc_get(uint32_t *deloc)
{
    uint32_t reg_delc=0;

#ifdef VALIDATE_PARMS
    if(!deloc)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, DELC, reg_delc);

    *deloc = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, DELC, DELOC, reg_delc);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_miscac1_set(uint16_t msstop, uint16_t msstart)
{
    uint32_t reg_miscac1=0;

#ifdef VALIDATE_PARMS
#endif

    reg_miscac1 = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, MISCAC1, MSSTOP, reg_miscac1, msstop);
    reg_miscac1 = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, MISCAC1, MSSTART, reg_miscac1, msstart);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, MISCAC1, reg_miscac1);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_miscac1_get(uint16_t *msstop, uint16_t *msstart)
{
    uint32_t reg_miscac1=0;

#ifdef VALIDATE_PARMS
    if(!msstop || !msstart)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, MISCAC1, reg_miscac1);

    *msstop = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, MISCAC1, MSSTOP, reg_miscac1);
    *msstart = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, MISCAC1, MSSTART, reg_miscac1);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_miscac2_set(bdmf_boolean ploamu, uint8_t dbru, bdmf_boolean fecu, uint8_t qu)
{
    uint32_t reg_miscac2=0;

#ifdef VALIDATE_PARMS
    if((ploamu >= _1BITS_MAX_VAL_) ||
       (dbru >= _2BITS_MAX_VAL_) ||
       (fecu >= _1BITS_MAX_VAL_) ||
       (qu >= _6BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_miscac2 = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, MISCAC2, PLOAMU, reg_miscac2, ploamu);
    reg_miscac2 = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, MISCAC2, DBRU, reg_miscac2, dbru);
    reg_miscac2 = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, MISCAC2, FECU, reg_miscac2, fecu);
    reg_miscac2 = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, MISCAC2, QU, reg_miscac2, qu);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, MISCAC2, reg_miscac2);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_miscac2_get(bdmf_boolean *ploamu, uint8_t *dbru, bdmf_boolean *fecu, uint8_t *qu)
{
    uint32_t reg_miscac2=0;

#ifdef VALIDATE_PARMS
    if(!ploamu || !dbru || !fecu || !qu)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, MISCAC2, reg_miscac2);

    *ploamu = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, MISCAC2, PLOAMU, reg_miscac2);
    *dbru = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, MISCAC2, DBRU, reg_miscac2);
    *fecu = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, MISCAC2, FECU, reg_miscac2);
    *qu = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, MISCAC2, QU, reg_miscac2);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_esall_set(bdmf_boolean esa)
{
    uint32_t reg_esall=0;

#ifdef VALIDATE_PARMS
    if((esa >= _1BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_esall = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, ESALL, ESA, reg_esall, esa);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, ESALL, reg_esall);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_esall_get(bdmf_boolean *esa)
{
    uint32_t reg_esall=0;

#ifdef VALIDATE_PARMS
    if(!esa)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, ESALL, reg_esall);

    *esa = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, ESALL, ESA, reg_esall);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_qforcntr_set(uint8_t qforgrp0, uint8_t qforgrp1, uint8_t qforgrp2, uint8_t qforgrp3)
{
    uint32_t reg_qforcntr=0;

#ifdef VALIDATE_PARMS
    if((qforgrp0 >= _6BITS_MAX_VAL_) ||
       (qforgrp1 >= _6BITS_MAX_VAL_) ||
       (qforgrp2 >= _6BITS_MAX_VAL_) ||
       (qforgrp3 >= _6BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_qforcntr = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, QFORCNTR, QFORGRP0, reg_qforcntr, qforgrp0);
    reg_qforcntr = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, QFORCNTR, QFORGRP1, reg_qforcntr, qforgrp1);
    reg_qforcntr = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, QFORCNTR, QFORGRP2, reg_qforcntr, qforgrp2);
    reg_qforcntr = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, QFORCNTR, QFORGRP3, reg_qforcntr, qforgrp3);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, QFORCNTR, reg_qforcntr);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_qforcntr_get(uint8_t *qforgrp0, uint8_t *qforgrp1, uint8_t *qforgrp2, uint8_t *qforgrp3)
{
    uint32_t reg_qforcntr=0;

#ifdef VALIDATE_PARMS
    if(!qforgrp0 || !qforgrp1 || !qforgrp2 || !qforgrp3)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, QFORCNTR, reg_qforcntr);

    *qforgrp0 = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, QFORCNTR, QFORGRP0, reg_qforcntr);
    *qforgrp1 = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, QFORCNTR, QFORGRP1, reg_qforcntr);
    *qforgrp2 = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, QFORCNTR, QFORGRP2, reg_qforcntr);
    *qforgrp3 = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, QFORCNTR, QFORGRP3, reg_qforcntr);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_plss_set(uint32_t plspat)
{
    uint32_t reg_plss=0;

#ifdef VALIDATE_PARMS
#endif

    reg_plss = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, PLSS, PLSPAT, reg_plss, plspat);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, PLSS, reg_plss);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_plss_get(uint32_t *plspat)
{
    uint32_t reg_plss=0;

#ifdef VALIDATE_PARMS
    if(!plspat)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, PLSS, reg_plss);

    *plspat = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, PLSS, PLSPAT, reg_plss);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_plsrs_set(uint8_t plsr)
{
    uint32_t reg_plsrs=0;

#ifdef VALIDATE_PARMS
    if((plsr >= _7BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_plsrs = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, PLSRS, PLSR, reg_plsrs, plsr);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, PLSRS, reg_plsrs);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_plsrs_get(uint8_t *plsr)
{
    uint32_t reg_plsrs=0;

#ifdef VALIDATE_PARMS
    if(!plsr)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, PLSRS, reg_plsrs);

    *plsr = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, PLSRS, PLSR, reg_plsrs);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_bips_set(uint16_t bipenum, uint8_t bipm, bdmf_boolean bipev, bdmf_boolean bipef)
{
    uint32_t reg_bips=0;

#ifdef VALIDATE_PARMS
    if((bipev >= _1BITS_MAX_VAL_) ||
       (bipef >= _1BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_bips = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, BIPS, BIPENUM, reg_bips, bipenum);
    reg_bips = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, BIPS, BIPM, reg_bips, bipm);
    reg_bips = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, BIPS, BIPEV, reg_bips, bipev);
    reg_bips = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, BIPS, BIPEF, reg_bips, bipef);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, BIPS, reg_bips);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_bips_get(uint16_t *bipenum, uint8_t *bipm, bdmf_boolean *bipev, bdmf_boolean *bipef)
{
    uint32_t reg_bips=0;

#ifdef VALIDATE_PARMS
    if(!bipenum || !bipm || !bipev || !bipef)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, BIPS, reg_bips);

    *bipenum = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, BIPS, BIPENUM, reg_bips);
    *bipm = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, BIPS, BIPM, reg_bips);
    *bipev = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, BIPS, BIPEV, reg_bips);
    *bipef = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, BIPS, BIPEF, reg_bips);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_crcoff_set(uint8_t crcoc)
{
    uint32_t reg_crcoff=0;

#ifdef VALIDATE_PARMS
    if((crcoc >= _7BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_crcoff = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, CRCOFF, CRCOC, reg_crcoff, crcoc);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, CRCOFF, reg_crcoff);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_crcoff_get(uint8_t *crcoc)
{
    uint32_t reg_crcoff=0;

#ifdef VALIDATE_PARMS
    if(!crcoc)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, CRCOFF, reg_crcoff);

    *crcoc = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, CRCOFF, CRCOC, reg_crcoff);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_flush_set(uint8_t flq, bdmf_boolean flen, bdmf_boolean flimm)
{
    uint32_t reg_flush=0;

#ifdef VALIDATE_PARMS
    if((flq >= _6BITS_MAX_VAL_) ||
       (flen >= _1BITS_MAX_VAL_) ||
       (flimm >= _1BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_flush = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, FLUSH, FLQ, reg_flush, flq);
    reg_flush = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, FLUSH, FLEN, reg_flush, flen);
    reg_flush = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, FLUSH, FLIMM, reg_flush, flimm);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, FLUSH, reg_flush);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_flush_get(uint8_t *flq, bdmf_boolean *flen, bdmf_boolean *flimm)
{
    uint32_t reg_flush=0;

#ifdef VALIDATE_PARMS
    if(!flq || !flen || !flimm)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, FLUSH, reg_flush);

    *flq = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, FLUSH, FLQ, reg_flush);
    *flen = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, FLUSH, FLEN, reg_flush);
    *flimm = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, FLUSH, FLIMM, reg_flush);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_flshdn_get(bdmf_boolean *fldn)
{
    uint32_t reg_flshdn=0;

#ifdef VALIDATE_PARMS
    if(!fldn)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, FLSHDN, reg_flshdn);

    *fldn = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, FLSHDN, FLDN, reg_flshdn);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_rdyind_set(bdmf_boolean set, bdmf_boolean rdy)
{
    uint32_t reg_rdyind=0;

#ifdef VALIDATE_PARMS
    if((set >= _1BITS_MAX_VAL_) ||
       (rdy >= _1BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_rdyind = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, RDYIND, SET, reg_rdyind, set);
    reg_rdyind = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, RDYIND, RDY, reg_rdyind, rdy);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, RDYIND, reg_rdyind);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_rdyind_get(bdmf_boolean *set, bdmf_boolean *rdy)
{
    uint32_t reg_rdyind=0;

#ifdef VALIDATE_PARMS
    if(!set || !rdy)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, RDYIND, reg_rdyind);

    *set = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, RDYIND, SET, reg_rdyind);
    *rdy = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, RDYIND, RDY, reg_rdyind);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_dvslp_set(bdmf_boolean dvpol, bdmf_boolean dvexten)
{
    uint32_t reg_dvslp=0;

#ifdef VALIDATE_PARMS
    if((dvpol >= _1BITS_MAX_VAL_) ||
       (dvexten >= _1BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_dvslp = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, DVSLP, DVPOL, reg_dvslp, dvpol);
    reg_dvslp = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, DVSLP, DVEXTEN, reg_dvslp, dvexten);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, DVSLP, reg_dvslp);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_dvslp_get(bdmf_boolean *dvpol, bdmf_boolean *dvexten)
{
    uint32_t reg_dvslp=0;

#ifdef VALIDATE_PARMS
    if(!dvpol || !dvexten)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, DVSLP, reg_dvslp);

    *dvpol = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, DVSLP, DVPOL, reg_dvslp);
    *dvexten = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, DVSLP, DVEXTEN, reg_dvslp);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_dvstp_set(uint32_t dvstu)
{
    uint32_t reg_dvstp=0;

#ifdef VALIDATE_PARMS
    if((dvstu >= _24BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_dvstp = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, DVSTP, DVSTU, reg_dvstp, dvstu);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, DVSTP, reg_dvstp);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_dvstp_get(uint32_t *dvstu)
{
    uint32_t reg_dvstp=0;

#ifdef VALIDATE_PARMS
    if(!dvstu)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, DVSTP, reg_dvstp);

    *dvstu = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, DVSTP, DVSTU, reg_dvstp);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_dvhld_set(uint32_t dvhld)
{
    uint32_t reg_dvhld=0;

#ifdef VALIDATE_PARMS
    if((dvhld >= _24BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_dvhld = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, DVHLD, DVHLD, reg_dvhld, dvhld);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, DVHLD, reg_dvhld);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_dvhld_get(uint32_t *dvhld)
{
    uint32_t reg_dvhld=0;

#ifdef VALIDATE_PARMS
    if(!dvhld)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, DVHLD, reg_dvhld);

    *dvhld = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, DVHLD, DVHLD, reg_dvhld);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_dpen_set(bdmf_boolean dpen, bdmf_boolean prbs, uint8_t stplen, uint8_t hldlen)
{
    uint32_t reg_dpen=0;

#ifdef VALIDATE_PARMS
    if((dpen >= _1BITS_MAX_VAL_) ||
       (prbs >= _1BITS_MAX_VAL_) ||
       (stplen >= _3BITS_MAX_VAL_) ||
       (hldlen >= _3BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_dpen = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, DPEN, DPEN, reg_dpen, dpen);
    reg_dpen = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, DPEN, PRBS, reg_dpen, prbs);
    reg_dpen = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, DPEN, STPLEN, reg_dpen, stplen);
    reg_dpen = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, DPEN, HLDLEN, reg_dpen, hldlen);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, DPEN, reg_dpen);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_dpen_get(bdmf_boolean *dpen, bdmf_boolean *prbs, uint8_t *stplen, uint8_t *hldlen)
{
    uint32_t reg_dpen=0;

#ifdef VALIDATE_PARMS
    if(!dpen || !prbs || !stplen || !hldlen)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, DPEN, reg_dpen);

    *dpen = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, DPEN, DPEN, reg_dpen);
    *prbs = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, DPEN, PRBS, reg_dpen);
    *stplen = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, DPEN, STPLEN, reg_dpen);
    *hldlen = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, DPEN, HLDLEN, reg_dpen);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_dstp_set(uint32_t stppat)
{
    uint32_t reg_dstp=0;

#ifdef VALIDATE_PARMS
#endif

    reg_dstp = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, DSTP, STPPAT, reg_dstp, stppat);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, DSTP, reg_dstp);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_dstp_get(uint32_t *stppat)
{
    uint32_t reg_dstp=0;

#ifdef VALIDATE_PARMS
    if(!stppat)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, DSTP, reg_dstp);

    *stppat = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, DSTP, STPPAT, reg_dstp);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_dhld_set(uint32_t hldpat)
{
    uint32_t reg_dhld=0;

#ifdef VALIDATE_PARMS
#endif

    reg_dhld = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, DHLD, HLDPAT, reg_dhld, hldpat);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, DHLD, reg_dhld);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_dhld_get(uint32_t *hldpat)
{
    uint32_t reg_dhld=0;

#ifdef VALIDATE_PARMS
    if(!hldpat)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, DHLD, reg_dhld);

    *hldpat = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, DHLD, HLDPAT, reg_dhld);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_rogue_onu_en_set(bdmf_boolean rogue_level_clr, bdmf_boolean rogue_diff_clr, bdmf_boolean rogue_dv_sel)
{
    uint32_t reg_rogue_onu_en=0;

#ifdef VALIDATE_PARMS
    if((rogue_level_clr >= _1BITS_MAX_VAL_) ||
       (rogue_diff_clr >= _1BITS_MAX_VAL_) ||
       (rogue_dv_sel >= _1BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_rogue_onu_en = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, ROGUE_ONU_EN, ROGUE_LEVEL_CLR, reg_rogue_onu_en, rogue_level_clr);
    reg_rogue_onu_en = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, ROGUE_ONU_EN, ROGUE_DIFF_CLR, reg_rogue_onu_en, rogue_diff_clr);
    reg_rogue_onu_en = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, ROGUE_ONU_EN, ROGUE_DV_SEL, reg_rogue_onu_en, rogue_dv_sel);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, ROGUE_ONU_EN, reg_rogue_onu_en);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_rogue_onu_en_get(bdmf_boolean *rogue_level_clr, bdmf_boolean *rogue_diff_clr, bdmf_boolean *rogue_dv_sel)
{
    uint32_t reg_rogue_onu_en=0;

#ifdef VALIDATE_PARMS
    if(!rogue_level_clr || !rogue_diff_clr || !rogue_dv_sel)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, ROGUE_ONU_EN, reg_rogue_onu_en);

    *rogue_level_clr = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, ROGUE_ONU_EN, ROGUE_LEVEL_CLR, reg_rogue_onu_en);
    *rogue_diff_clr = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, ROGUE_ONU_EN, ROGUE_DIFF_CLR, reg_rogue_onu_en);
    *rogue_dv_sel = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, ROGUE_ONU_EN, ROGUE_DV_SEL, reg_rogue_onu_en);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_rogue_level_time_set(uint32_t window_size)
{
    uint32_t reg_rogue_level_time=0;

#ifdef VALIDATE_PARMS
#endif

    reg_rogue_level_time = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, ROGUE_LEVEL_TIME, WINDOW_SIZE, reg_rogue_level_time, window_size);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, ROGUE_LEVEL_TIME, reg_rogue_level_time);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_rogue_level_time_get(uint32_t *window_size)
{
    uint32_t reg_rogue_level_time=0;

#ifdef VALIDATE_PARMS
    if(!window_size)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, ROGUE_LEVEL_TIME, reg_rogue_level_time);

    *window_size = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, ROGUE_LEVEL_TIME, WINDOW_SIZE, reg_rogue_level_time);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_rogue_diff_time_set(uint16_t window_size)
{
    uint32_t reg_rogue_diff_time=0;

#ifdef VALIDATE_PARMS
    if((window_size >= _10BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_rogue_diff_time = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, ROGUE_DIFF_TIME, WINDOW_SIZE, reg_rogue_diff_time, window_size);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, ROGUE_DIFF_TIME, reg_rogue_diff_time);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_general_configuration_rogue_diff_time_get(uint16_t *window_size)
{
    uint32_t reg_rogue_diff_time=0;

#ifdef VALIDATE_PARMS
    if(!window_size)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, ROGUE_DIFF_TIME, reg_rogue_diff_time);

    *window_size = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, ROGUE_DIFF_TIME, WINDOW_SIZE, reg_rogue_diff_time);

    return BDMF_ERR_OK;
}

#ifdef USE_BDMF_SHELL
enum
{
    BDMF_onuid,
    BDMF_ind,
    BDMF_prod,
    BDMF_prods,
    BDMF_prrb,
    BDMF_delc,
    BDMF_miscac1,
    BDMF_miscac2,
    BDMF_esall,
    BDMF_qforcntr,
    BDMF_plss,
    BDMF_plsrs,
    BDMF_bips,
    BDMF_crcoff,
    BDMF_flush,
    BDMF_flshdn,
    BDMF_rdyind,
    BDMF_dvslp,
    BDMF_dvstp,
    BDMF_dvhld,
    BDMF_dpen,
    BDMF_dstp,
    BDMF_dhld,
    BDMF_rogue_onu_en,
    BDMF_rogue_level_time,
    BDMF_rogue_diff_time,
};

typedef enum
{
    bdmf_address_onuid,
    bdmf_address_ind,
    bdmf_address_prod,
    bdmf_address_prods,
    bdmf_address_prrb,
    bdmf_address_delc,
    bdmf_address_miscac1,
    bdmf_address_miscac2,
    bdmf_address_esall,
    bdmf_address_qforcntr,
    bdmf_address_plss,
    bdmf_address_plsrs,
    bdmf_address_bips,
    bdmf_address_crcoff,
    bdmf_address_flush,
    bdmf_address_flshdn,
    bdmf_address_rdyind,
    bdmf_address_dvslp,
    bdmf_address_dvstp,
    bdmf_address_dvhld,
    bdmf_address_dpen,
    bdmf_address_dstp,
    bdmf_address_dhld,
    bdmf_address_rogue_onu_en,
    bdmf_address_rogue_level_time,
    bdmf_address_rogue_diff_time,
}
bdmf_address;

static int bcm_gpon_tx_general_configuration_cli_set(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_error_t err;

    switch(parm[0].value.unumber)
    {
    case BDMF_onuid:
        err = ag_drv_gpon_tx_general_configuration_onuid_set(parm[1].value.unumber);
        break;
    case BDMF_ind:
    {
        gpon_tx_general_configuration_ind ind = { .twt2=parm[1].value.unumber, .twt3=parm[2].value.unumber, .twt4=parm[3].value.unumber, .twt5=parm[4].value.unumber, .rdi=parm[5].value.unumber};
        err = ag_drv_gpon_tx_general_configuration_ind_set(&ind);
        break;
    }
    case BDMF_prod:
        err = ag_drv_gpon_tx_general_configuration_prod_set(parm[1].value.unumber, parm[2].value.unumber);
        break;
    case BDMF_prods:
        err = ag_drv_gpon_tx_general_configuration_prods_set(parm[1].value.unumber, parm[2].value.unumber);
        break;
    case BDMF_prrb:
        err = ag_drv_gpon_tx_general_configuration_prrb_set(parm[1].value.unumber);
        break;
    case BDMF_delc:
        err = ag_drv_gpon_tx_general_configuration_delc_set(parm[1].value.unumber);
        break;
    case BDMF_miscac1:
        err = ag_drv_gpon_tx_general_configuration_miscac1_set(parm[1].value.unumber, parm[2].value.unumber);
        break;
    case BDMF_miscac2:
        err = ag_drv_gpon_tx_general_configuration_miscac2_set(parm[1].value.unumber, parm[2].value.unumber, parm[3].value.unumber, parm[4].value.unumber);
        break;
    case BDMF_esall:
        err = ag_drv_gpon_tx_general_configuration_esall_set(parm[1].value.unumber);
        break;
    case BDMF_qforcntr:
        err = ag_drv_gpon_tx_general_configuration_qforcntr_set(parm[1].value.unumber, parm[2].value.unumber, parm[3].value.unumber, parm[4].value.unumber);
        break;
    case BDMF_plss:
        err = ag_drv_gpon_tx_general_configuration_plss_set(parm[1].value.unumber);
        break;
    case BDMF_plsrs:
        err = ag_drv_gpon_tx_general_configuration_plsrs_set(parm[1].value.unumber);
        break;
    case BDMF_bips:
        err = ag_drv_gpon_tx_general_configuration_bips_set(parm[1].value.unumber, parm[2].value.unumber, parm[3].value.unumber, parm[4].value.unumber);
        break;
    case BDMF_crcoff:
        err = ag_drv_gpon_tx_general_configuration_crcoff_set(parm[1].value.unumber);
        break;
    case BDMF_flush:
        err = ag_drv_gpon_tx_general_configuration_flush_set(parm[1].value.unumber, parm[2].value.unumber, parm[3].value.unumber);
        break;
    case BDMF_rdyind:
        err = ag_drv_gpon_tx_general_configuration_rdyind_set(parm[1].value.unumber, parm[2].value.unumber);
        break;
    case BDMF_dvslp:
        err = ag_drv_gpon_tx_general_configuration_dvslp_set(parm[1].value.unumber, parm[2].value.unumber);
        break;
    case BDMF_dvstp:
        err = ag_drv_gpon_tx_general_configuration_dvstp_set(parm[1].value.unumber);
        break;
    case BDMF_dvhld:
        err = ag_drv_gpon_tx_general_configuration_dvhld_set(parm[1].value.unumber);
        break;
    case BDMF_dpen:
        err = ag_drv_gpon_tx_general_configuration_dpen_set(parm[1].value.unumber, parm[2].value.unumber, parm[3].value.unumber, parm[4].value.unumber);
        break;
    case BDMF_dstp:
        err = ag_drv_gpon_tx_general_configuration_dstp_set(parm[1].value.unumber);
        break;
    case BDMF_dhld:
        err = ag_drv_gpon_tx_general_configuration_dhld_set(parm[1].value.unumber);
        break;
    case BDMF_rogue_onu_en:
        err = ag_drv_gpon_tx_general_configuration_rogue_onu_en_set(parm[1].value.unumber, parm[2].value.unumber, parm[3].value.unumber);
        break;
    case BDMF_rogue_level_time:
        err = ag_drv_gpon_tx_general_configuration_rogue_level_time_set(parm[1].value.unumber);
        break;
    case BDMF_rogue_diff_time:
        err = ag_drv_gpon_tx_general_configuration_rogue_diff_time_set(parm[1].value.unumber);
        break;
    default:
        err = BDMF_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

static int bcm_gpon_tx_general_configuration_cli_get(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_error_t err;

    switch(parm[0].value.unumber)
    {
    case BDMF_onuid:
    {
        uint8_t onuid;
        err = ag_drv_gpon_tx_general_configuration_onuid_get(&onuid);
        bdmf_session_print(session, "onuid = %u = 0x%x\n", onuid, onuid);
        break;
    }
    case BDMF_ind:
    {
        gpon_tx_general_configuration_ind ind;
        err = ag_drv_gpon_tx_general_configuration_ind_get(&ind);
        bdmf_session_print(session, "twt2 = %u = 0x%x\n", ind.twt2, ind.twt2);
        bdmf_session_print(session, "twt3 = %u = 0x%x\n", ind.twt3, ind.twt3);
        bdmf_session_print(session, "twt4 = %u = 0x%x\n", ind.twt4, ind.twt4);
        bdmf_session_print(session, "twt5 = %u = 0x%x\n", ind.twt5, ind.twt5);
        bdmf_session_print(session, "rdi = %u = 0x%x\n", ind.rdi, ind.rdi);
        break;
    }
    case BDMF_prod:
    {
        uint8_t prodc;
        err = ag_drv_gpon_tx_general_configuration_prod_get(parm[1].value.unumber, &prodc);
        bdmf_session_print(session, "prodc = %u = 0x%x\n", prodc, prodc);
        break;
    }
    case BDMF_prods:
    {
        uint8_t prcl;
        uint8_t prl;
        err = ag_drv_gpon_tx_general_configuration_prods_get(&prcl, &prl);
        bdmf_session_print(session, "prcl = %u = 0x%x\n", prcl, prcl);
        bdmf_session_print(session, "prl = %u = 0x%x\n", prl, prl);
        break;
    }
    case BDMF_prrb:
    {
        uint8_t brc;
        err = ag_drv_gpon_tx_general_configuration_prrb_get(&brc);
        bdmf_session_print(session, "brc = %u = 0x%x\n", brc, brc);
        break;
    }
    case BDMF_delc:
    {
        uint32_t deloc;
        err = ag_drv_gpon_tx_general_configuration_delc_get(&deloc);
        bdmf_session_print(session, "deloc = %u = 0x%x\n", deloc, deloc);
        break;
    }
    case BDMF_miscac1:
    {
        uint16_t msstop;
        uint16_t msstart;
        err = ag_drv_gpon_tx_general_configuration_miscac1_get(&msstop, &msstart);
        bdmf_session_print(session, "msstop = %u = 0x%x\n", msstop, msstop);
        bdmf_session_print(session, "msstart = %u = 0x%x\n", msstart, msstart);
        break;
    }
    case BDMF_miscac2:
    {
        bdmf_boolean ploamu;
        uint8_t dbru;
        bdmf_boolean fecu;
        uint8_t qu;
        err = ag_drv_gpon_tx_general_configuration_miscac2_get(&ploamu, &dbru, &fecu, &qu);
        bdmf_session_print(session, "ploamu = %u = 0x%x\n", ploamu, ploamu);
        bdmf_session_print(session, "dbru = %u = 0x%x\n", dbru, dbru);
        bdmf_session_print(session, "fecu = %u = 0x%x\n", fecu, fecu);
        bdmf_session_print(session, "qu = %u = 0x%x\n", qu, qu);
        break;
    }
    case BDMF_esall:
    {
        bdmf_boolean esa;
        err = ag_drv_gpon_tx_general_configuration_esall_get(&esa);
        bdmf_session_print(session, "esa = %u = 0x%x\n", esa, esa);
        break;
    }
    case BDMF_qforcntr:
    {
        uint8_t qforgrp0;
        uint8_t qforgrp1;
        uint8_t qforgrp2;
        uint8_t qforgrp3;
        err = ag_drv_gpon_tx_general_configuration_qforcntr_get(&qforgrp0, &qforgrp1, &qforgrp2, &qforgrp3);
        bdmf_session_print(session, "qforgrp0 = %u = 0x%x\n", qforgrp0, qforgrp0);
        bdmf_session_print(session, "qforgrp1 = %u = 0x%x\n", qforgrp1, qforgrp1);
        bdmf_session_print(session, "qforgrp2 = %u = 0x%x\n", qforgrp2, qforgrp2);
        bdmf_session_print(session, "qforgrp3 = %u = 0x%x\n", qforgrp3, qforgrp3);
        break;
    }
    case BDMF_plss:
    {
        uint32_t plspat;
        err = ag_drv_gpon_tx_general_configuration_plss_get(&plspat);
        bdmf_session_print(session, "plspat = %u = 0x%x\n", plspat, plspat);
        break;
    }
    case BDMF_plsrs:
    {
        uint8_t plsr;
        err = ag_drv_gpon_tx_general_configuration_plsrs_get(&plsr);
        bdmf_session_print(session, "plsr = %u = 0x%x\n", plsr, plsr);
        break;
    }
    case BDMF_bips:
    {
        uint16_t bipenum;
        uint8_t bipm;
        bdmf_boolean bipev;
        bdmf_boolean bipef;
        err = ag_drv_gpon_tx_general_configuration_bips_get(&bipenum, &bipm, &bipev, &bipef);
        bdmf_session_print(session, "bipenum = %u = 0x%x\n", bipenum, bipenum);
        bdmf_session_print(session, "bipm = %u = 0x%x\n", bipm, bipm);
        bdmf_session_print(session, "bipev = %u = 0x%x\n", bipev, bipev);
        bdmf_session_print(session, "bipef = %u = 0x%x\n", bipef, bipef);
        break;
    }
    case BDMF_crcoff:
    {
        uint8_t crcoc;
        err = ag_drv_gpon_tx_general_configuration_crcoff_get(&crcoc);
        bdmf_session_print(session, "crcoc = %u = 0x%x\n", crcoc, crcoc);
        break;
    }
    case BDMF_flush:
    {
        uint8_t flq;
        bdmf_boolean flen;
        bdmf_boolean flimm;
        err = ag_drv_gpon_tx_general_configuration_flush_get(&flq, &flen, &flimm);
        bdmf_session_print(session, "flq = %u = 0x%x\n", flq, flq);
        bdmf_session_print(session, "flen = %u = 0x%x\n", flen, flen);
        bdmf_session_print(session, "flimm = %u = 0x%x\n", flimm, flimm);
        break;
    }
    case BDMF_flshdn:
    {
        bdmf_boolean fldn;
        err = ag_drv_gpon_tx_general_configuration_flshdn_get(&fldn);
        bdmf_session_print(session, "fldn = %u = 0x%x\n", fldn, fldn);
        break;
    }
    case BDMF_rdyind:
    {
        bdmf_boolean set;
        bdmf_boolean rdy;
        err = ag_drv_gpon_tx_general_configuration_rdyind_get(&set, &rdy);
        bdmf_session_print(session, "set = %u = 0x%x\n", set, set);
        bdmf_session_print(session, "rdy = %u = 0x%x\n", rdy, rdy);
        break;
    }
    case BDMF_dvslp:
    {
        bdmf_boolean dvpol;
        bdmf_boolean dvexten;
        err = ag_drv_gpon_tx_general_configuration_dvslp_get(&dvpol, &dvexten);
        bdmf_session_print(session, "dvpol = %u = 0x%x\n", dvpol, dvpol);
        bdmf_session_print(session, "dvexten = %u = 0x%x\n", dvexten, dvexten);
        break;
    }
    case BDMF_dvstp:
    {
        uint32_t dvstu;
        err = ag_drv_gpon_tx_general_configuration_dvstp_get(&dvstu);
        bdmf_session_print(session, "dvstu = %u = 0x%x\n", dvstu, dvstu);
        break;
    }
    case BDMF_dvhld:
    {
        uint32_t dvhld;
        err = ag_drv_gpon_tx_general_configuration_dvhld_get(&dvhld);
        bdmf_session_print(session, "dvhld = %u = 0x%x\n", dvhld, dvhld);
        break;
    }
    case BDMF_dpen:
    {
        bdmf_boolean dpen;
        bdmf_boolean prbs;
        uint8_t stplen;
        uint8_t hldlen;
        err = ag_drv_gpon_tx_general_configuration_dpen_get(&dpen, &prbs, &stplen, &hldlen);
        bdmf_session_print(session, "dpen = %u = 0x%x\n", dpen, dpen);
        bdmf_session_print(session, "prbs = %u = 0x%x\n", prbs, prbs);
        bdmf_session_print(session, "stplen = %u = 0x%x\n", stplen, stplen);
        bdmf_session_print(session, "hldlen = %u = 0x%x\n", hldlen, hldlen);
        break;
    }
    case BDMF_dstp:
    {
        uint32_t stppat;
        err = ag_drv_gpon_tx_general_configuration_dstp_get(&stppat);
        bdmf_session_print(session, "stppat = %u = 0x%x\n", stppat, stppat);
        break;
    }
    case BDMF_dhld:
    {
        uint32_t hldpat;
        err = ag_drv_gpon_tx_general_configuration_dhld_get(&hldpat);
        bdmf_session_print(session, "hldpat = %u = 0x%x\n", hldpat, hldpat);
        break;
    }
    case BDMF_rogue_onu_en:
    {
        bdmf_boolean rogue_level_clr;
        bdmf_boolean rogue_diff_clr;
        bdmf_boolean rogue_dv_sel;
        err = ag_drv_gpon_tx_general_configuration_rogue_onu_en_get(&rogue_level_clr, &rogue_diff_clr, &rogue_dv_sel);
        bdmf_session_print(session, "rogue_level_clr = %u = 0x%x\n", rogue_level_clr, rogue_level_clr);
        bdmf_session_print(session, "rogue_diff_clr = %u = 0x%x\n", rogue_diff_clr, rogue_diff_clr);
        bdmf_session_print(session, "rogue_dv_sel = %u = 0x%x\n", rogue_dv_sel, rogue_dv_sel);
        break;
    }
    case BDMF_rogue_level_time:
    {
        uint32_t window_size;
        err = ag_drv_gpon_tx_general_configuration_rogue_level_time_get(&window_size);
        bdmf_session_print(session, "window_size = %u = 0x%x\n", window_size, window_size);
        break;
    }
    case BDMF_rogue_diff_time:
    {
        uint16_t window_size;
        err = ag_drv_gpon_tx_general_configuration_rogue_diff_time_get(&window_size);
        bdmf_session_print(session, "window_size = %u = 0x%x\n", window_size, window_size);
        break;
    }
    default:
        err = BDMF_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

static int bcm_gpon_tx_general_configuration_cli_test(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_test_method m = parm[0].value.unumber;
    bdmf_error_t err = BDMF_ERR_OK;

    {
        uint8_t onuid=gtmv(m, 8);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_onuid_set( %u)\n", onuid);
        if(!err) ag_drv_gpon_tx_general_configuration_onuid_set(onuid);
        if(!err) ag_drv_gpon_tx_general_configuration_onuid_get( &onuid);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_onuid_get( %u)\n", onuid);
        if(err || onuid!=gtmv(m, 8))
            return err ? err : BDMF_ERR_IO;
    }
    {
        gpon_tx_general_configuration_ind ind = {.twt2=gtmv(m, 1), .twt3=gtmv(m, 1), .twt4=gtmv(m, 1), .twt5=gtmv(m, 1), .rdi=gtmv(m, 1)};
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_ind_set( %u %u %u %u %u)\n", ind.twt2, ind.twt3, ind.twt4, ind.twt5, ind.rdi);
        if(!err) ag_drv_gpon_tx_general_configuration_ind_set(&ind);
        if(!err) ag_drv_gpon_tx_general_configuration_ind_get( &ind);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_ind_get( %u %u %u %u %u)\n", ind.twt2, ind.twt3, ind.twt4, ind.twt5, ind.rdi);
        if(err || ind.twt2!=gtmv(m, 1) || ind.twt3!=gtmv(m, 1) || ind.twt4!=gtmv(m, 1) || ind.twt5!=gtmv(m, 1) || ind.rdi!=gtmv(m, 1))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t word_idx=gtmv(m, 4);
        uint8_t prodc=gtmv(m, 8);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_prod_set( %u %u)\n", word_idx, prodc);
        if(!err) ag_drv_gpon_tx_general_configuration_prod_set(word_idx, prodc);
        if(!err) ag_drv_gpon_tx_general_configuration_prod_get( word_idx, &prodc);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_prod_get( %u %u)\n", word_idx, prodc);
        if(err || prodc!=gtmv(m, 8))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint8_t prcl=gtmv(m, 5);
        uint8_t prl=gtmv(m, 7);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_prods_set( %u %u)\n", prcl, prl);
        if(!err) ag_drv_gpon_tx_general_configuration_prods_set(prcl, prl);
        if(!err) ag_drv_gpon_tx_general_configuration_prods_get( &prcl, &prl);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_prods_get( %u %u)\n", prcl, prl);
        if(err || prcl!=gtmv(m, 5) || prl!=gtmv(m, 7))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint8_t brc=gtmv(m, 8);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_prrb_set( %u)\n", brc);
        if(!err) ag_drv_gpon_tx_general_configuration_prrb_set(brc);
        if(!err) ag_drv_gpon_tx_general_configuration_prrb_get( &brc);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_prrb_get( %u)\n", brc);
        if(err || brc!=gtmv(m, 8))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t deloc=gtmv(m, 32);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_delc_set( %u)\n", deloc);
        if(!err) ag_drv_gpon_tx_general_configuration_delc_set(deloc);
        if(!err) ag_drv_gpon_tx_general_configuration_delc_get( &deloc);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_delc_get( %u)\n", deloc);
        if(err || deloc!=gtmv(m, 32))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint16_t msstop=gtmv(m, 16);
        uint16_t msstart=gtmv(m, 16);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_miscac1_set( %u %u)\n", msstop, msstart);
        if(!err) ag_drv_gpon_tx_general_configuration_miscac1_set(msstop, msstart);
        if(!err) ag_drv_gpon_tx_general_configuration_miscac1_get( &msstop, &msstart);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_miscac1_get( %u %u)\n", msstop, msstart);
        if(err || msstop!=gtmv(m, 16) || msstart!=gtmv(m, 16))
            return err ? err : BDMF_ERR_IO;
    }
    {
        bdmf_boolean ploamu=gtmv(m, 1);
        uint8_t dbru=gtmv(m, 2);
        bdmf_boolean fecu=gtmv(m, 1);
        uint8_t qu=gtmv(m, 6);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_miscac2_set( %u %u %u %u)\n", ploamu, dbru, fecu, qu);
        if(!err) ag_drv_gpon_tx_general_configuration_miscac2_set(ploamu, dbru, fecu, qu);
        if(!err) ag_drv_gpon_tx_general_configuration_miscac2_get( &ploamu, &dbru, &fecu, &qu);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_miscac2_get( %u %u %u %u)\n", ploamu, dbru, fecu, qu);
        if(err || ploamu!=gtmv(m, 1) || dbru!=gtmv(m, 2) || fecu!=gtmv(m, 1) || qu!=gtmv(m, 6))
            return err ? err : BDMF_ERR_IO;
    }
    {
        bdmf_boolean esa=gtmv(m, 1);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_esall_set( %u)\n", esa);
        if(!err) ag_drv_gpon_tx_general_configuration_esall_set(esa);
        if(!err) ag_drv_gpon_tx_general_configuration_esall_get( &esa);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_esall_get( %u)\n", esa);
        if(err || esa!=gtmv(m, 1))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint8_t qforgrp0=gtmv(m, 6);
        uint8_t qforgrp1=gtmv(m, 6);
        uint8_t qforgrp2=gtmv(m, 6);
        uint8_t qforgrp3=gtmv(m, 6);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_qforcntr_set( %u %u %u %u)\n", qforgrp0, qforgrp1, qforgrp2, qforgrp3);
        if(!err) ag_drv_gpon_tx_general_configuration_qforcntr_set(qforgrp0, qforgrp1, qforgrp2, qforgrp3);
        if(!err) ag_drv_gpon_tx_general_configuration_qforcntr_get( &qforgrp0, &qforgrp1, &qforgrp2, &qforgrp3);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_qforcntr_get( %u %u %u %u)\n", qforgrp0, qforgrp1, qforgrp2, qforgrp3);
        if(err || qforgrp0!=gtmv(m, 6) || qforgrp1!=gtmv(m, 6) || qforgrp2!=gtmv(m, 6) || qforgrp3!=gtmv(m, 6))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t plspat=gtmv(m, 32);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_plss_set( %u)\n", plspat);
        if(!err) ag_drv_gpon_tx_general_configuration_plss_set(plspat);
        if(!err) ag_drv_gpon_tx_general_configuration_plss_get( &plspat);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_plss_get( %u)\n", plspat);
        if(err || plspat!=gtmv(m, 32))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint8_t plsr=gtmv(m, 7);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_plsrs_set( %u)\n", plsr);
        if(!err) ag_drv_gpon_tx_general_configuration_plsrs_set(plsr);
        if(!err) ag_drv_gpon_tx_general_configuration_plsrs_get( &plsr);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_plsrs_get( %u)\n", plsr);
        if(err || plsr!=gtmv(m, 7))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint16_t bipenum=gtmv(m, 16);
        uint8_t bipm=gtmv(m, 8);
        bdmf_boolean bipev=gtmv(m, 1);
        bdmf_boolean bipef=gtmv(m, 1);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_bips_set( %u %u %u %u)\n", bipenum, bipm, bipev, bipef);
        if(!err) ag_drv_gpon_tx_general_configuration_bips_set(bipenum, bipm, bipev, bipef);
        if(!err) ag_drv_gpon_tx_general_configuration_bips_get( &bipenum, &bipm, &bipev, &bipef);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_bips_get( %u %u %u %u)\n", bipenum, bipm, bipev, bipef);
        if(err || bipenum!=gtmv(m, 16) || bipm!=gtmv(m, 8) || bipev!=gtmv(m, 1) || bipef!=gtmv(m, 1))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint8_t crcoc=gtmv(m, 7);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_crcoff_set( %u)\n", crcoc);
        if(!err) ag_drv_gpon_tx_general_configuration_crcoff_set(crcoc);
        if(!err) ag_drv_gpon_tx_general_configuration_crcoff_get( &crcoc);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_crcoff_get( %u)\n", crcoc);
        if(err || crcoc!=gtmv(m, 7))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint8_t flq=gtmv(m, 6);
        bdmf_boolean flen=gtmv(m, 1);
        bdmf_boolean flimm=gtmv(m, 1);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_flush_set( %u %u %u)\n", flq, flen, flimm);
        if(!err) ag_drv_gpon_tx_general_configuration_flush_set(flq, flen, flimm);
        if(!err) ag_drv_gpon_tx_general_configuration_flush_get( &flq, &flen, &flimm);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_flush_get( %u %u %u)\n", flq, flen, flimm);
        if(err || flq!=gtmv(m, 6) || flen!=gtmv(m, 1) || flimm!=gtmv(m, 1))
            return err ? err : BDMF_ERR_IO;
    }
    {
        bdmf_boolean fldn=gtmv(m, 1);
        if(!err) ag_drv_gpon_tx_general_configuration_flshdn_get( &fldn);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_flshdn_get( %u)\n", fldn);
    }
    {
        bdmf_boolean set=gtmv(m, 1);
        bdmf_boolean rdy=gtmv(m, 1);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_rdyind_set( %u %u)\n", set, rdy);
        if(!err) ag_drv_gpon_tx_general_configuration_rdyind_set(set, rdy);
        if(!err) ag_drv_gpon_tx_general_configuration_rdyind_get( &set, &rdy);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_rdyind_get( %u %u)\n", set, rdy);
        if(err || set!=gtmv(m, 1) || rdy!=gtmv(m, 1))
            return err ? err : BDMF_ERR_IO;
    }
    {
        bdmf_boolean dvpol=gtmv(m, 1);
        bdmf_boolean dvexten=gtmv(m, 1);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_dvslp_set( %u %u)\n", dvpol, dvexten);
        if(!err) ag_drv_gpon_tx_general_configuration_dvslp_set(dvpol, dvexten);
        if(!err) ag_drv_gpon_tx_general_configuration_dvslp_get( &dvpol, &dvexten);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_dvslp_get( %u %u)\n", dvpol, dvexten);
        if(err || dvpol!=gtmv(m, 1) || dvexten!=gtmv(m, 1))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t dvstu=gtmv(m, 24);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_dvstp_set( %u)\n", dvstu);
        if(!err) ag_drv_gpon_tx_general_configuration_dvstp_set(dvstu);
        if(!err) ag_drv_gpon_tx_general_configuration_dvstp_get( &dvstu);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_dvstp_get( %u)\n", dvstu);
        if(err || dvstu!=gtmv(m, 24))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t dvhld=gtmv(m, 24);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_dvhld_set( %u)\n", dvhld);
        if(!err) ag_drv_gpon_tx_general_configuration_dvhld_set(dvhld);
        if(!err) ag_drv_gpon_tx_general_configuration_dvhld_get( &dvhld);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_dvhld_get( %u)\n", dvhld);
        if(err || dvhld!=gtmv(m, 24))
            return err ? err : BDMF_ERR_IO;
    }
    {
        bdmf_boolean dpen=gtmv(m, 1);
        bdmf_boolean prbs=gtmv(m, 1);
        uint8_t stplen=gtmv(m, 3);
        uint8_t hldlen=gtmv(m, 3);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_dpen_set( %u %u %u %u)\n", dpen, prbs, stplen, hldlen);
        if(!err) ag_drv_gpon_tx_general_configuration_dpen_set(dpen, prbs, stplen, hldlen);
        if(!err) ag_drv_gpon_tx_general_configuration_dpen_get( &dpen, &prbs, &stplen, &hldlen);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_dpen_get( %u %u %u %u)\n", dpen, prbs, stplen, hldlen);
        if(err || dpen!=gtmv(m, 1) || prbs!=gtmv(m, 1) || stplen!=gtmv(m, 3) || hldlen!=gtmv(m, 3))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t stppat=gtmv(m, 32);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_dstp_set( %u)\n", stppat);
        if(!err) ag_drv_gpon_tx_general_configuration_dstp_set(stppat);
        if(!err) ag_drv_gpon_tx_general_configuration_dstp_get( &stppat);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_dstp_get( %u)\n", stppat);
        if(err || stppat!=gtmv(m, 32))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t hldpat=gtmv(m, 32);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_dhld_set( %u)\n", hldpat);
        if(!err) ag_drv_gpon_tx_general_configuration_dhld_set(hldpat);
        if(!err) ag_drv_gpon_tx_general_configuration_dhld_get( &hldpat);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_dhld_get( %u)\n", hldpat);
        if(err || hldpat!=gtmv(m, 32))
            return err ? err : BDMF_ERR_IO;
    }
    {
        bdmf_boolean rogue_level_clr=gtmv(m, 1);
        bdmf_boolean rogue_diff_clr=gtmv(m, 1);
        bdmf_boolean rogue_dv_sel=gtmv(m, 1);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_rogue_onu_en_set( %u %u %u)\n", rogue_level_clr, rogue_diff_clr, rogue_dv_sel);
        if(!err) ag_drv_gpon_tx_general_configuration_rogue_onu_en_set(rogue_level_clr, rogue_diff_clr, rogue_dv_sel);
        if(!err) ag_drv_gpon_tx_general_configuration_rogue_onu_en_get( &rogue_level_clr, &rogue_diff_clr, &rogue_dv_sel);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_rogue_onu_en_get( %u %u %u)\n", rogue_level_clr, rogue_diff_clr, rogue_dv_sel);
        if(err || rogue_level_clr!=gtmv(m, 1) || rogue_diff_clr!=gtmv(m, 1) || rogue_dv_sel!=gtmv(m, 1))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t window_size=gtmv(m, 32);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_rogue_level_time_set( %u)\n", window_size);
        if(!err) ag_drv_gpon_tx_general_configuration_rogue_level_time_set(window_size);
        if(!err) ag_drv_gpon_tx_general_configuration_rogue_level_time_get( &window_size);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_rogue_level_time_get( %u)\n", window_size);
        if(err || window_size!=gtmv(m, 32))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint16_t window_size=gtmv(m, 10);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_rogue_diff_time_set( %u)\n", window_size);
        if(!err) ag_drv_gpon_tx_general_configuration_rogue_diff_time_set(window_size);
        if(!err) ag_drv_gpon_tx_general_configuration_rogue_diff_time_get( &window_size);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_general_configuration_rogue_diff_time_get( %u)\n", window_size);
        if(err || window_size!=gtmv(m, 10))
            return err ? err : BDMF_ERR_IO;
    }
    return err;
}

static int bcm_gpon_tx_general_configuration_cli_address(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
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
    case bdmf_address_onuid : reg = &RU_REG(GPON_TX_GENERAL_CONFIGURATION, ONUID); blk = &RU_BLK(GPON_TX_GENERAL_CONFIGURATION); break;
    case bdmf_address_ind : reg = &RU_REG(GPON_TX_GENERAL_CONFIGURATION, IND); blk = &RU_BLK(GPON_TX_GENERAL_CONFIGURATION); break;
    case bdmf_address_prod : reg = &RU_REG(GPON_TX_GENERAL_CONFIGURATION, PROD); blk = &RU_BLK(GPON_TX_GENERAL_CONFIGURATION); break;
    case bdmf_address_prods : reg = &RU_REG(GPON_TX_GENERAL_CONFIGURATION, PRODS); blk = &RU_BLK(GPON_TX_GENERAL_CONFIGURATION); break;
    case bdmf_address_prrb : reg = &RU_REG(GPON_TX_GENERAL_CONFIGURATION, PRRB); blk = &RU_BLK(GPON_TX_GENERAL_CONFIGURATION); break;
    case bdmf_address_delc : reg = &RU_REG(GPON_TX_GENERAL_CONFIGURATION, DELC); blk = &RU_BLK(GPON_TX_GENERAL_CONFIGURATION); break;
    case bdmf_address_miscac1 : reg = &RU_REG(GPON_TX_GENERAL_CONFIGURATION, MISCAC1); blk = &RU_BLK(GPON_TX_GENERAL_CONFIGURATION); break;
    case bdmf_address_miscac2 : reg = &RU_REG(GPON_TX_GENERAL_CONFIGURATION, MISCAC2); blk = &RU_BLK(GPON_TX_GENERAL_CONFIGURATION); break;
    case bdmf_address_esall : reg = &RU_REG(GPON_TX_GENERAL_CONFIGURATION, ESALL); blk = &RU_BLK(GPON_TX_GENERAL_CONFIGURATION); break;
    case bdmf_address_qforcntr : reg = &RU_REG(GPON_TX_GENERAL_CONFIGURATION, QFORCNTR); blk = &RU_BLK(GPON_TX_GENERAL_CONFIGURATION); break;
    case bdmf_address_plss : reg = &RU_REG(GPON_TX_GENERAL_CONFIGURATION, PLSS); blk = &RU_BLK(GPON_TX_GENERAL_CONFIGURATION); break;
    case bdmf_address_plsrs : reg = &RU_REG(GPON_TX_GENERAL_CONFIGURATION, PLSRS); blk = &RU_BLK(GPON_TX_GENERAL_CONFIGURATION); break;
    case bdmf_address_bips : reg = &RU_REG(GPON_TX_GENERAL_CONFIGURATION, BIPS); blk = &RU_BLK(GPON_TX_GENERAL_CONFIGURATION); break;
    case bdmf_address_crcoff : reg = &RU_REG(GPON_TX_GENERAL_CONFIGURATION, CRCOFF); blk = &RU_BLK(GPON_TX_GENERAL_CONFIGURATION); break;
    case bdmf_address_flush : reg = &RU_REG(GPON_TX_GENERAL_CONFIGURATION, FLUSH); blk = &RU_BLK(GPON_TX_GENERAL_CONFIGURATION); break;
    case bdmf_address_flshdn : reg = &RU_REG(GPON_TX_GENERAL_CONFIGURATION, FLSHDN); blk = &RU_BLK(GPON_TX_GENERAL_CONFIGURATION); break;
    case bdmf_address_rdyind : reg = &RU_REG(GPON_TX_GENERAL_CONFIGURATION, RDYIND); blk = &RU_BLK(GPON_TX_GENERAL_CONFIGURATION); break;
    case bdmf_address_dvslp : reg = &RU_REG(GPON_TX_GENERAL_CONFIGURATION, DVSLP); blk = &RU_BLK(GPON_TX_GENERAL_CONFIGURATION); break;
    case bdmf_address_dvstp : reg = &RU_REG(GPON_TX_GENERAL_CONFIGURATION, DVSTP); blk = &RU_BLK(GPON_TX_GENERAL_CONFIGURATION); break;
    case bdmf_address_dvhld : reg = &RU_REG(GPON_TX_GENERAL_CONFIGURATION, DVHLD); blk = &RU_BLK(GPON_TX_GENERAL_CONFIGURATION); break;
    case bdmf_address_dpen : reg = &RU_REG(GPON_TX_GENERAL_CONFIGURATION, DPEN); blk = &RU_BLK(GPON_TX_GENERAL_CONFIGURATION); break;
    case bdmf_address_dstp : reg = &RU_REG(GPON_TX_GENERAL_CONFIGURATION, DSTP); blk = &RU_BLK(GPON_TX_GENERAL_CONFIGURATION); break;
    case bdmf_address_dhld : reg = &RU_REG(GPON_TX_GENERAL_CONFIGURATION, DHLD); blk = &RU_BLK(GPON_TX_GENERAL_CONFIGURATION); break;
    case bdmf_address_rogue_onu_en : reg = &RU_REG(GPON_TX_GENERAL_CONFIGURATION, ROGUE_ONU_EN); blk = &RU_BLK(GPON_TX_GENERAL_CONFIGURATION); break;
    case bdmf_address_rogue_level_time : reg = &RU_REG(GPON_TX_GENERAL_CONFIGURATION, ROGUE_LEVEL_TIME); blk = &RU_BLK(GPON_TX_GENERAL_CONFIGURATION); break;
    case bdmf_address_rogue_diff_time : reg = &RU_REG(GPON_TX_GENERAL_CONFIGURATION, ROGUE_DIFF_TIME); blk = &RU_BLK(GPON_TX_GENERAL_CONFIGURATION); break;
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

bdmfmon_handle_t ag_drv_gpon_tx_general_configuration_cli_init(bdmfmon_handle_t driver_dir)
{
    bdmfmon_handle_t dir;

    if ((dir = bdmfmon_dir_find(driver_dir, "gpon_tx_general_configuration"))!=NULL)
        return dir;
    dir = bdmfmon_dir_add(driver_dir, "gpon_tx_general_configuration", "gpon_tx_general_configuration", BDMF_ACCESS_ADMIN, NULL);

    {
        static bdmfmon_cmd_parm_t set_onuid[]={
            BDMFMON_MAKE_PARM("onuid", "onuid", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_ind[]={
            BDMFMON_MAKE_PARM("twt2", "twt2", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("twt3", "twt3", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("twt4", "twt4", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("twt5", "twt5", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("rdi", "rdi", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_prod[]={
            BDMFMON_MAKE_PARM("word_idx", "word_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("prodc", "prodc", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_prods[]={
            BDMFMON_MAKE_PARM("prcl", "prcl", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("prl", "prl", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_prrb[]={
            BDMFMON_MAKE_PARM("brc", "brc", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_delc[]={
            BDMFMON_MAKE_PARM("deloc", "deloc", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_miscac1[]={
            BDMFMON_MAKE_PARM("msstop", "msstop", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("msstart", "msstart", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_miscac2[]={
            BDMFMON_MAKE_PARM("ploamu", "ploamu", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("dbru", "dbru", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("fecu", "fecu", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("qu", "qu", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_esall[]={
            BDMFMON_MAKE_PARM("esa", "esa", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_qforcntr[]={
            BDMFMON_MAKE_PARM("qforgrp0", "qforgrp0", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("qforgrp1", "qforgrp1", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("qforgrp2", "qforgrp2", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("qforgrp3", "qforgrp3", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_plss[]={
            BDMFMON_MAKE_PARM("plspat", "plspat", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_plsrs[]={
            BDMFMON_MAKE_PARM("plsr", "plsr", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_bips[]={
            BDMFMON_MAKE_PARM("bipenum", "bipenum", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("bipm", "bipm", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("bipev", "bipev", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("bipef", "bipef", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_crcoff[]={
            BDMFMON_MAKE_PARM("crcoc", "crcoc", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_flush[]={
            BDMFMON_MAKE_PARM("flq", "flq", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("flen", "flen", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("flimm", "flimm", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_rdyind[]={
            BDMFMON_MAKE_PARM("set", "set", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("rdy", "rdy", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_dvslp[]={
            BDMFMON_MAKE_PARM("dvpol", "dvpol", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("dvexten", "dvexten", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_dvstp[]={
            BDMFMON_MAKE_PARM("dvstu", "dvstu", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_dvhld[]={
            BDMFMON_MAKE_PARM("dvhld", "dvhld", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_dpen[]={
            BDMFMON_MAKE_PARM("dpen", "dpen", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("prbs", "prbs", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("stplen", "stplen", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("hldlen", "hldlen", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_dstp[]={
            BDMFMON_MAKE_PARM("stppat", "stppat", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_dhld[]={
            BDMFMON_MAKE_PARM("hldpat", "hldpat", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_rogue_onu_en[]={
            BDMFMON_MAKE_PARM("rogue_level_clr", "rogue_level_clr", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("rogue_diff_clr", "rogue_diff_clr", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("rogue_dv_sel", "rogue_dv_sel", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_rogue_level_time[]={
            BDMFMON_MAKE_PARM("window_size", "window_size", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_rogue_diff_time[]={
            BDMFMON_MAKE_PARM("window_size", "window_size", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_enum_val_t selector_table[] = {
            { .name="onuid", .val=BDMF_onuid, .parms=set_onuid },
            { .name="ind", .val=BDMF_ind, .parms=set_ind },
            { .name="prod", .val=BDMF_prod, .parms=set_prod },
            { .name="prods", .val=BDMF_prods, .parms=set_prods },
            { .name="prrb", .val=BDMF_prrb, .parms=set_prrb },
            { .name="delc", .val=BDMF_delc, .parms=set_delc },
            { .name="miscac1", .val=BDMF_miscac1, .parms=set_miscac1 },
            { .name="miscac2", .val=BDMF_miscac2, .parms=set_miscac2 },
            { .name="esall", .val=BDMF_esall, .parms=set_esall },
            { .name="qforcntr", .val=BDMF_qforcntr, .parms=set_qforcntr },
            { .name="plss", .val=BDMF_plss, .parms=set_plss },
            { .name="plsrs", .val=BDMF_plsrs, .parms=set_plsrs },
            { .name="bips", .val=BDMF_bips, .parms=set_bips },
            { .name="crcoff", .val=BDMF_crcoff, .parms=set_crcoff },
            { .name="flush", .val=BDMF_flush, .parms=set_flush },
            { .name="rdyind", .val=BDMF_rdyind, .parms=set_rdyind },
            { .name="dvslp", .val=BDMF_dvslp, .parms=set_dvslp },
            { .name="dvstp", .val=BDMF_dvstp, .parms=set_dvstp },
            { .name="dvhld", .val=BDMF_dvhld, .parms=set_dvhld },
            { .name="dpen", .val=BDMF_dpen, .parms=set_dpen },
            { .name="dstp", .val=BDMF_dstp, .parms=set_dstp },
            { .name="dhld", .val=BDMF_dhld, .parms=set_dhld },
            { .name="rogue_onu_en", .val=BDMF_rogue_onu_en, .parms=set_rogue_onu_en },
            { .name="rogue_level_time", .val=BDMF_rogue_level_time, .parms=set_rogue_level_time },
            { .name="rogue_diff_time", .val=BDMF_rogue_diff_time, .parms=set_rogue_diff_time },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "set", "set", bcm_gpon_tx_general_configuration_cli_set,
            BDMFMON_MAKE_PARM_SELECTOR("purpose", "purpose", selector_table, 0));
    }
    {
        static bdmfmon_cmd_parm_t set_default[]={
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_prod[]={
            BDMFMON_MAKE_PARM("word_idx", "word_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_enum_val_t selector_table[] = {
            { .name="onuid", .val=BDMF_onuid, .parms=set_default },
            { .name="ind", .val=BDMF_ind, .parms=set_default },
            { .name="prod", .val=BDMF_prod, .parms=set_prod },
            { .name="prods", .val=BDMF_prods, .parms=set_default },
            { .name="prrb", .val=BDMF_prrb, .parms=set_default },
            { .name="delc", .val=BDMF_delc, .parms=set_default },
            { .name="miscac1", .val=BDMF_miscac1, .parms=set_default },
            { .name="miscac2", .val=BDMF_miscac2, .parms=set_default },
            { .name="esall", .val=BDMF_esall, .parms=set_default },
            { .name="qforcntr", .val=BDMF_qforcntr, .parms=set_default },
            { .name="plss", .val=BDMF_plss, .parms=set_default },
            { .name="plsrs", .val=BDMF_plsrs, .parms=set_default },
            { .name="bips", .val=BDMF_bips, .parms=set_default },
            { .name="crcoff", .val=BDMF_crcoff, .parms=set_default },
            { .name="flush", .val=BDMF_flush, .parms=set_default },
            { .name="flshdn", .val=BDMF_flshdn, .parms=set_default },
            { .name="rdyind", .val=BDMF_rdyind, .parms=set_default },
            { .name="dvslp", .val=BDMF_dvslp, .parms=set_default },
            { .name="dvstp", .val=BDMF_dvstp, .parms=set_default },
            { .name="dvhld", .val=BDMF_dvhld, .parms=set_default },
            { .name="dpen", .val=BDMF_dpen, .parms=set_default },
            { .name="dstp", .val=BDMF_dstp, .parms=set_default },
            { .name="dhld", .val=BDMF_dhld, .parms=set_default },
            { .name="rogue_onu_en", .val=BDMF_rogue_onu_en, .parms=set_default },
            { .name="rogue_level_time", .val=BDMF_rogue_level_time, .parms=set_default },
            { .name="rogue_diff_time", .val=BDMF_rogue_diff_time, .parms=set_default },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "get", "get", bcm_gpon_tx_general_configuration_cli_get,
            BDMFMON_MAKE_PARM_SELECTOR("purpose", "purpose", selector_table, 0));
    }
    {
        static bdmfmon_enum_val_t enum_table_test_method[] = {
            { .name="low" , .val=bdmf_test_method_low },
            { .name="mid" , .val=bdmf_test_method_mid },
            { .name="high" , .val=bdmf_test_method_high },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "test", "test", bcm_gpon_tx_general_configuration_cli_test,
            BDMFMON_MAKE_PARM_ENUM("method", "low: 0000, mid: 1000, high: 1111", enum_table_test_method, 0));
    }
    {
        static bdmfmon_enum_val_t enum_table_address[] = {
            { .name="ONUID" , .val=bdmf_address_onuid },
            { .name="IND" , .val=bdmf_address_ind },
            { .name="PROD" , .val=bdmf_address_prod },
            { .name="PRODS" , .val=bdmf_address_prods },
            { .name="PRRB" , .val=bdmf_address_prrb },
            { .name="DELC" , .val=bdmf_address_delc },
            { .name="MISCAC1" , .val=bdmf_address_miscac1 },
            { .name="MISCAC2" , .val=bdmf_address_miscac2 },
            { .name="ESALL" , .val=bdmf_address_esall },
            { .name="QFORCNTR" , .val=bdmf_address_qforcntr },
            { .name="PLSS" , .val=bdmf_address_plss },
            { .name="PLSRS" , .val=bdmf_address_plsrs },
            { .name="BIPS" , .val=bdmf_address_bips },
            { .name="CRCOFF" , .val=bdmf_address_crcoff },
            { .name="FLUSH" , .val=bdmf_address_flush },
            { .name="FLSHDN" , .val=bdmf_address_flshdn },
            { .name="RDYIND" , .val=bdmf_address_rdyind },
            { .name="DVSLP" , .val=bdmf_address_dvslp },
            { .name="DVSTP" , .val=bdmf_address_dvstp },
            { .name="DVHLD" , .val=bdmf_address_dvhld },
            { .name="DPEN" , .val=bdmf_address_dpen },
            { .name="DSTP" , .val=bdmf_address_dstp },
            { .name="DHLD" , .val=bdmf_address_dhld },
            { .name="ROGUE_ONU_EN" , .val=bdmf_address_rogue_onu_en },
            { .name="ROGUE_LEVEL_TIME" , .val=bdmf_address_rogue_level_time },
            { .name="ROGUE_DIFF_TIME" , .val=bdmf_address_rogue_diff_time },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "address", "address", bcm_gpon_tx_general_configuration_cli_address,
            BDMFMON_MAKE_PARM_ENUM("method", "method", enum_table_address, 0),
            BDMFMON_MAKE_PARM("index2", "onu_id/alloc_id/port_id/etc...", BDMFMON_PARM_NUMBER, BDMFMON_PARM_FLAG_OPTIONAL));
    }
    return dir;
}
#endif /* USE_BDMF_SHELL */


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
#include "gpon_tx_general_configuration.h"
int ag_drv_gpon_tx_general_configuration_onuid_set(uint8_t onuid)
{
    uint32_t reg_onuid=0;

#ifdef VALIDATE_PARMS
#endif

    reg_onuid = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, ONUID, ONUID, reg_onuid, onuid);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, ONUID, reg_onuid);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_onuid_get(uint8_t *onuid)
{
    uint32_t reg_onuid=0;

#ifdef VALIDATE_PARMS
    if(!onuid)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, ONUID, reg_onuid);

    *onuid = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, ONUID, ONUID, reg_onuid);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_ind_set(const gpon_tx_general_configuration_ind *ind)
{
    uint32_t reg_ind=0;

#ifdef VALIDATE_PARMS
    if(!ind)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, 1);
        return 1;
    }
    if((ind->twt2 >= _1BITS_MAX_VAL_) ||
       (ind->twt3 >= _1BITS_MAX_VAL_) ||
       (ind->twt4 >= _1BITS_MAX_VAL_) ||
       (ind->twt5 >= _1BITS_MAX_VAL_) ||
       (ind->rdi >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_ind = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, IND, TWT2, reg_ind, ind->twt2);
    reg_ind = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, IND, TWT3, reg_ind, ind->twt3);
    reg_ind = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, IND, TWT4, reg_ind, ind->twt4);
    reg_ind = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, IND, TWT5, reg_ind, ind->twt5);
    reg_ind = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, IND, RDI, reg_ind, ind->rdi);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, IND, reg_ind);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_ind_get(gpon_tx_general_configuration_ind *ind)
{
    uint32_t reg_ind=0;

#ifdef VALIDATE_PARMS
    if(!ind)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, IND, reg_ind);

    ind->twt2 = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, IND, TWT2, reg_ind);
    ind->twt3 = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, IND, TWT3, reg_ind);
    ind->twt4 = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, IND, TWT4, reg_ind);
    ind->twt5 = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, IND, TWT5, reg_ind);
    ind->rdi = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, IND, RDI, reg_ind);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_prod_set(uint32_t word_idx, uint8_t prodc)
{
    uint32_t reg_prod=0;

#ifdef VALIDATE_PARMS
    if((word_idx >= 16))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_prod = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, PROD, PRODC, reg_prod, prodc);

    RU_REG_RAM_WRITE(0, word_idx, GPON_TX_GENERAL_CONFIGURATION, PROD, reg_prod);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_prod_get(uint32_t word_idx, uint8_t *prodc)
{
    uint32_t reg_prod=0;

#ifdef VALIDATE_PARMS
    if(!prodc)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((word_idx >= 16))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, word_idx, GPON_TX_GENERAL_CONFIGURATION, PROD, reg_prod);

    *prodc = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, PROD, PRODC, reg_prod);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_prods_set(uint8_t prcl, uint8_t prl)
{
    uint32_t reg_prods=0;

#ifdef VALIDATE_PARMS
    if((prcl >= _5BITS_MAX_VAL_) ||
       (prl >= _7BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_prods = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, PRODS, PRCL, reg_prods, prcl);
    reg_prods = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, PRODS, PRL, reg_prods, prl);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, PRODS, reg_prods);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_prods_get(uint8_t *prcl, uint8_t *prl)
{
    uint32_t reg_prods=0;

#ifdef VALIDATE_PARMS
    if(!prcl || !prl)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, PRODS, reg_prods);

    *prcl = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, PRODS, PRCL, reg_prods);
    *prl = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, PRODS, PRL, reg_prods);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_prrb_set(uint8_t brc)
{
    uint32_t reg_prrb=0;

#ifdef VALIDATE_PARMS
#endif

    reg_prrb = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, PRRB, BRC, reg_prrb, brc);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, PRRB, reg_prrb);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_prrb_get(uint8_t *brc)
{
    uint32_t reg_prrb=0;

#ifdef VALIDATE_PARMS
    if(!brc)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, PRRB, reg_prrb);

    *brc = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, PRRB, BRC, reg_prrb);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_delc_set(uint32_t deloc)
{
    uint32_t reg_delc=0;

#ifdef VALIDATE_PARMS
#endif

    reg_delc = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, DELC, DELOC, reg_delc, deloc);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, DELC, reg_delc);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_delc_get(uint32_t *deloc)
{
    uint32_t reg_delc=0;

#ifdef VALIDATE_PARMS
    if(!deloc)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, DELC, reg_delc);

    *deloc = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, DELC, DELOC, reg_delc);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_miscac1_set(uint16_t msstop, uint16_t msstart)
{
    uint32_t reg_miscac1=0;

#ifdef VALIDATE_PARMS
#endif

    reg_miscac1 = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, MISCAC1, MSSTOP, reg_miscac1, msstop);
    reg_miscac1 = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, MISCAC1, MSSTART, reg_miscac1, msstart);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, MISCAC1, reg_miscac1);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_miscac1_get(uint16_t *msstop, uint16_t *msstart)
{
    uint32_t reg_miscac1=0;

#ifdef VALIDATE_PARMS
    if(!msstop || !msstart)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, MISCAC1, reg_miscac1);

    *msstop = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, MISCAC1, MSSTOP, reg_miscac1);
    *msstart = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, MISCAC1, MSSTART, reg_miscac1);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_miscac2_set(uint8_t ploamu, uint8_t dbru, uint8_t fecu, uint8_t qu)
{
    uint32_t reg_miscac2=0;

#ifdef VALIDATE_PARMS
    if((ploamu >= _1BITS_MAX_VAL_) ||
       (dbru >= _2BITS_MAX_VAL_) ||
       (fecu >= _1BITS_MAX_VAL_) ||
       (qu >= _6BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_miscac2 = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, MISCAC2, PLOAMU, reg_miscac2, ploamu);
    reg_miscac2 = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, MISCAC2, DBRU, reg_miscac2, dbru);
    reg_miscac2 = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, MISCAC2, FECU, reg_miscac2, fecu);
    reg_miscac2 = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, MISCAC2, QU, reg_miscac2, qu);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, MISCAC2, reg_miscac2);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_miscac2_get(uint8_t *ploamu, uint8_t *dbru, uint8_t *fecu, uint8_t *qu)
{
    uint32_t reg_miscac2=0;

#ifdef VALIDATE_PARMS
    if(!ploamu || !dbru || !fecu || !qu)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, MISCAC2, reg_miscac2);

    *ploamu = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, MISCAC2, PLOAMU, reg_miscac2);
    *dbru = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, MISCAC2, DBRU, reg_miscac2);
    *fecu = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, MISCAC2, FECU, reg_miscac2);
    *qu = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, MISCAC2, QU, reg_miscac2);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_esall_set(uint8_t esa)
{
    uint32_t reg_esall=0;

#ifdef VALIDATE_PARMS
    if((esa >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_esall = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, ESALL, ESA, reg_esall, esa);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, ESALL, reg_esall);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_esall_get(uint8_t *esa)
{
    uint32_t reg_esall=0;

#ifdef VALIDATE_PARMS
    if(!esa)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, ESALL, reg_esall);

    *esa = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, ESALL, ESA, reg_esall);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_qforcntr_set(uint8_t qforgrp0, uint8_t qforgrp1, uint8_t qforgrp2, uint8_t qforgrp3)
{
    uint32_t reg_qforcntr=0;

#ifdef VALIDATE_PARMS
    if((qforgrp0 >= _6BITS_MAX_VAL_) ||
       (qforgrp1 >= _6BITS_MAX_VAL_) ||
       (qforgrp2 >= _6BITS_MAX_VAL_) ||
       (qforgrp3 >= _6BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_qforcntr = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, QFORCNTR, QFORGRP0, reg_qforcntr, qforgrp0);
    reg_qforcntr = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, QFORCNTR, QFORGRP1, reg_qforcntr, qforgrp1);
    reg_qforcntr = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, QFORCNTR, QFORGRP2, reg_qforcntr, qforgrp2);
    reg_qforcntr = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, QFORCNTR, QFORGRP3, reg_qforcntr, qforgrp3);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, QFORCNTR, reg_qforcntr);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_qforcntr_get(uint8_t *qforgrp0, uint8_t *qforgrp1, uint8_t *qforgrp2, uint8_t *qforgrp3)
{
    uint32_t reg_qforcntr=0;

#ifdef VALIDATE_PARMS
    if(!qforgrp0 || !qforgrp1 || !qforgrp2 || !qforgrp3)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, QFORCNTR, reg_qforcntr);

    *qforgrp0 = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, QFORCNTR, QFORGRP0, reg_qforcntr);
    *qforgrp1 = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, QFORCNTR, QFORGRP1, reg_qforcntr);
    *qforgrp2 = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, QFORCNTR, QFORGRP2, reg_qforcntr);
    *qforgrp3 = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, QFORCNTR, QFORGRP3, reg_qforcntr);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_plss_set(uint32_t plspat)
{
    uint32_t reg_plss=0;

#ifdef VALIDATE_PARMS
#endif

    reg_plss = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, PLSS, PLSPAT, reg_plss, plspat);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, PLSS, reg_plss);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_plss_get(uint32_t *plspat)
{
    uint32_t reg_plss=0;

#ifdef VALIDATE_PARMS
    if(!plspat)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, PLSS, reg_plss);

    *plspat = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, PLSS, PLSPAT, reg_plss);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_plsrs_set(uint8_t plsr)
{
    uint32_t reg_plsrs=0;

#ifdef VALIDATE_PARMS
    if((plsr >= _7BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_plsrs = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, PLSRS, PLSR, reg_plsrs, plsr);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, PLSRS, reg_plsrs);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_plsrs_get(uint8_t *plsr)
{
    uint32_t reg_plsrs=0;

#ifdef VALIDATE_PARMS
    if(!plsr)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, PLSRS, reg_plsrs);

    *plsr = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, PLSRS, PLSR, reg_plsrs);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_bips_set(uint16_t bipenum, uint8_t bipm, uint8_t bipev, uint8_t bipef)
{
    uint32_t reg_bips=0;

#ifdef VALIDATE_PARMS
    if((bipev >= _1BITS_MAX_VAL_) ||
       (bipef >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_bips = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, BIPS, BIPENUM, reg_bips, bipenum);
    reg_bips = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, BIPS, BIPM, reg_bips, bipm);
    reg_bips = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, BIPS, BIPEV, reg_bips, bipev);
    reg_bips = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, BIPS, BIPEF, reg_bips, bipef);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, BIPS, reg_bips);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_bips_get(uint16_t *bipenum, uint8_t *bipm, uint8_t *bipev, uint8_t *bipef)
{
    uint32_t reg_bips=0;

#ifdef VALIDATE_PARMS
    if(!bipenum || !bipm || !bipev || !bipef)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, BIPS, reg_bips);

    *bipenum = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, BIPS, BIPENUM, reg_bips);
    *bipm = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, BIPS, BIPM, reg_bips);
    *bipev = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, BIPS, BIPEV, reg_bips);
    *bipef = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, BIPS, BIPEF, reg_bips);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_crcoff_set(uint8_t crcoc)
{
    uint32_t reg_crcoff=0;

#ifdef VALIDATE_PARMS
    if((crcoc >= _7BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_crcoff = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, CRCOFF, CRCOC, reg_crcoff, crcoc);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, CRCOFF, reg_crcoff);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_crcoff_get(uint8_t *crcoc)
{
    uint32_t reg_crcoff=0;

#ifdef VALIDATE_PARMS
    if(!crcoc)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, CRCOFF, reg_crcoff);

    *crcoc = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, CRCOFF, CRCOC, reg_crcoff);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_flush_set(uint8_t flq, uint8_t flen, uint8_t flimm)
{
    uint32_t reg_flush=0;

#ifdef VALIDATE_PARMS
    if((flq >= _6BITS_MAX_VAL_) ||
       (flen >= _1BITS_MAX_VAL_) ||
       (flimm >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_flush = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, FLUSH, FLQ, reg_flush, flq);
    reg_flush = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, FLUSH, FLEN, reg_flush, flen);
    reg_flush = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, FLUSH, FLIMM, reg_flush, flimm);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, FLUSH, reg_flush);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_flush_get(uint8_t *flq, uint8_t *flen, uint8_t *flimm)
{
    uint32_t reg_flush=0;

#ifdef VALIDATE_PARMS
    if(!flq || !flen || !flimm)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, FLUSH, reg_flush);

    *flq = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, FLUSH, FLQ, reg_flush);
    *flen = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, FLUSH, FLEN, reg_flush);
    *flimm = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, FLUSH, FLIMM, reg_flush);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_flshdn_get(uint8_t *fldn)
{
    uint32_t reg_flshdn=0;

#ifdef VALIDATE_PARMS
    if(!fldn)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, FLSHDN, reg_flshdn);

    *fldn = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, FLSHDN, FLDN, reg_flshdn);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_rdyind_set(uint8_t set, uint8_t rdy)
{
    uint32_t reg_rdyind=0;

#ifdef VALIDATE_PARMS
    if((set >= _1BITS_MAX_VAL_) ||
       (rdy >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_rdyind = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, RDYIND, SET, reg_rdyind, set);
    reg_rdyind = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, RDYIND, RDY, reg_rdyind, rdy);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, RDYIND, reg_rdyind);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_rdyind_get(uint8_t *set, uint8_t *rdy)
{
    uint32_t reg_rdyind=0;

#ifdef VALIDATE_PARMS
    if(!set || !rdy)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, RDYIND, reg_rdyind);

    *set = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, RDYIND, SET, reg_rdyind);
    *rdy = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, RDYIND, RDY, reg_rdyind);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_dvslp_set(uint8_t dvpol, uint8_t dvexten)
{
    uint32_t reg_dvslp=0;

#ifdef VALIDATE_PARMS
    if((dvpol >= _1BITS_MAX_VAL_) ||
       (dvexten >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_dvslp = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, DVSLP, DVPOL, reg_dvslp, dvpol);
    reg_dvslp = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, DVSLP, DVEXTEN, reg_dvslp, dvexten);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, DVSLP, reg_dvslp);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_dvslp_get(uint8_t *dvpol, uint8_t *dvexten)
{
    uint32_t reg_dvslp=0;

#ifdef VALIDATE_PARMS
    if(!dvpol || !dvexten)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, DVSLP, reg_dvslp);

    *dvpol = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, DVSLP, DVPOL, reg_dvslp);
    *dvexten = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, DVSLP, DVEXTEN, reg_dvslp);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_dvstp_set(uint32_t dvstu)
{
    uint32_t reg_dvstp=0;

#ifdef VALIDATE_PARMS
    if((dvstu >= _24BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_dvstp = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, DVSTP, DVSTU, reg_dvstp, dvstu);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, DVSTP, reg_dvstp);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_dvstp_get(uint32_t *dvstu)
{
    uint32_t reg_dvstp=0;

#ifdef VALIDATE_PARMS
    if(!dvstu)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, DVSTP, reg_dvstp);

    *dvstu = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, DVSTP, DVSTU, reg_dvstp);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_dvhld_set(uint32_t dvhld)
{
    uint32_t reg_dvhld=0;

#ifdef VALIDATE_PARMS
    if((dvhld >= _24BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_dvhld = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, DVHLD, DVHLD, reg_dvhld, dvhld);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, DVHLD, reg_dvhld);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_dvhld_get(uint32_t *dvhld)
{
    uint32_t reg_dvhld=0;

#ifdef VALIDATE_PARMS
    if(!dvhld)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, DVHLD, reg_dvhld);

    *dvhld = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, DVHLD, DVHLD, reg_dvhld);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_dpen_set(uint8_t dpen, uint8_t prbs, uint8_t stplen, uint8_t hldlen)
{
    uint32_t reg_dpen=0;

#ifdef VALIDATE_PARMS
    if((dpen >= _1BITS_MAX_VAL_) ||
       (prbs >= _1BITS_MAX_VAL_) ||
       (stplen >= _3BITS_MAX_VAL_) ||
       (hldlen >= _3BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_dpen = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, DPEN, DPEN, reg_dpen, dpen);
    reg_dpen = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, DPEN, PRBS, reg_dpen, prbs);
    reg_dpen = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, DPEN, STPLEN, reg_dpen, stplen);
    reg_dpen = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, DPEN, HLDLEN, reg_dpen, hldlen);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, DPEN, reg_dpen);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_dpen_get(uint8_t *dpen, uint8_t *prbs, uint8_t *stplen, uint8_t *hldlen)
{
    uint32_t reg_dpen=0;

#ifdef VALIDATE_PARMS
    if(!dpen || !prbs || !stplen || !hldlen)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, DPEN, reg_dpen);

    *dpen = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, DPEN, DPEN, reg_dpen);
    *prbs = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, DPEN, PRBS, reg_dpen);
    *stplen = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, DPEN, STPLEN, reg_dpen);
    *hldlen = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, DPEN, HLDLEN, reg_dpen);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_dstp_set(uint32_t stppat)
{
    uint32_t reg_dstp=0;

#ifdef VALIDATE_PARMS
#endif

    reg_dstp = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, DSTP, STPPAT, reg_dstp, stppat);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, DSTP, reg_dstp);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_dstp_get(uint32_t *stppat)
{
    uint32_t reg_dstp=0;

#ifdef VALIDATE_PARMS
    if(!stppat)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, DSTP, reg_dstp);

    *stppat = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, DSTP, STPPAT, reg_dstp);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_dhld_set(uint32_t hldpat)
{
    uint32_t reg_dhld=0;

#ifdef VALIDATE_PARMS
#endif

    reg_dhld = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, DHLD, HLDPAT, reg_dhld, hldpat);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, DHLD, reg_dhld);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_dhld_get(uint32_t *hldpat)
{
    uint32_t reg_dhld=0;

#ifdef VALIDATE_PARMS
    if(!hldpat)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, DHLD, reg_dhld);

    *hldpat = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, DHLD, HLDPAT, reg_dhld);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_rogue_onu_en_set(uint8_t rogue_level_clr, uint8_t rogue_diff_clr, uint8_t rogue_dv_sel)
{
    uint32_t reg_rogue_onu_en=0;

#ifdef VALIDATE_PARMS
    if((rogue_level_clr >= _1BITS_MAX_VAL_) ||
       (rogue_diff_clr >= _1BITS_MAX_VAL_) ||
       (rogue_dv_sel >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_rogue_onu_en = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, ROGUE_ONU_EN, ROGUE_LEVEL_CLR, reg_rogue_onu_en, rogue_level_clr);
    reg_rogue_onu_en = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, ROGUE_ONU_EN, ROGUE_DIFF_CLR, reg_rogue_onu_en, rogue_diff_clr);
    reg_rogue_onu_en = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, ROGUE_ONU_EN, ROGUE_DV_SEL, reg_rogue_onu_en, rogue_dv_sel);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, ROGUE_ONU_EN, reg_rogue_onu_en);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_rogue_onu_en_get(uint8_t *rogue_level_clr, uint8_t *rogue_diff_clr, uint8_t *rogue_dv_sel)
{
    uint32_t reg_rogue_onu_en=0;

#ifdef VALIDATE_PARMS
    if(!rogue_level_clr || !rogue_diff_clr || !rogue_dv_sel)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, ROGUE_ONU_EN, reg_rogue_onu_en);

    *rogue_level_clr = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, ROGUE_ONU_EN, ROGUE_LEVEL_CLR, reg_rogue_onu_en);
    *rogue_diff_clr = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, ROGUE_ONU_EN, ROGUE_DIFF_CLR, reg_rogue_onu_en);
    *rogue_dv_sel = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, ROGUE_ONU_EN, ROGUE_DV_SEL, reg_rogue_onu_en);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_rogue_level_time_set(uint32_t window_size)
{
    uint32_t reg_rogue_level_time=0;

#ifdef VALIDATE_PARMS
#endif

    reg_rogue_level_time = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, ROGUE_LEVEL_TIME, WINDOW_SIZE, reg_rogue_level_time, window_size);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, ROGUE_LEVEL_TIME, reg_rogue_level_time);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_rogue_level_time_get(uint32_t *window_size)
{
    uint32_t reg_rogue_level_time=0;

#ifdef VALIDATE_PARMS
    if(!window_size)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, ROGUE_LEVEL_TIME, reg_rogue_level_time);

    *window_size = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, ROGUE_LEVEL_TIME, WINDOW_SIZE, reg_rogue_level_time);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_rogue_diff_time_set(uint16_t window_size)
{
    uint32_t reg_rogue_diff_time=0;

#ifdef VALIDATE_PARMS
    if((window_size >= _10BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_rogue_diff_time = RU_FIELD_SET(0, GPON_TX_GENERAL_CONFIGURATION, ROGUE_DIFF_TIME, WINDOW_SIZE, reg_rogue_diff_time, window_size);

    RU_REG_WRITE(0, GPON_TX_GENERAL_CONFIGURATION, ROGUE_DIFF_TIME, reg_rogue_diff_time);

    return 0;
}

int ag_drv_gpon_tx_general_configuration_rogue_diff_time_get(uint16_t *window_size)
{
    uint32_t reg_rogue_diff_time=0;

#ifdef VALIDATE_PARMS
    if(!window_size)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_GENERAL_CONFIGURATION, ROGUE_DIFF_TIME, reg_rogue_diff_time);

    *window_size = RU_FIELD_GET(0, GPON_TX_GENERAL_CONFIGURATION, ROGUE_DIFF_TIME, WINDOW_SIZE, reg_rogue_diff_time);

    return 0;
}


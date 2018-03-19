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
#include "ngpon_tx_int.h"
int ag_drv_ngpon_tx_int_isr0_set(uint8_t tx_tcont_dbr_idx, uint8_t is_tx_tcont_dbr_int_active)
{
    uint32_t reg_isr0=0;

#ifdef VALIDATE_PARMS
    if((is_tx_tcont_dbr_int_active >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_READ(0, NGPON_TX_INT, ISR0, reg_isr0);

    FIELD_SET(reg_isr0, (tx_tcont_dbr_idx % 32) *1, 0x1, is_tx_tcont_dbr_int_active);

    RU_REG_WRITE(0, NGPON_TX_INT, ISR0, reg_isr0);

    return 0;
}

int ag_drv_ngpon_tx_int_isr0_get(uint8_t tx_tcont_dbr_idx, uint8_t *is_tx_tcont_dbr_int_active)
{
    uint32_t reg_isr0=0;

#ifdef VALIDATE_PARMS
    if(!is_tx_tcont_dbr_int_active)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_INT, ISR0, reg_isr0);

    *is_tx_tcont_dbr_int_active = FIELD_GET(reg_isr0, (tx_tcont_dbr_idx % 32) *1, 0x1);

    return 0;
}

int ag_drv_ngpon_tx_int_ism0_get(uint8_t tx_tcont_dbr_idx, uint8_t *is_tx_tcont_dbr_int_mask)
{
    uint32_t reg_ism0=0;

#ifdef VALIDATE_PARMS
    if(!is_tx_tcont_dbr_int_mask)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_INT, ISM0, reg_ism0);

    *is_tx_tcont_dbr_int_mask = FIELD_GET(reg_ism0, (tx_tcont_dbr_idx % 32) *1, 0x1);

    return 0;
}

int ag_drv_ngpon_tx_int_ier0_set(uint8_t tx_tcont_dbr_idx, uint8_t is_tx_tcont_dbr_int_ena)
{
    uint32_t reg_ier0=0;

#ifdef VALIDATE_PARMS
    if((is_tx_tcont_dbr_int_ena >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_READ(0, NGPON_TX_INT, IER0, reg_ier0);

    FIELD_SET(reg_ier0, (tx_tcont_dbr_idx % 32) *1, 0x1, is_tx_tcont_dbr_int_ena);

    RU_REG_WRITE(0, NGPON_TX_INT, IER0, reg_ier0);

    return 0;
}

int ag_drv_ngpon_tx_int_ier0_get(uint8_t tx_tcont_dbr_idx, uint8_t *is_tx_tcont_dbr_int_ena)
{
    uint32_t reg_ier0=0;

#ifdef VALIDATE_PARMS
    if(!is_tx_tcont_dbr_int_ena)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_INT, IER0, reg_ier0);

    *is_tx_tcont_dbr_int_ena = FIELD_GET(reg_ier0, (tx_tcont_dbr_idx % 32) *1, 0x1);

    return 0;
}

int ag_drv_ngpon_tx_int_itr0_set(uint8_t tx_tcont_dbr_idx, uint8_t is_tx_tcont_dbr_int_test)
{
    uint32_t reg_itr0=0;

#ifdef VALIDATE_PARMS
    if((is_tx_tcont_dbr_int_test >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_READ(0, NGPON_TX_INT, ITR0, reg_itr0);

    FIELD_SET(reg_itr0, (tx_tcont_dbr_idx % 32) *1, 0x1, is_tx_tcont_dbr_int_test);

    RU_REG_WRITE(0, NGPON_TX_INT, ITR0, reg_itr0);

    return 0;
}

int ag_drv_ngpon_tx_int_itr0_get(uint8_t tx_tcont_dbr_idx, uint8_t *is_tx_tcont_dbr_int_test)
{
    uint32_t reg_itr0=0;

#ifdef VALIDATE_PARMS
    if(!is_tx_tcont_dbr_int_test)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_INT, ITR0, reg_itr0);

    *is_tx_tcont_dbr_int_test = FIELD_GET(reg_itr0, (tx_tcont_dbr_idx % 32) *1, 0x1);

    return 0;
}

int ag_drv_ngpon_tx_int_isr1_set(const ngpon_tx_int_isr1 *isr1)
{
    uint32_t reg_isr1=0;

#ifdef VALIDATE_PARMS
    if(!isr1)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, 1);
        return 1;
    }
    if((isr1->tx_plm_0 >= _1BITS_MAX_VAL_) ||
       (isr1->tx_plm_1 >= _1BITS_MAX_VAL_) ||
       (isr1->tx_plm_2 >= _1BITS_MAX_VAL_) ||
       (isr1->fe_data_overun >= _1BITS_MAX_VAL_) ||
       (isr1->pd_underun >= _1BITS_MAX_VAL_) ||
       (isr1->pd_overun >= _1BITS_MAX_VAL_) ||
       (isr1->af_err >= _1BITS_MAX_VAL_) ||
       (isr1->rog_dif >= _1BITS_MAX_VAL_) ||
       (isr1->rog_len >= _1BITS_MAX_VAL_) ||
       (isr1->tx_tcont_32_dbr >= _1BITS_MAX_VAL_) ||
       (isr1->tx_tcont_33_dbr >= _1BITS_MAX_VAL_) ||
       (isr1->tx_tcont_34_dbr >= _1BITS_MAX_VAL_) ||
       (isr1->tx_tcont_35_dbr >= _1BITS_MAX_VAL_) ||
       (isr1->tx_tcont_36_dbr >= _1BITS_MAX_VAL_) ||
       (isr1->tx_tcont_37_dbr >= _1BITS_MAX_VAL_) ||
       (isr1->tx_tcont_38_dbr >= _1BITS_MAX_VAL_) ||
       (isr1->tx_tcont_39_dbr >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_isr1 = RU_FIELD_SET(0, NGPON_TX_INT, ISR1, TX_PLM_0, reg_isr1, isr1->tx_plm_0);
    reg_isr1 = RU_FIELD_SET(0, NGPON_TX_INT, ISR1, TX_PLM_1, reg_isr1, isr1->tx_plm_1);
    reg_isr1 = RU_FIELD_SET(0, NGPON_TX_INT, ISR1, TX_PLM_2, reg_isr1, isr1->tx_plm_2);
    reg_isr1 = RU_FIELD_SET(0, NGPON_TX_INT, ISR1, FE_DATA_OVERUN, reg_isr1, isr1->fe_data_overun);
    reg_isr1 = RU_FIELD_SET(0, NGPON_TX_INT, ISR1, PD_UNDERUN, reg_isr1, isr1->pd_underun);
    reg_isr1 = RU_FIELD_SET(0, NGPON_TX_INT, ISR1, PD_OVERUN, reg_isr1, isr1->pd_overun);
    reg_isr1 = RU_FIELD_SET(0, NGPON_TX_INT, ISR1, AF_ERR, reg_isr1, isr1->af_err);
    reg_isr1 = RU_FIELD_SET(0, NGPON_TX_INT, ISR1, ROG_DIF, reg_isr1, isr1->rog_dif);
    reg_isr1 = RU_FIELD_SET(0, NGPON_TX_INT, ISR1, ROG_LEN, reg_isr1, isr1->rog_len);
    reg_isr1 = RU_FIELD_SET(0, NGPON_TX_INT, ISR1, TX_TCONT_32_DBR, reg_isr1, isr1->tx_tcont_32_dbr);
    reg_isr1 = RU_FIELD_SET(0, NGPON_TX_INT, ISR1, TX_TCONT_33_DBR, reg_isr1, isr1->tx_tcont_33_dbr);
    reg_isr1 = RU_FIELD_SET(0, NGPON_TX_INT, ISR1, TX_TCONT_34_DBR, reg_isr1, isr1->tx_tcont_34_dbr);
    reg_isr1 = RU_FIELD_SET(0, NGPON_TX_INT, ISR1, TX_TCONT_35_DBR, reg_isr1, isr1->tx_tcont_35_dbr);
    reg_isr1 = RU_FIELD_SET(0, NGPON_TX_INT, ISR1, TX_TCONT_36_DBR, reg_isr1, isr1->tx_tcont_36_dbr);
    reg_isr1 = RU_FIELD_SET(0, NGPON_TX_INT, ISR1, TX_TCONT_37_DBR, reg_isr1, isr1->tx_tcont_37_dbr);
    reg_isr1 = RU_FIELD_SET(0, NGPON_TX_INT, ISR1, TX_TCONT_38_DBR, reg_isr1, isr1->tx_tcont_38_dbr);
    reg_isr1 = RU_FIELD_SET(0, NGPON_TX_INT, ISR1, TX_TCONT_39_DBR, reg_isr1, isr1->tx_tcont_39_dbr);

    RU_REG_WRITE(0, NGPON_TX_INT, ISR1, reg_isr1);

    return 0;
}

int ag_drv_ngpon_tx_int_isr1_get(ngpon_tx_int_isr1 *isr1)
{
    uint32_t reg_isr1=0;

#ifdef VALIDATE_PARMS
    if(!isr1)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_INT, ISR1, reg_isr1);

    isr1->tx_plm_0 = RU_FIELD_GET(0, NGPON_TX_INT, ISR1, TX_PLM_0, reg_isr1);
    isr1->tx_plm_1 = RU_FIELD_GET(0, NGPON_TX_INT, ISR1, TX_PLM_1, reg_isr1);
    isr1->tx_plm_2 = RU_FIELD_GET(0, NGPON_TX_INT, ISR1, TX_PLM_2, reg_isr1);
    isr1->fe_data_overun = RU_FIELD_GET(0, NGPON_TX_INT, ISR1, FE_DATA_OVERUN, reg_isr1);
    isr1->pd_underun = RU_FIELD_GET(0, NGPON_TX_INT, ISR1, PD_UNDERUN, reg_isr1);
    isr1->pd_overun = RU_FIELD_GET(0, NGPON_TX_INT, ISR1, PD_OVERUN, reg_isr1);
    isr1->af_err = RU_FIELD_GET(0, NGPON_TX_INT, ISR1, AF_ERR, reg_isr1);
    isr1->rog_dif = RU_FIELD_GET(0, NGPON_TX_INT, ISR1, ROG_DIF, reg_isr1);
    isr1->rog_len = RU_FIELD_GET(0, NGPON_TX_INT, ISR1, ROG_LEN, reg_isr1);
    isr1->tx_tcont_32_dbr = RU_FIELD_GET(0, NGPON_TX_INT, ISR1, TX_TCONT_32_DBR, reg_isr1);
    isr1->tx_tcont_33_dbr = RU_FIELD_GET(0, NGPON_TX_INT, ISR1, TX_TCONT_33_DBR, reg_isr1);
    isr1->tx_tcont_34_dbr = RU_FIELD_GET(0, NGPON_TX_INT, ISR1, TX_TCONT_34_DBR, reg_isr1);
    isr1->tx_tcont_35_dbr = RU_FIELD_GET(0, NGPON_TX_INT, ISR1, TX_TCONT_35_DBR, reg_isr1);
    isr1->tx_tcont_36_dbr = RU_FIELD_GET(0, NGPON_TX_INT, ISR1, TX_TCONT_36_DBR, reg_isr1);
    isr1->tx_tcont_37_dbr = RU_FIELD_GET(0, NGPON_TX_INT, ISR1, TX_TCONT_37_DBR, reg_isr1);
    isr1->tx_tcont_38_dbr = RU_FIELD_GET(0, NGPON_TX_INT, ISR1, TX_TCONT_38_DBR, reg_isr1);
    isr1->tx_tcont_39_dbr = RU_FIELD_GET(0, NGPON_TX_INT, ISR1, TX_TCONT_39_DBR, reg_isr1);

    return 0;
}

int ag_drv_ngpon_tx_int_ism1_get(ngpon_tx_int_ism1 *ism1)
{
    uint32_t reg_ism1=0;

#ifdef VALIDATE_PARMS
    if(!ism1)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_INT, ISM1, reg_ism1);

    ism1->tx_plm_0 = RU_FIELD_GET(0, NGPON_TX_INT, ISM1, TX_PLM_0, reg_ism1);
    ism1->tx_plm_1 = RU_FIELD_GET(0, NGPON_TX_INT, ISM1, TX_PLM_1, reg_ism1);
    ism1->tx_plm_2 = RU_FIELD_GET(0, NGPON_TX_INT, ISM1, TX_PLM_2, reg_ism1);
    ism1->fe_data_overun = RU_FIELD_GET(0, NGPON_TX_INT, ISM1, FE_DATA_OVERUN, reg_ism1);
    ism1->pd_underun = RU_FIELD_GET(0, NGPON_TX_INT, ISM1, PD_UNDERUN, reg_ism1);
    ism1->pd_overun = RU_FIELD_GET(0, NGPON_TX_INT, ISM1, PD_OVERUN, reg_ism1);
    ism1->af_err = RU_FIELD_GET(0, NGPON_TX_INT, ISM1, AF_ERR, reg_ism1);
    ism1->rog_dif = RU_FIELD_GET(0, NGPON_TX_INT, ISM1, ROG_DIF, reg_ism1);
    ism1->rog_len = RU_FIELD_GET(0, NGPON_TX_INT, ISM1, ROG_LEN, reg_ism1);
    ism1->tx_tcont_32_dbr = RU_FIELD_GET(0, NGPON_TX_INT, ISM1, TX_TCONT_32_DBR, reg_ism1);
    ism1->tx_tcont_33_dbr = RU_FIELD_GET(0, NGPON_TX_INT, ISM1, TX_TCONT_33_DBR, reg_ism1);
    ism1->tx_tcont_34_dbr = RU_FIELD_GET(0, NGPON_TX_INT, ISM1, TX_TCONT_34_DBR, reg_ism1);
    ism1->tx_tcont_35_dbr = RU_FIELD_GET(0, NGPON_TX_INT, ISM1, TX_TCONT_35_DBR, reg_ism1);
    ism1->tx_tcont_36_dbr = RU_FIELD_GET(0, NGPON_TX_INT, ISM1, TX_TCONT_36_DBR, reg_ism1);
    ism1->tx_tcont_37_dbr = RU_FIELD_GET(0, NGPON_TX_INT, ISM1, TX_TCONT_37_DBR, reg_ism1);
    ism1->tx_tcont_38_dbr = RU_FIELD_GET(0, NGPON_TX_INT, ISM1, TX_TCONT_38_DBR, reg_ism1);
    ism1->tx_tcont_39_dbr = RU_FIELD_GET(0, NGPON_TX_INT, ISM1, TX_TCONT_39_DBR, reg_ism1);

    return 0;
}

int ag_drv_ngpon_tx_int_ier1_set(const ngpon_tx_int_ier1 *ier1)
{
    uint32_t reg_ier1=0;

#ifdef VALIDATE_PARMS
    if(!ier1)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, 1);
        return 1;
    }
    if((ier1->tx_plm_0 >= _1BITS_MAX_VAL_) ||
       (ier1->tx_plm_1 >= _1BITS_MAX_VAL_) ||
       (ier1->tx_plm_2 >= _1BITS_MAX_VAL_) ||
       (ier1->fe_data_overun >= _1BITS_MAX_VAL_) ||
       (ier1->pd_underun >= _1BITS_MAX_VAL_) ||
       (ier1->pd_overun >= _1BITS_MAX_VAL_) ||
       (ier1->af_err >= _1BITS_MAX_VAL_) ||
       (ier1->rog_dif >= _1BITS_MAX_VAL_) ||
       (ier1->rog_len >= _1BITS_MAX_VAL_) ||
       (ier1->tx_tcont_32_dbr >= _1BITS_MAX_VAL_) ||
       (ier1->tx_tcont_33_dbr >= _1BITS_MAX_VAL_) ||
       (ier1->tx_tcont_34_dbr >= _1BITS_MAX_VAL_) ||
       (ier1->tx_tcont_35_dbr >= _1BITS_MAX_VAL_) ||
       (ier1->tx_tcont_36_dbr >= _1BITS_MAX_VAL_) ||
       (ier1->tx_tcont_37_dbr >= _1BITS_MAX_VAL_) ||
       (ier1->tx_tcont_38_dbr >= _1BITS_MAX_VAL_) ||
       (ier1->tx_tcont_39_dbr >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_ier1 = RU_FIELD_SET(0, NGPON_TX_INT, IER1, TX_PLM_0, reg_ier1, ier1->tx_plm_0);
    reg_ier1 = RU_FIELD_SET(0, NGPON_TX_INT, IER1, TX_PLM_1, reg_ier1, ier1->tx_plm_1);
    reg_ier1 = RU_FIELD_SET(0, NGPON_TX_INT, IER1, TX_PLM_2, reg_ier1, ier1->tx_plm_2);
    reg_ier1 = RU_FIELD_SET(0, NGPON_TX_INT, IER1, FE_DATA_OVERUN, reg_ier1, ier1->fe_data_overun);
    reg_ier1 = RU_FIELD_SET(0, NGPON_TX_INT, IER1, PD_UNDERUN, reg_ier1, ier1->pd_underun);
    reg_ier1 = RU_FIELD_SET(0, NGPON_TX_INT, IER1, PD_OVERUN, reg_ier1, ier1->pd_overun);
    reg_ier1 = RU_FIELD_SET(0, NGPON_TX_INT, IER1, AF_ERR, reg_ier1, ier1->af_err);
    reg_ier1 = RU_FIELD_SET(0, NGPON_TX_INT, IER1, ROG_DIF, reg_ier1, ier1->rog_dif);
    reg_ier1 = RU_FIELD_SET(0, NGPON_TX_INT, IER1, ROG_LEN, reg_ier1, ier1->rog_len);
    reg_ier1 = RU_FIELD_SET(0, NGPON_TX_INT, IER1, TX_TCONT_32_DBR, reg_ier1, ier1->tx_tcont_32_dbr);
    reg_ier1 = RU_FIELD_SET(0, NGPON_TX_INT, IER1, TX_TCONT_33_DBR, reg_ier1, ier1->tx_tcont_33_dbr);
    reg_ier1 = RU_FIELD_SET(0, NGPON_TX_INT, IER1, TX_TCONT_34_DBR, reg_ier1, ier1->tx_tcont_34_dbr);
    reg_ier1 = RU_FIELD_SET(0, NGPON_TX_INT, IER1, TX_TCONT_35_DBR, reg_ier1, ier1->tx_tcont_35_dbr);
    reg_ier1 = RU_FIELD_SET(0, NGPON_TX_INT, IER1, TX_TCONT_36_DBR, reg_ier1, ier1->tx_tcont_36_dbr);
    reg_ier1 = RU_FIELD_SET(0, NGPON_TX_INT, IER1, TX_TCONT_37_DBR, reg_ier1, ier1->tx_tcont_37_dbr);
    reg_ier1 = RU_FIELD_SET(0, NGPON_TX_INT, IER1, TX_TCONT_38_DBR, reg_ier1, ier1->tx_tcont_38_dbr);
    reg_ier1 = RU_FIELD_SET(0, NGPON_TX_INT, IER1, TX_TCONT_39_DBR, reg_ier1, ier1->tx_tcont_39_dbr);

    RU_REG_WRITE(0, NGPON_TX_INT, IER1, reg_ier1);

    return 0;
}

int ag_drv_ngpon_tx_int_ier1_get(ngpon_tx_int_ier1 *ier1)
{
    uint32_t reg_ier1=0;

#ifdef VALIDATE_PARMS
    if(!ier1)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_INT, IER1, reg_ier1);

    ier1->tx_plm_0 = RU_FIELD_GET(0, NGPON_TX_INT, IER1, TX_PLM_0, reg_ier1);
    ier1->tx_plm_1 = RU_FIELD_GET(0, NGPON_TX_INT, IER1, TX_PLM_1, reg_ier1);
    ier1->tx_plm_2 = RU_FIELD_GET(0, NGPON_TX_INT, IER1, TX_PLM_2, reg_ier1);
    ier1->fe_data_overun = RU_FIELD_GET(0, NGPON_TX_INT, IER1, FE_DATA_OVERUN, reg_ier1);
    ier1->pd_underun = RU_FIELD_GET(0, NGPON_TX_INT, IER1, PD_UNDERUN, reg_ier1);
    ier1->pd_overun = RU_FIELD_GET(0, NGPON_TX_INT, IER1, PD_OVERUN, reg_ier1);
    ier1->af_err = RU_FIELD_GET(0, NGPON_TX_INT, IER1, AF_ERR, reg_ier1);
    ier1->rog_dif = RU_FIELD_GET(0, NGPON_TX_INT, IER1, ROG_DIF, reg_ier1);
    ier1->rog_len = RU_FIELD_GET(0, NGPON_TX_INT, IER1, ROG_LEN, reg_ier1);
    ier1->tx_tcont_32_dbr = RU_FIELD_GET(0, NGPON_TX_INT, IER1, TX_TCONT_32_DBR, reg_ier1);
    ier1->tx_tcont_33_dbr = RU_FIELD_GET(0, NGPON_TX_INT, IER1, TX_TCONT_33_DBR, reg_ier1);
    ier1->tx_tcont_34_dbr = RU_FIELD_GET(0, NGPON_TX_INT, IER1, TX_TCONT_34_DBR, reg_ier1);
    ier1->tx_tcont_35_dbr = RU_FIELD_GET(0, NGPON_TX_INT, IER1, TX_TCONT_35_DBR, reg_ier1);
    ier1->tx_tcont_36_dbr = RU_FIELD_GET(0, NGPON_TX_INT, IER1, TX_TCONT_36_DBR, reg_ier1);
    ier1->tx_tcont_37_dbr = RU_FIELD_GET(0, NGPON_TX_INT, IER1, TX_TCONT_37_DBR, reg_ier1);
    ier1->tx_tcont_38_dbr = RU_FIELD_GET(0, NGPON_TX_INT, IER1, TX_TCONT_38_DBR, reg_ier1);
    ier1->tx_tcont_39_dbr = RU_FIELD_GET(0, NGPON_TX_INT, IER1, TX_TCONT_39_DBR, reg_ier1);

    return 0;
}

int ag_drv_ngpon_tx_int_itr1_set(const ngpon_tx_int_itr1 *itr1)
{
    uint32_t reg_itr1=0;

#ifdef VALIDATE_PARMS
    if(!itr1)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, 1);
        return 1;
    }
    if((itr1->tx_plm_0 >= _1BITS_MAX_VAL_) ||
       (itr1->tx_plm_1 >= _1BITS_MAX_VAL_) ||
       (itr1->tx_plm_2 >= _1BITS_MAX_VAL_) ||
       (itr1->fe_data_overun >= _1BITS_MAX_VAL_) ||
       (itr1->pd_underun >= _1BITS_MAX_VAL_) ||
       (itr1->pd_overun >= _1BITS_MAX_VAL_) ||
       (itr1->af_err >= _1BITS_MAX_VAL_) ||
       (itr1->rog_dif >= _1BITS_MAX_VAL_) ||
       (itr1->rog_len >= _1BITS_MAX_VAL_) ||
       (itr1->tx_tcont_32_dbr >= _1BITS_MAX_VAL_) ||
       (itr1->tx_tcont_33_dbr >= _1BITS_MAX_VAL_) ||
       (itr1->tx_tcont_34_dbr >= _1BITS_MAX_VAL_) ||
       (itr1->tx_tcont_35_dbr >= _1BITS_MAX_VAL_) ||
       (itr1->tx_tcont_36_dbr >= _1BITS_MAX_VAL_) ||
       (itr1->tx_tcont_37_dbr >= _1BITS_MAX_VAL_) ||
       (itr1->tx_tcont_38_dbr >= _1BITS_MAX_VAL_) ||
       (itr1->tx_tcont_39_dbr >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_itr1 = RU_FIELD_SET(0, NGPON_TX_INT, ITR1, TX_PLM_0, reg_itr1, itr1->tx_plm_0);
    reg_itr1 = RU_FIELD_SET(0, NGPON_TX_INT, ITR1, TX_PLM_1, reg_itr1, itr1->tx_plm_1);
    reg_itr1 = RU_FIELD_SET(0, NGPON_TX_INT, ITR1, TX_PLM_2, reg_itr1, itr1->tx_plm_2);
    reg_itr1 = RU_FIELD_SET(0, NGPON_TX_INT, ITR1, FE_DATA_OVERUN, reg_itr1, itr1->fe_data_overun);
    reg_itr1 = RU_FIELD_SET(0, NGPON_TX_INT, ITR1, PD_UNDERUN, reg_itr1, itr1->pd_underun);
    reg_itr1 = RU_FIELD_SET(0, NGPON_TX_INT, ITR1, PD_OVERUN, reg_itr1, itr1->pd_overun);
    reg_itr1 = RU_FIELD_SET(0, NGPON_TX_INT, ITR1, AF_ERR, reg_itr1, itr1->af_err);
    reg_itr1 = RU_FIELD_SET(0, NGPON_TX_INT, ITR1, ROG_DIF, reg_itr1, itr1->rog_dif);
    reg_itr1 = RU_FIELD_SET(0, NGPON_TX_INT, ITR1, ROG_LEN, reg_itr1, itr1->rog_len);
    reg_itr1 = RU_FIELD_SET(0, NGPON_TX_INT, ITR1, TX_TCONT_32_DBR, reg_itr1, itr1->tx_tcont_32_dbr);
    reg_itr1 = RU_FIELD_SET(0, NGPON_TX_INT, ITR1, TX_TCONT_33_DBR, reg_itr1, itr1->tx_tcont_33_dbr);
    reg_itr1 = RU_FIELD_SET(0, NGPON_TX_INT, ITR1, TX_TCONT_34_DBR, reg_itr1, itr1->tx_tcont_34_dbr);
    reg_itr1 = RU_FIELD_SET(0, NGPON_TX_INT, ITR1, TX_TCONT_35_DBR, reg_itr1, itr1->tx_tcont_35_dbr);
    reg_itr1 = RU_FIELD_SET(0, NGPON_TX_INT, ITR1, TX_TCONT_36_DBR, reg_itr1, itr1->tx_tcont_36_dbr);
    reg_itr1 = RU_FIELD_SET(0, NGPON_TX_INT, ITR1, TX_TCONT_37_DBR, reg_itr1, itr1->tx_tcont_37_dbr);
    reg_itr1 = RU_FIELD_SET(0, NGPON_TX_INT, ITR1, TX_TCONT_38_DBR, reg_itr1, itr1->tx_tcont_38_dbr);
    reg_itr1 = RU_FIELD_SET(0, NGPON_TX_INT, ITR1, TX_TCONT_39_DBR, reg_itr1, itr1->tx_tcont_39_dbr);

    RU_REG_WRITE(0, NGPON_TX_INT, ITR1, reg_itr1);

    return 0;
}

int ag_drv_ngpon_tx_int_itr1_get(ngpon_tx_int_itr1 *itr1)
{
    uint32_t reg_itr1=0;

#ifdef VALIDATE_PARMS
    if(!itr1)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_TX_INT, ITR1, reg_itr1);

    itr1->tx_plm_0 = RU_FIELD_GET(0, NGPON_TX_INT, ITR1, TX_PLM_0, reg_itr1);
    itr1->tx_plm_1 = RU_FIELD_GET(0, NGPON_TX_INT, ITR1, TX_PLM_1, reg_itr1);
    itr1->tx_plm_2 = RU_FIELD_GET(0, NGPON_TX_INT, ITR1, TX_PLM_2, reg_itr1);
    itr1->fe_data_overun = RU_FIELD_GET(0, NGPON_TX_INT, ITR1, FE_DATA_OVERUN, reg_itr1);
    itr1->pd_underun = RU_FIELD_GET(0, NGPON_TX_INT, ITR1, PD_UNDERUN, reg_itr1);
    itr1->pd_overun = RU_FIELD_GET(0, NGPON_TX_INT, ITR1, PD_OVERUN, reg_itr1);
    itr1->af_err = RU_FIELD_GET(0, NGPON_TX_INT, ITR1, AF_ERR, reg_itr1);
    itr1->rog_dif = RU_FIELD_GET(0, NGPON_TX_INT, ITR1, ROG_DIF, reg_itr1);
    itr1->rog_len = RU_FIELD_GET(0, NGPON_TX_INT, ITR1, ROG_LEN, reg_itr1);
    itr1->tx_tcont_32_dbr = RU_FIELD_GET(0, NGPON_TX_INT, ITR1, TX_TCONT_32_DBR, reg_itr1);
    itr1->tx_tcont_33_dbr = RU_FIELD_GET(0, NGPON_TX_INT, ITR1, TX_TCONT_33_DBR, reg_itr1);
    itr1->tx_tcont_34_dbr = RU_FIELD_GET(0, NGPON_TX_INT, ITR1, TX_TCONT_34_DBR, reg_itr1);
    itr1->tx_tcont_35_dbr = RU_FIELD_GET(0, NGPON_TX_INT, ITR1, TX_TCONT_35_DBR, reg_itr1);
    itr1->tx_tcont_36_dbr = RU_FIELD_GET(0, NGPON_TX_INT, ITR1, TX_TCONT_36_DBR, reg_itr1);
    itr1->tx_tcont_37_dbr = RU_FIELD_GET(0, NGPON_TX_INT, ITR1, TX_TCONT_37_DBR, reg_itr1);
    itr1->tx_tcont_38_dbr = RU_FIELD_GET(0, NGPON_TX_INT, ITR1, TX_TCONT_38_DBR, reg_itr1);
    itr1->tx_tcont_39_dbr = RU_FIELD_GET(0, NGPON_TX_INT, ITR1, TX_TCONT_39_DBR, reg_itr1);

    return 0;
}


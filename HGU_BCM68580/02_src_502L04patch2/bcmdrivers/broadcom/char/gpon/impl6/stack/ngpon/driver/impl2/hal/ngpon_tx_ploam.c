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
#include "ngpon_tx_ploam.h"
int ag_drv_ngpon_tx_ploam_0_buf_tx_plm_0_buf_plm_set(uint32_t ngpon_tx_ploam_buf_0_word, uint32_t value)
{
    uint32_t reg_0_buf_tx_plm_0_buf_plm=0;

#ifdef VALIDATE_PARMS
    if((ngpon_tx_ploam_buf_0_word >= 10))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_0_buf_tx_plm_0_buf_plm = RU_FIELD_SET(0, NGPON_TX_PLOAM, 0_BUF_TX_PLM_0_BUF_PLM, VALUE, reg_0_buf_tx_plm_0_buf_plm, value);

    RU_REG_RAM_WRITE(0, ngpon_tx_ploam_buf_0_word, NGPON_TX_PLOAM, 0_BUF_TX_PLM_0_BUF_PLM, reg_0_buf_tx_plm_0_buf_plm);

    return 0;
}

int ag_drv_ngpon_tx_ploam_0_buf_tx_plm_0_buf_plm_get(uint32_t ngpon_tx_ploam_buf_0_word, uint32_t *value)
{
    uint32_t reg_0_buf_tx_plm_0_buf_plm=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ngpon_tx_ploam_buf_0_word >= 10))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ngpon_tx_ploam_buf_0_word, NGPON_TX_PLOAM, 0_BUF_TX_PLM_0_BUF_PLM, reg_0_buf_tx_plm_0_buf_plm);

    *value = RU_FIELD_GET(0, NGPON_TX_PLOAM, 0_BUF_TX_PLM_0_BUF_PLM, VALUE, reg_0_buf_tx_plm_0_buf_plm);

    return 0;
}

int ag_drv_ngpon_tx_ploam_1_2_buf_tx_plm_1_2_buf_plm_0_set(uint32_t ngpon_tx_ploam_buf_1_2_word_0, uint32_t value)
{
    uint32_t reg_1_2_buf_tx_plm_1_2_buf_plm_0=0;

#ifdef VALIDATE_PARMS
    if((ngpon_tx_ploam_buf_1_2_word_0 >= 2))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_1_2_buf_tx_plm_1_2_buf_plm_0 = RU_FIELD_SET(0, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_0, VALUE, reg_1_2_buf_tx_plm_1_2_buf_plm_0, value);

    RU_REG_RAM_WRITE(0, ngpon_tx_ploam_buf_1_2_word_0, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_0, reg_1_2_buf_tx_plm_1_2_buf_plm_0);

    return 0;
}

int ag_drv_ngpon_tx_ploam_1_2_buf_tx_plm_1_2_buf_plm_0_get(uint32_t ngpon_tx_ploam_buf_1_2_word_0, uint32_t *value)
{
    uint32_t reg_1_2_buf_tx_plm_1_2_buf_plm_0=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ngpon_tx_ploam_buf_1_2_word_0 >= 2))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ngpon_tx_ploam_buf_1_2_word_0, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_0, reg_1_2_buf_tx_plm_1_2_buf_plm_0);

    *value = RU_FIELD_GET(0, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_0, VALUE, reg_1_2_buf_tx_plm_1_2_buf_plm_0);

    return 0;
}

int ag_drv_ngpon_tx_ploam_1_2_buf_tx_plm_1_2_buf_plm_1_set(uint32_t ngpon_tx_ploam_buf_1_2_word_1, uint32_t value)
{
    uint32_t reg_1_2_buf_tx_plm_1_2_buf_plm_1=0;

#ifdef VALIDATE_PARMS
    if((ngpon_tx_ploam_buf_1_2_word_1 >= 2))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_1_2_buf_tx_plm_1_2_buf_plm_1 = RU_FIELD_SET(0, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_1, VALUE, reg_1_2_buf_tx_plm_1_2_buf_plm_1, value);

    RU_REG_RAM_WRITE(0, ngpon_tx_ploam_buf_1_2_word_1, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_1, reg_1_2_buf_tx_plm_1_2_buf_plm_1);

    return 0;
}

int ag_drv_ngpon_tx_ploam_1_2_buf_tx_plm_1_2_buf_plm_1_get(uint32_t ngpon_tx_ploam_buf_1_2_word_1, uint32_t *value)
{
    uint32_t reg_1_2_buf_tx_plm_1_2_buf_plm_1=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ngpon_tx_ploam_buf_1_2_word_1 >= 2))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ngpon_tx_ploam_buf_1_2_word_1, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_1, reg_1_2_buf_tx_plm_1_2_buf_plm_1);

    *value = RU_FIELD_GET(0, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_1, VALUE, reg_1_2_buf_tx_plm_1_2_buf_plm_1);

    return 0;
}

int ag_drv_ngpon_tx_ploam_1_2_buf_tx_plm_1_2_buf_plm_2_set(uint32_t ngpon_tx_ploam_buf_1_2_word_2, uint32_t value)
{
    uint32_t reg_1_2_buf_tx_plm_1_2_buf_plm_2=0;

#ifdef VALIDATE_PARMS
    if((ngpon_tx_ploam_buf_1_2_word_2 >= 2))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_1_2_buf_tx_plm_1_2_buf_plm_2 = RU_FIELD_SET(0, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_2, VALUE, reg_1_2_buf_tx_plm_1_2_buf_plm_2, value);

    RU_REG_RAM_WRITE(0, ngpon_tx_ploam_buf_1_2_word_2, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_2, reg_1_2_buf_tx_plm_1_2_buf_plm_2);

    return 0;
}

int ag_drv_ngpon_tx_ploam_1_2_buf_tx_plm_1_2_buf_plm_2_get(uint32_t ngpon_tx_ploam_buf_1_2_word_2, uint32_t *value)
{
    uint32_t reg_1_2_buf_tx_plm_1_2_buf_plm_2=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ngpon_tx_ploam_buf_1_2_word_2 >= 2))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ngpon_tx_ploam_buf_1_2_word_2, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_2, reg_1_2_buf_tx_plm_1_2_buf_plm_2);

    *value = RU_FIELD_GET(0, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_2, VALUE, reg_1_2_buf_tx_plm_1_2_buf_plm_2);

    return 0;
}

int ag_drv_ngpon_tx_ploam_1_2_buf_tx_plm_1_2_buf_plm_3_set(uint32_t ngpon_tx_ploam_buf_1_2_word_3, uint32_t value)
{
    uint32_t reg_1_2_buf_tx_plm_1_2_buf_plm_3=0;

#ifdef VALIDATE_PARMS
    if((ngpon_tx_ploam_buf_1_2_word_3 >= 2))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_1_2_buf_tx_plm_1_2_buf_plm_3 = RU_FIELD_SET(0, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_3, VALUE, reg_1_2_buf_tx_plm_1_2_buf_plm_3, value);

    RU_REG_RAM_WRITE(0, ngpon_tx_ploam_buf_1_2_word_3, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_3, reg_1_2_buf_tx_plm_1_2_buf_plm_3);

    return 0;
}

int ag_drv_ngpon_tx_ploam_1_2_buf_tx_plm_1_2_buf_plm_3_get(uint32_t ngpon_tx_ploam_buf_1_2_word_3, uint32_t *value)
{
    uint32_t reg_1_2_buf_tx_plm_1_2_buf_plm_3=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ngpon_tx_ploam_buf_1_2_word_3 >= 2))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ngpon_tx_ploam_buf_1_2_word_3, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_3, reg_1_2_buf_tx_plm_1_2_buf_plm_3);

    *value = RU_FIELD_GET(0, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_3, VALUE, reg_1_2_buf_tx_plm_1_2_buf_plm_3);

    return 0;
}

int ag_drv_ngpon_tx_ploam_1_2_buf_tx_plm_1_2_buf_plm_4_set(uint32_t ngpon_tx_ploam_buf_1_2_word_4, uint32_t value)
{
    uint32_t reg_1_2_buf_tx_plm_1_2_buf_plm_4=0;

#ifdef VALIDATE_PARMS
    if((ngpon_tx_ploam_buf_1_2_word_4 >= 2))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_1_2_buf_tx_plm_1_2_buf_plm_4 = RU_FIELD_SET(0, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_4, VALUE, reg_1_2_buf_tx_plm_1_2_buf_plm_4, value);

    RU_REG_RAM_WRITE(0, ngpon_tx_ploam_buf_1_2_word_4, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_4, reg_1_2_buf_tx_plm_1_2_buf_plm_4);

    return 0;
}

int ag_drv_ngpon_tx_ploam_1_2_buf_tx_plm_1_2_buf_plm_4_get(uint32_t ngpon_tx_ploam_buf_1_2_word_4, uint32_t *value)
{
    uint32_t reg_1_2_buf_tx_plm_1_2_buf_plm_4=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ngpon_tx_ploam_buf_1_2_word_4 >= 2))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ngpon_tx_ploam_buf_1_2_word_4, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_4, reg_1_2_buf_tx_plm_1_2_buf_plm_4);

    *value = RU_FIELD_GET(0, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_4, VALUE, reg_1_2_buf_tx_plm_1_2_buf_plm_4);

    return 0;
}

int ag_drv_ngpon_tx_ploam_1_2_buf_tx_plm_1_2_buf_plm_5_set(uint32_t ngpon_tx_ploam_buf_1_2_word_5, uint32_t value)
{
    uint32_t reg_1_2_buf_tx_plm_1_2_buf_plm_5=0;

#ifdef VALIDATE_PARMS
    if((ngpon_tx_ploam_buf_1_2_word_5 >= 2))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_1_2_buf_tx_plm_1_2_buf_plm_5 = RU_FIELD_SET(0, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_5, VALUE, reg_1_2_buf_tx_plm_1_2_buf_plm_5, value);

    RU_REG_RAM_WRITE(0, ngpon_tx_ploam_buf_1_2_word_5, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_5, reg_1_2_buf_tx_plm_1_2_buf_plm_5);

    return 0;
}

int ag_drv_ngpon_tx_ploam_1_2_buf_tx_plm_1_2_buf_plm_5_get(uint32_t ngpon_tx_ploam_buf_1_2_word_5, uint32_t *value)
{
    uint32_t reg_1_2_buf_tx_plm_1_2_buf_plm_5=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ngpon_tx_ploam_buf_1_2_word_5 >= 2))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ngpon_tx_ploam_buf_1_2_word_5, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_5, reg_1_2_buf_tx_plm_1_2_buf_plm_5);

    *value = RU_FIELD_GET(0, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_5, VALUE, reg_1_2_buf_tx_plm_1_2_buf_plm_5);

    return 0;
}

int ag_drv_ngpon_tx_ploam_1_2_buf_tx_plm_1_2_buf_plm_6_set(uint32_t ngpon_tx_ploam_buf_1_2_word_6, uint32_t value)
{
    uint32_t reg_1_2_buf_tx_plm_1_2_buf_plm_6=0;

#ifdef VALIDATE_PARMS
    if((ngpon_tx_ploam_buf_1_2_word_6 >= 2))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_1_2_buf_tx_plm_1_2_buf_plm_6 = RU_FIELD_SET(0, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_6, VALUE, reg_1_2_buf_tx_plm_1_2_buf_plm_6, value);

    RU_REG_RAM_WRITE(0, ngpon_tx_ploam_buf_1_2_word_6, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_6, reg_1_2_buf_tx_plm_1_2_buf_plm_6);

    return 0;
}

int ag_drv_ngpon_tx_ploam_1_2_buf_tx_plm_1_2_buf_plm_6_get(uint32_t ngpon_tx_ploam_buf_1_2_word_6, uint32_t *value)
{
    uint32_t reg_1_2_buf_tx_plm_1_2_buf_plm_6=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ngpon_tx_ploam_buf_1_2_word_6 >= 2))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ngpon_tx_ploam_buf_1_2_word_6, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_6, reg_1_2_buf_tx_plm_1_2_buf_plm_6);

    *value = RU_FIELD_GET(0, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_6, VALUE, reg_1_2_buf_tx_plm_1_2_buf_plm_6);

    return 0;
}

int ag_drv_ngpon_tx_ploam_1_2_buf_tx_plm_1_2_buf_plm_7_set(uint32_t ngpon_tx_ploam_buf_1_2_word_7, uint32_t value)
{
    uint32_t reg_1_2_buf_tx_plm_1_2_buf_plm_7=0;

#ifdef VALIDATE_PARMS
    if((ngpon_tx_ploam_buf_1_2_word_7 >= 2))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_1_2_buf_tx_plm_1_2_buf_plm_7 = RU_FIELD_SET(0, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_7, VALUE, reg_1_2_buf_tx_plm_1_2_buf_plm_7, value);

    RU_REG_RAM_WRITE(0, ngpon_tx_ploam_buf_1_2_word_7, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_7, reg_1_2_buf_tx_plm_1_2_buf_plm_7);

    return 0;
}

int ag_drv_ngpon_tx_ploam_1_2_buf_tx_plm_1_2_buf_plm_7_get(uint32_t ngpon_tx_ploam_buf_1_2_word_7, uint32_t *value)
{
    uint32_t reg_1_2_buf_tx_plm_1_2_buf_plm_7=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ngpon_tx_ploam_buf_1_2_word_7 >= 2))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ngpon_tx_ploam_buf_1_2_word_7, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_7, reg_1_2_buf_tx_plm_1_2_buf_plm_7);

    *value = RU_FIELD_GET(0, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_7, VALUE, reg_1_2_buf_tx_plm_1_2_buf_plm_7);

    return 0;
}

int ag_drv_ngpon_tx_ploam_1_2_buf_tx_plm_1_2_buf_plm_8_set(uint32_t ngpon_tx_ploam_buf_1_2_word_8, uint32_t value)
{
    uint32_t reg_1_2_buf_tx_plm_1_2_buf_plm_8=0;

#ifdef VALIDATE_PARMS
    if((ngpon_tx_ploam_buf_1_2_word_8 >= 2))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_1_2_buf_tx_plm_1_2_buf_plm_8 = RU_FIELD_SET(0, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_8, VALUE, reg_1_2_buf_tx_plm_1_2_buf_plm_8, value);

    RU_REG_RAM_WRITE(0, ngpon_tx_ploam_buf_1_2_word_8, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_8, reg_1_2_buf_tx_plm_1_2_buf_plm_8);

    return 0;
}

int ag_drv_ngpon_tx_ploam_1_2_buf_tx_plm_1_2_buf_plm_8_get(uint32_t ngpon_tx_ploam_buf_1_2_word_8, uint32_t *value)
{
    uint32_t reg_1_2_buf_tx_plm_1_2_buf_plm_8=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ngpon_tx_ploam_buf_1_2_word_8 >= 2))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ngpon_tx_ploam_buf_1_2_word_8, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_8, reg_1_2_buf_tx_plm_1_2_buf_plm_8);

    *value = RU_FIELD_GET(0, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_8, VALUE, reg_1_2_buf_tx_plm_1_2_buf_plm_8);

    return 0;
}

int ag_drv_ngpon_tx_ploam_1_2_buf_tx_plm_1_2_buf_plm_9_set(uint32_t ngpon_tx_ploam_buf_1_2_word_9, uint32_t value)
{
    uint32_t reg_1_2_buf_tx_plm_1_2_buf_plm_9=0;

#ifdef VALIDATE_PARMS
    if((ngpon_tx_ploam_buf_1_2_word_9 >= 2))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_1_2_buf_tx_plm_1_2_buf_plm_9 = RU_FIELD_SET(0, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_9, VALUE, reg_1_2_buf_tx_plm_1_2_buf_plm_9, value);

    RU_REG_RAM_WRITE(0, ngpon_tx_ploam_buf_1_2_word_9, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_9, reg_1_2_buf_tx_plm_1_2_buf_plm_9);

    return 0;
}

int ag_drv_ngpon_tx_ploam_1_2_buf_tx_plm_1_2_buf_plm_9_get(uint32_t ngpon_tx_ploam_buf_1_2_word_9, uint32_t *value)
{
    uint32_t reg_1_2_buf_tx_plm_1_2_buf_plm_9=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ngpon_tx_ploam_buf_1_2_word_9 >= 2))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ngpon_tx_ploam_buf_1_2_word_9, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_9, reg_1_2_buf_tx_plm_1_2_buf_plm_9);

    *value = RU_FIELD_GET(0, NGPON_TX_PLOAM, 1_2_BUF_TX_PLM_1_2_BUF_PLM_9, VALUE, reg_1_2_buf_tx_plm_1_2_buf_plm_9);

    return 0;
}


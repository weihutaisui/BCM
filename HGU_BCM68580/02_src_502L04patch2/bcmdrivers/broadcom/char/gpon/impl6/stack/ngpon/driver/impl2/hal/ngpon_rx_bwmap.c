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
#include "ngpon_rx_bwmap.h"
int ag_drv_ngpon_rx_bwmap_trfc_allocid_set(uint32_t tcont_idx, uint16_t value)
{
    uint32_t reg_trfc_allocid=0;

#ifdef VALIDATE_PARMS
    if((tcont_idx >= 40) ||
       (value >= _14BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_trfc_allocid = RU_FIELD_SET(0, NGPON_RX_BWMAP, TRFC_ALLOCID, VALUE, reg_trfc_allocid, value);

    RU_REG_RAM_WRITE(0, tcont_idx, NGPON_RX_BWMAP, TRFC_ALLOCID, reg_trfc_allocid);

    return 0;
}

int ag_drv_ngpon_rx_bwmap_trfc_allocid_get(uint32_t tcont_idx, uint16_t *value)
{
    uint32_t reg_trfc_allocid=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((tcont_idx >= 40))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, tcont_idx, NGPON_RX_BWMAP, TRFC_ALLOCID, reg_trfc_allocid);

    *value = RU_FIELD_GET(0, NGPON_RX_BWMAP, TRFC_ALLOCID, VALUE, reg_trfc_allocid);

    return 0;
}

int ag_drv_ngpon_rx_bwmap_rngng_allocid_set(uint32_t onu_id_idx, uint16_t value)
{
    uint32_t reg_rngng_allocid=0;

#ifdef VALIDATE_PARMS
    if((onu_id_idx >= 4) ||
       (value >= _14BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_rngng_allocid = RU_FIELD_SET(0, NGPON_RX_BWMAP, RNGNG_ALLOCID, VALUE, reg_rngng_allocid, value);

    RU_REG_RAM_WRITE(0, onu_id_idx, NGPON_RX_BWMAP, RNGNG_ALLOCID, reg_rngng_allocid);

    return 0;
}

int ag_drv_ngpon_rx_bwmap_rngng_allocid_get(uint32_t onu_id_idx, uint16_t *value)
{
    uint32_t reg_rngng_allocid=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((onu_id_idx >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, onu_id_idx, NGPON_RX_BWMAP, RNGNG_ALLOCID, reg_rngng_allocid);

    *value = RU_FIELD_GET(0, NGPON_RX_BWMAP, RNGNG_ALLOCID, VALUE, reg_rngng_allocid);

    return 0;
}

int ag_drv_ngpon_rx_bwmap_trfc_allocid_en1_set(uint32_t en)
{
    uint32_t reg_trfc_allocid_en1=0;

#ifdef VALIDATE_PARMS
#endif

    reg_trfc_allocid_en1 = RU_FIELD_SET(0, NGPON_RX_BWMAP, TRFC_ALLOCID_EN1, EN, reg_trfc_allocid_en1, en);

    RU_REG_WRITE(0, NGPON_RX_BWMAP, TRFC_ALLOCID_EN1, reg_trfc_allocid_en1);

    return 0;
}

int ag_drv_ngpon_rx_bwmap_trfc_allocid_en1_get(uint32_t *en)
{
    uint32_t reg_trfc_allocid_en1=0;

#ifdef VALIDATE_PARMS
    if(!en)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_BWMAP, TRFC_ALLOCID_EN1, reg_trfc_allocid_en1);

    *en = RU_FIELD_GET(0, NGPON_RX_BWMAP, TRFC_ALLOCID_EN1, EN, reg_trfc_allocid_en1);

    return 0;
}

int ag_drv_ngpon_rx_bwmap_trfc_allocid_en2_set(uint8_t en)
{
    uint32_t reg_trfc_allocid_en2=0;

#ifdef VALIDATE_PARMS
#endif

    reg_trfc_allocid_en2 = RU_FIELD_SET(0, NGPON_RX_BWMAP, TRFC_ALLOCID_EN2, EN, reg_trfc_allocid_en2, en);

    RU_REG_WRITE(0, NGPON_RX_BWMAP, TRFC_ALLOCID_EN2, reg_trfc_allocid_en2);

    return 0;
}

int ag_drv_ngpon_rx_bwmap_trfc_allocid_en2_get(uint8_t *en)
{
    uint32_t reg_trfc_allocid_en2=0;

#ifdef VALIDATE_PARMS
    if(!en)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_BWMAP, TRFC_ALLOCID_EN2, reg_trfc_allocid_en2);

    *en = RU_FIELD_GET(0, NGPON_RX_BWMAP, TRFC_ALLOCID_EN2, EN, reg_trfc_allocid_en2);

    return 0;
}

int ag_drv_ngpon_rx_bwmap_rngng_allocid_en_set(uint8_t en)
{
    uint32_t reg_rngng_allocid_en=0;

#ifdef VALIDATE_PARMS
    if((en >= _4BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_rngng_allocid_en = RU_FIELD_SET(0, NGPON_RX_BWMAP, RNGNG_ALLOCID_EN, EN, reg_rngng_allocid_en, en);

    RU_REG_WRITE(0, NGPON_RX_BWMAP, RNGNG_ALLOCID_EN, reg_rngng_allocid_en);

    return 0;
}

int ag_drv_ngpon_rx_bwmap_rngng_allocid_en_get(uint8_t *en)
{
    uint32_t reg_rngng_allocid_en=0;

#ifdef VALIDATE_PARMS
    if(!en)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_BWMAP, RNGNG_ALLOCID_EN, reg_rngng_allocid_en);

    *en = RU_FIELD_GET(0, NGPON_RX_BWMAP, RNGNG_ALLOCID_EN, EN, reg_rngng_allocid_en);

    return 0;
}

int ag_drv_ngpon_rx_bwmap_trfc_tcnt_assoc_set(uint32_t tcont_idx, uint8_t a0_tnum, uint8_t a1_tnum, uint8_t a2_tnum, uint8_t a3_tnum)
{
    uint32_t reg_trfc_tcnt_assoc=0;

#ifdef VALIDATE_PARMS
    if((tcont_idx >= 10) ||
       (a0_tnum >= _6BITS_MAX_VAL_) ||
       (a1_tnum >= _6BITS_MAX_VAL_) ||
       (a2_tnum >= _6BITS_MAX_VAL_) ||
       (a3_tnum >= _6BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_trfc_tcnt_assoc = RU_FIELD_SET(0, NGPON_RX_BWMAP, TRFC_TCNT_ASSOC, A0_TNUM, reg_trfc_tcnt_assoc, a0_tnum);
    reg_trfc_tcnt_assoc = RU_FIELD_SET(0, NGPON_RX_BWMAP, TRFC_TCNT_ASSOC, A1_TNUM, reg_trfc_tcnt_assoc, a1_tnum);
    reg_trfc_tcnt_assoc = RU_FIELD_SET(0, NGPON_RX_BWMAP, TRFC_TCNT_ASSOC, A2_TNUM, reg_trfc_tcnt_assoc, a2_tnum);
    reg_trfc_tcnt_assoc = RU_FIELD_SET(0, NGPON_RX_BWMAP, TRFC_TCNT_ASSOC, A3_TNUM, reg_trfc_tcnt_assoc, a3_tnum);

    RU_REG_RAM_WRITE(0, tcont_idx, NGPON_RX_BWMAP, TRFC_TCNT_ASSOC, reg_trfc_tcnt_assoc);

    return 0;
}

int ag_drv_ngpon_rx_bwmap_trfc_tcnt_assoc_get(uint32_t tcont_idx, uint8_t *a0_tnum, uint8_t *a1_tnum, uint8_t *a2_tnum, uint8_t *a3_tnum)
{
    uint32_t reg_trfc_tcnt_assoc=0;

#ifdef VALIDATE_PARMS
    if(!a0_tnum || !a1_tnum || !a2_tnum || !a3_tnum)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((tcont_idx >= 10))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, tcont_idx, NGPON_RX_BWMAP, TRFC_TCNT_ASSOC, reg_trfc_tcnt_assoc);

    *a0_tnum = RU_FIELD_GET(0, NGPON_RX_BWMAP, TRFC_TCNT_ASSOC, A0_TNUM, reg_trfc_tcnt_assoc);
    *a1_tnum = RU_FIELD_GET(0, NGPON_RX_BWMAP, TRFC_TCNT_ASSOC, A1_TNUM, reg_trfc_tcnt_assoc);
    *a2_tnum = RU_FIELD_GET(0, NGPON_RX_BWMAP, TRFC_TCNT_ASSOC, A2_TNUM, reg_trfc_tcnt_assoc);
    *a3_tnum = RU_FIELD_GET(0, NGPON_RX_BWMAP, TRFC_TCNT_ASSOC, A3_TNUM, reg_trfc_tcnt_assoc);

    return 0;
}

int ag_drv_ngpon_rx_bwmap_rngng_tcnt_assoc_set(uint8_t rng_a0_tnum, uint8_t rng_a1_tnum, uint8_t rng_a2_tnum, uint8_t rng_a3_tnum)
{
    uint32_t reg_rngng_tcnt_assoc=0;

#ifdef VALIDATE_PARMS
    if((rng_a0_tnum >= _6BITS_MAX_VAL_) ||
       (rng_a1_tnum >= _6BITS_MAX_VAL_) ||
       (rng_a2_tnum >= _6BITS_MAX_VAL_) ||
       (rng_a3_tnum >= _6BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_rngng_tcnt_assoc = RU_FIELD_SET(0, NGPON_RX_BWMAP, RNGNG_TCNT_ASSOC, RNG_A0_TNUM, reg_rngng_tcnt_assoc, rng_a0_tnum);
    reg_rngng_tcnt_assoc = RU_FIELD_SET(0, NGPON_RX_BWMAP, RNGNG_TCNT_ASSOC, RNG_A1_TNUM, reg_rngng_tcnt_assoc, rng_a1_tnum);
    reg_rngng_tcnt_assoc = RU_FIELD_SET(0, NGPON_RX_BWMAP, RNGNG_TCNT_ASSOC, RNG_A2_TNUM, reg_rngng_tcnt_assoc, rng_a2_tnum);
    reg_rngng_tcnt_assoc = RU_FIELD_SET(0, NGPON_RX_BWMAP, RNGNG_TCNT_ASSOC, RNG_A3_TNUM, reg_rngng_tcnt_assoc, rng_a3_tnum);

    RU_REG_WRITE(0, NGPON_RX_BWMAP, RNGNG_TCNT_ASSOC, reg_rngng_tcnt_assoc);

    return 0;
}

int ag_drv_ngpon_rx_bwmap_rngng_tcnt_assoc_get(uint8_t *rng_a0_tnum, uint8_t *rng_a1_tnum, uint8_t *rng_a2_tnum, uint8_t *rng_a3_tnum)
{
    uint32_t reg_rngng_tcnt_assoc=0;

#ifdef VALIDATE_PARMS
    if(!rng_a0_tnum || !rng_a1_tnum || !rng_a2_tnum || !rng_a3_tnum)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_BWMAP, RNGNG_TCNT_ASSOC, reg_rngng_tcnt_assoc);

    *rng_a0_tnum = RU_FIELD_GET(0, NGPON_RX_BWMAP, RNGNG_TCNT_ASSOC, RNG_A0_TNUM, reg_rngng_tcnt_assoc);
    *rng_a1_tnum = RU_FIELD_GET(0, NGPON_RX_BWMAP, RNGNG_TCNT_ASSOC, RNG_A1_TNUM, reg_rngng_tcnt_assoc);
    *rng_a2_tnum = RU_FIELD_GET(0, NGPON_RX_BWMAP, RNGNG_TCNT_ASSOC, RNG_A2_TNUM, reg_rngng_tcnt_assoc);
    *rng_a3_tnum = RU_FIELD_GET(0, NGPON_RX_BWMAP, RNGNG_TCNT_ASSOC, RNG_A3_TNUM, reg_rngng_tcnt_assoc);

    return 0;
}

int ag_drv_ngpon_rx_bwmap_bwcnt_assoc_set(uint8_t cnt9_assoc, uint8_t cnt10_assoc, uint8_t cnt11_assoc, uint8_t cnt12_assoc)
{
    uint32_t reg_bwcnt_assoc=0;

#ifdef VALIDATE_PARMS
    if((cnt9_assoc >= _6BITS_MAX_VAL_) ||
       (cnt10_assoc >= _6BITS_MAX_VAL_) ||
       (cnt11_assoc >= _6BITS_MAX_VAL_) ||
       (cnt12_assoc >= _6BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_bwcnt_assoc = RU_FIELD_SET(0, NGPON_RX_BWMAP, BWCNT_ASSOC, CNT9_ASSOC, reg_bwcnt_assoc, cnt9_assoc);
    reg_bwcnt_assoc = RU_FIELD_SET(0, NGPON_RX_BWMAP, BWCNT_ASSOC, CNT10_ASSOC, reg_bwcnt_assoc, cnt10_assoc);
    reg_bwcnt_assoc = RU_FIELD_SET(0, NGPON_RX_BWMAP, BWCNT_ASSOC, CNT11_ASSOC, reg_bwcnt_assoc, cnt11_assoc);
    reg_bwcnt_assoc = RU_FIELD_SET(0, NGPON_RX_BWMAP, BWCNT_ASSOC, CNT12_ASSOC, reg_bwcnt_assoc, cnt12_assoc);

    RU_REG_WRITE(0, NGPON_RX_BWMAP, BWCNT_ASSOC, reg_bwcnt_assoc);

    return 0;
}

int ag_drv_ngpon_rx_bwmap_bwcnt_assoc_get(uint8_t *cnt9_assoc, uint8_t *cnt10_assoc, uint8_t *cnt11_assoc, uint8_t *cnt12_assoc)
{
    uint32_t reg_bwcnt_assoc=0;

#ifdef VALIDATE_PARMS
    if(!cnt9_assoc || !cnt10_assoc || !cnt11_assoc || !cnt12_assoc)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_BWMAP, BWCNT_ASSOC, reg_bwcnt_assoc);

    *cnt9_assoc = RU_FIELD_GET(0, NGPON_RX_BWMAP, BWCNT_ASSOC, CNT9_ASSOC, reg_bwcnt_assoc);
    *cnt10_assoc = RU_FIELD_GET(0, NGPON_RX_BWMAP, BWCNT_ASSOC, CNT10_ASSOC, reg_bwcnt_assoc);
    *cnt11_assoc = RU_FIELD_GET(0, NGPON_RX_BWMAP, BWCNT_ASSOC, CNT11_ASSOC, reg_bwcnt_assoc);
    *cnt12_assoc = RU_FIELD_GET(0, NGPON_RX_BWMAP, BWCNT_ASSOC, CNT12_ASSOC, reg_bwcnt_assoc);

    return 0;
}


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
#include "gpon_rx_alloc_id.h"
int ag_drv_gpon_rx_alloc_id_rang_alc_id_set(uint16_t alloc_id)
{
    uint32_t reg_rang_alc_id=0;

#ifdef VALIDATE_PARMS
    if((alloc_id >= _12BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_rang_alc_id = RU_FIELD_SET(0, GPON_RX_ALLOC_ID, RANG_ALC_ID, ALLOC_ID, reg_rang_alc_id, alloc_id);

    RU_REG_WRITE(0, GPON_RX_ALLOC_ID, RANG_ALC_ID, reg_rang_alc_id);

    return 0;
}

int ag_drv_gpon_rx_alloc_id_rang_alc_id_get(uint16_t *alloc_id)
{
    uint32_t reg_rang_alc_id=0;

#ifdef VALIDATE_PARMS
    if(!alloc_id)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_RX_ALLOC_ID, RANG_ALC_ID, reg_rang_alc_id);

    *alloc_id = RU_FIELD_GET(0, GPON_RX_ALLOC_ID, RANG_ALC_ID, ALLOC_ID, reg_rang_alc_id);

    return 0;
}

int ag_drv_gpon_rx_alloc_id_alc_id_cfg_set(uint32_t tcont_idx, uint16_t alloc_id)
{
    uint32_t reg_alc_id_cfg=0;

#ifdef VALIDATE_PARMS
    if((tcont_idx >= 40) ||
       (alloc_id >= _12BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d) alloc_id %d\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE, alloc_id);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_alc_id_cfg = RU_FIELD_SET(0, GPON_RX_ALLOC_ID, ALC_ID_CFG, ALLOC_ID, reg_alc_id_cfg, alloc_id);

    RU_REG_RAM_WRITE(0, tcont_idx, GPON_RX_ALLOC_ID, ALC_ID_CFG, reg_alc_id_cfg);

    return 0;
}

int ag_drv_gpon_rx_alloc_id_alc_id_cfg_get(uint32_t tcont_idx, uint16_t *alloc_id)
{
    uint32_t reg_alc_id_cfg=0;

#ifdef VALIDATE_PARMS
    if(!alloc_id)
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

    RU_REG_RAM_READ(0, tcont_idx, GPON_RX_ALLOC_ID, ALC_ID_CFG, reg_alc_id_cfg);

    *alloc_id = RU_FIELD_GET(0, GPON_RX_ALLOC_ID, ALC_ID_CFG, ALLOC_ID, reg_alc_id_cfg);

    return 0;
}

int ag_drv_gpon_rx_alloc_id_rang_alc_id_en_set(uint8_t alloc_en)
{
    uint32_t reg_rang_alc_id_en=0;

#ifdef VALIDATE_PARMS
    if((alloc_en >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_rang_alc_id_en = RU_FIELD_SET(0, GPON_RX_ALLOC_ID, RANG_ALC_ID_EN, ALLOC_EN, reg_rang_alc_id_en, alloc_en);

    RU_REG_WRITE(0, GPON_RX_ALLOC_ID, RANG_ALC_ID_EN, reg_rang_alc_id_en);

    return 0;
}

int ag_drv_gpon_rx_alloc_id_rang_alc_id_en_get(uint8_t *alloc_en)
{
    uint32_t reg_rang_alc_id_en=0;

#ifdef VALIDATE_PARMS
    if(!alloc_en)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_RX_ALLOC_ID, RANG_ALC_ID_EN, reg_rang_alc_id_en);

    *alloc_en = RU_FIELD_GET(0, GPON_RX_ALLOC_ID, RANG_ALC_ID_EN, ALLOC_EN, reg_rang_alc_id_en);

    return 0;
}

int ag_drv_gpon_rx_alloc_id_alc_id_en_set(uint32_t onu_id_idx, uint8_t alloc_en)
{
    uint32_t reg_alc_id_en=0;

#ifdef VALIDATE_PARMS
    if((onu_id_idx >= 5))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_alc_id_en = RU_FIELD_SET(0, GPON_RX_ALLOC_ID, ALC_ID_EN, ALLOC_EN, reg_alc_id_en, alloc_en);

    RU_REG_RAM_WRITE(0, onu_id_idx, GPON_RX_ALLOC_ID, ALC_ID_EN, reg_alc_id_en);

    return 0;
}

int ag_drv_gpon_rx_alloc_id_alc_id_en_get(uint32_t onu_id_idx, uint8_t *alloc_en)
{
    uint32_t reg_alc_id_en=0;

#ifdef VALIDATE_PARMS
    if(!alloc_en)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((onu_id_idx >= 5))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, onu_id_idx, GPON_RX_ALLOC_ID, ALC_ID_EN, reg_alc_id_en);

    *alloc_en = RU_FIELD_GET(0, GPON_RX_ALLOC_ID, ALC_ID_EN, ALLOC_EN, reg_alc_id_en);

    return 0;
}

int ag_drv_gpon_rx_alloc_id_alc_tcnt_assoc_set(uint32_t indx, uint8_t a0_tnum, uint8_t a1_tnum, uint8_t a2_tnum, uint8_t a3_tnum)
{
    uint32_t reg_alc_tcnt_assoc=0;

#ifdef VALIDATE_PARMS
    if((indx >= 10) ||
       (a0_tnum >= _6BITS_MAX_VAL_) ||
       (a1_tnum >= _6BITS_MAX_VAL_) ||
       (a2_tnum >= _6BITS_MAX_VAL_) ||
       (a3_tnum >= _6BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_alc_tcnt_assoc = RU_FIELD_SET(0, GPON_RX_ALLOC_ID, ALC_TCNT_ASSOC, A0_TNUM, reg_alc_tcnt_assoc, a0_tnum);
    reg_alc_tcnt_assoc = RU_FIELD_SET(0, GPON_RX_ALLOC_ID, ALC_TCNT_ASSOC, A1_TNUM, reg_alc_tcnt_assoc, a1_tnum);
    reg_alc_tcnt_assoc = RU_FIELD_SET(0, GPON_RX_ALLOC_ID, ALC_TCNT_ASSOC, A2_TNUM, reg_alc_tcnt_assoc, a2_tnum);
    reg_alc_tcnt_assoc = RU_FIELD_SET(0, GPON_RX_ALLOC_ID, ALC_TCNT_ASSOC, A3_TNUM, reg_alc_tcnt_assoc, a3_tnum);

    RU_REG_RAM_WRITE(0, indx, GPON_RX_ALLOC_ID, ALC_TCNT_ASSOC, reg_alc_tcnt_assoc);

    return 0;
}

int ag_drv_gpon_rx_alloc_id_alc_tcnt_assoc_get(uint32_t indx, uint8_t *a0_tnum, uint8_t *a1_tnum, uint8_t *a2_tnum, uint8_t *a3_tnum)
{
    uint32_t reg_alc_tcnt_assoc=0;

#ifdef VALIDATE_PARMS
    if(!a0_tnum || !a1_tnum || !a2_tnum || !a3_tnum)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((indx >= 10))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, indx, GPON_RX_ALLOC_ID, ALC_TCNT_ASSOC, reg_alc_tcnt_assoc);

    *a0_tnum = RU_FIELD_GET(0, GPON_RX_ALLOC_ID, ALC_TCNT_ASSOC, A0_TNUM, reg_alc_tcnt_assoc);
    *a1_tnum = RU_FIELD_GET(0, GPON_RX_ALLOC_ID, ALC_TCNT_ASSOC, A1_TNUM, reg_alc_tcnt_assoc);
    *a2_tnum = RU_FIELD_GET(0, GPON_RX_ALLOC_ID, ALC_TCNT_ASSOC, A2_TNUM, reg_alc_tcnt_assoc);
    *a3_tnum = RU_FIELD_GET(0, GPON_RX_ALLOC_ID, ALC_TCNT_ASSOC, A3_TNUM, reg_alc_tcnt_assoc);

    return 0;
}

int ag_drv_gpon_rx_alloc_id_bw_cnt_assoc_set(uint8_t cnt0_assoc, uint8_t cnt1_assoc, uint8_t cnt2_assoc, uint8_t cnt3_assoc)
{
    uint32_t reg_bw_cnt_assoc=0;

#ifdef VALIDATE_PARMS
    if((cnt0_assoc >= _6BITS_MAX_VAL_) ||
       (cnt1_assoc >= _6BITS_MAX_VAL_) ||
       (cnt2_assoc >= _6BITS_MAX_VAL_) ||
       (cnt3_assoc >= _6BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_bw_cnt_assoc = RU_FIELD_SET(0, GPON_RX_ALLOC_ID, BW_CNT_ASSOC, CNT0_ASSOC, reg_bw_cnt_assoc, cnt0_assoc);
    reg_bw_cnt_assoc = RU_FIELD_SET(0, GPON_RX_ALLOC_ID, BW_CNT_ASSOC, CNT1_ASSOC, reg_bw_cnt_assoc, cnt1_assoc);
    reg_bw_cnt_assoc = RU_FIELD_SET(0, GPON_RX_ALLOC_ID, BW_CNT_ASSOC, CNT2_ASSOC, reg_bw_cnt_assoc, cnt2_assoc);
    reg_bw_cnt_assoc = RU_FIELD_SET(0, GPON_RX_ALLOC_ID, BW_CNT_ASSOC, CNT3_ASSOC, reg_bw_cnt_assoc, cnt3_assoc);

    RU_REG_WRITE(0, GPON_RX_ALLOC_ID, BW_CNT_ASSOC, reg_bw_cnt_assoc);

    return 0;
}

int ag_drv_gpon_rx_alloc_id_bw_cnt_assoc_get(uint8_t *cnt0_assoc, uint8_t *cnt1_assoc, uint8_t *cnt2_assoc, uint8_t *cnt3_assoc)
{
    uint32_t reg_bw_cnt_assoc=0;

#ifdef VALIDATE_PARMS
    if(!cnt0_assoc || !cnt1_assoc || !cnt2_assoc || !cnt3_assoc)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_RX_ALLOC_ID, BW_CNT_ASSOC, reg_bw_cnt_assoc);

    *cnt0_assoc = RU_FIELD_GET(0, GPON_RX_ALLOC_ID, BW_CNT_ASSOC, CNT0_ASSOC, reg_bw_cnt_assoc);
    *cnt1_assoc = RU_FIELD_GET(0, GPON_RX_ALLOC_ID, BW_CNT_ASSOC, CNT1_ASSOC, reg_bw_cnt_assoc);
    *cnt2_assoc = RU_FIELD_GET(0, GPON_RX_ALLOC_ID, BW_CNT_ASSOC, CNT2_ASSOC, reg_bw_cnt_assoc);
    *cnt3_assoc = RU_FIELD_GET(0, GPON_RX_ALLOC_ID, BW_CNT_ASSOC, CNT3_ASSOC, reg_bw_cnt_assoc);

    return 0;
}


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
#include "gpon_rx_alloc_id.h"
bdmf_error_t ag_drv_gpon_rx_alloc_id_rang_alc_id_set(uint16_t alloc_id)
{
    uint32_t reg_rang_alc_id=0;

#ifdef VALIDATE_PARMS
    if((alloc_id >= _12BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_rang_alc_id = RU_FIELD_SET(0, GPON_RX_ALLOC_ID, RANG_ALC_ID, ALLOC_ID, reg_rang_alc_id, alloc_id);

    RU_REG_WRITE(0, GPON_RX_ALLOC_ID, RANG_ALC_ID, reg_rang_alc_id);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_alloc_id_rang_alc_id_get(uint16_t *alloc_id)
{
    uint32_t reg_rang_alc_id=0;

#ifdef VALIDATE_PARMS
    if(!alloc_id)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_ALLOC_ID, RANG_ALC_ID, reg_rang_alc_id);

    *alloc_id = RU_FIELD_GET(0, GPON_RX_ALLOC_ID, RANG_ALC_ID, ALLOC_ID, reg_rang_alc_id);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_alloc_id_alc_id_cfg_set(uint32_t tcont_idx, uint16_t alloc_id)
{
    uint32_t reg_alc_id_cfg=0;

#ifdef VALIDATE_PARMS
    if((tcont_idx >= 40) ||
       (alloc_id >= _12BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_alc_id_cfg = RU_FIELD_SET(0, GPON_RX_ALLOC_ID, ALC_ID_CFG, ALLOC_ID, reg_alc_id_cfg, alloc_id);

    RU_REG_RAM_WRITE(0, tcont_idx, GPON_RX_ALLOC_ID, ALC_ID_CFG, reg_alc_id_cfg);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_alloc_id_alc_id_cfg_get(uint32_t tcont_idx, uint16_t *alloc_id)
{
    uint32_t reg_alc_id_cfg=0;

#ifdef VALIDATE_PARMS
    if(!alloc_id)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
    if((tcont_idx >= 40))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, tcont_idx, GPON_RX_ALLOC_ID, ALC_ID_CFG, reg_alc_id_cfg);

    *alloc_id = RU_FIELD_GET(0, GPON_RX_ALLOC_ID, ALC_ID_CFG, ALLOC_ID, reg_alc_id_cfg);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_alloc_id_rang_alc_id_en_set(bdmf_boolean alloc_en)
{
    uint32_t reg_rang_alc_id_en=0;

#ifdef VALIDATE_PARMS
    if((alloc_en >= _1BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_rang_alc_id_en = RU_FIELD_SET(0, GPON_RX_ALLOC_ID, RANG_ALC_ID_EN, ALLOC_EN, reg_rang_alc_id_en, alloc_en);

    RU_REG_WRITE(0, GPON_RX_ALLOC_ID, RANG_ALC_ID_EN, reg_rang_alc_id_en);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_alloc_id_rang_alc_id_en_get(bdmf_boolean *alloc_en)
{
    uint32_t reg_rang_alc_id_en=0;

#ifdef VALIDATE_PARMS
    if(!alloc_en)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_ALLOC_ID, RANG_ALC_ID_EN, reg_rang_alc_id_en);

    *alloc_en = RU_FIELD_GET(0, GPON_RX_ALLOC_ID, RANG_ALC_ID_EN, ALLOC_EN, reg_rang_alc_id_en);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_alloc_id_alc_id_en_set(uint32_t onu_id_idx, uint8_t alloc_en)
{
    uint32_t reg_alc_id_en=0;

#ifdef VALIDATE_PARMS
    if((onu_id_idx >= 5))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_alc_id_en = RU_FIELD_SET(0, GPON_RX_ALLOC_ID, ALC_ID_EN, ALLOC_EN, reg_alc_id_en, alloc_en);

    RU_REG_RAM_WRITE(0, onu_id_idx, GPON_RX_ALLOC_ID, ALC_ID_EN, reg_alc_id_en);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_alloc_id_alc_id_en_get(uint32_t onu_id_idx, uint8_t *alloc_en)
{
    uint32_t reg_alc_id_en=0;

#ifdef VALIDATE_PARMS
    if(!alloc_en)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
    if((onu_id_idx >= 5))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, onu_id_idx, GPON_RX_ALLOC_ID, ALC_ID_EN, reg_alc_id_en);

    *alloc_en = RU_FIELD_GET(0, GPON_RX_ALLOC_ID, ALC_ID_EN, ALLOC_EN, reg_alc_id_en);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_alloc_id_alc_tcnt_assoc_set(uint32_t indx, uint8_t a0_tnum, uint8_t a1_tnum, uint8_t a2_tnum, uint8_t a3_tnum)
{
    uint32_t reg_alc_tcnt_assoc=0;

#ifdef VALIDATE_PARMS
    if((indx >= 10) ||
       (a0_tnum >= _6BITS_MAX_VAL_) ||
       (a1_tnum >= _6BITS_MAX_VAL_) ||
       (a2_tnum >= _6BITS_MAX_VAL_) ||
       (a3_tnum >= _6BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_alc_tcnt_assoc = RU_FIELD_SET(0, GPON_RX_ALLOC_ID, ALC_TCNT_ASSOC, A0_TNUM, reg_alc_tcnt_assoc, a0_tnum);
    reg_alc_tcnt_assoc = RU_FIELD_SET(0, GPON_RX_ALLOC_ID, ALC_TCNT_ASSOC, A1_TNUM, reg_alc_tcnt_assoc, a1_tnum);
    reg_alc_tcnt_assoc = RU_FIELD_SET(0, GPON_RX_ALLOC_ID, ALC_TCNT_ASSOC, A2_TNUM, reg_alc_tcnt_assoc, a2_tnum);
    reg_alc_tcnt_assoc = RU_FIELD_SET(0, GPON_RX_ALLOC_ID, ALC_TCNT_ASSOC, A3_TNUM, reg_alc_tcnt_assoc, a3_tnum);

    RU_REG_RAM_WRITE(0, indx, GPON_RX_ALLOC_ID, ALC_TCNT_ASSOC, reg_alc_tcnt_assoc);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_alloc_id_alc_tcnt_assoc_get(uint32_t indx, uint8_t *a0_tnum, uint8_t *a1_tnum, uint8_t *a2_tnum, uint8_t *a3_tnum)
{
    uint32_t reg_alc_tcnt_assoc=0;

#ifdef VALIDATE_PARMS
    if(!a0_tnum || !a1_tnum || !a2_tnum || !a3_tnum)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
    if((indx >= 10))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, indx, GPON_RX_ALLOC_ID, ALC_TCNT_ASSOC, reg_alc_tcnt_assoc);

    *a0_tnum = RU_FIELD_GET(0, GPON_RX_ALLOC_ID, ALC_TCNT_ASSOC, A0_TNUM, reg_alc_tcnt_assoc);
    *a1_tnum = RU_FIELD_GET(0, GPON_RX_ALLOC_ID, ALC_TCNT_ASSOC, A1_TNUM, reg_alc_tcnt_assoc);
    *a2_tnum = RU_FIELD_GET(0, GPON_RX_ALLOC_ID, ALC_TCNT_ASSOC, A2_TNUM, reg_alc_tcnt_assoc);
    *a3_tnum = RU_FIELD_GET(0, GPON_RX_ALLOC_ID, ALC_TCNT_ASSOC, A3_TNUM, reg_alc_tcnt_assoc);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_alloc_id_bw_cnt_assoc_set(uint8_t cnt0_assoc, uint8_t cnt1_assoc, uint8_t cnt2_assoc, uint8_t cnt3_assoc)
{
    uint32_t reg_bw_cnt_assoc=0;

#ifdef VALIDATE_PARMS
    if((cnt0_assoc >= _6BITS_MAX_VAL_) ||
       (cnt1_assoc >= _6BITS_MAX_VAL_) ||
       (cnt2_assoc >= _6BITS_MAX_VAL_) ||
       (cnt3_assoc >= _6BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_bw_cnt_assoc = RU_FIELD_SET(0, GPON_RX_ALLOC_ID, BW_CNT_ASSOC, CNT0_ASSOC, reg_bw_cnt_assoc, cnt0_assoc);
    reg_bw_cnt_assoc = RU_FIELD_SET(0, GPON_RX_ALLOC_ID, BW_CNT_ASSOC, CNT1_ASSOC, reg_bw_cnt_assoc, cnt1_assoc);
    reg_bw_cnt_assoc = RU_FIELD_SET(0, GPON_RX_ALLOC_ID, BW_CNT_ASSOC, CNT2_ASSOC, reg_bw_cnt_assoc, cnt2_assoc);
    reg_bw_cnt_assoc = RU_FIELD_SET(0, GPON_RX_ALLOC_ID, BW_CNT_ASSOC, CNT3_ASSOC, reg_bw_cnt_assoc, cnt3_assoc);

    RU_REG_WRITE(0, GPON_RX_ALLOC_ID, BW_CNT_ASSOC, reg_bw_cnt_assoc);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_alloc_id_bw_cnt_assoc_get(uint8_t *cnt0_assoc, uint8_t *cnt1_assoc, uint8_t *cnt2_assoc, uint8_t *cnt3_assoc)
{
    uint32_t reg_bw_cnt_assoc=0;

#ifdef VALIDATE_PARMS
    if(!cnt0_assoc || !cnt1_assoc || !cnt2_assoc || !cnt3_assoc)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_ALLOC_ID, BW_CNT_ASSOC, reg_bw_cnt_assoc);

    *cnt0_assoc = RU_FIELD_GET(0, GPON_RX_ALLOC_ID, BW_CNT_ASSOC, CNT0_ASSOC, reg_bw_cnt_assoc);
    *cnt1_assoc = RU_FIELD_GET(0, GPON_RX_ALLOC_ID, BW_CNT_ASSOC, CNT1_ASSOC, reg_bw_cnt_assoc);
    *cnt2_assoc = RU_FIELD_GET(0, GPON_RX_ALLOC_ID, BW_CNT_ASSOC, CNT2_ASSOC, reg_bw_cnt_assoc);
    *cnt3_assoc = RU_FIELD_GET(0, GPON_RX_ALLOC_ID, BW_CNT_ASSOC, CNT3_ASSOC, reg_bw_cnt_assoc);

    return BDMF_ERR_OK;
}

#ifdef USE_BDMF_SHELL
enum
{
    BDMF_rang_alc_id,
    BDMF_alc_id_cfg,
    BDMF_rang_alc_id_en,
    BDMF_alc_id_en,
    BDMF_alc_tcnt_assoc,
    BDMF_bw_cnt_assoc,
};

typedef enum
{
    bdmf_address_rang_alc_id,
    bdmf_address_alc_id_cfg,
    bdmf_address_rang_alc_id_en,
    bdmf_address_alc_id_en,
    bdmf_address_alc_tcnt_assoc,
    bdmf_address_bw_cnt_assoc,
}
bdmf_address;

static int bcm_gpon_rx_alloc_id_cli_set(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_error_t err;

    switch(parm[0].value.unumber)
    {
    case BDMF_rang_alc_id:
        err = ag_drv_gpon_rx_alloc_id_rang_alc_id_set(parm[1].value.unumber);
        break;
    case BDMF_alc_id_cfg:
        err = ag_drv_gpon_rx_alloc_id_alc_id_cfg_set(parm[1].value.unumber, parm[2].value.unumber);
        break;
    case BDMF_rang_alc_id_en:
        err = ag_drv_gpon_rx_alloc_id_rang_alc_id_en_set(parm[1].value.unumber);
        break;
    case BDMF_alc_id_en:
        err = ag_drv_gpon_rx_alloc_id_alc_id_en_set(parm[1].value.unumber, parm[2].value.unumber);
        break;
    case BDMF_alc_tcnt_assoc:
        err = ag_drv_gpon_rx_alloc_id_alc_tcnt_assoc_set(parm[1].value.unumber, parm[2].value.unumber, parm[3].value.unumber, parm[4].value.unumber, parm[5].value.unumber);
        break;
    case BDMF_bw_cnt_assoc:
        err = ag_drv_gpon_rx_alloc_id_bw_cnt_assoc_set(parm[1].value.unumber, parm[2].value.unumber, parm[3].value.unumber, parm[4].value.unumber);
        break;
    default:
        err = BDMF_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

static int bcm_gpon_rx_alloc_id_cli_get(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_error_t err;

    switch(parm[0].value.unumber)
    {
    case BDMF_rang_alc_id:
    {
        uint16_t alloc_id;
        err = ag_drv_gpon_rx_alloc_id_rang_alc_id_get(&alloc_id);
        bdmf_session_print(session, "alloc_id = %u = 0x%x\n", alloc_id, alloc_id);
        break;
    }
    case BDMF_alc_id_cfg:
    {
        uint16_t alloc_id;
        err = ag_drv_gpon_rx_alloc_id_alc_id_cfg_get(parm[1].value.unumber, &alloc_id);
        bdmf_session_print(session, "alloc_id = %u = 0x%x\n", alloc_id, alloc_id);
        break;
    }
    case BDMF_rang_alc_id_en:
    {
        bdmf_boolean alloc_en;
        err = ag_drv_gpon_rx_alloc_id_rang_alc_id_en_get(&alloc_en);
        bdmf_session_print(session, "alloc_en = %u = 0x%x\n", alloc_en, alloc_en);
        break;
    }
    case BDMF_alc_id_en:
    {
        uint8_t alloc_en;
        err = ag_drv_gpon_rx_alloc_id_alc_id_en_get(parm[1].value.unumber, &alloc_en);
        bdmf_session_print(session, "alloc_en = %u = 0x%x\n", alloc_en, alloc_en);
        break;
    }
    case BDMF_alc_tcnt_assoc:
    {
        uint8_t a0_tnum;
        uint8_t a1_tnum;
        uint8_t a2_tnum;
        uint8_t a3_tnum;
        err = ag_drv_gpon_rx_alloc_id_alc_tcnt_assoc_get(parm[1].value.unumber, &a0_tnum, &a1_tnum, &a2_tnum, &a3_tnum);
        bdmf_session_print(session, "a0_tnum = %u = 0x%x\n", a0_tnum, a0_tnum);
        bdmf_session_print(session, "a1_tnum = %u = 0x%x\n", a1_tnum, a1_tnum);
        bdmf_session_print(session, "a2_tnum = %u = 0x%x\n", a2_tnum, a2_tnum);
        bdmf_session_print(session, "a3_tnum = %u = 0x%x\n", a3_tnum, a3_tnum);
        break;
    }
    case BDMF_bw_cnt_assoc:
    {
        uint8_t cnt0_assoc;
        uint8_t cnt1_assoc;
        uint8_t cnt2_assoc;
        uint8_t cnt3_assoc;
        err = ag_drv_gpon_rx_alloc_id_bw_cnt_assoc_get(&cnt0_assoc, &cnt1_assoc, &cnt2_assoc, &cnt3_assoc);
        bdmf_session_print(session, "cnt0_assoc = %u = 0x%x\n", cnt0_assoc, cnt0_assoc);
        bdmf_session_print(session, "cnt1_assoc = %u = 0x%x\n", cnt1_assoc, cnt1_assoc);
        bdmf_session_print(session, "cnt2_assoc = %u = 0x%x\n", cnt2_assoc, cnt2_assoc);
        bdmf_session_print(session, "cnt3_assoc = %u = 0x%x\n", cnt3_assoc, cnt3_assoc);
        break;
    }
    default:
        err = BDMF_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

static int bcm_gpon_rx_alloc_id_cli_test(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_test_method m = parm[0].value.unumber;
    bdmf_error_t err = BDMF_ERR_OK;

    {
        uint16_t alloc_id=gtmv(m, 12);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_alloc_id_rang_alc_id_set( %u)\n", alloc_id);
        if(!err) ag_drv_gpon_rx_alloc_id_rang_alc_id_set(alloc_id);
        if(!err) ag_drv_gpon_rx_alloc_id_rang_alc_id_get( &alloc_id);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_alloc_id_rang_alc_id_get( %u)\n", alloc_id);
        if(err || alloc_id!=gtmv(m, 12))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t tcont_idx=gtmv(m, 3);
        uint16_t alloc_id=gtmv(m, 12);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_alloc_id_alc_id_cfg_set( %u %u)\n", tcont_idx, alloc_id);
        if(!err) ag_drv_gpon_rx_alloc_id_alc_id_cfg_set(tcont_idx, alloc_id);
        if(!err) ag_drv_gpon_rx_alloc_id_alc_id_cfg_get( tcont_idx, &alloc_id);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_alloc_id_alc_id_cfg_get( %u %u)\n", tcont_idx, alloc_id);
        if(err || alloc_id!=gtmv(m, 12))
            return err ? err : BDMF_ERR_IO;
    }
    {
        bdmf_boolean alloc_en=gtmv(m, 1);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_alloc_id_rang_alc_id_en_set( %u)\n", alloc_en);
        if(!err) ag_drv_gpon_rx_alloc_id_rang_alc_id_en_set(alloc_en);
        if(!err) ag_drv_gpon_rx_alloc_id_rang_alc_id_en_get( &alloc_en);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_alloc_id_rang_alc_id_en_get( %u)\n", alloc_en);
        if(err || alloc_en!=gtmv(m, 1))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t onu_id_idx=gtmv(m, 0);
        uint8_t alloc_en=gtmv(m, 8);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_alloc_id_alc_id_en_set( %u %u)\n", onu_id_idx, alloc_en);
        if(!err) ag_drv_gpon_rx_alloc_id_alc_id_en_set(onu_id_idx, alloc_en);
        if(!err) ag_drv_gpon_rx_alloc_id_alc_id_en_get( onu_id_idx, &alloc_en);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_alloc_id_alc_id_en_get( %u %u)\n", onu_id_idx, alloc_en);
        if(err || alloc_en!=gtmv(m, 8))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t indx=gtmv(m, 1);
        uint8_t a0_tnum=gtmv(m, 6);
        uint8_t a1_tnum=gtmv(m, 6);
        uint8_t a2_tnum=gtmv(m, 6);
        uint8_t a3_tnum=gtmv(m, 6);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_alloc_id_alc_tcnt_assoc_set( %u %u %u %u %u)\n", indx, a0_tnum, a1_tnum, a2_tnum, a3_tnum);
        if(!err) ag_drv_gpon_rx_alloc_id_alc_tcnt_assoc_set(indx, a0_tnum, a1_tnum, a2_tnum, a3_tnum);
        if(!err) ag_drv_gpon_rx_alloc_id_alc_tcnt_assoc_get( indx, &a0_tnum, &a1_tnum, &a2_tnum, &a3_tnum);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_alloc_id_alc_tcnt_assoc_get( %u %u %u %u %u)\n", indx, a0_tnum, a1_tnum, a2_tnum, a3_tnum);
        if(err || a0_tnum!=gtmv(m, 6) || a1_tnum!=gtmv(m, 6) || a2_tnum!=gtmv(m, 6) || a3_tnum!=gtmv(m, 6))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint8_t cnt0_assoc=gtmv(m, 6);
        uint8_t cnt1_assoc=gtmv(m, 6);
        uint8_t cnt2_assoc=gtmv(m, 6);
        uint8_t cnt3_assoc=gtmv(m, 6);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_alloc_id_bw_cnt_assoc_set( %u %u %u %u)\n", cnt0_assoc, cnt1_assoc, cnt2_assoc, cnt3_assoc);
        if(!err) ag_drv_gpon_rx_alloc_id_bw_cnt_assoc_set(cnt0_assoc, cnt1_assoc, cnt2_assoc, cnt3_assoc);
        if(!err) ag_drv_gpon_rx_alloc_id_bw_cnt_assoc_get( &cnt0_assoc, &cnt1_assoc, &cnt2_assoc, &cnt3_assoc);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_alloc_id_bw_cnt_assoc_get( %u %u %u %u)\n", cnt0_assoc, cnt1_assoc, cnt2_assoc, cnt3_assoc);
        if(err || cnt0_assoc!=gtmv(m, 6) || cnt1_assoc!=gtmv(m, 6) || cnt2_assoc!=gtmv(m, 6) || cnt3_assoc!=gtmv(m, 6))
            return err ? err : BDMF_ERR_IO;
    }
    return err;
}

static int bcm_gpon_rx_alloc_id_cli_address(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
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
    case bdmf_address_rang_alc_id : reg = &RU_REG(GPON_RX_ALLOC_ID, RANG_ALC_ID); blk = &RU_BLK(GPON_RX_ALLOC_ID); break;
    case bdmf_address_alc_id_cfg : reg = &RU_REG(GPON_RX_ALLOC_ID, ALC_ID_CFG); blk = &RU_BLK(GPON_RX_ALLOC_ID); break;
    case bdmf_address_rang_alc_id_en : reg = &RU_REG(GPON_RX_ALLOC_ID, RANG_ALC_ID_EN); blk = &RU_BLK(GPON_RX_ALLOC_ID); break;
    case bdmf_address_alc_id_en : reg = &RU_REG(GPON_RX_ALLOC_ID, ALC_ID_EN); blk = &RU_BLK(GPON_RX_ALLOC_ID); break;
    case bdmf_address_alc_tcnt_assoc : reg = &RU_REG(GPON_RX_ALLOC_ID, ALC_TCNT_ASSOC); blk = &RU_BLK(GPON_RX_ALLOC_ID); break;
    case bdmf_address_bw_cnt_assoc : reg = &RU_REG(GPON_RX_ALLOC_ID, BW_CNT_ASSOC); blk = &RU_BLK(GPON_RX_ALLOC_ID); break;
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

bdmfmon_handle_t ag_drv_gpon_rx_alloc_id_cli_init(bdmfmon_handle_t driver_dir)
{
    bdmfmon_handle_t dir;

    if ((dir = bdmfmon_dir_find(driver_dir, "gpon_rx_alloc_id"))!=NULL)
        return dir;
    dir = bdmfmon_dir_add(driver_dir, "gpon_rx_alloc_id", "gpon_rx_alloc_id", BDMF_ACCESS_ADMIN, NULL);

    {
        static bdmfmon_cmd_parm_t set_rang_alc_id[]={
            BDMFMON_MAKE_PARM("alloc_id", "alloc_id", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_alc_id_cfg[]={
            BDMFMON_MAKE_PARM("tcont_idx", "tcont_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("alloc_id", "alloc_id", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_rang_alc_id_en[]={
            BDMFMON_MAKE_PARM("alloc_en", "alloc_en", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_alc_id_en[]={
            BDMFMON_MAKE_PARM("onu_id_idx", "onu_id_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("alloc_en", "alloc_en", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_alc_tcnt_assoc[]={
            BDMFMON_MAKE_PARM("indx", "indx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("a0_tnum", "a0_tnum", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("a1_tnum", "a1_tnum", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("a2_tnum", "a2_tnum", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("a3_tnum", "a3_tnum", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_bw_cnt_assoc[]={
            BDMFMON_MAKE_PARM("cnt0_assoc", "cnt0_assoc", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("cnt1_assoc", "cnt1_assoc", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("cnt2_assoc", "cnt2_assoc", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("cnt3_assoc", "cnt3_assoc", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_enum_val_t selector_table[] = {
            { .name="rang_alc_id", .val=BDMF_rang_alc_id, .parms=set_rang_alc_id },
            { .name="alc_id_cfg", .val=BDMF_alc_id_cfg, .parms=set_alc_id_cfg },
            { .name="rang_alc_id_en", .val=BDMF_rang_alc_id_en, .parms=set_rang_alc_id_en },
            { .name="alc_id_en", .val=BDMF_alc_id_en, .parms=set_alc_id_en },
            { .name="alc_tcnt_assoc", .val=BDMF_alc_tcnt_assoc, .parms=set_alc_tcnt_assoc },
            { .name="bw_cnt_assoc", .val=BDMF_bw_cnt_assoc, .parms=set_bw_cnt_assoc },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "set", "set", bcm_gpon_rx_alloc_id_cli_set,
            BDMFMON_MAKE_PARM_SELECTOR("purpose", "purpose", selector_table, 0));
    }
    {
        static bdmfmon_cmd_parm_t set_default[]={
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_alc_id_cfg[]={
            BDMFMON_MAKE_PARM("tcont_idx", "tcont_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_alc_id_en[]={
            BDMFMON_MAKE_PARM("onu_id_idx", "onu_id_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_alc_tcnt_assoc[]={
            BDMFMON_MAKE_PARM("indx", "indx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_enum_val_t selector_table[] = {
            { .name="rang_alc_id", .val=BDMF_rang_alc_id, .parms=set_default },
            { .name="alc_id_cfg", .val=BDMF_alc_id_cfg, .parms=set_alc_id_cfg },
            { .name="rang_alc_id_en", .val=BDMF_rang_alc_id_en, .parms=set_default },
            { .name="alc_id_en", .val=BDMF_alc_id_en, .parms=set_alc_id_en },
            { .name="alc_tcnt_assoc", .val=BDMF_alc_tcnt_assoc, .parms=set_alc_tcnt_assoc },
            { .name="bw_cnt_assoc", .val=BDMF_bw_cnt_assoc, .parms=set_default },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "get", "get", bcm_gpon_rx_alloc_id_cli_get,
            BDMFMON_MAKE_PARM_SELECTOR("purpose", "purpose", selector_table, 0));
    }
    {
        static bdmfmon_enum_val_t enum_table_test_method[] = {
            { .name="low" , .val=bdmf_test_method_low },
            { .name="mid" , .val=bdmf_test_method_mid },
            { .name="high" , .val=bdmf_test_method_high },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "test", "test", bcm_gpon_rx_alloc_id_cli_test,
            BDMFMON_MAKE_PARM_ENUM("method", "low: 0000, mid: 1000, high: 1111", enum_table_test_method, 0));
    }
    {
        static bdmfmon_enum_val_t enum_table_address[] = {
            { .name="RANG_ALC_ID" , .val=bdmf_address_rang_alc_id },
            { .name="ALC_ID_CFG" , .val=bdmf_address_alc_id_cfg },
            { .name="RANG_ALC_ID_EN" , .val=bdmf_address_rang_alc_id_en },
            { .name="ALC_ID_EN" , .val=bdmf_address_alc_id_en },
            { .name="ALC_TCNT_ASSOC" , .val=bdmf_address_alc_tcnt_assoc },
            { .name="BW_CNT_ASSOC" , .val=bdmf_address_bw_cnt_assoc },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "address", "address", bcm_gpon_rx_alloc_id_cli_address,
            BDMFMON_MAKE_PARM_ENUM("method", "method", enum_table_address, 0),
            BDMFMON_MAKE_PARM("index2", "onu_id/alloc_id/port_id/etc...", BDMFMON_PARM_NUMBER, BDMFMON_PARM_FLAG_OPTIONAL));
    }
    return dir;
}
#endif /* USE_BDMF_SHELL */


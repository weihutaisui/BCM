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
#include "ngpon_rx_tod.h"
int ag_drv_ngpon_rx_tod_cfg_set(uint16_t req_phase, uint8_t incr, uint8_t lof_beh)
{
    uint32_t reg_cfg=0;

#ifdef VALIDATE_PARMS
    if((incr >= _1BITS_MAX_VAL_) ||
       (lof_beh >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_cfg = RU_FIELD_SET(0, NGPON_RX_TOD, CFG, REQ_PHASE, reg_cfg, req_phase);
    reg_cfg = RU_FIELD_SET(0, NGPON_RX_TOD, CFG, INCR, reg_cfg, incr);
    reg_cfg = RU_FIELD_SET(0, NGPON_RX_TOD, CFG, LOF_BEH, reg_cfg, lof_beh);

    RU_REG_WRITE(0, NGPON_RX_TOD, CFG, reg_cfg);

    return 0;
}

int ag_drv_ngpon_rx_tod_cfg_get(uint16_t *req_phase, uint8_t *incr, uint8_t *lof_beh)
{
    uint32_t reg_cfg=0;

#ifdef VALIDATE_PARMS
    if(!req_phase || !incr || !lof_beh)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_TOD, CFG, reg_cfg);

    *req_phase = RU_FIELD_GET(0, NGPON_RX_TOD, CFG, REQ_PHASE, reg_cfg);
    *incr = RU_FIELD_GET(0, NGPON_RX_TOD, CFG, INCR, reg_cfg);
    *lof_beh = RU_FIELD_GET(0, NGPON_RX_TOD, CFG, LOF_BEH, reg_cfg);

    return 0;
}

int ag_drv_ngpon_rx_tod_cong_thr_set(uint8_t assert, uint8_t negate)
{
    uint32_t reg_cong_thr=0;

#ifdef VALIDATE_PARMS
    if((assert >= _6BITS_MAX_VAL_) ||
       (negate >= _6BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_cong_thr = RU_FIELD_SET(0, NGPON_RX_TOD, CONG_THR, ASSERT, reg_cong_thr, assert);
    reg_cong_thr = RU_FIELD_SET(0, NGPON_RX_TOD, CONG_THR, NEGATE, reg_cong_thr, negate);

    RU_REG_WRITE(0, NGPON_RX_TOD, CONG_THR, reg_cong_thr);

    return 0;
}

int ag_drv_ngpon_rx_tod_cong_thr_get(uint8_t *assert, uint8_t *negate)
{
    uint32_t reg_cong_thr=0;

#ifdef VALIDATE_PARMS
    if(!assert || !negate)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_TOD, CONG_THR, reg_cong_thr);

    *assert = RU_FIELD_GET(0, NGPON_RX_TOD, CONG_THR, ASSERT, reg_cong_thr);
    *negate = RU_FIELD_GET(0, NGPON_RX_TOD, CONG_THR, NEGATE, reg_cong_thr);

    return 0;
}

int ag_drv_ngpon_rx_tod_update_time_ls_set(uint32_t value)
{
    uint32_t reg_update_time_ls=0;

#ifdef VALIDATE_PARMS
#endif

    reg_update_time_ls = RU_FIELD_SET(0, NGPON_RX_TOD, UPDATE_TIME_LS, VALUE, reg_update_time_ls, value);

    RU_REG_WRITE(0, NGPON_RX_TOD, UPDATE_TIME_LS, reg_update_time_ls);

    return 0;
}

int ag_drv_ngpon_rx_tod_update_time_ls_get(uint32_t *value)
{
    uint32_t reg_update_time_ls=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_TOD, UPDATE_TIME_LS, reg_update_time_ls);

    *value = RU_FIELD_GET(0, NGPON_RX_TOD, UPDATE_TIME_LS, VALUE, reg_update_time_ls);

    return 0;
}

int ag_drv_ngpon_rx_tod_update_time_ms_set(uint32_t ms_value)
{
    uint32_t reg_update_time_ms=0;

#ifdef VALIDATE_PARMS
    if((ms_value >= _19BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_update_time_ms = RU_FIELD_SET(0, NGPON_RX_TOD, UPDATE_TIME_MS, MS_VALUE, reg_update_time_ms, ms_value);

    RU_REG_WRITE(0, NGPON_RX_TOD, UPDATE_TIME_MS, reg_update_time_ms);

    return 0;
}

int ag_drv_ngpon_rx_tod_update_time_ms_get(uint32_t *ms_value)
{
    uint32_t reg_update_time_ms=0;

#ifdef VALIDATE_PARMS
    if(!ms_value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_TOD, UPDATE_TIME_MS, reg_update_time_ms);

    *ms_value = RU_FIELD_GET(0, NGPON_RX_TOD, UPDATE_TIME_MS, MS_VALUE, reg_update_time_ms);

    return 0;
}

int ag_drv_ngpon_rx_tod_update_val_set(uint32_t ngpon_rx_tod_value, uint32_t value)
{
    uint32_t reg_update_val=0;

#ifdef VALIDATE_PARMS
    if((ngpon_rx_tod_value >= 2))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_update_val = RU_FIELD_SET(0, NGPON_RX_TOD, UPDATE_VAL, VALUE, reg_update_val, value);

    RU_REG_RAM_WRITE(0, ngpon_rx_tod_value, NGPON_RX_TOD, UPDATE_VAL, reg_update_val);

    return 0;
}

int ag_drv_ngpon_rx_tod_update_val_get(uint32_t ngpon_rx_tod_value, uint32_t *value)
{
    uint32_t reg_update_val=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ngpon_rx_tod_value >= 2))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ngpon_rx_tod_value, NGPON_RX_TOD, UPDATE_VAL, reg_update_val);

    *value = RU_FIELD_GET(0, NGPON_RX_TOD, UPDATE_VAL, VALUE, reg_update_val);

    return 0;
}

int ag_drv_ngpon_rx_tod_update_arm_set(uint8_t arm, uint8_t en, uint8_t rdarm)
{
    uint32_t reg_update_arm=0;

#ifdef VALIDATE_PARMS
    if((arm >= _1BITS_MAX_VAL_) ||
       (en >= _1BITS_MAX_VAL_) ||
       (rdarm >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_update_arm = RU_FIELD_SET(0, NGPON_RX_TOD, UPDATE_ARM, ARM, reg_update_arm, arm);
    reg_update_arm = RU_FIELD_SET(0, NGPON_RX_TOD, UPDATE_ARM, EN, reg_update_arm, en);
    reg_update_arm = RU_FIELD_SET(0, NGPON_RX_TOD, UPDATE_ARM, RDARM, reg_update_arm, rdarm);

    RU_REG_WRITE(0, NGPON_RX_TOD, UPDATE_ARM, reg_update_arm);

    return 0;
}

int ag_drv_ngpon_rx_tod_update_arm_get(uint8_t *arm, uint8_t *en, uint8_t *rdarm)
{
    uint32_t reg_update_arm=0;

#ifdef VALIDATE_PARMS
    if(!arm || !en || !rdarm)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_TOD, UPDATE_ARM, reg_update_arm);

    *arm = RU_FIELD_GET(0, NGPON_RX_TOD, UPDATE_ARM, ARM, reg_update_arm);
    *en = RU_FIELD_GET(0, NGPON_RX_TOD, UPDATE_ARM, EN, reg_update_arm);
    *rdarm = RU_FIELD_GET(0, NGPON_RX_TOD, UPDATE_ARM, RDARM, reg_update_arm);

    return 0;
}

int ag_drv_ngpon_rx_tod_nano_in_secs_set(uint32_t value)
{
    uint32_t reg_nano_in_secs=0;

#ifdef VALIDATE_PARMS
    if((value >= _30BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_nano_in_secs = RU_FIELD_SET(0, NGPON_RX_TOD, NANO_IN_SECS, VALUE, reg_nano_in_secs, value);

    RU_REG_WRITE(0, NGPON_RX_TOD, NANO_IN_SECS, reg_nano_in_secs);

    return 0;
}

int ag_drv_ngpon_rx_tod_nano_in_secs_get(uint32_t *value)
{
    uint32_t reg_nano_in_secs=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_TOD, NANO_IN_SECS, reg_nano_in_secs);

    *value = RU_FIELD_GET(0, NGPON_RX_TOD, NANO_IN_SECS, VALUE, reg_nano_in_secs);

    return 0;
}

int ag_drv_ngpon_rx_tod_opps_set(uint32_t duty_cycle_time, uint8_t en)
{
    uint32_t reg_opps=0;

#ifdef VALIDATE_PARMS
    if((duty_cycle_time >= _30BITS_MAX_VAL_) ||
       (en >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_opps = RU_FIELD_SET(0, NGPON_RX_TOD, OPPS, DUTY_CYCLE_TIME, reg_opps, duty_cycle_time);
    reg_opps = RU_FIELD_SET(0, NGPON_RX_TOD, OPPS, EN, reg_opps, en);

    RU_REG_WRITE(0, NGPON_RX_TOD, OPPS, reg_opps);

    return 0;
}

int ag_drv_ngpon_rx_tod_opps_get(uint32_t *duty_cycle_time, uint8_t *en)
{
    uint32_t reg_opps=0;

#ifdef VALIDATE_PARMS
    if(!duty_cycle_time || !en)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_TOD, OPPS, reg_opps);

    *duty_cycle_time = RU_FIELD_GET(0, NGPON_RX_TOD, OPPS, DUTY_CYCLE_TIME, reg_opps);
    *en = RU_FIELD_GET(0, NGPON_RX_TOD, OPPS, EN, reg_opps);

    return 0;
}

int ag_drv_ngpon_rx_tod_nanos_per_cycle_set(uint32_t value)
{
    uint32_t reg_nanos_per_cycle=0;

#ifdef VALIDATE_PARMS
    if((value >= _30BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_nanos_per_cycle = RU_FIELD_SET(0, NGPON_RX_TOD, NANOS_PER_CYCLE, VALUE, reg_nanos_per_cycle, value);

    RU_REG_WRITE(0, NGPON_RX_TOD, NANOS_PER_CYCLE, reg_nanos_per_cycle);

    return 0;
}

int ag_drv_ngpon_rx_tod_nanos_per_cycle_get(uint32_t *value)
{
    uint32_t reg_nanos_per_cycle=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_TOD, NANOS_PER_CYCLE, reg_nanos_per_cycle);

    *value = RU_FIELD_GET(0, NGPON_RX_TOD, NANOS_PER_CYCLE, VALUE, reg_nanos_per_cycle);

    return 0;
}

int ag_drv_ngpon_rx_tod_partial_ns_inc_set(uint32_t value)
{
    uint32_t reg_partial_ns_inc=0;

#ifdef VALIDATE_PARMS
    if((value >= _30BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_partial_ns_inc = RU_FIELD_SET(0, NGPON_RX_TOD, PARTIAL_NS_INC, VALUE, reg_partial_ns_inc, value);

    RU_REG_WRITE(0, NGPON_RX_TOD, PARTIAL_NS_INC, reg_partial_ns_inc);

    return 0;
}

int ag_drv_ngpon_rx_tod_partial_ns_inc_get(uint32_t *value)
{
    uint32_t reg_partial_ns_inc=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_TOD, PARTIAL_NS_INC, reg_partial_ns_inc);

    *value = RU_FIELD_GET(0, NGPON_RX_TOD, PARTIAL_NS_INC, VALUE, reg_partial_ns_inc);

    return 0;
}

int ag_drv_ngpon_rx_tod_partial_ns_in_nano_set(uint32_t value)
{
    uint32_t reg_partial_ns_in_nano=0;

#ifdef VALIDATE_PARMS
    if((value >= _30BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_partial_ns_in_nano = RU_FIELD_SET(0, NGPON_RX_TOD, PARTIAL_NS_IN_NANO, VALUE, reg_partial_ns_in_nano, value);

    RU_REG_WRITE(0, NGPON_RX_TOD, PARTIAL_NS_IN_NANO, reg_partial_ns_in_nano);

    return 0;
}

int ag_drv_ngpon_rx_tod_partial_ns_in_nano_get(uint32_t *value)
{
    uint32_t reg_partial_ns_in_nano=0;

#ifdef VALIDATE_PARMS
    if(!value)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_TOD, PARTIAL_NS_IN_NANO, reg_partial_ns_in_nano);

    *value = RU_FIELD_GET(0, NGPON_RX_TOD, PARTIAL_NS_IN_NANO, VALUE, reg_partial_ns_in_nano);

    return 0;
}

int ag_drv_ngpon_rx_tod_ts_up_val_set(uint32_t ngpon_rx_tod_ts_up_value, uint32_t ts_val)
{
    uint32_t reg_ts_up_val=0;

#ifdef VALIDATE_PARMS
    if((ngpon_rx_tod_ts_up_value >= 2))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_ts_up_val = RU_FIELD_SET(0, NGPON_RX_TOD, TS_UP_VAL, TS_VAL, reg_ts_up_val, ts_val);

    RU_REG_RAM_WRITE(0, ngpon_rx_tod_ts_up_value, NGPON_RX_TOD, TS_UP_VAL, reg_ts_up_val);

    return 0;
}

int ag_drv_ngpon_rx_tod_ts_up_val_get(uint32_t ngpon_rx_tod_ts_up_value, uint32_t *ts_val)
{
    uint32_t reg_ts_up_val=0;

#ifdef VALIDATE_PARMS
    if(!ts_val)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ngpon_rx_tod_ts_up_value >= 2))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ngpon_rx_tod_ts_up_value, NGPON_RX_TOD, TS_UP_VAL, reg_ts_up_val);

    *ts_val = RU_FIELD_GET(0, NGPON_RX_TOD, TS_UP_VAL, TS_VAL, reg_ts_up_val);

    return 0;
}

int ag_drv_ngpon_rx_tod_tod_rd_val_get(uint32_t ngpon_rx_tod_tod_rd_value, uint32_t *ts_val)
{
    uint32_t reg_tod_rd_val=0;

#ifdef VALIDATE_PARMS
    if(!ts_val)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ngpon_rx_tod_tod_rd_value >= 2))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ngpon_rx_tod_tod_rd_value, NGPON_RX_TOD, TOD_RD_VAL, reg_tod_rd_val);

    *ts_val = RU_FIELD_GET(0, NGPON_RX_TOD, TOD_RD_VAL, TS_VAL, reg_tod_rd_val);

    return 0;
}

int ag_drv_ngpon_rx_tod_ts_rd_val_get(uint32_t ngpon_rx_tod_ts_rd_value, uint32_t *ts_val)
{
    uint32_t reg_ts_rd_val=0;

#ifdef VALIDATE_PARMS
    if(!ts_val)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ngpon_rx_tod_ts_rd_value >= 2))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ngpon_rx_tod_ts_rd_value, NGPON_RX_TOD, TS_RD_VAL, reg_ts_rd_val);

    *ts_val = RU_FIELD_GET(0, NGPON_RX_TOD, TS_RD_VAL, TS_VAL, reg_ts_rd_val);

    return 0;
}



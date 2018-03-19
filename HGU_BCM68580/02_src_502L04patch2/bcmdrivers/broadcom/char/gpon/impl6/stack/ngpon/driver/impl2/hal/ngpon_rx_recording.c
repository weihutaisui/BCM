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
#include "ngpon_rx_recording.h"
int ag_drv_ngpon_rx_recording_trig_set(uint8_t first_trig, uint8_t next_trig, uint16_t trig_delay)
{
    uint32_t reg_trig=0;

#ifdef VALIDATE_PARMS
    if((first_trig >= _2BITS_MAX_VAL_) ||
       (next_trig >= _2BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_trig = RU_FIELD_SET(0, NGPON_RX_RECORDING, TRIG, FIRST_TRIG, reg_trig, first_trig);
    reg_trig = RU_FIELD_SET(0, NGPON_RX_RECORDING, TRIG, NEXT_TRIG, reg_trig, next_trig);
    reg_trig = RU_FIELD_SET(0, NGPON_RX_RECORDING, TRIG, TRIG_DELAY, reg_trig, trig_delay);

    RU_REG_WRITE(0, NGPON_RX_RECORDING, TRIG, reg_trig);

    return 0;
}

int ag_drv_ngpon_rx_recording_trig_get(uint8_t *first_trig, uint8_t *next_trig, uint16_t *trig_delay)
{
    uint32_t reg_trig=0;

#ifdef VALIDATE_PARMS
    if(!first_trig || !next_trig || !trig_delay)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_RECORDING, TRIG, reg_trig);

    *first_trig = RU_FIELD_GET(0, NGPON_RX_RECORDING, TRIG, FIRST_TRIG, reg_trig);
    *next_trig = RU_FIELD_GET(0, NGPON_RX_RECORDING, TRIG, NEXT_TRIG, reg_trig);
    *trig_delay = RU_FIELD_GET(0, NGPON_RX_RECORDING, TRIG, TRIG_DELAY, reg_trig);

    return 0;
}

int ag_drv_ngpon_rx_recording_cfg_set(uint8_t stop, uint8_t filt, uint16_t spec)
{
    uint32_t reg_cfg=0;

#ifdef VALIDATE_PARMS
    if((stop >= _1BITS_MAX_VAL_) ||
       (filt >= _2BITS_MAX_VAL_) ||
       (spec >= _14BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_cfg = RU_FIELD_SET(0, NGPON_RX_RECORDING, CFG, STOP, reg_cfg, stop);
    reg_cfg = RU_FIELD_SET(0, NGPON_RX_RECORDING, CFG, FILT, reg_cfg, filt);
    reg_cfg = RU_FIELD_SET(0, NGPON_RX_RECORDING, CFG, SPEC, reg_cfg, spec);

    RU_REG_WRITE(0, NGPON_RX_RECORDING, CFG, reg_cfg);

    return 0;
}

int ag_drv_ngpon_rx_recording_cfg_get(uint8_t *stop, uint8_t *filt, uint16_t *spec)
{
    uint32_t reg_cfg=0;

#ifdef VALIDATE_PARMS
    if(!stop || !filt || !spec)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_RECORDING, CFG, reg_cfg);

    *stop = RU_FIELD_GET(0, NGPON_RX_RECORDING, CFG, STOP, reg_cfg);
    *filt = RU_FIELD_GET(0, NGPON_RX_RECORDING, CFG, FILT, reg_cfg);
    *spec = RU_FIELD_GET(0, NGPON_RX_RECORDING, CFG, SPEC, reg_cfg);

    return 0;
}

int ag_drv_ngpon_rx_recording_en_set(uint8_t en)
{
    uint32_t reg_en=0;

#ifdef VALIDATE_PARMS
    if((en >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_en = RU_FIELD_SET(0, NGPON_RX_RECORDING, EN, EN, reg_en, en);

    RU_REG_WRITE(0, NGPON_RX_RECORDING, EN, reg_en);

    return 0;
}

int ag_drv_ngpon_rx_recording_en_get(uint8_t *en)
{
    uint32_t reg_en=0;

#ifdef VALIDATE_PARMS
    if(!en)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_RECORDING, EN, reg_en);

    *en = RU_FIELD_GET(0, NGPON_RX_RECORDING, EN, EN, reg_en);

    return 0;
}

int ag_drv_ngpon_rx_recording_status_get(uint8_t *done, uint8_t *oom, uint8_t *empty, uint16_t *last_ptr)
{
    uint32_t reg_status=0;

#ifdef VALIDATE_PARMS
    if(!done || !oom || !empty || !last_ptr)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_RECORDING, STATUS, reg_status);

    *done = RU_FIELD_GET(0, NGPON_RX_RECORDING, STATUS, DONE, reg_status);
    *oom = RU_FIELD_GET(0, NGPON_RX_RECORDING, STATUS, OOM, reg_status);
    *empty = RU_FIELD_GET(0, NGPON_RX_RECORDING, STATUS, EMPTY, reg_status);
    *last_ptr = RU_FIELD_GET(0, NGPON_RX_RECORDING, STATUS, LAST_PTR, reg_status);

    return 0;
}

int ag_drv_ngpon_rx_recording_mem_data_get(uint32_t llrcd_mem_data, uint32_t *data)
{
    uint32_t reg_mem_data=0;

#ifdef VALIDATE_PARMS
    if(!data)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((llrcd_mem_data >= 256))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, llrcd_mem_data, NGPON_RX_RECORDING, MEM_DATA, reg_mem_data);

    *data = RU_FIELD_GET(0, NGPON_RX_RECORDING, MEM_DATA, DATA, reg_mem_data);

    return 0;
}


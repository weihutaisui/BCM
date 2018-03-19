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
#include "gpon_rx_bwmap_record.h"
int ag_drv_gpon_rx_bwmap_record_config_set(const gpon_rx_bwmap_record_config *config)
{
    uint32_t reg_config=0;

#ifdef VALIDATE_PARMS
    if(!config)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, 1);
        return 1;
    }
    if((config->rcd_stop_on_map_end >= _1BITS_MAX_VAL_) ||
       (config->rcd_all >= _1BITS_MAX_VAL_) ||
       (config->rcd_all_onu >= _1BITS_MAX_VAL_) ||
       (config->rcd_specific_alloc >= _1BITS_MAX_VAL_) ||
       (config->secific_alloc >= _12BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_config = RU_FIELD_SET(0, GPON_RX_BWMAP_RECORD, CONFIG, RCD_STOP_ON_MAP_END, reg_config, config->rcd_stop_on_map_end);
    reg_config = RU_FIELD_SET(0, GPON_RX_BWMAP_RECORD, CONFIG, RCD_ALL, reg_config, config->rcd_all);
    reg_config = RU_FIELD_SET(0, GPON_RX_BWMAP_RECORD, CONFIG, RCD_ALL_ONU, reg_config, config->rcd_all_onu);
    reg_config = RU_FIELD_SET(0, GPON_RX_BWMAP_RECORD, CONFIG, RCD_SPECIFIC_ALLOC, reg_config, config->rcd_specific_alloc);
    reg_config = RU_FIELD_SET(0, GPON_RX_BWMAP_RECORD, CONFIG, SECIFIC_ALLOC, reg_config, config->secific_alloc);

    RU_REG_WRITE(0, GPON_RX_BWMAP_RECORD, CONFIG, reg_config);

    return 0;
}

int ag_drv_gpon_rx_bwmap_record_config_get(gpon_rx_bwmap_record_config *config)
{
    uint32_t reg_config=0;

#ifdef VALIDATE_PARMS
    if(!config)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_RX_BWMAP_RECORD, CONFIG, reg_config);

    config->rcd_stop_on_map_end = RU_FIELD_GET(0, GPON_RX_BWMAP_RECORD, CONFIG, RCD_STOP_ON_MAP_END, reg_config);
    config->rcd_all = RU_FIELD_GET(0, GPON_RX_BWMAP_RECORD, CONFIG, RCD_ALL, reg_config);
    config->rcd_all_onu = RU_FIELD_GET(0, GPON_RX_BWMAP_RECORD, CONFIG, RCD_ALL_ONU, reg_config);
    config->rcd_specific_alloc = RU_FIELD_GET(0, GPON_RX_BWMAP_RECORD, CONFIG, RCD_SPECIFIC_ALLOC, reg_config);
    config->secific_alloc = RU_FIELD_GET(0, GPON_RX_BWMAP_RECORD, CONFIG, SECIFIC_ALLOC, reg_config);

    return 0;
}

int ag_drv_gpon_rx_bwmap_record_enable_set(uint8_t rcd_enable)
{
    uint32_t reg_enable=0;

#ifdef VALIDATE_PARMS
    if((rcd_enable >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_enable = RU_FIELD_SET(0, GPON_RX_BWMAP_RECORD, ENABLE, RCD_ENABLE, reg_enable, rcd_enable);

    RU_REG_WRITE(0, GPON_RX_BWMAP_RECORD, ENABLE, reg_enable);

    return 0;
}

int ag_drv_gpon_rx_bwmap_record_enable_get(uint8_t *rcd_enable)
{
    uint32_t reg_enable=0;

#ifdef VALIDATE_PARMS
    if(!rcd_enable)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_RX_BWMAP_RECORD, ENABLE, reg_enable);

    *rcd_enable = RU_FIELD_GET(0, GPON_RX_BWMAP_RECORD, ENABLE, RCD_ENABLE, reg_enable);

    return 0;
}

int ag_drv_gpon_rx_bwmap_record_status_get(uint8_t *rcd_done, uint8_t *rcd_mismatch, uint8_t *ecd_empty, uint8_t *rcd_last_ptr)
{
    uint32_t reg_status=0;

#ifdef VALIDATE_PARMS
    if(!rcd_done || !rcd_mismatch || !ecd_empty || !rcd_last_ptr)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_RX_BWMAP_RECORD, STATUS, reg_status);

    *rcd_done = RU_FIELD_GET(0, GPON_RX_BWMAP_RECORD, STATUS, RCD_DONE, reg_status);
    *rcd_mismatch = RU_FIELD_GET(0, GPON_RX_BWMAP_RECORD, STATUS, RCD_MISMATCH, reg_status);
    *ecd_empty = RU_FIELD_GET(0, GPON_RX_BWMAP_RECORD, STATUS, ECD_EMPTY, reg_status);
    *rcd_last_ptr = RU_FIELD_GET(0, GPON_RX_BWMAP_RECORD, STATUS, RCD_LAST_PTR, reg_status);

    return 0;
}

int ag_drv_gpon_rx_bwmap_record_read_if_set(uint8_t read_addr)
{
    uint32_t reg_read_if=0;

#ifdef VALIDATE_PARMS
    if((read_addr >= _6BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_read_if = RU_FIELD_SET(0, GPON_RX_BWMAP_RECORD, READ_IF, READ_ADDR, reg_read_if, read_addr);

    RU_REG_WRITE(0, GPON_RX_BWMAP_RECORD, READ_IF, reg_read_if);

    return 0;
}

int ag_drv_gpon_rx_bwmap_record_read_if_get(uint8_t *read_addr)
{
    uint32_t reg_read_if=0;

#ifdef VALIDATE_PARMS
    if(!read_addr)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_RX_BWMAP_RECORD, READ_IF, reg_read_if);

    *read_addr = RU_FIELD_GET(0, GPON_RX_BWMAP_RECORD, READ_IF, READ_ADDR, reg_read_if);

    return 0;
}

int ag_drv_gpon_rx_bwmap_record_rdata_get(uint32_t word_id, uint32_t *rcd_data)
{
    uint32_t reg_rdata=0;

#ifdef VALIDATE_PARMS
    if(!rcd_data)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((word_id >= 2))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, word_id, GPON_RX_BWMAP_RECORD, RDATA, reg_rdata);

    *rcd_data = RU_FIELD_GET(0, GPON_RX_BWMAP_RECORD, RDATA, RCD_DATA, reg_rdata);

    return 0;
}


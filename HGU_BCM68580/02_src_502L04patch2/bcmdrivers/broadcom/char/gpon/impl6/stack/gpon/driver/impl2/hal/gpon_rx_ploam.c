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
#include "gpon_rx_ploam.h"
int ag_drv_gpon_rx_ploam_onu_id_set(uint32_t onu_id_idx, uint8_t onu_id, uint8_t disable)
{
    uint32_t reg_onu_id=0;

#ifdef VALIDATE_PARMS
    if((onu_id_idx >= 2) ||
       (disable >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_onu_id = RU_FIELD_SET(0, GPON_RX_PLOAM, ONU_ID, ONU_ID, reg_onu_id, onu_id);
    reg_onu_id = RU_FIELD_SET(0, GPON_RX_PLOAM, ONU_ID, DISABLE, reg_onu_id, disable);

    RU_REG_RAM_WRITE(0, onu_id_idx, GPON_RX_PLOAM, ONU_ID, reg_onu_id);

    return 0;
}

int ag_drv_gpon_rx_ploam_onu_id_get(uint32_t onu_id_idx, uint8_t *onu_id, uint8_t *disable)
{
    uint32_t reg_onu_id=0;

#ifdef VALIDATE_PARMS
    if(!onu_id || !disable)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((onu_id_idx >= 2))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, onu_id_idx, GPON_RX_PLOAM, ONU_ID, reg_onu_id);

    *onu_id = RU_FIELD_GET(0, GPON_RX_PLOAM, ONU_ID, ONU_ID, reg_onu_id);
    *disable = RU_FIELD_GET(0, GPON_RX_PLOAM, ONU_ID, DISABLE, reg_onu_id);

    return 0;
}

int ag_drv_gpon_rx_ploam_params_set(uint8_t crc_overrd, uint8_t idle_filter, uint8_t fapl)
{
    uint32_t reg_params=0;

#ifdef VALIDATE_PARMS
    if((crc_overrd >= _1BITS_MAX_VAL_) ||
       (idle_filter >= _1BITS_MAX_VAL_) ||
       (fapl >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    reg_params = RU_FIELD_SET(0, GPON_RX_PLOAM, PARAMS, CRC_OVERRD, reg_params, crc_overrd);
    reg_params = RU_FIELD_SET(0, GPON_RX_PLOAM, PARAMS, IDLE_FILTER, reg_params, idle_filter);
    reg_params = RU_FIELD_SET(0, GPON_RX_PLOAM, PARAMS, FAPL, reg_params, fapl);

    RU_REG_WRITE(0, GPON_RX_PLOAM, PARAMS, reg_params);

    return 0;
}

int ag_drv_gpon_rx_ploam_params_get(uint8_t *crc_overrd, uint8_t *idle_filter, uint8_t *fapl)
{
    uint32_t reg_params=0;

#ifdef VALIDATE_PARMS
    if(!crc_overrd || !idle_filter || !fapl)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PLOAM, PARAMS, reg_params);

    *crc_overrd = RU_FIELD_GET(0, GPON_RX_PLOAM, PARAMS, CRC_OVERRD, reg_params);
    *idle_filter = RU_FIELD_GET(0, GPON_RX_PLOAM, PARAMS, IDLE_FILTER, reg_params);
    *fapl = RU_FIELD_GET(0, GPON_RX_PLOAM, PARAMS, FAPL, reg_params);

    return 0;
}


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
#include "ngpon_rx_assign.h"
int ag_drv_ngpon_rx_assign_flow_set(uint32_t flow_id, const ngpon_rx_assign_flow *flow)
{
    uint32_t reg_flow=0;

#ifdef VALIDATE_PARMS
    if(!flow)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, 1);
        return 1;
    }
    if((flow_id >= 256) ||
       (flow->priority >= _2BITS_MAX_VAL_) ||
       (flow->enc_ring >= _2BITS_MAX_VAL_) ||
       (flow->crc_en >= _1BITS_MAX_VAL_) ||
       (flow->en >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_flow = RU_FIELD_SET(0, NGPON_RX_ASSIGN, FLOW, PORTID, reg_flow, flow->portid);
    reg_flow = RU_FIELD_SET(0, NGPON_RX_ASSIGN, FLOW, PRIORITY, reg_flow, flow->priority);
    reg_flow = RU_FIELD_SET(0, NGPON_RX_ASSIGN, FLOW, ENC_RING, reg_flow, flow->enc_ring);
    reg_flow = RU_FIELD_SET(0, NGPON_RX_ASSIGN, FLOW, CRC_EN, reg_flow, flow->crc_en);
    reg_flow = RU_FIELD_SET(0, NGPON_RX_ASSIGN, FLOW, EN, reg_flow, flow->en);

    RU_REG_RAM_WRITE(0, flow_id, NGPON_RX_ASSIGN, FLOW, reg_flow);

    return 0;
}

int ag_drv_ngpon_rx_assign_flow_get(uint32_t flow_id, ngpon_rx_assign_flow *flow)
{
    uint32_t reg_flow=0;

#ifdef VALIDATE_PARMS
    if(!flow)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((flow_id >= 256))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, flow_id, NGPON_RX_ASSIGN, FLOW, reg_flow);

    flow->portid = RU_FIELD_GET(0, NGPON_RX_ASSIGN, FLOW, PORTID, reg_flow);
    flow->priority = RU_FIELD_GET(0, NGPON_RX_ASSIGN, FLOW, PRIORITY, reg_flow);
    flow->enc_ring = RU_FIELD_GET(0, NGPON_RX_ASSIGN, FLOW, ENC_RING, reg_flow);
    flow->crc_en = RU_FIELD_GET(0, NGPON_RX_ASSIGN, FLOW, CRC_EN, reg_flow);
    flow->en = RU_FIELD_GET(0, NGPON_RX_ASSIGN, FLOW, EN, reg_flow);

    return 0;
}

int ag_drv_ngpon_rx_assign_last_flow_set(uint16_t mask)
{
    uint32_t reg_last_flow=0;

#ifdef VALIDATE_PARMS
#endif

    reg_last_flow = RU_FIELD_SET(0, NGPON_RX_ASSIGN, LAST_FLOW, MASK, reg_last_flow, mask);

    RU_REG_WRITE(0, NGPON_RX_ASSIGN, LAST_FLOW, reg_last_flow);

    return 0;
}

int ag_drv_ngpon_rx_assign_last_flow_get(uint16_t *mask)
{
    uint32_t reg_last_flow=0;

#ifdef VALIDATE_PARMS
    if(!mask)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_ASSIGN, LAST_FLOW, reg_last_flow);

    *mask = RU_FIELD_GET(0, NGPON_RX_ASSIGN, LAST_FLOW, MASK, reg_last_flow);

    return 0;
}


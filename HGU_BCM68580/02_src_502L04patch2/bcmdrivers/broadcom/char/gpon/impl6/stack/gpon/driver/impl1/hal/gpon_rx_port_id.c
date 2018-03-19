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
#include "gpon_rx_port_id.h"
int ag_drv_gpon_rx_port_id_fullcfg_set(uint32_t port_id_idx, uint16_t portid, uint8_t encrypt, uint8_t type, uint8_t priority)
{
    uint32_t reg_fullcfg=0;

#ifdef VALIDATE_PARMS
    if((port_id_idx >= 32) ||
       (portid >= _12BITS_MAX_VAL_) ||
       (encrypt >= _1BITS_MAX_VAL_) ||
       (type >= _1BITS_MAX_VAL_) ||
       (priority >= _2BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_fullcfg = RU_FIELD_SET(0, GPON_RX_PORT_ID, FULLCFG, PORTID, reg_fullcfg, portid);
    reg_fullcfg = RU_FIELD_SET(0, GPON_RX_PORT_ID, FULLCFG, ENCRYPT, reg_fullcfg, encrypt);
    reg_fullcfg = RU_FIELD_SET(0, GPON_RX_PORT_ID, FULLCFG, TYPE, reg_fullcfg, type);
    reg_fullcfg = RU_FIELD_SET(0, GPON_RX_PORT_ID, FULLCFG, PRIORITY, reg_fullcfg, priority);

    RU_REG_RAM_WRITE(0, port_id_idx, GPON_RX_PORT_ID, FULLCFG, reg_fullcfg);

    return 0;
}

int ag_drv_gpon_rx_port_id_fullcfg_get(uint32_t port_id_idx, uint16_t *portid, uint8_t *encrypt, uint8_t *type, uint8_t *priority)
{
    uint32_t reg_fullcfg=0;

#ifdef VALIDATE_PARMS
    if(!portid || !encrypt || !type || !priority)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((port_id_idx >= 32))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, port_id_idx, GPON_RX_PORT_ID, FULLCFG, reg_fullcfg);

    *portid = RU_FIELD_GET(0, GPON_RX_PORT_ID, FULLCFG, PORTID, reg_fullcfg);
    *encrypt = RU_FIELD_GET(0, GPON_RX_PORT_ID, FULLCFG, ENCRYPT, reg_fullcfg);
    *type = RU_FIELD_GET(0, GPON_RX_PORT_ID, FULLCFG, TYPE, reg_fullcfg);
    *priority = RU_FIELD_GET(0, GPON_RX_PORT_ID, FULLCFG, PRIORITY, reg_fullcfg);

    return 0;
}

int ag_drv_gpon_rx_port_id_cfg_set(uint32_t port_id_idx, uint16_t portid, uint8_t encrypt)
{
    uint32_t reg_cfg=0;

#ifdef VALIDATE_PARMS
    if((port_id_idx >= 224) ||
       (portid >= _12BITS_MAX_VAL_) ||
       (encrypt >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_cfg = RU_FIELD_SET(0, GPON_RX_PORT_ID, CFG, PORTID, reg_cfg, portid);
    reg_cfg = RU_FIELD_SET(0, GPON_RX_PORT_ID, CFG, ENCRYPT, reg_cfg, encrypt);

    RU_REG_RAM_WRITE(0, port_id_idx, GPON_RX_PORT_ID, CFG, reg_cfg);

    return 0;
}

int ag_drv_gpon_rx_port_id_cfg_get(uint32_t port_id_idx, uint16_t *portid, uint8_t *encrypt)
{
    uint32_t reg_cfg=0;

#ifdef VALIDATE_PARMS
    if(!portid || !encrypt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((port_id_idx >= 224))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, port_id_idx, GPON_RX_PORT_ID, CFG, reg_cfg);

    *portid = RU_FIELD_GET(0, GPON_RX_PORT_ID, CFG, PORTID, reg_cfg);
    *encrypt = RU_FIELD_GET(0, GPON_RX_PORT_ID, CFG, ENCRYPT, reg_cfg);

    return 0;
}

int ag_drv_gpon_rx_port_id_disable_set(uint32_t port_id_idx, uint8_t port_id)
{
    uint32_t reg_disable=0;

#ifdef VALIDATE_PARMS
    if((port_id_idx >= 256))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, port_id_idx / 32, GPON_RX_PORT_ID, DISABLE, reg_disable);

    FIELD_SET(reg_disable, (port_id_idx % 32) *1, 0x1, port_id);

    RU_REG_RAM_WRITE(0, port_id_idx / 32, GPON_RX_PORT_ID, DISABLE, reg_disable);

    return 0;
}

int ag_drv_gpon_rx_port_id_disable_get(uint32_t port_id_idx, uint8_t *port_id)
{
    uint32_t reg_disable=0;

#ifdef VALIDATE_PARMS
    if(!port_id)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((port_id_idx >= 256))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, port_id_idx / 32, GPON_RX_PORT_ID, DISABLE, reg_disable);

    *port_id = FIELD_GET(reg_disable, (port_id_idx % 32) *1, 0x1);

    return 0;
}

int ag_drv_gpon_rx_port_id_mask_set(uint16_t portidmsk)
{
    uint32_t reg_mask=0;

#ifdef VALIDATE_PARMS
    if((portidmsk >= _12BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_mask = RU_FIELD_SET(0, GPON_RX_PORT_ID, MASK, PORTIDMSK, reg_mask, portidmsk);

    RU_REG_WRITE(0, GPON_RX_PORT_ID, MASK, reg_mask);

    return 0;
}

int ag_drv_gpon_rx_port_id_mask_get(uint16_t *portidmsk)
{
    uint32_t reg_mask=0;

#ifdef VALIDATE_PARMS
    if(!portidmsk)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PORT_ID, MASK, reg_mask);

    *portidmsk = RU_FIELD_GET(0, GPON_RX_PORT_ID, MASK, PORTIDMSK, reg_mask);

    return 0;
}

int ag_drv_gpon_rx_port_id_lastind_set(uint8_t ptilast)
{
    uint32_t reg_lastind=0;

#ifdef VALIDATE_PARMS
#endif

    reg_lastind = RU_FIELD_SET(0, GPON_RX_PORT_ID, LASTIND, PTILAST, reg_lastind, ptilast);

    RU_REG_WRITE(0, GPON_RX_PORT_ID, LASTIND, reg_lastind);

    return 0;
}

int ag_drv_gpon_rx_port_id_lastind_get(uint8_t *ptilast)
{
    uint32_t reg_lastind=0;

#ifdef VALIDATE_PARMS
    if(!ptilast)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PORT_ID, LASTIND, reg_lastind);

    *ptilast = RU_FIELD_GET(0, GPON_RX_PORT_ID, LASTIND, PTILAST, reg_lastind);

    return 0;
}


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
#include "gpon_rx_port_id.h"
bdmf_error_t ag_drv_gpon_rx_port_id_fullcfg_set(uint32_t port_id_idx, uint16_t portid, bdmf_boolean encrypt, bdmf_boolean type, uint8_t priority)
{
    uint32_t reg_fullcfg=0;

#ifdef VALIDATE_PARMS
    if((port_id_idx >= 32) ||
       (portid >= _12BITS_MAX_VAL_) ||
       (encrypt >= _1BITS_MAX_VAL_) ||
       (type >= _1BITS_MAX_VAL_) ||
       (priority >= _2BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_fullcfg = RU_FIELD_SET(0, GPON_RX_PORT_ID, FULLCFG, PORTID, reg_fullcfg, portid);
    reg_fullcfg = RU_FIELD_SET(0, GPON_RX_PORT_ID, FULLCFG, ENCRYPT, reg_fullcfg, encrypt);
    reg_fullcfg = RU_FIELD_SET(0, GPON_RX_PORT_ID, FULLCFG, TYPE, reg_fullcfg, type);
    reg_fullcfg = RU_FIELD_SET(0, GPON_RX_PORT_ID, FULLCFG, PRIORITY, reg_fullcfg, priority);

    RU_REG_RAM_WRITE(0, port_id_idx, GPON_RX_PORT_ID, FULLCFG, reg_fullcfg);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_port_id_fullcfg_get(uint32_t port_id_idx, uint16_t *portid, bdmf_boolean *encrypt, bdmf_boolean *type, uint8_t *priority)
{
    uint32_t reg_fullcfg=0;

#ifdef VALIDATE_PARMS
    if(!portid || !encrypt || !type || !priority)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
    if((port_id_idx >= 32))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, port_id_idx, GPON_RX_PORT_ID, FULLCFG, reg_fullcfg);

    *portid = RU_FIELD_GET(0, GPON_RX_PORT_ID, FULLCFG, PORTID, reg_fullcfg);
    *encrypt = RU_FIELD_GET(0, GPON_RX_PORT_ID, FULLCFG, ENCRYPT, reg_fullcfg);
    *type = RU_FIELD_GET(0, GPON_RX_PORT_ID, FULLCFG, TYPE, reg_fullcfg);
    *priority = RU_FIELD_GET(0, GPON_RX_PORT_ID, FULLCFG, PRIORITY, reg_fullcfg);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_port_id_cfg_set(uint32_t port_id_idx, uint16_t portid, bdmf_boolean encrypt)
{
    uint32_t reg_cfg=0;

#ifdef VALIDATE_PARMS
    if((port_id_idx >= 224) ||
       (portid >= _12BITS_MAX_VAL_) ||
       (encrypt >= _1BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_cfg = RU_FIELD_SET(0, GPON_RX_PORT_ID, CFG, PORTID, reg_cfg, portid);
    reg_cfg = RU_FIELD_SET(0, GPON_RX_PORT_ID, CFG, ENCRYPT, reg_cfg, encrypt);

    RU_REG_RAM_WRITE(0, port_id_idx, GPON_RX_PORT_ID, CFG, reg_cfg);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_port_id_cfg_get(uint32_t port_id_idx, uint16_t *portid, bdmf_boolean *encrypt)
{
    uint32_t reg_cfg=0;

#ifdef VALIDATE_PARMS
    if(!portid || !encrypt)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
    if((port_id_idx >= 224))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, port_id_idx, GPON_RX_PORT_ID, CFG, reg_cfg);

    *portid = RU_FIELD_GET(0, GPON_RX_PORT_ID, CFG, PORTID, reg_cfg);
    *encrypt = RU_FIELD_GET(0, GPON_RX_PORT_ID, CFG, ENCRYPT, reg_cfg);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_port_id_disable_set(uint32_t port_id_idx, bdmf_boolean port_id)
{
    uint32_t reg_disable=0;

#ifdef VALIDATE_PARMS
    if((port_id_idx >= 256))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, port_id_idx / 32, GPON_RX_PORT_ID, DISABLE, reg_disable);

    FIELD_SET(reg_disable, (port_id_idx % 32) *1, 0x1, port_id);

    RU_REG_RAM_WRITE(0, port_id_idx / 32, GPON_RX_PORT_ID, DISABLE, reg_disable);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_port_id_disable_get(uint32_t port_id_idx, bdmf_boolean *port_id)
{
    uint32_t reg_disable=0;

#ifdef VALIDATE_PARMS
    if(!port_id)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
    if((port_id_idx >= 256))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, port_id_idx / 32, GPON_RX_PORT_ID, DISABLE, reg_disable);

    *port_id = FIELD_GET(reg_disable, (port_id_idx % 32) *1, 0x1);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_port_id_mask_set(uint16_t portidmsk)
{
    uint32_t reg_mask=0;

#ifdef VALIDATE_PARMS
    if((portidmsk >= _12BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_mask = RU_FIELD_SET(0, GPON_RX_PORT_ID, MASK, PORTIDMSK, reg_mask, portidmsk);

    RU_REG_WRITE(0, GPON_RX_PORT_ID, MASK, reg_mask);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_port_id_mask_get(uint16_t *portidmsk)
{
    uint32_t reg_mask=0;

#ifdef VALIDATE_PARMS
    if(!portidmsk)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PORT_ID, MASK, reg_mask);

    *portidmsk = RU_FIELD_GET(0, GPON_RX_PORT_ID, MASK, PORTIDMSK, reg_mask);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_port_id_lastind_set(uint8_t ptilast)
{
    uint32_t reg_lastind=0;

#ifdef VALIDATE_PARMS
#endif

    reg_lastind = RU_FIELD_SET(0, GPON_RX_PORT_ID, LASTIND, PTILAST, reg_lastind, ptilast);

    RU_REG_WRITE(0, GPON_RX_PORT_ID, LASTIND, reg_lastind);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_port_id_lastind_get(uint8_t *ptilast)
{
    uint32_t reg_lastind=0;

#ifdef VALIDATE_PARMS
    if(!ptilast)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PORT_ID, LASTIND, reg_lastind);

    *ptilast = RU_FIELD_GET(0, GPON_RX_PORT_ID, LASTIND, PTILAST, reg_lastind);

    return BDMF_ERR_OK;
}

#ifdef USE_BDMF_SHELL
enum
{
    BDMF_fullcfg,
    BDMF_cfg,
    BDMF_disable,
    BDMF_mask,
    BDMF_lastind,
};

typedef enum
{
    bdmf_address_fullcfg,
    bdmf_address_cfg,
    bdmf_address_disable,
    bdmf_address_mask,
    bdmf_address_lastind,
}
bdmf_address;

static int bcm_gpon_rx_port_id_cli_set(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_error_t err;

    switch(parm[0].value.unumber)
    {
    case BDMF_fullcfg:
        err = ag_drv_gpon_rx_port_id_fullcfg_set(parm[1].value.unumber, parm[2].value.unumber, parm[3].value.unumber, parm[4].value.unumber, parm[5].value.unumber);
        break;
    case BDMF_cfg:
        err = ag_drv_gpon_rx_port_id_cfg_set(parm[1].value.unumber, parm[2].value.unumber, parm[3].value.unumber);
        break;
    case BDMF_disable:
        err = ag_drv_gpon_rx_port_id_disable_set(parm[1].value.unumber, parm[2].value.unumber);
        break;
    case BDMF_mask:
        err = ag_drv_gpon_rx_port_id_mask_set(parm[1].value.unumber);
        break;
    case BDMF_lastind:
        err = ag_drv_gpon_rx_port_id_lastind_set(parm[1].value.unumber);
        break;
    default:
        err = BDMF_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

static int bcm_gpon_rx_port_id_cli_get(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_error_t err;

    switch(parm[0].value.unumber)
    {
    case BDMF_fullcfg:
    {
        uint16_t portid;
        bdmf_boolean encrypt;
        bdmf_boolean type;
        uint8_t priority;
        err = ag_drv_gpon_rx_port_id_fullcfg_get(parm[1].value.unumber, &portid, &encrypt, &type, &priority);
        bdmf_session_print(session, "portid = %u = 0x%x\n", portid, portid);
        bdmf_session_print(session, "encrypt = %u = 0x%x\n", encrypt, encrypt);
        bdmf_session_print(session, "type = %u = 0x%x\n", type, type);
        bdmf_session_print(session, "priority = %u = 0x%x\n", priority, priority);
        break;
    }
    case BDMF_cfg:
    {
        uint16_t portid;
        bdmf_boolean encrypt;
        err = ag_drv_gpon_rx_port_id_cfg_get(parm[1].value.unumber, &portid, &encrypt);
        bdmf_session_print(session, "portid = %u = 0x%x\n", portid, portid);
        bdmf_session_print(session, "encrypt = %u = 0x%x\n", encrypt, encrypt);
        break;
    }
    case BDMF_disable:
    {
        bdmf_boolean port_id;
        err = ag_drv_gpon_rx_port_id_disable_get(parm[1].value.unumber, &port_id);
        bdmf_session_print(session, "port_id = %u = 0x%x\n", port_id, port_id);
        break;
    }
    case BDMF_mask:
    {
        uint16_t portidmsk;
        err = ag_drv_gpon_rx_port_id_mask_get(&portidmsk);
        bdmf_session_print(session, "portidmsk = %u = 0x%x\n", portidmsk, portidmsk);
        break;
    }
    case BDMF_lastind:
    {
        uint8_t ptilast;
        err = ag_drv_gpon_rx_port_id_lastind_get(&ptilast);
        bdmf_session_print(session, "ptilast = %u = 0x%x\n", ptilast, ptilast);
        break;
    }
    default:
        err = BDMF_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

static int bcm_gpon_rx_port_id_cli_test(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_test_method m = parm[0].value.unumber;
    bdmf_error_t err = BDMF_ERR_OK;

    {
        uint32_t port_id_idx=gtmv(m, 5);
        uint16_t portid=gtmv(m, 12);
        bdmf_boolean encrypt=gtmv(m, 1);
        bdmf_boolean type=gtmv(m, 1);
        uint8_t priority=gtmv(m, 2);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_port_id_fullcfg_set( %u %u %u %u %u)\n", port_id_idx, portid, encrypt, type, priority);
        if(!err) ag_drv_gpon_rx_port_id_fullcfg_set(port_id_idx, portid, encrypt, type, priority);
        if(!err) ag_drv_gpon_rx_port_id_fullcfg_get( port_id_idx, &portid, &encrypt, &type, &priority);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_port_id_fullcfg_get( %u %u %u %u %u)\n", port_id_idx, portid, encrypt, type, priority);
        if(err || portid!=gtmv(m, 12) || encrypt!=gtmv(m, 1) || type!=gtmv(m, 1) || priority!=gtmv(m, 2))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t port_id_idx=gtmv(m, 5);
        uint16_t portid=gtmv(m, 12);
        bdmf_boolean encrypt=gtmv(m, 1);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_port_id_cfg_set( %u %u %u)\n", port_id_idx, portid, encrypt);
        if(!err) ag_drv_gpon_rx_port_id_cfg_set(port_id_idx, portid, encrypt);
        if(!err) ag_drv_gpon_rx_port_id_cfg_get( port_id_idx, &portid, &encrypt);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_port_id_cfg_get( %u %u %u)\n", port_id_idx, portid, encrypt);
        if(err || portid!=gtmv(m, 12) || encrypt!=gtmv(m, 1))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t port_id_idx=gtmv(m, 8);
        bdmf_boolean port_id=gtmv(m, 1);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_port_id_disable_set( %u %u)\n", port_id_idx, port_id);
        if(!err) ag_drv_gpon_rx_port_id_disable_set(port_id_idx, port_id);
        if(!err) ag_drv_gpon_rx_port_id_disable_get( port_id_idx, &port_id);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_port_id_disable_get( %u %u)\n", port_id_idx, port_id);
        if(err || port_id!=gtmv(m, 1))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint16_t portidmsk=gtmv(m, 12);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_port_id_mask_set( %u)\n", portidmsk);
        if(!err) ag_drv_gpon_rx_port_id_mask_set(portidmsk);
        if(!err) ag_drv_gpon_rx_port_id_mask_get( &portidmsk);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_port_id_mask_get( %u)\n", portidmsk);
        if(err || portidmsk!=gtmv(m, 12))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint8_t ptilast=gtmv(m, 8);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_port_id_lastind_set( %u)\n", ptilast);
        if(!err) ag_drv_gpon_rx_port_id_lastind_set(ptilast);
        if(!err) ag_drv_gpon_rx_port_id_lastind_get( &ptilast);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_port_id_lastind_get( %u)\n", ptilast);
        if(err || ptilast!=gtmv(m, 8))
            return err ? err : BDMF_ERR_IO;
    }
    return err;
}

static int bcm_gpon_rx_port_id_cli_address(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
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
    case bdmf_address_fullcfg : reg = &RU_REG(GPON_RX_PORT_ID, FULLCFG); blk = &RU_BLK(GPON_RX_PORT_ID); break;
    case bdmf_address_cfg : reg = &RU_REG(GPON_RX_PORT_ID, CFG); blk = &RU_BLK(GPON_RX_PORT_ID); break;
    case bdmf_address_disable : reg = &RU_REG(GPON_RX_PORT_ID, DISABLE); blk = &RU_BLK(GPON_RX_PORT_ID); break;
    case bdmf_address_mask : reg = &RU_REG(GPON_RX_PORT_ID, MASK); blk = &RU_BLK(GPON_RX_PORT_ID); break;
    case bdmf_address_lastind : reg = &RU_REG(GPON_RX_PORT_ID, LASTIND); blk = &RU_BLK(GPON_RX_PORT_ID); break;
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

bdmfmon_handle_t ag_drv_gpon_rx_port_id_cli_init(bdmfmon_handle_t driver_dir)
{
    bdmfmon_handle_t dir;

    if ((dir = bdmfmon_dir_find(driver_dir, "gpon_rx_port_id"))!=NULL)
        return dir;
    dir = bdmfmon_dir_add(driver_dir, "gpon_rx_port_id", "gpon_rx_port_id", BDMF_ACCESS_ADMIN, NULL);

    {
        static bdmfmon_cmd_parm_t set_fullcfg[]={
            BDMFMON_MAKE_PARM("port_id_idx", "port_id_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("portid", "portid", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("encrypt", "encrypt", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("type", "type", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("priority", "priority", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_cfg[]={
            BDMFMON_MAKE_PARM("port_id_idx", "port_id_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("portid", "portid", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("encrypt", "encrypt", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_disable[]={
            BDMFMON_MAKE_PARM("port_id_idx", "port_id_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("port_id", "port_id", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_mask[]={
            BDMFMON_MAKE_PARM("portidmsk", "portidmsk", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_lastind[]={
            BDMFMON_MAKE_PARM("ptilast", "ptilast", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_enum_val_t selector_table[] = {
            { .name="fullcfg", .val=BDMF_fullcfg, .parms=set_fullcfg },
            { .name="cfg", .val=BDMF_cfg, .parms=set_cfg },
            { .name="disable", .val=BDMF_disable, .parms=set_disable },
            { .name="mask", .val=BDMF_mask, .parms=set_mask },
            { .name="lastind", .val=BDMF_lastind, .parms=set_lastind },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "set", "set", bcm_gpon_rx_port_id_cli_set,
            BDMFMON_MAKE_PARM_SELECTOR("purpose", "purpose", selector_table, 0));
    }
    {
        static bdmfmon_cmd_parm_t set_default[]={
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_fullcfg[]={
            BDMFMON_MAKE_PARM("port_id_idx", "port_id_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_cfg[]={
            BDMFMON_MAKE_PARM("port_id_idx", "port_id_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_disable[]={
            BDMFMON_MAKE_PARM("port_id_idx", "port_id_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_enum_val_t selector_table[] = {
            { .name="fullcfg", .val=BDMF_fullcfg, .parms=set_fullcfg },
            { .name="cfg", .val=BDMF_cfg, .parms=set_cfg },
            { .name="disable", .val=BDMF_disable, .parms=set_disable },
            { .name="mask", .val=BDMF_mask, .parms=set_default },
            { .name="lastind", .val=BDMF_lastind, .parms=set_default },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "get", "get", bcm_gpon_rx_port_id_cli_get,
            BDMFMON_MAKE_PARM_SELECTOR("purpose", "purpose", selector_table, 0));
    }
    {
        static bdmfmon_enum_val_t enum_table_test_method[] = {
            { .name="low" , .val=bdmf_test_method_low },
            { .name="mid" , .val=bdmf_test_method_mid },
            { .name="high" , .val=bdmf_test_method_high },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "test", "test", bcm_gpon_rx_port_id_cli_test,
            BDMFMON_MAKE_PARM_ENUM("method", "low: 0000, mid: 1000, high: 1111", enum_table_test_method, 0));
    }
    {
        static bdmfmon_enum_val_t enum_table_address[] = {
            { .name="FULLCFG" , .val=bdmf_address_fullcfg },
            { .name="CFG" , .val=bdmf_address_cfg },
            { .name="DISABLE" , .val=bdmf_address_disable },
            { .name="MASK" , .val=bdmf_address_mask },
            { .name="LASTIND" , .val=bdmf_address_lastind },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "address", "address", bcm_gpon_rx_port_id_cli_address,
            BDMFMON_MAKE_PARM_ENUM("method", "method", enum_table_address, 0),
            BDMFMON_MAKE_PARM("index2", "onu_id/alloc_id/port_id/etc...", BDMFMON_PARM_NUMBER, BDMFMON_PARM_FLAG_OPTIONAL));
    }
    return dir;
}
#endif /* USE_BDMF_SHELL */


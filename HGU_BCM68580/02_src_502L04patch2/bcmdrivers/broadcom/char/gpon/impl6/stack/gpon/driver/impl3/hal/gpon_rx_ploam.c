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
#include "gpon_rx_ploam.h"
bdmf_error_t ag_drv_gpon_rx_ploam_onu_id_set(uint32_t onu_id_idx, uint8_t onu_id, bdmf_boolean disable)
{
    uint32_t reg_onu_id=0;

#ifdef VALIDATE_PARMS
    if((onu_id_idx >= 2) ||
       (disable >= _1BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_onu_id = RU_FIELD_SET(0, GPON_RX_PLOAM, ONU_ID, ONU_ID, reg_onu_id, onu_id);
    reg_onu_id = RU_FIELD_SET(0, GPON_RX_PLOAM, ONU_ID, DISABLE, reg_onu_id, disable);

    RU_REG_RAM_WRITE(0, onu_id_idx, GPON_RX_PLOAM, ONU_ID, reg_onu_id);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_ploam_onu_id_get(uint32_t onu_id_idx, uint8_t *onu_id, bdmf_boolean *disable)
{
    uint32_t reg_onu_id=0;

#ifdef VALIDATE_PARMS
    if(!onu_id || !disable)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
    if((onu_id_idx >= 2))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, onu_id_idx, GPON_RX_PLOAM, ONU_ID, reg_onu_id);

    *onu_id = RU_FIELD_GET(0, GPON_RX_PLOAM, ONU_ID, ONU_ID, reg_onu_id);
    *disable = RU_FIELD_GET(0, GPON_RX_PLOAM, ONU_ID, DISABLE, reg_onu_id);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_ploam_params_set(bdmf_boolean crc_overrd, bdmf_boolean idle_filter, bdmf_boolean fapl)
{
    uint32_t reg_params=0;

#ifdef VALIDATE_PARMS
    if((crc_overrd >= _1BITS_MAX_VAL_) ||
       (idle_filter >= _1BITS_MAX_VAL_) ||
       (fapl >= _1BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_params = RU_FIELD_SET(0, GPON_RX_PLOAM, PARAMS, CRC_OVERRD, reg_params, crc_overrd);
    reg_params = RU_FIELD_SET(0, GPON_RX_PLOAM, PARAMS, IDLE_FILTER, reg_params, idle_filter);
    reg_params = RU_FIELD_SET(0, GPON_RX_PLOAM, PARAMS, FAPL, reg_params, fapl);

    RU_REG_WRITE(0, GPON_RX_PLOAM, PARAMS, reg_params);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_ploam_params_get(bdmf_boolean *crc_overrd, bdmf_boolean *idle_filter, bdmf_boolean *fapl)
{
    uint32_t reg_params=0;

#ifdef VALIDATE_PARMS
    if(!crc_overrd || !idle_filter || !fapl)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PLOAM, PARAMS, reg_params);

    *crc_overrd = RU_FIELD_GET(0, GPON_RX_PLOAM, PARAMS, CRC_OVERRD, reg_params);
    *idle_filter = RU_FIELD_GET(0, GPON_RX_PLOAM, PARAMS, IDLE_FILTER, reg_params);
    *fapl = RU_FIELD_GET(0, GPON_RX_PLOAM, PARAMS, FAPL, reg_params);

    return BDMF_ERR_OK;
}

#ifdef USE_BDMF_SHELL
enum
{
    BDMF_onu_id,
    BDMF_params,
};

typedef enum
{
    bdmf_address_onu_id,
    bdmf_address_params,
}
bdmf_address;

static int bcm_gpon_rx_ploam_cli_set(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_error_t err;

    switch(parm[0].value.unumber)
    {
    case BDMF_onu_id:
        err = ag_drv_gpon_rx_ploam_onu_id_set(parm[1].value.unumber, parm[2].value.unumber, parm[3].value.unumber);
        break;
    case BDMF_params:
        err = ag_drv_gpon_rx_ploam_params_set(parm[1].value.unumber, parm[2].value.unumber, parm[3].value.unumber);
        break;
    default:
        err = BDMF_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

static int bcm_gpon_rx_ploam_cli_get(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_error_t err;

    switch(parm[0].value.unumber)
    {
    case BDMF_onu_id:
    {
        uint8_t onu_id;
        bdmf_boolean disable;
        err = ag_drv_gpon_rx_ploam_onu_id_get(parm[1].value.unumber, &onu_id, &disable);
        bdmf_session_print(session, "onu_id = %u = 0x%x\n", onu_id, onu_id);
        bdmf_session_print(session, "disable = %u = 0x%x\n", disable, disable);
        break;
    }
    case BDMF_params:
    {
        bdmf_boolean crc_overrd;
        bdmf_boolean idle_filter;
        bdmf_boolean fapl;
        err = ag_drv_gpon_rx_ploam_params_get(&crc_overrd, &idle_filter, &fapl);
        bdmf_session_print(session, "crc_overrd = %u = 0x%x\n", crc_overrd, crc_overrd);
        bdmf_session_print(session, "idle_filter = %u = 0x%x\n", idle_filter, idle_filter);
        bdmf_session_print(session, "fapl = %u = 0x%x\n", fapl, fapl);
        break;
    }
    default:
        err = BDMF_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

static int bcm_gpon_rx_ploam_cli_test(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_test_method m = parm[0].value.unumber;
    bdmf_error_t err = BDMF_ERR_OK;

    {
        uint32_t onu_id_idx=gtmv(m, 1);
        uint8_t onu_id=gtmv(m, 8);
        bdmf_boolean disable=gtmv(m, 1);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_ploam_onu_id_set( %u %u %u)\n", onu_id_idx, onu_id, disable);
        if(!err) ag_drv_gpon_rx_ploam_onu_id_set(onu_id_idx, onu_id, disable);
        if(!err) ag_drv_gpon_rx_ploam_onu_id_get( onu_id_idx, &onu_id, &disable);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_ploam_onu_id_get( %u %u %u)\n", onu_id_idx, onu_id, disable);
        if(err || onu_id!=gtmv(m, 8) || disable!=gtmv(m, 1))
            return err ? err : BDMF_ERR_IO;
    }
    {
        bdmf_boolean crc_overrd=gtmv(m, 1);
        bdmf_boolean idle_filter=gtmv(m, 1);
        bdmf_boolean fapl=gtmv(m, 1);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_ploam_params_set( %u %u %u)\n", crc_overrd, idle_filter, fapl);
        if(!err) ag_drv_gpon_rx_ploam_params_set(crc_overrd, idle_filter, fapl);
        if(!err) ag_drv_gpon_rx_ploam_params_get( &crc_overrd, &idle_filter, &fapl);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_ploam_params_get( %u %u %u)\n", crc_overrd, idle_filter, fapl);
        if(err || crc_overrd!=gtmv(m, 1) || idle_filter!=gtmv(m, 1) || fapl!=gtmv(m, 1))
            return err ? err : BDMF_ERR_IO;
    }
    return err;
}

static int bcm_gpon_rx_ploam_cli_address(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
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
    case bdmf_address_onu_id : reg = &RU_REG(GPON_RX_PLOAM, ONU_ID); blk = &RU_BLK(GPON_RX_PLOAM); break;
    case bdmf_address_params : reg = &RU_REG(GPON_RX_PLOAM, PARAMS); blk = &RU_BLK(GPON_RX_PLOAM); break;
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

bdmfmon_handle_t ag_drv_gpon_rx_ploam_cli_init(bdmfmon_handle_t driver_dir)
{
    bdmfmon_handle_t dir;

    if ((dir = bdmfmon_dir_find(driver_dir, "gpon_rx_ploam"))!=NULL)
        return dir;
    dir = bdmfmon_dir_add(driver_dir, "gpon_rx_ploam", "gpon_rx_ploam", BDMF_ACCESS_ADMIN, NULL);

    {
        static bdmfmon_cmd_parm_t set_onu_id[]={
            BDMFMON_MAKE_PARM("onu_id_idx", "onu_id_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("onu_id", "onu_id", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("disable", "disable", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_params[]={
            BDMFMON_MAKE_PARM("crc_overrd", "crc_overrd", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("idle_filter", "idle_filter", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("fapl", "fapl", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_enum_val_t selector_table[] = {
            { .name="onu_id", .val=BDMF_onu_id, .parms=set_onu_id },
            { .name="params", .val=BDMF_params, .parms=set_params },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "set", "set", bcm_gpon_rx_ploam_cli_set,
            BDMFMON_MAKE_PARM_SELECTOR("purpose", "purpose", selector_table, 0));
    }
    {
        static bdmfmon_cmd_parm_t set_default[]={
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_onu_id[]={
            BDMFMON_MAKE_PARM("onu_id_idx", "onu_id_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_enum_val_t selector_table[] = {
            { .name="onu_id", .val=BDMF_onu_id, .parms=set_onu_id },
            { .name="params", .val=BDMF_params, .parms=set_default },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "get", "get", bcm_gpon_rx_ploam_cli_get,
            BDMFMON_MAKE_PARM_SELECTOR("purpose", "purpose", selector_table, 0));
    }
    {
        static bdmfmon_enum_val_t enum_table_test_method[] = {
            { .name="low" , .val=bdmf_test_method_low },
            { .name="mid" , .val=bdmf_test_method_mid },
            { .name="high" , .val=bdmf_test_method_high },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "test", "test", bcm_gpon_rx_ploam_cli_test,
            BDMFMON_MAKE_PARM_ENUM("method", "low: 0000, mid: 1000, high: 1111", enum_table_test_method, 0));
    }
    {
        static bdmfmon_enum_val_t enum_table_address[] = {
            { .name="ONU_ID" , .val=bdmf_address_onu_id },
            { .name="PARAMS" , .val=bdmf_address_params },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "address", "address", bcm_gpon_rx_ploam_cli_address,
            BDMFMON_MAKE_PARM_ENUM("method", "method", enum_table_address, 0),
            BDMFMON_MAKE_PARM("index2", "onu_id/alloc_id/port_id/etc...", BDMFMON_PARM_NUMBER, BDMFMON_PARM_FLAG_OPTIONAL));
    }
    return dir;
}
#endif /* USE_BDMF_SHELL */


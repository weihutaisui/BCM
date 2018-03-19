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
#include "gpon_rx_short_frame.h"
bdmf_error_t ag_drv_gpon_rx_short_frame_short0_set(uint8_t cwforif, uint16_t cycinfr)
{
    uint32_t reg_short0=0;

#ifdef VALIDATE_PARMS
    if((cycinfr >= _15BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_short0 = RU_FIELD_SET(0, GPON_RX_SHORT_FRAME, SHORT0, CWFORIF, reg_short0, cwforif);
    reg_short0 = RU_FIELD_SET(0, GPON_RX_SHORT_FRAME, SHORT0, CYCINFR, reg_short0, cycinfr);

    RU_REG_WRITE(0, GPON_RX_SHORT_FRAME, SHORT0, reg_short0);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_short_frame_short0_get(uint8_t *cwforif, uint16_t *cycinfr)
{
    uint32_t reg_short0=0;

#ifdef VALIDATE_PARMS
    if(!cwforif || !cycinfr)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_SHORT_FRAME, SHORT0, reg_short0);

    *cwforif = RU_FIELD_GET(0, GPON_RX_SHORT_FRAME, SHORT0, CWFORIF, reg_short0);
    *cycinfr = RU_FIELD_GET(0, GPON_RX_SHORT_FRAME, SHORT0, CYCINFR, reg_short0);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_short_frame_short1_set(uint8_t zpcr0, uint8_t zpcr1, uint8_t cwinfr)
{
    uint32_t reg_short1=0;

#ifdef VALIDATE_PARMS
#endif

    reg_short1 = RU_FIELD_SET(0, GPON_RX_SHORT_FRAME, SHORT1, ZPCR0, reg_short1, zpcr0);
    reg_short1 = RU_FIELD_SET(0, GPON_RX_SHORT_FRAME, SHORT1, ZPCR1, reg_short1, zpcr1);
    reg_short1 = RU_FIELD_SET(0, GPON_RX_SHORT_FRAME, SHORT1, CWINFR, reg_short1, cwinfr);

    RU_REG_WRITE(0, GPON_RX_SHORT_FRAME, SHORT1, reg_short1);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_short_frame_short1_get(uint8_t *zpcr0, uint8_t *zpcr1, uint8_t *cwinfr)
{
    uint32_t reg_short1=0;

#ifdef VALIDATE_PARMS
    if(!zpcr0 || !zpcr1 || !cwinfr)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_SHORT_FRAME, SHORT1, reg_short1);

    *zpcr0 = RU_FIELD_GET(0, GPON_RX_SHORT_FRAME, SHORT1, ZPCR0, reg_short1);
    *zpcr1 = RU_FIELD_GET(0, GPON_RX_SHORT_FRAME, SHORT1, ZPCR1, reg_short1);
    *cwinfr = RU_FIELD_GET(0, GPON_RX_SHORT_FRAME, SHORT1, CWINFR, reg_short1);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_short_frame_short2_set(uint8_t bytescw1, uint8_t bytescw0)
{
    uint32_t reg_short2=0;

#ifdef VALIDATE_PARMS
#endif

    reg_short2 = RU_FIELD_SET(0, GPON_RX_SHORT_FRAME, SHORT2, BYTESCW1, reg_short2, bytescw1);
    reg_short2 = RU_FIELD_SET(0, GPON_RX_SHORT_FRAME, SHORT2, BYTESCW0, reg_short2, bytescw0);

    RU_REG_WRITE(0, GPON_RX_SHORT_FRAME, SHORT2, reg_short2);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_short_frame_short2_get(uint8_t *bytescw1, uint8_t *bytescw0)
{
    uint32_t reg_short2=0;

#ifdef VALIDATE_PARMS
    if(!bytescw1 || !bytescw0)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_SHORT_FRAME, SHORT2, reg_short2);

    *bytescw1 = RU_FIELD_GET(0, GPON_RX_SHORT_FRAME, SHORT2, BYTESCW1, reg_short2);
    *bytescw0 = RU_FIELD_GET(0, GPON_RX_SHORT_FRAME, SHORT2, BYTESCW0, reg_short2);

    return BDMF_ERR_OK;
}

#ifdef USE_BDMF_SHELL
enum
{
    BDMF_short0,
    BDMF_short1,
    BDMF_short2,
};

typedef enum
{
    bdmf_address_short0,
    bdmf_address_short1,
    bdmf_address_short2,
}
bdmf_address;

static int bcm_gpon_rx_short_frame_cli_set(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_error_t err;

    switch(parm[0].value.unumber)
    {
    case BDMF_short0:
        err = ag_drv_gpon_rx_short_frame_short0_set(parm[1].value.unumber, parm[2].value.unumber);
        break;
    case BDMF_short1:
        err = ag_drv_gpon_rx_short_frame_short1_set(parm[1].value.unumber, parm[2].value.unumber, parm[3].value.unumber);
        break;
    case BDMF_short2:
        err = ag_drv_gpon_rx_short_frame_short2_set(parm[1].value.unumber, parm[2].value.unumber);
        break;
    default:
        err = BDMF_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

static int bcm_gpon_rx_short_frame_cli_get(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_error_t err;

    switch(parm[0].value.unumber)
    {
    case BDMF_short0:
    {
        uint8_t cwforif;
        uint16_t cycinfr;
        err = ag_drv_gpon_rx_short_frame_short0_get(&cwforif, &cycinfr);
        bdmf_session_print(session, "cwforif = %u = 0x%x\n", cwforif, cwforif);
        bdmf_session_print(session, "cycinfr = %u = 0x%x\n", cycinfr, cycinfr);
        break;
    }
    case BDMF_short1:
    {
        uint8_t zpcr0;
        uint8_t zpcr1;
        uint8_t cwinfr;
        err = ag_drv_gpon_rx_short_frame_short1_get(&zpcr0, &zpcr1, &cwinfr);
        bdmf_session_print(session, "zpcr0 = %u = 0x%x\n", zpcr0, zpcr0);
        bdmf_session_print(session, "zpcr1 = %u = 0x%x\n", zpcr1, zpcr1);
        bdmf_session_print(session, "cwinfr = %u = 0x%x\n", cwinfr, cwinfr);
        break;
    }
    case BDMF_short2:
    {
        uint8_t bytescw1;
        uint8_t bytescw0;
        err = ag_drv_gpon_rx_short_frame_short2_get(&bytescw1, &bytescw0);
        bdmf_session_print(session, "bytescw1 = %u = 0x%x\n", bytescw1, bytescw1);
        bdmf_session_print(session, "bytescw0 = %u = 0x%x\n", bytescw0, bytescw0);
        break;
    }
    default:
        err = BDMF_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

static int bcm_gpon_rx_short_frame_cli_test(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_test_method m = parm[0].value.unumber;
    bdmf_error_t err = BDMF_ERR_OK;

    {
        uint8_t cwforif=gtmv(m, 8);
        uint16_t cycinfr=gtmv(m, 15);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_short_frame_short0_set( %u %u)\n", cwforif, cycinfr);
        if(!err) ag_drv_gpon_rx_short_frame_short0_set(cwforif, cycinfr);
        if(!err) ag_drv_gpon_rx_short_frame_short0_get( &cwforif, &cycinfr);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_short_frame_short0_get( %u %u)\n", cwforif, cycinfr);
        if(err || cwforif!=gtmv(m, 8) || cycinfr!=gtmv(m, 15))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint8_t zpcr0=gtmv(m, 8);
        uint8_t zpcr1=gtmv(m, 8);
        uint8_t cwinfr=gtmv(m, 8);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_short_frame_short1_set( %u %u %u)\n", zpcr0, zpcr1, cwinfr);
        if(!err) ag_drv_gpon_rx_short_frame_short1_set(zpcr0, zpcr1, cwinfr);
        if(!err) ag_drv_gpon_rx_short_frame_short1_get( &zpcr0, &zpcr1, &cwinfr);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_short_frame_short1_get( %u %u %u)\n", zpcr0, zpcr1, cwinfr);
        if(err || zpcr0!=gtmv(m, 8) || zpcr1!=gtmv(m, 8) || cwinfr!=gtmv(m, 8))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint8_t bytescw1=gtmv(m, 8);
        uint8_t bytescw0=gtmv(m, 8);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_short_frame_short2_set( %u %u)\n", bytescw1, bytescw0);
        if(!err) ag_drv_gpon_rx_short_frame_short2_set(bytescw1, bytescw0);
        if(!err) ag_drv_gpon_rx_short_frame_short2_get( &bytescw1, &bytescw0);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_short_frame_short2_get( %u %u)\n", bytescw1, bytescw0);
        if(err || bytescw1!=gtmv(m, 8) || bytescw0!=gtmv(m, 8))
            return err ? err : BDMF_ERR_IO;
    }
    return err;
}

static int bcm_gpon_rx_short_frame_cli_address(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
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
    case bdmf_address_short0 : reg = &RU_REG(GPON_RX_SHORT_FRAME, SHORT0); blk = &RU_BLK(GPON_RX_SHORT_FRAME); break;
    case bdmf_address_short1 : reg = &RU_REG(GPON_RX_SHORT_FRAME, SHORT1); blk = &RU_BLK(GPON_RX_SHORT_FRAME); break;
    case bdmf_address_short2 : reg = &RU_REG(GPON_RX_SHORT_FRAME, SHORT2); blk = &RU_BLK(GPON_RX_SHORT_FRAME); break;
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

bdmfmon_handle_t ag_drv_gpon_rx_short_frame_cli_init(bdmfmon_handle_t driver_dir)
{
    bdmfmon_handle_t dir;

    if ((dir = bdmfmon_dir_find(driver_dir, "gpon_rx_short_frame"))!=NULL)
        return dir;
    dir = bdmfmon_dir_add(driver_dir, "gpon_rx_short_frame", "gpon_rx_short_frame", BDMF_ACCESS_ADMIN, NULL);

    {
        static bdmfmon_cmd_parm_t set_short0[]={
            BDMFMON_MAKE_PARM("cwforif", "cwforif", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("cycinfr", "cycinfr", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_short1[]={
            BDMFMON_MAKE_PARM("zpcr0", "zpcr0", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("zpcr1", "zpcr1", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("cwinfr", "cwinfr", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_short2[]={
            BDMFMON_MAKE_PARM("bytescw1", "bytescw1", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("bytescw0", "bytescw0", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_enum_val_t selector_table[] = {
            { .name="short0", .val=BDMF_short0, .parms=set_short0 },
            { .name="short1", .val=BDMF_short1, .parms=set_short1 },
            { .name="short2", .val=BDMF_short2, .parms=set_short2 },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "set", "set", bcm_gpon_rx_short_frame_cli_set,
            BDMFMON_MAKE_PARM_SELECTOR("purpose", "purpose", selector_table, 0));
    }
    {
        static bdmfmon_cmd_parm_t set_default[]={
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_enum_val_t selector_table[] = {
            { .name="short0", .val=BDMF_short0, .parms=set_default },
            { .name="short1", .val=BDMF_short1, .parms=set_default },
            { .name="short2", .val=BDMF_short2, .parms=set_default },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "get", "get", bcm_gpon_rx_short_frame_cli_get,
            BDMFMON_MAKE_PARM_SELECTOR("purpose", "purpose", selector_table, 0));
    }
    {
        static bdmfmon_enum_val_t enum_table_test_method[] = {
            { .name="low" , .val=bdmf_test_method_low },
            { .name="mid" , .val=bdmf_test_method_mid },
            { .name="high" , .val=bdmf_test_method_high },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "test", "test", bcm_gpon_rx_short_frame_cli_test,
            BDMFMON_MAKE_PARM_ENUM("method", "low: 0000, mid: 1000, high: 1111", enum_table_test_method, 0));
    }
    {
        static bdmfmon_enum_val_t enum_table_address[] = {
            { .name="SHORT0" , .val=bdmf_address_short0 },
            { .name="SHORT1" , .val=bdmf_address_short1 },
            { .name="SHORT2" , .val=bdmf_address_short2 },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "address", "address", bcm_gpon_rx_short_frame_cli_address,
            BDMFMON_MAKE_PARM_ENUM("method", "method", enum_table_address, 0),
            BDMFMON_MAKE_PARM("index2", "onu_id/alloc_id/port_id/etc...", BDMFMON_PARM_NUMBER, BDMFMON_PARM_FLAG_OPTIONAL));
    }
    return dir;
}
#endif /* USE_BDMF_SHELL */


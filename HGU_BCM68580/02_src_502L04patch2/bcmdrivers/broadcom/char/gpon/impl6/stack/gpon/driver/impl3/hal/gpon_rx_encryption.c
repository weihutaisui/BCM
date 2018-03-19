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
#include "gpon_rx_encryption.h"
bdmf_error_t ag_drv_gpon_rx_encryption_sf_cntr_get(uint32_t *sf_cntr)
{
    uint32_t reg_sf_cntr=0;

#ifdef VALIDATE_PARMS
    if(!sf_cntr)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_ENCRYPTION, SF_CNTR, reg_sf_cntr);

    *sf_cntr = RU_FIELD_GET(0, GPON_RX_ENCRYPTION, SF_CNTR, SF_CNTR, reg_sf_cntr);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_encryption_key_set(uint32_t word_idx, uint32_t key)
{
    uint32_t reg_key=0;

#ifdef VALIDATE_PARMS
    if((word_idx >= 4))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_key = RU_FIELD_SET(0, GPON_RX_ENCRYPTION, KEY, KEY, reg_key, key);

    RU_REG_RAM_WRITE(0, word_idx, GPON_RX_ENCRYPTION, KEY, reg_key);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_encryption_key_get(uint32_t word_idx, uint32_t *key)
{
    uint32_t reg_key=0;

#ifdef VALIDATE_PARMS
    if(!key)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
    if((word_idx >= 4))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, word_idx, GPON_RX_ENCRYPTION, KEY, reg_key);

    *key = RU_FIELD_GET(0, GPON_RX_ENCRYPTION, KEY, KEY, reg_key);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_encryption_switch_time_set(uint32_t sw_time, bdmf_boolean arm)
{
    uint32_t reg_switch_time=0;

#ifdef VALIDATE_PARMS
    if((sw_time >= _30BITS_MAX_VAL_) ||
       (arm >= _1BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_switch_time = RU_FIELD_SET(0, GPON_RX_ENCRYPTION, SWITCH_TIME, SW_TIME, reg_switch_time, sw_time);
    reg_switch_time = RU_FIELD_SET(0, GPON_RX_ENCRYPTION, SWITCH_TIME, ARM, reg_switch_time, arm);

    RU_REG_WRITE(0, GPON_RX_ENCRYPTION, SWITCH_TIME, reg_switch_time);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_encryption_switch_time_get(uint32_t *sw_time, bdmf_boolean *arm)
{
    uint32_t reg_switch_time=0;

#ifdef VALIDATE_PARMS
    if(!sw_time || !arm)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_ENCRYPTION, SWITCH_TIME, reg_switch_time);

    *sw_time = RU_FIELD_GET(0, GPON_RX_ENCRYPTION, SWITCH_TIME, SW_TIME, reg_switch_time);
    *arm = RU_FIELD_GET(0, GPON_RX_ENCRYPTION, SWITCH_TIME, ARM, reg_switch_time);

    return BDMF_ERR_OK;
}

#ifdef USE_BDMF_SHELL
enum
{
    BDMF_sf_cntr,
    BDMF_key,
    BDMF_switch_time,
};

typedef enum
{
    bdmf_address_sf_cntr,
    bdmf_address_key,
    bdmf_address_switch_time,
}
bdmf_address;

static int bcm_gpon_rx_encryption_cli_set(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_error_t err;

    switch(parm[0].value.unumber)
    {
    case BDMF_key:
        err = ag_drv_gpon_rx_encryption_key_set(parm[1].value.unumber, parm[2].value.unumber);
        break;
    case BDMF_switch_time:
        err = ag_drv_gpon_rx_encryption_switch_time_set(parm[1].value.unumber, parm[2].value.unumber);
        break;
    default:
        err = BDMF_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

static int bcm_gpon_rx_encryption_cli_get(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_error_t err;

    switch(parm[0].value.unumber)
    {
    case BDMF_sf_cntr:
    {
        uint32_t sf_cntr;
        err = ag_drv_gpon_rx_encryption_sf_cntr_get(&sf_cntr);
        bdmf_session_print(session, "sf_cntr = %u = 0x%x\n", sf_cntr, sf_cntr);
        break;
    }
    case BDMF_key:
    {
        uint32_t key;
        err = ag_drv_gpon_rx_encryption_key_get(parm[1].value.unumber, &key);
        bdmf_session_print(session, "key = %u = 0x%x\n", key, key);
        break;
    }
    case BDMF_switch_time:
    {
        uint32_t sw_time;
        bdmf_boolean arm;
        err = ag_drv_gpon_rx_encryption_switch_time_get(&sw_time, &arm);
        bdmf_session_print(session, "sw_time = %u = 0x%x\n", sw_time, sw_time);
        bdmf_session_print(session, "arm = %u = 0x%x\n", arm, arm);
        break;
    }
    default:
        err = BDMF_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

static int bcm_gpon_rx_encryption_cli_test(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_test_method m = parm[0].value.unumber;
    bdmf_error_t err = BDMF_ERR_OK;

    {
        uint32_t sf_cntr=gtmv(m, 30);
        if(!err) ag_drv_gpon_rx_encryption_sf_cntr_get( &sf_cntr);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_encryption_sf_cntr_get( %u)\n", sf_cntr);
    }
    {
        uint32_t word_idx=gtmv(m, 2);
        uint32_t key=gtmv(m, 32);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_encryption_key_set( %u %u)\n", word_idx, key);
        if(!err) ag_drv_gpon_rx_encryption_key_set(word_idx, key);
        if(!err) ag_drv_gpon_rx_encryption_key_get( word_idx, &key);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_encryption_key_get( %u %u)\n", word_idx, key);
        if(err || key!=gtmv(m, 32))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t sw_time=gtmv(m, 30);
        bdmf_boolean arm=gtmv(m, 1);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_encryption_switch_time_set( %u %u)\n", sw_time, arm);
        if(!err) ag_drv_gpon_rx_encryption_switch_time_set(sw_time, arm);
        if(!err) ag_drv_gpon_rx_encryption_switch_time_get( &sw_time, &arm);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_encryption_switch_time_get( %u %u)\n", sw_time, arm);
        if(err || sw_time!=gtmv(m, 30) || arm!=gtmv(m, 1))
            return err ? err : BDMF_ERR_IO;
    }
    return err;
}

static int bcm_gpon_rx_encryption_cli_address(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
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
    case bdmf_address_sf_cntr : reg = &RU_REG(GPON_RX_ENCRYPTION, SF_CNTR); blk = &RU_BLK(GPON_RX_ENCRYPTION); break;
    case bdmf_address_key : reg = &RU_REG(GPON_RX_ENCRYPTION, KEY); blk = &RU_BLK(GPON_RX_ENCRYPTION); break;
    case bdmf_address_switch_time : reg = &RU_REG(GPON_RX_ENCRYPTION, SWITCH_TIME); blk = &RU_BLK(GPON_RX_ENCRYPTION); break;
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

bdmfmon_handle_t ag_drv_gpon_rx_encryption_cli_init(bdmfmon_handle_t driver_dir)
{
    bdmfmon_handle_t dir;

    if ((dir = bdmfmon_dir_find(driver_dir, "gpon_rx_encryption"))!=NULL)
        return dir;
    dir = bdmfmon_dir_add(driver_dir, "gpon_rx_encryption", "gpon_rx_encryption", BDMF_ACCESS_ADMIN, NULL);

    {
        static bdmfmon_cmd_parm_t set_key[]={
            BDMFMON_MAKE_PARM("word_idx", "word_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("key", "key", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_switch_time[]={
            BDMFMON_MAKE_PARM("sw_time", "sw_time", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("arm", "arm", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_enum_val_t selector_table[] = {
            { .name="key", .val=BDMF_key, .parms=set_key },
            { .name="switch_time", .val=BDMF_switch_time, .parms=set_switch_time },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "set", "set", bcm_gpon_rx_encryption_cli_set,
            BDMFMON_MAKE_PARM_SELECTOR("purpose", "purpose", selector_table, 0));
    }
    {
        static bdmfmon_cmd_parm_t set_default[]={
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_key[]={
            BDMFMON_MAKE_PARM("word_idx", "word_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_enum_val_t selector_table[] = {
            { .name="sf_cntr", .val=BDMF_sf_cntr, .parms=set_default },
            { .name="key", .val=BDMF_key, .parms=set_key },
            { .name="switch_time", .val=BDMF_switch_time, .parms=set_default },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "get", "get", bcm_gpon_rx_encryption_cli_get,
            BDMFMON_MAKE_PARM_SELECTOR("purpose", "purpose", selector_table, 0));
    }
    {
        static bdmfmon_enum_val_t enum_table_test_method[] = {
            { .name="low" , .val=bdmf_test_method_low },
            { .name="mid" , .val=bdmf_test_method_mid },
            { .name="high" , .val=bdmf_test_method_high },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "test", "test", bcm_gpon_rx_encryption_cli_test,
            BDMFMON_MAKE_PARM_ENUM("method", "low: 0000, mid: 1000, high: 1111", enum_table_test_method, 0));
    }
    {
        static bdmfmon_enum_val_t enum_table_address[] = {
            { .name="SF_CNTR" , .val=bdmf_address_sf_cntr },
            { .name="KEY" , .val=bdmf_address_key },
            { .name="SWITCH_TIME" , .val=bdmf_address_switch_time },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "address", "address", bcm_gpon_rx_encryption_cli_address,
            BDMFMON_MAKE_PARM_ENUM("method", "method", enum_table_address, 0),
            BDMFMON_MAKE_PARM("index2", "onu_id/alloc_id/port_id/etc...", BDMFMON_PARM_NUMBER, BDMFMON_PARM_FLAG_OPTIONAL));
    }
    return dir;
}
#endif /* USE_BDMF_SHELL */


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
#include "gpon_rx_bwmap_record.h"
bdmf_error_t ag_drv_gpon_rx_bwmap_record_config_set(const gpon_rx_bwmap_record_config *config)
{
    uint32_t reg_config=0;

#ifdef VALIDATE_PARMS
    if(!config)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
    if((config->rcd_stop_on_map_end >= _1BITS_MAX_VAL_) ||
       (config->rcd_all >= _1BITS_MAX_VAL_) ||
       (config->rcd_all_onu >= _1BITS_MAX_VAL_) ||
       (config->rcd_specific_alloc >= _1BITS_MAX_VAL_) ||
       (config->secific_alloc >= _12BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_config = RU_FIELD_SET(0, GPON_RX_BWMAP_RECORD, CONFIG, RCD_STOP_ON_MAP_END, reg_config, config->rcd_stop_on_map_end);
    reg_config = RU_FIELD_SET(0, GPON_RX_BWMAP_RECORD, CONFIG, RCD_ALL, reg_config, config->rcd_all);
    reg_config = RU_FIELD_SET(0, GPON_RX_BWMAP_RECORD, CONFIG, RCD_ALL_ONU, reg_config, config->rcd_all_onu);
    reg_config = RU_FIELD_SET(0, GPON_RX_BWMAP_RECORD, CONFIG, RCD_SPECIFIC_ALLOC, reg_config, config->rcd_specific_alloc);
    reg_config = RU_FIELD_SET(0, GPON_RX_BWMAP_RECORD, CONFIG, SECIFIC_ALLOC, reg_config, config->secific_alloc);

    RU_REG_WRITE(0, GPON_RX_BWMAP_RECORD, CONFIG, reg_config);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_bwmap_record_config_get(gpon_rx_bwmap_record_config *config)
{
    uint32_t reg_config=0;

#ifdef VALIDATE_PARMS
    if(!config)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_BWMAP_RECORD, CONFIG, reg_config);

    config->rcd_stop_on_map_end = RU_FIELD_GET(0, GPON_RX_BWMAP_RECORD, CONFIG, RCD_STOP_ON_MAP_END, reg_config);
    config->rcd_all = RU_FIELD_GET(0, GPON_RX_BWMAP_RECORD, CONFIG, RCD_ALL, reg_config);
    config->rcd_all_onu = RU_FIELD_GET(0, GPON_RX_BWMAP_RECORD, CONFIG, RCD_ALL_ONU, reg_config);
    config->rcd_specific_alloc = RU_FIELD_GET(0, GPON_RX_BWMAP_RECORD, CONFIG, RCD_SPECIFIC_ALLOC, reg_config);
    config->secific_alloc = RU_FIELD_GET(0, GPON_RX_BWMAP_RECORD, CONFIG, SECIFIC_ALLOC, reg_config);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_bwmap_record_enable_set(bdmf_boolean rcd_enable)
{
    uint32_t reg_enable=0;

#ifdef VALIDATE_PARMS
    if((rcd_enable >= _1BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_enable = RU_FIELD_SET(0, GPON_RX_BWMAP_RECORD, ENABLE, RCD_ENABLE, reg_enable, rcd_enable);

    RU_REG_WRITE(0, GPON_RX_BWMAP_RECORD, ENABLE, reg_enable);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_bwmap_record_enable_get(bdmf_boolean *rcd_enable)
{
    uint32_t reg_enable=0;

#ifdef VALIDATE_PARMS
    if(!rcd_enable)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_BWMAP_RECORD, ENABLE, reg_enable);

    *rcd_enable = RU_FIELD_GET(0, GPON_RX_BWMAP_RECORD, ENABLE, RCD_ENABLE, reg_enable);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_bwmap_record_status_get(bdmf_boolean *rcd_done, bdmf_boolean *rcd_mismatch, bdmf_boolean *ecd_empty, uint8_t *rcd_last_ptr)
{
    uint32_t reg_status=0;

#ifdef VALIDATE_PARMS
    if(!rcd_done || !rcd_mismatch || !ecd_empty || !rcd_last_ptr)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_BWMAP_RECORD, STATUS, reg_status);

    *rcd_done = RU_FIELD_GET(0, GPON_RX_BWMAP_RECORD, STATUS, RCD_DONE, reg_status);
    *rcd_mismatch = RU_FIELD_GET(0, GPON_RX_BWMAP_RECORD, STATUS, RCD_MISMATCH, reg_status);
    *ecd_empty = RU_FIELD_GET(0, GPON_RX_BWMAP_RECORD, STATUS, ECD_EMPTY, reg_status);
    *rcd_last_ptr = RU_FIELD_GET(0, GPON_RX_BWMAP_RECORD, STATUS, RCD_LAST_PTR, reg_status);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_bwmap_record_read_if_set(uint8_t read_addr)
{
    uint32_t reg_read_if=0;

#ifdef VALIDATE_PARMS
    if((read_addr >= _6BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_read_if = RU_FIELD_SET(0, GPON_RX_BWMAP_RECORD, READ_IF, READ_ADDR, reg_read_if, read_addr);

    RU_REG_WRITE(0, GPON_RX_BWMAP_RECORD, READ_IF, reg_read_if);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_bwmap_record_read_if_get(uint8_t *read_addr)
{
    uint32_t reg_read_if=0;

#ifdef VALIDATE_PARMS
    if(!read_addr)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_BWMAP_RECORD, READ_IF, reg_read_if);

    *read_addr = RU_FIELD_GET(0, GPON_RX_BWMAP_RECORD, READ_IF, READ_ADDR, reg_read_if);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_bwmap_record_rdata_get(uint32_t word_id, uint32_t *rcd_data)
{
    uint32_t reg_rdata=0;

#ifdef VALIDATE_PARMS
    if(!rcd_data)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
    if((word_id >= 2))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, word_id, GPON_RX_BWMAP_RECORD, RDATA, reg_rdata);

    *rcd_data = RU_FIELD_GET(0, GPON_RX_BWMAP_RECORD, RDATA, RCD_DATA, reg_rdata);

    return BDMF_ERR_OK;
}

#ifdef USE_BDMF_SHELL
enum
{
    BDMF_config,
    BDMF_enable,
    BDMF_status,
    BDMF_read_if,
    BDMF_rdata,
};

typedef enum
{
    bdmf_address_config,
    bdmf_address_enable,
    bdmf_address_status,
    bdmf_address_read_if,
    bdmf_address_rdata,
}
bdmf_address;

static int bcm_gpon_rx_bwmap_record_cli_set(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_error_t err;

    switch(parm[0].value.unumber)
    {
    case BDMF_config:
    {
        gpon_rx_bwmap_record_config config = { .rcd_stop_on_map_end=parm[1].value.unumber, .rcd_all=parm[2].value.unumber, .rcd_all_onu=parm[3].value.unumber, .rcd_specific_alloc=parm[4].value.unumber, .secific_alloc=parm[5].value.unumber};
        err = ag_drv_gpon_rx_bwmap_record_config_set(&config);
        break;
    }
    case BDMF_enable:
        err = ag_drv_gpon_rx_bwmap_record_enable_set(parm[1].value.unumber);
        break;
    case BDMF_read_if:
        err = ag_drv_gpon_rx_bwmap_record_read_if_set(parm[1].value.unumber);
        break;
    default:
        err = BDMF_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

static int bcm_gpon_rx_bwmap_record_cli_get(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_error_t err;

    switch(parm[0].value.unumber)
    {
    case BDMF_config:
    {
        gpon_rx_bwmap_record_config config;
        err = ag_drv_gpon_rx_bwmap_record_config_get(&config);
        bdmf_session_print(session, "rcd_stop_on_map_end = %u = 0x%x\n", config.rcd_stop_on_map_end, config.rcd_stop_on_map_end);
        bdmf_session_print(session, "rcd_all = %u = 0x%x\n", config.rcd_all, config.rcd_all);
        bdmf_session_print(session, "rcd_all_onu = %u = 0x%x\n", config.rcd_all_onu, config.rcd_all_onu);
        bdmf_session_print(session, "rcd_specific_alloc = %u = 0x%x\n", config.rcd_specific_alloc, config.rcd_specific_alloc);
        bdmf_session_print(session, "secific_alloc = %u = 0x%x\n", config.secific_alloc, config.secific_alloc);
        break;
    }
    case BDMF_enable:
    {
        bdmf_boolean rcd_enable;
        err = ag_drv_gpon_rx_bwmap_record_enable_get(&rcd_enable);
        bdmf_session_print(session, "rcd_enable = %u = 0x%x\n", rcd_enable, rcd_enable);
        break;
    }
    case BDMF_status:
    {
        bdmf_boolean rcd_done;
        bdmf_boolean rcd_mismatch;
        bdmf_boolean ecd_empty;
        uint8_t rcd_last_ptr;
        err = ag_drv_gpon_rx_bwmap_record_status_get(&rcd_done, &rcd_mismatch, &ecd_empty, &rcd_last_ptr);
        bdmf_session_print(session, "rcd_done = %u = 0x%x\n", rcd_done, rcd_done);
        bdmf_session_print(session, "rcd_mismatch = %u = 0x%x\n", rcd_mismatch, rcd_mismatch);
        bdmf_session_print(session, "ecd_empty = %u = 0x%x\n", ecd_empty, ecd_empty);
        bdmf_session_print(session, "rcd_last_ptr = %u = 0x%x\n", rcd_last_ptr, rcd_last_ptr);
        break;
    }
    case BDMF_read_if:
    {
        uint8_t read_addr;
        err = ag_drv_gpon_rx_bwmap_record_read_if_get(&read_addr);
        bdmf_session_print(session, "read_addr = %u = 0x%x\n", read_addr, read_addr);
        break;
    }
    case BDMF_rdata:
    {
        uint32_t rcd_data;
        err = ag_drv_gpon_rx_bwmap_record_rdata_get(parm[1].value.unumber, &rcd_data);
        bdmf_session_print(session, "rcd_data = %u = 0x%x\n", rcd_data, rcd_data);
        break;
    }
    default:
        err = BDMF_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

static int bcm_gpon_rx_bwmap_record_cli_test(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_test_method m = parm[0].value.unumber;
    bdmf_error_t err = BDMF_ERR_OK;

    {
        gpon_rx_bwmap_record_config config = {.rcd_stop_on_map_end=gtmv(m, 1), .rcd_all=gtmv(m, 1), .rcd_all_onu=gtmv(m, 1), .rcd_specific_alloc=gtmv(m, 1), .secific_alloc=gtmv(m, 12)};
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_bwmap_record_config_set( %u %u %u %u %u)\n", config.rcd_stop_on_map_end, config.rcd_all, config.rcd_all_onu, config.rcd_specific_alloc, config.secific_alloc);
        if(!err) ag_drv_gpon_rx_bwmap_record_config_set(&config);
        if(!err) ag_drv_gpon_rx_bwmap_record_config_get( &config);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_bwmap_record_config_get( %u %u %u %u %u)\n", config.rcd_stop_on_map_end, config.rcd_all, config.rcd_all_onu, config.rcd_specific_alloc, config.secific_alloc);
        if(err || config.rcd_stop_on_map_end!=gtmv(m, 1) || config.rcd_all!=gtmv(m, 1) || config.rcd_all_onu!=gtmv(m, 1) || config.rcd_specific_alloc!=gtmv(m, 1) || config.secific_alloc!=gtmv(m, 12))
            return err ? err : BDMF_ERR_IO;
    }
    {
        bdmf_boolean rcd_enable=gtmv(m, 1);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_bwmap_record_enable_set( %u)\n", rcd_enable);
        if(!err) ag_drv_gpon_rx_bwmap_record_enable_set(rcd_enable);
        if(!err) ag_drv_gpon_rx_bwmap_record_enable_get( &rcd_enable);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_bwmap_record_enable_get( %u)\n", rcd_enable);
        if(err || rcd_enable!=gtmv(m, 1))
            return err ? err : BDMF_ERR_IO;
    }
    {
        bdmf_boolean rcd_done=gtmv(m, 1);
        bdmf_boolean rcd_mismatch=gtmv(m, 1);
        bdmf_boolean ecd_empty=gtmv(m, 1);
        uint8_t rcd_last_ptr=gtmv(m, 6);
        if(!err) ag_drv_gpon_rx_bwmap_record_status_get( &rcd_done, &rcd_mismatch, &ecd_empty, &rcd_last_ptr);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_bwmap_record_status_get( %u %u %u %u)\n", rcd_done, rcd_mismatch, ecd_empty, rcd_last_ptr);
    }
    {
        uint8_t read_addr=gtmv(m, 6);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_bwmap_record_read_if_set( %u)\n", read_addr);
        if(!err) ag_drv_gpon_rx_bwmap_record_read_if_set(read_addr);
        if(!err) ag_drv_gpon_rx_bwmap_record_read_if_get( &read_addr);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_bwmap_record_read_if_get( %u)\n", read_addr);
        if(err || read_addr!=gtmv(m, 6))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t word_id=gtmv(m, 1);
        uint32_t rcd_data=gtmv(m, 32);
        if(!err) ag_drv_gpon_rx_bwmap_record_rdata_get( word_id, &rcd_data);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_bwmap_record_rdata_get( %u %u)\n", word_id, rcd_data);
    }
    return err;
}

static int bcm_gpon_rx_bwmap_record_cli_address(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
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
    case bdmf_address_config : reg = &RU_REG(GPON_RX_BWMAP_RECORD, CONFIG); blk = &RU_BLK(GPON_RX_BWMAP_RECORD); break;
    case bdmf_address_enable : reg = &RU_REG(GPON_RX_BWMAP_RECORD, ENABLE); blk = &RU_BLK(GPON_RX_BWMAP_RECORD); break;
    case bdmf_address_status : reg = &RU_REG(GPON_RX_BWMAP_RECORD, STATUS); blk = &RU_BLK(GPON_RX_BWMAP_RECORD); break;
    case bdmf_address_read_if : reg = &RU_REG(GPON_RX_BWMAP_RECORD, READ_IF); blk = &RU_BLK(GPON_RX_BWMAP_RECORD); break;
    case bdmf_address_rdata : reg = &RU_REG(GPON_RX_BWMAP_RECORD, RDATA); blk = &RU_BLK(GPON_RX_BWMAP_RECORD); break;
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

bdmfmon_handle_t ag_drv_gpon_rx_bwmap_record_cli_init(bdmfmon_handle_t driver_dir)
{
    bdmfmon_handle_t dir;

    if ((dir = bdmfmon_dir_find(driver_dir, "gpon_rx_bwmap_record"))!=NULL)
        return dir;
    dir = bdmfmon_dir_add(driver_dir, "gpon_rx_bwmap_record", "gpon_rx_bwmap_record", BDMF_ACCESS_ADMIN, NULL);

    {
        static bdmfmon_cmd_parm_t set_config[]={
            BDMFMON_MAKE_PARM("rcd_stop_on_map_end", "rcd_stop_on_map_end", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("rcd_all", "rcd_all", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("rcd_all_onu", "rcd_all_onu", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("rcd_specific_alloc", "rcd_specific_alloc", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("secific_alloc", "secific_alloc", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_enable[]={
            BDMFMON_MAKE_PARM("rcd_enable", "rcd_enable", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_read_if[]={
            BDMFMON_MAKE_PARM("read_addr", "read_addr", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_enum_val_t selector_table[] = {
            { .name="config", .val=BDMF_config, .parms=set_config },
            { .name="enable", .val=BDMF_enable, .parms=set_enable },
            { .name="read_if", .val=BDMF_read_if, .parms=set_read_if },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "set", "set", bcm_gpon_rx_bwmap_record_cli_set,
            BDMFMON_MAKE_PARM_SELECTOR("purpose", "purpose", selector_table, 0));
    }
    {
        static bdmfmon_cmd_parm_t set_default[]={
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_rdata[]={
            BDMFMON_MAKE_PARM("word_id", "word_id", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_enum_val_t selector_table[] = {
            { .name="config", .val=BDMF_config, .parms=set_default },
            { .name="enable", .val=BDMF_enable, .parms=set_default },
            { .name="status", .val=BDMF_status, .parms=set_default },
            { .name="read_if", .val=BDMF_read_if, .parms=set_default },
            { .name="rdata", .val=BDMF_rdata, .parms=set_rdata },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "get", "get", bcm_gpon_rx_bwmap_record_cli_get,
            BDMFMON_MAKE_PARM_SELECTOR("purpose", "purpose", selector_table, 0));
    }
    {
        static bdmfmon_enum_val_t enum_table_test_method[] = {
            { .name="low" , .val=bdmf_test_method_low },
            { .name="mid" , .val=bdmf_test_method_mid },
            { .name="high" , .val=bdmf_test_method_high },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "test", "test", bcm_gpon_rx_bwmap_record_cli_test,
            BDMFMON_MAKE_PARM_ENUM("method", "low: 0000, mid: 1000, high: 1111", enum_table_test_method, 0));
    }
    {
        static bdmfmon_enum_val_t enum_table_address[] = {
            { .name="CONFIG" , .val=bdmf_address_config },
            { .name="ENABLE" , .val=bdmf_address_enable },
            { .name="STATUS" , .val=bdmf_address_status },
            { .name="READ_IF" , .val=bdmf_address_read_if },
            { .name="RDATA" , .val=bdmf_address_rdata },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "address", "address", bcm_gpon_rx_bwmap_record_cli_address,
            BDMFMON_MAKE_PARM_ENUM("method", "method", enum_table_address, 0),
            BDMFMON_MAKE_PARM("index2", "onu_id/alloc_id/port_id/etc...", BDMFMON_PARM_NUMBER, BDMFMON_PARM_FLAG_OPTIONAL));
    }
    return dir;
}
#endif /* USE_BDMF_SHELL */


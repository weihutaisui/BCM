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
#include "gpon_tx_fifo_statistics.h"
bdmf_error_t ag_drv_gpon_tx_fifo_statistics_tqcntr_get(uint32_t tx_q_idx, uint16_t *tqcntc)
{
    uint32_t reg_tqcntr=0;

#ifdef VALIDATE_PARMS
    if(!tqcntc)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
    if((tx_q_idx >= 40))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, tx_q_idx, GPON_TX_FIFO_STATISTICS, TQCNTR, reg_tqcntr);

    *tqcntc = RU_FIELD_GET(0, GPON_TX_FIFO_STATISTICS, TQCNTR, TQCNTC, reg_tqcntr);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_fifo_statistics_tqpdcntr_get(uint32_t tx_pd_idx, uint16_t *tqpdcntc)
{
    uint32_t reg_tqpdcntr=0;

#ifdef VALIDATE_PARMS
    if(!tqpdcntc)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
    if((tx_pd_idx >= 40))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, tx_pd_idx, GPON_TX_FIFO_STATISTICS, TQPDCNTR, reg_tqpdcntr);

    *tqpdcntc = RU_FIELD_GET(0, GPON_TX_FIFO_STATISTICS, TQPDCNTR, TQPDCNTC, reg_tqpdcntr);

    return BDMF_ERR_OK;
}

#ifdef USE_BDMF_SHELL
enum
{
    BDMF_tqcntr,
    BDMF_tqpdcntr,
};

typedef enum
{
    bdmf_address_tqcntr,
    bdmf_address_tqpdcntr,
}
bdmf_address;

static int bcm_gpon_tx_fifo_statistics_cli_set(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_error_t err;

    switch(parm[0].value.unumber)
    {
    default:
        err = BDMF_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

static int bcm_gpon_tx_fifo_statistics_cli_get(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_error_t err;

    switch(parm[0].value.unumber)
    {
    case BDMF_tqcntr:
    {
        uint16_t tqcntc;
        err = ag_drv_gpon_tx_fifo_statistics_tqcntr_get(parm[1].value.unumber, &tqcntc);
        bdmf_session_print(session, "tqcntc = %u = 0x%x\n", tqcntc, tqcntc);
        break;
    }
    case BDMF_tqpdcntr:
    {
        uint16_t tqpdcntc;
        err = ag_drv_gpon_tx_fifo_statistics_tqpdcntr_get(parm[1].value.unumber, &tqpdcntc);
        bdmf_session_print(session, "tqpdcntc = %u = 0x%x\n", tqpdcntc, tqpdcntc);
        break;
    }
    default:
        err = BDMF_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

static int bcm_gpon_tx_fifo_statistics_cli_test(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_test_method m = parm[0].value.unumber;
    bdmf_error_t err = BDMF_ERR_OK;

    {
        uint32_t tx_q_idx=gtmv(m, 3);
        uint16_t tqcntc=gtmv(m, 13);
        if(!err) ag_drv_gpon_tx_fifo_statistics_tqcntr_get( tx_q_idx, &tqcntc);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_fifo_statistics_tqcntr_get( %u %u)\n", tx_q_idx, tqcntc);
    }
    {
        uint32_t tx_pd_idx=gtmv(m, 3);
        uint16_t tqpdcntc=gtmv(m, 9);
        if(!err) ag_drv_gpon_tx_fifo_statistics_tqpdcntr_get( tx_pd_idx, &tqpdcntc);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_fifo_statistics_tqpdcntr_get( %u %u)\n", tx_pd_idx, tqpdcntc);
    }
    return err;
}

static int bcm_gpon_tx_fifo_statistics_cli_address(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
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
    case bdmf_address_tqcntr : reg = &RU_REG(GPON_TX_FIFO_STATISTICS, TQCNTR); blk = &RU_BLK(GPON_TX_FIFO_STATISTICS); break;
    case bdmf_address_tqpdcntr : reg = &RU_REG(GPON_TX_FIFO_STATISTICS, TQPDCNTR); blk = &RU_BLK(GPON_TX_FIFO_STATISTICS); break;
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

bdmfmon_handle_t ag_drv_gpon_tx_fifo_statistics_cli_init(bdmfmon_handle_t driver_dir)
{
    bdmfmon_handle_t dir;

    if ((dir = bdmfmon_dir_find(driver_dir, "gpon_tx_fifo_statistics"))!=NULL)
        return dir;
    dir = bdmfmon_dir_add(driver_dir, "gpon_tx_fifo_statistics", "gpon_tx_fifo_statistics", BDMF_ACCESS_ADMIN, NULL);

    {
        static bdmfmon_enum_val_t selector_table[] = {
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "set", "set", bcm_gpon_tx_fifo_statistics_cli_set,
            BDMFMON_MAKE_PARM_SELECTOR("purpose", "purpose", selector_table, 0));
    }
    {
        static bdmfmon_cmd_parm_t set_tqcntr[]={
            BDMFMON_MAKE_PARM("tx_q_idx", "tx_q_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_tqpdcntr[]={
            BDMFMON_MAKE_PARM("tx_pd_idx", "tx_pd_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_enum_val_t selector_table[] = {
            { .name="tqcntr", .val=BDMF_tqcntr, .parms=set_tqcntr },
            { .name="tqpdcntr", .val=BDMF_tqpdcntr, .parms=set_tqpdcntr },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "get", "get", bcm_gpon_tx_fifo_statistics_cli_get,
            BDMFMON_MAKE_PARM_SELECTOR("purpose", "purpose", selector_table, 0));
    }
    {
        static bdmfmon_enum_val_t enum_table_test_method[] = {
            { .name="low" , .val=bdmf_test_method_low },
            { .name="mid" , .val=bdmf_test_method_mid },
            { .name="high" , .val=bdmf_test_method_high },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "test", "test", bcm_gpon_tx_fifo_statistics_cli_test,
            BDMFMON_MAKE_PARM_ENUM("method", "low: 0000, mid: 1000, high: 1111", enum_table_test_method, 0));
    }
    {
        static bdmfmon_enum_val_t enum_table_address[] = {
            { .name="TQCNTR" , .val=bdmf_address_tqcntr },
            { .name="TQPDCNTR" , .val=bdmf_address_tqpdcntr },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "address", "address", bcm_gpon_tx_fifo_statistics_cli_address,
            BDMFMON_MAKE_PARM_ENUM("method", "method", enum_table_address, 0),
            BDMFMON_MAKE_PARM("index2", "onu_id/alloc_id/port_id/etc...", BDMFMON_PARM_NUMBER, BDMFMON_PARM_FLAG_OPTIONAL));
    }
    return dir;
}
#endif /* USE_BDMF_SHELL */


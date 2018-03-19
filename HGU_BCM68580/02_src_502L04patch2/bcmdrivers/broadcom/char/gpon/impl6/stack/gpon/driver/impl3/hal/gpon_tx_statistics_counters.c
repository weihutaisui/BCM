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
#include "gpon_tx_statistics_counters.h"
bdmf_error_t ag_drv_gpon_tx_statistics_counters_iac_get(uint16_t *iacc)
{
    uint32_t reg_iac=0;

#ifdef VALIDATE_PARMS
    if(!iacc)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_STATISTICS_COUNTERS, IAC, reg_iac);

    *iacc = RU_FIELD_GET(0, GPON_TX_STATISTICS_COUNTERS, IAC, IACC, reg_iac);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_statistics_counters_rac_get(uint32_t tx_q, uint16_t *racc)
{
    uint32_t reg_rac=0;

#ifdef VALIDATE_PARMS
    if(!racc)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
    if((tx_q >= 4))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, tx_q, GPON_TX_STATISTICS_COUNTERS, RAC, reg_rac);

    *racc = RU_FIELD_GET(0, GPON_TX_STATISTICS_COUNTERS, RAC, RACC, reg_rac);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_statistics_counters_rdbc_get(uint32_t tx_q, uint16_t *rdbcc)
{
    uint32_t reg_rdbc=0;

#ifdef VALIDATE_PARMS
    if(!rdbcc)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
    if((tx_q >= 4))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, tx_q, GPON_TX_STATISTICS_COUNTERS, RDBC, reg_rdbc);

    *rdbcc = RU_FIELD_GET(0, GPON_TX_STATISTICS_COUNTERS, RDBC, RDBCC, reg_rdbc);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_statistics_counters_tpc_get(uint32_t tx_q, uint32_t *tpcc)
{
    uint32_t reg_tpc=0;

#ifdef VALIDATE_PARMS
    if(!tpcc)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
    if((tx_q >= 4))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, tx_q, GPON_TX_STATISTICS_COUNTERS, TPC, reg_tpc);

    *tpcc = RU_FIELD_GET(0, GPON_TX_STATISTICS_COUNTERS, TPC, TPCC, reg_tpc);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_statistics_counters_tgc_get(uint32_t tx_q, uint32_t *tgcc)
{
    uint32_t reg_tgc=0;

#ifdef VALIDATE_PARMS
    if(!tgcc)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
    if((tx_q >= 4))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, tx_q, GPON_TX_STATISTICS_COUNTERS, TGC, reg_tgc);

    *tgcc = RU_FIELD_GET(0, GPON_TX_STATISTICS_COUNTERS, TGC, TGCC, reg_tgc);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_statistics_counters_tic_get(uint32_t tx_q, uint32_t *tic)
{
    uint32_t reg_tic=0;

#ifdef VALIDATE_PARMS
    if(!tic)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
    if((tx_q >= 4))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, tx_q, GPON_TX_STATISTICS_COUNTERS, TIC, reg_tic);

    *tic = RU_FIELD_GET(0, GPON_TX_STATISTICS_COUNTERS, TIC, TIC, reg_tic);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_statistics_counters_ripc_get(uint32_t *ipcc)
{
    uint32_t reg_ripc=0;

#ifdef VALIDATE_PARMS
    if(!ipcc)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_STATISTICS_COUNTERS, RIPC, reg_ripc);

    *ipcc = RU_FIELD_GET(0, GPON_TX_STATISTICS_COUNTERS, RIPC, IPCC, reg_ripc);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_statistics_counters_rnpc_get(uint32_t *rnpcc)
{
    uint32_t reg_rnpc=0;

#ifdef VALIDATE_PARMS
    if(!rnpcc)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_STATISTICS_COUNTERS, RNPC, reg_rnpc);

    *rnpcc = RU_FIELD_GET(0, GPON_TX_STATISTICS_COUNTERS, RNPC, RNPCC, reg_rnpc);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_statistics_counters_rspc_get(uint16_t *rupcc, uint16_t *rrpcc)
{
    uint32_t reg_rspc=0;

#ifdef VALIDATE_PARMS
    if(!rupcc || !rrpcc)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_STATISTICS_COUNTERS, RSPC, reg_rspc);

    *rupcc = RU_FIELD_GET(0, GPON_TX_STATISTICS_COUNTERS, RSPC, RUPCC, reg_rspc);
    *rrpcc = RU_FIELD_GET(0, GPON_TX_STATISTICS_COUNTERS, RSPC, RRPCC, reg_rspc);

    return BDMF_ERR_OK;
}

#ifdef USE_BDMF_SHELL
enum
{
    BDMF_iac,
    BDMF_rac,
    BDMF_rdbc,
    BDMF_tpc,
    BDMF_tgc,
    BDMF_tic,
    BDMF_ripc,
    BDMF_rnpc,
    BDMF_rspc,
};

typedef enum
{
    bdmf_address_iac,
    bdmf_address_rac,
    bdmf_address_rdbc,
    bdmf_address_tpc,
    bdmf_address_tgc,
    bdmf_address_tic,
    bdmf_address_ripc,
    bdmf_address_rnpc,
    bdmf_address_rspc,
}
bdmf_address;

static int bcm_gpon_tx_statistics_counters_cli_set(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
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

static int bcm_gpon_tx_statistics_counters_cli_get(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_error_t err;

    switch(parm[0].value.unumber)
    {
    case BDMF_iac:
    {
        uint16_t iacc;
        err = ag_drv_gpon_tx_statistics_counters_iac_get(&iacc);
        bdmf_session_print(session, "iacc = %u = 0x%x\n", iacc, iacc);
        break;
    }
    case BDMF_rac:
    {
        uint16_t racc;
        err = ag_drv_gpon_tx_statistics_counters_rac_get(parm[1].value.unumber, &racc);
        bdmf_session_print(session, "racc = %u = 0x%x\n", racc, racc);
        break;
    }
    case BDMF_rdbc:
    {
        uint16_t rdbcc;
        err = ag_drv_gpon_tx_statistics_counters_rdbc_get(parm[1].value.unumber, &rdbcc);
        bdmf_session_print(session, "rdbcc = %u = 0x%x\n", rdbcc, rdbcc);
        break;
    }
    case BDMF_tpc:
    {
        uint32_t tpcc;
        err = ag_drv_gpon_tx_statistics_counters_tpc_get(parm[1].value.unumber, &tpcc);
        bdmf_session_print(session, "tpcc = %u = 0x%x\n", tpcc, tpcc);
        break;
    }
    case BDMF_tgc:
    {
        uint32_t tgcc;
        err = ag_drv_gpon_tx_statistics_counters_tgc_get(parm[1].value.unumber, &tgcc);
        bdmf_session_print(session, "tgcc = %u = 0x%x\n", tgcc, tgcc);
        break;
    }
    case BDMF_tic:
    {
        uint32_t tic;
        err = ag_drv_gpon_tx_statistics_counters_tic_get(parm[1].value.unumber, &tic);
        bdmf_session_print(session, "tic = %u = 0x%x\n", tic, tic);
        break;
    }
    case BDMF_ripc:
    {
        uint32_t ipcc;
        err = ag_drv_gpon_tx_statistics_counters_ripc_get(&ipcc);
        bdmf_session_print(session, "ipcc = %u = 0x%x\n", ipcc, ipcc);
        break;
    }
    case BDMF_rnpc:
    {
        uint32_t rnpcc;
        err = ag_drv_gpon_tx_statistics_counters_rnpc_get(&rnpcc);
        bdmf_session_print(session, "rnpcc = %u = 0x%x\n", rnpcc, rnpcc);
        break;
    }
    case BDMF_rspc:
    {
        uint16_t rupcc;
        uint16_t rrpcc;
        err = ag_drv_gpon_tx_statistics_counters_rspc_get(&rupcc, &rrpcc);
        bdmf_session_print(session, "rupcc = %u = 0x%x\n", rupcc, rupcc);
        bdmf_session_print(session, "rrpcc = %u = 0x%x\n", rrpcc, rrpcc);
        break;
    }
    default:
        err = BDMF_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

static int bcm_gpon_tx_statistics_counters_cli_test(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_test_method m = parm[0].value.unumber;
    bdmf_error_t err = BDMF_ERR_OK;

    {
        uint16_t iacc=gtmv(m, 16);
        if(!err) ag_drv_gpon_tx_statistics_counters_iac_get( &iacc);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_statistics_counters_iac_get( %u)\n", iacc);
    }
    {
        uint32_t tx_q=gtmv(m, 2);
        uint16_t racc=gtmv(m, 16);
        if(!err) ag_drv_gpon_tx_statistics_counters_rac_get( tx_q, &racc);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_statistics_counters_rac_get( %u %u)\n", tx_q, racc);
    }
    {
        uint32_t tx_q=gtmv(m, 2);
        uint16_t rdbcc=gtmv(m, 16);
        if(!err) ag_drv_gpon_tx_statistics_counters_rdbc_get( tx_q, &rdbcc);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_statistics_counters_rdbc_get( %u %u)\n", tx_q, rdbcc);
    }
    {
        uint32_t tx_q=gtmv(m, 2);
        uint32_t tpcc=gtmv(m, 32);
        if(!err) ag_drv_gpon_tx_statistics_counters_tpc_get( tx_q, &tpcc);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_statistics_counters_tpc_get( %u %u)\n", tx_q, tpcc);
    }
    {
        uint32_t tx_q=gtmv(m, 2);
        uint32_t tgcc=gtmv(m, 32);
        if(!err) ag_drv_gpon_tx_statistics_counters_tgc_get( tx_q, &tgcc);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_statistics_counters_tgc_get( %u %u)\n", tx_q, tgcc);
    }
    {
        uint32_t tx_q=gtmv(m, 2);
        uint32_t tic=gtmv(m, 32);
        if(!err) ag_drv_gpon_tx_statistics_counters_tic_get( tx_q, &tic);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_statistics_counters_tic_get( %u %u)\n", tx_q, tic);
    }
    {
        uint32_t ipcc=gtmv(m, 32);
        if(!err) ag_drv_gpon_tx_statistics_counters_ripc_get( &ipcc);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_statistics_counters_ripc_get( %u)\n", ipcc);
    }
    {
        uint32_t rnpcc=gtmv(m, 32);
        if(!err) ag_drv_gpon_tx_statistics_counters_rnpc_get( &rnpcc);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_statistics_counters_rnpc_get( %u)\n", rnpcc);
    }
    {
        uint16_t rupcc=gtmv(m, 16);
        uint16_t rrpcc=gtmv(m, 16);
        if(!err) ag_drv_gpon_tx_statistics_counters_rspc_get( &rupcc, &rrpcc);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_statistics_counters_rspc_get( %u %u)\n", rupcc, rrpcc);
    }
    return err;
}

static int bcm_gpon_tx_statistics_counters_cli_address(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
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
    case bdmf_address_iac : reg = &RU_REG(GPON_TX_STATISTICS_COUNTERS, IAC); blk = &RU_BLK(GPON_TX_STATISTICS_COUNTERS); break;
    case bdmf_address_rac : reg = &RU_REG(GPON_TX_STATISTICS_COUNTERS, RAC); blk = &RU_BLK(GPON_TX_STATISTICS_COUNTERS); break;
    case bdmf_address_rdbc : reg = &RU_REG(GPON_TX_STATISTICS_COUNTERS, RDBC); blk = &RU_BLK(GPON_TX_STATISTICS_COUNTERS); break;
    case bdmf_address_tpc : reg = &RU_REG(GPON_TX_STATISTICS_COUNTERS, TPC); blk = &RU_BLK(GPON_TX_STATISTICS_COUNTERS); break;
    case bdmf_address_tgc : reg = &RU_REG(GPON_TX_STATISTICS_COUNTERS, TGC); blk = &RU_BLK(GPON_TX_STATISTICS_COUNTERS); break;
    case bdmf_address_tic : reg = &RU_REG(GPON_TX_STATISTICS_COUNTERS, TIC); blk = &RU_BLK(GPON_TX_STATISTICS_COUNTERS); break;
    case bdmf_address_ripc : reg = &RU_REG(GPON_TX_STATISTICS_COUNTERS, RIPC); blk = &RU_BLK(GPON_TX_STATISTICS_COUNTERS); break;
    case bdmf_address_rnpc : reg = &RU_REG(GPON_TX_STATISTICS_COUNTERS, RNPC); blk = &RU_BLK(GPON_TX_STATISTICS_COUNTERS); break;
    case bdmf_address_rspc : reg = &RU_REG(GPON_TX_STATISTICS_COUNTERS, RSPC); blk = &RU_BLK(GPON_TX_STATISTICS_COUNTERS); break;
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

bdmfmon_handle_t ag_drv_gpon_tx_statistics_counters_cli_init(bdmfmon_handle_t driver_dir)
{
    bdmfmon_handle_t dir;

    if ((dir = bdmfmon_dir_find(driver_dir, "gpon_tx_statistics_counters"))!=NULL)
        return dir;
    dir = bdmfmon_dir_add(driver_dir, "gpon_tx_statistics_counters", "gpon_tx_statistics_counters", BDMF_ACCESS_ADMIN, NULL);

    {
        static bdmfmon_enum_val_t selector_table[] = {
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "set", "set", bcm_gpon_tx_statistics_counters_cli_set,
            BDMFMON_MAKE_PARM_SELECTOR("purpose", "purpose", selector_table, 0));
    }
    {
        static bdmfmon_cmd_parm_t set_default[]={
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_rac[]={
            BDMFMON_MAKE_PARM("tx_q", "tx_q", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_rdbc[]={
            BDMFMON_MAKE_PARM("tx_q", "tx_q", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_tpc[]={
            BDMFMON_MAKE_PARM("tx_q", "tx_q", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_tgc[]={
            BDMFMON_MAKE_PARM("tx_q", "tx_q", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_tic[]={
            BDMFMON_MAKE_PARM("tx_q", "tx_q", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_enum_val_t selector_table[] = {
            { .name="iac", .val=BDMF_iac, .parms=set_default },
            { .name="rac", .val=BDMF_rac, .parms=set_rac },
            { .name="rdbc", .val=BDMF_rdbc, .parms=set_rdbc },
            { .name="tpc", .val=BDMF_tpc, .parms=set_tpc },
            { .name="tgc", .val=BDMF_tgc, .parms=set_tgc },
            { .name="tic", .val=BDMF_tic, .parms=set_tic },
            { .name="ripc", .val=BDMF_ripc, .parms=set_default },
            { .name="rnpc", .val=BDMF_rnpc, .parms=set_default },
            { .name="rspc", .val=BDMF_rspc, .parms=set_default },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "get", "get", bcm_gpon_tx_statistics_counters_cli_get,
            BDMFMON_MAKE_PARM_SELECTOR("purpose", "purpose", selector_table, 0));
    }
    {
        static bdmfmon_enum_val_t enum_table_test_method[] = {
            { .name="low" , .val=bdmf_test_method_low },
            { .name="mid" , .val=bdmf_test_method_mid },
            { .name="high" , .val=bdmf_test_method_high },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "test", "test", bcm_gpon_tx_statistics_counters_cli_test,
            BDMFMON_MAKE_PARM_ENUM("method", "low: 0000, mid: 1000, high: 1111", enum_table_test_method, 0));
    }
    {
        static bdmfmon_enum_val_t enum_table_address[] = {
            { .name="IAC" , .val=bdmf_address_iac },
            { .name="RAC" , .val=bdmf_address_rac },
            { .name="RDBC" , .val=bdmf_address_rdbc },
            { .name="TPC" , .val=bdmf_address_tpc },
            { .name="TGC" , .val=bdmf_address_tgc },
            { .name="TIC" , .val=bdmf_address_tic },
            { .name="RIPC" , .val=bdmf_address_ripc },
            { .name="RNPC" , .val=bdmf_address_rnpc },
            { .name="RSPC" , .val=bdmf_address_rspc },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "address", "address", bcm_gpon_tx_statistics_counters_cli_address,
            BDMFMON_MAKE_PARM_ENUM("method", "method", enum_table_address, 0),
            BDMFMON_MAKE_PARM("index2", "onu_id/alloc_id/port_id/etc...", BDMFMON_PARM_NUMBER, BDMFMON_PARM_FLAG_OPTIONAL));
    }
    return dir;
}
#endif /* USE_BDMF_SHELL */


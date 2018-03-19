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
#include "gpon_rx_general_config.h"
bdmf_error_t ag_drv_gpon_rx_general_config_rcvr_status_get(bdmf_boolean *lof, bdmf_boolean *fec_state, bdmf_boolean *lcdg_state, uint8_t *bit_align)
{
    uint32_t reg_rcvr_status=0;

#ifdef VALIDATE_PARMS
    if(!lof || !fec_state || !lcdg_state || !bit_align)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_GENERAL_CONFIG, RCVR_STATUS, reg_rcvr_status);

    *lof = RU_FIELD_GET(0, GPON_RX_GENERAL_CONFIG, RCVR_STATUS, LOF, reg_rcvr_status);
    *fec_state = RU_FIELD_GET(0, GPON_RX_GENERAL_CONFIG, RCVR_STATUS, FEC_STATE, reg_rcvr_status);
    *lcdg_state = RU_FIELD_GET(0, GPON_RX_GENERAL_CONFIG, RCVR_STATUS, LCDG_STATE, reg_rcvr_status);
    *bit_align = RU_FIELD_GET(0, GPON_RX_GENERAL_CONFIG, RCVR_STATUS, BIT_ALIGN, reg_rcvr_status);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_general_config_rcvr_config_set(const gpon_rx_general_config_rcvr_config *rcvr_config)
{
    uint32_t reg_rcvr_config=0;

#ifdef VALIDATE_PARMS
    if(!rcvr_config)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
    if((rcvr_config->des_disable >= _1BITS_MAX_VAL_) ||
       (rcvr_config->fec_disable >= _1BITS_MAX_VAL_) ||
       (rcvr_config->rx_disable >= _1BITS_MAX_VAL_) ||
       (rcvr_config->loopback_enable >= _1BITS_MAX_VAL_) ||
       (rcvr_config->fec_force >= _1BITS_MAX_VAL_) ||
       (rcvr_config->fec_st_disc >= _1BITS_MAX_VAL_) ||
       (rcvr_config->squelch_dis >= _1BITS_MAX_VAL_) ||
       (rcvr_config->sop_reset >= _1BITS_MAX_VAL_) ||
       (rcvr_config->din_polarity >= _1BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_rcvr_config = RU_FIELD_SET(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, DES_DISABLE, reg_rcvr_config, rcvr_config->des_disable);
    reg_rcvr_config = RU_FIELD_SET(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, FEC_DISABLE, reg_rcvr_config, rcvr_config->fec_disable);
    reg_rcvr_config = RU_FIELD_SET(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, RX_DISABLE, reg_rcvr_config, rcvr_config->rx_disable);
    reg_rcvr_config = RU_FIELD_SET(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, LOOPBACK_ENABLE, reg_rcvr_config, rcvr_config->loopback_enable);
    reg_rcvr_config = RU_FIELD_SET(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, FEC_FORCE, reg_rcvr_config, rcvr_config->fec_force);
    reg_rcvr_config = RU_FIELD_SET(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, FEC_ST_DISC, reg_rcvr_config, rcvr_config->fec_st_disc);
    reg_rcvr_config = RU_FIELD_SET(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, SQUELCH_DIS, reg_rcvr_config, rcvr_config->squelch_dis);
    reg_rcvr_config = RU_FIELD_SET(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, SOP_RESET, reg_rcvr_config, rcvr_config->sop_reset);
    reg_rcvr_config = RU_FIELD_SET(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, DIN_POLARITY, reg_rcvr_config, rcvr_config->din_polarity);

    RU_REG_WRITE(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, reg_rcvr_config);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_general_config_rcvr_config_get(gpon_rx_general_config_rcvr_config *rcvr_config)
{
    uint32_t reg_rcvr_config=0;

#ifdef VALIDATE_PARMS
    if(!rcvr_config)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, reg_rcvr_config);

    rcvr_config->des_disable = RU_FIELD_GET(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, DES_DISABLE, reg_rcvr_config);
    rcvr_config->fec_disable = RU_FIELD_GET(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, FEC_DISABLE, reg_rcvr_config);
    rcvr_config->rx_disable = RU_FIELD_GET(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, RX_DISABLE, reg_rcvr_config);
    rcvr_config->loopback_enable = RU_FIELD_GET(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, LOOPBACK_ENABLE, reg_rcvr_config);
    rcvr_config->fec_force = RU_FIELD_GET(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, FEC_FORCE, reg_rcvr_config);
    rcvr_config->fec_st_disc = RU_FIELD_GET(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, FEC_ST_DISC, reg_rcvr_config);
    rcvr_config->squelch_dis = RU_FIELD_GET(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, SQUELCH_DIS, reg_rcvr_config);
    rcvr_config->sop_reset = RU_FIELD_GET(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, SOP_RESET, reg_rcvr_config);
    rcvr_config->din_polarity = RU_FIELD_GET(0, GPON_RX_GENERAL_CONFIG, RCVR_CONFIG, DIN_POLARITY, reg_rcvr_config);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_general_config_lof_params_set(uint8_t delta, uint8_t alpha)
{
    uint32_t reg_lof_params=0;

#ifdef VALIDATE_PARMS
    if((delta >= _4BITS_MAX_VAL_) ||
       (alpha >= _4BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_lof_params = RU_FIELD_SET(0, GPON_RX_GENERAL_CONFIG, LOF_PARAMS, DELTA, reg_lof_params, delta);
    reg_lof_params = RU_FIELD_SET(0, GPON_RX_GENERAL_CONFIG, LOF_PARAMS, ALPHA, reg_lof_params, alpha);

    RU_REG_WRITE(0, GPON_RX_GENERAL_CONFIG, LOF_PARAMS, reg_lof_params);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_general_config_lof_params_get(uint8_t *delta, uint8_t *alpha)
{
    uint32_t reg_lof_params=0;

#ifdef VALIDATE_PARMS
    if(!delta || !alpha)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_GENERAL_CONFIG, LOF_PARAMS, reg_lof_params);

    *delta = RU_FIELD_GET(0, GPON_RX_GENERAL_CONFIG, LOF_PARAMS, DELTA, reg_lof_params);
    *alpha = RU_FIELD_GET(0, GPON_RX_GENERAL_CONFIG, LOF_PARAMS, ALPHA, reg_lof_params);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_general_config_randomsd_get(uint32_t *randomsd)
{
    uint32_t reg_randomsd=0;

#ifdef VALIDATE_PARMS
    if(!randomsd)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_GENERAL_CONFIG, RANDOMSD, reg_randomsd);

    *randomsd = RU_FIELD_GET(0, GPON_RX_GENERAL_CONFIG, RANDOMSD, RANDOMSD, reg_randomsd);

    return BDMF_ERR_OK;
}

#ifdef USE_BDMF_SHELL
enum
{
    BDMF_rcvr_status,
    BDMF_rcvr_config,
    BDMF_lof_params,
    BDMF_randomsd,
};

typedef enum
{
    bdmf_address_rcvr_status,
    bdmf_address_rcvr_config,
    bdmf_address_lof_params,
    bdmf_address_randomsd,
}
bdmf_address;

static int bcm_gpon_rx_general_config_cli_set(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_error_t err;

    switch(parm[0].value.unumber)
    {
    case BDMF_rcvr_config:
    {
        gpon_rx_general_config_rcvr_config rcvr_config = { .des_disable=parm[1].value.unumber, .fec_disable=parm[2].value.unumber, .rx_disable=parm[3].value.unumber, .loopback_enable=parm[4].value.unumber, .fec_force=parm[5].value.unumber, .fec_st_disc=parm[6].value.unumber, .squelch_dis=parm[7].value.unumber, .sop_reset=parm[8].value.unumber, .din_polarity=parm[9].value.unumber};
        err = ag_drv_gpon_rx_general_config_rcvr_config_set(&rcvr_config);
        break;
    }
    case BDMF_lof_params:
        err = ag_drv_gpon_rx_general_config_lof_params_set(parm[1].value.unumber, parm[2].value.unumber);
        break;
    default:
        err = BDMF_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

static int bcm_gpon_rx_general_config_cli_get(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_error_t err;

    switch(parm[0].value.unumber)
    {
    case BDMF_rcvr_status:
    {
        bdmf_boolean lof;
        bdmf_boolean fec_state;
        bdmf_boolean lcdg_state;
        uint8_t bit_align;
        err = ag_drv_gpon_rx_general_config_rcvr_status_get(&lof, &fec_state, &lcdg_state, &bit_align);
        bdmf_session_print(session, "lof = %u = 0x%x\n", lof, lof);
        bdmf_session_print(session, "fec_state = %u = 0x%x\n", fec_state, fec_state);
        bdmf_session_print(session, "lcdg_state = %u = 0x%x\n", lcdg_state, lcdg_state);
        bdmf_session_print(session, "bit_align = %u = 0x%x\n", bit_align, bit_align);
        break;
    }
    case BDMF_rcvr_config:
    {
        gpon_rx_general_config_rcvr_config rcvr_config;
        err = ag_drv_gpon_rx_general_config_rcvr_config_get(&rcvr_config);
        bdmf_session_print(session, "des_disable = %u = 0x%x\n", rcvr_config.des_disable, rcvr_config.des_disable);
        bdmf_session_print(session, "fec_disable = %u = 0x%x\n", rcvr_config.fec_disable, rcvr_config.fec_disable);
        bdmf_session_print(session, "rx_disable = %u = 0x%x\n", rcvr_config.rx_disable, rcvr_config.rx_disable);
        bdmf_session_print(session, "loopback_enable = %u = 0x%x\n", rcvr_config.loopback_enable, rcvr_config.loopback_enable);
        bdmf_session_print(session, "fec_force = %u = 0x%x\n", rcvr_config.fec_force, rcvr_config.fec_force);
        bdmf_session_print(session, "fec_st_disc = %u = 0x%x\n", rcvr_config.fec_st_disc, rcvr_config.fec_st_disc);
        bdmf_session_print(session, "squelch_dis = %u = 0x%x\n", rcvr_config.squelch_dis, rcvr_config.squelch_dis);
        bdmf_session_print(session, "sop_reset = %u = 0x%x\n", rcvr_config.sop_reset, rcvr_config.sop_reset);
        bdmf_session_print(session, "din_polarity = %u = 0x%x\n", rcvr_config.din_polarity, rcvr_config.din_polarity);
        break;
    }
    case BDMF_lof_params:
    {
        uint8_t delta;
        uint8_t alpha;
        err = ag_drv_gpon_rx_general_config_lof_params_get(&delta, &alpha);
        bdmf_session_print(session, "delta = %u = 0x%x\n", delta, delta);
        bdmf_session_print(session, "alpha = %u = 0x%x\n", alpha, alpha);
        break;
    }
    case BDMF_randomsd:
    {
        uint32_t randomsd;
        err = ag_drv_gpon_rx_general_config_randomsd_get(&randomsd);
        bdmf_session_print(session, "randomsd = %u = 0x%x\n", randomsd, randomsd);
        break;
    }
    default:
        err = BDMF_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

static int bcm_gpon_rx_general_config_cli_test(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_test_method m = parm[0].value.unumber;
    bdmf_error_t err = BDMF_ERR_OK;

    {
        bdmf_boolean lof=gtmv(m, 1);
        bdmf_boolean fec_state=gtmv(m, 1);
        bdmf_boolean lcdg_state=gtmv(m, 1);
        uint8_t bit_align=gtmv(m, 4);
        if(!err) ag_drv_gpon_rx_general_config_rcvr_status_get( &lof, &fec_state, &lcdg_state, &bit_align);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_general_config_rcvr_status_get( %u %u %u %u)\n", lof, fec_state, lcdg_state, bit_align);
    }
    {
        gpon_rx_general_config_rcvr_config rcvr_config = {.des_disable=gtmv(m, 1), .fec_disable=gtmv(m, 1), .rx_disable=gtmv(m, 1), .loopback_enable=gtmv(m, 1), .fec_force=gtmv(m, 1), .fec_st_disc=gtmv(m, 1), .squelch_dis=gtmv(m, 1), .sop_reset=gtmv(m, 1), .din_polarity=gtmv(m, 1)};
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_general_config_rcvr_config_set( %u %u %u %u %u %u %u %u %u)\n", rcvr_config.des_disable, rcvr_config.fec_disable, rcvr_config.rx_disable, rcvr_config.loopback_enable, rcvr_config.fec_force, rcvr_config.fec_st_disc, rcvr_config.squelch_dis, rcvr_config.sop_reset, rcvr_config.din_polarity);
        if(!err) ag_drv_gpon_rx_general_config_rcvr_config_set(&rcvr_config);
        if(!err) ag_drv_gpon_rx_general_config_rcvr_config_get( &rcvr_config);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_general_config_rcvr_config_get( %u %u %u %u %u %u %u %u %u)\n", rcvr_config.des_disable, rcvr_config.fec_disable, rcvr_config.rx_disable, rcvr_config.loopback_enable, rcvr_config.fec_force, rcvr_config.fec_st_disc, rcvr_config.squelch_dis, rcvr_config.sop_reset, rcvr_config.din_polarity);
        if(err || rcvr_config.des_disable!=gtmv(m, 1) || rcvr_config.fec_disable!=gtmv(m, 1) || rcvr_config.rx_disable!=gtmv(m, 1) || rcvr_config.loopback_enable!=gtmv(m, 1) || rcvr_config.fec_force!=gtmv(m, 1) || rcvr_config.fec_st_disc!=gtmv(m, 1) || rcvr_config.squelch_dis!=gtmv(m, 1) || rcvr_config.sop_reset!=gtmv(m, 1) || rcvr_config.din_polarity!=gtmv(m, 1))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint8_t delta=gtmv(m, 4);
        uint8_t alpha=gtmv(m, 4);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_general_config_lof_params_set( %u %u)\n", delta, alpha);
        if(!err) ag_drv_gpon_rx_general_config_lof_params_set(delta, alpha);
        if(!err) ag_drv_gpon_rx_general_config_lof_params_get( &delta, &alpha);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_general_config_lof_params_get( %u %u)\n", delta, alpha);
        if(err || delta!=gtmv(m, 4) || alpha!=gtmv(m, 4))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t randomsd=gtmv(m, 32);
        if(!err) ag_drv_gpon_rx_general_config_randomsd_get( &randomsd);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_general_config_randomsd_get( %u)\n", randomsd);
    }
    return err;
}

static int bcm_gpon_rx_general_config_cli_address(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
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
    case bdmf_address_rcvr_status : reg = &RU_REG(GPON_RX_GENERAL_CONFIG, RCVR_STATUS); blk = &RU_BLK(GPON_RX_GENERAL_CONFIG); break;
    case bdmf_address_rcvr_config : reg = &RU_REG(GPON_RX_GENERAL_CONFIG, RCVR_CONFIG); blk = &RU_BLK(GPON_RX_GENERAL_CONFIG); break;
    case bdmf_address_lof_params : reg = &RU_REG(GPON_RX_GENERAL_CONFIG, LOF_PARAMS); blk = &RU_BLK(GPON_RX_GENERAL_CONFIG); break;
    case bdmf_address_randomsd : reg = &RU_REG(GPON_RX_GENERAL_CONFIG, RANDOMSD); blk = &RU_BLK(GPON_RX_GENERAL_CONFIG); break;
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

bdmfmon_handle_t ag_drv_gpon_rx_general_config_cli_init(bdmfmon_handle_t driver_dir)
{
    bdmfmon_handle_t dir;

    if ((dir = bdmfmon_dir_find(driver_dir, "gpon_rx_general_config"))!=NULL)
        return dir;
    dir = bdmfmon_dir_add(driver_dir, "gpon_rx_general_config", "gpon_rx_general_config", BDMF_ACCESS_ADMIN, NULL);

    {
        static bdmfmon_cmd_parm_t set_rcvr_config[]={
            BDMFMON_MAKE_PARM("des_disable", "des_disable", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("fec_disable", "fec_disable", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("rx_disable", "rx_disable", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("loopback_enable", "loopback_enable", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("fec_force", "fec_force", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("fec_st_disc", "fec_st_disc", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("squelch_dis", "squelch_dis", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("sop_reset", "sop_reset", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("din_polarity", "din_polarity", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_lof_params[]={
            BDMFMON_MAKE_PARM("delta", "delta", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("alpha", "alpha", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_enum_val_t selector_table[] = {
            { .name="rcvr_config", .val=BDMF_rcvr_config, .parms=set_rcvr_config },
            { .name="lof_params", .val=BDMF_lof_params, .parms=set_lof_params },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "set", "set", bcm_gpon_rx_general_config_cli_set,
            BDMFMON_MAKE_PARM_SELECTOR("purpose", "purpose", selector_table, 0));
    }
    {
        static bdmfmon_cmd_parm_t set_default[]={
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_enum_val_t selector_table[] = {
            { .name="rcvr_status", .val=BDMF_rcvr_status, .parms=set_default },
            { .name="rcvr_config", .val=BDMF_rcvr_config, .parms=set_default },
            { .name="lof_params", .val=BDMF_lof_params, .parms=set_default },
            { .name="randomsd", .val=BDMF_randomsd, .parms=set_default },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "get", "get", bcm_gpon_rx_general_config_cli_get,
            BDMFMON_MAKE_PARM_SELECTOR("purpose", "purpose", selector_table, 0));
    }
    {
        static bdmfmon_enum_val_t enum_table_test_method[] = {
            { .name="low" , .val=bdmf_test_method_low },
            { .name="mid" , .val=bdmf_test_method_mid },
            { .name="high" , .val=bdmf_test_method_high },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "test", "test", bcm_gpon_rx_general_config_cli_test,
            BDMFMON_MAKE_PARM_ENUM("method", "low: 0000, mid: 1000, high: 1111", enum_table_test_method, 0));
    }
    {
        static bdmfmon_enum_val_t enum_table_address[] = {
            { .name="RCVR_STATUS" , .val=bdmf_address_rcvr_status },
            { .name="RCVR_CONFIG" , .val=bdmf_address_rcvr_config },
            { .name="LOF_PARAMS" , .val=bdmf_address_lof_params },
            { .name="RANDOMSD" , .val=bdmf_address_randomsd },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "address", "address", bcm_gpon_rx_general_config_cli_address,
            BDMFMON_MAKE_PARM_ENUM("method", "method", enum_table_address, 0),
            BDMFMON_MAKE_PARM("index2", "onu_id/alloc_id/port_id/etc...", BDMFMON_PARM_NUMBER, BDMFMON_PARM_FLAG_OPTIONAL));
    }
    return dir;
}
#endif /* USE_BDMF_SHELL */


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
#include "gpon_rx_amd2.h"
bdmf_error_t ag_drv_gpon_rx_amd2_tod_sf_up_set(uint32_t up_sf)
{
    uint32_t reg_tod_sf_up=0;

#ifdef VALIDATE_PARMS
    if((up_sf >= _30BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_tod_sf_up = RU_FIELD_SET(0, GPON_RX_AMD2, TOD_SF_UP, UP_SF, reg_tod_sf_up, up_sf);

    RU_REG_WRITE(0, GPON_RX_AMD2, TOD_SF_UP, reg_tod_sf_up);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_amd2_tod_sf_up_get(uint32_t *up_sf)
{
    uint32_t reg_tod_sf_up=0;

#ifdef VALIDATE_PARMS
    if(!up_sf)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_AMD2, TOD_SF_UP, reg_tod_sf_up);

    *up_sf = RU_FIELD_GET(0, GPON_RX_AMD2, TOD_SF_UP, UP_SF, reg_tod_sf_up);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_amd2_tod_up_val_set(uint32_t word_id, uint32_t tod_val)
{
    uint32_t reg_tod_up_val=0;

#ifdef VALIDATE_PARMS
    if((word_id >= 2))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_tod_up_val = RU_FIELD_SET(0, GPON_RX_AMD2, TOD_UP_VAL, TOD_VAL, reg_tod_up_val, tod_val);

    RU_REG_RAM_WRITE(0, word_id, GPON_RX_AMD2, TOD_UP_VAL, reg_tod_up_val);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_amd2_tod_up_val_get(uint32_t word_id, uint32_t *tod_val)
{
    uint32_t reg_tod_up_val=0;

#ifdef VALIDATE_PARMS
    if(!tod_val)
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

    RU_REG_RAM_READ(0, word_id, GPON_RX_AMD2, TOD_UP_VAL, reg_tod_up_val);

    *tod_val = RU_FIELD_GET(0, GPON_RX_AMD2, TOD_UP_VAL, TOD_VAL, reg_tod_up_val);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_amd2_up_arm_set(bdmf_boolean uparm, bdmf_boolean en, bdmf_boolean rdarm)
{
    uint32_t reg_up_arm=0;

#ifdef VALIDATE_PARMS
    if((uparm >= _1BITS_MAX_VAL_) ||
       (en >= _1BITS_MAX_VAL_) ||
       (rdarm >= _1BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_up_arm = RU_FIELD_SET(0, GPON_RX_AMD2, UP_ARM, UPARM, reg_up_arm, uparm);
    reg_up_arm = RU_FIELD_SET(0, GPON_RX_AMD2, UP_ARM, EN, reg_up_arm, en);
    reg_up_arm = RU_FIELD_SET(0, GPON_RX_AMD2, UP_ARM, RDARM, reg_up_arm, rdarm);

    RU_REG_WRITE(0, GPON_RX_AMD2, UP_ARM, reg_up_arm);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_amd2_up_arm_get(bdmf_boolean *uparm, bdmf_boolean *en, bdmf_boolean *rdarm)
{
    uint32_t reg_up_arm=0;

#ifdef VALIDATE_PARMS
    if(!uparm || !en || !rdarm)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_AMD2, UP_ARM, reg_up_arm);

    *uparm = RU_FIELD_GET(0, GPON_RX_AMD2, UP_ARM, UPARM, reg_up_arm);
    *en = RU_FIELD_GET(0, GPON_RX_AMD2, UP_ARM, EN, reg_up_arm);
    *rdarm = RU_FIELD_GET(0, GPON_RX_AMD2, UP_ARM, RDARM, reg_up_arm);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_amd2_tod_cyc_in_sec_set(uint32_t cyc_in_sec)
{
    uint32_t reg_tod_cyc_in_sec=0;

#ifdef VALIDATE_PARMS
    if((cyc_in_sec >= _30BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_tod_cyc_in_sec = RU_FIELD_SET(0, GPON_RX_AMD2, TOD_CYC_IN_SEC, CYC_IN_SEC, reg_tod_cyc_in_sec, cyc_in_sec);

    RU_REG_WRITE(0, GPON_RX_AMD2, TOD_CYC_IN_SEC, reg_tod_cyc_in_sec);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_amd2_tod_cyc_in_sec_get(uint32_t *cyc_in_sec)
{
    uint32_t reg_tod_cyc_in_sec=0;

#ifdef VALIDATE_PARMS
    if(!cyc_in_sec)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_AMD2, TOD_CYC_IN_SEC, reg_tod_cyc_in_sec);

    *cyc_in_sec = RU_FIELD_GET(0, GPON_RX_AMD2, TOD_CYC_IN_SEC, CYC_IN_SEC, reg_tod_cyc_in_sec);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_amd2_tod_opps_duty_set(uint32_t duty_cycle, bdmf_boolean opps_en)
{
    uint32_t reg_tod_opps_duty=0;

#ifdef VALIDATE_PARMS
    if((duty_cycle >= _30BITS_MAX_VAL_) ||
       (opps_en >= _1BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_tod_opps_duty = RU_FIELD_SET(0, GPON_RX_AMD2, TOD_OPPS_DUTY, DUTY_CYCLE, reg_tod_opps_duty, duty_cycle);
    reg_tod_opps_duty = RU_FIELD_SET(0, GPON_RX_AMD2, TOD_OPPS_DUTY, OPPS_EN, reg_tod_opps_duty, opps_en);

    RU_REG_WRITE(0, GPON_RX_AMD2, TOD_OPPS_DUTY, reg_tod_opps_duty);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_amd2_tod_opps_duty_get(uint32_t *duty_cycle, bdmf_boolean *opps_en)
{
    uint32_t reg_tod_opps_duty=0;

#ifdef VALIDATE_PARMS
    if(!duty_cycle || !opps_en)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_AMD2, TOD_OPPS_DUTY, reg_tod_opps_duty);

    *duty_cycle = RU_FIELD_GET(0, GPON_RX_AMD2, TOD_OPPS_DUTY, DUTY_CYCLE, reg_tod_opps_duty);
    *opps_en = RU_FIELD_GET(0, GPON_RX_AMD2, TOD_OPPS_DUTY, OPPS_EN, reg_tod_opps_duty);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_amd2_nanos_per_clk_set(uint32_t ns_in_clk)
{
    uint32_t reg_nanos_per_clk=0;

#ifdef VALIDATE_PARMS
    if((ns_in_clk >= _30BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_nanos_per_clk = RU_FIELD_SET(0, GPON_RX_AMD2, NANOS_PER_CLK, NS_IN_CLK, reg_nanos_per_clk, ns_in_clk);

    RU_REG_WRITE(0, GPON_RX_AMD2, NANOS_PER_CLK, reg_nanos_per_clk);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_amd2_nanos_per_clk_get(uint32_t *ns_in_clk)
{
    uint32_t reg_nanos_per_clk=0;

#ifdef VALIDATE_PARMS
    if(!ns_in_clk)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_AMD2, NANOS_PER_CLK, reg_nanos_per_clk);

    *ns_in_clk = RU_FIELD_GET(0, GPON_RX_AMD2, NANOS_PER_CLK, NS_IN_CLK, reg_nanos_per_clk);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_amd2_nspartinc_set(uint32_t nspinc)
{
    uint32_t reg_nspartinc=0;

#ifdef VALIDATE_PARMS
    if((nspinc >= _30BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_nspartinc = RU_FIELD_SET(0, GPON_RX_AMD2, NSPARTINC, NSPINC, reg_nspartinc, nspinc);

    RU_REG_WRITE(0, GPON_RX_AMD2, NSPARTINC, reg_nspartinc);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_amd2_nspartinc_get(uint32_t *nspinc)
{
    uint32_t reg_nspartinc=0;

#ifdef VALIDATE_PARMS
    if(!nspinc)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_AMD2, NSPARTINC, reg_nspartinc);

    *nspinc = RU_FIELD_GET(0, GPON_RX_AMD2, NSPARTINC, NSPINC, reg_nspartinc);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_amd2_nsp_in_nano_set(uint32_t nsp_in_nano)
{
    uint32_t reg_nsp_in_nano=0;

#ifdef VALIDATE_PARMS
    if((nsp_in_nano >= _30BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_nsp_in_nano = RU_FIELD_SET(0, GPON_RX_AMD2, NSP_IN_NANO, NSP_IN_NANO, reg_nsp_in_nano, nsp_in_nano);

    RU_REG_WRITE(0, GPON_RX_AMD2, NSP_IN_NANO, reg_nsp_in_nano);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_amd2_nsp_in_nano_get(uint32_t *nsp_in_nano)
{
    uint32_t reg_nsp_in_nano=0;

#ifdef VALIDATE_PARMS
    if(!nsp_in_nano)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_AMD2, NSP_IN_NANO, reg_nsp_in_nano);

    *nsp_in_nano = RU_FIELD_GET(0, GPON_RX_AMD2, NSP_IN_NANO, NSP_IN_NANO, reg_nsp_in_nano);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_amd2_ts_up_val_set(uint32_t word_id, uint32_t ts_val)
{
    uint32_t reg_ts_up_val=0;

#ifdef VALIDATE_PARMS
    if((word_id >= 2))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_ts_up_val = RU_FIELD_SET(0, GPON_RX_AMD2, TS_UP_VAL, TS_VAL, reg_ts_up_val, ts_val);

    RU_REG_RAM_WRITE(0, word_id, GPON_RX_AMD2, TS_UP_VAL, reg_ts_up_val);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_amd2_ts_up_val_get(uint32_t word_id, uint32_t *ts_val)
{
    uint32_t reg_ts_up_val=0;

#ifdef VALIDATE_PARMS
    if(!ts_val)
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

    RU_REG_RAM_READ(0, word_id, GPON_RX_AMD2, TS_UP_VAL, reg_ts_up_val);

    *ts_val = RU_FIELD_GET(0, GPON_RX_AMD2, TS_UP_VAL, TS_VAL, reg_ts_up_val);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_amd2_tod_rd_val_get(uint32_t word_id, uint32_t *ts_val)
{
    uint32_t reg_tod_rd_val=0;

#ifdef VALIDATE_PARMS
    if(!ts_val)
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

    RU_REG_RAM_READ(0, word_id, GPON_RX_AMD2, TOD_RD_VAL, reg_tod_rd_val);

    *ts_val = RU_FIELD_GET(0, GPON_RX_AMD2, TOD_RD_VAL, TS_VAL, reg_tod_rd_val);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_amd2_ts_rd_val_get(uint32_t word_id, uint32_t *ts_val)
{
    uint32_t reg_ts_rd_val=0;

#ifdef VALIDATE_PARMS
    if(!ts_val)
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

    RU_REG_RAM_READ(0, word_id, GPON_RX_AMD2, TS_RD_VAL, reg_ts_rd_val);

    *ts_val = RU_FIELD_GET(0, GPON_RX_AMD2, TS_RD_VAL, TS_VAL, reg_ts_rd_val);

    return BDMF_ERR_OK;
}

#ifdef USE_BDMF_SHELL
enum
{
    BDMF_tod_sf_up,
    BDMF_tod_up_val,
    BDMF_up_arm,
    BDMF_tod_cyc_in_sec,
    BDMF_tod_opps_duty,
    BDMF_nanos_per_clk,
    BDMF_nspartinc,
    BDMF_nsp_in_nano,
    BDMF_ts_up_val,
    BDMF_tod_rd_val,
    BDMF_ts_rd_val,
};

typedef enum
{
    bdmf_address_tod_sf_up,
    bdmf_address_tod_up_val,
    bdmf_address_up_arm,
    bdmf_address_tod_cyc_in_sec,
    bdmf_address_tod_opps_duty,
    bdmf_address_nanos_per_clk,
    bdmf_address_nspartinc,
    bdmf_address_nsp_in_nano,
    bdmf_address_ts_up_val,
    bdmf_address_tod_rd_val,
    bdmf_address_ts_rd_val,
}
bdmf_address;

static int bcm_gpon_rx_amd2_cli_set(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_error_t err;

    switch(parm[0].value.unumber)
    {
    case BDMF_tod_sf_up:
        err = ag_drv_gpon_rx_amd2_tod_sf_up_set(parm[1].value.unumber);
        break;
    case BDMF_tod_up_val:
        err = ag_drv_gpon_rx_amd2_tod_up_val_set(parm[1].value.unumber, parm[2].value.unumber);
        break;
    case BDMF_up_arm:
        err = ag_drv_gpon_rx_amd2_up_arm_set(parm[1].value.unumber, parm[2].value.unumber, parm[3].value.unumber);
        break;
    case BDMF_tod_cyc_in_sec:
        err = ag_drv_gpon_rx_amd2_tod_cyc_in_sec_set(parm[1].value.unumber);
        break;
    case BDMF_tod_opps_duty:
        err = ag_drv_gpon_rx_amd2_tod_opps_duty_set(parm[1].value.unumber, parm[2].value.unumber);
        break;
    case BDMF_nanos_per_clk:
        err = ag_drv_gpon_rx_amd2_nanos_per_clk_set(parm[1].value.unumber);
        break;
    case BDMF_nspartinc:
        err = ag_drv_gpon_rx_amd2_nspartinc_set(parm[1].value.unumber);
        break;
    case BDMF_nsp_in_nano:
        err = ag_drv_gpon_rx_amd2_nsp_in_nano_set(parm[1].value.unumber);
        break;
    case BDMF_ts_up_val:
        err = ag_drv_gpon_rx_amd2_ts_up_val_set(parm[1].value.unumber, parm[2].value.unumber);
        break;
    default:
        err = BDMF_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

static int bcm_gpon_rx_amd2_cli_get(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_error_t err;

    switch(parm[0].value.unumber)
    {
    case BDMF_tod_sf_up:
    {
        uint32_t up_sf;
        err = ag_drv_gpon_rx_amd2_tod_sf_up_get(&up_sf);
        bdmf_session_print(session, "up_sf = %u = 0x%x\n", up_sf, up_sf);
        break;
    }
    case BDMF_tod_up_val:
    {
        uint32_t tod_val;
        err = ag_drv_gpon_rx_amd2_tod_up_val_get(parm[1].value.unumber, &tod_val);
        bdmf_session_print(session, "tod_val = %u = 0x%x\n", tod_val, tod_val);
        break;
    }
    case BDMF_up_arm:
    {
        bdmf_boolean uparm;
        bdmf_boolean en;
        bdmf_boolean rdarm;
        err = ag_drv_gpon_rx_amd2_up_arm_get(&uparm, &en, &rdarm);
        bdmf_session_print(session, "uparm = %u = 0x%x\n", uparm, uparm);
        bdmf_session_print(session, "en = %u = 0x%x\n", en, en);
        bdmf_session_print(session, "rdarm = %u = 0x%x\n", rdarm, rdarm);
        break;
    }
    case BDMF_tod_cyc_in_sec:
    {
        uint32_t cyc_in_sec;
        err = ag_drv_gpon_rx_amd2_tod_cyc_in_sec_get(&cyc_in_sec);
        bdmf_session_print(session, "cyc_in_sec = %u = 0x%x\n", cyc_in_sec, cyc_in_sec);
        break;
    }
    case BDMF_tod_opps_duty:
    {
        uint32_t duty_cycle;
        bdmf_boolean opps_en;
        err = ag_drv_gpon_rx_amd2_tod_opps_duty_get(&duty_cycle, &opps_en);
        bdmf_session_print(session, "duty_cycle = %u = 0x%x\n", duty_cycle, duty_cycle);
        bdmf_session_print(session, "opps_en = %u = 0x%x\n", opps_en, opps_en);
        break;
    }
    case BDMF_nanos_per_clk:
    {
        uint32_t ns_in_clk;
        err = ag_drv_gpon_rx_amd2_nanos_per_clk_get(&ns_in_clk);
        bdmf_session_print(session, "ns_in_clk = %u = 0x%x\n", ns_in_clk, ns_in_clk);
        break;
    }
    case BDMF_nspartinc:
    {
        uint32_t nspinc;
        err = ag_drv_gpon_rx_amd2_nspartinc_get(&nspinc);
        bdmf_session_print(session, "nspinc = %u = 0x%x\n", nspinc, nspinc);
        break;
    }
    case BDMF_nsp_in_nano:
    {
        uint32_t nsp_in_nano;
        err = ag_drv_gpon_rx_amd2_nsp_in_nano_get(&nsp_in_nano);
        bdmf_session_print(session, "nsp_in_nano = %u = 0x%x\n", nsp_in_nano, nsp_in_nano);
        break;
    }
    case BDMF_ts_up_val:
    {
        uint32_t ts_val;
        err = ag_drv_gpon_rx_amd2_ts_up_val_get(parm[1].value.unumber, &ts_val);
        bdmf_session_print(session, "ts_val = %u = 0x%x\n", ts_val, ts_val);
        break;
    }
    case BDMF_tod_rd_val:
    {
        uint32_t ts_val;
        err = ag_drv_gpon_rx_amd2_tod_rd_val_get(parm[1].value.unumber, &ts_val);
        bdmf_session_print(session, "ts_val = %u = 0x%x\n", ts_val, ts_val);
        break;
    }
    case BDMF_ts_rd_val:
    {
        uint32_t ts_val;
        err = ag_drv_gpon_rx_amd2_ts_rd_val_get(parm[1].value.unumber, &ts_val);
        bdmf_session_print(session, "ts_val = %u = 0x%x\n", ts_val, ts_val);
        break;
    }
    default:
        err = BDMF_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

static int bcm_gpon_rx_amd2_cli_test(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_test_method m = parm[0].value.unumber;
    bdmf_error_t err = BDMF_ERR_OK;

    {
        uint32_t up_sf=gtmv(m, 30);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_amd2_tod_sf_up_set( %u)\n", up_sf);
        if(!err) ag_drv_gpon_rx_amd2_tod_sf_up_set(up_sf);
        if(!err) ag_drv_gpon_rx_amd2_tod_sf_up_get( &up_sf);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_amd2_tod_sf_up_get( %u)\n", up_sf);
        if(err || up_sf!=gtmv(m, 30))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t word_id=gtmv(m, 1);
        uint32_t tod_val=gtmv(m, 32);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_amd2_tod_up_val_set( %u %u)\n", word_id, tod_val);
        if(!err) ag_drv_gpon_rx_amd2_tod_up_val_set(word_id, tod_val);
        if(!err) ag_drv_gpon_rx_amd2_tod_up_val_get( word_id, &tod_val);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_amd2_tod_up_val_get( %u %u)\n", word_id, tod_val);
        if(err || tod_val!=gtmv(m, 32))
            return err ? err : BDMF_ERR_IO;
    }
    {
        bdmf_boolean uparm=gtmv(m, 1);
        bdmf_boolean en=gtmv(m, 1);
        bdmf_boolean rdarm=gtmv(m, 1);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_amd2_up_arm_set( %u %u %u)\n", uparm, en, rdarm);
        if(!err) ag_drv_gpon_rx_amd2_up_arm_set(uparm, en, rdarm);
        if(!err) ag_drv_gpon_rx_amd2_up_arm_get( &uparm, &en, &rdarm);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_amd2_up_arm_get( %u %u %u)\n", uparm, en, rdarm);
        if(err || uparm!=gtmv(m, 1) || en!=gtmv(m, 1) || rdarm!=gtmv(m, 1))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t cyc_in_sec=gtmv(m, 30);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_amd2_tod_cyc_in_sec_set( %u)\n", cyc_in_sec);
        if(!err) ag_drv_gpon_rx_amd2_tod_cyc_in_sec_set(cyc_in_sec);
        if(!err) ag_drv_gpon_rx_amd2_tod_cyc_in_sec_get( &cyc_in_sec);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_amd2_tod_cyc_in_sec_get( %u)\n", cyc_in_sec);
        if(err || cyc_in_sec!=gtmv(m, 30))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t duty_cycle=gtmv(m, 30);
        bdmf_boolean opps_en=gtmv(m, 1);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_amd2_tod_opps_duty_set( %u %u)\n", duty_cycle, opps_en);
        if(!err) ag_drv_gpon_rx_amd2_tod_opps_duty_set(duty_cycle, opps_en);
        if(!err) ag_drv_gpon_rx_amd2_tod_opps_duty_get( &duty_cycle, &opps_en);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_amd2_tod_opps_duty_get( %u %u)\n", duty_cycle, opps_en);
        if(err || duty_cycle!=gtmv(m, 30) || opps_en!=gtmv(m, 1))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t ns_in_clk=gtmv(m, 30);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_amd2_nanos_per_clk_set( %u)\n", ns_in_clk);
        if(!err) ag_drv_gpon_rx_amd2_nanos_per_clk_set(ns_in_clk);
        if(!err) ag_drv_gpon_rx_amd2_nanos_per_clk_get( &ns_in_clk);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_amd2_nanos_per_clk_get( %u)\n", ns_in_clk);
        if(err || ns_in_clk!=gtmv(m, 30))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t nspinc=gtmv(m, 30);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_amd2_nspartinc_set( %u)\n", nspinc);
        if(!err) ag_drv_gpon_rx_amd2_nspartinc_set(nspinc);
        if(!err) ag_drv_gpon_rx_amd2_nspartinc_get( &nspinc);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_amd2_nspartinc_get( %u)\n", nspinc);
        if(err || nspinc!=gtmv(m, 30))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t nsp_in_nano=gtmv(m, 30);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_amd2_nsp_in_nano_set( %u)\n", nsp_in_nano);
        if(!err) ag_drv_gpon_rx_amd2_nsp_in_nano_set(nsp_in_nano);
        if(!err) ag_drv_gpon_rx_amd2_nsp_in_nano_get( &nsp_in_nano);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_amd2_nsp_in_nano_get( %u)\n", nsp_in_nano);
        if(err || nsp_in_nano!=gtmv(m, 30))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t word_id=gtmv(m, 1);
        uint32_t ts_val=gtmv(m, 32);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_amd2_ts_up_val_set( %u %u)\n", word_id, ts_val);
        if(!err) ag_drv_gpon_rx_amd2_ts_up_val_set(word_id, ts_val);
        if(!err) ag_drv_gpon_rx_amd2_ts_up_val_get( word_id, &ts_val);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_amd2_ts_up_val_get( %u %u)\n", word_id, ts_val);
        if(err || ts_val!=gtmv(m, 32))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t word_id=gtmv(m, 1);
        uint32_t ts_val=gtmv(m, 32);
        if(!err) ag_drv_gpon_rx_amd2_tod_rd_val_get( word_id, &ts_val);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_amd2_tod_rd_val_get( %u %u)\n", word_id, ts_val);
    }
    {
        uint32_t word_id=gtmv(m, 1);
        uint32_t ts_val=gtmv(m, 32);
        if(!err) ag_drv_gpon_rx_amd2_ts_rd_val_get( word_id, &ts_val);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_amd2_ts_rd_val_get( %u %u)\n", word_id, ts_val);
    }
    return err;
}

static int bcm_gpon_rx_amd2_cli_address(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
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
    case bdmf_address_tod_sf_up : reg = &RU_REG(GPON_RX_AMD2, TOD_SF_UP); blk = &RU_BLK(GPON_RX_AMD2); break;
    case bdmf_address_tod_up_val : reg = &RU_REG(GPON_RX_AMD2, TOD_UP_VAL); blk = &RU_BLK(GPON_RX_AMD2); break;
    case bdmf_address_up_arm : reg = &RU_REG(GPON_RX_AMD2, UP_ARM); blk = &RU_BLK(GPON_RX_AMD2); break;
    case bdmf_address_tod_cyc_in_sec : reg = &RU_REG(GPON_RX_AMD2, TOD_CYC_IN_SEC); blk = &RU_BLK(GPON_RX_AMD2); break;
    case bdmf_address_tod_opps_duty : reg = &RU_REG(GPON_RX_AMD2, TOD_OPPS_DUTY); blk = &RU_BLK(GPON_RX_AMD2); break;
    case bdmf_address_nanos_per_clk : reg = &RU_REG(GPON_RX_AMD2, NANOS_PER_CLK); blk = &RU_BLK(GPON_RX_AMD2); break;
    case bdmf_address_nspartinc : reg = &RU_REG(GPON_RX_AMD2, NSPARTINC); blk = &RU_BLK(GPON_RX_AMD2); break;
    case bdmf_address_nsp_in_nano : reg = &RU_REG(GPON_RX_AMD2, NSP_IN_NANO); blk = &RU_BLK(GPON_RX_AMD2); break;
    case bdmf_address_ts_up_val : reg = &RU_REG(GPON_RX_AMD2, TS_UP_VAL); blk = &RU_BLK(GPON_RX_AMD2); break;
    case bdmf_address_tod_rd_val : reg = &RU_REG(GPON_RX_AMD2, TOD_RD_VAL); blk = &RU_BLK(GPON_RX_AMD2); break;
    case bdmf_address_ts_rd_val : reg = &RU_REG(GPON_RX_AMD2, TS_RD_VAL); blk = &RU_BLK(GPON_RX_AMD2); break;
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

bdmfmon_handle_t ag_drv_gpon_rx_amd2_cli_init(bdmfmon_handle_t driver_dir)
{
    bdmfmon_handle_t dir;

    if ((dir = bdmfmon_dir_find(driver_dir, "gpon_rx_amd2"))!=NULL)
        return dir;
    dir = bdmfmon_dir_add(driver_dir, "gpon_rx_amd2", "gpon_rx_amd2", BDMF_ACCESS_ADMIN, NULL);

    {
        static bdmfmon_cmd_parm_t set_tod_sf_up[]={
            BDMFMON_MAKE_PARM("up_sf", "up_sf", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_tod_up_val[]={
            BDMFMON_MAKE_PARM("word_id", "word_id", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("tod_val", "tod_val", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_up_arm[]={
            BDMFMON_MAKE_PARM("uparm", "uparm", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("en", "en", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("rdarm", "rdarm", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_tod_cyc_in_sec[]={
            BDMFMON_MAKE_PARM("cyc_in_sec", "cyc_in_sec", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_tod_opps_duty[]={
            BDMFMON_MAKE_PARM("duty_cycle", "duty_cycle", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("opps_en", "opps_en", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_nanos_per_clk[]={
            BDMFMON_MAKE_PARM("ns_in_clk", "ns_in_clk", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_nspartinc[]={
            BDMFMON_MAKE_PARM("nspinc", "nspinc", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_nsp_in_nano[]={
            BDMFMON_MAKE_PARM("nsp_in_nano", "nsp_in_nano", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_ts_up_val[]={
            BDMFMON_MAKE_PARM("word_id", "word_id", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("ts_val", "ts_val", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_enum_val_t selector_table[] = {
            { .name="tod_sf_up", .val=BDMF_tod_sf_up, .parms=set_tod_sf_up },
            { .name="tod_up_val", .val=BDMF_tod_up_val, .parms=set_tod_up_val },
            { .name="up_arm", .val=BDMF_up_arm, .parms=set_up_arm },
            { .name="tod_cyc_in_sec", .val=BDMF_tod_cyc_in_sec, .parms=set_tod_cyc_in_sec },
            { .name="tod_opps_duty", .val=BDMF_tod_opps_duty, .parms=set_tod_opps_duty },
            { .name="nanos_per_clk", .val=BDMF_nanos_per_clk, .parms=set_nanos_per_clk },
            { .name="nspartinc", .val=BDMF_nspartinc, .parms=set_nspartinc },
            { .name="nsp_in_nano", .val=BDMF_nsp_in_nano, .parms=set_nsp_in_nano },
            { .name="ts_up_val", .val=BDMF_ts_up_val, .parms=set_ts_up_val },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "set", "set", bcm_gpon_rx_amd2_cli_set,
            BDMFMON_MAKE_PARM_SELECTOR("purpose", "purpose", selector_table, 0));
    }
    {
        static bdmfmon_cmd_parm_t set_default[]={
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_tod_up_val[]={
            BDMFMON_MAKE_PARM("word_id", "word_id", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_ts_up_val[]={
            BDMFMON_MAKE_PARM("word_id", "word_id", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_tod_rd_val[]={
            BDMFMON_MAKE_PARM("word_id", "word_id", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_ts_rd_val[]={
            BDMFMON_MAKE_PARM("word_id", "word_id", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_enum_val_t selector_table[] = {
            { .name="tod_sf_up", .val=BDMF_tod_sf_up, .parms=set_default },
            { .name="tod_up_val", .val=BDMF_tod_up_val, .parms=set_tod_up_val },
            { .name="up_arm", .val=BDMF_up_arm, .parms=set_default },
            { .name="tod_cyc_in_sec", .val=BDMF_tod_cyc_in_sec, .parms=set_default },
            { .name="tod_opps_duty", .val=BDMF_tod_opps_duty, .parms=set_default },
            { .name="nanos_per_clk", .val=BDMF_nanos_per_clk, .parms=set_default },
            { .name="nspartinc", .val=BDMF_nspartinc, .parms=set_default },
            { .name="nsp_in_nano", .val=BDMF_nsp_in_nano, .parms=set_default },
            { .name="ts_up_val", .val=BDMF_ts_up_val, .parms=set_ts_up_val },
            { .name="tod_rd_val", .val=BDMF_tod_rd_val, .parms=set_tod_rd_val },
            { .name="ts_rd_val", .val=BDMF_ts_rd_val, .parms=set_ts_rd_val },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "get", "get", bcm_gpon_rx_amd2_cli_get,
            BDMFMON_MAKE_PARM_SELECTOR("purpose", "purpose", selector_table, 0));
    }
    {
        static bdmfmon_enum_val_t enum_table_test_method[] = {
            { .name="low" , .val=bdmf_test_method_low },
            { .name="mid" , .val=bdmf_test_method_mid },
            { .name="high" , .val=bdmf_test_method_high },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "test", "test", bcm_gpon_rx_amd2_cli_test,
            BDMFMON_MAKE_PARM_ENUM("method", "low: 0000, mid: 1000, high: 1111", enum_table_test_method, 0));
    }
    {
        static bdmfmon_enum_val_t enum_table_address[] = {
            { .name="TOD_SF_UP" , .val=bdmf_address_tod_sf_up },
            { .name="TOD_UP_VAL" , .val=bdmf_address_tod_up_val },
            { .name="UP_ARM" , .val=bdmf_address_up_arm },
            { .name="TOD_CYC_IN_SEC" , .val=bdmf_address_tod_cyc_in_sec },
            { .name="TOD_OPPS_DUTY" , .val=bdmf_address_tod_opps_duty },
            { .name="NANOS_PER_CLK" , .val=bdmf_address_nanos_per_clk },
            { .name="NSPARTINC" , .val=bdmf_address_nspartinc },
            { .name="NSP_IN_NANO" , .val=bdmf_address_nsp_in_nano },
            { .name="TS_UP_VAL" , .val=bdmf_address_ts_up_val },
            { .name="TOD_RD_VAL" , .val=bdmf_address_tod_rd_val },
            { .name="TS_RD_VAL" , .val=bdmf_address_ts_rd_val },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "address", "address", bcm_gpon_rx_amd2_cli_address,
            BDMFMON_MAKE_PARM_ENUM("method", "method", enum_table_address, 0),
            BDMFMON_MAKE_PARM("index2", "onu_id/alloc_id/port_id/etc...", BDMFMON_PARM_NUMBER, BDMFMON_PARM_FLAG_OPTIONAL));
    }
    return dir;
}
#endif /* USE_BDMF_SHELL */


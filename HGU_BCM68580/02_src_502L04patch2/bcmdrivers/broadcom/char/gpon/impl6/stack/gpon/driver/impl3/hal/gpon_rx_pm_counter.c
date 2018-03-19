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
#include "gpon_rx_pm_counter.h"
bdmf_error_t ag_drv_gpon_rx_pm_counter_bwmaperr_get(uint32_t *bwmaperr)
{
    uint32_t reg_bwmaperr=0;

#ifdef VALIDATE_PARMS
    if(!bwmaperr)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PM_COUNTER, BWMAPERR, reg_bwmaperr);

    *bwmaperr = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, BWMAPERR, BWMAPERR, reg_bwmaperr);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_pm_counter_ploamerr_get(uint32_t *ploamerr)
{
    uint32_t reg_ploamerr=0;

#ifdef VALIDATE_PARMS
    if(!ploamerr)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PM_COUNTER, PLOAMERR, reg_ploamerr);

    *ploamerr = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, PLOAMERR, PLOAMERR, reg_ploamerr);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_pm_counter_plenderr_get(uint32_t *plenderr)
{
    uint32_t reg_plenderr=0;

#ifdef VALIDATE_PARMS
    if(!plenderr)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PM_COUNTER, PLENDERR, reg_plenderr);

    *plenderr = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, PLENDERR, PLENDERR, reg_plenderr);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_pm_counter_sfmismatch_get(uint32_t *sfmismatch)
{
    uint32_t reg_sfmismatch=0;

#ifdef VALIDATE_PARMS
    if(!sfmismatch)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PM_COUNTER, SFMISMATCH, reg_sfmismatch);

    *sfmismatch = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, SFMISMATCH, SFMISMATCH, reg_sfmismatch);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_pm_counter_bip_get(uint32_t *bip)
{
    uint32_t reg_bip=0;

#ifdef VALIDATE_PARMS
    if(!bip)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PM_COUNTER, BIP, reg_bip);

    *bip = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, BIP, BIP, reg_bip);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_pm_counter_gemfrag_get(uint32_t *gemfrag)
{
    uint32_t reg_gemfrag=0;

#ifdef VALIDATE_PARMS
    if(!gemfrag)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PM_COUNTER, GEMFRAG, reg_gemfrag);

    *gemfrag = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, GEMFRAG, GEMFRAG, reg_gemfrag);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_pm_counter_ethpack_get(uint32_t *ethpack)
{
    uint32_t reg_ethpack=0;

#ifdef VALIDATE_PARMS
    if(!ethpack)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PM_COUNTER, ETHPACK, reg_ethpack);

    *ethpack = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, ETHPACK, ETHPACK, reg_ethpack);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_pm_counter_tdmfrag_get(uint32_t *tdmfrag)
{
    uint32_t reg_tdmfrag=0;

#ifdef VALIDATE_PARMS
    if(!tdmfrag)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PM_COUNTER, TDMFRAG, reg_tdmfrag);

    *tdmfrag = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, TDMFRAG, TDMFRAG, reg_tdmfrag);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_pm_counter_symerror_get(uint32_t *symerrors)
{
    uint32_t reg_symerror=0;

#ifdef VALIDATE_PARMS
    if(!symerrors)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PM_COUNTER, SYMERROR, reg_symerror);

    *symerrors = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, SYMERROR, SYMERRORS, reg_symerror);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_pm_counter_biterror_get(uint32_t *biterrors)
{
    uint32_t reg_biterror=0;

#ifdef VALIDATE_PARMS
    if(!biterrors)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PM_COUNTER, BITERROR, reg_biterror);

    *biterrors = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, BITERROR, BITERRORS, reg_biterror);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_pm_counter_aeserror_get(uint32_t *aeserrcnt)
{
    uint32_t reg_aeserror=0;

#ifdef VALIDATE_PARMS
    if(!aeserrcnt)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PM_COUNTER, AESERROR, reg_aeserror);

    *aeserrcnt = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, AESERROR, AESERRCNT, reg_aeserror);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_pm_counter_tdmstats_get(uint32_t *rcvbytes)
{
    uint32_t reg_tdmstats=0;

#ifdef VALIDATE_PARMS
    if(!rcvbytes)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PM_COUNTER, TDMSTATS, reg_tdmstats);

    *rcvbytes = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, TDMSTATS, RCVBYTES, reg_tdmstats);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_pm_counter_ethfrag_get(uint32_t *ethfrag)
{
    uint32_t reg_ethfrag=0;

#ifdef VALIDATE_PARMS
    if(!ethfrag)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PM_COUNTER, ETHFRAG, reg_ethfrag);

    *ethfrag = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, ETHFRAG, ETHFRAG, reg_ethfrag);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_pm_counter_ethbytecnt_get(uint32_t *ethbytecnt)
{
    uint32_t reg_ethbytecnt=0;

#ifdef VALIDATE_PARMS
    if(!ethbytecnt)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PM_COUNTER, ETHBYTECNT, reg_ethbytecnt);

    *ethbytecnt = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, ETHBYTECNT, ETHBYTECNT, reg_ethbytecnt);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_pm_counter_fecuccwcnt_get(uint16_t *fecuccwcnt)
{
    uint32_t reg_fecuccwcnt=0;

#ifdef VALIDATE_PARMS
    if(!fecuccwcnt)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PM_COUNTER, FECUCCWCNT, reg_fecuccwcnt);

    *fecuccwcnt = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, FECUCCWCNT, FECUCCWCNT, reg_fecuccwcnt);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_pm_counter_fecccwcnt_get(uint32_t *fecccwcnt)
{
    uint32_t reg_fecccwcnt=0;

#ifdef VALIDATE_PARMS
    if(!fecccwcnt)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PM_COUNTER, FECCCWCNT, reg_fecccwcnt);

    *fecccwcnt = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, FECCCWCNT, FECCCWCNT, reg_fecccwcnt);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_pm_counter_plonucnt_get(uint32_t *pl_onu_cnt)
{
    uint32_t reg_plonucnt=0;

#ifdef VALIDATE_PARMS
    if(!pl_onu_cnt)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PM_COUNTER, PLONUCNT, reg_plonucnt);

    *pl_onu_cnt = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, PLONUCNT, PL_ONU_CNT, reg_plonucnt);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_pm_counter_plbcstcnt_get(uint32_t *pl_bcst_cnt)
{
    uint32_t reg_plbcstcnt=0;

#ifdef VALIDATE_PARMS
    if(!pl_bcst_cnt)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PM_COUNTER, PLBCSTCNT, reg_plbcstcnt);

    *pl_bcst_cnt = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, PLBCSTCNT, PL_BCST_CNT, reg_plbcstcnt);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_rx_pm_counter_bw_cnt_get(uint32_t tcont_idx, uint32_t *bw_cnt)
{
    uint32_t reg_bw_cnt=0;

#ifdef VALIDATE_PARMS
    if(!bw_cnt)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
    if((tcont_idx >= 4))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, tcont_idx, GPON_RX_PM_COUNTER, BW_CNT, reg_bw_cnt);

    *bw_cnt = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, BW_CNT, BW_CNT, reg_bw_cnt);

    return BDMF_ERR_OK;
}

#ifdef USE_BDMF_SHELL
enum
{
    BDMF_bwmaperr,
    BDMF_ploamerr,
    BDMF_plenderr,
    BDMF_sfmismatch,
    BDMF_bip,
    BDMF_gemfrag,
    BDMF_ethpack,
    BDMF_tdmfrag,
    BDMF_symerror,
    BDMF_biterror,
    BDMF_aeserror,
    BDMF_tdmstats,
    BDMF_ethfrag,
    BDMF_ethbytecnt,
    BDMF_fecuccwcnt,
    BDMF_fecccwcnt,
    BDMF_plonucnt,
    BDMF_plbcstcnt,
    BDMF_bw_cnt,
};

typedef enum
{
    bdmf_address_bwmaperr,
    bdmf_address_ploamerr,
    bdmf_address_plenderr,
    bdmf_address_sfmismatch,
    bdmf_address_bip,
    bdmf_address_gemfrag,
    bdmf_address_ethpack,
    bdmf_address_tdmfrag,
    bdmf_address_symerror,
    bdmf_address_biterror,
    bdmf_address_aeserror,
    bdmf_address_tdmstats,
    bdmf_address_ethfrag,
    bdmf_address_ethbytecnt,
    bdmf_address_fecuccwcnt,
    bdmf_address_fecccwcnt,
    bdmf_address_plonucnt,
    bdmf_address_plbcstcnt,
    bdmf_address_bw_cnt,
}
bdmf_address;

static int bcm_gpon_rx_pm_counter_cli_set(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
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

static int bcm_gpon_rx_pm_counter_cli_get(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_error_t err;

    switch(parm[0].value.unumber)
    {
    case BDMF_bwmaperr:
    {
        uint32_t bwmaperr;
        err = ag_drv_gpon_rx_pm_counter_bwmaperr_get(&bwmaperr);
        bdmf_session_print(session, "bwmaperr = %u = 0x%x\n", bwmaperr, bwmaperr);
        break;
    }
    case BDMF_ploamerr:
    {
        uint32_t ploamerr;
        err = ag_drv_gpon_rx_pm_counter_ploamerr_get(&ploamerr);
        bdmf_session_print(session, "ploamerr = %u = 0x%x\n", ploamerr, ploamerr);
        break;
    }
    case BDMF_plenderr:
    {
        uint32_t plenderr;
        err = ag_drv_gpon_rx_pm_counter_plenderr_get(&plenderr);
        bdmf_session_print(session, "plenderr = %u = 0x%x\n", plenderr, plenderr);
        break;
    }
    case BDMF_sfmismatch:
    {
        uint32_t sfmismatch;
        err = ag_drv_gpon_rx_pm_counter_sfmismatch_get(&sfmismatch);
        bdmf_session_print(session, "sfmismatch = %u = 0x%x\n", sfmismatch, sfmismatch);
        break;
    }
    case BDMF_bip:
    {
        uint32_t bip;
        err = ag_drv_gpon_rx_pm_counter_bip_get(&bip);
        bdmf_session_print(session, "bip = %u = 0x%x\n", bip, bip);
        break;
    }
    case BDMF_gemfrag:
    {
        uint32_t gemfrag;
        err = ag_drv_gpon_rx_pm_counter_gemfrag_get(&gemfrag);
        bdmf_session_print(session, "gemfrag = %u = 0x%x\n", gemfrag, gemfrag);
        break;
    }
    case BDMF_ethpack:
    {
        uint32_t ethpack;
        err = ag_drv_gpon_rx_pm_counter_ethpack_get(&ethpack);
        bdmf_session_print(session, "ethpack = %u = 0x%x\n", ethpack, ethpack);
        break;
    }
    case BDMF_tdmfrag:
    {
        uint32_t tdmfrag;
        err = ag_drv_gpon_rx_pm_counter_tdmfrag_get(&tdmfrag);
        bdmf_session_print(session, "tdmfrag = %u = 0x%x\n", tdmfrag, tdmfrag);
        break;
    }
    case BDMF_symerror:
    {
        uint32_t symerrors;
        err = ag_drv_gpon_rx_pm_counter_symerror_get(&symerrors);
        bdmf_session_print(session, "symerrors = %u = 0x%x\n", symerrors, symerrors);
        break;
    }
    case BDMF_biterror:
    {
        uint32_t biterrors;
        err = ag_drv_gpon_rx_pm_counter_biterror_get(&biterrors);
        bdmf_session_print(session, "biterrors = %u = 0x%x\n", biterrors, biterrors);
        break;
    }
    case BDMF_aeserror:
    {
        uint32_t aeserrcnt;
        err = ag_drv_gpon_rx_pm_counter_aeserror_get(&aeserrcnt);
        bdmf_session_print(session, "aeserrcnt = %u = 0x%x\n", aeserrcnt, aeserrcnt);
        break;
    }
    case BDMF_tdmstats:
    {
        uint32_t rcvbytes;
        err = ag_drv_gpon_rx_pm_counter_tdmstats_get(&rcvbytes);
        bdmf_session_print(session, "rcvbytes = %u = 0x%x\n", rcvbytes, rcvbytes);
        break;
    }
    case BDMF_ethfrag:
    {
        uint32_t ethfrag;
        err = ag_drv_gpon_rx_pm_counter_ethfrag_get(&ethfrag);
        bdmf_session_print(session, "ethfrag = %u = 0x%x\n", ethfrag, ethfrag);
        break;
    }
    case BDMF_ethbytecnt:
    {
        uint32_t ethbytecnt;
        err = ag_drv_gpon_rx_pm_counter_ethbytecnt_get(&ethbytecnt);
        bdmf_session_print(session, "ethbytecnt = %u = 0x%x\n", ethbytecnt, ethbytecnt);
        break;
    }
    case BDMF_fecuccwcnt:
    {
        uint16_t fecuccwcnt;
        err = ag_drv_gpon_rx_pm_counter_fecuccwcnt_get(&fecuccwcnt);
        bdmf_session_print(session, "fecuccwcnt = %u = 0x%x\n", fecuccwcnt, fecuccwcnt);
        break;
    }
    case BDMF_fecccwcnt:
    {
        uint32_t fecccwcnt;
        err = ag_drv_gpon_rx_pm_counter_fecccwcnt_get(&fecccwcnt);
        bdmf_session_print(session, "fecccwcnt = %u = 0x%x\n", fecccwcnt, fecccwcnt);
        break;
    }
    case BDMF_plonucnt:
    {
        uint32_t pl_onu_cnt;
        err = ag_drv_gpon_rx_pm_counter_plonucnt_get(&pl_onu_cnt);
        bdmf_session_print(session, "pl_onu_cnt = %u = 0x%x\n", pl_onu_cnt, pl_onu_cnt);
        break;
    }
    case BDMF_plbcstcnt:
    {
        uint32_t pl_bcst_cnt;
        err = ag_drv_gpon_rx_pm_counter_plbcstcnt_get(&pl_bcst_cnt);
        bdmf_session_print(session, "pl_bcst_cnt = %u = 0x%x\n", pl_bcst_cnt, pl_bcst_cnt);
        break;
    }
    case BDMF_bw_cnt:
    {
        uint32_t bw_cnt;
        err = ag_drv_gpon_rx_pm_counter_bw_cnt_get(parm[1].value.unumber, &bw_cnt);
        bdmf_session_print(session, "bw_cnt = %u = 0x%x\n", bw_cnt, bw_cnt);
        break;
    }
    default:
        err = BDMF_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

static int bcm_gpon_rx_pm_counter_cli_test(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_test_method m = parm[0].value.unumber;
    bdmf_error_t err = BDMF_ERR_OK;

    {
        uint32_t bwmaperr=gtmv(m, 32);
        if(!err) ag_drv_gpon_rx_pm_counter_bwmaperr_get( &bwmaperr);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_pm_counter_bwmaperr_get( %u)\n", bwmaperr);
    }
    {
        uint32_t ploamerr=gtmv(m, 32);
        if(!err) ag_drv_gpon_rx_pm_counter_ploamerr_get( &ploamerr);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_pm_counter_ploamerr_get( %u)\n", ploamerr);
    }
    {
        uint32_t plenderr=gtmv(m, 32);
        if(!err) ag_drv_gpon_rx_pm_counter_plenderr_get( &plenderr);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_pm_counter_plenderr_get( %u)\n", plenderr);
    }
    {
        uint32_t sfmismatch=gtmv(m, 32);
        if(!err) ag_drv_gpon_rx_pm_counter_sfmismatch_get( &sfmismatch);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_pm_counter_sfmismatch_get( %u)\n", sfmismatch);
    }
    {
        uint32_t bip=gtmv(m, 32);
        if(!err) ag_drv_gpon_rx_pm_counter_bip_get( &bip);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_pm_counter_bip_get( %u)\n", bip);
    }
    {
        uint32_t gemfrag=gtmv(m, 32);
        if(!err) ag_drv_gpon_rx_pm_counter_gemfrag_get( &gemfrag);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_pm_counter_gemfrag_get( %u)\n", gemfrag);
    }
    {
        uint32_t ethpack=gtmv(m, 32);
        if(!err) ag_drv_gpon_rx_pm_counter_ethpack_get( &ethpack);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_pm_counter_ethpack_get( %u)\n", ethpack);
    }
    {
        uint32_t tdmfrag=gtmv(m, 32);
        if(!err) ag_drv_gpon_rx_pm_counter_tdmfrag_get( &tdmfrag);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_pm_counter_tdmfrag_get( %u)\n", tdmfrag);
    }
    {
        uint32_t symerrors=gtmv(m, 32);
        if(!err) ag_drv_gpon_rx_pm_counter_symerror_get( &symerrors);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_pm_counter_symerror_get( %u)\n", symerrors);
    }
    {
        uint32_t biterrors=gtmv(m, 32);
        if(!err) ag_drv_gpon_rx_pm_counter_biterror_get( &biterrors);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_pm_counter_biterror_get( %u)\n", biterrors);
    }
    {
        uint32_t aeserrcnt=gtmv(m, 32);
        if(!err) ag_drv_gpon_rx_pm_counter_aeserror_get( &aeserrcnt);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_pm_counter_aeserror_get( %u)\n", aeserrcnt);
    }
    {
        uint32_t rcvbytes=gtmv(m, 32);
        if(!err) ag_drv_gpon_rx_pm_counter_tdmstats_get( &rcvbytes);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_pm_counter_tdmstats_get( %u)\n", rcvbytes);
    }
    {
        uint32_t ethfrag=gtmv(m, 32);
        if(!err) ag_drv_gpon_rx_pm_counter_ethfrag_get( &ethfrag);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_pm_counter_ethfrag_get( %u)\n", ethfrag);
    }
    {
        uint32_t ethbytecnt=gtmv(m, 32);
        if(!err) ag_drv_gpon_rx_pm_counter_ethbytecnt_get( &ethbytecnt);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_pm_counter_ethbytecnt_get( %u)\n", ethbytecnt);
    }
    {
        uint16_t fecuccwcnt=gtmv(m, 16);
        if(!err) ag_drv_gpon_rx_pm_counter_fecuccwcnt_get( &fecuccwcnt);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_pm_counter_fecuccwcnt_get( %u)\n", fecuccwcnt);
    }
    {
        uint32_t fecccwcnt=gtmv(m, 32);
        if(!err) ag_drv_gpon_rx_pm_counter_fecccwcnt_get( &fecccwcnt);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_pm_counter_fecccwcnt_get( %u)\n", fecccwcnt);
    }
    {
        uint32_t pl_onu_cnt=gtmv(m, 32);
        if(!err) ag_drv_gpon_rx_pm_counter_plonucnt_get( &pl_onu_cnt);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_pm_counter_plonucnt_get( %u)\n", pl_onu_cnt);
    }
    {
        uint32_t pl_bcst_cnt=gtmv(m, 32);
        if(!err) ag_drv_gpon_rx_pm_counter_plbcstcnt_get( &pl_bcst_cnt);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_pm_counter_plbcstcnt_get( %u)\n", pl_bcst_cnt);
    }
    {
        uint32_t tcont_idx=gtmv(m, 2);
        uint32_t bw_cnt=gtmv(m, 32);
        if(!err) ag_drv_gpon_rx_pm_counter_bw_cnt_get( tcont_idx, &bw_cnt);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_rx_pm_counter_bw_cnt_get( %u %u)\n", tcont_idx, bw_cnt);
    }
    return err;
}

static int bcm_gpon_rx_pm_counter_cli_address(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
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
    case bdmf_address_bwmaperr : reg = &RU_REG(GPON_RX_PM_COUNTER, BWMAPERR); blk = &RU_BLK(GPON_RX_PM_COUNTER); break;
    case bdmf_address_ploamerr : reg = &RU_REG(GPON_RX_PM_COUNTER, PLOAMERR); blk = &RU_BLK(GPON_RX_PM_COUNTER); break;
    case bdmf_address_plenderr : reg = &RU_REG(GPON_RX_PM_COUNTER, PLENDERR); blk = &RU_BLK(GPON_RX_PM_COUNTER); break;
    case bdmf_address_sfmismatch : reg = &RU_REG(GPON_RX_PM_COUNTER, SFMISMATCH); blk = &RU_BLK(GPON_RX_PM_COUNTER); break;
    case bdmf_address_bip : reg = &RU_REG(GPON_RX_PM_COUNTER, BIP); blk = &RU_BLK(GPON_RX_PM_COUNTER); break;
    case bdmf_address_gemfrag : reg = &RU_REG(GPON_RX_PM_COUNTER, GEMFRAG); blk = &RU_BLK(GPON_RX_PM_COUNTER); break;
    case bdmf_address_ethpack : reg = &RU_REG(GPON_RX_PM_COUNTER, ETHPACK); blk = &RU_BLK(GPON_RX_PM_COUNTER); break;
    case bdmf_address_tdmfrag : reg = &RU_REG(GPON_RX_PM_COUNTER, TDMFRAG); blk = &RU_BLK(GPON_RX_PM_COUNTER); break;
    case bdmf_address_symerror : reg = &RU_REG(GPON_RX_PM_COUNTER, SYMERROR); blk = &RU_BLK(GPON_RX_PM_COUNTER); break;
    case bdmf_address_biterror : reg = &RU_REG(GPON_RX_PM_COUNTER, BITERROR); blk = &RU_BLK(GPON_RX_PM_COUNTER); break;
    case bdmf_address_aeserror : reg = &RU_REG(GPON_RX_PM_COUNTER, AESERROR); blk = &RU_BLK(GPON_RX_PM_COUNTER); break;
    case bdmf_address_tdmstats : reg = &RU_REG(GPON_RX_PM_COUNTER, TDMSTATS); blk = &RU_BLK(GPON_RX_PM_COUNTER); break;
    case bdmf_address_ethfrag : reg = &RU_REG(GPON_RX_PM_COUNTER, ETHFRAG); blk = &RU_BLK(GPON_RX_PM_COUNTER); break;
    case bdmf_address_ethbytecnt : reg = &RU_REG(GPON_RX_PM_COUNTER, ETHBYTECNT); blk = &RU_BLK(GPON_RX_PM_COUNTER); break;
    case bdmf_address_fecuccwcnt : reg = &RU_REG(GPON_RX_PM_COUNTER, FECUCCWCNT); blk = &RU_BLK(GPON_RX_PM_COUNTER); break;
    case bdmf_address_fecccwcnt : reg = &RU_REG(GPON_RX_PM_COUNTER, FECCCWCNT); blk = &RU_BLK(GPON_RX_PM_COUNTER); break;
    case bdmf_address_plonucnt : reg = &RU_REG(GPON_RX_PM_COUNTER, PLONUCNT); blk = &RU_BLK(GPON_RX_PM_COUNTER); break;
    case bdmf_address_plbcstcnt : reg = &RU_REG(GPON_RX_PM_COUNTER, PLBCSTCNT); blk = &RU_BLK(GPON_RX_PM_COUNTER); break;
    case bdmf_address_bw_cnt : reg = &RU_REG(GPON_RX_PM_COUNTER, BW_CNT); blk = &RU_BLK(GPON_RX_PM_COUNTER); break;
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

bdmfmon_handle_t ag_drv_gpon_rx_pm_counter_cli_init(bdmfmon_handle_t driver_dir)
{
    bdmfmon_handle_t dir;

    if ((dir = bdmfmon_dir_find(driver_dir, "gpon_rx_pm_counter"))!=NULL)
        return dir;
    dir = bdmfmon_dir_add(driver_dir, "gpon_rx_pm_counter", "gpon_rx_pm_counter", BDMF_ACCESS_ADMIN, NULL);

    {
        static bdmfmon_enum_val_t selector_table[] = {
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "set", "set", bcm_gpon_rx_pm_counter_cli_set,
            BDMFMON_MAKE_PARM_SELECTOR("purpose", "purpose", selector_table, 0));
    }
    {
        static bdmfmon_cmd_parm_t set_default[]={
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_bw_cnt[]={
            BDMFMON_MAKE_PARM("tcont_idx", "tcont_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_enum_val_t selector_table[] = {
            { .name="bwmaperr", .val=BDMF_bwmaperr, .parms=set_default },
            { .name="ploamerr", .val=BDMF_ploamerr, .parms=set_default },
            { .name="plenderr", .val=BDMF_plenderr, .parms=set_default },
            { .name="sfmismatch", .val=BDMF_sfmismatch, .parms=set_default },
            { .name="bip", .val=BDMF_bip, .parms=set_default },
            { .name="gemfrag", .val=BDMF_gemfrag, .parms=set_default },
            { .name="ethpack", .val=BDMF_ethpack, .parms=set_default },
            { .name="tdmfrag", .val=BDMF_tdmfrag, .parms=set_default },
            { .name="symerror", .val=BDMF_symerror, .parms=set_default },
            { .name="biterror", .val=BDMF_biterror, .parms=set_default },
            { .name="aeserror", .val=BDMF_aeserror, .parms=set_default },
            { .name="tdmstats", .val=BDMF_tdmstats, .parms=set_default },
            { .name="ethfrag", .val=BDMF_ethfrag, .parms=set_default },
            { .name="ethbytecnt", .val=BDMF_ethbytecnt, .parms=set_default },
            { .name="fecuccwcnt", .val=BDMF_fecuccwcnt, .parms=set_default },
            { .name="fecccwcnt", .val=BDMF_fecccwcnt, .parms=set_default },
            { .name="plonucnt", .val=BDMF_plonucnt, .parms=set_default },
            { .name="plbcstcnt", .val=BDMF_plbcstcnt, .parms=set_default },
            { .name="bw_cnt", .val=BDMF_bw_cnt, .parms=set_bw_cnt },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "get", "get", bcm_gpon_rx_pm_counter_cli_get,
            BDMFMON_MAKE_PARM_SELECTOR("purpose", "purpose", selector_table, 0));
    }
    {
        static bdmfmon_enum_val_t enum_table_test_method[] = {
            { .name="low" , .val=bdmf_test_method_low },
            { .name="mid" , .val=bdmf_test_method_mid },
            { .name="high" , .val=bdmf_test_method_high },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "test", "test", bcm_gpon_rx_pm_counter_cli_test,
            BDMFMON_MAKE_PARM_ENUM("method", "low: 0000, mid: 1000, high: 1111", enum_table_test_method, 0));
    }
    {
        static bdmfmon_enum_val_t enum_table_address[] = {
            { .name="BWMAPERR" , .val=bdmf_address_bwmaperr },
            { .name="PLOAMERR" , .val=bdmf_address_ploamerr },
            { .name="PLENDERR" , .val=bdmf_address_plenderr },
            { .name="SFMISMATCH" , .val=bdmf_address_sfmismatch },
            { .name="BIP" , .val=bdmf_address_bip },
            { .name="GEMFRAG" , .val=bdmf_address_gemfrag },
            { .name="ETHPACK" , .val=bdmf_address_ethpack },
            { .name="TDMFRAG" , .val=bdmf_address_tdmfrag },
            { .name="SYMERROR" , .val=bdmf_address_symerror },
            { .name="BITERROR" , .val=bdmf_address_biterror },
            { .name="AESERROR" , .val=bdmf_address_aeserror },
            { .name="TDMSTATS" , .val=bdmf_address_tdmstats },
            { .name="ETHFRAG" , .val=bdmf_address_ethfrag },
            { .name="ETHBYTECNT" , .val=bdmf_address_ethbytecnt },
            { .name="FECUCCWCNT" , .val=bdmf_address_fecuccwcnt },
            { .name="FECCCWCNT" , .val=bdmf_address_fecccwcnt },
            { .name="PLONUCNT" , .val=bdmf_address_plonucnt },
            { .name="PLBCSTCNT" , .val=bdmf_address_plbcstcnt },
            { .name="BW_CNT" , .val=bdmf_address_bw_cnt },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "address", "address", bcm_gpon_rx_pm_counter_cli_address,
            BDMFMON_MAKE_PARM_ENUM("method", "method", enum_table_address, 0),
            BDMFMON_MAKE_PARM("index2", "onu_id/alloc_id/port_id/etc...", BDMFMON_PARM_NUMBER, BDMFMON_PARM_FLAG_OPTIONAL));
    }
    return dir;
}
#endif /* USE_BDMF_SHELL */


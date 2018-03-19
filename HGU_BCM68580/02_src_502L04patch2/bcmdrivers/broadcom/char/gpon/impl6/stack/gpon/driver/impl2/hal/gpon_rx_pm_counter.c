/*
* <:copyright-BRCM:2016:proprietary:gpon
* 
*    Copyright (c) 2016 Broadcom 
*    All Rights Reserved
* 
*  This program is the proprietary software of Broadcom and/or its
*  licensors, and may only be used, duplicated, modified or distributed pursuant
*  to the terms and conditions of a separate, written license agreement executed
*  between you and Broadcom (an "Authorized License").  Except as set forth in
*  an Authorized License, Broadcom grants no license (express or implied), right
*  to use, or waiver of any kind with respect to the Software, and Broadcom
*  expressly reserves all rights in and to the Software and all intellectual
*  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
*  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
*  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
* 
*  Except as expressly set forth in the Authorized License,
* 
*  1. This program, including its structure, sequence and organization,
*     constitutes the valuable trade secrets of Broadcom, and you shall use
*     all reasonable efforts to protect the confidentiality thereof, and to
*     use this information only in connection with your use of Broadcom
*     integrated circuit products.
* 
*  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
*     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
*     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
*     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
*     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
*     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
*     PERFORMANCE OF THE SOFTWARE.
* 
*  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
*     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
*     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
*     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
*     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
*     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
*     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
*     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
*     LIMITED REMEDY.
* :>
*/

#include "gpon_common.h"
#include "gpon_rx_pm_counter.h"
int ag_drv_gpon_rx_pm_counter_bwmaperr_get(uint32_t *bwmaperr)
{
    uint32_t reg_bwmaperr=0;

#ifdef VALIDATE_PARMS
    if(!bwmaperr)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PM_COUNTER, BWMAPERR, reg_bwmaperr);

    *bwmaperr = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, BWMAPERR, BWMAPERR, reg_bwmaperr);

    return 0;
}

int ag_drv_gpon_rx_pm_counter_ploamerr_get(uint32_t *ploamerr)
{
    uint32_t reg_ploamerr=0;

#ifdef VALIDATE_PARMS
    if(!ploamerr)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PM_COUNTER, PLOAMERR, reg_ploamerr);

    *ploamerr = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, PLOAMERR, PLOAMERR, reg_ploamerr);

    return 0;
}

int ag_drv_gpon_rx_pm_counter_plenderr_get(uint32_t *plenderr)
{
    uint32_t reg_plenderr=0;

#ifdef VALIDATE_PARMS
    if(!plenderr)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PM_COUNTER, PLENDERR, reg_plenderr);

    *plenderr = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, PLENDERR, PLENDERR, reg_plenderr);

    return 0;
}

int ag_drv_gpon_rx_pm_counter_sfmismatch_get(uint32_t *sfmismatch)
{
    uint32_t reg_sfmismatch=0;

#ifdef VALIDATE_PARMS
    if(!sfmismatch)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PM_COUNTER, SFMISMATCH, reg_sfmismatch);

    *sfmismatch = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, SFMISMATCH, SFMISMATCH, reg_sfmismatch);

    return 0;
}

int ag_drv_gpon_rx_pm_counter_bip_get(uint32_t *bip)
{
    uint32_t reg_bip=0;

#ifdef VALIDATE_PARMS
    if(!bip)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PM_COUNTER, BIP, reg_bip);

    *bip = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, BIP, BIP, reg_bip);

    return 0;
}

int ag_drv_gpon_rx_pm_counter_gemfrag_get(uint32_t *gemfrag)
{
    uint32_t reg_gemfrag=0;

#ifdef VALIDATE_PARMS
    if(!gemfrag)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PM_COUNTER, GEMFRAG, reg_gemfrag);

    *gemfrag = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, GEMFRAG, GEMFRAG, reg_gemfrag);

    return 0;
}

int ag_drv_gpon_rx_pm_counter_ethpack_get(uint32_t *ethpack)
{
    uint32_t reg_ethpack=0;

#ifdef VALIDATE_PARMS
    if(!ethpack)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PM_COUNTER, ETHPACK, reg_ethpack);

    *ethpack = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, ETHPACK, ETHPACK, reg_ethpack);

    return 0;
}

int ag_drv_gpon_rx_pm_counter_tdmfrag_get(uint32_t *tdmfrag)
{
    uint32_t reg_tdmfrag=0;

#ifdef VALIDATE_PARMS
    if(!tdmfrag)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PM_COUNTER, TDMFRAG, reg_tdmfrag);

    *tdmfrag = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, TDMFRAG, TDMFRAG, reg_tdmfrag);

    return 0;
}

int ag_drv_gpon_rx_pm_counter_symerror_get(uint32_t *symerrors)
{
    uint32_t reg_symerror=0;

#ifdef VALIDATE_PARMS
    if(!symerrors)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PM_COUNTER, SYMERROR, reg_symerror);

    *symerrors = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, SYMERROR, SYMERRORS, reg_symerror);

    return 0;
}

int ag_drv_gpon_rx_pm_counter_biterror_get(uint32_t *biterrors)
{
    uint32_t reg_biterror=0;

#ifdef VALIDATE_PARMS
    if(!biterrors)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PM_COUNTER, BITERROR, reg_biterror);

    *biterrors = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, BITERROR, BITERRORS, reg_biterror);

    return 0;
}

int ag_drv_gpon_rx_pm_counter_aeserror_get(uint32_t *aeserrcnt)
{
    uint32_t reg_aeserror=0;

#ifdef VALIDATE_PARMS
    if(!aeserrcnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PM_COUNTER, AESERROR, reg_aeserror);

    *aeserrcnt = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, AESERROR, AESERRCNT, reg_aeserror);

    return 0;
}

int ag_drv_gpon_rx_pm_counter_tdmstats_get(uint32_t *rcvbytes)
{
    uint32_t reg_tdmstats=0;

#ifdef VALIDATE_PARMS
    if(!rcvbytes)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PM_COUNTER, TDMSTATS, reg_tdmstats);

    *rcvbytes = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, TDMSTATS, RCVBYTES, reg_tdmstats);

    return 0;
}

int ag_drv_gpon_rx_pm_counter_ethfrag_get(uint32_t *ethfrag)
{
    uint32_t reg_ethfrag=0;

#ifdef VALIDATE_PARMS
    if(!ethfrag)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PM_COUNTER, ETHFRAG, reg_ethfrag);

    *ethfrag = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, ETHFRAG, ETHFRAG, reg_ethfrag);

    return 0;
}

int ag_drv_gpon_rx_pm_counter_ethbytecnt_get(uint32_t *ethbytecnt)
{
    uint32_t reg_ethbytecnt=0;

#ifdef VALIDATE_PARMS
    if(!ethbytecnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PM_COUNTER, ETHBYTECNT, reg_ethbytecnt);

    *ethbytecnt = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, ETHBYTECNT, ETHBYTECNT, reg_ethbytecnt);

    return 0;
}

int ag_drv_gpon_rx_pm_counter_fecuccwcnt_get(uint16_t *fecuccwcnt)
{
    uint32_t reg_fecuccwcnt=0;

#ifdef VALIDATE_PARMS
    if(!fecuccwcnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PM_COUNTER, FECUCCWCNT, reg_fecuccwcnt);

    *fecuccwcnt = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, FECUCCWCNT, FECUCCWCNT, reg_fecuccwcnt);

    return 0;
}

int ag_drv_gpon_rx_pm_counter_fecccwcnt_get(uint32_t *fecccwcnt)
{
    uint32_t reg_fecccwcnt=0;

#ifdef VALIDATE_PARMS
    if(!fecccwcnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PM_COUNTER, FECCCWCNT, reg_fecccwcnt);

    *fecccwcnt = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, FECCCWCNT, FECCCWCNT, reg_fecccwcnt);

    return 0;
}

int ag_drv_gpon_rx_pm_counter_plonucnt_get(uint32_t *pl_onu_cnt)
{
    uint32_t reg_plonucnt=0;

#ifdef VALIDATE_PARMS
    if(!pl_onu_cnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PM_COUNTER, PLONUCNT, reg_plonucnt);

    *pl_onu_cnt = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, PLONUCNT, PL_ONU_CNT, reg_plonucnt);

    return 0;
}

int ag_drv_gpon_rx_pm_counter_plbcstcnt_get(uint32_t *pl_bcst_cnt)
{
    uint32_t reg_plbcstcnt=0;

#ifdef VALIDATE_PARMS
    if(!pl_bcst_cnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_RX_PM_COUNTER, PLBCSTCNT, reg_plbcstcnt);

    *pl_bcst_cnt = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, PLBCSTCNT, PL_BCST_CNT, reg_plbcstcnt);

    return 0;
}

int ag_drv_gpon_rx_pm_counter_bw_cnt_get(uint32_t tcont_idx, uint32_t *bw_cnt)
{
    uint32_t reg_bw_cnt=0;

#ifdef VALIDATE_PARMS
    if(!bw_cnt)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((tcont_idx >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, tcont_idx, GPON_RX_PM_COUNTER, BW_CNT, reg_bw_cnt);

    *bw_cnt = RU_FIELD_GET(0, GPON_RX_PM_COUNTER, BW_CNT, BW_CNT, reg_bw_cnt);

    return 0;
}


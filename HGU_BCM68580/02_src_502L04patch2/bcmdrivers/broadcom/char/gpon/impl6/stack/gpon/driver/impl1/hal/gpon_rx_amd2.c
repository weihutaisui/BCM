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
#include "gpon_rx_amd2.h"
int ag_drv_gpon_rx_amd2_tod_sf_up_set(uint32_t up_sf)
{
    uint32_t reg_tod_sf_up=0;

#ifdef VALIDATE_PARMS
    if((up_sf >= _30BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_tod_sf_up = RU_FIELD_SET(0, GPON_RX_AMD2, TOD_SF_UP, UP_SF, reg_tod_sf_up, up_sf);

    RU_REG_WRITE(0, GPON_RX_AMD2, TOD_SF_UP, reg_tod_sf_up);

    return 0;
}

int ag_drv_gpon_rx_amd2_tod_sf_up_get(uint32_t *up_sf)
{
    uint32_t reg_tod_sf_up=0;

#ifdef VALIDATE_PARMS
    if(!up_sf)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;;
    }
#endif

    RU_REG_READ(0, GPON_RX_AMD2, TOD_SF_UP, reg_tod_sf_up);

    *up_sf = RU_FIELD_GET(0, GPON_RX_AMD2, TOD_SF_UP, UP_SF, reg_tod_sf_up);

    return 0;
}

int ag_drv_gpon_rx_amd2_tod_up_val_set(uint32_t word_id, uint32_t tod_val)
{
    uint32_t reg_tod_up_val=0;

#ifdef VALIDATE_PARMS
    if((word_id >= 2))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_tod_up_val = RU_FIELD_SET(0, GPON_RX_AMD2, TOD_UP_VAL, TOD_VAL, reg_tod_up_val, tod_val);

    RU_REG_RAM_WRITE(0, word_id, GPON_RX_AMD2, TOD_UP_VAL, reg_tod_up_val);

    return 0;
}

int ag_drv_gpon_rx_amd2_tod_up_val_get(uint32_t word_id, uint32_t *tod_val)
{
    uint32_t reg_tod_up_val=0;

#ifdef VALIDATE_PARMS
    if(!tod_val)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;;
    }
    if((word_id >= 2))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, word_id, GPON_RX_AMD2, TOD_UP_VAL, reg_tod_up_val);

    *tod_val = RU_FIELD_GET(0, GPON_RX_AMD2, TOD_UP_VAL, TOD_VAL, reg_tod_up_val);

    return 0;
}

int ag_drv_gpon_rx_amd2_tod_arm_set(uint8_t arm, uint8_t en)
{
    uint32_t reg_tod_arm=0;

#ifdef VALIDATE_PARMS
    if((arm >= _1BITS_MAX_VAL_) ||
       (en >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_tod_arm = RU_FIELD_SET(0, GPON_RX_AMD2, TOD_ARM, ARM, reg_tod_arm, arm);
    reg_tod_arm = RU_FIELD_SET(0, GPON_RX_AMD2, TOD_ARM, EN, reg_tod_arm, en);

    RU_REG_WRITE(0, GPON_RX_AMD2, TOD_ARM, reg_tod_arm);

    return 0;
}

int ag_drv_gpon_rx_amd2_tod_arm_get(uint8_t *arm, uint8_t *en)
{
    uint32_t reg_tod_arm=0;

#ifdef VALIDATE_PARMS
    if(!arm || !en)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;;
    }
#endif

    RU_REG_READ(0, GPON_RX_AMD2, TOD_ARM, reg_tod_arm);

    *arm = RU_FIELD_GET(0, GPON_RX_AMD2, TOD_ARM, ARM, reg_tod_arm);
    *en = RU_FIELD_GET(0, GPON_RX_AMD2, TOD_ARM, EN, reg_tod_arm);

    return 0;
}

int ag_drv_gpon_rx_amd2_tod_cyc_in_sec_set(uint32_t cyc_in_sec)
{
    uint32_t reg_tod_cyc_in_sec=0;

#ifdef VALIDATE_PARMS
    if((cyc_in_sec >= _30BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_tod_cyc_in_sec = RU_FIELD_SET(0, GPON_RX_AMD2, TOD_CYC_IN_SEC, CYC_IN_SEC, reg_tod_cyc_in_sec, cyc_in_sec);

    RU_REG_WRITE(0, GPON_RX_AMD2, TOD_CYC_IN_SEC, reg_tod_cyc_in_sec);

    return 0;
}

int ag_drv_gpon_rx_amd2_tod_cyc_in_sec_get(uint32_t *cyc_in_sec)
{
    uint32_t reg_tod_cyc_in_sec=0;

#ifdef VALIDATE_PARMS
    if(!cyc_in_sec)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;;
    }
#endif

    RU_REG_READ(0, GPON_RX_AMD2, TOD_CYC_IN_SEC, reg_tod_cyc_in_sec);

    *cyc_in_sec = RU_FIELD_GET(0, GPON_RX_AMD2, TOD_CYC_IN_SEC, CYC_IN_SEC, reg_tod_cyc_in_sec);

    return 0;
}

int ag_drv_gpon_rx_amd2_tod_opps_duty_set(uint32_t duty_cycle, uint8_t opps_en)
{
    uint32_t reg_tod_opps_duty=0;

#ifdef VALIDATE_PARMS
    if((duty_cycle >= _30BITS_MAX_VAL_) ||
       (opps_en >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_tod_opps_duty = RU_FIELD_SET(0, GPON_RX_AMD2, TOD_OPPS_DUTY, DUTY_CYCLE, reg_tod_opps_duty, duty_cycle);
    reg_tod_opps_duty = RU_FIELD_SET(0, GPON_RX_AMD2, TOD_OPPS_DUTY, OPPS_EN, reg_tod_opps_duty, opps_en);

    RU_REG_WRITE(0, GPON_RX_AMD2, TOD_OPPS_DUTY, reg_tod_opps_duty);

    return 0;
}

int ag_drv_gpon_rx_amd2_tod_opps_duty_get(uint32_t *duty_cycle, uint8_t *opps_en)
{
    uint32_t reg_tod_opps_duty=0;

#ifdef VALIDATE_PARMS
    if(!duty_cycle || !opps_en)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;;
    }
#endif

    RU_REG_READ(0, GPON_RX_AMD2, TOD_OPPS_DUTY, reg_tod_opps_duty);

    *duty_cycle = RU_FIELD_GET(0, GPON_RX_AMD2, TOD_OPPS_DUTY, DUTY_CYCLE, reg_tod_opps_duty);
    *opps_en = RU_FIELD_GET(0, GPON_RX_AMD2, TOD_OPPS_DUTY, OPPS_EN, reg_tod_opps_duty);

    return 0;
}

int ag_drv_gpon_rx_amd2_nanos_per_clk_set(uint32_t ns_in_clk)
{
    uint32_t reg_nanos_per_clk=0;

#ifdef VALIDATE_PARMS
    if((ns_in_clk >= _30BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_nanos_per_clk = RU_FIELD_SET(0, GPON_RX_AMD2, NANOS_PER_CLK, NS_IN_CLK, reg_nanos_per_clk, ns_in_clk);

    RU_REG_WRITE(0, GPON_RX_AMD2, NANOS_PER_CLK, reg_nanos_per_clk);

    return 0;
}

int ag_drv_gpon_rx_amd2_nanos_per_clk_get(uint32_t *ns_in_clk)
{
    uint32_t reg_nanos_per_clk=0;

#ifdef VALIDATE_PARMS
    if(!ns_in_clk)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;;
    }
#endif

    RU_REG_READ(0, GPON_RX_AMD2, NANOS_PER_CLK, reg_nanos_per_clk);

    *ns_in_clk = RU_FIELD_GET(0, GPON_RX_AMD2, NANOS_PER_CLK, NS_IN_CLK, reg_nanos_per_clk);

    return 0;
}

int ag_drv_gpon_rx_amd2_nspartinc_set(uint32_t nspinc)
{
    uint32_t reg_nspartinc=0;

#ifdef VALIDATE_PARMS
    if((nspinc >= _30BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_nspartinc = RU_FIELD_SET(0, GPON_RX_AMD2, NSPARTINC, NSPINC, reg_nspartinc, nspinc);

    RU_REG_WRITE(0, GPON_RX_AMD2, NSPARTINC, reg_nspartinc);

    return 0;
}

int ag_drv_gpon_rx_amd2_nspartinc_get(uint32_t *nspinc)
{
    uint32_t reg_nspartinc=0;

#ifdef VALIDATE_PARMS
    if(!nspinc)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;;
    }
#endif

    RU_REG_READ(0, GPON_RX_AMD2, NSPARTINC, reg_nspartinc);

    *nspinc = RU_FIELD_GET(0, GPON_RX_AMD2, NSPARTINC, NSPINC, reg_nspartinc);

    return 0;
}

int ag_drv_gpon_rx_amd2_nsp_in_nano_set(uint32_t nsp_in_nano)
{
    uint32_t reg_nsp_in_nano=0;

#ifdef VALIDATE_PARMS
    if((nsp_in_nano >= _30BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_nsp_in_nano = RU_FIELD_SET(0, GPON_RX_AMD2, NSP_IN_NANO, NSP_IN_NANO, reg_nsp_in_nano, nsp_in_nano);

    RU_REG_WRITE(0, GPON_RX_AMD2, NSP_IN_NANO, reg_nsp_in_nano);

    return 0;
}

int ag_drv_gpon_rx_amd2_nsp_in_nano_get(uint32_t *nsp_in_nano)
{
    uint32_t reg_nsp_in_nano=0;

#ifdef VALIDATE_PARMS
    if(!nsp_in_nano)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;;
    }
#endif

    RU_REG_READ(0, GPON_RX_AMD2, NSP_IN_NANO, reg_nsp_in_nano);

    *nsp_in_nano = RU_FIELD_GET(0, GPON_RX_AMD2, NSP_IN_NANO, NSP_IN_NANO, reg_nsp_in_nano);

    return 0;
}


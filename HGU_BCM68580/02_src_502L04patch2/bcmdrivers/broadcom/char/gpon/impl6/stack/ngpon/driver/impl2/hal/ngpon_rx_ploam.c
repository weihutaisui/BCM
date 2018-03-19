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

#include "ngpon_common.h"
#include "ngpon_rx_ploam.h"
int ag_drv_ngpon_rx_ploam_trfc_set(uint32_t ngpon_rx_onu_id, uint16_t onuid, uint8_t filt_en)
{
    uint32_t reg_trfc=0;

#ifdef VALIDATE_PARMS
    if((ngpon_rx_onu_id >= 2) ||
       (onuid >= _10BITS_MAX_VAL_) ||
       (filt_en >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_trfc = RU_FIELD_SET(0, NGPON_RX_PLOAM, TRFC, ONUID, reg_trfc, onuid);
    reg_trfc = RU_FIELD_SET(0, NGPON_RX_PLOAM, TRFC, FILT_EN, reg_trfc, filt_en);

    RU_REG_RAM_WRITE(0, ngpon_rx_onu_id, NGPON_RX_PLOAM, TRFC, reg_trfc);

    return 0;
}

int ag_drv_ngpon_rx_ploam_trfc_get(uint32_t ngpon_rx_onu_id, uint16_t *onuid, uint8_t *filt_en)
{
    uint32_t reg_trfc=0;

#ifdef VALIDATE_PARMS
    if(!onuid || !filt_en)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((ngpon_rx_onu_id >= 2))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, ngpon_rx_onu_id, NGPON_RX_PLOAM, TRFC, reg_trfc);

    *onuid = RU_FIELD_GET(0, NGPON_RX_PLOAM, TRFC, ONUID, reg_trfc);
    *filt_en = RU_FIELD_GET(0, NGPON_RX_PLOAM, TRFC, FILT_EN, reg_trfc);

    return 0;
}

int ag_drv_ngpon_rx_ploam_bcst_set(uint16_t onuid, uint8_t filt_en)
{
    uint32_t reg_bcst=0;

#ifdef VALIDATE_PARMS
    if((onuid >= _10BITS_MAX_VAL_) ||
       (filt_en >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_bcst = RU_FIELD_SET(0, NGPON_RX_PLOAM, BCST, ONUID, reg_bcst, onuid);
    reg_bcst = RU_FIELD_SET(0, NGPON_RX_PLOAM, BCST, FILT_EN, reg_bcst, filt_en);

    RU_REG_WRITE(0, NGPON_RX_PLOAM, BCST, reg_bcst);

    return 0;
}

int ag_drv_ngpon_rx_ploam_bcst_get(uint16_t *onuid, uint8_t *filt_en)
{
    uint32_t reg_bcst=0;

#ifdef VALIDATE_PARMS
    if(!onuid || !filt_en)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_PLOAM, BCST, reg_bcst);

    *onuid = RU_FIELD_GET(0, NGPON_RX_PLOAM, BCST, ONUID, reg_bcst);
    *filt_en = RU_FIELD_GET(0, NGPON_RX_PLOAM, BCST, FILT_EN, reg_bcst);

    return 0;
}

int ag_drv_ngpon_rx_ploam_gencfg_set(uint8_t ploam_filt_en)
{
    uint32_t reg_gencfg=0;

#ifdef VALIDATE_PARMS
    if((ploam_filt_en >= _1BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_gencfg = RU_FIELD_SET(0, NGPON_RX_PLOAM, GENCFG, PLOAM_FILT_EN, reg_gencfg, ploam_filt_en);

    RU_REG_WRITE(0, NGPON_RX_PLOAM, GENCFG, reg_gencfg);

    return 0;
}

int ag_drv_ngpon_rx_ploam_gencfg_get(uint8_t *ploam_filt_en)
{
    uint32_t reg_gencfg=0;

#ifdef VALIDATE_PARMS
    if(!ploam_filt_en)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, NGPON_RX_PLOAM, GENCFG, reg_gencfg);

    *ploam_filt_en = RU_FIELD_GET(0, NGPON_RX_PLOAM, GENCFG, PLOAM_FILT_EN, reg_gencfg);

    return 0;
}


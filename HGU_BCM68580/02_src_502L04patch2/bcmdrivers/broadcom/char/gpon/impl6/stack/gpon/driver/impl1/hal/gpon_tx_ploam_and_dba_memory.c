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
#include "gpon_tx_ploam_and_dba_memory.h"
int ag_drv_gpon_tx_ploam_and_dba_memory_rplm_set(uint32_t word_idx, uint8_t rplmc)
{
    uint32_t reg_rplm=0;

#ifdef VALIDATE_PARMS
    if((word_idx >= 13))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_rplm = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, RPLM, RPLMC, reg_rplm, rplmc);

    RU_REG_RAM_WRITE(0, word_idx, GPON_TX_PLOAM_AND_DBA_MEMORY, RPLM, reg_rplm);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_rplm_get(uint32_t word_idx, uint8_t *rplmc)
{
    uint32_t reg_rplm=0;

#ifdef VALIDATE_PARMS
    if(!rplmc)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((word_idx >= 13))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, word_idx, GPON_TX_PLOAM_AND_DBA_MEMORY, RPLM, reg_rplm);

    *rplmc = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, RPLM, RPLMC, reg_rplm);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_iplm_set(uint32_t word_idx, uint8_t iplc)
{
    uint32_t reg_iplm=0;

#ifdef VALIDATE_PARMS
    if((word_idx >= 13))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_iplm = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, IPLM, IPLC, reg_iplm, iplc);

    RU_REG_RAM_WRITE(0, word_idx, GPON_TX_PLOAM_AND_DBA_MEMORY, IPLM, reg_iplm);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_iplm_get(uint32_t word_idx, uint8_t *iplc)
{
    uint32_t reg_iplm=0;

#ifdef VALIDATE_PARMS
    if(!iplc)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((word_idx >= 13))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, word_idx, GPON_TX_PLOAM_AND_DBA_MEMORY, IPLM, reg_iplm);

    *iplc = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, IPLM, IPLC, reg_iplm);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_uplm_set(uint32_t word_idx, uint8_t uplmc)
{
    uint32_t reg_uplm=0;

#ifdef VALIDATE_PARMS
    if((word_idx >= 13))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_uplm = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, UPLM, UPLMC, reg_uplm, uplmc);

    RU_REG_RAM_WRITE(0, word_idx, GPON_TX_PLOAM_AND_DBA_MEMORY, UPLM, reg_uplm);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_uplm_get(uint32_t word_idx, uint8_t *uplmc)
{
    uint32_t reg_uplm=0;

#ifdef VALIDATE_PARMS
    if(!uplmc)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((word_idx >= 13))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, word_idx, GPON_TX_PLOAM_AND_DBA_MEMORY, UPLM, reg_uplm);

    *uplmc = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, UPLM, UPLMC, reg_uplm);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_nplm3_set(uint32_t word_idx, uint8_t nplmc)
{
    uint32_t reg_nplm3=0;

#ifdef VALIDATE_PARMS
    if((word_idx >= 13))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_nplm3 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NPLM3, NPLMC, reg_nplm3, nplmc);

    RU_REG_RAM_WRITE(0, word_idx, GPON_TX_PLOAM_AND_DBA_MEMORY, NPLM3, reg_nplm3);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_nplm3_get(uint32_t word_idx, uint8_t *nplmc)
{
    uint32_t reg_nplm3=0;

#ifdef VALIDATE_PARMS
    if(!nplmc)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((word_idx >= 13))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, word_idx, GPON_TX_PLOAM_AND_DBA_MEMORY, NPLM3, reg_nplm3);

    *nplmc = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NPLM3, NPLMC, reg_nplm3);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_nplm2_set(uint32_t word_idx, uint8_t nplmc)
{
    uint32_t reg_nplm2=0;

#ifdef VALIDATE_PARMS
    if((word_idx >= 13))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_nplm2 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NPLM2, NPLMC, reg_nplm2, nplmc);

    RU_REG_RAM_WRITE(0, word_idx, GPON_TX_PLOAM_AND_DBA_MEMORY, NPLM2, reg_nplm2);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_nplm2_get(uint32_t word_idx, uint8_t *nplmc)
{
    uint32_t reg_nplm2=0;

#ifdef VALIDATE_PARMS
    if(!nplmc)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((word_idx >= 13))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, word_idx, GPON_TX_PLOAM_AND_DBA_MEMORY, NPLM2, reg_nplm2);

    *nplmc = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NPLM2, NPLMC, reg_nplm2);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_nplm1_set(uint32_t word_idx, uint8_t nplmc)
{
    uint32_t reg_nplm1=0;

#ifdef VALIDATE_PARMS
    if((word_idx >= 13))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_nplm1 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NPLM1, NPLMC, reg_nplm1, nplmc);

    RU_REG_RAM_WRITE(0, word_idx, GPON_TX_PLOAM_AND_DBA_MEMORY, NPLM1, reg_nplm1);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_nplm1_get(uint32_t word_idx, uint8_t *nplmc)
{
    uint32_t reg_nplm1=0;

#ifdef VALIDATE_PARMS
    if(!nplmc)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((word_idx >= 13))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, word_idx, GPON_TX_PLOAM_AND_DBA_MEMORY, NPLM1, reg_nplm1);

    *nplmc = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NPLM1, NPLMC, reg_nplm1);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_nplm0_set(uint32_t word_idx, uint8_t nplmc)
{
    uint32_t reg_nplm0=0;

#ifdef VALIDATE_PARMS
    if((word_idx >= 13))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_nplm0 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NPLM0, NPLMC, reg_nplm0, nplmc);

    RU_REG_RAM_WRITE(0, word_idx, GPON_TX_PLOAM_AND_DBA_MEMORY, NPLM0, reg_nplm0);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_nplm0_get(uint32_t word_idx, uint8_t *nplmc)
{
    uint32_t reg_nplm0=0;

#ifdef VALIDATE_PARMS
    if(!nplmc)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((word_idx >= 13))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, word_idx, GPON_TX_PLOAM_AND_DBA_MEMORY, NPLM0, reg_nplm0);

    *nplmc = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NPLM0, NPLMC, reg_nplm0);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_idbr_set(uint32_t idbcc)
{
    uint32_t reg_idbr=0;

#ifdef VALIDATE_PARMS
#endif

    reg_idbr = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, IDBR, IDBCC, reg_idbr, idbcc);

    RU_REG_WRITE(0, GPON_TX_PLOAM_AND_DBA_MEMORY, IDBR, reg_idbr);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_idbr_get(uint32_t *idbcc)
{
    uint32_t reg_idbr=0;

#ifdef VALIDATE_PARMS
    if(!idbcc)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_PLOAM_AND_DBA_MEMORY, IDBR, reg_idbr);

    *idbcc = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, IDBR, IDBCC, reg_idbr);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_idbrbyte5_set(uint8_t idbr)
{
    uint32_t reg_idbrbyte5=0;

#ifdef VALIDATE_PARMS
#endif

    reg_idbrbyte5 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, IDBRBYTE5, IDBR, reg_idbrbyte5, idbr);

    RU_REG_WRITE(0, GPON_TX_PLOAM_AND_DBA_MEMORY, IDBRBYTE5, reg_idbrbyte5);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_idbrbyte5_get(uint8_t *idbr)
{
    uint32_t reg_idbrbyte5=0;

#ifdef VALIDATE_PARMS
    if(!idbr)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_PLOAM_AND_DBA_MEMORY, IDBRBYTE5, reg_idbrbyte5);

    *idbr = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, IDBRBYTE5, IDBR, reg_idbrbyte5);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr0_set(uint32_t ndbc)
{
    uint32_t reg_ndbr0=0;

#ifdef VALIDATE_PARMS
#endif

    reg_ndbr0 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR0, NDBC, reg_ndbr0, ndbc);

    RU_REG_WRITE(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR0, reg_ndbr0);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr0_get(uint32_t *ndbc)
{
    uint32_t reg_ndbr0=0;

#ifdef VALIDATE_PARMS
    if(!ndbc)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR0, reg_ndbr0);

    *ndbc = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR0, NDBC, reg_ndbr0);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr0byte5_set(uint8_t ndbcbyte5)
{
    uint32_t reg_ndbr0byte5=0;

#ifdef VALIDATE_PARMS
#endif

    reg_ndbr0byte5 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR0BYTE5, NDBCBYTE5, reg_ndbr0byte5, ndbcbyte5);

    RU_REG_WRITE(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR0BYTE5, reg_ndbr0byte5);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr0byte5_get(uint8_t *ndbcbyte5)
{
    uint32_t reg_ndbr0byte5=0;

#ifdef VALIDATE_PARMS
    if(!ndbcbyte5)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR0BYTE5, reg_ndbr0byte5);

    *ndbcbyte5 = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR0BYTE5, NDBCBYTE5, reg_ndbr0byte5);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr1_set(uint32_t ndbc)
{
    uint32_t reg_ndbr1=0;

#ifdef VALIDATE_PARMS
#endif

    reg_ndbr1 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR1, NDBC, reg_ndbr1, ndbc);

    RU_REG_WRITE(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR1, reg_ndbr1);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr1_get(uint32_t *ndbc)
{
    uint32_t reg_ndbr1=0;

#ifdef VALIDATE_PARMS
    if(!ndbc)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR1, reg_ndbr1);

    *ndbc = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR1, NDBC, reg_ndbr1);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr1byte5_set(uint8_t ndbcbyte5)
{
    uint32_t reg_ndbr1byte5=0;

#ifdef VALIDATE_PARMS
#endif

    reg_ndbr1byte5 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR1BYTE5, NDBCBYTE5, reg_ndbr1byte5, ndbcbyte5);

    RU_REG_WRITE(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR1BYTE5, reg_ndbr1byte5);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr1byte5_get(uint8_t *ndbcbyte5)
{
    uint32_t reg_ndbr1byte5=0;

#ifdef VALIDATE_PARMS
    if(!ndbcbyte5)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR1BYTE5, reg_ndbr1byte5);

    *ndbcbyte5 = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR1BYTE5, NDBCBYTE5, reg_ndbr1byte5);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr2_set(uint32_t ndbc)
{
    uint32_t reg_ndbr2=0;

#ifdef VALIDATE_PARMS
#endif

    reg_ndbr2 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR2, NDBC, reg_ndbr2, ndbc);

    RU_REG_WRITE(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR2, reg_ndbr2);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr2_get(uint32_t *ndbc)
{
    uint32_t reg_ndbr2=0;

#ifdef VALIDATE_PARMS
    if(!ndbc)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR2, reg_ndbr2);

    *ndbc = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR2, NDBC, reg_ndbr2);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr2byte5_set(uint8_t ndbcbyte5)
{
    uint32_t reg_ndbr2byte5=0;

#ifdef VALIDATE_PARMS
#endif

    reg_ndbr2byte5 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR2BYTE5, NDBCBYTE5, reg_ndbr2byte5, ndbcbyte5);

    RU_REG_WRITE(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR2BYTE5, reg_ndbr2byte5);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr2byte5_get(uint8_t *ndbcbyte5)
{
    uint32_t reg_ndbr2byte5=0;

#ifdef VALIDATE_PARMS
    if(!ndbcbyte5)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR2BYTE5, reg_ndbr2byte5);

    *ndbcbyte5 = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR2BYTE5, NDBCBYTE5, reg_ndbr2byte5);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr3_set(uint32_t ndbc)
{
    uint32_t reg_ndbr3=0;

#ifdef VALIDATE_PARMS
#endif

    reg_ndbr3 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR3, NDBC, reg_ndbr3, ndbc);

    RU_REG_WRITE(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR3, reg_ndbr3);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr3_get(uint32_t *ndbc)
{
    uint32_t reg_ndbr3=0;

#ifdef VALIDATE_PARMS
    if(!ndbc)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR3, reg_ndbr3);

    *ndbc = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR3, NDBC, reg_ndbr3);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr3byte5_set(uint8_t ndbcbyte5)
{
    uint32_t reg_ndbr3byte5=0;

#ifdef VALIDATE_PARMS
#endif

    reg_ndbr3byte5 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR3BYTE5, NDBCBYTE5, reg_ndbr3byte5, ndbcbyte5);

    RU_REG_WRITE(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR3BYTE5, reg_ndbr3byte5);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr3byte5_get(uint8_t *ndbcbyte5)
{
    uint32_t reg_ndbr3byte5=0;

#ifdef VALIDATE_PARMS
    if(!ndbcbyte5)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR3BYTE5, reg_ndbr3byte5);

    *ndbcbyte5 = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR3BYTE5, NDBCBYTE5, reg_ndbr3byte5);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr4_set(uint32_t ndbc)
{
    uint32_t reg_ndbr4=0;

#ifdef VALIDATE_PARMS
#endif

    reg_ndbr4 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR4, NDBC, reg_ndbr4, ndbc);

    RU_REG_WRITE(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR4, reg_ndbr4);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr4_get(uint32_t *ndbc)
{
    uint32_t reg_ndbr4=0;

#ifdef VALIDATE_PARMS
    if(!ndbc)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR4, reg_ndbr4);

    *ndbc = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR4, NDBC, reg_ndbr4);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr4byte5_set(uint8_t ndbcbyte5)
{
    uint32_t reg_ndbr4byte5=0;

#ifdef VALIDATE_PARMS
#endif

    reg_ndbr4byte5 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR4BYTE5, NDBCBYTE5, reg_ndbr4byte5, ndbcbyte5);

    RU_REG_WRITE(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR4BYTE5, reg_ndbr4byte5);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr4byte5_get(uint8_t *ndbcbyte5)
{
    uint32_t reg_ndbr4byte5=0;

#ifdef VALIDATE_PARMS
    if(!ndbcbyte5)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR4BYTE5, reg_ndbr4byte5);

    *ndbcbyte5 = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR4BYTE5, NDBCBYTE5, reg_ndbr4byte5);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr5_set(uint32_t ndbc)
{
    uint32_t reg_ndbr5=0;

#ifdef VALIDATE_PARMS
#endif

    reg_ndbr5 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR5, NDBC, reg_ndbr5, ndbc);

    RU_REG_WRITE(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR5, reg_ndbr5);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr5_get(uint32_t *ndbc)
{
    uint32_t reg_ndbr5=0;

#ifdef VALIDATE_PARMS
    if(!ndbc)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR5, reg_ndbr5);

    *ndbc = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR5, NDBC, reg_ndbr5);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr5byte5_set(uint8_t ndbcbyte5)
{
    uint32_t reg_ndbr5byte5=0;

#ifdef VALIDATE_PARMS
#endif

    reg_ndbr5byte5 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR5BYTE5, NDBCBYTE5, reg_ndbr5byte5, ndbcbyte5);

    RU_REG_WRITE(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR5BYTE5, reg_ndbr5byte5);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr5byte5_get(uint8_t *ndbcbyte5)
{
    uint32_t reg_ndbr5byte5=0;

#ifdef VALIDATE_PARMS
    if(!ndbcbyte5)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR5BYTE5, reg_ndbr5byte5);

    *ndbcbyte5 = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR5BYTE5, NDBCBYTE5, reg_ndbr5byte5);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr6_set(uint32_t ndbc)
{
    uint32_t reg_ndbr6=0;

#ifdef VALIDATE_PARMS
#endif

    reg_ndbr6 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR6, NDBC, reg_ndbr6, ndbc);

    RU_REG_WRITE(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR6, reg_ndbr6);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr6_get(uint32_t *ndbc)
{
    uint32_t reg_ndbr6=0;

#ifdef VALIDATE_PARMS
    if(!ndbc)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR6, reg_ndbr6);

    *ndbc = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR6, NDBC, reg_ndbr6);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr6byte5_set(uint8_t ndbcbyte5)
{
    uint32_t reg_ndbr6byte5=0;

#ifdef VALIDATE_PARMS
#endif

    reg_ndbr6byte5 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR6BYTE5, NDBCBYTE5, reg_ndbr6byte5, ndbcbyte5);

    RU_REG_WRITE(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR6BYTE5, reg_ndbr6byte5);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr6byte5_get(uint8_t *ndbcbyte5)
{
    uint32_t reg_ndbr6byte5=0;

#ifdef VALIDATE_PARMS
    if(!ndbcbyte5)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR6BYTE5, reg_ndbr6byte5);

    *ndbcbyte5 = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR6BYTE5, NDBCBYTE5, reg_ndbr6byte5);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr7_set(uint32_t ndbc)
{
    uint32_t reg_ndbr7=0;

#ifdef VALIDATE_PARMS
#endif

    reg_ndbr7 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR7, NDBC, reg_ndbr7, ndbc);

    RU_REG_WRITE(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR7, reg_ndbr7);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr7_get(uint32_t *ndbc)
{
    uint32_t reg_ndbr7=0;

#ifdef VALIDATE_PARMS
    if(!ndbc)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR7, reg_ndbr7);

    *ndbc = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR7, NDBC, reg_ndbr7);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr7byte5_set(uint8_t ndbcbyte5)
{
    uint32_t reg_ndbr7byte5=0;

#ifdef VALIDATE_PARMS
#endif

    reg_ndbr7byte5 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR7BYTE5, NDBCBYTE5, reg_ndbr7byte5, ndbcbyte5);

    RU_REG_WRITE(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR7BYTE5, reg_ndbr7byte5);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr7byte5_get(uint8_t *ndbcbyte5)
{
    uint32_t reg_ndbr7byte5=0;

#ifdef VALIDATE_PARMS
    if(!ndbcbyte5)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR7BYTE5, reg_ndbr7byte5);

    *ndbcbyte5 = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR7BYTE5, NDBCBYTE5, reg_ndbr7byte5);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr8to39_set(uint32_t dbr_idx, uint32_t ndbc)
{
    uint32_t reg_ndbr8to39=0;

#ifdef VALIDATE_PARMS
    if((dbr_idx >= 32) ||
       (ndbc >= _24BITS_MAX_VAL_))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    reg_ndbr8to39 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR8TO39, NDBC, reg_ndbr8to39, ndbc);

    RU_REG_RAM_WRITE(0, dbr_idx, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR8TO39, reg_ndbr8to39);

    return 0;
}

int ag_drv_gpon_tx_ploam_and_dba_memory_ndbr8to39_get(uint32_t dbr_idx, uint32_t *ndbc)
{
    uint32_t reg_ndbr8to39=0;

#ifdef VALIDATE_PARMS
    if(!ndbc)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((dbr_idx >= 32))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, dbr_idx, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR8TO39, reg_ndbr8to39);

    *ndbc = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR8TO39, NDBC, reg_ndbr8to39);

    return 0;
}


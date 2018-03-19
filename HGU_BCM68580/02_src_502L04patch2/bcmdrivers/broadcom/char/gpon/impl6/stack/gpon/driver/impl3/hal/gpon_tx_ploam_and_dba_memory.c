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
#include "gpon_tx_ploam_and_dba_memory.h"
bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_rplm_set(uint32_t word_idx, uint8_t rplmc)
{
    uint32_t reg_rplm=0;

#ifdef VALIDATE_PARMS
    if((word_idx >= 13))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_rplm = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, RPLM, RPLMC, reg_rplm, rplmc);

    RU_REG_RAM_WRITE(0, word_idx, GPON_TX_PLOAM_AND_DBA_MEMORY, RPLM, reg_rplm);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_rplm_get(uint32_t word_idx, uint8_t *rplmc)
{
    uint32_t reg_rplm=0;

#ifdef VALIDATE_PARMS
    if(!rplmc)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
    if((word_idx >= 13))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, word_idx, GPON_TX_PLOAM_AND_DBA_MEMORY, RPLM, reg_rplm);

    *rplmc = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, RPLM, RPLMC, reg_rplm);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_iplm_set(uint32_t word_idx, uint8_t iplc)
{
    uint32_t reg_iplm=0;

#ifdef VALIDATE_PARMS
    if((word_idx >= 13))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_iplm = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, IPLM, IPLC, reg_iplm, iplc);

    RU_REG_RAM_WRITE(0, word_idx, GPON_TX_PLOAM_AND_DBA_MEMORY, IPLM, reg_iplm);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_iplm_get(uint32_t word_idx, uint8_t *iplc)
{
    uint32_t reg_iplm=0;

#ifdef VALIDATE_PARMS
    if(!iplc)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
    if((word_idx >= 13))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, word_idx, GPON_TX_PLOAM_AND_DBA_MEMORY, IPLM, reg_iplm);

    *iplc = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, IPLM, IPLC, reg_iplm);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_uplm_set(uint32_t word_idx, uint8_t uplmc)
{
    uint32_t reg_uplm=0;

#ifdef VALIDATE_PARMS
    if((word_idx >= 13))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_uplm = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, UPLM, UPLMC, reg_uplm, uplmc);

    RU_REG_RAM_WRITE(0, word_idx, GPON_TX_PLOAM_AND_DBA_MEMORY, UPLM, reg_uplm);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_uplm_get(uint32_t word_idx, uint8_t *uplmc)
{
    uint32_t reg_uplm=0;

#ifdef VALIDATE_PARMS
    if(!uplmc)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
    if((word_idx >= 13))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, word_idx, GPON_TX_PLOAM_AND_DBA_MEMORY, UPLM, reg_uplm);

    *uplmc = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, UPLM, UPLMC, reg_uplm);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_nplm3_set(uint32_t word_idx, uint8_t nplmc)
{
    uint32_t reg_nplm3=0;

#ifdef VALIDATE_PARMS
    if((word_idx >= 13))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_nplm3 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NPLM3, NPLMC, reg_nplm3, nplmc);

    RU_REG_RAM_WRITE(0, word_idx, GPON_TX_PLOAM_AND_DBA_MEMORY, NPLM3, reg_nplm3);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_nplm3_get(uint32_t word_idx, uint8_t *nplmc)
{
    uint32_t reg_nplm3=0;

#ifdef VALIDATE_PARMS
    if(!nplmc)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
    if((word_idx >= 13))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, word_idx, GPON_TX_PLOAM_AND_DBA_MEMORY, NPLM3, reg_nplm3);

    *nplmc = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NPLM3, NPLMC, reg_nplm3);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_nplm2_set(uint32_t word_idx, uint8_t nplmc)
{
    uint32_t reg_nplm2=0;

#ifdef VALIDATE_PARMS
    if((word_idx >= 13))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_nplm2 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NPLM2, NPLMC, reg_nplm2, nplmc);

    RU_REG_RAM_WRITE(0, word_idx, GPON_TX_PLOAM_AND_DBA_MEMORY, NPLM2, reg_nplm2);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_nplm2_get(uint32_t word_idx, uint8_t *nplmc)
{
    uint32_t reg_nplm2=0;

#ifdef VALIDATE_PARMS
    if(!nplmc)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
    if((word_idx >= 13))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, word_idx, GPON_TX_PLOAM_AND_DBA_MEMORY, NPLM2, reg_nplm2);

    *nplmc = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NPLM2, NPLMC, reg_nplm2);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_nplm1_set(uint32_t word_idx, uint8_t nplmc)
{
    uint32_t reg_nplm1=0;

#ifdef VALIDATE_PARMS
    if((word_idx >= 13))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_nplm1 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NPLM1, NPLMC, reg_nplm1, nplmc);

    RU_REG_RAM_WRITE(0, word_idx, GPON_TX_PLOAM_AND_DBA_MEMORY, NPLM1, reg_nplm1);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_nplm1_get(uint32_t word_idx, uint8_t *nplmc)
{
    uint32_t reg_nplm1=0;

#ifdef VALIDATE_PARMS
    if(!nplmc)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
    if((word_idx >= 13))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, word_idx, GPON_TX_PLOAM_AND_DBA_MEMORY, NPLM1, reg_nplm1);

    *nplmc = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NPLM1, NPLMC, reg_nplm1);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_nplm0_set(uint32_t word_idx, uint8_t nplmc)
{
    uint32_t reg_nplm0=0;

#ifdef VALIDATE_PARMS
    if((word_idx >= 13))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_nplm0 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NPLM0, NPLMC, reg_nplm0, nplmc);

    RU_REG_RAM_WRITE(0, word_idx, GPON_TX_PLOAM_AND_DBA_MEMORY, NPLM0, reg_nplm0);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_nplm0_get(uint32_t word_idx, uint8_t *nplmc)
{
    uint32_t reg_nplm0=0;

#ifdef VALIDATE_PARMS
    if(!nplmc)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
    if((word_idx >= 13))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, word_idx, GPON_TX_PLOAM_AND_DBA_MEMORY, NPLM0, reg_nplm0);

    *nplmc = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NPLM0, NPLMC, reg_nplm0);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_idbr_set(uint32_t idbcc)
{
    uint32_t reg_idbr=0;

#ifdef VALIDATE_PARMS
#endif

    reg_idbr = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, IDBR, IDBCC, reg_idbr, idbcc);

    RU_REG_WRITE(0, GPON_TX_PLOAM_AND_DBA_MEMORY, IDBR, reg_idbr);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_idbr_get(uint32_t *idbcc)
{
    uint32_t reg_idbr=0;

#ifdef VALIDATE_PARMS
    if(!idbcc)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_PLOAM_AND_DBA_MEMORY, IDBR, reg_idbr);

    *idbcc = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, IDBR, IDBCC, reg_idbr);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_idbrbyte5_set(uint8_t idbr)
{
    uint32_t reg_idbrbyte5=0;

#ifdef VALIDATE_PARMS
#endif

    reg_idbrbyte5 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, IDBRBYTE5, IDBR, reg_idbrbyte5, idbr);

    RU_REG_WRITE(0, GPON_TX_PLOAM_AND_DBA_MEMORY, IDBRBYTE5, reg_idbrbyte5);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_idbrbyte5_get(uint8_t *idbr)
{
    uint32_t reg_idbrbyte5=0;

#ifdef VALIDATE_PARMS
    if(!idbr)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_PLOAM_AND_DBA_MEMORY, IDBRBYTE5, reg_idbrbyte5);

    *idbr = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, IDBRBYTE5, IDBR, reg_idbrbyte5);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr0_set(uint32_t ndbc)
{
    uint32_t reg_ndbr0=0;

#ifdef VALIDATE_PARMS
#endif

    reg_ndbr0 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR0, NDBC, reg_ndbr0, ndbc);

    RU_REG_WRITE(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR0, reg_ndbr0);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr0_get(uint32_t *ndbc)
{
    uint32_t reg_ndbr0=0;

#ifdef VALIDATE_PARMS
    if(!ndbc)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR0, reg_ndbr0);

    *ndbc = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR0, NDBC, reg_ndbr0);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr0byte5_set(uint8_t ndbcbyte5)
{
    uint32_t reg_ndbr0byte5=0;

#ifdef VALIDATE_PARMS
#endif

    reg_ndbr0byte5 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR0BYTE5, NDBCBYTE5, reg_ndbr0byte5, ndbcbyte5);

    RU_REG_WRITE(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR0BYTE5, reg_ndbr0byte5);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr0byte5_get(uint8_t *ndbcbyte5)
{
    uint32_t reg_ndbr0byte5=0;

#ifdef VALIDATE_PARMS
    if(!ndbcbyte5)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR0BYTE5, reg_ndbr0byte5);

    *ndbcbyte5 = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR0BYTE5, NDBCBYTE5, reg_ndbr0byte5);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr1_set(uint32_t ndbc)
{
    uint32_t reg_ndbr1=0;

#ifdef VALIDATE_PARMS
#endif

    reg_ndbr1 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR1, NDBC, reg_ndbr1, ndbc);

    RU_REG_WRITE(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR1, reg_ndbr1);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr1_get(uint32_t *ndbc)
{
    uint32_t reg_ndbr1=0;

#ifdef VALIDATE_PARMS
    if(!ndbc)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR1, reg_ndbr1);

    *ndbc = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR1, NDBC, reg_ndbr1);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr1byte5_set(uint8_t ndbcbyte5)
{
    uint32_t reg_ndbr1byte5=0;

#ifdef VALIDATE_PARMS
#endif

    reg_ndbr1byte5 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR1BYTE5, NDBCBYTE5, reg_ndbr1byte5, ndbcbyte5);

    RU_REG_WRITE(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR1BYTE5, reg_ndbr1byte5);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr1byte5_get(uint8_t *ndbcbyte5)
{
    uint32_t reg_ndbr1byte5=0;

#ifdef VALIDATE_PARMS
    if(!ndbcbyte5)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR1BYTE5, reg_ndbr1byte5);

    *ndbcbyte5 = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR1BYTE5, NDBCBYTE5, reg_ndbr1byte5);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr2_set(uint32_t ndbc)
{
    uint32_t reg_ndbr2=0;

#ifdef VALIDATE_PARMS
#endif

    reg_ndbr2 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR2, NDBC, reg_ndbr2, ndbc);

    RU_REG_WRITE(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR2, reg_ndbr2);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr2_get(uint32_t *ndbc)
{
    uint32_t reg_ndbr2=0;

#ifdef VALIDATE_PARMS
    if(!ndbc)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR2, reg_ndbr2);

    *ndbc = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR2, NDBC, reg_ndbr2);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr2byte5_set(uint8_t ndbcbyte5)
{
    uint32_t reg_ndbr2byte5=0;

#ifdef VALIDATE_PARMS
#endif

    reg_ndbr2byte5 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR2BYTE5, NDBCBYTE5, reg_ndbr2byte5, ndbcbyte5);

    RU_REG_WRITE(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR2BYTE5, reg_ndbr2byte5);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr2byte5_get(uint8_t *ndbcbyte5)
{
    uint32_t reg_ndbr2byte5=0;

#ifdef VALIDATE_PARMS
    if(!ndbcbyte5)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR2BYTE5, reg_ndbr2byte5);

    *ndbcbyte5 = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR2BYTE5, NDBCBYTE5, reg_ndbr2byte5);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr3_set(uint32_t ndbc)
{
    uint32_t reg_ndbr3=0;

#ifdef VALIDATE_PARMS
#endif

    reg_ndbr3 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR3, NDBC, reg_ndbr3, ndbc);

    RU_REG_WRITE(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR3, reg_ndbr3);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr3_get(uint32_t *ndbc)
{
    uint32_t reg_ndbr3=0;

#ifdef VALIDATE_PARMS
    if(!ndbc)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR3, reg_ndbr3);

    *ndbc = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR3, NDBC, reg_ndbr3);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr3byte5_set(uint8_t ndbcbyte5)
{
    uint32_t reg_ndbr3byte5=0;

#ifdef VALIDATE_PARMS
#endif

    reg_ndbr3byte5 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR3BYTE5, NDBCBYTE5, reg_ndbr3byte5, ndbcbyte5);

    RU_REG_WRITE(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR3BYTE5, reg_ndbr3byte5);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr3byte5_get(uint8_t *ndbcbyte5)
{
    uint32_t reg_ndbr3byte5=0;

#ifdef VALIDATE_PARMS
    if(!ndbcbyte5)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR3BYTE5, reg_ndbr3byte5);

    *ndbcbyte5 = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR3BYTE5, NDBCBYTE5, reg_ndbr3byte5);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr4_set(uint32_t ndbc)
{
    uint32_t reg_ndbr4=0;

#ifdef VALIDATE_PARMS
#endif

    reg_ndbr4 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR4, NDBC, reg_ndbr4, ndbc);

    RU_REG_WRITE(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR4, reg_ndbr4);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr4_get(uint32_t *ndbc)
{
    uint32_t reg_ndbr4=0;

#ifdef VALIDATE_PARMS
    if(!ndbc)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR4, reg_ndbr4);

    *ndbc = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR4, NDBC, reg_ndbr4);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr4byte5_set(uint8_t ndbcbyte5)
{
    uint32_t reg_ndbr4byte5=0;

#ifdef VALIDATE_PARMS
#endif

    reg_ndbr4byte5 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR4BYTE5, NDBCBYTE5, reg_ndbr4byte5, ndbcbyte5);

    RU_REG_WRITE(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR4BYTE5, reg_ndbr4byte5);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr4byte5_get(uint8_t *ndbcbyte5)
{
    uint32_t reg_ndbr4byte5=0;

#ifdef VALIDATE_PARMS
    if(!ndbcbyte5)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR4BYTE5, reg_ndbr4byte5);

    *ndbcbyte5 = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR4BYTE5, NDBCBYTE5, reg_ndbr4byte5);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr5_set(uint32_t ndbc)
{
    uint32_t reg_ndbr5=0;

#ifdef VALIDATE_PARMS
#endif

    reg_ndbr5 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR5, NDBC, reg_ndbr5, ndbc);

    RU_REG_WRITE(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR5, reg_ndbr5);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr5_get(uint32_t *ndbc)
{
    uint32_t reg_ndbr5=0;

#ifdef VALIDATE_PARMS
    if(!ndbc)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR5, reg_ndbr5);

    *ndbc = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR5, NDBC, reg_ndbr5);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr5byte5_set(uint8_t ndbcbyte5)
{
    uint32_t reg_ndbr5byte5=0;

#ifdef VALIDATE_PARMS
#endif

    reg_ndbr5byte5 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR5BYTE5, NDBCBYTE5, reg_ndbr5byte5, ndbcbyte5);

    RU_REG_WRITE(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR5BYTE5, reg_ndbr5byte5);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr5byte5_get(uint8_t *ndbcbyte5)
{
    uint32_t reg_ndbr5byte5=0;

#ifdef VALIDATE_PARMS
    if(!ndbcbyte5)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR5BYTE5, reg_ndbr5byte5);

    *ndbcbyte5 = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR5BYTE5, NDBCBYTE5, reg_ndbr5byte5);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr6_set(uint32_t ndbc)
{
    uint32_t reg_ndbr6=0;

#ifdef VALIDATE_PARMS
#endif

    reg_ndbr6 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR6, NDBC, reg_ndbr6, ndbc);

    RU_REG_WRITE(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR6, reg_ndbr6);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr6_get(uint32_t *ndbc)
{
    uint32_t reg_ndbr6=0;

#ifdef VALIDATE_PARMS
    if(!ndbc)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR6, reg_ndbr6);

    *ndbc = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR6, NDBC, reg_ndbr6);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr6byte5_set(uint8_t ndbcbyte5)
{
    uint32_t reg_ndbr6byte5=0;

#ifdef VALIDATE_PARMS
#endif

    reg_ndbr6byte5 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR6BYTE5, NDBCBYTE5, reg_ndbr6byte5, ndbcbyte5);

    RU_REG_WRITE(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR6BYTE5, reg_ndbr6byte5);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr6byte5_get(uint8_t *ndbcbyte5)
{
    uint32_t reg_ndbr6byte5=0;

#ifdef VALIDATE_PARMS
    if(!ndbcbyte5)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR6BYTE5, reg_ndbr6byte5);

    *ndbcbyte5 = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR6BYTE5, NDBCBYTE5, reg_ndbr6byte5);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr7_set(uint32_t ndbc)
{
    uint32_t reg_ndbr7=0;

#ifdef VALIDATE_PARMS
#endif

    reg_ndbr7 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR7, NDBC, reg_ndbr7, ndbc);

    RU_REG_WRITE(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR7, reg_ndbr7);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr7_get(uint32_t *ndbc)
{
    uint32_t reg_ndbr7=0;

#ifdef VALIDATE_PARMS
    if(!ndbc)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR7, reg_ndbr7);

    *ndbc = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR7, NDBC, reg_ndbr7);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr7byte5_set(uint8_t ndbcbyte5)
{
    uint32_t reg_ndbr7byte5=0;

#ifdef VALIDATE_PARMS
#endif

    reg_ndbr7byte5 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR7BYTE5, NDBCBYTE5, reg_ndbr7byte5, ndbcbyte5);

    RU_REG_WRITE(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR7BYTE5, reg_ndbr7byte5);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr7byte5_get(uint8_t *ndbcbyte5)
{
    uint32_t reg_ndbr7byte5=0;

#ifdef VALIDATE_PARMS
    if(!ndbcbyte5)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR7BYTE5, reg_ndbr7byte5);

    *ndbcbyte5 = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR7BYTE5, NDBCBYTE5, reg_ndbr7byte5);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr8to39_set(uint32_t dbr_idx, uint32_t ndbc)
{
    uint32_t reg_ndbr8to39=0;

#ifdef VALIDATE_PARMS
    if((dbr_idx >= 32) ||
       (ndbc >= _24BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_ndbr8to39 = RU_FIELD_SET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR8TO39, NDBC, reg_ndbr8to39, ndbc);

    RU_REG_RAM_WRITE(0, dbr_idx, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR8TO39, reg_ndbr8to39);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_gpon_tx_ploam_and_dba_memory_ndbr8to39_get(uint32_t dbr_idx, uint32_t *ndbc)
{
    uint32_t reg_ndbr8to39=0;

#ifdef VALIDATE_PARMS
    if(!ndbc)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
    if((dbr_idx >= 32))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, dbr_idx, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR8TO39, reg_ndbr8to39);

    *ndbc = RU_FIELD_GET(0, GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR8TO39, NDBC, reg_ndbr8to39);

    return BDMF_ERR_OK;
}

#ifdef USE_BDMF_SHELL
enum
{
    BDMF_rplm,
    BDMF_iplm,
    BDMF_uplm,
    BDMF_nplm3,
    BDMF_nplm2,
    BDMF_nplm1,
    BDMF_nplm0,
    BDMF_idbr,
    BDMF_idbrbyte5,
    BDMF_ndbr0,
    BDMF_ndbr0byte5,
    BDMF_ndbr1,
    BDMF_ndbr1byte5,
    BDMF_ndbr2,
    BDMF_ndbr2byte5,
    BDMF_ndbr3,
    BDMF_ndbr3byte5,
    BDMF_ndbr4,
    BDMF_ndbr4byte5,
    BDMF_ndbr5,
    BDMF_ndbr5byte5,
    BDMF_ndbr6,
    BDMF_ndbr6byte5,
    BDMF_ndbr7,
    BDMF_ndbr7byte5,
    BDMF_ndbr8to39,
};

typedef enum
{
    bdmf_address_rplm,
    bdmf_address_iplm,
    bdmf_address_uplm,
    bdmf_address_nplm3,
    bdmf_address_nplm2,
    bdmf_address_nplm1,
    bdmf_address_nplm0,
    bdmf_address_idbr,
    bdmf_address_idbrbyte5,
    bdmf_address_ndbr0,
    bdmf_address_ndbr0byte5,
    bdmf_address_ndbr1,
    bdmf_address_ndbr1byte5,
    bdmf_address_ndbr2,
    bdmf_address_ndbr2byte5,
    bdmf_address_ndbr3,
    bdmf_address_ndbr3byte5,
    bdmf_address_ndbr4,
    bdmf_address_ndbr4byte5,
    bdmf_address_ndbr5,
    bdmf_address_ndbr5byte5,
    bdmf_address_ndbr6,
    bdmf_address_ndbr6byte5,
    bdmf_address_ndbr7,
    bdmf_address_ndbr7byte5,
    bdmf_address_ndbr8to39,
}
bdmf_address;

static int bcm_gpon_tx_ploam_and_dba_memory_cli_set(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_error_t err;

    switch(parm[0].value.unumber)
    {
    case BDMF_rplm:
        err = ag_drv_gpon_tx_ploam_and_dba_memory_rplm_set(parm[1].value.unumber, parm[2].value.unumber);
        break;
    case BDMF_iplm:
        err = ag_drv_gpon_tx_ploam_and_dba_memory_iplm_set(parm[1].value.unumber, parm[2].value.unumber);
        break;
    case BDMF_uplm:
        err = ag_drv_gpon_tx_ploam_and_dba_memory_uplm_set(parm[1].value.unumber, parm[2].value.unumber);
        break;
    case BDMF_nplm3:
        err = ag_drv_gpon_tx_ploam_and_dba_memory_nplm3_set(parm[1].value.unumber, parm[2].value.unumber);
        break;
    case BDMF_nplm2:
        err = ag_drv_gpon_tx_ploam_and_dba_memory_nplm2_set(parm[1].value.unumber, parm[2].value.unumber);
        break;
    case BDMF_nplm1:
        err = ag_drv_gpon_tx_ploam_and_dba_memory_nplm1_set(parm[1].value.unumber, parm[2].value.unumber);
        break;
    case BDMF_nplm0:
        err = ag_drv_gpon_tx_ploam_and_dba_memory_nplm0_set(parm[1].value.unumber, parm[2].value.unumber);
        break;
    case BDMF_idbr:
        err = ag_drv_gpon_tx_ploam_and_dba_memory_idbr_set(parm[1].value.unumber);
        break;
    case BDMF_idbrbyte5:
        err = ag_drv_gpon_tx_ploam_and_dba_memory_idbrbyte5_set(parm[1].value.unumber);
        break;
    case BDMF_ndbr0:
        err = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr0_set(parm[1].value.unumber);
        break;
    case BDMF_ndbr0byte5:
        err = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr0byte5_set(parm[1].value.unumber);
        break;
    case BDMF_ndbr1:
        err = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr1_set(parm[1].value.unumber);
        break;
    case BDMF_ndbr1byte5:
        err = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr1byte5_set(parm[1].value.unumber);
        break;
    case BDMF_ndbr2:
        err = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr2_set(parm[1].value.unumber);
        break;
    case BDMF_ndbr2byte5:
        err = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr2byte5_set(parm[1].value.unumber);
        break;
    case BDMF_ndbr3:
        err = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr3_set(parm[1].value.unumber);
        break;
    case BDMF_ndbr3byte5:
        err = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr3byte5_set(parm[1].value.unumber);
        break;
    case BDMF_ndbr4:
        err = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr4_set(parm[1].value.unumber);
        break;
    case BDMF_ndbr4byte5:
        err = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr4byte5_set(parm[1].value.unumber);
        break;
    case BDMF_ndbr5:
        err = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr5_set(parm[1].value.unumber);
        break;
    case BDMF_ndbr5byte5:
        err = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr5byte5_set(parm[1].value.unumber);
        break;
    case BDMF_ndbr6:
        err = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr6_set(parm[1].value.unumber);
        break;
    case BDMF_ndbr6byte5:
        err = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr6byte5_set(parm[1].value.unumber);
        break;
    case BDMF_ndbr7:
        err = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr7_set(parm[1].value.unumber);
        break;
    case BDMF_ndbr7byte5:
        err = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr7byte5_set(parm[1].value.unumber);
        break;
    case BDMF_ndbr8to39:
        err = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr8to39_set(parm[1].value.unumber, parm[2].value.unumber);
        break;
    default:
        err = BDMF_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

static int bcm_gpon_tx_ploam_and_dba_memory_cli_get(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_error_t err;

    switch(parm[0].value.unumber)
    {
    case BDMF_rplm:
    {
        uint8_t rplmc;
        err = ag_drv_gpon_tx_ploam_and_dba_memory_rplm_get(parm[1].value.unumber, &rplmc);
        bdmf_session_print(session, "rplmc = %u = 0x%x\n", rplmc, rplmc);
        break;
    }
    case BDMF_iplm:
    {
        uint8_t iplc;
        err = ag_drv_gpon_tx_ploam_and_dba_memory_iplm_get(parm[1].value.unumber, &iplc);
        bdmf_session_print(session, "iplc = %u = 0x%x\n", iplc, iplc);
        break;
    }
    case BDMF_uplm:
    {
        uint8_t uplmc;
        err = ag_drv_gpon_tx_ploam_and_dba_memory_uplm_get(parm[1].value.unumber, &uplmc);
        bdmf_session_print(session, "uplmc = %u = 0x%x\n", uplmc, uplmc);
        break;
    }
    case BDMF_nplm3:
    {
        uint8_t nplmc;
        err = ag_drv_gpon_tx_ploam_and_dba_memory_nplm3_get(parm[1].value.unumber, &nplmc);
        bdmf_session_print(session, "nplmc = %u = 0x%x\n", nplmc, nplmc);
        break;
    }
    case BDMF_nplm2:
    {
        uint8_t nplmc;
        err = ag_drv_gpon_tx_ploam_and_dba_memory_nplm2_get(parm[1].value.unumber, &nplmc);
        bdmf_session_print(session, "nplmc = %u = 0x%x\n", nplmc, nplmc);
        break;
    }
    case BDMF_nplm1:
    {
        uint8_t nplmc;
        err = ag_drv_gpon_tx_ploam_and_dba_memory_nplm1_get(parm[1].value.unumber, &nplmc);
        bdmf_session_print(session, "nplmc = %u = 0x%x\n", nplmc, nplmc);
        break;
    }
    case BDMF_nplm0:
    {
        uint8_t nplmc;
        err = ag_drv_gpon_tx_ploam_and_dba_memory_nplm0_get(parm[1].value.unumber, &nplmc);
        bdmf_session_print(session, "nplmc = %u = 0x%x\n", nplmc, nplmc);
        break;
    }
    case BDMF_idbr:
    {
        uint32_t idbcc;
        err = ag_drv_gpon_tx_ploam_and_dba_memory_idbr_get(&idbcc);
        bdmf_session_print(session, "idbcc = %u = 0x%x\n", idbcc, idbcc);
        break;
    }
    case BDMF_idbrbyte5:
    {
        uint8_t idbr;
        err = ag_drv_gpon_tx_ploam_and_dba_memory_idbrbyte5_get(&idbr);
        bdmf_session_print(session, "idbr = %u = 0x%x\n", idbr, idbr);
        break;
    }
    case BDMF_ndbr0:
    {
        uint32_t ndbc;
        err = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr0_get(&ndbc);
        bdmf_session_print(session, "ndbc = %u = 0x%x\n", ndbc, ndbc);
        break;
    }
    case BDMF_ndbr0byte5:
    {
        uint8_t ndbcbyte5;
        err = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr0byte5_get(&ndbcbyte5);
        bdmf_session_print(session, "ndbcbyte5 = %u = 0x%x\n", ndbcbyte5, ndbcbyte5);
        break;
    }
    case BDMF_ndbr1:
    {
        uint32_t ndbc;
        err = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr1_get(&ndbc);
        bdmf_session_print(session, "ndbc = %u = 0x%x\n", ndbc, ndbc);
        break;
    }
    case BDMF_ndbr1byte5:
    {
        uint8_t ndbcbyte5;
        err = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr1byte5_get(&ndbcbyte5);
        bdmf_session_print(session, "ndbcbyte5 = %u = 0x%x\n", ndbcbyte5, ndbcbyte5);
        break;
    }
    case BDMF_ndbr2:
    {
        uint32_t ndbc;
        err = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr2_get(&ndbc);
        bdmf_session_print(session, "ndbc = %u = 0x%x\n", ndbc, ndbc);
        break;
    }
    case BDMF_ndbr2byte5:
    {
        uint8_t ndbcbyte5;
        err = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr2byte5_get(&ndbcbyte5);
        bdmf_session_print(session, "ndbcbyte5 = %u = 0x%x\n", ndbcbyte5, ndbcbyte5);
        break;
    }
    case BDMF_ndbr3:
    {
        uint32_t ndbc;
        err = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr3_get(&ndbc);
        bdmf_session_print(session, "ndbc = %u = 0x%x\n", ndbc, ndbc);
        break;
    }
    case BDMF_ndbr3byte5:
    {
        uint8_t ndbcbyte5;
        err = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr3byte5_get(&ndbcbyte5);
        bdmf_session_print(session, "ndbcbyte5 = %u = 0x%x\n", ndbcbyte5, ndbcbyte5);
        break;
    }
    case BDMF_ndbr4:
    {
        uint32_t ndbc;
        err = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr4_get(&ndbc);
        bdmf_session_print(session, "ndbc = %u = 0x%x\n", ndbc, ndbc);
        break;
    }
    case BDMF_ndbr4byte5:
    {
        uint8_t ndbcbyte5;
        err = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr4byte5_get(&ndbcbyte5);
        bdmf_session_print(session, "ndbcbyte5 = %u = 0x%x\n", ndbcbyte5, ndbcbyte5);
        break;
    }
    case BDMF_ndbr5:
    {
        uint32_t ndbc;
        err = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr5_get(&ndbc);
        bdmf_session_print(session, "ndbc = %u = 0x%x\n", ndbc, ndbc);
        break;
    }
    case BDMF_ndbr5byte5:
    {
        uint8_t ndbcbyte5;
        err = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr5byte5_get(&ndbcbyte5);
        bdmf_session_print(session, "ndbcbyte5 = %u = 0x%x\n", ndbcbyte5, ndbcbyte5);
        break;
    }
    case BDMF_ndbr6:
    {
        uint32_t ndbc;
        err = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr6_get(&ndbc);
        bdmf_session_print(session, "ndbc = %u = 0x%x\n", ndbc, ndbc);
        break;
    }
    case BDMF_ndbr6byte5:
    {
        uint8_t ndbcbyte5;
        err = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr6byte5_get(&ndbcbyte5);
        bdmf_session_print(session, "ndbcbyte5 = %u = 0x%x\n", ndbcbyte5, ndbcbyte5);
        break;
    }
    case BDMF_ndbr7:
    {
        uint32_t ndbc;
        err = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr7_get(&ndbc);
        bdmf_session_print(session, "ndbc = %u = 0x%x\n", ndbc, ndbc);
        break;
    }
    case BDMF_ndbr7byte5:
    {
        uint8_t ndbcbyte5;
        err = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr7byte5_get(&ndbcbyte5);
        bdmf_session_print(session, "ndbcbyte5 = %u = 0x%x\n", ndbcbyte5, ndbcbyte5);
        break;
    }
    case BDMF_ndbr8to39:
    {
        uint32_t ndbc;
        err = ag_drv_gpon_tx_ploam_and_dba_memory_ndbr8to39_get(parm[1].value.unumber, &ndbc);
        bdmf_session_print(session, "ndbc = %u = 0x%x\n", ndbc, ndbc);
        break;
    }
    default:
        err = BDMF_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

static int bcm_gpon_tx_ploam_and_dba_memory_cli_test(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_test_method m = parm[0].value.unumber;
    bdmf_error_t err = BDMF_ERR_OK;

    {
        uint32_t word_idx=gtmv(m, 0);
        uint8_t rplmc=gtmv(m, 8);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_rplm_set( %u %u)\n", word_idx, rplmc);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_rplm_set(word_idx, rplmc);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_rplm_get( word_idx, &rplmc);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_rplm_get( %u %u)\n", word_idx, rplmc);
        if(err || rplmc!=gtmv(m, 8))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t word_idx=gtmv(m, 0);
        uint8_t iplc=gtmv(m, 8);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_iplm_set( %u %u)\n", word_idx, iplc);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_iplm_set(word_idx, iplc);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_iplm_get( word_idx, &iplc);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_iplm_get( %u %u)\n", word_idx, iplc);
        if(err || iplc!=gtmv(m, 8))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t word_idx=gtmv(m, 0);
        uint8_t uplmc=gtmv(m, 8);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_uplm_set( %u %u)\n", word_idx, uplmc);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_uplm_set(word_idx, uplmc);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_uplm_get( word_idx, &uplmc);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_uplm_get( %u %u)\n", word_idx, uplmc);
        if(err || uplmc!=gtmv(m, 8))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t word_idx=gtmv(m, 0);
        uint8_t nplmc=gtmv(m, 8);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_nplm3_set( %u %u)\n", word_idx, nplmc);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_nplm3_set(word_idx, nplmc);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_nplm3_get( word_idx, &nplmc);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_nplm3_get( %u %u)\n", word_idx, nplmc);
        if(err || nplmc!=gtmv(m, 8))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t word_idx=gtmv(m, 0);
        uint8_t nplmc=gtmv(m, 8);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_nplm2_set( %u %u)\n", word_idx, nplmc);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_nplm2_set(word_idx, nplmc);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_nplm2_get( word_idx, &nplmc);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_nplm2_get( %u %u)\n", word_idx, nplmc);
        if(err || nplmc!=gtmv(m, 8))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t word_idx=gtmv(m, 0);
        uint8_t nplmc=gtmv(m, 8);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_nplm1_set( %u %u)\n", word_idx, nplmc);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_nplm1_set(word_idx, nplmc);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_nplm1_get( word_idx, &nplmc);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_nplm1_get( %u %u)\n", word_idx, nplmc);
        if(err || nplmc!=gtmv(m, 8))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t word_idx=gtmv(m, 0);
        uint8_t nplmc=gtmv(m, 8);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_nplm0_set( %u %u)\n", word_idx, nplmc);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_nplm0_set(word_idx, nplmc);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_nplm0_get( word_idx, &nplmc);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_nplm0_get( %u %u)\n", word_idx, nplmc);
        if(err || nplmc!=gtmv(m, 8))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t idbcc=gtmv(m, 32);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_idbr_set( %u)\n", idbcc);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_idbr_set(idbcc);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_idbr_get( &idbcc);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_idbr_get( %u)\n", idbcc);
        if(err || idbcc!=gtmv(m, 32))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint8_t idbr=gtmv(m, 8);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_idbrbyte5_set( %u)\n", idbr);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_idbrbyte5_set(idbr);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_idbrbyte5_get( &idbr);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_idbrbyte5_get( %u)\n", idbr);
        if(err || idbr!=gtmv(m, 8))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t ndbc=gtmv(m, 32);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_ndbr0_set( %u)\n", ndbc);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_ndbr0_set(ndbc);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_ndbr0_get( &ndbc);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_ndbr0_get( %u)\n", ndbc);
        if(err || ndbc!=gtmv(m, 32))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint8_t ndbcbyte5=gtmv(m, 8);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_ndbr0byte5_set( %u)\n", ndbcbyte5);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_ndbr0byte5_set(ndbcbyte5);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_ndbr0byte5_get( &ndbcbyte5);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_ndbr0byte5_get( %u)\n", ndbcbyte5);
        if(err || ndbcbyte5!=gtmv(m, 8))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t ndbc=gtmv(m, 32);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_ndbr1_set( %u)\n", ndbc);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_ndbr1_set(ndbc);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_ndbr1_get( &ndbc);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_ndbr1_get( %u)\n", ndbc);
        if(err || ndbc!=gtmv(m, 32))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint8_t ndbcbyte5=gtmv(m, 8);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_ndbr1byte5_set( %u)\n", ndbcbyte5);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_ndbr1byte5_set(ndbcbyte5);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_ndbr1byte5_get( &ndbcbyte5);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_ndbr1byte5_get( %u)\n", ndbcbyte5);
        if(err || ndbcbyte5!=gtmv(m, 8))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t ndbc=gtmv(m, 32);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_ndbr2_set( %u)\n", ndbc);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_ndbr2_set(ndbc);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_ndbr2_get( &ndbc);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_ndbr2_get( %u)\n", ndbc);
        if(err || ndbc!=gtmv(m, 32))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint8_t ndbcbyte5=gtmv(m, 8);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_ndbr2byte5_set( %u)\n", ndbcbyte5);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_ndbr2byte5_set(ndbcbyte5);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_ndbr2byte5_get( &ndbcbyte5);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_ndbr2byte5_get( %u)\n", ndbcbyte5);
        if(err || ndbcbyte5!=gtmv(m, 8))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t ndbc=gtmv(m, 32);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_ndbr3_set( %u)\n", ndbc);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_ndbr3_set(ndbc);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_ndbr3_get( &ndbc);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_ndbr3_get( %u)\n", ndbc);
        if(err || ndbc!=gtmv(m, 32))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint8_t ndbcbyte5=gtmv(m, 8);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_ndbr3byte5_set( %u)\n", ndbcbyte5);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_ndbr3byte5_set(ndbcbyte5);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_ndbr3byte5_get( &ndbcbyte5);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_ndbr3byte5_get( %u)\n", ndbcbyte5);
        if(err || ndbcbyte5!=gtmv(m, 8))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t ndbc=gtmv(m, 32);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_ndbr4_set( %u)\n", ndbc);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_ndbr4_set(ndbc);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_ndbr4_get( &ndbc);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_ndbr4_get( %u)\n", ndbc);
        if(err || ndbc!=gtmv(m, 32))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint8_t ndbcbyte5=gtmv(m, 8);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_ndbr4byte5_set( %u)\n", ndbcbyte5);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_ndbr4byte5_set(ndbcbyte5);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_ndbr4byte5_get( &ndbcbyte5);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_ndbr4byte5_get( %u)\n", ndbcbyte5);
        if(err || ndbcbyte5!=gtmv(m, 8))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t ndbc=gtmv(m, 32);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_ndbr5_set( %u)\n", ndbc);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_ndbr5_set(ndbc);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_ndbr5_get( &ndbc);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_ndbr5_get( %u)\n", ndbc);
        if(err || ndbc!=gtmv(m, 32))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint8_t ndbcbyte5=gtmv(m, 8);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_ndbr5byte5_set( %u)\n", ndbcbyte5);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_ndbr5byte5_set(ndbcbyte5);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_ndbr5byte5_get( &ndbcbyte5);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_ndbr5byte5_get( %u)\n", ndbcbyte5);
        if(err || ndbcbyte5!=gtmv(m, 8))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t ndbc=gtmv(m, 32);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_ndbr6_set( %u)\n", ndbc);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_ndbr6_set(ndbc);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_ndbr6_get( &ndbc);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_ndbr6_get( %u)\n", ndbc);
        if(err || ndbc!=gtmv(m, 32))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint8_t ndbcbyte5=gtmv(m, 8);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_ndbr6byte5_set( %u)\n", ndbcbyte5);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_ndbr6byte5_set(ndbcbyte5);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_ndbr6byte5_get( &ndbcbyte5);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_ndbr6byte5_get( %u)\n", ndbcbyte5);
        if(err || ndbcbyte5!=gtmv(m, 8))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t ndbc=gtmv(m, 32);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_ndbr7_set( %u)\n", ndbc);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_ndbr7_set(ndbc);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_ndbr7_get( &ndbc);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_ndbr7_get( %u)\n", ndbc);
        if(err || ndbc!=gtmv(m, 32))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint8_t ndbcbyte5=gtmv(m, 8);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_ndbr7byte5_set( %u)\n", ndbcbyte5);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_ndbr7byte5_set(ndbcbyte5);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_ndbr7byte5_get( &ndbcbyte5);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_ndbr7byte5_get( %u)\n", ndbcbyte5);
        if(err || ndbcbyte5!=gtmv(m, 8))
            return err ? err : BDMF_ERR_IO;
    }
    {
        uint32_t dbr_idx=gtmv(m, 5);
        uint32_t ndbc=gtmv(m, 24);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_ndbr8to39_set( %u %u)\n", dbr_idx, ndbc);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_ndbr8to39_set(dbr_idx, ndbc);
        if(!err) ag_drv_gpon_tx_ploam_and_dba_memory_ndbr8to39_get( dbr_idx, &ndbc);
        if(!err) bdmf_session_print(session, "ag_drv_gpon_tx_ploam_and_dba_memory_ndbr8to39_get( %u %u)\n", dbr_idx, ndbc);
        if(err || ndbc!=gtmv(m, 24))
            return err ? err : BDMF_ERR_IO;
    }
    return err;
}

static int bcm_gpon_tx_ploam_and_dba_memory_cli_address(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
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
    case bdmf_address_rplm : reg = &RU_REG(GPON_TX_PLOAM_AND_DBA_MEMORY, RPLM); blk = &RU_BLK(GPON_TX_PLOAM_AND_DBA_MEMORY); break;
    case bdmf_address_iplm : reg = &RU_REG(GPON_TX_PLOAM_AND_DBA_MEMORY, IPLM); blk = &RU_BLK(GPON_TX_PLOAM_AND_DBA_MEMORY); break;
    case bdmf_address_uplm : reg = &RU_REG(GPON_TX_PLOAM_AND_DBA_MEMORY, UPLM); blk = &RU_BLK(GPON_TX_PLOAM_AND_DBA_MEMORY); break;
    case bdmf_address_nplm3 : reg = &RU_REG(GPON_TX_PLOAM_AND_DBA_MEMORY, NPLM3); blk = &RU_BLK(GPON_TX_PLOAM_AND_DBA_MEMORY); break;
    case bdmf_address_nplm2 : reg = &RU_REG(GPON_TX_PLOAM_AND_DBA_MEMORY, NPLM2); blk = &RU_BLK(GPON_TX_PLOAM_AND_DBA_MEMORY); break;
    case bdmf_address_nplm1 : reg = &RU_REG(GPON_TX_PLOAM_AND_DBA_MEMORY, NPLM1); blk = &RU_BLK(GPON_TX_PLOAM_AND_DBA_MEMORY); break;
    case bdmf_address_nplm0 : reg = &RU_REG(GPON_TX_PLOAM_AND_DBA_MEMORY, NPLM0); blk = &RU_BLK(GPON_TX_PLOAM_AND_DBA_MEMORY); break;
    case bdmf_address_idbr : reg = &RU_REG(GPON_TX_PLOAM_AND_DBA_MEMORY, IDBR); blk = &RU_BLK(GPON_TX_PLOAM_AND_DBA_MEMORY); break;
    case bdmf_address_idbrbyte5 : reg = &RU_REG(GPON_TX_PLOAM_AND_DBA_MEMORY, IDBRBYTE5); blk = &RU_BLK(GPON_TX_PLOAM_AND_DBA_MEMORY); break;
    case bdmf_address_ndbr0 : reg = &RU_REG(GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR0); blk = &RU_BLK(GPON_TX_PLOAM_AND_DBA_MEMORY); break;
    case bdmf_address_ndbr0byte5 : reg = &RU_REG(GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR0BYTE5); blk = &RU_BLK(GPON_TX_PLOAM_AND_DBA_MEMORY); break;
    case bdmf_address_ndbr1 : reg = &RU_REG(GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR1); blk = &RU_BLK(GPON_TX_PLOAM_AND_DBA_MEMORY); break;
    case bdmf_address_ndbr1byte5 : reg = &RU_REG(GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR1BYTE5); blk = &RU_BLK(GPON_TX_PLOAM_AND_DBA_MEMORY); break;
    case bdmf_address_ndbr2 : reg = &RU_REG(GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR2); blk = &RU_BLK(GPON_TX_PLOAM_AND_DBA_MEMORY); break;
    case bdmf_address_ndbr2byte5 : reg = &RU_REG(GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR2BYTE5); blk = &RU_BLK(GPON_TX_PLOAM_AND_DBA_MEMORY); break;
    case bdmf_address_ndbr3 : reg = &RU_REG(GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR3); blk = &RU_BLK(GPON_TX_PLOAM_AND_DBA_MEMORY); break;
    case bdmf_address_ndbr3byte5 : reg = &RU_REG(GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR3BYTE5); blk = &RU_BLK(GPON_TX_PLOAM_AND_DBA_MEMORY); break;
    case bdmf_address_ndbr4 : reg = &RU_REG(GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR4); blk = &RU_BLK(GPON_TX_PLOAM_AND_DBA_MEMORY); break;
    case bdmf_address_ndbr4byte5 : reg = &RU_REG(GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR4BYTE5); blk = &RU_BLK(GPON_TX_PLOAM_AND_DBA_MEMORY); break;
    case bdmf_address_ndbr5 : reg = &RU_REG(GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR5); blk = &RU_BLK(GPON_TX_PLOAM_AND_DBA_MEMORY); break;
    case bdmf_address_ndbr5byte5 : reg = &RU_REG(GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR5BYTE5); blk = &RU_BLK(GPON_TX_PLOAM_AND_DBA_MEMORY); break;
    case bdmf_address_ndbr6 : reg = &RU_REG(GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR6); blk = &RU_BLK(GPON_TX_PLOAM_AND_DBA_MEMORY); break;
    case bdmf_address_ndbr6byte5 : reg = &RU_REG(GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR6BYTE5); blk = &RU_BLK(GPON_TX_PLOAM_AND_DBA_MEMORY); break;
    case bdmf_address_ndbr7 : reg = &RU_REG(GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR7); blk = &RU_BLK(GPON_TX_PLOAM_AND_DBA_MEMORY); break;
    case bdmf_address_ndbr7byte5 : reg = &RU_REG(GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR7BYTE5); blk = &RU_BLK(GPON_TX_PLOAM_AND_DBA_MEMORY); break;
    case bdmf_address_ndbr8to39 : reg = &RU_REG(GPON_TX_PLOAM_AND_DBA_MEMORY, NDBR8TO39); blk = &RU_BLK(GPON_TX_PLOAM_AND_DBA_MEMORY); break;
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

bdmfmon_handle_t ag_drv_gpon_tx_ploam_and_dba_memory_cli_init(bdmfmon_handle_t driver_dir)
{
    bdmfmon_handle_t dir;

    if ((dir = bdmfmon_dir_find(driver_dir, "gpon_tx_ploam_and_dba_memory"))!=NULL)
        return dir;
    dir = bdmfmon_dir_add(driver_dir, "gpon_tx_ploam_and_dba_memory", "gpon_tx_ploam_and_dba_memory", BDMF_ACCESS_ADMIN, NULL);

    {
        static bdmfmon_cmd_parm_t set_rplm[]={
            BDMFMON_MAKE_PARM("word_idx", "word_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("rplmc", "rplmc", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_iplm[]={
            BDMFMON_MAKE_PARM("word_idx", "word_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("iplc", "iplc", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_uplm[]={
            BDMFMON_MAKE_PARM("word_idx", "word_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("uplmc", "uplmc", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_nplm3[]={
            BDMFMON_MAKE_PARM("word_idx", "word_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("nplmc", "nplmc", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_nplm2[]={
            BDMFMON_MAKE_PARM("word_idx", "word_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("nplmc", "nplmc", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_nplm1[]={
            BDMFMON_MAKE_PARM("word_idx", "word_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("nplmc", "nplmc", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_nplm0[]={
            BDMFMON_MAKE_PARM("word_idx", "word_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("nplmc", "nplmc", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_idbr[]={
            BDMFMON_MAKE_PARM("idbcc", "idbcc", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_idbrbyte5[]={
            BDMFMON_MAKE_PARM("idbr", "idbr", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_ndbr0[]={
            BDMFMON_MAKE_PARM("ndbc", "ndbc", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_ndbr0byte5[]={
            BDMFMON_MAKE_PARM("ndbcbyte5", "ndbcbyte5", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_ndbr1[]={
            BDMFMON_MAKE_PARM("ndbc", "ndbc", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_ndbr1byte5[]={
            BDMFMON_MAKE_PARM("ndbcbyte5", "ndbcbyte5", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_ndbr2[]={
            BDMFMON_MAKE_PARM("ndbc", "ndbc", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_ndbr2byte5[]={
            BDMFMON_MAKE_PARM("ndbcbyte5", "ndbcbyte5", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_ndbr3[]={
            BDMFMON_MAKE_PARM("ndbc", "ndbc", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_ndbr3byte5[]={
            BDMFMON_MAKE_PARM("ndbcbyte5", "ndbcbyte5", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_ndbr4[]={
            BDMFMON_MAKE_PARM("ndbc", "ndbc", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_ndbr4byte5[]={
            BDMFMON_MAKE_PARM("ndbcbyte5", "ndbcbyte5", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_ndbr5[]={
            BDMFMON_MAKE_PARM("ndbc", "ndbc", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_ndbr5byte5[]={
            BDMFMON_MAKE_PARM("ndbcbyte5", "ndbcbyte5", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_ndbr6[]={
            BDMFMON_MAKE_PARM("ndbc", "ndbc", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_ndbr6byte5[]={
            BDMFMON_MAKE_PARM("ndbcbyte5", "ndbcbyte5", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_ndbr7[]={
            BDMFMON_MAKE_PARM("ndbc", "ndbc", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_ndbr7byte5[]={
            BDMFMON_MAKE_PARM("ndbcbyte5", "ndbcbyte5", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_ndbr8to39[]={
            BDMFMON_MAKE_PARM("dbr_idx", "dbr_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("ndbc", "ndbc", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_enum_val_t selector_table[] = {
            { .name="rplm", .val=BDMF_rplm, .parms=set_rplm },
            { .name="iplm", .val=BDMF_iplm, .parms=set_iplm },
            { .name="uplm", .val=BDMF_uplm, .parms=set_uplm },
            { .name="nplm3", .val=BDMF_nplm3, .parms=set_nplm3 },
            { .name="nplm2", .val=BDMF_nplm2, .parms=set_nplm2 },
            { .name="nplm1", .val=BDMF_nplm1, .parms=set_nplm1 },
            { .name="nplm0", .val=BDMF_nplm0, .parms=set_nplm0 },
            { .name="idbr", .val=BDMF_idbr, .parms=set_idbr },
            { .name="idbrbyte5", .val=BDMF_idbrbyte5, .parms=set_idbrbyte5 },
            { .name="ndbr0", .val=BDMF_ndbr0, .parms=set_ndbr0 },
            { .name="ndbr0byte5", .val=BDMF_ndbr0byte5, .parms=set_ndbr0byte5 },
            { .name="ndbr1", .val=BDMF_ndbr1, .parms=set_ndbr1 },
            { .name="ndbr1byte5", .val=BDMF_ndbr1byte5, .parms=set_ndbr1byte5 },
            { .name="ndbr2", .val=BDMF_ndbr2, .parms=set_ndbr2 },
            { .name="ndbr2byte5", .val=BDMF_ndbr2byte5, .parms=set_ndbr2byte5 },
            { .name="ndbr3", .val=BDMF_ndbr3, .parms=set_ndbr3 },
            { .name="ndbr3byte5", .val=BDMF_ndbr3byte5, .parms=set_ndbr3byte5 },
            { .name="ndbr4", .val=BDMF_ndbr4, .parms=set_ndbr4 },
            { .name="ndbr4byte5", .val=BDMF_ndbr4byte5, .parms=set_ndbr4byte5 },
            { .name="ndbr5", .val=BDMF_ndbr5, .parms=set_ndbr5 },
            { .name="ndbr5byte5", .val=BDMF_ndbr5byte5, .parms=set_ndbr5byte5 },
            { .name="ndbr6", .val=BDMF_ndbr6, .parms=set_ndbr6 },
            { .name="ndbr6byte5", .val=BDMF_ndbr6byte5, .parms=set_ndbr6byte5 },
            { .name="ndbr7", .val=BDMF_ndbr7, .parms=set_ndbr7 },
            { .name="ndbr7byte5", .val=BDMF_ndbr7byte5, .parms=set_ndbr7byte5 },
            { .name="ndbr8to39", .val=BDMF_ndbr8to39, .parms=set_ndbr8to39 },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "set", "set", bcm_gpon_tx_ploam_and_dba_memory_cli_set,
            BDMFMON_MAKE_PARM_SELECTOR("purpose", "purpose", selector_table, 0));
    }
    {
        static bdmfmon_cmd_parm_t set_default[]={
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_rplm[]={
            BDMFMON_MAKE_PARM("word_idx", "word_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_iplm[]={
            BDMFMON_MAKE_PARM("word_idx", "word_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_uplm[]={
            BDMFMON_MAKE_PARM("word_idx", "word_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_nplm3[]={
            BDMFMON_MAKE_PARM("word_idx", "word_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_nplm2[]={
            BDMFMON_MAKE_PARM("word_idx", "word_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_nplm1[]={
            BDMFMON_MAKE_PARM("word_idx", "word_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_nplm0[]={
            BDMFMON_MAKE_PARM("word_idx", "word_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_cmd_parm_t set_ndbr8to39[]={
            BDMFMON_MAKE_PARM("dbr_idx", "dbr_idx", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_enum_val_t selector_table[] = {
            { .name="rplm", .val=BDMF_rplm, .parms=set_rplm },
            { .name="iplm", .val=BDMF_iplm, .parms=set_iplm },
            { .name="uplm", .val=BDMF_uplm, .parms=set_uplm },
            { .name="nplm3", .val=BDMF_nplm3, .parms=set_nplm3 },
            { .name="nplm2", .val=BDMF_nplm2, .parms=set_nplm2 },
            { .name="nplm1", .val=BDMF_nplm1, .parms=set_nplm1 },
            { .name="nplm0", .val=BDMF_nplm0, .parms=set_nplm0 },
            { .name="idbr", .val=BDMF_idbr, .parms=set_default },
            { .name="idbrbyte5", .val=BDMF_idbrbyte5, .parms=set_default },
            { .name="ndbr0", .val=BDMF_ndbr0, .parms=set_default },
            { .name="ndbr0byte5", .val=BDMF_ndbr0byte5, .parms=set_default },
            { .name="ndbr1", .val=BDMF_ndbr1, .parms=set_default },
            { .name="ndbr1byte5", .val=BDMF_ndbr1byte5, .parms=set_default },
            { .name="ndbr2", .val=BDMF_ndbr2, .parms=set_default },
            { .name="ndbr2byte5", .val=BDMF_ndbr2byte5, .parms=set_default },
            { .name="ndbr3", .val=BDMF_ndbr3, .parms=set_default },
            { .name="ndbr3byte5", .val=BDMF_ndbr3byte5, .parms=set_default },
            { .name="ndbr4", .val=BDMF_ndbr4, .parms=set_default },
            { .name="ndbr4byte5", .val=BDMF_ndbr4byte5, .parms=set_default },
            { .name="ndbr5", .val=BDMF_ndbr5, .parms=set_default },
            { .name="ndbr5byte5", .val=BDMF_ndbr5byte5, .parms=set_default },
            { .name="ndbr6", .val=BDMF_ndbr6, .parms=set_default },
            { .name="ndbr6byte5", .val=BDMF_ndbr6byte5, .parms=set_default },
            { .name="ndbr7", .val=BDMF_ndbr7, .parms=set_default },
            { .name="ndbr7byte5", .val=BDMF_ndbr7byte5, .parms=set_default },
            { .name="ndbr8to39", .val=BDMF_ndbr8to39, .parms=set_ndbr8to39 },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "get", "get", bcm_gpon_tx_ploam_and_dba_memory_cli_get,
            BDMFMON_MAKE_PARM_SELECTOR("purpose", "purpose", selector_table, 0));
    }
    {
        static bdmfmon_enum_val_t enum_table_test_method[] = {
            { .name="low" , .val=bdmf_test_method_low },
            { .name="mid" , .val=bdmf_test_method_mid },
            { .name="high" , .val=bdmf_test_method_high },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "test", "test", bcm_gpon_tx_ploam_and_dba_memory_cli_test,
            BDMFMON_MAKE_PARM_ENUM("method", "low: 0000, mid: 1000, high: 1111", enum_table_test_method, 0));
    }
    {
        static bdmfmon_enum_val_t enum_table_address[] = {
            { .name="RPLM" , .val=bdmf_address_rplm },
            { .name="IPLM" , .val=bdmf_address_iplm },
            { .name="UPLM" , .val=bdmf_address_uplm },
            { .name="NPLM3" , .val=bdmf_address_nplm3 },
            { .name="NPLM2" , .val=bdmf_address_nplm2 },
            { .name="NPLM1" , .val=bdmf_address_nplm1 },
            { .name="NPLM0" , .val=bdmf_address_nplm0 },
            { .name="IDBR" , .val=bdmf_address_idbr },
            { .name="IDBRBYTE5" , .val=bdmf_address_idbrbyte5 },
            { .name="NDBR0" , .val=bdmf_address_ndbr0 },
            { .name="NDBR0BYTE5" , .val=bdmf_address_ndbr0byte5 },
            { .name="NDBR1" , .val=bdmf_address_ndbr1 },
            { .name="NDBR1BYTE5" , .val=bdmf_address_ndbr1byte5 },
            { .name="NDBR2" , .val=bdmf_address_ndbr2 },
            { .name="NDBR2BYTE5" , .val=bdmf_address_ndbr2byte5 },
            { .name="NDBR3" , .val=bdmf_address_ndbr3 },
            { .name="NDBR3BYTE5" , .val=bdmf_address_ndbr3byte5 },
            { .name="NDBR4" , .val=bdmf_address_ndbr4 },
            { .name="NDBR4BYTE5" , .val=bdmf_address_ndbr4byte5 },
            { .name="NDBR5" , .val=bdmf_address_ndbr5 },
            { .name="NDBR5BYTE5" , .val=bdmf_address_ndbr5byte5 },
            { .name="NDBR6" , .val=bdmf_address_ndbr6 },
            { .name="NDBR6BYTE5" , .val=bdmf_address_ndbr6byte5 },
            { .name="NDBR7" , .val=bdmf_address_ndbr7 },
            { .name="NDBR7BYTE5" , .val=bdmf_address_ndbr7byte5 },
            { .name="NDBR8TO39" , .val=bdmf_address_ndbr8to39 },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "address", "address", bcm_gpon_tx_ploam_and_dba_memory_cli_address,
            BDMFMON_MAKE_PARM_ENUM("method", "method", enum_table_address, 0),
            BDMFMON_MAKE_PARM("index2", "onu_id/alloc_id/port_id/etc...", BDMFMON_PARM_NUMBER, BDMFMON_PARM_FLAG_OPTIONAL));
    }
    return dir;
}
#endif /* USE_BDMF_SHELL */


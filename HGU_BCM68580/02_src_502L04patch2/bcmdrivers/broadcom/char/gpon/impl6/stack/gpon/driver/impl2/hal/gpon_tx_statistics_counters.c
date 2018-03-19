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
#include "gpon_tx_statistics_counters.h"
int ag_drv_gpon_tx_statistics_counters_iac_get(uint16_t *iacc)
{
    uint32_t reg_iac=0;

#ifdef VALIDATE_PARMS
    if(!iacc)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_STATISTICS_COUNTERS, IAC, reg_iac);

    *iacc = RU_FIELD_GET(0, GPON_TX_STATISTICS_COUNTERS, IAC, IACC, reg_iac);

    return 0;
}

int ag_drv_gpon_tx_statistics_counters_rac_get(uint32_t tx_q, uint16_t *racc)
{
    uint32_t reg_rac=0;

#ifdef VALIDATE_PARMS
    if(!racc)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((tx_q >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, tx_q, GPON_TX_STATISTICS_COUNTERS, RAC, reg_rac);

    *racc = RU_FIELD_GET(0, GPON_TX_STATISTICS_COUNTERS, RAC, RACC, reg_rac);

    return 0;
}

int ag_drv_gpon_tx_statistics_counters_rdbc_get(uint32_t tx_q, uint16_t *rdbcc)
{
    uint32_t reg_rdbc=0;

#ifdef VALIDATE_PARMS
    if(!rdbcc)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((tx_q >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, tx_q, GPON_TX_STATISTICS_COUNTERS, RDBC, reg_rdbc);

    *rdbcc = RU_FIELD_GET(0, GPON_TX_STATISTICS_COUNTERS, RDBC, RDBCC, reg_rdbc);

    return 0;
}

int ag_drv_gpon_tx_statistics_counters_tpc_get(uint32_t tx_q, uint32_t *tpcc)
{
    uint32_t reg_tpc=0;

#ifdef VALIDATE_PARMS
    if(!tpcc)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((tx_q >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, tx_q, GPON_TX_STATISTICS_COUNTERS, TPC, reg_tpc);

    *tpcc = RU_FIELD_GET(0, GPON_TX_STATISTICS_COUNTERS, TPC, TPCC, reg_tpc);

    return 0;
}

int ag_drv_gpon_tx_statistics_counters_tgc_get(uint32_t tx_q, uint32_t *tgcc)
{
    uint32_t reg_tgc=0;

#ifdef VALIDATE_PARMS
    if(!tgcc)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((tx_q >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, tx_q, GPON_TX_STATISTICS_COUNTERS, TGC, reg_tgc);

    *tgcc = RU_FIELD_GET(0, GPON_TX_STATISTICS_COUNTERS, TGC, TGCC, reg_tgc);

    return 0;
}

int ag_drv_gpon_tx_statistics_counters_tic_get(uint32_t tx_q, uint32_t *tic)
{
    uint32_t reg_tic=0;

#ifdef VALIDATE_PARMS
    if(!tic)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
    if((tx_q >= 4))
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, PARAM_NOT_IN_RANGE);
        return PARAM_NOT_IN_RANGE;
    }
#endif

    RU_REG_RAM_READ(0, tx_q, GPON_TX_STATISTICS_COUNTERS, TIC, reg_tic);

    *tic = RU_FIELD_GET(0, GPON_TX_STATISTICS_COUNTERS, TIC, TIC, reg_tic);

    return 0;
}

int ag_drv_gpon_tx_statistics_counters_ripc_get(uint32_t *ipcc)
{
    uint32_t reg_ripc=0;

#ifdef VALIDATE_PARMS
    if(!ipcc)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_STATISTICS_COUNTERS, RIPC, reg_ripc);

    *ipcc = RU_FIELD_GET(0, GPON_TX_STATISTICS_COUNTERS, RIPC, IPCC, reg_ripc);

    return 0;
}

int ag_drv_gpon_tx_statistics_counters_rnpc_get(uint32_t *rnpcc)
{
    uint32_t reg_rnpc=0;

#ifdef VALIDATE_PARMS
    if(!rnpcc)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_STATISTICS_COUNTERS, RNPC, reg_rnpc);

    *rnpcc = RU_FIELD_GET(0, GPON_TX_STATISTICS_COUNTERS, RNPC, RNPCC, reg_rnpc);

    return 0;
}

int ag_drv_gpon_tx_statistics_counters_rspc_get(uint16_t *rupcc, uint16_t *rrpcc)
{
    uint32_t reg_rspc=0;

#ifdef VALIDATE_PARMS
    if(!rupcc || !rrpcc)
    {
        pr_err("ERROR driver %s:%u|(%d)\n", __FILE__, __LINE__, ERROR_IN_PARAM);
        return ERROR_IN_PARAM;
    }
#endif

    RU_REG_READ(0, GPON_TX_STATISTICS_COUNTERS, RSPC, reg_rspc);

    *rupcc = RU_FIELD_GET(0, GPON_TX_STATISTICS_COUNTERS, RSPC, RUPCC, reg_rspc);
    *rrpcc = RU_FIELD_GET(0, GPON_TX_STATISTICS_COUNTERS, RSPC, RRPCC, reg_rspc);

    return 0;
}


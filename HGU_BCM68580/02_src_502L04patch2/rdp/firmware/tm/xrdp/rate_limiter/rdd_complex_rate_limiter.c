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

#include "rdd_complex_rate_limiter.h"
#include "rdd_scheduling.h"
#include "rdp_drv_qm.h"

extern uint32_t exponent_list[EXPONENT_LIST_LEN];

/* API to RDPA level */
bdmf_error_t rdd_complex_rate_limiter_cfg(rdpa_traffic_dir dir, uint8_t rl_index, rdd_complex_rl_cfg_t *rl_cfg)
{
    bdmf_error_t rc;
    uint32_t rl_vec_en, *valid_vector_ptr, sir_alloc, pir_alloc, pir_limit;
    rdd_rl_float_t complex_rl_float;
    uint32_t *complex_rate_limiter_table_ptr;

    RDD_BTRACE("dir = %d, rl_index = %d, rl_cfg %p = { sustain_budget = %d, peak_limit = %d, peak_rate = %d, "
        "block type =%d, block_index = %d }\n",
        dir, rl_index, rl_cfg, rl_cfg->sustain_budget, rl_cfg->peak_limit, rl_cfg->peak_rate, rl_cfg->type,
        rl_cfg->block_index);

    if ((rl_index >= MAX_INDEX_OF_COMPLEX_RL) ||
        (rl_index % 2 != 0) ||
        (rl_cfg->type >= num_of_rdd_complex_rl_block))
    {
        return BDMF_ERR_PARM;
    }

    if (dir == rdpa_dir_ds)
    {
        valid_vector_ptr = RDD_RATE_LIMITER_VALID_TABLE_DS_ADDRESS_ARR;
        complex_rate_limiter_table_ptr = RDD_BASIC_RATE_LIMITER_TABLE_DS_ADDRESS_ARR;
    }
    else
    {
        valid_vector_ptr = RDD_RATE_LIMITER_VALID_TABLE_US_ADDRESS_ARR;
        complex_rate_limiter_table_ptr = RDD_BASIC_RATE_LIMITER_TABLE_US_ADDRESS_ARR;
    }

    RDD_COMPLEX_RATE_LIMITER_DESCRIPTOR_SIR_CURRENT_BUDGET_WRITE_G(COMPLEX_RATE_LIMITER_SIR_INIT_RATE, complex_rate_limiter_table_ptr, rl_index / 2);
    RDD_COMPLEX_RATE_LIMITER_DESCRIPTOR_PIR_CURRENT_BUDGET_WRITE_G(COMPLEX_RATE_LIMITER_PIR_INIT_RATE, complex_rate_limiter_table_ptr, rl_index / 2);
    RDD_COMPLEX_RATE_LIMITER_DESCRIPTOR_BLOCK_INDEX_WRITE_G(rl_cfg->block_index, complex_rate_limiter_table_ptr, rl_index / 2);
    RDD_COMPLEX_RATE_LIMITER_DESCRIPTOR_BLOCK_TYPE_WRITE_G(rl_cfg->type, complex_rate_limiter_table_ptr, rl_index / 2);
    RDD_COMPLEX_RATE_LIMITER_DESCRIPTOR_RL_TYPE_WRITE_G(RDD_RL_TYPE_COMPLEX, complex_rate_limiter_table_ptr, rl_index / 2);

    sir_alloc = rdd_rate_to_alloc_unit(rl_cfg->sustain_budget, RATE_LIMITER_TIMER_PERIOD_IN_USEC);
    complex_rl_float = rdd_rate_limiter_get_floating_point_rep(sir_alloc, exponent_list);
    if ((!complex_rl_float.exponent) && (!complex_rl_float.mantissa))
        return BDMF_ERR_PARM;

    RDD_COMPLEX_RATE_LIMITER_DESCRIPTOR_SIR_ALLOC_EXPONENT_WRITE_G(complex_rl_float.exponent, complex_rate_limiter_table_ptr, rl_index / 2);
    RDD_COMPLEX_RATE_LIMITER_DESCRIPTOR_SIR_ALLOC_MANTISSA_WRITE_G(complex_rl_float.mantissa, complex_rate_limiter_table_ptr, rl_index / 2);
    RDD_COMPLEX_RATE_LIMITER_DESCRIPTOR_SIR_LIMIT_EXPONENT_WRITE_G(complex_rl_float.exponent, complex_rate_limiter_table_ptr, rl_index / 2);
    RDD_COMPLEX_RATE_LIMITER_DESCRIPTOR_SIR_LIMIT_MANTISSA_WRITE_G(complex_rl_float.mantissa, complex_rate_limiter_table_ptr, rl_index / 2);

    pir_alloc = rdd_rate_to_alloc_unit(rl_cfg->peak_rate, RATE_LIMITER_TIMER_PERIOD_IN_USEC);
    complex_rl_float = rdd_rate_limiter_get_floating_point_rep(pir_alloc, exponent_list);
    RDD_COMPLEX_RATE_LIMITER_DESCRIPTOR_PIR_ALLOC_EXPONENT_WRITE_G(complex_rl_float.exponent, complex_rate_limiter_table_ptr, rl_index / 2);
    RDD_COMPLEX_RATE_LIMITER_DESCRIPTOR_PIR_ALLOC_MANTISSA_WRITE_G(complex_rl_float.mantissa, complex_rate_limiter_table_ptr, rl_index / 2);

    pir_limit = rl_cfg->peak_limit;
    if (!pir_limit)
        pir_limit = pir_alloc;
    if (pir_limit > RL_MAX_BUCKET_SIZE)
        pir_limit = RL_MAX_BUCKET_SIZE;
    complex_rl_float = rdd_rate_limiter_get_floating_point_rep(pir_limit, exponent_list);
    if ((!complex_rl_float.exponent) && (!complex_rl_float.mantissa))
        return BDMF_ERR_PARM;

    RDD_COMPLEX_RATE_LIMITER_DESCRIPTOR_PIR_LIMIT_EXPONENT_WRITE_G(complex_rl_float.exponent, complex_rate_limiter_table_ptr, rl_index / 2);
    RDD_COMPLEX_RATE_LIMITER_DESCRIPTOR_PIR_LIMIT_MANTISSA_WRITE_G(complex_rl_float.mantissa, complex_rate_limiter_table_ptr, rl_index / 2);

    switch ((uint32_t)rl_cfg->type)
    {
        case rdd_complex_rl_queue:
            if (dir == rdpa_dir_us)
            {
                rc = rdd_ag_us_tm_scheduling_queue_table_rate_limiter_index_set(rl_cfg->block_index - drv_qm_get_us_start(), rl_index);
                rc = rc ? rc : rdd_ag_us_tm_scheduling_queue_table_rate_limit_enable_set(rl_cfg->block_index - drv_qm_get_us_start(), 1);
            }
            else
            {
                rc = rdd_ag_ds_tm_scheduling_queue_table_rate_limiter_index_set(rl_cfg->block_index - drv_qm_get_ds_start(), rl_index);
                rc = rc ? rc : rdd_ag_ds_tm_scheduling_queue_table_rate_limit_enable_set(rl_cfg->block_index - drv_qm_get_ds_start(), 1);
            }
            break;
        case rdd_complex_rl_basic_scheduler:
            rc = rdd_basic_scheduler_rate_limiter_cfg(dir, rl_cfg->block_index, rl_index);
            break;
        default:
            return BDMF_ERR_INTERNAL;
    }

    /* enable the rate limiter */
    RDD_BYTES_4_BITS_READ_G(rl_vec_en, valid_vector_ptr, rl_index / 32);
    rl_vec_en |= (1 << (rl_index & 0x1f));
    RDD_BYTES_4_BITS_WRITE_G(rl_vec_en, valid_vector_ptr, rl_index / 32);

    return rc;
}

bdmf_error_t rdd_complex_rate_limiter_remove(rdpa_traffic_dir dir, uint8_t rl_index)
{
    bdmf_error_t rc;
    uint32_t type, block_index;
    uint32_t *complex_rate_limiter_table_ptr;;

    RDD_BTRACE("dir = %d, rl_index = %d\n", dir, rl_index);

    if (rl_index >= MAX_INDEX_OF_COMPLEX_RL)
    {
        return BDMF_ERR_PARM;
    }

    if (dir == rdpa_dir_ds)
        complex_rate_limiter_table_ptr = RDD_BASIC_RATE_LIMITER_TABLE_DS_ADDRESS_ARR;
    else
        complex_rate_limiter_table_ptr = RDD_BASIC_RATE_LIMITER_TABLE_US_ADDRESS_ARR;

    RDD_COMPLEX_RATE_LIMITER_DESCRIPTOR_BLOCK_TYPE_READ_G(type, complex_rate_limiter_table_ptr, rl_index / 2);
    RDD_COMPLEX_RATE_LIMITER_DESCRIPTOR_BLOCK_INDEX_READ_G(block_index, complex_rate_limiter_table_ptr, rl_index / 2);
    switch (type)
    {
        case rdd_complex_rl_queue:
            rc = rdd_scheduling_queue_rate_limiter_remove(dir, block_index);
            break;
        case rdd_complex_rl_basic_scheduler:
            rc = rdd_basic_scheduler_rate_limiter_remove(dir, block_index);
            break;
        default:
            return BDMF_ERR_INTERNAL;
    }
    /* rl will be removed from rl_vec_en by the budget allocator task */
    RDD_COMPLEX_RATE_LIMITER_DESCRIPTOR_SIR_CURRENT_BUDGET_WRITE_G(COMPLEX_RATE_LIMITER_SIR_INIT_RATE, complex_rate_limiter_table_ptr, rl_index / 2);
    RDD_COMPLEX_RATE_LIMITER_DESCRIPTOR_PIR_CURRENT_BUDGET_WRITE_G(COMPLEX_RATE_LIMITER_PIR_INIT_RATE, complex_rate_limiter_table_ptr, rl_index / 2);

    return rc;
}


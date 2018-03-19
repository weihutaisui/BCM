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

#include "rdd_basic_rate_limiter.h"
#include "rdd_scheduling.h"
#include "rdp_drv_qm.h"

extern uint32_t exponent_list[EXPONENT_LIST_LEN];

/* API to RDPA level */
bdmf_error_t rdd_basic_rate_limiter_cfg(rdpa_traffic_dir dir, uint8_t basic_rl_index, rdd_basic_rl_cfg_t *rl_cfg)
{
    bdmf_error_t rc = BDMF_ERR_OK;
    uint32_t basic_rl_vec_en, *valid_vector_ptr, alloc_rate, limit_rate;
    rdd_rl_float_t basic_rl_float;
    uint32_t *basic_rate_limiter_table_ptr;

    RDD_BTRACE("dir = %d, basic_rl_index = %d, rl_cfg %p = { rate = %d, block type = %d, block_index = %d  }\n",
        dir, basic_rl_index, rl_cfg, rl_cfg->rate, rl_cfg->type, rl_cfg->block_index);
    if (basic_rl_index >= RDD_BASIC_RATE_LIMITER_TABLE_SIZE)
    {
        return BDMF_ERR_PARM;
    }

    if (dir == rdpa_dir_ds)
    {
        valid_vector_ptr = RDD_RATE_LIMITER_VALID_TABLE_DS_ADDRESS_ARR;
        basic_rate_limiter_table_ptr = RDD_BASIC_RATE_LIMITER_TABLE_DS_ADDRESS_ARR;
    }
    else
    {
        valid_vector_ptr = RDD_RATE_LIMITER_VALID_TABLE_US_ADDRESS_ARR;
        basic_rate_limiter_table_ptr = RDD_BASIC_RATE_LIMITER_TABLE_US_ADDRESS_ARR;
    }

    RDD_BASIC_RATE_LIMITER_DESCRIPTOR_CURRENT_BUDGET_WRITE_G(BASIC_RATE_LIMITER_INIT_RATE, basic_rate_limiter_table_ptr, basic_rl_index);
    RDD_BASIC_RATE_LIMITER_DESCRIPTOR_BLOCK_TYPE_WRITE_G(rl_cfg->type, basic_rate_limiter_table_ptr, basic_rl_index);
    if (rl_cfg->type == rdd_basic_rl_queue)
        if (dir == rdpa_dir_ds)
            RDD_BASIC_RATE_LIMITER_DESCRIPTOR_BLOCK_INDEX_WRITE_G((rl_cfg->block_index - drv_qm_get_ds_start()), basic_rate_limiter_table_ptr, basic_rl_index);
        else
            RDD_BASIC_RATE_LIMITER_DESCRIPTOR_BLOCK_INDEX_WRITE_G((rl_cfg->block_index - drv_qm_get_us_start()), basic_rate_limiter_table_ptr, basic_rl_index);
    else
        RDD_BASIC_RATE_LIMITER_DESCRIPTOR_BLOCK_INDEX_WRITE_G(rl_cfg->block_index, basic_rate_limiter_table_ptr, basic_rl_index);
    RDD_BASIC_RATE_LIMITER_DESCRIPTOR_RL_TYPE_WRITE_G(RDD_RL_TYPE_BASIC, basic_rate_limiter_table_ptr, basic_rl_index);

    alloc_rate = rdd_rate_to_alloc_unit(rl_cfg->rate, RATE_LIMITER_TIMER_PERIOD_IN_USEC);
    basic_rl_float = rdd_rate_limiter_get_floating_point_rep(alloc_rate, exponent_list);
    if ((!basic_rl_float.exponent) && (!basic_rl_float.mantissa))
        return BDMF_ERR_PARM;

    RDD_BASIC_RATE_LIMITER_DESCRIPTOR_ALLOC_EXPONENT_WRITE_G(basic_rl_float.exponent, basic_rate_limiter_table_ptr, basic_rl_index);
    RDD_BASIC_RATE_LIMITER_DESCRIPTOR_ALLOC_MANTISSA_WRITE_G(basic_rl_float.mantissa, basic_rate_limiter_table_ptr, basic_rl_index);

    limit_rate = rl_cfg->limit;
    if (!limit_rate)
        limit_rate = alloc_rate;
    if (limit_rate > RL_MAX_BUCKET_SIZE)
        limit_rate = RL_MAX_BUCKET_SIZE;
    basic_rl_float = rdd_rate_limiter_get_floating_point_rep(limit_rate, exponent_list);
    if ((!basic_rl_float.exponent) && (!basic_rl_float.mantissa))
        return BDMF_ERR_PARM;

    RDD_BASIC_RATE_LIMITER_DESCRIPTOR_LIMIT_EXPONENT_WRITE_G(basic_rl_float.exponent, basic_rate_limiter_table_ptr, basic_rl_index);
    RDD_BASIC_RATE_LIMITER_DESCRIPTOR_LIMIT_MANTISSA_WRITE_G(basic_rl_float.mantissa, basic_rate_limiter_table_ptr, basic_rl_index);

    switch ((uint32_t)rl_cfg->type)
    {
        case rdd_basic_rl_queue:
            if (dir == rdpa_dir_us)
            {
                rc = rdd_ag_us_tm_scheduling_queue_table_rate_limiter_index_set(rl_cfg->block_index - drv_qm_get_us_start(), basic_rl_index);
                rc = rc ? rc : rdd_ag_us_tm_scheduling_queue_table_rate_limit_enable_set(rl_cfg->block_index - drv_qm_get_us_start(), 1);
            }
            else
            {
                rc = rdd_ag_ds_tm_scheduling_queue_table_rate_limiter_index_set(rl_cfg->block_index - drv_qm_get_ds_start(), basic_rl_index);
                rc = rc ? rc : rdd_ag_ds_tm_scheduling_queue_table_rate_limit_enable_set(rl_cfg->block_index - drv_qm_get_ds_start(), 1);
            }
            break;
        case rdd_basic_rl_basic_scheduler:
            rc = rdd_basic_scheduler_rate_limiter_cfg(dir, rl_cfg->block_index, basic_rl_index);
            break;
        case rdd_basic_rl_complex_scheduler:
            {
                rdd_complex_scheduler_rl_cfg_t cs_rl_cfg = 
                {
                    .rate_limit_enable = 1,
                    .is_positive_budget = 1,
                    .rate_limiter_index = basic_rl_index
                };
                if (dir == rdpa_dir_ds)
                    return BDMF_ERR_NOT_SUPPORTED;
/*                  rc = rdd_ag_ds_tm_complex_scheduler_rl_cfg_set(rl_cfg->block_index, &cs_rl_cfg);  */
                else
                    rc = rdd_ag_us_tm_complex_scheduler_rl_cfg_set(rl_cfg->block_index, &cs_rl_cfg);
            }
            break;
    }

    /* enable the rate limiter */
    RDD_BYTES_4_BITS_READ_G(basic_rl_vec_en, valid_vector_ptr, basic_rl_index / 32);
    basic_rl_vec_en |= (1 << (basic_rl_index & 0x1f));
    RDD_BYTES_4_BITS_WRITE_G(basic_rl_vec_en, valid_vector_ptr, basic_rl_index / 32);

    return rc;
}

bdmf_error_t rdd_basic_rate_limiter_remove(rdpa_traffic_dir dir, uint8_t basic_rl_index)
{
    uint32_t block_index, type;
    bdmf_error_t rc;
    uint32_t *basic_rate_limiter_table_ptr;

    RDD_BTRACE("dir = %d, basic_rl_index = %d\n", dir, basic_rl_index);

    if (basic_rl_index >= RDD_BASIC_RATE_LIMITER_TABLE_SIZE)
    {
        return BDMF_ERR_PARM;
    }

    if (dir == rdpa_dir_ds)
        basic_rate_limiter_table_ptr = RDD_BASIC_RATE_LIMITER_TABLE_DS_ADDRESS_ARR;
    else
        basic_rate_limiter_table_ptr = RDD_BASIC_RATE_LIMITER_TABLE_US_ADDRESS_ARR;

    RDD_BASIC_RATE_LIMITER_DESCRIPTOR_BLOCK_TYPE_READ_G(type, basic_rate_limiter_table_ptr, basic_rl_index);
    RDD_BASIC_RATE_LIMITER_DESCRIPTOR_BLOCK_INDEX_READ_G(block_index, basic_rate_limiter_table_ptr, basic_rl_index);
    switch (type)
    {
        case rdd_basic_rl_queue:
            if (dir == rdpa_dir_ds)
                block_index += drv_qm_get_ds_start();
            rc = rdd_scheduling_queue_rate_limiter_remove(dir, block_index);
            break;
        case rdd_basic_rl_basic_scheduler:
            rc = rdd_basic_scheduler_rate_limiter_remove(dir, block_index);
            break;
        case rdd_basic_rl_complex_scheduler:
            {
                rdd_complex_scheduler_rl_cfg_t rl_cfg =
                {
                    .rate_limit_enable = 0,
                    .is_positive_budget = 1,
                    .rate_limiter_index = 0
                };
                if (dir == rdpa_dir_ds)
                    return BDMF_ERR_NOT_SUPPORTED;
/*                  rc = rdd_ag_ds_tm_complex_scheduler_rl_cfg_set(block_index, &rl_cfg); */
                else
                    rc = rdd_ag_us_tm_complex_scheduler_rl_cfg_set(block_index, &rl_cfg);
            }
            break;
        default:
            return BDMF_ERR_INTERNAL;
    }
    /* rl will be removed from rl_vec_en by the budget allocator task */
    RDD_BASIC_RATE_LIMITER_DESCRIPTOR_CURRENT_BUDGET_WRITE_G(BASIC_RATE_LIMITER_INIT_RATE, basic_rate_limiter_table_ptr, basic_rl_index);

    return rc;
}


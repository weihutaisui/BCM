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

#include "rdd_basic_scheduler.h"
#ifndef _CFE_REDUCED_XRDP_
#include "rdd_ag_us_tm.h"
#endif
#include "rdd_ag_ds_tm.h"
#include "rdp_drv_qm.h"


extern rdd_bb_id rdpa_emac_to_bb_id_tx[rdpa_emac__num_of];
extern bdmf_error_t rdd_scheduling_scheduler_block_cfg(rdpa_traffic_dir dir, uint8_t qm_queue_index, rdd_scheduling_queue_descriptor_t *scheduler_cfg, bdmf_boolean type, uint8_t dwrr_offset, bdmf_boolean enable);

/* mappping between basic scheduler to bbh queue */
static uint8_t basic_scheduler_to_bbh_queue[2][RDD_BASIC_SCHEDULER_TABLE_SIZE];

/* API to RDPA level */
bdmf_error_t rdd_basic_scheduler_cfg(rdpa_traffic_dir dir, uint8_t basic_scheduler_index, basic_scheduler_cfg_t *cfg)
{
    rdd_bbh_queue_descriptor_t bbh_queue_cfg;
    bdmf_error_t rc = BDMF_ERR_OK;
    uint32_t *basic_scheduler_table_ptr;

    RDD_BTRACE("dir = %d, basic_scheduler_index = %d, cfg %p = { dwrr_offset = %d, bbh_queue_index = %d }\n",
        dir, basic_scheduler_index, cfg, cfg->dwrr_offset, cfg->bbh_queue_index);

    if ((basic_scheduler_index >= RDD_BASIC_SCHEDULER_TABLE_SIZE) ||
        (cfg->dwrr_offset >= basic_scheduler_num_of_dwrr_offset) ||
        (cfg->bbh_queue_index >= RDD_BBH_QUEUE_TABLE_SIZE))
    {
        return BDMF_ERR_PARM;
    }

    bbh_queue_cfg.scheduler_index = basic_scheduler_index;
    bbh_queue_cfg.scheduler_type = RDD_SCHED_TYPE_BASIC;
    if (dir == rdpa_dir_ds)
        basic_scheduler_table_ptr = RDD_BASIC_SCHEDULER_TABLE_DS_ADDRESS_ARR;
#ifndef _CFE_REDUCED_XRDP_
    else
        basic_scheduler_table_ptr = RDD_BASIC_SCHEDULER_TABLE_US_ADDRESS_ARR;
#endif

    /* initialize budget for all queues - relevent for the case no rate limiter was configured */
    RDD_BASIC_SCHEDULER_DESCRIPTOR_SLOT_BUDGET_BIT_VECTOR_WRITE_G(BASIC_SCHEDULER_FULL_BUDGET_VECTOR, basic_scheduler_table_ptr, basic_scheduler_index);
    RDD_BASIC_SCHEDULER_DESCRIPTOR_DWRR_OFFSET_WRITE_G(cfg->dwrr_offset, basic_scheduler_table_ptr, basic_scheduler_index);
    RDD_BASIC_SCHEDULER_DESCRIPTOR_IS_POSITIVE_BUDGET_WRITE_G(1, basic_scheduler_table_ptr, basic_scheduler_index);

    if (dir == rdpa_dir_ds)
    {
#ifndef G9991
#ifdef XRDP_BBH_PER_LAN_PORT
        RDD_BASIC_SCHEDULER_DESCRIPTOR_BBH_QUEUE_WRITE_G(rdpa_emac_to_bb_id_tx[cfg->bbh_queue_index], basic_scheduler_table_ptr, basic_scheduler_index);
#else
        RDD_BASIC_SCHEDULER_DESCRIPTOR_BBH_QUEUE_WRITE_G(cfg->bbh_queue_index, basic_scheduler_table_ptr, basic_scheduler_index);
#endif
#else /* G9991 */
        RDD_BASIC_SCHEDULER_DESCRIPTOR_BBH_QUEUE_WRITE_G(0, basic_scheduler_table_ptr, basic_scheduler_index);
#endif
        rc = rdd_ag_ds_tm_bbh_queue_descriptor_set(cfg->bbh_queue_index, &bbh_queue_cfg);
    }
    else
    {
#ifndef _CFE_REDUCED_XRDP_
        RDD_BASIC_SCHEDULER_DESCRIPTOR_BBH_QUEUE_WRITE_G(cfg->bbh_queue_index, basic_scheduler_table_ptr, basic_scheduler_index);
        rc = rdd_ag_us_tm_bbh_queue_descriptor_set(cfg->bbh_queue_index, &bbh_queue_cfg);
#else
        rc = BDMF_ERR_NOT_SUPPORTED;
#endif
    }

    /* init bbh-queue */
    basic_scheduler_to_bbh_queue[dir][basic_scheduler_index] = cfg->bbh_queue_index;

    return rc;
}

bdmf_error_t rdd_basic_scheduler_dwrr_offset_cfg(rdpa_traffic_dir dir, uint8_t basic_scheduler_index, uint8_t dwrr_offset)
{
    uint32_t *basic_scheduler_table_ptr;

    RDD_BTRACE("dir = %d, basic_scheduler_index = %d, dwrr_offset = %d }\n",
        dir, basic_scheduler_index, dwrr_offset);

    if ((basic_scheduler_index >= RDD_BASIC_SCHEDULER_TABLE_SIZE) ||
        (dwrr_offset >= basic_scheduler_num_of_dwrr_offset))
    {
        return BDMF_ERR_PARM;
    }

    if (dir == rdpa_dir_ds)
        basic_scheduler_table_ptr = RDD_BASIC_SCHEDULER_TABLE_DS_ADDRESS_ARR;
#ifndef _CFE_REDUCED_XRDP_
    else
        basic_scheduler_table_ptr = RDD_BASIC_SCHEDULER_TABLE_US_ADDRESS_ARR;
#endif

    /* initialize budget for all queues - relevent for the case no rate limiter was configured */
    RDD_BASIC_SCHEDULER_DESCRIPTOR_DWRR_OFFSET_WRITE_G(dwrr_offset, basic_scheduler_table_ptr, basic_scheduler_index);

    return BDMF_ERR_OK;
}

bdmf_error_t rdd_basic_scheduler_queue_cfg(rdpa_traffic_dir dir, uint8_t basic_scheduler_index, basic_scheduler_queue_t *queue)
{
    uint32_t *basic_scheduler_table_ptr;
    bdmf_error_t rc;
    uint8_t first_queue_index;
    rdd_scheduling_queue_descriptor_t queue_cfg;

    RDD_BTRACE("dir = %d, basic_scheduler_index = %d, queue %p = { qm_queue_index = %d, queue_scheduler_index = %d, "
        "quantum_number = %d }\n",
        dir, basic_scheduler_index, queue, queue->qm_queue_index, queue->queue_scheduler_index, queue->quantum_number);

    if ((basic_scheduler_index >= RDD_BASIC_SCHEDULER_TABLE_SIZE) ||
        (queue->queue_scheduler_index >= BASIC_SCHEDULER_NUM_OF_QUEUES))
    {
        return BDMF_ERR_PARM;
    }
    if (dir == rdpa_dir_ds)
    {
        basic_scheduler_table_ptr = RDD_BASIC_SCHEDULER_TABLE_DS_ADDRESS_ARR;
        first_queue_index = drv_qm_get_ds_start();
        rc = rdd_ag_ds_tm_scheduling_queue_table_quantum_number_set((queue->qm_queue_index - first_queue_index), queue->quantum_number);
    }
#ifndef _CFE_REDUCED_XRDP_
    else
    {
        basic_scheduler_table_ptr = RDD_BASIC_SCHEDULER_TABLE_US_ADDRESS_ARR;
        first_queue_index = drv_qm_get_us_start();
        rc = rdd_ag_us_tm_scheduling_queue_table_quantum_number_set((queue->qm_queue_index - first_queue_index), queue->quantum_number);
    }
#endif

    /* mapping queue to basic scheduler */
    RDD_BASIC_SCHEDULER_DESCRIPTOR_QUEUE_INDEX_WRITE_G((queue->qm_queue_index - first_queue_index), basic_scheduler_table_ptr, basic_scheduler_index, queue->queue_scheduler_index);

    /* mapping basic scheduler to queue */
    queue_cfg.bbh_queue_index = basic_scheduler_to_bbh_queue[dir][basic_scheduler_index];
    queue_cfg.scheduler_index = basic_scheduler_index;
    queue_cfg.block_type = 0; /* basic scheduler */
    queue_cfg.queue_bit_mask = 1 << (queue->queue_scheduler_index);
    rc = rc ? rc : rdd_scheduling_scheduler_block_cfg(dir, queue->qm_queue_index, &queue_cfg, 0, 0, 1);

    return rc;
}

bdmf_error_t rdd_basic_scheduler_queue_remove(rdpa_traffic_dir dir, uint8_t basic_scheduler_index, uint8_t queue_scheduler_index)
{
    RDD_BASIC_SCHEDULER_DESCRIPTOR_DTS *entry;
    rdd_scheduling_queue_descriptor_t queue_cfg = {};
    uint8_t queue_index;
    bdmf_error_t rc;

    RDD_BTRACE("dir = %d, basic_scheduler_index = %d, queue_scheduler_index = %d\n",
        dir, basic_scheduler_index, queue_scheduler_index);

    if ((basic_scheduler_index >= RDD_BASIC_SCHEDULER_TABLE_SIZE) ||
        (queue_scheduler_index >= BASIC_SCHEDULER_NUM_OF_QUEUES))
    {
        return BDMF_ERR_PARM;
    }

    if (dir == rdpa_dir_ds)
        entry = ((RDD_BASIC_SCHEDULER_DESCRIPTOR_DTS *)RDD_BASIC_SCHEDULER_TABLE_DS_PTR(get_runner_idx(ds_tm_runner_image))) + basic_scheduler_index;
#ifndef _CFE_REDUCED_XRDP_
    else
        entry = ((RDD_BASIC_SCHEDULER_DESCRIPTOR_DTS *)RDD_BASIC_SCHEDULER_TABLE_US_PTR(get_runner_idx(us_tm_runner_image))) + basic_scheduler_index;
#endif

    /* write bit mask 0 to queue */
    RDD_BASIC_SCHEDULER_DESCRIPTOR_QUEUE_INDEX_READ(queue_index, entry, queue_scheduler_index);
    if (dir == rdpa_dir_ds)
        queue_index += drv_qm_get_ds_start();
    rc = rdd_scheduling_scheduler_block_cfg(dir, queue_index, &queue_cfg, 0, 0, 0);

    return rc;
}

/* API to complex scheduler module */
bdmf_error_t rdd_basic_scheduler_dwrr_cfg(rdpa_traffic_dir dir, uint8_t basic_scheduler_index, uint8_t quantum_number)
{
    uint32_t *basic_scheduler_table_ptr;

    if (basic_scheduler_index >= RDD_BASIC_SCHEDULER_TABLE_SIZE)
    {
        return BDMF_ERR_PARM;
    }

    if (dir == rdpa_dir_ds)
        basic_scheduler_table_ptr = RDD_BASIC_SCHEDULER_TABLE_DS_ADDRESS_ARR;
#ifndef _CFE_REDUCED_XRDP_
    else
        basic_scheduler_table_ptr = RDD_BASIC_SCHEDULER_TABLE_US_ADDRESS_ARR;
#endif

    RDD_BASIC_SCHEDULER_DESCRIPTOR_QUANTUM_NUMBER_WRITE_G(quantum_number, basic_scheduler_table_ptr, basic_scheduler_index);

    return BDMF_ERR_OK;
}

bdmf_error_t rdd_basic_scheduler_block_cfg(rdpa_traffic_dir dir, uint8_t basic_scheduler_index,
    rdd_scheduling_queue_descriptor_t *scheduler_cfg, uint8_t dwrr_offset)
{
    uint32_t *basic_scheduler_table_ptr;

    RDD_BTRACE("dir = %d, basic_scheduler_index = %d, scheduler_cfg %p = { scheduler_index = %d, bit_mask = %d, "
        "bbh_queue = %d, scheduler_type = %d, dwrr_offset = %d }\n",
        dir, basic_scheduler_index, scheduler_cfg, scheduler_cfg->scheduler_index, scheduler_cfg->queue_bit_mask,
        scheduler_cfg->bbh_queue_index, scheduler_cfg->block_type, dwrr_offset);

    if ((basic_scheduler_index >= RDD_BASIC_SCHEDULER_TABLE_SIZE) ||
        (dwrr_offset >= basic_scheduler_num_of_dwrr_offset) ||
        (scheduler_cfg->bbh_queue_index >= RDD_BBH_QUEUE_TABLE_SIZE) ||
#ifndef _CFE_REDUCED_XRDP_
        (scheduler_cfg->scheduler_index >= RDD_COMPLEX_SCHEDULER_TABLE_SIZE) ||
#endif
        (scheduler_cfg->queue_bit_mask >= RDD_BASIC_SCHEDULER_TABLE_SIZE))
    {
        return BDMF_ERR_PARM;
    }

    if (dir == rdpa_dir_ds)
        basic_scheduler_table_ptr = RDD_BASIC_SCHEDULER_TABLE_DS_ADDRESS_ARR;
#ifndef _CFE_REDUCED_XRDP_
    else
        basic_scheduler_table_ptr = RDD_BASIC_SCHEDULER_TABLE_US_ADDRESS_ARR;
#endif

    /* initialize budget for all queues - relevent for the case no rate limiter was configured */
    RDD_BASIC_SCHEDULER_DESCRIPTOR_SLOT_BUDGET_BIT_VECTOR_WRITE_G(BASIC_SCHEDULER_FULL_BUDGET_VECTOR, basic_scheduler_table_ptr, basic_scheduler_index);
    RDD_BASIC_SCHEDULER_DESCRIPTOR_DWRR_OFFSET_WRITE_G(dwrr_offset, basic_scheduler_table_ptr, basic_scheduler_index);

    /* mapping basic scheduler to complex scheduler */
    RDD_BASIC_SCHEDULER_DESCRIPTOR_COMPLEX_SCHEDULER_EXISTS_WRITE_G(scheduler_cfg->block_type, basic_scheduler_table_ptr, basic_scheduler_index);
    RDD_BASIC_SCHEDULER_DESCRIPTOR_COMPLEX_SCHEDULER_INDEX_WRITE_G(scheduler_cfg->scheduler_index, basic_scheduler_table_ptr, basic_scheduler_index);
    RDD_BASIC_SCHEDULER_DESCRIPTOR_COMPLEX_SCHEDULER_SLOT_INDEX_WRITE_G(scheduler_cfg->queue_bit_mask, basic_scheduler_table_ptr, basic_scheduler_index);

    basic_scheduler_to_bbh_queue[dir][basic_scheduler_index] = scheduler_cfg->bbh_queue_index;

    return BDMF_ERR_OK;
}

#ifndef _CFE_REDUCED_XRDP_
/* API to rate limiter module */
bdmf_error_t rdd_basic_scheduler_rate_limiter_cfg(rdpa_traffic_dir dir, uint8_t basic_scheduler_index, uint8_t rate_limiter_index)
{
    uint32_t *basic_scheduler_table_ptr;
    bdmf_boolean cs_exist;

    RDD_BTRACE("dir = %d, basic_scheduler_index = %d, rate_limiter_index = %d\n",
        dir, basic_scheduler_index, rate_limiter_index);

    if ((basic_scheduler_index >= RDD_BASIC_SCHEDULER_TABLE_SIZE) ||
        (rate_limiter_index >= RDD_BASIC_RATE_LIMITER_TABLE_SIZE))
    {
        return BDMF_ERR_PARM;
    }

    if (dir == rdpa_dir_ds)
        basic_scheduler_table_ptr = RDD_BASIC_SCHEDULER_TABLE_DS_ADDRESS_ARR;
    else
        basic_scheduler_table_ptr = RDD_BASIC_SCHEDULER_TABLE_US_ADDRESS_ARR;

    RDD_BASIC_SCHEDULER_DESCRIPTOR_RATE_LIMITER_INDEX_WRITE_G(rate_limiter_index, basic_scheduler_table_ptr, basic_scheduler_index);
    RDD_BASIC_SCHEDULER_DESCRIPTOR_RATE_LIMIT_ENABLE_WRITE_G(1, basic_scheduler_table_ptr, basic_scheduler_index);
    RDD_BASIC_SCHEDULER_DESCRIPTOR_COMPLEX_SCHEDULER_EXISTS_READ_G(cs_exist, basic_scheduler_table_ptr, basic_scheduler_index);
    if (!cs_exist)
        RDD_BASIC_SCHEDULER_DESCRIPTOR_IS_POSITIVE_BUDGET_WRITE_G(1, basic_scheduler_table_ptr, basic_scheduler_index);

    return BDMF_ERR_OK;
}

bdmf_error_t rdd_basic_scheduler_rate_limiter_remove(rdpa_traffic_dir dir, uint8_t basic_scheduler_index)
{
    uint32_t *basic_scheduler_table_ptr;
    bdmf_boolean cs_exist;
    uint8_t cs_slot_index, cs_index;
    uint32_t budget_vector;
    bdmf_error_t rc = BDMF_ERR_OK;

    RDD_BTRACE("dir = %d, basic_scheduler_index = %d\n", dir, basic_scheduler_index);

    if (basic_scheduler_index >= RDD_BASIC_SCHEDULER_TABLE_SIZE)
    {
        return BDMF_ERR_PARM;
    }

    if (dir == rdpa_dir_ds)
        basic_scheduler_table_ptr = RDD_BASIC_SCHEDULER_TABLE_DS_ADDRESS_ARR;
    else
        basic_scheduler_table_ptr = RDD_BASIC_SCHEDULER_TABLE_US_ADDRESS_ARR;

    RDD_BASIC_SCHEDULER_DESCRIPTOR_RATE_LIMIT_ENABLE_WRITE_G(0, basic_scheduler_table_ptr, basic_scheduler_index);

    /* in case the bs is under cs make sure it has rate */
    RDD_BASIC_SCHEDULER_DESCRIPTOR_COMPLEX_SCHEDULER_EXISTS_READ_G(cs_exist, basic_scheduler_table_ptr, basic_scheduler_index);
    if (cs_exist)
    {
        RDD_BASIC_SCHEDULER_DESCRIPTOR_COMPLEX_SCHEDULER_SLOT_INDEX_READ_G(cs_slot_index, basic_scheduler_table_ptr, basic_scheduler_index);
        RDD_BASIC_SCHEDULER_DESCRIPTOR_COMPLEX_SCHEDULER_INDEX_READ_G(cs_index, basic_scheduler_table_ptr, basic_scheduler_index);
        if (dir == rdpa_dir_ds)
        {
            return BDMF_ERR_NOT_SUPPORTED;
        }
        else
        {
            rdd_ag_us_tm_complex_scheduler_table_slot_budget_bit_vector_0_get(cs_index, &budget_vector);
            budget_vector |= (1 << cs_slot_index);
            rdd_ag_us_tm_complex_scheduler_table_slot_budget_bit_vector_0_set(cs_index, budget_vector);
        }
    }
    else
        RDD_BASIC_SCHEDULER_DESCRIPTOR_IS_POSITIVE_BUDGET_WRITE_G(1, basic_scheduler_table_ptr, basic_scheduler_index);

    return rc;
}

/* API to queue */
bdmf_error_t rdd_basic_scheduler_rate_set(rdpa_traffic_dir dir, uint8_t basic_scheduler_index, uint8_t queue_bit_mask)
{
    uint32_t *basic_scheduler_table_ptr;
    uint8_t budget;

    RDD_BTRACE("dir = %d, basic_scheduler_index = %d, queue_bit_mask = %d\n",
        dir, basic_scheduler_index, queue_bit_mask);

    if (basic_scheduler_index >= RDD_BASIC_SCHEDULER_TABLE_SIZE)
    {
        return BDMF_ERR_PARM;
    }

    if (dir == rdpa_dir_ds)
        basic_scheduler_table_ptr = RDD_BASIC_SCHEDULER_TABLE_DS_ADDRESS_ARR;
    else
        basic_scheduler_table_ptr = RDD_BASIC_SCHEDULER_TABLE_US_ADDRESS_ARR;

    RDD_BASIC_SCHEDULER_DESCRIPTOR_SLOT_BUDGET_BIT_VECTOR_READ_G(budget, basic_scheduler_table_ptr, basic_scheduler_index);
    budget |= queue_bit_mask;
    RDD_BASIC_SCHEDULER_DESCRIPTOR_SLOT_BUDGET_BIT_VECTOR_WRITE_G(budget, basic_scheduler_table_ptr, basic_scheduler_index);

    return BDMF_ERR_OK;
}
#endif /* #ifndef _CFE_REDUCED_XRDP_ */

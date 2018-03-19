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

#ifndef _RDD_COMPLEX_SCHEDULER_H
#define _RDD_COMPLEX_SCHEDULER_H

#include "rdd.h"

#define COMPLEX_SCHEDULER

#define COMPLEX_SCHEDULER_NUM_OF_QUEUES       32
#define COMPLEX_SCHEDULER_FULL_BUDGET_VECTOR  0xffffffff

typedef enum
{
    complex_scheduler_full_dwrr = 0,
    complex_scheduler_2sp_30dwrr = 1,
    complex_scheduler_4sp_28dwrr = 2,
    complex_scheduler_8sp_24dwrr = 3,
    complex_scheduler_16sp_16dwrr = 4,
    complex_scheduler_full_sp = 5,
    complex_scheduler_num_of_dwrr_offset
} complex_scheduler_dwrr_offset_t;

typedef struct
{
    complex_scheduler_dwrr_offset_t dwrr_offset_sir;
    complex_scheduler_dwrr_offset_t dwrr_offset_pir;
    uint8_t bbh_queue_index;
    bdmf_boolean is_scheduler;
} complex_scheduler_cfg_t;
typedef struct
{
    uint8_t block_index; /* queue or basic scheduler index */
    uint8_t scheduler_slot_index; /* internal index for complex scheduler */
    uint8_t bs_dwrr_offset;
    quantum_number_t quantum_number;
} complex_scheduler_block_t;

/* API to RDPA level */
bdmf_error_t rdd_complex_scheduler_cfg(rdpa_traffic_dir dir, uint8_t complex_scheduler_index, complex_scheduler_cfg_t *cfg);
bdmf_error_t rdd_complex_scheduler_block_cfg(rdpa_traffic_dir dir, uint8_t complex_scheduler_index, complex_scheduler_block_t *block);
bdmf_error_t rdd_complex_scheduler_block_remove(rdpa_traffic_dir dir, uint8_t complex_scheduler_index, uint8_t scheduler_slot_index);
bdmf_error_t rdd_complex_scheduler_weight_cfg(rdpa_traffic_dir dir, uint8_t complex_scheduler_index, complex_scheduler_block_t *block);

/* API to block */
bdmf_error_t rdd_complex_scheduler_rate_set(rdpa_traffic_dir dir, uint8_t complex_scheduler_index, uint32_t block_bit_mask);

#endif

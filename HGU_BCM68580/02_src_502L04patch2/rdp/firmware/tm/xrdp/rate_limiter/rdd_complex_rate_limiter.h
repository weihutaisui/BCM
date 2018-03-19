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

#ifndef _RDD_COMPLEX_RATE_LIMITER_H
#define _RDD_COMPLEX_RATE_LIMITER_H

#include "rdd.h"

#define COMPLEX_RATE_LIMITER
#define COMPLEX_RATE_LIMITER_SIR_INIT_RATE          0
#define COMPLEX_RATE_LIMITER_PIR_INIT_RATE          0
#define MAX_INDEX_OF_COMPLEX_RL                     127

typedef enum
{
    RDD_RL_SIR,
    RDD_RL_PIR,
} rdd_complex_rl_bucket_t;

typedef enum
{
    rdd_complex_rl_queue = 0,
    rdd_complex_rl_basic_scheduler = 1,
    num_of_rdd_complex_rl_block = 2
} rdd_complex_rl_block_t;

typedef struct
{
    uint32_t sustain_budget;
    uint32_t peak_limit;
    uint32_t peak_rate;
    rdd_complex_rl_block_t type;
    uint8_t block_index;
} rdd_complex_rl_cfg_t;

/* API to RDPA level */
bdmf_error_t rdd_complex_rate_limiter_cfg(rdpa_traffic_dir dir, uint8_t rl_index, rdd_complex_rl_cfg_t *rl_cfg);
bdmf_error_t rdd_complex_rate_limiter_remove(rdpa_traffic_dir dir, uint8_t rl_index);

#endif

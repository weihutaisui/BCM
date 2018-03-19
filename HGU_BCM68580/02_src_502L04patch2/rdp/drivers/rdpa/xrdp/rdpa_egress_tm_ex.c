/*
* <:copyright-BRCM:2013-2015:proprietary:standard
*
*    Copyright (c) 2013-2015 Broadcom
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
 :>
*/
/*
 * rdpa_egress_tm_ex.c
 *
 * rdpa_egress_tm interface toward XRDP-specific RDD implementations.
 */
#include "bdmf_dev.h"
#include "rdpa_api.h"
#include "rdpa_common.h"
#include "rdpa_int.h"
#include "rdd.h"
#include "rdpa_egress_tm_inline.h"
#include "rdpa_egress_tm_ex.h"
#include "rdd_scheduling.h"
#include "rdd_basic_scheduler.h"
#include "rdd_complex_scheduler.h"
#include "rdd_basic_rate_limiter.h"
#include "rdd_complex_rate_limiter.h"
#include "rdd_overall_rate_limiter.h"
#include "rdp_drv_qm.h"
#include "rdp_drv_cnpl.h"
#include "rdp_drv_cntr.h"

extern dpi_params_t dpi_params;

/* Per-direction number of queues currently flushed */
static uint8_t flush_queue_cnt[2] = {0};


/* Per-direction rl usage bitmasks.
 * dual rate represented by two bits */
static uint64_t rdd_rl_free_mask[2][2];

/* Per-direction basic and complex scheduler usage bitmasks.
 * 0 bits represent schedulers in use, 1 represents FREE */
static uint64_t rdd_basic_schedulers_free_mask[2];
static uint64_t rdd_complex_schedulers_free_mask[2];

/* !@# temporary until the relevant constant is available */
#define RDD_COMPLEX_SCHEDULER__NUM_OF       RDD_COMPLEX_SCHEDULER_TABLE_SIZE
#define RDD_BASIC_SCHEDULER__NUM_OF         RDD_BASIC_SCHEDULER_TABLE_SIZE
#define RDD_BASIC_SCHED_ID_OFFSET           RDD_COMPLEX_SCHEDULER__NUM_OF
#define RDPA_SCHED_ID__MAX                  (RDD_COMPLEX_SCHEDULER__NUM_OF + RDD_BASIC_SCHEDULER__NUM_OF - 1)
#define RDPA_SCHED_ID_UNASSIGNED            (-1)
#define RDPA_RL_ID_UNASSIGNED               (-1)
#define RDPA_RL_FREE_MASK                   0xffffffffffffffff
#define RDPA_FLUSH_TIMEOUT		    0Xffff

/* weight quantum */
#define RDD_WEIGHT_QUANTUM                  256

#define QM_QUEUES__NUM_OF                   256         /* Number of QM queues managed by egress_tm */
#define QM_WRED_PROFILE__NUM_OF             13          /* Number of QM WRED profiles available for allocation. Profile 15 is reserved for drop */

#define WRED_THRESHOLD_RESOLUTION           64          /* WRED thresholds are in units of 64 bytes */

#define RDPA_NUM_QUEUES_IN_BASIC_SCHED      8           /* Max number of queues controlled by basic scheduler */

/*
 * Per queue structure holding the "current" configuration
 */
typedef struct
{
    tm_qtm_ctl_t *qtm_ctl;
    uint32_t queue_id;
} qm_queue_info_t;

static qm_queue_info_t qm_queue_info[QM_QUEUES__NUM_OF];
static uint32_t qm_num_queues[2];                       /* Number of queues allocated per direction */
static uint32_t qm_drop_counters[QM_QUEUES__NUM_OF] = {};

/*
 * Helper functions
 */
static uint16_t _rdpa_rdd_tx_queue_calc_total_weight(struct bdmf_object *mo, tm_qtm_ctl_t *qtm_ctl);
static bdmf_error_t _rdpa_rdd_tx_queue_reconfigure_all_wrr(struct bdmf_object *mo, tm_qtm_ctl_t *qtm_ctl, uint16_t total_weight);

static inline uint64_t mask_according_to_object_size_set(uint8_t object_size)
{
    int i;
    uint64_t dst_mask = 0;

    for (i = 0; i < object_size; i++)
    {
        dst_mask |= 1 << i;   
    }
    return dst_mask;
}

static void _rdpa_rdd_check_init(void)
{
    /* Called for the 1st time, initialize use_masks in accordance with
     * total number of available schedulers
     */
    if (rdd_basic_schedulers_free_mask[rdpa_dir_us] == 0)
    {
        rdd_basic_schedulers_free_mask[rdpa_dir_us] = mask_according_to_object_size_set(RDD_BASIC_SCHEDULER__NUM_OF);
        rdd_basic_schedulers_free_mask[rdpa_dir_ds] = mask_according_to_object_size_set(RDD_BASIC_SCHEDULER__NUM_OF);
        rdd_complex_schedulers_free_mask[rdpa_dir_us] = mask_according_to_object_size_set(RDD_COMPLEX_SCHEDULER__NUM_OF);
        rdd_complex_schedulers_free_mask[rdpa_dir_ds] = mask_according_to_object_size_set(RDD_COMPLEX_SCHEDULER__NUM_OF);
        rdd_rl_free_mask[0][rdpa_dir_ds] = RDPA_RL_FREE_MASK;
        rdd_rl_free_mask[0][rdpa_dir_us] = RDPA_RL_FREE_MASK;
        rdd_rl_free_mask[1][rdpa_dir_ds] = RDPA_RL_FREE_MASK;
        rdd_rl_free_mask[1][rdpa_dir_us] = RDPA_RL_FREE_MASK;
    }
}

/* scheduler id stored in tm_channel_t structure encodes both scheduler type and RDD scheduler id.
 * this function is responcible for mapping of rdpa_sched_id --> rdd scheduler type and id
 */
static void _rdpa_rdd_shed_id2sched_type_index(int16_t rdpa_sched_id, rdpa_rdd_sched_type_t *p_type, uint8_t *p_index)
{
    BUG_ON((unsigned)rdpa_sched_id > RDPA_SCHED_ID__MAX);
    if (rdpa_sched_id >= RDD_BASIC_SCHED_ID_OFFSET)
    {
        *p_type = RDD_SCHED_TYPE_BASIC;
        *p_index = rdpa_sched_id - RDD_BASIC_SCHED_ID_OFFSET;
    }
    else
    {
        *p_type = RDD_SCHED_TYPE_COMPLEX;
        *p_index = rdpa_sched_id;
    }
}

/* get upper index in case of tm with mode==disabled */
static uint16_t _rdpa_get_qtm_ctl_upper_index(struct bdmf_object *mo)
{
    int i;
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    tm_drv_priv_t *upper_tm = (tm_drv_priv_t *)bdmf_obj_data(tm->upper_level_tm);

    if (tm->mode == rdpa_tm_sched_disabled && upper_tm->upper_level_tm)
        return _rdpa_get_qtm_ctl_upper_index(tm->upper_level_tm);
    else
    {
        for (i = 0; i < RDPA_TM_MAX_SCHED_ELEMENTS; i++)
        {
            if (upper_tm->sub_tms[i] == mo)
                return i;
        }
    }
    return 0;
}

/* Map RDD scheduler type and id to rdpa scheduler id */
static void _rdpa_rdd_sched_type_index2sched_id(rdpa_rdd_sched_type_t type, uint8_t index, int16_t *p_rdpa_sched_id)
{
    if (type == RDD_SCHED_TYPE_BASIC)
    {
        BUG_ON(index >= RDD_BASIC_SCHEDULER__NUM_OF);
        *p_rdpa_sched_id = index + RDD_BASIC_SCHED_ID_OFFSET;
    }
    else
    {
        BUG_ON(index >= RDD_COMPLEX_SCHEDULER__NUM_OF);
        *p_rdpa_sched_id = index;
    }
}

#ifndef HAS_FFSLL
static int ffsll(uint64_t n)
{
    uint32_t n32;
    int i;
    n32 = n & 0xffffffff;
    i = ffs(n32);
    if (!i)
    {
        n32 = (n >> 32);
        i = ffs(n32);
        if (i)
            i += 32;
    }
    return i;
}
#endif

static int16_t ffsll2(uint64_t vector)
{
    int16_t i;

    for (i = 0; i < 64; i += 2)
    {
        if ((int)((vector >> i) & 0x3) == 0x3)
            return i+1;
    }

    return 0;
}

/* Allocate scheduler index */
static bdmf_error_t _rdpa_rdd_sched_alloc(rdpa_traffic_dir dir, rdpa_rdd_sched_type_t type, int16_t *p_rdpa_sched_id)
{
    uint64_t *p_mask = (type == RDD_SCHED_TYPE_BASIC) ?
        &rdd_basic_schedulers_free_mask[dir] : &rdd_complex_schedulers_free_mask[dir];
    int index = ffsll(*p_mask);

    /* ffsll returns 1-based bit index. 0=not found */
    if (!index)
        return BDMF_ERR_NORES;
    _rdpa_rdd_sched_type_index2sched_id(type, index-1, p_rdpa_sched_id);
    *p_mask &= ~(1ULL << (index - 1));

    return BDMF_ERR_OK;
}

/* Free scheduler index */
static void _rdpa_rdd_sched_free(rdpa_traffic_dir dir, int16_t rdpa_sched_id)
{
    rdpa_rdd_sched_type_t type;
    uint8_t index;
    _rdpa_rdd_shed_id2sched_type_index(rdpa_sched_id, &type, &index);
    if (type == RDD_SCHED_TYPE_BASIC)
        rdd_basic_schedulers_free_mask[dir] |= (1 << index);
    else
        rdd_complex_schedulers_free_mask[dir] |= (1 << index);
}

/* Map egress_tm parameters to dwrr_offset */
static bdmf_error_t _rdpa_rdd_map_wrr_offset(struct bdmf_object *mo, rdpa_rdd_sched_type_t sched_type,
    rdpa_tm_sched_mode mode, rdpa_tm_num_sp_elem num_sp_elements, uint8_t *p_dwrr_offset)
{
    bdmf_error_t err = BDMF_ERR_OK;

    if (sched_type == RDD_SCHED_TYPE_BASIC)
    {
        if (mode == rdpa_tm_sched_wrr)
            *p_dwrr_offset = basic_scheduler_full_dwrr;
        else if (mode == rdpa_tm_sched_sp_wrr)
        {
            switch (num_sp_elements)
            {
            case rdpa_tm_num_sp_elem_0:
                *p_dwrr_offset = basic_scheduler_full_dwrr;
                break;
            case rdpa_tm_num_sp_elem_2:
                *p_dwrr_offset = basic_scheduler_2sp_6dwrr;
                break;
            case rdpa_tm_num_sp_elem_4:
                *p_dwrr_offset = basic_scheduler_4sp_4dwrr;
                break;
            case rdpa_tm_num_sp_elem_8:
                *p_dwrr_offset = basic_scheduler_full_sp;
                break;
            default:
                BDMF_TRACE_ERR_OBJ(mo, "Unexpected num_sp_elements = %d for basic scheduler\n", num_sp_elements);
                err = BDMF_ERR_PARM;
            }
        }
        else
            *p_dwrr_offset = basic_scheduler_full_sp;
    }
    else
    {
        /* Complex scheduler */
        if (mode == rdpa_tm_sched_wrr)
            *p_dwrr_offset = complex_scheduler_full_dwrr;
        else if (mode == rdpa_tm_sched_sp_wrr)
        {
            switch (num_sp_elements)
            {
            case rdpa_tm_num_sp_elem_0:
                *p_dwrr_offset = complex_scheduler_full_dwrr;
                break;
            case rdpa_tm_num_sp_elem_2:
                *p_dwrr_offset = complex_scheduler_2sp_30dwrr;
                break;
            case rdpa_tm_num_sp_elem_4:
                *p_dwrr_offset = complex_scheduler_4sp_28dwrr;
                break;
            case rdpa_tm_num_sp_elem_8:
                *p_dwrr_offset = complex_scheduler_8sp_24dwrr;
                break;
            case rdpa_tm_num_sp_elem_16:
                *p_dwrr_offset = complex_scheduler_16sp_16dwrr;
                break;
            case rdpa_tm_num_sp_elem_32:
                *p_dwrr_offset = complex_scheduler_full_sp;
                break;
            default:
                BDMF_TRACE_ERR_OBJ(mo, "Unexpected num_sp_elements = %d for complex scheduler\n", num_sp_elements);
                err = BDMF_ERR_PARM;
            }
        }
        else
            *p_dwrr_offset = complex_scheduler_full_sp;
    }

    return err;
}

static quantum_number_t _rdpa_rdd_calc_quantum_number(uint16_t total_weight, uint16_t sched_weight)
{
    quantum_number_t quantum_number;

    if (!total_weight || !sched_weight || sched_weight == total_weight)
        quantum_number = RDD_WEIGHT_QUANTUM - 1;
    else
        quantum_number = (((sched_weight * RDD_WEIGHT_QUANTUM) + (total_weight / 2)) / total_weight);

    /* minimum value for quantum is 1 */
    quantum_number = quantum_number ? quantum_number : 1;

    return quantum_number;
}


/* Destroy scheduler.
 * For now just release the index. In th efuture we might
 * have to do more cleanups on RDD level
 */
static void _rdpa_rdd_sched_destroy(tm_channel_t *channel, int16_t *p_rdpa_sched_id)
{
    if (*p_rdpa_sched_id != RDPA_SCHED_ID_UNASSIGNED)
    {
        _rdpa_rdd_sched_free(channel->dir, *p_rdpa_sched_id);
        *p_rdpa_sched_id = RDPA_SCHED_ID_UNASSIGNED;
    }
}

/* Configure basic / complex scheduler */
static bdmf_error_t _rdpa_rdd_sched_create(struct bdmf_object *mo, tm_channel_t *channel,
    rdpa_rdd_sched_type_t sched_type, const rdd_sched_cfg_t *cfg, int index_in_upper,
    int16_t *p_sched_id, int16_t *p_sched_mode)
{
    uint8_t index;
    bdmf_error_t err = BDMF_ERR_OK;
    uint8_t bbh_queue = (channel->dir == rdpa_dir_us) ? _rdd_wan_channel(channel->channel_id) : channel->channel_id;
    uint8_t dwrr_offset;
#ifndef XRDP_BBH_PER_LAN_PORT
#if defined(BCM63158)
        /* Each gbe wan port has its own bbh which uses only bbh_queue 0 */
        if (rdpa_is_gbe_mode() && channel->dir == rdpa_dir_us)
            bbh_queue = 0;
#else
        if (rdpa_is_gbe_mode() && channel->dir == rdpa_dir_us)
            bbh_queue = rdpa_gbe_wan_emac();
#endif
#endif
    err = _rdpa_rdd_sched_alloc(channel->dir, sched_type, p_sched_id);
    BDMF_TRACE_DBG_OBJ(mo, "_rdpa_rdd_sched_alloc(%d, %d, %d) --> %s\n",
        channel->dir, sched_type, *p_sched_id, bdmf_strerror(err));
    if (err)
    {
        BDMF_TRACE_ERR_OBJ(mo, "Failed to allocate scheduler in dir %d\n", channel->dir);
        return err;
    }
    /* Recover index */
    _rdpa_rdd_shed_id2sched_type_index(*p_sched_id, &sched_type, &index);

    err = _rdpa_rdd_map_wrr_offset(mo, sched_type, cfg->mode, cfg->num_sp_elements, &dwrr_offset);
    if (err)
        return err;

    if (sched_type == RDD_SCHED_TYPE_BASIC)
    {
        /* Top-level basic scheduler or sub-scheduler ? */
        if (channel->sched_id == RDPA_SCHED_ID_UNASSIGNED)
        {
            basic_scheduler_cfg_t basic_scheduler_cfg =
            {
                .dwrr_offset = dwrr_offset,
                .bbh_queue_index = bbh_queue
            };
            err = rdd_basic_scheduler_cfg(channel->dir, index, &basic_scheduler_cfg);
            BDMF_TRACE_DBG_OBJ(mo, "rdd_basic_scheduler_cfg(%d, %d, %d, %u) --> %s\n",
                channel->dir, index, dwrr_offset, bbh_queue, bdmf_strerror(err));
        }
        else
        {
            /* Basic scheduler is under complex */
            complex_scheduler_block_t sched_block =
            {
                .block_index = index,
                .scheduler_slot_index = index_in_upper,
                .bs_dwrr_offset = dwrr_offset,
                .quantum_number = 1 /* Temporary value. Will be re-assigned further in configuration flow */
            };
            rdpa_rdd_sched_type_t upper_type;
            uint8_t upper_index;
            _rdpa_rdd_shed_id2sched_type_index(channel->sched_id, &upper_type, &upper_index);
            BUG_ON(upper_type != RDD_SCHED_TYPE_COMPLEX);
            err = rdd_complex_scheduler_block_cfg(channel->dir, upper_index, &sched_block);
            BDMF_TRACE_DBG_OBJ(mo, "rdd_complex_scheduler_block_cfg(%d, %d, {%u, %u, %u}) --> %s\n",
                channel->dir, upper_index,
                sched_block.block_index, sched_block.scheduler_slot_index, sched_block.quantum_number,
                bdmf_strerror(err));
        }
        *p_sched_mode = dwrr_offset;
    }
    else
    {
        /* complex scheduler */
        complex_scheduler_cfg_t complex_scheduler_cfg =
        {
            .dwrr_offset_sir = dwrr_offset,
            .dwrr_offset_pir = complex_scheduler_full_sp,
            .bbh_queue_index = bbh_queue,
            .is_scheduler = (cfg->level == rdpa_tm_level_egress_tm)
        };

        if (channel->rl_rate_mode == rdpa_tm_rl_dual_rate)
        {
            complex_scheduler_cfg.dwrr_offset_sir = complex_scheduler_full_sp;
            complex_scheduler_cfg.dwrr_offset_pir = dwrr_offset;
        }
        err = rdd_complex_scheduler_cfg(channel->dir, index, &complex_scheduler_cfg);
        BDMF_TRACE_DBG_OBJ(mo, "rdd_complex_scheduler_cfg(%d, %d, %d, %d) --> %s\n",
            channel->dir, index, complex_scheduler_cfg.dwrr_offset_sir, bbh_queue, bdmf_strerror(err));
        *p_sched_mode = complex_scheduler_cfg.dwrr_offset_sir;
    }

    if (err)
    {
        BDMF_TRACE_ERR_OBJ(mo, "Failed to configure scheduler in dir %d\n", channel->dir);
        _rdpa_rdd_sched_destroy(channel, p_sched_id);
    }

    return err;
}

/*
 * RL configuration
 */

/* Allocate rl index */
static bdmf_error_t _rdpa_rdd_rl_alloc(rdpa_traffic_dir dir, rdpa_tm_rl_rate_mode rate_mode, int16_t *p_rdpa_rc_id)
{
    uint64_t *p_mask;
    int vec, index = 0;

    if (rate_mode == rdpa_tm_rl_single_rate)
    {
        /* ffsll returns 1-based bit index. 0=not found */
        for (vec = 0; vec < 2; vec++)
        {
            p_mask = &rdd_rl_free_mask[vec][dir];
            index = ffsll(*p_mask);
            if (index)
                break;
        }
    }
    else
    {
        /* ffsll2 returns 1-based bit index of first two bits. 0=not found */
        for (vec = 0; vec < 2; vec++)
        {
            p_mask = &rdd_rl_free_mask[vec][dir];
            index = ffsll2(*p_mask);
            if (index)
                break;
        }
    }

    if (!index)
        return BDMF_ERR_NORES;
    index--;

    if (vec)
        *p_rdpa_rc_id = index + 64;
    else
        *p_rdpa_rc_id = index;

    *p_mask &= ~(1ULL << index);
    if (rate_mode == rdpa_tm_rl_dual_rate)
        *p_mask &= ~(1ULL << (index + 1));

    return BDMF_ERR_OK;
}

/* Free rl index */
static void _rdpa_rdd_rl_free(rdpa_traffic_dir dir, int16_t rdpa_rl_id, rdpa_tm_rl_rate_mode type)
{
    if (type == rdpa_tm_rl_single_rate)
    {
        rdd_basic_rate_limiter_remove(dir, rdpa_rl_id);
        if (rdpa_rl_id < 64)
            rdd_rl_free_mask[0][dir] |= (1 << rdpa_rl_id);
        else
            rdd_rl_free_mask[1][dir] |= (1 << rdpa_rl_id);
    }
    else
    {
        rdd_complex_rate_limiter_remove(dir, rdpa_rl_id);
        if (rdpa_rl_id < 64)
        {
            rdd_rl_free_mask[0][dir] |= (1 << rdpa_rl_id);
            rdd_rl_free_mask[0][dir] |= (1 << (rdpa_rl_id + 1));
        }
        else
        {
            rdd_rl_free_mask[1][dir] |= (1 << rdpa_rl_id);
            rdd_rl_free_mask[1][dir] |= (1 << (rdpa_rl_id + 1));
        }
    }
}

/* Destroy rate controller.
 * For now just release the index. In the future we might
 * have to do more cleanups on RDD level
 */
static void _rdpa_rdd_rl_destroy(rdpa_traffic_dir dir, rdpa_tm_rl_rate_mode type, int16_t *p_rdpa_rc_id)
{
    if (*p_rdpa_rc_id != RDPA_RL_ID_UNASSIGNED)
    {
        _rdpa_rdd_rl_free(dir, *p_rdpa_rc_id, type);
        *p_rdpa_rc_id = RDPA_RL_ID_UNASSIGNED;
    }
}

/* Configure basic / complex rate limiter */
static bdmf_error_t _rdpa_rdd_rl_configure(struct bdmf_object *mo, rdpa_traffic_dir dir, int16_t rdpa_rc_id,
    bdmf_boolean is_sched_level, int16_t block_index, rdpa_tm_rl_rate_mode rl_rate_mode, const rdpa_tm_rl_cfg_t *cfg)
{
    uint8_t block_t, index = 0;
    bdmf_error_t err = BDMF_ERR_OK;

    /* Determine rate controller type */
    if (!is_sched_level)
    {
        block_t = rdd_basic_rl_queue;
        index = block_index;    /* QM queue index */
    }
    else
    {
        rdpa_rdd_sched_type_t type = 0;
        /* block index contains sched_id. map to sched type and index */
        _rdpa_rdd_shed_id2sched_type_index(block_index, &type, &index);
        block_t = (type == RDD_SCHED_TYPE_BASIC) ? rdd_basic_rl_basic_scheduler : rdd_basic_rl_complex_scheduler;
        if ((block_t > rdd_basic_rl_basic_scheduler) && (rl_rate_mode == rdpa_tm_rl_dual_rate))
            return BDMF_ERR_NORES;
    }

    if (rl_rate_mode == rdpa_tm_rl_single_rate)
    {
        /* Basic rl */
        rdd_basic_rl_cfg_t rl_cfg =
        {
            .rate = cfg->af_rate / BITS_IN_BYTE,
            .limit = cfg->burst_size,
            .type = block_t,
            .block_index = index
        };
        err = rdd_basic_rate_limiter_cfg(dir, rdpa_rc_id, &rl_cfg);
        BDMF_TRACE_DBG_OBJ(mo, "rdd_basic_rate_limiter_cfg(%d, %d, {%u, %u, %u}) --> %s\n",
            dir, rdpa_rc_id, rl_cfg.rate, rl_cfg.type, rl_cfg.block_index, bdmf_strerror(err));
    }
    else
    {
        /* Complex rl */
        rdd_complex_rl_cfg_t rl_cfg =
        {
            .sustain_budget = cfg->af_rate / BITS_IN_BYTE,
            .peak_limit = cfg->burst_size / BITS_IN_BYTE,
            .peak_rate = cfg->be_rate / BITS_IN_BYTE,
            .type = block_t,
            .block_index = index
        };
        err = rdd_complex_rate_limiter_cfg(dir, rdpa_rc_id, &rl_cfg);
        BDMF_TRACE_DBG_OBJ(mo, "rdd_complex_rate_limiter_cfg(%d, %d, {%u, %u, %u, %u}) --> %s\n",
            dir, rdpa_rc_id, rl_cfg.sustain_budget, rl_cfg.peak_limit, rl_cfg.peak_rate,
            rl_cfg.block_index, bdmf_strerror(err));
    }

    return err;
}

/* Create and configure basic / complex rate limiter */
static bdmf_error_t _rdpa_rdd_rl_create(struct bdmf_object *mo, rdpa_traffic_dir dir, bdmf_boolean is_sched_level,
    int16_t block_index, rdpa_tm_rl_rate_mode rl_rate_mode, const rdpa_tm_rl_cfg_t *cfg, int16_t *p_rdpa_rc_id)
{
    bdmf_error_t err;

    /* Allocate RC index */
    err = _rdpa_rdd_rl_alloc(dir, rl_rate_mode, p_rdpa_rc_id);
    if (err)
        return err;

    /* Configure rate controller */
    err = _rdpa_rdd_rl_configure(mo, dir, *p_rdpa_rc_id, is_sched_level, block_index, rl_rate_mode, cfg);
    if (err)
        _rdpa_rdd_rl_destroy(dir, rl_rate_mode, p_rdpa_rc_id);

    return err;
}

/* Create / destroy / configure basic / complex rate limiter */
static bdmf_error_t _rdpa_rdd_rl_reconfigure(struct bdmf_object *mo, rdpa_traffic_dir dir,
    bdmf_boolean is_sched_level, int16_t block_index, rdpa_tm_rl_rate_mode rl_rate_mode,
    const rdpa_tm_rl_cfg_t *cfg, int16_t *p_rdpa_rc_id)
{
    bdmf_error_t err = BDMF_ERR_OK;

    if ((cfg->af_rate < RDD_RATE_UNLIMITED) && (cfg->af_rate > 0))
    {
        if (*p_rdpa_rc_id == RDPA_RL_ID_UNASSIGNED)
            err = _rdpa_rdd_rl_create(mo, dir, is_sched_level, block_index, rl_rate_mode, cfg, p_rdpa_rc_id);
        else
            err = _rdpa_rdd_rl_configure(mo, dir, *p_rdpa_rc_id, is_sched_level, block_index, rl_rate_mode, cfg);
    }
    else
    {
        _rdpa_rdd_rl_destroy(dir, rl_rate_mode, p_rdpa_rc_id);
    }

    return err;
}

/* Allocate QM queue index */
static bdmf_error_t _rdpa_rdd_queue_alloc(tm_channel_t *channel, tm_qtm_ctl_t *qtm_ctl, tm_queue_hash_entry_t *qentry, int *q_idx)
{
    port_drv_priv_t *port;
    int first;
    int last;
    int i;

    rdpa_system_init_cfg_t *sys_init_cfg = (rdpa_system_init_cfg_t *)_rdpa_system_init_cfg_get();

    port = (port_drv_priv_t *)bdmf_obj_data(channel->owner);

    if (channel->dir == rdpa_dir_us)
    {
        first = drv_qm_get_us_start();
        last = drv_qm_get_us_end();
    }
    else if (!sys_init_cfg->dpu_split_scheduling_mode)
    {
        first = drv_qm_get_ds_start();
        last = drv_qm_get_ds_end();
    }
    else
    {
        if (port->cfg.emac == rdpa_emac0 || port->cfg.emac == rdpa_emac1 || port->cfg.emac == rdpa_emac5)
        {
            first = drv_qm_get_ds_start();
            last = drv_qm_get_ds_start() + (drv_qm_get_ds_end() - drv_qm_get_ds_start()) / 2;
        }
        else
        {
            first = drv_qm_get_ds_start() + (drv_qm_get_ds_end() - drv_qm_get_ds_start()) / 2 + 1;
            last = drv_qm_get_ds_end();
        }
    }

    for (i = first; i <= last; i++)
    {
        if (qm_queue_info[i].qtm_ctl == NULL)
        {
            *q_idx = i;
            ++qm_num_queues[channel->dir];
            qm_queue_info[i].qtm_ctl = qtm_ctl;
            qm_queue_info[i].queue_id = qentry->queue_id;
            return BDMF_ERR_OK;
        }
    }
    return BDMF_ERR_NORES;
}

/* Release QM queue index */
static void _rdpa_rdd_queue_free(rdpa_traffic_dir dir, int q_idx)
{
    BUG_ON((unsigned)q_idx >= QM_QUEUES__NUM_OF);
    BUG_ON(qm_queue_info[q_idx].qtm_ctl == NULL);
    qm_queue_info[q_idx].qtm_ctl = NULL;
    qm_queue_info[q_idx].queue_id = (uint32_t)BDMF_INDEX_UNASSIGNED;
    --qm_num_queues[dir];
}

/* Re-configure weights and DWRR mode for all blocks of a complex scheduler */
static bdmf_error_t _rdpa_rdd_set_sched_weights(struct bdmf_object *mo, tm_channel_t *channel)
{
    uint8_t bbh_index = (channel->dir == rdpa_dir_us) ? _rdd_wan_channel(channel->channel_id) : channel->channel_id;
    tm_qtm_ctl_t *qtm_ctl;
    rdpa_rdd_sched_type_t sched_type, sub_type;
    uint8_t sched_index, sub_index;
    uint32_t total_weight = 0;
    bdmf_error_t err = 0;

#ifndef XRDP_BBH_PER_LAN_PORT
#if defined(BCM63158)
        /* Each gbe wan port has its own bbh which uses onluy bbh_queue 0 */
        if (rdpa_is_gbe_mode() && channel->dir == rdpa_dir_us)
            bbh_index = 0;
#else
        if (rdpa_is_gbe_mode() && channel->dir == rdpa_dir_us)
            bbh_index = rdpa_gbe_wan_emac();
#endif
#endif

    if (channel->sched_mode == complex_scheduler_full_sp)
        return BDMF_ERR_OK;

    /* Get RDD scheduler type and index */
    _rdpa_rdd_shed_id2sched_type_index(channel->sched_id, &sched_type, &sched_index);

    /* Go over all sub-schedulers */
    STAILQ_FOREACH(qtm_ctl, &channel->qtm_ctls, list)
    {
        if (qtm_ctl->sched_id == RDPA_SCHED_ID_UNASSIGNED)
            continue;
        total_weight += qtm_ctl->sched_weight;
    }

    STAILQ_FOREACH(qtm_ctl, &channel->qtm_ctls, list)
    {
        complex_scheduler_block_t sched_block = {};

        if (qtm_ctl->sched_id == RDPA_SCHED_ID_UNASSIGNED)
            continue;

        _rdpa_rdd_shed_id2sched_type_index(qtm_ctl->sched_id, &sub_type, &sub_index);
        sched_block.block_index = sub_index;
        sched_block.scheduler_slot_index = qtm_ctl->sched_index_in_upper;
        sched_block.quantum_number = _rdpa_rdd_calc_quantum_number(total_weight, qtm_ctl->sched_weight);
        sched_block.bs_dwrr_offset = qtm_ctl->sched_mode;
        err = rdd_complex_scheduler_block_cfg(channel->dir, sched_index, &sched_block);
        BDMF_TRACE_DBG_OBJ(mo, "rdd_complex_scheduler_block_cfg(%d, %d, {%d, %d, %d}) --> %s\n",
            channel->dir, sched_index,
            sched_block.block_index, sched_block.scheduler_slot_index, sched_block.quantum_number,
            bdmf_strerror(err));
    }

    if (!err)
    {
        complex_scheduler_cfg_t complex_scheduler_cfg =
        {
            .dwrr_offset_sir = channel->sched_mode,
            .dwrr_offset_pir = complex_scheduler_full_sp,
            .bbh_queue_index = bbh_index,
            .is_scheduler = 1,
        };
        if (channel->rl_rate_mode == rdpa_tm_rl_dual_rate)
        {
            complex_scheduler_cfg.dwrr_offset_sir = complex_scheduler_full_sp;
            complex_scheduler_cfg.dwrr_offset_pir = channel->sched_mode;
        }

        err = rdd_complex_scheduler_cfg(channel->dir, sched_index, &complex_scheduler_cfg);
        BDMF_TRACE_DBG_OBJ(mo, "rdd_complex_scheduler_cfg(%d, %d, %d, %d) --> %s\n",
            channel->dir, sched_index, channel->sched_mode, bbh_index, bdmf_strerror(err));
    }

    return err;
}


static bdmf_boolean is_wrr_elem(rdpa_tm_rl_rate_mode rate_mode, int16_t sched_id,
    int16_t sched_mode, int elem_index)
{
    rdpa_rdd_sched_type_t sched_type;
    uint8_t sched_index;
    bdmf_boolean is_wrr = 0;

    _rdpa_rdd_shed_id2sched_type_index(sched_id, &sched_type, &sched_index);

    if (sched_type == RDD_SCHED_TYPE_BASIC)
    {
        if (sched_mode == basic_scheduler_full_dwrr                             ||
            (sched_mode == basic_scheduler_2sp_6dwrr && elem_index >= 2)        ||
            (sched_mode == basic_scheduler_4sp_4dwrr && elem_index >= 4))
        {
            is_wrr = 1;
        }
    }
    else
    {
        if (sched_mode == complex_scheduler_full_dwrr                           ||
            (sched_mode == complex_scheduler_2sp_30dwrr && elem_index >= 2)     ||
            (sched_mode == complex_scheduler_4sp_28dwrr && elem_index >= 4)     ||
            (sched_mode == complex_scheduler_8sp_24dwrr && elem_index >= 8)     ||
            (sched_mode == complex_scheduler_16sp_16dwrr && elem_index >= 16))
        {
            is_wrr = 1;
        }
    }
    return is_wrr;
}

static int is_wan_aggregation_disable(void)
{
    rdpa_wan_type wan_type = rdpa_wan_if_to_wan_type(rdpa_wan_type_to_if(rdpa_wan_epon));
    /* NOTE : Using rdpa_wan_epon based on below code which is looking for epon only related stuff */
    
    return (rdpa_is_epon_ae_mode() && (rdpa_wan_speed_get(rdpa_wan_type_to_if(wan_type)) == rdpa_speed_1g)) || (wan_type == rdpa_wan_epon);
}

/* Set QM queue context */
static void _rdpa_qm_queue_ctx_set(const tm_channel_t *channel, int queue_index,
    const rdd_queue_cfg_t *queue_cfg, qm_q_context *qm_queue_ctx)
{
    /* Basic queue profile is pre-=set in data_path_init.
     * We only update WRED profile here
     */
    bdmf_error_t err;
#ifdef G9991
    bdmf_boolean is_control;
    port_drv_priv_t *port;
#endif

    /* Setup queue profile */
    err = ag_drv_qm_q_context_get(queue_index, qm_queue_ctx);
    BUG_ON(err != BDMF_ERR_OK);

    qm_queue_ctx->wred_profile = queue_cfg->profile;
    qm_queue_ctx->fec_enable = channel->attr.fec_overhead;
    qm_queue_ctx->sci = channel->attr.sci_overhead;
    qm_queue_ctx->q_802_1ae = channel->attr.q_802_1ae;

    /* US queue aggregation change */
    if (queue_index <= drv_qm_get_us_end())
    {
        qm_queue_ctx->aggregation_disable = is_wan_aggregation_disable();
        BDMF_TRACE_INFO("QM queue index %d - aggregation_disable %d\n", queue_index, qm_queue_ctx->aggregation_disable);
    }
#ifdef G9991
    else if ((queue_index >= drv_qm_get_ds_start()) && (queue_index <= drv_qm_get_ds_end()))
    {
        /* If DS control queue set as exclusive queue */
        port = (port_drv_priv_t *)bdmf_obj_data(channel->owner);
        
        if ((1 << port->cfg.emac) & dpi_params.g9991_port_vec)
        {
            /* vport should be retrieved by rdpa_port_rdpa_if_to_vport() But symmetry is kept to rdd_g9991_control_sid_set
               usage in rdpa_port_ex.c */
            err = rdd_g9991_is_control_port_get((port->index - rdpa_if_lan0), (port->cfg.emac - rdpa_emac0), &is_control);

            BUG_ON(err != BDMF_ERR_OK);

            if (is_control)
                qm_queue_ctx->exclusive_priority = 1;
            else
                qm_queue_ctx->exclusive_priority = 0;
        }    
        else 
            qm_queue_ctx->exclusive_priority = 0;
    }
#endif
}

bdmf_error_t rdpa_rdd_tx_queue_channel_attr_update(const channel_attr *attr, int queue_index)
{
    bdmf_error_t err;
    qm_q_context qm_queue_ctx;

    /* Setup queue profile */
    err = drv_qm_queue_get_config(queue_index, &qm_queue_ctx);
    if (err)
        return err;

    if ((qm_queue_ctx.fec_enable == attr->fec_overhead) && (qm_queue_ctx.sci == attr->sci_overhead) &&
        (qm_queue_ctx.q_802_1ae == attr->q_802_1ae))
        return BDMF_ERR_OK;

    qm_queue_ctx.fec_enable = attr->fec_overhead;
    qm_queue_ctx.sci = attr->sci_overhead;
    qm_queue_ctx.q_802_1ae = attr->q_802_1ae;

    err = drv_qm_queue_config(queue_index, &qm_queue_ctx);

    return err;
}

/*
 * rdpa_egress_tm_ex interface
 */

/*
 * Channel configuration
 */

/* Destroy top-level scheduler */
void rdpa_rdd_top_sched_destroy(struct bdmf_object *mo, tm_channel_t *channel)
{
    _rdpa_rdd_sched_destroy(channel, &channel->sched_id);
    _rdpa_rdd_rl_destroy(channel->dir, rdpa_tm_rl_single_rate, &channel->rc_id);
}

/* Create top-level scheduler */
bdmf_error_t rdpa_rdd_top_sched_create(struct bdmf_object *mo, tm_channel_t *channel, const rdd_sched_cfg_t *cfg)
{
    bdmf_error_t err = BDMF_ERR_OK;
    int16_t sched_id = RDPA_SCHED_ID_UNASSIGNED;
    rdpa_rdd_sched_type_t sched_type;

    _rdpa_rdd_check_init();

    channel->sched_id = RDPA_SCHED_ID_UNASSIGNED;
    channel->rc_id = RDPA_RL_ID_UNASSIGNED;

    /* We allocate complex scheduler if at least 1 of the following conditions is true
     * - egress_tm level (as opposed to q-level)
     * - more than 8 queues for q-level egress_tm
     * - rate_mode = dual
     */
    if (cfg->level == rdpa_tm_level_egress_tm                   ||
        cfg->num_queues > RDPA_NUM_QUEUES_IN_BASIC_SCHED        ||
        cfg->rl_rate_mode == rdpa_tm_rl_dual_rate)
    {
        sched_type = RDD_SCHED_TYPE_COMPLEX;
    }
    else
    {
        sched_type = RDD_SCHED_TYPE_BASIC;
    }

    err = _rdpa_rdd_sched_create(mo, channel, sched_type,
        cfg, RDPA_SCHED_ID_UNASSIGNED, &sched_id, &channel->sched_mode);
    if (err)
        return err;

    channel->sched_id = sched_id;
    channel->rl_rate_mode = cfg->rl_rate_mode;

    /* in DS need to allocate RL for LAN port if egress_tm is tm-level.
     * if it is q-level, rdpa_rdd_qtm_ctl_create is about to be called.
     */
    if (channel->dir == rdpa_dir_ds && cfg->level == rdpa_tm_level_egress_tm)
    {
        if (cfg->rl_rate_mode != rdpa_tm_rl_single_rate)
        {
            BDMF_TRACE_RET_OBJ(BDMF_ERR_NOT_SUPPORTED, mo, "DS dual_mode rate limiter is not supported\n");
        }
        if ((cfg->rl_cfg.af_rate < RDD_RATE_UNLIMITED) && (cfg->rl_cfg.af_rate > 0))
        {
            err = _rdpa_rdd_rl_create(mo, channel->dir, 1, channel->sched_id,
                rdpa_tm_rl_single_rate, &cfg->rl_cfg, &channel->rc_id);
            if (err)
            {
                rdpa_rdd_top_sched_destroy(mo, channel);
                return err;
            }
        }
    }

    return err;
}

/* Create & configure q-level TM */
bdmf_error_t rdpa_rdd_qtm_ctl_create(struct bdmf_object *mo, tm_qtm_ctl_t *qtm_ctl, const rdd_sched_cfg_t *cfg)
{
    tm_channel_t *channel = qtm_ctl->channel;
    bdmf_error_t err = BDMF_ERR_OK;
    bdmf_boolean is_wrr;
    uint16_t upper_idx = qtm_ctl->sched_index_in_upper;
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    if (tm->mode == rdpa_tm_sched_disabled && tm->upper_level_tm)
        upper_idx = _rdpa_get_qtm_ctl_upper_index(mo);

    BUG_ON(!qtm_ctl);
    if (!channel)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_INTERNAL, mo, "qm_ctl->channel isn't set\n");

    BUG_ON(qtm_ctl->rc_id != RDPA_RL_ID_UNASSIGNED);
    qtm_ctl->sched_id = RDPA_SCHED_ID_UNASSIGNED;

    /* Allocate basic scheduler if isn't allocated yet */
    if (qtm_ctl->egress_tm != channel->egress_tm)
    {
        if (cfg->num_queues > RDPA_NUM_QUEUES_IN_BASIC_SCHED)
        {
            BDMF_TRACE_RET_OBJ(BDMF_ERR_RANGE, mo,
                "2nd level egress_tm objects supports up to %u queues\n", RDPA_NUM_QUEUES_IN_BASIC_SCHED);
        }
        err = _rdpa_rdd_sched_create(mo, channel, RDD_SCHED_TYPE_BASIC,
            cfg, upper_idx, &qtm_ctl->sched_id, &qtm_ctl->sched_mode);
        if (err)
            return err;

        /* If upper-level complex scheduler is DWRR - recalculate all weights */
        is_wrr = is_wrr_elem(channel->rl_rate_mode, channel->sched_id, channel->sched_mode,
            upper_idx);
        qtm_ctl->sched_weight = cfg->weight;

        if (is_wrr)
        {
            err = _rdpa_rdd_set_sched_weights(mo, channel);
            if (err)
            {
                _rdpa_rdd_sched_destroy(channel, &qtm_ctl->sched_id);
                return err;
            }
        }
    }

    /* create / re-configure rl if necessary.
     * Assign it in the same structure where scheduler is allocated.
     * In the legacy configuration in 2-tier hierarchy, dual rate is configured
     * in top-level egress_tm rather than 2nd level egress_tm that actually performs
     * the rate limiting.
     * XRDP implementation supports both options: dual rate can be configured at
     * top level or at the 2nd level. The actual result should be the same
     */
    qtm_ctl->rl_rate_mode = (channel->rl_rate_mode == rdpa_tm_rl_dual_rate) ? rdpa_tm_rl_dual_rate : cfg->rl_rate_mode;

    if (qtm_ctl->sched_id == RDPA_SCHED_ID_UNASSIGNED)
    {
        err = _rdpa_rdd_rl_reconfigure(mo, channel->dir, 1, channel->sched_id,
            rdpa_tm_rl_single_rate, &cfg->rl_cfg, &channel->rc_id);
    }
    else
    {
        err = _rdpa_rdd_rl_reconfigure(mo, channel->dir, 1, qtm_ctl->sched_id,
            qtm_ctl->rl_rate_mode, &cfg->rl_cfg, &qtm_ctl->rc_id);
    }

    return err;
}

void rdpa_rdd_qtm_ctl_destroy(struct bdmf_object *mo, tm_qtm_ctl_t *qtm_ctl)
{
    tm_channel_t *channel = qtm_ctl->channel;

    BUG_ON(!qtm_ctl);
    if (!channel)
    {
        BDMF_TRACE_ERR_OBJ(mo, "qm_ctl->channel isn't set\n");
        return;
    }

    /* Release sched_id */
    if (qtm_ctl->sched_id != RDPA_SCHED_ID_UNASSIGNED)
    {
        rdpa_rdd_sched_type_t top_sched_type;
        uint8_t top_sched_index;

        /* Release in upper-level complex scheduler */
        _rdpa_rdd_shed_id2sched_type_index(channel->sched_id, &top_sched_type, &top_sched_index);
        if (top_sched_type == RDD_SCHED_TYPE_COMPLEX)
            rdd_complex_scheduler_block_remove(channel->dir, top_sched_index, qtm_ctl->sched_index_in_upper);
        else
            rdd_basic_scheduler_queue_remove(channel->dir, top_sched_index, qtm_ctl->sched_index_in_upper);

        /* Release scheduler bit */
        _rdpa_rdd_sched_destroy(channel, &qtm_ctl->sched_id);
    }

    /* release rate controller, if any */
    _rdpa_rdd_rl_destroy(channel->dir, qtm_ctl->rl_rate_mode, &qtm_ctl->rc_id);
}

bdmf_error_t rdpa_rdd_qtm_ctl_modify(struct bdmf_object *mo, tm_qtm_ctl_t *qtm_ctl,
    const rdpa_tm_rl_cfg_t *cfg, int weight, int num_sp_elements)
{
    bdmf_error_t err = BDMF_ERR_OK;
    tm_channel_t *channel = qtm_ctl->channel;
    uint16_t total_weight = 0;

    BUG_ON(!qtm_ctl);
    if (!channel)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_INTERNAL, mo, "qm_ctl->channel isn't set\n");

    /* Update weight if under complex */
    if (weight >= 0)
    {
        if (qtm_ctl->egress_tm != channel->egress_tm  &&
            qtm_ctl->sched_weight != weight)
        {
            qtm_ctl->sched_weight = weight;
            err = _rdpa_rdd_set_sched_weights(mo, qtm_ctl->channel);
            if (err)
                return err;
        }
    }

    /* Update RL.
     * It might be on qtm_ctl or channel level, depending on where scheduler is allocated
     */
    if (cfg)
    {
        if (qtm_ctl->sched_id == RDPA_SCHED_ID_UNASSIGNED)
        {
            err = _rdpa_rdd_rl_reconfigure(mo, channel->dir, 1, channel->sched_id,
                rdpa_tm_rl_single_rate, cfg, &channel->rc_id);
        }
        else
        {
            err = _rdpa_rdd_rl_reconfigure(mo, channel->dir, 1, qtm_ctl->sched_id,
                qtm_ctl->rl_rate_mode, cfg, &qtm_ctl->rc_id);
        }
    }

    /* Update number of SP elements */
    if (num_sp_elements >= 0)
    {
        int16_t sched_id;
        rdpa_rdd_sched_type_t sched_type;
        uint8_t index;
        int16_t *p_sched_mode;
        uint8_t dwrr_offset;

        if (qtm_ctl->sched_id == RDPA_SCHED_ID_UNASSIGNED)
        {
            sched_id = channel->sched_id;
            p_sched_mode = &channel->sched_mode;
        }
        else
        {
            sched_id = qtm_ctl->sched_id;
            p_sched_mode = &qtm_ctl->sched_mode;
        }

        /* Recover index */
        _rdpa_rdd_shed_id2sched_type_index(sched_id, &sched_type, &index);

        err = _rdpa_rdd_map_wrr_offset(mo, sched_type, rdpa_tm_sched_sp_wrr, num_sp_elements, &dwrr_offset);
        if (err)
            return err;

        if (dwrr_offset != *p_sched_mode)
        {
            if (sched_type == RDD_SCHED_TYPE_BASIC)
            {
                err = rdd_basic_scheduler_dwrr_offset_cfg(channel->dir, index, dwrr_offset);
                BDMF_TRACE_DBG_OBJ(mo, "rdd_basic_scheduler_dwrr_offset_cfg(%d, %d, %d) --> %s\n",
                    channel->dir, index, dwrr_offset, bdmf_strerror(err));
            }
            else
            {
                if (channel->dir == rdpa_dir_ds)
                {
                    return BDMF_ERR_NOT_SUPPORTED;
                    /*err = rdd_ag_ds_tm_complex_scheduler_table_dwrr_offset_sir_set(index, dwrr_offset);
                    err = err ? err : rdd_ag_ds_tm_complex_scheduler_table_dwrr_offset_pir_set(index, complex_scheduler_full_sp);*/
                }
                else
                {
                    err = rdd_ag_us_tm_complex_scheduler_table_dwrr_offset_sir_set(index, dwrr_offset);
                    err = err ? err : rdd_ag_us_tm_complex_scheduler_table_dwrr_offset_pir_set(index, complex_scheduler_full_sp);
                }
                BDMF_TRACE_DBG_OBJ(mo, "rdd_complex_scheduler_dwrr_offset_cfg(%d, %d, {%u, %u}) --> %s\n",
                    channel->dir, index, dwrr_offset, complex_scheduler_full_dwrr, bdmf_strerror(err));
            }

            if (!err)
            {
                *p_sched_mode = dwrr_offset;
                total_weight = _rdpa_rdd_tx_queue_calc_total_weight(mo, qtm_ctl);
                err = _rdpa_rdd_tx_queue_reconfigure_all_wrr(mo, qtm_ctl, total_weight);
            }                  
        }
    }

    return err;
}

/*
 * Queue management
 */

static uint16_t _rdpa_rdd_tx_queue_calc_total_weight(struct bdmf_object *mo, tm_qtm_ctl_t *qtm_ctl)
{
    tm_channel_t *channel = qtm_ctl->channel;
    int16_t sched_id = (qtm_ctl->sched_id != RDPA_SCHED_ID_UNASSIGNED) ? qtm_ctl->sched_id : channel->sched_id;
    int16_t sched_mode = (qtm_ctl->sched_id != RDPA_SCHED_ID_UNASSIGNED) ? qtm_ctl->sched_mode : channel->sched_mode;
    rdpa_rdd_sched_type_t sched_type;
    uint8_t sched_index;
    bdmf_boolean is_wrr_queue;
    uint16_t total_weight = 0;
    int i;

    _rdpa_rdd_shed_id2sched_type_index(sched_id, &sched_type, &sched_index);
    if ((sched_type == RDD_SCHED_TYPE_BASIC && sched_mode == basic_scheduler_full_sp) ||
        (sched_type == RDD_SCHED_TYPE_COMPLEX && sched_mode == complex_scheduler_full_sp))
    {
        return BDMF_ERR_OK;
    }

    for (i = 0; i < RDPA_MAX_EGRESS_QUEUES; i++)
    {
        tm_queue_hash_entry_t *qentry = &qtm_ctl->hash_entry[i];
        is_wrr_queue = is_wrr_elem(channel->rl_rate_mode, sched_id, sched_mode, qentry->queue_index);
        if (qentry->rdp_queue_index >= 0 && is_wrr_queue)
            total_weight += qentry->sched_weight;
    }

    return total_weight;
}

static bdmf_error_t _rdpa_rdd_tx_queue_bind(struct bdmf_object *mo, tm_queue_hash_entry_t *qentry, uint16_t total_weight, bdmf_boolean weight_reconfig)
{
    tm_qtm_ctl_t *qtm_ctl =  egress_tm_hash_entry_container(qentry);
    tm_channel_t *channel = qtm_ctl->channel;
    int16_t sched_id = (qtm_ctl->sched_id != RDPA_SCHED_ID_UNASSIGNED) ? qtm_ctl->sched_id : channel->sched_id;
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);

    rdpa_rdd_sched_type_t sched_type;
    uint8_t sched_index;
    quantum_number_t quantum_number;
    bdmf_error_t err;

    BUG_ON(sched_id == RDPA_SCHED_ID_UNASSIGNED);

    quantum_number = _rdpa_rdd_calc_quantum_number(total_weight, qentry->sched_weight);
    _rdpa_rdd_shed_id2sched_type_index(sched_id, &sched_type, &sched_index);
    if (sched_type == RDD_SCHED_TYPE_BASIC)
    {
        basic_scheduler_queue_t sched_queue_cfg =
        {
            .qm_queue_index = qentry->rdp_queue_index,
            .queue_scheduler_index = (tm->upper_level_tm && (tm->mode == rdpa_tm_sched_disabled)) ? qtm_ctl->sched_index_in_upper : qentry->queue_index,
            .quantum_number = quantum_number
        };
        err = rdd_basic_scheduler_queue_cfg(channel->dir, sched_index, &sched_queue_cfg);
        BDMF_TRACE_DBG_OBJ(mo, "rdd_basic_scheduler_queue_cfg(%d, %d, {%d, %d, %d}) -> %s\n",
            channel->dir, sched_index, sched_queue_cfg.qm_queue_index,
            sched_queue_cfg.queue_scheduler_index, sched_queue_cfg.quantum_number, bdmf_strerror(err));
    }
    else
    {
        complex_scheduler_block_t sched_queue_cfg =
        {
            .block_index = qentry->rdp_queue_index,
            .scheduler_slot_index = qentry->queue_index,
            .quantum_number = quantum_number
        };
        if (weight_reconfig)
            err = rdd_complex_scheduler_weight_cfg(channel->dir, sched_index, &sched_queue_cfg);
        else
        err = rdd_complex_scheduler_block_cfg(channel->dir, sched_index, &sched_queue_cfg);
        BDMF_TRACE_DBG_OBJ(mo, "rdd_complex_scheduler_block_cfg(%d, %d, {%d, %d, %d}) -> %s\n",
            channel->dir, sched_index, sched_queue_cfg.block_index,
            sched_queue_cfg.scheduler_slot_index, sched_queue_cfg.quantum_number, bdmf_strerror(err));
    }
    return err;
}

/* Reconfigure all queues. It is needed when adding new WRR queue or changing weight
 * of the existing WRR queue
 */
static bdmf_error_t _rdpa_rdd_tx_queue_reconfigure_all_wrr(struct bdmf_object *mo, tm_qtm_ctl_t *qtm_ctl, uint16_t total_weight)
{
    int i;
    bdmf_error_t err = BDMF_ERR_OK;
    for (i = 0; i < RDPA_MAX_EGRESS_QUEUES && !err; i++)
    {
        tm_queue_hash_entry_t *qentry = &qtm_ctl->hash_entry[i];
        if (qentry->rdp_queue_index >= 0 && qentry->sched_weight > 0)
        {
            err = _rdpa_rdd_tx_queue_bind(mo, qentry, total_weight, 1);
        }
    }
    return err;
}

static bdmf_error_t _rdpa_rdd_tx_queue_unbind(struct bdmf_object *mo, tm_queue_hash_entry_t *qentry)
{
    tm_qtm_ctl_t *qtm_ctl =  egress_tm_hash_entry_container(qentry);
    tm_channel_t *channel = qtm_ctl->channel;
    int16_t sched_id = (qtm_ctl->sched_id != RDPA_SCHED_ID_UNASSIGNED) ? qtm_ctl->sched_id : channel->sched_id;
    rdpa_rdd_sched_type_t sched_type;
    uint8_t sched_index;
    bdmf_error_t err;

    BUG_ON(sched_id == RDPA_SCHED_ID_UNASSIGNED);

    /* clear queue counter */
    rdpa_rdd_tx_queue_stat_clear(mo, qentry);

    _rdpa_rdd_shed_id2sched_type_index(sched_id, &sched_type, &sched_index);
    if (sched_type == RDD_SCHED_TYPE_BASIC)
    {
        err = rdd_basic_scheduler_queue_remove(channel->dir, sched_index, qentry->queue_index);
        BDMF_TRACE_DBG_OBJ(mo, "rdd_basic_scheduler_queue_remove(%d, %d, %d) -> %s\n",
            channel->dir, sched_index, qentry->queue_index, bdmf_strerror(err));
    }
    else
    {
        err = rdd_complex_scheduler_block_remove(channel->dir, sched_index, qentry->queue_index);
        BDMF_TRACE_DBG_OBJ(mo, "rdd_complex_scheduler_block_remove(%d, %d, %d) -> %s\n",
            channel->dir, sched_index, qentry->queue_index, bdmf_strerror(err));
    }

    return err;
}

static rdpa_tm_rl_rate_mode _rdpa_rdd_tx_queue_rate_mode_get(tm_qtm_ctl_t *qtm_ctl)
{
    tm_channel_t *channel = qtm_ctl->channel;
    rdpa_tm_rl_rate_mode rate_mode;

    /* In single egress-tm hierarchy (top egress_tm is queue level)
     * queue rate mode is inherited from the egress_tm.
     * In dual egress_tm hierarchy queue rate mode is always single-rate
     */
    if (qtm_ctl->egress_tm != channel->egress_tm)
        rate_mode = rdpa_tm_rl_single_rate;
    else
        rate_mode = qtm_ctl->rl_rate_mode;

    return rate_mode;
}

bdmf_error_t rdpa_rdd_tx_queue_create(struct bdmf_object *mo, tm_queue_hash_entry_t *qentry,
    const rdd_queue_cfg_t *queue_cfg, pd_offload_ddr_queue_t *ddr_cfg)
{
    tm_qtm_ctl_t *qtm_ctl =  egress_tm_hash_entry_container(qentry);
    tm_channel_t *channel = qtm_ctl->channel;
    int16_t sched_id = (qtm_ctl->sched_id != RDPA_SCHED_ID_UNASSIGNED) ? qtm_ctl->sched_id : channel->sched_id;
    int16_t sched_mode = (qtm_ctl->sched_id != RDPA_SCHED_ID_UNASSIGNED) ? qtm_ctl->sched_mode : channel->sched_mode;
    qm_q_context qm_queue_ctx = {};
    bdmf_boolean is_wrr_queue;
    uint16_t total_weight = 0;
    bdmf_error_t err;

    BUG_ON(!channel);

    if (!queue_cfg->packet_threshold)
        return BDMF_ERR_PARM;

    is_wrr_queue = is_wrr_elem(channel->rl_rate_mode, sched_id, sched_mode, qentry->queue_index);
    if (is_wrr_queue)
    {
        if (queue_cfg->weight < RDPA_MIN_WEIGHT || queue_cfg->weight > RDPA_MAX_WEIGHT)
        {
            BDMF_TRACE_RET_OBJ(BDMF_ERR_RANGE, mo, "WRR weight %u is out of range %d..%d\n",
                (unsigned)queue_cfg->weight, (unsigned)RDPA_MIN_WEIGHT, (unsigned)RDPA_MAX_WEIGHT);
        }
        qentry->sched_weight = queue_cfg->weight;
    }
    else
    {
        qentry->sched_weight = queue_cfg->weight;
    }

    /* Set XRDP internal fields in qentry */
    qentry->rc_id = RDPA_RL_ID_UNASSIGNED;

    /* Allocate QM queue */
    err = _rdpa_rdd_queue_alloc(channel, qtm_ctl, qentry, &qentry->rdp_queue_index);
    if (err)
        return err;

    do {
        _rdpa_qm_queue_ctx_set(channel, qentry->rdp_queue_index, queue_cfg, &qm_queue_ctx);
        rdd_set_queue_enable(qentry->rdp_queue_index, 1);

        if (is_wrr_queue)
            total_weight = _rdpa_rdd_tx_queue_calc_total_weight(mo, qtm_ctl);

        /* Bind queue to the scheduler */
        if (is_wrr_queue && qentry->sched_weight != total_weight)
        {
            err = _rdpa_rdd_tx_queue_bind(mo, qentry, total_weight, 0);
            if (!err)
            err = _rdpa_rdd_tx_queue_reconfigure_all_wrr(mo, qtm_ctl, total_weight);
        }
        else
            err = _rdpa_rdd_tx_queue_bind(mo, qentry, total_weight, 0);
        if (err)
            break;

        /* Create RL on queue if necessary */
        err = _rdpa_rdd_rl_reconfigure(mo, channel->dir, 0, qentry->rdp_queue_index,
            _rdpa_rdd_tx_queue_rate_mode_get(qtm_ctl), &queue_cfg->rl_cfg, &qentry->rc_id);
        if (err)
            break;

        err = drv_qm_queue_config(qentry->rdp_queue_index, &qm_queue_ctx);

        BDMF_TRACE_DBG_OBJ(mo, "drv_qm_queue_config(%d, 1, {profile=%u}) -> %s\n",
            qentry->rdp_queue_index, queue_cfg->profile, bdmf_strerror(err));

     } while (0);

    if (err)
    {
        /* Release queue index */
        _rdpa_rdd_queue_free(channel->dir, qentry->rdp_queue_index);
        qentry->rdp_queue_index = -1;
        return err;
    }

    return err;
}

bdmf_error_t rdpa_rdd_tx_queue_modify(struct bdmf_object *mo, tm_queue_hash_entry_t *qentry,
    const rdd_queue_cfg_t *queue_cfg, pd_offload_ddr_queue_t *ddr_cfg)
{
    tm_qtm_ctl_t *qtm_ctl =  egress_tm_hash_entry_container(qentry);
    tm_channel_t *channel = qtm_ctl->channel;
    qm_q_context qm_queue_ctx = {};
    bdmf_error_t err = 0;

    BUG_ON(!channel);

    /* We only support changing WRED profile and drop threshold */
    /* At this point WRED profile is already configured */
    /* Re-Configure QM queue */
    rdd_set_queue_enable(qentry->rdp_queue_index, 1);
    
    ag_drv_qm_q_context_get(qentry->rdp_queue_index, &qm_queue_ctx);
    if (qm_queue_ctx.wred_profile != queue_cfg->profile)
    {
        _rdpa_qm_queue_ctx_set(channel, qentry->rdp_queue_index, queue_cfg, &qm_queue_ctx);
        err = drv_qm_queue_config(qentry->rdp_queue_index, &qm_queue_ctx);
        BDMF_TRACE_DBG_OBJ(mo, "drv_qm_queue_config(%d, 1, {profile=%u}) -> %s\n",
            qentry->rdp_queue_index, queue_cfg->profile, bdmf_strerror(err));
    }
    if (err)
        return err;

    /* Reconfigure WRR weight is necessary */
    if (qentry->sched_weight != queue_cfg->weight)
    {
        uint16_t total_weight;

        if (queue_cfg->weight < RDPA_MIN_WEIGHT || queue_cfg->weight > RDPA_MAX_WEIGHT)
        {
            BDMF_TRACE_ERR_OBJ(mo, "WRR weight %u is out of range %d..%d\n",
                (unsigned)queue_cfg->weight, (unsigned)RDPA_MIN_WEIGHT, (unsigned)RDPA_MAX_WEIGHT);
            err = BDMF_ERR_RANGE;
        }

        qentry->sched_weight = queue_cfg->weight;
        total_weight = _rdpa_rdd_tx_queue_calc_total_weight(mo, qtm_ctl);
        if (qentry->sched_weight != total_weight)
            err = err ? err : _rdpa_rdd_tx_queue_reconfigure_all_wrr(mo, qtm_ctl, total_weight);
    }

    /* Create / destroy / reconfigure RL on queue */
    err = err ? err : _rdpa_rdd_rl_reconfigure(mo, channel->dir, 0, qentry->rdp_queue_index,
        _rdpa_rdd_tx_queue_rate_mode_get(qtm_ctl), &queue_cfg->rl_cfg, &qentry->rc_id);

    return err;
}

void rdpa_rdd_tx_queue_disable(struct bdmf_object *mo, tm_queue_hash_entry_t *qentry, pd_offload_ddr_queue_t *ddr_cfg)
{
    qm_q_context qm_queue_ctx = {};

    ag_drv_qm_q_context_get(qentry->rdp_queue_index, &qm_queue_ctx);
    qm_queue_ctx.wred_profile = QM_WRED_PROFILE_DROP_ALL;
    ag_drv_qm_q_context_set(qentry->rdp_queue_index, &qm_queue_ctx);


    /* Flush the QM queue */
    rdd_set_queue_enable(qentry->rdp_queue_index, 0);
    rdpa_rdd_tx_queue_flush(mo, qentry, 1);
}

void rdpa_rdd_tx_queue_destroy(struct bdmf_object *mo, tm_queue_hash_entry_t *qentry, pd_offload_ddr_queue_t *ddr_cfg)
{
    tm_qtm_ctl_t *qtm_ctl =  egress_tm_hash_entry_container(qentry);
    tm_channel_t *channel = qtm_ctl->channel;

    qm_q_context qm_queue_ctx = {};

    BUG_ON(!channel);

    ag_drv_qm_q_context_get(qentry->rdp_queue_index, &qm_queue_ctx);
    qm_queue_ctx.wred_profile = QM_WRED_PROFILE_DROP_ALL;
    ag_drv_qm_q_context_set(qentry->rdp_queue_index, &qm_queue_ctx);

    /* Destroy rate controller if any */
    _rdpa_rdd_rl_destroy(channel->dir, _rdpa_rdd_tx_queue_rate_mode_get(qtm_ctl), &qentry->rc_id);

    /* Detach from scheduler and release */
    _rdpa_rdd_tx_queue_unbind(mo, qentry);
    _rdpa_rdd_queue_free(channel->dir, qentry->rdp_queue_index);

    /* Flush the QM queue */
    rdd_set_queue_enable(qentry->rdp_queue_index, 0);
    rdpa_rdd_tx_queue_flush(mo, qentry, 1);

    qentry->rdp_queue_index = -1;
    if (qentry->sched_weight)
    {
        uint16_t total_weight;
        qentry->sched_weight = 0;
        total_weight = _rdpa_rdd_tx_queue_calc_total_weight(mo, qtm_ctl);
        if (total_weight)
            _rdpa_rdd_tx_queue_reconfigure_all_wrr(mo, qtm_ctl, total_weight);
    }
}

bdmf_error_t rdpa_rdd_tx_queue_flush(struct bdmf_object *mo, tm_queue_hash_entry_t *qentry,
    bdmf_boolean is_wait)
{
    qm_q_context qm_queue_ctx;
    uint8_t wred_profile;
    uint32_t cnt = 0, flush_vector, timeout = 0, core_index;
    bdmf_error_t err;

    /* Setup queue profile */
    err = ag_drv_qm_q_context_get(qentry->rdp_queue_index, &qm_queue_ctx);
    BUG_ON(err != BDMF_ERR_OK);
    wred_profile = qm_queue_ctx.wred_profile;
    qm_queue_ctx.wred_profile = QM_WRED_PROFILE_DROP_ALL;
    ag_drv_qm_q_context_set(qentry->rdp_queue_index, &qm_queue_ctx);

    /* Activate flush in fw */
    if (egress_tm_get_queue_dir(qentry->dir_channel) == rdpa_dir_ds)
    {
        rdd_ag_ds_tm_scheduling_flush_vector_get(((qentry->rdp_queue_index - drv_qm_get_ds_start()) >> 5), &flush_vector);
        flush_vector |= (1 << ((qentry->rdp_queue_index - drv_qm_get_ds_start()) & 0x1f));
        rdd_ag_ds_tm_scheduling_flush_vector_set(((qentry->rdp_queue_index - drv_qm_get_ds_start()) >> 5), flush_vector);
        flush_queue_cnt[rdpa_dir_ds]++;
        rdd_ag_ds_tm_scheduling_global_flush_cfg_set(1);
        WMB();
        for (core_index = 0; core_index < GROUPED_EN_SEGMENTS_NUM; core_index++)
            if (IS_DS_TM_RUNNER_IMAGE(core_index))
                ag_drv_rnr_regs_cfg_cpu_wakeup_set(core_index, DS_TM_UPDATE_FIFO_THREAD_NUMBER);
    }
    else
    {
        rdd_ag_us_tm_scheduling_flush_vector_get((qentry->rdp_queue_index >> 5), &flush_vector);
        flush_vector |= (1 << (qentry->rdp_queue_index & 0x1f));
        rdd_ag_us_tm_scheduling_flush_vector_set((qentry->rdp_queue_index >> 5), flush_vector);
        flush_queue_cnt[rdpa_dir_us]++;
        rdd_ag_us_tm_scheduling_global_flush_cfg_set(1);
        WMB();
        ag_drv_rnr_regs_cfg_cpu_wakeup_set(get_runner_idx(us_tm_runner_image), US_TM_UPDATE_FIFO_THREAD_NUMBER);
    }

    /* Wait until queue drains */
    do
    {
        timeout++;
        ag_drv_qm_total_valid_cnt_get(qentry->rdp_queue_index * (sizeof(RDD_QM_QUEUE_COUNTER_DATA_DTS) / 4), &cnt);
    } while ((cnt != 0) && (timeout < RDPA_FLUSH_TIMEOUT));

    /* Check if other queues are flushed */
    if (egress_tm_get_queue_dir(qentry->dir_channel) == rdpa_dir_ds)
    {
        flush_queue_cnt[rdpa_dir_ds]--;
        if (!flush_queue_cnt[rdpa_dir_ds])
            rdd_ag_ds_tm_scheduling_global_flush_cfg_set(0);
        rdd_ag_ds_tm_scheduling_flush_vector_get(((qentry->rdp_queue_index - drv_qm_get_ds_start()) >> 5), &flush_vector);
        flush_vector &= ~(1 << ((qentry->rdp_queue_index - drv_qm_get_ds_start()) & 0x1f));
        rdd_ag_ds_tm_scheduling_flush_vector_set(((qentry->rdp_queue_index - drv_qm_get_ds_start()) >> 5), flush_vector);
    }
    else
    {
        flush_queue_cnt[rdpa_dir_us]--;
        if (!flush_queue_cnt[rdpa_dir_us])
            rdd_ag_us_tm_scheduling_global_flush_cfg_set(0);
        rdd_ag_us_tm_scheduling_flush_vector_get((qentry->rdp_queue_index >> 5), &flush_vector);
        flush_vector &= ~(1 << (qentry->rdp_queue_index & 0x1f));
        rdd_ag_us_tm_scheduling_flush_vector_set((qentry->rdp_queue_index >> 5), flush_vector);
    }

    /* Restore WRED profile */
    qm_queue_ctx.wred_profile = wred_profile;
    ag_drv_qm_q_context_set(qentry->rdp_queue_index, &qm_queue_ctx);
    if (timeout >= RDPA_FLUSH_TIMEOUT)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_INTERNAL, mo, "flush not done for egress queue %d\n", qentry->rdp_queue_index);

    return err;
}

bdmf_error_t rdpa_rdd_tx_queue_stat_read(struct bdmf_object *mo, tm_queue_hash_entry_t *qentry, rdpa_stat_1way_t *stat)
{
    int counter_index = qentry->rdp_queue_index * 2;
    bdmf_error_t err = BDMF_ERR_OK;
    uint32_t cntr_arr[MAX_NUM_OF_COUNTERS_PER_READ] = {};

    memset(stat, 0, sizeof(rdpa_stat_1way_t));

    /* VALID counters */
    if (egress_tm_get_queue_dir(qentry->dir_channel) == rdpa_dir_ds)
        err = drv_cntr_counter_read(DS_TX_QUEUE_CNTR_GROUP_ID, qentry->rdp_queue_index - drv_qm_get_ds_start(), cntr_arr);
    else
        err = drv_cntr_counter_read(US_TX_QUEUE_CNTR_GROUP_ID, qentry->rdp_queue_index, cntr_arr);
    stat->passed.packets = cntr_arr[0];
    stat->passed.bytes = cntr_arr[1];
    if (err)
        return err;

    err = ag_drv_qm_drop_counter_get(counter_index, &stat->discarded.packets);
    err = err ? err : ag_drv_qm_drop_counter_get(counter_index+1, &stat->discarded.bytes);

    return err;
}

bdmf_error_t rdpa_rdd_tx_queue_stat_clear(struct bdmf_object *mo, tm_queue_hash_entry_t *qentry)
{
    int counter_index = qentry->rdp_queue_index * 2;
    qm_drop_counters_ctrl drop_ctrl;
    bdmf_boolean read_clear;
    uint32_t cnt;

    /* Clear DROP counters
     * - set read-clear mode
     * - read packet and byte counters
     * - restore read-clear mode
     */
    ag_drv_qm_drop_counters_ctrl_get(&drop_ctrl);
    read_clear = drop_ctrl.read_clear_bytes;
    drop_ctrl.read_clear_bytes = 1;
    drop_ctrl.read_clear_pkts = 1;
    ag_drv_qm_drop_counters_ctrl_set(&drop_ctrl);
    ag_drv_qm_drop_counter_get(counter_index, &cnt);
    ag_drv_qm_drop_counter_get(counter_index+1, &cnt);
    drop_ctrl.read_clear_bytes = read_clear;
    drop_ctrl.read_clear_pkts = read_clear;
    ag_drv_qm_drop_counters_ctrl_set(&drop_ctrl);

    /* clear counter */
    if (egress_tm_get_queue_dir(qentry->dir_channel) == rdpa_dir_ds)
        drv_cntr_counter_clr(DS_TX_QUEUE_CNTR_GROUP_ID, qentry->rdp_queue_index - drv_qm_get_ds_start());
    else
        drv_cntr_counter_clr(US_TX_QUEUE_CNTR_GROUP_ID, qentry->rdp_queue_index);
    qm_drop_counters[qentry->rdp_queue_index] = 0;

    return BDMF_ERR_OK;
}


/* Get egress_tm object that owns the queue and queue_id given QM queue id */
bdmf_error_t rdpa_rdd_tx_queue_info_get(int rdd_queue_index, struct bdmf_object **p_owner, uint32_t *queue_id)
{
    tm_qtm_ctl_t *qtm_ctl;
    struct bdmf_object *owner;

    if ((unsigned)rdd_queue_index >= QM_QUEUES__NUM_OF)
        return BDMF_ERR_RANGE;

    qtm_ctl = qm_queue_info[rdd_queue_index].qtm_ctl;
    if (qtm_ctl == NULL)
        return BDMF_ERR_NOENT;

    owner = qtm_ctl->egress_tm;
    /* Paranoia check for the case qtm_ctl was destroyed by another thread */
    if (owner == NULL)
        return BDMF_ERR_NOENT;

    /* Get owner of the top-level egress_tm */
    owner = owner->owner;
    while (owner && owner->drv == rdpa_egress_tm_drv())
        owner = owner->owner;

    *queue_id = qm_queue_info[rdd_queue_index].queue_id;
    *p_owner = owner;

    return BDMF_ERR_OK;
}

/*
 * WRED support
 */

/* Convert thresholds to slope and mantissa
 *
 * drop_probability[7:0] =
 *   (mantissa[7:0] * (queue_occupancy[29:0] - min_threshold[23:0]*64)) >> exponent[4:0]
 *
 *   Calculation -
 *   (1) exponent set to max integer smaller than 
 *         log((max_threshold - min_threshold) * RDPA_WRED_MAX_DROP_PROBABILITY / max_drop_probability)
 *   (2) mantissa = 256 * 2^exponent / (max_threshold - min_threshold)
 */
static void _rdpa_rdd_wred_calc_slope(uint32_t min_threshold, uint32_t max_threshold,
    uint32_t max_drop_probability, uint8_t *mantissa, uint8_t *exponent)
{
    uint32_t i, delta = (max_threshold - min_threshold) * RDPA_WRED_MAX_DROP_PROBABILITY / max_drop_probability;

    if (delta < 2)
    {
        *mantissa = *exponent = 0;
        return;
    }

    for (i = 0; (1 << (i + 1)) < delta; i++)
        ;

    *exponent = i;
    *mantissa = (RDPA_WRED_MAX_DROP_PROBABILITY << *exponent) / delta;
}

static bdmf_error_t _rdpa_wred_profile_cfg(uint8_t profile_id, rdd_queue_profile_t *queue_profile)
{
    qm_wred_profile_cfg qm_wred_profile_cfg = {
        .min_thr0 = queue_profile->low_priority_class.min_threshold / WRED_THRESHOLD_RESOLUTION,
        .min_thr1 = queue_profile->high_priority_class.min_threshold / WRED_THRESHOLD_RESOLUTION,
        .max_thr0 = queue_profile->low_priority_class.max_threshold / WRED_THRESHOLD_RESOLUTION,
        .max_thr1 = queue_profile->high_priority_class.max_threshold / WRED_THRESHOLD_RESOLUTION,
    };
    bdmf_error_t err;

    /* Configure QM WRED profile */
    _rdpa_rdd_wred_calc_slope(queue_profile->low_priority_class.min_threshold, queue_profile->low_priority_class.max_threshold,
        queue_profile->low_priority_class.max_drop_probability, &qm_wred_profile_cfg.slope_mantissa0, &qm_wred_profile_cfg.slope_exp0);
    _rdpa_rdd_wred_calc_slope(queue_profile->high_priority_class.min_threshold, queue_profile->high_priority_class.max_threshold,
        queue_profile->high_priority_class.max_drop_probability, &qm_wred_profile_cfg.slope_mantissa1, &qm_wred_profile_cfg.slope_exp1);

    err = ag_drv_qm_wred_profile_cfg_set(profile_id, &qm_wred_profile_cfg);

    return err;
}

static rdpa_tm_wred_profile wred_profile[QM_WRED_PROFILE__NUM_OF];

/* [Allocate and] configure queue WRED profile */
bdmf_error_t rdpa_rdd_tm_queue_profile_cfg(struct bdmf_object *mo, rdpa_traffic_dir dir,
    const rdpa_tm_queue_cfg_t *old_queue_cfg, rdpa_tm_queue_cfg_t *new_queue_cfg,
    bdmf_boolean is_new, rdd_queue_profile_id_t *profile)
{
    rdd_queue_profile_t rdd_profile = {};
    int i;
    bdmf_boolean no_change = 0;
    uint32_t new_profile_index = QM_WRED_PROFILE__NUM_OF;

    /* first remove the old configuration if exist */
    /* For XRDP we WRED profile is assigned even for tail-drop queues */
    if (!is_new)
    {
        if (!rdpa_rdd_wred_param_modified(old_queue_cfg, new_queue_cfg))
            no_change = 1;
        else
            rdpa_rdd_tm_queue_profile_free(mo, dir, old_queue_cfg);
    }

    /* For XRDP we need WRED profiles even for tail-drop queues */
    if (new_queue_cfg->drop_alg == rdpa_tm_drop_alg_dt)
    {
        /* All thresholds are the same in case of tail-drop */
        new_queue_cfg->low_class.min_threshold = new_queue_cfg->drop_threshold;
        new_queue_cfg->low_class.max_threshold = new_queue_cfg->drop_threshold;
        new_queue_cfg->high_class.min_threshold = new_queue_cfg->drop_threshold;
        new_queue_cfg->high_class.max_threshold = new_queue_cfg->drop_threshold;
    }

    /* find exist profile or allocate new one*/
    for (i = 0; i < QM_WRED_PROFILE__NUM_OF; i++)
    {
        if (!wred_profile[i].attached_q_num)
        {
            new_profile_index = i; /*save empty profile index in case we need to allocate new one */
        }

        /* look for existing profile */
        if (wred_profile[i].drop_threshold == new_queue_cfg->drop_threshold &&
            wred_profile[i].low_class.min_threshold == new_queue_cfg->low_class.min_threshold &&
            wred_profile[i].low_class.max_threshold == new_queue_cfg->low_class.max_threshold &&
            wred_profile[i].low_class.max_drop_probability == new_queue_cfg->low_class.max_drop_probability &&
            wred_profile[i].high_class.min_threshold == new_queue_cfg->high_class.min_threshold &&
            wred_profile[i].high_class.max_threshold == new_queue_cfg->high_class.max_threshold &&
            wred_profile[i].high_class.max_drop_probability == new_queue_cfg->high_class.max_drop_probability)
        {
            /* same configuration just save the profile*/
            if (!no_change)
                wred_profile[i].attached_q_num++;

            *profile = i;
            BDMF_TRACE_DBG_OBJ(mo,
                "Set queue to existing profile dir %s index %d."
                "low threshold min %d low threshold max %d low max drop prob %d "
                "high threshold min %d high threshold max %d high max drop prob %d "
                "drop threshold %d attached queues %d\n", dir ? "us" : "ds", i,
                wred_profile[i].low_class.min_threshold,
                wred_profile[i].low_class.max_threshold,
                wred_profile[i].low_class.max_drop_probability,
                wred_profile[i].high_class.min_threshold,
                wred_profile[i].high_class.max_threshold,
                wred_profile[i].high_class.max_drop_probability,
                wred_profile[i].drop_threshold,
                wred_profile[i].attached_q_num);
            return 0;
        }
    }

    if (new_profile_index == QM_WRED_PROFILE__NUM_OF)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_NO_MORE, mo, "No available profiles left\n");

    if (new_queue_cfg->drop_alg == rdpa_tm_drop_alg_reserved)
    {
        rdd_profile.us_flow_control_mode = 1; /*, queues with this profile will work as flow control and NOT WRED */
        rdd_profile.high_priority_class.min_threshold = new_queue_cfg->high_class.min_threshold;
        rdd_profile.high_priority_class.max_threshold = new_queue_cfg->high_class.max_threshold;
    }
    else
    {
        rdd_profile.us_flow_control_mode = 0;
        rdd_profile.low_priority_class.min_threshold = new_queue_cfg->low_class.min_threshold;
        rdd_profile.low_priority_class.max_threshold = new_queue_cfg->low_class.max_threshold;
        rdd_profile.low_priority_class.max_drop_probability = new_queue_cfg->low_class.max_drop_probability ? 
            new_queue_cfg->low_class.max_drop_probability : RDPA_WRED_MAX_DROP_PROBABILITY;
        rdd_profile.high_priority_class.min_threshold = new_queue_cfg->high_class.min_threshold;
        rdd_profile.high_priority_class.max_threshold = new_queue_cfg->high_class.max_threshold;
        rdd_profile.high_priority_class.max_drop_probability = new_queue_cfg->high_class.max_drop_probability ? 
            new_queue_cfg->high_class.max_drop_probability : RDPA_WRED_MAX_DROP_PROBABILITY;
    }

    /* Configure in QM */
    _rdpa_wred_profile_cfg(new_profile_index, &rdd_profile);

    wred_profile[new_profile_index].drop_threshold = new_queue_cfg->drop_threshold;
    wred_profile[new_profile_index].high_class.min_threshold = new_queue_cfg->high_class.min_threshold;
    wred_profile[new_profile_index].high_class.max_threshold = new_queue_cfg->high_class.max_threshold;
    wred_profile[new_profile_index].high_class.max_drop_probability = new_queue_cfg->high_class.max_drop_probability;
    wred_profile[new_profile_index].low_class.min_threshold = new_queue_cfg->low_class.min_threshold;
    wred_profile[new_profile_index].low_class.max_threshold = new_queue_cfg->low_class.max_threshold;
    wred_profile[new_profile_index].low_class.max_drop_probability = new_queue_cfg->low_class.max_drop_probability;
    wred_profile[new_profile_index].attached_q_num++;

    *profile = new_profile_index;

    BDMF_TRACE_DBG_OBJ(mo, "Set queue to new profile dir %s index %d."
        "low threshold min %d low threshold max %d low max drop prob %d "
        "high threshold min %d high threshold max %d high max drop prob %d "
        "drop threshold %d attached queues %d\n", dir ? "us" : "ds", new_profile_index,
        wred_profile[new_profile_index].low_class.min_threshold,
        wred_profile[new_profile_index].low_class.max_threshold,
        wred_profile[new_profile_index].low_class.max_drop_probability,
        wred_profile[new_profile_index].high_class.min_threshold,
        wred_profile[new_profile_index].high_class.max_threshold,
        wred_profile[new_profile_index].high_class.max_drop_probability,
        wred_profile[new_profile_index].drop_threshold,
        wred_profile[new_profile_index].attached_q_num);

    return 0;
}

/* Unreference queue WRED profile */
bdmf_error_t rdpa_rdd_tm_queue_profile_free(struct bdmf_object *mo, rdpa_traffic_dir dir,
    const rdpa_tm_queue_cfg_t *queue_cfg)
{
    int i;

    for (i = 0; i < QM_WRED_PROFILE__NUM_OF; i++)
    {
        if (wred_profile[i].attached_q_num &&
            wred_profile[i].drop_threshold == queue_cfg->drop_threshold &&
            wred_profile[i].low_class.min_threshold == queue_cfg->low_class.min_threshold &&
            wred_profile[i].low_class.max_threshold == queue_cfg->low_class.max_threshold &&
            wred_profile[i].low_class.max_drop_probability == queue_cfg->low_class.max_drop_probability &&
            wred_profile[i].high_class.min_threshold == queue_cfg->high_class.min_threshold &&
            wred_profile[i].high_class.max_threshold == queue_cfg->high_class.max_threshold &&
            wred_profile[i].high_class.max_drop_probability == queue_cfg->high_class.max_drop_probability)
            break;
    }

    if (i == QM_WRED_PROFILE__NUM_OF)
    {
        BDMF_TRACE_DBG_OBJ(mo, "Can't find profile\n");
        return BDMF_ERR_INTERNAL;
    }
    wred_profile[i].attached_q_num--;

    /* no queues using this profile - remove it */
    if (!wred_profile[i].attached_q_num)
    {
        wred_profile[i].drop_threshold = 0;
        wred_profile[i].low_class.min_threshold = 0;
        wred_profile[i].low_class.max_threshold = 0;
        wred_profile[i].low_class.max_drop_probability = RDPA_WRED_MAX_DROP_PROBABILITY;
        wred_profile[i].high_class.min_threshold = 0;
        wred_profile[i].high_class.max_threshold = 0;
        wred_profile[i].high_class.max_drop_probability = RDPA_WRED_MAX_DROP_PROBABILITY;
    }

    BDMF_TRACE_DBG_OBJ(mo, "removed queue from profile dir %s index %d. "
        "remaining attached queues %d\n", dir ? "us" : "ds", i,
        wred_profile[i].attached_q_num);

    return 0;
}

/*
 * Overall rate limiter support
 */

/* Add / remove channel to overlall rate limiter */
bdmf_error_t rdpa_rdd_orl_channel_cfg(struct bdmf_object *mo, const tm_channel_t *channel,
    bdmf_boolean rate_limiter_enabled, rdpa_tm_orl_prty prio)
{
    uint8_t bbh_queue = (channel->dir == rdpa_dir_us) ? _rdd_wan_channel(channel->channel_id) : channel->channel_id;
    bdmf_error_t rc;

#ifndef XRDP_BBH_PER_LAN_PORT
#if defined(BCM63158)
        /* Each gbe wan port has its own bbh which uses onluy bbh_queue 0 */
        if (rdpa_is_gbe_mode() && channel->dir == rdpa_dir_us)
            bbh_queue = 0;
#else
        if (rdpa_is_gbe_mode() && channel->dir == rdpa_dir_us)
            bbh_queue = rdpa_gbe_wan_emac();
#endif
#endif

    if (rate_limiter_enabled)
    {
        rc = rdd_overall_rate_limiter_bbh_queue_cfg(bbh_queue, (prio == rdpa_tm_orl_prty_high));
        BDMF_TRACE_RET_OBJ(rc, mo, "rdd_overall_rate_limiter_bbh_queue_cfg(%u, %d) -> %d\n",
            bbh_queue, prio == rdpa_tm_orl_prty_high, rc);
    }
    else
    {
        rc = rdd_overall_rate_limiter_remove(bbh_queue);
        BDMF_TRACE_RET_OBJ(rc, mo, "rdd_overall_rate_limiter_remove(%u) -> %d\n", bbh_queue, rc);
    }
}

/* Configure overall rate limiter rate */
bdmf_error_t rdpa_rdd_orl_rate_cfg(struct bdmf_object *mo, uint32_t rate)
{
    bdmf_error_t rc;

    rc = rdd_overall_rate_limiter_rate_cfg(rate, rate);
    BDMF_TRACE_RET_OBJ(rc, mo, "rdd_overall_rate_limiter_rate_cfg({rate=%d, limit=%d}})\n",
        (int)rate, (int)rate);
}

int egress_tm_attr_service_queue_stat_read_ex(tm_drv_priv_t *tm, rdpa_stat_1way_t *stat)
{
    /* Service queues are not implemented in XRDP */
    return BDMF_ERR_NOT_SUPPORTED;
}


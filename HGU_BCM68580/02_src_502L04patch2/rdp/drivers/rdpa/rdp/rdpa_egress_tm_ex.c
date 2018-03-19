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
 * rdpa_egress_tm interface toward RDP-specific RDD implementations.
 */
#include "bdmf_dev.h"
#include "rdpa_api.h"
#include "rdpa_common.h"
#include "rdd.h"
#include "rdd_tm.h"
#include "rdpa_egress_tm.h"
#include "rdpa_egress_tm_inline.h"
#include "rdpa_egress_tm_ex.h"
#ifndef WL4908
#include "rdd_legacy_conv.h"
#endif
#include "rdd_service_queues.h"

/* max number of queues per RC */
#define RDD_MAX_QUEUES_PER_RC   8

static int num_us_rc; /* total number of US rate controllers in use */

/*
 * Channel configuration
 */
bdmf_error_t rdpa_rdd_top_sched_create(struct bdmf_object *mo, tm_channel_t *channel, const rdd_sched_cfg_t *cfg)
{
    int rdd_rc;

    if (channel->dir == rdpa_dir_us)
    {
        rdd_wan_channel_schedule_t rdd_sched_type;
        /* retain backward compatibility */
        rdd_peak_schedule_mode_t peak_sched_mode = RDD_PEAK_SCHEDULE_MODE_ROUND_ROBIN;

        if (cfg->level == rdpa_tm_level_egress_tm || cfg->rl_cfg.af_rate)
            rdd_sched_type = RDD_WAN_CHANNEL_SCHEDULE_RATE_CONTROL;
        else
            rdd_sched_type = RDD_WAN_CHANNEL_SCHEDULE_PRIORITY;

        /* SP will be used between dual-rate TMs */
        if (cfg->rl_rate_mode == rdpa_tm_rl_dual_rate)
            peak_sched_mode = RDD_PEAK_SCHEDULE_MODE_STRICT_PRIORITY;
        rdd_rc = rdd_wan_channel_cfg(_rdd_wan_channel(channel->channel_id), rdd_sched_type, peak_sched_mode);
        BDMF_TRACE_DBG_OBJ(mo, "rdd_wan_channel_cfg(%d, %d, %d) --> %d\n",
            channel->channel_id, rdd_sched_type, peak_sched_mode, rdd_rc);

        if (rdd_rc)
            return BDMF_ERR_INTERNAL;
    }

    return BDMF_ERR_OK;
}

void rdpa_rdd_top_sched_destroy(struct bdmf_object *mo, tm_channel_t *channel)
{
}

/*
 * Rate controllers, rate limiter
 */

static bdmf_error_t _rdpa_rdd_rate_ctl_alloc(struct bdmf_object *mo, tm_qtm_ctl_t *qtm_ctl)
{
    tm_channel_t *channel = qtm_ctl->channel;

    if (channel->dir == rdpa_dir_us)
    {
        int max_rcs;
        int unused_rc;

        if (num_us_rc >= RDPA_MAX_US_RATE_CONTROLLERS)
        {
            BDMF_TRACE_RET_OBJ(BDMF_ERR_NORES, mo,
                "No more unused US rate controllers (%d are in use)\n",
                RDPA_MAX_US_RATE_CONTROLLERS);
        }

        /* Assign max number of RCs per channel based on channel id (RDD restrictions) */
    #if !defined(WL4908)
        if (_rdd_wan_channel(channel->channel_id) <= RDD_WAN_CHANNEL_7)
    #else
        if (_rdd_wan_channel(channel->channel_id) <= RDD_WAN_CHANNEL_1)
    #endif
            max_rcs = 32;
        else
            max_rcs = 4;

        unused_rc = ffs(~channel->rc_mask); /* 1-based index of 1st 0 bit */
        if (!unused_rc || unused_rc > max_rcs)
        {
            BDMF_TRACE_RET_OBJ(BDMF_ERR_NORES, mo, "%d rate controllers on channel %d are already in use\n",
                max_rcs, channel->channel_id);
        }
        qtm_ctl->rc_id = unused_rc - 1; /* rc_id is 0-based, unused_rc is 1-based */
        ++num_us_rc;
    }
    else
    {
        /* Only 1 RC per channel is supported */
        if (channel->rc_mask)
            BDMF_TRACE_RET_OBJ(BDMF_ERR_INTERNAL, mo, "DS rate controller %d is already allocated\n", channel->channel_id);

        /* high-numbered ports in G999.1 do not support RC, and are mapped to RATE_LIMITER_IDLE */
        qtm_ctl->rc_id = (channel->channel_id >= RDD_RATE_LIMITER_DISABLED) ?
            RDD_RATE_LIMITER_DISABLED : RDD_RATE_LIMITER_PORT_0 + channel->channel_id;
    }
    channel->rc_mask |= 1 << qtm_ctl->rc_id;

    return BDMF_ERR_OK;
}

static void _rdpa_rdd_rate_ctl_free(struct bdmf_object *mo, tm_qtm_ctl_t *qtm_ctl)
{
    tm_channel_t *channel = qtm_ctl->channel;

    if (qtm_ctl->rc_id < 0)
        return;

    channel->rc_mask &= ~(1 << qtm_ctl->rc_id);
    if (channel->dir == rdpa_dir_us)
        --num_us_rc;

    qtm_ctl->rc_id = -1;
}

/* Create & configure q-level TM */
bdmf_error_t rdpa_rdd_qtm_ctl_create(struct bdmf_object *mo, tm_qtm_ctl_t *qtm_ctl, const rdd_sched_cfg_t *cfg)
{
    tm_channel_t *channel = qtm_ctl->channel;
    int rdd_rc;
    bdmf_error_t err = BDMF_ERR_INTERNAL;

    BUG_ON(!qtm_ctl);
    if (!channel)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_INTERNAL, mo, "qm_ctl->channel isn't set\n");

    BUG_ON(qtm_ctl->rc_id != -1);

    /* At this point qtm_ctl->rc_id must be -1 - unassigned yet */
    err = _rdpa_rdd_rate_ctl_alloc(mo, qtm_ctl);
    if (err)
        return err;

    if (channel->dir == rdpa_dir_us)
    {
        rdd_rate_cntrl_params_t rc_params = {};
        int16_t channel_id = _rdd_wan_channel(channel->channel_id);
        rdd_rc = rdd_rate_cntrl_cfg(channel_id, qtm_ctl->rc_id, &rc_params);
        BDMF_TRACE_DBG_OBJ(mo, "rdd_rate_cntrl_cfg(%d, %d, {0}) --> %d\n",
            (int)channel_id, (int)qtm_ctl->rc_id, rdd_rc);
        if (!rdd_rc)
            err = BDMF_ERR_OK;
    }

    if (err)
        _rdpa_rdd_rate_ctl_free(mo, qtm_ctl);

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

    if (channel->dir == rdpa_dir_us)
    {
        int16_t channel_id = _rdd_wan_channel(channel->channel_id);
        rdd_rate_cntrl_remove(channel_id, qtm_ctl->rc_id);
        BDMF_TRACE_DBG_OBJ(mo, "rdd_rate_cntrl_remove(%d, %d)\n",
            (int)channel_id, (int)qtm_ctl->rc_id);
    }

    /* Release rate controller id */
    _rdpa_rdd_rate_ctl_free(mo, qtm_ctl);
}

bdmf_error_t rdpa_rdd_qtm_ctl_modify(struct bdmf_object *mo, tm_qtm_ctl_t *qtm_ctl,
    const rdpa_tm_rl_cfg_t *cfg, int weight, int num_sp_elements)
{
    rdd_rate_cntrl_params_t rc_params = {};
    bdmf_error_t err = BDMF_ERR_INTERNAL;
    tm_channel_t *channel = qtm_ctl->channel;
    bdmf_boolean is_wrr = (weight != 0);
    int rdd_rc;

    BUG_ON(!qtm_ctl);
    if (!channel)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_INTERNAL, mo, "qm_ctl->channel isn't set\n");
    if (!cfg || weight < 0)
    {
        BDMF_TRACE_RET_OBJ(BDMF_ERR_INTERNAL, mo, "cfg (%p) isn't set or weight (%d) < 0\n",
            cfg, weight);
    }

    if (channel->dir == rdpa_dir_us)
    {
        int16_t channel_id = _rdd_wan_channel(channel->channel_id);
        if (is_wrr)
        {
            rc_params.peak_budget.rate = cfg->af_rate / BITS_IN_BYTE;
            rc_params.peak_budget.limit = cfg->burst_size / RDD_RATE_QUANTUM;
            rc_params.peak_weight = weight * RDD_WEIGHT_QUANTUM;
        }
        else
        {
            rc_params.sustain_budget = cfg->af_rate / BITS_IN_BYTE;
            /* required by FW, although in SP mode */
            rc_params.peak_weight = RDD_MAX_QUEUES_PER_RC * RDD_WEIGHT_QUANTUM;
            if (qtm_ctl->egress_tm != channel->egress_tm && channel->rl_rate_mode == rdpa_tm_rl_dual_rate)
            {
                rc_params.peak_budget.rate = cfg->be_rate / BITS_IN_BYTE; /* In bytes/s */
                rc_params.peak_budget.limit = cfg->burst_size / RDD_RATE_QUANTUM;
            }
        }
        rdd_rc = rdd_rate_cntrl_modify(channel_id, qtm_ctl->rc_id, &rc_params);
        BDMF_TRACE_DBG_OBJ(mo, "rdd_rate_cntrl_modify(%d, %d, {peak={r=%d, l=%d}, peak_w=%d, sust=%d)}) -> %d\n",
            (int)channel_id, (int)qtm_ctl->rc_id,
            (int)rc_params.peak_budget.rate, (int)rc_params.peak_budget.limit,
            (int)rc_params.peak_weight, (int)rc_params.sustain_budget, rdd_rc);
        if (!rdd_rc)
            err = BDMF_ERR_OK;
    }
    else
    {
        /* DS */
        if (cfg->af_rate != RDD_RATE_UNLIMITED)
        {
            rc_params.peak_budget.rate = cfg->af_rate / BITS_IN_BYTE; /* In bytes/s */
            rc_params.peak_budget.limit = MAX(cfg->burst_size, _rdpa_system_cfg_get()->mtu_size) / 1000;
        }
        else
        {
            rc_params.peak_budget.rate = 0;
            rc_params.peak_budget.limit = 0;
        }
        if (rc_params.peak_budget.rate)
        {
            rdd_rc = rdd_emac_rate_limiter_cfg(qtm_ctl->rc_id, &rc_params.peak_budget);
            BDMF_TRACE_DBG_OBJ(mo, "rdd_emac_rate_limiter_cfg(%d, {rate=%d, limit=%d}) -> %d\n",
                (int)qtm_ctl->rc_id, (int)rc_params.peak_budget.rate, (int)rc_params.peak_budget.limit, rdd_rc);
            if (rdd_rc)
                return BDMF_ERR_INTERNAL;
#ifndef LEGACY_RDP
            rdd_rc = rdd_lan_vport_cfg(channel->channel_id, qtm_ctl->rc_id);
            BDMF_TRACE_DBG_OBJ(mo, "rdd_lan_vport_cfg(%d, %d) -> %d\n",
                            (int)channel->channel_id, (int)qtm_ctl->rc_id, rdd_rc);
#else
            rdd_rc = rdd_emac_config(channel->channel_id, qtm_ctl->rc_id, 0);
            BDMF_TRACE_DBG_OBJ(mo, "rdd_emac_config(%d, %d) -> %d\n",
                (int)channel->channel_id, (int)qtm_ctl->rc_id, rdd_rc);
#endif
        }
        else
        {
#ifndef LEGACY_RDP
            rdd_rc = rdd_lan_vport_cfg(channel->channel_id, RDD_RATE_LIMITER_DISABLED);
            BDMF_TRACE_DBG_OBJ(mo, "rdd_lan_vport_cfg(%d, %d) -> %d\n",
                (int)channel->channel_id, RDD_RATE_LIMITER_DISABLED, rdd_rc);
#else
            rdd_rc = rdd_emac_config(channel->channel_id, RDD_RATE_LIMITER_DISABLED, 0);
            BDMF_TRACE_DBG_OBJ(mo, "rdd_emac_config(%d, %d) -> %d\n",
                (int)channel->channel_id, RDD_RATE_LIMITER_DISABLED, rdd_rc);
#endif
        }
        if (!rdd_rc)
            err = BDMF_ERR_OK;
    }
    return err;
}


/*
 * Queue management
 */

static bdmf_error_t rdpa_rdd_tx_ddr_queue_cfg(struct bdmf_object *mo, tm_qtm_ctl_t *qtm_ctl,
    tm_queue_hash_entry_t *qentry, const rdd_queue_cfg_t *queue_cfg, pd_offload_ddr_queue_t *ddr_cfg)
{
    tm_channel_t *channel = qtm_ctl->channel;
#if !defined(BDMF_SYSTEM_SIM)
    void *ddr_queue_aligned_addr;
#endif
    bdmf_error_t err = BDMF_ERR_OK;

    if (!rdpa_is_ddr_offload_enable(channel->dir))
        return BDMF_ERR_OK;

    if (queue_cfg->packet_threshold == ddr_cfg->size)
        return BDMF_ERR_OK;

#if !defined(BDMF_SYSTEM_SIM)
    if (channel->dir == rdpa_dir_us)
    {
        int16_t channel_id = _rdd_wan_channel(channel->channel_id);
        /* US */

        /* Flush and 0-size queue if called from new/modify. If called from remove,
         * it is done outside
         */
        if (queue_cfg->packet_threshold)
        {
            rdd_wan_tx_queue_modify(channel_id, qtm_ctl->rc_id, qentry->queue_index,
                0, RDD_QUEUE_PROFILE_DISABLED, queue_cfg->counter_id);
            rdd_wan_tx_queue_flush(channel_id, qtm_ctl->rc_id, qentry->queue_index, 1);
        }

        if (ddr_cfg->addr)
        {
            bdmf_free(ddr_cfg->addr);
            ddr_cfg->addr = NULL;
            ddr_cfg->size = 0;
            rdd_wan_tx_ddr_queue_addr_config(channel_id, qtm_ctl->rc_id, qentry->queue_index,
                0, 0, queue_cfg->counter_id);
        }
        if (queue_cfg->packet_threshold)
        {
            ddr_queue_aligned_addr = ddr_queue_alloc(queue_cfg->packet_threshold, &ddr_cfg->addr);
            if (ddr_queue_aligned_addr == NULL)
                err = BDMF_ERR_NOMEM;
            else
            {
                rdd_wan_tx_ddr_queue_addr_config(channel_id, qtm_ctl->rc_id, qentry->queue_index,
                    (uint32_t)VIRT_TO_PHYS(ddr_queue_aligned_addr), queue_cfg->packet_threshold,
                    queue_cfg->counter_id);
                BDMF_TRACE_DBG_OBJ(mo, "rdd_wan_tx_ddr_queue_addr_config(%d, %d, %d, 0x%x, %d, %d)\n",
                    (int)channel_id, (int)qtm_ctl->rc_id, (int)qentry->queue_index,
                    (uint32_t)VIRT_TO_PHYS(ddr_queue_aligned_addr),
                    (int)queue_cfg->packet_threshold, (int)queue_cfg->counter_id);
                ddr_cfg->size = queue_cfg->packet_threshold;
            }
        }
    }
    else
    {
        /* DS */
        if ((queue_cfg->packet_threshold > ddr_cfg->size) || !queue_cfg->packet_threshold)
        {
#ifndef LEGACY_RDP
            rdd_lan_vport_tx_queue_cfg(channel->channel_id, qentry->queue_index, 0, RDD_QUEUE_PROFILE_DISABLED);
#else
            rdd_eth_tx_queue_cfg(channel->channel_id, qentry->queue_index, 0, RDD_QUEUE_PROFILE_DISABLED, qtm_ctl->counter_id[qentry->queue_index]);
#endif
            rdd_eth_tx_queue_flush(channel->channel_id, qentry->queue_index, 1);
            if (ddr_cfg->addr)
            {
                /* G9991 flow */
                bdmf_free(ddr_cfg->addr);
                ddr_cfg->addr = NULL;
                ddr_cfg->size = 0;
                rdd_eth_tx_ddr_queue_addr_cfg(channel->channel_id, qentry->queue_index, 0, 0, queue_cfg->counter_id);
            }

            if (queue_cfg->packet_threshold)
            {
                ddr_queue_aligned_addr = ddr_queue_alloc(queue_cfg->packet_threshold, &ddr_cfg->addr);
                if (ddr_queue_aligned_addr == NULL)
                    err = BDMF_ERR_NOMEM;
                else
                {
                    rdd_eth_tx_ddr_queue_addr_cfg(channel->channel_id, qentry->queue_index,
                        (uint32_t)VIRT_TO_PHYS(ddr_queue_aligned_addr), queue_cfg->packet_threshold, queue_cfg->counter_id);
                    ddr_cfg->size = queue_cfg->packet_threshold;
                }
            }
        }
    }
#endif

    return err;
}

bdmf_error_t rdpa_rdd_tx_queue_channel_attr_update(const channel_attr *attr, int queue_index)
{
    return BDMF_ERR_OK;
}

bdmf_error_t rdpa_rdd_tx_queue_create(struct bdmf_object *mo, tm_queue_hash_entry_t *qentry,
    const rdd_queue_cfg_t *queue_cfg, pd_offload_ddr_queue_t *ddr_cfg)
{
    tm_qtm_ctl_t *qtm_ctl =  egress_tm_hash_entry_container(qentry);
    tm_channel_t *channel = qtm_ctl->channel;
    bdmf_error_t err = BDMF_ERR_INTERNAL;
    int rdd_rc;

    BUG_ON(!channel);

    if (queue_cfg->rl_cfg.af_rate || queue_cfg->rl_cfg.be_rate)
    {
        BDMF_TRACE_RET_OBJ(BDMF_ERR_NOT_SUPPORTED, mo,
            "Queue-level rate configuration is not supported\n");
    }

    qentry->rdp_queue_index = qentry->queue_index;
    if (channel->dir == rdpa_dir_us)
    {
        int16_t channel_id = _rdd_wan_channel(channel->channel_id);
        BUG_ON(qtm_ctl->rc_id < 0);
        rdd_rc = rdd_wan_tx_queue_cfg(channel_id, qtm_ctl->rc_id, qentry->queue_index,
            queue_cfg->packet_threshold, RDD_QUEUE_PROFILE_DISABLED, queue_cfg->counter_id);
        BDMF_TRACE_DBG_OBJ(mo, "rdd_wan_tx_queue_cfg(%d, %d, %d, %d, %d, %d) --> %d\n",
            (int)channel_id, (int)qtm_ctl->rc_id, (int)qentry->queue_index,
            (int)queue_cfg->packet_threshold, RDD_QUEUE_PROFILE_DISABLED,
            (int)queue_cfg->counter_id, rdd_rc);
        err = rdd_rc ? BDMF_ERR_INTERNAL : BDMF_ERR_OK;

        /* should be removed once rdd change config to receive the profile as well */
        if (!err && queue_cfg->profile != RDD_QUEUE_PROFILE_DISABLED)
        {
            rdd_rc = rdd_wan_tx_queue_modify(channel_id, qtm_ctl->rc_id, qentry->queue_index,
                queue_cfg->packet_threshold, queue_cfg->profile, queue_cfg->counter_id);
            BDMF_TRACE_DBG_OBJ(mo, "rdd_wan_tx_queue_modify(%d, %d, %d, %d, %d, %d) --> %d\n",
                (int)channel_id, (int)qtm_ctl->rc_id, (int)qentry->queue_index,
                (int)queue_cfg->packet_threshold, queue_cfg->profile,
                (int)queue_cfg->counter_id, rdd_rc);
            err = rdd_rc ? BDMF_ERR_INTERNAL : BDMF_ERR_OK;
        }

        /* DDR queue configuration */
        err = err ? err : rdpa_rdd_tx_ddr_queue_cfg(mo, qtm_ctl, qentry, queue_cfg, ddr_cfg);
    }
    else
    {
        /* DS */
        err = rdpa_rdd_tx_queue_modify(mo, qentry, queue_cfg, ddr_cfg);
    }

    return err;
}

bdmf_error_t rdpa_rdd_tx_queue_modify(struct bdmf_object *mo, tm_queue_hash_entry_t *qentry,
    const rdd_queue_cfg_t *queue_cfg, pd_offload_ddr_queue_t *ddr_cfg)
{
    tm_qtm_ctl_t *qtm_ctl =  egress_tm_hash_entry_container(qentry);
    tm_channel_t *channel = qtm_ctl->channel;
    bdmf_error_t err = BDMF_ERR_INTERNAL;
    int rdd_rc;

    BUG_ON(!channel);

    if (queue_cfg->rl_cfg.af_rate || queue_cfg->rl_cfg.be_rate)
    {
        BDMF_TRACE_RET_OBJ(BDMF_ERR_NOT_SUPPORTED, mo,
            "Queue-level rate configuration is not supported\n");
    }

    if (channel->dir == rdpa_dir_us)
    {
        int16_t channel_id = _rdd_wan_channel(channel->channel_id);
        BUG_ON(qtm_ctl->rc_id < 0);
        /* DDR queue configuration */
        err = rdpa_rdd_tx_ddr_queue_cfg(mo, qtm_ctl, qentry, queue_cfg, ddr_cfg);
        if (!err)
        {
            rdd_rc = rdd_wan_tx_queue_modify(channel_id, qtm_ctl->rc_id, qentry->queue_index,
                queue_cfg->packet_threshold, queue_cfg->profile, queue_cfg->counter_id);
            BDMF_TRACE_DBG_OBJ(mo, "rdd_wan_tx_queue_modify(%d, %d, %d, %d, %d, %d) --> %d\n",
                (int)channel_id, (int)qtm_ctl->rc_id, (int)qentry->queue_index,
                (int)queue_cfg->packet_threshold, queue_cfg->profile,
                (int)queue_cfg->counter_id, rdd_rc);
            err = rdd_rc ? BDMF_ERR_INTERNAL : BDMF_ERR_OK;
        }
    }
    else
    {
        /* DS */
        /* DDR queue configuration */
        err = rdpa_rdd_tx_ddr_queue_cfg(mo, qtm_ctl, qentry, queue_cfg, ddr_cfg);
        if (!err)
        {
#ifndef LEGACY_RDP
            rdd_rc = rdd_lan_vport_tx_queue_cfg(channel->channel_id, qentry->queue_index,
                queue_cfg->packet_threshold, queue_cfg->profile);
#else
            rdd_rc = rdd_eth_tx_queue_cfg(channel->channel_id, qentry->queue_index,
                queue_cfg->packet_threshold, queue_cfg->profile, queue_cfg->counter_id);
#endif
            err = rdd_rc ? BDMF_ERR_INTERNAL : BDMF_ERR_OK;
            BDMF_TRACE_DBG_OBJ(mo, "rdd_eth_tx_queue_config(%d, %d, %d, %d) -> %d\n",
                (int)channel->channel_id, (int)qentry->queue_index, (int)queue_cfg->packet_threshold,
                (int)queue_cfg->profile, rdd_rc);
        }
    }
    /* Release DDR queues if error */
    if (err && ddr_cfg->addr)
    {
        bdmf_free(ddr_cfg->addr);
        ddr_cfg->addr = NULL;
        ddr_cfg->size = 0;
    }

    return err;
}

void rdpa_rdd_tx_queue_destroy(struct bdmf_object *mo, tm_queue_hash_entry_t *qentry, pd_offload_ddr_queue_t *ddr_cfg)
{
    tm_qtm_ctl_t *qtm_ctl =  egress_tm_hash_entry_container(qentry);
    tm_channel_t *channel = qtm_ctl->channel;
    rdd_queue_cfg_t queue_cfg = {
        .packet_threshold = 0,
        .counter_id = 0
    };

    BUG_ON(!channel);

    if (channel->dir == rdpa_dir_us)
    {
        int16_t channel_id = _rdd_wan_channel(channel->channel_id);
        BUG_ON(qtm_ctl->rc_id < 0);
        rdd_wan_tx_queue_modify(channel_id, qtm_ctl->rc_id, qentry->queue_index,
            0, RDD_QUEUE_PROFILE_DISABLED, INVALID_COUNTER_ID);
        rdd_wan_tx_queue_flush(channel_id, qtm_ctl->rc_id, qentry->queue_index, 1);
        rdpa_rdd_tx_ddr_queue_cfg(mo, qtm_ctl, qentry, &queue_cfg, ddr_cfg);
        rdd_wan_tx_queue_remove(channel_id, qtm_ctl->rc_id, qentry->queue_index);
    }
    else
    {
#ifndef LEGACY_RDP
        rdd_lan_vport_tx_queue_cfg(channel->channel_id, qentry->queue_index, 0, RDD_QUEUE_PROFILE_DISABLED);
#else
        rdd_eth_tx_queue_cfg(channel->channel_id, qentry->queue_index, 0, RDD_QUEUE_PROFILE_DISABLED, INVALID_COUNTER_ID);
#endif
        rdd_eth_tx_queue_flush(channel->channel_id, qentry->queue_index, 1);
        rdpa_rdd_tx_ddr_queue_cfg(mo, qtm_ctl, qentry, &queue_cfg, ddr_cfg);
        if (ddr_cfg->addr)
        {
            bdmf_free(ddr_cfg->addr);
            ddr_cfg->addr = NULL;
            ddr_cfg->size = 0;
            rdd_eth_tx_ddr_queue_addr_cfg(channel->channel_id, qentry->queue_index, 0, 0, 0);
        }
    }
    qentry->rdp_queue_index = -1;
}

void rdpa_rdd_tx_queue_disable(struct bdmf_object *mo, tm_queue_hash_entry_t *qentry, pd_offload_ddr_queue_t *ddr_cfg)
{
    tm_qtm_ctl_t *qtm_ctl =  egress_tm_hash_entry_container(qentry);
    tm_channel_t *channel = qtm_ctl->channel;
    rdd_queue_cfg_t queue_cfg = {
        .packet_threshold = 0,
        .counter_id = 0
    };

    BUG_ON(!channel);

    if (channel->dir == rdpa_dir_us)
    {
        int16_t channel_id = _rdd_wan_channel(channel->channel_id);
        BUG_ON(qtm_ctl->rc_id < 0);
        rdd_wan_tx_queue_modify(channel_id, qtm_ctl->rc_id, qentry->queue_index,
            0, RDD_QUEUE_PROFILE_DISABLED, qtm_ctl->counter_id[qentry->queue_index]);
        rdd_wan_tx_queue_flush(channel_id, qtm_ctl->rc_id, qentry->queue_index, 1);
        rdpa_rdd_tx_ddr_queue_cfg(mo, qtm_ctl, qentry, &queue_cfg, ddr_cfg);
    }
    else
    {
#ifndef LEGACY_RDP
        rdd_lan_vport_tx_queue_cfg(channel->channel_id, qentry->queue_index, 0, RDD_QUEUE_PROFILE_DISABLED);
#else
        rdd_eth_tx_queue_cfg(channel->channel_id, qentry->queue_index, 0, RDD_QUEUE_PROFILE_DISABLED, qtm_ctl->counter_id[qentry->queue_index]);
#endif
        rdd_eth_tx_queue_flush(channel->channel_id, qentry->queue_index, 1);
        rdpa_rdd_tx_ddr_queue_cfg(mo, qtm_ctl, qentry, &queue_cfg, ddr_cfg);
        if (ddr_cfg->addr)
        {
            bdmf_free(ddr_cfg->addr);
            ddr_cfg->addr = NULL;
            ddr_cfg->size = 0;
            rdd_eth_tx_ddr_queue_addr_cfg(channel->channel_id, qentry->queue_index, 0, 0, qtm_ctl->counter_id[qentry->queue_index]);
        }
    }
}

bdmf_error_t rdpa_rdd_tx_queue_flush(struct bdmf_object *mo, tm_queue_hash_entry_t *qentry,
    bdmf_boolean is_wait)
{
    tm_qtm_ctl_t *qtm_ctl =  egress_tm_hash_entry_container(qentry);
    tm_channel_t *channel = qtm_ctl->channel;
    int rdd_rc;

    BUG_ON(!channel);

    /* flush queue in RDD */
    if (channel->dir == rdpa_dir_us)
    {
        int16_t channel_id = _rdd_wan_channel(channel->channel_id);
        rdd_rc = rdd_wan_tx_queue_flush(channel_id, qtm_ctl->rc_id, qentry->queue_index, is_wait);
        BDMF_TRACE_DBG_OBJ(mo, "rdd_wan_tx_queue_flush(%d, %d, %d, %d) -> %d\n",
            (int)channel_id, (int)qtm_ctl->rc_id, (int)qentry->queue_index, is_wait, rdd_rc);
    }
    else
    {
        rdd_rc = rdd_eth_tx_queue_flush(channel->channel_id, qentry->queue_index, is_wait);
        BDMF_TRACE_DBG_OBJ(mo, "rdd_eth_tx_queue_flush(%d, %d, %d) -> %d\n",
            (int)channel->channel_id, (int)qentry->queue_index, is_wait, rdd_rc);
    }
    return rdd_rc ? BDMF_ERR_INTERNAL : BDMF_ERR_OK;
}

bdmf_error_t rdpa_rdd_tx_queue_stat_read(struct bdmf_object *mo, tm_queue_hash_entry_t *qentry, rdpa_stat_1way_t *stat)
{
    tm_qtm_ctl_t *qtm_ctl =  egress_tm_hash_entry_container(qentry);
    tm_channel_t *channel = qtm_ctl->channel;
#if !defined(LEGACY_RDP) && !defined(WL4908)
    uint16_t num_of_packets;
#endif

    BUG_ON(!channel);

    memset(stat, 0, sizeof(rdpa_stat_1way_t));
#if defined(WL4908)
    if (channel->dir == rdpa_dir_us)
        rdd_wan_tx_queue_get_status(_rdd_wan_channel(channel->channel_id), qtm_ctl->rc_id, qentry->queue_index, stat);
    else
        rdd_lan_vport_tx_queue_status_get(channel->channel_id, qentry->queue_index, stat);
#elif defined(LEGACY_RDP)
    if (channel->dir == rdpa_dir_us)
        rdd_wan_tx_queue_get_status(_rdd_wan_channel(channel->channel_id), qtm_ctl->rc_id, qentry->queue_index, stat);
    else
        rdd_eth_tx_queue_get_status(channel->channel_id, qentry->queue_index, stat);
#else
    if (channel->dir == rdpa_dir_us)
        rdd_wan_tx_queue_get_status(_rdd_wan_channel(channel->channel_id), qtm_ctl->rc_id, qentry->queue_index, &num_of_packets);
    else
        rdd_eth_tx_queue_get_status(channel->channel_id, qentry->queue_index, &num_of_packets);
    stat->passed.packets = num_of_packets;
#endif
    return BDMF_ERR_OK;
}

bdmf_error_t rdpa_rdd_tx_queue_stat_clear(struct bdmf_object *mo, tm_queue_hash_entry_t *qentry)
{
    tm_qtm_ctl_t *qtm_ctl =  egress_tm_hash_entry_container(qentry);
    tm_channel_t *channel = qtm_ctl->channel;

    BUG_ON(!channel);
#if defined(LEGACY_RDP)
    if (channel->dir == rdpa_dir_us)
        rdd_wan_tx_queue_clear_stat(_rdd_wan_channel(channel->channel_id), qtm_ctl->rc_id, qentry->queue_index);
    else
        rdd_eth_tx_queue_clear_stat(channel->channel_id, qentry->queue_index);
#endif
    return BDMF_ERR_OK;
}

/*
 * WRED support
 */
#define RDPA_MAX_WRED_PROFILE_PER_DIRECTION 8  /**< Max number of wred profilers */

static rdpa_tm_wred_profile wred_profile[2][RDPA_MAX_WRED_PROFILE_PER_DIRECTION];

/* [Allocate and] configure queue WRED profile */
bdmf_error_t rdpa_rdd_tm_queue_profile_cfg(struct bdmf_object *mo, rdpa_traffic_dir dir,
    const rdpa_tm_queue_cfg_t *old_queue_cfg, rdpa_tm_queue_cfg_t *new_queue_cfg,
    bdmf_boolean is_new, rdd_queue_profile_id_t *profile)
{
    rdd_queue_profile_t rdd_profile = {};
    int i;
    bdmf_boolean no_change = 0;
    uint32_t new_profile_index = RDPA_MAX_WRED_PROFILE_PER_DIRECTION;

    /* first remove the old configuration if exist */
    /* For XRDP we WRED profile is assigned even for tail-drop queues */
    if (old_queue_cfg->drop_alg != rdpa_tm_drop_alg_dt)
    {
        if (!is_new)
        {
            if (!rdpa_rdd_wred_param_modified(old_queue_cfg, new_queue_cfg))
                no_change = 1;
            else
                rdpa_rdd_tm_queue_profile_free(mo, dir, old_queue_cfg);
        }
    }

    if (new_queue_cfg->drop_alg == rdpa_tm_drop_alg_dt)
    {
        *profile = RDD_QUEUE_PROFILE_DISABLED;
        return 0;
    }

    /* find exist profile or allocate new one*/
    for (i = 0; i < RDPA_MAX_WRED_PROFILE_PER_DIRECTION; i++)
    {
        if (!wred_profile[dir][i].attached_q_num)
        {
            new_profile_index = i; /*save empty profile index in case we need to allocate new one */
        }

        /* look for existing profile */
        if (wred_profile[dir][i].drop_threshold == new_queue_cfg->drop_threshold &&
            wred_profile[dir][i].low_class.min_threshold == new_queue_cfg->low_class.min_threshold &&
            wred_profile[dir][i].low_class.max_threshold == new_queue_cfg->low_class.max_threshold &&
            wred_profile[dir][i].high_class.min_threshold == new_queue_cfg->high_class.min_threshold &&
            wred_profile[dir][i].high_class.max_threshold == new_queue_cfg->high_class.max_threshold)
        {
            /* same configuration just save the profile*/
            if (!no_change)
                wred_profile[dir][i].attached_q_num++;

            *profile = i;
            BDMF_TRACE_DBG_OBJ(mo,
                "Set queue to existing profile dir %s index %d."
                "low threshold min %d low threshold max %d high threshold min %d high threshold max %d drop threshold %d "
                "attached queues %d\n", dir ? "us" : "ds", i,
                wred_profile[dir][i].low_class.min_threshold,
                wred_profile[dir][i].low_class.max_threshold,
                wred_profile[dir][i].high_class.min_threshold,
                wred_profile[dir][i].high_class.max_threshold,
                wred_profile[dir][i].drop_threshold,
                wred_profile[dir][i].attached_q_num);
            return 0;
        }
    }

    if (new_profile_index == RDPA_MAX_WRED_PROFILE_PER_DIRECTION)
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
        rdd_profile.high_priority_class.min_threshold = new_queue_cfg->high_class.min_threshold;
        rdd_profile.high_priority_class.max_threshold = new_queue_cfg->high_class.max_threshold;
        rdd_profile.max_drop_probability = RDPA_WRED_MAX_DROP_PROBABILITY;
    }

    rdd_queue_profile_cfg(dir, new_profile_index, &rdd_profile);

    wred_profile[dir][new_profile_index].drop_threshold = new_queue_cfg->drop_threshold;
    wred_profile[dir][new_profile_index].high_class.min_threshold = new_queue_cfg->high_class.min_threshold;
    wred_profile[dir][new_profile_index].high_class.max_threshold = new_queue_cfg->high_class.max_threshold;
    wred_profile[dir][new_profile_index].low_class.min_threshold = new_queue_cfg->low_class.min_threshold;
    wred_profile[dir][new_profile_index].low_class.max_threshold = new_queue_cfg->low_class.max_threshold;
    wred_profile[dir][new_profile_index].attached_q_num++;

    *profile = new_profile_index;

    BDMF_TRACE_DBG_OBJ(mo, "Set queue to new profile dir %s index %d."
        "low threshold min %d low threshold max %d high threshold min %d high threshold max %d drop threshold %d attached "
        "queues %d\n", dir ? "us" : "ds", new_profile_index,
        wred_profile[dir][new_profile_index].low_class.min_threshold,
        wred_profile[dir][new_profile_index].low_class.max_threshold,
        wred_profile[dir][new_profile_index].high_class.min_threshold,
        wred_profile[dir][new_profile_index].high_class.max_threshold,
        wred_profile[dir][new_profile_index].drop_threshold,
        wred_profile[dir][new_profile_index].attached_q_num);

    return 0;
}

/* Unreference queue WRED profile */
bdmf_error_t rdpa_rdd_tm_queue_profile_free(struct bdmf_object *mo, rdpa_traffic_dir dir,
    const rdpa_tm_queue_cfg_t *queue_cfg)
{
    int i;

    if (queue_cfg->drop_alg == rdpa_tm_drop_alg_dt)
        return 0;

    for (i = 0; i < RDPA_MAX_WRED_PROFILE_PER_DIRECTION; i++)
    {
        if (wred_profile[dir][i].drop_threshold == queue_cfg->drop_threshold &&
            wred_profile[dir][i].low_class.min_threshold == queue_cfg->low_class.min_threshold &&
            wred_profile[dir][i].low_class.max_threshold == queue_cfg->low_class.max_threshold &&
            wred_profile[dir][i].high_class.min_threshold == queue_cfg->high_class.min_threshold &&
            wred_profile[dir][i].high_class.max_threshold == queue_cfg->high_class.max_threshold)
            break;
    }

    if (i == RDPA_MAX_WRED_PROFILE_PER_DIRECTION)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_INTERNAL, mo, "Can't find profile\n");

    wred_profile[dir][i].attached_q_num--;

    /* no queues using this profile - remove it */
    if (!wred_profile[dir][i].attached_q_num)
    {
        wred_profile[dir][i].drop_threshold = 0;
        wred_profile[dir][i].low_class.min_threshold = 0;
        wred_profile[dir][i].low_class.max_threshold = 0;
        wred_profile[dir][i].high_class.min_threshold = 0;
        wred_profile[dir][i].high_class.max_threshold = 0;
    }

    BDMF_TRACE_DBG_OBJ(mo, "removed queue from profile dir %s index %d. "
        "remaining attached queues %d\n", dir ? "us" : "ds", i,
        wred_profile[dir][i].attached_q_num);

    return 0;
}

/*
 * Overall rate limiter support
 */

/* Add / remove channel to overlall rate limiter */
bdmf_error_t rdpa_rdd_orl_channel_cfg(struct bdmf_object *mo, const tm_channel_t *channel,
    bdmf_boolean rate_limiter_enabled, rdpa_tm_orl_prty prio)
{
    int rdd_rc;

    rdd_rc = rdd_wan_channel_rate_limiter_cfg(_rdd_wan_channel(channel->channel_id),
        rate_limiter_enabled, prio);
    BDMF_TRACE_DBG_OBJ(mo, "rdd_wan_channel_rate_limiter_cfg(%d, %d, %d) -> %d\n",
        (int)_rdd_wan_channel(channel->channel_id), rate_limiter_enabled, prio, rdd_rc);
    if (rdd_rc)
    {
        BDMF_TRACE_RET_OBJ(BDMF_ERR_INTERNAL, mo, "RDD error %d when configuring ORL on channel %d\n",
            rdd_rc, channel->channel_id);
    }
    return 0;
}

/* Configure overall rate limiter rate */
bdmf_error_t rdpa_rdd_orl_rate_cfg(struct bdmf_object *mo, uint32_t rate)
{
    rdd_rate_limit_params_t params = {};

    params.rate = rate; /* rate in byte/s */
    params.limit = params.rate / RDD_RATE_QUANTUM;
    rdd_us_overall_rate_limiter_cfg(&params);
    BDMF_TRACE_RET_OBJ(0, mo, "rdd_us_overall_rate_limiter_cfg({rate=%d, limit=%d}})\n",
        (int)params.rate, (int)params.limit);
}

int egress_tm_attr_service_queue_stat_read_ex(tm_drv_priv_t *tm, rdpa_stat_1way_t *stat)
{
    rdd_service_queue_pm_counters_t rdd_stat = {};
    int rc;

    if (tm->level == rdpa_tm_level_egress_tm)
        return BDMF_ERR_NOENT;

    rc = rdd_service_queue_counters_get(tm->queue_cfg[0].queue_id, &rdd_stat);
    if (rc)
    {
        BDMF_TRACE_RET_OBJ(BDMF_ERR_INTERNAL, tm->this, "Queue=%d stat read RDD error %d\n",
            tm->queue_cfg[0].queue_id, rc);
    }

    stat->discarded.packets = rdd_stat.error_tx_packets_discard;
    stat->passed.packets = rdd_stat.good_tx_packet;

    return 0;
}


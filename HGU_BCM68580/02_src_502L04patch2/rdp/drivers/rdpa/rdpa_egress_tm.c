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
 * rdpa_egress_tm.c
 *
 *  Created on: Aug 17, 2012
 *      Author: igort
 */

#include "bdmf_dev.h"
#include "rdpa_api.h"
#include "rdpa_common.h"
#include "rdd.h"
#include "rdpa_egress_tm_inline.h"
#include "rdpa_egress_tm_ex.h"
#ifndef XRDP
#include "rdd_tm.h"
#ifndef WL4908
#include "rdd_legacy_conv.h"
#endif
#include "rdd_service_queues.h"
#else
#include "rdd_runner_proj_defs.h"
#endif

/* Max number of scheduling levels */
#define EGRESS_TM_MAX_SCHED_LEVELS      2

/* Max number of DS rate controllers */
#define RDPA_MAX_DS_RATE_CONTROLLERS    RDD_RATE_LIMITER_DISABLED

/* Currently in DS there is no channel as such. Rate controller represents
 * channel */
#ifndef G9991
#define RDPA_MAX_DS_CHANNELS            RDPA_MAX_DS_RATE_CONTROLLERS
#else
/* XXX: Number of DS channels supported, in G9991 case we need one for each
 * rdpa lan port 0-21 */
#define RDPA_MAX_DS_CHANNELS            30
#endif

/* Service queue channel, used for get next function */
#define RDPA_SERVICE_Q_CHANNEL (RDPA_MAX_DS_CHANNELS + 1)

#ifndef G9991
#define RDPA_MAX_QUEUE_COUNTERS         130
#else
#define RDPA_MAX_QUEUE_COUNTERS         160
#endif

/***************************************************************************
 * egress_tm object type
 **************************************************************************/

/* Overview:
 * - egress_tm objects form an hierarchy using "subsidiary" attribute
 *   (subs_tms[] in tm_drv_priv_t structure)
 * - top-level object in hierarchy can be "bound" to multiple channels (TCONTs,
 *   PORTs) using _rdpa_egress_tm_channel_set()
 * - channel contexts (tm_channel_t) are pre-allocated in per-direction arrays
 *   us_channels, ds_channels
 *      - egress_tm and owner fields in unbound channel context are NULL.
 *              - egress_tm is top objecvt in hierarchy
 *              - owner is port/tcont object that "owns" the channel
 *      - each channel context points to LL of dynamically-allocated rate
 *        controller contexts (tm_qtm_ctl_t).
 *      - channel context also contains RDD's channel_id
 * - rate controller context (tm_qtm_ctl_t) is allocated per channel x per
 *   queue-level egress_tm
 *      - rate controller context contains per-queue array of hash entries which
 *        are inserted into global hash table when queue is configured
 *      - rate controller context also contains RDD's rc_id
 * - global hash table tm_queue_hash[] is used for fast mapping
 *   { dir, channel_id, queue_id } --> { rc_id, priority }
 *
 * - RDD-related resources, including rate controller contexts are allocated
 *   when channel is bound
 * - RDD queues are allocated and added to hash, when
 *   queue_cfg[].drop_thresahold on bound egress_tm is set > 0
 */

/* hash table for mapping (channel,queue) --> (egress_tm, queue_index) */

/* hash entry. stored inside tm_drv_priv_t structure.
 * Therefore, tm_drv_priv_t pointer is recoverable given the tm_queue_hash_entry pointer
 */

DEFINE_BDMF_FASTLOCK(tm_hash_lock_irq);

static struct bdmf_object *us_tm_objects[RDPA_TM_MAX_US_SCHED];
static struct bdmf_object *ds_tm_objects[RDPA_TM_MAX_DS_SCHED];
static struct bdmf_object *ds_service_q_tm_obj;
static struct bdmf_object *ds_service_q_obj[RDPA_MAX_SERVICE_QUEUE];
static struct bdmf_object *us_overall_rl_obj;

static int num_normal_ds_tm; /* count the number of not service queue egress_tm */

#ifdef EPON
static bdmf_boolean orl_tm_linked_to_llid; /* flag shows if "ORL TM" (only 1 TM configs
                                              ORL) is linked to LLID*/
#endif

/* US / DS channel contexts */
static tm_channel_t us_channels[RDPA_MAX_US_CHANNELS];
static tm_channel_t ds_channels[RDPA_MAX_DS_CHANNELS];

/* US / DS channel/queue hash entry shortcut lookup table */
static tm_queue_hash_entry_t *us_hash_lookup[RDPA_MAX_US_CHANNELS][RDPA_TM_MAX_SCHED_ELEMENTS * RDPA_MAX_EGRESS_QUEUES];
static tm_queue_hash_entry_t *ds_hash_lookup[RDPA_MAX_DS_CHANNELS][RDPA_TM_MAX_SCHED_ELEMENTS * RDPA_MAX_EGRESS_QUEUES];
/*
 * queue hash array
 */

/*call back pointer to register EPON stack gloabl shaper*/
epon_global_shaper_cb_t global_shaper_cb;
EXPORT_SYMBOL(global_shaper_cb);

/*call back pointer to register EPON stack gloabl shaper*/
epon_link_shaper_cb_t epon_link_shaper_cb;
EXPORT_SYMBOL(epon_link_shaper_cb);

static uint8_t queue_counters[2][RDPA_MAX_QUEUE_COUNTERS];
static uint8_t queue_counters_idx[2] = {0, 0};

struct tm_queue_hash_head tm_queue_hash[TM_QUEUE_HASH_SIZE];
static struct tm_queue_hash_entry *egress_tm_hash_delete(bdmf_object_handle mo, uint32_t dir_channel, uint32_t queue_id);
static void egress_tm_delete_single_queue(struct bdmf_object *mo, tm_qtm_ctl_t *qtm_ctl, tm_channel_t *channel, tm_drv_priv_t *tm, int i);

static tm_drv_priv_t *egress_tm_get_top(tm_drv_priv_t *tm)
{
    while (tm->upper_level_tm)
        tm = (tm_drv_priv_t *)bdmf_obj_data(tm->upper_level_tm);
    return tm;
}

static bdmf_object_handle egress_tm_get_top_object(bdmf_object_handle mo)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    return egress_tm_get_top(tm)->this;
}

/* Egress_tm with group owner. in this mode queue_id is unique per group_id rather than channel
 * Used for LLID control and data channels
 */
static inline bdmf_boolean egress_tm_is_group_owner(tm_drv_priv_t *tm)
{
    return (tm->channel_group_owner != NULL);
}

/* Egress_tm contains sub-trees bound to different channels in channel group
 * Used for LLID data
 */
static inline bdmf_boolean egress_tm_is_ch_group(tm_drv_priv_t *tm)
{
    return (tm->channel_group_owner != NULL && tm->level == rdpa_tm_level_egress_tm);
}

static inline bdmf_boolean egress_tm_is_service_q(tm_drv_priv_t *tm)
{
    return tm->service_q.enable;
}

/* root object differs from top object in case of channel group (LLID),
 * whereas each root object's subsidiary is considered top object in its hierarchy
 */
static bdmf_object_handle egress_tm_get_root_object(bdmf_object_handle mo)
{
    while (mo->owner && mo->owner->drv == rdpa_egress_tm_drv())
        mo = mo->owner;
    return mo;
}

/*
 * Resource management helpers
 */

/*
 * Channel helpers
 */

/* Get channel by dir, channel id */
static tm_channel_t *egress_tm_channel_get(rdpa_traffic_dir dir, int channel)
{
    tm_channel_t *channels;
    int max_channels;
    if (dir == rdpa_dir_ds)
    {
        max_channels = RDPA_MAX_DS_CHANNELS;
        channels = ds_channels;
    }
    else
    {
        max_channels = RDPA_MAX_US_CHANNELS;
        channels = us_channels;
    }
    if ((unsigned)channel >= max_channels)
        return NULL;
    return &channels[channel];
}

/* The function returns pointer of the 1st/next channel bound to egress_tm object.
 * returns NULL is non is bound
 */
static tm_channel_t *egress_tm_channel_get_next(struct bdmf_object *mo, tm_channel_t *ch)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    struct bdmf_object *top;
    tm_channel_t *channels;
    int num_channels;
    int i = 0;

    /* Special handling of channel-group TM.
     * Go over sub-channels
     */
    if (egress_tm_is_ch_group(tm))
    {
        /* Find "*this" sub-channel and look up from the next */
        if (ch)
        {
            struct bdmf_object *cur = ch->egress_tm;
            for (i = 0; i < tm->num_channels && cur != tm->sub_tms[i]; i++)
                ;
            ch = NULL;
            ++i;
        }
        for (; i < tm->num_channels && !ch; i++)
        {
            if (tm->sub_tms[i])
                ch = egress_tm_channel_get_next(tm->sub_tms[i], NULL);
        }
        return ch;
    }

    /* Go over all channels find the 1st one bound to the top egress_tm */
    top = egress_tm_get_top_object(mo);
    channels = (tm->dir == rdpa_dir_ds) ? ds_channels : us_channels;
    num_channels = (tm->dir == rdpa_dir_ds) ? RDPA_MAX_DS_CHANNELS : RDPA_MAX_US_CHANNELS;
    i = ch ? ch->channel_id + 1 : 0;
    for (; i < num_channels; i++)
    {
        ch = &channels[i];
        if (ch->egress_tm == top)
            return ch;
    }

    return NULL;
}

/* The function returns pointer of the 1st channel bound to egress_tm object.
 * returns NULL is non is bound
 */
static inline tm_channel_t *egress_tm_channel_get_first(struct bdmf_object *mo)
{
    return egress_tm_channel_get_next(mo, NULL);
}

/* Get rate controller context owned by specific egress_tm object */
tm_qtm_ctl_t *egress_tm_qtm_ctl_get(struct bdmf_object *mo,
    tm_channel_t *channel)
{
    tm_qtm_ctl_t *qtm_ctl;

    STAILQ_FOREACH(qtm_ctl, &channel->qtm_ctls, list)
    {
        if (qtm_ctl->egress_tm == mo)
            return qtm_ctl;
    }
    return NULL;
}

/* Allocate scheduler/rate controller context */
static tm_qtm_ctl_t *egress_tm_qtm_ctl_ctx_alloc(struct bdmf_object *mo, tm_channel_t *channel, int16_t sub_index)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    tm_qtm_ctl_t *qtm_ctl = bdmf_calloc(sizeof(tm_qtm_ctl_t));
    int i;

    if (!qtm_ctl)
        return NULL;
    qtm_ctl->rc_id = BDMF_INDEX_UNASSIGNED;
    qtm_ctl->channel = channel;
    qtm_ctl->egress_tm = mo;
    qtm_ctl->sched_index_in_upper = sub_index;
    qtm_ctl->sched_weight = tm->weight;
    qtm_ctl->rl_rate_mode = tm->rl_rate_mode;

    for (i = 0; i < tm->num_queues; i++)
    {
        qtm_ctl->hash_entry[i].queue_index = i;
        qtm_ctl->hash_entry[i].rdp_queue_index = -1;
        memset(&qtm_ctl->queue_cfg[i], 0x0, sizeof(rdpa_tm_queue_cfg_t));
    }
    STAILQ_INSERT_TAIL(&channel->qtm_ctls, qtm_ctl, list);
    return qtm_ctl;
}

/* Free rate controller context */
static void egress_tm_qtm_ctl_ctx_free(tm_qtm_ctl_t *qtm_ctl)
{
    STAILQ_REMOVE(&qtm_ctl->channel->qtm_ctls, qtm_ctl, tm_qtm_ctl, list);
    bdmf_free(qtm_ctl);
}

/* Calculate total number of scheduling levels in hierarchy */
static int egress_tm_num_levels(struct bdmf_object *mo, int level)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    int max_sub_levels = 0;
    int sub_levels;
    int ns;

#ifdef XRDP
    if (tm->mode == rdpa_tm_sched_disabled)
        return 1;
#else
    if (tm->mode == rdpa_tm_sched_disabled)
        return 0;
#endif

    if (tm->level != rdpa_tm_level_egress_tm)
        return 1;

    /* Prevent infinite recursion */
    if (level > EGRESS_TM_MAX_SCHED_LEVELS)
        return level;

    for (ns = 0; ns < RDPA_TM_MAX_SCHED_ELEMENTS; ns++)
    {
        if (tm->sub_tms[ns])
        {
            sub_levels = egress_tm_num_levels(tm->sub_tms[ns], level + 1);
            if (sub_levels > max_sub_levels)
                max_sub_levels = sub_levels;
        }
    }

    return max_sub_levels + 1;
}

/* Check total number of levels in hierarchy */
static int egress_tm_check_levels(struct bdmf_object *mo)
{
    int levels = egress_tm_num_levels(egress_tm_get_top_object(mo), 0);
    if (levels > EGRESS_TM_MAX_SCHED_LEVELS)
    {
        BDMF_TRACE_RET_OBJ(BDMF_ERR_NOT_SUPPORTED, mo, "Max number of scheduling levels (%d) exceeded\n",
            EGRESS_TM_MAX_SCHED_LEVELS);
    }
    return 0;
}

/* Check if scheduling element should be WRR-scheduled */
static bdmf_boolean egress_tm_is_wrr_elem(tm_drv_priv_t *tm, bdmf_index index)
{
    if (tm->mode == rdpa_tm_sched_wrr)
        return 1;
    /* For SP_WRR mode, scheduling element is WRR if
     * - index >= number of SP elements
     * - scheduler is dual rate. In this case all elements are scheduled in WRR for PIR
     */
    if (tm->mode == rdpa_tm_sched_sp_wrr &&
         (index >= tm->num_sp_elements || tm->rl_rate_mode == rdpa_tm_rl_dual_rate))
        return 1;
    return 0;
}

/* Validate scheduling hierarchy */
static int egress_tm_validate_hierarchy(struct bdmf_object *mo)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    int ns;
    int rc = 0;

    /* channel_id must be set for SP/WRR scheduler */
    if (!tm->overall_rl && tm->upper_level_tm && !egress_tm_channel_get_first(mo))
        BDMF_TRACE_RET_OBJ(BDMF_ERR_STATE, mo, "Channel is not set\n");

    /* For WRR scheduler all subsidiary schedulers must have af rate == 0
     * For SP scheduler all subsidiary schedulers must have be rate == 0
     */
    if (tm->level == rdpa_tm_level_egress_tm)
    {
        for (ns = 0; ns < RDPA_TM_MAX_SCHED_ELEMENTS && !rc; ns++)
        {
            if (tm->sub_tms[ns])
            {
                tm_drv_priv_t *stm = (tm_drv_priv_t *)bdmf_obj_data(tm->sub_tms[ns]);
                if (egress_tm_is_wrr_elem(tm, ns) &&
                    !(stm->weight >= RDPA_MIN_WEIGHT && stm->weight <= RDPA_MAX_WEIGHT))
                {
                    BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, mo, "WRR weight is not set for %s\n",
                        tm->sub_tms[ns]->name);
                }
                if (stm->level == rdpa_tm_level_egress_tm)
                    rc = egress_tm_validate_hierarchy(tm->sub_tms[ns]);
            }
        }
    }

    return rc;
}

/* Release RDD resources - on specific channel */
static void egress_tm_rdd_resources_free_on_channel(struct bdmf_object *mo, tm_channel_t *channel)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    tm_qtm_ctl_t *qtm_ctl = egress_tm_qtm_ctl_get(mo, channel);
    int i;

    /* Release scheduler/rate controller if any */
    if (qtm_ctl)
    {
        for (i = 0; i < RDPA_MAX_EGRESS_QUEUES; i++)
        {
            egress_tm_delete_single_queue(mo, qtm_ctl, channel, tm, i); 
        }

        rdpa_rdd_qtm_ctl_destroy(mo, qtm_ctl);
        egress_tm_qtm_ctl_ctx_free(qtm_ctl);
    }

    /* Release resources on subsidiary schedulers */
    if (tm->level == rdpa_tm_level_egress_tm)
    {
        int ns;
        for (ns = 0; ns < RDPA_TM_MAX_SCHED_ELEMENTS; ns++)
        {
            if (tm->sub_tms[ns])
                egress_tm_rdd_resources_free_on_channel(tm->sub_tms[ns], channel);
        }
    }

    if (mo == egress_tm_get_top_object(mo))
    {
        rdpa_rdd_top_sched_destroy(mo, channel);
        channel->res_allocated = 0;
    }
}

static void egress_tm_delete_single_queue(struct bdmf_object *mo, tm_qtm_ctl_t *qtm_ctl, tm_channel_t *channel, tm_drv_priv_t *tm, int i)
{
    pd_offload_ddr_queue_t *ddr_cfg; 
    tm_queue_hash_entry_t *qentry;

    if (qtm_ctl->counter_id[i] != INVALID_COUNTER_ID)
    {
        queue_counters[channel->dir][--queue_counters_idx[channel->dir]] = qtm_ctl->counter_id[i];
        qtm_ctl->counter_id[i] = INVALID_COUNTER_ID;
    }

    if (qtm_ctl->queue_configured[i])
    {
        /* Destroy US/DS priority queue */
        qentry = egress_tm_hash_delete(mo, qtm_ctl->hash_entry[i].dir_channel, qtm_ctl->hash_entry[i].queue_id);
        ddr_cfg = (channel->dir == rdpa_dir_us) ? &qtm_ctl->wan_tx_ddr_queue[i] : &tm->ddr_queue[i];
        rdpa_rdd_tx_queue_destroy(mo, qentry, ddr_cfg);
        qtm_ctl->queue_configured[i] = 0;
    }
}

/* set queue attribute to all queues under channel */
bdmf_error_t _rdpa_egress_tm_set_channel_attr(bdmf_object_handle tm_obj, channel_attr *attr, int channel_id)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(tm_obj);
    tm_channel_t *channel = egress_tm_channel_get(tm->dir, channel_id);
    tm_qtm_ctl_t *qtm_ctl = egress_tm_qtm_ctl_get(tm_obj, channel);
    int i, err;

    if (channel != NULL)
        channel->attr = *attr;

    if (qtm_ctl == NULL)
    {
        for (i = 0; i < tm->num_channels; i++)
        {
            if (tm->sub_tms[i])
            {
                err = _rdpa_egress_tm_set_channel_attr(tm->sub_tms[i], attr, tm->channels[i]);
                if (err)
                    BDMF_TRACE_RET_OBJ(err, tm_obj, "Failed to set queue attributes for channel %d", channel_id);
            }
        }
    }
    else
    {
        for (i = 0; i < tm->num_queues; i++)
        {
            if (qtm_ctl->queue_configured[i])
            {
                err = rdpa_rdd_tx_queue_channel_attr_update(attr, qtm_ctl->hash_entry[i].rdp_queue_index);
                if (err)
                    BDMF_TRACE_RET_OBJ(err, tm_obj, "Failed to set queue attributes for channel %d", channel_id);
            }
        }
    }

    return 0;
}

/* Release RDD resources - on all channels */
static void egress_tm_rdd_resources_free(struct bdmf_object *mo)
{
    struct bdmf_object *top = egress_tm_get_top_object(mo);
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    tm_channel_t *channels = (tm->dir == rdpa_dir_ds) ? ds_channels : us_channels;
    int num_channels = (tm->dir == rdpa_dir_ds) ? RDPA_MAX_DS_CHANNELS : RDPA_MAX_US_CHANNELS;
    int i;

    if (egress_tm_is_service_q(tm))
    {
        if (tm->service_queue_ddr_addr) 
            bdmf_free(tm->service_queue_ddr_addr);
        return;
    }

    /* Go over all channels and release resources for all channels bound to the top */
    for (i = 0; i < num_channels; i++)
    {
        tm_channel_t *ch = &channels[i];
        if (ch->egress_tm == top)
            egress_tm_rdd_resources_free_on_channel(mo, ch);
    }
}


/* Allocate RDD resources, except for queues - on specific channel */
static int egress_tm_rdd_resources_alloc_on_channel(struct bdmf_object *mo, tm_channel_t *channel, int sub_index)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    struct bdmf_object *top = egress_tm_get_top_object(mo);
    rdd_sched_cfg_t sched_cfg = {
        .level = tm->level,
        .mode = tm->mode,
        .rl_cfg = tm->rl_cfg,
        .rl_rate_mode = tm->rl_rate_mode,
        .weight = tm->weight,
        .num_queues = tm->num_queues,
        .num_sp_elements = tm->num_sp_elements
    };
    int rc = 0;

    /* Allocate channel-level resource */
    if (!channel->res_allocated && mo == top)
    {
        rc = rdpa_rdd_top_sched_create(mo, channel, &sched_cfg);
        if (rc)
            return rc;
        channel->rl_rate_mode = sched_cfg.rl_rate_mode;
    }

    /* Allocate rate controller if necessary */
    if ((tm->level == rdpa_tm_level_queue) && !egress_tm_qtm_ctl_get(mo, channel))
    {
        tm_qtm_ctl_t *qtm_ctl = egress_tm_qtm_ctl_ctx_alloc(mo, channel, sub_index);
        int rc;

        if (!qtm_ctl)
            return BDMF_ERR_NOMEM;

        /* Do initial (dummy) configuration */
        rc = rdpa_rdd_qtm_ctl_create(mo, qtm_ctl, &sched_cfg);
        if (rc)
            return rc;
        qtm_ctl->res_allocated = 1;
    }

    /* Allocate resources on subsidiary schedulers */
    if (tm->level == rdpa_tm_level_egress_tm)
    {
        int ns;
        for (ns = 0; ns < RDPA_TM_MAX_SCHED_ELEMENTS && !rc; ns++)
        {
            if (tm->sub_tms[ns])
                rc = egress_tm_rdd_resources_alloc_on_channel(tm->sub_tms[ns], channel, ns);
        }
    }

    if (mo == top && !rc)
        channel->res_allocated = 1;

    return rc;
}


/* ALlocate RDD resources, except for queues - for all channels */
static int egress_tm_rdd_resources_alloc(struct bdmf_object *mo, bdmf_index index)
{
    struct bdmf_object *top = egress_tm_get_top_object(mo);
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(top);
    tm_channel_t *channels = (tm->dir == rdpa_dir_ds) ? ds_channels : us_channels;
    int num_channels = (tm->dir == rdpa_dir_ds) ? RDPA_MAX_DS_CHANNELS : RDPA_MAX_US_CHANNELS;
    int i;
    int rc = 0;

    if (egress_tm_is_service_q(tm))
        return 0;

    /* Go over all channels and allocate resources for all channels bound to the top */
    for (i = 0; i < num_channels && !rc; i++)
    {
        tm_channel_t *ch = &channels[i];
        if (ch->egress_tm == top)
            rc = egress_tm_rdd_resources_alloc_on_channel(mo, ch, index);
    }

    if (rc)
    {
        /* Roll-back if failure */
        for (--i; i >= 0; i--)
        {
            tm_channel_t *ch = &channels[i];
            if (ch->egress_tm == top)
                egress_tm_rdd_resources_free_on_channel(mo, ch);
        }
    }

    return rc;
}

/*
 * queue hash management
 */

/* Delete hash entry. Returns the entry that has just been "deleted"
 * Note that the entry was deleted from the hash, but information contained in the entry
 * is still valid;
 */
static struct tm_queue_hash_entry *egress_tm_hash_delete(bdmf_object_handle mo, uint32_t dir_channel, uint32_t queue_id)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    int index = egress_tm_hash_func(dir_channel, queue_id);
    struct tm_queue_hash_entry *entry;
    unsigned long flags;

    bdmf_fastlock_lock_irq(&tm_hash_lock_irq, flags);
    SLIST_FOREACH(entry, &tm_queue_hash[index], list)
    {
        if (entry->dir_channel == dir_channel && entry->queue_id == queue_id)
        {
            SLIST_REMOVE(&tm_queue_hash[index], entry, tm_queue_hash_entry, list);
            --tm->queue_configured[entry->queue_index];
            bdmf_fastlock_unlock_irq(&tm_hash_lock_irq, flags);
            BDMF_TRACE_DBG_OBJ(mo, "removed entry q:%u dir_c:0x%x\n", (int)queue_id, (unsigned)dir_channel);
            if (egress_tm_get_queue_dir(dir_channel) == rdpa_dir_us)
                us_hash_lookup[egress_tm_get_queue_channel(dir_channel)][queue_id] = NULL;
            else
                ds_hash_lookup[egress_tm_get_queue_channel(dir_channel)][queue_id] = NULL;
            return entry;
        }
    }

    /* Hmm. We shouldn't be here. */
    bdmf_fastlock_unlock_irq(&tm_hash_lock_irq, flags);
    BDMF_TRACE_ERR("Attempt to remove entry (%u, %u) which isn't in the hash\n", dir_channel, queue_id);
    BUG();
    return NULL;
}

/* Insert hash entry. Can fail if entry with given key already exists.
 * Returns NULL in case of success, hash entry having the same key in case of
 * failure.
 */
static tm_queue_hash_entry_t *egress_tm_hash_insert(bdmf_object_handle mo,
    uint32_t dir_channel, uint32_t queue_id, tm_queue_hash_entry_t *entry)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    int index = egress_tm_hash_func(dir_channel, queue_id);
    tm_queue_hash_entry_t *old_entry;
    unsigned long flags;

    old_entry = egress_tm_hash_get(dir_channel, queue_id);
    if (old_entry)
        return old_entry;
    entry->dir_channel = dir_channel;
    entry->queue_id = queue_id;

    bdmf_fastlock_lock_irq(&tm_hash_lock_irq, flags);
    SLIST_INSERT_HEAD(&tm_queue_hash[index], entry, list);
    ++tm->queue_configured[entry->queue_index];
    bdmf_fastlock_unlock_irq(&tm_hash_lock_irq, flags);

    if (egress_tm_get_queue_dir(dir_channel) == rdpa_dir_us)
        us_hash_lookup[egress_tm_get_queue_channel(dir_channel)][queue_id] = entry;
    else
        ds_hash_lookup[egress_tm_get_queue_channel(dir_channel)][queue_id] = entry;

    BDMF_TRACE_DBG_OBJ(mo, "inserted entry q:%u dir_c:0x%x\n", (int)queue_id, (unsigned)dir_channel);
    return NULL;
}

/* Get unused unique queue_id */
static uint32_t egress_tm_queue_get_unique(struct bdmf_object *mo, uint32_t dir_channel)
{
    uint32_t queue_id = 0;
    while (egress_tm_hash_get(dir_channel, queue_id))
        ++queue_id;
    return queue_id;
}

/* use the shortcut table for faster search */
static tm_queue_hash_entry_t *egress_tm_hash_shortcut_get(uint32_t dir_channel, uint32_t queue_id)
{
    uint32_t channel = egress_tm_get_queue_channel(dir_channel);

    if (unlikely(queue_id >= (RDPA_TM_MAX_SCHED_ELEMENTS * RDPA_MAX_EGRESS_QUEUES)))
        return NULL;

    if ((egress_tm_get_queue_dir(dir_channel) == rdpa_dir_us) && (channel < RDPA_MAX_US_CHANNELS))
        return us_hash_lookup[channel][queue_id];
    else if ((egress_tm_get_queue_dir(dir_channel) == rdpa_dir_ds) && (channel < RDPA_MAX_DS_CHANNELS))
        return ds_hash_lookup[channel][queue_id];
    else
        return NULL;
}

/* Configure / remove egress queue.
 * It includes adding/removing queue from hash and updating RDD
 */
static int egress_tm_queue_cfg_on_channel(struct bdmf_object *mo,
    tm_channel_t *channel, int index, rdpa_tm_queue_cfg_t *new_queue_cfg)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    uint32_t dir_channel = egress_tm_dir_channel(tm->dir, channel->channel_id);
    tm_qtm_ctl_t *qtm_ctl = egress_tm_qtm_ctl_get(mo, channel);
    uint32_t drop_threshold;
    bdmf_object_handle system_obj = NULL;
    rdpa_system_cfg_t system_cfg = {};
    rdd_queue_cfg_t queue_cfg = {};
    pd_offload_ddr_queue_t *ddr_cfg = (tm->dir == rdpa_dir_us) ? &qtm_ctl->wan_tx_ddr_queue[index] : &tm->ddr_queue[index];
    tm_queue_hash_entry_t *qentry;
    bdmf_error_t rc = BDMF_ERR_OK;

    BUG_ON(qtm_ctl == NULL);
    BUG_ON(tm->level != rdpa_tm_level_queue);

    /* Special handling for channel-group egress_tm (LLID).
     * Queues are stored in hash with group_id instead of channel_id
     */
    {
        struct bdmf_object *root = egress_tm_get_root_object(mo);
        tm_drv_priv_t *root_tm = (tm_drv_priv_t *)bdmf_obj_data(root);

        if (egress_tm_is_group_owner(root_tm))
            dir_channel = egress_tm_dir_channel(tm->dir, root_tm->channel_group);
    }

    /*validate threshold parameters */
    rc = rdpa_system_get(&system_obj);
    if (!rc)
        rdpa_system_cfg_get(system_obj, &system_cfg);

    /* Validate (W)RED thresholds if drop alg is (W)RED */
    if (new_queue_cfg->drop_alg == rdpa_tm_drop_alg_red || new_queue_cfg->drop_alg == rdpa_tm_drop_alg_wred)
    {
        if (system_cfg.options == 0)
        {
            if (new_queue_cfg->high_class.max_threshold > RDPA_ETH_TX_PRIORITY_QUEUE_THRESHOLD ||
                new_queue_cfg->high_class.max_threshold > new_queue_cfg->drop_threshold)
            {
                BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, mo, "Wrong drop threshold hi %u, size%u\n",
                    new_queue_cfg->high_class.max_threshold, new_queue_cfg->drop_threshold);
            }
        }
        else if (system_cfg.options == 1)
        {
            if (new_queue_cfg->high_class.max_threshold > RDPA_ETH_TX_PRIORITY_QUEUE_THRESHOLD ||
                new_queue_cfg->high_class.max_threshold < new_queue_cfg->low_class.max_threshold ||
                new_queue_cfg->drop_threshold < new_queue_cfg->high_class.max_threshold)
            {
                BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, mo, "Wrong thresholds hi%u, lo%u, size%u\n",
                    new_queue_cfg->high_class.max_threshold, new_queue_cfg->low_class.max_threshold,
                    new_queue_cfg->drop_threshold);
            }
        }
        if (new_queue_cfg->high_class.max_threshold < new_queue_cfg->high_class.min_threshold ||
            new_queue_cfg->low_class.max_threshold < new_queue_cfg->low_class.min_threshold)
        {
            BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, mo, "Wrong thresholds hi_max%u, hi_min%u, lo_max%u, lo_min%u, size%u\n",
                new_queue_cfg->high_class.max_threshold, new_queue_cfg->high_class.min_threshold,
                new_queue_cfg->low_class.max_threshold, new_queue_cfg->low_class.min_threshold,
                new_queue_cfg->drop_threshold);
        }
    }

    /* Remove old configuration if queue becomes disabled */
    if (qtm_ctl->queue_configured[index] && !new_queue_cfg->drop_threshold)
    {
        /* Get hash entry */
        qentry = egress_tm_hash_get(dir_channel, tm->queue_cfg[index].queue_id);

        /* Disable US/DS priority queue */
        rdpa_rdd_tx_queue_disable(mo, qentry, ddr_cfg);

        tm->queue_id_assigned[index] = (new_queue_cfg->queue_id != BDMF_INDEX_UNASSIGNED);

        rdpa_rdd_tm_queue_profile_free(mo, tm->dir, &tm->queue_cfg[index]);

        BDMF_TRACE_DBG_OBJ(mo, "Flush & disable queue %d on channel %d\n", index, channel->channel_id);
    }

    if (new_queue_cfg->drop_threshold)
    {
        rdd_queue_profile_id_t rdd_queue_profile = RDD_QUEUE_PROFILE_DISABLED;
        tm_queue_hash_entry_t *old_entry = NULL;
        int auto_assigned = 0;

        /* Manage queue hash.
         * - if queue_id is unassigned - assign it automatically. It is considered unassigned if
         *      - queue_id = 0xFFFFFFFF or
         *      - queue_id==0 && has never been assigned && in use by other queue
         *        avoid active queue_id re-assignment unless requested explicitly
         * - try to insert into hash. If entry already exists - see if old entry
         *   is auto-assigned. If yes - reassign old and store new.
         *   If old entry had manually-assigned queue id - configuration fails.
         */
        if (new_queue_cfg->queue_id == BDMF_INDEX_UNASSIGNED ||
            (!new_queue_cfg->queue_id && !tm->queue_id_assigned[index] &&
            egress_tm_hash_get(dir_channel, new_queue_cfg->queue_id)))
        {
            new_queue_cfg->queue_id = egress_tm_queue_get_unique(mo, dir_channel);
            auto_assigned = 1;
        }

        /* Delete from hash if queue_id changed on active queue */
        if ((new_queue_cfg->queue_id != tm->queue_cfg[index].queue_id) &&
            qtm_ctl->queue_configured[index])
        {
            egress_tm_hash_delete(mo, dir_channel, tm->queue_cfg[index].queue_id);
        }

        /* Insert to cache if queue_id changed or queue was inactive */
        if ((new_queue_cfg->queue_id != tm->queue_cfg[index].queue_id) ||
            !qtm_ctl->queue_configured[index])
        {
            old_entry = egress_tm_hash_insert(mo, dir_channel, new_queue_cfg->queue_id, &qtm_ctl->hash_entry[index]);
        }

        /* Error handling */
        if (old_entry)
        {
            /* Found existing hash entry. If queue_id was auto-assigned - re-assign it.
             * Otherwise, there is nothing we can do.
             */
            tm_qtm_ctl_t *old_qtm_ctl = egress_tm_hash_entry_container(old_entry);
            tm_drv_priv_t *old_tm = (tm_drv_priv_t *)bdmf_obj_data(old_qtm_ctl->egress_tm);
            int old_queue_index = old_entry->queue_index;

            if (!old_tm->queue_id_auto_assigned[old_entry->queue_index])
            {
                /* Re-insert old queue id into hash if necessary */
                if (qtm_ctl->queue_configured[index])
                    egress_tm_hash_insert(mo, dir_channel, tm->queue_cfg[index].queue_id, &qtm_ctl->hash_entry[index]);
                BDMF_TRACE_RET_OBJ(BDMF_ERR_ALREADY, mo, "queue_id %u is already in use by %s, q %d\n",
                    new_queue_cfg->queue_id, old_tm->this->name, old_queue_index);
            }

            /* Re-assign old queue_id */
            old_tm->queue_cfg[old_queue_index].queue_id = egress_tm_queue_get_unique(old_tm->this, dir_channel);

            /* delete old queue that clashes with new one from hash. */
            egress_tm_hash_delete(mo, old_entry->dir_channel, old_entry->queue_id);

            /* Now re-insert old queue. It can't fail */
            old_entry = egress_tm_hash_insert(mo, old_entry->dir_channel,
                old_tm->queue_cfg[old_queue_index].queue_id, old_entry);
            BUG_ON(old_entry);

            /* Finally, insert new queue. It can't fail */
            old_entry = egress_tm_hash_insert(mo, dir_channel, new_queue_cfg->queue_id, &qtm_ctl->hash_entry[index]);
            BUG_ON(old_entry);
        }

        tm->queue_id_auto_assigned[index] = auto_assigned;
        tm->queue_id_assigned[index] = 1;

        rc = rdpa_rdd_tm_queue_profile_cfg(mo, tm->dir,
            &tm->queue_cfg[index], new_queue_cfg,
            (qtm_ctl->queue_configured[index] == 0), &rdd_queue_profile);
        if (rc)
            return rc;

        if (new_queue_cfg->drop_alg == rdpa_tm_drop_alg_wred)
            drop_threshold = new_queue_cfg->low_class.min_threshold;
        else if (new_queue_cfg->drop_alg == rdpa_tm_drop_alg_reserved)
            drop_threshold = new_queue_cfg->high_class.min_threshold;
        else
            drop_threshold = new_queue_cfg->drop_threshold;

        if (new_queue_cfg->stat_enable)
        {
            if (qtm_ctl->counter_id[index] == INVALID_COUNTER_ID)
            {
                if (queue_counters_idx[tm->dir] < RDPA_MAX_QUEUE_COUNTERS)
                    qtm_ctl->counter_id[index] = queue_counters[tm->dir][queue_counters_idx[tm->dir]++];
                else
                {
                    BDMF_TRACE_ERR_OBJ(mo, "Can't assign queue counter\n");
                    BUG();
                }
            }
        }
        else
        {
            if (qtm_ctl->counter_id[index] != INVALID_COUNTER_ID)
            {
                queue_counters[tm->dir][--queue_counters_idx[tm->dir]] = qtm_ctl->counter_id[index];
                qtm_ctl->counter_id[index] = INVALID_COUNTER_ID;
            }
        }

        /* At this point new queue is in the hash. Update RDD */
        queue_cfg.packet_threshold = drop_threshold;
        queue_cfg.counter_id = qtm_ctl->counter_id[index];
        queue_cfg.profile = rdd_queue_profile;
        queue_cfg.rl_cfg = new_queue_cfg->rl_cfg;
        queue_cfg.weight = new_queue_cfg->weight;
        if (!qtm_ctl->queue_configured[index])
            rc = rdpa_rdd_tx_queue_create(mo, &qtm_ctl->hash_entry[index], &queue_cfg, ddr_cfg);
        else /* Queue threshold has been changed */
            rc = rdpa_rdd_tx_queue_modify(mo, &qtm_ctl->hash_entry[index], &queue_cfg, ddr_cfg);
        qtm_ctl->queue_configured[index] = 1;
    }

    if (rc)
    {
        /* Remove queue from hash and return error. If we are here - it is an internal error.
         * At this point it is difficult to roll-back. It is safer just leave the queue un-configured
         */
        if (qtm_ctl->queue_configured[index])
        {
            egress_tm_hash_delete(mo, dir_channel, new_queue_cfg->queue_id);
            qtm_ctl->queue_configured[index] = 0;
        }
        if (tm->queue_id_auto_assigned[index])
        {
            new_queue_cfg->queue_id = BDMF_INDEX_UNASSIGNED;
            tm->queue_id_auto_assigned[index] = 0;
        }

        rdpa_rdd_tm_queue_profile_free(mo, tm->dir, new_queue_cfg);

        memset(&qtm_ctl->queue_cfg[index], 0x0, sizeof(rdpa_tm_queue_cfg_t));
        BDMF_TRACE_RET_OBJ(rc, mo,
            "Failed to configure priority queue %d, channel %d, rate "
            "controller %d\n", index, channel->channel_id, qtm_ctl->rc_id);
    }
    else
    {
        memcpy(&qtm_ctl->queue_cfg[index], new_queue_cfg, sizeof(rdpa_tm_queue_cfg_t));
    }

    return rc;
}

static int egress_tm_service_queue_rl_config(struct bdmf_object *mo, rdpa_tm_rl_cfg_t *rl_cfg, bdmf_boolean enable)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    uint32_t queue_id = tm->queue_cfg[0].queue_id;
    rdd_rate_cntrl_params_t rc_params = {};
    long rate, limit;

    rate = rl_cfg->af_rate;
    limit = rl_cfg->burst_size;
    if (!rate && !rl_cfg->be_rate && enable)
    {
        rate = RDD_RATE_UNLIMITED;
        limit = RDD_RATE_UNLIMITED;
    }
    /* peak best effort limit should be at least as high as the best effort rate */
    if (limit < rl_cfg->be_rate)
       limit = rl_cfg->be_rate;

    rc_params.sustain_budget = rate / 8;

    if (tm->level == rdpa_tm_level_egress_tm)
    {
        rdd_service_queue_overall_rate_limiter_enable(enable && rl_cfg->af_rate);
        BDMF_TRACE_DBG_OBJ(mo,
            "rdd_ds_service_queue_overall_rate_limiter_enable({%d})\n",
            (int)(enable && rl_cfg->af_rate));

        rdd_service_queue_overall_rate_limiter_cfg(&rc_params);
        BDMF_TRACE_DBG_OBJ(mo, "rdd_service_queue_overall_rate_limiter_cfg({peak={r=%d, l=%d}, peak_w=%d, sust=%d)})\n",
            (int)rc_params.peak_budget.rate, (int)rc_params.peak_budget.limit,
            (int)rc_params.peak_weight, (int)rc_params.sustain_budget);
    }
    else
    {
        /* required by FW, although in SP mode */
        rc_params.peak_weight = RDPA_MAX_SERVICE_QUEUE * RDD_WEIGHT_QUANTUM;
        rc_params.peak_budget.rate = rl_cfg->be_rate / 8;
        rc_params.peak_budget.limit = limit / 8;

        rdd_service_queue_rate_limiter_cfg(queue_id, &rc_params);
        BDMF_TRACE_DBG_OBJ(mo, "rdd_service_queue_rate_limiter_cfg(%d, {peak={r=%d, l=%d}, peak_w=%d, sust=%d)})\n",
            queue_id,
            (int)rc_params.peak_budget.rate, (int)rc_params.peak_budget.limit,
            (int)rc_params.peak_weight, (int)rc_params.sustain_budget);
    }
    return 0;
}

static int egress_tm_service_queue_rdd_cfg(struct bdmf_object *mo,
    rdpa_tm_queue_cfg_t *new_queue_cfg, rdpa_tm_rl_cfg_t *new_rl,
    bdmf_boolean enable)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    rdd_queue_profile_id_t rdd_queue_profile = RDD_QUEUE_PROFILE_DISABLED;
    bdmf_boolean rate_limit = (enable && new_rl->af_rate);

     /* wred should set rdd_.._tx_queue config drop_threshold to lowest value of
     * drop threshold traffic */
    uint16_t drop_threshold = !enable ? 0 :
        (new_queue_cfg->drop_alg == rdpa_tm_drop_alg_wred ?
            new_queue_cfg->low_class.max_threshold : new_queue_cfg->drop_threshold);
    int rc;

    rc = egress_tm_service_queue_rl_config(mo, new_rl, enable);
    if (rc)
        goto out;

    /* level egress_tm only need rl configuration */
    if (tm->level == rdpa_tm_level_egress_tm)
        goto out;

    if (tm->queue_cfg[0].queue_id != new_queue_cfg->queue_id)
    {
        BDMF_TRACE_RET_OBJ(BDMF_ERR_INTERNAL, mo,
            "Egress_tm object already have queue_id %dn",
            tm->queue_cfg[0].queue_id);
    }

#if !defined(BDMF_SYSTEM_SIM) && !defined(XRDP)
    {
        void *service_queue_aligned_addr = NULL;
        uint16_t service_queue_size = !enable ? 0 :
            (new_queue_cfg->drop_alg == rdpa_tm_drop_alg_wred ?
                new_queue_cfg->high_class.max_threshold : new_queue_cfg->drop_threshold);

        if (tm->service_queue_ddr_addr) 
            bdmf_free(tm->service_queue_ddr_addr);

        if (enable)
        {
            service_queue_aligned_addr = ddr_queue_alloc(2 * service_queue_size, &tm->service_queue_ddr_addr);
            if (!service_queue_aligned_addr)
            {
                rc = BDMF_ERR_NOMEM;
                goto out;
            }
        }

        rdd_service_queue_addr_cfg(new_queue_cfg->queue_id, (uint32_t)VIRT_TO_PHYS(service_queue_aligned_addr), service_queue_size);
    }
#endif

    rdd_service_queue_cfg(new_queue_cfg->queue_id, drop_threshold, rate_limit, rdd_queue_profile);

    BDMF_TRACE_DBG_OBJ(mo, "rdd_service_queue_cfg(%d, %d, %d, %d) -> %d\n",
            (int)new_queue_cfg->queue_id, drop_threshold, (int)rate_limit, (int)rdd_queue_profile, rc);

out:
    if (rc)
    {
        BDMF_TRACE_RET_OBJ(BDMF_ERR_INTERNAL, mo,
            "Failed to configure service queue %d err %d\n",
            new_queue_cfg->queue_id, rc);
    }
    return 0;
}

static int egress_tm_service_q_enable_set(struct bdmf_object *mo, bdmf_boolean enable)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    int i = 0, rc = 0;

    rc = egress_tm_service_queue_rdd_cfg(mo, &tm->queue_cfg[0], &tm->rl_cfg, enable);
    if (rc)
        goto err;

    if (tm->level != rdpa_tm_level_egress_tm)
        return 0;

    /* Enable/disable subsidiary TMs if any */
    for (i = 0; i < RDPA_TM_MAX_SCHED_ELEMENTS && !rc; i++)
    {
        if (tm->sub_tms[i])
        {
            rc = egress_tm_service_q_enable_set(tm->sub_tms[i], enable);
            if (rc)
                goto err_sub_queues;
        }
    }
    return 0;


err_sub_queues:
    /* Failure. Roll-back */
    for (--i; i >= 0; i--)
    {
        if (tm->sub_tms[i])
            egress_tm_service_q_enable_set(tm->sub_tms[i], !enable);
    }
err:
    egress_tm_service_queue_rdd_cfg(mo, &tm->queue_cfg[0], &tm->rl_cfg, !enable);
    return rc;
}

/* Configure / remove egress queue - for all channels
 * It includes adding/removing queue from hash and updating RDD
 */
static int egress_tm_queue_cfg(struct bdmf_object *mo, int index, rdpa_tm_queue_cfg_t *new_queue_cfg)
{
    struct bdmf_object *top = egress_tm_get_top_object(mo);
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(top);
    tm_channel_t *channels = (tm->dir == rdpa_dir_ds) ? ds_channels : us_channels;
    int num_channels = (tm->dir == rdpa_dir_ds) ? RDPA_MAX_DS_CHANNELS : RDPA_MAX_US_CHANNELS;
    int i;
    int rc = 0;

    /* Go over all channels and configure queue for all channels bound to the top */
    for (i = 0; i < num_channels && !rc; i++)
    {
        tm_channel_t *ch = &channels[i];
        if (ch->egress_tm == top)
            rc = egress_tm_queue_cfg_on_channel(mo, ch, index, new_queue_cfg);
    }

    return rc;
}


/* Configure overall rate limiter */
static int egress_tm_orl_config(struct bdmf_object *mo, rdpa_tm_rl_cfg_t *rl_cfg, bdmf_boolean enable)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    rl_rate_size_t rate = (enable && rl_cfg) ? rl_cfg->af_rate : 0;
    int rc = 0;

    if (tm->dir == rdpa_dir_us)
    {
#ifdef EPON
        int rdd_rc;
        if (rdpa_is_epon_or_xepon_mode() || rdpa_is_epon_ae_mode())
        {
            if (orl_tm_linked_to_llid)
            {
                if (NULL != global_shaper_cb)
                {
                    rdd_rc = global_shaper_cb((uint32_t)rate);
                    BDMF_TRACE_RET_OBJ(rdd_rc ? BDMF_ERR_INTERNAL : 0, mo,
                        "OntDirGlobalShaperSet({rate=%d}) -> %d\n", (int)rate, rdd_rc);
                }
            }
            else
            {
                tm->rl_cfg.af_rate = rate;
            }
        }
        else
#endif
        rc = rdpa_rdd_orl_rate_cfg(mo, rate / 8);
    }

    return rc;
}

/* Configure rate controller - on a single channel */
static int egress_tm_rl_config_on_channel(struct bdmf_object *mo, tm_channel_t *channel, rdpa_tm_rl_cfg_t *rl_cfg,
    int weight, bdmf_boolean enable)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    tm_qtm_ctl_t *qtm_ctl = egress_tm_qtm_ctl_get(mo, channel);
    rdpa_tm_rl_cfg_t tmp_rl_cfg = {};
    bdmf_error_t rc = BDMF_ERR_OK;
    int is_wrr;

    is_wrr = tm->upper_level_tm &&
        egress_tm_is_wrr_elem((tm_drv_priv_t *)bdmf_obj_data(tm->upper_level_tm), qtm_ctl->sched_index_in_upper);

    if (!is_wrr)
        weight = 0;

    if (enable)
    {
        tmp_rl_cfg = *rl_cfg;
        if (!tmp_rl_cfg.af_rate)
        {
            tmp_rl_cfg.af_rate = RDD_RATE_UNLIMITED;
            tmp_rl_cfg.burst_size = RDD_RATE_UNLIMITED / BITS_IN_BYTE;
        }
    }

    if (!qtm_ctl)
    {
        /* If egress_tm is group-level, it doesn't support rate limiting */
        if (tm->level == rdpa_tm_level_egress_tm)
            BDMF_TRACE_RET_OBJ(BDMF_ERR_NOT_SUPPORTED, mo, "Group-level rate limiting is not supported\n");
        /* Corner case that can happen in disable */
        BUG_ON(enable);
        return 0;
    }

    rc = rdpa_rdd_qtm_ctl_modify(mo, qtm_ctl, &tmp_rl_cfg, weight, -1);
    if (rc)
        BDMF_TRACE_RET_OBJ(rc, mo, "rdd RL configuration failed. Set trace_level=debug for details\n");

    return rc;
}

/* Configure rate controller - for all channels */
static int egress_tm_rl_config(struct bdmf_object *mo, rdpa_tm_rl_cfg_t *rl_cfg, int weight, bdmf_boolean enable)
{
    struct bdmf_object *top = egress_tm_get_top_object(mo);
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    tm_channel_t *channels = (tm->dir == rdpa_dir_ds) ? ds_channels : us_channels;
    int num_channels = (tm->dir == rdpa_dir_ds) ? RDPA_MAX_DS_CHANNELS : RDPA_MAX_US_CHANNELS;
    int i, rc = 0;
#ifdef EPON
    int is_llid_tm = 0;
#endif

    if (tm->overall_rl)
        return egress_tm_orl_config(mo, rl_cfg, enable);

#ifdef EPON
    if ((tm->dir == rdpa_dir_us) && rdpa_is_epon_or_xepon_mode())
    {
        for (i = 0;; i++)
        {
            bdmf_object_handle   llid_obj = NULL;
            bdmf_object_handle   tm_obj = NULL;
            bdmf_number tmp_id;

            rc = rdpa_llid_get(i, &llid_obj);
            if (rc)
                break;  /* no more LLID object */

            rc = rdpa_llid_egress_tm_get(llid_obj, &tm_obj);
            if (rc)
                continue;

            if (!tm_obj)
                continue;

            rc = rdpa_egress_tm_index_get(tm_obj, &tmp_id);
            if (rc)
                continue;

            if (tmp_id == tm->index)
            {
                is_llid_tm = 1;
                break;
            }
        }

        if (is_llid_tm)
        { /* we set epon mac for LLID obj */
            if (NULL != epon_link_shaper_cb)
            {
                rc = epon_link_shaper_cb(i, rl_cfg->af_rate, rl_cfg->burst_size);
                BDMF_TRACE_RET_OBJ(rc ? BDMF_ERR_INTERNAL : 0, mo,
                    "OntDirLinkShaperSet(%d {rate=%d, burst=%d}) -> %d\n",
                    i, (int)rl_cfg->af_rate, (int)rl_cfg->burst_size, rc);
            }
            return rc;
        }

        rc = 0;
    }
#endif

    /* Go over all channels and configure queue for all channels bound to the top */
    for (i = 0; i < num_channels && !rc; i++)
    {
        tm_channel_t *ch = &channels[i];
        if (ch->egress_tm == top)
            rc = egress_tm_rl_config_on_channel(mo, ch, rl_cfg, weight, enable);
    }

    return rc;
}

/* Configure number of SP elements  - on a single channel */
static int egress_tm_num_sp_elements_config_on_channel(struct bdmf_object *mo, tm_channel_t *channel, rdpa_tm_num_sp_elem num_sp_elements)
{
    tm_qtm_ctl_t *qtm_ctl = egress_tm_qtm_ctl_get(mo, channel);
    bdmf_error_t rc = BDMF_ERR_OK;

    if (qtm_ctl)
    {
        rc = rdpa_rdd_qtm_ctl_modify(mo, qtm_ctl, NULL, -1, num_sp_elements);
        if (rc)
            BDMF_TRACE_RET_OBJ(rc, mo, "rdd num_sp_elements configuration failed. Set trace_level=debug for details\n");
    }

    return rc;
}

/* Configure rate controller - for all channels */
static int egress_tm_num_sp_elements_config(struct bdmf_object *mo, rdpa_tm_num_sp_elem num_sp_elements)
{
    struct bdmf_object *top = egress_tm_get_top_object(mo);
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    tm_channel_t *channels = (tm->dir == rdpa_dir_ds) ? ds_channels : us_channels;
    int num_channels = (tm->dir == rdpa_dir_ds) ? RDPA_MAX_DS_CHANNELS : RDPA_MAX_US_CHANNELS;
    int i, rc = 0;

    /* Go over all channels and configure queue for all channels bound to the top */
    for (i = 0; i < num_channels && !rc; i++)
    {
        tm_channel_t *ch = &channels[i];
        if (ch->egress_tm == top)
            rc = egress_tm_num_sp_elements_config_on_channel(mo, ch, num_sp_elements);
    }

    return rc;
}

/* Set-up overall rate limiter - single channel */
static int egress_tm_orl_set_on_channel(struct bdmf_object *mo, tm_channel_t *channel, struct bdmf_object *orl_mo,
    rdpa_tm_orl_prty orl_prty)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);

    if (tm->dir == rdpa_dir_us)
    {
        int rc;
        rc = rdpa_rdd_orl_channel_cfg(mo, channel, orl_mo ? 1 : 0, orl_prty);
        if (rc)
            return rc;
    }

    channel->orl_tm = orl_mo;

    return 0;
}

/* Set-up overall rate limiter */
static int egress_tm_orl_set(struct bdmf_object *mo, rdpa_tm_orl_prty orl_prty)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    tm_channel_t *ch = NULL;
    int rc = 0;

    if (tm->dir == rdpa_dir_us)
    {
        while ((ch = egress_tm_channel_get_next(mo, ch)) && !rc)
        {
            if (!ch->orl_tm)
                continue;
            rc = egress_tm_orl_set_on_channel(mo, ch, ch->orl_tm, orl_prty);
        }
    }
    else
    {
        BDMF_TRACE_RET_OBJ(BDMF_ERR_NOT_SUPPORTED, mo, "DS overall RL is not supported.\n");
    }

    return rc;
}

/* Check if channel is linked to overall RL object */
static int egress_tm_is_channel_rl(struct bdmf_object *orl_obj, struct bdmf_object *channel)
{
    struct bdmf_link *link = NULL;

    while ((link = bdmf_get_next_us_link(channel, link)))
    {
        if (bdmf_us_link_to_object(link) == orl_obj)
            return 1;
    }
    while ((link = bdmf_get_next_ds_link(channel, link)))
    {
        if (bdmf_ds_link_to_object(link) == orl_obj)
            return 1;
    }
    return 0;
}


/* Enable/disable egress_tm channel */
static int egress_tm_enable_set_on_channel(struct bdmf_object *mo, tm_channel_t *channel, bdmf_boolean enable)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    int i = 0;
    int rc = 0;

    BDMF_TRACE_DBG_OBJ(mo, "enable=%d channel=%d prev_enable=%d\n", enable, channel->channel_id, channel->enable);

    /* Avoid double enable/disable on top level */
    if (!tm->upper_level_tm && enable == channel->enable)
        return 0;

    /* Configure rate controller */
    if (tm->level == rdpa_tm_level_queue || tm->overall_rl)
        rc = egress_tm_rl_config_on_channel(mo, channel, &tm->rl_cfg, tm->weight, enable);

    if (tm->level == rdpa_tm_level_queue)
    {
        /* Configure queues if any */
        rdpa_tm_queue_cfg_t disable_cfg = {};
        for (i = 0; i < tm->num_queues && !rc; i++)
        {
            if (tm->queue_cfg[i].drop_threshold)
                rc = egress_tm_queue_cfg_on_channel(mo, channel, i, enable ? &tm->queue_cfg[i] : &disable_cfg);
        }
    }
    else
    {
        /* Enable/disable subsidiary TMs if any */
        for (i = 0; i < RDPA_TM_MAX_SCHED_ELEMENTS && !rc; i++)
        {
            if (tm->sub_tms[i])
                rc = egress_tm_enable_set_on_channel(tm->sub_tms[i], channel, enable);
        }
    }

    if (rc)
        goto exit;

    if (!tm->upper_level_tm)
        channel->enable = enable;

    return 0;

exit:
    /* Failure. Roll-back */
    if (tm->level == rdpa_tm_level_queue || tm->overall_rl)
        egress_tm_rl_config_on_channel(mo, channel, &tm->rl_cfg, tm->weight, !enable);
    if (tm->level == rdpa_tm_level_queue)
    {
        rdpa_tm_queue_cfg_t disable_cfg = {};
        for (--i; i >= 0; i--)
            egress_tm_queue_cfg_on_channel(mo, channel, i, !enable ? &tm->queue_cfg[i] : &disable_cfg);
    }
    else
    {
        for (--i; i >= 0; i--)
        {
            if (tm->sub_tms[i])
                egress_tm_enable_set_on_channel(tm->sub_tms[i], channel, !enable);
        }
    }

    return rc;
}


/* Store enable status in all objects in hierarchy */
static void egress_tm_enable_store(tm_drv_priv_t *tm, bdmf_boolean enable)
{
    tm->enable = enable;
    if (tm->level == rdpa_tm_level_egress_tm)
    {
        int i;
        /* Store in subsidiaries */
        for (i = 0; i < RDPA_TM_MAX_SCHED_ELEMENTS; i++)
        {
            if (tm->sub_tms[i])
                egress_tm_enable_store((tm_drv_priv_t *)bdmf_obj_data(tm->sub_tms[i]), enable);
        }
    }
}

/* Enable/disable egress_tm */
static int egress_tm_enable_set(struct bdmf_object *mo, bdmf_boolean enable)
{
    struct bdmf_object *top = egress_tm_get_top_object(mo);
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    tm_channel_t *channels = (tm->dir == rdpa_dir_ds) ? ds_channels : us_channels;
    int num_channels = (tm->dir == rdpa_dir_ds) ? RDPA_MAX_DS_CHANNELS : RDPA_MAX_US_CHANNELS;
    int i = 0;
    int rc = 0;

    if (enable && tm->rl_cfg.burst_size == 0)
        tm->rl_cfg.burst_size = tm->rl_cfg.af_rate;

    if (egress_tm_is_service_q(tm))
        return egress_tm_service_q_enable_set(mo, enable);

    if (tm->overall_rl)
        return egress_tm_orl_config(mo, &tm->rl_cfg, enable);

    /* For channel group (LLID) go over all subgroups (sunsidiaries) */
    if (egress_tm_is_ch_group(tm))
    {
        for (i = 0; i < tm->num_channels && !rc; i++)
        {
            if (tm->sub_tms[i])
                rc = egress_tm_enable_set(tm->sub_tms[i], enable);
            if (rc)
                goto err_ch_group;
        }
    }
    else
    {
        /* Go over all channels and configure queue for all channels bound to the top */
        for (i = 0; i < num_channels && !rc; i++)
        {
            tm_channel_t *ch = &channels[i];
            if (ch->egress_tm == top)
                rc = egress_tm_enable_set_on_channel(mo, ch, enable);
            if (rc)
                goto err;
        }
    }

    egress_tm_enable_store(tm, enable);
    return 0;


err_ch_group:
    /* Failure. Roll-back */
    for (--i; i >= 0; i--)
    {
        if (tm->sub_tms[i])
            rc = egress_tm_enable_set(tm->sub_tms[i], !enable);
    }
    goto exit;

err:
    for (--i; i >= 0; i--)
    {
        tm_channel_t *ch = &channels[i];
        if (ch->egress_tm == top)
            egress_tm_enable_set_on_channel(mo, ch, !enable);
    }

exit:
    return rc;
}


/*
 * egress_tm object management callbacks
 */

/** This optional callback is called called at object init time
 *  before initial attributes are set.
 *  If function returns error code !=0, object creation is aborted
 */
static int egress_tm_pre_init(struct bdmf_object *mo)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    int n;

    tm->this = mo;
    tm->index = BDMF_INDEX_UNASSIGNED;
    tm->level = rdpa_tm_level_egress_tm;
    tm->rl_rate_mode = rdpa_tm_rl_single_rate;
    tm->num_queues = RDPA_DFT_NUM_EGRESS_QUEUES;

    for (n = 0; n < RDPA_MAX_EGRESS_QUEUES; n++)
    {
        tm->queue_cfg[n].queue_id = BDMF_INDEX_UNASSIGNED;
        /* no special drop algorithm */
        tm->queue_cfg[n].drop_alg = rdpa_tm_drop_alg_dt;
    }
    tm->channel_group = BDMF_INDEX_UNASSIGNED;
    for (n = 0; n < RDPA_MAX_WAN_SUBCHANNELS; n++)
        tm->channels[n] = BDMF_INDEX_UNASSIGNED;

    return 0;
}

/** This optional callback is called at object init time
 * after initial attributes are set.
 * Its work is:
 * - make sure that all necessary attributes are set and make sense
 * - allocate dynamic resources if any
 * - assign object name if not done in pre_init
 * - finalise object creation
 * If function returns error code !=0, object creation is aborted
 */
static int egress_tm_post_init(struct bdmf_object *mo)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    struct bdmf_object **tm_objects = (tm->dir == rdpa_dir_ds) ? ds_tm_objects : us_tm_objects;
    int max_scheds = (tm->dir == rdpa_dir_ds) ? RDPA_TM_MAX_DS_SCHED : RDPA_TM_MAX_US_SCHED;
    int rc = 0;

    /* If egress tm index is set - make sure it is unique.
     * Otherwise, assign free
     */
    if (tm->index < 0)
    {
        int i;
        /* Find and assign free index */
        for (i = 0; i < max_scheds; i++)
        {
            if (!tm_objects[i])
            {
                tm->index = i;
                break;
            }
        }
    }
    if ((unsigned)tm->index >= max_scheds)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, mo, "Too many egress tms or index %ld is out of range\n", tm->index);

    if (tm->dir == rdpa_dir_ds && num_normal_ds_tm == RDPA_MAX_DS_TM_QUEUE &&
        !egress_tm_is_service_q(tm))
    {
        BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, mo, "Too many DS egress tms\n");
    }

    /* set object name */
    snprintf(mo->name, sizeof(mo->name), "egress_tm/dir=%s,index=%ld",
        bdmf_attr_get_enum_text_hlp(&rdpa_traffic_dir_enum_table, tm->dir), tm->index);

#ifndef XRDP
    /* Only SP and single_queue are supported for queue scheduler */
    if (tm->level == rdpa_tm_level_queue && (tm->mode != rdpa_tm_sched_sp && tm->mode != rdpa_tm_sched_disabled))
        BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, mo, "Scheduling mode of queue-level egress_tm must be SP or disable\n");

    if (tm->mode == rdpa_tm_sched_sp_wrr)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, mo, "SPP_WRR scheduling mode is not supported\n");
#endif

    if (tm->level == rdpa_tm_level_queue && egress_tm_is_service_q(tm) && tm->mode != rdpa_tm_sched_disabled)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, mo, "Service queue, level queue scheduling mode must be disabled\n");

    if (tm->level == rdpa_tm_level_egress_tm && tm->mode == rdpa_tm_sched_disabled && !tm->overall_rl)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, mo, "Egress_tm-level object can't be used with disabled scheduling\n");

    if (tm->level == rdpa_tm_level_egress_tm && tm->dir == rdpa_dir_ds && !egress_tm_is_service_q(tm))
        BDMF_TRACE_RET_OBJ(BDMF_ERR_NOT_SUPPORTED, mo, "Only service queue multi-level scheduling supported in DS\n");

    if (tm->overall_rl && tm->dir == rdpa_dir_ds)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_NOT_SUPPORTED, mo, "Overall RL is only supported in the US\n");

    if (tm->level == rdpa_tm_level_egress_tm && tm->dir == rdpa_dir_ds &&
        ds_service_q_tm_obj)
    {
        BDMF_TRACE_RET_OBJ(BDMF_ERR_ALREADY, mo,
            "DS service queue egress_tm level already exists (%s)\n",
            ds_service_q_tm_obj->name);
    }

    if (tm->overall_rl && us_overall_rl_obj)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_ALREADY, mo, "US overall RL object already exists (%s)\n", us_overall_rl_obj->name);

    if (tm->overall_rl && tm->mode != rdpa_tm_sched_disabled)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, mo, "Scheduling mode must be \"disabled\" on overall RL object\n");

    if (tm_objects[tm->index])
        BDMF_TRACE_RET_OBJ(BDMF_ERR_ALREADY, mo, "%s already exists\n", mo->name);

    if (tm->mode == rdpa_tm_sched_disabled)
        tm->num_queues = 1;

    /* Try to set egress_tm on parent object */
    if (mo->owner->drv == rdpa_port_drv())
    {
        rdpa_port_tm_cfg_t tm_cfg;
        rc = rdpa_port_tm_cfg_get(mo->owner, &tm_cfg);
        tm_cfg.sched = mo;
        rc = rc ? rc : rdpa_port_tm_cfg_set(mo->owner, &tm_cfg);
    }
#ifdef CONFIG_BCM_TCONT
    else if (mo->owner->drv == rdpa_tcont_drv())
    {
        rc = rdpa_tcont_egress_tm_set(mo->owner, mo);
    }
#endif
    if (rc)
        BDMF_TRACE_RET_OBJ(rc, mo, "%s: can't set on parent object %s\n", mo->name, mo->owner->name);

    /* Overall RL object is enabled by default */
    if (tm->overall_rl)
        tm->enable = 1;

    /* Service queue level egress_tm is always enables
     * Service queue level queue is enabled once set as subsidiary to the egress_tm level*/
    if (egress_tm_is_service_q(tm) && tm->level == rdpa_tm_level_egress_tm)
        tm->enable = 1;

    /* Propagate to RDD if enabled */
    if (tm->enable)
        rc = egress_tm_enable_set(mo, 1);

    if (!rc)
    {
        tm_objects[tm->index] = mo;
        if (tm->overall_rl)
            us_overall_rl_obj = mo;

        if (egress_tm_is_service_q(tm))
        {
            if (tm->level == rdpa_tm_level_egress_tm)
                ds_service_q_tm_obj = mo;
        }
        else
            num_normal_ds_tm++;
    }
    return rc;
}

static void egress_tm_channel_cleanup(struct bdmf_object *mo)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    tm_channel_t *channels = (tm->dir == rdpa_dir_ds) ? ds_channels : us_channels;
    int num_channels = (tm->dir == rdpa_dir_ds) ? RDPA_MAX_DS_CHANNELS : RDPA_MAX_US_CHANNELS;
    int i;

    for (i = 0; i < num_channels; i++)
    {
        tm_channel_t *ch = &channels[i];
        if (ch->egress_tm == mo)
        {
            BDMF_TRACE_DBG_OBJ(mo, "egress_tm_channel_cleanup channel_id %d info, E:%d, O:0x%p, E:0x%p)\n",
                ch->channel_id, ch->enable, ch->owner, ch->egress_tm);
            ch->enable = 0;
            ch->egress_tm = NULL;
        }
     }
}

static void egress_tm_destroy(struct bdmf_object *mo)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    struct bdmf_object **tm_objects = (tm->dir == rdpa_dir_ds) ? ds_tm_objects : us_tm_objects;
    int max_subs = (tm->dir == rdpa_dir_ds) ? RDPA_TM_MAX_DS_SCHED : RDPA_TM_MAX_US_SCHED;

    if ((unsigned)tm->index >= max_subs || tm_objects[tm->index] != mo)
        return;

    if (egress_tm_is_service_q(tm))
    {
        if (tm->level == rdpa_tm_level_egress_tm)
            ds_service_q_tm_obj = NULL;
        else if (tm->queue_cfg[0].queue_id != BDMF_INDEX_UNASSIGNED)
            ds_service_q_obj[tm->queue_cfg[0].queue_id] = NULL;
    }
    else
        num_normal_ds_tm--;

    if (tm->enable)
        egress_tm_enable_set(mo, 0);
    egress_tm_rdd_resources_free(mo);
    egress_tm_channel_cleanup(mo);

    tm_objects[tm->index] = NULL;
    if (mo == us_overall_rl_obj)
        us_overall_rl_obj = NULL;
}

/** Called when other object is linked with THIS object.
 * Only overall rate limiter supports "link" operation
 */
static int egress_tm_link(struct bdmf_object *this, struct bdmf_object *other,
    const char *link_attrs)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(this);
    tm_channel_t *channels = (tm->dir == rdpa_dir_ds) ? ds_channels : us_channels;
    int num_channels = (tm->dir == rdpa_dir_ds) ? RDPA_MAX_DS_CHANNELS : RDPA_MAX_US_CHANNELS;
    tm_drv_priv_t *channel_tm;
    int i;
    int rc = 0;

    if (!tm->overall_rl)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, this, "Can only be linked with overall_rl object: %s\n", other->name);

#ifdef EPON
    {
        bdmf_boolean is_overall = 0;

        if (tm->dir == rdpa_dir_us && other->drv == rdpa_llid_drv())
        {
            is_overall = 1;
        }
        else if (tm->dir == rdpa_dir_us && other->drv == rdpa_port_drv())
        {
            rdpa_if rdpaif = rdpa_if_none;
            rdpa_port_index_get(other, &rdpaif);

            if (rdpaif == rdpa_wan_type_to_if(rdpa_wan_epon))
                is_overall = 1;
        }

        if (is_overall && (rdpa_is_epon_or_xepon_mode() || rdpa_is_epon_ae_mode()))
        {
            if (_rdpa_epon_mode_get() == rdpa_epon_ctc || 
                _rdpa_epon_mode_get() == rdpa_epon_cuc)
            {
                if (NULL != global_shaper_cb)
                {
                    uint32_t rate = tm->rl_cfg.af_rate;

                    rc = global_shaper_cb(rate);
                    orl_tm_linked_to_llid = 1;
                    BDMF_TRACE_RET_OBJ(rc ? BDMF_ERR_INTERNAL : 0, this,
                        "OntDirGlobalShaperSet({rate=%d}) -> %d\n", (int)rate, rc);
                }
            }
            else
            {
                BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, this,
                    "overall_rl object's link to llid is not supported\n");
            }
        }
    }
#endif

    /* Set ORL for all channels owned by ""other" */
    for (i = 0; i < num_channels && !rc; i++)
    {
        tm_channel_t *channel = &channels[i];
        if (channel->owner == other)
        {
            channel_tm = (tm_drv_priv_t *)bdmf_obj_data(channel->egress_tm);
            rc = egress_tm_orl_set_on_channel(channel->egress_tm, channel, this, channel_tm->orl_prty);
        }
    }

    return rc;
}

/** Called when object's downlink is disconnected */
static void egress_tm_unlink(struct bdmf_object *this, struct bdmf_object *other)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(this);
    tm_channel_t *channels = (tm->dir == rdpa_dir_ds) ? ds_channels : us_channels;
    int num_channels = (tm->dir == rdpa_dir_ds) ? RDPA_MAX_DS_CHANNELS : RDPA_MAX_US_CHANNELS;
    int i;

#ifdef EPON
    if (other->drv == rdpa_llid_drv())
    {
        if (_rdpa_epon_mode_get() == rdpa_epon_ctc || 
            _rdpa_epon_mode_get() == rdpa_epon_cuc)
        {
            orl_tm_linked_to_llid = 0;
        }
    }
#endif

    /* Clear ORL for all channels owned by ""other" */
    for (i = 0; i < num_channels; i++)
    {
        tm_channel_t *channel = &channels[i];
        if (channel->owner == other)
            egress_tm_orl_set_on_channel(channel->egress_tm, channel, NULL, 0);
    }
}

/*
 * egress tm attribute access
 */

/** "enable" attribute's "write" callback */
static int egress_tm_attr_enable_write(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, const void *val, uint32_t size)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    bdmf_boolean enable = *(bdmf_boolean *)val;
    int rc = 0;

    /* This attribute is only writable for top-level scheduler */
    if (mo->owner && mo->owner->drv == mo->drv)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_NOT_SUPPORTED, mo, "enable attribute is RO for subsidiary egress_tm\n");
    if (enable == tm->enable)
        return 0;
    if (mo->state != bdmf_state_init)
        rc = egress_tm_enable_set(mo, enable);
    else
        tm->enable = enable;

    return rc;
}

/** "rl" attribute's "write" callback */
static int egress_tm_attr_rl_write(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, const void *val, uint32_t size)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_tm_rl_cfg_t *rl_cfg = (rdpa_tm_rl_cfg_t *)val;
    int rc;

    if (mo->state != bdmf_state_init && tm->enable)
    {
        if (rl_cfg->burst_size == 0)
#ifdef XRDP
            rl_cfg->burst_size = rdd_rate_to_alloc_unit((rl_cfg->af_rate / BITS_IN_BYTE),
                RATE_LIMITER_TIMER_PERIOD_IN_USEC);
#else
            rl_cfg->burst_size = rdd_budget_to_alloc_unit((rl_cfg->af_rate / BITS_IN_BYTE), 1000, 0);
#endif
        if (egress_tm_is_service_q(tm))
            rc = egress_tm_service_queue_rdd_cfg(mo, &tm->queue_cfg[0], rl_cfg, tm->enable);
        else
            rc = egress_tm_rl_config(mo, rl_cfg, tm->weight, tm->enable);
        if (rc < 0)
            return rc;
    }
    tm->rl_cfg = *rl_cfg;

    return 0;
}

int egress_tm_attr_rl_rate_mode_read(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index, void *val,
    uint32_t size)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_tm_rl_rate_mode *rl_rate_mode = (rdpa_tm_rl_rate_mode *)val;

#ifndef XRDP
    if (tm->level == rdpa_tm_level_queue)
        return BDMF_ERR_NOENT;
#endif

    *rl_rate_mode = tm->rl_rate_mode;
    return 0;
}

static int egress_tm_attr_rl_rate_mode_write(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    const void *val, uint32_t size)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_tm_rl_rate_mode *rl_rate_mode = (rdpa_tm_rl_rate_mode *)val;

#ifndef XRDP
    if (tm->level == rdpa_tm_level_queue && *rl_rate_mode != rdpa_tm_rl_single_rate)
        return BDMF_ERR_INVALID_OP;
#endif

    tm->rl_rate_mode = *rl_rate_mode;

    return 0;
}

static int egress_tm_attr_num_queues_read(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index, void *val,
    uint32_t size)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    uint8_t *p_num_queues = (uint8_t *)val;

    if (tm->level != rdpa_tm_level_queue)
        return BDMF_ERR_NOENT;

    *p_num_queues = tm->num_queues;
    return 0;
}

static int egress_tm_attr_num_queues_write(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    const void *val, uint32_t size)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    uint8_t num_queues = *(uint8_t *)val;

    if (tm->level != rdpa_tm_level_queue)
        return BDMF_ERR_INVALID_OP;

    tm->num_queues = num_queues;

    return 0;
}

static int egress_tm_attr_num_sp_elements_read(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index, void *val,
    uint32_t size)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_tm_num_sp_elem *p_num_sp_elements = (rdpa_tm_num_sp_elem *)val;

    if (tm->mode != rdpa_tm_sched_sp_wrr)
        return BDMF_ERR_NOT_SUPPORTED;

    *p_num_sp_elements = tm->num_sp_elements;
    return 0;
}

static int egress_tm_attr_num_sp_elements_write(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    const void *val, uint32_t size)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_tm_num_sp_elem num_sp_elements = *(rdpa_tm_num_sp_elem *)val;
    int rc = 0;
    int i;

    if (tm->mode != rdpa_tm_sched_sp_wrr)
        return BDMF_ERR_NOT_SUPPORTED;
    if (tm->level == rdpa_tm_level_queue && num_sp_elements > tm->num_queues)
        return BDMF_ERR_RANGE;
    /* Must be a power of 2 > 0 */
    if (num_sp_elements & (num_sp_elements - 1))
        return BDMF_ERR_PARM;

    /* If TM object is already active, try to change num_sp_elements on the fly */
    if (mo->state == bdmf_state_active)
    {
        /* All WRR elements must have weight */
        if (tm->level == rdpa_tm_level_egress_tm)
        {
            for (i = num_sp_elements; i < tm->num_queues; i++)
            {
                if (tm->sub_tms[i])
                {
                    tm_drv_priv_t *stm = (tm_drv_priv_t *)bdmf_obj_data(tm->sub_tms[i]);
                    if (!(stm->weight >= RDPA_MIN_WEIGHT && stm->weight <= RDPA_MAX_WEIGHT))
                    {
                        BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, mo, "WRR weight is not set for %s\n",
                            tm->sub_tms[i]->name);
                    }
                }
            }
        }
        else
        {
            for (i = num_sp_elements; i < tm->num_queues; i++)
            {
                rdpa_tm_queue_cfg_t *q_cfg = &tm->queue_cfg[i];
                if (q_cfg->drop_threshold &&
                    !(q_cfg->weight >= RDPA_MIN_WEIGHT && q_cfg->weight <= RDPA_MAX_WEIGHT))
                {
                    BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, mo, "WRR weight is not set for queue %d of %s\n",
                        i, mo->name);
                }
            }
        }
        rc = egress_tm_num_sp_elements_config(mo, num_sp_elements);
    }

    if (!rc)
        tm->num_sp_elements = num_sp_elements;

    return rc;
}

/* "queue_cfg" attribute "read" callback */
static int egress_tm_attr_queue_cfg_read(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, void *val, uint32_t size)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_tm_queue_cfg_t *cfg = (rdpa_tm_queue_cfg_t *)val;

    if (tm->level != rdpa_tm_level_queue)
        return BDMF_ERR_NOT_SUPPORTED;
    if (index >= tm->num_queues)
        return BDMF_ERR_RANGE;
    if (tm->mode == rdpa_tm_sched_disabled && index)
        return BDMF_ERR_RANGE;
    if (!tm->queue_cfg[index].drop_threshold && !tm->queue_id_assigned[index])
        return BDMF_ERR_NOENT;
    *cfg = tm->queue_cfg[index];

    return 0;
}

/* get queue index from queue id */
static int get_queue_index(tm_drv_priv_t *tm, int queue_id)
{
    int i;

    if (queue_id == BDMF_INDEX_UNASSIGNED)
        return BDMF_INDEX_UNASSIGNED;
    
    for (i = 0; i < tm->num_queues; i++)
        if (tm->queue_cfg[i].queue_id == queue_id)
            return i;

    return BDMF_INDEX_UNASSIGNED;
}

/* delete single queue from a channel */
static int egress_tm_attr_queue_cfg_delete(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    tm_channel_t *ch = egress_tm_channel_get_first(mo);
    bdmf_error_t rc = BDMF_ERR_NOENT;
    
    if (index >= RDPA_MAX_EGRESS_QUEUES)
    {
        BDMF_TRACE_ERR("Queue index (%ld) out of range\n", index);
        return BDMF_ERR_RANGE;
    }

    while (ch)
    {
        tm_qtm_ctl_t *qtm_ctl = egress_tm_qtm_ctl_get(mo, ch);
        if (qtm_ctl)
        {
            rc = BDMF_ERR_OK;
            /* we found an egress_tm channel- delete the relevant queue */
            BDMF_TRACE_DBG_OBJ(mo, "%s: delete queue:: channel: %d, queue_id: %ld\n", mo->name, ch->channel_id, index);
            egress_tm_delete_single_queue(mo, qtm_ctl, ch, tm, index);
            tm->queue_cfg[index].queue_id = BDMF_INDEX_UNASSIGNED;
            tm->queue_cfg[index].drop_threshold = 0;
        }
         
        ch = egress_tm_channel_get_next(mo, ch); 
    }
    
    if (rc)
    {
        BDMF_TRACE_ERR("Could not find requested entry for queue %ld\n", index);
    }
    return rc;
}

static int egress_tm_queue_cfg_val_to_s(struct bdmf_object *mo, struct bdmf_attr *ad, const void *val, char *sbuf, uint32_t size)
{
    rdpa_tm_queue_cfg_t *cfg = (rdpa_tm_queue_cfg_t *)val;
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    queue_id_info_t queue_id_info = {0};
    int queue_index;
    int rc;
    
    queue_index = get_queue_index(tm, cfg->queue_id);

    if (queue_index == BDMF_INDEX_UNASSIGNED)
        return BDMF_ERR_NOENT;

    rc = snprintf(sbuf, size, "{queue_id=%d,drop_threshold=%d,weight=%d,drop_alg=%s",
                  cfg->queue_id,
                  cfg->drop_threshold,
                  cfg->weight,
                  bdmf_attr_get_enum_text_hlp(&tm_drop_policy_enum_table, cfg->drop_alg));
    if ((int)size - rc < 0)
        return BDMF_ERR_INTERNAL;
    size -= rc;
    sbuf += rc;
    if (cfg->drop_alg == rdpa_tm_drop_alg_red || cfg->drop_alg == rdpa_tm_drop_alg_wred)
    {
#if defined(DSL_63138) || defined(DSL_63148) || defined(WL4908)
        rc = snprintf(sbuf, size, ",high_class={min_thresh=%d,max_thresh=%d},low_class={min_thresh=%d,max_thresh=%d}"
                      "priority_mask_0=0x%08x,priority_mask_1=0x%08x",
                      cfg->high_class.min_threshold,
                      cfg->high_class.max_threshold,
                      cfg->low_class.min_threshold,
                      cfg->low_class.max_threshold,
                      cfg->priority_mask_0,
                      cfg->priority_mask_1);
#else
        rc = snprintf(sbuf, size, ",high_class={min_thresh=%d,max_thresh=%d,max_drop_prob=%d},low_class={min_thresh=%d,max_thresh=%d,max_drop_prob=%d}",
                      cfg->high_class.min_threshold,
                      cfg->high_class.max_threshold,
                      cfg->high_class.max_drop_probability,
                      cfg->low_class.min_threshold,
                      cfg->low_class.max_threshold,
                      cfg->low_class.max_drop_probability);
#endif
        if ((int)size - rc < 0)
            return BDMF_ERR_INTERNAL;
        size -= rc;
        sbuf += rc;
    }
#ifdef XRDP
    if (cfg->rl_cfg.af_rate || cfg->rl_cfg.be_rate || cfg->rl_cfg.burst_size)
    {
        rc = snprintf(sbuf, size, ",rl={af=%llu,be=%llu,burst=%llu}",
                    (long long int)cfg->rl_cfg.af_rate,
                    (long long int)cfg->rl_cfg.be_rate,
                    (long long int)cfg->rl_cfg.burst_size);
        if ((int)size - rc < 0)
            return BDMF_ERR_INTERNAL;
        size -= rc;
        sbuf += rc;
    }
#endif
    rc = snprintf(sbuf, size, ",stat_enable=%s, active=%s}", 
            cfg->stat_enable ? "yes" : "no",
            tm->queue_configured[queue_index] ? "yes" : "no");
    if ((int)size - rc < 0)
        return BDMF_ERR_INTERNAL;
    size -= rc;
    sbuf += rc;

    if (bdmf_global_trace_level == bdmf_trace_level_debug)
    {
        tm_channel_t *ch = egress_tm_channel_get_first(mo);
        
        if (ch == NULL)
        {
            return BDMF_ERR_INTERNAL;
        }

        if (egress_tm_queue_id_info_get(tm->dir, ch->channel_id, cfg->queue_id, &queue_id_info) == 0)
        {
            rc = snprintf(sbuf, size, "===>debug info: channel=%d queue=%d, rate_controller=%d",
                          _rdd_wan_channel(queue_id_info.channel), queue_id_info.queue, queue_id_info.rc_id);
        }
        else
        {
            rc = snprintf(sbuf, size, "===>debug get queue_id_info FAILED. params: DIR %d, channel_id %d, queue_id %d\n",
                    tm->dir, ch->channel_id, cfg->queue_id);
        }

        if ((int)size - rc < 0)
        {
            return BDMF_ERR_INTERNAL;
        }
    }
    return 0;
}

/* "queue_cfg" attribute "write" callback */
static int egress_tm_attr_queue_cfg_write(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, const void *val, uint32_t size)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_tm_queue_cfg_t cfg = *(rdpa_tm_queue_cfg_t *)val;
    bdmf_error_t rc;

    if (tm->level != rdpa_tm_level_queue)
        return BDMF_ERR_NOT_SUPPORTED;
    if (tm->mode == rdpa_tm_sched_disabled && index)
        return BDMF_ERR_RANGE;
    if (index >= tm->num_queues)
        return BDMF_ERR_RANGE;
#ifdef XRDP
    cfg.stat_enable = 1;
#endif
    if (egress_tm_is_service_q(tm))
    {
        if (index)
            BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, mo, "Only queue_cfg[0] is supported on service queue\n");

        if (ds_service_q_obj[cfg.queue_id] &&
            ds_service_q_obj[cfg.queue_id] != mo)
        {
            BDMF_TRACE_RET_OBJ(BDMF_ERR_ALREADY, mo,
                "DS service queue %d already exist in %s\n", cfg.queue_id,
                ds_service_q_obj[cfg.queue_id]->name);
        }
    }

    if (mo->state != bdmf_state_init && tm->enable)
    {
        /* In this level it's good enough to work in single queue resolution */
        if (egress_tm_is_service_q(tm))
            rc = egress_tm_service_q_enable_set(mo, 1);
        else
            rc = egress_tm_queue_cfg(mo, index, &cfg);

        if (rc < 0)
            return rc;
    }
    tm->queue_cfg[index] = cfg;
    if (egress_tm_is_service_q(tm) && ds_service_q_obj[cfg.queue_id] == NULL)
        ds_service_q_obj[cfg.queue_id] = mo;

    return 0;
}

/* read statistics of a single queue given RDD indexes */
static int egress_tm_queue_stat_read(tm_drv_priv_t *tm, tm_queue_hash_entry_t *qentry, rdpa_stat_1way_t *stat)
{
    return rdpa_rdd_tx_queue_stat_read(tm->this, qentry, stat);
}

/* clear statistics of a single queue given RDD indexes */
static int egress_tm_queue_stat_clear(tm_drv_priv_t *tm, tm_queue_hash_entry_t *qentry)
{
    return rdpa_rdd_tx_queue_stat_clear(tm->this, qentry);
}

/* "queue_stat" attribute "read" callback */
static int egress_tm_attr_queue_stat_read(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, void *val, uint32_t size)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_stat_1way_t *stat = (rdpa_stat_1way_t *)val;
    rdpa_tm_queue_index_t *qi = (rdpa_tm_queue_index_t *)index;
    struct bdmf_object *root = egress_tm_get_root_object(mo);
    tm_drv_priv_t *root_tm = (tm_drv_priv_t *)bdmf_obj_data(root);
    tm_queue_hash_entry_t *qentry;
    int16_t hashed_channel_id;
    int rc = 0;

    if (!qi || !val)
        return BDMF_ERR_PARM;

    /* UNASSIGNED means "all queues" */
    if (*(bdmf_index *)qi == BDMF_INDEX_UNASSIGNED)
    {
        /* Flush all */
        int i;

        /* Special handling for channel group - sum-up for all subsidiaries */
        if (mo == root && egress_tm_is_ch_group(root_tm))
        {
            rdpa_stat_1way_t subch_stat;

            for (i = 0; i < RDPA_TM_MAX_SCHED_ELEMENTS && !rc; i++)
            {
                if (tm->sub_tms[i])
                {
                    memset(&subch_stat, 0, sizeof(rdpa_stat_1way_t));

                    rc = egress_tm_attr_queue_stat_read(tm->sub_tms[i],
                        ad, index, &subch_stat, size);

                    stat->passed.packets    += subch_stat.passed.packets;
                    stat->passed.bytes      += subch_stat.passed.bytes;
                    stat->discarded.packets += subch_stat.discarded.packets;
                    stat->discarded.bytes   += subch_stat.discarded.bytes;
                }
            }
        }
        else
        {
            struct bdmf_object *top = egress_tm_get_top_object(mo);
            tm_channel_t *channels = (tm->dir == rdpa_dir_ds) ? ds_channels : us_channels;
            int num_channels = (tm->dir == rdpa_dir_ds) ? RDPA_MAX_DS_CHANNELS : RDPA_MAX_US_CHANNELS;
            rdpa_stat_1way_t queue_stat;

            for (i = 0; i < num_channels && !rc; i++)
            {
                tm_channel_t *ch = &channels[i];
                if (ch->egress_tm == top)
                {
                    qentry = egress_tm_hash_get_by_dir_channel_queue(tm->dir, ch->channel_id, qi->queue_id);
                    if (qentry)
                    {
                        rc = egress_tm_queue_stat_read(tm, qentry, &queue_stat);
                        stat->passed.packets    += queue_stat.passed.packets;
                        stat->passed.bytes      += queue_stat.passed.bytes;
                        stat->discarded.packets += queue_stat.discarded.packets;
                        stat->discarded.bytes   += queue_stat.discarded.bytes;
                    }
                }
            }
        }
    }
    else
#if defined(DSL_63138) || defined(DSL_63148) || defined(WL4908)
        /* Queue statistics are implemented using Runner Counters that are cleared after read.
         * To avoid counters being cleared during subsidiary tm stat querries, we only return
         * the statistics for all queues when root tm is querried.
         */
        if (mo == root)

        {
            /* Get stats of specific queue */
            hashed_channel_id = egress_tm_is_group_owner(root_tm) ? root_tm->channel_group : qi->channel;
            qentry = egress_tm_hash_get_by_dir_channel_queue(tm->dir, hashed_channel_id, qi->queue_id);
            if (qentry)
                rc = egress_tm_queue_stat_read(tm, qentry, stat);
            else
                rc = BDMF_ERR_NOENT;
        }
#else
        {
            int i;
            struct bdmf_object *top = egress_tm_get_top_object(mo);
            tm_channel_t *channels = (tm->dir == rdpa_dir_ds) ? ds_channels : us_channels;
            int num_channels = (tm->dir == rdpa_dir_ds) ? RDPA_MAX_DS_CHANNELS : RDPA_MAX_US_CHANNELS;
            memset(stat, 0, sizeof(rdpa_stat_1way_t));
            for (i = 0; i < num_channels && !rc; i++)
            {
                tm_channel_t *ch = &channels[i];
                if (ch->egress_tm == top)
                {
                    hashed_channel_id = egress_tm_is_group_owner(root_tm) ? root_tm->channel_group : ch->channel_id;
                    BDMF_TRACE_DBG_OBJ(mo, "egress_tm_hash_get_by_dir_channel_queue(%d %d)\n", (int)hashed_channel_id, (int)qi->queue_id);
                    qentry = egress_tm_hash_get_by_dir_channel_queue(tm->dir, hashed_channel_id, qi->queue_id);
                    if (qentry)
                    {
                        if (rdpa_is_car_mode() && (tm->dir == rdpa_dir_us) && (!rc))
                        {
                            rdpa_stat_1way_t queue_stat;
                            memset(&queue_stat, 0, sizeof(rdpa_stat_1way_t));
                            rc = egress_tm_queue_stat_read(tm, qentry, &queue_stat);
                            stat->passed.packets    += queue_stat.passed.packets;
                            stat->passed.bytes      += queue_stat.passed.bytes;
                            stat->discarded.packets += queue_stat.discarded.packets;
                            stat->discarded.bytes   += queue_stat.discarded.bytes;
                        }
                        else
                            rc = egress_tm_queue_stat_read(tm, qentry, stat);
                   }
                }
            }
        }


#endif
    return rc;
}

/* "queue_stat" attribute "write" callback */
static int egress_tm_attr_queue_stat_write(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, const void *val, uint32_t size)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_tm_queue_index_t *qi = (rdpa_tm_queue_index_t *)index;
    struct bdmf_object *root = egress_tm_get_root_object(mo);
    tm_drv_priv_t *root_tm = (tm_drv_priv_t *)bdmf_obj_data(root);
    tm_queue_hash_entry_t *qentry;
    int16_t hashed_channel_id;
    int rc = 0;

    if (egress_tm_is_service_q(tm))
        return BDMF_ERR_NOENT;

    if (!qi || !val)
        return BDMF_ERR_PARM;

    /* UNASSIGNED means "all queues" */
    if (*(bdmf_index *)qi == BDMF_INDEX_UNASSIGNED)
    {
        /* Flush all */
        int i;

        /* Special handling for channel group - sum-up for all subsidiaries */
        if (mo == root && egress_tm_is_ch_group(root_tm))
        {
            rdpa_stat_1way_t subch_stat;

            for (i = 0; i < RDPA_TM_MAX_SCHED_ELEMENTS && !rc; i++)
            {
                if (tm->sub_tms[i])
                {
                    rc = egress_tm_attr_queue_stat_write(tm->sub_tms[i],
                        ad, index, &subch_stat, size);
                }
            }
        }
        else
        {
            struct bdmf_object *top = egress_tm_get_top_object(mo);
            tm_channel_t *channels = (tm->dir == rdpa_dir_ds) ? ds_channels : us_channels;
            int num_channels = (tm->dir == rdpa_dir_ds) ? RDPA_MAX_DS_CHANNELS : RDPA_MAX_US_CHANNELS;

            for (i = 0; i < num_channels && !rc; i++)
            {
                tm_channel_t *ch = &channels[i];
                if (ch->egress_tm == top)
                {
                    qentry = egress_tm_hash_get_by_dir_channel_queue(tm->dir, ch->channel_id, qi->queue_id);
                    if (qentry)
                        rc = egress_tm_queue_stat_clear(tm, qentry);
                }
            }
        }
    }
    else
#if !defined(DSL_63138) && !defined(DSL_63148) && !defined(WL4908)
        /* Queue statistics are implemented using Runner Counters that are cleared after read.
         * To avoid counters being cleared during subsidiary tm stat querries, we only return
         * the statistics for all queues when root tm is querried.
         */
        if (mo == root)
#endif
        {
            /* Get stats of specific queue */
            hashed_channel_id = egress_tm_is_group_owner(root_tm) ? root_tm->channel_group : qi->channel;
            qentry = egress_tm_hash_get_by_dir_channel_queue(tm->dir, hashed_channel_id, qi->queue_id);
            if (qentry)
                rc = egress_tm_queue_stat_clear(tm, qentry);
            else
                rc = BDMF_ERR_NOENT;
        }
    return rc;
}

static int egress_tm_attr_deprecated_read(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, void *val, uint32_t size)
{
    return BDMF_ERR_NOT_SUPPORTED;
}

static int egress_tm_attr_deprecated_get_next(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index *index)
{
    return BDMF_ERR_NO_MORE;
}


static int egress_tm_attr_all_queues_stat_read(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, void *val, uint32_t size)
{
    int rc = 0;
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_stat_1way_t *stat = (rdpa_stat_1way_t *)val;

    memset(stat, 0, sizeof(rdpa_stat_1way_t));

    if (egress_tm_is_service_q(tm))
        return egress_tm_attr_service_queue_stat_read_ex(tm, stat);

    rc = egress_tm_attr_queue_stat_read(mo, ad, index, stat, size);
    return rc;
}

#if defined(DSL_63138) || defined(DSL_63148) || defined(WL4908)
/* read queue occupancy of a single queue given RDD indexes */
static int egress_tm_queue_occupancy_read(tm_drv_priv_t *tm, int channel_id, int rc_id, int prty, uint32_t *queue_occupancy)
{
    *queue_occupancy = 0;

    if (tm->dir == rdpa_dir_ds)
        return rdd_eth_tx_queue_get_occupancy(channel_id, prty, queue_occupancy);
    else
        return rdd_wan_tx_queue_get_occupancy(_rdd_wan_channel(channel_id), rc_id, prty, queue_occupancy);
}
#endif

/* "queue_occupancy" attribute "read" callback */
static int egress_tm_attr_queue_occupancy_read(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, void *val, uint32_t size)
{
#if defined(DSL_63138) || defined(DSL_63148) || defined(WL4908)
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    uint32_t *queue_occupancy = (uint32_t *)val;
    rdpa_tm_queue_index_t *qi = (rdpa_tm_queue_index_t *)index;
    int rc_id, prty;
    struct bdmf_object *root = egress_tm_get_root_object(mo);
    tm_drv_priv_t *root_tm = (tm_drv_priv_t *)bdmf_obj_data(root);
    int16_t hashed_channel_id;
    int rc = 0;

    if (egress_tm_is_service_q(tm))
        return BDMF_ERR_NOENT;

    if (!qi || !val)
        return BDMF_ERR_PARM;

    /* Get occupancy of specific queue */
    hashed_channel_id = egress_tm_is_group_owner(root_tm) ? root_tm->channel_group : qi->channel;
    rc = _rdpa_egress_tm_dir_channel_queue_to_rdd(tm->dir, hashed_channel_id, qi->queue_id, &rc_id, &prty);
    if (!rc)
    {
        BDMF_TRACE_DBG_OBJ(mo, "egress_tm_queue_occupancy_read(%d %d %d)\n", (int)qi->channel, (int)rc_id, (int)prty);
        rc = egress_tm_queue_occupancy_read(tm, qi->channel, rc_id, prty, queue_occupancy);
    }
    return rc;
#else
    return BDMF_ERR_NOT_SUPPORTED;
#endif
}

/* "sub_tm" attribute "read" callback */
static int egress_tm_attr_sub_tm_read(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index, void *val,
    uint32_t size)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);

    if (tm->level != rdpa_tm_level_egress_tm)
        return BDMF_ERR_NOT_SUPPORTED;
    if (!tm->sub_tms[index])
        return BDMF_ERR_NOENT;

    *(bdmf_object_handle *)val = tm->sub_tms[index];

    return 0;
}

/* check if egress_tm RDD resources has to be re-allocated.
 * It happens when egress_tm is (group, SP) and subsidiaries are configured
 * out of order. In this case it is important to ensure that rate controllers are
 * allocated to subsidieries in increasing order because RC index signifies priority
 */
static int egress_tm_is_reallocate(tm_drv_priv_t *tm, int new_ss_index)
{
    int i;

    if (tm->mode != rdpa_tm_sched_sp && tm->mode != rdpa_tm_sched_sp_wrr)
        return 0;

    for (i = new_ss_index + 1; i < RDPA_TM_MAX_SCHED_ELEMENTS; i++)
    {
        if (tm->sub_tms[i])
            return 1;
    }

    return 0;
}

/* "sub_tm" attribute "write" callback */
static int egress_tm_attr_sub_tm_write(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index, const void *val,
    uint32_t size)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    bdmf_object_handle sub_tm = *(bdmf_object_handle *)val;
    tm_drv_priv_t *ss = sub_tm ? (tm_drv_priv_t *)bdmf_obj_data(sub_tm) : NULL;
    bdmf_object_handle old_sub_tm;
    tm_drv_priv_t *old_ss = NULL;
    bdmf_object_handle old_parent = NULL;
    int enable = tm->enable;
    bdmf_error_t rc = 0;

    if (tm->level != rdpa_tm_level_egress_tm)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_NOT_SUPPORTED, mo, "Queue-level egress_tm\n");

    if (tm->overall_rl)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_NOT_SUPPORTED, mo, "Overall RL egress_tm\n");

    if (egress_tm_is_ch_group(tm) && (unsigned)index >= tm->num_channels)
    {
        BDMF_TRACE_RET_OBJ(BDMF_ERR_RANGE, mo,
            "Subsidiary index %ld exceeds number of channels in channel "
            "group (%d)\n", index, tm->num_channels);
    }

    if (tm->sub_tms[index] == sub_tm)
        return 0; /* Nothing to do */

    /* Basic validation */
    if (sub_tm)
    {
        if (sub_tm->drv != rdpa_egress_tm_drv())
            BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, mo, "Wrong object type of %s\n", sub_tm->name);

        if (ss->upper_level_tm)
        {
            BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, mo, "%s is subsidiary to %s\n", sub_tm->name,
                ss->upper_level_tm->name);
        }

        if (ss->dir != tm->dir)
            BDMF_TRACE_RET_OBJ(BDMF_ERR_NOT_SUPPORTED, sub_tm, "Subsidiary egress tm direction mismatch\n");
    }

    /* Release resources associated with old subsidiary scheduler */
    old_sub_tm = tm->sub_tms[index];
    if (old_sub_tm)
    {
        bdmf_object_handle system_obj = NULL;

        old_ss = (tm_drv_priv_t *)bdmf_obj_data(old_sub_tm);

        if (mo->state != bdmf_state_init)
        {
            if (tm->enable)
                egress_tm_enable_set(old_sub_tm, 0);
            if (egress_tm_is_ch_group(tm))
                _rdpa_egress_tm_channel_set(old_sub_tm, NULL, tm->channels[index]);
            else
                egress_tm_rdd_resources_free(old_sub_tm);
        }
        old_ss->upper_level_tm = NULL;
        /* Re-parent to system */
        rdpa_system_get(&system_obj);
        bdmf_object_parent_set(old_sub_tm, system_obj);
        bdmf_put(system_obj);
    }

    /* Set new subsidiary */
    tm->sub_tms[index] = sub_tm;
    if (sub_tm)
    {
        old_parent = sub_tm->owner;

        /* Make sure that total number of scheduling levels is not exceeded */
        rc = egress_tm_check_levels(mo);
        if (rc)
            goto done;

        /* If root TM is channel group (LLID), each subsidiary is considered top in its hierarchy */
        ss->upper_level_tm = egress_tm_is_ch_group(tm) ? NULL : mo;
        bdmf_object_parent_set(sub_tm, mo);

        /* (Re)allocate RDD resources and enable if necessary */
        if (egress_tm_channel_get_first(mo) || egress_tm_is_service_q(tm) || egress_tm_is_ch_group(tm))
        {
            bdmf_object_handle alloc_tm = sub_tm; /* tm to allocate resources on */
            if (egress_tm_is_ch_group(tm))
            {
                _rdpa_egress_tm_channel_set(sub_tm, tm->channel_group_owner, tm->channels[index]);
            }
            else
            {
                /* Subsidiary of service q must be service q type */
                if (egress_tm_is_service_q(tm) && !egress_tm_is_service_q((tm_drv_priv_t *)bdmf_obj_data(alloc_tm)))
                {
                    BDMF_TRACE_DBG_OBJ(mo, "Subsidiary of egress_tm type "
                        "service queue must be of type service queue as "
                        "well\n");
                    rc = BDMF_ERR_PARM;
                    goto done;
                }

                rc = egress_tm_validate_hierarchy(egress_tm_get_top_object(mo));
                if (!rc && egress_tm_is_reallocate(tm, index))
                {
                    tm->sub_tms[index] = NULL; /* disconnect for the moment */
                    egress_tm_enable_set(mo, 0);
                    egress_tm_rdd_resources_free(mo);
                    alloc_tm = mo;
                    tm->sub_tms[index] = sub_tm;
                }
                rc = rc ? rc : egress_tm_rdd_resources_alloc(alloc_tm, index);
            }

            rc = rc ? rc : egress_tm_enable_set(alloc_tm, enable);
        }
    }

done:
    if (rc < 0 && sub_tm)
    {
        if (egress_tm_is_ch_group(tm))
            _rdpa_egress_tm_channel_set(sub_tm, NULL, tm->channels[index]);
        else
            egress_tm_rdd_resources_free(sub_tm);
        ss->upper_level_tm = NULL;
        tm->sub_tms[index] = NULL;
        bdmf_object_parent_set(sub_tm, old_parent);
        /* Try to restore old subsidiary if any */
        if (old_sub_tm)
            egress_tm_attr_sub_tm_write(mo, ad, index, &old_sub_tm, size);
        BDMF_TRACE_RET_OBJ(rc, sub_tm, "failed to configure egress tm\n");
    }
    return 0;
}

/** "weight" attribute's "write" callback */
static int egress_tm_attr_weight_write(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index, const void *val,
    uint32_t size)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    uint32_t weight = *(uint32_t *)val;

    if (egress_tm_is_service_q(tm))
        BDMF_TRACE_RET_OBJ(BDMF_ERR_NOT_SUPPORTED, mo, "Service queue can't have weight\n");

    if (tm->overall_rl)
        return BDMF_ERR_NOT_SUPPORTED;

    if (mo->state != bdmf_state_init && tm->enable)
    {
        tm_drv_priv_t *upper_tm = tm->upper_level_tm ? (tm_drv_priv_t *)bdmf_obj_data(tm->upper_level_tm) : NULL;
        int is_wrr = upper_tm && egress_tm_is_wrr_elem(upper_tm, index);
        int rc;

        if (is_wrr)
        {
            /* weight is only relevant if upper is WRR.
             * Only validate if relevant, perhaps it is re-configuration in progress */
            if (weight < RDPA_MIN_WEIGHT || weight > RDPA_MAX_WEIGHT)
            {
                BDMF_TRACE_RET_OBJ(BDMF_ERR_RANGE, mo, "WRR weight %u is out of range %d..%d\n",
                    (unsigned)weight, (unsigned)RDPA_MIN_WEIGHT, (unsigned)RDPA_MAX_WEIGHT);
            }
            rc = egress_tm_rl_config(mo, &tm->rl_cfg, weight, tm->enable);
            if (rc < 0)
                BDMF_TRACE_RET_OBJ(rc, mo, "Can't configure weight %d in RDD\n", weight);
        }
    }

    tm->weight = weight;
    return 0;
}

/* Flush single queue given RDD indexes */
static int egress_tm_queue_flush(struct bdmf_object *mo, tm_queue_hash_entry_t *qentry)
{
    return rdpa_rdd_tx_queue_flush(mo, qentry, 1);
}

/* Flush sub egress_tm queues */
static int egress_tm_sub_tm_queue_flush(struct bdmf_object *sub, int16_t hashed_channel_id)
{
    tm_drv_priv_t *sub_tm = (tm_drv_priv_t *)bdmf_obj_data(sub);
    tm_queue_hash_entry_t *qentry = NULL;
    int rc = 0;
    int i;
    int queue_id;
                
    if (sub_tm->level == rdpa_tm_level_queue)
    {
        for (i = 0; i < sub_tm->num_queues; i++)
        {
            if (sub_tm->queue_configured[i])
            {
                queue_id = sub_tm->queue_cfg[i].queue_id;
                qentry = egress_tm_hash_get_by_dir_channel_queue(sub_tm->dir, hashed_channel_id, queue_id);
                if (qentry)
                    rc |= egress_tm_queue_flush(sub, qentry);
            }
        }
    }
    else
    {
        for (i = 0; i < RDPA_TM_MAX_SCHED_ELEMENTS; i++)
        {
            if (sub_tm->sub_tms[i])
                rc |= egress_tm_sub_tm_queue_flush(sub_tm->sub_tms[i], hashed_channel_id);
        }
    }

    return rc;
}

/* "rxq_flush" attribute "write" callback */
static int egress_tm_attr_queue_flush_write(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, const void *val, uint32_t size)
{
    rdpa_tm_queue_index_t *qi = (rdpa_tm_queue_index_t *)index;
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    bdmf_boolean do_flush = *(bdmf_boolean *)val;
    struct bdmf_object *root = egress_tm_get_root_object(mo);
    tm_drv_priv_t *root_tm = (tm_drv_priv_t *)bdmf_obj_data(root);
    int16_t hashed_channel_id;
    tm_queue_hash_entry_t *qentry;
    int rc = 0;

    if (!qi)
        return BDMF_ERR_PARM;
    if (egress_tm_is_service_q(tm))
        BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, mo, "queue_flush can't be set on service queue\n");
    if (tm->upper_level_tm)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, mo, "queue_flush must be set on top-level egress_tm\n");
    if (!do_flush)
        return 0;

    /* Flush all queues in queue_id bundle or specific queue */
    if (*(bdmf_index *)qi == BDMF_INDEX_UNASSIGNED)
    {
        int i;

        /* Special handling for channel group - sum-up for all subsidiaries */
        if (mo == root && egress_tm_is_ch_group(root_tm))
        {            
            for (i = 0; i < RDPA_TM_MAX_SCHED_ELEMENTS && !rc; i++)
            {
                if (tm->sub_tms[i])
                {
                    rc = egress_tm_attr_queue_flush_write(tm->sub_tms[i],
                        ad, index, &do_flush, size);
                }
            }
        }
        else
        {
            /* Flush all */
            struct bdmf_object *top = egress_tm_get_top_object(mo);
            tm_channel_t *channels = (tm->dir == rdpa_dir_ds) ? ds_channels : us_channels;
            int num_channels = (tm->dir == rdpa_dir_ds) ? RDPA_MAX_DS_CHANNELS : RDPA_MAX_US_CHANNELS;

            for (i = 0; i < num_channels && !rc; i++)
            {
                tm_channel_t *ch = &channels[i];
                hashed_channel_id = egress_tm_is_group_owner(root_tm) ? root_tm->channel_group : ch->channel_id;   
                if (ch->egress_tm == top)                    
                    egress_tm_sub_tm_queue_flush(top, hashed_channel_id);
            }
        }
    }
    else
    {
        /* Flush specific */
        hashed_channel_id = egress_tm_is_group_owner(root_tm) ? root_tm->channel_group : qi->channel;
        qentry = egress_tm_hash_get_by_dir_channel_queue(tm->dir, hashed_channel_id, qi->queue_id);
        if (qentry)
            rc = egress_tm_queue_flush(mo, qentry);
        else
            rc = BDMF_ERR_NOENT;
    }

    return rc;
}

/* Does channel belong in mo hierarchy ? */
static int egress_tm_is_channel_in_mo_hierarchy(struct bdmf_object *mo, tm_channel_t *ch)
{
    struct bdmf_object *ch_mo = ch->egress_tm;
    return egress_tm_get_root_object(mo) == egress_tm_get_root_object(ch_mo);
}

/* Channel selector string to internal value */
static int egress_tm_channel_val_to_s(struct bdmf_object *mo,
    struct bdmf_attr *ad, const void *val, char *sbuf, uint32_t size)
{
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    int32_t channel_id = *(int32_t *)val;
    tm_channel_t *ch;

    if (egress_tm_is_service_q(tm))
    {
        snprintf(sbuf, size, "{%s}", "service queue");
        return 0;
    }

    if (channel_id < 0)
    {
        strncpy(sbuf, "all", size);
        return 0;
    }
    ch = egress_tm_channel_get(tm->dir, channel_id);
    if (!ch || !ch->owner)
        return BDMF_ERR_NOENT;
    if (!egress_tm_is_channel_in_mo_hierarchy(mo, ch))
        return BDMF_ERR_NOENT;
    snprintf(sbuf, size, "{%s}", ch->owner->name);

    return 0;
}

/* Channel selector internal value to string */
static int egress_tm_channel_s_to_val(struct bdmf_object *mo_tmp,
    struct bdmf_attr *ad, const char *sbuf, void *val, uint32_t size)
{
    struct bdmf_object *mo = mo_tmp->drv_priv;
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    int32_t *channel_id = (int32_t *)val;
    tm_channel_t *channels = (tm->dir == rdpa_dir_ds) ? ds_channels : us_channels;
    int num_channels = (tm->dir == rdpa_dir_ds) ? RDPA_MAX_DS_CHANNELS : RDPA_MAX_US_CHANNELS;
    int i;

    if (!sbuf)
        return BDMF_ERR_PARM;

    if (egress_tm_is_service_q(tm) && !strcmp(sbuf, "{service queue}"))
    {
        *channel_id = RDPA_SERVICE_Q_CHANNEL;
        return 0;
    }

    if (!strcmp(sbuf, "all"))
    {
        *channel_id = -1;
        return 0;
    }

    for (i = 0; i < num_channels; i++)
    {
        tm_channel_t *ch = &channels[i];
        if (ch->owner && !strcmp(ch->owner->name, sbuf) && egress_tm_is_channel_in_mo_hierarchy(mo, ch))
            break;
    }
    if (i >= num_channels)
        return BDMF_ERR_NOENT;
    *channel_id = i;

    return 0;
}

/*
 * Helpers for egress_tm_is_qtm_ctl_under_mo - get next channel+queue for queue_stat[]
 */

/* Does RC belong in mo hierarchy ? */
static int egress_tm_is_qtm_ctl_under_mo(struct bdmf_object *mo, tm_qtm_ctl_t *qtm_ctl)
{
    struct bdmf_object *rc_mo = qtm_ctl->egress_tm;

    while (rc_mo && rc_mo->drv == rdpa_egress_tm_drv())
    {
        if (rc_mo == mo)
            return 1;
        rc_mo = rc_mo->owner;
    }
    return 0;
}

/* get qtm_ctl, index by queue_id */
static tm_qtm_ctl_t *egress_tm_qtm_ctl_get_by_queue(struct bdmf_object *mo,
    tm_channel_t *channel, uint32_t queue_id, int *prty)
{
    tm_qtm_ctl_t *qtm_ctl, *qtm_ctl_tmp;

    STAILQ_FOREACH_SAFE(qtm_ctl, &channel->qtm_ctls, list, qtm_ctl_tmp)
    {
        tm_drv_priv_t *qtm_ctl_tm;
        int i;

        /* Skip rate controllers not under mo */
        if (!egress_tm_is_qtm_ctl_under_mo(mo, qtm_ctl))
            continue;

        qtm_ctl_tm = (tm_drv_priv_t *)bdmf_obj_data(qtm_ctl->egress_tm);
        for (i = 0; i < qtm_ctl_tm->num_queues; i++)
        {
            if (qtm_ctl->queue_configured[i] && qtm_ctl->hash_entry[i].queue_id == queue_id)
            {
                *prty = i;
                return qtm_ctl;
            }
        }
    }
    return NULL;
}

/* get 1st/next configured queue on rate controller */
static uint32_t egress_tm_qtm_ctl_get_next_queue(struct bdmf_object *mo,
    tm_channel_t *channel, tm_qtm_ctl_t *qtm_ctl, uint32_t prev_prty)
{
    uint32_t i;

    while (qtm_ctl)
    {
        /* Skip rate controllers not under mo */
        if (egress_tm_is_qtm_ctl_under_mo(mo, qtm_ctl))
        {
            tm_drv_priv_t *qtm_ctl_tm = (tm_drv_priv_t *)bdmf_obj_data(qtm_ctl->egress_tm);
            /* Get next */
            BUG_ON(qtm_ctl_tm->num_queues > RDPA_MAX_EGRESS_QUEUES);
            for (i = prev_prty + 1; i < qtm_ctl_tm->num_queues; i++)
            {
                if (qtm_ctl->queue_configured[i])
                    return  qtm_ctl->hash_entry[i].queue_id;
            }
            prev_prty = -1;
        }
        qtm_ctl = STAILQ_NEXT(qtm_ctl, list);
    }
    return BDMF_INDEX_UNASSIGNED;
}

/* get 1st/next configured queue on channel */
static uint32_t egress_tm_channel_get_next_queue(struct bdmf_object *mo, tm_channel_t *channel, uint32_t prev_queue_id)
{
    tm_qtm_ctl_t *qtm_ctl;
    uint32_t queue_id;
    int prty = -1;

    /* Now find 1st/next queue */
    if (prev_queue_id == BDMF_INDEX_UNASSIGNED)
    {
        qtm_ctl = STAILQ_FIRST(&channel->qtm_ctls);
        prty = -1;
    }
    else
    {
        qtm_ctl = egress_tm_qtm_ctl_get_by_queue(mo, channel, prev_queue_id, &prty);
    }
    if (!qtm_ctl)
        return BDMF_INDEX_UNASSIGNED;
    queue_id = egress_tm_qtm_ctl_get_next_queue(mo, channel, qtm_ctl, prty);

    return queue_id;
}


/* get next queue+channel callback */
static int egress_tm_queue_channel_get_next(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index *index)
{
    rdpa_tm_queue_index_t *qi = (rdpa_tm_queue_index_t *)index;
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);
    tm_channel_t *ch;

    if (!qi)
        return BDMF_ERR_PARM;

    /* Get 1st ? */
    if (*index == BDMF_INDEX_UNASSIGNED)
    {
        ch = egress_tm_channel_get_first(mo);
        qi->queue_id = BDMF_INDEX_UNASSIGNED;
    }
    else
        ch = egress_tm_channel_get(tm->dir, qi->channel);
    if (!ch)
        return BDMF_ERR_NO_MORE;

    do
    {
        /* Find 1st/next queue on this channel */
        qi->channel = ch->channel_id;
        qi->queue_id = egress_tm_channel_get_next_queue(mo, ch, qi->queue_id);
        if (qi->queue_id != BDMF_INDEX_UNASSIGNED)
            break;
        /* Done with this channel. Find the next one */
        ch = egress_tm_channel_get_next(mo, ch);
    } while (ch);

    return ch ? 0 : BDMF_ERR_NO_MORE;
}


/* get next queue+channel callback */
static int egress_all_queues_get_next(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index *index)
{
    rdpa_tm_queue_index_t *qi = (rdpa_tm_queue_index_t *)index;
    tm_drv_priv_t *tm = (tm_drv_priv_t *)bdmf_obj_data(mo);

    if (egress_tm_is_service_q(tm))
    {
        if (tm->level == rdpa_tm_level_egress_tm)
            return BDMF_ERR_NO_MORE;

        if (*index == BDMF_INDEX_UNASSIGNED)
        {
            qi->channel = RDPA_SERVICE_Q_CHANNEL;
            qi->queue_id = tm->queue_cfg[0].queue_id;
            return 0;
        }
        return BDMF_ERR_NO_MORE;
    }

    return egress_tm_queue_channel_get_next(mo, ad, index);
}

/* Scheduler level enum values */
bdmf_attr_enum_table_t egress_tm_level_enum_table = {
    .type_name = "rdpa_tm_level_type", .help = "Egress tm level",
    .values = {
        {"queue", rdpa_tm_level_queue},
        {"egress_tm", rdpa_tm_level_egress_tm},
        {NULL, 0}
    }
};

/* Scheduling type enum values */
bdmf_attr_enum_table_t tm_sched_mode_enum_table = {
    .type_name = "rdpa_tm_sched_mode", .help = "Scheduling mode",
    .values = {
        {"disable",     rdpa_tm_sched_disabled},
        {"sp",          rdpa_tm_sched_sp},
        {"wrr",         rdpa_tm_sched_wrr},
#ifdef XRDP
        {"sp_wrr",      rdpa_tm_sched_sp_wrr},
#endif
        {NULL, 0}
    }
};

/* Drop policy enum values */
bdmf_attr_enum_table_t tm_drop_policy_enum_table = {
    .type_name = "rdpa_tm_drop_alg", .help = "Drop policy algorithm type",
    .values = {
        {"dt", rdpa_tm_drop_alg_dt},
        {"red", rdpa_tm_drop_alg_red},
        {"wred", rdpa_tm_drop_alg_wred},
        {"flow", rdpa_tm_drop_alg_reserved},
        {NULL, 0}
    }
};

/* Overall rate limiter priority enum values */
bdmf_attr_enum_table_t orl_prty_enum_table = {
    .type_name = "rdpa_tm_orl_prty", .help = "Priority for overall rate limiter",
    .values = {
        {"low", rdpa_tm_orl_prty_low},
        {"high", rdpa_tm_orl_prty_high},
        {NULL, 0}
    }
};

/* Rate limiter mode enum values */
bdmf_attr_enum_table_t tm_rl_mode_enum_table = {
    .type_name = "rdpa_tm_rl_rate_mode", .help = "Rate limiter mode",
    .values = {
        {"single_rate", rdpa_tm_rl_single_rate},
        {"dual_rate", rdpa_tm_rl_dual_rate},
        {NULL, 0}
    }
};

/* Number of SP elements enum values */
bdmf_attr_enum_table_t tm_num_sp_elem_enum_table = {
    .type_name = "rdpa_tm_num_sp_elem", .help = "Number of SP elements for sp_wrr scheduling mode",
    .values = {
        {"0",   rdpa_tm_num_sp_elem_0},
        {"2",   rdpa_tm_num_sp_elem_2},
        {"4",   rdpa_tm_num_sp_elem_4},
        {"8",   rdpa_tm_num_sp_elem_8},
        {"16",  rdpa_tm_num_sp_elem_16},
        {"32",  rdpa_tm_num_sp_elem_32},
        {NULL,  0}
    }
};

/*  tm_rl_cfg_type aggregate type : rate limiter configuration */
struct bdmf_aggr_type tm_rl_cfg_type = {
    .name = "tm_rl_cfg", .struct_name = "rdpa_tm_rl_cfg_t",
    .help = "Rate Limiter Configuration",
    .fields = (struct bdmf_attr[]) {
        { .name = "af", .help = "AF Rate (bps)", .type = bdmf_attr_number,
            .flags = BDMF_ATTR_UNSIGNED, .size = sizeof(rl_rate_size_t),
            .offset = offsetof(rdpa_tm_rl_cfg_t, af_rate)
        },
        { .name = "be", .help = "BE Rate (bps)", .type = bdmf_attr_number,
            .flags = BDMF_ATTR_UNSIGNED, .size = sizeof(rl_rate_size_t),
            .offset = offsetof(rdpa_tm_rl_cfg_t, be_rate)
        },
        { .name = "burst", .help = "Burst Size (bytes)", .type = bdmf_attr_number,
            .flags = BDMF_ATTR_UNSIGNED, .size = sizeof(rl_rate_size_t),
            .offset = offsetof(rdpa_tm_rl_cfg_t, burst_size)
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE(tm_rl_cfg_type);

/*  tm_prio_class_cfg_type aggregate type : wred priority class thresholds */
struct bdmf_aggr_type tm_prio_class_cfg_type = {
    .name = "tm_prio_class_cfg", .struct_name = "rdpa_tm_priority_class_t",
    .help = "priority class Configuration",
    .fields = (struct bdmf_attr[]) {
        { .name = "min_thresh", .help = "min threshold", .type = bdmf_attr_number,
            .flags = BDMF_ATTR_UNSIGNED, .size = sizeof(uint32_t),
            .offset = offsetof(rdpa_tm_priority_class_t, min_threshold)
        },
        { .name = "max_thresh", .help = "max threshold", .type = bdmf_attr_number,
            .flags = BDMF_ATTR_UNSIGNED, .size = sizeof(uint32_t),
            .offset = offsetof(rdpa_tm_priority_class_t, max_threshold)
        },
        { .name = "max_drop_probability", .help = "max drop probability", .type = bdmf_attr_number,
            .flags = BDMF_ATTR_UNSIGNED, .size = sizeof(uint32_t),
            .offset = offsetof(rdpa_tm_priority_class_t, max_drop_probability),
            .max_val = RDPA_WRED_MAX_DROP_PROBABILITY
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE(tm_prio_class_cfg_type);


/* tm_queue_cfg aggregate type : queue configuration */
struct bdmf_aggr_type tm_queue_cfg_type = {
    .name = "tm_queue_cfg", .struct_name = "rdpa_tm_queue_cfg_t",
    .help = "Egress Queue Configuration",
    .fields = (struct bdmf_attr[]) {
        { .name = "queue_id", .help = "Queue id", .type = bdmf_attr_number,
            .size = sizeof(uint32_t),
            .offset = offsetof(rdpa_tm_queue_cfg_t, queue_id)
        },
        { .name = "drop_threshold", .help = "Drop Threshold (queue size)",
            .type = bdmf_attr_number, .flags = BDMF_ATTR_UNSIGNED,
            .size = sizeof(uint32_t),
            .offset = offsetof(rdpa_tm_queue_cfg_t, drop_threshold)
        },
        { .name = "weight", .help = "Weight for WRR scheduling",
            .type = bdmf_attr_number, .size = sizeof(uint32_t),
            .offset = offsetof(rdpa_tm_queue_cfg_t, weight),
            .min_val = RDPA_WEIGHT_UNASSIGNED, .max_val = RDPA_MAX_WEIGHT,
        },
        { .name = "drop_alg", .help = "Drop policy",
            .type = bdmf_attr_enum, .ts.enum_table = &tm_drop_policy_enum_table,
            .size = sizeof(rdpa_tm_drop_alg),
            .offset = offsetof(rdpa_tm_queue_cfg_t, drop_alg)
        },
        { .name = "high_class", .help = "WRED high Thresholds",
            .type = bdmf_attr_aggregate, .ts.aggr_type_name = "tm_prio_class_cfg", 
            .flags = BDMF_ATTR_UNSIGNED, 
            .offset = offsetof(rdpa_tm_queue_cfg_t, high_class)
        },
        { .name = "low_class", .help = "WRED low Thresholds",
            .type = bdmf_attr_aggregate, .ts.aggr_type_name = "tm_prio_class_cfg", 
            .flags = BDMF_ATTR_UNSIGNED, 
            .offset = offsetof(rdpa_tm_queue_cfg_t, low_class)
        },
        { .name = "rl", .help = "Rate Configuration",
            .type = bdmf_attr_aggregate, .ts.aggr_type_name = "tm_rl_cfg",
            .offset = offsetof(rdpa_tm_queue_cfg_t, rl_cfg)
        },
        { .name = "priority_mask_0", .help = "Priority Mask cover TC value 0 to 31",
            .type = bdmf_attr_number, .size = sizeof(uint32_t),
            .offset = offsetof(rdpa_tm_queue_cfg_t, priority_mask_0),
        },
        { .name = "priority_mask_1", .help = "Priority Mask cover TC value 32 to 63",
            .type = bdmf_attr_number, .size = sizeof(uint32_t),
            .offset = offsetof(rdpa_tm_queue_cfg_t, priority_mask_1),
        },
        { .name = "stat_enable", .help = "Enable queue statistics",
            .type = bdmf_attr_boolean,
            .size = sizeof(bdmf_boolean),
            .offset = offsetof(rdpa_tm_queue_cfg_t, stat_enable)
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE(tm_queue_cfg_type);

/* tm_queue_index aggregate type : queue index - for flush and queue_stat attributes */
struct bdmf_aggr_type tm_queue_index_type = {
    .name = "tm_queue_index", .struct_name = "rdpa_tm_queue_index_t",
    .help = "Egress Queue Index",
    .fields = (struct bdmf_attr[]) {
        { .name = "channel", .help = "Channel selector. Object name or \"all\"",
            .type = bdmf_attr_number, .size = sizeof(bdmf_index),
            .offset = offsetof(rdpa_tm_queue_index_t, channel),
            .val_to_s = egress_tm_channel_val_to_s,
            .s_to_val = egress_tm_channel_s_to_val
        },
        { .name = "queue_id", .help = "Queue id", .type = bdmf_attr_number,
            .size = sizeof(uint32_t),
            .offset = offsetof(rdpa_tm_queue_index_t, queue_id)
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE(tm_queue_index_type);

/* service_queue_stat aggregate type */
struct bdmf_aggr_type service_queue_cfg_type = {
    .name = "service_queue_cfg", .struct_name = "rdpa_tm_service_queue_t",
    .help = "service Queue statistic",
    .fields = (struct bdmf_attr[]) {
        { .name = "enable", .help = "Enable service queue",
            .type = bdmf_attr_boolean, .size = sizeof(bdmf_boolean),
            .offset = offsetof(rdpa_tm_service_queue_t, enable),
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE(service_queue_cfg_type);

/* Object attribute descriptors */
static struct bdmf_attr egress_tm_attrs[] = {
    { .name = "dir", .help = "Traffic Direction",
        .type = bdmf_attr_enum, .ts.enum_table = &rdpa_traffic_dir_enum_table,
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE_INIT | BDMF_ATTR_CONFIG |
            BDMF_ATTR_KEY | BDMF_ATTR_MANDATORY,
        .size = sizeof(rdpa_traffic_dir), .offset = offsetof(tm_drv_priv_t, dir)
    },
    { .name = "index", .help = "Egress-TM Index", .type = bdmf_attr_number,
        .flags = BDMF_ATTR_READ | BDMF_ATTR_CONFIG | BDMF_ATTR_WRITE_INIT | BDMF_ATTR_KEY,
        .size = sizeof(bdmf_index), .offset = offsetof(tm_drv_priv_t, index)
    },
    { .name = "level", .help = "Egress-TM Next Level",
        .type = bdmf_attr_enum, .ts.enum_table = &egress_tm_level_enum_table,
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE_INIT | BDMF_ATTR_CONFIG,
        .size = sizeof(rdpa_tm_level_type), .offset = offsetof(tm_drv_priv_t, level)
    },
    { .name = "mode", .help = "Scheduler Operating Mode",
        .type = bdmf_attr_enum, .ts.enum_table = &tm_sched_mode_enum_table,
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE_INIT | BDMF_ATTR_CONFIG,
        .size = sizeof(rdpa_tm_sched_mode), .offset = offsetof(tm_drv_priv_t, mode)
    },
    { .name = "overall_rl", .help = "Overall Rate Limiter",
        .type = bdmf_attr_boolean,
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE_INIT | BDMF_ATTR_CONFIG,
        .size = sizeof(bdmf_boolean), .offset = offsetof(tm_drv_priv_t, overall_rl)
    },
    { .name = "service_queue", .help = "Service Queue Parameters Configuration",
        .type = bdmf_attr_aggregate, .ts.aggr_type_name = "service_queue_cfg",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE_INIT | BDMF_ATTR_CONFIG,
        .offset = offsetof(tm_drv_priv_t, service_q)
    },
    { .name = "enable", .help = "Enable object",
        .type = bdmf_attr_boolean,
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG,
        .size = sizeof(bdmf_boolean), .offset = offsetof(tm_drv_priv_t, enable),
        .write = egress_tm_attr_enable_write
    },
    { .name = "rl", .help = "Rate Configuration",
        .type = bdmf_attr_aggregate, .ts.aggr_type_name = "tm_rl_cfg",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG,
        .offset = offsetof(tm_drv_priv_t, rl_cfg), .write = egress_tm_attr_rl_write
    },
    { .name = "rl_rate_mode", .help = "Subsidiary Rate Limiter Rate Mode",
        .type = bdmf_attr_enum, .ts.enum_table = &tm_rl_mode_enum_table,
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE_INIT | BDMF_ATTR_CONFIG,
        .size = sizeof(rdpa_tm_rl_rate_mode), .offset = offsetof(tm_drv_priv_t, rl_rate_mode),
        .read = egress_tm_attr_rl_rate_mode_read, .write = egress_tm_attr_rl_rate_mode_write
    },
    { .name = "num_queues", .help = "Number of Queues",
        .type = bdmf_attr_number, .size = sizeof(uint8_t), .data_type_name = "uint8_t",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE_INIT | BDMF_ATTR_CONFIG | BDMF_ATTR_UNSIGNED,
        .min_val = 1, .max_val = RDPA_MAX_EGRESS_QUEUES,
        .read = egress_tm_attr_num_queues_read, .write = egress_tm_attr_num_queues_write,
    },
    { .name = "num_sp_elements", .help = "Number of SP Scheduling Elements for SP_WRR Scheduling Mode",
        .type = bdmf_attr_enum, .ts.enum_table = &tm_num_sp_elem_enum_table,
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG,
        .size = sizeof(rdpa_tm_num_sp_elem),
        .read = egress_tm_attr_num_sp_elements_read, .write = egress_tm_attr_num_sp_elements_write,
    },
    { .name = "queue_cfg", .help = "Queue Parameters Configuration",
        .type = bdmf_attr_aggregate, .ts.aggr_type_name = "tm_queue_cfg",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG,
        .array_size = RDPA_MAX_EGRESS_QUEUES,
        .read = egress_tm_attr_queue_cfg_read, .write = egress_tm_attr_queue_cfg_write,
        .del = egress_tm_attr_queue_cfg_delete, .val_to_s = egress_tm_queue_cfg_val_to_s,
    },
    { .name = "queue_flush", .help = "Flush Egress Queue", .type = bdmf_attr_boolean,
        .array_size = RDPA_MAX_EGRESS_QUEUES, .flags = BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG,
        .index_type = bdmf_attr_aggregate, .index_ts.aggr_type_name = "tm_queue_index",
        .write = egress_tm_attr_queue_flush_write
    },
    { .name = "queue_statistics", .help = "Dropped Service Queue Statistics",
        .type = bdmf_attr_aggregate, .ts.aggr_type_name = "rdpa_stat_1way",
        .index_type = bdmf_attr_aggregate, .index_ts.aggr_type_name = "tm_queue_index",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_STAT | BDMF_ATTR_DEPRECATED,
        .array_size = RDPA_MAX_EGRESS_QUEUES, .get_next = egress_tm_attr_deprecated_get_next,
        .read = egress_tm_attr_deprecated_read,
    },
    { .name = "queue_stat", .help = "Retrieve Egress Queue Statistics",
        .type = bdmf_attr_aggregate, .ts.aggr_type_name = "rdpa_stat_1way",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_STAT | BDMF_ATTR_UNSIGNED ,
        .array_size = RDPA_MAX_EGRESS_QUEUES,
        .index_type = bdmf_attr_aggregate, .index_ts.aggr_type_name = "tm_queue_index",
        .get_next = egress_all_queues_get_next, 
        .read = egress_tm_attr_all_queues_stat_read, .write = egress_tm_attr_queue_stat_write,
    },
    { .name = "queue_occupancy", .help = "Retrieve Egress Queue Occupancy",
        .type = bdmf_attr_number, .size = sizeof(uint32_t), .data_type_name = "uint32_t",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_UNSIGNED,
        .array_size = RDPA_MAX_EGRESS_QUEUES,
        .index_type = bdmf_attr_aggregate, .index_ts.aggr_type_name = "tm_queue_index",
        .get_next = egress_all_queues_get_next, 
        .read = egress_tm_attr_queue_occupancy_read,
    },
    { .name = "subsidiary", .help = "Next Level Egress-TM",
        .type = bdmf_attr_object, .size = sizeof(bdmf_object_handle),
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG,
        .ts.ref_type_name = "egress_tm", .array_size = RDPA_TM_MAX_SCHED_ELEMENTS,
        .read = egress_tm_attr_sub_tm_read, .write = egress_tm_attr_sub_tm_write
    },
    { .name = "weight", .help = "Weight for WRR scheduling (0 for unset)", .type = bdmf_attr_number,
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG | BDMF_ATTR_UNSIGNED,
        .size = sizeof(uint32_t), .offset = offsetof(tm_drv_priv_t, weight),
        .min_val = RDPA_WEIGHT_UNASSIGNED, .max_val = RDPA_MAX_WEIGHT,
        .write = egress_tm_attr_weight_write
    },
    BDMF_ATTR_LAST
};

static int egress_tm_drv_init(struct bdmf_type *drv);
static void egress_tm_drv_exit(struct bdmf_type *drv);

struct bdmf_type egress_tm_drv = {
    .name = "egress_tm",
    .parent = "system",
    .description = "Hierarchical Traffic Scheduler",
    .drv_init = egress_tm_drv_init,
    .drv_exit = egress_tm_drv_exit,
    .pre_init = egress_tm_pre_init,
    .post_init = egress_tm_post_init,
    .link_up = egress_tm_link,
    .link_down = egress_tm_link,
    .unlink_up = egress_tm_unlink,
    .unlink_down = egress_tm_unlink,
    .destroy = egress_tm_destroy,
    .extra_size = sizeof(tm_drv_priv_t),
    .aattr = egress_tm_attrs,
    .max_objs = RDPA_TM_MAX_US_SCHED+RDPA_TM_MAX_DS_SCHED,
    .flags = BDMF_DRV_FLAG_MUXUP | BDMF_DRV_FLAG_MUXDOWN
};
DECLARE_BDMF_TYPE(rdpa_egress_tm, egress_tm_drv);

extern int (*f_rdpa_egress_tm_queue_exists)(rdpa_if port, uint32_t queue_id);

/* Init/exit module. Cater for GPL layer */
int rdpa_egress_tm_check_queue(rdpa_if port, uint32_t queue_id)
{
#if defined(DSL_63138) || defined(DSL_63148) || defined(WL4908)
    if (port == rdpa_wan_type_to_if(rdpa_wan_gbe)) /* Ethernet WAN */
    {
        int channel = 0, rc_id, priority, tc;
        return _rdpa_egress_tm_wan_flow_queue_to_rdd_tc_check(GBE_WAN_FLOW_ID, queue_id, &channel, &rc_id, &priority, &tc);
    }
    else if (port >= rdpa_if_lan0 && port <= rdpa_if_lan_max) /* Ethernet LAN */
    {
        int rc_id, priority, tc;
        return _rdpa_egress_tm_lan_port_queue_to_rdd_tc_check(port, queue_id, &rc_id, &priority, &tc);
    }
    /* No support for DSL WAN */
#endif
    return BDMF_ERR_NOENT;
}

/* Init/exit module. Cater for GPL layer */
static int egress_tm_drv_init(struct bdmf_type *drv)
{
    int i;
#ifdef BDMF_DRIVER_GPL_LAYER
    f_rdpa_egress_tm_drv = rdpa_egress_tm_drv;
    f_rdpa_egress_tm_get = rdpa_egress_tm_get;
    f_rdpa_egress_tm_queue_exists = rdpa_egress_tm_check_queue;
#endif
    for (i = 0; i < TM_QUEUE_HASH_SIZE; i++)
        SLIST_INIT(&tm_queue_hash[i]);
    for (i = 0; i < RDPA_MAX_US_CHANNELS; i++)
    {
        STAILQ_INIT(&us_channels[i].qtm_ctls);
        us_channels[i].channel_id = i;
        us_channels[i].dir = rdpa_dir_us;
    }
    for (i = 0; i < RDPA_MAX_DS_CHANNELS; i++)
    {
        STAILQ_INIT(&ds_channels[i].qtm_ctls);
        ds_channels[i].channel_id = i;
        ds_channels[i].dir = rdpa_dir_ds;
    }
    for (i = 0; i < RDPA_MAX_QUEUE_COUNTERS; i++)
    {
        queue_counters[rdpa_dir_ds][i] = i + RDPA_MAX_QUEUE_COUNTERS;
        queue_counters[rdpa_dir_us][i] = i + RDPA_MAX_QUEUE_COUNTERS;
    }

    memset(us_hash_lookup, 0x0, sizeof(us_hash_lookup));
    memset(ds_hash_lookup, 0x0, sizeof(ds_hash_lookup));

    return 0;
}

static void egress_tm_drv_exit(struct bdmf_type *drv)
{
#ifdef BDMF_DRIVER_GPL_LAYER
    f_rdpa_egress_tm_drv = NULL;
    f_rdpa_egress_tm_get = NULL;
    f_rdpa_egress_tm_queue_exists = NULL;
#endif
}

/***************************************************************************
 * Functions declared in auto-generated header
 **************************************************************************/

/** Get egress_tm object by key */
int rdpa_egress_tm_get(const rdpa_egress_tm_key_t *_key_, bdmf_object_handle *_obj_)
{
    struct bdmf_object **tm_objects = (_key_->dir == rdpa_dir_ds) ? ds_tm_objects : us_tm_objects;
    int max_subs = (_key_->dir == rdpa_dir_ds) ? RDPA_TM_MAX_DS_SCHED : RDPA_TM_MAX_US_SCHED;
    return rdpa_obj_get(tm_objects, max_subs, _key_->index, _obj_);
}

/*************************************************************************
 * Functions in use by other drivers internally
 ************************************************************************/


/* Bind/unbind channel.
 * bind: owner!=NULL, unbind : owner==NULL
 */
int _rdpa_egress_tm_channel_set(bdmf_object_handle tm_obj, bdmf_object_handle owner, int16_t channel_id)
{
    tm_drv_priv_t *tm;
    tm_channel_t *channel;
    bdmf_error_t rc = 0;
    bdmf_boolean enable;
    bdmf_object_handle old_tm_obj;

    if (!tm_obj)
        return BDMF_ERR_PARM;
    tm = (tm_drv_priv_t *)bdmf_obj_data(tm_obj);

    /* This operation is only allowed for top-level TM */
#ifdef EPON
    if (tm->upper_level_tm && ((tm->dir != rdpa_dir_us) || (_rdpa_epon_mode_get() != rdpa_epon_ctc)))
#else
    if (tm->upper_level_tm)
#endif
        BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, tm_obj, "Can't set channel on subsidiary scheduler\n");

    enable = tm->enable;

    /* Find channel context. If it is already bound to another egress_tm - it is an error */
    channel = egress_tm_channel_get(tm->dir, channel_id);
    if (!channel)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_INTERNAL, tm_obj, "channel_id %d is invalid\n", channel_id);

    old_tm_obj = channel->egress_tm;
    /* Unbind the old egress_tm object from channel if necessary */
    if (old_tm_obj && old_tm_obj != tm_obj)
    {
        /* Unbind old egress_tm if bind new.
         * Report error if un-binding wrong egress_tm from the channel
         */
        if (owner)
        {
            _rdpa_egress_tm_channel_set(old_tm_obj, NULL, channel_id);
        }
        else
        {
            BDMF_TRACE_RET_OBJ(BDMF_ERR_INTERNAL, tm_obj, "channel_id %d is bound to %s\n",
                channel_id, channel->egress_tm->name);
        }
    }

    /* Unbind channel ? */
    if (!owner)
    {
        /* Do nothing if not bound */
        if (!channel->egress_tm)
            return 0;
        if (channel->orl_tm)
            egress_tm_orl_set_on_channel(tm_obj, channel, NULL, tm->orl_prty);
        if (enable)
            egress_tm_enable_set_on_channel(tm_obj, channel, 0);
        egress_tm_rdd_resources_free_on_channel(tm_obj, channel);
        channel->egress_tm = NULL;
        channel->owner = NULL;
    }
    else
    {
        /* Assign new channel */
        channel->egress_tm = tm_obj;
        channel->owner = owner;
        rc = egress_tm_rdd_resources_alloc_on_channel(tm_obj, channel, -1);
        if (enable)
            rc = rc ? rc : egress_tm_enable_set_on_channel(tm_obj, channel, enable);
        if (tm->dir == rdpa_dir_us && us_overall_rl_obj && egress_tm_is_channel_rl(us_overall_rl_obj, owner))
            rc = rc ? rc : egress_tm_orl_set_on_channel(tm_obj, channel, us_overall_rl_obj, tm->orl_prty);

        /* Try to roll-back */
        if (rc)
        {
            if (enable)
                egress_tm_enable_set_on_channel(tm_obj, channel, 0);
            egress_tm_rdd_resources_free_on_channel(tm_obj, channel);
            channel->egress_tm = NULL;
            channel->owner = NULL;
            if (old_tm_obj)
            {
                int rc1;
                rc1 = _rdpa_egress_tm_channel_set(old_tm_obj, owner, channel_id);
                rc1 = rc1 ? rc1 : egress_tm_enable_set_on_channel(old_tm_obj, channel, enable);
                if (rc1)
                {
                    BDMF_TRACE_RET_OBJ(rc1, tm_obj, "Roll-back binding of %s to %s failed\n",
                        owner->name, old_tm_obj->name);
                }
            }
        }
    }

    /* Unset enable if there are no channels on egress_tm */
    if (!egress_tm_channel_get_first(tm_obj))
        egress_tm_enable_store(tm, 0);

    return rc;
}

/* Bind/unbind channel group
 * bind: owner!=NULL, unbind : owner==NULL
 */
int _rdpa_egress_tm_channel_group_set(bdmf_object_handle tm_obj, bdmf_object_handle owner, int group_id,
    int num_channels, const int16_t *channels)
{
    tm_drv_priv_t *tm;
    int i;
    int rc = 0;

    if (!tm_obj)
        return BDMF_ERR_PARM;
    tm = (tm_drv_priv_t *)bdmf_obj_data(tm_obj);

    if (tm->upper_level_tm)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, tm_obj, "Can't set channel group for subsidiary scheduler\n");

    if (!num_channels || (unsigned)num_channels > RDPA_MAX_WAN_SUBCHANNELS)
    {
        BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, tm_obj, "num_channels %d is out of range 1..%d\n",
            num_channels, RDPA_MAX_WAN_SUBCHANNELS);
    }

    if (num_channels > 1 && tm->level != rdpa_tm_level_egress_tm)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, tm_obj, "Can't assign multiple channels to queue-level egress_tm\n");

    /* Make sure that there are no subsidiaries with index >= num_channels */
    for (i = num_channels; i < RDPA_TM_MAX_SCHED_ELEMENTS; i++)
    {
        if (tm->sub_tms[i])
        {
            BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, tm_obj,
                "Subsidiary index %d of %s is outside sub-channel range 0..%d\n\n",
                i, tm->sub_tms[i]->name, num_channels-1);
        }
    }

    if (owner)
    {
        if (tm->channel_group_owner && tm->channel_group_owner != owner)
        {
            BDMF_TRACE_RET_OBJ(BDMF_ERR_ALREADY, tm_obj, "egress_tm is already owned by channel group %s\n",
                tm->channel_group_owner->name);
        }

        /* Go over channels that were already assigned and release those
         * that are not assigned anymore
         */
        BUG_ON(tm->num_channels > RDPA_MAX_WAN_SUBCHANNELS); 
        for (i = num_channels; i < tm->num_channels; i++)
        {
            if (tm->level == rdpa_tm_level_egress_tm)
            {
                if (tm->sub_tms[i])
                    _rdpa_egress_tm_channel_set(tm->sub_tms[i], NULL, channels[i]);
            }
            else
            {
                _rdpa_egress_tm_channel_set(tm_obj, NULL, channels[i]);
            }
            tm->channels[i] = BDMF_INDEX_UNASSIGNED;
        }

        tm->num_channels = num_channels;
        tm->channel_group = group_id;
        tm->channel_group_owner = owner;
    }

    /* Go over all subsidiaries and clear upper_level_tm in order for the rest
     * of egress_tm logic to consider each subsidiary as root of hierarchy
     */
    for (i = 0; i < num_channels && !rc; i++)
    {
        if (tm->channels[i] != channels[i])
        {
            tm->channels[i] = channels[i];
            if (tm->level == rdpa_tm_level_egress_tm)
            {
                if (tm->sub_tms[i])
                {
                    tm_drv_priv_t *ss = (tm_drv_priv_t *)bdmf_obj_data(tm->sub_tms[i]);
                    ss->upper_level_tm = NULL;
                    rc = _rdpa_egress_tm_channel_set(tm->sub_tms[i], owner, channels[i]);
                }
            }
            else
            {
                rc = _rdpa_egress_tm_channel_set(tm_obj, owner, channels[i]);
            }
        }
    }

    /* roll-back if error */
    if (owner && rc)
    {
        while (--i >= 0)
        {
            if (tm->sub_tms[i])
                _rdpa_egress_tm_channel_set(tm->sub_tms[i], NULL, channels[i]);
        }
    }

    /* Clear stored channels if "clear" command or error */
    if (!owner || rc)
    {
        tm_channel_t *ch = NULL;

        /* Special handling for channel-group egress_tm (LLID). */
        while ((ch = egress_tm_channel_get_next(tm_obj, ch)) && !rc)
        {
            egress_tm_enable_set(ch->egress_tm, 0);
        }

        for (i = 0; i < num_channels && !rc; i++)
        {
            if (tm->sub_tms[i])
            {
                tm_drv_priv_t *ss = (tm_drv_priv_t *)bdmf_obj_data(tm->sub_tms[i]);
                ss->upper_level_tm = tm_obj;
            }
            tm->channels[i] = BDMF_INDEX_UNASSIGNED;
            tm->channel_group = BDMF_INDEX_UNASSIGNED;
        }
        tm->num_channels = 0;
        tm->channel_group_owner = NULL;
    }

    return rc;
}


int _rdpa_egress_tm_enable_set(bdmf_object_handle tm_obj, bdmf_boolean enable)
{
    tm_drv_priv_t *tm;
    int rc = 0;

    if (!tm_obj)
        return BDMF_ERR_PARM;
    tm = (tm_drv_priv_t *)bdmf_obj_data(tm_obj);

    /* This operation is only allowed for top-level TM */
    if (tm->upper_level_tm)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, tm_obj, "Can't enable/disable subsidiary scheduler\n");

    if (tm_obj->state != bdmf_state_init)
        rc = egress_tm_enable_set(tm_obj, enable);
    else
        tm->enable = enable;
    return rc;
}

/* enable/disable sub-channel in channel group */
int _rdpa_egress_tm_enable_subchannel_set(bdmf_object_handle tm_obj, int subch, bdmf_boolean enable)
{
    tm_drv_priv_t *tm;

    if (!tm_obj)
        return BDMF_ERR_PARM;
    tm = (tm_drv_priv_t *)bdmf_obj_data(tm_obj);

    /* This operation is only allowed for top-level TM */
    if (!egress_tm_is_group_owner(tm))
        BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, tm_obj, "Not a channel group (LLID)\n");

    if ((unsigned)subch >= tm->num_channels)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_RANGE, tm_obj, "subchannel %d is invalid\n", subch);

    if (!tm->sub_tms[subch])
        return BDMF_ERR_NOENT;

    return _rdpa_egress_tm_enable_set(tm->sub_tms[subch], enable);
}

int _rdpa_egress_tm_orl_prty_set(bdmf_object_handle tm_obj, rdpa_tm_orl_prty orl_prty)
{
    tm_drv_priv_t *tm;
    int rc = 0;

    if (!tm_obj)
        return BDMF_ERR_PARM;
    tm = (tm_drv_priv_t *)bdmf_obj_data(tm_obj);

    /* This operation is only allowed for top-level TM */
    if (tm->upper_level_tm)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, tm_obj, "Can't set orl_prty on subsidiary scheduler\n");

    if (tm_obj->state != bdmf_state_init && us_overall_rl_obj)
        rc = egress_tm_orl_set(tm_obj, orl_prty);
    if (!rc)
        tm->orl_prty = orl_prty;

    return rc;
}

int  _rdpa_egress_tm_channel_queue_to_rdd(rdpa_traffic_dir dir, int channel, uint32_t queue_id, int *rc_id, int *queue)
{
    return _rdpa_egress_tm_dir_channel_queue_to_rdd(dir, channel, queue_id, rc_id, queue);
}


/* Get queue_id by port / rate controller and queue index (priority) */
static int __rdpa_egress_tm_queue_id_by_dir_channel_rc_queue(rdpa_traffic_dir dir, int channel_id, int rc_id, int queue,
    bdmf_object_handle *mo, uint32_t *queue_id)
{
    tm_channel_t *channel = egress_tm_channel_get(dir, channel_id);
    tm_qtm_ctl_t *qtm_ctl;
    int rc = BDMF_ERR_NOENT;

    if ((unsigned)queue >= RDPA_MAX_EGRESS_QUEUES)
        return BDMF_ERR_PARM;

    if (!channel)
        return BDMF_ERR_NOENT;

    STAILQ_FOREACH(qtm_ctl, &channel->qtm_ctls, list)
    {
        tm_drv_priv_t *tm;
#ifdef XRDP
        if (qtm_ctl->sched_id != rc_id)
#else
        if (qtm_ctl->rc_id != rc_id)
#endif
            continue;
        if (!qtm_ctl->queue_configured[queue])
            break; /* BDMF_ERR_NOENT is set by default */
        /* Found it */
        tm = (tm_drv_priv_t *)bdmf_obj_data(qtm_ctl->egress_tm);
        *mo = qtm_ctl->egress_tm;
        *queue_id = tm->queue_cfg[queue].queue_id;
        rc = 0;
        break;
    }

    return rc;
}

/* Get queue_id by port / rate controller and queue index (priority) */
static int _rdpa_egress_tm_queue_id_by_dir_channel_rc_queue(rdpa_traffic_dir dir,
    int channel_id, int rc_id, int queue, uint32_t *queue_id)
{
    bdmf_object_handle egress_tm;
    return __rdpa_egress_tm_queue_id_by_dir_channel_rc_queue(dir, channel_id, rc_id, queue,
        &egress_tm, queue_id);
}

int _rdpa_egress_tm_queue_id_by_wan_flow_rc_queue(int *wan_flow, int rc_id, int queue, uint32_t *queue_id)
{
    int channel_id;
    int flow = *wan_flow;
    bdmf_object_handle egress_tm = NULL;
    int rc;

#ifdef CONFIG_BCM_TCONT
    if (rdpa_is_gpon_or_xgpon_mode())
    {
        int rc;
        /* Find tcont by GEM. channel is tcont->index */
        rc = rdpa_gem_flow_id_to_tcont_id(flow, &channel_id);
        if (rc)
            return rc;
    }
    else
#endif
    {
        channel_id = flow;
    }
    rc = __rdpa_egress_tm_queue_id_by_dir_channel_rc_queue(rdpa_dir_us, channel_id, rc_id, queue, &egress_tm, queue_id);
    if (egress_tm)
    {
        bdmf_object_handle root = egress_tm_get_root_object(egress_tm);
        tm_drv_priv_t *root_tm = (tm_drv_priv_t *)bdmf_obj_data(root);

        /* Special handling for epon whereas root egress_tm represents channel group */
        if (egress_tm_is_ch_group(root_tm))
            *wan_flow = root_tm->channel_group;
    }
    return rc;
}

int _rdpa_egress_tm_queue_id_by_channel_rc_queue(int channel_id, int rc_id, int queue, uint32_t *queue_id)
{
    return _rdpa_egress_tm_queue_id_by_dir_channel_rc_queue(rdpa_dir_us, channel_id, rc_id, queue, queue_id);
}

int _rdpa_egress_tm_queue_id_by_lan_port_queue(rdpa_if port, int queue, uint32_t *queue_id)
{
    int channel_id = _rdpa_port_channel(port);
    int rc_id = RDD_RATE_LIMITER_PORT_0 + channel_id;

    return _rdpa_egress_tm_queue_id_by_dir_channel_rc_queue(rdpa_dir_ds, channel_id, rc_id, queue, queue_id);
}


int _rdpa_egress_tm_rc_queue_id_tc_check_by_dir_channel_queue(
    rdpa_traffic_dir dir, int channel, uint32_t queue_id, int *rc_id,
    int *queue, int *tc)
{
    tm_queue_hash_entry_t *entry;
    tm_qtm_ctl_t *rate_ctl;
    rdpa_tm_queue_cfg_t *queue_cfg;

    entry = egress_tm_hash_shortcut_get(egress_tm_dir_channel(dir, channel), queue_id);
    if (!entry)
    {
        if (tc != NULL)
            *tc = 0;
        return BDMF_ERR_NOENT;
    }

    rate_ctl = egress_tm_hash_entry_container(entry);
#ifdef XRDP
    *rc_id = rate_ctl->sched_id;
#else
    *rc_id = rate_ctl->rc_id;
#endif
    *queue = entry->rdp_queue_index;

    if (tc != NULL)
    {
        queue_cfg = &rate_ctl->queue_cfg[entry->queue_index];
        if ((*tc < 32) && (queue_cfg->priority_mask_0 & (0x1 << *tc)))
            *tc = 1;
        else if ((*tc >= 32) && (*tc < 64) && (queue_cfg->priority_mask_1 & (0x1 << (*tc - 32))))
            *tc = 1;
        else
            *tc = 0;
    }

    return 0;
}

int _rdpa_egress_tm_channel_queue_enable_set(bdmf_object_handle tm_obj, int channel_id, uint32_t queue_index, bdmf_boolean enable)
{
    tm_drv_priv_t *tm;
    tm_channel_t *channel;
    int rc = 0;
    rdpa_tm_queue_cfg_t disable_cfg = {};

    if (!tm_obj)
        return BDMF_ERR_PARM;
    
    tm = (tm_drv_priv_t *)bdmf_obj_data(tm_obj);

    /* Find channel context */
    channel = egress_tm_channel_get(tm->dir, channel_id);
    if (!channel)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_INTERNAL, tm_obj, "channel_id %d is invalid\n", channel_id);

    /* This operation is only allowed for queue-level TM */
    if (tm->level != rdpa_tm_level_queue)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, tm_obj, "Can't enable/disable subsidiary scheduler\n");

    /* Configure specified queue if exist */
    if (tm->queue_configured[queue_index]) 
    {
        if (tm->queue_cfg[queue_index].drop_threshold)
            rc = egress_tm_queue_cfg_on_channel(tm_obj, channel, queue_index, enable ? &tm->queue_cfg[queue_index] : &disable_cfg);
    }
    
    return rc;
}

int _rdpa_egress_tm_channel_enable_set(bdmf_object_handle tm_obj, int channel_id, bdmf_boolean enable)
{
    tm_drv_priv_t *tm;
    tm_channel_t *channel;
    int rc = 0;

    if (!tm_obj)
        return BDMF_ERR_PARM;
    
    tm = (tm_drv_priv_t *)bdmf_obj_data(tm_obj);

    /* Find channel context */
    channel = egress_tm_channel_get(tm->dir, channel_id);
    if (!channel)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_INTERNAL, tm_obj, "channel_id %d is invalid\n", channel_id);

    /* Enable/disable egress_tm channel queues */
    rc = egress_tm_enable_set_on_channel(tm_obj, channel, enable);

    return rc;
}

#ifdef XRDP
static int _rdpa_egress_tm_queue_id_by_dir_channel_qm_queue(rdpa_traffic_dir dir,
    int channel_id, int qm_queue, bdmf_object_handle *mo, uint32_t *queue_id)
{
    tm_channel_t *channel = egress_tm_channel_get(dir, channel_id);
    tm_qtm_ctl_t *qtm_ctl;
    int queue;

    if (!channel)
        return BDMF_ERR_NOENT;

    STAILQ_FOREACH(qtm_ctl, &channel->qtm_ctls, list)
    {
        tm_drv_priv_t *tm;
        for (queue = 0; queue < RDPA_MAX_EGRESS_QUEUES; queue++)
        {
            if (!qtm_ctl->queue_configured[queue])
                continue;
            if (qtm_ctl->hash_entry[queue].rdp_queue_index == qm_queue)
            {
                /* Found it */
                tm = (tm_drv_priv_t *)bdmf_obj_data(qtm_ctl->egress_tm);
                *mo = qtm_ctl->egress_tm;
                *queue_id = tm->queue_cfg[queue].queue_id;
                return 0;
            }
        }
    }

    return BDMF_ERR_NOENT;
}

/* Get RDPA queue_id by wan_flow and QM queue */
int _rdpa_egress_tm_queue_id_by_wan_flow_qm_queue(int *wan_flow, int qm_queue, uint32_t *queue_id)
{
    int channel_id;
    int flow = *wan_flow;
    bdmf_object_handle egress_tm = NULL;
    int rc;

#ifdef CONFIG_BCM_TCONT
    if (rdpa_is_gpon_or_xgpon_mode())
    {
        int rc;
        /* Find tcont by GEM. channel is tcont->index */
        rc = rdpa_gem_flow_id_to_tcont_id(flow, &channel_id);
        if (rc)
            return rc;
    }
    else
#endif
    {
        channel_id = flow;
    }
    rc = _rdpa_egress_tm_queue_id_by_dir_channel_qm_queue(rdpa_dir_us, channel_id, qm_queue, &egress_tm, queue_id);
    if (egress_tm)
    {
        bdmf_object_handle root = egress_tm_get_root_object(egress_tm);
        tm_drv_priv_t *root_tm = (tm_drv_priv_t *)bdmf_obj_data(root);

        /* Special handling for epon whereas root egress_tm represents channel group */
        if (egress_tm_is_ch_group(root_tm))
            *wan_flow = root_tm->channel_group;
    }
    return rc;
}

/* Get RDPA queue_id by lan_port and QM queue */
int _rdpa_egress_tm_queue_id_by_lan_port_qm_queue(rdpa_if port, int qm_queue, uint32_t *queue_id)
{
    bdmf_object_handle egress_tm = NULL;
    int channel_id = _rdpa_port_channel(port);
    return _rdpa_egress_tm_queue_id_by_dir_channel_qm_queue(rdpa_dir_ds, channel_id, qm_queue, &egress_tm, queue_id);
}

#endif

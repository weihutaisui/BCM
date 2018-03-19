/*
* <:copyright-BRCM:2017:proprietary:standard
* 
*    Copyright (c) 2017 Broadcom 
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

#include "tmctl_bdmf_rdpa.h"
#include "rdpactl_api.h"

typedef struct
{
    tmctl_devType_e dev_type;
    rdpa_if if_id;
    rdpa_traffic_dir dir;
    rdpa_tm_level_type tm_level;
    rdpa_tm_sched_mode tm_mode;
    int num_queues;
    BOOL set_dual_rate;
} set_parm_t;

static void get_port_tm_caps(int dir, tmctl_devType_e dev_type, tmctl_portTmParms_t *tm)
{
    tm->queueShaper = TRUE;
    tm->dualRate = FALSE;

    if (dir == rdpa_dir_ds) 
    {
        tm->maxQueues = MAX_Q_PER_LAN_TM;
        tm->maxSpQueues = MAX_Q_PER_LAN_TM;
        tm->schedCaps = TMCTL_SP_CAPABLE;
#if defined(BCM_PON_XRDP)
        tm->schedCaps |= TMCTL_WRR_CAPABLE | TMCTL_SP_WRR_CAPABLE | TMCTL_1LEVEL_CAPABLE;
#else
        tm->queueShaper = FALSE;
#endif /* BCM_PON_XRDP */
        tm->portShaper = TRUE;
    }
    else
    {
        tm->maxQueues = MAX_Q_PER_WAN_TM;
        tm->maxSpQueues = MAX_Q_PER_WAN_TM;
        tm->schedCaps = RDPA_TM_SP_CAPABLE | RDPA_TM_WRR_CAPABLE | RDPA_TM_SP_WRR_CAPABLE;
#if defined(BCM_PON_XRDP)
        tm->schedCaps |= RDPA_TM_1LEVEL_CAPABLE;
        tm->dualRate = TRUE;
#endif  /* BCM_PON_XRDP */
        tm->portShaper = dev_type == TMCTL_DEV_ETH ? TRUE : FALSE;
    }

    tmctl_debug("dir[%d], maxQueues[%d], maxSpQueues[%d], portShaper[%d], queueShaper[%d], schedCaps[%d], dualRate[%d]", 
                dir, tm->maxQueues, tm->maxSpQueues, tm->portShaper, tm->queueShaper, tm->schedCaps, tm->dualRate );
}

static int get_tm_owner(tmctl_devType_e dev_type, rdpa_if if_id, bdmf_object_handle *owner)
{
    int ret = TMCTL_ERROR;

    tmctl_debug("Devtype[%d], if_id[%d]", dev_type, if_id);

    switch (dev_type)
    {
        case TMCTL_DEV_ETH:
            ret = rdpa_port_get(if_id, owner);
            break;
        case TMCTL_DEV_GPON:
            /* For T-CONT device type, index 0 is reserved for Default ALLOC Id so we
            need to increment by 1 the index of the TCONT that comes from OMCI userspace application. */
            ret = rdpa_tcont_get(++if_id, owner);
            break;
        case TMCTL_DEV_EPON:
        case TMCTL_DEV_SVCQ:
        default:
            tmctl_error("Device %d not supported", dev_type);
            break;
    }

    return ret;
}

static int notify_owner(tmctl_devType_e dev_type, rdpa_if if_id)
{
    bdmf_object_handle owner = BDMF_NULL;
    int ret;

    ret = get_tm_owner(dev_type, if_id, &owner);
    if (ret)
    {
        tmctl_error("Failed to get tm owner, ret[%d]", ret);
        return TMCTL_ERROR;
    }
 
    switch (dev_type)
    {
        case TMCTL_DEV_ETH:
            {
                rdpa_port_tm_cfg_t tm_cfg = {0};
                ret = rdpa_port_tm_cfg_get(owner, &tm_cfg);
                if (ret)
                {
                    tmctl_error("rdpa_port_tm_cfg_get failed, ret[%d]", ret);
                    goto error;
                }

                tm_cfg.sched = BDMF_NULL;
                ret = rdpa_port_tm_cfg_set(owner, &tm_cfg);
                if (ret)
                {
                    tmctl_error("rdpa_port_tm_cfg_set failed, ret[%d]", ret);
                    goto error;
                }
            }
            break;
        case TMCTL_DEV_GPON:
            ret = rdpa_tcont_egress_tm_set(owner, BDMF_NULL);
            if (ret)
            {
                tmctl_error("rdpa_tcont_egress_tm_set failed, rdpa_if[%d], ret[%d]", if_id, ret);
                goto error;
            }
            break;
        case TMCTL_DEV_EPON:
        case TMCTL_DEV_SVCQ:
        default:
            tmctl_error("Device %d not supported", dev_type);
            goto error;
    }
        
    bdmf_put(owner);
    return TMCTL_SUCCESS;
error:
    if (owner)
        bdmf_put(owner);
    return TMCTL_ERROR;
}

static int set_up_tm(set_parm_t *parm, bdmf_object_handle *egress_tm)
{
    bdmf_object_handle owner = BDMF_NULL;
    bdmf_mattr_handle mattr = BDMF_NULL;
    int ret;

    tmctl_debug("Dir[%d], tm_level[%d], tm_mode[%d], owner_if[%d]", 
                parm->dir, parm->tm_level, parm->tm_mode, parm->if_id);
    
    *egress_tm = BDMF_NULL;

    mattr = bdmf_mattr_alloc(rdpa_egress_tm_drv());
    if (!mattr)
    {
        tmctl_error("bdmf_mattr_alloc failed");
        goto error;
    }

    ret = rdpa_egress_tm_dir_set(mattr, parm->dir);
    if (ret)
    {
        tmctl_error("rdpa_egress_tm_dir_set failed");
        goto error;
    }

    ret = rdpa_egress_tm_level_set(mattr, parm->tm_level);
    if (ret)
    {
        tmctl_error("rdpa_egress_tm_level_set failed");
        goto error;
    }

    ret = rdpa_egress_tm_mode_set(mattr, parm->tm_mode);
    if (ret)
    {
        tmctl_error("rdpa_egress_tm_mode_set failed");
        goto error;
    }

    ret = rdpa_egress_tm_num_queues_set(mattr, parm->num_queues);
    if (ret)
    {
        tmctl_error("rdpa_egress_tm_num_queues_set failed");
        goto error;
    }

    if (parm->tm_mode == rdpa_tm_sched_sp_wrr)
    {
        ret = rdpa_egress_tm_num_sp_elements_set(mattr, rdpa_tm_num_sp_elem_0);
        if (ret)
        {
            tmctl_error("rdpa_egress_tm_num_sp_elements_set failed");
            goto error;
        }
    }
    
    if (parm->set_dual_rate)
    {
        ret = rdpa_egress_tm_rl_rate_mode_set(mattr, rdpa_tm_rl_dual_rate);
        if (ret)
        {
            tmctl_error("rdpa_egress_tm_rl_rate_mode_set failed");
            goto error;
        }
    }
    
    ret = get_tm_owner(parm->dev_type, parm->if_id, &owner);
    if (ret)
    {
        tmctl_error("Failed to get tm owner, if_id[%d], ret[%d]", parm->if_id, ret);
        goto error;
    }

    ret = bdmf_new_and_set(rdpa_egress_tm_drv(), owner, mattr, egress_tm);
    if (ret)
    {
        tmctl_error("bdmf_new_and_set failed to create egress_tm obj, ret[%d]", ret);
        goto error;
    }

    bdmf_put(owner);
    owner = BDMF_NULL;

    ret = rdpa_egress_tm_enable_set(*egress_tm, TRUE);
    if (ret)
    {
        tmctl_error("rdpa_egress_tm_enable_set failed, ret[%d]", ret);
        goto error;
    }

    bdmf_mattr_free(mattr);
    return TMCTL_SUCCESS;
error:
    if (mattr)
        bdmf_mattr_free(mattr);
    if (*egress_tm)
    {
        bdmf_destroy(*egress_tm);
        notify_owner(parm->dev_type, parm->if_id);
    }
    if (owner)
        bdmf_put(owner);

    return TMCTL_ERROR;
}

static int get_tm(tmctl_devType_e dev_type, rdpa_if if_id, bdmf_object_handle *egress_tm)
{
    bdmf_object_handle temp_tm = BDMF_NULL;
    bdmf_object_handle owner = BDMF_NULL;
    int ret;

    *egress_tm = BDMF_NULL;

    ret = get_tm_owner(dev_type, if_id, &owner);
    if (ret)
    {
        tmctl_error("Failed to get tm owner, ret[%d]", ret);
        return TMCTL_ERROR;
    }
 
    switch (dev_type)
    {
        case TMCTL_DEV_ETH:
            {
                rdpa_port_tm_cfg_t tm_cfg = {0};
                ret = rdpa_port_tm_cfg_get(owner, &tm_cfg);
                if (ret)
                {
                    tmctl_error("Failed to get tm_cfg, rdpa_if[%d], ret[%d]", if_id, ret);
                    goto error;
                }
                temp_tm = tm_cfg.sched;
            }
            break;
        case TMCTL_DEV_GPON:
            ret = rdpa_tcont_egress_tm_get(owner, &temp_tm);
            if (ret)
            {
                tmctl_error("Failed to get egress_tm, rdpa_if[%d], ret[%d]", if_id, ret);
                goto error;
            }
            break;
        case TMCTL_DEV_EPON:
        case TMCTL_DEV_SVCQ:
        default:
            tmctl_error("Device %d not supported", dev_type);
            goto error;
    }

    *egress_tm = temp_tm;
    if (*egress_tm)
        bdmf_get(*egress_tm);
    bdmf_put(owner);
    return TMCTL_SUCCESS;
error:
    bdmf_put(owner);
    return TMCTL_ERROR;
}

static void print_sched_caps(uint32_t sched_caps)
{
    printf("Sched caps are:\n");

    if (sched_caps & TMCTL_SP_WRR_CAPABLE)              
        printf("SP and WRR [0]\n"); 
    if (sched_caps & TMCTL_SP_CAPABLE) 
        printf("SP [256]\n");
    if (sched_caps & TMCTL_WRR_CAPABLE)              
        printf("WRR [512]\n");
}

static int set_up_default_queues(bdmf_object_handle egress_tm, 
                                 uint32_t sched_type, 
                                 rdpa_drv_ioctl_dev_type rdpa_dev, 
                                 set_parm_t *parm, 
                                 int cfg_flags)
{
    rdpa_tm_queue_cfg_t queue_cfg = {0};
    int prio_index;
    int qid;
    int ret;
    
    queue_cfg.drop_alg = rdpa_tm_drop_alg_dt;
    queue_cfg.drop_threshold = getDeviceQueueSize(getDefaultQueueSize(rdpa_dev, parm->dir));
    queue_cfg.weight = (parm->tm_mode == rdpa_tm_sched_sp || parm->tm_mode == rdpa_tm_sched_sp_wrr)? 0 : 1;
    queue_cfg.stat_enable = 1;

    if (sched_type == TMCTL_SCHED_TYPE_SP_WRR)
    {
        ret = rdpa_egress_tm_num_sp_elements_set(egress_tm, parm->num_queues);
        if (ret)
        {
            tmctl_error("rdpa_egress_tm_num_sp_elements_set failed, ret[%d]", ret);
            goto error;
        }
    }

    /* Initialize the default queues */
    for (qid = 0; qid < parm->num_queues; qid++)
    {
        if (cfg_flags & TMCTL_QIDPRIO_MAP_Q7P7)
            prio_index = rdpaCtl_getQueuePrioIndex(qid, parm->dir, parm->num_queues, qid);
        else
            prio_index = rdpaCtl_getQueuePrioIndex(qid, parm->dir, parm->num_queues,
                    (parm->num_queues - (uint32)qid - 1));

        queue_cfg.queue_id = qid;
        ret = rdpa_egress_tm_queue_cfg_set(egress_tm, prio_index, &queue_cfg);
        if (ret)
        {
            tmctl_error("rdpa_egress_tm_queue_cfg_set failed, ret[%d]", ret);
            goto error;
        }
    }

    return TMCTL_SUCCESS;
error:
    return TMCTL_ERROR;
}

static int check_caps(uint32_t sched_type, uint32_t sched_caps)
{
    if ((sched_type == TMCTL_SCHED_TYPE_SP    && !(sched_caps & TMCTL_SP_CAPABLE)) ||
       (sched_type == TMCTL_SCHED_TYPE_WRR    && !(sched_caps & TMCTL_WRR_CAPABLE)) ||
       (sched_type == TMCTL_SCHED_TYPE_SP_WRR && !(sched_caps & TMCTL_SP_WRR_CAPABLE)))
    {
       tmctl_error("Configured scheduler type %d is not supported", sched_type);
       print_sched_caps(sched_caps);
       return TMCTL_ERROR;
    }

    return TMCTL_SUCCESS;
}

static int check_cfg(tmctl_portTmParms_t *tm ,uint32_t sched_type, int num_queues, BOOL set_dual_rate)
{
    if (check_caps(sched_type, tm->schedCaps))
       return TMCTL_ERROR;

    if (num_queues > tm->maxQueues)
    {
        tmctl_error("Num queues wanted[%d] is higher than max queues allowed[%d]", num_queues, tm->maxQueues);
        return TMCTL_ERROR;
    }

    if (num_queues > 0 && num_queues != 8 && num_queues != 16 && num_queues != 32)
    {
        tmctl_error("Num queues wanted[%d] is not allowed[8, 16, 32]", num_queues);
        return TMCTL_ERROR;
    }

    if (set_dual_rate && !tm->dualRate)
    {
        tmctl_error("Dual rate is not supported");
        return TMCTL_ERROR;
    }

    return TMCTL_SUCCESS;
}

static int get_default_num_queues(rdpa_traffic_dir dir, uint32_t sched_type)
{
#ifndef CHIP_6846
    if (dir == rdpa_dir_us && sched_type == TMCTL_SCHED_TYPE_SP_WRR)
        return 32;
#endif
    return 8;
}

tmctl_ret_e tmctl_RdpaTmInit(rdpa_drv_ioctl_dev_type rdpa_dev,
                                    tmctl_devType_e dev_type,
                                    rdpa_if if_id,
                                    uint32_t cfg_flags,
                                    int num_queues)
{
    bdmf_object_handle egress_tm = BDMF_NULL;
    tmctl_portTmParms_t tm = {0};
    set_parm_t parm = {0};
    uint32_t sched_type;
    int ret;
    
    parm.if_id = if_id;
    parm.dev_type = dev_type;
    parm.tm_level = rdpa_tm_level_queue;
    parm.dir = getDir(rdpa_dev, if_id);
    get_port_tm_caps(parm.dir, dev_type, &tm);
    sched_type = cfg_flags & TMCTL_SCHED_TYPE_MASK;
    parm.set_dual_rate = cfg_flags & TMCTL_SET_DUAL_RATE;
    if (check_cfg(&tm , sched_type, num_queues, parm.set_dual_rate))
       return TMCTL_ERROR;

    ret = get_tm(dev_type, if_id, &egress_tm);
    if (ret)
    {
        tmctl_debug("get_tm failed");
        return TMCTL_ERROR;
    }
    else if (egress_tm)
    {
        tmctl_debug("Destroying old Tm object");
        bdmf_put(egress_tm);
        ret = bdmf_destroy(egress_tm);
        if (ret)
        {
            tmctl_debug("bdmf_destroy failed");
            return TMCTL_ERROR;
        }
        egress_tm = BDMF_NULL;
    }

    switch (sched_type)
    {
        case TMCTL_SCHED_TYPE_SP_WRR:
            parm.tm_mode = rdpa_tm_sched_sp_wrr;
            break;
        case TMCTL_SCHED_TYPE_SP:
            parm.tm_mode = rdpa_tm_sched_sp;
            break;
        case TMCTL_SCHED_TYPE_WRR:
            parm.tm_mode = rdpa_tm_sched_wrr;
            break;
        default:
            tmctl_error("Port sched type 0x%x is not supported.", sched_type);
            return TMCTL_ERROR;
    }

    if (num_queues > 0)
        parm.num_queues = num_queues;
    else
        parm.num_queues = get_default_num_queues(parm.dir, sched_type);

    ret = set_up_tm(&parm, &egress_tm);
    if (ret)
    {
        tmctl_error("Failed to set up new tm");
        goto error;
    }

    if (cfg_flags & TMCTL_INIT_DEFAULT_QUEUES)   /* The default queues initialization is required */
    {
        ret = set_up_default_queues(egress_tm, sched_type, rdpa_dev, &parm, cfg_flags);
        if (ret)
        {
            tmctl_error("set_up_default_queues failed");
            goto error;
        }
    }

    return TMCTL_SUCCESS;
error:
    if (egress_tm)
    {
        bdmf_destroy(egress_tm);
        notify_owner(dev_type, if_id);
    }
    return TMCTL_ERROR;
}

tmctl_ret_e tmctl_RdpaTmUninit(tmctl_devType_e dev_type, rdpa_if if_id)
{
    bdmf_object_handle egress_tm = BDMF_NULL;
    int ret;

    ret = get_tm(dev_type, if_id, &egress_tm);
    if (ret)
    {
        tmctl_error("Failed to get egress_tm");
        goto error;
    }
    if (!egress_tm)
        return TMCTL_SUCCESS;

    bdmf_put(egress_tm);
    ret = bdmf_destroy(egress_tm);
    if (ret)
    {
        tmctl_error("bdmf_destroy failed, ret[%d]", ret);
        goto error;
    }
    egress_tm = BDMF_NULL;

    ret = notify_owner(dev_type, if_id);
    if (ret)
    {
        tmctl_error("notify_owner failed, ret[%d]", ret);
        goto error;
    }

    return TMCTL_SUCCESS;
error:
    if (egress_tm)
        bdmf_put(egress_tm);    
    return TMCTL_ERROR;
}

static int get_q_idx(bdmf_object_handle egress_tm, int id)
{
    rdpa_tm_queue_cfg_t queue_cfg = {0};
    uint8_t max_queues;
    int ret;
    int i;

    ret = rdpa_egress_tm_num_queues_get(egress_tm, &max_queues);
    if (ret)
    {
        tmctl_error("rdpa_egress_tm_num_queues_get failed");
        return TMCTL_ERROR;
    }

    for (i = 0; i < max_queues ; ++i)
    {
        ret = rdpa_egress_tm_queue_cfg_get(egress_tm, i, &queue_cfg);
        if (ret && ret != BDMF_ERR_NOENT)
        {
            tmctl_error("rdpa_egress_tm_queue_cfg_get failed");
            break;
        }
        
        if (!ret && (queue_cfg.queue_id == id))
            return i;
    }
    
    return TMCTL_ERROR;
}

tmctl_ret_e tmctl_RdpaQueueCfgGet(rdpa_drv_ioctl_dev_type rdpa_dev,
                                tmctl_devType_e dev_type,
                                rdpa_if if_id,
                                int queue_id,
                                tmctl_queueCfg_t *qcfg_p)
{
    bdmf_object_handle egress_tm = BDMF_NULL;
    rdpa_tm_queue_cfg_t queue_cfg = {0};
    rdpa_tm_rl_cfg_t rl_cfg = {0};
    rdpa_tm_sched_mode tm_mode;
    int idx;
    int ret;

    get_tm(dev_type, if_id, &egress_tm);
    if (!egress_tm)
    {
        tmctl_error("Failed to get egress_tm, qid[%d]", queue_id);
        goto error;
    }

    idx = get_q_idx(egress_tm, queue_id);
    if (idx < 0)
    {
        bdmf_put(egress_tm);
        return TMCTL_NOT_FOUND;
    }

    ret = rdpa_egress_tm_mode_get(egress_tm, &tm_mode);
    if (ret)
    {
        tmctl_error("rdpa_egress_tm_mode_get failed, if_id[%d], qid[%d]", if_id, queue_id);
        goto error;
    }  

    ret = rdpa_egress_tm_queue_cfg_get(egress_tm, idx, &queue_cfg);
    if (ret)
    {
        tmctl_error("rdpa_egress_tm_queue_cfg_get failed, if_id[%d], qid[%d]", if_id, queue_id);
        goto error;
    }

    if (tm_mode == rdpa_tm_sched_disabled && queue_cfg.weight == 0) 
    {
        ret = rdpa_egress_tm_rl_get(egress_tm, &rl_cfg);
        if (ret)
        {
            tmctl_error("rdpa_egress_tm_rl_get failed, if_id[%d], qid[%d]", if_id, queue_id);
            goto error;
        }

        qcfg_p->shaper.shapingRate = rl_cfg.af_rate / 1000;
    }     

    qcfg_p->qid = queue_id;
    qcfg_p->qsize = getUsrQueueSize(queue_cfg.drop_threshold);
    qcfg_p->weight = queue_cfg.weight;
    if (queue_cfg.weight)
    {
        qcfg_p->schedMode = TMCTL_SCHED_WRR;
        qcfg_p->priority = INVALID_ID;
    }
    else
    {
        qcfg_p->schedMode = TMCTL_SCHED_SP;
        qcfg_p->priority = 0;
    }
#if defined(BCM_PON_XRDP)
    if (tm_mode != rdpa_tm_sched_disabled)
        qcfg_p->shaper.shapingRate = queue_cfg.rl_cfg.af_rate / 1000;
#endif
    qcfg_p->shaper.minRate = 0;
    qcfg_p->shaper.shapingBurstSize = 0;

    bdmf_put(egress_tm);    
    return TMCTL_SUCCESS;
error:
    if (egress_tm)
        bdmf_put(egress_tm);        
    return TMCTL_ERROR;
}

static int get_highest_wrr_q(bdmf_object_handle egress_tm, int ignord_id)
{
    rdpa_tm_queue_cfg_t queue_cfg = {0};
    rdpa_tm_num_sp_elem num_sp_queues;
    uint8_t max_queues;
    int ret;
    int i;

    ret = rdpa_egress_tm_num_queues_get(egress_tm, &max_queues);
    if (ret)
    {
        tmctl_error("rdpa_egress_tm_num_queues_get failed");
        return TMCTL_ERROR;
    }

    ret = rdpa_egress_tm_num_sp_elements_get(egress_tm, &num_sp_queues);
    if (ret)
    {
        tmctl_error("rdpa_egress_tm_num_sp_elements_get failed");
        return TMCTL_ERROR;
    }
    
    for (i = num_sp_queues; i < max_queues ; ++i)
    {
        ret = rdpa_egress_tm_queue_cfg_get(egress_tm, i, &queue_cfg);
        if (ret && ret != BDMF_ERR_NOENT)
        {
            tmctl_error("rdpa_egress_tm_queue_cfg_get failed");
            return TMCTL_ERROR;
        }

        if (!ret && IS_ACTIV_Q(queue_cfg) && queue_cfg.queue_id != ignord_id)
            break;
    }

    return i;
}

static rdpa_tm_num_sp_elem get_num_sp_queues(int highest_sp_q)
{
    if (highest_sp_q < 0)
        return rdpa_tm_num_sp_elem_0;
    if (highest_sp_q < 2)
        return rdpa_tm_num_sp_elem_2;
    if (highest_sp_q < 4)
        return rdpa_tm_num_sp_elem_4;
    if (highest_sp_q < 8)
        return rdpa_tm_num_sp_elem_8;
    if (highest_sp_q < 16)
        return rdpa_tm_num_sp_elem_16;
    return rdpa_tm_num_sp_elem_32;
}

/*
 * return the minimum number of SP queues if current queue(idx) will be remove   
 */
static int num_sp_queues_after_rm(bdmf_object_handle egress_tm, int idx, rdpa_tm_num_sp_elem *new_num_sp_queues)
{
    rdpa_tm_num_sp_elem curr_num_sp_queues;
    rdpa_tm_queue_cfg_t queue_cfg = {0};
    int highst_sp_q = -1;
    int ret;
    int i;

    ret = rdpa_egress_tm_num_sp_elements_get(egress_tm, &curr_num_sp_queues);
    if (ret)
    {
        tmctl_error("rdpa_egress_tm_num_sp_elements_get failed");
        return TMCTL_ERROR;
    }

    for (i = 0; i < curr_num_sp_queues; ++i)
    {
        /* skip current queue it will be removed */
        if (i != idx)
        {
            ret = rdpa_egress_tm_queue_cfg_get(egress_tm, i, &queue_cfg);
            if (ret && ret != BDMF_ERR_NOENT)
            {
                tmctl_error("rdpa_egress_tm_queue_cfg_get failed");
                return TMCTL_ERROR;
            }

            if (!ret && IS_ACTIV_Q(queue_cfg))
                highst_sp_q = i;
        }
    }

    *new_num_sp_queues = get_num_sp_queues(highst_sp_q);
    return TMCTL_SUCCESS;
}

/*
 * return the minimum number of SP queues if current queue(idx) will be add   
 */
static int num_sp_queues_after_add(bdmf_object_handle egress_tm, int idx, rdpa_tm_num_sp_elem *new_num_sp_queues)
{
    rdpa_tm_num_sp_elem curr_num_sp_queues;
    int ret;

    ret = rdpa_egress_tm_num_sp_elements_get(egress_tm, &curr_num_sp_queues);
    if (ret)
    {
        tmctl_error("rdpa_egress_tm_num_sp_elements_set failed");
        return TMCTL_ERROR;
    }
    
    *new_num_sp_queues = idx < curr_num_sp_queues ? curr_num_sp_queues : get_num_sp_queues(idx);
    return TMCTL_SUCCESS;
}

static void print_allowed_sched_mode(int mode)
{
    switch (mode)
    {
        case rdpa_tm_sched_sp_wrr:
            printf("Allowed sched modes are: SP and WRR.\n");
            break;
        case rdpa_tm_sched_sp:
             printf("Allowed sched mode is: SP.\n");
            break;
        case rdpa_tm_sched_wrr:
            printf("Allowed sched mode is: WRR.\n");
            break;
        case rdpa_tm_sched_disabled:
            printf("Sched mode is disabled.\n");
            break;
        default:
            tmctl_error("Sched mode %d, unknown.\n", mode);
            break;
    }
}

static int get_wrr_queue_idx(bdmf_object_handle egress_tm, int max, int min, int qid)
{
    rdpa_tm_queue_cfg_t queue_cfg = {0};
    int ret;
    int hights_free_queue = -1;
    int i;

    for (i = min; i < max; ++i)
    {
        ret = rdpa_egress_tm_queue_cfg_get(egress_tm, i, &queue_cfg);
        if (ret && ret != BDMF_ERR_NOENT)
        {
            tmctl_error("rdpa_egress_tm_queue_cfg_get failed");
            return -1;
        }
        if (!ret && queue_cfg.queue_id == qid)
            return i;

        if (ret || !IS_ACTIV_Q(queue_cfg))
             hights_free_queue = i;
    }

    if (hights_free_queue < 0)
        tmctl_error("No free queue index between min[%d] and max[%d]", min, max);

    return hights_free_queue;
}

static int get_index_by_priority(int priority, int max_queues)
{
    return max_queues - priority - 1;
}

static int prepare_set_q_in_sp_mode(bdmf_object_handle egress_tm, tmctl_queueCfg_t *qcfg_p, int current_idx)
{
    rdpa_tm_queue_cfg_t new_queue_cfg = {0};
    uint8_t max_queues;
    int idx;
    int ret;

    ret = rdpa_egress_tm_num_queues_get(egress_tm, &max_queues);
    if (ret)
    {
        tmctl_error("rdpa_egress_tm_num_queues_get failed, qid[%d]",qcfg_p->qid);
        return TMCTL_ERROR;
    }

    idx = get_index_by_priority(qcfg_p->priority, max_queues);
    ret = rdpa_egress_tm_queue_cfg_get(egress_tm, idx, &new_queue_cfg);
    if (ret && ret != BDMF_ERR_NOENT)
    {
        tmctl_error("rdpa_egress_tm_queue_cfg_get failed, qid[%d], index[%d]",qcfg_p->qid, idx);
        return TMCTL_ERROR;
    }
    
    if (!ret && IS_ACTIV_Q(new_queue_cfg) && new_queue_cfg.queue_id != qcfg_p->qid)
    {
        tmctl_error("Priority[%d](index[%d]) allready in use, qid[%d] current_qid[%d]", 
                qcfg_p->priority, idx, qcfg_p->qid, new_queue_cfg.queue_id);
        return TMCTL_ERROR;
    }

    /* if changing priority delete existing queue */
    if (idx != current_idx && (current_idx >= 0))
    {
        ret = rdpa_egress_tm_queue_cfg_delete(egress_tm, current_idx);
        if (ret)
        {
            tmctl_error("rdpa_egress_tm_queue_cfg_delete failed, qid[%d], currnet index[%d], ret[%d]",qcfg_p->qid, current_idx, ret);
            return TMCTL_ERROR;
        }
    }

    return idx;
}

static int prepare_set_q_in_wrr_mode(bdmf_object_handle egress_tm, tmctl_queueCfg_t *qcfg_p, int idx)
{
    uint8_t max_queues;
    int ret;

    /* queue exist */
    if (idx >= 0)
        goto exit;

    ret = rdpa_egress_tm_num_queues_get(egress_tm, &max_queues);
    if (ret)
    {
        tmctl_error("rdpa_egress_tm_num_queues_get failed, qid[%d]",qcfg_p->qid);
        return TMCTL_ERROR;
    }

    idx = get_wrr_queue_idx(egress_tm, max_queues, 0, qcfg_p->qid);

exit:
    return idx;
}

static int prepare_set_wrr_q_in_sp_wrr_mode(bdmf_object_handle egress_tm, 
                                                tmctl_queueCfg_t *qcfg_p, 
                                                int idx, 
                                                rdpa_tm_num_sp_elem *num_sp_queues_, 
                                                uint8_t max_queues)
{
    rdpa_tm_num_sp_elem num_sp_queues;
    int new_idx;
    int ret;

    ret = num_sp_queues_after_rm(egress_tm, idx, &num_sp_queues);
    if (ret)
    {
        tmctl_error("Failed to get new number of SP queues, qid[%d]",  qcfg_p->qid);
        return TMCTL_ERROR;
    }

    new_idx = get_wrr_queue_idx(egress_tm, max_queues, num_sp_queues, qcfg_p->qid);
    if (new_idx == -1)
    {
        tmctl_error("No place for new WRR queue, qid[%d]",  qcfg_p->qid);
        return TMCTL_ERROR;
    }

    *num_sp_queues_ = num_sp_queues;
    return new_idx;
}

static int prepare_set_sp_q_in_sp_wrr_mode(bdmf_object_handle egress_tm, tmctl_queueCfg_t *qcfg_p,
        int idx, rdpa_tm_num_sp_elem *num_sp_queues_, uint8_t max_queues)
{
    rdpa_tm_queue_cfg_t queue_cfg = {0};
    rdpa_tm_num_sp_elem num_sp_queues;
    int highest_wrr_q;
    int new_idx;
    int ret;

    new_idx = get_index_by_priority(qcfg_p->priority, max_queues);
    ret = rdpa_egress_tm_queue_cfg_get(egress_tm, new_idx, &queue_cfg);
    if (ret && ret != BDMF_ERR_NOENT)
    {
        tmctl_error("rdpa_egress_tm_queue_cfg_get failed, qid[%d] index[%d]",  qcfg_p->qid, new_idx);
        return TMCTL_ERROR;
    }

    if (!ret && IS_ACTIV_Q(queue_cfg) && queue_cfg.queue_id != qcfg_p->qid)
    {
        tmctl_error("No place for new SP queue, qid[%d] index[%d], priority[%d] is taken",  
                qcfg_p->qid, new_idx, qcfg_p->priority);
        return TMCTL_ERROR;
    }  

    ret = num_sp_queues_after_add(egress_tm, new_idx, &num_sp_queues);
    if (ret)
    {
        tmctl_error("Fail to get new number of SP queues, qid[%d]", qcfg_p->qid);
        return TMCTL_ERROR;
    }

    /* check that there are no WRR queues in new SP queues area */
    highest_wrr_q = get_highest_wrr_q(egress_tm, qcfg_p->qid);
    if (highest_wrr_q < 0)
    {
        tmctl_error("Fail to get highest WRR queue, qid[%d]", qcfg_p->qid);
        return TMCTL_ERROR;
    }

    if (highest_wrr_q < num_sp_queues)
    {
        tmctl_error("No place for new SP queue, qid[%d], priority[%d] highest WRR queue[%d]", 
                  qcfg_p->qid, qcfg_p->priority, highest_wrr_q);
        return TMCTL_ERROR;
    }

    *num_sp_queues_ = num_sp_queues;
    return new_idx;
}

static int prepare_set_q_in_sp_wrr_mode(bdmf_object_handle egress_tm, tmctl_queueCfg_t *qcfg_p, int current_idx)
{
    rdpa_tm_num_sp_elem num_sp_queues;
    rdpa_tm_queue_cfg_t cur_queue_cfg = {0};
    uint8_t max_queues;
    int new_idx;
    int ret;
    
    ret = rdpa_egress_tm_num_queues_get(egress_tm, &max_queues);
    if (ret)
    {
        tmctl_error("rdpa_egress_tm_num_queues_get failed, qid[%d]",  qcfg_p->qid);
        return TMCTL_ERROR;
    }

    if (qcfg_p->schedMode == TMCTL_SCHED_WRR)
        new_idx = prepare_set_wrr_q_in_sp_wrr_mode(egress_tm, qcfg_p, current_idx, &num_sp_queues, max_queues);
    else 
        new_idx = prepare_set_sp_q_in_sp_wrr_mode(egress_tm, qcfg_p, current_idx, &num_sp_queues, max_queues); 

    if (new_idx < 0)
        return TMCTL_ERROR;

    if (current_idx >= 0)
    {
        ret = rdpa_egress_tm_queue_cfg_get(egress_tm, current_idx, &cur_queue_cfg);
        if (ret)
        {
            tmctl_error("rdpa_egress_tm_queue_cfg_get failed, qid[%d], index[%d], ret[%d]",  qcfg_p->qid, new_idx, ret);
            return TMCTL_ERROR;
        }
    }
    
    /* if changing position delete queue in current position */
    if ((current_idx >= 0 && new_idx != current_idx) || ((current_idx >= 0) &&
        ((cur_queue_cfg.weight && qcfg_p->schedMode == TMCTL_SCHED_SP) ||  
        (!cur_queue_cfg.weight && qcfg_p->schedMode == TMCTL_SCHED_WRR))))
    {
        ret = rdpa_egress_tm_queue_cfg_delete(egress_tm, current_idx);
        if (ret)
        {
            tmctl_error("rdpa_egress_tm_queue_cfg_delete failed, qid[%d], currnet index[%d], ret[%d]",qcfg_p->qid, current_idx, ret);
            return TMCTL_ERROR;
        }
    }

    ret = rdpa_egress_tm_num_sp_elements_set(egress_tm, num_sp_queues);
    if (ret)
    {
        tmctl_error("rdpa_egress_tm_num_sp_elements_set failed, qid[%d], index[%d], ret[%d]",qcfg_p->qid, new_idx, ret);
        return TMCTL_ERROR;
    }

    return new_idx;
}

static int save_current_cfg(bdmf_object_handle egress_tm, 
                            rdpa_tm_num_sp_elem *cur_num_sp_queues,
                            rdpa_tm_queue_cfg_t *cur_queue_cfg, 
                            int current_idx, 
                            rdpa_tm_sched_mode mode)
{
    int ret;

    if (mode == rdpa_tm_sched_sp_wrr)
    {
        ret = rdpa_egress_tm_num_sp_elements_get(egress_tm, cur_num_sp_queues);
        if (ret)
        {
            tmctl_error("rdpa_egress_tm_num_sp_elements_get failed,ret[%d]", ret);
            return TMCTL_ERROR;
        }
    }
    
    if (current_idx >= 0)
    {
        ret = rdpa_egress_tm_queue_cfg_get(egress_tm, current_idx, cur_queue_cfg);
        if (ret && ret != BDMF_ERR_NOENT)
        {
            tmctl_error("rdpa_egress_tm_queue_cfg_get failed, index[%d], ret[%d]", current_idx, ret);
            return TMCTL_ERROR;        }
    }
    return TMCTL_SUCCESS;
}

static int set_back_old_cfg(bdmf_object_handle egress_tm, 
                            rdpa_tm_num_sp_elem cur_num_sp_queues,
                            rdpa_tm_queue_cfg_t *cur_queue_cfg, 
                            int current_idx, 
                            rdpa_tm_sched_mode mode)
{
    int ret;

    if (mode == rdpa_tm_sched_sp_wrr)
    {
        ret = rdpa_egress_tm_num_sp_elements_set(egress_tm, cur_num_sp_queues);
        if (ret)
        {
            tmctl_error("rdpa_egress_tm_num_sp_elements_set failed");
            return TMCTL_ERROR;
        }
    }

    if (current_idx >= 0)
    {
        ret = rdpa_egress_tm_queue_cfg_set(egress_tm, current_idx, cur_queue_cfg);
        if (ret && ret != BDMF_ERR_NOENT)
        {
            tmctl_error("rdpa_egress_tm_queue_cfg_set failed index[%d]", current_idx);
            return TMCTL_ERROR;
        }
    }

    return TMCTL_SUCCESS;
}

static int check_sched_mode(int wanted_sched_mode, int allowd_sched_mode)
{
    if ((wanted_sched_mode == TMCTL_SCHED_SP &&
        allowd_sched_mode != rdpa_tm_sched_sp  && allowd_sched_mode != rdpa_tm_sched_sp_wrr) ||
        (wanted_sched_mode == TMCTL_SCHED_WRR &&
        allowd_sched_mode != rdpa_tm_sched_wrr && allowd_sched_mode != rdpa_tm_sched_sp_wrr) ||
        allowd_sched_mode == rdpa_tm_sched_disabled || 
        (wanted_sched_mode != TMCTL_SCHED_SP && wanted_sched_mode != TMCTL_SCHED_WRR))
    {
        tmctl_error("Queue sched mode %d is not supported", wanted_sched_mode);
        print_allowed_sched_mode(allowd_sched_mode);
        return TMCTL_ERROR;
    }

    return TMCTL_SUCCESS;
}

static int check_new_queue_cfg(tmctl_queueCfg_t *qcfg_p, rdpa_tm_sched_mode mode, uint8_t max_queues)
{
    if (qcfg_p->schedMode != TMCTL_SCHED_WRR && (qcfg_p->priority > max_queues || qcfg_p->priority < 0))
    {
        tmctl_error("Priority of SP queue must < %d and >= 0, qid[%d] priority[%d]",
                max_queues, qcfg_p->qid, qcfg_p->priority);
        return TMCTL_ERROR;
    }

    if ((qcfg_p->schedMode == TMCTL_SCHED_WRR || qcfg_p->schedMode == TMCTL_SCHED_WFQ) &&
            (qcfg_p->weight == 0))
    {
        tmctl_error("Weight of WRR/WFQ queue must be non-zero. qid[%d]", qcfg_p->qid);
        return TMCTL_ERROR;
    }

    if (check_sched_mode(qcfg_p->schedMode, mode))
        return TMCTL_ERROR;;

    return TMCTL_SUCCESS;
}

tmctl_ret_e tmctl_RdpaTmQueueSet(rdpa_drv_ioctl_dev_type rdpa_dev,
                          tmctl_devType_e dev_type,
                          rdpa_if if_id,
                          tmctl_queueCfg_t *qcfg_p)
{
    bdmf_object_handle egress_tm = BDMF_NULL;
    rdpa_tm_queue_cfg_t cur_queue_cfg = {0};
    rdpa_tm_num_sp_elem cur_num_sp_queues;
    rdpa_tm_queue_cfg_t queue_cfg = {0};
    rdpa_tm_sched_mode mode;
    uint8_t max_queues;
    int current_idx;
    int idx;
    int ret;

    get_tm(dev_type, if_id, &egress_tm);
    if (!egress_tm)
    {
        tmctl_error("Failed to get egress_tm, qid[%d]", qcfg_p->qid);
        goto error;
    }

    ret = rdpa_egress_tm_mode_get(egress_tm, &mode);
    if (ret)
    {
        tmctl_error("rdpa_egress_tm_mode_get failed, qid[%d], ret[%d]", qcfg_p->qid, ret);
        goto error;
    }

    ret = rdpa_egress_tm_num_queues_get(egress_tm, &max_queues);
    if (ret)
    {
        tmctl_error("rdpa_egress_tm_mode_get failed, qid[%d], ret[%d]", qcfg_p->qid, ret);
        goto error;
    }

    if (check_new_queue_cfg(qcfg_p, mode, max_queues))
        goto error;

    queue_cfg.queue_id = qcfg_p->qid;
    queue_cfg.drop_alg = rdpa_tm_drop_alg_dt;
    queue_cfg.drop_threshold = getDeviceQueueSize(qcfg_p->qsize);
#if defined(BCM_PON_XRDP) 
    queue_cfg.rl_cfg.af_rate = qcfg_p->shaper.shapingRate * 1000;
#endif
    queue_cfg.stat_enable = 1;
    /* change configured weight to 0 for SP */
    queue_cfg.weight = qcfg_p->schedMode == TMCTL_SCHED_SP ? 0 : qcfg_p->weight;

    current_idx = get_q_idx(egress_tm, qcfg_p->qid);
    if (save_current_cfg(egress_tm, &cur_num_sp_queues, &cur_queue_cfg, current_idx, mode))
        goto error;

    switch (mode)
    {
        case rdpa_tm_sched_sp:
            idx = prepare_set_q_in_sp_mode(egress_tm, qcfg_p, current_idx);
            if (idx < 0)
            {
                tmctl_error("prepare_set_q_in_sp_mode failed, qid[%d]", qcfg_p->qid);
                goto error;
            }
            break;
        case rdpa_tm_sched_wrr:
            idx = prepare_set_q_in_wrr_mode(egress_tm, qcfg_p, current_idx);
            if (idx < 0)
            {
                tmctl_error("prepare_set_q_in_wrr_mode failed, qid[%d]", qcfg_p->qid);
                goto error;
            }
            break;
        case rdpa_tm_sched_sp_wrr:
            idx = prepare_set_q_in_sp_wrr_mode(egress_tm, qcfg_p, current_idx);
            if (idx < 0)
            {
                tmctl_error("prepare_set_q_in_sp_wrr_mode failed, qid[%d]", qcfg_p->qid);
                goto error;
            }
            break;
        default:
            tmctl_error("Mode %d not supported, qid[%d]", mode, qcfg_p->qid);
            goto error;
    }

    ret = rdpa_egress_tm_queue_cfg_set(egress_tm, idx, &queue_cfg);
    if (ret)
    {
        tmctl_error("rdpa_egress_tm_queue_cfg_set failed, qid[%d], index[%d], ret[%d]", qcfg_p->qid, idx, ret);
        ret = set_back_old_cfg(egress_tm, cur_num_sp_queues, &cur_queue_cfg, current_idx, mode);
        if (ret)
            tmctl_error("Failed to set old cfg back, qid[%d], index[%d]", qcfg_p->qid, idx);
        goto error;
    }
    
    bdmf_put(egress_tm);    
    return TMCTL_SUCCESS;
error:
    if (egress_tm)
        bdmf_put(egress_tm);        
    return TMCTL_ERROR;
}

tmctl_ret_e tmctl_RdpaTmQueueDel(rdpa_drv_ioctl_dev_type rdpa_dev,
                          tmctl_devType_e dev_type,
                          rdpa_if if_id,
                          int queue_id)
{
    bdmf_object_handle egress_tm = BDMF_NULL;
    int idx;
    int ret;
          
    get_tm(dev_type, if_id, &egress_tm);
    if (!egress_tm)
    {
        tmctl_error("Failed to get egress_tm, qid[%d]", queue_id);
        goto error;
    }

    idx = get_q_idx(egress_tm, queue_id);
    if (idx >= 0)
    {
        ret = rdpa_egress_tm_queue_cfg_delete(egress_tm, idx);
        if (ret)
        {
            tmctl_error("rdpa_egress_tm_queue_cfg_delete failed, qid[%d], index[%d], ret[%d]", queue_id, idx, ret);
            goto error;
        } 
    }

    bdmf_put(egress_tm);    
    return TMCTL_SUCCESS;
error:
    if (egress_tm)
        bdmf_put(egress_tm);        
    return TMCTL_ERROR;
}

tmctl_ret_e tmctl_RdpaGetPortShaper(rdpa_drv_ioctl_dev_type rdpa_dev,
                          tmctl_devType_e dev_type,
                          rdpa_if if_id,
                          tmctl_shaper_t *shaper_p)
{
    bdmf_object_handle egress_tm = BDMF_NULL;
    rdpa_tm_rl_cfg_t rl;
    int ret;

    get_tm(dev_type, if_id, &egress_tm);
    if (!egress_tm)
    {
        tmctl_error("Failed to get egress_tm if_id[%d]", if_id);
        goto error;
    }

    ret = rdpa_egress_tm_rl_get(egress_tm, &rl);
    if (ret)
    {
        tmctl_error("rdpa_egress_tm_rl_get failed, if_id[%d]", if_id);
        goto error;
    }
   
    shaper_p->shapingRate = rl.af_rate / 1000; /* Best Effort: shaping_rate is in kbit/s: 1 kilobit = 1000 bits */
    shaper_p->shapingBurstSize = rl.burst_size;

    bdmf_put(egress_tm);    
    return TMCTL_SUCCESS;
error:
    if (egress_tm)
        bdmf_put(egress_tm);        
    return TMCTL_ERROR;
}

tmctl_ret_e tmctl_RdpaSetPortShaper(rdpa_drv_ioctl_dev_type rdpa_dev,
                          tmctl_devType_e dev_type,
                          rdpa_if if_id,
                          tmctl_shaper_t *shaper_p)
{
    bdmf_object_handle egress_tm = BDMF_NULL;
    rdpa_tm_rl_cfg_t rl = {0};
    int ret;

    rl.af_rate = shaper_p->shapingRate * 1000; /* Best Effort: shaping_rate is in kbit/s: 1 kilobit = 1000 bits */
    rl.burst_size = shaper_p->shapingBurstSize;

    get_tm(dev_type, if_id, &egress_tm);
    if (!egress_tm)
    {
        tmctl_error("Failed to get egress_tm, if_id[%d]", if_id);
        goto error;
    }

    ret = rdpa_egress_tm_rl_set(egress_tm, &rl);
    if (ret)
    {
        tmctl_error("rdpa_egress_tm_rl_set failed, if_id[%d], ret[%d]", if_id, ret);
        goto error;
    }

    bdmf_put(egress_tm);    
    return TMCTL_SUCCESS;
error:
    if (egress_tm)
        bdmf_put(egress_tm);        
    return TMCTL_ERROR;
}
    
tmctl_ret_e tmctl_RdpaSetQueueDropAlg(rdpa_drv_ioctl_dev_type rdpa_dev,
                          tmctl_devType_e dev_type,
                          rdpa_if if_id,
                          int queue_id,
                          tmctl_queueDropAlg_t *dropAlg_p)
{
    int qsize = getDefaultQueueSize(rdpa_dev, getDir(rdpa_dev, if_id));
    bdmf_object_handle egress_tm = BDMF_NULL;
    rdpa_tm_queue_cfg_t queue_cfg = {0};
    int idx;    
    int ret;

    get_tm(dev_type, if_id, &egress_tm);
    if (!egress_tm)
    {
        tmctl_error("Failed to get egress_tm, if_id[%d], qid[%d]", if_id, queue_id);
        goto error;
    }

    idx = get_q_idx(egress_tm, queue_id);
    if (idx < 0)
    {
        tmctl_error("get_q_idx failed, qid[%d] not assign to a queue", queue_id);
        goto error;
    }  

    ret = rdpa_egress_tm_queue_cfg_get(egress_tm, idx, &queue_cfg);
    if (ret)
    {
        tmctl_error("rdpa_egress_tm_queue_cfg_get failed, if_id[%d], qid[%d]", if_id, queue_id);
        goto error;
    }   

    queue_cfg.stat_enable    = 1;
    queue_cfg.drop_alg = convertDropAlg(dropAlg_p->dropAlgorithm);

    if (queue_cfg.drop_alg == rdpa_tm_drop_alg_red)
    {
        queue_cfg.low_class.min_threshold  = getDeviceQueueSize(dropAlg_p->dropAlgLo.redMinThreshold);
        queue_cfg.low_class.max_threshold  = getDeviceQueueSize(dropAlg_p->dropAlgLo.redMaxThreshold);
        queue_cfg.high_class.min_threshold = queue_cfg.drop_threshold;
        queue_cfg.high_class.max_threshold = queue_cfg.drop_threshold;
    }
    else if (queue_cfg.drop_alg == rdpa_tm_drop_alg_wred)
    {
        queue_cfg.low_class.min_threshold  = getDeviceQueueSize(dropAlg_p->dropAlgLo.redMinThreshold);
        queue_cfg.low_class.max_threshold  = getDeviceQueueSize(dropAlg_p->dropAlgLo.redMaxThreshold);
        queue_cfg.high_class.min_threshold = getDeviceQueueSize(dropAlg_p->dropAlgHi.redMinThreshold);
        queue_cfg.high_class.max_threshold = getDeviceQueueSize(dropAlg_p->dropAlgHi.redMaxThreshold);
        queue_cfg.priority_mask_0 = dropAlg_p-> priorityMask0;        
        queue_cfg.priority_mask_1 = dropAlg_p-> priorityMask1;
    }
    else /* DT */
    {
        queue_cfg.low_class.min_threshold  = 0;   
        queue_cfg.low_class.max_threshold  = 0;   
        queue_cfg.high_class.min_threshold = 0;
        queue_cfg.high_class.max_threshold = 0;
    }

    ret = rdpa_egress_tm_queue_cfg_set(egress_tm, idx, &queue_cfg);
    if (ret)
    {
        tmctl_error("rdpa_egress_tm_queue_cfg_set failed, if_id[%d], qid[%d]", if_id, queue_id);
        goto error;
    }   

    bdmf_put(egress_tm);    
    return TMCTL_SUCCESS;
error:
    if (egress_tm)
        bdmf_put(egress_tm);        
    return TMCTL_ERROR;
}

tmctl_ret_e tmctl_RdpaGetQueueStats(rdpa_drv_ioctl_dev_type rdpa_dev,
                          tmctl_devType_e dev_type,
                          rdpa_if if_id,
                          int queue_id, 
                          tmctl_queueStats_t *stats_p)
{
    bdmf_object_handle egress_tm = BDMF_NULL;
    rdpa_tm_queue_index_t q_index = {0};
    rdpa_stat_1way_t queue_stat = {0};
    int ret;
    int idx;
    
    q_index.channel = if_id - rdpa_if_wan1;
    q_index.queue_id = queue_id; 

    get_tm(dev_type, if_id, &egress_tm);
    if (!egress_tm)
    {
        tmctl_error("Failed to get egress_tm, if_id[%d], qid[%d]", if_id, queue_id);
        goto error;
    }

    idx = get_q_idx(egress_tm, queue_id);
    if (idx < 0)
    {
        tmctl_error("get_q_idx failed, qid[%d] not assign to a queue", queue_id);
        goto error;
    } 
    
    ret = rdpa_egress_tm_queue_stat_get(egress_tm, &q_index, &queue_stat);
    if (ret)
    {
        tmctl_error("rdpa_egress_tm_queue_stat_get failed, qid[%d], ret[%d]", queue_id, ret);
        goto error;
    }

    stats_p->txPackets = queue_stat.passed.packets;
    stats_p->txBytes = queue_stat.passed.bytes;
    stats_p->droppedPackets = queue_stat.discarded.packets;
    stats_p->droppedBytes = queue_stat.discarded.bytes;

    bdmf_put(egress_tm);    
    return TMCTL_SUCCESS;
error:
    if (egress_tm)
        bdmf_put(egress_tm);        
    return TMCTL_ERROR;
}

tmctl_ret_e tmctl_RdpaSetQueueSize(rdpa_drv_ioctl_dev_type rdpa_dev,        
                                   tmctl_devType_e dev_type,
                                   int if_id,
                                   int queue_id,
                                   int size)
{
    bdmf_object_handle egress_tm = BDMF_NULL;
    rdpa_tm_queue_cfg_t queue_cfg = {0};
    int idx;
    int ret;

    get_tm(dev_type, if_id, &egress_tm);
    if (!egress_tm)
    {
        tmctl_error("Failed to get egress_tm, if_id[%d], qid[%d]", if_id, queue_id);
        goto error;
    }

    idx = get_q_idx(egress_tm, queue_id);
    if (idx < 0)
    {
        tmctl_error("get_q_idx failed, qid[%d] not assign to a queue", queue_id);
        goto error;
    } 

    ret = rdpa_egress_tm_queue_cfg_get(egress_tm, queue_id, &queue_cfg);
    if (ret)
    {
        tmctl_error("rdpa_egress_tm_queue_cfg_get failed, qid[%d], ret[%d]", queue_id, ret);
        goto error;
    }

    queue_cfg.drop_threshold = getDeviceQueueSize(size);
    
    ret = rdpa_egress_tm_queue_cfg_set(egress_tm, queue_id, &queue_cfg);
    if (ret)
    {
        tmctl_error("rdpa_egress_tm_queue_cfg_set failed, qid[%d], ret[%d]", queue_id, ret);
        goto error;
    }

    bdmf_put(egress_tm);    
    return TMCTL_SUCCESS;
error:
    if (egress_tm)
        bdmf_put(egress_tm);        
    return TMCTL_ERROR;
}

tmctl_ret_e tmctl_RdpaSetQueueShaper(rdpa_drv_ioctl_dev_type rdpa_dev,        
                                   tmctl_devType_e dev_type,
                                   int if_id,
                                   int queue_id,
                                   tmctl_shaper_t *shaper_p)
{
#if defined(BCM_PON_XRDP)    
    bdmf_object_handle egress_tm = BDMF_NULL;
    rdpa_tm_queue_cfg_t queue_cfg = {0};
    rdpa_tm_rl_rate_mode rl_mode;
    int idx;
    int ret;

    get_tm(dev_type, if_id, &egress_tm);
    if (!egress_tm)
    {
        tmctl_error("Failed to get egress_tm, if_id[%d], qid[%d]", if_id, queue_id);
        goto error;
    }

    idx = get_q_idx(egress_tm, queue_id);
    if (idx < 0)
    {
        tmctl_error("get_q_idx failed, qid[%d] not assign to a queue", queue_id);
        goto error;
    } 

    ret = rdpa_egress_tm_queue_cfg_get(egress_tm, queue_id, &queue_cfg);
    if (ret)
    {
        tmctl_error("rdpa_egress_tm_queue_cfg_get failed, qid[%d], ret[%d]", queue_id, ret);
        goto error;
    }

    ret = rdpa_egress_tm_rl_rate_mode_get(egress_tm, &rl_mode);
    if (ret)
    {
        tmctl_error("rdpa_egress_tm_rl_rate_mode_get failed, qid[%d], ret[%d]", queue_id, ret);
        goto error;
    }

    if (rl_mode == rdpa_tm_rl_dual_rate)
    {
        queue_cfg.rl_cfg.be_rate = shaper_p->shapingRate * 1000;
        queue_cfg.rl_cfg.af_rate = shaper_p->minRate * 1000;
        queue_cfg.rl_cfg.burst_size = shaper_p->shapingBurstSize * 8;
    }
    else
    {
        /* Single mode, variable af_rate used as best effort rate*/
        queue_cfg.rl_cfg.af_rate = shaper_p->shapingRate * 1000;
        queue_cfg.rl_cfg.burst_size = shaper_p->shapingBurstSize;
    }

    ret = rdpa_egress_tm_queue_cfg_set(egress_tm, queue_id, &queue_cfg);
    if (ret)
    {
        tmctl_error("rdpa_egress_tm_queue_cfg_set failed, qid[%d], ret[%d]", queue_id, ret);
        goto error;
    }

    bdmf_put(egress_tm);    
    return TMCTL_SUCCESS;
error:
    if (egress_tm)
        bdmf_put(egress_tm);        
    return TMCTL_ERROR;
#else
    return TMCTL_ERROR;
#endif    
}

tmctl_ret_e tmctl_RdpaGetPortTmParms(rdpa_drv_ioctl_dev_type rdpa_dev,        
                                    tmctl_devType_e dev_type,
                                    int if_id,
                                    tmctl_portTmParms_t *tm_parms)
{
    bdmf_object_handle egress_tm = BDMF_NULL;
    rdpa_tm_sched_mode mode;
    int ret;

    get_port_tm_caps(getDir(rdpa_dev, if_id), dev_type, tm_parms);

    ret = get_tm(dev_type, if_id, &egress_tm);
    if (ret)
    {
        tmctl_error("get_tm failed, if_id[%d], dev_type[%d]", if_id, dev_type);
        goto error; 
    }
    if (!egress_tm)
    {
        tmctl_debug("No egress_tm obj, if_id[%d]", if_id);
        tm_parms->cfgFlags = 0;
        return TMCTL_SUCCESS;
    }

    ret = rdpa_egress_tm_num_queues_get(egress_tm, &tm_parms->numQueues);
    if (ret)
    {
        tmctl_error("rdpa_egress_tm_num_queues_get failed, if_id[%d], ret[%d]", if_id, ret);
        goto error; 
    }
    
    ret = rdpa_egress_tm_mode_get(egress_tm, &mode);
    if (ret)
    {
        tmctl_error("rdpa_egress_tm_mode_get failed, if_id[%d], ret[%d]", if_id, ret);
        goto error; 
    }
    
    switch (mode)
    {
        case rdpa_tm_sched_sp_wrr:
            tm_parms->cfgFlags = TMCTL_SCHED_TYPE_SP_WRR;
            break;
        case rdpa_tm_sched_sp:
            tm_parms->cfgFlags = TMCTL_SCHED_TYPE_SP;
            break;
        case rdpa_tm_sched_wrr:
            tm_parms->cfgFlags = TMCTL_SCHED_TYPE_WRR;
            break;
        default:
            tmctl_error("Mode %d is not supported", mode);
            goto error;
    }

    bdmf_put(egress_tm);    
    return TMCTL_SUCCESS;
error:
    if (egress_tm)
        bdmf_put(egress_tm);        
    return TMCTL_ERROR;
}


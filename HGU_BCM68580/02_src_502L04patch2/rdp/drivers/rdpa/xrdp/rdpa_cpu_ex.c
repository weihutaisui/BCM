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

#ifndef RDP_SIM
#include<linux/kthread.h>
#endif
#include "bdmf_dev.h"
#include "rdpa_api.h"
#include "rdpa_int.h"
#include "rdd.h"
#include "rdd_common.h"
#include "rdd_cpu_rx.h"
#include "rdpa_rdd_inline.h"
#include "rdpa_egress_tm_inline.h"
#include "rdp_cpu_ring.h"
#include "rdpa_platform.h"
#include "rdpa_cpu_ex.h"
#include "rdp_drv_qm.h"
#include "rdp_drv_fpm.h"
#include "rdd_cpu_tx.h"
#include "xrdp_drv_ubus_slv_ag.h"
#include "bdmf_sysb_chain.h"
#if (defined(CONFIG_BCM_SPDSVC) || defined(CONFIG_BCM_SPDSVC_MODULE))
#include "rdd_spdsvc.h"
#endif
#ifdef CONFIG_CPU_REDIRECT_MODE_SUPPORT
#include "rdpa_egress_tm_ex.h"
#endif
#if defined(BCM63158)
#include "rdpa_cpu_dsl_inline.h"
#endif

#define RECYCLE_RING_BUDGET 2048
#define RECYCLE_INTERRUPT_THRESHOLD 64
#define FEED_RING_BUDGET 1024
#ifndef RDP_SIM
#define DEF_DATA_RING_SIZE 	1024
#define FEED_RING_MIN_SIZE (8 * 1024)
#define FEED_RING_MID_SIZE (16 * 1024)
#define FEED_RING_MAX_SIZE (32 * 1024)

#else
#define DEF_DATA_RING_SIZE 	128
#define SIM_FEED_RING_SIZE  (4096 + 128)
#endif

#define FEED_RING_LOW_WATERMARK(feed_ring_size) (feed_ring_size - (feed_ring_size >> 2))
#define RECYCLE_RING_HIGH_WATERMARK (RECYCLE_RING_SIZE - 8192)

extern struct bdmf_object *cpu_object[rdpa_cpu_port__num_of];
/* cpu_port enum values */
bdmf_attr_enum_table_t cpu_port_enum_table = {
    .type_name = "rdpa_cpu_port",
    .values = {
        {"host",     rdpa_cpu_host},
        {"cpu1",     rdpa_cpu1},
        {"cpu2",     rdpa_cpu2},
        {"cpu3",     rdpa_cpu3},
        {"wlan0",    rdpa_cpu_wlan0},
        {"wlan1",    rdpa_cpu_wlan1},
        {"wlan2",    rdpa_cpu_wlan2},
        {NULL,      0}
    }
};

extern bdmf_attr_enum_table_t cpu_tx_method_enum_table;

static bdmf_boolean cpu_tx_disable;
struct task_struct *recycle_task_s = NULL;
struct task_struct *feed_task_s = NULL;
#ifndef RDP_SIM
static wait_queue_head_t recycle_thread_wqh;
static volatile uint32_t recycle_wakeup;

wait_queue_head_t feed_thread_wqh;
volatile uint32_t feed_wakeup;
#endif

DEFINE_BDMF_FASTLOCK(ier_lock);
DEFINE_BDMF_FASTLOCK(isr_lock);

typedef struct {
    rdpa_cpu_port cpu_obj_idx;
    uint8_t rxq_idx;
} rdd_rxq_map_t;

static rdd_rxq_map_t rdd_rxq_map[RING_ID_NUM_OF];
static int num_of_avail_data_queues;
static int feed_ring_low_watermark;

/* Init/exit module. Cater for GPL layer */
#ifdef BDMF_DRIVER_GPL_LAYER
#if defined(DSL_63138) || defined(DSL_63148) || defined(WL4908) || defined(BCM63158)
extern int (*f_rdpa_cpu_tx_port_enet_or_dsl_wan)(bdmf_sysb sysb,
    uint32_t egress_queue, rdpa_flow wan_flow, rdpa_if wan_if, rdpa_cpu_tx_extra_info_t extra_info);
#endif
#endif

static int get_feed_ring_size(uint32_t *feed_ring_size)
{
#ifndef RDP_SIM
    uint32_t bpm_avail_bufs;

    bpm_avail_bufs = gbpm_get_avail_bufs();

    if (FEED_RING_MIN_SIZE > bpm_avail_bufs)
    {
        BDMF_TRACE_RET(BDMF_ERR_NOMEM, "Not enough bpm buffers=%d for allocating minimum feed ring size=%d\n", bpm_avail_bufs, FEED_RING_MIN_SIZE);
        return -1;
    }

    if (bpm_avail_bufs > FEED_RING_MAX_SIZE)
        *feed_ring_size = FEED_RING_MAX_SIZE;
    else if (bpm_avail_bufs > FEED_RING_MID_SIZE)
        *feed_ring_size = FEED_RING_MID_SIZE;
    else
        *feed_ring_size = FEED_RING_MIN_SIZE;
#else
    *feed_ring_size = SIM_FEED_RING_SIZE;
#endif

    return BDMF_ERR_OK;
}

static void dump_list_of_queues(void)
{
    int i;

    if (bdmf_global_trace_level < bdmf_trace_level_debug)
        return;

    bdmf_trace("RDD queues map:\n");
    bdmf_trace("===============\n");
    for (i = 0; i < RING_ID_NUM_OF; i++)
    {
        bdmf_trace("RDD Q #%d, CPU object %d, Q #%d\n", i, (int)rdd_rxq_map[i].cpu_obj_idx,
            (int)rdd_rxq_map[i].rxq_idx);
    }
    bdmf_trace("\n");
}

static int rdd_rxq_map_alloc(cpu_drv_priv_t *cpu_data)
{
    int i, j;

    if (cpu_data->num_queues > num_of_avail_data_queues)
    {
        bdmf_trace("Cannot allocate requested number of queues (requested %d, available %d)\n",
            cpu_data->num_queues, num_of_avail_data_queues);
        dump_list_of_queues();
        return BDMF_ERR_NORES;
    }

    memset(cpu_data->rxq_to_rdd_rxq, (uint8_t)BDMF_INDEX_UNASSIGNED, RDPA_CPU_MAX_NUM_OF_QUEUES_PER_OBJ);
    for (i = 0, j = 0; j < cpu_data->num_queues; i++)
    {
        if (rdd_rxq_map[i].rxq_idx == (uint8_t)BDMF_INDEX_UNASSIGNED)
        {
            rdd_rxq_map[i].cpu_obj_idx = cpu_data->index;
            rdd_rxq_map[i].rxq_idx = j;
            num_of_avail_data_queues--;
            cpu_data->rxq_to_rdd_rxq[j] = i;
            j++;
        }
    }

    return 0;
}

static void rdd_rxq_map_free(cpu_drv_priv_t *cpu_data)
{
    int i;
    uint8_t rdd_rxq_idx;

    for (i = 0; i < cpu_data->num_queues; i++)
    {
        rdd_rxq_idx = cpu_data->rxq_to_rdd_rxq[i];
        rdd_rxq_map[rdd_rxq_idx].cpu_obj_idx = rdpa_cpu_port__num_of;
        rdd_rxq_map[rdd_rxq_idx].rxq_idx = (uint8_t)BDMF_INDEX_UNASSIGNED;
        cpu_data->rxq_to_rdd_rxq[i] = BDMF_INDEX_UNASSIGNED;
        num_of_avail_data_queues++;
    }
}

static void rdd_rxq_map_reset(void)
{
    int i;

    num_of_avail_data_queues = DATA_RING_ID_LAST + 1; /* Data rings */
    feed_ring_low_watermark = 0;
    for (i = 0; i < num_of_avail_data_queues; i++)
    {
        rdd_rxq_map[i].cpu_obj_idx = rdpa_cpu_port__num_of;
        rdd_rxq_map[i].rxq_idx = (uint8_t)BDMF_INDEX_UNASSIGNED;
    }
    rdd_rxq_map[FEED_RING_ID].cpu_obj_idx = rdpa_cpu_host;
    rdd_rxq_map[FEED_RING_ID].rxq_idx = FEED_RING_ID;
    rdd_rxq_map[FEED_RCYCLE_RING_ID].cpu_obj_idx = rdpa_cpu_host;
    rdd_rxq_map[FEED_RCYCLE_RING_ID].rxq_idx = FEED_RCYCLE_RING_ID;
    rdd_rxq_map[TX_RCYCLE_RING_ID].cpu_obj_idx = rdpa_cpu_host;
    rdd_rxq_map[TX_RCYCLE_RING_ID].rxq_idx = TX_RCYCLE_RING_ID;
}

void rdpa_cpu_tx_disable(bdmf_boolean en)
{
    cpu_tx_disable = en;
}

void cpu_destroy_ex(struct bdmf_object *mo)
{
    cpu_drv_priv_t *cpu_data = (cpu_drv_priv_t *)bdmf_obj_data(mo);
    int i;

    for (i = 0; i < cpu_data->num_queues; i++)
    {
        if (cpu_data->rxq_cfg[i].size)
            rdpa_cpu_int_disconnect_ex(cpu_data, i);
    }
    rdd_rxq_map_free(cpu_data);
}

rdpa_ports rdpa_ports_all_lan(void)
{
    return RDPA_PORT_ALL_LAN;
}
#ifndef BDMF_DRIVER_GPL_LAYER
EXPORT_SYMBOL(rdpa_ports_all_lan);
#endif

#ifdef RDP_SIM
#define INTERRUPT_ID_XRDP_QUEUE_0 0
#endif

static int _cpu_isr_wrapper(int irq, void *priv)
{
    cpu_drv_priv_t *cpu_data = (cpu_drv_priv_t *)priv;
    uint8_t rdd_rxq_idx = irq - INTERRUPT_ID_XRDP_QUEUE_0;
    uint8_t queue_id = rdd_rxq_map[rdd_rxq_idx].rxq_idx;
    rdpa_cpu_rxq_cfg_t *rxq_cfg = &cpu_data->rxq_cfg[queue_id];

    cpu_data->rxq_stat[queue_id].interrupts++;
    rxq_cfg->rx_isr(rxq_cfg->isr_priv);

    bdmf_int_enable(irq);

    return BDMF_IRQ_HANDLED;
}

int rdpa_cpu_int_connect_ex(cpu_drv_priv_t *cpu_data, int queue_id, uint32_t affinity_mask)
{
    uint8_t rdd_rxq_idx;
    int irq, rc;
#ifndef RDP_SIM
    int cpu_num;
    struct cpumask cpus_mask;
#endif

    rdd_rxq_idx = cpu_rdd_rxq_idx_get(cpu_data, queue_id);
    if (rdd_rxq_idx == (uint8_t)BDMF_INDEX_UNASSIGNED)
        return BDMF_ERR_INTERNAL;

    irq = INTERRUPT_ID_XRDP_QUEUE_0 + rdd_rxq_idx;

    /* Connect IRQ */
    rc = bdmf_int_connect(irq, cpu_data->cpu_id, BDMF_IRQF_DISABLED,
        _cpu_isr_wrapper, bdmf_attr_get_enum_text_hlp(&cpu_port_enum_table,
        cpu_data->index), cpu_data);

    if (!rc)
        bdmf_int_enable(irq);

    if (!affinity_mask)
        return rc;

#ifndef RDP_SIM
    cpumask_clear(&cpus_mask);
    for (cpu_num = ffs(affinity_mask) - 1; affinity_mask; affinity_mask &= ~(1L << cpu_num), cpu_num = ffs(affinity_mask))
        cpumask_set_cpu(cpu_num, &cpus_mask);
    BcmHalSetIrqAffinity(irq, &cpus_mask);
#endif

    return rc;
}

void rdpa_cpu_int_disconnect_ex(cpu_drv_priv_t *cpu_data, int queue_id)
{
    uint8_t rdd_rxq_idx = cpu_rdd_rxq_idx_get(cpu_data, queue_id);

    if (rdd_rxq_idx == (uint8_t)BDMF_INDEX_UNASSIGNED)
        return;

    bdmf_int_disconnect(INTERRUPT_ID_XRDP_QUEUE_0 + rdd_rxq_idx, cpu_data);
}

static inline void __rdpa_cpu_int_enable(uint8_t rdd_rxq_idx)
{
    uint32_t intr_en_reg;
    unsigned long flags;

    bdmf_fastlock_lock_irq(&ier_lock, flags);

    ag_drv_ubus_slv_rnr_intr_ctrl_ier_get(&intr_en_reg);
    intr_en_reg |= (1 << rdd_rxq_idx);
    ag_drv_ubus_slv_rnr_intr_ctrl_ier_set(intr_en_reg);

    bdmf_fastlock_unlock_irq(&ier_lock, flags);
}

static inline void __rdpa_cpu_int_disable(uint8_t rdd_rxq_idx)
{
    uint32_t intr_en_reg;
    unsigned long flags;

    bdmf_fastlock_lock_irq(&ier_lock, flags);

    ag_drv_ubus_slv_rnr_intr_ctrl_ier_get(&intr_en_reg);
    intr_en_reg &= ~(1 << rdd_rxq_idx);
    ag_drv_ubus_slv_rnr_intr_ctrl_ier_set(intr_en_reg);

    bdmf_fastlock_unlock_irq(&ier_lock, flags);
}

static inline void __rdpa_cpu_int_clear(uint8_t rdd_rxq_idx)
{
    uint32_t mask = (1 << rdd_rxq_idx);
    unsigned long flags;

    bdmf_fastlock_lock_irq(&isr_lock, flags);
    ag_drv_ubus_slv_rnr_intr_ctrl_isr_set(mask);
    bdmf_fastlock_unlock_irq(&isr_lock, flags);
}

#ifndef RDP_SIM

static int _rdpa_recycle_thread_handler(void *data)
{
    int feed_rcycle_cnt, tx_rcycle_cnt;

    while (1)
    {
        wait_event_interruptible(recycle_thread_wqh, recycle_wakeup || kthread_should_stop());
        if (kthread_should_stop())
        {
            BDMF_TRACE_ERR("kthread_should_stop detected in recycle\n");
            break;
        }

        feed_rcycle_cnt = rdp_cpu_ring_recycle_free_host_buf(FEED_RCYCLE_RING_ID, RECYCLE_RING_BUDGET);
        tx_rcycle_cnt = rdp_cpu_ring_recycle_free_host_buf(TX_RCYCLE_RING_ID, RECYCLE_RING_BUDGET);

        if (feed_rcycle_cnt == RECYCLE_RING_BUDGET || tx_rcycle_cnt == RECYCLE_RING_BUDGET)
        {
            /* Budget for one other rings exceeded, it's possible that more buffers remained. Reschedule */
            yield();
            continue;
        }

        /*In Order to be more burst proof we set small threshold before enabling interrupts again*/
        if (rdp_cpu_ring_get_queued(FEED_RCYCLE_RING_ID) < RECYCLE_INTERRUPT_THRESHOLD &&
            rdp_cpu_ring_get_queued(TX_RCYCLE_RING_ID) < RECYCLE_INTERRUPT_THRESHOLD)
        {
            recycle_wakeup = 0;
            __rdpa_cpu_int_enable(FEED_RCYCLE_RING_ID);
            __rdpa_cpu_int_enable(TX_RCYCLE_RING_ID);
        }
        else
        {
            /* It's possible that when we reached this moment, one of the queues is full again. Return recycling */
            yield();
        }
    }

    return 0;
}

static void _recycle_isr_wrapper(long priv)
{
    __rdpa_cpu_int_disable((int)priv);
    __rdpa_cpu_int_clear((int)priv);

    recycle_wakeup = 1;
    wake_up_interruptible(&recycle_thread_wqh);
}

static int _rdpa_feed_thread_handler(void *data)
{
    int rc;

    /*we wish the feed ring to be always full*/
    while (1)
    {
        wait_event_interruptible(feed_thread_wqh, feed_wakeup);

        rc = rdp_cpu_fill_feed_ring(FEED_RING_BUDGET);
        if (rc < FEED_RING_BUDGET)
        {
            if (rdp_cpu_ring_get_queued(FEED_RING_ID) > feed_ring_low_watermark)
            {
                feed_wakeup = 0;
                __rdpa_cpu_int_enable(FEED_RING_ID);
            }
        }
        else
        {
            schedule();
        }
    }

    return 0;
}

static void _feed_isr_wrapper(long priv)
{
    __rdpa_cpu_int_disable(FEED_RING_ID);
    __rdpa_cpu_int_clear(FEED_RING_ID);

    feed_wakeup = 1;
    wake_up_interruptible(&feed_thread_wqh);
}
#elif !defined(XRDP_EMULATION)
static void _feed_isr_wrapper(long priv)
{
    rdp_cpu_fill_feed_ring(FEED_RING_BUDGET);
}

static void _recycle_isr_wrapper(long priv)
{
    rdp_cpu_ring_recycle_free_host_buf((int)priv, RECYCLE_RING_BUDGET);
}
#endif

#ifdef XRDP
extern void (*sysb_recycle_to_feed_cb)(void *datap);
extern void rdp_recycle_buf_to_feed(void *pdata);
#endif

static int cpu_rxq_feed_and_recycle_rings_init(struct bdmf_object *mo)
{
    rdpa_cpu_rxq_cfg_t feed_rcycle_rxq_cfg = {}, tx_rcycle_rxq_cfg = {};
    rdpa_cpu_rxq_cfg_t feedq_cfg = {};
    int rc = 0;

    /* Initialize the recycle rings */
#ifndef RDP_SIM
    init_waitqueue_head(&recycle_thread_wqh);
    recycle_task_s = kthread_create(&_rdpa_recycle_thread_handler, NULL, "recycle_sysb");
#ifndef CONFIG_BCM96846
    kthread_bind(recycle_task_s,  num_online_cpus() - 1);
#endif

    wake_up_process(recycle_task_s);
#ifndef CONFIG_BCM96846
    set_bit(num_online_cpus() - 1, &feed_rcycle_rxq_cfg.irq_affinity_mask);
    set_bit(num_online_cpus() - 1, &tx_rcycle_rxq_cfg.irq_affinity_mask);
#endif
#endif

    /* For platfoms that have single recycle task, the feed recycle settings will be overwritten by tx recycle settings.
     * This should remain this way as both tasks are same except the fact that tx recycle also turns on timer for
     * coalescing. */

    /* Feed Recycle Ring */
    feed_rcycle_rxq_cfg.size = RECYCLE_RING_SIZE;
    feed_rcycle_rxq_cfg.type = rdpa_ring_recycle;
    feed_rcycle_rxq_cfg.isr_priv = FEED_RCYCLE_RING_ID;
#if !defined(XRDP_EMULATION)
    feed_rcycle_rxq_cfg.rx_isr = _recycle_isr_wrapper;
#else
    feed_rcycle_rxq_cfg.rx_isr = 0;
#endif
    rc = cpu_attr_rxq_cfg_write(mo, NULL, FEED_RCYCLE_RING_ID, (void *)&feed_rcycle_rxq_cfg,
        sizeof(feed_rcycle_rxq_cfg));
    if (rc)
    {
        BDMF_TRACE_RET(BDMF_ERR_NORES, "Recycle ring: can't create\n");
    }
#ifndef RDP_SIM
    __rdpa_cpu_int_enable(FEED_RCYCLE_RING_ID);
#endif

    /* TX Recycle Ring */
    tx_rcycle_rxq_cfg.size = RECYCLE_RING_SIZE;
    tx_rcycle_rxq_cfg.type = rdpa_ring_recycle;
    tx_rcycle_rxq_cfg.isr_priv = TX_RCYCLE_RING_ID;
#if !defined(XRDP_EMULATION)
    tx_rcycle_rxq_cfg.rx_isr = _recycle_isr_wrapper;
#else
    tx_rcycle_rxq_cfg.rx_isr = 0;
#endif

    rc = cpu_attr_rxq_cfg_write(mo, NULL, TX_RCYCLE_RING_ID, (void *)&tx_rcycle_rxq_cfg, sizeof(tx_rcycle_rxq_cfg));
    if (rc)
    {
        BDMF_TRACE_RET(BDMF_ERR_NORES, "Recycle ring: can't create\n");
    }
#ifndef RDP_SIM
    __rdpa_cpu_int_enable(TX_RCYCLE_RING_ID);
#endif

    /* Initialize FEED ring */
#ifndef RDP_SIM
    init_waitqueue_head(&feed_thread_wqh);
    feed_task_s = kthread_create(&_rdpa_feed_thread_handler, NULL, "feed_sysb");
#ifndef CONFIG_BCM96846
    kthread_bind(feed_task_s,  num_online_cpus() - 2);
#endif
    wake_up_process(feed_task_s);
#ifndef CONFIG_BCM96846
    set_bit(num_online_cpus() - 2, &feedq_cfg.irq_affinity_mask);
#else
    /*set_bit(num_online_cpus() - 2, &feedq_cfg.irq_affinity_mask);*/
#endif
#endif
    rc = get_feed_ring_size(&feedq_cfg.size);
    if (rc)
    {
        BDMF_TRACE_RET(rc, "FEED ring: can't create, no mem !\n");
    }
    feed_ring_low_watermark = FEED_RING_LOW_WATERMARK(feedq_cfg.size);
    feedq_cfg.type = rdpa_ring_feed;
    feedq_cfg.isr_priv = 0;
#if !defined(XRDP_EMULATION)
    feedq_cfg.rx_isr = _feed_isr_wrapper;
#else
    feedq_cfg.rx_isr = 0;
#endif
    rc = cpu_attr_rxq_cfg_write(mo, NULL, FEED_RING_ID, (void *)&feedq_cfg, sizeof(feedq_cfg));
    if (rc)
    {
        BDMF_TRACE_RET(BDMF_ERR_NORES, "FEED ring: can't create\n");
    }
#ifndef RDP_SIM
#if 0
    __rdpa_cpu_int_enable(FEED_RING_ID);
#endif
#ifdef XRDP
    sysb_recycle_to_feed_cb = rdp_recycle_buf_to_feed;
#endif
#endif
    return 0;
}

/* "int_connect" attribute "write" callback */
int cpu_attr_int_connect_write(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, const void *val, uint32_t size)
{
    int rc = 0;
    static int infra_rings_inited;

    if (!infra_rings_inited)
    {
        /* Configure feed and recycle rings on first CPU object that is added to the system */
        rc = cpu_rxq_feed_and_recycle_rings_init(mo);
        if (rc)
            return rc;
        infra_rings_inited = 1;
    }
    /* TODO: invoke bdmf_int_connect */
    return rc;
}

/* "int_enabled" attribute "write" callback */
int cpu_attr_int_enabled_write(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, const void *val, uint32_t size)
{
    cpu_drv_priv_t *cpu_data = (cpu_drv_priv_t *)bdmf_obj_data(mo);
    bdmf_boolean enable = *(bdmf_boolean *)val;

    if (index >= cpu_data->num_queues)
        return BDMF_ERR_INTERNAL;

    if (enable)
        rdpa_cpu_int_enable(cpu_data->index, index);
    else
        rdpa_cpu_int_disable(cpu_data->index, index);
    return 0;
}

/* "int_enabled" attribute "read" callback */
int cpu_attr_int_enabled_read(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, void *val, uint32_t size)
{
    cpu_drv_priv_t *cpu_data = (cpu_drv_priv_t *)bdmf_obj_data(mo);
    bdmf_boolean *enable = (bdmf_boolean *)val;
    uint32_t mask = 0;

    if (index >= cpu_data->num_queues)
        return BDMF_ERR_INTERNAL;

    ag_drv_ubus_slv_rnr_intr_ctrl_ier_get(&mask);
    *enable = (mask & (1 << cpu_data->rxq_to_rdd_rxq[index])) != 0;

    return 0;
}

/** Enable CPU queue interrupt
 * \param[in]   port        port. rdpa_cpu, rdpa_wlan0, rdpa_wlan1
 * \param[in]   queue       Queue index < num_queues in port tm configuration
 */
void rdpa_cpu_int_enable(rdpa_cpu_port port, int queue)
{
    cpu_drv_priv_t *cpu_data = (cpu_drv_priv_t *)bdmf_obj_data(cpu_object[port]);

    __rdpa_cpu_int_enable(cpu_data->rxq_to_rdd_rxq[queue]);
}
#ifndef BDMF_DRIVER_GPL_LAYER
EXPORT_SYMBOL(rdpa_cpu_int_enable);
#endif

/** Disable CPU queue interrupt
 * \param[in]   port        port. rdpa_cpu, rdpa_wlan0, rdpa_wlan1
 * \param[in]   queue       Queue index < num_queues in port tm configuration
 */
void rdpa_cpu_int_disable(rdpa_cpu_port port, int queue)
{
    cpu_drv_priv_t *cpu_data = (cpu_drv_priv_t *)bdmf_obj_data(cpu_object[port]);

    __rdpa_cpu_int_disable(cpu_data->rxq_to_rdd_rxq[queue]);
}
#ifndef BDMF_DRIVER_GPL_LAYER
EXPORT_SYMBOL(rdpa_cpu_int_disable);
#endif

/** Clear CPU queue interrupt
 * \param[in]   port        port. rdpa_cpu, rdpa_wlan0, rdpa_wlan1
 * \param[in]   queue       Queue index < num_queues in port tm configuration
 */
void rdpa_cpu_int_clear(rdpa_cpu_port port, int queue)
{
    cpu_drv_priv_t *cpu_data = (cpu_drv_priv_t *)bdmf_obj_data(cpu_object[port]);

    __rdpa_cpu_int_clear(cpu_data->rxq_to_rdd_rxq[queue]);
}
#ifndef BDMF_DRIVER_GPL_LAYER
EXPORT_SYMBOL(rdpa_cpu_int_clear);
#endif

void cpu_tx_free_buffer(pbuf_t *pbuf)
{
    void *buf;

    if (!pbuf->abs_flag)
    {
        /* No chaining for FPM buffers */
        drv_fpm_free_buffer(pbuf->length, pbuf->fpm_bn);
        return;
    }

    if (!bdmf_sysb_is_chained(pbuf->sysb))
    {
        bdmf_sysb_free(pbuf->sysb);
        return;
    }

    buf = pbuf->sysb;
    do
    {
        void *next = bdmf_sysb_chain_next(buf);

        bdmf_sysb_free(buf);
        buf = next;
    } while (buf);
}

/* Map wan_flow + queue_id to channel, rc_id, priority */
#define CPU_MAP_US_INFO_TO_RDD(info, priority, buf, free_func) \
    do { \
        int rc, channel, rc_id;\
        rc = _rdpa_egress_tm_wan_flow_queue_to_rdd(info->x.wan.flow, \
            info->x.wan.queue_id, (int *)&channel, (int *)&rc_id, \
            (int *)&priority);\
        if (rc)\
        {\
            ++cpu_object_data->tx_stat.tx_invalid_queue;\
            free_func(buf);\
            if (cpu_object_data->tx_dump.enable) \
            {\
                BDMF_TRACE_ERR("can't map US flow %u, queue %u to RDD. rc=%d\n", \
                    (unsigned)info->x.wan.flow, (unsigned)info->x.wan.queue_id, rc);\
            } \
            return rc;\
        } \
    } while (0)

/* Map DS channel, queue to RDD */
#define CPU_MAP_DS_INFO_TO_RDD(info, priority, buf, free_func) \
    do { \
        int rc, rc_id; \
        if (rdpa_if_is_cpu_port(info->port)) \
        { \
            priority = 0; \
            return 0; \
        } \
        rc = _rdpa_egress_tm_lan_port_queue_to_rdd(info->port, \
            info->x.lan.queue_id, (int *)&rc_id, (int *)&priority); \
        if (rc) \
        { \
            ++cpu_object_data->tx_stat.tx_invalid_queue; \
            free_func(buf); \
            BDMF_TRACE_ERR("can't map DS port/queue %u/%u P%u to RDD. rc=%d\n", \
                    (unsigned)info->port, (unsigned)info->x.lan.queue_id, priority, rc);\
            return rc; \
        } \
    } while (0)


static inline void _rdpa_cpu_sysb_flush(void *nbuff, uint8_t *data, int len)
{
#ifndef RDP_SIM
    struct sk_buff *skb;
    uint8_t *dirty, *end;

    if (IS_FKBUFF_PTR(nbuff))
        goto flush_all; /* fkb smart flush is implemented in nbuff_flush */

    skb = PNBUFF_2_SKBUFF(nbuff);
    dirty = skb_shinfo(skb)->dirty_p;

    if (!dirty || dirty < skb->head || dirty > (data + len))
        goto flush_all;

    end = (dirty > data) ? dirty : data;
    bdmf_sysb_inv_headroom_data_flush(nbuff, data, end - data);
    return;

flush_all:
    bdmf_sysb_inv_headroom_data_flush(nbuff, data, len);
#endif
}

static inline void rdpa_cpu_sysb_flush(void *nbuff, uint8_t *data, int len)
{
#ifndef CONFIG_BCM_CACHE_COHERENCY
    _rdpa_cpu_sysb_flush(nbuff, data, len);
#endif
}
static inline void rdpa_cpu_fpm_sysb_flush(void *nbuff, uint8_t *data, int len)
{
#if !defined(CONFIG_BCM_CACHE_COHERENCY) || defined(CONFIG_BCM_FPM_COHERENCY_EXCLUDE)
    _rdpa_cpu_sysb_flush(nbuff, data, len);
#endif
}

static inline int rdpa_cpu_send_pbuf(pbuf_t *pbuf, const rdpa_cpu_tx_info_t *info)
{
    bdmf_error_t rdd_rc;
    int queue = 0;
    RDD_CPU_TX_DESCRIPTOR_DTS cpu_tx_descriptor = {};
    cpu_drv_priv_t *cpu_object_data = (cpu_drv_priv_t *)bdmf_obj_data(cpu_object[info->cpu_port]);
    uint32_t pkts_sent = 0;
    cpu_tx_bn1_or_abs2_or_1588 bn1_or_abs2_or_1588 = {};
    uint8_t exclusive = 0;

    /* Dump tx data for debugging */
#ifndef XRDP_EMULATION
    CPU_CHECK_DUMP_PBUF(pbuf, info);
#endif

    if (cpu_tx_disable)
        return BDMF_ERR_STATE;

    if (pbuf->abs_flag)
    {
        pbuf->sbpm_copy = 1;
        pbuf->fpm_fallback = 0;
        rdpa_cpu_sysb_flush(pbuf->sysb, pbuf->data, pbuf->length);
    }

    switch (info->method)
    {
    case rdpa_cpu_tx_egress: /**< Egress port and priority are specified explicitly. This is the most common mode */
    {
        if (rdpa_if_is_wan(info->port))
        {
            /* upstream, egress enqueue, bpm */
            /* Map wan_flow to queue_id */
            CPU_MAP_US_INFO_TO_RDD(info, queue, pbuf, cpu_tx_free_buffer);
            cpu_tx_descriptor.wan_flow_source_port = (int)info->x.wan.flow;
            if (pbuf->abs_flag)
            {
                pbuf->sbpm_copy = 0;
            }
        }
        else
        {
            /* downstream, egress enqueue, bpm */
            CPU_MAP_DS_INFO_TO_RDD(info, queue, pbuf, cpu_tx_free_buffer);
            /* to_lan bit */
            cpu_tx_descriptor.lan = 1;
            cpu_tx_descriptor.wan_flow_source_port = rdpa_port_rdpa_if_to_vport(info->port);
            if (rdpa_if_is_lan(info->port))
                bn1_or_abs2_or_1588.lag_index = info->lag_index;
            else
                bn1_or_abs2_or_1588.ssid = info->ssid;
            if (pbuf->abs_flag)
            {
                pbuf->fpm_fallback = 1;
            }
        }
        cpu_tx_descriptor.first_level_q = queue;
        break;
    }

    case rdpa_cpu_tx_ingress: /**< before bridge forwarding decision, before classification */
    {
        if (!rdpa_if_is_wan(info->port))
        {
            /* upstream, full, bpm */
            cpu_tx_descriptor.lan = 1;
            cpu_tx_descriptor.wan_flow_source_port = rdpa_port_rx_flow_src_port_get(info->port, 0);
            bn1_or_abs2_or_1588.ssid = info->ssid;
        }
        else
        {
            /* downstream, full, bpm */
            cpu_tx_descriptor.lan = 0;
            cpu_tx_descriptor.wan_flow_source_port = info->x.wan.flow;
        }
        break;
    }

    default:
        cpu_tx_free_buffer(pbuf);
        BDMF_TRACE_RET(BDMF_ERR_NOT_SUPPORTED, "transmit method %d\n", (int)info->method);
    }

    cpu_tx_descriptor.bn1_or_abs2_or_1588 = bn1_or_abs2_or_1588.bn1_or_abs2_or_1588;

    cpu_tx_descriptor.color = info->drop_precedence;
    rdd_cpu_tx_set_packet_descriptor(info, (void *)pbuf, &cpu_tx_descriptor);
#ifdef CONFIG_BCM_PTP_1588
    if (cpu_tx_descriptor.flag_1588)
        exclusive = 1;
#endif

#if (defined(CONFIG_BCM_SPDSVC) || defined(CONFIG_BCM_SPDSVC_MODULE))
    if (info->is_spdsvc_setup_packet && !rdpa_if_is_wifi(info->port))
    {
        return rdd_spdsvc_gen_start(pbuf, info, &cpu_tx_descriptor);
    }
#endif
    rdd_rc = drv_qm_cpu_tx((uint32_t *)&cpu_tx_descriptor, info->method ? QM_QUEUE_CPU_TX_INGRESS : QM_QUEUE_CPU_TX_EGRESS, info->no_lock, exclusive);

    pkts_sent++;

    if (pbuf->abs_flag && bdmf_sysb_is_chained(pbuf->sysb) && !rdd_rc)
    {
        void *buf = pbuf->sysb;

        /* handle chain of sysb */
        while ((buf = bdmf_sysb_chain_next(buf)))
        {
            pbuf->sysb = buf;
            pbuf->data = bdmf_sysb_data(buf);
            pbuf->length = bdmf_sysb_length(buf);

            rdd_cpu_tx_set_packet_descriptor(info, (void *)pbuf, &cpu_tx_descriptor);
            rdpa_cpu_sysb_flush(pbuf->sysb, pbuf->data, pbuf->length);
            rdd_rc = drv_qm_cpu_tx((uint32_t *)&cpu_tx_descriptor, info->method ? QM_QUEUE_CPU_TX_INGRESS : QM_QUEUE_CPU_TX_EGRESS, info->no_lock, 0);
            if (rdd_rc)
                break;
            pkts_sent++;
        }
    }

#ifndef XRDP_EMULATION
    CPU_CHECK_DUMP_RDD_RC("pbuf", rdd_rc);
#endif
    if (rdd_rc)
    {
        cpu_tx_free_buffer(pbuf);
        ++cpu_object_data->tx_stat.tx_rdd_error;
        /* BDMF_TRACE_RET(BDMF_ERR_IO, "rdd error %d\n", (int)rdd_rc); */
        return BDMF_ERR_IO;
    }
#ifndef XRDP_EMULATION
    cpu_object_data->tx_stat.tx_ok += pkts_sent;
#endif
    return 0;
}

/** Send raw packet
 *
 * \param[in]   data        Packet data
 * \param[in]   length      Packet length
 * \param[in]   info        Additional transmit info
 * \return 0=OK or int error code\n
 */
int rdpa_cpu_send_raw(void *data, uint32_t length, const rdpa_cpu_tx_info_t *info)
{
    int rc;
    pbuf_t pbuf;
    const rdpa_system_cfg_t *system_cfg = _rdpa_system_cfg_get();
    fpm_pool_stat fpm_stat;
    uint16_t xon_thr, xoff_thr;

    if (length > system_cfg->mtu_size - 4)
        return BDMF_ERR_OVERFLOW;

    rc = ag_drv_fpm_pool_stat_get(&fpm_stat);
    rc = rc ? rc : ag_drv_fpm_pool1_xon_xoff_cfg_get(&xon_thr, &xoff_thr);
    if (rc || ((fpm_stat.num_of_tokens_available >= xon_thr) && (fpm_stat.num_of_tokens_available <= xoff_thr)))
        return BDMF_ERR_NORES;

    pbuf.length = length;
    pbuf.offset = info->data_offset;
    pbuf.data = data;
    pbuf.abs_flag = 0;
    pbuf.sysb = NULL;

    rc = drv_fpm_alloc_buffer(pbuf.length, &(pbuf.fpm_bn));
    if (rc)
        return rc;

    drv_fpm_copy_from_host_buffer(pbuf.data, pbuf.fpm_bn & 0xffff, pbuf.length, pbuf.offset);
    return rdpa_cpu_send_pbuf(&pbuf, info);
}
#ifndef BDMF_DRIVER_GPL_LAYER
EXPORT_SYMBOL(rdpa_cpu_send_raw);
#endif

/** Send system buffer
 *
 * \param[in] sysb System buffer. Released regardless on the function outcome
 * \param[in] info Tx info
 * \return 0=OK or int error code\n
 */

int rdpa_cpu_send_sysb(bdmf_sysb sysb, const rdpa_cpu_tx_info_t *info)
{
    pbuf_t pbuf = {};

    pbuf.length = bdmf_sysb_length(sysb);
    pbuf.offset = 0;
    pbuf.data = bdmf_sysb_data(sysb);
    pbuf.abs_flag = 1;
    pbuf.sysb = sysb;
    return rdpa_cpu_send_pbuf(&pbuf, info);
}

#ifndef BDMF_DRIVER_GPL_LAYER
EXPORT_SYMBOL(rdpa_cpu_send_sysb);
#endif

/** Send system buffer allocated from FPM
 *
 * \param[in] sysb System buffer. Released regardless on the function outcome
 * \param[in] info Tx info
 * \return 0=OK or int error code\n
 */
int rdpa_cpu_send_sysb_fpm(bdmf_sysb sysb, const rdpa_cpu_tx_info_t *info)
{
    pbuf_t pbuf = {};

    pbuf.length = bdmf_sysb_length(sysb);
    pbuf.offset = info->data_offset;
    pbuf.data = bdmf_sysb_data(sysb);
    pbuf.abs_flag = 0;
    pbuf.sysb = NULL;
    pbuf.fpm_bn = bdmf_sysb_fpm_num(sysb);

    rdpa_cpu_fpm_sysb_flush(sysb, pbuf.data, pbuf.length);

    return rdpa_cpu_send_pbuf(&pbuf, info);
}
#ifndef BDMF_DRIVER_GPL_LAYER
EXPORT_SYMBOL(rdpa_cpu_send_sysb_fpm);
#endif

/** Send chained system buffer from WFD
 *
 * \param[in] sysb System buffer. Released regardless on the function outcome
 * \param[in] info Tx info
 * \return 0=OK or int error code\n
 */
int rdpa_cpu_send_wfd_to_bridge(bdmf_sysb sysb, const rdpa_cpu_tx_info_t *_info, size_t offset_next)
{
    /* TODO: implement */
    return -1;
}
#ifndef BDMF_DRIVER_GPL_LAYER
EXPORT_SYMBOL(rdpa_cpu_send_wfd_to_bridge);
#endif


/* Send system buffer ptp - similar to rdpa_cpu_send_sysb, but treats only
 * ptp-1588 packets */
int rdpa_cpu_send_sysb_ptp(bdmf_sysb sysb, const rdpa_cpu_tx_info_t *info)
{
    /* TODO */
    return 0;
}
#ifndef BDMF_DRIVER_GPL_LAYER
EXPORT_SYMBOL(rdpa_cpu_send_sysb_ptp);
#endif

/** Send system buffer - Special function to send EPON Dying
 *  Gasp:
 *  1. reduce "if"
 *  2. use fastlock_irq
 *
 * \param[in] sysb System buffer. Released regardless on the function outcome
 * \param[in] info Tx info
 * \return 0=OK or int error code\n
 *
 * TBD: Move this function to rdpa_epon.c !
 *  */
int rdpa_cpu_send_epon_dying_gasp(bdmf_sysb sysb,
    const rdpa_cpu_tx_info_t *info)
{
    /* TODO */
    return 0;
}
#ifndef BDMF_DRIVER_GPL_LAYER
EXPORT_SYMBOL(rdpa_cpu_send_epon_dying_gasp);
#endif

static int _cpu_tc_to_rxq_set(struct bdmf_object *mo, uint8_t tc, uint8_t rxq)
{
    cpu_drv_priv_t *cpu_data = (cpu_drv_priv_t *)bdmf_obj_data(mo);

/*    bdmf_trace("%s: ============= TC %d, RQX %d (unassidned %d)\n", __FUNCTION__, tc, rxq,
        (uint8_t)BDMF_INDEX_UNASSIGNED);*/
    if ((rxq != (uint8_t)BDMF_INDEX_UNASSIGNED && !cpu_data->rxq_cfg[rxq].size) ||
        rxq == FEED_RING_ID || rxq == FEED_RCYCLE_RING_ID || rxq == TX_RCYCLE_RING_ID)
    {
        BDMF_TRACE_RET_OBJ(BDMF_ERR_PERM, mo, "Cannot set TC %d to RXQ %d: queue is not configured\n", tc, rxq);
    }

    cpu_data->tc_to_rxq[tc] = rxq;
    rdd_cpu_tc_to_rxq_set(cpu_data->index, tc,
        rxq == (uint8_t)BDMF_INDEX_UNASSIGNED ? rxq : cpu_data->rxq_to_rdd_rxq[rxq]);
    return 0;
}

int cpu_post_init_ex(struct bdmf_object *mo)
{
    cpu_drv_priv_t *cpu_data = (cpu_drv_priv_t *)bdmf_obj_data(mo);
    int i, rc;

    /* Allocate free queues from the pool */
    rc = rdd_rxq_map_alloc(cpu_data);
    if (rc)
        return rc;

    for (i = 0; i < RDPA_CPU_TC_NUM; i++)
    {
        rc = _cpu_tc_to_rxq_set(mo, i, cpu_data->tc_to_rxq[i]);
        if (rc)
            return rc;
    }
    return 0;
}

int cpu_drv_init_ex(struct bdmf_type *drv)
{
    rdd_rxq_map_reset();
#ifdef BDMF_DRIVER_GPL_LAYER
#if defined(BCM63158)
    f_rdpa_cpu_tx_port_enet_or_dsl_wan = rdpa_cpu_tx_port_enet_or_dsl_wan;
#endif
#endif
    return 0;
}

void cpu_drv_exit_ex(struct bdmf_type *drv)
{
#ifndef RDP_SIM
    if (recycle_task_s)
       kthread_stop(recycle_task_s);
    if (feed_task_s)
       kthread_stop(feed_task_s);
#endif
#ifdef BDMF_DRIVER_GPL_LAYER
#if defined(BCM63158)
    f_rdpa_cpu_tx_port_enet_or_dsl_wan = NULL;
#endif
#endif
    rdd_rxq_map_reset();
}

int cpu_attr_tc_to_rxq_read_ex(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    void *val, uint32_t size)
{
    cpu_drv_priv_t *cpu_data = (cpu_drv_priv_t *)bdmf_obj_data(mo);
    uint8_t *rxq = (uint8_t *)val;

    *rxq = cpu_data->tc_to_rxq[index];
    return 0;
}

int cpu_attr_tc_to_rxq_write_ex(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    const void *val, uint32_t size)
{
    uint8_t rxq = *(uint8_t *)val;

    _cpu_tc_to_rxq_set(mo, (uint8_t)index, rxq);
    return 0;
}

int cpu_reason_cfg_validate_ex(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    const void *val, uint32_t size)
{
    /* No special validation is required */
    return 0;
}

int cpu_reason_cfg_rdd_ex(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    const void *val, uint32_t size)
{
    rdpa_cpu_reason_index_t *rindex = (rdpa_cpu_reason_index_t *)index;
    const rdpa_cpu_reason_cfg_t *reason_cfg = (const rdpa_cpu_reason_cfg_t *)val;

    if (rindex->dir == rdpa_dir_us && reason_cfg->meter_ports)
        return cpu_per_port_reason_meter_cfg(mo, rindex, reason_cfg);
    return cpu_meter_cfg_rdd(mo, rindex, reason_cfg->meter, 0);
}

int rdpa_cpu_loopback_packet_get(rdpa_cpu_loopback_type loopback_type, bdmf_index queue, bdmf_sysb *sysb,
    rdpa_cpu_rx_info_t *info)
{
    return BDMF_ERR_NOT_SUPPORTED;
}

int cpu_attr_l4_dst_port_to_reason_add_ex(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index *index,
    const void *val, uint32_t size)
{
    return BDMF_ERR_NOT_SUPPORTED;
}

int cpu_attr_l4_dst_port_to_reason_read_ex(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index, void *val,
    uint32_t size)
{
    return BDMF_ERR_NOT_SUPPORTED;
}

int cpu_attr_l4_dst_port_to_reason_delete_ex(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index)
{
    return BDMF_ERR_NOT_SUPPORTED;
}

int cpu_attr_l4_dst_port_to_reason_find_ex(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index *index,
    void *val, uint32_t size)
{
    return BDMF_ERR_NOT_SUPPORTED;
}

#ifdef RDP_SIM
#define _DUMP_PACKET_EX_GEN_FMT "%s: SRC_PORT-%s QUEUE-%d LEGNTH-%d COLOR-%d"
#define _DUMP_PACKET_EX_SSID_FMT "SSID-%d"
#define _DUMP_PACKET_EX_REASON_FMT "Reason:%s"
#define _DUMP_PACKET_EX_METADATA_FMT "WL_METADATA-0x%x"
#define _DUMP_PACKET_EX_REDIRECT_FMT "Egress object={%s}, egress queue id=%u, %s wan_flow=%u"
#else
#define _DUMP_PACKET_EX_GEN_FMT "Rx packet on %s: port %s queue %d, %d bytes, color %d,"
#define _DUMP_PACKET_EX_SSID_FMT "CPU Vport ext / SSID %d,"
#define _DUMP_PACKET_EX_REASON_FMT "reason '%s'"
#define _DUMP_PACKET_EX_METADATA_FMT "Metadata 0x%x,"
#define _DUMP_PACKET_EX_REDIRECT_FMT "Egress object={%s}, egress queue id=%u, %s wan_flow=%u"
#endif
void _dump_packet_ex(char *name, rdpa_cpu_port port, bdmf_index queue,
    rdpa_cpu_rx_info_t *info, uint32_t dst_ssid, rdpa_cpu_rx_ext_info_t *ext_info)
{
#ifdef RDP_SIM
    bdmf_session_handle session = g_cpu_rx_file_session;
#else
    bdmf_session_handle session = NULL;
#endif
    bdmf_session_print(session, _DUMP_PACKET_EX_GEN_FMT " ",
        name, bdmf_attr_get_enum_text_hlp(&rdpa_if_enum_table, info->src_port), (int)queue, info->size, info->color);
    if (rdpa_if_is_cpu_port(info->src_port) && (info->is_exception || info->is_ucast))
        bdmf_session_print(session, _DUMP_PACKET_EX_SSID_FMT " ", info->dest_ssid);
    if (info->is_exception)
    {
        bdmf_session_print(session, _DUMP_PACKET_EX_REASON_FMT "\n",
            bdmf_attr_get_enum_text_hlp(&rdpa_cpu_reason_enum_table, info->reason));
    }
    else /* Forwarding to CPU port */
    {
        int ssid_bit;
        bdmf_session_print(session, _DUMP_PACKET_EX_METADATA_FMT " ", info->wl_metadata);
        if (!info->is_ucast)
        {
            bdmf_session_print(session, "Multicast forwarding: ");
            while (dst_ssid)
            {
                ssid_bit = ffs(dst_ssid);
                bdmf_session_print(session, "%s",
                    bdmf_attr_get_enum_text_hlp(&rdpa_wlan_ssid_enum_table,
                    ssid_bit - 1 + rdpa_wlan_ssid0));
                dst_ssid &= ~(1 << (ssid_bit - 1));
                if (dst_ssid)
                    bdmf_session_print(session, "+");
            }
        }
        bdmf_session_print(session, "\n");
    }
#ifdef CONFIG_CPU_REDIRECT_MODE_SUPPORT
    if (ext_info)
    {
        /* Dump additional data first */
        bdmf_session_print(session, _DUMP_PACKET_EX_REDIRECT_FMT "\n",
            ext_info->egress_object ? ext_info->egress_object->name : "Unknown", ext_info->egress_queue_id,
            rdpa_if_is_wan(info->src_port) ? "ingress" : "egress", ext_info->wan_flow);
    }
#endif
    bdmf_session_hexdump(session, (void *)((uint8_t *)info->data + info->data_offset), 0, info->size);
}

void _dump_packet(char *name, rdpa_cpu_port port, bdmf_index queue, rdpa_cpu_rx_info_t *info,
    uint32_t dst_ssid)
{
    _dump_packet_ex(name, port, queue, info, dst_ssid, NULL);
}

static inline int rdpa_cpu_rx_params_parse(cpu_drv_priv_t *cpu_data, rdpa_cpu_port port, bdmf_index queue, CPU_RX_PARAMS *params, rdpa_cpu_rx_info_t *info)
{
    uint32_t context = 0;
    rdpa_traffic_dir dir;

    info->reason = (rdpa_cpu_reason)params->reason;
    info->src_port = rdpa_port_vport_to_rdpa_if(params->src_bridge_port);
    if (info->reason != rdpa_cpu_rx_reason_oam && info->reason != rdpa_cpu_rx_reason_omci)
    {
        if (info->src_port == rdpa_if_none)
        {
            cpu_data->rxq_stat[queue].dropped++;
            bdmf_sysb_databuf_free(params->data_ptr, context);
            return BDMF_ERR_PERM;
        }
    }

    info->reason_data = params->flow_id;
    info->dest_ssid = params->dst_ssid;
    info->wl_metadata = params->wl_metadata;
    info->ptp_index = params->ptp_index;
    info->data = (void *)params->data_ptr;
    info->data_offset = params->data_offset;
    info->size = params->packet_size;
    info->is_exception = params->is_exception;
    info->is_rx_offload = params->is_rx_offload;
    info->is_ucast = params->is_ucast;
    info->mcast_tx_prio = params->mcast_tx_prio;
    info->color = params->color;

    dir = rdpa_if_is_wan(info->src_port) ? rdpa_dir_ds : rdpa_dir_us;
    ++cpu_data->reason_stat[dir][info->reason];

    cpu_data->rxq_stat[queue].received++;
    if (unlikely(cpu_data->rxq_cfg[queue].dump))
    	_dump_packet(cpu_object[port]->name, port, queue, info, info->dest_ssid);

    return 0;
}

#ifdef CONFIG_CPU_REDIRECT_MODE_SUPPORT
static inline int rdpa_cpu_rx_params_parse_redirected(cpu_drv_priv_t *cpu_data, rdpa_cpu_port port, bdmf_index queue,
    CPU_RX_PARAMS *params, rdpa_cpu_rx_info_t *info, rdpa_cpu_rx_ext_info_t *ext_info)
{
    int rc;
    int dump = cpu_data->rxq_cfg[queue].dump;

    /* Disable dump temporarily if redirected packet. It will be dumped in slightly
     * different format
     */
    if ((rdpa_cpu_reason)params->reason == rdpa_cpu_rx_reason_cpu_redirect)
        cpu_data->rxq_cfg[queue].dump = 0;

    rc = rdpa_cpu_rx_params_parse(cpu_data, rdpa_cpu_host, queue, params, info);
    if (rc)
        return rc;

    /* Parse extended info if trap reason is REDIRECT */
    ext_info->valid = 0;
    if (info->reason == rdpa_cpu_rx_reason_cpu_redirect)
    {
        ext_info->egress_object = NULL;
        ext_info->egress_queue_id = (uint32_t)BDMF_INDEX_UNASSIGNED;
        rc = rdpa_rdd_tx_queue_info_get(params->cpu_redirect_egress_queue, &ext_info->egress_object,
            &ext_info->egress_queue_id);
        if (rc)
            return rc;
        ext_info->wan_flow = params->cpu_redirect_wan_flow;
        ext_info->valid = 1;
        if (unlikely(dump))
        {
            _dump_packet_ex(cpu_object[port]->name, port, queue, info, info->dest_ssid, ext_info);
        }
        cpu_data->rxq_cfg[queue].dump = dump;
    }
    return rc;
}
#endif

int rdpa_cpu_packets_bulk_get(rdpa_cpu_port port, bdmf_index queue, rdpa_cpu_rx_info_t *info, int max_count, int *count)
{
    cpu_drv_priv_t *cpu_data;
    CPU_RX_PARAMS params[CPU_RX_PACKETS_BULK_SIZE] = {};
    int rc, i, parsed_count;

    if ((unsigned)port >= rdpa_cpu_port__num_of || !cpu_object[port])
        return BDMF_ERR_PARM;

    cpu_data = (cpu_drv_priv_t *)bdmf_obj_data(cpu_object[port]);

    if (_check_queue_range(cpu_data, (unsigned)queue))
        return BDMF_ERR_RANGE;

    rc = rdp_cpu_ring_read_bulk(cpu_data->rxq_to_rdd_rxq[queue], params, max_count, count);
    for (parsed_count = 0, i = 0; i < *count; i++)
    {
        if (!rdpa_cpu_rx_params_parse(cpu_data, port, queue, &params[i], info))
        {
            parsed_count++;
            info++;
        }
    }

    *count = parsed_count;
    return rc;
}
#if !defined(BDMF_DRIVER_GPL_LAYER) && !defined(RUNNER_CPU_DQM_RX)
EXPORT_SYMBOL(rdpa_cpu_packets_bulk_get);
#endif

int rdpa_cpu_packet_get(rdpa_cpu_port port, bdmf_index queue, rdpa_cpu_rx_info_t *info)
{
    cpu_drv_priv_t *cpu_data;
    CPU_RX_PARAMS params = {};
    int rc;

    if ((unsigned)port >= rdpa_cpu_port__num_of || !cpu_object[port])
        return BDMF_ERR_PARM;

    cpu_data = (cpu_drv_priv_t *)bdmf_obj_data(cpu_object[port]);

    if (_check_queue_range(cpu_data, (unsigned)queue))
        return BDMF_ERR_RANGE;

    rc = rdp_cpu_ring_read_packet_refill(cpu_data->rxq_to_rdd_rxq[queue], &params);
    if (rc)
    {
        if (rc == BDMF_ERR_NO_MORE)
            return rc;
        return BDMF_ERR_INTERNAL;
    }

    return rdpa_cpu_rx_params_parse(cpu_data, port, queue, &params, info);
}

int rdpa_cpu_packet_get_redirected(rdpa_cpu_port port, bdmf_index queue,
    rdpa_cpu_rx_info_t *info, rdpa_cpu_rx_ext_info_t *ext_info)
{
#ifdef CONFIG_CPU_REDIRECT_MODE_SUPPORT
    cpu_drv_priv_t *cpu_data;
    CPU_RX_PARAMS params = {};
    int rc;

    if (!cpu_object[rdpa_cpu_host])
        return BDMF_ERR_PARM;

    cpu_data = (cpu_drv_priv_t *)bdmf_obj_data(cpu_object[port]);

    if (_check_queue_range(cpu_data, (unsigned)queue))
        return BDMF_ERR_RANGE;

    rc = rdp_cpu_ring_read_packet_refill(cpu_data->rxq_to_rdd_rxq[queue], &params);
    if (rc)
    {
        if (rc == BDMF_ERR_NO_MORE)
            return rc;
        return BDMF_ERR_INTERNAL;
    }

    return rdpa_cpu_rx_params_parse_redirected(cpu_data, port, queue, &params, info, ext_info);
#else
    return BDMF_ERR_NOT_SUPPORTED;
#endif
}

void cpu_rxq_cfg_params_init_ex(cpu_drv_priv_t *cpu_data, rdpa_cpu_rxq_cfg_t *rxq_cfg, uint32_t *entry_size,
    uint32_t *init_write_idx)
{
    if (entry_size)
    {
        /* same entry size for data and recycle rings */
        *entry_size = rxq_cfg->type == rdpa_ring_feed ?
            sizeof(RDD_CPU_FEED_DESCRIPTOR_DTS) : sizeof(RDD_CPU_RX_DESCRIPTOR_DTS);
    }
    if (init_write_idx)
    {
        *init_write_idx = rxq_cfg->type == rdpa_ring_feed ? rxq_cfg->size - 1 : 0;
    }
}

int _check_queue_range(cpu_drv_priv_t *cpu_data, uint32_t rxq_idx)
{
    if (rxq_idx >= cpu_data->num_queues && rxq_idx != FEED_RING_ID && rxq_idx != FEED_RCYCLE_RING_ID &&
        rxq_idx != TX_RCYCLE_RING_ID)
    {
        return BDMF_ERR_PARM;
    }

    return 0;
}

void cpu_rxq_cfg_indecies_get(cpu_drv_priv_t *cpu_data, uint8_t *first_rxq_idx, uint8_t *last_rxq_idx)
{
    *first_rxq_idx = 0;
    *last_rxq_idx = cpu_data->num_queues - 1;
}

#define RDPA_CPU_MAX_WLAN_RSV_QUEUES 9 /* 3 radios, 3 queues per radio (2 for WFD, 1 for DHD) */
int cpu_rxq_cfg_max_num_set(cpu_drv_priv_t *cpu_data)
{
    if (!num_of_avail_data_queues)
        return BDMF_ERR_NORES;
    if (cpu_data->index == rdpa_cpu_host) /* Created by system object at init time */
    {
        /* We must guarantee enough queues for WLAN support */
        cpu_data->num_queues = MIN(num_of_avail_data_queues - RDPA_CPU_MAX_WLAN_RSV_QUEUES, RDPA_CPU_MAX_NUM_OF_QUEUES_PER_OBJ);
    }
    else
    {
        cpu_data->num_queues = MIN(num_of_avail_data_queues, RDPA_CPU_MAX_NUM_OF_QUEUES_PER_OBJ);
    }
    return 0;
}

uint8_t cpu_rdd_rxq_idx_get(cpu_drv_priv_t *cpu_data, bdmf_index rxq_idx)
{
    if (rxq_idx == FEED_RING_ID || rxq_idx == FEED_RCYCLE_RING_ID || rxq_idx == TX_RCYCLE_RING_ID)
        return (uint8_t)rxq_idx;
    return rxq_idx == BDMF_INDEX_UNASSIGNED ? (uint8_t)BDMF_INDEX_UNASSIGNED : cpu_data->rxq_to_rdd_rxq[rxq_idx];
}

int cpu_rxq_cfg_size_validate_ex(cpu_drv_priv_t *cpu_data, rdpa_cpu_rxq_cfg_t *rxq_cfg)
{
    /* same size for feed and recycle rings */
    int max_rxq_size = rxq_cfg->type == rdpa_ring_data ? RDPA_CPU_QUEUE_MAX_SIZE : RDPA_FEED_QUEUE_MAX_SIZE;

    /* Check te MAX queue size */
    if (rxq_cfg->size > max_rxq_size)
    {
        BDMF_TRACE_RET(BDMF_ERR_RANGE, "Queue size %u is too big\n"
            "Maximum allowed is %u\n", rxq_cfg->size, max_rxq_size);
    }

    /* Check the MIN queue size. Zero is allowed*/
    if (rxq_cfg->size && rxq_cfg->size < RDPA_CPU_QUEUE_MIN_SIZE)
    {
        BDMF_TRACE_RET(BDMF_ERR_RANGE, "Queue size %u is too small."
            " Minimum allowed is %u\n", rxq_cfg->size, RDPA_CPU_QUEUE_MIN_SIZE);
    }

    /* Check is queue size is multiple of 32 */
    if (rxq_cfg->size & 0x1F)
    {
        BDMF_TRACE_RET(BDMF_ERR_RANGE, "Queue size %u have "
            "to be multiple of 32.\n", rxq_cfg->size);
    }
    return 0;
}

void rdpa_cpu_ring_read_idx_sync(rdpa_cpu_port port, bdmf_index queue)
{
    cpu_drv_priv_t *cpu_data;
    uint32_t ring_id;

    cpu_data = (cpu_drv_priv_t *)bdmf_obj_data(cpu_object[port]);
    ring_id = cpu_data->rxq_to_rdd_rxq[queue];
    rdp_cpu_ring_read_idx_ddr_sync(ring_id);
}



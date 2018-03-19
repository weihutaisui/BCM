/*
   <:copyright-BRCM:2015:DUAL/GPL:standard
   
      Copyright (c) 2015 Broadcom 
      All Rights Reserved
   
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

/*
 *  Created on: Nov/2015
 *      Author: ido@broadcom.com
 */
#include "port.h"
#include "enet.h"
#include "runner.h"
#include "mac_drv_unimac.h"
#include <rdpa_api.h>
#include <linux/of.h>
#include <linux/nbuff.h>
#include <linux/etherdevice.h>
#include <linux/kthread.h>
#include "enet_dbg.h"
#include "runner_wifi.h"
#ifdef CONFIG_BCM_PTP_1588
#include "ptp_1588.h"
#endif

#ifdef BRCM_FTTDP
#include "g9991.h"
#endif

#if (defined(CONFIG_BCM_SPDSVC) || defined(CONFIG_BCM_SPDSVC_MODULE))
#include "linux/bcm_log.h"
#include "spdsvc_defs.h"
static bcmFun_t *enet_spdsvc_transmit;
#endif

#if defined(DSL_RUNNER_DEVICE)
#define NUM_OF_LAN_QUEUES       8
#else
#define NUM_OF_LAN_QUEUES       4
#endif
#define NUM_OF_WAN_QUEUES       8

static int enet_dump_rx;
port_ops_t port_runner_port_wan_gbe;
static int system_is_read;

/* RDPA queue configuration */

#define RDPA_CPU_QUEUE_LOW 3
#define RDPA_CPU_QUEUE_HI 4
#define RDPA_CPU_TC_LOW 0
#define RDPA_CPU_TC_HIGH 1
#if defined(CONFIG_BCM_XRDP)
#define DEFAULT_Q_SIZE 1024
#elif defined(CONFIG_BCM96848)
#define DEFAULT_Q_SIZE 64
#elif defined(DSL_RUNNER_DEVICE)
#define DEFAULT_Q_SIZE 512
#else
#define DEFAULT_Q_SIZE 256
#endif

#define _rdpa_destroy_queue(cpu_obj, channel, q_id) _rdpa_create_queue(cpu_obj, channel, q_id, 0)

#ifdef ENET_CPU_LOW_PR_Q_METER_ENABLE
#define METER_INDEX                 (0)
#define MAX_RECOMENDED_RATE         (10000)
#define MAX_RECOMENDED_BURST_SIZE   (10000)
#endif

#define BC_RATE_LIMIT_METER_INDEX    (1)
#define BC_MAX_RECOMENDED_BURST_SIZE (65535) /* not exceed 16 bit max value */
#define BC_RATE_LIMIT_DISABLE        (0xffffffff)

#ifdef ENET_DEBUG_RX_CPU_TRAFFIC
#include "bcmenet_proc.h"
#endif

extern int enetx_weight_budget;
extern void bdmf_sysb_databuf_recycle(void *pBuf, unsigned long context);
extern int chan_thread_handler(void *data);
rdpa_system_init_cfg_t init_cfg = {};

void enetxapi_queue_int_enable(enetx_channel *chan, int q_id)
{
    rdpa_cpu_int_enable(rdpa_cpu_host, chan->rx_q[q_id]);
}

void enetxapi_queue_int_disable(enetx_channel *chan, int q_id)
{
    rdpa_cpu_int_disable(rdpa_cpu_host, chan->rx_q[q_id]);
    rdpa_cpu_int_clear(rdpa_cpu_host, chan->rx_q[q_id]);
}

static inline void _rdpa_reason_set_tc_and_queue(rdpa_cpu_reason reason, uint8_t *tc, bdmf_index *queue)
{
    switch (reason)
    {
    case rdpa_cpu_rx_reason_etype_pppoe_d:
    case rdpa_cpu_rx_reason_etype_pppoe_s:
    case rdpa_cpu_rx_reason_etype_arp:
    case rdpa_cpu_rx_reason_etype_801_1ag_cfm:
    case rdpa_cpu_rx_reason_l4_icmp:
    case rdpa_cpu_rx_reason_icmpv6:
    case rdpa_cpu_rx_reason_igmp:
    case rdpa_cpu_rx_reason_dhcp:
    case rdpa_cpu_rx_reason_l4_udef_0:
        *queue = RDPA_CPU_QUEUE_HI;
        *tc = RDPA_CPU_TC_HIGH;
        break;
    default:
        *queue = RDPA_CPU_QUEUE_LOW;
        *tc = RDPA_CPU_TC_LOW;
        break;
    }
}

#ifdef XRDP
static int _rdpa_map_reasons_2_queue(bdmf_object_handle system_obj, bdmf_object_handle cpu_obj)
{
    int rc;
    rdpa_cpu_reason reason;
    uint8_t tc;
    bdmf_index queue;
    
    for (reason = rdpa_cpu_reason_min; reason < rdpa_cpu_reason__num_of; reason++)
    {
        _rdpa_reason_set_tc_and_queue(reason, &tc, &queue);
        rc = rdpa_system_cpu_reason_to_tc_set(system_obj, reason, tc);
        rc = rc ? rc : rdpa_cpu_tc_to_rxq_set(cpu_obj, tc, queue);
        if (rc < 0)
        {
            enet_err("failed to set Map TC to RXQ, error: %d, RDPA reason %d, TC %d, CPU RXQ %d\n", rc,
                (int)reason, (int)tc, (int)queue);
            return rc;
        }	
    }
    return 0;
}
#else
static int _rdpa_map_reasons_2_queue(bdmf_object_handle system_obj, bdmf_object_handle cpu_obj)
{
    int rc = 0;
    rdpa_cpu_reason_index_t reason_cfg_idx = {BDMF_INDEX_UNASSIGNED, BDMF_INDEX_UNASSIGNED};
    rdpa_cpu_reason_cfg_t reason_cfg = {};
    uint8_t tc;
        
    reason_cfg.meter = BDMF_INDEX_UNASSIGNED;
    while (!rdpa_cpu_reason_cfg_get_next(cpu_obj, &reason_cfg_idx))
    {
        _rdpa_reason_set_tc_and_queue(reason_cfg_idx.reason, &tc, &reason_cfg.queue);
#if defined(DSL_RUNNER_DEVICE)
        /* sf2 based runner has LAN, WAN tables */
        reason_cfg_idx.table_index = (reason_cfg_idx.dir == rdpa_dir_us)?
                CPU_REASON_LAN_TABLE_INDEX : CPU_REASON_WAN1_TABLE_INDEX;
#endif
        rc = rdpa_cpu_reason_cfg_set(cpu_obj, &reason_cfg_idx, &reason_cfg);
        if (rc < 0)
        {
            enet_err("failed to set RDPA reason %d\n", rc);
            return rc;
        }
    }
    return 0;
}
#endif

#if defined(DSL_RUNNER_DEVICE) && !defined(CONFIG_BCM963158)
static int _rdpa_cfg_cpu_l4_dst_port_to_reason_init(bdmf_object_handle cpu_obj)
{
    rdpa_l4_dst_port_to_reason_cfg_t entry;
    int rc;
    bdmf_index idx;

    // IPv4 DHCP (port 67, 68), IPv6 DHCP (port 546, 547), DNS (TCP/UDP port 53)
    entry.is_static = 1;
    entry.is_tcp = 0;
    entry.reason = rdpa_cpu_rx_reason_dhcp;

    entry.l4_dst_port = 67;
    rc = rdpa_cpu_l4_dst_port_to_reason_add(cpu_obj, &idx, &entry);
    entry.l4_dst_port = 68;
    rc |= rdpa_cpu_l4_dst_port_to_reason_add(cpu_obj, &idx, &entry);
    entry.l4_dst_port = 546;
    rc |= rdpa_cpu_l4_dst_port_to_reason_add(cpu_obj, &idx, &entry);
    entry.l4_dst_port = 547;
    rc |= rdpa_cpu_l4_dst_port_to_reason_add(cpu_obj, &idx, &entry);

    // DNS
    entry.reason = rdpa_cpu_rx_reason_l4_udef_0;
    entry.l4_dst_port = 53;
    rc |= rdpa_cpu_l4_dst_port_to_reason_add(cpu_obj, &idx, &entry);
    entry.is_tcp = 1;
    rc |= rdpa_cpu_l4_dst_port_to_reason_add(cpu_obj, &idx, &entry);

    // HTTP 80, 8080
    entry.l4_dst_port = 80;
    rc |= rdpa_cpu_l4_dst_port_to_reason_add(cpu_obj, &idx, &entry);
    entry.l4_dst_port = 8080;
    rc |= rdpa_cpu_l4_dst_port_to_reason_add(cpu_obj, &idx, &entry);
    if (rc < 0)
    {
        enet_err(": Error (%d) configuraing CPU l4_dst_port_to_reason \n", rc );
        return rc;
    }
    return 0;
}
#endif

static void rdpa_rx_isr(long priv)
{
    enetx_rx_isr((enetx_channel *)priv);
}

static void rdpa_rx_dump_data_cb(bdmf_index queue, bdmf_boolean enabled)
{
    enet_dump_rx = enabled;
}

#ifdef ENET_CPU_LOW_PR_Q_METER_ENABLE
static int configure_cpu_low_prq_meter(bdmf_object_handle rdpa_cpu_obj)
{
    rdpa_dir_index_t dir_idx;
    rdpa_cpu_meter_cfg_t meter_cfg;
    rdpa_cpu_reason_index_t reason_cfg_idx = {BDMF_INDEX_UNASSIGNED, BDMF_INDEX_UNASSIGNED};
    rdpa_cpu_reason_cfg_t reason_cfg = {};
    int rc = 0;

    dir_idx.dir = rdpa_dir_us;
    dir_idx.index = METER_INDEX;
    
    meter_cfg.sir = MAX_RECOMENDED_RATE;
    meter_cfg.burst_size = MAX_RECOMENDED_BURST_SIZE;
        
    rc = rdpa_cpu_meter_cfg_set(rdpa_cpu_obj, &dir_idx , &meter_cfg);
    if (rc < 0)
    {
        enet_err("Error(%d) Meter CFG Set\n", rc);
        goto exit;
    }

    reason_cfg_idx.dir = rdpa_dir_us;
    reason_cfg_idx.reason = rdpa_cpu_rx_reason_ip_flow_miss;
    
    reason_cfg.meter_ports = rdpa_ports_all_lan();
    reason_cfg.meter = METER_INDEX;
    reason_cfg.queue = RDPA_CPU_QUEUE_LOW;

    rc = rdpa_cpu_reason_cfg_set(rdpa_cpu_obj, &reason_cfg_idx, &reason_cfg);
    if (rc < 0)
        enet_err("Error(%d) configuring CPU reason to meter\n", rc);

exit:
    return rc;

}

static int unconfigure_cpu_low_prq_meter(bdmf_object_handle rdpa_cpu_obj)
{
    rdpa_cpu_reason_index_t reason_cfg_idx = {BDMF_INDEX_UNASSIGNED, BDMF_INDEX_UNASSIGNED};
    rdpa_cpu_reason_cfg_t reason_cfg = {};
    int rc = 0;

    reason_cfg_idx.dir = rdpa_dir_us;
    reason_cfg_idx.reason = BDMF_INDEX_UNASSIGNED; // disconnect meter from reason
    
    reason_cfg.meter_ports = rdpa_ports_all_lan();
    reason_cfg.meter = METER_INDEX;
    reason_cfg.queue = RDPA_CPU_QUEUE_LOW;

    rc = rdpa_cpu_reason_cfg_set(rdpa_cpu_obj, &reason_cfg_idx, &reason_cfg);
    if (rc < 0)
        enet_err("Error(%d) configuring CPU reason to meter\n", rc);

    return rc;
}

#endif

int configure_bc_rate_limit_meter(int port_id, unsigned int rate_limit)
{
    bdmf_object_handle rdpa_cpu_obj = NULL;
    rdpa_dir_index_t dir_idx;
    rdpa_cpu_meter_cfg_t meter_cfg;
    rdpa_cpu_reason_index_t reason_cfg_idx = {BDMF_INDEX_UNASSIGNED, BDMF_INDEX_UNASSIGNED};
    rdpa_cpu_reason_cfg_t reason_cfg = {};
    int rc = 0;

    if ((rc = rdpa_cpu_get(rdpa_cpu_host, &rdpa_cpu_obj)) < 0)
        goto exit;

    dir_idx.dir = rdpa_dir_us;
    dir_idx.index = BC_RATE_LIMIT_METER_INDEX;
    if (BC_RATE_LIMIT_DISABLE != rate_limit)
    {
        meter_cfg.sir = rate_limit;
        meter_cfg.burst_size = BC_MAX_RECOMENDED_BURST_SIZE;
        if ((rc = rdpa_cpu_meter_cfg_set(rdpa_cpu_obj, &dir_idx, &meter_cfg)) < 0)
        {
            enet_err("Error Meter CFG Set!\n");
            goto exit;
        }
    }

    reason_cfg_idx.dir = rdpa_dir_us;
    reason_cfg_idx.reason = rdpa_cpu_rx_reason_bcast;
    reason_cfg_idx.entry_index = BC_RATE_LIMIT_METER_INDEX;
#if defined(CONFIG_BCM963138) || defined(CONFIG_BCM963148) || defined(CONFIG_BCM94908)
    reason_cfg_idx.table_index = (reason_cfg_idx.dir == rdpa_dir_us)
        ? CPU_REASON_LAN_TABLE_INDEX : CPU_REASON_WAN1_TABLE_INDEX;
#endif


    if ((rc = rdpa_cpu_reason_cfg_get(rdpa_cpu_obj, &reason_cfg_idx, &reason_cfg)) < 0)
        goto exit;

    if (BC_RATE_LIMIT_DISABLE == rate_limit)
    {
        reason_cfg.meter_ports &= ~(rdpa_if_id(port_id));
    }
    else
    {
        reason_cfg.meter_ports |= rdpa_if_id(port_id);
    }
    reason_cfg.meter = BC_RATE_LIMIT_METER_INDEX;
    reason_cfg.queue = RDPA_CPU_QUEUE_LOW;

    if ((rc = rdpa_cpu_reason_cfg_set(rdpa_cpu_obj, &reason_cfg_idx, &reason_cfg)) < 0)
    {
        enet_err("Error configuring CPU reason to meter!\n");
        goto exit;
    }
exit:
    if (rdpa_cpu_obj)
        bdmf_put(rdpa_cpu_obj);

    return rc;
}

static int _rdpa_create_queue(bdmf_object_handle cpu_obj, enetx_channel *chan, int q_id, int size)
{
    rdpa_cpu_rxq_cfg_t rxq_cfg;
    int rc = 0;

    /* Make sure interrupt will not be called during RX queue configuration since interfaces might not be up yet */
    enetxapi_queue_int_disable(chan, q_id);

    bdmf_lock();
    rdpa_cpu_rxq_cfg_get(cpu_obj, chan->rx_q[q_id], &rxq_cfg);
    rc = runner_ring_create_delete(chan, q_id, size, &rxq_cfg);
    if (rc)
        goto exit;

    rxq_cfg.size = size;
    rxq_cfg.isr_priv = size ? (long)chan : 0; 
    rxq_cfg.rx_isr = size ? rdpa_rx_isr : NULL;
#ifdef ENET_INT_COALESCING_ENABLE
    rxq_cfg.ic_cfg.ic_enable = 1;
    rxq_cfg.ic_cfg.ic_timeout_us = ENET_INTERRUPT_COALESCING_TIMEOUT_US;
    rxq_cfg.ic_cfg.ic_max_pktcnt = ENET_INTERRUPT_COALESCING_MAX_PKT_CNT;
#endif
    rxq_cfg.rx_dump_data_cb = rdpa_rx_dump_data_cb;
    /* XXX: Must make sure rdd_ring is destroyed before removing rdp ring */
    rc = rc ? rc : rdpa_cpu_rxq_cfg_set(cpu_obj, chan->rx_q[q_id], &rxq_cfg);

exit:
    bdmf_unlock();
    if (rc < 0)
        enet_err("failed to configure RDPA CPU RX q_id - %d chan->rx_q[q_id] - %d \n", q_id, chan->rx_q[q_id]);

    return rc;
}

static enetx_channel *_create_rdpa_queues(void)
{
    enetx_channel *chan;

    chan = kzalloc(sizeof(enetx_channel), GFP_KERNEL);
    if (!chan)
        return NULL;

#ifndef ONE_QUEUE_PER_CHANNEL
    chan->rx_q_count = 2;
    chan->rx_q[0] = RDPA_CPU_QUEUE_HI;
    chan->rx_q[1] = RDPA_CPU_QUEUE_LOW;
#else
    chan->next = kzalloc(sizeof(enetx_channel), GFP_KERNEL);
    if (!chan->next)
        return NULL;

    chan->rx_q_count = 1;
    chan->rx_q[0] = RDPA_CPU_QUEUE_HI;
    chan->next->rx_q_count = 1;
    chan->next->rx_q[0] = RDPA_CPU_QUEUE_LOW;
#endif

    return chan;
}

/* This implementation configures 2 queues (HI/LOW) on 1 channel */
int enetxapi_queues_init(enetx_channel **_chan)
{
    bdmf_object_handle cpu_obj = NULL;
    bdmf_object_handle system_obj = NULL;
    enetx_channel *chan, *next;
    int i, rc = 0;

    *_chan = chan = _create_rdpa_queues();
    if (!chan)
        goto exit;

    rc = rdpa_system_get(&system_obj);
    rc = rc ? rc : rdpa_cpu_get(rdpa_cpu_host, &cpu_obj);

    init_waitqueue_head(&chan->rxq_wqh);
    chan->rx_thread = kthread_run(chan_thread_handler, chan, "bcmsw_rx");

    while (chan)
    {
        next = chan->next;

        for (i = 0; i < chan->rx_q_count; i++)
            rc = rc ? : _rdpa_create_queue(cpu_obj, chan, i, DEFAULT_Q_SIZE);

        chan = next;
    }
    
#if defined(DSL_RUNNER_DEVICE)
    /* for DSL platform, in addition to LAN, XTM also needs resource so set NAPI budget as 32 like enet impl5 */
    enetx_weight_budget = 32;
#else
    enetx_weight_budget = DEFAULT_Q_SIZE / 2;
#endif
    rc = rc ? : _rdpa_map_reasons_2_queue(system_obj, cpu_obj);

#if defined(DSL_RUNNER_DEVICE) && !defined(CONFIG_BCM963158)
    /* FIXME!! Wen.. skip 63158 for now, this feature is yet to be implemented */
    rc = rc ? : _rdpa_cfg_cpu_l4_dst_port_to_reason_init(cpu_obj);
#endif

#ifdef ENET_CPU_LOW_PR_Q_METER_ENABLE 
    if (!rc) 
        rc = configure_cpu_low_prq_meter(cpu_obj);

#endif

exit:

    if (!rc)
    {
        enet_dbg("configured RDPA CPU queues\n");
    }
    else
    {
        enet_err("failed to configure RDPA CPU queues\n");
        enetxapi_queues_uninit(_chan);
    }

    if (cpu_obj)
        bdmf_put(cpu_obj);
    if (system_obj)
        bdmf_put(system_obj);

    return rc;
}

int enetxapi_queues_uninit(enetx_channel **_chan)
{
    bdmf_object_handle cpu_obj = NULL;
    int rc, i;
    enetx_channel *next, *chan = *_chan;

    rc = rdpa_cpu_get(rdpa_cpu_host, &cpu_obj);
    if (rc)
        goto exit;

    while (chan)
    {

        if (chan->rx_thread)
        {
                kthread_stop(chan->rx_thread);
                chan->rx_thread = NULL;
        }

        next = chan->next;

        for (i = 0; i < chan->rx_q_count; i++)
            rc |= _rdpa_destroy_queue(cpu_obj, chan, i);

        

        kfree(chan);
        chan = next;
    }


#ifdef ENET_CPU_LOW_PR_Q_METER_ENABLE 
    if (!rc) 
        rc = unconfigure_cpu_low_prq_meter(cpu_obj);

#endif

exit:
    if (rc)
        enet_err("failed to remove RDPA CPU queues\n");
    else
        enet_dbg("removed RDPA CPU queues\n");

    if (cpu_obj)
        bdmf_put(cpu_obj);

    *_chan = NULL;

    return rc;
}

int enetxapi_rx_pkt(int hw_q_id, FkBuff_t **fkb, enetx_rx_info_t *rx_info)
{
    rdpa_cpu_rx_info_t info = {};
    int rc;

    rc = runner_get_pkt_from_ring(hw_q_id, &info);
    if (unlikely(rc))
    {
        if (unlikely(rc != BDMF_ERR_NO_MORE))
        {
            enet_dbg_rx("enetxapi_rx_pkt error: rc %d\n", rc);
            return rc;
        }

        return -1;
    }
#ifdef ENET_DEBUG_RX_CPU_TRAFFIC
    if (g_debug_mode) 
    {
        *fkb = (FkBuff_t *)info.data;
        
        return 0;
    }
#endif


#if defined(CONFIG_BCM_PTP_1588) && defined(XRDP)
    /* In 1588 packets, TS(32 bit) is appended to the begining of the packet by the XRDP,
     * so the data_offset pointer should be forwarded by 32bit and packet length decreased by 32bit */
    if (unlikely(info.reason == rdpa_cpu_rx_reason_etype_ptp_1588))
    {
        info.data_offset += PTP_TS_LEN;
        info.size -= PTP_TS_LEN;
        info.ptp_index = 1;
    }
#endif

    *fkb = fkb_init((uint8_t *)info.data , BCM_PKT_HEADROOM, (uint8_t *)(info.data + info.data_offset), info.size);
    (*fkb)->recycle_hook = (RecycleFuncP)bdmf_sysb_recycle;

    /* Parameters required by enet */
    rx_info->src_port = info.src_port;
    rx_info->flow_id = info.reason_data;
    rx_info->ptp_index = info.ptp_index;
    rx_info->data_offset = info.data_offset;
    rx_info->reason = info.reason;
    rx_info->extra_skb_flags = 0;
#if defined(CONFIG_BCM_RUNNER_FLOODING)
    if (unlikely(info.reason == rdpa_cpu_rx_reason_unknown_da))
        rx_info->extra_skb_flags |= SKB_RNR_FLOOD;
#endif

    if (unlikely(enet_dump_rx))
        rdpa_cpu_rx_dump_packet("enet", rdpa_cpu_host, hw_q_id, &info, 0);

    return 0;
}

void enetxapi_fkb_databuf_recycle(FkBuff_t *fkb, void *context)
{
    bdmf_sysb_databuf_recycle(fkb, (unsigned long)context);
}

void enetxapi_buf_recycle(struct sk_buff *skb, uint32_t context, uint32_t flags)
{
    bdmf_sysb_recycle(skb, context, flags);
}

static inline int _rdpa_cpu_send_sysb(bdmf_sysb sysb, rdpa_cpu_tx_info_t *info)
{
#if (defined(CONFIG_BCM_SPDSVC) || defined(CONFIG_BCM_SPDSVC_MODULE))
    spdsvcHook_transmit_t spdsvc_transmit;
#endif
    int rc;

#ifdef BRCM_FTTDP
    /* FTTDP FW does not support sending from sysb, so we need to copy to bpm */
    rc = rdpa_cpu_send_raw(bdmf_sysb_data(sysb), bdmf_sysb_length(sysb), info);
    /* rdpa_cpu_send_raw copies to bpm but does not free buffer */
    nbuff_flushfree(sysb);
#else
#if (defined(CONFIG_BCM_SPDSVC) || defined(CONFIG_BCM_SPDSVC_MODULE))
    spdsvc_transmit.pNBuff = sysb;
    spdsvc_transmit.dev = NULL;
    spdsvc_transmit.header_type = SPDSVC_HEADER_TYPE_ETH;
    spdsvc_transmit.phy_overhead = BCM_ENET_OVERHEAD;
    info->is_spdsvc_setup_packet = enet_spdsvc_transmit(&spdsvc_transmit);
    if (info->is_spdsvc_setup_packet < 0)
    {
        /* In case of error, NBuff will be free by spdsvc */
        return -1;
    }
#endif
    rc = rdpa_cpu_send_sysb(sysb, info);
#endif

    return rc;
}

#if !defined(DSL_RUNNER_DEVICE)
int port_runner_dispatch_pkt_lan(dispatch_info_t *dispatch_info)
{
    rdpa_cpu_tx_info_t info = {};
#ifdef CONFIG_BCM_PTP_1588
    char *ptp_offset;
#endif

    info.method = rdpa_cpu_tx_port;
    info.port = dispatch_info->port->p.port_id;
    info.cpu_port = rdpa_cpu_host;
    info.x.lan.queue_id = dispatch_info->egress_queue;
    info.drop_precedence = dispatch_info->drop_eligible;

    enet_dbg_tx("rdpa_cpu_send: port %d queue %d\n", info.port, dispatch_info->egress_queue);

#ifdef CONFIG_BCM_PTP_1588
    if (unlikely(is_pkt_ptp_1588(dispatch_info->pNBuff, &info, &ptp_offset)))
        return ptp_1588_cpu_send_sysb((bdmf_sysb)dispatch_info->pNBuff, &info, ptp_offset);
    else
#endif
    return _rdpa_cpu_send_sysb((bdmf_sysb)dispatch_info->pNBuff, &info);
}

static int dispatch_pkt_gbe_wan(dispatch_info_t *dispatch_info)
{
    rdpa_cpu_tx_info_t info = {};

    info.method = rdpa_cpu_tx_port;
    info.port = dispatch_info->port->p.port_id;
    info.cpu_port = rdpa_cpu_host;
    info.x.wan.queue_id = dispatch_info->egress_queue;
    info.drop_precedence = dispatch_info->drop_eligible;

    enet_dbg_tx("rdpa_cpu_send: port %d queue %d\n", info.port, dispatch_info->egress_queue);

    return _rdpa_cpu_send_sysb((bdmf_sysb)dispatch_info->pNBuff, &info);
}
#endif

#ifdef XRDP
#define QUEUE_THRESHOLD 128*1536 /* Drop threshold in bytes */
#else
#define QUEUE_THRESHOLD 128 /* TODO: platform define */
#endif

static int rdpa_egress_tm_queues_cfg(bdmf_object_handle tm_obj, int q_count)
{
    bdmf_error_t rc = 0;
    rdpa_tm_queue_cfg_t queue_cfg = {};
    int qid;

    queue_cfg.drop_alg = rdpa_tm_drop_alg_dt;
    queue_cfg.drop_threshold = QUEUE_THRESHOLD;

    for (qid = 0; qid < q_count; qid++)
    {
        queue_cfg.queue_id = qid;
        if ((rc = rdpa_egress_tm_queue_cfg_set(tm_obj, qid, &queue_cfg)))
        {
            enet_err("Failed to configure RDPA egress tm queue %d. rc=%d\n", qid, rc);
            break;
        }
    }

    return rc;
}

static int create_rdpa_egress_tm(bdmf_object_handle port_obj)
{
    bdmf_error_t rc;
    BDMF_MATTR(tm_attr, rdpa_egress_tm_drv());
    bdmf_object_handle tm_obj = NULL;
    rdpa_port_tm_cfg_t port_tm_cfg;
    rdpa_if rdpaif;
    int is_wan;

    if ((rc = rdpa_port_index_get(port_obj, &rdpaif)))
    {
        enet_err("Failed to get RDPA port index. rc=%d\n", rc);
        goto Exit;
    }

    is_wan = rdpa_if_is_wan(rdpaif);

    rdpa_egress_tm_dir_set(tm_attr, is_wan ? rdpa_dir_us : rdpa_dir_ds);
    rdpa_egress_tm_level_set(tm_attr, rdpa_tm_level_queue);
    rdpa_egress_tm_mode_set(tm_attr, rdpa_tm_sched_sp);

    if ((rc = bdmf_new_and_set(rdpa_egress_tm_drv(), port_obj, tm_attr, &tm_obj)))
    {
        enet_err("Failed to create RDPA egress tm for port %d. rc=%d\n", rdpaif, rc);
        goto Exit;
    }

    if ((rc = rdpa_egress_tm_queues_cfg(tm_obj, is_wan ? NUM_OF_WAN_QUEUES : NUM_OF_LAN_QUEUES))) 
    {
        enet_err("Failed to configure RDPA egress tm queues for port %d. rc=%d\n", rdpaif, rc);
        goto Exit;
    }

    if ((rc = rdpa_port_tm_cfg_get(port_obj, &port_tm_cfg)))
    {
        enet_err("Failed to get RDPA egress tm for port %d. rc=%d\n", rdpaif, rc);
        goto Exit;
    }

    port_tm_cfg.sched = tm_obj;

    if ((rc = rdpa_port_tm_cfg_set(port_obj, &port_tm_cfg)))
    {
        enet_err("Failed to set RDPA egress tm for port %d. rc=%d\n", rdpaif, rc);
        goto Exit;
    }

    enet_dbg("Created RDPA egress tm %s\n", bdmf_object_name(tm_obj));

Exit:
    if (rc && tm_obj)
        bdmf_destroy(tm_obj);

    return rc;
}

bdmf_object_handle create_rdpa_port(rdpa_if rdpaif, rdpa_emac emac, bdmf_object_handle owner)
{
    bdmf_error_t rc;
    BDMF_MATTR(rdpa_port_attrs, rdpa_port_drv());
    bdmf_object_handle cpu_obj = NULL;
    bdmf_object_handle port_obj = NULL;
    rdpa_port_dp_cfg_t port_cfg = {};

    enet_dbgv("(if=%x, emac=%x, owner=%p)\n", rdpaif, emac, (void *)owner);
    if ((rc = rdpa_port_index_set(rdpa_port_attrs, rdpaif)))
    {
        enet_err("Failed to set RDPA port index %d. rc=%d\n", rdpaif, rc);
        return NULL;
    }

    if (emac != rdpa_emac_none)
    {
    port_cfg.emac = emac;

    if (rdpa_if_is_wan(rdpaif))
    {
        rdpa_port_wan_type_set(rdpa_port_attrs, rdpa_wan_gbe);
    }

    if (rdpaif != rdpa_if_switch)
    {
#ifndef BRCM_FTTDP
        port_cfg.sal_enable = 1;
        port_cfg.dal_enable = 1;
        port_cfg.sal_miss_action = rdpa_forward_action_host;
        port_cfg.dal_miss_action = rdpa_forward_action_host;
#endif
    }
#ifdef BRCM_FTTDP
    if (rdpa_if_is_lag_and_switch(rdpaif))
        port_cfg.control_sid = rdpa_if_lan0 + G9991_MGMT_SID;
#endif

    if ((rc = rdpa_port_cfg_set(rdpa_port_attrs, &port_cfg)))
    {
        enet_err("Failed to set configuration for RDPA port %d. rc=%d\n", rdpaif, rc);
        goto Exit;
    }
    }

    if ((rc = bdmf_new_and_set(rdpa_port_drv(), owner, rdpa_port_attrs, &port_obj)))
    {
        enet_err("Failed to create RDPA port %d. rc=%d\n", rdpaif, rc);
        goto Exit;
    }

    if (!rdpa_if_is_lag_and_switch(rdpaif) && (rc = create_rdpa_egress_tm(port_obj)))
    {
        enet_err("Failed to create ergress_tm for RDPA port %d. rc=%d\n", rdpaif, rc);
        goto Exit;
    }

#ifdef XRDP
    rc = rdpa_cpu_get(rdpa_cpu_host, &cpu_obj);
    rc = rc ? rc : rdpa_port_cpu_obj_set(port_obj, cpu_obj);
    if (rc)
    {
        enet_err("Failed to set CPU object for port %s, error %d\n", bdmf_object_name(port_obj), rc);
        goto Exit;
    }
#else
    rc = 0;
#endif

    enet_dbg("Created RDPA port: %s\n", bdmf_object_name(port_obj));

Exit:
    if (cpu_obj)
        bdmf_put(cpu_obj);
    if (rc && port_obj)
    {
        bdmf_destroy(port_obj);
        port_obj = NULL;
    }

    return port_obj;
}

int link_switch_to_rdpa_port(bdmf_object_handle port_obj)
{
    bdmf_error_t rc = 0;
    bdmf_object_handle switch_port_obj = NULL;

    if ((rc = rdpa_port_get(rdpa_if_switch, &switch_port_obj)))
    {
        enet_err("Failed to get RDPA switch port object. rc=%d\n", rc);
        goto Exit;
    }

    if ((rc = bdmf_link(switch_port_obj, port_obj, NULL)))
    {
        enet_err("Failed to link RDPA port to switch. rc=%d\n", rc);
        goto Exit;
    }

Exit:
    if (switch_port_obj)
        bdmf_put(switch_port_obj);

    return rc;
}

#if defined(DSL_RUNNER_DEVICE)

static int _demux_id_runner_port(enetx_port_t *self)
{
    return self->p.port_id; // rdpa_if
}

#else // !DSL_RUNNER_DEVICE
#if !defined(CONFIG_BCM963158)
int link_pbit_tc_to_q_to_rdpa_lan_port(bdmf_object_handle port_obj)
{
    bdmf_error_t rc = 0;
    bdmf_object_handle rdpa_tc_to_queue_obj = NULL;
    bdmf_object_handle rdpa_pbit_to_queue_obj = NULL;
    rdpa_tc_to_queue_key_t t2q_key;

    t2q_key.dir = rdpa_dir_ds; 
    t2q_key.table = 0; 

    if ((rc = rdpa_tc_to_queue_get(&t2q_key, &rdpa_tc_to_queue_obj)))
    {
        enet_err("Failed to get tc_to_q object. rc=%d\n", rc);
        goto Exit;
    }
   
    if ((rc = bdmf_link(rdpa_tc_to_queue_obj, port_obj, NULL)))
    {
        enet_err("Failed to link tc_to_q table to RDPA port rc=%d\n", rc);
        goto Exit;
    }
    
    if ((rc = rdpa_pbit_to_queue_get(0, &rdpa_pbit_to_queue_obj)))
    {
        enet_err("Failed to get pbit_to_q object. rc=%d\n", rc);
        goto Exit;
    }
   
    if ((rc = bdmf_link(rdpa_pbit_to_queue_obj, port_obj, NULL)))
    {
        enet_err("Failed to link pbit_to_q table to RDPA port rc=%d\n", rc);
        goto Exit;
    }

Exit:
    if (rdpa_tc_to_queue_obj)
        bdmf_put(rdpa_tc_to_queue_obj);
    if (rdpa_pbit_to_queue_obj)
        bdmf_put(rdpa_pbit_to_queue_obj);

    return rc;
}
#endif

static int _demux_id_runner_port(enetx_port_t *self)
{
    rdpa_if demuxif = self->p.port_id;

#if defined(CONFIG_BCM_PON_RDP) || defined(CONFIG_BCM_PON_XRDP)
#ifndef BRCM_FTTDP
    /* XXX non-G9991 FW wan gbe src port is lan0 + mac_id, not wan0 */
    if (rdpa_if_is_wan(demuxif))
    {
#ifndef XRDP
        demuxif = rdpa_if_lan0 + self->p.mac->mac_id;

        if (self->p.mac->mac_id == 5)
#endif
            demuxif = rdpa_wan_type_to_if(rdpa_wan_gbe); 
    }
#endif
#endif /* defined(CONFIG_BCM_PON_RDP) || defined(CONFIG_BCM_PON_XRDP) */
    return demuxif;
}

int runner_port_flooding_set(bdmf_object_handle port_obj, int enable)
{
    int rc = 0;
#if defined(CONFIG_BCM_RUNNER_FLOODING) && !defined(NETDEV_HW_SWITCH)
    rdpa_port_dp_cfg_t cfg;

    if ((rc = rdpa_port_cfg_get(port_obj , &cfg)))
        return -EFAULT;

    if (!cfg.dal_enable)
    {
        enet_err("DA lookup disabled on port, cannot configure flooding\n");
        return -EFAULT;
    }

    if (enable)
    {
        /* Enable flooding. */
        cfg.dal_miss_action = rdpa_forward_action_flood;
    }
    else
    {
        /* Disable flooding, assume action host. XXX: Are we required to store previous action and support drop? */
        cfg.dal_miss_action = rdpa_forward_action_host;
    }
    rc = rdpa_port_cfg_set(port_obj, &cfg);
#endif
    return rc;
}

static int port_runner_port_init(enetx_port_t *self)
{
    bdmf_error_t rc;
    rdpa_if rdpaif = self->p.port_id;
    rdpa_emac emac = rdpa_emac_none;

    if (demux_on_sw(self->p.parent_sw, _demux_id_runner_port(self), self))
        return -1;

    if (self->p.mac)
        emac = rdpa_emac0 + self->p.mac->mac_id;

#ifdef CONFIG_BLOG
    self->n.blog_phy = BLOG_ENETPHY;
    self->n.blog_chnl = self->n.blog_chnl_rx = emac;
#endif

    if (!(self->priv = create_rdpa_port(rdpaif, emac, NULL)))
    {
        enet_err("Failed to create RDPA port object for %s\n", self->obj_name);
        return -1;
    }
        
    /* Override bp_parser settings, since once a rdpa port object is created, port role cannot change */
    self->p.port_cap = rdpa_if_is_wan(rdpaif) ? PORT_CAP_WAN_ONLY : PORT_CAP_LAN_ONLY;
    self->n.port_netdev_role = rdpa_if_is_wan(rdpaif) ? PORT_NETDEV_ROLE_WAN : PORT_NETDEV_ROLE_LAN;

    if (rdpa_if_is_wan(rdpaif))
        self->p.ops = &port_runner_port_wan_gbe; /* use ops with correct dispatch_pkt */
#if !defined(BRCM_FTTDP) && !defined(CONFIG_BCM963158)
    else
    {
        rc = link_pbit_tc_to_q_to_rdpa_lan_port(self->priv);
        if (rc)
            return rc;
    }
#endif

    if (rdpa_if_is_lag_and_switch(rdpaif) && (rc = link_switch_to_rdpa_port(self->priv)))
    {
        enet_err("Failed to link RDPA switch to port object %s. rc =%d\n", self->obj_name, rc);
        return rc;
    }

    enet_dbg("Initialized runner port %s\n", self->obj_name);

    return 0;
}
#endif /* !defined(DSL_RUNNER_DEVICE) */

int port_runner_port_uninit(enetx_port_t *self)
{
    bdmf_object_handle port_obj = self->priv;
    rdpa_port_tm_cfg_t port_tm_cfg;

    if (!port_obj)
        return 0;

    rdpa_port_tm_cfg_get(port_obj, &port_tm_cfg);
    if (port_tm_cfg.sched)
        bdmf_destroy(port_tm_cfg.sched);

    bdmf_destroy(port_obj);
    self->priv = 0;

    demux_on_sw(self->p.parent_sw, _demux_id_runner_port(self), NULL);

    return 0;
}

int read_init_cfg(void)
{
    int rc;
    bdmf_object_handle system_obj = NULL;

    if (system_is_read)
        return 0;

    if ((rc = rdpa_system_get(&system_obj)))
    {
        enet_err("Failed to get RDPA System object\n");
        return rc;
    }
    
    rdpa_system_init_cfg_get(system_obj, &init_cfg);
    bdmf_put(system_obj);
    //enet_dbgv("init_cfg: .gbe_wan_emac=%x .runner_ext_sw_cfg.enabled=%d\n",init_cfg.gbe_wan_emac, init_cfg.runner_ext_sw_cfg.enabled);

    system_is_read = 1;

    enet_dbg("system init_cfg: gbe_wan_emac=%d\n", init_cfg.gbe_wan_emac);

    return 0;
}

extern void add_unspecified_ports(enetx_port_t *sw, uint32_t port_map, uint32_t imp_map);
#include "bcm_chip_arch.h"

int port_runner_sw_init(enetx_port_t *self)
{
#ifdef ENET_RUNNER_WIFI
    if (register_wifi_dev_forwarder())
        return -1;
#endif

#ifdef CONFIG_BCM_PTP_1588
    if (ptp_1588_init())
        return -1;
#endif

#ifdef SF2_DEVICE
    if (self == root_sw)
    {
    
        /* this function creates all unspecified port objects when multiple IMP SF2 ports are connected to runner.
           these need to be explicitly created, since board param file does not specify them.
           These port objects are used by ioctl to access hw info.
           Only device with multiple IMP ports will call this function.
        */
        add_unspecified_ports(self, chip_arch_all_portmap[0], chip_arch_mgmt_portmap[0]);
    }

    /* for SF2 based runner, only register external switch */
    if (init_cfg.runner_ext_sw_cfg.enabled && (self == root_sw))
        goto Exit;
#endif

    if (!(self->priv = create_rdpa_port(rdpa_if_switch, init_cfg.runner_ext_sw_cfg.emac_id, NULL)))
    {
        enet_err("Failed to create RDPA switch object for %s\n", self->obj_name);
        return -1;
    }

#ifdef SF2_DEVICE
Exit:
#endif
    enet_dbg("Initialized runner switch %s\n", self->obj_name);

#if (defined(CONFIG_BCM_SPDSVC) || defined(CONFIG_BCM_SPDSVC_MODULE))
    enet_spdsvc_transmit = bcmFun_get(BCM_FUN_ID_SPDSVC_TRANSMIT);
    BCM_ASSERT(enet_spdsvc_transmit != NULL);
#endif

    return 0;
}

int port_runner_sw_uninit(enetx_port_t *self)
{
    bdmf_object_handle port_obj = self->priv;

    if (port_obj)
    {
        bdmf_destroy(port_obj);
        self->priv = 0;
    }

#ifdef CONFIG_BCM_PTP_1588
    ptp_1588_uninit();
#endif
    return 0;
}

void port_runner_port_stats_get(enetx_port_t *self, struct rtnl_link_stats64 *net_stats)
{
    port_generic_stats_get(self, net_stats);
    
    /* Add FW dropped packets */
    net_stats->rx_dropped += self->n.port_stats.rx_dropped;
    net_stats->tx_dropped += self->n.port_stats.tx_dropped;
}

static void port_runner_port_stats_clear(enetx_port_t *self)
{
    bdmf_object_handle port_obj = self->priv;
    rdpa_port_stat_t stat = {};

    rdpa_port_stat_set(port_obj, &stat);
    port_generic_stats_clear(self);
}
    
/* mib dump for ports on internal runner switch */
static int port_runner_mib_dump(enetx_port_t *self, int all)
{
    /* based on impl5\bcmsw_runner.c bcmeapi_ethsw_dump_mib() */
    mac_stats_t         mac_stats;
    int                 port = self->p.mac->mac_id;
    uint64_t            errcnt = 0;

    mac_dev_stats_get(self->p.mac, &mac_stats);

    printk("\nRunner Stats : Port# %d\n",port);

    /* Display Tx statistics */
     /* Display Tx statistics */
    printk("\n");
    printk("TxUnicastPkts:          %10llu \n", mac_stats.tx_unicast_packet);
    printk("TxMulticastPkts:        %10llu \n", mac_stats.tx_multicast_packet);
    printk("TxBroadcastPkts:        %10llu \n", mac_stats.tx_broadcast_packet);
    printk("TxDropPkts:             %10llu \n", mac_stats.tx_error);

    /* Display remaining tx stats only if requested */
    if (all) {
        printk("TxBytes:                %10llu \n", mac_stats.tx_byte);
        printk("TxFragments:            %10llu \n", mac_stats.tx_fragments_frame);
        printk("TxCol:                  %10llu \n", mac_stats.tx_total_collision);
        printk("TxSingleCol:            %10llu \n", mac_stats.tx_single_collision);
        printk("TxMultipleCol:          %10llu \n", mac_stats.tx_multiple_collision);
        printk("TxDeferredTx:           %10llu \n", mac_stats.tx_deferral_packet);
        printk("TxLateCol:              %10llu \n", mac_stats.tx_late_collision);
        printk("TxExcessiveCol:         %10llu \n", mac_stats.tx_excessive_collision);
        printk("TxPausePkts:            %10llu \n", mac_stats.tx_pause_control_frame);
        printk("TxExcessivePkts:        %10llu \n", mac_stats.tx_excessive_deferral_packet);
        printk("TxJabberFrames:         %10llu \n", mac_stats.tx_jabber_frame);
        printk("TxFcsError:             %10llu \n", mac_stats.tx_fcs_error);
        printk("TxCtrlFrames:           %10llu \n", mac_stats.tx_control_frame);
        printk("TxOverSzFrames:         %10llu \n", mac_stats.tx_oversize_frame);
        printk("TxUnderSzFrames:        %10llu \n", mac_stats.tx_undersize_frame);
        printk("TxUnderrun:             %10llu \n", mac_stats.tx_underrun);
        printk("TxPkts64Octets:         %10llu \n", mac_stats.tx_frame_64);
        printk("TxPkts65to127Octets:    %10llu \n", mac_stats.tx_frame_65_127);
        printk("TxPkts128to255Octets:   %10llu \n", mac_stats.tx_frame_128_255);
        printk("TxPkts256to511Octets:   %10llu \n", mac_stats.tx_frame_256_511);
        printk("TxPkts512to1023Octets:  %10llu \n", mac_stats.tx_frame_512_1023);
        printk("TxPkts1024to1518Octets: %10llu \n", mac_stats.tx_frame_1024_1518);
        printk("TxPkts1519toMTUOctets:  %10llu \n", mac_stats.tx_frame_1519_mtu);
    }
    else {
        errcnt = 0;
        errcnt += mac_stats.tx_total_collision;
        errcnt += mac_stats.tx_single_collision;
        errcnt += mac_stats.tx_multiple_collision;
        errcnt += mac_stats.tx_deferral_packet;
        errcnt += mac_stats.tx_late_collision;
        errcnt += mac_stats.tx_excessive_collision;
        errcnt += mac_stats.tx_excessive_deferral_packet;
        errcnt += mac_stats.tx_jabber_frame;
        errcnt += mac_stats.tx_fcs_error;
        errcnt += mac_stats.tx_undersize_frame;
        errcnt += mac_stats.tx_underrun;
        printk("TxOtherErrors:          %10llu \n", errcnt);
    }

    /* Display Rx statistics */
    printk("\n");
    printk("RxUnicastPkts:          %10llu \n", mac_stats.rx_unicast_packet);
    printk("RxMulticastPkts:        %10llu \n", mac_stats.rx_multicast_packet);
    printk("RxBroadcastPkts:        %10llu \n", mac_stats.rx_broadcast_packet);

    /* Display remaining rx stats only if requested */
    if (all) {
        printk("RxBytes:                %10llu \n", mac_stats.rx_byte);
        printk("RxJabbers:              %10llu \n", mac_stats.rx_jabber);
        printk("RxAlignErrs:            %10llu \n", mac_stats.rx_alignment_error);
        printk("RxFCSErrs:              %10llu \n", mac_stats.rx_fcs_error);
        printk("RxFragments:            %10llu \n", mac_stats.rx_fragments);
        printk("RxOversizePkts:         %10llu \n", mac_stats.rx_oversize_packet);
        printk("RxUndersizePkts:        %10llu \n", mac_stats.rx_undersize_packet);
        printk("RxPausePkts:            %10llu \n", mac_stats.rx_pause_control_frame);
        printk("RxOverflow:             %10llu \n", mac_stats.rx_overflow);
        printk("RxCtrlPkts:             %10llu \n", mac_stats.rx_control_frame);
        printk("RxUnknownOp:            %10llu \n", mac_stats.rx_unknown_opcode);
        printk("RxLenError:             %10llu \n", mac_stats.rx_frame_length_error);
        printk("RxCodeError:            %10llu \n", mac_stats.rx_code_error);
        printk("RxCarrierSenseErr:      %10llu \n", mac_stats.rx_carrier_sense_error);
        printk("RxPkts64Octets:         %10llu \n", mac_stats.rx_frame_64);
        printk("RxPkts65to127Octets:    %10llu \n", mac_stats.rx_frame_65_127);
        printk("RxPkts128to255Octets:   %10llu \n", mac_stats.rx_frame_128_255);
        printk("RxPkts256to511Octets:   %10llu \n", mac_stats.rx_frame_256_511);
        printk("RxPkts512to1023Octets:  %10llu \n", mac_stats.rx_frame_512_1023);
        printk("RxPkts1024to1522Octets: %10llu \n", mac_stats.rx_frame_1024_1518);
        printk("RxPkts1523toMTU:        %10llu \n", mac_stats.rx_frame_1519_mtu);
    }
    else {
        errcnt = 0;
        errcnt += mac_stats.rx_jabber;
        errcnt += mac_stats.rx_alignment_error;
        errcnt += mac_stats.rx_fcs_error;
        errcnt += mac_stats.rx_oversize_packet;
        errcnt += mac_stats.rx_undersize_packet;
        errcnt += mac_stats.rx_overflow;
        errcnt += mac_stats.rx_unknown_opcode;
        errcnt += mac_stats.rx_frame_length_error;
        errcnt += mac_stats.rx_code_error;
        errcnt += mac_stats.rx_carrier_sense_error;
        printk("RxOtherErrors:          %10llu \n", errcnt);
    }
    return 0;
}

static int port_runner_sw_port_id_on_sw(port_info_t *port_info, int *port_id, port_type_t *port_type)
{
    int rc;

    if ((rc = read_init_cfg()))
        return rc;

#ifdef GPON
    if (port_info->is_gpon)
    {
        *port_type = PORT_TYPE_RUNNER_GPON;
        *port_id = rdpa_wan_type_to_if(rdpa_wan_gpon);
        return 0;
    }
#endif
    
#ifdef EPON
    if (port_info->is_epon)
    {
        *port_type = PORT_TYPE_RUNNER_EPON;
        *port_id = rdpa_wan_type_to_if(rdpa_wan_epon);
        return 0;
    }
#endif

    if (port_info->is_detect)
    {
        *port_type = PORT_TYPE_RUNNER_DETECT;
        *port_id = rdpa_if_wan0; /* FIXME: MULTI-WAN XPON */
        return 0;
    }

#ifdef ENET_RUNNER_WIFI
    if (port_info->port >= rdpa_if_ssid0 && port_info->port <= rdpa_if_ssid15)
    {
        *port_type = PORT_TYPE_RUNNER_WIFI;
        *port_id = port_info->port;
        return 0;
    }
#endif
    
    *port_type = PORT_TYPE_RUNNER_PORT;
    if (port_info->is_undef)
    {
        *port_id = rdpa_if_none;
        return 0;
    }
    
    if (port_info->port == init_cfg.gbe_wan_emac &&     /* XXX - all this will be removed later once we remove global-wan-concept */
        init_cfg.gbe_wan_emac != rdpa_emac_none)
    {
        *port_id = rdpa_wan_type_to_if(rdpa_wan_gbe);
        return 0;
    }

    if (port_info->is_attached)
        *port_id = rdpa_if_lag0 + port_info->port;
    else if (port_info->is_management)
        *port_id = rdpa_if_lan0 + 29;
    else if (init_cfg.runner_ext_sw_cfg.enabled)
        *port_id = -1;        // for runner port without CONFIG_BCM_ETHWAN
    else
        *port_id = rdpa_if_lan0 + port_info->port;

    return 0;
}

#if defined(CONFIG_BCM_PTP_1588) && !defined(XRDP)
static int port_runner_sw_demux(enetx_port_t *sw, enetx_rx_info_t *rx_info, FkBuff_t *fkb, enetx_port_t **out_port)
{
    uint32_t reason = rx_info->reason;

    if (unlikely(reason == rdpa_cpu_rx_reason_etype_ptp_1588))
        ptp_1588_rx_pkt_store_timestamp(fkb->data, fkb->len, rx_info->ptp_index);
    
    return port_generic_sw_demux(sw, rx_info, fkb, out_port);
}
#endif

#if !defined(DSL_RUNNER_DEVICE)
static int tr_runner_hw_switching_set_single(enetx_port_t *port, void *_ctx)
{
    unsigned long state = (unsigned long)_ctx;

    if (port->n.port_netdev_role != PORT_NETDEV_ROLE_LAN || !port->dev)
        return 0;
    return runner_port_flooding_set(port->priv, state == HW_SWITCHING_ENABLED);
}

static int runner_hw_switching_state = HW_SWITCHING_DISABLED;

static int runner_hw_switching_set(enetx_port_t *sw, unsigned long state)
{
    int rc;

    rc = port_traverse_ports(root_sw, tr_runner_hw_switching_set_single, PORT_CLASS_PORT, (void *)state);
    if (!rc)
        runner_hw_switching_state = (int)state;
    return rc;
}

static int runner_hw_switching_get(enetx_port_t *sw)
{
    return runner_hw_switching_state;
}

sw_ops_t port_runner_sw =
{
    .init = port_runner_sw_init,
    .uninit = port_runner_sw_uninit,
#if defined(CONFIG_BCM_PTP_1588) && !defined(XRDP)
    .port_demux = port_runner_sw_demux,
#else
    .port_demux = port_generic_sw_demux,
#endif
    .stats_get = port_generic_stats_get,
    .port_id_on_sw = port_runner_sw_port_id_on_sw,
    .hw_sw_state_set = runner_hw_switching_set,
    .hw_sw_state_get = runner_hw_switching_get,
};
#endif /* !defined(DSL_RUNNER_DEVICE) */

int port_runner_pause_get(enetx_port_t *self, int *rx_enable, int *tx_enable)
{
    if (!self->p.mac)
    {
        enet_err("missing mac device in port %s\n", self->obj_name);
        return -1;
    }

    return mac_dev_pause_get(self->p.mac, rx_enable, tx_enable);
}

int port_runner_pause_set(enetx_port_t *self, int rx_enable, int tx_enable)
{
    if (!self->p.mac || !self->dev)
    {
        enet_err("missing mac or net device in port %s\n", self->obj_name);
        return -1;
    }

    return mac_dev_pause_set(self->p.mac, rx_enable, tx_enable, self->dev->dev_addr);
}

int port_runner_generic_mtu_set(enetx_port_t *self, int mtu)
{
    bdmf_object_handle port_obj = self->priv;
    bdmf_number old_rdpa_mtu;
    int rc;

    /* XXX: Is this needed for rdpa, or only mac ? */
    mtu += ENET_MAX_MTU_EXTRA_SIZE;

    rdpa_port_mtu_size_get(port_obj, &old_rdpa_mtu);
    if ((rc = rdpa_port_mtu_size_set(port_obj, mtu)))
    {
        enet_err("failed to set rdpa mtu size %d on %s\n", mtu, self->obj_name);
        return -1;
    }

    if (port_generic_mtu_set(self, mtu))
    {
        /* Rollback */
        rdpa_port_mtu_size_set(port_obj, old_rdpa_mtu);
        return -1;
    }

    return 0;
}

void port_runner_print_status(enetx_port_t *self)
{
    printk("%s ", self->dev->name);
    phy_dev_print_status(self->p.phy);
}

#if defined(GPON) || defined(EPON)
enetx_port_t *pon_port;
#endif

int port_runner_empty_uninit(enetx_port_t *self)
{
    bdmf_object_handle port_obj = self->priv;

    demux_on_sw(self->p.parent_sw, rdpa_if_wan0, NULL); /* FIXME: MULTI-WAN XPON */

    bdmf_put(port_obj);
    self->priv = 0;

#if defined(GPON) || defined(EPON)
    pon_port = NULL;
#endif

    return 0;
}

#ifdef GPON
static void port_runner_gpon_stats(enetx_port_t *self, struct rtnl_link_stats64 *net_stats)
{
   rdpa_gem_stat_t gem_stat;
   bdmf_object_handle gem = NULL;
#if !defined(CONFIG_BCM963158)
   rdpa_iptv_stat_t iptv_stat;
   bdmf_object_handle iptv = NULL;
#endif
   int rc;

    while ((gem = bdmf_get_next(rdpa_gem_drv(), gem, NULL)))
    {
        rc = rdpa_gem_stat_get(gem, &gem_stat);
        if (rc)
            goto gem_exit;

        net_stats->rx_bytes += gem_stat.rx_bytes;
        net_stats->rx_packets += gem_stat.rx_packets;
        net_stats->rx_dropped += gem_stat.rx_packets_discard;
        net_stats->tx_bytes += gem_stat.tx_bytes;
        net_stats->tx_packets += gem_stat.tx_packets;
        net_stats->tx_dropped += gem_stat.tx_packets_discard;
    }

gem_exit:
    if (gem)
        bdmf_put(gem);

#if !defined(CONFIG_BCM963158)
    rc = rdpa_iptv_get(&iptv);
    if (rc)
        goto iptv_exit;

    rc = rdpa_iptv_iptv_stat_get(iptv, &iptv_stat);
    if (rc)
        goto iptv_exit;

    net_stats->multicast = iptv_stat.rx_valid_pkt;

iptv_exit:
    if (iptv)
        bdmf_put(iptv);
#endif
}

static int port_runner_gpon_init(enetx_port_t *self)
{
    self->p.port_cap = PORT_CAP_WAN_ONLY;
    self->n.port_netdev_role = PORT_NETDEV_ROLE_WAN;

    pon_port = self;

#ifdef CONFIG_BLOG
    self->n.blog_phy = BLOG_GPONPHY;
    self->n.set_channel_in_mark = 1; /* blog_chnl will be set to/from gem */
#endif
    if (demux_on_sw(self->p.parent_sw, rdpa_wan_type_to_if(rdpa_wan_gpon), self))
        return -1;
    
    if (rdpa_port_get(rdpa_wan_type_to_if(rdpa_wan_gpon), (bdmf_object_handle *)&self->priv))
        return -1;

    return 0;
}

static int dispatch_pkt_gpon(dispatch_info_t *dispatch_info)
{
    int rc;
    rdpa_cpu_tx_info_t info = {};

    info.method = rdpa_cpu_tx_port;
    info.port = rdpa_wan_type_to_if(rdpa_wan_gpon);
    info.cpu_port = rdpa_cpu_host;
    info.x.wan.queue_id = dispatch_info->egress_queue;
    info.x.wan.flow = dispatch_info->channel;
    info.drop_precedence = dispatch_info->drop_eligible;

    if (dispatch_info->channel == 0)
    {
        nbuff_flushfree(dispatch_info->pNBuff);
        return 0;
    }

    rc = _rdpa_cpu_send_sysb((bdmf_sysb)dispatch_info->pNBuff, &info);
    if (unlikely(rc < 0))
    {
        rdpa_gem_flow_us_cfg_t us_cfg = {};
        bdmf_object_handle gem = NULL;

        rdpa_gem_get(dispatch_info->channel, &gem);
        if (gem)
        {
            rdpa_gem_us_cfg_get(gem, &us_cfg);
            bdmf_put(gem);

            if (!us_cfg.tcont)
            {
                enet_err("can't send sysb - no tcont for gem (%d) \n", dispatch_info->channel);
                return rc;
            }
        }

        enet_err("_rdpa_cpu_send_sysb() rc %d (wan_flow: %d queue_id: %u)\n",
          rc, dispatch_info->channel, dispatch_info->egress_queue);
    }

    return rc;
}

port_ops_t port_runner_gpon =
{
    .init = port_runner_gpon_init,
    .uninit = port_runner_empty_uninit,
    .dispatch_pkt = dispatch_pkt_gpon,
    .stats_get = port_runner_gpon_stats,
    .mtu_set = port_runner_generic_mtu_set,
    /* TODO: stats_clear */
    .mib_dump = port_runner_mib_dump,
    .print_status = port_runner_print_status,
};
#endif /* GPON */

#if !defined(DSL_RUNNER_DEVICE)
int enetxapi_post_config(void)
{
    return 0;
}

port_ops_t port_runner_port =
{
    .init = port_runner_port_init,
    .uninit = port_runner_port_uninit,
    .dispatch_pkt = port_runner_dispatch_pkt_lan,
    .stats_get = port_runner_port_stats_get,
    .stats_clear = port_runner_port_stats_clear,
    .pause_get = port_runner_pause_get,
    .pause_set = port_runner_pause_set,
    .mtu_set = port_runner_generic_mtu_set,
    .mib_dump = port_runner_mib_dump,
    .print_status = port_runner_print_status,
};

port_ops_t port_runner_port_wan_gbe =
{
    .init = port_runner_port_init,
    .uninit = port_runner_port_uninit,
    .dispatch_pkt = dispatch_pkt_gbe_wan,
    .stats_get = port_runner_port_stats_get,
    .stats_clear = port_runner_port_stats_clear,
    .pause_get = port_runner_pause_get,
    .pause_set = port_runner_pause_set,
    .mtu_set = port_runner_generic_mtu_set,
    .mib_dump = port_runner_mib_dump,
    .print_status = port_runner_print_status,
};

#ifdef EPON
static void port_runner_epon_stats(enetx_port_t *self, struct rtnl_link_stats64 *net_stats)
{
    port_generic_stats_get(self, net_stats);
}

static int port_runner_epon_init(enetx_port_t *self)
{
    self->p.port_cap = PORT_CAP_WAN_ONLY;
    self->n.port_netdev_role = PORT_NETDEV_ROLE_WAN;
    pon_port = self;
#ifdef CONFIG_BLOG
    self->n.blog_phy = BLOG_EPONPHY;
    self->n.set_channel_in_mark = 1; /* blog_chnl will be set to/from gem */
#endif
    if (demux_on_sw(self->p.parent_sw, rdpa_wan_type_to_if(rdpa_wan_epon), self))
        return -1;

    if (rdpa_port_get(rdpa_wan_type_to_if(rdpa_wan_epon), (bdmf_object_handle *)&self->priv))
        return -1;

    return 0;
}

static int dispatch_pkt_epon(dispatch_info_t *dispatch_info)
{
    int rc;
    rdpa_cpu_tx_info_t info = {};

    /* TODO: queue/channel mapping from EponFrame.c:EponTrafficSend() */
    info.method = rdpa_cpu_tx_port;
    info.port = rdpa_wan_type_to_if(rdpa_wan_epon);
    info.cpu_port = rdpa_cpu_host;
    info.x.wan.queue_id = dispatch_info->egress_queue;
    info.x.wan.flow = dispatch_info->channel;
    info.drop_precedence = dispatch_info->drop_eligible;

    rc = _rdpa_cpu_send_sysb((bdmf_sysb)dispatch_info->pNBuff, &info);

    return rc;
}

port_ops_t port_runner_epon =
{
    .init = port_runner_epon_init,
    .uninit = port_runner_empty_uninit,
    .dispatch_pkt = dispatch_pkt_epon,
    .stats_get = port_runner_epon_stats,
    .mtu_set = port_runner_generic_mtu_set,
    /* TODO: stats_clear */
    .mib_dump = port_runner_mib_dump,
    .print_status = port_runner_print_status,
};
#endif /* EPON */

#else /* defined(DSL_RUNNER_DEVICE) */
#ifdef SF2_DEVICE
#include "sf2.h"
#endif

#include <bcmnet.h>

// =========== DSL runner port ops =============================

extern int speed_macro_2_mbps(phy_speed_t spd);
static int port_set_wan_role_link(enetx_port_t *port, port_netdev_role_t role)
{
    phy_dev_t *phy;

    if (!port_is_enet_link_handling(port))
        return 0; 

    phy = port->p.phy;
    port->n.port_netdev_role = role;
    if (role == PORT_NETDEV_ROLE_WAN)
    {
        /* Start PHY polling timer */
        if (port->p.port_cap == PORT_CAP_WAN_ONLY)
            phy_dev_link_change_register(phy, dslbase_phy_link_change_cb, phy);
        
        /* Force port link up if physical is up */
        if(phy->link)
            link_change_handler(port, phy->link, speed_macro_2_mbps(phy->speed), 
                    phy->duplex==PHY_DUPLEX_FULL);

    }
    else
    {
        if (port->p.port_cap == PORT_CAP_WAN_ONLY)
        {
            /* Stop PHY polling timer */
            phy_dev_link_change_unregister(phy);

            /* Force port link down */
            phy->link = 0;
            link_change_handler(port, 0, 0, 0);
        }
    }

    return 0;
}

// based on impl5\bcmenet_runner_inline.h:bcmeapi_pkt_xmt_dispatch()
static int dispatch_pkt_gbe_wan(dispatch_info_t *dispatch_info)
{
    rdpa_cpu_tx_extra_info_t extra_info;
    int rc;

    extra_info.u32 = 0; /* Initialize */

    {
#if (defined(CONFIG_BCM_SPDSVC) || defined(CONFIG_BCM_SPDSVC_MODULE))
        spdsvcHook_transmit_t spdsvc_transmit;

        spdsvc_transmit.pNBuff = dispatch_info->pNBuff;
        spdsvc_transmit.dev = NULL;
        spdsvc_transmit.header_type = SPDSVC_HEADER_TYPE_ETH;
        spdsvc_transmit.phy_overhead = BCM_ENET_OVERHEAD;

        rc = enet_spdsvc_transmit(&spdsvc_transmit);
        if(rc < 0)
        {
            /* In case of error, NBuff will be free by spdsvc */
            return 0;
        }
        extra_info.is_spdsvc_setup_packet = rc;
#endif
#if !defined(CONFIG_BCM963158)
        rc = rdpa_cpu_tx_port_enet_or_dsl_wan((bdmf_sysb)dispatch_info->pNBuff, dispatch_info->egress_queue,
                                              (rdpa_flow)GBE_WAN_FLOW_ID, dispatch_info->port->p.port_id, extra_info);
#else
        {
        rdpa_cpu_tx_info_t info = {};

        info.method = rdpa_cpu_tx_port;
        info.port = dispatch_info->port->p.port_id;
        info.cpu_port = rdpa_cpu_host;
        info.x.wan.queue_id = dispatch_info->egress_queue;
        info.drop_precedence = dispatch_info->drop_eligible;

        rc = rdpa_cpu_send_sysb((bdmf_sysb)dispatch_info->pNBuff, &info);
        }
#endif
        if (rc < 0)
        {
            /* skb is already released by rdpa_cpu_tx_port_enet_or_dsl_wan() */
            INC_STAT_TX_DROP(dispatch_info->port,tx_dropped_runner_wan_fail);
            return -1;
        }
    }

    return 0;
}

// =========== DSL runner switch ops ===========================

int port_runner_sw_config_trunk(enetx_port_t *sw, enetx_port_t *port, int grp_no, int add)
{
    /* based on impl5\bcmsw_runner.c:bcm_enet_rdp_config_bond() */
    rdpa_if rdpa_port = port->p.port_id;
    rdpa_if rdpa_bond = rdpa_if_bond0 + grp_no;
    bdmf_object_handle port_obj = NULL;
    bdmf_object_handle bond_obj = NULL;
    int rc;

    if ( rdpa_port == rdpa_if_none )
    {
        enet_err("Invalid rdpa port for %s\n\n", port->obj_name);
        return -1;
    }

    if ( rdpa_bond > rdpa_if_bond_max )
    {
        enet_err("Invalid rdpa bond %d for grp_no=%d %s\n\n", rdpa_bond, grp_no, port->obj_name);
        return -1;
    }

    rc = rdpa_port_get(rdpa_port, &port_obj);
    if (rc)
    {
        enet_err("NO rdpa port for rdpa_if %d %s\n\n", rdpa_port, port->obj_name);
        return -1;
    }

    /* get the rdpa bond port in order to link to lan ports */
    rc = rdpa_port_get(rdpa_bond, &bond_obj);
    if (rc)
    {
        if (add)
        {
            /* Bond object does not exist - Create one */
            BDMF_MATTR(rdpa_port_attrs, rdpa_port_drv());
            rdpa_port_index_set(rdpa_port_attrs, rdpa_bond);
            rc = bdmf_new_and_set(rdpa_port_drv(), NULL, rdpa_port_attrs, &bond_obj);
            if (rc)
            {
                enet_err("Failed to create bond port rc(%d)\n",  rc);
                goto error_exit;
            }
        }
        else
        {
            enet_err("No rdpa bond %d for grp_no=%d %s\n\n", rdpa_bond, grp_no, port->obj_name);
            goto error_exit;
        }
    }

    if (add)
    {
        /* Link the port with bond object */
        rc = bdmf_link(bond_obj, port_obj, NULL);
        if (rc)
        {
            enet_err("Failed to link bond port %d for grp_no=%d %s\n", rdpa_bond, grp_no, port->obj_name);
            goto error_exit;
        }
    }
    else
    {
        /* UnLink the port from bond object */
        rc = bdmf_unlink(bond_obj, port_obj);
        if (rc)
        {
            enet_err("Failed to unlink bond port %d for grp_no=%d %s\n", rdpa_bond, grp_no, port->obj_name);
            goto error_exit;
        }

        if (bdmf_get_next_us_link(bond_obj, NULL) == NULL)
        {
            /* No More linked objects to this bond object - destroy */
            if ( bdmf_destroy(bond_obj) )
            {
                enet_err("Failed to destroy bond port %d for grp_no=%d %s\n", rdpa_bond, grp_no, port->obj_name);
            }
            else
            {
                bond_obj = NULL;
            }
        }
    }

error_exit:
    if (port_obj)
    {
        bdmf_put(port_obj);
    }
    if (bond_obj)
    {
        bdmf_put(bond_obj);
    }
    return rc;
}

#if !defined(CONFIG_BCM963158) 
static int enet_house_keeping_thread(void *self)
{
    enetx_port_t *sw = self;
    while(1) {
        rdpa_cpu_tx_reclaim();
        set_current_state(TASK_INTERRUPTIBLE);
        schedule_timeout(HZ);
    }
    enet_err("House Keeping Thread of %s stopped.\n", sw->s.house_keeping_thread->comm);
    return 0;
}
#endif

sw_ops_t port_runner_sw =
{
    .init = port_runner_sw_init,
    .uninit = port_runner_sw_uninit,
#if !defined(CONFIG_BCM963158) 
    .house_keeping_thread_fun = enet_house_keeping_thread,
#endif
#if defined(SF2_DEVICE)
    .port_demux = port_sf2_sw_demux,
#else
    .port_demux = port_generic_sw_demux,
#endif    
    .stats_get = port_generic_stats_get,
    .port_id_on_sw = port_runner_sw_port_id_on_sw,
    .config_trunk = port_runner_sw_config_trunk,
};

/* port operations for DSL based runner port */
port_ops_t port_runner_port =
{
    .init = port_sf2_port_init,
    .uninit = port_runner_port_uninit,
    .dispatch_pkt = dispatch_pkt_gbe_wan,
    .stats_get = port_runner_port_stats_get,
    .stats_clear = port_runner_port_stats_clear,
    .pause_get = port_runner_pause_get,
    .pause_set = port_runner_pause_set,
    .mtu_set = port_runner_generic_mtu_set,
    .role_set = port_set_wan_role_link,
    .mib_dump = port_runner_mib_dump,
    .print_status = port_sf2_print_status,
};
#endif /* defined(DSL_RUNNER_DEVICE) */


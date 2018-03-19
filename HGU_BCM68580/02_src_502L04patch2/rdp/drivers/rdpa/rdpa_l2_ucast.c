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
 * rdpa_l2_ucast.c
 *
 *  Created on: May 25, 2015
 *      Author: vyadav
 */

#include <bdmf_dev.h>
#include <rdpa_api.h>
#include "rdpa_int.h"
#include <rdd.h>
#include "rdpa_egress_tm_inline.h"
#include "rdpa_rdd_inline.h"
#if defined(BCM63158)
#include "rdpa_l2_ucast.h"
#else
#include <rdd_ih_defs.h>
#include <rdd_data_structures.h>
#endif

#if defined(BCM63158)

/* TBD. Implement functions and move to a new file, rdd_l2_ucast.c */
#include "rdp_drv_natc.h"
#include "data_path_init.h"
#include "rdd_ucast.h"

typedef struct
{
    rdpa_ip_flow_key_t           *lookup_entry;
    rdpa_l2_flow_key_t           *l2_lookup_entry;
    rdd_fc_context_t             context_entry;
    uint32_t                     entry_index;
} __PACKING_ATTRIBUTE_STRUCT_END__ rdd_l2_flow_t;

int rdd_l2_connection_entry_add(rdd_l2_flow_t *add_connection, rdpa_traffic_dir direction)
{
    return -1;
}

int rdd_l2_connection_entry_delete(bdmf_index flow_entry_index)
{
    return -1;
}

int rdd_l2_connection_entry_search(rdd_l2_flow_t *get_connection, rdpa_traffic_dir direction,
    bdmf_index *entry_index)
{
    return -1;
}

int rdd_l2_context_entry_get(bdmf_index flow_entry_index, rdd_fc_context_t *context)
{
    return -1;
}

int rdd_l2_connection_entry_get(rdpa_traffic_dir direction, uint32_t entry_index,
    rdpa_l2_flow_key_t *nat_cache_lkp_entry, bdmf_index *flow_entry_index)
{
    return -1;
}

int rdd_l2_context_entry_flwstat_get(bdmf_index flow_entry_index, rdd_fc_context_t *context_entry)
{
    return rdd_l2_context_entry_get(flow_entry_index, context_entry);
}
int rdd_l2_context_entry_modify(rdd_fc_context_t *context_entry, bdmf_index entry_index)
{
    return -1;
}

int rdd_l2_flow_counters_get(bdmf_index flow_entry_index, uint32_t *packets, uint32_t *bytes)
{
    return -1;
}

#endif /* BCM63158 */


/***************************************************************************
 * l2_ucast object type
 **************************************************************************/

/* l2_ucast object private data */
typedef struct {
    uint32_t num_flows;     /**< Number of configured L2 flows */
} l2_ucast_drv_priv_t;

static struct bdmf_object *l2_ucast_object;
static DEFINE_BDMF_FASTLOCK(l2_ucast_lock);

static int _l2_ucast_read_rdd_flow(bdmf_index index, rdpa_l2_flow_info_t *info)
{
    rdd_fc_context_t rdd_l2_flow = {};

    RDD_FC_UCAST_FLOW_CONTEXT_ENTRY_DTS *rdd_ucast_all = &rdd_l2_flow.fc_ucast_flow_context_entry;
    RDD_FC_UCAST_FLOW_CONTEXT_ETH_XTM_ENTRY_DTS *rdd_ucast_eth_xtm = &rdd_l2_flow.fc_ucast_flow_context_eth_xtm_entry;
    RDD_FC_UCAST_FLOW_CONTEXT_WFD_NIC_ENTRY_DTS *rdd_ucast_wfd_nic = &rdd_l2_flow.fc_ucast_flow_context_wfd_nic_entry;
    RDD_FC_UCAST_FLOW_CONTEXT_WFD_DHD_ENTRY_DTS *rdd_ucast_wfd_dhd = &rdd_l2_flow.fc_ucast_flow_context_wfd_dhd_entry;
    RDD_FC_UCAST_FLOW_CONTEXT_RNR_DHD_ENTRY_DTS *rdd_ucast_rnr_dhd = &rdd_l2_flow.fc_ucast_flow_context_rnr_dhd_entry;

    int rc;

    /* read the l2 flow data from the RDD */
    memset(info, 0, sizeof(*info));

    rc = rdd_l2_context_entry_get(index, &rdd_l2_flow);

    if (rc || !rdd_l2_flow.fc_ucast_flow_context_entry.is_l2_accel ||
        rdd_l2_flow.fc_ucast_flow_context_entry.multicast_flag)
    {
        return BDMF_ERR_NOENT;
    }

    if (rdd_ucast_all->egress_phy == rdd_egress_phy_wlan)
    {
        info->result.egress_if = rdpa_if_wlan0;
        info->result.wfd.nic_ucast.is_wfd = rdd_ucast_all->is_unicast_wfd_any;
        if (rdd_ucast_all->is_unicast_wfd_any)
        {
            /* WFD mode */
            info->result.queue_id = BDMF_INDEX_UNASSIGNED;
            info->result.wfd.nic_ucast.is_chain = rdd_ucast_all->is_unicast_wfd_nic;

            if (rdd_ucast_all->is_unicast_wfd_nic)
            {
                /* WFD NIC mode */
                info->result.wfd.nic_ucast.wfd_prio = rdd_ucast_all->wfd_prio;
                info->result.wfd.nic_ucast.wfd_idx = rdd_ucast_all->wfd_idx;
                info->result.wfd.nic_ucast.priority = rdd_ucast_all->priority;
                info->result.wfd.nic_ucast.chain_idx = rdd_ucast_wfd_nic->chain_idx;
            }
            else
            {
                /* WFD DHD mode */
                info->result.wfd.dhd_ucast.wfd_prio = rdd_ucast_all->wfd_prio;
                info->result.wfd.dhd_ucast.wfd_idx = rdd_ucast_all->wfd_idx;
                info->result.wfd.dhd_ucast.priority = rdd_ucast_all->priority;
                info->result.wfd.dhd_ucast.ssid = rdd_ucast_wfd_dhd->wifi_ssid;
                info->result.wfd.dhd_ucast.flowring_idx = rdd_ucast_wfd_dhd->flow_ring_id;
            }
        }
        else
        {
            /* Runner DHD offload mode */
            info->result.rnr.radio_idx = rdd_ucast_rnr_dhd->radio_idx;
            info->result.rnr.priority = rdd_ucast_all->priority;
            info->result.rnr.ssid = rdd_ucast_rnr_dhd->wifi_ssid;
            info->result.rnr.flowring_idx = rdd_ucast_rnr_dhd->flow_ring_id;
        }
    }
    else if (rdd_ucast_all->egress_phy == rdd_egress_phy_eth_lan)
    {
#if defined(BCM63158)
        _rdpa_egress_tm_queue_id_by_lan_port_qm_queue(rdd_ucast_eth_xtm->egress_port, rdd_ucast_eth_xtm->queue,
            &info->result.queue_id);
#else
        info->result.egress_if = rdd_lan_mac_to_rdpa_if(rdd_ucast_eth_xtm->egress_port);
        /* This function shall be called to get the queue_id */
        info->result.queue_id = BDMF_INDEX_UNASSIGNED;
        _rdpa_egress_tm_queue_id_by_lan_port_queue(info->result.egress_if,
            rdd_ucast_eth_xtm->traffic_class, &info->result.queue_id);
#endif
    }
    else if (rdd_ucast_all->egress_phy == rdd_egress_phy_eth_wan ||
             rdd_ucast_all->egress_phy == rdd_egress_phy_dsl ||
             rdd_ucast_all->egress_phy == rdd_egress_phy_gpon)       /* WAN */
    {
        rdpa_wan_type wan_type = rdd_egress_phy2rdpa_wan_type(rdd_ucast_all->egress_phy);
        info->result.egress_if = rdpa_wan_type_to_if(wan_type);

        info->result.wan_flow = rdd_ucast_eth_xtm->egress_port;
#if defined(BCM63158)
        info->result.wan_flow_mode = 0;
#else
        info->result.wan_flow_mode = rdd_ucast_eth_xtm->egress_info;
#endif

        info->result.queue_id = BDMF_INDEX_UNASSIGNED;

#if defined(BCM63158)
        _rdpa_egress_tm_queue_id_by_wan_flow_qm_queue(&info->result.wan_flow, rdd_ucast_eth_xtm->queue,
            &info->result.queue_id);
#else
        {
        int channel = 0;
        
        /* For EthWan, channel is 0.
         * For DslWan, wan flows [0..15] are associated with channel 0.
         *             wan flows [16..31] are associated with channel 1.
         *             ........
         *             wan flows [240..255] are associated with channel 15.
         */
        if (wan_type == rdpa_wan_gbe) /* Ethernet WAN */
            channel = RDD_WAN1_CHANNEL_BASE;
        else
            channel = (info->result.wan_flow / 16) + RDD_WAN0_CHANNEL_BASE;
            
        _rdpa_egress_tm_queue_id_by_channel_rc_queue(channel,
                    rdd_ucast_eth_xtm->rate_controller, rdd_ucast_eth_xtm->traffic_class,
                    &info->result.queue_id);
        }
#endif
    }
    else
    {
        BDMF_TRACE_ERR("Invalid egress_phy %u\n", rdd_ucast_all->egress_phy);

        return BDMF_ERR_PARM;
    }

    info->result.is_routed = rdd_ucast_all->is_routed;
    info->result.is_l2_accel = rdd_ucast_all->is_l2_accel;
    info->result.service_queue_id = rdd_ucast_all->service_queue_id;
    info->result.is_wred_high_prio = rdd_ucast_all->is_high_prio;
    info->result.mtu = rdd_ucast_all->mtu;
    info->result.is_tos_mangle = rdd_ucast_all->is_tos_mangle;
    info->result.tos = rdd_ucast_all->tos;
    info->result.cmd_list_length = rdd_ucast_all->command_list_length_64 << 3;
    info->result.drop = rdd_ucast_all->drop;

    memcpy(info->result.cmd_list, rdd_ucast_all->command_list, RDPA_CMD_LIST_UCAST_LIST_SIZE);

#if defined(BCM63158)
    rc = rdd_l2_connection_entry_get(rdd_l2_flow.fc_ucast_flow_context_entry.connection_direction,
        index, &info->key, &index);
#else
    rc = rdd_l2_connection_entry_get(rdd_l2_flow.fc_ucast_flow_context_entry.connection_direction,
        rdd_l2_flow.fc_ucast_flow_context_entry.connection_table_index,
        &info->key, &index);
#endif
    if (rc)
    {
        return BDMF_ERR_NOENT;
    }

    /* FIXME: This should be moved to RDD! */
    info->key.dir = rdd_l2_flow.fc_ucast_flow_context_entry.connection_direction;
#if !defined(BCM63158) /* TBD. BCM63158 */
    {
    uint8_t wifi_ssid = 0;
    info->key.ingress_if = rdpa_rdd_bridge_port_to_if(info->key.lookup_port, wifi_ssid);
    }
#endif

    return 0;
}

int _l2_ucast_prepare_rdd_flow_params(const rdpa_l2_flow_info_t * const info, 
    rdd_fc_context_t *rdd_l2_flow, bdmf_boolean is_new_flow)
{
    RDD_FC_UCAST_FLOW_CONTEXT_ENTRY_DTS *rdd_ucast_all = &rdd_l2_flow->fc_ucast_flow_context_entry;
    RDD_FC_UCAST_FLOW_CONTEXT_ETH_XTM_ENTRY_DTS *rdd_ucast_eth_xtm = &rdd_l2_flow->fc_ucast_flow_context_eth_xtm_entry;
    RDD_FC_UCAST_FLOW_CONTEXT_WFD_NIC_ENTRY_DTS *rdd_ucast_wfd_nic = &rdd_l2_flow->fc_ucast_flow_context_wfd_nic_entry;
    RDD_FC_UCAST_FLOW_CONTEXT_WFD_DHD_ENTRY_DTS *rdd_ucast_wfd_dhd = &rdd_l2_flow->fc_ucast_flow_context_wfd_dhd_entry;
    RDD_FC_UCAST_FLOW_CONTEXT_RNR_DHD_ENTRY_DTS *rdd_ucast_rnr_dhd = &rdd_l2_flow->fc_ucast_flow_context_rnr_dhd_entry;
    int rc_id = 0;
    int priority = 0;

    if (is_new_flow) /* when new flow is created all the parameters should be set*/
    {
        rdd_ucast_all->multicast_flag = 0;
        rdd_ucast_all->is_routed = info->result.is_routed;
        rdd_ucast_all->is_l2_accel = info->result.is_l2_accel;
        rdd_ucast_all->service_queue_id = info->result.service_queue_id;
        rdd_ucast_all->mtu = info->result.mtu;
        rdd_ucast_all->is_tos_mangle = info->result.is_tos_mangle;
        rdd_ucast_all->tos = info->result.tos;
        rdd_ucast_all->is_unicast_wfd_any = 0;
        rdd_ucast_all->drop = info->result.drop;

        if (info->result.egress_if == rdpa_if_wlan0) /* WLAN-TX i.e. LAN/WAN to WLAN */
        {
            rdd_ucast_all->egress_phy = rdd_egress_phy_wlan;
            rdd_ucast_all->is_unicast_wfd_any = info->result.rnr.is_wfd;
            rdd_ucast_all->is_high_prio = 0;

            if (info->result.rnr.is_wfd)
            {
                if (info->result.wfd.nic_ucast.is_chain)
                {
                    /* WFD NIC mode */
                    rdd_ucast_all->is_unicast_wfd_nic = 1;
                    rdd_ucast_all->priority = info->result.wfd.nic_ucast.priority;
                    rdd_ucast_all->wfd_prio = info->result.wfd.nic_ucast.wfd_prio;
                    rdd_ucast_all->wfd_idx = info->result.wfd.nic_ucast.wfd_idx;
                    rdd_ucast_wfd_nic->chain_idx = info->result.wfd.nic_ucast.chain_idx;
                }
                else
                {
                    /* WFD DHD mode */
                    rdd_ucast_all->is_unicast_wfd_nic = 0;
                    rdd_ucast_all->priority = info->result.wfd.dhd_ucast.priority;
                    rdd_ucast_all->wfd_prio = info->result.wfd.dhd_ucast.wfd_prio;
                    rdd_ucast_all->wfd_idx = info->result.wfd.dhd_ucast.wfd_idx;
                    rdd_ucast_wfd_dhd->wifi_ssid = info->result.wfd.dhd_ucast.ssid;
                    rdd_ucast_wfd_dhd->flow_ring_id = info->result.wfd.dhd_ucast.flowring_idx;
                }
            }
            else 
            {
                /* Runner DHD offload mode */
                rdd_ucast_all->is_unicast_wfd_nic = 0;
                rdd_ucast_all->priority = info->result.rnr.priority;
                rdd_ucast_rnr_dhd->radio_idx = info->result.rnr.radio_idx;
                rdd_ucast_rnr_dhd->wifi_ssid = info->result.rnr.ssid;
                rdd_ucast_rnr_dhd->flow_ring_id = info->result.rnr.flowring_idx;
            }
        }
        else
        {
            int rc = 0;

            if (rdpa_if_is_wan(info->result.egress_if)) /* towards WAN */
            {
                rdpa_wan_type wan_type = rdpa_wan_if_to_wan_type(info->result.egress_if);
                rdd_ucast_all->egress_phy = rdpa_wan_type2rdd_egress_phy(wan_type);
                rdd_ucast_eth_xtm->egress_port = info->result.wan_flow;
                rdd_ucast_eth_xtm->egress_info = info->result.wan_flow_mode;

                if (!info->result.drop)
                {
                    int channel = 0, tc_id;
                    uint32_t runner_qid;

#if defined(BCM63158)
                    runner_qid = info->result.queue_id;
                    tc_id = info->result.tc;
                    rc = _rdpa_egress_tm_wan_flow_queue_to_rdd(info->result.wan_flow, runner_qid,
                        &channel, &rc_id, &priority);
#else
                    /* For EthWan, channel is 0.
                     * For DslWan,
                     *    wan flows [0..15] are associated with channel 0 and egress qid 0.
                     *    wan flows [16..31] are associated with channel 1 and egress qid 1.
                     *    ........
                     *    wan flows [240..255] are associated with channel 15 and egress qid 15.
                     *
                     * info->result.queue_id is the qid retrieved from skb->mark bits[0-3].
                     * For EthWan, info->result.queue_id is the runner egress queue id.
                     * For DslWan, info->result.queue_id may not be the runner egress queue id.
                     * For instance,
                     *    atm0 has 3 queues and atm1 has 2 queues.
                     *    Then info->result.queue_id of atm0 may be 0, 1 or 2.
                     *         info->result.queue_id of atm1 may be 0 or 1.
                     * Therefore, for DslWan, runner egress queue id shall be derived from
                     * wan_flow. The queue id will be correct for both atm and ptm mode.
                     */ 
                    if (wan_type == rdpa_wan_gbe)
                    {
                        /* EthWan */
                        runner_qid = info->result.queue_id;
                        channel    = RDD_WAN1_CHANNEL_BASE;
                    }
                    else
                    {
                        /* DslWan */
                        runner_qid = info->result.wan_flow / 16;
                        channel    = runner_qid + RDD_WAN0_CHANNEL_BASE;
                    }
                    tc_id = info->result.tc;
                    rc = _rdpa_egress_tm_channel_queue_to_rdd_tc_check(rdpa_dir_us, channel, runner_qid,
                                                                       &rc_id, &priority, &tc_id);
#endif
                    if (rc)
                    {
                        BDMF_TRACE_RET(rc, "ucast: US egress queue %u is not configured\n",
                                       runner_qid);
                    }
                    rdd_ucast_all->is_high_prio = tc_id;
                }
            }
            else /* towards Ethernet LAN */
            {
                int tc_id;
#if defined(BCM63158)
                rdd_ucast_eth_xtm->egress_port = rdpa_port_rdpa_if_to_vport(info->result.egress_if);
#else
                BL_LILAC_RDD_EMAC_ID_DTE rdd_emac; 
                uint8_t wifi_ssid;

                rdpa_if_to_rdd_lan_mac(info->result.egress_if, &rdd_emac, &wifi_ssid);
                rdd_ucast_eth_xtm->egress_port = rdd_emac;
#endif
                rdd_ucast_all->egress_phy = rdd_egress_phy_eth_lan;
                rdd_ucast_eth_xtm->egress_info = info->result.lag_port;

                if (!info->result.drop)
                {
                    /* This function shall be called to get rc_id and priority,
                     * so that packets are put in the correct Runner queue.
                     */
                    tc_id = info->result.tc;
                    rc = _rdpa_egress_tm_lan_port_queue_to_rdd_tc_check(info->result.egress_if,
                                                                        info->result.queue_id, &rc_id, &priority, &tc_id);
                    if (rc)
                    {
                        BDMF_TRACE_RET(rc, "ucast: DS egress queue %u is not configured\n",
                                       info->result.queue_id);
                    }
                    rdd_ucast_all->is_high_prio = tc_id;
                }
            }

#if defined(BCM63158)
            rdd_ucast_eth_xtm->queue = priority;
#else
            rdd_ucast_eth_xtm->rate_controller = rc_id;
            rdd_ucast_eth_xtm->traffic_class = priority;
#endif
        }

        if (!info->result.drop)
        {
            rdd_ucast_all->command_list_length_64 = (info->result.cmd_list_length + 7) >> 3;
        }

        memcpy(rdd_ucast_all->command_list, info->result.cmd_list, RDPA_CMD_LIST_UCAST_LIST_SIZE);
    }
    else 
    {
        /* TBD... */
    }

    return 0;
}

static void remove_all_l2_flows(struct bdmf_object *mo)
{
    int i, rc;
    rdd_fc_context_t rdd_l2_flow = {};

    for (i = 0; i < RDPA_UCAST_MAX_FLOWS; i++)
    {
        rc = rdd_l2_context_entry_get(i, &rdd_l2_flow);
        if (rc || !rdd_l2_flow.fc_ucast_flow_context_entry.is_l2_accel || rdd_l2_flow.fc_ucast_flow_context_entry.multicast_flag)
        {
            continue;
        }

#if defined(BCM63158)
        if (!rdd_l2_flow.fc_ucast_flow_context_entry.valid)
            continue;
#endif

        rc = rdpa_l2_ucast_flow_delete(mo, i);
        if (rc)
        {
            bdmf_trace("L2 Unicast flow deletion failed, error=%d\n", rc);
        }
    }
}

/** find l2_ucast object */
static int l2_ucast_get(struct bdmf_type *drv, struct bdmf_object *owner, const char *discr, struct bdmf_object **pmo)
{
    if (!l2_ucast_object)
        return BDMF_ERR_NOENT;
    *pmo = l2_ucast_object;
    return 0;
}

/*
 * l2_ucast attribute access
 */

/* "flow" attribute "read" callback */
static int l2_ucast_attr_flow_read(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index, void *val,
    uint32_t size)
{
    rdpa_l2_flow_info_t *info = (rdpa_l2_flow_info_t *)val;

    /* read the l2 flow data from the RDD */
    return _l2_ucast_read_rdd_flow(index, info);
}

/* "flow" attribute write callback */
static int l2_ucast_attr_flow_write(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index, const void *val,
    uint32_t size)
{
    rdpa_l2_flow_info_t *info = (rdpa_l2_flow_info_t *)val;
    rdd_fc_context_t rdd_l2_flow = {};
    RDD_FC_UCAST_FLOW_CONTEXT_ENTRY_DTS *rdd_ucast_all = &rdd_l2_flow.fc_ucast_flow_context_entry;
    int rc;

    /* read the l2 flow context from the RDD */
    rc = rdd_l2_context_entry_get(index, &rdd_l2_flow);

    if (rc || !rdd_ucast_all->is_l2_accel || rdd_ucast_all->multicast_flag)
    {
        return BDMF_ERR_NOENT;
    }

    rdd_ucast_all->service_queue_id = info->result.service_queue_id;

    return rdd_l2_context_entry_modify(&rdd_l2_flow, index);
}

/* "flow" attribute add callback */
static int l2_ucast_attr_flow_add(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index *index, const void *val,
    uint32_t size)
{
    l2_ucast_drv_priv_t *l2_ucast = (l2_ucast_drv_priv_t *)bdmf_obj_data(l2_ucast_object);
    rdpa_l2_flow_info_t *info = (rdpa_l2_flow_info_t *)val;
    rdd_l2_flow_t rdd_l2_flow = {};
    int rc;

    /* prepare l2 flow result to configure in RDD */
    rc = _l2_ucast_prepare_rdd_flow_params(info, &rdd_l2_flow.context_entry, 1);
    if (rc)
        return rc;

    /* create the l2 flow in the RDD */
    rdd_l2_flow.l2_lookup_entry = &info->key;  
#if !defined(BCM63158)
    {
    uint8_t wifi_ssid;
    uint8_t src_bridge_port;

    src_bridge_port = rdpa_if_to_rdd_bridge_port(info->key.ingress_if, &wifi_ssid);
    rdd_lookup_ports_mapping_table_get(src_bridge_port, &info->key.lookup_port);
    }
#endif

    rc = rdd_l2_connection_entry_add(&rdd_l2_flow, info->key.dir);
    if (rc)
    {
#if defined(WL4908) || defined(BCM63158)
        if (rc == BDMF_ERR_IGNORE)
            return BDMF_ERR_OK;
        else
            return rc;
#else
        if (rc == BL_LILAC_RDD_ERROR_LOOKUP_ENTRY_EXISTS) 
            return BDMF_ERR_ALREADY; 
        else if (rc == BL_LILAC_RDD_ERROR_ADD_CONTEXT_ENTRY)
            return BDMF_ERR_NORES;
        else if (rc == BL_LILAC_RDD_ERROR_ADD_LOOKUP_NO_EMPTY_ENTRY)
            return BL_LILAC_RDD_OK;
#endif

        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "L2 flow creation failed, error=%d\n", rc);
    }

    /* set the created flow index, to return*/
#if defined(BCM63158)
    *index = rdd_l2_flow.entry_index; 
#else
    *index = rdd_l2_flow.xo_entry_index; 
#endif

    bdmf_fastlock_lock(&l2_ucast_lock);
    l2_ucast->num_flows++;
    bdmf_fastlock_unlock(&l2_ucast_lock);

    return 0;
}

/* "flow" attribute delete callback */
static int l2_ucast_attr_flow_delete(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index)
{
    l2_ucast_drv_priv_t *l2_ucast = (l2_ucast_drv_priv_t *)bdmf_obj_data(l2_ucast_object);
    int rc;

    rc = rdd_l2_connection_entry_delete(index);
    if (rc) 
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "L2 Unicast flow deletion failed: index=%lu, error=%d\n", index, rc);

    bdmf_fastlock_lock(&l2_ucast_lock);
    l2_ucast->num_flows--;
    bdmf_fastlock_unlock(&l2_ucast_lock);

    return 0;
}

/* "flow" attribute find callback.
 * Updates *index, can update *val as well
 */
static int l2_ucast_attr_flow_find(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index *index,
    void *val, uint32_t size)
{
    rdpa_l2_flow_info_t *info = (rdpa_l2_flow_info_t *)val;
    rdd_l2_flow_t rdd_l2_flow = {};
    int rc = 1;

    rdd_l2_flow.l2_lookup_entry = &info->key;
#if !defined(BCM63158)
    {
    uint8_t wifi_ssid;
    uint8_t src_bridge_port;

    src_bridge_port = rdpa_if_to_rdd_bridge_port(info->key.ingress_if, &wifi_ssid);
    rdd_lookup_ports_mapping_table_get(src_bridge_port, &info->key.lookup_port);
    }
#endif

    /* search the l2 flow  in RDD, TODO remove casting when RDD is ready */
    rc = rdd_l2_connection_entry_search(&rdd_l2_flow, info->key.dir, index);
    if (rc)
        return BDMF_ERR_NOENT;

    return 0;
}

/* "flow_stat" attribute "read" callback */
static int l2_ucast_attr_flow_stat_read(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index, void *val,
    uint32_t size)
{
    rdpa_stat_t *stat = (rdpa_stat_t *)val;
    rdd_fc_context_t rdd_l2_flow = {};
    int rc;

    /* read the l2 flow stats from the RDD */
    rc = rdd_l2_context_entry_flwstat_get(index, &rdd_l2_flow);

    if (rc || !rdd_l2_flow.fc_ucast_flow_context_entry.is_l2_accel || rdd_l2_flow.fc_ucast_flow_context_entry.multicast_flag)
    {
        return BDMF_ERR_NOENT;
    }

#if defined(BCM63158)
    rc = rdd_l2_flow_counters_get(index, &stat->packets, &stat->bytes);
#else
    stat->packets = rdd_l2_flow.fc_ucast_flow_context_entry.flow_hits;
    stat->bytes = rdd_l2_flow.fc_ucast_flow_context_entry.flow_bytes;
#endif

    return 0;
}

/*  l2_flow_key aggregate type */
struct bdmf_aggr_type l2_flow_key_type = {
    .name = "l2_flow_key", .struct_name = "rdpa_l2_flow_key_t",
    .help = "L2 Flow Key",
    .fields = (struct bdmf_attr[]) 
    {
        {.name = "src_mac", .help = "Source MAC address", .size = sizeof(bdmf_mac_t),
            .type = bdmf_attr_ether_addr, .offset = offsetof(rdpa_l2_flow_key_t, src_mac)
        },
        {.name = "dst_mac", .help = "Destinantion MAC address", .size = sizeof(bdmf_mac_t),
            .type = bdmf_attr_ether_addr, .offset = offsetof(rdpa_l2_flow_key_t, dst_mac)
        },
        {.name = "vtag0", .help = "VLAN Tag 0",
            .type = bdmf_attr_number, .size = sizeof(uint32_t), .offset = offsetof(rdpa_l2_flow_key_t, vtag0),
            .flags = BDMF_ATTR_UNSIGNED
        },
        {.name = "vtag1", .help = "VLAN Tag 1",
            .type = bdmf_attr_number, .size = sizeof(uint32_t), .offset = offsetof(rdpa_l2_flow_key_t, vtag1),
            .flags = BDMF_ATTR_UNSIGNED
        },
        {.name = "vtag_num", .help = "Number of VLAN Tags", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_l2_flow_key_t, vtag_num),
            .flags = BDMF_ATTR_UNSIGNED
        },
        {.name = "eth_type", .help = "Ether Type", .size = sizeof(uint16_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_l2_flow_key_t, eth_type),
            .flags = BDMF_ATTR_UNSIGNED
        },
        {.name = "tos", .help = "ToS", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_l2_flow_key_t, tos),
            .flags = BDMF_ATTR_UNSIGNED
        },
        {.name = "dir", .help = "Traffic direction",
            .type = bdmf_attr_enum, .ts.enum_table = &rdpa_traffic_dir_enum_table,
            .size = sizeof(rdpa_traffic_dir), .offset = offsetof(rdpa_l2_flow_key_t, dir)
        },
#if !defined(BCM63158)
        { .name = "ingress_if", .help = "Ingress interface", 
            .type = bdmf_attr_enum, .ts.enum_table = &rdpa_if_enum_table,
            .size = sizeof(rdpa_if), .offset = offsetof(rdpa_l2_flow_key_t, ingress_if)
        },
        { .name = "lookup_port", .help = "Lookup port", .size = sizeof(uint8_t), 
            .type = bdmf_attr_number, .offset = offsetof(rdpa_l2_flow_key_t, lookup_port),
            .flags = BDMF_ATTR_UNSIGNED
        },
#endif
        BDMF_ATTR_LAST
    },
};
DECLARE_BDMF_AGGREGATE_TYPE(l2_flow_key_type);

/*  l2_flow_result aggregate type */
struct bdmf_aggr_type l2_flow_result_type = 
{
    .name = "l2_flow_result", .struct_name = "rdpa_l2_flow_result_t",
    .help = "L2 Flow Result",
    .fields = (struct bdmf_attr[])
    {
        { .name = "egress_if", .help = "Egress Interface", 
            .type = bdmf_attr_enum, .ts.enum_table = &rdpa_if_enum_table,
            .size = sizeof(rdpa_if), .offset = offsetof(rdpa_l2_flow_result_t, egress_if)
        },
        { .name = "queue_id", .help = "Egress Queue ID", .size = sizeof(uint32_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_l2_flow_result_t, queue_id)
        },
        { .name = "service_queue_id", .help = "Service Queue ID", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_l2_flow_result_t, service_queue_id)
        },
        { .name = "wan_flow", .help = "DSL ATM/PTM US channel", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_l2_flow_result_t, wan_flow),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "wan_flow_mode", .help = "xDSL PTM bonded or single", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_l2_flow_result_t, wan_flow_mode),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "is_routed", .help = "1: Routed Flow; 0: Bridged Flow", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_l2_flow_result_t, is_routed),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "is_l2_accel", .help = "1: L2 Accelerated Flow ; 0: L3 Accelerated Flow", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_l2_flow_result_t, is_l2_accel),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "is_wred_high_prio", .help = "1: High Priority for WRED; 0: Low Priority", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_l2_flow_result_t, is_wred_high_prio),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "drop", .help = "1: Drop packets; 0: Forward packets", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_l2_flow_result_t, drop),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "mtu", .help = "Egress Port MTU", .size = sizeof(uint16_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_l2_flow_result_t, mtu),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "is_tos_mangle", .help = "1: ToS mangle; 0: No ToS mangle", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_l2_flow_result_t, is_tos_mangle),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "tos", .help = "Tx ToS", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_l2_flow_result_t, tos),
            .flags = BDMF_ATTR_UNSIGNED | BDMF_ATTR_HEX_FORMAT
        },
        { .name = "wl_metadata", .help = "WLAN metadata ", .size = sizeof(uint32_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_l2_flow_result_t, wl_metadata),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "cmd_list_length", .help = "Command List Length", .size = sizeof(uint8_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_l2_flow_result_t, cmd_list_length),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "cmd_list", .help = "Command List", .size = RDPA_CMD_LIST_UCAST_LIST_SIZE,
            .type = bdmf_attr_buffer, .offset = offsetof(rdpa_l2_flow_result_t, cmd_list)
        },
        BDMF_ATTR_LAST
    },
};
DECLARE_BDMF_AGGREGATE_TYPE(l2_flow_result_type);

/*  l2_flow_info aggregate type */
struct bdmf_aggr_type l2_flow_info_type = {
    .name = "l2_flow_info", .struct_name = "rdpa_l2_flow_info_t",
    .help = "Fast L2 Connection Info (key+result)",
    .fields = (struct bdmf_attr[]) {
        { .name = "key", .help = "L2 flow key", .type = bdmf_attr_aggregate,
            .ts.aggr_type_name = "l2_flow_key", .offset = offsetof(rdpa_l2_flow_info_t, key)
        },
        { .name = "result", .help = "L2 flow result", .type = bdmf_attr_aggregate,
            .ts.aggr_type_name = "l2_flow_result", .offset = offsetof(rdpa_l2_flow_info_t, result)
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE(l2_flow_info_type);

/* Object attribute descriptors */
struct bdmf_attr l2_ucast_attrs[] = {
    { .name = "nflows", .help = "number of configured L2 flows",
        .type = bdmf_attr_number, .flags = BDMF_ATTR_READ | BDMF_ATTR_CONFIG,
        .size = sizeof(uint32_t), .offset = offsetof(l2_ucast_drv_priv_t, num_flows)
    },
    { .name = "flow", .help = "l2_ucast flow entry",
        .type = bdmf_attr_aggregate, .ts.aggr_type_name = "l2_flow_info", .array_size = RDPA_UCAST_MAX_FLOWS,
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG | BDMF_ATTR_NOLOCK,
        .read = l2_ucast_attr_flow_read, .write = l2_ucast_attr_flow_write,
        .add = l2_ucast_attr_flow_add, .del = l2_ucast_attr_flow_delete,
        .find = l2_ucast_attr_flow_find
    },
    { .name = "flow_stat", .help = "l2_ucast flow entry statistics",
        .type = bdmf_attr_aggregate, .ts.aggr_type_name = "rdpa_stat", .array_size = RDPA_UCAST_MAX_FLOWS,
        .flags = BDMF_ATTR_READ | BDMF_ATTR_STAT | BDMF_ATTR_NOLOCK,
        .read = l2_ucast_attr_flow_stat_read
    },
    BDMF_ATTR_LAST
};


static int l2_ucast_post_init(struct bdmf_object *mo);
static void l2_ucast_destroy(struct bdmf_object *mo);
static int l2_ucast_drv_init(struct bdmf_type *drv);
static void l2_ucast_drv_exit(struct bdmf_type *drv);

struct bdmf_type l2_ucast_drv = {
    .name = "l2_ucast",
    .parent = "system",
    .description = "L2 Unicast Flow Manager",
    .drv_init = l2_ucast_drv_init,
    .drv_exit = l2_ucast_drv_exit,
    .post_init = l2_ucast_post_init,
    .destroy = l2_ucast_destroy,
    .get = l2_ucast_get,
    .extra_size = sizeof(l2_ucast_drv_priv_t),
    .aattr = l2_ucast_attrs,
    .max_objs = 1,
};
DECLARE_BDMF_TYPE(rdpa_l2_ucast, l2_ucast_drv);

/** This optional callback is called at object init time
 * after initial attributes are set.
 * Its work is:
 * - make sure that all necessary attributes are set and make sense
 * - allocate dynamic resources if any
 * - assign object name if not done in pre_init
 * - finalise object creation
 * If function returns error code !=0, object creation is aborted
 */
static int l2_ucast_post_init(struct bdmf_object *mo)
{  
    /* save pointer to the l2_ucast object */
    l2_ucast_object = mo;

    /* set object name */
    snprintf(mo->name, sizeof(mo->name), "l2_ucast");

    /* disable console trace */
    bdmf_trace_level_set(&l2_ucast_drv, bdmf_trace_level_error);

    return 0;
}

static void l2_ucast_destroy(struct bdmf_object *mo)
{
    bdmf_trace_level_set(&l2_ucast_drv, bdmf_trace_level_info);

    remove_all_l2_flows(mo);

    l2_ucast_object = NULL;
}

/* Init/exit module. Cater for GPL layer */
static int l2_ucast_drv_init(struct bdmf_type *drv)
{
#ifdef BDMF_DRIVER_GPL_LAYER
    f_rdpa_l2_ucast_drv = rdpa_l2_ucast_drv;
    f_rdpa_l2_ucast_get = rdpa_l2_ucast_get;
#endif
    return 0;
}

static void l2_ucast_drv_exit(struct bdmf_type *drv)
{
#ifdef BDMF_DRIVER_GPL_LAYER
    f_rdpa_l2_ucast_drv = NULL;
    f_rdpa_l2_ucast_get = NULL;
#endif
}

/***************************************************************************
 * Functions declared in auto-generated header
 **************************************************************************/

/** Get l2_ucast object by key
 * \return  Object handle or NULL if not found
 */
int rdpa_l2_ucast_get(bdmf_object_handle *_obj_)
{
    if (!l2_ucast_object)
        return BDMF_ERR_NOENT;
    bdmf_get(l2_ucast_object);
    *_obj_ = l2_ucast_object;
    return 0;
}

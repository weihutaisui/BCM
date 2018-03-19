/*
<:copyright-BRCM:2013:proprietary:standard

   Copyright (c) 2013 Broadcom 
   All Rights Reserved

 This program is the proprietary software of Broadcom and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").  Except as set forth in
 an Authorized License, Broadcom grants no license (express or implied), right
 to use, or waiver of any kind with respect to the Software, and Broadcom
 expressly reserves all rights in and to the Software and all intellectual
 property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,

 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use
    all reasonable efforts to protect the confidentiality thereof, and to
    use this information only in connection with your use of Broadcom
    integrated circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
    PERFORMANCE OF THE SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
    LIMITED REMEDY.
:> 
*/

/*
*******************************************************************************
*
* File Name  : runner_mcast.c
*
* Description: This implementation supports the Runner Multicast Flows
*
*******************************************************************************
*/

#if defined(RDP_SIM)
#include "pktrunner_rdpa_sim.h"
#else
#include <linux/module.h>
#include <linux/if_ether.h>
#include <linux/if_vlan.h>
#include <linux/blog_rule.h>

#include <linux/bcm_log.h>
#include <linux/blog.h>
#include <net/ipv6.h>
#include "fcachehw.h"

#include "bcmtypes.h"
#include "bcm_vlan.h"
#endif

#include "cmdlist_api.h"

#include <rdpa_api.h>

#include "pktrunner_proto.h"
#include "pktrunner_mcast.h"
#include "pktrunner_wlan_mcast.h"

#if defined(CONFIG_BCM_CMDLIST_SIM)
#include "runner_sim.h"
#endif

/*******************************************************************************
 *
 * Global Variables and Definitions
 *
 *******************************************************************************/

/* IPv4 Multicast range: 224.0.0.0 to 239.255.255.255 (E0.*.*.* to EF.*.*.*) */
#define RUNNER_MCAST_IPV4_MASK  0xF0000000
#define RUNNER_MCAST_IPV4_VAL   0xE0000000

#define RUNNER_MCAST_IS_MCAST_IPV4(_addr)                                  \
    ( ((_addr) & RUNNER_MCAST_IPV4_MASK) == RUNNER_MCAST_IPV4_VAL )

/* IPv6 Multicast range:  FF00::/8  */
#define RUNNER_MCAST_IPV6_VAL   0xFF

#define RUNNER_MCAST_IS_MCAST_IPV6(_addr)                                \
    ( (_addr)  == RUNNER_MCAST_IPV6_VAL)

#if !defined(CONFIG_BCM_CMDLIST_SIM)
static int mcast_class_created_here   = 0;
static bdmf_object_handle mcast_class = NULL;
#endif

/*******************************************************************************
 *
 * Local Functions
 *
 *******************************************************************************/

static blogRuleAction_t *__findBlogRuleCommand(blogRule_t *blogRule_p,
                                               blogRuleCommand_t blogRuleCommand,
                                               uint32 *cmdIndex_p)
{
    blogRuleAction_t *action_p;
    int i;

    for(i=*cmdIndex_p; i<blogRule_p->actionCount; ++i)
    {
        action_p = &blogRule_p->action[i];

        if(action_p->cmd == blogRuleCommand)
        {
            *cmdIndex_p = i;

            return action_p;
        }
    }

    return NULL;
}

static inline blogRuleAction_t *findBlogRuleCommand(blogRule_t *blogRule_p,
                                                    blogRuleCommand_t blogRuleCommand)
{
    uint32 cmdIndex = 0;

    return __findBlogRuleCommand(blogRule_p, blogRuleCommand, &cmdIndex);
}

static void buildFlowKey(Blog_t *blog_p, rdpa_mcast_flow_key_t *key_p)
{
    uint16_t vlan_id;

#if defined(CC_PKTRUNNER_IPV6)
    if(blog_p->rx.info.bmap.PLD_IPv6)
    {
        key_p->src_ip.family = bdmf_ip_family_ipv6;
        key_p->dst_ip.family = bdmf_ip_family_ipv6;
        if (blog_p->is_ssm) 
        {
            /* Use source to lookup only for SSM flows */
            memcpy(key_p->src_ip.addr.ipv6.data, blog_p->tupleV6.saddr.p8, 16); 
        }
        else
        {
            memset(key_p->src_ip.addr.ipv6.data, 0, 16); 
        }
        memcpy(key_p->dst_ip.addr.ipv6.data, blog_p->tupleV6.daddr.p8, 16);
    }
    else
#endif
    {
        key_p->src_ip.family = bdmf_ip_family_ipv4;
        key_p->dst_ip.family = bdmf_ip_family_ipv4;
        if (blog_p->is_ssm) 
        {
            /* Use source to lookup only for SSM flows */
            key_p->src_ip.addr.ipv4 = ntohl(blog_p->rx.tuple.saddr);
        }
        else
        {
            key_p->src_ip.addr.ipv4 = 0;
        }
        key_p->dst_ip.addr.ipv4 = ntohl(blog_p->rx.tuple.daddr);
    }

    /* L4 protocol is set in the Blog Key for both IPv4 and IPv6 */
    key_p->protocol = blog_p->key.protocol;
    key_p->num_vlan_tags = blog_p->vtag_num;
    vlan_id = ntohl(blog_p->vtag[0]);
    key_p->outer_vlan_id = vlan_id & 0xFFF;
    vlan_id = ntohl(blog_p->vtag[1]);
    key_p->inner_vlan_id = vlan_id & 0xFFF;
}

int buildFlowResult(Blog_t *blog_p, rdpa_mcast_flow_result_t *result_p)
{
    rdpa_mcast_port_context_t *portContext_p;
    cmdlist_mcast_parse_t parse;
    blogRule_t *blogRule_p;
    uint32_t txInfoChannel = __enetLogicalPortToPhysicalPort(blog_p->tx.info.channel);
    int ret;
    rdpa_if rdpa_if_port;

    blogRule_p = blog_p->blogRule_p;
    if(blogRule_p == NULL)
    {
        __logError("NULL Blog Rule");

        return -1;
    }

    __dumpBlogRule(blogRule_p);

    if(__isWlanPhy(blog_p))
    {
        __logInfo("WLAN Multicast\n");

        rdpa_if_port = rdpa_if_lan7;
    }
    else
    {
        __logInfo("Ethernet Multicast\n");

        rdpa_if_port = rdpa_physical_port_to_rdpa_if(txInfoChannel);
    }

    result_p->port_mask = (1 << rdpa_if_port);

    result_p->mtu = blog_getTxMtu(blog_p);
    result_p->is_tos_mangle = 0;
    result_p->tos = blog_p->rx.tuple.tos;
    if ((blog_p->mcast_learn) && (blog_p->rx.info.bmap.PLD_IPv4))
    {
        if(blog_p->rx.tuple.tos != blog_p->tx.tuple.tos)
        {
            result_p->is_tos_mangle = 1;
            result_p->tos = blog_p->rx.tuple.tos;
        }
    }

    memset(result_p->port_context, 0, sizeof(result_p->port_context));

    portContext_p = &result_p->port_context[rdpa_if_port];

    /* Parse Blog Information */

    cmdlist_mcast_parse(blog_p, blogRule_p, &parse);

    portContext_p->state = rdpa_mcast_port_state_cmd_list; /* Only Command List is supported! */
    portContext_p->l2_header_length = parse.rxL2HeaderLength - parse.txAdjust;
    portContext_p->l2_push = (parse.txAdjust < 0) ? 1 : 0;
    portContext_p->l2_offset = abs(parse.txAdjust);
    portContext_p->tc = (uint8_t)__skbMarkToTrafficClass(blog_p->mark);
    portContext_p->lag_port = 0;
    if(!__isTxWlanPhy(blog_p))
    {
        portContext_p->lag_port = __lagPortGet(blog_p);
        if(portContext_p->lag_port < 0)
        {
            __logError("Could not get LAG Port");

            return -1;
        }
    }

    {
        blogRuleAction_t *blogRuleAction_p = findBlogRuleCommand(blogRule_p,
                                                                 BLOG_RULE_CMD_SET_SKB_MARK_QUEUE);

        if(blogRuleAction_p)
        {
            portContext_p->queue = (uint8_t)blogRuleAction_p->arg;
        }
        else
        {
            portContext_p->queue = (uint8_t)__skbMarkToQueuePriority(blog_p->mark);
        }
    }

    /*
     * Create L3 Command List
     */

    __debug("\n*** L3 Command List ***\n\n");

    cmdlist_init(result_p->l3_cmd_list, RDPA_CMD_LIST_MCAST_L3_LIST_SIZE, RDPA_CMD_LIST_MCAST_L3_LIST_OFFSET);

    ret = cmdlist_l3_mcast_create(blog_p, blogRule_p, &parse, &result_p->is_routed);
    if(ret != 0)
    {
        __logError("Could not createL3CommandList");

        return -1;
    }

    result_p->l3_cmd_list_length = cmdlist_get_length();

    __debug("l3_cmd_list_length = %u\n", result_p->l3_cmd_list_length);
    if(isLogDebug)
    {
        cmdlist_dump(result_p->l3_cmd_list, RDPA_CMD_LIST_MCAST_L3_LIST_SIZE_32);
    }

    /*
     * Create L2 Command List
     */

    __debug("\n*** L2 Command List ***\n\n");

    cmdlist_init(portContext_p->port_header.l2_cmd_list, RDPA_CMD_LIST_MCAST_L2_LIST_SIZE, RDPA_CMD_LIST_MCAST_L2_LIST_OFFSET);

    ret = cmdlist_l2_mcast_create(blog_p, blogRule_p, &parse);
    if(ret != 0)
    {
        __logError("Could not createL2CommandList");

        return -1;
    }

    portContext_p->l2_command_list_length = cmdlist_get_length();

    __debug("l2_command_list_length = %u\n", portContext_p->l2_command_list_length);
    if(isLogDebug)
    {
        cmdlist_dump(portContext_p->port_header.l2_cmd_list, RDPA_CMD_LIST_MCAST_L2_LIST_SIZE_32);
    }

    return 0;
}

int runnerMcast_activate(Blog_t *blog_p, int *isActivation_p)
{
    int flowIdx = FHW_TUPLE_INVALID;
    rdpa_mcast_flow_t *mcastFlow_p = NULL;
    rdpa_mcast_flow_t *currMcastFlow_p = NULL;
#if !defined(CONFIG_BCM_CMDLIST_SIM)
    bdmf_index index = FHW_TUPLE_INVALID;
#endif
    int ret;

    BCM_ASSERT(blog_p != BLOG_NULL);

    __logInfo("ACTIVATE");

    __debug("\n%s: ************** Multicast Flow **************\n\n", __FUNCTION__);

    if(blog_p->rx.info.channel == 0xFF)
    {
        __logInfo("LAN to LAN Multicast acceleration is not supported\n");

        goto out;
    }

    mcastFlow_p = __mcastFlowMalloc();
    if(mcastFlow_p == NULL)
    {
        __logError("Could not __mcastFlowMalloc");

        goto out;
    }

    buildFlowKey(blog_p, &mcastFlow_p->key);

    ret = buildFlowResult(blog_p, &mcastFlow_p->result);
    if(ret)
    {
        __logError("Could not buildFlowResult");

        goto out;
    }

    *isActivation_p = 0;

#if defined(CONFIG_BCM_CMDLIST_SIM)
    {
        rdpa_mcast_port_context_t *portContext_p =
            &mcastFlow_p->result.port_context[blog_p->tx.info.channel];
        runnerSim_mcastFlowKey_t simKey;

        simKey.ipDest = blog_p->rx.tuple.daddr;

        runnerSim_mcastActivate(blog_p, blog_p->tx.info.channel,
                                &simKey,
                                portContext_p->l2_header_length,
                                portContext_p->l2_push,
                                portContext_p->l2_offset,
                                mcastFlow_p->result.l3_cmd_list,
                                portContext_p->port_header.l2_cmd_list);

        flowIdx = 0;
    }

#else /* !CONFIG_BCM_CMDLIST_SIM */

    if(!RUNNER_MCAST_IS_MCAST_IPV4(mcastFlow_p->key.dst_ip.addr.ipv4)
#if defined(CC_PKTRUNNER_IPV6)
       && !RUNNER_MCAST_IS_MCAST_IPV6(blog_p->tupleV6.daddr.p8[0])
#endif
        )
    {
        __logError("Not IPv4 or IPv6 Multicast : %pI4, %pI6",
                   &blog_p->rx.tuple.daddr, blog_p->tupleV6.daddr.p8);

        ret = BDMF_ERR_INVALID_OP;

        goto out;
    }

    ret = rdpa_mcast_flow_find(mcast_class, &index, mcastFlow_p);
    if(ret < 0)
    {
        index = BDMF_ERR_NOENT;
    }

    if(blog_p->mc_sync)
    {
        __logInfo("SYNC: dev=%pS >>>>", blog_p->tx_dev_p);

        if(index == BDMF_ERR_NOENT)
        {
            __logError("SYNC: Mcast Flow does not exist");

            goto out;
        }

#if 1
        __logError("SYNC: Operation not supported");

        ret = BDMF_ERR_NOT_SUPPORTED;

        goto out;
#else
        {
            /* Save current flow index */

            bdmf_index currIndex = index;

            /* Create new Flow */

            ret = rdpa_mcast_flow_add(mcast_class, &index, &mcastFlow);
            if(ret < 0)
            {
                __logError("SYNC: Could not rdpa_mcast_flow_add, ret = %d", ret);

                goto out;
            }

            /* Delete current flow */

            ret = rdpa_mcast_flow_delete(mcast_class, currIndex);
            if(ret < 0)
            {
                __logError("SYNC: Could not rdpa_mcast_flow_delete, ret = %d", ret);

                goto out;
            }

            __logInfo("SYNC : SUCCESSFUL <<<<");
        }
#endif
    }
    else if(index == BDMF_ERR_NOENT)
    {
        __logInfo("CREATE: dev=%pS >>>>", blog_p->tx_dev_p);

        *isActivation_p = 1;

        /* Create new Flow */

        mcastFlow_p->result.wlan_mcast_fwd_table_index = RDPA_WLAN_MCAST_FWD_TABLE_INDEX_INVALID;

        if(__isWlanPhy(blog_p))
        {
            ret = pktrunner_wlan_mcast_add(blog_p, &mcastFlow_p->result.wlan_mcast_fwd_table_index);
            if(ret < 0)
            {
                __logError("CREATE: Could not pktrunner_wlan_mcast_add, ret = %d", ret);

                goto out;
            }

            __logInfo("WLAN MCAST FWD Table ADD: %ld", mcastFlow_p->result.wlan_mcast_fwd_table_index);

            mcastFlow_p->result.wlan_mcast_clients = 1;
            mcastFlow_p->result.number_of_ports = 0;
        }
        else
        {
            mcastFlow_p->result.wlan_mcast_clients = 0;
            mcastFlow_p->result.number_of_ports = 1;
        }

        ret = rdpa_mcast_flow_add(mcast_class, &index, mcastFlow_p);
        if(ret < 0)
        {
            __logError("CREATE: Could not rdpa_mcast_flow_add, ret = %d", ret);

            goto out;
        }
        else if(index == FHW_TUPLE_INVALID)
	    {
            __logInfo("CREATE: Could not rdpa_mcast_flow_add: collision list full");

            goto out;
        }

        __logInfo("CREATE: SUCCESSFUL <<<<");
    }
    else /* mcast flow already exists */
    {
        rdpa_if rdpa_if_port;

        __logInfo("ADD_PORT dev=%pS >>>>", blog_p->tx_dev_p);

        currMcastFlow_p = __mcastFlowMalloc();
        if(currMcastFlow_p == NULL)
        {
            __logError("Could not __mcastFlowMalloc");

            goto out;
        }

        /* Get current Multicast Flow */

        ret = rdpa_mcast_flow_get(mcast_class, index, currMcastFlow_p);
        if(ret < 0)
        {
            __logError("ADD_PORT Could not rdpa_mcast_flow_get, ret = %d", ret);

            goto out;
        }

        if(__isWlanPhy(blog_p))
        {
            ret = pktrunner_wlan_mcast_add(blog_p, &currMcastFlow_p->result.wlan_mcast_fwd_table_index);
            if(ret < 0)
            {
                __logError("CREATE: Could not pktrunner_wlan_mcast_add, ret = %d", ret);

                goto out;
            }

            rdpa_if_port = rdpa_if_lan7;

            currMcastFlow_p->result.wlan_mcast_clients++;
        }
        else // Ethernet
        {
            uint32_t txInfoChannel = __enetLogicalPortToPhysicalPort(blog_p->tx.info.channel);

            rdpa_if_port = rdpa_physical_port_to_rdpa_if(txInfoChannel);

            BCM_ASSERT(rdpa_if_port < rdpa_if_max_mcast_port);

            if(currMcastFlow_p->result.port_mask & mcastFlow_p->result.port_mask)
            {
                __logError("ADD_PORT Port 0x%02X has already been added", currMcastFlow_p->result.port_mask);

                goto out;
            }

            currMcastFlow_p->result.number_of_ports++;
        }

        /* Add new port to current Multicast Flow */

        currMcastFlow_p->result.port_mask |= mcastFlow_p->result.port_mask;

        currMcastFlow_p->result.port_context[rdpa_if_port] =
            mcastFlow_p->result.port_context[rdpa_if_port];

        /* Use the smallest MTU among the member Ports */

        if(currMcastFlow_p->result.mtu < mcastFlow_p->result.mtu)
        {
            currMcastFlow_p->result.mtu = mcastFlow_p->result.mtu;
        }

        /* Set updated Multicast Flow */

        ret = rdpa_mcast_flow_set(mcast_class, index, currMcastFlow_p);
        if(ret < 0)
        {
            __logError("ADD_PORT Could not rdpa_mcast_flow_set, ret = %d", ret);

            goto out;
        }

        __logInfo("ADD_PORT SUCCESSFUL <<<<");
    }

    /* if we reach here, the activation succeeded */
    flowIdx = (int)index;

#endif /* CONFIG_BCM_CMDLIST_SIM */

out:
    if(mcastFlow_p != NULL)
    {
        __mcastFlowFree(mcastFlow_p);
    }

    if(currMcastFlow_p != NULL)
    {
        __mcastFlowFree(currMcastFlow_p);
    }

    return flowIdx;
}

#if !defined(CONFIG_BCM_CMDLIST_SIM)
int runnerMcast_deactivate(Blog_t *blog_p, int *isDeactivation_p)
{
    rdpa_mcast_flow_t *mcastFlow_p = NULL;
    bdmf_index index;
    int ret;

    BCM_ASSERT(blog_p != BLOG_NULL);

    __logInfo("DEACTIVATE");

    mcastFlow_p = __mcastFlowMalloc();
    if(mcastFlow_p == NULL)
    {
        __logError("Could not __mcastFlowMalloc");

        goto out;
    }

    mcastFlow_p->result.number_of_ports = -1;
    mcastFlow_p->result.wlan_mcast_clients = -1;

    buildFlowKey(blog_p, &mcastFlow_p->key);

    *isDeactivation_p = 0;

    ret = rdpa_mcast_flow_find(mcast_class, &index, mcastFlow_p);
    if(ret < 0)
    {
        __logError("Cannot rdpa_mcast_flow_find, ret = %d", ret);

        goto out;
    }

    ret = rdpa_mcast_flow_get(mcast_class, index, mcastFlow_p);
    if(ret < 0)
    {
        __logError("Cannot rdpa_mcast_flow_get, ret = %d", ret);

        goto out;
    }

    /* Remove port from mcast flow */

    if((mcastFlow_p->result.number_of_ports +
        mcastFlow_p->result.wlan_mcast_clients) == 1)
    {
        __logInfo("DELETE: >>>>");

        *isDeactivation_p = 1;

        if(__isWlanPhy(blog_p))
        {
            BCM_ASSERT(mcastFlow_p->result.wlan_mcast_clients == 1);

            mcastFlow_p->result.wlan_mcast_clients--;

            __logInfo("WLAN MCAST FWD Table DELETE: %ld", mcastFlow_p->result.wlan_mcast_fwd_table_index);

            ret = pktrunner_wlan_mcast_delete(blog_p, &mcastFlow_p->result.wlan_mcast_fwd_table_index);
            if(ret < 0)
            {
                __logError("WLAN: Could not pktrunner_wlan_mcast_delete, ret = %d", ret);

                goto out;
            }

            BCM_ASSERT(mcastFlow_p->result.wlan_mcast_fwd_table_index ==
                       RDPA_WLAN_MCAST_FWD_TABLE_INDEX_INVALID);
        }
        else
        {
            BCM_ASSERT(mcastFlow_p->result.number_of_ports == 1);

            mcastFlow_p->result.number_of_ports--;
        }

        ret = rdpa_mcast_flow_delete(mcast_class, index);
        if(ret < 0)
        {
            __logError("DELETE: Could not rdpa_mcast_flow_delete, ret = %d", ret);

            goto out;
        }

        __logInfo("DELETE: SUCCESSFUL <<<<");
    }
    else
    {
        if(__isWlanPhy(blog_p))
        {
            mcastFlow_p->result.wlan_mcast_clients--;

            if(!mcastFlow_p->result.wlan_mcast_clients)
            {
                mcastFlow_p->result.port_mask &= ~(1 << rdpa_if_lan7);
            }

            ret = pktrunner_wlan_mcast_delete(blog_p, &mcastFlow_p->result.wlan_mcast_fwd_table_index);
            if(ret < 0)
            {
                __logError("WLAN: Could not pktrunner_wlan_mcast_delete, ret = %d", ret);

                goto out;
            }
        }
        else
        {
            uint32_t txInfoChannel = __enetLogicalPortToPhysicalPort(blog_p->tx.info.channel);
            rdpa_if rdpa_if_port = rdpa_physical_port_to_rdpa_if(txInfoChannel);

            if(!(mcastFlow_p->result.port_mask & (1 << rdpa_if_port)))
            {
                __logError("Port <%u:%d> has not joined flow %lu", txInfoChannel, rdpa_if_port, index);

                ret = BDMF_ERR_NOENT;

                goto out;
            }

            __logInfo("REM_PORT: dev=%pS >>>>", blog_p->tx_dev_p);

            mcastFlow_p->result.number_of_ports--;

            /* Remove port from Multicast Flow */

            mcastFlow_p->result.port_mask &= ~(1 << rdpa_if_port);
        }

        /* Set updated Multicast Flow */

        ret = rdpa_mcast_flow_set(mcast_class, index, mcastFlow_p);
        if(ret < 0)
        {
            __logError("REM_PORT Could not rdpa_mcast_flow_set, ret = %d", ret);

            goto out;
        }

        __logInfo("REM_PORT: SUCCESSFUL <<<<");
    }

out:
    if(mcastFlow_p != NULL)
    {
        int number_of_ports = (mcastFlow_p->result.number_of_ports +
                               mcastFlow_p->result.wlan_mcast_clients);
        __mcastFlowFree(mcastFlow_p);

        /* return number of associations to the mcast flow */
        return number_of_ports;
    }
    else
    {
        return -1;
    }
}

static void mcastDeleteAllFlows(void)
{
    rdpa_mcast_flow_t *mcastFlow_p;
    bdmf_index index;
    int rc;

    mcastFlow_p = __mcastFlowMalloc();
    if(mcastFlow_p == NULL)
    {
        __logError("Could not __mcastFlowMalloc");

        return;
    }

    for(index=0; index<RDPA_MCAST_MAX_FLOWS; ++index)
    {
        rc = rdpa_mcast_flow_get(mcast_class, index, mcastFlow_p);
        if(rc)
        {
            continue;
        }

        __logInfo("Deleted Multicast Flow Index %ld", index);

        rc = rdpa_mcast_flow_delete(mcast_class, index);
        if (rc)
        {
            __logError("Could not rdpa_mcast_flow_delete, index %ld, error=%d\n", index, rc);
        }
    }

    __mcastFlowFree(mcastFlow_p);
}

/*
 *------------------------------------------------------------------------------
 * Function   : runnerMcast_refresh
 * Description: This function is invoked to collect flow statistics
 * Parameters :
 *  tuple : 16bit index to refer to a Runner flow
 * Returns    : Total hits on this connection.
 *------------------------------------------------------------------------------
 */
int runnerMcast_refresh(int flowIdx, uint32_t *pktsCnt_p, uint32_t *octetsCnt_p)
{
    rdpa_stat_t flow_stat;
    int rc;
    rc = rdpa_mcast_flow_stat_get(mcast_class, flowIdx, &flow_stat);
    if (rc < 0)
    {
        __logError("Could not get flowIdx<%d> stats, rc %d", flowIdx, rc);
        return rc;
    }

    *pktsCnt_p = flow_stat.packets; /* cummulative packets */
    *octetsCnt_p = flow_stat.bytes;

    __logDebug( "flowIdx<%03u> "
                "cumm_pkt_hits<%u> cumm_octet_hits<%u>\n",
                flowIdx, *pktsCnt_p, *octetsCnt_p );
    return 0;
}

#else  /* CONFIG_BCM_CMDLIST_SIM */

int runnerMcast_deactivate(Blog_t *blog_p, int *isDeactivation_p)
{
    return 0;
}

int runnerMcast_refresh(int flowIdx, uint32_t *pktsCnt_p, uint32_t *octetsCnt_p)
{
    *pktsCnt_p = 1;
    *octetsCnt_p = 1;

    return 0;
}

#endif /* CONFIG_BCM_CMDLIST_SIM */

/*
*******************************************************************************
* Function   : runnerMcast_construct
* Description: Constructs the Runner Protocol layer
*******************************************************************************
*/
int __init runnerMcast_construct(void)
{
#if !defined(CONFIG_BCM_CMDLIST_SIM)
    int ret;
    BDMF_MATTR(mcast_attrs, rdpa_mcast_drv());

    ret = rdpa_mcast_get(&mcast_class);
    if (ret)
    {
        ret = bdmf_new_and_set(rdpa_mcast_drv(), NULL, mcast_attrs, &mcast_class);
        if (ret)
        {
            BCM_LOG_ERROR(BCM_LOG_ID_PKTRUNNER, "rdpa mcast class object does not exist and can't be created.\n");
            return ret;
        }
        mcast_class_created_here = 1;
    }

    __print("Initialized Runner Multicast Layer\n");
#endif

    return 0;
}

/*
*******************************************************************************
* Function   : runnerMcast_destruct
* Description: Destructs the Runner Protocol layer
*******************************************************************************
*/
void __exit runnerMcast_destruct(void)
{
#if !defined(CONFIG_BCM_CMDLIST_SIM)
    if (mcast_class)
    {
        mcastDeleteAllFlows();

        if (mcast_class_created_here)
        {
            bdmf_destroy(mcast_class);
            mcast_class_created_here = 0;
        }
        else
        {
            bdmf_put(mcast_class);
        }
    }
#endif
}

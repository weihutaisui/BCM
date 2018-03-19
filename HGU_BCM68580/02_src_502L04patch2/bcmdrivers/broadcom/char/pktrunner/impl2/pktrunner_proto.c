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
* File Name  : ptkrunner_proto.c
*
* Description: This implementation supports the dynamically learnt Flows in
*              xDSL platforms.
*
*******************************************************************************
*/

#if defined(RDP_SIM)
#include "pktrunner_rdpa_sim.h"
#else
#include <linux/module.h>
#include <linux/bcm_log.h>
#include <linux/blog.h>
#include <net/ipv6.h>
#include "fcachehw.h"
#include "bcmenet.h"
#include "clk_rst.h"
#endif

#include <rdpa_api.h>

#include "pktrunner_proto.h"
#include "pktrunner_ucast.h"
#include "pktrunner_mcast.h"
#include "pktrunner_wlan_mcast.h"
#include "pktrunner_l2_ucast.h"

#include "cmdlist_api.h"

#if defined(CONFIG_BCM963138) || defined(CONFIG_BCM963148) || defined(CONFIG_BCM94908)
#include "wfd_dev.h"
#endif

#if !defined(RDP_SIM)
#include "linux/bcm_skb_defines.h"
#endif

#ifndef ARRAYSIZE
#define ARRAYSIZE(a)  (sizeof(a)/sizeof(a[0]))
#endif

/*******************************************************************************
 *
 * Global Variables and Definitions
 *
 *******************************************************************************/

typedef struct {
    uint32_t status;        /* status: Enable=1 or Disable=0 */
    uint32_t activates;     /* number of activate (downcalls)   */
    uint32_t failures;      /* number of activate failures      */
    uint32_t deactivates;   /* number of deactivate (downcalls) */
    uint32_t flushes;       /* number of clear (upcalls)        */
    uint32_t active;
} pktRunner_state_t;

#ifdef XRDP
#define RDPA_MAX_IP_FLOW 65536 
#else
#define RDPA_MAX_IP_FLOW 16512
#endif

static pktRunner_state_t pktRunner_state_g;   /* Protocol layer global context */
static bdmf_number       runnerFlowResetStats_g[RDPA_MAX_IP_FLOW] = {}; /* stats when last reset per flow */
static int runnerClearRstStats(uint32_t tuple);

typedef union {
    bdmf_number    num;
    rdpa_stat_t    rdpastat;
} pktRunner_flowStat_t;


/*******************************************************************************
 *
 * Public API
 *
 *******************************************************************************/

int runnerActivate(Blog_t *blog_p)
{
    int flowIdx;

    BCM_ASSERT(blog_p != BLOG_NULL);

    __debug("\n::: runner_activate :::\n\n");
    __dumpBlog(blog_p);

//    __debug("\n%s: ************** New Flow **************\n\n", __FUNCTION__);

#if !defined(CONFIG_BCM963158)
    /* XXX: add GRE */
    if(blog_p->rx.info.bmap.PLD_L2 )
    {
        flowIdx = runnerL2Ucast_activate(blog_p);

        if(flowIdx == FHW_TUPLE_INVALID)
        {
            __logInfo("Could not runnerL2Ucast_activate");

            goto abort_activate;
        }

        pktRunner_state_g.activates++;
        pktRunner_state_g.active++;

        __debug("::: runnerL2Ucast_activate: flowIdx <%u>, cumm_activates <%u> :::\n\n",
                flowIdx, pktRunner_state_g.activates);

        runnerClearRstStats(flowIdx);

        return flowIdx;
    } 
    else
#endif
    {
        /* XXX: add GRE */
        if((blog_p->key.protocol != IPPROTO_UDP) &&
           (blog_p->key.protocol != IPPROTO_TCP) &&
           (blog_p->key.protocol != IPPROTO_IPV6) &&
           (blog_p->key.protocol != IPPROTO_IPIP))
        {
            __logInfo("Flow Type proto<%d> is not supported", blog_p->key.protocol);

            goto abort_activate;
        }

        if(blog_p->rx.multicast)
        {
            int isActivation = 0;

#if defined(CC_PKTRUNNER_MCAST)
            flowIdx = runnerMcast_activate(blog_p, &isActivation);
#else
            flowIdx = FHW_TUPLE_INVALID;
#endif
            if(flowIdx == FHW_TUPLE_INVALID)
            {
                __logInfo("Could not runnerMcast_activate");

                goto abort_activate;
            }

            if(isActivation)
            {
                pktRunner_state_g.activates++;
                pktRunner_state_g.active++;
            }

            __debug("::: runnerMcast_activate: flowIdx <%u>, cumm_activates <%u> :::\n\n",
                    flowIdx, pktRunner_state_g.activates);

            return flowIdx;
        }
        else
        {
            flowIdx = runnerUcast_activate(blog_p);

            if(flowIdx == FHW_TUPLE_INVALID)
            {
                __logInfo("Could not runnerUcast_activate");

                goto abort_activate;
            }

            pktRunner_state_g.activates++;
            pktRunner_state_g.active++;

        	runnerClearRstStats(flowIdx);
            __debug("::: runnerUcast_activate: flowIdx <%u>, cumm_activates <%u> :::\n\n",
                    flowIdx, pktRunner_state_g.activates);

            return flowIdx;
        }
    }

abort_activate:
    pktRunner_state_g.failures++;
    __logInfo("cumm_failures<%u>", pktRunner_state_g.failures);

    return FHW_TUPLE_INVALID;
}

/*
 *------------------------------------------------------------------------------
 * Function   : runnerClearRstStats
 * Description: This function is invoked to clear the reset stats of a flow
 * Parameters :
 *  tuple : 16bit index to refer to a Runner flow
 * Returns    : Total hits on this connection.
 *------------------------------------------------------------------------------
 */
 
static int runnerClearRstStats(uint32_t tuple) {
    int flowIdx = runner_get_hw_entix(tuple);
    if (flowIdx < 0 || flowIdx >= ARRAYSIZE(runnerFlowResetStats_g))
        return -1;
    runnerFlowResetStats_g[flowIdx] = 0; 
    return 0;
}

#if !defined(RDP_SIM)
/*
 *------------------------------------------------------------------------------
 * Function   : runnerRefresh
 * Description: This function is invoked to collect flow statistics
 * Parameters :
 *  tuple : 16bit index to refer to a Runner flow
 * Returns    : Total hits on this connection.
 *------------------------------------------------------------------------------
 */
static int runnerRefresh(uint32_t tuple, uint32_t *pktsCnt_p, uint32_t *octetsCnt_p)
{
    int flowIdx = runner_get_hw_entix(tuple);
    int rc;

    pktRunner_flowStat_t rawStat;
    pktRunner_flowStat_t resetStat;

    if (flowIdx < 0 || flowIdx >= ARRAYSIZE(runnerFlowResetStats_g))
        return -1;

    rc = runnerUcast_refresh(flowIdx, &rawStat.rdpastat.packets, &rawStat.rdpastat.bytes);
    if (rc < 0)
    {
        rc = runnerL2Ucast_refresh(flowIdx, &rawStat.rdpastat.packets, &rawStat.rdpastat.bytes);
        if(rc < 0)
        {
#if defined(CC_PKTRUNNER_MCAST)
            rc = runnerMcast_refresh(flowIdx, &rawStat.rdpastat.packets, &rawStat.rdpastat.bytes);
            if(rc < 0)
#endif
            {
                __logError("Could not get flowIdx<%d> stats, rc %d", flowIdx, rc);

                return rc;
            }
        }
    }

    resetStat.num = runnerFlowResetStats_g[flowIdx];

    *pktsCnt_p = rawStat.rdpastat.packets-resetStat.rdpastat.packets;
    *octetsCnt_p = rawStat.rdpastat.bytes-resetStat.rdpastat.bytes;

    return 0;
}

/*
 *------------------------------------------------------------------------------
 * Function   : runnerDeactivate
 * Description: This function is invoked when a Runner flow needs to be
 *              deactivated.
 * Parameters :
 *  tuple     : 16bit index to refer to a flow in HW
 *  blog_p    : pointer to a blog object (for multicast only)
 * Returns    : Remaining number of active ports (for multicast only)
 *------------------------------------------------------------------------------
 */
#if !defined(CONFIG_BCM_CMDLIST_SIM)
static int runnerDeactivate(uint32_t tuple, uint32_t *pktsCnt_p,
                            uint32_t *octetsCnt_p, struct blog_t *blog_p)
{
    int isDeactivation = 1;
    int rc;

    __debug("\n::: runnerDeactivate :::\n\n");
    __dumpBlog(blog_p);

#if !defined(CONFIG_BCM963158)
    if (blog_p->rx.info.bmap.PLD_L2)
    {
        /* Fetch last hit count */
        rc = runnerRefresh(tuple, pktsCnt_p, octetsCnt_p);
        if (rc < 0)
        {
            goto abort_deactivate;
        }

        rc = runnerL2Ucast_deactivate(tuple);
        if(rc)
        {
            goto abort_deactivate;
        }
    }
    else
#endif
    {
        /* Fetch last hit count */
        rc = runnerRefresh(tuple, pktsCnt_p, octetsCnt_p);
        if (rc < 0)
        {
            goto abort_deactivate;
        }

        if(blog_p->rx.multicast)
        {
#if defined(CC_PKTRUNNER_MCAST)
            rc = runnerMcast_deactivate(blog_p, &isDeactivation);
#else
            rc = -1;
#endif
            if(rc)
            {
                goto abort_deactivate;
            }
        }
        else
        {
            rc = runnerUcast_deactivate(tuple);
            if(rc)
            {
                goto abort_deactivate;
            }
        }
    }

    if(isDeactivation)
    {
        runnerClearRstStats(tuple);
        pktRunner_state_g.deactivates++;
        pktRunner_state_g.active--;
    }

    __logDebug("::: runnerDeactivate flowIx<%03u> hits<%u> bytes<%u> cumm_deactivates<%u> :::\n",
               tuple, *pktsCnt_p, *octetsCnt_p, pktRunner_state_g.deactivates);

    return rc;

abort_deactivate:
    pktRunner_state_g.failures++;

    return rc;
}

/*
 *------------------------------------------------------------------------------
 * Function   : runnerUpdate
 * Description: This function is invoked when a Runner flow needs to be
 *              updated.
 * Parameters :
 *  blog_p    : pointer to a blog object
 *  tuple     : 16bit index to refer to a flow in HW
 * Returns    : 0 on success.
 *------------------------------------------------------------------------------
 */
int runnerUpdate(BlogUpdate_t update, uint32_t tuple, Blog_t *blog_p)
{
    if(blog_p->rx.info.bmap.PLD_L2 )
    {
        return runnerL2Ucast_update(update, tuple, blog_p);
    }
    else
    {
        if(blog_p->rx.multicast)
        {
            __logError("Multicast flow updates are not supported");
        }
        else
        {
            return runnerUcast_update(update, tuple, blog_p);
        }
    }

    return -1;
}
#else
static int runnerDeactivate(uint16_t tuple, uint32_t *pktsCnt_p,
                            uint32_t *octetsCnt_p, struct blog_t *blog_p)
{
    return 0;
}

int runnerUpdate(BlogUpdate_t update, uint32_t tuple, Blog_t *blog_p)
{
    return 0;
}
#endif /* CONFIG_BCM_CMDLIST_SIM */
#endif /* !defined(RDP_SIM) */

/*
 *------------------------------------------------------------------------------
 * Function   : runnerResetAllStats
 * Description: This function is invoked to reset stats for all flows
 * Parameters :
 *  hwTuple: 16bit index to refer to a Runner flow
 * Returns    : 0 on success.
 *
 * Assumption: bdmf_number lines up in size and offsets with rdpa_stat_t 
 *------------------------------------------------------------------------------
 */
int runnerResetAllStats(void)
{
    int rc, idx;
    pktRunner_flowStat_t flowStat;

    for (idx = 0; idx < RDPA_MAX_IP_FLOW; idx++)
    {
        rc = runnerUcast_refresh(idx, &flowStat.rdpastat.packets, &flowStat.rdpastat.bytes);
        if (rc < 0)
        {
            rc = runnerL2Ucast_refresh(idx, &flowStat.rdpastat.packets, &flowStat.rdpastat.bytes);
#if defined(CC_PKTRUNNER_MCAST)
            if(rc < 0)
            {
                rc = runnerMcast_refresh(idx, &flowStat.rdpastat.packets, &flowStat.rdpastat.bytes);
            }
#endif
        }

        if (rc == 0)
        {
            runnerFlowResetStats_g[idx] = flowStat.num;
        }
    }

    return rc < 0;
}

/*
 *------------------------------------------------------------------------------
 * Function   : runnerResetStats
 * Description: This function is invoked to reset stats for a flow
 * Parameters :
 *  hwTuple: 16bit index to refer to a Runner flow
 * Returns    : 0 on success.
 * 
 * Assumption: bdmf_number lines up in size and offsets with rdpa_stat_t 
 *------------------------------------------------------------------------------
 */
int runnerResetStats(uint16_t tuple)
{
    int           rc;
    bdmf_index    idx;
    pktRunner_flowStat_t flowStat;

    idx = runner_get_hw_entix(tuple);
    if (idx < 0 || idx >= ARRAYSIZE(runnerFlowResetStats_g)) {
        return 1;
    }

    rc = runnerUcast_refresh(idx, &flowStat.rdpastat.packets, &flowStat.rdpastat.bytes);
    if (rc < 0)
    {
        rc = runnerL2Ucast_refresh(idx, &flowStat.rdpastat.packets, &flowStat.rdpastat.bytes);
#if defined(CC_PKTRUNNER_MCAST)
        if(rc < 0)
        {
            rc = runnerMcast_refresh(idx, &flowStat.rdpastat.packets, &flowStat.rdpastat.bytes);
        }
#endif
    }

    if (rc == 0)
    {
        runnerFlowResetStats_g[idx] = flowStat.num;
    }

    return rc < 0;
}


/*
 *------------------------------------------------------------------------------
 * Function   : runnerGetAdjustedStats
 * Description: This function is invoked read the stats.  This takes the last
 *              reset point into consideration..
 * Parameters :
 *      none
 * Returns    : Total hits on this connection.
 *------------------------------------------------------------------------------
 */
int runnerGetAdjustedStats(bdmf_object_handle mo_, rdpa_filter_stats_key_t * ai_, bdmf_number *stats_) {
    bdmf_index idx;
    int rc;
    pktRunner_flowStat_t flowStat;
    pktRunner_flowStat_t resetStat;

    // yuck... this line is ugly on many different levels
    // for now, casting the pointer to a long on purpose...
    idx = (bdmf_index)ai_;

    if (idx < 0 || idx >= ARRAYSIZE(runnerFlowResetStats_g)) {
        return 1;
    }

    rc = rdpa_filter_stats_get(mo_, ai_, &flowStat.num);
    if (rc) {
        return 1;
    }

    resetStat.num = (bdmf_number)runnerFlowResetStats_g[idx];

    flowStat.rdpastat.packets -= resetStat.rdpastat.packets;
    flowStat.rdpastat.bytes -= resetStat.rdpastat.bytes;

    *stats_ = flowStat.num;
    
    return 0;
}


/******************************************************************
 *
 * Flow Cache Binding
 *
 *****************************************************************/

#if defined(CONFIG_BCM_FHW)

static FC_CLEAR_HOOK fhw_clear_hook_fp = NULL;

/*
 *------------------------------------------------------------------------------
 * Function   : __clearFCache
 * Description: Clears FlowCache association(s) to Runner entries.
 *              This local function MUST be called with the Protocol Layer
 *              Lock taken.
 *------------------------------------------------------------------------------
 */
static int __clearFCache(uint32_t key, const FlowScope_t scope)
{
    int count = 0;

    /* Upcall into FlowCache */
    if(fhw_clear_hook_fp != NULL)
    {
        pktRunner_state_g.flushes += fhw_clear_hook_fp(key, scope);
    }


    // FIXME

//    count = fapPkt_deactivateAll();


    __debug("key<%03u> scope<%s> cumm_flushes<%u>",
            key,
            (scope == System_e) ? "System" : "Match",
            pktRunner_state_g.flushes);

    return count;
}
#endif

/*
 *------------------------------------------------------------------------------
 * Function   : runnerClear
 * Description: This function is invoked when all entries pertaining to
 *              a tuple in Runner need to be cleared.
 * Parameters :
 *  tuple: FHW Engine instance and match index
 * Returns    : success
 *------------------------------------------------------------------------------
 */
int runnerClear(uint16_t tuple)
{
    return 0;
}

/*
 *------------------------------------------------------------------------------
 * Function   : runnerEnable
 * Description: Binds the Runner Protocol Layer handler functions to the
 *              Flow Cache hooks.
 *------------------------------------------------------------------------------
 */
void runnerEnable(void)
{
#if defined(CONFIG_BCM_FHW)
    FhwBindHwHooks_t hwHooks;
    FhwHwAccPrio_t prioIx = FHW_PRIO_0;

    /* Block flow-cache from packet processing and try to push the flows */
    blog_lock();

    hwHooks.activate_fn = (HOOKP)runnerActivate;
    hwHooks.deactivate_fn = (HOOK4PARM)runnerDeactivate;
    hwHooks.update_fn = (HOOK3PARM)runnerUpdate;
    hwHooks.refresh_fn = (HOOK3PARM)runnerRefresh;
    hwHooks.fhw_clear_fn = &fhw_clear_hook_fp;
    hwHooks.reset_stats_fn =(HOOK32) runnerResetStats; 
    hwHooks.get_hw_entix_fn = (HOOK32)runner_get_hw_entix;
    hwHooks.cap = (1<<HW_CAP_IPV4_UCAST);

#if !defined(CONFIG_BCM963158)
    hwHooks.cap |= (1<<HW_CAP_L2_UCAST);
#endif

#if defined(CC_PKTRUNNER_MCAST)
    hwHooks.cap |= (1<<HW_CAP_IPV4_MCAST);
#endif

#if defined(CC_PKTRUNNER_IPV6)
    hwHooks.cap |= ((1<<HW_CAP_IPV6_UCAST) | (1<<HW_CAP_IPV6_TUNNEL));
#if defined(CC_PKTRUNNER_MCAST)
    hwHooks.cap |= (1<<HW_CAP_IPV6_MCAST);
#endif
#endif

#if !defined(CONFIG_BCM_CMDLIST_SIM) && !defined(CONFIG_BCM963158)
    hwHooks.max_ent = (RDPA_UCAST_MAX_FLOWS < FHW_MAX_ENT) ? RDPA_UCAST_MAX_FLOWS : FHW_MAX_ENT;
#else
    hwHooks.max_ent = FHW_MAX_ENT;
#endif

    /* Bind to fc HW layer for learning connection configurations dynamically */
    hwHooks.clear_fn = (HOOK32)runnerClear;

    fhw_bind_hw(prioIx, &hwHooks);

    BCM_ASSERT(fhw_clear_hook_fp != NULL);

    pktRunner_state_g.status = 1;

    blog_unlock();
    __print("Enabled Runner binding to Flow Cache\n");
#else
    __print("Flow Cache is not built\n");
#endif
}


/*
 *------------------------------------------------------------------------------
 * Function   : runnerDisable
 * Description: Clears all active Flow Cache associations with Runner.
 *              Unbind all flow cache to Runner hooks.
 *------------------------------------------------------------------------------
 */
void runnerDisable(void)
{
#if defined(CONFIG_BCM_FHW)
    FhwBindHwHooks_t hwHooks;
    FhwHwAccPrio_t prioIx = FHW_PRIO_0;

    /* Block flow-cache from packet processing and try to push the flows */
    blog_lock(); 

    /* Clear system wide active FlowCache associations, and disable learning. */

    __clearFCache(0, System_e);

    hwHooks.activate_fn = (HOOKP)NULL;
    hwHooks.deactivate_fn = (HOOK4PARM)NULL;
    hwHooks.update_fn = (HOOK3PARM)NULL;
    hwHooks.refresh_fn = (HOOK3PARM)NULL;
    hwHooks.fhw_clear_fn = &fhw_clear_hook_fp;
    hwHooks.reset_stats_fn =(HOOK32) runnerResetStats; 
    hwHooks.get_hw_entix_fn = (HOOK32)NULL; 
    hwHooks.cap = 0;
    hwHooks.max_ent = 0;
    hwHooks.clear_fn = (HOOK32)NULL;

    fhw_bind_hw(prioIx, &hwHooks);

    fhw_clear_hook_fp = (FC_CLEAR_HOOK)NULL;

    pktRunner_state_g.status = 0;

    blog_unlock();

    __print("Disabled Runner binding to Flow Cache\n");
#else
    __print("Flow Cache is not built\n");
#endif
}

/*
*******************************************************************************
* Function   : runnerProto_construct
* Description: Constructs the Runner Protocol layer
*******************************************************************************
*/
int __init runnerProto_construct(void)
{
    int ret;
    
    memset(&pktRunner_state_g, 0, sizeof(pktRunner_state_t));

    ret = runnerUcast_construct();
    if (ret)
    {
        return ret;
    }

    ret = runnerL2Ucast_construct();
    if (ret)
    {
        runnerUcast_destruct();
        return ret;
    }

#if defined(CC_PKTRUNNER_MCAST)
    ret = runnerMcast_construct();
    if (ret)
    {
        runnerL2Ucast_destruct();
        runnerUcast_destruct();

        return ret;
    }

#if !defined(CONFIG_BCM963158)
#if !defined(CONFIG_BCM_CMDLIST_SIM)
    ret = pktrunner_wlan_mcast_construct();
    if (ret)
    {
        runnerMcast_destruct();
        runnerL2Ucast_destruct();
        runnerUcast_destruct();

        return ret;
    }
#endif
#endif
#endif

#if !defined(CONFIG_BCM_CMDLIST_SIM)
    {
        cmdlist_hooks_t cmdlist_hooks;

        cmdlist_hooks.ipv6_addresses_table_add = runnerUcast_ipv6_addresses_table_add;
        cmdlist_hooks.ipv4_addresses_table_add = runnerUcast_ipv4_addresses_table_add;

        cmdlist_bind(&cmdlist_hooks);
    }
#endif

    runnerEnable();

#if !defined(CONFIG_BCM_CMDLIST_SIM) && !defined(RDP_SIM)
    {
        unsigned int rdp_freq;

        get_rdp_freq(&rdp_freq);

        __print("Initialized Runner Protocol Layer (%u)\n", rdp_freq);
    }
#else
    /* Override default log level to DEBUG */
    bcmLog_setLogLevel(BCM_LOG_ID_PKTRUNNER, BCM_LOG_LEVEL_DEBUG);

    __print("Initialized Runner Protocol Layer in SIMULATION MODE\n");
#endif /* CONFIG_BCM_CMDLIST_SIM */

    return 0;
}

/*
*******************************************************************************
* Function   : runnerProto_destruct
* Description: Destructs the Runner Protocol layer
*******************************************************************************
*/
void __exit runnerProto_destruct(void)
{
#if !defined(CONFIG_BCM_CMDLIST_SIM)
    runnerDisable();

    cmdlist_unbind();

#if defined(CC_PKTRUNNER_MCAST)
    runnerMcast_destruct();

    pktrunner_wlan_mcast_destruct();
#endif

    runnerL2Ucast_destruct();

    runnerUcast_destruct();
#endif
}


/*******************************************************************************
 *
 * Auxiliary Functions
 *
 *******************************************************************************/

rdpa_mcast_flow_t *__mcastFlowMalloc(void)
{
    return kmalloc(sizeof(rdpa_mcast_flow_t), GFP_ATOMIC);
}

void __mcastFlowFree(rdpa_mcast_flow_t *mcastFlow_p)
{
    kfree(mcastFlow_p);
}

uint32_t __enetLogicalPortToPhysicalPort(uint32_t logicalPort)
{
    return LOGICAL_PORT_TO_PHYSICAL_PORT(logicalPort);
}

uint32_t __skbMarkToQueuePriority(uint32_t skbMark)
{
    return SKBMARK_GET_Q_PRIO(skbMark);
}

uint32_t __skbMarkToTrafficClass(uint32_t skbMark)
{
    return SKBMARK_GET_TC_ID(skbMark);
}

int __isEnetWanPort(uint32_t logicalPort)
{
   int isWanPort = FALSE ;

#if defined(RDP_SIM)
   isWanPort = rdpa_blog_is_wan_port(logicalPort);
#else   
   bcmFun_t *enetIsWanPortFun = bcmFun_get(BCM_FUN_ID_ENET_IS_WAN_PORT);

   BCM_ASSERT(enetIsWanPortFun != NULL);

   isWanPort = enetIsWanPortFun(&logicalPort);
#endif

   return (isWanPort);
}

int __lagPortGet(Blog_t *blog_p)
{
#if defined(RDP_SIM)
    return blog_p->lag_port;
#else   
    bcmFun_t *lagPortGetFun = bcmFun_get(BCM_FUN_ID_ENET_LAG_PORT_GET);

    if(lagPortGetFun != NULL)
    {
        return lagPortGetFun(blog_p->tx_dev_p);
    }

    return 0;
#endif
}

/* Returns TRUE if LAN/SF2-Port is bonded with Runner WAN port */
int __isEnetBondedLanWanPort(uint32_t logicalPort)
{
   int ret_val = FALSE ;

#if !defined(CONFIG_BCM963158) && !defined(RDP_SIM)
  bcmFun_t *enetFunc = bcmFun_get(BCM_FUN_ID_ENET_IS_BONDED_LAN_WAN_PORT);

  BCM_ASSERT(enetFunc != NULL);

  ret_val = enetFunc(&logicalPort);
#endif

   return (ret_val);
}

int __isWlanPhy(Blog_t *blog_p)
{
#if !defined(CONFIG_BCM963158)
    return (blog_p->rx.info.phyHdrType == BLOG_WLANPHY ||
            blog_p->tx.info.phyHdrType == BLOG_WLANPHY);
#else
    return 0;
#endif
}

int __isTxWlanPhy(Blog_t *blog_p)
{
    return (blog_p->tx.info.phyHdrType == BLOG_WLANPHY);
}

int __ucastSetFwdAndFilters(Blog_t *blog_p, rdpa_ip_flow_info_t *ip_flow_p)
{
// FIXME!! need to fix rdpa_ip_flow_info_t from 6858 to support what we have for other platforms */
    if(blog_p->tx.info.bmap.BCM_XPHY) /* LAN/WLAN to XTM-WAN */
    {
        __debug("dest.phy XTM\n");

        ip_flow_p->key.dir = rdpa_dir_us;

        ip_flow_p->result.egress_if = rdpa_wan_type_to_if(rdpa_wan_dsl);

        ip_flow_p->result.wan_flow = blog_p->tx.info.channel ; /* WAN FLOW table index */
        ip_flow_p->result.wan_flow_mode = blog_p->ptm_us_bond ; /* WAN FLOW bonded/single */
    }
    else if(blog_p->rx.info.bmap.BCM_XPHY) /* XTM-WAN to LAN/WLAN */
    {
        ip_flow_p->key.dir = rdpa_dir_ds;

        if(blog_p->tx.info.phyHdrType == BLOG_ENETPHY)
        {
            __debug("dest.phy ETH\n");

            ip_flow_p->result.egress_if = rdpa_physical_port_to_rdpa_if(LOGICAL_PORT_TO_PHYSICAL_PORT(blog_p->tx.info.channel));
        }
        else if(BLOG_IS_TX_HWACC_ENABLED_WLAN_PHY(blog_p->tx.info.phyHdrType))
        {
            ip_flow_p->result.egress_if = rdpa_if_wlan0;  /* WLAN flow */
        }
        else
        {
            __logError("DSL-to-LAN flows are not supported");

            return -1;
        }
    }
    else if ((blog_p->tx.info.phyHdrType == BLOG_ENETPHY) &&
             __isEnetWanPort(blog_p->tx.info.channel)) /* LAN/WLAN to ENET-WAN */
    {
        __debug("dest.phy ETH\n");

        ip_flow_p->key.dir = rdpa_dir_us;

        ip_flow_p->result.egress_if = rdpa_wan_type_to_if(rdpa_wan_gbe);

        ip_flow_p->result.wan_flow = GBE_WAN_FLOW_ID ; /* WAN FLOW table index */
        ip_flow_p->result.wan_flow_mode = 0 ; /* WAN FLOW bonded/single */
    }
    else if ((blog_p->tx.info.phyHdrType == BLOG_ENETPHY) &&
             __isEnetBondedLanWanPort(blog_p->tx.info.channel) )  /* LAN/WLAN to Enet-Bonded-LAN-as-WAN */
    {
        __debug("dest.phy ETH\n");

        ip_flow_p->key.dir = rdpa_dir_us;

        ip_flow_p->result.egress_if = rdpa_physical_port_to_rdpa_if(LOGICAL_PORT_TO_PHYSICAL_PORT(blog_p->tx.info.channel));
    }
    else if (blog_p->rx.info.phyHdrType == BLOG_ENETPHY &&
            (__isEnetWanPort(blog_p->rx.info.channel) || /* ENET-WAN to LAN/WLAN */
             __isEnetBondedLanWanPort(blog_p->rx.info.channel)) ) /* Enet-Bonded-LAN-as-WAN to LAN/WLAN : Unlikely to hit this case */
    {
        /* Traffic from Runner-WAN or SF2-LAN-as-WAN are both handled by DS cluster */
        ip_flow_p->key.dir = rdpa_dir_ds;

        if(BLOG_IS_TX_HWACC_ENABLED_WLAN_PHY(blog_p->tx.info.phyHdrType))
        {
            ip_flow_p->result.egress_if = rdpa_if_wlan0;  /* WLAN flow */
        }
        else if(blog_p->tx.info.phyHdrType == BLOG_ENETPHY)
        {
            __debug("dest.phy ETH\n");

            ip_flow_p->result.egress_if = rdpa_physical_port_to_rdpa_if(LOGICAL_PORT_TO_PHYSICAL_PORT(blog_p->tx.info.channel));
        }
        else
        {
            __logError("ETHWAN-to-LAN flows are not supported");

            return -1;
        }
    }
    else if (blog_p->tx.info.phyHdrType == BLOG_GPONPHY)  /* GPON Upstream */
    {
        __debug("dest.phy GPON\n");

        ip_flow_p->key.dir = rdpa_dir_us;

        ip_flow_p->result.egress_if = rdpa_wan_type_to_if(rdpa_wan_gpon); 
        ip_flow_p->result.wan_flow = blog_p->tx.info.channel ; /* WAN FLOW table index */
        ip_flow_p->result.wan_flow_mode = 0; 
    }
    else if (blog_p->rx.info.phyHdrType == BLOG_GPONPHY) /* GPON downstream */
    {
        ip_flow_p->key.dir = rdpa_dir_ds;

        if(BLOG_IS_TX_HWACC_ENABLED_WLAN_PHY(blog_p->tx.info.phyHdrType))
        {
            ip_flow_p->result.egress_if = rdpa_if_wlan0;  /* WLAN flow */
        }
        else if(blog_p->tx.info.phyHdrType == BLOG_ENETPHY)
        {
            __debug("dest.phy ETH\n");

            ip_flow_p->result.egress_if = rdpa_physical_port_to_rdpa_if(LOGICAL_PORT_TO_PHYSICAL_PORT(blog_p->tx.info.channel));
        }
        else
        {
            __logError("GPON-to-LAN flows are not supported");

            return -1;
        }
    }
    else
    {
        /* LAN-to-LAN (not supported) */ 
        /* LAN-to-WLAN or WLAN-to-LAN or WLAN-to-WLAN */

        if( BLOG_IS_TX_HWACC_ENABLED_WLAN_PHY(blog_p->tx.info.phyHdrType) && 
            ( blog_p->rx.info.phyHdrType == BLOG_ENETPHY   ||  /* LAN to WLAN */
              blog_p->rx.info.phyHdrType == BLOG_WLANPHY )     /* WLAN to WLAN */
          )
        {
            ip_flow_p->key.dir = rdpa_dir_us;
            ip_flow_p->result.egress_if = rdpa_if_wlan0;  /* WLAN flow */
        }
        else if(blog_p->rx.info.phyHdrType == BLOG_WLANPHY &&
                blog_p->tx.info.phyHdrType == BLOG_ENETPHY) /* WLAN to LAN */
        {
            __debug("dest.phy ETH\n");

            ip_flow_p->key.dir = rdpa_dir_us; /* put in us connection table, fw will send ds */

            ip_flow_p->result.egress_if = rdpa_physical_port_to_rdpa_if(LOGICAL_PORT_TO_PHYSICAL_PORT(blog_p->tx.info.channel));
        }
        else
        {
            __logError("LAN-to-LAN flow is not supported: Rx %u, Tx %u", blog_p->rx.info.phyHdrType, blog_p->tx.info.phyHdrType);

            return -1;
        }
    }

    if(blog_p->rx.info.bmap.BCM_XPHY) /* XTM-WAN to LAN/WLAN */
    {
        __debug("source.phy XTM\n");

        ip_flow_p->key.ingress_if = rdpa_wan_type_to_if(rdpa_wan_dsl);
    }
    else if(blog_p->rx.info.phyHdrType == BLOG_ENETPHY &&
            __isEnetWanPort(blog_p->rx.info.channel)) /* ENET-WAN to LAN/WLAN */
    {
        __debug("source.phy ETH WAN\n");

        ip_flow_p->key.ingress_if = rdpa_wan_type_to_if(rdpa_wan_gbe);
    }
    else if(blog_p->rx.info.phyHdrType == BLOG_GPONPHY) 
    {
        __debug("source.phy GPON WAN\n");

        ip_flow_p->key.ingress_if = rdpa_wan_type_to_if(rdpa_wan_gpon);
    } 
    else
    {
        /* LAN */ 
        if (blog_p->rx.info.phyHdrType == BLOG_ENETPHY)
        {
            __debug("source.phy ETH\n");

            ip_flow_p->key.ingress_if = rdpa_physical_port_to_rdpa_if(LOGICAL_PORT_TO_PHYSICAL_PORT(blog_p->rx.info.channel));
        }
        else if (blog_p->rx.info.phyHdrType == BLOG_WLANPHY)
        {
            __debug("source.phy WLAN\n");

            ip_flow_p->key.ingress_if = rdpa_if_wlan0;  /* WLAN flow */
        }
        else
        {
            __logError("LAN flow is not supported: Rx %u, Tx %u", blog_p->rx.info.phyHdrType, blog_p->tx.info.phyHdrType);

            return -1;
        }
    }

    if(BLOG_IS_TX_HWACC_ENABLED_WLAN_PHY(blog_p->tx.info.phyHdrType))
    {
        ip_flow_p->result.queue_id  = 0;     /* queue_id not used for WLAN */
        ip_flow_p->result.rnr.is_wfd = blog_p->rnr.is_wfd; /* union with wfd nic and wfd dhd */
        if (blog_p->rnr.is_wfd)
        {
            ip_flow_p->result.wfd.nic_ucast.is_chain = blog_p->wfd.nic_ucast.is_chain;
            if (blog_p->wfd.nic_ucast.is_chain)
            {
                ip_flow_p->result.wfd.nic_ucast.wfd_prio = blog_p->wfd.nic_ucast.wfd_prio;
                ip_flow_p->result.wfd.nic_ucast.wfd_idx = blog_p->wfd.nic_ucast.wfd_idx;
                ip_flow_p->result.wfd.nic_ucast.priority = blog_p->wfd.nic_ucast.priority;
                ip_flow_p->result.wfd.nic_ucast.chain_idx = blog_p->wfd.nic_ucast.chain_idx;
            }
            else
            {
                ip_flow_p->result.wfd.dhd_ucast.wfd_prio = blog_p->wfd.dhd_ucast.wfd_prio;
                ip_flow_p->result.wfd.dhd_ucast.ssid = blog_p->wfd.dhd_ucast.ssid;
                ip_flow_p->result.wfd.dhd_ucast.wfd_idx = blog_p->wfd.dhd_ucast.wfd_idx;
                ip_flow_p->result.wfd.dhd_ucast.priority = blog_p->wfd.dhd_ucast.priority;
                ip_flow_p->result.wfd.dhd_ucast.flowring_idx = blog_p->wfd.dhd_ucast.flowring_idx;
            }
        }
        else /* Flow through DHD-RNR */
        {
            ip_flow_p->result.rnr.radio_idx = blog_p->rnr.radio_idx;
            ip_flow_p->result.rnr.priority = blog_p->rnr.priority;
            ip_flow_p->result.rnr.ssid = blog_p->rnr.ssid;
            ip_flow_p->result.rnr.flowring_idx = blog_p->rnr.flowring_idx;
        }
    }
    else
    {
        ip_flow_p->result.queue_id = SKBMARK_GET_Q_PRIO(blog_p->mark);
    }

    __debug("dest.channel %u\n", ip_flow_p->result.egress_if);
    __debug("dest.queue %u\n", ip_flow_p->result.queue_id);
    __debug("direction %s\n", ip_flow_p->key.dir == rdpa_dir_us ? "US" : "DS");

    /* set source */
    if(blog_p->rx.info.bmap.BCM_XPHY) /* XTM-WAN to LAN/WLAN */
    {
        /* Received from SAR */
        __debug("source.phy XTM\n");

        ip_flow_p->key.ingress_if = rdpa_wan_type_to_if(rdpa_wan_dsl);
    }
    else if(blog_p->rx.info.phyHdrType == BLOG_ENETPHY &&
            __isEnetWanPort(blog_p->rx.info.channel)) /* ENET-WAN to LAN/WLAN */
    {
        /* Received from ETH WAN */
        __debug("source.phy ETH WAN\n");

        ip_flow_p->key.ingress_if = rdpa_wan_type_to_if(rdpa_wan_gbe);
    }
    else if (blog_p->rx.info.phyHdrType == BLOG_ENETPHY)
    {
        /* Received from ETH LAN */
        __debug("source.phy ETH LAN\n");

        ip_flow_p->key.ingress_if = rdpa_physical_port_to_rdpa_if(LOGICAL_PORT_TO_PHYSICAL_PORT(blog_p->rx.info.channel));
    }
    else if (blog_p->rx.info.phyHdrType == BLOG_WLANPHY)
    {
        __debug("source.phy WLAN\n");

        ip_flow_p->key.ingress_if = rdpa_if_wlan0;  /* WLAN flow */
    }
    else if(blog_p->rx.info.phyHdrType == BLOG_GPONPHY) 
    {
        __debug("source.phy GPON WAN\n");

        ip_flow_p->key.ingress_if = rdpa_wan_type_to_if(rdpa_wan_gpon);
    } 
    else
    {
        __logError("flow is not supported: Rx %u, Tx %u", blog_p->rx.info.phyHdrType, blog_p->tx.info.phyHdrType);

        return -1;
    }
    __debug("source.channel %u\n", blog_p->rx.info.channel);

    /* L4 protocol is set in the Blog Key for both IPv4 and IPv6 */
    ip_flow_p->key.prot = blog_p->key.protocol;
    ip_flow_p->result.lag_port = blog_p->lag_port;

    __debug("protocol %u\n", ip_flow_p->key.prot);

#if defined(CC_PKTRUNNER_IPV6)
    if(blog_p->rx.info.bmap.PLD_IPv6 && !(T4in6DN(blog_p)))
    {
        ip_flow_p->key.src_ip.family = bdmf_ip_family_ipv6;
        memcpy(ip_flow_p->key.src_ip.addr.ipv6.data, blog_p->tupleV6.saddr.p8, 16);
        ip_flow_p->key.dst_ip.family = bdmf_ip_family_ipv6;
        memcpy(ip_flow_p->key.dst_ip.addr.ipv6.data, blog_p->tupleV6.daddr.p8, 16);
        ip_flow_p->key.src_port = ntohs(blog_p->tupleV6.port.source);
        ip_flow_p->key.dst_port = ntohs(blog_p->tupleV6.port.dest);

        __debug("IPv6 Src " IP6PHEX "\n", IP6(ip_flow_p->key.src_ip.addr.ipv6.data), ip_flow_p->key.src_port);
        __debug("IPv6 Dst " IP6PHEX "\n", IP6(ip_flow_p->key.dst_ip.addr.ipv6.data), ip_flow_p->key.dst_port);
    }
    else
#endif
    {
        ip_flow_p->key.src_ip.family = bdmf_ip_family_ipv4;
        ip_flow_p->key.src_ip.addr.ipv4 = ntohl(blog_p->rx.tuple.saddr);
        ip_flow_p->key.dst_ip.family = bdmf_ip_family_ipv4;
        ip_flow_p->key.dst_ip.addr.ipv4 = ntohl(blog_p->rx.tuple.daddr);
        ip_flow_p->key.src_port = ntohs(blog_p->rx.tuple.port.source);
        ip_flow_p->key.dst_port = ntohs(blog_p->rx.tuple.port.dest);

        __debug("IPv4 Src <%pI4:%u>\n", &blog_p->rx.tuple.saddr, ip_flow_p->key.src_port);
        __debug("IPv4 Dst <%pI4:%u>\n", &blog_p->rx.tuple.daddr, ip_flow_p->key.dst_port);
    }

    __debug("\n");

    /* Check if the flow is routed or bridged */

    ip_flow_p->result.is_routed = 0;

#if defined(CC_PKTRUNNER_IPV6)
    if (MAPT(blog_p))
    {
        ip_flow_p->result.is_routed = 1;
        ip_flow_p->result.is_df = blog_p->is_df;

        if (MAPT_UP(blog_p))
            ip_flow_p->result.is_mapt_us = 1;
    }
    else if(CHK4in6(blog_p) || CHK6in4(blog_p) || MAPT(blog_p))
    {
        ip_flow_p->result.is_routed = 1;
    }
    else if(CHK6to6(blog_p))
    {
        if(blog_p->tupleV6.rx_hop_limit != blog_p->tupleV6.tx_hop_limit)
        {
            ip_flow_p->result.is_routed = 1;
        }
    }
    else
#endif
    {
        if(CHK4to4(blog_p))
        {
            if(blog_p->rx.tuple.ttl != blog_p->tx.tuple.ttl)
            {
                ip_flow_p->result.is_routed = 1;
            }
        }
        else
        {
            __logError("Unable to determine if the flow is routed or bridged");

            return -1;
        }
    }

    ip_flow_p->result.tc = SKBMARK_GET_TC_ID(blog_p->mark);
    ip_flow_p->result.mtu = blog_getTxMtu(blog_p);

    ip_flow_p->result.tos = blog_p->rx.tuple.tos;
    ip_flow_p->result.is_l2_accel = 0;

    return 0;
}

int __l2ucastSetFwdAndFilters(Blog_t *blog_p, rdpa_l2_flow_info_t *l2_flow_p)
{
    if(blog_p->tx.info.bmap.BCM_XPHY) /* LAN/WLAN to XTM-WAN */
    {
        __debug("dest.phy XTM\n");

        l2_flow_p->key.dir = rdpa_dir_us;

        l2_flow_p->result.egress_if = rdpa_wan_type_to_if(rdpa_wan_dsl);

        l2_flow_p->result.wan_flow = blog_p->tx.info.channel ; /* WAN FLOW table index */
        l2_flow_p->result.wan_flow_mode = blog_p->ptm_us_bond ; /* WAN FLOW bonded/single */
    }
    else if(blog_p->rx.info.bmap.BCM_XPHY) /* XTM-WAN to LAN/WLAN */
    {
        l2_flow_p->key.dir = rdpa_dir_ds;

        if(blog_p->tx.info.phyHdrType == BLOG_ENETPHY)
        {
            __debug("dest.phy ETH\n");

            l2_flow_p->result.egress_if = rdpa_physical_port_to_rdpa_if(LOGICAL_PORT_TO_PHYSICAL_PORT(blog_p->tx.info.channel));
        }
        else if(BLOG_IS_TX_HWACC_ENABLED_WLAN_PHY(blog_p->tx.info.phyHdrType))
        {
            l2_flow_p->result.egress_if = rdpa_if_wlan0;  /* WLAN flow */
        }
        else
        {
            __logError("DSL-to-LAN flows are not supported");

            return -1;
        }
    }
    else if(blog_p->tx.info.phyHdrType == BLOG_ENETPHY &&
             __isEnetWanPort(blog_p->tx.info.channel)) /* LAN/WLAN to ENET-WAN */
    {
        __debug("dest.phy ETH\n");

        l2_flow_p->key.dir = rdpa_dir_us;

        l2_flow_p->result.egress_if = rdpa_wan_type_to_if(rdpa_wan_gbe);

        l2_flow_p->result.wan_flow = GBE_WAN_FLOW_ID ; /* WAN FLOW table index */
        l2_flow_p->result.wan_flow_mode = 0 ;
    }
    else if ((blog_p->tx.info.phyHdrType == BLOG_ENETPHY) &&
             __isEnetBondedLanWanPort(blog_p->tx.info.channel) )  /* LAN/WLAN to Enet-Bonded-LAN-as-WAN */
    {
        __debug("dest.phy ETH\n");

        l2_flow_p->key.dir = rdpa_dir_us;

        l2_flow_p->result.egress_if = rdpa_physical_port_to_rdpa_if(LOGICAL_PORT_TO_PHYSICAL_PORT(blog_p->tx.info.channel));
    }
    else if(blog_p->rx.info.phyHdrType == BLOG_ENETPHY &&
            (__isEnetWanPort(blog_p->rx.info.channel) || /* ENET-WAN to LAN/WLAN */
             __isEnetBondedLanWanPort(blog_p->rx.info.channel)) ) /* Enet-Bonded-LAN-as-WAN to LAN/WLAN : Unlikely to hit this case */
    {
        /* Traffic from Runner-WAN or SF2-LAN-as-WAN are both handled by DS cluster */
        l2_flow_p->key.dir = rdpa_dir_ds;

        if(BLOG_IS_TX_HWACC_ENABLED_WLAN_PHY(blog_p->tx.info.phyHdrType))
        {
            l2_flow_p->result.egress_if = rdpa_if_wlan0;  /* WLAN flow */
        }
        else if(blog_p->tx.info.phyHdrType == BLOG_ENETPHY)
        {
            __debug("dest.phy ETH\n");

            l2_flow_p->result.egress_if = rdpa_physical_port_to_rdpa_if(LOGICAL_PORT_TO_PHYSICAL_PORT(blog_p->tx.info.channel));
        }
        else
        {
            __logError("ETHWAN-to-LAN flows are not supported");

            return -1;
        }
    }
    else
    {
        /* LAN-to-LAN (not supported) */ 
        /* LAN-to-WLAN or WLAN-to-LAN or WLAN-to-WLAN */

        if( BLOG_IS_TX_HWACC_ENABLED_WLAN_PHY(blog_p->tx.info.phyHdrType) && 
            ( blog_p->rx.info.phyHdrType == BLOG_ENETPHY   ||  /* LAN to WLAN */
              blog_p->rx.info.phyHdrType == BLOG_WLANPHY )     /* WLAN to WLAN */
          )
        {
            l2_flow_p->key.dir = rdpa_dir_us;
            l2_flow_p->result.egress_if = rdpa_if_wlan0;  /* WLAN flow */
        }
        else if(blog_p->rx.info.phyHdrType == BLOG_WLANPHY &&
                blog_p->tx.info.phyHdrType == BLOG_ENETPHY) /* WLAN to LAN */
        {
            __debug("dest.phy ETH\n");

            l2_flow_p->key.dir = rdpa_dir_us; /* put in us connection table, fw will send ds */

            l2_flow_p->result.egress_if = rdpa_physical_port_to_rdpa_if(LOGICAL_PORT_TO_PHYSICAL_PORT(blog_p->tx.info.channel));
        }
        else
        {
            __logError("LAN-to-LAN flow is not supported: Rx %u, Tx %u", blog_p->rx.info.phyHdrType, blog_p->tx.info.phyHdrType);

            return -1;
        }
    }

    if(BLOG_IS_TX_HWACC_ENABLED_WLAN_PHY(blog_p->tx.info.phyHdrType))
    {
        l2_flow_p->result.queue_id  = 0;     /* queue_id not used for WLAN */
        l2_flow_p->result.rnr.is_wfd = blog_p->rnr.is_wfd; /* union with wfd nic and wfd dhd */
        if (blog_p->rnr.is_wfd)
        {
            l2_flow_p->result.wfd.nic_ucast.is_chain = blog_p->wfd.nic_ucast.is_chain;
            if (blog_p->wfd.nic_ucast.is_chain)
            {
                l2_flow_p->result.wfd.nic_ucast.wfd_prio = blog_p->wfd.nic_ucast.wfd_prio;
                l2_flow_p->result.wfd.nic_ucast.wfd_idx = blog_p->wfd.nic_ucast.wfd_idx;
                l2_flow_p->result.wfd.nic_ucast.priority = blog_p->wfd.nic_ucast.priority;
                l2_flow_p->result.wfd.nic_ucast.chain_idx = blog_p->wfd.nic_ucast.chain_idx;
            }
            else
            {
                l2_flow_p->result.wfd.dhd_ucast.wfd_prio = blog_p->wfd.dhd_ucast.wfd_prio;
                l2_flow_p->result.wfd.dhd_ucast.ssid = blog_p->wfd.dhd_ucast.ssid;
                l2_flow_p->result.wfd.dhd_ucast.wfd_idx = blog_p->wfd.dhd_ucast.wfd_idx;
                l2_flow_p->result.wfd.dhd_ucast.priority = blog_p->wfd.dhd_ucast.priority;
                l2_flow_p->result.wfd.dhd_ucast.flowring_idx = blog_p->wfd.dhd_ucast.flowring_idx;
            }
        }
        else /* Flow through DHD-RNR */
        {
            l2_flow_p->result.rnr.radio_idx = blog_p->rnr.radio_idx;
            l2_flow_p->result.rnr.priority = blog_p->rnr.priority;
            l2_flow_p->result.rnr.ssid = blog_p->rnr.ssid;
            l2_flow_p->result.rnr.flowring_idx = blog_p->rnr.flowring_idx;
        }
    }
    else
    {
        l2_flow_p->result.queue_id = SKBMARK_GET_Q_PRIO(blog_p->mark);
    }

    __debug("dest.channel %u\n", l2_flow_p->result.egress_if);
    __debug("dest.queue %u\n", l2_flow_p->result.queue_id);
    __debug("direction %s\n", l2_flow_p->key.dir == rdpa_dir_us ? "US" : "DS");

    /* set source */
    if(blog_p->rx.info.bmap.BCM_XPHY) /* XTM-WAN to LAN/WLAN */
    {
        /* Received from SAR */
        __debug("source.phy XTM\n");

        l2_flow_p->key.ingress_if = rdpa_wan_type_to_if(rdpa_wan_dsl);
    }
    else if(blog_p->rx.info.phyHdrType == BLOG_ENETPHY &&
            (__isEnetWanPort(blog_p->rx.info.channel) || /* ENET-WAN to LAN/WLAN */
             __isEnetBondedLanWanPort(blog_p->rx.info.channel)) ) /* Enet-Bonded-LAN-as-WAN to LAN/WLAN : Unlikely to hit this case */
    {
        /* Received from ETH WAN */
        __debug("source.phy ETH WAN\n");

        l2_flow_p->key.ingress_if = rdpa_wan_type_to_if(rdpa_wan_gbe);
    }
    else if (blog_p->rx.info.phyHdrType == BLOG_ENETPHY)
    {
        /* Received from ETH LAN */
        __debug("source.phy ETH LAN\n");

        l2_flow_p->key.ingress_if = rdpa_physical_port_to_rdpa_if(LOGICAL_PORT_TO_PHYSICAL_PORT(blog_p->rx.info.channel));
    }
    else if (blog_p->rx.info.phyHdrType == BLOG_WLANPHY)
    {
        __debug("source.phy WLAN\n");

        l2_flow_p->key.ingress_if = rdpa_if_wlan0;  /* WLAN flow */
    }
    else
    {
        __logError("flow is not supported: Rx %u, Tx %u", blog_p->rx.info.phyHdrType, blog_p->tx.info.phyHdrType);

        return -1;
    }
    __debug("source.channel %u\n", blog_p->rx.info.channel);

    memcpy( &l2_flow_p->key.dst_mac.b[0], &blog_p->rx.l2hdr[0], BLOG_ETH_ADDR_LEN );
    memcpy( &l2_flow_p->key.src_mac.b[0], &blog_p->rx.l2hdr[6], BLOG_ETH_ADDR_LEN );
    l2_flow_p->key.eth_type = ntohs(blog_p->eth_type);
    l2_flow_p->key.vtag0 = ntohl(blog_p->vtag[0]);
    l2_flow_p->key.vtag1 = ntohl(blog_p->vtag[1]);
    l2_flow_p->key.vtag_num = blog_p->vtag_num;
    l2_flow_p->key.tos = blog_p->rx.tuple.tos;

    __debug("Dst MAC <%02x:%02x:%02x:%02x:%02x:%02x>\n", 
        blog_p->rx.l2hdr[0], blog_p->rx.l2hdr[1], blog_p->rx.l2hdr[2],
        blog_p->rx.l2hdr[3], blog_p->rx.l2hdr[4], blog_p->rx.l2hdr[5] );
    __debug("Src MAC <%02x:%02x:%02x:%02x:%02x:%02x>\n", 
        blog_p->rx.l2hdr[6], blog_p->rx.l2hdr[7], blog_p->rx.l2hdr[8],
        blog_p->rx.l2hdr[9], blog_p->rx.l2hdr[10], blog_p->rx.l2hdr[11] );
    __debug("vtag_num = %u, vtag[0] = 0x%08x, vtag[1] = 0x%08x, eth_type = 0x%04x tos= 0x%02x\n", 
            blog_p->vtag_num, ntohl(blog_p->vtag[0]), ntohl(blog_p->vtag[1]), ntohs(blog_p->eth_type), 
            blog_p->rx.tuple.tos) ;
    __debug("\n");

    /* Check if the flow is routed or bridged */
    l2_flow_p->result.is_routed = 0;
    l2_flow_p->result.is_l2_accel = 1;
    l2_flow_p->result.tc = SKBMARK_GET_TC_ID(blog_p->mark);
    l2_flow_p->result.mtu = blog_getTxMtu(blog_p);
    l2_flow_p->result.is_tos_mangle = 0;
    l2_flow_p->result.tos = blog_p->tx.tuple.tos;
    l2_flow_p->result.lag_port = blog_p->lag_port;

    /* L2 accel: tos field holds tos value for both IPv4 and IPv6 */
    if(blog_p->rx.tuple.tos != blog_p->tx.tuple.tos)
    {
        l2_flow_p->result.is_tos_mangle = 1;
        l2_flow_p->result.tos = blog_p->tx.tuple.tos;
    }

    return 0;
}

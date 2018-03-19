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
* File Name  : ptkrunner_l2_ucast.c
*
* Description: This implementation translates L2 Unicast Blogs into Runner Flows
*              for xDSL platforms.
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
#include <bcmtypes.h>
#include "fcachehw.h"
#include "bcmxtmcfg.h"
#endif

#include "cmdlist_api.h"

#include <rdpa_api.h>

#include "pktrunner_proto.h"
#include "pktrunner_mcast.h"

#if defined(CONFIG_BCM_CMDLIST_SIM)
#include "runner_sim.h"
#endif

/*******************************************************************************
 *
 * Global Variables and Definitions
 *
 *******************************************************************************/

#if !defined(CONFIG_BCM_CMDLIST_SIM)
static int l2_ucast_class_created_here   = 0;
static bdmf_object_handle l2_ucast_class = NULL;
#endif

int runnerL2Ucast_activate(Blog_t *blog_p)
{
    int flowIdx = FHW_TUPLE_INVALID;
    rdpa_l2_flow_info_t l2_flow;
#if defined(BCM63158)
    cmdlist_cmd_target_t target = CMDLIST_CMD_TARGET_SRAM;
#else
    cmdlist_cmd_target_t target = CMDLIST_CMD_TARGET_DDR;
#endif
    int ret=0;

    memset(&l2_flow, 0, sizeof(rdpa_l2_flow_info_t));

    ret = __l2ucastSetFwdAndFilters(blog_p, &l2_flow);
    if(ret != 0)
    {
        __logInfo("Could not setFwdAndFilters");

        goto abort_activate;
    }

    cmdlist_init(l2_flow.result.cmd_list, RDPA_CMD_LIST_UCAST_LIST_SIZE, RDPA_CMD_LIST_UCAST_LIST_OFFSET);

    ret = cmdlist_l2_ucast_create(blog_p, target);
    if(ret != 0)
    {
        __logInfo("Could not cmdlist_create");

        goto abort_activate;
    }

    l2_flow.result.cmd_list_length = cmdlist_get_length();

    __debug("cmd_list_length = %u\n", l2_flow.result.cmd_list_length);
    __dumpCmdList(l2_flow.result.cmd_list);

#if defined(CONFIG_BCM_CMDLIST_SIM)
    {
        int skip_brcm_tag_len =
            __isEnetWanPort(blog_p->rx.info.channel) ? BRCM_TAG_TYPE2_LEN : 0;

        runnerSim_activate(blog_p, l2_flow.result.cmd_list, RDPA_CMD_LIST_UCAST_LIST_OFFSET,
                           NULL, 0, skip_brcm_tag_len);
    }
#else
    {
        bdmf_index index;

        ret = rdpa_l2_ucast_flow_add(l2_ucast_class, &index, &l2_flow);
        if (ret < 0)
        {
            __logInfo("Cannot rdpa_l2_ucast_flow_add");

            goto abort_activate;
        }
	
        flowIdx = (int)index;
    }
#endif /* CONFIG_BCM_CMDLIST_SIM */

    return flowIdx;

abort_activate:
    return FHW_TUPLE_INVALID;
}

#if !defined(CONFIG_BCM_CMDLIST_SIM)
static int l2UcastDeleteFlow(bdmf_index flowIdx, int speculative)
{
    rdpa_l2_flow_info_t l2_flow;
    int rc;

    rc = rdpa_l2_ucast_flow_get(l2_ucast_class, flowIdx, &l2_flow);
    if(rc < 0)
    {
        if(!speculative)
        {
            __logError("Cannot rdpa_l2_ucast_flow_get (flowIdx %ld)", flowIdx);
        }

        return rc;
    }

    rc = rdpa_l2_ucast_flow_delete(l2_ucast_class, flowIdx);
    if(rc < 0)
    {
        __logError("Cannot rdpa_l2_ucast_flow_delete (flowIdx %ld)", flowIdx);

        return rc;
    }

    return 0;
}

static void l2UcastDeleteAllFlows(void)
{
    bdmf_index flowIdx;
    int rc;

    for(flowIdx=0; flowIdx<RDPA_UCAST_MAX_FLOWS; ++flowIdx)
    {
        rc = l2UcastDeleteFlow(flowIdx, 1);
        if(rc)
        {
            continue;
        }

        __logInfo("Deleted Unicast Flow Index: %ld", flowIdx);
    }
}

int runnerL2Ucast_deactivate(uint16_t tuple)
{
    bdmf_index flowIdx = runner_get_hw_entix(tuple);

    return l2UcastDeleteFlow(flowIdx, 0);
}

int runnerL2Ucast_update(BlogUpdate_t update, uint16_t tuple, Blog_t *blog_p)
{
    bdmf_index flowIdx = runner_get_hw_entix(tuple);
    rdpa_l2_flow_info_t l2_flow = {};

    switch(update)
    {
        case BLOG_UPDATE_DPI_QUEUE:
            l2_flow.result.service_queue_id = blog_p->dpi_queue;
            break;

        default:
            __logError("Invalid BLOG Update: <%d>", update);
            return -1;
    }

    return rdpa_l2_ucast_flow_set(l2_ucast_class, flowIdx, &l2_flow);
}

/*
 *------------------------------------------------------------------------------
 * Function   : runnerL2Ucast_refresh
 * Description: This function is invoked to collect flow statistics
 * Parameters :
 *  tuple : 16bit index to refer to a Runner flow
 * Returns    : Total hits on this connection.
 *------------------------------------------------------------------------------
 */
int runnerL2Ucast_refresh(int flowIdx, uint32_t *pktsCnt_p, uint32_t *octetsCnt_p)
{
#if !defined(CONFIG_BCM963158)
    rdpa_stat_t flow_stat;
    int rc;

    rc = rdpa_l2_ucast_flow_stat_get(l2_ucast_class, flowIdx, &flow_stat);
    if (rc < 0)
    {
//        __logDebug("Could not get flowIdx<%d> stats, rc %d", flowIdx, rc);
        return rc;
    }

    *pktsCnt_p = flow_stat.packets; /* cummulative packets */
    *octetsCnt_p = flow_stat.bytes;

    __logDebug( "flowIdx<%03u> "
                "cumm_pkt_hits<%u> cumm_octet_hits<%u>\n",
                flowIdx, *pktsCnt_p, *octetsCnt_p );
    return 0;
#else
    return -1;
#endif
}

#else /* CONFIG_BCM_CMDLIST_SIM */

int runnerL2Ucast_deactivate(uint16_t tuple)
{
    return 0;
}

int runnerL2Ucast_refresh(int flowIdx, uint32_t *pktsCnt_p, uint32_t *octetsCnt_p)
{
    *pktsCnt_p = 1;
    *octetsCnt_p = 1;

    return 0;
}

#endif /* CONFIG_BCM_CMDLIST_SIM */

/*
*******************************************************************************
* Function   : runnerL2Ucast_construct
* Description: Constructs the Runner Protocol layer
*******************************************************************************
*/
int __init runnerL2Ucast_construct(void)
{
#if !defined(CONFIG_BCM_CMDLIST_SIM)
    int ret;

    BDMF_MATTR(l2_ucast_attrs, rdpa_l2_ucast_drv());

    ret = rdpa_l2_ucast_get(&l2_ucast_class);
    if (ret)
    {
        ret = bdmf_new_and_set(rdpa_l2_ucast_drv(), NULL, l2_ucast_attrs, &l2_ucast_class);
        if (ret)
        {
            BCM_LOG_ERROR(BCM_LOG_ID_PKTRUNNER, "rdpa l2_ucast_class object does not exist and can't be created.\n");
            return ret;
        }
        l2_ucast_class_created_here = 1;
    }
#endif

    __print("Initialized Runner L2 Unicast Layer\n");

    return 0;
}

/*
*******************************************************************************
* Function   : runnerL2Ucast_destruct
* Description: Destructs the Runner Protocol layer
*******************************************************************************
*/
void __exit runnerL2Ucast_destruct(void)
{
#if !defined(CONFIG_BCM_CMDLIST_SIM)
    if(l2_ucast_class)
    {
        l2UcastDeleteAllFlows();

        if(l2_ucast_class_created_here)
        {
            bdmf_destroy(l2_ucast_class);
            l2_ucast_class_created_here = 0;
        }
        else
        {
            bdmf_put(l2_ucast_class);
        }
    }
#endif
}

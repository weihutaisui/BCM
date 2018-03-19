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
 * :>
 */
/******************************************************************************
 Filename: cmdlist_driver.c
           This file implements the Command List Driver
******************************************************************************/

#if defined(RDP_SIM)
#include "pktrunner_rdpa_sim.h"
#else
#include <linux/module.h>
#include <linux/bcm_log.h>
#include <linux/blog.h>
#include "bcmenet.h"
#endif

#include "cmdlist.h"

cmdlist_hooks_t cmdlist_hooks_g;

/*******************************************************************************
 *
 * Helper Functions
 *
 *******************************************************************************/

#if !defined(RDP_SIM)
int __isEnetWanPort(uint32_t logicalPort)
{
   bcmFun_t *enetIsWanPortFun = bcmFun_get(BCM_FUN_ID_ENET_IS_WAN_PORT);
   int isWanPort = 0;

   BCM_ASSERT(enetIsWanPortFun != NULL);

   isWanPort = enetIsWanPortFun(&logicalPort);

   return (isWanPort);
}

int __isTxWlanPhy(Blog_t *blog_p)
{
    return (blog_p->tx.info.phyHdrType == BLOG_WLANPHY);
}
#endif /* !RDP_SIM */

uint32_t __buildBrcmTagType2(Blog_t *blog_p)
{
    uint32_t tag;

    tag = (BRCM_TAG2_EGRESS | (SKBMARK_GET_Q_PRIO(blog_p->mark) << 10)) << 16;
    tag |= (uint16_t)(1 << LOGICAL_PORT_TO_PHYSICAL_PORT(blog_p->tx.info.channel));

    return tag;
}

/*******************************************************************************
 *
 * API
 *
 *******************************************************************************/

int cmdlist_bind(cmdlist_hooks_t *hooks_p)
{
    cmdlist_hooks_g.ipv6_addresses_table_add = hooks_p->ipv6_addresses_table_add;
    cmdlist_hooks_g.ipv4_addresses_table_add = hooks_p->ipv4_addresses_table_add;

    return 0;
}

void cmdlist_unbind(void)
{
    memset(&cmdlist_hooks_g, 0, sizeof(cmdlist_hooks_t));
}

void cmdlist_init(uint32_t *cmd_list_p, uint32_t cmd_list_length_max, uint32_t cmd_list_start_offset)
{
    cmdlist_init_bin(cmd_list_p, cmd_list_length_max, cmd_list_start_offset);
}

int cmdlist_ucast_create(Blog_t *blog_p, cmdlist_cmd_target_t target)
{
    int ret = cmdlist_ucast_create_bin(blog_p, target);

    if(!ret)
    {
        ret = cmdlist_cmd_end_bin();
        if(ret != 0)
        {
            __logInfo("Could not cmdlist_cmd_end_bin");
        }
    }

    return ret;
}

int cmdlist_l2_ucast_create(Blog_t *blog_p, cmdlist_cmd_target_t target)
{
    int ret = cmdlist_l2_ucast_create_bin(blog_p, target);

    if(!ret)
    {
        ret = cmdlist_cmd_end_bin();
        if(ret != 0)
        {
            __logInfo("Could not cmdlist_cmd_end_bin");
        }
    }

    return ret;
}

void cmdlist_mcast_parse(Blog_t *blog_p, blogRule_t *blogRule_p, cmdlist_mcast_parse_t *parse_p)
{
    cmdlist_mcast_parse_bin(blog_p, blogRule_p, parse_p);
}

int cmdlist_l2_mcast_create(Blog_t *blog_p, blogRule_t *blogRule_p, cmdlist_mcast_parse_t *parse_p)
{
    int ret = cmdlist_l2_mcast_create_bin(blog_p, blogRule_p, parse_p);

    if(!ret)
    {
        ret = cmdlist_cmd_end_bin();
        if(ret != 0)
        {
            __logInfo("Could not cmdlist_cmd_end_bin");
        }
    }

    return ret;
}

int cmdlist_l3_mcast_create(Blog_t *blog_p, blogRule_t *blogRule_p, cmdlist_mcast_parse_t *parse_p, uint8_t *isRouted_p)
{
    int ret = cmdlist_l3_mcast_create_bin(blog_p, blogRule_p, parse_p, isRouted_p);

    if(!ret)
    {
        ret = cmdlist_cmd_end_bin();
        if(ret != 0)
        {
            __logInfo("Could not cmdlist_cmd_end_bin");
        }
    }

    return ret;
}

uint32_t cmdlist_get_length(void)
{
    return cmdlist_get_length_bin();
}

void cmdlist_dump(uint32_t *cmd_list_p, int length32)
{
    cmdlist_dump_bin(cmd_list_p, length32);
}

void cmdlist_dump_partial(void)
{
    cmdlist_dump_partial_bin();
}

/*******************************************************************************
 *
 * Driver
 *
 *******************************************************************************/

int __init cmdlist_construct(void)
{
#if defined(CONFIG_BCM_CMDLIST_SIM)
    bcmLog_setLogLevel(BCM_LOG_ID_CMDLIST, BCM_LOG_LEVEL_DEBUG);
#else
    bcmLog_setLogLevel(BCM_LOG_ID_CMDLIST, BCM_LOG_LEVEL_ERROR);
#endif

    cmdlist_unbind();

    __print("Broadcom Command List Driver v1.0\n");

    return 0;
}

void __exit cmdlist_destruct(void)
{
}

#if !defined(RDP_SIM)
module_init(cmdlist_construct);
module_exit(cmdlist_destruct);

EXPORT_SYMBOL(cmdlist_init);
EXPORT_SYMBOL(cmdlist_ucast_create);
EXPORT_SYMBOL(cmdlist_l2_ucast_create);
EXPORT_SYMBOL(cmdlist_mcast_parse);
EXPORT_SYMBOL(cmdlist_l2_mcast_create);
EXPORT_SYMBOL(cmdlist_l3_mcast_create);
EXPORT_SYMBOL(cmdlist_get_length);
EXPORT_SYMBOL(cmdlist_bind);
EXPORT_SYMBOL(cmdlist_unbind);
EXPORT_SYMBOL(cmdlist_dump);
EXPORT_SYMBOL(cmdlist_dump_partial);

MODULE_DESCRIPTION("Broadcom Command List Driver");
MODULE_VERSION("v1.0");

MODULE_LICENSE("Proprietary");
#endif

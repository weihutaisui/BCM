/*
  <:copyright-broadcom 
 
  Copyright (c) 2007 Broadcom Corporation 
  All Rights Reserved 
  No portions of this material may be reproduced in any form without the 
  written permission of: 
  Broadcom Corporation 
  Irvine, California
  All information contained in this document is Broadcom Corporation 
  company private, proprietary, and trade secret. 
 
  :>
*/
//**************************************************************************
// File Name  : bcm_vlan_user.c
//
// Description: Broadcom VLAN Interface Driver
//               
//**************************************************************************

#include "bcm_vlan_local.h"
#include "bcm_vlan_dev.h"
#include <linux/blog_rule.h>

#define vlanTestPrint(_fmt, _arg...)            \
    printk("VLAN TEST : " _fmt, ##_arg)

#define vlanTestError(_fmt, _arg...)                            \
    BCM_LOG_ERROR(BCM_LOG_ID_VLAN, "TEST : " _fmt, ##_arg)

int bcmVlan_blogVlanFlows(Blog_t *blog_p,
                          struct net_device *rxVlanDev,
                          struct net_device *txVlanDev);


#if defined(CC_BCM_VLAN_FLOW)
static void vlanMcastTest1(struct net_device *rxVlanDev, struct net_device *txVlanDev)
{
    int ret;
    Blog_t *blog_p;
    blogRule_t *blogRule_p;
    blogRuleAction_t blogRuleAction;
    int blogRuleCount;

    blog_p = (Blog_t *)kmalloc(sizeof(Blog_t), GFP_KERNEL);
    if(blog_p == NULL)
    {
        vlanTestError("Failed to allocate Blog!");
        return;
    }

    memset(blog_p, 0, sizeof(Blog_t));

    /* create master blog rule */
    blogRule_p = blog_rule_alloc();
    if(blogRule_p == NULL)
    {
        vlanTestError("Failed to allocate Blog Rule!");
        return;
    }
    blog_rule_init(blogRule_p);

    memset(&blogRuleAction, 0, sizeof(blogRuleAction_t));
    blogRuleAction.cmd = BLOG_RULE_CMD_POP_PPPOE_HDR;

    ret = blog_rule_add_action(blogRule_p, &blogRuleAction);
    vlanTestPrint("blog_rule_add_action = <%d>\n", ret);

    /* attach master blog rule to the blog */
    blog_p->blogRule_p = blogRule_p;

    /* Rx Fwd Info */
    blog_p->rx.info.phyHdr = BLOG_GPONPHY;
    blog_p->rx.info.channel = 10;
    /* Tx Fwd Info */
    blog_p->tx.info.phyHdr = BLOG_ENETPHY;
//    blog_p->tx.info.channel = 1;
    /* Tuple */
//    blog_p->rx.tuple.daddr = 0xE80A0A01; /* SSM : 232.10.10.1 */
//    blog_p->rx.tuple.saddr = 0x0A0A0A01; /* 10.10.10.1 */

    ret = bcmVlan_blogVlanFlows(blog_p, rxVlanDev, txVlanDev);
    vlanTestPrint("bcmVlan_blogVlanFlows = <%d>\n", ret);


    /* free blog rules */

    blogRuleCount = blog_rule_free_list(blog_p);

    vlanTestPrint("Found <%u> Blog Rules\n", blogRuleCount);

    /* free blog */

    kfree(blog_p);
}

static void vlanMcastTest2(struct net_device *rxVlanDev, struct net_device *txVlanDev, int sync)
{
    int ret;
    Blog_t *blog_p;
    blogRule_t *blogRule_p;
    blogRuleAction_t blogRuleAction;
    int blogRuleCount;

    blog_p = (Blog_t *)kmalloc(sizeof(Blog_t), GFP_KERNEL);
    if(blog_p == NULL)
    {
        vlanTestError("Failed to allocate Blog!");
        return;
    }

    memset(blog_p, 0, sizeof(Blog_t));

    /* create master blog rule */
    blogRule_p = blog_rule_alloc();
    if(blogRule_p == NULL)
    {
        vlanTestError("Failed to allocate Blog Rule!");
        return;
    }
    blog_rule_init(blogRule_p);

    memset(&blogRuleAction, 0, sizeof(blogRuleAction_t));
    blogRuleAction.cmd = BLOG_RULE_CMD_POP_PPPOE_HDR;

    ret = blog_rule_add_action(blogRule_p, &blogRuleAction);
    vlanTestPrint("blog_rule_add_action = <%d>\n", ret);

    /* attach master blog rule to the blog */
    blog_p->blogRule_p = blogRule_p;

    /* Rx Fwd Info */
    blog_p->rx.info.phyHdr = BLOG_GPONPHY;
    blog_p->rx.info.channel = 10;
    /* Tx Fwd Info */
    blog_p->tx.info.phyHdr = BLOG_ENETPHY;

    ret = bcmVlan_blogVlanFlows(blog_p, rxVlanDev, txVlanDev);
    vlanTestPrint("bcmVlan_blogVlanFlows = <%d>\n", ret);


    /* free blog rules */

    blogRuleCount = blog_rule_free_list(blog_p);

    vlanTestPrint("Found <%u> Blog Rules\n", blogRuleCount);

    /* free blog */

    kfree(blog_p);
}

void vlanMcastTest3(struct net_device *rxVlanDev, struct net_device *txVlanDev)
{
    vlanMcastTest2(rxVlanDev, txVlanDev, 1);
}

#endif /* CC_BCM_VLAN_FLOW */

void bcmVlan_runTest(bcmVlan_iocRunTest_t *iocRunTest_p)
{
    struct net_device *rxVlanDev;
    struct net_device *txVlanDev;

    vlanTestPrint("Test <%u>\n", iocRunTest_p->testNbr);

    rxVlanDev = dev_get_by_name(&init_net, iocRunTest_p->rxVlanDevName);
    if(rxVlanDev != NULL)
    {
        vlanTestPrint("rxVlanDev: %s\n", rxVlanDev->name);
    }
    else
    { 
        vlanTestPrint("rxVlanDev: %s\n", iocRunTest_p->rxVlanDevName);
    }

    txVlanDev = dev_get_by_name(&init_net, iocRunTest_p->txVlanDevName);
    if(txVlanDev != NULL)
    {
        vlanTestPrint("txVlanDev: %s\n", txVlanDev->name);
    }
    else
    {
        vlanTestPrint("txVlanDev: %s\n", iocRunTest_p->txVlanDevName);
    }

#if defined(CC_BCM_VLAN_FLOW)
    switch(iocRunTest_p->testNbr)
    {
        case 1:
            vlanMcastTest1(rxVlanDev, txVlanDev);
            break;

        case 2:
            vlanMcastTest2(rxVlanDev, txVlanDev, 0);
            break;

        case 3:
            vlanMcastTest3(rxVlanDev, txVlanDev);
            break;

        default:
            vlanTestPrint("Invalid testNbr <%d>\n", iocRunTest_p->testNbr);
    }
#endif

    if(rxVlanDev != NULL)
    {
        dev_put(rxVlanDev);
    }

    if(txVlanDev != NULL)
    {
        dev_put(txVlanDev);
    }

    vlanTestPrint("Done!\n");
}

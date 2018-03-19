/*
 Copyright 2007-2010 Broadcom Corp. All Rights Reserved.

 <:label-BRCM:2011:DUAL/GPL:standard

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

#define _BCMENET_LOCAL_

#include <linux/types.h>
#include <linux/delay.h>
#include <linux/mii.h>
#include <linux/stddef.h>
#include <linux/ctype.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <board.h>
#include "boardparms.h"
#include <bcm_map_part.h>
#include "bcm_intr.h"
#include "bcmenet.h"
#include "bcmmii.h"
#include "hwapi_mac.h"
#include <rdpa_api.h>
#include "bcmswshared.h"
#include "ethsw.h"
#include "ethsw_phy.h"
#include "eth_pwrmngt.h"
#include "bcmsw_runner.h"


extern struct semaphore bcm_ethlock_switch_config;


int ethsw_reset_ports(struct net_device *dev)
{
    return 0;
}

int ethsw_add_proc_files(struct net_device *dev)
{
    return 0;
}

int ethsw_del_proc_files(void)
{
    return 0;
}

int ethsw_disable_hw_switching(void)
{
    return 0;
}

void ethsw_dump_page(int page)
{
}

void fast_age_port(uint8_t port, uint8_t age_static)
{
}

int ethsw_counter_collect(uint32_t portmap, int discard)
{
    return 0;
}

void ethsw_get_txrx_imp_port_pkts(unsigned int *tx, unsigned int *rx)
{
}
EXPORT_SYMBOL(ethsw_get_txrx_imp_port_pkts);

int ethsw_phy_intr_ctrl(int port, int on)
{
    return 0;
}
void ethsw_phy_apply_init_bp(void)
{
}
int ethsw_setup_led(void)
{
    return 0;
}
int ethsw_setup_phys(void)
{
    ethsw_shutdown_unused_phys();
    return 0;
}
int ethsw_enable_hw_switching(void)
{
    return 0;
}

/* Code to handle exceptions chip specific cases */
void ethsw_phy_handle_exception_cases(void)
{
}


void ethsw_port_mirror_get(int *enable, int *mirror_port, unsigned int *ing_pmap,
                           unsigned int *eg_pmap, unsigned int *blk_no_mrr,
                           int *tx_port, int *rx_port)
{
    printk("Runner port mirroring is not supported\n");
}

void ethsw_port_mirror_set(int enable, int mirror_port, unsigned int ing_pmap,
                           unsigned int eg_pmap, unsigned int blk_no_mrr,
                           int tx_port, int rx_port)
{
    printk("Runner port mirroring is not supported\n");
}


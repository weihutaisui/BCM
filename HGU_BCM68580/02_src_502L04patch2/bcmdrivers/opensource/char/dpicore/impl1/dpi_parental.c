/*
 * <:copyright-BRCM:2017:DUAL/GPL:standard 
 * 
 *    Copyright (c) 2017 Broadcom 
 *    All Rights Reserved
 * 
 * Unless you and Broadcom execute a separate written software license
 * agreement governing use of this software, this software is licensed
 * to you under the terms of the GNU General Public License version 2
 * (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
 * with the following added to such license:
 * 
 *    As a special exception, the copyright holders of this software give
 *    you permission to link this software with independent modules, and
 *    to copy and distribute the resulting executable under terms of your
 *    choice, provided that you also meet, for each linked independent
 *    module, the terms and conditions of the license of that module.
 *    An independent module is a module which is not derived from this
 *    software.  The special exception does not apply to any modifications
 *    of the software.
 * 
 * Not withstanding the above, under no circumstances may you combine
 * this software in any way with any other Broadcom software provided
 * under a license other than the GPL, without Broadcom's express prior
 * written consent.
 * 
 * :>
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/ip.h>
#include <linux/netfilter.h>
#include <linux/printk.h>
#include <net/netfilter/nf_conntrack.h>

#include <linux/blog.h>
#include <bcmdpi.h>

#include "dpi_local.h"

#define DPI_PARENTAL_MAX	32

/* ----- local variables ----- */
static DpictlParentalConfig_t parental[DPI_PARENTAL_MAX] = { 0 };


/* ----- local functions ----- */
static DpictlParentalConfig_t *dpi_find_parental(int app_id)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(parental); i++) {
		if (parental[i].app_id == app_id)
			return &parental[i];
	}

	return NULL;
}

int dpi_add_parental(DpictlParentalConfig_t *cfg)
{
	DpictlParentalConfig_t *entry = dpi_find_parental(cfg->app_id);

	if (entry) {
		pr_info("parental entry exists\n");
		return 0;
	}

	/* find first free entry in parental table */
	entry = dpi_find_parental(DPI_APPID_INVALID);
	if (!entry) {
		pr_err("no space for new parental control entry\n");
		return -1;
	}

	entry->app_id = cfg->app_id;
#if defined(CONFIG_BCM_KF_BLOG)
	blog_dm(DPI_PARENTAL, cfg->app_id, 0);
#endif
	return 0;
}

int dpi_del_parental(DpictlParentalConfig_t *cfg)
{
	DpictlParentalConfig_t *entry = dpi_find_parental(cfg->app_id);

	if (!entry)
		pr_info("parental entry doesn't exist\n");
	else
		entry->app_id = DPI_APPID_INVALID;

	return 0;
}

uint32_t dpi_parental_filter(struct sk_buff *skb)
{
	struct dpi_app *app = dpi_info(skb).app;

	if (!app)
		return NF_ACCEPT;
	/* if app exists in parental control table, drop by default */
	if (dpi_find_parental(app->app_id))
		return NF_DROP;

	return NF_ACCEPT;
}

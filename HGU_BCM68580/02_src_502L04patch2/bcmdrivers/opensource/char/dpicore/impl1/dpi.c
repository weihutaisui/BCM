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

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/if_ether.h>
#include <linux/netfilter_ipv6.h>
#include <linux/netlink.h>
#include <linux/udp.h>
#include <net/netfilter/nf_conntrack_acct.h>

#include <linux/dpi.h>
#include <linux/blog.h>
#include <bcmdpi.h>

#include <tdts.h>

#include "dpi_local.h"

#define DPI_DEFAULT_MAX_PKT	15

/* ----- local functions ----- */
static unsigned int nf_hook_local(const struct nf_hook_ops *ops,
				  struct sk_buff *skb,
				  const struct nf_hook_state *state);
static unsigned int nf_hook_forward(const struct nf_hook_ops *ops,
				    struct sk_buff *skb,
				    const struct nf_hook_state *state);

/* ----- global variables ----- */
struct dpi_stats dpi_stats = {0};

struct proc_dir_entry *dpi_dir;
EXPORT_SYMBOL(dpi_dir);

int dpi_enabled = 1;
EXPORT_SYMBOL(dpi_enabled);

/* ----- local variables ----- */
static struct nf_hook_ops hooks[] = {
	{
		.hook		= nf_hook_forward,
		.owner		= THIS_MODULE,
		.pf		= PF_INET,
		.hooknum	= NF_INET_FORWARD,
		.priority	= NF_IP_PRI_FILTER,
	},
	{
		.hook		= nf_hook_local,
		.owner		= THIS_MODULE,
		.pf		= PF_INET,
		.hooknum	= NF_INET_LOCAL_IN,
		.priority	= NF_IP_PRI_FILTER,
	},
#if defined(CONFIG_IPV6)
	{
		.hook		= nf_hook_forward,
		.owner		= THIS_MODULE,
		.pf		= PF_INET6,
		.hooknum	= NF_INET_FORWARD,
		.priority	= NF_IP6_PRI_FILTER,
	},
#endif
};

static struct sock *nl_sk;
static struct dpi_hooks dpi_hooks;
static int dpi_option_list[DPI_CONFIG_MAX];
static int dpi_max_pkt = DPI_DEFAULT_MAX_PKT;


/* ----- local functions ----- */
static uint64_t pkt_count(struct sk_buff *skb)
{
	struct nf_conn_acct *acct;
	struct nf_conn_counter *counter;

	if (!nfct(skb))
		return 0;

	acct = nf_conn_acct_find(nfct(skb));
	if (!acct)
		return 0;

	counter = acct->counter;
	return atomic64_read(&counter[0].packets) +
		atomic64_read(&counter[1].packets);
}

static int dir_downstream(struct nf_conn *ct, int dir)
{
	/*
	 * If the connection was initiated from the LAN side, then the reply
	 * direction is downstream. If the connection was initiated from the
	 * WAN side, then the originating direction is downstream.
	 */
	if (!DPI_IS_CT_INIT_FROM_WAN(ct))
		return (dir == IP_CT_DIR_REPLY);
	else
		return (dir == IP_CT_DIR_ORIGINAL);
}

static void dpi_classify_device(struct sk_buff *skb,
				tdts_pkt_parameter_t *pkt_param)
{
	struct dpi_dev *dev = dpi_info(skb).dev;

	dpi_stats.devs.lookups++;

	/* find or allocate device info */
	if (!dev) {
		dev = dpi_dev_find_or_alloc(eth_hdr(skb)->h_source);
		if (!dev)
			return;
		dpi_info(skb).dev = dev;
	}

	if (!tdts_check_pkt_parameter_res(pkt_param, TDTS_RES_TYPE_DEVID))
		return;

	/* save device classification information */
	dev->vendor_id = TDTS_PKT_PARAMETER_RES_DEVID_VENDOR_ID(pkt_param);
	dev->os_id     = TDTS_PKT_PARAMETER_RES_DEVID_OS_NAME_ID(pkt_param);
	dev->class_id  = TDTS_PKT_PARAMETER_RES_DEVID_OS_CLASS_ID(pkt_param);
	dev->type_id   = TDTS_PKT_PARAMETER_RES_DEVID_DEV_CAT_ID(pkt_param);
	dev->dev_id    = TDTS_PKT_PARAMETER_RES_DEVID_DEV_ID(pkt_param);

	set_bit(DPI_DEVID_STOP_CLASSIFY_BIT, &dpi_info(skb).flags);
	dpi_stats.devs.hits++;

	pr_debug("ct<%p> devid<%08x> type<%d> class<%d> vendor<%d> os<%d>\n",
		 nfct(skb), dev->dev_id, dev->type_id, dev->class_id,
		 dev->vendor_id, dev->os_id);
}

static void dpi_classify_application(struct sk_buff *skb,
				     tdts_pkt_parameter_t *pkt_param)
{
	struct dpi_app *app = dpi_info(skb).app;
	uint32_t app_id;

	dpi_stats.apps.lookups++;

	/* update flags */
	if (TDTS_PKT_PARAMETER_RES_APPID_CHECK_NOMORE(pkt_param))
		set_bit(DPI_APPID_STOP_CLASSIFY_BIT, &dpi_info(skb).flags);
	if (TDTS_PKT_PARAMETER_RES_APPID_CHECK_NOINT(pkt_param))
		set_bit(DPI_APPID_STOP_CLASSIFY_BIT, &dpi_info(skb).flags);

	if (!tdts_check_pkt_parameter_res(pkt_param, TDTS_RES_TYPE_APPID))
		return;
	app_id = dpi_get_app_id(TDTS_PKT_PARAMETER_RES_APPID_CAT_ID(pkt_param),
				TDTS_PKT_PARAMETER_RES_APPID_APP_ID(pkt_param),
				0);

	/* if this is a reclassification, set the current app and appinst to
	 * NULL to search again for new entries */
	if (app && app->app_id != app_id) {
		pr_debug("reclassify from <%08x> to <%08x>\n", app->app_id,
			 app_id);
		dpi_info(skb).app = NULL;
		dpi_info(skb).appinst = NULL;
		app = NULL;
	}

	/* find or allocate application info */
	if (!app) {
		app = dpi_app_find_or_alloc(app_id);
		if (!app)
			return;
		dpi_info(skb).app = app;
	}

	set_bit(DPI_APPID_IDENTIFIED_BIT, &dpi_info(skb).flags);
	if (TDTS_PKT_PARAMETER_RES_APPID_CHECK_FINAL(pkt_param)) {
		set_bit(DPI_APPID_FINAL_BIT, &dpi_info(skb).flags);
		set_bit(DPI_APPID_STOP_CLASSIFY_BIT, &dpi_info(skb).flags);
	}
	dpi_stats.apps.hits++;

	pr_debug("ct<%p> appid<%08x> [app<%u>(%s) cat<%u>(%s) beh<%u>(%s)] flags<%lx> [final<%d> nomore<%d> noint<%d>]\n",
		 nfct(skb),
		 app->app_id,
		 TDTS_PKT_PARAMETER_RES_APPID_APP_ID(pkt_param),
		 TDTS_PKT_PARAMETER_RES_APPID_APP_NAME(pkt_param),
		 TDTS_PKT_PARAMETER_RES_APPID_CAT_ID(pkt_param),
		 TDTS_PKT_PARAMETER_RES_APPID_CAT_NAME(pkt_param),
		 TDTS_PKT_PARAMETER_RES_APPID_BEH_ID(pkt_param),
		 TDTS_PKT_PARAMETER_RES_APPID_BEH_NAME(pkt_param),
		 dpi_info(skb).flags,
		 TDTS_PKT_PARAMETER_RES_APPID_CHECK_FINAL(pkt_param) ? 1 : 0,
		 TDTS_PKT_PARAMETER_RES_APPID_CHECK_NOMORE(pkt_param) ? 1 : 0,
		 TDTS_PKT_PARAMETER_RES_APPID_CHECK_NOINT(pkt_param) ? 1 : 0);
}

static void dpi_classify_url(struct sk_buff *skb,
			     tdts_pkt_parameter_t *pkt_param)
{
#ifdef DPI_URL_RECORD
	struct dpi_url *url = dpi_info(skb).url;
	char *hostname;
	int len;

	dpi_stats.urls.lookups++;

	hostname = TDTS_PKT_PARAMETER_RES_URL_DOMAIN(pkt_param);
	len = TDTS_PKT_PARAMETER_RES_URL_DOMAIN_LEN(pkt_param);
	if (!hostname || !len)
		return;

	/* find or allocate url info */
	if (!url) {
		url = dpi_url_find_or_alloc(hostname, len);
		if (!url)
			return;
		dpi_info(skb).url = url;
	}

	dpi_stats.urls.hits++;
#endif /* DPI_URL_RECORD */

	set_bit(DPI_URL_STOP_CLASSIFY_BIT, &dpi_info(skb).flags);
}

extern int tdts_shell_tcp_conn_remove(uint8_t ip_ver, uint8_t *sip,
		uint8_t *dip, uint16_t sport, uint16_t dport);

static void dpi_stop_classification(struct sk_buff *skb)
{
	int ret = 0;
	/* ignore all further classifications for this flow */
	set_bit(DPI_CLASSIFICATION_STOP_BIT, &dpi_info(skb).flags);

	/* remove connection from Trend's internal database */
	if (skb->protocol == htons(ETH_P_IP) && ip_hdr(skb)->protocol == IPPROTO_TCP)
		ret = tdts_shell_tcp_conn_remove(ip_hdr(skb)->version,
						 (uint8_t*) &ip_hdr(skb)->saddr,
						 (uint8_t*) &ip_hdr(skb)->daddr,
						 ntohs(tcp_hdr(skb)->source),
						 ntohs(tcp_hdr(skb)->dest));
	else if (skb->protocol == htons(ETH_P_IPV6) &&
		 ipv6_hdr(skb)->nexthdr == IPPROTO_TCP)
		ret = tdts_shell_tcp_conn_remove(ipv6_hdr(skb)->version,
						 (uint8_t*) &ipv6_hdr(skb)->saddr,
						 (uint8_t*) &ipv6_hdr(skb)->daddr,
						 ntohs(tcp_hdr(skb)->source),
						 ntohs(tcp_hdr(skb)->dest));
}

static void dpi_classify(struct sk_buff *skb, int lookup_flags)
{
	tdts_pkt_parameter_t pkt_param;
	unsigned long flags = dpi_info(skb).flags;

	if (test_bit(DPI_CLASSIFICATION_STOP_BIT, &flags))
		return;

	/* filter out classification based on current classification status */
	if (test_bit(DPI_APPID_STOP_CLASSIFY_BIT, &flags) &&
	    !blog_request(FLOWTRACK_ALG_HELPER, nfct(skb), 0, 0))
		lookup_flags &= ~SW_APP;
	if (test_bit(DPI_DEVID_STOP_CLASSIFY_BIT, &flags) ||
	    (dpi_info(skb).dev && dpi_info(skb).dev->dev_id))
		lookup_flags &= ~SW_DEVID;
#ifdef DPI_URL_RECORD
	if (test_bit(DPI_URL_STOP_CLASSIFY_BIT, &flags))
#endif
		lookup_flags &= ~SW_URL_QUERY;

	/* if we have no features to lookup, return */
	if (!lookup_flags)
		return;

	/* if we have classified too many packets, ignore further */
	if (pkt_count(skb) > dpi_max_pkt) {
		pr_debug("exceeded classification window<%d>, flags<%lx>\n",
			 dpi_max_pkt, flags);

		/* save stats */
		if (!test_bit(DPI_APPID_STOP_CLASSIFY_BIT, &flags))
			dpi_stats.apps.unclassified++;
		if (!test_bit(DPI_DEVID_STOP_CLASSIFY_BIT, &flags))
			dpi_stats.devs.unclassified++;
		if (!test_bit(DPI_URL_STOP_CLASSIFY_BIT, &flags))
			dpi_stats.urls.unclassified++;

		dpi_stop_classification(skb);
		return;
	}

	/* classify packet */
	dpi_stats.total_lookups++;
	tdts_init_pkt_parameter(&pkt_param, lookup_flags, 0);
	if (tdts_shell_dpi_l3_skb(skb, &pkt_param)) {
		pr_err("unable to scan packet\n");
		return;
	}

	/* handle classification results based on our lookup flags */
	if (lookup_flags & SW_DEVID)
		dpi_classify_device(skb, &pkt_param);
	if (lookup_flags & SW_APP)
		dpi_classify_application(skb, &pkt_param);
	if (lookup_flags & SW_URL_QUERY)
		dpi_classify_url(skb, &pkt_param);

	/* if there is no application instance but both app and device info are
	 * available, find or allocate an appinst. */
	if (!dpi_info(skb).appinst && dpi_info(skb).app && dpi_info(skb).dev) {
		dpi_info(skb).appinst =
			dpi_appinst_find_or_alloc(dpi_info(skb).app,
						  dpi_info(skb).dev);
	}

	/* if we have newly classified something, save stats */
	flags ^= dpi_info(skb).flags;
	if (test_bit(DPI_APPID_STOP_CLASSIFY_BIT, &flags))
		dpi_stats.apps.classified++;
	if (test_bit(DPI_DEVID_STOP_CLASSIFY_BIT, &flags))
		dpi_stats.devs.classified++;
#ifdef DPI_URL_RECORD
	if (test_bit(DPI_URL_STOP_CLASSIFY_BIT, &flags))
		dpi_stats.urls.classified++;
#endif

	/* if we have not yet identified everything, skip accelerating */
	if (!test_bit(DPI_APPID_STOP_CLASSIFY_BIT, &dpi_info(skb).flags) ||
	    !test_bit(DPI_DEVID_STOP_CLASSIFY_BIT, &dpi_info(skb).flags))
		blog_skip(skb, blog_skip_reason_dpi);

	/* if we have finished identifying everything, explicitly stop further
	 * classification calls and remove from Trend internal list (if TCP) */
	if (test_bit(DPI_APPID_STOP_CLASSIFY_BIT, &dpi_info(skb).flags) &&
	    test_bit(DPI_DEVID_STOP_CLASSIFY_BIT, &dpi_info(skb).flags) &&
	    test_bit(DPI_URL_STOP_CLASSIFY_BIT, &dpi_info(skb).flags))
		dpi_stop_classification(skb);
}

static int is_dhcp_pkt(const struct sk_buff *skb)
{
	if (skb->protocol == htons(ETH_P_IP))
		return (ip_hdr(skb)->protocol == IPPROTO_UDP) &&
			(udp_hdr(skb)->source == htons(68));
	else if (skb->protocol == htons(ETH_P_IPV6))
		/* FIXME: parse extention header */
		return (ipv6_hdr(skb)->nexthdr == IPPROTO_UDP) &&
			(udp_hdr(skb)->source == htons(546));
	return 0;
}

static unsigned int nf_hook_local(const struct nf_hook_ops *ops,
				  struct sk_buff *skb,
				  const struct nf_hook_state *state)
{
	if (!dpi_enabled)
		goto out;

	if (unlikely(!skb || !skb->dev || !skb->nfct)) {
		pr_debug("no skb or data, skipping packet\n");
		goto out;
	}

	/* assume only UDP packets (DHCP) contain device info */
	if (is_wan_dev(skb) || !is_dhcp_pkt(skb))
		goto out;

	dpi_classify(skb, SW_DEVID);

out:
	return NF_ACCEPT;
}

static unsigned int nf_hook_forward(const struct nf_hook_ops *ops,
				    struct sk_buff *skb,
				    const struct nf_hook_state *state)
{
	if (!dpi_enabled)
		goto out;

	if (unlikely(!skb || !skb->dev || !skb->nfct)) {
		pr_debug("no skb or data, skipping packet\n");
		goto out;
	}

	/* perform application and/or device classification */
	dpi_classify(skb, SW_APP | SW_URL_QUERY | SW_DEVID);

out:
	return NF_ACCEPT;
}


/* ----- netlink handling functions ----- */
static int dpi_config_option(unsigned short option, unsigned short value)
{
	if (option >= DPI_CONFIG_MAX)
		return -1;

	switch (option) {
	case DPI_CONFIG_PARENTAL:
		dpi_option_list[option] = value;
		break;

	default:
		pr_err("option '%d' is not supported\n", option);
		break;
	}

	return 0;
}

#define dpi_nl_data_ptr(_skb)  \
	(NLMSG_DATA((struct nlmsghdr *)(_skb)->data) + sizeof(DpictlMsgHdr_t))

static void dpi_nl_process_maxpkt(struct sk_buff *skb)
{
	int *cfg = (int *) dpi_nl_data_ptr(skb);

	if (!cfg)
		return;
	dpi_max_pkt = *cfg;
}

static void dpi_nl_process_config(struct sk_buff *skb)
{
	DpictlConfig_t *cfg = (DpictlConfig_t *) dpi_nl_data_ptr(skb);

	if (!cfg)
		return;
	dpi_config_option(cfg->option, cfg->value);
}

static void dpi_nl_config_parental(struct sk_buff *skb, uint32_t mode)
{
	DpictlParentalConfig_t *cfg =
		(DpictlParentalConfig_t *) dpi_nl_data_ptr(skb);

	if (!cfg)
		return;

	if (mode == DPICTL_NLMSG_ADD_PARENTAL)
		dpi_add_parental(cfg);
	else
		dpi_del_parental(cfg);
}

static void dpi_nl_process_status(struct sk_buff *skb)
{
	dpi_nl_msg_reply(skb, DPICTL_NLMSG_STATUS,
			 sizeof(dpi_enabled), &dpi_enabled);
}

static void dpi_nl_handler(struct sk_buff *skb)
{
	struct nlmsghdr *nlh = (struct nlmsghdr *)skb->data;
	DpictlMsgHdr_t *msg;

	if (skb->len < NLMSG_SPACE(0))
		return;

	msg = (DpictlMsgHdr_t *) NLMSG_DATA(nlh);
	if (nlh->nlmsg_len < sizeof(*nlh) || skb->len < nlh->nlmsg_len)
		return;

	switch (msg->type) {
	case DPICTL_NLMSG_ENABLE:
		dpi_enabled = 1;
		break;

	case DPICTL_NLMSG_DISABLE:
		dpi_enabled = 0;
		break;

	case DPICTL_NLMSG_STATUS:
		dpi_nl_process_status(skb);
		break;

	case DPICTL_NLMSG_CONFIG_OPT:
		dpi_nl_process_config(skb);
		break;

	case DPICTL_NLMSG_MAXPKT:
		dpi_nl_process_maxpkt(skb);
		break;

	case DPICTL_NLMSG_ADD_PARENTAL:
	case DPICTL_NLMSG_DEL_PARENTAL:
		dpi_nl_config_parental(skb, msg->type);
		break;

	case DPICTL_NLMSG_RESET_STATS:
		dpi_reset_stats();
		break;

	default:
		if (dpi_hooks.nl_handler && !dpi_hooks.nl_handler(skb))
			break;
		pr_err("unknown msg type '%d'\n", msg->type);
		break;
	}
}

/* ----- public interface functions ----- */
noinline struct dpi_info *dpi_info_get(struct nf_conn *conn)
{
	return &conn->dpi;
}
EXPORT_SYMBOL(dpi_info_get);

uint32_t dpi_appinst_app_id(struct dpi_appinst *appinst)
{
	return appinst->app ? appinst->app->app_id : 0;
}
EXPORT_SYMBOL(dpi_appinst_app_id);

uint8_t *dpi_appinst_mac(struct dpi_appinst *appinst)
{
	return appinst->dev ? appinst->dev->mac : NULL;
}
EXPORT_SYMBOL(dpi_appinst_mac);

uint32_t dpi_appinst_dev_id(struct dpi_appinst *appinst)
{
	return appinst->dev ? appinst->dev->dev_id : 0;
}
EXPORT_SYMBOL(dpi_appinst_dev_id);

struct dpi_ct_stats *dpi_appinst_stats(struct nf_conn *ct, int dir)
{
	if (dir_downstream(ct, dir))
		return &ct->dpi.appinst->ds;
	else
		return &ct->dpi.appinst->us;
}
EXPORT_SYMBOL(dpi_appinst_stats);

void dpi_print_flow(struct seq_file *s, struct nf_conn *ct)
{
	seq_printf(s, "%u %pM %u",
		   ct->dpi.app->app_id,
		   ct->dpi.dev->mac,
		   ct->dpi.dev->dev_id);
}
EXPORT_SYMBOL(dpi_print_flow);

void dpi_blog_key_get(struct nf_conn *conn, unsigned int *blog_key_0,
		      unsigned int *blog_key_1)
{
	*blog_key_0 = conn->blog_key[0];
	*blog_key_1 = conn->blog_key[1];
}
EXPORT_SYMBOL(dpi_blog_key_get);

int dpi_bind(struct dpi_hooks *hooks)
{
	dpi_hooks.nl_handler  = hooks->nl_handler;
	dpi_hooks.cpu_enqueue = hooks->cpu_enqueue;
	return 0;
}
EXPORT_SYMBOL(dpi_bind);

void dpi_nl_msg_reply(struct sk_buff *skb, int type, unsigned short len,
		      void *data)
{
	DpictlMsgHdr_t  *hdr;
	struct nlmsghdr *nlh = (struct nlmsghdr *)skb->data;
	struct nlmsghdr *new_nlh;
	struct sk_buff  *new_skb;
	int pid = nlh->nlmsg_pid;
	int buf_size;

	buf_size = NLMSG_SPACE(sizeof(DpictlMsgHdr_t) + len);
	new_skb = alloc_skb(buf_size, GFP_ATOMIC);
	if (!new_skb) {
		pr_err("couldn't allocate memory for skb\n");
		return;
	}

	new_nlh = nlmsg_put(new_skb, 0, 0, NLMSG_DONE, buf_size, 0);
	hdr = NLMSG_DATA(new_nlh);

	hdr->type = type;
	hdr->len = len;
	hdr++;

	memcpy(hdr, data, len);

	NETLINK_CB(new_skb).dst_group = 0;

	if (netlink_unicast(nl_sk, new_skb, pid, MSG_DONTWAIT) < 0)
		pr_err("error while sending DPI status\n");
}
EXPORT_SYMBOL(dpi_nl_msg_reply);

int dpi_cpu_enqueue(pNBuff_t pNBuff, struct net_device *dev)
{
	if (dpi_hooks.cpu_enqueue)
		return dpi_hooks.cpu_enqueue(pNBuff, dev);
	return -1;
}
EXPORT_SYMBOL(dpi_cpu_enqueue);


/* ----- driver funs ----- */
static int dpi_stat_seq_show(struct seq_file *s, void *v)
{
	seq_printf(s, "%-9s %-10s   %-10s   %-10s   %-10s\n",
		   "type", "lookups", "hits", "classified", "unclassified");
	seq_puts(s, "-------------------------------------------------------------\n");
	seq_printf(s, " app      %-10u   %-10u   %-10u   %-10u\n",
		   dpi_stats.apps.lookups,
		   dpi_stats.apps.hits,
		   dpi_stats.apps.classified,
		   dpi_stats.apps.unclassified);
	seq_printf(s, " device   %-10u   %-10u   %-10u   %-10u\n",
		   dpi_stats.devs.lookups,
		   dpi_stats.devs.hits,
		   dpi_stats.devs.classified,
		   dpi_stats.devs.unclassified);
	seq_printf(s, " url      %-10u   %-10u   %-10u   %-10u\n",
		   dpi_stats.urls.lookups,
		   dpi_stats.urls.hits,
		   dpi_stats.urls.classified,
		   dpi_stats.urls.unclassified);
	seq_printf(s, " total    %-10u\n", dpi_stats.total_lookups);
	seq_puts(s, "-------------------------------------------------------------\n");
	seq_printf(s, "    apps identified: %u\n", dpi_stats.app_count);
	seq_printf(s, " devices identified: %u\n", dpi_stats.dev_count);
	seq_printf(s, "appinsts identified: %u\n", dpi_stats.appinst_count);
	seq_printf(s, "    urls identified: %u\n", dpi_stats.url_count);
	return 0;
}
static int dpi_stat_open(struct inode *inode, struct file *file)
{
	return single_open(file, dpi_stat_seq_show, NULL);
};
static const struct file_operations dpi_stat_fops = {
	.open    = dpi_stat_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release
};

static int dpi_info_seq_show(struct seq_file *s, void *v)
{
	unsigned int val, timeout;

	seq_printf(s, " DPI engine version : %d.%d.%d%s\n",
		      TMCFG_E_MAJ_VER,
		      TMCFG_E_MID_VER,
		      TMCFG_E_MIN_VER,
		      TMCFG_E_LOCAL_VER);
	tdts_shell_system_setting_tcp_conn_max_get(&val);
	tdts_shell_system_setting_tcp_conn_timeout_get(&timeout);
	seq_printf(s, " Max TCP connections: %-8u  Timeout: %u\n", val,
		      timeout);
	tdts_shell_system_setting_udp_conn_max_get(&val);
	seq_printf(s, " Max UDP connections: %u\n", val);
	return 0;
}
static int dpi_info_open(struct inode *inode, struct file *file)
{
	return single_open(file, dpi_info_seq_show, NULL);
};
static const struct file_operations dpi_info_fops = {
	.open    = dpi_info_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release
};

static int __init dpicore_init(void)
{
	struct proc_dir_entry *pde;
	struct netlink_kernel_cfg cfg = {
		.groups = 0,
		.input  = dpi_nl_handler,
	};

	memset(&dpi_stats, 0, sizeof(dpi_stats));
	memset(&dpi_hooks, 0, sizeof(dpi_hooks));
	memset(dpi_option_list, 0, sizeof(dpi_option_list));

	/* create proc entries */
	dpi_dir = proc_mkdir("dpi", NULL);
	if (!dpi_dir) {
		pr_err("couldn't create dpi proc directory\n");
		goto err;
	}
	pde = proc_create("stats", 0440, dpi_dir, &dpi_stat_fops);
	if (!pde) {
		pr_err("couldn't create proc entry 'stats'\n");
		goto err_free_dpi_dir;
	}
	pde = proc_create("info", 0440, dpi_dir, &dpi_info_fops);
	if (!pde) {
		pr_err("couldn't create proc entry 'info'\n");
		goto err_free_stats;
	}

	/* initialize kernel tables */
	if (dpi_init_tables())
		goto err_free_info;

	/* register netfilter hooks */
	if (nf_register_hooks(hooks, ARRAY_SIZE(hooks))) {
		pr_err("cannot register netfilter hooks\n");
		goto err_free_tables;
	}

	nl_sk = netlink_kernel_create(&init_net, NETLINK_DPI, &cfg);
	if (!nl_sk) {
		pr_err("failed to create kernel netlink socket\n");
		goto err_unreg_nf_hooks;
	}
	return 0;

err_unreg_nf_hooks:
	nf_unregister_hooks(hooks, ARRAY_SIZE(hooks));
err_free_tables:
	dpi_deinit_tables();
err_free_info:
	remove_proc_entry("info", dpi_dir);
err_free_stats:
	remove_proc_entry("stats", dpi_dir);
err_free_dpi_dir:
	proc_remove(dpi_dir);
err:
	return -1;
}

static void __exit dpicore_exit(void)
{
	nf_unregister_hooks(hooks, ARRAY_SIZE(hooks));

	if (nl_sk)
		netlink_kernel_release(nl_sk);

	/* deinitialize tables */
	dpi_deinit_tables();

	/* remove proc entries */
	remove_proc_entry("info", dpi_dir);
	remove_proc_entry("stats", dpi_dir);
	proc_remove(dpi_dir);
}

module_init(dpicore_init);
module_exit(dpicore_exit);

MODULE_LICENSE("GPL");

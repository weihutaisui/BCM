#if defined(CONFIG_BCM_KF_DPI) && defined(CONFIG_BCM_DPI_MODULE)
#ifndef _LINUX_DPI_H
#define _LINUX_DPI_H

#include <linux/if_ether.h>
#include <linux/list.h>

#define DPI_APPID_ONGOING_BIT		0
#define DPI_APPID_IDENTIFIED_BIT	1
#define DPI_APPID_FINAL_BIT		2
#define DPI_APPID_STOP_CLASSIFY_BIT	3
#define DPI_APPID_RESYNC_BIT		4
#define DPI_DEVID_ONGOING_BIT		5
#define DPI_DEVID_IDENTIFIED_BIT	6
#define DPI_DEVID_FINAL_BIT		7
#define DPI_DEVID_STOP_CLASSIFY_BIT	8
#define DPI_URL_STOP_CLASSIFY_BIT	9
#define DPI_CLASSIFICATION_STOP_BIT	14
#define DPI_CT_INIT_FROM_WAN_BIT	15

#define DPI_IS_CT_INIT_FROM_WAN(ct) \
	test_bit(DPI_CT_INIT_FROM_WAN_BIT, &(ct)->dpi.flags)

struct dpi_ct_stats {
	unsigned long		pkts;
	unsigned long long	bytes;
	unsigned long		ts;
};

struct dpi_app;
struct dpi_dev;
struct dpi_appinst;
struct dpi_url;

struct dpi_info {
	struct dpi_dev *dev;
	struct dpi_app *app;
	struct dpi_appinst *appinst;
	struct dpi_url *url;
	unsigned long flags;
};

struct dpi_hooks {
	int (* nl_handler)(struct sk_buff *skb);
	int (* cpu_enqueue)(pNBuff_t pNBuff, struct net_device *dev);
};

struct nf_conn;

/* ----- dpicore driver functions ----- */
struct dpi_info *dpi_info_get(struct nf_conn *conn);
uint32_t dpi_appinst_app_id(struct dpi_appinst *appinst);
uint8_t *dpi_appinst_mac(struct dpi_appinst *appinst);
uint32_t dpi_appinst_dev_id(struct dpi_appinst *appinst);

struct dpi_ct_stats *dpi_appinst_stats(struct nf_conn *ct, int dir);
void dpi_print_flow(struct seq_file *s, struct nf_conn *ct);
int dpi_bind(struct dpi_hooks *hooks);
void dpi_blog_key_get(struct nf_conn *conn, unsigned int *blog_key_0,
		      unsigned int *blog_key_1);
void dpi_nl_msg_reply(struct sk_buff *skb, int type, unsigned short len,
		      void *data);
int dpi_cpu_enqueue(pNBuff_t pNBuff, struct net_device *dev);

/* ----- dpicore driver variables ----- */
extern struct proc_dir_entry *dpi_dir;
extern int dpi_enabled;

#endif /* _LINUX_DPI_H */
#endif /* defined(CONFIG_BCM_KF_DPI) && defined(CONFIG_BCM_DPI_MODULE) */

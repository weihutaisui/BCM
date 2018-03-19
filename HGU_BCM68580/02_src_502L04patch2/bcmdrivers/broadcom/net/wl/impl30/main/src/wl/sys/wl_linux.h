/*
 * wl_linux.c exported functions and definitions
 *
 * Copyright (C) 2017, Broadcom. All Rights Reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * $Id: wl_linux.h 713740 2017-08-01 09:53:04Z $
 */

#ifndef _wl_linux_h_
#define _wl_linux_h_

#include <wlc_types.h>

/* WL_ALL_PASSIVE should be defined for high-only driver */
#if !defined(WLC_LOW) && !defined(WL_ALL_PASSIVE)
#define WL_ALL_PASSIVE 1
#endif // endif

#define WL_LOCK_TAKEN    (TRUE)
#define WL_LOCK_NOTTAKEN (FALSE)

/* BMAC Note: High-only driver is no longer working in softirq context as it needs to block and
 * sleep so perimeter lock has to be a semaphore instead of spinlock. This requires timers to be
 * submitted to workqueue instead of being on kernel timer
 */
typedef struct wl_timer {
	struct timer_list 	timer;
	struct wl_info 		*wl;
	void 				(*fn)(void *);
	void				*arg; /* argument to fn */
	uint 				ms;
	bool 				periodic;
	bool 				set;
	struct wl_timer 	*next;
#ifdef BCMDBG
	char* 				name; /* Description of the timer */
	uint32				ticks;	/* how many timer timer fired */
#endif // endif
} wl_timer_t;

/* contortion to call functions at safe time */
/* In 2.6.20 kernels work functions get passed a pointer to the struct work, so things
 * will continue to work as long as the work structure is the first component of the task structure.
 */
typedef struct wl_task {
	struct work_struct work;
	void *context;
} wl_task_t;

/* This becomes netdev->priv and is the link between netdev and wlif struct */
typedef struct priv_link {
	wl_if_t *wlif;
} priv_link_t;

#define WL_DEV_IF(dev)          ((wl_if_t*)((priv_link_t*)DEV_PRIV(dev))->wlif)

#define WL_INFO(dev)	((wl_info_t*)(WL_DEV_IF(dev)->wl))	/* dev to wl_info_t */

#define TXQ_LOCK(_wl) spin_lock_bh(&(_wl)->txq_lock)
#define TXQ_UNLOCK(_wl) spin_unlock_bh(&(_wl)->txq_lock)

#ifdef CONFIG_SMP
#define WL_CONFIG_SMP()	TRUE
#else
#define WL_CONFIG_SMP()	FALSE
#endif /* CONFIG_SMP */

#ifdef DSLCPE_TX_PRIO
#define PKT_PRIO_BASE_CNT	8
#define PKT_PRIO_LVL		2
#endif /* DSLCPE_TX_PRIO */

#define WL_IFTYPE_BSS	1 /* iftype subunit for BSS */
#define WL_IFTYPE_WDS	2 /* iftype subunit for WDS */
#define WL_IFTYPE_MON	3 /* iftype subunit for MONITOR */

#ifdef PLC

#ifdef PLCDBG
#define WL_PLC(fmt, args...)	printk(fmt, ##args)
#else /* PLCDBG */
#define WL_PLC(fmt, args...)
#endif /* PLCDBG */

#define WL_PLC_IF_WDS		1
#define WL_PLC_IF_PLC		2
#define WL_PLC_IF_BR		3

#define WL_PLC_ACTION_TAG		0
#define WL_PLC_ACTION_UNTAG		1
#define WL_PLC_ACTION_NONE		2

#define WL_PLC_DUMMY_VID	1

#define WLIF_IS_WDS(wlif)	((wlif)->if_type == WL_IFTYPE_WDS)

#define NODE_TBL_SZ     32

struct wl_plc_node;

typedef struct wl_plc {
	bool	inited;		/* PLC initialized */
	int32	users;		/* Number of interfaces using PLC */
	int32	tx_vid;		/* VLAN used to send to PLC */
	int32	rx_vid1;	/* Frames rx'd on this VLAN will be sent to BR */
	int32	rx_vid2;	/* Frames rx'd on this VLAN will be sent to WDS/PLC */
	int32	rx_vid3;	/* Frames rx'd on this VLAN will be sent to PLC */
	struct net_device *plc_dev; /* PLC device (VID 3) used for sending */
	struct net_device *wl_dev; /* WL device that is master of the PLC */
	struct net_device *plc_rxdev1; /* PLC device (VID 4) used for receiving */
	struct net_device *plc_rxdev2; /* PLC device (VID 5) used for sending & receiving */
	struct net_device *plc_rxdev3; /* PLC device (VID 6) used for sending & receiving */
	uint32	wifi_tx;	/* Frames sent on wireless link */
	uint32	wifi_rx;	/* Frames received on wireless link */
	uint32	plc_tx;		/* Frames sent on plc link */
	uint32	plc_rx;		/* Frames received on plc link */
	wlc_bsscfg_t *cfg;	/* Master bss config */
	struct wl_plc_node *node_tbl[NODE_TBL_SZ];
#ifdef PLC_WET
	struct wl_timer *plc_wdtimer;	/* plc watchdog timer for plc node aging out */
	uint32 plc_wdtimer_interval;	/* plc watchdog timer interval */
#endif /* PLC_WET */
} wl_plc_t;
#endif /* PLC */

struct wl_if {
#ifdef USE_IW
	wl_iw_t		iw;		/* wireless extensions state (must be first) */
#endif /* USE_IW */
#ifdef DSLCPE
	void *(*nic_hook_fn)(int cmd,void *p,void *p2);
#endif
	struct wl_if *next;
	struct wl_info *wl;		/* back pointer to main wl_info_t */
	struct net_device *dev;		/* virtual netdevice */
#if defined(BCM_GMAC3)
	struct fwder *fwdh;     /* pointer to forwarder handle */
#endif /* BCM_GMAC3 */
	struct wlc_if *wlcif;		/* wlc interface handle */
	uint subunit;			/* WDS/BSS unit */
	bool dev_registered;	/* netdev registed done */
	int  if_type;			/* WL_IFTYPE */
	char name[IFNAMSIZ];		/* netdev may not be alloced yet, so store the name
					   here temporarily until the netdev comes online
					 */
#ifdef ARPOE
	wl_arp_info_t	*arpi;		/* pointer to arp agent offload info */
#endif /* ARPOE */
	struct net_device_stats stats;  /* stat counter reporting structure */
	uint    stats_id;               /* the current set of stats */
	struct net_device_stats stats_watchdog[2]; /* ping-pong stats counters updated */
	                                           /* by Linux watchdog */
#ifdef USE_IW
	struct iw_statistics wstats_watchdog[2];
	struct iw_statistics wstats;
	int             phy_noise;
#endif /* USE_IW */
#ifdef PLC
	wl_plc_t *plc;			/* PLC interface information */
#endif /* PLC */
#if defined(PKTC_TBL)
	struct pktc_info	*pktci;
#endif // endif
};

struct rfkill_stuff {
	struct rfkill *rfkill;
	char rfkill_name[32];
	char registered;
};

struct wl_info {
	uint		unit;		/* device instance number */
	wlc_pub_t	*pub;		/* pointer to public wlc state */
	void		*wlc;		/* pointer to private common os-independent data */
	osl_t		*osh;		/* pointer to os handler */
#if defined(BCM_GMAC3)
	struct fwder *fwdh;		/* pointer to forwarder handle */
	int			fwder_unit; /* assigned fwder unit (modulo-FWDER_MAX_UNIT) */
#endif /* BCM_GMAC3 */
#ifdef HNDCTF
	ctf_t		*cih;		/* ctf instance handle */
#endif /* HNDCTF */
	struct net_device *dev;		/* backpoint to device */

	struct semaphore sem;		/* use semaphore to allow sleep */
	spinlock_t	lock;		/* per-device perimeter lock */
	spinlock_t	isr_lock;	/* per-device ISR synchronization lock */

	uint		bcm_bustype;	/* bus type */
	bool		piomode;	/* set from insmod argument */
	void *regsva;			/* opaque chip registers virtual address */
	wl_if_t *if_list;		/* list of all interfaces */
	atomic_t callbacks;		/* # outstanding callback functions */
	struct wl_timer *timers;	/* timer cleanup queue */
#ifndef NAPI_POLL
	struct tasklet_struct tasklet;	/* dpc tasklet */
	struct tasklet_struct tx_tasklet; /* tx tasklet */
#endif /* NAPI_POLL */

#if defined(NAPI_POLL) && (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 30))
	struct napi_struct napi;
#endif /* defined(NAPI_POLL) && (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 30)) */

	struct net_device *monitor_dev;	/* monitor pseudo device */
	uint		monitor_type;	/* monitor pseudo device */
	bool		resched;	/* dpc needs to be and is rescheduled */
#ifdef TOE
	wl_toe_info_t	*toei;		/* pointer to toe specific information */
#endif // endif
#ifdef ARPOE
	wl_arp_info_t	*arpi;		/* pointer to arp agent offload info */
#endif // endif
	uint32		pci_psstate[16];	/* pci ps-state save/restore */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 14)
#define NUM_GROUP_KEYS 4
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 29)
	struct lib80211_crypto_ops *tkipmodops;
#else
	struct ieee80211_crypto_ops *tkipmodops;	/* external tkip module ops */
#endif // endif
	struct ieee80211_tkip_data  *tkip_ucast_data;
	struct ieee80211_tkip_data  *tkip_bcast_data[NUM_GROUP_KEYS];
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 14) */
	/* RPC, handle, lock, txq, workitem */
#ifdef WLC_HIGH_ONLY
	rpc_info_t 	*rpc;		/* RPC handle */
	rpc_tp_info_t	*rpc_th;	/* RPC transport handle */
	wlc_rpc_ctx_t	rpc_dispatch_ctx;

	bool	   rpcq_dispatched;	/* Avoid scheduling multiple tasks */
	spinlock_t rpcq_lock;		/* Lock for the queue */
	rpc_buf_t *rpcq_head;		/* RPC Q */
	rpc_buf_t *rpcq_tail;		/* Points to the last buf */
	int        rpcq_len;		/* length of RPC queue */
#endif /* WLC_HIGH_ONLY */

	bool		txq_dispatched;	/* Avoid scheduling multiple tasks */
	spinlock_t	txq_lock;	/* Lock for the queue */
#ifndef DSLCPE_TX_PRIO
	struct sk_buff	*txq_head;	/* TX Q */
	struct sk_buff	*txq_tail;	/* Points to the last buf */
	int		txq_cnt;	/* the txq length */
#else
	struct sk_buff *txq_head[PKT_PRIO_BASE_CNT*PKT_PRIO_LVL];	/* TX Q */
	struct sk_buff *txq_tail[PKT_PRIO_BASE_CNT*PKT_PRIO_LVL];	/* Points to the last buf */
	int		txq_cnt[PKT_PRIO_BASE_CNT*PKT_PRIO_LVL];	/* the txq length */
#endif /* DSLCPE_TX_PRIO */
#ifdef WL_ALL_PASSIVE
	wl_task_t	txq_task;	/* work queue for wl_start() */
	wl_task_t	multicast_task;	/* work queue for wl_set_multicast_list() */

#if defined(PKTC_TBL)
	bool		txq_txchain_dispatched;	/* dispatched flag for wl thread */
#endif // endif
#ifdef WLC_LOW
	wl_task_t	wl_dpc_task;	/* work queue for wl_dpc() */
	bool		all_dispatch_mode;
#endif /* WLC_LOW */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 4, 0))
	int processor_id; /* TP id for WLAN TX/RX */
	struct task_struct      *kthread;
	wait_queue_head_t       kthread_wqh;
#ifdef WLC_LOW
	bool                    rxq_dispatched;
#endif // endif
#endif /* WL_USE_L34_THREAD */
#endif /* WL_ALL_PASSIVE */
#ifdef DSLCPE_PREALLOC_SKB
	bool			prealloc_skb_mode;
#endif /* DSLCPE_PREALLOC_SKB */

#ifdef WL_THREAD
    struct task_struct      *thread;
    wait_queue_head_t       thread_wqh;
    struct sk_buff_head     tx_queue;
    struct sk_buff_head     rpc_queue;
#endif /* WL_THREAD */
#if defined(WL_CONFIG_RFKILL)
	struct rfkill_stuff wl_rfkill;
	mbool last_phyind;
#endif /* defined(WL_CONFIG_RFKILL) */

	uint processed;		/* Number of rx frames processed */
	struct proc_dir_entry *proc_entry;
#ifdef WLOFFLD
	uchar* bar1_addr;
	uint32 bar1_size;
#endif // endif
#ifdef BCM_WFD
	int wfd_idx;
#endif // endif
};

#ifdef LINUX_HYBRID
#define HYBRID_PROC   "brcm_monitor"
#endif // endif

#if (defined(NAPI_POLL) && defined(WL_ALL_PASSIVE))
#error "WL_ALL_PASSIVE cannot co-exists w/ NAPI_POLL"
#endif /* defined(NAPI_POLL) && defined(WL_ALL_PASSIVE) */

#if defined(BCM_GMAC3)
#define WLIF_FWDER(wlif)        ((wlif)->fwdh != FWDER_NULL)
#else
#define WLIF_FWDER(wlif)        (FALSE)
#endif /* BCM_GMAC3 */

#if defined(BCM_GMAC3)
#define WL_ALL_PASSIVE_ENAB(wl) 0
#elif defined(WL_ALL_PASSIVE_ON) || defined(WLC_HIGH_ONLY)
#define WL_ALL_PASSIVE_ENAB(wl)	1
#else
#ifdef WL_ALL_PASSIVE
#define WL_ALL_PASSIVE_ENAB(wl)	(!(wl)->all_dispatch_mode)
#else
#define WL_ALL_PASSIVE_ENAB(wl)	0
#endif /* WL_ALL_PASSIVE */
#endif /* !BCM_GMAC3 && !(WL_ALL_PASSIVE_ON || WLC_HIGH_ONLY) */

/* perimeter lock */
#define WL_LOCK(wl)	do { \
				if (WL_ALL_PASSIVE_ENAB(wl)) \
					down(&(wl)->sem); \
				else \
					spin_lock_bh(&(wl)->lock); \
			} while (0)

#define WL_UNLOCK(wl)	do { \
				if (WL_ALL_PASSIVE_ENAB(wl)) \
					up(&(wl)->sem); \
				else \
					spin_unlock_bh(&(wl)->lock); \
			} while (0)

#if defined(BCM_GMAC3)
#define WL_LOCK_TRY(wl, lock_taken)	do { \
						if (lock_taken == WL_LOCK_NOTTAKEN) { \
							WL_LOCK(wl); \
						} \
					} while (0)

#define WL_UNLOCK_TRY(wl, lock_taken)	do { \
						if (lock_taken == WL_LOCK_NOTTAKEN) { \
							WL_UNLOCK(wl); \
						} \
					} while (0)
#else
#define WL_LOCK_TRY(wl, lock_taken)	WL_LOCK(wl)
#define WL_UNLOCK_TRY(wl, lock_taken)	WL_UNLOCK(wl)
#endif /* BCM_GMAC3 */

#ifdef WLC_HIGH_ONLY
/* locking from inside wl_isr */
#define WL_ISRLOCK(wl, flags)
#define WL_ISRUNLOCK(wl, flags)

#else
/* locking from inside wl_isr */
#define WL_ISRLOCK(wl, flags) do {spin_lock(&(wl)->isr_lock); (void)(flags);} while (0)
#define WL_ISRUNLOCK(wl, flags) do {spin_unlock(&(wl)->isr_lock); (void)(flags);} while (0)

/* locking under WL_LOCK() to synchronize with wl_isr */
#define INT_LOCK(wl, flags)	spin_lock_irqsave(&(wl)->isr_lock, flags)
#define INT_UNLOCK(wl, flags)	spin_unlock_irqrestore(&(wl)->isr_lock, flags)
#endif	/* WLC_HIGH_ONLY */

/* handle forward declaration */
typedef struct wl_info wl_info_t;

#ifndef PCI_D0
#define PCI_D0		0
#endif // endif

#ifndef PCI_D3hot
#define PCI_D3hot	3
#endif // endif

/* exported functions */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 20)
extern irqreturn_t wl_isr(int irq, void *dev_id);
#else
extern irqreturn_t wl_isr(int irq, void *dev_id, struct pt_regs *ptregs);
#endif // endif

extern int wl_start_int(wl_info_t *wl, wl_if_t *wlif, struct sk_buff *skb);
#if defined(WL_ALL_PASSIVE) && defined(WLC_LOW)
void wl_dpc_rxwork(struct wl_task *task);
#endif /* WL_ALL_PASSIVE && WLC_LOW */
extern int __devinit wl_pci_probe(struct pci_dev *pdev, const struct pci_device_id *ent);
extern void wl_free(wl_info_t *wl);
extern int  wl_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd);
extern struct net_device * wl_netdev_get(wl_info_t *wl);
extern wlc_bsscfg_t * wl_bsscfg_find(wl_if_t *wlif);

#ifdef BCM_WL_EMULATOR
extern wl_info_t *  wl_wlcreate(osl_t *osh, void *pdev);
#endif // endif

#endif /* _wl_linux_h_ */

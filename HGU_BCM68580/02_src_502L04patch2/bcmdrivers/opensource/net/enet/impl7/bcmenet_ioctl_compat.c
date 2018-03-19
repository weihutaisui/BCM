/*
   <:copyright-BRCM:2015:DUAL/GPL:standard
   
      Copyright (c) 2015 Broadcom 
      All Rights Reserved
   
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

/*
 *  Created on: Dec/2015
 *      Author: ido@broadcom.com
 */

#include "enet.h"
#include "port.h"
#include "enet_dbg.h"
#include <bcm/bcmswapitypes.h>
#include <bcmnet.h>
#include <boardparms.h>
#ifdef RUNNER
#include "runner.h"
#endif
#ifdef SF2_DEVICE
#include "sf2.h"
#endif
#ifdef CONFIG_BCM_PTP_1588
#include "ptp_1588.h"
#endif
#if defined(DSL_RUNNER_DEVICE)
#include "phy_drv_sf2.h"    // TODO_DSL: need to move reference to C45 phy out
#endif
#include "opticaldet.h"
#include "bcmenet_common.h"
#include "crossbar_dev.h"

extern int apd_enabled;
extern int eee_enabled;

#if defined(CONFIG_BCM_ETH_PWRSAVE)
static int pm_apd_set_single(enetx_port_t *p, void *ctx)
{
    phy_dev_t *phy_dev = p->p.phy;

    if (phy_dev)
        phy_dev_apd_set(phy_dev, apd_enabled);

    return 0;
}
#endif

#if defined(CONFIG_BCM_ENERGY_EFFICIENT_ETHERNET)
static int pm_eee_set_single(enetx_port_t *p, void *ctx)
{
    mac_dev_t *mac_dev = p->p.mac;
    phy_dev_t *phy_dev = p->p.phy;

    if (mac_dev)
        mac_dev_eee_set(mac_dev, 0);

    if (phy_dev)
        phy_dev_eee_set(phy_dev, eee_enabled);

    return 0;
}
#endif

#if defined(CONFIG_BCM_ETH_PWRSAVE)
static int pm_apd_get(void)
{
    return apd_enabled;
}

static void pm_apd_set(int enabled)
{
    if (apd_enabled == enabled)
        return;

    apd_enabled = enabled;
    port_traverse_ports(root_sw, pm_apd_set_single, PORT_CLASS_PORT, NULL);
}
#endif

#if defined(CONFIG_BCM_ENERGY_EFFICIENT_ETHERNET)
static int pm_eee_get(void)
{
    return eee_enabled;
}

static void pm_eee_set(int enabled)
{
    if (eee_enabled == enabled)
        return;

    eee_enabled = enabled;
    port_traverse_ports(root_sw, pm_eee_set_single, PORT_CLASS_PORT, NULL);
}
#endif

enetx_port_t *unit_port_array[BP_MAX_ENET_MACS][COMPAT_MAX_SWITCH_PORTS] = {}; 

int unit_port_oam_idx_array[BP_MAX_ENET_MACS][COMPAT_MAX_SWITCH_PORTS] = {[0 ... BP_MAX_ENET_MACS-1][0 ... COMPAT_MAX_SWITCH_PORTS-1] = -1}; 
#define COMPAT_PORT(u, p) ((unit_port_array[u][p] && (unit_port_array[u][p]->port_class & (PORT_CLASS_PORT|PORT_CLASS_PORT_UNDEF))) ? \
    unit_port_array[u][p] : NULL)
#define COMPAT_OAM_IDX(u, p) (COMPAT_PORT(u, p) ? unit_port_oam_idx_array[u][p] : -1)

#ifdef RUNNER
#define COMPAT_RPDA(u, p) _port_rdpa_object_by_port(_compat_port_object_from_unit_port(u, p))
#endif

#ifdef GPON
static int _handle_gpon(struct ifreq *rq)
{
    struct gponif_data gpn;
    static enetx_port_t *gpon_port;
    port_info_t port_info =
    {
        .is_gpon = 1,
    };

    if (copy_from_user(&gpn, rq->ifr_data, sizeof(gpn)))
        return -EFAULT;

    switch (gpn.op)
    {
        case CREATEGPONVPORT:
            if (gpon_port)
                return -EALREADY;

            if (!(gpon_port = port_create(&port_info, root_sw)))
                return -EFAULT;

            gpon_port->n.rtnl_is_locked = 1;
            gpon_port->has_interface = 1;
            if ((gpon_port->p.phy = phy_dev_add(PHY_TYPE_PON, 0, NULL)) == NULL)
                return -EFAULT;

            if (phy_driver_init(PHY_TYPE_PON))
                return -EFAULT;

            strcpy(gpon_port->name, "gpondef");
            if (sw_init(root_sw))
                return -EFAULT;
            
            gpon_port->dev->path.hw_subport_mcast_idx = NETDEV_PATH_HW_SUBPORTS_MAX;

            return 0;

        case DELETEALLGPONVPORTS:
        case DELETEGPONVPORT:
            if (!gpon_port)
                return -ENOENT;

            gpon_port->n.rtnl_is_locked = 1;
            sw_free(&gpon_port);
            return 0;

        case GETFREEGEMIDMAP:
        case SETGEMIDMAP:
        case GETGEMIDMAP:
            return 0;
        case SETMCASTGEMID:
            {
                int i;
            
                if (!gpon_port)
                    return -ENOENT;

                for (i = 0; i < CONFIG_BCM_MAX_GEM_PORTS; i++)
                {
                    if (!gpn.gem_map_arr[i])
                        continue;

                    netdev_path_set_hw_subport_mcast_idx(gpon_port->dev, i);
                    break;
                }

                return 0;
            }
    }

    return -EOPNOTSUPP;
}
#endif

#ifdef EPON
static int _handle_epon(int ethctl_op)
{
    static enetx_port_t *epon_port;
    port_info_t port_info =
    {
        .is_epon = 1,
    };

    switch (ethctl_op)
    {
        case ETHCREATEEPONVPORT:
            if (epon_port)
                return -EALREADY;

            if (!(epon_port = port_create(&port_info, root_sw)))
                return -EFAULT;

            epon_port->n.rtnl_is_locked = 1;
            epon_port->has_interface = 1;
            if ((epon_port->p.phy = phy_dev_add(PHY_TYPE_PON, 0, NULL)) == NULL)
                return -EFAULT;

            if (phy_driver_init(PHY_TYPE_PON))
                return -EFAULT;

            strcpy(epon_port->name, "epon0");
            if (sw_init(root_sw))
                return -EFAULT;

            return 0;
    }
    
    return -EOPNOTSUPP;
}

static int tr_init_detect_epon_ae(enetx_port_t *port, void *_ctx)
{
    struct ethctl_data *ethctl = (struct ethctl_data *)_ctx;
    int port_id = ethctl->val;

    if (port->p.port_id != port_id)
        return 0;

    port->n.rtnl_is_locked = 1;
    if(!port->p.mac)
    {
        port->p.mac = mac_dev_add(MAC_TYPE_EPON_AE, 0, NULL);
        mac_driver_init(MAC_TYPE_EPON_AE);
    }
    if (port_init_detect(port, PORT_TYPE_RUNNER_EPON))
        return -1;
    
    strncpy(ethctl->ifname, port->dev->name, strlen(port->dev->name));

    return 1;
}

static int _handle_epon_ae(struct ifreq *rq, struct ethctl_data *ethctl)
{
    static enetx_port_t *ae_port;
    port_info_t port_info =
    {
        .is_epon = 1,
    };

    switch (ethctl->op)
    {
        case ETHCREATEEPONAEVPORT:
            if (port_traverse_ports(root_sw, tr_init_detect_epon_ae, PORT_CLASS_PORT_DETECT, ethctl) <= 0)
            {
                /* Detect port doesn't exist, create it */
                if (!(ae_port = port_create(&port_info, root_sw)))
                    return -EFAULT;
                
                /* put port into tail of unit_port_array */
                unit_port_array[0][BP_MAX_SWITCH_PORTS-1] = ae_port;
                ae_port->n.rtnl_is_locked = 1;
                ae_port->has_interface = 1;
                if ((ae_port->p.phy = phy_dev_add(PHY_TYPE_PON, 0, NULL)) == NULL)
                    return -EFAULT;
                ae_port->p.mac = mac_dev_add(MAC_TYPE_EPON_AE, 0, ae_port->p.phy);
                
                if (mac_driver_init(MAC_TYPE_EPON_AE))
                    return -EFAULT;
                if (phy_driver_init(PHY_TYPE_PON))
                    return -EFAULT;
                
                if (sw_init(root_sw))
                    return -EFAULT;
                
                memcpy((void*)ethctl->ifname, (void*)ae_port->dev->name, sizeof(ae_port->dev->name));
            }

            if (copy_to_user((void *)rq->ifr_data, (void *)ethctl, sizeof(*ethctl)))
                return -EFAULT;
            
            return 0;
    }
    
    return -EOPNOTSUPP;
}

#endif
                        
#if defined(CONFIG_BCM96838) || defined(CONFIG_BCM96848)
static int tr_init_detect(enetx_port_t *port, void *_ctx)
{
    struct ethctl_data *ethctl = (struct ethctl_data *)_ctx;
    int port_id = ethctl->val;

    if (port->p.port_id != port_id)
        return 0;

    port->n.rtnl_is_locked = 1;
    if (port_init_detect(port, PORT_TYPE_RUNNER_PORT))
        return -1;
    
    strncpy(ethctl->ifname, port->dev->name, strlen(port->dev->name));

    return 1;
}

static int _handle_init_wan(struct ifreq *rq, struct ethctl_data *ethctl)
{
    int rc;

    if ((rc = port_traverse_ports(root_sw, tr_init_detect, PORT_CLASS_PORT_DETECT, ethctl)) <= 0)
    {
        if (rc == 0)
            enet_err("did not find any auto detect ports with index %d\n", ethctl->val);

        return -EFAULT;
    }

    if (copy_to_user((void *)rq->ifr_data, ethctl, sizeof(*ethctl)))
        return -EFAULT;

    return 0;
}
#endif

static int tr_port_by_netdev(enetx_port_t *port, void *_ctx)
{
    if (port->dev == *(struct net_device **)_ctx)
    {
        *(enetx_port_t **)_ctx = port;
        return 1;
    }
    return 0;
}

int port_by_netdev(struct net_device *dev, enetx_port_t **match)
{
    void *in_out = (void *)dev;
    
    *match = NULL;
    if (port_traverse_ports(root_sw, tr_port_by_netdev, PORT_CLASS_PORT|PORT_CLASS_SW, &in_out) <= 0)
        return -1;
    *match =(enetx_port_t *)in_out;
    return 0;
}

typedef struct list_ctx
{ 
    int maxlen;
    char *str;
    int op_code;
} list_ctx;

static int _devname_concat(enetx_port_t *port, char *buf, int maxlen)
{
    int dev_len, buf_len;

    if (!port->dev)
        return 0;

    buf_len = strlen(buf);
    dev_len = strlen(port->dev->name) + (buf[0] ? 1 : 0);

    if (buf_len + dev_len >= maxlen)
        return -1;

    if (buf[0])
        strcat(buf, ",");

    strcat(buf, port->dev->name);

    return 0;
}

static int tr_wan_devname_concat(enetx_port_t *port, void *_ctx)
{
    list_ctx *ctx = (list_ctx *)_ctx;

    if (port->n.port_netdev_role == PORT_NETDEV_ROLE_WAN &&
        _devname_concat(port, ctx->str, ctx->maxlen) < 0)
    {
        return -1;
    }

    return 0;
}

static int tr_ifreq_devname_concat(enetx_port_t *port, void *_ctx)
{
    list_ctx *ctx = (list_ctx *)_ctx;
    
    if (port->p.port_cap == ctx->op_code && _devname_concat(port, ctx->str, ctx->maxlen) < 0)
        return -1;

    return 0;
}

static int get_root_dev_by_name(char *ifname, struct net_device **_dev)
{ 
    struct net_device *dev;

    if (!_dev)
        return -1;

    dev = dev_get_by_name(&init_net, ifname);
    if (!dev)
        return -1;

    /* find root device */
    while (1)
    {
        if (netdev_path_is_root(dev))
            break;

        dev_put(dev);
        dev = netdev_path_next_dev(dev);
        dev_hold(dev);
    }

    *_dev = dev;
    dev_put(dev);

    return 0;
}

int get_port_by_if_name(char *ifname, enetx_port_t **port)
{
    struct net_device *dev;

    if (get_root_dev_by_name(ifname, &dev))
    {
        enet_err("illegal device name %s\n", ifname);
        return -EFAULT;
    }

    if (port_by_netdev(dev, port))
    {
        enet_err("is not an enet dev %s\n", dev->name);
        return -EFAULT;
    }
    return 0;
}

/* returns if a port with a phy is in up state */
static int tr_is_phy_up(enetx_port_t *port, void *_ctx)
{
    if (port->p.phy)
        return port->p.phy->link ? 1 : 0;

    return 0;
}
    
static int tr_stats_clear(enetx_port_t *port, void *_ctx)
{
    port_stats_clear(port);
    return 0;
}

#define ESTATS_FMT "  %-40s: %10lu;   %-40s: %10lu"
#define ESTATS_VAL(v1, v2) #v1, estats->v1, #v2, estats->v2
#define ESTATS_FMT1 "  %-40s: %10lu"
#define ESTATS_FMT1b "  %-40s: %10s"
#define ESTATS_VAL1b(v1) #v1, v1? "True": "False"
#define ESTATS_VAL1(v1) #v1, v1
#define ESTATS_VAL2(v1, v2) #v1, v1, #v2, v2
typedef struct counterSum
{
    unsigned long rxIn, rx2Kernel, rx2Blog, rxDrops,
                  txIn, txOut, txDrops, txExtraChain;
} counterSum_t;

static int display_enet_dev_stats(enetx_port_t *port, void *ctx)
{
    // based on imp5\bcmenet.c display_enet_dev_stats()
    port_stats_t *estats = &port->n.port_stats;
    counterSum_t *cts = (counterSum_t *)ctx;
    counterSum_t portCnt = {};
    int i;

    if (port->p.child_sw) return 0;  // skip imp port

    printk("Device %s:\n", port->dev->name);
    printk(ESTATS_FMT "\n", ESTATS_VAL(tx_packets, rx_packets));
    printk(ESTATS_FMT "\n", ESTATS_VAL(tx_bytes, rx_bytes));
    printk(ESTATS_FMT "\n\n", ESTATS_VAL(tx_dropped, rx_dropped));

#if defined(DEBUG_STATS)
    printk(ESTATS_FMT "\n", ESTATS_VAL(rx_packets_queue[0], rx_packets_queue[1]));
    printk(ESTATS_FMT "\n", ESTATS_VAL(rx_packets_queue[2], rx_packets_queue[3]));
    printk(ESTATS_FMT "\n", ESTATS_VAL(rx_packets_queue[4], rx_packets_queue[5]));
    printk(ESTATS_FMT "\n", ESTATS_VAL(rx_packets_queue[6], rx_packets_queue[7]));
    printk(ESTATS_FMT "\n", ESTATS_VAL(tx_packets_queue_in[0], tx_packets_queue_in[1]));
    printk(ESTATS_FMT "\n", ESTATS_VAL(tx_packets_queue_in[2], tx_packets_queue_in[3]));
    printk(ESTATS_FMT "\n", ESTATS_VAL(tx_packets_queue_in[4], tx_packets_queue_in[5]));
    printk(ESTATS_FMT "\n", ESTATS_VAL(tx_packets_queue_in[6], tx_packets_queue_in[7]));
    printk(ESTATS_FMT "\n", ESTATS_VAL(tx_packets_queue_out[0], tx_packets_queue_out[1]));
    printk(ESTATS_FMT "\n", ESTATS_VAL(tx_packets_queue_out[2], tx_packets_queue_out[3]));
    printk(ESTATS_FMT "\n", ESTATS_VAL(tx_packets_queue_out[4], tx_packets_queue_out[5]));
    printk(ESTATS_FMT "\n", ESTATS_VAL(tx_packets_queue_out[6], tx_packets_queue_out[7]));
    printk(ESTATS_FMT "\n", ESTATS_VAL(rx_dropped_no_rxdev, rx_dropped_blog_drop));
    printk(ESTATS_FMT "\n", ESTATS_VAL(rx_dropped_no_skb, rx_packets_blog_done));
    printk(ESTATS_FMT "\n", ESTATS_VAL(rx_dropped_no_srcport, rx_packets_netif_receive_skb));
    printk(ESTATS_FMT "\n", ESTATS_VAL(tx_dropped_dispatch, tx_dropped_mux_failed ));
    printk(ESTATS_FMT "\n", ESTATS_VAL(tx_dropped_no_skb, tx_dropped_bad_nbuff ));
    printk(ESTATS_FMT "\n", ESTATS_VAL(tx_dropped_no_lowlvl_resource, tx_dropped_no_fkb));
    printk(ESTATS_FMT "\n", ESTATS_VAL(tx_dropped_runner_lan_fail, tx_dropped_runner_wan_fail));

    for(i = 0; i<MAX_NUM_OF_PRI_QS; i++)
    {
        portCnt.rxIn += estats->rx_packets_queue[i]; 
        portCnt.txIn += estats->tx_packets_queue_in[i]; 
        portCnt.txOut += estats->tx_packets_queue_out[i]; 
    }

    portCnt.rx2Kernel = estats->rx_packets_netif_receive_skb;
    portCnt.rx2Blog = estats->rx_packets_blog_done;
    portCnt.rxDrops =   estats->rx_dropped_no_rxdev +
        estats->rx_dropped_blog_drop +
        estats->rx_dropped_no_skb +
        estats->rx_dropped_no_srcport;

    portCnt.txDrops = estats->tx_dropped_bad_nbuff +
        estats->tx_dropped_no_lowlvl_resource +
        estats->tx_dropped_mux_failed +
        estats->tx_dropped_no_fkb +
        estats->tx_dropped_no_fkb +
        estats->tx_dropped_no_skb +
        estats->tx_dropped_runner_lan_fail +
        estats->tx_dropped_runner_wan_fail;
#endif /* DEBUG_STATS */

    printk(ESTATS_FMT "\n", ESTATS_VAL2(portCnt.rxIn, portCnt.rx2Kernel));
    printk(ESTATS_FMT "\n", ESTATS_VAL2(portCnt.rx2Blog, portCnt.rx2Kernel + portCnt.rx2Blog));
    printk(ESTATS_FMT1 "\n", ESTATS_VAL1(portCnt.rxDrops));
    printk(ESTATS_FMT1 "\n", ESTATS_VAL1((portCnt.rx2Kernel + portCnt.rx2Blog + portCnt.rxDrops)));
    printk(ESTATS_FMT1b "\n", ESTATS_VAL1b(portCnt.rxIn == (portCnt.rx2Kernel + portCnt.rx2Blog + portCnt.rxDrops)));
    printk(ESTATS_FMT "\n", ESTATS_VAL2(portCnt.txIn, portCnt.txOut));
    printk(ESTATS_FMT "\n", ESTATS_VAL2(portCnt.txDrops, portCnt.txOut + portCnt.txDrops));
    printk(ESTATS_FMT1b "\n", ESTATS_VAL1b(portCnt.txIn == (portCnt.txOut + portCnt.txDrops)));
    printk("\n");

    if(cts)
    {
        cts->rxIn += portCnt.rxIn;
        cts->rx2Kernel += portCnt.rx2Kernel;
        cts->rx2Blog += portCnt.rx2Blog;
        cts->rxDrops += portCnt.rxDrops;
        cts->txIn += portCnt.txIn;
        cts->txOut += portCnt.txOut;
        cts->txDrops += portCnt.txDrops;
    }
    return 0;
}

static void display_software_stats(enetx_port_t *port)
{
    // based on imp5\bcmenet.c display_enet_stats()
    counterSum_t total = {};

    if(port->port_class == PORT_CLASS_PORT)
    {
        display_enet_dev_stats(port, NULL);
        return;
    }

    // traverse all ports on switch
    port_traverse_ports(port, display_enet_dev_stats, PORT_CLASS_PORT, &total);

    printk("\n");
    printk(ESTATS_FMT "\n", ESTATS_VAL2(total.rxIn, total.rx2Kernel));
    printk(ESTATS_FMT "\n", ESTATS_VAL2(total.rx2Blog, total.rx2Kernel + total.rx2Blog));
    printk(ESTATS_FMT "\n", ESTATS_VAL2(total.rxDrops, (total.rx2Kernel + total.rx2Blog + total.rxDrops)));
    printk(ESTATS_FMT1b "\n", ESTATS_VAL1b(total.rxIn == (total.rx2Kernel + total.rx2Blog + total.rxDrops)));
    printk(ESTATS_FMT "\n", ESTATS_VAL2(total.txIn, total.txOut));
    printk(ESTATS_FMT "\n", ESTATS_VAL2(total.txDrops, total.txOut + total.txDrops));
    printk(ESTATS_FMT1b "\n", ESTATS_VAL1b(total.txIn == (total.txOut + total.txDrops)));
}

static int _compat_validate_unit_port(int unit, int port)
{
    if (unit < 0 || port < 0 || unit >= BP_MAX_ENET_MACS || port >= COMPAT_MAX_SWITCH_PORTS)
    {
        enet_err("invalid unit %d, port %d values\n", unit, port);
        return -1;
    }

    return 0;
}


static enetx_port_t *_compat_port_object_from_unit_port(int unit, int port)
{
    enetx_port_t *p;

    if (_compat_validate_unit_port(unit, port))
        return NULL;

    if (!(p = COMPAT_PORT(unit, port)))
    {
        enet_err("cannot map unit %d, port %d to port object\n", unit, port);
        return NULL;
    }

    return p;
}

static void mac_dev_stats_to_emac_stats(struct emac_stats *emac_stats, mac_stats_t *mac_stats)
{
#define MAC_STATS_COPY(f) emac_stats->f = mac_stats->f

    MAC_STATS_COPY(rx_byte);
    MAC_STATS_COPY(rx_packet);
    MAC_STATS_COPY(rx_frame_64);
    MAC_STATS_COPY(rx_frame_65_127);
    MAC_STATS_COPY(rx_frame_128_255);
    MAC_STATS_COPY(rx_frame_256_511);
    MAC_STATS_COPY(rx_frame_512_1023);
    MAC_STATS_COPY(rx_frame_1024_1518);
    MAC_STATS_COPY(rx_frame_1519_mtu);
    MAC_STATS_COPY(rx_multicast_packet);
    MAC_STATS_COPY(rx_broadcast_packet);
    MAC_STATS_COPY(rx_unicast_packet);
    MAC_STATS_COPY(rx_alignment_error);
    MAC_STATS_COPY(rx_frame_length_error);
    MAC_STATS_COPY(rx_code_error);
    MAC_STATS_COPY(rx_carrier_sense_error);
    MAC_STATS_COPY(rx_fcs_error);
    MAC_STATS_COPY(rx_undersize_packet);
    MAC_STATS_COPY(rx_oversize_packet);
    MAC_STATS_COPY(rx_fragments);
    MAC_STATS_COPY(rx_jabber);
    MAC_STATS_COPY(rx_overflow);
    MAC_STATS_COPY(rx_control_frame);
    MAC_STATS_COPY(rx_pause_control_frame);
    MAC_STATS_COPY(rx_unknown_opcode);
    MAC_STATS_COPY(tx_byte);
    MAC_STATS_COPY(tx_packet);
    MAC_STATS_COPY(tx_frame_64);
    MAC_STATS_COPY(tx_frame_65_127);
    MAC_STATS_COPY(tx_frame_128_255);
    MAC_STATS_COPY(tx_frame_256_511);
    MAC_STATS_COPY(tx_frame_512_1023);
    MAC_STATS_COPY(tx_frame_1024_1518);
    MAC_STATS_COPY(tx_frame_1519_mtu);
    MAC_STATS_COPY(tx_fcs_error);
    MAC_STATS_COPY(tx_multicast_packet);
    MAC_STATS_COPY(tx_broadcast_packet);
    MAC_STATS_COPY(tx_unicast_packet);
    MAC_STATS_COPY(tx_total_collision);
    MAC_STATS_COPY(tx_jabber_frame);
    MAC_STATS_COPY(tx_oversize_frame);
    MAC_STATS_COPY(tx_undersize_frame);
    MAC_STATS_COPY(tx_fragments_frame);
    MAC_STATS_COPY(tx_error);
    MAC_STATS_COPY(tx_underrun);
    MAC_STATS_COPY(tx_excessive_collision);
    MAC_STATS_COPY(tx_late_collision);
    MAC_STATS_COPY(tx_single_collision);
    MAC_STATS_COPY(tx_multiple_collision);
    MAC_STATS_COPY(tx_pause_control_frame);
    MAC_STATS_COPY(tx_deferral_packet);
    MAC_STATS_COPY(tx_excessive_deferral_packet);
    MAC_STATS_COPY(tx_control_frame);
}

static int tr_port_by_port_id(enetx_port_t *port, void *_ctx)
{
    if (port->p.port_id == *(uint32_t *)_ctx)
    {
        *(enetx_port_t **)_ctx = port;
        return 1;
    }

    return 0;
}

int port_by_port_id(enetx_port_t *sw, int port_id, enetx_port_t **match)
{
    void *in_out = (void *)(unsigned long)port_id;

    if (port_traverse_ports(sw, tr_port_by_port_id, PORT_CLASS_PORT, &in_out) <= 0)
        return -1;

    *match = (enetx_port_t *)in_out;

    return 0;
}

enetx_port_t *port_by_unit_port(int unit_port)
{

#if defined(DSL_RUNNER_DEVICE)
    return _compat_port_object_from_unit_port(LOGICAL_PORT_TO_UNIT_NUMBER(unit_port), LOGICAL_PORT_TO_PHYSICAL_PORT(unit_port));
#else
    return _compat_port_object_from_unit_port(0, unit_port);
#endif
}

#if defined(DSL_RUNNER_DEVICE)
// ----------- SIOCETHSWCTLOPS ETHSWKERNELPOLL functions ---
extern int enet_opened;

int ioctl_ethsw_kernel_poll(struct ethswctl_data *e)
{
    // based on shared\bcmenet.c:enet_ioctl_kernel_poll()
    static int kernel_polled = 0;

    if (kernel_polled == 0)
    {
        // MAC tx/rx needs to be disabled until after 1st kernel poll
        // if enet_open() occurred first, MAC tx/rx enable need to be in 1st kernel poll instead.
        if (enet_opened)
            enet_err("error: enet_open() occurred before 1st SWMDK polling!!!\n");

        // TODO_DSL:  initPorts if necessary, do polling
    }
    kernel_polled++;
    
    e->mdk_kernel_poll.link_change = ETHSW_LINK_FORCE_CHECK;
    return BCM_E_NONE;
}

// ----------- SIOCGMIIPHY,  SIOCGMIIREG, SIOCSMIIREG functions ---
static int _handle_siocgmiiphy(enetx_port_t *self, struct mii_ioctl_data *mii)
{
    ETHERNET_MAC_INFO *EnetInfo = (ETHERNET_MAC_INFO *)BpGetEthernetMacInfoArrayPtr();
    ETHERNET_MAC_INFO *info = &EnetInfo[PORT_ON_ROOT_SW(self)?0:1];
    int phy_id;

    if (mii->val_in != 0xffff)
        phy_id = info->sw.crossbar[BP_PHY_PORT_TO_CROSSBAR_PORT(mii->val_in)].phy_id;
    else

        phy_id = info->sw.phy_id[self->p.mac->mac_id];

    mii->phy_id = (uint16_t)phy_id;
    /* Let us also return phy flags needed for accessing the phy */
    mii->val_out = phy_id & CONNECTED_TO_EXTERN_SW? ETHCTL_FLAG_ACCESS_EXTSW_PHY: 0;
    mii->val_out |= IsExtPhyId(phy_id)? ETHCTL_FLAG_ACCESS_EXT_PHY: 0;
    mii->val_out |= IsSerdes(phy_id)? ETHCTL_FLAG_ACCESS_SERDES: 0;

    return 0;
}

int ioctl_handle_mii(struct net_device *dev, struct ifreq *ifr, int cmd)
{
    // based on bcmenet.c:bcm63xx_enet_ioctl()
    struct mii_ioctl_data *mii = if_mii(ifr);
    enetx_port_t *port = NETDEV_PRIV(dev)->port;
    phy_dev_t *phy = port->p.phy;
    int rc;
    u16 v16;

    if (port->port_class != PORT_CLASS_PORT || !port->p.phy)
        return -EINVAL;

    if (cmd == SIOCGMIIPHY) /* Get address of MII PHY in use by dev */
        return _handle_siocgmiiphy(port, mii);

    if ((phy = phy_dev_get(PHY_TYPE_UNKNOWN, mii->phy_id & BCM_PHY_ID_M)) == NULL)
        return -EINVAL;

    if (phy->sw_port != port)
        return -EINVAL;

    switch (cmd)
    {
        case SIOCGMIIREG: /* Read MII PHY register. */
            rc = phy_bus_read(phy, mii->reg_num & 0x1f, &v16);
            mii->val_out = v16;
            return rc ? -EINVAL : 0;
        case SIOCSMIIREG: /* Write MII PHY register. */
            rc = phy_bus_write(phy, mii->reg_num & 0x1f, mii->val_in);
            return rc? -EINVAL : 0;
    }

    return -EINVAL;
}

#include "phy_bp_parsing.h"
void add_unspecified_ports(enetx_port_t *sw, uint32_t port_map, uint32_t imp_map)
{
    enetx_port_t *port;
    int ndx;
    int unit = IS_ROOT_SW(sw)? 0:1;

    for (ndx = 0; ndx <= BP_MAX_SWITCH_PORTS; ndx++)
    {
        port_info_t port_info = {};
        ETHERNET_MAC_INFO emac_info = {};

        if (unit_port_array[unit][ndx]) continue;       // port object already defined
        if (!(port_map & (1<<ndx))) continue;           // skipping none defined ports

        port_info.port = ndx;
        port_info.is_undef = 1;
        if (imp_map & (1<<ndx)) port_info.is_management = 1;
        
        if (!(port = port_create(&port_info, sw)))
        {
            enet_err("Failed to create unit %d port %d\n", unit, ndx);
            return;
        }

        emac_info.ucPhyType = unit ? BP_ENET_EXTERNAL_SWITCH /*SF2 */ : BP_ENET_NO_PHY /* runner */;
        port->p.mac = bp_parse_mac_dev(&emac_info, ndx);
        mac_dev_init(port->p.mac);
        
        if (imp_map & (1<<ndx))
        {
            port->p.port_cap = PORT_CAP_MGMT;
            mac_dev_enable(port->p.mac);        // enable secondary ports connecting to IMP ports
        }

        unit_port_array[unit][ndx] = port;
        enet_dbgv("%s add port %pK macId=%x (%s) %pK\n", sw->obj_name, port, ndx, port->p.mac->mac_drv->name, port->p.mac);
    }
}
#endif // DSL_RUNNER_DEVICE

#define ethctl_field_copy_to_user(f)      \
    copy_to_user((void*)((char*)rq->ifr_data +((char*)&(ethctl->f) - (char*)&rq_data)), &(ethctl->f), sizeof(ethctl->f))

extern void xgSerdesRegRead(uint32_t phy_reg, uint32_t *val);
extern void xgSerdesRegWrite(uint32_t phy_reg, uint32_t val);
extern void xgPcsRegRead(uint32_t phy_reg, uint32_t *val);
extern void xgPcsRegWrite(uint32_t phy_reg, uint32_t val);

int enet_ioctl_compat_ethctl(struct net_device *dev, struct ifreq *rq, int cmd)
{
    struct ethctl_data rq_data;
    struct ethctl_data *ethctl = &rq_data;
    enetx_port_t *port = NULL;

    if (copy_from_user(ethctl, rq->ifr_data, sizeof(struct ethctl_data)))
        return -EFAULT;

#if defined(enet_dbgv_ioctl_enabled)
    /* print 1 line showing type of ioctl */
    switch(ethctl->op)
    {
    /* exclude following ioctl */
    case ETHGETMIIREG:
        break;
    default:
        enet_dbgv(" dev=%s, cmd=%x(SIOCETHCTLOPS), ops=%d\n", NETDEV_PRIV(dev)->port->obj_name, cmd, ethctl->op);
    }
#endif

    switch(ethctl->op)
    {
#if defined(CONFIG_BCM96838) || defined(CONFIG_BCM96848)
    case ETHINITWAN:
        return _handle_init_wan(rq, ethctl);
#endif
#ifdef EPON
    case ETHCREATEEPONVPORT:
        return _handle_epon(ethctl->op);
    case ETHCREATEEPONAEVPORT:
        return _handle_epon_ae(rq, ethctl);
#endif
    case ETHGETSOFTWARESTATS:
        port = ((enetx_netdev *)netdev_priv(dev))->port;
        display_software_stats(port);
        return 0;
    /* MII read/write functions for EGPHY compatible PHYs, will not work correctly on other PHY types */
    case ETHGETMIIREG: /* Read MII PHY register */
    case ETHSETMIIREG: /* Write MII PHY register */
#if defined(DSL_RUNNER_DEVICE)
        {
            int ret = 0;
            phy_dev_t *phy_dev;

            phy_dev = phy_dev_get(PHY_TYPE_UNKNOWN, ethctl->phy_addr);
            if (ethctl->flags == ETHCTL_FLAG_ACCESS_I2C_PHY)
            {
                phy_dev = phy_dev_get_by_i2c(ethctl->phy_addr);
                if (!phy_dev)
                    return -1;
            }

            if (!(ethctl->flags & (ETHCTL_FLAG_ACCESS_10GSERDES|ETHCTL_FLAG_ACCESS_10GPCS)) && (phy_dev == NULL || !phy_dev->phy_drv))
            {
                enet_err("No PHY at address %d has been found.\n", ethctl->phy_addr);
                return -EFAULT;
            }

            down(&root_sw->s.conf_sem);

            if (ethctl->flags == ETHCTL_FLAG_ACCESS_SERDES_POWER_MODE)
            {
                if (ethctl->op == ETHSETMIIREG)
                    ret = phy_dev->phy_drv->apd_set ?
                        phy_dev->phy_drv->apd_set(phy_dev, ethctl->phy_reg) : 0;
                else
                {
                    ret = phy_dev->phy_drv->apd_get ?
                        phy_dev->phy_drv->apd_get(phy_dev, &ethctl->val) : 0;
                }
            }
#if defined(PHY_SERDES_10G_CAPABLE)
            else if (ethctl->flags & ETHCTL_FLAG_ACCESS_10GSERDES)
            {
                if (ethctl->op == ETHGETMIIREG)
                {
                    ret = readXgaeSerdesReg(ethctl->phy_reg, &ethctl->val);
                }
                else
                {
                    ret = writeXgaeSerdesReg(ethctl->phy_reg, ethctl->val);
                }
            }
            else if (ethctl->flags & ETHCTL_FLAG_ACCESS_10GPCS)
            {
                if (ethctl->op == ETHGETMIIREG)
                {
                    ret = readXgaePcsReg(ethctl->phy_reg, &ethctl->val);
                }
                else
                {
                    ret = writeXgaePcsReg(ethctl->phy_reg, ethctl->val);
                }
            }
#endif
            else
            {
                if (ethctl->op == ETHGETMIIREG)
                    ret = ethsw_phy_exp_read32(phy_dev, ethctl->phy_reg, &ethctl->val);
                else
                    ret = ethsw_phy_exp_write(phy_dev, ethctl->phy_reg, ethctl->val);
            }

            up(&root_sw->s.conf_sem);

            if (ret) return ret;

            if (copy_to_user(rq->ifr_data, ethctl, sizeof(struct ethctl_data)))
                return -EFAULT;

            return 0;
        }
#elif defined(CONFIG_BCM_PON)
        {
            uint16_t val = ethctl->val;
            bus_drv_t *bus_drv = NULL;
            
            if (ethctl->flags == ETHCTL_FLAG_ACCESS_INT_PHY)
            {
#ifdef CONFIG_BCM96838
                bus_drv = bus_drv_get(BUS_TYPE_6838_EGPHY);
#endif
#ifdef CONFIG_BCM96848
                bus_drv = bus_drv_get(BUS_TYPE_6848_INT);
#endif
#ifdef CONFIG_BCM96858
                bus_drv = bus_drv_get(BUS_TYPE_6858_LPORT);
#endif
#ifdef CONFIG_BCM96836
                bus_drv = bus_drv_get(BUS_TYPE_6836_INT);
#endif

#ifdef CONFIG_BCM96856
                bus_drv = bus_drv_get(BUS_TYPE_6836_INT);
#endif
/*#ifdef CONFIG_BCM96846 //TBD:Dima's commit
                    bus_drv = bus_drv_get(BUS_TYPE_6846_INT);
#endif*/
            }
            else if (ethctl->flags == ETHCTL_FLAG_ACCESS_EXT_PHY)
            {
#ifdef CONFIG_BCM96838
                bus_drv = bus_drv_get(BUS_TYPE_6838_EXT);
#endif
#ifdef CONFIG_BCM96848
                bus_drv = bus_drv_get(BUS_TYPE_6848_EXT);
#endif
#ifdef CONFIG_BCM96858
                bus_drv = bus_drv_get(BUS_TYPE_6858_LPORT);
#endif
            }
            
            if (!bus_drv)
            {
                enet_err("cannot resolve phy bus driver for phy_addr %d, flags %d\n", ethctl->phy_addr, ethctl->flags);
                return -EFAULT;
            }

            if (ethctl->op == ETHGETMIIREG)
            {
                if (bus_read(bus_drv, ethctl->phy_addr, ethctl->phy_reg, &val))
                    return -EFAULT;

                ethctl->val = val;
                enet_dbg("get phy_id: %d; reg_num = %d; val = 0x%x \n", ethctl->phy_addr, ethctl->phy_reg, val);
                
                if (copy_to_user(rq->ifr_data, ethctl, sizeof(struct ethctl_data)))
                    return -EFAULT;
            }
            else
            {
                if (bus_write(bus_drv, ethctl->phy_addr, ethctl->phy_reg, ethctl->val))
                    return -EFAULT;
                
                enet_dbg("set phy_id: %d; reg_num = %d; val = 0x%x \n", ethctl->phy_addr, ethctl->phy_reg, ethctl->val);
            }

            return 0;
        }
#endif /* defined(CONFIG_BCM96838) || defined(CONFIG_BCM96848) || defined(CONFIG_BCM96858) */
    case ETHSETSPOWERUP:
    case ETHSETSPOWERDOWN:
        {
            port = ((enetx_netdev *)netdev_priv(dev))->port;

            if (!port->p.phy)
            {
                enet_err("port - %s has no PHY conncted \n", port->name);
                return -EFAULT;
            }

            return phy_dev_power_set(port->p.phy, ethctl->op == ETHSETSPOWERUP) ? -EFAULT : 0;
        }
    default:
        return -EOPNOTSUPP;
    }

    return -EOPNOTSUPP;
}

static int tr_net_dev_hw_switching_set(enetx_port_t *port, void *_ctx)
{
    struct ethswctl_data *ethswctl = (struct ethswctl_data *)_ctx;

    if (port->n.port_netdev_role != PORT_NETDEV_ROLE_LAN || !port->dev)
        return 0;

    if (ethswctl->type == TYPE_ENABLE)
        port->dev->priv_flags |= IFF_HW_SWITCH;
    else
        port->dev->priv_flags &= IFF_HW_SWITCH;

    return 0;
}

static int _enet_ioctl_hw_switching_set(struct ethswctl_data *ethswctl)
{
#if defined(DSL_RUNNER_DEVICE)
    enetx_port_t *sw = sf2_sw;
#else
    enetx_port_t *sw = root_sw;
#endif
    int rc = 0;

    switch (ethswctl->type)
    {
    case TYPE_ENABLE:
    case TYPE_DISABLE:
        rc = sw->s.ops->hw_sw_state_set(sw, ethswctl->type == TYPE_ENABLE);
        if (!rc)
            port_traverse_ports(sw, tr_net_dev_hw_switching_set, PORT_CLASS_PORT, ethswctl);
        break;
    case TYPE_GET:
        ethswctl->status = sw->s.ops->hw_sw_state_get(sw);
        break;
    default:
        rc = -1;
    }
    if (rc)
    {
        enet_err("Flooding configuration failed, err %d\n", rc);
        return -EFAULT;
    }

    return 0;
}

static void _enet_ioctl_hw_switch_flag_set(struct ethswctl_data *ethswctl)
{
#if !defined(DSL_RUNNER_DEVICE) && defined(CONFIG_BCM_PON_XRDP)
    enetx_port_t *sw = root_sw;

    port_traverse_ports(sw, tr_net_dev_hw_switching_set, PORT_CLASS_PORT, ethswctl);
    
    return;
#endif
}

#define ethswctl_field_copy_to_user(f)      \
    copy_to_user((void*)((char*)rq->ifr_data +((char*)&(ethswctl->f) - (char*)&rq_data)), &(ethswctl->f), sizeof(ethswctl->f))

#define ethswctl_field_len_copy_to_user(f,l)      \
    copy_to_user((void*)((char*)rq->ifr_data +((char*)&(ethswctl->f) - (char*)&rq_data)), &(ethswctl->f), ethswctl->l)

int enet_ioctl_compat_ethswctl(struct net_device *dev, struct ifreq *rq, int cmd)
{
    struct ethswctl_data rq_data;
    struct ethswctl_data *ethswctl = &rq_data;
    phy_dev_t *phy_dev;
    enetx_port_t *port = NULL;
#if defined(DSL_RUNNER_DEVICE)
    int ret;
#endif

    if (copy_from_user(ethswctl, rq->ifr_data, sizeof(struct ethswctl_data)))
        return -EFAULT;

#if defined(enet_dbgv_ioctl_enabled)
    /* print 1 line showing type of ioctl */
    switch(ethswctl->op)
    {
    /* exclude following ioctl */
    case ETHSWKERNELPOLL:       /* called once a second */
    case ETHSWREGACCESS:        /* called multiple times every seconde */
    case ETHSWUNITPORT:         /* called when link change */
    case ETHSWRDPAPORTGET:      /* called when link is up */
    case ETHSWCOSSCHED:         /* called when link is up */
    case ETHSWPORTTXRATE:       /* called when link is up */
    case ETHSWPHYMODE:
    case ETHSWINFO:
    case ETHSWPHYCFG:
/*
    case ETHSWLINKSTATUS:
*/
        break;
    default:
        enet_dbgv(" dev=%s, cmd=%x(SIOCETHSWCTLOPS), ops=%d u/p=%d/%d\n", NETDEV_PRIV(dev)->port->obj_name, cmd, ethswctl->op, ethswctl->unit, ethswctl->port);
    }
#endif

    switch(ethswctl->op)
    {
      case ETHSWPORTTRAFFICCTRL:
        {
            if (!(port = _compat_port_object_from_unit_port(ethswctl->unit, ethswctl->port)))
               return -EFAULT;

            if (ethswctl->type == TYPE_GET)
            {
                enet_err("%s ETHSWPORTTRAFFICCTRL: Unsupported request\n", __FUNCTION__);
                return -EFAULT;
            }

            if (ethswctl->type == TYPE_SET)
            {
                if (ethswctl->val) 
                    port_generic_stop(port);
                else 
                    port_generic_open(port);
            }
            
            return 0;
        }

    case ETHSWDUMPMIB:
        {
            if (!(port = _compat_port_object_from_unit_port(ethswctl->unit, ethswctl->port)))
               return -EFAULT;
            return port_mib_dump(port, ethswctl->type);
        }

#if defined(DSL_RUNNER_DEVICE)
    case ETHSWDUMPPAGE:
        /* only support ext sw and page 0 */
        if ((ethswctl->unit == 1) && (ethswctl->page == 0))
            ioctl_extsw_dump_page0();
        return 0;
    case ETHSWCONTROL:
        if (ethswctl->unit == 0) return -(EOPNOTSUPP);

        ret = ioctl_extsw_control(ethswctl);
        if (!ret && copy_to_user(rq->ifr_data , ethswctl, sizeof(struct ethswctl_data)))
            return -EFAULT;

        return ret;
    case ETHSWPRIOCONTROL:
        if (ethswctl->unit == 0) return -(EOPNOTSUPP);

        ret = ioctl_extsw_prio_control(ethswctl);
        if (!ret && copy_to_user(rq->ifr_data , ethswctl, sizeof(struct ethswctl_data)))
            return -EFAULT;

        return ret;
    case ETHSWQUEMAP:
        ret = ioctl_extsw_que_map(ethswctl);
        if (!ret && copy_to_user(rq->ifr_data , ethswctl, sizeof(struct ethswctl_data)))
            return -EFAULT;

        return ret;
    case ETHSWQUEMON:
        if (ethswctl->unit == 0) return -(EOPNOTSUPP);

        ret = ioctl_extsw_que_mon(ethswctl);
        if (!ret && copy_to_user(rq->ifr_data , ethswctl, sizeof(struct ethswctl_data)))
            return -EFAULT;

        return ret;
    case ETHSWACBCONTROL:
        BCM_IOC_PTR_ZERO_EXT(ethswctl->vptr);
        return ioctl_extsw_cfg_acb(ethswctl);
    case ETHSWCFP:
        ret = ioctl_extsw_cfp(ethswctl);
        if (!ret && ethswctl_field_copy_to_user(cfpArgs))
            return -EFAULT;
        return ret;
    case ETHSWPBVLAN:
        if (ethswctl->unit == 0) return -(EOPNOTSUPP);

        ret = ioctl_extsw_pbvlan(ethswctl);
        if (!ret && (ethswctl->type == TYPE_GET))
            if (ethswctl_field_copy_to_user(fwd_map))
                return -EFAULT;

        return ret;
    case ETHSWMIRROR:
        if (ethswctl->unit == 0) {
            enet_err("runner mirror port not supported yet.\n");    //TODO_DSL? support runner port mirroring?
            return -(EOPNOTSUPP);
        }
        
        ret = ioctl_extsw_port_mirror_ops(ethswctl);
        if (!ret && (ethswctl->type == TYPE_GET))
            if (ethswctl_field_copy_to_user(port_mirror_cfg))
                return -EFAULT;
        return ret;
    case ETHSWPORTTRUNK:
        if (ethswctl->unit == 0) return -(EOPNOTSUPP);
        
        ret = ioctl_extsw_port_trunk_ops(ethswctl);
        if (!ret && (ethswctl->type == TYPE_GET))
            if (ethswctl_field_copy_to_user(port_trunk_cfg))
                return -EFAULT;
        return ret;
    case ETHSWKERNELPOLL:
        ret = ioctl_ethsw_kernel_poll(ethswctl);

        if (ethswctl_field_copy_to_user(mdk_kernel_poll.link_change))
            return -EFAULT;

        return ret;
    case ETHSWREGACCESS:
        if ((ethswctl->offset & IS_PHY_ADDR_FLAG) &&
            !(port = _compat_port_object_from_unit_port(1,ethswctl->offset & PORT_ID_M)))
            return BCM_E_ERROR;
        ret = ioctl_extsw_regaccess(ethswctl, port);
        if (!ret && ethswctl->length)
            if (ethswctl_field_len_copy_to_user(data[0], length))
                return -EFAULT;

        return ret;
    case ETHSWINFO:
        if ((ret = ioctl_extsw_info(ethswctl)) >= 0) {
            if (copy_to_user(rq->ifr_data , ethswctl, sizeof(struct ethswctl_data)))
                return -EFAULT;
        } else
            enet_dbg("ETHSWINFO: error ret=%d\n", ret);

        return ret;
    case ETHSWRDPAPORTGET:
        port = _compat_port_object_from_unit_port(ethswctl->unit, ethswctl->port);
        ethswctl->val = (port)? port->p.port_id : rdpa_if_none;
        
        //enet_dbgv("ETHSWRDPAPORTGET: u=%d p=%d > rdpa_if=%d\n", ethswctl->unit, ethswctl->port, ethswctl->val);
        if (ethswctl_field_copy_to_user(val))
            return -EFAULT;

        return 0;
    case ETHSWCOSSCHED:
        /* only valid for ports on external SF2 switch */
        if (ethswctl->unit == 0) return -(EOPNOTSUPP);

        if (!(port = _compat_port_object_from_unit_port(ethswctl->unit, ethswctl->port)))
            return -EFAULT;

        ret = ioctl_extsw_cosq_sched(port, ethswctl);
        if (ret >= 0)
        {
            if (ethswctl->type == TYPE_GET)
            {
                ethswctl->ret_val = ethswctl->val;
            }
            if (copy_to_user(rq->ifr_data , ethswctl, sizeof(struct ethswctl_data)))
                return -EFAULT;
            return 0;
        }

        enet_dbg("ETHSWCOSSCHED: error ret=%d\n", ret);
        return ret;
    case ETHSWCOSPORTMAP:
        if (ethswctl->unit == 0) return -(EOPNOTSUPP);
 
        ret = ioctl_extsw_cosq_port_mapping(ethswctl);
        if (ret < 0) return ret;
        if (ethswctl->type == TYPE_GET) {
            ethswctl->queue = ret;  /* queue returned from function. Return value to user */
            if (ethswctl_field_copy_to_user(queue))
                return -EFAULT;
            ret = 0;
        }

        return ret;
        
    case ETHSWARLACCESS:
        ret = ioctl_extsw_arl_access(ethswctl);
        if (!ret && copy_to_user(rq->ifr_data , ethswctl, sizeof(struct ethswctl_data)))
            return -EFAULT;
        return ret;
    case ETHSWCOSPRIORITYMETHOD:
        if (ethswctl->unit == 0) {
            enet_err("runner COS priority method config not supported yet.\n");    //TODO_DSL?
            return -(EOPNOTSUPP);
        }

        ret = ioctl_extsw_cos_priority_method_cfg(ethswctl);
        if (!ret && (ethswctl->type == TYPE_GET))
            if (ethswctl_field_copy_to_user(ret_val))
                return -EFAULT;
        return ret;
    case ETHSWJUMBO:
        if (ethswctl->unit == 0) return -(EOPNOTSUPP);

        ret = ioctl_extsw_port_jumbo_control(ethswctl);
        if (!ret && ethswctl_field_copy_to_user(ret_val))
            return -EFAULT;
        return ret;

    case ETHSWCOSPCPPRIOMAP:
        if (ethswctl->unit == 0) return 0;

        ret = ioctl_extsw_pcp_to_priority_mapping(ethswctl);
        if (!ret && (ethswctl->type == TYPE_GET))
            if (ethswctl_field_copy_to_user(priority))
                return -EFAULT;
        return ret;
    case ETHSWCOSPIDPRIOMAP:
        if (ethswctl->unit == 0) {
            enet_err("runner COS PID priority mapping not supported yet.\n");    //TODO_DSL?
            return -(EOPNOTSUPP);
        }

        ret = ioctl_extsw_pid_to_priority_mapping(ethswctl);
        if (!ret && (ethswctl->type == TYPE_GET))
            if (ethswctl_field_copy_to_user(priority))
                return -EFAULT;
        return ret;
    case ETHSWCOSDSCPPRIOMAP:
        if (ethswctl->unit == 0) return 0;

        ret = ioctl_extsw_dscp_to_priority_mapping(ethswctl);
        if (!ret && (ethswctl->type == TYPE_GET))
            if (ethswctl_field_copy_to_user(priority))
                return -EFAULT;
        return ret;
        
    case ETHSWPORTSHAPERCFG:
        if (ethswctl->unit == 0) return -(EOPNOTSUPP);

        return ioctl_extsw_port_shaper_config(ethswctl);
    case ETHSWDOSCTRL:
        if (ethswctl->unit == 0) return -(EOPNOTSUPP);

        ret = ioctl_extsw_dos_ctrl(ethswctl);
        if (!ret && (ethswctl->type == TYPE_GET))
            if (ethswctl_field_copy_to_user(dosCtrl))
                return -EFAULT;
        return ret;
    
    case ETHSWHWSTP:
        if (ethswctl->type == TYPE_GET)
        {
            ethswctl->status = root_sw->s.stpDisabledPortMap;   // TODO_DSL? bridge_stp_handler ???
            if (ethswctl_field_copy_to_user(status))
                return -EFAULT;
            return 0;
        }
        else if ((ethswctl->type == TYPE_ENABLE) || (ethswctl->type == TYPE_DISABLE))
        {
            if (!(port = _compat_port_object_from_unit_port(ethswctl->unit, ethswctl->port)))
                return -EFAULT;
            if (port->p.ops->stp_set)
            {
                port->p.ops->stp_set(port, (ethswctl->type == TYPE_ENABLE)?STP_MODE_ENABLE:STP_MODE_DISABLE, STP_STATE_UNCHANGED);
                if ((ethswctl->type == TYPE_ENABLE) && (port->dev->flags & IFF_UP))
                {
                    dev_change_flags(port->dev, (port->dev->flags & ~IFF_UP));
                    dev_change_flags(port->dev, (port->dev->flags | IFF_UP));
                }
            }
            return 0;
        }
        else
            return -EFAULT;

    case ETHSWPORTSTORMCTRL:
        if (ethswctl->unit == 0) return -(EOPNOTSUPP);

        ret = ioctl_extsw_port_storm_ctrl(ethswctl);
        if (!ret && copy_to_user(rq->ifr_data , ethswctl, sizeof(struct ethswctl_data)))
            return -EFAULT;
        return ret;

    case ETHSWMULTIPORT:
        if (ethswctl->unit !=0) return BCM_E_NONE;
        if (ethswctl->type == TYPE_SET) {
            ioctl_extsw_set_multiport_address(ethswctl->mac);
        }
        return BCM_E_NONE;

#if defined(CONFIG_BCM_ETH_DEEP_GREEN_MODE)
    case ETHSWDEEPGREENMODE:
        if ( ethswctl->type == TYPE_GET )
        {
            ethswctl->val = ioctl_pwrmngt_get_deepgreenmode(ethswctl->val);
            if (ethswctl_field_copy_to_user(val))
                return -EFAULT;
        }
        else
            ioctl_pwrmngt_set_deepgreenmode(ethswctl->val);
        return 0;
#endif
#else /* !defined(DSL_RUNNER_DEVICE) */        
        case ETHSWPORTLOOPBACK:
            {
                mac_loopback_t loopback_op;
                int loopback_type = 0;
                int val = 0;
                
                enet_dbg("ethswctl ETHSWPORTLOOPBACK : ioctl\n");
                   
                if (ethswctl->type == TYPE_SET)
                {             
                    if (!(port = _compat_port_object_from_unit_port(ethswctl->unit, ethswctl->port)))
                    {
                        enet_err("failed to get port_obj from unit_port %d\n", ethswctl->port);
                        return -EFAULT;
                    }
                    
                    if (!port->p.mac)
                    {
                        enet_err("failed to get mac_dev\n");
                        return -EFAULT;
                    }
                    
                    val = ethswctl->val;

                    loopback_op = val & 0x000000FF;
                    loopback_type = (val & 0x0000FF00) >> 8;

                    if (loopback_type == 0)
                    {
                        mac_dev_loopback_set(port->p.mac, MAC_LOOPBACK_NONE);                               
                    }
                    else
                    {
                        mac_dev_loopback_set(port->p.mac, loopback_op); 
                    }
                }
            
                return 0;
            }
           
#endif /* !defined(DSL_RUNNER_DEVICE) */
#ifdef RUNNER
    case ETHSWPORTRXRATE:
#if defined(DSL_RUNNER_DEVICE)
        if (ethswctl->unit == 0) return -(EOPNOTSUPP);

        if (ethswctl->type == TYPE_GET)
        { 
            ret = ioctl_extsw_port_irc_get(ethswctl);
            if (!ret && (ethswctl_field_copy_to_user(limit) || ethswctl_field_copy_to_user(burst_size)))
               return -EFAULT;
        }
        else
            ret = ioctl_extsw_port_irc_set(ethswctl);
        return ret;
#else /* !defined(DSL_RUNNER_DEVICE) */
        {
            bdmf_object_handle port_obj;
            rdpa_port_flow_ctrl_t flowctl_cfg;

            enet_dbg("ethswctl ETHSWPORTRXRATE : ioctl\n");

            /* it's about LAN ports */
            port = _compat_port_object_from_unit_port(ethswctl->unit, ethswctl->port);

            if (!(port_obj = _port_rdpa_object_by_port(port)))
            {
                enet_err("failed to get port_obj for %s\n", port->obj_name);
                return -EFAULT;
            }

            if (rdpa_port_flow_control_get(port_obj , &flowctl_cfg))
            {
                enet_err("failed to get port_obj for %s\n", port->obj_name);
                return -EFAULT;
            }
                
            if (ethswctl->type == TYPE_GET) 
            {
                enet_dbg("ethswctl->type : GET\n");

                ethswctl->limit = 0;
                ethswctl->burst_size = 0;

#if defined(CONFIG_BCM_PON_XRDP)
                if (flowctl_cfg.policer)
                {
                    rdpa_tm_policer_cfg_t cfg;

                    rdpa_policer_cfg_get(flowctl_cfg.policer, &cfg);
                    ethswctl->limit = cfg.commited_rate;
                    ethswctl->burst_size = cfg.committed_burst_size;
                }
#else
                ethswctl->limit = flowctl_cfg.rate/1000;           /* bps -> kbps */
                ethswctl->burst_size = (flowctl_cfg.mbs*8)/1000;   /* bytes -> kbits */
#endif

                if (ethswctl_field_copy_to_user(limit) || ethswctl_field_copy_to_user(burst_size))
                   return -EFAULT;
            }
            else if (ethswctl->type == TYPE_SET)
            { 
                enet_dbg("ethswctl->type : SET  limit=%u burst_size=%u \n", ethswctl->limit, ethswctl->burst_size);

#ifdef CONFIG_BCM96846
#define PEAK_RATE 1000000000UL
#else
#define PEAK_RATE 10000000000UL
#endif

#if defined(CONFIG_BCM_PON_XRDP)
                {
                    rdpa_tm_policer_cfg_t cfg = {};

                    if (flowctl_cfg.policer)
                        rdpa_policer_cfg_get(flowctl_cfg.policer, &cfg);

                    if (ethswctl->limit)
                    {
                        cfg.type = rdpa_tm_policer_tr_dual_token_bucket;
                        cfg.commited_rate = ethswctl->limit * 1000;

                        if (ethswctl->burst_size)
                            cfg.committed_burst_size = ethswctl->burst_size * 1000 / 8;
                        else
                            cfg.committed_burst_size = (18 * cfg.commited_rate / 1000000000 + 14) * (cfg.commited_rate * 999) / (8 * 10000000);

                        cfg.peak_rate = PEAK_RATE;
                        cfg.peak_burst_size = 15 * (PEAK_RATE * 999) / (8 * 1000000);
                    }

                    if (flowctl_cfg.policer)
                    {
                        rdpa_policer_cfg_set(flowctl_cfg.policer, &cfg); 
                    }
                    else
                    {
                        BDMF_MATTR(policer_attr, rdpa_policer_drv());
                        bdmf_object_handle policer_obj = NULL;

                        rdpa_policer_dir_set(policer_attr, rdpa_dir_us);
                        rdpa_policer_cfg_set(policer_attr, &cfg); 

                        if (!bdmf_new_and_set(rdpa_policer_drv(), NULL, policer_attr, &policer_obj))
                        {
                            flowctl_cfg.policer = policer_obj;
                            flowctl_cfg.traffic_types = RDPA_FLOW_CONTROL_MASK_ALL_TRAFFIC;
                        }
                    }

                    if (flowctl_cfg.policer && !ethswctl->limit)
                    {
                        bdmf_destroy(flowctl_cfg.policer);
                        flowctl_cfg.policer = NULL;
                        flowctl_cfg.traffic_types = 0;
                    }
                }
#else
                if(ethswctl->limit != 0)
                   memcpy(flowctl_cfg.src_address.b, port->dev->dev_addr, ETH_ALEN);
                else
                   memset(flowctl_cfg.src_address.b, 0, ETH_ALEN);

                flowctl_cfg.rate = ethswctl->limit*1000;            /* kbps -> bps per second */
                flowctl_cfg.mbs = (ethswctl->burst_size*1000)/8;    /* kbits->bits->bytes */
                if (flowctl_cfg.mbs < 20000)
                   flowctl_cfg.mbs = 20000;                         /* mbs/2 > jumbo packet*/
                flowctl_cfg.threshold = flowctl_cfg.mbs/2;          /* kbits->bits->bytes, a half of mbs */
#endif

                if (rdpa_port_flow_control_set(port_obj, &flowctl_cfg))
                {
                   enet_err("failed to set rdpa port flow control data %s\n", port->obj_name);
                   return -EFAULT;
                }
            }

            return 0;
        }
#endif /* !defined(DSL_RUNNER_DEVICE) */
    case ETHSWPORTTXRATE:
#if defined(DSL_RUNNER_DEVICE)
        if (ethswctl->unit == 0) return -(EOPNOTSUPP);

        BCM_IOC_PTR_ZERO_EXT(ethswctl->vptr);
        ret = ioctl_extsw_port_erc_config(ethswctl);
        /* copy_to_user() done by above function */
        return ret;
#else /* !defined(DSL_RUNNER_DEVICE) */
        {
            bdmf_object_handle port_obj = NULL;
            rdpa_port_tm_cfg_t  port_tm_cfg;
            rdpa_tm_rl_cfg_t  tm_rl_cfg = {0};

            if (ethswctl->type == TYPE_SET)
            {
                port = _compat_port_object_from_unit_port(ethswctl->unit, ethswctl->port);

                if (!(port_obj = _port_rdpa_object_by_port(port)))
                {
                    enet_err("failed to get port_obj for %s\n", port->obj_name);
                    return -EFAULT;
                }

                if (rdpa_port_tm_cfg_get(port_obj, &port_tm_cfg))
                {
                    enet_err("rdpa_port_tm_cfg_get failed\n");
                    return -EFAULT;
                }

                if (!port_tm_cfg.sched)
                {
                    enet_err("can't configure rate limiter\n");
                    return -EFAULT;
                }

                tm_rl_cfg.af_rate = ethswctl->limit*1000;
                if (rdpa_egress_tm_rl_set(port_tm_cfg.sched, &tm_rl_cfg) != 0)
                {
                    enet_err("rdpa_egress_tm_rl_set failed\n");
                    return -EFAULT;
                }
            } 
            else 
            {
                enet_err("%s ETHSWPORTTXRATE: Unsupported request\n", __FUNCTION__);
                return -EFAULT;
            }

            return 0;
        }
#endif /* !defined(DSL_RUNNER_DEVICE) */
#endif /* RUNNER */
    case ETHSWEMACCLEAR:
        {
            if (!(port = _compat_port_object_from_unit_port(ethswctl->unit, ethswctl->port)))
                return -EFAULT;

            if (!port->p.mac)
                return -EFAULT;

            if (mac_dev_stats_clear(port->p.mac))
                return -EFAULT;

            return 0;
        }
    case ETHSWEMACGET:
        {
            mac_stats_t mac_stats;
            
            if (!(port = _compat_port_object_from_unit_port(ethswctl->unit, ethswctl->port)))
                return -EFAULT;

            if (!port->p.mac)
                return -EFAULT;

            if (mac_dev_stats_get(port->p.mac, &mac_stats))
                return -EFAULT;

            mac_dev_stats_to_emac_stats(&ethswctl->emac_stats_s, &mac_stats);
            if (ethswctl_field_copy_to_user(emac_stats_s))
                return -EFAULT;

            return 0;
        }

    case ETHSWPORTPAUSECAPABILITY:            
        enet_dbg("ethswctl ETHSWPORTPAUSECAPABILITY ioctl: %s\n", ethswctl->type == TYPE_GET ? "get" : "set");
        if (!(port = _compat_port_object_from_unit_port(ethswctl->unit, ethswctl->port)))
            return -EFAULT;

        if (ethswctl->type == TYPE_GET)
        {
            int rx_enable, tx_enable;

            if (port_pause_get(port, &rx_enable, &tx_enable))
                return -EFAULT;

            if (rx_enable)
                ethswctl->ret_val = tx_enable ? PAUSE_FLOW_CTRL_BOTH : PAUSE_FLOW_CTRL_RX;
            else
                ethswctl->ret_val = tx_enable ? PAUSE_FLOW_CTRL_TX : PAUSE_FLOW_CTRL_NONE;

            if (ethswctl_field_copy_to_user(ret_val))
                return -EFAULT;

            return 0;
        }
        else if (ethswctl->type == TYPE_SET)
        {
            int rx_enable = (ethswctl->val == PAUSE_FLOW_CTRL_RX) || (ethswctl->val == PAUSE_FLOW_CTRL_BOTH);
            int tx_enable = (ethswctl->val == PAUSE_FLOW_CTRL_TX) || (ethswctl->val == PAUSE_FLOW_CTRL_BOTH);

            if (port_pause_set(port, rx_enable, tx_enable))
                return -EFAULT;

            return 0;
        }

        return -EFAULT;
    
    case ETHSWUNITPORT:
        {
            int u, p;
            struct net_device *dev;

            if (get_root_dev_by_name(ethswctl->ifname, &dev))
                return -EFAULT;

            for (u = 0; u < BP_MAX_ENET_MACS; u++)
            {
                for (p = 0; p < COMPAT_MAX_SWITCH_PORTS; p++)
                {
                    if (!COMPAT_PORT(u, p) || COMPAT_PORT(u, p)->dev != dev)
                        continue;

                    ethswctl->unit = u;
                    ethswctl->port_map = 1 << p;
                    break;
                }

                if (p != COMPAT_MAX_SWITCH_PORTS)
                    break;
            }
            
            if (p == COMPAT_MAX_SWITCH_PORTS)
                return -EFAULT;

            if (ethswctl_field_copy_to_user(unit) || 
                ethswctl_field_copy_to_user(port_map))
                return -EFAULT;

            //enet_dbgv("ETHSWUNITPORT: if=[%s] p=%d u=%d\n", ethswctl->ifname, p, u);
            return 0;
        }

    case ETHSWSTATPORTCLR:
        if (!(port = _compat_port_object_from_unit_port(ethswctl->unit, ethswctl->port)))
            return -EFAULT;

        port_stats_clear(port);
        return 0;

#ifdef RUNNER
    case ETHSWCPUMETER:
        {
            if (ethswctl->type == TYPE_GET)
            {
                enet_err("%s ETHSWCPUMETER: Unsupported request\n", __FUNCTION__);
                return -EFAULT;
            }

            if (!(port = _compat_port_object_from_unit_port(ethswctl->unit, ethswctl->port)))
                return -EFAULT;

            if (configure_bc_rate_limit_meter(port->p.port_id, ethswctl->cpu_meter_rate_limit.rate_limit))
            {
                enet_err("%s ETHSWCPUMETER: Configure CPU BC rate limit failed!\n", __FUNCTION__);
                return -EFAULT;
            }

            return 0;
        }
    case ETHSWPORTSALDAL:
        {
            int rc;
            bdmf_object_handle port_obj;
            rdpa_port_dp_cfg_t cfg;

            if (ethswctl->type != TYPE_SET)
                return -EFAULT;

            if (ethswctl->unit) /* unit == 0 for modifying lan ports, unit == 1 for rdpa_if_wan0 */
                port_by_port_id(root_sw, rdpa_if_wan0, &port); /* FIXME: MULTI-WAN XPON */
            else
                port = _compat_port_object_from_unit_port(ethswctl->unit, ethswctl->port);

            if (!(port_obj = _port_rdpa_object_by_port(port)))
                return -EFAULT;

            if ((rc = rdpa_port_cfg_get(port_obj , &cfg)))
                return -EFAULT;

            cfg.sal_enable = cfg.dal_enable = ethswctl->sal_dal_en;
            if ((rc = rdpa_port_cfg_set(port_obj, &cfg)))
            {
                enet_err("failed to set rdpa sal/dal port configuration on %s\n", port->obj_name);
                return -EFAULT;
            }

            return 0;
        }
    case ETHSWPORTTRANSPARENT:
        {
            bdmf_object_handle port_obj;
            rdpa_port_vlan_isolation_t vlan_isolation;
            int rc;

            enet_dbg("ETHSWPORTTRANSPARENT ioctl");

            if (ethswctl->type != TYPE_SET)
                return -EFAULT;

            if (!(port_obj = COMPAT_RPDA(ethswctl->unit, ethswctl->port)))
                return -EFAULT;

            if ((rc = rdpa_port_transparent_set(port_obj, ethswctl->transparent)))
                return -EFAULT;

            rc = rdpa_port_vlan_isolation_get(port_obj, &vlan_isolation);
            if (rc >= 0)
            {
                vlan_isolation.ds = !ethswctl->transparent;
                vlan_isolation.us = !ethswctl->transparent;
                rc = rdpa_port_vlan_isolation_set(port_obj, &vlan_isolation);
            }
            
            return rc ? -EFAULT : 0;
        }
    case ETHSWPORTVLANISOLATION:
        {
            bdmf_object_handle port_obj;
            rdpa_port_vlan_isolation_t vlan_isolation;
            int rc;

            enet_dbg("ETHSWPORTVLANISOLATION ioctl");

            if (ethswctl->type != TYPE_SET)
                return -EFAULT;
            
            if (!(port_obj = COMPAT_RPDA(ethswctl->unit, ethswctl->port)))
                return -EFAULT;

            rc = rdpa_port_vlan_isolation_get(port_obj, &vlan_isolation);
            if (rc >= 0)
            {
                vlan_isolation.ds = ethswctl->vlan_isolation.ds_enable;
                vlan_isolation.us = ethswctl->vlan_isolation.us_enable;
                rc = rdpa_port_vlan_isolation_set(port_obj, &vlan_isolation);
            }

            return rc ? -EFAULT : 0;
        }
    case ETHSWRDPAPORTGETFROMNAME:
        {
            rdpa_if index;

            if (get_port_by_if_name(ethswctl->ifname, &port))
                return -EFAULT;

            if (_port_rdpa_if_by_port(port, &index))
            {
                enet_err("cannot retreive an rdpa index for %s\n", ethswctl->ifname);
                return -EFAULT;
            }
            
            ethswctl->val = index;

            if (ethswctl_field_copy_to_user(val))
                return -EFAULT;

            return 0;
        }
#endif
    case ETHSWGETIFNAME:
        if (!(port = _compat_port_object_from_unit_port(ethswctl->unit, ethswctl->port)))
            return -EFAULT;

        if (!port->dev)
            return -EFAULT;
        
        strcpy(ethswctl->ifname, port->dev->name);
        if (ethswctl_field_copy_to_user(ifname))
            return -EFAULT;

        return 0;

    case ETHSWPHYCFG: /* get boardparams phyid value */
        if (!(port = _compat_port_object_from_unit_port(ethswctl->val, ethswctl->port)) || !port->p.phy)
            return -EFAULT;
        ethswctl->phycfg = port->p.phy->meta_id;

        if (ethswctl_field_copy_to_user(phycfg))
            return -EFAULT;

        return 0;

    case ETHSWSWITCHING:
        if (_enet_ioctl_hw_switching_set(ethswctl))
            return -EFAULT;

        if (ethswctl->type == TYPE_GET && ethswctl_field_copy_to_user(status))
            return -EFAULT;
        return 0;

    case ETHSWSWITCHFLAG:
        _enet_ioctl_hw_switch_flag_set(ethswctl);

        return 0;

    case ETHSWLINKSTATUS:
        // enet_dbg("ETHSWLINKSTATUS\n");

        if (!(port = _compat_port_object_from_unit_port(ethswctl->unit, ethswctl->port)) || !port->p.phy)
            return -EFAULT;

        if (ethswctl->type != TYPE_GET)
        {
#if defined(DSL_RUNNER_DEVICE)
            link_change_handler(port, ethswctl->status, ethswctl->speed, ethswctl->duplex);
#else
            return -EFAULT;
#endif
        }
        else
        {
            ethswctl->status = port->p.phy ? port->p.phy->link : 1;
            if (ethswctl_field_copy_to_user(status))
                return -EFAULT;
        }

        return 0;

    case ETHSWPHYAUTONEG:
        {
            uint32_t caps;

            if (!(port = _compat_port_object_from_unit_port(ethswctl->unit, ethswctl->port)))
                return -EFAULT;

            if (!port->p.phy)
            {
                enet_err("port - %s has no PHY conncted \n", port->name);
                return -EFAULT;
            }

            if (ethswctl->type == TYPE_GET)
            {
                if (phy_dev_caps_get(port->p.phy, &caps))
                    return -EFAULT;

                ethswctl->autoneg_info = (caps & PHY_CAP_AUTONEG) ? 1 : 0;
                ethswctl->autoneg_ad = 0;

                if (caps & PHY_CAP_10_HALF)
                    ethswctl->autoneg_ad |= AN_10M_HALF;

                if (caps & PHY_CAP_10_FULL)
                    ethswctl->autoneg_ad |= AN_10M_FULL;

                if (caps & PHY_CAP_100_HALF)
                    ethswctl->autoneg_ad |= AN_100M_HALF;

                if (caps & PHY_CAP_100_FULL)
                    ethswctl->autoneg_ad |= AN_100M_FULL;

                if (caps & PHY_CAP_1000_HALF)
                    ethswctl->autoneg_ad |= AN_10M_HALF;

                if (caps & PHY_CAP_1000_FULL)
                    ethswctl->autoneg_ad |= AN_1000M_FULL;

                if (caps & PHY_CAP_PAUSE)
                    ethswctl->autoneg_ad |= AN_FLOW_CONTROL;
                
                /* Copied from impl5... */
                ethswctl->autoneg_local = ethswctl->autoneg_ad;
                ethswctl->ret_val = 0;

                if (copy_to_user(rq->ifr_data , ethswctl, sizeof(struct ethswctl_data)))
                    return -EFAULT;

                return 0;
            }
            else
            {
                if (port->port_type != PORT_TYPE_RUNNER_EPON)
                {
                    if (ethswctl->autoneg_info & AUTONEG_CTRL_MASK)
                            return phy_dev_init(port->p.phy) ? -EFAULT : 0;

                    if (phy_dev_caps_get(port->p.phy, &caps))
                        return -EFAULT;

                    caps &= ~PHY_CAP_AUTONEG;
                }
                else
                {
                    if (phy_dev_caps_get(port->p.phy, &caps))
                        return -EFAULT;
                    
                    if (ethswctl->autoneg_info & AUTONEG_CTRL_MASK)
                        caps |= PHY_CAP_AUTONEG;
                    else
                        caps &= ~PHY_CAP_AUTONEG;
                }

                return phy_dev_caps_set(port->p.phy, caps) ? -EFAULT : 0;
            }
        }

    case ETHSWPHYAUTONEGCAPADV:
        {
            uint32_t caps;

            if (ethswctl->type != TYPE_SET)
                return -EFAULT;

            if (!(port = _compat_port_object_from_unit_port(ethswctl->unit, ethswctl->port)))
                return -EFAULT;

            if (!port->p.phy)
            {
                enet_err("port - %s has no PHY conncted \n", port->name);
                return -EFAULT;
            }

            if (phy_dev_caps_get(port->p.phy, &caps))
                return -EFAULT;

            caps &= ~(PHY_CAP_10_HALF | PHY_CAP_10_FULL | PHY_CAP_100_HALF | PHY_CAP_100_FULL | PHY_CAP_1000_HALF |
                PHY_CAP_1000_FULL | PHY_CAP_PAUSE);

            caps |= PHY_CAP_AUTONEG;

            if (ethswctl->autoneg_local & AN_FLOW_CONTROL)
                caps |= PHY_CAP_PAUSE;

            if (ethswctl->autoneg_local & AN_10M_HALF)
                caps |= PHY_CAP_10_HALF;

            if (ethswctl->autoneg_local & AN_10M_FULL)
                caps |= PHY_CAP_10_FULL;

            if (ethswctl->autoneg_local & AN_100M_HALF)
                caps |= PHY_CAP_100_HALF;

            if (ethswctl->autoneg_local & AN_100M_FULL)
                caps |= PHY_CAP_100_FULL;

            if (ethswctl->autoneg_local & AN_1000M_HALF)
                caps |= PHY_CAP_1000_HALF;

            if (ethswctl->autoneg_local & AN_1000M_FULL)
                caps |= PHY_CAP_1000_FULL;

            return phy_dev_caps_set(port->p.phy, caps) ? -EFAULT : 0;
        }

    case ETHSWPHYMODE:
        {
            if (ethswctl->addressing_flag & ETHSW_ADDRESSING_DEV)
                port = ((enetx_netdev *)netdev_priv(dev))->port;
            else
                port = _compat_port_object_from_unit_port(ethswctl->unit, ethswctl->port);

            if (!port)
                return -EFAULT;

            phy_dev = port->p.phy;
            if (!phy_dev)
            {
                enet_err("port - %s has no PHY conncted \n", port->name);
                return -EFAULT;
            }

            if (ethswctl->type == TYPE_SET)
            {
                phy_speed_t speed;

                if (ethswctl->speed == 10)
                    speed = PHY_SPEED_10;
                else if (ethswctl->speed == 100)
                    speed = PHY_SPEED_100;
                else if (ethswctl->speed == 1000)
                    speed = PHY_SPEED_1000;
                else if (ethswctl->speed == 2500)
                    speed = PHY_SPEED_2500;
                else if (ethswctl->speed == 5000)
                    speed = PHY_SPEED_5000;
                else if (ethswctl->speed == 10000)
                    speed = PHY_SPEED_10000;
                else if (ethswctl->speed == 0)
                    speed = PHY_SPEED_AUTO;
                else
                    speed = PHY_SPEED_UNKNOWN;

                return phy_dev_speed_set(phy_dev, speed,
                    ethswctl->duplex ? PHY_DUPLEX_FULL : PHY_DUPLEX_HALF) ? -EFAULT : 0;
            }
            else if (ethswctl->type == TYPE_GET)
            {
                uint32_t caps = 0;
                phy_speed_t speed; 
                phy_duplex_t duplex;


                if (phy_dev_caps_get(phy_dev, &caps))
                    return -EFAULT;

                if (phy_dev->cascade_prev) {
                    uint32_t caps_prev;
                    phy_dev_caps_get(phy_dev->cascade_prev, &caps_prev);
                    caps &= caps_prev;
                }

                if (phy_dev_config_speed_get(phy_dev, &speed, &duplex))
                    return -EFAULT;

                switch(speed) {
                    case PHY_SPEED_AUTO:
                        ethswctl->cfgSpeed = 0;
                        break;
                    case PHY_SPEED_10000:
                        ethswctl->cfgSpeed = 10000;
                        break;
                    case PHY_SPEED_5000:
                        ethswctl->cfgSpeed = 5000;
                        break;
                    case PHY_SPEED_2500:
                        ethswctl->cfgSpeed = 2500;
                        break;
                    case PHY_SPEED_1000:
                        ethswctl->cfgSpeed = 1000;
                        break;
                    case PHY_SPEED_100:
                        ethswctl->cfgSpeed = 100;
                        break;
                    case PHY_SPEED_10:
                        ethswctl->cfgSpeed = 10;
                        break;
                    default:
                        ethswctl->cfgSpeed = 0;
                }

                if (duplex == PHY_DUPLEX_FULL)
                    ethswctl->cfgDuplex = 1;
                else
                    ethswctl->cfgDuplex = 0;

                ethswctl->phyCap = 0;
                ethswctl->phyCap |= (caps & PHY_CAP_AUTONEG) ? PHY_CFG_AUTO_NEGO : 0;
                ethswctl->phyCap |= (caps & PHY_CAP_10_HALF) ? PHY_CFG_10HD : 0;
                ethswctl->phyCap |= (caps & PHY_CAP_10_FULL) ? PHY_CFG_10FD : 0;
                ethswctl->phyCap |= (caps & PHY_CAP_100_HALF) ? PHY_CFG_100HD : 0;
                ethswctl->phyCap |= (caps & PHY_CAP_100_FULL) ? PHY_CFG_100FD : 0;
                ethswctl->phyCap |= (caps & PHY_CAP_1000_HALF) ? PHY_CFG_1000HD : 0;
                ethswctl->phyCap |= (caps & PHY_CAP_1000_FULL) ? PHY_CFG_1000FD : 0;
                ethswctl->phyCap |= (caps & PHY_CAP_2500) ? PHY_CFG_2500FD : 0;
                ethswctl->phyCap |= (caps & PHY_CAP_5000) ? PHY_CFG_5000FD : 0;
                ethswctl->phyCap |= (caps & PHY_CAP_10000) ? PHY_CFG_10000FD : 0;
                        
                if (phy_dev->link)
                {
                    ethswctl->duplex = (phy_dev->duplex == PHY_DUPLEX_FULL) ? 1 : 0;

                    if (phy_dev->speed == PHY_SPEED_10)
                        ethswctl->speed = 10;
                    else if (phy_dev->speed == PHY_SPEED_100)
                        ethswctl->speed = 100;
                    else if (phy_dev->speed == PHY_SPEED_1000)
                        ethswctl->speed = 1000;
                    else if (phy_dev->speed == PHY_SPEED_2500)
                        ethswctl->speed = 2500;
                    else if (phy_dev->speed == PHY_SPEED_5000)
                        ethswctl->speed = 5000;
                    else if (phy_dev->speed == PHY_SPEED_10000)
                        ethswctl->speed = 10000;
                    else
                        return -EFAULT;
                }
                else
                {
                    ethswctl->speed = 0;
                    ethswctl->duplex = 0;
                }

                if (copy_to_user(rq->ifr_data , ethswctl, sizeof(struct ethswctl_data)))
                    return -EFAULT;

                return 0;
            }

            return -EFAULT;
        }

#if defined(CONFIG_BCM_ETH_PWRSAVE)
    case ETHSWPHYAPD:
        if (ethswctl->type == TYPE_GET)
            ethswctl->val = pm_apd_get();
        else
            pm_apd_set(ethswctl->val);

        ethswctl->ret_val = 1;
        if (copy_to_user(rq->ifr_data , ethswctl, sizeof(struct ethswctl_data)))
            return -EFAULT;

        return 0;
#endif

#if defined(CONFIG_BCM_ENERGY_EFFICIENT_ETHERNET)
    case ETHSWPHYEEE:
        if (ethswctl->type == TYPE_GET)
            ethswctl->val = pm_eee_get();
        else
            pm_eee_set(ethswctl->val);

        ethswctl->ret_val = 1;
        if (copy_to_user(rq->ifr_data , ethswctl, sizeof(struct ethswctl_data)))
            return -EFAULT;

        return 0;
#endif

#ifdef RUNNER
    case ETHSWOAMIDXMAPPING:
        if (ethswctl->oam_idx_str.map_sub_type == OAM_MAP_SUB_TYPE_FROM_UNIT_PORT)
        {
            if (_compat_validate_unit_port(ethswctl->unit, ethswctl->port))
                return -EFAULT;

            ethswctl->oam_idx_str.oam_idx = COMPAT_OAM_IDX(ethswctl->unit, ethswctl->port);
            if (ethswctl->oam_idx_str.oam_idx == -1)
                return -EFAULT;

            if (copy_to_user(rq->ifr_data , ethswctl, sizeof(struct ethswctl_data)))
                return -EFAULT;
            return 0;
        }
        else
        {
            int u, p;

            for (u = 0; u < BP_MAX_ENET_MACS; u++)
            {
                for (p = 0; p < COMPAT_MAX_SWITCH_PORTS; p++)
                {
                    if (COMPAT_OAM_IDX(u, p) == ethswctl->oam_idx_str.oam_idx)
                        break;
                }

                if (p != COMPAT_MAX_SWITCH_PORTS)
                    break;
            }
            
            if (p == COMPAT_MAX_SWITCH_PORTS)
                return -EFAULT;

            ethswctl->unit = u;
            ethswctl->port = p;
            
            if (ethswctl->oam_idx_str.map_sub_type == OAM_MAP_SUB_TYPE_TO_RDPA_IF)
            {
                rdpa_if index;

                if (_port_rdpa_if_by_port(unit_port_array[u][p], &index))
                    return -EFAULT;

                ethswctl->oam_idx_str.rdpa_if = index;
            }

            if (copy_to_user(rq->ifr_data , ethswctl, sizeof(struct ethswctl_data)))
                return -EFAULT;

            return 0;

        }
#endif

        return -EFAULT;
    }

    return -(EOPNOTSUPP);
}

int enet_ioctl_compat(struct net_device *dev, struct ifreq *rq, int cmd)
{
    enetx_port_t *port = NULL;
    union {
        struct ethswctl_data ethswctl_data;
        struct interface_data interface_data;
        ifreq_ext_t ifre;
    } rq_data;

    struct ethswctl_data *ethswctl = (struct ethswctl_data *)&rq_data;
    ifreq_ext_t *ifx = (ifreq_ext_t *)&rq_data;

#if defined(enet_dbgv_ioctl_enabled)
    /* print 1 line showing type of ioctl */
    switch (cmd)
    {
    /* exclude following ioctl */
        case SIOCDEVPRIVATE:
        case SIOCETHSWCTLOPS:       /* ethswctl has its own debug print */
        case SIOCETHCTLOPS:         /* ethctl has its own debug print */
        case SIOCGQUERYNUMPORTS:
        case SIOCGLINKSTATE:
        case SIOCGWANPORT:
        case SIOCSWANPORT:
            break;
        default:
            enet_dbgv(" dev=%s, cmd=%x ...\n", NETDEV_PRIV(dev)->port->obj_name, cmd);
    }
#endif

    switch (cmd)
    {
#if defined(DSL_RUNNER_DEVICE)
    case SIOCGSWITCHPORT:   // Get Switch Port from name
        {
            int u, p;
            struct net_device *dev;
            struct interface_data *enetif_data = (struct interface_data*)&rq_data;
            
            if (copy_from_user(enetif_data, rq->ifr_data, sizeof(*enetif_data)))
                return -EFAULT;
            if (get_root_dev_by_name(enetif_data->ifname, &dev))
                return -EFAULT;

            for (u = 0; u < BP_MAX_ENET_MACS; u++)
            {
                for (p = 0; p < COMPAT_MAX_SWITCH_PORTS; p++)
                {
                    if (!COMPAT_PORT(u, p) || COMPAT_PORT(u, p)->dev != dev)
                        continue;
                    enetif_data->switch_port_id = PHYSICAL_PORT_TO_LOGICAL_PORT(p,u);
                    if (copy_to_user(rq->ifr_data, enetif_data, sizeof(*enetif_data)))
                        return -EFAULT;
                    return 0;
                }
            }

           return -EFAULT;
        }
#endif //DSL_RUNNER_DEVICE
    case SIOCGWANPORT:
        {
            list_ctx ctx;

            if (copy_from_user(ethswctl, rq->ifr_data, sizeof(struct ethswctl_data)))
                return -EFAULT;

            if (ethswctl->up_len.len <= 0)
            return -EFAULT;

            if (!(ctx.str = kzalloc(ethswctl->up_len.len, GFP_KERNEL)))
                return -EFAULT;

            ctx.maxlen = ethswctl->up_len.len;
            ctx.str[0] = '\0';
            if (!port_traverse_ports(root_sw, tr_wan_devname_concat, PORT_CLASS_PORT, &ctx))
            {
                if (copy_to_user(ethswctl->up_len.uptr, ctx.str, ethswctl->up_len.len))
                    return -EFAULT;
            }

            kfree(ctx.str);
            return 0;
        }

    case SIOCSCLEARMIBCNTR:
        {
            if (get_port_by_if_name(rq->ifr_name, &port))
                return -EFAULT;
            port_traverse_ports(port, tr_stats_clear, PORT_CLASS_PORT | PORT_CLASS_SW | PORT_CLASS_PORT_UNDEF, NULL);
            return 0;
        }
    case SIOCMIBINFO:
        {
            IOCTL_MIB_INFO mib = { };

            port = ((enetx_netdev *)netdev_priv(dev))->port;
            if (port->port_class != PORT_CLASS_PORT)
                return -EFAULT;
    
            if (port->p.phy && port->p.phy->link)
            {
                phy_dev_t *phy_dev = port->p.phy;

                mib.ulIfDuplex = (phy_dev->duplex == PHY_DUPLEX_FULL) ? 1 : 0;
                mib.ulIfLastChange = port->p.phy_last_change;

                if (phy_dev->speed == PHY_SPEED_10)
                    mib.ulIfSpeed = SPEED_10MBIT;
                else if (phy_dev->speed == PHY_SPEED_100)
                    mib.ulIfSpeed = SPEED_100MBIT;
                else if (phy_dev->speed == PHY_SPEED_1000)
                    mib.ulIfSpeed = SPEED_1000MBIT;
                else /* TODO: add 2500, 10000 speed */
                    return -EFAULT;
            }

            if (copy_to_user(rq->ifr_data, (void *)&mib, sizeof(mib)))
                return -EFAULT;

            return 0;
        }
    case SIOCGLINKSTATE:
        {
            int ret, data = 0; /* XXX 64 support ?? */

            port = ((enetx_netdev *)netdev_priv(dev))->port;
            ret = port_traverse_ports(port, tr_is_phy_up, PORT_CLASS_PORT, NULL);
            data = ret != 0;

            //enet_dbgv("SIOCGLINKSTATE: %s, link %s ...\n", dev->name, data? "up":"down");
            if (copy_to_user(rq->ifr_data, &data, sizeof(data)))
                return -EFAULT;

            return 0;
        }
        
#ifdef GPON
    case SIOCGPONIF:
        enet_dbg("SIOCGPONIF\n");
        return _handle_gpon(rq);
#endif
    case SIOCSWANPORT:
        {
            int data = (unsigned long)rq->ifr_data;

            port = ((enetx_netdev *)netdev_priv(dev))->port;
            return port_netdev_role_set(port, data ? PORT_NETDEV_ROLE_WAN : PORT_NETDEV_ROLE_LAN) ? -EFAULT : 0;
        }

    case SIOCIFREQ_EXT:
        {
            list_ctx ctx;
            port_cap_t port_cap;

            if (copy_from_user(ifx, rq->ifr_data, sizeof(*ifx))) 
                return -EFAULT;

            BCM_IOC_PTR_ZERO_EXT(ifx->stringBuf);
            switch (ifx->opcode)
            {
            case SIOCGPORTWANONLY:
                port_cap = PORT_CAP_WAN_ONLY;
                break;
            case SIOCGPORTWANPREFERRED:
                port_cap = PORT_CAP_WAN_PREFERRED;
                break;
            case SIOCGPORTLANONLY:
                port_cap = PORT_CAP_LAN_ONLY;
                break;
            default:
                return -EFAULT;
            }

            if (ifx->bufLen <= 0)
                return -EFAULT;

            if (!(ctx.str = kzalloc(ifx->bufLen, GFP_KERNEL)))
                return -EFAULT;

            ctx.maxlen = ifx->bufLen;
            ctx.op_code = port_cap;
            ctx.str[0] = '\0';
            if (!port_traverse_ports(root_sw, tr_ifreq_devname_concat, PORT_CLASS_PORT, &ctx))
            {
                if (copy_to_user(ifx->stringBuf, ctx.str, ifx->bufLen))
                    return -EFAULT;
            }

            kfree(ctx.str);

            return 0;
        }

    case SIOCGQUERYNUMPORTS:
        {
            uint32_t map = 0;

            port = NETDEV_PRIV(dev)->port;
            if (port->port_class != PORT_CLASS_PORT)
                return -EFAULT;

            crossbar_group_external_endpoint_count(port->p.phy, &map);
            map <<= BP_CROSSBAR_PORT_BASE;

            if (copy_to_user(rq->ifr_data, (void *)&map, sizeof(map)))
                return -EFAULT;

            return 0;
        }

    case SIOCETHCTLOPS:
        return enet_ioctl_compat_ethctl(dev, rq, cmd);
        
    case SIOCETHSWCTLOPS:
        enet_ioctl_compat_ethswctl(dev, rq, cmd);
        return 0;

#ifdef CONFIG_BCM_PTP_1588
    case SIOCSHWTSTAMP:
        return ptp_ioctl_hwtstamp_set(dev, rq);

    case SIOCGHWTSTAMP:
        return ptp_ioctl_hwtstamp_get(dev, rq);
#endif

    }

    return -EOPNOTSUPP;
}


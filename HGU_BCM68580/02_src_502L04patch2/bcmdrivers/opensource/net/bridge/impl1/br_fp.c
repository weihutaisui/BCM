/*
* <:copyright-BRCM:2012:DUAL/GPL:standard
* 
*    Copyright (c) 2012 Broadcom 
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

#include <linux/version.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/proc_fs.h>
#include <linux/device.h>
#include <linux/spinlock.h>
#include <linux/if_vlan.h>
#include <linux/if_bridge.h>
#include <asm/cpu.h>
#include <asm/uaccess.h>
#include <asm/string.h>
#include "bcm_OS_Deps.h"
#include "br_fp.h"
#include "br_private.h"
#include "rdpa_api.h"
#include "rdpa_mw_blog_parse.h"
#include "bcmenet_common.h"

#if defined(CONFIG_EPON_SFU) && defined(CONFIG_BCM_PON_XRDP)
static char *aggr_br_name_prefix = "br_aggr";
#endif
static char init_br_name[IFNAMSIZ] = "br0";
static bdmf_object_handle init_br_obj = NULL;
module_param_string(init_br_name, init_br_name, IFNAMSIZ, 0);

static bdmf_fastlock bridge_lock;
static bdmf_mac_t zero_mac;

static int is_wlan_accl_enabled(void)
{
    bdmf_object_handle cpu_obj;
                
    if (!rdpa_cpu_get(rdpa_cpu_wlan0, &cpu_obj))
    {
        bdmf_put(cpu_obj);
        return 1;
    }
    return 0;
}

static int is_vlan(struct net_device *dev)
{
    return dev->priv_flags & IFF_BCM_VLAN;
}

static rdpa_bridge_type rdpa_bridge_type_get(bdmf_object_handle bridge_object)
{
    rdpa_bridge_cfg_t br_cfg;

    rdpa_bridge_config_get(bridge_object, &br_cfg);
    
    return br_cfg.type;
}

static void br_fp_rdpa_fdb_key_set(struct net_bridge_fdb_entry *fdb,
    rdpa_fdb_key_t *key, bdmf_object_handle br_obj)
{
    memset(key, 0, sizeof(*key));
    memcpy(&key->mac, fdb->addr.addr, sizeof(key->mac));
    if (rdpa_bridge_type_get(br_obj) == rdpa_bridge_802_1q)
        key->vid = GET_FDB_VLAN(fdb);
}

static bdmf_error_t br_fp_add_or_modify_mac_tbl_entry(struct net_bridge_fdb_entry *fdb, int is_add)
{
    bdmf_object_handle bridge;
    rdpa_fdb_key_t key;
    rdpa_fdb_data_t data;
    rdpa_if port;
    struct net_device *root_dev;
    bdmf_error_t rc;

    root_dev = netdev_path_get_root(fdb->dst->dev);
    if (!root_dev)
        BDMF_TRACE_RET(BDMF_ERR_NODEV, "Cannot find the physical device of %s", fdb->dst->dev->name);

    port = rdpa_mw_root_dev2rdpa_if(root_dev);
    if (port == rdpa_if_none)
    {
       	BDMF_TRACE_RET(BDMF_ERR_PARM, "%s MAC Table: invalid entry. src dev name[%s] mac[%pM]", is_add ? "ADD" : "MOD",
            root_dev->name, fdb->addr.addr);
    }

    bridge = (bdmf_object_handle)fdb->dst->br->fp_hooks.rdpa_priv;
    if (!bridge)
        return BDMF_ERR_NOENT;

    br_fp_rdpa_fdb_key_set(fdb, &key, bridge);

    bdmf_fastlock_lock(&bridge_lock);

    rc = rdpa_bridge_mac_get(bridge, &key, &data);
    if (is_add && !rc)
    {
        /*when using ingress classifier we have a usecase which there is an extra entry in RDPA which will not be deleted
         *thus we have to add it anyway with the real bridge port*/
        if (!(data.ports & rdpa_if_id(rdpa_if_cpu)))
        {
            rc = BDMF_ERR_ALREADY; /* The MAC entry already exists for this bridge. */
            goto exit;
        }
    }

    data.ports = rdpa_if_id(port);
    data.sa_action = rdpa_forward_action_forward;
    if (rdpa_if_is_wifi(port) && !is_wlan_accl_enabled())
        data.da_action = rdpa_forward_action_host;
    else
        data.da_action = rdpa_forward_action_forward;

    rc = rdpa_bridge_mac_set(bridge, &key, &data);
    if (rc)
    {
        bdmf_trace("Failed to add mac entry, src dev name[%s] mac[%pM] (rc=%d)", fdb->dst->dev->name,
            fdb->addr.addr, rc);
    }

#if defined(CONFIG_BCM_KF_BRIDGE_COUNTERS)
    if ((rc == BDMF_ERR_NO_MORE) && !(fdb->is_static))
    {
        fdb->dst->br->mac_entry_discard_counter++;
    }
#endif

exit:
    bdmf_fastlock_unlock(&bridge_lock);
    return rc;
}

static int exists_in_other_bridge(struct net_device *dev_orig, unsigned char *addr)
{
    int i;
    struct hlist_node *h;
    struct net_bridge_fdb_entry *f;
    struct net_bridge *br;
    struct net_device *dev;

    for_each_netdev(&init_net, dev)
    {
        if (!(dev->priv_flags & IFF_EBRIDGE) || dev == dev_orig)
            continue;

        br = netdev_priv(dev);

        for (i = 0; i < BR_HASH_SIZE; i++)
        {
            HLIST_FOR_EACH_ENTRY_RCU(f, h, &br->hash[i], hlist)
            {
                if (f->is_local) 
                    continue;

                if (is_ether_addr_same(f->addr.addr, addr))
                    return 1;
            }
        }
    }

    return 0;
}

static int br_fp_remove_mac_tbl_entry(struct net_bridge_fdb_entry *fdb, int remove_dups)
{
    struct net_bridge_port *p;
    bdmf_object_handle bridge;
    rdpa_fdb_key_t key = {};
    bdmf_error_t rc;

    p = br_port_get_rcu(fdb->dst->dev);
    if (!p)
        return -1;

    if (!remove_dups &&
        exists_in_other_bridge(p->br->dev, fdb->addr.addr))
    {
        return 0;
    }

    bridge = (bdmf_object_handle)fdb->dst->br->fp_hooks.rdpa_priv;
    if (!bridge)
        return BDMF_ERR_NOENT;

    br_fp_rdpa_fdb_key_set(fdb, &key, bridge);
    rc = rdpa_bridge_mac_set(bridge, &key, NULL);
    if (rc == BDMF_ERR_NOENT)
    {
        BDMF_TRACE_DBG("Failed to remove mac entry, src dev name[%s] mac[%pM]", fdb->dst->dev->name,
            fdb->addr.addr);
        
        return rc;
    }
    else 
    {
        BDMF_TRACE_RET(rc, "Failed to remove mac entry, src dev name[%s] mac[%pM]", fdb->dst->dev->name,
            fdb->addr.addr);
    }

    return 0;
}

static int br_fp_ageing_mac_tbl_entry(struct net_bridge_fdb_entry *fdb, int *age_valid)
{
    bdmf_error_t rc;
    bdmf_boolean _age_valid = 0;
    bdmf_object_handle bridge;
    rdpa_fdb_key_t key = {};

    bridge = (bdmf_object_handle)fdb->dst->br->fp_hooks.rdpa_priv;
    if (!bridge)
        return BDMF_ERR_NOENT;

    br_fp_rdpa_fdb_key_set(fdb, &key, bridge);
    rc = rdpa_bridge_mac_status_get(bridge, &key, &_age_valid);
    if (rc)
    {
    	BDMF_TRACE_ERR("Aging check failed(%d). src dev name[%s] mac[%pM]", rc, fdb->dst->dev->name, fdb->addr.addr);
        _age_valid = 0; /* return status, which will cause deletion of fdb entry in kernel bridge */
    }

    *age_valid = _age_valid;
    return 0;
}

static bdmf_object_handle dev2rdpa_port(struct net_device *dev)
{
#if defined(CONFIG_BLOG)
    bdmf_object_handle port_obj = NULL;
    rdpa_if port;

    port = rdpa_mw_root_dev2rdpa_if(dev);
    if (port == rdpa_if_none)
    {
        BDMF_TRACE_ERR("Can't find rdpa_port for dev '%s'\n", dev->name);
        return NULL;
    }

    if (rdpa_port_get(port, &port_obj))
        return NULL;
    else
        return port_obj;
#else
    /* temporaliy fix before BLOG merge is finished. but can this driver run without blog? */
    printk(KERN_ERR "dev2rdpa_port not implemented yet for non blog config!!!\n");
    return NULL;
#endif
}

static int br_fp_rdpa_bridge_add(struct net_bridge *br)
{
    static int is_br0_init = 1;
    bdmf_object_handle br_obj = NULL;
    BDMF_MATTR(bridge_attr, rdpa_bridge_drv());

    rdpa_bridge_cfg_t rdpa_bridge_cfg = {
        .type = rdpa_bridge_802_1d,
        .learning_mode = rdpa_bridge_learn_svl,
        .auto_forward = 1,
        .auto_aggregate = 0,
    };

    /* Init sequence of external switch requires the bridge object to exist before we have it created in
     * Linux. So we look for the first configured bridge in RDPA, and expect it to be a single bridge in the system.
     * If won't found, new bridge object is added. */
    if (!strcmp(br->dev->name, init_br_name) && is_br0_init)
    {
        br_obj = bdmf_get_next(rdpa_bridge_drv(), NULL, NULL);
        init_br_obj = br_obj;
        is_br0_init = 0;
    }
#if defined(CONFIG_GPON_HGU) || defined(CONFIG_EPON_HGU)
    else
    {
        rdpa_bridge_cfg.type = rdpa_bridge_802_1q;
        rdpa_bridge_cfg.auto_aggregate = 1;
    }
#endif

    if (!br_obj)
    {
        int rc;
#if defined(CONFIG_EPON_SFU) && defined(CONFIG_BCM_PON_XRDP)
        if (!memcmp(br->dev->name, aggr_br_name_prefix, strlen(aggr_br_name_prefix)))
        {
            rdpa_bridge_cfg.type = rdpa_bridge_802_1q;
            rdpa_bridge_cfg.auto_aggregate = 1;
        }
#endif

        rdpa_bridge_config_set(bridge_attr, &rdpa_bridge_cfg);
        rc = bdmf_new_and_set(rdpa_bridge_drv(), NULL, bridge_attr, &br_obj);
        if (rc < 0)
        {
            BDMF_TRACE_ERR("Failed to add RDPA bridge object for %s, error %d\n", br->dev->name, rc);
            return -1;
        }
    }
    br->fp_hooks.rdpa_priv = br_obj;

    return 0;
}

static int br_fp_rdpa_bridge_del(struct net_bridge *br)
{
    if (!br->fp_hooks.rdpa_priv)
        return 0;

    if (br->fp_hooks.rdpa_priv == init_br_obj)
        bdmf_put((bdmf_object_handle)br->fp_hooks.rdpa_priv);
    else
        bdmf_destroy((bdmf_object_handle)br->fp_hooks.rdpa_priv);
    br->fp_hooks.rdpa_priv = NULL;
    return 0;
}

static int is_vlan_vid_enabled(bdmf_object_handle vlan_object)
{
    int i;
    bdmf_boolean vid_enabled = 0;

    for (i = 0; i < 4096 && !vid_enabled ; i++)
    {
        if (rdpa_vlan_vid_enable_get(vlan_object, i, &vid_enabled))
            vid_enabled = 0;
    }

    return vid_enabled;
}

static int is_netdev_exist(const char *dev_name)
{
    struct net_device *dev = dev_get_by_name(&init_net, dev_name);

    if (dev)
        dev_put(dev);

    return dev ? 1 : 0;
}

static bdmf_object_handle get_link_object(bdmf_object_handle bridge_object, struct net_device *dev)
{
    bdmf_object_handle link_object =  NULL, port_object = NULL, vlan_object = NULL;

    if (rdpa_bridge_type_get(bridge_object) == rdpa_bridge_802_1q &&
        is_vlan(dev) && !rdpa_vlan_get(dev->name, &vlan_object) && is_vlan_vid_enabled(vlan_object))
    {
        link_object = vlan_object;
    }
    else
    {
        link_object = port_object = dev2rdpa_port(netdev_path_get_root(dev));
    }

    if (port_object && vlan_object)
        bdmf_put(vlan_object);

    return link_object;
}

static int get_link_usage(bdmf_object_handle bridge_object, const char *object_name, struct net_bridge *br)
{
    struct net_bridge_port *br_port;
    int count = 0;

    list_for_each_entry(br_port, &br->port_list, list)
    {
        bdmf_object_handle link_object = get_link_object(bridge_object, br_port->dev);

        if (!link_object)
            continue;

        if (!strcmp(bdmf_object_name(link_object), object_name))
            count++;

        bdmf_put(link_object);
    }

    return count;
}

static int get_rdpa_objects(struct net_bridge *br, struct net_device *dev,
    bdmf_object_handle *bridge_object, bdmf_object_handle *link_object)
{
#if defined(CONFIG_GPON_HGU) || defined(CONFIG_EPON_HGU)
    if (dev->priv_flags & IFF_WANDEV)
        return -1;
#endif

    *bridge_object = (bdmf_object_handle)br->fp_hooks.rdpa_priv;
    if (!*bridge_object)
    {
        BDMF_TRACE_ERR("Can't find RDPA bridge object for %s\n", br->dev->name);
        return -1;
    }

    *link_object = get_link_object(*bridge_object, dev);
    if (!*link_object)
    {
        BDMF_TRACE_ERR("Can't find RDPA object for %s\n", dev->name);
        return -1;
    }

    return 0; 
}

static int br_fp_del_port(struct net_bridge *br, struct net_device *dev)
{
    bdmf_object_handle bridge_object = NULL, link_object = NULL;
    struct bdmf_link *plink;
    int ret = 0;

    rcu_read_lock();

    if (get_rdpa_objects(br, dev, &bridge_object, &link_object))
        goto Exit;

    if (get_link_usage(bridge_object, bdmf_object_name(link_object), br) > 1)
        goto Exit;

    if (!bdmf_is_linked(link_object, bridge_object, &plink))
        goto Exit;

    if ((ret = bdmf_unlink(link_object, bridge_object)))
    {
        BDMF_TRACE_ERR("Failed to unlink RDPA objects %s - %s, error %s (%d)\n",
            bdmf_object_name(link_object), bdmf_object_name(bridge_object),
            bdmf_strerror(ret), ret);
    }

Exit:
    if (link_object)
        bdmf_put(link_object);

    rcu_read_unlock();

    return ret; 
}

static int br_fp_add_port(struct net_bridge *br, struct net_device *dev)
{
    bdmf_object_handle bridge_object = NULL, link_object = NULL;
    struct bdmf_link *plink;
    int ret = 0;

    rcu_read_lock();

    if (get_rdpa_objects(br, dev, &bridge_object, &link_object))
        goto Exit;

    if (bdmf_is_linked(link_object, bridge_object, &plink))
        goto Exit;

    if ((ret = bdmf_link(link_object, bridge_object, NULL)))
    {
        BDMF_TRACE_ERR("Failed to link RDPA objects %s - %s, error %s (%d)\n",
            bdmf_object_name(link_object), bdmf_object_name(bridge_object),
            bdmf_strerror(ret), ret);
    }

Exit:
    if (link_object)
        bdmf_put(link_object);

    rcu_read_unlock();

    return ret; 
}

static int is_rdpa_bridge_supported_device(struct net_device *root_dev)
{
#ifdef XRDP
	/* In XRDP Runner the bridge doesn't support WLAN/NETXL ports */
    switch (netdev_path_get_hw_port_type(root_dev))
    {
        case BLOG_WLANPHY:
        case BLOG_NETXLPHY:
            return 0;
        default:
            break;
    }
#endif

    return 1;
}

static int br_fp_rdpa_hook(int cmd, void *param1, void *param2)
{
    struct net_bridge *br = NULL;
    struct net_device *dev = NULL;
    struct net_bridge_fdb_entry *fdb = NULL;
    int *age_valid = NULL;
    struct net_device *root_dev;
    unsigned int hw_port_type;

    if (cmd == BR_FP_PORT_ADD || cmd == BR_FP_PORT_REMOVE)
    {
        br = param1;
        dev = param2;
    }
    else
    {
        fdb = param1;
        age_valid = param2;
        dev = fdb->dst->dev;
    }

    if (!is_netdev_exist(dev->name))
        return 0;

    root_dev = netdev_path_get_root(dev);
    if (!root_dev)
        BDMF_TRACE_RET(0, "Cannot find the physical device of %s", dev->name);
    hw_port_type = netdev_path_get_hw_port_type(root_dev);

    switch (cmd)
    {
    case BR_FP_PORT_ADD:
        if (!is_rdpa_bridge_supported_device(root_dev))
            return 0;
        return br_fp_add_port(br, dev);
    case BR_FP_PORT_REMOVE:
        if (!is_rdpa_bridge_supported_device(root_dev))
            return 0;
        return br_fp_del_port(br, dev);
    /* Even if we don't add some devices to bridge, we do learn mac addresses so DA lookup won't fail */
    case BR_FP_FDB_ADD:
        return br_fp_add_or_modify_mac_tbl_entry(fdb, 1);
    case BR_FP_FDB_MODIFY:
        return br_fp_add_or_modify_mac_tbl_entry(fdb, 0);
    case BR_FP_FDB_REMOVE:
        return br_fp_remove_mac_tbl_entry(fdb, 0);
    case BR_FP_FDB_CHECK_AGE:
        return br_fp_ageing_mac_tbl_entry(fdb, age_valid);
    default: 
        break;
    }
    return -1;
}

static void do_fdb_entries(struct net_device *brdev, int add)
{
    int i;
    struct hlist_node *h;
    struct net_bridge_fdb_entry *f;
    struct net_bridge *br;

    br = netdev_priv(brdev);

    rcu_read_lock();
    for (i = 0; i < BR_HASH_SIZE; i++)
    {
        HLIST_FOR_EACH_ENTRY_RCU(f, h, &br->hash[i], hlist)
        {
            if (f->is_local)
                continue;

            if (add)
                br_fp_add_or_modify_mac_tbl_entry(f, 1);
            else
            {
                /* always remove, even if exists in more then 1 bridge */
                br_fp_remove_mac_tbl_entry(f, 1);
            }
        }
    }
    rcu_read_unlock();
}

/* this function is registered to device events. we are interested in mac change of the bridge*/
static int br_fp_notifier_call(struct notifier_block *nb, unsigned long event, void *_br)
{
    struct net_device *dev = NETDEV_NOTIFIER_GET_DEV(_br);
    static int up_br_ref_cnt = 0;
    bdmf_error_t rc;
    struct net_bridge *br;

    switch (event)    
    {
    /* NETDEV_CHANGEADDR is not enough - a device may be granted a MAC address
     * on its way up without moving through NETDEV_CHANGEADDR. */
    case NETDEV_REGISTER:
    case NETDEV_UNREGISTER:
        {
            if (!(dev->priv_flags & IFF_EBRIDGE))
                break;

            br = (struct net_bridge *)netdev_priv(dev);
            if (event == NETDEV_REGISTER)
                return br_fp_rdpa_bridge_add(br);
            else
                return br_fp_rdpa_bridge_del(br);
        }
    case NETDEV_UP:
    case NETDEV_DOWN:
    case NETDEV_CHANGEADDR:
        if (dev->priv_flags & IFF_EBRIDGE)
        {
            bdmf_object_handle bridge = NULL;
            bdmf_mac_t lan_mac;

            if (event == NETDEV_UP)
            {
                up_br_ref_cnt++;
                if (up_br_ref_cnt > 1)
                {
                    /* This is not the first bridge that we add, skip LAN mac configuration */
                    break;
                }
            }
            else if (event == NETDEV_DOWN)
            {
                up_br_ref_cnt--;
                if (up_br_ref_cnt)
                {
                    /* This is not the single bridge remained, skip LAN mac configuration */
                    break;
                }
            }

            bdmf_lock();
            bridge = bdmf_get_next(rdpa_bridge_drv(), NULL, NULL);
            if (bridge)
            {
                memcpy(&lan_mac.b, event == NETDEV_DOWN ? zero_mac.b : dev->dev_addr, sizeof(lan_mac));
                rc = rdpa_bridge_lan_mac_set(bridge, &lan_mac);
                bdmf_put(bridge);
            }
            else
                rc = BDMF_ERR_NOENT;
            bdmf_unlock();

            if (rc < 0)
                BDMF_TRACE_ERR("unable to change LAN MAC address, rc %d\n", rc);
        }
        break;
    default:
        break;
    }

    return 0;
}

static struct notifier_block nb =
{
    .notifier_call = br_fp_notifier_call,
    .priority = 0,
};

static struct br_fp_data gs_fp_hooks =
{
    .rdpa_hook = br_fp_rdpa_hook,
};

static int br_fp_init(void)
{
    struct net_device *dev;

    printk("Bridge fastpath module\n");

    if (register_netdevice_notifier(&nb))
    {
        printk(KERN_ERR "register_netdevice_notifier() failed(%d)", 0);
        return -1;
    }

    for_each_netdev(&init_net, dev)
    {
        if (dev->priv_flags & IFF_EBRIDGE)
            do_fdb_entries(dev, 1);
    }

    br_fp_set_callbacks(&gs_fp_hooks);

    return 0;
}

static void br_fp_cleanup(void)
{
    struct net_device *dev;

    br_fp_clear_callbacks();

    for_each_netdev(&init_net, dev)
    {
        if (dev->priv_flags & IFF_EBRIDGE)
            do_fdb_entries(dev, 0);
    }

    unregister_netdevice_notifier(&nb);
}

MODULE_LICENSE("GPL");
module_init(br_fp_init);
module_exit(br_fp_cleanup);

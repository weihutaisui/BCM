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
* File Name  : ptkrunner_host.c
*
* Description: Management of Host MAC Addresses
*
*******************************************************************************
*/

#include <linux/module.h>
#include <linux/bcm_log.h>
#include <linux/blog.h>
#include <linux/netdevice.h>
#include <linux/inetdevice.h>
#if defined(CONFIG_IPV6)
#include <net/ipv6.h>
#endif
#include <net/addrconf.h>
#include "fcachehw.h"

#include <rdpa_api.h>

#include "pktrunner_proto.h"

/*******************************************************************************
 *
 * Forward Declarations
 *
 *******************************************************************************/

#if defined(CONFIG_IPV6)
static int runnerUcast_inet6addr_event(struct notifier_block *this, unsigned long event, void *ptr);
#endif
static int runnerUcast_inetaddr_event(struct notifier_block *this, unsigned long event, void *ptr);

/*******************************************************************************
 *
 * Global Variables and Definitions
 *
 *******************************************************************************/

typedef enum {
    PKTRUNNER_OK                                        = 0,
    PKTRUNNER_ERROR_NOENT                               = 1,
    PKTRUNNER_ERROR_NORES                               = 2,
    PKTRUNNER_ERROR_ENTRY_EXISTS                        = 3,
    PKTRUNNER_ERROR_HOST_MAC_INVALID                    = 4,
    PKTRUNNER_ERROR_HOST_MAC_ADDR_TABLE_INDEX_INVALID   = 5,
    PKTRUNNER_ERROR_HOST_DEV_TABLE_INDEX_INVALID        = 6,
    PKTRUNNER_ERROR_MAX
} PKTRUNNER_ERROR_t;

extern int ucast_class_created_here;
extern bdmf_object_handle ucast_class;

#if defined(CONFIG_IPV6)
static struct notifier_block runnerUcast_inet6addr_notifier = {
    .notifier_call = runnerUcast_inet6addr_event,
};
#endif

static struct notifier_block runnerUcast_inetaddr_notifier = {
    .notifier_call = runnerUcast_inetaddr_event,
};

static int runnerUcast_netdev_event(struct notifier_block *this, unsigned long event, void *ptr);

static struct notifier_block runnerUcast_netdev_notifier = {
    .notifier_call = runnerUcast_netdev_event,
};

/* Device MAC address */
typedef struct
{
    bdmf_mac_t mac_addr;
    uint16_t   ref_count;
} host_mac_addr_table_t;

/* Device, and index to MAC address in host_mac_addr_table */
typedef struct
{
    struct net_device *dev_p;
    uint16_t   mac_index;
    uint16_t   ref_count;
} host_dev_table_t;

#define UCAST_HOST_DEV_TABLE_SIZE   16
#define UCAST_HOST_MAC_ADDR_TABLE_SIZE  RDPA_UCAST_HOST_MAC_ADDRESS_TABLE_SIZE
/*missing-braces warning is enabled, so can't use {0} initializer. Relying on BSS zero init rule instead.*/
static host_mac_addr_table_t g_host_mac_addr_table[UCAST_HOST_MAC_ADDR_TABLE_SIZE];
static host_dev_table_t g_host_dev_table[UCAST_HOST_DEV_TABLE_SIZE];

/*******************************************************************************
 *
 * Local Functions
 *
 *******************************************************************************/

static unsigned char *__getDevAddr(struct net_device *dev_p)
{
   return &dev_p->dev_addr[0];
}

static char *__getDevName(struct net_device *dev_p)
{
   return dev_p->name;
}

#if defined(CONFIG_IPV6)
static int runnerUcast_inet6addr_event(struct notifier_block *this, unsigned long event, void *ptr)
{
#if !defined(CONFIG_BCM963158)
    struct inet6_ifaddr *ifa = (struct inet6_ifaddr *)ptr;
    int ret;
    bdmf_index index=0;
    rdpa_ipv6_host_address_table_t ipv6_host_entry;

    memcpy(&ipv6_host_entry.ipv6_host_address, &ifa->addr, 16);
    ipv6_host_entry.reference_count = 0; /*Reference Count is managed by RDPA layer*/

    switch (event) {
    case NETDEV_UP:
        __logInfo("Adding IPv6 host address %pI6", &ifa->addr);
        ret = rdpa_ucast_ipv6_host_address_table_add(ucast_class, &index, &ipv6_host_entry);
        if(ret != 0)
        {
            __logError("Could not rdpa_ucast_ipv6_host_address_table_add");
        }

        break;
    case NETDEV_DOWN:
        __logInfo("Removing IPv6 host address %pI6", &ifa->addr);
        ret = rdpa_ucast_ipv6_host_address_table_find(ucast_class, &index, &ipv6_host_entry);
        if(ret != 0)
        {
            __logError("Could not rdpa_ucast_ipv6_host_address_table_find");
        }
        else
        {
            ret = rdpa_ucast_ipv6_host_address_table_delete(ucast_class, index);
            if(ret != 0)
            {
                __logError("Could not rdpa_ucast_ipv6_host_address_table_delete");
            }
        }

        break;
    }
#endif /* !defined(CONFIG_BCM963158) */

    return NOTIFY_DONE;
}
#endif

static int runnerUcast_inetaddr_event(struct notifier_block *this, unsigned long event, void *ptr)
{
#if !defined(CONFIG_BCM963158)

    struct in_ifaddr *ifa = (struct in_ifaddr *)ptr;
    int ret;
    bdmf_index index=0;
    rdpa_ipv4_host_address_table_t ipv4_host_entry;

    ipv4_host_entry.ipv4_host_address = (bdmf_ipv4)be32_to_cpu(ifa->ifa_local);
    ipv4_host_entry.reference_count = 0; /*Reference Count is managed by RDPA layer*/

    switch (event) {
    case NETDEV_UP:
        __logInfo("Adding IPv4 host address %pI4", &ifa->ifa_local);
        ret = rdpa_ucast_ipv4_host_address_table_add(ucast_class, &index, &ipv4_host_entry);
        if(ret != 0)
        {
            __logError("Could not rdpa_ucast_ipv4_host_address_table_add");
        }
        break;

    case NETDEV_DOWN:
        __logInfo("Removing IPv4 host address %pI4", &ifa->ifa_local);
        ret = rdpa_ucast_ipv4_host_address_table_find(ucast_class, &index, &ipv4_host_entry);
        if(ret != 0)
        {
            __logError("Could not rdpa_ucast_ipv4_host_address_table_find");
        }
        else
        {
            ret = rdpa_ucast_ipv4_host_address_table_delete(ucast_class, index);
            if(ret != 0)
            {
                __logError("Could not rdpa_ucast_ipv4_host_address_table_delete");
            }
        }
        break;
    }
#endif /* !defined(CONFIG_BCM963158) */
    
    return NOTIFY_DONE;
}

static PKTRUNNER_ERROR_t ucast_host_mac_addr_table_set(uint32_t xi_table_index, const bdmf_mac_t *xi_host_mac_addr, uint16_t xi_ref_count)
{
    if (xi_table_index >= UCAST_HOST_MAC_ADDR_TABLE_SIZE)
    {
        return PKTRUNNER_ERROR_HOST_MAC_ADDR_TABLE_INDEX_INVALID;
    }

    /*Store MAC address in a local table so we can return in the get accessor*/
    g_host_mac_addr_table[xi_table_index].mac_addr = *xi_host_mac_addr;
    g_host_mac_addr_table[xi_table_index].ref_count = xi_ref_count;

    return PKTRUNNER_OK;
}

static PKTRUNNER_ERROR_t ucast_host_mac_addr_table_get(uint32_t xi_table_index, bdmf_mac_t *xo_host_mac_addr, uint16_t *xo_ref_count)
{
    if (xi_table_index >= UCAST_HOST_MAC_ADDR_TABLE_SIZE)
    {
        return PKTRUNNER_ERROR_HOST_MAC_ADDR_TABLE_INDEX_INVALID;
    }

    /*Look up address in local table.*/
    *xo_host_mac_addr = g_host_mac_addr_table[xi_table_index].mac_addr;
    *xo_ref_count = g_host_mac_addr_table[xi_table_index].ref_count;

    return PKTRUNNER_OK;
}

static PKTRUNNER_ERROR_t ucast_host_mac_addr_table_find(uint16_t *index_p, void *val_p)
{
    host_mac_addr_table_t *entry = (host_mac_addr_table_t *)val_p;
    uint32_t ii;

    /*Search for entry with matching address and greater than zero reference count*/
    for (ii = 0; ii < UCAST_HOST_MAC_ADDR_TABLE_SIZE; ++ii)
    {
        bdmf_mac_t test_val;
        uint16_t ref_count;
        int res = ucast_host_mac_addr_table_get(ii, &test_val, &ref_count);
        assert(!res);

        if ((!memcmp(&test_val.b[0], &entry->mac_addr.b[0], sizeof(bdmf_mac_t))) && (ref_count > 0))
        {
            /*Entry found. Set the index to return */
            *index_p = ii;

            return PKTRUNNER_OK;
        }
    }

    /*Not found*/
    return PKTRUNNER_ERROR_NOENT;
}

static PKTRUNNER_ERROR_t ucast_host_mac_addr_table_add(uint16_t *index_p, const void *val_p)
{
    host_mac_addr_table_t *entry = (host_mac_addr_table_t *)val_p;
    uint32_t ii;
    bdmf_mac_t dummy;
    uint16_t ref_count;
    int res;

    /*Search for entry. Bump reference count if found.*/
    if (!ucast_host_mac_addr_table_find(index_p, (void *)val_p))
    {
        res = ucast_host_mac_addr_table_get(*index_p, &dummy, &ref_count);
        assert(!res);
        ucast_host_mac_addr_table_set(*index_p, &entry->mac_addr, ++ref_count);
        return PKTRUNNER_OK;
    }

    /*Find a free entry. Reference count 0 means entry is free.*/
    for (ii = 0; ii < UCAST_HOST_MAC_ADDR_TABLE_SIZE; ++ii)
    {
        res = ucast_host_mac_addr_table_get(ii, &dummy, &ref_count);
        assert(!res);

        if (ref_count == 0)
        {
            /*Free entry found. Use it*/
            ucast_host_mac_addr_table_set(ii, &entry->mac_addr, 1 /*reference count*/);
            /* set the index to return */
            *index_p = ii;
            return PKTRUNNER_OK;
        }
    }

    /*No free entries available*/
    return PKTRUNNER_ERROR_NORES;
}

static PKTRUNNER_ERROR_t ucast_host_mac_addr_table_delete(uint16_t index)
{
    int res;
    bdmf_mac_t host_mac_addr;
    uint16_t ref_count;

    /*Decrement reference count at index if not zero.*/
    res = ucast_host_mac_addr_table_get(index, &host_mac_addr, &ref_count);
    assert(!res);

    if (ref_count > 0)
    {
        ucast_host_mac_addr_table_set(index, &host_mac_addr, --ref_count);
        return PKTRUNNER_OK;
    }
    else
    {
        return PKTRUNNER_ERROR_NOENT;
    }
}

#if 0
static void ucast_host_mac_addr_table_print(void)
{
    uint32_t ii;
    bcm_printk("\nidx         MAC         mac_ref\n");

    /*Find a free entry. Reference count 0 means entry is free.*/
    for (ii = 0; ii < UCAST_HOST_MAC_ADDR_TABLE_SIZE; ++ii)
    {
        /*Look up dev in local table.*/
        bcm_printk("%3u <%02x:%2x:%02x:%02x:%02x:%02x> %7d\n", ii, 
            g_host_mac_addr_table[ii].mac_addr.b[0],
            g_host_mac_addr_table[ii].mac_addr.b[1],
            g_host_mac_addr_table[ii].mac_addr.b[2],
            g_host_mac_addr_table[ii].mac_addr.b[3],
            g_host_mac_addr_table[ii].mac_addr.b[4],
            g_host_mac_addr_table[ii].mac_addr.b[5],
            g_host_mac_addr_table[ii].ref_count);
    }
}
#endif

static PKTRUNNER_ERROR_t ucast_host_dev_table_set(uint32_t xi_table_index, struct net_device *xi_dev_p, uint16_t xi_mac_index, uint16_t xi_ref_count)
{
    if (xi_table_index >= UCAST_HOST_DEV_TABLE_SIZE)
    {
        return PKTRUNNER_ERROR_HOST_DEV_TABLE_INDEX_INVALID;
    }

    /*Store MAC address in a local table so we can return in the get accessor*/
    g_host_dev_table[xi_table_index].dev_p = xi_dev_p;
    g_host_dev_table[xi_table_index].mac_index = xi_mac_index;
    g_host_dev_table[xi_table_index].ref_count = xi_ref_count;

    return PKTRUNNER_OK;
}

static PKTRUNNER_ERROR_t ucast_host_dev_table_get(uint32_t xi_table_index, struct net_device **xo_dev_pp, uint16_t *xo_mac_index, uint16_t *xo_ref_count)
{
    if (xi_table_index >= UCAST_HOST_DEV_TABLE_SIZE)
    {
        return PKTRUNNER_ERROR_HOST_DEV_TABLE_INDEX_INVALID;
    }

    /*Look up dev in local table.*/
    *xo_dev_pp = g_host_dev_table[xi_table_index].dev_p;
    *xo_mac_index = g_host_dev_table[xi_table_index].mac_index;
    *xo_ref_count = g_host_dev_table[xi_table_index].ref_count;

    return PKTRUNNER_OK;
}

static PKTRUNNER_ERROR_t ucast_host_dev_table_find(uint16_t *index_p, void *val_p)
{
    host_dev_table_t *entry = (host_dev_table_t *)val_p;
    uint32_t ii;

    /*Search for entry with matching dev and greater than zero reference count*/
    for (ii = 0; ii < UCAST_HOST_DEV_TABLE_SIZE; ++ii)
    {
        struct net_device *test_val;
        uint16_t ref_count;
        uint16_t mac_index;
        int res = ucast_host_dev_table_get(ii, &test_val, &mac_index, &ref_count);
        assert(!res);

        if ((test_val == entry->dev_p) && (ref_count > 0))
        {
            /*Entry found. Set the index to return */
            *index_p = ii;

            return PKTRUNNER_OK;
        }
    }

    /*Not found*/
    return PKTRUNNER_ERROR_NOENT;
}

static PKTRUNNER_ERROR_t ucast_host_dev_table_add(uint16_t *index_p, const void *val_p, uint16_t mac_index)
{
    host_dev_table_t *entry = (host_dev_table_t *)val_p;
    uint32_t ii;
    struct net_device *dummy_dev_p;
    uint16_t ref_count;
    uint16_t tmp_mac_index;
    int res;

    /*Search for entry. Bump reference count if found.*/
    if (!ucast_host_dev_table_find(index_p, entry))
        return PKTRUNNER_ERROR_ENTRY_EXISTS;

    /*Find a free entry. Reference count 0 means entry is free.*/
    for (ii = 0; ii < UCAST_HOST_DEV_TABLE_SIZE; ++ii)
    {
        res = ucast_host_dev_table_get(ii, &dummy_dev_p, &tmp_mac_index, &ref_count);
        assert(!res);

        if (ref_count == 0)
        {
            /*Free entry found. Use it*/
            ucast_host_dev_table_set(ii, entry->dev_p, mac_index, 1 /*reference count*/);
            /* set the index to return */
            *index_p = ii;
            return PKTRUNNER_OK;
        }
    }

    /*No free entries available*/
    return PKTRUNNER_ERROR_NORES;
}

static PKTRUNNER_ERROR_t ucast_host_dev_table_delete(uint16_t index)
{
    int res;
    struct net_device *dummy_dev_p;
    uint16_t ref_count;
    uint16_t mac_index;

    /*Decrement reference count at index if not zero.*/
    res = ucast_host_dev_table_get(index, &dummy_dev_p, &mac_index, &ref_count);
    assert(!res);

    if (ref_count > 0)
    {
        ucast_host_dev_table_set(index, dummy_dev_p, mac_index, --ref_count);
        return PKTRUNNER_OK;
    }
    else
        return PKTRUNNER_ERROR_NOENT;
}

#if 0
static void ucast_host_dev_table_print(void)
{
    uint32_t ii;
    bcm_printk("\n\nidx           device      dev_p dev_ref mac_idx\n");

    /*Find a free entry. Reference count 0 means entry is free.*/
    for (ii = 0; ii < UCAST_HOST_DEV_TABLE_SIZE; ++ii)
    {
        /*Look up dev in local table.*/
        bcm_printk("%3u %16s 0x%p %7d %7d\n", ii, 
            __getDevName(((struct net_device *) g_host_dev_table[ii].dev_p),
            g_host_dev_table[ii].dev_p,
            g_host_dev_table[ii].ref_count,
            g_host_dev_table[ii].mac_index);
    }
}
#endif

static PKTRUNNER_ERROR_t ucast_host_dev_mac_table_add(const void *val_p)
{
    struct net_device *dev_p = (struct net_device *)val_p;
    uint16_t dev_index;
    uint16_t mac_index;
    int ret;
    bdmf_mac_t zero_mac = {};
    host_mac_addr_table_t host_mac_addr_entry;
    host_dev_table_t host_dev_entry;

    if (!memcmp(__getDevAddr(dev_p), &zero_mac.b[0], sizeof(bdmf_mac_t)))
        return PKTRUNNER_ERROR_HOST_MAC_INVALID;

    memcpy(&host_mac_addr_entry.mac_addr.b[0], __getDevAddr(dev_p), ETH_ALEN);
    host_mac_addr_entry.ref_count = 0;

    /* Device entry already exists? If yes, no need to add new device or mac entry. */
    ret = ucast_host_dev_table_find(&dev_index, &host_dev_entry);
    if (ret == PKTRUNNER_OK)
        return ret;

    ret = ucast_host_mac_addr_table_add(&mac_index, &host_mac_addr_entry);
    if (ret != PKTRUNNER_OK)
        return ret;

    host_dev_entry.dev_p = dev_p;
    host_dev_entry.mac_index = 0xFFFF;
    host_dev_entry.ref_count = 0;

    /* Add new device. If add fails, then delete the MAC entry. */
    ret = ucast_host_dev_table_add(&dev_index, &host_dev_entry, mac_index);
    if (ret != PKTRUNNER_OK)
        ucast_host_mac_addr_table_delete(mac_index);

    return ret;
}

static PKTRUNNER_ERROR_t ucast_host_dev_mac_table_delete(const void *val_p)
{
    int ret;
    struct net_device *dev_p = (struct net_device *)val_p;
    uint16_t ref_count;
    uint16_t dev_index;
    uint16_t mac_index;
    host_dev_table_t host_dev_entry;

    host_dev_entry.dev_p = dev_p;
    host_dev_entry.mac_index = 0xFFFF;
    host_dev_entry.ref_count = 0;

    /* Search for entry */
    ret = ucast_host_dev_table_find(&dev_index, &host_dev_entry);
    if (ret != PKTRUNNER_OK)
        return ret;

    /* Get mac_index for this device */
    ret = ucast_host_dev_table_get(dev_index, &dev_p, &mac_index, &ref_count);
    if (ret != PKTRUNNER_OK)
        return ret;

    if (ref_count == 0)
        return PKTRUNNER_ERROR_NOENT;
    else
        ret = ucast_host_dev_table_delete(dev_index);

    ret = ucast_host_mac_addr_table_delete(mac_index);
    return ret;
}

static int runnerUcast_netdev_event_add_host_mac(struct net_device *dev_p, bdmf_index *index_p, rdpa_host_mac_address_table_t *host_mac_entry_p)
{
    int ret;
    unsigned char *dev_addr;

    dev_addr = __getDevAddr(dev_p);
    __logInfo("dev=%s : Adding host MAC <0x%02x:%02x:%02x:%02x:%02x:%02x>\n", 
            __getDevName(dev_p),
            dev_addr[0], dev_addr[1], dev_addr[2],
            dev_addr[3], dev_addr[4], dev_addr[5]);

    ret = ucast_host_dev_mac_table_add(dev_p);
    if (ret != PKTRUNNER_OK)
        return ret;

    ret = rdpa_ucast_host_mac_address_table_add(ucast_class, index_p, host_mac_entry_p);
    if(ret != 0)
    {
        __logError("Could not rdpa_ucast_host_mac_address_table_add\n");
        ret = ucast_host_dev_mac_table_delete(dev_p);
    }

    return ret;
}

static int runnerUcast_netdev_event_delete_host_mac(struct net_device *dev_p, bdmf_index *index_p)
{
    int ret;
    struct net_device *dummy_dev_p;
    uint16_t ref_count;
    uint16_t dev_index;
    uint16_t mac_index = 0xFFFF;
    unsigned char *dev_addr;
    host_dev_table_t host_dev_entry;
    rdpa_host_mac_address_table_t host_mac_entry;

    host_dev_entry.dev_p = dev_p;
    host_dev_entry.mac_index = 0xFFFF;
    host_dev_entry.ref_count = 0;

    /* Search for entry */
    ret = ucast_host_dev_table_find(&dev_index, &host_dev_entry);
    if (ret != PKTRUNNER_OK)
        return ret;

    ret = ucast_host_dev_table_get(dev_index, &dummy_dev_p, &mac_index, &ref_count);
    if (ret != PKTRUNNER_OK)
        return ret;

    if (mac_index == 0xFFFF)
    {
        __logError("Invalid mac_index =%d\n", mac_index);
        return PKTRUNNER_ERROR_NOENT;
    }

    host_mac_entry.reference_count = 0; /*Reference Count is managed by RDPA layer*/
    ret = ucast_host_mac_addr_table_get(mac_index, &host_mac_entry.host_mac_address, &ref_count);
    if (ret != PKTRUNNER_OK)
        return ret;

    dev_addr = __getDevAddr(dev_p);
    __logInfo("dev=%s : Removing host MAC <0x%02x:%02x:%02x:%02x:%02x:%02x>\n", 
            __getDevName(dev_p), 
            dev_addr[0], dev_addr[1], dev_addr[2],
            dev_addr[3], dev_addr[4], dev_addr[5]);

    ret = rdpa_ucast_host_mac_address_table_find(ucast_class, index_p, &host_mac_entry);
    if(ret != 0)
    {
        __logError("Could not rdpa_ucast_host_mac_address_table_find\n");
    }
    else
    {
        ret = rdpa_ucast_host_mac_address_table_delete(ucast_class, *index_p);
        if(ret != 0)
        {
            __logError("Could not rdpa_ucast_host_mac_address_table_delete\n");
        }
    }

    ret = ucast_host_dev_mac_table_delete(dev_p);

    return ret;
}

static int runnerUcast_netdev_event(struct notifier_block *this, unsigned long event, void *ptr)
{
    int ret;
    struct net_device *dev_p = NETDEV_NOTIFIER_GET_DEV(ptr);
    bdmf_index index=0;
    rdpa_host_mac_address_table_t host_mac_entry;

    __logDebug("\n\ndev=%s event=%lu\n", __getDevName(dev_p), event);

    if (!blog_is_config_netdev_mac(dev_p, 0))
        return NOTIFY_DONE;

    memcpy(&host_mac_entry.host_mac_address.b[0], __getDevAddr(dev_p), ETH_ALEN);
    host_mac_entry.reference_count = 0; /*Reference Count is managed by RDPA layer*/

    switch (event) {
    case NETDEV_UP:
        __logInfo("Link UP 0x%p\n", dev_p);
        ret = runnerUcast_netdev_event_add_host_mac(dev_p, &index, &host_mac_entry);
        break;

    case NETDEV_CHANGEADDR:
        __logInfo("MAC Change %p\n", dev_p);
        ret = runnerUcast_netdev_event_delete_host_mac(dev_p, &index);
        ret = runnerUcast_netdev_event_add_host_mac(dev_p, &index, &host_mac_entry);
        break;

    case NETDEV_DOWN:
        __logInfo("Link DN 0x%p\n", dev_p);
        ret = runnerUcast_netdev_event_delete_host_mac(dev_p, &index);
        break;

    case NETDEV_GOING_DOWN:
        __logInfo("Link Going DN 0x%p\n", dev_p);
        ret = runnerUcast_netdev_event_delete_host_mac(dev_p, &index);
        break;

    case NETDEV_CHANGE:
        if (netif_carrier_ok(dev_p))
        {
            __logInfo("Link Change UP 0x%p\n", dev_p);
            ret = runnerUcast_netdev_event_add_host_mac(dev_p, &index, &host_mac_entry);
        }
        else
        {
            __logInfo("Link Change DN 0x%p\n", dev_p);
            ret = runnerUcast_netdev_event_delete_host_mac(dev_p, &index);
        }
        break;
    }

    return NOTIFY_DONE;
}

static int runnerUcast_fc_accel_mode_set( uint32_t accel_mode )
{
    int ret;
    rdpa_fc_accel_mode_t    rdpa_fc_accel_mode;

    rdpa_fc_accel_mode.fc_accel_mode = accel_mode;

    ret = rdpa_ucast_fc_accel_mode_table_set(ucast_class, &rdpa_fc_accel_mode);

    return ret;
}

int __init runnerHost_construct(void)
{
    register_inetaddr_notifier(&runnerUcast_inetaddr_notifier);
#if defined(CONFIG_IPV6)
    register_inet6addr_notifier(&runnerUcast_inet6addr_notifier);
#endif
    register_netdevice_notifier(&runnerUcast_netdev_notifier);

    /* bind to acceleration mode function hook used by blog/flow_cache */
    blog_accel_mode_set_fn = (blog_accel_mode_set_t) runnerUcast_fc_accel_mode_set;

    /* Set the Runner acceleration mode to be in sync with blog/flow cache */
    runnerUcast_fc_accel_mode_set( blog_support_get_accel_mode() );

    __print("Initialized Runner Host Layer\n");

    return 0;
}

void __exit runnerHost_destruct(void)
{
    unregister_netdevice_notifier(&runnerUcast_netdev_notifier);
    unregister_inetaddr_notifier(&runnerUcast_inetaddr_notifier);
#if defined(CONFIG_IPV6)
    unregister_inet6addr_notifier(&runnerUcast_inet6addr_notifier);
#endif

    blog_accel_mode_set_fn = (blog_accel_mode_set_t) NULL;
}

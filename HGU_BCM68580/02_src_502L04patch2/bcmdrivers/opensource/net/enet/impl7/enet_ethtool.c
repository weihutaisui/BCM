/*
   <:copyright-BRCM:2017:DUAL/GPL:standard
   
      Copyright (c) 2017 Broadcom 
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
 */

#include <linux/netdevice.h>
#include <linux/ethtool.h>
#include "bcmenet_ethtool.h"
#include "enet.h"
#include "port.h"

#ifdef RUNNER
#include "rdpa_api.h"
#endif
#ifdef CONFIG_BCM_PTP_1588
#include "ptp_1588.h"
#endif

static void enet_get_ethtool_stats(struct net_device *dev, struct ethtool_stats *stats, u64 *data)
{
    const struct rtnl_link_stats64 *ethStats;
    struct rtnl_link_stats64 temp;
    enetx_port_t *port = ((enetx_netdev *)netdev_priv(dev))->port;
    phy_dev_t *phy_dev = port->p.phy;
    u64 speed = 0;

    ethStats = dev_get_stats(dev, &temp);
    
    data[ET_TX_BYTES] =     ethStats->tx_bytes;
    data[ET_TX_PACKETS] =   ethStats->tx_packets;
    data[ET_TX_ERRORS] =    ethStats->tx_errors;
    data[ET_RX_BYTES] =     ethStats->rx_bytes;
    data[ET_RX_PACKETS] =   ethStats->rx_packets;
    data[ET_RX_ERRORS] =    ethStats->rx_errors;

    /* Note: capacity is in bytes per second */
    if (phy_dev && phy_dev->link)
    {
        switch (phy_dev->speed)
        {
        case PHY_SPEED_10:      speed =    10000000UL;  break;
        case PHY_SPEED_100:     speed =   100000000UL;  break;
        case PHY_SPEED_1000:    speed =  1000000000UL;  break;
        case PHY_SPEED_2500:    speed =  2500000000UL;  break;
        case PHY_SPEED_5000:    speed =  5000000000UL;  break;
        case PHY_SPEED_10000:   speed = 10000000000UL;  break;
        default:                speed = 0;
        }
    }
    data[ET_TX_CAPACITY] = speed / 8;
}


static int enet_ethtool_get_settings(struct net_device *dev, struct ethtool_cmd *ecmd)
{
    enetx_port_t *port = ((enetx_netdev *)netdev_priv(dev))->port;
    phy_dev_t *phy_dev = port->p.phy;

    if (!phy_dev || !phy_dev->link)
        return -1;

    switch (phy_dev->speed)
    {
    case PHY_SPEED_10:      ecmd->speed = SPEED_10;     break;
    case PHY_SPEED_100:     ecmd->speed = SPEED_100;    break;
    case PHY_SPEED_1000:    ecmd->speed = SPEED_1000;   break;
    case PHY_SPEED_2500:    ecmd->speed = SPEED_2500;   break;
    case PHY_SPEED_5000:    ecmd->speed = 5000;         break;
    case PHY_SPEED_10000:   ecmd->speed = SPEED_10000;  break;
    default:                return -1;
    }

    switch (phy_dev->duplex)
    {
    case PHY_DUPLEX_FULL:   ecmd->duplex = DUPLEX_FULL; break;
    case PHY_DUPLEX_HALF:   ecmd->duplex = DUPLEX_HALF; break;
    default:                ecmd->duplex = DUPLEX_UNKNOWN;
    }
    return 0;
}

const struct ethtool_ops enet_ethtool_ops =
{
    .get_settings =         enet_ethtool_get_settings,
    .get_ethtool_stats =    enet_get_ethtool_stats,
#ifdef CONFIG_BCM_PTP_1588
    .get_ts_info = ptp_1588_get_ts_info,
#endif
};



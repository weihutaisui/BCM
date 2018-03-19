/*
   Copyright (c) 2015 Broadcom Corporation
   All Rights Reserved

    <:label-BRCM:2015:DUAL/GPL:standard

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
 *  Created on: Dec 2015
 *      Author: yuval.raviv@broadcom.com
 */

#include "bp_parsing.h"
#include "enet_dbg.h"
#include "phy_drv.h"
#include "mac_drv.h"
#include "phy_bp_parsing.h"
#include "boardparms.h"
#include "drivers/bcm_chip_arch.c"
#include <crossbar_dev.h>

#define SWITCH_UNIT_ROOT 0
#define SWITCH_UNIT_EXTERNAL 1

static int __init bp_parse_port(enetx_port_t *sw, const ETHERNET_MAC_INFO *emac_info, uint32_t port, uint32_t unit);

extern int unit_port_oam_idx_array[BP_MAX_ENET_MACS][COMPAT_MAX_SWITCH_PORTS];
static int __init bp_parse_port_cap(const ETHERNET_MAC_INFO *emac_info, uint32_t unit, uint32_t port, enetx_port_t *p)
{
    int port_flags = emac_info->sw.port_flags[port];
    int port_cap = -1;
    static char *cap2string[PORT_CAP_MAX] = 
                    {[PORT_CAP_NONE]="", 
                     [PORT_CAP_MGMT]="", 
                     [PORT_CAP_LAN_WAN]="LAN/WAN", 
                     [PORT_CAP_LAN_ONLY]="LAN Only", 
                     [PORT_CAP_WAN_ONLY]="WAN Only", 
                     [PORT_CAP_WAN_PREFERRED]="WAN Preferred"};

    if (port_flags & PORT_FLAG_MGMT)
        return PORT_CAP_MGMT;

    switch (port_flags & PORT_FLAG_LANWAN_M)
    {
        case PORT_FLAG_WAN_ONLY:
            if (chip_arch_lan_only_portmap[unit] & (1<<port))
                enet_err("ERROR: ****** Conflict WAN Only Defined on Switch %d, port %d:\n", unit, port);
            else
                port_cap = PORT_CAP_WAN_ONLY;
            break;
        case PORT_FLAG_WAN_PREFERRED:
            if ((chip_arch_lan_only_portmap[unit] & (1<<port)) ||
                    (chip_arch_wan_only_portmap[unit] & (1<<port)))
                enet_err("ERROR: ****** Conflict WAN Prefered Defined on Switch %d, port %d:\n", unit, port);
            else
                port_cap = PORT_CAP_WAN_PREFERRED;
            break;
        case PORT_FLAG_LAN_ONLY:
            if (chip_arch_lan_only_portmap[unit] & (1<<port))
                enet_err("ERROR: ****** Conflict LAN Only Defined on Switch %d, port %d:\n", unit, port);
            else
                port_cap = PORT_CAP_LAN_ONLY;
            break;
        default:
            break;
    }

    if (port_cap == -1)
    {
        if (chip_arch_lan_only_portmap[unit] & (1<<port))
            port_cap = PORT_CAP_LAN_ONLY;
        else if (chip_arch_wan_only_portmap[unit] & (1<<port))
            port_cap = PORT_CAP_WAN_ONLY;
        else if (chip_arch_wan_pref_portmap[unit] & (1<<port))
            port_cap = PORT_CAP_WAN_PREFERRED;
        else
            port_cap = PORT_CAP_LAN_WAN;
    }

    enet_dbg("Unit %d, Port %d, %s LAN/WAN Capability: %s\n", unit, port, p->dev->name, cap2string[port_cap]);
    return port_cap;
}

static enetx_port_t __init *bp_parse_attached_port(enetx_port_t *sw, int sid, const char *dev_name, uint32_t port)
{
    enetx_port_t *p;
    port_info_t port_info =
    {
        .port = sid,
    };

    if (!(p = port_create(&port_info, sw)))
    {
        enet_err("Failed to create g9991 port: port=%d sid=%d\n", port, sid);
        return NULL;
    }

    p->has_interface = 1;
    p->p.port_cap = PORT_CAP_LAN_ONLY;

    if (dev_name)
        strncpy(p->name, dev_name, IFNAMSIZ);
    else
        snprintf(p->name, IFNAMSIZ, "sid%d", sid);

    enet_dbg("Created g9991 port: name=%s port=%d sid=%d\n", dev_name, port, sid);

    /* Add translation arry - port-> unit,port */
    unit_port_array[0][sid] = p;
    enet_dbg("backward compat unit %d port %d -> %s\n", 0, sid, p->obj_name);
    /* keep backword compatible oam_idx mapping */
    unit_port_oam_idx_array[0][sid] = sid;
    enet_dbg("backward oam unit %d port %d -> %d\n", 0, sid, unit_port_oam_idx_array[0][sid]);

    return p;
}

static enetx_port_t *__init bp_create_sw(port_type_t type, char *name, enetx_port_t *parent_port)
{
    enetx_port_t *sw;

    if (!(sw = sw_create(type, parent_port)))
        return NULL;

    if (name)
    {
        sw->has_interface = 1;
        strncpy(sw->name, name, IFNAMSIZ);
    }

    return sw;
}

static int __init bp_parse_attached(enetx_port_t *parent_port, uint32_t port)
{
    int i;
    enetx_port_t *sw;
    BP_ATTACHED_INFO bp_attached_info;

    if (BpGetAttachedInfo(port, &bp_attached_info))
    {
        enet_err("Failed to get attached ports for port %d\n", port);
        return -1;
    }

    sw = bp_create_sw(PORT_TYPE_G9991_SW, NULL, parent_port);
    if (!sw)
        return -1;

    for (i = 0; i < BP_MAX_ATTACHED_PORTS; i++)
    {
        if (!(bp_attached_info.port_map & 1<<i))
            continue;

        if (!bp_parse_attached_port(sw, bp_attached_info.ports[i], bp_attached_info.devnames[i], port))
            return -1;
    }

    return 0;
}

static int __init bp_parse_unit(enetx_port_t *parent_port, int unit)
{
    ETHERNET_MAC_INFO *emac_info;
    port_type_t type;
    char *ifname = NULL;
    uint32_t port;
    enetx_port_t *sw;

    if ((emac_info = (ETHERNET_MAC_INFO *)BpGetEthernetMacInfoArrayPtr()) == NULL)
    {
        enet_err("Error reading Ethernet MAC info from board params\n");
        return -1;
    }

    switch (emac_info[unit].ucPhyType)
    {
        case BP_ENET_INTERNAL_PHY:
            type = PORT_TYPE_RUNNER_SW;
            ifname = "bcmsw";
            break;
        case BP_ENET_NO_PHY:
            /* note: DSL chips with runner use BP_ENET_NO_PHY as PhyType */
            type = PORT_TYPE_RUNNER_SW;
            ifname = "bcmsw";
            break;
        case BP_ENET_EXTERNAL_SWITCH:
#ifdef SF2_DEVICE
            /* DSL Star Fighter 2 external switch */
            type = PORT_TYPE_SF2_SW;
            break;
#endif
        case BP_ENET_SWITCH_VIA_INTERNAL_PHY:
            /* XXX No one uses this type */
        default:
            enet_err("cannot find sw object match for boardparms unit type %d\n", emac_info[unit].ucPhyType);
            return -1;
    }

    sw = bp_create_sw(type, ifname, parent_port);
    if (unit == SWITCH_UNIT_ROOT)
        root_sw = sw;

    if (!sw)
        return -1;

    for (port = 0; port < BP_MAX_SWITCH_PORTS; port++)
    {
        if (!(emac_info[unit].sw.port_map & (1<<port)))
            continue;

        if (bp_parse_port(sw, &emac_info[unit], port, unit))
            return -1;
    }

    return 0;
}

void bp_copy_port_info(const ETHERNET_MAC_INFO *emac_info, uint32_t port, EMAC_PORT_INFO *port_info)
{
    port_info->switch_port  = port;
    port_info->phy_id       = emac_info->sw.phy_id[port];
    port_info->phy_id_ext   = emac_info->sw.phy_id_ext[port];
    port_info->phyconn      = emac_info->sw.phyconn[port];
    port_info->phy_devName  = emac_info->sw.phy_devName[port];
    memcpy(&(port_info->ledInfo), &(emac_info->sw.ledInfo[port]), sizeof(port_info->ledInfo));
    port_info->phyinit      = emac_info->sw.phyinit[port];
    port_info->port_flags   = emac_info->sw.port_flags[port];
    port_info->phyReset     = emac_info->sw.phyReset[port];
    port_info->oamIndex     = emac_info->sw.oamIndex[port];
    port_info->portMaxRate  = emac_info->sw.portMaxRate[port];
}

int bp_parse_crossbar_phy_devs(ETHERNET_MAC_INFO *emac_info, uint32_t unit, uint32_t port, enetx_port_t *p, phy_dev_t *phy_dev_crossbar)
{
    int i;
    phy_dev_t *phy_dev = NULL;
    
    for (i = BP_MAX_CROSSBAR_EXT_PORTS-1; i >= 0; i--)
    {
        EMAC_PORT_INFO port_info;
        
        if (emac_info->sw.crossbar[i].switch_port != port)
            continue;

        /* copy crossbar info to port */
        port_info = emac_info->sw.crossbar[i];

        if ((phy_dev = bp_parse_phy_dev(&port_info)) == NULL)
        {
            enet_err("Failed to create crossbar phy devices: unit=%d port=%d cbe=%d\n", unit, port, i);
            return -1;
        }
        
        crossbar_phy_add(phy_dev_crossbar, phy_dev, i /* external_endpoint */);
        bp_parse_cascade_phy(&port_info, phy_dev);

        if (phy_dev->cascade_next)
            enet_dbgv("cascaded phyId=%x (%s) %pK",phy_dev->cascade_next->addr, phy_dev->cascade_next->phy_drv->name, phy_dev->cascade_next);
    }

    return 0;
}

static int __init bp_parse_port(enetx_port_t *sw, const ETHERNET_MAC_INFO *emac_info, uint32_t port, uint32_t unit)
{
    enetx_port_t *p;
    int port_flags = emac_info->sw.port_flags[port];
#if !defined(DSL_RUNNER_DEVICE)
    int is_wan_only = port_flags & PORT_FLAG_WAN_ONLY ? 1 : 0;
#endif
    int is_attached = port_flags & PORT_FLAG_ATTACHED ? 1 : 0;
    char *dev_name = emac_info->sw.phy_devName[port];
    int ext_switch = IsPortConnectedToExternalSwitch(emac_info->sw.phy_id[port]);
    int is_management = port_flags & PORT_FLAG_MGMT ? 1 : 0;
    int oam_idx = emac_info->sw.oamIndex[port];
    port_info_t port_info =
    {
        .port = port,
        .is_management = is_management,
        .is_attached = is_attached,
#if !defined(DSL_RUNNER_DEVICE)
        .is_detect = is_wan_only, /* Not equivalent in DSL platform. XXX: Should add detect flag to boardparms */
#endif
    };

    oam_idx = oam_idx; /* Satisfy compiler warning */

    p = port_create(&port_info, sw);

    // port in board param is not being used, move to next
    if (port_info.is_undef)
        return 0;

    if (!p)
    {
        enet_err("Failed to create unit %d port %d\n", unit, port);
        return -1;
    }

    {
        EMAC_PORT_INFO port_info = {};
        int cb = emac_info->sw.phy_id[port]; // workaround for testing
        
        bp_copy_port_info(emac_info, port, &port_info);

        p->p.phy = bp_parse_phy_dev(&port_info);
        if (p->p.phy)
            bp_parse_cascade_phy(&port_info, p->p.phy);
        else if (port_info.phy_id == BP_PHY_ID_NOT_SPECIFIED)
        {
            p->p.phy = crossbar_phy_dev_get(unit, port);
            if (!p->p.phy)
            {
                enet_err("Failed to allocate new crossbar object for unit %d port %d\n", unit, port);
                return -1;
            }

            if (bp_parse_crossbar_phy_devs((ETHERNET_MAC_INFO *)emac_info, unit, port, p, p->p.phy))
                return -1;
        }

        if (p->p.phy) p->p.phy->sw_port = p;
        if (p->p.phy) enet_dbgv("bp_parse_phy_dev: phyId=%x (%s) %pK\n", p->p.phy->addr, p->p.phy->phy_drv->name, p->p.phy);
        
        p->p.mac = bp_parse_mac_dev(emac_info, port); 
        if (p->p.mac) enet_dbgv("bp_parse_mac_dev: port %pK macId=%x (%s) %pK\n", p, p->p.mac->mac_id, p->p.mac->mac_drv->name, p->p.mac);

        ((ETHERNET_MAC_INFO *)emac_info)->sw.phy_id[port] = cb;
    }

    if (is_attached)
    {
        if (bp_parse_attached(p, port))
        {
            enet_err("Failed to parse attached ports for port %d\n", port);
            return -1;
        }
    }
    else if (ext_switch)
    {
        if (bp_parse_unit(p, SWITCH_UNIT_EXTERNAL))
        {
            enet_err("Failed to parse external switch ports for unit %d port %d\n", unit, port);
            return -1;
        }
    }
    else
    {
        p->p.port_cap = bp_parse_port_cap(emac_info, unit, port, p);
#ifdef VLANTAG /* vlan tag port example: attach to physical port */
        {
            port_info_t port_info =
            {
                .port = 0, // vlan tag
            };

            sw = sw_create(PORT_TYPE_VLAN_SW, p);
            if (!sw || !(p = port_create(&port_info, sw))) /* overwrites *p, still used below */
            {
                enet_err("Failed to vlan sw/port for unit %d port %d\n", unit, port);
                return -1;
            }
        }
#endif

        p->has_interface = 1;
        /* Create all ports as LAN by default */
#if !defined(DSL_RUNNER_DEVICE) /* The PORT_FLAG_WAN_ONLY flag is designed as CAP not ROLE. This is miss used */
        if (!is_wan_only)
            p->n.port_netdev_role = PORT_NETDEV_ROLE_LAN;
        else
            p->n.port_netdev_role = PORT_NETDEV_ROLE_WAN;
#endif

        if (dev_name)
            strncpy(p->name, dev_name, IFNAMSIZ);
    }
    
#ifndef BRCM_FTTDP
    /* Add translation arry - port-> unit,port */
    unit_port_array[unit][port] = p;
    enet_dbg("backward compat unit %d port %d -> %s\n", unit, port, p->obj_name);
    /* keep backword compatible oam_idx mapping */
    unit_port_oam_idx_array[unit][port] = (oam_idx >= 0) ? oam_idx : (unit == 0) ? port : -1;
    enet_dbg("backward oam unit %d port %d -> %d\n", unit, port, unit_port_oam_idx_array[unit][port]);
#endif

    return 0;
}

int __init bp_parse(void)
{
    char boardIdStr[20];

    BpGetBoardId(boardIdStr);
    enet_dbg("Parsing board configuration for %s\n", boardIdStr);

    return bp_parse_unit(NULL, SWITCH_UNIT_ROOT);
}


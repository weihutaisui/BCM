/*
* <:copyright-BRCM:2013-2015:proprietary:standard
*
*    Copyright (c) 2013-2015 Broadcom
*    All Rights Reserved
*
*  This program is the proprietary software of Broadcom and/or its
*  licensors, and may only be used, duplicated, modified or distributed pursuant
*  to the terms and conditions of a separate, written license agreement executed
*  between you and Broadcom (an "Authorized License").  Except as set forth in
*  an Authorized License, Broadcom grants no license (express or implied), right
*  to use, or waiver of any kind with respect to the Software, and Broadcom
*  expressly reserves all rights in and to the Software and all intellectual
*  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
*  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
*  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
*
*  Except as expressly set forth in the Authorized License,
*
*  1. This program, including its structure, sequence and organization,
*     constitutes the valuable trade secrets of Broadcom, and you shall use
*     all reasonable efforts to protect the confidentiality thereof, and to
*     use this information only in connection with your use of Broadcom
*     integrated circuit products.
*
*  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
*     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
*     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
*     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
*     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
*     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
*     PERFORMANCE OF THE SOFTWARE.
*
*  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
*     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
*     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
*     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
*     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
*     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
*     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
*     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
*     LIMITED REMEDY.
 :>
*/

/*
 * rdpa_port.c
 *
 *  Created on: Aug 23, 2012
 *  Author: igort
 */


#include <bdmf_dev.h>
#include "rdpa_api.h"
#include "rdpa_int.h"
#if defined(__OREN__)
#include "rdpa_ingress_class_int.h"
#endif
#include "rdpa_rdd_inline.h"
#include "rdd.h"
#if !defined(LEGACY_RDP) && !defined(XRDP)
#include "rdd_multicast_processing.h"
#endif
#include "rdpa_port_int.h"
#ifdef XRDP
#include "rdd_stubs.h"
#include "rdp_drv_rnr.h"
#include "rdpa_rdd_map.h"
#include "rdd_runner_proj_defs.h"
#include "rdd_scheduling.h"
#include "rdd_defs.h"
#include "xrdp_drv_qm_ag.h"
#endif
#ifdef INGRESS_FILTERS
#include "rdpa_filter_ex.h"
#endif
#include "rdpa_cpu_ex.h"

int port_attr_def_flow_read(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index, void *val,
    uint32_t size);
int port_attr_def_flow_write(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index, const void *val,
    uint32_t size);

/* rdpa physical enum values */
const bdmf_attr_enum_table_t rdpa_physical_port_enum_table =
{
    .type_name = "rdpa_physical_port",
    .help = "Physical ports",
    .values =
    {
        { "port0", rdpa_physical_port0 },
        { "port1", rdpa_physical_port1 },
        { "port2", rdpa_physical_port2 },
        { "port3", rdpa_physical_port3 },
        { "port4", rdpa_physical_port4 },
        { "port5", rdpa_physical_port5 },
        { "port6", rdpa_physical_port6 },
        { "port7", rdpa_physical_port7 },
        { "none", rdpa_physical_none },
        { NULL, 0 }
    }
};

static const bdmf_attr_enum_table_t rdpa_fc_traffic_fields_enum_table =
{
    .type_name = "traffic_types", .help = "Flow Control Traffic Type",
    .values = {
        {"broadcast", RDPA_FLOW_CONTROL_BROADCAST},
        {"multicast", RDPA_FLOW_CONTROL_MULTICAST},
        {"unknown_da", RDPA_FLOW_CONTROL_UNKNOWN_DA},
        {"all_traffic", RDPA_FLOW_CONTROL_ALL_TRAFFIC},
        {NULL, 0}
    }
};

static rdpa_if mirror_port = rdpa_if_none;

/***************************************************************************
 * port object type
 **************************************************************************/
DEFINE_BDMF_FASTLOCK(port_fastlock);

struct bdmf_object *port_objects[rdpa_if__number_of] = {};
rdpa_if physical_port_to_rdpa_if[rdpa_physical_none] = {};

/* Return wan_type of rdpa_if_wanX */
rdpa_wan_type rdpa_wan_if_to_wan_type(rdpa_if wan_if)
{
    port_drv_priv_t *port;

    if (!rdpa_if_is_wan(wan_if) || !port_objects[wan_if])
        return rdpa_wan_none;

    port = (port_drv_priv_t *)bdmf_obj_data(port_objects[wan_if]);
    return port->wan_type;
}

rdpa_if rdpa_physical_port_to_rdpa_if(rdpa_physical_port port)
{
    return physical_port_to_rdpa_if[port];
}
#ifndef BDMF_DRIVER_GPL_LAYER
EXPORT_SYMBOL(rdpa_physical_port_to_rdpa_if);
#else
extern rdpa_if(*f_rdpa_physical_port_to_rdpa_if)(rdpa_physical_port port);
#endif

static int port_emac_to_rdpa_if_set[rdpa_emac__num_of] = {};
static rdpa_if port_emac_to_rdpa_if_map[rdpa_emac__num_of];
rdpa_if _rdpa_port_emac_to_rdpa_if(rdpa_emac emac)
{
    if (emac == rdpa_emac_none || !port_emac_to_rdpa_if_set[emac])
        return rdpa_if_none;
    return port_emac_to_rdpa_if_map[emac];
}

static int rdpa_if_to_port_emac_set[rdpa_if__number_of] = {};
rdpa_emac rdpa_if_to_port_emac_map[rdpa_if__number_of];
rdpa_emac rdpa_port_rdpa_if_to_emac(rdpa_if port)
{
    if (!rdpa_if_to_port_emac_set[port])
        return rdpa_emac_none;
    return rdpa_if_to_port_emac_map[port];
}

#if defined(XRDP)
int rdpa_if_to_rdd_vport_set[rdpa_if__number_of]  = {};
rdd_vport_id_t rdpa_if_to_rdd_vport_map[rdpa_if__number_of];
/* These two arrays used only in simulator, defined also in rdpa gpl */
#ifdef RDP_SIM
int rdd_vport_to_rdpa_if_set[RDD_VPORT_LAST + 1]  = {};
rdpa_if rdd_vport_to_rdpa_if_map[RDD_VPORT_LAST + 1];
#else
extern int rdd_vport_to_rdpa_if_set[RDD_VPORT_LAST + 1];
extern rdpa_if rdd_vport_to_rdpa_if_map[RDD_VPORT_LAST + 1];
#endif
#endif

rdpa_ports rdpa_lag_mask = 0; /**<Lag linked ports, relevant only to switch port */
rdd_emac_id_vector_t emac_id_vector;

rdpa_system_init_cfg_t *sys_init_cfg;
rdpa_port_stat_t accumulate_port_stat[rdpa_if__number_of];

bdmf_boolean is_lag_config_done(void)
{
    bdmf_link_handle link;

    /* Go over all object us links */
    link = bdmf_get_next_us_link(port_objects[rdpa_if_switch], NULL);

    while (link)
    {
        struct bdmf_link *next = bdmf_get_next_us_link(port_objects[rdpa_if_switch], link);

        if (bdmf_us_link_to_object(link)->drv != rdpa_port_drv())
            return 1;

        link = next;
    }

    return 0;
}

/** This optional callback is called called at object init time
 * before initial attributes are set.
 * If function returns error code !=0, object creation is aborted
 */
static int port_pre_init(struct bdmf_object *mo)
{
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo);
    bdmf_error_t rc = BDMF_ERR_OK;
    sys_init_cfg = (rdpa_system_init_cfg_t *)_rdpa_system_init_cfg_get();

    port->index = rdpa_if_none;
    port->wan_type = rdpa_wan_none;
    port->default_cfg_exist = 0;
    port->cfg.control_sid = rdpa_if_none;
    port->speed = rdpa_speed_none;
    port->cfg.sal_enable = 1;
    /* For NOT XRDP configure default SA lookup different if triple tag detection configured */
#ifndef XRDP
    if (is_triple_tag_detect())
        port->cfg.sal_enable = 0;
#endif  /* XRDP */
    port->cfg.dal_enable = 1;
    port->cfg.sal_miss_action = rdpa_forward_action_host;
    port->cfg.dal_miss_action = rdpa_forward_action_host;
    port->cfg.ae_enable = 0;
    /* EMAC cfg to none */
    port->cfg.emac = rdpa_emac_none;

    memset(&port->flow_ctrl.src_address, 0, sizeof(bdmf_mac_t));

    /* Physical port id for external switch */
    port->cfg.physical_port = rdpa_physical_none;

    /* Disable port mirroring */
    port->mirror_cfg.rx_dst_port = NULL;
    port->mirror_cfg.tx_dst_port = NULL;

    /* Port channel */
    port->channel = -1;
    port->def_flow_index = BDMF_INDEX_UNASSIGNED;

    /* Vlan isolation initial configuration, the rdd was configured in system object creation */
    port->vlan_isolation.us = (sys_init_cfg->switching_mode != rdpa_switching_none) ? 1 : 0;
    port->vlan_isolation.ds = (sys_init_cfg->switching_mode != rdpa_switching_none) ? 1 : 0;

    /* Transparent initial configuration */
    port->transparent = 0;

    /* port loopback */
    port->loopback_cfg.type = rdpa_loopback_type_none;
    port->loopback_cfg.op = rdpa_loopback_op_none;
    port->loopback_cfg.wan_flow = BDMF_INDEX_UNASSIGNED;
    port->loopback_cfg.queue = BDMF_INDEX_UNASSIGNED;

    /* port flow cache bypass */
    port->cfg.ls_fc_enable = rdpa_if_is_wlan(port->index) ? 1 : 0;
#if defined(__OREN__)
    if (port->index == rdpa_if_wlan0)
        rdd_local_switching_fc_enable(RDD_EMAC_ID_START, port->cfg.ls_fc_enable);
#endif
    /* Ingress QOS*/
    port->tm_cfg.discard_prty = rdpa_discard_prty_low;
    port->bridge_obj = NULL;

    /* Ingress filters */
    memset(port->ingress_filters, 0, sizeof(port->ingress_filters));
#ifdef INGRESS_FILTERS
    port->ingress_filters_profile = INVALID_PROFILE_IDX;
#endif

    port->proto_filters = rdpa_proto_filter_any_mask;
    return rc;
}

void rdpa_port_dp_cfg2mac_lkp_cfg(rdpa_port_dp_cfg_t *cfg,  rdpa_mac_lookup_cfg_t *mac_lkp_cfg)
{
    mac_lkp_cfg->sal_enable = cfg->sal_enable;
    mac_lkp_cfg->dal_enable = cfg->dal_enable;
    mac_lkp_cfg->sal_miss_action = cfg->sal_miss_action;
    mac_lkp_cfg->dal_miss_action = cfg->dal_miss_action;
}

int rdpa_cfg_sa_da_lookup(port_drv_priv_t *port, rdpa_port_dp_cfg_t *cfg, bdmf_boolean old_sa_action,
    bdmf_boolean is_active)
{
    bdmf_error_t rc;
    rdpa_mac_lookup_cfg_t mac_lkp_cfg;

    rdpa_port_dp_cfg2mac_lkp_cfg(cfg, &mac_lkp_cfg);
    rc = mac_lkp_cfg_validate_ex(&mac_lkp_cfg, port, cfg->ls_fc_enable);
    if (rc)
        return rc;

#ifdef XRDP
    if (!rdpa_if_is_lag_and_switch(port->index))
    {
        /* If object is not active, rdpa_if to vport mapping is not valid */
        if (is_active)
            rc = rdpa_cfg_sa_da_lookup_ex(port, cfg);
#else
    if ((rdpa_if_id(port->index) & RDPA_PORT_ALL_LOOKUP_PORTS) ||
        (rdpa_if_is_lag_and_switch(port->index) && port->index != rdpa_if_switch))
    {
        rc = rdpa_cfg_sa_da_lookup_ex(port, cfg);
#endif
        if (rc)
            return rc;
            /* parser configuration */
        rc = rdpa_update_da_sa_searches(port->index, cfg->dal_enable);
        if (rc)
            BDMF_TRACE_RET(rc, "error in func: rdpa_update_da_sa_searches\n");

        /* Handle the SA operations ref counter */
        if (is_active)
        {
            if (old_sa_action != cfg->sal_enable)
            {
                if (old_sa_action)
                    sa_mac_use_count_down();
                else
                    sa_mac_use_count_up();
            }
        }
        else
        {
            if (cfg->sal_enable)
                sa_mac_use_count_up();
        }
    }

    return rc;
}

/* Update all_ports_mask
 * Add/remove port to/from VID-0 eligibility mask if necessary
 */
int port_update_all_ports_set(bdmf_object_handle mo, int is_add)
{
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo);
    static rdpa_ports all_ports_mask; /* All configured ports */
    rdpa_ports new_mask = all_ports_mask;

    if (is_add)
        new_mask |= rdpa_if_id(port->index);
    else
        new_mask &= ~rdpa_if_id(port->index);

    /* Update VID0 if necessary */
    if (sys_init_cfg->switching_mode == rdpa_mac_based_switching)
    {
#ifndef XRDP
        rdd_emac_id_vector_t old_rdd_mask = rdpa_ports2rdd_emac_id_vector(all_ports_mask & RDPA_PORT_ALL_LAN);
        rdd_emac_id_vector_t new_rdd_mask = rdpa_ports2rdd_emac_id_vector(new_mask & RDPA_PORT_ALL_LAN);
#else
        rdd_vport_vector_t old_rdd_mask = rdpa_ports_to_rdd_egress_port_vector(all_ports_mask & RDPA_PORT_ALL_LAN, 0);
        rdd_vport_vector_t new_rdd_mask = rdpa_ports_to_rdd_egress_port_vector(new_mask & RDPA_PORT_ALL_LAN, 0);
#endif
        if (old_rdd_mask != new_rdd_mask)
        {
            int rdd_rc;
            rdd_lan_vid_cfg_t rdd_lan_vid_params = {
                .vid = 0,
                .aggregation_mode_port_vector = new_rdd_mask,
                .isolation_mode_port_vector = new_rdd_mask,
                .aggregation_vid_index = RDPA_VLAN_AGGR_ENTRY_DONT_CARE
            };

            rdd_rc = rdd_lan_vid_entry_cfg(0, &rdd_lan_vid_params);
            BDMF_TRACE_DBG_OBJ(mo, "rdd_lan_vid_entry_cfg(0, 0x%x) -> %d\n",
                (unsigned)new_rdd_mask, rdd_rc);
            if (rdd_rc)
            {
                BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "rdd_lan_vid_entry_cfg(0, 0x%x) --> %d\n",
                    (unsigned)new_rdd_mask, rdd_rc);
            }
        }
    }

    all_ports_mask = new_mask;

    return 0;
}

extern int system_post_init_wan(rdpa_wan_type wan_type, rdpa_emac wan_emac);

static int set_egress_tm_to_rdd(struct bdmf_object *mo, rdpa_port_tm_cfg_t *tm_cfg, int car_tcont_autoassign)
{
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo);
    int rc, channel_id;

    rc = rdpa_port_get_egress_tm_channel_from_port_ex(port->index, &channel_id);
    if (rc)
        return rc;

    /* Clear or assign ? */
    if (!car_tcont_autoassign)
    {
        if (tm_cfg->sched)
        {
            rc = _rdpa_egress_tm_channel_set(tm_cfg->sched, mo, channel_id);
            rc = rc ? rc : _rdpa_egress_tm_enable_set(tm_cfg->sched, 1);
        }
        else if (port->tm_cfg.sched)
        {
            rc = _rdpa_egress_tm_channel_set(port->tm_cfg.sched, NULL, channel_id);
        }
    }
#ifdef CONFIG_BCM_TCONT
    else if (rdpa_is_gpon_or_xgpon_mode())
    {
        /* Assign egress_tm on all TCONTs */
        bdmf_type_handle tcont_drv = rdpa_tcont_drv();
        bdmf_object_handle tcont_obj = NULL;
        bdmf_boolean mgmt;

        while ((tcont_obj = bdmf_get_next(tcont_drv, tcont_obj, NULL)))
        {
            /* OMCI management tcont should stay with default configuration*/
            rdpa_tcont_management_get(tcont_obj, &mgmt);
            if (mgmt == 1)
                continue;
            rc = rc ? rc : rdpa_tcont_egress_tm_set(tcont_obj, tm_cfg->sched);
        }
    }
#endif
#ifdef EPON
    else if (rdpa_is_epon_or_xepon_mode())
    {
        /* Assign egress_tm on all LLIDs */
        bdmf_type_handle llid_drv = rdpa_llid_drv();
        bdmf_object_handle llid_obj = NULL;

        while ((llid_obj = bdmf_get_next(llid_drv, llid_obj, NULL)))
            rc = rc ? rc : rdpa_llid_egress_tm_set(llid_obj, tm_cfg->sched);
    }
#endif
    port->channel = channel_id;

    return rc;
}

bdmf_error_t port_tm_reconf(struct bdmf_object *mo, rdpa_port_tm_cfg_t *tm_cfg, bdmf_boolean post_init)
{
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_ports lag_ports = rdpa_get_switch_lag_port_mask();
    int car_tcont_autoassign = 0;
    bdmf_error_t rc = BDMF_ERR_OK;
    int from_port = (port->index == rdpa_if_switch) ? rdpa_if_lag0 : port->index;
    int to_port = (port->index == rdpa_if_switch) ? rdpa_if_lag4 : port->index;
    int i;

    if (port->tm_cfg.discard_prty != tm_cfg->discard_prty)
    {
        rc = rdpa_port_tm_discard_prty_cfg_ex(mo, tm_cfg);
        if (rc)
            return rc;
    }

    /* For now we only support egress_tm on LAN, SWITCH and WAN ports */
    if (!rdpa_if_is_lan(port->index) && !rdpa_if_is_wan(port->index) && port->index != rdpa_if_switch)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_NOT_SUPPORTED, mo, "Egress TM is only supported on LAN, WAN and Switch ports\n");

    if (port->index == rdpa_if_switch && sys_init_cfg->runner_ext_sw_cfg.enabled)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_NOT_SUPPORTED, mo, "Egress TM is only supported on switch LAG interface\n");

    /* No change - good */
    if (port->tm_cfg.sched == tm_cfg->sched && !post_init)
        return 0;

    if (rdpa_if_is_wan(port->index)) /* WAN side */
    {
        /* egress_tm attribute can only be set on WAN port if it is GbE or we are
         * in car_tcont_autoassign mode
         */
        if (!rdpa_is_gbe_mode() && !rdpa_is_epon_ae_mode())
        {
            if (!rdpa_is_car_mode() && tm_cfg->sched)
            {
                BDMF_TRACE_RET_OBJ(BDMF_ERR_NOT_SUPPORTED, mo,
                    "Egress TM is only supported on GbE WAN port or in CAR mode\n");
            }
            car_tcont_autoassign = 1;
        }
    }

    for (i = from_port; i <= to_port; i++)
    {
        if (port_objects[i] && ((rdpa_if_id(i) & lag_ports) || port->index != rdpa_if_switch))
        {
            rc = set_egress_tm_to_rdd(port_objects[i], tm_cfg, car_tcont_autoassign);

            if (rc)
                return rc;
        }
    }

    /* Re-assign sched */
    port->tm_cfg.sched = tm_cfg->sched;

    return rc;
}

static int rdpa_cfg_transparent_cfg(port_drv_priv_t *port)
{
    rdd_vport_id_t rdd_port;
    int rdd_rc;
    bdmf_boolean transparent = port->transparent;

    if (!rdpa_if_is_lan(port->index))
        return 0;

    /* Get the RDD bridge port */
#ifndef XRDP
    rdd_port = rdpa_if_to_rdd_bridge_port(port->index, NULL);
#else
    rdd_port = rdpa_if_to_rdd_vport(port->index, port->wan_type);
#endif
    /* Disable aggregation on the port if it is being configured to transparent mode (and vice versa) */
    rdd_rc = rdd_us_vlan_aggregation_config(rdd_port, !transparent);
    if (rdd_rc)
    {
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "rdd_us_vlan_aggregation_config(%d, %d) --> %d\n",
            rdd_port, !transparent, rdd_rc);
    }
#ifdef BRIDGE_AGGR
    /* Update the aggregation mode in all the MAC entries related to this port */
    _rdpa_bridge_update_aggregation_in_mac_table(rdd_port, !transparent);
#endif
    return rdd_rc;
}

/** This optional callback is called at object init time
 * after initial attributes are set.
 * Its work is:
 * - make sure that all necessary attributes are set and make sense
 * - allocate dynamic resources if any
 * - assign object name if not done in pre_init
 * - finalise object creation
 * If function returns error code !=0, object creation is aborted
 */
static int port_post_init(struct bdmf_object *mo)
{
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo);
    int rc = 0;
#if !defined(RDPA_MULTI_WAN_SUPPORT)
    rdpa_if wan_if_idx;
#endif

    if (port_objects[port->index])
    {
        BDMF_TRACE_RET(BDMF_ERR_ALREADY, "Port %s is already configured\n",
            bdmf_attr_get_enum_text_hlp(&rdpa_if_enum_table, port->index));
    }

    /* From this point on do cleanup in destroy if there is error in the rest of post_init */
    port_objects[port->index] = mo;
    if (rdpa_if_is_wan(port->index))
    {
#if !defined(RDPA_MULTI_WAN_SUPPORT)
        for (wan_if_idx = rdpa_if_wan0; wan_if_idx <= rdpa_if_wan_max; wan_if_idx++)
        {
            if (wan_if_idx != port->index && rdpa_wan_if_to_wan_type(wan_if_idx) != rdpa_wan_none)
            {
                BDMF_TRACE_RET(BDMF_ERR_PARM, "Only 1 rdpa WAN port supported. System already configured with %s.\n",
                               bdmf_attr_get_enum_text_hlp(&rdpa_wan_type_enum_table, rdpa_wan_if_to_wan_type(wan_if_idx)));
            }
        }
#endif
        if (port->wan_type != rdpa_wan_none)
        {
            rc = system_post_init_wan(port->wan_type, port->cfg.emac);
            if (rc)
            {
                port_objects[port->index] = NULL;
                BDMF_TRACE_RET(BDMF_ERR_PARM, "system_post_init_wan failed: %s!\n",
                    bdmf_attr_get_enum_text_hlp(&rdpa_if_enum_table, port->index));
            }
        }
        else
        {
            port_objects[port->index] = NULL;
            BDMF_TRACE_RET(BDMF_ERR_PARM, "Mandatory attribute wan_type is not set: %s!\n",
                bdmf_attr_get_enum_text_hlp(&rdpa_if_enum_table, port->index));
        }
    }

    /* LAG ports and fttdp mode always work in promiscuous mode */
    if ((rdpa_if_is_lag_and_switch(port->index) && !rdpa_is_ext_switch_mode()) ||
        rdpa_is_fttdp_mode())
    {
        port->cfg.sal_enable = 0;
        port->cfg.dal_enable = 0;
    }

    /* set object name */
    snprintf(mo->name, sizeof(mo->name), "port/index=%s",
        bdmf_attr_get_enum_text_hlp(&rdpa_if_enum_table, port->index));

    /* Clear counters */
    memset(&accumulate_port_stat[port->index], 0, sizeof(rdpa_port_stat_t));

    /* Enable scheduling/policing */
    if (port->tm_cfg.sched)
    {
        rc = port_tm_reconf(mo, &port->tm_cfg, 1);
        if (rc)
            BDMF_TRACE_RET_OBJ(rc, mo, "Failed to configure tm settings\n");
    }

    if (port->transparent)
    {
        rc = rdpa_cfg_transparent_cfg(port);
        if (rc)
            BDMF_TRACE_RET_OBJ(rc, mo, "Failed to configure transparent");
    }

    if (port->cfg.emac != rdpa_emac_none)
    {
        port_emac_to_rdpa_if_map[port->cfg.emac] = port->index;
        port_emac_to_rdpa_if_set[port->cfg.emac] = 1;
        rdpa_if_to_port_emac_map[port->index] = port->cfg.emac;
        rdpa_if_to_port_emac_set[port->index] = 1;
    }
#ifdef XRDP
    else if (!rdpa_if_is_lag_and_switch(port->index) && rdpa_is_ext_switch_mode() && rdpa_if_is_lan(port->index))
    {
        /* SF2 port */
        rdpa_emac emac = rdpa_emac0 + port->index - rdpa_if_lan0;
        port_emac_to_rdpa_if_map[emac] = port->index;
        port_emac_to_rdpa_if_set[emac] = 1;
        rdpa_if_to_port_emac_map[port->index] = emac;
        rdpa_if_to_port_emac_set[port->index] = 1;

        update_broadcom_tag_size();
    }
#endif

    rc = port_post_init_ex(mo);
    if (rc)
        rdpa_if_to_port_emac_set[port->index] = 0; /* rollback */
    return rc;
}

static void port_destroy(struct bdmf_object *mo)
{
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo);

    if ((unsigned)port->index >= rdpa_if__number_of || port_objects[port->index] != mo)
        return;

    /*
     * Cleanups
     */
#ifdef XRDP
    port_flow_del(port);
    if (rdpa_if_to_rdd_vport_set[port->index])
        rdd_tx_flow_enable(rdpa_port_rdpa_if_to_vport(port->index), rdpa_dir_ds, 0);
#endif

    if (port->cfg.emac != rdpa_emac_none && port_emac_to_rdpa_if_set[port->cfg.emac])
    {
        port_emac_to_rdpa_if_set[port->cfg.emac] = 0;
        rdpa_if_to_port_emac_set[port->index] = 0;
#ifdef XRDP
        rdpa_if_to_rdd_vport_set[port->index] = 0;
        rdd_vport_to_rdpa_if_set[rdpa_if_to_rdd_vport_map[port->index]] = 0;
#endif
    }

    /* Handle the SA lookup ref counter */
    if (port->cfg.sal_enable)
        sa_mac_use_count_down();

    /* Disable scheduling/policing */
    if (port->tm_cfg.sched)
    {
        rdpa_port_tm_cfg_t dummy_tm_cfg = {};
        port_tm_reconf(mo, &dummy_tm_cfg, 0);
    }
    port_update_all_ports_set(mo, 0);

    port_objects[port->index] = NULL;
}

/* When writing this value, it MUST come after setting index - order of how attrs are processed */
static int port_attr_wan_type_write(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, const void *val, uint32_t size)
{
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_wan_type wan_type = *(rdpa_wan_type *)val;

    if (wan_type == rdpa_wan_none)
    {
        if (rdpa_if_is_wan(port->index))
        {
            BDMF_TRACE_RET(BDMF_ERR_PARM, "Missing wan_type attribute on WAN port: %s!\n",
                bdmf_attr_get_enum_text_hlp(&rdpa_if_enum_table, port->index));
        }
        return 0;
    }

    if (!rdpa_if_is_wan(port->index))
    {
        BDMF_TRACE_RET(BDMF_ERR_PARM, "Can not configure wan_type attribute on non-WAN port: %s!\n",
            bdmf_attr_get_enum_text_hlp(&rdpa_if_enum_table, port->index));
    }

    if (port->wan_type != rdpa_wan_none)
    {
        BDMF_TRACE_RET(BDMF_ERR_PARM, "wan type already set: %s!\n",
            bdmf_attr_get_enum_text_hlp(&rdpa_wan_type_enum_table, port->wan_type));
    }
    port->wan_type = wan_type;

    return port_attr_wan_type_write_ex(port, wan_type);
}

static int port_attr_wan_type_read(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
        void *val, uint32_t size)
{
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo);

    if (!rdpa_if_is_wan(port->index))
        return BDMF_ERR_NOENT;

    *(rdpa_wan_type *)val = port->wan_type;

    return 0;
}

/* "cfg" attribute "write" callback */
static int port_attr_cfg_write(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, const void *val, uint32_t size)
{
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_port_dp_cfg_t *cfg = (rdpa_port_dp_cfg_t *)val;
    bdmf_error_t rc = BDMF_ERR_OK;
    rdpa_physical_port prev_physical_port = port->cfg.physical_port;

#ifdef RDP
    if (rdpa_is_fttdp_mode() && (cfg->sal_enable || cfg->dal_enable))
        BDMF_TRACE_RET(BDMF_ERR_PARM, "Can not configure SA/DA lookup for fttdp!\n");

    /* Check if triple tag detection and SA lookup are configured both - bypass for parser bug in RDP only*/
    if (is_triple_tag_detect() && cfg->sal_enable)
        BDMF_TRACE_RET(BDMF_ERR_PARM, "Can not configure SA action with Triple tag detection!\n");
#endif
    if (cfg->emac != port->cfg.emac && mo->state != bdmf_state_init)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_NOT_SUPPORTED, mo, "Changing emac on active port is not supported\n");

    if (cfg->sal_enable && cfg->sal_miss_action == rdpa_forward_action_flood)
        BDMF_TRACE_RET(BDMF_ERR_PARM, "Flooding can be configured as DA lookup miss action only\n");

    /* Configure lan ports */
    if (cfg->emac != rdpa_emac_none && port->cfg.emac != cfg->emac)
    {
#if !defined(DSL_63138) && !defined(DSL_63148) && !defined(WL4908)
        if ((port->wan_type != rdpa_wan_gbe) && rdpa_if_is_wan(port->index))
            BDMF_TRACE_RET(BDMF_ERR_PARM, "Configure emac on wan port in none GBE mode is invalid\n");

        if ((port->wan_type == rdpa_wan_gbe && sys_init_cfg->gbe_wan_emac == cfg->emac) &&
            !rdpa_if_is_wan(port->index))
        {
            BDMF_TRACE_RET(BDMF_ERR_PARM, "emac %s is GBE emac id\n",
                bdmf_attr_get_enum_text_hlp(&rdpa_emac_enum_table, cfg->emac));
        }
#endif

        /* Check for the emac id is enabled at system level */
        if (!(sys_init_cfg->enabled_emac & (rdpa_emac_id(cfg->emac)))) /* for lan check init_cfg enabled_mac */
        {
            BDMF_TRACE_RET_OBJ(BDMF_ERR_RANGE, mo, "emac %s is not configured in system\n",
                bdmf_attr_get_enum_text_hlp(&rdpa_emac_enum_table, cfg->emac));
        }

        /* Check if the wanted EMAC is lan, wan or lag one*/
        if (!rdpa_if_is_lan_lag_and_switch(port->index) && !rdpa_if_is_wan(port->index))
        {
            BDMF_TRACE_RET_OBJ(BDMF_ERR_RANGE, mo, "port id - %s is not lan , wan or physical port\n",
                bdmf_attr_get_enum_text_hlp(&rdpa_if_enum_table, port->index));
        }

#ifndef G9991
        {
            int i;

            /* Check if the EMAC already configure to another port */
            for (i = 0; i < rdpa_if__number_of; ++i)
            {
                bdmf_fastlock_lock(&port_fastlock);
                if (port_objects[i])
                {
                    port_drv_priv_t *temp_port = (port_drv_priv_t *)bdmf_obj_data(port_objects[i]);
                    if (temp_port->cfg.emac == cfg->emac)
                    {
                        bdmf_fastlock_unlock(&port_fastlock);
                        BDMF_TRACE_RET_OBJ(BDMF_ERR_ALREADY, mo,
                            "emac %s is already configured to other port\n",
                            bdmf_attr_get_enum_text_hlp(&rdpa_emac_enum_table, cfg->emac));
                    }
                }
                bdmf_fastlock_unlock(&port_fastlock);
            }
        }
#endif
    }

    rc = port_ls_fc_cfg_ex(mo, cfg);
    if (rc)
        return rc;

    bdmf_fastlock_lock(&port_fastlock);
    if (mo->state == bdmf_state_active)
    {
#ifdef G9991
        if (rdpa_if_is_lan(port->index))
        {
            rdd_g9991_vport_to_emac_mapping_cfg(port->index - rdpa_if_lan0,
                cfg->physical_port);
        }
        else if (rdpa_if_is_lag_and_switch(port->index) && (cfg->control_sid != rdpa_if_none))
        {
#ifndef XRDP
            bdmf_fastlock_unlock(&port_fastlock);
            BDMF_TRACE_RET(BDMF_ERR_PARM, "error in parameters: control sid can be configured to LAG port only\n");
#else
            port->cfg.control_sid = cfg->control_sid;
            rdd_g9991_control_sid_set(port->cfg.control_sid - rdpa_if_lan0, cfg->physical_port);
#endif
        }
        else
#endif
            cfg->physical_port = prev_physical_port; /* Write physical_port just in init time */
#ifdef XRDP
        rc = rdpa_cfg_sa_da_lookup(port, cfg, port->cfg.sal_enable, mo->state == bdmf_state_active);
#else
        rc = rdpa_cfg_sa_da_lookup(port, cfg, port->cfg.sal_enable, 1);
#endif
        if (rc)
        {
            bdmf_fastlock_unlock(&port_fastlock);
            BDMF_TRACE_RET(rc, "error in func: rdpa_cfg_sa_da_lookup\n");
        }
    }

    /* Save configuration */
    port->cfg = *cfg;
#if defined(BCM63158)
    port->cfg.physical_port = rdpa_physical_none;
#endif
    bdmf_fastlock_unlock(&port_fastlock);

    return 0;
}

/* "tm_cfg" attribute "write" callback */
static int port_attr_tm_cfg_write(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, const void *val, uint32_t size)
{
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_port_tm_cfg_t *tm_cfg = (rdpa_port_tm_cfg_t *)val;
    int rc = 0;

    /* If we are during object destroy, object state might not be active. */
    if (mo->state != bdmf_state_init)
    {
        rc = port_tm_reconf(mo, tm_cfg, 0);
        if (rc < 0)
            return rc;
    }

    port->tm_cfg = *tm_cfg;

    return 0;
}

/* "sa_limit" attribute "write" callback */
static int port_attr_sa_limit_write(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, const void *val, uint32_t size)
{
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_port_sa_limit_t *sa_limit = (rdpa_port_sa_limit_t *)val;

    /* validate parameters */
    if (rdpa_if_is_lag_and_switch(port->index))
    {
        BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, mo, "Can't set SA limit on lag port\n");
    }

    if (sa_limit->max_sa && port->sa_limit.num_sa > sa_limit->max_sa)
    {
        BDMF_TRACE_RET_OBJ(BDMF_ERR_TOO_MANY, mo, "max_sa %u is less than the current num_sa %u\n",
            sa_limit->max_sa, port->sa_limit.num_sa);
    }

    /* Copy up to the 1st read-only field */
    port->sa_limit.max_sa = sa_limit->max_sa;

    return 0;
}

/* "flow_control" attribute "write" callback */
static int port_attr_flow_control_write(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, const void *val, uint32_t size)
{
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_port_flow_ctrl_t *flow_ctrl = (rdpa_port_flow_ctrl_t *)val;
    int rc;

    rc = port_flow_control_cfg_ex(port, flow_ctrl);
    if (rc)
    {
        BDMF_TRACE_RET(rc, "Failed to configure flow control for port %s\n",
            bdmf_attr_get_enum_text_hlp(&rdpa_if_enum_table, port->index));
    }

    return rc;
}

/* "mirror_cfg" attribute "write" callback */
static int port_attr_mirror_cfg_write(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, const void *val, uint32_t size)
{
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_port_mirror_cfg_t *new_port_mirror = (rdpa_port_mirror_cfg_t *)val;
    rdpa_if  rx_port_idx, tx_port_idx;
    int rc;

    if (mirror_port != rdpa_if_none && port->index != mirror_port)
    {
        BDMF_TRACE_RET(BDMF_ERR_PARM, "Mirroring already configured for port %s\n",
            bdmf_attr_get_enum_text_hlp(&rdpa_if_enum_table, port->index));
    }
    if (new_port_mirror->rx_dst_port && new_port_mirror->rx_dst_port->drv != rdpa_port_drv())
        BDMF_TRACE_RET(BDMF_ERR_PARM, "Incorrect RX destination mirroring port. Only PORT object can be mirrored\n");

    if (new_port_mirror->tx_dst_port && new_port_mirror->tx_dst_port->drv != rdpa_port_drv())
        BDMF_TRACE_RET(BDMF_ERR_PARM, "Incorrect TX destination mirroring port, Only PORT object can be mirrored\n");
    rdpa_port_index_get(new_port_mirror->tx_dst_port, &tx_port_idx);
    rdpa_port_index_get(new_port_mirror->rx_dst_port, &rx_port_idx);
    if (rdpa_if_is_cpu_port(tx_port_idx) || rdpa_if_is_cpu_port(rx_port_idx))
        BDMF_TRACE_RET(BDMF_ERR_NOT_SUPPORTED, "TX or RX destination mirroring port is CPU\n");
    if (rdpa_if_is_cpu_port(port->index))
        BDMF_TRACE_RET(BDMF_ERR_PARM, "Cannot mirror CPU port\n");
    rc = port_mirror_cfg_ex(mo, port, new_port_mirror);
    if (rc)
        BDMF_TRACE_RET(rc, "Failed to configure port mirroring\n");
    /* Save configuration */
    port->mirror_cfg.rx_dst_port = new_port_mirror->rx_dst_port;
    port->mirror_cfg.tx_dst_port = new_port_mirror->tx_dst_port;
    mirror_port = port->index;
    return 0;
}

/* "loopback" attribute "read" callback */
static int port_attr_loopback_read(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    void *val, uint32_t size)
{
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_port_loopback_t *lb_req = (rdpa_port_loopback_t *)val;

    lb_req->type = port->loopback_cfg.type;

    /* if the type is none, the operation is irrelevant */
    lb_req->op = (port->loopback_cfg.type == rdpa_loopback_type_none) ?
        rdpa_loopback_op_none : port->loopback_cfg.op;
    lb_req->wan_flow = port->loopback_cfg.wan_flow;
    lb_req->queue = port->loopback_cfg.queue;

    return 0;
}

/* "loopback" attribute "write" callback */
static int port_attr_loopback_write(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, const void *val, uint32_t size)
{
    return port_attr_loopback_write_ex(mo, ad, index, val, size);
}

/* "vlan_isolation" attribute "write" callback */
static int port_vlan_isolation_cfg_write(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, const void *val, uint32_t size)
{
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_port_vlan_isolation_t *vlan_isolation_cfg = (rdpa_port_vlan_isolation_t *)val;
    int rc;

    if (!rdpa_if_is_lan(port->index))
        BDMF_TRACE_RET(BDMF_ERR_RANGE, "Error configuring vlan isolation not on lan port\n");

    rc = port_vlan_isolation_cfg_ex(port, vlan_isolation_cfg, mo->state == bdmf_state_active);
    if (rc)
        BDMF_TRACE_RET(rc, "Failed to configure vlan isolation\n");

    /* Save configuration */
    port->vlan_isolation.us = vlan_isolation_cfg->us;
    port->vlan_isolation.ds = vlan_isolation_cfg->ds;
    return 0;
}

/* "vlan_isolation" attribute "read" callback */
static int port_vlan_isolation_cfg_read(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    void *val, uint32_t size)
{
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_port_vlan_isolation_t *vlan_isolation_cfg = (rdpa_port_vlan_isolation_t *)val;
#ifdef XRDP
    if (!rdpa_if_is_lan(port->index))
        return BDMF_ERR_NOENT;
#else
    if (!rdpa_if_is_lan(port->index) || sys_init_cfg->switching_mode == rdpa_switching_none)
        return BDMF_ERR_NOENT;
#endif
    vlan_isolation_cfg->us = port->vlan_isolation.us;
    vlan_isolation_cfg->ds = port->vlan_isolation.ds;

    return 0;
}

rdpa_speed_type rdpa_wan_speed_get(rdpa_if if_)
{
    struct bdmf_object *mo;
    port_drv_priv_t *port = NULL;

    if (!rdpa_if_is_wan(if_))
        return rdpa_speed_none;

    mo = port_objects[if_];

    if (!mo)
        return rdpa_speed_none;

    port = (port_drv_priv_t *)bdmf_obj_data(mo);
    return port->speed;
}

static int port_attr_speed_write(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, const void *val, uint32_t size)
{
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_speed_type new_speed = *(rdpa_speed_type *)val;

    if (!port->cfg.ae_enable)
    {
        BDMF_TRACE_RET(BDMF_ERR_PARM, "Speed can only be set in ae_enable mode\n");
    }
    else
    {
#ifdef XRDP
        qm_epon_overhead_ctrl epon_counter_cfg = {};

        ag_drv_qm_epon_overhead_ctrl_get(&epon_counter_cfg);
        if (new_speed == rdpa_speed_1g)
            epon_counter_cfg.epon_line_rate = 0;
        else
            epon_counter_cfg.epon_line_rate = 1;
        ag_drv_qm_epon_overhead_ctrl_set(&epon_counter_cfg);
#endif
        port->speed = new_speed;
    }

    return 0;
}

bdmf_boolean rdpa_is_epon_ae_mode(void)
{
    struct bdmf_object *mo = NULL;
    port_drv_priv_t *port = NULL;
    rdpa_if if_ = rdpa_wan_type_to_if(rdpa_wan_epon);

    if (!rdpa_if_is_wan(if_))
        return 0;
    if (!port_objects[if_])
    {
        BDMF_TRACE_ERR("Port wan not set\n");
        return 0;
    }
    mo = port_objects[if_];

    port = (port_drv_priv_t *)bdmf_obj_data(mo);
    return (port->wan_type == rdpa_wan_epon || port->wan_type == rdpa_wan_xepon) && port->cfg.ae_enable;
}

/* XXX: Stub should be moved RDD files */
#ifdef G9991
#define rdd_us_vlan_aggregation_config(...) -1
#endif

/* "transparent" attribute "write" callback */
static int port_transparent_cfg_write(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index, const void *val,
    uint32_t size)
{
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo);
    bdmf_boolean transparent = *(bdmf_boolean *)val;
    int rdd_rc = 0;
    bdmf_boolean is_object_active = (mo->state == bdmf_state_active ? 1 : 0);

    port->transparent = transparent;

    if (is_object_active)
    {
        rdd_rc = rdpa_cfg_transparent_cfg(port);
    }
    return rdd_rc;
}
#if defined(DSL_63138) || defined(DSL_63148) || defined(WL4908) || defined(DSL_63158)
static int bond_link_update_lookup_port(struct bdmf_object *bond_obj, struct bdmf_object *link_obj)
{
    port_drv_priv_t *link_port = (port_drv_priv_t *)bdmf_obj_data(link_obj);
    port_drv_priv_t *bond_port = (port_drv_priv_t *)bdmf_obj_data(bond_obj);
    bdmf_link_handle link;
    rdpa_if min_rdpa_if = rdpa_if__number_of; /* Start with the maximum */
    BL_LILAC_RDD_BRIDGE_PORT_DTE xx_bridge_port;
    uint8_t xx_lookup_port;

    /* Go over all linked objects to find the lowest rdpa_if value */
    if (link_port->index < min_rdpa_if)
    {
        min_rdpa_if = link_port->index;
    }
    link = bdmf_get_next_us_link(port_objects[bond_port->index], NULL);
    while (link)
    {
        struct bdmf_object *local_link_mo = bdmf_us_link_to_object(link);
        port_drv_priv_t *local_link_port = (port_drv_priv_t *)bdmf_obj_data(local_link_mo);
        if (local_link_port->index < min_rdpa_if)
        {
            min_rdpa_if = local_link_port->index;
        }

        link = bdmf_get_next_us_link(port_objects[bond_port->index], link);
    }

    /* Set the lowest rdpa_if/Bridge_port value as lookup port */
    xx_lookup_port = rdpa_if_to_rdd_bridge_port(min_rdpa_if, NULL);

    /* Go over all linked objects to set/update the lookup port */
    xx_bridge_port = rdpa_if_to_rdd_bridge_port(link_port->index, NULL);
    rdd_lookup_ports_mapping_table_config(xx_bridge_port, xx_lookup_port);

    link = bdmf_get_next_us_link(port_objects[bond_port->index], NULL);
    while (link)
    {
        struct bdmf_object *local_link_mo = bdmf_us_link_to_object(link);
        port_drv_priv_t *local_link_port = (port_drv_priv_t *)bdmf_obj_data(local_link_mo);
        xx_bridge_port = rdpa_if_to_rdd_bridge_port(local_link_port->index, NULL);
        rdd_lookup_ports_mapping_table_config(xx_bridge_port, xx_lookup_port);

        link = bdmf_get_next_us_link(port_objects[bond_port->index], link);
    }

    return 0;
}

static int bond_unlink_update_lookup_port(struct bdmf_object *bond_obj, struct bdmf_object *link_obj)
{
    port_drv_priv_t *link_port = (port_drv_priv_t *)bdmf_obj_data(link_obj);
    port_drv_priv_t *bond_port = (port_drv_priv_t *)bdmf_obj_data(bond_obj);
    bdmf_link_handle link;
    rdpa_if min_rdpa_if = rdpa_if__number_of; /* Start with the maximum */
    BL_LILAC_RDD_BRIDGE_PORT_DTE xx_bridge_port;
    uint8_t xx_lookup_port;

    /* Go over all linked objects to find the lowest rdpa_if value
     * DO NOT include the object to be removed but restore its lookup port */
    xx_bridge_port = rdpa_if_to_rdd_bridge_port(link_port->index, NULL);
    rdd_lookup_ports_mapping_table_restore(xx_bridge_port);

    link = bdmf_get_next_us_link(port_objects[bond_port->index], NULL);
    while (link)
    {
        struct bdmf_object *local_link_mo = bdmf_us_link_to_object(link);
        port_drv_priv_t *local_link_port = (port_drv_priv_t *)bdmf_obj_data(local_link_mo);
        if (link_obj != local_link_mo && /* Skip the node to be removed */
            local_link_port->index < min_rdpa_if)
        {
            min_rdpa_if = local_link_port->index;
        }

        link = bdmf_get_next_us_link(port_objects[bond_port->index], link);
    }

    /* Set the lowest rdpa_if/Bridge_port value as lookup port */
    xx_lookup_port = rdpa_if_to_rdd_bridge_port(min_rdpa_if, NULL);

    /* Go over all linked objects to set/update the lookup port */
    link = bdmf_get_next_us_link(port_objects[bond_port->index], NULL);
    while (link)
    {
        struct bdmf_object *local_link_mo = bdmf_us_link_to_object(link);
        port_drv_priv_t *local_link_port = (port_drv_priv_t *)bdmf_obj_data(local_link_mo);
        if (link_obj != local_link_mo) /* Skip the node to be removed */
        {
            xx_bridge_port = rdpa_if_to_rdd_bridge_port(local_link_port->index, NULL);
            rdd_lookup_ports_mapping_table_config(xx_bridge_port, xx_lookup_port);
        }

        link = bdmf_get_next_us_link(port_objects[bond_port->index], link);
    }

    return 0;
}
#else
static int bond_link_update_lookup_port(struct bdmf_object *bond_obj, struct bdmf_object *link_obj)
{
    return 0;
}
static int bond_unlink_update_lookup_port(struct bdmf_object *bond_obj, struct bdmf_object *link_obj)
{
    return 0;
}
#endif

static int is_bonding_lan_wan_port(struct bdmf_object *bond_obj, struct bdmf_object *link_obj,
                                   rdpa_physical_port *physical_lan_port_p)
{
    /* Below logic is to find if this group is bonding LAN & WAN ports together
       In general, only LAN or only WAN ports should be bonded together. */
    port_drv_priv_t *link_port = (port_drv_priv_t *)bdmf_obj_data(link_obj);
    port_drv_priv_t *bond_port = (port_drv_priv_t *)bdmf_obj_data(bond_obj);
    bdmf_link_handle link;
    int is_wan = 0;
    int is_lan = 0;

    if (rdpa_if_is_wan(link_port->index))
    {
        is_wan = 1;
    }
    else if (rdpa_if_is_lan(link_port->index))
    {
        rdpa_if_to_rdpa_physical_port(link_port->index, physical_lan_port_p);
        is_lan = 1;
    }
    /* Go over all upstream links */
    link = bdmf_get_next_us_link(port_objects[bond_port->index], NULL);

    while (link)
    {
        struct bdmf_object *local_link_mo = bdmf_us_link_to_object(link);
        port_drv_priv_t *local_link_port = (port_drv_priv_t *)bdmf_obj_data(local_link_mo);
        if (is_lan && rdpa_if_is_wan(local_link_port->index))
        {
            is_wan = 1;
            break;
        }
        else if (is_wan && rdpa_if_is_lan(local_link_port->index))
        {
            is_lan = 1;
            rdpa_if_to_rdpa_physical_port(local_link_port->index, physical_lan_port_p);
            break;
        }

        link = bdmf_get_next_us_link(port_objects[bond_port->index], link);
    }
    return is_lan & is_wan;
}

static int bond_link_port(struct bdmf_object *bond_obj, struct bdmf_object *link_obj)
{
    /* NOTE : When this function is called, the bonding object is not modified yet,
       i.e. the object to be added is NOT linked with bonding object */
    int err = 0;
    rdpa_physical_port physical_lan_port = rdpa_physical_none;
    if (is_bonding_lan_wan_port(bond_obj, link_obj, &physical_lan_port))
    {
        if (physical_lan_port != rdpa_physical_none)
        {
            err = rdpa_port_bond_link_ex(physical_lan_port);
        }
    }

    if (!err)
    {
        err = bond_link_update_lookup_port(bond_obj, link_obj);
    }

    return err;
}
static int bond_unlink_port(struct bdmf_object *bond_obj, struct bdmf_object *link_obj)
{
    /* NOTE : When this function is called, the bonding object is not modified yet,
       i.e. the object to be removed is still linked with bonding object */
    int err = 0;
    rdpa_physical_port physical_lan_port = rdpa_physical_none;
    if (is_bonding_lan_wan_port(bond_obj, link_obj, &physical_lan_port))
    {
        if (physical_lan_port != rdpa_physical_none)
        {
            err = rdpa_port_bond_unlink_ex(0xFF);
        }
    }

    if (!err)
    {
        err = bond_unlink_update_lookup_port(bond_obj, link_obj);
    }
    return err;
}

/** Called when switch port is linked to lag port
    or bond ports is linked to any port */
static int port_attr_link_port(struct bdmf_object *sw_obj, struct bdmf_object *lag_obj,
    const char *link_attrs)
{
    port_drv_priv_t *lag_port = (port_drv_priv_t *)bdmf_obj_data(lag_obj);
    port_drv_priv_t *sw_port = (port_drv_priv_t *)bdmf_obj_data(sw_obj);
    int rc;
    if (sw_port->index >= rdpa_if_bond0 && sw_port->index <= rdpa_if_bond_max)
    {
        return bond_link_port(sw_obj, lag_obj);
    }

    /* linking of port to bridge or qos mapper is done in their link function */
    if (sw_obj->drv != rdpa_port_drv() || lag_obj->drv != rdpa_port_drv())
        return 0;

    if (rdpa_is_ext_switch_mode())
        BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, sw_obj, "Error: lag and external switch can't work together\n");

    if (sw_port->index != rdpa_if_switch || (lag_port->index < rdpa_if_lag0 && lag_port->index > rdpa_if_lag4))
        BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, sw_obj, "Error: only switch and lag port can be linked\n");

    /* noting to do, return */
    if (rdpa_is_fttdp_mode())
    {
#if defined G9991 && !defined XRDP
        update_port_tag_size(lag_port->cfg.emac, 4);
#endif
        return 0;
    }

    if (is_lag_config_done())
        BDMF_TRACE_RET_OBJ(BDMF_ERR_PARM, sw_obj, "Error: switch is already linked to other objects but lag\n");

    rc = rdpa_port_lag_link_ex(lag_port);
    if (!rc)
        BDMF_TRACE_DBG_OBJ(sw_obj, "Switch port is linked with %x port mask\n", (int)rdpa_lag_mask);

    return rc;
}

/** Called when switch port is unlinked from lag port
    or bond ports is unlinked from any port */
static void port_attr_unlink_port(struct bdmf_object *sw_obj, struct bdmf_object *lag_obj)
{
    port_drv_priv_t *lag_port = (port_drv_priv_t *)bdmf_obj_data(lag_obj);
    port_drv_priv_t *sw_port = (port_drv_priv_t *)bdmf_obj_data(sw_obj);

    if (sw_port->index >= rdpa_if_bond0 && sw_port->index <= rdpa_if_bond_max)
    {
        bond_unlink_port(sw_obj, lag_obj);
        return;
    }

    /* unlinking of port to bridge or qos mapper is done in their link function */
    if (sw_obj->drv != rdpa_port_drv() || lag_obj->drv != rdpa_port_drv())
        return;

    if (sw_port->index != rdpa_if_switch || (lag_port->index < rdpa_if_lag0 && lag_port->index > rdpa_if_lag4))
        BDMF_TRACE_DBG_OBJ(sw_obj, "Error: only switch and lag port can be linked\n");

    /* noting to do, return */
    if (rdpa_is_fttdp_mode())
    {
        update_port_tag_size(lag_port->cfg.emac, 0);
        return;
    }

    if (is_lag_config_done())
        BDMF_TRACE_DBG_OBJ(sw_obj, "Error: switch is linked to other objects but lag\n");

    rdpa_port_lag_unlink_ex(lag_port);
    BDMF_TRACE_DBG_OBJ(sw_obj, "Switch port is unlinked from %x port mask\n", (int)rdpa_lag_mask);
}

static int port_attr_ingress_filter_read(struct bdmf_object *mo, struct bdmf_attr *ad,
    bdmf_index index, void *val, uint32_t size)
{
#ifdef INGRESS_FILTERS
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo);

    return ingress_filter_ctrl_cfg_read(port->ingress_filters, index, val);
#else
    return BDMF_ERR_NOT_SUPPORTED;
#endif
}

static int port_attr_ingress_filter_write(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, const void *val, uint32_t size)
{
#ifdef INGRESS_FILTERS
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_filter_ctrl_t *ctrl = (rdpa_filter_ctrl_t *)val;
    int rc;

    rc = ingress_filter_ctrl_cfg_validate(index, (void *)val);
    if (rc)
        return rc;

    rc = port_attr_ingress_filter_write_ex(mo, ad, index, val, size);
    if (rc)
        return rc;

    memcpy(&port->ingress_filters[index], ctrl, sizeof(rdpa_filter_ctrl_t));

    return 0;
#else
    return BDMF_ERR_NOT_SUPPORTED;
#endif
}

static int port_attr_cpu_meter_write(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, const void *val, uint32_t size)
{
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_traffic_dir dir = rdpa_if_is_wan(port->index) ? rdpa_dir_ds : rdpa_dir_us;
    bdmf_index meter_idx = val ? *(bdmf_index *)val : BDMF_INDEX_UNASSIGNED;
    bdmf_error_t rc;

    if (meter_idx != BDMF_INDEX_UNASSIGNED)
    {
        if (!cpu_meter_is_configured(dir, meter_idx))
        {
            BDMF_TRACE_RET_OBJ(BDMF_ERR_NOENT, mo,
                "CPU meter %ld is not configured\n", meter_idx);
        }
    }

    rc = port_attr_cpu_meter_write_ex(mo, dir, port->index, meter_idx);
    if (rc)
    {
        BDMF_TRACE_RET_OBJ(rc, mo,
            "Can't provision per-port CPU meter %ld\n", meter_idx);
    }
    port->cpu_meter = meter_idx;
    return 0;
}

static int port_attr_ingress_filter_get_next(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index *index)
{
#ifdef INGRESS_FILTERS
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo);

    return ingress_filter_ctrl_cfg_get_next(port->ingress_filters, index);
#else
    return BDMF_ERR_NOENT;
#endif
}

/* port is being notified that object it references
 * has been changed. It is used for policer update.
 * The callback can be called multiple times if multiple flows refer to
 * the same policer that has been changed.
 */
static void port_ref_changed(struct bdmf_object *mo, struct bdmf_object *ref_obj,
    struct bdmf_attr *ad, bdmf_index index, uint16_t attr_offset)
{
#if !(defined(DSL_63138) || defined(DSL_63148) || defined(WL4908) || defined(BCM63158))
    policer_drv_priv_t *policer;
    port_drv_priv_t *priv = (port_drv_priv_t *)bdmf_obj_data(mo);

    /* We only care about policer changes */
    if (ref_obj->drv != rdpa_policer_drv())
        return;
    policer = (policer_drv_priv_t *)bdmf_obj_data(ref_obj);
    port_set_bbh_timer_clock_ex(priv, policer);
#endif    
    return;
}

/* port_dp aggregate type */
struct bdmf_aggr_type port_dp_type =
{
    .name = "port_dp", .struct_name = "rdpa_port_dp_cfg_t",
    .help = "Data path port configuration",
    .fields = (struct bdmf_attr[])
    {
        {
            .name = "emac",
            .help = "EMAC identifier", .type = bdmf_attr_enum,
            .ts.enum_table = &rdpa_emac_enum_table,
            /* .min_val = rdpa_emac0, .max_val = rdpa_emac5, */
            .size = sizeof(rdpa_emac),
            .offset = offsetof(rdpa_port_dp_cfg_t, emac)
        },
        {
            .name = "sal",
            .help = "Source address lookup",
            .type = bdmf_attr_boolean,
            .size = sizeof(bdmf_boolean),
            .offset = offsetof(rdpa_port_dp_cfg_t, sal_enable)
        },
        {
            .name = "dal",
            .help = "Destination address lookup",
            .type = bdmf_attr_boolean,
            .size = sizeof(bdmf_boolean),
            .offset = offsetof(rdpa_port_dp_cfg_t, dal_enable)
        },
        {
            .name = "sal_miss_action",
            .help = "SA lookup miss action",
            .type = bdmf_attr_enum,
            .ts.enum_table = &rdpa_forward_action_enum_table,
            .size = sizeof(rdpa_forward_action),
            .offset = offsetof(rdpa_port_dp_cfg_t, sal_miss_action)
        },
        {
            .name = "dal_miss_action",
            .help = "DA lookup miss action",
            .type = bdmf_attr_enum,
            .ts.enum_table = &rdpa_forward_action_enum_table,
            .size = sizeof(rdpa_forward_action),
            .offset = offsetof(rdpa_port_dp_cfg_t, dal_miss_action)
        },
        {
            .name = "physical_port",
            .help = "Physical port for external switch",
            .type = bdmf_attr_enum,
            .ts.enum_table = &rdpa_physical_port_enum_table,
            .size = sizeof(rdpa_physical_port),
            .offset = offsetof(rdpa_port_dp_cfg_t, physical_port)
        },
        {
            .name = "ls_fc_enable",
            .help = "Local switching via flow cache enable",
            .type = bdmf_attr_boolean,
            .size = sizeof(bdmf_boolean),
            .offset = offsetof(rdpa_port_dp_cfg_t, ls_fc_enable)
        },
        {
            .name = "control_sid",
            .help = "Control SID (relevant for G9991 only)",
            .type = bdmf_attr_enum,
            .ts.enum_table = &rdpa_if_enum_table,
            .size = sizeof(rdpa_if),
            .offset = offsetof(rdpa_port_dp_cfg_t, control_sid)
        },
        {
            .name = "ae_enable",
            .help = "Active ethernet port",
            .type = bdmf_attr_boolean,
            .size = sizeof(bdmf_boolean),
            .offset = offsetof(rdpa_port_dp_cfg_t, ae_enable)
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE(port_dp_type);

/* port_sa_limit aggregate type */
struct bdmf_aggr_type port_sa_limit_type =
{
    .name = "port_sa_limit", .struct_name = "rdpa_port_sa_limit_t",
    .help = "Port SA limit configuration",
    .fields = (struct bdmf_attr[])
    {
        {
            .name = "max_sa",
            .help = "Max number of SAs that can be learned on port",
            .type = bdmf_attr_number,
            .size = sizeof(uint16_t),
            .offset = offsetof(rdpa_port_sa_limit_t, max_sa)
        },
        {
            .name = "num_sa",
            .help = "Number of SAs learned on port. Read-only",
            .type = bdmf_attr_number,
            .size = sizeof(uint16_t),
            .offset = offsetof(rdpa_port_sa_limit_t, num_sa)
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE(port_sa_limit_type);

/* port_tm aggregate type */
struct bdmf_aggr_type port_tm_type =
{
    .name = "port_tm", .struct_name = "rdpa_port_tm_cfg_t",
    .help = "Port TM configuration",
    .fields = (struct bdmf_attr[])
    {
        {
            .name = "egress_tm",
            .help = "Egress scheduler",
            .type = bdmf_attr_object,
            .ts.ref_type_name = "egress_tm",
            .size = sizeof(bdmf_object_handle),
            .offset = offsetof(rdpa_port_tm_cfg_t, sched),
        },
        {
            .name = "discard_prty",
            .help = "Ingress QOS priority",
            .type = bdmf_attr_enum, .ts.enum_table = &rdpa_disc_prty_enum_table,
            .size = sizeof(rdpa_discard_prty),
            .offset = offsetof(rdpa_port_tm_cfg_t, discard_prty),
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE(port_tm_type);

/* port_stat aggregate type */
struct bdmf_aggr_type port_stat_type =
{
    .name = "stat",
    .struct_name = "rdpa_port_stat_t",
    .help = "Port statistics",
    .extra_flags = BDMF_ATTR_UNSIGNED,
    .fields = (struct bdmf_attr[])
    {
        {
            .name = "rx_valid_pkt",
            .help = "Received valid packets",
            .size = sizeof(uint32_t),
            .type = bdmf_attr_number,
            .offset = offsetof(rdpa_port_stat_t, rx_valid_pkt)
        },
        {
            .name = "rx_crc_error_pkt",
            .help = "Received CRC error packets",
            .size = sizeof(uint32_t),
            .type = bdmf_attr_number,
            .offset = offsetof(rdpa_port_stat_t, rx_crc_error_pkt)
        },
        {
            .name = "rx_discard_1",
            .help = "RX discard 1",
            .size = sizeof(uint32_t),
            .type = bdmf_attr_number,
            .offset = offsetof(rdpa_port_stat_t, rx_discard_1)
        },
        {
            .name = "rx_discard_2",
            .help = "RX discard 2",
            .size = sizeof(uint32_t),
            .type = bdmf_attr_number,
            .offset = offsetof(rdpa_port_stat_t, rx_discard_2)
        },
        {
            .name = "bbh_drop_1",
            .help = "BBH drop 1",
            .size = sizeof(uint32_t),
            .type = bdmf_attr_number,
            .offset = offsetof(rdpa_port_stat_t, bbh_drop_1)
        },
        {
            .name = "bbh_drop_2",
            .help = "BBH drop 2",
            .size = sizeof(uint32_t),
            .type = bdmf_attr_number,
            .offset = offsetof(rdpa_port_stat_t, bbh_drop_2)
        },
        {
            .name = "bbh_drop_3",
            .help = "BBH drop 3",
            .size = sizeof(uint32_t),
            .type = bdmf_attr_number,
            .offset = offsetof(rdpa_port_stat_t, bbh_drop_3)
        },
        {
            .name = "rx_discard_max_length",
            .help = "Oversize packets discard",
            .size = sizeof(uint32_t),
            .type = bdmf_attr_number,
            .offset = offsetof(rdpa_port_stat_t, rx_discard_max_length)
        },
        {
            .name = "rx_discard_min_length",
            .help = "Undersize packets discard",
            .size = sizeof(uint32_t),
            .type = bdmf_attr_number,
            .offset = offsetof(rdpa_port_stat_t, rx_discard_min_length)
        },
        {
            .name = "tx_valid_pkt",
            .help = "Valid transmitted packets",
            .size = sizeof(uint32_t),
            .type = bdmf_attr_number,
            .offset = offsetof(rdpa_port_stat_t, tx_valid_pkt)
        },
        {
            .name = "tx_discard",
            .help = "TX packets discarded (TX FIFO full or tx error)",
            .size = sizeof(uint32_t),
            .type = bdmf_attr_number,
            .offset = offsetof(rdpa_port_stat_t, tx_discard)
        },
        {
            .name = "discard_pkt",
            .help = "Filtered discard packets",
            .size = sizeof(uint32_t),
            .type = bdmf_attr_number,
            .offset = offsetof(rdpa_port_stat_t, discard_pkt)
        },
        {
            .name = "rx_bytes",
            .help = "Fttdp only: Received valid bytes",
            .size = sizeof(uint32_t),
            .type = bdmf_attr_number,
            .offset = offsetof(rdpa_port_stat_t, rx_valid_bytes)
        },
        {
            .name = "rx_multicast_pkt",
            .help = "Fttdp only: Received multicast packets",
            .size = sizeof(uint32_t),
            .type = bdmf_attr_number,
            .offset = offsetof(rdpa_port_stat_t, rx_multicast_pkt)
        },
        {
            .name = "rx_broadcast_pkt",
            .help = "Fttdp only: Received broadcast packets",
            .size = sizeof(uint32_t),
            .type = bdmf_attr_number,
            .offset = offsetof(rdpa_port_stat_t, rx_broadcast_pkt)
        },
        {
            .name = "tx_bytes",
            .help = "Fttdp only: Sent valid bytes",
            .size = sizeof(uint32_t),
            .type = bdmf_attr_number,
            .offset = offsetof(rdpa_port_stat_t, tx_valid_bytes)
        },
        {
            .name = "tx_multicast_pkt",
            .help = "Fttdp only: Sent multicast packets",
            .size = sizeof(uint32_t),
            .type = bdmf_attr_number,
            .offset = offsetof(rdpa_port_stat_t, tx_multicast_pkt)
        },
        {
            .name = "tx_broadcast_pkt",
            .help = "Fttdp only: Sent broadcast packets",
            .size = sizeof(uint32_t),
            .type = bdmf_attr_number,
            .offset = offsetof(rdpa_port_stat_t, tx_broadcast_pkt)
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE(port_stat_type);

/* Flow control ranges are same as in former GMP */
#define FLOW_CTL_MAX_RATE 1048576000 /* 1GB with fw granularity */
#define FLOW_CTL_MAX_BURST_SIZE 16777215
#define FLOW_CTL_MAX_THRESHOLD 8388607

/* port_flow_control aggregate type */
struct bdmf_aggr_type port_flow_control_type =
{
    .name = "port_flow_control",
    .struct_name = "rdpa_port_flow_ctrl_t",
    .help = "Port flow control configuration",
    .extra_flags = BDMF_ATTR_UNSIGNED,
    .fields = (struct bdmf_attr[])
    {
        {
            .name = "rate",
            .help = " limit rate [bits/sec]",
            .type = bdmf_attr_number,
            .min_val = 0,
            .max_val = FLOW_CTL_MAX_RATE,
            .size = sizeof(uint32_t),
            .offset = offsetof(rdpa_port_flow_ctrl_t, rate)
        },
        {
            .name = "burst_size",
            .help = "(MBS) specifies maximal burst size in bytes, Maximal size is 2^24-1.",
            .type = bdmf_attr_number,
            .min_val = 0,
            .max_val = FLOW_CTL_MAX_BURST_SIZE,
            .size = sizeof(uint32_t),
            .offset = offsetof(rdpa_port_flow_ctrl_t, mbs)
        },
        {
            .name = "fc_thresh",
            .help = "Flow control threshold (FCT) [bytes], value must be smaller than 1/2MBS",
            .type = bdmf_attr_number,
            .min_val = 0,
            .max_val = FLOW_CTL_MAX_THRESHOLD,
            .size = sizeof(uint32_t),
            .offset = offsetof(rdpa_port_flow_ctrl_t, threshold)
        },
        {
            .name = "src_address",
            .help = "Flow control mac source address for pause packets",
            .type = bdmf_attr_ether_addr,
            .size = sizeof(bdmf_mac_t),
            .offset = offsetof(rdpa_port_flow_ctrl_t, src_address)
        },
        {   .name = "traffic_types", 
            .help = "Flow Control Traffic Types", 
            .type = bdmf_attr_enum_mask,
            .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG,
            .ts.enum_table = &rdpa_fc_traffic_fields_enum_table,
            .size = sizeof(rdpa_flow_control_traffic), 
            .offset = offsetof(rdpa_port_flow_ctrl_t , traffic_types),
        },
        {
            .name = "policer",
            .help = " referenced policer for flow control values",
            .type = bdmf_attr_object,
            .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG,
            .size = sizeof(bdmf_object_handle),
            .offset = offsetof(rdpa_port_flow_ctrl_t, policer)
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE(port_flow_control_type);

/* port_mirror_cfg aggregate type */
struct bdmf_aggr_type port_mirror_cfg_type =
{
    .name = "port_mirror_cfg",
    .struct_name = "rdpa_port_mirror_cfg_t",
    .help = "Port mirroring configuration",
    .fields = (struct bdmf_attr[])
    {
        {
            .name = "rx_dst_port",
            .help = "RX destination port mirroring",
            .type = bdmf_attr_object,
            .size = sizeof(bdmf_object_handle),
            .offset = offsetof(rdpa_port_mirror_cfg_t, rx_dst_port)
        },
        {
            .name = "tx_dst_port",
            .help = "TX destination port mirroring",
            .type = bdmf_attr_object,
            .size = sizeof(bdmf_object_handle),
            .offset = offsetof(rdpa_port_mirror_cfg_t, tx_dst_port)
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE(port_mirror_cfg_type);

/* port_mirror_cfg aggregate type */
struct bdmf_aggr_type port_vlan_isolation_cfg_type =
{
    .name = "vlan_isolation",
    .struct_name = "rdpa_port_vlan_isolation_t",
    .help = "LAN Port VLAN isolation control",
    .fields = (struct bdmf_attr[])
    {
        {
            .name = "us",
            .help = "Ingress LAN port VLAN isolation control",
            .type = bdmf_attr_boolean,
            .size = sizeof(bdmf_boolean),
            .offset = offsetof(rdpa_port_vlan_isolation_t, us)
        },
        {
            .name = "ds",
            .help = "Egress LAN port VLAN isolation control",
            .type = bdmf_attr_boolean,
            .size = sizeof(bdmf_boolean),
            .offset = offsetof(rdpa_port_vlan_isolation_t, ds)
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE(port_vlan_isolation_cfg_type);

/* 'rdpa_loopback type': Enumeration table */
static const bdmf_attr_enum_table_t rdpa_lb_type_enum_table =
{
    .type_name = "rdpa_lb_type_enum_table",
    .help = "Loopback type",
    .values =
    {
        { "none", rdpa_loopback_type_none },
        { "fw", rdpa_loopback_type_fw },
        { "mac", rdpa_loopback_type_mac },
        { "phy", rdpa_loopback_type_phy },
        { NULL, 0 }
    }
};

/* 'rdpa_loopback operation': Enumeration table */
static const bdmf_attr_enum_table_t rdpa_lb_operation_enum_table =
{
    .type_name = "rdpa_lb_operation_enum_table",
    .help = "Loopback operation",
    .values =
    {
        { "none", rdpa_loopback_op_none },
        { "local", rdpa_loopback_op_local },
        { "remote", rdpa_loopback_op_remote },
        { NULL, 0 }
    }
};

/* 'port_loopback_type' */
struct bdmf_aggr_type port_loopback_conf =
{
    .name = "port_loopback_conf",
    .struct_name = "rdpa_port_loopback_t",
    .help = "Port loopback configuration",
    .fields = (struct bdmf_attr[])
    {
        {
            .name = "type",
            .help = "Loopback type",
            .type = bdmf_attr_enum,
            .ts.enum_table = &rdpa_lb_type_enum_table,
            .size = sizeof(rdpa_loopback_type),
            .offset = offsetof(rdpa_port_loopback_t, type)
        },
        {
            .name = "op",
            .help = "Operation",
            .type = bdmf_attr_enum,
            .ts.enum_table = &rdpa_lb_operation_enum_table,
            .size = sizeof(rdpa_loopback_op),
            .offset = offsetof(rdpa_port_loopback_t, op)
        },
        {
            .name = "wan_flow",
            .help = "GEM/LLID for WAN-WAN loopback",
            .type = bdmf_attr_number,
            .size = sizeof(int32_t),
            .offset = offsetof(rdpa_port_loopback_t, wan_flow)
        },
        {
            .name = "queue",
            .help = "Queue id for firmware loopback",
            .type = bdmf_attr_number,
            .size = sizeof(int32_t),
            .offset = offsetof(rdpa_port_loopback_t, queue)
        },
        BDMF_ATTR_LAST
    }
};
DECLARE_BDMF_AGGREGATE_TYPE(port_loopback_conf);

/* Object attribute descriptors */
static struct bdmf_attr port_attrs[] =
{
    {
        .name = "index", .help = "Port index", .type = bdmf_attr_enum,
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE_INIT | BDMF_ATTR_CONFIG | BDMF_ATTR_KEY | BDMF_ATTR_MANDATORY,
        .ts.enum_table = &rdpa_if_enum_table, .size = sizeof(rdpa_if), .offset = offsetof(port_drv_priv_t, index),
        .min_val = rdpa_if_wan0, .max_val = rdpa_if__number_of - 1
    },
    {
        .name = "wan_type",
        .help = "Wan type",
        .type = bdmf_attr_enum,
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE_INIT | BDMF_ATTR_CONFIG,
        .ts.enum_table = &rdpa_wan_type_enum_table,
        .size = sizeof(bdmf_attr_enum),
        .offset = offsetof(port_drv_priv_t, wan_type),
        .read = port_attr_wan_type_read,
        .write = port_attr_wan_type_write,
    },
    {
        .name = "speed",
        .help = "Active Ethernet Speed",
        .type = bdmf_attr_enum,
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE_INIT | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG,
        .ts.enum_table = &rdpa_speed_type_enum_table,
        .size = sizeof(bdmf_attr_enum),
        .offset = offsetof(port_drv_priv_t, speed),
        .write = port_attr_speed_write,
    },
    {
        .name = "cfg", .help = "Logical port configuration", .type = bdmf_attr_aggregate,
        .ts.aggr_type_name = "port_dp", .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG,
        .size = sizeof(rdpa_port_dp_cfg_t), .offset = offsetof(port_drv_priv_t, cfg),
        .write = port_attr_cfg_write
    },
    {
        .name = "tm_cfg", .help = "TM configuration", .type = bdmf_attr_aggregate, .ts.aggr_type_name = "port_tm",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG,
        .size = sizeof(rdpa_port_tm_cfg_t), .offset = offsetof(port_drv_priv_t, tm_cfg),
        .write = port_attr_tm_cfg_write
    },
    {
        .name = "sa_limit", .help = "SA limit configuration", .type = bdmf_attr_aggregate,
        .ts.aggr_type_name = "port_sa_limit", .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG,
        .offset = offsetof(port_drv_priv_t, sa_limit), .write = port_attr_sa_limit_write
    },
    {
        .name = "def_flow",
        .help = "DS default flow classification. Wifi: last default flow will be applied for all wifi ports",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG | BDMF_ATTR_NO_NULLCHECK,
        .type = bdmf_attr_aggregate, .ts.aggr_type_name = "classification_result",
        .write = port_attr_def_flow_write, .read = port_attr_def_flow_read
    },
    {
        .name = "stat", .help = "Port statistics", .type = bdmf_attr_aggregate, .ts.aggr_type_name = "stat",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_STAT,
        .read = port_attr_stat_read, .write = port_attr_stat_write
    },
    {
        .name = "flow_control", .help = "The port flow control", .type = bdmf_attr_aggregate,
        .ts.aggr_type_name = "port_flow_control", .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG,
        .size = sizeof(rdpa_port_flow_ctrl_t), .offset = offsetof(port_drv_priv_t, flow_ctrl),
        .write = port_attr_flow_control_write
    },
    {
        .name = "mirror_cfg", .help = "Port mirroring configuration", .type = bdmf_attr_aggregate,
        .ts.aggr_type_name = "port_mirror_cfg", .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG,
        .size = sizeof(rdpa_port_mirror_cfg_t), .offset = offsetof(port_drv_priv_t, mirror_cfg),
        .write = port_attr_mirror_cfg_write
    },
    {
        .name = "vlan_isolation", .help = "LAN port VLAN isolation control", .type = bdmf_attr_aggregate,
        .ts.aggr_type_name = "vlan_isolation", .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG,
        .size = sizeof(rdpa_port_vlan_isolation_t), .offset = offsetof(port_drv_priv_t, vlan_isolation),
        .write = port_vlan_isolation_cfg_write, .read = port_vlan_isolation_cfg_read
    },
    {
        .name = "transparent", .help = "LAN port transparent control", .type = bdmf_attr_boolean,
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG,
        .size = sizeof(bdmf_boolean), .offset = offsetof(port_drv_priv_t, transparent),
        .write = port_transparent_cfg_write
    },
    /* loopbacks */
    {
        .name = "loopback", .help = "Port loopbacks", .ts.aggr_type_name = "port_loopback_conf",
        .type = bdmf_attr_aggregate, .size = sizeof(rdpa_port_loopback_t),
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG,
        .write = port_attr_loopback_write, .read = port_attr_loopback_read
    },
    {
        .name = "mtu_size", .help = "Port MTU",
        .type = bdmf_attr_number, .min_val = RDPA_MIN_MTU, .size = sizeof(uint32_t),
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG,
        .write = port_attr_mtu_size_write, .read = port_attr_mtu_size_read
    },
    {
        .name = "cpu_obj", .help = "CPU object for exception/forwarding-through-cpu packets",
        .type = bdmf_attr_object, .ts.ref_type_name = "cpu", .size = sizeof(bdmf_object_handle),
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG,
        .offset = offsetof(port_drv_priv_t, cpu_obj),
        .write = port_attr_cpu_obj_write_ex
    },
    {
        .name = "cpu_meter", .help = "Index of per-port CPU meter",
        .type = bdmf_attr_number, .size = sizeof(bdmf_index),
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG,
        .offset = offsetof(port_drv_priv_t, cpu_meter),
        .min_val = BDMF_INDEX_UNASSIGNED, .max_val = RDPA_CPU_MAX_METERS - 1,
        .write = port_attr_cpu_meter_write
    },
    {
        .name = "ingress_filter", .help = "Ingress filter configuration per Port object",
        .array_size = RDPA_FILTERS_QUANT,
        .index_type = bdmf_attr_enum, .index_ts.enum_table = &rdpa_filter_enum_table,
        .type = bdmf_attr_aggregate, .ts.aggr_type_name = "filter_ctrl",
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG,
        .read = port_attr_ingress_filter_read, .write = port_attr_ingress_filter_write,
        .get_next = port_attr_ingress_filter_get_next
    },
    {
        .name = "protocol_filters", .help = "Protocol Filters define allowed traffic type",
        .type = bdmf_attr_enum_mask, .ts.enum_table = &rdpa_protocol_filters_table,
        .flags = BDMF_ATTR_READ | BDMF_ATTR_WRITE | BDMF_ATTR_CONFIG,
        .size = sizeof(rdpa_proto_filters_mask_t), .offset = offsetof(port_drv_priv_t, proto_filters),
        .write = port_attr_proto_filters_write_ex, .data_type_name = "rdpa_proto_filters_mask_t"
    },

    BDMF_ATTR_LAST
};

static int port_drv_init(struct bdmf_type *drv);
static void port_drv_exit(struct bdmf_type *drv);

struct bdmf_type port_drv =
{
    .name = "port",
    .parent = "system",
    .description = "Physical or Virtual Interface",
    .drv_init = port_drv_init,
    .drv_exit = port_drv_exit,
    .pre_init = port_pre_init,
    .post_init = port_post_init,
    .destroy = port_destroy,
    .link_up = port_attr_link_port,
    .unlink_up = port_attr_unlink_port,
    .ref_changed = port_ref_changed,
    .extra_size = sizeof(port_drv_priv_t),
    .aattr = port_attrs,
    .flags = BDMF_DRV_FLAG_MUXUP | BDMF_DRV_FLAG_MUXDOWN,
    .max_objs = rdpa_if__number_of,
};
DECLARE_BDMF_TYPE(rdpa_port, port_drv);

rdpa_if rdpa_port_map_from_hw_port(int hw_port, bdmf_boolean emac_only)
{
    struct bdmf_object *mo = NULL;
    port_drv_priv_t *port_priv;
    rdpa_if port, max_port;

    max_port = emac_only ? rdpa_if_lan_max : rdpa_if_switch;

    bdmf_fastlock_lock(&port_fastlock);
    for (port = rdpa_if_first; port <= max_port; port++)
    {
        mo = port_objects[port];
        if (!mo)
            continue;

        port_priv = (port_drv_priv_t *)bdmf_obj_data(mo);
        if (rdpa_is_ext_switch_mode() && rdpa_if_is_lan(port))
        {
            if ((port_priv->cfg.physical_port) == hw_port)
                goto exit;
        }
        else if (port_priv->cfg.emac == hw_port)
            goto exit;
    }
    port = rdpa_if_none;
exit:
    bdmf_fastlock_unlock(&port_fastlock);
    return port;
}
#ifndef BDMF_DRIVER_GPL_LAYER
EXPORT_SYMBOL(rdpa_port_map_from_hw_port);
#else
extern rdpa_if (*f_rdpa_port_map_from_hw_port)(int hw_port, bdmf_boolean emac_only);
#endif

/* Init/exit module. Cater for GPL layer */
static int port_drv_init(struct bdmf_type *drv)
{
#ifdef BDMF_DRIVER_GPL_LAYER
    f_rdpa_port_drv = rdpa_port_drv;
    f_rdpa_port_get = rdpa_port_get;
    f_rdpa_port_map_from_hw_port = rdpa_port_map_from_hw_port;
    f_rdpa_physical_port_to_rdpa_if = rdpa_physical_port_to_rdpa_if;
#endif
#if defined(BCM63158)
    physical_port_to_rdpa_if[0] = rdpa_if_lan0;
    physical_port_to_rdpa_if[1] = rdpa_if_lan1;
    physical_port_to_rdpa_if[2] = rdpa_if_lan2;
    physical_port_to_rdpa_if[3] = rdpa_if_lan3;
    physical_port_to_rdpa_if[4] = rdpa_if_lan4;
    physical_port_to_rdpa_if[5] = rdpa_if_none;
    physical_port_to_rdpa_if[6] = rdpa_if_lan5;
    physical_port_to_rdpa_if[7] = rdpa_if_none;
#endif
    return 0;
}

static void port_drv_exit(struct bdmf_type *drv)
{
#ifdef BDMF_DRIVER_GPL_LAYER
    f_rdpa_port_drv = NULL;
    f_rdpa_port_get = NULL;
    f_rdpa_port_map_from_hw_port = NULL;
    f_rdpa_physical_port_to_rdpa_if = NULL;
#endif
}

/*
 * Internal functions
 */

/* Increment/decrement num_sa on port.
 * The operation can fail if port doesn't exist
 * or num_sa exceeds max_sa.
 *
 * The function must be called under lock!
 * \return 0 if OK or error code < 0
 */
int rdpa_port_inc_dec_num_sa(rdpa_if index, bdmf_boolean is_inc)
{
    struct bdmf_object *mo;
    port_drv_priv_t *port;
    int rc = 0;

    bdmf_fastlock_lock(&port_fastlock);
    mo = port_objects[index];
    if (!mo)
    {
        bdmf_fastlock_unlock(&port_fastlock);
        return BDMF_ERR_NOENT;
    }
    port = (port_drv_priv_t *)bdmf_obj_data(mo);
    if (is_inc)
    {
        if (port->sa_limit.max_sa && port->sa_limit.num_sa >= port->sa_limit.max_sa)
        {
            rc = BDMF_ERR_TOO_MANY;
            goto exit;
        }
        ++port->sa_limit.num_sa;
    }
    else
    {
        BUG_ON(!port->sa_limit.num_sa);
        --port->sa_limit.num_sa;
    }

exit:
    bdmf_fastlock_unlock(&port_fastlock);
    return rc;
}

bdmf_boolean rdpa_if_is_active(rdpa_if port)
{
    if ((unsigned)port >= rdpa_if__number_of)
        return 0;
    return port_objects[port] != NULL;
}

unsigned long rdpa_get_switch_lag_port_mask()
{
    return rdpa_lag_mask;
}

int rdpa_port_transparent(rdpa_if index, bdmf_boolean *enable)
{
    struct bdmf_object *mo;
    port_drv_priv_t *port;
    int rc = 0;

    bdmf_fastlock_lock(&port_fastlock);
    mo = port_objects[index];
    if (!mo)
    {
        bdmf_fastlock_unlock(&port_fastlock);
        return BDMF_ERR_NOENT;
    }
    port = (port_drv_priv_t *)bdmf_obj_data(mo);
    if (port->transparent)
        *enable = 1;
    else
        *enable = 0;

    bdmf_fastlock_unlock(&port_fastlock);
    return rc;
}

int _rdpa_port_set_linked_bridge(rdpa_if index, bdmf_object_handle bridge_obj)
{
    port_drv_priv_t *port;
    struct bdmf_object *mo;
    int rc;

    mo = port_objects[index];
    port = (port_drv_priv_t *)bdmf_obj_data(mo);
    rc = _rdpa_port_set_linked_bridge_ex(port, bridge_obj);
    if (rc)
        return rc;

    port->bridge_obj = bridge_obj;
    return 0;
}

/***************************************************************************
 * Functions declared in auto-generated header
 **************************************************************************/

/** Get port object by key
 * \param[in] _index_       Object key
 * \param[out] port_obj     Object handle
 * \return  0=OK or error <0
 */
int rdpa_port_get(rdpa_if _index_, bdmf_object_handle *port_obj)
{
    struct bdmf_object *mo;
    if ((unsigned)_index_ >= rdpa_if__number_of)
        return BDMF_ERR_RANGE;
    mo = port_objects[_index_];
    if (mo)
    {
        bdmf_get(mo);
        *port_obj = mo;
        return 0;
    }
    return BDMF_ERR_NOENT;
}


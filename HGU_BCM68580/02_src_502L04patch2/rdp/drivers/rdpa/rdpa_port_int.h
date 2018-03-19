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

#ifndef _RDPA_PORT_INT_H_
#define _RDPA_PORT_INT_H_

#include "bdmf_dev.h"
#include "bdmf_errno.h"
#include "rdpa_api.h"
#include "rdpa_int.h"
#include "rdpa_policer_ex.h"
#ifndef XRDP
#include "rdp_drv_ih.h"
#include "rdp_drv_bbh.h"
#else
#include "rdp_drv_bbh_tx.h"
#include "rdp_drv_bbh_rx.h"
#include "rdp_drv_rnr.h"
#endif
#include "rdd.h"
#include "rdd_data_structures_auto.h"

extern const bdmf_attr_enum_table_t rdpa_physical_port_enum_table;

#define EXCEPTION_RX_MIRROR (1 << 0)
#define EXCEPTION_PORT_FW_LOOPBACK (1 << 1)
#define EXCEPTION_PORT_WAN_GBE (1 << 2)
/* port object private data */
typedef struct
{
    rdpa_if index; /**< port index */
    rdpa_wan_type wan_type; /**< WAN type */
    rdpa_speed_type speed; /**< Ethernet speed for epon mode, must be set after /ref ae_enable */
    rdpa_port_dp_cfg_t cfg; /**< Logical configuration */
    rdpa_port_tm_cfg_t tm_cfg; /**< TM configuration */
    rdpa_ic_result_t default_cfg; /**< Default configuration */
    bdmf_boolean default_cfg_exist; /**< flag for post_init*/
    rdpa_port_sa_limit_t sa_limit; /**< SA limit configuration */
    rdpa_port_flow_ctrl_t flow_ctrl; /**< Ingress flow control */
    bdmf_index def_flow_index;  /**index of def flow context configuration */
    rdpa_port_mirror_cfg_t mirror_cfg; /**< Port mirroring configuration */
    rdpa_stat_tx_rx_t host_stat;  /**< Host statistics shadow */
    rdpa_port_vlan_isolation_t vlan_isolation;  /**<Port VLAN isolation control */
    bdmf_boolean transparent;  /**<Port  transparent control */
    int channel;        /* port channel (RDD) */
    rdpa_port_loopback_t  loopback_cfg; /* port loopbacks configuration */
    uint32_t mtu_size;  /*port mtu size*/
    bdmf_object_handle cpu_obj; /**< CPU object for exception/forwarding-through-cpu packets */
    bdmf_index cpu_meter; /**< Index of per-port CPU meter. If set, supersedes per-reason meter configuration */
    uint32_t exception_flags;
    bdmf_object_handle bridge_obj; /**< Bridge object that is linked to port */
    rdpa_filter_ctrl_t ingress_filters[RDPA_FILTERS_QUANT]; /** Ingress filters per port */
    uint8_t ingress_filters_profile; /**< Profile ID for internal usage, \XRDP_LIMITED */
    uint32_t proto_filters; /**< Map of rdpa_proto_filtres_t */
} port_drv_priv_t;

extern bdmf_fastlock port_fastlock;
extern struct bdmf_object *port_objects[];

static inline int _rdpa_port_get_locked(rdpa_if index,
    port_drv_priv_t **port)
{
    struct bdmf_object *mo;
    if ((unsigned)index >= rdpa_if__number_of)
        return -1;
    bdmf_fastlock_lock(&port_fastlock);
    mo = port_objects[index];
    if (!mo)
    {
        bdmf_fastlock_unlock(&port_fastlock);
        return -1;
    }
    *port = (port_drv_priv_t *)bdmf_obj_data(mo);
    return 0;
}

static inline int _rdpa_port_channel(rdpa_if index)
{
    port_drv_priv_t *port;
    int channel, rc;

    rc = _rdpa_port_get_locked(index, &port);
    if (rc)
        return -1;
    channel = port->channel;
    bdmf_fastlock_unlock(&port_fastlock);
    return channel;
}

static inline int _rdpa_port_channel_no_lock(rdpa_if index)
{
    struct bdmf_object *mo;
    port_drv_priv_t *port;

    if ((unsigned)index >= rdpa_if__number_of)
        return -1;
    mo = port_objects[index];
    if (!mo)
        return -1;
    port = (port_drv_priv_t *)bdmf_obj_data(mo);
    return port->channel;
}

static inline bdmf_object_handle _rdpa_port_get_linked_bridge(rdpa_if index)
{
    struct bdmf_object *mo;
    port_drv_priv_t *port;

    if ((unsigned)index >= rdpa_if__number_of)
        return NULL;
    mo = port_objects[index];
    if (!mo)
        return NULL;
    port = (port_drv_priv_t *)bdmf_obj_data(mo);
    return port->bridge_obj;
}

int rdpa_update_da_sa_searches(rdpa_if port, bdmf_boolean dal);
#ifndef XRDP
void update_port_tag_size(rdpa_emac emac, DRV_IH_PROPRIETARY_TAG_SIZE new_tag_size);
#else
void update_port_tag_size(rdpa_emac emac, drv_rnr_prop_tag_size_t new_tag_size);
void update_broadcom_tag_size(void);
int port_def_flow_cntr_add(port_drv_priv_t *port);
void port_flow_add(rdpa_if if_vport);
void port_flow_del(port_drv_priv_t *port);
#endif
int rdpa_if_to_rdpa_physical_port(rdpa_if port, rdpa_physical_port *physical_port);
int port_attr_mtu_size_read(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    void *val, uint32_t size);
int port_attr_mtu_size_write(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    const void *val, uint32_t size);

bdmf_error_t rdpa_port_tm_discard_prty_cfg_ex(struct bdmf_object *mo, rdpa_port_tm_cfg_t *tm_cfg);

int port_attr_stat_read(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index, void *val,
    uint32_t size);
int port_attr_stat_write(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index, const void *val,
    uint32_t size);

int port_ls_fc_cfg_ex(struct bdmf_object *mo, rdpa_port_dp_cfg_t *cfg);
int port_mirror_cfg_ex(struct bdmf_object *mo, port_drv_priv_t *port, rdpa_port_mirror_cfg_t *mirror_cfg);
int port_attr_loopback_write_ex(struct bdmf_object *mo,
    struct bdmf_attr *ad, bdmf_index index, const void *val, uint32_t size);
int port_vlan_isolation_cfg_ex(port_drv_priv_t *port,
    rdpa_port_vlan_isolation_t *vlan_isolation_cfg, bdmf_boolean is_active);
int port_flow_control_cfg_ex(port_drv_priv_t *port, rdpa_port_flow_ctrl_t *flow_ctrl);
int port_set_bbh_timer_clock_ex(port_drv_priv_t *port, policer_drv_priv_t *policer);
int port_post_init_ex(struct bdmf_object *mo);
int port_attr_wan_type_write_ex(port_drv_priv_t *port, rdpa_wan_type wan_type);
int port_attr_cpu_obj_write_ex(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index, const void *val,
    uint32_t size);
int port_attr_cpu_meter_write_ex(struct bdmf_object *mo, rdpa_traffic_dir dir, rdpa_if intf, bdmf_index meter_idx);

int rdpa_cfg_sa_da_lookup(port_drv_priv_t *port, rdpa_port_dp_cfg_t *cfg, bdmf_boolean old_sa_action,
    bdmf_boolean is_active);
int port_update_all_ports_set(bdmf_object_handle mo, int is_add);

int rdpa_cfg_sa_da_lookup_ex(port_drv_priv_t *port, rdpa_port_dp_cfg_t *cfg);

/* rdpa_if <-> rdpa_emac */
rdpa_if rdpa_port_emac_to_rdpa_if(rdpa_emac emac);
rdpa_emac rdpa_port_rdpa_if_to_emac(rdpa_if port);

/* rdpa_if <-> rdd_vport_id_t */
rdd_vport_id_t rdpa_port_rdpa_if_to_vport(rdpa_if port);
rdpa_if rdpa_port_vport_to_rdpa_if(rdd_vport_id_t rdd_vport);
#ifndef XRDP
rdpa_if rdd_vport_to_rdpa_if(rdd_vport_id_t vport, uint8_t wifi_ssid);
rdd_vport_id_t rdpa_if_to_rdd_vport(rdpa_if port, uint8_t *rdd_wifi_ssid);
#else
int update_port_bridge_and_vlan_lookup_method_ex(rdpa_if port);
rdd_vport_id_t rdpa_if_to_rdd_vport(rdpa_if port, rdpa_wan_type wan_type);
#endif

#ifndef XRDP
uint32_t rdpa_port_ports2rdd_ssid_vector(rdpa_ports ports);
rdpa_ports rdpa_port_ssid_vector2rdpa_ports(uint32_t ssid_vector);
#endif

int rdpa_port_lag_link_ex(port_drv_priv_t *lag_port);
void rdpa_port_lag_unlink_ex(port_drv_priv_t *lag_port);
int rdpa_port_bond_link_ex(rdpa_physical_port physical_port);
int rdpa_port_bond_unlink_ex(rdpa_physical_port physical_port);

int rdpa_port_get_egress_tm_channel_from_port_ex(rdpa_if port, int *channel_id);

#ifdef XRDP
uint32_t rdpa_port_rx_flow_src_port_get(rdpa_if port, int set_lan_indication);
uint32_t disabled_proto_mask_get(uint32_t proto_filters_mask);
int port_update_hash_port_and_vid0(port_drv_priv_t *port, void *modify_cb, void *modify_ctx);
#endif

int port_attr_ingress_filter_write_ex(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    const void *val, uint32_t size);
int port_attr_proto_filters_write_ex(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    const void *val, uint32_t size);

int _rdpa_port_set_linked_bridge(rdpa_if index, bdmf_object_handle bridge_obj);
int _rdpa_port_set_linked_bridge_ex(port_drv_priv_t *port, bdmf_object_handle bridge_obj);

void rdpa_port_dp_cfg2mac_lkp_cfg(rdpa_port_dp_cfg_t *cfg,  rdpa_mac_lookup_cfg_t *mac_lkp_cfg);
int mac_lkp_cfg_validate_ex(rdpa_mac_lookup_cfg_t *mac_lkp_cfg, port_drv_priv_t *port, int ls_fc_enable);

#endif


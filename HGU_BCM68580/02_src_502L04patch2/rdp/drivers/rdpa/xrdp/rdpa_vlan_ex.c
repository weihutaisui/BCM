/*
* <:copyright-BRCM:2012-2015:proprietary:standard
* 
*    Copyright (c) 2012-2015 Broadcom 
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
* :>
*/

/*
 * rdpa_vlan_ex.c
 *
 *  Created on: oct 26, 2016
 *      Author: danielc
 */

#include "rdpa_bridge_ex.h"
#include "rdpa_vlan_ex.h"
#include "rdpa_port_int.h"
#include "rdpa_rdd_map.h"
#include "rdp_drv_hash.h"
#include "rdd_ag_processing.h"
#include "rdd_bridge.h"
#ifdef INGRESS_FILTERS
#include "rdpa_filter_ex.h"
#endif
#include "rdp_drv_rnr.h"

#define NUM_OF_PARSER_VID_FILTERS 8
static int16_t prty_vids[NUM_OF_PARSER_VID_FILTERS] = {
    BDMF_INDEX_UNASSIGNED, BDMF_INDEX_UNASSIGNED, BDMF_INDEX_UNASSIGNED, BDMF_INDEX_UNASSIGNED,
    BDMF_INDEX_UNASSIGNED, BDMF_INDEX_UNASSIGNED, BDMF_INDEX_UNASSIGNED, BDMF_INDEX_UNASSIGNED
};

static int total_vids;

int vlan_lan_to_wan_link_ex(struct bdmf_object *mo, struct bdmf_object *other)
{
    vlan_drv_priv_t *this_vlan = (vlan_drv_priv_t *)bdmf_obj_data(mo);
    vlan_drv_priv_t *other_vlan = (vlan_drv_priv_t *)bdmf_obj_data(other);
    bridge_drv_priv_t *bridge;

    if (!this_vlan->linked_bridge)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_INVALID_OP, mo, "lan vlan %s is not linked to any bridge", this_vlan->name);
    if (!other_vlan->linked_bridge)
        BDMF_TRACE_RET_OBJ(BDMF_ERR_INVALID_OP, mo, "wan vlan %s is not linked to any bridge", other_vlan->name);
    if (this_vlan->linked_bridge != other_vlan->linked_bridge)
    {
        BDMF_TRACE_RET_OBJ(BDMF_ERR_INVALID_OP, mo, 
            "lan vlan %s and wan vlan %s are not linked to the same bridge", this_vlan->name, other_vlan->name);
    }
    bridge = (bridge_drv_priv_t *)bdmf_obj_data(this_vlan->linked_bridge);
    if (!bridge->cfg.auto_aggregate)
    {
        BDMF_TRACE_RET_OBJ(BDMF_ERR_INVALID_OP, mo, 
            "lan vlan %s and wan vlan %s are not linked to an aggregated enabled bridge/index=%d", 
            this_vlan->name, other_vlan->name, (int)bridge->index);
    }
    return vlan_update_aggr_all_vids(mo, other, 1);
}

void vlan_lan_to_wan_unlink_ex(struct bdmf_object *mo, struct bdmf_object *other)
{
    BDMF_TRACE_DBG("Arrived at vlan_lan_to_wan_unlink_ex\n");
    vlan_update_aggr_all_vids(mo, other, 0);
}

static int vlan_wan_aggr_add_to_rdd_sanity(struct bdmf_object *lan_vlan_obj, int16_t lan_vid, int16_t wan_vid)
{
    vlan_drv_priv_t *lan_vlan = (vlan_drv_priv_t *)bdmf_obj_data(lan_vlan_obj);
    bridge_drv_priv_t *bridge;

    /* Paranoya check: lan obj linked to bridge?*/
    if (lan_vlan->linked_bridge == NULL)
    {
        BDMF_TRACE_ERR_OBJ(lan_vlan_obj, 
            "Error modifying aggregation for vid %d, object not linked to bridge\n", lan_vid);
        return BDMF_ERR_NOT_LINKED;
    }
    bridge = (bridge_drv_priv_t *)bdmf_obj_data(lan_vlan->linked_bridge);

    /* Paranoya check: wan vlan linked to bridge?*/
    if (bridge->wan_vid == BDMF_INDEX_UNASSIGNED)
    {
        BDMF_TRACE_ERR_OBJ(lan_vlan_obj, 
            "Error modifying aggregation for vid %d, no WAN VLAN is linked to bridge\n", lan_vid);
        return BDMF_ERR_NOT_LINKED;
    }

    /* Paranoya check: wan vid in bridge configured correctly */
    if (bridge->wan_vid != wan_vid)
    {
        BDMF_TRACE_ERR_OBJ(lan_vlan_obj, 
            "Error modifying aggregation for vid %d, mismatch of WAN vid %d"
            " and bridge WAN vid %d", lan_vid, wan_vid, (int)bridge->wan_vid);
        return BDMF_ERR_PARM;
    }
    return BDMF_ERR_OK;
}

static int vlan_wan_aggr_add_to_rdd(struct bdmf_object *lan_vlan_obj, int16_t lan_vid, int16_t wan_vid)
{
    vlan_drv_priv_t *lan_vlan = (vlan_drv_priv_t *)bdmf_obj_data(lan_vlan_obj);
    rdpa_if port = bridge_vlan_to_if(lan_vlan_obj);
    int vid_idx;
    int rc;
    uint8_t ena = 1;

    rc = vlan_wan_aggr_add_to_rdd_sanity(lan_vlan_obj, lan_vid, wan_vid);
    if (rc)
        return rc;

    BUG_ON(port > rdpa_if__number_of);
    vid_idx = vlan_get_vid_cfg_idx(lan_vlan, lan_vid);
    if (vid_idx < 0)
        BDMF_TRACE_RET(BDMF_ERR_PARM, "Unknown VLAN VID %d\n",  lan_vid); 

    rc = rdpa_vlan_hash_entry_modify(port, lan_vid, rdpa_vlan_wan_aggregation_modify_cb, &ena);
    if (rc)
        return rc;

    /* The configuration for aggregation is bi-directional, assuming that all ports in the bridge with auto-aggregation
     * enabled will be aggregated. */
    /* XXX: Alternatively we could store the aggregation bit in MAC context table, as this is the function of bridge, as
     * this would also allow us to support manual (=partial) aggregation. */
    rc = rdpa_vlan_hash_entry_modify(rdpa_if_wan0, wan_vid, rdpa_vlan_wan_aggregation_modify_cb, &ena);
    if (rc)
        return rc;

    return 0;
}

static void vlan_wan_aggr_delete_to_rdd(struct bdmf_object *lan_vlan_obj, int16_t lan_vid, int16_t wan_vid)
{
    vlan_drv_priv_t *lan_vlan = (vlan_drv_priv_t *)bdmf_obj_data(lan_vlan_obj);   
    rdpa_if port = bridge_vlan_to_if(lan_vlan_obj);
    int vid_idx;
    uint8_t ena = 0;

    if (lan_vlan->linked_bridge == NULL)
        return;

    BUG_ON(port > rdpa_if__number_of);

    vid_idx = vlan_get_vid_cfg_idx(lan_vlan, lan_vid);
    if (vid_idx < 0)
    {
        BDMF_TRACE_ERR("Unknown VLAN VID %d\n",  lan_vid);
        return;
    }

    rdpa_vlan_hash_entry_modify(port, lan_vid, rdpa_vlan_wan_aggregation_modify_cb, &ena);
}

static void bridge_and_vlan_ctx_mac_lkp_cfg_set(RDD_BRIDGE_AND_VLAN_LKP_RESULT_DTS *bridge_and_vlan_ctx_lkp_result,
    rdpa_mac_lookup_cfg_t *mac_lkp_cfg)
{
    bridge_and_vlan_ctx_lkp_result->sa_lookup_en = mac_lkp_cfg->sal_enable;
    bridge_and_vlan_ctx_lkp_result->sa_lookup_miss_action =
        rdpa_forward_action2rdd_action(mac_lkp_cfg->sal_miss_action);
    bridge_and_vlan_ctx_lkp_result->da_lookup_en = mac_lkp_cfg->dal_enable;
    bridge_and_vlan_ctx_lkp_result->da_lookup_miss_action =
        rdpa_forward_action2rdd_action(mac_lkp_cfg->dal_miss_action);
}

/* Configure port isolation map in hash entry according to vport mask (prior to updating in hash table) */
void rdpa_vlan_hash_entry_isolation_vector_set(rdd_vport_vector_t vport_mask,
    RDD_BRIDGE_AND_VLAN_LKP_RESULT_DTS *bridge_and_vlan_ctx_lkp_result)
{
    bridge_and_vlan_ctx_lkp_result->port_isolation_map_15_0 = (uint32_t)(vport_mask & 0xFFFF);
    bridge_and_vlan_ctx_lkp_result->port_isolation_map_23_16 = (uint32_t)((vport_mask >> 16) & 0xFF);
    bridge_and_vlan_ctx_lkp_result->port_isolation_map_31_24 = (uint32_t)((vport_mask >> 24) & 0xFF);
}

int rdpa_vlan_add_single_port_and_vid(rdpa_if port, int16_t vid, struct bdmf_object *bridge_obj,
    uint32_t proto_filters, rdpa_mac_lookup_cfg_t *mac_lkp_cfg, uint8_t ingress_filters_profile)
{
    RDD_BRIDGE_AND_VLAN_LKP_RESULT_DTS bridge_and_vlan_ctx_lkp_result = {};
    rdpa_ports isolation_vector = 0;
    rdd_vport_vector_t vport_mask;

    bridge_and_vlan_ctx_lkp_result.protocol_filters_dis = disabled_proto_mask_get(proto_filters); 

    bridge_and_vlan_ctx_mac_lkp_cfg_set(&bridge_and_vlan_ctx_lkp_result, mac_lkp_cfg);

    /* Can come with bridge if VLAN object exist (not during creation process) and either:
     * - linked to 802.1Q bridge 
     * - owned by port for which isolation is turned on */
    if (bridge_obj)
    {
        bridge_drv_priv_t *bridge = (bridge_drv_priv_t *)bdmf_obj_data(bridge_obj);
        isolation_vector = bridge->port_fw_elig[port];

        if (bridge->cfg.learning_mode == rdpa_bridge_learn_ivl)
        {
            /* MAC + VID. In this case, unless WAN VID is set, learning is not possible */
            bridge_and_vlan_ctx_lkp_result.arl_lookup_method = 1;
        }
        if (bridge->wan_vid != BDMF_INDEX_UNASSIGNED)
            bridge_and_vlan_ctx_lkp_result.wan_vid = bridge->wan_vid;
    }
    vport_mask = rdpa_ports_to_rdd_egress_port_vector(isolation_vector, 0);
    rdpa_vlan_hash_entry_isolation_vector_set(vport_mask, &bridge_and_vlan_ctx_lkp_result);

    bridge_and_vlan_ctx_lkp_result.ingress_filter_profile = ingress_filters_profile;

    return rdpa_vlan_hash_entry_write(port, vid, &bridge_and_vlan_ctx_lkp_result);
}

int vlan_vid_table_update_ex(struct bdmf_object *mo, int16_t vid, int is_add)
{
    vlan_drv_priv_t *vlan = (vlan_drv_priv_t *)bdmf_obj_data(mo);
    bdmf_object_handle bridge_obj = vlan->linked_bridge;
    bridge_drv_priv_t *bridge = NULL;
    rdpa_if port;
    int rc = 0;

    if (is_add)
    {
        if (total_vids == RDPA_MAX_VLANS)
        {
            BDMF_TRACE_ERR_OBJ(mo, "VID table is full (%d VIDs)\n", RDPA_MAX_VLANS);
            return BDMF_ERR_TOO_MANY;
        }
    }
    else
        BUG_ON(!total_vids);

    rdpa_port_index_get(mo->owner, &port);
    if (bridge_obj == NULL)
        bridge_obj = _rdpa_port_get_linked_bridge(port);
    if (bridge_obj)
        bridge = (bridge_drv_priv_t *)bdmf_obj_data(bridge_obj);

    if (is_add)
    {
        rc = rdpa_vlan_add_single_port_and_vid(port, vid, bridge_obj, vlan->proto_filters, &vlan->mac_lkp_cfg,
            vlan->ingress_filters_profile);
        if (rc)
            return rc;

        if (vlan->linked_bridge && bridge->cfg.auto_aggregate)
            rc = vlan_update_aggr_all_links(mo, vid, 1);
        if (rc)
        {
            BDMF_TRACE_RET_OBJ(rc, mo, "Failed to update VLAN aggregation configuration with newly added VID %d\n",
                vid);
        }
        total_vids++;
    }
    else
    {
        if (vlan->linked_bridge && bridge->cfg.auto_aggregate)
            rc = vlan_update_aggr_all_links(mo, vid, 0);
        if (rc)
        {
            BDMF_TRACE_ERR("Failed to update VLAN aggregation configuration with removed VID %d, rc = %d\n",
                vid, rc);
        } 
        rdpa_vlan_hash_entry_delete(port, vid);
        total_vids--;
        rc = 0;
    }
    if (mo->state == bdmf_state_active)
        rc = update_port_bridge_and_vlan_lookup_method_ex(port);

    return rc;
}

int vlan_attr_ingress_filter_write_ex(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    const void *val, uint32_t size)
{
#ifdef INGRESS_FILTERS
    vlan_drv_priv_t *vlan = (vlan_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_filter_ctrl_t *ctrl = (rdpa_filter_ctrl_t *)val;

    if (mo->state == bdmf_state_active)
    {
        return ingress_filter_entry_set((rdpa_filter)index, mo, vlan->ingress_filters, ctrl,
            &vlan->ingress_filters_profile);
    }
    return 0;
#else
    return BDMF_ERR_NOT_SUPPORTED;
#endif
}

int vlan_ctx_update_invoke(struct bdmf_object *vlan_obj, rdpa_vlan_hash_entry_modify_cb_t modify_cb, void *modify_ctx)
{
    vlan_drv_priv_t *vlan = (vlan_drv_priv_t *)bdmf_obj_data(vlan_obj);
    int rc = 0, i;
    rdpa_if port;

    rdpa_port_index_get(vlan_obj->owner, &port);
    for (i = 0; i < RDPA_MAX_VLANS; i++)
    {
        if (vlan->vids[i].vid == BDMF_INDEX_UNASSIGNED)
            continue;
        rc |= rdpa_vlan_hash_entry_modify(port, vlan->vids[i].vid, modify_cb, modify_ctx);
    }
    return rc;
}

int rdpa_vlan_mac_lkp_cfg_modify_cb(RDD_BRIDGE_AND_VLAN_LKP_RESULT_DTS *bridge_and_vlan_ctx_lkp_result,
    void *modify_ctx)
{
    bridge_and_vlan_ctx_mac_lkp_cfg_set(bridge_and_vlan_ctx_lkp_result, modify_ctx);
    return 0;
}

int rdpa_vlan_proto_filters_modify_cb(RDD_BRIDGE_AND_VLAN_LKP_RESULT_DTS *bridge_and_vlan_ctx_lkp_result,
    void *modify_ctx)
{
    bridge_and_vlan_ctx_lkp_result->protocol_filters_dis = disabled_proto_mask_get(*(uint32_t *)modify_ctx);
    return 0;
}

int rdpa_vlan_ingress_filter_profile_modify_cb(RDD_BRIDGE_AND_VLAN_LKP_RESULT_DTS *bridge_and_vlan_ctx_lkp_result,
    void *modify_ctx)
{
    bridge_and_vlan_ctx_lkp_result->ingress_filter_profile = *(uint8_t *)modify_ctx;
    return 0;
}

int rdpa_vlan_isolation_vector_modify_cb(RDD_BRIDGE_AND_VLAN_LKP_RESULT_DTS *bridge_and_vlan_ctx_lkp_result,
    void *modify_ctx)
{
    rdpa_ports isolation_vector = *(rdpa_ports *)modify_ctx;
    rdd_vport_vector_t vport_mask = rdpa_ports_to_rdd_egress_port_vector(isolation_vector, 0);

    rdpa_vlan_hash_entry_isolation_vector_set(vport_mask, bridge_and_vlan_ctx_lkp_result);
    return 0;
}

int rdpa_vlan_wan_aggregation_modify_cb(RDD_BRIDGE_AND_VLAN_LKP_RESULT_DTS *bridge_and_vlan_ctx_lkp_result,
    void *modify_ctx)
{
    bridge_and_vlan_ctx_lkp_result->aggregation_en = *(uint8_t *)modify_ctx;
    return 0;
}

int rdpa_vlan_arl_lkp_method_modify_cb(RDD_BRIDGE_AND_VLAN_LKP_RESULT_DTS *bridge_and_vlan_ctx_lkp_result,
    void *modify_ctx)
{
    bridge_arl_lkp_ctx_t *arl_lkp_ctx = (bridge_arl_lkp_ctx_t *)modify_ctx;

    bridge_and_vlan_ctx_lkp_result->arl_lookup_method = arl_lkp_ctx->lkp_method;
    bridge_and_vlan_ctx_lkp_result->wan_vid = arl_lkp_ctx->wan_vid;
    return 0;
}

int vlan_attr_mac_lkp_cfg_write_ex(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    const void *val, uint32_t size)
{
    vlan_drv_priv_t *vlan = (vlan_drv_priv_t *)bdmf_obj_data(mo);
    rdpa_mac_lookup_cfg_t *mac_lkp_cfg = (rdpa_mac_lookup_cfg_t *)val;
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo->owner);
    int rc;

    rc = mac_lkp_cfg_validate_ex(mac_lkp_cfg, port, port->cfg.ls_fc_enable);
    if (rc)
        return rc;

    if (mo->state == bdmf_state_active)
    {
        rc = vlan_ctx_update_invoke(mo, rdpa_vlan_mac_lkp_cfg_modify_cb, mac_lkp_cfg);
        if (rc)
            return rc;
    }

    vlan->mac_lkp_cfg = *mac_lkp_cfg;
    return 0;
}

int vlan_attr_proto_filters_write_ex(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index,
    const void *val, uint32_t size)
{
    vlan_drv_priv_t *vlan = (vlan_drv_priv_t *)bdmf_obj_data(mo);
    uint32_t profo_filters = *(uint32_t *)val;

    if (mo->state == bdmf_state_active)
    {
        int rc = vlan_ctx_update_invoke(mo, rdpa_vlan_proto_filters_modify_cb, &profo_filters);
        if (rc)
            return rc;
    }

    vlan->proto_filters = profo_filters;
    return 0;
}

int vlan_post_init_ex(struct bdmf_object *mo)
{
    vlan_drv_priv_t *vlan = (vlan_drv_priv_t *)bdmf_obj_data(mo);
    int rc;
#ifdef INGRESS_FILTERS
    int i;
    rdpa_filter_ctrl_t ingress_filters_init_ctrl[RDPA_FILTERS_QUANT] = {};
#endif
    port_drv_priv_t *port = (port_drv_priv_t *)bdmf_obj_data(mo->owner);
    rdd_vport_id_t vport = rdpa_port_rdpa_if_to_vport(port->index);

    rc = vlan_ctx_update_invoke(mo, rdpa_vlan_mac_lkp_cfg_modify_cb, &vlan->mac_lkp_cfg);
    rc = rc ? rc : vlan_ctx_update_invoke(mo, rdpa_vlan_proto_filters_modify_cb, &vlan->proto_filters);
    if (rc)
        return rc;

#ifdef INGRESS_FILTERS
    /* Set up ingress filters if configured */
    for (i = 0; i < RDPA_FILTERS_QUANT; i++)
    {
        if (!vlan->ingress_filters[i].enabled)
            continue;
        rc = ingress_filter_entry_set((rdpa_filter)i, mo, ingress_filters_init_ctrl,
            &vlan->ingress_filters[i], &vlan->ingress_filters_profile);
        if (rc)
            break;
    }
    if (rc)
        return rc;
#endif

    /* Update lookup method to port+VID if one of the VLANs enabled */
    if (vlan_is_any_vid_enabled(vlan))
    {
        return rdd_ag_processing_vport_cfg_table_bridge_and_vlan_lookup_method_set(vport, 
            BRIDGE_AND_VLAN_CTX_LOOKUP_METHOD_VPORT_VID);
    }
    
    return 0;
}

int vlan_is_any_vid_enabled(vlan_drv_priv_t *vlan)
{
    int i;
    
    for (i = 0; i < RDPA_MAX_VLANS; i++)
    {
        if (vlan->vids[i].vid != BDMF_INDEX_UNASSIGNED)
            return 1;
    }
    return 0;
}

int vlan_update_priority_vids(int16_t vid, bdmf_boolean vid_enable)
{
    int i, j;

    /* Priority for untagged packets should be the same as for priority tagged */
    if (vid == 0)
        rdd_ingress_qos_wan_untagged_priority_set(vid_enable);

    for (i = 0; i < NUM_OF_PARSER_VID_FILTERS; i++)
    {
        if (vid_enable)
        {
            if (prty_vids[i] == vid)  /* no change */
            {
                return 0;
            }
            else if (prty_vids[i] == BDMF_INDEX_UNASSIGNED)
            {
                prty_vids[i] = vid;
                break;
            }
        }
        else if (prty_vids[i] == vid)
        {
            prty_vids[i] = BDMF_INDEX_UNASSIGNED;
            break;
        }
    }

    if ((i == NUM_OF_PARSER_VID_FILTERS) && (vid_enable))
        BDMF_TRACE_RET(BDMF_ERR_PARM, "only 8 high priority vid is supported\n");
    
    BDMF_TRACE_DBG("configure filter number=%d vid=%d enable=%d\n", i, vid, vid_enable);
    for (j = 0; j < NUM_OF_RNR_QUADS; j++)
    {
        switch (i)
        {
        case 0:
            ag_drv_rnr_quad_parser_vid0_set(j, vid, vid_enable);
            break;               
        case 1:
            ag_drv_rnr_quad_parser_vid1_set(j, vid, vid_enable);
            break;               
        case 2:
            ag_drv_rnr_quad_parser_vid2_set(j, vid, vid_enable);
            break;               
        case 3:
            ag_drv_rnr_quad_parser_vid3_set(j, vid, vid_enable);
            break;               
        case 4:
            ag_drv_rnr_quad_parser_vid4_set(j, vid, vid_enable);
            break;               
        case 5:
            ag_drv_rnr_quad_parser_vid5_set(j, vid, vid_enable);
            break;               
        case 6:
            ag_drv_rnr_quad_parser_vid6_set(j, vid, vid_enable);
            break;               
        case 7:
            ag_drv_rnr_quad_parser_vid7_set(j, vid, vid_enable);
            break; 
       }
    }
    return 0;
}

int rdpa_vlan_hash_entry_idx_find(rdpa_if port, int16_t vid, hash_result_t *hash_res)
{
    RDD_BRIDGE_AND_VLAN_LKP_CMD_DTS bridge_and_vlan_ctx_lkp_cmd = 
    {
        .vport = rdpa_port_rdpa_if_to_vport(port),
        .vid = vid
    };
    int rc;

    rc = drv_hash_find(HASH_TABLE_BRIDGE_AND_VLAN_LKP, (uint8_t *)&bridge_and_vlan_ctx_lkp_cmd, hash_res);
    RDD_BTRACE("** looking for port %d with vid %d (orig %d), match status: %d, first_free_idx:%d, match_index:%d **\n",
        bridge_and_vlan_ctx_lkp_cmd.vport, (int)bridge_and_vlan_ctx_lkp_cmd.vid, (int)vid, hash_res->match,
        hash_res->first_free_idx, hash_res->match_index);

    if (rc)
    {
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Failed to search for [port %s vid %d] in hash, error = %d\n", 
            bdmf_attr_get_enum_text_hlp(&rdpa_if_enum_table, port), vid, rc);
    }

    if (hash_res->match == RESERVED) /* Paranoya check */
    {
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Unexpected hash lookup operation response for [port %s vid %d]\n", 
            bdmf_attr_get_enum_text_hlp(&rdpa_if_enum_table, port), vid);
    }

    return 0;
}

int rdpa_vlan_hash_entry_read(rdpa_if port, int16_t vid,
    RDD_BRIDGE_AND_VLAN_LKP_RESULT_DTS *bridge_and_vlan_ctx_lkp_result)
{
    hash_result_t hash_res = {};
    uint8_t ext_ctx[6] = {}, int_ctx[3] = {}, unused;
    bdmf_boolean skip = 0;
    int rc;

    rc = rdpa_vlan_hash_entry_idx_find(port, vid, &hash_res);
    if (rc)
        return rc;

    if (hash_res.match == HASH_MISS)
        return BDMF_ERR_NOENT;

    rc = drv_hash_get_context(hash_res.match_index, HASH_TABLE_BRIDGE_AND_VLAN_LKP, ext_ctx, int_ctx, &skip, &unused);
    if (rc || skip)
        BDMF_TRACE_RET(BDMF_ERR_NOENT, "Cannot read entry from hash, rc %d, (failed in CAM %d)\n", rc, skip);

    /* There are some reserved fields in the struct that are not filled when decomposing the context from hash. To allow
     * comparing contexts using memcmp it's better to have these fields set to 0. Hence memset prior to decompose */
    memset(bridge_and_vlan_ctx_lkp_result, 0, sizeof(RDD_BRIDGE_AND_VLAN_LKP_RESULT_DTS));

    rdd_bridge_bridge_and_vlan_ctx_hash_ctx_decompose(bridge_and_vlan_ctx_lkp_result, int_ctx, ext_ctx);
    return 0;
}

int rdpa_vlan_hash_entry_write(rdpa_if port, int16_t vid,
    RDD_BRIDGE_AND_VLAN_LKP_RESULT_DTS *bridge_and_vlan_ctx_lkp_result)
{
    hash_result_t hash_res = {};
    uint8_t ext_ctx[6] = {}, int_ctx[3] = {};
    int rc;

    rc = rdpa_vlan_hash_entry_idx_find(port, vid, &hash_res);
    if (rc)
        return rc;

    rdd_bridge_bridge_and_vlan_ctx_hash_ctx_compose(bridge_and_vlan_ctx_lkp_result, int_ctx, ext_ctx);

    if (hash_res.match == HASH_MISS)
    {                        
        RDD_BRIDGE_AND_VLAN_LKP_CMD_DTS bridge_and_vlan_ctx_lkp_cmd = 
        {
            .vport = rdpa_port_rdpa_if_to_vport(port),
            .vid = vid
        };

        RDD_BTRACE("*** add rule: match index = %d ***\n", hash_res.first_free_idx);
        rc = drv_hash_rule_add(HASH_TABLE_BRIDGE_AND_VLAN_LKP, (uint8_t *)&bridge_and_vlan_ctx_lkp_cmd, 
            ext_ctx, int_ctx);
    }
    else
    {
        RDD_BTRACE("*** modify rule: match index = %d ***\n", hash_res.match_index); 
        rc = drv_hash_modify_context(HASH_TABLE_BRIDGE_AND_VLAN_LKP, hash_res.match_index, ext_ctx, int_ctx);
    }

    if (rc)
    {
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Failed to update context for [port %s vid %d] in hash, error = %d\n", 
            bdmf_attr_get_enum_text_hlp(&rdpa_if_enum_table, port), vid, rc);
    }

    return 0;
}

int rdpa_vlan_hash_entry_modify(rdpa_if port, int16_t vid, rdpa_vlan_hash_entry_modify_cb_t modify_cb, void *modify_ctx)
{
    RDD_BRIDGE_AND_VLAN_LKP_RESULT_DTS bridge_and_vlan_ctx_lkp_result;
    int rc;

    rc = rdpa_vlan_hash_entry_read(port, vid, &bridge_and_vlan_ctx_lkp_result);
    if (rc)
        return rc;

    rc = modify_cb(&bridge_and_vlan_ctx_lkp_result, modify_ctx);
    if (rc)
        return rc;

    return rdpa_vlan_hash_entry_write(port, vid, &bridge_and_vlan_ctx_lkp_result);
}

int rdpa_vlan_hash_entry_delete(rdpa_if port, int16_t vid)
{
    hash_result_t hash_res = {};
    int rc;

    rc = rdpa_vlan_hash_entry_idx_find(port, vid, &hash_res);
    if (rc)
        return rc;

    if (hash_res.match == HASH_MISS)
        return BDMF_ERR_NOENT;

    rc = drv_hash_rule_remove_index(HASH_TABLE_BRIDGE_AND_VLAN_LKP, hash_res.match_index);
    if (rc)
    {
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "Failed to delete hash entry %d for [port %s vid %d], error = %d\n", 
            hash_res.match_index, bdmf_attr_get_enum_text_hlp(&rdpa_if_enum_table, port), vid, rc);
    }

    return 0;
}

int vlan_update_bridge_arl_lkp_method_ex(struct bdmf_object *vlan_obj, bridge_arl_lkp_ctx_t *arl_lkp_ctx)
{
    return vlan_ctx_update_invoke(vlan_obj, rdpa_vlan_arl_lkp_method_modify_cb, arl_lkp_ctx);
}


/* Add aggregation wan_vlan_obj --> lan_vlan_obj by LAN vid */
int vlan_wan_aggr_add_ex(struct bdmf_object *lan_vlan_obj, struct bdmf_object *wan_vlan_obj, int16_t lan_vid)
{
    vlan_drv_priv_t *wan_vlan = (vlan_drv_priv_t *)bdmf_obj_data(wan_vlan_obj);
 
    /* Do nothing if no VID in WAN container */
    if (wan_vlan->vids[0].vid == BDMF_INDEX_UNASSIGNED)
        return 0;

    return vlan_wan_aggr_add_to_rdd(lan_vlan_obj, lan_vid, wan_vlan->vids[0].vid);
}

/* Delete aggregation wan_vlan_obj --> lan_vlan_obj by LAN vid */
void vlan_wan_aggr_del_ex(struct bdmf_object *lan_vlan_obj, struct bdmf_object *wan_vlan_obj, int16_t lan_vid)
{
    vlan_drv_priv_t *wan_vlan = (vlan_drv_priv_t *)bdmf_obj_data(wan_vlan_obj);

    vlan_wan_aggr_delete_to_rdd(lan_vlan_obj, lan_vid, wan_vlan->vids[0].vid);
}

void vlan_drv_init_ex(void)
{
    total_vids = 0;
}

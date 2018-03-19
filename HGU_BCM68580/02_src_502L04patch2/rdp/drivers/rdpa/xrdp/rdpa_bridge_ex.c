
/*
 * <:copyright-BRCM:2015:proprietary:standard
 * 
 *    Copyright (c) 2015 Broadcom 
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

#include "rdpa_rdd_map.h"
#include "rdpa_bridge_ex.h"
#include "rdpa_vlan_ex.h"
#include "rdp_drv_rnr.h"
#include "rdp_drv_hash.h"
#include "rdd_ag_bridge.h"
#include "rdd_ag_processing.h"
#include "rdd_bridge.h"
#include "rdpa_filter_ex.h"

#define _RDPA_BRIDGE_MAX_FDB_ENTRIES  1024
#define _RDPA_BRIDGE_MAX_VLAN_ENTRIES  128
#define MODIFIED_ARL_ENTRY(entry)  (entry / ((HASH_NUM_OF_ENGINES / HASH_NUM_OF_EFFECTIVE_ENGINES)))

/* Bridge ID helpers*/

extern bdmf_fastlock bridge_fastlock;
extern bdmf_fastlock vlan_vid_refs_fastlock;

int bridge_set_fw_elig_ex(struct bdmf_object *mo, rdpa_if port_idx, rdpa_ports new_mask)
{
    bridge_drv_priv_t *bridge = (bridge_drv_priv_t *)bdmf_obj_data(mo);
    bdmf_object_handle port_obj, vlan_obj = NULL;
    rdpa_ports new_global_mask, old_global_mask;
    port_drv_priv_t *port;
    int rc;
    rdd_vport_id_t rdd_vport;
    rdd_vport_vector_t bridge_vport_mask;

    /* It's possible that OMCI configuration requires same port be linked to different 802.1D bridges. In this case,
     * need to collect eligibility maps from all 802.1D bridges the port linked to. */
    if (bridge->cfg.type == rdpa_bridge_802_1d)
    {
        old_global_mask = bridge_get_global_fw_elig(bridge, port_idx);
        new_global_mask = old_global_mask | new_mask;
    }
    else
        new_global_mask = new_mask;

    rdpa_port_get(port_idx, &port_obj);
    port = (port_drv_priv_t *)bdmf_obj_data(port_obj);

    /* protect bridge_and_vlan_ctx table from on the fly changes*/
    bdmf_fastlock_lock(&vlan_vid_refs_fastlock);

    /* Update VLANs entries that are children of new port and linked to same bridge */
    while ((vlan_obj = bdmf_get_next_child(port_obj, "vlan", vlan_obj)) != NULL)
    {
        vlan_drv_priv_t *vlan = (vlan_drv_priv_t *)bdmf_obj_data(vlan_obj);

        /* Vlan linked to different 802.1Q bridge */
        if (vlan->linked_bridge && vlan->linked_bridge != mo)
            continue;

        /* Vlan not linked to any bridge, but isolation is disabled. Here we must to take care for Ingress isolation and
         * update the isolation (=forwarding) vector as we use the lookup in hash to make forwarding decision.
         * XXX: No need for egress isolation? */
        if (!vlan->linked_bridge && !(port->vlan_isolation.us || port->vlan_isolation.ds))
            continue;

        rc = vlan_ctx_update_invoke(vlan_obj, rdpa_vlan_isolation_vector_modify_cb, &new_global_mask);
        if (rc)
        {
            BDMF_TRACE_ERR_OBJ(mo, "Failed to configure new isolation vector for VLAN object '%s' in bridge/index=%d, "
                "rc %d\n", vlan_obj->name, (int)bridge->index, rc);
            goto exit;
        }
    }
    
    /* Update Vport default table and isolated VLANS table if bridge is linked to current port */
    if (port->bridge_obj == mo)
    {
        /* Update port entries (vport only) */
        rdd_vport = rdpa_port_rdpa_if_to_vport(port_idx);
        bridge_vport_mask = rdpa_ports_to_rdd_egress_port_vector(new_global_mask, 0);

        rc = rdd_ag_processing_vport_cfg_table_egress_isolation_map_set(rdd_vport, bridge_vport_mask);
        if (rc)
            goto exit;

        /* Work-around in order to update the case when port is linked to 802.1Q bridge. */
        if (bridge->cfg.type == rdpa_bridge_802_1q)
            rc = port_update_hash_port_and_vid0(port, rdpa_vlan_isolation_vector_modify_cb, &new_global_mask);
        
        if (rc)
        {
            BDMF_TRACE_ERR_OBJ(mo, "Failed to Add/Modify port isolation context %s when reconfiguring bridge %d\n",
                bdmf_attr_get_enum_text_hlp(&rdpa_if_enum_table, port_idx), (int)bridge->index);
            goto exit;
        }
    }
    
    bridge->port_fw_elig[port_idx] = new_global_mask;

exit:
    bdmf_fastlock_unlock(&vlan_vid_refs_fastlock);
    if (vlan_obj)
        bdmf_put(vlan_obj);
    bdmf_put(port_obj);
    return rc;
}

/* ARL helpers*/

static void _rdpa_bridge_rdd_arl_data2rdpa_fdb_data(rdpa_fdb_data_t *rdpa_data,
    const RDD_BRIDGE_ARL_LKP_RESULT_DTS *rdd_arl_data)
{
    rdd_action rdd_bridge_action = (rdd_arl_data->da_match_action_fwd == 1) ? ACTION_FORWARD
        : (rdd_arl_data->lan_vid_msb_or_da_no_fwd_action == NO_FWD_ACTION_DROP) ?
        ACTION_DROP : ACTION_TRAP;

    rdpa_data->sa_action = 
        rdd_action2rdpa_forward_action(rdd_arl_data->sa_match_action);
    rdpa_data->da_action = 
        rdd_action2rdpa_forward_action(rdd_bridge_action);
    rdpa_data->ports = rdpa_if_id(rdpa_port_vport_to_rdpa_if(rdd_arl_data->vport));
}

static void _rdpa_bridge_rdpa_fdb_data2rdd_arl_data(const rdpa_fdb_data_t *rdpa_data,
   RDD_BRIDGE_ARL_LKP_RESULT_DTS *rdd_arl_data, int lan_vid, rdpa_if port)
{
    rdd_arl_data->vport = rdpa_port_rdpa_if_to_vport(port);
    rdd_arl_data->sa_match_action = 
        rdpa_forward_action2rdd_action(rdpa_data->sa_action);

    if (rdpa_data->da_action == rdpa_forward_action_forward)
    {
        rdd_arl_data->da_match_action_fwd = 1;
        if (!rdpa_if_is_wan(port))
        {
            rdd_arl_data->lan_vid_8lsb = lan_vid & 0xFF;
            rdd_arl_data->lan_vid_msb_or_da_no_fwd_action = lan_vid >> 8;
        }
    }
    else
    {
        rdd_arl_data->lan_vid_msb_or_da_no_fwd_action = (rdpa_data->da_action == rdpa_forward_action_drop) ?
            NO_FWD_ACTION_DROP : NO_FWD_ACTION_TRAP;
    }
}

static void _rdpa_bridge_rdpa_fdb_key2rdd_arl_key(
   const rdpa_fdb_key_t *rdpa_key, RDD_BRIDGE_ARL_LKP_CMD_DTS *rdd_arl_key)
{
    int i;

    memset(rdd_arl_key, 0, sizeof(RDD_BRIDGE_ARL_LKP_CMD_DTS));
    /* TODO: move to a common macro*/
    /* rdpa_key: b[5]| b[4]|b[3] | b[2] | b[1] | b[0]*/
    /* rdd_arl_key:
                       uint16_t mac_1_2 holds b[0] << 8 | b[1]
                       uitn32_t mac_3_6 holds b[2] << 24 | b[3] << 16 | b[4] << 8 | b[5]
      */
    rdd_arl_key->mac_1_2 = 
        ((uint32_t)rdpa_key->mac.b[0] << 8) | rdpa_key->mac.b[1];
    
    for (i = 0; i < sizeof(uint32_t); i++)
    	rdd_arl_key->mac_3_6 = 
            (rdd_arl_key->mac_3_6 << 8) | rdpa_key->mac.b[2 + i];

    rdd_arl_key->vid = rdpa_key->vid;
}

#if 0
static void _rdpa_bridge_rdd_arl_key2rdpa_fdb_key(
   const  RDD_BRIDGE_ARL_LKP_CMD_DTS *rdd_arl_key, rdpa_fdb_key_t *rdpa_key)
{
   /* TODO: move to a common macro */
    int i;

    rdpa_key->mac.b[0] = rdd_arl_key->mac_1_2 >> 8;
    rdpa_key->mac.b[1] = rdd_arl_key->mac_1_2 & 0xFF;
    for (i = 0; i < sizeof(uint32_t); i++)
    {
        rdpa_key->mac.b[5 - i] = 
            (rdd_arl_key->mac_3_6 >> (8 * i)) & 0xFF;
    }
    if (rdd_arl_key->vid)
        rdpa_key->vid = rdd_arl_key->vid;
}
#endif  

static int _rdpa_arl_entry_search(const rdpa_fdb_key_t *rdpa_key, uint16_t *arl_entry_index, int is_add)
{
   RDD_BRIDGE_ARL_LKP_CMD_DTS rdd_arl_key = {};
   hash_result_t hash_res = {};
   int rc;

   _rdpa_bridge_rdpa_fdb_key2rdd_arl_key(rdpa_key, &rdd_arl_key);
   rc = drv_hash_find(HASH_TABLE_ARL, (uint8_t *)&rdd_arl_key, &hash_res);
   if (rc)
       return rc;

   if (hash_res.match == HASH_MISS)
   {
       if (is_add && hash_res.first_free_idx == HASH_INVALID_IDX)
       {
           RDD_BTRACE("Cannot add MAC %pM, neither Hash Nor CAM slot available", &rdpa_key->mac);
           return BDMF_ERR_INTERNAL;
       }
       *arl_entry_index = hash_res.first_free_idx;
       return BDMF_ERR_NOENT;
   }
   else
   {
       *arl_entry_index = hash_res.match_index;
       return BDMF_ERR_OK;
   }
}

int bridge_post_init_ex(struct bdmf_object *mo)
{
    return BDMF_ERR_OK;
}

void bridge_destroy_ex(struct bdmf_object *mo)
{
    bridge_drv_priv_t *bridge = (bridge_drv_priv_t *)bdmf_obj_data(mo);

    drv_rnr_quad_parser_da_filter_without_mask_set(BRIDGE_DA_MAC_FILTER_GROUP,
        bridge->index, bridge->lan_mac.b, 0);
}

int bridge_link_port_ex(bridge_drv_priv_t *bridge, bdmf_number port_index)
{
    if (bridge->cfg.type == rdpa_bridge_802_1q)
        return update_port_bridge_and_vlan_lookup_method_ex(port_index);
    return BDMF_ERR_OK;
}

void bridge_unlink_port_ex(bridge_drv_priv_t *bridge, bdmf_number port_index)
{
    if (bridge->cfg.type == rdpa_bridge_802_1q)
        update_port_bridge_and_vlan_lookup_method_ex(port_index);
}

static int _rdpa_find_arl_entry_index(struct bdmf_object *mo, rdpa_fdb_key_t *key, rdpa_fdb_key_t *rdpa_shadow_key,
    uint16_t *arl_entry_index, int is_add)
{
    int rc = 0;
    bridge_drv_priv_t *bridge = (bridge_drv_priv_t *)bdmf_obj_data(mo);

    memcpy(rdpa_shadow_key, key, sizeof(rdpa_fdb_key_t));
    if (bridge->cfg.learning_mode == rdpa_bridge_learn_ivl)
    {
        if (bridge->wan_vid == BDMF_INDEX_UNASSIGNED)
        {
            BDMF_TRACE_RET_OBJ(BDMF_ERR_PERM, mo,
                "WAN VID not assigned, mac arl entry find failed\n");
        }
        rdpa_shadow_key->vid = bridge->wan_vid;
    }
    else /* rdpa_bridge_learn_svl */
        rdpa_shadow_key->vid = 0;

    rc = _rdpa_arl_entry_search(rdpa_shadow_key, arl_entry_index, is_add);
    if (rc && (rc != BDMF_ERR_NOENT))
    {
        BDMF_TRACE_ERR("Cannot search MAC %pM with VLAN %d in ARL table, error = %d\n", &(rdpa_shadow_key->mac),
            rdpa_shadow_key->vid, rc);
    }
    return rc;
}

int bridge_mac_delete_ex(struct bdmf_object *mo, rdpa_fdb_key_t *key)
{
    bridge_drv_priv_t *bridge = (bridge_drv_priv_t *)bdmf_obj_data(mo);
    uint16_t arl_entry_index;
    rdpa_fdb_key_t rdpa_shadow_key;
    int rc;

    bdmf_fastlock_lock(&bridge_fastlock);
    rc = _rdpa_find_arl_entry_index(mo, key, &rdpa_shadow_key, &arl_entry_index, 0);
    if (rc)
    {
        bdmf_fastlock_unlock(&bridge_fastlock);
        if (rc == BDMF_ERR_NOENT)
        {
            BDMF_TRACE_DBG_OBJ(mo, "Cannot delete MAC %pM with VLAN %d, not in ARL table, error = %d\n",
               &(rdpa_shadow_key.mac), rdpa_shadow_key.vid, rc);
        }
        return rc;
    }

    rc = drv_hash_rule_remove_index(HASH_TABLE_ARL, arl_entry_index);
    bdmf_fastlock_unlock(&bridge_fastlock);
    if (!rc)
    {
        bridge_occupancy_entry_t *entry = NULL, *tmp_entry;

        DLIST_FOREACH_SAFE(entry, &bridge->bridge_occupancy_list, list, tmp_entry)
        {
            if (entry->key.entry == arl_entry_index)
            {
                DLIST_REMOVE(entry, list);
                bdmf_free(entry);
            }
        }
    }

    return rc;
}

int bridge_mac_add_modify_ex(struct bdmf_object *mo, rdpa_fdb_key_t *rdpa_key,
    const rdpa_fdb_data_t *data)
{
    bridge_drv_priv_t *bridge = (bridge_drv_priv_t *)bdmf_obj_data(mo);
    RDD_BRIDGE_ARL_LKP_CMD_DTS hash_key = {};
    RDD_BRIDGE_ARL_LKP_RESULT_DTS hash_ctx = {};
    uint16_t arl_entry_index, lan_vid = 0;
    uint8_t ext_ctx[3];
    rdpa_ports data_ports = data->ports;
    rdpa_if port;
    rdpa_fdb_key_t rdpa_shadow_key;
    int rc;

    if (!rdpa_port_is_single(data->ports))
        BDMF_TRACE_RET_OBJ(BDMF_ERR_NOT_SUPPORTED, mo, "Multiple destination ports for MAC\n");

    /* If port is lan, and bridge is aggregated, set lan_vid in arl result */
    port = rdpa_port_get_next(&data_ports);
    if (rdpa_if_is_lan(port) && bridge->wan_vid != BDMF_INDEX_UNASSIGNED && bridge->cfg.auto_aggregate)
    {
        lan_vid = rdpa_key->vid;
    }

    bdmf_fastlock_lock(&bridge_fastlock);
    rc = _rdpa_find_arl_entry_index(mo, rdpa_key, &rdpa_shadow_key, &arl_entry_index, 1);
    if (rc && (rc != BDMF_ERR_NOENT))
    {
        bdmf_fastlock_unlock(&bridge_fastlock);
        BDMF_TRACE_RET_OBJ(BDMF_ERR_INTERNAL, mo,
            "Cannot search MAC %pM with VID %d in ARL table, error = %d\n",
            &(rdpa_key->mac), rdpa_key->vid, rc);
    }

    _rdpa_bridge_rdpa_fdb_key2rdd_arl_key(&rdpa_shadow_key, &hash_key);

    _rdpa_bridge_rdpa_fdb_data2rdd_arl_data(data, &hash_ctx, lan_vid, port);
    map_rdd_arl_data_to_ext_ctx(&hash_ctx, ext_ctx);
    if (rc == BDMF_ERR_NOENT)
    {
        bridge_occupancy_entry_t *new_entry = NULL;
        /* Add functionality*/
        rc = drv_hash_rule_add(HASH_TABLE_ARL, (uint8_t *)&hash_key, ext_ctx, NULL);
        bdmf_fastlock_unlock(&bridge_fastlock);
        if (!rc)
        {
            rdpa_key->entry = arl_entry_index;
            new_entry = bdmf_calloc(sizeof(bridge_occupancy_entry_t));
            if (!new_entry)
            {
                return BDMF_ERR_NOMEM;
            }
            memcpy(&new_entry->key, rdpa_key, sizeof(rdpa_fdb_key_t));
            DLIST_INSERT_HEAD(&bridge->bridge_occupancy_list, new_entry, list);
            BDMF_TRACE_INFO_OBJ(mo, "Added MAC %pM with VID %d to bridge %d mac_index: %d\n",
                &(rdpa_key->mac), rdpa_key->vid, (uint8_t)bridge->index, arl_entry_index);
        }
    }
    else
    {
        rc = drv_hash_modify_context(HASH_TABLE_ARL, arl_entry_index, ext_ctx, NULL);
        /* Modify functionality*/
        bdmf_fastlock_unlock(&bridge_fastlock);
    }
    return rc;
}

int bridge_attr_mac_get_next_ex(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index *index)
{
    bridge_drv_priv_t *bridge = (bridge_drv_priv_t *)bdmf_obj_data(mo);
    bridge_occupancy_entry_t *entry = NULL, *tmp_entry;
    rdpa_fdb_key_t *key = (rdpa_fdb_key_t *)index;
    rdpa_fdb_key_t  rdpa_shadow_key;
    int rc = 0;
    uint16_t arl_entry_index;

    bdmf_fastlock_lock(&bridge_fastlock);
    if (*index != BDMF_INDEX_UNASSIGNED)
    {
        key = ((rdpa_fdb_key_t *)index);
        /* Ussually user doesn't specify the entry index- in this case calculte it.*/
        if (key->entry == BDMF_INDEX_UNASSIGNED) /* XXX: How can happen? Re-check the flow */
        {
            rc = _rdpa_find_arl_entry_index(mo, key, &rdpa_shadow_key, &arl_entry_index, 0);
            if (!rc)
                key->entry = arl_entry_index;
        }
        DLIST_FOREACH_SAFE(entry, &bridge->bridge_occupancy_list, list, tmp_entry)
        {
            if (entry->key.entry == key->entry)
            {
                entry = DLIST_NEXT(entry, list);
                break;
            }
        }
    }
    else
        entry = DLIST_FIRST(&bridge->bridge_occupancy_list);
    
    bdmf_fastlock_unlock(&bridge_fastlock);
    if (!entry)
        return BDMF_ERR_NO_MORE;
    memcpy(key, &(entry->key), sizeof(rdpa_fdb_key_t));
    return BDMF_ERR_OK;    
}

int bridge_mac_read_ex(struct bdmf_object *mo, rdpa_fdb_key_t *key, rdpa_fdb_data_t *data)
{
    uint16_t arl_entry_index;
    RDD_BRIDGE_ARL_LKP_RESULT_DTS rdd_arl_data = {};
    rdpa_fdb_key_t rdpa_shadow_key;
    bdmf_boolean skip;
    uint8_t valid;
    uint8_t ext_ctx[3];
    int rc;

    rc = _rdpa_find_arl_entry_index(mo, key, &rdpa_shadow_key, &arl_entry_index, 0);

    if (rc == BDMF_ERR_NOENT)
        return BDMF_ERR_NOENT;

    rc = drv_hash_get_context(arl_entry_index, HASH_TABLE_ARL, ext_ctx, NULL, &skip, &valid);
    map_ext_ctx_to_rdd_arl_data(ext_ctx, &rdd_arl_data);

    if (rc && (!valid || skip))
    {
        BDMF_TRACE_RET(BDMF_ERR_INVALID_OP,
           "MAC %pM with VID %d is invalid in ARL table, cannot read entry\n",
           &(key->mac), key->vid);
    }

    _rdpa_bridge_rdd_arl_data2rdpa_fdb_data(data, &rdd_arl_data);

    return BDMF_ERR_OK;
}

int bridge_attr_mac_status_read_ex(struct bdmf_object *mo, rdpa_fdb_key_t *key,
    bdmf_boolean *status)
{
    uint16_t arl_entry_index;
    bdmf_boolean skip, age;
    uint8_t valid;
    uint8_t ext_ctx[3];
    uint8_t int_ctx[3];
    int rc = BDMF_ERR_OK;
    rdpa_fdb_key_t rdpa_shadow_key;

    rc = _rdpa_find_arl_entry_index(mo, key, &rdpa_shadow_key, &arl_entry_index, 0);
    if (rc)
    {
        if (rc != BDMF_ERR_NOENT)
        {
            BDMF_TRACE_ERR("Cannot search MAC %pM with VLAN %d in ARL table\n",
                &(key->mac), key->vid);
            rc = BDMF_ERR_INTERNAL;
        }
        goto exit;
    }
    rc = drv_hash_get_context(arl_entry_index, HASH_TABLE_ARL, (uint8_t *)ext_ctx, (uint8_t *)int_ctx, &skip, &valid);
    if (rc || skip)
    {
        rc = BDMF_ERR_NOENT;
        goto exit;
    }
    /* this hash fucntion first gets previous aging value, and then sets it to aged*/
    rc = drv_hash_set_aging(HASH_TABLE_ARL, arl_entry_index, &age);

    *status = !age;
exit:
    return rc;
}

int bridge_attr_fw_eligible_write_ex(struct bdmf_object *mo,
    bdmf_index index, rdpa_ports fw_elig)
{
    return BDMF_ERR_OK;
}

int bridge_attr_lan_mac_write_ex(struct bdmf_object *mo, bdmf_mac_t *mac, bdmf_index index)
{
    int rc =  drv_rnr_quad_parser_da_filter_without_mask_set(BRIDGE_DA_MAC_FILTER_GROUP,
        index, mac->b, 0);
    rc =  rc ? rc : drv_rnr_quad_parser_da_filter_without_mask_set(BRIDGE_DA_MAC_FILTER_GROUP,
        index, mac->b, bdmf_mac_is_zero(mac) ? 0 : 1);

    if (rc)
    {
        BDMF_TRACE_RET_OBJ(BDMF_ERR_INTERNAL, mo,
            "cannot set MAC %pM in PARSER, error = %d\n", mac, rc);
    }
    return BDMF_ERR_OK;
}       


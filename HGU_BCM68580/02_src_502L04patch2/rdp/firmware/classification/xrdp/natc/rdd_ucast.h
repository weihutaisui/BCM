/*
    <:copyright-BRCM:2013:DUAL/GPL:standard
    
       Copyright (c) 2013 Broadcom 
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


#ifndef _RDD_UCAST_H
#define _RDD_UCAST_H

#include "rdd.h"
#include "rdpa_ucast.h"
#include "rdd_tuple_lkp.h"
#include "rdp_drv_natc.h"
#include "data_path_init.h"

int rdd_connection_entry_add(rdd_ip_flow_t *add_connection, rdpa_traffic_dir direction);

int rdd_connection_entry_delete(bdmf_index flow_entry_index);

int rdd_connection_entry_get(rdpa_traffic_dir direction, uint32_t entry_index,
    rdpa_ip_flow_key_t *nat_cache_lkp_entry, bdmf_index *flow_entry_index);

int rdd_connection_entry_search(rdd_ip_flow_t *get_connection, rdpa_traffic_dir direction,
    bdmf_index *entry_index);

int rdd_context_entry_get(bdmf_index flow_entry_index, rdd_fc_context_t *context_entry);

int rdd_context_entry_modify(rdd_fc_context_t *context, bdmf_index flow_entry_index);

int rdd_context_entry_flwstat_get(bdmf_index flow_entry_index, rdd_fc_context_t *context_entry);

int rdd_flow_counters_get(bdmf_index flow_entry_index, uint32_t *packets, uint32_t *bytes);

int rdpa_wan_type2rdd_egress_phy(rdpa_wan_type src);

rdpa_wan_type rdd_egress_phy2rdpa_wan_type(int src);

int rdd_fc_flow_ip_addresses_add(RDD_FC_FLOW_IP_ADDRESSES_ENTRY_DTS *ip_addresses_entry,
    bdmf_index *entry_index, uint16_t *entry_sram_address);

int rdd_fc_flow_ip_addresses_delete_by_index(bdmf_index entry_index);

int rdd_fc_flow_ip_addresses_get(bdmf_index entry_index, RDD_FC_FLOW_IP_ADDRESSES_ENTRY_DTS *ip_addresses_entry,
    uint16_t *entry_sram_address);

int rdd_ipv4_host_address_table_get(uint32_t table_index, bdmf_ipv4 *ipv4_host_addr, uint16_t *ref_count);

int rdd_ipv4_host_address_table_set(uint32_t table_index, bdmf_ipv4 ipv4_host_addr, uint16_t ref_count);

int rdd_ipv6_host_address_table_get(uint32_t table_index, bdmf_ipv6_t *ipv6_host_addr, uint16_t *ref_count);

int rdd_ipv6_host_address_table_set(uint32_t table_index, const bdmf_ipv6_t *ipv6_host_addr, uint16_t ref_count);

int rdd_ucast_ds_wan_udp_filter_get(bdmf_index entry_index, RDD_DS_WAN_UDP_FILTER_ENTRY_DTS *ds_wan_udp_filter_entry);

int rdd_ucast_ds_wan_udp_filter_add( RDD_DS_WAN_UDP_FILTER_ENTRY_DTS  *ds_wan_udp_filter_entry, bdmf_index *entry_index_res);

int rdd_ucast_ds_wan_udp_filter_delete( bdmf_index entry_index );

int rdpa_if_wan2rdd_egress_phy(rdpa_if src);

#endif /* RDD_UCAST_H */


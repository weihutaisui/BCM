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


#include "rdd_ucast.h"

#define RDD_NATC_TABLE_SIZE_PER_DIRECTION  (RDD_CONTEXT_TABLE_SIZE/2)
#define GET_NATC_INDEX_FROM_BDMF_INDEX(index) (index < RDD_NATC_TABLE_SIZE_PER_DIRECTION ? index : index - RDD_NATC_TABLE_SIZE_PER_DIRECTION)
#define GET_TABLE_ACCORDING_TO_INDEX(index) (index < RDD_NATC_TABLE_SIZE_PER_DIRECTION ? tuple_lkp_ds_tbl : tuple_lkp_us_tbl)
#define SET_BDMF_INDEX_FROM_NATC_INDEX(index, dir) (dir ==  rdpa_dir_ds ? index : index + RDD_NATC_TABLE_SIZE_PER_DIRECTION)

typedef struct
{
    uint32_t natc_control;
    rdd_fc_context_t rdd_ip_flow;
} natc_result_entry_t;

typedef struct
{
    bdmf_ipv6_t ipv6_address;
    uint16_t    ref_count;
} ipv6_host_table_t;

static ipv6_host_table_t g_ipv6_host_table[RDD_IPV6_HOST_ADDRESS_CRC_TABLE_SIZE];
static uint16_t g_ipv4_host_ref_count_table[RDD_IPV4_HOST_ADDRESS_TABLE_SIZE];

/* rdd_ipproto_lookup_port_get() gets IP Proto (TCP, UDP, etc.) and source bridge port from the protocol fields */
static inline int rdd_ipproto_lookup_port_get(uint8_t *prot_ptr, uint8_t *lookup_port_ptr)
{
    RDD_TUPLE_PROTO_DTS *proto = (RDD_TUPLE_PROTO_DTS *) prot_ptr;
    uint8_t ipproto_idx;
    uint8_t ipproto;

    *lookup_port_ptr = proto->lookup_port;
    ipproto_idx = proto->protocol;

    switch (ipproto_idx)
    {
    case IP_PROTO_IDX_TCP:
        ipproto = IP_PROTO_TCP;
        break;

    case IP_PROTO_IDX_UDP:
        ipproto = IP_PROTO_UDP;
        break;

    case IP_PROTO_IDX_IPIP:
        ipproto = IP_PROTO_IPIP;
        break;

    case IP_PROTO_IDX_IPV6:
        ipproto = IP_PROTO_IPV6;
        break;

    default:
        ipproto = IP_PROTO_UDP;
    }

    *prot_ptr = ipproto;

    return 0;
}

/* rdd_prot_set() sets protocol field using IP Proto (TCP, UDP, etc.) and connection port */
static inline int rdd_prot_set(uint8_t *prot_ptr, uint8_t lookup_port)
{
    RDD_TUPLE_PROTO_DTS *proto = (RDD_TUPLE_PROTO_DTS *) prot_ptr;

    switch (*prot_ptr)
    {
    case IP_PROTO_TCP:
        proto->protocol = IP_PROTO_IDX_TCP;
        break;

    case IP_PROTO_UDP:
        proto->protocol = IP_PROTO_IDX_UDP;
        break;

    case IP_PROTO_IPIP:
        proto->protocol = IP_PROTO_IDX_IPIP;
        break;

    case IP_PROTO_IPV6:
        proto->protocol = IP_PROTO_IDX_IPV6;
        break;

    default:
        proto->protocol = IP_PROTO_IDX_UNDEF;
        break;
    }

    proto->lookup_port = lookup_port;

    return 0;
}

int rdd_connection_entry_add(rdd_ip_flow_t *add_connection, rdpa_traffic_dir direction)
{
    bdmf_error_t rc = BDMF_ERR_OK;
    uint8_t keyword[NATC_MAX_ENTRY_LEN] = {};
    natc_result_entry_t context_entry = {};
    uint8_t *ip_flow = (uint8_t *) &context_entry.rdd_ip_flow;
    uint8_t nat_cache_lkp_entry_protocol;
#if defined(USE_NATC_VAR_CONTEXT_LEN)
    uint8_t hash_keyword[NATC_MAX_ENTRY_LEN] = {};
#endif

    nat_cache_lkp_entry_protocol = add_connection->lookup_entry->prot;
    rdd_prot_set ( &add_connection->lookup_entry->prot, add_connection->lookup_entry->lookup_port );

#if defined(USE_NATC_VAR_CONTEXT_LEN)
    rc = rdd_ip_class_key_entry_var_size_ctx_compose(add_connection->lookup_entry, keyword, hash_keyword, (natc_var_size_ctx)
        add_connection->context_entry.fc_ucast_flow_context_entry.command_list_length_64 + 1); /* +1 for non-command list fields */
#else
    rc = rdd_ip_class_key_entry_compose(add_connection->lookup_entry, keyword);
#endif
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_PERM, "Generating ip flow key entry failed, error %d\n", rc);

    add_connection->context_entry.fc_ucast_flow_context_entry.valid = 1;
    add_connection->context_entry.fc_ucast_flow_context_entry.connection_direction = direction;

    memcpy(ip_flow, &add_connection->context_entry, sizeof(rdd_fc_context_t));

#ifdef FIRMWARE_LITTLE_ENDIAN
    SWAPBYTES(ip_flow, FC_UCAST_FLOW_CONTEXT_ENTRY_COMMAND_LIST_OFFSET);
#endif

#if defined(USE_NATC_VAR_CONTEXT_LEN)
    rc = drv_natc_key_result_entry_var_size_ctx_add(direction, hash_keyword, keyword, (uint8_t *) &context_entry, &add_connection->entry_index);
#else
    rc = drv_natc_key_result_entry_add(direction, keyword, (uint8_t *) &context_entry, &add_connection->entry_index);
#endif
    if (rc==BDMF_ERR_NOENT) {
       /* Hash table is full. Skip adding but don't report an error. entry_index remains INVALID */
       BDMF_TRACE_INFO("Adding connection to nat cache failed due to no_ent, rc = %d\n", rc);
       return 0;
    }
    else if(rc)
        BDMF_TRACE_RET(BDMF_ERR_PERM, "Adding connection to nat cache failed %d\n", rc);
    
    add_connection->entry_index = SET_BDMF_INDEX_FROM_NATC_INDEX(add_connection->entry_index, direction);
    add_connection->lookup_entry->prot = nat_cache_lkp_entry_protocol;

    return 0;
}

int rdd_connection_entry_delete(bdmf_index flow_entry_index)
{
    int rc = BDMF_ERR_OK;
    uint32_t index = GET_NATC_INDEX_FROM_BDMF_INDEX(flow_entry_index);
    uint8_t tbl_idx = GET_TABLE_ACCORDING_TO_INDEX(flow_entry_index);

    rc = drv_natc_entry_delete(tbl_idx, index, 1);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_INTERNAL, "IP flow deletion failed, error=%d\n", rc);

    return 0;
}

int rdd_connection_entry_get(rdpa_traffic_dir direction, uint32_t entry_index,
    rdpa_ip_flow_key_t *nat_cache_lkp_entry, bdmf_index *flow_entry_index)
{
    bdmf_error_t rc = BDMF_ERR_OK;
    uint8_t keyword[NATC_MAX_ENTRY_LEN] = {};
    uint8_t sub_tbl_id = 0;
    bdmf_boolean valid = 0;
    uint32_t index = GET_NATC_INDEX_FROM_BDMF_INDEX(entry_index);
    uint8_t tbl_idx = GET_TABLE_ACCORDING_TO_INDEX(entry_index);

    rc = drv_natc_key_entry_get(tbl_idx, index, &valid, keyword);

    if (!valid)
        rc = BDMF_ERR_NOENT;

    if (rc)
        return rc;

    rdd_ip_class_key_entry_decompose(nat_cache_lkp_entry, &sub_tbl_id, keyword);

    if (sub_tbl_id)
        return BDMF_ERR_NOENT;

    rdd_ipproto_lookup_port_get(&nat_cache_lkp_entry->prot, &nat_cache_lkp_entry->lookup_port);

    nat_cache_lkp_entry->dir = tbl_idx;

    *flow_entry_index = entry_index;

    return 0;
}

int rdd_connection_entry_search(rdd_ip_flow_t *get_connection, rdpa_traffic_dir direction,
    bdmf_index *entry_index)
{
    bdmf_error_t rc = BDMF_ERR_OK;
    uint8_t keyword[NATC_MAX_ENTRY_LEN] = {};
    uint32_t hash_idx;
    uint8_t nat_cache_lkp_entry_protocol;

    nat_cache_lkp_entry_protocol = get_connection->lookup_entry->prot;
    rdd_prot_set ( &get_connection->lookup_entry->prot, get_connection->lookup_entry->lookup_port );

    rc = rdd_ip_class_key_entry_compose(get_connection->lookup_entry, keyword);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_PERM, "Generating ip flow key entry failed, error %d\n", rc);

    rc = drv_natc_key_idx_get(direction, keyword, &hash_idx, (uint32_t *)entry_index);

    get_connection->lookup_entry->prot = nat_cache_lkp_entry_protocol;

    return rc;
}

int rdd_context_entry_get(bdmf_index flow_entry_index, rdd_fc_context_t *context)
{
    bdmf_error_t rc = BDMF_ERR_OK;
    natc_result_entry_t context_entry = {};
    uint8_t *ip_flow = (uint8_t *) &context_entry.rdd_ip_flow;
    uint32_t index = GET_NATC_INDEX_FROM_BDMF_INDEX(flow_entry_index);
    uint8_t tbl_idx = GET_TABLE_ACCORDING_TO_INDEX(flow_entry_index);

    rc = drv_natc_result_entry_get(tbl_idx, index, (uint8_t *) &context_entry);
    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_PERM, "Retrieving context entry failed, error %d\n", rc);

#ifdef FIRMWARE_LITTLE_ENDIAN
    SWAPBYTES(ip_flow, FC_UCAST_FLOW_CONTEXT_ENTRY_COMMAND_LIST_OFFSET);
#endif

    if (!context_entry.rdd_ip_flow.fc_ucast_flow_context_entry.valid)
        rc = BDMF_ERR_NOENT;

    memcpy(context, ip_flow, sizeof(rdd_fc_context_t));

    return rc;
}

int rdd_context_entry_modify(rdd_fc_context_t *context, bdmf_index flow_entry_index)
{
    bdmf_error_t rc = BDMF_ERR_OK;
    natc_result_entry_t context_entry = {};
    uint8_t *ip_flow = (uint8_t *) &context_entry.rdd_ip_flow;
    uint32_t index = GET_NATC_INDEX_FROM_BDMF_INDEX(flow_entry_index);
    uint8_t tbl_idx = GET_TABLE_ACCORDING_TO_INDEX(flow_entry_index);

    memcpy(ip_flow, context, sizeof(rdd_fc_context_t));

#ifdef FIRMWARE_LITTLE_ENDIAN
    SWAPBYTES(ip_flow, FC_UCAST_FLOW_CONTEXT_ENTRY_COMMAND_LIST_OFFSET);
#endif

    rc = drv_natc_result_entry_add(tbl_idx, index, (uint8_t *) &context_entry);

    if (rc)
        BDMF_TRACE_RET(BDMF_ERR_PERM, "Adding context entry failed, error %d\n", rc);

    return rc;
}

int rdd_context_entry_flwstat_get(bdmf_index flow_entry_index, rdd_fc_context_t *context_entry)
{
    return rdd_context_entry_get(flow_entry_index, context_entry);
}

int rdd_flow_counters_get(bdmf_index flow_entry_index, uint32_t *stat_packets, uint32_t *stat_bytes)
{
    bdmf_error_t rc = BDMF_ERR_OK;
    uint64_t packets = 0;
    uint64_t bytes = 0;
    uint32_t index = GET_NATC_INDEX_FROM_BDMF_INDEX(flow_entry_index);
    uint8_t tbl_idx = GET_TABLE_ACCORDING_TO_INDEX(flow_entry_index);

    rc = drv_natc_entry_counters_get(tbl_idx, index, &packets, &bytes);
    if (rc == BDMF_ERR_OK)
    {
        *stat_packets = (uint32_t)packets;
        *stat_bytes = (uint32_t)bytes;
    }

    return rc;
}
/* Wrapper mapping functions to convert rdpa_wan_type to
 * egress phy type. */
static int2int_map_t rdpa_wan_type_to_rdd_egress_phy[] =
{
    {rdpa_wan_gbe, rdd_egress_phy_eth_wan},
    {rdpa_wan_dsl, rdd_egress_phy_dsl},
    {rdpa_wan_gpon, rdd_egress_phy_gpon},
    {BDMF_ERR_PARM, BDMF_ERR_PARM},
};

int rdpa_wan_type2rdd_egress_phy(rdpa_wan_type src)
{
    return int2int_map(rdpa_wan_type_to_rdd_egress_phy, src, BDMF_ERR_PARM);
}

rdpa_wan_type rdd_egress_phy2rdpa_wan_type(int src)
{
    return int2int_map_r(rdpa_wan_type_to_rdd_egress_phy, src, BDMF_ERR_PARM);
}

int rdd_fc_flow_ip_addresses_add(RDD_FC_FLOW_IP_ADDRESSES_ENTRY_DTS *ip_addresses_entry,
    bdmf_index *entry_index, uint16_t *entry_sram_address)
{

    bdmf_error_t rc = BDMF_ERR_NORES;
    RDD_FC_FLOW_IP_ADDRESSES_ENTRY_DTS curr, available;
    uint8_t *addr;
    uint32_t i, j;
    uint32_t core_index;

    *entry_index = *entry_sram_address = ip_addresses_entry->reference_count = 0;

    for (i = 0; i < RDD_FC_FLOW_IP_ADDRESSES_TABLE_SIZE; i++)
    {
        for (j = 0, addr = curr.sa_da_addresses; j < RDD_FC_FLOW_IP_ADDRESSES_ENTRY_SA_DA_ADDRESSES_NUMBER; j++, addr++)
            RDD_FC_FLOW_IP_ADDRESSES_ENTRY_SA_DA_ADDRESSES_READ_G(*addr, RDD_FC_FLOW_IP_ADDRESSES_TABLE_ADDRESS_ARR, i, j);

        if (!memcmp(curr.sa_da_addresses, ip_addresses_entry->sa_da_addresses, sizeof(curr.sa_da_addresses)))
        {
            /* Entry is already in the table, update the reference count and return the entry index and address. */
            RDD_FC_FLOW_IP_ADDRESSES_ENTRY_REFERENCE_COUNT_READ_G(ip_addresses_entry->reference_count, RDD_FC_FLOW_IP_ADDRESSES_TABLE_ADDRESS_ARR, i);
            ip_addresses_entry->reference_count++;
            RDD_FC_FLOW_IP_ADDRESSES_ENTRY_REFERENCE_COUNT_WRITE_G(ip_addresses_entry->reference_count, RDD_FC_FLOW_IP_ADDRESSES_TABLE_ADDRESS_ARR, i);

            *entry_index = i;

            for (core_index = 0; core_index < GROUPED_EN_SEGMENTS_NUM; core_index++)
            {
                if (RDD_FC_FLOW_IP_ADDRESSES_TABLE_ADDRESS_ARR[core_index] != INVALID_TABLE_ADDRESS)
                {
                    /* The SRAM address is configured to be the same for all cores. */
                    *entry_sram_address = RDD_FC_FLOW_IP_ADDRESSES_TABLE_ADDRESS_ARR[core_index] + (i * sizeof(RDD_FC_FLOW_IP_ADDRESSES_ENTRY_DTS));
                    break;
                }
            }

            rc = BDMF_ERR_OK;
            break;
        }
    }

    if (rc != BDMF_ERR_OK)
    {
        memset((uint8_t *) &available, 0x00, sizeof(RDD_FC_FLOW_IP_ADDRESSES_ENTRY_DTS));

        for (i = 0; i < RDD_FC_FLOW_IP_ADDRESSES_TABLE_SIZE; i++)
        {
            for (j = 0, addr = curr.sa_da_addresses; j < RDD_FC_FLOW_IP_ADDRESSES_ENTRY_SA_DA_ADDRESSES_NUMBER; j++, addr++)
                RDD_FC_FLOW_IP_ADDRESSES_ENTRY_SA_DA_ADDRESSES_READ_G(*addr, RDD_FC_FLOW_IP_ADDRESSES_TABLE_ADDRESS_ARR, i, j);

            if (!memcmp(curr.sa_da_addresses, available.sa_da_addresses, sizeof(curr.sa_da_addresses)))
            {
                /* Add new entry */
                for (j = 0, addr = ip_addresses_entry->sa_da_addresses; j < RDD_FC_FLOW_IP_ADDRESSES_ENTRY_SA_DA_ADDRESSES_NUMBER; j++, addr++)
                {
                    RDD_FC_FLOW_IP_ADDRESSES_ENTRY_SA_DA_ADDRESSES_WRITE_G(*addr, RDD_FC_FLOW_IP_ADDRESSES_TABLE_ADDRESS_ARR, i, j);
                }
                ip_addresses_entry->reference_count = 1;
                RDD_FC_FLOW_IP_ADDRESSES_ENTRY_REFERENCE_COUNT_WRITE_G(ip_addresses_entry->reference_count, RDD_FC_FLOW_IP_ADDRESSES_TABLE_ADDRESS_ARR, i);
                RDD_FC_FLOW_IP_ADDRESSES_ENTRY_IS_IPV6_ADDRESS_WRITE_G(ip_addresses_entry->is_ipv6_address, RDD_FC_FLOW_IP_ADDRESSES_TABLE_ADDRESS_ARR, i);
                *entry_index = i;

                for (core_index = 0; core_index < GROUPED_EN_SEGMENTS_NUM; core_index++)
                {
                    if (RDD_FC_FLOW_IP_ADDRESSES_TABLE_ADDRESS_ARR[core_index] != INVALID_TABLE_ADDRESS)
                    {
                        /* The SRAM address is configured to be the same for all cores. */
                        *entry_sram_address = RDD_FC_FLOW_IP_ADDRESSES_TABLE_ADDRESS_ARR[core_index] + (i * sizeof(RDD_FC_FLOW_IP_ADDRESSES_ENTRY_DTS));
                        break;
                    }
                }

                rc = BDMF_ERR_OK;
                break;
            }
        }
    }

    return rc;
}

int rdd_fc_flow_ip_addresses_get(bdmf_index entry_index, RDD_FC_FLOW_IP_ADDRESSES_ENTRY_DTS *ip_addresses_entry,
    uint16_t *entry_sram_address)
{
    bdmf_error_t rc = BDMF_ERR_NORES;
    uint8_t *addr;
    uint32_t j;
    uint32_t core_index;

    *entry_sram_address = 0;
    memset( (uint8_t *) ip_addresses_entry, 0x00, sizeof(RDD_FC_FLOW_IP_ADDRESSES_ENTRY_DTS));

    if (entry_index < RDD_FC_FLOW_IP_ADDRESSES_TABLE_SIZE)
    {
        RDD_FC_FLOW_IP_ADDRESSES_ENTRY_REFERENCE_COUNT_READ_G(ip_addresses_entry->reference_count, RDD_FC_FLOW_IP_ADDRESSES_TABLE_ADDRESS_ARR, entry_index);
        RDD_FC_FLOW_IP_ADDRESSES_ENTRY_IS_IPV6_ADDRESS_READ_G(ip_addresses_entry->is_ipv6_address, RDD_FC_FLOW_IP_ADDRESSES_TABLE_ADDRESS_ARR, entry_index);

        if (ip_addresses_entry->reference_count)
        {
            for (j = 0, addr = ip_addresses_entry->sa_da_addresses; j < RDD_FC_FLOW_IP_ADDRESSES_ENTRY_SA_DA_ADDRESSES_NUMBER; j++, addr++)
            {
                RDD_FC_FLOW_IP_ADDRESSES_ENTRY_SA_DA_ADDRESSES_READ_G(*addr, RDD_FC_FLOW_IP_ADDRESSES_TABLE_ADDRESS_ARR, entry_index, j);
            }

            for (core_index = 0; core_index < GROUPED_EN_SEGMENTS_NUM; core_index++)
            {
                if (RDD_FC_FLOW_IP_ADDRESSES_TABLE_ADDRESS_ARR[core_index] != INVALID_TABLE_ADDRESS)
                {
                    /* The SRAM address is configured to be the same for all cores. */
                    *entry_sram_address = RDD_FC_FLOW_IP_ADDRESSES_TABLE_ADDRESS_ARR[core_index] + (entry_index * sizeof(RDD_FC_FLOW_IP_ADDRESSES_ENTRY_DTS));
                    break;
                }
            }

            rc = BDMF_ERR_OK;
        }
    }

    return rc;
}

int rdd_fc_flow_ip_addresses_delete_by_index(bdmf_index entry_index)
{
    bdmf_error_t rc = BDMF_ERR_NORES;
    uint16_t reference_count;
    uint32_t i;

    if (entry_index < RDD_FC_FLOW_IP_ADDRESSES_TABLE_SIZE)
    {
        RDD_FC_FLOW_IP_ADDRESSES_ENTRY_REFERENCE_COUNT_READ_G(reference_count, RDD_FC_FLOW_IP_ADDRESSES_TABLE_ADDRESS_ARR, entry_index);
        if (reference_count > 0)
        {
            reference_count--;
            RDD_FC_FLOW_IP_ADDRESSES_ENTRY_REFERENCE_COUNT_WRITE_G(reference_count, RDD_FC_FLOW_IP_ADDRESSES_TABLE_ADDRESS_ARR, entry_index);
        }

        if (reference_count == 0)
        {
            for (i = 0; i < RDD_FC_FLOW_IP_ADDRESSES_ENTRY_SA_DA_ADDRESSES_NUMBER; i++)
            {
                RDD_FC_FLOW_IP_ADDRESSES_ENTRY_SA_DA_ADDRESSES_WRITE_G(0, RDD_FC_FLOW_IP_ADDRESSES_TABLE_ADDRESS_ARR, entry_index, i);
            }
        }

        rc = BDMF_ERR_OK;
    }

    return rc;
}

int rdd_ipv4_host_address_table_set(uint32_t table_index, bdmf_ipv4 ipv4_host_addr, uint16_t ref_count)
{
    RDD_IPV4_HOST_ADDRESS_TABLE_DTS *host_table;
    uint32_t core_index;

    if (table_index >= RDD_IPV4_HOST_ADDRESS_TABLE_SIZE)
    {
        return BDMF_ERR_RANGE;
    }

    for (core_index = 0; core_index < GROUPED_EN_SEGMENTS_NUM; core_index++)
    {
        if (RDD_FC_FLOW_IP_ADDRESSES_TABLE_ADDRESS_ARR[core_index] != INVALID_TABLE_ADDRESS)
        {
            host_table = RDD_IPV4_HOST_ADDRESS_TABLE_PTR(core_index);
            MWRITE_32(host_table->entry + table_index, ipv4_host_addr);
            g_ipv4_host_ref_count_table[table_index] = ref_count;
        }
    }

    return 0;
}

int rdd_ipv4_host_address_table_get(uint32_t table_index, bdmf_ipv4 *ipv4_host_addr, uint16_t *ref_count)
{
    RDD_IPV4_HOST_ADDRESS_TABLE_DTS *host_table;
    uint32_t core_index;

    if (table_index >= RDD_IPV4_HOST_ADDRESS_TABLE_SIZE)
    {
        return BDMF_ERR_RANGE;
    }

    for (core_index = 0; core_index < GROUPED_EN_SEGMENTS_NUM; core_index++)
    {
        if (RDD_FC_FLOW_IP_ADDRESSES_TABLE_ADDRESS_ARR[core_index] != INVALID_TABLE_ADDRESS)
        {
            host_table = RDD_IPV4_HOST_ADDRESS_TABLE_PTR(core_index);

            /*Retrieve host address from RDD table*/
            MREAD_32(host_table->entry + table_index, *ipv4_host_addr);

            /*Retrieve reference count from local table*/
            *ref_count = g_ipv4_host_ref_count_table[table_index];
        }
    }

    return 0;
}

int rdd_ipv6_host_address_table_set(uint32_t table_index, const bdmf_ipv6_t *ipv6_host_addr, uint16_t ref_count)
{
    RDD_IPV6_HOST_ADDRESS_CRC_TABLE_DTS *host_table;
    uint32_t ipv6_crc;
    uint32_t crc_init_value;
    uint32_t core_index;

    if (table_index >= RDD_IPV6_HOST_ADDRESS_CRC_TABLE_SIZE)
    {
        return BDMF_ERR_RANGE;
    }

    for (core_index = 0; core_index < GROUPED_EN_SEGMENTS_NUM; core_index++)
    {
        if (RDD_FC_FLOW_IP_ADDRESSES_TABLE_ADDRESS_ARR[core_index] != INVALID_TABLE_ADDRESS)
        {
            host_table = RDD_IPV6_HOST_ADDRESS_CRC_TABLE_PTR(core_index);

            /*Reduce IPV6 address to a 32-bit value using CRC. This reduced value is what RDP FW will be using for lookup.*/
            /* rdd_crc_ipv6_addr_calc(ipv6_host_addr, &ipv6_crc); */
            crc_init_value = rdd_crc_init_value_get(RDD_CRC_TYPE_32);
            ipv6_crc = rdd_crc_bit_by_bit(ipv6_host_addr->data, 16, 0, crc_init_value, RDD_CRC_TYPE_32);

            /*Store ipv6 address in a local table so we can return in the get accessor*/
            g_ipv6_host_table[table_index].ipv6_address = *ipv6_host_addr;
            g_ipv6_host_table[table_index].ref_count = ref_count;

            /*Store the CRC in the RDP FW table*/
            MWRITE_32( host_table->entry + table_index, ipv6_crc);
        }
    }

    return 0;
}

int rdd_ipv6_host_address_table_get(uint32_t table_index, bdmf_ipv6_t *ipv6_host_addr, uint16_t *ref_count)
{
    if (table_index >= RDD_IPV6_HOST_ADDRESS_CRC_TABLE_SIZE)
    {
        return BDMF_ERR_RANGE;
    }

    /*Look up address in local table. The full IP address is not stored in an RDP table, only the CRC is.*/
    *ipv6_host_addr = g_ipv6_host_table[table_index].ipv6_address;
    *ref_count = g_ipv6_host_table[table_index].ref_count;

    return 0;
}

int rdd_ucast_ds_wan_udp_filter_get(bdmf_index entry_index, RDD_DS_WAN_UDP_FILTER_ENTRY_DTS *ds_wan_udp_filter_entry)
{
    return 0;
}

int rdd_ucast_ds_wan_udp_filter_add( RDD_DS_WAN_UDP_FILTER_ENTRY_DTS  *ds_wan_udp_filter_entry, bdmf_index *entry_index_res)
{
    return 0;
}

int rdd_ucast_ds_wan_udp_filter_delete( bdmf_index entry_index )
{
    return 0;
}

int rdpa_if_wan2rdd_egress_phy(rdpa_if src)
{
    return 0;
}


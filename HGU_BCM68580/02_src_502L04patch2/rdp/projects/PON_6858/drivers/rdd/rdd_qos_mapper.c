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

#include "rdd.h"
#include "rdd_common.h"
#include "rdd_qos_mapper.h"
#include "rdpa_qos_mapper.h"
#include "rdd_defs.h"
#include "rdp_drv_qm.h"

#define QOS_MAPPER_TABLE_UNASSIGNED RDPA_TC_TO_QUEUE_ID_MAX_TABLES
#define RDD_QOS_MAPPER_INDEX_UNASSIGNED RDD_QOS_MAPPER_ID_MAX_TABLES
#define RDD_QOS_MAPPER_TABLE_FREE 0
#define RDD_QOS_MAPPER_TABLE_ALLOCATED 1

#define QM_QUEUE_TO_BYTE_QUEUE(qm_queue) ( qm_queue < drv_qm_get_ds_start() ? qm_queue : (qm_queue - drv_qm_get_ds_start()))

uint8_t g_free_qos_mapper_tables_map[RDD_QOS_MAPPER_ID_MAX_TABLES] = {};
uint8_t g_table_ptr_to_rdd_table_index_mapping[RDD_TX_FLOW_TABLE_SIZE] = {};
uint8_t g_table_ptr_to_rdpa_table_index_mapping[RDD_TX_FLOW_TABLE_SIZE] = {};
int g_drop_q;

void rdd_qos_mapper_init()
{
    int i;

    g_drop_q = QM_QUEUE_TO_BYTE_QUEUE(QM_QUEUE_DROP);
    for (i = 0; i < RDD_TX_FLOW_TABLE_SIZE; i++)
        g_table_ptr_to_rdd_table_index_mapping[i] = RDD_QOS_MAPPER_INDEX_UNASSIGNED;
    for (i = 0; i < RDD_QOS_MAPPER_ID_MAX_TABLES; i++)
        g_free_qos_mapper_tables_map[i] = RDD_QOS_MAPPER_TABLE_FREE;
    /* set all DS tc to queue table entries to DS drop queue */
    for (i = 0; i < RDD_DS_QOS_MAPPER_ID_MAX_TABLES * 8; i++)
    {
        RDD_TC_TO_QUEUE_8_QUEUE_OFFSET_WRITE_G(g_drop_q, RDD_TC_TO_QUEUE_TABLE_ADDRESS_ARR, i / 8, i % 8);
        RDD_PBIT_TO_QUEUE_8_QUEUE_OFFSET_WRITE_G(g_drop_q, RDD_PBIT_TO_QUEUE_TABLE_ADDRESS_ARR, i / 8, i % 8);
    }
    /* Set drop table to to DS drop queue */
    for (i = (RDD_TC_TO_QUEUE_TABLE_SIZE - 1) * 8; i < RDD_TC_TO_QUEUE_TABLE_SIZE * 8; i++)
    {
        RDD_TC_TO_QUEUE_8_QUEUE_OFFSET_WRITE_G(g_drop_q, RDD_TC_TO_QUEUE_TABLE_ADDRESS_ARR, i / 8, i % 8);
        RDD_PBIT_TO_QUEUE_8_QUEUE_OFFSET_WRITE_G(g_drop_q, RDD_PBIT_TO_QUEUE_TABLE_ADDRESS_ARR, i / 8, i % 8);
    }
    /* Set all TC_TO_QUEUE table pointers to invalid table ptr*/
    for (i = 0; i < RDD_TX_FLOW_TABLE_SIZE; i++)
        RDD_TX_FLOW_ENTRY_QOS_TABLE_PTR_WRITE_G(RDD_QOS_MAPPER_INDEX_UNASSIGNED, RDD_TX_FLOW_TABLE_ADDRESS_ARR, i);
    return;
}

static void _rdd_tc_to_queue_set_q(uint8_t rdd_index, uint8_t tc, uint16_t qm_queue_index)
{
    uint16_t entry_offset = rdd_index * RDD_TC_TO_QUEUE_8_QUEUE_OFFSET_NUMBER + tc;

    RDD_BTRACE("Setting TC %d to qm_queue %d at table number rdd_index %d\n", tc, qm_queue_index, rdd_index);
    GROUP_MWRITE_8(RDD_TC_TO_QUEUE_TABLE_ADDRESS_ARR, entry_offset, QM_QUEUE_TO_BYTE_QUEUE(qm_queue_index));
}

static void _rdd_pbit_to_queue_set_q(uint8_t rdd_index, uint8_t pbit, uint16_t qm_queue_index)
{
    uint16_t entry_offset = rdd_index * RDD_TC_TO_QUEUE_8_QUEUE_OFFSET_NUMBER + pbit;
    RDD_BTRACE("Setting PBIT %d to qm_queue %d at table number rdd_index %d\n", pbit, qm_queue_index, rdd_index);
    GROUP_MWRITE_8(RDD_PBIT_TO_QUEUE_TABLE_ADDRESS_ARR, entry_offset, QM_QUEUE_TO_BYTE_QUEUE(qm_queue_index));
}

/* TODO: implement
static void  _rdd_to_to_queue_copy_table(uint8_t old_idx, uint8_t alloc_idx, uint8_t vport_or_gem,
            rdpa_traffic_dir dir)
{
    return;
}
*/

static bdmf_error_t _rdd_alloc_tc_to_queue_table(rdpa_traffic_dir dir, uint16_t port_or_wan_flow, uint8_t *alloc_idx)
{
    uint8_t start_idx, search_depth;
    int i, free_counter = 0;
    uint16_t tx_flow = PORT_OR_WAN_FLOW_TO_TX_FLOW(port_or_wan_flow, dir);

    if (dir == rdpa_dir_ds)
    {
        RDD_BTRACE("Allocating a DS TC to Queue table for port %d\n", port_or_wan_flow);
        start_idx = 0;
        search_depth = RDD_DS_QOS_MAPPER_ID_MAX_TABLES;
    }
    else
    {
        RDD_BTRACE("Allocating an US TC to Queue table for wan_flow %d\n", port_or_wan_flow);
        start_idx = RDD_DS_QOS_MAPPER_ID_MAX_TABLES;
        search_depth = RDD_US_QOS_MAPPER_ID_MAX_TABLES;
    }
    for (i = start_idx; i < start_idx + search_depth; i++)
    {
        free_counter = (g_free_qos_mapper_tables_map[i] == RDD_QOS_MAPPER_TABLE_FREE) ? free_counter + 1 : 0;
        /* rdd table index is aligned to the required table size and there are enough free table entries */
        if (free_counter)
        {
            *alloc_idx = i;
            goto exit;
        }
    }
    RDD_TRACE("TC to Queue table memory depleted, cannot allocate table");
    return BDMF_ERR_NOMEM;

exit:
    /* allocate the new table */
    g_free_qos_mapper_tables_map[*alloc_idx] = RDD_QOS_MAPPER_TABLE_ALLOCATED;
    g_table_ptr_to_rdd_table_index_mapping[tx_flow] = *alloc_idx;
    RDD_TX_FLOW_ENTRY_QOS_TABLE_PTR_WRITE_G(*alloc_idx, RDD_TX_FLOW_TABLE_ADDRESS_ARR, tx_flow);
    return BDMF_ERR_OK;
}

/* used for EPON CTC, point all tx flows to the same table */
void rdd_qos_mapper_set_table_id_to_tx_flow(uint16_t src_tx_flow, uint16_t dst_tx_flow)
{
    uint8_t table_id;
    RDD_TX_FLOW_ENTRY_QOS_TABLE_PTR_READ_G(table_id, RDD_TX_FLOW_TABLE_ADDRESS_ARR, src_tx_flow);
    RDD_TX_FLOW_ENTRY_QOS_TABLE_PTR_WRITE_G(table_id, RDD_TX_FLOW_TABLE_ADDRESS_ARR, dst_tx_flow);
}

void rdd_qos_mapper_invalidate_table(uint16_t port_or_wan_flow, rdpa_traffic_dir dir, uint8_t size)
{
    uint16_t tx_flow = PORT_OR_WAN_FLOW_TO_TX_FLOW(port_or_wan_flow, dir);
    uint8_t rdd_index = g_table_ptr_to_rdd_table_index_mapping[tx_flow];

    RDD_BTRACE("Invalidating RDD TC to Queue table %d of size %d and port/wan_flow %d\n",
        size, rdd_index, port_or_wan_flow);
    RDD_TX_FLOW_ENTRY_QOS_TABLE_PTR_WRITE_G(RDD_QOS_MAPPER_INDEX_UNASSIGNED, RDD_TX_FLOW_TABLE_ADDRESS_ARR, tx_flow);
    g_table_ptr_to_rdd_table_index_mapping[tx_flow] = RDD_QOS_MAPPER_INDEX_UNASSIGNED;
    g_free_qos_mapper_tables_map[rdd_index] = RDD_QOS_MAPPER_TABLE_FREE; 
}

bdmf_error_t rdd_realloc_tc_to_queue_table(uint16_t port_or_wan_flow,
    rdpa_traffic_dir dir, uint8_t *size)
{
    /*bdmf_error_t rc;
    uint8_t temp_rdd_idx, alloc_idx;*/

    /* TODO:Implement */
    return BDMF_ERR_OK;
}

uint8_t _rdd_qos_mapping_entry_set(uint16_t port_or_wan_flow, rdpa_traffic_dir dir)
{
    uint16_t tx_flow = PORT_OR_WAN_FLOW_TO_TX_FLOW(port_or_wan_flow, dir);
    uint8_t rdd_index = g_table_ptr_to_rdd_table_index_mapping[tx_flow];

    /* tc to queue table not allocated -  allocate new table entry */
    if (rdd_index == RDD_QOS_MAPPER_INDEX_UNASSIGNED)
        _rdd_alloc_tc_to_queue_table(dir, port_or_wan_flow, &rdd_index);
    return rdd_index;
}

bdmf_error_t rdd_tc_to_queue_entry_set(uint16_t port_or_wan_flow, rdpa_traffic_dir dir,
        uint8_t *size, uint8_t tc, uint16_t qm_queue_index)
{
    uint8_t rdd_index;

    rdd_index = _rdd_qos_mapping_entry_set(port_or_wan_flow, dir);

    if (dir == rdpa_dir_ds)
        RDD_BTRACE("Setting TC %d to Queue %d of RDD table number %d and port %d\n",
                tc, qm_queue_index, rdd_index, port_or_wan_flow);
    else
        RDD_BTRACE("Setting TC %d to Queue %d of RDD table number %d and wan_flow %d\n",
                tc, qm_queue_index, rdd_index, port_or_wan_flow);

    _rdd_tc_to_queue_set_q(rdd_index, tc, qm_queue_index);
    return BDMF_ERR_OK;

}

bdmf_error_t rdd_pbit_to_queue_entry_set(uint16_t port_or_wan_flow, rdpa_traffic_dir dir, uint8_t pbit, 
    uint16_t qm_queue_index)
{
    uint8_t rdd_index;

    rdd_index = _rdd_qos_mapping_entry_set(port_or_wan_flow, dir);

    if (dir == rdpa_dir_ds)
        RDD_BTRACE("Setting PBIT %d to Queue %d of RDD table number %d and port %d\n",
                pbit, qm_queue_index, rdd_index, port_or_wan_flow);
    else
        RDD_BTRACE("Setting PBIT %d to Queue %d of RDD table number %d and wan_flow %d\n",
                pbit, qm_queue_index, rdd_index, port_or_wan_flow);

    _rdd_pbit_to_queue_set_q(rdd_index, pbit, qm_queue_index);
    return BDMF_ERR_OK;
}


bdmf_error_t rdd_us_pbits_to_wan_flow_entry_cfg(uint8_t gem_mapping_table, uint8_t pbit, uint8_t gem)
{
    GROUP_MWRITE_8(RDD_PBIT_TO_GEM_TABLE_ADDRESS_ARR, (gem_mapping_table * RDD_PBIT_TO_GEM_TABLE_SIZE) + pbit, gem);
    return BDMF_ERR_OK;
}

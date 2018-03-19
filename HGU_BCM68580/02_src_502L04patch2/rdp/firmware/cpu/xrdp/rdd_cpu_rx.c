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

#include "rdd_cpu_rx.h"
#ifndef _CFE_REDUCED_XRDP_
#include "rdp_drv_rnr.h"
#include "rdp_drv_proj_cntr.h"
#endif

#ifndef RDP_SIM
#define FEED_RING_INTERRUPT_THRESHOLD    0x2000
#else
#define FEED_RING_INTERRUPT_THRESHOLD    0x200
#endif
#define RECYCLE_RING_INTERRUPT_THRESHOLD 256
#define RECYCLE_RING_INTERRUPT_COALESCING_DEFAULT_VALUE  500


#ifndef _CFE_REDUCED_XRDP_
extern uintptr_t xrdp_virt2phys(const ru_block_rec *ru_block, uint8_t addr_idx);
#endif

#ifdef RDP_SIM
extern uint8_t *cpu_rx_ring_base_addr_ptr;
#endif

void rdd_cpu_if_init(void)
{
#ifndef _CFE_REDUCED_XRDP_
    uint32_t hi, lo;
    uintptr_t addr = xrdp_virt2phys(&RU_BLK(UBUS_SLV), 0) + RU_REG_OFFSET(UBUS_SLV, RNR_INTR_CTRL_ITR);

    GET_ADDR_HIGH_LOW(hi, lo, addr);

    RDD_DDR_ADDRESS_HIGH_WRITE_G(hi, RDD_CPU_RX_INTERRUPT_ID_DDR_ADDR_ADDRESS_ARR, 0);
    RDD_DDR_ADDRESS_LOW_WRITE_G(lo, RDD_CPU_RX_INTERRUPT_ID_DDR_ADDR_ADDRESS_ARR, 0);

    RDD_DDR_ADDRESS_HIGH_WRITE_G(hi, RDD_CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR_ADDRESS_ARR, 0);
    RDD_DDR_ADDRESS_LOW_WRITE_G(lo, RDD_CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR_ADDRESS_ARR, 0);
#endif
}

int rdd_ring_init(uint32_t ring_id, uint8_t type, bdmf_phys_addr_t phys_ring_address, uint32_t number_of_entries,
    uint32_t size_of_entry, uint32_t irq, uint32_t write_idx_init_val, bdmf_phys_addr_t phys_ring_index_addr)
{
    uint32_t addr_lo, addr_hi;
    int rc = BDMF_ERR_OK;

    if ((type > CPU_IF_RDD_LAST) || (ring_id > RING_ID_LAST))
        return BDMF_ERR_PARM;

    RDD_BTRACE("ring_id = %d, phys_ring_address = 0x%lx, number_of_entries = %d, size_of_entry = %d, irq = %d, write_idx_init_val = %d (not in use, "
        "ring_id used instead)\n",
        ring_id, (uintptr_t)phys_ring_address, number_of_entries, size_of_entry, irq, write_idx_init_val);

    if (type == CPU_IF_RDD_DATA)
    {
        RDD_CPU_RING_DESCRIPTOR_READ_IDX_WRITE_G(0, RDD_CPU_RING_DESCRIPTORS_TABLE_ADDRESS_ARR, ring_id);
        RDD_CPU_RING_DESCRIPTOR_WRITE_IDX_WRITE_G(0, RDD_CPU_RING_DESCRIPTORS_TABLE_ADDRESS_ARR, ring_id);
        RDD_CPU_RING_DESCRIPTOR_SIZE_OF_ENTRY_WRITE_G(size_of_entry, RDD_CPU_RING_DESCRIPTORS_TABLE_ADDRESS_ARR, ring_id);
        RDD_CPU_RING_DESCRIPTOR_NUMBER_OF_ENTRIES_WRITE_G((number_of_entries + 31) >> CPU_RING_SIZE_32_RESOLUTION, RDD_CPU_RING_DESCRIPTORS_TABLE_ADDRESS_ARR,
            ring_id);
        RDD_CPU_RING_DESCRIPTOR_INTERRUPT_ID_WRITE_G(irq, RDD_CPU_RING_DESCRIPTORS_TABLE_ADDRESS_ARR, ring_id);

        GET_ADDR_HIGH_LOW(addr_hi, addr_lo, phys_ring_address);
        RDD_CPU_RING_DESCRIPTOR_BASE_ADDR_LOW_WRITE_G(addr_lo, RDD_CPU_RING_DESCRIPTORS_TABLE_ADDRESS_ARR, ring_id);
        RDD_CPU_RING_DESCRIPTOR_BASE_ADDR_HIGH_WRITE_G(addr_hi, RDD_CPU_RING_DESCRIPTORS_TABLE_ADDRESS_ARR, ring_id);
    }
    else if (type == CPU_IF_RDD_RECYCLE)
    {
        RDD_CPU_RING_DESCRIPTOR_DTS *entry = get_recycle_ring_entry(ring_id);
        RDD_CPU_RECYCLE_RING_INDEX_DDR_ADDR_TABLE_DTS *index_addr;

        RDD_CPU_RING_DESCRIPTOR_WRITE_IDX_WRITE(0, entry);
        RDD_CPU_RING_DESCRIPTOR_READ_IDX_WRITE(0, entry);

        /* XXX: change to size_of_entry when sw is fixed */
        RDD_CPU_RING_DESCRIPTOR_SIZE_OF_ENTRY_WRITE(8, entry);
        RDD_CPU_RING_DESCRIPTOR_NUMBER_OF_ENTRIES_WRITE((number_of_entries + 31) >> CPU_RING_SIZE_32_RESOLUTION, entry);
        RDD_CPU_RING_DESCRIPTOR_INTERRUPT_ID_WRITE(irq, entry);

        GET_ADDR_HIGH_LOW(addr_hi, addr_lo, phys_ring_address);
        RDD_CPU_RING_DESCRIPTOR_BASE_ADDR_LOW_WRITE(addr_lo, entry);
        RDD_CPU_RING_DESCRIPTOR_BASE_ADDR_HIGH_WRITE(addr_hi, entry);

        /* Update write index address */
        GET_ADDR_HIGH_LOW(addr_hi, addr_lo, phys_ring_index_addr);
        if (ring_id == FEED_RCYCLE_RING_ID)
            index_addr = RDD_CPU_RECYCLE_RING_INDEX_DDR_ADDR_TABLE_PTR(get_runner_idx(cpu_rx_runner_image));
        else
            index_addr = RDD_CPU_RECYCLE_RING_INDEX_DDR_ADDR_TABLE_PTR(get_runner_idx(cpu_tx_runner_image));
        RDD_DDR_ADDRESS_HIGH_WRITE(addr_hi, index_addr);
        RDD_DDR_ADDRESS_LOW_WRITE(addr_lo, index_addr);

        /* Update recycle coalescing thresholds. Same to all cores, can afford WRITE_G */
        RDD_CPU_RING_INTERRUPT_COUNTER_ENTRY_MAX_SIZE_WRITE_G(RECYCLE_RING_INTERRUPT_THRESHOLD,
            RDD_CPU_RECYCLE_RING_INTERRUPT_COUNTER_TABLE_ADDRESS_ARR, 0);
        RDD_CPU_INTERRUPT_COALESCING_ENTRY_TIMER_PERIOD_WRITE_G(RECYCLE_RING_INTERRUPT_COALESCING_DEFAULT_VALUE,
            RDD_CPU_RECYCLE_INTERRUPT_COALESCING_TABLE_ADDRESS_ARR, 0);

        WMB();
#if !defined(G9991) && !defined(_CFE_REDUCED_XRDP_)
        /* After running slow-path performance tests decided not to do coalescing when recycling buffers from feed. Even
         * if will open it in future, in current implementation same coalescing task serves both data and recycle
         * queues, and for RX it is opened as part of data queues configuration. Should reconsider to use different
         * coalescing tasks data and recycle queues */
        if (ring_id == TX_RCYCLE_RING_ID)
        {
            rc = ag_drv_rnr_regs_cfg_cpu_wakeup_set(get_runner_idx(cpu_tx_runner_image),
                RECYCLE_INTERRUPT_COALESCING_THREAD_NUMBER);
        }
#endif
    }
    else if (type == CPU_IF_RDD_FEED)
    {
        RDD_CPU_RING_DESCRIPTOR_READ_IDX_WRITE_G(0, RDD_CPU_FEED_RING_DESCRIPTOR_TABLE_ADDRESS_ARR, 0);
        RDD_CPU_RING_DESCRIPTOR_WRITE_IDX_WRITE_G(write_idx_init_val, RDD_CPU_FEED_RING_DESCRIPTOR_TABLE_ADDRESS_ARR, 0);
        RDD_CPU_RING_DESCRIPTOR_SIZE_OF_ENTRY_WRITE_G(size_of_entry, RDD_CPU_FEED_RING_DESCRIPTOR_TABLE_ADDRESS_ARR, 0);
        RDD_CPU_RING_DESCRIPTOR_NUMBER_OF_ENTRIES_WRITE_G((number_of_entries + 31) >> CPU_RING_SIZE_64_RESOLUTION,
            RDD_CPU_FEED_RING_DESCRIPTOR_TABLE_ADDRESS_ARR, 0);
        RDD_CPU_RING_DESCRIPTOR_INTERRUPT_ID_WRITE_G(irq, RDD_CPU_FEED_RING_DESCRIPTOR_TABLE_ADDRESS_ARR, 0);

        GET_ADDR_HIGH_LOW(addr_hi, addr_lo, phys_ring_address);
        RDD_CPU_RING_DESCRIPTOR_BASE_ADDR_LOW_WRITE_G(addr_lo, RDD_CPU_FEED_RING_DESCRIPTOR_TABLE_ADDRESS_ARR, 0);
        RDD_CPU_RING_DESCRIPTOR_BASE_ADDR_HIGH_WRITE_G(addr_hi, RDD_CPU_FEED_RING_DESCRIPTOR_TABLE_ADDRESS_ARR, 0);
        RDD_BYTES_2_BITS_WRITE_G(FEED_RING_INTERRUPT_THRESHOLD, RDD_CPU_FEED_RING_INTERRUPT_THRESHOLD_ADDRESS_ARR, 0);
    }

    return rc;
}

int rdd_cpu_rx_ring_low_prio_set(uint32_t ring_id, uint8_t type, uint16_t threshold)
{
    if ((type > CPU_IF_RDD_LAST) || (ring_id > RING_ID_LAST))
        return BDMF_ERR_PARM;

    if (type == CPU_IF_RDD_DATA)
        RDD_CPU_RING_DESCRIPTOR_LOW_PRIORITY_THRESHOLD_WRITE_G(threshold, RDD_CPU_RING_DESCRIPTORS_TABLE_ADDRESS_ARR, ring_id);
    else if (type == CPU_IF_RDD_FEED)
        RDD_CPU_RING_DESCRIPTOR_LOW_PRIORITY_THRESHOLD_WRITE_G(threshold, RDD_CPU_FEED_RING_DESCRIPTOR_TABLE_ADDRESS_ARR, 0);
    else if (type == CPU_IF_RDD_RECYCLE)
        RDD_CPU_RING_DESCRIPTOR_LOW_PRIORITY_THRESHOLD_WRITE_G(threshold, RDD_CPU_RECYCLE_RING_DESCRIPTOR_TABLE_ADDRESS_ARR, 0);

    return BDMF_ERR_OK;
}

int rdd_ring_destroy(uint32_t ring_id)
{
    RDD_BTRACE("ring_id = %d\n", ring_id);
    RDD_TRACE("This is stub, consider to implement\n");

    /* TODO */
    return 0;
}

void rdd_cpu_tc_to_rqx_init(uint8_t def_rxq_idx)
{
    int i;

    RDD_BTRACE("def_rxq_idx = %d\n", def_rxq_idx);

    for (i = 0; i < RDD_CPU_REASON_TO_TC_SIZE; i++)
    {
        GROUP_MWRITE_8(RDD_TC_TO_CPU_RXQ_ADDRESS_ARR, i, def_rxq_idx);
        GROUP_MWRITE_8(RDD_EXC_TC_TO_CPU_RXQ_ADDRESS_ARR, i, (uint8_t)BDMF_INDEX_UNASSIGNED);
    }
}

void rdd_cpu_vport_cpu_obj_init(uint8_t def_cpu_obj_idx)
{
    int i;

    RDD_BTRACE("def_cpu_obj_idx = %d\n", def_cpu_obj_idx);

    for (i = 0; i < RDD_VPORT_TO_CPU_OBJ_SIZE; i++)
        GROUP_MWRITE_8(RDD_VPORT_TO_CPU_OBJ_ADDRESS_ARR, i, def_cpu_obj_idx);
}

void rdd_cpu_rx_meters_init(void)
{
    int i;

    RDD_BTRACE("\n");

    /* Reason to meters configuration */
    for (i = 0; i < RDD_US_CPU_REASON_TO_METER_TABLE_SIZE; i++)
    {
        GROUP_MWRITE_8(RDD_US_CPU_REASON_TO_METER_TABLE_ADDRESS_ARR, i, CPU_RX_METER_DISABLE);
        GROUP_MWRITE_8(RDD_DS_CPU_REASON_TO_METER_TABLE_ADDRESS_ARR, i, CPU_RX_METER_DISABLE);
    }

    for (i = 0; i < RDD_CPU_REASON_AND_VPORT_TO_METER_TABLE_SIZE; i++)
        GROUP_MWRITE_8(RDD_CPU_REASON_AND_VPORT_TO_METER_TABLE_ADDRESS_ARR, i, CPU_RX_METER_DISABLE);

    for (i = 0; i < RDD_CPU_VPORT_TO_METER_TABLE_SIZE; i++)
        GROUP_MWRITE_8(RDD_CPU_VPORT_TO_METER_TABLE_ADDRESS_ARR, i, CPU_RX_METER_DISABLE);
}

#ifndef _CFE_REDUCED_XRDP_
int rdd_cpu_rx_interrupt_coalescing_config(uint32_t ring_id, uint32_t timeout_us, uint32_t max_packet_count)
{
    int rc = BDMF_ERR_OK;
    static bdmf_boolean int_coalescing_init;

    if (ring_id > DATA_RING_ID_LAST)
        return BDMF_ERR_PARM;

    RDD_BTRACE("ring_id = %d\n", ring_id);

    RDD_CPU_INTERRUPT_COALESCING_ENTRY_TIMER_PERIOD_WRITE_G(timeout_us, RDD_CPU_INTERRUPT_COALESCING_TABLE_ADDRESS_ARR, 0);
    RDD_CPU_RING_INTERRUPT_COUNTER_ENTRY_MAX_SIZE_WRITE_G(max_packet_count, RDD_CPU_RING_INTERRUPT_COUNTER_TABLE_ADDRESS_ARR, ring_id);

    if (!int_coalescing_init)
    {
        /* Make sure the timer configured before the cpu weakeup */
        WMB();
        rc = ag_drv_rnr_regs_cfg_cpu_wakeup_set(get_runner_idx(cpu_rx_runner_image), INTERRUPT_COALESCING_THREAD_NUMBER);
        int_coalescing_init = 1;
    }

    return rc;
}

int rdd_cpu_rx_meter_drop_counter_get(int cpu_meter, rdpa_traffic_dir dir, uint16_t *drop_counter)
{
    uint32_t rx_cntr_arr[MAX_NUM_OF_COUNTERS_PER_READ] = {};
    uint32_t counter_id;
    int rc;
    RDD_BTRACE("cpu_meter = %d, dir = %d, drop_counter = %p\n",
        cpu_meter, dir, drop_counter);
    counter_id = cpu_meter;
    if (dir == rdpa_dir_us)
        counter_id += RDD_DS_CPU_RX_METER_TABLE_SIZE;

    rc = drv_cntr_counter_read(CPU_RX_METER_DROP_CNTR_GROUP_ID, counter_id, rx_cntr_arr);
    if (rc)
    {
        *drop_counter = 0;
        RDD_BTRACE("Could not read CPU Meter %d (dir=%s) drop counter, err: %d\n", cpu_meter,
            dir == rdpa_dir_ds ? "DS" : "US", rc);
        return BDMF_ERR_INTERNAL;
    }

    *drop_counter = rx_cntr_arr[0];
    return 0;
}

int rdd_cpu_rx_meter_config(int cpu_meter, uint16_t average_rate, uint16_t burst_size, rdpa_traffic_dir dir)
{
    uint16_t allocated_budget;
    uint32_t *addr_arr;

    RDD_BTRACE("cpu_meter = %d, average_rate = %d, burst_size = %d, dir = %d\n",
        cpu_meter, average_rate, burst_size, dir);

    addr_arr = dir == rdpa_dir_ds ? RDD_DS_CPU_RX_METER_TABLE_ADDRESS_ARR : RDD_US_CPU_RX_METER_TABLE_ADDRESS_ARR;
    allocated_budget = (uint16_t)rdd_rate_to_alloc_unit(average_rate, CPU_RX_METER_TIMER_PERIOD);

    RDD_CPU_RX_METER_ENTRY_CURRENT_BUDGET_WRITE_G(allocated_budget, addr_arr, cpu_meter);
    RDD_CPU_RX_METER_ENTRY_BUDGET_LIMIT_WRITE_G(burst_size, addr_arr, cpu_meter);
    RDD_CPU_RX_METER_ENTRY_ALLOCATED_BUDGET_WRITE_G(allocated_budget, addr_arr, cpu_meter);

    return 0;
}

int rdd_cpu_rx_queue_discard_get(uint32_t ring_id, uint16_t *num_of_packets)
{
#ifndef RDP_SIM
    RDD_CPU_RING_DESCRIPTOR_DTS *entry;
#endif

    RDD_BTRACE("ring_id = %d\n", ring_id);

    if (ring_id > RING_ID_LAST)
        return BDMF_ERR_PARM;

#ifndef RDP_SIM
    entry = ((RDD_CPU_RING_DESCRIPTOR_DTS *)RDD_CPU_RING_DESCRIPTORS_TABLE_PTR(get_runner_idx(cpu_rx_runner_image))) + ring_id;
    RDD_CPU_RING_DESCRIPTOR_DROP_COUNTER_READ(*num_of_packets, entry);
    RDD_CPU_RING_DESCRIPTOR_DROP_COUNTER_WRITE_G(0, RDD_CPU_RING_DESCRIPTORS_TABLE_ADDRESS_ARR, ring_id);
#else
    RDD_CPU_RING_DESCRIPTOR_DROP_COUNTER_READ_G(*num_of_packets, RDD_CPU_FEED_RING_DESCRIPTOR_TABLE_ADDRESS_ARR, ring_id);
    RDD_CPU_RING_DESCRIPTOR_DROP_COUNTER_WRITE_G(0, RDD_CPU_RING_DESCRIPTORS_TABLE_ADDRESS_ARR, ring_id);
#endif

    return BDMF_ERR_OK;
}

void rdd_cpu_tc_to_rxq_set(rdpa_cpu_port port, uint8_t tc, uint8_t rxq)
{
    RDD_BTRACE("cpu port = %d, tc = %d, queue = %d\n", (int)port, tc, rxq);

    GROUP_MWRITE_8(RDD_TC_TO_CPU_RXQ_ADDRESS_ARR, (port * RDPA_CPU_TC_NUM) + tc, rxq);
}

void rdd_cpu_exc_tc_to_rxq_set(rdpa_cpu_port port, uint8_t tc, uint8_t rxq)
{
    RDD_BTRACE("cpu port = %d, tc = %d, queue = %d\n", (int)port, tc, rxq);

    GROUP_MWRITE_8(RDD_EXC_TC_TO_CPU_RXQ_ADDRESS_ARR, (port * RDPA_CPU_TC_NUM) + tc, rxq);
}


void rdd_cpu_vport_cpu_obj_set(rdd_vport_id_t vport, uint8_t cpu_obj_idx)
{
    RDD_BTRACE("vport = %d, cpu_obj_idx = %d\n", (int)vport, cpu_obj_idx);

    GROUP_MWRITE_8(RDD_VPORT_TO_CPU_OBJ_ADDRESS_ARR, vport, cpu_obj_idx);
}

int rdd_cpu_reason_to_cpu_rx_meter(rdpa_cpu_reason reason, rdd_cpu_rx_meter meter, rdpa_traffic_dir dir,
    rdd_vport_vector_t vports_vector)
{
    int reason_offset, is_per_port = 0;

    RDD_BTRACE("reason = %d, meter = %d, dir = '%s', vports_vector = %lx\n", (int)reason, meter,
        dir == rdpa_dir_ds ? "ds" : "us", (unsigned long)vports_vector);

    if (dir == rdpa_dir_us)
    {
        switch (reason)
        {
        case rdpa_cpu_rx_reason_mcast:
            reason_offset = 0;
            is_per_port = 1;
            break;
        case rdpa_cpu_rx_reason_bcast:
            reason_offset = RDD_VPORT_LAST + 1;
            is_per_port = 1;
            break;
        case rdpa_cpu_rx_reason_unknown_da:
            reason_offset = (RDD_VPORT_LAST + 1) * 2;
            is_per_port = 1;
            break;
        default:
            break;
        }
    }

    if (is_per_port)
    {
        uint32_t *addr_arr = RDD_CPU_REASON_AND_VPORT_TO_METER_TABLE_ADDRESS_ARR;
        rdd_vport_id_t vport;

        /* For each port in vector, set the meter. For other, set CPU_RX_METER_DISABLE */
        for (vport = RDD_VPORT_FIRST; vport <= RDD_VPORT_LAST; vport++)
        {
            int vport_meter = meter;
            int curr_meter;

            if (!(vports_vector & (1LL << vport)))
                vport_meter = CPU_RX_METER_DISABLE;

            /* Meter update: if new meter set for vport in the mask - update */
            GROUP_MREAD_8(addr_arr, reason_offset + vport, curr_meter);
            if (vport_meter != CPU_RX_METER_DISABLE || curr_meter == meter)
            {
                RDD_BTRACE("Setting meter %d for reason %d (offset %d) for vport %d\n", vport_meter, reason,
                    reason_offset, vport);
                GROUP_MWRITE_8(addr_arr, reason_offset + vport, vport_meter);
            }
        }
    }
    else
    {
        uint32_t *addr_arr = dir == rdpa_dir_ds ? RDD_DS_CPU_REASON_TO_METER_TABLE_ADDRESS_ARR :
            RDD_US_CPU_REASON_TO_METER_TABLE_ADDRESS_ARR;

        GROUP_MWRITE_8(addr_arr, reason, meter);
    }

    return 0;
}

#endif  /* #ifndef _CFE_REDUCED_XRDP_ */

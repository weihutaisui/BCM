/*
   Copyright (c) 2015 Broadcom
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



/* This is an automated file. Do not edit its contents. */


#include "bdmf_shell.h"
#include "rdd_map_auto.h"
#include "rdd_runner_reg_dump.h"
#include "rdd_runner_reg_dump_addrs.h"
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DS_TM_PD_FIFO_TABLE =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT COMPLEX_SCHEDULER_TABLE =
{
	64,
	{
		{ dump_RDD_COMPLEX_SCHEDULER_DESCRIPTOR, 0x1400 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DS_TM_SCHEDULING_QUEUE_TABLE =
{
	8,
	{
		{ dump_RDD_SCHEDULING_QUEUE_DESCRIPTOR, 0x1800 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DS_TM_BBH_TX_EGRESS_COUNTER_TABLE =
{
	8,
	{
		{ dump_RDD_BBH_TX_EGRESS_COUNTER_ENTRY, 0x1d00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DS_TM_TM_FLOW_CNTR_TABLE =
{
	1,
	{
		{ dump_RDD_TM_FLOW_CNTR_ENTRY, 0x1d40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT REGISTERS_BUFFER =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x1d80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT BASIC_SCHEDULER_TABLE_DS =
{
	16,
	{
		{ dump_RDD_BASIC_SCHEDULER_DESCRIPTOR, 0x1e00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT BASIC_RATE_LIMITER_TABLE_DS =
{
	16,
	{
		{ dump_RDD_BASIC_RATE_LIMITER_DESCRIPTOR, 0x2000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT RX_FLOW_TABLE =
{
	2,
	{
		{ dump_RDD_RX_FLOW_ENTRY, 0x2800 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT BUDGET_ALLOCATION_TIMER_VALUE =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2a94 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DS_TM_BB_DESTINATION_TABLE =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2a96 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DS_TM_BBH_TX_QUEUE_ID_TABLE =
{
	4,
	{
		{ dump_RDD_BBH_TX_QUEUE_ID_ENTRY, 0x2a98 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT RUNNER_GLOBAL_REGISTERS_INIT =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2aa0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DS_TM_TM_ACTION_PTR_TABLE =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2ac0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DS_TM_SCHEDULING_GLOBAL_FLUSH_CFG =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2ae2 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT SCHEDULING_FLUSH_GLOBAL_CFG =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2ae3 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT TASK_IDX =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2ae4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_RX_INTERRUPT_ID_DDR_ADDR =
{
	8,
	{
		{ dump_RDD_DDR_ADDRESS, 0x2ae8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DS_TM_FLUSH_DISPATCHER_CREDIT_TABLE =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2af0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_FEED_RING_INTERRUPT_COUNTER_MAX =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2afc },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT TX_MIRRORING_CONFIGURATION =
{
	2,
	{
		{ dump_RDD_MIRRORING_DESCRIPTOR, 0x2afe },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DIRECT_PROCESSING_PD_TABLE =
{
	16,
	{
		{ dump_RDD_PROCESSING_RX_DESCRIPTOR, 0x2b00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DS_TM_SCHEDULING_FLUSH_VECTOR =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2b20 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT SRAM_DUMMY_STORE =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2b34 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT RATE_LIMIT_OVERHEAD =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2b35 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT MAC_TYPE =
{
	1,
	{
		{ dump_RDD_MAC_TYPE_ENTRY, 0x2b36 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DS_TM_FIRST_QUEUE_MAPPING =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2b37 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT GENERAL_QUEUE_DYNAMIC_MNG_TABLE =
{
	8,
	{
		{ dump_RDD_QUEUE_DYNAMIC_MNG_ENTRY, 0x2b38 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT UPDATE_FIFO_TABLE =
{
	4,
	{
		{ dump_RDD_UPDATE_FIFO_ENTRY, 0x2b40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DS_TM_SCHEDULING_QUEUE_AGING_VECTOR =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2b60 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT ACB_PORT_STATUS_TABLE =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2b78 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DS_TM_SCHEDULING_AGGREGATION_CONTEXT_VECTOR =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2b80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT SCRATCH =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2b98 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT MIRRORING_SCRATCH =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0x2bb8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT ACB_PACKET_SENT_PD_COPY =
{
	16,
	{
		{ dump_RDD_BYTES_16, 0x2bc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT TX_MIRRORING_DISPATCHER_CREDIT_TABLE =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2bd0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT RATE_LIMITER_VALID_TABLE_DS =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2be0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_FEED_RING_DESCRIPTOR_TABLE =
{
	16,
	{
		{ dump_RDD_CPU_RING_DESCRIPTOR, 0x2bf0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT RUNNER_PROFILING_TRACE_BUFFER =
{
	4,
	{
		{ dump_RDD_TRACE_EVENT, 0x2c00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DS_TM_BBH_QUEUE_TABLE =
{
	4,
	{
		{ dump_RDD_BBH_QUEUE_DESCRIPTOR, 0x2e00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT FPM_GLOBAL_CFG =
{
	12,
	{
		{ dump_RDD_FPM_GLOBAL_CFG, 0x2e20 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DIRECT_PROCESSING_EPON_CONTROL_SCRATCH =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2e30 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DS_TM_EGRESS_PORT_RR_TABLE =
{
	2,
	{
		{ dump_RDD_DS_TM_EGRESS_PORT_RR_ENTRY, 0x2f00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DEBUG_TRACE =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x3e00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT US_TM_PD_FIFO_TABLE_1 =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT COMPLEX_SCHEDULER_TABLE_1 =
{
	64,
	{
		{ dump_RDD_COMPLEX_SCHEDULER_DESCRIPTOR, 0x1400 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT US_TM_SCHEDULING_QUEUE_TABLE_1 =
{
	8,
	{
		{ dump_RDD_SCHEDULING_QUEUE_DESCRIPTOR, 0x1800 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT REPORTING_COUNTER_TABLE_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x1d00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT RUNNER_GLOBAL_REGISTERS_INIT_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x1da0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT US_TM_BBH_TX_EGRESS_COUNTER_TABLE_1 =
{
	8,
	{
		{ dump_RDD_BBH_TX_EGRESS_COUNTER_ENTRY, 0x1dc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT BASIC_SCHEDULER_TABLE_US_1 =
{
	16,
	{
		{ dump_RDD_BASIC_SCHEDULER_DESCRIPTOR, 0x1e00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT REPORTING_QUEUE_DESCRIPTOR_TABLE_1 =
{
	16,
	{
		{ dump_RDD_REPORTING_QUEUE_DESCRIPTOR, 0x2000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT US_TM_BBH_QUEUE_TABLE_1 =
{
	4,
	{
		{ dump_RDD_BBH_QUEUE_DESCRIPTOR, 0x2a00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT US_TM_SCHEDULING_FLUSH_VECTOR_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2aa0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT GHOST_REPORTING_GLOBAL_CFG_1 =
{
	4,
	{
		{ dump_RDD_GHOST_REPORTING_GLOBAL_CFG_ENTRY, 0x2ab4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT US_TM_BBH_TX_QUEUE_ID_TABLE_1 =
{
	4,
	{
		{ dump_RDD_BBH_TX_QUEUE_ID_ENTRY, 0x2ab8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT US_TM_TM_ACTION_PTR_TABLE_1 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2ac0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT BUDGET_ALLOCATION_TIMER_VALUE_1 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2ae2 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT US_TM_BB_DESTINATION_TABLE_1 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2ae4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT US_TM_SCHEDULING_GLOBAL_FLUSH_CFG_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2ae6 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT SCHEDULING_FLUSH_GLOBAL_CFG_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2ae7 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT GENERAL_QUEUE_DYNAMIC_MNG_TABLE_1 =
{
	8,
	{
		{ dump_RDD_QUEUE_DYNAMIC_MNG_ENTRY, 0x2ae8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT XGPON_REPORT_TABLE_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2af0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT US_TM_TM_FLOW_CNTR_TABLE_1 =
{
	1,
	{
		{ dump_RDD_TM_FLOW_CNTR_ENTRY, 0x2b00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT REPORTING_QUEUE_ACCUMULATED_TABLE_1 =
{
	8,
	{
		{ dump_RDD_REPORTING_ACCUMULATED_DATA, 0x2b80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT RUNNER_PROFILING_TRACE_BUFFER_1 =
{
	4,
	{
		{ dump_RDD_TRACE_EVENT, 0x2c00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT REGISTERS_BUFFER_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2e00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT UPDATE_FIFO_TABLE_1 =
{
	4,
	{
		{ dump_RDD_UPDATE_FIFO_ENTRY, 0x2e80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT US_TM_SCHEDULING_QUEUE_AGING_VECTOR_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2ea0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT TASK_IDX_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2eb4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT MIRRORING_SCRATCH_1 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0x2eb8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT US_TM_SCHEDULING_AGGREGATION_CONTEXT_VECTOR_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2ec0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT TX_MIRRORING_CONFIGURATION_1 =
{
	2,
	{
		{ dump_RDD_MIRRORING_DESCRIPTOR, 0x2ed4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT SRAM_DUMMY_STORE_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2ed6 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT RATE_LIMIT_OVERHEAD_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2ed7 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT REPORT_BBH_TX_QUEUE_ID_TABLE_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2ed8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT GHOST_REPORTING_QUEUE_STATUS_BIT_VECTOR_TABLE_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2ee0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT BBH_TX_FIFO_SIZE_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2ef4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT US_TM_FIRST_QUEUE_MAPPING_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2ef5 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT BBH_TX_INGRESS_COUNTER_TABLE_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2ef6 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT BBH_TX_EGRESS_REPORT_COUNTER_TABLE_1 =
{
	8,
	{
		{ dump_RDD_BBH_TX_EGRESS_COUNTER_ENTRY, 0x2ef8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT RATE_LIMITER_VALID_TABLE_US_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2f00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT OVERALL_RATE_LIMITER_TABLE_1 =
{
	16,
	{
		{ dump_RDD_OVERALL_RATE_LIMITER_DESCRIPTOR, 0x2f10 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT US_TM_FLUSH_DISPATCHER_CREDIT_TABLE_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2f20 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DISPATCHER_CREDIT_TABLE_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2f30 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT TX_MIRRORING_DISPATCHER_CREDIT_TABLE_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2f40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT FPM_GLOBAL_CFG_1 =
{
	12,
	{
		{ dump_RDD_FPM_GLOBAL_CFG, 0x2f50 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT XGPON_REPORT_ZERO_SENT_TABLE_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2f60 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT BASIC_RATE_LIMITER_TABLE_US_1 =
{
	16,
	{
		{ dump_RDD_BASIC_RATE_LIMITER_DESCRIPTOR, 0x3000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DEBUG_TRACE_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x3e00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DS_PACKET_BUFFER_2 =
{
	512,
	{
		{ dump_RDD_PACKET_BUFFER, 0x0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_TX_SCRATCHPAD_2 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0x1000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT RX_FLOW_TABLE_2 =
{
	2,
	{
		{ dump_RDD_RX_FLOW_ENTRY, 0x1800 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DS_WAN_UDP_FILTER_CONTROL_TABLE_2 =
{
	4,
	{
		{ dump_RDD_DS_WAN_UDP_FILTER_CONTROL_ENTRY, 0x1a94 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT FPM_REPLY_2 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0x1a98 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT RUNNER_GLOBAL_REGISTERS_INIT_2 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x1aa0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT GLOBAL_DSCP_TO_PBITS_TABLE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x1ac0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DSCP_TO_PBITS_MAP_TABLE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x1b00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT TC_TO_QUEUE_TABLE_2 =
{
	8,
	{
		{ dump_RDD_TC_TO_QUEUE_8, 0x1c00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT IPTV_CONFIGURATION_TABLE_2 =
{
	8,
	{
		{ dump_RDD_HW_IPTV_CONFIGURATION, 0x1e08 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DS_WAN_UDP_FILTER_TABLE_2 =
{
	16,
	{
		{ dump_RDD_DS_WAN_UDP_FILTER_ENTRY, 0x1e10 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT VPORT_TO_DSCP_TO_PBITS_TABLE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x1e20 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT FPM_POOL_NUMBER_MAPPING_TABLE_2 =
{
	1,
	{
		{ dump_RDD_FPM_POOL_NUMBER, 0x1e40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_TX_FPM_POOL_NUMBER_MAPPING_TABLE_2 =
{
	1,
	{
		{ dump_RDD_FPM_POOL_NUMBER, 0x1e80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT PBIT_TO_GEM_TABLE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x1ec0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_PROFILE_TABLE_2 =
{
	8,
	{
		{ dump_RDD_INGRESS_FILTER_CTRL, 0x1f00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DUAL_STACK_LITE_TABLE_2 =
{
	64,
	{
		{ dump_RDD_DUAL_STACK_LITE_ENTRY, 0x1f80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT GPE_COMMAND_PRIMITIVE_TABLE_2 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x1fc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT PBIT_TO_QUEUE_TABLE_2 =
{
	8,
	{
		{ dump_RDD_PBIT_TO_QUEUE_8, 0x2000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT REGISTERS_BUFFER_2 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2208 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT GENERAL_QUEUE_DYNAMIC_MNG_TABLE_2 =
{
	8,
	{
		{ dump_RDD_QUEUE_DYNAMIC_MNG_ENTRY, 0x2288 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT BROADCOM_SWITCH_PORT_TO_RX_FLOW_MAPPING_TABLE_2 =
{
	2,
	{
		{ dump_RDD_BROADCOM_SWITCH_PORT_MAPPING, 0x2290 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT TCAM_IC_CFG_TABLE_2 =
{
	16,
	{
		{ dump_RDD_TCAM_IC_CFG, 0x22a0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT LOOPBACK_QUEUE_TABLE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2300 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT PROCESSING_QUEUE_DYNAMIC_MNG_TABLE_2 =
{
	8,
	{
		{ dump_RDD_QUEUE_DYNAMIC_MNG_ENTRY, 0x2328 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT VPORT_TO_LOOKUP_PORT_MAPPING_TABLE_2 =
{
	2,
	{
		{ dump_RDD_BROADCOM_SWITCH_PORT_MAPPING, 0x2330 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT QUEUE_THRESHOLD_VECTOR_2 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2340 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT TASK_IDX_2 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2364 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT RX_FLOW_CONTEXT_DDR_ADDR_2 =
{
	8,
	{
		{ dump_RDD_DEF_FLOW_CONTEXT_DDR_ADDR, 0x2368 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_INTERRUPT_COALESCING_TABLE_2 =
{
	16,
	{
		{ dump_RDD_CPU_INTERRUPT_COALESCING_ENTRY, 0x2370 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT FLOW_BASED_ACTION_PTR_TABLE_2 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2380 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT FORCE_DSCP_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x23a2 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CORE_ID_TABLE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x23a3 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT RX_MIRRORING_CONFIGURATION_2 =
{
	2,
	{
		{ dump_RDD_MIRRORING_DESCRIPTOR, 0x23a4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT SRAM_DUMMY_STORE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x23a6 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT RATE_LIMIT_OVERHEAD_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x23a7 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT NULL_BUFFER_2 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0x23a8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_L2_REASON_TABLE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x23b0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_TX_EGRESS_UPDATE_FIFO_TABLE_2 =
{
	4,
	{
		{ dump_RDD_UPDATE_FIFO_ENTRY, 0x23c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT IPV4_HOST_ADDRESS_TABLE_2 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x23e0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT RUNNER_PROFILING_TRACE_BUFFER_2 =
{
	4,
	{
		{ dump_RDD_TRACE_EVENT, 0x2400 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT TCAM_IC_CMD_TABLE_2 =
{
	4,
	{
		{ dump_RDD_TCAM_IC_CMD, 0x2600 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_TX_INGRESS_UPDATE_FIFO_TABLE_2 =
{
	4,
	{
		{ dump_RDD_UPDATE_FIFO_ENTRY, 0x27c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT IPV6_HOST_ADDRESS_CRC_TABLE_2 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x27e0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT LAYER2_GRE_TUNNEL_TABLE_2 =
{
	32,
	{
		{ dump_RDD_LAYER2_GRE_TUNNEL_ENTRY, 0x2800 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_TX_EGRESS_PD_FIFO_TABLE_2 =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x2980 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT TCAM_IC_HANDLER_TABLE_2 =
{
	2,
	{
		{ dump_RDD_TCAM_IC_HANDLER, 0x29a0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_TX_INGRESS_PD_FIFO_TABLE_2 =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x29c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_TX_EGRESS_DISPATCHER_CREDIT_TABLE_2 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x29e0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_TX_RECYCLE_RING_ID_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x29ec },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT IC_DEBUG_COUNTER_MODE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x29ed },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_REDIRECT_MODE_2 =
{
	1,
	{
		{ dump_RDD_CPU_REDIRECT_MODE_ENTRY, 0x29ee },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT IPTV_CLASSIFICATION_CFG_TABLE_2 =
{
	1,
	{
		{ dump_RDD_IPTV_CLASSIFICATION_CFG_ENTRY, 0x29ef },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_TX_INGRESS_DISPATCHER_CREDIT_TABLE_2 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x29f0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_1588_CFG_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x29fc },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT VPORT_CFG_TABLE_2 =
{
	8,
	{
		{ dump_RDD_VPORT_CFG_ENTRY, 0x2a00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_RING_DESCRIPTOR_TABLE_2 =
{
	16,
	{
		{ dump_RDD_CPU_RING_DESCRIPTOR, 0x2b40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_RING_INDEX_DDR_ADDR_TABLE_2 =
{
	8,
	{
		{ dump_RDD_DDR_ADDRESS, 0x2b50 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR_2 =
{
	8,
	{
		{ dump_RDD_DDR_ADDRESS, 0x2b60 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_RX_INTERRUPT_ID_DDR_ADDR_2 =
{
	8,
	{
		{ dump_RDD_DDR_ADDRESS, 0x2b68 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_FEED_RING_DESCRIPTOR_TABLE_2 =
{
	16,
	{
		{ dump_RDD_CPU_RING_DESCRIPTOR, 0x2b70 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_INTERRUPT_SCRATCH_2 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2b80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT SYSTEM_CONFIGURATION_2 =
{
	5,
	{
		{ dump_RDD_SYSTEM_CONFIGURATION_ENTRY, 0x2b88 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT FPM_GLOBAL_CFG_2 =
{
	12,
	{
		{ dump_RDD_FPM_GLOBAL_CFG, 0x2b90 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_RING_INTERRUPT_COUNTER_TABLE_2 =
{
	8,
	{
		{ dump_RDD_CPU_RING_INTERRUPT_COUNTER_ENTRY, 0x2ba0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT TUNNELS_PARSING_CFG_2 =
{
	8,
	{
		{ dump_RDD_TUNNELS_PARSING_CFG, 0x2ba8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT IPTV_CFG_TABLE_2 =
{
	8,
	{
		{ dump_RDD_IPTV_CFG, 0x2bb0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT IPTV_DDR_CTX_TABLE_ADDRESS_2 =
{
	8,
	{
		{ dump_RDD_IPTV_DDR_CTX_TABLE_ADDRESS, 0x2bb8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT NAT_CACHE_CFG_2 =
{
	7,
	{
		{ dump_RDD_NAT_CACHE_CFG, 0x2bc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_CFG_2 =
{
	2,
	{
		{ dump_RDD_INGRESS_FILTER_CFG, 0x2bc8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT TX_FLOW_TABLE_2 =
{
	1,
	{
		{ dump_RDD_TX_FLOW_ENTRY, 0x2c00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_SHADOW_RD_IDX_2 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2d80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_SRAM_PD_FIFO_2 =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x2e00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_SHADOW_WR_IDX_2 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2f00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT FC_FLOW_IP_ADDRESSES_TABLE_2 =
{
	48,
	{
		{ dump_RDD_FC_FLOW_IP_ADDRESSES_ENTRY, 0x3d00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DEBUG_TRACE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x3e00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT RX_FLOW_TABLE_3 =
{
	2,
	{
		{ dump_RDD_RX_FLOW_ENTRY, 0x0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DS_WAN_UDP_FILTER_CONTROL_TABLE_3 =
{
	4,
	{
		{ dump_RDD_DS_WAN_UDP_FILTER_CONTROL_ENTRY, 0x294 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT IPTV_CONFIGURATION_TABLE_3 =
{
	8,
	{
		{ dump_RDD_HW_IPTV_CONFIGURATION, 0x298 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT RUNNER_GLOBAL_REGISTERS_INIT_3 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2a0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT GLOBAL_DSCP_TO_PBITS_TABLE_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DSCP_TO_PBITS_MAP_TABLE_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x300 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_RX_SCRATCHPAD_3 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0x400 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT TCAM_IC_CMD_TABLE_3 =
{
	4,
	{
		{ dump_RDD_TCAM_IC_CMD, 0x600 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT FPM_POOL_NUMBER_MAPPING_TABLE_3 =
{
	1,
	{
		{ dump_RDD_FPM_POOL_NUMBER, 0x7c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT TC_TO_QUEUE_TABLE_3 =
{
	8,
	{
		{ dump_RDD_TC_TO_QUEUE_8, 0x800 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT GENERAL_QUEUE_DYNAMIC_MNG_TABLE_3 =
{
	8,
	{
		{ dump_RDD_QUEUE_DYNAMIC_MNG_ENTRY, 0xa08 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DS_WAN_UDP_FILTER_TABLE_3 =
{
	16,
	{
		{ dump_RDD_DS_WAN_UDP_FILTER_ENTRY, 0xa10 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT VPORT_TO_DSCP_TO_PBITS_TABLE_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xa20 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT PBIT_TO_GEM_TABLE_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xa40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DS_CPU_RX_METER_TABLE_3 =
{
	8,
	{
		{ dump_RDD_CPU_RX_METER_ENTRY, 0xa80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_RING_INTERRUPT_COUNTER_TABLE_3 =
{
	8,
	{
		{ dump_RDD_CPU_RING_INTERRUPT_COUNTER_ENTRY, 0xb00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT BROADCOM_SWITCH_PORT_TO_RX_FLOW_MAPPING_TABLE_3 =
{
	2,
	{
		{ dump_RDD_BROADCOM_SWITCH_PORT_MAPPING, 0xb90 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT IPV4_HOST_ADDRESS_TABLE_3 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0xba0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DUAL_STACK_LITE_TABLE_3 =
{
	64,
	{
		{ dump_RDD_DUAL_STACK_LITE_ENTRY, 0xbc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT PBIT_TO_QUEUE_TABLE_3 =
{
	8,
	{
		{ dump_RDD_PBIT_TO_QUEUE_8, 0xc00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT PROCESSING_QUEUE_DYNAMIC_MNG_TABLE_3 =
{
	8,
	{
		{ dump_RDD_QUEUE_DYNAMIC_MNG_ENTRY, 0xe08 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT VPORT_TO_LOOKUP_PORT_MAPPING_TABLE_3 =
{
	2,
	{
		{ dump_RDD_BROADCOM_SWITCH_PORT_MAPPING, 0xe10 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT IPV6_HOST_ADDRESS_CRC_TABLE_3 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0xe20 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT GPE_COMMAND_PRIMITIVE_TABLE_3 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0xe40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT US_CPU_RX_METER_TABLE_3 =
{
	8,
	{
		{ dump_RDD_CPU_RX_METER_ENTRY, 0xe80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_PROFILE_TABLE_3 =
{
	8,
	{
		{ dump_RDD_INGRESS_FILTER_CTRL, 0xf00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_REASON_AND_VPORT_TO_METER_TABLE_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xf80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT RX_FLOW_CONTEXT_DDR_ADDR_3 =
{
	8,
	{
		{ dump_RDD_DEF_FLOW_CONTEXT_DDR_ADDR, 0xff8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DS_PACKET_BUFFER_3 =
{
	512,
	{
		{ dump_RDD_PACKET_BUFFER, 0x1000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT RUNNER_PROFILING_TRACE_BUFFER_3 =
{
	4,
	{
		{ dump_RDD_TRACE_EVENT, 0x2000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT LAYER2_GRE_TUNNEL_TABLE_3 =
{
	32,
	{
		{ dump_RDD_LAYER2_GRE_TUNNEL_ENTRY, 0x2200 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_RX_COPY_PD_FIFO_TABLE_3 =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x2380 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DS_CPU_REASON_TO_METER_TABLE_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x23c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT VPORT_CFG_TABLE_3 =
{
	8,
	{
		{ dump_RDD_VPORT_CFG_ENTRY, 0x2400 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT US_CPU_REASON_TO_METER_TABLE_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2540 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT REGISTERS_BUFFER_3 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2580 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT TX_FLOW_TABLE_3 =
{
	1,
	{
		{ dump_RDD_TX_FLOW_ENTRY, 0x2600 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT TCAM_IC_CFG_TABLE_3 =
{
	16,
	{
		{ dump_RDD_TCAM_IC_CFG, 0x2740 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT TCAM_IC_HANDLER_TABLE_3 =
{
	2,
	{
		{ dump_RDD_TCAM_IC_HANDLER, 0x27a0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT LOOPBACK_QUEUE_TABLE_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x27c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT NULL_BUFFER_3 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0x27e8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_INTERRUPT_COALESCING_TABLE_3 =
{
	16,
	{
		{ dump_RDD_CPU_INTERRUPT_COALESCING_ENTRY, 0x27f0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_SRAM_PD_FIFO_3 =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x2800 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_VPORT_TO_METER_TABLE_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2900 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR_3 =
{
	8,
	{
		{ dump_RDD_DDR_ADDRESS, 0x2928 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_RX_PSRAM_GET_NEXT_SCRATCHPAD_3 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2930 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT QUEUE_THRESHOLD_VECTOR_3 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2940 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_RX_INTERRUPT_SCRATCH_3 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2964 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_RX_INTERRUPT_ID_DDR_ADDR_3 =
{
	8,
	{
		{ dump_RDD_DDR_ADDRESS, 0x2968 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_L2_REASON_TABLE_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2970 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT FLOW_BASED_ACTION_PTR_TABLE_3 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2980 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT FORCE_DSCP_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x29a2 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CORE_ID_TABLE_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x29a3 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT TASK_IDX_3 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x29a4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT SYSTEM_CONFIGURATION_3 =
{
	5,
	{
		{ dump_RDD_SYSTEM_CONFIGURATION_ENTRY, 0x29a8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT SRAM_DUMMY_STORE_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x29ad },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_FEED_RING_INTERRUPT_THRESHOLD_3 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x29ae },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_RX_COPY_DISPATCHER_CREDIT_TABLE_3 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x29b0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_FEED_RING_INTERRUPT_COUNTER_3 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x29bc },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT RX_MIRRORING_CONFIGURATION_3 =
{
	2,
	{
		{ dump_RDD_MIRRORING_DESCRIPTOR, 0x29be },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT UPDATE_FIFO_TABLE_3 =
{
	4,
	{
		{ dump_RDD_UPDATE_FIFO_ENTRY, 0x29c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_RING_DESCRIPTOR_TABLE_3 =
{
	16,
	{
		{ dump_RDD_CPU_RING_DESCRIPTOR, 0x29e0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_RING_INDEX_DDR_ADDR_TABLE_3 =
{
	8,
	{
		{ dump_RDD_DDR_ADDRESS, 0x29f0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_RX_COPY_UPDATE_FIFO_TABLE_3 =
{
	4,
	{
		{ dump_RDD_UPDATE_FIFO_ENTRY, 0x2a00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_INTERRUPT_COALESCING_TABLE_3 =
{
	16,
	{
		{ dump_RDD_CPU_INTERRUPT_COALESCING_ENTRY, 0x2a20 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_FEED_RING_DESCRIPTOR_TABLE_3 =
{
	16,
	{
		{ dump_RDD_CPU_RING_DESCRIPTOR, 0x2a30 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT PD_FIFO_TABLE_3 =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x2a40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_REASON_TO_TC_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2a60 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT TC_TO_CPU_RXQ_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2aa0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT EXC_TC_TO_CPU_RXQ_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2ae0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_RX_LOCAL_SCRATCH_3 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0x2b20 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT FPM_GLOBAL_CFG_3 =
{
	12,
	{
		{ dump_RDD_FPM_GLOBAL_CFG, 0x2b30 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT RATE_LIMIT_OVERHEAD_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2b3c },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_TX_RECYCLE_RING_ID_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2b3d },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT IC_DEBUG_COUNTER_MODE_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2b3e },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_REDIRECT_MODE_3 =
{
	1,
	{
		{ dump_RDD_CPU_REDIRECT_MODE_ENTRY, 0x2b3f },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT VPORT_TO_CPU_OBJ_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2b40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_RING_INTERRUPT_COUNTER_TABLE_3 =
{
	8,
	{
		{ dump_RDD_CPU_RING_INTERRUPT_COUNTER_ENTRY, 0x2b68 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_RX_COPY_LOCAL_SCRATCH_3 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0x2b70 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT TUNNELS_PARSING_CFG_3 =
{
	8,
	{
		{ dump_RDD_TUNNELS_PARSING_CFG, 0x2b78 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_INTERRUPT_SCRATCH_3 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2b80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT IPTV_CFG_TABLE_3 =
{
	8,
	{
		{ dump_RDD_IPTV_CFG, 0x2b88 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT IPTV_DDR_CTX_TABLE_ADDRESS_3 =
{
	8,
	{
		{ dump_RDD_IPTV_DDR_CTX_TABLE_ADDRESS, 0x2b90 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT NAT_CACHE_CFG_3 =
{
	7,
	{
		{ dump_RDD_NAT_CACHE_CFG, 0x2b98 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT IPTV_CLASSIFICATION_CFG_TABLE_3 =
{
	1,
	{
		{ dump_RDD_IPTV_CLASSIFICATION_CFG_ENTRY, 0x2b9f },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_CFG_3 =
{
	2,
	{
		{ dump_RDD_INGRESS_FILTER_CFG, 0x2ba0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_1588_CFG_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2ba2 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_SHADOW_RD_IDX_3 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2c00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_SHADOW_WR_IDX_3 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2c80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_RING_DESCRIPTORS_TABLE_3 =
{
	16,
	{
		{ dump_RDD_CPU_RING_DESCRIPTOR, 0x3000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT FC_FLOW_IP_ADDRESSES_TABLE_3 =
{
	48,
	{
		{ dump_RDD_FC_FLOW_IP_ADDRESSES_ENTRY, 0x3d00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DEBUG_TRACE_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x3e00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DS_PACKET_BUFFER_4 =
{
	512,
	{
		{ dump_RDD_PACKET_BUFFER, 0x0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT RX_FLOW_TABLE_4 =
{
	2,
	{
		{ dump_RDD_RX_FLOW_ENTRY, 0x1000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DS_WAN_UDP_FILTER_CONTROL_TABLE_4 =
{
	4,
	{
		{ dump_RDD_DS_WAN_UDP_FILTER_CONTROL_ENTRY, 0x1294 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT IPTV_CONFIGURATION_TABLE_4 =
{
	8,
	{
		{ dump_RDD_HW_IPTV_CONFIGURATION, 0x1298 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT RUNNER_GLOBAL_REGISTERS_INIT_4 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x12a0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT GLOBAL_DSCP_TO_PBITS_TABLE_4 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x12c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DSCP_TO_PBITS_MAP_TABLE_4 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x1300 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT TC_TO_QUEUE_TABLE_4 =
{
	8,
	{
		{ dump_RDD_TC_TO_QUEUE_8, 0x1400 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT GENERAL_QUEUE_DYNAMIC_MNG_TABLE_4 =
{
	8,
	{
		{ dump_RDD_QUEUE_DYNAMIC_MNG_ENTRY, 0x1608 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DS_WAN_UDP_FILTER_TABLE_4 =
{
	16,
	{
		{ dump_RDD_DS_WAN_UDP_FILTER_ENTRY, 0x1610 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT VPORT_TO_DSCP_TO_PBITS_TABLE_4 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x1620 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT FPM_POOL_NUMBER_MAPPING_TABLE_4 =
{
	1,
	{
		{ dump_RDD_FPM_POOL_NUMBER, 0x1640 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT PBIT_TO_GEM_TABLE_4 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x1680 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DUAL_STACK_LITE_TABLE_4 =
{
	64,
	{
		{ dump_RDD_DUAL_STACK_LITE_ENTRY, 0x16c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_PROFILE_TABLE_4 =
{
	8,
	{
		{ dump_RDD_INGRESS_FILTER_CTRL, 0x1700 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT GPE_COMMAND_PRIMITIVE_TABLE_4 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x1780 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT LOOPBACK_QUEUE_TABLE_4 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x17c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT PROCESSING_QUEUE_DYNAMIC_MNG_TABLE_4 =
{
	8,
	{
		{ dump_RDD_QUEUE_DYNAMIC_MNG_ENTRY, 0x17e8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT BROADCOM_SWITCH_PORT_TO_RX_FLOW_MAPPING_TABLE_4 =
{
	2,
	{
		{ dump_RDD_BROADCOM_SWITCH_PORT_MAPPING, 0x17f0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT PBIT_TO_QUEUE_TABLE_4 =
{
	8,
	{
		{ dump_RDD_PBIT_TO_QUEUE_8, 0x1800 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT REGISTERS_BUFFER_4 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x1a08 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT RX_FLOW_CONTEXT_DDR_ADDR_4 =
{
	8,
	{
		{ dump_RDD_DEF_FLOW_CONTEXT_DDR_ADDR, 0x1a88 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT VPORT_TO_LOOKUP_PORT_MAPPING_TABLE_4 =
{
	2,
	{
		{ dump_RDD_BROADCOM_SWITCH_PORT_MAPPING, 0x1a90 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT TCAM_IC_CFG_TABLE_4 =
{
	16,
	{
		{ dump_RDD_TCAM_IC_CFG, 0x1aa0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT QUEUE_THRESHOLD_VECTOR_4 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x1b00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT TASK_IDX_4 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x1b24 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT NULL_BUFFER_4 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0x1b28 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_L2_REASON_TABLE_4 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x1b30 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT FLOW_BASED_ACTION_PTR_TABLE_4 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x1b40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT FORCE_DSCP_4 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x1b62 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CORE_ID_TABLE_4 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x1b63 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT RX_MIRRORING_CONFIGURATION_4 =
{
	2,
	{
		{ dump_RDD_MIRRORING_DESCRIPTOR, 0x1b64 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT SRAM_DUMMY_STORE_4 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x1b66 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT RATE_LIMIT_OVERHEAD_4 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x1b67 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT SYSTEM_CONFIGURATION_4 =
{
	5,
	{
		{ dump_RDD_SYSTEM_CONFIGURATION_ENTRY, 0x1b68 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT IC_DEBUG_COUNTER_MODE_4 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x1b6d },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_REDIRECT_MODE_4 =
{
	1,
	{
		{ dump_RDD_CPU_REDIRECT_MODE_ENTRY, 0x1b6e },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT IPTV_CLASSIFICATION_CFG_TABLE_4 =
{
	1,
	{
		{ dump_RDD_IPTV_CLASSIFICATION_CFG_ENTRY, 0x1b6f },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_FEED_RING_DESCRIPTOR_TABLE_4 =
{
	16,
	{
		{ dump_RDD_CPU_RING_DESCRIPTOR, 0x1b70 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT IPV4_HOST_ADDRESS_TABLE_4 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x1b80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT IPV6_HOST_ADDRESS_CRC_TABLE_4 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x1ba0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT TCAM_IC_HANDLER_TABLE_4 =
{
	2,
	{
		{ dump_RDD_TCAM_IC_HANDLER, 0x1bc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT FPM_GLOBAL_CFG_4 =
{
	12,
	{
		{ dump_RDD_FPM_GLOBAL_CFG, 0x1be0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_1588_CFG_4 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x1bec },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT TUNNELS_PARSING_CFG_4 =
{
	8,
	{
		{ dump_RDD_TUNNELS_PARSING_CFG, 0x1bf0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT IPTV_CFG_TABLE_4 =
{
	8,
	{
		{ dump_RDD_IPTV_CFG, 0x1bf8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT RUNNER_PROFILING_TRACE_BUFFER_4 =
{
	4,
	{
		{ dump_RDD_TRACE_EVENT, 0x1c00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT TCAM_IC_CMD_TABLE_4 =
{
	4,
	{
		{ dump_RDD_TCAM_IC_CMD, 0x1e00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT IPTV_DDR_CTX_TABLE_ADDRESS_4 =
{
	8,
	{
		{ dump_RDD_IPTV_DDR_CTX_TABLE_ADDRESS, 0x1fc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT NAT_CACHE_CFG_4 =
{
	7,
	{
		{ dump_RDD_NAT_CACHE_CFG, 0x1fc8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_CFG_4 =
{
	2,
	{
		{ dump_RDD_INGRESS_FILTER_CFG, 0x1fd0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT LAYER2_GRE_TUNNEL_TABLE_4 =
{
	32,
	{
		{ dump_RDD_LAYER2_GRE_TUNNEL_ENTRY, 0x2000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT VPORT_CFG_TABLE_4 =
{
	8,
	{
		{ dump_RDD_VPORT_CFG_ENTRY, 0x2200 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT TX_FLOW_TABLE_4 =
{
	1,
	{
		{ dump_RDD_TX_FLOW_ENTRY, 0x2400 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT FC_FLOW_IP_ADDRESSES_TABLE_4 =
{
	48,
	{
		{ dump_RDD_FC_FLOW_IP_ADDRESSES_ENTRY, 0x3d00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DEBUG_TRACE_4 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x3e00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DS_PACKET_BUFFER_5 =
{
	512,
	{
		{ dump_RDD_PACKET_BUFFER, 0x0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT RX_FLOW_TABLE_5 =
{
	2,
	{
		{ dump_RDD_RX_FLOW_ENTRY, 0x1000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DS_WAN_UDP_FILTER_CONTROL_TABLE_5 =
{
	4,
	{
		{ dump_RDD_DS_WAN_UDP_FILTER_CONTROL_ENTRY, 0x1294 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT IPTV_CONFIGURATION_TABLE_5 =
{
	8,
	{
		{ dump_RDD_HW_IPTV_CONFIGURATION, 0x1298 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT RUNNER_GLOBAL_REGISTERS_INIT_5 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x12a0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT GLOBAL_DSCP_TO_PBITS_TABLE_5 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x12c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DSCP_TO_PBITS_MAP_TABLE_5 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x1300 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT TC_TO_QUEUE_TABLE_5 =
{
	8,
	{
		{ dump_RDD_TC_TO_QUEUE_8, 0x1400 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT GENERAL_QUEUE_DYNAMIC_MNG_TABLE_5 =
{
	8,
	{
		{ dump_RDD_QUEUE_DYNAMIC_MNG_ENTRY, 0x1608 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DS_WAN_UDP_FILTER_TABLE_5 =
{
	16,
	{
		{ dump_RDD_DS_WAN_UDP_FILTER_ENTRY, 0x1610 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT VPORT_TO_DSCP_TO_PBITS_TABLE_5 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x1620 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT FPM_POOL_NUMBER_MAPPING_TABLE_5 =
{
	1,
	{
		{ dump_RDD_FPM_POOL_NUMBER, 0x1640 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT PBIT_TO_GEM_TABLE_5 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x1680 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DUAL_STACK_LITE_TABLE_5 =
{
	64,
	{
		{ dump_RDD_DUAL_STACK_LITE_ENTRY, 0x16c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_PROFILE_TABLE_5 =
{
	8,
	{
		{ dump_RDD_INGRESS_FILTER_CTRL, 0x1700 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT GPE_COMMAND_PRIMITIVE_TABLE_5 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x1780 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT LOOPBACK_QUEUE_TABLE_5 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x17c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT PROCESSING_QUEUE_DYNAMIC_MNG_TABLE_5 =
{
	8,
	{
		{ dump_RDD_QUEUE_DYNAMIC_MNG_ENTRY, 0x17e8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT BROADCOM_SWITCH_PORT_TO_RX_FLOW_MAPPING_TABLE_5 =
{
	2,
	{
		{ dump_RDD_BROADCOM_SWITCH_PORT_MAPPING, 0x17f0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT PBIT_TO_QUEUE_TABLE_5 =
{
	8,
	{
		{ dump_RDD_PBIT_TO_QUEUE_8, 0x1800 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT REGISTERS_BUFFER_5 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x1a08 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT RX_FLOW_CONTEXT_DDR_ADDR_5 =
{
	8,
	{
		{ dump_RDD_DEF_FLOW_CONTEXT_DDR_ADDR, 0x1a88 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT VPORT_TO_LOOKUP_PORT_MAPPING_TABLE_5 =
{
	2,
	{
		{ dump_RDD_BROADCOM_SWITCH_PORT_MAPPING, 0x1a90 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT TCAM_IC_CFG_TABLE_5 =
{
	16,
	{
		{ dump_RDD_TCAM_IC_CFG, 0x1aa0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT QUEUE_THRESHOLD_VECTOR_5 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x1b00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT TASK_IDX_5 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x1b24 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT NULL_BUFFER_5 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0x1b28 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_L2_REASON_TABLE_5 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x1b30 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT FLOW_BASED_ACTION_PTR_TABLE_5 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x1b40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT FORCE_DSCP_5 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x1b62 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CORE_ID_TABLE_5 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x1b63 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT RX_MIRRORING_CONFIGURATION_5 =
{
	2,
	{
		{ dump_RDD_MIRRORING_DESCRIPTOR, 0x1b64 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT SRAM_DUMMY_STORE_5 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x1b66 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT RATE_LIMIT_OVERHEAD_5 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x1b67 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT SYSTEM_CONFIGURATION_5 =
{
	5,
	{
		{ dump_RDD_SYSTEM_CONFIGURATION_ENTRY, 0x1b68 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT IC_DEBUG_COUNTER_MODE_5 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x1b6d },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_REDIRECT_MODE_5 =
{
	1,
	{
		{ dump_RDD_CPU_REDIRECT_MODE_ENTRY, 0x1b6e },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT IPTV_CLASSIFICATION_CFG_TABLE_5 =
{
	1,
	{
		{ dump_RDD_IPTV_CLASSIFICATION_CFG_ENTRY, 0x1b6f },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT CPU_FEED_RING_DESCRIPTOR_TABLE_5 =
{
	16,
	{
		{ dump_RDD_CPU_RING_DESCRIPTOR, 0x1b70 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT IPV4_HOST_ADDRESS_TABLE_5 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x1b80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT IPV6_HOST_ADDRESS_CRC_TABLE_5 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x1ba0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT TCAM_IC_HANDLER_TABLE_5 =
{
	2,
	{
		{ dump_RDD_TCAM_IC_HANDLER, 0x1bc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT FPM_GLOBAL_CFG_5 =
{
	12,
	{
		{ dump_RDD_FPM_GLOBAL_CFG, 0x1be0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_1588_CFG_5 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x1bec },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT TUNNELS_PARSING_CFG_5 =
{
	8,
	{
		{ dump_RDD_TUNNELS_PARSING_CFG, 0x1bf0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT IPTV_CFG_TABLE_5 =
{
	8,
	{
		{ dump_RDD_IPTV_CFG, 0x1bf8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT RUNNER_PROFILING_TRACE_BUFFER_5 =
{
	4,
	{
		{ dump_RDD_TRACE_EVENT, 0x1c00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT TCAM_IC_CMD_TABLE_5 =
{
	4,
	{
		{ dump_RDD_TCAM_IC_CMD, 0x1e00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT IPTV_DDR_CTX_TABLE_ADDRESS_5 =
{
	8,
	{
		{ dump_RDD_IPTV_DDR_CTX_TABLE_ADDRESS, 0x1fc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT NAT_CACHE_CFG_5 =
{
	7,
	{
		{ dump_RDD_NAT_CACHE_CFG, 0x1fc8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_CFG_5 =
{
	2,
	{
		{ dump_RDD_INGRESS_FILTER_CFG, 0x1fd0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT LAYER2_GRE_TUNNEL_TABLE_5 =
{
	32,
	{
		{ dump_RDD_LAYER2_GRE_TUNNEL_ENTRY, 0x2000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT VPORT_CFG_TABLE_5 =
{
	8,
	{
		{ dump_RDD_VPORT_CFG_ENTRY, 0x2200 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT TX_FLOW_TABLE_5 =
{
	1,
	{
		{ dump_RDD_TX_FLOW_ENTRY, 0x2400 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT FC_FLOW_IP_ADDRESSES_TABLE_5 =
{
	48,
	{
		{ dump_RDD_FC_FLOW_IP_ADDRESSES_ENTRY, 0x3d00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM63158
static DUMP_RUNNERREG_STRUCT DEBUG_TRACE_5 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x3e00 },
		{ 0, 0 },
	}
};
#endif

TABLE_STRUCT RUNNER_TABLES[NUMBER_OF_TABLES] =
{
#if defined BCM63158
	{ "DS_TM_PD_FIFO_TABLE", 1, CORE_0_INDEX, &DS_TM_PD_FIFO_TABLE, 320, 1, 1 },
#endif
#if defined BCM63158
	{ "COMPLEX_SCHEDULER_TABLE", 1, CORE_0_INDEX, &COMPLEX_SCHEDULER_TABLE, 16, 1, 1 },
#endif
#if defined BCM63158
	{ "DS_TM_SCHEDULING_QUEUE_TABLE", 1, CORE_0_INDEX, &DS_TM_SCHEDULING_QUEUE_TABLE, 160, 1, 1 },
#endif
#if defined BCM63158
	{ "DS_TM_BBH_TX_EGRESS_COUNTER_TABLE", 1, CORE_0_INDEX, &DS_TM_BBH_TX_EGRESS_COUNTER_TABLE, 8, 1, 1 },
#endif
#if defined BCM63158
	{ "DS_TM_TM_FLOW_CNTR_TABLE", 1, CORE_0_INDEX, &DS_TM_TM_FLOW_CNTR_TABLE, 64, 1, 1 },
#endif
#if defined BCM63158
	{ "REGISTERS_BUFFER", 1, CORE_0_INDEX, &REGISTERS_BUFFER, 32, 1, 1 },
#endif
#if defined BCM63158
	{ "BASIC_SCHEDULER_TABLE_DS", 1, CORE_0_INDEX, &BASIC_SCHEDULER_TABLE_DS, 32, 1, 1 },
#endif
#if defined BCM63158
	{ "BASIC_RATE_LIMITER_TABLE_DS", 1, CORE_0_INDEX, &BASIC_RATE_LIMITER_TABLE_DS, 128, 1, 1 },
#endif
#if defined BCM63158
	{ "RX_FLOW_TABLE", 1, CORE_0_INDEX, &RX_FLOW_TABLE, 330, 1, 1 },
#endif
#if defined BCM63158
	{ "BUDGET_ALLOCATION_TIMER_VALUE", 1, CORE_0_INDEX, &BUDGET_ALLOCATION_TIMER_VALUE, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "DS_TM_BB_DESTINATION_TABLE", 1, CORE_0_INDEX, &DS_TM_BB_DESTINATION_TABLE, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "DS_TM_BBH_TX_QUEUE_ID_TABLE", 1, CORE_0_INDEX, &DS_TM_BBH_TX_QUEUE_ID_TABLE, 2, 1, 1 },
#endif
#if defined BCM63158
	{ "RUNNER_GLOBAL_REGISTERS_INIT", 1, CORE_0_INDEX, &RUNNER_GLOBAL_REGISTERS_INIT, 8, 1, 1 },
#endif
#if defined BCM63158
	{ "DS_TM_TM_ACTION_PTR_TABLE", 1, CORE_0_INDEX, &DS_TM_TM_ACTION_PTR_TABLE, 17, 1, 1 },
#endif
#if defined BCM63158
	{ "DS_TM_SCHEDULING_GLOBAL_FLUSH_CFG", 1, CORE_0_INDEX, &DS_TM_SCHEDULING_GLOBAL_FLUSH_CFG, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "SCHEDULING_FLUSH_GLOBAL_CFG", 1, CORE_0_INDEX, &SCHEDULING_FLUSH_GLOBAL_CFG, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "TASK_IDX", 1, CORE_0_INDEX, &TASK_IDX, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_RX_INTERRUPT_ID_DDR_ADDR", 1, CORE_0_INDEX, &CPU_RX_INTERRUPT_ID_DDR_ADDR, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "DS_TM_FLUSH_DISPATCHER_CREDIT_TABLE", 1, CORE_0_INDEX, &DS_TM_FLUSH_DISPATCHER_CREDIT_TABLE, 3, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_FEED_RING_INTERRUPT_COUNTER_MAX", 1, CORE_0_INDEX, &CPU_FEED_RING_INTERRUPT_COUNTER_MAX, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "TX_MIRRORING_CONFIGURATION", 1, CORE_0_INDEX, &TX_MIRRORING_CONFIGURATION, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "DIRECT_PROCESSING_PD_TABLE", 1, CORE_0_INDEX, &DIRECT_PROCESSING_PD_TABLE, 2, 1, 1 },
#endif
#if defined BCM63158
	{ "DS_TM_SCHEDULING_FLUSH_VECTOR", 1, CORE_0_INDEX, &DS_TM_SCHEDULING_FLUSH_VECTOR, 5, 1, 1 },
#endif
#if defined BCM63158
	{ "SRAM_DUMMY_STORE", 1, CORE_0_INDEX, &SRAM_DUMMY_STORE, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "RATE_LIMIT_OVERHEAD", 1, CORE_0_INDEX, &RATE_LIMIT_OVERHEAD, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "MAC_TYPE", 1, CORE_0_INDEX, &MAC_TYPE, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "DS_TM_FIRST_QUEUE_MAPPING", 1, CORE_0_INDEX, &DS_TM_FIRST_QUEUE_MAPPING, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "GENERAL_QUEUE_DYNAMIC_MNG_TABLE", 1, CORE_0_INDEX, &GENERAL_QUEUE_DYNAMIC_MNG_TABLE, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "UPDATE_FIFO_TABLE", 1, CORE_0_INDEX, &UPDATE_FIFO_TABLE, 8, 1, 1 },
#endif
#if defined BCM63158
	{ "DS_TM_SCHEDULING_QUEUE_AGING_VECTOR", 1, CORE_0_INDEX, &DS_TM_SCHEDULING_QUEUE_AGING_VECTOR, 5, 1, 1 },
#endif
#if defined BCM63158
	{ "ACB_PORT_STATUS_TABLE", 1, CORE_0_INDEX, &ACB_PORT_STATUS_TABLE, 8, 1, 1 },
#endif
#if defined BCM63158
	{ "DS_TM_SCHEDULING_AGGREGATION_CONTEXT_VECTOR", 1, CORE_0_INDEX, &DS_TM_SCHEDULING_AGGREGATION_CONTEXT_VECTOR, 5, 1, 1 },
#endif
#if defined BCM63158
	{ "SCRATCH", 1, CORE_0_INDEX, &SCRATCH, 8, 1, 1 },
#endif
#if defined BCM63158
	{ "MIRRORING_SCRATCH", 1, CORE_0_INDEX, &MIRRORING_SCRATCH, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "ACB_PACKET_SENT_PD_COPY", 1, CORE_0_INDEX, &ACB_PACKET_SENT_PD_COPY, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "TX_MIRRORING_DISPATCHER_CREDIT_TABLE", 1, CORE_0_INDEX, &TX_MIRRORING_DISPATCHER_CREDIT_TABLE, 3, 1, 1 },
#endif
#if defined BCM63158
	{ "RATE_LIMITER_VALID_TABLE_DS", 1, CORE_0_INDEX, &RATE_LIMITER_VALID_TABLE_DS, 4, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_FEED_RING_DESCRIPTOR_TABLE", 1, CORE_0_INDEX, &CPU_FEED_RING_DESCRIPTOR_TABLE, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "RUNNER_PROFILING_TRACE_BUFFER", 1, CORE_0_INDEX, &RUNNER_PROFILING_TRACE_BUFFER, 128, 1, 1 },
#endif
#if defined BCM63158
	{ "DS_TM_BBH_QUEUE_TABLE", 1, CORE_0_INDEX, &DS_TM_BBH_QUEUE_TABLE, 8, 1, 1 },
#endif
#if defined BCM63158
	{ "FPM_GLOBAL_CFG", 1, CORE_0_INDEX, &FPM_GLOBAL_CFG, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "DIRECT_PROCESSING_EPON_CONTROL_SCRATCH", 1, CORE_0_INDEX, &DIRECT_PROCESSING_EPON_CONTROL_SCRATCH, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "DS_TM_EGRESS_PORT_RR_TABLE", 1, CORE_0_INDEX, &DS_TM_EGRESS_PORT_RR_TABLE, 3, 1, 1 },
#endif
#if defined BCM63158
	{ "DEBUG_TRACE", 1, CORE_0_INDEX, &DEBUG_TRACE, 512, 1, 1 },
#endif
#if defined BCM63158
	{ "US_TM_PD_FIFO_TABLE", 1, CORE_1_INDEX, &US_TM_PD_FIFO_TABLE_1, 320, 1, 1 },
#endif
#if defined BCM63158
	{ "COMPLEX_SCHEDULER_TABLE", 1, CORE_1_INDEX, &COMPLEX_SCHEDULER_TABLE_1, 16, 1, 1 },
#endif
#if defined BCM63158
	{ "US_TM_SCHEDULING_QUEUE_TABLE", 1, CORE_1_INDEX, &US_TM_SCHEDULING_QUEUE_TABLE_1, 160, 1, 1 },
#endif
#if defined BCM63158
	{ "REPORTING_COUNTER_TABLE", 1, CORE_1_INDEX, &REPORTING_COUNTER_TABLE_1, 40, 1, 1 },
#endif
#if defined BCM63158
	{ "RUNNER_GLOBAL_REGISTERS_INIT", 1, CORE_1_INDEX, &RUNNER_GLOBAL_REGISTERS_INIT_1, 8, 1, 1 },
#endif
#if defined BCM63158
	{ "US_TM_BBH_TX_EGRESS_COUNTER_TABLE", 1, CORE_1_INDEX, &US_TM_BBH_TX_EGRESS_COUNTER_TABLE_1, 8, 1, 1 },
#endif
#if defined BCM63158
	{ "BASIC_SCHEDULER_TABLE_US", 1, CORE_1_INDEX, &BASIC_SCHEDULER_TABLE_US_1, 32, 1, 1 },
#endif
#if defined BCM63158
	{ "REPORTING_QUEUE_DESCRIPTOR_TABLE", 1, CORE_1_INDEX, &REPORTING_QUEUE_DESCRIPTOR_TABLE_1, 160, 1, 1 },
#endif
#if defined BCM63158
	{ "US_TM_BBH_QUEUE_TABLE", 1, CORE_1_INDEX, &US_TM_BBH_QUEUE_TABLE_1, 40, 1, 1 },
#endif
#if defined BCM63158
	{ "US_TM_SCHEDULING_FLUSH_VECTOR", 1, CORE_1_INDEX, &US_TM_SCHEDULING_FLUSH_VECTOR_1, 5, 1, 1 },
#endif
#if defined BCM63158
	{ "GHOST_REPORTING_GLOBAL_CFG", 1, CORE_1_INDEX, &GHOST_REPORTING_GLOBAL_CFG_1, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "US_TM_BBH_TX_QUEUE_ID_TABLE", 1, CORE_1_INDEX, &US_TM_BBH_TX_QUEUE_ID_TABLE_1, 2, 1, 1 },
#endif
#if defined BCM63158
	{ "US_TM_TM_ACTION_PTR_TABLE", 1, CORE_1_INDEX, &US_TM_TM_ACTION_PTR_TABLE_1, 17, 1, 1 },
#endif
#if defined BCM63158
	{ "BUDGET_ALLOCATION_TIMER_VALUE", 1, CORE_1_INDEX, &BUDGET_ALLOCATION_TIMER_VALUE_1, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "US_TM_BB_DESTINATION_TABLE", 1, CORE_1_INDEX, &US_TM_BB_DESTINATION_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "US_TM_SCHEDULING_GLOBAL_FLUSH_CFG", 1, CORE_1_INDEX, &US_TM_SCHEDULING_GLOBAL_FLUSH_CFG_1, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "SCHEDULING_FLUSH_GLOBAL_CFG", 1, CORE_1_INDEX, &SCHEDULING_FLUSH_GLOBAL_CFG_1, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "GENERAL_QUEUE_DYNAMIC_MNG_TABLE", 1, CORE_1_INDEX, &GENERAL_QUEUE_DYNAMIC_MNG_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "XGPON_REPORT_TABLE", 1, CORE_1_INDEX, &XGPON_REPORT_TABLE_1, 4, 1, 1 },
#endif
#if defined BCM63158
	{ "US_TM_TM_FLOW_CNTR_TABLE", 1, CORE_1_INDEX, &US_TM_TM_FLOW_CNTR_TABLE_1, 128, 1, 1 },
#endif
#if defined BCM63158
	{ "REPORTING_QUEUE_ACCUMULATED_TABLE", 1, CORE_1_INDEX, &REPORTING_QUEUE_ACCUMULATED_TABLE_1, 16, 1, 1 },
#endif
#if defined BCM63158
	{ "RUNNER_PROFILING_TRACE_BUFFER", 1, CORE_1_INDEX, &RUNNER_PROFILING_TRACE_BUFFER_1, 128, 1, 1 },
#endif
#if defined BCM63158
	{ "REGISTERS_BUFFER", 1, CORE_1_INDEX, &REGISTERS_BUFFER_1, 32, 1, 1 },
#endif
#if defined BCM63158
	{ "UPDATE_FIFO_TABLE", 1, CORE_1_INDEX, &UPDATE_FIFO_TABLE_1, 8, 1, 1 },
#endif
#if defined BCM63158
	{ "US_TM_SCHEDULING_QUEUE_AGING_VECTOR", 1, CORE_1_INDEX, &US_TM_SCHEDULING_QUEUE_AGING_VECTOR_1, 5, 1, 1 },
#endif
#if defined BCM63158
	{ "TASK_IDX", 1, CORE_1_INDEX, &TASK_IDX_1, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "MIRRORING_SCRATCH", 1, CORE_1_INDEX, &MIRRORING_SCRATCH_1, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "US_TM_SCHEDULING_AGGREGATION_CONTEXT_VECTOR", 1, CORE_1_INDEX, &US_TM_SCHEDULING_AGGREGATION_CONTEXT_VECTOR_1, 5, 1, 1 },
#endif
#if defined BCM63158
	{ "TX_MIRRORING_CONFIGURATION", 1, CORE_1_INDEX, &TX_MIRRORING_CONFIGURATION_1, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "SRAM_DUMMY_STORE", 1, CORE_1_INDEX, &SRAM_DUMMY_STORE_1, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "RATE_LIMIT_OVERHEAD", 1, CORE_1_INDEX, &RATE_LIMIT_OVERHEAD_1, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "REPORT_BBH_TX_QUEUE_ID_TABLE", 1, CORE_1_INDEX, &REPORT_BBH_TX_QUEUE_ID_TABLE_1, 2, 1, 1 },
#endif
#if defined BCM63158
	{ "GHOST_REPORTING_QUEUE_STATUS_BIT_VECTOR_TABLE", 1, CORE_1_INDEX, &GHOST_REPORTING_QUEUE_STATUS_BIT_VECTOR_TABLE_1, 5, 1, 1 },
#endif
#if defined BCM63158
	{ "BBH_TX_FIFO_SIZE", 1, CORE_1_INDEX, &BBH_TX_FIFO_SIZE_1, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "US_TM_FIRST_QUEUE_MAPPING", 1, CORE_1_INDEX, &US_TM_FIRST_QUEUE_MAPPING_1, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "BBH_TX_INGRESS_COUNTER_TABLE", 1, CORE_1_INDEX, &BBH_TX_INGRESS_COUNTER_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "BBH_TX_EGRESS_REPORT_COUNTER_TABLE", 1, CORE_1_INDEX, &BBH_TX_EGRESS_REPORT_COUNTER_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "RATE_LIMITER_VALID_TABLE_US", 1, CORE_1_INDEX, &RATE_LIMITER_VALID_TABLE_US_1, 4, 1, 1 },
#endif
#if defined BCM63158
	{ "OVERALL_RATE_LIMITER_TABLE", 1, CORE_1_INDEX, &OVERALL_RATE_LIMITER_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "US_TM_FLUSH_DISPATCHER_CREDIT_TABLE", 1, CORE_1_INDEX, &US_TM_FLUSH_DISPATCHER_CREDIT_TABLE_1, 3, 1, 1 },
#endif
#if defined BCM63158
	{ "DISPATCHER_CREDIT_TABLE", 1, CORE_1_INDEX, &DISPATCHER_CREDIT_TABLE_1, 3, 1, 1 },
#endif
#if defined BCM63158
	{ "TX_MIRRORING_DISPATCHER_CREDIT_TABLE", 1, CORE_1_INDEX, &TX_MIRRORING_DISPATCHER_CREDIT_TABLE_1, 3, 1, 1 },
#endif
#if defined BCM63158
	{ "FPM_GLOBAL_CFG", 1, CORE_1_INDEX, &FPM_GLOBAL_CFG_1, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "XGPON_REPORT_ZERO_SENT_TABLE", 1, CORE_1_INDEX, &XGPON_REPORT_ZERO_SENT_TABLE_1, 10, 1, 1 },
#endif
#if defined BCM63158
	{ "BASIC_RATE_LIMITER_TABLE_US", 1, CORE_1_INDEX, &BASIC_RATE_LIMITER_TABLE_US_1, 128, 1, 1 },
#endif
#if defined BCM63158
	{ "DEBUG_TRACE", 1, CORE_1_INDEX, &DEBUG_TRACE_1, 512, 1, 1 },
#endif
#if defined BCM63158
	{ "DS_PACKET_BUFFER", 1, CORE_2_INDEX, &DS_PACKET_BUFFER_2, 8, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_TX_SCRATCHPAD", 1, CORE_2_INDEX, &CPU_TX_SCRATCHPAD_2, 256, 1, 1 },
#endif
#if defined BCM63158
	{ "RX_FLOW_TABLE", 1, CORE_2_INDEX, &RX_FLOW_TABLE_2, 330, 1, 1 },
#endif
#if defined BCM63158
	{ "DS_WAN_UDP_FILTER_CONTROL_TABLE", 1, CORE_2_INDEX, &DS_WAN_UDP_FILTER_CONTROL_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "FPM_REPLY", 1, CORE_2_INDEX, &FPM_REPLY_2, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "RUNNER_GLOBAL_REGISTERS_INIT", 1, CORE_2_INDEX, &RUNNER_GLOBAL_REGISTERS_INIT_2, 8, 1, 1 },
#endif
#if defined BCM63158
	{ "GLOBAL_DSCP_TO_PBITS_TABLE", 1, CORE_2_INDEX, &GLOBAL_DSCP_TO_PBITS_TABLE_2, 1, 64, 1 },
#endif
#if defined BCM63158
	{ "DSCP_TO_PBITS_MAP_TABLE", 1, CORE_2_INDEX, &DSCP_TO_PBITS_MAP_TABLE_2, 4, 64, 1 },
#endif
#if defined BCM63158
	{ "TC_TO_QUEUE_TABLE", 1, CORE_2_INDEX, &TC_TO_QUEUE_TABLE_2, 65, 1, 1 },
#endif
#if defined BCM63158
	{ "IPTV_CONFIGURATION_TABLE", 1, CORE_2_INDEX, &IPTV_CONFIGURATION_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "DS_WAN_UDP_FILTER_TABLE", 1, CORE_2_INDEX, &DS_WAN_UDP_FILTER_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "VPORT_TO_DSCP_TO_PBITS_TABLE", 1, CORE_2_INDEX, &VPORT_TO_DSCP_TO_PBITS_TABLE_2, 32, 1, 1 },
#endif
#if defined BCM63158
	{ "FPM_POOL_NUMBER_MAPPING_TABLE", 1, CORE_2_INDEX, &FPM_POOL_NUMBER_MAPPING_TABLE_2, 64, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_TX_FPM_POOL_NUMBER_MAPPING_TABLE", 1, CORE_2_INDEX, &CPU_TX_FPM_POOL_NUMBER_MAPPING_TABLE_2, 64, 1, 1 },
#endif
#if defined BCM63158
	{ "PBIT_TO_GEM_TABLE", 1, CORE_2_INDEX, &PBIT_TO_GEM_TABLE_2, 8, 8, 1 },
#endif
#if defined BCM63158
	{ "INGRESS_FILTER_PROFILE_TABLE", 1, CORE_2_INDEX, &INGRESS_FILTER_PROFILE_TABLE_2, 16, 1, 1 },
#endif
#if defined BCM63158
	{ "DUAL_STACK_LITE_TABLE", 1, CORE_2_INDEX, &DUAL_STACK_LITE_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "GPE_COMMAND_PRIMITIVE_TABLE", 1, CORE_2_INDEX, &GPE_COMMAND_PRIMITIVE_TABLE_2, 32, 1, 1 },
#endif
#if defined BCM63158
	{ "PBIT_TO_QUEUE_TABLE", 1, CORE_2_INDEX, &PBIT_TO_QUEUE_TABLE_2, 65, 1, 1 },
#endif
#if defined BCM63158
	{ "REGISTERS_BUFFER", 1, CORE_2_INDEX, &REGISTERS_BUFFER_2, 32, 1, 1 },
#endif
#if defined BCM63158
	{ "GENERAL_QUEUE_DYNAMIC_MNG_TABLE", 1, CORE_2_INDEX, &GENERAL_QUEUE_DYNAMIC_MNG_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "BROADCOM_SWITCH_PORT_TO_RX_FLOW_MAPPING_TABLE", 1, CORE_2_INDEX, &BROADCOM_SWITCH_PORT_TO_RX_FLOW_MAPPING_TABLE_2, 8, 1, 1 },
#endif
#if defined BCM63158
	{ "TCAM_IC_CFG_TABLE", 1, CORE_2_INDEX, &TCAM_IC_CFG_TABLE_2, 6, 1, 1 },
#endif
#if defined BCM63158
	{ "LOOPBACK_QUEUE_TABLE", 1, CORE_2_INDEX, &LOOPBACK_QUEUE_TABLE_2, 40, 1, 1 },
#endif
#if defined BCM63158
	{ "PROCESSING_QUEUE_DYNAMIC_MNG_TABLE", 1, CORE_2_INDEX, &PROCESSING_QUEUE_DYNAMIC_MNG_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "VPORT_TO_LOOKUP_PORT_MAPPING_TABLE", 1, CORE_2_INDEX, &VPORT_TO_LOOKUP_PORT_MAPPING_TABLE_2, 8, 1, 1 },
#endif
#if defined BCM63158
	{ "QUEUE_THRESHOLD_VECTOR", 1, CORE_2_INDEX, &QUEUE_THRESHOLD_VECTOR_2, 9, 1, 1 },
#endif
#if defined BCM63158
	{ "TASK_IDX", 1, CORE_2_INDEX, &TASK_IDX_2, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "RX_FLOW_CONTEXT_DDR_ADDR", 1, CORE_2_INDEX, &RX_FLOW_CONTEXT_DDR_ADDR_2, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_RECYCLE_INTERRUPT_COALESCING_TABLE", 1, CORE_2_INDEX, &CPU_RECYCLE_INTERRUPT_COALESCING_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "FLOW_BASED_ACTION_PTR_TABLE", 1, CORE_2_INDEX, &FLOW_BASED_ACTION_PTR_TABLE_2, 17, 1, 1 },
#endif
#if defined BCM63158
	{ "FORCE_DSCP", 1, CORE_2_INDEX, &FORCE_DSCP_2, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "CORE_ID_TABLE", 1, CORE_2_INDEX, &CORE_ID_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "RX_MIRRORING_CONFIGURATION", 1, CORE_2_INDEX, &RX_MIRRORING_CONFIGURATION_2, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "SRAM_DUMMY_STORE", 1, CORE_2_INDEX, &SRAM_DUMMY_STORE_2, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "RATE_LIMIT_OVERHEAD", 1, CORE_2_INDEX, &RATE_LIMIT_OVERHEAD_2, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "NULL_BUFFER", 1, CORE_2_INDEX, &NULL_BUFFER_2, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "INGRESS_FILTER_L2_REASON_TABLE", 1, CORE_2_INDEX, &INGRESS_FILTER_L2_REASON_TABLE_2, 16, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_TX_EGRESS_UPDATE_FIFO_TABLE", 1, CORE_2_INDEX, &CPU_TX_EGRESS_UPDATE_FIFO_TABLE_2, 8, 1, 1 },
#endif
#if defined BCM63158
	{ "IPV4_HOST_ADDRESS_TABLE", 1, CORE_2_INDEX, &IPV4_HOST_ADDRESS_TABLE_2, 8, 1, 1 },
#endif
#if defined BCM63158
	{ "RUNNER_PROFILING_TRACE_BUFFER", 1, CORE_2_INDEX, &RUNNER_PROFILING_TRACE_BUFFER_2, 128, 1, 1 },
#endif
#if defined BCM63158
	{ "TCAM_IC_CMD_TABLE", 1, CORE_2_INDEX, &TCAM_IC_CMD_TABLE_2, 7, 16, 1 },
#endif
#if defined BCM63158
	{ "CPU_TX_INGRESS_UPDATE_FIFO_TABLE", 1, CORE_2_INDEX, &CPU_TX_INGRESS_UPDATE_FIFO_TABLE_2, 8, 1, 1 },
#endif
#if defined BCM63158
	{ "IPV6_HOST_ADDRESS_CRC_TABLE", 1, CORE_2_INDEX, &IPV6_HOST_ADDRESS_CRC_TABLE_2, 8, 1, 1 },
#endif
#if defined BCM63158
	{ "LAYER2_GRE_TUNNEL_TABLE", 1, CORE_2_INDEX, &LAYER2_GRE_TUNNEL_TABLE_2, 12, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_TX_EGRESS_PD_FIFO_TABLE", 1, CORE_2_INDEX, &CPU_TX_EGRESS_PD_FIFO_TABLE_2, 2, 1, 1 },
#endif
#if defined BCM63158
	{ "TCAM_IC_HANDLER_TABLE", 1, CORE_2_INDEX, &TCAM_IC_HANDLER_TABLE_2, 16, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_TX_INGRESS_PD_FIFO_TABLE", 1, CORE_2_INDEX, &CPU_TX_INGRESS_PD_FIFO_TABLE_2, 2, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_TX_EGRESS_DISPATCHER_CREDIT_TABLE", 1, CORE_2_INDEX, &CPU_TX_EGRESS_DISPATCHER_CREDIT_TABLE_2, 3, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_TX_RECYCLE_RING_ID", 1, CORE_2_INDEX, &CPU_TX_RECYCLE_RING_ID_2, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "IC_DEBUG_COUNTER_MODE", 1, CORE_2_INDEX, &IC_DEBUG_COUNTER_MODE_2, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_REDIRECT_MODE", 1, CORE_2_INDEX, &CPU_REDIRECT_MODE_2, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "IPTV_CLASSIFICATION_CFG_TABLE", 1, CORE_2_INDEX, &IPTV_CLASSIFICATION_CFG_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_TX_INGRESS_DISPATCHER_CREDIT_TABLE", 1, CORE_2_INDEX, &CPU_TX_INGRESS_DISPATCHER_CREDIT_TABLE_2, 3, 1, 1 },
#endif
#if defined BCM63158
	{ "INGRESS_FILTER_1588_CFG", 1, CORE_2_INDEX, &INGRESS_FILTER_1588_CFG_2, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "VPORT_CFG_TABLE", 1, CORE_2_INDEX, &VPORT_CFG_TABLE_2, 40, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_RECYCLE_RING_DESCRIPTOR_TABLE", 1, CORE_2_INDEX, &CPU_RECYCLE_RING_DESCRIPTOR_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_RECYCLE_RING_INDEX_DDR_ADDR_TABLE", 1, CORE_2_INDEX, &CPU_RECYCLE_RING_INDEX_DDR_ADDR_TABLE_2, 2, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR", 1, CORE_2_INDEX, &CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR_2, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_RX_INTERRUPT_ID_DDR_ADDR", 1, CORE_2_INDEX, &CPU_RX_INTERRUPT_ID_DDR_ADDR_2, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_FEED_RING_DESCRIPTOR_TABLE", 1, CORE_2_INDEX, &CPU_FEED_RING_DESCRIPTOR_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_RECYCLE_INTERRUPT_SCRATCH", 1, CORE_2_INDEX, &CPU_RECYCLE_INTERRUPT_SCRATCH_2, 2, 1, 1 },
#endif
#if defined BCM63158
	{ "SYSTEM_CONFIGURATION", 1, CORE_2_INDEX, &SYSTEM_CONFIGURATION_2, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "FPM_GLOBAL_CFG", 1, CORE_2_INDEX, &FPM_GLOBAL_CFG_2, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_RECYCLE_RING_INTERRUPT_COUNTER_TABLE", 1, CORE_2_INDEX, &CPU_RECYCLE_RING_INTERRUPT_COUNTER_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "TUNNELS_PARSING_CFG", 1, CORE_2_INDEX, &TUNNELS_PARSING_CFG_2, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "IPTV_CFG_TABLE", 1, CORE_2_INDEX, &IPTV_CFG_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "IPTV_DDR_CTX_TABLE_ADDRESS", 1, CORE_2_INDEX, &IPTV_DDR_CTX_TABLE_ADDRESS_2, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "NAT_CACHE_CFG", 1, CORE_2_INDEX, &NAT_CACHE_CFG_2, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "INGRESS_FILTER_CFG", 1, CORE_2_INDEX, &INGRESS_FILTER_CFG_2, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "TX_FLOW_TABLE", 1, CORE_2_INDEX, &TX_FLOW_TABLE_2, 320, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_RECYCLE_SHADOW_RD_IDX", 1, CORE_2_INDEX, &CPU_RECYCLE_SHADOW_RD_IDX_2, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_RECYCLE_SRAM_PD_FIFO", 1, CORE_2_INDEX, &CPU_RECYCLE_SRAM_PD_FIFO_2, 16, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_RECYCLE_SHADOW_WR_IDX", 1, CORE_2_INDEX, &CPU_RECYCLE_SHADOW_WR_IDX_2, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "FC_FLOW_IP_ADDRESSES_TABLE", 1, CORE_2_INDEX, &FC_FLOW_IP_ADDRESSES_TABLE_2, 4, 1, 1 },
#endif
#if defined BCM63158
	{ "DEBUG_TRACE", 1, CORE_2_INDEX, &DEBUG_TRACE_2, 512, 1, 1 },
#endif
#if defined BCM63158
	{ "RX_FLOW_TABLE", 1, CORE_3_INDEX, &RX_FLOW_TABLE_3, 330, 1, 1 },
#endif
#if defined BCM63158
	{ "DS_WAN_UDP_FILTER_CONTROL_TABLE", 1, CORE_3_INDEX, &DS_WAN_UDP_FILTER_CONTROL_TABLE_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "IPTV_CONFIGURATION_TABLE", 1, CORE_3_INDEX, &IPTV_CONFIGURATION_TABLE_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "RUNNER_GLOBAL_REGISTERS_INIT", 1, CORE_3_INDEX, &RUNNER_GLOBAL_REGISTERS_INIT_3, 8, 1, 1 },
#endif
#if defined BCM63158
	{ "GLOBAL_DSCP_TO_PBITS_TABLE", 1, CORE_3_INDEX, &GLOBAL_DSCP_TO_PBITS_TABLE_3, 1, 64, 1 },
#endif
#if defined BCM63158
	{ "DSCP_TO_PBITS_MAP_TABLE", 1, CORE_3_INDEX, &DSCP_TO_PBITS_MAP_TABLE_3, 4, 64, 1 },
#endif
#if defined BCM63158
	{ "CPU_RX_SCRATCHPAD", 1, CORE_3_INDEX, &CPU_RX_SCRATCHPAD_3, 64, 1, 1 },
#endif
#if defined BCM63158
	{ "TCAM_IC_CMD_TABLE", 1, CORE_3_INDEX, &TCAM_IC_CMD_TABLE_3, 7, 16, 1 },
#endif
#if defined BCM63158
	{ "FPM_POOL_NUMBER_MAPPING_TABLE", 1, CORE_3_INDEX, &FPM_POOL_NUMBER_MAPPING_TABLE_3, 64, 1, 1 },
#endif
#if defined BCM63158
	{ "TC_TO_QUEUE_TABLE", 1, CORE_3_INDEX, &TC_TO_QUEUE_TABLE_3, 65, 1, 1 },
#endif
#if defined BCM63158
	{ "GENERAL_QUEUE_DYNAMIC_MNG_TABLE", 1, CORE_3_INDEX, &GENERAL_QUEUE_DYNAMIC_MNG_TABLE_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "DS_WAN_UDP_FILTER_TABLE", 1, CORE_3_INDEX, &DS_WAN_UDP_FILTER_TABLE_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "VPORT_TO_DSCP_TO_PBITS_TABLE", 1, CORE_3_INDEX, &VPORT_TO_DSCP_TO_PBITS_TABLE_3, 32, 1, 1 },
#endif
#if defined BCM63158
	{ "PBIT_TO_GEM_TABLE", 1, CORE_3_INDEX, &PBIT_TO_GEM_TABLE_3, 8, 8, 1 },
#endif
#if defined BCM63158
	{ "DS_CPU_RX_METER_TABLE", 1, CORE_3_INDEX, &DS_CPU_RX_METER_TABLE_3, 16, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_RING_INTERRUPT_COUNTER_TABLE", 1, CORE_3_INDEX, &CPU_RING_INTERRUPT_COUNTER_TABLE_3, 18, 1, 1 },
#endif
#if defined BCM63158
	{ "BROADCOM_SWITCH_PORT_TO_RX_FLOW_MAPPING_TABLE", 1, CORE_3_INDEX, &BROADCOM_SWITCH_PORT_TO_RX_FLOW_MAPPING_TABLE_3, 8, 1, 1 },
#endif
#if defined BCM63158
	{ "IPV4_HOST_ADDRESS_TABLE", 1, CORE_3_INDEX, &IPV4_HOST_ADDRESS_TABLE_3, 8, 1, 1 },
#endif
#if defined BCM63158
	{ "DUAL_STACK_LITE_TABLE", 1, CORE_3_INDEX, &DUAL_STACK_LITE_TABLE_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "PBIT_TO_QUEUE_TABLE", 1, CORE_3_INDEX, &PBIT_TO_QUEUE_TABLE_3, 65, 1, 1 },
#endif
#if defined BCM63158
	{ "PROCESSING_QUEUE_DYNAMIC_MNG_TABLE", 1, CORE_3_INDEX, &PROCESSING_QUEUE_DYNAMIC_MNG_TABLE_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "VPORT_TO_LOOKUP_PORT_MAPPING_TABLE", 1, CORE_3_INDEX, &VPORT_TO_LOOKUP_PORT_MAPPING_TABLE_3, 8, 1, 1 },
#endif
#if defined BCM63158
	{ "IPV6_HOST_ADDRESS_CRC_TABLE", 1, CORE_3_INDEX, &IPV6_HOST_ADDRESS_CRC_TABLE_3, 8, 1, 1 },
#endif
#if defined BCM63158
	{ "GPE_COMMAND_PRIMITIVE_TABLE", 1, CORE_3_INDEX, &GPE_COMMAND_PRIMITIVE_TABLE_3, 32, 1, 1 },
#endif
#if defined BCM63158
	{ "US_CPU_RX_METER_TABLE", 1, CORE_3_INDEX, &US_CPU_RX_METER_TABLE_3, 16, 1, 1 },
#endif
#if defined BCM63158
	{ "INGRESS_FILTER_PROFILE_TABLE", 1, CORE_3_INDEX, &INGRESS_FILTER_PROFILE_TABLE_3, 16, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_REASON_AND_VPORT_TO_METER_TABLE", 1, CORE_3_INDEX, &CPU_REASON_AND_VPORT_TO_METER_TABLE_3, 120, 1, 1 },
#endif
#if defined BCM63158
	{ "RX_FLOW_CONTEXT_DDR_ADDR", 1, CORE_3_INDEX, &RX_FLOW_CONTEXT_DDR_ADDR_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "DS_PACKET_BUFFER", 1, CORE_3_INDEX, &DS_PACKET_BUFFER_3, 8, 1, 1 },
#endif
#if defined BCM63158
	{ "RUNNER_PROFILING_TRACE_BUFFER", 1, CORE_3_INDEX, &RUNNER_PROFILING_TRACE_BUFFER_3, 128, 1, 1 },
#endif
#if defined BCM63158
	{ "LAYER2_GRE_TUNNEL_TABLE", 1, CORE_3_INDEX, &LAYER2_GRE_TUNNEL_TABLE_3, 12, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_RX_COPY_PD_FIFO_TABLE", 1, CORE_3_INDEX, &CPU_RX_COPY_PD_FIFO_TABLE_3, 4, 1, 1 },
#endif
#if defined BCM63158
	{ "DS_CPU_REASON_TO_METER_TABLE", 1, CORE_3_INDEX, &DS_CPU_REASON_TO_METER_TABLE_3, 64, 1, 1 },
#endif
#if defined BCM63158
	{ "VPORT_CFG_TABLE", 1, CORE_3_INDEX, &VPORT_CFG_TABLE_3, 40, 1, 1 },
#endif
#if defined BCM63158
	{ "US_CPU_REASON_TO_METER_TABLE", 1, CORE_3_INDEX, &US_CPU_REASON_TO_METER_TABLE_3, 64, 1, 1 },
#endif
#if defined BCM63158
	{ "REGISTERS_BUFFER", 1, CORE_3_INDEX, &REGISTERS_BUFFER_3, 32, 1, 1 },
#endif
#if defined BCM63158
	{ "TX_FLOW_TABLE", 1, CORE_3_INDEX, &TX_FLOW_TABLE_3, 320, 1, 1 },
#endif
#if defined BCM63158
	{ "TCAM_IC_CFG_TABLE", 1, CORE_3_INDEX, &TCAM_IC_CFG_TABLE_3, 6, 1, 1 },
#endif
#if defined BCM63158
	{ "TCAM_IC_HANDLER_TABLE", 1, CORE_3_INDEX, &TCAM_IC_HANDLER_TABLE_3, 16, 1, 1 },
#endif
#if defined BCM63158
	{ "LOOPBACK_QUEUE_TABLE", 1, CORE_3_INDEX, &LOOPBACK_QUEUE_TABLE_3, 40, 1, 1 },
#endif
#if defined BCM63158
	{ "NULL_BUFFER", 1, CORE_3_INDEX, &NULL_BUFFER_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_RECYCLE_INTERRUPT_COALESCING_TABLE", 1, CORE_3_INDEX, &CPU_RECYCLE_INTERRUPT_COALESCING_TABLE_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_RECYCLE_SRAM_PD_FIFO", 1, CORE_3_INDEX, &CPU_RECYCLE_SRAM_PD_FIFO_3, 16, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_VPORT_TO_METER_TABLE", 1, CORE_3_INDEX, &CPU_VPORT_TO_METER_TABLE_3, 40, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR", 1, CORE_3_INDEX, &CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_RX_PSRAM_GET_NEXT_SCRATCHPAD", 1, CORE_3_INDEX, &CPU_RX_PSRAM_GET_NEXT_SCRATCHPAD_3, 4, 1, 1 },
#endif
#if defined BCM63158
	{ "QUEUE_THRESHOLD_VECTOR", 1, CORE_3_INDEX, &QUEUE_THRESHOLD_VECTOR_3, 9, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_RX_INTERRUPT_SCRATCH", 1, CORE_3_INDEX, &CPU_RX_INTERRUPT_SCRATCH_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_RX_INTERRUPT_ID_DDR_ADDR", 1, CORE_3_INDEX, &CPU_RX_INTERRUPT_ID_DDR_ADDR_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "INGRESS_FILTER_L2_REASON_TABLE", 1, CORE_3_INDEX, &INGRESS_FILTER_L2_REASON_TABLE_3, 16, 1, 1 },
#endif
#if defined BCM63158
	{ "FLOW_BASED_ACTION_PTR_TABLE", 1, CORE_3_INDEX, &FLOW_BASED_ACTION_PTR_TABLE_3, 17, 1, 1 },
#endif
#if defined BCM63158
	{ "FORCE_DSCP", 1, CORE_3_INDEX, &FORCE_DSCP_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "CORE_ID_TABLE", 1, CORE_3_INDEX, &CORE_ID_TABLE_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "TASK_IDX", 1, CORE_3_INDEX, &TASK_IDX_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "SYSTEM_CONFIGURATION", 1, CORE_3_INDEX, &SYSTEM_CONFIGURATION_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "SRAM_DUMMY_STORE", 1, CORE_3_INDEX, &SRAM_DUMMY_STORE_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_FEED_RING_INTERRUPT_THRESHOLD", 1, CORE_3_INDEX, &CPU_FEED_RING_INTERRUPT_THRESHOLD_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_RX_COPY_DISPATCHER_CREDIT_TABLE", 1, CORE_3_INDEX, &CPU_RX_COPY_DISPATCHER_CREDIT_TABLE_3, 3, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_FEED_RING_INTERRUPT_COUNTER", 1, CORE_3_INDEX, &CPU_FEED_RING_INTERRUPT_COUNTER_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "RX_MIRRORING_CONFIGURATION", 1, CORE_3_INDEX, &RX_MIRRORING_CONFIGURATION_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "UPDATE_FIFO_TABLE", 1, CORE_3_INDEX, &UPDATE_FIFO_TABLE_3, 8, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_RECYCLE_RING_DESCRIPTOR_TABLE", 1, CORE_3_INDEX, &CPU_RECYCLE_RING_DESCRIPTOR_TABLE_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_RECYCLE_RING_INDEX_DDR_ADDR_TABLE", 1, CORE_3_INDEX, &CPU_RECYCLE_RING_INDEX_DDR_ADDR_TABLE_3, 2, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_RX_COPY_UPDATE_FIFO_TABLE", 1, CORE_3_INDEX, &CPU_RX_COPY_UPDATE_FIFO_TABLE_3, 8, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_INTERRUPT_COALESCING_TABLE", 1, CORE_3_INDEX, &CPU_INTERRUPT_COALESCING_TABLE_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_FEED_RING_DESCRIPTOR_TABLE", 1, CORE_3_INDEX, &CPU_FEED_RING_DESCRIPTOR_TABLE_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "PD_FIFO_TABLE", 1, CORE_3_INDEX, &PD_FIFO_TABLE_3, 2, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_REASON_TO_TC", 1, CORE_3_INDEX, &CPU_REASON_TO_TC_3, 64, 1, 1 },
#endif
#if defined BCM63158
	{ "TC_TO_CPU_RXQ", 1, CORE_3_INDEX, &TC_TO_CPU_RXQ_3, 64, 1, 1 },
#endif
#if defined BCM63158
	{ "EXC_TC_TO_CPU_RXQ", 1, CORE_3_INDEX, &EXC_TC_TO_CPU_RXQ_3, 64, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_RX_LOCAL_SCRATCH", 1, CORE_3_INDEX, &CPU_RX_LOCAL_SCRATCH_3, 2, 1, 1 },
#endif
#if defined BCM63158
	{ "FPM_GLOBAL_CFG", 1, CORE_3_INDEX, &FPM_GLOBAL_CFG_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "RATE_LIMIT_OVERHEAD", 1, CORE_3_INDEX, &RATE_LIMIT_OVERHEAD_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_TX_RECYCLE_RING_ID", 1, CORE_3_INDEX, &CPU_TX_RECYCLE_RING_ID_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "IC_DEBUG_COUNTER_MODE", 1, CORE_3_INDEX, &IC_DEBUG_COUNTER_MODE_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_REDIRECT_MODE", 1, CORE_3_INDEX, &CPU_REDIRECT_MODE_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "VPORT_TO_CPU_OBJ", 1, CORE_3_INDEX, &VPORT_TO_CPU_OBJ_3, 40, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_RECYCLE_RING_INTERRUPT_COUNTER_TABLE", 1, CORE_3_INDEX, &CPU_RECYCLE_RING_INTERRUPT_COUNTER_TABLE_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_RX_COPY_LOCAL_SCRATCH", 1, CORE_3_INDEX, &CPU_RX_COPY_LOCAL_SCRATCH_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "TUNNELS_PARSING_CFG", 1, CORE_3_INDEX, &TUNNELS_PARSING_CFG_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_RECYCLE_INTERRUPT_SCRATCH", 1, CORE_3_INDEX, &CPU_RECYCLE_INTERRUPT_SCRATCH_3, 2, 1, 1 },
#endif
#if defined BCM63158
	{ "IPTV_CFG_TABLE", 1, CORE_3_INDEX, &IPTV_CFG_TABLE_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "IPTV_DDR_CTX_TABLE_ADDRESS", 1, CORE_3_INDEX, &IPTV_DDR_CTX_TABLE_ADDRESS_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "NAT_CACHE_CFG", 1, CORE_3_INDEX, &NAT_CACHE_CFG_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "IPTV_CLASSIFICATION_CFG_TABLE", 1, CORE_3_INDEX, &IPTV_CLASSIFICATION_CFG_TABLE_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "INGRESS_FILTER_CFG", 1, CORE_3_INDEX, &INGRESS_FILTER_CFG_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "INGRESS_FILTER_1588_CFG", 1, CORE_3_INDEX, &INGRESS_FILTER_1588_CFG_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_RECYCLE_SHADOW_RD_IDX", 1, CORE_3_INDEX, &CPU_RECYCLE_SHADOW_RD_IDX_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_RECYCLE_SHADOW_WR_IDX", 1, CORE_3_INDEX, &CPU_RECYCLE_SHADOW_WR_IDX_3, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_RING_DESCRIPTORS_TABLE", 1, CORE_3_INDEX, &CPU_RING_DESCRIPTORS_TABLE_3, 16, 1, 1 },
#endif
#if defined BCM63158
	{ "FC_FLOW_IP_ADDRESSES_TABLE", 1, CORE_3_INDEX, &FC_FLOW_IP_ADDRESSES_TABLE_3, 4, 1, 1 },
#endif
#if defined BCM63158
	{ "DEBUG_TRACE", 1, CORE_3_INDEX, &DEBUG_TRACE_3, 512, 1, 1 },
#endif
#if defined BCM63158
	{ "DS_PACKET_BUFFER", 1, CORE_4_INDEX, &DS_PACKET_BUFFER_4, 8, 1, 1 },
#endif
#if defined BCM63158
	{ "RX_FLOW_TABLE", 1, CORE_4_INDEX, &RX_FLOW_TABLE_4, 330, 1, 1 },
#endif
#if defined BCM63158
	{ "DS_WAN_UDP_FILTER_CONTROL_TABLE", 1, CORE_4_INDEX, &DS_WAN_UDP_FILTER_CONTROL_TABLE_4, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "IPTV_CONFIGURATION_TABLE", 1, CORE_4_INDEX, &IPTV_CONFIGURATION_TABLE_4, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "RUNNER_GLOBAL_REGISTERS_INIT", 1, CORE_4_INDEX, &RUNNER_GLOBAL_REGISTERS_INIT_4, 8, 1, 1 },
#endif
#if defined BCM63158
	{ "GLOBAL_DSCP_TO_PBITS_TABLE", 1, CORE_4_INDEX, &GLOBAL_DSCP_TO_PBITS_TABLE_4, 1, 64, 1 },
#endif
#if defined BCM63158
	{ "DSCP_TO_PBITS_MAP_TABLE", 1, CORE_4_INDEX, &DSCP_TO_PBITS_MAP_TABLE_4, 4, 64, 1 },
#endif
#if defined BCM63158
	{ "TC_TO_QUEUE_TABLE", 1, CORE_4_INDEX, &TC_TO_QUEUE_TABLE_4, 65, 1, 1 },
#endif
#if defined BCM63158
	{ "GENERAL_QUEUE_DYNAMIC_MNG_TABLE", 1, CORE_4_INDEX, &GENERAL_QUEUE_DYNAMIC_MNG_TABLE_4, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "DS_WAN_UDP_FILTER_TABLE", 1, CORE_4_INDEX, &DS_WAN_UDP_FILTER_TABLE_4, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "VPORT_TO_DSCP_TO_PBITS_TABLE", 1, CORE_4_INDEX, &VPORT_TO_DSCP_TO_PBITS_TABLE_4, 32, 1, 1 },
#endif
#if defined BCM63158
	{ "FPM_POOL_NUMBER_MAPPING_TABLE", 1, CORE_4_INDEX, &FPM_POOL_NUMBER_MAPPING_TABLE_4, 64, 1, 1 },
#endif
#if defined BCM63158
	{ "PBIT_TO_GEM_TABLE", 1, CORE_4_INDEX, &PBIT_TO_GEM_TABLE_4, 8, 8, 1 },
#endif
#if defined BCM63158
	{ "DUAL_STACK_LITE_TABLE", 1, CORE_4_INDEX, &DUAL_STACK_LITE_TABLE_4, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "INGRESS_FILTER_PROFILE_TABLE", 1, CORE_4_INDEX, &INGRESS_FILTER_PROFILE_TABLE_4, 16, 1, 1 },
#endif
#if defined BCM63158
	{ "GPE_COMMAND_PRIMITIVE_TABLE", 1, CORE_4_INDEX, &GPE_COMMAND_PRIMITIVE_TABLE_4, 32, 1, 1 },
#endif
#if defined BCM63158
	{ "LOOPBACK_QUEUE_TABLE", 1, CORE_4_INDEX, &LOOPBACK_QUEUE_TABLE_4, 40, 1, 1 },
#endif
#if defined BCM63158
	{ "PROCESSING_QUEUE_DYNAMIC_MNG_TABLE", 1, CORE_4_INDEX, &PROCESSING_QUEUE_DYNAMIC_MNG_TABLE_4, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "BROADCOM_SWITCH_PORT_TO_RX_FLOW_MAPPING_TABLE", 1, CORE_4_INDEX, &BROADCOM_SWITCH_PORT_TO_RX_FLOW_MAPPING_TABLE_4, 8, 1, 1 },
#endif
#if defined BCM63158
	{ "PBIT_TO_QUEUE_TABLE", 1, CORE_4_INDEX, &PBIT_TO_QUEUE_TABLE_4, 65, 1, 1 },
#endif
#if defined BCM63158
	{ "REGISTERS_BUFFER", 1, CORE_4_INDEX, &REGISTERS_BUFFER_4, 32, 1, 1 },
#endif
#if defined BCM63158
	{ "RX_FLOW_CONTEXT_DDR_ADDR", 1, CORE_4_INDEX, &RX_FLOW_CONTEXT_DDR_ADDR_4, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "VPORT_TO_LOOKUP_PORT_MAPPING_TABLE", 1, CORE_4_INDEX, &VPORT_TO_LOOKUP_PORT_MAPPING_TABLE_4, 8, 1, 1 },
#endif
#if defined BCM63158
	{ "TCAM_IC_CFG_TABLE", 1, CORE_4_INDEX, &TCAM_IC_CFG_TABLE_4, 6, 1, 1 },
#endif
#if defined BCM63158
	{ "QUEUE_THRESHOLD_VECTOR", 1, CORE_4_INDEX, &QUEUE_THRESHOLD_VECTOR_4, 9, 1, 1 },
#endif
#if defined BCM63158
	{ "TASK_IDX", 1, CORE_4_INDEX, &TASK_IDX_4, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "NULL_BUFFER", 1, CORE_4_INDEX, &NULL_BUFFER_4, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "INGRESS_FILTER_L2_REASON_TABLE", 1, CORE_4_INDEX, &INGRESS_FILTER_L2_REASON_TABLE_4, 16, 1, 1 },
#endif
#if defined BCM63158
	{ "FLOW_BASED_ACTION_PTR_TABLE", 1, CORE_4_INDEX, &FLOW_BASED_ACTION_PTR_TABLE_4, 17, 1, 1 },
#endif
#if defined BCM63158
	{ "FORCE_DSCP", 1, CORE_4_INDEX, &FORCE_DSCP_4, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "CORE_ID_TABLE", 1, CORE_4_INDEX, &CORE_ID_TABLE_4, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "RX_MIRRORING_CONFIGURATION", 1, CORE_4_INDEX, &RX_MIRRORING_CONFIGURATION_4, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "SRAM_DUMMY_STORE", 1, CORE_4_INDEX, &SRAM_DUMMY_STORE_4, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "RATE_LIMIT_OVERHEAD", 1, CORE_4_INDEX, &RATE_LIMIT_OVERHEAD_4, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "SYSTEM_CONFIGURATION", 1, CORE_4_INDEX, &SYSTEM_CONFIGURATION_4, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "IC_DEBUG_COUNTER_MODE", 1, CORE_4_INDEX, &IC_DEBUG_COUNTER_MODE_4, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_REDIRECT_MODE", 1, CORE_4_INDEX, &CPU_REDIRECT_MODE_4, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "IPTV_CLASSIFICATION_CFG_TABLE", 1, CORE_4_INDEX, &IPTV_CLASSIFICATION_CFG_TABLE_4, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_FEED_RING_DESCRIPTOR_TABLE", 1, CORE_4_INDEX, &CPU_FEED_RING_DESCRIPTOR_TABLE_4, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "IPV4_HOST_ADDRESS_TABLE", 1, CORE_4_INDEX, &IPV4_HOST_ADDRESS_TABLE_4, 8, 1, 1 },
#endif
#if defined BCM63158
	{ "IPV6_HOST_ADDRESS_CRC_TABLE", 1, CORE_4_INDEX, &IPV6_HOST_ADDRESS_CRC_TABLE_4, 8, 1, 1 },
#endif
#if defined BCM63158
	{ "TCAM_IC_HANDLER_TABLE", 1, CORE_4_INDEX, &TCAM_IC_HANDLER_TABLE_4, 16, 1, 1 },
#endif
#if defined BCM63158
	{ "FPM_GLOBAL_CFG", 1, CORE_4_INDEX, &FPM_GLOBAL_CFG_4, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "INGRESS_FILTER_1588_CFG", 1, CORE_4_INDEX, &INGRESS_FILTER_1588_CFG_4, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "TUNNELS_PARSING_CFG", 1, CORE_4_INDEX, &TUNNELS_PARSING_CFG_4, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "IPTV_CFG_TABLE", 1, CORE_4_INDEX, &IPTV_CFG_TABLE_4, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "RUNNER_PROFILING_TRACE_BUFFER", 1, CORE_4_INDEX, &RUNNER_PROFILING_TRACE_BUFFER_4, 128, 1, 1 },
#endif
#if defined BCM63158
	{ "TCAM_IC_CMD_TABLE", 1, CORE_4_INDEX, &TCAM_IC_CMD_TABLE_4, 7, 16, 1 },
#endif
#if defined BCM63158
	{ "IPTV_DDR_CTX_TABLE_ADDRESS", 1, CORE_4_INDEX, &IPTV_DDR_CTX_TABLE_ADDRESS_4, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "NAT_CACHE_CFG", 1, CORE_4_INDEX, &NAT_CACHE_CFG_4, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "INGRESS_FILTER_CFG", 1, CORE_4_INDEX, &INGRESS_FILTER_CFG_4, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "LAYER2_GRE_TUNNEL_TABLE", 1, CORE_4_INDEX, &LAYER2_GRE_TUNNEL_TABLE_4, 12, 1, 1 },
#endif
#if defined BCM63158
	{ "VPORT_CFG_TABLE", 1, CORE_4_INDEX, &VPORT_CFG_TABLE_4, 40, 1, 1 },
#endif
#if defined BCM63158
	{ "TX_FLOW_TABLE", 1, CORE_4_INDEX, &TX_FLOW_TABLE_4, 320, 1, 1 },
#endif
#if defined BCM63158
	{ "FC_FLOW_IP_ADDRESSES_TABLE", 1, CORE_4_INDEX, &FC_FLOW_IP_ADDRESSES_TABLE_4, 4, 1, 1 },
#endif
#if defined BCM63158
	{ "DEBUG_TRACE", 1, CORE_4_INDEX, &DEBUG_TRACE_4, 512, 1, 1 },
#endif
#if defined BCM63158
	{ "DS_PACKET_BUFFER", 1, CORE_5_INDEX, &DS_PACKET_BUFFER_5, 8, 1, 1 },
#endif
#if defined BCM63158
	{ "RX_FLOW_TABLE", 1, CORE_5_INDEX, &RX_FLOW_TABLE_5, 330, 1, 1 },
#endif
#if defined BCM63158
	{ "DS_WAN_UDP_FILTER_CONTROL_TABLE", 1, CORE_5_INDEX, &DS_WAN_UDP_FILTER_CONTROL_TABLE_5, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "IPTV_CONFIGURATION_TABLE", 1, CORE_5_INDEX, &IPTV_CONFIGURATION_TABLE_5, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "RUNNER_GLOBAL_REGISTERS_INIT", 1, CORE_5_INDEX, &RUNNER_GLOBAL_REGISTERS_INIT_5, 8, 1, 1 },
#endif
#if defined BCM63158
	{ "GLOBAL_DSCP_TO_PBITS_TABLE", 1, CORE_5_INDEX, &GLOBAL_DSCP_TO_PBITS_TABLE_5, 1, 64, 1 },
#endif
#if defined BCM63158
	{ "DSCP_TO_PBITS_MAP_TABLE", 1, CORE_5_INDEX, &DSCP_TO_PBITS_MAP_TABLE_5, 4, 64, 1 },
#endif
#if defined BCM63158
	{ "TC_TO_QUEUE_TABLE", 1, CORE_5_INDEX, &TC_TO_QUEUE_TABLE_5, 65, 1, 1 },
#endif
#if defined BCM63158
	{ "GENERAL_QUEUE_DYNAMIC_MNG_TABLE", 1, CORE_5_INDEX, &GENERAL_QUEUE_DYNAMIC_MNG_TABLE_5, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "DS_WAN_UDP_FILTER_TABLE", 1, CORE_5_INDEX, &DS_WAN_UDP_FILTER_TABLE_5, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "VPORT_TO_DSCP_TO_PBITS_TABLE", 1, CORE_5_INDEX, &VPORT_TO_DSCP_TO_PBITS_TABLE_5, 32, 1, 1 },
#endif
#if defined BCM63158
	{ "FPM_POOL_NUMBER_MAPPING_TABLE", 1, CORE_5_INDEX, &FPM_POOL_NUMBER_MAPPING_TABLE_5, 64, 1, 1 },
#endif
#if defined BCM63158
	{ "PBIT_TO_GEM_TABLE", 1, CORE_5_INDEX, &PBIT_TO_GEM_TABLE_5, 8, 8, 1 },
#endif
#if defined BCM63158
	{ "DUAL_STACK_LITE_TABLE", 1, CORE_5_INDEX, &DUAL_STACK_LITE_TABLE_5, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "INGRESS_FILTER_PROFILE_TABLE", 1, CORE_5_INDEX, &INGRESS_FILTER_PROFILE_TABLE_5, 16, 1, 1 },
#endif
#if defined BCM63158
	{ "GPE_COMMAND_PRIMITIVE_TABLE", 1, CORE_5_INDEX, &GPE_COMMAND_PRIMITIVE_TABLE_5, 32, 1, 1 },
#endif
#if defined BCM63158
	{ "LOOPBACK_QUEUE_TABLE", 1, CORE_5_INDEX, &LOOPBACK_QUEUE_TABLE_5, 40, 1, 1 },
#endif
#if defined BCM63158
	{ "PROCESSING_QUEUE_DYNAMIC_MNG_TABLE", 1, CORE_5_INDEX, &PROCESSING_QUEUE_DYNAMIC_MNG_TABLE_5, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "BROADCOM_SWITCH_PORT_TO_RX_FLOW_MAPPING_TABLE", 1, CORE_5_INDEX, &BROADCOM_SWITCH_PORT_TO_RX_FLOW_MAPPING_TABLE_5, 8, 1, 1 },
#endif
#if defined BCM63158
	{ "PBIT_TO_QUEUE_TABLE", 1, CORE_5_INDEX, &PBIT_TO_QUEUE_TABLE_5, 65, 1, 1 },
#endif
#if defined BCM63158
	{ "REGISTERS_BUFFER", 1, CORE_5_INDEX, &REGISTERS_BUFFER_5, 32, 1, 1 },
#endif
#if defined BCM63158
	{ "RX_FLOW_CONTEXT_DDR_ADDR", 1, CORE_5_INDEX, &RX_FLOW_CONTEXT_DDR_ADDR_5, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "VPORT_TO_LOOKUP_PORT_MAPPING_TABLE", 1, CORE_5_INDEX, &VPORT_TO_LOOKUP_PORT_MAPPING_TABLE_5, 8, 1, 1 },
#endif
#if defined BCM63158
	{ "TCAM_IC_CFG_TABLE", 1, CORE_5_INDEX, &TCAM_IC_CFG_TABLE_5, 6, 1, 1 },
#endif
#if defined BCM63158
	{ "QUEUE_THRESHOLD_VECTOR", 1, CORE_5_INDEX, &QUEUE_THRESHOLD_VECTOR_5, 9, 1, 1 },
#endif
#if defined BCM63158
	{ "TASK_IDX", 1, CORE_5_INDEX, &TASK_IDX_5, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "NULL_BUFFER", 1, CORE_5_INDEX, &NULL_BUFFER_5, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "INGRESS_FILTER_L2_REASON_TABLE", 1, CORE_5_INDEX, &INGRESS_FILTER_L2_REASON_TABLE_5, 16, 1, 1 },
#endif
#if defined BCM63158
	{ "FLOW_BASED_ACTION_PTR_TABLE", 1, CORE_5_INDEX, &FLOW_BASED_ACTION_PTR_TABLE_5, 17, 1, 1 },
#endif
#if defined BCM63158
	{ "FORCE_DSCP", 1, CORE_5_INDEX, &FORCE_DSCP_5, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "CORE_ID_TABLE", 1, CORE_5_INDEX, &CORE_ID_TABLE_5, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "RX_MIRRORING_CONFIGURATION", 1, CORE_5_INDEX, &RX_MIRRORING_CONFIGURATION_5, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "SRAM_DUMMY_STORE", 1, CORE_5_INDEX, &SRAM_DUMMY_STORE_5, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "RATE_LIMIT_OVERHEAD", 1, CORE_5_INDEX, &RATE_LIMIT_OVERHEAD_5, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "SYSTEM_CONFIGURATION", 1, CORE_5_INDEX, &SYSTEM_CONFIGURATION_5, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "IC_DEBUG_COUNTER_MODE", 1, CORE_5_INDEX, &IC_DEBUG_COUNTER_MODE_5, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_REDIRECT_MODE", 1, CORE_5_INDEX, &CPU_REDIRECT_MODE_5, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "IPTV_CLASSIFICATION_CFG_TABLE", 1, CORE_5_INDEX, &IPTV_CLASSIFICATION_CFG_TABLE_5, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "CPU_FEED_RING_DESCRIPTOR_TABLE", 1, CORE_5_INDEX, &CPU_FEED_RING_DESCRIPTOR_TABLE_5, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "IPV4_HOST_ADDRESS_TABLE", 1, CORE_5_INDEX, &IPV4_HOST_ADDRESS_TABLE_5, 8, 1, 1 },
#endif
#if defined BCM63158
	{ "IPV6_HOST_ADDRESS_CRC_TABLE", 1, CORE_5_INDEX, &IPV6_HOST_ADDRESS_CRC_TABLE_5, 8, 1, 1 },
#endif
#if defined BCM63158
	{ "TCAM_IC_HANDLER_TABLE", 1, CORE_5_INDEX, &TCAM_IC_HANDLER_TABLE_5, 16, 1, 1 },
#endif
#if defined BCM63158
	{ "FPM_GLOBAL_CFG", 1, CORE_5_INDEX, &FPM_GLOBAL_CFG_5, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "INGRESS_FILTER_1588_CFG", 1, CORE_5_INDEX, &INGRESS_FILTER_1588_CFG_5, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "TUNNELS_PARSING_CFG", 1, CORE_5_INDEX, &TUNNELS_PARSING_CFG_5, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "IPTV_CFG_TABLE", 1, CORE_5_INDEX, &IPTV_CFG_TABLE_5, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "RUNNER_PROFILING_TRACE_BUFFER", 1, CORE_5_INDEX, &RUNNER_PROFILING_TRACE_BUFFER_5, 128, 1, 1 },
#endif
#if defined BCM63158
	{ "TCAM_IC_CMD_TABLE", 1, CORE_5_INDEX, &TCAM_IC_CMD_TABLE_5, 7, 16, 1 },
#endif
#if defined BCM63158
	{ "IPTV_DDR_CTX_TABLE_ADDRESS", 1, CORE_5_INDEX, &IPTV_DDR_CTX_TABLE_ADDRESS_5, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "NAT_CACHE_CFG", 1, CORE_5_INDEX, &NAT_CACHE_CFG_5, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "INGRESS_FILTER_CFG", 1, CORE_5_INDEX, &INGRESS_FILTER_CFG_5, 1, 1, 1 },
#endif
#if defined BCM63158
	{ "LAYER2_GRE_TUNNEL_TABLE", 1, CORE_5_INDEX, &LAYER2_GRE_TUNNEL_TABLE_5, 12, 1, 1 },
#endif
#if defined BCM63158
	{ "VPORT_CFG_TABLE", 1, CORE_5_INDEX, &VPORT_CFG_TABLE_5, 40, 1, 1 },
#endif
#if defined BCM63158
	{ "TX_FLOW_TABLE", 1, CORE_5_INDEX, &TX_FLOW_TABLE_5, 320, 1, 1 },
#endif
#if defined BCM63158
	{ "FC_FLOW_IP_ADDRESSES_TABLE", 1, CORE_5_INDEX, &FC_FLOW_IP_ADDRESSES_TABLE_5, 4, 1, 1 },
#endif
#if defined BCM63158
	{ "DEBUG_TRACE", 1, CORE_5_INDEX, &DEBUG_TRACE_5, 512, 1, 1 },
#endif
};

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
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT REPORTING_QUEUE_DESCRIPTOR_TABLE =
{
	16,
	{
		{ dump_RDD_REPORTING_QUEUE_DESCRIPTOR, 0x0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT BASIC_SCHEDULER_TABLE_US =
{
	16,
	{
		{ dump_RDD_BASIC_SCHEDULER_DESCRIPTOR, 0xa00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT BASIC_SCHEDULER_TABLE_DS =
{
	16,
	{
		{ dump_RDD_BASIC_SCHEDULER_DESCRIPTOR, 0xb00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT US_TM_SCHEDULING_QUEUE_TABLE =
{
	8,
	{
		{ dump_RDD_SCHEDULING_QUEUE_DESCRIPTOR, 0xc00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RUNNER_GLOBAL_REGISTERS_INIT =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0xea0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT US_TM_BBH_TX_EGRESS_COUNTER_TABLE =
{
	8,
	{
		{ dump_RDD_BBH_TX_EGRESS_COUNTER_ENTRY, 0xec0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT COMPLEX_SCHEDULER_TABLE =
{
	64,
	{
		{ dump_RDD_COMPLEX_SCHEDULER_DESCRIPTOR, 0xf00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT US_TM_PD_FIFO_TABLE =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x1000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT BASIC_RATE_LIMITER_TABLE_US =
{
	16,
	{
		{ dump_RDD_BASIC_RATE_LIMITER_DESCRIPTOR, 0x1800 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_TM_SCHEDULING_QUEUE_TABLE =
{
	8,
	{
		{ dump_RDD_SCHEDULING_QUEUE_DESCRIPTOR, 0x1c00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT US_TM_SCHEDULING_FLUSH_VECTOR =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x1ea0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT GHOST_REPORTING_GLOBAL_CFG =
{
	4,
	{
		{ dump_RDD_GHOST_REPORTING_GLOBAL_CFG_ENTRY, 0x1eb4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT US_TM_BBH_TX_QUEUE_ID_TABLE =
{
	4,
	{
		{ dump_RDD_BBH_TX_QUEUE_ID_ENTRY, 0x1eb8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_TM_BBH_TX_EGRESS_COUNTER_TABLE =
{
	8,
	{
		{ dump_RDD_BBH_TX_EGRESS_COUNTER_ENTRY, 0x1ec0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT REPORTING_COUNTER_TABLE =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x1f00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_TM_SCHEDULING_FLUSH_VECTOR =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x1fa0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT BUDGET_ALLOCATION_TIMER_VALUE =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x1fb4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT US_TM_BB_DESTINATION_TABLE =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x1fb6 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_TM_BBH_TX_QUEUE_ID_TABLE =
{
	4,
	{
		{ dump_RDD_BBH_TX_QUEUE_ID_ENTRY, 0x1fb8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT BBH_TX_EPON_INGRESS_COUNTER_TABLE =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x1fc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT BBH_TX_EPON_QUEUE_ID_TABLE =
{
	4,
	{
		{ dump_RDD_BBH_TX_QUEUE_ID_ENTRY, 0x1fe8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT XGPON_REPORT_TABLE =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x1ff0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_TM_PD_FIFO_TABLE =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x2000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT US_TM_TM_FLOW_CNTR_TABLE =
{
	1,
	{
		{ dump_RDD_TM_FLOW_CNTR_ENTRY, 0x2500 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT REPORTING_QUEUE_ACCUMULATED_TABLE =
{
	8,
	{
		{ dump_RDD_REPORTING_ACCUMULATED_DATA, 0x2580 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT US_TM_BBH_QUEUE_TABLE =
{
	4,
	{
		{ dump_RDD_BBH_QUEUE_DESCRIPTOR, 0x2600 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT US_TM_CPU_TX_ABS_COUNTERS =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x26a0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_TM_CPU_TX_ABS_COUNTERS =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2720 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT US_TM_SCHEDULING_QUEUE_AGING_VECTOR =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x27a0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_TM_BB_DESTINATION_TABLE =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x27b4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT US_TM_SCHEDULING_GLOBAL_FLUSH_CFG =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x27b6 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_TM_SCHEDULING_GLOBAL_FLUSH_CFG =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x27b7 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT GENERAL_QUEUE_DYNAMIC_MNG_TABLE =
{
	8,
	{
		{ dump_RDD_QUEUE_DYNAMIC_MNG_ENTRY, 0x27b8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT BBH_TX_EPON_EGRESS_COUNTER_TABLE =
{
	8,
	{
		{ dump_RDD_BBH_TX_EGRESS_COUNTER_ENTRY, 0x27c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT MIRRORING_SCRATCH =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0x27e8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT OVERALL_RATE_LIMITER_TABLE =
{
	16,
	{
		{ dump_RDD_OVERALL_RATE_LIMITER_DESCRIPTOR, 0x27f0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT BASIC_RATE_LIMITER_TABLE_DS =
{
	16,
	{
		{ dump_RDD_BASIC_RATE_LIMITER_DESCRIPTOR, 0x2800 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RUNNER_PROFILING_TRACE_BUFFER =
{
	4,
	{
		{ dump_RDD_TRACE_EVENT, 0x2c00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT REGISTERS_BUFFER =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2e00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT US_TM_TM_ACTION_PTR_TABLE =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2e80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT SCHEDULING_FLUSH_GLOBAL_CFG =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2ea2 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT SRAM_DUMMY_STORE =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2ea3 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TASK_IDX =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2ea4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT REPORT_BBH_TX_QUEUE_ID_TABLE =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2ea8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT FLUSH_DISPATCHER_CREDIT_TABLE =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2eb0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TX_MIRRORING_CONFIGURATION =
{
	2,
	{
		{ dump_RDD_MIRRORING_DESCRIPTOR, 0x2ebc },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RATE_LIMIT_OVERHEAD =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2ebe },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT BBH_TX_FIFO_SIZE =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2ebf },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_TM_TM_ACTION_PTR_TABLE =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2ec0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT US_TM_FIRST_QUEUE_MAPPING =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2ee2 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_TM_FIRST_QUEUE_MAPPING =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2ee3 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT BBH_TX_INGRESS_COUNTER_TABLE =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2ee4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_TM_TM_FLOW_CNTR_TABLE =
{
	1,
	{
		{ dump_RDD_TM_FLOW_CNTR_ENTRY, 0x2ee8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DISPATCHER_CREDIT_TABLE =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2ef0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT NATC_TBL_CFG =
{
	24,
	{
		{ dump_RDD_NATC_TBL_CONFIGURATION, 0x2f00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TX_MIRRORING_DISPATCHER_CREDIT_TABLE =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2f30 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT US_TM_UPDATE_FIFO_TABLE =
{
	4,
	{
		{ dump_RDD_UPDATE_FIFO_ENTRY, 0x2f40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_TM_SCHEDULING_QUEUE_AGING_VECTOR =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2f60 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT BBH_TX_EGRESS_REPORT_COUNTER_TABLE =
{
	8,
	{
		{ dump_RDD_BBH_TX_EGRESS_COUNTER_ENTRY, 0x2f78 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_TM_UPDATE_FIFO_TABLE =
{
	4,
	{
		{ dump_RDD_UPDATE_FIFO_ENTRY, 0x2f80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT US_TM_SCHEDULING_AGGREGATION_CONTEXT_VECTOR =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2fa0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT EPON_UPDATE_FIFO_TABLE =
{
	4,
	{
		{ dump_RDD_UPDATE_FIFO_ENTRY, 0x2fc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_TM_SCHEDULING_AGGREGATION_CONTEXT_VECTOR =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2fe0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_TM_BBH_QUEUE_TABLE =
{
	4,
	{
		{ dump_RDD_BBH_QUEUE_DESCRIPTOR, 0x3000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT GHOST_REPORTING_QUEUE_STATUS_BIT_VECTOR_TABLE =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x3020 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RATE_LIMITER_VALID_TABLE_US =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x3040 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT FPM_GLOBAL_CFG =
{
	12,
	{
		{ dump_RDD_FPM_GLOBAL_CFG, 0x3050 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RATE_LIMITER_VALID_TABLE_DS =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x3060 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT XGPON_REPORT_ZERO_SENT_TABLE =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x3070 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TX_FLOW_TABLE_1 =
{
	1,
	{
		{ dump_RDD_TX_FLOW_ENTRY, 0x0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DIRECT_PROCESSING_PD_TABLE_1 =
{
	16,
	{
		{ dump_RDD_PROCESSING_RX_DESCRIPTOR, 0x140 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RUNNER_GLOBAL_REGISTERS_INIT_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x160 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_CPU_RX_METER_TABLE_1 =
{
	8,
	{
		{ dump_RDD_CPU_RX_METER_ENTRY, 0x180 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TCAM_IC_CMD_TABLE_1 =
{
	4,
	{
		{ dump_RDD_TCAM_IC_CMD, 0x200 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT GLOBAL_DSCP_TO_PBITS_TABLE_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x3c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RX_SCRATCHPAD_1 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0x400 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT LAYER2_GRE_TUNNEL_TABLE_1 =
{
	32,
	{
		{ dump_RDD_LAYER2_GRE_TUNNEL_ENTRY, 0x600 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT US_CPU_RX_METER_TABLE_1 =
{
	8,
	{
		{ dump_RDD_CPU_RX_METER_ENTRY, 0x780 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RX_FLOW_TABLE_1 =
{
	2,
	{
		{ dump_RDD_RX_FLOW_ENTRY, 0x800 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT LAYER2_HEADER_COPY_MAPPING_TABLE_1 =
{
	4,
	{
		{ dump_RDD_LAYER2_HEADER_COPY_MAPPING_ENTRY, 0xa80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT IPTV_CONFIGURATION_TABLE_1 =
{
	8,
	{
		{ dump_RDD_HW_IPTV_CONFIGURATION, 0xaf8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DSCP_TO_PBITS_MAP_TABLE_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xb00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TC_TO_QUEUE_TABLE_1 =
{
	8,
	{
		{ dump_RDD_TC_TO_QUEUE_8, 0xc00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT GENERAL_QUEUE_DYNAMIC_MNG_TABLE_1 =
{
	8,
	{
		{ dump_RDD_QUEUE_DYNAMIC_MNG_ENTRY, 0xe08 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT FPM_POOL_NUMBER_MAPPING_TABLE_1 =
{
	1,
	{
		{ dump_RDD_FPM_POOL_NUMBER, 0xe10 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT VPORT_TO_DSCP_TO_PBITS_TABLE_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xe20 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT PBIT_TO_GEM_TABLE_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xe40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_REASON_AND_VPORT_TO_METER_TABLE_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xe80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT PROCESSING_QUEUE_DYNAMIC_MNG_TABLE_1 =
{
	8,
	{
		{ dump_RDD_QUEUE_DYNAMIC_MNG_ENTRY, 0xef8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RING_INTERRUPT_COUNTER_TABLE_1 =
{
	8,
	{
		{ dump_RDD_CPU_RING_INTERRUPT_COUNTER_ENTRY, 0xf00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_INTERRUPT_COALESCING_TABLE_1 =
{
	16,
	{
		{ dump_RDD_CPU_INTERRUPT_COALESCING_ENTRY, 0xf90 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TCAM_IC_HANDLER_TABLE_1 =
{
	2,
	{
		{ dump_RDD_TCAM_IC_HANDLER, 0xfa0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DUAL_STACK_LITE_TABLE_1 =
{
	64,
	{
		{ dump_RDD_DUAL_STACK_LITE_ENTRY, 0xfc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_PACKET_BUFFER_1 =
{
	512,
	{
		{ dump_RDD_PACKET_BUFFER, 0x1000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT PBIT_TO_QUEUE_TABLE_1 =
{
	8,
	{
		{ dump_RDD_PBIT_TO_QUEUE_8, 0x2000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RX_FLOW_CONTEXT_DDR_ADDR_1 =
{
	8,
	{
		{ dump_RDD_DEF_FLOW_CONTEXT_DDR_ADDR, 0x2208 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RX_PSRAM_GET_NEXT_SCRATCHPAD_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2210 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT BRIDGE_CFG_TABLE_1 =
{
	8,
	{
		{ dump_RDD_BRIDGE_CFG, 0x2220 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT NULL_BUFFER_1 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0x2238 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_CPU_REASON_TO_METER_TABLE_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2240 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT POLICER_PARAMS_TABLE_1 =
{
	1,
	{
		{ dump_RDD_POLICER_PARAMS_ENTRY, 0x2280 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT SCRATCH_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x22d0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_L2_REASON_TABLE_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x22f0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_PROFILE_TABLE_1 =
{
	8,
	{
		{ dump_RDD_INGRESS_FILTER_CTRL, 0x2300 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RX_COPY_PD_FIFO_TABLE_1 =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x2380 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT US_CPU_REASON_TO_METER_TABLE_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x23c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RUNNER_PROFILING_TRACE_BUFFER_1 =
{
	4,
	{
		{ dump_RDD_TRACE_EVENT, 0x2400 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT VPORT_CFG_TABLE_1 =
{
	8,
	{
		{ dump_RDD_VPORT_CFG_ENTRY, 0x2600 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT REGISTERS_BUFFER_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2740 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT LOOPBACK_QUEUE_TABLE_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x27c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR_1 =
{
	8,
	{
		{ dump_RDD_DDR_ADDRESS, 0x27e8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_RX_COMPLETE_0_DISPATCHER_CREDIT_TABLE_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x27f0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RX_INTERRUPT_SCRATCH_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x27fc },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_SRAM_PD_FIFO_1 =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x2800 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT NATC_TBL_CFG_1 =
{
	24,
	{
		{ dump_RDD_NATC_TBL_CONFIGURATION, 0x2900 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_RX_COMPLETE_1_DISPATCHER_CREDIT_TABLE_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2930 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TASK_IDX_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x293c },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TCAM_IC_CFG_TABLE_1 =
{
	16,
	{
		{ dump_RDD_TCAM_IC_CFG, 0x2940 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_RX_COMPLETE_2_DISPATCHER_CREDIT_TABLE_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x29a0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_FEED_RING_INTERRUPT_COUNTER_MAX_1 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x29ac },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT FORCE_IC_IP_FLOW_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x29ae },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT FORCE_DSCP_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x29af },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RX_COPY_DISPATCHER_CREDIT_TABLE_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x29b0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CORE_ID_TABLE_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x29bc },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT SRAM_DUMMY_STORE_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x29bd },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_FEED_RING_INTERRUPT_THRESHOLD_1 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x29be },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT FLOW_BASED_ACTION_PTR_TABLE_1 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x29c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_FEED_RING_INTERRUPT_COUNTER_1 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x29e2 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_FPM_LOW_THRESHOLD_1 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x29e4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_FPM_HIGH_THRESHOLD_1 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x29e6 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RX_INTERRUPT_ID_DDR_ADDR_1 =
{
	8,
	{
		{ dump_RDD_DDR_ADDRESS, 0x29e8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_RING_DESCRIPTOR_TABLE_1 =
{
	16,
	{
		{ dump_RDD_CPU_RING_DESCRIPTOR, 0x29f0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_COMPLETE_COMMON_RADIO_DATA_1 =
{
	112,
	{
		{ dump_RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY, 0x2a00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_RING_INDEX_DDR_ADDR_TABLE_1 =
{
	8,
	{
		{ dump_RDD_DDR_ADDRESS, 0x2a70 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT VPORT_CFG_EX_TABLE_1 =
{
	1,
	{
		{ dump_RDD_VPORT_CFG_EX_ENTRY, 0x2a80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR_1 =
{
	8,
	{
		{ dump_RDD_DDR_ADDRESS, 0x2aa8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_INTERRUPT_COALESCING_TABLE_1 =
{
	16,
	{
		{ dump_RDD_CPU_INTERRUPT_COALESCING_ENTRY, 0x2ab0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RULE_BASED_ACTION_PTR_TABLE_1 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2ac0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_RX_POST_RING_SIZE_1 =
{
	2,
	{
		{ dump_RDD_DHD_RING_SIZE, 0x2ae2 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_RX_COMPLETE_RING_SIZE_1 =
{
	2,
	{
		{ dump_RDD_DHD_RING_SIZE, 0x2ae4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_TX_COMPLETE_RING_SIZE_1 =
{
	2,
	{
		{ dump_RDD_DHD_RING_SIZE, 0x2ae6 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT SYSTEM_CONFIGURATION_1 =
{
	5,
	{
		{ dump_RDD_SYSTEM_CONFIGURATION_ENTRY, 0x2ae8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RATE_LIMIT_OVERHEAD_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2aed },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RX_MIRRORING_CONFIGURATION_1 =
{
	2,
	{
		{ dump_RDD_MIRRORING_DESCRIPTOR, 0x2aee },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_FEED_RING_DESCRIPTOR_TABLE_1 =
{
	16,
	{
		{ dump_RDD_CPU_RING_DESCRIPTOR, 0x2af0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_VPORT_TO_METER_TABLE_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2b00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RX_LOCAL_SCRATCH_1 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0x2b28 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_RING_INTERRUPT_COUNTER_TABLE_1 =
{
	8,
	{
		{ dump_RDD_CPU_RING_INTERRUPT_COUNTER_ENTRY, 0x2b38 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT IPTV_ACTION_PTR_TABLE_1 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2b40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT INGRESS_QOS_DONT_DROP_RATIO_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2b62 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT INGRESS_QOS_WAN_UNTAGGED_PRIORITY_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2b63 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT MAC_TYPE_1 =
{
	1,
	{
		{ dump_RDD_MAC_TYPE_ENTRY, 0x2b64 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT IC_DEBUG_COUNTER_MODE_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2b65 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT IC_MCAST_ENABLE_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2b66 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_REDIRECT_MODE_1 =
{
	1,
	{
		{ dump_RDD_CPU_REDIRECT_MODE_ENTRY, 0x2b67 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RX_COPY_LOCAL_SCRATCH_1 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0x2b68 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT FPM_GLOBAL_CFG_1 =
{
	12,
	{
		{ dump_RDD_FPM_GLOBAL_CFG, 0x2b70 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT IPTV_CLASSIFICATION_CFG_TABLE_1 =
{
	1,
	{
		{ dump_RDD_IPTV_CLASSIFICATION_CFG_ENTRY, 0x2b7c },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_1588_CFG_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2b7d },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT VLAN_ACTION_GPE_HANDLER_PTR_TABLE_1 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2b80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TUNNELS_PARSING_CFG_1 =
{
	8,
	{
		{ dump_RDD_TUNNELS_PARSING_CFG, 0x2ba8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_DOORBELL_VALUE_1 =
{
	4,
	{
		{ dump_RDD_DHD_DOORBELL, 0x2bb0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT IPTV_CFG_TABLE_1 =
{
	8,
	{
		{ dump_RDD_IPTV_CFG, 0x2bb8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT UPDATE_FIFO_TABLE_1 =
{
	4,
	{
		{ dump_RDD_UPDATE_FIFO_ENTRY, 0x2bc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT IPTV_DDR_CTX_TABLE_ADDRESS_1 =
{
	8,
	{
		{ dump_RDD_IPTV_DDR_CTX_TABLE_ADDRESS, 0x2be0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT NAT_CACHE_CFG_1 =
{
	7,
	{
		{ dump_RDD_NAT_CACHE_CFG, 0x2be8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DIRECT_PROCESSING_EPON_CONTROL_SCRATCH_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2bf0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_CFG_1 =
{
	2,
	{
		{ dump_RDD_INGRESS_FILTER_CFG, 0x2bf8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RX_COPY_UPDATE_FIFO_TABLE_1 =
{
	4,
	{
		{ dump_RDD_UPDATE_FIFO_ENTRY, 0x2c00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT PD_FIFO_TABLE_1 =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x2c40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_REASON_TO_TC_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2c60 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TC_TO_CPU_RXQ_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2ca0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT EXC_TC_TO_CPU_RXQ_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2ce0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT VPORT_TO_CPU_OBJ_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2d40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_RX_COMPLETE_FLOW_RING_BUFFER_1 =
{
	32,
	{
		{ dump_RDD_DHD_RX_COMPLETE_DESCRIPTOR, 0x2d80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_TX_COMPLETE_FLOW_RING_BUFFER_1 =
{
	16,
	{
		{ dump_RDD_DHD_TX_COMPLETE_DESCRIPTOR, 0x2dc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_RX_POST_FLOW_RING_BUFFER_1 =
{
	32,
	{
		{ dump_RDD_DHD_RX_POST_DESCRIPTOR, 0x2e00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_FPM_REPLY_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2e40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_CPU_RECYCLE_RING_DESCRIPTOR_TABLE_1 =
{
	16,
	{
		{ dump_RDD_CPU_RING_DESCRIPTOR, 0x2e80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_HW_CFG_1 =
{
	12,
	{
		{ dump_RDD_DHD_HW_CONFIGURATION, 0x2f00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_INTERRUPT_SCRATCH_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2f80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RING_DESCRIPTORS_TABLE_1 =
{
	16,
	{
		{ dump_RDD_CPU_RING_DESCRIPTOR, 0x3000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_SHADOW_RD_IDX_1 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x3100 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_SHADOW_WR_IDX_1 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x3180 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_TX_SCRATCHPAD_2 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0x0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RX_FLOW_TABLE_2 =
{
	2,
	{
		{ dump_RDD_RX_FLOW_ENTRY, 0x400 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT LAYER2_HEADER_COPY_MAPPING_TABLE_2 =
{
	4,
	{
		{ dump_RDD_LAYER2_HEADER_COPY_MAPPING_ENTRY, 0x680 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT IPTV_CONFIGURATION_TABLE_2 =
{
	8,
	{
		{ dump_RDD_HW_IPTV_CONFIGURATION, 0x6f8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DSCP_TO_PBITS_MAP_TABLE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x700 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TC_TO_QUEUE_TABLE_2 =
{
	8,
	{
		{ dump_RDD_TC_TO_QUEUE_8, 0x800 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT GENERAL_QUEUE_DYNAMIC_MNG_TABLE_2 =
{
	8,
	{
		{ dump_RDD_QUEUE_DYNAMIC_MNG_ENTRY, 0xa08 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT FPM_POOL_NUMBER_MAPPING_TABLE_2 =
{
	1,
	{
		{ dump_RDD_FPM_POOL_NUMBER, 0xa10 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RUNNER_GLOBAL_REGISTERS_INIT_2 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0xa20 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT GLOBAL_DSCP_TO_PBITS_TABLE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xa40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT POLICER_PARAMS_TABLE_2 =
{
	1,
	{
		{ dump_RDD_POLICER_PARAMS_ENTRY, 0xa80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_TX_FPM_POOL_NUMBER_MAPPING_TABLE_2 =
{
	1,
	{
		{ dump_RDD_FPM_POOL_NUMBER, 0xad0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT VPORT_TO_DSCP_TO_PBITS_TABLE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xae0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_PROFILE_TABLE_2 =
{
	8,
	{
		{ dump_RDD_INGRESS_FILTER_CTRL, 0xb00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT PBIT_TO_GEM_TABLE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xb80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DUAL_STACK_LITE_TABLE_2 =
{
	64,
	{
		{ dump_RDD_DUAL_STACK_LITE_ENTRY, 0xbc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT PBIT_TO_QUEUE_TABLE_2 =
{
	8,
	{
		{ dump_RDD_PBIT_TO_QUEUE_8, 0xc00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT REGISTERS_BUFFER_2 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0xe08 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT PROCESSING_QUEUE_DYNAMIC_MNG_TABLE_2 =
{
	8,
	{
		{ dump_RDD_QUEUE_DYNAMIC_MNG_ENTRY, 0xe88 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_FPM_POOL_NUMBER_MAPPING_TABLE_2 =
{
	1,
	{
		{ dump_RDD_FPM_POOL_NUMBER, 0xe90 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TCAM_IC_HANDLER_TABLE_2 =
{
	2,
	{
		{ dump_RDD_TCAM_IC_HANDLER, 0xea0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_DFT_LIST_ENTRY_SCRATCH_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xec0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_POST_COMMON_RADIO_DATA_2 =
{
	72,
	{
		{ dump_RDD_DHD_POST_COMMON_RADIO_ENTRY, 0xf00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RX_FLOW_CONTEXT_DDR_ADDR_2 =
{
	8,
	{
		{ dump_RDD_DEF_FLOW_CONTEXT_DDR_ADDR, 0xf48 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT FPM_REPLY_2 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0xf50 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT BRIDGE_CFG_TABLE_2 =
{
	8,
	{
		{ dump_RDD_BRIDGE_CFG, 0xf60 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT NULL_BUFFER_2 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0xf78 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_DFT_LIST_SIZE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xf80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT LOOPBACK_QUEUE_TABLE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xfc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR_2 =
{
	8,
	{
		{ dump_RDD_DDR_ADDRESS, 0xfe8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_INTERRUPT_COALESCING_TABLE_2 =
{
	16,
	{
		{ dump_RDD_CPU_INTERRUPT_COALESCING_ENTRY, 0xff0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_PACKET_BUFFER_2 =
{
	512,
	{
		{ dump_RDD_PACKET_BUFFER, 0x1000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RUNNER_PROFILING_TRACE_BUFFER_2 =
{
	4,
	{
		{ dump_RDD_TRACE_EVENT, 0x2000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TCAM_IC_CMD_TABLE_2 =
{
	4,
	{
		{ dump_RDD_TCAM_IC_CMD, 0x2200 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT QUEUE_THRESHOLD_VECTOR_2 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x23c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TASK_IDX_2 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x23e4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RX_INTERRUPT_ID_DDR_ADDR_2 =
{
	8,
	{
		{ dump_RDD_DDR_ADDRESS, 0x23e8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_L2_REASON_TABLE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x23f0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_DHD_STATION_TABLE_2 =
{
	8,
	{
		{ dump_RDD_WLAN_MCAST_DHD_STATION_ENTRY, 0x2400 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT LAYER2_GRE_TUNNEL_TABLE_2 =
{
	32,
	{
		{ dump_RDD_LAYER2_GRE_TUNNEL_ENTRY, 0x2600 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TCAM_IC_CFG_TABLE_2 =
{
	16,
	{
		{ dump_RDD_TCAM_IC_CFG, 0x2780 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_TX_EGRESS_DISPATCHER_CREDIT_TABLE_2 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x27e0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT FORCE_IC_IP_FLOW_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x27ec },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT FORCE_DSCP_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x27ed },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CORE_ID_TABLE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x27ee },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT SRAM_DUMMY_STORE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x27ef },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_TX_INGRESS_DISPATCHER_CREDIT_TABLE_2 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x27f0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_FPM_LOW_THRESHOLD_2 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x27fc },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_FPM_HIGH_THRESHOLD_2 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x27fe },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TX_FLOW_TABLE_2 =
{
	1,
	{
		{ dump_RDD_TX_FLOW_ENTRY, 0x2800 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT FLOW_BASED_ACTION_PTR_TABLE_2 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2940 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RX_MIRRORING_CONFIGURATION_2 =
{
	2,
	{
		{ dump_RDD_MIRRORING_DESCRIPTOR, 0x2962 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RATE_LIMIT_OVERHEAD_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2964 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT INGRESS_QOS_DONT_DROP_RATIO_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2965 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT INGRESS_QOS_WAN_UNTAGGED_PRIORITY_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2966 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT IC_DEBUG_COUNTER_MODE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2967 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR_2 =
{
	8,
	{
		{ dump_RDD_DDR_ADDRESS, 0x2968 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_MCAST_DISPATCHER_CREDIT_TABLE_2 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2970 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT IC_MCAST_ENABLE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x297c },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_REDIRECT_MODE_2 =
{
	1,
	{
		{ dump_RDD_CPU_REDIRECT_MODE_ENTRY, 0x297d },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT IPTV_CLASSIFICATION_CFG_TABLE_2 =
{
	1,
	{
		{ dump_RDD_IPTV_CLASSIFICATION_CFG_ENTRY, 0x297e },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_1588_CFG_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x297f },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT VPORT_CFG_EX_TABLE_2 =
{
	1,
	{
		{ dump_RDD_VPORT_CFG_EX_ENTRY, 0x2980 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_DFT_ADDR_2 =
{
	8,
	{
		{ dump_RDD_PHYS_ADDR_64_PTR, 0x29a8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_RING_DESCRIPTOR_TABLE_2 =
{
	16,
	{
		{ dump_RDD_CPU_RING_DESCRIPTOR, 0x29b0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RULE_BASED_ACTION_PTR_TABLE_2 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x29c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_RING_INDEX_DDR_ADDR_TABLE_2 =
{
	8,
	{
		{ dump_RDD_DDR_ADDRESS, 0x29e8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT SYSTEM_CONFIGURATION_2 =
{
	5,
	{
		{ dump_RDD_SYSTEM_CONFIGURATION_ENTRY, 0x29f8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT VPORT_CFG_TABLE_2 =
{
	8,
	{
		{ dump_RDD_VPORT_CFG_ENTRY, 0x2a00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT IPTV_ACTION_PTR_TABLE_2 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2b40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_RING_INTERRUPT_COUNTER_TABLE_2 =
{
	8,
	{
		{ dump_RDD_CPU_RING_INTERRUPT_COUNTER_ENTRY, 0x2b68 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_FEED_RING_DESCRIPTOR_TABLE_2 =
{
	16,
	{
		{ dump_RDD_CPU_RING_DESCRIPTOR, 0x2b70 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT VLAN_ACTION_GPE_HANDLER_PTR_TABLE_2 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2b80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TUNNELS_PARSING_CFG_2 =
{
	8,
	{
		{ dump_RDD_TUNNELS_PARSING_CFG, 0x2ba8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT FPM_GLOBAL_CFG_2 =
{
	12,
	{
		{ dump_RDD_FPM_GLOBAL_CFG, 0x2bb0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_TX_EGRESS_UPDATE_FIFO_TABLE_2 =
{
	4,
	{
		{ dump_RDD_UPDATE_FIFO_ENTRY, 0x2bc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_SCRATCHPAD_2 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2be0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT IPTV_CFG_TABLE_2 =
{
	8,
	{
		{ dump_RDD_IPTV_CFG, 0x2be8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_DOORBELL_VALUE_2 =
{
	4,
	{
		{ dump_RDD_DHD_DOORBELL, 0x2bf0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT IPTV_DDR_CTX_TABLE_ADDRESS_2 =
{
	8,
	{
		{ dump_RDD_IPTV_DDR_CTX_TABLE_ADDRESS, 0x2bf8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_SRAM_PD_FIFO_2 =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x2c00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT NATC_TBL_CFG_2 =
{
	24,
	{
		{ dump_RDD_NATC_TBL_CONFIGURATION, 0x2d00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT NAT_CACHE_CFG_2 =
{
	7,
	{
		{ dump_RDD_NAT_CACHE_CFG, 0x2d30 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_CFG_2 =
{
	2,
	{
		{ dump_RDD_INGRESS_FILTER_CFG, 0x2d38 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_TX_INGRESS_UPDATE_FIFO_TABLE_2 =
{
	4,
	{
		{ dump_RDD_UPDATE_FIFO_ENTRY, 0x2d40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_TX_POST_UPDATE_FIFO_TABLE_2 =
{
	4,
	{
		{ dump_RDD_UPDATE_FIFO_ENTRY, 0x2d80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_MCAST_UPDATE_FIFO_TABLE_2 =
{
	4,
	{
		{ dump_RDD_UPDATE_FIFO_ENTRY, 0x2dc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_TX_POST_PD_FIFO_TABLE_2 =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x2e00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_TX_EGRESS_PD_FIFO_TABLE_2 =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x2ec0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_TX_POST_FLOW_RING_BUFFER_2 =
{
	48,
	{
		{ dump_RDD_DHD_TX_POST_DESCRIPTOR, 0x2f00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_TX_INGRESS_PD_FIFO_TABLE_2 =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x2f40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_MCAST_PD_FIFO_TABLE_2 =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x2f80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_FLOW_RING_CACHE_CTX_TABLE_2 =
{
	16,
	{
		{ dump_RDD_DHD_FLOW_RING_CACHE_CTX_ENTRY, 0x3000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_FLOW_RING_CACHE_LKP_TABLE_2 =
{
	2,
	{
		{ dump_RDD_DHD_FLOW_RING_CACHE_LKP_ENTRY, 0x3100 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_L2_HEADER_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x3180 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_SSID_MAC_ADDRESS_TABLE_2 =
{
	8,
	{
		{ dump_RDD_WLAN_MCAST_SSID_MAC_ADDRESS_ENTRY, 0x3200 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_CPU_RECYCLE_RING_DESCRIPTOR_TABLE_2 =
{
	16,
	{
		{ dump_RDD_CPU_RING_DESCRIPTOR, 0x3280 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_TX_POST_FLOW_RING_MGMT_DESCRIPTOR_BUFFER_2 =
{
	16,
	{
		{ dump_RDD_DHD_TX_POST_FLOW_RING_MGMT_DESCRIPTOR, 0x3300 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_HW_CFG_2 =
{
	12,
	{
		{ dump_RDD_DHD_HW_CONFIGURATION, 0x3380 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_DHD_STATION_CTX_TABLE_2 =
{
	1,
	{
		{ dump_RDD_WLAN_MCAST_DHD_STATION_CTX_ENTRY, 0x3400 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_INTERRUPT_SCRATCH_2 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x3480 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_SHADOW_RD_IDX_2 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x3500 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_SHADOW_WR_IDX_2 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x3580 },
		{ 0, 0 },
	}
};
#endif

TABLE_STRUCT RUNNER_TABLES[NUMBER_OF_TABLES] =
{
#if defined BCM6846
	{ "REPORTING_QUEUE_DESCRIPTOR_TABLE", 1, CORE_0_INDEX, &REPORTING_QUEUE_DESCRIPTOR_TABLE, 160, 1, 1 },
#endif
#if defined BCM6846
	{ "BASIC_SCHEDULER_TABLE_US", 1, CORE_0_INDEX, &BASIC_SCHEDULER_TABLE_US, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "BASIC_SCHEDULER_TABLE_DS", 1, CORE_0_INDEX, &BASIC_SCHEDULER_TABLE_DS, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "US_TM_SCHEDULING_QUEUE_TABLE", 1, CORE_0_INDEX, &US_TM_SCHEDULING_QUEUE_TABLE, 84, 1, 1 },
#endif
#if defined BCM6846
	{ "RUNNER_GLOBAL_REGISTERS_INIT", 1, CORE_0_INDEX, &RUNNER_GLOBAL_REGISTERS_INIT, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "US_TM_BBH_TX_EGRESS_COUNTER_TABLE", 1, CORE_0_INDEX, &US_TM_BBH_TX_EGRESS_COUNTER_TABLE, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "COMPLEX_SCHEDULER_TABLE", 1, CORE_0_INDEX, &COMPLEX_SCHEDULER_TABLE, 4, 1, 1 },
#endif
#if defined BCM6846
	{ "US_TM_PD_FIFO_TABLE", 1, CORE_0_INDEX, &US_TM_PD_FIFO_TABLE, 128, 1, 1 },
#endif
#if defined BCM6846
	{ "BASIC_RATE_LIMITER_TABLE_US", 1, CORE_0_INDEX, &BASIC_RATE_LIMITER_TABLE_US, 64, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_TM_SCHEDULING_QUEUE_TABLE", 1, CORE_0_INDEX, &DS_TM_SCHEDULING_QUEUE_TABLE, 84, 1, 1 },
#endif
#if defined BCM6846
	{ "US_TM_SCHEDULING_FLUSH_VECTOR", 1, CORE_0_INDEX, &US_TM_SCHEDULING_FLUSH_VECTOR, 5, 1, 1 },
#endif
#if defined BCM6846
	{ "GHOST_REPORTING_GLOBAL_CFG", 1, CORE_0_INDEX, &GHOST_REPORTING_GLOBAL_CFG, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "US_TM_BBH_TX_QUEUE_ID_TABLE", 1, CORE_0_INDEX, &US_TM_BBH_TX_QUEUE_ID_TABLE, 2, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_TM_BBH_TX_EGRESS_COUNTER_TABLE", 1, CORE_0_INDEX, &DS_TM_BBH_TX_EGRESS_COUNTER_TABLE, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "REPORTING_COUNTER_TABLE", 1, CORE_0_INDEX, &REPORTING_COUNTER_TABLE, 40, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_TM_SCHEDULING_FLUSH_VECTOR", 1, CORE_0_INDEX, &DS_TM_SCHEDULING_FLUSH_VECTOR, 5, 1, 1 },
#endif
#if defined BCM6846
	{ "BUDGET_ALLOCATION_TIMER_VALUE", 1, CORE_0_INDEX, &BUDGET_ALLOCATION_TIMER_VALUE, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "US_TM_BB_DESTINATION_TABLE", 1, CORE_0_INDEX, &US_TM_BB_DESTINATION_TABLE, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_TM_BBH_TX_QUEUE_ID_TABLE", 1, CORE_0_INDEX, &DS_TM_BBH_TX_QUEUE_ID_TABLE, 2, 1, 1 },
#endif
#if defined BCM6846
	{ "BBH_TX_EPON_INGRESS_COUNTER_TABLE", 1, CORE_0_INDEX, &BBH_TX_EPON_INGRESS_COUNTER_TABLE, 40, 1, 1 },
#endif
#if defined BCM6846
	{ "BBH_TX_EPON_QUEUE_ID_TABLE", 1, CORE_0_INDEX, &BBH_TX_EPON_QUEUE_ID_TABLE, 2, 1, 1 },
#endif
#if defined BCM6846
	{ "XGPON_REPORT_TABLE", 1, CORE_0_INDEX, &XGPON_REPORT_TABLE, 4, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_TM_PD_FIFO_TABLE", 1, CORE_0_INDEX, &DS_TM_PD_FIFO_TABLE, 80, 1, 1 },
#endif
#if defined BCM6846
	{ "US_TM_TM_FLOW_CNTR_TABLE", 1, CORE_0_INDEX, &US_TM_TM_FLOW_CNTR_TABLE, 128, 1, 1 },
#endif
#if defined BCM6846
	{ "REPORTING_QUEUE_ACCUMULATED_TABLE", 1, CORE_0_INDEX, &REPORTING_QUEUE_ACCUMULATED_TABLE, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "US_TM_BBH_QUEUE_TABLE", 1, CORE_0_INDEX, &US_TM_BBH_QUEUE_TABLE, 40, 1, 1 },
#endif
#if defined BCM6846
	{ "US_TM_CPU_TX_ABS_COUNTERS", 1, CORE_0_INDEX, &US_TM_CPU_TX_ABS_COUNTERS, 32, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_TM_CPU_TX_ABS_COUNTERS", 1, CORE_0_INDEX, &DS_TM_CPU_TX_ABS_COUNTERS, 32, 1, 1 },
#endif
#if defined BCM6846
	{ "US_TM_SCHEDULING_QUEUE_AGING_VECTOR", 1, CORE_0_INDEX, &US_TM_SCHEDULING_QUEUE_AGING_VECTOR, 5, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_TM_BB_DESTINATION_TABLE", 1, CORE_0_INDEX, &DS_TM_BB_DESTINATION_TABLE, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "US_TM_SCHEDULING_GLOBAL_FLUSH_CFG", 1, CORE_0_INDEX, &US_TM_SCHEDULING_GLOBAL_FLUSH_CFG, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_TM_SCHEDULING_GLOBAL_FLUSH_CFG", 1, CORE_0_INDEX, &DS_TM_SCHEDULING_GLOBAL_FLUSH_CFG, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "GENERAL_QUEUE_DYNAMIC_MNG_TABLE", 1, CORE_0_INDEX, &GENERAL_QUEUE_DYNAMIC_MNG_TABLE, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "BBH_TX_EPON_EGRESS_COUNTER_TABLE", 1, CORE_0_INDEX, &BBH_TX_EPON_EGRESS_COUNTER_TABLE, 5, 1, 1 },
#endif
#if defined BCM6846
	{ "MIRRORING_SCRATCH", 1, CORE_0_INDEX, &MIRRORING_SCRATCH, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "OVERALL_RATE_LIMITER_TABLE", 1, CORE_0_INDEX, &OVERALL_RATE_LIMITER_TABLE, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "BASIC_RATE_LIMITER_TABLE_DS", 1, CORE_0_INDEX, &BASIC_RATE_LIMITER_TABLE_DS, 64, 1, 1 },
#endif
#if defined BCM6846
	{ "RUNNER_PROFILING_TRACE_BUFFER", 1, CORE_0_INDEX, &RUNNER_PROFILING_TRACE_BUFFER, 128, 1, 1 },
#endif
#if defined BCM6846
	{ "REGISTERS_BUFFER", 1, CORE_0_INDEX, &REGISTERS_BUFFER, 32, 1, 1 },
#endif
#if defined BCM6846
	{ "US_TM_TM_ACTION_PTR_TABLE", 1, CORE_0_INDEX, &US_TM_TM_ACTION_PTR_TABLE, 17, 1, 1 },
#endif
#if defined BCM6846
	{ "SCHEDULING_FLUSH_GLOBAL_CFG", 1, CORE_0_INDEX, &SCHEDULING_FLUSH_GLOBAL_CFG, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "SRAM_DUMMY_STORE", 1, CORE_0_INDEX, &SRAM_DUMMY_STORE, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "TASK_IDX", 1, CORE_0_INDEX, &TASK_IDX, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "REPORT_BBH_TX_QUEUE_ID_TABLE", 1, CORE_0_INDEX, &REPORT_BBH_TX_QUEUE_ID_TABLE, 2, 1, 1 },
#endif
#if defined BCM6846
	{ "FLUSH_DISPATCHER_CREDIT_TABLE", 1, CORE_0_INDEX, &FLUSH_DISPATCHER_CREDIT_TABLE, 3, 1, 1 },
#endif
#if defined BCM6846
	{ "TX_MIRRORING_CONFIGURATION", 1, CORE_0_INDEX, &TX_MIRRORING_CONFIGURATION, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "RATE_LIMIT_OVERHEAD", 1, CORE_0_INDEX, &RATE_LIMIT_OVERHEAD, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "BBH_TX_FIFO_SIZE", 1, CORE_0_INDEX, &BBH_TX_FIFO_SIZE, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_TM_TM_ACTION_PTR_TABLE", 1, CORE_0_INDEX, &DS_TM_TM_ACTION_PTR_TABLE, 17, 1, 1 },
#endif
#if defined BCM6846
	{ "US_TM_FIRST_QUEUE_MAPPING", 1, CORE_0_INDEX, &US_TM_FIRST_QUEUE_MAPPING, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_TM_FIRST_QUEUE_MAPPING", 1, CORE_0_INDEX, &DS_TM_FIRST_QUEUE_MAPPING, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "BBH_TX_INGRESS_COUNTER_TABLE", 1, CORE_0_INDEX, &BBH_TX_INGRESS_COUNTER_TABLE, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_TM_TM_FLOW_CNTR_TABLE", 1, CORE_0_INDEX, &DS_TM_TM_FLOW_CNTR_TABLE, 5, 1, 1 },
#endif
#if defined BCM6846
	{ "DISPATCHER_CREDIT_TABLE", 1, CORE_0_INDEX, &DISPATCHER_CREDIT_TABLE, 3, 1, 1 },
#endif
#if defined BCM6846
	{ "NATC_TBL_CFG", 1, CORE_0_INDEX, &NATC_TBL_CFG, 2, 1, 1 },
#endif
#if defined BCM6846
	{ "TX_MIRRORING_DISPATCHER_CREDIT_TABLE", 1, CORE_0_INDEX, &TX_MIRRORING_DISPATCHER_CREDIT_TABLE, 3, 1, 1 },
#endif
#if defined BCM6846
	{ "US_TM_UPDATE_FIFO_TABLE", 1, CORE_0_INDEX, &US_TM_UPDATE_FIFO_TABLE, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_TM_SCHEDULING_QUEUE_AGING_VECTOR", 1, CORE_0_INDEX, &DS_TM_SCHEDULING_QUEUE_AGING_VECTOR, 5, 1, 1 },
#endif
#if defined BCM6846
	{ "BBH_TX_EGRESS_REPORT_COUNTER_TABLE", 1, CORE_0_INDEX, &BBH_TX_EGRESS_REPORT_COUNTER_TABLE, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_TM_UPDATE_FIFO_TABLE", 1, CORE_0_INDEX, &DS_TM_UPDATE_FIFO_TABLE, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "US_TM_SCHEDULING_AGGREGATION_CONTEXT_VECTOR", 1, CORE_0_INDEX, &US_TM_SCHEDULING_AGGREGATION_CONTEXT_VECTOR, 5, 1, 1 },
#endif
#if defined BCM6846
	{ "EPON_UPDATE_FIFO_TABLE", 1, CORE_0_INDEX, &EPON_UPDATE_FIFO_TABLE, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_TM_SCHEDULING_AGGREGATION_CONTEXT_VECTOR", 1, CORE_0_INDEX, &DS_TM_SCHEDULING_AGGREGATION_CONTEXT_VECTOR, 5, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_TM_BBH_QUEUE_TABLE", 1, CORE_0_INDEX, &DS_TM_BBH_QUEUE_TABLE, 5, 1, 1 },
#endif
#if defined BCM6846
	{ "GHOST_REPORTING_QUEUE_STATUS_BIT_VECTOR_TABLE", 1, CORE_0_INDEX, &GHOST_REPORTING_QUEUE_STATUS_BIT_VECTOR_TABLE, 5, 1, 1 },
#endif
#if defined BCM6846
	{ "RATE_LIMITER_VALID_TABLE_US", 1, CORE_0_INDEX, &RATE_LIMITER_VALID_TABLE_US, 4, 1, 1 },
#endif
#if defined BCM6846
	{ "FPM_GLOBAL_CFG", 1, CORE_0_INDEX, &FPM_GLOBAL_CFG, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "RATE_LIMITER_VALID_TABLE_DS", 1, CORE_0_INDEX, &RATE_LIMITER_VALID_TABLE_DS, 4, 1, 1 },
#endif
#if defined BCM6846
	{ "XGPON_REPORT_ZERO_SENT_TABLE", 1, CORE_0_INDEX, &XGPON_REPORT_ZERO_SENT_TABLE, 10, 1, 1 },
#endif
#if defined BCM6846
	{ "TX_FLOW_TABLE", 1, CORE_1_INDEX, &TX_FLOW_TABLE_1, 320, 1, 1 },
#endif
#if defined BCM6846
	{ "DIRECT_PROCESSING_PD_TABLE", 1, CORE_1_INDEX, &DIRECT_PROCESSING_PD_TABLE_1, 2, 1, 1 },
#endif
#if defined BCM6846
	{ "RUNNER_GLOBAL_REGISTERS_INIT", 1, CORE_1_INDEX, &RUNNER_GLOBAL_REGISTERS_INIT_1, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_CPU_RX_METER_TABLE", 1, CORE_1_INDEX, &DS_CPU_RX_METER_TABLE_1, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "TCAM_IC_CMD_TABLE", 1, CORE_1_INDEX, &TCAM_IC_CMD_TABLE_1, 7, 16, 1 },
#endif
#if defined BCM6846
	{ "GLOBAL_DSCP_TO_PBITS_TABLE", 1, CORE_1_INDEX, &GLOBAL_DSCP_TO_PBITS_TABLE_1, 1, 64, 1 },
#endif
#if defined BCM6846
	{ "CPU_RX_SCRATCHPAD", 1, CORE_1_INDEX, &CPU_RX_SCRATCHPAD_1, 64, 1, 1 },
#endif
#if defined BCM6846
	{ "LAYER2_GRE_TUNNEL_TABLE", 1, CORE_1_INDEX, &LAYER2_GRE_TUNNEL_TABLE_1, 12, 1, 1 },
#endif
#if defined BCM6846
	{ "US_CPU_RX_METER_TABLE", 1, CORE_1_INDEX, &US_CPU_RX_METER_TABLE_1, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "RX_FLOW_TABLE", 1, CORE_1_INDEX, &RX_FLOW_TABLE_1, 320, 1, 1 },
#endif
#if defined BCM6846
	{ "LAYER2_HEADER_COPY_MAPPING_TABLE", 1, CORE_1_INDEX, &LAYER2_HEADER_COPY_MAPPING_TABLE_1, 30, 1, 1 },
#endif
#if defined BCM6846
	{ "IPTV_CONFIGURATION_TABLE", 1, CORE_1_INDEX, &IPTV_CONFIGURATION_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DSCP_TO_PBITS_MAP_TABLE", 1, CORE_1_INDEX, &DSCP_TO_PBITS_MAP_TABLE_1, 4, 64, 1 },
#endif
#if defined BCM6846
	{ "TC_TO_QUEUE_TABLE", 1, CORE_1_INDEX, &TC_TO_QUEUE_TABLE_1, 65, 1, 1 },
#endif
#if defined BCM6846
	{ "GENERAL_QUEUE_DYNAMIC_MNG_TABLE", 1, CORE_1_INDEX, &GENERAL_QUEUE_DYNAMIC_MNG_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "FPM_POOL_NUMBER_MAPPING_TABLE", 1, CORE_1_INDEX, &FPM_POOL_NUMBER_MAPPING_TABLE_1, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "VPORT_TO_DSCP_TO_PBITS_TABLE", 1, CORE_1_INDEX, &VPORT_TO_DSCP_TO_PBITS_TABLE_1, 32, 1, 1 },
#endif
#if defined BCM6846
	{ "PBIT_TO_GEM_TABLE", 1, CORE_1_INDEX, &PBIT_TO_GEM_TABLE_1, 8, 8, 1 },
#endif
#if defined BCM6846
	{ "CPU_REASON_AND_VPORT_TO_METER_TABLE", 1, CORE_1_INDEX, &CPU_REASON_AND_VPORT_TO_METER_TABLE_1, 120, 1, 1 },
#endif
#if defined BCM6846
	{ "PROCESSING_QUEUE_DYNAMIC_MNG_TABLE", 1, CORE_1_INDEX, &PROCESSING_QUEUE_DYNAMIC_MNG_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RING_INTERRUPT_COUNTER_TABLE", 1, CORE_1_INDEX, &CPU_RING_INTERRUPT_COUNTER_TABLE_1, 18, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RECYCLE_INTERRUPT_COALESCING_TABLE", 1, CORE_1_INDEX, &CPU_RECYCLE_INTERRUPT_COALESCING_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "TCAM_IC_HANDLER_TABLE", 1, CORE_1_INDEX, &TCAM_IC_HANDLER_TABLE_1, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "DUAL_STACK_LITE_TABLE", 1, CORE_1_INDEX, &DUAL_STACK_LITE_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_PACKET_BUFFER", 1, CORE_1_INDEX, &DS_PACKET_BUFFER_1, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "PBIT_TO_QUEUE_TABLE", 1, CORE_1_INDEX, &PBIT_TO_QUEUE_TABLE_1, 65, 1, 1 },
#endif
#if defined BCM6846
	{ "RX_FLOW_CONTEXT_DDR_ADDR", 1, CORE_1_INDEX, &RX_FLOW_CONTEXT_DDR_ADDR_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RX_PSRAM_GET_NEXT_SCRATCHPAD", 1, CORE_1_INDEX, &CPU_RX_PSRAM_GET_NEXT_SCRATCHPAD_1, 4, 1, 1 },
#endif
#if defined BCM6846
	{ "BRIDGE_CFG_TABLE", 1, CORE_1_INDEX, &BRIDGE_CFG_TABLE_1, 3, 1, 1 },
#endif
#if defined BCM6846
	{ "NULL_BUFFER", 1, CORE_1_INDEX, &NULL_BUFFER_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_CPU_REASON_TO_METER_TABLE", 1, CORE_1_INDEX, &DS_CPU_REASON_TO_METER_TABLE_1, 64, 1, 1 },
#endif
#if defined BCM6846
	{ "POLICER_PARAMS_TABLE", 1, CORE_1_INDEX, &POLICER_PARAMS_TABLE_1, 80, 1, 1 },
#endif
#if defined BCM6846
	{ "SCRATCH", 1, CORE_1_INDEX, &SCRATCH_1, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "INGRESS_FILTER_L2_REASON_TABLE", 1, CORE_1_INDEX, &INGRESS_FILTER_L2_REASON_TABLE_1, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "INGRESS_FILTER_PROFILE_TABLE", 1, CORE_1_INDEX, &INGRESS_FILTER_PROFILE_TABLE_1, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RX_COPY_PD_FIFO_TABLE", 1, CORE_1_INDEX, &CPU_RX_COPY_PD_FIFO_TABLE_1, 4, 1, 1 },
#endif
#if defined BCM6846
	{ "US_CPU_REASON_TO_METER_TABLE", 1, CORE_1_INDEX, &US_CPU_REASON_TO_METER_TABLE_1, 64, 1, 1 },
#endif
#if defined BCM6846
	{ "RUNNER_PROFILING_TRACE_BUFFER", 1, CORE_1_INDEX, &RUNNER_PROFILING_TRACE_BUFFER_1, 128, 1, 1 },
#endif
#if defined BCM6846
	{ "VPORT_CFG_TABLE", 1, CORE_1_INDEX, &VPORT_CFG_TABLE_1, 40, 1, 1 },
#endif
#if defined BCM6846
	{ "REGISTERS_BUFFER", 1, CORE_1_INDEX, &REGISTERS_BUFFER_1, 32, 1, 1 },
#endif
#if defined BCM6846
	{ "LOOPBACK_QUEUE_TABLE", 1, CORE_1_INDEX, &LOOPBACK_QUEUE_TABLE_1, 40, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR", 1, CORE_1_INDEX, &CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_RX_COMPLETE_0_DISPATCHER_CREDIT_TABLE", 1, CORE_1_INDEX, &DHD_RX_COMPLETE_0_DISPATCHER_CREDIT_TABLE_1, 3, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RX_INTERRUPT_SCRATCH", 1, CORE_1_INDEX, &CPU_RX_INTERRUPT_SCRATCH_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RECYCLE_SRAM_PD_FIFO", 1, CORE_1_INDEX, &CPU_RECYCLE_SRAM_PD_FIFO_1, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "NATC_TBL_CFG", 1, CORE_1_INDEX, &NATC_TBL_CFG_1, 2, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_RX_COMPLETE_1_DISPATCHER_CREDIT_TABLE", 1, CORE_1_INDEX, &DHD_RX_COMPLETE_1_DISPATCHER_CREDIT_TABLE_1, 3, 1, 1 },
#endif
#if defined BCM6846
	{ "TASK_IDX", 1, CORE_1_INDEX, &TASK_IDX_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "TCAM_IC_CFG_TABLE", 1, CORE_1_INDEX, &TCAM_IC_CFG_TABLE_1, 6, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_RX_COMPLETE_2_DISPATCHER_CREDIT_TABLE", 1, CORE_1_INDEX, &DHD_RX_COMPLETE_2_DISPATCHER_CREDIT_TABLE_1, 3, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_FEED_RING_INTERRUPT_COUNTER_MAX", 1, CORE_1_INDEX, &CPU_FEED_RING_INTERRUPT_COUNTER_MAX_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "FORCE_IC_IP_FLOW", 1, CORE_1_INDEX, &FORCE_IC_IP_FLOW_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "FORCE_DSCP", 1, CORE_1_INDEX, &FORCE_DSCP_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RX_COPY_DISPATCHER_CREDIT_TABLE", 1, CORE_1_INDEX, &CPU_RX_COPY_DISPATCHER_CREDIT_TABLE_1, 3, 1, 1 },
#endif
#if defined BCM6846
	{ "CORE_ID_TABLE", 1, CORE_1_INDEX, &CORE_ID_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "SRAM_DUMMY_STORE", 1, CORE_1_INDEX, &SRAM_DUMMY_STORE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_FEED_RING_INTERRUPT_THRESHOLD", 1, CORE_1_INDEX, &CPU_FEED_RING_INTERRUPT_THRESHOLD_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "FLOW_BASED_ACTION_PTR_TABLE", 1, CORE_1_INDEX, &FLOW_BASED_ACTION_PTR_TABLE_1, 17, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_FEED_RING_INTERRUPT_COUNTER", 1, CORE_1_INDEX, &CPU_FEED_RING_INTERRUPT_COUNTER_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_FPM_LOW_THRESHOLD", 1, CORE_1_INDEX, &DHD_FPM_LOW_THRESHOLD_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_FPM_HIGH_THRESHOLD", 1, CORE_1_INDEX, &DHD_FPM_HIGH_THRESHOLD_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RX_INTERRUPT_ID_DDR_ADDR", 1, CORE_1_INDEX, &CPU_RX_INTERRUPT_ID_DDR_ADDR_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RECYCLE_RING_DESCRIPTOR_TABLE", 1, CORE_1_INDEX, &CPU_RECYCLE_RING_DESCRIPTOR_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_COMPLETE_COMMON_RADIO_DATA", 1, CORE_1_INDEX, &DHD_COMPLETE_COMMON_RADIO_DATA_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RECYCLE_RING_INDEX_DDR_ADDR_TABLE", 1, CORE_1_INDEX, &CPU_RECYCLE_RING_INDEX_DDR_ADDR_TABLE_1, 2, 1, 1 },
#endif
#if defined BCM6846
	{ "VPORT_CFG_EX_TABLE", 1, CORE_1_INDEX, &VPORT_CFG_EX_TABLE_1, 40, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR", 1, CORE_1_INDEX, &DHD_CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_INTERRUPT_COALESCING_TABLE", 1, CORE_1_INDEX, &CPU_INTERRUPT_COALESCING_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "RULE_BASED_ACTION_PTR_TABLE", 1, CORE_1_INDEX, &RULE_BASED_ACTION_PTR_TABLE_1, 17, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_RX_POST_RING_SIZE", 1, CORE_1_INDEX, &DHD_RX_POST_RING_SIZE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_RX_COMPLETE_RING_SIZE", 1, CORE_1_INDEX, &DHD_RX_COMPLETE_RING_SIZE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_TX_COMPLETE_RING_SIZE", 1, CORE_1_INDEX, &DHD_TX_COMPLETE_RING_SIZE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "SYSTEM_CONFIGURATION", 1, CORE_1_INDEX, &SYSTEM_CONFIGURATION_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "RATE_LIMIT_OVERHEAD", 1, CORE_1_INDEX, &RATE_LIMIT_OVERHEAD_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "RX_MIRRORING_CONFIGURATION", 1, CORE_1_INDEX, &RX_MIRRORING_CONFIGURATION_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_FEED_RING_DESCRIPTOR_TABLE", 1, CORE_1_INDEX, &CPU_FEED_RING_DESCRIPTOR_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_VPORT_TO_METER_TABLE", 1, CORE_1_INDEX, &CPU_VPORT_TO_METER_TABLE_1, 40, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RX_LOCAL_SCRATCH", 1, CORE_1_INDEX, &CPU_RX_LOCAL_SCRATCH_1, 2, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RECYCLE_RING_INTERRUPT_COUNTER_TABLE", 1, CORE_1_INDEX, &CPU_RECYCLE_RING_INTERRUPT_COUNTER_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "IPTV_ACTION_PTR_TABLE", 1, CORE_1_INDEX, &IPTV_ACTION_PTR_TABLE_1, 17, 1, 1 },
#endif
#if defined BCM6846
	{ "INGRESS_QOS_DONT_DROP_RATIO", 1, CORE_1_INDEX, &INGRESS_QOS_DONT_DROP_RATIO_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "INGRESS_QOS_WAN_UNTAGGED_PRIORITY", 1, CORE_1_INDEX, &INGRESS_QOS_WAN_UNTAGGED_PRIORITY_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "MAC_TYPE", 1, CORE_1_INDEX, &MAC_TYPE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "IC_DEBUG_COUNTER_MODE", 1, CORE_1_INDEX, &IC_DEBUG_COUNTER_MODE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "IC_MCAST_ENABLE", 1, CORE_1_INDEX, &IC_MCAST_ENABLE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_REDIRECT_MODE", 1, CORE_1_INDEX, &CPU_REDIRECT_MODE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RX_COPY_LOCAL_SCRATCH", 1, CORE_1_INDEX, &CPU_RX_COPY_LOCAL_SCRATCH_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "FPM_GLOBAL_CFG", 1, CORE_1_INDEX, &FPM_GLOBAL_CFG_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "IPTV_CLASSIFICATION_CFG_TABLE", 1, CORE_1_INDEX, &IPTV_CLASSIFICATION_CFG_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "INGRESS_FILTER_1588_CFG", 1, CORE_1_INDEX, &INGRESS_FILTER_1588_CFG_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "VLAN_ACTION_GPE_HANDLER_PTR_TABLE", 1, CORE_1_INDEX, &VLAN_ACTION_GPE_HANDLER_PTR_TABLE_1, 17, 1, 1 },
#endif
#if defined BCM6846
	{ "TUNNELS_PARSING_CFG", 1, CORE_1_INDEX, &TUNNELS_PARSING_CFG_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_DOORBELL_VALUE", 1, CORE_1_INDEX, &DHD_DOORBELL_VALUE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "IPTV_CFG_TABLE", 1, CORE_1_INDEX, &IPTV_CFG_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "UPDATE_FIFO_TABLE", 1, CORE_1_INDEX, &UPDATE_FIFO_TABLE_1, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "IPTV_DDR_CTX_TABLE_ADDRESS", 1, CORE_1_INDEX, &IPTV_DDR_CTX_TABLE_ADDRESS_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "NAT_CACHE_CFG", 1, CORE_1_INDEX, &NAT_CACHE_CFG_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DIRECT_PROCESSING_EPON_CONTROL_SCRATCH", 1, CORE_1_INDEX, &DIRECT_PROCESSING_EPON_CONTROL_SCRATCH_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "INGRESS_FILTER_CFG", 1, CORE_1_INDEX, &INGRESS_FILTER_CFG_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RX_COPY_UPDATE_FIFO_TABLE", 1, CORE_1_INDEX, &CPU_RX_COPY_UPDATE_FIFO_TABLE_1, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "PD_FIFO_TABLE", 1, CORE_1_INDEX, &PD_FIFO_TABLE_1, 2, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_REASON_TO_TC", 1, CORE_1_INDEX, &CPU_REASON_TO_TC_1, 64, 1, 1 },
#endif
#if defined BCM6846
	{ "TC_TO_CPU_RXQ", 1, CORE_1_INDEX, &TC_TO_CPU_RXQ_1, 64, 1, 1 },
#endif
#if defined BCM6846
	{ "EXC_TC_TO_CPU_RXQ", 1, CORE_1_INDEX, &EXC_TC_TO_CPU_RXQ_1, 64, 1, 1 },
#endif
#if defined BCM6846
	{ "VPORT_TO_CPU_OBJ", 1, CORE_1_INDEX, &VPORT_TO_CPU_OBJ_1, 40, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_RX_COMPLETE_FLOW_RING_BUFFER", 1, CORE_1_INDEX, &DHD_RX_COMPLETE_FLOW_RING_BUFFER_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_TX_COMPLETE_FLOW_RING_BUFFER", 1, CORE_1_INDEX, &DHD_TX_COMPLETE_FLOW_RING_BUFFER_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_RX_POST_FLOW_RING_BUFFER", 1, CORE_1_INDEX, &DHD_RX_POST_FLOW_RING_BUFFER_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_FPM_REPLY", 1, CORE_1_INDEX, &DHD_FPM_REPLY_1, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_CPU_RECYCLE_RING_DESCRIPTOR_TABLE", 1, CORE_1_INDEX, &DHD_CPU_RECYCLE_RING_DESCRIPTOR_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_HW_CFG", 1, CORE_1_INDEX, &DHD_HW_CFG_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RECYCLE_INTERRUPT_SCRATCH", 1, CORE_1_INDEX, &CPU_RECYCLE_INTERRUPT_SCRATCH_1, 2, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RING_DESCRIPTORS_TABLE", 1, CORE_1_INDEX, &CPU_RING_DESCRIPTORS_TABLE_1, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RECYCLE_SHADOW_RD_IDX", 1, CORE_1_INDEX, &CPU_RECYCLE_SHADOW_RD_IDX_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RECYCLE_SHADOW_WR_IDX", 1, CORE_1_INDEX, &CPU_RECYCLE_SHADOW_WR_IDX_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_TX_SCRATCHPAD", 1, CORE_2_INDEX, &CPU_TX_SCRATCHPAD_2, 128, 1, 1 },
#endif
#if defined BCM6846
	{ "RX_FLOW_TABLE", 1, CORE_2_INDEX, &RX_FLOW_TABLE_2, 320, 1, 1 },
#endif
#if defined BCM6846
	{ "LAYER2_HEADER_COPY_MAPPING_TABLE", 1, CORE_2_INDEX, &LAYER2_HEADER_COPY_MAPPING_TABLE_2, 30, 1, 1 },
#endif
#if defined BCM6846
	{ "IPTV_CONFIGURATION_TABLE", 1, CORE_2_INDEX, &IPTV_CONFIGURATION_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DSCP_TO_PBITS_MAP_TABLE", 1, CORE_2_INDEX, &DSCP_TO_PBITS_MAP_TABLE_2, 4, 64, 1 },
#endif
#if defined BCM6846
	{ "TC_TO_QUEUE_TABLE", 1, CORE_2_INDEX, &TC_TO_QUEUE_TABLE_2, 65, 1, 1 },
#endif
#if defined BCM6846
	{ "GENERAL_QUEUE_DYNAMIC_MNG_TABLE", 1, CORE_2_INDEX, &GENERAL_QUEUE_DYNAMIC_MNG_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "FPM_POOL_NUMBER_MAPPING_TABLE", 1, CORE_2_INDEX, &FPM_POOL_NUMBER_MAPPING_TABLE_2, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "RUNNER_GLOBAL_REGISTERS_INIT", 1, CORE_2_INDEX, &RUNNER_GLOBAL_REGISTERS_INIT_2, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "GLOBAL_DSCP_TO_PBITS_TABLE", 1, CORE_2_INDEX, &GLOBAL_DSCP_TO_PBITS_TABLE_2, 1, 64, 1 },
#endif
#if defined BCM6846
	{ "POLICER_PARAMS_TABLE", 1, CORE_2_INDEX, &POLICER_PARAMS_TABLE_2, 80, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_TX_FPM_POOL_NUMBER_MAPPING_TABLE", 1, CORE_2_INDEX, &CPU_TX_FPM_POOL_NUMBER_MAPPING_TABLE_2, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "VPORT_TO_DSCP_TO_PBITS_TABLE", 1, CORE_2_INDEX, &VPORT_TO_DSCP_TO_PBITS_TABLE_2, 32, 1, 1 },
#endif
#if defined BCM6846
	{ "INGRESS_FILTER_PROFILE_TABLE", 1, CORE_2_INDEX, &INGRESS_FILTER_PROFILE_TABLE_2, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "PBIT_TO_GEM_TABLE", 1, CORE_2_INDEX, &PBIT_TO_GEM_TABLE_2, 8, 8, 1 },
#endif
#if defined BCM6846
	{ "DUAL_STACK_LITE_TABLE", 1, CORE_2_INDEX, &DUAL_STACK_LITE_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "PBIT_TO_QUEUE_TABLE", 1, CORE_2_INDEX, &PBIT_TO_QUEUE_TABLE_2, 65, 1, 1 },
#endif
#if defined BCM6846
	{ "REGISTERS_BUFFER", 1, CORE_2_INDEX, &REGISTERS_BUFFER_2, 32, 1, 1 },
#endif
#if defined BCM6846
	{ "PROCESSING_QUEUE_DYNAMIC_MNG_TABLE", 1, CORE_2_INDEX, &PROCESSING_QUEUE_DYNAMIC_MNG_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_FPM_POOL_NUMBER_MAPPING_TABLE", 1, CORE_2_INDEX, &DHD_FPM_POOL_NUMBER_MAPPING_TABLE_2, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "TCAM_IC_HANDLER_TABLE", 1, CORE_2_INDEX, &TCAM_IC_HANDLER_TABLE_2, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "WLAN_MCAST_DFT_LIST_ENTRY_SCRATCH", 1, CORE_2_INDEX, &WLAN_MCAST_DFT_LIST_ENTRY_SCRATCH_2, 64, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_POST_COMMON_RADIO_DATA", 1, CORE_2_INDEX, &DHD_POST_COMMON_RADIO_DATA_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "RX_FLOW_CONTEXT_DDR_ADDR", 1, CORE_2_INDEX, &RX_FLOW_CONTEXT_DDR_ADDR_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "FPM_REPLY", 1, CORE_2_INDEX, &FPM_REPLY_2, 2, 1, 1 },
#endif
#if defined BCM6846
	{ "BRIDGE_CFG_TABLE", 1, CORE_2_INDEX, &BRIDGE_CFG_TABLE_2, 3, 1, 1 },
#endif
#if defined BCM6846
	{ "NULL_BUFFER", 1, CORE_2_INDEX, &NULL_BUFFER_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "WLAN_MCAST_DFT_LIST_SIZE", 1, CORE_2_INDEX, &WLAN_MCAST_DFT_LIST_SIZE_2, 64, 1, 1 },
#endif
#if defined BCM6846
	{ "LOOPBACK_QUEUE_TABLE", 1, CORE_2_INDEX, &LOOPBACK_QUEUE_TABLE_2, 40, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR", 1, CORE_2_INDEX, &CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RECYCLE_INTERRUPT_COALESCING_TABLE", 1, CORE_2_INDEX, &CPU_RECYCLE_INTERRUPT_COALESCING_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_PACKET_BUFFER", 1, CORE_2_INDEX, &DS_PACKET_BUFFER_2, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "RUNNER_PROFILING_TRACE_BUFFER", 1, CORE_2_INDEX, &RUNNER_PROFILING_TRACE_BUFFER_2, 128, 1, 1 },
#endif
#if defined BCM6846
	{ "TCAM_IC_CMD_TABLE", 1, CORE_2_INDEX, &TCAM_IC_CMD_TABLE_2, 7, 16, 1 },
#endif
#if defined BCM6846
	{ "QUEUE_THRESHOLD_VECTOR", 1, CORE_2_INDEX, &QUEUE_THRESHOLD_VECTOR_2, 9, 1, 1 },
#endif
#if defined BCM6846
	{ "TASK_IDX", 1, CORE_2_INDEX, &TASK_IDX_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RX_INTERRUPT_ID_DDR_ADDR", 1, CORE_2_INDEX, &CPU_RX_INTERRUPT_ID_DDR_ADDR_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "INGRESS_FILTER_L2_REASON_TABLE", 1, CORE_2_INDEX, &INGRESS_FILTER_L2_REASON_TABLE_2, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "WLAN_MCAST_DHD_STATION_TABLE", 1, CORE_2_INDEX, &WLAN_MCAST_DHD_STATION_TABLE_2, 64, 1, 1 },
#endif
#if defined BCM6846
	{ "LAYER2_GRE_TUNNEL_TABLE", 1, CORE_2_INDEX, &LAYER2_GRE_TUNNEL_TABLE_2, 12, 1, 1 },
#endif
#if defined BCM6846
	{ "TCAM_IC_CFG_TABLE", 1, CORE_2_INDEX, &TCAM_IC_CFG_TABLE_2, 6, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_TX_EGRESS_DISPATCHER_CREDIT_TABLE", 1, CORE_2_INDEX, &CPU_TX_EGRESS_DISPATCHER_CREDIT_TABLE_2, 3, 1, 1 },
#endif
#if defined BCM6846
	{ "FORCE_IC_IP_FLOW", 1, CORE_2_INDEX, &FORCE_IC_IP_FLOW_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "FORCE_DSCP", 1, CORE_2_INDEX, &FORCE_DSCP_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CORE_ID_TABLE", 1, CORE_2_INDEX, &CORE_ID_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "SRAM_DUMMY_STORE", 1, CORE_2_INDEX, &SRAM_DUMMY_STORE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_TX_INGRESS_DISPATCHER_CREDIT_TABLE", 1, CORE_2_INDEX, &CPU_TX_INGRESS_DISPATCHER_CREDIT_TABLE_2, 3, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_FPM_LOW_THRESHOLD", 1, CORE_2_INDEX, &DHD_FPM_LOW_THRESHOLD_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_FPM_HIGH_THRESHOLD", 1, CORE_2_INDEX, &DHD_FPM_HIGH_THRESHOLD_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "TX_FLOW_TABLE", 1, CORE_2_INDEX, &TX_FLOW_TABLE_2, 320, 1, 1 },
#endif
#if defined BCM6846
	{ "FLOW_BASED_ACTION_PTR_TABLE", 1, CORE_2_INDEX, &FLOW_BASED_ACTION_PTR_TABLE_2, 17, 1, 1 },
#endif
#if defined BCM6846
	{ "RX_MIRRORING_CONFIGURATION", 1, CORE_2_INDEX, &RX_MIRRORING_CONFIGURATION_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "RATE_LIMIT_OVERHEAD", 1, CORE_2_INDEX, &RATE_LIMIT_OVERHEAD_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "INGRESS_QOS_DONT_DROP_RATIO", 1, CORE_2_INDEX, &INGRESS_QOS_DONT_DROP_RATIO_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "INGRESS_QOS_WAN_UNTAGGED_PRIORITY", 1, CORE_2_INDEX, &INGRESS_QOS_WAN_UNTAGGED_PRIORITY_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "IC_DEBUG_COUNTER_MODE", 1, CORE_2_INDEX, &IC_DEBUG_COUNTER_MODE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR", 1, CORE_2_INDEX, &DHD_CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_MCAST_DISPATCHER_CREDIT_TABLE", 1, CORE_2_INDEX, &DHD_MCAST_DISPATCHER_CREDIT_TABLE_2, 3, 1, 1 },
#endif
#if defined BCM6846
	{ "IC_MCAST_ENABLE", 1, CORE_2_INDEX, &IC_MCAST_ENABLE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_REDIRECT_MODE", 1, CORE_2_INDEX, &CPU_REDIRECT_MODE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "IPTV_CLASSIFICATION_CFG_TABLE", 1, CORE_2_INDEX, &IPTV_CLASSIFICATION_CFG_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "INGRESS_FILTER_1588_CFG", 1, CORE_2_INDEX, &INGRESS_FILTER_1588_CFG_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "VPORT_CFG_EX_TABLE", 1, CORE_2_INDEX, &VPORT_CFG_EX_TABLE_2, 40, 1, 1 },
#endif
#if defined BCM6846
	{ "WLAN_MCAST_DFT_ADDR", 1, CORE_2_INDEX, &WLAN_MCAST_DFT_ADDR_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RECYCLE_RING_DESCRIPTOR_TABLE", 1, CORE_2_INDEX, &CPU_RECYCLE_RING_DESCRIPTOR_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "RULE_BASED_ACTION_PTR_TABLE", 1, CORE_2_INDEX, &RULE_BASED_ACTION_PTR_TABLE_2, 17, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RECYCLE_RING_INDEX_DDR_ADDR_TABLE", 1, CORE_2_INDEX, &CPU_RECYCLE_RING_INDEX_DDR_ADDR_TABLE_2, 2, 1, 1 },
#endif
#if defined BCM6846
	{ "SYSTEM_CONFIGURATION", 1, CORE_2_INDEX, &SYSTEM_CONFIGURATION_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "VPORT_CFG_TABLE", 1, CORE_2_INDEX, &VPORT_CFG_TABLE_2, 40, 1, 1 },
#endif
#if defined BCM6846
	{ "IPTV_ACTION_PTR_TABLE", 1, CORE_2_INDEX, &IPTV_ACTION_PTR_TABLE_2, 17, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RECYCLE_RING_INTERRUPT_COUNTER_TABLE", 1, CORE_2_INDEX, &CPU_RECYCLE_RING_INTERRUPT_COUNTER_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_FEED_RING_DESCRIPTOR_TABLE", 1, CORE_2_INDEX, &CPU_FEED_RING_DESCRIPTOR_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "VLAN_ACTION_GPE_HANDLER_PTR_TABLE", 1, CORE_2_INDEX, &VLAN_ACTION_GPE_HANDLER_PTR_TABLE_2, 17, 1, 1 },
#endif
#if defined BCM6846
	{ "TUNNELS_PARSING_CFG", 1, CORE_2_INDEX, &TUNNELS_PARSING_CFG_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "FPM_GLOBAL_CFG", 1, CORE_2_INDEX, &FPM_GLOBAL_CFG_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_TX_EGRESS_UPDATE_FIFO_TABLE", 1, CORE_2_INDEX, &CPU_TX_EGRESS_UPDATE_FIFO_TABLE_2, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "WLAN_MCAST_SCRATCHPAD", 1, CORE_2_INDEX, &WLAN_MCAST_SCRATCHPAD_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "IPTV_CFG_TABLE", 1, CORE_2_INDEX, &IPTV_CFG_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_DOORBELL_VALUE", 1, CORE_2_INDEX, &DHD_DOORBELL_VALUE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "IPTV_DDR_CTX_TABLE_ADDRESS", 1, CORE_2_INDEX, &IPTV_DDR_CTX_TABLE_ADDRESS_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RECYCLE_SRAM_PD_FIFO", 1, CORE_2_INDEX, &CPU_RECYCLE_SRAM_PD_FIFO_2, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "NATC_TBL_CFG", 1, CORE_2_INDEX, &NATC_TBL_CFG_2, 2, 1, 1 },
#endif
#if defined BCM6846
	{ "NAT_CACHE_CFG", 1, CORE_2_INDEX, &NAT_CACHE_CFG_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "INGRESS_FILTER_CFG", 1, CORE_2_INDEX, &INGRESS_FILTER_CFG_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_TX_INGRESS_UPDATE_FIFO_TABLE", 1, CORE_2_INDEX, &CPU_TX_INGRESS_UPDATE_FIFO_TABLE_2, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_TX_POST_UPDATE_FIFO_TABLE", 1, CORE_2_INDEX, &DHD_TX_POST_UPDATE_FIFO_TABLE_2, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_MCAST_UPDATE_FIFO_TABLE", 1, CORE_2_INDEX, &DHD_MCAST_UPDATE_FIFO_TABLE_2, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_TX_POST_PD_FIFO_TABLE", 1, CORE_2_INDEX, &DHD_TX_POST_PD_FIFO_TABLE_2, 12, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_TX_EGRESS_PD_FIFO_TABLE", 1, CORE_2_INDEX, &CPU_TX_EGRESS_PD_FIFO_TABLE_2, 2, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_TX_POST_FLOW_RING_BUFFER", 1, CORE_2_INDEX, &DHD_TX_POST_FLOW_RING_BUFFER_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_TX_INGRESS_PD_FIFO_TABLE", 1, CORE_2_INDEX, &CPU_TX_INGRESS_PD_FIFO_TABLE_2, 2, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_MCAST_PD_FIFO_TABLE", 1, CORE_2_INDEX, &DHD_MCAST_PD_FIFO_TABLE_2, 2, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_FLOW_RING_CACHE_CTX_TABLE", 1, CORE_2_INDEX, &DHD_FLOW_RING_CACHE_CTX_TABLE_2, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_FLOW_RING_CACHE_LKP_TABLE", 1, CORE_2_INDEX, &DHD_FLOW_RING_CACHE_LKP_TABLE_2, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_L2_HEADER", 1, CORE_2_INDEX, &DHD_L2_HEADER_2, 24, 1, 1 },
#endif
#if defined BCM6846
	{ "WLAN_MCAST_SSID_MAC_ADDRESS_TABLE", 1, CORE_2_INDEX, &WLAN_MCAST_SSID_MAC_ADDRESS_TABLE_2, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_CPU_RECYCLE_RING_DESCRIPTOR_TABLE", 1, CORE_2_INDEX, &DHD_CPU_RECYCLE_RING_DESCRIPTOR_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_TX_POST_FLOW_RING_MGMT_DESCRIPTOR_BUFFER", 1, CORE_2_INDEX, &DHD_TX_POST_FLOW_RING_MGMT_DESCRIPTOR_BUFFER_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_HW_CFG", 1, CORE_2_INDEX, &DHD_HW_CFG_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "WLAN_MCAST_DHD_STATION_CTX_TABLE", 1, CORE_2_INDEX, &WLAN_MCAST_DHD_STATION_CTX_TABLE_2, 64, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RECYCLE_INTERRUPT_SCRATCH", 1, CORE_2_INDEX, &CPU_RECYCLE_INTERRUPT_SCRATCH_2, 2, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RECYCLE_SHADOW_RD_IDX", 1, CORE_2_INDEX, &CPU_RECYCLE_SHADOW_RD_IDX_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RECYCLE_SHADOW_WR_IDX", 1, CORE_2_INDEX, &CPU_RECYCLE_SHADOW_WR_IDX_2, 1, 1, 1 },
#endif
};

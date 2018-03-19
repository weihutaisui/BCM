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
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DS_TM_PD_FIFO_TABLE =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT COMPLEX_SCHEDULER_TABLE =
{
	64,
	{
		{ dump_RDD_COMPLEX_SCHEDULER_DESCRIPTOR, 0x1400 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DS_TM_SCHEDULING_QUEUE_TABLE =
{
	8,
	{
		{ dump_RDD_SCHEDULING_QUEUE_DESCRIPTOR, 0x1800 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT REPORTING_COUNTER_TABLE =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x1d00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RUNNER_GLOBAL_REGISTERS_INIT =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x1da0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DS_TM_TM_FLOW_CNTR_TABLE =
{
	1,
	{
		{ dump_RDD_TM_FLOW_CNTR_ENTRY, 0x1dc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT BASIC_SCHEDULER_TABLE_DS =
{
	16,
	{
		{ dump_RDD_BASIC_SCHEDULER_DESCRIPTOR, 0x1e00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT REPORTING_QUEUE_DESCRIPTOR_TABLE =
{
	16,
	{
		{ dump_RDD_REPORTING_QUEUE_DESCRIPTOR, 0x2000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT REPORTING_QUEUE_ACCUMULATED_TABLE =
{
	8,
	{
		{ dump_RDD_REPORTING_ACCUMULATED_DATA, 0x2a00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DS_TM_BBH_TX_EGRESS_COUNTER_TABLE =
{
	8,
	{
		{ dump_RDD_BBH_TX_EGRESS_COUNTER_ENTRY, 0x2a80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DS_TM_CPU_TX_ABS_COUNTERS =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2ac0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT REGISTERS_BUFFER =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2b40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DS_TM_TM_ACTION_PTR_TABLE =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2bc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT BUDGET_ALLOCATION_TIMER_VALUE =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2be2 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT GHOST_REPORTING_GLOBAL_CFG =
{
	4,
	{
		{ dump_RDD_GHOST_REPORTING_GLOBAL_CFG_ENTRY, 0x2be4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DS_TM_BBH_TX_QUEUE_ID_TABLE =
{
	4,
	{
		{ dump_RDD_BBH_TX_QUEUE_ID_ENTRY, 0x2be8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT XGPON_REPORT_TABLE =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2bf0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RUNNER_PROFILING_TRACE_BUFFER =
{
	4,
	{
		{ dump_RDD_TRACE_EVENT, 0x2c00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT NATC_TBL_CFG =
{
	24,
	{
		{ dump_RDD_NATC_TBL_CONFIGURATION, 0x2e00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DS_TM_FLUSH_DISPATCHER_CREDIT_TABLE =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2e30 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DS_TM_BB_DESTINATION_TABLE =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2e3c },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DS_TM_SCHEDULING_GLOBAL_FLUSH_CFG =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2e3e },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT SCHEDULING_FLUSH_GLOBAL_CFG =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2e3f },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT UPDATE_FIFO_TABLE =
{
	4,
	{
		{ dump_RDD_UPDATE_FIFO_ENTRY, 0x2e40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DS_TM_SCHEDULING_FLUSH_VECTOR =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2e60 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TASK_IDX =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2e74 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT GENERAL_QUEUE_DYNAMIC_MNG_TABLE =
{
	8,
	{
		{ dump_RDD_QUEUE_DYNAMIC_MNG_ENTRY, 0x2e78 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DS_TM_SCHEDULING_QUEUE_AGING_VECTOR =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2e80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TX_MIRRORING_CONFIGURATION =
{
	2,
	{
		{ dump_RDD_MIRRORING_DESCRIPTOR, 0x2e94 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT SRAM_DUMMY_STORE =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2e96 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RATE_LIMIT_OVERHEAD =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2e97 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT MIRRORING_SCRATCH =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0x2e98 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DS_TM_SCHEDULING_AGGREGATION_CONTEXT_VECTOR =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2ea0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DS_TM_FIRST_QUEUE_MAPPING =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2eb4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT BBH_TX_INGRESS_COUNTER_TABLE =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2eb5 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT REPORT_BBH_TX_QUEUE_ID_TABLE =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2eb8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT GHOST_REPORTING_QUEUE_STATUS_BIT_VECTOR_TABLE =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2ec0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT BBH_TX_EGRESS_REPORT_COUNTER_TABLE =
{
	8,
	{
		{ dump_RDD_BBH_TX_EGRESS_COUNTER_ENTRY, 0x2ed8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RATE_LIMITER_VALID_TABLE_DS =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2ee0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TX_MIRRORING_DISPATCHER_CREDIT_TABLE =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2ef0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT FPM_GLOBAL_CFG =
{
	12,
	{
		{ dump_RDD_FPM_GLOBAL_CFG, 0x2f00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT XGPON_REPORT_ZERO_SENT_TABLE =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2f10 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT BASIC_RATE_LIMITER_TABLE_DS =
{
	16,
	{
		{ dump_RDD_BASIC_RATE_LIMITER_DESCRIPTOR, 0x3000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DS_TM_BBH_QUEUE_TABLE =
{
	4,
	{
		{ dump_RDD_BBH_QUEUE_DESCRIPTOR, 0x3800 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT US_TM_PD_FIFO_TABLE_1 =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT COMPLEX_SCHEDULER_TABLE_1 =
{
	64,
	{
		{ dump_RDD_COMPLEX_SCHEDULER_DESCRIPTOR, 0x1400 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT US_TM_SCHEDULING_QUEUE_TABLE_1 =
{
	8,
	{
		{ dump_RDD_SCHEDULING_QUEUE_DESCRIPTOR, 0x1800 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_POST_COMMON_RADIO_DATA_1 =
{
	72,
	{
		{ dump_RDD_DHD_POST_COMMON_RADIO_ENTRY, 0x1d00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT US_TM_BBH_TX_QUEUE_ID_TABLE_1 =
{
	4,
	{
		{ dump_RDD_BBH_TX_QUEUE_ID_ENTRY, 0x1dd8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RUNNER_GLOBAL_REGISTERS_INIT_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x1de0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT BASIC_SCHEDULER_TABLE_US_1 =
{
	16,
	{
		{ dump_RDD_BASIC_SCHEDULER_DESCRIPTOR, 0x1e00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT BASIC_RATE_LIMITER_TABLE_US_1 =
{
	16,
	{
		{ dump_RDD_BASIC_RATE_LIMITER_DESCRIPTOR, 0x2000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_FLOW_RING_CACHE_CTX_TABLE_1 =
{
	16,
	{
		{ dump_RDD_DHD_FLOW_RING_CACHE_CTX_ENTRY, 0x2800 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_TX_POST_FLOW_RING_BUFFER_1 =
{
	48,
	{
		{ dump_RDD_DHD_TX_POST_DESCRIPTOR, 0x2b00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_FPM_POOL_NUMBER_MAPPING_TABLE_1 =
{
	1,
	{
		{ dump_RDD_FPM_POOL_NUMBER, 0x2b90 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT US_TM_SCHEDULING_FLUSH_VECTOR_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2ba0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT BUDGET_ALLOCATION_TIMER_VALUE_1 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2bb4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT US_TM_BB_DESTINATION_TABLE_1 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2bb6 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT BBH_TX_EPON_QUEUE_ID_TABLE_1 =
{
	4,
	{
		{ dump_RDD_BBH_TX_QUEUE_ID_ENTRY, 0x2bb8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT US_TM_BBH_TX_EGRESS_COUNTER_TABLE_1 =
{
	8,
	{
		{ dump_RDD_BBH_TX_EGRESS_COUNTER_ENTRY, 0x2bc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RUNNER_PROFILING_TRACE_BUFFER_1 =
{
	4,
	{
		{ dump_RDD_TRACE_EVENT, 0x2c00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_SSID_MAC_ADDRESS_TABLE_1 =
{
	8,
	{
		{ dump_RDD_WLAN_MCAST_SSID_MAC_ADDRESS_ENTRY, 0x2e00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT US_TM_TM_FLOW_CNTR_TABLE_1 =
{
	1,
	{
		{ dump_RDD_TM_FLOW_CNTR_ENTRY, 0x2f80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_DHD_STATION_TABLE_1 =
{
	8,
	{
		{ dump_RDD_WLAN_MCAST_DHD_STATION_ENTRY, 0x3000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_TX_POST_PD_FIFO_TABLE_1 =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x3200 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_DFT_LIST_ENTRY_SCRATCH_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x32c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_FLOW_RING_CACHE_LKP_TABLE_1 =
{
	2,
	{
		{ dump_RDD_DHD_FLOW_RING_CACHE_LKP_ENTRY, 0x3300 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT US_TM_SCHEDULING_QUEUE_AGING_VECTOR_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x3360 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TASK_IDX_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x3374 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT GENERAL_QUEUE_DYNAMIC_MNG_TABLE_1 =
{
	8,
	{
		{ dump_RDD_QUEUE_DYNAMIC_MNG_ENTRY, 0x3378 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_L2_HEADER_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x3380 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR_1 =
{
	8,
	{
		{ dump_RDD_DDR_ADDRESS, 0x33c8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT OVERALL_RATE_LIMITER_TABLE_1 =
{
	16,
	{
		{ dump_RDD_OVERALL_RATE_LIMITER_DESCRIPTOR, 0x33d0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT US_TM_SCHEDULING_AGGREGATION_CONTEXT_VECTOR_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x33e0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT US_TM_SCHEDULING_GLOBAL_FLUSH_CFG_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x33f4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT SCHEDULING_FLUSH_GLOBAL_CFG_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x33f5 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TX_MIRRORING_CONFIGURATION_1 =
{
	2,
	{
		{ dump_RDD_MIRRORING_DESCRIPTOR, 0x33f6 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_DFT_ADDR_1 =
{
	8,
	{
		{ dump_RDD_PHYS_ADDR_64_PTR, 0x33f8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT US_TM_BBH_QUEUE_TABLE_1 =
{
	4,
	{
		{ dump_RDD_BBH_QUEUE_DESCRIPTOR, 0x3400 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT US_TM_CPU_TX_ABS_COUNTERS_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x34a0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT REGISTERS_BUFFER_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x3520 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RATE_LIMITER_VALID_TABLE_US_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x35a0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT US_TM_FLUSH_DISPATCHER_CREDIT_TABLE_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x35b0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT SRAM_DUMMY_STORE_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x35bc },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RATE_LIMIT_OVERHEAD_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x35bd },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT BBH_TX_FIFO_SIZE_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x35be },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT US_TM_FIRST_QUEUE_MAPPING_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x35bf },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT BBH_TX_EPON_INGRESS_COUNTER_TABLE_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x35c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT MIRRORING_SCRATCH_1 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0x35e8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DISPATCHER_CREDIT_TABLE_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x35f0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_DFT_LIST_SIZE_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x3600 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT BBH_TX_EPON_EGRESS_COUNTER_TABLE_1 =
{
	8,
	{
		{ dump_RDD_BBH_TX_EGRESS_COUNTER_ENTRY, 0x3640 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_MCAST_DISPATCHER_CREDIT_TABLE_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x3670 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_CPU_RECYCLE_RING_DESCRIPTOR_TABLE_1 =
{
	16,
	{
		{ dump_RDD_CPU_RING_DESCRIPTOR, 0x3680 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_DOORBELL_VALUE_1 =
{
	4,
	{
		{ dump_RDD_DHD_DOORBELL, 0x36b0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT US_TM_TM_ACTION_PTR_TABLE_1 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x36c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TX_MIRRORING_DISPATCHER_CREDIT_TABLE_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x36f0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT NATC_TBL_CFG_1 =
{
	24,
	{
		{ dump_RDD_NATC_TBL_CONFIGURATION, 0x3700 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT FPM_GLOBAL_CFG_1 =
{
	12,
	{
		{ dump_RDD_FPM_GLOBAL_CFG, 0x3730 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT UPDATE_FIFO_TABLE_1 =
{
	4,
	{
		{ dump_RDD_UPDATE_FIFO_ENTRY, 0x3740 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_SCRATCHPAD_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x3760 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_TX_POST_FLOW_RING_MGMT_DESCRIPTOR_BUFFER_1 =
{
	16,
	{
		{ dump_RDD_DHD_TX_POST_FLOW_RING_MGMT_DESCRIPTOR, 0x3780 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT EPON_UPDATE_FIFO_TABLE_1 =
{
	4,
	{
		{ dump_RDD_UPDATE_FIFO_ENTRY, 0x37c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_DHD_STATION_CTX_TABLE_1 =
{
	1,
	{
		{ dump_RDD_WLAN_MCAST_DHD_STATION_CTX_ENTRY, 0x3800 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_TX_POST_UPDATE_FIFO_TABLE_1 =
{
	4,
	{
		{ dump_RDD_UPDATE_FIFO_ENTRY, 0x3840 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_MCAST_UPDATE_FIFO_TABLE_1 =
{
	4,
	{
		{ dump_RDD_UPDATE_FIFO_ENTRY, 0x3880 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_MCAST_PD_FIFO_TABLE_1 =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x38c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_HW_CFG_1 =
{
	12,
	{
		{ dump_RDD_DHD_HW_CONFIGURATION, 0x3900 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_TX_SCRATCHPAD_2 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0x0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RX_FLOW_TABLE_2 =
{
	2,
	{
		{ dump_RDD_RX_FLOW_ENTRY, 0x400 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT LAYER2_HEADER_COPY_MAPPING_TABLE_2 =
{
	4,
	{
		{ dump_RDD_LAYER2_HEADER_COPY_MAPPING_ENTRY, 0x680 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IPTV_CONFIGURATION_TABLE_2 =
{
	8,
	{
		{ dump_RDD_HW_IPTV_CONFIGURATION, 0x6f8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DSCP_TO_PBITS_MAP_TABLE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x700 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TC_TO_QUEUE_TABLE_2 =
{
	8,
	{
		{ dump_RDD_TC_TO_QUEUE_8, 0x800 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT GENERAL_QUEUE_DYNAMIC_MNG_TABLE_2 =
{
	8,
	{
		{ dump_RDD_QUEUE_DYNAMIC_MNG_ENTRY, 0xa08 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT FPM_POOL_NUMBER_MAPPING_TABLE_2 =
{
	1,
	{
		{ dump_RDD_FPM_POOL_NUMBER, 0xa10 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RUNNER_GLOBAL_REGISTERS_INIT_2 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0xa20 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT GLOBAL_DSCP_TO_PBITS_TABLE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xa40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT POLICER_PARAMS_TABLE_2 =
{
	1,
	{
		{ dump_RDD_POLICER_PARAMS_ENTRY, 0xa80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_TX_FPM_POOL_NUMBER_MAPPING_TABLE_2 =
{
	1,
	{
		{ dump_RDD_FPM_POOL_NUMBER, 0xad0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT VPORT_TO_DSCP_TO_PBITS_TABLE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xae0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_PROFILE_TABLE_2 =
{
	8,
	{
		{ dump_RDD_INGRESS_FILTER_CTRL, 0xb00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT PBIT_TO_GEM_TABLE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xb80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DUAL_STACK_LITE_TABLE_2 =
{
	64,
	{
		{ dump_RDD_DUAL_STACK_LITE_ENTRY, 0xbc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT PBIT_TO_QUEUE_TABLE_2 =
{
	8,
	{
		{ dump_RDD_PBIT_TO_QUEUE_8, 0xc00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT REGISTERS_BUFFER_2 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0xe08 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT PROCESSING_QUEUE_DYNAMIC_MNG_TABLE_2 =
{
	8,
	{
		{ dump_RDD_QUEUE_DYNAMIC_MNG_ENTRY, 0xe88 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT FPM_REPLY_2 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0xe90 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TCAM_IC_CFG_TABLE_2 =
{
	16,
	{
		{ dump_RDD_TCAM_IC_CFG, 0xea0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT NATC_TBL_CFG_2 =
{
	24,
	{
		{ dump_RDD_NATC_TBL_CONFIGURATION, 0xf00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_INTERRUPT_COALESCING_TABLE_2 =
{
	16,
	{
		{ dump_RDD_CPU_INTERRUPT_COALESCING_ENTRY, 0xf30 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT LOOPBACK_QUEUE_TABLE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xf40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RX_FLOW_CONTEXT_DDR_ADDR_2 =
{
	8,
	{
		{ dump_RDD_DEF_FLOW_CONTEXT_DDR_ADDR, 0xf68 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_L2_REASON_TABLE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xf70 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT VPORT_CFG_EX_TABLE_2 =
{
	1,
	{
		{ dump_RDD_VPORT_CFG_EX_ENTRY, 0xf80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT NULL_BUFFER_2 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0xfa8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_TX_EGRESS_DISPATCHER_CREDIT_TABLE_2 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0xfb0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TASK_IDX_2 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0xfbc },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT QUEUE_THRESHOLD_VECTOR_2 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0xfc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT FORCE_IC_IP_FLOW_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xfe4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT FORCE_DSCP_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xfe5 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CORE_ID_TABLE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xfe6 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT SRAM_DUMMY_STORE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xfe7 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR_2 =
{
	8,
	{
		{ dump_RDD_DDR_ADDRESS, 0xfe8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_TX_INGRESS_DISPATCHER_CREDIT_TABLE_2 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0xff0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_FPM_LOW_THRESHOLD_2 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0xffc },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_FPM_HIGH_THRESHOLD_2 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0xffe },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DS_PACKET_BUFFER_2 =
{
	512,
	{
		{ dump_RDD_PACKET_BUFFER, 0x1000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RUNNER_PROFILING_TRACE_BUFFER_2 =
{
	4,
	{
		{ dump_RDD_TRACE_EVENT, 0x2000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TCAM_IC_CMD_TABLE_2 =
{
	4,
	{
		{ dump_RDD_TCAM_IC_CMD, 0x2200 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT FLOW_BASED_ACTION_PTR_TABLE_2 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x23c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RX_MIRRORING_CONFIGURATION_2 =
{
	2,
	{
		{ dump_RDD_MIRRORING_DESCRIPTOR, 0x23e2 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RATE_LIMIT_OVERHEAD_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x23e4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT INGRESS_QOS_DONT_DROP_RATIO_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x23e5 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT INGRESS_QOS_WAN_UNTAGGED_PRIORITY_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x23e6 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IC_DEBUG_COUNTER_MODE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x23e7 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_RX_INTERRUPT_ID_DDR_ADDR_2 =
{
	8,
	{
		{ dump_RDD_DDR_ADDRESS, 0x23e8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_RING_DESCRIPTOR_TABLE_2 =
{
	16,
	{
		{ dump_RDD_CPU_RING_DESCRIPTOR, 0x23f0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT LAYER2_GRE_TUNNEL_TABLE_2 =
{
	32,
	{
		{ dump_RDD_LAYER2_GRE_TUNNEL_ENTRY, 0x2400 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RULE_BASED_ACTION_PTR_TABLE_2 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2580 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IC_MCAST_ENABLE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x25a2 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_REDIRECT_MODE_2 =
{
	1,
	{
		{ dump_RDD_CPU_REDIRECT_MODE_ENTRY, 0x25a3 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IPTV_CLASSIFICATION_CFG_TABLE_2 =
{
	1,
	{
		{ dump_RDD_IPTV_CLASSIFICATION_CFG_ENTRY, 0x25a4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_1588_CFG_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x25a5 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_RING_INDEX_DDR_ADDR_TABLE_2 =
{
	8,
	{
		{ dump_RDD_DDR_ADDRESS, 0x25a8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT SYSTEM_CONFIGURATION_2 =
{
	5,
	{
		{ dump_RDD_SYSTEM_CONFIGURATION_ENTRY, 0x25b8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IPTV_ACTION_PTR_TABLE_2 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x25c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_RING_INTERRUPT_COUNTER_TABLE_2 =
{
	8,
	{
		{ dump_RDD_CPU_RING_INTERRUPT_COUNTER_ENTRY, 0x25e8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_FEED_RING_DESCRIPTOR_TABLE_2 =
{
	16,
	{
		{ dump_RDD_CPU_RING_DESCRIPTOR, 0x25f0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TX_FLOW_TABLE_2 =
{
	1,
	{
		{ dump_RDD_TX_FLOW_ENTRY, 0x2600 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT VLAN_ACTION_GPE_HANDLER_PTR_TABLE_2 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2740 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TUNNELS_PARSING_CFG_2 =
{
	8,
	{
		{ dump_RDD_TUNNELS_PARSING_CFG, 0x2768 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT FPM_GLOBAL_CFG_2 =
{
	12,
	{
		{ dump_RDD_FPM_GLOBAL_CFG, 0x2770 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_TX_EGRESS_UPDATE_FIFO_TABLE_2 =
{
	4,
	{
		{ dump_RDD_UPDATE_FIFO_ENTRY, 0x2780 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TCAM_IC_HANDLER_TABLE_2 =
{
	2,
	{
		{ dump_RDD_TCAM_IC_HANDLER, 0x27a0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_TX_INGRESS_UPDATE_FIFO_TABLE_2 =
{
	4,
	{
		{ dump_RDD_UPDATE_FIFO_ENTRY, 0x27c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT BRIDGE_CFG_TABLE_2 =
{
	8,
	{
		{ dump_RDD_BRIDGE_CFG, 0x27e0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IPTV_CFG_TABLE_2 =
{
	8,
	{
		{ dump_RDD_IPTV_CFG, 0x27f8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT VPORT_CFG_TABLE_2 =
{
	8,
	{
		{ dump_RDD_VPORT_CFG_ENTRY, 0x2800 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_TX_EGRESS_PD_FIFO_TABLE_2 =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x2940 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IPTV_DDR_CTX_TABLE_ADDRESS_2 =
{
	8,
	{
		{ dump_RDD_IPTV_DDR_CTX_TABLE_ADDRESS, 0x2960 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT NAT_CACHE_CFG_2 =
{
	7,
	{
		{ dump_RDD_NAT_CACHE_CFG, 0x2968 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_CFG_2 =
{
	2,
	{
		{ dump_RDD_INGRESS_FILTER_CFG, 0x2970 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_TX_INGRESS_PD_FIFO_TABLE_2 =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x2980 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_SRAM_PD_FIFO_2 =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x2a00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_INTERRUPT_SCRATCH_2 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2b00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_SHADOW_RD_IDX_2 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2b80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_SHADOW_WR_IDX_2 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2c00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TX_FLOW_TABLE_3 =
{
	1,
	{
		{ dump_RDD_TX_FLOW_ENTRY, 0x0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DIRECT_PROCESSING_PD_TABLE_3 =
{
	16,
	{
		{ dump_RDD_PROCESSING_RX_DESCRIPTOR, 0x140 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RUNNER_GLOBAL_REGISTERS_INIT_3 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x160 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DS_CPU_RX_METER_TABLE_3 =
{
	8,
	{
		{ dump_RDD_CPU_RX_METER_ENTRY, 0x180 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TCAM_IC_CMD_TABLE_3 =
{
	4,
	{
		{ dump_RDD_TCAM_IC_CMD, 0x200 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT GLOBAL_DSCP_TO_PBITS_TABLE_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x3c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_RX_SCRATCHPAD_3 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0x400 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT LAYER2_GRE_TUNNEL_TABLE_3 =
{
	32,
	{
		{ dump_RDD_LAYER2_GRE_TUNNEL_ENTRY, 0x600 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT US_CPU_RX_METER_TABLE_3 =
{
	8,
	{
		{ dump_RDD_CPU_RX_METER_ENTRY, 0x780 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RX_FLOW_TABLE_3 =
{
	2,
	{
		{ dump_RDD_RX_FLOW_ENTRY, 0x800 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT LAYER2_HEADER_COPY_MAPPING_TABLE_3 =
{
	4,
	{
		{ dump_RDD_LAYER2_HEADER_COPY_MAPPING_ENTRY, 0xa80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IPTV_CONFIGURATION_TABLE_3 =
{
	8,
	{
		{ dump_RDD_HW_IPTV_CONFIGURATION, 0xaf8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DSCP_TO_PBITS_MAP_TABLE_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xb00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TC_TO_QUEUE_TABLE_3 =
{
	8,
	{
		{ dump_RDD_TC_TO_QUEUE_8, 0xc00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT GENERAL_QUEUE_DYNAMIC_MNG_TABLE_3 =
{
	8,
	{
		{ dump_RDD_QUEUE_DYNAMIC_MNG_ENTRY, 0xe08 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT FPM_POOL_NUMBER_MAPPING_TABLE_3 =
{
	1,
	{
		{ dump_RDD_FPM_POOL_NUMBER, 0xe10 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT VPORT_TO_DSCP_TO_PBITS_TABLE_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xe20 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT PBIT_TO_GEM_TABLE_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xe40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_REASON_AND_VPORT_TO_METER_TABLE_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xe80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT PROCESSING_QUEUE_DYNAMIC_MNG_TABLE_3 =
{
	8,
	{
		{ dump_RDD_QUEUE_DYNAMIC_MNG_ENTRY, 0xef8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_RING_INTERRUPT_COUNTER_TABLE_3 =
{
	8,
	{
		{ dump_RDD_CPU_RING_INTERRUPT_COUNTER_ENTRY, 0xf00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_INTERRUPT_COALESCING_TABLE_3 =
{
	16,
	{
		{ dump_RDD_CPU_INTERRUPT_COALESCING_ENTRY, 0xf90 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TCAM_IC_HANDLER_TABLE_3 =
{
	2,
	{
		{ dump_RDD_TCAM_IC_HANDLER, 0xfa0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DUAL_STACK_LITE_TABLE_3 =
{
	64,
	{
		{ dump_RDD_DUAL_STACK_LITE_ENTRY, 0xfc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DS_PACKET_BUFFER_3 =
{
	512,
	{
		{ dump_RDD_PACKET_BUFFER, 0x1000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT PBIT_TO_QUEUE_TABLE_3 =
{
	8,
	{
		{ dump_RDD_PBIT_TO_QUEUE_8, 0x2000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RX_FLOW_CONTEXT_DDR_ADDR_3 =
{
	8,
	{
		{ dump_RDD_DEF_FLOW_CONTEXT_DDR_ADDR, 0x2208 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_RX_PSRAM_GET_NEXT_SCRATCHPAD_3 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2210 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT BRIDGE_CFG_TABLE_3 =
{
	8,
	{
		{ dump_RDD_BRIDGE_CFG, 0x2220 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT NULL_BUFFER_3 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0x2238 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DS_CPU_REASON_TO_METER_TABLE_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2240 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT POLICER_PARAMS_TABLE_3 =
{
	1,
	{
		{ dump_RDD_POLICER_PARAMS_ENTRY, 0x2280 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT SCRATCH_3 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x22d0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_L2_REASON_TABLE_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x22f0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_PROFILE_TABLE_3 =
{
	8,
	{
		{ dump_RDD_INGRESS_FILTER_CTRL, 0x2300 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_RX_COPY_PD_FIFO_TABLE_3 =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x2380 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT US_CPU_REASON_TO_METER_TABLE_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x23c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RUNNER_PROFILING_TRACE_BUFFER_3 =
{
	4,
	{
		{ dump_RDD_TRACE_EVENT, 0x2400 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT VPORT_CFG_TABLE_3 =
{
	8,
	{
		{ dump_RDD_VPORT_CFG_ENTRY, 0x2600 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT REGISTERS_BUFFER_3 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2740 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT LOOPBACK_QUEUE_TABLE_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x27c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR_3 =
{
	8,
	{
		{ dump_RDD_DDR_ADDRESS, 0x27e8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_RX_COPY_DISPATCHER_CREDIT_TABLE_3 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x27f0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_RX_INTERRUPT_SCRATCH_3 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x27fc },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_SRAM_PD_FIFO_3 =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x2800 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT NATC_TBL_CFG_3 =
{
	24,
	{
		{ dump_RDD_NATC_TBL_CONFIGURATION, 0x2900 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_RING_DESCRIPTOR_TABLE_3 =
{
	16,
	{
		{ dump_RDD_CPU_RING_DESCRIPTOR, 0x2930 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TCAM_IC_CFG_TABLE_3 =
{
	16,
	{
		{ dump_RDD_TCAM_IC_CFG, 0x2940 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_RING_INDEX_DDR_ADDR_TABLE_3 =
{
	8,
	{
		{ dump_RDD_DDR_ADDRESS, 0x29a0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_INTERRUPT_COALESCING_TABLE_3 =
{
	16,
	{
		{ dump_RDD_CPU_INTERRUPT_COALESCING_ENTRY, 0x29b0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT FLOW_BASED_ACTION_PTR_TABLE_3 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x29c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DIRECT_PROCESSING_WAN_VIQ_EXCLUSIVE_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x29e2 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT FORCE_IC_IP_FLOW_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x29e3 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TASK_IDX_3 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x29e4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_RX_INTERRUPT_ID_DDR_ADDR_3 =
{
	8,
	{
		{ dump_RDD_DDR_ADDRESS, 0x29e8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_FEED_RING_DESCRIPTOR_TABLE_3 =
{
	16,
	{
		{ dump_RDD_CPU_RING_DESCRIPTOR, 0x29f0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT VPORT_CFG_EX_TABLE_3 =
{
	1,
	{
		{ dump_RDD_VPORT_CFG_EX_ENTRY, 0x2a00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_RX_LOCAL_SCRATCH_3 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0x2a28 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT SYSTEM_CONFIGURATION_3 =
{
	5,
	{
		{ dump_RDD_SYSTEM_CONFIGURATION_ENTRY, 0x2a38 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT FORCE_DSCP_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2a3d },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CORE_ID_TABLE_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2a3e },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT SRAM_DUMMY_STORE_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2a3f },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RULE_BASED_ACTION_PTR_TABLE_3 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2a40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_FEED_RING_INTERRUPT_THRESHOLD_3 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2a62 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_FEED_RING_INTERRUPT_COUNTER_3 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2a64 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_FPM_LOW_THRESHOLD_3 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2a66 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_RING_INTERRUPT_COUNTER_TABLE_3 =
{
	8,
	{
		{ dump_RDD_CPU_RING_INTERRUPT_COUNTER_ENTRY, 0x2a68 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT FPM_GLOBAL_CFG_3 =
{
	12,
	{
		{ dump_RDD_FPM_GLOBAL_CFG, 0x2a70 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_FPM_HIGH_THRESHOLD_3 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2a7c },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RX_MIRRORING_CONFIGURATION_3 =
{
	2,
	{
		{ dump_RDD_MIRRORING_DESCRIPTOR, 0x2a7e },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_VPORT_TO_METER_TABLE_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2a80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_RX_COPY_LOCAL_SCRATCH_3 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0x2aa8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TUNNELS_PARSING_CFG_3 =
{
	8,
	{
		{ dump_RDD_TUNNELS_PARSING_CFG, 0x2ab0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IPTV_CFG_TABLE_3 =
{
	8,
	{
		{ dump_RDD_IPTV_CFG, 0x2ab8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IPTV_ACTION_PTR_TABLE_3 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2ac0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RATE_LIMIT_OVERHEAD_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2ae2 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT INGRESS_QOS_DONT_DROP_RATIO_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2ae3 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT INGRESS_QOS_WAN_UNTAGGED_PRIORITY_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2ae4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT MAC_TYPE_3 =
{
	1,
	{
		{ dump_RDD_MAC_TYPE_ENTRY, 0x2ae5 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IC_DEBUG_COUNTER_MODE_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2ae6 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IC_MCAST_ENABLE_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2ae7 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IPTV_DDR_CTX_TABLE_ADDRESS_3 =
{
	8,
	{
		{ dump_RDD_IPTV_DDR_CTX_TABLE_ADDRESS, 0x2ae8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT NAT_CACHE_CFG_3 =
{
	7,
	{
		{ dump_RDD_NAT_CACHE_CFG, 0x2af0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_REDIRECT_MODE_3 =
{
	1,
	{
		{ dump_RDD_CPU_REDIRECT_MODE_ENTRY, 0x2af7 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DIRECT_PROCESSING_EPON_CONTROL_SCRATCH_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2af8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IPTV_CLASSIFICATION_CFG_TABLE_3 =
{
	1,
	{
		{ dump_RDD_IPTV_CLASSIFICATION_CFG_ENTRY, 0x2af9 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_1588_CFG_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2afa },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT VLAN_ACTION_GPE_HANDLER_PTR_TABLE_3 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2b00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_CFG_3 =
{
	2,
	{
		{ dump_RDD_INGRESS_FILTER_CFG, 0x2b28 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT UPDATE_FIFO_TABLE_3 =
{
	4,
	{
		{ dump_RDD_UPDATE_FIFO_ENTRY, 0x2b40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_RX_COPY_UPDATE_FIFO_TABLE_3 =
{
	4,
	{
		{ dump_RDD_UPDATE_FIFO_ENTRY, 0x2b80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT PD_FIFO_TABLE_3 =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x2bc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_REASON_TO_TC_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2be0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TC_TO_CPU_RXQ_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2c20 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT EXC_TC_TO_CPU_RXQ_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2c60 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT VPORT_TO_CPU_OBJ_3 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2cc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_INTERRUPT_SCRATCH_3 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2d00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_SHADOW_RD_IDX_3 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2d80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_SHADOW_WR_IDX_3 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2e00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_RING_DESCRIPTORS_TABLE_3 =
{
	16,
	{
		{ dump_RDD_CPU_RING_DESCRIPTOR, 0x3000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RX_FLOW_TABLE_4 =
{
	2,
	{
		{ dump_RDD_RX_FLOW_ENTRY, 0x0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT LAYER2_HEADER_COPY_MAPPING_TABLE_4 =
{
	4,
	{
		{ dump_RDD_LAYER2_HEADER_COPY_MAPPING_ENTRY, 0x280 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IPTV_CONFIGURATION_TABLE_4 =
{
	8,
	{
		{ dump_RDD_HW_IPTV_CONFIGURATION, 0x2f8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DSCP_TO_PBITS_MAP_TABLE_4 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x300 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TC_TO_QUEUE_TABLE_4 =
{
	8,
	{
		{ dump_RDD_TC_TO_QUEUE_8, 0x400 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT GENERAL_QUEUE_DYNAMIC_MNG_TABLE_4 =
{
	8,
	{
		{ dump_RDD_QUEUE_DYNAMIC_MNG_ENTRY, 0x608 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT FPM_POOL_NUMBER_MAPPING_TABLE_4 =
{
	1,
	{
		{ dump_RDD_FPM_POOL_NUMBER, 0x610 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RUNNER_GLOBAL_REGISTERS_INIT_4 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x620 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT GLOBAL_DSCP_TO_PBITS_TABLE_4 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x640 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_RX_COMPLETE_FLOW_RING_BUFFER_4 =
{
	32,
	{
		{ dump_RDD_DHD_RX_COMPLETE_DESCRIPTOR, 0x680 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT VPORT_TO_DSCP_TO_PBITS_TABLE_4 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x6e0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_PROFILE_TABLE_4 =
{
	8,
	{
		{ dump_RDD_INGRESS_FILTER_CTRL, 0x700 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_RX_POST_FLOW_RING_BUFFER_4 =
{
	32,
	{
		{ dump_RDD_DHD_RX_POST_DESCRIPTOR, 0x780 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TCAM_IC_HANDLER_TABLE_4 =
{
	2,
	{
		{ dump_RDD_TCAM_IC_HANDLER, 0x7e0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT PBIT_TO_QUEUE_TABLE_4 =
{
	8,
	{
		{ dump_RDD_PBIT_TO_QUEUE_8, 0x800 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT PROCESSING_QUEUE_DYNAMIC_MNG_TABLE_4 =
{
	8,
	{
		{ dump_RDD_QUEUE_DYNAMIC_MNG_ENTRY, 0xa08 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_L2_REASON_TABLE_4 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xa10 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT BRIDGE_CFG_TABLE_4 =
{
	8,
	{
		{ dump_RDD_BRIDGE_CFG, 0xa20 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RX_FLOW_CONTEXT_DDR_ADDR_4 =
{
	8,
	{
		{ dump_RDD_DEF_FLOW_CONTEXT_DDR_ADDR, 0xa38 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT PBIT_TO_GEM_TABLE_4 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xa40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT POLICER_PARAMS_TABLE_4 =
{
	1,
	{
		{ dump_RDD_POLICER_PARAMS_ENTRY, 0xa80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_RX_COMPLETE_0_DISPATCHER_CREDIT_TABLE_4 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0xad0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TASK_IDX_4 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0xadc },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_RX_COMPLETE_1_DISPATCHER_CREDIT_TABLE_4 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0xae0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT FORCE_IC_IP_FLOW_4 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xaec },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT FORCE_DSCP_4 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xaed },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CORE_ID_TABLE_4 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xaee },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT SRAM_DUMMY_STORE_4 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xaef },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_RX_COMPLETE_2_DISPATCHER_CREDIT_TABLE_4 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0xaf0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_FPM_LOW_THRESHOLD_4 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0xafc },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_FPM_HIGH_THRESHOLD_4 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0xafe },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DUAL_STACK_LITE_TABLE_4 =
{
	64,
	{
		{ dump_RDD_DUAL_STACK_LITE_ENTRY, 0xb00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT REGISTERS_BUFFER_4 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0xb40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_TX_COMPLETE_FLOW_RING_BUFFER_4 =
{
	16,
	{
		{ dump_RDD_DHD_TX_COMPLETE_DESCRIPTOR, 0xbc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_DOORBELL_VALUE_4 =
{
	4,
	{
		{ dump_RDD_DHD_DOORBELL, 0xbf0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_RX_POST_RING_SIZE_4 =
{
	2,
	{
		{ dump_RDD_DHD_RING_SIZE, 0xbfc },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_RX_COMPLETE_RING_SIZE_4 =
{
	2,
	{
		{ dump_RDD_DHD_RING_SIZE, 0xbfe },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RUNNER_PROFILING_TRACE_BUFFER_4 =
{
	4,
	{
		{ dump_RDD_TRACE_EVENT, 0xc00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TCAM_IC_CMD_TABLE_4 =
{
	4,
	{
		{ dump_RDD_TCAM_IC_CMD, 0xe00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT LOOPBACK_QUEUE_TABLE_4 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xfc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT NULL_BUFFER_4 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0xfe8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_FEED_RING_DESCRIPTOR_TABLE_4 =
{
	16,
	{
		{ dump_RDD_CPU_RING_DESCRIPTOR, 0xff0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DS_PACKET_BUFFER_4 =
{
	512,
	{
		{ dump_RDD_PACKET_BUFFER, 0x1000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT LAYER2_GRE_TUNNEL_TABLE_4 =
{
	32,
	{
		{ dump_RDD_LAYER2_GRE_TUNNEL_ENTRY, 0x2000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_CPU_RECYCLE_RING_DESCRIPTOR_TABLE_4 =
{
	16,
	{
		{ dump_RDD_CPU_RING_DESCRIPTOR, 0x2180 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR_4 =
{
	8,
	{
		{ dump_RDD_DDR_ADDRESS, 0x21b0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT SYSTEM_CONFIGURATION_4 =
{
	5,
	{
		{ dump_RDD_SYSTEM_CONFIGURATION_ENTRY, 0x21b8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RATE_LIMIT_OVERHEAD_4 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x21bd },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_TX_COMPLETE_RING_SIZE_4 =
{
	2,
	{
		{ dump_RDD_DHD_RING_SIZE, 0x21be },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT FLOW_BASED_ACTION_PTR_TABLE_4 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x21c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RX_MIRRORING_CONFIGURATION_4 =
{
	2,
	{
		{ dump_RDD_MIRRORING_DESCRIPTOR, 0x21e2 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT INGRESS_QOS_DONT_DROP_RATIO_4 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x21e4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT INGRESS_QOS_WAN_UNTAGGED_PRIORITY_4 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x21e5 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IC_DEBUG_COUNTER_MODE_4 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x21e6 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IC_MCAST_ENABLE_4 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x21e7 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TUNNELS_PARSING_CFG_4 =
{
	8,
	{
		{ dump_RDD_TUNNELS_PARSING_CFG, 0x21e8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT FPM_GLOBAL_CFG_4 =
{
	12,
	{
		{ dump_RDD_FPM_GLOBAL_CFG, 0x21f0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_REDIRECT_MODE_4 =
{
	1,
	{
		{ dump_RDD_CPU_REDIRECT_MODE_ENTRY, 0x21fc },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IPTV_CLASSIFICATION_CFG_TABLE_4 =
{
	1,
	{
		{ dump_RDD_IPTV_CLASSIFICATION_CFG_ENTRY, 0x21fd },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_1588_CFG_4 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x21fe },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_COMPLETE_COMMON_RADIO_DATA_4 =
{
	112,
	{
		{ dump_RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY, 0x2200 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IPTV_CFG_TABLE_4 =
{
	8,
	{
		{ dump_RDD_IPTV_CFG, 0x2350 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IPTV_DDR_CTX_TABLE_ADDRESS_4 =
{
	8,
	{
		{ dump_RDD_IPTV_DDR_CTX_TABLE_ADDRESS, 0x2358 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TCAM_IC_CFG_TABLE_4 =
{
	16,
	{
		{ dump_RDD_TCAM_IC_CFG, 0x2360 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RULE_BASED_ACTION_PTR_TABLE_4 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x23c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT NAT_CACHE_CFG_4 =
{
	7,
	{
		{ dump_RDD_NAT_CACHE_CFG, 0x23e8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_CFG_4 =
{
	2,
	{
		{ dump_RDD_INGRESS_FILTER_CFG, 0x23f0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TX_FLOW_TABLE_4 =
{
	1,
	{
		{ dump_RDD_TX_FLOW_ENTRY, 0x2400 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IPTV_ACTION_PTR_TABLE_4 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2540 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT VPORT_CFG_EX_TABLE_4 =
{
	1,
	{
		{ dump_RDD_VPORT_CFG_EX_ENTRY, 0x2580 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT VLAN_ACTION_GPE_HANDLER_PTR_TABLE_4 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x25c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT VPORT_CFG_TABLE_4 =
{
	8,
	{
		{ dump_RDD_VPORT_CFG_ENTRY, 0x2600 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_FPM_REPLY_4 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2740 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_HW_CFG_4 =
{
	12,
	{
		{ dump_RDD_DHD_HW_CONFIGURATION, 0x2780 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT NATC_TBL_CFG_4 =
{
	24,
	{
		{ dump_RDD_NATC_TBL_CONFIGURATION, 0x2800 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RX_FLOW_TABLE_5 =
{
	2,
	{
		{ dump_RDD_RX_FLOW_ENTRY, 0x0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT LAYER2_HEADER_COPY_MAPPING_TABLE_5 =
{
	4,
	{
		{ dump_RDD_LAYER2_HEADER_COPY_MAPPING_ENTRY, 0x280 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IPTV_CONFIGURATION_TABLE_5 =
{
	8,
	{
		{ dump_RDD_HW_IPTV_CONFIGURATION, 0x2f8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DSCP_TO_PBITS_MAP_TABLE_5 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x300 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TC_TO_QUEUE_TABLE_5 =
{
	8,
	{
		{ dump_RDD_TC_TO_QUEUE_8, 0x400 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT GENERAL_QUEUE_DYNAMIC_MNG_TABLE_5 =
{
	8,
	{
		{ dump_RDD_QUEUE_DYNAMIC_MNG_ENTRY, 0x608 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT FPM_POOL_NUMBER_MAPPING_TABLE_5 =
{
	1,
	{
		{ dump_RDD_FPM_POOL_NUMBER, 0x610 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RUNNER_GLOBAL_REGISTERS_INIT_5 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x620 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT GLOBAL_DSCP_TO_PBITS_TABLE_5 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x640 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT POLICER_PARAMS_TABLE_5 =
{
	1,
	{
		{ dump_RDD_POLICER_PARAMS_ENTRY, 0x680 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_L2_REASON_TABLE_5 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x6d0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT VPORT_TO_DSCP_TO_PBITS_TABLE_5 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x6e0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_PROFILE_TABLE_5 =
{
	8,
	{
		{ dump_RDD_INGRESS_FILTER_CTRL, 0x700 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT PBIT_TO_GEM_TABLE_5 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x780 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DUAL_STACK_LITE_TABLE_5 =
{
	64,
	{
		{ dump_RDD_DUAL_STACK_LITE_ENTRY, 0x7c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT PBIT_TO_QUEUE_TABLE_5 =
{
	8,
	{
		{ dump_RDD_PBIT_TO_QUEUE_8, 0x800 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT REGISTERS_BUFFER_5 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0xa08 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT PROCESSING_QUEUE_DYNAMIC_MNG_TABLE_5 =
{
	8,
	{
		{ dump_RDD_QUEUE_DYNAMIC_MNG_ENTRY, 0xa88 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RX_FLOW_CONTEXT_DDR_ADDR_5 =
{
	8,
	{
		{ dump_RDD_DEF_FLOW_CONTEXT_DDR_ADDR, 0xa90 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT NULL_BUFFER_5 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0xa98 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TCAM_IC_CFG_TABLE_5 =
{
	16,
	{
		{ dump_RDD_TCAM_IC_CFG, 0xaa0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT NATC_TBL_CFG_5 =
{
	24,
	{
		{ dump_RDD_NATC_TBL_CONFIGURATION, 0xb00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_FEED_RING_DESCRIPTOR_TABLE_5 =
{
	16,
	{
		{ dump_RDD_CPU_RING_DESCRIPTOR, 0xb30 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT LOOPBACK_QUEUE_TABLE_5 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xb40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT SYSTEM_CONFIGURATION_5 =
{
	5,
	{
		{ dump_RDD_SYSTEM_CONFIGURATION_ENTRY, 0xb68 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT FORCE_IC_IP_FLOW_5 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xb6d },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT FORCE_DSCP_5 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xb6e },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CORE_ID_TABLE_5 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xb6f },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT FPM_GLOBAL_CFG_5 =
{
	12,
	{
		{ dump_RDD_FPM_GLOBAL_CFG, 0xb70 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TASK_IDX_5 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0xb7c },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT VPORT_CFG_EX_TABLE_5 =
{
	1,
	{
		{ dump_RDD_VPORT_CFG_EX_ENTRY, 0xb80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TUNNELS_PARSING_CFG_5 =
{
	8,
	{
		{ dump_RDD_TUNNELS_PARSING_CFG, 0xba8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IPTV_CFG_TABLE_5 =
{
	8,
	{
		{ dump_RDD_IPTV_CFG, 0xbb0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IPTV_DDR_CTX_TABLE_ADDRESS_5 =
{
	8,
	{
		{ dump_RDD_IPTV_DDR_CTX_TABLE_ADDRESS, 0xbb8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT FLOW_BASED_ACTION_PTR_TABLE_5 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0xbc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_FPM_LOW_THRESHOLD_5 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0xbe2 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_FPM_HIGH_THRESHOLD_5 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0xbe4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RX_MIRRORING_CONFIGURATION_5 =
{
	2,
	{
		{ dump_RDD_MIRRORING_DESCRIPTOR, 0xbe6 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT NAT_CACHE_CFG_5 =
{
	7,
	{
		{ dump_RDD_NAT_CACHE_CFG, 0xbe8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT SRAM_DUMMY_STORE_5 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xbef },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_CFG_5 =
{
	2,
	{
		{ dump_RDD_INGRESS_FILTER_CFG, 0xbf0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RATE_LIMIT_OVERHEAD_5 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xbf2 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT INGRESS_QOS_DONT_DROP_RATIO_5 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xbf3 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT INGRESS_QOS_WAN_UNTAGGED_PRIORITY_5 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xbf4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IC_DEBUG_COUNTER_MODE_5 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xbf5 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IC_MCAST_ENABLE_5 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xbf6 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_REDIRECT_MODE_5 =
{
	1,
	{
		{ dump_RDD_CPU_REDIRECT_MODE_ENTRY, 0xbf7 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IPTV_CLASSIFICATION_CFG_TABLE_5 =
{
	1,
	{
		{ dump_RDD_IPTV_CLASSIFICATION_CFG_ENTRY, 0xbf8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_1588_CFG_5 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xbf9 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RUNNER_PROFILING_TRACE_BUFFER_5 =
{
	4,
	{
		{ dump_RDD_TRACE_EVENT, 0xc00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TCAM_IC_CMD_TABLE_5 =
{
	4,
	{
		{ dump_RDD_TCAM_IC_CMD, 0xe00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RULE_BASED_ACTION_PTR_TABLE_5 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0xfc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DS_PACKET_BUFFER_5 =
{
	512,
	{
		{ dump_RDD_PACKET_BUFFER, 0x1000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT LAYER2_GRE_TUNNEL_TABLE_5 =
{
	32,
	{
		{ dump_RDD_LAYER2_GRE_TUNNEL_ENTRY, 0x2000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IPTV_ACTION_PTR_TABLE_5 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2180 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT VLAN_ACTION_GPE_HANDLER_PTR_TABLE_5 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x21c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TX_FLOW_TABLE_5 =
{
	1,
	{
		{ dump_RDD_TX_FLOW_ENTRY, 0x2200 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TCAM_IC_HANDLER_TABLE_5 =
{
	2,
	{
		{ dump_RDD_TCAM_IC_HANDLER, 0x2340 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT BRIDGE_CFG_TABLE_5 =
{
	8,
	{
		{ dump_RDD_BRIDGE_CFG, 0x2360 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT VPORT_CFG_TABLE_5 =
{
	8,
	{
		{ dump_RDD_VPORT_CFG_ENTRY, 0x2400 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RX_FLOW_TABLE_6 =
{
	2,
	{
		{ dump_RDD_RX_FLOW_ENTRY, 0x0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT LAYER2_HEADER_COPY_MAPPING_TABLE_6 =
{
	4,
	{
		{ dump_RDD_LAYER2_HEADER_COPY_MAPPING_ENTRY, 0x280 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IPTV_CONFIGURATION_TABLE_6 =
{
	8,
	{
		{ dump_RDD_HW_IPTV_CONFIGURATION, 0x2f8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DSCP_TO_PBITS_MAP_TABLE_6 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x300 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TC_TO_QUEUE_TABLE_6 =
{
	8,
	{
		{ dump_RDD_TC_TO_QUEUE_8, 0x400 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT GENERAL_QUEUE_DYNAMIC_MNG_TABLE_6 =
{
	8,
	{
		{ dump_RDD_QUEUE_DYNAMIC_MNG_ENTRY, 0x608 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT FPM_POOL_NUMBER_MAPPING_TABLE_6 =
{
	1,
	{
		{ dump_RDD_FPM_POOL_NUMBER, 0x610 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RUNNER_GLOBAL_REGISTERS_INIT_6 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x620 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT GLOBAL_DSCP_TO_PBITS_TABLE_6 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x640 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT POLICER_PARAMS_TABLE_6 =
{
	1,
	{
		{ dump_RDD_POLICER_PARAMS_ENTRY, 0x680 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_L2_REASON_TABLE_6 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x6d0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT VPORT_TO_DSCP_TO_PBITS_TABLE_6 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x6e0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_PROFILE_TABLE_6 =
{
	8,
	{
		{ dump_RDD_INGRESS_FILTER_CTRL, 0x700 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT PBIT_TO_GEM_TABLE_6 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x780 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DUAL_STACK_LITE_TABLE_6 =
{
	64,
	{
		{ dump_RDD_DUAL_STACK_LITE_ENTRY, 0x7c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT PBIT_TO_QUEUE_TABLE_6 =
{
	8,
	{
		{ dump_RDD_PBIT_TO_QUEUE_8, 0x800 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT REGISTERS_BUFFER_6 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0xa08 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT PROCESSING_QUEUE_DYNAMIC_MNG_TABLE_6 =
{
	8,
	{
		{ dump_RDD_QUEUE_DYNAMIC_MNG_ENTRY, 0xa88 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RX_FLOW_CONTEXT_DDR_ADDR_6 =
{
	8,
	{
		{ dump_RDD_DEF_FLOW_CONTEXT_DDR_ADDR, 0xa90 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT NULL_BUFFER_6 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0xa98 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TCAM_IC_CFG_TABLE_6 =
{
	16,
	{
		{ dump_RDD_TCAM_IC_CFG, 0xaa0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT NATC_TBL_CFG_6 =
{
	24,
	{
		{ dump_RDD_NATC_TBL_CONFIGURATION, 0xb00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_FEED_RING_DESCRIPTOR_TABLE_6 =
{
	16,
	{
		{ dump_RDD_CPU_RING_DESCRIPTOR, 0xb30 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT LOOPBACK_QUEUE_TABLE_6 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xb40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT SYSTEM_CONFIGURATION_6 =
{
	5,
	{
		{ dump_RDD_SYSTEM_CONFIGURATION_ENTRY, 0xb68 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT FORCE_IC_IP_FLOW_6 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xb6d },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT FORCE_DSCP_6 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xb6e },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CORE_ID_TABLE_6 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xb6f },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT FPM_GLOBAL_CFG_6 =
{
	12,
	{
		{ dump_RDD_FPM_GLOBAL_CFG, 0xb70 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TASK_IDX_6 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0xb7c },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT VPORT_CFG_EX_TABLE_6 =
{
	1,
	{
		{ dump_RDD_VPORT_CFG_EX_ENTRY, 0xb80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TUNNELS_PARSING_CFG_6 =
{
	8,
	{
		{ dump_RDD_TUNNELS_PARSING_CFG, 0xba8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IPTV_CFG_TABLE_6 =
{
	8,
	{
		{ dump_RDD_IPTV_CFG, 0xbb0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IPTV_DDR_CTX_TABLE_ADDRESS_6 =
{
	8,
	{
		{ dump_RDD_IPTV_DDR_CTX_TABLE_ADDRESS, 0xbb8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT FLOW_BASED_ACTION_PTR_TABLE_6 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0xbc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_FPM_LOW_THRESHOLD_6 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0xbe2 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_FPM_HIGH_THRESHOLD_6 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0xbe4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RX_MIRRORING_CONFIGURATION_6 =
{
	2,
	{
		{ dump_RDD_MIRRORING_DESCRIPTOR, 0xbe6 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT NAT_CACHE_CFG_6 =
{
	7,
	{
		{ dump_RDD_NAT_CACHE_CFG, 0xbe8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT SRAM_DUMMY_STORE_6 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xbef },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_CFG_6 =
{
	2,
	{
		{ dump_RDD_INGRESS_FILTER_CFG, 0xbf0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RATE_LIMIT_OVERHEAD_6 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xbf2 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT INGRESS_QOS_DONT_DROP_RATIO_6 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xbf3 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT INGRESS_QOS_WAN_UNTAGGED_PRIORITY_6 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xbf4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IC_DEBUG_COUNTER_MODE_6 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xbf5 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IC_MCAST_ENABLE_6 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xbf6 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_REDIRECT_MODE_6 =
{
	1,
	{
		{ dump_RDD_CPU_REDIRECT_MODE_ENTRY, 0xbf7 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IPTV_CLASSIFICATION_CFG_TABLE_6 =
{
	1,
	{
		{ dump_RDD_IPTV_CLASSIFICATION_CFG_ENTRY, 0xbf8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_1588_CFG_6 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xbf9 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RUNNER_PROFILING_TRACE_BUFFER_6 =
{
	4,
	{
		{ dump_RDD_TRACE_EVENT, 0xc00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TCAM_IC_CMD_TABLE_6 =
{
	4,
	{
		{ dump_RDD_TCAM_IC_CMD, 0xe00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RULE_BASED_ACTION_PTR_TABLE_6 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0xfc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DS_PACKET_BUFFER_6 =
{
	512,
	{
		{ dump_RDD_PACKET_BUFFER, 0x1000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT LAYER2_GRE_TUNNEL_TABLE_6 =
{
	32,
	{
		{ dump_RDD_LAYER2_GRE_TUNNEL_ENTRY, 0x2000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IPTV_ACTION_PTR_TABLE_6 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2180 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT VLAN_ACTION_GPE_HANDLER_PTR_TABLE_6 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x21c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TX_FLOW_TABLE_6 =
{
	1,
	{
		{ dump_RDD_TX_FLOW_ENTRY, 0x2200 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TCAM_IC_HANDLER_TABLE_6 =
{
	2,
	{
		{ dump_RDD_TCAM_IC_HANDLER, 0x2340 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT BRIDGE_CFG_TABLE_6 =
{
	8,
	{
		{ dump_RDD_BRIDGE_CFG, 0x2360 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT VPORT_CFG_TABLE_6 =
{
	8,
	{
		{ dump_RDD_VPORT_CFG_ENTRY, 0x2400 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RX_FLOW_TABLE_7 =
{
	2,
	{
		{ dump_RDD_RX_FLOW_ENTRY, 0x0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT LAYER2_HEADER_COPY_MAPPING_TABLE_7 =
{
	4,
	{
		{ dump_RDD_LAYER2_HEADER_COPY_MAPPING_ENTRY, 0x280 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IPTV_CONFIGURATION_TABLE_7 =
{
	8,
	{
		{ dump_RDD_HW_IPTV_CONFIGURATION, 0x2f8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DSCP_TO_PBITS_MAP_TABLE_7 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x300 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TC_TO_QUEUE_TABLE_7 =
{
	8,
	{
		{ dump_RDD_TC_TO_QUEUE_8, 0x400 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT GENERAL_QUEUE_DYNAMIC_MNG_TABLE_7 =
{
	8,
	{
		{ dump_RDD_QUEUE_DYNAMIC_MNG_ENTRY, 0x608 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT FPM_POOL_NUMBER_MAPPING_TABLE_7 =
{
	1,
	{
		{ dump_RDD_FPM_POOL_NUMBER, 0x610 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RUNNER_GLOBAL_REGISTERS_INIT_7 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x620 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT GLOBAL_DSCP_TO_PBITS_TABLE_7 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x640 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT POLICER_PARAMS_TABLE_7 =
{
	1,
	{
		{ dump_RDD_POLICER_PARAMS_ENTRY, 0x680 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_L2_REASON_TABLE_7 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x6d0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT VPORT_TO_DSCP_TO_PBITS_TABLE_7 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x6e0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_PROFILE_TABLE_7 =
{
	8,
	{
		{ dump_RDD_INGRESS_FILTER_CTRL, 0x700 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT PBIT_TO_GEM_TABLE_7 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x780 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DUAL_STACK_LITE_TABLE_7 =
{
	64,
	{
		{ dump_RDD_DUAL_STACK_LITE_ENTRY, 0x7c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT PBIT_TO_QUEUE_TABLE_7 =
{
	8,
	{
		{ dump_RDD_PBIT_TO_QUEUE_8, 0x800 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT REGISTERS_BUFFER_7 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0xa08 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT PROCESSING_QUEUE_DYNAMIC_MNG_TABLE_7 =
{
	8,
	{
		{ dump_RDD_QUEUE_DYNAMIC_MNG_ENTRY, 0xa88 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RX_FLOW_CONTEXT_DDR_ADDR_7 =
{
	8,
	{
		{ dump_RDD_DEF_FLOW_CONTEXT_DDR_ADDR, 0xa90 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT NULL_BUFFER_7 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0xa98 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TCAM_IC_CFG_TABLE_7 =
{
	16,
	{
		{ dump_RDD_TCAM_IC_CFG, 0xaa0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT NATC_TBL_CFG_7 =
{
	24,
	{
		{ dump_RDD_NATC_TBL_CONFIGURATION, 0xb00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_FEED_RING_DESCRIPTOR_TABLE_7 =
{
	16,
	{
		{ dump_RDD_CPU_RING_DESCRIPTOR, 0xb30 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT LOOPBACK_QUEUE_TABLE_7 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xb40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT SYSTEM_CONFIGURATION_7 =
{
	5,
	{
		{ dump_RDD_SYSTEM_CONFIGURATION_ENTRY, 0xb68 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT FORCE_IC_IP_FLOW_7 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xb6d },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT FORCE_DSCP_7 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xb6e },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CORE_ID_TABLE_7 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xb6f },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT FPM_GLOBAL_CFG_7 =
{
	12,
	{
		{ dump_RDD_FPM_GLOBAL_CFG, 0xb70 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TASK_IDX_7 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0xb7c },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT VPORT_CFG_EX_TABLE_7 =
{
	1,
	{
		{ dump_RDD_VPORT_CFG_EX_ENTRY, 0xb80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TUNNELS_PARSING_CFG_7 =
{
	8,
	{
		{ dump_RDD_TUNNELS_PARSING_CFG, 0xba8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IPTV_CFG_TABLE_7 =
{
	8,
	{
		{ dump_RDD_IPTV_CFG, 0xbb0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IPTV_DDR_CTX_TABLE_ADDRESS_7 =
{
	8,
	{
		{ dump_RDD_IPTV_DDR_CTX_TABLE_ADDRESS, 0xbb8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT FLOW_BASED_ACTION_PTR_TABLE_7 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0xbc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_FPM_LOW_THRESHOLD_7 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0xbe2 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DHD_FPM_HIGH_THRESHOLD_7 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0xbe4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RX_MIRRORING_CONFIGURATION_7 =
{
	2,
	{
		{ dump_RDD_MIRRORING_DESCRIPTOR, 0xbe6 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT NAT_CACHE_CFG_7 =
{
	7,
	{
		{ dump_RDD_NAT_CACHE_CFG, 0xbe8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT SRAM_DUMMY_STORE_7 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xbef },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_CFG_7 =
{
	2,
	{
		{ dump_RDD_INGRESS_FILTER_CFG, 0xbf0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RATE_LIMIT_OVERHEAD_7 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xbf2 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT INGRESS_QOS_DONT_DROP_RATIO_7 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xbf3 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT INGRESS_QOS_WAN_UNTAGGED_PRIORITY_7 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xbf4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IC_DEBUG_COUNTER_MODE_7 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xbf5 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IC_MCAST_ENABLE_7 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xbf6 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT CPU_REDIRECT_MODE_7 =
{
	1,
	{
		{ dump_RDD_CPU_REDIRECT_MODE_ENTRY, 0xbf7 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IPTV_CLASSIFICATION_CFG_TABLE_7 =
{
	1,
	{
		{ dump_RDD_IPTV_CLASSIFICATION_CFG_ENTRY, 0xbf8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_1588_CFG_7 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xbf9 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RUNNER_PROFILING_TRACE_BUFFER_7 =
{
	4,
	{
		{ dump_RDD_TRACE_EVENT, 0xc00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TCAM_IC_CMD_TABLE_7 =
{
	4,
	{
		{ dump_RDD_TCAM_IC_CMD, 0xe00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT RULE_BASED_ACTION_PTR_TABLE_7 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0xfc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT DS_PACKET_BUFFER_7 =
{
	512,
	{
		{ dump_RDD_PACKET_BUFFER, 0x1000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT LAYER2_GRE_TUNNEL_TABLE_7 =
{
	32,
	{
		{ dump_RDD_LAYER2_GRE_TUNNEL_ENTRY, 0x2000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT IPTV_ACTION_PTR_TABLE_7 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2180 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT VLAN_ACTION_GPE_HANDLER_PTR_TABLE_7 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x21c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TX_FLOW_TABLE_7 =
{
	1,
	{
		{ dump_RDD_TX_FLOW_ENTRY, 0x2200 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT TCAM_IC_HANDLER_TABLE_7 =
{
	2,
	{
		{ dump_RDD_TCAM_IC_HANDLER, 0x2340 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT BRIDGE_CFG_TABLE_7 =
{
	8,
	{
		{ dump_RDD_BRIDGE_CFG, 0x2360 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6856
static DUMP_RUNNERREG_STRUCT VPORT_CFG_TABLE_7 =
{
	8,
	{
		{ dump_RDD_VPORT_CFG_ENTRY, 0x2400 },
		{ 0, 0 },
	}
};
#endif

TABLE_STRUCT RUNNER_TABLES[NUMBER_OF_TABLES] =
{
#if defined BCM6856
	{ "DS_TM_PD_FIFO_TABLE", 1, CORE_0_INDEX, &DS_TM_PD_FIFO_TABLE, 320, 1, 1 },
#endif
#if defined BCM6856
	{ "COMPLEX_SCHEDULER_TABLE", 1, CORE_0_INDEX, &COMPLEX_SCHEDULER_TABLE, 16, 1, 1 },
#endif
#if defined BCM6856
	{ "DS_TM_SCHEDULING_QUEUE_TABLE", 1, CORE_0_INDEX, &DS_TM_SCHEDULING_QUEUE_TABLE, 160, 1, 1 },
#endif
#if defined BCM6856
	{ "REPORTING_COUNTER_TABLE", 1, CORE_0_INDEX, &REPORTING_COUNTER_TABLE, 40, 1, 1 },
#endif
#if defined BCM6856
	{ "RUNNER_GLOBAL_REGISTERS_INIT", 1, CORE_0_INDEX, &RUNNER_GLOBAL_REGISTERS_INIT, 8, 1, 1 },
#endif
#if defined BCM6856
	{ "DS_TM_TM_FLOW_CNTR_TABLE", 1, CORE_0_INDEX, &DS_TM_TM_FLOW_CNTR_TABLE, 64, 1, 1 },
#endif
#if defined BCM6856
	{ "BASIC_SCHEDULER_TABLE_DS", 1, CORE_0_INDEX, &BASIC_SCHEDULER_TABLE_DS, 32, 1, 1 },
#endif
#if defined BCM6856
	{ "REPORTING_QUEUE_DESCRIPTOR_TABLE", 1, CORE_0_INDEX, &REPORTING_QUEUE_DESCRIPTOR_TABLE, 160, 1, 1 },
#endif
#if defined BCM6856
	{ "REPORTING_QUEUE_ACCUMULATED_TABLE", 1, CORE_0_INDEX, &REPORTING_QUEUE_ACCUMULATED_TABLE, 16, 1, 1 },
#endif
#if defined BCM6856
	{ "DS_TM_BBH_TX_EGRESS_COUNTER_TABLE", 1, CORE_0_INDEX, &DS_TM_BBH_TX_EGRESS_COUNTER_TABLE, 8, 1, 1 },
#endif
#if defined BCM6856
	{ "DS_TM_CPU_TX_ABS_COUNTERS", 1, CORE_0_INDEX, &DS_TM_CPU_TX_ABS_COUNTERS, 32, 1, 1 },
#endif
#if defined BCM6856
	{ "REGISTERS_BUFFER", 1, CORE_0_INDEX, &REGISTERS_BUFFER, 32, 1, 1 },
#endif
#if defined BCM6856
	{ "DS_TM_TM_ACTION_PTR_TABLE", 1, CORE_0_INDEX, &DS_TM_TM_ACTION_PTR_TABLE, 17, 1, 1 },
#endif
#if defined BCM6856
	{ "BUDGET_ALLOCATION_TIMER_VALUE", 1, CORE_0_INDEX, &BUDGET_ALLOCATION_TIMER_VALUE, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "GHOST_REPORTING_GLOBAL_CFG", 1, CORE_0_INDEX, &GHOST_REPORTING_GLOBAL_CFG, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "DS_TM_BBH_TX_QUEUE_ID_TABLE", 1, CORE_0_INDEX, &DS_TM_BBH_TX_QUEUE_ID_TABLE, 2, 1, 1 },
#endif
#if defined BCM6856
	{ "XGPON_REPORT_TABLE", 1, CORE_0_INDEX, &XGPON_REPORT_TABLE, 4, 1, 1 },
#endif
#if defined BCM6856
	{ "RUNNER_PROFILING_TRACE_BUFFER", 1, CORE_0_INDEX, &RUNNER_PROFILING_TRACE_BUFFER, 128, 1, 1 },
#endif
#if defined BCM6856
	{ "NATC_TBL_CFG", 1, CORE_0_INDEX, &NATC_TBL_CFG, 2, 1, 1 },
#endif
#if defined BCM6856
	{ "DS_TM_FLUSH_DISPATCHER_CREDIT_TABLE", 1, CORE_0_INDEX, &DS_TM_FLUSH_DISPATCHER_CREDIT_TABLE, 3, 1, 1 },
#endif
#if defined BCM6856
	{ "DS_TM_BB_DESTINATION_TABLE", 1, CORE_0_INDEX, &DS_TM_BB_DESTINATION_TABLE, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "DS_TM_SCHEDULING_GLOBAL_FLUSH_CFG", 1, CORE_0_INDEX, &DS_TM_SCHEDULING_GLOBAL_FLUSH_CFG, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "SCHEDULING_FLUSH_GLOBAL_CFG", 1, CORE_0_INDEX, &SCHEDULING_FLUSH_GLOBAL_CFG, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "UPDATE_FIFO_TABLE", 1, CORE_0_INDEX, &UPDATE_FIFO_TABLE, 8, 1, 1 },
#endif
#if defined BCM6856
	{ "DS_TM_SCHEDULING_FLUSH_VECTOR", 1, CORE_0_INDEX, &DS_TM_SCHEDULING_FLUSH_VECTOR, 5, 1, 1 },
#endif
#if defined BCM6856
	{ "TASK_IDX", 1, CORE_0_INDEX, &TASK_IDX, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "GENERAL_QUEUE_DYNAMIC_MNG_TABLE", 1, CORE_0_INDEX, &GENERAL_QUEUE_DYNAMIC_MNG_TABLE, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "DS_TM_SCHEDULING_QUEUE_AGING_VECTOR", 1, CORE_0_INDEX, &DS_TM_SCHEDULING_QUEUE_AGING_VECTOR, 5, 1, 1 },
#endif
#if defined BCM6856
	{ "TX_MIRRORING_CONFIGURATION", 1, CORE_0_INDEX, &TX_MIRRORING_CONFIGURATION, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "SRAM_DUMMY_STORE", 1, CORE_0_INDEX, &SRAM_DUMMY_STORE, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "RATE_LIMIT_OVERHEAD", 1, CORE_0_INDEX, &RATE_LIMIT_OVERHEAD, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "MIRRORING_SCRATCH", 1, CORE_0_INDEX, &MIRRORING_SCRATCH, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "DS_TM_SCHEDULING_AGGREGATION_CONTEXT_VECTOR", 1, CORE_0_INDEX, &DS_TM_SCHEDULING_AGGREGATION_CONTEXT_VECTOR, 5, 1, 1 },
#endif
#if defined BCM6856
	{ "DS_TM_FIRST_QUEUE_MAPPING", 1, CORE_0_INDEX, &DS_TM_FIRST_QUEUE_MAPPING, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "BBH_TX_INGRESS_COUNTER_TABLE", 1, CORE_0_INDEX, &BBH_TX_INGRESS_COUNTER_TABLE, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "REPORT_BBH_TX_QUEUE_ID_TABLE", 1, CORE_0_INDEX, &REPORT_BBH_TX_QUEUE_ID_TABLE, 2, 1, 1 },
#endif
#if defined BCM6856
	{ "GHOST_REPORTING_QUEUE_STATUS_BIT_VECTOR_TABLE", 1, CORE_0_INDEX, &GHOST_REPORTING_QUEUE_STATUS_BIT_VECTOR_TABLE, 5, 1, 1 },
#endif
#if defined BCM6856
	{ "BBH_TX_EGRESS_REPORT_COUNTER_TABLE", 1, CORE_0_INDEX, &BBH_TX_EGRESS_REPORT_COUNTER_TABLE, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "RATE_LIMITER_VALID_TABLE_DS", 1, CORE_0_INDEX, &RATE_LIMITER_VALID_TABLE_DS, 4, 1, 1 },
#endif
#if defined BCM6856
	{ "TX_MIRRORING_DISPATCHER_CREDIT_TABLE", 1, CORE_0_INDEX, &TX_MIRRORING_DISPATCHER_CREDIT_TABLE, 3, 1, 1 },
#endif
#if defined BCM6856
	{ "FPM_GLOBAL_CFG", 1, CORE_0_INDEX, &FPM_GLOBAL_CFG, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "XGPON_REPORT_ZERO_SENT_TABLE", 1, CORE_0_INDEX, &XGPON_REPORT_ZERO_SENT_TABLE, 10, 1, 1 },
#endif
#if defined BCM6856
	{ "BASIC_RATE_LIMITER_TABLE_DS", 1, CORE_0_INDEX, &BASIC_RATE_LIMITER_TABLE_DS, 128, 1, 1 },
#endif
#if defined BCM6856
	{ "DS_TM_BBH_QUEUE_TABLE", 1, CORE_0_INDEX, &DS_TM_BBH_QUEUE_TABLE, 8, 1, 1 },
#endif
#if defined BCM6856
	{ "US_TM_PD_FIFO_TABLE", 1, CORE_1_INDEX, &US_TM_PD_FIFO_TABLE_1, 320, 1, 1 },
#endif
#if defined BCM6856
	{ "COMPLEX_SCHEDULER_TABLE", 1, CORE_1_INDEX, &COMPLEX_SCHEDULER_TABLE_1, 16, 1, 1 },
#endif
#if defined BCM6856
	{ "US_TM_SCHEDULING_QUEUE_TABLE", 1, CORE_1_INDEX, &US_TM_SCHEDULING_QUEUE_TABLE_1, 160, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_POST_COMMON_RADIO_DATA", 1, CORE_1_INDEX, &DHD_POST_COMMON_RADIO_DATA_1, 3, 1, 1 },
#endif
#if defined BCM6856
	{ "US_TM_BBH_TX_QUEUE_ID_TABLE", 1, CORE_1_INDEX, &US_TM_BBH_TX_QUEUE_ID_TABLE_1, 2, 1, 1 },
#endif
#if defined BCM6856
	{ "RUNNER_GLOBAL_REGISTERS_INIT", 1, CORE_1_INDEX, &RUNNER_GLOBAL_REGISTERS_INIT_1, 8, 1, 1 },
#endif
#if defined BCM6856
	{ "BASIC_SCHEDULER_TABLE_US", 1, CORE_1_INDEX, &BASIC_SCHEDULER_TABLE_US_1, 32, 1, 1 },
#endif
#if defined BCM6856
	{ "BASIC_RATE_LIMITER_TABLE_US", 1, CORE_1_INDEX, &BASIC_RATE_LIMITER_TABLE_US_1, 128, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_FLOW_RING_CACHE_CTX_TABLE", 1, CORE_1_INDEX, &DHD_FLOW_RING_CACHE_CTX_TABLE_1, 48, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_TX_POST_FLOW_RING_BUFFER", 1, CORE_1_INDEX, &DHD_TX_POST_FLOW_RING_BUFFER_1, 3, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_FPM_POOL_NUMBER_MAPPING_TABLE", 1, CORE_1_INDEX, &DHD_FPM_POOL_NUMBER_MAPPING_TABLE_1, 16, 1, 1 },
#endif
#if defined BCM6856
	{ "US_TM_SCHEDULING_FLUSH_VECTOR", 1, CORE_1_INDEX, &US_TM_SCHEDULING_FLUSH_VECTOR_1, 5, 1, 1 },
#endif
#if defined BCM6856
	{ "BUDGET_ALLOCATION_TIMER_VALUE", 1, CORE_1_INDEX, &BUDGET_ALLOCATION_TIMER_VALUE_1, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "US_TM_BB_DESTINATION_TABLE", 1, CORE_1_INDEX, &US_TM_BB_DESTINATION_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "BBH_TX_EPON_QUEUE_ID_TABLE", 1, CORE_1_INDEX, &BBH_TX_EPON_QUEUE_ID_TABLE_1, 2, 1, 1 },
#endif
#if defined BCM6856
	{ "US_TM_BBH_TX_EGRESS_COUNTER_TABLE", 1, CORE_1_INDEX, &US_TM_BBH_TX_EGRESS_COUNTER_TABLE_1, 8, 1, 1 },
#endif
#if defined BCM6856
	{ "RUNNER_PROFILING_TRACE_BUFFER", 1, CORE_1_INDEX, &RUNNER_PROFILING_TRACE_BUFFER_1, 128, 1, 1 },
#endif
#if defined BCM6856
	{ "WLAN_MCAST_SSID_MAC_ADDRESS_TABLE", 1, CORE_1_INDEX, &WLAN_MCAST_SSID_MAC_ADDRESS_TABLE_1, 48, 1, 1 },
#endif
#if defined BCM6856
	{ "US_TM_TM_FLOW_CNTR_TABLE", 1, CORE_1_INDEX, &US_TM_TM_FLOW_CNTR_TABLE_1, 128, 1, 1 },
#endif
#if defined BCM6856
	{ "WLAN_MCAST_DHD_STATION_TABLE", 1, CORE_1_INDEX, &WLAN_MCAST_DHD_STATION_TABLE_1, 64, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_TX_POST_PD_FIFO_TABLE", 1, CORE_1_INDEX, &DHD_TX_POST_PD_FIFO_TABLE_1, 12, 1, 1 },
#endif
#if defined BCM6856
	{ "WLAN_MCAST_DFT_LIST_ENTRY_SCRATCH", 1, CORE_1_INDEX, &WLAN_MCAST_DFT_LIST_ENTRY_SCRATCH_1, 64, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_FLOW_RING_CACHE_LKP_TABLE", 1, CORE_1_INDEX, &DHD_FLOW_RING_CACHE_LKP_TABLE_1, 48, 1, 1 },
#endif
#if defined BCM6856
	{ "US_TM_SCHEDULING_QUEUE_AGING_VECTOR", 1, CORE_1_INDEX, &US_TM_SCHEDULING_QUEUE_AGING_VECTOR_1, 5, 1, 1 },
#endif
#if defined BCM6856
	{ "TASK_IDX", 1, CORE_1_INDEX, &TASK_IDX_1, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "GENERAL_QUEUE_DYNAMIC_MNG_TABLE", 1, CORE_1_INDEX, &GENERAL_QUEUE_DYNAMIC_MNG_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_L2_HEADER", 1, CORE_1_INDEX, &DHD_L2_HEADER_1, 72, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR", 1, CORE_1_INDEX, &DHD_CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR_1, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "OVERALL_RATE_LIMITER_TABLE", 1, CORE_1_INDEX, &OVERALL_RATE_LIMITER_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "US_TM_SCHEDULING_AGGREGATION_CONTEXT_VECTOR", 1, CORE_1_INDEX, &US_TM_SCHEDULING_AGGREGATION_CONTEXT_VECTOR_1, 5, 1, 1 },
#endif
#if defined BCM6856
	{ "US_TM_SCHEDULING_GLOBAL_FLUSH_CFG", 1, CORE_1_INDEX, &US_TM_SCHEDULING_GLOBAL_FLUSH_CFG_1, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "SCHEDULING_FLUSH_GLOBAL_CFG", 1, CORE_1_INDEX, &SCHEDULING_FLUSH_GLOBAL_CFG_1, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "TX_MIRRORING_CONFIGURATION", 1, CORE_1_INDEX, &TX_MIRRORING_CONFIGURATION_1, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "WLAN_MCAST_DFT_ADDR", 1, CORE_1_INDEX, &WLAN_MCAST_DFT_ADDR_1, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "US_TM_BBH_QUEUE_TABLE", 1, CORE_1_INDEX, &US_TM_BBH_QUEUE_TABLE_1, 40, 1, 1 },
#endif
#if defined BCM6856
	{ "US_TM_CPU_TX_ABS_COUNTERS", 1, CORE_1_INDEX, &US_TM_CPU_TX_ABS_COUNTERS_1, 32, 1, 1 },
#endif
#if defined BCM6856
	{ "REGISTERS_BUFFER", 1, CORE_1_INDEX, &REGISTERS_BUFFER_1, 32, 1, 1 },
#endif
#if defined BCM6856
	{ "RATE_LIMITER_VALID_TABLE_US", 1, CORE_1_INDEX, &RATE_LIMITER_VALID_TABLE_US_1, 4, 1, 1 },
#endif
#if defined BCM6856
	{ "US_TM_FLUSH_DISPATCHER_CREDIT_TABLE", 1, CORE_1_INDEX, &US_TM_FLUSH_DISPATCHER_CREDIT_TABLE_1, 3, 1, 1 },
#endif
#if defined BCM6856
	{ "SRAM_DUMMY_STORE", 1, CORE_1_INDEX, &SRAM_DUMMY_STORE_1, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "RATE_LIMIT_OVERHEAD", 1, CORE_1_INDEX, &RATE_LIMIT_OVERHEAD_1, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "BBH_TX_FIFO_SIZE", 1, CORE_1_INDEX, &BBH_TX_FIFO_SIZE_1, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "US_TM_FIRST_QUEUE_MAPPING", 1, CORE_1_INDEX, &US_TM_FIRST_QUEUE_MAPPING_1, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "BBH_TX_EPON_INGRESS_COUNTER_TABLE", 1, CORE_1_INDEX, &BBH_TX_EPON_INGRESS_COUNTER_TABLE_1, 40, 1, 1 },
#endif
#if defined BCM6856
	{ "MIRRORING_SCRATCH", 1, CORE_1_INDEX, &MIRRORING_SCRATCH_1, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "DISPATCHER_CREDIT_TABLE", 1, CORE_1_INDEX, &DISPATCHER_CREDIT_TABLE_1, 3, 1, 1 },
#endif
#if defined BCM6856
	{ "WLAN_MCAST_DFT_LIST_SIZE", 1, CORE_1_INDEX, &WLAN_MCAST_DFT_LIST_SIZE_1, 64, 1, 1 },
#endif
#if defined BCM6856
	{ "BBH_TX_EPON_EGRESS_COUNTER_TABLE", 1, CORE_1_INDEX, &BBH_TX_EPON_EGRESS_COUNTER_TABLE_1, 5, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_MCAST_DISPATCHER_CREDIT_TABLE", 1, CORE_1_INDEX, &DHD_MCAST_DISPATCHER_CREDIT_TABLE_1, 3, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_CPU_RECYCLE_RING_DESCRIPTOR_TABLE", 1, CORE_1_INDEX, &DHD_CPU_RECYCLE_RING_DESCRIPTOR_TABLE_1, 3, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_DOORBELL_VALUE", 1, CORE_1_INDEX, &DHD_DOORBELL_VALUE_1, 3, 1, 1 },
#endif
#if defined BCM6856
	{ "US_TM_TM_ACTION_PTR_TABLE", 1, CORE_1_INDEX, &US_TM_TM_ACTION_PTR_TABLE_1, 17, 1, 1 },
#endif
#if defined BCM6856
	{ "TX_MIRRORING_DISPATCHER_CREDIT_TABLE", 1, CORE_1_INDEX, &TX_MIRRORING_DISPATCHER_CREDIT_TABLE_1, 3, 1, 1 },
#endif
#if defined BCM6856
	{ "NATC_TBL_CFG", 1, CORE_1_INDEX, &NATC_TBL_CFG_1, 2, 1, 1 },
#endif
#if defined BCM6856
	{ "FPM_GLOBAL_CFG", 1, CORE_1_INDEX, &FPM_GLOBAL_CFG_1, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "UPDATE_FIFO_TABLE", 1, CORE_1_INDEX, &UPDATE_FIFO_TABLE_1, 8, 1, 1 },
#endif
#if defined BCM6856
	{ "WLAN_MCAST_SCRATCHPAD", 1, CORE_1_INDEX, &WLAN_MCAST_SCRATCHPAD_1, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_TX_POST_FLOW_RING_MGMT_DESCRIPTOR_BUFFER", 1, CORE_1_INDEX, &DHD_TX_POST_FLOW_RING_MGMT_DESCRIPTOR_BUFFER_1, 3, 1, 1 },
#endif
#if defined BCM6856
	{ "EPON_UPDATE_FIFO_TABLE", 1, CORE_1_INDEX, &EPON_UPDATE_FIFO_TABLE_1, 8, 1, 1 },
#endif
#if defined BCM6856
	{ "WLAN_MCAST_DHD_STATION_CTX_TABLE", 1, CORE_1_INDEX, &WLAN_MCAST_DHD_STATION_CTX_TABLE_1, 64, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_TX_POST_UPDATE_FIFO_TABLE", 1, CORE_1_INDEX, &DHD_TX_POST_UPDATE_FIFO_TABLE_1, 8, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_MCAST_UPDATE_FIFO_TABLE", 1, CORE_1_INDEX, &DHD_MCAST_UPDATE_FIFO_TABLE_1, 8, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_MCAST_PD_FIFO_TABLE", 1, CORE_1_INDEX, &DHD_MCAST_PD_FIFO_TABLE_1, 2, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_HW_CFG", 1, CORE_1_INDEX, &DHD_HW_CFG_1, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_TX_SCRATCHPAD", 1, CORE_2_INDEX, &CPU_TX_SCRATCHPAD_2, 128, 1, 1 },
#endif
#if defined BCM6856
	{ "RX_FLOW_TABLE", 1, CORE_2_INDEX, &RX_FLOW_TABLE_2, 320, 1, 1 },
#endif
#if defined BCM6856
	{ "LAYER2_HEADER_COPY_MAPPING_TABLE", 1, CORE_2_INDEX, &LAYER2_HEADER_COPY_MAPPING_TABLE_2, 30, 1, 1 },
#endif
#if defined BCM6856
	{ "IPTV_CONFIGURATION_TABLE", 1, CORE_2_INDEX, &IPTV_CONFIGURATION_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "DSCP_TO_PBITS_MAP_TABLE", 1, CORE_2_INDEX, &DSCP_TO_PBITS_MAP_TABLE_2, 4, 64, 1 },
#endif
#if defined BCM6856
	{ "TC_TO_QUEUE_TABLE", 1, CORE_2_INDEX, &TC_TO_QUEUE_TABLE_2, 65, 1, 1 },
#endif
#if defined BCM6856
	{ "GENERAL_QUEUE_DYNAMIC_MNG_TABLE", 1, CORE_2_INDEX, &GENERAL_QUEUE_DYNAMIC_MNG_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "FPM_POOL_NUMBER_MAPPING_TABLE", 1, CORE_2_INDEX, &FPM_POOL_NUMBER_MAPPING_TABLE_2, 16, 1, 1 },
#endif
#if defined BCM6856
	{ "RUNNER_GLOBAL_REGISTERS_INIT", 1, CORE_2_INDEX, &RUNNER_GLOBAL_REGISTERS_INIT_2, 8, 1, 1 },
#endif
#if defined BCM6856
	{ "GLOBAL_DSCP_TO_PBITS_TABLE", 1, CORE_2_INDEX, &GLOBAL_DSCP_TO_PBITS_TABLE_2, 1, 64, 1 },
#endif
#if defined BCM6856
	{ "POLICER_PARAMS_TABLE", 1, CORE_2_INDEX, &POLICER_PARAMS_TABLE_2, 80, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_TX_FPM_POOL_NUMBER_MAPPING_TABLE", 1, CORE_2_INDEX, &CPU_TX_FPM_POOL_NUMBER_MAPPING_TABLE_2, 16, 1, 1 },
#endif
#if defined BCM6856
	{ "VPORT_TO_DSCP_TO_PBITS_TABLE", 1, CORE_2_INDEX, &VPORT_TO_DSCP_TO_PBITS_TABLE_2, 32, 1, 1 },
#endif
#if defined BCM6856
	{ "INGRESS_FILTER_PROFILE_TABLE", 1, CORE_2_INDEX, &INGRESS_FILTER_PROFILE_TABLE_2, 16, 1, 1 },
#endif
#if defined BCM6856
	{ "PBIT_TO_GEM_TABLE", 1, CORE_2_INDEX, &PBIT_TO_GEM_TABLE_2, 8, 8, 1 },
#endif
#if defined BCM6856
	{ "DUAL_STACK_LITE_TABLE", 1, CORE_2_INDEX, &DUAL_STACK_LITE_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "PBIT_TO_QUEUE_TABLE", 1, CORE_2_INDEX, &PBIT_TO_QUEUE_TABLE_2, 65, 1, 1 },
#endif
#if defined BCM6856
	{ "REGISTERS_BUFFER", 1, CORE_2_INDEX, &REGISTERS_BUFFER_2, 32, 1, 1 },
#endif
#if defined BCM6856
	{ "PROCESSING_QUEUE_DYNAMIC_MNG_TABLE", 1, CORE_2_INDEX, &PROCESSING_QUEUE_DYNAMIC_MNG_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "FPM_REPLY", 1, CORE_2_INDEX, &FPM_REPLY_2, 2, 1, 1 },
#endif
#if defined BCM6856
	{ "TCAM_IC_CFG_TABLE", 1, CORE_2_INDEX, &TCAM_IC_CFG_TABLE_2, 6, 1, 1 },
#endif
#if defined BCM6856
	{ "NATC_TBL_CFG", 1, CORE_2_INDEX, &NATC_TBL_CFG_2, 2, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_RECYCLE_INTERRUPT_COALESCING_TABLE", 1, CORE_2_INDEX, &CPU_RECYCLE_INTERRUPT_COALESCING_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "LOOPBACK_QUEUE_TABLE", 1, CORE_2_INDEX, &LOOPBACK_QUEUE_TABLE_2, 40, 1, 1 },
#endif
#if defined BCM6856
	{ "RX_FLOW_CONTEXT_DDR_ADDR", 1, CORE_2_INDEX, &RX_FLOW_CONTEXT_DDR_ADDR_2, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "INGRESS_FILTER_L2_REASON_TABLE", 1, CORE_2_INDEX, &INGRESS_FILTER_L2_REASON_TABLE_2, 16, 1, 1 },
#endif
#if defined BCM6856
	{ "VPORT_CFG_EX_TABLE", 1, CORE_2_INDEX, &VPORT_CFG_EX_TABLE_2, 40, 1, 1 },
#endif
#if defined BCM6856
	{ "NULL_BUFFER", 1, CORE_2_INDEX, &NULL_BUFFER_2, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_TX_EGRESS_DISPATCHER_CREDIT_TABLE", 1, CORE_2_INDEX, &CPU_TX_EGRESS_DISPATCHER_CREDIT_TABLE_2, 3, 1, 1 },
#endif
#if defined BCM6856
	{ "TASK_IDX", 1, CORE_2_INDEX, &TASK_IDX_2, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "QUEUE_THRESHOLD_VECTOR", 1, CORE_2_INDEX, &QUEUE_THRESHOLD_VECTOR_2, 9, 1, 1 },
#endif
#if defined BCM6856
	{ "FORCE_IC_IP_FLOW", 1, CORE_2_INDEX, &FORCE_IC_IP_FLOW_2, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "FORCE_DSCP", 1, CORE_2_INDEX, &FORCE_DSCP_2, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "CORE_ID_TABLE", 1, CORE_2_INDEX, &CORE_ID_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "SRAM_DUMMY_STORE", 1, CORE_2_INDEX, &SRAM_DUMMY_STORE_2, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR", 1, CORE_2_INDEX, &CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR_2, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_TX_INGRESS_DISPATCHER_CREDIT_TABLE", 1, CORE_2_INDEX, &CPU_TX_INGRESS_DISPATCHER_CREDIT_TABLE_2, 3, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_FPM_LOW_THRESHOLD", 1, CORE_2_INDEX, &DHD_FPM_LOW_THRESHOLD_2, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_FPM_HIGH_THRESHOLD", 1, CORE_2_INDEX, &DHD_FPM_HIGH_THRESHOLD_2, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "DS_PACKET_BUFFER", 1, CORE_2_INDEX, &DS_PACKET_BUFFER_2, 8, 1, 1 },
#endif
#if defined BCM6856
	{ "RUNNER_PROFILING_TRACE_BUFFER", 1, CORE_2_INDEX, &RUNNER_PROFILING_TRACE_BUFFER_2, 128, 1, 1 },
#endif
#if defined BCM6856
	{ "TCAM_IC_CMD_TABLE", 1, CORE_2_INDEX, &TCAM_IC_CMD_TABLE_2, 7, 16, 1 },
#endif
#if defined BCM6856
	{ "FLOW_BASED_ACTION_PTR_TABLE", 1, CORE_2_INDEX, &FLOW_BASED_ACTION_PTR_TABLE_2, 17, 1, 1 },
#endif
#if defined BCM6856
	{ "RX_MIRRORING_CONFIGURATION", 1, CORE_2_INDEX, &RX_MIRRORING_CONFIGURATION_2, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "RATE_LIMIT_OVERHEAD", 1, CORE_2_INDEX, &RATE_LIMIT_OVERHEAD_2, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "INGRESS_QOS_DONT_DROP_RATIO", 1, CORE_2_INDEX, &INGRESS_QOS_DONT_DROP_RATIO_2, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "INGRESS_QOS_WAN_UNTAGGED_PRIORITY", 1, CORE_2_INDEX, &INGRESS_QOS_WAN_UNTAGGED_PRIORITY_2, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "IC_DEBUG_COUNTER_MODE", 1, CORE_2_INDEX, &IC_DEBUG_COUNTER_MODE_2, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_RX_INTERRUPT_ID_DDR_ADDR", 1, CORE_2_INDEX, &CPU_RX_INTERRUPT_ID_DDR_ADDR_2, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_RECYCLE_RING_DESCRIPTOR_TABLE", 1, CORE_2_INDEX, &CPU_RECYCLE_RING_DESCRIPTOR_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "LAYER2_GRE_TUNNEL_TABLE", 1, CORE_2_INDEX, &LAYER2_GRE_TUNNEL_TABLE_2, 12, 1, 1 },
#endif
#if defined BCM6856
	{ "RULE_BASED_ACTION_PTR_TABLE", 1, CORE_2_INDEX, &RULE_BASED_ACTION_PTR_TABLE_2, 17, 1, 1 },
#endif
#if defined BCM6856
	{ "IC_MCAST_ENABLE", 1, CORE_2_INDEX, &IC_MCAST_ENABLE_2, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_REDIRECT_MODE", 1, CORE_2_INDEX, &CPU_REDIRECT_MODE_2, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "IPTV_CLASSIFICATION_CFG_TABLE", 1, CORE_2_INDEX, &IPTV_CLASSIFICATION_CFG_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "INGRESS_FILTER_1588_CFG", 1, CORE_2_INDEX, &INGRESS_FILTER_1588_CFG_2, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_RECYCLE_RING_INDEX_DDR_ADDR_TABLE", 1, CORE_2_INDEX, &CPU_RECYCLE_RING_INDEX_DDR_ADDR_TABLE_2, 2, 1, 1 },
#endif
#if defined BCM6856
	{ "SYSTEM_CONFIGURATION", 1, CORE_2_INDEX, &SYSTEM_CONFIGURATION_2, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "IPTV_ACTION_PTR_TABLE", 1, CORE_2_INDEX, &IPTV_ACTION_PTR_TABLE_2, 17, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_RECYCLE_RING_INTERRUPT_COUNTER_TABLE", 1, CORE_2_INDEX, &CPU_RECYCLE_RING_INTERRUPT_COUNTER_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_FEED_RING_DESCRIPTOR_TABLE", 1, CORE_2_INDEX, &CPU_FEED_RING_DESCRIPTOR_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "TX_FLOW_TABLE", 1, CORE_2_INDEX, &TX_FLOW_TABLE_2, 320, 1, 1 },
#endif
#if defined BCM6856
	{ "VLAN_ACTION_GPE_HANDLER_PTR_TABLE", 1, CORE_2_INDEX, &VLAN_ACTION_GPE_HANDLER_PTR_TABLE_2, 17, 1, 1 },
#endif
#if defined BCM6856
	{ "TUNNELS_PARSING_CFG", 1, CORE_2_INDEX, &TUNNELS_PARSING_CFG_2, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "FPM_GLOBAL_CFG", 1, CORE_2_INDEX, &FPM_GLOBAL_CFG_2, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_TX_EGRESS_UPDATE_FIFO_TABLE", 1, CORE_2_INDEX, &CPU_TX_EGRESS_UPDATE_FIFO_TABLE_2, 8, 1, 1 },
#endif
#if defined BCM6856
	{ "TCAM_IC_HANDLER_TABLE", 1, CORE_2_INDEX, &TCAM_IC_HANDLER_TABLE_2, 16, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_TX_INGRESS_UPDATE_FIFO_TABLE", 1, CORE_2_INDEX, &CPU_TX_INGRESS_UPDATE_FIFO_TABLE_2, 8, 1, 1 },
#endif
#if defined BCM6856
	{ "BRIDGE_CFG_TABLE", 1, CORE_2_INDEX, &BRIDGE_CFG_TABLE_2, 3, 1, 1 },
#endif
#if defined BCM6856
	{ "IPTV_CFG_TABLE", 1, CORE_2_INDEX, &IPTV_CFG_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "VPORT_CFG_TABLE", 1, CORE_2_INDEX, &VPORT_CFG_TABLE_2, 40, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_TX_EGRESS_PD_FIFO_TABLE", 1, CORE_2_INDEX, &CPU_TX_EGRESS_PD_FIFO_TABLE_2, 2, 1, 1 },
#endif
#if defined BCM6856
	{ "IPTV_DDR_CTX_TABLE_ADDRESS", 1, CORE_2_INDEX, &IPTV_DDR_CTX_TABLE_ADDRESS_2, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "NAT_CACHE_CFG", 1, CORE_2_INDEX, &NAT_CACHE_CFG_2, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "INGRESS_FILTER_CFG", 1, CORE_2_INDEX, &INGRESS_FILTER_CFG_2, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_TX_INGRESS_PD_FIFO_TABLE", 1, CORE_2_INDEX, &CPU_TX_INGRESS_PD_FIFO_TABLE_2, 2, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_RECYCLE_SRAM_PD_FIFO", 1, CORE_2_INDEX, &CPU_RECYCLE_SRAM_PD_FIFO_2, 16, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_RECYCLE_INTERRUPT_SCRATCH", 1, CORE_2_INDEX, &CPU_RECYCLE_INTERRUPT_SCRATCH_2, 2, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_RECYCLE_SHADOW_RD_IDX", 1, CORE_2_INDEX, &CPU_RECYCLE_SHADOW_RD_IDX_2, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_RECYCLE_SHADOW_WR_IDX", 1, CORE_2_INDEX, &CPU_RECYCLE_SHADOW_WR_IDX_2, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "TX_FLOW_TABLE", 1, CORE_3_INDEX, &TX_FLOW_TABLE_3, 320, 1, 1 },
#endif
#if defined BCM6856
	{ "DIRECT_PROCESSING_PD_TABLE", 1, CORE_3_INDEX, &DIRECT_PROCESSING_PD_TABLE_3, 2, 1, 1 },
#endif
#if defined BCM6856
	{ "RUNNER_GLOBAL_REGISTERS_INIT", 1, CORE_3_INDEX, &RUNNER_GLOBAL_REGISTERS_INIT_3, 8, 1, 1 },
#endif
#if defined BCM6856
	{ "DS_CPU_RX_METER_TABLE", 1, CORE_3_INDEX, &DS_CPU_RX_METER_TABLE_3, 16, 1, 1 },
#endif
#if defined BCM6856
	{ "TCAM_IC_CMD_TABLE", 1, CORE_3_INDEX, &TCAM_IC_CMD_TABLE_3, 7, 16, 1 },
#endif
#if defined BCM6856
	{ "GLOBAL_DSCP_TO_PBITS_TABLE", 1, CORE_3_INDEX, &GLOBAL_DSCP_TO_PBITS_TABLE_3, 1, 64, 1 },
#endif
#if defined BCM6856
	{ "CPU_RX_SCRATCHPAD", 1, CORE_3_INDEX, &CPU_RX_SCRATCHPAD_3, 64, 1, 1 },
#endif
#if defined BCM6856
	{ "LAYER2_GRE_TUNNEL_TABLE", 1, CORE_3_INDEX, &LAYER2_GRE_TUNNEL_TABLE_3, 12, 1, 1 },
#endif
#if defined BCM6856
	{ "US_CPU_RX_METER_TABLE", 1, CORE_3_INDEX, &US_CPU_RX_METER_TABLE_3, 16, 1, 1 },
#endif
#if defined BCM6856
	{ "RX_FLOW_TABLE", 1, CORE_3_INDEX, &RX_FLOW_TABLE_3, 320, 1, 1 },
#endif
#if defined BCM6856
	{ "LAYER2_HEADER_COPY_MAPPING_TABLE", 1, CORE_3_INDEX, &LAYER2_HEADER_COPY_MAPPING_TABLE_3, 30, 1, 1 },
#endif
#if defined BCM6856
	{ "IPTV_CONFIGURATION_TABLE", 1, CORE_3_INDEX, &IPTV_CONFIGURATION_TABLE_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "DSCP_TO_PBITS_MAP_TABLE", 1, CORE_3_INDEX, &DSCP_TO_PBITS_MAP_TABLE_3, 4, 64, 1 },
#endif
#if defined BCM6856
	{ "TC_TO_QUEUE_TABLE", 1, CORE_3_INDEX, &TC_TO_QUEUE_TABLE_3, 65, 1, 1 },
#endif
#if defined BCM6856
	{ "GENERAL_QUEUE_DYNAMIC_MNG_TABLE", 1, CORE_3_INDEX, &GENERAL_QUEUE_DYNAMIC_MNG_TABLE_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "FPM_POOL_NUMBER_MAPPING_TABLE", 1, CORE_3_INDEX, &FPM_POOL_NUMBER_MAPPING_TABLE_3, 16, 1, 1 },
#endif
#if defined BCM6856
	{ "VPORT_TO_DSCP_TO_PBITS_TABLE", 1, CORE_3_INDEX, &VPORT_TO_DSCP_TO_PBITS_TABLE_3, 32, 1, 1 },
#endif
#if defined BCM6856
	{ "PBIT_TO_GEM_TABLE", 1, CORE_3_INDEX, &PBIT_TO_GEM_TABLE_3, 8, 8, 1 },
#endif
#if defined BCM6856
	{ "CPU_REASON_AND_VPORT_TO_METER_TABLE", 1, CORE_3_INDEX, &CPU_REASON_AND_VPORT_TO_METER_TABLE_3, 120, 1, 1 },
#endif
#if defined BCM6856
	{ "PROCESSING_QUEUE_DYNAMIC_MNG_TABLE", 1, CORE_3_INDEX, &PROCESSING_QUEUE_DYNAMIC_MNG_TABLE_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_RING_INTERRUPT_COUNTER_TABLE", 1, CORE_3_INDEX, &CPU_RING_INTERRUPT_COUNTER_TABLE_3, 18, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_RECYCLE_INTERRUPT_COALESCING_TABLE", 1, CORE_3_INDEX, &CPU_RECYCLE_INTERRUPT_COALESCING_TABLE_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "TCAM_IC_HANDLER_TABLE", 1, CORE_3_INDEX, &TCAM_IC_HANDLER_TABLE_3, 16, 1, 1 },
#endif
#if defined BCM6856
	{ "DUAL_STACK_LITE_TABLE", 1, CORE_3_INDEX, &DUAL_STACK_LITE_TABLE_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "DS_PACKET_BUFFER", 1, CORE_3_INDEX, &DS_PACKET_BUFFER_3, 8, 1, 1 },
#endif
#if defined BCM6856
	{ "PBIT_TO_QUEUE_TABLE", 1, CORE_3_INDEX, &PBIT_TO_QUEUE_TABLE_3, 65, 1, 1 },
#endif
#if defined BCM6856
	{ "RX_FLOW_CONTEXT_DDR_ADDR", 1, CORE_3_INDEX, &RX_FLOW_CONTEXT_DDR_ADDR_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_RX_PSRAM_GET_NEXT_SCRATCHPAD", 1, CORE_3_INDEX, &CPU_RX_PSRAM_GET_NEXT_SCRATCHPAD_3, 4, 1, 1 },
#endif
#if defined BCM6856
	{ "BRIDGE_CFG_TABLE", 1, CORE_3_INDEX, &BRIDGE_CFG_TABLE_3, 3, 1, 1 },
#endif
#if defined BCM6856
	{ "NULL_BUFFER", 1, CORE_3_INDEX, &NULL_BUFFER_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "DS_CPU_REASON_TO_METER_TABLE", 1, CORE_3_INDEX, &DS_CPU_REASON_TO_METER_TABLE_3, 64, 1, 1 },
#endif
#if defined BCM6856
	{ "POLICER_PARAMS_TABLE", 1, CORE_3_INDEX, &POLICER_PARAMS_TABLE_3, 80, 1, 1 },
#endif
#if defined BCM6856
	{ "SCRATCH", 1, CORE_3_INDEX, &SCRATCH_3, 8, 1, 1 },
#endif
#if defined BCM6856
	{ "INGRESS_FILTER_L2_REASON_TABLE", 1, CORE_3_INDEX, &INGRESS_FILTER_L2_REASON_TABLE_3, 16, 1, 1 },
#endif
#if defined BCM6856
	{ "INGRESS_FILTER_PROFILE_TABLE", 1, CORE_3_INDEX, &INGRESS_FILTER_PROFILE_TABLE_3, 16, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_RX_COPY_PD_FIFO_TABLE", 1, CORE_3_INDEX, &CPU_RX_COPY_PD_FIFO_TABLE_3, 4, 1, 1 },
#endif
#if defined BCM6856
	{ "US_CPU_REASON_TO_METER_TABLE", 1, CORE_3_INDEX, &US_CPU_REASON_TO_METER_TABLE_3, 64, 1, 1 },
#endif
#if defined BCM6856
	{ "RUNNER_PROFILING_TRACE_BUFFER", 1, CORE_3_INDEX, &RUNNER_PROFILING_TRACE_BUFFER_3, 128, 1, 1 },
#endif
#if defined BCM6856
	{ "VPORT_CFG_TABLE", 1, CORE_3_INDEX, &VPORT_CFG_TABLE_3, 40, 1, 1 },
#endif
#if defined BCM6856
	{ "REGISTERS_BUFFER", 1, CORE_3_INDEX, &REGISTERS_BUFFER_3, 32, 1, 1 },
#endif
#if defined BCM6856
	{ "LOOPBACK_QUEUE_TABLE", 1, CORE_3_INDEX, &LOOPBACK_QUEUE_TABLE_3, 40, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR", 1, CORE_3_INDEX, &CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_RX_COPY_DISPATCHER_CREDIT_TABLE", 1, CORE_3_INDEX, &CPU_RX_COPY_DISPATCHER_CREDIT_TABLE_3, 3, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_RX_INTERRUPT_SCRATCH", 1, CORE_3_INDEX, &CPU_RX_INTERRUPT_SCRATCH_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_RECYCLE_SRAM_PD_FIFO", 1, CORE_3_INDEX, &CPU_RECYCLE_SRAM_PD_FIFO_3, 16, 1, 1 },
#endif
#if defined BCM6856
	{ "NATC_TBL_CFG", 1, CORE_3_INDEX, &NATC_TBL_CFG_3, 2, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_RECYCLE_RING_DESCRIPTOR_TABLE", 1, CORE_3_INDEX, &CPU_RECYCLE_RING_DESCRIPTOR_TABLE_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "TCAM_IC_CFG_TABLE", 1, CORE_3_INDEX, &TCAM_IC_CFG_TABLE_3, 6, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_RECYCLE_RING_INDEX_DDR_ADDR_TABLE", 1, CORE_3_INDEX, &CPU_RECYCLE_RING_INDEX_DDR_ADDR_TABLE_3, 2, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_INTERRUPT_COALESCING_TABLE", 1, CORE_3_INDEX, &CPU_INTERRUPT_COALESCING_TABLE_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "FLOW_BASED_ACTION_PTR_TABLE", 1, CORE_3_INDEX, &FLOW_BASED_ACTION_PTR_TABLE_3, 17, 1, 1 },
#endif
#if defined BCM6856
	{ "DIRECT_PROCESSING_WAN_VIQ_EXCLUSIVE", 1, CORE_3_INDEX, &DIRECT_PROCESSING_WAN_VIQ_EXCLUSIVE_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "FORCE_IC_IP_FLOW", 1, CORE_3_INDEX, &FORCE_IC_IP_FLOW_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "TASK_IDX", 1, CORE_3_INDEX, &TASK_IDX_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_RX_INTERRUPT_ID_DDR_ADDR", 1, CORE_3_INDEX, &CPU_RX_INTERRUPT_ID_DDR_ADDR_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_FEED_RING_DESCRIPTOR_TABLE", 1, CORE_3_INDEX, &CPU_FEED_RING_DESCRIPTOR_TABLE_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "VPORT_CFG_EX_TABLE", 1, CORE_3_INDEX, &VPORT_CFG_EX_TABLE_3, 40, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_RX_LOCAL_SCRATCH", 1, CORE_3_INDEX, &CPU_RX_LOCAL_SCRATCH_3, 2, 1, 1 },
#endif
#if defined BCM6856
	{ "SYSTEM_CONFIGURATION", 1, CORE_3_INDEX, &SYSTEM_CONFIGURATION_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "FORCE_DSCP", 1, CORE_3_INDEX, &FORCE_DSCP_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "CORE_ID_TABLE", 1, CORE_3_INDEX, &CORE_ID_TABLE_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "SRAM_DUMMY_STORE", 1, CORE_3_INDEX, &SRAM_DUMMY_STORE_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "RULE_BASED_ACTION_PTR_TABLE", 1, CORE_3_INDEX, &RULE_BASED_ACTION_PTR_TABLE_3, 17, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_FEED_RING_INTERRUPT_THRESHOLD", 1, CORE_3_INDEX, &CPU_FEED_RING_INTERRUPT_THRESHOLD_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_FEED_RING_INTERRUPT_COUNTER", 1, CORE_3_INDEX, &CPU_FEED_RING_INTERRUPT_COUNTER_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_FPM_LOW_THRESHOLD", 1, CORE_3_INDEX, &DHD_FPM_LOW_THRESHOLD_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_RECYCLE_RING_INTERRUPT_COUNTER_TABLE", 1, CORE_3_INDEX, &CPU_RECYCLE_RING_INTERRUPT_COUNTER_TABLE_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "FPM_GLOBAL_CFG", 1, CORE_3_INDEX, &FPM_GLOBAL_CFG_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_FPM_HIGH_THRESHOLD", 1, CORE_3_INDEX, &DHD_FPM_HIGH_THRESHOLD_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "RX_MIRRORING_CONFIGURATION", 1, CORE_3_INDEX, &RX_MIRRORING_CONFIGURATION_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_VPORT_TO_METER_TABLE", 1, CORE_3_INDEX, &CPU_VPORT_TO_METER_TABLE_3, 40, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_RX_COPY_LOCAL_SCRATCH", 1, CORE_3_INDEX, &CPU_RX_COPY_LOCAL_SCRATCH_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "TUNNELS_PARSING_CFG", 1, CORE_3_INDEX, &TUNNELS_PARSING_CFG_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "IPTV_CFG_TABLE", 1, CORE_3_INDEX, &IPTV_CFG_TABLE_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "IPTV_ACTION_PTR_TABLE", 1, CORE_3_INDEX, &IPTV_ACTION_PTR_TABLE_3, 17, 1, 1 },
#endif
#if defined BCM6856
	{ "RATE_LIMIT_OVERHEAD", 1, CORE_3_INDEX, &RATE_LIMIT_OVERHEAD_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "INGRESS_QOS_DONT_DROP_RATIO", 1, CORE_3_INDEX, &INGRESS_QOS_DONT_DROP_RATIO_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "INGRESS_QOS_WAN_UNTAGGED_PRIORITY", 1, CORE_3_INDEX, &INGRESS_QOS_WAN_UNTAGGED_PRIORITY_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "MAC_TYPE", 1, CORE_3_INDEX, &MAC_TYPE_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "IC_DEBUG_COUNTER_MODE", 1, CORE_3_INDEX, &IC_DEBUG_COUNTER_MODE_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "IC_MCAST_ENABLE", 1, CORE_3_INDEX, &IC_MCAST_ENABLE_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "IPTV_DDR_CTX_TABLE_ADDRESS", 1, CORE_3_INDEX, &IPTV_DDR_CTX_TABLE_ADDRESS_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "NAT_CACHE_CFG", 1, CORE_3_INDEX, &NAT_CACHE_CFG_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_REDIRECT_MODE", 1, CORE_3_INDEX, &CPU_REDIRECT_MODE_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "DIRECT_PROCESSING_EPON_CONTROL_SCRATCH", 1, CORE_3_INDEX, &DIRECT_PROCESSING_EPON_CONTROL_SCRATCH_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "IPTV_CLASSIFICATION_CFG_TABLE", 1, CORE_3_INDEX, &IPTV_CLASSIFICATION_CFG_TABLE_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "INGRESS_FILTER_1588_CFG", 1, CORE_3_INDEX, &INGRESS_FILTER_1588_CFG_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "VLAN_ACTION_GPE_HANDLER_PTR_TABLE", 1, CORE_3_INDEX, &VLAN_ACTION_GPE_HANDLER_PTR_TABLE_3, 17, 1, 1 },
#endif
#if defined BCM6856
	{ "INGRESS_FILTER_CFG", 1, CORE_3_INDEX, &INGRESS_FILTER_CFG_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "UPDATE_FIFO_TABLE", 1, CORE_3_INDEX, &UPDATE_FIFO_TABLE_3, 8, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_RX_COPY_UPDATE_FIFO_TABLE", 1, CORE_3_INDEX, &CPU_RX_COPY_UPDATE_FIFO_TABLE_3, 8, 1, 1 },
#endif
#if defined BCM6856
	{ "PD_FIFO_TABLE", 1, CORE_3_INDEX, &PD_FIFO_TABLE_3, 2, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_REASON_TO_TC", 1, CORE_3_INDEX, &CPU_REASON_TO_TC_3, 64, 1, 1 },
#endif
#if defined BCM6856
	{ "TC_TO_CPU_RXQ", 1, CORE_3_INDEX, &TC_TO_CPU_RXQ_3, 64, 1, 1 },
#endif
#if defined BCM6856
	{ "EXC_TC_TO_CPU_RXQ", 1, CORE_3_INDEX, &EXC_TC_TO_CPU_RXQ_3, 64, 1, 1 },
#endif
#if defined BCM6856
	{ "VPORT_TO_CPU_OBJ", 1, CORE_3_INDEX, &VPORT_TO_CPU_OBJ_3, 40, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_RECYCLE_INTERRUPT_SCRATCH", 1, CORE_3_INDEX, &CPU_RECYCLE_INTERRUPT_SCRATCH_3, 2, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_RECYCLE_SHADOW_RD_IDX", 1, CORE_3_INDEX, &CPU_RECYCLE_SHADOW_RD_IDX_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_RECYCLE_SHADOW_WR_IDX", 1, CORE_3_INDEX, &CPU_RECYCLE_SHADOW_WR_IDX_3, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_RING_DESCRIPTORS_TABLE", 1, CORE_3_INDEX, &CPU_RING_DESCRIPTORS_TABLE_3, 16, 1, 1 },
#endif
#if defined BCM6856
	{ "RX_FLOW_TABLE", 1, CORE_4_INDEX, &RX_FLOW_TABLE_4, 320, 1, 1 },
#endif
#if defined BCM6856
	{ "LAYER2_HEADER_COPY_MAPPING_TABLE", 1, CORE_4_INDEX, &LAYER2_HEADER_COPY_MAPPING_TABLE_4, 30, 1, 1 },
#endif
#if defined BCM6856
	{ "IPTV_CONFIGURATION_TABLE", 1, CORE_4_INDEX, &IPTV_CONFIGURATION_TABLE_4, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "DSCP_TO_PBITS_MAP_TABLE", 1, CORE_4_INDEX, &DSCP_TO_PBITS_MAP_TABLE_4, 4, 64, 1 },
#endif
#if defined BCM6856
	{ "TC_TO_QUEUE_TABLE", 1, CORE_4_INDEX, &TC_TO_QUEUE_TABLE_4, 65, 1, 1 },
#endif
#if defined BCM6856
	{ "GENERAL_QUEUE_DYNAMIC_MNG_TABLE", 1, CORE_4_INDEX, &GENERAL_QUEUE_DYNAMIC_MNG_TABLE_4, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "FPM_POOL_NUMBER_MAPPING_TABLE", 1, CORE_4_INDEX, &FPM_POOL_NUMBER_MAPPING_TABLE_4, 16, 1, 1 },
#endif
#if defined BCM6856
	{ "RUNNER_GLOBAL_REGISTERS_INIT", 1, CORE_4_INDEX, &RUNNER_GLOBAL_REGISTERS_INIT_4, 8, 1, 1 },
#endif
#if defined BCM6856
	{ "GLOBAL_DSCP_TO_PBITS_TABLE", 1, CORE_4_INDEX, &GLOBAL_DSCP_TO_PBITS_TABLE_4, 1, 64, 1 },
#endif
#if defined BCM6856
	{ "DHD_RX_COMPLETE_FLOW_RING_BUFFER", 1, CORE_4_INDEX, &DHD_RX_COMPLETE_FLOW_RING_BUFFER_4, 3, 1, 1 },
#endif
#if defined BCM6856
	{ "VPORT_TO_DSCP_TO_PBITS_TABLE", 1, CORE_4_INDEX, &VPORT_TO_DSCP_TO_PBITS_TABLE_4, 32, 1, 1 },
#endif
#if defined BCM6856
	{ "INGRESS_FILTER_PROFILE_TABLE", 1, CORE_4_INDEX, &INGRESS_FILTER_PROFILE_TABLE_4, 16, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_RX_POST_FLOW_RING_BUFFER", 1, CORE_4_INDEX, &DHD_RX_POST_FLOW_RING_BUFFER_4, 3, 1, 1 },
#endif
#if defined BCM6856
	{ "TCAM_IC_HANDLER_TABLE", 1, CORE_4_INDEX, &TCAM_IC_HANDLER_TABLE_4, 16, 1, 1 },
#endif
#if defined BCM6856
	{ "PBIT_TO_QUEUE_TABLE", 1, CORE_4_INDEX, &PBIT_TO_QUEUE_TABLE_4, 65, 1, 1 },
#endif
#if defined BCM6856
	{ "PROCESSING_QUEUE_DYNAMIC_MNG_TABLE", 1, CORE_4_INDEX, &PROCESSING_QUEUE_DYNAMIC_MNG_TABLE_4, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "INGRESS_FILTER_L2_REASON_TABLE", 1, CORE_4_INDEX, &INGRESS_FILTER_L2_REASON_TABLE_4, 16, 1, 1 },
#endif
#if defined BCM6856
	{ "BRIDGE_CFG_TABLE", 1, CORE_4_INDEX, &BRIDGE_CFG_TABLE_4, 3, 1, 1 },
#endif
#if defined BCM6856
	{ "RX_FLOW_CONTEXT_DDR_ADDR", 1, CORE_4_INDEX, &RX_FLOW_CONTEXT_DDR_ADDR_4, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "PBIT_TO_GEM_TABLE", 1, CORE_4_INDEX, &PBIT_TO_GEM_TABLE_4, 8, 8, 1 },
#endif
#if defined BCM6856
	{ "POLICER_PARAMS_TABLE", 1, CORE_4_INDEX, &POLICER_PARAMS_TABLE_4, 80, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_RX_COMPLETE_0_DISPATCHER_CREDIT_TABLE", 1, CORE_4_INDEX, &DHD_RX_COMPLETE_0_DISPATCHER_CREDIT_TABLE_4, 3, 1, 1 },
#endif
#if defined BCM6856
	{ "TASK_IDX", 1, CORE_4_INDEX, &TASK_IDX_4, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_RX_COMPLETE_1_DISPATCHER_CREDIT_TABLE", 1, CORE_4_INDEX, &DHD_RX_COMPLETE_1_DISPATCHER_CREDIT_TABLE_4, 3, 1, 1 },
#endif
#if defined BCM6856
	{ "FORCE_IC_IP_FLOW", 1, CORE_4_INDEX, &FORCE_IC_IP_FLOW_4, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "FORCE_DSCP", 1, CORE_4_INDEX, &FORCE_DSCP_4, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "CORE_ID_TABLE", 1, CORE_4_INDEX, &CORE_ID_TABLE_4, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "SRAM_DUMMY_STORE", 1, CORE_4_INDEX, &SRAM_DUMMY_STORE_4, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_RX_COMPLETE_2_DISPATCHER_CREDIT_TABLE", 1, CORE_4_INDEX, &DHD_RX_COMPLETE_2_DISPATCHER_CREDIT_TABLE_4, 3, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_FPM_LOW_THRESHOLD", 1, CORE_4_INDEX, &DHD_FPM_LOW_THRESHOLD_4, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_FPM_HIGH_THRESHOLD", 1, CORE_4_INDEX, &DHD_FPM_HIGH_THRESHOLD_4, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "DUAL_STACK_LITE_TABLE", 1, CORE_4_INDEX, &DUAL_STACK_LITE_TABLE_4, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "REGISTERS_BUFFER", 1, CORE_4_INDEX, &REGISTERS_BUFFER_4, 32, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_TX_COMPLETE_FLOW_RING_BUFFER", 1, CORE_4_INDEX, &DHD_TX_COMPLETE_FLOW_RING_BUFFER_4, 3, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_DOORBELL_VALUE", 1, CORE_4_INDEX, &DHD_DOORBELL_VALUE_4, 3, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_RX_POST_RING_SIZE", 1, CORE_4_INDEX, &DHD_RX_POST_RING_SIZE_4, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_RX_COMPLETE_RING_SIZE", 1, CORE_4_INDEX, &DHD_RX_COMPLETE_RING_SIZE_4, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "RUNNER_PROFILING_TRACE_BUFFER", 1, CORE_4_INDEX, &RUNNER_PROFILING_TRACE_BUFFER_4, 128, 1, 1 },
#endif
#if defined BCM6856
	{ "TCAM_IC_CMD_TABLE", 1, CORE_4_INDEX, &TCAM_IC_CMD_TABLE_4, 7, 16, 1 },
#endif
#if defined BCM6856
	{ "LOOPBACK_QUEUE_TABLE", 1, CORE_4_INDEX, &LOOPBACK_QUEUE_TABLE_4, 40, 1, 1 },
#endif
#if defined BCM6856
	{ "NULL_BUFFER", 1, CORE_4_INDEX, &NULL_BUFFER_4, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_FEED_RING_DESCRIPTOR_TABLE", 1, CORE_4_INDEX, &CPU_FEED_RING_DESCRIPTOR_TABLE_4, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "DS_PACKET_BUFFER", 1, CORE_4_INDEX, &DS_PACKET_BUFFER_4, 8, 1, 1 },
#endif
#if defined BCM6856
	{ "LAYER2_GRE_TUNNEL_TABLE", 1, CORE_4_INDEX, &LAYER2_GRE_TUNNEL_TABLE_4, 12, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_CPU_RECYCLE_RING_DESCRIPTOR_TABLE", 1, CORE_4_INDEX, &DHD_CPU_RECYCLE_RING_DESCRIPTOR_TABLE_4, 3, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR", 1, CORE_4_INDEX, &DHD_CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR_4, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "SYSTEM_CONFIGURATION", 1, CORE_4_INDEX, &SYSTEM_CONFIGURATION_4, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "RATE_LIMIT_OVERHEAD", 1, CORE_4_INDEX, &RATE_LIMIT_OVERHEAD_4, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_TX_COMPLETE_RING_SIZE", 1, CORE_4_INDEX, &DHD_TX_COMPLETE_RING_SIZE_4, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "FLOW_BASED_ACTION_PTR_TABLE", 1, CORE_4_INDEX, &FLOW_BASED_ACTION_PTR_TABLE_4, 17, 1, 1 },
#endif
#if defined BCM6856
	{ "RX_MIRRORING_CONFIGURATION", 1, CORE_4_INDEX, &RX_MIRRORING_CONFIGURATION_4, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "INGRESS_QOS_DONT_DROP_RATIO", 1, CORE_4_INDEX, &INGRESS_QOS_DONT_DROP_RATIO_4, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "INGRESS_QOS_WAN_UNTAGGED_PRIORITY", 1, CORE_4_INDEX, &INGRESS_QOS_WAN_UNTAGGED_PRIORITY_4, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "IC_DEBUG_COUNTER_MODE", 1, CORE_4_INDEX, &IC_DEBUG_COUNTER_MODE_4, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "IC_MCAST_ENABLE", 1, CORE_4_INDEX, &IC_MCAST_ENABLE_4, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "TUNNELS_PARSING_CFG", 1, CORE_4_INDEX, &TUNNELS_PARSING_CFG_4, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "FPM_GLOBAL_CFG", 1, CORE_4_INDEX, &FPM_GLOBAL_CFG_4, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_REDIRECT_MODE", 1, CORE_4_INDEX, &CPU_REDIRECT_MODE_4, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "IPTV_CLASSIFICATION_CFG_TABLE", 1, CORE_4_INDEX, &IPTV_CLASSIFICATION_CFG_TABLE_4, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "INGRESS_FILTER_1588_CFG", 1, CORE_4_INDEX, &INGRESS_FILTER_1588_CFG_4, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_COMPLETE_COMMON_RADIO_DATA", 1, CORE_4_INDEX, &DHD_COMPLETE_COMMON_RADIO_DATA_4, 3, 1, 1 },
#endif
#if defined BCM6856
	{ "IPTV_CFG_TABLE", 1, CORE_4_INDEX, &IPTV_CFG_TABLE_4, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "IPTV_DDR_CTX_TABLE_ADDRESS", 1, CORE_4_INDEX, &IPTV_DDR_CTX_TABLE_ADDRESS_4, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "TCAM_IC_CFG_TABLE", 1, CORE_4_INDEX, &TCAM_IC_CFG_TABLE_4, 6, 1, 1 },
#endif
#if defined BCM6856
	{ "RULE_BASED_ACTION_PTR_TABLE", 1, CORE_4_INDEX, &RULE_BASED_ACTION_PTR_TABLE_4, 17, 1, 1 },
#endif
#if defined BCM6856
	{ "NAT_CACHE_CFG", 1, CORE_4_INDEX, &NAT_CACHE_CFG_4, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "INGRESS_FILTER_CFG", 1, CORE_4_INDEX, &INGRESS_FILTER_CFG_4, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "TX_FLOW_TABLE", 1, CORE_4_INDEX, &TX_FLOW_TABLE_4, 320, 1, 1 },
#endif
#if defined BCM6856
	{ "IPTV_ACTION_PTR_TABLE", 1, CORE_4_INDEX, &IPTV_ACTION_PTR_TABLE_4, 17, 1, 1 },
#endif
#if defined BCM6856
	{ "VPORT_CFG_EX_TABLE", 1, CORE_4_INDEX, &VPORT_CFG_EX_TABLE_4, 40, 1, 1 },
#endif
#if defined BCM6856
	{ "VLAN_ACTION_GPE_HANDLER_PTR_TABLE", 1, CORE_4_INDEX, &VLAN_ACTION_GPE_HANDLER_PTR_TABLE_4, 17, 1, 1 },
#endif
#if defined BCM6856
	{ "VPORT_CFG_TABLE", 1, CORE_4_INDEX, &VPORT_CFG_TABLE_4, 40, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_FPM_REPLY", 1, CORE_4_INDEX, &DHD_FPM_REPLY_4, 24, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_HW_CFG", 1, CORE_4_INDEX, &DHD_HW_CFG_4, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "NATC_TBL_CFG", 1, CORE_4_INDEX, &NATC_TBL_CFG_4, 2, 1, 1 },
#endif
#if defined BCM6856
	{ "RX_FLOW_TABLE", 1, CORE_5_INDEX, &RX_FLOW_TABLE_5, 320, 1, 1 },
#endif
#if defined BCM6856
	{ "LAYER2_HEADER_COPY_MAPPING_TABLE", 1, CORE_5_INDEX, &LAYER2_HEADER_COPY_MAPPING_TABLE_5, 30, 1, 1 },
#endif
#if defined BCM6856
	{ "IPTV_CONFIGURATION_TABLE", 1, CORE_5_INDEX, &IPTV_CONFIGURATION_TABLE_5, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "DSCP_TO_PBITS_MAP_TABLE", 1, CORE_5_INDEX, &DSCP_TO_PBITS_MAP_TABLE_5, 4, 64, 1 },
#endif
#if defined BCM6856
	{ "TC_TO_QUEUE_TABLE", 1, CORE_5_INDEX, &TC_TO_QUEUE_TABLE_5, 65, 1, 1 },
#endif
#if defined BCM6856
	{ "GENERAL_QUEUE_DYNAMIC_MNG_TABLE", 1, CORE_5_INDEX, &GENERAL_QUEUE_DYNAMIC_MNG_TABLE_5, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "FPM_POOL_NUMBER_MAPPING_TABLE", 1, CORE_5_INDEX, &FPM_POOL_NUMBER_MAPPING_TABLE_5, 16, 1, 1 },
#endif
#if defined BCM6856
	{ "RUNNER_GLOBAL_REGISTERS_INIT", 1, CORE_5_INDEX, &RUNNER_GLOBAL_REGISTERS_INIT_5, 8, 1, 1 },
#endif
#if defined BCM6856
	{ "GLOBAL_DSCP_TO_PBITS_TABLE", 1, CORE_5_INDEX, &GLOBAL_DSCP_TO_PBITS_TABLE_5, 1, 64, 1 },
#endif
#if defined BCM6856
	{ "POLICER_PARAMS_TABLE", 1, CORE_5_INDEX, &POLICER_PARAMS_TABLE_5, 80, 1, 1 },
#endif
#if defined BCM6856
	{ "INGRESS_FILTER_L2_REASON_TABLE", 1, CORE_5_INDEX, &INGRESS_FILTER_L2_REASON_TABLE_5, 16, 1, 1 },
#endif
#if defined BCM6856
	{ "VPORT_TO_DSCP_TO_PBITS_TABLE", 1, CORE_5_INDEX, &VPORT_TO_DSCP_TO_PBITS_TABLE_5, 32, 1, 1 },
#endif
#if defined BCM6856
	{ "INGRESS_FILTER_PROFILE_TABLE", 1, CORE_5_INDEX, &INGRESS_FILTER_PROFILE_TABLE_5, 16, 1, 1 },
#endif
#if defined BCM6856
	{ "PBIT_TO_GEM_TABLE", 1, CORE_5_INDEX, &PBIT_TO_GEM_TABLE_5, 8, 8, 1 },
#endif
#if defined BCM6856
	{ "DUAL_STACK_LITE_TABLE", 1, CORE_5_INDEX, &DUAL_STACK_LITE_TABLE_5, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "PBIT_TO_QUEUE_TABLE", 1, CORE_5_INDEX, &PBIT_TO_QUEUE_TABLE_5, 65, 1, 1 },
#endif
#if defined BCM6856
	{ "REGISTERS_BUFFER", 1, CORE_5_INDEX, &REGISTERS_BUFFER_5, 32, 1, 1 },
#endif
#if defined BCM6856
	{ "PROCESSING_QUEUE_DYNAMIC_MNG_TABLE", 1, CORE_5_INDEX, &PROCESSING_QUEUE_DYNAMIC_MNG_TABLE_5, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "RX_FLOW_CONTEXT_DDR_ADDR", 1, CORE_5_INDEX, &RX_FLOW_CONTEXT_DDR_ADDR_5, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "NULL_BUFFER", 1, CORE_5_INDEX, &NULL_BUFFER_5, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "TCAM_IC_CFG_TABLE", 1, CORE_5_INDEX, &TCAM_IC_CFG_TABLE_5, 6, 1, 1 },
#endif
#if defined BCM6856
	{ "NATC_TBL_CFG", 1, CORE_5_INDEX, &NATC_TBL_CFG_5, 2, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_FEED_RING_DESCRIPTOR_TABLE", 1, CORE_5_INDEX, &CPU_FEED_RING_DESCRIPTOR_TABLE_5, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "LOOPBACK_QUEUE_TABLE", 1, CORE_5_INDEX, &LOOPBACK_QUEUE_TABLE_5, 40, 1, 1 },
#endif
#if defined BCM6856
	{ "SYSTEM_CONFIGURATION", 1, CORE_5_INDEX, &SYSTEM_CONFIGURATION_5, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "FORCE_IC_IP_FLOW", 1, CORE_5_INDEX, &FORCE_IC_IP_FLOW_5, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "FORCE_DSCP", 1, CORE_5_INDEX, &FORCE_DSCP_5, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "CORE_ID_TABLE", 1, CORE_5_INDEX, &CORE_ID_TABLE_5, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "FPM_GLOBAL_CFG", 1, CORE_5_INDEX, &FPM_GLOBAL_CFG_5, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "TASK_IDX", 1, CORE_5_INDEX, &TASK_IDX_5, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "VPORT_CFG_EX_TABLE", 1, CORE_5_INDEX, &VPORT_CFG_EX_TABLE_5, 40, 1, 1 },
#endif
#if defined BCM6856
	{ "TUNNELS_PARSING_CFG", 1, CORE_5_INDEX, &TUNNELS_PARSING_CFG_5, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "IPTV_CFG_TABLE", 1, CORE_5_INDEX, &IPTV_CFG_TABLE_5, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "IPTV_DDR_CTX_TABLE_ADDRESS", 1, CORE_5_INDEX, &IPTV_DDR_CTX_TABLE_ADDRESS_5, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "FLOW_BASED_ACTION_PTR_TABLE", 1, CORE_5_INDEX, &FLOW_BASED_ACTION_PTR_TABLE_5, 17, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_FPM_LOW_THRESHOLD", 1, CORE_5_INDEX, &DHD_FPM_LOW_THRESHOLD_5, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_FPM_HIGH_THRESHOLD", 1, CORE_5_INDEX, &DHD_FPM_HIGH_THRESHOLD_5, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "RX_MIRRORING_CONFIGURATION", 1, CORE_5_INDEX, &RX_MIRRORING_CONFIGURATION_5, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "NAT_CACHE_CFG", 1, CORE_5_INDEX, &NAT_CACHE_CFG_5, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "SRAM_DUMMY_STORE", 1, CORE_5_INDEX, &SRAM_DUMMY_STORE_5, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "INGRESS_FILTER_CFG", 1, CORE_5_INDEX, &INGRESS_FILTER_CFG_5, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "RATE_LIMIT_OVERHEAD", 1, CORE_5_INDEX, &RATE_LIMIT_OVERHEAD_5, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "INGRESS_QOS_DONT_DROP_RATIO", 1, CORE_5_INDEX, &INGRESS_QOS_DONT_DROP_RATIO_5, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "INGRESS_QOS_WAN_UNTAGGED_PRIORITY", 1, CORE_5_INDEX, &INGRESS_QOS_WAN_UNTAGGED_PRIORITY_5, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "IC_DEBUG_COUNTER_MODE", 1, CORE_5_INDEX, &IC_DEBUG_COUNTER_MODE_5, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "IC_MCAST_ENABLE", 1, CORE_5_INDEX, &IC_MCAST_ENABLE_5, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_REDIRECT_MODE", 1, CORE_5_INDEX, &CPU_REDIRECT_MODE_5, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "IPTV_CLASSIFICATION_CFG_TABLE", 1, CORE_5_INDEX, &IPTV_CLASSIFICATION_CFG_TABLE_5, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "INGRESS_FILTER_1588_CFG", 1, CORE_5_INDEX, &INGRESS_FILTER_1588_CFG_5, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "RUNNER_PROFILING_TRACE_BUFFER", 1, CORE_5_INDEX, &RUNNER_PROFILING_TRACE_BUFFER_5, 128, 1, 1 },
#endif
#if defined BCM6856
	{ "TCAM_IC_CMD_TABLE", 1, CORE_5_INDEX, &TCAM_IC_CMD_TABLE_5, 7, 16, 1 },
#endif
#if defined BCM6856
	{ "RULE_BASED_ACTION_PTR_TABLE", 1, CORE_5_INDEX, &RULE_BASED_ACTION_PTR_TABLE_5, 17, 1, 1 },
#endif
#if defined BCM6856
	{ "DS_PACKET_BUFFER", 1, CORE_5_INDEX, &DS_PACKET_BUFFER_5, 8, 1, 1 },
#endif
#if defined BCM6856
	{ "LAYER2_GRE_TUNNEL_TABLE", 1, CORE_5_INDEX, &LAYER2_GRE_TUNNEL_TABLE_5, 12, 1, 1 },
#endif
#if defined BCM6856
	{ "IPTV_ACTION_PTR_TABLE", 1, CORE_5_INDEX, &IPTV_ACTION_PTR_TABLE_5, 17, 1, 1 },
#endif
#if defined BCM6856
	{ "VLAN_ACTION_GPE_HANDLER_PTR_TABLE", 1, CORE_5_INDEX, &VLAN_ACTION_GPE_HANDLER_PTR_TABLE_5, 17, 1, 1 },
#endif
#if defined BCM6856
	{ "TX_FLOW_TABLE", 1, CORE_5_INDEX, &TX_FLOW_TABLE_5, 320, 1, 1 },
#endif
#if defined BCM6856
	{ "TCAM_IC_HANDLER_TABLE", 1, CORE_5_INDEX, &TCAM_IC_HANDLER_TABLE_5, 16, 1, 1 },
#endif
#if defined BCM6856
	{ "BRIDGE_CFG_TABLE", 1, CORE_5_INDEX, &BRIDGE_CFG_TABLE_5, 3, 1, 1 },
#endif
#if defined BCM6856
	{ "VPORT_CFG_TABLE", 1, CORE_5_INDEX, &VPORT_CFG_TABLE_5, 40, 1, 1 },
#endif
#if defined BCM6856
	{ "RX_FLOW_TABLE", 1, CORE_6_INDEX, &RX_FLOW_TABLE_6, 320, 1, 1 },
#endif
#if defined BCM6856
	{ "LAYER2_HEADER_COPY_MAPPING_TABLE", 1, CORE_6_INDEX, &LAYER2_HEADER_COPY_MAPPING_TABLE_6, 30, 1, 1 },
#endif
#if defined BCM6856
	{ "IPTV_CONFIGURATION_TABLE", 1, CORE_6_INDEX, &IPTV_CONFIGURATION_TABLE_6, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "DSCP_TO_PBITS_MAP_TABLE", 1, CORE_6_INDEX, &DSCP_TO_PBITS_MAP_TABLE_6, 4, 64, 1 },
#endif
#if defined BCM6856
	{ "TC_TO_QUEUE_TABLE", 1, CORE_6_INDEX, &TC_TO_QUEUE_TABLE_6, 65, 1, 1 },
#endif
#if defined BCM6856
	{ "GENERAL_QUEUE_DYNAMIC_MNG_TABLE", 1, CORE_6_INDEX, &GENERAL_QUEUE_DYNAMIC_MNG_TABLE_6, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "FPM_POOL_NUMBER_MAPPING_TABLE", 1, CORE_6_INDEX, &FPM_POOL_NUMBER_MAPPING_TABLE_6, 16, 1, 1 },
#endif
#if defined BCM6856
	{ "RUNNER_GLOBAL_REGISTERS_INIT", 1, CORE_6_INDEX, &RUNNER_GLOBAL_REGISTERS_INIT_6, 8, 1, 1 },
#endif
#if defined BCM6856
	{ "GLOBAL_DSCP_TO_PBITS_TABLE", 1, CORE_6_INDEX, &GLOBAL_DSCP_TO_PBITS_TABLE_6, 1, 64, 1 },
#endif
#if defined BCM6856
	{ "POLICER_PARAMS_TABLE", 1, CORE_6_INDEX, &POLICER_PARAMS_TABLE_6, 80, 1, 1 },
#endif
#if defined BCM6856
	{ "INGRESS_FILTER_L2_REASON_TABLE", 1, CORE_6_INDEX, &INGRESS_FILTER_L2_REASON_TABLE_6, 16, 1, 1 },
#endif
#if defined BCM6856
	{ "VPORT_TO_DSCP_TO_PBITS_TABLE", 1, CORE_6_INDEX, &VPORT_TO_DSCP_TO_PBITS_TABLE_6, 32, 1, 1 },
#endif
#if defined BCM6856
	{ "INGRESS_FILTER_PROFILE_TABLE", 1, CORE_6_INDEX, &INGRESS_FILTER_PROFILE_TABLE_6, 16, 1, 1 },
#endif
#if defined BCM6856
	{ "PBIT_TO_GEM_TABLE", 1, CORE_6_INDEX, &PBIT_TO_GEM_TABLE_6, 8, 8, 1 },
#endif
#if defined BCM6856
	{ "DUAL_STACK_LITE_TABLE", 1, CORE_6_INDEX, &DUAL_STACK_LITE_TABLE_6, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "PBIT_TO_QUEUE_TABLE", 1, CORE_6_INDEX, &PBIT_TO_QUEUE_TABLE_6, 65, 1, 1 },
#endif
#if defined BCM6856
	{ "REGISTERS_BUFFER", 1, CORE_6_INDEX, &REGISTERS_BUFFER_6, 32, 1, 1 },
#endif
#if defined BCM6856
	{ "PROCESSING_QUEUE_DYNAMIC_MNG_TABLE", 1, CORE_6_INDEX, &PROCESSING_QUEUE_DYNAMIC_MNG_TABLE_6, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "RX_FLOW_CONTEXT_DDR_ADDR", 1, CORE_6_INDEX, &RX_FLOW_CONTEXT_DDR_ADDR_6, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "NULL_BUFFER", 1, CORE_6_INDEX, &NULL_BUFFER_6, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "TCAM_IC_CFG_TABLE", 1, CORE_6_INDEX, &TCAM_IC_CFG_TABLE_6, 6, 1, 1 },
#endif
#if defined BCM6856
	{ "NATC_TBL_CFG", 1, CORE_6_INDEX, &NATC_TBL_CFG_6, 2, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_FEED_RING_DESCRIPTOR_TABLE", 1, CORE_6_INDEX, &CPU_FEED_RING_DESCRIPTOR_TABLE_6, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "LOOPBACK_QUEUE_TABLE", 1, CORE_6_INDEX, &LOOPBACK_QUEUE_TABLE_6, 40, 1, 1 },
#endif
#if defined BCM6856
	{ "SYSTEM_CONFIGURATION", 1, CORE_6_INDEX, &SYSTEM_CONFIGURATION_6, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "FORCE_IC_IP_FLOW", 1, CORE_6_INDEX, &FORCE_IC_IP_FLOW_6, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "FORCE_DSCP", 1, CORE_6_INDEX, &FORCE_DSCP_6, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "CORE_ID_TABLE", 1, CORE_6_INDEX, &CORE_ID_TABLE_6, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "FPM_GLOBAL_CFG", 1, CORE_6_INDEX, &FPM_GLOBAL_CFG_6, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "TASK_IDX", 1, CORE_6_INDEX, &TASK_IDX_6, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "VPORT_CFG_EX_TABLE", 1, CORE_6_INDEX, &VPORT_CFG_EX_TABLE_6, 40, 1, 1 },
#endif
#if defined BCM6856
	{ "TUNNELS_PARSING_CFG", 1, CORE_6_INDEX, &TUNNELS_PARSING_CFG_6, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "IPTV_CFG_TABLE", 1, CORE_6_INDEX, &IPTV_CFG_TABLE_6, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "IPTV_DDR_CTX_TABLE_ADDRESS", 1, CORE_6_INDEX, &IPTV_DDR_CTX_TABLE_ADDRESS_6, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "FLOW_BASED_ACTION_PTR_TABLE", 1, CORE_6_INDEX, &FLOW_BASED_ACTION_PTR_TABLE_6, 17, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_FPM_LOW_THRESHOLD", 1, CORE_6_INDEX, &DHD_FPM_LOW_THRESHOLD_6, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_FPM_HIGH_THRESHOLD", 1, CORE_6_INDEX, &DHD_FPM_HIGH_THRESHOLD_6, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "RX_MIRRORING_CONFIGURATION", 1, CORE_6_INDEX, &RX_MIRRORING_CONFIGURATION_6, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "NAT_CACHE_CFG", 1, CORE_6_INDEX, &NAT_CACHE_CFG_6, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "SRAM_DUMMY_STORE", 1, CORE_6_INDEX, &SRAM_DUMMY_STORE_6, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "INGRESS_FILTER_CFG", 1, CORE_6_INDEX, &INGRESS_FILTER_CFG_6, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "RATE_LIMIT_OVERHEAD", 1, CORE_6_INDEX, &RATE_LIMIT_OVERHEAD_6, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "INGRESS_QOS_DONT_DROP_RATIO", 1, CORE_6_INDEX, &INGRESS_QOS_DONT_DROP_RATIO_6, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "INGRESS_QOS_WAN_UNTAGGED_PRIORITY", 1, CORE_6_INDEX, &INGRESS_QOS_WAN_UNTAGGED_PRIORITY_6, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "IC_DEBUG_COUNTER_MODE", 1, CORE_6_INDEX, &IC_DEBUG_COUNTER_MODE_6, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "IC_MCAST_ENABLE", 1, CORE_6_INDEX, &IC_MCAST_ENABLE_6, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_REDIRECT_MODE", 1, CORE_6_INDEX, &CPU_REDIRECT_MODE_6, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "IPTV_CLASSIFICATION_CFG_TABLE", 1, CORE_6_INDEX, &IPTV_CLASSIFICATION_CFG_TABLE_6, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "INGRESS_FILTER_1588_CFG", 1, CORE_6_INDEX, &INGRESS_FILTER_1588_CFG_6, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "RUNNER_PROFILING_TRACE_BUFFER", 1, CORE_6_INDEX, &RUNNER_PROFILING_TRACE_BUFFER_6, 128, 1, 1 },
#endif
#if defined BCM6856
	{ "TCAM_IC_CMD_TABLE", 1, CORE_6_INDEX, &TCAM_IC_CMD_TABLE_6, 7, 16, 1 },
#endif
#if defined BCM6856
	{ "RULE_BASED_ACTION_PTR_TABLE", 1, CORE_6_INDEX, &RULE_BASED_ACTION_PTR_TABLE_6, 17, 1, 1 },
#endif
#if defined BCM6856
	{ "DS_PACKET_BUFFER", 1, CORE_6_INDEX, &DS_PACKET_BUFFER_6, 8, 1, 1 },
#endif
#if defined BCM6856
	{ "LAYER2_GRE_TUNNEL_TABLE", 1, CORE_6_INDEX, &LAYER2_GRE_TUNNEL_TABLE_6, 12, 1, 1 },
#endif
#if defined BCM6856
	{ "IPTV_ACTION_PTR_TABLE", 1, CORE_6_INDEX, &IPTV_ACTION_PTR_TABLE_6, 17, 1, 1 },
#endif
#if defined BCM6856
	{ "VLAN_ACTION_GPE_HANDLER_PTR_TABLE", 1, CORE_6_INDEX, &VLAN_ACTION_GPE_HANDLER_PTR_TABLE_6, 17, 1, 1 },
#endif
#if defined BCM6856
	{ "TX_FLOW_TABLE", 1, CORE_6_INDEX, &TX_FLOW_TABLE_6, 320, 1, 1 },
#endif
#if defined BCM6856
	{ "TCAM_IC_HANDLER_TABLE", 1, CORE_6_INDEX, &TCAM_IC_HANDLER_TABLE_6, 16, 1, 1 },
#endif
#if defined BCM6856
	{ "BRIDGE_CFG_TABLE", 1, CORE_6_INDEX, &BRIDGE_CFG_TABLE_6, 3, 1, 1 },
#endif
#if defined BCM6856
	{ "VPORT_CFG_TABLE", 1, CORE_6_INDEX, &VPORT_CFG_TABLE_6, 40, 1, 1 },
#endif
#if defined BCM6856
	{ "RX_FLOW_TABLE", 1, CORE_7_INDEX, &RX_FLOW_TABLE_7, 320, 1, 1 },
#endif
#if defined BCM6856
	{ "LAYER2_HEADER_COPY_MAPPING_TABLE", 1, CORE_7_INDEX, &LAYER2_HEADER_COPY_MAPPING_TABLE_7, 30, 1, 1 },
#endif
#if defined BCM6856
	{ "IPTV_CONFIGURATION_TABLE", 1, CORE_7_INDEX, &IPTV_CONFIGURATION_TABLE_7, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "DSCP_TO_PBITS_MAP_TABLE", 1, CORE_7_INDEX, &DSCP_TO_PBITS_MAP_TABLE_7, 4, 64, 1 },
#endif
#if defined BCM6856
	{ "TC_TO_QUEUE_TABLE", 1, CORE_7_INDEX, &TC_TO_QUEUE_TABLE_7, 65, 1, 1 },
#endif
#if defined BCM6856
	{ "GENERAL_QUEUE_DYNAMIC_MNG_TABLE", 1, CORE_7_INDEX, &GENERAL_QUEUE_DYNAMIC_MNG_TABLE_7, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "FPM_POOL_NUMBER_MAPPING_TABLE", 1, CORE_7_INDEX, &FPM_POOL_NUMBER_MAPPING_TABLE_7, 16, 1, 1 },
#endif
#if defined BCM6856
	{ "RUNNER_GLOBAL_REGISTERS_INIT", 1, CORE_7_INDEX, &RUNNER_GLOBAL_REGISTERS_INIT_7, 8, 1, 1 },
#endif
#if defined BCM6856
	{ "GLOBAL_DSCP_TO_PBITS_TABLE", 1, CORE_7_INDEX, &GLOBAL_DSCP_TO_PBITS_TABLE_7, 1, 64, 1 },
#endif
#if defined BCM6856
	{ "POLICER_PARAMS_TABLE", 1, CORE_7_INDEX, &POLICER_PARAMS_TABLE_7, 80, 1, 1 },
#endif
#if defined BCM6856
	{ "INGRESS_FILTER_L2_REASON_TABLE", 1, CORE_7_INDEX, &INGRESS_FILTER_L2_REASON_TABLE_7, 16, 1, 1 },
#endif
#if defined BCM6856
	{ "VPORT_TO_DSCP_TO_PBITS_TABLE", 1, CORE_7_INDEX, &VPORT_TO_DSCP_TO_PBITS_TABLE_7, 32, 1, 1 },
#endif
#if defined BCM6856
	{ "INGRESS_FILTER_PROFILE_TABLE", 1, CORE_7_INDEX, &INGRESS_FILTER_PROFILE_TABLE_7, 16, 1, 1 },
#endif
#if defined BCM6856
	{ "PBIT_TO_GEM_TABLE", 1, CORE_7_INDEX, &PBIT_TO_GEM_TABLE_7, 8, 8, 1 },
#endif
#if defined BCM6856
	{ "DUAL_STACK_LITE_TABLE", 1, CORE_7_INDEX, &DUAL_STACK_LITE_TABLE_7, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "PBIT_TO_QUEUE_TABLE", 1, CORE_7_INDEX, &PBIT_TO_QUEUE_TABLE_7, 65, 1, 1 },
#endif
#if defined BCM6856
	{ "REGISTERS_BUFFER", 1, CORE_7_INDEX, &REGISTERS_BUFFER_7, 32, 1, 1 },
#endif
#if defined BCM6856
	{ "PROCESSING_QUEUE_DYNAMIC_MNG_TABLE", 1, CORE_7_INDEX, &PROCESSING_QUEUE_DYNAMIC_MNG_TABLE_7, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "RX_FLOW_CONTEXT_DDR_ADDR", 1, CORE_7_INDEX, &RX_FLOW_CONTEXT_DDR_ADDR_7, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "NULL_BUFFER", 1, CORE_7_INDEX, &NULL_BUFFER_7, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "TCAM_IC_CFG_TABLE", 1, CORE_7_INDEX, &TCAM_IC_CFG_TABLE_7, 6, 1, 1 },
#endif
#if defined BCM6856
	{ "NATC_TBL_CFG", 1, CORE_7_INDEX, &NATC_TBL_CFG_7, 2, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_FEED_RING_DESCRIPTOR_TABLE", 1, CORE_7_INDEX, &CPU_FEED_RING_DESCRIPTOR_TABLE_7, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "LOOPBACK_QUEUE_TABLE", 1, CORE_7_INDEX, &LOOPBACK_QUEUE_TABLE_7, 40, 1, 1 },
#endif
#if defined BCM6856
	{ "SYSTEM_CONFIGURATION", 1, CORE_7_INDEX, &SYSTEM_CONFIGURATION_7, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "FORCE_IC_IP_FLOW", 1, CORE_7_INDEX, &FORCE_IC_IP_FLOW_7, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "FORCE_DSCP", 1, CORE_7_INDEX, &FORCE_DSCP_7, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "CORE_ID_TABLE", 1, CORE_7_INDEX, &CORE_ID_TABLE_7, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "FPM_GLOBAL_CFG", 1, CORE_7_INDEX, &FPM_GLOBAL_CFG_7, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "TASK_IDX", 1, CORE_7_INDEX, &TASK_IDX_7, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "VPORT_CFG_EX_TABLE", 1, CORE_7_INDEX, &VPORT_CFG_EX_TABLE_7, 40, 1, 1 },
#endif
#if defined BCM6856
	{ "TUNNELS_PARSING_CFG", 1, CORE_7_INDEX, &TUNNELS_PARSING_CFG_7, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "IPTV_CFG_TABLE", 1, CORE_7_INDEX, &IPTV_CFG_TABLE_7, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "IPTV_DDR_CTX_TABLE_ADDRESS", 1, CORE_7_INDEX, &IPTV_DDR_CTX_TABLE_ADDRESS_7, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "FLOW_BASED_ACTION_PTR_TABLE", 1, CORE_7_INDEX, &FLOW_BASED_ACTION_PTR_TABLE_7, 17, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_FPM_LOW_THRESHOLD", 1, CORE_7_INDEX, &DHD_FPM_LOW_THRESHOLD_7, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "DHD_FPM_HIGH_THRESHOLD", 1, CORE_7_INDEX, &DHD_FPM_HIGH_THRESHOLD_7, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "RX_MIRRORING_CONFIGURATION", 1, CORE_7_INDEX, &RX_MIRRORING_CONFIGURATION_7, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "NAT_CACHE_CFG", 1, CORE_7_INDEX, &NAT_CACHE_CFG_7, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "SRAM_DUMMY_STORE", 1, CORE_7_INDEX, &SRAM_DUMMY_STORE_7, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "INGRESS_FILTER_CFG", 1, CORE_7_INDEX, &INGRESS_FILTER_CFG_7, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "RATE_LIMIT_OVERHEAD", 1, CORE_7_INDEX, &RATE_LIMIT_OVERHEAD_7, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "INGRESS_QOS_DONT_DROP_RATIO", 1, CORE_7_INDEX, &INGRESS_QOS_DONT_DROP_RATIO_7, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "INGRESS_QOS_WAN_UNTAGGED_PRIORITY", 1, CORE_7_INDEX, &INGRESS_QOS_WAN_UNTAGGED_PRIORITY_7, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "IC_DEBUG_COUNTER_MODE", 1, CORE_7_INDEX, &IC_DEBUG_COUNTER_MODE_7, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "IC_MCAST_ENABLE", 1, CORE_7_INDEX, &IC_MCAST_ENABLE_7, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "CPU_REDIRECT_MODE", 1, CORE_7_INDEX, &CPU_REDIRECT_MODE_7, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "IPTV_CLASSIFICATION_CFG_TABLE", 1, CORE_7_INDEX, &IPTV_CLASSIFICATION_CFG_TABLE_7, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "INGRESS_FILTER_1588_CFG", 1, CORE_7_INDEX, &INGRESS_FILTER_1588_CFG_7, 1, 1, 1 },
#endif
#if defined BCM6856
	{ "RUNNER_PROFILING_TRACE_BUFFER", 1, CORE_7_INDEX, &RUNNER_PROFILING_TRACE_BUFFER_7, 128, 1, 1 },
#endif
#if defined BCM6856
	{ "TCAM_IC_CMD_TABLE", 1, CORE_7_INDEX, &TCAM_IC_CMD_TABLE_7, 7, 16, 1 },
#endif
#if defined BCM6856
	{ "RULE_BASED_ACTION_PTR_TABLE", 1, CORE_7_INDEX, &RULE_BASED_ACTION_PTR_TABLE_7, 17, 1, 1 },
#endif
#if defined BCM6856
	{ "DS_PACKET_BUFFER", 1, CORE_7_INDEX, &DS_PACKET_BUFFER_7, 8, 1, 1 },
#endif
#if defined BCM6856
	{ "LAYER2_GRE_TUNNEL_TABLE", 1, CORE_7_INDEX, &LAYER2_GRE_TUNNEL_TABLE_7, 12, 1, 1 },
#endif
#if defined BCM6856
	{ "IPTV_ACTION_PTR_TABLE", 1, CORE_7_INDEX, &IPTV_ACTION_PTR_TABLE_7, 17, 1, 1 },
#endif
#if defined BCM6856
	{ "VLAN_ACTION_GPE_HANDLER_PTR_TABLE", 1, CORE_7_INDEX, &VLAN_ACTION_GPE_HANDLER_PTR_TABLE_7, 17, 1, 1 },
#endif
#if defined BCM6856
	{ "TX_FLOW_TABLE", 1, CORE_7_INDEX, &TX_FLOW_TABLE_7, 320, 1, 1 },
#endif
#if defined BCM6856
	{ "TCAM_IC_HANDLER_TABLE", 1, CORE_7_INDEX, &TCAM_IC_HANDLER_TABLE_7, 16, 1, 1 },
#endif
#if defined BCM6856
	{ "BRIDGE_CFG_TABLE", 1, CORE_7_INDEX, &BRIDGE_CFG_TABLE_7, 3, 1, 1 },
#endif
#if defined BCM6856
	{ "VPORT_CFG_TABLE", 1, CORE_7_INDEX, &VPORT_CFG_TABLE_7, 40, 1, 1 },
#endif
};

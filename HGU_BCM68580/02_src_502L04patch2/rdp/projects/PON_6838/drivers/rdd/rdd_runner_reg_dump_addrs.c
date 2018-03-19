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
#include "rdp_map.h"
#include "rdd_runner_reg_dump.h"
#include "rdd_runner_reg_dump_addrs.h"
unsigned int SEGMENTS_ADDRESSES[NUMBER_OF_SEGMENTS] =
{
#ifdef OREN
 0x0 + RUNNER_PRIVATE_0_OFFSET, 0x0 + RUNNER_PRIVATE_1_OFFSET, 0x0 + RUNNER_COMMON_0_OFFSET, 0x8000 + RUNNER_COMMON_1_OFFSET, 0x0, 0x0 + PSRAM_BLOCK_OFFSET,
#endif
#ifdef G9991
 0x0 + RUNNER_PRIVATE_0_OFFSET, 0x0 + RUNNER_PRIVATE_1_OFFSET, 0x0 + RUNNER_COMMON_0_OFFSET, 0x8000 + RUNNER_COMMON_1_OFFSET, 0x0, 0x0 + PSRAM_BLOCK_OFFSET,
#endif
};
#if defined OREN
static DUMP_RUNNERREG_STRUCT INGRESS_HANDLER_BUFFER =
{
	256,
	{
		{ dump_RDD_IH_BUFFER, 0x0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_FREE_PACKET_DESCRIPTORS_POOL =
{
	8,
	{
		{ dump_RDD_PACKET_DESCRIPTOR, 0x2000 },
		{ dump_RDD_BBH_TX_DESCRIPTOR, 0x2000 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT SERVICE_QUEUES_DDR_CACHE_FIFO =
{
	16,
	{
		{ dump_RDD_SIXTEEN_BYTES, 0x5000 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_INGRESS_CLASSIFICATION_CONTEXT_TABLE =
{
#if defined OREN
	8,
	{
		{ dump_RDD_DS_INGRESS_CLASSIFICATION_CONTEXT_ENTRY, 0x6800 },
#endif
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_INGRESS_FILTERS_LOOKUP_TABLE =
{
	4,
	{
		{ dump_RDD_INGRESS_FILTERS_LOOKUP_ENTRY, 0x7000 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT ETH_TX_RS_QUEUE_DESCRIPTOR_TABLE =
{
	16,
	{
		{ dump_RDD_ETH_TX_RS_QUEUE_DESCRIPTOR, 0x7500 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_PBITS_TO_PBITS_TABLE =
{
	1,
	{
		{ dump_RDD_PBITS_TO_PBITS_ENTRY, 0x7d00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_WAN_FLOW_TABLE =
{
	2,
	{
		{ dump_RDD_DS_WAN_FLOW_ENTRY, 0x7e00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT IPTV_COUNTERS_TABLE =
{
	2,
	{
		{ dump_RDD_IPTV_COUNTER_ENTRY, 0x8000 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT ETH_TX_LOCAL_REGISTERS =
{
	8,
	{
		{ dump_RDD_ETH_TX_LOCAL_REGISTERS_ENTRY, 0x8240 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_QUEUE_PROFILE_TABLE =
{
	16,
	{
		{ dump_RDD_QUEUE_PROFILE, 0x8280 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_PBITS_PARAMETER_TABLE =
{
	2,
	{
		{ dump_RDD_PBITS_PARAMETER_ENTRY, 0x8300 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT IPTV_SSID_EXTENSION_TABLE =
{
	2,
	{
		{ dump_RDD_IPTV_SSID_EXTENSION_ENTRY, 0x8400 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_VLAN_PARAMETER_TABLE =
{
	4,
	{
		{ dump_RDD_VLAN_PARAMETER_ENTRY, 0x8600 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_CPU_REASON_TO_METER_TABLE =
{
	1,
	{
		{ dump_RDD_CPU_REASON_TO_METER_ENTRY, 0x8800 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT IPTV_INGRESS_CLASSIFICATION_CONTEXT_TABLE =
{
	1,
	{
		{ dump_RDD_IPTV_INGRESS_CLASSIFICATION_CONTEXT_ENTRY, 0x8840 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_MAIN_TIMER_TASK_DESCRIPTOR_TABLE =
{
	8,
	{
		{ dump_RDD_TIMER_TASK_DESCRIPTOR_ENTRY, 0x8960 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT SBPM_REPLY =
{
	128,
	{
		{ dump_RDD_SBPM_REPLY_ENTRY, 0x8980 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_CONNECTION_CONTEXT_BUFFER =
{
	64,
	{
		{ dump_RDD_CONNECTION_CONTEXT_BUFFER_ENTRY, 0x8a00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_FWD_TABLE =
{
	8,
	{
		{ dump_RDD_WLAN_MCAST_FWD_ENTRY, 0x8c00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT ETH_TX_QUEUES_TABLE =
{
	16,
	{
		{ dump_RDD_ETH_TX_QUEUE_DESCRIPTOR, 0x8e00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_LAN_VID_TABLE =
{
	2,
	{
		{ dump_RDD_VID_ENTRY, 0x9100 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_PBITS_PRIMITIVE_TABLE =
{
	2,
	{
		{ dump_RDD_PBITS_PRIMITIVE_ENTRY, 0x9200 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_FAST_CPU_TX_DESCRIPTOR_ABS_DATA_PTR_QUEUE =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0x9240 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_PICO_CPU_TX_DESCRIPTOR_ABS_DATA_PTR_QUEUE =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0x9280 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT RATE_LIMITER_REMAINDER_TABLE =
{
	2,
	{
		{ dump_RDD_RATE_LIMITER_REMAINDER_ENTRY, 0x92c0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_RATE_LIMITER_TABLE =
{
	8,
	{
		{ dump_RDD_RATE_LIMITER_ENTRY, 0x9300 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT SERVICE_QUEUES_RATE_LIMITER_TABLE =
{
	24,
	{
		{ dump_RDD_SERVICE_QUEUES_RATE_LIMITER_DESCRIPTOR, 0x9400 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_VLAN_COMMANDS_TABLE =
{
	8,
	{
		{ dump_RDD_VLAN_COMMAND_ENRTY, 0x9700 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_POLICER_TABLE =
{
	16,
	{
		{ dump_RDD_POLICER_ENTRY, 0x9900 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_CPU_TX_BBH_DESCRIPTORS =
{
	8,
	{
		{ dump_RDD_CPU_TX_BBH_DESCRIPTORS_ENTRY, 0x9a00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT ETH_TX_QUEUES_POINTERS_TABLE =
{
	4,
	{
		{ dump_RDD_ETH_TX_QUEUE_POINTERS_ENTRY, 0x9b00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_INGRESS_CLASSIFICATION_KEY_PRIMITIVE_TABLE =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0x9bc0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT CPU_RX_PD_INGRESS_QUEUE =
{
	8,
	{
		{ dump_RDD_EIGHT_BYTES, 0x9c00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_INGRESS_FILTERS_PARAMETER_TABLE =
{
	1,
	{
		{ dump_RDD_INGRESS_FILTERS_PARAMETER_ENTRY, 0x9d00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_PICO_TIMER_TASK_DESCRIPTOR_TABLE =
{
	8,
	{
		{ dump_RDD_TIMER_TASK_DESCRIPTOR_ENTRY, 0x9da0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT IPTV_SSID_EXTENSION_TABLE_CAM =
{
	64,
	{
		{ dump_RDD_IPTV_SSID_EXTENSION_TABLE_CAM, 0x9dc0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT CPU_RX_FAST_PD_INGRESS_QUEUE =
{
	8,
	{
		{ dump_RDD_EIGHT_BYTES, 0x9e00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_CONTROL_TABLE =
{
	148,
	{
		{ dump_RDD_WLAN_MCAST_CONTROL_ENTRY, 0x9f00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT BPM_DDR_OPTIMIZED_BUFFERS_WITHOUT_HEADROOM_BASE =
{
	4,
	{
		{ dump_RDD_BPM_DDR_OPTIMIZED_BUFFERS_WITHOUT_HEADROOM_BASE, 0x9f94 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_CPU_PARAMETERS_BLOCK =
{
	8,
	{
		{ dump_RDD_CPU_PARAMETERS_BLOCK_ENTRY, 0x9f98 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_INGRESS_CLASSIFICATION_GENERIC_RULE_CFG_TABLE =
{
	8,
	{
		{ dump_RDD_INGRESS_CLASSIFICATION_GENERIC_RULE_CFG_ENTRY, 0x9fa0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_FLOW_BASED_ACTION_PTR_TABLE =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0x9fc0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DHD_TX_POST_PD_INGRESS_QUEUE =
{
	8,
	{
		{ dump_RDD_EIGHT_BYTES, 0xa000 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_FORWARDING_MATRIX_TABLE =
{
	1,
	{
		{ dump_RDD_FORWARDING_MATRIX_ENTRY, 0xa100 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_TPID_OVERWRITE_TABLE =
{
	2,
	{
		{ dump_RDD_TPID_OVERWRITE_ENTRY, 0xa190 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_FAST_RUNNER_GLOBAL_REGISTERS_INIT =
{
	4,
	{
		{ dump_RDD_FAST_RUNNER_GLOBAL_REGISTERS_INIT_ENTRY, 0xa1a0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_TRAFFIC_CLASS_TO_QUEUE_TABLE =
{
	1,
	{
		{ dump_RDD_QUEUE_ENTRY, 0xa1c0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_TIMER_SCHEDULER_PRIMITIVE_TABLE =
{
	2,
	{
		{ dump_RDD_TIMER_SCHEDULER_PRIMITIVE_ENTRY, 0xa1f0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_PD_INGRESS_QUEUE =
{
	8,
	{
		{ dump_RDD_EIGHT_BYTES, 0xa200 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_LAYER4_FILTERS_LOOKUP_TABLE =
{
	8,
	{
		{ dump_RDD_LAYER4_FILTERS_LOOKUP_ENTRY, 0xa300 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_CPU_RX_METER_TABLE =
{
	8,
	{
		{ dump_RDD_CPU_RX_METER_ENTRY, 0xa380 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_BRIDGE_CONFIGURATION_REGISTER =
{
	256,
	{
		{ dump_RDD_BRIDGE_CONFIGURATION_REGISTER, 0xa400 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT INGRESS_HANDLER_SKB_DATA_POINTER =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0xa500 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_INGRESS_CLASSIFICATION_RULE_CFG_TABLE =
{
	8,
	{
		{ dump_RDD_INGRESS_CLASSIFICATION_RULE_CFG_ENTRY, 0xa580 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_PROFILING_BUFFER_PICO_RUNNER =
{
	256,
	{
		{ dump_RDD_PROFILING_BUFFER_PICO_RUNNER, 0xa600 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DOWNSTREAM_MULTICAST_LAN_ENQUEUE_INGRESS_QUEUE =
{
	8,
	{
		{ dump_RDD_EIGHT_BYTES, 0xa700 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_VLAN_OPTIMIZATION_TABLE =
{
	1,
	{
		{ dump_RDD_VLAN_OPTIMIZATION_ENTRY, 0xa780 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DOWNSTREAM_MULTICAST_LAN_ENQUEUE_SERVICE_QUEUE =
{
	8,
	{
		{ dump_RDD_EIGHT_BYTES, 0xa800 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT PCI_MULTICAST_SCRATCHPAD =
{
	128,
	{
		{ dump_RDD_PCI_MULTICAST_SCRATCHPAD, 0xa880 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT CPU_RX_MIRRORING_PD_INGRESS_QUEUE =
{
	8,
	{
		{ dump_RDD_EIGHT_BYTES, 0xa900 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT EMAC_SKB_ENQUEUED_INDEXES_FIFO =
{
	32,
	{
		{ dump_RDD_EMAC_SKB_ENQUEUED_INDEXES_FIFO_ENTRY, 0xa980 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_PICO_RUNNER_GLOBAL_REGISTERS_INIT =
{
	4,
	{
		{ dump_RDD_PICO_RUNNER_GLOBAL_REGISTERS_INIT_ENTRY, 0xaa20 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DHD_TX_COMPLETE_FLOW_RING_BUFFER =
{
	16,
	{
		{ dump_RDD_DHD_TX_COMPLETE_DESCRIPTOR, 0xaa40 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT EMAC_SKB_ENQUEUED_INDEXES_PUT_PTR =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xaa70 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_RUNNER_FLOW_HEADER_BUFFER =
{
	128,
	{
		{ dump_RDD_RUNNER_FLOW_HEADER_BUFFER, 0xaa80 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DOWNSTREAM_LAN_ENQUEUE_INGRESS_QUEUE =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xab00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DHD_COMPLETE_RING_DESCRIPTOR_BUFFER =
{
	16,
	{
		{ dump_RDD_DHD_COMPLETE_RING_DESCRIPTOR, 0xab40 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT EMAC_SKB_ENQUEUED_INDEXES_FREE_PTR =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xab70 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DOWNSTREAM_LAN_ENQUEUE_SERVICE_QUEUE =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xab80 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_DSCP_TO_PBITS_TABLE =
{
	1,
	{
		{ dump_RDD_DSCP_TO_PBITS_ENTRY, 0xabc0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT CPU_TX_FAST_QUEUE =
{
	8,
	{
		{ dump_RDD_CPU_TX_DESCRIPTOR, 0xac00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_CORE, 0xac00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_BPM, 0xac00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_ABS, 0xac00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_DS_FAST, 0xac00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_US_FAST, 0xac00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_DS_PICO, 0xac00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_DS_PICO_WIFI, 0xac00 },
		{ dump_RDD_CPU_TX_MESSAGE_DESCRIPTOR, 0xac00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT PACKET_SRAM_TO_DDR_COPY_BUFFER =
{
	128,
	{
		{ dump_RDD_PACKET_SRAM_TO_DDR_COPY_BUFFER, 0xac80 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT CPU_TX_PICO_QUEUE =
{
	8,
	{
		{ dump_RDD_CPU_TX_DESCRIPTOR, 0xad00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_CORE, 0xad00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_BPM, 0xad00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_ABS, 0xad00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_DS_FAST, 0xad00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_US_FAST, 0xad00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_DS_PICO, 0xad00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_DS_PICO_WIFI, 0xad00 },
		{ dump_RDD_CPU_TX_MESSAGE_DESCRIPTOR, 0xad00 },
		{ dump_RDD_CPU_TX_DHD_DESCRIPTOR, 0xad00 },
		{ dump_RDD_CPU_TX_DHD_MESSAGE_DESCRIPTOR, 0xad00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_CPU_RX_PICO_INGRESS_QUEUE =
{
	1,
	{
		{ dump_RDD_INGRESS_QUEUE_ENTRY, 0xad80 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_VLAN_PRIMITIVE_TABLE =
{
	2,
	{
		{ dump_RDD_VLAN_PRIMITIVE_ENTRY, 0xadc0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT ETH_TX_MAC_TABLE =
{
#if defined OREN
	64,
	{
		{ dump_RDD_ETH_TX_MAC_DESCRIPTOR, 0xae00 },
#endif
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DOWNSTREAM_MULTICAST_INGRESS_QUEUE =
{
	1,
	{
		{ dump_RDD_INGRESS_QUEUE_ENTRY, 0xaf80 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_CPU_TX_MESSAGE_DATA_BUFFER =
{
	64,
	{
		{ dump_RDD_CPU_TX_MESSAGE_DATA_BUFFER_ENTRY, 0xafc0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_ROUTER_INGRESS_QUEUE =
{
	1,
	{
		{ dump_RDD_INGRESS_QUEUE_ENTRY, 0xb000 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_VLAN_ACTION_BUFFER =
{
	64,
	{
		{ dump_RDD_VLAN_ACTION_BUFFER, 0xb040 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_CPU_RX_FAST_INGRESS_QUEUE =
{
	1,
	{
		{ dump_RDD_INGRESS_QUEUE_ENTRY, 0xb080 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_DHD_TX_POST_FLOW_RING_BUFFER =
{
	48,
	{
		{ dump_RDD_DHD_TX_POST_DESCRIPTOR, 0xb0c0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT HASH_BUFFER =
{
	16,
	{
		{ dump_RDD_HASH_BUFFER, 0xb0f0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT CPU_FLOW_CACHE_INGRESS_QUEUE =
{
	1,
	{
		{ dump_RDD_INGRESS_QUEUE_ENTRY, 0xb100 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_DEBUG_BUFFER =
{
	4,
	{
		{ dump_RDD_DEBUG_BUFFER_ENTRY, 0xb140 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DHD_COMPLETE_RING_BUFFER =
{
	8,
	{
		{ dump_RDD_DHD_COMPLETE_RING_ENTRY, 0xb1c0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT FIREWALL_CONFIGURATION_REGISTER =
{
	8,
	{
		{ dump_RDD_FIREWALL_CONFIGURATION_REGISTER_ENTRY, 0xb1d8 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_DATA_POINTER_DUMMY_TARGET =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0xb1e0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_BPM_EXTRA_DDR_BUFFERS_BASE =
{
	4,
	{
		{ dump_RDD_BPM_EXTRA_DDR_BUFFERS_BASE, 0xb1f4 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_RUNNER_FLOW_HEADER_DESCRIPTOR =
{
	8,
	{
		{ dump_RDD_RUNNER_FLOW_HEADER_DESCRIPTOR, 0xb1f8 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_DHD_TX_POST_INGRESS_QUEUE =
{
	1,
	{
		{ dump_RDD_INGRESS_QUEUE_ENTRY, 0xb200 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_DHD_FLOW_RING_DROP_COUNTER =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0xb240 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT IPTV_SSM_CONTEXT_TABLE_PTR =
{
	4,
	{
		{ dump_RDD_IPTV_SSM_CONTEXT_TABLE_PTR, 0xb254 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_NULL_BUFFER =
{
	8,
	{
		{ dump_RDD_EIGHT_BYTES, 0xb258 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_FAST_FREE_SKB_INDEXES_FIFO_LOCAL_TABLE =
{
	2,
	{
		{ dump_RDD_FREE_SKB_INDEXES_FIFO_ENTRY, 0xb260 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT IPTV_DMA_LKP_KEY =
{
	16,
	{
		{ dump_RDD_IPTV_DMA_LKP_KEY, 0xb270 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_INGRESS_QUEUE =
{
	1,
	{
		{ dump_RDD_INGRESS_QUEUE_ENTRY, 0xb280 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT EMAC_ABSOLUTE_TX_BBH_COUNTER =
{
	8,
	{
		{ dump_RDD_EIGHT_BYTES, 0xb2c0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT FREE_PACKET_DESCRIPTORS_POOL_DESCRIPTOR =
{
	8,
	{
		{ dump_RDD_FREE_PACKET_DESCRIPTORS_POOL_DESCRIPTOR_ENTRY, 0xb2f0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_ONE_BUFFER =
{
	8,
	{
		{ dump_RDD_EIGHT_BYTES, 0xb2f8 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_PICO_FREE_SKB_INDEXES_FIFO_LOCAL_TABLE =
{
	2,
	{
		{ dump_RDD_FREE_SKB_INDEXES_FIFO_ENTRY, 0xb300 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_IPTV_SBPM_REPLICATION_BN =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xb310 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_PARALLEL_PROCESSING_CONTEXT_INDEX_CACHE_CAM =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xb330 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT MULTICAST_HEADER_BUFFER =
{
	32,
	{
		{ dump_RDD_MULTICAST_HEADER_BUFFER, 0xb340 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_INGRESS_FILTERS_CONFIGURATION_TABLE =
{
	4,
	{
		{ dump_RDD_INGRESS_FILTERS_CONFIGURATION_ENTRY, 0xb360 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT IPTV_TABLE_POINTER =
{
	4,
	{
		{ dump_RDD_IPTV_PTR_ENTRY, 0xb36c },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_LAYER4_FILTERS_CONTEXT_TABLE =
{
	1,
	{
		{ dump_RDD_LAYER4_FILTERS_CONTEXT_ENTRY, 0xb370 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT SERVICE_QUEUES_OVERALL_RATE_LIMITER =
{
	24,
	{
		{ dump_RDD_SERVICE_QUEUES_RATE_LIMITER_DESCRIPTOR, 0xb380 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT GSO_TX_DHD_L2_BUFFER =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb398 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_MAIN_TIMER_CONTROL_DESCRIPTOR =
{
	2,
	{
		{ dump_RDD_TIMER_CONTROL_DESCRIPTOR, 0xb3ae },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT CPU_TX_DHD_L2_BUFFER =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb3b0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_PICO_TIMER_CONTROL_DESCRIPTOR =
{
	2,
	{
		{ dump_RDD_TIMER_CONTROL_DESCRIPTOR, 0xb3c6 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_SSID_STATS_STATE_TABLE =
{
	6,
	{
		{ dump_RDD_WLAN_MCAST_SSID_STATS_STATE_ENTRY, 0xb3c8 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT IPTV_COUNTERS_BUFFER =
{
	2,
	{
		{ dump_RDD_IPTV_COUNTERS_BUFFER, 0xb3ce },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT IPTV_DMA_RW_BUFFER =
{
	16,
	{
		{ dump_RDD_IPTV_DMA_RW_BUFFER, 0xb3d0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_INGRESS_CLASSIFICATION_KEY_BUFFER =
{
	16,
	{
		{ dump_RDD_SIXTEEN_BYTES, 0xb3e0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT ETH_TX_QUEUE_DUMMY_DESCRIPTOR =
{
	16,
	{
		{ dump_RDD_ETH_TX_QUEUE_DUMMY_DESCRIPTOR, 0xb3f0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT GPON_RX_DIRECT_DESCRIPTORS =
{
	8,
	{
		{ dump_RDD_BBH_RX_DESCRIPTOR, 0xb400 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT FIREWALL_RULE_ENTRY_BUFFER =
{
	16,
	{
		{ dump_RDD_SIXTEEN_BYTES, 0xb500 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT CPU_TX_DS_PICO_DHD_TX_POST_CONTEXT =
{
	16,
	{
		{ dump_RDD_DHD_TX_POST_CONTEXT_ENTRY, 0xb510 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_DHD_TX_POST_CONTEXT =
{
	16,
	{
		{ dump_RDD_DHD_TX_POST_CONTEXT_ENTRY, 0xb520 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT GSO_TX_DS_PICO_DHD_TX_POST_CONTEXT =
{
	16,
	{
		{ dump_RDD_DHD_TX_POST_CONTEXT_ENTRY, 0xb530 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT SC_BUFFER =
{
	12,
	{
		{ dump_RDD_SC_BUFFER, 0xb540 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT IPTV_CONTEXT_TABLE_POINTER =
{
	4,
	{
		{ dump_RDD_IPTV_CONTEXT_PTR_ENTRY, 0xb54c },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_MULTICAST_VECTOR_TO_PORT_TABLE =
{
	1,
	{
		{ dump_RDD_MULTICAST_VECTOR_TO_PORT_ENTRY, 0xb550 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT MULTICAST_DUMMY_VLAN_INDEXES_TABLE =
{
	8,
	{
		{ dump_RDD_MULTICAST_DUMMY_VLAN_INDEXES_TABLE, 0xb558 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_BPM_DDR_OPTIMIZED_BUFFERS_BASE =
{
	4,
	{
		{ dump_RDD_BPM_DDR_OPTIMIZED_BUFFERS_BASE, 0xb560 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_BPM_DDR_BUFFERS_BASE =
{
	4,
	{
		{ dump_RDD_BPM_DDR_BUFFERS_BASE, 0xb564 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_CONNECTION_TABLE_CONFIG =
{
	4,
	{
		{ dump_RDD_CONNECTION_TABLE_CONFIG, 0xb568 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_CONTEXT_TABLE_CONFIG =
{
	4,
	{
		{ dump_RDD_CONTEXT_TABLE_CONFIG, 0xb56c },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_PARALLEL_PROCESSING_IH_BUFFER_VECTOR =
{
#if defined OREN
	1,
	{
		{ dump_RDD_PARALLEL_PROCESSING_ENTRY, 0xb570 },
#endif
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_DEBUG_PERIPHERALS_STATUS_REGISTER =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0xb574 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_PARALLEL_PROCESSING_TASK_REORDER_FIFO =
{
#if defined OREN
	1,
	{
		{ dump_RDD_PARALLEL_PROCESSING_TASK_REORDER_ENTRY, 0xb578 },
#endif
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_DDR_ADDRESS_FOR_SYNC_DMA_POINTER =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0xb57c },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_PARALLEL_PROCESSING_CONTEXT_INDEX_CACHE_TABLE =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb580 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT EMAC_SKB_ENQUEUED_INDEXES_FIFO_COUNTERS =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb588 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT ETH_TX_EMACS_STATUS =
{
#if defined OREN
	1,
	{
		{ dump_RDD_ETH_TX_EMACS_STATUS_ENTRY, 0xb58d },
#endif
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_INGRESS_CLASSIFICATION_COUNTERS_BUFFER =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xb58e },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT HASH_BASED_FORWARDING_PORT_TABLE =
{
	1,
	{
		{ dump_RDD_HASH_BASED_FORWARDING_PORT_ENTRY, 0xb590 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT FIREWALL_IPV6_R16_BUFFER =
{
	4,
	{
		{ dump_RDD_FIREWALL_IPV6_R16_BUFFER_ENTRY, 0xb594 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT CPU_TX_PICO_INGRESS_QUEUE_PTR =
{
	2,
	{
		{ dump_RDD_CPU_TX_PICO_INGRESS_QUEUE_PTR, 0xb598 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT FREE_PACKET_DESCRIPTORS_POOL_THRESHOLD =
{
	2,
	{
		{ dump_RDD_FREE_PACKET_DESCRIPTORS_POOL_THRESHOLD, 0xb59a },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DOWNSTREAM_MULTICAST_LAN_ENQUEUE_INGRESS_QUEUE_PTR =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xb59c },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT CPU_RX_PD_INGRESS_QUEUE_PTR =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xb59e },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT CPU_RX_MIRRORING_PD_INGRESS_QUEUE_PTR =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xb5a0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_CPU_RX_PICO_INGRESS_QUEUE_PTR =
{
	2,
	{
		{ dump_RDD_CPU_RX_PICO_INGRESS_QUEUE_PTR, 0xb5a2 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_BPM_DDR_BUFFER_HEADROOM_SIZE =
{
	2,
	{
		{ dump_RDD_BPM_DDR_BUFFER_HEADROOM_SIZE, 0xb5a4 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_BPM_DDR_BUFFER_HEADROOM_SIZE_2_BYTE_RESOLUTION =
{
	2,
	{
		{ dump_RDD_BPM_DDR_BUFFER_HEADROOM_SIZE_2_BYTE_RESOLUTION, 0xb5a6 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_CPU_RX_FAST_INGRESS_QUEUE_PTR =
{
	2,
	{
		{ dump_RDD_CPU_RX_FAST_INGRESS_QUEUE_PTR, 0xb5a8 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_PARALLEL_PROCESSING_IH_BUFFER_VECTOR_PTR =
{
#if defined OREN
	2,
	{
		{ dump_RDD_PARALLEL_PROCESSING_IH_BUFFER_PTR, 0xb5aa },
#endif
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_MICROCODE_VERSION =
{
	4,
	{
		{ dump_RDD_MICROCODE_VERSION_ENTRY, 0xb5ac },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_RATE_LIMITER_COUNTER_BUFFER =
{
	4,
	{
		{ dump_RDD_RATE_LIMITER_COUNTER_BUFFER_ENTRY, 0xb5b0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_DDR_ADDRESS_FOR_FULL_READ_DMA_POINTER =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0xb5b4 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT GSO_TX_DHD_HOST_BUF_PTR =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb5b8 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DHD_TX_POST_PD_INGRESS_QUEUE_WR_PTR =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xb5bc },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_MEMLIB_SEMAPHORE =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xb5be },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT CPU_TX_DHD_HOST_BUF_PTR =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb5c0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_DHD_TX_POST_HOST_DATA_PTR_BUFFER =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0xb5c4 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DUAL_STACK_LITE_LAYER_2_HEADER_LENGTH =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xb5c8 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DUAL_STACK_LITE_LAYER_3_HEADER_LENGTH =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xb5ca },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DOWNSTREAM_MULTICAST_LAN_ENQUEUE_SERVICE_QUEUE_INGRESS_QUEUE_PTR =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xb5cc },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_RUNNER_CONGESTION_STATE =
{
	2,
	{
		{ dump_RDD_RUNNER_CONGESTION_STATE_ENTRY, 0xb5ce },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT FIREWALL_RULE_MAP_ENTRY_BUFFER =
{
	2,
	{
		{ dump_RDD_FIREWALL_RULE_MAP_ENTRY_BUFFER, 0xb5d0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DHD_TX_COMPLETE_BPM_REF_COUNTER =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xb5d2 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DHD_TX_POST_CPU_BPM_REF_COUNTER =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xb5d4 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_DHD_BPM_CONGESTION_UG3_DROP_COUNTER =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xb5d6 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_DHD_BPM_CONGESTION_ALLOC_FAIL_DROP_COUNTER =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xb5d8 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT PRIVATE_A_DUMMY_STORE =
{
	1,
	{
		{ dump_RDD_DUMMY_STORE_ENTRY, 0xb5da },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT ETH_TX_INTER_LAN_SCHEDULING_OFFSET =
{
#if defined OREN
	1,
	{
		{ dump_RDD_ETH_TX_INTER_LAN_SCHEDULING_OFFSET_ENTRY, 0xb5db },
#endif
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_FAST_FREE_SKB_INDEXES_FIFO_LOCAL_TABLE_PTR =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb5dc },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_PICO_FREE_SKB_INDEXES_FIFO_LOCAL_TABLE_PTR =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb5dd },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_INGRESS_CLASSIFICATION_RULE_CFG_DESCRIPTOR =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb5de },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_INGRESS_CLASSIFICATION_IP_FLOW_RULE_CFG_DESCRIPTOR =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb5df },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_PACKET_BUFFER_SIZE_ASR_8 =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb5e0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_MAIN_DMA_SYNCRONIZATION_ADDRESS =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb5e1 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_PICO_DMA_SYNCRONIZATION_ADDRESS =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb5e2 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_RUNNER_FLOW_IH_RESPONSE_MUTEX =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb5e3 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DOWNSTREAM_MULTICAST_SERVICE_QUEUE_SSID_PTR =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb5e4 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT FIREWALL_RULE_MAP_ENTRY_BUFFER_SYNC =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb5e5 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_PARALLEL_PROCESSING_SLAVE_VECTOR =
{
#if defined OREN
	1,
	{
		{ dump_RDD_PARALLEL_PROCESSING_SLAVE_VECTOR, 0xb5e6 },
#endif
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_PARALLEL_PROCESSING_CONTEXT_CACHE_MODE =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb5e7 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT IP_SYNC_1588_TX_ENQUEUE_RESULT =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb5e8 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_DHD_DMA_SYNCHRONIZATION =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb5e9 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DHD_TX_POST_CPU_SEMAPHORE =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb5ea },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT PCI_TX_FIFO_DESCRIPTORS_TABLE =
{
	8,
	{
		{ dump_RDD_PCI_TX_FIFO_DESCRIPTOR_ENTRY, 0xb6c0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT PCI_TX_QUEUES_VECTOR =
{
	4,
	{
		{ dump_RDD_PCI_TX_QUEUES_VECTOR, 0xb6e0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT PCI_TX_FIFO_FULL_VECTOR =
{
	1,
	{
		{ dump_RDD_PCI_TX_FIFO_FULL_VECTOR_ENTRY, 0xb6e4 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT RUNNER_FLOW_IH_RESPONSE =
{
	8,
	{
		{ dump_RDD_RUNNER_FLOW_IH_RESPONSE, 0xb6f0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT GPON_RX_NORMAL_DESCRIPTORS =
{
	8,
	{
		{ dump_RDD_BBH_RX_DESCRIPTOR, 0xb800 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DOWNSTREAM_DMA_PIPE_BUFFER =
{
	4,
	{
		{ dump_RDD_DOWNSTREAM_DMA_PIPE_BUFFER, 0xbcb8 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_INGRESS_HANDLER_BUFFER =
{
	256,
	{
		{ dump_RDD_IH_BUFFER, 0x0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_FREE_PACKET_DESCRIPTORS_POOL =
{
	8,
	{
		{ dump_RDD_PACKET_DESCRIPTOR, 0x2000 },
		{ dump_RDD_BBH_TX_DESCRIPTOR, 0x2000 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_INGRESS_CLASSIFICATION_CONTEXT_TABLE =
{
	8,
	{
		{ dump_RDD_US_INGRESS_CLASSIFICATION_CONTEXT_ENTRY, 0x8000 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_DSCP_TO_PBITS_TABLE =
{
	1,
	{
		{ dump_RDD_DSCP_TO_PBITS_ENTRY, 0x8800 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_SBPM_REPLY =
{
	128,
	{
		{ dump_RDD_SBPM_REPLY_ENTRY, 0x8980 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_VLAN_PARAMETER_TABLE =
{
	4,
	{
		{ dump_RDD_VLAN_PARAMETER_ENTRY, 0x8a00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT WAN_CHANNELS_8_39_TABLE =
{
	32,
	{
		{ dump_RDD_WAN_CHANNEL_8_39_DESCRIPTOR, 0x8c00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_RUNNER_FLOW_HEADER_BUFFER =
{
	128,
	{
		{ dump_RDD_RUNNER_FLOW_HEADER_BUFFER, 0x9000 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_QUEUE_PROFILE_TABLE =
{
	16,
	{
		{ dump_RDD_QUEUE_PROFILE, 0x9180 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_CPU_REASON_TO_METER_TABLE =
{
	1,
	{
		{ dump_RDD_CPU_REASON_TO_METER_ENTRY, 0x9200 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_FAST_CPU_TX_DESCRIPTOR_ABS_DATA_PTR_QUEUE =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0x9240 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_PICO_CPU_TX_DESCRIPTOR_ABS_DATA_PTR_QUEUE =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0x9280 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_PBITS_TO_QOS_TABLE =
{
	1,
	{
		{ dump_RDD_US_QUEUE_ENTRY, 0x92c0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_RATE_LIMITER_TABLE =
{
	8,
	{
		{ dump_RDD_RATE_LIMITER_ENTRY, 0x9300 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_LAYER4_FILTERS_LOOKUP_TABLE =
{
	8,
	{
		{ dump_RDD_LAYER4_FILTERS_LOOKUP_ENTRY, 0x9380 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_WAN_FLOW_TABLE =
{
	4,
	{
		{ dump_RDD_US_WAN_FLOW_ENTRY, 0x9400 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_INGRESS_FILTERS_LOOKUP_TABLE =
{
	4,
	{
		{ dump_RDD_INGRESS_FILTERS_LOOKUP_ENTRY, 0x9800 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_PBITS_TO_PBITS_TABLE =
{
	1,
	{
		{ dump_RDD_PBITS_TO_PBITS_ENTRY, 0x9b00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT WAN_CHANNELS_0_7_TABLE =
{
	88,
	{
		{ dump_RDD_WAN_CHANNEL_0_7_DESCRIPTOR, 0x9c00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_TRAFFIC_CLASS_TO_QUEUE_TABLE =
{
	1,
	{
		{ dump_RDD_US_QUEUE_ENTRY, 0x9ec0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_PBITS_PARAMETER_TABLE =
{
	2,
	{
		{ dump_RDD_PBITS_PARAMETER_ENTRY, 0x9f00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT SMART_CARD_DESCRIPTOR_TABLE =
{
	272,
	{
		{ dump_RDD_SMART_CARD_DESCRIPTOR_ENTRY, 0xa000 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_VLAN_COMMANDS_TABLE =
{
	8,
	{
		{ dump_RDD_VLAN_COMMAND_ENRTY, 0xa110 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_RATE_CONTROL_BUDGET_ALLOCATOR_TABLE =
{
	2,
	{
		{ dump_RDD_BUDGET_ALLOCATOR_ENTRY, 0xa310 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_MAIN_TIMER_TASK_DESCRIPTOR_TABLE =
{
	8,
	{
		{ dump_RDD_TIMER_TASK_DESCRIPTOR_ENTRY, 0xa320 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_PBITS_TO_WAN_FLOW_TABLE =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xa340 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_CPU_RX_METER_TABLE =
{
	8,
	{
		{ dump_RDD_CPU_RX_METER_ENTRY, 0xa380 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_LAN_VID_TABLE =
{
	2,
	{
		{ dump_RDD_VID_ENTRY, 0xa400 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_POLICER_TABLE =
{
	16,
	{
		{ dump_RDD_POLICER_ENTRY, 0xa500 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_PACKET_BUFFER_TABLE =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xa600 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_FORWARDING_MATRIX_TABLE =
{
	1,
	{
		{ dump_RDD_FORWARDING_MATRIX_ENTRY, 0xa700 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_TPID_OVERWRITE_TABLE =
{
	2,
	{
		{ dump_RDD_TPID_OVERWRITE_ENTRY, 0xa790 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_PICO_TIMER_TASK_DESCRIPTOR_TABLE =
{
	8,
	{
		{ dump_RDD_TIMER_TASK_DESCRIPTOR_ENTRY, 0xa7a0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_INGRESS_CLASSIFICATION_KEY_PRIMITIVE_TABLE =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xa7c0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_CPU_TX_BBH_DESCRIPTORS =
{
	8,
	{
		{ dump_RDD_CPU_TX_BBH_DESCRIPTORS_ENTRY, 0xa800 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_INGRESS_HANDLER_SKB_DATA_POINTER =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0xa900 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_TIMER_SCHEDULER_PRIMITIVE_TABLE =
{
	2,
	{
		{ dump_RDD_TIMER_SCHEDULER_PRIMITIVE_ENTRY, 0xa980 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT GPON_ABSOLUTE_TX_BBH_COUNTER =
{
	1,
	{
		{ dump_RDD_GPON_ABSOLUTE_TX_COUNTER, 0xa990 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_CPU_PARAMETERS_BLOCK =
{
	8,
	{
		{ dump_RDD_CPU_PARAMETERS_BLOCK_ENTRY, 0xa9b8 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_FLOW_BASED_ACTION_PTR_TABLE =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xa9c0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_CONNECTION_CONTEXT_BUFFER =
{
	64,
	{
		{ dump_RDD_CONNECTION_CONTEXT_BUFFER_ENTRY, 0xaa00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_BRIDGE_CONFIGURATION_REGISTER =
{
	256,
	{
		{ dump_RDD_BRIDGE_CONFIGURATION_REGISTER, 0xab00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_CPU_TX_FAST_QUEUE =
{
	8,
	{
		{ dump_RDD_CPU_TX_DESCRIPTOR, 0xac00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_CORE, 0xac00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_BPM, 0xac00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_ABS, 0xac00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_DS_FAST, 0xac00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_US_FAST, 0xac00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_DS_PICO, 0xac00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_DS_PICO_WIFI, 0xac00 },
		{ dump_RDD_CPU_TX_MESSAGE_DESCRIPTOR, 0xac00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_INGRESS_CLASSIFICATION_RULE_CFG_TABLE =
{
	8,
	{
		{ dump_RDD_INGRESS_CLASSIFICATION_RULE_CFG_ENTRY, 0xac80 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_CPU_TX_PICO_QUEUE =
{
	8,
	{
		{ dump_RDD_CPU_TX_DESCRIPTOR, 0xad00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_CORE, 0xad00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_BPM, 0xad00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_ABS, 0xad00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_DS_FAST, 0xad00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_US_FAST, 0xad00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_DS_PICO, 0xad00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_DS_PICO_WIFI, 0xad00 },
		{ dump_RDD_CPU_TX_MESSAGE_DESCRIPTOR, 0xad00 },
		{ dump_RDD_CPU_TX_DHD_DESCRIPTOR, 0xad00 },
		{ dump_RDD_CPU_TX_DHD_MESSAGE_DESCRIPTOR, 0xad00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_INGRESS_FILTERS_PARAMETER_TABLE =
{
	1,
	{
		{ dump_RDD_INGRESS_FILTERS_PARAMETER_ENTRY, 0xad80 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_INGRESS_CLASSIFICATION_GENERIC_RULE_CFG_TABLE =
{
	8,
	{
		{ dump_RDD_INGRESS_CLASSIFICATION_GENERIC_RULE_CFG_ENTRY, 0xade0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_PROFILING_BUFFER_PICO_RUNNER =
{
	256,
	{
		{ dump_RDD_PROFILING_BUFFER_PICO_RUNNER, 0xae00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DHD_RX_COMPLETE_FLOW_RING_BUFFER =
{
	32,
	{
		{ dump_RDD_DHD_RX_POST_DESCRIPTOR, 0xaf00 },
		{ dump_RDD_DHD_RX_COMPLETE_DESCRIPTOR, 0xaf00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_FAST_RUNNER_GLOBAL_REGISTERS_INIT =
{
	4,
	{
		{ dump_RDD_FAST_RUNNER_GLOBAL_REGISTERS_INIT_ENTRY, 0xaf60 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DHD_RX_POST_FLOW_RING_BUFFER =
{
	32,
	{
		{ dump_RDD_DHD_RX_POST_DESCRIPTOR, 0xaf80 },
		{ dump_RDD_DHD_RX_COMPLETE_DESCRIPTOR, 0xaf80 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_PICO_RUNNER_GLOBAL_REGISTERS_INIT =
{
	4,
	{
		{ dump_RDD_PICO_RUNNER_GLOBAL_REGISTERS_INIT_ENTRY, 0xafe0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT ETH0_RX_DESCRIPTORS =
{
	8,
	{
		{ dump_RDD_ETH_RX_DESCRIPTORS, 0xb000 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_INGRESS_RATE_LIMITER_TABLE =
{
	16,
	{
		{ dump_RDD_INGRESS_RATE_LIMITER_ENTRY, 0xb100 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_VLAN_OPTIMIZATION_TABLE =
{
	1,
	{
		{ dump_RDD_VLAN_OPTIMIZATION_ENTRY, 0xb150 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_INGRESS_CLASSIFICATION_COUNTERS_BUFFER =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xb1d0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_BPM_EXTRA_DDR_BUFFERS_BASE =
{
	4,
	{
		{ dump_RDD_BPM_EXTRA_DDR_BUFFERS_BASE, 0xb1dc },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_INGRESS_FILTERS_CONFIGURATION_TABLE =
{
	4,
	{
		{ dump_RDD_INGRESS_FILTERS_CONFIGURATION_ENTRY, 0xb1e0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT SMART_CARD_ERROR_COUNTERS_TABLE =
{
	8,
	{
		{ dump_RDD_SMART_CARD_ERROR_COUNTERS_ENTRY, 0xb1f8 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT ETH1_RX_DESCRIPTORS =
{
	8,
	{
		{ dump_RDD_ETH_RX_DESCRIPTORS, 0xb200 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_PBITS_PRIMITIVE_TABLE =
{
	2,
	{
		{ dump_RDD_PBITS_PRIMITIVE_ENTRY, 0xb300 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT SPEED_SERVICE_PARAMETERS_TABLE =
{
	42,
	{
		{ dump_RDD_SPEED_SERVICE_PARAMETERS, 0xb340 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_MAIN_TIMER_CONTROL_DESCRIPTOR =
{
	2,
	{
		{ dump_RDD_TIMER_CONTROL_DESCRIPTOR, 0xb36a },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_BPM_DDR_OPTIMIZED_BUFFERS_BASE =
{
	4,
	{
		{ dump_RDD_BPM_DDR_OPTIMIZED_BUFFERS_BASE, 0xb36c },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_LAYER4_FILTERS_CONTEXT_TABLE =
{
	1,
	{
		{ dump_RDD_LAYER4_FILTERS_CONTEXT_ENTRY, 0xb370 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_CPU_RX_PICO_INGRESS_QUEUE =
{
	1,
	{
		{ dump_RDD_INGRESS_QUEUE_ENTRY, 0xb380 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_ACK_PACKETS_QUEUE_INDEX_TABLE =
{
	1,
	{
		{ dump_RDD_US_QUEUE_ENTRY, 0xb3c0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT CPU_REASON_AND_SRC_BRIDGE_PORT_TO_METER_TABLE =
{
	1,
	{
		{ dump_RDD_CPU_REASON_TO_METER_ENTRY, 0xb3e8 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_PICO_TIMER_CONTROL_DESCRIPTOR =
{
	2,
	{
		{ dump_RDD_TIMER_CONTROL_DESCRIPTOR, 0xb3fa },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_BPM_DDR_BUFFERS_BASE =
{
	4,
	{
		{ dump_RDD_BPM_DDR_BUFFERS_BASE, 0xb3fc },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_GPON_RX_DIRECT_DESCRIPTORS =
{
	8,
	{
		{ dump_RDD_BBH_RX_DESCRIPTOR, 0xb400 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_ROUTER_INGRESS_QUEUE =
{
	1,
	{
		{ dump_RDD_INGRESS_QUEUE_ENTRY, 0xb500 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_VLAN_PRIMITIVE_TABLE =
{
	2,
	{
		{ dump_RDD_VLAN_PRIMITIVE_ENTRY, 0xb540 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_CPU_RX_FAST_INGRESS_QUEUE =
{
	1,
	{
		{ dump_RDD_INGRESS_QUEUE_ENTRY, 0xb580 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_DEBUG_BUFFER =
{
	4,
	{
		{ dump_RDD_DEBUG_BUFFER_ENTRY, 0xb5c0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_CPU_TX_MESSAGE_DATA_BUFFER =
{
	64,
	{
		{ dump_RDD_CPU_TX_MESSAGE_DATA_BUFFER_ENTRY, 0xb640 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT LOCAL_SWITCHING_MULTICAST_INGRESS_QUEUE =
{
	1,
	{
		{ dump_RDD_INGRESS_QUEUE_ENTRY, 0xb680 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_RUNNER_FLOW_HEADER_DESCRIPTOR =
{
	8,
	{
		{ dump_RDD_RUNNER_FLOW_HEADER_DESCRIPTOR, 0xb6c0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_PARALLEL_PROCESSING_CONTEXT_INDEX_CACHE_CAM =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xb6d8 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_NULL_BUFFER =
{
	8,
	{
		{ dump_RDD_EIGHT_BYTES, 0xb6e0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT BBH_TX_EPON_REQUEST_WAN_CHANNEL_INDEX =
{
	8,
	{
		{ dump_RDD_EIGHT_BYTES, 0xb6e8 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_RUNNER_FLOW_IH_RESPONSE =
{
	8,
	{
		{ dump_RDD_RUNNER_FLOW_IH_RESPONSE, 0xb6f0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_MULTICAST_VECTOR_TO_PORT_TABLE =
{
	1,
	{
		{ dump_RDD_MULTICAST_VECTOR_TO_PORT_ENTRY, 0xb6f8 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT VLAN_ACTION_BRIDGE_INGRESS_QUEUE =
{
	1,
	{
		{ dump_RDD_INGRESS_QUEUE_ENTRY, 0xb700 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_VLAN_ACTION_BUFFER =
{
	64,
	{
		{ dump_RDD_VLAN_ACTION_BUFFER, 0xb740 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT UPSTREAM_FLOODING_INGRESS_QUEUE =
{
	1,
	{
		{ dump_RDD_INGRESS_QUEUE_ENTRY, 0xb780 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_DHD_TX_POST_FLOW_RING_BUFFER =
{
	48,
	{
		{ dump_RDD_DHD_TX_POST_DESCRIPTOR, 0xb7c0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_INGRESS_CLASSIFICATION_KEY_BUFFER =
{
	16,
	{
		{ dump_RDD_SIXTEEN_BYTES, 0xb7f0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT ETH2_RX_DESCRIPTORS =
{
	8,
	{
		{ dump_RDD_ETH_RX_DESCRIPTORS, 0xb800 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT WAN_INTERWORKING_INGRESS_QUEUE =
{
	1,
	{
		{ dump_RDD_INGRESS_QUEUE_ENTRY, 0xb900 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT CPU_TX_DS_EGRESS_DHD_TX_POST_FLOW_RING_BUFFER =
{
	48,
	{
		{ dump_RDD_DHD_TX_POST_DESCRIPTOR, 0xb940 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT CPU_TX_DS_EGRESS_DHD_TX_POST_CONTEXT =
{
	16,
	{
		{ dump_RDD_DHD_TX_POST_CONTEXT_ENTRY, 0xb970 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_DHD_TX_POST_INGRESS_QUEUE =
{
	1,
	{
		{ dump_RDD_INGRESS_QUEUE_ENTRY, 0xb980 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT LAN_INGRESS_FIFO_DESCRIPTOR_TABLE =
{
	4,
	{
		{ dump_RDD_LAN_INGRESS_FIFO_DESCRIPTOR_ENTRY, 0xb9c0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_CONNECTION_TABLE_CONFIG =
{
	4,
	{
		{ dump_RDD_CONNECTION_TABLE_CONFIG, 0xb9d4 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT GPON_ABSOLUTE_TX_FIRMWARE_COUNTER =
{
	1,
	{
		{ dump_RDD_GPON_ABSOLUTE_TX_COUNTER, 0xb9d8 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT ETH3_RX_DESCRIPTORS =
{
	8,
	{
		{ dump_RDD_ETH_RX_DESCRIPTORS, 0xba00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_OVERALL_RATE_LIMITER_WAN_CHANNEL_PTR_TABLE =
{
	2,
	{
		{ dump_RDD_US_OVERALL_RATE_LIMITER_WAN_CHANNEL_PTR_ENTRY, 0xbb00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_FAST_FREE_SKB_INDEXES_FIFO_LOCAL_TABLE =
{
	2,
	{
		{ dump_RDD_FREE_SKB_INDEXES_FIFO_ENTRY, 0xbb60 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT IPV6_LOCAL_IP =
{
	14,
	{
		{ dump_RDD_IPV6_LOCAL_IP, 0xbb70 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_CPU_RX_PICO_INGRESS_QUEUE_PTR =
{
	2,
	{
		{ dump_RDD_CPU_RX_PICO_INGRESS_QUEUE_PTR, 0xbb7e },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_DHD_FLOW_RING_DROP_COUNTER =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0xbb80 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_CONTEXT_TABLE_CONFIG =
{
	4,
	{
		{ dump_RDD_CONTEXT_TABLE_CONFIG, 0xbb94 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_PARALLEL_PROCESSING_IH_BUFFER_VECTOR =
{
#if defined OREN
	1,
	{
		{ dump_RDD_PARALLEL_PROCESSING_ENTRY, 0xbb98 },
#endif
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_DEBUG_PERIPHERALS_STATUS_REGISTER =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0xbb9c },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_PICO_FREE_SKB_INDEXES_FIFO_LOCAL_TABLE =
{
	2,
	{
		{ dump_RDD_FREE_SKB_INDEXES_FIFO_ENTRY, 0xbba0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_INGRESS_CLASSIFICATION_DEFAULT_FLOWS_TABLE =
{
	1,
	{
		{ dump_RDD_US_INGRESS_CLASSIFICATION_DEFAULT_FLOWS_ENTRY, 0xbbb0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT CPU_TX_DHD_LAYER2_HEADER_BUFFER =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xbbd0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_BPM_DDR_BUFFER_HEADROOM_SIZE =
{
	2,
	{
		{ dump_RDD_BPM_DDR_BUFFER_HEADROOM_SIZE, 0xbbde },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT LOCAL_SWITCHING_MODE_TABLE =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xbbe0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_BPM_DDR_BUFFER_HEADROOM_SIZE_2_BYTE_RESOLUTION =
{
	2,
	{
		{ dump_RDD_BPM_DDR_BUFFER_HEADROOM_SIZE_2_BYTE_RESOLUTION, 0xbbe6 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_PARALLEL_PROCESSING_TASK_REORDER_FIFO =
{
#if defined OREN
	1,
	{
		{ dump_RDD_PARALLEL_PROCESSING_TASK_REORDER_ENTRY, 0xbbe8 },
#endif
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT IH_BUFFER_BBH_POINTER =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0xbbec },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT BROADCOM_SWITCH_PORT_TO_BRIDGE_PORT_MAPPING_TABLE =
{
	1,
	{
		{ dump_RDD_BROADCOM_SWITCH_PORT_MAPPING, 0xbbf0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_DDR_ADDRESS_FOR_SYNC_DMA_POINTER =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0xbbf8 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_DATA_POINTER_DUMMY_TARGET =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0xbbfc },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT INGRESS_VLAN_SWITCHING_ISOLATION_CONFIG_TABLE =
{
	1,
	{
		{ dump_RDD_VLAN_SWITCHING_ISOLATION_CONFIG_ENTRY, 0xbc00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_TX_QUEUE_FLOW_CONTROL_BRIDGE_PORT_TABLE =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xbc05 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_CPU_RX_FAST_INGRESS_QUEUE_PTR =
{
	2,
	{
		{ dump_RDD_CPU_RX_FAST_INGRESS_QUEUE_PTR, 0xbc06 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_MICROCODE_VERSION =
{
	4,
	{
		{ dump_RDD_MICROCODE_VERSION_ENTRY, 0xbc08 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_RATE_LIMITER_COUNTER_BUFFER =
{
	4,
	{
		{ dump_RDD_RATE_LIMITER_COUNTER_BUFFER_ENTRY, 0xbc0c },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT LOCAL_SWITCHING_MULTICAST_LAN_ENQUEUE_INGRESS_QUEUE_PTR =
{
	2,
	{
		{ dump_RDD_LOCAL_SWITCHING_MULTICAST_LAN_ENQUEUE_INGRESS_QUEUE_PTR, 0xbc10 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT LOCAL_SWITCHING_LAN_ENQUEUE_INGRESS_QUEUE_PTR =
{
	2,
	{
		{ dump_RDD_LOCAL_SWITCHING_LAN_ENQUEUE_INGRESS_QUEUE_PTR, 0xbc12 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_PARALLEL_PROCESSING_IH_BUFFER_VECTOR_PTR =
{
#if defined OREN
	2,
	{
		{ dump_RDD_PARALLEL_PROCESSING_IH_BUFFER_PTR, 0xbc14 },
#endif
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT IP_SYNC_1588_DESCRIPTOR_QUEUE_POINTER =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xbc16 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_PARALLEL_PROCESSING_CONTEXT_INDEX_CACHE_TABLE =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xbc18 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_DDR_ADDRESS_FOR_FULL_READ_DMA_POINTER =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0xbc1c },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_DHD_TX_POST_HOST_DATA_PTR_BUFFER =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0xbc20 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_MEMLIB_SEMAPHORE =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xbc24 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_RUNNER_CONGESTION_STATE =
{
	2,
	{
		{ dump_RDD_RUNNER_CONGESTION_STATE_ENTRY, 0xbc26 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_DHD_BPM_CONGESTION_UG3_DROP_COUNTER =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xbc28 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_DHD_BPM_CONGESTION_ALLOC_FAIL_DROP_COUNTER =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xbc2a },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT PRIVATE_B_DUMMY_STORE =
{
	1,
	{
		{ dump_RDD_DUMMY_STORE_ENTRY, 0xbc2c },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_FAST_FREE_SKB_INDEXES_FIFO_LOCAL_TABLE_PTR =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xbc2d },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_PICO_FREE_SKB_INDEXES_FIFO_LOCAL_TABLE_PTR =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xbc2e },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_INGRESS_CLASSIFICATION_RULE_CFG_DESCRIPTOR =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xbc2f },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_INGRESS_CLASSIFICATION_IP_FLOW_RULE_CFG_DESCRIPTOR =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xbc30 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_PACKET_BUFFER_SIZE_ASR_8 =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xbc31 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_MAIN_DMA_SYNCRONIZATION_ADDRESS =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xbc32 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_PICO_DMA_SYNCRONIZATION_ADDRESS =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xbc33 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_RUNNER_FLOW_IH_RESPONSE_MUTEX =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xbc34 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_PARALLEL_PROCESSING_SLAVE_VECTOR =
{
#if defined OREN
	1,
	{
		{ dump_RDD_PARALLEL_PROCESSING_SLAVE_VECTOR, 0xbc35 },
#endif
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_PARALLEL_PROCESSING_CONTEXT_CACHE_MODE =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xbc36 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT WAN_TX_QUEUES_DDR_OFFLOAD_ENABLE =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xbc37 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT PON_TYPE_FLAG =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xbc38 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_FLOODING_PACKET_COPY_SYNC_STATUS =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xbc39 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_DHD_DMA_SYNCHRONIZATION =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xbc3a },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT BBH_TX_WAN_CHANNEL_INDEX =
{
	8,
	{
		{ dump_RDD_BBH_TX_WAN_CHANNEL_INDEX, 0xbcb8 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT ETH4_RX_DESCRIPTORS =
{
	8,
	{
		{ dump_RDD_ETH_RX_DESCRIPTORS, 0xbe00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT MAC_TABLE =
{
	8,
	{
		{ dump_RDD_MAC_ENTRY, 0x0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT IPTV_LOOKUP_TABLE =
{
	8,
	{
		{ dump_RDD_IPTV_L2_LOOKUP_ENTRY, 0x2000 },
		{ dump_RDD_IPTV_L3_LOOKUP_ENTRY, 0x2000 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT MAC_CONTEXT_TABLE =
{
	2,
	{
		{ dump_RDD_MAC_CONTEXT_ENTRY, 0x2800 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT IPTV_CONTEXT_TABLE =
{
#if defined OREN
	2,
	{
		{ dump_RDD_IPTV_CONTEXT_ENTRY, 0x3000 },
#endif
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT SERVICE_QUEUES_DESCRIPTOR_TABLE =
{
	16,
	{
		{ dump_RDD_DDR_QUEUE_DESCRIPTOR, 0x3200 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT IPTV_LOOKUP_TABLE_CAM =
{
	8,
	{
		{ dump_RDD_MAC_ENTRY, 0x3400 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT MAC_TABLE_CAM =
{
	8,
	{
		{ dump_RDD_MAC_ENTRY, 0x3500 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT IPTV_CONTEXT_TABLE_CAM =
{
#if defined OREN
	2,
	{
		{ dump_RDD_IPTV_CONTEXT_ENTRY, 0x3600 },
#endif
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT INTERRUPT_COALESCING_CONFIG_TABLE =
{
	4,
	{
		{ dump_RDD_INTERRUPT_COALESCING_CONFIG, 0x3640 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_DHD_TX_POST_FLOW_RING_MGMT_DESCRIPTOR_BUFFER =
{
	16,
	{
		{ dump_RDD_DHD_TX_POST_FLOW_RING_MGMT_DESCRIPTOR, 0x3670 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_PBITS_TO_QOS_TABLE =
{
	1,
	{
		{ dump_RDD_PBITS_TO_QOS_ENTRY, 0x3680 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT WAN_VID_TABLE =
{
	2,
	{
		{ dump_RDD_VID_ENTRY, 0x36b0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT INTERRUPT_COALESCING_TIMER_CONFIG_TABLE =
{
	4,
	{
		{ dump_RDD_INTERRUPT_COALESCING_TIMER_CONFIG, 0x36b8 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT FREE_SKB_INDEXES_DDR_FIFO_TAIL =
{
	4,
	{
		{ dump_RDD_FREE_SKB_INDEXES_FIFO_TAIL, 0x36bc },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DHD_RADIO_INSTANCE_COMMON_A_DATA =
{
	16,
	{
		{ dump_RDD_DHD_RADIO_INSTANCE_COMMON_A_ENTRY, 0x36c0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT BPM_CONGESTION_CONTROL_TABLE =
{
	12,
	{
		{ dump_RDD_BPM_CONGESTION_CONTROL_ENTRY, 0x36f0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DDR_ADDRESS_FOR_SKB_DATA_POINTERS_TABLE =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0x36fc },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT IPTV_L3_SRC_IP_LOOKUP_TABLE =
{
	8,
	{
		{ dump_RDD_IPTV_L3_SRC_IP_LOOKUP_ENTRY, 0x3700 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_DHD_STATION_TABLE =
{
	10,
	{
		{ dump_RDD_WLAN_MCAST_DHD_STATION_ENTRY, 0x3800 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_CPU_REASON_TO_CPU_RX_QUEUE_TABLE =
{
	1,
	{
		{ dump_RDD_CPU_REASON_TO_CPU_RX_QUEUE_ENTRY, 0x3a80 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT MAC_CONTEXT_TABLE_CAM =
{
	2,
	{
		{ dump_RDD_MAC_CONTEXT_ENTRY, 0x3ac0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT MAC_EXTENSION_TABLE =
{
	1,
	{
		{ dump_RDD_MAC_EXTENSION_ENTRY, 0x3b00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_DHD_FLOW_RING_CACHE_LKP_TABLE =
{
	2,
	{
		{ dump_RDD_DHD_FLOW_RING_CACHE_LKP_ENTRY, 0x3f00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT MAC_UNKNOWN_DA_FORWARDING_TABLE =
{
	8,
	{
		{ dump_RDD_MAC_UNKNOWN_DA_FORWARDING_ENTRY, 0x3f20 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DHD_TX_POST_BUFFERS_THRESHOLD =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0x3f28 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DHD_TX_POST_BUFFERS_IN_USE_COUNTER =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0x3f2a },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT COMMON_A_DUMMY_STORE =
{
	1,
	{
		{ dump_RDD_DUMMY_STORE_ENTRY, 0x3f2c },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DDR_ADDRESS_FOR_FREE_SKB_INDEXES_FIFO_TABLE =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0x3f30 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DDR_ADDRESS_FOR_FREE_SKB_INDEXES_FIFO_TABLE_LAST_ENTRY =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0x3f34 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT SCT_FILTER =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0x3f3c },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT BPM_REPLY_RUNNER_A =
{
	48,
	{
		{ dump_RDD_BPM_REPLY, 0x3f40 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT GLOBAL_DSCP_TO_PBITS_TABLE =
{
	1,
	{
		{ dump_RDD_DSCP_TO_PBITS_ENTRY, 0x3f70 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT GLOBAL_DSCP_TO_PBITS_DEI_TABLE =
{
	1,
	{
		{ dump_RDD_DSCP_TO_PBITS_DEI_ENTRY, 0x3fb0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT GPON_SKB_ENQUEUED_INDEXES_FIFO =
{
	48,
	{
		{ dump_RDD_GPON_SKB_ENQUEUED_INDEXES_FIFO_ENTRY, 0x4000 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT VLAN_COMMAND_INDEX_TABLE =
{
#if defined OREN
	8,
	{
		{ dump_RDD_VLAN_COMMAND_INDEX_ENTRY, 0x4600 },
#endif
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_SSID_MAC_ADDRESS_TABLE =
{
	8,
	{
		{ dump_RDD_WLAN_MCAST_SSID_MAC_ADDRESS_ENTRY, 0x4e00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DHD_FLOW_RING_SHADOW_WR_PTR_TABLE =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0x4f80 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT SERVICE_QUEUES_CFG =
{
	20,
	{
		{ dump_RDD_SERVICE_QUEUES_CFG_ENTRY, 0x4fa0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT MAC_EXTENSION_TABLE_CAM =
{
	1,
	{
		{ dump_RDD_MAC_EXTENSION_ENTRY, 0x4fb8 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT PM_COUNTERS_BUFFER =
{
	32,
	{
		{ dump_RDD_PM_COUNTERS_BUFFER, 0x4fe0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_INGRESS_CLASSIFICATION_LOOKUP_LONG_TABLE =
{
	16,
	{
		{ dump_RDD_INGRESS_CLASSIFICATION_LONG_LOOKUP_ENTRY, 0x5000 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT PM_COUNTERS =
{
	6144,
	{
		{ dump_RDD_PM_COUNTERS, 0x5800 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT WAN_DDR_QUEUE_ADDRESS_TABLE =
{
	8,
	{
		{ dump_RDD_DDR_QUEUE_ADDRESS_ENTRY, 0x7000 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT LAN_VID_CONTEXT_TABLE =
{
	2,
	{
		{ dump_RDD_LAN_VID_CONTEXT_ENTRY, 0x7400 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT RING_DESCRIPTORS_TABLE =
{
	16,
	{
		{ dump_RDD_RING_DESCRIPTOR, 0x7500 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_SSID_STATS_TABLE =
{
	8,
	{
		{ dump_RDD_WLAN_MCAST_SSID_STATS_ENTRY, 0x7600 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT RUNNER_A_SCRATCHPAD =
{
	256,
	{
		{ dump_RDD_RUNNER_SCRATCHPAD, 0x7800 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT CPU_RX_RUNNER_A_SCRATCHPAD =
{
	256,
	{
		{ dump_RDD_RUNNER_SCRATCHPAD, 0x7900 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_RUNNER_A_SCRATCHPAD =
{
	256,
	{
		{ dump_RDD_RUNNER_SCRATCHPAD, 0x7a00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT SERVICE_QUEUES_DDR_QUEUE_ADDRESS_TABLE =
{
	8,
	{
		{ dump_RDD_DDR_QUEUE_ADDRESS_ENTRY, 0x7b00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_RING_PACKET_DESCRIPTORS_CACHE =
{
	16,
	{
		{ dump_RDD_CPU_RX_DESCRIPTOR, 0x7c00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT CONNECTION_BUFFER_TABLE =
{
	16,
	{
		{ dump_RDD_CONNECTION_ENTRY, 0x7cc0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT LOCAL_SWITCHING_MULTICAST_LAN_ENQUEUE_INGRESS_QUEUE =
{
	8,
	{
		{ dump_RDD_BBH_RX_DESCRIPTOR, 0x7e00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_INGRESS_CLASSIFICATION_LOOKUP_LONG_TABLE =
{
	16,
	{
		{ dump_RDD_INGRESS_CLASSIFICATION_LONG_LOOKUP_ENTRY, 0x8000 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT WAN_EXT_DDR_CACHE_FIFO =
{
	8,
	{
		{ dump_RDD_BBH_TX_DESCRIPTOR, 0x8800 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT LAYER2_GRE_TUNNEL_TABLE =
{
	32,
	{
		{ dump_RDD_LAYER2_GRE_TUNNEL_ENTRY, 0x9000 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT EPON_DDR_QUEUE_ADDRESS_TABLE =
{
	8,
	{
		{ dump_RDD_DDR_QUEUE_ADDRESS_ENTRY, 0x9180 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DUAL_STACK_LITE_TABLE =
{
	64,
	{
		{ dump_RDD_DUAL_STACK_LITE_ENTRY, 0x9200 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_INGRESS_CLASSIFICATION_LOOKUP_CAM_TABLE =
{
	8,
	{
		{ dump_RDD_DS_INGRESS_CLASSIFICATION_IH_LOOKUP_ENTRY, 0x9300 },
		{ dump_RDD_US_INGRESS_CLASSIFICATION_IH_LOOKUP_ENTRY, 0x9300 },
		{ dump_RDD_DS_INGRESS_CLASSIFICATION_OPTIMIZED_LOOKUP_ENTRY, 0x9300 },
		{ dump_RDD_US_INGRESS_CLASSIFICATION_OPTIMIZED_LOOKUP_ENTRY, 0x9300 },
		{ dump_RDD_INGRESS_CLASSIFICATION_SHORT_LOOKUP_ENTRY, 0x9300 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT EPON_DDR_CACHE_FIFO =
{
	8,
	{
		{ dump_RDD_BBH_TX_DESCRIPTOR, 0x9400 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT IP_SYNC_1588_DESCRIPTOR_QUEUE =
{
	16,
	{
		{ dump_RDD_IP_SYNC_1588_DESCRIPTOR_ENTRY, 0x9a00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT RUNNER_B_SCRATCHPAD =
{
	256,
	{
		{ dump_RDD_RUNNER_SCRATCHPAD, 0x9b00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_INGRESS_CLASSIFICATION_LOOKUP_CAM_TABLE =
{
	8,
	{
		{ dump_RDD_DS_INGRESS_CLASSIFICATION_IH_LOOKUP_ENTRY, 0x9c00 },
		{ dump_RDD_US_INGRESS_CLASSIFICATION_IH_LOOKUP_ENTRY, 0x9c00 },
		{ dump_RDD_DS_INGRESS_CLASSIFICATION_OPTIMIZED_LOOKUP_ENTRY, 0x9c00 },
		{ dump_RDD_US_INGRESS_CLASSIFICATION_OPTIMIZED_LOOKUP_ENTRY, 0x9c00 },
		{ dump_RDD_INGRESS_CLASSIFICATION_SHORT_LOOKUP_ENTRY, 0x9c00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT CPU_RX_RUNNER_B_SCRATCHPAD =
{
	256,
	{
		{ dump_RDD_RUNNER_SCRATCHPAD, 0x9d00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DHD_FLOW_RING_CACHE_CTX_TABLE =
{
	16,
	{
		{ dump_RDD_DHD_FLOW_RING_CACHE_CTX_ENTRY, 0x9e00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_RING_PACKET_DESCRIPTORS_CACHE =
{
	16,
	{
		{ dump_RDD_CPU_RX_DESCRIPTOR, 0x9f00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DUMMY_RATE_CONTROLLER_DESCRIPTOR =
{
	64,
	{
		{ dump_RDD_DUMMY_RATE_CONTROLLER_DESCRIPTOR, 0x9fc0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_INGRESS_CLASSIFICATION_LOOKUP_TABLE =
{
	8,
	{
		{ dump_RDD_DS_INGRESS_CLASSIFICATION_IH_LOOKUP_ENTRY, 0xa000 },
		{ dump_RDD_US_INGRESS_CLASSIFICATION_IH_LOOKUP_ENTRY, 0xa000 },
		{ dump_RDD_DS_INGRESS_CLASSIFICATION_OPTIMIZED_LOOKUP_ENTRY, 0xa000 },
		{ dump_RDD_US_INGRESS_CLASSIFICATION_OPTIMIZED_LOOKUP_ENTRY, 0xa000 },
		{ dump_RDD_INGRESS_CLASSIFICATION_SHORT_LOOKUP_ENTRY, 0xa000 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_INGRESS_CLASSIFICATION_LOOKUP_TABLE =
{
	8,
	{
		{ dump_RDD_DS_INGRESS_CLASSIFICATION_IH_LOOKUP_ENTRY, 0xa800 },
		{ dump_RDD_US_INGRESS_CLASSIFICATION_IH_LOOKUP_ENTRY, 0xa800 },
		{ dump_RDD_DS_INGRESS_CLASSIFICATION_OPTIMIZED_LOOKUP_ENTRY, 0xa800 },
		{ dump_RDD_US_INGRESS_CLASSIFICATION_OPTIMIZED_LOOKUP_ENTRY, 0xa800 },
		{ dump_RDD_INGRESS_CLASSIFICATION_SHORT_LOOKUP_ENTRY, 0xa800 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT WAN_TX_QUEUES_TABLE =
{
	16,
	{
		{ dump_RDD_WAN_TX_QUEUE_DESCRIPTOR, 0xb000 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DHD_RADIO_INSTANCE_COMMON_B_DATA =
{
	64,
	{
		{ dump_RDD_DHD_RADIO_INSTANCE_COMMON_B_ENTRY, 0xc000 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT EPON_DDR_QUEUE_DESCRIPTORS_TABLE =
{
	16,
	{
		{ dump_RDD_DDR_QUEUE_DESCRIPTOR, 0xc0c0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT LOCAL_SWITCHING_PBITS_TO_QOS_TABLE =
{
	1,
	{
		{ dump_RDD_PBITS_TO_QOS_ENTRY, 0xc1c0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DUMMY_WAN_TX_QUEUE_DESCRIPTOR =
{
	16,
	{
		{ dump_RDD_DUMMY_WAN_TX_QUEUE_DESCRIPTOR, 0xc1f0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT LOCAL_SWITCHING_LAN_ENQUEUE_INGRESS_QUEUE =
{
	8,
	{
		{ dump_RDD_BBH_RX_DESCRIPTOR, 0xc200 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT SRC_MAC_ANTI_SPOOFING_TABLE =
{
	4,
	{
		{ dump_RDD_SRC_MAC_ANTI_SPOOFING_ENTRY, 0xc280 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT WIFI_SSID_FORWARDING_MATRIX_TABLE =
{
	2,
	{
		{ dump_RDD_WIFI_SSID_FORWARDING_MATRIX_ENTRY, 0xc2e0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT GPON_SKB_ENQUEUED_INDEXES_FREE_PTR =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xc300 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_DHD_TX_POST_FLOW_RING_MGMT_DESCRIPTOR_BUFFER =
{
	16,
	{
		{ dump_RDD_DHD_TX_POST_FLOW_RING_MGMT_DESCRIPTOR, 0xc350 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT SPEED_SERVICE_STREAM_PREFIX =
{
	16,
	{
		{ dump_RDD_SIXTEEN_BYTES, 0xc360 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT BRIDGE_PORT_TO_BROADCOM_SWITCH_PORT_MAPPING_TABLE =
{
	1,
	{
		{ dump_RDD_BROADCOM_SWITCH_PORT_MAPPING, 0xc370 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT SPEED_SERVICE_RX_TIMESTAMPS_TABLE =
{
	8,
	{
		{ dump_RDD_SPEED_SERVICE_RX_TIMESTAMPS, 0xc378 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT GPON_SKB_ENQUEUED_INDEXES_PUT_PTR =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xc380 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT BRIDGE_PORT_TO_BBH_PERIPHERAL_RX_TABLE =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xc3d0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT COMMON_B_DUMMY_STORE =
{
	1,
	{
		{ dump_RDD_DUMMY_STORE_ENTRY, 0xc3d7 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT EGRESS_VLAN_SWITCHING_ISOLATION_CONFIG_TABLE =
{
	1,
	{
		{ dump_RDD_VLAN_SWITCHING_ISOLATION_CONFIG_ENTRY, 0xc3d8 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DHD_FLOW_RING_CACHE_CTX_NEXT_INDEX =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xc3dd },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT BRIDGE_PORT_TO_BROADCOM_SWITCH_PORT_MAPPING_TABLE_PTR =
{
	2,
	{
		{ dump_RDD_BRIDGE_PORT_TO_BROADCOM_SWITCH_PORT_MAPPING_TABLE_PTR, 0xc3de },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT CPU_TX_DESCRIPTOR_QUEUE_TAIL_TABLE =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xc3e0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT CPU_TX_PICO_US_FLOODING_QUEUE_PTR =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xc3e4 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT WAN_PHYSICAL_PORT =
{
	2,
	{
		{ dump_RDD_WAN_PHYSICAL_PORT, 0xc3e6 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT RATE_CONTROLLER_EXPONENT_TABLE =
{
	1,
	{
		{ dump_RDD_RATE_CONTROLLER_EXPONENT_ENTRY, 0xc3e8 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT PACKET_SRAM_TO_DDR_COPY_BUFFER_1 =
{
	128,
	{
		{ dump_RDD_PACKET_SRAM_TO_DDR_COPY_BUFFER, 0xc400 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT PACKET_SRAM_TO_DDR_COPY_BUFFER_2 =
{
	128,
	{
		{ dump_RDD_PACKET_SRAM_TO_DDR_COPY_BUFFER, 0xc480 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT LAN0_INGRESS_FIFO =
{
	64,
	{
		{ dump_RDD_LAN_INGRESS_FIFO_ENTRY, 0xc500 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_CPU_REASON_TO_CPU_RX_QUEUE_TABLE =
{
	1,
	{
		{ dump_RDD_CPU_REASON_TO_CPU_RX_QUEUE_ENTRY, 0xc540 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT LAN1_INGRESS_FIFO =
{
	64,
	{
		{ dump_RDD_LAN_INGRESS_FIFO_ENTRY, 0xc580 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT MULTICAST_ACTIVE_PORTS_TABLE =
{
	1,
	{
		{ dump_RDD_MULTICAST_ACTIVE_PORTS_ENTRY, 0xc5c0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT LAN2_INGRESS_FIFO =
{
	64,
	{
		{ dump_RDD_LAN_INGRESS_FIFO_ENTRY, 0xc600 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT CPU_TX_EMAC_LOOPBACK_QUEUE =
{
	8,
	{
		{ dump_RDD_BBH_RX_DESCRIPTOR, 0xc640 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT LAN3_INGRESS_FIFO =
{
	64,
	{
		{ dump_RDD_LAN_INGRESS_FIFO_ENTRY, 0xc680 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT CPU_TX_US_FLOODING_QUEUE =
{
	8,
	{
		{ dump_RDD_BBH_RX_DESCRIPTOR, 0xc6c0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT LAN4_INGRESS_FIFO =
{
	64,
	{
		{ dump_RDD_LAN_INGRESS_FIFO_ENTRY, 0xc700 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_DHD_FLOW_RING_CACHE_LKP_TABLE =
{
	2,
	{
		{ dump_RDD_DHD_FLOW_RING_CACHE_LKP_ENTRY, 0xc740 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT WAN_ENQUEUE_INGRESS_QUEUE =
{
	1,
	{
		{ dump_RDD_INGRESS_QUEUE_ENTRY, 0xc780 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT BPM_REPLY_RUNNER_B =
{
	48,
	{
		{ dump_RDD_BPM_REPLY, 0xc7d0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT WAN_DDR_CACHE_FIFO =
{
	8,
	{
		{ dump_RDD_BBH_TX_DESCRIPTOR, 0xc800 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_RATE_CONTROLLERS_TABLE =
{
	48,
	{
		{ dump_RDD_US_RATE_CONTROLLER_DESCRIPTOR, 0xe800 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_CONNECTION_TABLE =
{
	16,
	{
		{ dump_RDD_CONNECTION_ENTRY, 0x0 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_CONNECTION_TABLE =
{
	16,
	{
		{ dump_RDD_CONNECTION_ENTRY, 0x80000 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT CONTEXT_TABLE =
{
	64,
	{
		{ dump_RDD_FLOW_CACHE_CONTEXT_ENTRY, 0x100000 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT FIREWALL_RULES_TABLE =
{
	16,
	{
		{ dump_RDD_FIREWALL_RULE_ENTRY, 0x202000 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DDR_ADDRESS_FOR_SYNC_DMA =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0x203000 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT EPON_TX_POST_SCHEDULING_DDR_QUEUES =
{
	8,
	{
		{ dump_RDD_BBH_TX_DESCRIPTOR, 0x240000 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT IPTV_DDR_CONTEXT_TABLE =
{
#if defined OREN
	16,
	{
		{ dump_RDD_IPTV_DDR_CONTEXT_ENTRY, 0x300000 },
#endif
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT IPTV_DDR_LOOKUP_TABLE =
{
	16,
	{
		{ dump_RDD_IPTV_L2_DDR_LOOKUP_ENTRY, 0x340000 },
		{ dump_RDD_IPTV_L3_DDR_LOOKUP_ENTRY, 0x340000 },
		{ dump_RDD_IPTV_L3_SSM_DDR_LOOKUP_ENTRY, 0x340000 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT IPTV_SSM_DDR_CONTEXT_TABLE =
{
#if defined OREN
	16,
	{
		{ dump_RDD_IPTV_DDR_CONTEXT_ENTRY, 0x380000 },
#endif
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT FIREWALL_RULES_MAP_TABLE =
{
	1,
	{
		{ dump_RDD_FIREWALL_RULES_MAP_ENTRY, 0x400000 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_DHD_LIST_TABLE =
{
	64,
	{
		{ dump_RDD_WLAN_MCAST_DHD_LIST_ENTRY_ARRAY, 0x5c0000 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_DHD_LIST_FORMAT_TABLE =
{
	1,
	{
		{ dump_RDD_WLAN_MCAST_DHD_LIST_ENTRY, 0x5c1000 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DHD_RX_POST_DDR_BUFFER =
{
	32,
	{
		{ dump_RDD_DHD_RX_POST_DESCRIPTOR, 0x5c1100 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DHD_RX_COMPLETE_DDR_BUFFER =
{
	32,
	{
		{ dump_RDD_DHD_RX_COMPLETE_DESCRIPTOR, 0x5c9100 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT R2D_WR_ARR_DDR_BUFFER =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0x5d1100 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT D2R_RD_ARR_DDR_BUFFER =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0x5d1200 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT R2D_RD_ARR_DDR_BUFFER =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0x5d1300 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT D2R_WR_ARR_DDR_BUFFER =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0x5d1400 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DHD_TX_POST_FLOW_RING_MGMT_DESCRIPTOR_TABLE =
{
	16,
	{
		{ dump_RDD_DHD_TX_POST_FLOW_RING_MGMT_DESCRIPTOR, 0x5d1500 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DHD_TX_POST_DDR_BUFFER =
{
	48,
	{
		{ dump_RDD_DHD_TX_POST_DESCRIPTOR, 0x5d1580 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DHD_TX_COMPLETE_DDR_BUFFER =
{
	16,
	{
		{ dump_RDD_DHD_TX_COMPLETE_DESCRIPTOR, 0x5d2d80 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT DS_INGRESS_CLASSIFICATION_COUNTERS_TABLE =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0x1fc00 },
		{ 0, 0 },
	}
};
#endif
#if defined OREN
static DUMP_RUNNERREG_STRUCT US_INGRESS_CLASSIFICATION_COUNTERS_TABLE =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0x1fe00 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT INGRESS_HANDLER_BUFFER =
{
	256,
	{
		{ dump_RDD_IH_BUFFER, 0x0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT G9991_DDR_CACHE_FIFO =
{
	8,
	{
		{ dump_RDD_BBH_TX_DESCRIPTOR, 0x2000 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT VLAN_COMMAND_INDEX_TABLE =
{
#if defined G9991
	24,
	{
		{ dump_RDD_VLAN_COMMAND_INDEX_ENTRY, 0x4d00 },
#endif
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_PBITS_TO_PBITS_TABLE =
{
	1,
	{
		{ dump_RDD_PBITS_TO_PBITS_ENTRY, 0x6500 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_WAN_FLOW_TABLE =
{
	2,
	{
		{ dump_RDD_DS_WAN_FLOW_ENTRY, 0x6600 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_INGRESS_CLASSIFICATION_CONTEXT_TABLE =
{
#if defined G9991
	8,
	{
		{ dump_RDD_DS_INGRESS_CLASSIFICATION_CONTEXT_ENTRY, 0x6800 },
#endif
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_INGRESS_FILTERS_LOOKUP_TABLE =
{
	4,
	{
		{ dump_RDD_INGRESS_FILTERS_LOOKUP_ENTRY, 0x7000 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT ETH_TX_RS_QUEUE_DESCRIPTOR_TABLE =
{
	16,
	{
		{ dump_RDD_ETH_TX_RS_QUEUE_DESCRIPTOR, 0x7500 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT ETH_TX_QUEUES_TABLE =
{
	16,
	{
		{ dump_RDD_ETH_TX_QUEUE_DESCRIPTOR, 0x7d00 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_QUEUE_PROFILE_TABLE =
{
	16,
	{
		{ dump_RDD_QUEUE_PROFILE, 0x8480 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_PBITS_PARAMETER_TABLE =
{
	2,
	{
		{ dump_RDD_PBITS_PARAMETER_ENTRY, 0x8500 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT IPTV_SSID_EXTENSION_TABLE =
{
	2,
	{
		{ dump_RDD_IPTV_SSID_EXTENSION_ENTRY, 0x8600 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_CPU_REASON_TO_METER_TABLE =
{
	1,
	{
		{ dump_RDD_CPU_REASON_TO_METER_ENTRY, 0x8800 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT IPTV_INGRESS_CLASSIFICATION_CONTEXT_TABLE =
{
	1,
	{
		{ dump_RDD_IPTV_INGRESS_CLASSIFICATION_CONTEXT_ENTRY, 0x8840 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_MAIN_TIMER_TASK_DESCRIPTOR_TABLE =
{
	8,
	{
		{ dump_RDD_TIMER_TASK_DESCRIPTOR_ENTRY, 0x8960 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT SBPM_REPLY =
{
	128,
	{
		{ dump_RDD_SBPM_REPLY_ENTRY, 0x8980 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_VLAN_PARAMETER_TABLE =
{
	4,
	{
		{ dump_RDD_VLAN_PARAMETER_ENTRY, 0x8a00 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT IPTV_COUNTERS_TABLE =
{
	2,
	{
		{ dump_RDD_IPTV_COUNTER_ENTRY, 0x8c00 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT RATE_LIMITER_REMAINDER_TABLE =
{
	2,
	{
		{ dump_RDD_RATE_LIMITER_REMAINDER_ENTRY, 0x8e40 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_LAYER4_FILTERS_LOOKUP_TABLE =
{
	8,
	{
		{ dump_RDD_LAYER4_FILTERS_LOOKUP_ENTRY, 0x8e80 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_LAN_VID_TABLE =
{
	2,
	{
		{ dump_RDD_VID_ENTRY, 0x8f00 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT ETH_TX_QUEUES_POINTERS_TABLE =
{
	4,
	{
		{ dump_RDD_ETH_TX_QUEUE_POINTERS_ENTRY, 0x9000 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_PICO_TIMER_TASK_DESCRIPTOR_TABLE =
{
	8,
	{
		{ dump_RDD_TIMER_TASK_DESCRIPTOR_ENTRY, 0x91e0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_PBITS_PRIMITIVE_TABLE =
{
	2,
	{
		{ dump_RDD_PBITS_PRIMITIVE_ENTRY, 0x9200 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_FAST_CPU_TX_DESCRIPTOR_ABS_DATA_PTR_QUEUE =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0x9240 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_PICO_CPU_TX_DESCRIPTOR_ABS_DATA_PTR_QUEUE =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0x9280 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_INGRESS_CLASSIFICATION_KEY_PRIMITIVE_TABLE =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0x92c0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_RATE_LIMITER_TABLE =
{
	8,
	{
		{ dump_RDD_RATE_LIMITER_ENTRY, 0x9300 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_VLAN_COMMANDS_TABLE =
{
	8,
	{
		{ dump_RDD_VLAN_COMMAND_ENRTY, 0x9400 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_POLICER_TABLE =
{
	16,
	{
		{ dump_RDD_POLICER_ENTRY, 0x9600 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT ETH_TX_LOCAL_REGISTERS =
{
	8,
	{
		{ dump_RDD_ETH_TX_LOCAL_REGISTERS_ENTRY, 0x9700 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_TPID_OVERWRITE_TABLE =
{
	2,
	{
		{ dump_RDD_TPID_OVERWRITE_ENTRY, 0x97f0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_CPU_TX_BBH_DESCRIPTORS =
{
	8,
	{
		{ dump_RDD_CPU_TX_BBH_DESCRIPTORS_ENTRY, 0x9800 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_INGRESS_FILTERS_PARAMETER_TABLE =
{
	1,
	{
		{ dump_RDD_INGRESS_FILTERS_PARAMETER_ENTRY, 0x9900 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_INGRESS_CLASSIFICATION_GENERIC_RULE_CFG_TABLE =
{
	8,
	{
		{ dump_RDD_INGRESS_CLASSIFICATION_GENERIC_RULE_CFG_ENTRY, 0x99a0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT IPTV_SSID_EXTENSION_TABLE_CAM =
{
	64,
	{
		{ dump_RDD_IPTV_SSID_EXTENSION_TABLE_CAM, 0x99c0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT CPU_RX_PD_INGRESS_QUEUE =
{
	8,
	{
		{ dump_RDD_EIGHT_BYTES, 0x9a00 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_CONTROL_TABLE =
{
	148,
	{
		{ dump_RDD_WLAN_MCAST_CONTROL_ENTRY, 0x9b00 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT ETH_TX_EMACS_STATUS =
{
#if defined G9991
	4,
	{
		{ dump_RDD_ETH_TX_EMACS_STATUS_ENTRY, 0x9b94 },
#endif
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_CPU_PARAMETERS_BLOCK =
{
	8,
	{
		{ dump_RDD_CPU_PARAMETERS_BLOCK_ENTRY, 0x9b98 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_FAST_RUNNER_GLOBAL_REGISTERS_INIT =
{
	4,
	{
		{ dump_RDD_FAST_RUNNER_GLOBAL_REGISTERS_INIT_ENTRY, 0x9ba0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_FLOW_BASED_ACTION_PTR_TABLE =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0x9bc0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT CPU_RX_FAST_PD_INGRESS_QUEUE =
{
	8,
	{
		{ dump_RDD_EIGHT_BYTES, 0x9c00 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_FORWARDING_MATRIX_TABLE =
{
	1,
	{
		{ dump_RDD_FORWARDING_MATRIX_ENTRY, 0x9d00 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_TIMER_SCHEDULER_PRIMITIVE_TABLE =
{
	2,
	{
		{ dump_RDD_TIMER_SCHEDULER_PRIMITIVE_ENTRY, 0x9d90 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_PICO_RUNNER_GLOBAL_REGISTERS_INIT =
{
	4,
	{
		{ dump_RDD_PICO_RUNNER_GLOBAL_REGISTERS_INIT_ENTRY, 0x9da0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_TRAFFIC_CLASS_TO_QUEUE_TABLE =
{
	1,
	{
		{ dump_RDD_QUEUE_ENTRY, 0x9dc0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT EMAC_SKB_ENQUEUED_INDEXES_PUT_PTR =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0x9df0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_BRIDGE_CONFIGURATION_REGISTER =
{
	256,
	{
		{ dump_RDD_BRIDGE_CONFIGURATION_REGISTER, 0x9e00 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_CPU_RX_METER_TABLE =
{
	8,
	{
		{ dump_RDD_CPU_RX_METER_ENTRY, 0x9f00 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT INGRESS_HANDLER_SKB_DATA_POINTER =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0x9f80 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT ETH_TX_MAC_TABLE =
{
#if defined G9991
	64,
	{
		{ dump_RDD_ETH_TX_MAC_DESCRIPTOR, 0xa000 },
#endif
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_INGRESS_CLASSIFICATION_RULE_CFG_TABLE =
{
	8,
	{
		{ dump_RDD_INGRESS_CLASSIFICATION_RULE_CFG_ENTRY, 0xa780 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_PROFILING_BUFFER_PICO_RUNNER =
{
	256,
	{
		{ dump_RDD_PROFILING_BUFFER_PICO_RUNNER, 0xa800 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DOWNSTREAM_MULTICAST_LAN_ENQUEUE_INGRESS_QUEUE =
{
	8,
	{
		{ dump_RDD_EIGHT_BYTES, 0xa900 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_VLAN_OPTIMIZATION_TABLE =
{
	1,
	{
		{ dump_RDD_VLAN_OPTIMIZATION_ENTRY, 0xa980 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DOWNSTREAM_MULTICAST_LAN_ENQUEUE_SERVICE_QUEUE =
{
	8,
	{
		{ dump_RDD_EIGHT_BYTES, 0xaa00 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT PCI_MULTICAST_SCRATCHPAD =
{
	128,
	{
		{ dump_RDD_PCI_MULTICAST_SCRATCHPAD, 0xaa80 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT CPU_RX_MIRRORING_PD_INGRESS_QUEUE =
{
	8,
	{
		{ dump_RDD_EIGHT_BYTES, 0xab00 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_RUNNER_FLOW_HEADER_BUFFER =
{
	128,
	{
		{ dump_RDD_RUNNER_FLOW_HEADER_BUFFER, 0xab80 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT CPU_TX_FAST_QUEUE =
{
	8,
	{
		{ dump_RDD_CPU_TX_DESCRIPTOR, 0xac00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_CORE, 0xac00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_BPM, 0xac00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_ABS, 0xac00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_DS_FAST, 0xac00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_US_FAST, 0xac00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_DS_PICO, 0xac00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_DS_PICO_WIFI, 0xac00 },
		{ dump_RDD_CPU_TX_MESSAGE_DESCRIPTOR, 0xac00 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DOWNSTREAM_LAN_ENQUEUE_INGRESS_QUEUE =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xac80 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DHD_TX_COMPLETE_FLOW_RING_BUFFER =
{
	16,
	{
		{ dump_RDD_DHD_TX_COMPLETE_DESCRIPTOR, 0xacc0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT EMAC_SKB_ENQUEUED_INDEXES_FREE_PTR =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xacf0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT CPU_TX_PICO_QUEUE =
{
	8,
	{
		{ dump_RDD_CPU_TX_DESCRIPTOR, 0xad00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_CORE, 0xad00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_BPM, 0xad00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_ABS, 0xad00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_DS_FAST, 0xad00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_US_FAST, 0xad00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_DS_PICO, 0xad00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_DS_PICO_WIFI, 0xad00 },
		{ dump_RDD_CPU_TX_MESSAGE_DESCRIPTOR, 0xad00 },
		{ dump_RDD_CPU_TX_DHD_DESCRIPTOR, 0xad00 },
		{ dump_RDD_CPU_TX_DHD_MESSAGE_DESCRIPTOR, 0xad00 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT EMAC_SKB_ENQUEUED_INDEXES_FIFO =
{
	32,
	{
		{ dump_RDD_EMAC_SKB_ENQUEUED_INDEXES_FIFO_ENTRY, 0xad80 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT G9991_FRAGMENT_BUFFER =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xae20 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT ETH_TX_INTER_LAN_SCHEDULING_OFFSET =
{
#if defined G9991
	4,
	{
		{ dump_RDD_ETH_TX_INTER_LAN_SCHEDULING_OFFSET_ENTRY, 0xaea4 },
#endif
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT FIREWALL_CONFIGURATION_REGISTER =
{
	8,
	{
		{ dump_RDD_FIREWALL_CONFIGURATION_REGISTER_ENTRY, 0xaea8 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT HASH_BUFFER =
{
	16,
	{
		{ dump_RDD_HASH_BUFFER, 0xaeb0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DHD_COMPLETE_RING_DESCRIPTOR_BUFFER =
{
	16,
	{
		{ dump_RDD_DHD_COMPLETE_RING_DESCRIPTOR, 0xaec0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT IPTV_DMA_LKP_KEY =
{
	16,
	{
		{ dump_RDD_IPTV_DMA_LKP_KEY, 0xaef0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT G9991_FRAGMENT_ENQUEUE_INGRESS_QUEUE =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xaf00 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_DSCP_TO_PBITS_TABLE =
{
	1,
	{
		{ dump_RDD_DSCP_TO_PBITS_ENTRY, 0xaf40 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DOWNSTREAM_LAN_ENQUEUE_SERVICE_QUEUE =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xaf80 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_VLAN_PRIMITIVE_TABLE =
{
	2,
	{
		{ dump_RDD_VLAN_PRIMITIVE_ENTRY, 0xafc0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT PACKET_SRAM_TO_DDR_COPY_BUFFER =
{
	128,
	{
		{ dump_RDD_PACKET_SRAM_TO_DDR_COPY_BUFFER, 0xb000 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_CPU_RX_PICO_INGRESS_QUEUE =
{
	1,
	{
		{ dump_RDD_INGRESS_QUEUE_ENTRY, 0xb080 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_CPU_TX_MESSAGE_DATA_BUFFER =
{
	64,
	{
		{ dump_RDD_CPU_TX_MESSAGE_DATA_BUFFER_ENTRY, 0xb0c0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DOWNSTREAM_MULTICAST_INGRESS_QUEUE =
{
	1,
	{
		{ dump_RDD_INGRESS_QUEUE_ENTRY, 0xb100 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_VLAN_ACTION_BUFFER =
{
	64,
	{
		{ dump_RDD_VLAN_ACTION_BUFFER, 0xb140 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_ROUTER_INGRESS_QUEUE =
{
	1,
	{
		{ dump_RDD_INGRESS_QUEUE_ENTRY, 0xb180 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_DHD_TX_POST_FLOW_RING_BUFFER =
{
	48,
	{
		{ dump_RDD_DHD_TX_POST_DESCRIPTOR, 0xb1c0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_INGRESS_FILTERS_CONFIGURATION_TABLE =
{
	4,
	{
		{ dump_RDD_INGRESS_FILTERS_CONFIGURATION_ENTRY, 0xb1f0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT G9991_VIRTUAL_PORT_RATE_LIMITER_STATUS =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0xb1fc },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_CPU_RX_FAST_INGRESS_QUEUE =
{
	1,
	{
		{ dump_RDD_INGRESS_QUEUE_ENTRY, 0xb200 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DHD_COMPLETE_RING_BUFFER =
{
	8,
	{
		{ dump_RDD_DHD_COMPLETE_RING_ENTRY, 0xb240 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_RUNNER_FLOW_HEADER_DESCRIPTOR =
{
	8,
	{
		{ dump_RDD_RUNNER_FLOW_HEADER_DESCRIPTOR, 0xb258 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_DATA_POINTER_DUMMY_TARGET =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0xb260 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT BPM_DDR_OPTIMIZED_BUFFERS_WITHOUT_HEADROOM_BASE =
{
	4,
	{
		{ dump_RDD_BPM_DDR_OPTIMIZED_BUFFERS_WITHOUT_HEADROOM_BASE, 0xb274 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_NULL_BUFFER =
{
	8,
	{
		{ dump_RDD_EIGHT_BYTES, 0xb278 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT CPU_FLOW_CACHE_INGRESS_QUEUE =
{
	1,
	{
		{ dump_RDD_INGRESS_QUEUE_ENTRY, 0xb280 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT EMAC_ABSOLUTE_TX_BBH_COUNTER =
{
	8,
	{
		{ dump_RDD_EIGHT_BYTES, 0xb2c0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT FREE_PACKET_DESCRIPTORS_POOL_DESCRIPTOR =
{
	8,
	{
		{ dump_RDD_FREE_PACKET_DESCRIPTORS_POOL_DESCRIPTOR_ENTRY, 0xb2f0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_DEBUG_BUFFER =
{
	4,
	{
		{ dump_RDD_DEBUG_BUFFER_ENTRY, 0xb2f8 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_ONE_BUFFER =
{
	8,
	{
		{ dump_RDD_EIGHT_BYTES, 0xb378 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_DHD_TX_POST_INGRESS_QUEUE =
{
	1,
	{
		{ dump_RDD_INGRESS_QUEUE_ENTRY, 0xb380 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_DHD_FLOW_RING_DROP_COUNTER =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0xb3c0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_BPM_EXTRA_DDR_BUFFERS_BASE =
{
	4,
	{
		{ dump_RDD_BPM_EXTRA_DDR_BUFFERS_BASE, 0xb3d4 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_SSID_STATS_STATE_TABLE =
{
	6,
	{
		{ dump_RDD_WLAN_MCAST_SSID_STATS_STATE_ENTRY, 0xb3d8 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_MAIN_TIMER_CONTROL_DESCRIPTOR =
{
	2,
	{
		{ dump_RDD_TIMER_CONTROL_DESCRIPTOR, 0xb3de },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_FAST_FREE_SKB_INDEXES_FIFO_LOCAL_TABLE =
{
	2,
	{
		{ dump_RDD_FREE_SKB_INDEXES_FIFO_ENTRY, 0xb3e0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_LAYER4_FILTERS_CONTEXT_TABLE =
{
	1,
	{
		{ dump_RDD_LAYER4_FILTERS_CONTEXT_ENTRY, 0xb3f0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT GPON_RX_DIRECT_DESCRIPTORS =
{
	8,
	{
		{ dump_RDD_BBH_RX_DESCRIPTOR, 0xb400 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_INGRESS_QUEUE =
{
	1,
	{
		{ dump_RDD_INGRESS_QUEUE_ENTRY, 0xb500 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_PICO_FREE_SKB_INDEXES_FIFO_LOCAL_TABLE =
{
	2,
	{
		{ dump_RDD_FREE_SKB_INDEXES_FIFO_ENTRY, 0xb540 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_IPTV_SBPM_REPLICATION_BN =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xb550 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT IPTV_DMA_RW_BUFFER =
{
	16,
	{
		{ dump_RDD_IPTV_DMA_RW_BUFFER, 0xb570 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT MULTICAST_HEADER_BUFFER =
{
	32,
	{
		{ dump_RDD_MULTICAST_HEADER_BUFFER, 0xb580 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT GSO_TX_DHD_L2_BUFFER =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb5a0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_PICO_TIMER_CONTROL_DESCRIPTOR =
{
	2,
	{
		{ dump_RDD_TIMER_CONTROL_DESCRIPTOR, 0xb5b6 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT CPU_TX_DHD_L2_BUFFER =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb5b8 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT IPTV_COUNTERS_BUFFER =
{
	2,
	{
		{ dump_RDD_IPTV_COUNTERS_BUFFER, 0xb5ce },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_INGRESS_CLASSIFICATION_KEY_BUFFER =
{
	16,
	{
		{ dump_RDD_SIXTEEN_BYTES, 0xb5d0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT ETH_TX_QUEUE_DUMMY_DESCRIPTOR =
{
	16,
	{
		{ dump_RDD_ETH_TX_QUEUE_DUMMY_DESCRIPTOR, 0xb5e0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT FIREWALL_RULE_ENTRY_BUFFER =
{
	16,
	{
		{ dump_RDD_SIXTEEN_BYTES, 0xb5f0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT CPU_TX_DS_PICO_DHD_TX_POST_CONTEXT =
{
	16,
	{
		{ dump_RDD_DHD_TX_POST_CONTEXT_ENTRY, 0xb600 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_DHD_TX_POST_CONTEXT =
{
	16,
	{
		{ dump_RDD_DHD_TX_POST_CONTEXT_ENTRY, 0xb610 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT GSO_TX_DS_PICO_DHD_TX_POST_CONTEXT =
{
	16,
	{
		{ dump_RDD_DHD_TX_POST_CONTEXT_ENTRY, 0xb620 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT SC_BUFFER =
{
	12,
	{
		{ dump_RDD_SC_BUFFER, 0xb630 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT IPTV_SSM_CONTEXT_TABLE_PTR =
{
	4,
	{
		{ dump_RDD_IPTV_SSM_CONTEXT_TABLE_PTR, 0xb63c },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT ETH_PHYSICAL_PORT_ACK_PENDING =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb640 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT PRIVATE_A_DUMMY_STORE =
{
	1,
	{
		{ dump_RDD_DUMMY_STORE_ENTRY, 0xb645 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_INGRESS_CLASSIFICATION_COUNTERS_BUFFER =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xb646 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_MULTICAST_VECTOR_TO_PORT_TABLE =
{
	1,
	{
		{ dump_RDD_MULTICAST_VECTOR_TO_PORT_ENTRY, 0xb648 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT MULTICAST_DUMMY_VLAN_INDEXES_TABLE =
{
	8,
	{
		{ dump_RDD_MULTICAST_DUMMY_VLAN_INDEXES_TABLE, 0xb650 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT IPTV_TABLE_POINTER =
{
	4,
	{
		{ dump_RDD_IPTV_PTR_ENTRY, 0xb658 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT IPTV_CONTEXT_TABLE_POINTER =
{
	4,
	{
		{ dump_RDD_IPTV_CONTEXT_PTR_ENTRY, 0xb65c },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_BPM_DDR_OPTIMIZED_BUFFERS_BASE =
{
	4,
	{
		{ dump_RDD_BPM_DDR_OPTIMIZED_BUFFERS_BASE, 0xb660 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_BPM_DDR_BUFFERS_BASE =
{
	4,
	{
		{ dump_RDD_BPM_DDR_BUFFERS_BASE, 0xb664 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_CONNECTION_TABLE_CONFIG =
{
	4,
	{
		{ dump_RDD_CONNECTION_TABLE_CONFIG, 0xb668 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_CONTEXT_TABLE_CONFIG =
{
	4,
	{
		{ dump_RDD_CONTEXT_TABLE_CONFIG, 0xb66c },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_DEBUG_PERIPHERALS_STATUS_REGISTER =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0xb670 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_DDR_ADDRESS_FOR_SYNC_DMA_POINTER =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0xb674 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT EMAC_SKB_ENQUEUED_INDEXES_FIFO_COUNTERS =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb678 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_FAST_FREE_SKB_INDEXES_FIFO_LOCAL_TABLE_PTR =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb67d },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT CPU_TX_PICO_INGRESS_QUEUE_PTR =
{
	2,
	{
		{ dump_RDD_CPU_TX_PICO_INGRESS_QUEUE_PTR, 0xb67e },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT HASH_BASED_FORWARDING_PORT_TABLE =
{
	1,
	{
		{ dump_RDD_HASH_BASED_FORWARDING_PORT_ENTRY, 0xb680 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT FIREWALL_IPV6_R16_BUFFER =
{
	4,
	{
		{ dump_RDD_FIREWALL_IPV6_R16_BUFFER_ENTRY, 0xb684 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT FREE_PACKET_DESCRIPTORS_POOL_THRESHOLD =
{
	2,
	{
		{ dump_RDD_FREE_PACKET_DESCRIPTORS_POOL_THRESHOLD, 0xb688 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DOWNSTREAM_MULTICAST_LAN_ENQUEUE_INGRESS_QUEUE_PTR =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xb68a },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT G9991_DOWNSTREAM_MULTICAST_FRAGMENTATION_INGRESS_QUEUE_PTR =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xb68c },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT CPU_RX_PD_INGRESS_QUEUE_PTR =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xb68e },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT CPU_RX_MIRRORING_PD_INGRESS_QUEUE_PTR =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xb690 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_CPU_RX_PICO_INGRESS_QUEUE_PTR =
{
	2,
	{
		{ dump_RDD_CPU_RX_PICO_INGRESS_QUEUE_PTR, 0xb692 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_BPM_DDR_BUFFER_HEADROOM_SIZE =
{
	2,
	{
		{ dump_RDD_BPM_DDR_BUFFER_HEADROOM_SIZE, 0xb694 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_BPM_DDR_BUFFER_HEADROOM_SIZE_2_BYTE_RESOLUTION =
{
	2,
	{
		{ dump_RDD_BPM_DDR_BUFFER_HEADROOM_SIZE_2_BYTE_RESOLUTION, 0xb696 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_CPU_RX_FAST_INGRESS_QUEUE_PTR =
{
	2,
	{
		{ dump_RDD_CPU_RX_FAST_INGRESS_QUEUE_PTR, 0xb698 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_MEMLIB_SEMAPHORE =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xb69a },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_MICROCODE_VERSION =
{
	4,
	{
		{ dump_RDD_MICROCODE_VERSION_ENTRY, 0xb69c },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_RATE_LIMITER_COUNTER_BUFFER =
{
	4,
	{
		{ dump_RDD_RATE_LIMITER_COUNTER_BUFFER_ENTRY, 0xb6a0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_DDR_ADDRESS_FOR_FULL_READ_DMA_POINTER =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0xb6a4 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT GSO_TX_DHD_HOST_BUF_PTR =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb6a8 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_DHD_TX_POST_HOST_DATA_PTR_BUFFER =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0xb6ac },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT CPU_TX_DHD_HOST_BUF_PTR =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb6b0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DUAL_STACK_LITE_LAYER_2_HEADER_LENGTH =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xb6b4 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DUAL_STACK_LITE_LAYER_3_HEADER_LENGTH =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xb6b6 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DOWNSTREAM_MULTICAST_LAN_ENQUEUE_SERVICE_QUEUE_INGRESS_QUEUE_PTR =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xb6b8 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_RUNNER_CONGESTION_STATE =
{
	2,
	{
		{ dump_RDD_RUNNER_CONGESTION_STATE_ENTRY, 0xb6ba },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT FIREWALL_RULE_MAP_ENTRY_BUFFER =
{
	2,
	{
		{ dump_RDD_FIREWALL_RULE_MAP_ENTRY_BUFFER, 0xb6bc },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DHD_TX_COMPLETE_BPM_REF_COUNTER =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xb6be },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT PCI_TX_FIFO_DESCRIPTORS_TABLE =
{
	8,
	{
		{ dump_RDD_PCI_TX_FIFO_DESCRIPTOR_ENTRY, 0xb6c0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT PCI_TX_QUEUES_VECTOR =
{
	4,
	{
		{ dump_RDD_PCI_TX_QUEUES_VECTOR, 0xb6e0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT PCI_TX_FIFO_FULL_VECTOR =
{
	1,
	{
		{ dump_RDD_PCI_TX_FIFO_FULL_VECTOR_ENTRY, 0xb6e4 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_PICO_FREE_SKB_INDEXES_FIFO_LOCAL_TABLE_PTR =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb6e5 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DHD_TX_POST_CPU_BPM_REF_COUNTER =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xb6e6 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_DHD_BPM_CONGESTION_UG3_DROP_COUNTER =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xb6e8 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_DHD_BPM_CONGESTION_ALLOC_FAIL_DROP_COUNTER =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xb6ea },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT G9991_FRAGMENT_DMA_SYNC =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb6ec },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_INGRESS_CLASSIFICATION_RULE_CFG_DESCRIPTOR =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb6ed },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_INGRESS_CLASSIFICATION_IP_FLOW_RULE_CFG_DESCRIPTOR =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb6ee },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_PACKET_BUFFER_SIZE_ASR_8 =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb6ef },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT RUNNER_FLOW_IH_RESPONSE =
{
	8,
	{
		{ dump_RDD_RUNNER_FLOW_IH_RESPONSE, 0xb6f0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_MAIN_DMA_SYNCRONIZATION_ADDRESS =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb6f8 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_PICO_DMA_SYNCRONIZATION_ADDRESS =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb6f9 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_RUNNER_FLOW_IH_RESPONSE_MUTEX =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb6fa },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DOWNSTREAM_MULTICAST_SERVICE_QUEUE_SSID_PTR =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb6fb },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT FIREWALL_RULE_MAP_ENTRY_BUFFER_SYNC =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb6fc },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT IP_SYNC_1588_TX_ENQUEUE_RESULT =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb6fd },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_DHD_DMA_SYNCHRONIZATION =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb6fe },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DHD_TX_POST_CPU_SEMAPHORE =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xb6ff },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT GPON_RX_NORMAL_DESCRIPTORS =
{
	8,
	{
		{ dump_RDD_BBH_RX_DESCRIPTOR, 0xb800 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DOWNSTREAM_DMA_PIPE_BUFFER =
{
	4,
	{
		{ dump_RDD_DOWNSTREAM_DMA_PIPE_BUFFER, 0xbcb8 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_DHD_BPM_CONGESTION_ALLOC_FAIL_DROP_COUNTER =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_INGRESS_HANDLER_BUFFER =
{
	256,
	{
		{ dump_RDD_IH_BUFFER, 0x0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_FREE_PACKET_DESCRIPTORS_POOL =
{
	8,
	{
		{ dump_RDD_PACKET_DESCRIPTOR, 0x2000 },
		{ dump_RDD_BBH_TX_DESCRIPTOR, 0x2000 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_INGRESS_CLASSIFICATION_CONTEXT_TABLE =
{
	8,
	{
		{ dump_RDD_US_INGRESS_CLASSIFICATION_CONTEXT_ENTRY, 0x8000 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_DSCP_TO_PBITS_TABLE =
{
	1,
	{
		{ dump_RDD_DSCP_TO_PBITS_ENTRY, 0x8800 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_SBPM_REPLY =
{
	128,
	{
		{ dump_RDD_SBPM_REPLY_ENTRY, 0x8980 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_VLAN_PARAMETER_TABLE =
{
	4,
	{
		{ dump_RDD_VLAN_PARAMETER_ENTRY, 0x8a00 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_SID_CONTEXT_TABLE =
{
#if defined G9991
	32,
	{
		{ dump_RDD_US_SID_CONTEXT_ENTRY, 0x8c00 },
#endif
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_INGRESS_RATE_LIMITER_TABLE =
{
	16,
	{
		{ dump_RDD_INGRESS_RATE_LIMITER_ENTRY, 0x9000 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_QUEUE_PROFILE_TABLE =
{
	16,
	{
		{ dump_RDD_QUEUE_PROFILE, 0x9180 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_CPU_REASON_TO_METER_TABLE =
{
	1,
	{
		{ dump_RDD_CPU_REASON_TO_METER_ENTRY, 0x9200 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_FAST_CPU_TX_DESCRIPTOR_ABS_DATA_PTR_QUEUE =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0x9240 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_PICO_CPU_TX_DESCRIPTOR_ABS_DATA_PTR_QUEUE =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0x9280 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_PBITS_TO_QOS_TABLE =
{
	1,
	{
		{ dump_RDD_US_QUEUE_ENTRY, 0x92c0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_RATE_LIMITER_TABLE =
{
	8,
	{
		{ dump_RDD_RATE_LIMITER_ENTRY, 0x9300 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_LAYER4_FILTERS_LOOKUP_TABLE =
{
	8,
	{
		{ dump_RDD_LAYER4_FILTERS_LOOKUP_ENTRY, 0x9380 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT WAN_CHANNELS_8_39_TABLE =
{
	32,
	{
		{ dump_RDD_WAN_CHANNEL_8_39_DESCRIPTOR, 0x9400 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_WAN_FLOW_TABLE =
{
	4,
	{
		{ dump_RDD_US_WAN_FLOW_ENTRY, 0x9800 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_INGRESS_FILTERS_LOOKUP_TABLE =
{
	4,
	{
		{ dump_RDD_INGRESS_FILTERS_LOOKUP_ENTRY, 0x9c00 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_PBITS_TO_PBITS_TABLE =
{
	1,
	{
		{ dump_RDD_PBITS_TO_PBITS_ENTRY, 0x9f00 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT WAN_CHANNELS_0_7_TABLE =
{
	88,
	{
		{ dump_RDD_WAN_CHANNEL_0_7_DESCRIPTOR, 0xa000 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_TRAFFIC_CLASS_TO_QUEUE_TABLE =
{
	1,
	{
		{ dump_RDD_US_QUEUE_ENTRY, 0xa2c0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_PBITS_PARAMETER_TABLE =
{
	2,
	{
		{ dump_RDD_PBITS_PARAMETER_ENTRY, 0xa300 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_RUNNER_FLOW_HEADER_BUFFER =
{
	128,
	{
		{ dump_RDD_RUNNER_FLOW_HEADER_BUFFER, 0xa400 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_CPU_RX_METER_TABLE =
{
	8,
	{
		{ dump_RDD_CPU_RX_METER_ENTRY, 0xa580 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT SMART_CARD_DESCRIPTOR_TABLE =
{
	272,
	{
		{ dump_RDD_SMART_CARD_DESCRIPTOR_ENTRY, 0xa600 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_VLAN_COMMANDS_TABLE =
{
	8,
	{
		{ dump_RDD_VLAN_COMMAND_ENRTY, 0xa710 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_VLAN_OPTIMIZATION_TABLE =
{
	1,
	{
		{ dump_RDD_VLAN_OPTIMIZATION_ENTRY, 0xa910 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT GPON_ABSOLUTE_TX_BBH_COUNTER =
{
	1,
	{
		{ dump_RDD_GPON_ABSOLUTE_TX_COUNTER, 0xa990 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_CPU_PARAMETERS_BLOCK =
{
	8,
	{
		{ dump_RDD_CPU_PARAMETERS_BLOCK_ENTRY, 0xa9b8 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_PBITS_TO_WAN_FLOW_TABLE =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xa9c0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_LAN_VID_TABLE =
{
	2,
	{
		{ dump_RDD_VID_ENTRY, 0xaa00 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_POLICER_TABLE =
{
	16,
	{
		{ dump_RDD_POLICER_ENTRY, 0xab00 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_CPU_TX_FAST_QUEUE =
{
	8,
	{
		{ dump_RDD_CPU_TX_DESCRIPTOR, 0xac00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_CORE, 0xac00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_BPM, 0xac00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_ABS, 0xac00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_DS_FAST, 0xac00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_US_FAST, 0xac00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_DS_PICO, 0xac00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_DS_PICO_WIFI, 0xac00 },
		{ dump_RDD_CPU_TX_MESSAGE_DESCRIPTOR, 0xac00 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_INGRESS_HANDLER_SKB_DATA_POINTER =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0xac80 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_CPU_TX_PICO_QUEUE =
{
	8,
	{
		{ dump_RDD_CPU_TX_DESCRIPTOR, 0xad00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_CORE, 0xad00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_BPM, 0xad00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_ABS, 0xad00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_DS_FAST, 0xad00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_US_FAST, 0xad00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_DS_PICO, 0xad00 },
		{ dump_RDD_CPU_TX_DESCRIPTOR_DS_PICO_WIFI, 0xad00 },
		{ dump_RDD_CPU_TX_MESSAGE_DESCRIPTOR, 0xad00 },
		{ dump_RDD_CPU_TX_DHD_DESCRIPTOR, 0xad00 },
		{ dump_RDD_CPU_TX_DHD_MESSAGE_DESCRIPTOR, 0xad00 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_INGRESS_CLASSIFICATION_RULE_CFG_TABLE =
{
	8,
	{
		{ dump_RDD_INGRESS_CLASSIFICATION_RULE_CFG_ENTRY, 0xad80 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_CPU_TX_BBH_DESCRIPTORS =
{
	8,
	{
		{ dump_RDD_CPU_TX_BBH_DESCRIPTORS_ENTRY, 0xae00 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_FORWARDING_MATRIX_TABLE =
{
	1,
	{
		{ dump_RDD_FORWARDING_MATRIX_ENTRY, 0xaf00 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_RATE_CONTROL_BUDGET_ALLOCATOR_TABLE =
{
	2,
	{
		{ dump_RDD_BUDGET_ALLOCATOR_ENTRY, 0xaf90 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_MAIN_TIMER_TASK_DESCRIPTOR_TABLE =
{
	8,
	{
		{ dump_RDD_TIMER_TASK_DESCRIPTOR_ENTRY, 0xafa0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_INGRESS_CLASSIFICATION_KEY_PRIMITIVE_TABLE =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xafc0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT ETH0_RX_DESCRIPTORS =
{
	8,
	{
		{ dump_RDD_ETH_RX_DESCRIPTORS, 0xb000 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_BRIDGE_CONFIGURATION_REGISTER =
{
	256,
	{
		{ dump_RDD_BRIDGE_CONFIGURATION_REGISTER, 0xb100 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT ETH1_RX_DESCRIPTORS =
{
	8,
	{
		{ dump_RDD_ETH_RX_DESCRIPTORS, 0xb200 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_PROFILING_BUFFER_PICO_RUNNER =
{
	256,
	{
		{ dump_RDD_PROFILING_BUFFER_PICO_RUNNER, 0xb300 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_GPON_RX_DIRECT_DESCRIPTORS =
{
	8,
	{
		{ dump_RDD_BBH_RX_DESCRIPTOR, 0xb400 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_INGRESS_FILTERS_PARAMETER_TABLE =
{
	1,
	{
		{ dump_RDD_INGRESS_FILTERS_PARAMETER_ENTRY, 0xb500 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_PICO_TIMER_TASK_DESCRIPTOR_TABLE =
{
	8,
	{
		{ dump_RDD_TIMER_TASK_DESCRIPTOR_ENTRY, 0xb560 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DHD_RX_COMPLETE_FLOW_RING_BUFFER =
{
	32,
	{
		{ dump_RDD_DHD_RX_POST_DESCRIPTOR, 0xb580 },
		{ dump_RDD_DHD_RX_COMPLETE_DESCRIPTOR, 0xb580 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_INGRESS_CLASSIFICATION_GENERIC_RULE_CFG_TABLE =
{
	8,
	{
		{ dump_RDD_INGRESS_CLASSIFICATION_GENERIC_RULE_CFG_ENTRY, 0xb5e0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DHD_RX_POST_FLOW_RING_BUFFER =
{
	32,
	{
		{ dump_RDD_DHD_RX_POST_DESCRIPTOR, 0xb600 },
		{ dump_RDD_DHD_RX_COMPLETE_DESCRIPTOR, 0xb600 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_FAST_RUNNER_GLOBAL_REGISTERS_INIT =
{
	4,
	{
		{ dump_RDD_FAST_RUNNER_GLOBAL_REGISTERS_INIT_ENTRY, 0xb660 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_PBITS_PRIMITIVE_TABLE =
{
	2,
	{
		{ dump_RDD_PBITS_PRIMITIVE_ENTRY, 0xb680 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_ACK_PACKETS_QUEUE_INDEX_TABLE =
{
	1,
	{
		{ dump_RDD_US_QUEUE_ENTRY, 0xb6c0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT BBH_TX_EPON_REQUEST_WAN_CHANNEL_INDEX =
{
	8,
	{
		{ dump_RDD_EIGHT_BYTES, 0xb6e8 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_RUNNER_FLOW_IH_RESPONSE =
{
	8,
	{
		{ dump_RDD_RUNNER_FLOW_IH_RESPONSE, 0xb6f0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT SMART_CARD_ERROR_COUNTERS_TABLE =
{
	8,
	{
		{ dump_RDD_SMART_CARD_ERROR_COUNTERS_ENTRY, 0xb6f8 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_CPU_RX_PICO_INGRESS_QUEUE =
{
	1,
	{
		{ dump_RDD_INGRESS_QUEUE_ENTRY, 0xb700 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_FLOW_BASED_ACTION_PTR_TABLE =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xb740 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_ROUTER_INGRESS_QUEUE =
{
	1,
	{
		{ dump_RDD_INGRESS_QUEUE_ENTRY, 0xb780 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_INGRESS_CLASSIFICATION_COUNTERS_BUFFER =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0xb7c0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_BPM_EXTRA_DDR_BUFFERS_BASE =
{
	4,
	{
		{ dump_RDD_BPM_EXTRA_DDR_BUFFERS_BASE, 0xb7fc },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT ETH2_RX_DESCRIPTORS =
{
	8,
	{
		{ dump_RDD_ETH_RX_DESCRIPTORS, 0xb800 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_CPU_RX_FAST_INGRESS_QUEUE =
{
	1,
	{
		{ dump_RDD_INGRESS_QUEUE_ENTRY, 0xb900 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_DEBUG_BUFFER =
{
	4,
	{
		{ dump_RDD_DEBUG_BUFFER_ENTRY, 0xb940 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT SPEED_SERVICE_PARAMETERS_TABLE =
{
	42,
	{
		{ dump_RDD_SPEED_SERVICE_PARAMETERS, 0xb9c0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_MAIN_TIMER_CONTROL_DESCRIPTOR =
{
	2,
	{
		{ dump_RDD_TIMER_CONTROL_DESCRIPTOR, 0xb9ea },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_BPM_DDR_OPTIMIZED_BUFFERS_BASE =
{
	4,
	{
		{ dump_RDD_BPM_DDR_OPTIMIZED_BUFFERS_BASE, 0xb9ec },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_TPID_OVERWRITE_TABLE =
{
	2,
	{
		{ dump_RDD_TPID_OVERWRITE_ENTRY, 0xb9f0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT ETH3_RX_DESCRIPTORS =
{
	8,
	{
		{ dump_RDD_ETH_RX_DESCRIPTORS, 0xba00 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT LOCAL_SWITCHING_MULTICAST_INGRESS_QUEUE =
{
	1,
	{
		{ dump_RDD_INGRESS_QUEUE_ENTRY, 0xbb00 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_VLAN_PRIMITIVE_TABLE =
{
	2,
	{
		{ dump_RDD_VLAN_PRIMITIVE_ENTRY, 0xbb40 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT VLAN_ACTION_BRIDGE_INGRESS_QUEUE =
{
	1,
	{
		{ dump_RDD_INGRESS_QUEUE_ENTRY, 0xbb80 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_CPU_TX_MESSAGE_DATA_BUFFER =
{
	64,
	{
		{ dump_RDD_CPU_TX_MESSAGE_DATA_BUFFER_ENTRY, 0xbbc0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT UPSTREAM_FLOODING_INGRESS_QUEUE =
{
	1,
	{
		{ dump_RDD_INGRESS_QUEUE_ENTRY, 0xbc00 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_OVERALL_RATE_LIMITER_WAN_CHANNEL_PTR_TABLE =
{
	2,
	{
		{ dump_RDD_US_OVERALL_RATE_LIMITER_WAN_CHANNEL_PTR_ENTRY, 0xbc40 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_INGRESS_FILTERS_CONFIGURATION_TABLE =
{
	4,
	{
		{ dump_RDD_INGRESS_FILTERS_CONFIGURATION_ENTRY, 0xbca0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT BBH_TX_WAN_CHANNEL_INDEX =
{
	8,
	{
		{ dump_RDD_BBH_TX_WAN_CHANNEL_INDEX, 0xbcb8 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_VLAN_ACTION_BUFFER =
{
	64,
	{
		{ dump_RDD_VLAN_ACTION_BUFFER, 0xbcc0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT WAN_INTERWORKING_INGRESS_QUEUE =
{
	1,
	{
		{ dump_RDD_INGRESS_QUEUE_ENTRY, 0xbd00 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_PICO_RUNNER_GLOBAL_REGISTERS_INIT =
{
	4,
	{
		{ dump_RDD_PICO_RUNNER_GLOBAL_REGISTERS_INIT_ENTRY, 0xbd40 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT INGRESS_FILTERS_PROFILE_TABLE =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0xbd60 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_DHD_TX_POST_INGRESS_QUEUE =
{
	1,
	{
		{ dump_RDD_INGRESS_QUEUE_ENTRY, 0xbd80 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_RUNNER_FLOW_HEADER_DESCRIPTOR =
{
	8,
	{
		{ dump_RDD_RUNNER_FLOW_HEADER_DESCRIPTOR, 0xbdc0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_NULL_BUFFER =
{
	8,
	{
		{ dump_RDD_EIGHT_BYTES, 0xbdd8 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT LAN_INGRESS_FIFO_DESCRIPTOR_TABLE =
{
	4,
	{
		{ dump_RDD_LAN_INGRESS_FIFO_DESCRIPTOR_ENTRY, 0xbde0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_BPM_DDR_BUFFERS_BASE =
{
	4,
	{
		{ dump_RDD_BPM_DDR_BUFFERS_BASE, 0xbdf4 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_MULTICAST_VECTOR_TO_PORT_TABLE =
{
	1,
	{
		{ dump_RDD_MULTICAST_VECTOR_TO_PORT_ENTRY, 0xbdf8 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT ETH4_RX_DESCRIPTORS =
{
	8,
	{
		{ dump_RDD_ETH_RX_DESCRIPTORS, 0xbe00 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT CPU_REASON_AND_SRC_BRIDGE_PORT_TO_METER_TABLE =
{
	1,
	{
		{ dump_RDD_CPU_REASON_TO_METER_ENTRY, 0xbf00 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_PICO_TIMER_CONTROL_DESCRIPTOR =
{
	2,
	{
		{ dump_RDD_TIMER_CONTROL_DESCRIPTOR, 0xbf5a },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_CONNECTION_TABLE_CONFIG =
{
	4,
	{
		{ dump_RDD_CONNECTION_TABLE_CONFIG, 0xbf5c },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_TIMER_SCHEDULER_PRIMITIVE_TABLE =
{
	2,
	{
		{ dump_RDD_TIMER_SCHEDULER_PRIMITIVE_ENTRY, 0xbf60 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_LAYER4_FILTERS_CONTEXT_TABLE =
{
	1,
	{
		{ dump_RDD_LAYER4_FILTERS_CONTEXT_ENTRY, 0xbf70 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_DHD_TX_POST_FLOW_RING_BUFFER =
{
	48,
	{
		{ dump_RDD_DHD_TX_POST_DESCRIPTOR, 0xbf80 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_INGRESS_CLASSIFICATION_KEY_BUFFER =
{
	16,
	{
		{ dump_RDD_SIXTEEN_BYTES, 0xbfb0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT CPU_TX_DS_EGRESS_DHD_TX_POST_FLOW_RING_BUFFER =
{
	48,
	{
		{ dump_RDD_DHD_TX_POST_DESCRIPTOR, 0xbfc0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT CPU_TX_DS_EGRESS_DHD_TX_POST_CONTEXT =
{
	16,
	{
		{ dump_RDD_DHD_TX_POST_CONTEXT_ENTRY, 0xbff0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT G9991_DFC_VECTOR =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT COMMON_A_DUMMY_STORE =
{
	1,
	{
		{ dump_RDD_DUMMY_STORE_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT SCT_FILTER =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0x3f3c },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT RING_DESCRIPTORS_TABLE =
{
	16,
	{
		{ dump_RDD_RING_DESCRIPTOR, 0x7500 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT INTERRUPT_COALESCING_CONFIG_TABLE =
{
	4,
	{
		{ dump_RDD_INTERRUPT_COALESCING_CONFIG, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT INTERRUPT_COALESCING_TIMER_CONFIG_TABLE =
{
	4,
	{
		{ dump_RDD_INTERRUPT_COALESCING_TIMER_CONFIG, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_CPU_REASON_TO_CPU_RX_QUEUE_TABLE =
{
	1,
	{
		{ dump_RDD_CPU_REASON_TO_CPU_RX_QUEUE_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_PBITS_TO_QOS_TABLE =
{
	1,
	{
		{ dump_RDD_PBITS_TO_QOS_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT GLOBAL_DSCP_TO_PBITS_TABLE =
{
	1,
	{
		{ dump_RDD_DSCP_TO_PBITS_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT GLOBAL_DSCP_TO_PBITS_DEI_TABLE =
{
	1,
	{
		{ dump_RDD_DSCP_TO_PBITS_DEI_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT MAC_TABLE =
{
	8,
	{
		{ dump_RDD_MAC_ENTRY, 0x0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT MAC_TABLE_CAM =
{
	8,
	{
		{ dump_RDD_MAC_ENTRY, 0x3500 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT MAC_EXTENSION_TABLE =
{
	1,
	{
		{ dump_RDD_MAC_EXTENSION_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT MAC_EXTENSION_TABLE_CAM =
{
	1,
	{
		{ dump_RDD_MAC_EXTENSION_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT MAC_CONTEXT_TABLE =
{
	2,
	{
		{ dump_RDD_MAC_CONTEXT_ENTRY, 0x2800 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT MAC_CONTEXT_TABLE_CAM =
{
	2,
	{
		{ dump_RDD_MAC_CONTEXT_ENTRY, 0x3ac0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT MAC_UNKNOWN_DA_FORWARDING_TABLE =
{
	8,
	{
		{ dump_RDD_MAC_UNKNOWN_DA_FORWARDING_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT IPTV_LOOKUP_TABLE =
{
	8,
	{
		{ dump_RDD_IPTV_L2_LOOKUP_ENTRY, 0x2000 },
		{ dump_RDD_IPTV_L3_LOOKUP_ENTRY, 0x2000 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT IPTV_L3_SRC_IP_LOOKUP_TABLE =
{
	8,
	{
		{ dump_RDD_IPTV_L3_SRC_IP_LOOKUP_ENTRY, 0x3700 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT IPTV_LOOKUP_TABLE_CAM =
{
	8,
	{
		{ dump_RDD_MAC_ENTRY, 0x3400 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT IPTV_CONTEXT_TABLE =
{
#if defined G9991
	4,
	{
		{ dump_RDD_IPTV_CONTEXT_ENTRY, 0x3000 },
#endif
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT IPTV_CONTEXT_TABLE_CAM =
{
#if defined G9991
	4,
	{
		{ dump_RDD_IPTV_CONTEXT_ENTRY, 0x3600 },
#endif
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_INGRESS_CLASSIFICATION_LOOKUP_LONG_TABLE =
{
	16,
	{
		{ dump_RDD_INGRESS_CLASSIFICATION_LONG_LOOKUP_ENTRY, 0x5000 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT LAN_VID_CONTEXT_TABLE =
{
	2,
	{
		{ dump_RDD_LAN_VID_CONTEXT_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT WAN_VID_TABLE =
{
	2,
	{
		{ dump_RDD_VID_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT G9991_VIRTUAL_PORT_STATUS_PER_EMAC =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT G9991_DOWNSTREAM_MULTICAST_FRAGMENTATION_INGRESS_QUEUE =
{
	8,
	{
		{ dump_RDD_EIGHT_BYTES, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT PACKET_SRAM_TO_DDR_COPY_BUFFER_1 =
{
	128,
	{
		{ dump_RDD_PACKET_SRAM_TO_DDR_COPY_BUFFER, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT PACKET_SRAM_TO_DDR_COPY_BUFFER_2 =
{
	128,
	{
		{ dump_RDD_PACKET_SRAM_TO_DDR_COPY_BUFFER, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT G9991_DDR_QUEUE_DESCRIPTORS_TABLE =
{
	16,
	{
		{ dump_RDD_DDR_QUEUE_DESCRIPTOR, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT G9991_DDR_QUEUE_ADDRESS_TABLE =
{
	8,
	{
		{ dump_RDD_DDR_QUEUE_ADDRESS_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT PM_COUNTERS_BUFFER =
{
	32,
	{
		{ dump_RDD_PM_COUNTERS_BUFFER, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT FREE_SKB_INDEXES_DDR_FIFO_TAIL =
{
	4,
	{
		{ dump_RDD_FREE_SKB_INDEXES_FIFO_TAIL, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DDR_ADDRESS_FOR_FREE_SKB_INDEXES_FIFO_TABLE =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0x3f30 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DDR_ADDRESS_FOR_FREE_SKB_INDEXES_FIFO_TABLE_LAST_ENTRY =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0x3f34 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DDR_ADDRESS_FOR_SKB_DATA_POINTERS_TABLE =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT BPM_REPLY_RUNNER_A =
{
	48,
	{
		{ dump_RDD_BPM_REPLY, 0x3f40 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT PM_COUNTERS =
{
	6144,
	{
		{ dump_RDD_PM_COUNTERS, 0x5800 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT RUNNER_A_SCRATCHPAD =
{
	256,
	{
		{ dump_RDD_RUNNER_SCRATCHPAD, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT CPU_RX_RUNNER_A_SCRATCHPAD =
{
	256,
	{
		{ dump_RDD_RUNNER_SCRATCHPAD, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT GPON_SKB_ENQUEUED_INDEXES_FIFO =
{
	48,
	{
		{ dump_RDD_GPON_SKB_ENQUEUED_INDEXES_FIFO_ENTRY, 0x4000 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_RING_PACKET_DESCRIPTORS_CACHE =
{
	16,
	{
		{ dump_RDD_CPU_RX_DESCRIPTOR, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT CONNECTION_BUFFER_TABLE =
{
	16,
	{
		{ dump_RDD_CONNECTION_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT BPM_CONGESTION_CONTROL_TABLE =
{
	12,
	{
		{ dump_RDD_BPM_CONGESTION_CONTROL_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_DHD_TX_POST_FLOW_RING_MGMT_DESCRIPTOR_BUFFER =
{
	16,
	{
		{ dump_RDD_DHD_TX_POST_FLOW_RING_MGMT_DESCRIPTOR, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_DHD_FLOW_RING_CACHE_LKP_TABLE =
{
	2,
	{
		{ dump_RDD_DHD_FLOW_RING_CACHE_LKP_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DHD_FLOW_RING_SHADOW_WR_PTR_TABLE =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DHD_RADIO_INSTANCE_COMMON_A_DATA =
{
	16,
	{
		{ dump_RDD_DHD_RADIO_INSTANCE_COMMON_A_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_DHD_STATION_TABLE =
{
	10,
	{
		{ dump_RDD_WLAN_MCAST_DHD_STATION_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_SSID_MAC_ADDRESS_TABLE =
{
	8,
	{
		{ dump_RDD_WLAN_MCAST_SSID_MAC_ADDRESS_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_SSID_STATS_TABLE =
{
	8,
	{
		{ dump_RDD_WLAN_MCAST_SSID_STATS_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT BRIDGE_PORT_TO_BBH_PERIPHERAL_RX_TABLE =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT COMMON_B_DUMMY_STORE =
{
	1,
	{
		{ dump_RDD_DUMMY_STORE_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT CPU_TX_DESCRIPTOR_QUEUE_TAIL_TABLE =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT EGRESS_VLAN_SWITCHING_ISOLATION_CONFIG_TABLE =
{
	1,
	{
		{ dump_RDD_VLAN_SWITCHING_ISOLATION_CONFIG_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT RATE_CONTROLLER_EXPONENT_TABLE =
{
	1,
	{
		{ dump_RDD_RATE_CONTROLLER_EXPONENT_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_RATE_CONTROLLERS_TABLE =
{
	48,
	{
		{ dump_RDD_US_RATE_CONTROLLER_DESCRIPTOR, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT WAN_TX_QUEUES_TABLE =
{
	16,
	{
		{ dump_RDD_WAN_TX_QUEUE_DESCRIPTOR, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_CPU_REASON_TO_CPU_RX_QUEUE_TABLE =
{
	1,
	{
		{ dump_RDD_CPU_REASON_TO_CPU_RX_QUEUE_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DUAL_STACK_LITE_TABLE =
{
	64,
	{
		{ dump_RDD_DUAL_STACK_LITE_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT LAYER2_GRE_TUNNEL_TABLE =
{
	32,
	{
		{ dump_RDD_LAYER2_GRE_TUNNEL_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT LOCAL_SWITCHING_PBITS_TO_QOS_TABLE =
{
	1,
	{
		{ dump_RDD_PBITS_TO_QOS_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_INGRESS_CLASSIFICATION_LOOKUP_TABLE =
{
	8,
	{
		{ dump_RDD_DS_INGRESS_CLASSIFICATION_IH_LOOKUP_ENTRY, 0xa000 },
		{ dump_RDD_US_INGRESS_CLASSIFICATION_IH_LOOKUP_ENTRY, 0xa000 },
		{ dump_RDD_DS_INGRESS_CLASSIFICATION_OPTIMIZED_LOOKUP_ENTRY, 0xa000 },
		{ dump_RDD_US_INGRESS_CLASSIFICATION_OPTIMIZED_LOOKUP_ENTRY, 0xa000 },
		{ dump_RDD_INGRESS_CLASSIFICATION_SHORT_LOOKUP_ENTRY, 0xa000 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_INGRESS_CLASSIFICATION_LOOKUP_TABLE =
{
	8,
	{
		{ dump_RDD_DS_INGRESS_CLASSIFICATION_IH_LOOKUP_ENTRY, 0xa800 },
		{ dump_RDD_US_INGRESS_CLASSIFICATION_IH_LOOKUP_ENTRY, 0xa800 },
		{ dump_RDD_DS_INGRESS_CLASSIFICATION_OPTIMIZED_LOOKUP_ENTRY, 0xa800 },
		{ dump_RDD_US_INGRESS_CLASSIFICATION_OPTIMIZED_LOOKUP_ENTRY, 0xa800 },
		{ dump_RDD_INGRESS_CLASSIFICATION_SHORT_LOOKUP_ENTRY, 0xa800 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_INGRESS_CLASSIFICATION_LOOKUP_LONG_TABLE =
{
	16,
	{
		{ dump_RDD_INGRESS_CLASSIFICATION_LONG_LOOKUP_ENTRY, 0x8000 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT WIFI_SSID_FORWARDING_MATRIX_TABLE =
{
	2,
	{
		{ dump_RDD_WIFI_SSID_FORWARDING_MATRIX_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT MULTICAST_ACTIVE_PORTS_TABLE =
{
	1,
	{
		{ dump_RDD_MULTICAST_ACTIVE_PORTS_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT SRC_MAC_ANTI_SPOOFING_TABLE =
{
	4,
	{
		{ dump_RDD_SRC_MAC_ANTI_SPOOFING_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT WAN_PHYSICAL_PORT =
{
	2,
	{
		{ dump_RDD_WAN_PHYSICAL_PORT, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT BRIDGE_PORT_TO_BROADCOM_SWITCH_PORT_MAPPING_TABLE =
{
	1,
	{
		{ dump_RDD_BROADCOM_SWITCH_PORT_MAPPING, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT BRIDGE_PORT_TO_BROADCOM_SWITCH_PORT_MAPPING_TABLE_PTR =
{
	2,
	{
		{ dump_RDD_BRIDGE_PORT_TO_BROADCOM_SWITCH_PORT_MAPPING_TABLE_PTR, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT EPON_DDR_CACHE_FIFO =
{
	8,
	{
		{ dump_RDD_BBH_TX_DESCRIPTOR, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT EPON_DDR_QUEUE_DESCRIPTORS_TABLE =
{
	16,
	{
		{ dump_RDD_DDR_QUEUE_DESCRIPTOR, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT EPON_DDR_QUEUE_ADDRESS_TABLE =
{
	8,
	{
		{ dump_RDD_DDR_QUEUE_ADDRESS_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT GPON_SKB_ENQUEUED_INDEXES_FREE_PTR =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT GPON_SKB_ENQUEUED_INDEXES_PUT_PTR =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT LOCAL_SWITCHING_LAN_ENQUEUE_INGRESS_QUEUE =
{
	8,
	{
		{ dump_RDD_BBH_RX_DESCRIPTOR, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT IP_SYNC_1588_DESCRIPTOR_QUEUE =
{
	16,
	{
		{ dump_RDD_IP_SYNC_1588_DESCRIPTOR_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT BPM_REPLY_RUNNER_B =
{
	48,
	{
		{ dump_RDD_BPM_REPLY, 0xc7d0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_INGRESS_CLASSIFICATION_LOOKUP_CAM_TABLE =
{
	8,
	{
		{ dump_RDD_DS_INGRESS_CLASSIFICATION_IH_LOOKUP_ENTRY, 0x9300 },
		{ dump_RDD_US_INGRESS_CLASSIFICATION_IH_LOOKUP_ENTRY, 0x9300 },
		{ dump_RDD_DS_INGRESS_CLASSIFICATION_OPTIMIZED_LOOKUP_ENTRY, 0x9300 },
		{ dump_RDD_US_INGRESS_CLASSIFICATION_OPTIMIZED_LOOKUP_ENTRY, 0x9300 },
		{ dump_RDD_INGRESS_CLASSIFICATION_SHORT_LOOKUP_ENTRY, 0x9300 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_INGRESS_CLASSIFICATION_LOOKUP_CAM_TABLE =
{
	8,
	{
		{ dump_RDD_DS_INGRESS_CLASSIFICATION_IH_LOOKUP_ENTRY, 0x9c00 },
		{ dump_RDD_US_INGRESS_CLASSIFICATION_IH_LOOKUP_ENTRY, 0x9c00 },
		{ dump_RDD_DS_INGRESS_CLASSIFICATION_OPTIMIZED_LOOKUP_ENTRY, 0x9c00 },
		{ dump_RDD_US_INGRESS_CLASSIFICATION_OPTIMIZED_LOOKUP_ENTRY, 0x9c00 },
		{ dump_RDD_INGRESS_CLASSIFICATION_SHORT_LOOKUP_ENTRY, 0x9c00 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT RUNNER_B_SCRATCHPAD =
{
	256,
	{
		{ dump_RDD_RUNNER_SCRATCHPAD, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT CPU_RX_RUNNER_B_SCRATCHPAD =
{
	256,
	{
		{ dump_RDD_RUNNER_SCRATCHPAD, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DUMMY_WAN_TX_QUEUE_DESCRIPTOR =
{
	16,
	{
		{ dump_RDD_DUMMY_WAN_TX_QUEUE_DESCRIPTOR, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT CPU_TX_EMAC_LOOPBACK_QUEUE =
{
	8,
	{
		{ dump_RDD_BBH_RX_DESCRIPTOR, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT CPU_TX_US_FLOODING_QUEUE =
{
	8,
	{
		{ dump_RDD_BBH_RX_DESCRIPTOR, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT CPU_TX_PICO_US_FLOODING_QUEUE_PTR =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DUMMY_RATE_CONTROLLER_DESCRIPTOR =
{
	64,
	{
		{ dump_RDD_DUMMY_RATE_CONTROLLER_DESCRIPTOR, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_RING_PACKET_DESCRIPTORS_CACHE =
{
	16,
	{
		{ dump_RDD_CPU_RX_DESCRIPTOR, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT LAN0_INGRESS_FIFO =
{
	64,
	{
		{ dump_RDD_LAN_INGRESS_FIFO_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT LAN1_INGRESS_FIFO =
{
	64,
	{
		{ dump_RDD_LAN_INGRESS_FIFO_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT LAN2_INGRESS_FIFO =
{
	64,
	{
		{ dump_RDD_LAN_INGRESS_FIFO_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT LAN3_INGRESS_FIFO =
{
	64,
	{
		{ dump_RDD_LAN_INGRESS_FIFO_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT LAN4_INGRESS_FIFO =
{
	64,
	{
		{ dump_RDD_LAN_INGRESS_FIFO_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT WAN_ENQUEUE_INGRESS_QUEUE =
{
	1,
	{
		{ dump_RDD_INGRESS_QUEUE_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT SPEED_SERVICE_STREAM_PREFIX =
{
	16,
	{
		{ dump_RDD_SIXTEEN_BYTES, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT SPEED_SERVICE_RX_TIMESTAMPS_TABLE =
{
	8,
	{
		{ dump_RDD_SPEED_SERVICE_RX_TIMESTAMPS, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_DHD_TX_POST_FLOW_RING_MGMT_DESCRIPTOR_BUFFER =
{
	16,
	{
		{ dump_RDD_DHD_TX_POST_FLOW_RING_MGMT_DESCRIPTOR, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_DHD_FLOW_RING_CACHE_LKP_TABLE =
{
	2,
	{
		{ dump_RDD_DHD_FLOW_RING_CACHE_LKP_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DHD_FLOW_RING_CACHE_CTX_TABLE =
{
	16,
	{
		{ dump_RDD_DHD_FLOW_RING_CACHE_CTX_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DHD_FLOW_RING_CACHE_CTX_NEXT_INDEX =
{
	1,
	{
		{ dump_RDD_ONE_BYTE, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DHD_RADIO_INSTANCE_COMMON_B_DATA =
{
	64,
	{
		{ dump_RDD_DHD_RADIO_INSTANCE_COMMON_B_ENTRY, 0x-1 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT IPTV_DDR_LOOKUP_TABLE =
{
	16,
	{
		{ dump_RDD_IPTV_L2_DDR_LOOKUP_ENTRY, 0x340000 },
		{ dump_RDD_IPTV_L3_DDR_LOOKUP_ENTRY, 0x340000 },
		{ dump_RDD_IPTV_L3_SSM_DDR_LOOKUP_ENTRY, 0x340000 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT IPTV_DDR_CONTEXT_TABLE =
{
#if defined G9991
	16,
	{
		{ dump_RDD_IPTV_DDR_CONTEXT_ENTRY, 0x300000 },
#endif
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT IPTV_SSM_DDR_CONTEXT_TABLE =
{
#if defined G9991
	16,
	{
		{ dump_RDD_IPTV_DDR_CONTEXT_ENTRY, 0x380000 },
#endif
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT FIREWALL_RULES_MAP_TABLE =
{
	1,
	{
		{ dump_RDD_FIREWALL_RULES_MAP_ENTRY, 0x400000 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT FIREWALL_RULES_TABLE =
{
	16,
	{
		{ dump_RDD_FIREWALL_RULE_ENTRY, 0x202000 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_CONNECTION_TABLE =
{
	16,
	{
		{ dump_RDD_CONNECTION_ENTRY, 0x0 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_CONNECTION_TABLE =
{
	16,
	{
		{ dump_RDD_CONNECTION_ENTRY, 0x80000 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT CONTEXT_TABLE =
{
	64,
	{
		{ dump_RDD_FLOW_CACHE_CONTEXT_ENTRY, 0x100000 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DDR_ADDRESS_FOR_SYNC_DMA =
{
	4,
	{
		{ dump_RDD_FOUR_BYTES, 0x203000 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT EPON_TX_POST_SCHEDULING_DDR_QUEUES =
{
	8,
	{
		{ dump_RDD_BBH_TX_DESCRIPTOR, 0x240000 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DHD_TX_POST_FLOW_RING_MGMT_DESCRIPTOR_TABLE =
{
	16,
	{
		{ dump_RDD_DHD_TX_POST_FLOW_RING_MGMT_DESCRIPTOR, 0x5d1500 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DHD_RX_POST_DDR_BUFFER =
{
	32,
	{
		{ dump_RDD_DHD_RX_POST_DESCRIPTOR, 0x5c1100 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DHD_RX_COMPLETE_DDR_BUFFER =
{
	32,
	{
		{ dump_RDD_DHD_RX_COMPLETE_DESCRIPTOR, 0x5c9100 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DHD_TX_POST_DDR_BUFFER =
{
	48,
	{
		{ dump_RDD_DHD_TX_POST_DESCRIPTOR, 0x5d1580 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DHD_TX_COMPLETE_DDR_BUFFER =
{
	16,
	{
		{ dump_RDD_DHD_TX_COMPLETE_DESCRIPTOR, 0x5d2d80 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT R2D_WR_ARR_DDR_BUFFER =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0x5d1100 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT D2R_RD_ARR_DDR_BUFFER =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0x5d1200 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT R2D_RD_ARR_DDR_BUFFER =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0x5d1300 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT D2R_WR_ARR_DDR_BUFFER =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0x5d1400 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_DHD_LIST_TABLE =
{
	64,
	{
		{ dump_RDD_WLAN_MCAST_DHD_LIST_ENTRY_ARRAY, 0x5c0000 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_DHD_LIST_FORMAT_TABLE =
{
	1,
	{
		{ dump_RDD_WLAN_MCAST_DHD_LIST_ENTRY, 0x5c1000 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT DS_INGRESS_CLASSIFICATION_COUNTERS_TABLE =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0x1fc00 },
		{ 0, 0 },
	}
};
#endif
#if defined G9991
static DUMP_RUNNERREG_STRUCT US_INGRESS_CLASSIFICATION_COUNTERS_TABLE =
{
	2,
	{
		{ dump_RDD_TWO_BYTES, 0x1fe00 },
		{ 0, 0 },
	}
};
#endif

TABLE_STRUCT RUNNER_TABLES[NUMBER_OF_TABLES] =
{
#if defined OREN
	{ "INGRESS_HANDLER_BUFFER", 1, PRIVATE_A_INDEX, &INGRESS_HANDLER_BUFFER, 32, 1, 1 },
#endif
#if defined OREN
	{ "DS_FREE_PACKET_DESCRIPTORS_POOL", 1, PRIVATE_A_INDEX, &DS_FREE_PACKET_DESCRIPTORS_POOL, 1536, 1, 1 },
#endif
#if defined OREN
	{ "SERVICE_QUEUES_DDR_CACHE_FIFO", 1, PRIVATE_A_INDEX, &SERVICE_QUEUES_DDR_CACHE_FIFO, 384, 1, 1 },
#endif
#if defined OREN
	{ "DS_INGRESS_CLASSIFICATION_CONTEXT_TABLE", 1, PRIVATE_A_INDEX, &DS_INGRESS_CLASSIFICATION_CONTEXT_TABLE, 256, 1, 1 },
#endif
#if defined OREN
	{ "DS_INGRESS_FILTERS_LOOKUP_TABLE", 1, PRIVATE_A_INDEX, &DS_INGRESS_FILTERS_LOOKUP_TABLE, 10, 32, 1 },
#endif
#if defined OREN
	{ "ETH_TX_RS_QUEUE_DESCRIPTOR_TABLE", 1, PRIVATE_A_INDEX, &ETH_TX_RS_QUEUE_DESCRIPTOR_TABLE, 128, 1, 1 },
#endif
#if defined OREN
	{ "DS_PBITS_TO_PBITS_TABLE", 1, PRIVATE_A_INDEX, &DS_PBITS_TO_PBITS_TABLE, 32, 8, 1 },
#endif
#if defined OREN
	{ "DS_WAN_FLOW_TABLE", 1, PRIVATE_A_INDEX, &DS_WAN_FLOW_TABLE, 256, 1, 1 },
#endif
#if defined OREN
	{ "IPTV_COUNTERS_TABLE", 1, PRIVATE_A_INDEX, &IPTV_COUNTERS_TABLE, 288, 1, 1 },
#endif
#if defined OREN
	{ "ETH_TX_LOCAL_REGISTERS", 1, PRIVATE_A_INDEX, &ETH_TX_LOCAL_REGISTERS, 8, 1, 1 },
#endif
#if defined OREN
	{ "DS_QUEUE_PROFILE_TABLE", 1, PRIVATE_A_INDEX, &DS_QUEUE_PROFILE_TABLE, 8, 1, 1 },
#endif
#if defined OREN
	{ "DS_PBITS_PARAMETER_TABLE", 1, PRIVATE_A_INDEX, &DS_PBITS_PARAMETER_TABLE, 128, 1, 1 },
#endif
#if defined OREN
	{ "IPTV_SSID_EXTENSION_TABLE", 1, PRIVATE_A_INDEX, &IPTV_SSID_EXTENSION_TABLE, 256, 1, 1 },
#endif
#if defined OREN
	{ "DS_VLAN_PARAMETER_TABLE", 1, PRIVATE_A_INDEX, &DS_VLAN_PARAMETER_TABLE, 128, 1, 1 },
#endif
#if defined OREN
	{ "DS_CPU_REASON_TO_METER_TABLE", 1, PRIVATE_A_INDEX, &DS_CPU_REASON_TO_METER_TABLE, 64, 1, 1 },
#endif
#if defined OREN
	{ "IPTV_INGRESS_CLASSIFICATION_CONTEXT_TABLE", 1, PRIVATE_A_INDEX, &IPTV_INGRESS_CLASSIFICATION_CONTEXT_TABLE, 288, 1, 1 },
#endif
#if defined OREN
	{ "DS_MAIN_TIMER_TASK_DESCRIPTOR_TABLE", 1, PRIVATE_A_INDEX, &DS_MAIN_TIMER_TASK_DESCRIPTOR_TABLE, 4, 1, 1 },
#endif
#if defined OREN
	{ "SBPM_REPLY", 1, PRIVATE_A_INDEX, &SBPM_REPLY, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_CONNECTION_CONTEXT_BUFFER", 1, PRIVATE_A_INDEX, &DS_CONNECTION_CONTEXT_BUFFER, 8, 1, 1 },
#endif
#if defined OREN
	{ "WLAN_MCAST_FWD_TABLE", 1, PRIVATE_A_INDEX, &WLAN_MCAST_FWD_TABLE, 64, 1, 1 },
#endif
#if defined OREN
	{ "ETH_TX_QUEUES_TABLE", 1, PRIVATE_A_INDEX, &ETH_TX_QUEUES_TABLE, 48, 1, 1 },
#endif
#if defined OREN
	{ "DS_LAN_VID_TABLE", 1, PRIVATE_A_INDEX, &DS_LAN_VID_TABLE, 128, 1, 1 },
#endif
#if defined OREN
	{ "DS_PBITS_PRIMITIVE_TABLE", 1, PRIVATE_A_INDEX, &DS_PBITS_PRIMITIVE_TABLE, 32, 1, 1 },
#endif
#if defined OREN
	{ "DS_FAST_CPU_TX_DESCRIPTOR_ABS_DATA_PTR_QUEUE", 1, PRIVATE_A_INDEX, &DS_FAST_CPU_TX_DESCRIPTOR_ABS_DATA_PTR_QUEUE, 16, 1, 1 },
#endif
#if defined OREN
	{ "DS_PICO_CPU_TX_DESCRIPTOR_ABS_DATA_PTR_QUEUE", 1, PRIVATE_A_INDEX, &DS_PICO_CPU_TX_DESCRIPTOR_ABS_DATA_PTR_QUEUE, 16, 1, 1 },
#endif
#if defined OREN
	{ "RATE_LIMITER_REMAINDER_TABLE", 1, PRIVATE_A_INDEX, &RATE_LIMITER_REMAINDER_TABLE, 32, 1, 1 },
#endif
#if defined OREN
	{ "DS_RATE_LIMITER_TABLE", 1, PRIVATE_A_INDEX, &DS_RATE_LIMITER_TABLE, 32, 1, 1 },
#endif
#if defined OREN
	{ "SERVICE_QUEUES_RATE_LIMITER_TABLE", 1, PRIVATE_A_INDEX, &SERVICE_QUEUES_RATE_LIMITER_TABLE, 32, 1, 1 },
#endif
#if defined OREN
	{ "DS_VLAN_COMMANDS_TABLE", 1, PRIVATE_A_INDEX, &DS_VLAN_COMMANDS_TABLE, 64, 1, 1 },
#endif
#if defined OREN
	{ "DS_POLICER_TABLE", 1, PRIVATE_A_INDEX, &DS_POLICER_TABLE, 16, 1, 1 },
#endif
#if defined OREN
	{ "DS_CPU_TX_BBH_DESCRIPTORS", 1, PRIVATE_A_INDEX, &DS_CPU_TX_BBH_DESCRIPTORS, 32, 1, 1 },
#endif
#if defined OREN
	{ "ETH_TX_QUEUES_POINTERS_TABLE", 1, PRIVATE_A_INDEX, &ETH_TX_QUEUES_POINTERS_TABLE, 48, 1, 1 },
#endif
#if defined OREN
	{ "DS_INGRESS_CLASSIFICATION_KEY_PRIMITIVE_TABLE", 1, PRIVATE_A_INDEX, &DS_INGRESS_CLASSIFICATION_KEY_PRIMITIVE_TABLE, 32, 1, 1 },
#endif
#if defined OREN
	{ "CPU_RX_PD_INGRESS_QUEUE", 1, PRIVATE_A_INDEX, &CPU_RX_PD_INGRESS_QUEUE, 32, 1, 1 },
#endif
#if defined OREN
	{ "DS_INGRESS_FILTERS_PARAMETER_TABLE", 1, PRIVATE_A_INDEX, &DS_INGRESS_FILTERS_PARAMETER_TABLE, 10, 16, 1 },
#endif
#if defined OREN
	{ "DS_PICO_TIMER_TASK_DESCRIPTOR_TABLE", 1, PRIVATE_A_INDEX, &DS_PICO_TIMER_TASK_DESCRIPTOR_TABLE, 4, 1, 1 },
#endif
#if defined OREN
	{ "IPTV_SSID_EXTENSION_TABLE_CAM", 1, PRIVATE_A_INDEX, &IPTV_SSID_EXTENSION_TABLE_CAM, 1, 1, 1 },
#endif
#if defined OREN
	{ "CPU_RX_FAST_PD_INGRESS_QUEUE", 1, PRIVATE_A_INDEX, &CPU_RX_FAST_PD_INGRESS_QUEUE, 32, 1, 1 },
#endif
#if defined OREN
	{ "WLAN_MCAST_CONTROL_TABLE", 1, PRIVATE_A_INDEX, &WLAN_MCAST_CONTROL_TABLE, 1, 1, 1 },
#endif
#if defined OREN
	{ "BPM_DDR_OPTIMIZED_BUFFERS_WITHOUT_HEADROOM_BASE", 1, PRIVATE_A_INDEX, &BPM_DDR_OPTIMIZED_BUFFERS_WITHOUT_HEADROOM_BASE, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_CPU_PARAMETERS_BLOCK", 1, PRIVATE_A_INDEX, &DS_CPU_PARAMETERS_BLOCK, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_INGRESS_CLASSIFICATION_GENERIC_RULE_CFG_TABLE", 1, PRIVATE_A_INDEX, &DS_INGRESS_CLASSIFICATION_GENERIC_RULE_CFG_TABLE, 4, 1, 1 },
#endif
#if defined OREN
	{ "DS_FLOW_BASED_ACTION_PTR_TABLE", 1, PRIVATE_A_INDEX, &DS_FLOW_BASED_ACTION_PTR_TABLE, 32, 1, 1 },
#endif
#if defined OREN
	{ "DHD_TX_POST_PD_INGRESS_QUEUE", 1, PRIVATE_A_INDEX, &DHD_TX_POST_PD_INGRESS_QUEUE, 32, 1, 1 },
#endif
#if defined OREN
	{ "DS_FORWARDING_MATRIX_TABLE", 1, PRIVATE_A_INDEX, &DS_FORWARDING_MATRIX_TABLE, 9, 16, 1 },
#endif
#if defined OREN
	{ "DS_TPID_OVERWRITE_TABLE", 1, PRIVATE_A_INDEX, &DS_TPID_OVERWRITE_TABLE, 8, 1, 1 },
#endif
#if defined OREN
	{ "DS_FAST_RUNNER_GLOBAL_REGISTERS_INIT", 1, PRIVATE_A_INDEX, &DS_FAST_RUNNER_GLOBAL_REGISTERS_INIT, 8, 1, 1 },
#endif
#if defined OREN
	{ "DS_TRAFFIC_CLASS_TO_QUEUE_TABLE", 1, PRIVATE_A_INDEX, &DS_TRAFFIC_CLASS_TO_QUEUE_TABLE, 6, 8, 1 },
#endif
#if defined OREN
	{ "DS_TIMER_SCHEDULER_PRIMITIVE_TABLE", 1, PRIVATE_A_INDEX, &DS_TIMER_SCHEDULER_PRIMITIVE_TABLE, 8, 1, 1 },
#endif
#if defined OREN
	{ "WLAN_MCAST_PD_INGRESS_QUEUE", 1, PRIVATE_A_INDEX, &WLAN_MCAST_PD_INGRESS_QUEUE, 32, 1, 1 },
#endif
#if defined OREN
	{ "DS_LAYER4_FILTERS_LOOKUP_TABLE", 1, PRIVATE_A_INDEX, &DS_LAYER4_FILTERS_LOOKUP_TABLE, 16, 1, 1 },
#endif
#if defined OREN
	{ "DS_CPU_RX_METER_TABLE", 1, PRIVATE_A_INDEX, &DS_CPU_RX_METER_TABLE, 16, 1, 1 },
#endif
#if defined OREN
	{ "DS_BRIDGE_CONFIGURATION_REGISTER", 1, PRIVATE_A_INDEX, &DS_BRIDGE_CONFIGURATION_REGISTER, 1, 1, 1 },
#endif
#if defined OREN
	{ "INGRESS_HANDLER_SKB_DATA_POINTER", 1, PRIVATE_A_INDEX, &INGRESS_HANDLER_SKB_DATA_POINTER, 32, 1, 1 },
#endif
#if defined OREN
	{ "DS_INGRESS_CLASSIFICATION_RULE_CFG_TABLE", 1, PRIVATE_A_INDEX, &DS_INGRESS_CLASSIFICATION_RULE_CFG_TABLE, 16, 1, 1 },
#endif
#if defined OREN
	{ "DS_PROFILING_BUFFER_PICO_RUNNER", 1, PRIVATE_A_INDEX, &DS_PROFILING_BUFFER_PICO_RUNNER, 1, 1, 1 },
#endif
#if defined OREN
	{ "DOWNSTREAM_MULTICAST_LAN_ENQUEUE_INGRESS_QUEUE", 1, PRIVATE_A_INDEX, &DOWNSTREAM_MULTICAST_LAN_ENQUEUE_INGRESS_QUEUE, 16, 1, 1 },
#endif
#if defined OREN
	{ "DS_VLAN_OPTIMIZATION_TABLE", 1, PRIVATE_A_INDEX, &DS_VLAN_OPTIMIZATION_TABLE, 128, 1, 1 },
#endif
#if defined OREN
	{ "DOWNSTREAM_MULTICAST_LAN_ENQUEUE_SERVICE_QUEUE", 1, PRIVATE_A_INDEX, &DOWNSTREAM_MULTICAST_LAN_ENQUEUE_SERVICE_QUEUE, 16, 1, 1 },
#endif
#if defined OREN
	{ "PCI_MULTICAST_SCRATCHPAD", 1, PRIVATE_A_INDEX, &PCI_MULTICAST_SCRATCHPAD, 1, 1, 1 },
#endif
#if defined OREN
	{ "CPU_RX_MIRRORING_PD_INGRESS_QUEUE", 1, PRIVATE_A_INDEX, &CPU_RX_MIRRORING_PD_INGRESS_QUEUE, 16, 1, 1 },
#endif
#if defined OREN
	{ "EMAC_SKB_ENQUEUED_INDEXES_FIFO", 1, PRIVATE_A_INDEX, &EMAC_SKB_ENQUEUED_INDEXES_FIFO, 5, 1, 1 },
#endif
#if defined OREN
	{ "DS_PICO_RUNNER_GLOBAL_REGISTERS_INIT", 1, PRIVATE_A_INDEX, &DS_PICO_RUNNER_GLOBAL_REGISTERS_INIT, 8, 1, 1 },
#endif
#if defined OREN
	{ "DHD_TX_COMPLETE_FLOW_RING_BUFFER", 1, PRIVATE_A_INDEX, &DHD_TX_COMPLETE_FLOW_RING_BUFFER, 3, 1, 1 },
#endif
#if defined OREN
	{ "EMAC_SKB_ENQUEUED_INDEXES_PUT_PTR", 1, PRIVATE_A_INDEX, &EMAC_SKB_ENQUEUED_INDEXES_PUT_PTR, 8, 1, 1 },
#endif
#if defined OREN
	{ "DS_RUNNER_FLOW_HEADER_BUFFER", 1, PRIVATE_A_INDEX, &DS_RUNNER_FLOW_HEADER_BUFFER, 1, 1, 1 },
#endif
#if defined OREN
	{ "DOWNSTREAM_LAN_ENQUEUE_INGRESS_QUEUE", 1, PRIVATE_A_INDEX, &DOWNSTREAM_LAN_ENQUEUE_INGRESS_QUEUE, 64, 1, 1 },
#endif
#if defined OREN
	{ "DHD_COMPLETE_RING_DESCRIPTOR_BUFFER", 1, PRIVATE_A_INDEX, &DHD_COMPLETE_RING_DESCRIPTOR_BUFFER, 3, 1, 1 },
#endif
#if defined OREN
	{ "EMAC_SKB_ENQUEUED_INDEXES_FREE_PTR", 1, PRIVATE_A_INDEX, &EMAC_SKB_ENQUEUED_INDEXES_FREE_PTR, 8, 1, 1 },
#endif
#if defined OREN
	{ "DOWNSTREAM_LAN_ENQUEUE_SERVICE_QUEUE", 1, PRIVATE_A_INDEX, &DOWNSTREAM_LAN_ENQUEUE_SERVICE_QUEUE, 64, 1, 1 },
#endif
#if defined OREN
	{ "DS_DSCP_TO_PBITS_TABLE", 1, PRIVATE_A_INDEX, &DS_DSCP_TO_PBITS_TABLE, 1, 64, 1 },
#endif
#if defined OREN
	{ "CPU_TX_FAST_QUEUE", 1, PRIVATE_A_INDEX, &CPU_TX_FAST_QUEUE, 16, 1, 1 },
#endif
#if defined OREN
	{ "PACKET_SRAM_TO_DDR_COPY_BUFFER", 1, PRIVATE_A_INDEX, &PACKET_SRAM_TO_DDR_COPY_BUFFER, 1, 1, 1 },
#endif
#if defined OREN
	{ "CPU_TX_PICO_QUEUE", 1, PRIVATE_A_INDEX, &CPU_TX_PICO_QUEUE, 16, 1, 1 },
#endif
#if defined OREN
	{ "DS_CPU_RX_PICO_INGRESS_QUEUE", 1, PRIVATE_A_INDEX, &DS_CPU_RX_PICO_INGRESS_QUEUE, 64, 1, 1 },
#endif
#if defined OREN
	{ "DS_VLAN_PRIMITIVE_TABLE", 1, PRIVATE_A_INDEX, &DS_VLAN_PRIMITIVE_TABLE, 32, 1, 1 },
#endif
#if defined OREN
	{ "ETH_TX_MAC_TABLE", 1, PRIVATE_A_INDEX, &ETH_TX_MAC_TABLE, 6, 1, 1 },
#endif
#if defined OREN
	{ "DOWNSTREAM_MULTICAST_INGRESS_QUEUE", 1, PRIVATE_A_INDEX, &DOWNSTREAM_MULTICAST_INGRESS_QUEUE, 64, 1, 1 },
#endif
#if defined OREN
	{ "DS_CPU_TX_MESSAGE_DATA_BUFFER", 1, PRIVATE_A_INDEX, &DS_CPU_TX_MESSAGE_DATA_BUFFER, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_ROUTER_INGRESS_QUEUE", 1, PRIVATE_A_INDEX, &DS_ROUTER_INGRESS_QUEUE, 64, 1, 1 },
#endif
#if defined OREN
	{ "DS_VLAN_ACTION_BUFFER", 1, PRIVATE_A_INDEX, &DS_VLAN_ACTION_BUFFER, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_CPU_RX_FAST_INGRESS_QUEUE", 1, PRIVATE_A_INDEX, &DS_CPU_RX_FAST_INGRESS_QUEUE, 64, 1, 1 },
#endif
#if defined OREN
	{ "DS_DHD_TX_POST_FLOW_RING_BUFFER", 1, PRIVATE_A_INDEX, &DS_DHD_TX_POST_FLOW_RING_BUFFER, 1, 1, 1 },
#endif
#if defined OREN
	{ "HASH_BUFFER", 1, PRIVATE_A_INDEX, &HASH_BUFFER, 1, 1, 1 },
#endif
#if defined OREN
	{ "CPU_FLOW_CACHE_INGRESS_QUEUE", 1, PRIVATE_A_INDEX, &CPU_FLOW_CACHE_INGRESS_QUEUE, 64, 1, 1 },
#endif
#if defined OREN
	{ "DS_DEBUG_BUFFER", 1, PRIVATE_A_INDEX, &DS_DEBUG_BUFFER, 32, 1, 1 },
#endif
#if defined OREN
	{ "DHD_COMPLETE_RING_BUFFER", 1, PRIVATE_A_INDEX, &DHD_COMPLETE_RING_BUFFER, 3, 1, 1 },
#endif
#if defined OREN
	{ "FIREWALL_CONFIGURATION_REGISTER", 1, PRIVATE_A_INDEX, &FIREWALL_CONFIGURATION_REGISTER, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_DATA_POINTER_DUMMY_TARGET", 1, PRIVATE_A_INDEX, &DS_DATA_POINTER_DUMMY_TARGET, 5, 1, 1 },
#endif
#if defined OREN
	{ "DS_BPM_EXTRA_DDR_BUFFERS_BASE", 1, PRIVATE_A_INDEX, &DS_BPM_EXTRA_DDR_BUFFERS_BASE, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_RUNNER_FLOW_HEADER_DESCRIPTOR", 1, PRIVATE_A_INDEX, &DS_RUNNER_FLOW_HEADER_DESCRIPTOR, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_DHD_TX_POST_INGRESS_QUEUE", 1, PRIVATE_A_INDEX, &DS_DHD_TX_POST_INGRESS_QUEUE, 64, 1, 1 },
#endif
#if defined OREN
	{ "DS_DHD_FLOW_RING_DROP_COUNTER", 1, PRIVATE_A_INDEX, &DS_DHD_FLOW_RING_DROP_COUNTER, 5, 1, 1 },
#endif
#if defined OREN
	{ "IPTV_SSM_CONTEXT_TABLE_PTR", 1, PRIVATE_A_INDEX, &IPTV_SSM_CONTEXT_TABLE_PTR, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_NULL_BUFFER", 1, PRIVATE_A_INDEX, &DS_NULL_BUFFER, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_FAST_FREE_SKB_INDEXES_FIFO_LOCAL_TABLE", 1, PRIVATE_A_INDEX, &DS_FAST_FREE_SKB_INDEXES_FIFO_LOCAL_TABLE, 8, 1, 1 },
#endif
#if defined OREN
	{ "IPTV_DMA_LKP_KEY", 1, PRIVATE_A_INDEX, &IPTV_DMA_LKP_KEY, 1, 1, 1 },
#endif
#if defined OREN
	{ "WLAN_MCAST_INGRESS_QUEUE", 1, PRIVATE_A_INDEX, &WLAN_MCAST_INGRESS_QUEUE, 64, 1, 1 },
#endif
#if defined OREN
	{ "EMAC_ABSOLUTE_TX_BBH_COUNTER", 1, PRIVATE_A_INDEX, &EMAC_ABSOLUTE_TX_BBH_COUNTER, 6, 1, 1 },
#endif
#if defined OREN
	{ "FREE_PACKET_DESCRIPTORS_POOL_DESCRIPTOR", 1, PRIVATE_A_INDEX, &FREE_PACKET_DESCRIPTORS_POOL_DESCRIPTOR, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_ONE_BUFFER", 1, PRIVATE_A_INDEX, &DS_ONE_BUFFER, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_PICO_FREE_SKB_INDEXES_FIFO_LOCAL_TABLE", 1, PRIVATE_A_INDEX, &DS_PICO_FREE_SKB_INDEXES_FIFO_LOCAL_TABLE, 8, 1, 1 },
#endif
#if defined OREN
	{ "DS_IPTV_SBPM_REPLICATION_BN", 1, PRIVATE_A_INDEX, &DS_IPTV_SBPM_REPLICATION_BN, 16, 1, 1 },
#endif
#if defined OREN
	{ "DS_PARALLEL_PROCESSING_CONTEXT_INDEX_CACHE_CAM", 1, PRIVATE_A_INDEX, &DS_PARALLEL_PROCESSING_CONTEXT_INDEX_CACHE_CAM, 8, 1, 1 },
#endif
#if defined OREN
	{ "MULTICAST_HEADER_BUFFER", 1, PRIVATE_A_INDEX, &MULTICAST_HEADER_BUFFER, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_INGRESS_FILTERS_CONFIGURATION_TABLE", 1, PRIVATE_A_INDEX, &DS_INGRESS_FILTERS_CONFIGURATION_TABLE, 3, 1, 1 },
#endif
#if defined OREN
	{ "IPTV_TABLE_POINTER", 1, PRIVATE_A_INDEX, &IPTV_TABLE_POINTER, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_LAYER4_FILTERS_CONTEXT_TABLE", 1, PRIVATE_A_INDEX, &DS_LAYER4_FILTERS_CONTEXT_TABLE, 16, 1, 1 },
#endif
#if defined OREN
	{ "SERVICE_QUEUES_OVERALL_RATE_LIMITER", 1, PRIVATE_A_INDEX, &SERVICE_QUEUES_OVERALL_RATE_LIMITER, 1, 1, 1 },
#endif
#if defined OREN
	{ "GSO_TX_DHD_L2_BUFFER", 1, PRIVATE_A_INDEX, &GSO_TX_DHD_L2_BUFFER, 22, 1, 1 },
#endif
#if defined OREN
	{ "DS_MAIN_TIMER_CONTROL_DESCRIPTOR", 1, PRIVATE_A_INDEX, &DS_MAIN_TIMER_CONTROL_DESCRIPTOR, 1, 1, 1 },
#endif
#if defined OREN
	{ "CPU_TX_DHD_L2_BUFFER", 1, PRIVATE_A_INDEX, &CPU_TX_DHD_L2_BUFFER, 22, 1, 1 },
#endif
#if defined OREN
	{ "DS_PICO_TIMER_CONTROL_DESCRIPTOR", 1, PRIVATE_A_INDEX, &DS_PICO_TIMER_CONTROL_DESCRIPTOR, 1, 1, 1 },
#endif
#if defined OREN
	{ "WLAN_MCAST_SSID_STATS_STATE_TABLE", 1, PRIVATE_A_INDEX, &WLAN_MCAST_SSID_STATS_STATE_TABLE, 1, 1, 1 },
#endif
#if defined OREN
	{ "IPTV_COUNTERS_BUFFER", 1, PRIVATE_A_INDEX, &IPTV_COUNTERS_BUFFER, 1, 1, 1 },
#endif
#if defined OREN
	{ "IPTV_DMA_RW_BUFFER", 1, PRIVATE_A_INDEX, &IPTV_DMA_RW_BUFFER, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_INGRESS_CLASSIFICATION_KEY_BUFFER", 1, PRIVATE_A_INDEX, &DS_INGRESS_CLASSIFICATION_KEY_BUFFER, 1, 1, 1 },
#endif
#if defined OREN
	{ "ETH_TX_QUEUE_DUMMY_DESCRIPTOR", 1, PRIVATE_A_INDEX, &ETH_TX_QUEUE_DUMMY_DESCRIPTOR, 1, 1, 1 },
#endif
#if defined OREN
	{ "GPON_RX_DIRECT_DESCRIPTORS", 1, PRIVATE_A_INDEX, &GPON_RX_DIRECT_DESCRIPTORS, 32, 1, 1 },
#endif
#if defined OREN
	{ "FIREWALL_RULE_ENTRY_BUFFER", 1, PRIVATE_A_INDEX, &FIREWALL_RULE_ENTRY_BUFFER, 1, 1, 1 },
#endif
#if defined OREN
	{ "CPU_TX_DS_PICO_DHD_TX_POST_CONTEXT", 1, PRIVATE_A_INDEX, &CPU_TX_DS_PICO_DHD_TX_POST_CONTEXT, 1, 1, 1 },
#endif
#if defined OREN
	{ "WLAN_MCAST_DHD_TX_POST_CONTEXT", 1, PRIVATE_A_INDEX, &WLAN_MCAST_DHD_TX_POST_CONTEXT, 1, 1, 1 },
#endif
#if defined OREN
	{ "GSO_TX_DS_PICO_DHD_TX_POST_CONTEXT", 1, PRIVATE_A_INDEX, &GSO_TX_DS_PICO_DHD_TX_POST_CONTEXT, 1, 1, 1 },
#endif
#if defined OREN
	{ "SC_BUFFER", 1, PRIVATE_A_INDEX, &SC_BUFFER, 1, 1, 1 },
#endif
#if defined OREN
	{ "IPTV_CONTEXT_TABLE_POINTER", 1, PRIVATE_A_INDEX, &IPTV_CONTEXT_TABLE_POINTER, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_MULTICAST_VECTOR_TO_PORT_TABLE", 1, PRIVATE_A_INDEX, &DS_MULTICAST_VECTOR_TO_PORT_TABLE, 8, 1, 1 },
#endif
#if defined OREN
	{ "MULTICAST_DUMMY_VLAN_INDEXES_TABLE", 1, PRIVATE_A_INDEX, &MULTICAST_DUMMY_VLAN_INDEXES_TABLE, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_BPM_DDR_OPTIMIZED_BUFFERS_BASE", 1, PRIVATE_A_INDEX, &DS_BPM_DDR_OPTIMIZED_BUFFERS_BASE, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_BPM_DDR_BUFFERS_BASE", 1, PRIVATE_A_INDEX, &DS_BPM_DDR_BUFFERS_BASE, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_CONNECTION_TABLE_CONFIG", 1, PRIVATE_A_INDEX, &DS_CONNECTION_TABLE_CONFIG, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_CONTEXT_TABLE_CONFIG", 1, PRIVATE_A_INDEX, &DS_CONTEXT_TABLE_CONFIG, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_PARALLEL_PROCESSING_IH_BUFFER_VECTOR", 1, PRIVATE_A_INDEX, &DS_PARALLEL_PROCESSING_IH_BUFFER_VECTOR, 4, 1, 1 },
#endif
#if defined OREN
	{ "DS_DEBUG_PERIPHERALS_STATUS_REGISTER", 1, PRIVATE_A_INDEX, &DS_DEBUG_PERIPHERALS_STATUS_REGISTER, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_PARALLEL_PROCESSING_TASK_REORDER_FIFO", 1, PRIVATE_A_INDEX, &DS_PARALLEL_PROCESSING_TASK_REORDER_FIFO, 4, 1, 1 },
#endif
#if defined OREN
	{ "DS_DDR_ADDRESS_FOR_SYNC_DMA_POINTER", 1, PRIVATE_A_INDEX, &DS_DDR_ADDRESS_FOR_SYNC_DMA_POINTER, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_PARALLEL_PROCESSING_CONTEXT_INDEX_CACHE_TABLE", 1, PRIVATE_A_INDEX, &DS_PARALLEL_PROCESSING_CONTEXT_INDEX_CACHE_TABLE, 8, 1, 1 },
#endif
#if defined OREN
	{ "EMAC_SKB_ENQUEUED_INDEXES_FIFO_COUNTERS", 1, PRIVATE_A_INDEX, &EMAC_SKB_ENQUEUED_INDEXES_FIFO_COUNTERS, 5, 1, 1 },
#endif
#if defined OREN
	{ "ETH_TX_EMACS_STATUS", 1, PRIVATE_A_INDEX, &ETH_TX_EMACS_STATUS, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_INGRESS_CLASSIFICATION_COUNTERS_BUFFER", 1, PRIVATE_A_INDEX, &DS_INGRESS_CLASSIFICATION_COUNTERS_BUFFER, 1, 1, 1 },
#endif
#if defined OREN
	{ "HASH_BASED_FORWARDING_PORT_TABLE", 1, PRIVATE_A_INDEX, &HASH_BASED_FORWARDING_PORT_TABLE, 4, 1, 1 },
#endif
#if defined OREN
	{ "FIREWALL_IPV6_R16_BUFFER", 1, PRIVATE_A_INDEX, &FIREWALL_IPV6_R16_BUFFER, 1, 1, 1 },
#endif
#if defined OREN
	{ "CPU_TX_PICO_INGRESS_QUEUE_PTR", 1, PRIVATE_A_INDEX, &CPU_TX_PICO_INGRESS_QUEUE_PTR, 1, 1, 1 },
#endif
#if defined OREN
	{ "FREE_PACKET_DESCRIPTORS_POOL_THRESHOLD", 1, PRIVATE_A_INDEX, &FREE_PACKET_DESCRIPTORS_POOL_THRESHOLD, 1, 1, 1 },
#endif
#if defined OREN
	{ "DOWNSTREAM_MULTICAST_LAN_ENQUEUE_INGRESS_QUEUE_PTR", 1, PRIVATE_A_INDEX, &DOWNSTREAM_MULTICAST_LAN_ENQUEUE_INGRESS_QUEUE_PTR, 1, 1, 1 },
#endif
#if defined OREN
	{ "CPU_RX_PD_INGRESS_QUEUE_PTR", 1, PRIVATE_A_INDEX, &CPU_RX_PD_INGRESS_QUEUE_PTR, 1, 1, 1 },
#endif
#if defined OREN
	{ "CPU_RX_MIRRORING_PD_INGRESS_QUEUE_PTR", 1, PRIVATE_A_INDEX, &CPU_RX_MIRRORING_PD_INGRESS_QUEUE_PTR, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_CPU_RX_PICO_INGRESS_QUEUE_PTR", 1, PRIVATE_A_INDEX, &DS_CPU_RX_PICO_INGRESS_QUEUE_PTR, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_BPM_DDR_BUFFER_HEADROOM_SIZE", 1, PRIVATE_A_INDEX, &DS_BPM_DDR_BUFFER_HEADROOM_SIZE, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_BPM_DDR_BUFFER_HEADROOM_SIZE_2_BYTE_RESOLUTION", 1, PRIVATE_A_INDEX, &DS_BPM_DDR_BUFFER_HEADROOM_SIZE_2_BYTE_RESOLUTION, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_CPU_RX_FAST_INGRESS_QUEUE_PTR", 1, PRIVATE_A_INDEX, &DS_CPU_RX_FAST_INGRESS_QUEUE_PTR, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_PARALLEL_PROCESSING_IH_BUFFER_VECTOR_PTR", 1, PRIVATE_A_INDEX, &DS_PARALLEL_PROCESSING_IH_BUFFER_VECTOR_PTR, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_MICROCODE_VERSION", 1, PRIVATE_A_INDEX, &DS_MICROCODE_VERSION, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_RATE_LIMITER_COUNTER_BUFFER", 1, PRIVATE_A_INDEX, &DS_RATE_LIMITER_COUNTER_BUFFER, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_DDR_ADDRESS_FOR_FULL_READ_DMA_POINTER", 1, PRIVATE_A_INDEX, &DS_DDR_ADDRESS_FOR_FULL_READ_DMA_POINTER, 1, 1, 1 },
#endif
#if defined OREN
	{ "GSO_TX_DHD_HOST_BUF_PTR", 1, PRIVATE_A_INDEX, &GSO_TX_DHD_HOST_BUF_PTR, 4, 1, 1 },
#endif
#if defined OREN
	{ "DHD_TX_POST_PD_INGRESS_QUEUE_WR_PTR", 1, PRIVATE_A_INDEX, &DHD_TX_POST_PD_INGRESS_QUEUE_WR_PTR, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_MEMLIB_SEMAPHORE", 1, PRIVATE_A_INDEX, &DS_MEMLIB_SEMAPHORE, 1, 1, 1 },
#endif
#if defined OREN
	{ "CPU_TX_DHD_HOST_BUF_PTR", 1, PRIVATE_A_INDEX, &CPU_TX_DHD_HOST_BUF_PTR, 4, 1, 1 },
#endif
#if defined OREN
	{ "DS_DHD_TX_POST_HOST_DATA_PTR_BUFFER", 1, PRIVATE_A_INDEX, &DS_DHD_TX_POST_HOST_DATA_PTR_BUFFER, 1, 1, 1 },
#endif
#if defined OREN
	{ "DUAL_STACK_LITE_LAYER_2_HEADER_LENGTH", 1, PRIVATE_A_INDEX, &DUAL_STACK_LITE_LAYER_2_HEADER_LENGTH, 1, 1, 1 },
#endif
#if defined OREN
	{ "DUAL_STACK_LITE_LAYER_3_HEADER_LENGTH", 1, PRIVATE_A_INDEX, &DUAL_STACK_LITE_LAYER_3_HEADER_LENGTH, 1, 1, 1 },
#endif
#if defined OREN
	{ "DOWNSTREAM_MULTICAST_LAN_ENQUEUE_SERVICE_QUEUE_INGRESS_QUEUE_PTR", 1, PRIVATE_A_INDEX, &DOWNSTREAM_MULTICAST_LAN_ENQUEUE_SERVICE_QUEUE_INGRESS_QUEUE_PTR, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_RUNNER_CONGESTION_STATE", 1, PRIVATE_A_INDEX, &DS_RUNNER_CONGESTION_STATE, 1, 1, 1 },
#endif
#if defined OREN
	{ "FIREWALL_RULE_MAP_ENTRY_BUFFER", 1, PRIVATE_A_INDEX, &FIREWALL_RULE_MAP_ENTRY_BUFFER, 1, 1, 1 },
#endif
#if defined OREN
	{ "DHD_TX_COMPLETE_BPM_REF_COUNTER", 1, PRIVATE_A_INDEX, &DHD_TX_COMPLETE_BPM_REF_COUNTER, 1, 1, 1 },
#endif
#if defined OREN
	{ "DHD_TX_POST_CPU_BPM_REF_COUNTER", 1, PRIVATE_A_INDEX, &DHD_TX_POST_CPU_BPM_REF_COUNTER, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_DHD_BPM_CONGESTION_UG3_DROP_COUNTER", 1, PRIVATE_A_INDEX, &DS_DHD_BPM_CONGESTION_UG3_DROP_COUNTER, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_DHD_BPM_CONGESTION_ALLOC_FAIL_DROP_COUNTER", 1, PRIVATE_A_INDEX, &DS_DHD_BPM_CONGESTION_ALLOC_FAIL_DROP_COUNTER, 1, 1, 1 },
#endif
#if defined OREN
	{ "PRIVATE_A_DUMMY_STORE", 1, PRIVATE_A_INDEX, &PRIVATE_A_DUMMY_STORE, 1, 1, 1 },
#endif
#if defined OREN
	{ "ETH_TX_INTER_LAN_SCHEDULING_OFFSET", 1, PRIVATE_A_INDEX, &ETH_TX_INTER_LAN_SCHEDULING_OFFSET, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_FAST_FREE_SKB_INDEXES_FIFO_LOCAL_TABLE_PTR", 1, PRIVATE_A_INDEX, &DS_FAST_FREE_SKB_INDEXES_FIFO_LOCAL_TABLE_PTR, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_PICO_FREE_SKB_INDEXES_FIFO_LOCAL_TABLE_PTR", 1, PRIVATE_A_INDEX, &DS_PICO_FREE_SKB_INDEXES_FIFO_LOCAL_TABLE_PTR, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_INGRESS_CLASSIFICATION_RULE_CFG_DESCRIPTOR", 1, PRIVATE_A_INDEX, &DS_INGRESS_CLASSIFICATION_RULE_CFG_DESCRIPTOR, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_INGRESS_CLASSIFICATION_IP_FLOW_RULE_CFG_DESCRIPTOR", 1, PRIVATE_A_INDEX, &DS_INGRESS_CLASSIFICATION_IP_FLOW_RULE_CFG_DESCRIPTOR, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_PACKET_BUFFER_SIZE_ASR_8", 1, PRIVATE_A_INDEX, &DS_PACKET_BUFFER_SIZE_ASR_8, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_MAIN_DMA_SYNCRONIZATION_ADDRESS", 1, PRIVATE_A_INDEX, &DS_MAIN_DMA_SYNCRONIZATION_ADDRESS, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_PICO_DMA_SYNCRONIZATION_ADDRESS", 1, PRIVATE_A_INDEX, &DS_PICO_DMA_SYNCRONIZATION_ADDRESS, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_RUNNER_FLOW_IH_RESPONSE_MUTEX", 1, PRIVATE_A_INDEX, &DS_RUNNER_FLOW_IH_RESPONSE_MUTEX, 1, 1, 1 },
#endif
#if defined OREN
	{ "DOWNSTREAM_MULTICAST_SERVICE_QUEUE_SSID_PTR", 1, PRIVATE_A_INDEX, &DOWNSTREAM_MULTICAST_SERVICE_QUEUE_SSID_PTR, 1, 1, 1 },
#endif
#if defined OREN
	{ "FIREWALL_RULE_MAP_ENTRY_BUFFER_SYNC", 1, PRIVATE_A_INDEX, &FIREWALL_RULE_MAP_ENTRY_BUFFER_SYNC, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_PARALLEL_PROCESSING_SLAVE_VECTOR", 1, PRIVATE_A_INDEX, &DS_PARALLEL_PROCESSING_SLAVE_VECTOR, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_PARALLEL_PROCESSING_CONTEXT_CACHE_MODE", 1, PRIVATE_A_INDEX, &DS_PARALLEL_PROCESSING_CONTEXT_CACHE_MODE, 1, 1, 1 },
#endif
#if defined OREN
	{ "IP_SYNC_1588_TX_ENQUEUE_RESULT", 1, PRIVATE_A_INDEX, &IP_SYNC_1588_TX_ENQUEUE_RESULT, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_DHD_DMA_SYNCHRONIZATION", 1, PRIVATE_A_INDEX, &DS_DHD_DMA_SYNCHRONIZATION, 1, 1, 1 },
#endif
#if defined OREN
	{ "DHD_TX_POST_CPU_SEMAPHORE", 1, PRIVATE_A_INDEX, &DHD_TX_POST_CPU_SEMAPHORE, 1, 1, 1 },
#endif
#if defined OREN
	{ "PCI_TX_FIFO_DESCRIPTORS_TABLE", 1, PRIVATE_A_INDEX, &PCI_TX_FIFO_DESCRIPTORS_TABLE, 4, 1, 1 },
#endif
#if defined OREN
	{ "PCI_TX_QUEUES_VECTOR", 1, PRIVATE_A_INDEX, &PCI_TX_QUEUES_VECTOR, 1, 1, 1 },
#endif
#if defined OREN
	{ "PCI_TX_FIFO_FULL_VECTOR", 1, PRIVATE_A_INDEX, &PCI_TX_FIFO_FULL_VECTOR, 1, 1, 1 },
#endif
#if defined OREN
	{ "RUNNER_FLOW_IH_RESPONSE", 1, PRIVATE_A_INDEX, &RUNNER_FLOW_IH_RESPONSE, 1, 1, 1 },
#endif
#if defined OREN
	{ "GPON_RX_NORMAL_DESCRIPTORS", 1, PRIVATE_A_INDEX, &GPON_RX_NORMAL_DESCRIPTORS, 32, 1, 1 },
#endif
#if defined OREN
	{ "DOWNSTREAM_DMA_PIPE_BUFFER", 1, PRIVATE_A_INDEX, &DOWNSTREAM_DMA_PIPE_BUFFER, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_INGRESS_HANDLER_BUFFER", 1, PRIVATE_B_INDEX, &US_INGRESS_HANDLER_BUFFER, 32, 1, 1 },
#endif
#if defined OREN
	{ "US_FREE_PACKET_DESCRIPTORS_POOL", 1, PRIVATE_B_INDEX, &US_FREE_PACKET_DESCRIPTORS_POOL, 3072, 1, 1 },
#endif
#if defined OREN
	{ "US_INGRESS_CLASSIFICATION_CONTEXT_TABLE", 1, PRIVATE_B_INDEX, &US_INGRESS_CLASSIFICATION_CONTEXT_TABLE, 256, 1, 1 },
#endif
#if defined OREN
	{ "US_DSCP_TO_PBITS_TABLE", 1, PRIVATE_B_INDEX, &US_DSCP_TO_PBITS_TABLE, 6, 64, 1 },
#endif
#if defined OREN
	{ "US_SBPM_REPLY", 1, PRIVATE_B_INDEX, &US_SBPM_REPLY, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_VLAN_PARAMETER_TABLE", 1, PRIVATE_B_INDEX, &US_VLAN_PARAMETER_TABLE, 128, 1, 1 },
#endif
#if defined OREN
	{ "WAN_CHANNELS_8_39_TABLE", 1, PRIVATE_B_INDEX, &WAN_CHANNELS_8_39_TABLE, 32, 1, 1 },
#endif
#if defined OREN
	{ "US_RUNNER_FLOW_HEADER_BUFFER", 1, PRIVATE_B_INDEX, &US_RUNNER_FLOW_HEADER_BUFFER, 3, 1, 1 },
#endif
#if defined OREN
	{ "US_QUEUE_PROFILE_TABLE", 1, PRIVATE_B_INDEX, &US_QUEUE_PROFILE_TABLE, 8, 1, 1 },
#endif
#if defined OREN
	{ "US_CPU_REASON_TO_METER_TABLE", 1, PRIVATE_B_INDEX, &US_CPU_REASON_TO_METER_TABLE, 64, 1, 1 },
#endif
#if defined OREN
	{ "US_FAST_CPU_TX_DESCRIPTOR_ABS_DATA_PTR_QUEUE", 1, PRIVATE_B_INDEX, &US_FAST_CPU_TX_DESCRIPTOR_ABS_DATA_PTR_QUEUE, 16, 1, 1 },
#endif
#if defined OREN
	{ "US_PICO_CPU_TX_DESCRIPTOR_ABS_DATA_PTR_QUEUE", 1, PRIVATE_B_INDEX, &US_PICO_CPU_TX_DESCRIPTOR_ABS_DATA_PTR_QUEUE, 16, 1, 1 },
#endif
#if defined OREN
	{ "US_PBITS_TO_QOS_TABLE", 1, PRIVATE_B_INDEX, &US_PBITS_TO_QOS_TABLE, 8, 8, 1 },
#endif
#if defined OREN
	{ "US_RATE_LIMITER_TABLE", 1, PRIVATE_B_INDEX, &US_RATE_LIMITER_TABLE, 16, 1, 1 },
#endif
#if defined OREN
	{ "US_LAYER4_FILTERS_LOOKUP_TABLE", 1, PRIVATE_B_INDEX, &US_LAYER4_FILTERS_LOOKUP_TABLE, 16, 1, 1 },
#endif
#if defined OREN
	{ "US_WAN_FLOW_TABLE", 1, PRIVATE_B_INDEX, &US_WAN_FLOW_TABLE, 256, 1, 1 },
#endif
#if defined OREN
	{ "US_INGRESS_FILTERS_LOOKUP_TABLE", 1, PRIVATE_B_INDEX, &US_INGRESS_FILTERS_LOOKUP_TABLE, 6, 32, 1 },
#endif
#if defined OREN
	{ "US_PBITS_TO_PBITS_TABLE", 1, PRIVATE_B_INDEX, &US_PBITS_TO_PBITS_TABLE, 32, 8, 1 },
#endif
#if defined OREN
	{ "WAN_CHANNELS_0_7_TABLE", 1, PRIVATE_B_INDEX, &WAN_CHANNELS_0_7_TABLE, 8, 1, 1 },
#endif
#if defined OREN
	{ "US_TRAFFIC_CLASS_TO_QUEUE_TABLE", 1, PRIVATE_B_INDEX, &US_TRAFFIC_CLASS_TO_QUEUE_TABLE, 8, 8, 1 },
#endif
#if defined OREN
	{ "US_PBITS_PARAMETER_TABLE", 1, PRIVATE_B_INDEX, &US_PBITS_PARAMETER_TABLE, 128, 1, 1 },
#endif
#if defined OREN
	{ "SMART_CARD_DESCRIPTOR_TABLE", 1, PRIVATE_B_INDEX, &SMART_CARD_DESCRIPTOR_TABLE, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_VLAN_COMMANDS_TABLE", 1, PRIVATE_B_INDEX, &US_VLAN_COMMANDS_TABLE, 64, 1, 1 },
#endif
#if defined OREN
	{ "US_RATE_CONTROL_BUDGET_ALLOCATOR_TABLE", 1, PRIVATE_B_INDEX, &US_RATE_CONTROL_BUDGET_ALLOCATOR_TABLE, 8, 1, 1 },
#endif
#if defined OREN
	{ "US_MAIN_TIMER_TASK_DESCRIPTOR_TABLE", 1, PRIVATE_B_INDEX, &US_MAIN_TIMER_TASK_DESCRIPTOR_TABLE, 4, 1, 1 },
#endif
#if defined OREN
	{ "US_PBITS_TO_WAN_FLOW_TABLE", 1, PRIVATE_B_INDEX, &US_PBITS_TO_WAN_FLOW_TABLE, 8, 8, 1 },
#endif
#if defined OREN
	{ "US_CPU_RX_METER_TABLE", 1, PRIVATE_B_INDEX, &US_CPU_RX_METER_TABLE, 16, 1, 1 },
#endif
#if defined OREN
	{ "US_LAN_VID_TABLE", 1, PRIVATE_B_INDEX, &US_LAN_VID_TABLE, 128, 1, 1 },
#endif
#if defined OREN
	{ "US_POLICER_TABLE", 1, PRIVATE_B_INDEX, &US_POLICER_TABLE, 16, 1, 1 },
#endif
#if defined OREN
	{ "US_PACKET_BUFFER_TABLE", 1, PRIVATE_B_INDEX, &US_PACKET_BUFFER_TABLE, 256, 1, 1 },
#endif
#if defined OREN
	{ "US_FORWARDING_MATRIX_TABLE", 1, PRIVATE_B_INDEX, &US_FORWARDING_MATRIX_TABLE, 9, 16, 1 },
#endif
#if defined OREN
	{ "US_TPID_OVERWRITE_TABLE", 1, PRIVATE_B_INDEX, &US_TPID_OVERWRITE_TABLE, 8, 1, 1 },
#endif
#if defined OREN
	{ "US_PICO_TIMER_TASK_DESCRIPTOR_TABLE", 1, PRIVATE_B_INDEX, &US_PICO_TIMER_TASK_DESCRIPTOR_TABLE, 4, 1, 1 },
#endif
#if defined OREN
	{ "US_INGRESS_CLASSIFICATION_KEY_PRIMITIVE_TABLE", 1, PRIVATE_B_INDEX, &US_INGRESS_CLASSIFICATION_KEY_PRIMITIVE_TABLE, 32, 1, 1 },
#endif
#if defined OREN
	{ "US_CPU_TX_BBH_DESCRIPTORS", 1, PRIVATE_B_INDEX, &US_CPU_TX_BBH_DESCRIPTORS, 32, 1, 1 },
#endif
#if defined OREN
	{ "US_INGRESS_HANDLER_SKB_DATA_POINTER", 1, PRIVATE_B_INDEX, &US_INGRESS_HANDLER_SKB_DATA_POINTER, 32, 1, 1 },
#endif
#if defined OREN
	{ "US_TIMER_SCHEDULER_PRIMITIVE_TABLE", 1, PRIVATE_B_INDEX, &US_TIMER_SCHEDULER_PRIMITIVE_TABLE, 8, 1, 1 },
#endif
#if defined OREN
	{ "GPON_ABSOLUTE_TX_BBH_COUNTER", 1, PRIVATE_B_INDEX, &GPON_ABSOLUTE_TX_BBH_COUNTER, 40, 1, 1 },
#endif
#if defined OREN
	{ "US_CPU_PARAMETERS_BLOCK", 1, PRIVATE_B_INDEX, &US_CPU_PARAMETERS_BLOCK, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_FLOW_BASED_ACTION_PTR_TABLE", 1, PRIVATE_B_INDEX, &US_FLOW_BASED_ACTION_PTR_TABLE, 32, 1, 1 },
#endif
#if defined OREN
	{ "US_CONNECTION_CONTEXT_BUFFER", 1, PRIVATE_B_INDEX, &US_CONNECTION_CONTEXT_BUFFER, 4, 1, 1 },
#endif
#if defined OREN
	{ "US_BRIDGE_CONFIGURATION_REGISTER", 1, PRIVATE_B_INDEX, &US_BRIDGE_CONFIGURATION_REGISTER, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_CPU_TX_FAST_QUEUE", 1, PRIVATE_B_INDEX, &US_CPU_TX_FAST_QUEUE, 16, 1, 1 },
#endif
#if defined OREN
	{ "US_INGRESS_CLASSIFICATION_RULE_CFG_TABLE", 1, PRIVATE_B_INDEX, &US_INGRESS_CLASSIFICATION_RULE_CFG_TABLE, 16, 1, 1 },
#endif
#if defined OREN
	{ "US_CPU_TX_PICO_QUEUE", 1, PRIVATE_B_INDEX, &US_CPU_TX_PICO_QUEUE, 16, 1, 1 },
#endif
#if defined OREN
	{ "US_INGRESS_FILTERS_PARAMETER_TABLE", 1, PRIVATE_B_INDEX, &US_INGRESS_FILTERS_PARAMETER_TABLE, 6, 16, 1 },
#endif
#if defined OREN
	{ "US_INGRESS_CLASSIFICATION_GENERIC_RULE_CFG_TABLE", 1, PRIVATE_B_INDEX, &US_INGRESS_CLASSIFICATION_GENERIC_RULE_CFG_TABLE, 4, 1, 1 },
#endif
#if defined OREN
	{ "US_PROFILING_BUFFER_PICO_RUNNER", 1, PRIVATE_B_INDEX, &US_PROFILING_BUFFER_PICO_RUNNER, 1, 1, 1 },
#endif
#if defined OREN
	{ "DHD_RX_COMPLETE_FLOW_RING_BUFFER", 1, PRIVATE_B_INDEX, &DHD_RX_COMPLETE_FLOW_RING_BUFFER, 3, 1, 1 },
#endif
#if defined OREN
	{ "US_FAST_RUNNER_GLOBAL_REGISTERS_INIT", 1, PRIVATE_B_INDEX, &US_FAST_RUNNER_GLOBAL_REGISTERS_INIT, 8, 1, 1 },
#endif
#if defined OREN
	{ "DHD_RX_POST_FLOW_RING_BUFFER", 1, PRIVATE_B_INDEX, &DHD_RX_POST_FLOW_RING_BUFFER, 3, 1, 1 },
#endif
#if defined OREN
	{ "US_PICO_RUNNER_GLOBAL_REGISTERS_INIT", 1, PRIVATE_B_INDEX, &US_PICO_RUNNER_GLOBAL_REGISTERS_INIT, 8, 1, 1 },
#endif
#if defined OREN
	{ "ETH0_RX_DESCRIPTORS", 1, PRIVATE_B_INDEX, &ETH0_RX_DESCRIPTORS, 32, 1, 1 },
#endif
#if defined OREN
	{ "US_INGRESS_RATE_LIMITER_TABLE", 1, PRIVATE_B_INDEX, &US_INGRESS_RATE_LIMITER_TABLE, 5, 1, 1 },
#endif
#if defined OREN
	{ "US_VLAN_OPTIMIZATION_TABLE", 1, PRIVATE_B_INDEX, &US_VLAN_OPTIMIZATION_TABLE, 128, 1, 1 },
#endif
#if defined OREN
	{ "US_INGRESS_CLASSIFICATION_COUNTERS_BUFFER", 1, PRIVATE_B_INDEX, &US_INGRESS_CLASSIFICATION_COUNTERS_BUFFER, 6, 1, 1 },
#endif
#if defined OREN
	{ "US_BPM_EXTRA_DDR_BUFFERS_BASE", 1, PRIVATE_B_INDEX, &US_BPM_EXTRA_DDR_BUFFERS_BASE, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_INGRESS_FILTERS_CONFIGURATION_TABLE", 1, PRIVATE_B_INDEX, &US_INGRESS_FILTERS_CONFIGURATION_TABLE, 6, 1, 1 },
#endif
#if defined OREN
	{ "SMART_CARD_ERROR_COUNTERS_TABLE", 1, PRIVATE_B_INDEX, &SMART_CARD_ERROR_COUNTERS_TABLE, 1, 1, 1 },
#endif
#if defined OREN
	{ "ETH1_RX_DESCRIPTORS", 1, PRIVATE_B_INDEX, &ETH1_RX_DESCRIPTORS, 32, 1, 1 },
#endif
#if defined OREN
	{ "US_PBITS_PRIMITIVE_TABLE", 1, PRIVATE_B_INDEX, &US_PBITS_PRIMITIVE_TABLE, 32, 1, 1 },
#endif
#if defined OREN
	{ "SPEED_SERVICE_PARAMETERS_TABLE", 1, PRIVATE_B_INDEX, &SPEED_SERVICE_PARAMETERS_TABLE, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_MAIN_TIMER_CONTROL_DESCRIPTOR", 1, PRIVATE_B_INDEX, &US_MAIN_TIMER_CONTROL_DESCRIPTOR, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_BPM_DDR_OPTIMIZED_BUFFERS_BASE", 1, PRIVATE_B_INDEX, &US_BPM_DDR_OPTIMIZED_BUFFERS_BASE, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_LAYER4_FILTERS_CONTEXT_TABLE", 1, PRIVATE_B_INDEX, &US_LAYER4_FILTERS_CONTEXT_TABLE, 16, 1, 1 },
#endif
#if defined OREN
	{ "US_CPU_RX_PICO_INGRESS_QUEUE", 1, PRIVATE_B_INDEX, &US_CPU_RX_PICO_INGRESS_QUEUE, 64, 1, 1 },
#endif
#if defined OREN
	{ "US_ACK_PACKETS_QUEUE_INDEX_TABLE", 1, PRIVATE_B_INDEX, &US_ACK_PACKETS_QUEUE_INDEX_TABLE, 40, 1, 1 },
#endif
#if defined OREN
	{ "CPU_REASON_AND_SRC_BRIDGE_PORT_TO_METER_TABLE", 1, PRIVATE_B_INDEX, &CPU_REASON_AND_SRC_BRIDGE_PORT_TO_METER_TABLE, 3, 6, 1 },
#endif
#if defined OREN
	{ "US_PICO_TIMER_CONTROL_DESCRIPTOR", 1, PRIVATE_B_INDEX, &US_PICO_TIMER_CONTROL_DESCRIPTOR, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_BPM_DDR_BUFFERS_BASE", 1, PRIVATE_B_INDEX, &US_BPM_DDR_BUFFERS_BASE, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_GPON_RX_DIRECT_DESCRIPTORS", 1, PRIVATE_B_INDEX, &US_GPON_RX_DIRECT_DESCRIPTORS, 32, 1, 1 },
#endif
#if defined OREN
	{ "US_ROUTER_INGRESS_QUEUE", 1, PRIVATE_B_INDEX, &US_ROUTER_INGRESS_QUEUE, 64, 1, 1 },
#endif
#if defined OREN
	{ "US_VLAN_PRIMITIVE_TABLE", 1, PRIVATE_B_INDEX, &US_VLAN_PRIMITIVE_TABLE, 32, 1, 1 },
#endif
#if defined OREN
	{ "US_CPU_RX_FAST_INGRESS_QUEUE", 1, PRIVATE_B_INDEX, &US_CPU_RX_FAST_INGRESS_QUEUE, 64, 1, 1 },
#endif
#if defined OREN
	{ "US_DEBUG_BUFFER", 1, PRIVATE_B_INDEX, &US_DEBUG_BUFFER, 32, 1, 1 },
#endif
#if defined OREN
	{ "US_CPU_TX_MESSAGE_DATA_BUFFER", 1, PRIVATE_B_INDEX, &US_CPU_TX_MESSAGE_DATA_BUFFER, 1, 1, 1 },
#endif
#if defined OREN
	{ "LOCAL_SWITCHING_MULTICAST_INGRESS_QUEUE", 1, PRIVATE_B_INDEX, &LOCAL_SWITCHING_MULTICAST_INGRESS_QUEUE, 64, 1, 1 },
#endif
#if defined OREN
	{ "US_RUNNER_FLOW_HEADER_DESCRIPTOR", 1, PRIVATE_B_INDEX, &US_RUNNER_FLOW_HEADER_DESCRIPTOR, 3, 1, 1 },
#endif
#if defined OREN
	{ "US_PARALLEL_PROCESSING_CONTEXT_INDEX_CACHE_CAM", 1, PRIVATE_B_INDEX, &US_PARALLEL_PROCESSING_CONTEXT_INDEX_CACHE_CAM, 4, 1, 1 },
#endif
#if defined OREN
	{ "US_NULL_BUFFER", 1, PRIVATE_B_INDEX, &US_NULL_BUFFER, 1, 1, 1 },
#endif
#if defined OREN
	{ "BBH_TX_EPON_REQUEST_WAN_CHANNEL_INDEX", 1, PRIVATE_B_INDEX, &BBH_TX_EPON_REQUEST_WAN_CHANNEL_INDEX, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_RUNNER_FLOW_IH_RESPONSE", 1, PRIVATE_B_INDEX, &US_RUNNER_FLOW_IH_RESPONSE, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_MULTICAST_VECTOR_TO_PORT_TABLE", 1, PRIVATE_B_INDEX, &US_MULTICAST_VECTOR_TO_PORT_TABLE, 8, 1, 1 },
#endif
#if defined OREN
	{ "VLAN_ACTION_BRIDGE_INGRESS_QUEUE", 1, PRIVATE_B_INDEX, &VLAN_ACTION_BRIDGE_INGRESS_QUEUE, 64, 1, 1 },
#endif
#if defined OREN
	{ "US_VLAN_ACTION_BUFFER", 1, PRIVATE_B_INDEX, &US_VLAN_ACTION_BUFFER, 1, 1, 1 },
#endif
#if defined OREN
	{ "UPSTREAM_FLOODING_INGRESS_QUEUE", 1, PRIVATE_B_INDEX, &UPSTREAM_FLOODING_INGRESS_QUEUE, 64, 1, 1 },
#endif
#if defined OREN
	{ "US_DHD_TX_POST_FLOW_RING_BUFFER", 1, PRIVATE_B_INDEX, &US_DHD_TX_POST_FLOW_RING_BUFFER, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_INGRESS_CLASSIFICATION_KEY_BUFFER", 1, PRIVATE_B_INDEX, &US_INGRESS_CLASSIFICATION_KEY_BUFFER, 1, 1, 1 },
#endif
#if defined OREN
	{ "ETH2_RX_DESCRIPTORS", 1, PRIVATE_B_INDEX, &ETH2_RX_DESCRIPTORS, 32, 1, 1 },
#endif
#if defined OREN
	{ "WAN_INTERWORKING_INGRESS_QUEUE", 1, PRIVATE_B_INDEX, &WAN_INTERWORKING_INGRESS_QUEUE, 64, 1, 1 },
#endif
#if defined OREN
	{ "CPU_TX_DS_EGRESS_DHD_TX_POST_FLOW_RING_BUFFER", 1, PRIVATE_B_INDEX, &CPU_TX_DS_EGRESS_DHD_TX_POST_FLOW_RING_BUFFER, 1, 1, 1 },
#endif
#if defined OREN
	{ "CPU_TX_DS_EGRESS_DHD_TX_POST_CONTEXT", 1, PRIVATE_B_INDEX, &CPU_TX_DS_EGRESS_DHD_TX_POST_CONTEXT, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_DHD_TX_POST_INGRESS_QUEUE", 1, PRIVATE_B_INDEX, &US_DHD_TX_POST_INGRESS_QUEUE, 64, 1, 1 },
#endif
#if defined OREN
	{ "LAN_INGRESS_FIFO_DESCRIPTOR_TABLE", 1, PRIVATE_B_INDEX, &LAN_INGRESS_FIFO_DESCRIPTOR_TABLE, 5, 1, 1 },
#endif
#if defined OREN
	{ "US_CONNECTION_TABLE_CONFIG", 1, PRIVATE_B_INDEX, &US_CONNECTION_TABLE_CONFIG, 1, 1, 1 },
#endif
#if defined OREN
	{ "GPON_ABSOLUTE_TX_FIRMWARE_COUNTER", 1, PRIVATE_B_INDEX, &GPON_ABSOLUTE_TX_FIRMWARE_COUNTER, 40, 1, 1 },
#endif
#if defined OREN
	{ "ETH3_RX_DESCRIPTORS", 1, PRIVATE_B_INDEX, &ETH3_RX_DESCRIPTORS, 32, 1, 1 },
#endif
#if defined OREN
	{ "US_OVERALL_RATE_LIMITER_WAN_CHANNEL_PTR_TABLE", 1, PRIVATE_B_INDEX, &US_OVERALL_RATE_LIMITER_WAN_CHANNEL_PTR_TABLE, 48, 1, 1 },
#endif
#if defined OREN
	{ "US_FAST_FREE_SKB_INDEXES_FIFO_LOCAL_TABLE", 1, PRIVATE_B_INDEX, &US_FAST_FREE_SKB_INDEXES_FIFO_LOCAL_TABLE, 8, 1, 1 },
#endif
#if defined OREN
	{ "IPV6_LOCAL_IP", 1, PRIVATE_B_INDEX, &IPV6_LOCAL_IP, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_CPU_RX_PICO_INGRESS_QUEUE_PTR", 1, PRIVATE_B_INDEX, &US_CPU_RX_PICO_INGRESS_QUEUE_PTR, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_DHD_FLOW_RING_DROP_COUNTER", 1, PRIVATE_B_INDEX, &US_DHD_FLOW_RING_DROP_COUNTER, 5, 1, 1 },
#endif
#if defined OREN
	{ "US_CONTEXT_TABLE_CONFIG", 1, PRIVATE_B_INDEX, &US_CONTEXT_TABLE_CONFIG, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_PARALLEL_PROCESSING_IH_BUFFER_VECTOR", 1, PRIVATE_B_INDEX, &US_PARALLEL_PROCESSING_IH_BUFFER_VECTOR, 4, 1, 1 },
#endif
#if defined OREN
	{ "US_DEBUG_PERIPHERALS_STATUS_REGISTER", 1, PRIVATE_B_INDEX, &US_DEBUG_PERIPHERALS_STATUS_REGISTER, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_PICO_FREE_SKB_INDEXES_FIFO_LOCAL_TABLE", 1, PRIVATE_B_INDEX, &US_PICO_FREE_SKB_INDEXES_FIFO_LOCAL_TABLE, 8, 1, 1 },
#endif
#if defined OREN
	{ "US_INGRESS_CLASSIFICATION_DEFAULT_FLOWS_TABLE", 1, PRIVATE_B_INDEX, &US_INGRESS_CLASSIFICATION_DEFAULT_FLOWS_TABLE, 32, 1, 1 },
#endif
#if defined OREN
	{ "CPU_TX_DHD_LAYER2_HEADER_BUFFER", 1, PRIVATE_B_INDEX, &CPU_TX_DHD_LAYER2_HEADER_BUFFER, 14, 1, 1 },
#endif
#if defined OREN
	{ "US_BPM_DDR_BUFFER_HEADROOM_SIZE", 1, PRIVATE_B_INDEX, &US_BPM_DDR_BUFFER_HEADROOM_SIZE, 1, 1, 1 },
#endif
#if defined OREN
	{ "LOCAL_SWITCHING_MODE_TABLE", 1, PRIVATE_B_INDEX, &LOCAL_SWITCHING_MODE_TABLE, 6, 1, 1 },
#endif
#if defined OREN
	{ "US_BPM_DDR_BUFFER_HEADROOM_SIZE_2_BYTE_RESOLUTION", 1, PRIVATE_B_INDEX, &US_BPM_DDR_BUFFER_HEADROOM_SIZE_2_BYTE_RESOLUTION, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_PARALLEL_PROCESSING_TASK_REORDER_FIFO", 1, PRIVATE_B_INDEX, &US_PARALLEL_PROCESSING_TASK_REORDER_FIFO, 4, 1, 1 },
#endif
#if defined OREN
	{ "IH_BUFFER_BBH_POINTER", 1, PRIVATE_B_INDEX, &IH_BUFFER_BBH_POINTER, 1, 1, 1 },
#endif
#if defined OREN
	{ "BROADCOM_SWITCH_PORT_TO_BRIDGE_PORT_MAPPING_TABLE", 1, PRIVATE_B_INDEX, &BROADCOM_SWITCH_PORT_TO_BRIDGE_PORT_MAPPING_TABLE, 8, 1, 1 },
#endif
#if defined OREN
	{ "US_DDR_ADDRESS_FOR_SYNC_DMA_POINTER", 1, PRIVATE_B_INDEX, &US_DDR_ADDRESS_FOR_SYNC_DMA_POINTER, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_DATA_POINTER_DUMMY_TARGET", 1, PRIVATE_B_INDEX, &US_DATA_POINTER_DUMMY_TARGET, 1, 1, 1 },
#endif
#if defined OREN
	{ "INGRESS_VLAN_SWITCHING_ISOLATION_CONFIG_TABLE", 1, PRIVATE_B_INDEX, &INGRESS_VLAN_SWITCHING_ISOLATION_CONFIG_TABLE, 5, 1, 1 },
#endif
#if defined OREN
	{ "US_TX_QUEUE_FLOW_CONTROL_BRIDGE_PORT_TABLE", 1, PRIVATE_B_INDEX, &US_TX_QUEUE_FLOW_CONTROL_BRIDGE_PORT_TABLE, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_CPU_RX_FAST_INGRESS_QUEUE_PTR", 1, PRIVATE_B_INDEX, &US_CPU_RX_FAST_INGRESS_QUEUE_PTR, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_MICROCODE_VERSION", 1, PRIVATE_B_INDEX, &US_MICROCODE_VERSION, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_RATE_LIMITER_COUNTER_BUFFER", 1, PRIVATE_B_INDEX, &US_RATE_LIMITER_COUNTER_BUFFER, 1, 1, 1 },
#endif
#if defined OREN
	{ "LOCAL_SWITCHING_MULTICAST_LAN_ENQUEUE_INGRESS_QUEUE_PTR", 1, PRIVATE_B_INDEX, &LOCAL_SWITCHING_MULTICAST_LAN_ENQUEUE_INGRESS_QUEUE_PTR, 1, 1, 1 },
#endif
#if defined OREN
	{ "LOCAL_SWITCHING_LAN_ENQUEUE_INGRESS_QUEUE_PTR", 1, PRIVATE_B_INDEX, &LOCAL_SWITCHING_LAN_ENQUEUE_INGRESS_QUEUE_PTR, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_PARALLEL_PROCESSING_IH_BUFFER_VECTOR_PTR", 1, PRIVATE_B_INDEX, &US_PARALLEL_PROCESSING_IH_BUFFER_VECTOR_PTR, 1, 1, 1 },
#endif
#if defined OREN
	{ "IP_SYNC_1588_DESCRIPTOR_QUEUE_POINTER", 1, PRIVATE_B_INDEX, &IP_SYNC_1588_DESCRIPTOR_QUEUE_POINTER, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_PARALLEL_PROCESSING_CONTEXT_INDEX_CACHE_TABLE", 1, PRIVATE_B_INDEX, &US_PARALLEL_PROCESSING_CONTEXT_INDEX_CACHE_TABLE, 4, 1, 1 },
#endif
#if defined OREN
	{ "US_DDR_ADDRESS_FOR_FULL_READ_DMA_POINTER", 1, PRIVATE_B_INDEX, &US_DDR_ADDRESS_FOR_FULL_READ_DMA_POINTER, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_DHD_TX_POST_HOST_DATA_PTR_BUFFER", 1, PRIVATE_B_INDEX, &US_DHD_TX_POST_HOST_DATA_PTR_BUFFER, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_MEMLIB_SEMAPHORE", 1, PRIVATE_B_INDEX, &US_MEMLIB_SEMAPHORE, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_RUNNER_CONGESTION_STATE", 1, PRIVATE_B_INDEX, &US_RUNNER_CONGESTION_STATE, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_DHD_BPM_CONGESTION_UG3_DROP_COUNTER", 1, PRIVATE_B_INDEX, &US_DHD_BPM_CONGESTION_UG3_DROP_COUNTER, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_DHD_BPM_CONGESTION_ALLOC_FAIL_DROP_COUNTER", 1, PRIVATE_B_INDEX, &US_DHD_BPM_CONGESTION_ALLOC_FAIL_DROP_COUNTER, 1, 1, 1 },
#endif
#if defined OREN
	{ "PRIVATE_B_DUMMY_STORE", 1, PRIVATE_B_INDEX, &PRIVATE_B_DUMMY_STORE, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_FAST_FREE_SKB_INDEXES_FIFO_LOCAL_TABLE_PTR", 1, PRIVATE_B_INDEX, &US_FAST_FREE_SKB_INDEXES_FIFO_LOCAL_TABLE_PTR, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_PICO_FREE_SKB_INDEXES_FIFO_LOCAL_TABLE_PTR", 1, PRIVATE_B_INDEX, &US_PICO_FREE_SKB_INDEXES_FIFO_LOCAL_TABLE_PTR, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_INGRESS_CLASSIFICATION_RULE_CFG_DESCRIPTOR", 1, PRIVATE_B_INDEX, &US_INGRESS_CLASSIFICATION_RULE_CFG_DESCRIPTOR, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_INGRESS_CLASSIFICATION_IP_FLOW_RULE_CFG_DESCRIPTOR", 1, PRIVATE_B_INDEX, &US_INGRESS_CLASSIFICATION_IP_FLOW_RULE_CFG_DESCRIPTOR, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_PACKET_BUFFER_SIZE_ASR_8", 1, PRIVATE_B_INDEX, &US_PACKET_BUFFER_SIZE_ASR_8, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_MAIN_DMA_SYNCRONIZATION_ADDRESS", 1, PRIVATE_B_INDEX, &US_MAIN_DMA_SYNCRONIZATION_ADDRESS, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_PICO_DMA_SYNCRONIZATION_ADDRESS", 1, PRIVATE_B_INDEX, &US_PICO_DMA_SYNCRONIZATION_ADDRESS, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_RUNNER_FLOW_IH_RESPONSE_MUTEX", 1, PRIVATE_B_INDEX, &US_RUNNER_FLOW_IH_RESPONSE_MUTEX, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_PARALLEL_PROCESSING_SLAVE_VECTOR", 1, PRIVATE_B_INDEX, &US_PARALLEL_PROCESSING_SLAVE_VECTOR, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_PARALLEL_PROCESSING_CONTEXT_CACHE_MODE", 1, PRIVATE_B_INDEX, &US_PARALLEL_PROCESSING_CONTEXT_CACHE_MODE, 1, 1, 1 },
#endif
#if defined OREN
	{ "WAN_TX_QUEUES_DDR_OFFLOAD_ENABLE", 1, PRIVATE_B_INDEX, &WAN_TX_QUEUES_DDR_OFFLOAD_ENABLE, 1, 1, 1 },
#endif
#if defined OREN
	{ "PON_TYPE_FLAG", 1, PRIVATE_B_INDEX, &PON_TYPE_FLAG, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_FLOODING_PACKET_COPY_SYNC_STATUS", 1, PRIVATE_B_INDEX, &US_FLOODING_PACKET_COPY_SYNC_STATUS, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_DHD_DMA_SYNCHRONIZATION", 1, PRIVATE_B_INDEX, &US_DHD_DMA_SYNCHRONIZATION, 1, 1, 1 },
#endif
#if defined OREN
	{ "BBH_TX_WAN_CHANNEL_INDEX", 1, PRIVATE_B_INDEX, &BBH_TX_WAN_CHANNEL_INDEX, 1, 1, 1 },
#endif
#if defined OREN
	{ "ETH4_RX_DESCRIPTORS", 1, PRIVATE_B_INDEX, &ETH4_RX_DESCRIPTORS, 32, 1, 1 },
#endif
#if defined OREN
	{ "MAC_TABLE", 1, COMMON_A_INDEX, &MAC_TABLE, 1024, 1, 1 },
#endif
#if defined OREN
	{ "IPTV_LOOKUP_TABLE", 1, COMMON_A_INDEX, &IPTV_LOOKUP_TABLE, 256, 1, 1 },
#endif
#if defined OREN
	{ "MAC_CONTEXT_TABLE", 1, COMMON_A_INDEX, &MAC_CONTEXT_TABLE, 1024, 1, 1 },
#endif
#if defined OREN
	{ "IPTV_CONTEXT_TABLE", 1, COMMON_A_INDEX, &IPTV_CONTEXT_TABLE, 256, 1, 1 },
#endif
#if defined OREN
	{ "SERVICE_QUEUES_DESCRIPTOR_TABLE", 1, COMMON_A_INDEX, &SERVICE_QUEUES_DESCRIPTOR_TABLE, 32, 1, 1 },
#endif
#if defined OREN
	{ "IPTV_LOOKUP_TABLE_CAM", 1, COMMON_A_INDEX, &IPTV_LOOKUP_TABLE_CAM, 32, 1, 1 },
#endif
#if defined OREN
	{ "MAC_TABLE_CAM", 1, COMMON_A_INDEX, &MAC_TABLE_CAM, 32, 1, 1 },
#endif
#if defined OREN
	{ "IPTV_CONTEXT_TABLE_CAM", 1, COMMON_A_INDEX, &IPTV_CONTEXT_TABLE_CAM, 32, 1, 1 },
#endif
#if defined OREN
	{ "INTERRUPT_COALESCING_CONFIG_TABLE", 1, COMMON_A_INDEX, &INTERRUPT_COALESCING_CONFIG_TABLE, 12, 1, 1 },
#endif
#if defined OREN
	{ "DS_DHD_TX_POST_FLOW_RING_MGMT_DESCRIPTOR_BUFFER", 1, COMMON_A_INDEX, &DS_DHD_TX_POST_FLOW_RING_MGMT_DESCRIPTOR_BUFFER, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_PBITS_TO_QOS_TABLE", 1, COMMON_A_INDEX, &DS_PBITS_TO_QOS_TABLE, 6, 8, 1 },
#endif
#if defined OREN
	{ "WAN_VID_TABLE", 1, COMMON_A_INDEX, &WAN_VID_TABLE, 4, 1, 1 },
#endif
#if defined OREN
	{ "INTERRUPT_COALESCING_TIMER_CONFIG_TABLE", 1, COMMON_A_INDEX, &INTERRUPT_COALESCING_TIMER_CONFIG_TABLE, 1, 1, 1 },
#endif
#if defined OREN
	{ "FREE_SKB_INDEXES_DDR_FIFO_TAIL", 1, COMMON_A_INDEX, &FREE_SKB_INDEXES_DDR_FIFO_TAIL, 1, 1, 1 },
#endif
#if defined OREN
	{ "DHD_RADIO_INSTANCE_COMMON_A_DATA", 1, COMMON_A_INDEX, &DHD_RADIO_INSTANCE_COMMON_A_DATA, 3, 1, 1 },
#endif
#if defined OREN
	{ "BPM_CONGESTION_CONTROL_TABLE", 1, COMMON_A_INDEX, &BPM_CONGESTION_CONTROL_TABLE, 1, 1, 1 },
#endif
#if defined OREN
	{ "DDR_ADDRESS_FOR_SKB_DATA_POINTERS_TABLE", 1, COMMON_A_INDEX, &DDR_ADDRESS_FOR_SKB_DATA_POINTERS_TABLE, 1, 1, 1 },
#endif
#if defined OREN
	{ "IPTV_L3_SRC_IP_LOOKUP_TABLE", 1, COMMON_A_INDEX, &IPTV_L3_SRC_IP_LOOKUP_TABLE, 32, 1, 1 },
#endif
#if defined OREN
	{ "WLAN_MCAST_DHD_STATION_TABLE", 1, COMMON_A_INDEX, &WLAN_MCAST_DHD_STATION_TABLE, 64, 1, 1 },
#endif
#if defined OREN
	{ "DS_CPU_REASON_TO_CPU_RX_QUEUE_TABLE", 1, COMMON_A_INDEX, &DS_CPU_REASON_TO_CPU_RX_QUEUE_TABLE, 64, 1, 1 },
#endif
#if defined OREN
	{ "MAC_CONTEXT_TABLE_CAM", 1, COMMON_A_INDEX, &MAC_CONTEXT_TABLE_CAM, 32, 1, 1 },
#endif
#if defined OREN
	{ "MAC_EXTENSION_TABLE", 1, COMMON_A_INDEX, &MAC_EXTENSION_TABLE, 1024, 1, 1 },
#endif
#if defined OREN
	{ "DS_DHD_FLOW_RING_CACHE_LKP_TABLE", 1, COMMON_A_INDEX, &DS_DHD_FLOW_RING_CACHE_LKP_TABLE, 16, 1, 1 },
#endif
#if defined OREN
	{ "MAC_UNKNOWN_DA_FORWARDING_TABLE", 1, COMMON_A_INDEX, &MAC_UNKNOWN_DA_FORWARDING_TABLE, 1, 1, 1 },
#endif
#if defined OREN
	{ "DHD_TX_POST_BUFFERS_THRESHOLD", 1, COMMON_A_INDEX, &DHD_TX_POST_BUFFERS_THRESHOLD, 1, 1, 1 },
#endif
#if defined OREN
	{ "DHD_TX_POST_BUFFERS_IN_USE_COUNTER", 1, COMMON_A_INDEX, &DHD_TX_POST_BUFFERS_IN_USE_COUNTER, 1, 1, 1 },
#endif
#if defined OREN
	{ "COMMON_A_DUMMY_STORE", 1, COMMON_A_INDEX, &COMMON_A_DUMMY_STORE, 1, 1, 1 },
#endif
#if defined OREN
	{ "DDR_ADDRESS_FOR_FREE_SKB_INDEXES_FIFO_TABLE", 1, COMMON_A_INDEX, &DDR_ADDRESS_FOR_FREE_SKB_INDEXES_FIFO_TABLE, 1, 1, 1 },
#endif
#if defined OREN
	{ "DDR_ADDRESS_FOR_FREE_SKB_INDEXES_FIFO_TABLE_LAST_ENTRY", 1, COMMON_A_INDEX, &DDR_ADDRESS_FOR_FREE_SKB_INDEXES_FIFO_TABLE_LAST_ENTRY, 1, 1, 1 },
#endif
#if defined OREN
	{ "SCT_FILTER", 1, COMMON_A_INDEX, &SCT_FILTER, 1, 1, 1 },
#endif
#if defined OREN
	{ "BPM_REPLY_RUNNER_A", 1, COMMON_A_INDEX, &BPM_REPLY_RUNNER_A, 1, 1, 1 },
#endif
#if defined OREN
	{ "GLOBAL_DSCP_TO_PBITS_TABLE", 1, COMMON_A_INDEX, &GLOBAL_DSCP_TO_PBITS_TABLE, 1, 64, 1 },
#endif
#if defined OREN
	{ "GLOBAL_DSCP_TO_PBITS_DEI_TABLE", 1, COMMON_A_INDEX, &GLOBAL_DSCP_TO_PBITS_DEI_TABLE, 1, 64, 1 },
#endif
#if defined OREN
	{ "GPON_SKB_ENQUEUED_INDEXES_FIFO", 1, COMMON_A_INDEX, &GPON_SKB_ENQUEUED_INDEXES_FIFO, 32, 1, 1 },
#endif
#if defined OREN
	{ "VLAN_COMMAND_INDEX_TABLE", 1, COMMON_A_INDEX, &VLAN_COMMAND_INDEX_TABLE, 256, 1, 1 },
#endif
#if defined OREN
	{ "WLAN_MCAST_SSID_MAC_ADDRESS_TABLE", 1, COMMON_A_INDEX, &WLAN_MCAST_SSID_MAC_ADDRESS_TABLE, 48, 1, 1 },
#endif
#if defined OREN
	{ "DHD_FLOW_RING_SHADOW_WR_PTR_TABLE", 1, COMMON_A_INDEX, &DHD_FLOW_RING_SHADOW_WR_PTR_TABLE, 16, 1, 1 },
#endif
#if defined OREN
	{ "SERVICE_QUEUES_CFG", 1, COMMON_A_INDEX, &SERVICE_QUEUES_CFG, 1, 1, 1 },
#endif
#if defined OREN
	{ "MAC_EXTENSION_TABLE_CAM", 1, COMMON_A_INDEX, &MAC_EXTENSION_TABLE_CAM, 32, 1, 1 },
#endif
#if defined OREN
	{ "PM_COUNTERS_BUFFER", 1, COMMON_A_INDEX, &PM_COUNTERS_BUFFER, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_INGRESS_CLASSIFICATION_LOOKUP_LONG_TABLE", 1, COMMON_A_INDEX, &DS_INGRESS_CLASSIFICATION_LOOKUP_LONG_TABLE, 128, 1, 1 },
#endif
#if defined OREN
	{ "PM_COUNTERS", 1, COMMON_A_INDEX, &PM_COUNTERS, 1, 1, 1 },
#endif
#if defined OREN
	{ "WAN_DDR_QUEUE_ADDRESS_TABLE", 1, COMMON_A_INDEX, &WAN_DDR_QUEUE_ADDRESS_TABLE, 128, 1, 1 },
#endif
#if defined OREN
	{ "LAN_VID_CONTEXT_TABLE", 1, COMMON_A_INDEX, &LAN_VID_CONTEXT_TABLE, 128, 1, 1 },
#endif
#if defined OREN
	{ "RING_DESCRIPTORS_TABLE", 1, COMMON_A_INDEX, &RING_DESCRIPTORS_TABLE, 12, 1, 1 },
#endif
#if defined OREN
	{ "WLAN_MCAST_SSID_STATS_TABLE", 1, COMMON_A_INDEX, &WLAN_MCAST_SSID_STATS_TABLE, 48, 1, 1 },
#endif
#if defined OREN
	{ "RUNNER_A_SCRATCHPAD", 1, COMMON_A_INDEX, &RUNNER_A_SCRATCHPAD, 1, 1, 1 },
#endif
#if defined OREN
	{ "CPU_RX_RUNNER_A_SCRATCHPAD", 1, COMMON_A_INDEX, &CPU_RX_RUNNER_A_SCRATCHPAD, 1, 1, 1 },
#endif
#if defined OREN
	{ "WLAN_MCAST_RUNNER_A_SCRATCHPAD", 1, COMMON_A_INDEX, &WLAN_MCAST_RUNNER_A_SCRATCHPAD, 1, 1, 1 },
#endif
#if defined OREN
	{ "SERVICE_QUEUES_DDR_QUEUE_ADDRESS_TABLE", 1, COMMON_A_INDEX, &SERVICE_QUEUES_DDR_QUEUE_ADDRESS_TABLE, 32, 1, 1 },
#endif
#if defined OREN
	{ "DS_RING_PACKET_DESCRIPTORS_CACHE", 1, COMMON_A_INDEX, &DS_RING_PACKET_DESCRIPTORS_CACHE, 12, 1, 1 },
#endif
#if defined OREN
	{ "CONNECTION_BUFFER_TABLE", 1, COMMON_A_INDEX, &CONNECTION_BUFFER_TABLE, 5, 4, 1 },
#endif
#if defined OREN
	{ "LOCAL_SWITCHING_MULTICAST_LAN_ENQUEUE_INGRESS_QUEUE", 1, COMMON_A_INDEX, &LOCAL_SWITCHING_MULTICAST_LAN_ENQUEUE_INGRESS_QUEUE, 16, 1, 1 },
#endif
#if defined OREN
	{ "US_INGRESS_CLASSIFICATION_LOOKUP_LONG_TABLE", 1, COMMON_B_INDEX, &US_INGRESS_CLASSIFICATION_LOOKUP_LONG_TABLE, 128, 1, 1 },
#endif
#if defined OREN
	{ "WAN_EXT_DDR_CACHE_FIFO", 1, COMMON_B_INDEX, &WAN_EXT_DDR_CACHE_FIFO, 256, 1, 1 },
#endif
#if defined OREN
	{ "LAYER2_GRE_TUNNEL_TABLE", 1, COMMON_B_INDEX, &LAYER2_GRE_TUNNEL_TABLE, 12, 1, 1 },
#endif
#if defined OREN
	{ "EPON_DDR_QUEUE_ADDRESS_TABLE", 1, COMMON_B_INDEX, &EPON_DDR_QUEUE_ADDRESS_TABLE, 16, 1, 1 },
#endif
#if defined OREN
	{ "DUAL_STACK_LITE_TABLE", 1, COMMON_B_INDEX, &DUAL_STACK_LITE_TABLE, 4, 1, 1 },
#endif
#if defined OREN
	{ "DS_INGRESS_CLASSIFICATION_LOOKUP_CAM_TABLE", 1, COMMON_B_INDEX, &DS_INGRESS_CLASSIFICATION_LOOKUP_CAM_TABLE, 32, 1, 1 },
#endif
#if defined OREN
	{ "EPON_DDR_CACHE_FIFO", 1, COMMON_B_INDEX, &EPON_DDR_CACHE_FIFO, 192, 1, 1 },
#endif
#if defined OREN
	{ "IP_SYNC_1588_DESCRIPTOR_QUEUE", 1, COMMON_B_INDEX, &IP_SYNC_1588_DESCRIPTOR_QUEUE, 16, 1, 1 },
#endif
#if defined OREN
	{ "RUNNER_B_SCRATCHPAD", 1, COMMON_B_INDEX, &RUNNER_B_SCRATCHPAD, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_INGRESS_CLASSIFICATION_LOOKUP_CAM_TABLE", 1, COMMON_B_INDEX, &US_INGRESS_CLASSIFICATION_LOOKUP_CAM_TABLE, 32, 1, 1 },
#endif
#if defined OREN
	{ "CPU_RX_RUNNER_B_SCRATCHPAD", 1, COMMON_B_INDEX, &CPU_RX_RUNNER_B_SCRATCHPAD, 1, 1, 1 },
#endif
#if defined OREN
	{ "DHD_FLOW_RING_CACHE_CTX_TABLE", 1, COMMON_B_INDEX, &DHD_FLOW_RING_CACHE_CTX_TABLE, 16, 1, 1 },
#endif
#if defined OREN
	{ "US_RING_PACKET_DESCRIPTORS_CACHE", 1, COMMON_B_INDEX, &US_RING_PACKET_DESCRIPTORS_CACHE, 12, 1, 1 },
#endif
#if defined OREN
	{ "DUMMY_RATE_CONTROLLER_DESCRIPTOR", 1, COMMON_B_INDEX, &DUMMY_RATE_CONTROLLER_DESCRIPTOR, 1, 1, 1 },
#endif
#if defined OREN
	{ "DS_INGRESS_CLASSIFICATION_LOOKUP_TABLE", 1, COMMON_B_INDEX, &DS_INGRESS_CLASSIFICATION_LOOKUP_TABLE, 256, 1, 1 },
#endif
#if defined OREN
	{ "US_INGRESS_CLASSIFICATION_LOOKUP_TABLE", 1, COMMON_B_INDEX, &US_INGRESS_CLASSIFICATION_LOOKUP_TABLE, 256, 1, 1 },
#endif
#if defined OREN
	{ "WAN_TX_QUEUES_TABLE", 1, COMMON_B_INDEX, &WAN_TX_QUEUES_TABLE, 256, 1, 1 },
#endif
#if defined OREN
	{ "DHD_RADIO_INSTANCE_COMMON_B_DATA", 1, COMMON_B_INDEX, &DHD_RADIO_INSTANCE_COMMON_B_DATA, 3, 1, 1 },
#endif
#if defined OREN
	{ "EPON_DDR_QUEUE_DESCRIPTORS_TABLE", 1, COMMON_B_INDEX, &EPON_DDR_QUEUE_DESCRIPTORS_TABLE, 16, 1, 1 },
#endif
#if defined OREN
	{ "LOCAL_SWITCHING_PBITS_TO_QOS_TABLE", 1, COMMON_B_INDEX, &LOCAL_SWITCHING_PBITS_TO_QOS_TABLE, 6, 8, 1 },
#endif
#if defined OREN
	{ "DUMMY_WAN_TX_QUEUE_DESCRIPTOR", 1, COMMON_B_INDEX, &DUMMY_WAN_TX_QUEUE_DESCRIPTOR, 1, 1, 1 },
#endif
#if defined OREN
	{ "LOCAL_SWITCHING_LAN_ENQUEUE_INGRESS_QUEUE", 1, COMMON_B_INDEX, &LOCAL_SWITCHING_LAN_ENQUEUE_INGRESS_QUEUE, 16, 1, 1 },
#endif
#if defined OREN
	{ "SRC_MAC_ANTI_SPOOFING_TABLE", 1, COMMON_B_INDEX, &SRC_MAC_ANTI_SPOOFING_TABLE, 6, 4, 1 },
#endif
#if defined OREN
	{ "WIFI_SSID_FORWARDING_MATRIX_TABLE", 1, COMMON_B_INDEX, &WIFI_SSID_FORWARDING_MATRIX_TABLE, 16, 1, 1 },
#endif
#if defined OREN
	{ "GPON_SKB_ENQUEUED_INDEXES_FREE_PTR", 1, COMMON_B_INDEX, &GPON_SKB_ENQUEUED_INDEXES_FREE_PTR, 40, 1, 1 },
#endif
#if defined OREN
	{ "US_DHD_TX_POST_FLOW_RING_MGMT_DESCRIPTOR_BUFFER", 1, COMMON_B_INDEX, &US_DHD_TX_POST_FLOW_RING_MGMT_DESCRIPTOR_BUFFER, 1, 1, 1 },
#endif
#if defined OREN
	{ "SPEED_SERVICE_STREAM_PREFIX", 1, COMMON_B_INDEX, &SPEED_SERVICE_STREAM_PREFIX, 1, 1, 1 },
#endif
#if defined OREN
	{ "BRIDGE_PORT_TO_BROADCOM_SWITCH_PORT_MAPPING_TABLE", 1, COMMON_B_INDEX, &BRIDGE_PORT_TO_BROADCOM_SWITCH_PORT_MAPPING_TABLE, 8, 1, 1 },
#endif
#if defined OREN
	{ "SPEED_SERVICE_RX_TIMESTAMPS_TABLE", 1, COMMON_B_INDEX, &SPEED_SERVICE_RX_TIMESTAMPS_TABLE, 1, 1, 1 },
#endif
#if defined OREN
	{ "GPON_SKB_ENQUEUED_INDEXES_PUT_PTR", 1, COMMON_B_INDEX, &GPON_SKB_ENQUEUED_INDEXES_PUT_PTR, 40, 1, 1 },
#endif
#if defined OREN
	{ "BRIDGE_PORT_TO_BBH_PERIPHERAL_RX_TABLE", 1, COMMON_B_INDEX, &BRIDGE_PORT_TO_BBH_PERIPHERAL_RX_TABLE, 7, 1, 1 },
#endif
#if defined OREN
	{ "COMMON_B_DUMMY_STORE", 1, COMMON_B_INDEX, &COMMON_B_DUMMY_STORE, 1, 1, 1 },
#endif
#if defined OREN
	{ "EGRESS_VLAN_SWITCHING_ISOLATION_CONFIG_TABLE", 1, COMMON_B_INDEX, &EGRESS_VLAN_SWITCHING_ISOLATION_CONFIG_TABLE, 5, 1, 1 },
#endif
#if defined OREN
	{ "DHD_FLOW_RING_CACHE_CTX_NEXT_INDEX", 1, COMMON_B_INDEX, &DHD_FLOW_RING_CACHE_CTX_NEXT_INDEX, 1, 1, 1 },
#endif
#if defined OREN
	{ "BRIDGE_PORT_TO_BROADCOM_SWITCH_PORT_MAPPING_TABLE_PTR", 1, COMMON_B_INDEX, &BRIDGE_PORT_TO_BROADCOM_SWITCH_PORT_MAPPING_TABLE_PTR, 1, 1, 1 },
#endif
#if defined OREN
	{ "CPU_TX_DESCRIPTOR_QUEUE_TAIL_TABLE", 1, COMMON_B_INDEX, &CPU_TX_DESCRIPTOR_QUEUE_TAIL_TABLE, 4, 1, 1 },
#endif
#if defined OREN
	{ "CPU_TX_PICO_US_FLOODING_QUEUE_PTR", 1, COMMON_B_INDEX, &CPU_TX_PICO_US_FLOODING_QUEUE_PTR, 1, 1, 1 },
#endif
#if defined OREN
	{ "WAN_PHYSICAL_PORT", 1, COMMON_B_INDEX, &WAN_PHYSICAL_PORT, 1, 1, 1 },
#endif
#if defined OREN
	{ "RATE_CONTROLLER_EXPONENT_TABLE", 1, COMMON_B_INDEX, &RATE_CONTROLLER_EXPONENT_TABLE, 4, 1, 1 },
#endif
#if defined OREN
	{ "PACKET_SRAM_TO_DDR_COPY_BUFFER_1", 1, COMMON_B_INDEX, &PACKET_SRAM_TO_DDR_COPY_BUFFER_1, 1, 1, 1 },
#endif
#if defined OREN
	{ "PACKET_SRAM_TO_DDR_COPY_BUFFER_2", 1, COMMON_B_INDEX, &PACKET_SRAM_TO_DDR_COPY_BUFFER_2, 1, 1, 1 },
#endif
#if defined OREN
	{ "LAN0_INGRESS_FIFO", 1, COMMON_B_INDEX, &LAN0_INGRESS_FIFO, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_CPU_REASON_TO_CPU_RX_QUEUE_TABLE", 1, COMMON_B_INDEX, &US_CPU_REASON_TO_CPU_RX_QUEUE_TABLE, 64, 1, 1 },
#endif
#if defined OREN
	{ "LAN1_INGRESS_FIFO", 1, COMMON_B_INDEX, &LAN1_INGRESS_FIFO, 1, 1, 1 },
#endif
#if defined OREN
	{ "MULTICAST_ACTIVE_PORTS_TABLE", 1, COMMON_B_INDEX, &MULTICAST_ACTIVE_PORTS_TABLE, 64, 1, 1 },
#endif
#if defined OREN
	{ "LAN2_INGRESS_FIFO", 1, COMMON_B_INDEX, &LAN2_INGRESS_FIFO, 1, 1, 1 },
#endif
#if defined OREN
	{ "CPU_TX_EMAC_LOOPBACK_QUEUE", 1, COMMON_B_INDEX, &CPU_TX_EMAC_LOOPBACK_QUEUE, 4, 1, 1 },
#endif
#if defined OREN
	{ "LAN3_INGRESS_FIFO", 1, COMMON_B_INDEX, &LAN3_INGRESS_FIFO, 1, 1, 1 },
#endif
#if defined OREN
	{ "CPU_TX_US_FLOODING_QUEUE", 1, COMMON_B_INDEX, &CPU_TX_US_FLOODING_QUEUE, 4, 1, 1 },
#endif
#if defined OREN
	{ "LAN4_INGRESS_FIFO", 1, COMMON_B_INDEX, &LAN4_INGRESS_FIFO, 1, 1, 1 },
#endif
#if defined OREN
	{ "US_DHD_FLOW_RING_CACHE_LKP_TABLE", 1, COMMON_B_INDEX, &US_DHD_FLOW_RING_CACHE_LKP_TABLE, 16, 1, 1 },
#endif
#if defined OREN
	{ "WAN_ENQUEUE_INGRESS_QUEUE", 1, COMMON_B_INDEX, &WAN_ENQUEUE_INGRESS_QUEUE, 64, 1, 1 },
#endif
#if defined OREN
	{ "BPM_REPLY_RUNNER_B", 1, COMMON_B_INDEX, &BPM_REPLY_RUNNER_B, 1, 1, 1 },
#endif
#if defined OREN
	{ "WAN_DDR_CACHE_FIFO", 1, COMMON_B_INDEX, &WAN_DDR_CACHE_FIFO, 1024, 1, 1 },
#endif
#if defined OREN
	{ "US_RATE_CONTROLLERS_TABLE", 1, COMMON_B_INDEX, &US_RATE_CONTROLLERS_TABLE, 128, 1, 1 },
#endif
#if defined OREN
	{ "DS_CONNECTION_TABLE", 1, DDR_INDEX, &DS_CONNECTION_TABLE, 32768, 1, 1 },
#endif
#if defined OREN
	{ "US_CONNECTION_TABLE", 1, DDR_INDEX, &US_CONNECTION_TABLE, 32768, 1, 1 },
#endif
#if defined OREN
	{ "CONTEXT_TABLE", 1, DDR_INDEX, &CONTEXT_TABLE, 16512, 1, 1 },
#endif
#if defined OREN
	{ "FIREWALL_RULES_TABLE", 1, DDR_INDEX, &FIREWALL_RULES_TABLE, 256, 1, 1 },
#endif
#if defined OREN
	{ "DDR_ADDRESS_FOR_SYNC_DMA", 1, DDR_INDEX, &DDR_ADDRESS_FOR_SYNC_DMA, 1, 1, 1 },
#endif
#if defined OREN
	{ "EPON_TX_POST_SCHEDULING_DDR_QUEUES", 1, DDR_INDEX, &EPON_TX_POST_SCHEDULING_DDR_QUEUES, 65536, 1, 1 },
#endif
#if defined OREN
	{ "IPTV_DDR_CONTEXT_TABLE", 1, DDR_INDEX, &IPTV_DDR_CONTEXT_TABLE, 8192, 1, 1 },
#endif
#if defined OREN
	{ "IPTV_DDR_LOOKUP_TABLE", 1, DDR_INDEX, &IPTV_DDR_LOOKUP_TABLE, 8192, 1, 1 },
#endif
#if defined OREN
	{ "IPTV_SSM_DDR_CONTEXT_TABLE", 1, DDR_INDEX, &IPTV_SSM_DDR_CONTEXT_TABLE, 32768, 1, 1 },
#endif
#if defined OREN
	{ "FIREWALL_RULES_MAP_TABLE", 1, DDR_INDEX, &FIREWALL_RULES_MAP_TABLE, 8, 2, 65536 },
#endif
#if defined OREN
	{ "WLAN_MCAST_DHD_LIST_TABLE", 1, DDR_INDEX, &WLAN_MCAST_DHD_LIST_TABLE, 64, 1, 1 },
#endif
#if defined OREN
	{ "WLAN_MCAST_DHD_LIST_FORMAT_TABLE", 1, DDR_INDEX, &WLAN_MCAST_DHD_LIST_FORMAT_TABLE, 1, 1, 1 },
#endif
#if defined OREN
	{ "DHD_RX_POST_DDR_BUFFER", 1, DDR_INDEX, &DHD_RX_POST_DDR_BUFFER, 1024, 1, 1 },
#endif
#if defined OREN
	{ "DHD_RX_COMPLETE_DDR_BUFFER", 1, DDR_INDEX, &DHD_RX_COMPLETE_DDR_BUFFER, 1024, 1, 1 },
#endif
#if defined OREN
	{ "R2D_WR_ARR_DDR_BUFFER", 1, DDR_INDEX, &R2D_WR_ARR_DDR_BUFFER, 128, 1, 1 },
#endif
#if defined OREN
	{ "D2R_RD_ARR_DDR_BUFFER", 1, DDR_INDEX, &D2R_RD_ARR_DDR_BUFFER, 128, 1, 1 },
#endif
#if defined OREN
	{ "R2D_RD_ARR_DDR_BUFFER", 1, DDR_INDEX, &R2D_RD_ARR_DDR_BUFFER, 128, 1, 1 },
#endif
#if defined OREN
	{ "D2R_WR_ARR_DDR_BUFFER", 1, DDR_INDEX, &D2R_WR_ARR_DDR_BUFFER, 128, 1, 1 },
#endif
#if defined OREN
	{ "DHD_TX_POST_FLOW_RING_MGMT_DESCRIPTOR_TABLE", 1, DDR_INDEX, &DHD_TX_POST_FLOW_RING_MGMT_DESCRIPTOR_TABLE, 8, 1, 1 },
#endif
#if defined OREN
	{ "DHD_TX_POST_DDR_BUFFER", 1, DDR_INDEX, &DHD_TX_POST_DDR_BUFFER, 8, 16, 1 },
#endif
#if defined OREN
	{ "DHD_TX_COMPLETE_DDR_BUFFER", 1, DDR_INDEX, &DHD_TX_COMPLETE_DDR_BUFFER, 16, 1, 1 },
#endif
#if defined OREN
	{ "DS_INGRESS_CLASSIFICATION_COUNTERS_TABLE", 1, PSRAM_INDEX, &DS_INGRESS_CLASSIFICATION_COUNTERS_TABLE, 256, 1, 1 },
#endif
#if defined OREN
	{ "US_INGRESS_CLASSIFICATION_COUNTERS_TABLE", 1, PSRAM_INDEX, &US_INGRESS_CLASSIFICATION_COUNTERS_TABLE, 256, 1, 1 },
#endif
#if defined G9991
	{ "INGRESS_HANDLER_BUFFER", 1, PRIVATE_A_INDEX, &INGRESS_HANDLER_BUFFER, 32, 1, 1 },
#endif
#if defined G9991
	{ "G9991_DDR_CACHE_FIFO", 1, PRIVATE_A_INDEX, &G9991_DDR_CACHE_FIFO, 1440, 1, 1 },
#endif
#if defined G9991
	{ "VLAN_COMMAND_INDEX_TABLE", 1, PRIVATE_A_INDEX, &VLAN_COMMAND_INDEX_TABLE, 256, 1, 1 },
#endif
#if defined G9991
	{ "DS_PBITS_TO_PBITS_TABLE", 1, PRIVATE_A_INDEX, &DS_PBITS_TO_PBITS_TABLE, 32, 8, 1 },
#endif
#if defined G9991
	{ "DS_WAN_FLOW_TABLE", 1, PRIVATE_A_INDEX, &DS_WAN_FLOW_TABLE, 256, 1, 1 },
#endif
#if defined G9991
	{ "DS_INGRESS_CLASSIFICATION_CONTEXT_TABLE", 1, PRIVATE_A_INDEX, &DS_INGRESS_CLASSIFICATION_CONTEXT_TABLE, 256, 1, 1 },
#endif
#if defined G9991
	{ "DS_INGRESS_FILTERS_LOOKUP_TABLE", 1, PRIVATE_A_INDEX, &DS_INGRESS_FILTERS_LOOKUP_TABLE, 10, 32, 1 },
#endif
#if defined G9991
	{ "ETH_TX_RS_QUEUE_DESCRIPTOR_TABLE", 1, PRIVATE_A_INDEX, &ETH_TX_RS_QUEUE_DESCRIPTOR_TABLE, 128, 1, 1 },
#endif
#if defined G9991
	{ "ETH_TX_QUEUES_TABLE", 1, PRIVATE_A_INDEX, &ETH_TX_QUEUES_TABLE, 120, 1, 1 },
#endif
#if defined G9991
	{ "DS_QUEUE_PROFILE_TABLE", 1, PRIVATE_A_INDEX, &DS_QUEUE_PROFILE_TABLE, 8, 1, 1 },
#endif
#if defined G9991
	{ "DS_PBITS_PARAMETER_TABLE", 1, PRIVATE_A_INDEX, &DS_PBITS_PARAMETER_TABLE, 128, 1, 1 },
#endif
#if defined G9991
	{ "IPTV_SSID_EXTENSION_TABLE", 1, PRIVATE_A_INDEX, &IPTV_SSID_EXTENSION_TABLE, 256, 1, 1 },
#endif
#if defined G9991
	{ "DS_CPU_REASON_TO_METER_TABLE", 1, PRIVATE_A_INDEX, &DS_CPU_REASON_TO_METER_TABLE, 64, 1, 1 },
#endif
#if defined G9991
	{ "IPTV_INGRESS_CLASSIFICATION_CONTEXT_TABLE", 1, PRIVATE_A_INDEX, &IPTV_INGRESS_CLASSIFICATION_CONTEXT_TABLE, 288, 1, 1 },
#endif
#if defined G9991
	{ "DS_MAIN_TIMER_TASK_DESCRIPTOR_TABLE", 1, PRIVATE_A_INDEX, &DS_MAIN_TIMER_TASK_DESCRIPTOR_TABLE, 4, 1, 1 },
#endif
#if defined G9991
	{ "SBPM_REPLY", 1, PRIVATE_A_INDEX, &SBPM_REPLY, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_VLAN_PARAMETER_TABLE", 1, PRIVATE_A_INDEX, &DS_VLAN_PARAMETER_TABLE, 128, 1, 1 },
#endif
#if defined G9991
	{ "IPTV_COUNTERS_TABLE", 1, PRIVATE_A_INDEX, &IPTV_COUNTERS_TABLE, 288, 1, 1 },
#endif
#if defined G9991
	{ "RATE_LIMITER_REMAINDER_TABLE", 1, PRIVATE_A_INDEX, &RATE_LIMITER_REMAINDER_TABLE, 32, 1, 1 },
#endif
#if defined G9991
	{ "DS_LAYER4_FILTERS_LOOKUP_TABLE", 1, PRIVATE_A_INDEX, &DS_LAYER4_FILTERS_LOOKUP_TABLE, 16, 1, 1 },
#endif
#if defined G9991
	{ "DS_LAN_VID_TABLE", 1, PRIVATE_A_INDEX, &DS_LAN_VID_TABLE, 128, 1, 1 },
#endif
#if defined G9991
	{ "ETH_TX_QUEUES_POINTERS_TABLE", 1, PRIVATE_A_INDEX, &ETH_TX_QUEUES_POINTERS_TABLE, 120, 1, 1 },
#endif
#if defined G9991
	{ "DS_PICO_TIMER_TASK_DESCRIPTOR_TABLE", 1, PRIVATE_A_INDEX, &DS_PICO_TIMER_TASK_DESCRIPTOR_TABLE, 4, 1, 1 },
#endif
#if defined G9991
	{ "DS_PBITS_PRIMITIVE_TABLE", 1, PRIVATE_A_INDEX, &DS_PBITS_PRIMITIVE_TABLE, 32, 1, 1 },
#endif
#if defined G9991
	{ "DS_FAST_CPU_TX_DESCRIPTOR_ABS_DATA_PTR_QUEUE", 1, PRIVATE_A_INDEX, &DS_FAST_CPU_TX_DESCRIPTOR_ABS_DATA_PTR_QUEUE, 16, 1, 1 },
#endif
#if defined G9991
	{ "DS_PICO_CPU_TX_DESCRIPTOR_ABS_DATA_PTR_QUEUE", 1, PRIVATE_A_INDEX, &DS_PICO_CPU_TX_DESCRIPTOR_ABS_DATA_PTR_QUEUE, 16, 1, 1 },
#endif
#if defined G9991
	{ "DS_INGRESS_CLASSIFICATION_KEY_PRIMITIVE_TABLE", 1, PRIVATE_A_INDEX, &DS_INGRESS_CLASSIFICATION_KEY_PRIMITIVE_TABLE, 32, 1, 1 },
#endif
#if defined G9991
	{ "DS_RATE_LIMITER_TABLE", 1, PRIVATE_A_INDEX, &DS_RATE_LIMITER_TABLE, 32, 1, 1 },
#endif
#if defined G9991
	{ "DS_VLAN_COMMANDS_TABLE", 1, PRIVATE_A_INDEX, &DS_VLAN_COMMANDS_TABLE, 64, 1, 1 },
#endif
#if defined G9991
	{ "DS_POLICER_TABLE", 1, PRIVATE_A_INDEX, &DS_POLICER_TABLE, 16, 1, 1 },
#endif
#if defined G9991
	{ "ETH_TX_LOCAL_REGISTERS", 1, PRIVATE_A_INDEX, &ETH_TX_LOCAL_REGISTERS, 30, 1, 1 },
#endif
#if defined G9991
	{ "DS_TPID_OVERWRITE_TABLE", 1, PRIVATE_A_INDEX, &DS_TPID_OVERWRITE_TABLE, 8, 1, 1 },
#endif
#if defined G9991
	{ "DS_CPU_TX_BBH_DESCRIPTORS", 1, PRIVATE_A_INDEX, &DS_CPU_TX_BBH_DESCRIPTORS, 32, 1, 1 },
#endif
#if defined G9991
	{ "DS_INGRESS_FILTERS_PARAMETER_TABLE", 1, PRIVATE_A_INDEX, &DS_INGRESS_FILTERS_PARAMETER_TABLE, 10, 16, 1 },
#endif
#if defined G9991
	{ "DS_INGRESS_CLASSIFICATION_GENERIC_RULE_CFG_TABLE", 1, PRIVATE_A_INDEX, &DS_INGRESS_CLASSIFICATION_GENERIC_RULE_CFG_TABLE, 4, 1, 1 },
#endif
#if defined G9991
	{ "IPTV_SSID_EXTENSION_TABLE_CAM", 1, PRIVATE_A_INDEX, &IPTV_SSID_EXTENSION_TABLE_CAM, 1, 1, 1 },
#endif
#if defined G9991
	{ "CPU_RX_PD_INGRESS_QUEUE", 1, PRIVATE_A_INDEX, &CPU_RX_PD_INGRESS_QUEUE, 32, 1, 1 },
#endif
#if defined G9991
	{ "WLAN_MCAST_CONTROL_TABLE", 1, PRIVATE_A_INDEX, &WLAN_MCAST_CONTROL_TABLE, 1, 1, 1 },
#endif
#if defined G9991
	{ "ETH_TX_EMACS_STATUS", 1, PRIVATE_A_INDEX, &ETH_TX_EMACS_STATUS, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_CPU_PARAMETERS_BLOCK", 1, PRIVATE_A_INDEX, &DS_CPU_PARAMETERS_BLOCK, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_FAST_RUNNER_GLOBAL_REGISTERS_INIT", 1, PRIVATE_A_INDEX, &DS_FAST_RUNNER_GLOBAL_REGISTERS_INIT, 8, 1, 1 },
#endif
#if defined G9991
	{ "DS_FLOW_BASED_ACTION_PTR_TABLE", 1, PRIVATE_A_INDEX, &DS_FLOW_BASED_ACTION_PTR_TABLE, 32, 1, 1 },
#endif
#if defined G9991
	{ "CPU_RX_FAST_PD_INGRESS_QUEUE", 1, PRIVATE_A_INDEX, &CPU_RX_FAST_PD_INGRESS_QUEUE, 32, 1, 1 },
#endif
#if defined G9991
	{ "DS_FORWARDING_MATRIX_TABLE", 1, PRIVATE_A_INDEX, &DS_FORWARDING_MATRIX_TABLE, 9, 16, 1 },
#endif
#if defined G9991
	{ "DS_TIMER_SCHEDULER_PRIMITIVE_TABLE", 1, PRIVATE_A_INDEX, &DS_TIMER_SCHEDULER_PRIMITIVE_TABLE, 8, 1, 1 },
#endif
#if defined G9991
	{ "DS_PICO_RUNNER_GLOBAL_REGISTERS_INIT", 1, PRIVATE_A_INDEX, &DS_PICO_RUNNER_GLOBAL_REGISTERS_INIT, 8, 1, 1 },
#endif
#if defined G9991
	{ "DS_TRAFFIC_CLASS_TO_QUEUE_TABLE", 1, PRIVATE_A_INDEX, &DS_TRAFFIC_CLASS_TO_QUEUE_TABLE, 6, 8, 1 },
#endif
#if defined G9991
	{ "EMAC_SKB_ENQUEUED_INDEXES_PUT_PTR", 1, PRIVATE_A_INDEX, &EMAC_SKB_ENQUEUED_INDEXES_PUT_PTR, 8, 1, 1 },
#endif
#if defined G9991
	{ "DS_BRIDGE_CONFIGURATION_REGISTER", 1, PRIVATE_A_INDEX, &DS_BRIDGE_CONFIGURATION_REGISTER, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_CPU_RX_METER_TABLE", 1, PRIVATE_A_INDEX, &DS_CPU_RX_METER_TABLE, 16, 1, 1 },
#endif
#if defined G9991
	{ "INGRESS_HANDLER_SKB_DATA_POINTER", 1, PRIVATE_A_INDEX, &INGRESS_HANDLER_SKB_DATA_POINTER, 32, 1, 1 },
#endif
#if defined G9991
	{ "ETH_TX_MAC_TABLE", 1, PRIVATE_A_INDEX, &ETH_TX_MAC_TABLE, 30, 1, 1 },
#endif
#if defined G9991
	{ "DS_INGRESS_CLASSIFICATION_RULE_CFG_TABLE", 1, PRIVATE_A_INDEX, &DS_INGRESS_CLASSIFICATION_RULE_CFG_TABLE, 16, 1, 1 },
#endif
#if defined G9991
	{ "DS_PROFILING_BUFFER_PICO_RUNNER", 1, PRIVATE_A_INDEX, &DS_PROFILING_BUFFER_PICO_RUNNER, 1, 1, 1 },
#endif
#if defined G9991
	{ "DOWNSTREAM_MULTICAST_LAN_ENQUEUE_INGRESS_QUEUE", 1, PRIVATE_A_INDEX, &DOWNSTREAM_MULTICAST_LAN_ENQUEUE_INGRESS_QUEUE, 16, 1, 1 },
#endif
#if defined G9991
	{ "DS_VLAN_OPTIMIZATION_TABLE", 1, PRIVATE_A_INDEX, &DS_VLAN_OPTIMIZATION_TABLE, 128, 1, 1 },
#endif
#if defined G9991
	{ "DOWNSTREAM_MULTICAST_LAN_ENQUEUE_SERVICE_QUEUE", 1, PRIVATE_A_INDEX, &DOWNSTREAM_MULTICAST_LAN_ENQUEUE_SERVICE_QUEUE, 16, 1, 1 },
#endif
#if defined G9991
	{ "PCI_MULTICAST_SCRATCHPAD", 1, PRIVATE_A_INDEX, &PCI_MULTICAST_SCRATCHPAD, 1, 1, 1 },
#endif
#if defined G9991
	{ "CPU_RX_MIRRORING_PD_INGRESS_QUEUE", 1, PRIVATE_A_INDEX, &CPU_RX_MIRRORING_PD_INGRESS_QUEUE, 16, 1, 1 },
#endif
#if defined G9991
	{ "DS_RUNNER_FLOW_HEADER_BUFFER", 1, PRIVATE_A_INDEX, &DS_RUNNER_FLOW_HEADER_BUFFER, 1, 1, 1 },
#endif
#if defined G9991
	{ "CPU_TX_FAST_QUEUE", 1, PRIVATE_A_INDEX, &CPU_TX_FAST_QUEUE, 16, 1, 1 },
#endif
#if defined G9991
	{ "DOWNSTREAM_LAN_ENQUEUE_INGRESS_QUEUE", 1, PRIVATE_A_INDEX, &DOWNSTREAM_LAN_ENQUEUE_INGRESS_QUEUE, 64, 1, 1 },
#endif
#if defined G9991
	{ "DHD_TX_COMPLETE_FLOW_RING_BUFFER", 1, PRIVATE_A_INDEX, &DHD_TX_COMPLETE_FLOW_RING_BUFFER, 3, 1, 1 },
#endif
#if defined G9991
	{ "EMAC_SKB_ENQUEUED_INDEXES_FREE_PTR", 1, PRIVATE_A_INDEX, &EMAC_SKB_ENQUEUED_INDEXES_FREE_PTR, 8, 1, 1 },
#endif
#if defined G9991
	{ "CPU_TX_PICO_QUEUE", 1, PRIVATE_A_INDEX, &CPU_TX_PICO_QUEUE, 16, 1, 1 },
#endif
#if defined G9991
	{ "EMAC_SKB_ENQUEUED_INDEXES_FIFO", 1, PRIVATE_A_INDEX, &EMAC_SKB_ENQUEUED_INDEXES_FIFO, 5, 1, 1 },
#endif
#if defined G9991
	{ "G9991_FRAGMENT_BUFFER", 1, PRIVATE_A_INDEX, &G9991_FRAGMENT_BUFFER, 132, 1, 1 },
#endif
#if defined G9991
	{ "ETH_TX_INTER_LAN_SCHEDULING_OFFSET", 1, PRIVATE_A_INDEX, &ETH_TX_INTER_LAN_SCHEDULING_OFFSET, 1, 1, 1 },
#endif
#if defined G9991
	{ "FIREWALL_CONFIGURATION_REGISTER", 1, PRIVATE_A_INDEX, &FIREWALL_CONFIGURATION_REGISTER, 1, 1, 1 },
#endif
#if defined G9991
	{ "HASH_BUFFER", 1, PRIVATE_A_INDEX, &HASH_BUFFER, 1, 1, 1 },
#endif
#if defined G9991
	{ "DHD_COMPLETE_RING_DESCRIPTOR_BUFFER", 1, PRIVATE_A_INDEX, &DHD_COMPLETE_RING_DESCRIPTOR_BUFFER, 3, 1, 1 },
#endif
#if defined G9991
	{ "IPTV_DMA_LKP_KEY", 1, PRIVATE_A_INDEX, &IPTV_DMA_LKP_KEY, 1, 1, 1 },
#endif
#if defined G9991
	{ "G9991_FRAGMENT_ENQUEUE_INGRESS_QUEUE", 1, PRIVATE_A_INDEX, &G9991_FRAGMENT_ENQUEUE_INGRESS_QUEUE, 64, 1, 1 },
#endif
#if defined G9991
	{ "DS_DSCP_TO_PBITS_TABLE", 1, PRIVATE_A_INDEX, &DS_DSCP_TO_PBITS_TABLE, 1, 64, 1 },
#endif
#if defined G9991
	{ "DOWNSTREAM_LAN_ENQUEUE_SERVICE_QUEUE", 1, PRIVATE_A_INDEX, &DOWNSTREAM_LAN_ENQUEUE_SERVICE_QUEUE, 64, 1, 1 },
#endif
#if defined G9991
	{ "DS_VLAN_PRIMITIVE_TABLE", 1, PRIVATE_A_INDEX, &DS_VLAN_PRIMITIVE_TABLE, 32, 1, 1 },
#endif
#if defined G9991
	{ "PACKET_SRAM_TO_DDR_COPY_BUFFER", 1, PRIVATE_A_INDEX, &PACKET_SRAM_TO_DDR_COPY_BUFFER, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_CPU_RX_PICO_INGRESS_QUEUE", 1, PRIVATE_A_INDEX, &DS_CPU_RX_PICO_INGRESS_QUEUE, 64, 1, 1 },
#endif
#if defined G9991
	{ "DS_CPU_TX_MESSAGE_DATA_BUFFER", 1, PRIVATE_A_INDEX, &DS_CPU_TX_MESSAGE_DATA_BUFFER, 1, 1, 1 },
#endif
#if defined G9991
	{ "DOWNSTREAM_MULTICAST_INGRESS_QUEUE", 1, PRIVATE_A_INDEX, &DOWNSTREAM_MULTICAST_INGRESS_QUEUE, 64, 1, 1 },
#endif
#if defined G9991
	{ "DS_VLAN_ACTION_BUFFER", 1, PRIVATE_A_INDEX, &DS_VLAN_ACTION_BUFFER, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_ROUTER_INGRESS_QUEUE", 1, PRIVATE_A_INDEX, &DS_ROUTER_INGRESS_QUEUE, 64, 1, 1 },
#endif
#if defined G9991
	{ "DS_DHD_TX_POST_FLOW_RING_BUFFER", 1, PRIVATE_A_INDEX, &DS_DHD_TX_POST_FLOW_RING_BUFFER, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_INGRESS_FILTERS_CONFIGURATION_TABLE", 1, PRIVATE_A_INDEX, &DS_INGRESS_FILTERS_CONFIGURATION_TABLE, 3, 1, 1 },
#endif
#if defined G9991
	{ "G9991_VIRTUAL_PORT_RATE_LIMITER_STATUS", 1, PRIVATE_A_INDEX, &G9991_VIRTUAL_PORT_RATE_LIMITER_STATUS, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_CPU_RX_FAST_INGRESS_QUEUE", 1, PRIVATE_A_INDEX, &DS_CPU_RX_FAST_INGRESS_QUEUE, 64, 1, 1 },
#endif
#if defined G9991
	{ "DHD_COMPLETE_RING_BUFFER", 1, PRIVATE_A_INDEX, &DHD_COMPLETE_RING_BUFFER, 3, 1, 1 },
#endif
#if defined G9991
	{ "DS_RUNNER_FLOW_HEADER_DESCRIPTOR", 1, PRIVATE_A_INDEX, &DS_RUNNER_FLOW_HEADER_DESCRIPTOR, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_DATA_POINTER_DUMMY_TARGET", 1, PRIVATE_A_INDEX, &DS_DATA_POINTER_DUMMY_TARGET, 5, 1, 1 },
#endif
#if defined G9991
	{ "BPM_DDR_OPTIMIZED_BUFFERS_WITHOUT_HEADROOM_BASE", 1, PRIVATE_A_INDEX, &BPM_DDR_OPTIMIZED_BUFFERS_WITHOUT_HEADROOM_BASE, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_NULL_BUFFER", 1, PRIVATE_A_INDEX, &DS_NULL_BUFFER, 1, 1, 1 },
#endif
#if defined G9991
	{ "CPU_FLOW_CACHE_INGRESS_QUEUE", 1, PRIVATE_A_INDEX, &CPU_FLOW_CACHE_INGRESS_QUEUE, 64, 1, 1 },
#endif
#if defined G9991
	{ "EMAC_ABSOLUTE_TX_BBH_COUNTER", 1, PRIVATE_A_INDEX, &EMAC_ABSOLUTE_TX_BBH_COUNTER, 6, 1, 1 },
#endif
#if defined G9991
	{ "FREE_PACKET_DESCRIPTORS_POOL_DESCRIPTOR", 1, PRIVATE_A_INDEX, &FREE_PACKET_DESCRIPTORS_POOL_DESCRIPTOR, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_DEBUG_BUFFER", 1, PRIVATE_A_INDEX, &DS_DEBUG_BUFFER, 32, 1, 1 },
#endif
#if defined G9991
	{ "DS_ONE_BUFFER", 1, PRIVATE_A_INDEX, &DS_ONE_BUFFER, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_DHD_TX_POST_INGRESS_QUEUE", 1, PRIVATE_A_INDEX, &DS_DHD_TX_POST_INGRESS_QUEUE, 64, 1, 1 },
#endif
#if defined G9991
	{ "DS_DHD_FLOW_RING_DROP_COUNTER", 1, PRIVATE_A_INDEX, &DS_DHD_FLOW_RING_DROP_COUNTER, 5, 1, 1 },
#endif
#if defined G9991
	{ "DS_BPM_EXTRA_DDR_BUFFERS_BASE", 1, PRIVATE_A_INDEX, &DS_BPM_EXTRA_DDR_BUFFERS_BASE, 1, 1, 1 },
#endif
#if defined G9991
	{ "WLAN_MCAST_SSID_STATS_STATE_TABLE", 1, PRIVATE_A_INDEX, &WLAN_MCAST_SSID_STATS_STATE_TABLE, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_MAIN_TIMER_CONTROL_DESCRIPTOR", 1, PRIVATE_A_INDEX, &DS_MAIN_TIMER_CONTROL_DESCRIPTOR, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_FAST_FREE_SKB_INDEXES_FIFO_LOCAL_TABLE", 1, PRIVATE_A_INDEX, &DS_FAST_FREE_SKB_INDEXES_FIFO_LOCAL_TABLE, 8, 1, 1 },
#endif
#if defined G9991
	{ "DS_LAYER4_FILTERS_CONTEXT_TABLE", 1, PRIVATE_A_INDEX, &DS_LAYER4_FILTERS_CONTEXT_TABLE, 16, 1, 1 },
#endif
#if defined G9991
	{ "GPON_RX_DIRECT_DESCRIPTORS", 1, PRIVATE_A_INDEX, &GPON_RX_DIRECT_DESCRIPTORS, 32, 1, 1 },
#endif
#if defined G9991
	{ "WLAN_MCAST_INGRESS_QUEUE", 1, PRIVATE_A_INDEX, &WLAN_MCAST_INGRESS_QUEUE, 64, 1, 1 },
#endif
#if defined G9991
	{ "DS_PICO_FREE_SKB_INDEXES_FIFO_LOCAL_TABLE", 1, PRIVATE_A_INDEX, &DS_PICO_FREE_SKB_INDEXES_FIFO_LOCAL_TABLE, 8, 1, 1 },
#endif
#if defined G9991
	{ "DS_IPTV_SBPM_REPLICATION_BN", 1, PRIVATE_A_INDEX, &DS_IPTV_SBPM_REPLICATION_BN, 16, 1, 1 },
#endif
#if defined G9991
	{ "IPTV_DMA_RW_BUFFER", 1, PRIVATE_A_INDEX, &IPTV_DMA_RW_BUFFER, 1, 1, 1 },
#endif
#if defined G9991
	{ "MULTICAST_HEADER_BUFFER", 1, PRIVATE_A_INDEX, &MULTICAST_HEADER_BUFFER, 1, 1, 1 },
#endif
#if defined G9991
	{ "GSO_TX_DHD_L2_BUFFER", 1, PRIVATE_A_INDEX, &GSO_TX_DHD_L2_BUFFER, 22, 1, 1 },
#endif
#if defined G9991
	{ "DS_PICO_TIMER_CONTROL_DESCRIPTOR", 1, PRIVATE_A_INDEX, &DS_PICO_TIMER_CONTROL_DESCRIPTOR, 1, 1, 1 },
#endif
#if defined G9991
	{ "CPU_TX_DHD_L2_BUFFER", 1, PRIVATE_A_INDEX, &CPU_TX_DHD_L2_BUFFER, 22, 1, 1 },
#endif
#if defined G9991
	{ "IPTV_COUNTERS_BUFFER", 1, PRIVATE_A_INDEX, &IPTV_COUNTERS_BUFFER, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_INGRESS_CLASSIFICATION_KEY_BUFFER", 1, PRIVATE_A_INDEX, &DS_INGRESS_CLASSIFICATION_KEY_BUFFER, 1, 1, 1 },
#endif
#if defined G9991
	{ "ETH_TX_QUEUE_DUMMY_DESCRIPTOR", 1, PRIVATE_A_INDEX, &ETH_TX_QUEUE_DUMMY_DESCRIPTOR, 1, 1, 1 },
#endif
#if defined G9991
	{ "FIREWALL_RULE_ENTRY_BUFFER", 1, PRIVATE_A_INDEX, &FIREWALL_RULE_ENTRY_BUFFER, 1, 1, 1 },
#endif
#if defined G9991
	{ "CPU_TX_DS_PICO_DHD_TX_POST_CONTEXT", 1, PRIVATE_A_INDEX, &CPU_TX_DS_PICO_DHD_TX_POST_CONTEXT, 1, 1, 1 },
#endif
#if defined G9991
	{ "WLAN_MCAST_DHD_TX_POST_CONTEXT", 1, PRIVATE_A_INDEX, &WLAN_MCAST_DHD_TX_POST_CONTEXT, 1, 1, 1 },
#endif
#if defined G9991
	{ "GSO_TX_DS_PICO_DHD_TX_POST_CONTEXT", 1, PRIVATE_A_INDEX, &GSO_TX_DS_PICO_DHD_TX_POST_CONTEXT, 1, 1, 1 },
#endif
#if defined G9991
	{ "SC_BUFFER", 1, PRIVATE_A_INDEX, &SC_BUFFER, 1, 1, 1 },
#endif
#if defined G9991
	{ "IPTV_SSM_CONTEXT_TABLE_PTR", 1, PRIVATE_A_INDEX, &IPTV_SSM_CONTEXT_TABLE_PTR, 1, 1, 1 },
#endif
#if defined G9991
	{ "ETH_PHYSICAL_PORT_ACK_PENDING", 1, PRIVATE_A_INDEX, &ETH_PHYSICAL_PORT_ACK_PENDING, 5, 1, 1 },
#endif
#if defined G9991
	{ "PRIVATE_A_DUMMY_STORE", 1, PRIVATE_A_INDEX, &PRIVATE_A_DUMMY_STORE, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_INGRESS_CLASSIFICATION_COUNTERS_BUFFER", 1, PRIVATE_A_INDEX, &DS_INGRESS_CLASSIFICATION_COUNTERS_BUFFER, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_MULTICAST_VECTOR_TO_PORT_TABLE", 1, PRIVATE_A_INDEX, &DS_MULTICAST_VECTOR_TO_PORT_TABLE, 8, 1, 1 },
#endif
#if defined G9991
	{ "MULTICAST_DUMMY_VLAN_INDEXES_TABLE", 1, PRIVATE_A_INDEX, &MULTICAST_DUMMY_VLAN_INDEXES_TABLE, 1, 1, 1 },
#endif
#if defined G9991
	{ "IPTV_TABLE_POINTER", 1, PRIVATE_A_INDEX, &IPTV_TABLE_POINTER, 1, 1, 1 },
#endif
#if defined G9991
	{ "IPTV_CONTEXT_TABLE_POINTER", 1, PRIVATE_A_INDEX, &IPTV_CONTEXT_TABLE_POINTER, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_BPM_DDR_OPTIMIZED_BUFFERS_BASE", 1, PRIVATE_A_INDEX, &DS_BPM_DDR_OPTIMIZED_BUFFERS_BASE, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_BPM_DDR_BUFFERS_BASE", 1, PRIVATE_A_INDEX, &DS_BPM_DDR_BUFFERS_BASE, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_CONNECTION_TABLE_CONFIG", 1, PRIVATE_A_INDEX, &DS_CONNECTION_TABLE_CONFIG, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_CONTEXT_TABLE_CONFIG", 1, PRIVATE_A_INDEX, &DS_CONTEXT_TABLE_CONFIG, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_DEBUG_PERIPHERALS_STATUS_REGISTER", 1, PRIVATE_A_INDEX, &DS_DEBUG_PERIPHERALS_STATUS_REGISTER, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_DDR_ADDRESS_FOR_SYNC_DMA_POINTER", 1, PRIVATE_A_INDEX, &DS_DDR_ADDRESS_FOR_SYNC_DMA_POINTER, 1, 1, 1 },
#endif
#if defined G9991
	{ "EMAC_SKB_ENQUEUED_INDEXES_FIFO_COUNTERS", 1, PRIVATE_A_INDEX, &EMAC_SKB_ENQUEUED_INDEXES_FIFO_COUNTERS, 5, 1, 1 },
#endif
#if defined G9991
	{ "DS_FAST_FREE_SKB_INDEXES_FIFO_LOCAL_TABLE_PTR", 1, PRIVATE_A_INDEX, &DS_FAST_FREE_SKB_INDEXES_FIFO_LOCAL_TABLE_PTR, 1, 1, 1 },
#endif
#if defined G9991
	{ "CPU_TX_PICO_INGRESS_QUEUE_PTR", 1, PRIVATE_A_INDEX, &CPU_TX_PICO_INGRESS_QUEUE_PTR, 1, 1, 1 },
#endif
#if defined G9991
	{ "HASH_BASED_FORWARDING_PORT_TABLE", 1, PRIVATE_A_INDEX, &HASH_BASED_FORWARDING_PORT_TABLE, 4, 1, 1 },
#endif
#if defined G9991
	{ "FIREWALL_IPV6_R16_BUFFER", 1, PRIVATE_A_INDEX, &FIREWALL_IPV6_R16_BUFFER, 1, 1, 1 },
#endif
#if defined G9991
	{ "FREE_PACKET_DESCRIPTORS_POOL_THRESHOLD", 1, PRIVATE_A_INDEX, &FREE_PACKET_DESCRIPTORS_POOL_THRESHOLD, 1, 1, 1 },
#endif
#if defined G9991
	{ "DOWNSTREAM_MULTICAST_LAN_ENQUEUE_INGRESS_QUEUE_PTR", 1, PRIVATE_A_INDEX, &DOWNSTREAM_MULTICAST_LAN_ENQUEUE_INGRESS_QUEUE_PTR, 1, 1, 1 },
#endif
#if defined G9991
	{ "G9991_DOWNSTREAM_MULTICAST_FRAGMENTATION_INGRESS_QUEUE_PTR", 1, PRIVATE_A_INDEX, &G9991_DOWNSTREAM_MULTICAST_FRAGMENTATION_INGRESS_QUEUE_PTR, 1, 1, 1 },
#endif
#if defined G9991
	{ "CPU_RX_PD_INGRESS_QUEUE_PTR", 1, PRIVATE_A_INDEX, &CPU_RX_PD_INGRESS_QUEUE_PTR, 1, 1, 1 },
#endif
#if defined G9991
	{ "CPU_RX_MIRRORING_PD_INGRESS_QUEUE_PTR", 1, PRIVATE_A_INDEX, &CPU_RX_MIRRORING_PD_INGRESS_QUEUE_PTR, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_CPU_RX_PICO_INGRESS_QUEUE_PTR", 1, PRIVATE_A_INDEX, &DS_CPU_RX_PICO_INGRESS_QUEUE_PTR, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_BPM_DDR_BUFFER_HEADROOM_SIZE", 1, PRIVATE_A_INDEX, &DS_BPM_DDR_BUFFER_HEADROOM_SIZE, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_BPM_DDR_BUFFER_HEADROOM_SIZE_2_BYTE_RESOLUTION", 1, PRIVATE_A_INDEX, &DS_BPM_DDR_BUFFER_HEADROOM_SIZE_2_BYTE_RESOLUTION, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_CPU_RX_FAST_INGRESS_QUEUE_PTR", 1, PRIVATE_A_INDEX, &DS_CPU_RX_FAST_INGRESS_QUEUE_PTR, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_MEMLIB_SEMAPHORE", 1, PRIVATE_A_INDEX, &DS_MEMLIB_SEMAPHORE, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_MICROCODE_VERSION", 1, PRIVATE_A_INDEX, &DS_MICROCODE_VERSION, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_RATE_LIMITER_COUNTER_BUFFER", 1, PRIVATE_A_INDEX, &DS_RATE_LIMITER_COUNTER_BUFFER, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_DDR_ADDRESS_FOR_FULL_READ_DMA_POINTER", 1, PRIVATE_A_INDEX, &DS_DDR_ADDRESS_FOR_FULL_READ_DMA_POINTER, 1, 1, 1 },
#endif
#if defined G9991
	{ "GSO_TX_DHD_HOST_BUF_PTR", 1, PRIVATE_A_INDEX, &GSO_TX_DHD_HOST_BUF_PTR, 4, 1, 1 },
#endif
#if defined G9991
	{ "DS_DHD_TX_POST_HOST_DATA_PTR_BUFFER", 1, PRIVATE_A_INDEX, &DS_DHD_TX_POST_HOST_DATA_PTR_BUFFER, 1, 1, 1 },
#endif
#if defined G9991
	{ "CPU_TX_DHD_HOST_BUF_PTR", 1, PRIVATE_A_INDEX, &CPU_TX_DHD_HOST_BUF_PTR, 4, 1, 1 },
#endif
#if defined G9991
	{ "DUAL_STACK_LITE_LAYER_2_HEADER_LENGTH", 1, PRIVATE_A_INDEX, &DUAL_STACK_LITE_LAYER_2_HEADER_LENGTH, 1, 1, 1 },
#endif
#if defined G9991
	{ "DUAL_STACK_LITE_LAYER_3_HEADER_LENGTH", 1, PRIVATE_A_INDEX, &DUAL_STACK_LITE_LAYER_3_HEADER_LENGTH, 1, 1, 1 },
#endif
#if defined G9991
	{ "DOWNSTREAM_MULTICAST_LAN_ENQUEUE_SERVICE_QUEUE_INGRESS_QUEUE_PTR", 1, PRIVATE_A_INDEX, &DOWNSTREAM_MULTICAST_LAN_ENQUEUE_SERVICE_QUEUE_INGRESS_QUEUE_PTR, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_RUNNER_CONGESTION_STATE", 1, PRIVATE_A_INDEX, &DS_RUNNER_CONGESTION_STATE, 1, 1, 1 },
#endif
#if defined G9991
	{ "FIREWALL_RULE_MAP_ENTRY_BUFFER", 1, PRIVATE_A_INDEX, &FIREWALL_RULE_MAP_ENTRY_BUFFER, 1, 1, 1 },
#endif
#if defined G9991
	{ "DHD_TX_COMPLETE_BPM_REF_COUNTER", 1, PRIVATE_A_INDEX, &DHD_TX_COMPLETE_BPM_REF_COUNTER, 1, 1, 1 },
#endif
#if defined G9991
	{ "PCI_TX_FIFO_DESCRIPTORS_TABLE", 1, PRIVATE_A_INDEX, &PCI_TX_FIFO_DESCRIPTORS_TABLE, 4, 1, 1 },
#endif
#if defined G9991
	{ "PCI_TX_QUEUES_VECTOR", 1, PRIVATE_A_INDEX, &PCI_TX_QUEUES_VECTOR, 1, 1, 1 },
#endif
#if defined G9991
	{ "PCI_TX_FIFO_FULL_VECTOR", 1, PRIVATE_A_INDEX, &PCI_TX_FIFO_FULL_VECTOR, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_PICO_FREE_SKB_INDEXES_FIFO_LOCAL_TABLE_PTR", 1, PRIVATE_A_INDEX, &DS_PICO_FREE_SKB_INDEXES_FIFO_LOCAL_TABLE_PTR, 1, 1, 1 },
#endif
#if defined G9991
	{ "DHD_TX_POST_CPU_BPM_REF_COUNTER", 1, PRIVATE_A_INDEX, &DHD_TX_POST_CPU_BPM_REF_COUNTER, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_DHD_BPM_CONGESTION_UG3_DROP_COUNTER", 1, PRIVATE_A_INDEX, &DS_DHD_BPM_CONGESTION_UG3_DROP_COUNTER, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_DHD_BPM_CONGESTION_ALLOC_FAIL_DROP_COUNTER", 1, PRIVATE_A_INDEX, &DS_DHD_BPM_CONGESTION_ALLOC_FAIL_DROP_COUNTER, 1, 1, 1 },
#endif
#if defined G9991
	{ "G9991_FRAGMENT_DMA_SYNC", 1, PRIVATE_A_INDEX, &G9991_FRAGMENT_DMA_SYNC, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_INGRESS_CLASSIFICATION_RULE_CFG_DESCRIPTOR", 1, PRIVATE_A_INDEX, &DS_INGRESS_CLASSIFICATION_RULE_CFG_DESCRIPTOR, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_INGRESS_CLASSIFICATION_IP_FLOW_RULE_CFG_DESCRIPTOR", 1, PRIVATE_A_INDEX, &DS_INGRESS_CLASSIFICATION_IP_FLOW_RULE_CFG_DESCRIPTOR, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_PACKET_BUFFER_SIZE_ASR_8", 1, PRIVATE_A_INDEX, &DS_PACKET_BUFFER_SIZE_ASR_8, 1, 1, 1 },
#endif
#if defined G9991
	{ "RUNNER_FLOW_IH_RESPONSE", 1, PRIVATE_A_INDEX, &RUNNER_FLOW_IH_RESPONSE, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_MAIN_DMA_SYNCRONIZATION_ADDRESS", 1, PRIVATE_A_INDEX, &DS_MAIN_DMA_SYNCRONIZATION_ADDRESS, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_PICO_DMA_SYNCRONIZATION_ADDRESS", 1, PRIVATE_A_INDEX, &DS_PICO_DMA_SYNCRONIZATION_ADDRESS, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_RUNNER_FLOW_IH_RESPONSE_MUTEX", 1, PRIVATE_A_INDEX, &DS_RUNNER_FLOW_IH_RESPONSE_MUTEX, 1, 1, 1 },
#endif
#if defined G9991
	{ "DOWNSTREAM_MULTICAST_SERVICE_QUEUE_SSID_PTR", 1, PRIVATE_A_INDEX, &DOWNSTREAM_MULTICAST_SERVICE_QUEUE_SSID_PTR, 1, 1, 1 },
#endif
#if defined G9991
	{ "FIREWALL_RULE_MAP_ENTRY_BUFFER_SYNC", 1, PRIVATE_A_INDEX, &FIREWALL_RULE_MAP_ENTRY_BUFFER_SYNC, 1, 1, 1 },
#endif
#if defined G9991
	{ "IP_SYNC_1588_TX_ENQUEUE_RESULT", 1, PRIVATE_A_INDEX, &IP_SYNC_1588_TX_ENQUEUE_RESULT, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_DHD_DMA_SYNCHRONIZATION", 1, PRIVATE_A_INDEX, &DS_DHD_DMA_SYNCHRONIZATION, 1, 1, 1 },
#endif
#if defined G9991
	{ "DHD_TX_POST_CPU_SEMAPHORE", 1, PRIVATE_A_INDEX, &DHD_TX_POST_CPU_SEMAPHORE, 1, 1, 1 },
#endif
#if defined G9991
	{ "GPON_RX_NORMAL_DESCRIPTORS", 1, PRIVATE_A_INDEX, &GPON_RX_NORMAL_DESCRIPTORS, 32, 1, 1 },
#endif
#if defined G9991
	{ "DOWNSTREAM_DMA_PIPE_BUFFER", 1, PRIVATE_A_INDEX, &DOWNSTREAM_DMA_PIPE_BUFFER, 1, 1, 1 },
#endif
#if defined G9991
	{ "US_DHD_BPM_CONGESTION_ALLOC_FAIL_DROP_COUNTER", 1, PRIVATE_B_INDEX, &US_DHD_BPM_CONGESTION_ALLOC_FAIL_DROP_COUNTER, 1, 1, 1 },
#endif
#if defined G9991
	{ "US_INGRESS_HANDLER_BUFFER", 1, PRIVATE_B_INDEX, &US_INGRESS_HANDLER_BUFFER, 32, 1, 1 },
#endif
#if defined G9991
	{ "US_FREE_PACKET_DESCRIPTORS_POOL", 1, PRIVATE_B_INDEX, &US_FREE_PACKET_DESCRIPTORS_POOL, 3072, 1, 1 },
#endif
#if defined G9991
	{ "US_INGRESS_CLASSIFICATION_CONTEXT_TABLE", 1, PRIVATE_B_INDEX, &US_INGRESS_CLASSIFICATION_CONTEXT_TABLE, 256, 1, 1 },
#endif
#if defined G9991
	{ "US_DSCP_TO_PBITS_TABLE", 1, PRIVATE_B_INDEX, &US_DSCP_TO_PBITS_TABLE, 6, 64, 1 },
#endif
#if defined G9991
	{ "US_SBPM_REPLY", 1, PRIVATE_B_INDEX, &US_SBPM_REPLY, 1, 1, 1 },
#endif
#if defined G9991
	{ "US_VLAN_PARAMETER_TABLE", 1, PRIVATE_B_INDEX, &US_VLAN_PARAMETER_TABLE, 128, 1, 1 },
#endif
#if defined G9991
	{ "US_SID_CONTEXT_TABLE", 1, PRIVATE_B_INDEX, &US_SID_CONTEXT_TABLE, 32, 1, 1 },
#endif
#if defined G9991
	{ "US_INGRESS_RATE_LIMITER_TABLE", 1, PRIVATE_B_INDEX, &US_INGRESS_RATE_LIMITER_TABLE, 24, 1, 1 },
#endif
#if defined G9991
	{ "US_QUEUE_PROFILE_TABLE", 1, PRIVATE_B_INDEX, &US_QUEUE_PROFILE_TABLE, 8, 1, 1 },
#endif
#if defined G9991
	{ "US_CPU_REASON_TO_METER_TABLE", 1, PRIVATE_B_INDEX, &US_CPU_REASON_TO_METER_TABLE, 64, 1, 1 },
#endif
#if defined G9991
	{ "US_FAST_CPU_TX_DESCRIPTOR_ABS_DATA_PTR_QUEUE", 1, PRIVATE_B_INDEX, &US_FAST_CPU_TX_DESCRIPTOR_ABS_DATA_PTR_QUEUE, 16, 1, 1 },
#endif
#if defined G9991
	{ "US_PICO_CPU_TX_DESCRIPTOR_ABS_DATA_PTR_QUEUE", 1, PRIVATE_B_INDEX, &US_PICO_CPU_TX_DESCRIPTOR_ABS_DATA_PTR_QUEUE, 16, 1, 1 },
#endif
#if defined G9991
	{ "US_PBITS_TO_QOS_TABLE", 1, PRIVATE_B_INDEX, &US_PBITS_TO_QOS_TABLE, 8, 8, 1 },
#endif
#if defined G9991
	{ "US_RATE_LIMITER_TABLE", 1, PRIVATE_B_INDEX, &US_RATE_LIMITER_TABLE, 16, 1, 1 },
#endif
#if defined G9991
	{ "US_LAYER4_FILTERS_LOOKUP_TABLE", 1, PRIVATE_B_INDEX, &US_LAYER4_FILTERS_LOOKUP_TABLE, 16, 1, 1 },
#endif
#if defined G9991
	{ "WAN_CHANNELS_8_39_TABLE", 1, PRIVATE_B_INDEX, &WAN_CHANNELS_8_39_TABLE, 32, 1, 1 },
#endif
#if defined G9991
	{ "US_WAN_FLOW_TABLE", 1, PRIVATE_B_INDEX, &US_WAN_FLOW_TABLE, 256, 1, 1 },
#endif
#if defined G9991
	{ "US_INGRESS_FILTERS_LOOKUP_TABLE", 1, PRIVATE_B_INDEX, &US_INGRESS_FILTERS_LOOKUP_TABLE, 6, 32, 1 },
#endif
#if defined G9991
	{ "US_PBITS_TO_PBITS_TABLE", 1, PRIVATE_B_INDEX, &US_PBITS_TO_PBITS_TABLE, 32, 8, 1 },
#endif
#if defined G9991
	{ "WAN_CHANNELS_0_7_TABLE", 1, PRIVATE_B_INDEX, &WAN_CHANNELS_0_7_TABLE, 8, 1, 1 },
#endif
#if defined G9991
	{ "US_TRAFFIC_CLASS_TO_QUEUE_TABLE", 1, PRIVATE_B_INDEX, &US_TRAFFIC_CLASS_TO_QUEUE_TABLE, 8, 8, 1 },
#endif
#if defined G9991
	{ "US_PBITS_PARAMETER_TABLE", 1, PRIVATE_B_INDEX, &US_PBITS_PARAMETER_TABLE, 128, 1, 1 },
#endif
#if defined G9991
	{ "US_RUNNER_FLOW_HEADER_BUFFER", 1, PRIVATE_B_INDEX, &US_RUNNER_FLOW_HEADER_BUFFER, 3, 1, 1 },
#endif
#if defined G9991
	{ "US_CPU_RX_METER_TABLE", 1, PRIVATE_B_INDEX, &US_CPU_RX_METER_TABLE, 16, 1, 1 },
#endif
#if defined G9991
	{ "SMART_CARD_DESCRIPTOR_TABLE", 1, PRIVATE_B_INDEX, &SMART_CARD_DESCRIPTOR_TABLE, 1, 1, 1 },
#endif
#if defined G9991
	{ "US_VLAN_COMMANDS_TABLE", 1, PRIVATE_B_INDEX, &US_VLAN_COMMANDS_TABLE, 64, 1, 1 },
#endif
#if defined G9991
	{ "US_VLAN_OPTIMIZATION_TABLE", 1, PRIVATE_B_INDEX, &US_VLAN_OPTIMIZATION_TABLE, 128, 1, 1 },
#endif
#if defined G9991
	{ "GPON_ABSOLUTE_TX_BBH_COUNTER", 1, PRIVATE_B_INDEX, &GPON_ABSOLUTE_TX_BBH_COUNTER, 40, 1, 1 },
#endif
#if defined G9991
	{ "US_CPU_PARAMETERS_BLOCK", 1, PRIVATE_B_INDEX, &US_CPU_PARAMETERS_BLOCK, 1, 1, 1 },
#endif
#if defined G9991
	{ "US_PBITS_TO_WAN_FLOW_TABLE", 1, PRIVATE_B_INDEX, &US_PBITS_TO_WAN_FLOW_TABLE, 8, 8, 1 },
#endif
#if defined G9991
	{ "US_LAN_VID_TABLE", 1, PRIVATE_B_INDEX, &US_LAN_VID_TABLE, 128, 1, 1 },
#endif
#if defined G9991
	{ "US_POLICER_TABLE", 1, PRIVATE_B_INDEX, &US_POLICER_TABLE, 16, 1, 1 },
#endif
#if defined G9991
	{ "US_CPU_TX_FAST_QUEUE", 1, PRIVATE_B_INDEX, &US_CPU_TX_FAST_QUEUE, 16, 1, 1 },
#endif
#if defined G9991
	{ "US_INGRESS_HANDLER_SKB_DATA_POINTER", 1, PRIVATE_B_INDEX, &US_INGRESS_HANDLER_SKB_DATA_POINTER, 32, 1, 1 },
#endif
#if defined G9991
	{ "US_CPU_TX_PICO_QUEUE", 1, PRIVATE_B_INDEX, &US_CPU_TX_PICO_QUEUE, 16, 1, 1 },
#endif
#if defined G9991
	{ "US_INGRESS_CLASSIFICATION_RULE_CFG_TABLE", 1, PRIVATE_B_INDEX, &US_INGRESS_CLASSIFICATION_RULE_CFG_TABLE, 16, 1, 1 },
#endif
#if defined G9991
	{ "US_CPU_TX_BBH_DESCRIPTORS", 1, PRIVATE_B_INDEX, &US_CPU_TX_BBH_DESCRIPTORS, 32, 1, 1 },
#endif
#if defined G9991
	{ "US_FORWARDING_MATRIX_TABLE", 1, PRIVATE_B_INDEX, &US_FORWARDING_MATRIX_TABLE, 9, 16, 1 },
#endif
#if defined G9991
	{ "US_RATE_CONTROL_BUDGET_ALLOCATOR_TABLE", 1, PRIVATE_B_INDEX, &US_RATE_CONTROL_BUDGET_ALLOCATOR_TABLE, 8, 1, 1 },
#endif
#if defined G9991
	{ "US_MAIN_TIMER_TASK_DESCRIPTOR_TABLE", 1, PRIVATE_B_INDEX, &US_MAIN_TIMER_TASK_DESCRIPTOR_TABLE, 4, 1, 1 },
#endif
#if defined G9991
	{ "US_INGRESS_CLASSIFICATION_KEY_PRIMITIVE_TABLE", 1, PRIVATE_B_INDEX, &US_INGRESS_CLASSIFICATION_KEY_PRIMITIVE_TABLE, 32, 1, 1 },
#endif
#if defined G9991
	{ "ETH0_RX_DESCRIPTORS", 1, PRIVATE_B_INDEX, &ETH0_RX_DESCRIPTORS, 32, 1, 1 },
#endif
#if defined G9991
	{ "US_BRIDGE_CONFIGURATION_REGISTER", 1, PRIVATE_B_INDEX, &US_BRIDGE_CONFIGURATION_REGISTER, 1, 1, 1 },
#endif
#if defined G9991
	{ "ETH1_RX_DESCRIPTORS", 1, PRIVATE_B_INDEX, &ETH1_RX_DESCRIPTORS, 32, 1, 1 },
#endif
#if defined G9991
	{ "US_PROFILING_BUFFER_PICO_RUNNER", 1, PRIVATE_B_INDEX, &US_PROFILING_BUFFER_PICO_RUNNER, 1, 1, 1 },
#endif
#if defined G9991
	{ "US_GPON_RX_DIRECT_DESCRIPTORS", 1, PRIVATE_B_INDEX, &US_GPON_RX_DIRECT_DESCRIPTORS, 32, 1, 1 },
#endif
#if defined G9991
	{ "US_INGRESS_FILTERS_PARAMETER_TABLE", 1, PRIVATE_B_INDEX, &US_INGRESS_FILTERS_PARAMETER_TABLE, 6, 16, 1 },
#endif
#if defined G9991
	{ "US_PICO_TIMER_TASK_DESCRIPTOR_TABLE", 1, PRIVATE_B_INDEX, &US_PICO_TIMER_TASK_DESCRIPTOR_TABLE, 4, 1, 1 },
#endif
#if defined G9991
	{ "DHD_RX_COMPLETE_FLOW_RING_BUFFER", 1, PRIVATE_B_INDEX, &DHD_RX_COMPLETE_FLOW_RING_BUFFER, 3, 1, 1 },
#endif
#if defined G9991
	{ "US_INGRESS_CLASSIFICATION_GENERIC_RULE_CFG_TABLE", 1, PRIVATE_B_INDEX, &US_INGRESS_CLASSIFICATION_GENERIC_RULE_CFG_TABLE, 4, 1, 1 },
#endif
#if defined G9991
	{ "DHD_RX_POST_FLOW_RING_BUFFER", 1, PRIVATE_B_INDEX, &DHD_RX_POST_FLOW_RING_BUFFER, 3, 1, 1 },
#endif
#if defined G9991
	{ "US_FAST_RUNNER_GLOBAL_REGISTERS_INIT", 1, PRIVATE_B_INDEX, &US_FAST_RUNNER_GLOBAL_REGISTERS_INIT, 8, 1, 1 },
#endif
#if defined G9991
	{ "US_PBITS_PRIMITIVE_TABLE", 1, PRIVATE_B_INDEX, &US_PBITS_PRIMITIVE_TABLE, 32, 1, 1 },
#endif
#if defined G9991
	{ "US_ACK_PACKETS_QUEUE_INDEX_TABLE", 1, PRIVATE_B_INDEX, &US_ACK_PACKETS_QUEUE_INDEX_TABLE, 40, 1, 1 },
#endif
#if defined G9991
	{ "BBH_TX_EPON_REQUEST_WAN_CHANNEL_INDEX", 1, PRIVATE_B_INDEX, &BBH_TX_EPON_REQUEST_WAN_CHANNEL_INDEX, 1, 1, 1 },
#endif
#if defined G9991
	{ "US_RUNNER_FLOW_IH_RESPONSE", 1, PRIVATE_B_INDEX, &US_RUNNER_FLOW_IH_RESPONSE, 1, 1, 1 },
#endif
#if defined G9991
	{ "SMART_CARD_ERROR_COUNTERS_TABLE", 1, PRIVATE_B_INDEX, &SMART_CARD_ERROR_COUNTERS_TABLE, 1, 1, 1 },
#endif
#if defined G9991
	{ "US_CPU_RX_PICO_INGRESS_QUEUE", 1, PRIVATE_B_INDEX, &US_CPU_RX_PICO_INGRESS_QUEUE, 64, 1, 1 },
#endif
#if defined G9991
	{ "US_FLOW_BASED_ACTION_PTR_TABLE", 1, PRIVATE_B_INDEX, &US_FLOW_BASED_ACTION_PTR_TABLE, 32, 1, 1 },
#endif
#if defined G9991
	{ "US_ROUTER_INGRESS_QUEUE", 1, PRIVATE_B_INDEX, &US_ROUTER_INGRESS_QUEUE, 64, 1, 1 },
#endif
#if defined G9991
	{ "US_INGRESS_CLASSIFICATION_COUNTERS_BUFFER", 1, PRIVATE_B_INDEX, &US_INGRESS_CLASSIFICATION_COUNTERS_BUFFER, 30, 1, 1 },
#endif
#if defined G9991
	{ "US_BPM_EXTRA_DDR_BUFFERS_BASE", 1, PRIVATE_B_INDEX, &US_BPM_EXTRA_DDR_BUFFERS_BASE, 1, 1, 1 },
#endif
#if defined G9991
	{ "ETH2_RX_DESCRIPTORS", 1, PRIVATE_B_INDEX, &ETH2_RX_DESCRIPTORS, 32, 1, 1 },
#endif
#if defined G9991
	{ "US_CPU_RX_FAST_INGRESS_QUEUE", 1, PRIVATE_B_INDEX, &US_CPU_RX_FAST_INGRESS_QUEUE, 64, 1, 1 },
#endif
#if defined G9991
	{ "US_DEBUG_BUFFER", 1, PRIVATE_B_INDEX, &US_DEBUG_BUFFER, 32, 1, 1 },
#endif
#if defined G9991
	{ "SPEED_SERVICE_PARAMETERS_TABLE", 1, PRIVATE_B_INDEX, &SPEED_SERVICE_PARAMETERS_TABLE, 1, 1, 1 },
#endif
#if defined G9991
	{ "US_MAIN_TIMER_CONTROL_DESCRIPTOR", 1, PRIVATE_B_INDEX, &US_MAIN_TIMER_CONTROL_DESCRIPTOR, 1, 1, 1 },
#endif
#if defined G9991
	{ "US_BPM_DDR_OPTIMIZED_BUFFERS_BASE", 1, PRIVATE_B_INDEX, &US_BPM_DDR_OPTIMIZED_BUFFERS_BASE, 1, 1, 1 },
#endif
#if defined G9991
	{ "US_TPID_OVERWRITE_TABLE", 1, PRIVATE_B_INDEX, &US_TPID_OVERWRITE_TABLE, 8, 1, 1 },
#endif
#if defined G9991
	{ "ETH3_RX_DESCRIPTORS", 1, PRIVATE_B_INDEX, &ETH3_RX_DESCRIPTORS, 32, 1, 1 },
#endif
#if defined G9991
	{ "LOCAL_SWITCHING_MULTICAST_INGRESS_QUEUE", 1, PRIVATE_B_INDEX, &LOCAL_SWITCHING_MULTICAST_INGRESS_QUEUE, 64, 1, 1 },
#endif
#if defined G9991
	{ "US_VLAN_PRIMITIVE_TABLE", 1, PRIVATE_B_INDEX, &US_VLAN_PRIMITIVE_TABLE, 32, 1, 1 },
#endif
#if defined G9991
	{ "VLAN_ACTION_BRIDGE_INGRESS_QUEUE", 1, PRIVATE_B_INDEX, &VLAN_ACTION_BRIDGE_INGRESS_QUEUE, 64, 1, 1 },
#endif
#if defined G9991
	{ "US_CPU_TX_MESSAGE_DATA_BUFFER", 1, PRIVATE_B_INDEX, &US_CPU_TX_MESSAGE_DATA_BUFFER, 1, 1, 1 },
#endif
#if defined G9991
	{ "UPSTREAM_FLOODING_INGRESS_QUEUE", 1, PRIVATE_B_INDEX, &UPSTREAM_FLOODING_INGRESS_QUEUE, 64, 1, 1 },
#endif
#if defined G9991
	{ "US_OVERALL_RATE_LIMITER_WAN_CHANNEL_PTR_TABLE", 1, PRIVATE_B_INDEX, &US_OVERALL_RATE_LIMITER_WAN_CHANNEL_PTR_TABLE, 48, 1, 1 },
#endif
#if defined G9991
	{ "US_INGRESS_FILTERS_CONFIGURATION_TABLE", 1, PRIVATE_B_INDEX, &US_INGRESS_FILTERS_CONFIGURATION_TABLE, 6, 1, 1 },
#endif
#if defined G9991
	{ "BBH_TX_WAN_CHANNEL_INDEX", 1, PRIVATE_B_INDEX, &BBH_TX_WAN_CHANNEL_INDEX, 1, 1, 1 },
#endif
#if defined G9991
	{ "US_VLAN_ACTION_BUFFER", 1, PRIVATE_B_INDEX, &US_VLAN_ACTION_BUFFER, 1, 1, 1 },
#endif
#if defined G9991
	{ "WAN_INTERWORKING_INGRESS_QUEUE", 1, PRIVATE_B_INDEX, &WAN_INTERWORKING_INGRESS_QUEUE, 64, 1, 1 },
#endif
#if defined G9991
	{ "US_PICO_RUNNER_GLOBAL_REGISTERS_INIT", 1, PRIVATE_B_INDEX, &US_PICO_RUNNER_GLOBAL_REGISTERS_INIT, 8, 1, 1 },
#endif
#if defined G9991
	{ "INGRESS_FILTERS_PROFILE_TABLE", 1, PRIVATE_B_INDEX, &INGRESS_FILTERS_PROFILE_TABLE, 32, 1, 1 },
#endif
#if defined G9991
	{ "US_DHD_TX_POST_INGRESS_QUEUE", 1, PRIVATE_B_INDEX, &US_DHD_TX_POST_INGRESS_QUEUE, 64, 1, 1 },
#endif
#if defined G9991
	{ "US_RUNNER_FLOW_HEADER_DESCRIPTOR", 1, PRIVATE_B_INDEX, &US_RUNNER_FLOW_HEADER_DESCRIPTOR, 3, 1, 1 },
#endif
#if defined G9991
	{ "US_NULL_BUFFER", 1, PRIVATE_B_INDEX, &US_NULL_BUFFER, 1, 1, 1 },
#endif
#if defined G9991
	{ "LAN_INGRESS_FIFO_DESCRIPTOR_TABLE", 1, PRIVATE_B_INDEX, &LAN_INGRESS_FIFO_DESCRIPTOR_TABLE, 5, 1, 1 },
#endif
#if defined G9991
	{ "US_BPM_DDR_BUFFERS_BASE", 1, PRIVATE_B_INDEX, &US_BPM_DDR_BUFFERS_BASE, 1, 1, 1 },
#endif
#if defined G9991
	{ "US_MULTICAST_VECTOR_TO_PORT_TABLE", 1, PRIVATE_B_INDEX, &US_MULTICAST_VECTOR_TO_PORT_TABLE, 8, 1, 1 },
#endif
#if defined G9991
	{ "ETH4_RX_DESCRIPTORS", 1, PRIVATE_B_INDEX, &ETH4_RX_DESCRIPTORS, 32, 1, 1 },
#endif
#if defined G9991
	{ "CPU_REASON_AND_SRC_BRIDGE_PORT_TO_METER_TABLE", 1, PRIVATE_B_INDEX, &CPU_REASON_AND_SRC_BRIDGE_PORT_TO_METER_TABLE, 3, 30, 1 },
#endif
#if defined G9991
	{ "US_PICO_TIMER_CONTROL_DESCRIPTOR", 1, PRIVATE_B_INDEX, &US_PICO_TIMER_CONTROL_DESCRIPTOR, 1, 1, 1 },
#endif
#if defined G9991
	{ "US_CONNECTION_TABLE_CONFIG", 1, PRIVATE_B_INDEX, &US_CONNECTION_TABLE_CONFIG, 1, 1, 1 },
#endif
#if defined G9991
	{ "US_TIMER_SCHEDULER_PRIMITIVE_TABLE", 1, PRIVATE_B_INDEX, &US_TIMER_SCHEDULER_PRIMITIVE_TABLE, 8, 1, 1 },
#endif
#if defined G9991
	{ "US_LAYER4_FILTERS_CONTEXT_TABLE", 1, PRIVATE_B_INDEX, &US_LAYER4_FILTERS_CONTEXT_TABLE, 16, 1, 1 },
#endif
#if defined G9991
	{ "US_DHD_TX_POST_FLOW_RING_BUFFER", 1, PRIVATE_B_INDEX, &US_DHD_TX_POST_FLOW_RING_BUFFER, 1, 1, 1 },
#endif
#if defined G9991
	{ "US_INGRESS_CLASSIFICATION_KEY_BUFFER", 1, PRIVATE_B_INDEX, &US_INGRESS_CLASSIFICATION_KEY_BUFFER, 1, 1, 1 },
#endif
#if defined G9991
	{ "CPU_TX_DS_EGRESS_DHD_TX_POST_FLOW_RING_BUFFER", 1, PRIVATE_B_INDEX, &CPU_TX_DS_EGRESS_DHD_TX_POST_FLOW_RING_BUFFER, 1, 1, 1 },
#endif
#if defined G9991
	{ "CPU_TX_DS_EGRESS_DHD_TX_POST_CONTEXT", 1, PRIVATE_B_INDEX, &CPU_TX_DS_EGRESS_DHD_TX_POST_CONTEXT, 1, 1, 1 },
#endif
#if defined G9991
	{ "G9991_DFC_VECTOR", 1, COMMON_A_INDEX, &G9991_DFC_VECTOR, 1, 1, 1 },
#endif
#if defined G9991
	{ "COMMON_A_DUMMY_STORE", 1, COMMON_A_INDEX, &COMMON_A_DUMMY_STORE, 1, 1, 1 },
#endif
#if defined G9991
	{ "SCT_FILTER", 1, COMMON_A_INDEX, &SCT_FILTER, 1, 1, 1 },
#endif
#if defined G9991
	{ "RING_DESCRIPTORS_TABLE", 1, COMMON_A_INDEX, &RING_DESCRIPTORS_TABLE, 12, 1, 1 },
#endif
#if defined G9991
	{ "INTERRUPT_COALESCING_CONFIG_TABLE", 1, COMMON_A_INDEX, &INTERRUPT_COALESCING_CONFIG_TABLE, 12, 1, 1 },
#endif
#if defined G9991
	{ "INTERRUPT_COALESCING_TIMER_CONFIG_TABLE", 1, COMMON_A_INDEX, &INTERRUPT_COALESCING_TIMER_CONFIG_TABLE, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_CPU_REASON_TO_CPU_RX_QUEUE_TABLE", 1, COMMON_A_INDEX, &DS_CPU_REASON_TO_CPU_RX_QUEUE_TABLE, 64, 1, 1 },
#endif
#if defined G9991
	{ "DS_PBITS_TO_QOS_TABLE", 1, COMMON_A_INDEX, &DS_PBITS_TO_QOS_TABLE, 6, 8, 1 },
#endif
#if defined G9991
	{ "GLOBAL_DSCP_TO_PBITS_TABLE", 1, COMMON_A_INDEX, &GLOBAL_DSCP_TO_PBITS_TABLE, 1, 64, 1 },
#endif
#if defined G9991
	{ "GLOBAL_DSCP_TO_PBITS_DEI_TABLE", 1, COMMON_A_INDEX, &GLOBAL_DSCP_TO_PBITS_DEI_TABLE, 1, 64, 1 },
#endif
#if defined G9991
	{ "MAC_TABLE", 1, COMMON_A_INDEX, &MAC_TABLE, 1024, 1, 1 },
#endif
#if defined G9991
	{ "MAC_TABLE_CAM", 1, COMMON_A_INDEX, &MAC_TABLE_CAM, 32, 1, 1 },
#endif
#if defined G9991
	{ "MAC_EXTENSION_TABLE", 1, COMMON_A_INDEX, &MAC_EXTENSION_TABLE, 1024, 1, 1 },
#endif
#if defined G9991
	{ "MAC_EXTENSION_TABLE_CAM", 1, COMMON_A_INDEX, &MAC_EXTENSION_TABLE_CAM, 32, 1, 1 },
#endif
#if defined G9991
	{ "MAC_CONTEXT_TABLE", 1, COMMON_A_INDEX, &MAC_CONTEXT_TABLE, 1024, 1, 1 },
#endif
#if defined G9991
	{ "MAC_CONTEXT_TABLE_CAM", 1, COMMON_A_INDEX, &MAC_CONTEXT_TABLE_CAM, 32, 1, 1 },
#endif
#if defined G9991
	{ "MAC_UNKNOWN_DA_FORWARDING_TABLE", 1, COMMON_A_INDEX, &MAC_UNKNOWN_DA_FORWARDING_TABLE, 1, 1, 1 },
#endif
#if defined G9991
	{ "IPTV_LOOKUP_TABLE", 1, COMMON_A_INDEX, &IPTV_LOOKUP_TABLE, 256, 1, 1 },
#endif
#if defined G9991
	{ "IPTV_L3_SRC_IP_LOOKUP_TABLE", 1, COMMON_A_INDEX, &IPTV_L3_SRC_IP_LOOKUP_TABLE, 32, 1, 1 },
#endif
#if defined G9991
	{ "IPTV_LOOKUP_TABLE_CAM", 1, COMMON_A_INDEX, &IPTV_LOOKUP_TABLE_CAM, 32, 1, 1 },
#endif
#if defined G9991
	{ "IPTV_CONTEXT_TABLE", 1, COMMON_A_INDEX, &IPTV_CONTEXT_TABLE, 256, 1, 1 },
#endif
#if defined G9991
	{ "IPTV_CONTEXT_TABLE_CAM", 1, COMMON_A_INDEX, &IPTV_CONTEXT_TABLE_CAM, 32, 1, 1 },
#endif
#if defined G9991
	{ "DS_INGRESS_CLASSIFICATION_LOOKUP_LONG_TABLE", 1, COMMON_A_INDEX, &DS_INGRESS_CLASSIFICATION_LOOKUP_LONG_TABLE, 128, 1, 1 },
#endif
#if defined G9991
	{ "LAN_VID_CONTEXT_TABLE", 1, COMMON_A_INDEX, &LAN_VID_CONTEXT_TABLE, 128, 1, 1 },
#endif
#if defined G9991
	{ "WAN_VID_TABLE", 1, COMMON_A_INDEX, &WAN_VID_TABLE, 4, 1, 1 },
#endif
#if defined G9991
	{ "G9991_VIRTUAL_PORT_STATUS_PER_EMAC", 1, COMMON_A_INDEX, &G9991_VIRTUAL_PORT_STATUS_PER_EMAC, 5, 1, 1 },
#endif
#if defined G9991
	{ "G9991_DOWNSTREAM_MULTICAST_FRAGMENTATION_INGRESS_QUEUE", 1, COMMON_A_INDEX, &G9991_DOWNSTREAM_MULTICAST_FRAGMENTATION_INGRESS_QUEUE, 32, 1, 1 },
#endif
#if defined G9991
	{ "PACKET_SRAM_TO_DDR_COPY_BUFFER_1", 1, COMMON_A_INDEX, &PACKET_SRAM_TO_DDR_COPY_BUFFER_1, 1, 1, 1 },
#endif
#if defined G9991
	{ "PACKET_SRAM_TO_DDR_COPY_BUFFER_2", 1, COMMON_A_INDEX, &PACKET_SRAM_TO_DDR_COPY_BUFFER_2, 1, 1, 1 },
#endif
#if defined G9991
	{ "G9991_DDR_QUEUE_DESCRIPTORS_TABLE", 1, COMMON_A_INDEX, &G9991_DDR_QUEUE_DESCRIPTORS_TABLE, 120, 1, 1 },
#endif
#if defined G9991
	{ "G9991_DDR_QUEUE_ADDRESS_TABLE", 1, COMMON_A_INDEX, &G9991_DDR_QUEUE_ADDRESS_TABLE, 120, 1, 1 },
#endif
#if defined G9991
	{ "PM_COUNTERS_BUFFER", 1, COMMON_A_INDEX, &PM_COUNTERS_BUFFER, 1, 1, 1 },
#endif
#if defined G9991
	{ "FREE_SKB_INDEXES_DDR_FIFO_TAIL", 1, COMMON_A_INDEX, &FREE_SKB_INDEXES_DDR_FIFO_TAIL, 1, 1, 1 },
#endif
#if defined G9991
	{ "DDR_ADDRESS_FOR_FREE_SKB_INDEXES_FIFO_TABLE", 1, COMMON_A_INDEX, &DDR_ADDRESS_FOR_FREE_SKB_INDEXES_FIFO_TABLE, 1, 1, 1 },
#endif
#if defined G9991
	{ "DDR_ADDRESS_FOR_FREE_SKB_INDEXES_FIFO_TABLE_LAST_ENTRY", 1, COMMON_A_INDEX, &DDR_ADDRESS_FOR_FREE_SKB_INDEXES_FIFO_TABLE_LAST_ENTRY, 1, 1, 1 },
#endif
#if defined G9991
	{ "DDR_ADDRESS_FOR_SKB_DATA_POINTERS_TABLE", 1, COMMON_A_INDEX, &DDR_ADDRESS_FOR_SKB_DATA_POINTERS_TABLE, 1, 1, 1 },
#endif
#if defined G9991
	{ "BPM_REPLY_RUNNER_A", 1, COMMON_A_INDEX, &BPM_REPLY_RUNNER_A, 1, 1, 1 },
#endif
#if defined G9991
	{ "PM_COUNTERS", 1, COMMON_A_INDEX, &PM_COUNTERS, 1, 1, 1 },
#endif
#if defined G9991
	{ "RUNNER_A_SCRATCHPAD", 1, COMMON_A_INDEX, &RUNNER_A_SCRATCHPAD, 1, 1, 1 },
#endif
#if defined G9991
	{ "CPU_RX_RUNNER_A_SCRATCHPAD", 1, COMMON_A_INDEX, &CPU_RX_RUNNER_A_SCRATCHPAD, 1, 1, 1 },
#endif
#if defined G9991
	{ "GPON_SKB_ENQUEUED_INDEXES_FIFO", 1, COMMON_A_INDEX, &GPON_SKB_ENQUEUED_INDEXES_FIFO, 32, 1, 1 },
#endif
#if defined G9991
	{ "DS_RING_PACKET_DESCRIPTORS_CACHE", 1, COMMON_A_INDEX, &DS_RING_PACKET_DESCRIPTORS_CACHE, 12, 1, 1 },
#endif
#if defined G9991
	{ "CONNECTION_BUFFER_TABLE", 1, COMMON_A_INDEX, &CONNECTION_BUFFER_TABLE, 5, 4, 1 },
#endif
#if defined G9991
	{ "BPM_CONGESTION_CONTROL_TABLE", 1, COMMON_A_INDEX, &BPM_CONGESTION_CONTROL_TABLE, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_DHD_TX_POST_FLOW_RING_MGMT_DESCRIPTOR_BUFFER", 1, COMMON_A_INDEX, &DS_DHD_TX_POST_FLOW_RING_MGMT_DESCRIPTOR_BUFFER, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_DHD_FLOW_RING_CACHE_LKP_TABLE", 1, COMMON_A_INDEX, &DS_DHD_FLOW_RING_CACHE_LKP_TABLE, 16, 1, 1 },
#endif
#if defined G9991
	{ "DHD_FLOW_RING_SHADOW_WR_PTR_TABLE", 1, COMMON_A_INDEX, &DHD_FLOW_RING_SHADOW_WR_PTR_TABLE, 16, 1, 1 },
#endif
#if defined G9991
	{ "DHD_RADIO_INSTANCE_COMMON_A_DATA", 1, COMMON_A_INDEX, &DHD_RADIO_INSTANCE_COMMON_A_DATA, 3, 1, 1 },
#endif
#if defined G9991
	{ "WLAN_MCAST_DHD_STATION_TABLE", 1, COMMON_A_INDEX, &WLAN_MCAST_DHD_STATION_TABLE, 64, 1, 1 },
#endif
#if defined G9991
	{ "WLAN_MCAST_SSID_MAC_ADDRESS_TABLE", 1, COMMON_A_INDEX, &WLAN_MCAST_SSID_MAC_ADDRESS_TABLE, 48, 1, 1 },
#endif
#if defined G9991
	{ "WLAN_MCAST_SSID_STATS_TABLE", 1, COMMON_A_INDEX, &WLAN_MCAST_SSID_STATS_TABLE, 48, 1, 1 },
#endif
#if defined G9991
	{ "BRIDGE_PORT_TO_BBH_PERIPHERAL_RX_TABLE", 1, COMMON_B_INDEX, &BRIDGE_PORT_TO_BBH_PERIPHERAL_RX_TABLE, 7, 1, 1 },
#endif
#if defined G9991
	{ "COMMON_B_DUMMY_STORE", 1, COMMON_B_INDEX, &COMMON_B_DUMMY_STORE, 1, 1, 1 },
#endif
#if defined G9991
	{ "CPU_TX_DESCRIPTOR_QUEUE_TAIL_TABLE", 1, COMMON_B_INDEX, &CPU_TX_DESCRIPTOR_QUEUE_TAIL_TABLE, 4, 1, 1 },
#endif
#if defined G9991
	{ "EGRESS_VLAN_SWITCHING_ISOLATION_CONFIG_TABLE", 1, COMMON_B_INDEX, &EGRESS_VLAN_SWITCHING_ISOLATION_CONFIG_TABLE, 5, 1, 1 },
#endif
#if defined G9991
	{ "RATE_CONTROLLER_EXPONENT_TABLE", 1, COMMON_B_INDEX, &RATE_CONTROLLER_EXPONENT_TABLE, 4, 1, 1 },
#endif
#if defined G9991
	{ "US_RATE_CONTROLLERS_TABLE", 1, COMMON_B_INDEX, &US_RATE_CONTROLLERS_TABLE, 128, 1, 1 },
#endif
#if defined G9991
	{ "WAN_TX_QUEUES_TABLE", 1, COMMON_B_INDEX, &WAN_TX_QUEUES_TABLE, 256, 1, 1 },
#endif
#if defined G9991
	{ "US_CPU_REASON_TO_CPU_RX_QUEUE_TABLE", 1, COMMON_B_INDEX, &US_CPU_REASON_TO_CPU_RX_QUEUE_TABLE, 64, 1, 1 },
#endif
#if defined G9991
	{ "DUAL_STACK_LITE_TABLE", 1, COMMON_B_INDEX, &DUAL_STACK_LITE_TABLE, 4, 1, 1 },
#endif
#if defined G9991
	{ "LAYER2_GRE_TUNNEL_TABLE", 1, COMMON_B_INDEX, &LAYER2_GRE_TUNNEL_TABLE, 12, 1, 1 },
#endif
#if defined G9991
	{ "LOCAL_SWITCHING_PBITS_TO_QOS_TABLE", 1, COMMON_B_INDEX, &LOCAL_SWITCHING_PBITS_TO_QOS_TABLE, 6, 8, 1 },
#endif
#if defined G9991
	{ "DS_INGRESS_CLASSIFICATION_LOOKUP_TABLE", 1, COMMON_B_INDEX, &DS_INGRESS_CLASSIFICATION_LOOKUP_TABLE, 256, 1, 1 },
#endif
#if defined G9991
	{ "US_INGRESS_CLASSIFICATION_LOOKUP_TABLE", 1, COMMON_B_INDEX, &US_INGRESS_CLASSIFICATION_LOOKUP_TABLE, 256, 1, 1 },
#endif
#if defined G9991
	{ "US_INGRESS_CLASSIFICATION_LOOKUP_LONG_TABLE", 1, COMMON_B_INDEX, &US_INGRESS_CLASSIFICATION_LOOKUP_LONG_TABLE, 128, 1, 1 },
#endif
#if defined G9991
	{ "WIFI_SSID_FORWARDING_MATRIX_TABLE", 1, COMMON_B_INDEX, &WIFI_SSID_FORWARDING_MATRIX_TABLE, 16, 1, 1 },
#endif
#if defined G9991
	{ "MULTICAST_ACTIVE_PORTS_TABLE", 1, COMMON_B_INDEX, &MULTICAST_ACTIVE_PORTS_TABLE, 64, 1, 1 },
#endif
#if defined G9991
	{ "SRC_MAC_ANTI_SPOOFING_TABLE", 1, COMMON_B_INDEX, &SRC_MAC_ANTI_SPOOFING_TABLE, 6, 4, 1 },
#endif
#if defined G9991
	{ "WAN_PHYSICAL_PORT", 1, COMMON_B_INDEX, &WAN_PHYSICAL_PORT, 1, 1, 1 },
#endif
#if defined G9991
	{ "BRIDGE_PORT_TO_BROADCOM_SWITCH_PORT_MAPPING_TABLE", 1, COMMON_B_INDEX, &BRIDGE_PORT_TO_BROADCOM_SWITCH_PORT_MAPPING_TABLE, 8, 1, 1 },
#endif
#if defined G9991
	{ "BRIDGE_PORT_TO_BROADCOM_SWITCH_PORT_MAPPING_TABLE_PTR", 1, COMMON_B_INDEX, &BRIDGE_PORT_TO_BROADCOM_SWITCH_PORT_MAPPING_TABLE_PTR, 1, 1, 1 },
#endif
#if defined G9991
	{ "EPON_DDR_CACHE_FIFO", 1, COMMON_B_INDEX, &EPON_DDR_CACHE_FIFO, 192, 1, 1 },
#endif
#if defined G9991
	{ "EPON_DDR_QUEUE_DESCRIPTORS_TABLE", 1, COMMON_B_INDEX, &EPON_DDR_QUEUE_DESCRIPTORS_TABLE, 16, 1, 1 },
#endif
#if defined G9991
	{ "EPON_DDR_QUEUE_ADDRESS_TABLE", 1, COMMON_B_INDEX, &EPON_DDR_QUEUE_ADDRESS_TABLE, 16, 1, 1 },
#endif
#if defined G9991
	{ "GPON_SKB_ENQUEUED_INDEXES_FREE_PTR", 1, COMMON_B_INDEX, &GPON_SKB_ENQUEUED_INDEXES_FREE_PTR, 40, 1, 1 },
#endif
#if defined G9991
	{ "GPON_SKB_ENQUEUED_INDEXES_PUT_PTR", 1, COMMON_B_INDEX, &GPON_SKB_ENQUEUED_INDEXES_PUT_PTR, 40, 1, 1 },
#endif
#if defined G9991
	{ "LOCAL_SWITCHING_LAN_ENQUEUE_INGRESS_QUEUE", 1, COMMON_B_INDEX, &LOCAL_SWITCHING_LAN_ENQUEUE_INGRESS_QUEUE, 16, 1, 1 },
#endif
#if defined G9991
	{ "IP_SYNC_1588_DESCRIPTOR_QUEUE", 1, COMMON_B_INDEX, &IP_SYNC_1588_DESCRIPTOR_QUEUE, 16, 1, 1 },
#endif
#if defined G9991
	{ "BPM_REPLY_RUNNER_B", 1, COMMON_B_INDEX, &BPM_REPLY_RUNNER_B, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_INGRESS_CLASSIFICATION_LOOKUP_CAM_TABLE", 1, COMMON_B_INDEX, &DS_INGRESS_CLASSIFICATION_LOOKUP_CAM_TABLE, 32, 1, 1 },
#endif
#if defined G9991
	{ "US_INGRESS_CLASSIFICATION_LOOKUP_CAM_TABLE", 1, COMMON_B_INDEX, &US_INGRESS_CLASSIFICATION_LOOKUP_CAM_TABLE, 32, 1, 1 },
#endif
#if defined G9991
	{ "RUNNER_B_SCRATCHPAD", 1, COMMON_B_INDEX, &RUNNER_B_SCRATCHPAD, 1, 1, 1 },
#endif
#if defined G9991
	{ "CPU_RX_RUNNER_B_SCRATCHPAD", 1, COMMON_B_INDEX, &CPU_RX_RUNNER_B_SCRATCHPAD, 1, 1, 1 },
#endif
#if defined G9991
	{ "DUMMY_WAN_TX_QUEUE_DESCRIPTOR", 1, COMMON_B_INDEX, &DUMMY_WAN_TX_QUEUE_DESCRIPTOR, 1, 1, 1 },
#endif
#if defined G9991
	{ "CPU_TX_EMAC_LOOPBACK_QUEUE", 1, COMMON_B_INDEX, &CPU_TX_EMAC_LOOPBACK_QUEUE, 4, 1, 1 },
#endif
#if defined G9991
	{ "CPU_TX_US_FLOODING_QUEUE", 1, COMMON_B_INDEX, &CPU_TX_US_FLOODING_QUEUE, 4, 1, 1 },
#endif
#if defined G9991
	{ "CPU_TX_PICO_US_FLOODING_QUEUE_PTR", 1, COMMON_B_INDEX, &CPU_TX_PICO_US_FLOODING_QUEUE_PTR, 1, 1, 1 },
#endif
#if defined G9991
	{ "DUMMY_RATE_CONTROLLER_DESCRIPTOR", 1, COMMON_B_INDEX, &DUMMY_RATE_CONTROLLER_DESCRIPTOR, 1, 1, 1 },
#endif
#if defined G9991
	{ "US_RING_PACKET_DESCRIPTORS_CACHE", 1, COMMON_B_INDEX, &US_RING_PACKET_DESCRIPTORS_CACHE, 12, 1, 1 },
#endif
#if defined G9991
	{ "LAN0_INGRESS_FIFO", 1, COMMON_B_INDEX, &LAN0_INGRESS_FIFO, 1, 1, 1 },
#endif
#if defined G9991
	{ "LAN1_INGRESS_FIFO", 1, COMMON_B_INDEX, &LAN1_INGRESS_FIFO, 1, 1, 1 },
#endif
#if defined G9991
	{ "LAN2_INGRESS_FIFO", 1, COMMON_B_INDEX, &LAN2_INGRESS_FIFO, 1, 1, 1 },
#endif
#if defined G9991
	{ "LAN3_INGRESS_FIFO", 1, COMMON_B_INDEX, &LAN3_INGRESS_FIFO, 1, 1, 1 },
#endif
#if defined G9991
	{ "LAN4_INGRESS_FIFO", 1, COMMON_B_INDEX, &LAN4_INGRESS_FIFO, 1, 1, 1 },
#endif
#if defined G9991
	{ "WAN_ENQUEUE_INGRESS_QUEUE", 1, COMMON_B_INDEX, &WAN_ENQUEUE_INGRESS_QUEUE, 64, 1, 1 },
#endif
#if defined G9991
	{ "SPEED_SERVICE_STREAM_PREFIX", 1, COMMON_B_INDEX, &SPEED_SERVICE_STREAM_PREFIX, 1, 1, 1 },
#endif
#if defined G9991
	{ "SPEED_SERVICE_RX_TIMESTAMPS_TABLE", 1, COMMON_B_INDEX, &SPEED_SERVICE_RX_TIMESTAMPS_TABLE, 1, 1, 1 },
#endif
#if defined G9991
	{ "US_DHD_TX_POST_FLOW_RING_MGMT_DESCRIPTOR_BUFFER", 1, COMMON_B_INDEX, &US_DHD_TX_POST_FLOW_RING_MGMT_DESCRIPTOR_BUFFER, 1, 1, 1 },
#endif
#if defined G9991
	{ "US_DHD_FLOW_RING_CACHE_LKP_TABLE", 1, COMMON_B_INDEX, &US_DHD_FLOW_RING_CACHE_LKP_TABLE, 16, 1, 1 },
#endif
#if defined G9991
	{ "DHD_FLOW_RING_CACHE_CTX_TABLE", 1, COMMON_B_INDEX, &DHD_FLOW_RING_CACHE_CTX_TABLE, 16, 1, 1 },
#endif
#if defined G9991
	{ "DHD_FLOW_RING_CACHE_CTX_NEXT_INDEX", 1, COMMON_B_INDEX, &DHD_FLOW_RING_CACHE_CTX_NEXT_INDEX, 1, 1, 1 },
#endif
#if defined G9991
	{ "DHD_RADIO_INSTANCE_COMMON_B_DATA", 1, COMMON_B_INDEX, &DHD_RADIO_INSTANCE_COMMON_B_DATA, 3, 1, 1 },
#endif
#if defined G9991
	{ "IPTV_DDR_LOOKUP_TABLE", 1, DDR_INDEX, &IPTV_DDR_LOOKUP_TABLE, 8192, 1, 1 },
#endif
#if defined G9991
	{ "IPTV_DDR_CONTEXT_TABLE", 1, DDR_INDEX, &IPTV_DDR_CONTEXT_TABLE, 8192, 1, 1 },
#endif
#if defined G9991
	{ "IPTV_SSM_DDR_CONTEXT_TABLE", 1, DDR_INDEX, &IPTV_SSM_DDR_CONTEXT_TABLE, 32768, 1, 1 },
#endif
#if defined G9991
	{ "FIREWALL_RULES_MAP_TABLE", 1, DDR_INDEX, &FIREWALL_RULES_MAP_TABLE, 8, 2, 65536 },
#endif
#if defined G9991
	{ "FIREWALL_RULES_TABLE", 1, DDR_INDEX, &FIREWALL_RULES_TABLE, 256, 1, 1 },
#endif
#if defined G9991
	{ "DS_CONNECTION_TABLE", 1, DDR_INDEX, &DS_CONNECTION_TABLE, 32768, 1, 1 },
#endif
#if defined G9991
	{ "US_CONNECTION_TABLE", 1, DDR_INDEX, &US_CONNECTION_TABLE, 32768, 1, 1 },
#endif
#if defined G9991
	{ "CONTEXT_TABLE", 1, DDR_INDEX, &CONTEXT_TABLE, 16512, 1, 1 },
#endif
#if defined G9991
	{ "DDR_ADDRESS_FOR_SYNC_DMA", 1, DDR_INDEX, &DDR_ADDRESS_FOR_SYNC_DMA, 1, 1, 1 },
#endif
#if defined G9991
	{ "EPON_TX_POST_SCHEDULING_DDR_QUEUES", 1, DDR_INDEX, &EPON_TX_POST_SCHEDULING_DDR_QUEUES, 65536, 1, 1 },
#endif
#if defined G9991
	{ "DHD_TX_POST_FLOW_RING_MGMT_DESCRIPTOR_TABLE", 1, DDR_INDEX, &DHD_TX_POST_FLOW_RING_MGMT_DESCRIPTOR_TABLE, 8, 1, 1 },
#endif
#if defined G9991
	{ "DHD_RX_POST_DDR_BUFFER", 1, DDR_INDEX, &DHD_RX_POST_DDR_BUFFER, 1024, 1, 1 },
#endif
#if defined G9991
	{ "DHD_RX_COMPLETE_DDR_BUFFER", 1, DDR_INDEX, &DHD_RX_COMPLETE_DDR_BUFFER, 1024, 1, 1 },
#endif
#if defined G9991
	{ "DHD_TX_POST_DDR_BUFFER", 1, DDR_INDEX, &DHD_TX_POST_DDR_BUFFER, 8, 16, 1 },
#endif
#if defined G9991
	{ "DHD_TX_COMPLETE_DDR_BUFFER", 1, DDR_INDEX, &DHD_TX_COMPLETE_DDR_BUFFER, 16, 1, 1 },
#endif
#if defined G9991
	{ "R2D_WR_ARR_DDR_BUFFER", 1, DDR_INDEX, &R2D_WR_ARR_DDR_BUFFER, 128, 1, 1 },
#endif
#if defined G9991
	{ "D2R_RD_ARR_DDR_BUFFER", 1, DDR_INDEX, &D2R_RD_ARR_DDR_BUFFER, 128, 1, 1 },
#endif
#if defined G9991
	{ "R2D_RD_ARR_DDR_BUFFER", 1, DDR_INDEX, &R2D_RD_ARR_DDR_BUFFER, 128, 1, 1 },
#endif
#if defined G9991
	{ "D2R_WR_ARR_DDR_BUFFER", 1, DDR_INDEX, &D2R_WR_ARR_DDR_BUFFER, 128, 1, 1 },
#endif
#if defined G9991
	{ "WLAN_MCAST_DHD_LIST_TABLE", 1, DDR_INDEX, &WLAN_MCAST_DHD_LIST_TABLE, 64, 1, 1 },
#endif
#if defined G9991
	{ "WLAN_MCAST_DHD_LIST_FORMAT_TABLE", 1, DDR_INDEX, &WLAN_MCAST_DHD_LIST_FORMAT_TABLE, 1, 1, 1 },
#endif
#if defined G9991
	{ "DS_INGRESS_CLASSIFICATION_COUNTERS_TABLE", 1, PSRAM_INDEX, &DS_INGRESS_CLASSIFICATION_COUNTERS_TABLE, 256, 1, 1 },
#endif
#if defined G9991
	{ "US_INGRESS_CLASSIFICATION_COUNTERS_TABLE", 1, PSRAM_INDEX, &US_INGRESS_CLASSIFICATION_COUNTERS_TABLE, 256, 1, 1 },
#endif
};

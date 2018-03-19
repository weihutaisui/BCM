/*
    <:copyright-BRCM:2015:DUAL/GPL:standard

       Copyright (c) 2015 Broadcom
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

#include "rdd.h"
#include "rdd_defs.h"
#include "rdd_init.h"

#include "XRDP_AG.h"
#include "rdd_map_auto.h"
#include "rdd_scheduling.h"
#include "rdd_common.h"
#include "rdd_cpu.h"
#include "rdp_common.h"
#include "rdd_runner_proj_defs.h"
#include "xrdp_drv_rnr_regs_ag.h"

extern uintptr_t xrdp_virt2phys(const ru_block_rec *ru_block, uint8_t addr_idx);
static void rdd_proj_init(rdd_init_params_t *init_params);
static void rdd_tm_actions_proj_init(void);

#ifdef RDP_SIM
extern uint8_t *soc_base_address;
extern uint32_t natc_lkp_table_ptr;
extern int rdd_sim_alloc_segments(void);
extern void rdd_sim_free_segments(void);
#endif

extern int reg_id[32];

#ifdef USE_BDMF_SHELL
extern int rdd_make_shell_commands(void);
#endif /* USE_BDMF_SHELL */

rdd_bb_id rdpa_emac_to_bb_id_rx[rdpa_emac__num_of] = {
    BB_ID_RX_XLMAC1_0_RGMII,
    BB_ID_RX_XLMAC0_1_2P5G,
    BB_ID_RX_XLMAC0_2_1G,
    BB_ID_RX_XLMAC0_3_1G,
    BB_ID_RX_XLMAC0_0_10G,
    BB_ID_RX_XLMAC1_1_RGMII,
    BB_ID_RX_XLMAC1_2_RGMII,
    BB_ID_LAST,
};

rdd_bb_id rdpa_emac_to_bb_id_tx[rdpa_emac__num_of] = {
    BB_ID_TX_XLMAC1_0_RGMII,
    BB_ID_TX_XLMAC0_1_2P5G,
    BB_ID_TX_XLMAC0_2_1G,
    BB_ID_TX_XLMAC0_3_1G,
    BB_ID_TX_XLMAC0_0_10G,
    BB_ID_TX_XLMAC1_1_RGMII,
    BB_ID_TX_XLMAC1_2_RGMII,
    BB_ID_LAST,
};

bbh_id_e rdpa_emac_to_bbh_id_e[rdpa_emac__num_of] = {
    BBH_ID_XLMAC1_0_RGMII,
    BBH_ID_XLMAC0_1_2p5G,
    BBH_ID_XLMAC0_2_1G,
    BBH_ID_XLMAC0_3_1G,
    BBH_ID_XLMAC0_0_10G,
    BBH_ID_XLMAC1_1_RGMII,
    BBH_ID_XLMAC1_2_RGMII,
    BBH_ID_NULL,
};

#ifndef G9991
rdd_vport_id_t rx_flow_to_vport[RX_FLOW_CONTEXTS_NUMBER] = {
    [RDD_NUM_OF_WAN_FLOWS + BB_ID_RX_XLMAC1_0_RGMII] = RDD_LAN0_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + BB_ID_RX_XLMAC0_1_2P5G] = RDD_LAN1_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + BB_ID_RX_XLMAC0_2_1G] = RDD_LAN2_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + BB_ID_RX_XLMAC0_3_1G] = RDD_LAN3_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + BB_ID_RX_XLMAC0_0_10G] = RDD_LAN4_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + BB_ID_RX_XLMAC1_1_RGMII] = RDD_LAN5_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + BB_ID_RX_XLMAC1_2_RGMII] = RDD_LAN6_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + BB_ID_CPU0] = RDD_CPU0_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + BB_ID_CPU1] = RDD_CPU1_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + BB_ID_CPU2] = RDD_CPU2_VPORT,
};
#else
rdd_vport_id_t rx_flow_to_vport[RX_FLOW_CONTEXTS_NUMBER] = {
    [RDD_NUM_OF_WAN_FLOWS + RDD_LAN0_VPORT ] = RDD_LAN0_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + RDD_LAN1_VPORT ] = RDD_LAN1_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + RDD_LAN2_VPORT ] = RDD_LAN2_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + RDD_LAN3_VPORT ] = RDD_LAN3_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + RDD_LAN4_VPORT ] = RDD_LAN4_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + RDD_LAN5_VPORT ] = RDD_LAN5_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + RDD_LAN6_VPORT ] = RDD_LAN6_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + RDD_LAN7_VPORT ] = RDD_LAN7_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + RDD_LAN8_VPORT ] = RDD_LAN8_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + RDD_LAN9_VPORT ] = RDD_LAN9_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + RDD_LAN10_VPORT ] = RDD_LAN10_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + RDD_LAN11_VPORT ] = RDD_LAN11_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + RDD_LAN12_VPORT ] = RDD_LAN12_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + RDD_LAN13_VPORT ] = RDD_LAN13_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + RDD_LAN14_VPORT ] = RDD_LAN14_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + RDD_LAN15_VPORT ] = RDD_LAN15_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + RDD_LAN16_VPORT ] = RDD_LAN16_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + RDD_LAN17_VPORT ] = RDD_LAN17_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + RDD_LAN18_VPORT ] = RDD_LAN18_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + RDD_LAN19_VPORT ] = RDD_LAN19_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + RDD_LAN20_VPORT ] = RDD_LAN20_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + RDD_LAN21_VPORT ] = RDD_LAN21_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + RDD_LAN22_VPORT ] = RDD_LAN22_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + RDD_LAN23_VPORT ] = RDD_LAN23_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + RDD_LAN24_VPORT ] = RDD_LAN24_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + RDD_LAN25_VPORT ] = RDD_LAN25_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + RDD_LAN26_VPORT ] = RDD_LAN26_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + RDD_LAN27_VPORT ] = RDD_LAN27_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + RDD_LAN28_VPORT ] = RDD_LAN28_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + RDD_LAN29_VPORT ] = RDD_LAN29_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + RDD_LAN30_VPORT ] = RDD_LAN30_VPORT,
    [RDD_NUM_OF_WAN_FLOWS + RDD_LAN31_VPORT ] = RDD_LAN31_VPORT,
};
#endif

rdpa_emac bbh_id_to_rdpa_emac[BBH_ID_NUM] = {
    rdpa_emac4,
    rdpa_emac1,
    rdpa_emac2,
    rdpa_emac3,
    rdpa_emac0,
    rdpa_emac5,
    rdpa_emac6,
    rdpa_emac7,
    rdpa_emac_none
};

extern RDD_FPM_GLOBAL_CFG_DTS g_fpm_hw_cfg;

int rdd_init(void)
{
#ifdef RDP_SIM
    if (rdd_sim_alloc_segments())
        return -1;
#endif
    return 0;
}

void rdd_exit(void)
{
#ifdef RDP_SIM
    rdd_sim_free_segments();
#endif
}

#ifndef RDP_SIM
void rdp_rnr_write_context(void *__to, void *__from, unsigned int __n);
#endif

static void rdd_global_registers_init(uint32_t core_index)
{
    static uint32_t global_regs[8] = {};
    uint32_t i;

    /********** common to all runners **********/
    global_regs[1] = 1; /* R1 = 1 */

    //global_regs[2] = RDD_CPU_RECYCLE_SRAM_PD_FIFO_ADDRESS_ARR[core_index]; /* See usage in fw_runner_defs.h */
    //global_regs[4] = RDD_VPORT_CFG_TABLE_ADDRESS_ARR[core_index];

    for (i = 0; i < 8; ++i)
        RDD_BYTES_4_BITS_WRITE(global_regs[i], (uint8_t *)RDD_RUNNER_GLOBAL_REGISTERS_INIT_PTR(core_index) + (sizeof(RDD_BYTES_4_DTS) * i));
}

static void image_0_context_set(uint32_t core_index, rdd_init_params_t *init_params)
{
    static uint32_t local_regs[NUM_OF_MAIN_RUNNER_THREADS][32];
    uint32_t mem_cntxt_byte_num;
    uint32_t *sram_context;

    rdd_global_registers_init(core_index);
    sram_context = (uint32_t *)RUNNER_CORE_CONTEXT_ADDRESS(core_index);
    mem_cntxt_byte_num = (RU_REG_RAM_CNT(RNR_CNTXT, MEM_ENTRY) + 1) * sizeof(uint32_t);
#ifndef XRDP_EMULATION
    /* read the local registers from the Context memory - maybe it was initialized by the ACE compiler */
    MREAD_BLK_32(local_regs, sram_context, mem_cntxt_byte_num);
#endif
#ifndef G9991

    /* UPDATE_FIFO_READ: thread 1 */
    local_regs[IMAGE_0_DS_TM_UPDATE_FIFO_THREAD_NUMBER][reg_id[16]] = ADDRESS_OF(image_0, update_fifo_read_1st_wakeup_request) << 16;
    local_regs[IMAGE_0_DS_TM_UPDATE_FIFO_THREAD_NUMBER][reg_id[14]] = IMAGE_0_COMPLEX_SCHEDULER_TABLE_ADDRESS | (IMAGE_0_DS_TM_SCHEDULING_QUEUE_AGING_VECTOR_ADDRESS << 16);
    local_regs[IMAGE_0_DS_TM_UPDATE_FIFO_THREAD_NUMBER][reg_id[12]] = (IMAGE_0_DS_TM_PD_FIFO_TABLE_ADDRESS << 16);
    local_regs[IMAGE_0_DS_TM_UPDATE_FIFO_THREAD_NUMBER][reg_id[11]] = IMAGE_0_UPDATE_FIFO_TABLE_ADDRESS;
    local_regs[IMAGE_0_DS_TM_UPDATE_FIFO_THREAD_NUMBER][reg_id[10]] = BB_ID_QM_RNR_GRID | (IMAGE_0_DS_TM_BBH_QUEUE_TABLE_ADDRESS << 16);
    local_regs[IMAGE_0_DS_TM_UPDATE_FIFO_THREAD_NUMBER][reg_id[9]]  = IMAGE_0_DS_TM_SCHEDULING_QUEUE_TABLE_ADDRESS | (IMAGE_0_BASIC_SCHEDULER_TABLE_DS_ADDRESS << 16);

    /* FLUSH TASK: thread 2 */
    local_regs[IMAGE_0_DS_TM_FLUSH_THREAD_NUMBER][reg_id[16]] = ADDRESS_OF(image_0, flush_task_1st_wakeup_request) << 16;
    local_regs[IMAGE_0_DS_TM_FLUSH_THREAD_NUMBER][reg_id[12]] = IMAGE_0_DS_TM_PD_FIFO_TABLE_ADDRESS;
    local_regs[IMAGE_0_DS_TM_FLUSH_THREAD_NUMBER][reg_id[11]] = IMAGE_0_DS_TM_SCHEDULING_QUEUE_AGING_VECTOR_ADDRESS;
    local_regs[IMAGE_0_DS_TM_FLUSH_THREAD_NUMBER][reg_id[10]] = (BB_ID_DISPATCHER_REORDER + (DISP_REOR_VIQ_DS_TM_FLUSH << 6));
    local_regs[IMAGE_0_DS_TM_FLUSH_THREAD_NUMBER][reg_id[9]] = IMAGE_0_DS_TM_SCHEDULING_AGGREGATION_CONTEXT_VECTOR_ADDRESS;
    local_regs[IMAGE_0_DS_TM_FLUSH_THREAD_NUMBER][reg_id[8]] = xrdp_virt2phys(&RU_BLK(QM), 0) + RU_REG_OFFSET(QM, GLOBAL_CFG_AGGREGATION_CONTEXT_VALID) +
        (QM_QUEUE_DS_START / 8);

    /* SCHEDULING LAN 0: thread 4 */
    local_regs[IMAGE_0_DS_TM_LAN0_THREAD_NUMBER][reg_id[16]] = ADDRESS_OF(image_0, tx_task_1st_wakeup_request) << 16;
    local_regs[IMAGE_0_DS_TM_LAN0_THREAD_NUMBER][reg_id[12]] = (IMAGE_0_DS_TM_BBH_TX_EGRESS_COUNTER_TABLE_ADDRESS << 16);
    local_regs[IMAGE_0_DS_TM_LAN0_THREAD_NUMBER][reg_id[11]] = BB_ID_TX_XLMAC1_0_RGMII | (IMAGE_0_COMPLEX_SCHEDULER_TABLE_ADDRESS << 16);
    local_regs[IMAGE_0_DS_TM_LAN0_THREAD_NUMBER][reg_id[10]] = IMAGE_0_DS_TM_PD_FIFO_TABLE_ADDRESS | (IMAGE_0_BASIC_RATE_LIMITER_TABLE_DS_ADDRESS << 16);
    local_regs[IMAGE_0_DS_TM_LAN0_THREAD_NUMBER][reg_id[9]] = (IMAGE_0_DS_TM_SCHEDULING_QUEUE_TABLE_ADDRESS << 16);
    local_regs[IMAGE_0_DS_TM_LAN0_THREAD_NUMBER][reg_id[8]]  = IMAGE_0_DS_TM_BBH_QUEUE_TABLE_ADDRESS | (IMAGE_0_BASIC_SCHEDULER_TABLE_DS_ADDRESS << 16);

    /* SCHEDULING LAN 1: thread 5 */
    local_regs[IMAGE_0_DS_TM_LAN1_THREAD_NUMBER][reg_id[16]] = ADDRESS_OF(image_0, tx_task_1st_wakeup_request) << 16;
    local_regs[IMAGE_0_DS_TM_LAN1_THREAD_NUMBER][reg_id[12]] = ((IMAGE_0_DS_TM_BBH_TX_EGRESS_COUNTER_TABLE_ADDRESS + 1*sizeof(RDD_BBH_TX_EGRESS_COUNTER_ENTRY_DTS)) << 16);
    local_regs[IMAGE_0_DS_TM_LAN1_THREAD_NUMBER][reg_id[11]] = BB_ID_TX_XLMAC0_1_2P5G | (IMAGE_0_COMPLEX_SCHEDULER_TABLE_ADDRESS << 16);
    local_regs[IMAGE_0_DS_TM_LAN1_THREAD_NUMBER][reg_id[10]] = IMAGE_0_DS_TM_PD_FIFO_TABLE_ADDRESS | (IMAGE_0_BASIC_RATE_LIMITER_TABLE_DS_ADDRESS << 16);
    local_regs[IMAGE_0_DS_TM_LAN1_THREAD_NUMBER][reg_id[9]]  = (IMAGE_0_DS_TM_SCHEDULING_QUEUE_TABLE_ADDRESS << 16);
    local_regs[IMAGE_0_DS_TM_LAN1_THREAD_NUMBER][reg_id[8]]  = (IMAGE_0_DS_TM_BBH_QUEUE_TABLE_ADDRESS + sizeof(RDD_BBH_QUEUE_DESCRIPTOR_DTS))
        | (IMAGE_0_BASIC_SCHEDULER_TABLE_DS_ADDRESS << 16);

    /* SCHEDULING LAN 2: thread 6 */
    local_regs[IMAGE_0_DS_TM_LAN2_THREAD_NUMBER][reg_id[16]] = ADDRESS_OF(image_0, tx_task_1st_wakeup_request) << 16;
    local_regs[IMAGE_0_DS_TM_LAN2_THREAD_NUMBER][reg_id[12]] = ((IMAGE_0_DS_TM_BBH_TX_EGRESS_COUNTER_TABLE_ADDRESS + 2*sizeof(RDD_BBH_TX_EGRESS_COUNTER_ENTRY_DTS)) << 16);
    local_regs[IMAGE_0_DS_TM_LAN2_THREAD_NUMBER][reg_id[11]] = BB_ID_TX_XLMAC0_2_1G | (IMAGE_0_COMPLEX_SCHEDULER_TABLE_ADDRESS << 16);
    local_regs[IMAGE_0_DS_TM_LAN2_THREAD_NUMBER][reg_id[10]] = IMAGE_0_DS_TM_PD_FIFO_TABLE_ADDRESS | (IMAGE_0_BASIC_RATE_LIMITER_TABLE_DS_ADDRESS << 16);
    local_regs[IMAGE_0_DS_TM_LAN2_THREAD_NUMBER][reg_id[9]]  = (IMAGE_0_DS_TM_SCHEDULING_QUEUE_TABLE_ADDRESS << 16);
    local_regs[IMAGE_0_DS_TM_LAN2_THREAD_NUMBER][reg_id[8]]  = (IMAGE_0_DS_TM_BBH_QUEUE_TABLE_ADDRESS + (2 * sizeof(RDD_BBH_QUEUE_DESCRIPTOR_DTS)))
        | (IMAGE_0_BASIC_SCHEDULER_TABLE_DS_ADDRESS << 16);

    /* SCHEDULING LAN 3: thread 7 */
    local_regs[IMAGE_0_DS_TM_LAN3_THREAD_NUMBER][reg_id[16]] = ADDRESS_OF(image_0, tx_task_1st_wakeup_request) << 16;
    local_regs[IMAGE_0_DS_TM_LAN3_THREAD_NUMBER][reg_id[12]] = ((IMAGE_0_DS_TM_BBH_TX_EGRESS_COUNTER_TABLE_ADDRESS + 3*sizeof(RDD_BBH_TX_EGRESS_COUNTER_ENTRY_DTS)) << 16);
    local_regs[IMAGE_0_DS_TM_LAN3_THREAD_NUMBER][reg_id[11]] = BB_ID_TX_XLMAC0_3_1G | (IMAGE_0_COMPLEX_SCHEDULER_TABLE_ADDRESS << 16);
    local_regs[IMAGE_0_DS_TM_LAN3_THREAD_NUMBER][reg_id[10]] = IMAGE_0_DS_TM_PD_FIFO_TABLE_ADDRESS | (IMAGE_0_BASIC_RATE_LIMITER_TABLE_DS_ADDRESS << 16);
    local_regs[IMAGE_0_DS_TM_LAN3_THREAD_NUMBER][reg_id[9]]  = (IMAGE_0_DS_TM_SCHEDULING_QUEUE_TABLE_ADDRESS << 16);
    local_regs[IMAGE_0_DS_TM_LAN3_THREAD_NUMBER][reg_id[8]]  = (IMAGE_0_DS_TM_BBH_QUEUE_TABLE_ADDRESS + (3 * sizeof(RDD_BBH_QUEUE_DESCRIPTOR_DTS)))
        | (IMAGE_0_BASIC_SCHEDULER_TABLE_DS_ADDRESS << 16);

    /* SCHEDULING LAN 4: thread 8 */
    local_regs[IMAGE_0_DS_TM_LAN4_THREAD_NUMBER][reg_id[16]] = ADDRESS_OF(image_0, tx_task_1st_wakeup_request) << 16;
    local_regs[IMAGE_0_DS_TM_LAN4_THREAD_NUMBER][reg_id[12]] = ((IMAGE_0_DS_TM_BBH_TX_EGRESS_COUNTER_TABLE_ADDRESS + 4*sizeof(RDD_BBH_TX_EGRESS_COUNTER_ENTRY_DTS)) << 16);
    local_regs[IMAGE_0_DS_TM_LAN4_THREAD_NUMBER][reg_id[11]] = BB_ID_TX_XLMAC0_0_10G | (IMAGE_0_COMPLEX_SCHEDULER_TABLE_ADDRESS << 16);
    local_regs[IMAGE_0_DS_TM_LAN4_THREAD_NUMBER][reg_id[10]] = IMAGE_0_DS_TM_PD_FIFO_TABLE_ADDRESS | (IMAGE_0_BASIC_RATE_LIMITER_TABLE_DS_ADDRESS << 16);
    local_regs[IMAGE_0_DS_TM_LAN4_THREAD_NUMBER][reg_id[9]]  = (IMAGE_0_DS_TM_SCHEDULING_QUEUE_TABLE_ADDRESS << 16);
    local_regs[IMAGE_0_DS_TM_LAN4_THREAD_NUMBER][reg_id[8]]  = (IMAGE_0_DS_TM_BBH_QUEUE_TABLE_ADDRESS + (4 * sizeof(RDD_BBH_QUEUE_DESCRIPTOR_DTS)))
        | (IMAGE_0_BASIC_SCHEDULER_TABLE_DS_ADDRESS << 16);

    /* SCHEDULING LAN 5: thread 9 */
    local_regs[IMAGE_0_DS_TM_LAN5_THREAD_NUMBER][reg_id[16]] = ADDRESS_OF(image_0, tx_task_1st_wakeup_request) << 16;
    local_regs[IMAGE_0_DS_TM_LAN5_THREAD_NUMBER][reg_id[12]] = ((IMAGE_0_DS_TM_BBH_TX_EGRESS_COUNTER_TABLE_ADDRESS + 5*sizeof(RDD_BBH_TX_EGRESS_COUNTER_ENTRY_DTS)) << 16);
    local_regs[IMAGE_0_DS_TM_LAN5_THREAD_NUMBER][reg_id[11]] = BB_ID_TX_XLMAC1_1_RGMII | (IMAGE_0_COMPLEX_SCHEDULER_TABLE_ADDRESS << 16);
    local_regs[IMAGE_0_DS_TM_LAN5_THREAD_NUMBER][reg_id[10]] = IMAGE_0_DS_TM_PD_FIFO_TABLE_ADDRESS | (IMAGE_0_BASIC_RATE_LIMITER_TABLE_DS_ADDRESS << 16);
    local_regs[IMAGE_0_DS_TM_LAN5_THREAD_NUMBER][reg_id[9]]  = (IMAGE_0_DS_TM_SCHEDULING_QUEUE_TABLE_ADDRESS << 16);
    local_regs[IMAGE_0_DS_TM_LAN5_THREAD_NUMBER][reg_id[8]]  = (IMAGE_0_DS_TM_BBH_QUEUE_TABLE_ADDRESS + (5 * sizeof(RDD_BBH_QUEUE_DESCRIPTOR_DTS)))
        | (IMAGE_0_BASIC_SCHEDULER_TABLE_DS_ADDRESS << 16);

    /* SCHEDULING LAN 6: thread 10 */
    local_regs[IMAGE_0_DS_TM_LAN6_THREAD_NUMBER][reg_id[16]] = ADDRESS_OF(image_0, tx_task_1st_wakeup_request) << 16;
    local_regs[IMAGE_0_DS_TM_LAN6_THREAD_NUMBER][reg_id[12]] = ((IMAGE_0_DS_TM_BBH_TX_EGRESS_COUNTER_TABLE_ADDRESS + 6*sizeof(RDD_BBH_TX_EGRESS_COUNTER_ENTRY_DTS)) << 16);
    local_regs[IMAGE_0_DS_TM_LAN6_THREAD_NUMBER][reg_id[11]] = BB_ID_TX_XLMAC1_2_RGMII | (IMAGE_0_COMPLEX_SCHEDULER_TABLE_ADDRESS << 16);
    local_regs[IMAGE_0_DS_TM_LAN6_THREAD_NUMBER][reg_id[10]] = IMAGE_0_DS_TM_PD_FIFO_TABLE_ADDRESS | (IMAGE_0_BASIC_RATE_LIMITER_TABLE_DS_ADDRESS << 16);
    local_regs[IMAGE_0_DS_TM_LAN6_THREAD_NUMBER][reg_id[9]]  = (IMAGE_0_DS_TM_SCHEDULING_QUEUE_TABLE_ADDRESS << 16);
    local_regs[IMAGE_0_DS_TM_LAN6_THREAD_NUMBER][reg_id[8]]  = (IMAGE_0_DS_TM_BBH_QUEUE_TABLE_ADDRESS + (6 * sizeof(RDD_BBH_QUEUE_DESCRIPTOR_DTS)))
        | (IMAGE_0_BASIC_SCHEDULER_TABLE_DS_ADDRESS << 16);
#else
    /* G9991 FLOW CONTROL: thread 0 */
    local_regs[IMAGE_0_DS_TM_FLOW_CONTROL_THREAD_NUMBER][reg_id[16]] = ADDRESS_OF(image_0, g9991_flow_control_wakeup_request) << 16;
    local_regs[IMAGE_0_DS_TM_FLOW_CONTROL_THREAD_NUMBER][reg_id[15]] = IMAGE_0_DS_TM_FLOW_CONTROL_THREAD_NUMBER;
    local_regs[IMAGE_0_DS_TM_FLOW_CONTROL_THREAD_NUMBER][reg_id[14]] = BB_ID_SBPM;
    local_regs[IMAGE_0_DS_TM_FLOW_CONTROL_THREAD_NUMBER][reg_id[13]] = BB_ID_DISPATCHER_REORDER;
    local_regs[IMAGE_0_DS_TM_FLOW_CONTROL_THREAD_NUMBER][reg_id[12]] = IMAGE_0_G9991_FLOW_CONTROL_PACKET_HEDAER_ADDRESS;

    /* Budget allocation: thread 1 */
    local_regs[IMAGE_0_DS_TM_BUDGET_ALLOCATION_THREAD_NUMBER][reg_id[16]] = ADDRESS_OF(image_0, budget_allocator_1st_wakeup_request) << 16;
    local_regs[IMAGE_0_DS_TM_BUDGET_ALLOCATION_THREAD_NUMBER][reg_id[13]] = (RDD_BASIC_RATE_LIMITER_TABLE_SIZE / 4) * sizeof(RDD_BASIC_RATE_LIMITER_DESCRIPTOR_DTS);
    local_regs[IMAGE_0_DS_TM_BUDGET_ALLOCATION_THREAD_NUMBER][reg_id[12]] = IMAGE_0_DS_TM_SCHEDULING_QUEUE_TABLE_ADDRESS | (IMAGE_0_COMPLEX_SCHEDULER_TABLE_ADDRESS << 16);
    local_regs[IMAGE_0_DS_TM_BUDGET_ALLOCATION_THREAD_NUMBER][reg_id[10]] = IMAGE_0_RATE_LIMITER_VALID_TABLE_DS_ADDRESS;
    local_regs[IMAGE_0_DS_TM_BUDGET_ALLOCATION_THREAD_NUMBER][reg_id[9]]  = IMAGE_0_BASIC_RATE_LIMITER_TABLE_DS_ADDRESS | (IMAGE_0_BASIC_SCHEDULER_TABLE_DS_ADDRESS << 16);

    /* UPDATE_FIFO_READ: thread 2 */
    local_regs[IMAGE_0_DS_TM_UPDATE_FIFO_THREAD_NUMBER][reg_id[16]] = ADDRESS_OF(image_0, update_fifo_read_1st_wakeup_request) << 16;
    local_regs[IMAGE_0_DS_TM_UPDATE_FIFO_THREAD_NUMBER][reg_id[14]] = IMAGE_0_COMPLEX_SCHEDULER_TABLE_ADDRESS | (IMAGE_0_DS_TM_SCHEDULING_QUEUE_AGING_VECTOR_ADDRESS << 16);
    local_regs[IMAGE_0_DS_TM_UPDATE_FIFO_THREAD_NUMBER][reg_id[13]] = (((IMAGE_0_DS_TM_SCHEDULING_TASK_THREAD_NUMBER) << 4) + 1);
    local_regs[IMAGE_0_DS_TM_UPDATE_FIFO_THREAD_NUMBER][reg_id[11]] = IMAGE_0_UPDATE_FIFO_TABLE_ADDRESS;
    local_regs[IMAGE_0_DS_TM_UPDATE_FIFO_THREAD_NUMBER][reg_id[10]] = BB_ID_QM_RNR_GRID | (IMAGE_0_DS_TM_BBH_QUEUE_TABLE_ADDRESS << 16);
    local_regs[IMAGE_0_DS_TM_UPDATE_FIFO_THREAD_NUMBER][reg_id[9]]  = IMAGE_0_DS_TM_SCHEDULING_QUEUE_TABLE_ADDRESS | (IMAGE_0_BASIC_SCHEDULER_TABLE_DS_ADDRESS << 16);

    /* FLUSH TASK: thread 3 */
    local_regs[IMAGE_0_DS_TM_FLUSH_THREAD_NUMBER][reg_id[16]] = ADDRESS_OF(image_0, flush_task_1st_wakeup_request) << 16;
    local_regs[IMAGE_0_DS_TM_FLUSH_THREAD_NUMBER][reg_id[12]] = IMAGE_0_DS_TM_PD_FIFO_TABLE_ADDRESS;
    local_regs[IMAGE_0_DS_TM_FLUSH_THREAD_NUMBER][reg_id[11]] = IMAGE_0_DS_TM_SCHEDULING_QUEUE_AGING_VECTOR_ADDRESS;
    local_regs[IMAGE_0_DS_TM_FLUSH_THREAD_NUMBER][reg_id[10]] = (BB_ID_DISPATCHER_REORDER + (DISP_REOR_VIQ_DS_TM_FLUSH << 6));
    local_regs[IMAGE_0_DS_TM_FLUSH_THREAD_NUMBER][reg_id[9]] = IMAGE_0_DS_TM_SCHEDULING_AGGREGATION_CONTEXT_VECTOR_ADDRESS;
    local_regs[IMAGE_0_DS_TM_FLUSH_THREAD_NUMBER][reg_id[8]] = xrdp_virt2phys(&RU_BLK(QM), 0) + RU_REG_OFFSET(QM, GLOBAL_CFG_AGGREGATION_CONTEXT_VALID) +
        (QM_QUEUE_DS_START / 8);

    /* SCHEDULING TASK: thread 4 */
    local_regs[IMAGE_0_DS_TM_SCHEDULING_TASK_THREAD_NUMBER][reg_id[16]] = ADDRESS_OF(image_0, tx_task_1st_wakeup_request) << 16;
    local_regs[IMAGE_0_DS_TM_SCHEDULING_TASK_THREAD_NUMBER][reg_id[12]] = IMAGE_0_G9991_PD_FIFO_TABLE_ADDRESS | (IMAGE_0_DS_TM_BBH_TX_EGRESS_COUNTER_TABLE_ADDRESS << 16);
    local_regs[IMAGE_0_DS_TM_SCHEDULING_TASK_THREAD_NUMBER][reg_id[11]] = BB_ID_TX_XLMAC0_0_10G | (IMAGE_0_COMPLEX_SCHEDULER_TABLE_ADDRESS << 16);
    local_regs[IMAGE_0_DS_TM_SCHEDULING_TASK_THREAD_NUMBER][reg_id[10]] = IMAGE_0_DS_TM_PD_FIFO_TABLE_ADDRESS | (IMAGE_0_BASIC_RATE_LIMITER_TABLE_DS_ADDRESS << 16);
    local_regs[IMAGE_0_DS_TM_SCHEDULING_TASK_THREAD_NUMBER][reg_id[9]]  = (IMAGE_0_DS_TM_SCHEDULING_QUEUE_TABLE_ADDRESS << 16);
    local_regs[IMAGE_0_DS_TM_SCHEDULING_TASK_THREAD_NUMBER][reg_id[8]]  = IMAGE_0_DS_TM_BBH_QUEUE_TABLE_ADDRESS | (IMAGE_0_BASIC_SCHEDULER_TABLE_DS_ADDRESS << 16);

    /* Fragmentation 0: thread 5 */
    local_regs[IMAGE_0_DS_TM_FRAG0_THREAD_NUMBER][reg_id[16]] = ADDRESS_OF(image_0, g9991_tx_task_1st_wakeup_request) << 16;
    local_regs[IMAGE_0_DS_TM_FRAG0_THREAD_NUMBER][reg_id[14]] = IMAGE_0_G9991_CONTROL_SID_TABLE_ADDRESS;
    local_regs[IMAGE_0_DS_TM_FRAG0_THREAD_NUMBER][reg_id[13]] = MAX_FRAG_SIZE;
    local_regs[IMAGE_0_DS_TM_FRAG0_THREAD_NUMBER][reg_id[12]] = IMAGE_0_G9991_FIFO_DESCRIPTOR_TABLE_ADDRESS |
      (IMAGE_0_G9991_SID_TO_PHYSICAL_PORT_MASK_ADDRESS << 16);
    local_regs[IMAGE_0_DS_TM_FRAG0_THREAD_NUMBER][reg_id[11]] = IMAGE_0_G9991_PHYS_PORT_BB_ID_TABLE_ADDRESS |
        ((IMAGE_0_DS_TM_BBH_TX_EGRESS_COUNTER_TABLE_ADDRESS + (init_params->g9991_port[0]) * sizeof(RDD_BBH_TX_EGRESS_COUNTER_ENTRY_DTS)) << 16);
    local_regs[IMAGE_0_DS_TM_FRAG0_THREAD_NUMBER][reg_id[10]] = IMAGE_0_G9991_PD_FIFO_TABLE_ADDRESS |
        ((IMAGE_0_DS_TM_BBH_QUEUE_TABLE_ADDRESS + (init_params->g9991_port[0]) * sizeof(RDD_BBH_QUEUE_DESCRIPTOR_DTS)) << 16);

    /* Fragmentation 1: thread 6 */
    local_regs[IMAGE_0_DS_TM_FRAG1_THREAD_NUMBER][reg_id[16]] = ADDRESS_OF(image_0, g9991_tx_task_1st_wakeup_request) << 16;
    local_regs[IMAGE_0_DS_TM_FRAG1_THREAD_NUMBER][reg_id[14]] = IMAGE_0_G9991_CONTROL_SID_TABLE_ADDRESS + 4;
    local_regs[IMAGE_0_DS_TM_FRAG1_THREAD_NUMBER][reg_id[13]] = MAX_FRAG_SIZE;
    local_regs[IMAGE_0_DS_TM_FRAG1_THREAD_NUMBER][reg_id[12]] = IMAGE_0_G9991_FIFO_DESCRIPTOR_TABLE_ADDRESS |
      ((IMAGE_0_G9991_SID_TO_PHYSICAL_PORT_MASK_ADDRESS + 4) << 16);
    local_regs[IMAGE_0_DS_TM_FRAG1_THREAD_NUMBER][reg_id[11]] = (IMAGE_0_G9991_PHYS_PORT_BB_ID_TABLE_ADDRESS + 1) |
      ((IMAGE_0_DS_TM_BBH_TX_EGRESS_COUNTER_TABLE_ADDRESS + (init_params->g9991_port[1]) * sizeof(RDD_BBH_TX_EGRESS_COUNTER_ENTRY_DTS)) << 16);
    local_regs[IMAGE_0_DS_TM_FRAG1_THREAD_NUMBER][reg_id[10]] = IMAGE_0_G9991_PD_FIFO_TABLE_ADDRESS |
        ((IMAGE_0_DS_TM_BBH_QUEUE_TABLE_ADDRESS + (init_params->g9991_port[1]) * sizeof(RDD_BBH_QUEUE_DESCRIPTOR_DTS)) << 16);

    /* Fragmentation 2: thread 7 */
    local_regs[IMAGE_0_DS_TM_FRAG2_THREAD_NUMBER][reg_id[16]] = ADDRESS_OF(image_0, g9991_tx_task_1st_wakeup_request) << 16;
    local_regs[IMAGE_0_DS_TM_FRAG2_THREAD_NUMBER][reg_id[13]] = MAX_FRAG_SIZE;
    local_regs[IMAGE_0_DS_TM_FRAG2_THREAD_NUMBER][reg_id[12]] = IMAGE_0_G9991_FIFO_DESCRIPTOR_TABLE_ADDRESS |
      ((IMAGE_0_G9991_SID_TO_PHYSICAL_PORT_MASK_ADDRESS + 8) << 16);
    local_regs[IMAGE_0_DS_TM_FRAG2_THREAD_NUMBER][reg_id[11]] = (IMAGE_0_G9991_PHYS_PORT_BB_ID_TABLE_ADDRESS + 2) |
      ((IMAGE_0_DS_TM_BBH_TX_EGRESS_COUNTER_TABLE_ADDRESS + (init_params->g9991_port[2]) * sizeof(RDD_BBH_TX_EGRESS_COUNTER_ENTRY_DTS)) << 16);
    local_regs[IMAGE_0_DS_TM_FRAG2_THREAD_NUMBER][reg_id[10]] = IMAGE_0_G9991_PD_FIFO_TABLE_ADDRESS |
        ((IMAGE_0_DS_TM_BBH_QUEUE_TABLE_ADDRESS + (init_params->g9991_port[2]) * sizeof(RDD_BBH_QUEUE_DESCRIPTOR_DTS)) << 16);

    /* Fragmentation 3: thread 8 */
    local_regs[IMAGE_0_DS_TM_FRAG3_THREAD_NUMBER][reg_id[16]] = ADDRESS_OF(image_0, g9991_tx_task_1st_wakeup_request) << 16;
    local_regs[IMAGE_0_DS_TM_FRAG3_THREAD_NUMBER][reg_id[13]] = MAX_FRAG_SIZE;
    local_regs[IMAGE_0_DS_TM_FRAG3_THREAD_NUMBER][reg_id[12]] = IMAGE_0_G9991_FIFO_DESCRIPTOR_TABLE_ADDRESS |
      ((IMAGE_0_G9991_SID_TO_PHYSICAL_PORT_MASK_ADDRESS + 12) << 16);
    local_regs[IMAGE_0_DS_TM_FRAG3_THREAD_NUMBER][reg_id[11]] = (IMAGE_0_G9991_PHYS_PORT_BB_ID_TABLE_ADDRESS + 3) |
      ((IMAGE_0_DS_TM_BBH_TX_EGRESS_COUNTER_TABLE_ADDRESS + (init_params->g9991_port[3]) * sizeof(RDD_BBH_TX_EGRESS_COUNTER_ENTRY_DTS)) << 16);
    local_regs[IMAGE_0_DS_TM_FRAG3_THREAD_NUMBER][reg_id[10]] = IMAGE_0_G9991_PD_FIFO_TABLE_ADDRESS |
        ((IMAGE_0_DS_TM_BBH_QUEUE_TABLE_ADDRESS + (init_params->g9991_port[3]) * sizeof(RDD_BBH_QUEUE_DESCRIPTOR_DTS)) << 16);
#endif
#ifdef RDP_SIM
    /* copy the local registers initial values to the Context memory */
    MWRITE_BLK_32(sram_context, local_regs, mem_cntxt_byte_num);
#else
    rdp_rnr_write_context(sram_context, local_regs, mem_cntxt_byte_num);
#endif
}

static void image_1_context_set(uint32_t core_index)
{
    static uint32_t local_regs[NUM_OF_MAIN_RUNNER_THREADS][32];
    uint32_t mem_cntxt_byte_num;
    uint32_t *sram_context;

    rdd_global_registers_init(core_index);
    sram_context = (uint32_t *)RUNNER_CORE_CONTEXT_ADDRESS(core_index);
    mem_cntxt_byte_num = (RU_REG_RAM_CNT(RNR_CNTXT, MEM_ENTRY) + 1) * sizeof(uint32_t);
#ifndef XRDP_EMULATION
    /* read the local registers from the Context memory - maybe it was initialized by the ACE compiler */
    MREAD_BLK_32(local_regs, sram_context, mem_cntxt_byte_num);
#endif

#ifdef G9991
    /* G9991 us processing : thread 0 */
    local_regs[IMAGE_1_REASSEMBLY_G9991_0_THREAD_NUMBER][reg_id[16]]  = IMAGE_1_G9991_PD_TABLE_ADDRESS | (ADDRESS_OF(image_1, g9991_reassembly_wakeup_request) << 16);
    local_regs[IMAGE_1_REASSEMBLY_G9991_0_THREAD_NUMBER][reg_id[14]]  = (DISP_REOR_VIQ_G9991_REASS0 << 6) |  BB_ID_DISPATCHER_REORDER;
    local_regs[IMAGE_1_REASSEMBLY_G9991_0_THREAD_NUMBER][reg_id[13]]  = BB_ID_SBPM;
    local_regs[IMAGE_1_REASSEMBLY_G9991_0_THREAD_NUMBER][reg_id[12]]  = IMAGE_1_G9991_US_SID_CONTEXT_TABLE_ADDRESS | (IMAGE_1_REASSEMBLY_DISPATCHER_CREDIT_TABLE_ADDRESS << 16);
    local_regs[IMAGE_1_REASSEMBLY_G9991_0_THREAD_NUMBER][reg_id[11]]  = BB_ID_DISPATCHER_REORDER;
    local_regs[IMAGE_1_REASSEMBLY_G9991_0_THREAD_NUMBER][reg_id[10]]  = (IMAGE_1_REASSEMBLY_G9991_0_THREAD_NUMBER << 6) |  BB_ID_DISPATCHER_REORDER;

    /* G9991 us processing : thread 1 */
    local_regs[IMAGE_1_REASSEMBLY_G9991_1_THREAD_NUMBER][reg_id[16]]  = (IMAGE_1_G9991_PD_TABLE_ADDRESS + 32) | (ADDRESS_OF(image_1, g9991_reassembly_wakeup_request) << 16);
    local_regs[IMAGE_1_REASSEMBLY_G9991_1_THREAD_NUMBER][reg_id[14]]  = (DISP_REOR_VIQ_G9991_REASS1 << 6) |  BB_ID_DISPATCHER_REORDER;
    local_regs[IMAGE_1_REASSEMBLY_G9991_1_THREAD_NUMBER][reg_id[13]]  = BB_ID_SBPM;
    local_regs[IMAGE_1_REASSEMBLY_G9991_1_THREAD_NUMBER][reg_id[12]]  = IMAGE_1_G9991_US_SID_CONTEXT_TABLE_ADDRESS | ((IMAGE_1_REASSEMBLY_DISPATCHER_CREDIT_TABLE_ADDRESS + 16) << 16);
    local_regs[IMAGE_1_REASSEMBLY_G9991_1_THREAD_NUMBER][reg_id[11]]  = BB_ID_DISPATCHER_REORDER;
    local_regs[IMAGE_1_REASSEMBLY_G9991_1_THREAD_NUMBER][reg_id[10]]  = (IMAGE_1_REASSEMBLY_G9991_1_THREAD_NUMBER << 6) |  BB_ID_DISPATCHER_REORDER;

    /* G9991 us processing : thread 2 */
    local_regs[IMAGE_1_REASSEMBLY_G9991_2_THREAD_NUMBER][reg_id[16]]  = (IMAGE_1_G9991_PD_TABLE_ADDRESS + 64) | (ADDRESS_OF(image_1, g9991_reassembly_wakeup_request) << 16);
    local_regs[IMAGE_1_REASSEMBLY_G9991_2_THREAD_NUMBER][reg_id[14]]  = (DISP_REOR_VIQ_G9991_REASS2 << 6) |  BB_ID_DISPATCHER_REORDER;
    local_regs[IMAGE_1_REASSEMBLY_G9991_2_THREAD_NUMBER][reg_id[13]]  = BB_ID_SBPM;
    local_regs[IMAGE_1_REASSEMBLY_G9991_2_THREAD_NUMBER][reg_id[12]]  = IMAGE_1_G9991_US_SID_CONTEXT_TABLE_ADDRESS | ((IMAGE_1_REASSEMBLY_DISPATCHER_CREDIT_TABLE_ADDRESS + 32) << 16);
    local_regs[IMAGE_1_REASSEMBLY_G9991_2_THREAD_NUMBER][reg_id[11]]  = BB_ID_DISPATCHER_REORDER;
    local_regs[IMAGE_1_REASSEMBLY_G9991_2_THREAD_NUMBER][reg_id[10]]  = (IMAGE_1_REASSEMBLY_G9991_2_THREAD_NUMBER << 6) |  BB_ID_DISPATCHER_REORDER;

    /* G9991 us processing : thread 3 */
    local_regs[IMAGE_1_REASSEMBLY_G9991_3_THREAD_NUMBER][reg_id[16]]  = (IMAGE_1_G9991_PD_TABLE_ADDRESS + 96) | (ADDRESS_OF(image_1, g9991_reassembly_wakeup_request) << 16);
    local_regs[IMAGE_1_REASSEMBLY_G9991_3_THREAD_NUMBER][reg_id[14]]  = (DISP_REOR_VIQ_G9991_REASS3 << 6) |  BB_ID_DISPATCHER_REORDER;
    local_regs[IMAGE_1_REASSEMBLY_G9991_3_THREAD_NUMBER][reg_id[13]]  = BB_ID_SBPM;
    local_regs[IMAGE_1_REASSEMBLY_G9991_3_THREAD_NUMBER][reg_id[12]]  = IMAGE_1_G9991_US_SID_CONTEXT_TABLE_ADDRESS | ((IMAGE_1_REASSEMBLY_DISPATCHER_CREDIT_TABLE_ADDRESS + 48) << 16);
    local_regs[IMAGE_1_REASSEMBLY_G9991_3_THREAD_NUMBER][reg_id[11]]  = BB_ID_DISPATCHER_REORDER;
    local_regs[IMAGE_1_REASSEMBLY_G9991_3_THREAD_NUMBER][reg_id[10]]  = (IMAGE_1_REASSEMBLY_G9991_3_THREAD_NUMBER << 6) |  BB_ID_DISPATCHER_REORDER;
#else
    /* DIRECT PROCESSING : thread 0 */
    local_regs[IMAGE_1_CPU_IF_1_WAN_DIRECT_THREAD_NUMBER][reg_id[9]] = IMAGE_1_RX_FLOW_TABLE_ADDRESS << 16 | IMAGE_1_CPU_IF_1_WAN_DIRECT_THREAD_NUMBER;
    local_regs[IMAGE_1_CPU_IF_1_WAN_DIRECT_THREAD_NUMBER][reg_id[10]] = BB_ID_DISPATCHER_REORDER;
    local_regs[IMAGE_1_CPU_IF_1_WAN_DIRECT_THREAD_NUMBER][reg_id[11]] = QM_QUEUE_CPU_RX_COPY_EXCLUSIVE | ( IMAGE_1_DIRECT_PROCESSING_EPON_CONTROL_SCRATCH_ADDRESS << 16 );
    local_regs[IMAGE_1_CPU_IF_1_WAN_DIRECT_THREAD_NUMBER][reg_id[16]] =
        ADDRESS_OF(image_1, direct_processing_wakeup_request) << 16 | IMAGE_1_DIRECT_PROCESSING_PD_TABLE_ADDRESS;

    /* CPU_RX_FIFO_READ: thread 1 */
    local_regs[IMAGE_1_CPU_IF_1_CPU_RX_THREAD_NUMBER][reg_id[16]] = ADDRESS_OF(image_1, cpu_rx_wakeup_request) << 16;
    local_regs[IMAGE_1_CPU_IF_1_CPU_RX_THREAD_NUMBER][reg_id[14]] = IMAGE_1_CPU_RX_SCRATCHPAD_ADDRESS |
        (IMAGE_1_CPU_RX_PSRAM_GET_NEXT_SCRATCHPAD_ADDRESS << 16);
    local_regs[IMAGE_1_CPU_IF_1_CPU_RX_THREAD_NUMBER][reg_id[12]] = IMAGE_1_CPU_RING_DESCRIPTORS_TABLE_ADDRESS;
    local_regs[IMAGE_1_CPU_IF_1_CPU_RX_THREAD_NUMBER][reg_id[11]] = IMAGE_1_PD_FIFO_TABLE_ADDRESS | (IMAGE_1_CPU_RECYCLE_SRAM_PD_FIFO_ADDRESS << 16);

    /* CPU_RX_UPDATE_FIFO_READ: thread 2 */
    local_regs[IMAGE_1_CPU_IF_1_CPU_RX_UPDATE_FIFO_THREAD_NUMBER][reg_id[16]] = ADDRESS_OF(image_1, cpu_rx_update_fifo_wakeup_request) << 16;
    local_regs[IMAGE_1_CPU_IF_1_CPU_RX_UPDATE_FIFO_THREAD_NUMBER][reg_id[12]] = (((IMAGE_1_CPU_IF_1_CPU_RX_THREAD_NUMBER) << 4) + 9);
    local_regs[IMAGE_1_CPU_IF_1_CPU_RX_UPDATE_FIFO_THREAD_NUMBER][reg_id[11]] = IMAGE_1_UPDATE_FIFO_TABLE_ADDRESS;

#if 0  /* !!!!!!!!!!!!!!! */
    /* CPU_RX_METER_BUDGET_ALLOCATOR: thread 4 */
    local_regs[IMAGE_1_CPU_IF_1_CPU_RX_METER_BUDGET_ALLOCATOR_THREAD_NUMBER][reg_id[16]] =
        ADDRESS_OF(image_1, cpu_rx_meter_budget_allocator_1st_wakeup_request) << 16;
    local_regs[IMAGE_1_CPU_IF_1_CPU_RX_METER_BUDGET_ALLOCATOR_THREAD_NUMBER][reg_id[11]] = CPU_RX_METER_TIMER_PERIOD;
#endif

    /* CPU_RX_COPY_FIFO_READ: thread 13 */
    local_regs[IMAGE_1_CPU_IF_1_CPU_RX_COPY_THREAD_NUMBER][reg_id[16]] = ADDRESS_OF(image_1, cpu_rx_copy_wakeup_request) << 16;
    local_regs[IMAGE_1_CPU_IF_1_CPU_RX_COPY_THREAD_NUMBER][reg_id[14]] = IMAGE_1_CPU_RX_SCRATCHPAD_ADDRESS |
        (IMAGE_1_CPU_RX_PSRAM_GET_NEXT_SCRATCHPAD_ADDRESS << 16);
    local_regs[IMAGE_1_CPU_IF_1_CPU_RX_COPY_THREAD_NUMBER][reg_id[12]] = 0;
    local_regs[IMAGE_1_CPU_IF_1_CPU_RX_COPY_THREAD_NUMBER][reg_id[11]] = (IMAGE_1_CPU_RX_COPY_PD_FIFO_TABLE_ADDRESS) | (IMAGE_1_CPU_RX_COPY_DISPATCHER_CREDIT_TABLE_ADDRESS << 16);

    /* CPU_RX_COPY_UPDATE_FIFO_READ: thread 14 */
    local_regs[IMAGE_1_CPU_IF_1_CPU_RX_COPY_UPDATE_FIFO_THREAD_NUMBER][reg_id[16]] = ADDRESS_OF(image_1, cpu_rx_copy_update_fifo_wakeup_request) << 16;
    local_regs[IMAGE_1_CPU_IF_1_CPU_RX_COPY_UPDATE_FIFO_THREAD_NUMBER][reg_id[12]] = (((IMAGE_1_CPU_IF_1_CPU_RX_COPY_THREAD_NUMBER) << 4) + 9);
    local_regs[IMAGE_1_CPU_IF_1_CPU_RX_COPY_UPDATE_FIFO_THREAD_NUMBER][reg_id[11]] = IMAGE_1_CPU_RX_COPY_UPDATE_FIFO_TABLE_ADDRESS;
#endif /* G9991 */

#ifdef RDP_SIM
    /* copy the local registers initial values to the Context memory */
    MWRITE_BLK_32(sram_context, local_regs, mem_cntxt_byte_num);
#else
    rdp_rnr_write_context(sram_context, local_regs, mem_cntxt_byte_num);
#endif
}

static void image_2_context_set(uint32_t core_index)
{
    static uint32_t local_regs[NUM_OF_MAIN_RUNNER_THREADS][32];
    uint32_t mem_cntxt_byte_num;
    uint32_t *sram_context;

    rdd_global_registers_init(core_index);
    sram_context = (uint32_t *)RUNNER_CORE_CONTEXT_ADDRESS(core_index);
    mem_cntxt_byte_num = (RU_REG_RAM_CNT(RNR_CNTXT, MEM_ENTRY) + 1) * sizeof(uint32_t);
#ifndef XRDP_EMULATION
    /* read the local registers from the Context memory - maybe it was initialized by the ACE compiler */
    MREAD_BLK_32(local_regs, sram_context, mem_cntxt_byte_num);
#endif

    /* CPU_TX_EGRESS: thread 3 */
    local_regs[IMAGE_2_CPU_IF_2_CPU_TX_EGRESS_THREAD_NUMBER][reg_id[16]] = ADDRESS_OF(image_2, cpu_tx_1st_wakeup_request) << 16;
    local_regs[IMAGE_2_CPU_IF_2_CPU_TX_EGRESS_THREAD_NUMBER][reg_id[8]] = IMAGE_2_CPU_TX_RING_TABLE_ADDRESS;
    local_regs[IMAGE_2_CPU_IF_2_CPU_TX_EGRESS_THREAD_NUMBER][reg_id[9]] = ((BB_ID_DISPATCHER_REORDER + (DISP_REOR_VIQ_CPU_TX_EGRESS << 6)) << 16);
    local_regs[IMAGE_2_CPU_IF_2_CPU_TX_EGRESS_THREAD_NUMBER][reg_id[10]] = IMAGE_2_CPU_IF_2_CPU_TX_EGRESS_THREAD_NUMBER;

#ifdef G9991

    /* DIRECT PROCESSING : thread 5 */
    local_regs[IMAGE_2_CPU_IF_2_WAN_DIRECT_THREAD_NUMBER][reg_id[9]] = IMAGE_2_RX_FLOW_TABLE_ADDRESS << 16 | IMAGE_2_CPU_IF_2_WAN_DIRECT_THREAD_NUMBER;
    local_regs[IMAGE_2_CPU_IF_2_WAN_DIRECT_THREAD_NUMBER][reg_id[10]] = BB_ID_DISPATCHER_REORDER;
    local_regs[IMAGE_2_CPU_IF_2_WAN_DIRECT_THREAD_NUMBER][reg_id[11]] = QM_QUEUE_CPU_RX_COPY_EXCLUSIVE | ( IMAGE_2_DIRECT_PROCESSING_EPON_CONTROL_SCRATCH_ADDRESS << 16 );
    local_regs[IMAGE_2_CPU_IF_2_WAN_DIRECT_THREAD_NUMBER][reg_id[16]] =
        ADDRESS_OF(image_2, gpon_control_wakeup_request) << 16 | IMAGE_2_DIRECT_PROCESSING_PD_TABLE_ADDRESS;

    /* DIRECT PROCESSING : thread 6 */
    local_regs[IMAGE_2_CPU_IF_2_DIRECT_SYSTEM_PORT_THREAD_NUMBER][reg_id[9]] = IMAGE_2_RX_FLOW_TABLE_ADDRESS << 16 | IMAGE_2_CPU_IF_2_DIRECT_SYSTEM_PORT_THREAD_NUMBER;
    local_regs[IMAGE_2_CPU_IF_2_DIRECT_SYSTEM_PORT_THREAD_NUMBER][reg_id[10]] = BB_ID_DISPATCHER_REORDER;
    local_regs[IMAGE_2_CPU_IF_2_DIRECT_SYSTEM_PORT_THREAD_NUMBER][reg_id[11]] = QM_QUEUE_CPU_RX_COPY_EXCLUSIVE | ( IMAGE_2_DIRECT_PROCESSING_EPON_CONTROL_SCRATCH_ADDRESS << 16 );
    local_regs[IMAGE_2_CPU_IF_2_DIRECT_SYSTEM_PORT_THREAD_NUMBER][reg_id[16]] =
        ADDRESS_OF(image_2, direct_processing_wakeup_request) << 16 | (IMAGE_2_DIRECT_PROCESSING_PD_TABLE_ADDRESS + sizeof(RDD_PROCESSING_RX_DESCRIPTOR_DTS));

    /* CPU_RX_FIFO_READ: thread 7 */
    local_regs[IMAGE_2_CPU_IF_2_CPU_RX_THREAD_NUMBER][reg_id[16]] = ADDRESS_OF(image_2, cpu_rx_wakeup_request) << 16;
    local_regs[IMAGE_2_CPU_IF_2_CPU_RX_THREAD_NUMBER][reg_id[14]] = IMAGE_2_CPU_RX_SCRATCHPAD_ADDRESS |
        (IMAGE_2_CPU_RX_PSRAM_GET_NEXT_SCRATCHPAD_ADDRESS << 16);
    local_regs[IMAGE_2_CPU_IF_2_CPU_RX_THREAD_NUMBER][reg_id[12]] = IMAGE_2_CPU_RING_DESCRIPTORS_TABLE_ADDRESS;
    local_regs[IMAGE_2_CPU_IF_2_CPU_RX_THREAD_NUMBER][reg_id[11]] = IMAGE_2_PD_FIFO_TABLE_ADDRESS | (IMAGE_2_CPU_RECYCLE_SRAM_PD_FIFO_ADDRESS << 16);

    /* CPU_RX_UPDATE_FIFO_READ: thread 2 */
    local_regs[IMAGE_2_CPU_IF_2_CPU_RX_UPDATE_FIFO_THREAD_NUMBER][reg_id[16]] = ADDRESS_OF(image_2, cpu_rx_update_fifo_wakeup_request) << 16;
    local_regs[IMAGE_2_CPU_IF_2_CPU_RX_UPDATE_FIFO_THREAD_NUMBER][reg_id[12]] = (((IMAGE_2_CPU_IF_2_CPU_RX_THREAD_NUMBER) << 4) + 9);
    local_regs[IMAGE_2_CPU_IF_2_CPU_RX_UPDATE_FIFO_THREAD_NUMBER][reg_id[11]] = IMAGE_2_UPDATE_FIFO_TABLE_ADDRESS;

    /* CPU_RX_METER_BUDGET_ALLOCATOR: thread 11 */
    local_regs[IMAGE_2_CPU_IF_2_CPU_RX_METER_BUDGET_ALLOCATOR_THREAD_NUMBER][reg_id[16]] =
        ADDRESS_OF(image_2, cpu_rx_meter_budget_allocator_1st_wakeup_request) << 16;
    local_regs[IMAGE_2_CPU_IF_2_CPU_RX_METER_BUDGET_ALLOCATOR_THREAD_NUMBER][reg_id[11]] = CPU_RX_METER_TIMER_PERIOD;

    /* CPU_RX_COPY_FIFO_READ: thread 13 */
    local_regs[IMAGE_2_CPU_IF_2_CPU_RX_COPY_THREAD_NUMBER][reg_id[16]] = ADDRESS_OF(image_2, cpu_rx_copy_wakeup_request) << 16;
    local_regs[IMAGE_2_CPU_IF_2_CPU_RX_COPY_THREAD_NUMBER][reg_id[14]] = IMAGE_2_CPU_RX_SCRATCHPAD_ADDRESS |
        (IMAGE_2_CPU_RX_PSRAM_GET_NEXT_SCRATCHPAD_ADDRESS << 16);
    local_regs[IMAGE_2_CPU_IF_2_CPU_RX_COPY_THREAD_NUMBER][reg_id[12]] = 0;
    local_regs[IMAGE_2_CPU_IF_2_CPU_RX_COPY_THREAD_NUMBER][reg_id[11]] = (IMAGE_2_CPU_RX_COPY_PD_FIFO_TABLE_ADDRESS) | (IMAGE_2_CPU_RX_COPY_DISPATCHER_CREDIT_TABLE_ADDRESS << 16);

    /* CPU_RX_COPY_UPDATE_FIFO_READ: thread 14 */
    local_regs[IMAGE_2_CPU_IF_2_CPU_RX_COPY_UPDATE_FIFO_THREAD_NUMBER][reg_id[16]] = ADDRESS_OF(image_2, cpu_rx_copy_update_fifo_wakeup_request) << 16;
    local_regs[IMAGE_2_CPU_IF_2_CPU_RX_COPY_UPDATE_FIFO_THREAD_NUMBER][reg_id[12]] = (((IMAGE_2_CPU_IF_2_CPU_RX_COPY_THREAD_NUMBER) << 4) + 9);
    local_regs[IMAGE_2_CPU_IF_2_CPU_RX_COPY_UPDATE_FIFO_THREAD_NUMBER][reg_id[11]] = IMAGE_2_CPU_RX_COPY_UPDATE_FIFO_TABLE_ADDRESS;

#endif /* #ifdef G9991 */

#ifdef RDP_SIM
    /* copy the local registers initial values to the Context memory */
    MWRITE_BLK_32(sram_context, local_regs, mem_cntxt_byte_num);
#else
    rdp_rnr_write_context(sram_context, local_regs, mem_cntxt_byte_num);
#endif
}

static void rdd_local_registers_init(rdd_init_params_t *init_params)
{
    uint32_t core_index;

    for (core_index = 0; core_index < GROUPED_EN_SEGMENTS_NUM; core_index++)
    {
        switch (rdp_core_to_image_map[core_index])
        {
        case image_0_runner_image:
            image_0_context_set(core_index, init_params);
            break;

        case image_1_runner_image:
            image_1_context_set(core_index);
            break;

        case image_2_runner_image:
            image_2_context_set(core_index);
            break;

        default:
            bdmf_trace("ERROR driver %s:%u| unsupported Runner image = %d\n", __FILE__, __LINE__, rdp_core_to_image_map[core_index]);
            break;
        }
    }
}

static int rdd_cpu_proj_init(void)
{
    uint8_t def_idx = 0;
    int rc = 0;

    rdd_cpu_tc_to_rqx_init(def_idx);
    rdd_cpu_vport_cpu_obj_init(def_idx);
    rdd_cpu_rx_meters_init();
    return rc;
}

#ifdef G9991
static void rdd_g9991_ds_tx_init(rdd_init_params_t *init_params)
{
    int i;

    RDD_BYTES_4_BITS_WRITE_G(DS_BBH_TX_BYTES_FIFO_VALUE, RDD_G9991_BBH_TX_BYTES_FIFO_SIZE_ADDRESS_ARR, 0);
    RDD_BYTES_4_BITS_WRITE_G(PD_FIFO_CREDIT_INIT_VALUE, RDD_G9991_NOT_FULL_VECTOR_ADDRESS_ARR, 0);
    RDD_BYTES_4_BITS_WRITE_G(FLOW_CONTROL_INIT_VALUE, RDD_G9991_FLOW_CONTROL_VECTOR_ADDRESS_ARR, 0);
    for (i = 0; i < RDD_G9991_SID_TO_PHYSICAL_PORT_MASK_SIZE; i++)
    {
        if (init_params->g9991_port[i] >= 0)
            RDD_BYTE_1_BITS_WRITE_G(rdpa_emac_to_bb_id_tx[init_params->g9991_port[i]], RDD_G9991_PHYS_PORT_BB_ID_TABLE_ADDRESS_ARR, i);
    }
}

static void rdd_g9991_us_tx_init(void)
{
    int i;

    for (i = 0; i < RDD_G9991_US_SID_CONTEXT_TABLE_SIZE; ++i)
        RDD_G9991_US_SID_CONTEXT_ENTRY_FRAGMENT_COUNT_WRITE_G(1, RDD_G9991_US_SID_CONTEXT_TABLE_ADDRESS_ARR, i);
}
#endif

int rdd_data_structures_init(rdd_init_params_t *init_params)
{
    bdmf_error_t rc = BDMF_ERR_OK;
    int i;

    rdd_local_registers_init(init_params);

    rdd_cpu_if_init();

#ifdef G9991
    /* DS: init G9991 fragmentation */
    rdd_g9991_ds_tx_init(init_params);

    /* US: init G9991 reassembly */
    rdd_g9991_us_tx_init();
#endif
    /* init first queue mapping */
    rdd_ag_ds_tm_first_queue_mapping_set(QM_QUEUE_DS_START);

    /* init bbh-queue */
    rdd_bbh_queue_init();

    /* WA for A0 - enable queue in fw */
    rdd_set_queue_enable(QM_QUEUE_CPU_RX, 1);
    rdd_set_queue_enable(QM_QUEUE_CPU_RX_COPY_NORMAL, 1);
    rdd_set_queue_enable(QM_QUEUE_CPU_RX_COPY_EXCLUSIVE, 1);

    /* enable all tx_flow (ports) */
    for (i=0; i <= 8; i++)
    {
        rdd_tx_flow_enable(i, rdpa_dir_ds, 1);
        rdd_tx_flow_enable(i, rdpa_dir_us, 1);
    }

    rdd_proj_init(init_params);
    rdd_tm_actions_proj_init();

    rc = rc ? rc : rdd_cpu_proj_init();

#ifdef CPU_TX_RING
    ag_drv_rnr_regs_cfg_cpu_wakeup_set(get_runner_idx(cpu_tx_runner_image), IMAGE_2_CPU_IF_2_CPU_TX_EGRESS_THREAD_NUMBER);
#endif

#ifdef USE_BDMF_SHELL
    /* register shell commands */
    rc = rc ? : rdd_make_shell_commands();
#endif
    return rc;
}


static void rdd_proj_init(rdd_init_params_t *init_params)
{
    /* Classification modules initialization */
    if (init_params->is_basic)
        return;
}

static void rdd_tm_actions_proj_init(void)
{
    uint32_t action_index;
    RDD_BYTES_2_DTS *tm_action_ptr;
    uint16_t ds_actions_arr[] = {
        [0]  = ADDRESS_OF(image_0, basic_scheduler_update_dwrr),
        [1 ... 15] = ADDRESS_OF(image_0, scheduling_action_not_valid),
        [16] = ADDRESS_OF(image_0, scheduling_update_status)
    };

    for (action_index = 0; action_index < RDD_TM_ACTION_PTR_TABLE_SIZE; action_index++)
    {
        tm_action_ptr = ((RDD_BYTES_2_DTS *)RDD_DS_TM_TM_ACTION_PTR_TABLE_PTR(get_runner_idx(ds_tm_runner_image))) + action_index;
        RDD_BYTES_2_BITS_WRITE(ds_actions_arr[action_index], tm_action_ptr);
    }
}

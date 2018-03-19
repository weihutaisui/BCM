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

#include "rdd_ingress_filter.h"
#include "rdd_ag_processing.h"
#include "rdp_drv_proj_cntr.h"

int rdd_ingress_filter_module_init(const rdd_module_t *module)
{
    uint8_t ix, l2_filter_reasons[INGRESS_FILTER_L2_REASON_TABLE_SIZE] = { };

    /* init L2 filter reasons */
    l2_filter_reasons[INGRESS_FILTER_ETYPE_PPPOE_D] = CPU_RX_REASON_ETYPE_PPPOE_D;
    l2_filter_reasons[INGRESS_FILTER_ETYPE_PPPOE_S] = CPU_RX_REASON_ETYPE_PPPOE_S;
    l2_filter_reasons[INGRESS_FILTER_ETYPE_UDEF_0] = CPU_RX_REASON_ETYPE_UDEF_0;
    l2_filter_reasons[INGRESS_FILTER_ETYPE_UDEF_1] = CPU_RX_REASON_ETYPE_UDEF_1;
    l2_filter_reasons[INGRESS_FILTER_ETYPE_UDEF_2] = CPU_RX_REASON_ETYPE_UDEF_2;
    l2_filter_reasons[INGRESS_FILTER_ETYPE_UDEF_3] = CPU_RX_REASON_ETYPE_UDEF_3;
    l2_filter_reasons[INGRESS_FILTER_ETYPE_ARP] = CPU_RX_REASON_ETYPE_ARP;
    l2_filter_reasons[INGRESS_FILTER_ETYPE_802_1X] = CPU_RX_REASON_ETYPE_802_1X;
    l2_filter_reasons[INGRESS_FILTER_ETYPE_802_1AG_CFM] = CPU_RX_REASON_ETYPE_801_1AG_CFM;
    l2_filter_reasons[INGRESS_FILTER_ETYPE_PTP_1588] = CPU_RX_REASON__1588;

    /* init res_offset in cfg... */
    RDD_INGRESS_FILTER_CFG_RES_OFFSET_WRITE_G((uint16_t)module->res_offset, RDD_INGRESS_FILTER_CFG_ADDRESS_ARR, 0);

    /* init profiles table */
    for (ix = 0; ix < RDD_INGRESS_FILTER_PROFILE_TABLE_SIZE; ix++)
    {
        RDD_INGRESS_FILTER_CTRL_ENABLE_VECTOR_WRITE_G(0, RDD_INGRESS_FILTER_PROFILE_TABLE_ADDRESS_ARR, ix);
        RDD_INGRESS_FILTER_CTRL_ACTION_VECTOR_WRITE_G(0, RDD_INGRESS_FILTER_PROFILE_TABLE_ADDRESS_ARR, ix);
    }

    for (ix = 0; ix < INGRESS_FILTER_L2_REASON_TABLE_SIZE; ix++)
        RDD_BYTE_1_BITS_WRITE_G(l2_filter_reasons[ix], RDD_INGRESS_FILTER_L2_REASON_TABLE_ADDRESS_ARR, ix);

    return 0;
}

int rdd_ingress_filter_vport_to_profile_set(uint8_t vport, uint8_t profile)
{
#if defined(BCM63158)
    return rdd_ag_processing_vport_cfg_table_ingress_filter_profile_set(vport, profile);
#else
    return rdd_ag_processing_vport_cfg_ex_table_ingress_filter_profile_set(vport, profile);
#endif
}

void rdd_ingress_filter_profile_cfg(uint8_t profile, uint32_t filter_mask, uint32_t action_mask)
{
    RDD_INGRESS_FILTER_CTRL_ENABLE_VECTOR_WRITE_G(filter_mask, RDD_INGRESS_FILTER_PROFILE_TABLE_ADDRESS_ARR, profile);
    RDD_INGRESS_FILTER_CTRL_ACTION_VECTOR_WRITE_G(action_mask, RDD_INGRESS_FILTER_PROFILE_TABLE_ADDRESS_ARR, profile);
}

void rdd_ingress_filter_1588_cfg(int enable)
{
    RDD_BYTE_1_BITS_WRITE_G(enable, RDD_INGRESS_FILTER_1588_CFG_ADDRESS_ARR, 0);
}

int rdd_ingress_filter_drop_counter_get(uint8_t filter, rdpa_traffic_dir dir, uint16_t *drop_counter)
{
    int counter_idx;

    if (dir == rdpa_dir_ds)
        counter_idx = filter + COUNTER_INGRESS_FILTER_DROP_FIRST_DS;
    else
        counter_idx = filter + COUNTER_INGRESS_FILTER_DROP_FIRST_US;

    return drv_cntr_varios_counter_get(counter_idx, drop_counter);
}

void rdd_ingress_filter_cpu_bypass_cfg_set(uint8_t profile, bdmf_boolean cpu_bypass)
{
    RDD_INGRESS_FILTER_CTRL_CPU_BYPASS_WRITE_G((int)cpu_bypass, RDD_INGRESS_FILTER_PROFILE_TABLE_ADDRESS_ARR, profile);
}


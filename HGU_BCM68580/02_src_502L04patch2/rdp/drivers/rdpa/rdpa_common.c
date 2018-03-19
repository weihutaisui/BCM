/*
* <:copyright-BRCM:2013-2015:proprietary:standard
* 
*    Copyright (c) 2013-2015 Broadcom 
*    All Rights Reserved
* 
*  This program is the proprietary software of Broadcom and/or its
*  licensors, and may only be used, duplicated, modified or distributed pursuant
*  to the terms and conditions of a separate, written license agreement executed
*  between you and Broadcom (an "Authorized License").  Except as set forth in
*  an Authorized License, Broadcom grants no license (express or implied), right
*  to use, or waiver of any kind with respect to the Software, and Broadcom
*  expressly reserves all rights in and to the Software and all intellectual
*  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
*  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
*  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
* 
*  Except as expressly set forth in the Authorized License,
* 
*  1. This program, including its structure, sequence and organization,
*     constitutes the valuable trade secrets of Broadcom, and you shall use
*     all reasonable efforts to protect the confidentiality thereof, and to
*     use this information only in connection with your use of Broadcom
*     integrated circuit products.
* 
*  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
*     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
*     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
*     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
*     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
*     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
*     PERFORMANCE OF THE SOFTWARE.
* 
*  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
*     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
*     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
*     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
*     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
*     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
*     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
*     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
*     LIMITED REMEDY.
 :>
*/

#include <bdmf_dev.h>
#include "rdpa_api.h"
#include "rdpa_common.h"
#include "rdpa_int.h"

static int sa_lookup_ref_count;

/* same custom user reasons can't be use by IP_CLASS/INGRESS_CLASS in parallel */
const bdmf_attr_enum_table_t rdpa_ic_trap_reason_enum_table =
{
    .type_name = "ic_trap_reason",
    .values =
    {
        {"no_trap", 0},
        {"ic_trap0", rdpa_cpu_rx_reason_udef_0},
        {"ic_trap1", rdpa_cpu_rx_reason_udef_1},
        {"ic_trap2", rdpa_cpu_rx_reason_udef_2},
        {"ic_trap3", rdpa_cpu_rx_reason_udef_3},
        {"ic_trap4", rdpa_cpu_rx_reason_udef_4},
        {"ic_trap5", rdpa_cpu_rx_reason_udef_5},
        {"ic_trap6", rdpa_cpu_rx_reason_udef_6},
        {"ic_trap7", rdpa_cpu_rx_reason_udef_7},
        {NULL, 0}
    }
};

bdmf_error_t rdpa_obj_get(struct bdmf_object **rdpa_objs, int max_rdpa_objs_num, int index,
    struct bdmf_object **mo)
{
    if ((unsigned)index >= max_rdpa_objs_num)
        return BDMF_ERR_RANGE;

    *mo = rdpa_objs[index];
    if (*mo)
        bdmf_get(*mo);
    return *mo ? BDMF_ERR_OK : BDMF_ERR_NOENT;
}

int rdpa_dir_index_get_next(rdpa_dir_index_t *dir_index, bdmf_index max_index)
{
    if (!dir_index)
        return BDMF_ERR_PARM;
    if (*(bdmf_index *)dir_index == BDMF_INDEX_UNASSIGNED)
    {
        dir_index->dir = rdpa_dir_ds;
        dir_index->index = 0;
    }
    if (++dir_index->index >= max_index)
    {
        if (dir_index->dir == rdpa_dir_us)
            return BDMF_ERR_NO_MORE;
        dir_index->dir = rdpa_dir_us;
        dir_index->index = 0;
    }
    return 0;
}

#if !defined(DSL_63138) && !defined(DSL_63148) && !defined(WL4908) && !defined(BCM63158)
extern rdpa_epon_mode g_epon_mode;
rdpa_epon_mode _rdpa_epon_mode_get(void)
{
    rdpa_epon_mode mode = rdpa_epon_none;

    if (rdpa_is_epon_or_xepon_mode())
    {
#ifdef BDMF_DRIVER_GPL_LAYER
    	epon_get_mode(&mode);
#else
    	mode = g_epon_mode;
#endif
    	return mode;
    }

    return rdpa_epon_none;
}
#endif

bdmf_boolean is_sa_mac_use(void)
{
    return (bdmf_boolean)sa_lookup_ref_count;
}

void sa_mac_use_count_up(void)
{
    sa_lookup_ref_count++;
}

void sa_mac_use_count_down(void)
{
    if (sa_lookup_ref_count)
        sa_lookup_ref_count--;
}

#if !defined(WL4908)
/* classification_result aggregate type */
struct bdmf_aggr_type classification_result_type =
{
    .name = "classification_result", .struct_name = "rdpa_ic_result_t ",
    .help = "Ingress classification result",
    .fields = (struct bdmf_attr[])
    {
        { .name = "qos_method", .help = "QoS classification method", .size = sizeof(rdpa_qos_method),
            .type = bdmf_attr_enum, .ts.enum_table = &rdpa_qos_method_enum_table,
            .offset = offsetof(rdpa_ic_result_t , qos_method)
        },
        { .name = "wan_flow", .help = "WAN Flow instance",
            .type = bdmf_attr_number, /* WAN object can be either GEM or another type, no need for ref_type_name */
            .offset = offsetof(rdpa_ic_result_t , wan_flow), .size = sizeof(uint8_t),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "action", .help = "Forwarding action host/drop or forward (ignored for multicast)",
            .type = bdmf_attr_enum, .ts.enum_table = &rdpa_forward_action_enum_table,
            .size = sizeof(rdpa_forward_action), .offset = offsetof(rdpa_ic_result_t , action)
        },
#if !defined(BCM63158)
        { .name = "policer", .help = "Traffic policer id (ignored for multicast)",
            .type = bdmf_attr_object, .size = sizeof(bdmf_object_handle), .ts.ref_type_name = "policer",
            .offset = offsetof(rdpa_ic_result_t, policer)
        },
#endif
        { .name = "forw_mode", .help = "DS Forwarding mode",
            .type = bdmf_attr_enum, .ts.enum_table = &rdpa_forward_mode_enum_table,
            .offset = offsetof(rdpa_ic_result_t , forw_mode), .size = sizeof(rdpa_forwarding_mode),
        },
        { .name = "egress_port", .help = "Egress port", .type = bdmf_attr_enum,
            .size = sizeof(rdpa_if), .ts.enum_table = &rdpa_if_enum_table,
            .offset = offsetof(rdpa_ic_result_t , egress_port),
        },
        { .name = "queue_id", .help = "Egress queue id", .size = sizeof(uint32_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_ic_result_t , queue_id),
            .flags = BDMF_ATTR_UNSIGNED
        },
#if !defined(DSL_63138) && !defined(DSL_63148) && !defined(BCM63158)
        { .name = "vlan_action",
            .help = "VLAN action object. Notice: setting this attribute run over per port vlan action",
            .type = bdmf_attr_object, .size = sizeof(bdmf_object_handle), .ts.ref_type_name = "vlan_action",
            .offset = offsetof(rdpa_ic_result_t, vlan_action)
        },
#endif
        { .name = "opbit_remark", .help = "Outer pbit remark flag",
            .type = bdmf_attr_boolean,
            .offset = offsetof(rdpa_ic_result_t , opbit_remark), .size = sizeof(bdmf_boolean),
        },
        { .name = "opbit_val", .help = "Outer Pbit remark value",
            .type = bdmf_attr_number, .min_val = 0, .max_val = 7,
            .offset = offsetof(rdpa_ic_result_t , opbit_val), .size = sizeof(rdpa_pbit),
        },
        { .name = "ipbit_remark", .help = "Inner pbit remark flag",
            .type = bdmf_attr_boolean,
            .offset = offsetof(rdpa_ic_result_t , ipbit_remark), .size = sizeof(bdmf_boolean),
        },
        { .name = "ipbit_val", .help = "Inner Pbit remark value",
            .type = bdmf_attr_number, .min_val = 0, .max_val = 7,
            .offset = offsetof(rdpa_ic_result_t , ipbit_val), .size = sizeof(rdpa_pbit),
        },
        { .name = "dscp_remark", .help = "DSCP remark flag",
            .type = bdmf_attr_boolean,
            .offset = offsetof(rdpa_ic_result_t , dscp_remark), .size = sizeof(bdmf_boolean),
        },
        { .name = "dscp_val", .help = "DSCP remark value",
            .type = bdmf_attr_number, .min_val = 0, .max_val = 63,
            .offset = offsetof(rdpa_ic_result_t , dscp_val), .size = sizeof(rdpa_dscp),
        },
#if !defined(DSL_63138) && !defined(DSL_63148) && !defined(WL4908) && !defined(BCM63158) /* TODO! do we need this to support GPON on 63158 */
        { .name = "pbit_to_gem_table", .help = "pBit to GEM Mapping table",
            .type = bdmf_attr_object, .offset = offsetof(rdpa_ic_result_t , pbit_to_gem_table), 
            .ts.ref_type_name = "pbit_to_gem", .size = sizeof(bdmf_object_handle),
        },
#endif
        {.name = "action_vec", .help = "Vector of actions to perfrom on the flow", .size = sizeof(uint16_t),
            .type = bdmf_attr_enum_mask, .ts.enum_table = &rdpa_ic_act_vect_enum_table,
            .offset = offsetof(rdpa_ic_result_t, action_vec)
        },
        { .name = "service_queue_id", .help = "service queue id", .size = sizeof(bdmf_index),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_ic_result_t , service_q_id),
            .flags = BDMF_ATTR_HAS_DISABLE, .disable_val = BDMF_INDEX_UNASSIGNED,
        },
        { .name = "dei", .help = "DEI command", .size = sizeof(rdpa_ic_dei_command),
            .type = bdmf_attr_enum, .ts.enum_table = &rdpa_ic_dei_command_enum_table,
            .offset = offsetof(rdpa_ic_result_t , dei_command),
        },
        { .name = "trap_reason", .help = "Trap reason",
            .type = bdmf_attr_enum, .ts.enum_table = &rdpa_ic_trap_reason_enum_table,
            .size = sizeof(rdpa_cpu_reason), .offset = offsetof(rdpa_ic_result_t, trap_reason),
        },
        { .name = "include_mcast", .help = "Include mcast flow flag",
            .type = bdmf_attr_boolean,
            .offset = offsetof(rdpa_ic_result_t, include_mcast), .size = sizeof(bdmf_boolean),
        },
        BDMF_ATTR_LAST
    }
};
#else /* WL4908 */
/* classification_result aggregate type */
struct bdmf_aggr_type classification_result_type =
{
    .name = "classification_result", .struct_name = "rdpa_ic_result_t ",
    .help = "Ingress classification result",
    .fields = (struct bdmf_attr[])
    {
        { .name = "wan_flow", .help = "WAN Flow instance",
            .type = bdmf_attr_number, /* WAN object can be either GEM or another type, no need for ref_type_name */
            .offset = offsetof(rdpa_ic_result_t , wan_flow), .size = sizeof(uint8_t),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "policer", .help = "Traffic policer id (ignored for multicast)",
            .type = bdmf_attr_object, .size = sizeof(bdmf_object_handle), .ts.ref_type_name = "policer",
            .offset = offsetof(rdpa_ic_result_t, policer)
        },
        { .name = "queue_id", .help = "Egress queue id", .size = sizeof(uint32_t),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_ic_result_t , queue_id),
            .flags = BDMF_ATTR_UNSIGNED
        },
        { .name = "opbit_val", .help = "Outer Pbit remark value",
            .type = bdmf_attr_number, .min_val = 0, .max_val = 7,
            .offset = offsetof(rdpa_ic_result_t , opbit_val), .size = sizeof(rdpa_pbit),
        },
        { .name = "ipbit_val", .help = "Inner Pbit remark value",
            .type = bdmf_attr_number, .min_val = 0, .max_val = 7,
            .offset = offsetof(rdpa_ic_result_t , ipbit_val), .size = sizeof(rdpa_pbit),
        },
        { .name = "dscp_val", .help = "DSCP remark value",
            .type = bdmf_attr_number, .min_val = 0, .max_val = 63,
            .offset = offsetof(rdpa_ic_result_t , dscp_val), .size = sizeof(rdpa_dscp),
        },
        { .name = "service_queue_id", .help = "service queue id", .size = sizeof(bdmf_index),
            .type = bdmf_attr_number, .offset = offsetof(rdpa_ic_result_t , service_q_id),
            .flags = BDMF_ATTR_HAS_DISABLE, .disable_val = BDMF_INDEX_UNASSIGNED,
        },
        BDMF_ATTR_LAST
    }
};
#endif /* WL4908 */
DECLARE_BDMF_AGGREGATE_TYPE(classification_result_type);

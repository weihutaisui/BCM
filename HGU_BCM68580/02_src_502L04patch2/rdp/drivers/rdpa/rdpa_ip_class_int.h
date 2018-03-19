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


#ifndef _RDPA_IP_CLASS_INT_H_
#define _RDPA_IP_CLASS_INT_H_

#include <bdmf_interface.h>
#include "rdpa_ip_class_basic.h"
#include "rdpa_cpu_basic.h"
#include "rdpa_ip_class.h"
#ifdef XRDP
#include "rdd_tuple_lkp.h"
#endif

#define RDPA_MAX_IP_FLOW (RDD_CONTEXT_TABLE_SIZE)

struct rdp_v6_subnets
{
    bdmf_ipv6_t src;
    bdmf_ipv6_t dst;
    uint32_t refcnt; /* zero means subnet is not in use */
};

struct ip_flow_index_list_entry
{
    DLIST_ENTRY(ip_flow_index_list_entry) list;
    bdmf_index index; /* Channel result index in RDD */
};

typedef struct ip_flow_index_list_entry ip_flow_index_list_entry_t;
DLIST_HEAD(ip_flow_index_list_t, ip_flow_index_list_entry);

/* ip_class object private data */
typedef struct {
    uint32_t num_flows;     /**< Number of configured IP flows */
    rdpa_ip_class_method op_method; /** Operational method of the IP class */
    rdpa_l4_filter_cfg_t l4_filters[RDPA_MAX_L4_FILTERS]; /**< L4 filters configuration */
    uint32_t l4_filter_stat[RDPA_MAX_L4_FILTERS]; /**< L4 filter statistics shadow */
    bdmf_mac_t routed_mac[RDPA_MAX_ROUTED_MAC]; /**< Router mac address */
    rdpa_gre_tunnel_cfg_t gre_tunnels[RDPA_MAX_GRE_TUNNELS]; /**list of GRE tunnels */
    rdpa_fc_bypass fc_bypass_mask;/**< mac mode bitmask */
    rdpa_key_type ip_key_type;/**< ip flow key type */
    struct ip_flow_index_list_t ip_flow_index_list; /**< Classification contexts list */
} ip_class_drv_priv_t;

#define ECN_IN_TOS_SHIFT 2

int ip_class_attr_flow_status_read(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index, void *val,
    uint32_t size);
int ip_class_attr_flow_stat_read(struct bdmf_object *mo, struct bdmf_attr *ad,
    bdmf_index index, void *val, uint32_t size);
int ip_class_attr_flow_find(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index *index,
    void *val, uint32_t size);
void remove_all_flows(struct bdmf_object *mo);
int ip_class_attr_flow_delete_ex(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index);
int ip_class_attr_flow_add_ex(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index *index, const void *val,
    uint32_t size);
int ip_class_attr_flow_write(struct bdmf_object *mo, struct bdmf_attr *ad,
    bdmf_index index, const void *val, uint32_t size);
int ip_class_attr_flow_read(struct bdmf_object *mo, struct bdmf_attr *ad,
    bdmf_index index, void *val, uint32_t size);
int ip_class_attr_flow_delete(struct bdmf_object *mo, struct bdmf_attr *ad, bdmf_index index);

int ip_flow_can_change_on_fly_params_ex(const rdpa_ip_flow_info_t *const info, rdd_fc_context_t *rdd_ip_flow_ctx);
void ip_class_prepare_new_rdd_ip_flow_params_ex(rdpa_ip_flow_info_t *const info, rdd_fc_context_t *rdd_ip_flow_ctx);
void ip_class_rdd_ip_flow_cpu_vport_cfg_ex(rdpa_ip_flow_info_t *const info, rdd_fc_context_t *rdd_ip_flow_ctx);

#endif /* _RDPA_IP_CLASS_INT_H_ */

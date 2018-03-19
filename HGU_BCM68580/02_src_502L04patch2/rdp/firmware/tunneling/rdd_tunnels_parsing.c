/*
   Copyright (c) 2014 Broadcom Corporation
   All Rights Reserved

    <:label-BRCM:2014:DUAL/GPL:standard

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
#include "rdd_tunnels_parsing.h"

void rdd_tunnels_parsing_enable(const rdd_module_t *module, bdmf_boolean enable)
{
    RDD_TUNNELS_PARSING_CFG_TUNNELING_ENABLE_WRITE_G(enable, RDD_TUNNELS_PARSING_CFG_ADDRESS_ARR, 0);
}

int rdd_tunnels_parsing_init(const rdd_module_t *module)
{
    RDD_TUNNELS_PARSING_CFG_DTS cfg_entry = {};

    cfg_entry.tunneling_enable = ((tunnels_parsing_params_t *)module->params)->tunneling_enable;
    cfg_entry.res_offset = module->res_offset;

    RDD_TUNNELS_PARSING_CFG_RES_OFFSET_WRITE_G(cfg_entry.res_offset, RDD_TUNNELS_PARSING_CFG_ADDRESS_ARR, 0);
    RDD_TUNNELS_PARSING_CFG_TUNNELING_ENABLE_WRITE_G(cfg_entry.tunneling_enable, RDD_TUNNELS_PARSING_CFG_ADDRESS_ARR, 0);
    return BDMF_ERR_OK;
}

void rdd_ds_lite_tunnel_cfg(bdmf_ipv6_t *ipv6_src_ip, bdmf_ipv6_t *ipv6_dst_ip)
{
    uint32_t i;

    for (i = 0; i < RDD_DUAL_STACK_LITE_ENTRY_SRC_IP_NUMBER; i++)
        RDD_DUAL_STACK_LITE_ENTRY_SRC_IP_WRITE_G(ipv6_src_ip->data[i], RDD_DUAL_STACK_LITE_TABLE_ADDRESS_ARR, 0, i);

    for (i = 0; i < RDD_DUAL_STACK_LITE_ENTRY_DST_IP_NUMBER; i++)
        RDD_DUAL_STACK_LITE_ENTRY_DST_IP_WRITE_G(ipv6_dst_ip->data[i], RDD_DUAL_STACK_LITE_TABLE_ADDRESS_ARR, 0, i);
}

void rdd_gre_tunnel_cfg(uint32_t tunnel_idx, bdmf_mac_t *src_mac, bdmf_mac_t *dst_mac, bdmf_ipv4 ipv4_src_ip, bdmf_ipv4 ipv4_dst_ip)
{
    uint32_t i;

    for (i = 0; i < RDD_LAYER2_GRE_TUNNEL_ENTRY_SRC_MAC_NUMBER; i++)
        RDD_LAYER2_GRE_TUNNEL_ENTRY_SRC_MAC_WRITE_G(src_mac->b[i], RDD_LAYER2_GRE_TUNNEL_TABLE_ADDRESS_ARR, i, tunnel_idx);

    for (i = 0; i < RDD_LAYER2_GRE_TUNNEL_ENTRY_DST_MAC_NUMBER; i++)
        RDD_LAYER2_GRE_TUNNEL_ENTRY_DST_MAC_WRITE_G(dst_mac->b[i], RDD_LAYER2_GRE_TUNNEL_TABLE_ADDRESS_ARR, i, tunnel_idx);

    RDD_LAYER2_GRE_TUNNEL_ENTRY_SRC_IP_WRITE_G(ipv4_src_ip, RDD_LAYER2_GRE_TUNNEL_TABLE_ADDRESS_ARR, tunnel_idx);
    RDD_LAYER2_GRE_TUNNEL_ENTRY_DST_IP_WRITE_G(ipv4_dst_ip, RDD_LAYER2_GRE_TUNNEL_TABLE_ADDRESS_ARR, tunnel_idx);
}


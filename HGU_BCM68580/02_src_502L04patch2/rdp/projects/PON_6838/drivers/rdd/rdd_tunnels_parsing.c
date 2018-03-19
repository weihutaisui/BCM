/*
   Copyright (c) 2016 Broadcom
   All Rights Reserved

    <:label-BRCM:2016:DUAL/GPL:standard

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

extern void f_global_ingress_vector_config ( uint32_t, rdpa_traffic_dir, bdmf_boolean );

void rdd_oren_tunnels_parsing_enable( bdmf_boolean  xi_tunneling_enable )
{
    f_global_ingress_vector_config ( GLOBAL_INGRESS_CONFIG_TUNNELING_ENABLE, rdpa_dir_ds, xi_tunneling_enable);
}

BL_LILAC_RDD_ERROR_DTE rdd_dual_stack_lite_tunnel_config ( bdmf_ipv6_t  *xi_ipv6_src_ip,
                                                           bdmf_ipv6_t  *xi_ipv6_dst_ip )
{
    RDD_DUAL_STACK_LITE_TABLE_DTS  *dual_stack_lite_table_ptr;
    RDD_DUAL_STACK_LITE_ENTRY_DTS  *dual_stack_lite_entry_ptr;
    uint32_t                       i,j;

    dual_stack_lite_table_ptr = ( RDD_DUAL_STACK_LITE_TABLE_DTS * )(DEVICE_ADDRESS( RUNNER_COMMON_1_OFFSET ) + DUAL_STACK_LITE_TABLE_ADDRESS - sizeof ( RUNNER_COMMON ) );

    for ( j = 0; j < RDD_DUAL_STACK_LITE_TABLE_SIZE; j++ )
    {
        dual_stack_lite_entry_ptr = &(dual_stack_lite_table_ptr->entry[j]); 

        for ( i = 0; i < 16; i++ )
        {
            RDD_DUAL_STACK_LITE_ENTRY_SRC_IP_WRITE ( xi_ipv6_src_ip->data[ i ], dual_stack_lite_entry_ptr, i );
            RDD_DUAL_STACK_LITE_ENTRY_DST_IP_WRITE ( xi_ipv6_dst_ip->data[ i ], dual_stack_lite_entry_ptr, i );
        }
    }
    
    return ( BL_LILAC_RDD_OK );
}

void rdd_gre_tunnel_cfg(uint32_t tunnel_idx, bdmf_mac_t *src_mac, bdmf_mac_t *dst_mac, bdmf_ipv4 ipv4_src_ip, bdmf_ipv4 ipv4_dst_ip)
{
    RDD_LAYER2_GRE_TUNNEL_TABLE_DTS *layer2_gre_table;
    RDD_LAYER2_GRE_TUNNEL_ENTRY_DTS *layer2_gre_entry;
    uint32_t i;

    layer2_gre_table = RDD_LAYER2_GRE_TUNNEL_TABLE_PTR();
    layer2_gre_entry = &(layer2_gre_table->entry[tunnel_idx]);

    for (i = 0; i < RDD_LAYER2_GRE_TUNNEL_ENTRY_SRC_MAC_NUMBER; i++)
        RDD_LAYER2_GRE_TUNNEL_ENTRY_SRC_MAC_WRITE(src_mac->b[i], layer2_gre_entry, i);

    for (i = 0; i < RDD_LAYER2_GRE_TUNNEL_ENTRY_DST_MAC_NUMBER; i++)
        RDD_LAYER2_GRE_TUNNEL_ENTRY_DST_MAC_WRITE(dst_mac->b[i], layer2_gre_entry, i);

    RDD_LAYER2_GRE_TUNNEL_ENTRY_SRC_IP_WRITE(ipv4_src_ip, layer2_gre_entry);
    RDD_LAYER2_GRE_TUNNEL_ENTRY_DST_IP_WRITE(ipv4_dst_ip, layer2_gre_entry);
}

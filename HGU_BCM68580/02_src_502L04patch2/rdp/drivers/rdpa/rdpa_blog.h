/*
* <:copyright-BRCM:2017:DUAL/GPL:standard
* 
*    Copyright (c) 2017 Broadcom 
*    All Rights Reserved
* 
* Unless you and Broadcom execute a separate written software license
* agreement governing use of this software, this software is licensed
* to you under the terms of the GNU General Public License version 2
* (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
* with the following added to such license:
* 
*    As a special exception, the copyright holders of this software give
*    you permission to link this software with independent modules, and
*    to copy and distribute the resulting executable under terms of your
*    choice, provided that you also meet, for each linked independent
*    module, the terms and conditions of the license of that module.
*    An independent module is a module which is not derived from this
*    software.  The special exception does not apply to any modifications
*    of the software.
* 
* Not withstanding the above, under no circumstances may you combine
* this software in any way with any other Broadcom software provided
* under a license other than the GPL, without Broadcom's express prior
* written consent.
* 
* :> 
*/

#ifdef RDP_SIM /* Don't include this file if not RDP_SIM */
#ifndef _RDPA_BLOG_H_
#define _RDPA_BLOG_H_

#include "pktrunner_blog.h"

typedef struct {
    uint8_t         channel;        /* e.g. port number, txchannel, ... */
    uint8_t         phy_hdr_len;
    uint8_t         phy_hdr_type;
    bdmf_boolean    GRE_ETH;        /* Ethernet over GRE */
    bdmf_boolean    BCM_XPHY;       /* e.g. BCM_XTM */
    bdmf_boolean    BCM_SWC;
    bdmf_boolean    ETH_802x;
    bdmf_boolean    VLAN_8021Q;    
    bdmf_boolean    PPPoE_2516;
    bdmf_boolean    PPP_1661;
    bdmf_boolean    PLD_IPv4;
    bdmf_boolean    PLD_IPv6;
    bdmf_boolean    PPTP;
    bdmf_boolean    L2TP;
    bdmf_boolean    GRE;
    bdmf_boolean    ESP; 
    bdmf_boolean    DEL_IPv4;
    bdmf_boolean    DEL_IPv6;
    bdmf_boolean    PLD_L2;
}  rdpa_blog_info_t;
 
 
/** Blog Tuple\n
 * Rx or Tx Tuple type \n
 * \n
 */
typedef struct {
    bdmf_ip_t       src_ip;    /* Source IP address, in GRE mode should be 0 */
    bdmf_ip_t       dst_ip;    /* Destination IP address, in GRE mode should be call ID*/
    uint16_t        src_port;  /* Source port */
    uint16_t        dst_port;  /* Destination port */
    uint8_t         prot;      /* Protocol / next header */
    uint8_t         ttl;       /* IP header ttl/hop count*/
} rdpa_blog_ip_tuple_shared_t;


typedef struct {
    rdpa_blog_ip_tuple_shared_t header;
    uint8_t                     tos;       /* IP header tos */
    uint16_t                    check;     /* checksum */
} rdpa_blog_ipv4_tuple_t;


typedef struct {
    rdpa_blog_ip_tuple_shared_t header;
    uint32_t                    word0;
    uint16_t                    length; 
    uint8_t                     tx_hop_limit;
    uint8_t                     exthdrs;        /* Bit field of IPv6 extension headers */
    bdmf_boolean                fragflag;       /* 6in4 Upstream IPv4 fragmentation flag */
    bdmf_boolean                tunnel;         /* Indication of IPv6 tunnel */
    uint16_t                    ipid;           /* 6in4 Upstream IPv4 identification */
} rdpa_blog_ipv6_tuple_t;


typedef struct {
    rdpa_blog_ipv4_tuple_t  tuple;                     /* L3+L4 IP Tuple log */
    rdpa_blog_info_t        info;
    bdmf_boolean            multicast;                 /* multicast flag */
    uint8_t                 count;                     /* # of L2 encapsulations */
    uint8_t                 length;                    /* L2 header total length */
    uint32_t                encap[BLOG_ENCAP_MAX];     /* All L2 header types */
    uint8_t                 l2hdr[BLOG_HDRSZ_MAX];     /* Data of all L2 headers */
} rdpa_blog_header_t;


typedef struct {
    uint32_t                wfd_rnr;
    uint8_t                 vtag_num;
    uint16_t                eth_type;
    uint8_t                 lag_port;        /* LAG port when trunking is done by internal switch/runner */
    uint8_t                 ack_cnt;         /* back to back TCP ACKs for prio */
    bdmf_boolean            ptm_us_bond;     /* PTM US Bonding Mode */
    bdmf_boolean            tos_mode_us;     /* ToS mode for US: fixed, inherit */
    bdmf_boolean            tos_mode_ds;     /* ToS mode for DS: fixed, inherit */
    bdmf_boolean            has_pppoe;
    bdmf_boolean            ack_done;        /* TCP ACK prio decision made */
    bdmf_boolean            nf_dir_pld;
    bdmf_boolean            nf_dir_del;
    bdmf_boolean            pop_pppoa;
    bdmf_boolean            insert_eth;
    bdmf_boolean            iq_prio;
    bdmf_boolean            mc_sync;
    bdmf_boolean            rtp_seq_chk;     /* RTP sequence check enable       */
    bdmf_boolean            incomplete;
    uint32_t                mark;
    uint32_t                priority;        /* Tx  priority */    
    uint8_t                 blog_rule_count; /* Count of blog rules (0 or 1 for now) */
    uint8_t                 blog_rule_index; /* Index into our blog rule table.  When we build the actual blog, this will be represented with a pointer. */
    uint8_t                 mcast_learn;
    uint32_t                vtag0;
    uint32_t                vtag1;
    rdpa_blog_ipv6_tuple_t  tupleV6;
    rdpa_blog_header_t      tx;
    rdpa_blog_header_t      rx;
} rdpa_blog_entry_t;


typedef struct {
    bdmf_boolean    is_wan_port;
    uint16_t        tx_mtu;
} rdpa_blog_logical_port_t;


/* Blog Rule Types for RDPA */
typedef struct {
    uint32_t        nbr_of_vlan_tags;
    uint32_t        has_pppoe_header;
    uint8_t         ipv4_tos_mask;
    uint8_t         ipv4_tos_value;
    uint8_t         ipv4_ip_proto_mask;
    uint8_t         ipv4_ip_proto_value;
    uint8_t         ipv6_tclass_mask;
    uint8_t         ipv6_tclass_value;
    uint8_t         ipv6_next_hdr_mask;
    uint8_t         ipv6_next_hdr_value;
    uint32_t        skb_priority;
    uint16_t        skb_mark_flow_id;
    uint16_t        skb_mark_port;
    uint32_t        flags;
} rdpa_blog_rule_filter_t;

/* Blog Rule Action Type */
/* Note that the actions are passed as a 'buffer' because embedded arrays aren't supported by rdpa. */
/* Size of block rule action structure for use as a 'bdmf_buffer' type. */
#define BLOG_RULE_ACTION_SIZE 10

typedef struct {
    rdpa_blog_rule_filter_t  filter;
    uint32_t                 action_count;
    uint8_t                  action[BLOG_RULE_ACTION_MAX * BLOG_RULE_ACTION_SIZE];
    /* For now blog rules here are only 1 deep, but support may be added later to allow for Blog Rule Linked List */
} rdpa_blog_rule_t;


#endif /* _RDPA_BLOG_H_ */
#endif /* RDP_SIM */

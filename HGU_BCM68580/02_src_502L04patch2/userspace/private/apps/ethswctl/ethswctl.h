/***********************************************************************
 *
 *  Copyright (c) 2007  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2012:proprietary:standard

 This program is the proprietary software of Broadcom and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").  Except as set forth in
 an Authorized License, Broadcom grants no license (express or implied), right
 to use, or waiver of any kind with respect to the Software, and Broadcom
 expressly reserves all rights in and to the Software and all intellectual
 property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,

 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use
    all reasonable efforts to protect the confidentiality thereof, and to
    use this information only in connection with your use of Broadcom
    integrated circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
    PERFORMANCE OF THE SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
    LIMITED REMEDY.
:>
 *
 ************************************************************************/

#ifndef _ETHSWCTL_H_
#define _ETHSWCTL_H_

typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long long u64;

#define ETHSWCTL_VERSION "0.0.2"
#define ETHSWCTL_RELDATE "January 27, 2009"
#define ETHSWCTL_NAME    "ethswctl"
static char *version =
ETHSWCTL_NAME ".c:v" ETHSWCTL_VERSION " (" ETHSWCTL_RELDATE ")\n"
"o Pratapa Reddy Vaka (pvaka@broadcom.com).\n";

#define cpu_to_le64(x) (u64)__cpu_to_le64((u64)(x))
#define le64_to_cpu(x) (u64)__le64_to_cpu((u64)(x))

typedef enum msgs_e {
  HWSWITCHING_MSG = 0,
  MIBDUMP_MSG,
  PAGEDUMP_MSG,
  IUDMADUMP_MSG,
  IUDMASPLIT_MSG,
  SWCTRL_MSG,
  SWPRIOCTRL_MSG,
#if defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_4908) || defined(CHIP_63158)
  SWFLOWCTRL_MSG,
  SWQUEMAP_MSG,
  SWQUEMON_MSG,
  SWACB_MSG,
  SWCFPTCAM_MSG,
#endif
  RXSCHED_MSG,
  WRRPARAMS_MSG,
  VLAN_MSG,
  PBVLANMAP_MSG,
  GETSTAT_MSG,
  GETSTAT32_MSG,
  PAUSE_MSG,
  COSQ_MSG,
  COSTXQ_MSG,
  BYTERATECFG_MSG,
  BYTERATE_MSG,
  PKTRATECFG_MSG,
  PKTRATE_MSG,
  DEFTXQ_MSG,
  DEFTXQEN_MSG,
  GETRXCNTRS_MSG,
  RESETRXCNTRS_MSG,
  ARL_MSG,
  ARLDUMP_MSG,
  ARLFLUSH_MSG,
  UNTAGGEDPRIO_MSG,
  COSQPRIOMETHOD_MSG,
  COSQSCHED_MSG,
  PORTCTRL_MSG,
  PORTLOOPBACK_MSG,
  PHYMODE_MSG,
  TEST_MSG,
  JUMBO_MSG,
  PID2PRIO_MSG,
  PCP2PRIO_MSG,
  DSCP2PRIO_MSG,
  REGACCESS_MSG,
  SPIACCESS_MSG,
  PSEUDOMDIOACCESS_MSG,
  SWITCHINFO_MSG,
  SETLINKSTATUS_MSG,
  PORTRXRATECTRL_MSG,
  PORTTXRATECTRL_MSG,
#if defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_4908) || defined(CHIP_63158)
  PORTTXRATECFG_MSG,
#endif
  DOSCTRLCTRL_MSG,
  SOFTSWITCH_MSG,
  HWSTP_MSG,
  WAN_MSG,
  PORT_MIRROR_MSG,
  PORT_TRUNK_MSG,
#if defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_4908) || defined(CHIP_63158)
  PORT_STORM_CTRL_MSG,
#endif
  OAMIDX_MSG,
  MAX_NUM_COMMANDS,
}msgs_t;

static const char *usage_msgs[] =  {
"    ethswctl {-c|--command} hw-switching {-o|--operation} <enable|disable>\n",
"    ethswctl {-c|--command} mibdump [-n <unit>] {-p|--port} <port> [-q <queue>] {-a|--all}\n"
"          -q <queue> is meaningful for chip only mornitoring single queue each time\n"
"             The queue packet counter will be reset when mornitored queue changes\n",
"    ethswctl {-c|--command} pagedump {-P|--Page} <page>\n",
"    ethswctl {-c|--command} iudma [-n <channel>] [-r] [-w] [-d <count>] [-b <byte>] [-e <0|1>]\n"
"          Display/Control Robo Switch DMA channel\n"
"          When -n is omit, all channels will be displayed, -n 100 means GMAC DMA channel\n"
"          -d <count> Display <count> number of descriptor entries started from current pointer backward, \n"
"             <count>=-1 means all descriptor entries\n"
"          -r Display DMA Rx channels only\n"
"          -w Display DMA Tx Ouput channels only\n"
"          -b <bytes> Display buffer content <bytes> length, <bytes>=-1 means full pakcet dump\n"
"          -e <1|0> Enable(1)/Disable(0) a DMA channel, -n <channel> needes for this operation\n",
"    ethswctl {-c|--command} iudmasplit -p <port> {-q <iudma>}\n",
"    ethswctl {-c|--command} swctrl {-t|--type} <type> [{-v|--value} <val>]\n"
"          type = 0-BUFF_CTRL; val = 1-TXQ_PAUSE_EN, 2-TXQ_DROP_EN, 4-TOT_PAUSE_EN, 8-TOT_DROP_EN\n"
"                 1-8021Q_CTRL; val = 0-DISABLE, 0x1-ENABLE, Shared VLAN learning, 0x2-ENABLE, Individual VLAN learning\n",
#if defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_4908) || defined(CHIP_63158)
"    ethswctl {-c|--command} swprioctrl [-m mode] [{-q|--priority} <prio>] [-l <maxStreams>]\n"
"               [{-t|--type} <type>] [{-v|--value} <val>] [-n <unit>] [-p <port>]\n"
"               Queue Threshold Configuration Command\n"
"          mode = 0-Dynamic Mode: Queue Thresholds are configured by Ethernet driver\n"
"                         based on WAN queue map and link up port number dynamically.\n"
"                         see ethswctl -c quemap for reference\n"
"                 1-Static Mode: Queue thresholds are configured by Ethernet driver\n"
"                         based on WAN queue map and total port number.\n"
"                         No reconfiguration based on port link up and down.\n"
"                 2-Manual Mode: Queue thresholds are configured manually\n"
"          type = 0-TXQ_RESERVE, 1-TXQ_HI_HYS), 2-TXQ_HI_PAUSE\n"
"                 3-TXQ_HI_DROP, 4-TOT_HYST, 5-TOT_PAUSE, 6-TOT_DROP\n"
"          maxStreams: Max stream number with best system balanced results under\n"
"                 Dynamic or Static mode, -1 to set it back to default value\n",
"    ethswctl {-c|--command} swflowctrl [-n <unit>] -p <port> {-t|--type} <type> [{-v|--value} <val>]\n"
"          type = 0-FC_MODE val=(0-Global, 1-per port)\n"
"                 1-TX_BASED_FC, 2-TXQ_PAUSE_EN, 3-TXQ_DROP_EN, 4-TOT_PAUSE_EN\n"
"                 5-TOT_DROP_EN, 6-QBASED_PAUSE, val=1-ENABLE, 0-DISABLE\n",
"   ethswctl {-c|--command} quemap [-v <WAN Queue Bit Map>] [-q <Ethernet Driver Queue Remapping Map>]\n"
"         Get or Set port WAN Queue Bit Map and WAN Queue Mapping Bit Map\n"
"         <WAN Queue Bit Map>: Each bit of total 8 bits, represents the queue\n"
"                configured for WAN traffic with more buffer allocated than\n"
"                the rest queues configured for LAN traffic\n"
"         <Ethernet Driver Queue Remapping Bit Map>: Each 4 bits represents the queue number\n"
"                to be mapped to. The 4 bit position represent the queue\n"
"                number to be mapped from. Ex. -q 0x76543255 means queu 0 and 1\n"
"                will be both remapped queue 5 by Ethernet driver.\n"
"          Note: The two parameters above need to match each other or configuration will\n"
"                be rejected. Ex. -q 2 -v 0x11111111 will be accepted, but\n"
"                -q 2 -v 0x11110000 will be rejected. since queu 0 is not WAN queue.\n",
"  ethswctl {-c|--command} quemon [-n <unit>] -p <port> [{-q|-queue} <queue#>]\n"
"           [{-t|--type} <type>], [{-v|--value} <val>]\n"
"        type = 0-QueCurCount, 1-QuePeakCount, 2-SysTotalPeakCount, 3-SysTotalCurCount\n"
"               4-PortPeakRxBuffer, 5-QueFinalCongestedStatus, 6-PortPauseHistory,\n"
"               7-PortQuantumPauseHistory, 8-PortRxBasedPauseHistory,\n"
"               9-PortRxBufferErrorHitory, 10-QueCongestedStatus, 11-QueTotalCongestedStatus\n"
"             * Non parameter means all values applied(ex. (no -q)=(all eight queues)).\n"
"             * All counters are in 256byte page unit.\n"
"             * The Peak and History value are read-cleared, thus display values from last command.\n",
"    ethswctl {-c|--command} acb_cfg {-t|--type} <type> [ -q queue] [{-v|--value} <val>]\n"
"          type = 0-read all;"
                  #if defined(ACB_ALGORITHM2)
                  " 1-acb_en; 2-eop_delay; 3-que_flush; 4-algorithm;"
                  #endif
                  "\n"
"                 5-total_xon_hyst 256B init; 6-queue_xon_hyst 256B unit; 7-pessimistic_mode;\n"
"                 8-total_xon_en; 9-xon_en; 10-total_xoff_en; 11-pkt_len, 1..36;\n"
"                 12-total_xoff_thresh 256B unit; 13-xoff_thresh 256Bytes unit;"
                  #if defined(ACB_ALGORITHM2)
                  " 14-pkts_in_flight"
                  #endif
                  "\n"
                  #if defined(ACB_ALGORITHM2)
"                 Setting 4-Algorithm will set multiple fields to default values for that algorithm.\n"
                  #endif
                   ,
"    ethswctl {-c|--command} cfp <Operations> [<Position>] <Matching Conditions> <Actions>\n"
"          ----------------------- <Operations> ---------------------------------------------------------\n"
"          --read                      : Read cfp rules.\n"
"          --add                       : Add a rule to the top of rules.\n"
"          --append                    : Append a rule to the end of rules.\n"
"          --insert                    : Insert a rule in front of a specified rule.\n"
"                                        --priority and --index are mandatory for this operation.\n"
"          --delete                    : Delete a single CFP rule. --priority and --index are mandatory.\n"
"          --delete_all                : Delete multiple CFP rules.\n"
"                                      : If --index omitted, all rules of the priority will be deleted.\n"
"                                      : If --priority omitted, rules of all priorities will be deleted.\n"
"                                      : If both --priority and --index omitted, all rules will be deleted.\n"
"          ----------------------- <Positions> -------------------------------------------------------------\n"
"          --priority <0-2>            : Priority Group 0-2; 2 is the highest; 0 is the lowest priority.\n"
"                                        Priority group has higher priority than rule index (0-255) inside\n"
"                                        a priority group. If this option is omitted, for --read operation,\n"
"                                        all priority groups will be displayed; for non --read operation,\n"
"                                        priority 0 will be used.\n"
"          --index <0-255>             : The priority index inside a priority group, index 0 has the highest\n"
"                                        priority. The operation Insert needs to use this option to specify\n"
"                                        insertion index. If omitted for Read op, all entries inside the\n"
"                                        group will be displayed.\n"
"          ------------------------ <Matching Conditions> ---------------------------------------------------\n"
"          * Each condition is followed by a optional mask, if mask is omitted, all bits will be valid\n"
"          --spmap <sourcePortMap>             : Frame receiving source port map, if omitted, all ports valid\n"
"          --da <hex 6 bytes>,[<mask>]         : Destination MAC address in hex format.\n"
"          --sa <hex 6 bytes>,[<mask>]         : Source MAC address in hex format.\n"
"          --svlan_tag <0-0xffff>,[<mask>]     : SVLAN(TPID=0x9100) 2 bytes Service Privider VLAN Tag\n"
"          --cvlan_tag <0-0xffff>,[<mask>]     : CVLAN(TPID=0x8100) 2 bytes Customer VLAN Tag\n"
"          --l2 <L2_Framing>                   : 0-EthernetII; 1-SNAP Public; 2-LLC; 3-SNAP Private\n"
"          --pppoe <1|0>                       : PPPoE session or not\n"
"          --etype_sap <0-0xffff>,[<mask>]     : Ethernet Type when L2=EthernetII or SNAP_Public,\n"
"                                                or {DSAP,SSAP} when L2=LLC or SNAP_Private.\n"
"          --l3 <L3_Framing>                   : 0-IPv4, 1-IPv6, 2-Non IP, when omitted, IPv4 will be used\n"
"          --dscp <0-0x3f>,[<mask>]            : IP DSCP field\n"
"          --ip_protocol <0-255>,[<mask>]      : IP Protocol Number or IPv6 Next Hearder Protocol Number\n"
"          --ipsa <IPv4 address>,[<mask>]      : Source IP address.\n"
"          --ipda <IPv4 address>,[<mask>]      : Destination IP address.\n"
"          --tcp_udp_sport <2 byte value>,[<mask>]    : TCP/UDP source port.\n"
"          --tcp_udp_dport <2 bype value>,[<mask>]    : TCP/UDP destination port.\n"
"          ----------------------- <Actions> ---------------------------------------------------------------\n"
"          --new_dscp_ib <0-0x3f>      : New change in-band traffic's DSCP to a new value\n"
"          --chng_fpmap_ib <1-3>       : Change in-band traffic Forwarding Port Map\n"
"                                      : 1-Remove DST_MAP from ARL result; 2-Replace ARL result with DST_MAP\n"
"                                        3-Add DST_MAP to ARL result\n"
"                                        If omitted and --dst_fpmap_ib set, replacement will be performed\n"
"          --fpmap_ib <0-0x1ff>        : New in-band Forwarding Port Map used by --chng_fwd_map_ib\n"
"          ----------------------- <Examples> --------------------------------------------------------------\n"
"          ethswctl -c cfp --add --da 102233445566 --chng_fpmap_ib 2 --fpmap_ib 0\n"
"                  # Discard matched DA packets.\n"
"          ethswctl -c cfp --add --ipda 192.168.0.0,255.255.0.0 --chng_fpmap_ib 2 --fpmap_ib 0x108\n" 
"                  # force packet with destination IP address 192.168.0.0:255.255.0.0 to port 8 and 3.\n"
,
#else
"    ethswctl {-c|--command} swprioctrl {-q|--priority} <prio>\n"
"                 {-t|--type} <type> {-v|--value} [val]\n"
"          type = 0-TOT_DROP, 1-TOT_PAUSE, 2-TOT_HYST, 3-TXQ_HI_DROP,\n"
"                 4-TXQ_HI_PAUSE, 5-TXQ_HI_HYST, 6-TXQ_LO_DROP\n",
#endif
"    ethswctl {-c|--command} rxscheduling {-v|--value} [val]\n"
"          val = 0-SP, 1-WRR\n",
"    ethswctl {-c|--command} wrrparams {-t|--type} <type> {-v|--value} [val]\n"
"          type = 0-interval_in_pkts, 1-weight1, 2-weight2, 3-weight3, 4-weight4\n"
"          val = interval_in_pkts:<1-1280>; weight:<1-16>\n",
"    ethswctl {-c|--command} vlan {-v|--value} vlan_id {-f|--fwdmap}\n"
"          fwd_map {-u|-untagmap} untag_map\n",
"    ethswctl {-c|--command} pbvlanmap [-n <unit>] {-p|--port} <port> [{-v|--value} <val>]\n"
"          val = port_map; Bit8: MIPS, Bit7: GPON; Bit5: MoCA: Bits3-0: UNI\n",
"    ethswctl {-c|--command} getstat [-n <unit>] -p <port> -t <type>\n",
"    ethswctl {-c|--command} getstat32 [-n <unit>] -p <port> -t <type>\n",
"    ethswctl {-c|--command} pause [-n <unit>] -p <port> -v val\n"
"          val = 0-None, 1-Auto, 2-Both, 3-Tx only, 4-Rx only\n",
"    ethswctl {-c|--command} cosq [-v <val>] # Get/Set Number of TxQ\n"
"    ethswctl {-c|--command} cosq [-n <unit>] -p <port> -q <priority> -v <txq>\n"
"          Set Packet reveived at <port> with CoS <priority> to TxQueue number <txq>\n"
"    ethswctl {-c|--command} cosq -q <priority> -v <dmaChannel>\n"
"               Set IMP port Tx Queue of <priority> to DMA Channel of <dmaChannel>\n",
"    ethswctl {-c|--command} costxq [-v <channel> -q <queue>] [-t <method>]\n"
"          method = 0-USE_BD_PRIO, 1-USE_TX_IUDMA\n",
"    ethswctl {-c|--command} byteratecfg -v <channel> -x {0-disable, 1-enable}\n",
"    ethswctl {-c|--command} byterate -v <channel> -x <rate>\n",
"    ethswctl {-c|--command} pktratecfg -v <channel> -x {0-disable, 1-enable}\n",
"    ethswctl {-c|--command} pktrate -v <channel> -x <rate>\n",
"    ethswctl {-c|--command} deftxq -i <if_name> -v <val>\n",
"    ethswctl {-c|--command} deftxqen -i <if_name> -o <enable|disable>\n",
"    ethswctl {-c|--command} getrxcntrs\n",
"    ethswctl {-c|--command} resetrxcntrs\n",
"    ethswctl {-c|--command} arl -m mac [-v <vid>] [-x <data>] [-n <unit>] [-r]\n"
"          data: 15:Valid, 14:Static, 13:Age, 12-10:Priority,\n"
"                8-0:Port ID for Unicast or Port Bit Map for Multicast\n"
"          <-x 0> <-v vid> will remove the ARL entry in a specific VLAN\n"
"          <-x 0> <-v -1> will remove ARL entries in all VLANs\n",
"    ethswctl {-c|--command} arldump\n",
"    ethswctl {-c|--command} arlflush\n",
"    ethswctl -c untaggedprio [-n <unit>] -p <port> [-v <val>]\n"
"          Set/Get packet default untagged priority in Rx Port <port> with value <val>\n",
#if !defined(CHIP_63138) && !defined(CHIP_63148)  && !defined(CHIP_4908) &&!defined(CHIP_63158)
"    ethswctl -c cosqpriomethod [-n <unit>] -p port [-v val]\n"
"          val = 0-PORT_QOS, 1-MAC_QOS, 2-8021P_QOS, 3-DIFFSERV_QOS\n",
"    ethswctl -c cosqsched -v [val] -t <sp_endq> -w <weight1> -x <weight2>\n"
"               -y <weight3> -z <weight4>\n"
"          val = 0-SP, 1-WRR, 2-COMBO\n"
"          sp_endq: Queue where SP scheduling ends. Valid only for COMBO\n",
#else
"    ethswctl -c cosqpriomethod [-n <unit>] [-p port] [-t <pkt_type_mask>] [-v <val>] [-b]\n"
"          pkt_type_mask = 0x4-Static DA MAC, 0x4-Priority Tagged, 0x1-ipv4/6\n"
"                        mandatory for 63138\n"
"          val = 0-PORT_QOS, 1-MAC_QOS, 2-8021P_QOS, 3-DIFFSERV_QOS\n"
"          -b: Disable specifc QoS of a port(For 8021P_QOS and DIFFSERV_QOS)\n",
"    ethswctl -c cosqsched [-n <unit>] -p <port> [-v val] [-t <sp_queues>] [ -T <deficit_rr>]\n"
"          [ [-o <which_queues>] -x <weight1> -y <weight2> -z <weight3> -w <weight4> ]\n"
"          val = 0-SP, 1-WRR\n"
"          sp_queues: number of SP queues between 1 and 4; Default All queues\n"
"          deficit_rr = 0-Default Weighted round robin, 1-Deficit Round Robin\n"
"          which_queues = 'lower', weights for queue 0..3\n"
"                         'upper', weights for queue 4..7\n"
"                          Default 'lower'\n",
#endif
"    ethswctl {-c|--command} portctrl [-n <unit>] {-p|--port} <port> [-v <val>]\n"
"          val bit-0:  1-Rx Disable, 0-Rx Enable\n"
"          val bit-1:  1-Tx Disable, 0-Tx Enable\n",
"    ethswctl {-c|--command} portloopback [-n <unit>] {-p|--port} <port> [-v <val>]\n"
"          val:  1-Enable, 0-Disable\n",
"    ethswctl {-c|--command} phymode [-n <unit>] {-p|--port} <port> [-y <speed>] [-z <duplex>]\n"
"          speed = 0-auto, 10000, 5000, 2500, 1000, 100, 10\n"
"          duplex: 0-halfduplex, 1-fullduplex\n",
"    ethswctl {-c|--command} test -t <type> [-x <param>] [-v <val>] [-s]\n"
"          type = 0-ISR config, param=channel; val=1|0\n"
"          type = 1-Enable Rx Dump, val=1|0\n"
"          type = 2-Reset MIB\n"
"          type = 3-Reset Switch; -s for SPI interface\n",
"    ethswctl {-c|--command} jumbo [-n <unit>] {-p|--port} <port>\n"
"          [{-v|--value} <0(disable)|1(enable)>]\n"
"          port = 9-ALL, 8-MIPS, 7-GPON, 6-USB, 5-MOCA,\n"
"                 4-GPON_SERDES, 3-GMII_2, 2-GMII_1, 1-GPHY_1, 0-GPHY_0\n",
"    ethswctl -c pid2prio [-n <unit>] -p <port> [-q <prio>]\n"
"          Port id based priority (TC)\n"
"          prio is TC value for the port\n",
"    ethswctl {-c|--command} pcp2prio -v <pcpVal> [-n <unit>] -p <port> [-q <prio>]\n"
"          <unit> is switch number, Default 0\n"
"          <port>is switch port\n"
"          <pcpVal> is the 802.1p priority value\n"
"          <prio> is the switch priority\n",
"    ethswctl -c dscp2prio -v <dscpVal> [-q <prio>] >\n"
"          <dscpVal> is the DSCP value\n"
"          <prio> is the switch priority\n",
"    ethswctl {-c|--command} regaccess -v offset -l len [-d <data>] [-n <unit>]\n",
"    ethswctl {-c|--command} spiaccess -x addr -l len [-d <data>]\n",
"    ethswctl {-c|--command} pmdioaccess -x addr -l len [-d <data>]\n",
"    ethswctl {-c|--command} switchinfo -v switch_number\n",
"    ethswctl {-c|--command} setlinkstatus [-n <unit>] -p port -x linkstatus -y speed -z duplex\n"
"          speed = 10-10Mbps, 100-100Mbps, 1000-1000Mbps\n",
"    ethswctl {-c|--command} rxratectrl [-n <unit>] -p port [-x limit-kbps -y burst-kbits]\n"
"          if -y 0, the Rx rate control will be disabled, thus no rate limitation\n",
#if defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_4908) || defined(CHIP_63158)
"    ethswctl {-c|--command} txratectrl [-n <unit>] -p port [ -q <queue>  -T <pkt or byte>] [-x <limit> -y <burst>]\n"
"          specify -q if configuring per q\n"
"          specify -T 1 if packet mode shaper, or it is Byte mode\n"
"          limit: kbps units in Byte mode, pps in Packet mode\n"
"          burst: kbits units in Byte mode, 1 pkt in Packet mode\n",
"    ethswctl {-c|--command} txratecfg [-n <unit>] -p port [ -q <queue>  ] -t <shaper_mode_mask> -v <val>\n"
"          shaper_mode_mask = 1-Enable Shaper, 2-packet mode else byte mode\n"
"                             4-Shaper Blocked, 8-Include IFG\n"
"          val = 1-enable, 0-disable\n", 
#else
"    ethswctl {-c|--command} txratectrl [-n <unit>] -p port [-x limit-kbps -y burst-kbits]\n",
#endif
"    ethswctl {-c|--command} dosctrl -v <value>\n"
"          <value> = 1-IP_LAN, 2-TCP_BLAT, 3-UDP_BLAT, 4-TCP_NULL\n"
"                    5-TCP_XMAS, 6-TCP_SYNFIN, 7-TCP_SYNERR, 8-TCP_SHORTHDR\n"
"                    9-TCP_FRAGERR, 10-ICMPv4_FRAG, 11-ICMPv6_FRAG, 12-ICMPv4_LONGPING\n"
"                    13-ICMPv6_LONGPING, 14-DOS_LRN\n",
"    ethswctl {-c|--command} softswitch [-i] <if_name> [-o] <enable|disable>\n",
"    ethswctl {-c|--command} hwstp [-i] <if_name> [-o] <enable|disable>\n",
"    ethswctl {-c|--command} wan [ -i if_name -o <enable|disable> ]\n",
"    ethswctl {-c|--command} mirror -o <enable|disable> [-n <unit>] [-p <mirror_port> -x <ingress_mirror_port_map>\n"
"               -y <egress_mirror_port_map> [-z <block_no_mirror_pkts 0|1>]\n"
"               [-v <TX packet mirror port map>] [-w <RX packet mirror port map>\n"
"          Note: -v and -w are applicable if not supplied all traffic is mirrored\n"
"                to <mirror_port> in Runner platform\n",
"    ethswctl {-c|--command} trunk [ -o <sada|da|sa> ]\n",
#if defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_4908) || defined(CHIP_63158)
"    ethswctl {-c|--command} stormctrl [-n <unit>] -p port [-t pkt_type_mask -x rate -y bucket_size]\n"
"          pkt_type_mask = 0x1(unicast lookup hit), 0x2(multicast lookup hit), 0x4(reserved mac addr 01-80-c2-00-00- 00~2f)\n"
"                         0x8(broadcast), 0x10(multicast lookup fail), 0x20(unicast lookup fail)\n"
"          rate = 1~28   (Bit rate = rate*8*1024/125, that is 64Kb~1.792Mb with resolution 64Kb)\n"
"                 29~127 (Bit rate = (rate-27)1024, that is 2Mb~100Mb with resolution 1Mb)\n"
"                 128~240 (Bit rate = (rate-115)*8*1024, that is 104Mb~1000Mb with resolution 8Mb)\n"
"          bucket_size = 0(4K), 1(8K), 2(16K), 3(32K), 4(64K), others(488K) bytes\n",
#endif
"    ethswctl {-c|--command} oamindex -v <oam index> -t <0=unit/port; 1=rdpa_if>]\n",
};

/* Command-line flags. */
unsigned int
opt_a = 0,    /* Indicates all or subset for mibdump */
opt_b = 0,    /* disable flag */
opt_c = 0,    /* Command name */
opt_d = 0,    /* data */
opt_e = 0,    /* Enable/Disable */
opt_f = 0,    /* forward_map */
opt_h = 0,    /* Help*/
opt_i = 0,    /* Interface name */
opt_l = 0,    /* Length */
opt_m = 0,    /* MAC address */
opt_n = 0,    /* MAC address */
opt_o = 0,    /* Enable/Disable operation */
opt_p = 0,    /* Port number */
opt_P = 0,    /* Page number */
opt_q = 0,    /* Priority */
opt_r = 0,    /* Read */
opt_s = 0,    /* indicates access to external spi slave device */
opt_t = 0,    /* Type parameter */
opt_T = 0,    /* Type parameter */
opt_u = 0,    /* untag_map */
opt_v = 0,    /* Value*/
opt_V = 0,    /* Version flag */
opt_w = 0,    /* Generic param4 */
opt_x = 0,    /* Generic param1 */
opt_y = 0,    /* Generic param2 */
opt_z = 0;    /* Generic param3 */

#define OPTVAL(x)   (128 + (x))

struct option longopts[] = {
    /* { name  has_arg, *flag  val } */
    {"all",       0, 0, 'a'}, /* mibdump parameter */
    {"command",   1, 0, 'c'}, /* Command Name */
    {"operation", 1, 0, 'o'}, /* Enable/Disable Operation for hw-switching */
    {"port",    1, 0, 'p'}, /* Port Number for mibdump */
    {"Page",    1, 0, 'P'}, /* Page Number for pagedump */
    {"version",   0, 0, 'V'}, /* Emit version information.*/
    {"help",      0, 0, 'h'}, /* Emit help message */
    {"value",     1, 0, 'v'}, /* Value parameter*/
    {"priority",  1, 0, 'q'}, /* Priority */
    {"type",      1, 0, 't'}, /* Type */
    {"length",    1, 0, 'l'},	/* Type */
    {"untagmap",  1, 0, 'u'}, /* VLAN untag map */
    {"fwdmap",    1, 0, 'f'}, /* VLAN fwd map */
    {"param1",    1, 0, 'x'}, /* Generic parameter1 */
    {"param2",    1, 0, 'y'}, /* generic parameter2 */
    {"param3",    1, 0, 'z'}, /* generic parameter3 */
    {"param4",    1, 0, 'w'}, /* Generic parameter4 */
    {"interface", 1, 0, 'i'}, /* Interface name */
    {"mac",       1, 0, 'm'}, /* MAC address */
    {"unit",      1, 0, 'n'},	/* unit */        
    {"length",    1, 0, 'l'},	/* Length */
    {"type",      1, 0, 'T'}, /* wrr Type */

    {"read",        0, 0, OPTVAL(OPT_READ)},         
    {"add",         0, 0, OPTVAL(OPT_ADD)},         
    {"append",      0, 0, OPTVAL(OPT_APPEND)},         
    {"insert",      0, 0, OPTVAL(OPT_INSERT)},         

    {"delete",      0, 0, OPTVAL(OPT_DELETE)},         
    {"delete_all",  0, 0, OPTVAL(OPT_DELETE_ALL)},
    {"index",       1, 0, OPTVAL(OPT_INDEX)},
    {"spmap",       1, 0, OPTVAL(OPT_SPMAP)},

    {"da",          1, 0, OPTVAL(OPT_DA)},
    {"sa",          1, 0, OPTVAL(OPT_SA)},
    {"svlan_tag",   1, 0, OPTVAL(OPT_SVLAN_TAG)},
    {"cvlan_tag",   1, 0, OPTVAL(OPT_CVLAN_TAG)},

    {"pppoe",       1, 0, OPTVAL(OPT_PPPOE)},
    {"etype_sap",   1, 0, OPTVAL(OPT_ETYPE)},
    {"l2",          1, 0, OPTVAL(OPT_L2)},
    {"l3",          1, 0, OPTVAL(OPT_L3)},
    {"dscp",        1, 0, OPTVAL(OPT_DSCP)},
    {"ip_protocol", 1, 0, OPTVAL(OPT_IP_PROTOCOL)},
    {"ipsa",        1, 0, OPTVAL(OPT_IPSA)},
    {"ipda",        1, 0, OPTVAL(OPT_IPDA)},
    {"tcp_udp_sport",   1, 0, OPTVAL(OPT_TCPUDP_SPORT)},

    {"tcp_udp_dport",   1, 0, OPTVAL(OPT_TCPUDP_DPORT)},
/*
    {"slan_vid",    1, 0, OPTVAL(OPT_SVLAN_VID)},
    {"svlan_pcp",   1, 0, OPTVAL(OPT_SVLAN_PCP)},

    {"clan_vid",    1, 0, OPTVAL(OPT_CVLAN_VID)},
    {"cvlan_pcp",   1, 0, OPTVAL(OPT_CVLAN_PCP)},


*/
    {"new_dscp_ib",     1, 0, OPTVAL(OPT_NEW_DSCP_IB)},
    {"chng_fpmap_ib",   1, 0, OPTVAL(OPT_CHANGE_FPMAP_IB)},
    {"fpmap_ib",        1, 0, OPTVAL(OPT_FPMAP_IB)},
    {0, 0, 0, 0}
};

/* Defines for swctrl value parameter */
#define SWITCH_BUFFER_CONTROL   0

/* Defines for swprioctrl type parameter */
enum {
#if defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_4908) || defined(CHIP_63158)
    TXQ_RSERVE_THRESHOLD,
#else
    TXQ_LO_DROP_THRESHOLD,
#endif
    TXQ_HI_HYST_THRESHOLD,
    TXQ_HI_PAUSE_THRESHOLD,
    TXQ_HI_DROP_THRESHOLD,
    TOTAL_HYST_THRESHOLD,
    TOTAL_PAUSE_THRESHOLD,
    TOTAL_DROP_THRESHOLD,
    TXQ_MAX_TYPES,
    TXQ_THRED_CONFIG_MODE,
    GET_TOTAL_PORTS,
    GET_LINK_UP_LAN_PORTS,
    GET_LINK_UP_WAN_PORTS,
    TXQ_MAX_STREAMS,
};
#define SF2_MAX_QUEUES 8

static char *buf_thresh_literals [] = {
#if defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_4908) || defined(CHIP_63158)
          "SwitchTxQHiReserveThreshold",
#else
          "SwitchTxQLowDropThreshold",
#endif
          "SwitchTxQHiHysteresisThreshold",
          "SwitchTxQHiPauseThreshold",
          "SwitchTxQHiDropThreshold",
          "SwitchTotalHysteresisThreshold",
          "SwitchTotalPauseThreshold",
          "SwitchTotalDropThreshold",
};
/* Local functions */
static int ethswctl_control_get(int unit, int type, unsigned int *val);
int ethswctl_flow_control_get(int unit, int type, unsigned int *val);
int ethswctl_flow_control_set(int unit, int port, int type, unsigned int val);
int ethswctl_acb_cfg_get(int unit, int queue, int type);
int ethswctl_control_priority_op(int unit, int port, int queue, int type, unsigned int *val, int *thrMode, int *maxStr, int get);
#define ethswctl_control_priority_get(unit, port, queue, type, val, thrMod, maxStre) \
    ethswctl_control_priority_op(unit, port, queue, val, manMode, maxStr, 1)
#define ethswctl_control_priority_set(unit, port, queue, type, val, thrMode, maxStr) \
    ethswctl_control_priority_op(unit, port, queue, val, manMode, maxStr, 0)
int ethswctl_quemon_op(int unit, int port, int queue, int type, int *val);
int ethswctl_quemap(int unit, int val, int priority);
int ethswctl_wrr_param_get(int unit);
int ethswctl_wrr_param_set(int unit, int type, unsigned int val);

/* Defines for WRR parameter type */
#define WRR_MAX_PKTS_PER_RND    0
#define WRR_CH0_WEIGHT          1
#define WRR_CH1_WEIGHT          2
#define WRR_CH2_WEIGHT          3
#define WRR_CH3_WEIGHT          4

#define ETHSWCTL_DISABLE 0
#define ETHSWCTL_ENABLE  1

#define SF2_IMP0_PORT                       8
#define SF2_WAN_IMP1_PORT                   5

// Defines for JUMBO register control
#define ETHSWCTL_JUMBO_PORT_ALL                       9   // bill
#define ETHSWCTL_JUMBO_PORT_MIPS                      8
#define ETHSWCTL_JUMBO_PORT_GPON                      7
#define ETHSWCTL_JUMBO_PORT_USB                       6
#define ETHSWCTL_JUMBO_PORT_MOCA                      5
#define ETHSWCTL_JUMBO_PORT_GPON_SERDES               4
#define ETHSWCTL_JUMBO_PORT_GMII_2                    3
#define ETHSWCTL_JUMBO_PORT_GMII_1                    2
#define ETHSWCTL_JUMBO_PORT_GPHY_1                    1
#define ETHSWCTL_JUMBO_PORT_GPHY_0                    0

#define ETHSWCTL_JUMBO_PORT_MIPS_MASK                 0x0100
#define ETHSWCTL_JUMBO_PORT_GPON_MASK                 0x0080
#define ETHSWCTL_JUMBO_PORT_USB_MASK                  0x0040
#define ETHSWCTL_JUMBO_PORT_MOCA_MASK                 0x0020
#define ETHSWCTL_JUMBO_PORT_GPON_SERDES_MASK          0x0010
#define ETHSWCTL_JUMBO_PORT_GMII_2_MASK               0x0008
#define ETHSWCTL_JUMBO_PORT_GMII_1_MASK               0x0004
#define ETHSWCTL_JUMBO_PORT_GPHY_1_MASK               0x0002
#define ETHSWCTL_JUMBO_PORT_GPHY_0_MASK               0x0001
#define ETHSWCTL_JUMBO_PORT_MASK_VAL                  0x01FF

typedef struct command cmd_t;
typedef int (cmd_func_t)(int, cmd_t *cmd, char** argv);

struct command
{
    int         nargs;
    const char  *name;
    cmd_func_t  *func;
    const char  *help;
};

cmd_t *command_lookup(const char *cmd);
void command_help(const cmd_t *);
void command_helpall(void);

#endif  /* _ETHSWCTL_H_ */

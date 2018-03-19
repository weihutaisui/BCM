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

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "os_defs.h"

#include "vlanctl_api.h"


/*
 * Macros
 */
#define VLANCTL_MAX_COMMAND_ARGS 4

#define VLANCTL_MAX_NAME_LENGTH  16

#define VLANCTL_COPY_VALUE(_to, _from) ( (_to) = (typeof(_to))(_from) )

#define VLANCTL_GET_STRING_ARG(_to, _from)                              \
    do {                                                                \
        strncpy((_to), (char *)(_from), VLANCTL_MAX_NAME_LENGTH);       \
        (_to)[VLANCTL_MAX_NAME_LENGTH-1] = '\0';                        \
    } while(0)


/*
 * Global variables
 */
char vlanCtl_pgmName[VLANCTL_MAX_NAME_LENGTH] = "INVALID";


/*
 * Local variables
 */
static char strArg[VLANCTL_MAX_COMMAND_ARGS][VLANCTL_MAX_NAME_LENGTH];


/*
 * Local functions
 */
static void usage(void)
{
    printf("\n\nVLAN Control Utility:\n\n");
    printf("::: Usage:\n\n");
    printf("vlanctl\n\n");
    printf("\t--if <if_name> Sets the target Interface of a composite vlanctl command to <if_name>.\n\n");
    printf("\t--rx Sets the direction of a composite vlanctl command to RECEIVE\n\n");
    printf("\t--tx Sets the direction of a composite vlanctl command to TRANSMIT\n\n");
    printf("\t--tags <nbr_of_tags> Sets the number of tags of a composite vlanctl command to <nbr_of_tags>\n\n");
    printf("\t--if-create <real_if_name> <if_index> Creates a new VOPI named <real_if_name>.v<if_index> and attaches it to the real device"
           "\n\t <real_if_name>. For instance, if this command were executed for the eth0 real interface and the VOPI interface index were"
           "\n\t set to 3, the resulting interface would have been named eth0.v3.\n\n");
    printf("\t--if-create-name <real_if_name> <vlan_if_name> Creates a new VOPI named <vlan_if_name> and attaches it to the real device"
           "\n\n");
    printf("\t--if-delete <vlan_if_name> Destroy the VOPI named <vlan_if_name>.\n\n");
    printf("\t--rule-append Inserts a new Tagging Rule as the last rule of the specified Tagging Rule Table. Dependencies: --if, --rx or"
           "\n\t --tx, and --tags.\n\n");
    printf("\t--rule-insert-before <rule-id> Inserts a new Tagging Rule before the Tagging Rule whose identifier matches <rule-id> in the"
           "\n\tspecified Tagging Rule Table. Dependencies: --if, --rx or --tx, and --tags.\n\n");
    printf("\t--rule-insert-after <rule-id> Inserts a new Tagging Rule after the Tagging Rule whose identifier matches <rule-id> in the"
           "\n\tspecified Tagging Rule Table. Dependencies: --if, --rx or --tx, and --tags.\n\n");
    printf("\t--rule-remove <rule-id> Removes the Tagging Rule that matches <rule-id> from the specified Tagging Rule Table. Dependencies:"
           "\n\t--if, --rx or --tx, and --tags.\n\n");
    printf("\t--rule-remove-all <real_if_name> <vlan_if_name> Removes all the Tagging Rules for the vlan device. \n\n");
    printf("\t--show-table Lists all Tagging Rules stored in the specified Tagging Rule Table. Dependencies: --if, --rx or --tx, and"
           "\n\t--tags.\n\n");
    printf("\t--default-tpid <tpid> Sets the default TPID value of a tagging rule table to <tpid>. When a table is created, its default"
           "\n\tTPID value is set to 0x8100. Dependencies: --if, --rx or --tx, and --tags.\n\n");
    printf("\t--default-pbits <pbits> Sets the default PBITS value of a tagging rule table to <pbits>. When a table is created, its "
           "\n\tdefault PBITS value is set to 0. Dependencies: --if, --rx or --tx, and --tags.\n\n");
    printf("\t--default-cfi <cfi> Sets the default CFI value of a tagging rule table to <cfi>. When a table is created, its default CFI "
           "\n\tvalue is set to 0. Dependencies: --if, --rx or --tx, and --tags.\n\n");
    printf("\t--default-vid <vid> Sets the default VID value of a tagging rule table to <vid>. When a table is created, its default VID "
           "\n\tvalue is set to 1 (as per IEEE 802.1Q). Dependencies: --if, --rx or --tx, and --tags.\n\n");
    printf("\t--cfg-dscp2pbits <dscp> <pbits> Programs the entry number <dscp> of the DSCP-TO-PBITS translation table of a Real Device to"
           "\n\tthe value specified by <pbits>. When a tagging rule table is created, the default values of the DSCP-TO-PBITS table are "
           "\n\tset by copying the lowest 3 bits of each DSCP value as the PBITS value, for each entry in the table. For instance, the "
           "\n\tfollowing entries are programmed by default: DSCP=5:PBITS=5, DSCP=15:PBITS=7, etc. The DSCP-TO-PBITS translation table "
           "\n\thas 64 entries." " Dependencies: --if.\n\n");
    printf("\t--show-dscp2pbits Lists the values programmed in the DSCP-TO-PBITS table of the specified Real Device. Dependencies: "
           "\n\t--if.\n\n");
    printf("\t--cfg-tpid <tpid0> <tpid1> <tpid2> <tpid3> Configures the TPID Table entries of a given Real Interface. The configured "
           "\n\tTPID values are used to identify VLAN Headers of packets received from and transmitted to the VOPIs created for a given"
           "\n\tReal Interface. Four values must always be specified. The default TPID values are 0x8100, 0x8100, 0x8100, and 0x8100. "
           "\n\tDependencies: --if.\n\n");
    printf("\t--show-tpid Lists the values programmed in the TPID Table of the specified Real Device. Dependencies: --if.\n\n");
    printf("\t--local-stats <vlan_if_name> Shows the statistics counters maintained for the VOPI named <vlan_if_name>. These counters "
           "\n\tare complimentary to the standard counters maintained for the device, which can be read via the Linux ifconfig "
           "\n\tcommand.\n\n");
    printf("\t--filter-ethertype <ethertype> Match the Ethertype field in the Ethernet Header of incoming frames against <ethertype>.\n\n");
    printf("\t--filter-pbits <pbits> <tag_nbr> Match the PBITS value of VLAN Header number <tag_nbr> of incoming frames against "
           "\n\t<pbits>.\n\n");
    printf("\t--filter-cfi <cfi> <tag_nbr> Match the CFI bit of VLAN Header number <tag_nbr> of incoming frames against <cfi>.\n\n");
    printf("\t--filter-vid <vid> <tag_nbr> Match the VID value of VLAN Header number <tag_nbr> of incoming frames against <vid>.\n\n");
    printf("\t--filter-tag-ethertype <ethertype> <tag_nbr> Match the Ethertype field of the VLAN Header number <tag_nbr> of incoming "
           "\n\tframes against <ethertype>.\n\n");
    printf("\t--filter-dscp <dscp> Match the DSCP value in the IPv4 header of incoming frames against <dscp>.\n");
    printf("\t--filter-rxif <real_if_name> Match the rx VOPI of the transmitting packet against <real-if-name>. This filter can be used to bind a Tagging "
           "\n\tRule to a specific rx VOPI on the TRANSMIT direction. This filter is not applicable for rules in the RECEIVE direction.\n\n");
    printf("\t--filter-txif <vlan_if_name> Match the transmitting VOPI against <vlan-if-name>. This filter can be used to bind a Tagging "
           "\n\tRule to a specific VOPI on the TRANSMIT direction. This filter is not applicable for rules in the RECEIVE direction. "
           "\n\tTRANSMIT rules without this filter will apply to all frames transmitted from all VOPIs attached to the Real Device.\n\n");
    printf("\t--filter-skb-prio <priority> Match the SKB priority of incoming frames against <priority>.\n\n");
    printf("\t--filter-skb-mark-flowid <flowid> Match the Flow ID subfield of the SKB Mark field against <flowid>. The SKB Mark Flow ID "
           "\n\tsubfield can be used as a way to correlate packet classification made by other Linux modules (such as ebtables and "
           "\n\tiptables) with Tagging Rules. A possible usage for this filter would be to direct packets generated by an application to "
           "\n\ta specific port of a real interface (such as a GPON port) based on layer 3 filters. In this example a socket would be "
           "\n\tcreated on a VOPI," "IP Tables rules would be created to identify flows and set Flow IDs, and Tagging rules would be "
           "\n\tcreated to match on such " "Flow IDs and apply treatments, such as setting the destination GEM Port and Queue.\n\n");
    printf("\t--filter-skb-mark-port <port> Match the Port subfield of the SKB Mark field against <port>. This filter can be used to bind"
           "\n\t certain Tagging Rules with a specific Real Interface port (for instance a GPON Port).\n\n");
    printf("\t--filter-vlan-dev-mac-addr <ignore_if_multicast> Match the recv frame dest MAC addr against the recv virtual interface."
           "\n\t Set <ignore_if_multicast> to 0 to apply filter on all recv frames."
           "\n\t Set <ignore_if_multicast> to 1 to apply filter on unicast frames only."
           "\n\t This filter is not applicable for rules in the TRANSMIT direction.\n\n");
    printf("\t--pop-tag Remove the outermost VLAN tag. If multiple tags are to be removed, this treatment should be specified for each "
           "\n\tVLAN tag to be removed.\n\n");
    printf("\t--push-tag Add the default VLAN tag of the corresponding Tagging Rule Table as the new outer tag. The default TPID value"
           "\n\twill be used as the new Ethertype value in the Ethernet header, the existing Ethertype of the Ethernet Header will be used"
           "\n\tas the Tag Ethertype field of the new tag, and the default PBITS, CFI and VID will be used as the TCI of the new tag. If"
           "\n\tmultiple tags are to be inserted, this treatment must be specified for each VLAN tag to be inserted.\n\n");
#if defined(CONFIG_BCM_VLAN_AGGREGATION)
    printf("\t--deaggr-tag Accord to bridge MAC+VID table to modify the outermost VLAN tag.\n\n");
#endif
    printf("\t--set-ethertype <ethertype> Set the Ethertype value of the Ethernet Header to <ethertype>.\n\n");
    printf("\t--set-pbits <pbits> <tag_nbr> Set the PBITS value of the VLAN Header number <tag_nbr> to <pbits>.\n\n");
    printf("\t--set-cfi <cfi> <tag_nbr> Set the CFI bit of the VLAN Heade number <tag_nbr> to <cfi>.\n\n");
    printf("\t--set-vid <vid> <tag_nbr> Set the VID of the VLAN Header number <tag_nbr> to <vid>.\n\n");
    printf("\t--set-tag-ethertype <ethertype> Set the Ethertype field of the VLAN Header number <tag_nbr> to <ethertype>.\n\n");
    printf("\t--set-dscp <dscp> Set the IPv4 DSCP value of the matching frame to <dscp>.\n\n");
    printf("\t--copy-pbits <from_tag_nbr> <to_tag_nbr> Copy the PBITS value from VLAN Header number <from_tag_nbr> to VLAN Header number "
           "\n\t<to_tag_nbr>.\n\n");
    printf("\t--copy-cfi <from_tag_nbr> <to_tag_nbr> Copy the CFI value from VLAN Header number <from_tag_nbr> to VLAN Header number "
           "\n\t<to_tag_nbr>.\n\n");
    printf("\t--copy-vid <from_tag_nbr> <to_tag_nbr> Copy the VID value from VLAN Header number <from_tag_nbr> to VLAN Header number "
           "\n\t<to_tag_nbr>.\n\n");
    printf("\t--copy-tag-ethertype <from_tag_nbr> <to_tag_nbr> Copy the Ethertype value from VLAN Header number <from_tag_nbr> to VLAN "
           "\n\tHeader number <to_tag_nbr>.\n\n");
    printf("\t--dscp2pbits <tag_nbr> Translate the IPv4 DSCP into a PIBTS value, and write the translated PBITS value in the VLAN Header "
           "\n\tnumber <tag_nbr>. The DSCP-To-PBITS table of the respective Real Device is used for translation.\n\n");
    printf("\t--set-rxif <vlan_if_name> Forward frames in the RECEIVE direction that match this rule to the VOPI specified in "
           "\n\t<vlan_if_name>. If not specified, the frame will be forwarded to a randomly chosen VOPI. Using this treatment in the "
           "\n\tTRANSMIT direction has no effect.\n\n");
    printf("\t--set-if-mode-ont Set real device mode to ONT.\n\n");
    printf("\t--set-if-mode-rg  Set real device mode to RG.\n\n");
    printf("\t--drop-frame Drop the matching frame.\n");
    printf("\t--set-skb-prio <priority> Set the SKB priority to <priority>.\n\n");
    printf("\t--set-skb-mark-port <port> Set the Port subfield of the SKB Mark field to <port>. The SKB Mark Port subfield is used by "
           "\n\tthe Broadcom device drivers to send a frame to a specific port within a Real Interface. For instance, a GPON Real "
           "\n\tInterface may have been configured with multiple GEM Ports. When a packet is sent to that interface, the driver uses "
           "\n\tthe SKB Mark Port subfield as the GEM Port to which the packets will be transmitted.\n\n");
    printf("\t--set-skb-mark-queue <queue> Set the Queue subfield of the SKB Mark field to <queue>. The SKB Mark Queue subfield is used "
           "\n\tby the Broadcom device drivers to determine the queue to which transmit a frame.\n\n");
    printf("\t--set-skb-mark-queue-by-pbits Set the Queue subfield of the SKB Mark field based on the packet pbits value.\n\n");
    printf("\t--set-skb-mark-flowid <flowid> Set the Flow ID subfield of the SKB Mark field to <flowID>. The SKB Mark Flow ID subfield "
           "\n\tcan be used as a way to correlate packet classification made by Tagging Rules with other Linux modules (such as "
           "\n\tebtables and iptables).\n\n");
    printf("\t--rule-type <type> set the type of rule. 0: flow; 1: qos; \n\n");
    printf("\t--create-flows <rx_vlan_ifname> <tx_vlan_ifname> Setup vlan flows for the path (rx_vlan_ifname->tx_vlan_ifname).\n\n");
    printf("\t--delete-flows <rx_vlan_ifname> <tx_vlan_ifname> Remove vlan flows for the path (rx_vlan_ifname->tx_vlan_ifname).\n\n");
    printf("\t--drop-precedence <0:US|1:DS> <dp_code> Configure drop precedence code.\n");
    printf("\tdp_code value can be none(0)|internal(1)|dei(2)|8p0d(3)|7p1d(4)|6p2d(5)|5p3d(6)|dscp(7).\n\n");
}

static void dbgPrintCmd(uintptr_t *arg)
{
    VLANCTL_GET_STRING_ARG(strArg[0], arg[0]);
    VLANCTL_GET_STRING_ARG(strArg[1], arg[3]);

    printf("CLI Debug: %s %d %d %s\n",
           strArg[0], (unsigned int)(arg[1]),
           (unsigned int)(arg[2]), strArg[1]);
}


#define VLANCTL_IS_DEC(_x) ( ((_x) == '-') || ((_x) >= '0' && (_x) <= '9') )
#define VLANCTL_IS_HEX(_x) ( ((_x) >= 'a' && (_x) <= 'f') ||    \
                             ((_x) >= 'A' && (_x) <= 'F') )

static int isNumericArg(char *argv)
{
    int ret = 1;
    char *s = argv;
    int len;
    int i;

    if(strstr(s, "0x") == s)
    {
        /* hex string */
        s += 2;
        len = strlen(s);
        for(i=0; i<len; ++i)
        {
            if(VLANCTL_IS_HEX(s[i]) || VLANCTL_IS_DEC(s[i]))
            {
                continue;
            }

            ret = 0;
        }
    }
    else
    {
        /* decimal string */
        len = strlen(s);
        for(i=0; i<len; ++i)
        {
            if(VLANCTL_IS_DEC(s[i]))
            {
                continue;
            }

            ret = 0;
        }
    }

    return ret;
}

static int getNumericArg(int *argc, char ***argv, uintptr_t *arg)
{
    if(--(*argc))
    {
        (*argv)++;

        if(!isNumericArg(**argv))
        {
//            VLANCTL_LOG_ERROR("Not a numeric argument");
            return -1;
        }

        errno = 0;
        *arg = strtol(**argv, NULL, 0);
        if(errno)
        {
            VLANCTL_LOG_ERROR("%s: %" PRIuPTR, strerror(errno), *arg);
            return -1;
        }

        return 0;
    }

    VLANCTL_LOG_ERROR("Missing argument");
    return -1;
}

static int getStringArg(int *argc, char ***argv, uintptr_t *arg)
{
    if(--(*argc))
    {
        (*argv)++;

        *arg = (uintptr_t)**argv;

        return 0;
    }

    VLANCTL_LOG_ERROR("Missing argument");
    return -1;
}

static int getCmdArgs(int *argc, char ***argv, uintptr_t *arg, const char *cmdMask)
{
    int ret = 0;
    int done = 0;
    int i;
    char *cmdName = **argv;

    for(i=0; !ret && !done && i<VLANCTL_MAX_COMMAND_ARGS; ++i)
    {
        switch(cmdMask[i])
        {
            case 'd':
                ret = getNumericArg(argc, argv, &arg[i]);
                break;

            case 's':
                ret = getStringArg(argc, argv, &arg[i]);
                break;

            case '\0':
                done = 1;
                break;

            default:
                VLANCTL_LOG_ERROR("Internal Error: cmdMask[%d] = %c", i, cmdMask[i]);
                ret = -1;
        }
    }

    if(ret)
    {
        VLANCTL_LOG_ERROR("Command '%s' has Invalid or Missing Arguments", cmdName);
    }

    return ret;
}


/*
 * Global Funtions
 */
int main(int argc, char **argv)
{
    int ret;
    int commandCount = 0;
    uintptr_t arg[VLANCTL_MAX_COMMAND_ARGS];

    char devName[VLANCTL_MAX_NAME_LENGTH] = "INVALID";
    vlanCtl_direction_t tableDir = VLANCTL_DONT_CARE;
    unsigned int nbrOfTags = VLANCTL_DONT_CARE;
    unsigned int tagRuleId = VLANCTL_DONT_CARE;
    int isRouted = 0;
    int isMulticast = 0;

    VLANCTL_GET_STRING_ARG(vlanCtl_pgmName, *argv);

    VLANCTL_LOG_DEBUG("Initializing %s...", vlanCtl_pgmName);

    VLANCTL_LOG_DEBUG("argc = %d", argc);

    ret = vlanCtl_init();

    vlanCtl_initTagRule();

    /* parse commands */
    argc--, argv++;

    while(!ret && argc)
    {
        if(!strcmp(*argv, "--help") || !strcmp(*argv, "--h"))
        {
            commandCount++;
            usage();
        }
        else if(!strcmp(*argv, "--debug-cli"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "sdds");
            if(!ret)
            {
                dbgPrintCmd(arg);
            }
        }
        else if(!strcmp(*argv, "--if"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "s");
            if(!ret)
            {
                strncpy(devName, (char *)arg[0], VLANCTL_MAX_NAME_LENGTH);
                /* NULL terminate name, just in case */
                devName[VLANCTL_MAX_NAME_LENGTH-1] = '\0';
            }
        }
        else if(!strcmp(*argv, "--rx"))
        {
            commandCount++;
            tableDir = VLANCTL_DIRECTION_RX;
        }
        else if(!strcmp(*argv, "--tx"))
        {
            commandCount++;
            tableDir = VLANCTL_DIRECTION_TX;
        }
        else if(!strcmp(*argv, "--routed"))
        {
            commandCount++;
            isRouted = 1;
        }
        else if(!strcmp(*argv, "--mcast"))
        {
            commandCount++;
            isMulticast = 1;
        }
        else if(!strcmp(*argv, "--tags"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "d");
            if(!ret)
            {
                VLANCTL_COPY_VALUE(nbrOfTags, arg[0]);
            }
        }
        else if(!strcmp(*argv, "--if-create"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "sd");
            if(!ret)
            {
                ret = vlanCtl_createVlanInterface((char *)arg[0], arg[1], isRouted, isMulticast);
            }
        }
        else if(!strcmp(*argv, "--if-create-name"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "ss");
            if(!ret)
            {
                ret = vlanCtl_createVlanInterfaceByName((char *)arg[0], (char *)arg[1], isRouted, isMulticast);
            }
        }
        else if(!strcmp(*argv, "--if-delete"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "s");
            if(!ret)
            {
                ret = vlanCtl_deleteVlanInterface((char *)arg[0]);
            }
        }
        else if(!strcmp(*argv, "--filter-skb-prio"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "d");
            if(!ret)
            {
                vlanCtl_filterOnSkbPriority(arg[0]);
            }
        }
        else if(!strcmp(*argv, "--filter-skb-mark-flowid"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "d");
            if(!ret)
            {
                vlanCtl_filterOnSkbMarkFlowId(arg[0]);
            }
        }
        else if(!strcmp(*argv, "--filter-skb-mark-port"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "d");
            if(!ret)
            {
                vlanCtl_filterOnSkbMarkPort(arg[0]);
            }
        }
        else if(!strcmp(*argv, "--filter-ethertype"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "d");
            if(!ret)
            {
                vlanCtl_filterOnEthertype(arg[0]);
            }
        }
        else if(!strcmp(*argv, "--filter-ipproto"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "d");
            if(!ret)
            {
                vlanCtl_filterOnIpProto(arg[0]);
            }
        }
        else if(!strcmp(*argv, "--filter-dscp"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "d");
            if(!ret)
            {
                vlanCtl_filterOnDscp(arg[0]);
            }
        }
        else if(!strcmp(*argv, "--filter-dscp2pbits"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "d");
            if(!ret)
            {
                vlanCtl_filterOnDscp2Pbits(arg[0]);
            }
        }
        else if(!strcmp(*argv, "--filter-pbits"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "dd");
            if(!ret)
            {
                ret = vlanCtl_filterOnTagPbits(arg[0], arg[1]);
            }
        }
        else if(!strcmp(*argv, "--filter-cfi"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "dd");
            if(!ret)
            {
                ret = vlanCtl_filterOnTagCfi(arg[0], arg[1]);
            }
        }
        else if(!strcmp(*argv, "--filter-vid"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "dd");
            if(!ret)
            {
                ret = vlanCtl_filterOnTagVid(arg[0], arg[1]);
            }
        }
        else if(!strcmp(*argv, "--filter-tag-ethertype"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "dd");
            if(!ret)
            {
                ret = vlanCtl_filterOnTagEtherType(arg[0], arg[1]);
            }
        }
        else if(!strcmp(*argv, "--filter-rxif"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "s");
            if(!ret)
            {
                ret = vlanCtl_filterOnRxRealDevice((char *)arg[0]);
            }
        }
        else if(!strcmp(*argv, "--filter-txif"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "s");
            if(!ret)
            {
                ret = vlanCtl_filterOnTxVlanDevice((char *)arg[0]);
            }
        }
        else if(!strcmp(*argv, "--filter-vlan-dev-mac-addr"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "d");
            if(!ret)
            {
               vlanCtl_filterOnVlanDeviceMacAddr(arg[0]);
            }
        }
        else if(!strcmp(*argv, "--filter-unicast"))
        {
            commandCount++;
            ret = vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_IS_UNICAST);
        }
        else if(!strcmp(*argv, "--filter-multicast"))
        {
            commandCount++;
            ret = vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_IS_MULTICAST);
        }
        else if(!strcmp(*argv, "--filter-broadcast"))
        {
            commandCount++;
            ret = vlanCtl_filterOnFlags(BCM_VLAN_FILTER_FLAGS_IS_BROADCAST);
        }
        else if(!strcmp(*argv, "--pop-tag"))
        {
            commandCount++;
            ret = vlanCtl_cmdPopVlanTag();
        }
        else if(!strcmp(*argv, "--push-tag"))
        {
            commandCount++;
            ret = vlanCtl_cmdPushVlanTag();
        }
#if defined(CONFIG_BCM_VLAN_AGGREGATION)
        else if(!strcmp(*argv, "--deaggr-tag"))
        {
            commandCount++;
            ret = vlanCtl_cmdDeaggrVlanTag();
        }
#endif
        else if(!strcmp(*argv, "--set-ethertype"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "d");
            if(!ret)
            {
                ret = vlanCtl_cmdSetEtherType(arg[0]);
            }
        }
        else if(!strcmp(*argv, "--set-pbits"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "dd");
            if(!ret)
            {
                ret = vlanCtl_cmdSetTagPbits(arg[0], arg[1]);
            }
        }
        else if(!strcmp(*argv, "--set-cfi"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "dd");
            if(!ret)
            {
                ret = vlanCtl_cmdSetTagCfi(arg[0], arg[1]);
            }
        }
        else if(!strcmp(*argv, "--set-vid"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "dd");
            if(!ret)
            {
                ret = vlanCtl_cmdSetTagVid(arg[0], arg[1]);
            }
        }
        else if(!strcmp(*argv, "--rule-type"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "d");
            if(!ret)
            {
                vlanCtl_setVlanRuleTableType(arg[0]);
            }
        }
        else if(!strcmp(*argv, "--set-tag-ethertype"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "dd");
            if(!ret)
            {
                ret = vlanCtl_cmdSetTagEtherType(arg[0], arg[1]);
            }
        }
        else if(!strcmp(*argv, "--copy-pbits"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "dd");
            if(!ret)
            {
                ret = vlanCtl_cmdCopyTagPbits(arg[0], arg[1]);
            }
        }
        else if(!strcmp(*argv, "--copy-cfi"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "dd");
            if(!ret)
            {
                ret = vlanCtl_cmdCopyTagCfi(arg[0], arg[1]);
            }
        }
        else if(!strcmp(*argv, "--copy-vid"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "dd");
            if(!ret)
            {
                ret = vlanCtl_cmdCopyTagVid(arg[0], arg[1]);
            }
        }
        else if(!strcmp(*argv, "--copy-tag-ethertype"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "dd");
            if(!ret)
            {
                ret = vlanCtl_cmdCopyTagEtherType(arg[0], arg[1]);
            }
        }
        else if(!strcmp(*argv, "--dscp2pbits"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "d");
            if(!ret)
            {
                ret = vlanCtl_cmdDscpToPbits(arg[0]);
            }
        }
        else if(!strcmp(*argv, "--set-dscp"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "d");
            if(!ret)
            {
                ret = vlanCtl_cmdSetDscp(arg[0]);
            }
        }
        else if(!strcmp(*argv, "--drop-frame"))
        {
            commandCount++;
            ret = vlanCtl_cmdDropFrame();
        }
        else if(!strcmp(*argv, "--set-skb-prio"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "d");
            if(!ret)
            {
                ret = vlanCtl_cmdSetSkbPriority(arg[0]);
            }
        }
        else if(!strcmp(*argv, "--set-skb-mark-port"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "d");
            if(!ret)
            {
                ret = vlanCtl_cmdSetSkbMarkPort(arg[0]);
            }
        }
        else if(!strcmp(*argv, "--set-skb-mark-queue"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "d");
            if(!ret)
            {
                ret = vlanCtl_cmdSetSkbMarkQueue(arg[0]);
            }
        }
        else if(!strcmp(*argv, "--set-skb-mark-queue-by-pbits"))
        {
            commandCount++;
            ret = vlanCtl_cmdSetSkbMarkQueueByPbits();
        }
        else if(!strcmp(*argv, "--set-skb-mark-flowid"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "d");
            if(!ret)
            {
                ret = vlanCtl_cmdSetSkbMarkFlowId(arg[0]);
            }
        }
        else if(!strcmp(*argv, "--ovrd-learn-vid"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "d");
            if(!ret)
            {
                ret = vlanCtl_cmdOvrdLearningVid(arg[0]);
            }
        }
        else if(!strcmp(*argv, "--continue"))
        {
            commandCount++;
            ret = vlanCtl_cmdContinue();
        }
        else if(!strcmp(*argv, "--set-rxif"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "s");
            if(!ret)
            {
                ret = vlanCtl_setReceiveVlanDevice((char *)arg[0]);
            }
        }
        else if(!strcmp(*argv, "--set-if-mode-ont"))
        {
            commandCount++;
            ret = vlanCtl_setRealDevMode(devName, BCM_VLAN_MODE_ONT);
        }
        else if(!strcmp(*argv, "--set-if-mode-rg"))
        {
            commandCount++;
            ret = vlanCtl_setRealDevMode(devName, BCM_VLAN_MODE_RG);
        }
        else if(!strcmp(*argv, "--rule-append"))
        {
            commandCount++;
            ret = vlanCtl_insertTagRule(devName, tableDir, nbrOfTags,
                                        VLANCTL_POSITION_APPEND, VLANCTL_DONT_CARE, &tagRuleId);
        }
        else if(!strcmp(*argv, "--rule-insert-last"))
        {
            commandCount++;
            ret = vlanCtl_insertTagRule(devName, tableDir, nbrOfTags,
                                        VLANCTL_POSITION_LAST, VLANCTL_DONT_CARE, &tagRuleId);
        }
        else if(!strcmp(*argv, "--rule-insert-before"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "d");
            if(!ret)
            {
                ret = vlanCtl_insertTagRule(devName, tableDir, nbrOfTags,
                                            VLANCTL_POSITION_BEFORE, arg[0], &tagRuleId);
            }
        }
        else if(!strcmp(*argv, "--rule-insert-after"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "d");
            if(!ret)
            {
                ret = vlanCtl_insertTagRule(devName, tableDir, nbrOfTags,
                                            VLANCTL_POSITION_AFTER, arg[0], &tagRuleId);
            }
        }
        else if(!strcmp(*argv, "--rule-remove"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "d");
            if(!ret)
            {
                ret = vlanCtl_removeTagRule(devName, tableDir, nbrOfTags, arg[0]);
            }
        }
        else if(!strcmp(*argv, "--rule-remove-all"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "s");
            if(!ret)
            {
                ret = vlanCtl_removeAllTagRule((char *)arg[0]);
            }
        }
        else if(!strcmp(*argv, "--rule-remove-by-filter"))
        {
            commandCount++;
            ret = vlanCtl_removeTagRuleByFilter(devName, tableDir, nbrOfTags);
        }
        else if(!strcmp(*argv, "--show-table"))
        {
            commandCount++;
            ret = vlanCtl_dumpRuleTable(devName, tableDir, nbrOfTags);
        }
        else if(!strcmp(*argv, "--default-tpid"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "d");
            if(!ret)
            {
                ret = vlanCtl_setDefaultVlanTag(devName, tableDir, nbrOfTags,
                                                arg[0], VLANCTL_DONT_CARE, VLANCTL_DONT_CARE, VLANCTL_DONT_CARE);
            }
        }
        else if(!strcmp(*argv, "--default-pbits"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "d");
            if(!ret)
            {
                ret = vlanCtl_setDefaultVlanTag(devName, tableDir, nbrOfTags,
                                                VLANCTL_DONT_CARE, arg[0], VLANCTL_DONT_CARE, VLANCTL_DONT_CARE);
            }
        }
        else if(!strcmp(*argv, "--default-cfi"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "d");
            if(!ret)
            {
                ret = vlanCtl_setDefaultVlanTag(devName, tableDir, nbrOfTags,
                                                VLANCTL_DONT_CARE, VLANCTL_DONT_CARE, arg[0], VLANCTL_DONT_CARE);
            }
        }
        else if(!strcmp(*argv, "--default-vid"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "d");
            if(!ret)
            {
                ret = vlanCtl_setDefaultVlanTag(devName, tableDir, nbrOfTags,
                                                VLANCTL_DONT_CARE, VLANCTL_DONT_CARE, VLANCTL_DONT_CARE, arg[0]);
            }
        }
        else if(!strcmp(*argv, "--default-miss-accept"))
        {
            commandCount++;
            if(tableDir == VLANCTL_DIRECTION_RX)
            {
                ret = getCmdArgs(&argc, &argv, arg, "s");
                if(!ret)
                {
                    ret = vlanCtl_setDefaultAction(devName, VLANCTL_DIRECTION_RX, nbrOfTags,
                                                   VLANCTL_ACTION_ACCEPT, (char *)arg[0]);
                }
            }
            else /* TX */
            {
                ret = vlanCtl_setDefaultAction(devName, VLANCTL_DIRECTION_TX, nbrOfTags,
                                               VLANCTL_ACTION_ACCEPT, NULL);
            }
        }
        else if(!strcmp(*argv, "--default-miss-drop"))
        {
            commandCount++;
            ret = vlanCtl_setDefaultAction(devName, tableDir, nbrOfTags,
                                           VLANCTL_ACTION_DROP, NULL);
        }
        else if(!strcmp(*argv, "--cfg-dscp2pbits"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "dd");
            if(!ret)
            {
                ret = vlanCtl_setDscpToPbits(devName, arg[0], arg[1]);
            }
        }
        else if(!strcmp(*argv, "--show-dscp2pbits"))
        {
            commandCount++;
            ret = vlanCtl_dumpDscpToPbits(devName, VLANCTL_DONT_CARE);
        }
        else if(!strcmp(*argv, "--cfg-tpid"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "dddd");
            if(!ret)
            {
                ret = vlanCtl_setTpidTable(devName, (unsigned int *)arg);
            }
        }
        else if(!strcmp(*argv, "--show-tpid"))
        {
            commandCount++;
            ret = vlanCtl_dumpTpidTable(devName);
        }
        else if(!strcmp(*argv, "--local-stats"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "s");
            if(!ret)
            {
                ret = vlanCtl_dumpLocalStats((char *)arg[0]);
            }
        }
        else if(!strcmp(*argv, "--if-suffix"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "s");
            if(!ret)
            {
                ret = vlanCtl_setIfSuffix((char *)arg[0]);
            }
        }
        else if(!strcmp(*argv, "--run-test"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "dss");
            if(!ret)
            {
                ret = vlanCtl_runTest(arg[0], (char *)arg[1], (char *)arg[2]);
            }
        }
        else if(!strcmp(*argv, "--create-flows"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "ss");
            if(!ret)
            {
                ret = vlanCtl_createVlanFlows((char *)arg[0], (char *)arg[1]);
            }
        }
        else if(!strcmp(*argv, "--delete-flows"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "ss");
            if(!ret)
            {
                ret = vlanCtl_deleteVlanFlows((char *)arg[0], (char *)arg[1]);
            }
        }
        else if(!strcmp(*argv, "--drop-precedence"))
        {
            commandCount++;
            ret = getCmdArgs(&argc, &argv, arg, "dd");
            if(!ret)
            {
                ret = vlanCtl_setDropPrecedence(arg[0], arg[1]);
            }
        }
        else
        {
            VLANCTL_LOG_ERROR("Invalid Command or Excessive Arguments: '%s'", *argv);
            ret = -1;
        }

        if(argc)
        {
            argc--, argv++;
        }
    }

    if(!ret && !commandCount)
    {
        usage();
        ret = -1;
    }

    VLANCTL_LOG_DEBUG("Exiting with code %d", ret);

    vlanCtl_cleanup();

    return ret;
}


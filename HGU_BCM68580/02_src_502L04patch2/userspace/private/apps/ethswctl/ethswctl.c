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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <asm/byteorder.h>
#include <sys/errno.h>
#include <getopt.h>

#include "bcm/bcmswapitypes.h"
#include "ethswctl.h"
#include "ethswctl_api.h"
#include "bcm/bcmswapistat.h"
#include "boardparms.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int handle_new_format_commands(int argc, char *argv[]);
static int getifname_handler(int argc, cmd_t *cmd, char** argv);
static int debug_handler(int argc, cmd_t *cmd, char** argv);

void outputJumboStatus_MIPS(unsigned int regVal)
{
  // Test if port accepts or rejects jumbo frames.
  if ((regVal & ETHSWCTL_JUMBO_PORT_MIPS_MASK) != 0) {
    printf("  MIPS port accepts jumbo frames.\n");
  }
  else {
    printf("  MIPS port rejects jumbo frames.\n");
  }
}


void outputJumboStatus_GPON(unsigned int regVal)
{
  // Test if port accepts or rejects jumbo frames.
  if ((regVal & ETHSWCTL_JUMBO_PORT_GPON_MASK) != 0) {
    printf("  GPON port accepts jumbo frames.\n");
  }
  else {
    printf("  GPON port rejects jumbo frames.\n");
  }
}


void outputJumboStatus_USB(unsigned int regVal)
{
  // Test if port accepts or rejects jumbo frames.
  if ((regVal & ETHSWCTL_JUMBO_PORT_USB_MASK) != 0) {
    printf("  USB port accepts jumbo frames.\n");
  }
  else {
    printf("  USB port rejects jumbo frames.\n");
  }
}


void outputJumboStatus_MOCA(unsigned int regVal)
{
  // Test if port accepts or rejects jumbo frames.
  if ((regVal & ETHSWCTL_JUMBO_PORT_MOCA_MASK) != 0) {
    printf("  MOCA port accepts jumbo frames.\n");
  }
  else {
    printf("  MOCA port rejects jumbo frames.\n");
  }
}


void outputJumboStatus_GPON_SERDES(unsigned int regVal)
{
  // Test if port accepts or rejects jumbo frames.
  if ((regVal & ETHSWCTL_JUMBO_PORT_GPON_SERDES_MASK) != 0) {
    printf("  GPON_SERDES port accepts jumbo frames.\n");
  }
  else {
    printf("  GPON_SERDES port rejects jumbo frames.\n");
  }
}


void outputJumboStatus_GMII_2(unsigned int regVal)
{
  // Test if port accepts or rejects jumbo frames.
  if ((regVal & ETHSWCTL_JUMBO_PORT_GMII_2_MASK) != 0) {
    printf("  GMII_2 port accepts jumbo frames.\n");
  }
  else {
    printf("  GMII_2 port rejects jumbo frames.\n");
  }
}


void outputJumboStatus_GMII_1(unsigned int regVal)
{
  // Test if port accepts or rejects jumbo frames.
  if ((regVal & ETHSWCTL_JUMBO_PORT_GMII_1_MASK) != 0) {
    printf("  GMII_1 port accepts jumbo frames.\n");
  }
  else {
    printf("  GMII_1 port rejects jumbo frames.\n");
  }
}


void outputJumboStatus_GPHY_1(unsigned int regVal)
{
  // Test if port accepts or rejects jumbo frames.
  if ((regVal & ETHSWCTL_JUMBO_PORT_GPHY_1_MASK) != 0) {
    printf("  GPHY_1 port accepts jumbo frames.\n");
  }
  else {
    printf("  GPHY_1 port rejects jumbo frames.\n");
  }
}


void outputJumboStatus_GPHY_0(unsigned int regVal)
{
  // Test if port accepts or rejects jumbo frames.
  if ((regVal & ETHSWCTL_JUMBO_PORT_GPHY_0_MASK) != 0) {
    printf("  GPHY_0 port accepts jumbo frames.\n");
  }
  else {
    printf("  GPHY_0 port rejects jumbo frames.\n");
  }
}


void outputJumboStatus(int portNum, unsigned int regVal)  // bill
{
    // Switch on specified port.
    switch (portNum & ETHSWCTL_JUMBO_PORT_MASK_VAL) {
        case ETHSWCTL_JUMBO_PORT_ALL:
            printf("JUMBO_PORT_MASK:0x%08X\n", regVal);
            outputJumboStatus_GPHY_0(regVal);
            outputJumboStatus_GPHY_1(regVal);
            outputJumboStatus_GMII_1(regVal);
            outputJumboStatus_GMII_2(regVal);
            outputJumboStatus_GPON_SERDES(regVal);
            outputJumboStatus_MOCA(regVal);
            outputJumboStatus_USB(regVal);
            outputJumboStatus_GPON(regVal);
            outputJumboStatus_MIPS(regVal);
            break;
        case ETHSWCTL_JUMBO_PORT_MIPS:
            outputJumboStatus_MIPS(regVal);
            break;
        case ETHSWCTL_JUMBO_PORT_GPON:
            outputJumboStatus_GPON(regVal);
            break;
        case ETHSWCTL_JUMBO_PORT_USB:
            outputJumboStatus_USB(regVal);
            break;
        case ETHSWCTL_JUMBO_PORT_MOCA:
            outputJumboStatus_MOCA(regVal);
            break;
        case ETHSWCTL_JUMBO_PORT_GPON_SERDES:
            outputJumboStatus_GPON_SERDES(regVal);
            break;
        case ETHSWCTL_JUMBO_PORT_GMII_2:
            outputJumboStatus_GMII_2(regVal);
            break;
        case ETHSWCTL_JUMBO_PORT_GMII_1:
            outputJumboStatus_GMII_1(regVal);
            break;
        case ETHSWCTL_JUMBO_PORT_GPHY_1:
            outputJumboStatus_GPHY_1(regVal);
            break;
        case ETHSWCTL_JUMBO_PORT_GPHY_0:
            outputJumboStatus_GPHY_0(regVal);
            break;
    }
}

static const struct command commands[] = {
    { 1, "getifname", getifname_handler,
        ": Get the interface name for the given port\n"
            "  ethswctl getifname <unit [0...1]> <port [0...7]>\n"
            "  NOTE : if only one argument; unit is assumed 0"
    },
    { 0, "debug", debug_handler,
        ": Get/Set Debug level\n"
            "  ethswctl debug <level>"
    },
};

cmd_t *command_lookup(const char *cmd)
{
    int i;

    for (i = 0; i < sizeof(commands)/sizeof(commands[0]); i++) {
        if (!strcmp(cmd, commands[i].name))
            return (cmd_t *)&commands[i];
    }

    return NULL;
}

void command_help(const cmd_t *cmd)
{
    fprintf(stderr, "  %s %s\n\n", cmd->name, cmd->help);
}

void command_helpall(void)
{
    int i;

    fprintf(stderr, "Usage: ethswctl <command> [arguments...]\n\n");
    for (i = 0; i < sizeof(commands)/sizeof(commands[0]); i++)
        command_help(commands+i);
}

#define IFNAMESIZ 16
static int getifname_handler(int argc, cmd_t *cmd, char** argv)
{
    char ifname[IFNAMESIZ];
    int ret_val = 0, unit, port;
    char*  argv_port;
    char*  argv_unit =  NULL;

    if (*argv) {
        if (argc >= 2+2)  /* SYNTAX : ethswctl getifname unit port */
        {
            argv_unit = argv[2]; 
            argv_port = argv[3]; 
        }
        else /* backward compatibility */
             /* SYNTAX : ethswctl getifname port */
        {
            argv_port = argv[2]; 
        }

        port = strtol(argv_port, NULL, 0);
        unit = (argv_unit == NULL) ? 0 : strtol(argv_unit, NULL, 0);
        if ((port < 0) || (port >= 8) || (unit < 0) || (unit > 1) ) {
            command_help(cmd);
            return -1;
        }
        ret_val = bcm_ifname_get(unit, port, ifname);
        if (ret_val == 0) {
            printf("The interface name for unit %d port %d is %s\n", unit, port, ifname);
        }
    } else {
        fprintf(stderr, "%s: argv is NULL for some reason\n", __FUNCTION__);
    }
    return ret_val;
}

static int debug_handler(int argc, cmd_t *cmd, char** argv)
{
    int level, ret_val = 0;

    argv = argv+2;;
    if (*argv) {
        level = strtol(*argv, NULL, 0);
        ret_val = bcm_enet_debug_set(0, level);
        if (ret_val == 0) {
            printf("The Enet Driver Log Level is set successfully\n");
        }
    } else {
        ret_val = bcm_enet_debug_get(0, &level);
        if (ret_val == 0) {
            printf("The Enet Driver Log Level is %d\n", level);
        } else {
            printf("The UNI 2 UNI status get failed\n");
        }
    }
    return ret_val;
}

int handle_new_format_commands(int argc, char *argv[])
{
    int ret_val = 0;
    cmd_t *cmd;

    cmd = command_lookup(argv[1]);
    if (cmd == NULL) {
        fprintf(stderr, "invalid command [%s]\n", argv[1]);
        command_helpall();
        return -1;
    }

    if (argc < cmd->nargs + 2) {
        fprintf(stderr, "incorrect number of arguments\n");
        command_help(cmd);
        return -1;
    }

    ret_val = cmd->func(argc, cmd, argv);

    return ret_val;
}

void print_usage()
{
    int i;

    fprintf(stderr, "Usage:\n");
    for(i=0; i<MAX_NUM_COMMANDS; i++) {
        fprintf(stderr, usage_msgs[i]);
    }
    command_helpall();
}


static const char *command_lookup2(char *name)
{
    static char *delimit = " \t";
    static char cbuf[64];
    char *cmd = cbuf;
    int i;

    for(i=0; i<MAX_NUM_COMMANDS; i++) {
        strncpy(cbuf, (char*)usage_msgs[i], sizeof(cbuf)-1);
        cmd = strtok(cbuf, delimit);
        cmd = strtok(NULL, delimit);
        cmd = strtok(NULL, delimit);
        if(strcmp(name, cmd) == 0) {
            return usage_msgs[i];
        }
    }
    return NULL;
}

static void get_opt_optarg(int argc, char *argv[], unsigned int *opt, unsigned int *param)
{
    if (optind < argc && argv[optind][0] != '-')
    {
        (*opt)++; 
        *param = (unsigned int)(strtoul(argv[optind], NULL, 0));
        optind++;
    }
}

#if defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_4908) || defined(CHIP_63158)
static int cfp_read(cfpArg_t *cfpArg)
{
    char buffer[512], *buf=buffer;
    int i;
    buf[0] = 0;
    uint32 v32;

    bcm_cfp_op(cfpArg);

    if (cfpArg->rc == CFP_RC_NON_EXISTING_INDEX)
        return -1;

    buf += sprintf(buf, "  --index %d", cfpArg->index);
    for (i=0; i<sizeof(cfpArg->argFlag)*8; i++) {
        if ((cfpArg->argFlag & (1<<i)) == 0)
            continue;

        switch((1<<i)) {
            case CFP_ARG_SPMAP_M:
                buf += sprintf(buf, " --spmap 0x%x", cfpArg->spmap); 
                break;
            case CFP_ARG_DA_M:
                buf += sprintf(buf, " --da %012llx",  cfpArg->da);
                if (cfpArg->da_mask)
                    buf += sprintf(buf, ",%012llx", cfpArg->da_mask);
                break;
            case CFP_ARG_SA_M:
                buf += sprintf(buf, " --sa %012llx",  cfpArg->sa);
                if (cfpArg->sa_mask)
                    buf += sprintf(buf, ",%012llx", cfpArg->sa_mask);
                break;
            case CFP_ARG_IP_PROTOCOL_M:
                buf += sprintf(buf, " --ip_protocol 0x%04x",  cfpArg->ip_protocol);
                if (cfpArg->ip_protocol_mask)
                    buf += sprintf(buf, ",0x%04x", cfpArg->ip_protocol_mask);
                break;

            case CFP_ARG_L2_FRAMING_M:
                buf += sprintf(buf, " --l2 %d",  cfpArg->l2_framing);
                break;
            case CFP_ARG_L3_FRAMING_M:
                buf += sprintf(buf, " --l3 %d",  cfpArg->l3_framing);
                break;
            case CFP_ARG_DSCP_M:
                buf += sprintf(buf, " --dscp 0x%04x",  cfpArg->dscp);
                if (cfpArg->dscp_mask)
                    buf += sprintf(buf, ",0x%04x", cfpArg->dscp_mask);
                break;
            /* CFP_ARG_PRIORITY_M: */

            /* CFP_ARG_INDEX_M: */
            case CFP_ARG_IPSA_M:
                v32 = htonl(cfpArg->ipsa);
                buf += sprintf(buf, " --ipsa %s", inet_ntoa(*(struct in_addr *)&v32));
                if (cfpArg->ipsa_mask)
                {
                    v32 = htonl(cfpArg->ipsa_mask);
                    buf += sprintf(buf, ",%s", inet_ntoa(*(struct in_addr *)&v32));
                }
                break;
            case CFP_ARG_IPDA_M:
                v32 = htonl(cfpArg->ipda);
                buf += sprintf(buf, " --ipda %s", inet_ntoa(*(struct in_addr *)&v32));
                if (cfpArg->ipda_mask)
                {
                    v32 = htonl(cfpArg->ipda_mask);
                    buf += sprintf(buf, ",%s", inet_ntoa(*(struct in_addr *)&v32));
                }
                break;
            case CFP_ARG_TCPUDP_SPORT_M:
                buf += sprintf(buf, " --tcpudp_sport 0x%x", cfpArg->tcpudp_sport);
                if (cfpArg->tcpudp_sport_mask)
                    buf += sprintf(buf, ",0x%x", cfpArg->tcpudp_sport_mask);
                break;

            case CFP_ARG_TCPUDP_DPORT_M:
                buf += sprintf(buf, " --tcpudp_dport 0x%x", cfpArg->tcpudp_dport);
                if (cfpArg->tcpudp_dport_mask)
                    buf += sprintf(buf, ",0x%x", cfpArg->tcpudp_dport_mask);
                break;
            case CFP_ARG_NEW_DSCP_IB_M:
                buf += sprintf(buf, " --new_dscp_ib 0x%x", cfpArg->new_dscp_ib);
                break;
            case CFP_ARG_CHG_FPMAP_IB_M:
                buf += sprintf(buf, " --chg_fpmap_ib %d", cfpArg->chg_fpmap_ib);
                break;
            case CFP_ARG_FPMAP_IB_M:
                buf += sprintf(buf, " --fpmap_ib 0x%x", cfpArg->fpmap_ib);
                break;

            case CFP_ARG_SVLAN_TAG_M:
                buf += sprintf(buf, " --svtag 0x%x", cfpArg->svtag);
                if (cfpArg->svtag_mask)
                    buf += sprintf(buf, ",0x%x", cfpArg->svtag_mask);
                break;
            case CFP_ARG_CVLAN_TAG_M:
                buf += sprintf(buf, " --svtag 0x%x", cfpArg->cvtag);
                if (cfpArg->cvtag_mask)
                    buf += sprintf(buf, ",0x%x", cfpArg->cvtag_mask);
                break;
            case CFP_ARG_PPPOE_M:
                buf += sprintf(buf, " --pppoe %d", cfpArg->pppoe);
                break;
            case CFP_ARG_ETYPE_SAP_M:
                buf += sprintf(buf, " --etype_sap 0x%x", cfpArg->etype_sap);
                if (cfpArg->etype_sap_mask)
                    buf += sprintf(buf, ",0x%x", cfpArg->etype_sap_mask);
                break;
        }
    }
    printf("%s\n", buffer);
    return 1;
}
#endif

#define MAX_PERSISTENT_WAN_PORT     39
#ifdef BUILD_STATIC
int ethswctl_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    int c;
    char *name = NULL, *operation = NULL, *if_name = NULL, *str;
    const char *msg;
    int port = 0, page = 0, exclusive = 0,  priority = 0, type = 0, mode = -1, enable = -1;
    int i, unit = 0, ret_val = 0, fwd_map = 0, untag_map = 0, len = 0, lval = -1, channel = -1;
    unsigned int param1 = 0, param2 = 0, param3 = 0, param4 = 0, param_b = 0;
    unsigned int val = 0;
    unsigned int val32 = 0;
    unsigned long long val64 = 0, mac_addr = 0, regdata = 0;
    unsigned int u1 = 0, u2 = 0, u3 = 0, u4 = 0, u5 = 0, u6 = 0, u7 = 0;
    int i8;
    char val8;
    unsigned char data[8], mac[6];
    unsigned short v16;
    int temp = 0;
    int Type = 0;
    int longIdx;
    cfpArg_t cfpArg;
    static char *qosMethodString[4] = {"PortBasedQoS", "MACBasedQoS", "802.1PQoS", "DiffServeQoS"};
#if defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_4908) || defined(CHIP_63158)
#define SF2_QOS_TC_SRC_SEL_PKT_TYPE_ALL 8
#define SF2_QOS_TC_SRC_SEL_VAL_MASK 3
    static char *pktTypeMaskString[8] = { "NoMatch", "IPv4/6", "PriorityTagged", "PriorityTagged|IPv4/6",
        "StaticMAC", "StaticMAC|IPv4/6", "StaticMAC|PriorityTagged", "StaticMAC|PriorityTag|IPv4/6"};
    unit = 1;
#endif

    memset(&cfpArg, 0, sizeof(cfpArg));
    while ((c = getopt_long(argc, argv, "abc:d:e:f:hi:l:m:n:o:p:P:q:rst:T:u:v:V:wx:y:z:", longopts, &longIdx)) != -1) {

        switch (c) {

            case 'a': opt_a++; break;

            case 'b': opt_b++; get_opt_optarg(argc, argv, &opt_b, &param_b); break;  

            case 'c': opt_c++; exclusive++; name = optarg; break;

            case 'd': opt_d++; regdata = (unsigned long long)(strtoll(optarg, NULL, 0)); break;

            case 'e': opt_e++; enable = (int)strtoul(optarg, NULL, 0); break;

            case 'f': opt_f++; fwd_map = (int)(strtol(optarg, NULL, 0)); break;

            case 'h': opt_h++; break;

            case 'i': opt_i++; if_name = optarg; break;

            case 'l': opt_l++; len = (int) strtol(optarg, NULL, 0);
                      lval = (int) strtol(optarg, NULL, 0); break;

            case 'm': opt_m++; mac_addr = (unsigned long long)(strtoll(optarg, NULL, 16)); 
                      mode = (int)strtoul(optarg, NULL, 0); break;

            case 'n': opt_n++; unit = channel = (int)(strtol(optarg, NULL, 0)); break;

            case 'o': opt_o++; operation = optarg; break;

            case 'p': opt_p++; port = (int)(strtol(optarg, NULL, 0)); break;

            case 'P': opt_P++; page = (int)(strtol(optarg, NULL, 0)); break;

            case 'q': opt_q++; priority = (int)(strtol(optarg, NULL, 0));
                      cfpArg.argFlag |= CFP_ARG_PRIORITY_M;
                      cfpArg.priority = priority;
                      break;

            case 'r': opt_r++;  break;

            case 's': opt_s++; break;

            case 't': opt_t++; type = (int)(strtol(optarg, NULL, 0)); break;

#if defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_4908) || defined(CHIP_63158)
            case 'T': opt_T++; Type = (int)(strtol(optarg, NULL, 0)); break;
#endif

            case 'u': opt_u++; untag_map = (int)(strtol(optarg, NULL, 0)); break;

            case 'v': opt_v++; val = (unsigned int) strtoul(optarg, NULL, 0); break;

            case 'V': opt_V++; exclusive++; break;

            case 'w': opt_w++; get_opt_optarg(argc, argv, &opt_w, &param4); break;  

            case 'x': opt_x++; param1 = (unsigned int)(strtoul(optarg, NULL, 0)); break;

            case 'y': opt_y++; param2 = (unsigned int)(strtoul(optarg, NULL, 0)); break;

            case 'z': opt_z++; param3 = (unsigned int)(strtoul(optarg, NULL, 0)); break;

            case '?':
                      if(name && (msg = command_lookup2(name))) {
                          fprintf(stderr, msg);
                      }
                      else {
                          print_usage();
                      }
                      return -2;
            case OPTVAL(OPT_READ):
                      cfpArg.op = CFPOP_READ;
                      cfpArg.argFlag = CFP_ARG_OP_M;
                      break;
            case OPTVAL(OPT_ADD):
                      cfpArg.op = CFPOP_ADD;
                      cfpArg.argFlag = CFP_ARG_OP_M;
                      break;
            case OPTVAL(OPT_APPEND):
                      cfpArg.op = CFPOP_APPEND;
                      cfpArg.argFlag = CFP_ARG_OP_M;
                      break;
            case OPTVAL(OPT_INSERT):
                      cfpArg.op = CFPOP_INSERT;
                      cfpArg.argFlag = CFP_ARG_OP_M;
                      break;
            case OPTVAL(OPT_DELETE):
                      cfpArg.op = CFPOP_DELETE;
                      cfpArg.argFlag = CFP_ARG_OP_M;
                      break;
            case OPTVAL(OPT_DELETE_ALL):
                      cfpArg.op = CFPOP_DELETE_ALL;
                      cfpArg.argFlag = CFP_ARG_OP_M;
                      break;
            case OPTVAL(OPT_INDEX): 
                      cfpArg.index = strtol(optarg, NULL, 0);
                      cfpArg.argFlag |= CFP_ARG_INDEX_M;
                      break;
            case OPTVAL(OPT_SPMAP):
                      cfpArg.argFlag |= CFP_ARG_SPMAP_M;
                      cfpArg.spmap = strtol(optarg, NULL, 0);
                      break;
            case OPTVAL(OPT_DA):
                      cfpArg.argFlag |= CFP_ARG_DA_M;
                      str = strchr(optarg, ',');
                      if (str) *str = 0;
                      cfpArg.da = (unsigned long long)(strtoll(optarg, NULL, 16)); 
                      if (str) {
                          cfpArg.da_mask = (unsigned long long)(strtoll((str+1), NULL, 16)); 
                          *str = ',';
                      }
                      printf(" da: %llx\n", cfpArg.da); usleep(100000);
                      break;
            case OPTVAL(OPT_SA):
                      cfpArg.argFlag |= CFP_ARG_SA_M;
                      str = strchr(optarg, ',');
                      if (str) *str = 0;
                      cfpArg.sa = (unsigned long long)(strtoll(optarg, NULL, 16)); 
                      if (str) {
                          cfpArg.sa_mask = (unsigned long long)(strtoll((str+1), NULL, 16)); 
                          *str = ',';
                      }
                      break;
            case OPTVAL(OPT_L2):
                      cfpArg.argFlag |= CFP_ARG_L2_FRAMING_M;
                      cfpArg.l2_framing = strtol(optarg, NULL, 0);
                      break;
            case OPTVAL(OPT_L3): 
                      cfpArg.argFlag |= CFP_ARG_L3_FRAMING_M;
                      cfpArg.l3_framing = strtol(optarg, NULL, 0);
                      break;
            case OPTVAL(OPT_SVLAN_TAG):
                      cfpArg.argFlag |= CFP_ARG_SVLAN_TAG_M;
                      str = strchr(optarg, ',');
                      if (str) *str = 0;
                      cfpArg.svtag = strtol(optarg, NULL, 0);
                      if (str) {
                          cfpArg.svtag_mask = strtoll((str+1), NULL, 0); 
                          *str = ',';
                      }
                      break;
            case OPTVAL(OPT_CVLAN_TAG):
                      cfpArg.argFlag |= CFP_ARG_CVLAN_TAG_M;
                      str = strchr(optarg, ',');
                      if (str) *str = 0;
                      cfpArg.cvtag = strtol(optarg, NULL, 0);
                      if (str) {
                          cfpArg.cvtag_mask = strtoll((str+1), NULL, 0);
                          *str = ',';
                      }
                      break;
            case OPTVAL(OPT_PPPOE):
                      cfpArg.argFlag |= CFP_ARG_PPPOE_M;
                      cfpArg.pppoe = 1;
                      break;
            case OPTVAL(OPT_ETYPE):
                      cfpArg.argFlag |= CFP_ARG_ETYPE_SAP_M;
                      str = strchr(optarg, ',');
                      if (str) *str = 0;
                      cfpArg.etype_sap = strtol(optarg, NULL, 0);
                      if (str) {
                          cfpArg.etype_sap_mask = strtoll((str+1), NULL, 0);
                          *str = ',';
                      }
                      break;
            case OPTVAL(OPT_DSCP): 
                      cfpArg.argFlag |= CFP_ARG_DSCP_M;
                      str = strchr(optarg, ',');
                      if (str) *str = 0;
                      cfpArg.dscp = strtol(optarg, NULL, 0);
                      if (str) {
                          cfpArg.dscp_mask = strtoll((str+1), NULL, 0);
                          *str = ',';
                      }
                      break;
            case OPTVAL(OPT_IP_PROTOCOL):
                      cfpArg.argFlag |= CFP_ARG_IP_PROTOCOL_M;
                      str = strchr(optarg, ',');
                      if (str) *str = 0;
                      cfpArg.ip_protocol = strtol(optarg, NULL, 0);
                      if (str) {
                          cfpArg.ip_protocol_mask = strtol((str+1), NULL, 0);
                          *str = ',';
                      }
                      break;
            case OPTVAL(OPT_IPSA):
                      cfpArg.argFlag |= CFP_ARG_IPSA_M;
                      str = strchr(optarg, ',');
                      if (str) *str = 0;
                      inet_aton(optarg, (void *)&cfpArg.ipsa);
                      cfpArg.ipsa = ntohl(cfpArg.ipsa);
                      if (str) {
                          inet_aton(str+1, (void *)&cfpArg.ipsa_mask);
                          cfpArg.ipsa_mask = ntohl(cfpArg.ipsa_mask);
                          *str = ',';
                      }
                      break;
            case OPTVAL(OPT_IPDA):
                      cfpArg.argFlag |= CFP_ARG_IPDA_M;
                      str = strchr(optarg, ',');
                      if (str) *str = 0;
                      inet_aton(optarg, (void *)&cfpArg.ipda);
                      cfpArg.ipda = ntohl(cfpArg.ipda);
                      if (str) {
                          inet_aton(str+1, (void *)&cfpArg.ipda_mask);
                          cfpArg.ipda_mask = ntohl(cfpArg.ipda_mask);
                          *str = ',';
                      }
                      break;
            case OPTVAL(OPT_TCPUDP_SPORT):
                      cfpArg.argFlag |= CFP_ARG_TCPUDP_SPORT_M;
                      str = strchr(optarg, ',');
                      if (str) *str = 0;
                      cfpArg.tcpudp_sport = strtol(optarg, NULL, 0);
                      if (str) {
                          cfpArg.tcpudp_sport_mask = strtol(str+1, NULL, 0);
                          *str = ',';
                      }
                      break;
            case OPTVAL(OPT_TCPUDP_DPORT):
                      cfpArg.argFlag |= CFP_ARG_TCPUDP_DPORT_M;
                      str = strchr(optarg, ',');
                      if (str) *str = 0;
                      cfpArg.tcpudp_dport = strtol(optarg, NULL, 0);
                      if (str) {
                          cfpArg.tcpudp_dport = strtol(str+1, NULL, 0);
                          *str = ',';
                      }
                      break;
            case OPTVAL(OPT_CHANGE_FPMAP_IB):
                      cfpArg.argFlag |= CFP_ARG_CHG_FPMAP_IB_M;
                      cfpArg.chg_fpmap_ib = strtol(optarg, NULL, 0);
                      break;
            case OPTVAL(OPT_FPMAP_IB):
                      cfpArg.argFlag |= CFP_ARG_FPMAP_IB_M;
                      cfpArg.fpmap_ib = strtol(optarg, NULL, 0);
                      break;
            case OPTVAL(OPT_NEW_DSCP_IB):
                      cfpArg.argFlag |= CFP_ARG_NEW_DSCP_IB_M;
                      cfpArg.tcpudp_dport = strtol(optarg, NULL, 0);
                      break;

            default:
                fprintf(stderr, "Unknow arguments: %d, %s\n", c, optarg? optarg: "");
                ret_val = -1;
                goto close_n_ret;
        } /* End of switch */
        temp++;
    } /* End of while */

    if (temp <= 0 && argc >= 2) {
        return handle_new_format_commands(argc, argv);
    }

    /* options check */
    if (exclusive > 1 || exclusive == 0) {
        fprintf(stderr, "more than 1 or no exclusive option is entered\n");
        if(name && (msg = command_lookup2(name))) {
            fprintf(stderr, msg);
        }
        else {
            print_usage();
        }
        return -2;
    }

    if (opt_V) {
        fprintf(stderr, version);
        return 0;
    }

    if (!opt_c) {
        print_usage();
        return 0;
    }
    if (strcmp(name, "hw-switching") == 0) {
        if (opt_h) {
            fprintf(stderr, usage_msgs[HWSWITCHING_MSG]);
            return 0;
        }
        if (opt_o) {
            if (strcmp(operation, "enable") == 0) {
                ret_val = ethswctl_enable_switching();
            } else if (strcmp(operation, "disable") == 0) {
                ret_val = ethswctl_disable_switching();
            } else {
                fprintf(stderr, usage_msgs[HWSWITCHING_MSG]);
            }
        } else {
            ret_val = ethswctl_get_switching_status(&i8);
            if (ret_val == 0) {
                if(i8) {
                    printf("Enabled \n");
                } else {
                    printf("Disabled \n");
                }
            }
        }
    } else if (strcmp(name, "mibdump") == 0) {
        if (opt_h || opt_p == 0) {
            fprintf(stderr, usage_msgs[MIBDUMP_MSG]);
            return 0;
        }

        if (opt_s) {
            port |= BCM_EXT_6829;
        }

        if (!opt_q) {
            priority = -1;
        }

        ret_val = ethswctl_mibdump_x(unit, port, priority, opt_a);
    } else if (strcmp(name, "pagedump") == 0) {
        if (opt_h || opt_P == 0) {
            fprintf(stderr, usage_msgs[PAGEDUMP_MSG]);
            return 0;
        }
        if (opt_s) {
            page |= BCM_EXT_6829;
        }
        ret_val = ethswctl_pagedump(unit, page);
    } else if (strcmp(name, "iudma") == 0) {
        if (opt_h) {
            fprintf(stderr, usage_msgs[IUDMADUMP_MSG]);
            return 0;
        }

        mode = 0;
        if (opt_r) {
            mode |= ETHSW_DMA_RX;
        }

        if (opt_w) {
            mode |= ETHSW_DMA_TX;
        }

        if (!opt_w && !opt_r) {
            mode = ETHSW_DMA_RX|ETHSW_DMA_TX;
        }

        if (!opt_d) {
            regdata = 0;
        }

        if (opt_e && !opt_n) {
            fprintf(stderr, "Need to specify a channel when use -e\n");
            fprintf(stderr, usage_msgs[IUDMADUMP_MSG]);
            return 0;
        }

        ret_val = bcm_iudma_op(channel, mode, opt_a>0, enable, regdata, param_b);
    } else if (strcmp(name, "iudmasplit") == 0) {
        int iudma;

        if (!opt_p) {
            fprintf(stderr, usage_msgs[IUDMASPLIT_MSG]);
            return 0;
        }
        if(!opt_q)
            iudma = -1;
        else
            iudma = priority;
        ret_val = bcm_iudma_split(port, iudma);
    } else if (strcmp(name, "swctrl") == 0) {
        if (opt_h) {
            fprintf(stderr, usage_msgs[SWCTRL_MSG]);
            return 0;
        }
        if (!opt_t) {
            fprintf(stderr, "Type (-t option) should be specified\n");
            fprintf(stderr, usage_msgs[SWCTRL_MSG]);
            return 0;
        }
        if (opt_v) {
            if (val > 2) {
                printf("Illegal val !\n");
                return 0;
            }
            ret_val = bcm_switch_control_set(unit, type, (int)val);
        } else {
            ret_val = ethswctl_control_get(unit, type, &val);
        }
    } 
    else if (strcmp(name, "swprioctrl") == 0) {
        if (opt_h) {
            fprintf(stderr, usage_msgs[SWPRIOCTRL_MSG]);
            return 0;
        }

        if (!opt_p) {
            port = -1;
        }

        if (!opt_q) {
            priority = -1;
        }

        if (!opt_t) {
            type = -1;
        }

        if (opt_m && (mode > ThreModeMax || mode < ThreModeMin))
        {
            fprintf(stderr, "Error: Invalid Mode value\n");
            fprintf(stderr, usage_msgs[SWPRIOCTRL_MSG]);
            return 0;
        }

        if (opt_v || opt_m || opt_l) {
            ret_val = ethswctl_control_priority_op(unit, port, priority, type, &val, &mode, &lval, 0);
        } else {
            ret_val = ethswctl_control_priority_op(unit, port, priority, type, &val, &mode, &lval, 1);
        }
    }
#if defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_4908) || defined(CHIP_63158)
    else if (strcmp(name, "swflowctrl") == 0) {
        if (opt_h) {
            fprintf(stderr, usage_msgs[SWFLOWCTRL_MSG]);
            return 0;
        }
        if (!opt_t) {
            fprintf(stderr, "Type (-t option) should be specified\n");
            fprintf(stderr, usage_msgs[SWFLOWCTRL_MSG]);
            return 0;
        }
        if (opt_v) {
            ret_val = ethswctl_flow_control_set(unit, port, type, val);
        } else {
            ret_val = ethswctl_flow_control_get(unit, type, &val);
        }
    } else if (strcmp(name, "quemap") == 0) {
        if (opt_h) {
            fprintf(stderr, usage_msgs[SWQUEMAP_MSG]);
            return 0;
        }

        if (!opt_v) val = -1;
        if (!opt_q) priority = -1;
        ret_val = ethswctl_quemap(unit, val, priority);
    }
    else if (strcmp(name, "quemon") == 0) {
        if (opt_h) {
            fprintf(stderr, usage_msgs[SWQUEMON_MSG]);
            return 0;
        }

        if (!opt_p) {
            port = -1;
        }

        if (!opt_q) {
            priority = -1;
        }

        if (!opt_t) {
            type = -1;
        }

        ret_val = ethswctl_quemon_op(unit, port, priority, type, (int*)&val);
    }
    else if (strcmp(name, "acb_cfg") == 0) {
        if (opt_h) {
            fprintf(stderr, usage_msgs[SWACB_MSG]);
            return 0;
        }
        if (!opt_t) {
            fprintf(stderr, "Type (-t option) should be specified\n");
            fprintf(stderr, usage_msgs[SWACB_MSG]);
            return 0;
        }
        if (opt_v) {
            ret_val = bcm_acb_cfg_set(unit, priority, type, val);
        } else {
            ret_val = ethswctl_acb_cfg_get(unit, priority, type);
        }
    }
    else if (strcmp(name, "cfp") == 0) {
        if (opt_h || cfpArg.argFlag == 0) {
            fprintf(stderr, usage_msgs[SWCFPTCAM_MSG]);
            return 0;
        }

        if ((cfpArg.argFlag & CFP_ARG_MUST_ARGS_M) == 0) {
            fprintf(stderr, "Operation or actions arguments are missing\n");
            fprintf(stderr, usage_msgs[SWCFPTCAM_MSG]);
            return 0;
        }


        if ((cfpArg.argFlag & CFP_ARG_L3_FRAMING_M) == 0) {
            if (cfpArg.argFlag & CFP_ARG_IP_FLAGS)
                cfpArg.l3_framing = CfpL3Ipv4;
            else {
                if ((cfpArg.argFlag & CFP_ARG_ETYPE_SAP_M) && cfpArg.etype_sap == 0x0800) {
                    cfpArg.l3_framing = CfpL3Ipv4;
                    cfpArg.argFlag &= ~CFP_ARG_ETYPE_SAP_M;
                }
                else
                    cfpArg.l3_framing = CfpL3NoIP;
            }
            cfpArg.argFlag |= CFP_ARG_L3_FRAMING_M;
        }

        if ((cfpArg.argFlag & OPT_CHANGE_FPMAP_IB) ^ (cfpArg.argFlag & OPT_FPMAP_IB)) {
            fprintf(stderr, "--chng_fpmap_ib and --fpmap_ib need to set together.\n");
            ret_val = -1;
            goto close_n_ret;
        }

        if (cfpArg.op == CFPOP_READ) {
            int pr, prSt, prEnd;
            if ((cfpArg.argFlag & CFP_ARG_PRIORITY_M))
                prSt = prEnd = cfpArg.priority;
            else {
                prSt = 0;
                prEnd = 2;
            }

            for (pr = prSt; pr<=prEnd; pr++)
            {
                printf("CFP rules: Priority %d\n", pr);
                printf("==============================================================================\n");
                if ((cfpArg.argFlag & CFP_ARG_INDEX_M)) {
                    cfp_read(&cfpArg);
                    if (cfpArg.rc == CFP_RC_NON_EXISTING_INDEX)
                        fprintf (stderr, "Failed: requested entry of the index does not exist\n");
                }
                else  {
                    for (cfpArg.index = -1, cfpArg.priority = pr, cfpArg.op = CFPOP_READ, cfp_read(&cfpArg), 
                            cfpArg.op = CFPOP_READ_NEXT; cfpArg.rc != CFP_RC_NON_EXISTING_INDEX;) 
                        cfp_read(&cfpArg);
                }
            }
        }
        else
        {
            bcm_cfp_op(&cfpArg);
            switch (cfpArg.rc)
            {
                case CFP_RC_SUCCESS:
                    break;
                case CFP_RC_NON_EXISTING_INDEX:
                    if (cfpArg.op != CFPOP_DELETE_ALL)
                        fprintf(stderr, "Failed: requested entry of the index does not exist\n");
                        ret_val = -1;
                    break;
                case CFP_RC_CFP_FULL:
                    fprintf(stderr, "Failed: Current CFP TCAM resource is full\n");
                    ret_val = -1;
                    break;
                case CFP_RC_UDF_FULL:
                    fprintf(stderr, "Failed: Current CFP UDF resource is full\n");
                    ret_val = -1;
                    break;
            } 
        }
    }
#endif
    else if (strcmp(name, "rxscheduling") == 0) {
        if (opt_h) {
            fprintf(stderr, usage_msgs[RXSCHED_MSG]);
            return 0;
        }
        if (opt_v) {
            if ((val != SP_SCHEDULING) && (val != WRR_SCHEDULING)) {
                fprintf(stderr, "Invalid Scheduling Type\n");
                ret_val = -1;
                goto close_n_ret;
            }
            ret_val = bcm_enet_driver_rx_scheduling_set(unit, (int)val);
        } else {
            ret_val = bcm_enet_driver_rx_scheduling_get(unit, (int *)&val);
            if (ret_val) {
                goto close_n_ret;
            } else if (val == SP_SCHEDULING) {
                printf("Strict Priority Scheduling\n");
            } else {
                printf("Weighted Round Robin Scheduling\n");
            }
        }
    } else if (strcmp(name, "wrrparams") == 0) {
        if (opt_h) {
            fprintf(stderr, usage_msgs[WRRPARAMS_MSG]);
            return 0;
        }
        if (opt_v) {
            if (!opt_t) {
                fprintf(stderr, "Type (-t) should be specified\n");
                fprintf(stderr, usage_msgs[WRRPARAMS_MSG]);
                return 0;
            }
            ret_val = ethswctl_wrr_param_set(unit, type, (int)val);
        } else {
            ret_val = ethswctl_wrr_param_get(unit);
        }
    } else if (strcmp(name, "vlan") == 0) {
        if (opt_h) {
            fprintf(stderr, usage_msgs[VLAN_MSG]);
            return 0;
        }
        if (!opt_v) {
            fprintf(stderr, "vlan_id (-v option) must be specified\n");
            fprintf(stderr, usage_msgs[VLAN_MSG]);
            return 0;
        }
    if (opt_f && opt_u) {
            ret_val = bcm_vlan_port_set(unit, (int)val, fwd_map, untag_map);
    } else {
        ret_val = bcm_vlan_port_get(unit, (int)val, &fwd_map, &untag_map);
            if (ret_val) {
                goto close_n_ret;
            }
            printf("The fwd_map is 0x%04x\n", fwd_map);
            printf("The untag_map is 0x%04x\n", untag_map);
        }
    } else if (strcmp(name, "getrxcntrs") == 0) {
        if (opt_h) {
            fprintf(stderr, usage_msgs[GETRXCNTRS_MSG]);
            return 0;
        }
        ret_val = bcm_switch_getrxcntrs();
    } else if (strcmp(name, "resetrxcntrs") == 0) {
        if (opt_h) {
            fprintf(stderr, usage_msgs[RESETRXCNTRS_MSG]);
            return 0;
        }
        ret_val = bcm_switch_resetrxcntrs();
    } else if (strcmp(name, "pbvlanmap") == 0) {
        if (opt_h) {
            fprintf(stderr, usage_msgs[PBVLANMAP_MSG]);
            return 0;
        }
    if (opt_v) {
            ret_val = bcm_port_pbvlanmap_set(unit, port, val);
        } else {
            ret_val = bcm_port_pbvlanmap_get(unit, port, &fwd_map);
            if (ret_val) {
                goto close_n_ret;
            }
            printf("The pbvlan_map is 0x%04x\n", fwd_map);
        }
    } else if (strcmp(name, "getstat") == 0) {
        if (opt_h) {
            fprintf(stderr, usage_msgs[GETSTAT_MSG]);
            return 0;
        }
        if (!opt_t || !opt_p) {
            fprintf(stderr, "Type (-t) and port(-p) should be specified\n");
            fprintf(stderr, usage_msgs[GETSTAT_MSG]);
            return 0;
        }
        ret_val = bcm_stat_get(unit, port, type, &val64);
        if (ret_val) {
            goto close_n_ret;
        }
        printf("val64 = 0x%llx\n", val64);
    } else if (strcmp(name, "getstat32") == 0) {
        if (opt_h) {
            fprintf(stderr, usage_msgs[GETSTAT32_MSG]);
            return 0;
        }
        if (!opt_t || !opt_p) {
            fprintf(stderr, "Type (-t) and port(-p) should be specified\n");
            fprintf(stderr, usage_msgs[GETSTAT32_MSG]);
            return 0;
        }
        ret_val = bcm_stat_get32(unit, port, type, &val32);
        if (ret_val) {
            goto close_n_ret;
        }
        printf("val32 = %x\n", (unsigned int)val32);
    } else if (strcmp(name, "pause") == 0) {
        if (opt_h) {
            fprintf(stderr, usage_msgs[PAUSE_MSG]);
            return 0;
        }
        if (!opt_p) {
            fprintf(stderr, "Port(-p) should be specified\n");
            fprintf(stderr, usage_msgs[PAUSE_MSG]);
            return 0;
        }
        if (opt_v) {
            ret_val = bcm_port_pause_capability_set(unit, port, val);
        } else {
            ret_val = bcm_port_pause_capability_get(unit, port, &val8);
            if (ret_val) {
                goto close_n_ret;
            }
            printf("The val8 = %d\n", val8);
            if (val8 == PAUSE_FLOW_CTRL_BOTH) {
                printf("Both Tx and Rx Pause Enabled\n");
            } else if (val8 == PAUSE_FLOW_CTRL_TX) {
                printf("Tx Pause Enabled\n");
            } else if (val8 == PAUSE_FLOW_CTRL_RX) {
                printf("Rx Pause Enabled\n");
            } else {
                printf("Pause Disabled\n");
            }
        }
    } else if (strcmp(name, "cosq") == 0) {
        if (opt_h) {
            fprintf(stderr, usage_msgs[COSQ_MSG]);
            return 0;
        }
        if (opt_p && opt_q) {
            if (opt_v) {
                ret_val = bcm_cosq_port_mapping_set(unit, port, priority, val);
            } else {
                ret_val = bcm_cosq_port_mapping_get(unit, port, priority, (int *)&val);
                if (ret_val) {
                    goto close_n_ret;
                }
                printf("For Port %2d, priority %2d is mapped to queue %2d\n",
                        port, priority, val);
            }
        } else if (opt_q) {
            if (opt_v) {
                ret_val =bcm_cosq_rxchannel_mapping_set(unit, priority, val);
            } else {
                ret_val = bcm_cosq_rxchannel_mapping_get(unit, priority, (int *)&val);
                if (ret_val) {
                    goto close_n_ret;
                }
                printf("queue %2d is mapped to channel %2d\n", priority, val);
            }
        } else if (opt_v) {
            ret_val = bcm_cosq_config_set(unit, val);
        } else {
            ret_val = bcm_cosq_config_get(unit, (int *)&val);
            if (ret_val) {
                goto close_n_ret;
            }
            printf("Number of COS queues: %2d\n", val);
        }
    } else if (strcmp(name, "costxq") == 0) {
        if (opt_h) {
            fprintf(stderr, usage_msgs[COSTXQ_MSG]);
            return 0;
        }
        if (opt_v) {
            if (opt_q) {
                ret_val = bcm_cosq_txchannel_mapping_set(unit, val, priority);
            } else {
                ret_val = bcm_cosq_txchannel_mapping_get(unit, val, &priority);
                if (ret_val) {
                    goto close_n_ret;
                }
                printf("tx channel %2d is mapped to queue %2d\n", val, priority);
            }
        } else {
            if (opt_t) {
                ret_val = bcm_cosq_txq_selection_set(unit, type);
            } else {
                ret_val = bcm_cosq_txq_selection_get(unit, &type);
                if (ret_val) {
                    goto close_n_ret;
                }
                printf("The switch egress queue selection is based on ");
                if (type == USE_TX_BD_PRIORITY) {
                    printf("Tx BD priority\n");
                } else if (type == USE_TX_DMA_CHANNEL) {
                    printf("Tx iuDMA channel\n");
                } else {
                    printf("neither Tx BD nor Tx channel\n");
                }
            }
        }
    } else if (strcmp(name, "byteratecfg") == 0) {
        if (opt_h) {
            fprintf(stderr, usage_msgs[BYTERATECFG_MSG]);
            return 0;
        }
        if (!opt_v) {
            fprintf(stderr, "Channel (-v option) should be specified\n");
            fprintf(stderr, usage_msgs[BYTERATECFG_MSG]);
            return 0;
        }
        if (opt_x) {
            ret_val = bcm_enet_driver_rx_rate_limit_cfg_set(unit, val, param1);
        } else {
            ret_val = bcm_enet_driver_rx_rate_limit_cfg_get(unit, val, (int *)&param1);
            if (ret_val) {
                goto close_n_ret;
            }
            /* Display the enable/disable status returned through type */
            if(param1) {
                printf("Rate Limiting of Channel-%d is Enabled\n", val);
            } else {
                printf("Rate Limiting of Channel-%d is Disabled\n", val);
            }
        }
    } else if (strcmp(name, "byterate") == 0) {
        if (opt_h) {
            fprintf(stderr, usage_msgs[BYTERATE_MSG]);
            return 0;
        }
        if (!opt_v) {
            fprintf(stderr, "Channel (-v option) should be specified\n");
            fprintf(stderr, usage_msgs[BYTERATE_MSG]);
            return 0;
        }
        if (opt_x) {
            ret_val = bcm_enet_driver_rx_rate_set(unit, val, param1);
        } else {
            ret_val = bcm_enet_driver_rx_rate_get(unit, val, (int *)&param1);
            if (ret_val) {
                goto close_n_ret;
            }
            printf("Channel-%d rate is %d\n", val, param1);
        }
    } else if (strcmp(name, "deftxqen") == 0) {
        if (opt_h) {
            fprintf(stderr, usage_msgs[DEFTXQEN_MSG]);
            return 0;
        }
        if (!opt_i) {
            fprintf(stderr, "The if_name (-i option) should be specified\n");
            fprintf(stderr, usage_msgs[DEFTXQEN_MSG]);
            return 0;
        }
        if (opt_o) {
            if (strcmp(operation, "enable") == 0) {
                val = 1;
            } else {
                val = 0;
            }
            ret_val = bcm_enet_driver_use_default_txq_status_set(unit, if_name, val);
        } else {
            ret_val = bcm_enet_driver_use_default_txq_status_get(unit, if_name, (int *)&val);
            if (ret_val) {
                goto close_n_ret;
            } else if (val) {
                printf("Use of default txq is enabled for %s\n", if_name);
            } else {
                printf("Use of default txq is disabled for %s\n", if_name);
            }
        }
    } else if (strcmp(name, "deftxq") == 0) {
        if (opt_h) {
            fprintf(stderr, usage_msgs[DEFTXQ_MSG]);
            return 0;
        }
        if (!opt_i) {
            fprintf(stderr, "The if_name (-i option) should be specified\n");
            fprintf(stderr, usage_msgs[DEFTXQ_MSG]);
            return 0;
        }
        if (opt_v) {
            ret_val = bcm_enet_driver_default_txq_set(unit, if_name, val);
        } else {
            ret_val = bcm_enet_driver_default_txq_get(unit, if_name, (int *)&val);
            if (ret_val) {
                goto close_n_ret;
            }
            printf("Default txq for %s is %d\n", if_name, val);
        }
    } else if (strcmp(name, "test") == 0) {
        if (opt_h) {
            fprintf(stderr, usage_msgs[TEST_MSG]);
            return 0;
        }
        if (!opt_t) {
            fprintf(stderr, "Type (-t option) should be specified\n");
            ret_val = -1;
            return 0;
        }
        if (type == SUBTYPE_ISRCFG) {
            if (!opt_x) {
                printf("Channel (-x option) should be specified\n");
                ret_val = -1;
                goto close_n_ret;
            }
        }
        if (opt_v) {
            ret_val = bcm_enet_driver_test_config_set(unit, type,
                                                      param1, (int)val);
        } else {
            if ((type == SUBTYPE_RESETSWITCH) && opt_s) {
                ret_val = bcm_enet_driver_test_config_get(unit, type,
                                                          (param1 | BCM_EXT_6829),
                                                          (int *)&val);
            } else {
                ret_val = bcm_enet_driver_test_config_get(unit, type,
                                                          param1, (int *)&val);
            }
            if (ret_val) {
                goto close_n_ret;
            }
            printf("ret val is %d\n", (int)val);
        }
    } else if (strcmp(name, "pktratecfg") == 0) {
        if (opt_h) {
            fprintf(stderr, usage_msgs[PKTRATECFG_MSG]);
            return 0;
        }
        if (!opt_v) {
            fprintf(stderr, "Channel (-v option) should be specified\n");
            fprintf(stderr, usage_msgs[PKTRATECFG_MSG]);
            return 0;
        }
        if (opt_x) {
            ret_val = bcm_enet_driver_rx_pkt_rate_cfg_set(unit, val, param1);
        } else {
            ret_val = bcm_enet_driver_rx_pkt_rate_cfg_get(unit, val, (int *)&param1);
            if (ret_val) {
                goto close_n_ret;
            }
            /* Display the enable/disable status returned through type */
            if(param1) {
                printf("Rate Limiting of Channel-%d is Enabled\n", val);
            } else {
                printf("Rate Limiting of Channel-%d is Disabled\n", val);
            }
        }
    } else if (strcmp(name, "pktrate") == 0) {
        if (opt_h) {
            fprintf(stderr, usage_msgs[PKTRATE_MSG]);
            return 0;
        }
        if (!opt_v) {
            fprintf(stderr, "Channel (-v option) should be specified\n");
            fprintf(stderr, usage_msgs[PKTRATE_MSG]);
            return 0;
        }
        if (opt_x) {
            ret_val = bcm_enet_driver_rx_pkt_rate_set(unit, val, param1);
        } else {
            ret_val = bcm_enet_driver_rx_pkt_rate_get(unit, val, (int *)&param1);
            if (ret_val) {
                goto close_n_ret;
            }
            printf("Channel-%d rate is %d\n", val, param1);
        }
    } else if (strcmp(name, "pad") == 0) {
        if (opt_v) {
            bcm_packet_padding_set(unit, val, len);
        } else {
            bcm_packet_padding_get(unit, &ret_val, &len);
            if (ret_val) {
                printf("Padding is enabled with min len of %d\n", len);
            } else {
                printf("Padding is disabled\n");
            }
        }
    } else if (strcmp(name, "arl") == 0) {
        bcm_vlan_t vid = val;
        if (opt_h) {
            fprintf(stderr, usage_msgs[ARL_MSG]);
            return 0;
        }
        if (!opt_m) {
            fprintf(stderr, "-m mac_addr should be specified\n");
            fprintf(stderr, usage_msgs[ARL_MSG]);
            return 0;
        }
        for (i = 0; i < 6; i++) {
            mac[i] = (mac_addr >> ((5-i)*8)) & 0xff;
        }
        if (opt_x) {
            if (!opt_v) {
                vid = 0;
            }
            ret_val = bcm_arl_write2(unit, (char *)mac, vid, param1);
        } else {
            #define MAX_SWITCH_NUMBER 2
            int n, n1 = 0, n2 = MAX_SWITCH_NUMBER, found = 0;
            if (!opt_v) {
                vid = -1;
            }

            if (opt_n) {
                n1 = unit;
                n2 = unit + 1;
            }

            for (n=n1; n < n2; n++) {
                ret_val = bcm_arl_read2(&n, (char *)mac, &vid, &v16);
                if (!ret_val) {
                    printf("Found Entry in %s Switch:\n", unit? "External": "Internal");
                    printf("MAC: %02x:%02x:%02x:%02x:%02x:%02x; VLAN ID: %d\n",
                            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], vid);
                    printf("Data(15:Valid,14:Static,13:Age,Prio(12-10),8-0:Port/Pmap): 0x%04x\n", v16);
                    found++;
                }
                else {
                    if (n2 == n1 + 1) {
                        printf("Switch unit %d does not exist\n", n1);
                    }
                }
            }

            if (found == 0)
                fprintf(stderr, "Specified MAC address or VLAN was not found.\n");
        }
    } else if (strcmp(name, "arldump") == 0) {
        if (opt_h) {
            fprintf(stderr, usage_msgs[ARLDUMP_MSG]);
            return 0;
        }
        ret_val = bcm_arl_dump(unit);
    } else if (strcmp(name, "arlflush") == 0) {
        if (opt_h) {
            fprintf(stderr, usage_msgs[ARLFLUSH_MSG]);
            return 0;
        }
        ret_val = bcm_arl_flush(unit);
        if (ret_val) {
            goto close_n_ret;
        }
        printf("ARL table flush done\n");
    } else if (strcmp(name, "untaggedprio") == 0) {
        if (opt_h) {
            fprintf(stderr, usage_msgs[UNTAGGEDPRIO_MSG]);
            return 0;
        }
        if (!opt_p) {
            fprintf(stderr, "Port(-p) should be specified\n");
            fprintf(stderr, usage_msgs[UNTAGGEDPRIO_MSG]);
            return 0;
        }
        if (opt_v) {
            ret_val = bcm_port_untagged_priority_set(unit, port, val);
        } else {
            ret_val = bcm_port_untagged_priority_get(unit, port, (int *)&val);
            if (ret_val) {
                goto close_n_ret;
            }
            printf("Port %d default tag priority is %d\n", port, val);
        }
    } else if (strcmp(name, "cosqpriomethod") == 0) {
        if (opt_h) {
            fprintf(stderr, usage_msgs[COSQPRIOMETHOD_MSG]);
            return 0;
        }

#if defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_4908) || defined(CHIP_63158)
        if (!opt_p || !opt_t)
        {
            fprintf(stderr, "-p <port> and -t <packet type mask> should be specified\n");
            fprintf(stderr, usage_msgs[COSQPRIOMETHOD_MSG]);
            return 0;
        }
#else
        if (!opt_p)
        {
            port = 0;   /* Don't know need port or not depending on HW value */
        }
#endif

        if (opt_v) {
#if defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_4908) || defined(CHIP_63158)
            if (type != SF2_QOS_TC_SRC_SEL_PKT_TYPE_ALL && opt_b) {
                val |= 1<<16;
            }
#endif
            ret_val = bcm_cosq_priority_method_set_X(unit, port, val, type);
        } else {
            ret_val = bcm_cosq_priority_method_get_X(unit, port, (int *)&val32, type);
            if (ret_val) {
                goto close_n_ret;
            }
#if defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_4908) || defined(CHIP_63158)
            if (type == SF2_QOS_TC_SRC_SEL_PKT_TYPE_ALL) {
                for (i = 0; i < type; i++) {
                    val = (val32 >> (i * 2)) & SF2_QOS_TC_SRC_SEL_VAL_MASK;
                    printf("port %d: Packet Type: %x-%-30s; QoS Method: %x-%s - %s\n",
                            port, i, pktTypeMaskString[i], val, qosMethodString[val], ((val32>>(16+i))&1)? "Disabled": "Enabled");
                }
            } else {
                    val = val32 & SF2_QOS_TC_SRC_SEL_VAL_MASK;
                printf("port %d: Packet Type: %x-%s; QoS Method: %x-%s - %s\n",
                            port, type, pktTypeMaskString[type], val, qosMethodString[val], (val32>>16)? "Disabled": "Enabled");
            }
#else
            if(val32 > DIFFSERV_QOS) {
                val32 = DIFFSERV_QOS;
            }

            if (val32 > MAC_QOS) {
                printf("Port %d QoS Method: %s\n", port, qosMethodString[val32]);
            }else {
                printf("QoS Method: %s\n", qosMethodString[val32]);
            }
#endif
        }
    } else if (strcmp(name, "cosqsched") == 0) {
        int weights[BCM_COS_COUNT];
        port_qos_sched_t qs;
#if defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_4908) || defined(CHIP_63158)
        if (opt_h) {
            fprintf(stderr, usage_msgs[COSQSCHED_MSG]);
            return 0;
        }
        if (opt_v) { //SET
            // validate type 1..4
            memset(&qs, 0, sizeof(port_qos_sched_t));
            if (opt_t) {
                if ((type < 1) || (type > 4)) {
                    fprintf(stderr, " sp_qnum (%d) Error: not in range 1..4\n", type);
                    goto close_n_ret;
                }
            } else {
                type = 0;
            }
            qs.num_spq = type;
            if (!opt_T || Type == 0) {
                qs.wrr_type = 1;

            } else {
                qs.wrr_type = 2;
            }
            /*
             * All queues in SP, no weights to program.
             */
            if (opt_o && (!strcmp(operation, "upper"))) {
                qs.weights_upper = 1;
                weights[4] = param1;
                weights[5] = param2;
                weights[6] = param3;
                weights[7] = param4;
            } else {
                qs.weights_upper = 0;
                weights[0] = param1;
                weights[1] = param2;
                weights[2] = param3;
                weights[3] = param4;
            }
            qs.sched_mode = val;
            ret_val = bcm_cosq_sched_set_X(unit, port, weights, &qs);
        } else { // GET
            ret_val = bcm_cosq_sched_get_X(unit, port, weights, &qs);
            if (ret_val) {
                goto close_n_ret;
            }
            if(qs.sched_mode == BCM_COSQ_STRICT) {
                printf("Strict Priority Scheduling\n");
            } else if (qs.sched_mode == BCM_COSQ_WRR) {
                printf("%s Round Robin Scheduling\n", qs.wrr_type == 1? "Weighted": "Deficit");
                printf("Weights: %d %d %d %d %d %d %d %d\n",
                        weights[7], weights[6], weights[5], weights[4],
                        weights[3], weights[2], weights[1], weights[0]);
            } else if (qs.sched_mode == BCM_COSQ_COMBO) {
                printf("SP+%s combo scheduling\n",  qs.wrr_type == 1? "WRR": "WDR");
                printf("SP from Queue7 to Queue%d and WRR for"
                        " remaining queues\n", BCM_COS_COUNT - qs.num_spq);
                printf("Weights: %d %d %d %d %d %d %d %d\n",
                        weights[7], weights[6], weights[5], weights[4],
                        weights[3], weights[2], weights[1], weights[0]);
            }
        }
#else
        if (opt_h) {
            fprintf(stderr, usage_msgs[COSQSCHED_MSG]);
            return 0;
        }
        if (opt_v) {
            weights[0] = param1;
            weights[1] = param2;
            weights[2] = param3;
            weights[3] = param4;
            qs.sched_mode = val;
            qs.num_spq = type;
            // in the following arg list:  port, qs.weights_upper, qs.wrr_type do not apply for Legacy
            // They are there as we unified the API across switches
            ret_val = bcm_cosq_sched_set_X(unit, 0, weights, &qs);
        } else {
            ret_val = bcm_cosq_sched_get_X(unit, 0, weights, &qs);
            if (ret_val) {
                goto close_n_ret;
            }
            if(qs.sched_mode == BCM_COSQ_STRICT) {
                printf("Strict Priority Scheduling\n");
            } else if (qs.sched_mode == BCM_COSQ_WRR) {
                printf("Weighted Round Robin Scheduling\n");
                printf("Weights: %d %d %d %d\n", weights[0], weights[1],
                        weights[2], weights[3]);
            } else if (qs.sched_mode == BCM_COSQ_COMBO) {
                printf("SP+WRR combo scheduling\n");
                printf("SP from Queue3 to Queue%d and WRR for"
                        " remaining queues\n", qs.num_spq);
                printf("Weights: %d %d %d %d\n", weights[0], weights[1],
                        weights[2], weights[3]);
            }
        }
#endif
    } else if (strcmp(name, "portctrl") == 0) {
        if (opt_h) {
            fprintf(stderr, usage_msgs[PORTCTRL_MSG]);
            return 0;
        }
        if (opt_v) {
            bcm_port_traffic_control_set(unit, port, val);
            bcm_port_traffic_control_get(unit, port, (int *)&param1);
        } else {
            ret_val = bcm_port_traffic_control_get(unit, port, (int *)&param1);
            if (ret_val) {
                goto close_n_ret;
            }
        }
        printf("Unit %d Port %d: Rx is %s, Tx is %s\n", unit, port,
                param1&PORT_RXDISABLE?"Disabled":"Enabled",
                param1&PORT_TXDISABLE?"Disabled":"Enabled");
    } else if (strcmp(name, "portloopback") == 0) {
        if (opt_h) {
            fprintf(stderr, usage_msgs[PORTLOOPBACK_MSG]);
            return 0;
        }
        if (opt_v) {
            bcm_port_loopback_set(unit, port, val);
        } else {
            ret_val = bcm_port_loopback_get(unit, port, (int *)&param1);
            if (ret_val) {
                goto close_n_ret;
            }
            if (param1) {
                printf("Port %d loopback is enabled\n", port);
            } else {
                printf("Port %d loopback is disabled\n", port);
            }
        }
    } else if (strcmp(name, "phymode") == 0) {
        if (opt_h) {
            fprintf(stderr, usage_msgs[PHYMODE_MSG]);
            return 0;
        }

        if (opt_p && !opt_y && !opt_z) {
            ret_val = bcm_phy_mode_get(unit, port, (int *)&param1, (int *)&param2);
            if (ret_val) {
                goto close_n_ret;
            }
            if (param1 == 0)
                printf("Down\n");
            else {
                if (param1 == 10000) printf("10G");
                if (param1 == 5000) printf("5G");
                if (param1 == 2500) printf("2.5G");
                if (param1 == 1000) printf("1G");
                if (param1 == 100) printf("100M");
                if (param1 == 10) printf("10M");
                if (param2 == 0) printf(" HD\n");
                if (param2 == 1) printf(" FD\n");
            }
        }
        else if (opt_p &&
                ( (opt_y && param2 == 0 && !opt_z) || /* auto */
                   (opt_y && (param2 == 10000 || param2 == 5000 || param2 == 2500 ||
                   param2 == 1000 || param2 == 100 || param2 == 10) &&
                    (opt_z && (param3 == 0 || param3 == 1)) ))) {
            ret_val = bcm_phy_mode_set(unit, port, (int)param2, (int)param3);
        }
        else {
            fprintf(stderr, usage_msgs[PHYMODE_MSG]);
            return 0;
        }
    } else if (strcmp(name, "jumbo") == 0) { // bill
        if (opt_h || opt_p == 0) {
            fprintf(stderr, usage_msgs[JUMBO_MSG]);
            return 0;
        }

        if (opt_v) {
            // Read/modify/write current JUMBO control register.
            ret_val = bcm_port_jumbo_control_set(unit, port, (int *)&val);
            if (ret_val) {
                goto close_n_ret;
            }
        }
        else {
            // Read current JUMBO control register.
            ret_val = bcm_port_jumbo_control_get(unit, port, (int *)&val);
            if (ret_val) {
                goto close_n_ret;
            }
        }

        // Output jumbo status register.
        outputJumboStatus(port, val);
    } else if (strcmp(name, "dscp2prio") == 0) {
        if (opt_h || opt_v == 0) {
            fprintf(stderr, usage_msgs[DSCP2PRIO_MSG]);
            return 0;
        }

        if (opt_q) {
            ret_val = bcm_cosq_dscp_priority_mapping_set(unit, val, priority);
        } else {
            ret_val = bcm_cosq_dscp_priority_mapping_get(unit, val, &priority);
            if (ret_val) {
                goto close_n_ret;
            }
            printf("DSCP %d is mapped to priority %d\n", val, priority);
        }
    } else if (strcmp(name, "pcp2prio") == 0) {
        if (opt_h || opt_v == 0) {
            fprintf(stderr, usage_msgs[PCP2PRIO_MSG]);
            return 0;
        }

        if (opt_q) {
            ret_val = bcm_cosq_pcp_priority_mapping_set(unit, port, val, priority);
        } else {
            ret_val = bcm_cosq_pcp_priority_mapping_get(unit, port, val, &priority);
            if (ret_val) {
                goto close_n_ret;
            }
            printf("PCP %d is mapped to priority %d\n", val, priority);
        }
    } else if (strcmp(name, "pid2prio") == 0) {
        if (opt_h || opt_p == 0) {
            fprintf(stderr, usage_msgs[PID2PRIO_MSG]);
            return 0;
        }

        if (opt_q) {
            ret_val = bcm_cosq_pid_priority_mapping_set(unit, port, priority);
        } else {
            ret_val = bcm_cosq_pid_priority_mapping_get(unit, port, &priority);
            if (ret_val) {
                goto close_n_ret;
            }
            printf("PID %d is mapped to priority %d\n", port, priority);
        }
    } else if (strcmp(name, "regaccess") == 0) {
        unsigned long long data64 = 0;
        if (opt_h) {
            fprintf(stderr, usage_msgs[REGACCESS_MSG]);
            return 0;
        }
        //len = len >= 6? len | SWAP_TYPE_MASK: len;
        if (opt_d) {
            data64 = cpu_to_le64(regdata);
            ret_val = bcm_reg_write_X(unit, val, (char *)&data64, len);
        } else {
            ret_val = bcm_reg_read_X(unit, val, (char *)&data64, len);
            if (ret_val) {
                goto close_n_ret;
            }
            data64 = le64_to_cpu(data64);
            printf("Data: %08x %08x \n", (unsigned int)(data64 >> 32), (unsigned int)(data64) );
        }
    }
    else if (strcmp(name, "dosctrl") == 0) {
        struct bcm_dos_ctrl_params dosCtrl;
        memset(&dosCtrl,0, sizeof(dosCtrl));
        if (opt_h ) {
            fprintf(stderr, usage_msgs[DOSCTRLCTRL_MSG]);
            return 0;
        }
        if (opt_v) {
            if (val & (1<<1)) dosCtrl.ip_lan_drop_en =1;
            if (val & (1<<2)) dosCtrl.tcp_blat_drop_en =1;
            if (val & (1<<3)) dosCtrl.udp_blat_drop_en =1;
            if (val & (1<<4)) dosCtrl.tcp_null_scan_drop_en =1;
            if (val & (1<<5)) dosCtrl.tcp_xmas_scan_drop_en =1;
            if (val & (1<<6)) dosCtrl.tcp_synfin_scan_drop_en =1;
            if (val & (1<<7)) dosCtrl.tcp_synerr_drop_en =1;
            if (val & (1<<8)) dosCtrl.tcp_shorthdr_drop_en =1;
            if (val & (1<<9)) dosCtrl.tcp_fragerr_drop_en =1;
            if (val & (1<<10)) dosCtrl.icmpv4_frag_drop_en =1;
            if (val & (1<<11)) dosCtrl.icmpv6_frag_drop_en =1;
            if (val & (1<<12)) dosCtrl.icmpv4_longping_drop_en =1;
            if (val & (1<<13)) dosCtrl.icmpv6_longping_drop_en =1;
            if (val & (1<<14)) dosCtrl.dos_disable_lrn =1;
            ret_val = bcm_dos_ctrl_set(1,&dosCtrl); /* unit=1 ; Only External switch supports DosCtrl */
        }
        else {
            ret_val = bcm_dos_ctrl_get(1,&dosCtrl); /* unit=1 ; Only External switch supports DosCtrl */

            if (ret_val) {
                goto close_n_ret;
            }
            printf(" IP_LAN=%d; TCP_BLAT=%d; UDP_BLAT=%d; TCP_NULL=%d;\n",(int)dosCtrl.ip_lan_drop_en,
                   (int)dosCtrl.tcp_blat_drop_en, (int)dosCtrl.udp_blat_drop_en, (int)dosCtrl.tcp_null_scan_drop_en);
            printf(" TCP_XMAS=%d; TCP_SYNFIN=%d; TCP_SYNERR=%d; TCP_SHORTHDR=%d;\n",(int)dosCtrl.tcp_xmas_scan_drop_en,
                   (int)dosCtrl.tcp_synfin_scan_drop_en, (int)dosCtrl.tcp_synerr_drop_en, (int)dosCtrl.tcp_shorthdr_drop_en);
            printf(" TCP_FRAGERR=%d; ICMPv4_FRAG=%d; ICMPv6_FRAG=%d; ICMPv4_LONGPING=%d;\n",(int)dosCtrl.tcp_fragerr_drop_en,
                   (int)dosCtrl.icmpv4_frag_drop_en,(int)dosCtrl.icmpv6_frag_drop_en,(int)dosCtrl.icmpv4_longping_drop_en);
            printf(" ICMPv6_LONGPING=%d;DOS_LRN=%d\n\n",(int)dosCtrl.icmpv6_longping_drop_en, (int)dosCtrl.dos_disable_lrn);

        }

    }
    else if (strcmp(name, "spiaccess") == 0) {
        unsigned long long data64 = 0;
        if (opt_h) {
            fprintf(stderr, usage_msgs[SPIACCESS_MSG]);
            return 0;
        }
        if (opt_d) {
            data64 = cpu_to_le64(regdata);
            for (i = 0; i < 8; i++) {
                data[i] = (unsigned char) (*( ((char *)&data64) + i));
            }
            DBG(printf("Data: %02x%02x%02x%02x %02x%02x%02x%02x\n", data[7],
             data[6], data[5], data[4], data[3], data[2], data[1], data[0]););
            ret_val = bcm_spi_write(param1, (char *)data, len);
        } else {
            memset(data, 0, sizeof(data));
            ret_val = bcm_spi_read(param1, (char *)data, len);
            if (ret_val) {
                goto close_n_ret;
            }
            printf("Data: %02x%02x%02x%02x %02x%02x%02x%02x\n", data[7],
                data[6], data[5], data[4], data[3],
                data[2], data[1], data[0]);
        }
    } else if (strcmp(name, "pmdioaccess") == 0) {
        unsigned long long data64 = 0;
        if (opt_h) {
            fprintf(stderr, usage_msgs[PSEUDOMDIOACCESS_MSG]);
            return 0;
        }
        if (opt_d) {
            data64 = cpu_to_le64(regdata);
            for (i = 0; i < 8; i++) {
                data[i] = (unsigned char) (*( ((char *)&data64) + i));
            }
            DBG(printf("Data: %02x%02x%02x%02x %02x%02x%02x%02x\n", data[7],
             data[6], data[5], data[4], data[3], data[2], data[1], data[0]););
            ret_val = bcm_pseudo_mdio_write(param1, (char*)data, len);
        } else {
            memset(data, 0, sizeof(data));
            ret_val = bcm_pseudo_mdio_read(param1, (char*)data, len);
            if (ret_val) {
                goto close_n_ret;
            }
            printf("Data: %02x%02x%02x%02x %02x%02x%02x%02x\n", data[7],
                data[6], data[5], data[4], data[3],
                data[2], data[1], data[0]);
        }
    } else if (strcmp(name, "setlinkstatus") == 0) {
        if (opt_h) {
            fprintf(stderr, usage_msgs[SETLINKSTATUS_MSG]);
            return 0;
        }
        if (opt_n && opt_p && opt_x && opt_y && opt_z) {
            ret_val = bcm_set_linkstatus(unit, port, param1, param2, param3);
        } else {
            fprintf(stderr, usage_msgs[SETLINKSTATUS_MSG]);
            return 0;
        }
    } else if (strcmp(name, "switchinfo") == 0) {
        if (opt_h) {
            fprintf(stderr, usage_msgs[SWITCHINFO_MSG]);
            return 0;
        }
        if (opt_v) {
            ret_val = bcm_get_switch_info(val, &u1, &u2, &u3, (int *)&param4, &u4, &u5, &u6, &u7, &i8);
            printf("Port Map = 0x%x; Phy Port Map = 0x%x\n", u6, u7);
            if ((param4 == MBUS_SPI) || (param4 == MBUS_HS_SPI)) {
                printf("Switch connected through SPI\n");
                printf("VendorID = 0x%x, Dev ID = 0x%x, Rev ID = 0x%x, "
                       "spi_id = %d, spi_cid = %d\n", u1, u2, u3, u4, u5);
            } else if (param4 == MBUS_MDIO) {
                printf("Switch connected through MDIO\n");
                printf("VendorID = 0x%x, Dev ID = 0x%x, Rev ID = 0x%x, ", u1, u2, u3);
            } else if (param4 == MBUS_UBUS) {
                printf("Switch connected through UBUS\n");
                printf("VendorID = 0x%x, Dev ID = 0x%x, Rev ID = 0x%x, epon_port=%d, ", u1, u2, u3, i8);
            } else if (param4 == MBUS_MMAP) {
                printf("switch connected through MMAP\n");
                printf("VendorID = 0x%x, Dev ID = 0x%x, Rev ID = 0x%x, ", u1, u2, u3);
            } else {
                printf("No switch connected\n");
            }
        } else {
            fprintf(stderr, usage_msgs[SWITCHINFO_MSG]);
            return 0;
        }
    } else if (strcmp(name, "wan") == 0) {
        if (opt_h || (!opt_i ^ !opt_o)) {
            fprintf(stderr, usage_msgs[WAN_MSG]);
            return 0;
        }
        if (opt_o) {
            if (strcmp(operation, "enable") == 0) {
                val = 1;
            } else {
                val = 0;
            }
            DBG(printf("%s ethswctl -c wan SET if %s val %d\n", __FUNCTION__,
                    if_name, val););
            ret_val = bcm_enet_driver_wan_interface_set(if_name, val);
        } else {
            char *if_names;
            unsigned int sz = MAX_PERSISTENT_WAN_PORT * (IFNAMSIZ+1) + 2;
            if ((if_names = malloc(sz)) == NULL) {
                printf("Get Wan, if_names malloc failed\n");
                return -1;
            }
            DBG(printf("%s ethswctl -c wan GET\n", __FUNCTION__););
            if_names[0] = '\0';
            if (bcm_enet_driver_wan_interface_get(if_names, sz) == 0) {
                printf("Interface(s) %s marked WAN\n", if_names);
                printf("%s\n", if_names); // This is for stdout capture and parse
            }
        }
    }
    else if (strcmp(name, "rxratectrl") == 0) {
        if (opt_h || (opt_p==0) || ((opt_x && !opt_y) || (!opt_x && opt_y))) {
            fprintf(stderr, usage_msgs[PORTRXRATECTRL_MSG]);
            return 0;
        }
        if (opt_x) { /* Optional Parameter present ==> Set */
            ret_val = bcm_port_rate_ingress_set(unit,port,param1,param2);
        }
        else { /* No Optional Parameter ==> Get */
            ret_val = bcm_port_rate_ingress_get(unit,port,&param1,&param2);
            if (param1 == 0)
                printf("Port %d ingress rate control is disabled\n", port);
            else
                printf("Port %d ingress rate is %d kbps and burst size is %d kbits\n",port,param1,param2);
        }
    }
    else if (strcmp(name, "txratectrl") == 0) {

        if (opt_h || (opt_p==0) || ((opt_x && !opt_y) || (!opt_x && opt_y))) {
            fprintf(stderr, usage_msgs[PORTTXRATECTRL_MSG]);
            return 0;
        }
        if (opt_x) { /* Optional Parameter present ==> Set */
            int is_pkt_mode;
            priority = opt_q? priority: -1;
            if (priority >= BCM_COS_COUNT || port > IMP_PORT_ID) {
               fprintf(stderr, usage_msgs[PORTTXRATECTRL_MSG]);
               return 0;
            }

            /* If is_pkt_mode == 0, Byte mode shaper, otherwise Packet mode shaper */
            is_pkt_mode =  (!opt_T || Type == 0)? 0: 1;

            ret_val = bcm_port_rate_egress_set_X(unit,port,param1,param2, priority, is_pkt_mode);
        }
        else { /* No Optional Parameter ==> Get */
#if defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_4908) || defined(CHIP_63158)
            egress_shaper_stats_t stats;
            unsigned char q_shaper;
            priority = opt_q? priority: -1;  // Q shaper or port shaper
            printf("\t Egress Shaper Stats: Unit %d  Port %d ", unit, port);
            q_shaper = priority  >= 0;
            if ( !q_shaper) {
                printf("\n\n");
            } else {
                printf(" Queue %d\n\n", priority);

            }
            ret_val = bcm_port_rate_egress_get_X(unit, port,
                                   &param1,&param2, priority, (void *)&stats);
            if (ret_val == 0) {
                if (stats.egress_shaper_flags & SHAPER_RATE_PACKET_MODE) {
                    printf("\t\t Egress shaper Rate : %d pps\n", stats.egress_rate_cfg); 
                } else {
                    printf("\t\t Egress shaper Rate : %d kbps\n", stats.egress_rate_cfg); 
                }
                if (stats.egress_shaper_flags & SHAPER_RATE_PACKET_MODE) {
                    printf("\t\t Egress shaper Burst Size : %d packets\n", stats.egress_burst_sz_cfg);
                } else {
                    printf("\t\t Egress shaper Burst Size : %d Kbits\n", stats.egress_burst_sz_cfg); 
                }

                printf("\t\t Egress shaper Current Tokens (in Bucket): %d\n\n", stats.egress_cur_tokens);
                printf("\t\t Egress Shaper Flags: %s  %s %s\n \t\t\t %s  %s  %s\n",
                        stats.egress_shaper_flags & SHAPER_ENABLE? "\tSHAPER ENABLED": "\tSHAPER NOT ENABLED",
                        stats.egress_shaper_flags & SHAPER_RATE_PACKET_MODE? "\tPACKET MODE": "\tBYTE MODE",
                        stats.egress_shaper_flags & SHAPER_BLOCKING_MODE? "\tBLOCKING_MODE ": "",
                        /* Only port shaper (Not q shaper) would have IFG attribure */
                        q_shaper? "":
                        stats.egress_shaper_flags & SHAPER_INCLUDE_IFG? "\tINCLUDES IFG": "",
                        stats.egress_shaper_flags & SHAPER_OVF_FLAG? "\tSHAPER OVERFLOWED": "",
                        stats.egress_shaper_flags & SHAPER_INPF_FLAG? "\tSHAPER IN PROFILE": "" );
            } else {
                 printf("bcm_port_rate_egress_get() failed\n");
            }
#else
            ret_val = bcm_port_rate_egress_get_X(unit,port,&param1,&param2, -1, NULL);
            printf("Port %d egress rate is %d kbps and burst size is %d kbits\n",port,param1,param2);
#endif
        }
    }
#if defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_4908) || defined(CHIP_63158)
    else if (strcmp(name, "txratecfg") == 0) {
        if (opt_h || (opt_p == 0) || (opt_t == 0) || (opt_v == 0)) {
            fprintf(stderr, usage_msgs[PORTTXRATECFG_MSG]);
            return 0;
        }
        priority = opt_q? priority: -1;
        ret_val = bcm_port_shaper_cfg(unit, port, priority, type, val);
    }
#endif
    else if (strcmp(name, "softswitch") == 0) {
        if (opt_h) {
            fprintf(stderr, usage_msgs[SOFTSWITCH_MSG]);
            return 0;
        }

        if ( opt_o && opt_i ) {
            if (strcmp(operation, "enable") == 0) {
               bcm_enet_driver_enable_soft_switching_port(1, if_name);
            }
            else if (strcmp(operation, "disable") == 0) {
               bcm_enet_driver_enable_soft_switching_port(0, if_name);
            }
            else {
               fprintf(stderr, usage_msgs[SOFTSWITCH_MSG]);
               return 0;
            }
        }
        else if ( !opt_o && !opt_i ) {
            ret_val = bcm_enet_driver_get_soft_switching_status( &param1 );
            printf("Soft switching enabled port map is %x\n", param1);
        }
        else {
            fprintf(stderr, usage_msgs[SOFTSWITCH_MSG]);
            return 0;
        }
    }
    else if (strcmp(name, "hwstp") == 0) {
        if (opt_h) {
            fprintf(stderr, usage_msgs[HWSTP_MSG]);
            return 0;
        }

        if (opt_o && opt_i) {
            if (strcmp(operation, "enable") == 0) {
                ret_val = bcm_enet_driver_hw_stp_set(1, if_name);
            }
            else if (strcmp(operation, "disable") == 0) {
               ret_val = bcm_enet_driver_hw_stp_set(0, if_name);
            }
            else {
               fprintf(stderr, usage_msgs[HWSTP_MSG]);
               return 0;
            }
        }
        else if ( !opt_o && !opt_p ) {
           ret_val = bcm_enet_driver_get_hw_stp_status(&param1);
           printf("HW STP disabled port map: %x\n", param1);
        }
        else {
            fprintf(stderr, usage_msgs[HWSTP_MSG]);
            return 0;
        }
    }
    else if (strcmp(name, "mirror") == 0) {
        int enbl = 0, tx_port = -1, rx_port = -1;
        if (opt_h || (opt_o && (strcmp(operation, "enable") == 0) && ((!opt_p || !opt_x || !opt_y)))) {
            fprintf(stderr, usage_msgs[PORT_MIRROR_MSG]);
            return 0;
        }
        if (opt_o) { /* Optional Parameter present ==> Set */
            if (strcmp(operation, "enable") == 0) {
                enbl = 1;
            }

            if (opt_v)
                tx_port = val;

            if (opt_w) {
                if (opt_w != 2) {
                    fprintf(stderr, "-w requests argument\n");
                    fprintf(stderr, usage_msgs[PORT_MIRROR_MSG]);
                    return 0;
                }
                rx_port = param4;
            }

            ret_val = bcm_port_mirror_set(unit,enbl,port,param1,param2, param3, tx_port, rx_port);
        }
        else { /* No Optional Parameter ==> Get */
            ret_val = bcm_port_mirror_get(unit,&enbl,&port,&param1,&param2, &param3, &tx_port, &rx_port);
        }
        if (enbl) {
            printf("%s Switch Port Mirroring is Enabled\n", (unit?"Ext":"Int"));
            printf("Mirror Port            = %d\nIngress Mirror Port Map = 0x%02x\n"
                   "Egress Mirror Port Map = 0x%02x\nBlock no_mirror pkts    = %s\n"
                   "TX packet mirror port  = %d (Optional; Applicable only to Runner)\n"
                   "RX packet mirror port  = %d (Optional; Applicable only to Runner)\n",
                   port,param1,param2, (param3?"Yes":"No"), tx_port, rx_port);
        }
        else {
            printf("%s Switch Port Mirroring is disabled\n", (unit?"Ext":"Int"));
        }
    }
    else if (strcmp(name, "trunk") == 0) {
        int enbl = 0;
        if (opt_h ) {
            fprintf(stderr, usage_msgs[PORT_TRUNK_MSG]);
            return 0;
        }
        if (opt_o) { /* Optional Parameter present ==> Set */
            if (strcmp(operation, "sada") == 0) {
                param1 = 0;
            } else if (strcmp(operation, "da") == 0) {
                param1 = 1;
            } else if (strcmp(operation, "sa") == 0) {
                param1 = 2;
            } else {
                fprintf(stderr, usage_msgs[PORT_TRUNK_MSG]);
                return 0;
            }

            ret_val = bcm_port_trunk_set(unit,param1);
        }
        else { /* No Optional Parameter ==> Get */
            ret_val = bcm_port_trunk_get(unit,&enbl,&param1,&param2, &param3);
            if (!ret_val) {
                if (enbl) {
                    printf("%s Switch Port Trunking is Enabled\n", (unit?"Ext":"Int"));
                    printf("Hash Selection             = %d <%s>\n",param1,param1==0 ? "SA+DA" : (param1==1 ? "DA" :(param1==2 ? "SA" :"Invalid")));
                    printf("Trunk Group_0 Port Map = 0x%04x <Ports:",param2);
                    for (i = 0; param2 && i < 8; i++) {
                        if (param2 & (1<<i)) {
                            printf(" %d",i);
                            param2 &= ~(1<<i);
                        }
                    }
                    printf(">\n");
                    printf("Trunk Group_1 Port Map = 0x%04x <Ports:",param3);
                    for (i = 0; param3 && i < 8; i++) {
                        if (param3 & (1<<i)) {
                            printf(" %d",i);
                            param3 &= ~(1<<i);
                        }
                    }
                    printf(">\n");
                }
                else {
                    printf("%s Switch Port Mirroring is disabled\n", (unit?"Ext":"Int"));
                }
            }
        }
    }
#if defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_4908) || defined(CHIP_63158)
    else if (strcmp(name, "stormctrl") == 0) {

          if (!opt_p) {
              printf("Port(-p) should be specified \n");
              printf(usage_msgs[PORT_STORM_CTRL_MSG]);
              return 0;
          }
          if (opt_t && opt_x && opt_y)
          {              
              bcm_port_storm_ctrl_set(1, port, type, param1, param2);
          }
          else
          {
              bcm_port_storm_ctrl_get(1, port, &type, &param1, &param2);
              printf("pkt_type_mask=0x%x, rate=%d, bucket_size=%d\n\n", type, param1, param2);
              printf(usage_msgs[PORT_STORM_CTRL_MSG]);
          }
    }
#endif
    else if (strcmp(name, "oamindex") == 0) {
        if (opt_h || (opt_v == 0) ||(opt_t == 0)) {
            fprintf(stderr, usage_msgs[OAMIDX_MSG]);
            return 0;
        }
        if (type == OAM_MAP_SUB_TYPE_TO_RDPA_IF)
        {
            port = bcm_enet_map_oam_idx_to_rdpaif(val);
            printf("oam_index %d maps to rdpa_if=%d\n",val,port);
        }
        else
        {
            ret_val = bcm_enet_map_oam_idx_to_unit_port(val, &unit, &port);
            if (!ret_val)
            {
                printf("oam_index %d maps to unit=%d, port=%d\n",val,unit,port);
            }
        }
    }
    else {
        print_usage();
        return 0;
    }

close_n_ret:
    if (ret_val) {
        fprintf(stderr, "Failure\n");
    } else {
        printf("Success.\n");
    }
    return ret_val;
}

static int getThreConfMode(int unit)
{
    int val;

    bcm_switch_control_get(unit, bcmSwitchBufferControl, &val);
    return (val & (1 << bcmSwitchFcMode)) > 0;
}

#define ethswctl_control_priority_get_single(unit, port, priority, type, val) \
    ethswctl_control_priority_op_single(unit, port, priority, type, (void *)val, 1)
#define ethswctl_control_priority_set_single(unit, port, priority, type, val) \
    ethswctl_control_priority_op_single(unit, port, priority, type, (void *)&val, 0)

static int ethswctl_control_priority_op_single(int unit, int port, 
        int priority, int type, int *val, int get)
{
    int ret_val;

    switch (type) {
        case TOTAL_DROP_THRESHOLD:
            type = bcmSwitchTotalDropThreshold;
            break;
        case TOTAL_PAUSE_THRESHOLD:
            type = bcmSwitchTotalPauseThreshold;
            break;
        case TOTAL_HYST_THRESHOLD:
            type = bcmSwitchTotalHysteresisThreshold;
            break;
        case TXQ_HI_DROP_THRESHOLD:
            type = bcmSwitchTxQHiDropThreshold;
            break;
        case TXQ_HI_PAUSE_THRESHOLD:
            type = bcmSwitchTxQHiPauseThreshold;
            break;
        case TXQ_HI_HYST_THRESHOLD:
            type = bcmSwitchTxQHiHysteresisThreshold;
            break;
#if defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_4908) || defined(CHIP_63158)
        case TXQ_RSERVE_THRESHOLD:
            type = bcmSwitchTxQHiReserveThreshold;
            break;
        case TXQ_THRED_CONFIG_MODE:
            type = bcmSwitchTxQThresholdConfigMode;
            break;
        case GET_TOTAL_PORTS:
            type = bcmSwitchTotalPorts;
            break;
        case GET_LINK_UP_LAN_PORTS:
            type = bcmSwitchLinkUpLanPorts;
            break;
        case GET_LINK_UP_WAN_PORTS:
            type = bcmSwitchLinkUpWanPorts;
            break;
        case TXQ_MAX_STREAMS:
            type = bcmSwitchMaxStreams;
            break;
#else
        case TXQ_LO_DROP_THRESHOLD:
            type = bcmSwitchTxQLowDropThreshold;
            break;
#endif
        default:
            printf("Invalid Threshold type %d\n", type);
            return -1;
    }
#if defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_4908) || defined(CHIP_63158)
    if (get)
        ret_val = bcm_switch_control_priority_getX(unit, port, priority, type, (void *)val);
    else
        ret_val = bcm_switch_control_priority_setX(unit, port, priority, type, *val);
#else
    if (get)
        ret_val = bcm_switch_control_priority_get(unit, priority, type, (void *)val);
    else
        ret_val = bcm_switch_control_priority_set(unit, priority, type, *val);
#endif
    return ret_val;
}

static int get_switch_pmap(int unit)
{
    static unsigned int pmap[BP_MAX_ENET_MACS] = {-1,-1}; 
    int i8, i4;
    unsigned int u1 = 0, u2 = 0, u3 = 0, u4 = 0, u5 = 0, u6 = 0;

    if(pmap[unit] == -1) {
        pmap[unit] = 0;
        bcm_get_switch_info(unit, &u1, &u2, &u3, &i4, &u4, &u5, &u6, &pmap[unit], &i8);
    }

    return pmap[unit];
}

int ethswctl_control_priority_op(int unit, int port,
    int queue, int type, unsigned int *val, int *threConfMode, int *maxStr, int get)
{
    int ret_val = 0, p, q, t, pmap, threPortConfMode, totalPorts = 0, 
        lanUpPorts = 0, wanUpPorts = 0;

    pmap = get_switch_pmap(unit) | (1<<SF2_IMP0_PORT) | (1<<SF2_WAN_IMP1_PORT);
    threPortConfMode = getThreConfMode(unit);

    if (!threPortConfMode && port != -1 && port != SF2_IMP0_PORT) {
        fprintf(stderr, "Error: Global Flow Threshold mode; no port number other than IMP(8) should be specified\n"
                "    Or use ethswctl -c swflowctrl -t 0 -v 1 to set Port Flow Threshold Mode first.\n");
        return -1;
    }

    if (!get) { // For set 
        if (*threConfMode == -1) {
            ethswctl_control_priority_get_single(unit, 0, 0, TXQ_THRED_CONFIG_MODE, (void *)threConfMode);
        }

        if (*threConfMode == 0 && type != -1) {
            fprintf(stderr, "Error: Auto Threshold mode does not allow to set queue thresholds manually.\n"
                    "       Set the mode to manually.\n");
            return -1;
        }
        ret_val = ethswctl_control_priority_set_single(unit, 0, 0, TXQ_THRED_CONFIG_MODE, *threConfMode);
        ret_val = ethswctl_control_priority_set_single(unit, 0, 0, TXQ_MAX_STREAMS, *maxStr);

        if (type == -1) return ret_val;
    }
    else { // For get
        static char *threConfModeStr[ThreModeTotalCnt] = {"Dyanmic", "Static", "Manual"};
        ethswctl_control_priority_get_single(unit, 0, 0, TXQ_THRED_CONFIG_MODE, (void *)threConfMode);
        ethswctl_control_priority_get_single(unit, 0, 0, GET_TOTAL_PORTS, (void *)&totalPorts);
        ethswctl_control_priority_get_single(unit, 0, 0, GET_LINK_UP_LAN_PORTS, (void *)&lanUpPorts);
        ethswctl_control_priority_get_single(unit, 0, 0, GET_LINK_UP_WAN_PORTS, (void *)&wanUpPorts);
        ethswctl_control_priority_get_single(unit, 0, 0, TXQ_MAX_STREAMS, (void *)maxStr);
        printf("Current Thredshold Configuration Mode: %s\n", threConfModeStr[*threConfMode]);
        printf("Total Ports         : %d\n", totalPorts);
        printf("Link Up LAN Ports   : %d\n", lanUpPorts);
        printf("Link Up WAN Ports   : %d\n", wanUpPorts);
        printf("Max Stream Number   : %d\n", *maxStr);
    }

    printf("%s Flow Threshold Mode.\n", threPortConfMode? "Port": "Global");
    for( p = port==-1? 0: port; p <= SF2_IMP0_PORT; p++) {
        if((pmap & (1<<p)) == 0) {
            continue;
        }

        for(q = queue==-1? 0: queue; q < SF2_MAX_QUEUES; q++) {
            for(t = type==-1? 0: type; t < TXQ_MAX_TYPES; t++) {
                if (get) {
                    ret_val = ethswctl_control_priority_get_single(unit, p, q, t, val);
                }
                else {
                    ret_val = ethswctl_control_priority_set_single(unit, p, q, t, *val);
                }

                if (ret_val == 0) {
                    if ( p == SF2_IMP0_PORT) {
                        printf("IMP0, ");
                    }
                    else if(!threPortConfMode) {
                        printf("All Ports, ");
                    }
                    else {
                        printf("   Port %d, ", p);
                    }

                    printf("Queue %d, Type:%d-%-32s: 0x%04x (%d) %s\n",
                            q, t, buf_thresh_literals[t], (short)*val, (short)*val,
                            get? "": ": Succeeded");
                }
                if(type != -1) break;
            }
            if(queue != -1) break;
            if(q < 7) printf("--------------------------------------------------------------------------\n");
        }

        if (port != -1) break;
        if (port == -1 && !threPortConfMode && p < SF2_IMP0_PORT ) p = SF2_IMP0_PORT - 1;
        printf("==========================================================================\n");
    }
    return ret_val;
}

static int is_quemap_valid(int queWan, int queRemap)
{
    int i, q, wq = 0;

    for(i=0; i<8; i++)
    {
        q = (queRemap >> i*4) & 0xf;
        wq |= (1<<q);
    }
    return wq == queWan;
}

int ethswctl_quemap(int unit, int queWan, int queRemap)
{
    int rc = 0, curWan, curRemap;

    if (queWan == -1 || queRemap == -1)
    {
        rc = ethswctl_quemap_call(unit, &curWan, &curRemap, 0);
    }

    if (queWan == -1 && queRemap == -1)
    {
        printf(" WAN Queue Bit Map: 0x%01x, Ethernet Driver Queue Remapping Map: 0x%08x\n", curWan, curRemap);
        return rc;
    }
    else if (queWan == -1)
    {
        if (is_quemap_valid(curWan, queRemap) == 0)
        {
            fprintf(stderr, "Mismatching Current WAN queue map and Queue remapping:\n"
                            "Current Wan Queue: 0x%01x, New Queue Remapping: 0x%08x\n",
                            curWan, queRemap);
            return -1;
        }
    }
    else if (queRemap == -1)
    {
        if (is_quemap_valid(queWan, curRemap) == 0)
        {
            fprintf(stderr, "Mismatching of New WAN queue map and current Queue remapping:\n"
                            "New Wan Queue: 0x%01x, Current Queue Remapping: 0x%08x\n",
                            queWan, curRemap);
            return -1;
        }
    }
    else
    {
        if (is_quemap_valid(queWan, queRemap) == 0)
        {
            fprintf(stderr, "Mismatching of New WAN queue map and New Queue remapping:\n"
                            "New Wan Queue: 0x%01x, New Queue Remapping: 0x%08x\n",
                            queWan, queRemap);
            return -1;
        }
    }

    rc = ethswctl_quemap_call(unit, &queWan, &queRemap, 1);
    printf("WAN Queue Bit Map: 0x%01x, Ethernet Driver Queue Remapping Map: 0x%08x\n", curWan, queRemap);

    return rc;
}

int ethswctl_quemon_op(int unit, int port, int queue, int type, int *val)
{
    int ret_val = 0, p = 0, q = 0, t, pmap,
        queCache[QUE_MON_MAX_TYPES], portCache[QUE_MON_MAX_TYPES];    /* Cache data for Bit Map data */
    static char *fc_type_name[QUE_MON_MAX_TYPES] = {
        "QueCurCount",
        "QuePeakCount",
        "SysTotalPeakCount",
        "SysTotalCurCount",

        "PortPeakRxBuffer", 
        "QueFinalCongestedStatus", 
        "PortPauseHistory",
        "PortQuantumPauseHistory", 

        "PortRxBasedPauseHistory",
        "PortRxBufferErrorHitory",
        "QueCongestedStatus", 
        "QueTotalCongestedStatus"
    };

    static char *fc_que_stat_name[] = {
        "Below HW Reserved", "Above HW Reserved and Not In Pause", 
        "Above Hysteresis and In Pause", 
        "Above Drop"};

    enum {
        QueMonTypeSys,
        QueMonTypePort,
        QueMonTypePortBits,
        QueMonTypeQue,
        QueMonTypeQueBits,
    };

    static int queMonType[QUE_MON_MAX_TYPES] = {
        QueMonTypeQue, QueMonTypeQue, QueMonTypeSys, QueMonTypeSys,
        QueMonTypePort, QueMonTypeQueBits, QueMonTypePortBits, QueMonTypePortBits,
        QueMonTypePortBits, QueMonTypePortBits, QueMonTypeQueBits, QueMonTypeQueBits,
    };

    pmap = get_switch_pmap(unit) | (1<<SF2_IMP0_PORT) | (1<<SF2_WAN_IMP1_PORT);

    for(t = type==-1? 0: type; t < QUE_MON_MAX_TYPES; t++) {
        if (type == -1 && t == 0) printf("==========================================================================\n");
        if (queMonType[t] != QueMonTypeSys) continue;
        if((ret_val = ethswctl_quemon_get(unit, p, q, t, val)) != 0) continue;
        printf("Type:%d-%-32s: 0x%04x (%d)\n",
                t, fc_type_name[t], *val, *val);
        if(type != -1) break;
    }
    if (type == -1) printf("==========================================================================\n");
        
    for( p = port==-1? 0: port; p <= SF2_IMP0_PORT; p++) {
        if((pmap & (1<<p)) == 0) continue;

        printf("Port %d:\n", p);
        for(t = type==-1? 0: type; t < QUE_MON_MAX_TYPES; t++) {
            if (queMonType[t] == QueMonTypePort) {
                ret_val = ethswctl_quemon_get(unit, p, q, t, val);
                if(ret_val != 0) continue;
                printf(" Type:%d-%-32s: 0x%04x (%d)\n", t, fc_type_name[t], *val, *val);
            }
            else if (queMonType[t] == QueMonTypePortBits) {
                if (p != 0 || port != -1) {
                    ret_val = ethswctl_quemon_get(unit, p, q, t, &portCache[t]);
                    if(ret_val != 0) continue;
                }
                *val = (portCache[t] >> p) & 1;
                printf(" Type:%d-%-32s: %s\n", t, fc_type_name[t], *val? "Yes": "No");
            }
            else continue;

            if(type != -1) break;
        }
        printf("--------------------------------------------------------------------------\n");
        
        for(q = queue==-1? 0: queue; q < SF2_MAX_QUEUES; q++) {
            for(t = type==-1? 0: type; t < QUE_MON_MAX_TYPES; t++) {
                if (queMonType[t] == QueMonTypeQue) {
                    ret_val = ethswctl_quemon_get(unit, p, q, t, val);
                    if(ret_val != 0) continue;
                }
                else if (queMonType[t] == QueMonTypeQueBits) {
                    if (q == 0 || queue != -1) {
                        ret_val = ethswctl_quemon_get(unit, p, q, t, &queCache[t]);
                        if(ret_val != 0) continue;
                    }
                    *val = (queCache[t] >> (q*2)) & 3;
                }
                else continue;

                printf("Queue %d, Type:%2d-%-28s: 0x%x:(%d)", q, t, fc_type_name[t], *val, *val);
                if (queMonType[t] == QueMonTypeQueBits) printf(" - %s", fc_que_stat_name[*val]);
                printf("\n");
                if(type != -1) break;
            }
            if(queue != -1) break;
            if(q < 7) printf("--------------------------------------------------------------------------\n");
        }
        if (port != -1) break;
        printf("==========================================================================\n");
    }
    return 0;
}

#define VLAN_CTRL 1
static int ethswctl_control_get(int unit, int type, unsigned int *val)
{
    int ret_val = 0;

    ret_val = bcm_switch_control_get(unit, type, (int *)val);
    if (ret_val != 0) {
        printf("bcm_switch_control_get failed\n");
        return ret_val;
    }

    if (type == VLAN_CTRL) {
        if (*val == 0) {
            printf("802.1Q VLAN DISABLED\n");
        } else if ( *val == 1){
            printf("802.1Q VLAN ENABLED, Shared VLAN learning mode.\n");
        } else if ( *val == 2){
            printf("802.1Q VLAN ENABLED, Individual VLAN learning mode.\n");
        }
    } else {
        printf("The Buffer Control Val = 0x%04x\n", (unsigned short)(*val));
        if ( (*val) & BCM_SWITCH_TOTAL_DROP) {
            printf("    Total Drop is Enabled\n");
        } else {
            printf("    Total Drop is Disabled\n");
        }
        if ((*val) & BCM_SWITCH_TOTAL_PAUSE) {
            printf("    Total Pause is Enabled\n");
        } else {
            printf("    Total {Pause is Disabled\n");
        }
        if ((*val) & BCM_SWITCH_TXQ_DROP) {
            printf("    Tx Queue Drop is Enabled\n");
        } else {
            printf("    Tx Queue Drop is Disabled\n");
        }
        if ((*val) & BCM_SWITCH_TXQ_PAUSE) {
            printf("    Tx Queue Pause is Enabled\n");
        } else {
            printf("    Tx Queue Pause is Disabled\n");
        }
    }

    return ret_val;
}

int ethswctl_flow_control_get(int unit, int type, unsigned int *val)
{
    int ret_val = 0;

    ret_val = bcm_switch_control_get(unit, bcmSwitchBufferControl, (int *)val);
    if (ret_val != 0) {
        printf("bcm_switch_control_get failed\n");
        return ret_val;
    }
    printf("The Flow Control modes Val = 0x%04x\n", (unsigned short)(*val));
    printf("    %s Flow control\n", ((*val) & (1 << bcmSwitchFcMode))? "Port": "Global");
    printf("    %s based Flow control\n", ((*val) & (1 << bcmSwitchTxBasedFc))? "Tx": "Rx");
    printf("    Switch TxQ Pause is %s\n", ((*val) & (1 << bcmSwitchTxQpauseEn))?
                                                 "Enabled": "Disabled");
    printf("    Switch TxQ Pause Imp0 is %s\n", ((*val) & (1 << bcmSwitchTxQpauseEnImp0))?
                                                 "Enabled": "Disabled");
    printf("    Switch TxQ Drop is %s\n", ((*val) & (1 << bcmSwitchTxQdropEn))?
                                                 "Enabled": "Disabled");
    printf("    Switch Tx Total Pause is %s\n", ((*val) & (1 << bcmSwitchTxTotPauseEn))?
                                                 "Enabled": "Disabled");
    printf("    Switch Tx Total Pause Imp0 is %s\n", ((*val) & (1 << bcmSwitchTxTotPauseEnImp0))?
                                                 "Enabled": "Disabled");
    printf("    Switch Tx Total Drop is %s\n", ((*val) & (1 << bcmSwitchTxTotdropEn))?
                                                 "Enabled": "Disabled");
    printf("    Queue based Pause is %s\n", ((*val) & (1 << bcmSwitchQbasedpauseEn))?
                                                 "Enabled": "Disabled");
    return 0;
}

int ethswctl_flow_control_set(int unit, int port, int type, unsigned int val)
{

    // type is  bcm_switch_fc_t
    type =  ((port == 8) && (type == bcmSwitchTxQpauseEn))? bcmSwitchTxQpauseEnImp0: type;
    type =  ((port == 8) && (type == bcmSwitchTxTotPauseEn))? bcmSwitchTxTotPauseEnImp0: type;
    return bcm_switch_control_setX(unit, bcmSwitchBufferControl, (bcm_switch_fc_t)type, (int)val);
}

int ethswctl_acb_cfg_get(int unit, int queue, int type)
{
    int ret_val = 0;
    acb_q_params_t val;
    ret_val = bcm_acb_cfg_get(unit, queue, type, (void *)&val);
    if (ret_val != 0) {
        printf("bcm_acb_cfg_get() failed\n");
        return ret_val;
    }
    switch (type) {
        case acb_en:
            printf("ACB is %s\n", val.acb_en? "Enabled": "Disabled");
            break;
#if defined(ACB_ALGORITHM2)
        case acb_eop_delay:
            printf("ACB EOP Delay is %d\n", val.eop_delay);
            break;
        case acb_flush:
            printf("ACB Queue flush is %d\n", val.flush);
            break;
        case acb_algorithm:
            printf("ACB Algorithm is %d\n", val.algorithm);
            break;
#endif
        case acb_tot_xon_hyst:
            printf("ACB Total Xon Hysteresis is %d\n", val.total_xon_hyst);
            break;
        case acb_xon_hyst:
            printf("ACB Xon Hysteresis is %d\n", val.xon_hyst);
            break;
        case acb_q_pessimistic_mode:
            printf("ACB q pessimistic mode is %s\n", val.acb_queue_config.pessimistic_mode? "Enabled": "Disabled");
            break;
        case acb_q_total_xon_en:
            printf("ACB q Total Xon is %s\n", val.acb_queue_config.total_xon_en? "Enabled": "Disabled");
            break;
        case acb_q_xon_en:
            printf("ACB q Xon is %s\n", val.acb_queue_config.xon_en? "Enabled": "Disabled");
            break;
        case acb_q_total_xoff_en:
            printf("ACB q Total Xoff is %s\n", val.acb_queue_config.total_xoff_en? "Enabled": "Disabled");
            break;
        case acb_q_pkt_len:
            printf("ACB q pkt len is %d Buffers\n", val.acb_queue_config.pkt_len);
            break;
        case acb_q_tot_xoff_thresh:
            printf("ACB q Total Xoff Threshold is %d Buffers\n", val.acb_queue_config.total_xoff_threshold);
            break;
        case acb_q_xoff_thresh:
            printf("ACB q Xoff Threshold is %d Buffers\n", val.acb_queue_config.xoff_threshold);
            break;
#if defined(ACB_ALGORITHM2)
        case acb_q_pkts_in_flight:
            printf("ACB q in flight is %d\n", val.pkts_in_flight);
            break;
#endif
        case acb_parms_all:
            printf("ACB is %s\n", val.acb_en? "Enabled": "Disabled");
            printf("ACB Total Xon Hysteresis is %d\n", val.total_xon_hyst);
            printf("ACB Xon Hysteresis is %d\n", val.xon_hyst);
            printf("ACB q %d, pessimistic mode is %s\n", queue,
                               val.acb_queue_config.pessimistic_mode? "Enabled": "Disabled");
            printf("ACB q %d, Total Xon is %s\n", queue,
                               val.acb_queue_config.total_xon_en? "Enabled": "Disabled");
            printf("ACB q %d, Xon is %s\n", queue,
                               val.acb_queue_config.xon_en? "Enabled": "Disabled");
            printf("ACB q %d, Total Xoff is %s\n", queue,
                               val.acb_queue_config.total_xoff_en? "Enabled": "Disabled");
            printf("ACB q %d, pkt len is %d Buffers\n", queue,
                               val.acb_queue_config.pkt_len);
            printf("ACB q %d, Total Xoff Threshold is %d Buffers\n", queue,
                               val.acb_queue_config.total_xoff_threshold);
            printf("ACB q %d, Xoff Threshold is %d Buffers\n", queue,
                               val.acb_queue_config.xoff_threshold);
#if defined(ACB_ALGORITHM2)
            printf("ACB EOP Delay is %d\n", val.eop_delay);
            printf("ACB Queue flush is %d\n", val.flush);
            printf("ACB Algorithm is %d\n", val.algorithm);
            printf("ACB q in flight is %d\n", val.pkts_in_flight);
#endif
            break;
    }
    return 0;
}

/* Set ENET Driver WRR Scheduling Parameters */
int ethswctl_wrr_param_set(int unit, int type, unsigned int val)
{
    int ret_val = 0, max_pkts_per_iter;
    int weights[4];

    ret_val = bcm_enet_driver_wrr_weights_get(unit, &max_pkts_per_iter, weights);
    if (ret_val == 0) {
        if (type < WRR_MAX_PKTS_PER_RND || type > WRR_CH3_WEIGHT) {
            printf("Invalid Type\n");
            return -1;
        }
        if (type == WRR_MAX_PKTS_PER_RND) {
            max_pkts_per_iter = (int)val;
        } else {
            if (val > MAX_WRR_WEIGHTS) {
                printf("Invalid WRR Weight Value\n");
                return -1;
            }
            weights[type - 1] = (int)val;
        }
        ret_val = bcm_enet_driver_wrr_weights_set(unit, max_pkts_per_iter, weights);
    }

    return ret_val;
}


/* Get ENET Driver WRR Scheduling Parameters */
int ethswctl_wrr_param_get(int unit)
{
    int ret_val = 0, max_pkts_per_iter;
    int weights[BCM_COS_COUNT], weight_pkts[BCM_COS_COUNT]; 
    int rx_queues, total_weights=0;
    int i;

    memset(weights, 0, sizeof(weights));
    ret_val = bcm_enet_driver_wrr_weights_get2(unit, &max_pkts_per_iter, weights, weight_pkts, &rx_queues);

    if (ret_val)
        return ret_val;

    for(i=0; i<rx_queues; i++) total_weights += weights[i];
    printf("Maximum Packets per round = %4d\n", max_pkts_per_iter);
    printf("Total Active Queues = %d; Total Weights: %d\n", rx_queues, total_weights);
    for(i=0; i<rx_queues; i++)
        printf("Queue %d: Weights = %d = %d Packets\n", i, weights[i], weight_pkts[i]);

    return ret_val;
}


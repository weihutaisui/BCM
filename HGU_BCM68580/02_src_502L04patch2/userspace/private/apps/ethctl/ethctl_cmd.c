/***********************************************************************
 *
 *  Copyright (c) 2004-2010  Broadcom Corporation
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
 * <:copyright-BRCM:2004:proprietary:standard
 *
 *    Copyright (c) 2004 Broadcom Corporation
 *    All Rights Reserved
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its
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
 * :>
 *
************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <errno.h>
#include <asm/param.h>
#include <fcntl.h>
typedef unsigned short u16;
#include <linux/mii.h>
#include <linux/if_vlan.h>
#include <linux/sockios.h>
#include "bcm/bcmswapitypes.h"
#include "ethctl.h"
#include "boardparms.h"
#include "bcmnet.h"
#include "ethctl_api.h"

struct ethctl_data ethctl;
static int parse_media_options(char *option)
{
    int mode = -1;

    if (strcmp(option, "auto") == 0) {
        mode = MEDIA_TYPE_AUTO;
    } else if (strcmp(option, "100FD") == 0) {
        mode = MEDIA_TYPE_100M_FD;
    } else if (strcmp(option, "100HD") == 0) {
        mode = MEDIA_TYPE_100M_HD;
    } else if (strcmp(option, "10FD") == 0) {
        mode = MEDIA_TYPE_10M_FD;
    } else if (strcmp(option, "10HD") == 0) {
        mode = MEDIA_TYPE_10M_HD;
    } else if (strcmp(option, "1000FD") == 0) {
        mode = MEDIA_TYPE_1000M_FD;
    } else if (strcmp(option, "1000HD") == 0) {
        mode = MEDIA_TYPE_1000M_HD;
    } else if (strcmp(option, "2500FD") == 0) {
        mode = MEDIA_TYPE_2500M_FD;
    } else if (strcmp(option, "10000FD") == 0) {
        mode = MEDIA_TYPE_10000M_FD;
    }
    return mode;
}

static char *print_speed(int speed)
{
    static char buf[32];
    if (speed == 2500) return "2.5G";
    if (speed >= 1000) sprintf(buf, "%dG", speed/1000);
    else sprintf(buf, "%dM", speed);
    return buf;
}

static char *phy_cap_to_string(int phyCap)
{
    static char buf[256];
    char *b = buf;
    int middle = 0;
    buf[0] = 0;
    if (phyCap & PHY_CFG_10000FD) b += sprintf(b, "%s10GFD", middle++? "|":"");
    if (phyCap & PHY_CFG_5000FD) b += sprintf(b, "%s5GFD", middle++? "|":"");
    if (phyCap & PHY_CFG_2500FD) b += sprintf(b, "%s2.5GFD", middle++? "|":"");
    if (phyCap & PHY_CFG_1000FD) b += sprintf(b, "%s1GFD", middle++? "|":"");
    if (phyCap & PHY_CFG_1000HD) b += sprintf(b, "%s1GHD", middle++?"|":"");
    if (phyCap & PHY_CFG_100FD) b += sprintf(b, "%s100MFD", middle++?"|":"");
    if (phyCap & PHY_CFG_100HD) b += sprintf(b, "%s100MHD", middle++?"|":"");
    if (phyCap & PHY_CFG_10FD) b += sprintf(b, "%s10MFD", middle++?"|":"");
    if (phyCap & PHY_CFG_10HD) b += sprintf(b, "%s10MHD", middle++?"|":"");
    return buf;
}

static void show_speed_setting(int skfd, struct ifreq *ifr, int phy_id, int sub_port)
{
    struct ethswctl_data _ifdata, *ifdata = &_ifdata;
    char *anStr = (ETHCTL_GET_FLAG_FROM_PHYID(phy_id) & ETHCTL_FLAG_ACCESS_SERDES)? 
        "Auto Detection of Serdes": "Auto-Negotiation";

    ifr->ifr_data = ifdata;
    get_link_speed(skfd, ifr, phy_id, sub_port);
    if (ifdata->cfgSpeed == 0)
        printf("    %s: Enabled\n", anStr);
    else
        printf("    %s: Disabled; Configured Speed: %s; Duplex: %s\n", 
            anStr, print_speed(ifdata->cfgSpeed), ifdata->cfgDuplex?"FD":"HD");

    printf("    PHY Capabilities: %s\n", phy_cap_to_string(ifdata->phyCap));

    if (ifdata->speed == 0) {
        printf("    Link is Down.\n");
    }
    else {
        printf("    Link is Up at Speed: %s, Duplex: %s\n",
                print_speed(ifdata->speed), ifdata->duplex? "FD":"HD");
    }
}

static int et_cmd_media_type_op(int skfd, struct ifreq *ifr, cmd_t *cmd, char** argv)
{
    int phy_id = 0, sub_port = -1, err;
    int set = 0;
    int mode = 0;;
    int val = 0;
    int nway_advert, gig_ctrl, gig_cap = 0;
    struct ethswctl_data ifdata;

    for(argv += 3; *argv; argv++) {
        if (!strcmp(*argv, "port") && *(++argv))
            sub_port = strtol(*argv, NULL, 0);
        else {
            if ((mode = parse_media_options(*argv)) < 0)
                goto error;
            set = 1;
        }
    }

    if ((phy_id = et_get_phyid(skfd, ifr, sub_port)) == -1)
        goto error;

    if (set) {
        ifr->ifr_data = &ifdata;
        if(ETHCTL_GET_FLAG_FROM_PHYID(phy_id) & ETHCTL_FLAG_ACCESS_SERDES) {
            switch (mode) {
                case MEDIA_TYPE_AUTO:
                    ifdata.speed = 0;
                    ifdata.duplex = 1;
                    break;
                case MEDIA_TYPE_100M_FD:
                    ifdata.speed = 100;
                    ifdata.duplex = 1;
                    break;
                case MEDIA_TYPE_100M_HD:
                    ifdata.speed = 100;
                    ifdata.duplex = 0;
                    break;
                case MEDIA_TYPE_10M_FD:
                    ifdata.speed = 10;
                    ifdata.duplex = 1;
                    break;
                case MEDIA_TYPE_10M_HD:
                    ifdata.speed = 10;
                    ifdata.duplex = 0;
                    break;
                case MEDIA_TYPE_1000M_FD:
                    ifdata.speed = 1000;
                    ifdata.duplex = 1;
                    break;
                case MEDIA_TYPE_2500M_FD:
                    ifdata.speed = 2500;
                    ifdata.duplex = 1;
                    break;                                
                case MEDIA_TYPE_10000M_FD:
                    ifdata.speed = 10000;
                    ifdata.duplex = 1;
                    break;                                
                default:
                    fprintf(stderr, "Illegal mode: %d\n", mode);
                    return -1;
            }

            ifdata.op = ETHSWPHYMODE;
            ifdata.type = TYPE_SET;
            ifdata.addressing_flag = ETHSW_ADDRESSING_DEV;
            if (sub_port != -1) {
                ifdata.sub_unit = -1; /* Set sub_unit to -1 so that main unit of dev will be used */
                ifdata.sub_port = sub_port;
                ifdata.addressing_flag |= ETHSW_ADDRESSING_SUBPORT;
            }
            if((err = ioctl(skfd, SIOCETHSWCTLOPS, ifr))) {
                fprintf(stderr, "ioctl command return error %d!\n", err);
                return -1;
            }
        } else {
            switch (mode) {
                case MEDIA_TYPE_AUTO:
                    val = BMCR_ANENABLE | BMCR_ANRESTART;
                    break;
                case MEDIA_TYPE_100M_FD:
                    val = BMCR_SPEED100 | BMCR_FULLDPLX;
                    break;
                case MEDIA_TYPE_100M_HD:
                    val = BMCR_SPEED100;
                    break;
                case MEDIA_TYPE_10M_FD:
                    val = BMCR_FULLDPLX;
                    break;
                case MEDIA_TYPE_10M_HD:
                    val = 0;
                    break;
                case MEDIA_TYPE_1000M_FD:
                    val = BMCR_SPEED1000| BMCR_FULLDPLX | BMCR_ANENABLE | BMCR_ANRESTART;
                    gig_cap = ADVERTISE_1000HALF;
                    break;
                case MEDIA_TYPE_1000M_HD:
                    val = BMCR_SPEED1000 | BMCR_ANENABLE | BMCR_ANRESTART;
                    gig_cap = ADVERTISE_1000FULL;                
                    break;                                
                case MEDIA_TYPE_2500M_FD:
                    val = BMCR_SPEED2500| BMCR_FULLDPLX;
                    break;                                
                case MEDIA_TYPE_10000M_FD:
                    val = BMCR_SPEED10000| BMCR_FULLDPLX;
                    break;                                
            }

            nway_advert = mdio_read(skfd, ifr, phy_id, MII_ADVERTISE);
            gig_ctrl = mdio_read(skfd, ifr, phy_id, MII_CTRL1000);
            gig_ctrl |= (ADVERTISE_1000FULL | ADVERTISE_1000HALF);

            if (mode == MEDIA_TYPE_1000M_FD || mode == MEDIA_TYPE_1000M_HD) {
                nway_advert &= ~(ADVERTISE_100BASE4 |
                        ADVERTISE_100FULL |
                        ADVERTISE_100HALF |
                        ADVERTISE_10FULL |
                        ADVERTISE_10HALF );
                gig_ctrl &= ~gig_cap;
            }
            else {
                nway_advert |= (ADVERTISE_100BASE4 |
                        ADVERTISE_100FULL |
                        ADVERTISE_100HALF |
                        ADVERTISE_10FULL |
                        ADVERTISE_10HALF );
            }

            mdio_write(skfd, ifr, phy_id, MII_ADVERTISE, nway_advert);
            mdio_write(skfd, ifr, phy_id, MII_CTRL1000, gig_ctrl);
            mdio_write(skfd, ifr, phy_id, MII_BMCR, val);
            if (mode == MEDIA_TYPE_AUTO)
                sleep(2);

        }
    }
    show_speed_setting(skfd, ifr, phy_id, sub_port);
    return 0;
error:
    command_help(cmd);
    return -1;
}

static int et_cmd_phy_reset_op(int skfd, struct ifreq *ifr, cmd_t *cmd, char** argv)
{
    int phy_id = 0, sub_port = -1;

    if (argv[3] && !strcmp(argv[3], "port") && argv[4])
        sub_port = strtol(argv[4], NULL, 0);

    if ((phy_id = et_get_phyid(skfd, ifr, sub_port)) == -1) {
        command_help(cmd);
        return -1;
    }

    mdio_write(skfd, ifr, phy_id, MII_BMCR, BMCR_RESET);
    sleep(2);
    show_speed_setting(skfd, ifr, phy_id, sub_port);
    return 0;
}

static int et_cmd_mii_op(int skfd, struct ifreq *ifr, cmd_t *cmd, char** argv)
{
    int phy_id;
    int set = 0;
    int val = 0;
    int reg = -1;
    int sub_port = -1;

    for(argv += 3; *argv; argv++) {
        if (!strcmp(*argv, "port") && *(++argv))
            sub_port = strtol(*argv, NULL, 0);
        else {
            if(reg == -1) {
                reg = strtoul(*argv, NULL, 0);
                if ((reg < 0) || (reg > 31))
                    goto error;
            }
            else {
                val = strtoul(*argv, NULL, 0);
                set = 1;
            }
        }
    }

    if ((phy_id = et_get_phyid(skfd, ifr, sub_port)) == -1)
        goto error;

    if (set)
        mdio_write(skfd, ifr, phy_id, reg, val);
    val = mdio_read(skfd, ifr, phy_id, reg);
    printf("mii (phy addr 0x%x) register %d is 0x%04x\n", phy_id, reg, val);
    return 0;
error:
    command_help(cmd);
    return -1;
}

static int et_cmd_phy_op(int skfd, struct ifreq *ifr, cmd_t *cmd, char** argv)
{
    int err, phy_id = 0, phy_flag = 0, four_byte;
    unsigned int set = 0, get = 1, val = 0, reg = 0, r, i, dump = 0;

    argv = argv+2;;
    if (*argv) {
        if (strcmp(*argv, "ext") == 0) {
            phy_flag = ETHCTL_FLAG_ACCESS_EXT_PHY;
        } else if (strcmp(*argv, "int") == 0) {
            phy_flag = ETHCTL_FLAG_ACCESS_INT_PHY;
        } else if (strcmp(*argv, "extsw") == 0) { // phy connected to external switch
            phy_flag = ETHCTL_FLAG_ACCESS_EXTSW_PHY;
        } else if (strcmp(*argv, "i2c") == 0) { // phy connected through I2C bus
            phy_flag = ETHCTL_FLAG_ACCESS_I2C_PHY;
        } else if (strcmp(*argv, "10gserdes") == 0) { // phy connected through I2C bus
            phy_flag = ETHCTL_FLAG_ACCESS_10GSERDES;
        } else if (strcmp(*argv, "10gpcs") == 0) { // phy connected through I2C bus
            phy_flag = ETHCTL_FLAG_ACCESS_10GPCS;
        } else if (strcmp(*argv, "serdespower") == 0) { // Serdes power saving mode
            phy_flag = ETHCTL_FLAG_ACCESS_SERDES_POWER_MODE;
        } else if (strcmp(*argv, "ext32") == 0) { // Extended 32bit register access.
            phy_flag = ETHCTL_FLAG_ACCESS_32BIT|ETHCTL_FLAG_ACCESS_EXT_PHY;
        } else {
            goto print_error_and_return;
        }
        argv++;
    } else {
        goto print_error_and_return;
    }

    if (*argv) {
        /* parse phy address */
        phy_id = strtol(*argv, NULL, 0);
        if ((phy_id < 0) || (phy_id > 31)) {
            fprintf(stderr, "Invalid Phy Address 0x%02x\n", phy_id);
            command_help(cmd);
            return -1;
        }
        argv++;
    } else {
        goto print_error_and_return;
    }

    if (*argv) {
        reg = strtoul(*argv, NULL, 0);

        if(phy_flag == ETHCTL_FLAG_ACCESS_SERDES_POWER_MODE)
        {
            if (reg < 0 || reg > 2)
            {
                fprintf(stderr, "Invalid Serdes Power Mode%02x\n", reg);
                command_help(cmd);
                return -1;
            }
            set = 1;
        }
        argv++;
    } else if(phy_flag != ETHCTL_FLAG_ACCESS_SERDES_POWER_MODE) {
        goto print_error_and_return;
    }

    if (*argv) {
        /* parse register setting value */
        val = strtoul(*argv, NULL, 0);
        set = 1;
        argv++;
    }

    if (*argv && phy_flag != ETHCTL_FLAG_ACCESS_SERDES_POWER_MODE) {
        /* parse no read back flag */
        if (strcmp(*argv, "no_read_back") == 0) {
            get = 0;
        } else if (strcmp(*argv, "-d") == 0) {
            dump = 1;
            get = 0;
            set = 0;
        } else {
            fprintf(stderr, "Invalid command %s, expecting no_read_back.\n", *argv);
            command_help(cmd);
            return -1;
        }
        argv++;
    }

    ethctl.phy_addr = phy_id;
    ethctl.phy_reg = reg;
    ethctl.flags = phy_flag;

    if (set) {
        ethctl.op = ETHSETMIIREG;
        ethctl.val = val;
        ifr->ifr_data = (void *)&ethctl;
        err = ioctl(skfd, SIOCETHCTLOPS, ifr);
        if (ethctl.ret_val || err) {
            fprintf(stderr, "command return error!\n");
            return err;
        }
        else
            printf("PHY register set successfully\n");
    }

    four_byte = ((reg > 0x200000) || (phy_flag & ETHCTL_FLAG_ACCESS_32BIT)) && !(phy_flag & (ETHCTL_FLAG_ACCESS_10GSERDES|ETHCTL_FLAG_ACCESS_10GPCS));
    if (get || dump) {
        for(r = reg, i=0; (dump && r < val) || (get && r == reg); r+=four_byte?4:1, i++)
        {
            ethctl.op = ETHGETMIIREG;
            ethctl.phy_reg = r;
            ifr->ifr_data = (void *)&ethctl;
            err = ioctl(skfd, SIOCETHCTLOPS, ifr);
            if (ethctl.ret_val || err) {
                fprintf(stderr, "command return error!\n");
                return err;
            }

            if(phy_flag == ETHCTL_FLAG_ACCESS_SERDES_POWER_MODE) {
                static char *mode[] = {"No Power Saving", "Basic Power Saving", "Device Forced Off"};
                printf("Serdes power saving mode: %d-\"%s\"\n\n",
                        ethctl.val, mode[ethctl.val]);
                break;
            }

            if (get) {
                if (reg < 0xffff)
                    printf("mii register 0x%04x is ", reg);
                else
                    printf("mii register 0x%08x is ", reg);

                if (four_byte)
                    printf("0x%08x\n", ethctl.val);
                else
                    printf("0x%04x\n", ethctl.val);
            }
            else {  // dump
                if ((i % 8) == 0) {
                    printf("\n");
                    printf("  %04x: ", r);
                }
                else if ((i%4) == 0) {
                    printf("  ");
                }
                if (four_byte)
                    printf(" %08x", ethctl.val);
                else
                    printf("  %04x", ethctl.val);
            }
        }
        printf("\n");
    }

    return err;

print_error_and_return:
    fprintf(stderr, "Invalid syntax\n");
    command_help(cmd);
    return -1;
}

static int et_cmd_phy_power_op(int skfd, struct ifreq *ifr, cmd_t *cmd, char** argv)
{
    int err = -1;

    if (!argv[3]) {
        command_help(cmd);
        return 1;
    }

    if (strcmp(argv[3], "up") == 0) {
        ethctl.op = ETHSETSPOWERUP;
    } else if (strcmp(argv[3], "down") == 0) {
        ethctl.op = ETHSETSPOWERDOWN;
    } else {
        command_help(cmd);
        return 1;
    }

    ifr->ifr_data = (void *)&ethctl;
    err = ioctl(skfd, SIOCETHCTLOPS, ifr);

    if (err) {
        fprintf(stderr, "command return error!\n");
        return err;
    } else {
        printf("Powered %s \n", argv[3]);
    }

    return err;
}

static int et_cmd_vport_enable(int skfd, struct ifreq *ifr)
{
    int err = 0;

    err = ioctl(skfd, SIOCGENABLEVLAN, ifr);

    return err;
}

static int et_cmd_vport_disable(int skfd, struct ifreq *ifr)
{
    int err = 0;

    err = ioctl(skfd, SIOCGDISABLEVLAN, ifr);

    return err;
}

static int et_cmd_vport_query(int skfd, struct ifreq *ifr)
{
    int err = 0;
    int ports = 0;

    ifr->ifr_data = (char*)&ports;
    err = ioctl(skfd, SIOCGQUERYNUMVLANPORTS, ifr);
    if (err == 0)
        printf("%u\n", ports);

    return err;
}

static int et_cmd_vport_op(int skfd, struct ifreq *ifr, cmd_t *cmd, char** argv)
{
    int err = -1;
    char *arg;

    arg = argv[3];
    if (strcmp(arg, "enable") == 0) {
        err = et_cmd_vport_enable(skfd, ifr);
    } else if (strcmp(arg, "disable") == 0) {
        err = et_cmd_vport_disable(skfd, ifr);
    } else if (strcmp(arg, "query") == 0) {
        err = et_cmd_vport_query(skfd, ifr);
    } else {
        command_help(cmd);
        return 1;
    }
    if (err)
        fprintf(stderr, "command return error!\n");

    return err;
}

#define MAX_NUM_CHANNELS 4
/* Set/Get number of Tx IUDMA channels */
static int et_cmd_tx_iudma_op(int skfd, struct ifreq *ifr, cmd_t *cmd,
                              char** argv)
{
    int err = -1;

    if (argv[2]) {
        ethctl.num_channels = (int) strtol(argv[2], NULL, 0);
        if ((ethctl.num_channels < 1) ||
            (ethctl.num_channels > MAX_NUM_CHANNELS)) {
            fprintf(stderr, "Invalid number of Tx IUDMA Channels \n");
        }
        ethctl.op = ETHSETNUMTXDMACHANNELS;
    } else {
        ethctl.op = ETHGETNUMTXDMACHANNELS;
    }

    ifr->ifr_data = (void *)&ethctl;
    err = ioctl(skfd, SIOCETHCTLOPS, ifr);

    if (err) {
        fprintf(stderr, "command return error!\n");
        return err;
    } else if (ethctl.op == ETHGETNUMTXDMACHANNELS) {
        printf("The number of Tx DMA channels: %d\n",
                ethctl.ret_val);
    }

    return err;
}

/* Set/Get number of Rx IUDMA channels */
static int et_cmd_rx_iudma_op(int skfd, struct ifreq *ifr, cmd_t *cmd,
                              char** argv)
{
    int err = -1;

    if (argv[2]) {
        ethctl.num_channels = (int) strtol(argv[2], NULL, 0);
        if ((ethctl.num_channels < 1) ||
            (ethctl.num_channels > MAX_NUM_CHANNELS)) {
            fprintf(stderr, "Invalid number of Rx IUDMA Channels \n");
        }
        ethctl.op = ETHSETNUMRXDMACHANNELS;
    } else {
        ethctl.op = ETHGETNUMRXDMACHANNELS;
    }

    ifr->ifr_data = (void *)&ethctl;
    err = ioctl(skfd, SIOCETHCTLOPS, ifr);

    if (err) {
        fprintf(stderr, "command return error!\n");
        return err;
    } else if (ethctl.op == ETHGETNUMRXDMACHANNELS) {
        printf("The number of Rx DMA channels: %d\n",
                ethctl.ret_val);
    }

    return err;
}

/* Display software stats */
static int et_cmd_stats_op(int skfd, struct ifreq *ifr, cmd_t *cmd,
                              char** argv)
{
    int err = -1;

    ethctl.op = ETHGETSOFTWARESTATS;

    ifr->ifr_data = (void *)&ethctl;
    err = ioctl(skfd, SIOCETHCTLOPS, ifr);

    if (err) {
        fprintf(stderr, "command return error!\n");
        return err;
    }

    return err;
}

/* Enable/Disable ethernet@wirespeed */
static int et_cmd_ethernet_at_wirespeed_op(int skfd, struct ifreq *ifr, cmd_t *cmd,
                              char** argv)
{
    int phy_id = 0;
    int gig_ctrl, v16, ctrl, sub_port = -1;

    if (!argv[3]) {
        command_help(cmd);
        return 1;
    }

    if (argv[4] && !strcmp(argv[4], "port") && argv[4])
        sub_port = strtol(argv[5], NULL, 0);

    if ((phy_id = et_get_phyid(skfd, ifr, sub_port)) == -1) {
        return 1;
    }

    if(ETHCTL_GET_FLAG_FROM_PHYID(phy_id) & ETHCTL_FLAG_ACCESS_SERDES) {
        fprintf(stderr, "ethernet@wirespeed is not supported on SERDES interface\n");
        return 1;
    }

    gig_ctrl = mdio_read(skfd, ifr, phy_id, MII_CTRL1000);

    // check ethernet@wirspeed only for PHY support 1G
    if(!(gig_ctrl & ADVERTISE_1000FULL || gig_ctrl & ADVERTISE_1000HALF)) {
        fprintf(stderr, "ethernet@wirespeed is not supported on 10/100Mbps.\n");
        return 0;
    }

    // read current setting
    mdio_write(skfd, ifr, phy_id, 0x18, 0x7007);
    v16 = mdio_read(skfd, ifr, phy_id, 0x18);

    if (strcmp(argv[3], "enable") == 0) {
        v16 = v16 | 0x8010; // set bit15 for write, bit4 for ethernet@wirespeed
        mdio_write(skfd, ifr, phy_id, 0x18, v16);

        // Restart AN
        ctrl = mdio_read(skfd, ifr, phy_id, MII_BMCR);
        ctrl = ctrl | (BMCR_ANENABLE | BMCR_ANRESTART);

        mdio_write(skfd, ifr, phy_id, MII_BMCR, ctrl);
    } else if (strcmp(argv[3], "disable") == 0) {
        v16 = (v16 & 0xffef) | 0x8000; // set bit15 for write, clear bit4 for ethernet@wirespeed
        mdio_write(skfd, ifr, phy_id, 0x18, v16);
    }
    if (v16 & 0x0010)
        fprintf(stderr, "ethernet@wirespeed is enabled\n");
    else
        fprintf(stderr, "ethernet@wirespeed is disabled\n");
    return 0;
}

static const struct command commands[] = {
    { 0, "media-type", et_cmd_media_type_op,
      ": Set/Get media type\n"
      "  ethctl <interface> media-type [option] [port <sub_port#> ]\n"
      "    [option]: auto - auto select\n"
      "              10000FD - 10Gb, Full Duplex\n"
      "              2500FD - 2.5Gb, Full Duplex\n"
      "              1000FD - 1000Mb, Full Duplex\n"
      "              1000HD - 1000Mb, Half Duplex\n"
      "              100FD - 100Mb, Full Duplex\n"
      "              100HD - 100Mb, Half Duplex\n"
      "              10FD  - 10Mb,  Full Duplex\n"
      "              10HD  - 10Mb,  Half Duplex\n"
      "    [port <sub_port#>]: required if <interface> has Crossbar or Trunk port underneath\n"
    },
    { 0, "phy-reset", et_cmd_phy_reset_op,
      ": Soft reset the transceiver\n"
      "  ethctl <interface> phy-reset [port <sub_port#>]\n"
      "    [port <sub_port#>]: required if <interface> has Crossbar or Trunk port underneath\n"
    },
    { 1, "reg", et_cmd_mii_op,
      ": Set/Get port mii register\n"
      "  ethctl <interface> reg <[0-31]> [0xhhhh] [port <sub_port#>]\n"
      "    [port <sub_port#>]: required if <interface> has Crossbar or Trunk port underneath\n"
    },
    { 1, "phy-power", et_cmd_phy_power_op,
      ": Phy power up/down control\n"
      "  ethctl <interface> phy-power <up|down>"
    },
    { 1, "vport", et_cmd_vport_op,
      ": Enable/disable/query Switch for VLAN port mapping\n"
      "  ethctl <interface> vport <enable|disable|query>"
    },
    { 0, "stats", et_cmd_stats_op,
      ": Display software stats\n"
      "  ethctl <interface> stats"
    },
    { 1, "ethernet@wirespeed", et_cmd_ethernet_at_wirespeed_op,
      ": Enable/Disable ethernet@wirespeed\n"
      "  ethctl <interface> ethernet@wirespeed <show|enable|disable> [port <sub_port#>]\n"
      "    [port <sub_port#>]: required if <interface> has Crossbar or Trunk port underneath\n"
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

/* These commands don't require interface to be specified */
static const struct command common_commands[] = {
    { 0, "tx_iudma", et_cmd_tx_iudma_op,
      ": Set/Get number of Tx iuDMA channels\n"
      "  ethctl tx_iudma <[1-4]>"
    },
    { 0, "rx_iudma", et_cmd_rx_iudma_op,
      ": Set/Get number of Rx iuDMA channels\n"
      "  ethctl rx_iudma <[1-4]>\n"
    },
    { 3, "phy", et_cmd_phy_op,
      ": Phy Access \n"
      "  ethctl phy int|ext|extsw|ext32 <phy_addr> <reg> [<value|reg2> [no_read_back]] [-d]\n"
      "  ethctl phy i2c <bus_num> <reg> [<value|reg2> [no_read_back]] [-d] \n"
      "      <reg>: 0-0x1f: CL22 IEEE register; 0x1f-0xffff: Broadcom Extended Registers.\n"
      "             0x1,0000-0x1f,ffff: CL45 IEEE Register, DeviceAddress + 2 byte Registers.\n"
      "             0x20,ffff-0xffff,ffff: Broadcom PHY 32bit address.\n"
      "      <ext32>: Force to access Broadcom phy 32bit address.\n" 
      "      <bus_num>: I2C Bus Number: "
#if defined(CHIP_63158)
      " 0: 10GAE SFP Module; 1: 2.5G Serdes SFP Module\n"
#else
      " 0: 2.5G SFP Module\n"
#endif
      "  ethctl phy serdespower <phy_addr> [<power_mode>]\n"
      "      [<power_mode>]: 0 - Non power saving mode; for loop back, inter connection\n"
      "                      1 - Basic power saving mode; default mode\n"
      "                      2 - Device Forced Off\n"
      "  ethctl phy 10gserdes 0 <register> [<value&mask(hi16&lo16)>]\n"
      "  ethctl phy 10gpcs 0 <register> [<value&mask(hi16&lo16)>]\n"
      "     Read/Write 10G Serdes/PCS registers\n"
      "      <register>: 10G AE register address including higher 16bits total 32bits\n"
      "         Higher 16 Bit definitions:\n"
      "             DEVID_0:0x0000, DEVID_1: 0x0800; PLL_0: 0x0000, PLL_1: 0x0100\n"
      "             LANE_0-3: 0x0000-0x0003, LANE_BRDCST: 0x00FF\n"
      "      <val&mask>: higher 16 bit values and lower 16 bit mask.\n"
      "             1s in mask will ignore bits in value. 0x1234ff00 write 0x34 only.\n"
      "  -d: Dump registers started from <reg> to <reg2>.\n"
    },
};


cmd_t *common_command_lookup(const char *cmd)
{
    int i;

    for (i = 0; i < sizeof(common_commands)/sizeof(common_commands[0]); i++) {
        if (!strcmp(cmd, common_commands[i].name))
            return (cmd_t *)&common_commands[i];
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

    fprintf(stderr, "Interface specific commands:\n");
    fprintf(stderr, "Usage: ethctl <interface> <command> [arguments...]\n\n");
    for (i = 0; i < sizeof(commands)/sizeof(commands[0]); i++)
        command_help(commands+i);

    fprintf(stderr, "\nCommon commands:\n");
    fprintf(stderr, "Usage: ethctl <command> [arguments...]\n\n");
    for (i = 0; i < sizeof(common_commands)/sizeof(common_commands[0]); i++)
        command_help(common_commands+i);
}

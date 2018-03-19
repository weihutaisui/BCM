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
#include <sys/errno.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/ethtool.h>
#include "bcm/bcmswapitypes.h"
#include "ethswctl_api.h"

//#include "bcmtypes.h"


#ifdef DESKTOP_LINUX

/* when running on DESKTOP_LINUX, redirect ioctl's to a fake one */
static int fake_ethsw_ioctl(int fd, int cmd, void *data);
#define ETHSW_IOCTL_WRAPPER  fake_ethsw_ioctl

#else

/* When running on actual target, call the real ioctl */
#define ETHSW_IOCTL_WRAPPER  ioctl

#endif

/* Init the socket to bcmsw interface */
static inline int ethswctl_init(struct ifreq *p_ifr)
{
    int skfd;

    /* Open a basic socket */
    if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket open error");
        return -1;
    }

#ifndef DESKTOP_LINUX
    /* Get the name -> if_index mapping for ethswctl */
    strcpy(p_ifr->ifr_name, "bcmsw");
    if (ioctl(skfd, SIOCGIFINDEX, p_ifr) < 0 ) {
        strcpy(p_ifr->ifr_name, "eth0");
        if (ioctl(skfd, SIOCGIFINDEX, p_ifr) < 0 ) {
            close(skfd);
            printf("neither bcmsw nor eth0 exist\n");
            return -1;
        }
    }
#endif

    return skfd;
}

/* Init the socket to given p_ifr->ifr_name */
static inline int ethswctl_open_socket(struct ifreq *p_ifr)
{
    int skfd;

    /* Open a basic socket */
    if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket open error");
        return -1;
    }

    /* Get the name -> if_index mapping for ethswctl */
    if (ETHSW_IOCTL_WRAPPER(skfd, SIOCGIFINDEX, p_ifr) < 0 ) {
        close(skfd);
        printf("%s interface does not exist \n", p_ifr->ifr_name);
        return -1;
    }

    return skfd;
}

/* Dump MIB counters of a given switch port */
/* type = 0 to dump a subset; type = 1 to dump all */
/* unit = 0 Internal switch; unit = 1 External switch */
/* priority is meaningful for chip only mornitoring single queue each time" */
int ethswctl_mibdump_x(int unit, int port, int priority, int type)
{
    int skfd, err = -1;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    /* Validate inputs */
    if (((port & ~BCM_EXT_6829) < 0) || ((port & ~BCM_EXT_6829) > 8)) {
        printf("Invalid port. Valid values are 0 to 8. \n");
        err = -1;
        goto out;
    }
    if (type < 0 || type > 2) {
        printf("Invalid type <%d> . Valid values are 0,1 and 2. \n", type);
        err = -1;
        goto out;
    }
    if (unit < 0 || unit > 1) {
        printf("Invalid unit. Valid values are 0 and 1. \n");
        err = -1;
        goto out;
    }

    /* Determine whether to dump all or a subset of mib counters */
    e->op = ETHSWDUMPMIB;
    e->type = type;
    e->port = port;
    e->unit = unit;
    e->queue = priority;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
    }

out:
    close(skfd);
    return err;
}

/* Dump all the registers of a given page of the switch */
int ethswctl_pagedump(int unit, int page)
{
    int skfd, err = -1;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    if (((page & ~BCM_EXT_6829) < 0) || ((page & ~BCM_EXT_6829) > 0x72) ||
        ((page & ~BCM_EXT_6829) == 0x03) || ((page & ~BCM_EXT_6829) == 0x29) ||
        (((page & ~BCM_EXT_6829) >= 0x06) && ((page & ~BCM_EXT_6829) <= 0x19)) ||
        (((page & ~BCM_EXT_6829) >= 0x35) && ((page & ~BCM_EXT_6829) <= 0x39)) ||
        (((page & ~BCM_EXT_6829) >= 0x44) && ((page & ~BCM_EXT_6829) <= 0x71)) ) {
        printf("Invalid page number. \n");
        err = -1;
        goto out;
    }

    e->op = ETHSWDUMPPAGE;
    e->unit = unit;
    e->page = page;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
    }

out:
    close(skfd);
    return err;
}

/* Dump all enet iuDMA rx and tx channel info - Aug 2010 */
int bcm_iudma_op(int channel, int mode, int all, int enable, int descCnt, int byteCnt)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWDUMPIUDMA;
    e->channel = channel;
    e->op_map = mode;
    e->val = descCnt;
    e->length = byteCnt;

    if (enable == -1) {
        e->sub_type = TYPE_GET;
    }
    else {
        if (enable)
            e->sub_type = TYPE_ENABLE;
        else
            e->sub_type = TYPE_DISABLE;
    }

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
    }

    close(skfd);
    return err;
}

/* Get/Set the iuDMA rx channel for a specific eth port - Jan 2011 */
int bcm_iudma_split(int port, int iudma)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWIUDMASPLIT;
    e->port = port;
    if(-1 == iudma)
        e->type = TYPE_GET;
    else
    {
        e->type = TYPE_SET;
        e->val = iudma;
    }

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
    }

    close(skfd);
    return err;
}

/* Enable hardware switching */
int ethswctl_enable_switching(void)
{
    int skfd, err = -1;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWSWITCHING;
    e->type = TYPE_ENABLE;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        printf("ioctl SIOCETHSWCTLOPS returned error %d (%s)\n", errno, strerror(errno));
    }

    close(skfd);
    return err;
}

/* Disable hardware switching */
int ethswctl_disable_switching(void)
{
    int skfd, err = -1;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWSWITCHING;
    e->type = TYPE_DISABLE;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
    }

    close(skfd);
    return err;
}

/* Get enable/disable status of hardware switching */
int ethswctl_get_switching_status(int *status)
{
    int skfd, err = -1;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWSWITCHING;
    e->type = TYPE_GET;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
        goto out;
    }
    *status = e->status;


out:
    close(skfd);
    return err;
}

/*Only set hardware switching flag, don't change dal_miss_action*/
int ethswctl_set_switch_flag(BOOL enable)
{
    int skfd, err = -1;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWSWITCHFLAG;
    if (enable)    
    {
        e->type = TYPE_ENABLE;
    }
    else
    {
        e->type = TYPE_DISABLE;
    }
    
    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        printf("ioctl SIOCETHSWCTLOPS returned error %d (%s)\n", errno, strerror(errno));
    }

    close(skfd);
    return err;
}

/****************************************************************************/
/*  Switch Control API                                                      */
/****************************************************************************/

/*
 * Function:
 *  bcm_switch_control_set
 * Description:
 *  Set Flow Control Drop/Pause Control mechanisms of the switch.
 *  unit - RoboSwitch PCI device unit number (driver internal).
 *    type - The desired configuration parameter to modify.
 *    arg - The value with which to set the parameter.
 * Returns:
 *    BCM_E_xxxx
 */
int bcm_switch_control_set(int unit, bcm_switch_control_t type, int arg)
{
    return bcm_switch_control_setX(unit, type, 0, arg);
}
int bcm_switch_control_setX(int unit, bcm_switch_control_t type,
                   bcm_switch_fc_t sub_type, int arg)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWCONTROL;
    e->type = TYPE_SET;
    e->sw_ctrl_type = type;
    e->sub_type = sub_type;
    e->val = arg;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
    }

    close(skfd);
    return err;
}

/***************************************************************************
 * Function:
 * int ethswctl_quemap_call(int unit, int port, int que, int type);
 * Description:
 *  Set/Get WAN/LAN Queue bit map
 *  port - Port number to be monitored.
 * Returns:
 *    BCM_E_xxxx
***************************************************************************/
int ethswctl_quemap_call(int unit, int *val, int *queRemap, int set)
{
    int skfd, err = -1;
    struct ifreq ifr;
    struct ethswctl_data ifd;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifd;

    ifd.op = ETHSWQUEMAP;
    ifd.type = set? TYPE_SET: TYPE_GET;
    ifd.val = *val;
    ifd.priority = *queRemap;
    ifd.unit = 1;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
        goto out;
    }

    *val = ifd.val;
    *queRemap = ifd.priority;

out:
    close(skfd);
    return err;
}

/****************************************************************************/
/*
 * Function:
 * int ethswctl_quemon_get(int unit, int port, int que, int type);
 * Description:
 *  Set Flow Control Drop/Pause Control mechanisms of the switch.
 *  Unit - Unit number to be monitored.
 *  port - Port number to be monitored.
 *  type - Flow control type to be monitored.
 * Returns:
 *    BCM_E_xxxx
 */
int ethswctl_quemon_get(int unit, int port, int que, int type, int *val)
{
    int skfd, err = -1;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWQUEMON;
    e->type = TYPE_GET;
    e->unit = 1;
    e->port = port;
    e->priority = que;
    e->sw_ctrl_type = type;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
        goto out;
    }

    *val = e->val;

out:
    close(skfd);
    return e->ret_val;
}

/*
 * Function:
 *  bcm_switch_control_get
 * Description:
 *  Get Flow Control Drop/Pause Control mechanisms of the switch.
 * Parameters:
 *  unit - RoboSwitch PCI device unit number (driver internal).
 *    type - The desired configuration parameter to retrieve.
 *    arg - Pointer to where the retrieved value will be written.
 * Returns:
 *    BCM_E_xxxx
 */
int bcm_switch_control_get(int unit, bcm_switch_control_t type, int *arg)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWCONTROL;
    e->type = TYPE_GET;
    e->sw_ctrl_type = type;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
        goto out;
    }

    *arg = e->val;

out:
    close(skfd);
    return err;
}


/*
 * Function:
 *  bcm_switch_control_priority_set
 * Description:
 *  Set switch parameters on a per-priority (cos) basis.
 * Parameters:
 *  unit - Device unit number
 *  priority - The priority to affect
 *  type - The desired configuration parameter to modify
 *  arg - The value with which to set the parameter
 * Returns:
 *  BCM_E_xxx
 */
int bcm_switch_control_priority_set(int unit, bcm_cos_t priority,
                  bcm_switch_control_t type, int arg)
{
    return bcm_switch_control_priority_setX(unit, 0, priority, type, arg);
}

int bcm_switch_control_priority_setX(int unit, int port, bcm_cos_t priority,
                  bcm_switch_control_t type, int arg)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWPRIOCONTROL;
    e->priority = priority;
    e->type = TYPE_SET;
    e->val = arg;
    e->sw_ctrl_type = type;
    e->unit = unit;
    e->port = port;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
    }

    close(skfd);
    return err;
}

/*
 * Function:
 *  bcm_switch_control_priority_get
 * Description:
 *  Get switch parameters on a per-priority (cos) basis.
 * Parameters:
 *  unit - Device unit number
 *  priority - The priority to affect
 *  type - The desired configuration parameter to retrieve
 *  arg - Pointer to where the retrieved value will be written
 * Returns:
 *  BCM_E_xxx
 */
int bcm_switch_control_priority_get(int unit, bcm_cos_t priority,
                  bcm_switch_control_t type, int *arg)
{
    return bcm_switch_control_priority_getX(unit, 0, priority, type, arg);
}

int bcm_switch_control_priority_getX(int unit, int port, bcm_cos_t priority,
                  bcm_switch_control_t type, int *arg)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWPRIOCONTROL;
    e->unit = unit;
    e->port = port;
    e->priority = priority;
    e->type = TYPE_GET;
    e->sw_ctrl_type = type;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
        goto out;
    }

    *arg = e->ret_val;

out:
    close(skfd);
    return err;
}
/*
 * Function:
 *  bcm_acb_set
 * Description:
 *  Set ACB configuration
 *  unit - RoboSwitch PCI device unit number (driver internal).
 *    type - The desired configuration parameter to modify.
 *    arg - The value with which to set the parameter.
 * Returns:
 *    BCM_E_xxxx
 */
int bcm_acb_cfg_set(int unit, int queue, bcm_switch_acb_control_t type, int arg)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWACBCONTROL;
    e->type = TYPE_SET;
    e->sw_ctrl_type = type;
    e->val = arg;
    e->unit = unit;
    e->queue = queue;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
    }

    close(skfd);
    return err;
}

/*
 * Function:
 *  bcm_switch_control_get
 * Description:
 *  Get Flow Control Drop/Pause Control mechanisms of the switch.
 * Parameters:
 *  unit - RoboSwitch PCI device unit number (driver internal).
 *    type - The desired configuration parameter to retrieve.
 *    arg - Pointer to where the retrieved value will be written.
 * Returns:
 *    BCM_E_xxxx
 */
int bcm_acb_cfg_get(int unit, int queue, bcm_switch_acb_control_t type, void *arg)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWACBCONTROL;
    e->type = TYPE_GET;
    e->sw_ctrl_type = type;
    e->unit = unit;
    e->queue = queue;
    e->vptr = arg;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
        goto out;
    }

out:
    close(skfd);
    return err;
}

/****************************************************************************/
/*  Port Configuration API:  For Configuring Pause Enable/Disable                                    */
/****************************************************************************/
/* Enable/Disable the Pause  */
int bcm_port_pause_capability_set(int unit, bcm_port_t port, char val )
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWPORTPAUSECAPABILITY;
    e->port = port;
    e->type = TYPE_SET;
    e->val = val;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        printf("ioctl SIOCETHSWCTLOPS returned error %d (%s)\n", errno, strerror(errno));
    }

    close(skfd);
    return err;
}

/* Get the Pause Enable/Disable status */
int bcm_port_pause_capability_get(int unit, bcm_port_t port, char *val)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWPORTPAUSECAPABILITY;
    e->port = port;
    e->type = TYPE_GET;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
      goto out;
    }

    *val = (char) e->ret_val;

out:
    close(skfd);
    return err;
}

/*
 * Function:
 *  bcm_port_rate_ingress_set
 * Purpose:
 *  Set ingress rate limiting parameters
 * Parameters:
 *  unit - Device number
 *  port - Port number
 *  kbits_sec - Rate in kilobits (1000 bits) per second.
 *            Rate of 0 disables rate limiting.
 *  kbits_burst - Maximum burst size in kilobits (1000 bits).
 * Returns:
 *  BCM_E_XXX
 * Note :
 *  1. Robo Switch support 2 ingress buckets for different packet type.
 *     And the bucket1 contains higher priority if PKT_MSK confilict
 *       with bucket0's PKT_MSK.
 *  2. Robo Switch allowed system basis rate/packet type assignment for
 *     Rate Control. The RATE_TYPE and PKT_MSK will be set once in the
 *       initial routine.
 */
int bcm_port_rate_ingress_set(int unit,
                  bcm_port_t port,
                  unsigned int kbits_sec,
                  unsigned int kbits_burst)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWPORTRXRATE;
    e->port = port;
    e->limit = kbits_sec;
    e->burst_size = kbits_burst;
    e->type = TYPE_SET;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
    }

    close(skfd);
    return err;
}

/*
 * Function:
 *  bcm_port_rate_ingress_get
 * Purpose:
 *  Get ingress rate limiting parameters
 * Parameters:
 *  unit - Device number
 *  port - Port number
 *  kbits_sec - (OUT) Rate in kilobits (1000 bits) per second, or
 *                  zero if rate limiting is disabled.
 *  kbits_burst - (OUT) Maximum burst size in kilobits (1000 bits).
 * Returns:
 *  BCM_E_XXX
 */
int bcm_port_rate_ingress_get(int unit,
                  bcm_port_t port,
                  unsigned int *kbits_sec,
                  unsigned int *kbits_burst)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWPORTRXRATE;
    e->port = port;
    e->type = TYPE_GET;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
      goto out;
    }

    *kbits_sec =  e->limit;
    *kbits_burst = e->burst_size;

out:
    close(skfd);
    return err;
}


/*
 * Function:
 *  bcm_port_rate_egress_set
 * Purpose:
 *  Set egress rate limiting parameters
 * Parameters:
 *  unit - Device number
 *  port - Port number
 *  kbits_sec - Rate in kilobits (1000 bits) per second.
 *          Rate of 0 disables rate limiting.
 *  kbits_burst - Maximum burst size in kilobits (1000 bits).
 * Returns:
 *  BCM_E_XXX
 * Note :
 *  1. Robo Switch support 2 ingress buckets for different packet type.
 *     And the bucket1 contains higher priority if PKT_MSK confilict
 *       with bucket0's PKT_MSK.
 *  2. Robo Switch allowed system basis rate/packet type assignment for
 *     Rate Control. The RATE_TYPE and PKT_MSK will be set once in the
 *       initial routine.
 */
int bcm_port_shaper_cfg(int unit,
                 bcm_port_t port,
                 int queue,
                 int shaper_cfg_flags,
                 int val)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWPORTSHAPERCFG;
    e->port = port;
    e->type = TYPE_SET;
    e->unit = unit;
    e->queue = queue;
    e->sub_type = shaper_cfg_flags;
    e->val = val;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
    }

    close(skfd);
    return err;
}
/*
 * Function:
 *  bcm_port_rate_egress_set
 * Purpose:
 *  Set egress rate limiting parameters
 * Parameters:
 *  unit - Device number
 *  port - Port number
 *  kbits_sec - Rate in kilobits (1000 bits) per second.
 *          Rate of 0 disables rate limiting.
 *  kbits_burst - Maximum burst size in kilobits (1000 bits).
 * Returns:
 *  BCM_E_XXX
 * Note :
 *  1. Robo Switch support 2 ingress buckets for different packet type.
 *     And the bucket1 contains higher priority if PKT_MSK confilict
 *       with bucket0's PKT_MSK.
 *  2. Robo Switch allowed system basis rate/packet type assignment for
 *     Rate Control. The RATE_TYPE and PKT_MSK will be set once in the
 *       initial routine.
 */
int bcm_port_rate_egress_set(int unit,
                 bcm_port_t port,
                 unsigned int erc_limit,
                 unsigned int erc_burst)
{
    int queue = -1;
    int is_pkt_mode = 0;
#if defined(CHIP_63138) || defined(CHIP_63148)
    DBG(printf("On your platform, calling bcm_port_rate_egress_Set_X() delivers enhanced functionality \n\n"););
#endif
    return bcm_port_rate_egress_set_X(unit, port, erc_limit, erc_burst,
                 queue, is_pkt_mode);
}
int bcm_port_rate_egress_set_X(int unit,
                 bcm_port_t port,
                 unsigned int erc_limit,
                 unsigned int erc_burst,
                 int queue,
                 int is_pkt_mode)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWPORTTXRATE;
    e->port = port;
    e->limit = erc_limit;
    e->burst_size = erc_burst;
    e->type = TYPE_SET;
    e->unit = unit;
    e->queue = queue;
    e->sub_type = is_pkt_mode;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
    }

    close(skfd);
    return err;
}

/*
 * Function:
 *  bcm_port_rate_egress_get
 * Purpose:
 *  Get egress rate limiting parameters
 * Parameters:
 *  unit - Device number
 *  port - Port number
 *  kbits_sec - (OUT) Rate in kilobits (1000 bits) per second, or
 *              zero if rate limiting is disabled.
 *  kbits_burst - (OUT) Maximum burst size in kilobits (1000 bits).
 * Returns:
 *  BCM_E_XXX
 */
int bcm_port_rate_egress_get(int unit,
                 bcm_port_t port,
                 unsigned int *kbits_sec,
                 unsigned int *kbits_burst)
{
    int queue = -1;
#if defined(CHIP_63138) || defined(CHIP_63148)
    DBG(printf("On your platform, calling bcm_port_rate_egress_get_X() delivers enhanced functionality \n\n"););
#endif
    return bcm_port_rate_egress_get_X(unit, port, kbits_sec, kbits_burst, queue, NULL);

}
int bcm_port_rate_egress_get_X(int unit,
                 bcm_port_t port,
                 unsigned int *kbits_sec,
                 unsigned int *kbits_burst,
                 int queue,           // Extended interface from here
                 void *vptr)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWPORTTXRATE;
    e->port = port;
    e->type = TYPE_GET;
    e->unit = unit;
    e->queue = queue;
    e->vptr = vptr;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
        goto out;
    }
out:
    if (vptr == NULL) { // Tells Legacy interface has been called
    *kbits_sec =  e->limit;
    *kbits_burst = e->burst_size;
    }
    close(skfd);
    return err;
}
int bcm_port_learning_ind_set(int unit, bcm_port_t port, unsigned char learningInd)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWPORTSALDAL;
    e->port = port;
    e->type = TYPE_SET;
    e->unit = unit;
    e-> sal_dal_en = learningInd;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr)))
      {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
          }
    close(skfd);
    return err;
}

int   bcm_mtu_set (unsigned int mtu)
 {
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;
    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;
    e->op = ETHSWPORTMTU;
    e->port = 0;
    e->type = TYPE_SET;
    e->unit = 0;
    e->mtu = mtu;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr)))
      {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
          }
    close(skfd);
    return err;
 }

/* Set the 802.1p bits of port default tag */
int bcm_port_untagged_priority_set(int unit, bcm_port_t port, int priority)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWPORTDEFTAG;
    e->port = port;
    e->type = TYPE_SET;
    e->priority = priority;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
    }

    close(skfd);
    return err;
}

/* Retrieve the 802.1p bits from port default tag */
int bcm_port_untagged_priority_get(int unit, bcm_port_t port, int *priority)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWPORTDEFTAG;
    e->port = port;
    e->type = TYPE_GET;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
      goto out;
    }

    *priority = e->priority;

out:
    close(skfd);
    return err;
}

int bcm_port_transparent_set(int unit, bcm_port_t port, unsigned char enable)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWPORTTRANSPARENT;
    e->port = port;
    e->type = TYPE_SET;
    e->unit = unit;
    e->transparent = enable;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr)))
      {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
          }
    close(skfd);
    return err;
}


int bcm_port_vlan_isolation_set(int unit, bcm_port_t port, unsigned char us,unsigned char ds)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWPORTVLANISOLATION;
    e->port = port;
    e->type = TYPE_SET;
    e->unit = unit;
    e->vlan_isolation.us_enable = us;
    e->vlan_isolation.ds_enable = ds;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr)))
      {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
          }
    close(skfd);
    return err;
}


int bcm_port_bc_rate_limit_set(int unit, bcm_port_t port, unsigned int rate)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWCPUMETER;
    e->port = port;
    e->type = TYPE_SET;
    e->unit = unit;
    e->cpu_meter_rate_limit.meter_type = METER_TYPE_BROADCAST;
    e->cpu_meter_rate_limit.rate_limit = rate;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr)))
      {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
          }
    close(skfd);
    return err;
}


/****************************************************************************/
/*  Enet Driver Config/Control API:  For Configuring Enet Driver Rx Scheduling                 */
/****************************************************************************/

/*
 * Function:
 *  bcm_robo_enet_driver_rx_scheduling_set
 * Description:
 *  Select the enet driver rx scheduling mechanism
 * Parameters:
 *  unit - Device unit number
 *  scheduling - scheduling mechanism selection
 * Returns:
 *  BCM_E_xxx
 */
int bcm_enet_driver_rx_scheduling_set(int unit, int scheduling)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWRXSCHEDULING;
    e->type = TYPE_SET;
    e->scheduling = scheduling;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
    }

    close(skfd);
    return err;
}

/*
 * Function:
 *  bcm_robo_enet_driver_rx_scheduling_get
 * Description:
 *  Get the enet driver rx scheduling mechanism
 * Parameters:
 *  unit - Device unit number
 *  scheduling - scheduling mechanism
 * Returns:
 *  BCM_E_xxx
 */
int bcm_enet_driver_rx_scheduling_get(int unit, int *scheduling)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWRXSCHEDULING;
    e->type = TYPE_GET;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
        goto out;
    }

    *scheduling = (char) e->scheduling;

out:
    close(skfd);
    return err;
}

/*
 * Function:
 *  bcm_robo_enet_driver_wrr_config_set
 * Description:
 *  Configure the WRR parameters
 * Parameters:
 *  unit - Device unit number
 *  max_pkts_per_iter - Max number of packets when the weights will be reloaded.
 *  weights - Pointer to integer array of WRR weights of all 4 channels.
 * Returns:
 *  BCM_E_xxx
 */
int bcm_enet_driver_wrr_weights_set(int unit, int max_pkts_per_iter,
                           int *weights)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWWRRPARAM;
    e->type = TYPE_SET;
    e->unit = unit;
    e->max_pkts_per_iter = max_pkts_per_iter;
    e->weights[0] = *weights;
    e->weights[1] = *(weights+1);
    e->weights[2] = *(weights+2);
    e->weights[3] = *(weights+3);

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
    }

    close(skfd);
    return err;
}

/*
 * Function:
 *  bcm_robo_enet_driver_wrr_config_get
 * Description:
 *  Retrieve the WRR parameters
 * Parameters:
 *  unit - Device unit number
 *  max_pkts_per_iter - Max number of packets when the weights will be reloaded.
 *  weights - Pointer to integer array of WRR weights of all 4 channels.
 * Returns:
 *  BCM_E_xxx
 */
int bcm_enet_driver_wrr_weights_get2(int unit, int *max_pkts_per_iter, 
    int *weights, int *weight_pkts, int *rx_queues)
{
    int i, skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWWRRPARAM;
    e->type = TYPE_GET;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
        goto out;
    }

    *max_pkts_per_iter = e->max_pkts_per_iter;

    if (rx_queues) *rx_queues = e->rx_queues;

    for (i=0; i<BCM_COS_COUNT; i++) {
        weights[i] = e->weights[i];
        if (weight_pkts) weight_pkts[i] = e->weight_pkts[i];
    }

out:
    close(skfd);
    return err;

}

/* Get enable/disable status of using default queue as egress queue */
int bcm_enet_driver_use_default_txq_status_get(int unit, char *ifname,
                                               int *operation)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    strcpy(ifr.ifr_name, ifname);
    if ((skfd=ethswctl_open_socket(&ifr)) < 0) {
      printf("ethswctl_open_socket failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWUSEDEFTXQ;
    e->type = TYPE_GET;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
      goto out;
    }

    *operation = e->ret_val;

out:
    close(skfd);
    return err;
}

/* Enable or disable using default queue as egress queue */
int bcm_enet_driver_use_default_txq_status_set(int unit, char *ifname,
                                               int operation)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    strcpy(ifr.ifr_name, ifname);
    if ((skfd=ethswctl_open_socket(&ifr)) < 0) {
      printf("ethswctl_open_socket failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWUSEDEFTXQ;
    e->type = TYPE_SET;
    e->val = operation;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
    }

    close(skfd);
    return err;
}

/* Get enable/disable status of using default queue as egress queue */
int bcm_enet_driver_default_txq_get(int unit, char *ifname,
                                    bcm_cos_queue_t *cosq)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    strcpy(ifr.ifr_name, ifname);
    if ((skfd=ethswctl_open_socket(&ifr)) < 0) {
      printf("ethswctl_open_socket failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWDEFTXQ;
    e->type = TYPE_GET;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
      goto out;
    }

    *cosq = (bcm_cos_queue_t)e->queue;

out:
    close(skfd);
    return err;
}

/* Enable or disable using default queue as egress queue */
int bcm_enet_driver_default_txq_set(int unit, char *ifname,
                                    bcm_cos_queue_t cosq)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    strcpy(ifr.ifr_name, ifname);
    if ((skfd=ethswctl_open_socket(&ifr)) < 0) {
      printf("ethswctl_open_socket failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWDEFTXQ;
    e->type = TYPE_SET;
    e->queue = cosq;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
    }

    close(skfd);
    return err;
}

/* Get enable/disable status of rx byte rate limiting of given channel */
int bcm_enet_driver_rx_rate_limit_cfg_get(int unit, int channel, int *status)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWRXRATELIMITCFG;
    e->type = TYPE_GET;
    e->channel = channel;
    e->unit = unit;

    printf("e->channel = %d \n", e->channel);
    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
      goto out;
    }

    *status = (bcm_cos_queue_t)e->ret_val;

out:
    close(skfd);
    return err;
}

/* Enable or disable rx byte rate limiting of given channel */
int bcm_enet_driver_rx_rate_limit_cfg_set(int unit, int channel, int value)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWRXRATELIMITCFG;
    e->type = TYPE_SET;
    e->channel = channel;
    e->val = value;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
    }

    close(skfd);
    return err;
}

/* Get rx byte rate of a given channel */
int bcm_enet_driver_rx_rate_get(int unit, int channel, int *rate)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWRXRATECFG;
    e->type = TYPE_GET;
    e->channel = channel;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
      goto out;
    }

    *rate = e->ret_val;

out:
    close(skfd);
    return err;
}

/* Set rx byte rate of a given channel */
/* rate in bytes per second */
int bcm_enet_driver_rx_rate_set(int unit, int channel, int rate)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWRXRATECFG;
    e->type = TYPE_SET;
    e->channel = channel;
    e->val = rate;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
    }

    close(skfd);
    return err;
}

/* Get Enable/Disable status of the Interrupt processing of a given channel */
/* For debugging only */
int bcm_enet_driver_test_config_get(int unit, int type, int param, int *val)
{
  int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

  if ((skfd=ethswctl_init(&ifr)) < 0) {
    printf("ethswctl_init failed. \n");
        return skfd;
  }
    ifr.ifr_data = (char *)&ifdata;

  e->op = ETHSWTEST1;
  e->type = TYPE_GET;
  e->sub_type = type;
  e->unit = unit;
  if ((type == SUBTYPE_ISRCFG) ||
      (type == SUBTYPE_RESETSWITCH)) {
    e->channel = param;
  }
  if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
    printf("ioctl command return error!\n");
    goto out;
  }

  if (type == SUBTYPE_RESETMIB) {
    /* Also reset the software counters */
    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCSCLEARMIBCNTR, &ifr))) {
      printf("ioctl SIOCSCLEARMIBCNTR return error!\n");
      goto out;
    }
  }

  *val = (int)e->ret_val;

out:
    close(skfd);
    return err;
}

/* Enable or Disable the Interrupt processing of a given channel */
/* For debugging only */
int bcm_enet_driver_test_config_set(int unit, int type, int param, int val)
{
  int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

  if ((skfd=ethswctl_init(&ifr)) < 0) {
    printf("ethswctl_init failed. \n");
        return skfd;
  }
    ifr.ifr_data = (char *)&ifdata;

  e->op = ETHSWTEST1;
  e->type = TYPE_SET;
  e->sub_type = type;
  e->unit = unit;
    if (type == SUBTYPE_ISRCFG) {
        e->channel = param;
    }
    e->val = val;

  if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
    printf("ioctl command return error!\n");
  }

    close(skfd);
    return err;
}

/* Get enable/disable status of rx pkt rate limiting of given channel */
int bcm_enet_driver_rx_pkt_rate_cfg_get(int unit, int channel, int *status)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWRXPKTRATELIMITCFG;
    e->type = TYPE_GET;
    e->channel = channel;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
      goto out;
    }

    *status = (bcm_cos_queue_t)e->ret_val;

out:
    close(skfd);
    return err;
}

/* Enable or disable rx pkt rate limiting of given channel */
int bcm_enet_driver_rx_pkt_rate_cfg_set(int unit, int channel, int value)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWRXPKTRATELIMITCFG;
    e->type = TYPE_SET;
    e->channel = channel;
    e->val = value;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
    }

    close(skfd);
    return err;
}

/* Get rx packet rate of a given channel */
int bcm_enet_driver_rx_pkt_rate_get(int unit, int channel, int *rate)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWRXPKTRATECFG;
    e->type = TYPE_GET;
    e->channel = channel;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
      goto out;
    }

    *rate = e->ret_val;

out:
    close(skfd);
    return err;
}

/* Set rx packet rate of a given channel */
/* rate in packets per second */
int bcm_enet_driver_rx_pkt_rate_set(int unit, int channel, int rate)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWRXPKTRATECFG;
    e->type = TYPE_SET;
    e->channel = channel;
    e->val = rate;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
    }

    close(skfd);
    return err;
}
/*
 * Function:
 * Description:
 *  bcm_enet_map_ifname_to_unit_portmap
 * Parameters:
 * input 
 *       - ifname - interface name 
 *  output
 *       - unit -- unit number to caller
 *       - portmap -- port number bitmap to caller
 * Returns:
 *  BCM_E_xxx
 */
int bcm_enet_map_ifname_to_unit_portmap(const char *ifName, int *unit, unsigned int *portmap)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
#ifndef CONFIG_BCM960333
        // do not raise an error for the 60333 chip
        printf("ethswctl_init failed. \n");
#endif
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op   = ETHSWUNITPORT;
    e->type = TYPE_GET;
    strncpy(e->ifname, ifName, IFNAMSIZ);
    e->ifname[IFNAMSIZ - 1] = '\0';
    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP) {
            printf("ioctl not supported!\n");
        }
        else {
            printf("ioctl command return error %d!\n", err);
        }
    }
    *unit = e->unit;
    *portmap = e->port_map;
    DBG(printf("ifname %s is: unit %d portmap 0x%x", *unit, *portmap););
    close(skfd);
    return err;
}
/*
 * Function:
 * Description:
 *  bcm_enet_map_ifname_to_unit_port
 * Parameters:
 * input 
 *       - ifname - interface name 
 *  output
 *       - unit -- unit number to caller
 *       - port -- port number to caller
 * Returns:
 *  BCM_E_xxx
 */
int bcm_enet_map_ifname_to_unit_port(const char *ifName, int *unit, bcm_port_t *port)
{
    unsigned int portmap;
    int err = bcm_enet_map_ifname_to_unit_portmap(ifName,unit,&portmap);
    *port = 0;
    if (!err)
    {
        while(portmap)
        {
            if (portmap & (1<<(*port)))
            {
                break;
            }
            *port +=1;
        }
    }
    DBG(printf("ifname %s is: unit %d port 0x%x", *unit, *port););
    return err;
}

/*
 * Function:
 * Description:
 *  bcm_enet_map_oam_idx_to_unit_port
 * Parameters:
 * input 
 *       - oamIdx - OAM Index for port; as used by TR69/OMCI/EPON-OAM etc 
 *  output
 *       - unit -- unit number to caller
 *       - port -- port number to caller
 * Returns:
 *  BCM_E_xxx
 */
int bcm_enet_map_oam_idx_to_unit_port(int oamIdx, int *unit, bcm_port_t *port)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op   = ETHSWOAMIDXMAPPING;
    e->type = TYPE_GET;
    e->oam_idx_str.map_sub_type = OAM_MAP_SUB_TYPE_TO_UNIT_PORT;
    e->oam_idx_str.oam_idx = oamIdx;
    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP) {
            printf("ioctl not supported!\n");
        }
        else {
            printf("ioctl command return error %d!\n", err);
        }
    }
    *unit = e->unit;
    *port = e->port;
    DBG(printf("oamIdx %d is: unit %d port %d", oamIdx, *unit, *port););
    close(skfd);
    return err;
}


/*
 * Function:
 * Description:
 *  bcm_enet_map_unit_port_to_oam_idx
 * Parameters:
 *  input
 *       - unit - switch unit
 *       - port - physical port on the switch
 * Returns:
 *       - oamIdx - OAM Index for port; as used by TR69/OMCI/EPON-OAM etc
 */
int bcm_enet_map_unit_port_to_oam_idx(int unit, bcm_port_t port)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
         printf("ethswctl_init failed.\n");
         return port;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op   = ETHSWOAMIDXMAPPING;
    e->type = TYPE_GET;
    e->oam_idx_str.map_sub_type = OAM_MAP_SUB_TYPE_FROM_UNIT_PORT;
    e->unit = unit;
    e->port = port;
    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP) {
            printf("ioctl not supported!\n");
        }
        else {
            printf("ioctl command return error %d!\n", err);
        }
        close(skfd);
        return port;
    }

    DBG(printf("oamIdx %d is: unit %d port %d", oamIdx, *unit, *port););
    close(skfd);
    return  e->oam_idx_str.oam_idx;
}

/*
 * Function:
 * Description:
 *  bcm_enet_map_oam_idx_to_rdpaif
 * Parameters:
 * input 
 *       - oamIdx - OAM Index for port; as used by TR69/OMCI/EPON-OAM etc 
 * Returns:
 *       - rdpa_if
 */
int bcm_enet_map_oam_idx_to_rdpaif(int oamIdx)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op   = ETHSWOAMIDXMAPPING;
    e->type = TYPE_GET;
    e->oam_idx_str.map_sub_type = OAM_MAP_SUB_TYPE_TO_RDPA_IF;
    e->oam_idx_str.oam_idx = oamIdx;
    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP) {
            printf("ioctl not supported!\n");
        }
        else {
            printf("ioctl command return error %d!\n", err);
        }
    }
    DBG(printf("oamIdx %d is: rdpa_if %d", oamIdx, *rdpa_if););
    close(skfd);
    return e->oam_idx_str.rdpa_if;
}

/*
 * Function:
 * Description:
 *  bcm_enet_map_oam_idx_to_phys_port
 * Parameters:
 * input 
 *       - oamIdx - OAM Index for port; as used by TR69/OMCI/EPON-OAM etc 
 * Returns:
 *       - physical port
 */
int bcm_enet_map_oam_idx_to_phys_port(bcm_port_t oam_idx)
{
    int err = 0;
    int unit = 0;
    int port = 0;

    err = bcm_enet_map_oam_idx_to_unit_port(oam_idx, &unit, &port);
    /*if error raised, return port without mapping*/
    if (err){
        printf("Get unit port failed, oam_idx=%d\n", oam_idx);
        return oam_idx;
    }
    return port;
}

/*
 * Function:
 * Description:
 *  bcm_enet_map_phys_port_to_rdpa_if()
 * Parameters:
 * input 
 *       - unit - switch unit
 *       - port - physical port on the switch
 *  output
 *       - port --  rdpa_if enum
 * Returns:
 *  BCM_E_xxx
 */
int bcm_enet_map_phys_port_to_rdpa_if(int unit, bcm_port_t port)
{

    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op   = ETHSWRDPAPORTGET;
    e->type = TYPE_GET;
    e->unit = unit;
    e->port = port;
    
    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP) {
            printf("ioctl not supported!\n");
        }
        else {
            printf("ioctl command return error %d!\n", err);
        }
        close(skfd);
        return err;
    } else {
        DBG(printf("unit %d port %d --> rdpa_if %#x", e->unit, e->port, e->val););
        close(skfd);
        return e->val;
    }
}

int bcm_enet_get_rdpa_if_from_if_name(const char* ifname, int* rdpaIf_p)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op   = ETHSWRDPAPORTGETFROMNAME;
    e->type = TYPE_GET;
    strcpy(e->ifname, ifname);

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP) {
            printf("ioctl not supported!\n");
        }
        else {
            printf("ioctl command return error %d!\n", err);
        }
        close(skfd);
        return err;
    } else {
        DBG(printf("ifname - %s rdpa_if %d", e->ifname, e->val););
        close(skfd);
        *rdpaIf_p = e->val;
        return err;
    }
}


int bcm_enet_driver_enable_soft_switching_port(int bEnable, char *ifName)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    err = bcm_enet_map_ifname_to_unit_port(ifName, &e->unit, &e->port);
    if(err) return err;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op   = ETHSWSOFTSWITCHING;
    e->type = bEnable ? TYPE_ENABLE : TYPE_DISABLE;
    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP) {
            printf("ioctl not supported!\n");
        }
        else {
            printf("ioctl command return error %d!\n", err);
        }
    }

    close(skfd);
    return err;
}

int bcm_enet_driver_get_soft_switching_status(unsigned int *portmap)
{
    int skfd, err = -1;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWSOFTSWITCHING;
    e->type = TYPE_GET;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP) {
            printf("ioctl not supported!\n");
        }
        else {
            printf("ioctl command return error %d!\n", err);
        }
        goto out;
    }

    *portmap = (unsigned int)e->status;
out:
    close(skfd);
    return err;
}

int bcm_enet_driver_hw_stp_set(int bEnable, char *ifName)
{
    int skfd, err = -1;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    err = bcm_enet_map_ifname_to_unit_port(ifName, &e->unit, &e->port);
    if(err) return err;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op   = ETHSWHWSTP;
    e->type = bEnable ? TYPE_ENABLE : TYPE_DISABLE;
    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
    }

    close(skfd);
    return err;
}

int bcm_enet_driver_get_hw_stp_status(unsigned int *portmap)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op   = ETHSWHWSTP;
    e->type = TYPE_GET;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
      goto out;
    }

    *portmap = (unsigned int)e->status;
out:
    close(skfd);
    return err;
}

/* VLAN TBL ACCESS FUNCTIONS */

/*
 * Function:
 *  bcm_robo_vlan_port_set
 * Description:
 *  Configure the WRR parameters
 * Parameters:
 *  unit - Device unit number
 *  vid -  VLAN ID
 *  fwd_map - Members of the VLAN
 *  untag_map - Untagged members of the VLAN
 * Returns:
 *  BCM_E_xxx
 */
int bcm_vlan_port_set(int unit, int vid, int fwd_map, int untag_map)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWVLAN;
    e->type = TYPE_SET;
    e->vid = vid;
    e->fwd_map = fwd_map;
    e->untag_map = untag_map;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
    }

    close(skfd);
    return err;
}

/*
 * Function:
 *  bcm_robo_vlan_port_get
 * Description:
 *  Retrieve the WRR parameters
 * Parameters:
 *  unit - Device unit number
 *  vid -  VLAN ID
 *  fwd_map - Members of the VLAN
 *  untag_map - Untagged members of the VLAN
 * Returns:
 *  BCM_E_xxx
 */
int bcm_vlan_port_get(int unit, int vid, int * fwd_map, int *untag_map)

{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWVLAN;
    e->type = TYPE_GET;
    e->vid = vid;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
        goto out;
    }

    *fwd_map = e->fwd_map;
    *untag_map = e->untag_map;

out:
    close(skfd);
    return err;

}


int bcm_switch_getrxcntrs()
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWGETRXCOUNTERS;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
    }

    close(skfd);
    return err;
}

int bcm_switch_resetrxcntrs()
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWRESETRXCOUNTERS;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
    }

    close(skfd);
    return err;
}

/*
 * Function:
 * Description:
 * Parameters:
 * Returns:
 *  BCM_E_xxx
 */
int bcm_port_pbvlanmap_set(int unit, int port, int fwd_map)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWPBVLAN;
    e->port = port;
    e->fwd_map = fwd_map;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
    }

    close(skfd);
    return err;
}

/*
 * Function:
 * Description:
 * Parameters:
 * Returns:
 *  BCM_E_xxx
 */
int bcm_port_pbvlanmap_get(int unit, int port, int *fwd_map)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWPBVLAN;
    e->type = TYPE_GET;
    e->port = port;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
      goto out;
    }

    *fwd_map = e->fwd_map;

out:
    close(skfd);
    return err;
}

/* CoS API */

int bcm_cosq_config_get(int unit, int *numq)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWCOSCONF;
    e->type = TYPE_GET;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
      goto out;
    }

    *numq = e->numq;

out:
    close(skfd);
    return err;
}


int bcm_cosq_config_set(int unit, int numq)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWCOSCONF;
    e->numq = numq;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
    }

    close(skfd);
    return err;
}

int bcm_cosq_sched_get(int unit, int *mode, int *sp_endq,
                       int weights[BCM_COS_COUNT]) {
    int err;
    port_qos_sched_t qs;
    
    err = bcm_cosq_sched_get_X(unit, 0, weights, &qs);
    if (err == 0) {
        *mode    = qs.sched_mode;
        *sp_endq = qs.num_spq;
    }
    return err;
}
int bcm_cosq_sched_get_X(int unit, bcm_port_t port,
                       int weights[BCM_COS_COUNT], port_qos_sched_t *qs)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWCOSSCHED;
    e->type = TYPE_GET;
    e->unit = unit;
    e->port = port;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
      goto out;
    }
    *qs = e->port_qos_sched;
    memcpy(weights, e->weights, BCM_COS_COUNT * sizeof(int));

out:
    close(skfd);
    return err;
}

int bcm_cosq_sched_set(int unit, int mode, int sp_endq,
                        int weights[BCM_COS_COUNT])
{
    port_qos_sched_t qs;
    qs.sched_mode = mode;
    qs.num_spq = sp_endq;
    return bcm_cosq_sched_set_X(unit, 0, weights, &qs);
}
int bcm_cosq_sched_set_X(int unit, int port, 
                        int *weights, port_qos_sched_t *qs)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWCOSSCHED;
    e->type = TYPE_SET;
    e->port_qos_sched = *qs;
    memcpy(e->weights, weights, BCM_COS_COUNT * sizeof(int));
    e->unit = unit;
    e->port = port;
    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
    }

    close(skfd);
    return err;
}

/* Set the internal priority to egress queue mapping of the given port */
int bcm_cosq_port_mapping_get(int unit, bcm_port_t port, bcm_cos_t priority,
                              bcm_cos_queue_t *cosq)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWCOSPORTMAP;
    e->type = TYPE_GET;
    e->priority = priority;
    e->port = port;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
      goto out;
    }

    *cosq = (bcm_cos_queue_t)e->queue;

out:
    close(skfd);
    return err;
}

/* Get the internal priority to egress queue mapping of the given port */
int bcm_cosq_port_mapping_set(int unit, bcm_port_t port, bcm_cos_t priority,
                              bcm_cos_queue_t cosq)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWCOSPORTMAP;
    e->priority = priority;
    e->port = port;
    e->queue = cosq;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
    }

    close(skfd);
    return err;
}

/* Get the egress queue to Rx iuDMA channel (Rx on MIPS) mapping */
int bcm_cosq_rxchannel_mapping_get(int unit, bcm_cos_queue_t cosq, int *channel)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWCOSRXCHMAP;
    e->type = TYPE_GET;
    e->queue = (int)cosq;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
      goto out;
    }

    *channel = e->channel;

out:
    close(skfd);
    return err;
}

/* Set the egress queue to Rx iuDMA channel (Rx on MIPS) mapping */
int bcm_cosq_rxchannel_mapping_set(int unit, bcm_cos_queue_t cosq, int channel)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWCOSRXCHMAP;
    e->queue = (int)cosq;
    e->channel = channel;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
    }

    close(skfd);
    return err;
}

/* Get the Tx iuDMA channel (Tx from MIPS) to egress queue mapping */
int bcm_cosq_txchannel_mapping_get(int unit, int channel, bcm_cos_queue_t *cosq)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWCOSTXCHMAP;
    e->type = TYPE_GET;
    e->channel = channel;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
      goto out;
    }

    *cosq = (bcm_cos_queue_t)e->queue;

out:
    close(skfd);
    return err;
}

/* Set the Tx iuDMA channel (Tx from MIPS) to egress queue mapping */
int bcm_cosq_txchannel_mapping_set(int unit, int channel, bcm_cos_queue_t cosq)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWCOSTXCHMAP;
    e->channel = channel;
    e->queue = (int)cosq;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
    }

    close(skfd);
    return err;
}

/* Get the method used to specify the egress queue for Tx packets from MIPS */
int bcm_cosq_txq_selection_get(int unit, int *method)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWCOSTXQSEL;
    e->type = TYPE_GET;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
      goto out;
    }

    *method = e->ret_val;

out:
    close(skfd);
    return err;
}

/* Set the method used to specify the egress queue for Tx packets from MIPS */
int bcm_cosq_txq_selection_set(int unit, int method)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWCOSTXQSEL;
    e->type = TYPE_SET;
    e->val = method;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
    }

    close(skfd);
    return err;
}

/*
 * Function:
 *  bcm_cosq_priority_method_get
 * Description:
 * Retrieve the method for deciding on frame priority
 * Parameters:
 *  unit - RoboSwitch PCI device unit number (driver internal).
 *  method -
 *  PORT_QOS: Frame priority is based on the priority of port default tag
 *  MAC_QOS: Frame priority is based on the destination MAC address
 *  IEEE8021P_QOS: Frame priority is based on 802.1p field of the frame
 *  DIFFSERV_QOS: Frame priority is based on the diffserv field of the frame
 * Returns:
 *  BCM_E_NONE - Success.
 */
int bcm_cosq_priority_method_get(int unit, int port, int *method)
{
    return bcm_cosq_priority_method_get_X(unit, port, method, 0);
}
int bcm_cosq_priority_method_get_X(int unit, int port, int *method, int pkt_type_mask)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWCOSPRIORITYMETHOD;
    e->type = TYPE_GET;
    e->unit = unit;
    e->port = port;
    e->pkt_type_mask = pkt_type_mask;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
      goto out;
    }

    *method = e->ret_val;

out:
    close(skfd);
    return err;
}

/*
 * Function:
 *  bcm_cosq_priority_method_set
 * Description:
 *  Set the method for deciding on frame priority
 * Parameters:
 *  unit - RoboSwitch PCI device unit number (driver internal).
 *  method -
 *  PORT_QOS: Frame priority is based on the priority of port default tag
 *  MAC_QOS: Frame priority is based on the destination MAC address
 *  IEEE8021P_QOS: Frame priority is based on 802.1p field of the frame
 *  DIFFSERV_QOS: Frame priority is based on the diffserv field of the frame
 * Returns:
 *  BCM_E_NONE - Success.
 */
int bcm_cosq_priority_method_set(int unit, int port, int method)
{
     return bcm_cosq_priority_method_set_X(unit, port, method, 0);   
}
int bcm_cosq_priority_method_set_X(int unit, int port, int method, int pkt_type_mask)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWCOSPRIORITYMETHOD;
    e->type = TYPE_SET;
    e->val = method;
    e->unit = unit;
    e->port = port;
    e->pkt_type_mask = 0;
    e->pkt_type_mask = pkt_type_mask;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
    }

    close(skfd);
    return err;
}

/*
 * Function:
 *  bcm_cosq_pcp_priority_mapping_get
 * Description:
 *  Get PID to priority mapping
 * Parameters:
 *  unit - RoboSwitch PCI device unit number (driver internal).
 *  priority:  switch priority
 * Returns:
 *  BCM_E_NONE - Success.
 */
int bcm_cosq_pid_priority_mapping_get(int unit, int port, bcm_cos_t *priority)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWCOSPIDPRIOMAP;
    e->type = TYPE_GET;
    e->unit = unit;
    e->port = port;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        printf("ioctl command return error!\n");
    }

    *priority = (bcm_cos_t)e->priority;

    close(skfd);
    return err;
}
/*
 * Function:
 *  bcm_cosq_pid_priority_mapping_set
 * Description:
 *  Configure DSCP to priority mapping
 * Parameters:
 *  unit - RoboSwitch PCI device unit number (driver internal).
 *  priority:  switch priority
 * Returns:
 *  BCM_E_NONE - Success.
 */
int bcm_cosq_pid_priority_mapping_set(int unit, int port, bcm_cos_t priority)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWCOSPIDPRIOMAP;
    e->type = TYPE_SET;
    e->priority = priority;
    e->unit = unit;
    e->port = port;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        printf("ioctl command return error!\n");
    }

    close(skfd);
    return err;
}
/*
 * Function:
 *  bcm_cosq_pcp_priority_mapping_set
 * Description:
 *  Configure PCP to priority mapping
 * Parameters:
 *  unit - RoboSwitch PCI device unit number (driver internal).
 *  pcp:  3-bit pcp value
 *  priority:  switch priority
 * Returns:
 *  BCM_E_NONE - Success.
 */
int bcm_cosq_pcp_priority_mapping_set(int unit, int port, int pcp, bcm_cos_t priority)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWCOSPCPPRIOMAP;
    e->type = TYPE_SET;
    e->val = pcp;
    e->priority = priority;
    e->unit = unit;
    e->port = port;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        printf("ioctl command return error!\n");
    }

    close(skfd);
    return err;
}

/*
 * Function:
 *  bcm_cosq_pcp_priority_mapping_get
 * Description:
 *  Get PCP to priority mapping
 * Parameters:
 *  unit - RoboSwitch PCI device unit number (driver internal).
 *  pcp:  3-bit pcp value
 *  priority:  switch priority
 * Returns:
 *  BCM_E_NONE - Success.
 */
int bcm_cosq_pcp_priority_mapping_get(int unit, int port, int pcp, bcm_cos_t *priority)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWCOSPCPPRIOMAP;
    e->type = TYPE_GET;
    e->val = pcp;
    e->unit = unit;
    e->port = port;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        printf("ioctl command return error!\n");
    }

    *priority = (bcm_cos_t)e->priority;

    close(skfd);
    return err;
}
/*
 * Function:
 *  bcm_cosq_dscp_priority_mapping_set
 * Description:
 *  Configure DSCP to priority mapping
 * Parameters:
 *  unit - RoboSwitch PCI device unit number (driver internal).
 *  dscp:  6-bit dscp value
 *  priority:  switch priority
 * Returns:
 *  BCM_E_NONE - Success.
 */
int bcm_cosq_dscp_priority_mapping_set(int unit, int dscp, bcm_cos_t priority)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWCOSDSCPPRIOMAP;
    e->type = TYPE_SET;
    e->val = dscp;
    e->priority = priority;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        printf("ioctl command return error!\n");
    }

    close(skfd);
    return err;
}

/*
 * Function:
 *  bcm_cosq_dscp_priority_mapping_get
 * Description:
 *  Get DSCP to priority mapping
 * Parameters:
 *  unit - RoboSwitch PCI device unit number (driver internal).
 *  dscp:  6-bit dscp value
 *  priority:  switch priority
 * Returns:
 *  BCM_E_NONE - Success.
 */
int bcm_cosq_dscp_priority_mapping_get(int unit, int dscp, bcm_cos_t *priority)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWCOSDSCPPRIOMAP;
    e->type = TYPE_GET;
    e->val = dscp;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        printf("ioctl command return error!\n");
    }

    *priority = (bcm_cos_t)e->priority;

    close(skfd);
    return err;
}

/****************************************************************************/
/*  Statistics API                                                          */
/****************************************************************************/

/*
 * Function:
 *  bcm_stat_clear
 * Description:
 *  Clear the software statistics of given port
 * Parameters:
 *  unit - RoboSwitch PCI device unit number (driver internal).
 *  port - zero-based port number
 * Returns:
 *  BCM_E_NONE - Success.
 */
int bcm_stat_clear(int unit)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWSTATCLR;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
    }

    close(skfd);
    return err;
}


/*
 * Function:
 *  bcm_stat_port_clear
 * Description:
 *  Clear the port based statistics from the RoboSwitch port.
 * Parameters:
 *  unit - RoboSwitch PCI device unit number (driver internal).
 *  port - zero-based port number
 * Returns:
 *  BCM_E_NONE - Success.
 */
int bcm_stat_port_clear(int unit, bcm_port_t port)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWSTATPORTCLR;
    e->port = port;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
    }

    close(skfd);
    return err;
}

/*
 * Function:
 *  bcm_stat_sync
 * Description:
 *  Synchronize software counters with hardware
 * Parameters:
 *  unit - RoboSwitch PCI device unit number (driver internal).
 * Returns:
 *  BCM_E_NONE - Success.
 * Notes:
 *  Makes sure all counter hardware activity prior to the call to
 *  bcm_stat_sync is reflected in all bcm_stat_get calls that come
 *  after the call to bcm_stat_sync.
 */

int bcm_stat_sync(int unit)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWSTATSYNC;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
    }

    close(skfd);
    return err;
}

/*
 * Function:
 *  bcm_stat_get
 * Description:
 *  Get the specified statistic from the RoboSwitch
 * Parameters:
 *  unit - RoboSwitch PCI device unit number (driver internal).
 *  port - zero-based port number
 *  type - SNMP statistics type (see stat.h)
 *      val - (OUT) 64-bit counter value.
 * Returns:
 *  BCM_E_NONE - Success.
 * Notes:
 *  Some counters are implemented on a given port only when it is
 *  operating in a specific mode, for example, 10 or 100, and not
 *  1000. If the counter is not implemented on a given port, OR,
 *  on the port given its current operating mode, BCM_E_UNAVAIL
 *  is returned.
 */
int bcm_stat_get(int unit, bcm_port_t port,
        bcm_stat_val_t type, unsigned long long *value)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWSTATGET;
    e->port = port;
    e->counter_type = type;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr)) && (err != BCM_E_UNAVAIL)) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
        goto out;
    }

    *value = e->counter_val;

out:
    close(skfd);
    return err;
}

/*
 * Function:
 *  bcm_stat_clear_emac
 * Description:
 *  clear the specified statistic from the cached data in bcmenet
 * Parameters:
 *  unit - RoboSwitch PCI device unit number (driver internal).
 *  port - zero-based port number
 * Returns:
 *  BCM_E_NONE - Success.
 */
int bcm_stat_clear_emac(int unit, bcm_port_t port)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWEMACCLEAR;
    e->port = port;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr)) && (err != BCM_E_UNAVAIL)) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
        goto out;
    }

out:
    close(skfd);
    return err;
}


/*
 * Function:
 *  bcm_stat_get_emac
 * Description:
 *  Get the specified statistic from the cached data
 * Parameters:
 *  unit - RoboSwitch PCI device unit number (driver internal).
 *  port - zero-based port number
 *  value - (OUT) emac address to return data.
 * Returns:
 *  BCM_E_NONE - Success.
 */
int bcm_stat_get_emac(int unit, bcm_port_t port, struct emac_stats* value)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWEMACGET;
    e->port = port;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr)) && (err != BCM_E_UNAVAIL)) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
        goto out;
    }

    memcpy(value, &(e->emac_stats_s), sizeof(struct emac_stats));

out:
    close(skfd);
    return err;
}


/*
 * Function:
 *  bcm_stat_get_port
 * Description:
 *  Get the specified statistic from the cached data
 * Parameters:
 *  unit - RoboSwitch PCI device unit number (driver internal).
 *  port - zero-based port number
 *  value - (OUT) port address to return data.
 * Returns:
 *  BCM_E_NONE - Success.
 */
int bcm_stat_get_port(int unit, bcm_port_t port, struct rdpa_port_stats* value)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    memset(&ifdata,0,sizeof(struct ethswctl_data));
    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWSTATPORTGET;
    e->port = port;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr)) && (err != BCM_E_UNAVAIL)) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
        goto out;
    }

    memcpy(value, &(e->rdpa_port_stats_s), sizeof(struct rdpa_port_stats));

out:
    close(skfd);
    return err;
}


/*
 * Function:
 *  bcm_stat_get32
 * Description:
 *  Get the specified statistic from the RoboSwitch
 * Parameters:
 *  unit - RoboSwitch PCI device unit number (driver internal).
 *  port - zero-based port number
 *  type - SNMP statistics type (see stat.h)
 *      val - (OUT) 32-bit counter value.
 * Returns:
 *  BCM_E_NONE - Success.
 * Notes:
 *  Same as bcm_stat_get, except converts result to 32-bit.
 */
int bcm_stat_get32(int unit, bcm_port_t port,
          bcm_stat_val_t type, unsigned int *value)
{
    int err = 0;
    unsigned long long val64;

    err = bcm_stat_get(unit, port, type, &val64);

    *value = (unsigned int) val64;

    return err;
}

int bcm_arl_read(int unit, char *mac, bcm_vlan_t vid, unsigned short *value) /* Deprecated API */
{
    char _mac[6];
    int i;
    *value = -1;

    for(i=0; i<6; i++)
    {
        _mac[i] = mac[5-i];
    }
    return bcm_arl_read2(&unit, _mac, &vid, value);
}

int bcm_arl_read2(int *unit, char *mac, bcm_vlan_t *vid, unsigned short *value)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->type = TYPE_GET;
    e->op = ETHSWARLACCESS;
    e->vid = (short)*vid;
    e->val = (short)*value;
    memcpy(e->mac, mac, 6);
    e->unit = *unit;

    err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr);
    *value = e->val;
    *vid = e->vid;
    *unit = e->unit;

    close(skfd);
    return err;
}

int bcm_arl_write(int unit, char *mac, bcm_vlan_t vid, unsigned short value) /* Deprecated API */
{
    char _mac[6];
    int i;

    for(i=0; i<6; i++)
    {
        _mac[i] = mac[5-i];
    }
    return bcm_arl_write2(unit, _mac, vid, value|(1<<31));
}

int bcm_arl_write2(int unit, char *mac, bcm_vlan_t vid, int val)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->type = TYPE_SET;
    e->op = ETHSWARLACCESS;
    e->vid = vid;
    memcpy(e->mac, mac, 6);
    e->val = val;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr)) && (err != BCM_E_UNAVAIL)) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
        goto out;
    }

out:
    close(skfd);
    return err;
}

int bcm_arl_dump(int unit)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->type = TYPE_DUMP;
    e->op = ETHSWARLACCESS;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr)) && (err != BCM_E_UNAVAIL)) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
        goto out;
    }

out:
    close(skfd);
    return err;
}

/*  
    ARL Flush: flush the dynamic ARL entries
    No static entries will be blindly removed anymore in driver.
    If static entries need to be reconfigured due to certain reason,
    use bcm_arl_write() to set it to correct one.
*/
int bcm_arl_flush(int unit)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->type = TYPE_FLUSH;
    e->op = ETHSWARLACCESS;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr)) && (err != BCM_E_UNAVAIL)) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
        goto out;
    }

out:
    close(skfd);
    return err;
}

/*
 * Function:
 *    bcm_reg_write
 * Description:
 *  Write to a switch register
 * Parameters:
 *    addr = offset
 *    len = length of register
 *    data = ptr to value to be written to register
 * Returns:
 *    BCM_E_NONE - Success.
 */
int bcm_reg_write(unsigned int addr, char* data, int len)
{
    return bcm_reg_write_X(0,addr,data,len);
}
int bcm_reg_write_X(int unit, unsigned int addr, char* data, int len)
{
    int skfd, err = 0, i;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    DBG(printf("write: addr = 0x%x, len = %d; data = ", addr, len););
    for (i=len-1; i>=0; i--) {
        DBG(printf("%2x:", data[i]););
    }
    DBG(printf("\n"););

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWREGACCESS;
    e->type = TYPE_SET;
    e->offset = addr;
    e->length = len;
    e->unit = unit;
    memcpy(e->data, data, len);

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        printf("ioctl command return error!\n");
        goto out;
    }

out:
    close(skfd);
    return err;
}

/*
 * Function:
 *    bcm_reg_read
 * Description:
 *  Read from a switch register
 * Parameters:
 *    addr = offset
 *    len = length of register
 *    data = ptr to value read from register
 * Returns:
 *    BCM_E_NONE - Success.
 */
int bcm_reg_read(unsigned int addr, char* data, int len)
{
    return bcm_reg_read_X(0,addr,data,len);
}
int bcm_reg_read_X(int unit, unsigned int addr, char* data, int len)
{
    int skfd, err = 0, i;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWREGACCESS;
    e->type = TYPE_GET;
    e->offset = addr;
    e->length = len;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        printf("ioctl command return error!\n");
        goto out;
    }

    memcpy(data, e->data, len);

    DBG(printf("read: addr = 0x%x, len = %d; data = ", addr, len););
    for (i=len-1; i>=0; i--) {
        DBG(printf("%2x:", data[i]););
    }
    DBG(printf("\n"););

out:
    close(skfd);
    return err;
}

/*
 * Function:
 *    bcm_spi_write
 * Description:
 *  Write to a switch register
 * Parameters:
 *    unit - RoboSwitch device unit number (driver internal).
 *    page = switch page
 *    offset = offset of reg within the page
 *    len = length of register
 *    val = value to be written to register
 * Returns:
 *    BCM_E_NONE - Success.
 */
int bcm_spi_write(unsigned int addr, char *data, int len)
{
    int i, skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    DBG(printf("len = %d; data = ", len););
    for (i=len; i>=0; i--) {
        DBG(printf("%2x:", (unsigned char)data[i]););
    }
    DBG(printf("\n"););

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWSPIACCESS;
    e->type = TYPE_SET;
    e->offset = addr;
    e->length = len;
    memcpy(e->data, data, len);
    DBG(printf("data = %02x%02x%02x%02x %02x%02x%02x%02x \n", e->data[7],
      e->data[6], e->data[5], e->data[4], e->data[3], e->data[2],
      e->data[1], e->data[0]););

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        printf("ioctl command return error!\n");
        goto out;
    }

out:
    close(skfd);
    return err;
}

/*
 * Function:
 *    bcm_spi_read
 * Description:
 *  Read from a switch register
 * Parameters:
 *    unit - RoboSwitch device unit number (driver internal).
 *    page = switch page
 *    offset = offset of reg within the page
 *    len = length of register
 *    val = value read from register
 * Returns:
 *    BCM_E_NONE - Success.
 */
int bcm_spi_read(unsigned int addr, char *data, int len)
{
    int i, skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWSPIACCESS;
    e->type = TYPE_GET;
    e->offset = addr;
    e->length = len;
    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        printf("ioctl command return error!\n");
        goto out;
    }

    memcpy(data, e->data, len);
    DBG(printf("data = "););
    for (i=len; i>=0; i--) {
        DBG(printf("%2x:", (unsigned char)e->data[i]););
    }
    DBG(printf("\n"););

out:
    close(skfd);
    return err;
}

/*
 * Function:
 *    bcm_pseudo_mdio_write
 * Description:
 *  Write to a switch register
 * Parameters:
 *    unit - RoboSwitch device unit number (driver internal).
 *    page = switch page
 *    offset = offset of reg within the page
 *    len = length of register
 *    val = value to be written to register
 * Returns:
 *    BCM_E_NONE - Success.
 */
int bcm_pseudo_mdio_write(unsigned int addr, char* data, int len)
{
    int i, skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    DBG(printf("addr = %d, len = %d \n", addr, len););
    DBG(printf("data = "););
    for (i=len; i>=0; i--) {
        DBG(printf("%2x:", (unsigned char)data[i]););
    }
    DBG(printf("\n"););

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWPSEUDOMDIOACCESS;
    e->type = TYPE_SET;
    e->offset = addr;
    e->length = len;
    memcpy(e->data, data, len);
    DBG(printf("data = %02x%02x%02x%02x %02x%02x%02x%02x \n", e->data[7],
      e->data[6], e->data[5], e->data[4], e->data[3], e->data[2],
      e->data[1], e->data[0]););

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        printf("ioctl command return error!\n");
        goto out;
    }

out:
    close(skfd);
    return err;
}


/*
 * Function:
 *    bcm_pseudo_mdio_read
 * Description:
 *  Read from a switch register
 * Parameters:
 *    unit - RoboSwitch device unit number (driver internal).
 *    page = switch page
 *    offset = offset of reg within the page
 *    len = length of register
 *    val = value read from register
 * Returns:
 *    BCM_E_NONE - Success.
 */
int bcm_pseudo_mdio_read(unsigned int addr, char* data, int len)
{
    int i, skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    DBG(printf("addr = %x, len = %d \n", addr, len););

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWPSEUDOMDIOACCESS;
    e->type = TYPE_GET;
    e->offset = addr;
    e->length = len;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        printf("ioctl command return error!\n");
        goto out;
    }

    memcpy(data, e->data, len);
    DBG(printf("data = "););
    for (i=len; i>=0; i--) {
        DBG(printf("%2x:", (unsigned char)e->data[i]););
    }
    DBG(printf("\n"););

out:
    close(skfd);
    return err;
}

/*
 * Function:
 *    bcm_get_switch_info
 * Description:
 *     Get switch and board info
 * Parameters:
 *    switch_id - Switch ID number.
 *    vend_dev_id - Switch Vendor and Device IDs
 *    bus_type =  How switch is accessed (SPI or MDIO or Direct)
 *    spi_id = SPI ID for SPI accesses
 *    chip_id =  Chip ID for SPI accesses
 * Returns:
 *    BCM_E_NONE - Success.
 */
int bcm_get_switch_info(int switch_id, unsigned int *vendor_id, unsigned int *dev_id,
  unsigned int *rev_id, int *bus_type, unsigned int *spi_id, unsigned int *chip_id,
  unsigned int *pbmp, unsigned int *phypbmp, int *epon_port)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    DBG(printf("switch_id = %d \n", switch_id););

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWINFO;
    e->type = TYPE_GET;
    e->val = switch_id;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        printf("ioctl command return error!\n");
        goto out;
    }

    *vendor_id = e->vendor_id;
    *dev_id = e->dev_id;
    *rev_id = e->rev_id;
    *bus_type = e->ret_val;
    *spi_id = e->spi_id;
    *chip_id = e->chip_id;
    *pbmp = e->port_map;
    *phypbmp = e->phy_portmap;
	*epon_port = e->epon_port;
    DBG(printf("vend_id = 0x%x, dev_id = 0x%x, rev_id = 0x%x, bus_type = %d;"
     " spi_id = %d; chip_id = %d; pbmp = 0x%x; phypbmp = 0x%x; epon_port=%d \n", *vendor_id, *dev_id,
     *rev_id, *bus_type, *spi_id, *chip_id, *pbmp, *phypbmp, *epon_port););

out:
    close(skfd);
    return err;
}

/*
 * Function:
 *    bcm_set_linkstatus
 * Description:
 *  Notify link status to Enet driver
 * Parameters:
 *    port - port.
 *    linkstatus - link status.
 * Returns:
 *    BCM_E_NONE - Success.
 */
int bcm_set_linkstatus(int unit, int port, int linkstatus, int speed, int duplex)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWLINKSTATUS;
    e->type = TYPE_SET;
    e->unit = unit;
    e->port = port;
    e->status = linkstatus;
    e->speed = speed;
    e->duplex = duplex;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        printf("ioctl command return error!\n");
        goto out;
    }

out:
    close(skfd);
    return err;
}

/*
 * Function:
 *    bcm_get_linkstatus
 * Description:
 *  Get link status from Enet driver
 * Parameters:
 *    port - port.
 *    linkstatus - link status.
 * Returns:
 *    BCM_E_NONE - Success.
 */
int bcm_get_linkstatus(int unit, int port, int *linkstatus)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWLINKSTATUS;
    e->type = TYPE_GET;
    e->unit = unit;
    e->port = port;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        printf("ioctl command return error!\n");
        goto out;
    }
	
    *linkstatus = e->status;

out:
    close(skfd);
    return err;
}

/*
 * Function:
 *    bcm_set_extphylinkstatus
 * Description:
 *  Notify link status of extern phy to Enet driver
 * Parameters:
 *    unit - unit
 *    port - port.
 *    phyType - phyType
 *    linkstatus - link status.
 *    speed - speed
 *    duplex - duplex
 * Returns:
 *    BCM_E_NONE - Success.
 */
int bcm_set_extphylinkstatus(int unit, int port, int phyType, int linkstatus, int speed, int duplex)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    skfd=ethswctl_init(&ifr);
    if (skfd < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op     = ETHSWEXTPHYLINKSTATUS;
    e->type   = TYPE_SET;
    e->unit   = unit;
    e->port   = port;
    e->status = linkstatus;
    e->val    = phyType;
    e->speed  = speed;
    e->duplex = duplex;

    err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr);
    if(err) {
        printf("ioctl command return error!\n");
    }

    close(skfd);
    return err;
}

/*
 * Function:
 *  bcm_port_traffic_control_set
 * Description:
 *  Enable/Disable tx/rx of a switch port
 * Parameters:
 *  unit - RoboSwitch PCI device unit number (driver internal).
 *  port - Port number
 *  ctrl_map: bit0 = rx_disable (1 = disable rx; 0 = enable rx)
 *            bit1 = tx_disable (1 = disable tx; 0 = enable tx)
 * Returns:
 *  BCM_E_NONE - Success.
 */
int bcm_port_traffic_control_set(int unit, bcm_port_t port, int ctrl_map)
{
  int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

  if ((skfd=ethswctl_init(&ifr)) < 0) {
    printf("ethswctl_init failed. \n");
        return skfd;
  }
    ifr.ifr_data = (char *)&ifdata;

  e->op = ETHSWPORTTRAFFICCTRL;
  e->type = TYPE_SET;
  e->val = ctrl_map;
  e->port = port;
  e->unit = unit;

  if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
    printf("ioctl command return error!\n");
    goto out;
  }

out:
    close(skfd);
    return err;
}

/*
 * Function:
 *  bcm_port_traffic_control_get
 * Description:
 *  Get Enable/Disable status of tx/rx of a switch port
 * Parameters:
 *  unit - RoboSwitch PCI device unit number (driver internal).
 *  port - Port number
 *  ctrl_map: bit0 = rx_disable (1 = disable rx; 0 = enable rx)
 *            bit1 = tx_disable (1 = disable tx; 0 = enable tx)
 * Returns:
 *  BCM_E_NONE - Success.
 */
int bcm_port_traffic_control_get(int unit, bcm_port_t port, int *ctrl_map)
{
  int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

  if ((skfd=ethswctl_init(&ifr)) < 0) {
    printf("ethswctl_init failed. \n");
        return skfd;
  }
    ifr.ifr_data = (char *)&ifdata;

  e->op = ETHSWPORTTRAFFICCTRL;
  e->type = TYPE_GET;
  e->port = port;
  e->unit = unit;

  if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
    printf("ioctl command return error!\n");
  }

#if defined(CONFIG_BCM96838) || defined(CONFIG_BCM96848)
	*ctrl_map = 0;
	if (e->ret_val == TRUE)
	{
		*ctrl_map &= ~PORT_RXDISABLE;
		*ctrl_map &= ~PORT_TXDISABLE;
	}
	else if (e->ret_val == FALSE)
	{
		*ctrl_map |= PORT_RXDISABLE;
		*ctrl_map |= PORT_TXDISABLE;
	}
#else	
	*ctrl_map = e->ret_val;
#endif		

    close(skfd);
    return err;
}

/*
 * Function:
 *  bcm_port_loopback_set
 * Description:
 *  Enable/Disable of loopback of USB port or LAN port Phy
 * Parameters:
 *  unit - RoboSwitch PCI device unit number (driver internal).
 *  status:  1 = Enable loopback; 0 = Disable loopback
 * Returns:
 *  BCM_E_NONE - Success.
 */
int bcm_port_loopback_set(int unit, bcm_port_t port, int status)
{
  int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

  if ((skfd=ethswctl_init(&ifr)) < 0) {
    printf("ethswctl_init failed. \n");
        return skfd;
  }
    ifr.ifr_data = (char *)&ifdata;

  e->op = ETHSWPORTLOOPBACK;
  e->type = TYPE_SET;
  e->val = status;
  e->port = port;
  e->unit = unit;

  if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
    printf("ioctl command return error!\n");
    goto out;
  }

out:
    close(skfd);
    return err;
}

/*
 * Function:
 *  bcm_port_loopback_get
 * Description:
 *  Get loopback status of USB port or LAN port Phy
 * Parameters:
 *  unit - RoboSwitch PCI device unit number (driver internal).
 *  status:  1 = Enable loopback; 0 = Disable loopback
 * Returns:
 *  BCM_E_NONE - Success.
 */
int bcm_port_loopback_get(int unit, bcm_port_t port, int *status)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

  if ((skfd=ethswctl_init(&ifr)) < 0) {
    printf("ethswctl_init failed. \n");
        return skfd;
  }
    ifr.ifr_data = (char *)&ifdata;

  e->op = ETHSWPORTLOOPBACK;
  e->type = TYPE_GET;
  e->port = port;
  e->unit = unit;

  if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
    printf("ioctl command return error!\n");
  }

    *status = e->ret_val;

    close(skfd);
    return err;
}

/*
 * Function:
 *    bcm_packet_padding_set
 * Description:
 *  Configure enable/disable of packet padding and min length with padding
 * Parameters:
 *    unit - RoboSwitch PCI device unit number (driver internal).
 *    enable - 1 = Enable padding; 0 = Disable padding
 *    length -  min length of the packet after padding. valid only when enable=1
 * Returns:
 *    BCM_E_NONE - Success.
 */
int bcm_packet_padding_set(int unit, int enable, int length)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWPKTPAD;
    e->type = TYPE_SET;
    e->val = enable;
    e->length = length;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        printf("ioctl command return error!\n");
        goto out;
    }

out:
    close(skfd);
    return err;
}

/*
 * Function:
 *    bcm_packet_padding_get
 * Description:
 *  Get enable/disable of packet padding and min length with padding
 * Parameters:
 *    unit - RoboSwitch PCI device unit number (driver internal).
 *    enable - 1 = padding enabled; 0 = padding disabled
 *    length -  min length of the packet after padding. valid only when enable=1
 * Returns:
 *    BCM_E_NONE - Success.
 */
int bcm_packet_padding_get(int unit, int *enable, int *length)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWPKTPAD;
    e->type = TYPE_GET;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        printf("ioctl command return error!\n");
    }

    *enable = e->ret_val;
    *length = e->length;

    close(skfd);
    return err;
}


/*
 * Function:
 *  bcm_port_jumbo_control_set
 * Description:
 *  Set jumbo accept/reject control of selected port(s)
 * Parameters:
 *  port - Port number 9(ALL), 8(MIPS), 7(GPON), 6(USB), 5(MOCA), 4(GPON_SERDES), 3(GMII_2), 2(GMII_1), 1(GPHY_1), 0(GPHY_0)
 *  ctrlValPtr - pointer to result
 * Returns:
 *  BCM_E_NONE - Success.
 */
int bcm_port_jumbo_control_set(int unit, bcm_port_t port, int* ctrlValPtr) // bill
{
  int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

  if ((skfd=ethswctl_init(&ifr)) < 0) {
    printf("ethswctl_init failed. \n");
        return skfd;
  }
    ifr.ifr_data = (char *)&ifdata;

  e->op = ETHSWJUMBO;
  e->type = TYPE_SET;
  e->val = *ctrlValPtr;
  e->unit = unit;
  e->port = port;

  if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
    printf("ioctl command return error!\n");
  }

  *ctrlValPtr = e->ret_val;

  close(skfd);
  return err;
}

/*
 * Function:
 *  bcm_port_jumbo_control_get
 * Description:
 *  Get jumbo accept/reject status of selected port(s)
 * Parameters:
 *  port - Port number 9(ALL), 8(MIPS), 7(GPON), 6(USB), 5(MOCA), 4(GPON_SERDES), 3(GMII_2), 2(GMII_1), 1(GPHY_1), 0(GPHY_0)
 *  ctrlValPtr - pointer to result
 * Returns:
 *  BCM_E_NONE - Success.
 */
int bcm_port_jumbo_control_get(int unit, bcm_port_t port, int *ctrlValPtr) // bill
{
  int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

  if ((skfd=ethswctl_init(&ifr)) < 0) {
    printf("ethswctl_init failed. \n");
        return skfd;
  }
    ifr.ifr_data = (char *)&ifdata;

  e->op = ETHSWJUMBO;
  e->type = TYPE_GET;
  e->unit = unit;
  e->port = port;

  if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
    printf("ioctl command return error!\n");
  }

  *ctrlValPtr = e->ret_val;

  close(skfd);
  return err;
}

/*
 * Function:
 *    bcm_ethsw_kernel_poll
 * Description:
 *  Polling is now done in userspace,
 *  but some work remains in kernel space
 * Parameters:
 *    void
 * Returns:
 *    BCM_E_NONE - Success.
 */
int bcm_ethsw_kernel_poll(struct mdk_kernel_poll *mdk_kernel_poll)
{
    int skfd, err;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWKERNELPOLL;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        printf("ioctl command return error!\n");
    }
    close(skfd);

    memcpy(mdk_kernel_poll, &e->mdk_kernel_poll, sizeof(*mdk_kernel_poll));
    return err;
}

int linux_user_spi_read(void *dvc, unsigned int addr, unsigned char *data, unsigned int len)
{
    return bcm_spi_read(addr, (char *)data, (int)len);
}

int linux_user_spi_write(void *dvc, unsigned int addr, const unsigned char *data, unsigned int len)
{
    return bcm_spi_write(addr, (char *)data, (int)len);
}

int linux_user_mdio_read(void *dvc, unsigned int addr, unsigned char *data, unsigned int len)
{
    return bcm_pseudo_mdio_read(addr, (char *)data, (int)len);
}

int linux_user_mdio_write(void *dvc, unsigned int addr, const unsigned char *data, unsigned int len)
{
    return bcm_pseudo_mdio_write(addr, (char *)data, (int)len);
}

int linux_user_ubus_read(void *dvc, unsigned int addr, unsigned char *data, unsigned int len)
{
    return bcm_reg_read(addr, (char *)data, (int)len);
}

int linux_user_ubus_write(void *dvc, unsigned int addr, const unsigned char *data, unsigned int len)
{
    return bcm_reg_write(addr, (char *)data, (int)len);
}

int linux_user_mmap_read(void *dvc, unsigned int addr, unsigned char *data, unsigned int len)
{
    return bcm_reg_read_X(1, addr, (char *)data, (int)len); /* FIXME - the unit should come from the caller */
}

int linux_user_mmap_write(void *dvc, unsigned int addr, const unsigned char *data, unsigned int len)
{
    return bcm_reg_write_X(1, addr, (char *)data, (int)len); /* FIXME - the unit should come from the caller */
}

/* Get the Phy Config from Board Params */
int bcm_phy_config_get(int unit, bcm_port_t port, int *phy_config)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
          return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWPHYCFG;
    e->type = TYPE_GET;
    e->port = port;
    e->val = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
      printf("ioctl command return error!\n");
    }

    if (e->ret_val == -1) {
        err = -1;
    } else {
        *phy_config = e->phycfg;
    }

    close(skfd);
    return err;
}

int bcm_phy_mode_getV(char *ifname, int *speed, int *duplex)
{
    int  err = 0;
    int unit;
    bcm_port_t port;

    err = bcm_enet_map_ifname_to_unit_port(ifname, &unit, &port);
    if(err) return err;

    return bcm_phy_mode_get(unit, port, speed, duplex);
}

int bcm_phy_mode_setV(char *ifname, int speed, int duplex)
{
    int err = 0;
    int unit;
    bcm_port_t port;

    err = bcm_enet_map_ifname_to_unit_port(ifname, &unit, &port);
    if(err) return err;

    return bcm_phy_mode_set(unit, port, speed, duplex);

}
/*
 * Function:
 *    bcm_phy_mode_set
 * Description:
 *  Set phy mode
 * Parameters:
 *    unit - RoboSwitch PCI device unit number (driver internal).
 *    port - port.
 *    speed - 0 is auto or 10, 100, 1000, 2500, 5000, 10000
 *    duplex - 0: full, 1:half
 * Returns:
 *    BCM_E_NONE - Success.
 */
int bcm_phy_mode_set(int unit, int port, int speed, int duplex)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWPHYMODE;
    e->type = TYPE_SET;
    e->unit = unit;
    e->port = port;
    e->speed = speed;
    e->duplex = duplex;
    e->unit = unit;
    e->addressing_flag = 0;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        printf("ioctl command return error!\n");
        goto out;
    }

out:
    close(skfd);
    return err;
}

/*
 * Function:
 *    bcm_phy_mode_get
 * Description:
 *  Get phy mode
 * Parameters:
 *    port - port.
 * Returns:
 *    speed - 0 is auto or 10, 100, 1000, 2500, 5000, 10000
 *    duplex - 0: full, 1:half
 *    BCM_E_NONE - Success.
 */
int bcm_phy_mode_get(int unit, int port, int *speed, int *duplex)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWPHYMODE;
    e->type = TYPE_GET;
    e->port = port;
    e->unit = unit;
    e->addressing_flag = 0;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        printf("ioctl command return error!\n");
        goto out;
    }

    if (e->ret_val != -1) {
        *speed = e->speed;
        *duplex = e->duplex;
    }
out:
    close(skfd);
    return err;
}

/*
 * Function:
 *    bcm_ifname_get
 * Description:
 *  Get Linux interface name for a given port
 * Parameters:
 *    port - port.
 * Returns:
*     name of interface associated with the port
 *    BCM_E_NONE - Success.
 */
int bcm_ifname_get(int unit, int port, char *name)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWGETIFNAME;
    e->type = TYPE_GET;
    e->port = port;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        printf("ioctl command return error!\n");
        goto out;
    }

    if (e->ret_val != -1) {
       strcpy(name, e->ifname);
    }
out:
    close(skfd);
    return err;
}

/*
 * Function:
 *  bcm_multiport_set
 * Description:
 *  Set Multicast address in Switch Multiport register
 * Parameters:
 *  unit - RoboSwitch PCI device unit number (set to 0)
 *  mac  - MAC address to be added to second multiport register
 * Returns:
 *  BCM_E_NONE - Success.
 */
int bcm_multiport_set(int unit, unsigned char *mac)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->type = TYPE_SET;
    e->op = ETHSWMULTIPORT;
    memcpy(e->mac, mac, 6);
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr)) && (err != BCM_E_UNAVAIL)) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
        goto out;
    }

out:
    close(skfd);
    return err;
}

int bcm_dos_ctrl_set(int unit, struct bcm_dos_ctrl_params *pDosCtrlParams)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->type = TYPE_SET;
    e->op = ETHSWDOSCTRL;
	e->unit = unit;
	memset(&(e->dosCtrl),0,sizeof(e->dosCtrl));
	/* Taking short-cut : Not following BCM coding guidelines */
	if (pDosCtrlParams->ip_lan_drop_en) e->dosCtrl.ip_lan_drop_en = 1;
	if (pDosCtrlParams->tcp_blat_drop_en) e->dosCtrl.tcp_blat_drop_en = 1;
	if (pDosCtrlParams->udp_blat_drop_en) e->dosCtrl.udp_blat_drop_en = 1;
	if (pDosCtrlParams->tcp_null_scan_drop_en) e->dosCtrl.tcp_null_scan_drop_en = 1;
	if (pDosCtrlParams->tcp_xmas_scan_drop_en) e->dosCtrl.tcp_xmas_scan_drop_en = 1;
	if (pDosCtrlParams->tcp_synfin_scan_drop_en) e->dosCtrl.tcp_synfin_scan_drop_en = 1;
	if (pDosCtrlParams->tcp_synerr_drop_en) e->dosCtrl.tcp_synerr_drop_en = 1;
	if (pDosCtrlParams->tcp_shorthdr_drop_en) e->dosCtrl.tcp_shorthdr_drop_en = 1;
	if (pDosCtrlParams->tcp_fragerr_drop_en) e->dosCtrl.tcp_fragerr_drop_en = 1;
	if (pDosCtrlParams->icmpv4_frag_drop_en) e->dosCtrl.icmpv4_frag_drop_en = 1;
	if (pDosCtrlParams->icmpv6_frag_drop_en) e->dosCtrl.icmpv6_frag_drop_en = 1;
	if (pDosCtrlParams->icmpv4_longping_drop_en) e->dosCtrl.icmpv4_longping_drop_en = 1;
	if (pDosCtrlParams->icmpv6_longping_drop_en) e->dosCtrl.icmpv6_longping_drop_en = 1;
	if (pDosCtrlParams->dos_disable_lrn) e->dosCtrl.dos_disable_lrn = 1;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr)) && (err != BCM_E_UNAVAIL)) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
        goto out;
    }

out:
    close(skfd);
    return err;
}
int bcm_dos_ctrl_get(int unit, struct bcm_dos_ctrl_params *pDosCtrlParams)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0)
    {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->type = TYPE_GET;
    e->op = ETHSWDOSCTRL;
    e->unit = unit;
    memset(&(e->dosCtrl),0,sizeof(e->dosCtrl));

    if ((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr)) && (err != BCM_E_UNAVAIL))
    {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
        goto out;
    }
    /* Taking short-cut : Not following BCM coding guidelines */
    if (e->dosCtrl.ip_lan_drop_en) pDosCtrlParams->ip_lan_drop_en = 1;
    if (e->dosCtrl.tcp_blat_drop_en) pDosCtrlParams->tcp_blat_drop_en = 1;
    if (e->dosCtrl.udp_blat_drop_en) pDosCtrlParams->udp_blat_drop_en = 1;
    if (e->dosCtrl.tcp_null_scan_drop_en) pDosCtrlParams->tcp_null_scan_drop_en = 1;
    if (e->dosCtrl.tcp_xmas_scan_drop_en) pDosCtrlParams->tcp_xmas_scan_drop_en = 1;
    if (e->dosCtrl.tcp_synfin_scan_drop_en) pDosCtrlParams->tcp_synfin_scan_drop_en = 1;
    if (e->dosCtrl.tcp_synerr_drop_en) pDosCtrlParams->tcp_synerr_drop_en = 1;
    if (e->dosCtrl.tcp_shorthdr_drop_en) pDosCtrlParams->tcp_shorthdr_drop_en = 1;
    if (e->dosCtrl.tcp_fragerr_drop_en) pDosCtrlParams->tcp_fragerr_drop_en = 1;
    if (e->dosCtrl.icmpv4_frag_drop_en) pDosCtrlParams->icmpv4_frag_drop_en = 1;
    if (e->dosCtrl.icmpv6_frag_drop_en) pDosCtrlParams->icmpv6_frag_drop_en = 1;
    if (e->dosCtrl.icmpv4_longping_drop_en) pDosCtrlParams->icmpv4_longping_drop_en = 1;
    if (e->dosCtrl.icmpv6_longping_drop_en) pDosCtrlParams->icmpv6_longping_drop_en = 1;
    if (e->dosCtrl.dos_disable_lrn) pDosCtrlParams->dos_disable_lrn = 1;

    out:
    close(skfd);
    return err;
}

/*
 * Function:
 *    bcm_enet_driver_wan_interface_set
 * Description:
 *  Set  port wan mode
 * Parameters:
 *    ifname - interface name
 *    val    - boolean, wan or no wan mode.
 * Returns:
 *    BCM_E_NONE - Success.
 */

/* Set Clear an ethernet interface as wan port */
int bcm_enet_driver_wan_interface_set(char *ifname, unsigned int val)
{
    int skfd;
    struct ifreq ifr;

    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
    ifr.ifr_name[IFNAMSIZ - 1] = '\0';
    if ((skfd=ethswctl_open_socket(&ifr)) < 0) {
      printf("ethswctl_open_socket failed. \n");
        return -1;
    }
    ifr.ifr_data = (void *)val;
    if (ioctl(skfd, SIOCSWANPORT, &ifr) < 0)
    {
         printf(" %s ioctl SIOCSWANPORT(%04x) %s returns error! (%d/%s)",
                      __FUNCTION__, SIOCSWANPORT,  ifr.ifr_name, errno, strerror(errno));
         close(skfd);
         return -1;
    }
    close(skfd);
    return 0;
}


/*
 * Function:
 *    bcm_enet_driver_wan_interface_get
 * Description:
 *  Get all interfaces that have wan set.
 * Parameters:
 *    ifname - user buffer to return interface names
 *    sz    -  buffer length.
 * Returns:
 *    BCM_E_NONE - Success.
 */
int bcm_enet_driver_get_port_list_name(char *ifname, unsigned int sz, int ioctlVal, char *ioctlName)
{
    int skfd;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;
    /*
     * after changing other callers of SIOCGWANPORT ioctl,
     * pass sz for buffer length and enforce no buffer overruns.
     */
    if (!ifname) {
        return -1;
    }
    strcpy(ifr.ifr_name, "bcmsw");
    if ((skfd=ethswctl_open_socket(&ifr)) < 0) {
      printf("ethswctl_open_socket failed. \n");
        return -1;
    }
    ifr.ifr_data = &ifdata;
    e->up_len.uptr = ifname;
    e->up_len.len  = sz;

    if (ioctl(skfd, ioctlVal, &ifr) < 0)
    {
         printf("%s ioctl %s returns error!", __FUNCTION__, ioctlName);
         close(skfd);
         return -1;
    }

    return 0;
}
/*
 * Function:
 *    bcm_enet_debug_set
 * Description:
 *  Set Enet Driver Debug Level
 * Returns:
 *    BCM_E_NONE - Success.
 */
int bcm_enet_debug_set(int unit, int level)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWDEBUG;
    e->type = TYPE_SET;
    e->val = level;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        printf("ioctl command return error!\n");
        goto out;
    }

out:
    close(skfd);
    return err;
}

/*
 * Function:
 *    bcm_enet_debug_get
 * Description:
 *    Get the Enet Driver log level
 *  Get phy mode
 * Returns:
 *    BCM_E_NONE - Success.
 */
int bcm_enet_debug_get(int unit, int *level)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWDEBUG;
    e->type = TYPE_GET;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        printf("ioctl command return error!\n");
        goto out;
    }

    if (e->ret_val != -1) {
        *level = e->ret_val;
    }
out:
    close(skfd);
    return err;
}

/*
 * Function:
 *    bcm_phy_autoneg_info_get
 * Description:
 *  Get autoneg info for port
 * Parameters:
 *    unit - unit
 *    port - port.
 * Returns:
 *    autoneg - 0 is disabled and 1 is enabled
 *    local_cap - local capability for port
 *    ad_cap  -  advertised capability for port
 *    BCM_E_NONE - Success.
 */
 
int bcm_phy_autoneg_info_get(int unit, int port, unsigned char *autoneg, unsigned short *local_cap, unsigned short* ad_cap)
{
	int skfd, err = 0;
	struct ifreq ifr;
	struct ethswctl_data ifdata;
	struct ethswctl_data *e = &ifdata;	

	if ((skfd=ethswctl_init(&ifr)) < 0) {
		printf("ethswctl_init failed. \n");
		return skfd;
	}
	memset(&ifdata, 0, sizeof(ifdata));
	ifr.ifr_data = (char *)&ifdata;

	e->op = ETHSWPHYAUTONEG;
	e->type = TYPE_GET;
	e->port = port;
	e->unit = unit;
  
	if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
		printf("ioctl command return error!\n");
		goto out;
	}

	if (e->ret_val != -1) {
		*local_cap = e->autoneg_local;
		*ad_cap = e->autoneg_ad;
		*autoneg = e->autoneg_info;
	}
out:
	close(skfd);
	return err;
}

/*
 * Function:
 *    bcm_phy_autoneg_info_set
 * Description:
 *  Set autoneg info for port
 * Parameters:
 *    unit - unit
 *    port - port.
 * Returns:
 *    autoneg - 0 is disabled and 1 is enabled
 *    BCM_E_NONE - Success.
 */
int bcm_phy_autoneg_info_set(int unit, int port, unsigned char autoneg)
{
	int skfd, err = 0;
	struct ifreq ifr;
	struct ethswctl_data ifdata;
	struct ethswctl_data *e = &ifdata;	

	if ((skfd=ethswctl_init(&ifr)) < 0) {
		printf("ethswctl_init failed. \n");
		return skfd;
	}
	ifr.ifr_data = (char *)&ifdata;

	e->op = ETHSWPHYAUTONEG;
	e->type = TYPE_SET;
	e->port = port;
	e->unit = unit;
	e->autoneg_info = autoneg;

	if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
		printf("ioctl command return error!\n");
		goto out;
	}

out:
	close(skfd);
	return err;
}

/*
 * Function:
 *    bcm_phy_autoneg_cap_adv_set
 * Description:
 *  Set autoneg local capability
 * Parameters:
 *    unit - unit
 *    port - port.
 * Returns:
 *    autoneg - bit 0:  0  --- disable auto-negotiation
                               1  ---- enable auto-negotiation
                      bit 1:  1 --- restart auto-negotiation
 *    BCM_E_NONE - Success.
 */
int bcm_phy_autoneg_cap_adv_set(int unit, int port, unsigned char autoneg, unsigned short* ad_cap)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;
    
    if ((skfd=ethswctl_init(&ifr)) < 0) {
        printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWPHYAUTONEGCAPADV;
    e->type = TYPE_SET;
    e->port = port;
    e->unit = unit;
    e->autoneg_info = autoneg;
    e->autoneg_local = *ad_cap;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        printf("ioctl command return error!\n");
        goto out;
    }

out:
    close(skfd);
    return err;
}


/*
 * Function:
 *    bcm_port_mirror_set/get
 * Description:
 *  Set/Get switch port mirroring configuration
 * Parameters:
 *    unit - unit
 *    enbl - enable(1)/disable(0)
 *    port - mirror port.
 *    ing_pmap - Port map of ingress mirror ports
 *    eg_pmap - Port map of egress mirror ports
 *    blk_no_mrr - switch should block all non-mirrored traffic towards mirror_port
 *    tx_port - TX packet mirror port, Optional - if not supplied all traffic is mirrored to "mirror_port"; Applicable only to Runner
 *    rx_port - RX packet mirror port, Optional - if not supplied all traffic is mirrored to "mirror_port"; Applicable only to Runner
 * Returns:
 *    0 - Success else failure.
 */
int bcm_port_mirror_set(int unit,int enbl,int port,unsigned int ing_pmap,
                        unsigned int eg_pmap, unsigned int blk_no_mrr, 
                        int tx_port, int rx_port)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op   = ETHSWMIRROR;
    e->type = TYPE_SET;
    e->unit = unit;
    e->port_mirror_cfg.enable = enbl;
    e->port_mirror_cfg.mirror_port = port;
    e->port_mirror_cfg.ing_pmap = ing_pmap;
    e->port_mirror_cfg.eg_pmap = eg_pmap;
    e->port_mirror_cfg.blk_no_mrr = blk_no_mrr;
    e->port_mirror_cfg.tx_port = tx_port;
    e->port_mirror_cfg.rx_port = rx_port;    

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
      goto out;
    }

out:
    close(skfd);
    return err;
}
int bcm_port_mirror_get(int unit,int *enbl,int *port,unsigned int *ing_pmap,
                        unsigned int *eg_pmap, unsigned int *blk_no_mrr,
                        int *tx_port, int *rx_port)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op   = ETHSWMIRROR;
    e->type = TYPE_GET;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
      goto out;
    }
    *enbl = e->port_mirror_cfg.enable;
    *port = e->port_mirror_cfg.mirror_port;
    *ing_pmap = e->port_mirror_cfg.ing_pmap;
    *eg_pmap = e->port_mirror_cfg.eg_pmap;
    *blk_no_mrr = e->port_mirror_cfg.blk_no_mrr;
    *tx_port = e->port_mirror_cfg.tx_port;
    *rx_port= e->port_mirror_cfg.rx_port;

out:
    close(skfd);
    return err;
}

/*
 * Function:
 *    bcm_port_trunk_set/get
 * Description:
 *  Set/Get switch port trunking configuration
 * Parameters:
 *    unit - unit
 *    enbl - enable(1)/disable(0)
 *    hash_sel - MAC hash selection criteria.
 *    grp0_pmap - Port map of group_0 trunk ports
 *    grp1_pmap - Port map of group_1 trunk ports
 * Returns:
 *    0 - Success else failure.
 */
int bcm_port_trunk_set(int unit,unsigned int hash_sel)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op   = ETHSWPORTTRUNK;
    e->type = TYPE_SET;
    e->unit = unit;
    e->port_trunk_cfg.hash_sel = hash_sel;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
      goto out;
    }

out:
    close(skfd);
    return err;
}
int bcm_port_trunk_get(int unit,int *enbl,unsigned int *hash_sel,unsigned int *grp0_pmap,unsigned int *grp1_pmap)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op   = ETHSWPORTTRUNK;
    e->type = TYPE_GET;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
      goto out;
    }
    *enbl = e->port_trunk_cfg.enable;
    *hash_sel = e->port_trunk_cfg.hash_sel;
    *grp0_pmap = e->port_trunk_cfg.grp0_pmap;
    *grp1_pmap = e->port_trunk_cfg.grp1_pmap;

out:
    close(skfd);
    return err;
}

int bcm_cfp_op(cfpArg_t *cfpArg)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;
    memcpy(&e->cfpArgs, cfpArg, sizeof(e->cfpArgs));

    e->op   = ETHSWCFP;
    e->unit = cfpArg->unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
      goto out;
    }
    memcpy(cfpArg, &e->cfpArgs, sizeof(e->cfpArgs));

out:
    close(skfd);
    return err;
}


#if defined(SUPPORT_ETH_PWRSAVE)
/*
 * Function:
 *    bcm_phy_apd_get
 * Description:
 *    Gets global PHY auto-power-down setting
 * Parameters:
 *    none
 * Returns:
 *    apd_en - 0 disabled, 1 enabled
 *    BCM_E_NONE - Success.
 */
int bcm_phy_apd_get(unsigned int* apd_en)
{
	int skfd, err = 0;
	struct ifreq ifr;
	struct ethswctl_data ifdata;
	struct ethswctl_data *e = &ifdata;	

	if ((skfd=ethswctl_init(&ifr)) < 0) {
		printf("ethswctl_init failed. \n");
		return skfd;
	}
	ifr.ifr_data = (char *)&ifdata;

	e->op = ETHSWPHYAPD;
	e->type = TYPE_GET;
  
	if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
		printf("ioctl command return error!\n");
		goto out;
	}

	if (e->ret_val != -1) {
		*apd_en = e->val;
	}
out:
	close(skfd);
	return err;
}

/*
 * Function:
 *    bcm_phy_apd_set
 * Description:
 *    Globally sets auto-power-down on all PHYs
 * Parameters:
 *    apd_en - 0 disabled, 1 enabled
 * Returns:
 *    BCM_E_NONE - Success.
 */
int bcm_phy_apd_set(unsigned int apd_en)
{
	int skfd, err = 0;
	struct ifreq ifr;
	struct ethswctl_data ifdata;
	struct ethswctl_data *e = &ifdata;	

	if ((skfd=ethswctl_init(&ifr)) < 0) {
		printf("ethswctl_init failed. \n");
		return skfd;
	}
	ifr.ifr_data = (char *)&ifdata;

	e->op = ETHSWPHYAPD;
	e->type = TYPE_SET;
	e->val = apd_en;

	if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
		printf("ioctl command return error!\n");
		goto out;
	}

out:
	close(skfd);
	return err;
}
#endif

#if defined(SUPPORT_ENERGY_EFFICIENT_ETHERNET)
/*
 * Function:
 *    bcm_phy_eee_get
 * Description:
 *    Gets global PHY Energy Efficient Ethernet setting
 * Parameters:
 *    none
 * Returns:
 *    apd_en - 0 disabled, 1 enabled
 *    BCM_E_NONE - Success.
 */
int bcm_phy_eee_get(unsigned int* eee_en)
{
	int skfd, err = 0;
	struct ifreq ifr;
	struct ethswctl_data ifdata;
	struct ethswctl_data *e = &ifdata;	

	if ((skfd=ethswctl_init(&ifr)) < 0) {
		printf("ethswctl_init failed. \n");
		return skfd;
	}
	ifr.ifr_data = (char *)&ifdata;

	e->op = ETHSWPHYEEE;
	e->type = TYPE_GET;
  
	if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
		printf("ioctl command return error!\n");
		goto out;
	}

	if (e->ret_val != -1) {
		*eee_en = e->val;
	}
out:
	close(skfd);
	return err;
}

/*
 * Function:
 *    bcm_phy_eee_set
 * Description:
 *    Globally sets Energy Efficient Ethernet on all PHYs
 * Parameters:
 *    apd_en - 0 disabled, 1 enabled
 * Returns:
 *    BCM_E_NONE - Success.
 */
int bcm_phy_eee_set(unsigned int eee_en)
{
	int skfd, err = 0;
	struct ifreq ifr;
	struct ethswctl_data ifdata;
	struct ethswctl_data *e = &ifdata;	

	if ((skfd=ethswctl_init(&ifr)) < 0) {
		printf("ethswctl_init failed. \n");
		return skfd;
	}
	ifr.ifr_data = (char *)&ifdata;

	e->op = ETHSWPHYEEE;
	e->type = TYPE_SET;
	e->val = eee_en;

	if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
		printf("ioctl command return error!\n");
		goto out;
	}

out:
	close(skfd);
	return err;
}
#endif

#if defined(SUPPORT_ETH_DEEP_GREEN_MODE)
/*
 * Function:
 *    bcm_DeepGreenMode_get
 * Description:
 *    Gets the SF2 Switch Deep Green Mode setting
 * Parameters:
 *    dgm_en - 0 to get whether DGM Feature is enabled/disabled, 1 to get whether DGM Feature is activated/deactivated
 * Returns:
 *    dgm_en - 0 disabled/deactivated, 1 enabled/activated
 *    BCM_E_NONE - Success.
 */
int bcm_DeepGreenMode_get(unsigned int* dgm_en)
{
	int skfd, err = 0;
	struct ifreq ifr;
	struct ethswctl_data ifdata;
	struct ethswctl_data *e = &ifdata;	

	if ((skfd=ethswctl_init(&ifr)) < 0) {
		printf("ethswctl_init failed. \n");
		return skfd;
	}
	ifr.ifr_data = (char *)&ifdata;

	e->op = ETHSWDEEPGREENMODE;
	e->type = TYPE_GET;
	e->val = *dgm_en;
  
	if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
		printf("ioctl command return error!\n");
		goto out;
	}

	if (e->ret_val != -1) {
		*dgm_en = e->val;
	}
out:
	close(skfd);
	return err;
}

/*
 * Function:
 *    bcm_DeepGreenMode_set
 * Description:
 *    Enables/disables the SF2 Switch Deep Green Mode
 * Parameters:
 *    dgm_en - 0 disabled, 1 enabled
 * Returns:
 *    BCM_E_NONE - Success.
 */
int bcm_DeepGreenMode_set(unsigned int dgm_en)
{
	int skfd, err = 0;
	struct ifreq ifr;
	struct ethswctl_data ifdata;
	struct ethswctl_data *e = &ifdata;	

	if ((skfd=ethswctl_init(&ifr)) < 0) {
		printf("ethswctl_init failed. \n");
		return skfd;
	}
	ifr.ifr_data = (char *)&ifdata;

	e->op = ETHSWDEEPGREENMODE;
	e->type = TYPE_SET;
	e->val = dgm_en;

	if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
		printf("ioctl command return error!\n");
		goto out;
	}

out:
	close(skfd);
	return err;
}
#endif

#if defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_4908) || defined(CHIP_63158)
/*
 * Function:
 *  bcm_port_storm_ctrl_set
 * Purpose:
 *  Set ingress storm control rate limiting parameters
 * Parameters:
 *  unit - Device number
 *  port - Port number
 *  pkt_msk = 0x1(unicast lookup hit), 0x2(multicast lookup hit), 0x4(reserved mac addr 01-80-c2-00-00- 00~2f)
 *            0x8(broadcast), 0x10(multicast lookup fail), 0x20(unicast lookup fail)
 *  rate = 1~28   (Bit rate = rate*8*1024/125, that is 64Kb~1.792Mb with resolution 64Kb)
 *         29~127 (Bit rate = (rate-27)1024, that is 2Mb~100Mb with resolution 1Mb)
 *         128~240 (Bit rate = (rate-115)*8*1024, that is 104Mb~1000Mb with resolution 8Mb)
 *  bucket_size = 0(4K), 1(8K), 2(16K), 3(32K), 4(64K), others(488K) bytes
 * Returns:
 *  BCM_E_XXX
 * Note :
 *  Robo Switch support 2 ingress buckets for different packet type. Only use bucket 0 here.
 *  PKT_MSK1 and PKT_MSK0 shouldn't have any overlaps on packet type selection. Otherwise, the accuracy of rate would be affected. 
 */
int bcm_port_storm_ctrl_set(int unit,bcm_port_t port, int pkt_msk, unsigned int rate, unsigned int bucket_size)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWPORTSTORMCTRL;
    e->port = port;
    e->type = TYPE_SET;
    e->unit = unit;
    e->pkt_type_mask = pkt_msk;
    e->limit = rate;
    e->burst_size = bucket_size;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
    }

    close(skfd);
    return err;
}

int bcm_port_storm_ctrl_get(int unit,bcm_port_t port,int *pkt_msk, unsigned int *rate,unsigned int *bucket_size)
{
    int skfd, err = 0;
    struct ifreq ifr;
    struct ethswctl_data ifdata;
    struct ethswctl_data *e = &ifdata;

    if ((skfd=ethswctl_init(&ifr)) < 0) {
      printf("ethswctl_init failed. \n");
        return skfd;
    }
    ifr.ifr_data = (char *)&ifdata;

    e->op = ETHSWPORTSTORMCTRL;
    e->port = port;
    e->type = TYPE_GET;
    e->unit = unit;

    if((err = ETHSW_IOCTL_WRAPPER(skfd, SIOCETHSWCTLOPS, &ifr))) {
        if (err == -EOPNOTSUPP)
            printf("ioctl not supported!\n");
        else
            printf("ioctl command return error %d!\n", err);
    }    
    close(skfd);

    *pkt_msk = e->pkt_type_mask;
    *rate = e->limit;
    *bucket_size = e->burst_size;
    return err;
}
#endif

#ifdef DESKTOP_LINUX
static int fake_ethsw_ioctl(int fd __attribute__((unused)),
                            int cmd __attribute__((unused)),
                            void *data __attribute__((unused)))
{
   return 0;
}
#endif

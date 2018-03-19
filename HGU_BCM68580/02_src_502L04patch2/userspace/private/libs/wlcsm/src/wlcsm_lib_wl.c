/*************************************************
* <:copyright-BRCM:2013:proprietary:standard
* 
*    Copyright (c) 2013 Broadcom 
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
***********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/sockios.h>
#include <linux/ethtool.h>
#include <wlioctl.h>
#include <bcmutils.h>
#include "wlcsm_linux.h"
#include "wlcsm_lib_api.h"
#include "wlcsm_lib_netlink.h"
#include "wlcsm_lib_nvram.h"
#include "wlcsm_lib_wl.h"


static void syserr(const char *s)
{
    perror(s);
    exit(errno);
}




#if 0
static int wl_ioctl(void *wl, int cmd, void *buf, int len, char set)
{
    struct ifreq *ifr = (struct ifreq *) wl;
    wl_ioctl_t ioc;
    int ret = WLCSM_SUCCESS;
    int s;

    /* open socket to kernel */
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        syserr("socket");

    /* do it */
    ioc.cmd = cmd;
    ioc.buf = buf;
    ioc.len = len;
    ioc.set = set;
    ifr->ifr_data = (caddr_t) &ioc;
    if ((ret = ioctl(s, SIOCDEVPRIVATE, ifr)) < 0) {
        if (cmd != WLC_GET_MAGIC) {
            ret = WLCSM_GEN_ERR;
        }
    }

    /* cleanup */
    close(s);
    return ret;
}
#endif


static int
_wl_get_dev_type(char *name, void *buf, int len)
{
    int s;
    int ret;
    struct ifreq ifr;
    struct ethtool_drvinfo info;

    /* open socket to kernel */
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        syserr("socket");

    /* get device type */
    memset(&info, 0, sizeof(info));
    info.cmd = ETHTOOL_GDRVINFO;
    ifr.ifr_data = (caddr_t)&info;
    strncpy(ifr.ifr_name, name, IFNAMESIZE);
    if ((ret = ioctl(s, SIOCETHTOOL, &ifr)) < 0) {

        /* print a good diagnostic if not superuser */
        if (errno == EPERM)
            syserr("_wl_get_dev_type");

        *(char *)buf = '\0';
    } else {
        strncpy(buf, info.driver, len);
    }

    close(s);
    return ret;
}

static inline int _wl_get_intf_num(char b_adapter)
{
    char proc_net_dev[] = "/proc/net/dev";
    FILE *fp;
    char buf[1000], *c, *name;
    char dev_type[DEV_TYPE_LEN];
    int wlif_num=0;

    if (!(fp = fopen(proc_net_dev, "r")))
        return WLCSM_GEN_ERR;

    /* eat first two lines */
    if (!fgets(buf, sizeof(buf), fp) ||
        !fgets(buf, sizeof(buf), fp)) {
        fclose(fp);
        return WLCSM_GEN_ERR;
    }

    while (fgets(buf, sizeof(buf), fp)) {
        c = buf;
        while (isspace(*c))
            c++;
        if (!(name = strsep(&c, ":")))
            continue;
        WLCSM_TRACE(WLCSM_TRACE_LOG,"JXUJXU:%s:%d  name:%s \r\n",__FUNCTION__,__LINE__,name);
        if (_wl_get_dev_type(name, dev_type, DEV_TYPE_LEN) >= 0 &&
            !strncmp(dev_type, "wl", 2)) {
            WLCSM_TRACE(WLCSM_TRACE_LOG,"JXUJXU:%s:%d  it is wireless interface and devtype:%s \r\n",__FUNCTION__,__LINE__,dev_type );
            if(!b_adapter ||( b_adapter && !strstr(name,".")))
                wlif_num++;
            if (wlif_num >= MAX_WLIF_NUM)
                break;
        }
    }
    fclose(fp);
    return wlif_num;
}

int wlcsm_wl_get_adapter_num(void)
{
    return _wl_get_intf_num(1);
}



int wlcsm_wl_get_mbss_num(int idx)
{

    char buf[128],cmd[32];
    FILE *fp;
    /* default to system maximum */
    int num=WL_DEFAULT_NUM_SSID ;

    sprintf(cmd, "wlctl -i wl%d cap > /var/wl%dcap", idx, idx);
    system(cmd);

    sprintf(cmd, "/var/wl%dcap", idx);

    fp = fopen(cmd, "r");

    if ( fp != NULL ) {
        for (; fgets(buf, sizeof(buf), fp);) {
            if(strstr(buf, "1ssid")) {
                num= 1;
                break;
            }
            if(strstr(buf, "mbss4")) {
                num= 4;
                break;
            }
            if(strstr(buf, "mbss8")) {
                num= 8;
                break;

            } else if(strstr(buf, "mbss16")) {
                num= 16;
                break;
            }
        }
        fclose(fp);
    }

    /* limit the max MBSS to the maximum */
    return num>WL_MAX_NUM_SSID? WL_MAX_NUM_SSID:num;
}

int wl_get_br_names(char *brnames)
{

#if 0
    struct ifreq *ifr = (struct ifreq *) wl;
    wl_ioctl_t ioc;
    int s;
    unsigned long args[4];
    ifr.ifr_data=(char *)&args;
    jkk

    /* open socket to kernel */
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        syserr("socket");
#endif
    WLCSM_TRACE(WLCSM_TRACE_DBG,"TODO....\n");
    strcpy(brnames,"br0");
    return 0;
}

int wl_get_br_ifnames(char *br,char *ifnames)
{

#if 0
    struct ifreq *ifr = (struct ifreq *) wl;
    wl_ioctl_t ioc;
    int s;
    unsigned long args[4];
    ifr.ifr_data=(char *)&args;
    jkk

    /* open socket to kernel */
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        syserr("socket");
#endif
    WLCSM_TRACE(WLCSM_TRACE_DBG,"TODO....\n");
    strcpy(ifnames,"eth0 eth1 eth2 eth3 wl0 usb0");
    return 0;
}

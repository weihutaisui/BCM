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
 *
************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include "bcmnet.h"

#include "ethctl.h"

extern cmd_t *common_command_lookup(const char *cmd);

#ifdef BUILD_STATIC
int ethctl_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    cmd_t *cmd;
    struct ifreq ifr;
    int skfd, rc;

    if (argc < 2) {
        command_helpall();
        return -1;
    }

    cmd = common_command_lookup((const char *)argv[1]);
    if ((argc < 3) && (cmd == NULL)) {
        command_helpall();
        return -1;
    }

    /* All the common commands will send an ioctl to bcmsw interface. All the
       interface specific commands will send an ioctl to given interface */
    if (cmd == NULL) {
        cmd = command_lookup(argv[2]);
        if (cmd == NULL) {
            fprintf(stderr, "invalid command [%s]\n", argv[2]);
            command_helpall();
            return -1;
        }

        if (argc < cmd->nargs + 3) {
            fprintf(stderr, "incorrect number of arguments\n");
            command_help(cmd);
            return -1;
        }
 
        /* check if the interface name starts with "eth" or not */
        if ( strstr(argv[1], "eth") == argv[1] || 
             strstr(argv[1], "epon") == argv[1] ||
             strstr(argv[1], ETHERNET_ROOT_DEVICE_NAME) == argv[1] ) {
            strcpy(ifr.ifr_name, argv[1]);
        } else {
            fprintf(stderr, "invalid interface name %s\n", argv[1]);
            command_help(cmd);
            //close(skfd);
            return -1;
        }
    } else {
        if (argc < cmd->nargs + 1) {
            fprintf(stderr, "incorrect number of arguments\n");
            command_help(cmd);
            return -1;
        }

        /* All the common commands (not specific to an interface) use the 
           bcmsw interface. */
        strcpy(ifr.ifr_name, "bcmsw");
    }

    if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        fprintf(stderr, "socket open error\n");
        return -1;
    }

    if (ioctl(skfd, SIOCGIFINDEX, &ifr) < 0 ) {
        fprintf(stderr, "ioctl failed. check if %s exists\n", ifr.ifr_name);
        command_help(cmd);
        close(skfd);
        return -1;
    }
 
    rc = cmd->func(skfd, &ifr, cmd, argv);
    close(skfd);
    return rc;
}

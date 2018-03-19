/******************************************************************************
 *
 *  Copyright (c) 2010  Broadcom Corporation
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

/***************************************************************************
 *
 *     Copyright (c) 2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 *  Description: MoCA OS Abstraction -- Linux implementation
 *
 ***************************************************************************/
#include <fcntl.h>
#include <unistd.h>
#include <moca_os.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <stdarg.h>
#include <time.h>
#include <signal.h>
#include <glob.h>
#include <malloc.h>
#include <errno.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/mman.h>
#include <sys/time.h>

#include <linux/types.h>
#include <linux/sockios.h>
#include <linux/ethtool.h>
#include <linux/in6.h>
#include <linux/if.h>
#include <linux/if_bridge.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/sysinfo.h>

#include "mocalib.h"
#include "mocaint.h"

#ifdef __MOCA_USE_SYSLOG_
#include <syslog.h>
#endif

#if defined(DSL_MOCA)
#include <bmoca.h>
#include <ethswctl_api.h>
#include <board.h>

#include <linux/version.h>
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4,0,0))
#include <sys/sysinfo.h>
#endif

#else
#include <linux/bmoca.h>
#endif

#define MAX(x, y)   (((x) > (y)) ? (x) : (y))
#define MIN(x, y)   (((x) < (y)) ? (x) : (y))

typedef struct
{
    int cmd_sock_port;
    int evt_sock_port;
    int dev_sock_port;

    int cmd_listen_fd;
    int evt_listen_fd;

    int device_fd;
    int board_fd;

    fd_set select_fdset;
    int select_maxfd;
    fd_set evt_fdset;
    int evt_maxfd;

    unsigned char *fw_img;
    int fw_len;

    char ifname[MoCAOS_IFNAMSIZE];

    int roundrobin;  // index for processing multiple simultaneous clients in a round-robin fashion

    MoCAOS_DrvInfo kdrv_info;

    pthread_t printfthread;
    int pipefd[2];

    int daemon;
    void *moca_handle;
} LinuxOS_Context;

LinuxOS_Context g_context;
MoCAOS_Handle g_handle = (MoCAOS_Handle) &g_context;

static int mocaos_OpenListener(char *sockname)
{
    int fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    struct sockaddr_un unix_addr;

    if(fd < 0)
    {
        MoCAOS_Printf(g_handle, "can't open socket: %s", strerror(errno));
        exit(-1);
    }
    
    memset(&unix_addr, 0, sizeof(unix_addr));
    unix_addr.sun_family = AF_UNIX;
    strcpy(unix_addr.sun_path, sockname);

    if(bind(fd, (const struct sockaddr *)&unix_addr, sizeof(unix_addr)) < 0)
    {
        MoCAOS_Printf(g_handle, "can't bind: %s", strerror(errno));
        exit(-1);
    }
    if(listen(fd, 8) < 0)
    {
        MoCAOS_Printf(g_handle, "can't listen: %s", strerror(errno));
        exit(-1);
    }
    if(fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
    {
        MoCAOS_Printf(g_handle, "can't fcntl: %s", strerror(errno));
        exit(-1);
    }
 
    return(fd);
}

static int mocaos_Accept(MoCAOS_Handle handle, int listen_fd)
{
    int newfd;

    newfd = accept(listen_fd, NULL, NULL);
    if(newfd < 0) {
        MoCAOS_Printf(handle, "warning: accept() failed: %s\n",
            strerror(errno));
        return(-1);
    } else {
        if(fcntl(newfd, F_SETFL, O_NONBLOCK) < 0)
        {
           MoCAOS_Printf(handle, "can't fcntl: %s", strerror(errno));            
            exit(-1);
        }
    }

    return(newfd);
}

static int mocaos_FindIFName(MoCAOS_Handle handle, MoCAOS_DrvInfo *kdrv_info, char *ifname)
{
    glob_t g;
    int ret = -1, i, match = -1, fd;

    if (kdrv_info->enet_id == ~0UL) {
	      /* latest drivers pass network device ifname */
	      strcpy((char *)ifname, kdrv_info->enet_name);
	      return 0;
    }

    strcpy((char *)ifname, "unknown");
    ret = glob(IFNAME_GLOB, 0, NULL, &g);
    if(ret == GLOB_NOMATCH) {
        MoCAOS_Printf(handle, "can't find network interface list "
            "under %s\n", IFNAME_GLOB);
        return(-1);
    }
    if(ret != 0) {
        MoCAOS_Printf(handle, "error listing %s\n", IFNAME_GLOB);
        return(-1);
    }

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0) {
        MoCAOS_Printf(handle, "can't open ethtool socket\n");
        goto out;
    }

    for(i = 0; i < g.gl_pathc; i++) {
        struct ifreq ifr;
        struct ethtool_drvinfo drvinfo;
        char *t_ifname;
        int err;

        t_ifname = strrchr(g.gl_pathv[i], '/');
        if(t_ifname)
            t_ifname++;
        else
            t_ifname = g.gl_pathv[i];

        memset(&ifr, 0, sizeof(ifr));
        memset(&drvinfo, 0, sizeof(drvinfo));

        drvinfo.cmd = ETHTOOL_GDRVINFO;
        ifr.ifr_data = (caddr_t)&drvinfo;
        strcpy(ifr.ifr_name, t_ifname);

        err = ioctl(fd, SIOCETHTOOL, &ifr) < 0;

        if(err)
        {
            fprintf(stderr,"Error finding IFNAME\n");
            continue;
        }
        if(strcmp(drvinfo.driver,
                (char *)kdrv_info->enet_name) == 0) {
            match++;
            if(kdrv_info->enet_id == match) {
                strcpy((char *)ifname, t_ifname);
                ret = 0;
                goto out2;
            }
        }
    }

out2:
    close(fd);
out:
    globfree(&g);
    return(ret);
}


#if !defined(DSL_MOCA)

static int mocaos_add_rtattr(struct nlmsghdr *nlmsg, int nlmsgsz, int type, const void *data, int datalen)
{
   struct rtattr *ra;
   int rasz = RTA_LENGTH(datalen);

   if (NLMSG_ALIGN(nlmsg->nlmsg_len) + rasz > nlmsgsz)
      return 0;
   ra = (struct rtattr *) ((char *)nlmsg + NLMSG_ALIGN(nlmsg->nlmsg_len));
   ra->rta_type = type;
   ra->rta_len = rasz;
   memcpy(RTA_DATA(ra), data, datalen);

   return NLMSG_ALIGN(nlmsg->nlmsg_len) + rasz;
}

static int mocaos_set_link_operstate(MoCAOS_Handle handle, const char *ifname, int status)
{
   int ret, s;
   struct {
      struct nlmsghdr nlmsg;
      struct ifinfomsg ifi;
      char buf[100];
   } msg;
   unsigned char state = (status == 1) ? IF_OPER_UP : IF_OPER_DORMANT;

   s = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
   if (s == -1)
      return -1;

   memset(&msg, 0, sizeof(msg));
   msg.nlmsg.nlmsg_len = NLMSG_LENGTH(sizeof(msg.ifi));
   msg.nlmsg.nlmsg_flags = NLM_F_REQUEST;
   msg.nlmsg.nlmsg_type = RTM_SETLINK;

   msg.ifi.ifi_family = AF_UNSPEC;

   int ifnamesz = strlen(ifname)+1;
   msg.nlmsg.nlmsg_len = mocaos_add_rtattr(&msg.nlmsg, sizeof(msg), IFLA_IFNAME, ifname, ifnamesz);
   if (msg.nlmsg.nlmsg_len == 0) {
      close(s);
      return -1;
   }

#if 0 // the genet driver should initialize this and we don't need to do it.
   unsigned char mode = IF_LINK_MODE_DORMANT;
   msg.nlmsg.nlmsg_len = mocaos_add_rtattr(&msg.nlmsg, sizeof(msg), IFLA_LINKMODE, &mode, sizeof(mode));
   if (msg.nlmsg.nlmsg_len == 0) {
      close(s);
      return -1;
   }
#endif

   msg.nlmsg.nlmsg_len = mocaos_add_rtattr(&msg.nlmsg, sizeof(msg), IFLA_OPERSTATE, &state, sizeof(state));
   if (msg.nlmsg.nlmsg_len == 0) {
      close(s);
      return -1;
   }

   ret = send(s, &msg, msg.nlmsg.nlmsg_len, 0);

   close(s);
   return (ret == msg.nlmsg.nlmsg_len) ? 0: -1;
}

void mocaos_setLinkStatus(MoCAOS_Handle handle, int status)
{
    int fd;
    struct ifreq ifr;
    struct ethtool_cmd ethcmd;
    struct ethtool_ringparam ringparam;
    LinuxOS_Context *ctx = (LinuxOS_Context *)handle;    
    int err;
    
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0) 
    {
        perror("can't open ethtool socket ");
        return;
    }

    memset(&ifr, 0, sizeof(ifr));
    memset(&ethcmd, 0, sizeof(ethcmd));
    memset(&ringparam, 0, sizeof(ringparam));

#if 1
    // Setting the RUNNING state by overriding the autoneg setting is now obsolete
    //  for 3.14 kernels.  Older 3.3 kernels may still need this.  Only do this
    //  on chips that may be running the 3.3 kernel
    if (ctx->kdrv_info.hw_rev < MOCA_CHIP_20_3)
    {
       ethcmd.cmd = ETHTOOL_GSET;
       ifr.ifr_data = (caddr_t)&ethcmd;
       strcpy(ifr.ifr_name, ctx->ifname);

       err = ioctl(fd, SIOCETHTOOL, &ifr) < 0;

       if(err)
       {
           close(fd);
           return;
       }

       ethcmd.cmd = ETHTOOL_SSET;
       ethcmd.autoneg = status;  // we override the autoneg meaning for link up/down in the old genet driver
       ifr.ifr_data = (caddr_t)&ethcmd;
       strcpy(ifr.ifr_name, ctx->ifname);

       err = ioctl(fd, SIOCETHTOOL, &ifr) < 0;
    
       if(err)
       {
           close(fd);
           return;
       }
   }
#endif

    err = mocaos_set_link_operstate(handle, ctx->ifname, status);
    if(err)
    {
        MoCAOS_Printf(handle, "mocaos_set_link_operstate ERROR!\n");
        close(fd);
        return;
    }

    /* set up TX rings in ethernet driver.  This may fail with older kernels whose GENET driver doesn't support setting queues with ethtool */
    if (!status) // disabled
    {
       ringparam.cmd = ETHTOOL_GRINGPARAM;
       ifr.ifr_data = (caddr_t)&ringparam;
       strcpy(ifr.ifr_name, ctx->ifname);

       err = ioctl(fd, SIOCETHTOOL, &ifr) < 0;

       if (err)
       {
          close(fd);
          return;
       }
       
       ringparam.cmd = ETHTOOL_SRINGPARAM;
       ringparam.tx_pending = 5;  // number of queues = 5
       ifr.ifr_data = (caddr_t)&ringparam;
       strcpy(ifr.ifr_name, ctx->ifname);

       err = ioctl(fd, SIOCETHTOOL, &ifr) < 0;
       if (err)
       {
          close(fd);
          return;
       }
    }

    close(fd);
}

#endif

static int mocaos_OpenDev(MoCAOS_Handle handle, const char *chardev)
{
    int fd, major, minor;
    char filename[MOCA_FILENAME_LEN], devnum[16];

    if (strlen(chardev) > (MOCA_FILENAME_LEN - 1))
    {
        MoCAOS_Printf(handle, "filename is too long: '%s'\n", chardev);
        exit(-1);
    }

    fd = open(chardev, O_RDWR | O_NONBLOCK);
    if (fd >= 0)
        return(fd);
    
    if (errno != ENOENT)
        goto bad;

    /* find "root" device name */

#if defined(DSL_MOCA) /* DSL Code */
    chardev = strstr(chardev, "bcmmoca");
#else
    chardev = strstr(chardev, "bmoca");
#endif

    if(! chardev || (strlen(chardev) > 16))
        goto bad;

    /* try to find major/minor number */

    sprintf(filename, DEVNAME_FMT, chardev);
    fd = open(filename, O_RDONLY);
    if (fd < 0)
        goto bad;
    if (read(fd, devnum, sizeof(devnum)) <= 0)
        goto bad;
    if (sscanf(devnum, "%d:%d", &major, &minor) != 2)
        goto bad;
 
    sprintf(filename, "/tmp/%s", chardev);
    unlink(filename);
    if (mknod(filename, 0600 | S_IFCHR, makedev(major, minor)) < 0)
        goto bad;

    fd = open(filename, O_RDWR | O_NONBLOCK);

    if(fd >= 0)
        return(fd);

bad:
    MoCAOS_Printf(handle, "can't open MoCA device: %s", strerror(errno));
    exit(-1);
    
    return(-1);
}

static void mocaos_Close(MoCAOS_Handle handle, int fd)
{
    LinuxOS_Context *ctx = (LinuxOS_Context *)handle;

    close(fd);
    FD_CLR(fd, &ctx->select_fdset);
    FD_CLR(fd, &ctx->evt_fdset);
    if(fd == ctx->select_maxfd) {
        fd_set fds = ctx->select_fdset;
        FD_SET(ctx->cmd_listen_fd, &fds);
        FD_SET(ctx->device_fd, &fds);
        while(! FD_ISSET(ctx->select_maxfd, &fds))
            ctx->select_maxfd--;
    }
}

static int mocaos_CheckForData(MoCAOS_Handle handle)
{
    LinuxOS_Context *ctx = (LinuxOS_Context *)handle;
    int ret;

    if (ioctl(ctx->device_fd, MOCA_IOCTL_CHECK_FOR_DATA, &ret) < 0) // this will fail on older kernels
        return(0);

    return(ret);
}

void *mocaos_printfthread(void *x)
{
    LinuxOS_Context *ctx = (LinuxOS_Context *)x;
    unsigned char buf[256];
    int fd;

    fd = open("/dev/console", O_WRONLY);

    if (!fd)
    {
        stdout = stderr;
        return NULL;
    }

    while(1)
    {
        int co;
        co = read(ctx->pipefd[0], buf, sizeof(buf)-1);

        if (co > 0)
        {
            if (ctx->daemon)
                write(fd, buf, co);
            else
                fwrite(buf, co, 1, stderr);
        }
    }

    return(NULL);
}

void sigTermHandler()
{
#if !defined(DSL_MOCA)
      mocaos_setLinkStatus(g_handle, 0);
      __moca_set_shutdown(g_context.moca_handle);
#endif
     exit(0);

}

MoCAOS_Handle MoCAOS_Init(const char *chardev, char *ifname, const char *workdir, int daemon, void *moca_handle)
{    
    char cmd_sockname[MOCA_FILENAME_LEN];
    char evt_sockname[MOCA_FILENAME_LEN];
    char defsock[MOCA_FILENAME_LEN];

    signal(SIGPIPE, SIG_IGN);
   
    if (chdir (workdir))
    {
        printf("WARNING: unable to change dir to %s, trying /etc/moca\n", workdir);
        if (chdir ("/etc/moca"))
        {
            printf("WARNING: unable to change dir to /etc/moca, using current dir\n");
        }
    }

    memset(&g_context, 0, sizeof(g_context));

    g_context.daemon = daemon;
    g_context.device_fd = mocaos_OpenDev((MoCAOS_Handle)&g_context, chardev);
    g_context.moca_handle = moca_handle;

#if defined(DSL_MOCA)
    g_context.board_fd = open("/dev/brcmboard", O_RDWR);
    if (g_context.board_fd == -1)
    {
        MoCAOS_Printf(g_handle, "Could not open /dev/brcmboard\n");
        exit(-1);
    }
    else
    {
      MoCAOS_Printf(g_handle, "board_fd = 0x%x\n", g_context.board_fd);
    }
#endif

    if (MoCAOS_GetDriverInfo((MoCAOS_Handle)&g_context, &g_context.kdrv_info) != 0)
    {
        MoCAOS_Printf(g_handle, "Kernel/mocad version mismatch.  Ensure kernel and mocad versions are compatible\n");
        exit(-1);
    }

    if(g_context.kdrv_info.refcount != 1)
    {
        MoCAOS_Printf(g_handle, "nonzero refcount (%d); is another mocad running?\n", g_context.kdrv_info.refcount);
        exit(-1);
    }

    /* get ifname and use it to name the sockets */
    if (ifname[0] == 0x00)
        mocaos_FindIFName((MoCAOS_Handle)&g_context, &g_context.kdrv_info, ifname);

    strncpy(g_context.ifname, ifname, MoCAOS_IFNAMSIZE);
    sprintf(cmd_sockname, MOCA_CMD_SOCK_FMT, ifname);
    sprintf(evt_sockname, MOCA_EVT_SOCK_FMT, ifname);

    unlink(cmd_sockname);
    unlink(evt_sockname);

    /* create symlinks making this the default moca interface */

    sprintf(defsock, MOCA_CMD_SOCK_FMT, MOCA_DEFAULT_IFNAME);
    unlink(defsock);
    symlink(cmd_sockname, defsock);

    sprintf(defsock, MOCA_EVT_SOCK_FMT, MOCA_DEFAULT_IFNAME);
    unlink(defsock);
    symlink(evt_sockname, defsock);

    g_context.cmd_listen_fd = mocaos_OpenListener(cmd_sockname);
    g_context.evt_listen_fd = mocaos_OpenListener(evt_sockname);
    
    FD_ZERO(&g_context.select_fdset);
    FD_SET(g_context.device_fd, &g_context.select_fdset);
    FD_SET(g_context.cmd_listen_fd, &g_context.select_fdset);
    FD_SET(g_context.evt_listen_fd, &g_context.select_fdset);

    g_context.select_maxfd = MAX(g_context.device_fd,
                                 MAX(g_context.cmd_listen_fd, 
                                     g_context.evt_listen_fd));

    if(daemon) 
    {
        pid_t pid = fork();
        if(pid < 0) 
        {
            MoCAOS_Printf(g_handle, "can't fork: %s\n", strerror(errno));
            exit(-1);
        }
        if(pid > 0) 
        {
            /* parent */
            exit(0);
        }
        setsid();
    }

    if (pipe(&g_context.pipefd[0]) != 0)
    {
        MoCAOS_Printf(g_handle, "WARNING: unable to create pipe for debug prints: %s\n", strerror(errno));
    }
    else
    {
        fcntl(g_context.pipefd[1], F_SETFL, O_NONBLOCK);
        stdout = fdopen(g_context.pipefd[1], "w");

        if (!stdout)
            stdout = stderr;
        else
            pthread_create(&g_context.printfthread, NULL, mocaos_printfthread, (void *)&g_context);
    }
    setvbuf(stdout, (char *)NULL, _IONBF, (size_t)0);  // turn off buffering on stdout
#if !defined(DSL_MOCA)
    signal(SIGTERM, sigTermHandler);
#else
#ifdef BRCM_CMS_BUILD
    /* In CMS, daemons should ignore SIGINT */
    signal(SIGINT, SIG_IGN);
#endif
#endif
    return (MoCAOS_Handle)&g_context;
}

MoCAOS_ClientHandle MoCAOS_WaitForRequest(MoCAOS_Handle handle, unsigned int timeout_sec)
{
    LinuxOS_Context *ctx = (LinuxOS_Context *)handle;
    fd_set fds = ctx->select_fdset;
    int ret, i;
    struct timeval tv;
    
    tv.tv_sec = timeout_sec;
    tv.tv_usec = 0;

    ret = select(ctx->select_maxfd + 1, &fds, NULL, NULL, &tv);

    if(ret < 0)
        MoCAOS_Printf(handle, "warning: select() failed: %s\n",
            strerror(errno));

    // when the CPU is loaded, make sure we timed out because
    // of a lack of data, not a lack of CPU time
    if ( (ret == 0) && FD_ISSET(ctx->device_fd, &ctx->select_fdset) && mocaos_CheckForData(handle) )
    {
        return(MoCAOS_CLIENT_CORE);
    }

    if(ret == 0)
    {
        return MoCAOS_CLIENT_TIMEOUT;
    }

    if(FD_ISSET(ctx->cmd_listen_fd, &fds)) 
    {
        int newfd = mocaos_Accept(handle, ctx->cmd_listen_fd);
        if(newfd >= 0) 
        {
            FD_SET(newfd, &ctx->select_fdset);
            ctx->select_maxfd =
                MAX(ctx->select_maxfd, newfd);
        }
        if(--ret == 0)
            return(MoCAOS_CLIENT_NULL);
        FD_CLR(ctx->cmd_listen_fd, &fds);
    }
    if(FD_ISSET(ctx->evt_listen_fd, &fds)) 
    {
        int newfd = mocaos_Accept(handle, ctx->evt_listen_fd);

        if(newfd >= 0) 
        {
            FD_SET(newfd, &ctx->evt_fdset);
            ctx->evt_maxfd = MAX(ctx->evt_maxfd, newfd);

            FD_SET(newfd, &ctx->select_fdset);
            ctx->select_maxfd =
                MAX(ctx->select_maxfd, newfd);
        }
        if(--ret == 0)
            return(MoCAOS_CLIENT_NULL);
        FD_CLR(ctx->evt_listen_fd, &fds);
    }

    for(i = 0; ret && (i <= ctx->select_maxfd); i++) 
    {
        if(FD_ISSET(i, &fds))
        {
            if(FD_ISSET(i, &ctx->evt_fdset))
            {                
                mocaos_Close(handle, i);
                ret--;                
            }             
        }
    }
    
    if(FD_ISSET(ctx->device_fd, &fds))
    {
        return MoCAOS_CLIENT_CORE;
    }

    if (ctx->roundrobin > ctx->select_maxfd)
        ctx->roundrobin = 0;

    for(i = ctx->roundrobin; ret; i++) 
    {
        if(FD_ISSET(i, &fds)) 
        {
            return(MoCAOS_ClientHandle)i;
        }

        if (i == ctx->select_maxfd)
            i = 0;
    }

    return(MoCAOS_CLIENT_NULL);
}

int MoCAOS_SendMMP(MoCAOS_Handle handle, MoCAOS_ClientHandle client, const unsigned char *IE, int len)
{
    LinuxOS_Context *ctx = (LinuxOS_Context *)handle;
    int ret;
    int fd;
    int i;

    if (client == MoCAOS_CLIENT_BROADCAST)
    {
        /* broadcast event to all evt clients */
        for(i = 0; i <= ctx->evt_maxfd; i++)
        {
            if(FD_ISSET(i, &ctx->evt_fdset))
            {
                 ret = write(i, IE, len);
                 if (ret < 0)
                 {
                     mocaos_Close(handle,i);
                 }
                 else if (ret != len)
                 {
                     MoCAOS_Printf(handle, "Warning: short write (%d, %d)\n", ret, len);
                 }
            }
        }
        return(0);
    }

    if (client == MoCAOS_CLIENT_CORE)
        fd = ctx->device_fd;
    else
        fd = (int) client;

    ret = write(fd, IE, len);

    if (ret != len)
    {
        MoCAOS_Printf(handle, "warning: short write fd=%d (%d, %d, %s)\n", fd, ret, len, strerror(errno));
        return(-1);
    }
    
    return(0);
}

int MoCAOS_ReadMMP(MoCAOS_Handle handle, MoCAOS_ClientHandle client, 
    unsigned int timeout_sec, unsigned char *IE, int *len)  // return 0 timeout, <0 failure, >0 success
{
    LinuxOS_Context *ctx = (LinuxOS_Context *)handle;
    int ret;
    fd_set fds;
    int fd;
    struct timeval tv;

    if (!ctx)
        ctx = &g_context;

    if (client == MoCAOS_CLIENT_CORE)
        fd = ctx->device_fd;
    else
        fd = (int) client;

    if (timeout_sec != MoCAOS_TIMEOUT_INFINITE)
    {
        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        
        tv.tv_sec = timeout_sec;
        tv.tv_usec = 0;
        
        ret = select(fd+1, &fds, NULL, NULL, &tv);

        if(ret < 0)
        {
            MoCAOS_Printf(handle, "warning: select() failed: %s\n",
                strerror(errno));
            return(-1);
        }

        // when the CPU is loaded, make sure we timed out because
        // of a lack of data, not a lack of CPU time
        if ( (ret == 0) && FD_ISSET(ctx->device_fd, &ctx->select_fdset) && mocaos_CheckForData(handle) )
        {
            ret = 1;
        }

        if(ret == 0)
        {
            return 0;
        }
    }

    while (1)
    {
        *len = read(fd, IE, *len);
        if (*len < 0) 
        {
            if (errno == EINTR)
                continue;

            if (errno != ECONNRESET)
                MoCAOS_Printf(handle, "warning: read failed: %s\n", strerror(errno));
            else
                mocaos_Close((MoCAOS_Handle)ctx, fd);  // for traps, the event loop will close the other end of the pipe when terminated.  This is normal.

            return(-1);
        } 
        else if ((*len == 0) && (client != MoCAOS_CLIENT_CORE))
        {
            /* closed connection */
            mocaos_Close((MoCAOS_Handle)ctx, fd);
            return(-2);
        }
        else
        {
            break;
        }
    }

    return (1);
}

int MoCAOS_ReadMem(MoCAOS_Handle handle, unsigned char *buf, int len, unsigned char *addr)
{
    LinuxOS_Context *ctx = (LinuxOS_Context *)handle;
    struct moca_xfer x;
    
    x.buf = (unsigned long)buf;
    x.len = len;
    x.moca_addr = (unsigned int)addr;

    if (ioctl(ctx->device_fd, MOCA_IOCTL_READMEM, &x) < 0)
        return(errno);
    
    return(0);
}

int MoCAOS_WriteMem(MoCAOS_Handle handle, unsigned char *buf, int len, unsigned char *addr)
{
    LinuxOS_Context *ctx = (LinuxOS_Context *)handle;
    struct moca_xfer x;
    
    x.buf = (unsigned long)buf;
    x.len = len;
    x.moca_addr = (unsigned int)addr;

    if (ioctl(ctx->device_fd, MOCA_IOCTL_WRITEMEM, &x) < 0)
        return(errno);
    
    return(0);
}

int MoCAOS_StartCore(MoCAOS_Handle handle, unsigned char *fw_img, int fw_len,
    unsigned int boot_flags)
{
    LinuxOS_Context *ctx = (LinuxOS_Context *)handle;    
    struct moca_start start;

    start.x.buf = (unsigned long)fw_img;
    start.x.len = (fw_len + 3) & ~3;
    start.x.moca_addr = 0;

    start.boot_flags = 0;
    if (boot_flags & MOCAOS_BOOT_FLAGS_BONDING_EN)
       start.boot_flags |= MOCA_BOOT_FLAGS_BONDED;

    if (ioctl(ctx->device_fd, MOCA_IOCTL_START, (void *)&start) < 0)
        return(errno);

    return(0);
}

int MoCAOS_StopCore(MoCAOS_Handle handle)
{
    LinuxOS_Context *ctx = (LinuxOS_Context *)handle;
    
    if (ioctl(ctx->device_fd, MOCA_IOCTL_STOP, NULL) < 0)
        return(errno);

    return(0);
}

int MoCAOS_WolCtrl(MoCAOS_Handle handle, int enable)
{
    LinuxOS_Context *ctx = (LinuxOS_Context *)handle;

    if (ioctl(ctx->device_fd, MOCA_IOCTL_WOL, (void*)enable) < 0)
        return(errno);

    return(0);
}

int MoCAOS_PMDone(MoCAOS_Handle handle)
{
    LinuxOS_Context *ctx = (LinuxOS_Context *)handle;

    if (ioctl(ctx->device_fd, MOCA_IOCTL_PM_SUSPEND) < 0)
        return(errno);

    return(0);
}

MoCAOS_ThreadHandle MoCAOS_CreateThread(MoCAOS_ThreadEntry func, void *arg)
{
    pthread_t thread = 0;
    if (0 != pthread_create(&thread, NULL, (void * (*)(void *))func, arg))
    {
        fprintf(stderr,"ERROR: Unable to create thread\n");
    }
    return((MoCAOS_ThreadHandle)thread);
}

void MoCAOS_GetTimeOfDay(unsigned int *sec, unsigned int *usec)
{
   struct timeval tv;
   gettimeofday(&tv, NULL);
   if (sec)
      *sec = tv.tv_sec;
   if (usec)
      *usec = tv.tv_usec;
}

unsigned int MoCAOS_GetTimeSec()
{
   static int error = 0;

   struct sysinfo info;   

   if (!error)
   {
      error = sysinfo(&info);

      if ( error )
         fprintf(stderr, "WARNING: calling sysinfo, using time() as fallback\n");
      else
         return((unsigned int)info.uptime);
   }

   if (error)
   {
      return((unsigned int)time(NULL));
   }

   return 0;
}

int MoCAOS_GetDriverInfo(MoCAOS_Handle handle, MoCAOS_DrvInfo *kdrv_info)
{
    LinuxOS_Context *ctx = (LinuxOS_Context *)handle;
    int rc;
    int oldkernel = 0;
    
    memset (kdrv_info, 0, sizeof(MoCAOS_DrvInfo));
    
    rc = ioctl(ctx->device_fd, MOCA_IOCTL_GET_DRV_INFO, kdrv_info);

    if ((rc >= 0) && (kdrv_info->chip_id == 0))
    {        
        // the ioctl worked, but chipid returned was invalid.  This can
        // occur with kernels that have the new bmoca driver but older kernel.
        oldkernel = 1;
    }

    if ((rc < 0) || (oldkernel))
    {
        if ((rc < 0) && (ioctl(ctx->device_fd, MOCA_IOCTL_GET_DRV_INFO_V2, kdrv_info) < 0))
        {
            return(errno);
        }

        // in this version of ioctl, the chip_id was returned in the hw_rev field        
        kdrv_info->chip_id = kdrv_info->hw_rev;
        
        // chip_id not set by kernel
        // Determing MoCA hw_rev from chip id
        switch(kdrv_info->chip_id & 0xFFFF0000)
        {
            case 0x74200000:
            case 0x74100000:
            case 0x33200000:
            case 0x73400000:
            case 0x73420000:
            case 0x71250000:
            case 0x74180000:
                kdrv_info->hw_rev = MOCA_CHIP_11;
                break;
            case 0x74080000:
                kdrv_info->hw_rev = MOCA_CHIP_11_LITE;
                break;
            default:
                kdrv_info->hw_rev = MOCA_CHIP_11_PLUS;
        }
    }   


    if (((ctx->kdrv_info.chip_id & 0xFFFFFFF0) == 0x742900B0)
        || (ctx->kdrv_info.chip_id == 0x74295A0))
        kdrv_info->hw_rev = MOCA_CHIP_20_2;

    return(0);
}

// return 0 on success
//  -1 if the 3450 isn't accessible to the host
//  -2 if the ioctl doesn't exist
int MoCAOS_Get3450Reg(MoCAOS_Handle handle, unsigned int reg, unsigned int * val) 
{
    LinuxOS_Context *ctx = (LinuxOS_Context *)handle;    
    struct moca_xfer x;
 
    x.buf = (unsigned long)val;
    x.moca_addr = (unsigned int)reg;

    if (ioctl(ctx->device_fd, MOCA_IOCTL_GET_3450_REG, &x) < 0)
    {
       if (errno == -ENODEV)
          return -1;
       else
          return -2;
    }

    return(0);
}

int MoCAOS_Set3450Reg(MoCAOS_Handle handle, unsigned int reg, unsigned int val) 
{
    LinuxOS_Context *ctx = (LinuxOS_Context *)handle;    
    struct moca_xfer x;
 
    x.buf = (unsigned long)&val;
    x.moca_addr = (unsigned int)reg;

    if (ioctl(ctx->device_fd, MOCA_IOCTL_SET_3450_REG, &x) < 0)
    {
       if (errno == -ENODEV)
          return -1;
       else
          return -2;
    }

    return(0);
}

unsigned char *MoCAOS_GetFw(MoCAOS_Handle handle, unsigned char *filename, int *fw_len)
{
    LinuxOS_Context *ctx = (LinuxOS_Context *)handle;
    int fd;

    do
    {
        if(filename)
        {
            fd = open((char *)filename, O_RDONLY);
            if(fd < 0) 
            {
                MoCAOS_Printf(handle, "can't open %s: %s", filename,
                    strerror(errno));
                exit(-1);
            }
            break;
        }
        
        fd = open(MOCACORE_PATH_0, O_RDONLY);
        if(fd >= 0)
            break;
        fd = open(MOCACORE_PATH_1, O_RDONLY);
        if(fd >= 0)
            break;        

        if ((ctx->kdrv_info.hw_rev == MOCA_CHIP_11) ||
            (ctx->kdrv_info.hw_rev == MOCA_CHIP_11_LITE))
        {
            fd = open(MOCACORE_PATH_GEN_1, O_RDONLY);
            if(fd >= 0)
                break;
        }
        else if ((ctx->kdrv_info.hw_rev == MOCA_CHIP_20) ||
                 (ctx->kdrv_info.hw_rev == MOCA_CHIP_20_1))
        {
            fd = open(MOCA20CORE_PATH_GEN_1B, O_RDONLY);
            if(fd >= 0)
                break;                    

            fd = open(MOCA20CORE_PATH_GEN_1, O_RDONLY);
            if(fd >= 0)
                break;                        
        }
        else if (ctx->kdrv_info.hw_rev == MOCA_CHIP_20_2)
        {
            fd = open(MOCA20CORE_PATH_GEN_2B, O_RDONLY);
            if(fd >= 0)
                break;

            fd = open(MOCA20CORE_PATH_GEN_2, O_RDONLY);
            if(fd >= 0)
                break;
        }
        else if (ctx->kdrv_info.hw_rev == MOCA_CHIP_20_3)
        {
            fd = open(MOCA20CORE_PATH_GEN_3B, O_RDONLY);
            if(fd >= 0)
                break;

            fd = open(MOCA20CORE_PATH_GEN_3, O_RDONLY);
            if(fd >= 0)
                break;
        }
        else if (ctx->kdrv_info.hw_rev == MOCA_CHIP_20_4)
        {
            fd = open(MOCA20CORE_PATH_GEN_4B, O_RDONLY);
            if(fd >= 0)
                break;

            fd = open(MOCA20CORE_PATH_GEN_4, O_RDONLY);
            if(fd >= 0)
                break;
        }
        else if (ctx->kdrv_info.hw_rev == MOCA_CHIP_20_5)
        {
            fd = open(MOCA20CORE_PATH_GEN_5B, O_RDONLY);
            if(fd >= 0)
                break;

            fd = open(MOCA20CORE_PATH_GEN_5, O_RDONLY);
            if(fd >= 0)
                break;
        }
        else
        {
            fd = open(MOCACORE_PATH_GEN_2, O_RDONLY);
            if(fd >= 0)
                break;                        
        }
        
        MoCAOS_Printf(handle, "can't find firmware image");
        exit(-1);
    } while(0);

    /* Free any previously mapped memory */
    if (ctx->fw_img != NULL)
    {
        if (munmap(ctx->fw_img, ctx->fw_len) != 0)
            MoCAOS_Printf(handle, "unmap failed (%s)\n", strerror(errno));
    }

    ctx->fw_len = lseek(fd, 0, SEEK_END);
    ctx->fw_img = mmap(NULL, ctx->fw_len, PROT_READ, MAP_SHARED, fd, 0);
    
    if (ctx->fw_img == NULL)
    {
        MoCAOS_Printf(handle, "can't mmap firmware image");
        exit(-1);
    }
    
    close(fd);

    *fw_len = ctx->fw_len;

    return(ctx->fw_img);
}


void MoCAOS_Printf(MoCAOS_Handle handle, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);

#ifdef __MOCA_USE_SYSLOG_
    vsyslog(LOG_NOTICE, fmt, ap);
#else
    vprintf(fmt, ap);
#endif

    fflush(stdout);
    
    va_end(ap);
}

#if defined(DSL_MOCA)
void moca_getBoardName(char *board, int size)
{
   BOARD_IOCTL_PARMS ioctlParms;
   int boardFd = 0;

   boardFd = open(BOARD_DEVICE_NAME, O_RDWR);

   if ( boardFd != -1 )
   {
      ioctlParms.string = board;
      ioctlParms.strLen = size;
      ioctlParms.offset = 0;
      ioctlParms.action = 0;
      ioctlParms.buf    = NULL;
      ioctlParms.result = -1;

      ioctl(boardFd, BOARD_IOCTL_GET_ID, &ioctlParms);
      close(boardFd);
   }
}

#endif
void MoCAOS_EnableDataIf(MoCAOS_Handle handle, char *ifname, int enable)
{
    char cli[64];
    LinuxOS_Context *ctx = (LinuxOS_Context *)handle;

#if defined(DSL_MOCA)
    int rc;
    int unit;
    bcm_port_t port;
    char board[16];

    board[0] = '\0';

    moca_getBoardName(board, sizeof(board));

    board[sizeof(board)-1] = '\0';
#endif

    // disable only TX, leave RX for WOL
    if (enable == 1)
    {
       sprintf(cli,"/sbin/ifconfig %s txqueuelen 0", ifname);
       system(cli);
    }
    else if (enable == 2) // disable interface
    {
#if defined(DSL_MOCA)
       rc = bcm_enet_map_ifname_to_unit_port(ifname, &unit, &port);
       if ( 0 == rc )
       {
           bcm_set_extphylinkstatus(unit, port, PHY_CONN_TYPE_MOCA, 0, 0, 0);
       }

       if (strcmp(board, "96802SV") == 0)
       {
          // EPORT_REG_GPHY_CNTRL  
          sprintf(cli, "spictl setbits 0x10800004 0x5 4 %d", ctx->kdrv_info.device_id);
          system(cli);
 
          // SUN_TOP_CTRL_SW_INIT_0_CLEAR, ghpy_sw_init=reset
          sprintf(cli, "spictl write 0x1040431c 0xFBFFFFFF 4 %d", ctx->kdrv_info.device_id);
          system(cli);

          // SUN_TOP_CTRL_GENERAL_CTRL_NO_SCAN_1, ldo off:
          sprintf(cli, "spictl write 0x104040a8 0x2 4 %d", ctx->kdrv_info.device_id);
          system(cli);
       }

       sprintf(cli, "spictl clearbits 0x10800808 0x3 4 %d", ctx->kdrv_info.device_id);
       system(cli);

       sprintf(cli, "spictl setbits 0x10800808 0x2000 4 %d", ctx->kdrv_info.device_id);
       system(cli);

       sprintf(cli, "spictl clearbits 0x10800808 0x2000 4 %d", ctx->kdrv_info.device_id);
       system(cli);

       // Flush TX FIFO - set bit
       sprintf(cli, "spictl write 0x10800b34 1 4 %d", ctx->kdrv_info.device_id);
       system(cli);
#else
       mocaos_setLinkStatus(handle, 0);
       if ((ctx->kdrv_info.chip_id & 0xFFFE0000) == 0x68020000)
       {
          // This is a 7xxx chip with a 680x chip connected via SPI
          // clear bits 0 and 1, set bit 13:
          sprintf(cli, "bbsictl 0x10800808 0x2000 0x2003");
          system(cli);

          // Flush TX FIFO - set bit
          sprintf(cli, "bbsictl 0x10800b34 0x1 0x1");
          system(cli);
       }
#endif
    }
    else // enable interface
    {
#if defined(DSL_MOCA)
       if (strcmp(board, "96802SV") == 0)
       {
          // SUN_TOP_CTRL_GENERAL_CTRL_NO_SCAN_1, ldo on:
          sprintf(cli, "spictl write 0x104040a8 0x1 4 %d", ctx->kdrv_info.device_id);
          system(cli);
 
          // SUN_TOP_CTRL_SW_INIT_0_CLEAR, ghpy_sw_init=reset 
          sprintf(cli, "spictl write 0x1040431c 0xFBFFFFFF 4 %d", ctx->kdrv_info.device_id);
          system(cli);

          // EPORT_REG_GPHY_CNTRL  
          sprintf(cli, "spictl clearbits 0x10800004 0xF 4 %d", ctx->kdrv_info.device_id);
          system(cli);

          // SUN_TOP_CTRL_SW_INIT_0_CLEAR, ghpy_sw_init=not_reset 
          sprintf(cli, "spictl write 0x1040431c 0xFFFFFFFF 4 %d", ctx->kdrv_info.device_id);
          system(cli);
       }

       sprintf(cli, "spictl clearbits 0x10800808 0x2000 4 %d", ctx->kdrv_info.device_id);
       system(cli);

       sprintf(cli, "spictl setbits 0x10800808 0x3 4 %d", ctx->kdrv_info.device_id);
       system(cli);

       // Flush TX FIFO - clear bit
       sprintf(cli, "spictl write 0x10800b34 0 4 %d", ctx->kdrv_info.device_id);
       system(cli);

       rc = bcm_enet_map_ifname_to_unit_port(ifname, &unit, &port);
       if ( 0 == rc )
       {
           bcm_set_extphylinkstatus(unit, port, PHY_CONN_TYPE_MOCA, 1, 0, 0);
       }
#else
       mocaos_setLinkStatus(handle, 1);
       if ((ctx->kdrv_info.chip_id & 0xFFFE0000) == 0x68020000)
       {
          // This is a 7xxx chip with a 680x chip connected via SPI
          // clear bit 13, set bits 0 and 1:
          sprintf(cli, "bbsictl 0x10800808 0x3 0x2003");
          system(cli);

          // Flush TX FIFO - clear bit
          sprintf(cli, "bbsictl 0x10800b34 0x0 0x1");
          system(cli);
       }
#
#endif
    }
}

// Try to allocate contiguous physical memory using the 'allocmem' kernel module
static unsigned int mocaos_AllocPhysMem1(unsigned int size, unsigned int **paddr)
{
    int rc;
    int file;

    // try a few locations
    system("insmod memalloc.ko > /dev/null 2>&1");
    system("insmod /lib/modules/memalloc.ko > /dev/null 2>&1");
    system("insmod /memalloc.ko > /dev/null 2>&1");
    system("insmod /mnt/memalloc.ko > /dev/null 2>&1");     
    system("mknod /dev/memalloc c 112 0 > /dev/null 2>&1");

    file = open("/dev/memalloc", O_NONBLOCK);

    if (!file)
    {
        return(1);
    }

    rc = ioctl(file, 0, &size);

    close(file);

    if (rc <= 0)
    {
        return(1);
    }

    *paddr = (unsigned int *)rc;
    
    return(0);
}

// try to alloc contiguous physical memory using the malloc/crossed fingers method 
static unsigned int mocaos_AllocPhysMem2(unsigned int size, unsigned int **paddr)
{
    int i, j;
    volatile unsigned int *x;
    volatile unsigned int *buffers[32];
    unsigned int *virtaddr;
    int k;

    /* We can't really allocate a chunk of physical memory in Linux userspace,
       but since this is only for a debug feature, it only needs to work
       "most of the time".  

       We allocate virtual memory via malloc.  The kernel will put it somewhere in
       physical memory.  If we are lucky, it will all be contiguous in memory.  We
       write a pattern to memory, and then read from /dev/mem (addressed via physical
       address), to match the pattern and figure out where the kernel put our block in
       physical memory.

       A few things can go wrong here:  
       1) The block may not be contiguous in memory.  In this case we allocate another
          block and try again.  We do this up to 32 times.
       2) The kernel may reallocate physical memory and move our buffer somewhere else.  In this
          case we're screwed, we can't lock the buffer in physical memory.  If you're running
          other programs that are allocating and deallocating memory, you have a greater risk
          of this happening.
    */
    
    int fd = open("/dev/mem", O_RDWR | O_SYNC);

    if (fd < 0)
    {
        MoCAOS_Printf(g_handle, "Unable to alloc physical memory!\n");
        return(1);
    }

    x  = mmap(NULL, 0x20000000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if ((unsigned int) x == (unsigned int)-1)
    {
        MoCAOS_Printf(g_handle, "Unable to map physical memory!\n");
        return(1);        
    }

    for (k=0;k<32;k++)
    {
        buffers[k] = virtaddr = (unsigned int *)malloc(size);
 
        // initialize buffer with some pattern
        for (i=0; i<(int)size/4;i++)
        {
            (virtaddr)[i] = i^0x12345679;
        }        

        (virtaddr)[size/4-1] = k;
    }

    // check all memory locations for that pattern
    for (i=0; i<0x10000000/4; i++)
    {
        for (j=0;j<(int)size/4-1;j++)
        {
            if (x[i+j] != (unsigned int)(j^0x12345679))
            {
                break;
            }
            x[i+j] = 0xAAAAAAAA;
        }
        if (j == (int)size/4-1) // found a match
            break;
    }

    k = x[i+j];
    
    if (j != (int)size/4-1) // found a match
    {
        i = 0;
        MoCAOS_Printf(g_handle, "Unable to allocate contiguous physical memory.  Reboot and try again. (%d, %d)\n",j,k);
    }

    for (j=0;j<32;j++)
    {
        if (j != k)
            free((void *)buffers[j]);
    }

    x[0x102a212c/sizeof(unsigned int)] = (i*4+256) & 0xFFFFFF00; // save address in MOCA_HOSTMISC_MMP_HOST_MMP15
    
    munmap((void *)x, 0x20000000);

    close(fd);

    *paddr = (unsigned int *)(i*4);
    
    return(0);
}

unsigned int MoCAOS_AllocPhysMem(unsigned int size, unsigned int **paddr)
{
    // try two different ways of allocating contiguous virtual memory.    
    if (mocaos_AllocPhysMem1(size, paddr) == 0)
        return(0);
    if (mocaos_AllocPhysMem2(size, paddr) == 0)
        return(0);    

    return(1);
}


MoCAOS_ClientHandle MoCAOS_ConnectToMocad(MoCAOS_Handle handle, const char *fmt, char *ifname)
{
    int fd;
    struct sockaddr_un unix_addr;
    char sockname[MOCA_FILENAME_LEN];
    int retVal;

    sprintf(sockname, fmt, ifname);

    fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);

    if(fd < 0)
        return(MoCAOS_CLIENT_NULL);
    
    memset(&unix_addr, 0, sizeof(unix_addr));
    unix_addr.sun_family = AF_UNIX;
    strcpy(unix_addr.sun_path, sockname);
    retVal = connect(fd, (const struct sockaddr *)&unix_addr,
            sizeof(unix_addr));

    if(retVal < 0)
    {
        close(fd);
        return(MoCAOS_CLIENT_NULL);
    }

    return((MoCAOS_ClientHandle)fd);
}

void MoCAOS_CloseClient(MoCAOS_Handle handle, MoCAOS_ClientHandle client)
{
    close((int)client);
}

MoCAOS_MutexHandle MoCAOS_MutexInit()
{   
    pthread_mutex_t *x; 

    x = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    
    pthread_mutex_init(x, NULL);
    
    return((MoCAOS_MutexHandle) x); return 0;
}

void MoCAOS_MutexLock(MoCAOS_MutexHandle x)
{
    pthread_mutex_lock((pthread_mutex_t *)x);
}

void MoCAOS_MutexUnlock(MoCAOS_MutexHandle x)
{
    pthread_mutex_unlock((pthread_mutex_t *)x);
}

void MoCAOS_MutexClose(MoCAOS_MutexHandle x)
{
    pthread_mutex_destroy((pthread_mutex_t *) x);
    free((void *)x);
}

int MoCAOS_MemAlign(void **memptr, int alignment, int size)
{
#if (defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200112L ) || (defined(_XOPEN_SOURCE) &&  _XOPEN_SOURCE >= 600)
// normally used posix_memalign
		if (posix_memalign(memptr, alignment, size) < 0)
			return -1;
#else
// use memalign if posix_memalign not available
		*memptr = memalign(alignment, size);
		if (*memptr == 0)
			return -1;
#endif

	return(0);
}

void MoCAOS_MSleep(int msec)
{
   usleep(msec*1000);
}

static unsigned int mocaos_get_phy_rate(LinuxOS_Context *ctx, uint32_t cpu_freq_hz)
{
   if (ctx->kdrv_info.hw_rev >= MOCA_CHIP_20_3)
   {
      if ((ctx->kdrv_info.chip_id == 0x714500A0) ||
          (ctx->kdrv_info.chip_id == 0x744500A0) ||
          (ctx->kdrv_info.chip_id == 0x744500B0))
      {
         return(300000000);
      }
      else if (ctx->kdrv_info.chip_id == 0x339000B0)  
      {
 		 return(450000000);
      }
	  else
      {
         return(400000000);
      }
   }
   if (ctx->kdrv_info.hw_rev >= MOCA_CHIP_20_4)
   {
      if (ctx->kdrv_info.chip_id == 0x339000B0)
      {
         return(450000000);
      }
      else // For now, return 450MHz for future chips
      {
         return(450000000);
      }
   }
   else
   {
      if ((ctx->kdrv_info.chip_id == 0x680200C0) && 
         (cpu_freq_hz == 440000000))
      {
         return(314000000);
      }
      else
      {
        return(300000000);
      }
   }
}

unsigned int MoCAOS_SetPhyClk(MoCAOS_Handle handle, uint32_t cpu_freq_hz)
{
   LinuxOS_Context *ctx = (LinuxOS_Context *)handle;
   unsigned int rate = mocaos_get_phy_rate(ctx, cpu_freq_hz);

   ioctl(ctx->device_fd, MOCA_IOCTL_SET_PHY_RATE, rate);

   return(rate);
}

unsigned int MoCAOS_SetCpuClk(MoCAOS_Handle handle, uint32_t freq_hz)
{
   LinuxOS_Context *ctx = (LinuxOS_Context *)handle;
   int rc = -1;

   if (ctx->kdrv_info.chip_id == 0x680200C0)
   {
      rc = ioctl(ctx->device_fd, MOCA_IOCTL_SET_CPU_RATE, freq_hz);
   }

   // If the set failed, the default frequency will be used
   if ((freq_hz == 0) || (rc < 0))
   {
      if (ctx->kdrv_info.chip_id == 0x680200C0)
         return(440000000);
      else if (ctx->kdrv_info.chip_id == 0x339000B0)
         return(450000000);
      else
         return(400000000);
   }
   else
   {
      return(freq_hz);
   }
}

unsigned int MoCAOS_GetRandomValue()
{
   FILE *fp = fopen("/dev/urandom","r");
   unsigned int val;
   int rc = 0;

   if (fp)
      rc=fread(&val, 4, 1, fp);

   if (!fp || (rc!=1))
   {
      unsigned int sec, usec;
      MoCAOS_GetTimeOfDay(&sec, &usec);
      srand(sec^usec);
      return(rand());
   }

   fclose(fp);

   return(val);
}

/* Used to formulate the paths /proc/device-tree/rdb/bmoca@f0e00000/brcm,avs-rmon-{hz|vt} */
#define DT_DIRPATH "/proc/device-tree/rdb/"
#define DT_PREFIX "bmoca"
#define DT_BMOCA_ADDRSZ sizeof("@ffffffff") /* 32bit address */
#define DT_AVS_RMON_HZ "/brcm,avs-rmon-hz" /* Must be >= sizeof(DT_AVS_RMON_VT) */
#define DT_AVS_RMON_VT "/brcm,avs-rmon-vt"

static int make_bmoca_devicetree_path(char *path)
{
   DIR *dir;
   struct dirent *de;

   dir = opendir(DT_DIRPATH);
   if (dir == NULL)
      return -1;

   while ((de = readdir(dir)) != NULL) {
      if (strncmp(de->d_name, DT_PREFIX, MIN(sizeof(de->d_name)-1, sizeof(DT_PREFIX)-1)) == 0)
        break;
   }
   closedir(dir); dir = NULL;

   if (de == NULL)
     return -2;
   if (strlen(de->d_name) > sizeof(DT_PREFIX)-1 + DT_BMOCA_ADDRSZ-1) {
     return -3;
   }

   memcpy(path, DT_DIRPATH, sizeof(DT_DIRPATH));
   memcpy(path + sizeof(DT_DIRPATH) - 1, de->d_name, strlen(de->d_name));
   path[sizeof(DT_DIRPATH) - 1 + strlen(de->d_name)] = '\0';
   return 0;
}

static int set_rmon_value(const char *name, int namesz, char *fnamebuf, int offset, unsigned int *dest)
{
   int fd;
   int ret = 0;

   memcpy(fnamebuf + offset, name, namesz);

   fd = open(fnamebuf, O_RDONLY);
   if (fd == -1)
     return -2;

   if (read(fd, dest, sizeof(*dest)) != sizeof(*dest))
     ret = -1;
   *dest = BE32(*dest);

   close(fd);
   return ret;
}

int MoCAOS_GetRMON(unsigned int *rmon_hz, unsigned int *rmon_vt)
{
   char filename[sizeof(DT_DIRPATH) + 
		 sizeof(DT_PREFIX) + 
		 DT_BMOCA_ADDRSZ + 
		 sizeof(DT_AVS_RMON_HZ)]; /* 3 bytes too large due to 4 uses of sizeof */
   int pathlen;

   if (make_bmoca_devicetree_path(filename))
      return -1;
   pathlen = strlen(filename);

   if (set_rmon_value(DT_AVS_RMON_HZ, sizeof(DT_AVS_RMON_HZ), filename, pathlen, rmon_hz))
      return -2;
   if (set_rmon_value(DT_AVS_RMON_VT, sizeof(DT_AVS_RMON_VT), filename, pathlen, rmon_vt))
      return -3;

   return 0;
}

void MoCAOS_FreeMemAlign(void *x)
{
   free(x);
}

void MoCAOS_Alarm(unsigned int seconds)
{
   alarm(seconds);
}

int MoCAOS_SetLedMode(MoCAOS_Handle handle, uint32_t led_mode)
{
   LinuxOS_Context *ctx = (LinuxOS_Context *)handle;
   char cli[64];

   // Set/clear TX_ACT_EN & RX_ACT_EN bits in EPORT_REG_LED_CNTRL
#if defined(DSL_MOCA)
   sprintf(cli, "spictl %s 0x10800024 0x30 4 %d",
         (led_mode==1 ? "clearbits" : "setbits"), ctx->kdrv_info.device_id);
   system(cli);
#else
   if ((ctx->kdrv_info.chip_id & 0xFFFE0000) == 0x68020000)
   {
      // This is a 7xxx chip with a 680x chip connected via SPI
      sprintf(cli, "bbsictl 0x10800024 0x%x 0x30", led_mode==1?0:0x30);
      system(cli);
   }
#endif

   return(0);
}

int MoCAOS_SetLoopback(MoCAOS_Handle handle, uint32_t enable, uint32_t device_id)
{
   LinuxOS_Context *ctx = (LinuxOS_Context *)handle;
   char cli[64];

#if defined(DSL_MOCA)
   sprintf(cli, "spictl %s 0x10800808 0x8000 4 %d",
         (enable ? "setbits" : "clearbits"), ctx->kdrv_info.device_id);
   system(cli);
#else
   if ((ctx->kdrv_info.chip_id & 0xFFFE0000) == 0x68020000)
   {
      // This is a 7xxx chip with a 680x chip connected via SPI
      sprintf(cli, "bbsictl 0x10800808 0x%x 0x8000", enable? 0x8000:0);
      system(cli);
   }
#endif
   return 0;
}


int MoCAOS_SetSSC(MoCAOS_Handle handle, int enable)
{
   LinuxOS_Context *ctx = (LinuxOS_Context *)handle;
   int rc = -1;

   rc = ioctl(ctx->device_fd, MOCA_IOCTL_CLK_SSC, &enable);
   
   return(rc);
}


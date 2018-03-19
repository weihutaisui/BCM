/*
 * Wl server for linux
 *
 * Copyright 2017 Broadcom
 *
 * This program is the proprietary software of Broadcom and/or
 * its licensors, and may only be used, duplicated, modified or distributed
 * pursuant to the terms and conditions of a separate, written license
 * agreement executed between you and Broadcom (an "Authorized License").
 * Except as set forth in an Authorized License, Broadcom grants no license
 * (express or implied), right to use, or waiver of any kind with respect to
 * the Software, and Broadcom expressly reserves all rights in and to the
 * Software and all intellectual property rights therein.  IF YOU HAVE NO
 * AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY
 * WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF
 * THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1. This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof, and to
 * use this information only in connection with your use of Broadcom
 * integrated circuit products.
 *
 * 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
 * "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR
 * OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL,
 * SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR
 * IN ANY WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 * IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii)
 * ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF
 * OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY
 * NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
 *
 *
 * <<Broadcom-WL-IPTag/Proprietary:>>
 *
 * $Id: wlu_server_linux.c 629921 2016-04-06 22:39:18Z $
 */

/* Revision History:Linux port of Remote wl server
 *
 * Date        Author         Description
 *
 * 27-Dec-2007 Suganthi        Version 0.1
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <malloc.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#include <sys/ioctl.h>
#include <net/if.h>
#ifndef TARGETENV_android
#include <linux/types.h>
#endif // endif

#include <linux/sockios.h>
#include <linux/ethtool.h>

#include <typedefs.h>
#include <wlioctl.h>
#include <dhdioctl.h>
#include <proto/ethernet.h>
#include <bcmendian.h>
#include <bcmutils.h>
#include <bcmcdc.h>
#include <proto/802.11.h>
#include "wlu_remote.h"

#define DEV_TYPE_LEN    3 /* length for devtype 'wl'/'et' */
#define BUF_LENGTH		1000

#ifdef IL_BIGENDIAN
bool swap = FALSE;
#endif // endif
const char *rwl_server_name;
void *g_wl_handle;
unsigned short defined_debug = DEBUG_ERR | DEBUG_INFO;
extern int remote_server_exec(int argc, char **argv, void *ifr);

/* Global to have the PID of the current sync command
 * This is required in case the sync command fails to respond,
 * the alarm handler shall kill the PID upon a timeout
 */
int g_shellsync_pid;
unsigned char g_return_stat = 0;

static void
syserr(char *s)
{
	fprintf(stderr, "%s: ", rwl_server_name);
	perror(s);
	exit(errno);
}
/* The handler for DHD commands */
int
dhd_ioctl(void *dhd, int cmd, void *buf, int len, bool set)
{
	struct ifreq *ifr = (struct ifreq *) dhd;
	dhd_ioctl_t ioc;
	int ret = 0;
	int s;

	/* open socket to kernel */
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		syserr("socket");

	/* do it */
	ioc.cmd = cmd;
	ioc.buf = buf;
	ioc.len = len;
	ioc.set = set;
	ioc.driver = DHD_IOCTL_MAGIC;
	ifr->ifr_data = (caddr_t) &ioc;
	if ((ret = ioctl(s, SIOCDEVPRIVATE, ifr)) < 0) {
		if (cmd != DHD_GET_MAGIC) {
			ret = BCME_IOCTL_ERROR;
		}
	}
	/* cleanup */
	close(s);
	return ret;
}

int
wl_ioctl(void *wl, int cmd, void *buf, int len, bool set)
{
	struct ifreq *ifr = (struct ifreq *) wl;
	wl_ioctl_t ioc;
	int ret = 0;
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
			ret = BCME_IOCTL_ERROR;
		}
	}
	/* cleanup */
	close(s);
	return ret;
}

/* Functions copied from wlu.c to check for the driver adapter in the server machine */
int
wl_check(void *wl)
{
	int ret;
	int val;

	if ((ret = wl_ioctl(wl, WLC_GET_MAGIC, &val, sizeof(int), FALSE)) < 0)
		return ret;
#ifdef IL_BIGENDIAN
	/* Detect if IOCTL swapping is necessary */
	if (val == (int)bcmswap32(WLC_IOCTL_MAGIC))
	{
		val = bcmswap32(val);
		swap = TRUE;
	}
#endif // endif
	if (val != WLC_IOCTL_MAGIC)
		return -1;
	if ((ret = wl_ioctl(wl, WLC_GET_VERSION, &val, sizeof(int), FALSE)) < 0)
		return ret;
	val = dtoh32(val);

	if (val > WLC_IOCTL_VERSION) {
		fprintf(stderr, "Version mismatch, please upgrade\n");
		return -1;
	}
	return 0;
}

static int
wl_get_dev_type(char *name, void *buf, int len)
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
	strncpy(ifr.ifr_name, name, IFNAMSIZ);
	if ((ret = ioctl(s, SIOCETHTOOL, &ifr)) < 0) {
	/* print a good diagnostic if not superuser */
		if (errno == EPERM)
			syserr("wl_get_dev_type");
		*(char *)buf = '\0';
	} else {
		strncpy(buf, info.driver, len);
	}
	close(s);
	return ret;
}

static void
wl_find_adapter(struct ifreq *ifr)
{
	char proc_net_dev[] = "/proc/net/dev";
	FILE *fp;
	char buf[BUF_LENGTH], *c, *name;
	char dev_type[DEV_TYPE_LEN];

	ifr->ifr_name[0] = '\0';

	if (!(fp = fopen(proc_net_dev, "r")))
		return;

	/* eat first two lines */
	if (!fgets(buf, sizeof(buf), fp) ||
	    !fgets(buf, sizeof(buf), fp)) {
		fclose(fp);
		return;
	}

	while (fgets(buf, sizeof(buf), fp)) {
		c = buf;
		while (isspace(*c))
			c++;
		if (!(name = strsep(&c, ":")))
			continue;
		strncpy(ifr->ifr_name, name, IFNAMSIZ-1);
		ifr->ifr_name[IFNAMSIZ-1] = 0;
		if (wl_get_dev_type(name, dev_type, DEV_TYPE_LEN) >= 0 &&
			!strncmp(dev_type, "wl", 2))
		if (wl_check((void *) ifr) == 0)
				break;
		ifr->ifr_name[0] = '\0';
	}

	fclose(fp);
}

int
wl_get(void *wl, int cmd, void *buf, int len)
{
	int error;

	error = wl_ioctl(wl, cmd, buf, len, FALSE);

	return error;
}

int
wl_set(void *wl, int cmd, void *buf, int len)
{
	int error;

	error = wl_ioctl(wl,  cmd, buf, len, TRUE);

	return error;
}

extern int set_ctrlc;
void handle_ctrlc(int unused)
{
	UNUSED_PARAMETER(unused);
	set_ctrlc = 1;
	return;
}

volatile sig_atomic_t g_sig_chld = 1;
void rwl_chld_handler(int num)
{
	int child_status;

	UNUSED_PARAMETER(num);
	/* g_return_stat is being set with the return status of sh commands */
	waitpid(g_shellsync_pid, &child_status, WNOHANG);
	if (WIFEXITED(child_status))
		g_return_stat = WEXITSTATUS(child_status);
	else if (g_rem_ptr->msg.flags == (unsigned)CTRLC_FLAG)
		g_return_stat = 0;
	else
		g_return_stat = 1;
	g_sig_chld = 0;
}

/* Alarm handler called after SHELL_TIMEOUT value
 * This handler kills the non-responsive shell process
 * with the PID value g_shellsync_pid
 */
static void
sigalrm_handler(int s)
{
	UNUSED_PARAMETER(s);

	if (g_shellsync_pid) {
		kill(g_shellsync_pid, SIGINT);
	}
#ifdef RWL_SOCKET
	g_sig_chld = 0;
#endif // endif
}

static void
def_handler(int s)
{
	UNUSED_PARAMETER(s);
	kill(g_shellsync_pid, SIGKILL);
	exit(0);
}

static void
pipe_handler(int s)
{
	UNUSED_PARAMETER(s);
	kill(g_shellsync_pid, SIGKILL);
}

int
main(int argc, char **argv)
{
	int err = 0;
	struct ifreq *ifr;

	rwl_server_name = argv[0];

	if ((ifr = calloc(1, sizeof(*ifr))) == NULL)
	{
		DPRINT_ERR(ERR, "wl_server: Unable to allocate memory for handle\n");
		exit(1);
	}

	/* use default interface */
	if (!ifr->ifr_name[0])
		wl_find_adapter(ifr);
	/* validate the interface */
	if (!ifr->ifr_name[0] || wl_check((void *)ifr)) {
		DPRINT_INFO(OUTPUT, "wl_server: wl driver adapter not found\n");
	}
	g_wl_handle = ifr;

	/* Register signal handlers */
	signal(SIGCHLD, rwl_chld_handler);
	signal(SIGALRM, sigalrm_handler);
	signal(SIGTERM, def_handler);
	signal(SIGPIPE, pipe_handler);
	signal(SIGABRT, def_handler);
#ifdef RWL_DONGLE
	signal(SIGINT, handle_ctrlc);
#endif // endif
	/* Main server process for all transport types */
	err = remote_server_exec(argc, argv, ifr);
	free(ifr);
	return err;
}

/*
 * Funtion to store old interface.
 */
void
store_old_interface(void *wl, char *old_intf_name)
{
	strcpy(old_intf_name, ((struct ifreq *)wl)->ifr_name);
}

/*
 * Function to set interface.
 */
int
set_interface(void *wl, char *intf_name)
{
	struct ifreq *ifr = (struct ifreq *)wl;

	if (strlen(intf_name) != 0) {
		strncpy(ifr->ifr_name, intf_name, strlen(intf_name) + 1);
		return BCME_OK;
	}
	else {
		DPRINT_DBG(OUTPUT, "Default Interface will be used ... \n");
		return BCME_ERROR;
	}
}

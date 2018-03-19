/*
 * Linux port of dhd command line utility, hacked from wl utility.
 *
 * Copyright (C) 2017, Broadcom. All Rights Reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *
 * <<Broadcom-WL-IPTag/Open:>>
 *
 * $Id: dhdu_linux.c 596203 2015-10-30 05:40:09Z $
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <proto/ethernet.h>
#include <proto/bcmip.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#ifndef TARGETENV_android
#ifndef __FreeBSD__
#include <error.h>
#endif // endif
typedef u_int64_t u64;
typedef u_int32_t u32;
typedef u_int16_t u16;
typedef u_int8_t u8;
#endif /* TARGETENV_android */
#ifndef __FreeBSD__
#include <linux/sockios.h>
#include <linux/types.h>
#include <linux/ethtool.h>
#endif // endif

#include <typedefs.h>
#include <signal.h>
#include <dhdioctl.h>
#include <wlioctl.h>
#include <bcmcdc.h>
#include <bcmutils.h>

#if defined(RWL_WIFI) || defined(RWL_DONGLE) || defined(RWL_SOCKET) \
	||defined(RWL_SERIAL)
#define RWL_ENABLE
#endif /* RWL_WIFI | RWL_DONGLE | RWL_SOCKET | RWL_SERIAL */

#include "dhdu.h"
#ifdef RWL_ENABLE
#include "wlu_remote.h"
#include "wlu_client_shared.h"
#include "wlu_pipe.h"
#endif /* RWL_ENABLE */
#include <netdb.h>
#include <netinet/in.h>
#include <dhdioctl.h>
#include "dhdu_common.h"
#include "dhdu_nl80211.h"

char *av0;
static int rwl_os_type = LINUX_OS;

static int process_args(struct ifreq* ifr, char **argv);

/* Search the dhd_cmds table for a matching command name.
 * Return the matching command or NULL if no match found.
 */
static cmd_t *
dhd_find_cmd(char* name)
{
	cmd_t *cmd = NULL;
	/* search the dhd_cmds for a matching name */
	for (cmd = dhd_cmds; cmd->name && strcmp(cmd->name, name); cmd++);
	if (cmd->name == NULL)
		cmd = NULL;
	return cmd;
}

#ifdef NL80211
static void
syserr(const char *s)
{
}

static int __dhd_drvio_nl_vndr_cmd(void *dhd, dhd_ioctl_t *ioc)
{
	struct dhd_netlink_info dhd_nli;
	struct ifreq *ifr = (struct ifreq *)dhd;
	int ret = 0;

	dhd_nli.ifidx = if_nametoindex(ifr->ifr_name);
	if (!dhd_nli.ifidx) {
		fprintf(stderr, "invalid device %s\n", ifr->ifr_name);
		return BCME_IOCTL_ERROR;
	}

	if (dhd_nl_sock_connect(&dhd_nli) < 0) {
		fprintf(stderr, "%s: ", av0);
		perror("socket");
		exit(errno);
	}

	ret = dhd_nl_do_vndr_cmd(&dhd_nli, ioc);
	dhd_nl_sock_disconnect(&dhd_nli);
	return ret;
}
#else
static void
syserr(const char *s)
{
	fprintf(stderr, "%s: ", av0);
	perror(s);
	exit(errno);
}

static int __dhd_drvio_nl_vndr_cmd(void *dhd, dhd_ioctl_t *ioc)
{
	return BCME_IOCTL_ERROR;
}
#endif /* NL80211 */
static int __dhd_drvio_ioctl(void *dhd, dhd_ioctl_t *ioc)
{
	struct ifreq *ifr = (struct ifreq *)dhd;
	int s;
	int ret = 0;

	/* pass ioctl data */
	ifr->ifr_data = (caddr_t)ioc;

	/* open socket to kernel */
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		syserr("socket");

	ret = ioctl(s, SIOCDEVPRIVATE, ifr);
	if (ret < 0 && errno != EAGAIN)
		syserr(__FUNCTION__);

	/* cleanup */
	close(s);
	return ret;
}

static int (*g_dhd_driver_io)(void *dhd, dhd_ioctl_t *ioc) = __dhd_drvio_ioctl;
/* This function is called by ioctl_setinformation_fe or ioctl_queryinformation_fe
 * for executing  remote commands or local commands
 */
static int
dhd_ioctl(void *dhd, int cmd, void *buf, int len, bool set)
{
	dhd_ioctl_t ioc;
	int ret = 0;

	/* By default try to execute wl commands */
	int driver_magic = WLC_IOCTL_MAGIC;
	int get_magic = WLC_GET_MAGIC;

	/* For local dhd commands execute dhd. For wifi transport we still
	 * execute wl commands.
	 */
	if (remote_type == NO_REMOTE && strncmp (buf, RWL_WIFI_ACTION_CMD,
		strlen(RWL_WIFI_ACTION_CMD)) && strncmp(buf, RWL_WIFI_GET_ACTION_CMD,
		strlen(RWL_WIFI_GET_ACTION_CMD))) {
		driver_magic = DHD_IOCTL_MAGIC;
		get_magic = DHD_GET_MAGIC;
	}

	/* do it */
	ioc.cmd = cmd;
	ioc.buf = buf;
	ioc.len = len;
	ioc.set = set;
	ioc.driver = driver_magic;

	ret = g_dhd_driver_io(dhd, &ioc);
	if (ret < 0) {
		if (g_dhd_driver_io == __dhd_drvio_ioctl) {
			/* try using nl80211 testmode interface */
			ret = __dhd_drvio_nl_vndr_cmd(dhd, &ioc);
			if (ret >= 0) {
				g_dhd_driver_io = __dhd_drvio_nl_vndr_cmd;
				goto exit;
			}
		}
		if (cmd != get_magic)
			ret = BCME_IOCTL_ERROR;
	}

exit:
	return ret;
}

/* This function is called in wlu_pipe.c remote_wifi_ser_init() to execute
 * the initial set of wl commands for wifi transport (e.g slow_timer, fast_timer etc)
 */
int wl_ioctl(void *wl, int cmd, void *buf, int len, bool set)
{
	return dhd_ioctl(wl, cmd, buf, len, set); /* Call actual wl_ioctl here: Shubhro */
}

/* Search if dhd adapter or wl adapter is present
 * This is called by dhd_find to check if it supports wl or dhd
 * The reason for checking wl adapter is that we can still send remote dhd commands over
 * wifi transport.
 */
#ifdef __FreeBSD__
static int
dhd_get_dev_type(char *name, void *buf, char *type)
{
	return 0;
}
#else
static int
dhd_get_dev_type(char *name, void *buf, char *type)
{
	int s;
	int ret;
	struct ifreq ifr;
	struct ethtool_drvinfo info;
	unsigned len;

	/* get device type */
	memset(&info, 0, sizeof(info));
	info.cmd = ETHTOOL_GDRVINFO;
	strcpy(info.driver, "?");
	len = strlen(type);
	if (len + strlen(info.driver) >= sizeof(info.driver)) {
		return BCME_ERROR;
	}
	strncat(info.driver, type, len);
	ifr.ifr_data = (caddr_t)&info;
	strncpy(ifr.ifr_name, name, IFNAMSIZ);
	ifr.ifr_name[IFNAMSIZ - 1] = '\0';
	/* open socket to kernel */
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		syserr("socket");

	if ((ret = ioctl(s, SIOCETHTOOL, &ifr)) < 0) {

		if (errno != EAGAIN)
			syserr(__FUNCTION__);

		*(char *)buf = '\0';
	}
	else
		strcpy(buf, info.driver);

	close(s);
	return ret;
}
#endif /* __FreeBSD__ */

/* dhd_get/dhd_set is called by several functions in dhdu.c. This used to call dhd_ioctl
 * directly. However now we need to execute the dhd commands remotely.
 * So we make use of wl pipes to execute this.
 * wl_get or wl_set functions also check if it is a local command hence they in turn
 * call dhd_ioctl if required. Name wl_get/wl_set is retained because these functions are
 * also called by wlu_pipe.c wlu_client_shared.c
 */
int
dhd_get(void *dhd, int cmd, void *buf, int len)
{
	return wl_get(dhd, cmd, buf, len);
}

/*
 * To use /dev/node interface:
 *   1.  mknod /dev/hnd0 c 248 0
 *   2.  chmod 777 /dev/hnd0
 */
#define NODE "/dev/hnd0"

int
dhd_set(void *dhd, int cmd, void *buf, int len)
{
	static int dnode = -1;

	switch (cmd) {
	case DHD_DLDN_ST:
		if (dnode == -1)
			dnode = open(NODE, O_RDWR);
		else
			fprintf(stderr, "devnode already opened!\n");

		return dnode;
		break;
	case DHD_DLDN_WRITE:
		if (dnode > 0)
			return write(dnode, buf, len);
		break;
	case DHD_DLDN_END:
		if (dnode > 0)
			return close(dnode);
		break;
	default:
		return wl_set(dhd, cmd, buf, len);

	}

	return -1;
}

/* Verify the wl adapter found.
 * This is called by dhd_find to check if it supports wl
 * The reason for checking wl adapter is that we can still send remote dhd commands over
 * wifi transport. The function is copied from wlu.c.
 */
int
wl_check(void *wl)
{
	int ret;
	int val = 0;

	if (!dhd_check (wl))
		return 0;

	/*
	 *  If dhd_check() fails then go for a regular wl driver verification
	 */
	if ((ret = wl_get(wl, WLC_GET_MAGIC, &val, sizeof(int))) < 0)
		return ret;
	if (val != WLC_IOCTL_MAGIC)
		return BCME_ERROR;
	if ((ret = wl_get(wl, WLC_GET_VERSION, &val, sizeof(int))) < 0)
		return ret;
	if (val > WLC_IOCTL_VERSION) {
		fprintf(stderr, "Version mismatch, please upgrade\n");
		return BCME_ERROR;
	}
	return 0;
}
/* Search and verify the request type of adapter (wl or dhd)
 * This is called by main before executing local dhd commands
 * or sending remote dhd commands over wifi transport
 */
void
dhd_find(struct ifreq *ifr, char *type)
{
	char proc_net_dev[] = "/proc/net/dev";
	FILE *fp = NULL;
	static char buf[400];
	char *c, *name;
	char dev_type[32];

	ifr->ifr_name[0] = '\0';
	/* eat first two lines */
	if (!(fp = fopen(proc_net_dev, "r")) ||
	    !fgets(buf, sizeof(buf), fp) ||
	    !fgets(buf, sizeof(buf), fp))
		goto exit;

	while (fgets(buf, sizeof(buf), fp)) {
		c = buf;
		while (isspace(*c))
			c++;
		if (!(name = strsep(&c, ":")))
			continue;
		strncpy(ifr->ifr_name, name, IFNAMSIZ);
		if (dhd_get_dev_type(name, dev_type, type) >= 0 &&
			!strncmp(dev_type, type, strlen(dev_type) - 1))
		{
			if (!wl_check((void*)ifr))
				break;
		}
		ifr->ifr_name[0] = '\0';
	}

exit:
	if (fp)
		fclose(fp);
}
/* This function is called by wl_get to execute either local dhd command
 * or send a dhd command over wl transport
 */
static int
ioctl_queryinformation_fe(void *wl, int cmd, void* input_buf, int *input_len)
{
	if (remote_type == NO_REMOTE) {
		return dhd_ioctl(wl, cmd, input_buf, *input_len, FALSE);
	}
#ifdef RWL_ENABLE
	else {
		return rwl_queryinformation_fe(wl, cmd, input_buf,
			(unsigned long*)input_len, 0, RDHD_GET_IOCTL);
	}
#else /* RWL_ENABLE */
	return BCME_IOCTL_ERROR;
#endif /* RWL_ENABLE */
}

/* This function is called by wl_set to execute either local dhd command
 * or send a dhd command over wl transport
 */
static int
ioctl_setinformation_fe(void *wl, int cmd, void* buf, int *len)
{
	if (remote_type == NO_REMOTE) {
		return dhd_ioctl(wl,  cmd, buf, *len, TRUE);
	}
#ifdef RWL_ENABLE
	else {
		return rwl_setinformation_fe(wl, cmd, buf, (unsigned long*)len, 0, RDHD_SET_IOCTL);

	}
#else /* RWL_ENABLE */
	return BCME_IOCTL_ERROR;
#endif /* RWL_ENABLE */
}

/* The function is replica of wl_get in wlu_linux.c. Optimize when we have some
 * common code between wlu_linux.c and dhdu_linux.c
 */
int
wl_get(void *wl, int cmd, void *buf, int len)
{
	int error = BCME_OK;
	/* For RWL: When interfacing to a Windows client, need t add in OID_BASE */
	if ((rwl_os_type == WIN32_OS) && (remote_type != NO_REMOTE)) {
		error = (int)ioctl_queryinformation_fe(wl, WL_OID_BASE + cmd, buf, &len);
	} else {
		error = (int)ioctl_queryinformation_fe(wl, cmd, buf, &len);
	}
	if (error == BCME_SERIAL_PORT_ERR)
		return BCME_SERIAL_PORT_ERR;

	if (error != 0)
		return BCME_IOCTL_ERROR;

	return error;
}

/* The function is replica of wl_set in wlu_linux.c. Optimize when we have some
 * common code between wlu_linux.c and dhdu_linux.c
 */
int
wl_set(void *wl, int cmd, void *buf, int len)
{
	int error = BCME_OK;

	/* For RWL: When interfacing to a Windows client, need t add in OID_BASE */
	if ((rwl_os_type == WIN32_OS) && (remote_type != NO_REMOTE)) {
		error = (int)ioctl_setinformation_fe(wl, WL_OID_BASE + cmd, buf, &len);
	} else {
		error = (int)ioctl_setinformation_fe(wl, cmd, buf, &len);
	}

	if (error == BCME_SERIAL_PORT_ERR)
		return BCME_SERIAL_PORT_ERR;

	if (error != 0) {
		return BCME_IOCTL_ERROR;
	}
	return error;
}

int
wl_validatedev(void *dev_handle)
{
	int retval = 1;
	struct ifreq *ifr = (struct ifreq *)dev_handle;
	/* validate the interface */
	if (!strlen(ifr->ifr_name) || wl_check((void *)ifr)) {
		retval = 0;
	}
	return retval;
}

/* Main client function
 * The code is mostly from wlu_linux.c. This function takes care of executing remote dhd commands
 * along with the local dhd commands now.
 */
int
main(int argc, char **argv)
{
	struct ifreq ifr;
	char *ifname = NULL;
	int err = 0;
	int help = 0;
	int status = CMD_DHD;
#ifdef RWL_DONGLE
	void* serialHandle = NULL;
#endif /* RWL_DONGLE */
#ifdef RWL_SOCKET
	struct ipv4_addr temp;
#endif /* RWL_SOCKET */

	UNUSED_PARAMETER(argc);

	av0 = argv[0];
	memset(&ifr, 0, sizeof(ifr));
	argv++;

	if ((status = dhd_option(&argv, &ifname, &help)) == CMD_OPT) {
		if (ifname)
			strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	}
	/* Linux client looking for a Win32 server */
	if (*argv && strncmp (*argv, "--wince", strlen(*argv)) == 0) {
		rwl_os_type = WIN32_OS;
		argv++;
	}

	/* RWL socket transport Usage: --socket ipaddr [port num] */
	if (*argv && strncmp (*argv, "--socket", strlen(*argv)) == 0) {
		argv++;

		remote_type = REMOTE_SOCKET;
#ifdef RWL_SOCKET
		if (!(*argv)) {
			rwl_usage(remote_type);
			return err;
		}

		if (!dhd_atoip(*argv, &temp)) {
			rwl_usage(remote_type);
			return err;
		}
		g_rwl_servIP = *argv;
		argv++;

		g_rwl_servport = DEFAULT_SERVER_PORT;
		if ((*argv) && isdigit(**argv)) {
			g_rwl_servport = atoi(*argv);
			argv++;
		}
#endif /* RWL_SOCKET */
	}

	/* RWL from system serial port on client to uart dongle port on server */
	/* Usage: --dongle /dev/ttyS0 */
	if (*argv && strncmp (*argv, "--dongle", strlen(*argv)) == 0) {
		argv++;
		remote_type = REMOTE_DONGLE;
#ifdef RWL_DONGLE
		if (!(*argv)) {
			rwl_usage(remote_type);
			return err;
		}
		g_rwl_device_name_serial = *argv;
		argv++;
		if ((serialHandle = rwl_open_pipe(remote_type, "\0", 0, 0)) == NULL) {
			DPRINT_ERR(ERR, "serial device open error\r\n");
			return BCME_ERROR;
		}
		ifr = (*(struct ifreq *)serialHandle);
#endif /* RWL_DONGLE */
	}

	/* RWL over wifi.  Usage: --wifi mac_address */
	if (*argv && strncmp (*argv, "--wifi", strlen(*argv)) == 0) {
		argv++;
#ifdef RWL_WIFI
		remote_type = NO_REMOTE;
		if (!ifr.ifr_name[0])
		{
			dhd_find(&ifr, "wl");
		}
		/* validate the interface */
		if (!ifr.ifr_name[0] || wl_check((void*)&ifr)) {
			fprintf(stderr, "%s: wl driver adapter not found\n", av0);
			exit(1);
		}
		remote_type = REMOTE_WIFI;

		if (argc < 4) {
			rwl_usage(remote_type);
			return err;
		}
		/* copy server mac address to local buffer for later use by findserver cmd */
		if (!dhd_ether_atoe(*argv, (struct ether_addr *)g_rwl_buf_mac)) {
			fprintf(stderr,
			        "could not parse as an ethernet MAC address\n");
			return FAIL;
		}
		argv++;
#else /* RWL_WIFI */
		remote_type = REMOTE_WIFI;
#endif /* RWL_WIFI */
	}

	/* Process for local dhd */
	if (remote_type == NO_REMOTE) {
		err = process_args(&ifr, argv);
		return err;
	}

#ifdef RWL_ENABLE
	if (*argv) {
		err = process_args(&ifr, argv);
		if ((err == BCME_SERIAL_PORT_ERR) && (remote_type == REMOTE_DONGLE)) {
			DPRINT_ERR(ERR, "\n Retry again\n");
			err = process_args((struct ifreq*)&ifr, argv);
		}
		return err;
	}
	rwl_usage(remote_type);
#endif /* RWL_ENABLE */

#ifdef RWL_DONGLE
	if (remote_type == REMOTE_DONGLE)
		rwl_close_pipe(remote_type, (void*)&ifr);
#endif /* RWL_DONGLE */
	return err;
}
/*
 * Function called for  'local' execution and for 'remote' non-interactive session
 * (shell cmd, wl cmd) .The code is mostly from wlu_linux.c. This code can be
 * common to wlu_linux.c and dhdu_linux.c
 */
static int
process_args(struct ifreq* ifr, char **argv)
{
	char *ifname = NULL;
	int help = 0;
	int status = 0;
	int err = BCME_OK;
	cmd_t *cmd = NULL;
	while (*argv) {
#ifdef RWL_ENABLE
		if ((strcmp (*argv, "sh") == 0) && (remote_type != NO_REMOTE)) {
			argv++; /* Get the shell command */
			if (*argv) {
				/* Register handler in case of shell command only */
				signal(SIGINT, ctrlc_handler);
				err = rwl_shell_cmd_proc((void*)ifr, argv, SHELL_CMD);
			} else {
				DPRINT_ERR(ERR,
				"Enter the shell command (e.g ls(Linux) or dir(Win CE) \n");
				err = BCME_ERROR;
			}
			return err;
		}
#endif /* RWL_ENABLE */
		if ((status = dhd_option(&argv, &ifname, &help)) == CMD_OPT) {
			if (help)
				break;
			if (ifname)
				strncpy(ifr->ifr_name, ifname, IFNAMSIZ);
			continue;
		}
		/* parse error */
		else if (status == CMD_ERR)
		    break;

		if (remote_type == NO_REMOTE) {
			int ret;

			/* use default interface */
			if (!ifr->ifr_name[0])
				dhd_find(ifr, "dhd");
			/* validate the interface */
			if (!ifr->ifr_name[0]) {
				if (strcmp("dldn", *argv) != 0) {
					exit(ENXIO);
					syserr("interface");
				}
			}
			if ((ret = dhd_check((void *)ifr)) != 0) {
				if (strcmp("dldn", *argv) != 0) {
					errno = -ret;
					syserr("dhd_check");
				}
			}
		}
		/* search for command */
		cmd = dhd_find_cmd(*argv);
		/* if not found, use default set_var and get_var commands */
		if (!cmd) {
			cmd = &dhd_varcmd;
		}

		/* do command */
		err = (*cmd->func)((void *) ifr, cmd, argv);
		break;
	} /* while loop end */

	/* provide for help on a particular command */
	if (help && *argv) {
		cmd = dhd_find_cmd(*argv);
		if (cmd) {
			dhd_cmd_usage(cmd);
		} else {
			DPRINT_ERR(ERR, "%s: Unrecognized command \"%s\", type -h for help\n",
			           av0, *argv);
		}
	} else if (!cmd)
		dhd_usage(NULL);
	else if (err == BCME_USAGE_ERROR)
		dhd_cmd_usage(cmd);
	else if (err == BCME_IOCTL_ERROR)
		dhd_printlasterror((void *) ifr);

	return err;
}

/*
* Search a string backwards for a set of characters
* This is the reverse version of strspn()
*
* @param	s	string to search backwards
* @param	accept	set of chars for which to search
* @return	number of characters in the trailing segment of s
*		which consist only of characters from accept.
*/
static size_t
sh_strrspn(const char *s, const char *accept)
{
	const char *p;
	size_t accept_len = strlen(accept);
	int i;

	if (s[0] == '\0') {
		return 0;
	}

	p = s + strlen(s);
	i = 0;
	do {
		p--;
		if (memchr(accept, *p, accept_len) == NULL) {
			break;
		}
		i++;
	} while (p != s);

	return i;
}

/*
* Parse the unit and subunit from an interface string such as wlXX or wlXX.YY
*
* @param	ifname	interface string to parse
* @param	unit	pointer to return the unit number, may pass NULL
* @param	subunit	pointer to return the subunit number, may pass NULL
* @return	Returns 0 if the string ends with digits or digits.digits, -1 otherwise.
*		If ifname ends in digits.digits, then unit and subuint are set
*		to the first and second values respectively. If ifname ends
*		in just digits, unit is set to the value, and subunit is set
*		to -1. On error both unit and subunit are -1. NULL may be passed
*		for unit and/or subuint to ignore the value.
*/
static int
get_ifname_unit(const char* ifname, int *unit, int *subunit)
{
	const char digits[] = "0123456789";
	char str[64];
	char *p;
	size_t ifname_len = strlen(ifname);
	size_t len;
	unsigned long val;

	if (unit) {
		*unit = -1;
	}
	if (subunit) {
		*subunit = -1;
	}

	if (ifname_len + 1 > sizeof(str)) {
		return -1;
	}

	strcpy(str, ifname);

	/* find the trailing digit chars */
	len = sh_strrspn(str, digits);

	/* fail if there were no trailing digits */
	if (len == 0) {
		return -1;
	}

	/* point to the beginning of the last integer and convert */
	p = str + (ifname_len - len);
	val = strtoul(p, NULL, 10);

	/* if we are at the beginning of the string, or the previous
	* character is not a '.', then we have the unit number and
	* we are done parsing
	*/
	if (p == str || p[-1] != '.') {
		if (unit) {
			*unit = val;
		}
		return 0;
	} else {
		if (subunit) {
			*subunit = val;
		}
	}

	/* chop off the '.NNN' and get the unit number */
	p--;
	p[0] = '\0';

	/* find the trailing digit chars */
	len = sh_strrspn(str, digits);

	/* fail if there were no trailing digits */
	if (len == 0) {
		return -1;
	}

	/* point to the beginning of the last integer and convert */
	p = p - len;
	val = strtoul(p, NULL, 10);

	/* save the unit number */
	if (unit) {
		*unit = val;
	}

	return 0;
}

int
get_bsscfg_idx(void *dhd, int *bsscfg_idx)
{
	struct ifreq *ifr = dhd;
	int vif;
	int ret = BCME_ERROR;
	ret = get_ifname_unit(ifr->ifr_name, NULL, &vif);
	/* only return values possible from get_ifname_unit are
	 * -1 or the positive subunit number for vif
	 *  representation will be in this format : wl<unit>.<subunit>
	 */
	if (vif != -1) {
		*bsscfg_idx = vif;
	} else {
		*bsscfg_idx = 0;
	}

	return ret;
}

int
rwl_shell_createproc(void *wl)
{
	UNUSED_PARAMETER(wl);
	return fork();
}

void
rwl_shell_killproc(int pid)
{
	kill(pid, SIGKILL);
}

#ifdef RWL_SOCKET
/* validate hostname/ip given by the client */
int
validate_server_address()
{
	struct hostent *he;
	struct ipv4_addr temp;

	if (!dhd_atoip(g_rwl_servIP, &temp)) {
	/* Wrong IP address format check for hostname */
		if ((he = gethostbyname(g_rwl_servIP)) != NULL) {
			if (!dhd_atoip(*he->h_addr_list, &temp)) {
				g_rwl_servIP = inet_ntoa(*(struct in_addr *)*he->h_addr_list);
				if (g_rwl_servIP == NULL) {
					DPRINT_ERR(ERR, "Error at inet_ntoa \n");
					return FAIL;
				}
			} else {
				DPRINT_ERR(ERR, "Error in IP address \n");
				return FAIL;
			}
		} else {
			DPRINT_ERR(ERR, "Enter correct IP address/hostname format\n");
			return FAIL;
		}
	}
	return SUCCESS;
}
#endif /* RWL_SOCKET */

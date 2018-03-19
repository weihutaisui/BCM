 /*
 * Broadcom Home Gateway Reference Design
 * Web Page Configuration Support Routines
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
 * $Id$
 */

#ifdef WEBS
#include <webs.h>
#include <uemf.h>
#include <ej.h>
#else /* !WEBS */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <assert.h>
#include <httpd.h>
#endif /* WEBS */

#include <typedefs.h>
#include <proto/ethernet.h>
#include <bcmparams.h>
#include <bcmconfig.h>
#include <bcmnvram.h>
#include <bcmutils.h>
#include <bcmdevs.h>
#include <shutils.h>
#include <wlif_utils.h>
#include <netconf.h>
#include <nvparse.h>
#include <wlutils.h>
#include <bcmcvar.h>
#include <ezc.h>
#include <opencrypto.h>
#include <time.h>
#include <epivers.h>
#include "router_version.h"
#include <proto/802.11.h>
#include <proto/802.1d.h>
#include <security_ipc.h>
#ifdef __CONFIG_WPS__
#include <wps_ui.h>
#endif // endif

#include <cgi_common.h>

#if defined(linux)
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <sys/klog.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <net/if.h>

typedef u_int64_t u64;
typedef u_int32_t u32;
typedef u_int16_t u16;
typedef u_int8_t u8;
#include <linux/types.h>
#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <net/if_arp.h>
#include <sys/utsname.h>

struct lease_t {
	unsigned char chaddr[16];
	u_int32_t yiaddr;
	u_int32_t expires;
	char hostname[64];
};

/* Dump leases in <tr><td>hostname</td><td>MAC</td><td>IP</td><td>expires</td></tr> format */
static int
ej_lan_leases(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp = NULL;
	struct lease_t lease;
	int i;
	int index,num_interfaces=0;
	char buf[128];
	struct in_addr addr;
	unsigned long expires = 0;
	char sigusr1[] = "-XXXX";
	int ret = 0;

	/* Write out leases file */
	snprintf(sigusr1, sizeof(sigusr1), "-%d", SIGUSR1);
	eval("killall", sigusr1, "udhcpd");

	/* Count the number of lan and guest interfaces */

	if (nvram_get("lan_ifname"))
		num_interfaces++;

	if (nvram_get("lan1_ifname"))
		num_interfaces++;

	for (index =0; index < num_interfaces; index++){
		snprintf(buf,sizeof(buf),"/tmp/udhcpd%d.leases",index);

		if (!(fp = fopen(buf, "r")))
			continue;

		while (fread(&lease, sizeof(lease), 1, fp)) {
			/* Do not display reserved leases */
			if (ETHER_ISNULLADDR(lease.chaddr))
				continue;
			lease.hostname[sizeof(lease.hostname) - 1] = '\0';
			ret += websWrite(wp, "<tr><td>%s</td><td>", lease.hostname);
			for (i = 0; i < 6; i++) {
				ret += websWrite(wp, "%02X", lease.chaddr[i]);
				if (i != 5) ret += websWrite(wp, ":");
			}
			addr.s_addr = lease.yiaddr;
			ret += websWrite(wp, "</td><td>%s</td><td>", inet_ntoa(addr));
			expires = ntohl(lease.expires);
			if (!expires)
				ret += websWrite(wp, "Expired");
			else
				ret += websWrite(wp, "%s", reltime(expires));
			if(index)
				ret += websWrite(wp, "</td><td>Guest</td><td>");
			else
				ret += websWrite(wp, "</td><td>Internal</td><td>");
			ret += websWrite(wp, "</td></tr>");
		}

		fclose(fp);
	}

	return ret;
}
REG_EJ_HANDLER(lan_leases);
#endif /* defined(linux) */

/*
 * Example:
 * lan_route=192.168.2.0:255.255.255.0:192.168.2.1:1
 * <% lan_route("ipaddr", 0); %> produces "192.168.2.0"
 */
static int
ej_lan_route(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg;
	int which;
	char word[256], *next;
	char *ipaddr, *netmask, *gateway, *metric;

	if (ejArgs(argc, argv, "%s %d", &arg, &which) < 2) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	foreach(word, nvram_safe_get("lan_route"), next) {
		if (which-- == 0) {
			netmask = word;
			ipaddr = strsep(&netmask, ":");
			if (!ipaddr || !netmask)
				continue;
			gateway = netmask;
			netmask = strsep(&gateway, ":");
			if (!netmask || !gateway)
				continue;
			metric = gateway;
			gateway = strsep(&metric, ":");
			if (!gateway || !metric)
				continue;
			if (!strcmp(arg, "ipaddr"))
				return websWrite(wp, ipaddr);
			else if (!strcmp(arg, "netmask"))
				return websWrite(wp, netmask);
			else if (!strcmp(arg, "gateway"))
				return websWrite(wp, gateway);
			else if (!strcmp(arg, "metric"))
				return websWrite(wp, metric);
		}
	}

	return 0;
}
REG_EJ_HANDLER(lan_route);

static int
ej_emf_enable_display(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(BCA_HNDROUTER) && defined(MCPD_PROXY)
	websWrite(wp, "<p>\n");
	return 1;
#endif /* BCA_HNDROUTER && MCPD_PROXY */

#ifdef __CONFIG_EMF__
	websWrite(wp, "<p>\n");
	websWrite(wp, "<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n");
	websWrite(wp, "<tr>\n");
	websWrite(wp, "<th width=\"310\"\n");
	websWrite(wp, "onMouseOver=\"return overlib(\'Enables/Disables Efficient Multicast Forwarding feature\', LEFT);\"\n");
	websWrite(wp, "onMouseOut=\"return nd();\">\n");
	websWrite(wp, "EMF:&nbsp;&nbsp;\n");
	websWrite(wp, "</th>\n");
	websWrite(wp, "<td>&nbsp;&nbsp;</td>\n");
	websWrite(wp, "<td>\n");
	websWrite(wp, "<select name=\"emf_enable\">\n");
	websWrite(wp, "<option value=\"1\" %s>Enabled</option>", nvram_match("emf_enable", "1") ? "selected": "\n");
	websWrite(wp, "<option value=\"0\" %s>Disabled</option>", nvram_match("emf_enable", "0") ? "selected": "\n");
	websWrite(wp, "</select>\n");
	websWrite(wp, "</td>\n");
	websWrite(wp, "</tr>\n");
	websWrite(wp, "</table>\n");
#endif /* __CONFIG_EMF__ */

	return 1;
}
REG_EJ_HANDLER(emf_enable_display);

static int
ej_emf_entries_display(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __CONFIG_EMF__
	char value[32];

	websWrite(wp, "<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n");
	websWrite(wp, "<tr>\n");
	websWrite(wp, "<th width=\"310\" valign=\"top\" rowspan=\"6\"\n");
	websWrite(wp, "onMouseOver=\"return overlib(\'Add/Delete static forwarding entries for the multicast groups.\', LEFT);\"\n");
	websWrite(wp, "onMouseOut=\"return nd();\">\n");
	websWrite(wp, "<input type=\"hidden\" name=\"emf_entry\" value=\"5\">\n");
	websWrite(wp, "Static Multicast Forwarding Entries:&nbsp;&nbsp;\n");
	websWrite(wp, "</th>\n");
	websWrite(wp, "<td>&nbsp;&nbsp;</td>\n");
	websWrite(wp, "<td class=\"label\">Multicast IP Address</td>\n");
	websWrite(wp, "<td></td>\n");
	websWrite(wp, "<td class=\"label\">Interface</td>\n");
	websWrite(wp, "</tr>\n");
	websWrite(wp, "<tr>\n");
	websWrite(wp, "<td>&nbsp;&nbsp;</td>\n");
	get_emf_entry("mgrp", 0, value, sizeof(value));
	websWrite(wp, "<td><input name=\"emf_entry_mgrp0\" value=\"%s\" size=\"15\" maxlength=\"15\"></td>\n", value);
	websWrite(wp, "<td>&nbsp;</td>\n");
	get_emf_entry("if", 0, value, sizeof(value));
	websWrite(wp, "<td><input name=\"emf_entry_if0\" value=\"%s\" size=\"15\" maxlength=\"15\"></td>\n", value);
	websWrite(wp, "</tr>\n");
	websWrite(wp, "<tr>\n");
	websWrite(wp, "<td>&nbsp;&nbsp;</td>\n");
	get_emf_entry("mgrp", 1, value, sizeof(value));
	websWrite(wp, "<td><input name=\"emf_entry_mgrp1\" value=\"%s\" size=\"15\" maxlength=\"15\"></td>\n", value);
	websWrite(wp, "<td>&nbsp;</td>\n");
	get_emf_entry("if", 1, value, sizeof(value));
	websWrite(wp, "<td><input name=\"emf_entry_if1\" value=\"%s\" size=\"15\" maxlength=\"15\"></td>\n", value);
	websWrite(wp, "</tr>\n");
	websWrite(wp, "<tr>\n");
	websWrite(wp, "<td>&nbsp;&nbsp;</td>\n");
	get_emf_entry("mgrp", 2, value, sizeof(value));
	websWrite(wp, "<td><input name=\"emf_entry_mgrp2\" value=\"%s\" size=\"15\" maxlength=\"15\"></td>\n", value);
	websWrite(wp, "<td>&nbsp;</td>\n");
	get_emf_entry("if", 2, value, sizeof(value));
	websWrite(wp, "<td><input name=\"emf_entry_if2\" value=\"%s\" size=\"15\" maxlength=\"15\"></td>\n", value);
	websWrite(wp, "</tr>\n");
	websWrite(wp, "<tr>\n");
	websWrite(wp, "<td>&nbsp;&nbsp;</td>\n");
	get_emf_entry("mgrp", 3, value, sizeof(value));
	websWrite(wp, "<td><input name=\"emf_entry_mgrp3\" value=\"%s\" size=\"15\" maxlength=\"15\"></td>\n", value);
	websWrite(wp, "<td>&nbsp;</td>\n");
	get_emf_entry("if", 3, value, sizeof(value));
	websWrite(wp, "<td><input name=\"emf_entry_if3\" value=\"%s\" size=\"15\" maxlength=\"15\"></td>\n", value);
	websWrite(wp, "</tr>\n");
	websWrite(wp, "<tr>\n");
	websWrite(wp, "<td>&nbsp;&nbsp;</td>\n");
	get_emf_entry("mgrp", 4, value, sizeof(value));
	websWrite(wp, "<td><input name=\"emf_entry_mgrp4\" value=\"%s\" size=\"15\" maxlength=\"15\"></td>\n", value);
	websWrite(wp, "<td>&nbsp;</td>\n");
	get_emf_entry("if", 4, value, sizeof(value));
	websWrite(wp, "<td><input name=\"emf_entry_if4\" value=\"%s\" size=\"15\" maxlength=\"15\"></td>\n", value);
	websWrite(wp, "</tr>\n");
	websWrite(wp, "</table>\n");
#endif /* __CONFIG_EMF__ */

	return 1;
}
REG_EJ_HANDLER(emf_entries_display);

static int
ej_emf_uffp_entries_display(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __CONFIG_EMF__
	char value[32];

	websWrite(wp, "<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n");
	websWrite(wp, "<tr>\n");
	websWrite(wp, "<th width=\"310\" valign=\"top\" rowspan=\"6\"\n");
	websWrite(wp, "onMouseOver=\"return overlib(\'Add/Delete unregistered multicast data frames forwarding port entries. Multicast data frames that fail MFDB lookup will be flooded on to these ports.\', LEFT);\"\n");
	websWrite(wp, "onMouseOut=\"return nd();\">\n");
	websWrite(wp, "<input type=\"hidden\" name=\"emf_uffp_entry\" value=\"5\">\n");
	websWrite(wp, "Unregistered Multicast Frames Forwarding Ports:&nbsp;&nbsp;\n");
	websWrite(wp, "</th>\n");
	websWrite(wp, "<td>&nbsp;&nbsp;</td>\n");
	websWrite(wp, "<td class=\"label\">Interface</td>\n");
	websWrite(wp, "<td></td>\n");
	websWrite(wp, "</tr>\n");
	websWrite(wp, "<tr>\n");
	websWrite(wp, "<td>&nbsp;&nbsp;</td>\n");
	get_emf_uffp_entry("if", 0, value, sizeof(value));
	websWrite(wp, "<td><input name=\"emf_uffp_entry_if0\" value=\"%s\" size=\"15\" maxlength=\"15\"></td>\n", value);
	websWrite(wp, "<td>&nbsp;&nbsp;</td>\n");
	websWrite(wp, "<td></td>\n");
	websWrite(wp, "</tr>\n");
	websWrite(wp, "<tr>\n");
	websWrite(wp, "<td>&nbsp;&nbsp;</td>\n");
	get_emf_uffp_entry("if", 1, value, sizeof(value));
	websWrite(wp, "<td><input name=\"emf_uffp_entry_if1\" value=\"%s\" size=\"15\" maxlength=\"15\"></td>\n", value);
	websWrite(wp, "<td>&nbsp;&nbsp;</td>\n");
	websWrite(wp, "<td></td>\n");
	websWrite(wp, "</tr>\n");
	websWrite(wp, "<tr>\n");
	websWrite(wp, "<td>&nbsp;&nbsp;</td>\n");
	get_emf_uffp_entry("if", 2, value, sizeof(value));
	websWrite(wp, "<td><input name=\"emf_uffp_entry_if2\" value=\"%s\" size=\"15\" maxlength=\"15\"></td>\n", value);
	websWrite(wp, "<td>&nbsp;&nbsp;</td>\n");
	websWrite(wp, "<td></td>\n");
	websWrite(wp, "</tr>\n");
	websWrite(wp, "<tr>\n");
	websWrite(wp, "<td>&nbsp;&nbsp;</td>\n");
	get_emf_uffp_entry("if", 3, value, sizeof(value));
	websWrite(wp, "<td><input name=\"emf_uffp_entry_if3\" value=\"%s\" size=\"15\" maxlength=\"15\"></td>\n", value);
	websWrite(wp, "<td>&nbsp;&nbsp;</td>\n");
	websWrite(wp, "<td></td>\n");
	websWrite(wp, "</tr>\n");
	websWrite(wp, "<tr>\n");
	websWrite(wp, "<td>&nbsp;&nbsp;</td>\n");
	get_emf_uffp_entry("if", 4, value, sizeof(value));
	websWrite(wp, "<td><input name=\"emf_uffp_entry_if4\" value=\"%s\" size=\"15\" maxlength=\"15\"></td>\n", value);
	websWrite(wp, "<td>&nbsp;&nbsp;</td>\n");
	websWrite(wp, "<td></td>\n");
	websWrite(wp, "</tr>\n");
	websWrite(wp, "</table>\n");
#endif /* __CONFIG_EMF__ */

	return 1;
}
REG_EJ_HANDLER(emf_uffp_entries_display);

static int
ej_emf_rtport_entries_display(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __CONFIG_EMF__
	char value[32];

	websWrite(wp, "<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n");
	websWrite(wp, "<tr>\n");
	websWrite(wp, "<th width=\"310\" valign=\"top\" rowspan=\"6\"\n");
	websWrite(wp, "onMouseOver=\"return overlib(\'These are the LAN interfaces on which multicast routers are present. IGMP Report frames are forwared to these ports.\', LEFT);\"\n");
	websWrite(wp, "onMouseOut=\"return nd();\">\n");
	websWrite(wp, "<input type=\"hidden\" name=\"emf_rtport_entry\" value=\"5\">\n");
	websWrite(wp, "Multicast Router / IGMP Forwarding Ports:&nbsp;&nbsp;\n");
	websWrite(wp, "</th>\n");
	websWrite(wp, "<td>&nbsp;&nbsp;</td>\n");
	websWrite(wp, "<td class=\"label\">Interface</td>\n");
	websWrite(wp, "<td></td>\n");
	websWrite(wp, "</tr>\n");
	websWrite(wp, "<tr>\n");
	websWrite(wp, "<td>&nbsp;&nbsp;</td>\n");
	get_emf_rtport_entry("if", 0, value, sizeof(value));
	websWrite(wp, "<td><input name=\"emf_rtport_entry_if0\" value=\"%s\" size=\"15\" maxlength=\"15\"></td>\n", value);
	websWrite(wp, "<td>&nbsp;&nbsp;</td>\n");
	websWrite(wp, "<td></td>\n");
	websWrite(wp, "</tr>\n");
	websWrite(wp, "<tr>\n");
	websWrite(wp, "<td>&nbsp;&nbsp;</td>\n");
	get_emf_rtport_entry("if", 1, value, sizeof(value));
	websWrite(wp, "<td><input name=\"emf_rtport_entry_if1\" value=\"%s\" size=\"15\" maxlength=\"15\"></td>\n", value);
	websWrite(wp, "<td>&nbsp;&nbsp;</td>\n");
	websWrite(wp, "<td></td>\n");
	websWrite(wp, "</tr>\n");
	websWrite(wp, "<tr>\n");
	websWrite(wp, "<td>&nbsp;&nbsp;</td>\n");
	get_emf_rtport_entry("if", 2, value, sizeof(value));
	websWrite(wp, "<td><input name=\"emf_rtport_entry_if2\" value=\"%s\" size=\"15\" maxlength=\"15\"></td>\n", value);
	websWrite(wp, "<td>&nbsp;&nbsp;</td>\n");
	websWrite(wp, "<td></td>\n");
	websWrite(wp, "</tr>\n");
	websWrite(wp, "<tr>\n");
	websWrite(wp, "<td>&nbsp;&nbsp;</td>\n");
	get_emf_rtport_entry("if", 3, value, sizeof(value));
	websWrite(wp, "<td><input name=\"emf_rtport_entry_if3\" value=\"%s\" size=\"15\" maxlength=\"15\"></td>\n", value);
	websWrite(wp, "<td>&nbsp;&nbsp;</td>\n");
	websWrite(wp, "<td></td>\n");
	websWrite(wp, "</tr>\n");
	websWrite(wp, "<tr>\n");
	websWrite(wp, "<td>&nbsp;&nbsp;</td>\n");
	get_emf_rtport_entry("if", 4, value, sizeof(value));
	websWrite(wp, "<td><input name=\"emf_rtport_entry_if4\" value=\"%s\" size=\"15\" maxlength=\"15\"></td>\n", value);
	websWrite(wp, "<td>&nbsp;&nbsp;</td>\n");
	websWrite(wp, "<td></td>\n");
	websWrite(wp, "</tr>\n");
	websWrite(wp, "</table>\n");
#endif /* __CONFIG_EMF__ */

	return 1;
}
REG_EJ_HANDLER(emf_rtport_entries_display);

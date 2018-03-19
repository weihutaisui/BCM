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

static int
ej_wet_tunnel_display(int eid, webs_t wp, int argc, char_t **argv)
{
	char tmp[NVRAM_BUFSIZE], prefix[] = "wlXXXXXXXXXX_";
	char cap[WLC_IOCTL_SMLEN];
	char caps[WLC_IOCTL_MEDLEN];
	char *name = NULL;
	char *next = NULL;
	int wet_tunnel_cap = 0;

	if (!make_wl_prefix(prefix, sizeof(prefix), 0, NULL)) {
		websError(wp, 400, "unit number variable doesn't exist\n");
		return -1;
	}

	name = nvram_safe_get(strcat_r(prefix, "ifname", tmp));
	if (wl_iovar_get(name, "cap", (void *)caps, sizeof(caps)))
		return -1;

	foreach(cap, caps, next) {
		if (!strcmp(cap, "wet_tunnel")) {
			wet_tunnel_cap = 1;
			break;
		}
	}
	if (wet_tunnel_cap == 0)
		return -1;

	websWrite(wp, "<p>\n");
	websWrite(wp, "<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n");
	websWrite(wp, "<tr>\n");
	websWrite(wp, "<th width=\"310\"\n");
	websWrite(wp, "onMouseOver=\"return overlib(\'Enable/Disable WET tunnel\', LEFT);\"\n");
	websWrite(wp, "onMouseOut=\"return nd();\">\n");
	websWrite(wp, "WET Tunnel:&nbsp;&nbsp;\n");
	websWrite(wp, "</th>\n");
	websWrite(wp, "<td>&nbsp;&nbsp;</td>\n");
	websWrite(wp, "<td>\n");
	websWrite(wp, "<select name=\"wl_wet_tunnel\">\n");
	websWrite(wp, "<option value=\"1\" %s>Enabled</option>", nvram_match("wl_wet_tunnel", "1") ? "selected": "\n");
	websWrite(wp, "<option value=\"0\" %s>Disabled</option>", nvram_match("wl_wet_tunnel", "0") ? "selected": "\n");
	websWrite(wp, "</select>\n");
	websWrite(wp, "</td>\n");
	websWrite(wp, "</tr>\n");
	websWrite(wp, "</table>\n");

	return 1;
}
REG_EJ_HANDLER(wet_tunnel_display);

#ifdef TRAFFIC_MGMT_RSSI_POLICY
static int
ej_trf_mgmt_rssi_policy_display(int eid, webs_t wp, int argc, char_t **argv)
{
	char tmp[NVRAM_BUFSIZE], prefix[] = "wlXXXXXXXXXX_";
	char cap[WLC_IOCTL_SMLEN];
	char caps[WLC_IOCTL_MEDLEN];
	char *name = NULL;
	char *next = NULL;
	int trf_mgmt_cap = 0;

	if (!make_wl_prefix(prefix, sizeof(prefix), 0, NULL)) {
		websError(wp, 400, "unit number variable doesn't exist\n");
		return -1;
	}

	name = nvram_safe_get(strcat_r(prefix, "ifname", tmp));
	if (wl_iovar_get(name, "cap", (void *)caps, sizeof(caps)))
		return -1;

	foreach(cap, caps, next) {
		if (!strcmp(cap, "traffic-mgmt")) {
			trf_mgmt_cap = 1;
			break;
		}
	}
	if (trf_mgmt_cap == 0)
		return -1;

	websWrite(wp, "<p>\n");
	websWrite(wp, "<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n");
	websWrite(wp, "<tr>\n");
	websWrite(wp, "<th width=\"310\"\n");
	websWrite(wp, "onMouseOver=\"return overlib(\'Set to map lowest RSSI STA to low priority(BE) or disable.\', LEFT);\"\n");
	websWrite(wp, "onMouseOut=\"return nd();\">\n");
	websWrite(wp, "Traffic Management RSSI Policy:&nbsp;&nbsp;\n");
	websWrite(wp, "</th>\n");
	websWrite(wp, "<td>&nbsp;&nbsp;</td>\n");
	websWrite(wp, "<td>\n");
	websWrite(wp, "<select name=\"wl_trf_mgmt_rssi_policy\">\n");
	websWrite(wp, "<option value=\"0\" %s>None</option>", nvram_match("wl_trf_mgmt_rssi_policy", "0") ? "selected": "\n");
	websWrite(wp, "<option value=\"1\" %s>Lowest RSSI</option>", nvram_match("wl_trf_mgmt_rssi_policy", "1") ? "selected": "\n");
	websWrite(wp, "</select>\n");
	websWrite(wp, "</td>\n");
	websWrite(wp, "</tr>\n");
	websWrite(wp, "</table>\n");

	return 1;
}
REG_EJ_HANDLER(trf_mgmt_rssi_policy_display);
#endif /* TRAFFIC_MGMT_RSSI_POLICY */

/* Fill HTML input form with DSCP to WMM AC Mapping filters data stored in NVRAM */
static int
ej_trf_mgmt_dwm_display(int eid, webs_t wp, int argc, char_t **argv)
{
	char tmp[NVRAM_BUFSIZE], prefix[] = "wlXXXXXXXXXX_";
	char cap[WLC_IOCTL_SMLEN], caps[WLC_IOCTL_MEDLEN];
	char *name = NULL, *next = NULL, *mode = NULL;
	int trf_mgmt_cap = 0, trf_mgmt_dwm_cap = 0, ret = 0, i, ap = 0;
	int n = MAX_NUM_TRF_MGMT_DWM_RULES;
	bool valid;
	netconf_trmgmt_t trm_dwm;

	if (!make_wl_prefix(prefix, sizeof(prefix), 0, NULL)) {
		websError(wp, 400, "unit number variable doesn't exist\n");
		return -1;
	}

	name = nvram_safe_get(strcat_r(prefix, "ifname", tmp));
	if (wl_iovar_get(name, "cap", (void *)caps, sizeof(caps)))
		return -1;

	foreach(cap, caps, next) {
		if (!strcmp(cap, "traffic-mgmt")) {
			trf_mgmt_cap = 1;
		}
		if (!strcmp(cap, "traffic-mgmt-dwm")) {
			trf_mgmt_dwm_cap = 1;
		}
	}

	mode = nvram_safe_get(strcat_r(prefix, "mode", tmp));
	ap = (!strcmp(mode, "") || !strcmp(mode, "ap"));

	if (trf_mgmt_cap == 0 || trf_mgmt_dwm_cap == 0 || ap == 0)
		return -1;

	websWrite(wp, "<p>");
	websWrite(wp, "<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\">");
	websWrite(wp, "<tr>");
	websWrite(wp, "<th width=\"310\" valign=\"top\" rowspan=\"23\"");
	websWrite(wp, "onMouseOver=\"return overlib('Adds up to 21 DSCP to WMM AC mapping filters which specifies DSCP and its matching WMM AC.', LEFT);\"");
	websWrite(wp, "onMouseOut=\"return nd();\">");
	websWrite(wp, "<input type=\"hidden\" name=\"trf_mgmt_dwm\" value=\"22\">");
	websWrite(wp, "Traffic Management DSCP to WMM AC Mapping:&nbsp;&nbsp;");
	websWrite(wp, "</th>");
	websWrite(wp, "<td>&nbsp;&nbsp;</td>");
	websWrite(wp, "<td class=\"label\">DSCP</td>");
	websWrite(wp, "<td></td>");
	websWrite(wp, "<td class=\"label\">Priority</td>");
	websWrite(wp, "<td></td>");
	websWrite(wp, "<td class=\"label\">Favored</td>");
	websWrite(wp, "<td></td>");
	websWrite(wp, "<td class=\"label\">Enabled</td>");
	websWrite(wp, "</tr>");

	for (i = 0; i < n; i++) {

		valid = get_trf_mgmt_dwm(prefix, i, &trm_dwm);

		ret += websWrite(wp, "<tr>");
		ret += websWrite(wp, "<td></td>");

		/* Print DSCP */
		ret += websWrite(wp, "<td>");
		ret += websWrite(wp, "<select name=\"%strf_mgmt_dwm_dscp%d\">", prefix, i);
		ret += websWrite(wp, "<option value=\"0\" %s>None   (0x0)</option>",
				 valid && trm_dwm.match.dscp == 0 ? "selected" : "");
		ret += websWrite(wp, "<option value=\"8\" %s>CS1    (0x08)</option>",
				 valid && trm_dwm.match.dscp == 8 ? "selected" : "");
		ret += websWrite(wp, "<option value=\"10\" %s>AF11  (0x0A)</option>",
				 valid && trm_dwm.match.dscp == 10 ? "selected" : "");
		ret += websWrite(wp, "<option value=\"12\" %s>AF12  (0x0C)</option>",
				 valid && trm_dwm.match.dscp == 12 ? "selected" : "");
		ret += websWrite(wp, "<option value=\"14\" %s>AF13  (0x0E)</option>",
				 valid && trm_dwm.match.dscp == 14 ? "selected" : "");
		ret += websWrite(wp, "<option value=\"16\" %s>CS2   (0x10)</option>",
				 valid && trm_dwm.match.dscp == 16 ? "selected" : "");
		ret += websWrite(wp, "<option value=\"18\" %s>AF21  (0x12)</option>",
				 valid && trm_dwm.match.dscp == 18 ? "selected" : "");
		ret += websWrite(wp, "<option value=\"20\" %s>AF22  (0x14)</option>",
				 valid && trm_dwm.match.dscp == 20 ? "selected" : "");
		ret += websWrite(wp, "<option value=\"22\" %s>AF23  (0x16)</option>",
				 valid && trm_dwm.match.dscp == 22 ? "selected" : "");
		ret += websWrite(wp, "<option value=\"24\" %s>CS3   (0x18)</option>",
				 valid && trm_dwm.match.dscp == 24 ? "selected" : "");
		ret += websWrite(wp, "<option value=\"26\" %s>AF31  (0x1A)</option>",
				 valid && trm_dwm.match.dscp == 26 ? "selected" : "");
		ret += websWrite(wp, "<option value=\"28\" %s>AF32  (0x1C)</option>",
				 valid && trm_dwm.match.dscp == 28 ? "selected" : "");
		ret += websWrite(wp, "<option value=\"30\" %s>AF33  (0x1E)</option>",
				 valid && trm_dwm.match.dscp == 30 ? "selected" : "");
		ret += websWrite(wp, "<option value=\"32\" %s>CS4   (0x20)</option>",
				 valid && trm_dwm.match.dscp == 32 ? "selected" : "");
		ret += websWrite(wp, "<option value=\"34\" %s>AF41  (0x22)</option>",
				 valid && trm_dwm.match.dscp == 34 ? "selected" : "");
		ret += websWrite(wp, "<option value=\"36\" %s>AF42  (0x24)</option>",
				 valid && trm_dwm.match.dscp == 36 ? "selected" : "");
		ret += websWrite(wp, "<option value=\"38\" %s>AF43  (0x26)</option>",
				 valid && trm_dwm.match.dscp == 38 ? "selected" : "");
		ret += websWrite(wp, "<option value=\"40\" %s>CS5   (0x28)</option>",
				 valid && trm_dwm.match.dscp == 40 ? "selected" : "");
		ret += websWrite(wp, "<option value=\"46\" %s>EF    (0x2E)</option>",
				 valid && trm_dwm.match.dscp == 46 ? "selected" : "");
		ret += websWrite(wp, "<option value=\"48\" %s>CS6   (0x30)</option>",
				 valid && trm_dwm.match.dscp == 48 ? "selected" : "");
		ret += websWrite(wp, "<option value=\"56\" %s>CS7   (0x38)</option>",
				 valid && trm_dwm.match.dscp == 56 ? "selected" : "");

		ret += websWrite(wp, "</select>");
		ret += websWrite(wp, "</td>");
		ret += websWrite(wp, "<td></td>");

		/* Print priority */
		ret += websWrite(wp, "<td>");
		ret += websWrite(wp, "<select name=\"%strf_mgmt_dwm_prio%d\">", prefix, i);
		ret += websWrite(wp, "<option value=\"1\" %s>BK</option>",
				 valid && trm_dwm.prio == 1 ? "selected" : "");
		ret += websWrite(wp, "<option value=\"0\" %s>BE</option>",
				 valid && trm_dwm.prio == 0 ? "selected" : "");
		ret += websWrite(wp, "<option value=\"5\" %s>VI</option>",
				 valid && trm_dwm.prio == 5 ? "selected" : "");
		ret += websWrite(wp, "<option value=\"6\" %s>VO</option>",
				 valid && trm_dwm.prio == 6 ? "selected" : "");

		ret += websWrite(wp, "</select>");
		ret += websWrite(wp, "</td>");
		ret += websWrite(wp, "<td></td>");

		/* Print Favored */
		ret += websWrite(wp, "<td><input type=\"checkbox\" name=\"%strf_mgmt_dwm_favored%d\" %s></td>",
				 prefix, i, valid && (trm_dwm.favored) ? "checked" : "");

		/* Print enable */
		ret += websWrite(wp, "<td><input type=\"checkbox\" name=\"%strf_mgmt_dwm_enable%d\" %s></td>",
				 prefix, i, valid && !(trm_dwm.match.flags & NETCONF_DISABLED) ? "checked" : "");

		ret += websWrite(wp, "</tr>\n");

	} /* for all filters */

	websWrite(wp, "</table>");

	return ret;
}
REG_EJ_HANDLER(trf_mgmt_dwm_display);

/* Display DFS Reentry parameters */
static int
ej_dfs_reentry_display(int eid, webs_t wp, int argc, char_t **argv)
{
#define NWINS 3
	static struct {
		const char *keyword;
		const char *description;
	} wins[NWINS] = {
		{ "acs_dfsr_immediate", "Immediate Reentry" },
		{ "acs_dfsr_deferred", "Deferred Reentry" },
		{ "acs_dfsr_activity", "Channel Active" }
	};
	static char *table_start =
	"<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n"
	"<tr>"
		"<th width=\"310\""
			" onMouseOver=\"return overlib('DFS Reentry Window parameters', LEFT);\""
			" onMouseOut=\"return nd();\">"
			"DFS Reentry Window Settings&nbsp;&nbsp;"
		"</th>"
		"<td>&nbsp;&nbsp;</td>"
		"<td class=\"label\">Seconds</td>"
		"<td class=\"label\">Threshold</td>"
	"</tr>\n";
	static char *table_entry =
	"<tr>"
		"<th width=\"310\""
			" onMouseOver=\"return overlib('DFS %s window parameters', LEFT);\""
			" onMouseOut=\"return nd();\">"
			"%s:&nbsp;&nbsp;"
		"</th>"
		"<input type=\"hidden\" name=\"wl_%s\" value=\"1\">" /* needed to call validateFn */
		"<td>&nbsp;&nbsp;</td>"
		"<td>"
			"<input name=\"wl_%s_sec\" "
			" onMouseOver=\"return overlib('Window size in seconds', LEFT);\""
			" onMouseOut=\"return nd();\""
			" value=\"%u\" size=\"8\" maxlength=\"8\">"
		"</td>"
		"<td>"
			"<input name=\"wl_%s_thr\" "
			" onMouseOver=\"return overlib('Window threshold value', LEFT);\""
			" onMouseOut=\"return nd();\""
			" value=\"%u\" size=\"8\" maxlength=\"8\">"
		"</td>"
	"</tr>\n";
	static char *table_end = "</table>\n";
	char tmp[NVRAM_BUFSIZE], prefix[16]; /* "wlXXXXXXXXXX_" */
	unsigned sec, thr;
	int i;

	if (!make_wl_prefix(prefix, sizeof(prefix), 0, NULL)) {
		strncpy_n(prefix, "wl_", sizeof(prefix));
	}

	websWrite(wp, table_start);

	for (i = 0; i < NWINS; ++i) {
		if (sscanf(nvram_safe_get(strcat_r(prefix, wins[i].keyword, tmp)), "%u %u",
			&sec, &thr) != 2) {
			sec = thr = 0;
		}
		websWrite(wp, table_entry, wins[i].description, wins[i].description,
			wins[i].keyword,
			wins[i].keyword, sec,
			wins[i].keyword, thr );
	}
	websWrite(wp, table_end);
	return 0;
}
REG_EJ_HANDLER(dfs_reentry_display);

/* Fill HTML input form with traffic management filters data stored in NVRAM */
static int
ej_trf_mgmt_display(int eid, webs_t wp, int argc, char_t **argv)
{
	char tmp[NVRAM_BUFSIZE], prefix[] = "wlXXXXXXXXXX_";
	char cap[WLC_IOCTL_SMLEN];
	char caps[WLC_IOCTL_MEDLEN];
	char *name = NULL;
	char *next = NULL;
	int trf_mgmt_cap = 0;
	int i, n = 9, ret = 0;
	netconf_trmgmt_t trm;
	bool valid;
	char port[] = "XXXXX";
	char eastr[ETHER_ADDR_STR_LEN];
	unsigned char *hwaddr;

	if (!make_wl_prefix(prefix, sizeof(prefix), 0, NULL)) {
		websError(wp, 400, "unit number variable doesn't exist\n");
		return -1;
	}

	name = nvram_safe_get(strcat_r(prefix, "ifname", tmp));
#ifdef __CONFIG_DHDAP__
	/* Traffic Management Setting is not supported on DHD/PCIEFD driver */
	if (!dhd_probe(name)) {
		return -1;
	}
#endif // endif
	if (wl_iovar_get(name, "cap", (void *)caps, sizeof(caps)))
		return -1;

	foreach(cap, caps, next) {
		if (!strcmp(cap, "traffic-mgmt")) {
			trf_mgmt_cap = 1;
			break;
		}
	}
	if (trf_mgmt_cap == 0)
		return -1;

	websWrite(wp, "<p>");
	websWrite(wp, "<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\">");
	websWrite(wp, "<tr>");
	websWrite(wp, "<th width=\"310\" valign=\"top\" rowspan=\"11\"");
	websWrite(wp, "onMouseOver=\"return overlib('Adds up to 10 traffic management filters which specifies the TCP/UDP port, MAC and priority.', LEFT);\"");
	websWrite(wp, "onMouseOut=\"return nd();\">");
	websWrite(wp, "<input type=\"hidden\" name=\"trf_mgmt_port\" value=\"10\">");
	websWrite(wp, "Traffic Management Settings:&nbsp;&nbsp;");
	websWrite(wp, "</th>");
	websWrite(wp, "<td>&nbsp;&nbsp;</td>");
	websWrite(wp, "<td class=\"label\">Protocol</td>");
	websWrite(wp, "<td></td>");
	websWrite(wp, "<td class=\"label\">Src Port</td>");
	websWrite(wp, "<td></td>");
	websWrite(wp, "<td class=\"label\">Dst Port</td>");
	websWrite(wp, "<td></td>");
	websWrite(wp, "<td class=\"label\">Dst Mac Addr</td>");
	websWrite(wp, "<td></td>");
	websWrite(wp, "<td class=\"label\">Priority</td>");
	websWrite(wp, "<td></td>");
	websWrite(wp, "<td class=\"label\">Favored</td>");
	websWrite(wp, "<td></td>");
	websWrite(wp, "<td class=\"label\">Enabled</td>");
	websWrite(wp, "</tr>");

	for (i = 0; i <= n; i++) {
		valid = get_trf_mgmt_port(prefix, i, &trm);

		ret += websWrite(wp, "<tr>");
		ret += websWrite(wp, "<td></td>");

		/* Print protocol */
		ret += websWrite(wp, "<td>");
		ret += websWrite(wp, "<select name=\"%strf_mgmt_port_proto%d\">", prefix, i);
		ret += websWrite(wp, "<option value=\"tcp\" %s>TCP</option>",
				 valid && trm.match.ipproto == IPPROTO_TCP ? "selected" : "");
		ret += websWrite(wp, "<option value=\"udp\" %s>UDP</option>",
				 valid && trm.match.ipproto == IPPROTO_UDP ? "selected" : "");
		ret += websWrite(wp, "<option value=\"mac\" %s>MAC</option>",
				 valid && trm.match.ipproto == IPPROTO_IP ? "selected" : "");
		ret += websWrite(wp, "</select>");
		ret += websWrite(wp, "</td>");
		ret += websWrite(wp, "<td></td>");

		/* Print source port number,  map source port value  zero to  NULL */
		if ((valid) && (trm.match.ipproto != IPPROTO_IP) && trm.match.src.ports[0])
			snprintf(port, sizeof(port), "%d", ntohs(trm.match.src.ports[0]));
		else
			*port = '\0';
		ret += websWrite(wp, "<td><input name=\"%strf_mgmt_port_sport%d\" value=\"%s\" size=\"5\" maxlength=\"5\"></td>",
				 prefix, i, port);
		ret += websWrite(wp, "<td></td>");

		/* Print destination port number,  map dest port value  zero to  NULL */
		if ((valid) && (trm.match.ipproto != IPPROTO_IP) && trm.match.dst.ports[0])
			snprintf(port, sizeof(port), "%d", ntohs(trm.match.dst.ports[0]));
		else
			*port = '\0';
		ret += websWrite(wp, "<td><input name=\"%strf_mgmt_port_dport%d\" value=\"%s\" size=\"5\" maxlength=\"5\"></td>",
				 prefix, i, port);
		ret += websWrite(wp, "<td></td>");

		/* Print mac address */
		hwaddr = (unsigned char *)&trm.match.mac;
		/* Check for bad, multicast, broadcast, or null address */
		if ((hwaddr[0] & 1) ||
			(hwaddr[0] & hwaddr[1] & hwaddr[2] & hwaddr[3] & hwaddr[4] & hwaddr[5]) == 0xff ||
			(hwaddr[0] | hwaddr[1] | hwaddr[2] | hwaddr[3] | hwaddr[4] | hwaddr[5]) == 0x00) {
			ret += websWrite(wp, "<td><input name=\"%strf_mgmt_port_macaddr%d\" value=\"%s\" size=\"17\" maxlength=\"17\"></td>",
					 prefix, i, "");
		} else {
			ret += websWrite(wp, "<td><input name=\"%strf_mgmt_port_macaddr%d\" value=\"%s\" size=\"17\" maxlength=\"17\"></td>",
					 prefix, i, valid ? ether_etoa((const unsigned char *)&trm.match.mac, eastr) : "");
		}
		ret += websWrite(wp, "<td></td>");

		/* Print priority */
		ret += websWrite(wp, "<td>");
		ret += websWrite(wp, "<select name=\"%strf_mgmt_port_prio%d\">", prefix, i);
		ret += websWrite(wp, "<option value=\"0\" %s>BK</option>",
				 valid && trm.prio == 0 ? "selected" : "");
		ret += websWrite(wp, "<option value=\"1\" %s>BE</option>",
				 valid && trm.prio == 1 ? "selected" : "");
		ret += websWrite(wp, "<option value=\"2\" %s>VI</option>",
				 valid && trm.prio == 2 ? "selected" : "");
		ret += websWrite(wp, "<option value=\"3\" %s>NOCHANGE</option>",
				 valid && trm.prio == 3 ? "selected" : "");

		ret += websWrite(wp, "</select>");
		ret += websWrite(wp, "</td>");
		ret += websWrite(wp, "<td></td>");

		/* Print Favored */
		ret += websWrite(wp, "<td><input type=\"checkbox\" name=\"%strf_mgmt_port_favored%d\" %s></td>",
				 prefix, i, valid && (trm.favored) ? "checked" : "");

		/* Print enable */
		ret += websWrite(wp, "<td><input type=\"checkbox\" name=\"%strf_mgmt_port_enable%d\" %s></td>",
				 prefix, i, valid && !(trm.match.flags & NETCONF_DISABLED) ? "checked" : "");

		ret += websWrite(wp, "</tr>\n");
	}

	websWrite(wp, "</table>");

	return ret;
}
REG_EJ_HANDLER(trf_mgmt_display);

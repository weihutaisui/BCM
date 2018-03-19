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

#ifdef __CONFIG_NAT__
/*
 * Example:
 * <% filter_client(1, 10); %> produces a table of the first 10 client filter entries
 */
static int
ej_filter_client(int eid, webs_t wp, int argc, char_t **argv)
{
	int i, n, j, ret = 0;
	netconf_filter_t start, end;
	bool valid;
	char port[] = "XXXXX";
	char *days[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
	char *hours[] = {
		"12:00 AM", "1:00 AM", "2:00 AM", "3:00 AM", "4:00 AM", "5:00 AM",
		"6:00 AM", "7:00 AM", "8:00 AM", "9:00 AM", "10:00 AM", "11:00 AM",
		"12:00 PM", "1:00 PM", "2:00 PM", "3:00 PM", "4:00 PM", "5:00 PM",
		"6:00 PM", "7:00 PM", "8:00 PM", "9:00 PM", "10:00 PM", "11:00 PM"
	};

	if (ejArgs(argc, argv, "%d %d", &i, &n) < 2) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	for (; i <= n; i++) {
		valid = get_filter_client(i, &start, &end);

		ret += websWrite(wp, "<tr>");
		ret += websWrite(wp, "<td></td>");

		/* Print address range */
		ret += websWrite(wp, "<td><input name=\"filter_client_from_start%d\" value=\"%s\" size=\"15\" maxlength=\"15\"></td>",
				 i, valid ? inet_ntoa(start.match.src.ipaddr) : "");
		ret += websWrite(wp, "<td>-</td>");
		ret += websWrite(wp, "<td><input name=\"filter_client_from_end%d\" value=\"%s\" size=\"15\" maxlength=\"15\"></td>",
				 i, valid ? inet_ntoa(end.match.src.ipaddr) : "");
		ret += websWrite(wp, "<td></td>");

		/* Print protocol */
		ret += websWrite(wp, "<td>");
		ret += websWrite(wp, "<select name=\"filter_client_proto%d\">", i);
		ret += websWrite(wp, "<option value=\"tcp\" %s>TCP</option>",
				 valid && start.match.ipproto == IPPROTO_TCP ? "selected" : "");
		ret += websWrite(wp, "<option value=\"udp\" %s>UDP</option>",
				 valid && start.match.ipproto == IPPROTO_UDP ? "selected" : "");
		ret += websWrite(wp, "</select>");
		ret += websWrite(wp, "</td>");
		ret += websWrite(wp, "<td></td>");

		/* Print port range */
		if (valid)
			snprintf(port, sizeof(port), "%d", ntohs(start.match.dst.ports[0]));
		else
			*port = '\0';
		ret += websWrite(wp, "<td><input name=\"filter_client_to_start%d\" value=\"%s\" size=\"5\" maxlength=\"5\"></td>",
				 i, port);
		ret += websWrite(wp, "<td>-</td>");
		if (valid)
			snprintf(port, sizeof(port), "%d", ntohs(start.match.dst.ports[1]));
		else
			*port = '\0';
		ret += websWrite(wp, "<td><input name=\"filter_client_to_end%d\" value=\"%s\" size=\"5\" maxlength=\"5\"></td>",
				 i, port);
		ret += websWrite(wp, "<td></td>");

		/* Print day range */
		ret += websWrite(wp, "<td>");
		ret += websWrite(wp, "<select name=\"filter_client_from_day%d\">", i);
		for (j = 0; j < ARRAYSIZE(days); j++)
			ret += websWrite(wp, "<option value=\"%d\" %s>%s</option>",
					 j, valid && start.match.days[0] == j ? "selected" : "", days[j]);
		ret += websWrite(wp, "</select>");
		ret += websWrite(wp, "</td>");
		ret += websWrite(wp, "<td>-</td>");
		ret += websWrite(wp, "<td>");
		ret += websWrite(wp, "<select name=\"filter_client_to_day%d\">", i);
		for (j = 0; j < ARRAYSIZE(days); j++)
			ret += websWrite(wp, "<option value=\"%d\" %s>%s</option>",
					 j, valid && start.match.days[1] == j ? "selected" : "", days[j]);
		ret += websWrite(wp, "</select>");
		ret += websWrite(wp, "</td>");
		ret += websWrite(wp, "<td></td>");

		/* Print time range */
		ret += websWrite(wp, "<td>");
		ret += websWrite(wp, "<select name=\"filter_client_from_sec%d\">", i);
		for (j = 0; j < ARRAYSIZE(hours); j++)
			ret += websWrite(wp, "<option value=\"%d\" %s>%s</option>",
					 j * 3600, valid && start.match.secs[0] == (j * 3600) ? "selected" : "", hours[j]);
		ret += websWrite(wp, "</select>");
		ret += websWrite(wp, "</td>");
		ret += websWrite(wp, "<td>-</td>");

		ret += websWrite(wp, "<td>");
		ret += websWrite(wp, "<select name=\"filter_client_to_sec%d\">", i);
		for (j = 0; j < ARRAYSIZE(hours); j++)
			ret += websWrite(wp, "<option value=\"%d\" %s>%s</option>",
					 j * 3600, valid && start.match.secs[1] == (j * 3600) ? "selected" : "", hours[j]);
		/* Special case for 11:59:59 PM */
		ret += websWrite(wp, "<option value=\"%d\" %s>12:00 AM</option>",
				 24 * 3600 - 1, valid && start.match.secs[1] == (24 * 3600 - 1) ? "selected" : "");
		ret += websWrite(wp, "</select>");
		ret += websWrite(wp, "</td>");
		ret += websWrite(wp, "<td></td>");

		/* Print enable */
		ret += websWrite(wp, "<td><input type=\"checkbox\" name=\"filter_client_enable%d\" %s></td>",
				 i, valid && !(start.match.flags & NETCONF_DISABLED) ? "checked" : "");

		ret += websWrite(wp, "</tr>");
	}

	return ret;
}
REG_EJ_HANDLER(filter_client);

/*
 * Example:
 * <% filter_url(1, 10); %> produces a table of the first 10 client filter entries
 */
static int
ej_filter_url(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;

#ifdef __CONFIG_URLFILTER__
	int i, n;
	netconf_urlfilter_t start, end;
	bool valid;

	if (ejArgs(argc, argv, "%d %d", &i, &n) < 2) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	/* Start of table */
	ret += websWrite(wp, "<p>");
	ret += websWrite(wp, "<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\">");
	ret += websWrite(wp, "<tr>");
	ret += websWrite(wp, "<th width=\"310\" valign=\"top\" rowspan=\"11\" ");
	ret += websWrite(wp, "onMouseOver=");
	ret += websWrite(wp,
	"\"return overlib('Filter packets from IP addresses destined to certain URL.', LEFT);\" ");
	ret += websWrite(wp, "onMouseOut=\"return nd();\">");
	ret += websWrite(wp, "<input type=\"hidden\" name=\"filter_url\" value=\"10\">");
	ret += websWrite(wp, "URL Filters:&nbsp;&nbsp;");
	ret += websWrite(wp, "</th>");
	ret += websWrite(wp, "<td>&nbsp;&nbsp;</td>");
	ret += websWrite(wp, "<td class=\"label\" colspan=\"3\">LAN IP Address Range</td>");
	ret += websWrite(wp, "<td>&nbsp;</td>");
	ret += websWrite(wp, "<td class=\"label\">URL filter string</td>");
	ret += websWrite(wp, "<td class=\"label\">Enabled</td>");
	ret += websWrite(wp, "</tr>");

	/* Entries */
	for (; i <= n; i++) {
		valid = get_filter_url(i, &start, &end);

		ret += websWrite(wp, "<tr>");
		ret += websWrite(wp, "<td></td>");

		/* Print address range */
		ret += websWrite(wp, "<td><input name=\"filter_url_from_start%d\" value=\"%s\" size=\"15\" maxlength=\"15\"></td>",
				 i, valid ? inet_ntoa(start.match.src.ipaddr) : "");
		ret += websWrite(wp, "<td>-</td>");
		ret += websWrite(wp, "<td><input name=\"filter_url_from_end%d\" value=\"%s\" size=\"15\" maxlength=\"15\"></td>",
				 i, valid ? inet_ntoa(end.match.src.ipaddr) : "");
		ret += websWrite(wp, "<td></td>");

		/* Print URL string */
		ret += websWrite(wp, "<td><input name=\"filter_url_addr%d\" value=\"%s\" size=\"40\" maxlength=\"128\"></td>",
				 i, valid ?  start.url : "");

		/* Print enable */
		ret += websWrite(wp, "<td><input type=\"checkbox\" name=\"filter_url_enable%d\" %s></td>",
				 i, valid && !(start.match.flags & NETCONF_DISABLED) ? "checked" : "");

		ret += websWrite(wp, "</tr>");
	}

	/* End of table */
	ret += websWrite(wp, "</table>");
#endif /* __CONFIG_URLFILTER__ */

	return ret;
}
REG_EJ_HANDLER(filter_url);

#endif	/* __CONFIG_NAT__ */

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
ej_wps_config_change_display(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __CONFIG_WPS__
	int wps_sta = 0;
	char prefix[] = "wlXXXXXXXXXX_";
	char tmp[] = "wlXXXXXXXXXX_mode";
	char *mode;
	char *closed;

	if (!make_wl_prefix(prefix, sizeof(prefix), 1, NULL)) {
		websError(wp, 400, "unit number variable doesn't exist\n");
		return EINVAL;
	}

	/* Get wl_mode */
	snprintf(tmp, sizeof(tmp), "%smode", tmp);
	mode = nvram_safe_get(tmp);
	if (!strcmp(mode, "wet") ||
		!strcmp(mode, "sta") ||
		!strcmp(mode, "psr") ||
		!strcmp(mode, "psta") ||
		!strcmp(mode, "mac_spoof")) {

		wps_sta = 1;
	}

	/* Get wl_closed */
	snprintf(tmp, sizeof(tmp), "%sclosed", prefix);
	closed = nvram_safe_get(tmp);

	websWrite(wp,"    var wps_sta = \"%d\";\n", wps_sta);
	websWrite(wp,"    var wl_closed = \"%s\";\n", closed);
	websWrite(wp,"    var msg = \"Enable WPS will change the SSID Network Type to Open, Are you sure?\";\n");

	websWrite(wp,"    if ((wps_sta == \"0\") && (wl_closed == \"1\") &&\n");
	websWrite(wp,"        (document.forms[0].wl_wps_mode.value == \"enabled\") && !confirm(msg)) {\n");
	websWrite(wp,"        document.forms[0].wl_wps_mode.value = \"disabled\";\n");
	websWrite(wp,"    }\n");
#endif /* __CONFIG_WPS__ */

	return 1;
}
REG_EJ_HANDLER(wps_config_change_display);

static int
ej_wps_current_psk_window_display(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __CONFIG_WPS__
	char prefix[] = "wlXXXXXXXXXX_";
	char tmp[100];
	char *value;

	if (!make_wl_prefix(prefix, sizeof(prefix), 1, NULL)) {
		websError(wp, 400, "unit number variable doesn't exist\n");
		return -1;
	}
	snprintf(tmp, sizeof(tmp), "%swpa_psk", prefix);
	value = nvram_safe_get(tmp);

	websWrite(wp,"	var psk_window = window.open(\"\", \"\", \"toolbar=no,scrollbars=yes,width=400,height=100\");\n");
	websWrite(wp,"	psk_window.document.write(\"The WPA passphrase is %s\");\n", value);
	websWrite(wp,"	psk_window.document.close();\n");
#endif /* __CONFIG_WPS__ */
	return 1;
}
REG_EJ_HANDLER(wps_current_psk_window_display);

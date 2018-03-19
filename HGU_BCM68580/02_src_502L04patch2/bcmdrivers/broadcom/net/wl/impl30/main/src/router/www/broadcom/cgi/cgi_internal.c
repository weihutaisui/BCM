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

static int
ej_syslog(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp;
	char buf[256] = "/sbin/logread > ";
	char tmp[] = "/tmp/log.XXXXXX";
	int ret;

	if (!nvram_match("log_ram_enable", "1")) {
		websError(wp, 400, "\"Syslog in RAM\" is not enabled.\n");
		return (-1);
	}

	mktemp(tmp);
	strncat(buf, tmp, sizeof(buf) - strlen(buf) - 1);
	system(buf);

	fp = fopen(tmp, "r");

	unlink(tmp);

	if (fp == NULL) {
		websError(wp, 400, "logread error\n");
		return (-1);
	}

	websWrite(wp, "<pre>");

	ret = 0;
	while(fgets(buf, sizeof(buf), fp))
		ret += websWrite(wp, buf);

	ret += websWrite(wp, "</pre>");

	fclose(fp);

	return (ret);
}
REG_EJ_HANDLER(syslog);
#endif /* defined(linux) */

static int
ej_wl_radio_roam_option(int eid, webs_t wp, int argc, char_t **argv)
{
	char tmp[NVRAM_BUFSIZE], prefix[] = "wlXXXXXXXXXX_";
	char *name=NULL;
	int radio_status = 0;

	if (!make_wl_prefix(prefix,sizeof(prefix),0,NULL)) {
		websError(wp, 400, "unit number variable doesn't exist\n");
		return -1;
	}
	name = nvram_get(strcat_r(prefix, "ifname", tmp));

	if (!name){
		websError(wp, 400, "Could not find: %s\n",strcat_r(prefix, "ifname", tmp));
		return -1;
	}

#ifdef DSLCPE_ENDIAN
	wl_endian_probe(name);
#endif

	wl_ioctl(name, WLC_GET_RADIO, &radio_status, sizeof (radio_status));
#ifdef DSLCPE_ENDIAN
	radio_status=etoh32(radio_status);
#endif 
	radio_status &= WL_RADIO_SW_DISABLE | WL_RADIO_HW_DISABLE;

	if (!radio_status) /* Radio on*/
		websWrite(wp, "<input type=\"submit\" name=\"action\" value=\"RadioOff\" >");
	else /* Radio Off */
		websWrite(wp, "<input type=\"submit\" name=\"action\" value=\"RadioOn\" >");

	return 1;

}
REG_EJ_HANDLER(wl_radio_roam_option);

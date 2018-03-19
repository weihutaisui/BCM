/*
 * Wireless network adapter utilities
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
 * $Id: wl.c 704368 2017-06-13 09:05:36Z $
 */
#include <typedefs.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <sys/ioctl.h>
#if	defined(__ECOS)
#include <sys/socket.h>
#endif // endif
#include <net/if.h>
#include <bcmendian.h>
#include <bcmutils.h>
#include <wlutils.h>
#include <bcmconfig.h>

#ifndef MAX_WLAN_ADAPTER
#define MAX_WLAN_ADAPTER	4
#endif	/* MAX_WLAN_ADAPTER */

/* Global swap variable */
bool gg_swap = FALSE;

/*
 * Probes the wireless interface for endianness.
 * Maintains a static array for each interface.
 * First probe for the adapter will set the swap variable alongwith
 * the static array for that particular interface index, subsequent invokations
 * will not probe the adapter, swap variable will be set based on the
 * value present in array for the interface index.
 */
int
wl_endian_probe(char *name)
{
	int ret = 0, val;
	char *c = name;	/* Can be wl0, wl1, wl0.1, wl1.0 etc. */
	uint8 idx = MAX_WLAN_ADAPTER;
	static int endian_stat[MAX_WLAN_ADAPTER] = {0};

	while (*c != '\0') {
	        if (isdigit(*c)) {
		        idx = *c - '0';
		        break;
		} else {
			c++;
		}
	}

	if (idx >= MAX_WLAN_ADAPTER) {
		fprintf(stderr, "Error: WLAN adapter index out of range in %s at line %d\n",
			__FUNCTION__, __LINE__);
		goto end;
	} else if (endian_stat[idx] != 0) {
		gg_swap = endian_stat[idx] > 0 ? TRUE : FALSE;
		goto end;
	}

	if ((ret = wl_ioctl(name, WLC_GET_MAGIC, &val, sizeof(int))) < 0) {
		gg_swap = FALSE;
		endian_stat[idx] = -1;
		goto end;
	}

	if (val == (int)bcmswap32(WLC_IOCTL_MAGIC)) {
		gg_swap = TRUE;
		endian_stat[idx] = 1;
	} else {
		gg_swap = FALSE;
		endian_stat[idx] = -1;
	}

end:
	return ret;
}

int
wl_probe(char *name)
{
	int ret, val;

#if defined(linux) || defined(__ECOS)
	char buf[DEV_TYPE_LEN];
	if ((ret = wl_get_dev_type(name, buf, DEV_TYPE_LEN)) < 0)
		return ret;
	/* Check interface */
	if (strncmp(buf, "wl", 2))
		return -1;
#else
	/* Check interface */
	if ((ret = wl_ioctl(name, WLC_GET_MAGIC, &val, sizeof(val))))
		return ret;
#endif // endif
#ifdef DSLCPE_ENDIAN
	 wl_endian_probe(name);
#endif
	if ((ret = wl_ioctl(name, WLC_GET_VERSION, &val, sizeof(val))))
		return ret;
#ifdef DSLCPE_ENDIAN
	if (etoh32(val) > WLC_IOCTL_VERSION)
#else
	if (val > WLC_IOCTL_VERSION)
#endif
		return -1;

	return ret;
}

#ifdef __CONFIG_DHDAP__
#include <dhdioctl.h>
/*
 * Probe the specified interface.
 * @param	name	interface name
 * @return	0       if using dhd driver
 *          <0      otherwise
 */
int
dhd_probe(char *name)
{
	int ret, val;
	val = 0;
	/* Check interface */
	ret = dhd_ioctl(name, DHD_GET_MAGIC, &val, sizeof(val));
	if (val == WLC_IOCTL_MAGIC) {
		ret = 1; /* is_dhd = !dhd_probe(), so ret 1 for WL */
	} else if (val == DHD_IOCTL_MAGIC) {
		ret = 0;
	} else {
		if (ret < 0) {
			perror("dhd_ioctl");
		}
		ret = 1; /* default: WL mode */
	}
	return ret;
}
#endif /* __CONFIG_DHDAP__ */

int
wl_iovar_getbuf(char *ifname, char *iovar, void *param, int paramlen, void *bufptr, int buflen)
{
	int err;
	uint namelen;
	uint iolen;

	namelen = strlen(iovar) + 1;	 /* length of iovar name plus null */
	iolen = namelen + paramlen;

	/* check for overflow */
	if (iolen > buflen)
		return (BCME_BUFTOOSHORT);

	memcpy(bufptr, iovar, namelen);	/* copy iovar name including null */
	memcpy((int8*)bufptr + namelen, param, paramlen);

	err = wl_ioctl(ifname, WLC_GET_VAR, bufptr, buflen);

	return (err);
}

#ifdef __CONFIG_DHDAP__
int
dhd_iovar_setbuf(char *ifname, char *iovar, void *param, int paramlen, void *bufptr, int buflen)
{
	uint namelen;
	uint iolen;

	namelen = strlen(iovar) + 1;	 /* length of iovar name plus null */
	iolen = namelen + paramlen;

	/* check for overflow */
	if (iolen > buflen)
		return (BCME_BUFTOOSHORT);

	memcpy(bufptr, iovar, namelen);	/* copy iovar name including null */
	memcpy((int8*)bufptr + namelen, param, paramlen);

	return dhd_ioctl(ifname, WLC_SET_VAR, bufptr, iolen);
}
#endif /* __CONFIG_DHDAP__ */

int
wl_iovar_setbuf(char *ifname, char *iovar, void *param, int paramlen, void *bufptr, int buflen)
{
	uint namelen;
	uint iolen;

	namelen = strlen(iovar) + 1;	 /* length of iovar name plus null */
	iolen = namelen + paramlen;

	/* check for overflow */
	if (iolen > buflen)
		return (BCME_BUFTOOSHORT);

	memcpy(bufptr, iovar, namelen);	/* copy iovar name including null */
	memcpy((int8*)bufptr + namelen, param, paramlen);

	return wl_ioctl(ifname, WLC_SET_VAR, bufptr, iolen);
}

#ifdef __CONFIG_DHDAP__
int
dhd_iovar_set(char *ifname, char *iovar, void *param, int paramlen)
{
	char smbuf[WLC_IOCTL_SMLEN];

	return dhd_iovar_setbuf(ifname, iovar, param, paramlen, smbuf, sizeof(smbuf));
}
#endif /* __CONFIG_DHDAP__ */

int
wl_iovar_set(char *ifname, char *iovar, void *param, int paramlen)
{
	char smbuf[WLC_IOCTL_SMLEN];

	return wl_iovar_setbuf(ifname, iovar, param, paramlen, smbuf, sizeof(smbuf));
}

int
wl_iovar_get(char *ifname, char *iovar, void *bufptr, int buflen)
{
	char smbuf[WLC_IOCTL_SMLEN];
	int ret;

	/* use the return buffer if it is bigger than what we have on the stack */
	if (buflen > sizeof(smbuf)) {
		ret = wl_iovar_getbuf(ifname, iovar, NULL, 0, bufptr, buflen);
	} else {
		ret = wl_iovar_getbuf(ifname, iovar, NULL, 0, smbuf, sizeof(smbuf));
		if (ret == 0)
			memcpy(bufptr, smbuf, buflen);
	}

	return ret;
}

#ifdef __CONFIG_DHDAP__
/*
 * set named driver variable to int value
 * calling example: dhd_iovar_setint(ifname, "arate", rate)
*/
int
dhd_iovar_setint(char *ifname, char *iovar, int val)
{
	return dhd_iovar_set(ifname, iovar, &val, sizeof(val));
}
#endif /* __CONFIG_DHDAP__ */

/*
 * set named driver variable to int value
 * calling example: wl_iovar_setint(ifname, "arate", rate)
*/
int
wl_iovar_setint(char *ifname, char *iovar, int val)
{
#ifdef DSLCPE_ENDIAN
	wl_endian_probe(ifname);
	val=htoe32(val);
#endif
	return wl_iovar_set(ifname, iovar, &val, sizeof(val));
}

/*
 * get named driver variable to int value and return error indication
 * calling example: wl_iovar_getint(ifname, "arate", &rate)
 */
int
wl_iovar_getint(char *ifname, char *iovar, int *val)
{
#ifdef DSLCPE_ENDIAN
	int ret=0;
	wl_endian_probe(ifname);
	ret=wl_iovar_get(ifname, iovar, val, sizeof(int));
	*val=etoh32(*val);
	return ret;
#else
	return wl_iovar_get(ifname, iovar, val, sizeof(int));
#endif
}

/*
 * format a bsscfg indexed iovar buffer
 */
static int
wl_bssiovar_mkbuf(char *iovar, int bssidx, void *param, int paramlen, void *bufptr, int buflen,
                  int *plen)
{
	char *prefix = "bsscfg:";
	int8* p;
	uint prefixlen;
	uint namelen;
	uint iolen;

	prefixlen = strlen(prefix);	/* length of bsscfg prefix */
	namelen = strlen(iovar) + 1;	/* length of iovar name + null */
	iolen = prefixlen + namelen + sizeof(int) + paramlen;

	/* check for overflow */
	if (buflen < 0 || iolen > (uint)buflen) {
		*plen = 0;
		return BCME_BUFTOOSHORT;
	}

	p = (int8*)bufptr;

	/* copy prefix, no null */
	memcpy(p, prefix, prefixlen);
	p += prefixlen;

	/* copy iovar name including null */
#ifdef DSLCPE_ENDIAN
	bssidx = htoe32(bssidx);
#endif
	memcpy(p, iovar, namelen);
	p += namelen;

	/* bss config index as first param */
	memcpy(p, &bssidx, sizeof(int32));
	p += sizeof(int32);

	/* parameter buffer follows */
	if (paramlen)
		memcpy(p, param, paramlen);

	*plen = iolen;
	return 0;
}

/*
 * set named & bss indexed driver variable to buffer value
 */
int
wl_bssiovar_setbuf(char *ifname, char *iovar, int bssidx, void *param, int paramlen, void *bufptr,
                   int buflen)
{
	int err;
	int iolen;
#ifdef DSLCPE_ENDIAN
	wl_endian_probe(ifname);
#endif
	err = wl_bssiovar_mkbuf(iovar, bssidx, param, paramlen, bufptr, buflen, &iolen);
	if (err)
		return err;

	return wl_ioctl(ifname, WLC_SET_VAR, bufptr, iolen);
}

#ifdef __CONFIG_DHDAP__
int
dhd_ioctl(char *name, int cmd, void *buf, int len)
{
	struct ifreq ifr;
	dhd_ioctl_t ioc;
	int ret = 0;
	int s;
	char buffer[WLC_IOCTL_SMLEN];

	/* open socket to kernel */
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		return -1;
	}

	/* do it */
	if (cmd == WLC_SET_VAR) {
		cmd = DHD_SET_VAR;
	} else if (cmd == WLC_GET_VAR) {
		cmd = DHD_GET_VAR;
	}

	ioc.cmd = cmd;
	ioc.buf = buf;
	ioc.len = len;
	ioc.set = FALSE;
	ioc.driver = DHD_IOCTL_MAGIC;
	ioc.used = 0;
	ioc.needed = 0;

	strncpy(ifr.ifr_name, name, sizeof(ifr.ifr_name) - 1);
	ifr.ifr_name[sizeof(ifr.ifr_name) - 1] = '\0';

	ifr.ifr_data = (caddr_t) &ioc;
	if ((ret = ioctl(s, SIOCDEVPRIVATE, &ifr)) < 0)
		if (cmd != WLC_GET_MAGIC && cmd != WLC_GET_BSSID) {
			snprintf(buffer, sizeof(buffer), "%s: cmd=%d", name, cmd);
			perror(buffer);
		}
	/* cleanup */
	close(s);
	return ret;
}

/*
 * set named & bss indexed driver variable to buffer value
 */
int
dhd_bssiovar_setbuf(char *ifname, char *iovar, int bssidx, void *param, int paramlen, void *bufptr,
                   int buflen)
{
	int err;
	int iolen;

#ifdef DSLCPE_ENDIAN
	wl_endian_probe(ifname);
	err = wl_bssiovar_mkbuf(iovar, htoe32(bssidx), param, paramlen, bufptr, buflen, &iolen);
#else
	err = wl_bssiovar_mkbuf(iovar, bssidx, param, paramlen, bufptr, buflen, &iolen);
#endif
	if (err)
		return err;

	return dhd_ioctl(ifname, WLC_SET_VAR, bufptr, iolen);
}
#endif /* __CONFIG_DHDAP__ */

/*
 * get named & bss indexed driver variable buffer value
 */
int
wl_bssiovar_getbuf(char *ifname, char *iovar, int bssidx, void *param, int paramlen, void *bufptr,
                   int buflen)
{
	int err;
	int iolen;

#ifdef DSLCPE_ENDIAN
	wl_endian_probe(ifname);
#endif
	err = wl_bssiovar_mkbuf(iovar, bssidx, param, paramlen, bufptr, buflen, &iolen);
	if (err)
		return err;

	return wl_ioctl(ifname, WLC_GET_VAR, bufptr, buflen);
}

/*
 * set named & bss indexed driver variable to buffer value
 */
int
wl_bssiovar_set(char *ifname, char *iovar, int bssidx, void *param, int paramlen)
{
	char smbuf[WLC_IOCTL_SMLEN];

	return wl_bssiovar_setbuf(ifname, iovar, bssidx, param, paramlen, smbuf, sizeof(smbuf));
}

#ifdef __CONFIG_DHDAP__
/*
 * set named & bss indexed driver variable to buffer value
 */
int
dhd_bssiovar_set(char *ifname, char *iovar, int bssidx, void *param, int paramlen)
{
	char smbuf[WLC_IOCTL_SMLEN];

	return dhd_bssiovar_setbuf(ifname, iovar, bssidx, param, paramlen, smbuf, sizeof(smbuf));
}
#endif // endif

/*
 * get named & bss indexed driver variable buffer value
 */
int
wl_bssiovar_get(char *ifname, char *iovar, int bssidx, void *outbuf, int len)
{
	char smbuf[WLC_IOCTL_SMLEN];
	int err;

	/* use the return buffer if it is bigger than what we have on the stack */
	if (len > (int)sizeof(smbuf)) {
		err = wl_bssiovar_getbuf(ifname, iovar, bssidx, NULL, 0, outbuf, len);
	} else {
		memset(smbuf, 0, sizeof(smbuf));
		err = wl_bssiovar_getbuf(ifname, iovar, bssidx, NULL, 0, smbuf, sizeof(smbuf));
		if (err == 0)
			memcpy(outbuf, smbuf, len);
	}

	return err;
}

/*
 * set named & bss indexed driver variable to int value
 */
int
wl_bssiovar_setint(char *ifname, char *iovar, int bssidx, int val)
{
#ifdef DSLCPE_ENDIAN
	wl_endian_probe(ifname);
	val=htoe32(val);
#endif
	return wl_bssiovar_set(ifname, iovar, bssidx, &val, sizeof(int));
}

#ifdef __CONFIG_DHDAP__
/*
 * set named & bss indexed driver variable to int value
 */
int
dhd_bssiovar_setint(char *ifname, char *iovar, int bssidx, int val)
{
	return dhd_bssiovar_set(ifname, iovar, bssidx, &val, sizeof(int));
}
#endif // endif

/*
void
wl_printlasterror(char *name)
{
	char err_buf[WLC_IOCTL_SMLEN];
	strcpy(err_buf, "bcmerrstr");

	fprintf(stderr, "Error: ");
	if ( wl_ioctl(name, WLC_GET_VAR, err_buf, sizeof (err_buf)) != 0)
		fprintf(stderr, "Error getting the Errorstring from driver\n");
	else
		fprintf(stderr, err_buf);
}
*/

/*
 * Linux-specific portion of SSD (SSID Steering Daemon)
 * (OS dependent file)
 *
 * Copyright (C) 2014, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 *
 * $Id: ssd.c $
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <linux/if_packet.h>
#include <typedefs.h>
#include <bcmutils.h>
#include <proto/ethernet.h>
#include <wlutils.h>
#include <bcmnvram.h>
#include <ctype.h>
#include <wlif_utils.h>
#include <bcmparams.h>
#include <shutils.h>
#include <ssd.h>
#include <security_ipc.h>
#ifdef DSLCPE
#include <time.h>
#endif


extern char * strcasestr(const char * s1, const char * s2);

#ifdef SSD_DEBUG
int ssd_msglevel = SSD_DEBUG_ERROR | SSD_DEBUG_WARNING | SSD_DEBUG_INFO;
#define SSD_DEBUG_DUMP
#else
int ssd_msglevel = SSD_DEBUG_ERROR | SSD_DEBUG_WARNING;
#endif // endif

/* some global variables */
ssd_maclist_t *ssd_maclist_hdr = NULL; /* associated sta maclist to be handled */

#ifdef SSD_DEBUG_DUMP
static void ssd_hexdump_ascii(const char *title, const unsigned char *buf,
        unsigned int len)
{
	int i, llen;
	const unsigned char *pos = buf;

	printf("%s - (data len=%lu):\n", title, (unsigned long) len);
	while (len) {
		llen = len > MAX_LINE_LEN ? MAX_LINE_LEN : len;
		printf("    ");
		for (i = 0; i < llen; i++)
			printf(" %02x", pos[i]);
		for (i = llen; i < MAX_LINE_LEN; i++)
			printf("   ");
		printf("   ");
		for (i = 0; i < llen; i++) {
			if (isprint(pos[i]))
				printf("%c", pos[i]);
			else
				printf("*");
		}
		for (i = llen; i < MAX_LINE_LEN; i++)
			printf(" ");
		printf("\n");
		pos += llen;
		len -= llen;
	}
}
#endif /* SSD_DEBUG_DUMP */

/* add addr to assoc maclist */
static void ssd_add_assoc_maclist(char *ssid, bcm_event_t *dpkt)
{
	ssd_maclist_t *ptr;
	char buf[32];
	struct ether_addr *sta_addr;

	if (!dpkt) {
		SSD_PRINT_ERROR("Exiting: empty event packet\n");
		return;
	}

	sta_addr = &(dpkt->event.addr);

	/* adding to maclist */
	ptr = ssd_maclist_hdr;

	while (ptr) {
		if ((eacmp(&(ptr->addr), sta_addr) == 0) &&
			(ptr->ifidx == dpkt->event.ifidx) &&
			(ptr->bsscfgidx == dpkt->event.bsscfgidx)) {
			ptr->timestamp = time(NULL);
			SSD_PRINT_INFO("update ssid %s, MAC=%s\n",
				ssid, ether_etoa(sta_addr->octet, buf));
			return;
		}
		ptr = ptr->next;
	}

	/* add new sta to maclist */
	ptr = malloc(sizeof(ssd_maclist_t));
	if (!ptr) {
		SSD_PRINT_ERROR("Exiting malloc failure\n");
		return;
	}
	memset(ptr, 0, sizeof(ssd_maclist_t));
	memcpy(&ptr->addr, sta_addr, sizeof(struct ether_addr));
	ptr->next = ssd_maclist_hdr;
	ssd_maclist_hdr = ptr;

	ptr->timestamp = time(NULL);
	strncpy(ptr->ssid, ssid, sizeof(ptr->ssid));
	ptr->ssid[sizeof(ptr->ssid)-1] = '\0';
	ptr->ifidx = dpkt->event.ifidx;
	ptr->bsscfgidx = dpkt->event.bsscfgidx;
	ptr->security = 0;

	SSD_PRINT_INFO("add ssid %s, MAC=%s\n", ssid, ether_etoa(sta_addr->octet, buf));

	return;
}

/* remove addr from maclist */
static void ssd_del_assoc_maclist(struct ether_addr *sta_addr, uint8 ifidx, uint8 bsscfgidx)
{
	ssd_maclist_t *ptr, *prev;
	char buf[32];
	int found = 0;

	ptr = ssd_maclist_hdr;

	if (ptr == NULL) {
		SSD_PRINT_INFO("ssd_mac_list is empty\n");
		return;
	}

	if ((eacmp(&(ptr->addr), sta_addr) == 0) &&
		(ptr->ifidx == ifidx) && (ptr->bsscfgidx == bsscfgidx)) {
		/* this is the first one */
		ssd_maclist_hdr = ptr->next;
		found = 1;
	}
	else {
		prev = ptr;
		ptr = ptr->next;

		while (ptr) {
			if ((eacmp(&(ptr->addr), sta_addr) == 0) &&
				(ptr->ifidx == ifidx) && (ptr->bsscfgidx == bsscfgidx)) {
				prev->next = ptr->next;
				found = 1;
				break;
			}
			prev = ptr;
			ptr = ptr->next;
		}
	}

	if (ptr && found) {
		SSD_PRINT_INFO("Free ssid %s, MAC=%s, timestamp=%lu\n",
			ptr->ssid, ether_etoa(ptr->addr.octet, buf), ptr->timestamp);
		free(ptr);
	}

	/* Debug only, show list after removal */
	ptr = ssd_maclist_hdr;
	while (ptr) {
		SSD_PRINT_INFO("ssid %s, MAC=%s, timestamp=%lu\n",
			ptr->ssid, ether_etoa(ptr->addr.octet, buf), ptr->timestamp);
		ptr = ptr->next;
	}

	return;
}

static int ssd_ssid_type(char *ssid, char* ifname)
{
	int ret;
	char nvram_name[100], prefix[IFNAMSIZ];
	char *nvram_str;

	if (osifname_to_nvifname(ifname, prefix, IFNAMSIZ) < 0) {
		SSD_PRINT_WARNING("fail to convert ifname %s to nv_ifname\n", ifname);
		return SSD_TYPE_DISABLE;
	}

	SSD_PRINT_INFO("os_ifname=%s; nv_ifname=%s\n", ifname, prefix);

	/* compare ssid first */
	if (ssid) {
		sprintf(nvram_name, "%s_ssid", prefix);
		nvram_str = nvram_get(nvram_name);
		if ((nvram_str == NULL) || strcmp(nvram_str, ssid)) {
			SSD_PRINT_INFO("ssid (%s) not match (%s) on nvram (%s)\n",
				ssid, nvram_str, nvram_name);
			return SSD_TYPE_DISABLE;
		}
	}

	sprintf(nvram_name, "%s_%s", prefix, NVRAM_SSD_SSID_TYPE);
	nvram_str = nvram_get(nvram_name);

	SSD_PRINT_INFO("nvram=%s, value=%s\n", nvram_name, nvram_str);

	if (nvram_str == NULL) {
		ret = SSD_TYPE_DISABLE;
	}
	else {
		ret = atoi(nvram_str);
		if ((ret < SSD_TYPE_DISABLE) || (ret > SSD_TYPE_PUBLIC))
			ret = SSD_TYPE_DISABLE;
	}

	return ret;
}

static char *nvram_get_with_prefix(char *prefix, char *basename)
{
	char nvram_name[100];
	sprintf(nvram_name, "%s_%s", prefix, basename);
	return nvram_get(nvram_name);
}

static void ssd_update_deny_list(char *prefix, ssd_maclist_t *ptr)
{
	char *nvram_str;
	char buf[32], maclist_str[1000];
	char mbuf[WLC_IOCTL_SMLEN] __attribute__ ((aligned(4)));
	struct maclist *maclist_ptr;
	struct ether_addr *ea;
	char var[80], *next;
	char os_ifname[IFNAMSIZ];
	int change_macmode = 0, val;

	/* xx_bss_enabled=1 */
	nvram_str = nvram_get_with_prefix(prefix, "bss_enabled");
	if ((nvram_str == NULL) || (atoi(nvram_str) != 1)) {
		return;
	}
	SSD_PRINT_INFO("%s_%s=%s\n", prefix, "bss_enabled", nvram_str);

	/* if xx_mode == ap */
	nvram_str = nvram_get_with_prefix(prefix, "mode");
	if ((nvram_str == NULL) || strcmp(nvram_str, "ap")) {
		return;
	}
	SSD_PRINT_INFO("%s_%s=%s\n", prefix, "mode", nvram_str);

	/* if xx_ssd_type == SSD_TYPE_PUBLIC */
	nvram_str = nvram_get_with_prefix(prefix, "ssd_type");
	if ((nvram_str == NULL) || (atoi(nvram_str) != SSD_TYPE_PUBLIC)) {
		return;
	}
	SSD_PRINT_INFO("%s_%s=%s\n", prefix, "ssd_type", nvram_str);

	/* if xx_macmode == deny or disabled */
	nvram_str = nvram_get_with_prefix(prefix, "macmode");
	if ((nvram_str == NULL) || (strcmp(nvram_str, "allow") == 0)) {
		return;
	}

	if (strcmp(nvram_str, "disabled") == 0)
		change_macmode = 1; /* force to use "deny" when original setting is "disabled" */
	SSD_PRINT_INFO("%s_%s=%s\n", prefix, "macmode", nvram_str);

	/* add the mac to xx_maclist */
	nvram_str = nvram_get_with_prefix(prefix, "maclist");
	ether_etoa(ptr->addr.octet, buf);

	SSD_PRINT_INFO("maclists=%s, new mac=%s\n", nvram_str, buf);
#ifdef DSLCPE
	if (nvram_str==NULL||strcasestr(nvram_str, buf) == NULL)
#else
	if (strcasestr(nvram_str, buf) == NULL)
#endif
	{
		/* not exist, append the new mac */
		SSD_PRINT_INFO("change %s's deny maclist: append new MAC=%s\n", prefix, buf);

		/* update nvram */
#ifdef DSLCPE
		if (nvram_str==NULL)
		        sprintf(maclist_str, "%s", buf);
		else
		{
#endif
		sprintf(maclist_str, "%s %s", nvram_str, buf);
#ifdef DSLCPE
		}
#endif
		sprintf(buf, "%s_maclist", prefix);
		nvram_set(buf, maclist_str);

		if (change_macmode) {
			sprintf(buf, "%s_macmode", prefix);
			nvram_set(buf, "deny");
		}

		/* nvram_commit */
		nvram_commit();

		/* Set the real-time MAC list via ioctl */
		maclist_ptr = (struct maclist *)mbuf;
		maclist_ptr->count = 0;

		ea = maclist_ptr->ea;
		foreach(var, maclist_str, next) {
			if (((char *)((&ea[1])->octet)) > ((char *)(&mbuf[sizeof(mbuf)])))
				break;
			if (ether_atoe(var, ea->octet)) {
				maclist_ptr->count++;
				ea++;
			}
		}
		SSD_PRINT_INFO("maclist_ptr->count=%d\n", maclist_ptr->count);

#ifdef DSLCPE_ENDIAN
		maclist_ptr->count = htod32(maclist_ptr->count);
#endif

		/* ioctl to inform driver (need os_name) */
		if (nvifname_to_osifname(prefix, os_ifname, sizeof(os_ifname)) < 0) {
			SSD_PRINT_WARNING("fail to convert to os_ifname for %s!\n", prefix);
			return;
		}

		if (wl_ioctl(os_ifname, WLC_SET_MACLIST, mbuf, sizeof(mbuf))) {
			SSD_PRINT_WARNING("wl_ioctl WLC_SET_MACLIST error!\n");
		}

		if (change_macmode) {
#ifdef DSLCPE_ENDIAN
		        val = htod32(WLC_MACMODE_DENY);
#else
			val = WLC_MACMODE_DENY;
#endif
			if (wl_ioctl(os_ifname, WLC_SET_MACMODE, &val, sizeof(val))) {
				SSD_PRINT_WARNING("wl_ioctl WLC_SET_MACMODE error!\n");
			}
		}
	}
	else {
		SSD_PRINT_INFO("MAC %s is already in %s's deny maclist\n", buf, prefix);
	}
}

static void ssd_process_assoc_maclist(ssd_maclist_t *ptr)
{
	int i, j;
	char nv_name[IFNAMSIZ], ifname[IFNAMSIZ];
	char buf[32];

	SSD_PRINT_INFO("ssid %s, MAC=%s, timestamp=%lu\n",
		ptr->ssid, ether_etoa(ptr->addr.octet, buf), ptr->timestamp);

	/* for loop for all interfaces */
#ifdef DSLCPE
	for (i = 0; i < DEV_NUMIFS; i++) {
		sprintf(ifname, "wl%d", i);
#else
	for (i = 1; i <= DEV_NUMIFS; i++) {
		sprintf(ifname, "eth%d", i);
#endif
		if (!wl_probe(ifname)) {
			if (osifname_to_nvifname(ifname, nv_name, sizeof(nv_name)) < 0) {
				SSD_PRINT_WARNING("fail to convert ifname %s to nv_name\n", ifname);
				return;
			}

			SSD_PRINT_INFO("convert ifname %s to nv_name %s\n", ifname, nv_name);

			/* handle this primary interface */
			if (strlen(nv_name) == 3) {
				SSD_PRINT_INFO("handle the primary interface %s\n", nv_name);
				ssd_update_deny_list(nv_name, ptr);
			}

			/* for loop for all virtual BSS interface */
			for (j = 1; j < WL_MAXBSSCFG; j++) {
				sprintf(ifname, "%s.%d", nv_name, j);
				ssd_update_deny_list(ifname, ptr);
			}
		}
	}
}

static void ssd_check_assoc_maclist(void)
{
	ssd_maclist_t *ptr;
	time_t now = time(NULL);
	char buf[32];

	ptr = ssd_maclist_hdr;

	if (ptr) {
		SSD_PRINT_INFO("current timestamp=%lu\n", now);
	}

	while (ptr) {
		SSD_PRINT_INFO("ssid %s, MAC=%s, timestamp=%lu\n",
			       ptr->ssid, ether_etoa(ptr->addr.octet, buf), ptr->timestamp);
		if (now - ptr->timestamp > INTERVAL_ASSOC_CONFIRM) {
			ssd_process_assoc_maclist(ptr);
			ssd_del_assoc_maclist(&(ptr->addr), ptr->ifidx, ptr->bsscfgidx);
			/* handle one item once */
			break;
		}
		ptr = ptr->next;
	}

	return;
}

static void ssd_event_handler(int sock)
{
	int bytes;
	bcm_event_t *dpkt;
	char buf[32];
	uchar buf_ptr[MAX_EVENT_BUFFER_LEN], *ptr = buf_ptr;
	char ssid[MAX_SSID_LEN+1];
	uint8 ssid_len;
	int ssid_type;
	uint32 event_id;
	struct timeval tv = {1, 0};    /* timed out every second */
	fd_set fdset;
	int status, fdmax;

	FD_ZERO(&fdset);
	fdmax = -1;

	if (sock >= 0) {
		FD_SET(sock, &fdset);
		if (sock > fdmax)
			fdmax = sock;
	}
	else {
		SSD_PRINT_ERROR("Err: wrong socket\n");
		return;
	}

	status = select(fdmax+1, &fdset, NULL, NULL, &tv);
	if ((status > 0) && FD_ISSET(sock, &fdset)) {
		if ((bytes = recv(sock, ptr, MAX_EVENT_BUFFER_LEN, 0)) > IFNAMSIZ) {

			ptr = ptr + IFNAMSIZ;
			dpkt = (bcm_event_t *)ptr;

#ifdef SSD_DEBUG_DUMP
			ssd_hexdump_ascii("REVD:", ptr, bytes);
#endif // endif

			event_id = ntohl(dpkt->event.event_type);
			SSD_PRINT_INFO("Received event %d, MAC=%s\n",
				event_id, ether_etoa(dpkt->event.addr.octet, buf));

			if ((event_id == WLC_E_ASSOC_IND) || (event_id == WLC_E_REASSOC_IND)) {
				ptr += BCM_EVENT_HEADER_LEN;
				ssid_len = *(ptr+1);
				if ((*ptr == 0) && (ssid_len > 0) && (ssid_len < (MAX_SSID_LEN+1)))
				{
					/* SSID IE */
					strncpy(ssid, (char*)(ptr+2), ssid_len);
					ssid[ssid_len] = '\0';
					SSD_PRINT_INFO("event=%d,ssid=%s,if=%s,idx=%d,bidx=%d\n",
						event_id, ssid, dpkt->event.ifname,
						dpkt->event.ifidx, dpkt->event.bsscfgidx);

					ssid_type = ssd_ssid_type(ssid, dpkt->event.ifname);
					if (ssid_type == SSD_TYPE_PRIVATE) {
						SSD_PRINT_INFO("Add MAC=%s for private %s\n",
							ether_etoa(dpkt->event.addr.octet, buf),
							dpkt->event.ifname);
						ssd_add_assoc_maclist(ssid, dpkt);
					}
				}
			}
			else if (DISCONNECT_EVENT(event_id)) {
				/* remove the STA from the ssd_maclist */
				ssid_type = ssd_ssid_type(NULL, dpkt->event.ifname);
				if (ssid_type == SSD_TYPE_PRIVATE) {
					SSD_PRINT_INFO("Event %d: del MAC=%s ifidx=%d, bssidx=%d\n",
						event_id, ether_etoa(dpkt->event.addr.octet, buf),
						dpkt->event.ifidx, dpkt->event.bsscfgidx);
					ssd_del_assoc_maclist(&(dpkt->event.addr),
						dpkt->event.ifidx, dpkt->event.bsscfgidx);
				}
			}
		}
	}

	/* check maclist */
	if (ssd_maclist_hdr)
		ssd_check_assoc_maclist();

	return;
}

static void
ssd_main_loop(int sock)
{
	while (1) {
		ssd_event_handler(sock);
	}
}

static int ssd_eapd_socket_init(void)
{
	int reuse = 1;
	struct sockaddr_in sockaddr;
	int ssd_socket = -1;

	/* open loopback socket to communicate with EAPD */
	memset(&sockaddr, 0, sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	sockaddr.sin_port = htons(EAPD_WKSP_SSD_UDP_SPORT);

	if ((ssd_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		SSD_PRINT_ERROR("Unable to create loopback socket\n");
		return -1;
	}

	if (setsockopt(ssd_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse)) < 0) {
		SSD_PRINT_ERROR("Unable to setsockopt to loopback socket %d.\n", ssd_socket);
		goto exit1;
	}

	if (bind(ssd_socket, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0) {
		SSD_PRINT_ERROR("Unable to bind to loopback socket %d\n", ssd_socket);
		goto exit1;
	}

	SSD_PRINT_INFO("opened loopback socket %d\n", ssd_socket);
	return ssd_socket;

	/* error handling */
exit1:
	close(ssd_socket);
	return -1;
}

int
main(int argc, char **argv)
{
	int sock;

	/* UDP socket to eapd init */
	if ((sock = ssd_eapd_socket_init()) < 0) {
		SSD_PRINT_ERROR("Err: fail to init socket\n");
		return sock;
	}

	/* receive wl event from ssd-eap via UDP */
	ssd_main_loop(sock);

	close(sock);
	return 0;
}

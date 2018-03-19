/*
 * Broadcom Home Gateway Reference Design
 * Broadcom wbd Webpage functions
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
 * <<Broadcom-WL-IPTag/Open:>>
 * $Id: cgi_wbd.c 705265 2017-06-16 05:22:14Z $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <security_ipc.h>

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <linux/rtnetlink.h>
#include <bcmnvram.h>
#include <common_utils.h>
#include <wlutils.h>
#include <shutils.h>

#include "wbd_rc_shared.h"
#include "cgi_wbd.h"

#define INVALID_SOCKET				-1
#define MAX_READ_BUFFER				1448
#define WBD_LOOPBACK_IP				"127.0.0.1"
#define EAPD_WKSP_WBD_TCP_MASTERCLI_PORT	EAPD_WKSP_WBD_UDP_PORT + 0x100
#define WBD_TM_SOCKET				10
#define WBD_MIN_REFRESH_RATE			1
#define WBD_DELIM				"?=&"
#define WBD_URL_SIZE				128
#define WBD_DEFAULT_RSSI			-65
/* Indicates the level of debug message to be printed on the console */
static int wbd_msglevel = 1;

/* WBD debug print macros. */
#define WBD_DEBUG_ERROR		0x0001
#define WBD_DEBUG_WARNING	0x0002
#define WBD_DEBUG_INFO		0x0004

#define WBD_PRINT(fmt, arg...) \
	printf("WBD >> (%lu) %s: "fmt, (unsigned long)time(NULL), __FUNCTION__, ##arg)

#define WBD_ERROR(fmt, arg...) \
	if (wbd_msglevel & WBD_DEBUG_ERROR) \
		WBD_PRINT(fmt, ##arg)

#define WBD_INFO(fmt, arg...) \
	if (wbd_msglevel & WBD_DEBUG_INFO) \
		WBD_PRINT(fmt, ##arg)

/* Variable To hold output string */
static char* g_wbd_output_buffer = NULL;

/* WBD request ids */
typedef enum wbd_req_args_id {
	WBD_UNKNOWN,
	WBD_MASTERINFO,
	WBD_CONFIG,
	WBD_MASTERLOGS,
	WBD_CLEARLOGS
} wbd_req_args_id_t;

/* WBD request ids and request name mapping */
typedef struct wbd_req_args {
	wbd_req_args_id_t id;	/* ID of the request argument */
	char *req_name;		/* Name of the argument */
} wbd_req_args_t;

/* WBD request ids and name array */
wbd_req_args_t wbd_req_args_list[] = {
	{WBD_UNKNOWN, "unknown"},
	{WBD_MASTERINFO, "info"},
	{WBD_CONFIG, "config"},
	{WBD_MASTERLOGS, "logs"},
	{WBD_CLEARLOGS, "clearlogs"}
};

/* Write json answer to stream */
void
do_wbd_get(char *url, FILE *stream)
{
	if (g_wbd_output_buffer != NULL) {
		fputs(g_wbd_output_buffer, stream);
		free(g_wbd_output_buffer);
		g_wbd_output_buffer = NULL;
	}
}

/* Closes the socket */
static void
wbd_close_socket(int *sockfd)
{
	if (*sockfd == INVALID_SOCKET) {
		return;
	}
	close(*sockfd);
	*sockfd = INVALID_SOCKET;
}

/* Connects to the server given the IP address and port number */
int
wbd_connect_to_server(char* straddrs, unsigned int nport)
{
	struct sockaddr_in server_addr;
	int res, valopt;
	long arg;
	fd_set readfds;
	struct timeval tv;
	socklen_t lon;
	int sockfd;

	sockfd = INVALID_SOCKET;
	memset(&server_addr, 0, sizeof(server_addr));

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		WBD_ERROR("Error in socket is : %s\n", __FUNCTION__);
		goto error;
	}

	/* Set nonblock on the socket so we can timeout */
	if ((arg = fcntl(sockfd, F_GETFL, NULL)) < 0 ||
	fcntl(sockfd, F_SETFL, arg | O_NONBLOCK) < 0) {
		WBD_ERROR("Error in fcntl is : %s\n", __FUNCTION__);
		goto error;
	}
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(nport);
	server_addr.sin_addr.s_addr = inet_addr(straddrs);

	res = connect(sockfd, (struct sockaddr*)&server_addr,
	sizeof(struct sockaddr));
	if (res < 0) {
		if (errno == EINPROGRESS) {
			tv.tv_sec = WBD_TM_SOCKET;
			tv.tv_usec = 0;
			FD_ZERO(&readfds);
			FD_SET(sockfd, &readfds);
			if (select(sockfd+1, NULL, &readfds, NULL, &tv) > 0) {
				lon = sizeof(int);
				getsockopt(sockfd, SOL_SOCKET, SO_ERROR,
				(void*)(&valopt), &lon);
				if (valopt) {
					WBD_ERROR("Error in connection() %d - %s\n",
						valopt, strerror(valopt));
					goto error;
				}
			} else {
				WBD_ERROR("Timeout or error() %d - %s\n",
				valopt, strerror(valopt));
				goto error;
			}
		} else {
			WBD_ERROR("Error connecting %d - %s\n",
			errno, strerror(errno));
			goto error;
		}
	}
	WBD_INFO("Connection Successfull with server : %s\n", straddrs);

	return sockfd;

/* Error Handling */
error:
	WBD_INFO("Connection not sucessful\n");
	if (sockfd != INVALID_SOCKET)
		wbd_close_socket(&sockfd);

	return INVALID_SOCKET;
}

/* Sends the data to socket */
int
wbd_socket_send_data(int sockfd, char *data, unsigned int len)
{
	int nret = 0, ret = 0;
	int totalsize = len, totalsent = 0;

	/* Loop till all the data sent */
	while (totalsent < totalsize) {
		fd_set WriteFDs;
		struct timeval tv;

		FD_ZERO(&WriteFDs);

		if (sockfd == INVALID_SOCKET) {
			WBD_INFO("sockfd[%d]. Invalid socket\n", sockfd);
			goto error;
		}

		FD_SET(sockfd, &WriteFDs);

		tv.tv_sec = WBD_TM_SOCKET;
		tv.tv_usec = 0;
		if ((ret = select(sockfd+1, NULL, &WriteFDs, NULL, &tv)) > 0) {
			if (FD_ISSET(sockfd, &WriteFDs)) {
				;
			} else {
				WBD_INFO("sockfd[%d]. Exception occured\n", sockfd);
				goto error;
			}
		} else {
			goto error;
		}

		/* Send data */
		nret = send(sockfd, &(data[totalsent]), len, 0);
		if (nret < 0) {
			WBD_INFO("send error is : %s\n", __FUNCTION__);
			goto error;
		}
		totalsent += nret;
		len -= nret;
		nret = 0;
	}
	return totalsent;
error:
	return INVALID_SOCKET;
}

/* to recieve data till the null character. caller should free the memory */
int
wbd_socket_recv_data(int sockfd, char **data)
{
	unsigned int nbytes, totalread = 0, cursize = 0;
	struct timeval tv;
	fd_set ReadFDs, ExceptFDs;
	char *buffer = NULL;
	int ret = 0;

	/* Read till the null character or error */
	while (1) {
		FD_ZERO(&ReadFDs);
		FD_ZERO(&ExceptFDs);
		FD_SET(sockfd, &ReadFDs);
		FD_SET(sockfd, &ExceptFDs);
		tv.tv_sec = WBD_TM_SOCKET;
		tv.tv_usec = 0;

		/* Allocate memory for the buffer */
		if (totalread >= cursize) {
			char *tmp;

			cursize += MAX_READ_BUFFER;
			tmp = (char*)realloc(buffer, cursize);
			if (tmp == NULL) {
				WBD_ERROR("Failed to allocate memory for read buffer\n");
				goto error;
			}
			buffer = tmp;
		}
		if ((ret = select(sockfd+1, &ReadFDs, NULL, &ExceptFDs, &tv)) > 0) {
			if (FD_ISSET(sockfd, &ReadFDs)) {
				;
			} else {
				WBD_INFO("sockfd[%d]. Exception occured\n", sockfd);
				goto error;
			}
		} else {
			goto error;
		}

		nbytes = read(sockfd, buffer+totalread, (cursize - totalread));
		totalread += nbytes;

		if (nbytes <= 0) {
			WBD_INFO("sockfd[%d], read error is : %s\n", sockfd, __FUNCTION__);
			goto error;
		}

		/* Check the last byte for NULL termination */
		if (buffer[totalread-1] == '\0') {
			break;
		}
	}

	*data = buffer;
	return totalread;

error:
	if (buffer)
		free(buffer);
	return INVALID_SOCKET;
}

/* Function to send request and receive data.  */
static int
wbd_send_and_receive_data(char *req, char *ip, int port)
{
	int ret = 0, rcv_ret = 0;
	int sockfd = INVALID_SOCKET;
	char *read_buf = NULL;

	sockfd = wbd_connect_to_server(ip, port);

	if (sockfd != INVALID_SOCKET) {
		/* Send the data */
		if (wbd_socket_send_data(sockfd, req, strlen(req)+1) <= 0) {
			ret = -1;
			WBD_ERROR("%s: Failed to send\n", req);
			goto exit;
		}

		/* Get the response from the server */
		rcv_ret = wbd_socket_recv_data(sockfd, &read_buf);
		if ((rcv_ret <= 0) || (read_buf == NULL)) {
			ret = -1;
			WBD_ERROR("%s: Failed to recieve\n", req);
			goto exit;
		}

		g_wbd_output_buffer = read_buf;
	} else {
		WBD_INFO("%s invalid socket\n", __FUNCTION__);
	}
exit:
	wbd_close_socket(&sockfd);
	return ret;
}

/* Function to get master info data. */
static int
wbd_get_master_info_data()
{
	char *req = "{\"Cmd\":\"info\", \"SubCmd\":\"info\", \"MAC\":\"\", "
			"\"BSSID\":\"\", \"Band\":0, \"Flags\":2}";

	return wbd_send_and_receive_data(req, WBD_LOOPBACK_IP, EAPD_WKSP_WBD_TCP_MASTERCLI_PORT);
}

/* Get wlX_ or wlX.y_ Prefix from OS specific interface name */
static int
wbd_wl_get_prefix(char *ifname, char *prefix, int prefix_len)
{
	int ret = 0;
	char wl_name[IFNAMSIZ];

	/* Convert eth name to wl name - returns 0 if success */
	ret = osifname_to_nvifname(ifname, wl_name, sizeof(wl_name));

	/* Get prefix of the interface from Driver */
	make_wl_prefix(prefix, prefix_len, 1, wl_name);
	WBD_INFO("Interface: %s, wl_name: %s, Prefix: %s\n", ifname, wl_name, prefix);

	return ret;
}

/* Get wbd config setting. */
static void
wbd_get_config_data(char *req_name)
{
	int refresh_interval = WBD_MIN_REFRESH_RATE;
	int default_rssi_2g = WBD_DEFAULT_RSSI, default_rssi_5g = WBD_DEFAULT_RSSI;
	int band, ret, idle_rate, num;
	char *ptr = NULL, prefix[IFNAMSIZ] = {0};
	char wbd_ifnames[NVRAM_MAX_VALUE_LEN] = {0};
	char name[IFNAMSIZ] = {0}, var_intf[IFNAMSIZ] = {0}, *next_intf;
	char wbd_nvram_weak_sta_cfg[NVRAM_MAX_VALUE_LEN] = {0};

	/* Read actual ifnames */
	ret = wbd_read_actual_ifnames(wbd_ifnames, sizeof(wbd_ifnames), FALSE);

	/* Traverse wbd_ifnames */
	foreach(var_intf, wbd_ifnames, next_intf) {
		/* Copy interface name temporarily */
		strncpy_n(name, var_intf, sizeof(name));

#ifdef DSLCPE_ENDIAN
		 wl_endian_probe(name);
#endif

		/* Get Band from interface name */
		ret = wl_ioctl(name, WLC_GET_BAND, &band, sizeof(band));
#ifdef DSLCPE_ENDIAN
		band=etoh32(band);
#endif
		if (!ret) {
			/* Get prefix name */
			ret = wbd_wl_get_prefix(name, prefix, sizeof(prefix));
			if (!ret) {
				snprintf(wbd_nvram_weak_sta_cfg, sizeof(wbd_nvram_weak_sta_cfg),
					"%swbd_weak_sta_cfg", prefix);
				ptr = nvram_safe_get(wbd_nvram_weak_sta_cfg);

				/* Get default rssi from nvram */
				if (band == WLC_BAND_2G) {
					num = sscanf(ptr, "%d %d", &idle_rate, &default_rssi_2g);
				} else if (band == WLC_BAND_5G) {
					num = sscanf(ptr, "%d %d", &idle_rate, &default_rssi_5g);
				}

				if (num != 2) {
					WBD_INFO("idle rate and rssi is not defined\n");
				}
			}
		}
	}

	/* refresh rate */
	ptr = nvram_safe_get("wbd_refresh_interval");
	refresh_interval = (atoi(ptr) > 0) ? atoi(ptr) : WBD_MIN_REFRESH_RATE;

	/* default rssi */
	default_rssi_2g = (default_rssi_2g < 0) ? default_rssi_2g : WBD_DEFAULT_RSSI;
	default_rssi_5g = (default_rssi_5g < 0) ? default_rssi_5g : WBD_DEFAULT_RSSI;

	g_wbd_output_buffer = (char*)malloc(sizeof(char) * MAX_READ_BUFFER);
	snprintf(g_wbd_output_buffer, MAX_READ_BUFFER,
		"{\"Cmd\":\"%s\",\"RefreshInterval\":\"%d\",\"RSSI5G\":\"%d\", \"RSSI2G\":\"%d\"}",
		req_name, refresh_interval, default_rssi_5g, default_rssi_2g);
}

/* Function to get master logs data. */
static int
wbd_get_master_logs_data()
{

	char *req = "{\"Cmd\":\"logs\", \"SubCmd\":\"logs\", \"MAC\":\"\", "
			"\"BSSID\":\"\", \"Band\":0, \"Flags\":2}";

	return wbd_send_and_receive_data(req, WBD_LOOPBACK_IP, EAPD_WKSP_WBD_TCP_MASTERCLI_PORT);
}

/* Function to clear master logs data. */
static int
wbd_clear_master_logs_data()
{
	char *req = "{\"Cmd\":\"logs\", \"SubCmd\":\"logs\", \"MAC\":\"\", "
			"\"BSSID\":\"\", \"Band\":0, \"Flags\":4}";

	return wbd_send_and_receive_data(req, WBD_LOOPBACK_IP, EAPD_WKSP_WBD_TCP_MASTERCLI_PORT);
}

/* Read query from stream in json format */
void
do_wbd_post(const char *orig_url, FILE *stream, int len, const char *boundary)
{
	char wbd_url[WBD_URL_SIZE] = {0}, *pch = NULL, *saveptr = NULL;
#ifdef DSLCPE
	int idx = 0;
#else
	int idx;
#endif
	wbd_req_args_id_t id = WBD_UNKNOWN;

	if (orig_url == NULL) {
		return;
	}

	/* Copy url */
	strncpy_n(wbd_url, orig_url, sizeof(wbd_url));

	/* Parse url */
	pch = strtok_r(wbd_url, WBD_DELIM, &saveptr);
	while (pch != NULL) {
		for (idx = 0; idx < (sizeof(wbd_req_args_list)/sizeof(wbd_req_args_t)); idx++) {
			if (!strcmp(pch, wbd_req_args_list[idx].req_name)) {
				id = wbd_req_args_list[idx].id;
				break;
			}
		}
		pch = strtok_r(NULL, WBD_DELIM, &saveptr);
	}

	/* Based on request id do processing. */
	switch (id) {
		case WBD_MASTERINFO:
			(void)wbd_get_master_info_data();
		break;

		case WBD_CONFIG:
			wbd_get_config_data(wbd_req_args_list[idx].req_name);
		break;

		case WBD_MASTERLOGS:
			(void)wbd_get_master_logs_data();
		break;

		case WBD_CLEARLOGS:
			(void)wbd_clear_master_logs_data();
		break;

		case WBD_UNKNOWN:
			WBD_INFO("unknown\n");
		break;
	}
}

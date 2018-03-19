/*
 * Broadcom Home Gateway Reference Design
 * Broadcom iQoS Webpage functions
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
 * $Id: iqos_gui.h 458674 2014-02-27 11:23:19Z $
 */

#ifndef _IQOS_GUI_H_
#define _IQOS_GUI_H_

#include <confmtd_utils.h>

#define IQOS_NOOP	0x0
#define IQOS_START	0x1
#define IQOS_RESTART	0x2
#define IQOS_STOP	0x4

#define IQOS_DEBUG_ERROR		0x0001
#define IQOS_DEBUG_WARNING		0x0002
#define IQOS_DEBUG_INFO			0x0004

#define FILE_LOCK_RETRY		10

/* path */
#define CONFMTD_IQOS_DIR		RAMFS_CONFMTD_DIR"/iqos"
#define TREND_DIR			"/tmp/trend"
#define SRC_FILE_DIR			"/usr/sbin"

/* file */
#define QOS_CONF			"qos.conf"

#define PATH_QOS_CONF			TREND_DIR"/"QOS_CONF
#define PATH_QOS_CONF_TMP		TREND_DIR"/"QOS_CONF".tmp"
#define PATH_CONFMTD_QOS_CONF		CONFMTD_IQOS_DIR"/"QOS_CONF
#define PATH_SRC_FILE_QOS_CONF		SRC_FILE_DIR"/"QOS_CONF

typedef struct iqos_mac_priority {
	char macaddr[20];				/* string of MAC address */
	int prio;
} iqos_mac_priority_t;

extern void do_iqos_get(char *url, FILE *stream);
extern void do_iqos_post(const char *orig_url, FILE *stream, int len, const char *boundary);
extern int iqos_is_enabled(void);
extern int iqos_enable(int enable);
extern int iqos_is_wan_bw_auto(void);
extern int iqos_set_wan_bw_auto(int enable);
extern int iqos_get_wan_bw(int *upbw, int *downbw);
extern int iqos_set_wan_bw(int upbw, int downbw);
extern int iqos_get_apps_priority(int appcatid);
extern int iqos_set_apps_priority(int appcatid, int prio);
extern int iqos_get_devs_priority(int devcatid);
extern int iqos_set_devs_priority(int devcatid, int prio);
extern int iqos_list_mac_priority(iqos_mac_priority_t *buf, int size);
extern int iqos_get_mac_priority(char *macaddr);
extern int iqos_set_mac_priority(char *macaddr, int prio);
extern int iqos_is_default_conf(void);
extern int iqos_set_default_conf(int enable);
extern int iqos_file_backup(void);
#endif /* _IQOS_GUI_H_ */

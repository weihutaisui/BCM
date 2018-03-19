/*
 * Linux port of dhd command line utility, hacked from wl utility.
 *
 * Copyright 2017 Broadcom
 *
 * This program is the proprietary software of Broadcom and/or
 * its licensors, and may only be used, duplicated, modified or distributed
 * pursuant to the terms and conditions of a separate, written license
 * agreement executed between you and Broadcom (an "Authorized License").
 * Except as set forth in an Authorized License, Broadcom grants no license
 * (express or implied), right to use, or waiver of any kind with respect to
 * the Software, and Broadcom expressly reserves all rights in and to the
 * Software and all intellectual property rights therein.  IF YOU HAVE NO
 * AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY
 * WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF
 * THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1. This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof, and to
 * use this information only in connection with your use of Broadcom
 * integrated circuit products.
 *
 * 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
 * "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR
 * OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL,
 * SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR
 * IN ANY WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 * IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii)
 * ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF
 * OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY
 * NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
 *
 *
 * <<Broadcom-WL-IPTag/Proprietary:>>
 *
 * $Id: dhdu_common.h 514727 2014-11-12 03:02:48Z $
 */

/* Common header file for dhdu_linux.c and dhdu_ndis.c */

#ifndef _dhdu_common_h
#define _dhdu_common_h

#if !defined(RWL_WIFI) && !defined(RWL_DONGLE) && !defined(RWL_SOCKET) && \
	!defined(RWL_SERIAL)

#define NO_REMOTE       0
#define REMOTE_SERIAL 	1
#define REMOTE_SOCKET 	2
#define REMOTE_WIFI     3
#define REMOTE_DONGLE   4

/* For cross OS support */
#define LINUX_OS  	1
#define WIN32_OS  	2
#define MAC_OSX		3
#define BACKLOG 	4
#define WINVISTA_OS	5
#define INDONGLE	6

#define RWL_WIFI_ACTION_CMD   		"wifiaction"
#define RWL_WIFI_GET_ACTION_CMD         "rwlwifivsaction"
#define RWL_DONGLE_SET_CMD		"dongleset"

#define SUCCESS 	1
#define FAIL   		-1
#define NO_PACKET       -2

/* Added for debug utility support */
#define ERR		stderr
#define OUTPUT		stdout
#define DEBUG_ERR	0x0001
#define DEBUG_INFO	0x0002
#define DEBUG_DBG	0x0004

#define DPRINT_ERR	if (defined_debug & DEBUG_ERR) \
			    fprintf
#define DPRINT_INFO	if (defined_debug & DEBUG_INFO) \
			    fprintf
#define DPRINT_DBG	if (defined_debug & DEBUG_DBG) \
			    fprintf

extern int wl_get(void *wl, int cmd, void *buf, int len);
extern int wl_set(void *wl, int cmd, void *buf, int len);
#endif /* !RWL_WIFI & !RWL_DONGLE & !RWL_SOCKET & !RWL_SERIAL */

/* DHD utility function declarations */
extern int dhd_check(void *dhd);
extern int dhd_atoip(const char *a, struct ipv4_addr *n);
extern int dhd_option(char ***pargv, char **pifname, int *phelp);
void dhd_usage(cmd_t *port_cmds);

/* Remote DHD declarations */
int remote_type = NO_REMOTE;
extern char *g_rwl_buf_mac;
extern char* g_rwl_device_name_serial;
unsigned short g_rwl_servport;
char *g_rwl_servIP = NULL;
unsigned short defined_debug = DEBUG_ERR | DEBUG_INFO;

static int process_args(struct ifreq* ifr, char **argv);

#ifdef IL_BIGENDIAN
bool swap = FALSE;
#define htod32(i) (swap?bcmswap32(i):i)
#define dtoh32(i) (swap?bcmswap32(i):i)
#define dtohenum(i) ((sizeof(i) == 4) ? dtoh32(i) : ((sizeof(i) == 2) ? htod16(i) : i))
#else /* IL_BIGENDIAN */
#define dtoh32(i) i
#define dtoh16(i) i
#endif	/* IL_BIGENDIAN */

#endif  /* _dhdu_common_h_ */

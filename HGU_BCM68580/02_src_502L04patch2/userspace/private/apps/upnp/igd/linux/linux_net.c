/*
 *
 * Copyright (c) 2003-2012  Broadcom Corporation
 * All Rights Reserved
 *
 * <:label-BRCM:2012:proprietary:standard
 * 
 *  This program is the proprietary software of Broadcom and/or its
 *  licensors, and may only be used, duplicated, modified or distributed pursuant
 *  to the terms and conditions of a separate, written license agreement executed
 *  between you and Broadcom (an "Authorized License").  Except as set forth in
 *  an Authorized License, Broadcom grants no license (express or implied), right
 *  to use, or waiver of any kind with respect to the Software, and Broadcom
 *  expressly reserves all rights in and to the Software and all intellectual
 *  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 *  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 *  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 * 
 *  Except as expressly set forth in the Authorized License,
 * 
 *  1. This program, including its structure, sequence and organization,
 *     constitutes the valuable trade secrets of Broadcom, and you shall use
 *     all reasonable efforts to protect the confidentiality thereof, and to
 *     use this information only in connection with your use of Broadcom
 *     integrated circuit products.
 * 
 *  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
 *     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
 *     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
 *     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
 *     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
 *     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
 *     PERFORMANCE OF THE SOFTWARE.
 * 
 *  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
 *     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
 *     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
 *     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 *     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
 *     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
 *     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
 *     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
 *     LIMITED REMEDY.
 * :>
 *
 * $Id: linux_net.c,v 1.1.48.2 2003/10/31 21:31:36 mthawani Exp $
 */

#include <sys/ioctl.h>
#include <net/if.h>
//#include <linux/sockios.h>
#include <linux/socket.h>

#include "upnp_osl.h"
#include "upnp.h"
#include "../igd/igd.h"

char *get_name(char *name, char *p)
{
    while (isspace(*p))
	p++;
    while (*p) {
	if (isspace(*p))
	    break;
	if (*p == ':') {	/* could be an alias */
	    char *dot = p, *dotname = name;
	    *name++ = *p++;
	    while (isdigit(*p))
		*name++ = *p++;
	    if (*p != ':') {	/* it wasn't, backup */
		p = dot;
		name = dotname;
	    }
	    if (*p == '\0')
		return NULL;
	    p++;
	    break;
	}
	*name++ = *p++;
    }
    *name++ = '\0';
    return p;
}

int get_dev_fields(char *bp, int versioninfo, if_stats_t *pstats )
{
    switch (versioninfo) {
    case 3:
	sscanf(bp,
	"%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld",
	       &pstats->rx_bytes,
	       &pstats->rx_packets,
	       &pstats->rx_errors,
	       &pstats->rx_dropped,
	       &pstats->rx_fifo_errors,
	       &pstats->rx_frame_errors,
	       &pstats->rx_compressed,
	       &pstats->rx_multicast,

	       &pstats->tx_bytes,
	       &pstats->tx_packets,
	       &pstats->tx_errors,
	       &pstats->tx_dropped,
	       &pstats->tx_fifo_errors,
	       &pstats->collisions,
	       &pstats->tx_carrier_errors,
	       &pstats->tx_compressed);
	break;
    case 2:
	sscanf(bp, "%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld",
	       &pstats->rx_bytes,
	       &pstats->rx_packets,
	       &pstats->rx_errors,
	       &pstats->rx_dropped,
	       &pstats->rx_fifo_errors,
	       &pstats->rx_frame_errors,

	       &pstats->tx_bytes,
	       &pstats->tx_packets,
	       &pstats->tx_errors,
	       &pstats->tx_dropped,
	       &pstats->tx_fifo_errors,
	       &pstats->collisions,
	       &pstats->tx_carrier_errors);
	pstats->rx_multicast = 0;
	break;
    case 1:
	sscanf(bp, "%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld",
	       &pstats->rx_packets,
	       &pstats->rx_errors,
	       &pstats->rx_dropped,
	       &pstats->rx_fifo_errors,
	       &pstats->rx_frame_errors,

	       &pstats->tx_packets,
	       &pstats->tx_errors,
	       &pstats->tx_dropped,
	       &pstats->tx_fifo_errors,
	       &pstats->collisions,
	       &pstats->tx_carrier_errors);
	pstats->rx_bytes = 0;
	pstats->tx_bytes = 0;
	pstats->rx_multicast = 0;
	break;
    }
    return 0;
}

int procnetdev_version(char *buf)
{
    if (strstr(buf, "compressed"))
	return 3;
    if (strstr(buf, "bytes"))
	return 2;
    return 1;
}

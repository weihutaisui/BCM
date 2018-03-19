/***********************************************************************
 *
 *  Copyright (c) 2004-2010  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2012:proprietary:standard

 This program is the proprietary software of Broadcom and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").  Except as set forth in
 an Authorized License, Broadcom grants no license (express or implied), right
 to use, or waiver of any kind with respect to the Software, and Broadcom
 expressly reserves all rights in and to the Software and all intellectual
 property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,

 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use
    all reasonable efforts to protect the confidentiality thereof, and to
    use this information only in connection with your use of Broadcom
    integrated circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
    PERFORMANCE OF THE SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
    LIMITED REMEDY.
:>
 *
************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "ethctl.h"

void br_dump_bridge_id(const unsigned char *x)
{
	printf("%.2x%.2x.%.2x%.2x%.2x%.2x%.2x%.2x", x[0], x[1], x[2], x[3],
	       x[4], x[5], x[6], x[7]);
}

void br_show_timer(const struct timeval *tv)
{
	printf("%4i.%.2i", (int)tv->tv_sec, (int)tv->tv_usec/10000);
}

static int first;

static int dump_interface(const char *b, const char *p, void *arg)
{

	if (first) 
		first = 0;
	else
		printf("\n\t\t\t\t\t\t\t");

	printf("%s", p);

	return 0;
}

void br_dump_interface_list(const char *br)
{
	int err;

	first = 1;
	err = br_foreach_port(br, dump_interface, NULL);
	if (err < 0)
		printf(" can't get port info: %s\n", strerror(-err));
	else
		printf("\n");
}

static int dump_port_info(const char *br, const char *p,  void *arg)
{
	struct port_info pinfo;

	if (br_get_port_info(br, p, &pinfo)) {
		printf("Can't get info for %p",p);
		return 1;
	}

	printf("%s (%d)\n", p, pinfo.port_no);
	printf(" port id\t\t%.4x",  pinfo.port_id);
	printf("\t\t\tstate\t\t%15s\n", br_get_state_name(pinfo.state));
	printf(" designated root\t");
	br_dump_bridge_id((unsigned char *)&pinfo.designated_root);
	printf("\tpath cost\t\t%4i\n", pinfo.path_cost);

	printf(" designated bridge\t");
	br_dump_bridge_id((unsigned char *)&pinfo.designated_bridge);
	printf("\tmessage age timer\t");
	br_show_timer(&pinfo.message_age_timer_value);
	printf("\n designated port\t%.4x", pinfo.designated_port);
	printf("\t\t\tforward delay timer\t");
	br_show_timer(&pinfo.forward_delay_timer_value);
	printf("\n designated cost\t%4i", pinfo.designated_cost);
	printf("\t\t\thold timer\t\t");
	br_show_timer(&pinfo.hold_timer_value);
	printf("\n flags\t\t\t");
	if (pinfo.config_pending)
		printf("CONFIG_PENDING ");
	if (pinfo.top_change_ack)
		printf("TOPOLOGY_CHANGE_ACK ");
	printf("\n");
	printf("\n");
	return 0;
}

void br_dump_info(const char *br, const struct bridge_info *bri)
{
	int err;

	printf("%s\n", br);
	printf(" bridge id\t\t");
	br_dump_bridge_id((unsigned char *)&bri->bridge_id);
	printf("\n designated root\t");
	br_dump_bridge_id((unsigned char *)&bri->designated_root);
	printf("\n root port\t\t%4i\t\t\t", bri->root_port);
	printf("path cost\t\t%4i\n", bri->root_path_cost);
	printf(" max age\t\t");
	br_show_timer(&bri->max_age);
	printf("\t\t\tbridge max age\t\t");
	br_show_timer(&bri->bridge_max_age);
	printf("\n hello time\t\t");
	br_show_timer(&bri->hello_time);
	printf("\t\t\tbridge hello time\t");
	br_show_timer(&bri->bridge_hello_time);
	printf("\n forward delay\t\t");
	br_show_timer(&bri->forward_delay);
	printf("\t\t\tbridge forward delay\t");
	br_show_timer(&bri->bridge_forward_delay);
	printf("\n ageing time\t\t");
	br_show_timer(&bri->ageing_time);
	printf("\n hello timer\t\t");
	br_show_timer(&bri->hello_timer_value);
	printf("\t\t\ttcn timer\t\t");
	br_show_timer(&bri->tcn_timer_value);
	printf("\n topology change timer\t");
	br_show_timer(&bri->topology_change_timer_value);
	printf("\t\t\tgc timer\t\t");
	br_show_timer(&bri->gc_timer_value);
	printf("\n flags\t\t\t");
	if (bri->topology_change)
		printf("TOPOLOGY_CHANGE ");
	if (bri->topology_change_detected)
		printf("TOPOLOGY_CHANGE_DETECTED ");
	printf("\n");
	printf("\n");
	printf("\n");

	err = br_foreach_port(br, dump_port_info, NULL);
	if (err < 0)
		printf("can't get ports: %s\n", strerror(-err));
}

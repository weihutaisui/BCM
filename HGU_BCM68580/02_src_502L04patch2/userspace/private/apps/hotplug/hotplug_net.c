/***********************************************************************
 *
 *  Copyright (c) 2007  Broadcom Corporation
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
#include "hotplug.h"
#ifdef  BRCM_WLAN
#include "wlcsm_lib_api.h"
#endif

#define IFC_BRIDGE_NAME		 "br0"


int
hotplug_net(void)
{
	char *interface, *action;
    	char cmd[80];

	if (!(interface = getenv("INTERFACE")) ||
	    !(action = getenv("ACTION")))
		return EINVAL;

#ifdef  BRCM_WLAN
	if (!strncmp(interface, "wl", 2) || 
		!strncmp(interface, "radiotap", 8)||
		!strncmp(interface, "wds", 3)) 
		wlcsm_mngr_nethotplug_notify(interface,action);
#endif

	/* Bridge WDS interfaces */
	if (!strncmp(interface, "wds", 3)) {
		if (!strcmp(action, "register")||!strcmp(action, "add")) {
						
			/* add interface to bridge */
       			sprintf(cmd, "ifconfig %s up", interface);
	   		bcmSystem(cmd);
       			sprintf(cmd, "brctl addif %s %s", IFC_BRIDGE_NAME, interface);
       			bcmSystem(cmd);

			/* enable spanning tree protocol */
       			sprintf(cmd, "brctl stp %s on", IFC_BRIDGE_NAME);
       			bcmSystem(cmd);	

   			/* send ARP packet with bridge IP and hardware address to device
   			 * this piece of code is -required- to make br0's mac work properly
   			 */
   			sprintf(cmd, "sendarp -s %s -d %s", IFC_BRIDGE_NAME, IFC_BRIDGE_NAME);
   			bcmSystem(cmd);
               
       			/* Notify NAS of adding the interface */
			notify_nas("lan", interface, "up");
		
		       	return 0;
		}
	}

	return 0;
}

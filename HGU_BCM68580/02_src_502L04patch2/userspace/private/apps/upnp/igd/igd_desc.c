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
 * $Id: igd_desc.c,v 1.10.20.2 2003/10/31 21:31:35 mthawani Exp $
 */

#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"


#undef INCLUDE_ETHERLINK
#undef INCLUDE_CABLELINKCONFIG
#undef INCLUDE_LANDEVICE
#undef INCLUDE_LANHOSTCONFIG

#define INCLUDE_LAYER3
#define INCLUDE_PPPCONNECTION
#define INCLUDE_IPCONNECTION
#define INCLUDE_DSLLINKCONFIG



extern int WANDevice_Init(PDevice pdev, device_state_t state, va_list ap);
extern int LANDevice_Init(PDevice pdev, device_state_t state, va_list ap);
extern int IGDevice_Init(PDevice igdev, device_state_t state, va_list ap);


/* Global structure for storing the state table for this device */

extern ServiceTemplate Template_Layer3Forwarding;
extern ServiceTemplate Template_WANCommonInterfaceConfig;
extern ServiceTemplate Template_WANCableLinkConfig;
extern ServiceTemplate Template_WANEthernetLinkConfig;
extern ServiceTemplate Template_WANDSLLinkConfig;
extern ServiceTemplate Template_WANPPPConnection;
extern ServiceTemplate Template_WANIPConnection;

extern ServiceTemplate Template_LANHostConfigManagement;

PServiceTemplate svcs_igd[]           = { 
#if defined(INCLUDE_OSINFO)
    &Template_OSInfo,
#endif
#ifdef INCLUDE_LAYER3
    &Template_Layer3Forwarding
#endif
};

PServiceTemplate svcs_wandevice[]     = { 
     &Template_WANCommonInterfaceConfig 
};

PServiceTemplate svcs_wanconnection[] = { 
#if defined(INCLUDE_ETHERLINK)
     &Template_WANEthernetLinkConfig, 
#endif
#if defined(INCLUDE_PPPCONNECTION)
     &Template_WANPPPConnection,
#endif
#if defined(INCLUDE_IPCONNECTION)
     &Template_WANIPConnection,
#endif
#if defined(INCLUDE_CABLELINKCONFIG)
     &Template_WANCableLinkConfig,
#endif
#if defined(INCLUDE_CABLELINKCONFIG)
     &Template_WANDSLLinkConfig,
#endif
};

PServiceTemplate svcs_landevice[] = { 
#if defined(INCLUDE_LANHOSTCONFIG)
     &Template_LANHostConfigManagement,
#endif
};

DeviceTemplate subdevs_wandevice[] = { 
    {
	"urn:schemas-upnp-org:device:WANConnectionDevice:1",
	"WANCONNECTION",
	NULL, /* PFDEVINIT */
	NULL, /* PFDEVXML */
	ARRAYSIZE(svcs_wanconnection), svcs_wanconnection 
    }
};

DeviceTemplate LANDeviceTemplate = {
	"urn:schemas-upnp-org:device:LANDevice:1",
	"LANDEVICEUDN",
	LANDevice_Init, 	/* PFDEVINIT */
	NULL, /* PFDEVXML */
	ARRAYSIZE(svcs_landevice), svcs_landevice 
};

DeviceTemplate WANDeviceTemplate = {
    "urn:schemas-upnp-org:device:WANDevice:1",
    "WANDEVICEUDN",
    WANDevice_Init, 	/* PFDEVINIT */
    NULL, 		/* PFDEVXML */
    ARRAYSIZE(svcs_wandevice), svcs_wandevice/*, 
    ARRAYSIZE(subdevs_wandevice), subdevs_wandevice*/
};
    
extern void igd_xml(PDevice pdev, UFILE *up);

DeviceTemplate IGDeviceTemplate = {
    "urn:schemas-upnp-org:device:InternetGatewayDevice:1",
    "ROOTUDN",
    IGDevice_Init,  	/* PFDEVINIT */
    NULL,  		/* PFDEVXML */
    ARRAYSIZE(svcs_igd), svcs_igd
};






/*
 *
 * <:copyright-BRCM:2012:proprietary:standard 
 * 
 *    Copyright (c) 2012 Broadcom 
 *    All Rights Reserved
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
 */
 
#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "igd.h"

extern int IGDevice_Init(PDevice igdev, device_state_t state, va_list ap);
extern void igd_xml(PDevice pdev, UFILE *up);
extern ServiceTemplate Template_Layer3Forwarding;
extern ServiceTemplate Template_WANCommonInterfaceConfig;
extern ServiceTemplate Template_WANCableLinkConfig;
extern ServiceTemplate Template_WANETHLinkConfig;
extern ServiceTemplate Template_WANDSLLinkConfig;
extern ServiceTemplate Template_WANPPPConnection;
extern ServiceTemplate Template_WANIPConnection;
extern ServiceTemplate Template_LANHostConfigManagement;
extern ServiceTemplate Template_LANEthernetInterfaceConfig;
extern ServiceTemplate Template_WANDSLConnMgt;
extern ServiceTemplate Template_IPPingConfig;
#ifdef USB
extern ServiceTemplate Template_LANUSBInterfaceConfig;
#endif
#ifdef WIRELESS
extern ServiceTemplate Template_WLANConfig;
#endif
extern ServiceTemplate Template_DeviceInfo;
extern ServiceTemplate Template_LANHosts;
extern ServiceTemplate Template_MgtServer;
extern ServiceTemplate Template_WANDSLInterfaceConfig;
extern ServiceTemplate Template_DeviceConfig;
extern ServiceTemplate Template_LANConfigSecurity ;
extern ServiceTemplate Template_TimeServer;
extern ServiceTemplate Template_Layer2Bridging;
extern ServiceTemplate Template_QueueManagement;
/** All devices are supported: InternetGatewayDevice, LANDevice and WANDevice (and sub-devices).
 *  Services are selected in make Menuconfig:
 *     IGD's services:
 *        Layer3Forwarding, DeviceInfo, DeviceConfig, LANConfigSecurity, ManagementServer, Time,
 *        UserInterface.
 *     LANDevice's services:
 *        LANHostConfigMgmt, LANEthernetInterfaceConfig, WLANConfiguration, LANUSBInterfaceConfig,
 *        Hosts.
 *     WANDevice's services:
 *        WANCommonInterfaceConfig, WANDSLInterfaceConfig, WANEthernetInterfaceConfig,
 *        WANDSLConnectionManagement, WANDSLDiagnostics.
 *        WANConnectionDevice's serives:
 *           WANPOTSLinkConfig, WANDSLLinkConfig, WANCableLinkConfig, WANEthernetLinkConfig,
 *           WANIPConnection, WANPPPConnection.
 */

/** IGD's services
 */

PServiceTemplate svcs_igd[] = { 
#ifdef INCLUDE_LAYER3
    &Template_Layer3Forwarding,
#endif
#ifdef INCLUDE_DEVICEINFO 
    &Template_DeviceInfo,
#endif
#ifdef INCLUDE_DEVICECONFIG
    &Template_DeviceConfig,
#endif
#ifdef INCLUDE_LANCONFIGSECURITY
    &Template_LANConfigSecurity,
#endif
#ifdef INCLUDE_MANAGEMENTSERVER 
    &Template_MgtServer,
#endif
#ifdef INCLUDE_TIME
    &Template_TimeServer,
#endif
#ifdef INCLUDE_USERINTERFACE
#endif
#ifdef INCLUDE_QUEUEMANAGEMENT
    &Template_QueueManagement,
#endif
#ifdef INCLUDE_LAYER2BRIDGE
    &Template_Layer2Bridging,
#endif
#ifdef INCLUDE_IPPINGDIAG
    &Template_IPPingConfig,
#endif
};


/* WAN device template */
PServiceTemplate svcs_wandevice[] = { 
};

/** WAN's services
 */
PServiceTemplate svcs_wanconnection[] = { 

};

/** LAN's services
 */
PServiceTemplate svcs_landevice[] = { 

};

/** WAN's sub devices
 */
DeviceTemplate subdevs_wandevice[] = { 
   {
      "urn:dslforum-org:device:WANConnectionDevice:",
      "WANCONNECTION",
      NULL, /* PFDEVINIT */
      NULL, /* PFDEVXML */
      ARRAYSIZE(svcs_wanconnection), 
      svcs_wanconnection,
      0,
      NULL,
      NULL
   }
};

DeviceTemplate IGDeviceTemplate = {
   "urn:dslforum-org:device:InternetGatewayDevice:1",
   "ROOTUDN",
   IGDevice_Init,      /* PFDEVINIT */
   NULL,          /* PFDEVXML */
   ARRAYSIZE(svcs_igd), 
   svcs_igd,
   0,
   NULL,
   NULL
};

DeviceTemplate LANDeviceTemplate = {
    "urn:dslforum-org:device:LANDevice:1",
    "LANDEVICEUDN",
    LANDevice_Init,     /* PFDEVINIT */
    NULL, /* PFDEVXML */
    ARRAYSIZE(svcs_landevice), 
    svcs_landevice,
    0,
    NULL,
    NULL
};

DeviceTemplate WANDeviceTemplate = {
   "urn:dslforum-org:device:WANDevice:",
   "WANDEVICEUDN",
   NULL,     /* PFDEVINIT */
   NULL,         /* PFDEVXML */
   ARRAYSIZE(svcs_wandevice), 
   svcs_wandevice, 
   0,
   NULL,
   NULL
};


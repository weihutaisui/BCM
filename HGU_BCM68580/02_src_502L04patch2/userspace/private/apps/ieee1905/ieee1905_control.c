/***********************************************************************
 *
 *  Copyright (c) 2013  Broadcom Corporation
 *  All Rights Reserved
 *
 * <:label-BRCM:2013:proprietary:standard
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
 * $Change: 160447 $
 ***********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if_arp.h>
#include <linux/if_ether.h>
#include <sys/select.h>
#include <errno.h>
#include <fcntl.h>
#include "i5api.h"
#include "ieee1905_glue.h"
#include "ieee1905_socket.h"
#include "ieee1905_datamodel.h"
#include "ieee1905_trace.h"
#include "ieee1905_plc.h"
#include "ieee1905_wlcfg.h"
#include "ieee1905_flowmanager.h"
#include "ieee1905_json.h"
#include "i5ctl_wlcfg.h"
#include "ieee1905_security.h"
#include "ieee1905_udpsocket.h"
#include "ieee1905_message.h"
#include "i5ctl.h"
#include "i5api.h"
#include "ieee1905_wlmetric.h"
#include "ieee1905_cmsmdm.h"


#ifndef FD_COPY
#define FD_COPY(src,dest) memcpy((dest),(src),sizeof(dest))
#endif

#define I5_TRACE_MODULE i5TraceControl

static void _i5ControlUnknownMessage(i5_socket_type *psock, t_I5_API_CMD_NAME cmd) 
{
    i5apiSendMessage(psock->sd, cmd, 0, 0);
}

static void _i5ControlSetCmdRespSendMsgType(t_I5_CTL_CMDRESP_SEND_MSG_TYPE *response)
{
  memcpy(response->srcMacAddr, i5_config.i5_mac_address, MAC_ADDR_LEN);
  response->messageId = i5_config.last_message_identifier;
}

static void i5ControlDataModel(i5_socket_type *psock, int cmd)
{
  char *pMsgBuf;
  int   length;

  /* get size of DM and allocate memory */
  length  = i5DmCtlSizeGet();
  pMsgBuf = malloc(length);
  if ( pMsgBuf != NULL ) {
    /* send message back to requesting socket */
    i5DmLinkMetricsActivate();
    i5DmCtlRetrieve(pMsgBuf);
    i5apiSendMessage(psock->sd, cmd, pMsgBuf, length);
    free(pMsgBuf);
  }
}

static void i5ControlAlMacAddress(i5_socket_type *psock, int cmd)
{
  /* get size of DM and allocate memory */
  char *pMsgBuf = malloc(MAC_ADDR_LEN);
  if ( pMsgBuf != NULL ) {
    /* send message back to requesting socket */
    i5DmCtlAlMacRetrieve(pMsgBuf);
    i5apiSendMessage(psock->sd, cmd, pMsgBuf, MAC_ADDR_LEN);
    free(pMsgBuf);
  }
}

#if defined(SUPPORT_HOMEPLUG)
static void i5ControlPlcHandler(t_I5_API_PLC_MSG *pMsg)
{
  if ( I5_API_PLC_UKE_START == pMsg->subcmd ) {
    i5PlcUKEStart();
  }
  else if ( I5_API_PLC_UKE_RANDOMIZE == pMsg->subcmd ) {
    i5PlcUKERandomize();
  }
  else {
    printf("Unknown PLC command %d\n", pMsg->subcmd);
  }
}
#endif

static void i5ControlJsonLegHandler(t_I5_API_JSON_LEG_MSG *pMsg)
{
  if (( I5_API_JSON_LEG_OFF == pMsg->subcmd ) || ( I5_API_JSON_LEG_ON == pMsg->subcmd )) {
    i5JsonConfigLegacyDisplay(I5_JSON_ALL_CLIENTS, pMsg->subcmd);
  }
  else {
    printf("Unknown JSON legacy command %d\n", pMsg->subcmd);
  }
}

#if defined(DMP_DEVICE2_IEEE1905BASELINE_1)
static void i5ControlFetchLinkMetrics (int sd, unsigned char *deviceId, unsigned char *remoteInterfaceId)
{
   const char blank[6] = {0};
   i5_dm_device_type* device = NULL;
   t_I5_API_CONFIG_GET_LINK_METRICS_REPLY rspData = {0};

   i5TraceInfo("Dev " I5_MAC_DELIM_FMT " RemIf " I5_MAC_DELIM_FMT " \n",
               I5_MAC_PRM(deviceId), I5_MAC_PRM(remoteInterfaceId) );
   
   if (memcmp(blank, deviceId, 6) == 0) {
      device = i5DmDeviceFind(i5_config.i5_mac_address);
   }
   else {
      device = i5DmDeviceFind(deviceId);
   }
   if (device != NULL) {
      i5_dm_1905_neighbor_type *neighbor = i5Dm1905FindNeighborByRemoteInterface(device, remoteInterfaceId);
      if (NULL != neighbor) {
         rspData.linkAvailability = neighbor->availableThroughputCapacity;
         rspData.MacThroughputCapacity = neighbor->MacThroughputCapacity;
         rspData.packetErrors = 0;
         rspData.packetErrorsReceived = 0;
         rspData.transmittedPackets = 0;
         rspData.packetsReceived = 0;
         rspData.phyRate = 0;
         rspData.rssi = 0;
      }
   }
   i5apiSendMessage(sd, I5_API_CMD_GET_CONFIG, &rspData, sizeof(rspData));       
}
#endif

static void i5ControlLinkMetricIntervalHandler(t_I5_API_LINK_METRIC_INTERVAL *pMsg)
{
  i5DmSetLinkMetricInterval (pMsg->intervalMsec);
}

static void i5ControlSendBytesCommand (int length, t_I5_API_GOLDEN_NODE_SEND_MSG_BYTES *pmsg)
{
  unsigned int msgSize = length - sizeof (t_I5_API_MSG) - MAC_ADDR_LEN;
  i5MessageRawMessageSend(pmsg->macAddr, pmsg->message, msgSize);
}

static void i5ControlSendCommand(i5_socket_type *psock, t_I5_API_GOLDEN_NODE_SEND_MSG *pMsg, int cmd)
{
  t_I5_CTL_CMDRESP_SEND_MSG_TYPE response = {};

  switch (pMsg->messageId) {
    /* Type = 0 */
    case i5MessageTopologyDiscoveryValue:
      {
        i5_socket_type *bridgeSocket = i5SocketFindDevSocketByType(i5_socket_type_bridge_ll);
        if (NULL != bridgeSocket) {
          i5MessageTopologyDiscoverySend(bridgeSocket);
          _i5ControlSetCmdRespSendMsgType(&response);
        }
        break;
      }
    /* Type = 1 */
    case i5MessageTopologyNotificationValue:
      i5MessageTopologyNotificationSend();
      _i5ControlSetCmdRespSendMsgType(&response);
      break;
    /* Type = 2 */
    case i5MessageTopologyQueryValue:
      {
        i5_socket_type *bridgeSocket = i5SocketFindDevSocketByType(i5_socket_type_bridge_ll);
        if (NULL != bridgeSocket) {
          i5MessageRawTopologyQuerySend (bridgeSocket, pMsg->macAddr, 0 /* without retries */, i5MessageTopologyQueryValue);
          _i5ControlSetCmdRespSendMsgType(&response);
        }
        break;
      }
    case i5MessageTopologyResponseValue:
      i5TraceInfo("Unhandled i5MessageTopologyResponseValue\n");
      break;
    /* Type = 4 */
    case i5MessageVendorSpecificValue:
      /* I can do this */
      /* Hard code something something */
      i5TraceError("i5MessageVendorSpecificValue : TBD - spec has multiple results\n");
      break;      
    /* Type = 5 */
    case i5MessageLinkMetricQueryValue:
      {
        i5_socket_type *bridgeSocket = i5SocketFindDevSocketByType(i5_socket_type_bridge_ll);
        if (NULL != bridgeSocket) {
          /* TBD - this message is not well defined in the document */
          i5MessageLinkMetricQuerySend(bridgeSocket, pMsg->macAddr, 0x01, i5_config.i5_mac_address);
          _i5ControlSetCmdRespSendMsgType(&response);
        }
        break;
      }
    case i5MessageLinkMetricResponseValue:
      i5TraceInfo("Unhandled i5MessageLinkMetricResponseValue\n");
      break;
    case i5MessageApAutoconfigurationSearchValue:
      i5TraceInfo("Unhandled i5MessageApAutoconfigurationSearchValue\n");
      break;
    case i5MessageApAutoconfigurationResponseValue:
      i5TraceInfo("Unhandled i5MessageApAutoconfigurationResponseValue\n");
      break;
    case i5MessageApAutoconfigurationWscValue:
      i5TraceInfo("Unhandled i5MessageApAutoconfigurationWscValue\n");
      break;
    case i5MessageApAutoconfigurationRenewValue:
      i5TraceInfo("Unhandled i5MessageApAutoconfigurationRenewValue\n");
      break;
    case i5MessagePushButtonEventNotificationValue:
      i5TraceInfo("Unhandled i5MessagePushButtonEventNotificationValue\n");
      break;
    case i5MessagePushButtonJoinNotificationValue:
      i5TraceInfo("Unhandled i5MessagePushButtonJoinNotificationValue\n");
      break;
    default:
      i5TraceInfo("Unhandled Message Type %d\n", pMsg->messageId);
      break;
  }

  i5apiSendMessage(psock->sd, cmd, &response, sizeof(response) );  
}

#if defined(WIRELESS)
static void i5ControlSetWiFiPassSsid ( t_I5_API_PASSWORD_SSID_MSG* msg )
{
  char cmdStr[256] = "";
  int commitFlag = 0;

  i5Trace("ssid:(%s) password:(%s)\n", msg->ssid, msg->password);

  if (msg->ssid[0] != '\0') {
    if (strpbrk((char *)msg->ssid, "?\"$[\\]+") == NULL) {
      snprintf(cmdStr, sizeof(cmdStr), "nvram set wl0_ssid=\"%s\"",msg->ssid);
      i5Trace("%s",cmdStr);
      system(cmdStr);
      commitFlag = 1;
    }
  }
  if (msg->password[0] != '\0') {
    if (strpbrk((char *)msg->password, "?\"$[\\]+") == NULL) {
      snprintf(cmdStr, sizeof(cmdStr), "nvram set wl0_wpa_psk=\"%s\"",msg->password);
      i5Trace("%s",cmdStr);
      system(cmdStr);
      commitFlag = 1;
    }
  }

#if defined(SUPPORT_IEEE1905_GOLDENNODE) && defined(SUPPORT_IEEE1905_REGISTRAR)
  snprintf(cmdStr, sizeof(cmdStr), "nvram set wl0_akm=\"psk2\"");
  i5Trace("%s",cmdStr);
  system(cmdStr);

  snprintf(cmdStr, sizeof(cmdStr), "nvram set wl0_crypto=\"aes\"");
  i5Trace("%s",cmdStr);
  system(cmdStr);

  i5Trace("nvram commit");
#endif


  if (commitFlag) {
    i5Trace("nvram commit");
    system("nvram commit");
  }
}
#endif

#if defined(SUPPORT_HOMEPLUG)
static void i5ControlSetPlcPass (i5_socket_type *psock, t_I5_API_PASSWORD_SSID_MSG* msg, int cmd)
{
  char replyData = i5PlcSetPasswordNmk(msg->password);

  i5Trace("\n");  
  if (0 != replyData) {
    i5TraceError("Internal error while setting NMK\n");    
  }
}
#endif

static void i5ControlSetConfigHandler(void *pMsg, int length)
{
  t_I5_API_CONFIG_BASE *pCfg = (t_I5_API_CONFIG_BASE *)pMsg;
  i5TraceInfo("Subcommand %d\n", pCfg->subcmd);
  switch ( pCfg->subcmd ) {
    case I5_API_CONFIG_BASE:
    {
      i5_dm_device_type *pdevice = i5DmDeviceFind(i5_config.i5_mac_address);
 
      if ( length < (sizeof(t_I5_API_MSG) + sizeof(t_I5_API_CONFIG_BASE)) ) {
        printf("Invalid length for I5_API_CMD_SET_CONFIG\n");
        break;
      }
 
      i5Trace("I5_API_CMD_SET_CONFIG - BASE: %d %s %d - bandEn %d, %d\n", 
              pCfg->isEnabled,
              pCfg->deviceFriendlyName, 
              pCfg->isRegistrar,
              pCfg->apFreqBand24En,
              pCfg->apFreqBand5En);
 
      i5_config.running = pCfg->isEnabled;
      if ( i5_config.running && pdevice )
      {
 #if defined(WIRELESS)
        int wlConfChange = 0;
 
        if ( i5_config.isRegistrar != pCfg->isRegistrar )
        {
          /* change to enrollee - clear configure setting */
          if (0 == pCfg->isRegistrar) 
          {
            i5_dm_interface_type *pinterface = pdevice->interface_list.ll.next;
            while ( pinterface )
            {
              if ( i5DmIsInterfaceWireless(pinterface->MediaType) )
              {
                pinterface->isConfigured = 0;
              }
              pinterface = pinterface->ll.next;
            }
          }
          wlConfChange = 1;
        }
        else if ( (i5_config.freqBandEnable[i5MessageFreqBand_802_11_2_4Ghz] != pCfg->apFreqBand24En) ||
                  (i5_config.freqBandEnable[i5MessageFreqBand_802_11_5Ghz] != pCfg->apFreqBand5En) )
        {
          wlConfChange = 1;
        }
 
        i5_config.isRegistrar = pCfg->isRegistrar;
        i5_config.freqBandEnable[i5MessageFreqBand_802_11_2_4Ghz] = pCfg->apFreqBand24En;
        i5_config.freqBandEnable[i5MessageFreqBand_802_11_5Ghz] = pCfg->apFreqBand5En;
 
        if ( wlConfChange )
        {
          i5WlcfgApAutoconfigurationStop(NULL);
          i5WlcfgApAutoconfigurationStart(NULL);
        }
 #endif
        /* do not assume that setup->deviceFriendlyName is NULL terminated */
        strncpy(i5_config.friendlyName, pCfg->deviceFriendlyName, I5_DEVICE_FRIENDLY_NAME_LEN-1);
        i5_config.friendlyName[I5_DEVICE_FRIENDLY_NAME_LEN-1] = '\0';
        strncpy(pdevice->friendlyName, pCfg->deviceFriendlyName, I5_DEVICE_FRIENDLY_NAME_LEN-1);
        pdevice->friendlyName[I5_DEVICE_FRIENDLY_NAME_LEN-1] = '\0';
      }
      break;
    }
 #if defined(DMP_DEVICE2_IEEE1905BASELINE_1)
    case I5_API_CONFIG_SET_NETWORK_TOPOLOGY:
    {
      t_I5_API_CONFIG_SET_NETWORK_TOPOLOGY *pCfg = (t_I5_API_CONFIG_SET_NETWORK_TOPOLOGY *)pMsg;
      if ( length < (sizeof(t_I5_API_MSG) + sizeof(t_I5_API_CONFIG_SET_NETWORK_TOPOLOGY)) ) {
        printf("Invalid length for I5_API_CMD_SET_CONFIG\n");
        break;
      }
 
      i5Trace("I5_API_CMD_SET_CONFIG - NETWORK TOPOLOGY: %d\n", pCfg->isEnabled);
      if (i5_config.networkTopEnabled != pCfg->isEnabled) {
        i5_config.networkTopEnabled = pCfg->isEnabled;
        i5CmsMdmProcessNetworkTopologyConfigChange();
      }
      break;
    }
 #endif
 
    default:
      break;
  }
}

static void i5ControlGetConfigHandler(void *pMsg, i5_socket_type *psock, int length )
{
  t_I5_API_CONFIG_BASE *pCfg = (t_I5_API_CONFIG_BASE *)pMsg;
  i5TraceInfo("Subcommand %d\n", pCfg->subcmd);
  switch ( pCfg->subcmd ) {
    case I5_API_CONFIG_BASE:
    {
      t_I5_API_CONFIG_BASE rspData = { 0 };
      
      rspData.isEnabled = i5_config.running;
      rspData.isRegistrar = i5_config.isRegistrar;
      /* friendlyName is NULL terminated */
      strncpy(rspData.deviceFriendlyName, i5_config.friendlyName, I5_DEVICE_FRIENDLY_NAME_LEN);
      rspData.apFreqBand24En = i5_config.freqBandEnable[i5MessageFreqBand_802_11_2_4Ghz];
      rspData.apFreqBand5En = i5_config.freqBandEnable[i5MessageFreqBand_802_11_5Ghz];
 
 
      i5Trace("I5_API_CMD_SET_CONFIG - BASE: %d %s %d - bandEn %d, %d\n", 
              rspData.isEnabled,
              rspData.deviceFriendlyName, 
              rspData.isRegistrar,
              rspData.apFreqBand24En,
              rspData.apFreqBand5En);
      
      i5apiSendMessage(psock->sd, I5_API_CMD_GET_CONFIG, &rspData, sizeof(rspData));
      break;
    }
#if defined(DMP_DEVICE2_IEEE1905BASELINE_1)
    case I5_API_CONFIG_GET_LINK_METRICS:
    {
      t_I5_API_CONFIG_GET_LINK_METRICS* msg = pMsg;
      i5ControlFetchLinkMetrics(psock->sd, msg->ieee1905Id, msg->remoteInterfaceId);
      break;
    }
#endif
 
    default:
      i5TraceError("Unhandled Get command - %d\n", pCfg->subcmd);
      _i5ControlUnknownMessage(psock, I5_API_CMD_GET_CONFIG);
      break;
  }
}

static void i5ControlSocketReceive(i5_socket_type *psock)
{
  int length;
  char buf[4096];

  i5TraceInfo("\n");
  length = recv(psock->sd, &buf[0], sizeof(buf), 0);
  if (length < 0) {
    printf("recvfrom() error %d: %s\n", errno, strerror(errno));
  }
  else if (length == 0) {
    /* socket has been closed */
    i5SocketClose(psock);
  }
  else {
    t_I5_API_MSG *pMsg;

    pMsg = (t_I5_API_MSG *)&buf[0];
    i5TraceInfo("Command %d \n", pMsg->cmd);
    switch ( pMsg->cmd ) {
      case I5_API_CMD_RETRIEVE_DM:
        if ( length < sizeof(t_I5_API_MSG)) {
          printf("Invalid length for I5_API_CMD_RETRIEVE_DM\n");
          _i5ControlUnknownMessage(psock, pMsg->cmd);
        }
        else {
          i5ControlDataModel(psock, pMsg->cmd);
        }
        break;

      case I5_API_CMD_TRACE:
        if ( length < (sizeof(t_I5_API_MSG) + sizeof(t_I5_API_TRACE_MSG))) {
          printf("Invalid length for I5_API_CMD_TRACE\n");
        }
        else {
          t_I5_API_TRACE_MSG* traceCmd = (t_I5_API_TRACE_MSG *)(pMsg + 1);
          if ( ((traceCmd->module_id >= 0) && (traceCmd->module_id < i5TraceLast)) || 
                (traceCmd->module_id == 255) || (traceCmd->module_id == i5TracePacket) ) {
            i5TraceSet(traceCmd->module_id, traceCmd->depth, traceCmd->ifindex, traceCmd->interfaceMac);
          }
          else {
            printf("Invalid moduled_id (%d) for I5_API_CMD_TRACE\n", traceCmd->module_id);
          }
        }
        break;

      case I5_API_CMD_TRACE_TIME:
        if ( length < (sizeof(t_I5_API_MSG) + sizeof(int)) ) {
          printf("Invalid length for I5_API_CMD_TRACE_TIME\n");
        }
        else {
          i5TraceTimestampSet(*(int *)(pMsg + 1));
        }
        break;

#if defined(WIRELESS)
      case I5_API_CMD_WLCFG:
        if ( length < (sizeof(t_I5_API_MSG) + sizeof(t_I5_API_WLCFG_MSG))) {
          printf("Invalid length for I5_API_CMD_WLCFG\n");
          _i5ControlUnknownMessage(psock, pMsg->cmd);
        }
        else {
          i5ctlWlcfgHandler(psock, (t_I5_API_WLCFG_MSG *)(pMsg + 1));
        }
        break;
#endif

#if defined(SUPPORT_HOMEPLUG)
      case I5_API_CMD_PLC:
        if ( length < (sizeof(t_I5_API_MSG) + sizeof(t_I5_API_PLC_MSG))) {
          printf("Invalid length for I5_API_CMD_PLC\n");
        }
        else {
          i5ControlPlcHandler((t_I5_API_PLC_MSG *)(pMsg + 1));
        }
        break;
#endif
#if defined(SUPPORT_IEEE1905_FM)
      case I5_API_CMD_FLOWSHOW:
         i5FlowManagerShow();
         break;
#endif /* defined(SUPPORT_IEEE1905_FM) */

      case I5_API_CMD_LINKUPDATE:
        i5MessageSendLinkQueries();
        break;

      case I5_API_CMD_JSON_LEG:
        if ( length < (sizeof(t_I5_API_MSG) + sizeof(t_I5_API_JSON_LEG_MSG))) {
          printf("Invalid length for I5_API_CMD_JSON_LEG\n");
        }
        else {
          i5ControlJsonLegHandler((t_I5_API_JSON_LEG_MSG *)(pMsg + 1));
        }
        break;

      case I5_API_CMD_PUSH_BUTTON:
        i5SecurityProcessLocalPushButtonEvent();
#if defined(IEEE1905_KERNEL_MODULE_PB_SUPPORT)
        i5UdpSocketTriggerWirelessPushButtonEvent(1);
#endif
        break;

      case I5_API_CMD_SHOW_AL_MAC:
        i5ControlAlMacAddress(psock, pMsg->cmd);
        break;

      case I5_API_CMD_SEND_MESSAGE:
        if ( length < (sizeof(t_I5_API_MSG) + sizeof(t_I5_API_GOLDEN_NODE_SEND_MSG)) ) {
          printf("Invalid length for I5_API_CMD_SEND_MESSAGE\n");
          _i5ControlUnknownMessage(psock, pMsg->cmd);
        }
        else {
          i5ControlSendCommand(psock, (t_I5_API_GOLDEN_NODE_SEND_MSG *)(pMsg + 1), pMsg->cmd);
        }
        break;

      case I5_API_CMD_SET_LQ_INTERVAL:
        if ( length < (sizeof(t_I5_API_MSG) + sizeof(t_I5_API_LINK_METRIC_INTERVAL)) ) {
          printf("Invalid length for I5_API_CMD_SET_LQ_INTERVAL\n");
        }
        else {
          i5ControlLinkMetricIntervalHandler((t_I5_API_LINK_METRIC_INTERVAL *)(pMsg + 1));
        }
        break;
        
      case I5_API_CMD_SEND_BYTES:
        if ( length < (sizeof(t_I5_API_MSG) + sizeof(t_I5_API_GOLDEN_NODE_SEND_MSG_BYTES)) ) {
          printf("Invalid length for I5_API_CMD_SEND_BYTES\n");
        }
        else {
          i5ControlSendBytesCommand (length, (t_I5_API_GOLDEN_NODE_SEND_MSG_BYTES*) (pMsg + 1) );
        }
        break;

#if defined(WIRELESS)
      case I5_API_CMD_SET_WIFI_PASS_SSID:
        if ( length < (sizeof(t_I5_API_MSG) + sizeof(t_I5_API_PASSWORD_SSID_MSG)) ) {
          printf("Invalid length for I5_API_CMD_SET_WIFI_PASS_SSID\n");
        }
        else {
          i5ControlSetWiFiPassSsid ( (t_I5_API_PASSWORD_SSID_MSG*) (pMsg + 1));
        }
        break;
      case I5_API_CMD_SET_WIFI_OVERRIDE_BW:
        if ( length < (sizeof(t_I5_API_MSG) + sizeof(t_I5_API_OVERRIDE_BW_MSG)) ) {
          printf("Invalid length for I5_API_CMD_SET_WIFI_OVERRIDE_BW\n");
        }
        else {
          t_I5_API_OVERRIDE_BW_MSG *overrideMsg = (t_I5_API_OVERRIDE_BW_MSG*) (pMsg + 1);
          i5WlLinkMetricsOverrideBandwidth(overrideMsg->availBwMbps, overrideMsg->macThroughBwMbps, overrideMsg->overrideCount);
        }
        break;
#endif
#if defined(SUPPORT_HOMEPLUG)
      case I5_API_CMD_SET_PLC_PASS_NMK:
        if ( length < (sizeof(t_I5_API_MSG) + sizeof(t_I5_API_PASSWORD_SSID_MSG)) ) {
          printf("Invalid length for I5_API_CMD_SET_PLC_PASS_NMK\n");
        }
        else {         
          i5ControlSetPlcPass (psock, (t_I5_API_PASSWORD_SSID_MSG*) (pMsg + 1), pMsg->cmd);
        }
        break;
      case I5_API_CMD_SET_PLC_OVERRIDE_BW:
        if ( length < (sizeof(t_I5_API_MSG) + sizeof(t_I5_API_OVERRIDE_BW_MSG)) ) {
          printf("Invalid length for I5_API_CMD_SET_PLC_OVERRIDE_BW\n");
        }
        else { 
          t_I5_API_OVERRIDE_BW_MSG *overrideMsg =  (t_I5_API_OVERRIDE_BW_MSG*) (pMsg + 1);
          i5PlcLinkMetricsOverrideBandwidth(overrideMsg->availBwMbps, overrideMsg->macThroughBwMbps, overrideMsg->overrideCount);
        }
        break;
#endif
#if defined(SUPPORT_MOCA)
      case I5_API_CMD_SET_MOCA_PASS:
        /* There's no password in MoCA */
        break;
#endif
      case I5_API_CMD_SHOW_MSGS:
        i5MessageDumpMessages();
        break;
      case I5_API_CMD_SHOW_SOCKETS:
        i5SocketDumpSockets();
        break;
      case I5_API_CMD_STOP:
        i5_config.running = 0;
        break;
      case I5_API_CMD_SET_CONFIG:
          i5TraceInfo("SET CONFIG\n");
          if ( length < (sizeof(t_I5_API_MSG) + sizeof(t_I5_API_CONFIG_SUBCMD)) ) {
            printf("Invalid length for I5_API_CMD_SET_CONFIG\n");
          }
          else {
            i5ControlSetConfigHandler((pMsg + 1), length);
          }
          break;
      case I5_API_CMD_GET_CONFIG:
          i5TraceInfo("GET CONFIG\n");
          if ( length < (sizeof(t_I5_API_MSG) + sizeof(t_I5_API_CONFIG_SUBCMD)) ) {
            printf("Invalid length for I5_API_CMD_GET_CONFIG\n");
            _i5ControlUnknownMessage(psock, pMsg->cmd);
          }
          else {
            i5ControlGetConfigHandler((pMsg + 1), psock, length);
          }
          break;
      default:
        printf("Unknown command received %d\n", pMsg->cmd);
        _i5ControlUnknownMessage(psock, pMsg->cmd); 
        break;
    }
  }

  return;
}

static void i5ControlSocketAccept(i5_socket_type *psock)
{
  struct sockaddr_un clientAddr;
  unsigned int sockAddrSize;
  int sd;
  int flags;

  i5TraceInfo("\n");
  sockAddrSize = sizeof(clientAddr);
  if ((sd = accept(psock->sd, (struct sockaddr *)&clientAddr, &sockAddrSize)) < 0) {
    fprintf(stderr, "%s: accept connection failed. errno=%d\n", __func__, errno);
    return;
  }

  flags = fcntl(sd, F_GETFL, 0);
  if(flags < 0) {
    fprintf(stderr, "%s: cannot retrieve socket flags. errno=%d\n", __func__, errno);
  }
  if ( fcntl(sd, F_SETFL, flags | O_NONBLOCK) < 0 ) {
    fprintf(stderr, "%s: cannot set socket to non-blocking. errno=%d\n", __func__, errno);
  }

  if ( NULL == i5SocketNew(sd, i5_socket_type_stream, i5ControlSocketReceive) ) {
    close(sd);
  }
  return;
}

i5_socket_type *i5ControlSocketCreate(void)
{
  i5_socket_type *psock;
  int             sd;
  int             flags;
  int             optval = 1;
  socklen_t       optlen = sizeof(optval);

  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("socket() error, %s\n", strerror(errno));
    return NULL;
  }

  psock = i5SocketNew(sd, i5_socket_type_stream, i5ControlSocketAccept);
  if ( NULL == psock ) {
    printf("i5SocketNew failed\n");
    close(sd);
    return NULL;
  }

  /* Allow reusing the socket immediately when application is restarted */
  if (setsockopt(psock->sd, SOL_SOCKET, SO_REUSEADDR, &optval, optlen)) {
    printf("setsockopt error %s\n", strerror(errno));
  }

  psock->u.sinl.sa.sin_family      = AF_INET;
  psock->u.sinl.sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  psock->u.sinl.sa.sin_port        = htons( (unsigned short)I5_GLUE_CONTROL_SOCK_PORT);
  if((bind(sd, (struct sockaddr *)&psock->u.sinl.sa, sizeof(struct sockaddr_in))) == -1) {
    printf("bind() to port %d error, %s\n", I5_GLUE_CONTROL_SOCK_PORT, strerror(errno));
    i5SocketClose(psock);
    return NULL;
  }

  if ((listen(sd, I5_MESSAGE_BACKLOG)) == -1) {
    printf("listen() to port %d error, %s", I5_GLUE_CONTROL_SOCK_PORT, strerror(errno));
    i5SocketClose(psock);
    return NULL;
  }

  flags = fcntl(sd, F_GETFL, 0);
  if(flags < 0) {
    printf("cannot retrieve socket flags. error=%s", strerror(errno));
  }
  if ( fcntl(sd, F_SETFL, flags | O_NONBLOCK) < 0 ) {
    printf("cannot set socket to non-blocking. error=%s", strerror(errno));
  }

  return(psock);
}

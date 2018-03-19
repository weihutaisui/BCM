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

/*
 * IEEE1905 Interface
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if_arp.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include "ieee1905_timer.h"
#include "ieee1905_message.h"
#include "ieee1905_socket.h"
#include "ieee1905_datamodel.h"
#include "ieee1905_interface.h"
#include "ieee1905_trace.h"
#include "ieee1905_plc.h"
#include "ieee1905_plc_fob.h"
#include "ieee1905_tlv.h"
#include "ieee1905_wlmetric.h"
#if defined(BRCM_CMS_BUILD)
#include "ieee1905_cmsutil.h"
#endif
#include "ieee1905_flowmanager.h"
#include "ieee1905_brutil.h"
#include "ieee1905_glue.h"
#if defined(WIRELESS)
#include "ieee1905_wlcfg.h"
#endif
#include "ieee1905_ethstat.h"

#define I5_TRACE_MODULE                                i5TraceInterface
#define I5_INTERFACE_WIFI_PROMISCUOUS_STP_TIMEOUT_MSEC 5000 

typedef void (*i5SocketReceiveFunc)(i5_socket_type *psock);

int i5InterfaceInfoGet(char *ifname, unsigned char *mac_address)
{
  struct ifreq ifr;
  int sockfd;
  int ifindex;
#if defined(SUPPORT_HOMEPLUG)
  unsigned short mediaType;
#endif

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    return -1;
  }

  strncpy(ifr.ifr_name, ifname, I5_MAX_IFNAME-1);
  ifr.ifr_name[I5_MAX_IFNAME-1] = '\0';
  if (ioctl(sockfd, SIOCGIFINDEX, &ifr) == -1) {
    close(sockfd);
    return -1;
  }
  ifindex = ifr.ifr_ifindex;

#if defined(SUPPORT_HOMEPLUG)
  unsigned char netTechOui[I5_PHY_INTERFACE_NETTECHOUI_SIZE];
  mediaType = i5GlueInterfaceGetMediaInfoFromName(ifname, NULL, NULL, netTechOui, NULL, NULL, NULL, 0);
  if ( i5DmIsInterfacePlc(mediaType, netTechOui) ) {
    if ( i5PlcControlSockReady() ){
      if ( i5PlcGetDevMacAddress(mac_address) < 0 ) {
        close(sockfd);
        return -1;
      }
    }
    else {
      close(sockfd);
      return -1;
    }
  }
  else
#endif
  {
    ifr.ifr_addr.sa_family = AF_INET;
    if (ioctl(sockfd, SIOCGIFHWADDR, &ifr) == -1) {
      close(sockfd);
      return -1;
    }
    memcpy(mac_address, ifr.ifr_hwaddr.sa_data, ETH_ALEN);
  }

  close(sockfd);
  return (ifindex);
}

void i5InterfacePacketSend(i5_socket_type *psock, i5_packet_type *ppkt)
{
  if (sendto(psock->sd, ppkt->pbuf, ppkt->length, 0, (struct sockaddr*)&(psock->u.sll.sa), sizeof(struct sockaddr_ll)) == -1) {
//    printf("sendto() error\n");
  }
}

void i5InterfacePacketReceive(i5_socket_type *psock)
{
  int length;
  i5_packet_type *ppkt;

  i5TraceInfo(": received packet from interface %s (socket %d, proto:%04x type:%d)\n", psock->u.sll.ifname, psock->sd, ntohs(psock->u.sll.sa.sll_protocol), psock->type);
  if ((ppkt = i5PacketNew()) != NULL) {
    length = recvfrom(psock->sd, ppkt->pbuf, I5_PACKET_BUF_LEN, 0, NULL, NULL);
    if (psock->type == i5_socket_type_bridge_ll) {
      i5TraceError("Error, received packet on bridge\n");
      i5PacketFree(ppkt);
      return;
    }
    if (length == -1) {
      i5PacketFree(ppkt);
    }
    else {
      ppkt->length = length;
      i5MessagePacketReceive(psock, ppkt);
    }
  }
}

void i5InterfaceEthernetNotifyReceiveOperStatus(i5_socket_type *psock, unsigned char const operStatus)
{
  unsigned char aggregateOperStatus = operStatus;
  
  i5Trace("Oper Status: %d\n", aggregateOperStatus);
  if (IF_OPER_UP == aggregateOperStatus || IF_OPER_UNKNOWN == aggregateOperStatus) {
    psock->u.sll.discoveryRetryPeriod = 0;
    i5MessageTopologyDiscoveryTimeout(psock);
  }
  else if (IF_OPER_DOWN == aggregateOperStatus) {
    i5_socket_type *searchSock = i5_config.i5_socket_list.ll.next;
    while (searchSock) {
      // The interface is still up if we can find a socket with same MAC
      // but don't count the input psock, since that's going down, and don't count the bridge
      if ((psock->type == i5_socket_type_ll) &&
          (psock->u.sll.sa.sll_ifindex != searchSock->u.sll.sa.sll_ifindex) &&
          (memcmp(psock->u.sll.mac_address, searchSock->u.sll.mac_address, ETH_ALEN) == 0)) {
        aggregateOperStatus = IF_OPER_UP;
        break;
      }
      searchSock = searchSock->ll.next;
    }
  }
  i5DmInterfaceStatusSet(i5_config.i5_mac_address, psock->u.sll.mac_address, i5SocketGetIfIndex(psock), aggregateOperStatus);

  if ((IF_OPER_DOWN == operStatus) && (IF_OPER_UP == aggregateOperStatus)) {
    i5DmDeviceFreeUnreachableNeighbors(i5_config.i5_mac_address, i5SocketGetIfIndex(psock), NULL, 0);
  }
}

void i5InterfaceSocketPromiscuousMulticastSet(i5_socket_type *psock, unsigned char *multicast_address)
{
  struct packet_mreq mr;

  memset(&mr,0,sizeof(mr));
  mr.mr_ifindex = psock->u.sll.sa.sll_ifindex;
  mr.mr_type = PACKET_MR_MULTICAST;
  mr.mr_alen = ETH_ALEN;
  memcpy(mr.mr_address, multicast_address, ETH_ALEN);

  if(setsockopt(psock->sd, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr, sizeof(mr)) < 0)
  {
    printf("%s: setsockopt error (%s)\n", __func__, strerror(errno));
    printf("multicast_addr: %02x %02x %02x %02x %02x %02x\n", 
           multicast_address[0],multicast_address[1],multicast_address[2],
           multicast_address[3],multicast_address[4],multicast_address[5]);
  }
}

void i5InterfaceWifiPromiscuousStp(void *arg)
{
  unsigned char stp_mc_address[] = {0x01, 0x80, 0xc2, 0x00, 0x00, 0x00};
  i5_socket_type *psock = (i5_socket_type *)arg;

  i5TimerFree(psock->u.sll.pnltmr);
  psock->u.sll.pnltmr = NULL;
  i5InterfaceSocketPromiscuousMulticastSet(psock, stp_mc_address);

  i5MessageTopologyDiscoveryTimeout(psock);
}

void i5InterfaceWifiNotifyReceiveOperStatus(i5_socket_type *psock, unsigned char oper_status)
{
  i5Trace("Oper Status: %d\n", oper_status);
  if (IF_OPER_UP == oper_status || IF_OPER_UNKNOWN == oper_status) {
    /* A delay is needed by the wlan interface, unfortunately, before we can configure it */
    if (psock->u.sll.pnltmr) {
      i5TimerFree(psock->u.sll.pnltmr);
    }
    psock->u.sll.pnltmr = i5TimerNew(I5_INTERFACE_WIFI_PROMISCUOUS_STP_TIMEOUT_MSEC, i5InterfaceWifiPromiscuousStp, psock);
#if defined(SUPPORT_IEEE1905_FM) && defined(SUPPORT_IEEE1905_AUTO_WDS)
    i5FlowManagerProcessWirelessUp();
#endif
#if defined(WIRELESS)
    i5WlcfgApAutoconfigurationStart(psock->u.sll.ifname);
#endif
  }
  else if (IF_OPER_DOWN == oper_status) {
#if defined(SUPPORT_IEEE1905_FM) && defined(SUPPORT_IEEE1905_AUTO_WDS)
    i5FlowManagerProcessLocalWirelessDown();
#endif
#if defined(WIRELESS)
    i5WlcfgApAutoconfigurationStop(psock->u.sll.ifname);
#endif
  }
  i5DmInterfaceStatusSet(i5_config.i5_mac_address, psock->u.sll.mac_address, i5SocketGetIfIndex(psock), oper_status);
}

void i5InterfacePlcNotifyReceiveOperStatus(i5_socket_type *psock, unsigned char oper_status)
{
  i5Trace("Oper Status: %d\n", oper_status);
  if (IF_OPER_UP == oper_status || IF_OPER_DOWN == oper_status) {
    i5DmInterfaceStatusSet(i5_config.i5_mac_address, psock->u.sll.mac_address, i5SocketGetIfIndex(psock), oper_status);
  }
}

i5_socket_type *i5InterfaceProtoSocketCreate(char *ifname, unsigned short protocol, unsigned int socket_type, i5SocketReceiveFunc pRcvFunc)
{
  i5_socket_type *psock;
  int sd;
  int flags;

  if ((sd = socket(AF_PACKET, SOCK_RAW, htons(protocol))) == -1) {
    printf("socket() error - ensure that this process is running as root\n");
    return NULL;
  }

  flags = fcntl(sd, F_GETFL, 0);
  if(flags < 0) {
     printf("cannot retrieve socket flags. errno=%d", errno);
  }
  if ( fcntl(sd, F_SETFL, flags | O_NONBLOCK) < 0 ) {
     printf("cannot set socket to non-blocking. errno=%d", errno);
  }

  psock = i5SocketNew(sd, socket_type, pRcvFunc);
  if ( psock == NULL ) {
    close(sd);
    return NULL;
  }
  psock->u.sll.sa.sll_family   = PF_PACKET;
  psock->u.sll.sa.sll_protocol = htons(protocol); // Set to receive only these packets
  psock->u.sll.sa.sll_halen    = ETH_ALEN; // Some versions of linux can't send without this
  strncpy(psock->u.sll.ifname, ifname, I5_MAX_IFNAME-1);
  psock->u.sll.ifname[I5_MAX_IFNAME-1] = '\0';

  if ((psock->u.sll.sa.sll_ifindex = i5InterfaceInfoGet(ifname, psock->u.sll.mac_address)) == -1) {
    printf("i5InterfaceInfoGet(%s) error\n", ifname);
    i5SocketClose(psock);
    return NULL;
  }

  if((bind(sd, (struct sockaddr *)&(psock->u.sll.sa), sizeof(struct sockaddr_ll)))== -1) {
    printf("bind() error\n");
    i5SocketClose(psock);
    return NULL;
  }

  i5Trace("new socket: sd=%d, protocol=%04hx, %s\n", sd, protocol, ifname);

  return(psock);
}

i5_socket_type *i5InterfaceSocketSet(char *ifname, unsigned short media_type, unsigned char const *pNetTechOui)
{
  i5_socket_type *pifsock;
  i5_socket_type *psock;

  /* create 1905 protocol socket */
  pifsock = i5InterfaceProtoSocketCreate(ifname, I5_PROTO, i5_socket_type_ll, i5InterfacePacketReceive);
  if ( NULL == pifsock ) {
    return NULL;
  }
  i5InterfaceSocketPromiscuousMulticastSet(pifsock, I5_MULTICAST_MAC);
  if ( i5DmIsInterfaceWireless(media_type) ) {
    pifsock->u.sll.options = USE_IF_MAC_AS_SRC;
    pifsock->u.sll.notify  = i5InterfaceWifiNotifyReceiveOperStatus;
  }
  else if (i5DmIsInterfaceEthernet(media_type) ) {
    pifsock->u.sll.options = USE_AL_MAC_AS_SRC;
    pifsock->u.sll.notify  = i5InterfaceEthernetNotifyReceiveOperStatus;
    pifsock->u.sll.pInterfaceCtx = i5EthStatGetCtx(ifname);
  }
  else if (i5DmIsInterfacePlc(media_type, pNetTechOui) ) {
    pifsock->u.sll.options = USE_AL_MAC_AS_SRC;
    pifsock->u.sll.notify = i5InterfacePlcNotifyReceiveOperStatus;
#if defined(SUPPORT_HOMEPLUG)
    psock = i5InterfaceProtoSocketCreate(ifname, IEEE1905_PLC_FOB_IND_ETHERTYPE, i5_socket_type_raweth, i5PlcFOBMessageReceive);
    if ( NULL == psock ) {
      i5SocketClose(pifsock);
      return NULL;
    }
    psock->u.sll.options = pifsock->u.sll.options;
    psock->u.sll.notify = NULL;
    pifsock->u.sll.pMetricSock = psock;
#endif
  }
  else {
    pifsock->u.sll.options = USE_AL_MAC_AS_SRC;
    pifsock->u.sll.notify = NULL;
  }

  /* create LLDP socket */
  psock = i5InterfaceProtoSocketCreate(ifname, LLDP_PROTO, i5_socket_type_raweth, i5InterfacePacketReceive);
  if ( NULL == psock ) {
    i5SocketClose(pifsock);
    return NULL;
  }
  psock->u.sll.options = pifsock->u.sll.options;
  psock->u.sll.notify = NULL;
  pifsock->u.sll.pLldpProtoSock = psock;
  i5InterfaceSocketPromiscuousMulticastSet(psock, LLDP_MULTICAST_MAC);

  return(pifsock);
}

void i5InterfaceNew(char *ifname, unsigned short media_type, unsigned char const *media_specific_info, 
                    unsigned int media_specific_info_size, 
                    unsigned char const *pNetTechOui,  unsigned char const *pNetTechVariant,  unsigned char const *pNetTechName, unsigned char const *url, int sizeUrl,
                    i5MacAddressDeliveryFunc deliverFunc)
{
  i5_socket_type *psock = NULL;
  unsigned char   macAddr[MAC_ADDR_LEN];
  unsigned char  *interfaceId = NULL;
  int             ifindex;
  int             bCreateSocket;
  unsigned char   status;
#if defined(SUPPORT_BRIDGE_DEDICATED_PORT) && defined(WIRELESS) && defined(SUPPORT_IEEE1905_FM)
  char            parentIfnameBuf[I5_MAX_IFNAME];
  char            bridgeIfnameBuf[I5_MAX_IFNAME];
  char *          brName;
  char *          wlParentName;
#endif

  i5Trace("called for interface %s media type = %04x\n", ifname, media_type);

#if defined(SUPPORT_BRIDGE_DEDICATED_PORT) && defined(WIRELESS) && defined(SUPPORT_IEEE1905_FM)
  wlParentName = i5WlcfgGetWlParentInterface(ifname, parentIfnameBuf);
  if (  wlParentName != NULL && wlParentName != ifname) {
      // we are the child interface of another interface.  Assume it's dedicated.
      brName = i5BrUtilGetBridgeName(ifname, bridgeIfnameBuf);
      i5TraceAssert(brName != NULL);
      if (brName) {
          printf("Marking %s as dedicated (bridge: %s)\n", ifname, brName);
          i5BrUtilMarkDedicatedStpPort(brName, ifname, 1);
      }
  }
#endif

  bCreateSocket = i5GlueInterfaceIsSocketRequired(ifname);
  if ( 0 == bCreateSocket ) {
    i5TraceInfo("create dm interface but not sockets for %s\n", ifname);
    ifindex = i5InterfaceInfoGet(ifname, &macAddr[0]);
    if ( ifindex <= 0 ) {
      i5TraceError("Unable to read interface information for %s\n", ifname);
      return;
    }
    interfaceId = &macAddr[0];
    status = IF_OPER_DOWN;
  }
  else {
    i5TraceInfo("create dm interface and sockets for %s\n", ifname);
    psock = i5InterfaceSocketSet(ifname, media_type, pNetTechOui);
    if ( psock == NULL ) {
      i5TraceError("Unable to create sockets for %s\n", ifname);
      return;
    }
    interfaceId = &psock->u.sll.mac_address[0];
    status = IF_OPER_UP;
  }

  i5DmInterfaceUpdate(i5_config.i5_mac_address, interfaceId, I5_MESSAGE_VERSION, media_type, 
                      media_specific_info, media_specific_info_size, 
                      deliverFunc, ifname, status);

  if ((media_type == 0xffff) && (pNetTechOui || pNetTechVariant || pNetTechName || url)) {
    i5DmInterfacePhyUpdate(i5_config.i5_mac_address, interfaceId, pNetTechOui, pNetTechVariant, pNetTechName, url);
  }
  
  if (psock) {
#if defined(SUPPORT_IEEE1905_FM)
    i5FlowManagerActivateInterface(psock);
#endif /* defined(SUPPORT_IEEE1905_FM) */
    i5MessageTopologyDiscoveryTimeout(psock);
  }
}

void i5InterfaceAddDefaultBrouteEntries(char *ifname)
{
  int ret;
  char *cmd;
  unsigned char macAddr[6];

  ret = i5InterfaceInfoGet(ifname, &macAddr[0]);
  if ( ret < 0 ) {
    i5TraceError("Unable to retrieve interface information\n");
  }

  cmd = (char *)malloc(256);
  snprintf(cmd, 256,
               "ebtables -t broute -D BROUTING -d " I5_MAC_DELIM_FMT " -p 0x%x -j DROP 2>/dev/null; "
               "ebtables -t broute -I BROUTING 1 -d " I5_MAC_DELIM_FMT " -p 0x%x -j DROP 2>/dev/null; ",
               I5_MAC_PRM(I5_MULTICAST_MAC), I5_PROTO,
               I5_MAC_PRM(I5_MULTICAST_MAC), I5_PROTO);
  system(cmd);

  snprintf(cmd, 256,
               "ebtables -t broute -D BROUTING -d " I5_MAC_DELIM_FMT " -p 0x%x -j DROP 2>/dev/null; "
               "ebtables -t broute -I BROUTING 1 -d " I5_MAC_DELIM_FMT " -p 0x%x -j DROP 2>/dev/null; ",
               I5_MAC_PRM(i5_config.i5_mac_address), I5_PROTO,
               I5_MAC_PRM(i5_config.i5_mac_address), I5_PROTO);
  system(cmd);

  snprintf(cmd, 256,
               "ebtables -t broute -D BROUTING -d " I5_MAC_DELIM_FMT " -p 0x%x -j DROP 2>/dev/null; "
               "ebtables -t broute -I BROUTING 1 -d " I5_MAC_DELIM_FMT " -p 0x%x -j DROP 2>/dev/null; ",
               I5_MAC_PRM(macAddr), I5_PROTO,
               I5_MAC_PRM(macAddr), I5_PROTO);
  system(cmd);
  free(cmd);
}

void i5InterfaceBridgeNotifyReceiveOperStatus( i5_socket_type *psock, unsigned char oper_status )
{
  unsigned char  brIfMacs[I5_DM_BRIDGE_TUPLE_MAX_INTERFACES][MAC_ADDR_LEN];
  char           portList[I5_DM_BRIDGE_TUPLE_MAX_INTERFACES][I5_MAX_IFNAME];
  int            index;
  int            prt, j;
  int            portCnt;
  int            found;
  int            ret;
#if defined(SUPPORT_HOMEPLUG)
  int            hasPlc = 0;
#endif

  i5Trace("\n");

  if ( (oper_status != IF_OPER_UP) && (oper_status != IF_OPER_UNKNOWN) ) {
    /* remove bridging tuple entry */
    i5DmBridgingTupleUpdate(i5_config.i5_mac_address, I5_MESSAGE_VERSION, psock->u.sll.ifname, 0, NULL);
  }
  else {
    memset(portList, 0, I5_DM_BRIDGE_TUPLE_MAX_INTERFACES * I5_MAX_IFNAME);
    portCnt = i5BrUtilGetPortList(psock->u.sll.ifname, I5_DM_BRIDGE_TUPLE_MAX_INTERFACES, &portList[0][0]);
    if ( portCnt == 0 ) {
      /* bridging tuple entry */
      i5DmBridgingTupleUpdate(i5_config.i5_mac_address, I5_MESSAGE_VERSION, psock->u.sll.ifname, 0, NULL);
    }
    else {
      int sockfd;
      struct ifreq ifr;

      if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        return;
      }

      index = 0;
      for( prt = 0; prt < portCnt; prt++) {
#if defined(SUPPORT_HOMEPLUG)
        unsigned char netTechOui[I5_PHY_INTERFACE_NETTECHOUI_SIZE];
        unsigned short mediaType = i5GlueInterfaceGetMediaInfoFromName(portList[prt], NULL, NULL, netTechOui, NULL, NULL, NULL, 0);
        if ( 1 == i5DmIsInterfacePlc(mediaType, netTechOui) ) {
          hasPlc = 1;
        }
#endif
        strncpy(ifr.ifr_name, portList[prt], I5_MAX_IFNAME-1);
        ifr.ifr_name[I5_MAX_IFNAME-1] = '\0';
        ifr.ifr_addr.sa_family = AF_INET;
        ret = ioctl(sockfd, SIOCGIFHWADDR, &ifr);
        if ( ret >= 0 ) {
          memcpy(&brIfMacs[index], ifr.ifr_hwaddr.sa_data, ETH_ALEN);
          if ( index == 0 ) {
            index++;
            continue;
          }
          found = 0;
          for ( j = 0; j < index; j++) {
            if (0 == memcmp(&brIfMacs[j], &brIfMacs[index], MAC_ADDR_LEN)) {
              /* already in list */
              found = 1;
              break;
            }
          }
          if ( 0 == found ) {
            index++;
            if ( I5_DM_BRIDGE_TUPLE_MAX_INTERFACES == index) {
               i5Trace(" maximum entries reached\n");
            }
          }
        }
      }

      close(sockfd);
      if ( index ) {
#if defined(SUPPORT_HOMEPLUG)
         if ((1 == hasPlc) && (index < I5_DM_BRIDGE_TUPLE_MAX_INTERFACES)) {
           if ( i5PlcControlSockReady() ) {
             if ( 0 == i5PlcGetDevMacAddress(brIfMacs[index]) ) {
               index++;
             }
           }
         }
#endif
        i5DmBridgingTupleUpdate(i5_config.i5_mac_address, I5_MESSAGE_VERSION, psock->u.sll.ifname, index, &brIfMacs[0][0]);         
      }
    }
  }
}

void i5InterfaceAdd(char *ifname, unsigned short matchMediaType)
{
  unsigned char  mediaSpecificInfo[i5TlvMediaSpecificInfoWiFi_Length] = {0};
  unsigned short mediaType;
  int            mediaLen = i5TlvMediaSpecificInfoWiFi_Length;
  unsigned char  netTechOui[I5_PHY_INTERFACE_NETTECHOUI_SIZE];
  unsigned char  netTechVariant;
  unsigned char  netTechName[I5_PHY_INTERFACE_NETTECHNAME_SIZE];
  unsigned char  url[I5_PHY_INTERFACE_URL_MAX_SIZE];
  

  /* skip interfaces that have already been learned */
  if ( i5SocketFindDevSocketByName(ifname) != NULL ) {
    return;
  }

  mediaType = i5GlueInterfaceGetMediaInfoFromName(ifname, mediaSpecificInfo, &mediaLen, netTechOui, &netTechVariant, netTechName, url, I5_PHY_INTERFACE_URL_MAX_SIZE);
  i5Trace("called for interface %s, mediaType %x, matchType %x\n", ifname, mediaType, matchMediaType);
  if ( I5_MATCH_MEDIA_TYPE_WL == matchMediaType ) {
    if (!i5DmIsInterfaceWireless(mediaType) ) {
      return;
    }
  }
  else if ( I5_MATCH_MEDIA_TYPE_PLC == matchMediaType ) {
    i5Trace("I5_MATCH_MEDIA_TYPE_PLC Checking\n"); 
    if (!i5DmIsInterfacePlc(mediaType, mediaType == 0xffff ? netTechOui : NULL) ) {
      i5Trace("I5_MATCH_MEDIA_TYPE_PLC Check failed\n");
      return;
    }
    i5Trace("I5_MATCH_MEDIA_TYPE_PLC Check passed\n");
  }
  else if ( I5_MATCH_MEDIA_TYPE_ETH == matchMediaType ) {
    if (!i5DmIsInterfaceEthernet(mediaType) ) {
      return;
    }
  }
  else if ( (matchMediaType != I5_MATCH_MEDIA_TYPE_ANY) && (matchMediaType != mediaType) ) {
    return;
  }

  if ( I5_MEDIA_TYPE_BRIDGE == mediaType) {
    i5_socket_type *pifsock = i5InterfaceProtoSocketCreate(ifname, I5_PROTO, i5_socket_type_bridge_ll, i5InterfacePacketReceive);
    if (NULL == pifsock) {
      exit(-1);
    }
    pifsock->u.sll.options = USE_AL_MAC_AS_SRC;
    pifsock->u.sll.notify  = i5InterfaceBridgeNotifyReceiveOperStatus;
    i5InterfaceBridgeNotifyReceiveOperStatus(pifsock, IF_OPER_UP);
    i5InterfaceAddDefaultBrouteEntries(ifname);
    return;
  }
#if defined(SUPPORT_HOMEPLUG)
  else if ( i5DmIsInterfacePlc(mediaType, netTechOui) ) {
    i5Trace("SUPPORT HOMEPLUG\n");
    if ( i5PlcControlSockReady() ) {
      i5Trace("SUPPORT HOMEPLUG socket ready\n");
      i5InterfaceNew(ifname, mediaType, mediaSpecificInfo, mediaLen, 
                     netTechOui, &netTechVariant, netTechName, url, I5_PHY_INTERFACE_URL_MAX_SIZE,
                     i5PlcLinkMetricsUpdateMacList );
    }
    else {
      i5Trace("SUPPORT HOMEPLUG socket not ready\n");
    }
  }
#endif
#if defined(WIRELESS)
  else if ( i5DmIsInterfaceWireless(mediaType) ) {
    i5InterfaceNew(ifname, mediaType, mediaSpecificInfo, mediaLen, 
                   NULL, NULL, NULL, NULL, I5_PHY_INTERFACE_URL_MAX_SIZE,
                   NULL);
  }
#endif
  else if (I5_MEDIA_TYPE_UNKNOWN != mediaType) {
    i5InterfaceNew(ifname, mediaType, mediaSpecificInfo, mediaLen,
                   NULL, NULL, NULL, NULL, I5_PHY_INTERFACE_URL_MAX_SIZE,
                   NULL);
  }
  return;
}

void i5InterfaceSearchAdd(unsigned short matchMediaType)
{
  DIR *dp;
  struct dirent *ep;
  char brif_path[MAX_PATH_NAME];

  snprintf(brif_path, MAX_PATH_NAME, "/sys/class/net");
  dp = opendir (brif_path);
  if (dp != NULL)
  {
    while ((ep = readdir(dp)) != NULL) {
      i5InterfaceAdd(ep->d_name, matchMediaType);
    }
    closedir (dp);
  }
  else {
    printf("Warning can't read interface list\n");
    return;
  }
}

/* return of zero indicates the searchString is not present *
 * any other value returned is the 1-based line number      *
 *
 * Notes: "wl wds" puts MACs in REVERSE order               */
int i5InterfaceSearchFileForString(char const *file, char const *searchString)
{
  char line[32] = "";
  unsigned int lineNumber = 0;
  int found = 0;

  if ((file == NULL) || (searchString == NULL)) {
    return 0;
  }

  i5Trace("Search for %s in %s\n", searchString, file);
  
  FILE* fp = fopen(file,"r");
  if (fp == NULL) {
    i5Trace("No such file\n");
    return 0;
  }
  
  while (fgets(line, sizeof(line)-1, fp) != NULL) {
    i5TraceInfo("Line: %s\n", line);
    if (!found) {
      if (strstr(line, searchString) != NULL ) {
        found = 1;
        lineNumber = 1; /* if this is last line in the "wl wds" list, we will return "1" */
        i5TraceInfo("Found! index=%d\n", lineNumber);
      }
    }
    else {
      /* For each line after the line we wanted, add 1 to the count */
      i5TraceInfo("Incrementing: index=%d\n", lineNumber);
      lineNumber ++;
    }
  }
  fclose(fp);
  i5Trace("returning index=%d\n", lineNumber);
  return lineNumber;
}

/* return of a pointer to the string in the index'th position *
 * return 0 on success
 * return 1 on failure
 *
 * Notes: "wl wds" puts MACs in REVERSE order by index        */
int i5InterfaceSearchFileForIndex(char const *file, unsigned int wdsIndex, char* macString, unsigned int size)
{
  unsigned int totalLines = 0;
  char line[32] = "";

  if ((file == NULL) || (wdsIndex == 0) || (macString == NULL) || (size == 0)) {
    return -1;
  }

  i5Trace("Search for index %d in %s\n", wdsIndex, file);
  
  FILE* fp = fopen(file,"r");
  if (fp == NULL) {
    i5Trace("No such file\n");
    return -1;
  }

  while (fgets(line, sizeof(line)-1, fp) != NULL) {
    totalLines ++;
  }

  if (wdsIndex > totalLines) {
    i5TraceError("ERROR: only %d WDS bridges, can't find bridge #%d\n", totalLines, wdsIndex);
    fclose(fp);
    return -1;
  }
  rewind(fp);
  while (totalLines >= wdsIndex) {
    fgets(line, sizeof(line)-1, fp);
    totalLines --;
  }
  strncpy(macString, &line[4], size); /* line[] looks like "wds xx:xx:xx:xx:xx:xx" */
  macString[size-1] = '\0';
  fclose(fp);
  return 0;
}

void i5InterfaceInit()
{
  srand((unsigned)time(NULL));  
  i5_config.last_message_identifier = rand()%0xFFFF;
  i5InterfaceSearchAdd(I5_MEDIA_TYPE_BRIDGE);
  i5InterfaceSearchAdd(I5_MATCH_MEDIA_TYPE_ANY);
}

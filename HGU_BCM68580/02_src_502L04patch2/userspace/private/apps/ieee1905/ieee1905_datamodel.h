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

#ifndef _IEEE1905_DATAMODEL_H_
#define _IEEE1905_DATAMODEL_H_

/*
 * IEEE1905 Data-Model
 */

#include "ieee1905_linkedlist.h"
#include "ieee1905_socket.h"
#include "ieee1905_message.h"

#define I5_DM_VERSION 0

/* media types defined by specification */
#define I5_MEDIA_TYPE_FAST_ETH     0x0000
#define I5_MEDIA_TYPE_GIGA_ETH     0x0001
#define I5_MEDIA_TYPE_WIFI_B       0x0100
#define I5_MEDIA_TYPE_WIFI_G       0x0101
#define I5_MEDIA_TYPE_WIFI_A       0x0102
#define I5_MEDIA_TYPE_WIFI_N24     0x0103
#define I5_MEDIA_TYPE_WIFI_N5      0x0104
#define I5_MEDIA_TYPE_WIFI_AC      0x0105
#define I5_MEDIA_TYPE_WIFI_AD      0x0106
#define I5_MEDIA_TYPE_WIFI_AF      0x0107
#define I5_MEDIA_TYPE_1901_WAVELET 0x0200
#define I5_MEDIA_TYPE_1901_FFT     0x0201
#define I5_MEDIA_TYPE_MOCA_V11     0x0300
#define I5_MEDIA_TYPE_UNKNOWN      0xFFFF

/* internal media types */
#define I5_MEDIA_TYPE_BRIDGE       0x8000
#define I5_MATCH_MEDIA_TYPE_WL     0x9001
#define I5_MATCH_MEDIA_TYPE_ETH    0x9002
#define I5_MATCH_MEDIA_TYPE_PLC    0x9004
#define I5_MATCH_MEDIA_TYPE_ANY    0x9008

/* General Phy Media Types */
#define I5_GEN_PHY_HPAV2_NETTECHOUI_01   0x02
#define I5_GEN_PHY_HPAV2_NETTECHOUI_02   0x10
#define I5_GEN_PHY_HPAV2_NETTECHOUI_03   0x18

/* General Phy Variant Types */
#define I5_GEN_PHY_HPAV2_NETTECHVARIANT  0x42

#define I5_MEDIA_SPECIFIC_INFO_MAX_SIZE      10
#define I5_PHY_INTERFACE_NETTECHOUI_SIZE      3
#define I5_PHY_INTERFACE_NETTECHNAME_SIZE    32
#define I5_PHY_INTERFACE_URL_MAX_SIZE        64

#define I5_INTERMEDIATE_LEGACY_BRIDGE_FALSE  0
#define I5_INTERMEDIATE_LEGACY_BRIDGE_TRUE   1

#define I5_DM_BRIDGE_TUPLE_MAX_INTERFACES    64
#define I5_DM_LINK_METRIC_ACTIVATED_TIME_MSEC 15000 
#define FWD_IF_LIST_LEN (I5_DM_BRIDGE_TUPLE_MAX_INTERFACES*MAC_ADDR_LEN)

#define I5_DM_NODE_VERSION_UNKNOWN      0
#define I5_DM_NODE_VERSION_1905         1
#define I5_DM_NODE_VERSION_19051A       2
#define I5_DM_VERSION_TIMER_MSEC        500

enum {
  i5DmStateDone = 0,
  i5DmStateNew,
  i5DmStatePending,
  i5DmStateNotFound  
};

typedef int (*i5MacAddressDeliveryFunc)(unsigned char const * macAddressList, unsigned char numMacs);

typedef struct {
  i5_ll_listitem ll;
  unsigned char  state;
  unsigned char  InterfaceId[MAC_ADDR_LEN];
  unsigned int   Status;
  unsigned int   SecurityStatus;
  unsigned int   LastChanged;
  unsigned short MediaType;       // 0xffff indicates a Generic Phy Device
  unsigned char  netTechOui[I5_PHY_INTERFACE_NETTECHOUI_SIZE];
  unsigned char  netTechVariant;
  unsigned char  netTechName[I5_PHY_INTERFACE_NETTECHNAME_SIZE];
  unsigned char  url[I5_PHY_INTERFACE_URL_MAX_SIZE];
  unsigned char  MediaSpecificInfo[I5_MEDIA_SPECIFIC_INFO_MAX_SIZE];
  unsigned int   MediaSpecificInfoSize;
  i5MacAddressDeliveryFunc i5MacAddrDeliver;
  union {
    struct {
      char          wlParentName[I5_MAX_IFNAME];
      unsigned char isRenewPending;
      unsigned char confStatus;
      unsigned char credChanged;
      unsigned char isConfigured; /* enrollee only */
    };
  };
} i5_dm_interface_type;

typedef struct {
  i5_ll_listitem ll;
  unsigned char  state;
  unsigned char  LocalInterfaceId[MAC_ADDR_LEN];
  unsigned char  NeighborInterfaceId[MAC_ADDR_LEN];
} i5_dm_legacy_neighbor_type;

typedef struct {
  i5_ll_listitem   ll;
  unsigned char    state;
  unsigned char    LocalInterfaceId[MAC_ADDR_LEN];
  unsigned char    Ieee1905Id[MAC_ADDR_LEN];
  unsigned char    NeighborInterfaceId[MAC_ADDR_LEN];
  unsigned char    IntermediateLegacyBridge;
  unsigned short   MacThroughputCapacity;             /* in Mbit/s */
  unsigned short   availableThroughputCapacity;       /* in Mbit/s */
  unsigned int     prevRxBytes;
  unsigned int     latestRxBytes;
  unsigned char    ignoreLinkMetricsCountdown;
  char             localIfname[I5_MAX_IFNAME];
  unsigned int     localIfindex;
  timer_elem_type *bridgeDiscoveryTimer;
} i5_dm_1905_neighbor_type;

typedef struct {
  i5_ll_listitem ll;
  unsigned char  state;
  char           ifname[I5_MAX_IFNAME];
  unsigned int   forwardingInterfaceListNumEntries;
  unsigned char  ForwardingInterfaceList[FWD_IF_LIST_LEN];
} i5_dm_bridging_tuple_info_type;

typedef struct {
  i5_ll_listitem ll;
  unsigned char  state;
  unsigned char  queryState;
  unsigned char  validated;
  unsigned char  numTopQueryFailures;
  unsigned char  nodeVersion;
  timer_elem_type *nodeVersionTimer;
  unsigned int   hasChanged;
  timer_elem_type *watchdogTimer;
  unsigned int   Version;
  unsigned char  DeviceId[MAC_ADDR_LEN];
  unsigned int   Status;
  unsigned int   LastChanged;
  unsigned int   InterfaceNumberOfEntries;
  unsigned int   LegacyNeighborNumberOfEntries;
  unsigned int   Ieee1905NeighborNumberOfEntries;
  unsigned int   BridgingTuplesNumberOfEntries;
  char           friendlyName[I5_DEVICE_FRIENDLY_NAME_LEN];
  i5_dm_interface_type           interface_list;
  i5_dm_legacy_neighbor_type     legacy_list;
  i5_dm_1905_neighbor_type       neighbor1905_list;
  i5_dm_bridging_tuple_info_type bridging_tuple_list;
} i5_dm_device_type;

typedef struct {
  timer_elem_type *dmLinkMetricTimer;
  char            dmLinkMetricIntervalValid;
  unsigned int    dmLinkMetricIntervalMsec;
  timer_elem_type *dmLinkMetricActivatedTimer;
} i5_dm_link_metric_autoquery_type;

typedef struct {
  i5_dm_device_type device_list;
  unsigned int   DevicesNumberOfEntries;
  unsigned char  updateStpNeeded;
  i5_dm_link_metric_autoquery_type linkMetricAuto;
  timer_elem_type *pLinkMetricTimer;
} i5_dm_network_topology_type;

extern i5_dm_network_topology_type i5_dm_network_topology;

int i5DmIsMacNull(unsigned char *mac);
int i5DmAnyWirelessInterfaceUp(i5_dm_device_type *deviceToCheck);
int i5DmDeviceIsSelf(unsigned char *device_id);
void i5DmDeviceFree(i5_dm_device_type *device);
i5_dm_interface_type *i5DmInterfaceGetLocalPlcInterface(void);
int i5DmIsInterfaceWireless(unsigned short mediaType);
int i5DmIsInterfacePlc(unsigned short mediaType, unsigned char const *netTechOui);
int i5DmIsInterfaceEthernet(unsigned short mediaType);
i5_dm_interface_type *i5DmInterfaceFind(i5_dm_device_type const *parent, unsigned char const *interface_id);
void i5DmLinkMetricsActivate(void);
void i5DmSetLinkMetricInterval (unsigned int newIntervalMsec);
unsigned char const* i5DmGetNameForMediaType(unsigned short mediaType);
unsigned int i5DmInterfaceStatusGet(unsigned char *device_id, unsigned char *interface_id);
void i5DmInterfaceStatusSet(unsigned char *device_id, unsigned char * interfaceId, int ifindex, unsigned int status);
void i5DmInterfacePending(unsigned char *device_id);
void i5DmInterfaceDone(unsigned char *device_id);
int i5DmInterfaceUpdate(unsigned char *device_id, unsigned char *interface_id, int version, unsigned short media_type, 
                        unsigned char const *media_specific_info, unsigned int media_specific_info_size,
                        i5MacAddressDeliveryFunc deliverFunc, char const *ifname, unsigned char status);
int i5DmInterfacePhyUpdate(unsigned char *device_id, unsigned char *interface_id,
                           unsigned char const *pNetTechOui, unsigned char const *pNetTechVariant, unsigned char const *pNetTechName, unsigned char const *url);
int i5DmBridgingTupleUpdate(unsigned char *device_id, int version, char *ifname, unsigned char tuple_num_macaddrs, unsigned char *bridging_tuple_list);
int i5DmBridgingTuplePending(unsigned char *device_id);
int i5DmBridgingTupleDone(unsigned char *device_id);
int i5DmLegacyNeighborPending(unsigned char *device_id);
int i5DmLegacyNeighborDone(unsigned char *device_id);
int i5DmLegacyNeighborUpdate(unsigned char *device_id, unsigned char *local_interface_id, unsigned char *neighbor_interface_id);
i5_dm_legacy_neighbor_type *i5DmLegacyNeighborFind(i5_dm_device_type *parent, unsigned char *local_interface_id, unsigned char *neighbor_interface_id);
int i5Dm1905NeighborPending(unsigned char *device_id);
int i5Dm1905NeighborDone(unsigned char *device_id);
int i5Dm1905NeighborUpdate(unsigned char *device_id, unsigned char *local_interface_id, unsigned char *neighbor1905_al_mac_address, unsigned char *neighbor1905_interface_id,
                           unsigned char *intermediate_legacy_bridge, char *localifName, int localifindex, unsigned char createNeighbor);
i5_dm_1905_neighbor_type *i5Dm1905GetLocalNeighbor (unsigned char const *neighbor1905_interface_id);
i5_dm_1905_neighbor_type *i5Dm1905NeighborFind(i5_dm_device_type *parent, unsigned char *local_interface_id, unsigned char *neighbor1905_al_mac_address);
int i5Dm1905NeighborBandwidthUpdate (i5_dm_1905_neighbor_type *neighbor, unsigned short MacThroughputCapacity,
                                     unsigned short availableThroughputCapacity, unsigned int rxBytesCumulative, unsigned char* sourceAlMac);
i5_dm_interface_type *i5Dm1905GetLocalInterface(i5_dm_1905_neighbor_type const *neighbor);
void i5DmDeviceQueryStateSet(unsigned char *device_id, unsigned char state);
unsigned char i5DmDeviceQueryStateGet(unsigned char const *device_id);
void i5DmWaitForGenericPhyQuery(i5_dm_device_type *destDev);
unsigned char i5DmAreThereNodesWithVersion(int nodeVersion);
i5_dm_device_type *i5DmDeviceNew(unsigned char *device_id, int version, char const* pDevFriendlyName);
i5_dm_device_type *i5DmDeviceFind(unsigned char const *device_id);
void i5DmDeviceDelete(unsigned char *device_id);
void i5DmDeviceTopologyQuerySendToAllNew(i5_socket_type *psock);
int i5DmDeviceTopologyChangeProcess(unsigned char *device_id);
void i5DmDeviceNewIfNew(unsigned char *neighbor_al_mac_address);
void i5DmTopologyFreeUnreachableDevices(void);
void i5DmDeviceFreeUnreachableNeighbors(unsigned char *device_id, int ifindex, unsigned char *neighbor_interface_list, unsigned int length);
int i5DmIsWifiBandSupported(char *ifname, unsigned int freqBand);

int  i5DmCtlSizeGet(void);
void i5DmCtlAlMacRetrieve(char *pMsgBuf);
void i5DmCtlRetrieve(char * pMsgBuf);

void i5DmRetryPlcRegistry(void);
i5_dm_1905_neighbor_type *i5Dm1905FindNeighborByRemoteInterface(i5_dm_device_type const *parent, unsigned char const *remote_interface_id);
int i5DmGetInterfacesWithNeighbor(unsigned char const *neighbor_al_mac, 
                                  unsigned char * local_interface_mac_addrs, unsigned char * neighbor_interface_mac_addrs, int maxInterfaces);

void i5Dm1905NeighborUpdateIntermediateBridgeFlag(i5_dm_device_type *device, i5_dm_1905_neighbor_type *neighbor, unsigned char bridgeFlag);
void i5DmRefreshDeviceTimer(unsigned char *alMacAddress, char createFlag);
void i5DmSetFriendlyName(const char * name);
int  i5DmInit(void);
void i5DmDeinit(void);

#endif

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
 * IEEE1905 Data-Model
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/if.h>
#include "ieee1905_json.h"
#include "ieee1905_message.h"
#include "ieee1905_trace.h"
#include "ieee1905_tlv.h"
#include "ieee1905_wlcfg.h"
#include "ieee1905_plc.h"
#include "ieee1905_flowmanager.h"
#include "ieee1905_interface.h"
#include "ieee1905_ethstat.h"
#include "ieee1905_wlmetric.h"
#include "ieee1905_utils.h"
#include "ieee1905_glue.h"
#include "ieee1905_timer.h"
#if defined(DMP_DEVICE2_IEEE1905BASELINE_1)
#include "ieee1905_cmsmdm.h"
#endif

#define I5_TRACE_MODULE i5TraceDm

#define I5_DM_MAX_LINK_METRICS_LIST_SIZE 16

#define I5_DM_NEIGHBOR_BRIDGE_DISCOVERY_TIMEOUT (2*I5_MESSAGE_TOPOLOGY_DISCOVERY_PERIOD_MSEC)

/* One timer for fetching link metrics from all devices, NOT "one timer per device" */
#if defined(SUPPORT_IEEE1905_GOLDENNODE)
#define I5_DM_LINK_METRICS_DEFAULT_AUTO_FETCH_INTERVAL_MSEC 0
#else
#define I5_DM_LINK_METRICS_DEFAULT_AUTO_FETCH_INTERVAL_MSEC 5000
#endif
#define I5_DM_LINK_METRICS_MINIMUM_AUTO_FETCH_INTERVAL_MSEC 50
#define I5_DM_LINK_METRICS_MAXIMUM_AUTO_FETCH_INTERVAL_MSEC 600000

#define I5_DM_DEVICE_TIMER_ROBUSTNESS            2
#define I5_DM_DEVICE_TIMER_LATENCY_MSEC          50
#define I5_DM_TOP_DISC_DEVICE_GONE_TIMER         I5_DM_DEVICE_TIMER_ROBUSTNESS*I5_MESSAGE_STANDARD_TOPOLOGY_DISCOVERY_PERIOD_MSEC + I5_DM_DEVICE_TIMER_LATENCY_MSEC

int i5DmInterfaceFindMatchingInterfaceId(i5_dm_device_type *parent, unsigned char *local_interface_id, unsigned char *other_al_mac_address, unsigned char *other_interface_id);
void i5DmDeviceFree(i5_dm_device_type *device);

i5_dm_network_topology_type i5_dm_network_topology = {};
 
unsigned char const* i5DmGetNameForMediaType(unsigned short mediaType)
{
   return i5UtilsGetNameForMediaType(mediaType);
}

int i5DmIsInterfaceWireless(unsigned short mediaType)
{
   return ((mediaType >= I5_MEDIA_TYPE_WIFI_B) && (mediaType <= I5_MEDIA_TYPE_WIFI_AF));
}

int i5DmIsInterfacePlc(unsigned short mediaType, unsigned char const *netTechOui)
{
   return ((mediaType >= I5_MEDIA_TYPE_1901_WAVELET) && (mediaType <= I5_MEDIA_TYPE_1901_FFT)) ||
           ((mediaType == I5_MEDIA_TYPE_UNKNOWN) && (netTechOui) && 
            (I5_GEN_PHY_HPAV2_NETTECHOUI_01 == netTechOui[0]) &&
            (I5_GEN_PHY_HPAV2_NETTECHOUI_02 == netTechOui[1]) &&
            (I5_GEN_PHY_HPAV2_NETTECHOUI_03 == netTechOui[2]) ); 
}

int i5DmIsInterfaceEthernet(unsigned short mediaType)
{
   return ((mediaType >= I5_MEDIA_TYPE_FAST_ETH) && (mediaType <= I5_MEDIA_TYPE_GIGA_ETH));
}

int i5DmIsMacNull(unsigned char *mac)
{
  int index = 0;
  for ( ; index < MAC_ADDR_LEN; index ++) {
    if (mac[index] != 0) {
      return 0;
    }
  }
  return 1;
}

int i5DmAnyWirelessInterfaceUp(i5_dm_device_type *deviceToCheck)
{
  if (NULL == deviceToCheck) {
    i5TraceError("NULL device passed in\n");
    return 0;
  }
  i5_dm_interface_type*interfaceToCheck = deviceToCheck->interface_list.ll.next;
  while (interfaceToCheck) {
    if ((i5DmIsInterfaceWireless(interfaceToCheck->MediaType)) && (interfaceToCheck->Status != IF_OPER_DOWN)) {
      return 1;
    }
    interfaceToCheck = interfaceToCheck->ll.next;
  }
  return 0;
}

/*
 *  Searches through the available interfaces and returns
 *  whether or not the given frequency band is supported.
 */
int i5DmIsWifiBandSupported(char *ifname, unsigned int freqBand)
{
#if defined(WIRELESS)
  i5_dm_device_type *pdmdev;
  i5_dm_interface_type *pdmif;

  if ((pdmdev = i5DmDeviceFind(i5_config.i5_mac_address)) != NULL) {
    pdmif = (i5_dm_interface_type *)pdmdev->interface_list.ll.next;
    while (pdmif != NULL) {
      unsigned int freqBandSupported = i5TlvGetFreqBandFromMediaType (pdmif->MediaType);
      if ((i5MessageFreqBand_Reserved != freqBandSupported) && 
          (freqBandSupported == freqBand)) {
        strcpy(ifname, pdmif->wlParentName);
        return 1;
      }
      pdmif = pdmif->ll.next;
    }
  }
#endif
  return 0;
}

void i5DmFillNeighborInterfaceId( void )
{
  i5_dm_device_type        *pCurrDev;
  i5_dm_device_type        *pNeighDev;
  i5_dm_interface_type     *pCurrDevIf;
  i5_dm_1905_neighbor_type *pCurrDevNeigh;
  i5_dm_1905_neighbor_type *pNeighDevNeigh;
  i5_dm_interface_type     *pNeighDevIf;

  i5Trace("\n");

  pCurrDev = i5_dm_network_topology.device_list.ll.next;
  while ( pCurrDev != NULL ) {
    pCurrDevNeigh = pCurrDev->neighbor1905_list.ll.next;
    while(pCurrDevNeigh != NULL) {
      pCurrDevIf = i5DmInterfaceFind(pCurrDev, pCurrDevNeigh->LocalInterfaceId);
      pNeighDev = i5DmDeviceFind(pCurrDevNeigh->Ieee1905Id);
      if (pNeighDev != NULL) {
        pNeighDevNeigh = pNeighDev->neighbor1905_list.ll.next;
        while ( pNeighDevNeigh != NULL ) {
          pNeighDevIf = i5DmInterfaceFind(pNeighDev, pNeighDevNeigh->LocalInterfaceId);
          if (!pCurrDevIf) {
            printf("i5DmFillNeighborInterfaceId NULL==pCurrDevIf (" I5_MAC_DELIM_FMT ")\n", I5_MAC_PRM(pCurrDevNeigh->LocalInterfaceId));
          }
          else
          if (!pNeighDevIf) {
            printf("i5DmFillNeighborInterfaceId NULL==pNeighDevIf (" I5_MAC_DELIM_FMT ")\n", I5_MAC_PRM(pNeighDevNeigh->LocalInterfaceId));
          }
          else
          if ( 0 == memcmp(pCurrDev->DeviceId, pNeighDevNeigh->Ieee1905Id, MAC_ADDR_LEN) ) {
            /* note that if we have multiple connections between two neighbors and two or more
               of the connections use interfces of the same mediatype then we will not be able
               to pick the correct neighbor interface */
            if ( pCurrDevIf->MediaType == pNeighDevIf->MediaType ) {
              /* we can update the neighbor id of the current devices neighbour entry using the
                 local device id of the neighbour devives neighbor entry 
                 for the local device this is technically a device change but the neighbor 
                 interface id is not in the topology response so no need to flag the change */
              memcpy(&pCurrDevNeigh->NeighborInterfaceId[0], &pNeighDevNeigh->LocalInterfaceId[0], MAC_ADDR_LEN);
              i5Json1905NeighborPrint(I5_JSON_ALL_CLIENTS, pCurrDevNeigh, i5Json_Add);            
            }
          }
          pNeighDevNeigh = pNeighDevNeigh->ll.next;
        }
      }
      pCurrDevNeigh = pCurrDevNeigh->ll.next;
    }
    pCurrDev = pCurrDev->ll.next;
  }
}

i5_dm_legacy_neighbor_type *i5DmLegacyNeighborNew(i5_dm_device_type *parent, unsigned char *local_interface_id, unsigned char *neighbor_interface_id)
{
  i5_dm_legacy_neighbor_type *new = (i5_dm_legacy_neighbor_type *)malloc(sizeof(i5_dm_legacy_neighbor_type));

  i5Trace( I5_MAC_FMT "\n", I5_MAC_PRM(neighbor_interface_id) );

  if (NULL != new) {
    memset(new, 0, sizeof(i5_dm_legacy_neighbor_type));
    i5LlItemAdd(parent, &parent->legacy_list, new);
    ++parent->LegacyNeighborNumberOfEntries;
    memcpy(new->LocalInterfaceId, local_interface_id, MAC_ADDR_LEN);
    memcpy(new->NeighborInterfaceId, neighbor_interface_id, MAC_ADDR_LEN);
  }

  return new;
}

i5_dm_legacy_neighbor_type *i5DmLegacyNeighborFind(i5_dm_device_type *parent, unsigned char *local_interface_id, unsigned char *neighbor_interface_id)
{  
  i5_dm_legacy_neighbor_type *item = (i5_dm_legacy_neighbor_type *)parent->legacy_list.ll.next;

  while ((item != NULL) && (memcmp(item->LocalInterfaceId, local_interface_id, MAC_ADDR_LEN) || memcmp(item->NeighborInterfaceId, neighbor_interface_id, MAC_ADDR_LEN))) {
    item = item->ll.next;
  }
  return item;
}

int i5DmLegacyNeighborFree(i5_dm_device_type *parent, i5_dm_legacy_neighbor_type *legacy_neighbor)
{
  i5Json1905LegacyNeighborPrint(I5_JSON_ALL_CLIENTS, legacy_neighbor, i5Json_Delete);
#if defined(DMP_DEVICE2_IEEE1905BASELINE_1)
  i5CmsMdmRemoveNetworkTopologyDevLegacyNeighbor(parent, legacy_neighbor);
#endif
  if (i5LlItemFree(&parent->legacy_list, legacy_neighbor) == 0) {
    --parent->LegacyNeighborNumberOfEntries;
    if ( i5DmDeviceIsSelf(parent->DeviceId) ) {
       parent->hasChanged++;
    }
    return 0;
  }
  return -1;
}

int i5DmLegacyNeighborPending(unsigned char *device_id)
{  
  i5_dm_device_type *device;
  i5_dm_legacy_neighbor_type *item;

  if ((device = i5DmDeviceFind(device_id)) == NULL) {
    return -1;
  }
  
  item = (i5_dm_legacy_neighbor_type *)device->legacy_list.ll.next;

  while (item != NULL) {
    item->state = i5DmStatePending;
    item = item->ll.next;
  }
  return 0;
}

int i5DmLegacyNeighborDone(unsigned char *device_id)
{  
  i5_dm_device_type *device;
  i5_dm_legacy_neighbor_type *item;
  i5_dm_legacy_neighbor_type *next;

  if ((device = i5DmDeviceFind(device_id)) == NULL) {
    return -1;
  }

  item = (i5_dm_legacy_neighbor_type *)device->legacy_list.ll.next;
  while (item != NULL) {
    next = item->ll.next;
    if (item->state == i5DmStatePending) {
      i5DmLegacyNeighborFree(device, item);
    }
    item = next;
  }
  return 0;
}

int i5DmLegacyNeighborUpdate(unsigned char *device_id, unsigned char *local_interface_id, unsigned char *neighbor_interface_id)
{
  i5_dm_device_type *device;
  i5_dm_legacy_neighbor_type *neighbor;

  i5TraceInfo(I5_MAC_FMT "\n", I5_MAC_PRM(neighbor_interface_id));
  if ((device = i5DmDeviceFind(device_id)) == NULL) {
    return -1;
  }

  if ((neighbor = i5DmLegacyNeighborFind(device, local_interface_id, neighbor_interface_id)) == NULL) {
    if ((neighbor = i5DmLegacyNeighborNew(device, local_interface_id, neighbor_interface_id)) == NULL) {
      return -1;
    }
    else {
      i5Json1905LegacyNeighborPrint(I5_JSON_ALL_CLIENTS, neighbor, i5Json_Add);
      device->hasChanged++;
    }
  }

#if defined(DMP_DEVICE2_IEEE1905BASELINE_1)
  i5CmsMdmUpdateNetworkTopologyDevLegacyNeighbor(device, neighbor);
#endif

  neighbor->state = i5DmStateDone;
  
  return 0;
}

static int i5DmLegacyNeighborRemove(unsigned char *id)
{
  i5_dm_device_type          *pDmDevice;
  i5_dm_legacy_neighbor_type *pLegNeighbor;

  i5TraceInfo("\n");

  pDmDevice = i5DmDeviceFind(&i5_config.i5_mac_address[0]);
  if ( pDmDevice != NULL ) {
    pLegNeighbor = (i5_dm_legacy_neighbor_type *)pDmDevice->legacy_list.ll.next;
    while ( pLegNeighbor != NULL ) {
       if ( 0 == memcmp(id, pLegNeighbor->NeighborInterfaceId, MAC_ADDR_LEN) ) {
         break;
       }
       pLegNeighbor = pLegNeighbor->ll.next;
    }
    if ( pLegNeighbor != NULL ) {
      i5TraceInfo("Removing legacy neighbor matching 1905 neighbor: " I5_MAC_FMT "\n", I5_MAC_PRM(pLegNeighbor->NeighborInterfaceId));
      i5DmLegacyNeighborFree(pDmDevice, pLegNeighbor);
      i5MessageTopologyNotificationSend();
    }
  }

  return 0;
}

void i5DmSetLocalInterfaceInfoForNeighbor(i5_dm_device_type *parent, i5_dm_1905_neighbor_type *neighbor, char const *localifName, int localifindex)
{
#if defined(SUPPORT_IEEE1905_FM)   
  i5_dm_interface_type *pLocalInterface;
#endif

  if (!i5DmDeviceIsSelf(parent->DeviceId)) {
    return;
  }

  if ((NULL == localifName) || (0 == strlen(localifName))) {
    i5TraceAssert(1==0);    
    return;
  }

  i5Trace("Neighbor " I5_MAC_DELIM_FMT " LocalIf: %s (%d)\n", 
          I5_MAC_PRM(neighbor->Ieee1905Id), localifName, localifindex);

  snprintf(neighbor->localIfname, sizeof(neighbor->localIfname), localifName);
  neighbor->localIfname[sizeof(neighbor->localIfname)-1] = '\0';
  neighbor->localIfindex = localifindex;

#if defined(SUPPORT_IEEE1905_FM)
  pLocalInterface = i5DmInterfaceFind(parent, neighbor->LocalInterfaceId);
  if (i5DmIsInterfaceWireless(pLocalInterface->MediaType) || i5DmIsInterfacePlc(pLocalInterface->MediaType, pLocalInterface->netTechOui)) {
    i5FlowManagerAddConnectionIndex(neighbor->Ieee1905Id, neighbor->localIfindex);
  }
#endif /* defined(SUPPORT_IEEE1905_FM) */
}

i5_dm_1905_neighbor_type *i5Dm1905NeighborNew(i5_dm_device_type *parent, unsigned char *local_interface_id, unsigned char *neighbor1905_al_mac_address)
{
  i5_dm_1905_neighbor_type *newNeighbor = (i5_dm_1905_neighbor_type *)malloc(sizeof(i5_dm_1905_neighbor_type));

  i5Trace(I5_MAC_FMT "\n", I5_MAC_PRM(neighbor1905_al_mac_address));
  if (NULL != newNeighbor) {
    memset(newNeighbor, 0, sizeof(i5_dm_1905_neighbor_type));
    i5LlItemAdd(parent, &parent->neighbor1905_list, newNeighbor);
    ++parent->Ieee1905NeighborNumberOfEntries;
    memcpy(newNeighbor->LocalInterfaceId, local_interface_id, MAC_ADDR_LEN);
    memcpy(newNeighbor->Ieee1905Id, neighbor1905_al_mac_address, MAC_ADDR_LEN);
#if defined(SUPPORT_IEEE1905_FM)
    i5FlowManagerProcessNewNeighbor (newNeighbor);
#endif /* defined(SUPPORT_IEEE1905_FM) */
    i5Dm1905NeighborUpdateIntermediateBridgeFlag(parent, newNeighbor, 0);
  }
  return newNeighbor;
}

/* returns (0 and up) the number of interfaces found
 * returns -1 if the local mac isn't in the device list
 * (It will never return more actual interfaces than "maxInterfaces", 
 *    but the return *value* of the function will be higher if more interfaces are available)
 */
int i5DmGetInterfacesWithNeighbor(unsigned char const *neighbor_al_mac, 
                                  unsigned char * local_interface_mac_addrs, unsigned char * neighbor_interface_mac_addrs, int maxInterfaces)
{
  i5_dm_device_type * currentDevice = i5DmDeviceFind(i5_config.i5_mac_address);
  int totalInterfacesFound = 0;

  i5_dm_1905_neighbor_type * currentNeighbor = &currentDevice->neighbor1905_list;
  while (currentNeighbor != NULL) {
    if (memcmp (currentNeighbor->Ieee1905Id, neighbor_al_mac, MAC_ADDR_LEN) == 0) {
      if (totalInterfacesFound < maxInterfaces) {
        memcpy(local_interface_mac_addrs, currentNeighbor->LocalInterfaceId, MAC_ADDR_LEN);
        local_interface_mac_addrs += MAC_ADDR_LEN;
        memcpy(neighbor_interface_mac_addrs, currentNeighbor->NeighborInterfaceId, MAC_ADDR_LEN);
        neighbor_interface_mac_addrs += MAC_ADDR_LEN;
      }
      totalInterfacesFound ++;
    }
    // check next neighbor
    currentNeighbor = (i5_dm_1905_neighbor_type *)currentNeighbor->ll.next;
  }

   return totalInterfacesFound;
}

void i5DmProcessLocalInterfaceChange (i5_dm_device_type *parent, unsigned char * localInterfaceId)
{
   i5_dm_interface_type *dmInterface = NULL;
   unsigned char macAddressList[I5_DM_MAX_LINK_METRICS_LIST_SIZE][MAC_ADDR_LEN];
   unsigned char numMacsInList = 0;

   i5Trace("called for interfaceId " I5_MAC_FMT "\n", I5_MAC_PRM(localInterfaceId) );

   /* Go through all of our neighbors */
   i5_dm_1905_neighbor_type * currNeighbor = (i5_dm_1905_neighbor_type *)&parent->neighbor1905_list.ll.next;
   
   while (currNeighbor != NULL) {
      if (memcmp(currNeighbor->LocalInterfaceId, localInterfaceId, MAC_ADDR_LEN) == 0){
         /* add to MAC address list */
         if (numMacsInList >= I5_DM_MAX_LINK_METRICS_LIST_SIZE) {
            i5Trace("Too many neighbors for MAC Address List (more than %d)\n", I5_DM_MAX_LINK_METRICS_LIST_SIZE);
         } else {
            memcpy(macAddressList[numMacsInList], currNeighbor->NeighborInterfaceId, MAC_ADDR_LEN);
            numMacsInList ++;
         }
      }      
      currNeighbor = currNeighbor->ll.next;
   }

   i5Trace("MAC Address List ready: %d addresses in list\n", numMacsInList);
   dmInterface = i5DmInterfaceFind(parent, localInterfaceId);
   if ((dmInterface != NULL) && (dmInterface->i5MacAddrDeliver != NULL)) {
      dmInterface->i5MacAddrDeliver( &macAddressList[0][0], numMacsInList);
   }
   else {
      i5Trace("Nowhere to deliver list\n");
   }
}

void i5DmRetryPlcRegistry(void)
{ 
  unsigned char *localId = i5DmInterfaceGetLocalPlcInterface()->InterfaceId;
  i5_dm_device_type *selfDevice = i5DmDeviceFind(i5_config.i5_mac_address);
  if (NULL == selfDevice) {
    return;
  }
  i5Trace("adding all PLC registries\n");
  i5DmProcessLocalInterfaceChange(selfDevice, localId);
}


i5_dm_1905_neighbor_type *i5Dm1905FindNeighborByRemoteInterface(i5_dm_device_type const *parent, unsigned char const *remote_interface_id)
{
  i5_dm_1905_neighbor_type *item = (i5_dm_1905_neighbor_type *)parent->neighbor1905_list.ll.next;

  while ((item != NULL) && memcmp(item->NeighborInterfaceId, remote_interface_id, MAC_ADDR_LEN)) {
    item = item->ll.next;
  }
  return item;
}

i5_dm_1905_neighbor_type *i5Dm1905NeighborFind(i5_dm_device_type *parent, unsigned char *local_interface_id, unsigned char *neighbor1905_al_mac_address)
{  
  i5_dm_1905_neighbor_type *item = (i5_dm_1905_neighbor_type *)parent->neighbor1905_list.ll.next;

  while ((item != NULL) && (memcmp(item->LocalInterfaceId, local_interface_id, MAC_ADDR_LEN) || memcmp(item->Ieee1905Id, neighbor1905_al_mac_address, MAC_ADDR_LEN))) {
    item = item->ll.next;
  }
  return item;
}

int i5Dm1905NeighborFree(i5_dm_device_type *parent, i5_dm_1905_neighbor_type *neighbor_1905)
{
  unsigned char local_interface_id[MAC_ADDR_LEN];

  if ( i5DmDeviceIsSelf(parent->DeviceId) ) {
    if ( neighbor_1905->bridgeDiscoveryTimer ) {
      i5TimerFree(neighbor_1905->bridgeDiscoveryTimer);
    }
#if defined(DMP_DEVICE2_IEEE1905BASELINE_1)
    i5CmsMdmLocalNeighborRemove(neighbor_1905);
#endif
  }

#if defined(DMP_DEVICE2_IEEE1905BASELINE_1)
  i5CmsMdmRemoveNetworkTopologyDevNeighbor(parent, neighbor_1905);
#endif

  i5Trace(I5_MAC_FMT " " I5_MAC_FMT "\n", I5_MAC_PRM(parent->DeviceId), I5_MAC_PRM(neighbor_1905->Ieee1905Id));
  i5Json1905NeighborPrint(I5_JSON_ALL_CLIENTS, neighbor_1905, i5Json_Delete);
#if defined(SUPPORT_IEEE1905_FM)
  i5FlowManagerProcessNeighborRemoved(neighbor_1905);
#endif /* defined(SUPPORT_IEEE1905_FM) */

  memcpy (local_interface_id, neighbor_1905->LocalInterfaceId, MAC_ADDR_LEN);
  if (i5LlItemFree(&parent->neighbor1905_list, neighbor_1905) == 0) {
    --parent->Ieee1905NeighborNumberOfEntries;
    if (i5DmDeviceIsSelf(parent->DeviceId)) {
      /* update the MAC address list based on media type*/
      i5DmProcessLocalInterfaceChange (parent, local_interface_id);
      parent->hasChanged++;
    }
    return 0;
  }
  return -1;
}

void i5Dm1905NeighborFreeAllLinksRemoteDevice(unsigned char *remoteDeviceAlMac)
{
  i5Trace(I5_MAC_DELIM_FMT "\n", I5_MAC_PRM(remoteDeviceAlMac));
  i5_dm_device_type *localDevice = i5DmDeviceFind(i5_config.i5_mac_address);
  i5_dm_1905_neighbor_type *neigh = localDevice->neighbor1905_list.ll.next;
  while (neigh) {
    i5_dm_1905_neighbor_type *nextNeigh = neigh->ll.next;
    if (memcmp(neigh->Ieee1905Id, remoteDeviceAlMac, MAC_ADDR_LEN) == 0) {
      i5Dm1905NeighborFree(localDevice, neigh);
    }
    neigh = nextNeigh;
  }

}

int i5Dm1905NeighborPending(unsigned char *device_id)
{  
  i5_dm_device_type *device;
  i5_dm_1905_neighbor_type *item;

  i5Trace("\n");
  if ((device = i5DmDeviceFind(device_id)) == NULL) {
    return -1;
  }
  
  item = (i5_dm_1905_neighbor_type *)device->neighbor1905_list.ll.next;

  while (item != NULL) {
    item->state = i5DmStatePending;
    item = item->ll.next;
  }
  return 0;
}

int i5Dm1905NeighborDone(unsigned char *device_id)
{  
  i5_dm_device_type *device;
  i5_dm_1905_neighbor_type *item;
  i5_dm_1905_neighbor_type *next;

  i5Trace("\n");
  if ((device = i5DmDeviceFind(device_id)) == NULL) {
    return -1;
  }

  item = (i5_dm_1905_neighbor_type *)device->neighbor1905_list.ll.next;
  while (item != NULL) {
    next = item->ll.next;
    if (item->state == i5DmStatePending) {
      i5Dm1905NeighborFree(device, item);
    }
    item = next;
  }
  return 0;
}

void i5DmUpdateNeighborLinkMetrics(void *arg)
{
  i5_dm_device_type *pDevice;

  i5TraceInfo("\n");

  i5TimerFree(i5_dm_network_topology.pLinkMetricTimer);
  
  pDevice = i5DmDeviceFind(i5_config.i5_mac_address);
  if ( NULL != pDevice ) {
    i5_dm_interface_type *pInterface = pDevice->interface_list.ll.next;
    while ( pInterface != NULL ) {
      /* update the metrics for each Ethernet interface and then update 
         the corresponding neighbor entries */
      if ( i5DmIsInterfaceEthernet(pInterface->MediaType) ) {
        i5_socket_type *pif = i5SocketFindDevSocketByAddr(pInterface->InterfaceId, NULL);
        while ( pif != NULL ) {
          i5TraceModuleInfo(i5TraceEthStat, "Update stats for %s (%d)\n", i5SocketGetIfName(pif), i5SocketGetIfIndex(pif));
          if ( pif->u.sll.pInterfaceCtx != NULL ) {
            long capacity;
            long dataRate;
            int rt = i5EthStatGetDataRateAndCapacity(pif->u.sll.pInterfaceCtx, &dataRate, &capacity);
            if (rt >= 0) {
              i5_dm_1905_neighbor_type *pNeighbor = pDevice->neighbor1905_list.ll.next;
              const long MBit = 1000000L;
              i5TraceModuleInfo(i5TraceEthStat, "capacity=%ld, dataRate=%ld\n", capacity, dataRate);
              while (pNeighbor != NULL) {
                if ( pNeighbor->localIfindex == i5SocketGetIfIndex(pif)) {
                  i5Dm1905NeighborBandwidthUpdate(pNeighbor, (capacity*8)/MBit, ((capacity - dataRate)*8)/MBit, 0, pDevice->DeviceId);
                }
                pNeighbor = pNeighbor->ll.next;
              }
            }
          }
          pif = i5SocketFindDevSocketByAddr(pInterface->InterfaceId, pif);
        }
      }

#if defined(WIRELESS)
      /* collect WL neigbour devices and query WL for metrics*/
      if ( i5DmIsInterfaceWireless(pInterface->MediaType) ) {
         unsigned char macAddressList[I5_DM_MAX_LINK_METRICS_LIST_SIZE][MAC_ADDR_LEN];
         int count = 0;

         i5_dm_1905_neighbor_type *pNeighbor = pDevice->neighbor1905_list.ll.next;
         while (pNeighbor != NULL) {
           if ( 0 == memcmp(pNeighbor->LocalInterfaceId, pInterface->InterfaceId, MAC_ADDR_LEN) ) {
             memcpy(&macAddressList[count][0], pNeighbor->NeighborInterfaceId, MAC_ADDR_LEN);
             count++;
           }
           pNeighbor = pNeighbor->ll.next;
         }
         if ( count > 0 ) {
           i5wlmMetricUpdateLinkMetrics(pInterface->wlParentName, count, &macAddressList[0][0]);
         }
      }
#endif
      pInterface = pInterface->ll.next;
    }
  }

  i5_dm_network_topology.pLinkMetricTimer = i5TimerNew(1000, i5DmUpdateNeighborLinkMetrics, NULL);
}

int i5Dm1905NeighborUpdate(     unsigned char *device_id, 
                                unsigned char *local_interface_id, 
                                unsigned char *neighbor1905_al_mac_address, 
                                unsigned char *neighbor1905_interface_id, 
                                unsigned char *intermediate_legacy_bridge,
                                char          *localifName,
                                int            localifindex,
                                unsigned char  createNeighbor)
{
  i5_dm_device_type *device;
  i5_dm_1905_neighbor_type *neighbor;

  if (neighbor1905_interface_id) {
    i5Trace("Dev_id " I5_MAC_FMT " Loc IF " I5_MAC_FMT
            " Nei_id " I5_MAC_FMT " Nei IF " I5_MAC_FMT "\n",
      I5_MAC_PRM(device_id), I5_MAC_PRM(local_interface_id),
      I5_MAC_PRM(neighbor1905_al_mac_address), I5_MAC_PRM(neighbor1905_interface_id)
      );
  } else {
    i5Trace("Dev_id " I5_MAC_FMT " Loc IF " I5_MAC_FMT
            " Nei_id " I5_MAC_FMT " Nei IF  { NULL } \n", 
      I5_MAC_PRM(device_id), I5_MAC_PRM(local_interface_id),
      I5_MAC_PRM(neighbor1905_al_mac_address)
      );  
  }
  if ((device = i5DmDeviceFind(device_id)) == NULL) {
    return -1;
  }

  neighbor = i5Dm1905NeighborFind(device, local_interface_id, neighbor1905_al_mac_address);
  if ( neighbor == NULL ) {
    if (0 == createNeighbor) {
      return 0;
    }

    i5Trace("Creating New Neighbor\n");
    if ((neighbor = i5Dm1905NeighborNew(device, local_interface_id, neighbor1905_al_mac_address)) == NULL) {
      return -1;
    }
    else {
      i5Trace("Device has new neighbor\n");
      device->hasChanged++;
    }
  }

  if ( neighbor1905_interface_id && memcmp(neighbor1905_interface_id, neighbor->NeighborInterfaceId, MAC_ADDR_LEN)) {
    memcpy(neighbor->NeighborInterfaceId, neighbor1905_interface_id, MAC_ADDR_LEN);
    i5DmLegacyNeighborRemove(neighbor1905_interface_id);
    /* for the local device this is technically a device change but the neighbor 
       interface id is not in the topology response so no need to flag the change */
    if (i5DmDeviceIsSelf(device_id)) {
      i5DmSetLocalInterfaceInfoForNeighbor (neighbor->ll.parent, neighbor, localifName, localifindex);
      /* update the MAC address list based on media type*/
      i5DmProcessLocalInterfaceChange (device, local_interface_id);
    }
    if (!i5DmIsMacNull(neighbor->NeighborInterfaceId)) {
      i5Json1905NeighborPrint(I5_JSON_ALL_CLIENTS, neighbor, i5Json_Add);
    }
  }

  if (intermediate_legacy_bridge) {
   i5Dm1905NeighborUpdateIntermediateBridgeFlag(device, neighbor, *intermediate_legacy_bridge);
  }

  i5DmFillNeighborInterfaceId();
  neighbor->state = i5DmStateDone;

#if defined(DMP_DEVICE2_IEEE1905BASELINE_1)
  if (i5DmDeviceIsSelf(device_id)) {
    i5CmsMdmLocalNeighborUpdate(neighbor);
  }
  i5CmsMdmUpdateNetworkTopologyDevNeighbor(device, neighbor);
#endif

  return 0;
}

/* For a given remote interface MAC, this function searches through the neighbors of the local device
   and returns the neighbor structure that corresponds to the given remote MAC */
i5_dm_1905_neighbor_type *i5Dm1905GetLocalNeighbor (unsigned char const *neighbor1905_interface_id)
{
   i5_dm_device_type const *selfDevice = i5DmDeviceFind(i5_config.i5_mac_address);
   if (NULL == selfDevice) {
     return NULL;
   }
   i5_dm_1905_neighbor_type *neighbor = i5Dm1905FindNeighborByRemoteInterface(selfDevice, neighbor1905_interface_id);
   if (NULL == neighbor) {
     return NULL;
   }

   i5TraceInfo("%02x:%02x:%02x:%02x:%02x:%02x\n",
      neighbor1905_interface_id[0], neighbor1905_interface_id[1], neighbor1905_interface_id[2],
      neighbor1905_interface_id[3], neighbor1905_interface_id[4], neighbor1905_interface_id[5]);

   return neighbor;
}

int i5Dm1905NeighborBandwidthUpdate ( i5_dm_1905_neighbor_type *neighbor, 
                                      unsigned short MacThroughputCapacity,
                                      unsigned short availableThroughputCapacity,
                                      unsigned int rxBytesCumulative,
                                      unsigned char* sourceAlMac)
{
    /* Note: capacities are in Mbit/s here */

   if (!neighbor) {
      return -1;
   }      
   neighbor->MacThroughputCapacity = MacThroughputCapacity;
   neighbor->availableThroughputCapacity = availableThroughputCapacity;
   neighbor->prevRxBytes = neighbor->latestRxBytes;
   neighbor->latestRxBytes = rxBytesCumulative;

   if (neighbor->ignoreLinkMetricsCountdown) {
      neighbor->ignoreLinkMetricsCountdown --;
      i5Trace("Countdown now %d\n",neighbor->ignoreLinkMetricsCountdown);
      if (!neighbor->ignoreLinkMetricsCountdown) {
         i5_dm_network_topology.updateStpNeeded = 1;
      }
   } 
   
   if (i5DmDeviceIsSelf(sourceAlMac)) {
     /* Data from our local drivers is given preference */
     i5Json1905NeighborUpdatePrint(I5_JSON_ALL_CLIENTS, neighbor, NULL);
   } 
   else if (i5DmDeviceIsSelf(neighbor->Ieee1905Id)) {
     /* Don't listen to another device telling us about ourselves */
   } 
   else {
     /* This is a link between two devices which aren't us, so take an average of both ends */
     i5_dm_device_type *symmDevice = i5DmDeviceFind(neighbor->Ieee1905Id);
     i5_dm_1905_neighbor_type *symmNeighbor = NULL;
     int success = 0;

     if (symmDevice != NULL) {
       symmNeighbor = i5Dm1905NeighborFind(symmDevice, neighbor->NeighborInterfaceId, sourceAlMac);
       if (symmNeighbor != NULL) {
         i5Json1905NeighborUpdatePrint(I5_JSON_ALL_CLIENTS, neighbor, symmNeighbor);     
         success = 1;
       }
     }

     if (0 == success) {
       /* couldn't do the average, so take what was given */
       i5Json1905NeighborUpdatePrint(I5_JSON_ALL_CLIENTS, neighbor, NULL);
     }
   }

#if defined(SUPPORT_IEEE1905_FM) && defined(SUPPORT_FBCTL)
   i5FlowManagerCheckNeighborForOverload(neighbor);
#endif /* defined(SUPPORT_IEEE1905_FM) */
   
   return 0;
}

/* Given a neighbor device, return the interface structure of the interface that connects to it */
i5_dm_interface_type *i5Dm1905GetLocalInterface(i5_dm_1905_neighbor_type const *neighbor)
{
   i5_dm_device_type* devSelf = i5DmDeviceFind(i5_config.i5_mac_address);
   if (NULL == devSelf) {
      return NULL;
   }
   return i5DmInterfaceFind(devSelf, neighbor->LocalInterfaceId);
}

i5_dm_bridging_tuple_info_type *i5DmBridgingTupleNew(i5_dm_device_type *parent, char *ifname, unsigned char tuple_num_macaddrs, unsigned char *forwarding_interface_list)
{
  i5_dm_bridging_tuple_info_type *new = (i5_dm_bridging_tuple_info_type *)malloc(sizeof(i5_dm_bridging_tuple_info_type));

  i5Trace("\n");
  if (NULL != new) {
    memset(new, 0, sizeof(i5_dm_bridging_tuple_info_type));
    i5LlItemAdd(parent, &parent->bridging_tuple_list, new);
    ++parent->BridgingTuplesNumberOfEntries;
    if ( ifname ) {
      memcpy(new->ifname, ifname, I5_MAX_IFNAME);
    }
    new->forwardingInterfaceListNumEntries = tuple_num_macaddrs;
    memcpy(new->ForwardingInterfaceList, forwarding_interface_list, tuple_num_macaddrs*MAC_ADDR_LEN);
#if defined(DMP_DEVICE2_IEEE1905BASELINE_1)
    i5CmsMdmUpdateBridgingTuple(parent, new);
#endif

    return new;
  }
  return NULL;
}

int i5DmBridgingTupleFree(i5_dm_device_type *parent, i5_dm_bridging_tuple_info_type *bridging_tuple)
{
  if ( NULL == bridging_tuple) {
    return -1;
  }

#if defined(DMP_DEVICE2_IEEE1905BASELINE_1)
  i5CmsMdmDeleteBridgingTuple(parent, bridging_tuple);
#endif

  if (i5LlItemFree(&parent->bridging_tuple_list, bridging_tuple) == 0) {
    --parent->BridgingTuplesNumberOfEntries;
    return 0;
  }
  return -1;
}

int i5DmBridgingTupleUpdate(unsigned char *device_id, int version, char *ifname, unsigned char tuple_num_macaddrs, unsigned char *bridging_tuple_list)
{
  i5_dm_device_type *device;
  i5_dm_bridging_tuple_info_type *pdmbrtuple;

  i5Trace("ifname %s, numentries %d\n", ifname ? ifname : "NULL", tuple_num_macaddrs );
  device = i5DmDeviceFind(device_id);
  if (device == NULL) {
    return -1;
  }

  if ( 0 == tuple_num_macaddrs ) {
    pdmbrtuple = (i5_dm_bridging_tuple_info_type *)device->bridging_tuple_list.ll.next; 
    while(pdmbrtuple != NULL) {
      i5_dm_bridging_tuple_info_type *nexttuple = pdmbrtuple->ll.next;
      /* NULL ifname means delete all, otherwise delete matching entry */
      if ( (NULL == ifname) || (0 == strcmp(ifname, pdmbrtuple->ifname)) ) {
        if ( i5DmDeviceIsSelf(device_id) ) {
          device->hasChanged++;
        }
        i5DmBridgingTupleFree(device, pdmbrtuple);
      }
      pdmbrtuple = nexttuple;
    }
  }
  else {
    if ( tuple_num_macaddrs > I5_DM_BRIDGE_TUPLE_MAX_INTERFACES ) {
      return -1;
    }

    /* find a matching tuple */
    if ( NULL == ifname ) {
      i5_dm_bridging_tuple_info_type *savedTuple = NULL;
      /* NULL ifname means this is not the local device.
         A matching device is one that is in the pending state.
         To maintain the order, we want the entry that is closest to the
         last entry of the list since this was the first one added */
      pdmbrtuple = (i5_dm_bridging_tuple_info_type *)device->bridging_tuple_list.ll.next;
      while (pdmbrtuple != NULL) {
        if ( pdmbrtuple->state == i5DmStatePending ) {
          savedTuple = pdmbrtuple;
        }
        pdmbrtuple = pdmbrtuple->ll.next;
      }
      pdmbrtuple = savedTuple;
    }
    else {
      pdmbrtuple = (i5_dm_bridging_tuple_info_type *)device->bridging_tuple_list.ll.next;
      while (pdmbrtuple != NULL) {
        if (0 == strcmp(ifname, pdmbrtuple->ifname)) {
          break;
        }
        pdmbrtuple = pdmbrtuple->ll.next;
      }
    }

    /* found an entry to update */
    if (pdmbrtuple) {
      int i, j;
      int found;
      int changed = 0;

      for (i = 0; i < tuple_num_macaddrs; i++ ) {
        found = 0;
        for ( j = 0; j < pdmbrtuple->forwardingInterfaceListNumEntries; j++) {
          if (0 == memcmp(&bridging_tuple_list[i*MAC_ADDR_LEN], &pdmbrtuple->ForwardingInterfaceList[j*MAC_ADDR_LEN], MAC_ADDR_LEN)) {
            found = 1;
            break;
          }
        }
        if ( found == 0 ) {
          /* interface added */
          changed = 1;
        }
      }
 
      for (i = 0; i < pdmbrtuple->forwardingInterfaceListNumEntries; i++ ) {
        found = 0;
        for ( j = 0; j < tuple_num_macaddrs; j++) {
          if (0 == memcmp(&pdmbrtuple->ForwardingInterfaceList[i*MAC_ADDR_LEN], &bridging_tuple_list[j*MAC_ADDR_LEN], MAC_ADDR_LEN)) {
            found = 1;
            break;
          }
        }
        if ( found == 0 ) {
          /* interface removed */
          changed = 1;
        }
      }

      pdmbrtuple->state = i5DmStateDone;
      if ( changed ) {
        /* update entry */
        for (i = 0; i < tuple_num_macaddrs; i++ ) {
          memcpy(&pdmbrtuple->ForwardingInterfaceList[i*MAC_ADDR_LEN], &bridging_tuple_list[i*MAC_ADDR_LEN], MAC_ADDR_LEN);
        }
        pdmbrtuple->forwardingInterfaceListNumEntries = tuple_num_macaddrs;
        if ( i5DmDeviceIsSelf(device_id) ) {
          device->hasChanged++;
        }
#if defined(DMP_DEVICE2_IEEE1905BASELINE_1)
        i5CmsMdmUpdateBridgingTuple(device, pdmbrtuple);
#endif
      }
    }
    else {
      i5DmBridgingTupleNew(device, ifname, tuple_num_macaddrs, bridging_tuple_list);
      if ( i5DmDeviceIsSelf(device_id) ) {
        device->hasChanged++;
      }
    }
  }
  return 0;
}

int i5DmBridgingTuplePending(unsigned char *device_id)
{
  i5_dm_device_type *pdevice;
  i5_dm_bridging_tuple_info_type *item;

  if ((pdevice = i5DmDeviceFind(device_id)) == NULL) {
    return -1;
  }
  
  item = (i5_dm_bridging_tuple_info_type *)pdevice->bridging_tuple_list.ll.next;
  while (item != NULL) {
    item->state = i5DmStatePending;
    item = item->ll.next;
  }
  return 0;
}

int i5DmBridgingTupleDone(unsigned char *device_id)
{  
  i5_dm_device_type *pdevice;
  i5_dm_bridging_tuple_info_type *item;
  i5_dm_bridging_tuple_info_type *next;

  if ((pdevice = i5DmDeviceFind(device_id)) == NULL) {
    return -1;
  }

  item = (i5_dm_bridging_tuple_info_type *)pdevice->bridging_tuple_list.ll.next;
  while (item != NULL) {
    next = item->ll.next;
    if (item->state == i5DmStatePending) {
      i5DmBridgingTupleFree(pdevice, item);
    }
    item = next;
  }
  return 0;
}

void i5Dm1905NeighborBridgeDiscoveryTimeout(void *arg)
{
  i5_dm_device_type* selfDevice = i5DmDeviceFind(i5_config.i5_mac_address);
  if (NULL == selfDevice) {
    return;
  }
  i5_dm_1905_neighbor_type *neighbor = (i5_dm_1905_neighbor_type *)arg;

  i5Dm1905NeighborUpdateIntermediateBridgeFlag(selfDevice, neighbor, 1);
}

void i5Dm1905NeighborUpdateIntermediateBridgeFlag(i5_dm_device_type *device, i5_dm_1905_neighbor_type *neighbor, unsigned char bridgeFlag)
{
  if ( i5DmDeviceIsSelf(device->DeviceId) ) {
    if ( neighbor->bridgeDiscoveryTimer ) {
      i5TimerFree(neighbor->bridgeDiscoveryTimer);
    }
    neighbor->bridgeDiscoveryTimer = i5TimerNew(I5_DM_NEIGHBOR_BRIDGE_DISCOVERY_TIMEOUT, i5Dm1905NeighborBridgeDiscoveryTimeout, neighbor);
  }
  
  if ( neighbor->IntermediateLegacyBridge != bridgeFlag) {
    neighbor->IntermediateLegacyBridge = bridgeFlag;
    device->hasChanged++;
  }
}

i5_dm_interface_type *i5DmInterfaceGetLocalPlcInterface(void)
{
  i5_dm_device_type* selfDevice = i5DmDeviceFind(i5_config.i5_mac_address);
  if (NULL == selfDevice) {
    return NULL;
  }
  i5_dm_interface_type *currInterface = (i5_dm_interface_type *)selfDevice->interface_list.ll.next;

  while (currInterface != NULL) {
    if (i5DmIsInterfacePlc(currInterface->MediaType, currInterface->netTechOui)) {
      return currInterface;      
    }
    currInterface = currInterface->ll.next;
  }
  return NULL;
}

/* parentDev, destinterface and interface_id MUST NOT be null
 * media_type MUST be set (i.e. GEN PHY calls MUST set it to 0xFFFF even if that's already known)
 */
void i5DmInterfaceCopyInfo(i5_dm_device_type* parentDev, i5_dm_interface_type *destInterface, 
                           unsigned char *interface_id, unsigned short media_type, 
                           unsigned char const *media_specific_info, unsigned int media_specific_info_size,
                           unsigned char const *pNetTechOui, unsigned char const *pNetTechVariant, unsigned char const *pNetTechName, unsigned char const *url,
                           i5MacAddressDeliveryFunc deliverFunc)
{
  i5Trace(I5_MAC_DELIM_FMT " %s %d %s\n", I5_MAC_PRM(interface_id), 
          i5DmIsInterfaceWireless(media_type) ? "WL" : "NOT-WL",
          destInterface->Status,
          (destInterface->Status == IF_OPER_DOWN) ? "DOWN" : "UP" );
  if (destInterface) {
    unsigned char mediaTypeChanged = 0;
    
    memcpy(destInterface->InterfaceId, interface_id, MAC_ADDR_LEN);
    if (destInterface->MediaType != media_type) {
      destInterface->MediaType = media_type;
      mediaTypeChanged = 1;
    }
    if (deliverFunc == NULL) {
      if (destInterface->i5MacAddrDeliver == NULL) {
        i5Trace("  DeliveryFunc not set.\n");
      }
    }
    else {
      destInterface->i5MacAddrDeliver = deliverFunc;
    }

    if ((NULL != media_specific_info) && (media_specific_info_size > 0)) {
      if (media_specific_info_size > I5_MEDIA_SPECIFIC_INFO_MAX_SIZE) {
        printf("Error media_specific_info_size > I5_MEDIA_SPECIFIC_INFO_MAX_SIZE\n");
      } else {
        destInterface->MediaSpecificInfoSize = media_specific_info_size;
        memcpy(destInterface->MediaSpecificInfo, media_specific_info, media_specific_info_size);
      }
    }
    if (pNetTechOui) {
      memcpy(destInterface->netTechOui, pNetTechOui, I5_PHY_INTERFACE_NETTECHOUI_SIZE );
    }
    if (pNetTechVariant) {
      destInterface->netTechVariant = *pNetTechVariant;
    }
    if (pNetTechName) {
      memcpy(destInterface->netTechName, pNetTechName, I5_PHY_INTERFACE_NETTECHNAME_SIZE );
    }
    if (url) {
      strncpy((char *)destInterface->url, (char *)url, sizeof(destInterface->url));
    }
    if (destInterface->state != i5DmStateNew) {
      if (mediaTypeChanged == 1) {
        /* This will cause only the label (the ifname) to be updated */
        i5JsonUpdateInterfacePrint(I5_JSON_ALL_CLIENTS, destInterface);
        if ( i5DmDeviceIsSelf(parentDev->DeviceId) ) {
          parentDev->hasChanged++;
        }
      }
    }
    else {
      i5JsonInterfacePrint(I5_JSON_ALL_CLIENTS, parentDev, destInterface, i5Json_Add);
    }
#if defined(SUPPORT_IEEE1905_FM) && defined(SUPPORT_IEEE1905_AUTO_WDS)
    if (i5DmIsInterfaceWireless(media_type) && (destInterface->Status != IF_OPER_DOWN ) ) {
      i5FlowManagerProcessWirelessUp();
    }
#endif
    if (i5DmDeviceIsSelf(parentDev->DeviceId) ) {
      i5DmLegacyNeighborRemove(interface_id);
    }
  }
}

i5_dm_interface_type *i5DmInterfaceNew(i5_dm_device_type *parent)
{
  i5_dm_interface_type *newIf = (i5_dm_interface_type *)malloc(sizeof(i5_dm_interface_type));

  if (NULL != newIf) {
    memset(newIf, 0, sizeof(i5_dm_interface_type));
    i5LlItemAdd(parent, &parent->interface_list, newIf);
    ++parent->InterfaceNumberOfEntries;
    return newIf;
  }

  return NULL;
}

i5_dm_interface_type *i5DmInterfaceFind(i5_dm_device_type const *parent, unsigned char const *interface_id)
{  
  i5_dm_interface_type *item = (i5_dm_interface_type *)parent->interface_list.ll.next;

  while ((item != NULL) && (memcmp(item->InterfaceId, interface_id, MAC_ADDR_LEN))) {
    item = item->ll.next;
  }
  return item;
}

int i5DmInterfaceFree(i5_dm_device_type *parent, i5_dm_interface_type *interface)
{
  i5JsonInterfacePrint(I5_JSON_ALL_CLIENTS, parent, interface, i5Json_Delete);
#if defined(DMP_DEVICE2_IEEE1905BASELINE_1)
  i5CmsMdmRemoveNetworkTopologyDevIfc(parent, interface);
#endif
  if (i5LlItemFree(&parent->interface_list, interface) == 0) {
    --parent->InterfaceNumberOfEntries;
    if ( i5DmDeviceIsSelf(parent->DeviceId) ) {
      parent->hasChanged++;
    }
    return 0;
  }
  return -1;
}

void i5DmInterfacePending(unsigned char *device_id)
{  
  i5_dm_device_type *device;
  i5_dm_interface_type *interface;

  if ((device = i5DmDeviceFind(device_id)) != NULL) {
    interface = (i5_dm_interface_type *)device->interface_list.ll.next;
    while (interface != NULL) {
      interface->state = i5DmStatePending;
      interface = interface->ll.next;
    }
  }
}

void i5DmInterfaceDone(unsigned char *device_id)
{  
  i5_dm_device_type *device;
  i5_dm_interface_type *interface, *next;

  if ((device = i5DmDeviceFind(device_id)) != NULL) {
    interface = (i5_dm_interface_type *)device->interface_list.ll.next;
    while (interface != NULL) {
      next = interface->ll.next;
      if (interface->state == i5DmStatePending) {
        i5DmInterfaceFree(device, interface);
      }
      interface = next;
    }
  }
}

int i5DmInterfaceUpdate(unsigned char *device_id, unsigned char *interface_id, int version, unsigned short media_type, 
                        unsigned char const *media_specific_info, unsigned int media_specific_info_size,
                        i5MacAddressDeliveryFunc deliverFunc, char const *ifname, unsigned char status)
{
  i5_dm_device_type *pdevice;
  i5_dm_interface_type *pinterface;

  i5Trace(I5_MAC_FMT ", ifname (%s)\n", I5_MAC_PRM(interface_id), ifname ? ifname : "NULL");

  pdevice = i5DmDeviceFind(device_id);
  if ( pdevice == NULL ) {
    return -1;
  }
  
  if ((pinterface = i5DmInterfaceFind(pdevice, interface_id)) == NULL) {
    if ((pinterface = i5DmInterfaceNew(pdevice)) == NULL) {
      return -1;
    }
    pdevice->hasChanged++;
    pinterface->state = i5DmStateNew;
    pinterface->Status = status;
#if defined(WIRELESS)
    if ( (ifname != NULL) && i5DmDeviceIsSelf(device_id) && i5DmIsInterfaceWireless(media_type) ) {
      char wlname[I5_MAX_IFNAME];
      char *wlparent;
      wlparent = i5WlcfgGetWlParentInterface(ifname, &wlname[0]);
      strncpy(pinterface->wlParentName, wlparent, I5_MAX_IFNAME-1);
      pinterface->wlParentName[I5_MAX_IFNAME-1] = '\0';
    }
#endif
  }

  i5DmInterfaceCopyInfo(pdevice, pinterface, interface_id, media_type, 
                        media_specific_info, media_specific_info_size,  
                        NULL, NULL, NULL, NULL,
                        deliverFunc);

#if defined(DMP_DEVICE2_IEEE1905BASELINE_1)
  if (i5DmDeviceIsSelf(device_id)) {
    i5CmsMdmLocalInterfaceUpdate(pinterface);
  }
  i5CmsMdmUpdateNetworkTopologyDevIfc(pdevice, pinterface);
#endif

  pinterface->state = i5DmStateDone;

  return 0;
}

int i5DmInterfacePhyUpdate(unsigned char *device_id, unsigned char *interface_id,
                           unsigned char const *pNetTechOui, unsigned char const *pNetTechVariant, unsigned char const *pNetTechName, unsigned char const *url)
{
  i5_dm_device_type *pdevice;
  i5_dm_interface_type *pinterface;

  i5Trace(I5_MAC_FMT "\n", I5_MAC_PRM(interface_id) );

  pdevice = i5DmDeviceFind(device_id);
  if ( pdevice == NULL ) {
    i5Trace("Generic Phy Info received for a non-existent device\n");
    return -1;
  }
  
  if ((pinterface = i5DmInterfaceFind(pdevice, interface_id)) == NULL) {
    i5Trace("Generic Phy Info received for a non-existent interface ID\n");
    return -1;
  }

  i5DmInterfaceCopyInfo(pdevice, pinterface, interface_id, I5_MEDIA_TYPE_UNKNOWN, 
                        NULL, 0,  
                        pNetTechOui, pNetTechVariant, pNetTechName, url,
                        NULL);

#if defined(DMP_DEVICE2_IEEE1905BASELINE_1)
  if (i5DmDeviceIsSelf(device_id)) {
    i5CmsMdmLocalInterfaceUpdate(pinterface);
  }
  i5CmsMdmUpdateNetworkTopologyDevIfc(pdevice, pinterface);
#endif

  pinterface->state = i5DmStateDone;

  return 0;
}


/* Given a device, local interface ID and remote device, find:
   the remote device's interface ID */
int i5DmInterfaceFindMatchingInterfaceId(i5_dm_device_type *parent, unsigned char *local_interface_id, unsigned char *other_al_mac_address, unsigned char *other_interface_id)
{
  i5_dm_interface_type *interface;
  i5_dm_device_type *other_device;
  i5_dm_interface_type *other_interface;

  i5Trace(I5_MAC_FMT "\n", I5_MAC_PRM(other_al_mac_address));
  if ((interface = i5DmInterfaceFind(parent, local_interface_id)) != NULL) {
    if ((other_device = i5DmDeviceFind(other_al_mac_address)) != NULL) {
      other_interface = (i5_dm_interface_type *)other_device->interface_list.ll.next;
      while (other_interface != NULL) {
        if (interface->MediaType == other_interface->MediaType) {
          memcpy(other_interface_id, other_interface->InterfaceId, MAC_ADDR_LEN);
          return 0;
        }
        other_interface = other_interface->ll.next;
      }
    }
  }
  return -1;
}

static void i5DmLinkMetricsFreeTimer(void)
{
  if (NULL != i5_dm_network_topology.linkMetricAuto.dmLinkMetricTimer) {
    i5TimerFree(i5_dm_network_topology.linkMetricAuto.dmLinkMetricTimer);
    i5_dm_network_topology.linkMetricAuto.dmLinkMetricTimer = NULL;
  }  
}

static void i5DmLinkMetricsFreeMetricsActivatedTimer(void)
{
  if (NULL != i5_dm_network_topology.linkMetricAuto.dmLinkMetricActivatedTimer) {
    i5TimerFree(i5_dm_network_topology.linkMetricAuto.dmLinkMetricActivatedTimer);
    i5_dm_network_topology.linkMetricAuto.dmLinkMetricActivatedTimer = NULL;
  }
}

void i5DmLinkMetricsActivateTimedOut(void *arg)
{
  i5Trace("\n");
  i5DmLinkMetricsFreeMetricsActivatedTimer();
  i5DmLinkMetricsFreeTimer();  
}

static void i5DmValidateLinkMetricInterval (void)
{
  if (!i5_dm_network_topology.linkMetricAuto.dmLinkMetricIntervalValid) {
    i5_dm_network_topology.linkMetricAuto.dmLinkMetricIntervalMsec = I5_DM_LINK_METRICS_DEFAULT_AUTO_FETCH_INTERVAL_MSEC;
    i5_dm_network_topology.linkMetricAuto.dmLinkMetricIntervalValid = 1;
    i5Trace("Validated: %dms\n", i5_dm_network_topology.linkMetricAuto.dmLinkMetricIntervalMsec);  
  }
}

static void i5DmLinkMetricsQueryTimeout(void *arg)
{
  i5Trace("Next Interval: %dms\n", i5_dm_network_topology.linkMetricAuto.dmLinkMetricIntervalMsec);  
  i5DmLinkMetricsFreeTimer();
  
  if (i5MessageSendLinkQueries() > 0) {
    /* If there were actual Devices to which we can send queries, start a new timer */
    i5DmValidateLinkMetricInterval();
    if (0 != i5_dm_network_topology.linkMetricAuto.dmLinkMetricIntervalMsec) {
      i5_dm_network_topology.linkMetricAuto.dmLinkMetricTimer = i5TimerNew(i5_dm_network_topology.linkMetricAuto.dmLinkMetricIntervalMsec, 
                                                                           i5DmLinkMetricsQueryTimeout, NULL);
    }
  } else {
    i5TraceInfo("No more devices to Query.  Stopping Timer.\n");
  }
}

void i5DmLinkMetricsActivate(void)
{
  i5Trace("\n");
  i5DmLinkMetricsFreeMetricsActivatedTimer();
  i5_dm_network_topology.linkMetricAuto.dmLinkMetricActivatedTimer = i5TimerNew(I5_DM_LINK_METRIC_ACTIVATED_TIME_MSEC, i5DmLinkMetricsActivateTimedOut, NULL);

  /* Only trigger a link metric query if we're not doing queries yet */
  if (NULL == i5_dm_network_topology.linkMetricAuto.dmLinkMetricTimer) {
    i5DmLinkMetricsQueryTimeout(NULL);
  }
}


/* Returns 1 if the device is still present
 *         0 if the device can be timed out
 */
int i5DmIsDeviceConnected (i5_dm_device_type *device)
{
  i5_dm_device_type *self = i5DmDeviceFind(i5_config.i5_mac_address);
  i5_dm_1905_neighbor_type *neighbor = self->neighbor1905_list.ll.next;
  i5Trace(I5_MAC_DELIM_FMT "\n", I5_MAC_PRM(device->DeviceId));
  // check for direct neighbours
  while (neighbor) {
    // Is this a neighbour to the device in question?
    if (memcmp (neighbor->Ieee1905Id, device->DeviceId, MAC_ADDR_LEN) == 0) {
      unsigned char  netTechOui[I5_PHY_INTERFACE_NETTECHOUI_SIZE];
      unsigned short mediaType = i5GlueInterfaceGetMediaInfoFromName(neighbor->localIfname, NULL, NULL, netTechOui, NULL, NULL, NULL, 0);
      if ( i5DmIsInterfacePlc(mediaType, netTechOui)) {
        // homeplugd would have told us that the device was gone
        i5TraceInfo("Device Connected via PLC\n");
        return 1;
      }
      else if ( i5DmIsInterfaceWireless(mediaType) ) { 
        if (neighbor->latestRxBytes != neighbor->prevRxBytes) {
          i5TraceInfo("Device Connected via WDS with rxbytes accumulating\n");
          return 1;
        }
      }
      // For ETH, MoCA, etc, continue
    }
    neighbor = (i5_dm_1905_neighbor_type *)(neighbor->ll.next);
  }
  return 0;
}

void i5DmDeviceWatchDogTimeout(void *arg)
{
  i5_dm_device_type *device = (i5_dm_device_type *) arg;
  i5TraceError("Device Timed Out: " I5_MAC_DELIM_FMT " \"%s\" \n", I5_MAC_PRM(device->DeviceId), 
               device->friendlyName);
  // Before freeing, check for traffic in link metrics
  if (i5DmIsDeviceConnected (device)) {
    i5TraceError("Ignoring Time Out and resetting timer\n");
    i5DmRefreshDeviceTimer(device->DeviceId, 0);
    return;
  }
  
  i5TimerFree(device->watchdogTimer);
  i5DmDeviceFree(device);
}

void i5DmRefreshDeviceTimer(unsigned char *alMacAddress, char createFlag)
{
  i5_dm_device_type *device = i5DmDeviceIsSelf(alMacAddress) ? NULL : i5DmDeviceFind(alMacAddress);

  if (device != NULL) {
    if (device->watchdogTimer != NULL) {
      i5TimerFree(device->watchdogTimer);
      createFlag = 1;
    }
    if (1 == createFlag) {
      device->watchdogTimer = i5TimerNew(i5_config.deviceWatchdogTimeoutMsec, i5DmDeviceWatchDogTimeout, (void *)device);
    }
  }
}

void i5DmRefreshAllDeviceTimer(void)
{
  i5_dm_device_type *device = i5_dm_network_topology.device_list.ll.next;

  i5Trace("All device timers Reset to %d ms\n", i5_config.deviceWatchdogTimeoutMsec);

  while (device != NULL) {
    i5DmRefreshDeviceTimer(device->DeviceId, 0);
    device = device->ll.next;
  }
}

void i5DmConfigureDeviceWatchdogTimer(void)
{
  unsigned int linkMetricInt = i5_dm_network_topology.linkMetricAuto.dmLinkMetricIntervalMsec;
  
  /* Check if Link Metric Queries are currently in use */
  if (0 != linkMetricInt) {
    i5_config.deviceWatchdogTimeoutMsec = linkMetricInt * I5_DM_DEVICE_TIMER_ROBUSTNESS + I5_DM_DEVICE_TIMER_LATENCY_MSEC;
    if (i5_config.deviceWatchdogTimeoutMsec > I5_DM_TOP_DISC_DEVICE_GONE_TIMER) {
      /* Link Queries are so far apart that it's faster to use Top Disc Timer Value */
      i5_config.deviceWatchdogTimeoutMsec = I5_DM_TOP_DISC_DEVICE_GONE_TIMER;
    }
  } 
  else {
    /* There are no more link queries going out, so use the Topology Discovery Timer Value */
    i5_config.deviceWatchdogTimeoutMsec = I5_DM_TOP_DISC_DEVICE_GONE_TIMER;
  }

  i5DmRefreshAllDeviceTimer();
}

void i5DmSetLinkMetricInterval (unsigned int newIntervalMsec)
{
  int oldIntervalMsec = i5_dm_network_topology.linkMetricAuto.dmLinkMetricIntervalMsec;
  i5Trace("Called with %dms\n", newIntervalMsec);

  if (newIntervalMsec == i5_dm_network_topology.linkMetricAuto.dmLinkMetricIntervalMsec) {
    i5TraceInfo("No change.  Exiting.\n");
    return;
  }

  if (0 == newIntervalMsec) {
    /* this means turn it off */
  }
  else if (newIntervalMsec < I5_DM_LINK_METRICS_MINIMUM_AUTO_FETCH_INTERVAL_MSEC) {
    newIntervalMsec = I5_DM_LINK_METRICS_MINIMUM_AUTO_FETCH_INTERVAL_MSEC;
  }
  else if (newIntervalMsec > I5_DM_LINK_METRICS_MAXIMUM_AUTO_FETCH_INTERVAL_MSEC) {
    newIntervalMsec = I5_DM_LINK_METRICS_MAXIMUM_AUTO_FETCH_INTERVAL_MSEC;
  }
  
  i5Trace("Using %dms\n", newIntervalMsec);
  i5_dm_network_topology.linkMetricAuto.dmLinkMetricIntervalMsec = newIntervalMsec;
  i5_dm_network_topology.linkMetricAuto.dmLinkMetricIntervalValid = 1;
  if ((oldIntervalMsec == 0) || (oldIntervalMsec > i5_dm_network_topology.linkMetricAuto.dmLinkMetricIntervalMsec)){
    if (i5_dm_network_topology.linkMetricAuto.dmLinkMetricTimer) {
      i5TimerFree(i5_dm_network_topology.linkMetricAuto.dmLinkMetricTimer);
    }
    i5_dm_network_topology.linkMetricAuto.dmLinkMetricTimer = i5TimerNew(i5_dm_network_topology.linkMetricAuto.dmLinkMetricIntervalMsec, 
                                                                         i5DmLinkMetricsQueryTimeout, NULL);
  }

  i5DmConfigureDeviceWatchdogTimer();
}

void i5DmDeviceVersionTimeout(void *arg)
{
  i5_dm_device_type *destDev = arg;
  
  i5TraceError("Device Version Timed Out: " I5_MAC_DELIM_FMT "\n", I5_MAC_PRM(destDev->DeviceId) );

  if ( (NULL != destDev) && (I5_DM_NODE_VERSION_UNKNOWN == destDev->nodeVersion) ){
    i5Trace("  Treating as Legacy Device\n");
    // Since it didn't send us a Generic PHY in time, we regard it as a Legacy Node
    destDev->nodeVersion = I5_DM_NODE_VERSION_1905;
    i5MessageTopologyNotificationSend();
  }
  else {
    i5Trace("  Treating as 1905.1a Device\n");
  }
  i5TimerFree(destDev->nodeVersionTimer);
}

void i5DmWaitForGenericPhyQuery(i5_dm_device_type *destDev)
{
  if (destDev) {
    i5Trace("Starting Timer for Generic Phy query: " I5_MAC_DELIM_FMT "\n", I5_MAC_PRM(destDev->DeviceId) );
    if (destDev->nodeVersionTimer) {
      i5TimerFree(destDev->nodeVersionTimer);
    }
    destDev->nodeVersionTimer = i5TimerNew(I5_DM_VERSION_TIMER_MSEC, i5DmDeviceVersionTimeout, destDev);
  }
}

unsigned char i5DmAreThereNodesWithVersion(int nodeVersion)
{
  i5_dm_device_type *device = i5_dm_network_topology.device_list.ll.next;

  while (device != NULL) {
    if(device->nodeVersion == nodeVersion) {
      return 1;
    }
    device = device->ll.next;
  }
  return 0;
}


i5_dm_device_type *i5DmDeviceNew(unsigned char *device_id, int version, char const* pDevFriendlyName)
{
  i5_dm_device_type *new = (i5_dm_device_type *)malloc(sizeof(i5_dm_device_type));

  i5Trace(I5_MAC_FMT "\n", I5_MAC_PRM(device_id));
  if (NULL != new) {
    memset(new, 0, sizeof(i5_dm_device_type));
    i5LlItemAdd(&i5_dm_network_topology, &i5_dm_network_topology.device_list, new);
    ++i5_dm_network_topology.DevicesNumberOfEntries;
    memcpy(new->DeviceId, device_id, MAC_ADDR_LEN);
    new->Version = version;
    if ( pDevFriendlyName ) {
      strncpy(new->friendlyName, (char *)pDevFriendlyName, I5_DEVICE_FRIENDLY_NAME_LEN-1);
      new->friendlyName[I5_DEVICE_FRIENDLY_NAME_LEN-1] = '\0';
    }
    if (i5DmDeviceIsSelf(device_id)) {
      /* Never need to query self */
      new->queryState = i5DmStateDone;
    } 
    else {
      new->queryState = i5DmStateNew;
      new->validated = 0;
      new->numTopQueryFailures = 0;
      /* Start doing Link Metrics Queries if this is the first new device */
      if (NULL == i5_dm_network_topology.linkMetricAuto.dmLinkMetricTimer) {
        i5DmValidateLinkMetricInterval();
        if (0 != i5_dm_network_topology.linkMetricAuto.dmLinkMetricIntervalMsec) {
          i5TraceInfo("Start Link Metric Query Timer\n");
          i5_dm_network_topology.linkMetricAuto.dmLinkMetricTimer = i5TimerNew(i5_dm_network_topology.linkMetricAuto.dmLinkMetricIntervalMsec, 
                                                                               i5DmLinkMetricsQueryTimeout, NULL);
        }
      }

      if ( NULL == i5_dm_network_topology.pLinkMetricTimer ) {
        i5_dm_network_topology.pLinkMetricTimer = i5TimerNew(1000, i5DmUpdateNeighborLinkMetrics, NULL);
      }

      i5DmLegacyNeighborRemove(device_id);
    }
    i5JsonDevicePrint(I5_JSON_ALL_CLIENTS, new, i5Json_Add);
#if defined(DMP_DEVICE2_IEEE1905BASELINE_1)
    i5CmsMdmUpdateNetworkTopologyDev(new);
#endif
    return new;
  }

  return NULL;
}

i5_dm_device_type *i5DmDeviceFind(unsigned char const *device_id)
{  
  i5_dm_device_type *item = (i5_dm_device_type *)i5_dm_network_topology.device_list.ll.next;

  while ((item != NULL) && (memcmp(item->DeviceId, device_id, MAC_ADDR_LEN))) {
    item = item->ll.next;
  }
  return item;
}

void i5DmDeviceQueryStateSet(unsigned char *device_id, unsigned char queryState)
{
  i5_dm_device_type *device;

  if ((device = i5DmDeviceFind(device_id)) == NULL) {
    return;
  }
  if (i5DmStateDone == queryState) {
    device->validated = 1;
    device->numTopQueryFailures = 0;
  }
  device->queryState = queryState;
}

unsigned char i5DmDeviceQueryStateGet(unsigned char const *device_id)
{
  i5_dm_device_type *device;

  if ((device = i5DmDeviceFind(device_id)) == NULL) {
    return i5DmStateNotFound;
  }

  return (device->queryState);
}

void i5DmDeviceFree(i5_dm_device_type *device)
{  
  i5Trace(I5_MAC_FMT "\n", I5_MAC_PRM(device->DeviceId));
  while (device->interface_list.ll.next != NULL) {
    i5DmInterfaceFree(device, device->interface_list.ll.next);
  }

  while (device->legacy_list.ll.next != NULL) {
    i5DmLegacyNeighborFree(device, device->legacy_list.ll.next);
  }

  while (device->neighbor1905_list.ll.next != NULL) {
    i5Dm1905NeighborFree(device, device->neighbor1905_list.ll.next);
  }

  i5Dm1905NeighborFreeAllLinksRemoteDevice(device->DeviceId);

  while (device->bridging_tuple_list.ll.next != NULL) {
    i5DmBridgingTupleFree(device, device->bridging_tuple_list.ll.next);
  }

  if (device->watchdogTimer) {
    i5TimerFree(device->watchdogTimer);
  }

  if (device->nodeVersionTimer) {
    i5TimerFree(device->nodeVersionTimer);
  }

  i5JsonDevicePrint(I5_JSON_ALL_CLIENTS, device, i5Json_Delete);
#if defined(DMP_DEVICE2_IEEE1905BASELINE_1)
  i5CmsMdmRemoveNetworkTopologyDev(device->DeviceId);
#endif

  i5LlItemFree(&i5_dm_network_topology.device_list, device);
  --i5_dm_network_topology.DevicesNumberOfEntries;
}

void i5DmDevicePending(void)
{  
  i5_dm_device_type *device;

  i5Trace("\n");
  device = (i5_dm_device_type *)i5_dm_network_topology.device_list.ll.next;
  while (device != NULL) {
    device->state = i5DmStatePending;
    device = device->ll.next;
  }
}

void i5DmDeviceDone(void)
{  
  i5_dm_device_type *device, *next;

  i5Trace("\n");
  device = (i5_dm_device_type *)i5_dm_network_topology.device_list.ll.next;
  while (device != NULL) {
    next = device->ll.next;
    if (device->state == i5DmStatePending) {
      i5DmDeviceFree(device);
    }
    device = next;
  }
}

void i5DmDeviceDelete(unsigned char *device_id)
{
  i5_dm_device_type *device;

  i5Trace("\n");
  if ((device = i5DmDeviceFind(device_id)) == NULL) {
    return;
  }

  i5DmDeviceFree(device);
}

void i5DmDeviceNewIfNew(unsigned char *neighbor_al_mac_address)
{
  if (i5DmDeviceFind(neighbor_al_mac_address) == NULL) {
    i5DmDeviceNew(neighbor_al_mac_address, 0, NULL);
  }
}

void i5DmDeviceTopologyQuerySendToAllNew(i5_socket_type *psock)
{
  i5_dm_device_type *device = (i5_dm_device_type *)i5_dm_network_topology.device_list.ll.next;
  
  while (device != NULL) {
    if (device->queryState == i5DmStateNew) {
      i5MessageTopologyQuerySend(psock, device->DeviceId);
    }
    device = device->ll.next;
  }
}

int i5DmDeviceIsSelf(unsigned char *device_id)
{
   return (memcmp(device_id, i5_config.i5_mac_address, MAC_ADDR_LEN) == 0);
}

int i5DmDeviceTopologyChangeProcess(unsigned char *device_id)
{
  i5_dm_device_type *device;

  if ((device = i5DmDeviceFind(device_id)) == NULL) {
    return 0;
  }

  if (i5DmDeviceIsSelf(device_id)) {
    if (device->hasChanged) {
      device->hasChanged = 0;
      i5TraceInfo("Local device has changed\n");
      return 1;
    }
  }
  return 0;
}

void i5DmTopologyFreeUnreachableDevices(void)
{  
  i5_dm_device_type *device;
  i5_dm_device_type *neighbor_device;
  i5_dm_1905_neighbor_type *neighbor_1905;
  i5_ll_search_item_type search_list = {{0}};

  i5Trace("\n");

  /* Start with self and traverse all devices based on reachable neighbors */
  i5DmDevicePending();
  if ((device = i5DmDeviceFind(i5_config.i5_mac_address)) != NULL) {
    i5LlSearchItemPush(&search_list, device);
    device->state = i5DmStateDone;
  }
  else {
    i5Trace("Self not found!\n");
  }

  while ((device = (i5_dm_device_type *)i5LlSearchItemPop(&search_list)) != NULL) {
    neighbor_1905 = (i5_dm_1905_neighbor_type *)device->neighbor1905_list.ll.next;
    while (neighbor_1905 != NULL) {
      neighbor_device = i5DmDeviceFind(neighbor_1905->Ieee1905Id);
      if (neighbor_device != NULL && neighbor_device->state == i5DmStatePending) {
        i5LlSearchItemPush(&search_list, neighbor_device);
        neighbor_device->state = i5DmStateDone;
      }
      neighbor_1905 = (i5_dm_1905_neighbor_type *)neighbor_1905->ll.next;
    }
  }  
  i5DmDeviceDone();
}

void i5DmDeviceFreeUnreachableNeighbors(unsigned char *device_id, int ifindex, unsigned char *neighbor_interface_list, unsigned int length)
{  
  i5_dm_device_type *device;
  i5_dm_1905_neighbor_type *item;
  i5_dm_1905_neighbor_type *next;
  int i;

  i5Trace(I5_MAC_FMT " Ifindex=%d \n", I5_MAC_PRM(device_id), ifindex);
  if ((device = i5DmDeviceFind(device_id)) == NULL) {
    return;
  }

  item = (i5_dm_1905_neighbor_type *)device->neighbor1905_list.ll.next;
  while (item != NULL) {
    next = item->ll.next;
    if (item->localIfindex == ifindex) {
      if (NULL == neighbor_interface_list) {
        i5Dm1905NeighborFree(device, item);
      }
      else {
        for (i=0;i<length-5;i+=6) {
          if (memcmp(item->NeighborInterfaceId, &neighbor_interface_list[i], MAC_ADDR_LEN) == 0) {
            i5Dm1905NeighborFree(device, item);
          }
        }
      }
    }
    item = next;
  }
  i5DmTopologyFreeUnreachableDevices();
}

unsigned int i5DmInterfaceStatusGet(unsigned char *device_id, unsigned char *interface_id)
{
  i5_dm_device_type *device;
  i5_dm_interface_type *interface;

  i5Trace(I5_MAC_FMT "\n", I5_MAC_PRM(interface_id));
  if ((device = i5DmDeviceFind(device_id)) != NULL) {
    if ((interface = i5DmInterfaceFind(device, interface_id)) != NULL) {
      return interface->Status;
    }
  }
  
  return IF_OPER_UNKNOWN;
}

void i5DmInterfaceStatusSet(unsigned char *device_id, unsigned char * interfaceId, int ifindex, unsigned int status)
{
  i5_dm_device_type *device;
  i5_dm_interface_type *interface;

  i5Trace(I5_MAC_FMT " %d %s \n", I5_MAC_PRM(device_id), ifindex, 
                                                  (status==IF_OPER_DOWN) ? "DOWN" : ((status==IF_OPER_UP) ? "UP" : "OTHER") );
  if ((device = i5DmDeviceFind(device_id)) != NULL) {
    if ((interface = i5DmInterfaceFind(device, interfaceId)) != NULL) {
      interface->Status = status;
    }
    if (IF_OPER_DOWN == status) {
      i5DmDeviceFreeUnreachableNeighbors(device_id, ifindex, NULL, 0);
    }
  }
}

int i5DmCtlSizeGet( void )
{  
  i5_dm_device_type *device = (i5_dm_device_type *)i5_dm_network_topology.device_list.ll.next;
  int msgSize;

  msgSize = sizeof(i5_dm_network_topology_type);
  while (device != NULL) {
    msgSize += sizeof(i5_dm_device_type);
    msgSize += sizeof(i5_dm_interface_type) * device->InterfaceNumberOfEntries;
    msgSize += sizeof(i5_dm_legacy_neighbor_type) * device->LegacyNeighborNumberOfEntries;
    msgSize += sizeof(i5_dm_1905_neighbor_type) * device->Ieee1905NeighborNumberOfEntries;
    msgSize += sizeof(i5_dm_bridging_tuple_info_type) * device->BridgingTuplesNumberOfEntries;
    device = device->ll.next;
  }

  return msgSize;
}

void i5DmCtlAlMacRetrieve(char *pMsgBuf)
{
  memcpy(pMsgBuf, i5_config.i5_mac_address, MAC_ADDR_LEN);
}

void i5DmCtlRetrieve(char *pMsgBuf)
{  
  i5_dm_device_type *device = (i5_dm_device_type *)i5_dm_network_topology.device_list.ll.next;

  memcpy(pMsgBuf, &i5_dm_network_topology, sizeof(i5_dm_network_topology_type));
  pMsgBuf += sizeof(i5_dm_network_topology_type);
  
  device = (i5_dm_device_type *)i5_dm_network_topology.device_list.ll.next;
  while (device != NULL) {
    i5_dm_interface_type *interface;
    i5_dm_1905_neighbor_type *neighbor;
    i5_dm_legacy_neighbor_type *legacy;
    i5_dm_bridging_tuple_info_type *bridging;
  
    memcpy(pMsgBuf, device, sizeof(i5_dm_device_type));
    pMsgBuf += sizeof(i5_dm_device_type);

    interface = (i5_dm_interface_type *)device->interface_list.ll.next;
    while (interface != NULL) {
      memcpy(pMsgBuf, interface, sizeof(i5_dm_interface_type));
      pMsgBuf += sizeof(i5_dm_interface_type);
      interface = interface->ll.next;
    }
    
    legacy = (i5_dm_legacy_neighbor_type *)device->legacy_list.ll.next;
    while (legacy != NULL) {
      memcpy(pMsgBuf, legacy, sizeof(i5_dm_legacy_neighbor_type));
      pMsgBuf += sizeof(i5_dm_legacy_neighbor_type);
      legacy = legacy->ll.next;
    }
    
    neighbor = (i5_dm_1905_neighbor_type *)device->neighbor1905_list.ll.next;
    while (neighbor != NULL) {
      memcpy(pMsgBuf, neighbor, sizeof(i5_dm_1905_neighbor_type));
      pMsgBuf += sizeof(i5_dm_1905_neighbor_type);
      neighbor = neighbor->ll.next;
    }

    bridging = (i5_dm_bridging_tuple_info_type *)device->bridging_tuple_list.ll.next;
    while (bridging != NULL) {
      memcpy(pMsgBuf, bridging, sizeof(i5_dm_bridging_tuple_info_type));
      pMsgBuf += sizeof(i5_dm_bridging_tuple_info_type);
      bridging = bridging->ll.next;
    }

    device = device->ll.next;
  }
}


void i5DmSetFriendlyName(const char * name) {
    
    i5_dm_device_type *device;
    device = i5DmDeviceFind(i5_config.i5_mac_address);
    if (device == NULL) {
        // can't find myself...
        i5TraceError("Self not found!\n");
        return;
    }
    snprintf(device->friendlyName, I5_DEVICE_FRIENDLY_NAME_LEN, "%s", name);    
}

void i5DmDeinit(void)
{
  if ( i5_dm_network_topology.pLinkMetricTimer ) {
    i5TimerFree(i5_dm_network_topology.pLinkMetricTimer);
  }
  if ( i5_dm_network_topology.linkMetricAuto.dmLinkMetricTimer ) {
    i5TimerFree(i5_dm_network_topology.linkMetricAuto.dmLinkMetricTimer);
  }
  while ( i5_dm_network_topology.device_list.ll.next ) {
    i5DmDeviceFree(i5_dm_network_topology.device_list.ll.next);
  }
}

int i5DmInit( )
{
  /* create local device */
  i5_dm_device_type *pdevice = i5DmDeviceNew(i5_config.i5_mac_address, I5_MESSAGE_VERSION, i5_config.friendlyName);

  if (NULL == pdevice) {
    return -1;
  }

  i5DmValidateLinkMetricInterval();
  i5DmConfigureDeviceWatchdogTimer();

  return 0;
}


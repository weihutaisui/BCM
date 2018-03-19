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
 * IEEE1905 TLVs
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ieee1905_message.h"
#include "ieee1905_datamodel.h"
#include "ieee1905_tlv.h"
#include "ieee1905_json.h"
#include "ieee1905_trace.h"
#include "ieee1905_utils.h"

#define I5_TRACE_MODULE i5TraceTlv

enum {
  i5TlvEndOfMessageType = 0,
  i5TlvAlMacAddressType,
  i5TlvMacAddressType,
  i5TlvDeviceInformationType,
  i5TlvDeviceBridgingCapabilityType,
  i5TlvMediaType,
  i5TlvLegacyNeighborDeviceType,
  i5Tlv1905NeighborDeviceType,
  i5TlvLinkMetricQueryType,
  i5TlvTransmitterLinkMetricType,
  i5TlvReceiverLinkMetricType,
  i5TlvVendorSpecificType,
  i5TlvLinkMetricResultCodeType,
  i5TlvSearchedRoleType,
  i5TlvAutoconfigFreqBandType,
  i5TlvSupportedRoleType,
  i5TlvSupportedFreqBandType,
  i5TlvWscType,
  i5TlvPushButtonEventNotificationType,
  i5TlvPushButtonJoinNotificationType,   // UNUSED
  i5TlvGenericPhyDevInfoType,
  i5TlvDeviceIdentificationType,         // UNUSED
  i5TlvControlUrlType,                   // UNUSED
  i5TlvIpv4Type,                         // UNUSED
  i5TlvIpv6Type,                         // UNUSED
  i5TlvPushButtonGenericPhyEventNotificationType, 
  
  i5TlvBrcmRoutingTableType = 0x80,
  i5TlvBrcmFriendlyNameType = 0x81,
  i5TlvBrcmFriendlyUrlType = 0x82,
  i5TlvBrcmFriendlyIpv4Type = 0x84,
  i5TlvBrcmFriendlyIpv6Type = 0x86,
};

/* Vendor specifics */
#define i5TlvVendorSpecificOui_Byte1 0x00
#define i5TlvVendorSpecificOui_Byte2 0x10
#define i5TlvVendorSpecificOui_Byte3 0x18
#define i5TlvVendorSpecificOui_Length 3

char const i5TlvNames [][24] = {
   "End of Message", 
   "Al MAC Address",
   "MAC Address",
   "Device Information",
   "Device Bridge Cap.",
   "Media Type",
   "Legacy Neighbor Dev.",
   "1905 Neighbor Dev.",
   "Link Metric Query",
   "Tx Link Metric",
   "Rx Link Metric",
   "Vendor Specific",
   "Link Metric Result",
   "Searched Role",
   "Autoconfig Freq Band",
   "Supported Role",
   "Supported Freq Band",
   "Wsc",
   "Push Button Notify"
};

#define i5TlvFirstProprietaryNumber 0x80
char const i5TlvBrcmNames [][24] = {
  "Brcm Routing Table",
  "Brcm Friendly"
};

int i5TlvIsEndOfMessageType(int tlvType)
{
  return (tlvType == i5TlvEndOfMessageType);
}

char const *i5TlvGetTlvTypeString(int tlvType)
{
  if (tlvType < i5TlvFirstProprietaryNumber) {
    return i5TlvNames[tlvType];
  }
  else {
    return i5TlvBrcmNames[tlvType-i5TlvFirstProprietaryNumber];
  }
}

int i5TlvEndOfMessageTypeInsert(i5_message_type *pmsg)
{
  unsigned char buf[3];
  i5_tlv_t *ptlv = (i5_tlv_t *)buf;
  int len = 0;

  ptlv->type = i5TlvEndOfMessageType;
  ptlv->length = htons(0);
  len += sizeof(i5_tlv_t);

  return (i5MessageInsertTlv(pmsg, buf, len));
}

int i5TlvEndOfMessageTypeExtract(i5_message_type *pmsg)
{
  unsigned int length;
  unsigned char *pvalue;

  if (i5MessageTlvExtract(pmsg, i5TlvEndOfMessageType, &length, &pvalue, i5MessageTlvExtractWithReset) == 0) {
    if (length == 0) {
      return 0;
    }
  }
  return -1;
}

int i5TlvAlMacAddressTypeInsert(i5_message_type *pmsg)
{
  unsigned char buf[9];
  i5_tlv_t *ptlv = (i5_tlv_t *)buf;
  int len = 0;

  ptlv->type = i5TlvAlMacAddressType;
  ptlv->length = htons(MAC_ADDR_LEN);
  len += sizeof(i5_tlv_t);
  memcpy(&buf[len], i5_config.i5_mac_address, MAC_ADDR_LEN);
  len+=MAC_ADDR_LEN;

  return (i5MessageInsertTlv(pmsg, buf, len));
}

int i5TlvAlMacAddressTypeExtract(i5_message_type *pmsg, unsigned char *mac_address)
{
  unsigned char *pvalue;
  unsigned int length;

  if (i5MessageTlvExtract(pmsg, i5TlvAlMacAddressType, &length, &pvalue, i5MessageTlvExtractWithReset) == 0) {
    if (length == MAC_ADDR_LEN) {
      if ( i5DmDeviceIsSelf(pvalue) ) {
        i5TraceInfo("Received packet using local AL MAC\n");
        return -1;
      }
      memcpy(mac_address, pvalue, MAC_ADDR_LEN);
      i5DmRefreshDeviceTimer(mac_address, 0);
      return 0;
    }
  }
  return -1;
}

int i5TlvMacAddressTypeInsert(i5_message_type *pmsg, unsigned char *mac_address)
{
  unsigned char buf[9];
  i5_tlv_t *ptlv = (i5_tlv_t *)buf;
  int len = 0;

  ptlv->type = i5TlvMacAddressType;
  ptlv->length = htons(MAC_ADDR_LEN);
  len += sizeof(i5_tlv_t);
  memcpy(&buf[len], mac_address, MAC_ADDR_LEN);
  len+=MAC_ADDR_LEN;

  return (i5MessageInsertTlv(pmsg, buf, len));
}

int i5TlvMacAddressTypeExtract(i5_message_type *pmsg, unsigned char *mac_address)
{
  unsigned char *pvalue;
  unsigned int length;

  if (i5MessageTlvExtract(pmsg, i5TlvMacAddressType, &length, &pvalue, i5MessageTlvExtractWithReset) == 0) {
    if (length == MAC_ADDR_LEN) {
      memcpy(mac_address, pvalue, MAC_ADDR_LEN);
      return 0;
    }
  }
  return -1;
}

int i5TlvDeviceInformationTypeInsert(i5_message_type *pmsg, unsigned char useLegacyHpav, char* containsGenericPhy)
{
  unsigned char *pbuf, *pmem;
  i5_tlv_t *ptlv;
  i5_dm_device_type *pdmdev;
  i5_dm_interface_type *pdmif;
  int rc = 0;

  if ((pmem = (unsigned char *)malloc(I5_PACKET_BUF_LEN)) == NULL) {
    printf("malloc error\n");
    return -1;
  }

  pbuf = pmem + 3; // Header filled at the end
  memcpy(pbuf, i5_config.i5_mac_address, MAC_ADDR_LEN);
  pbuf += MAC_ADDR_LEN;

  if ((pdmdev = i5DmDeviceFind(i5_config.i5_mac_address)) != NULL) {
    *pbuf = pdmdev->InterfaceNumberOfEntries;
    pbuf++;

    pdmif = (i5_dm_interface_type *)pdmdev->interface_list.ll.next;
    while (pdmif != NULL) {
      memcpy(pbuf, pdmif->InterfaceId, MAC_ADDR_LEN);
      pbuf += MAC_ADDR_LEN;
      *((unsigned short *)pbuf) = htons(pdmif->MediaType);
      if (pdmif->MediaType == I5_MEDIA_TYPE_UNKNOWN) {
        *containsGenericPhy = 1;
        if ( (useLegacyHpav) && i5DmIsInterfacePlc(pdmif->MediaType, pdmif->netTechOui) ) {
          // The legacy node has to be told to use the old school HPAV 1
          *((unsigned short *)pbuf) =  htons(I5_MEDIA_TYPE_1901_FFT);
        }
      }
      pbuf+=2;
      *pbuf = pdmif->MediaSpecificInfoSize;
      pbuf++;
      if (pdmif->MediaSpecificInfoSize > 0) {
        memcpy(pbuf, pdmif->MediaSpecificInfo, pdmif->MediaSpecificInfoSize);
        pbuf += pdmif->MediaSpecificInfoSize;
      }
      pdmif = pdmif->ll.next;
    }

    ptlv = (i5_tlv_t *)pmem;
    ptlv->type = i5TlvDeviceInformationType;
    ptlv->length = htons(pbuf-pmem-3);
    rc = i5MessageInsertTlv(pmsg, pmem, pbuf-pmem);
  }

  free(pmem);
  return (rc);
}

int i5TlvDeviceInformationTypeExtractAlMac(i5_message_type *pmsg, unsigned char *neighbor_al_mac_address)
{
  unsigned char *pvalue;
  unsigned int length;
  int retval = -1;

  i5MessageReset(pmsg);
  while (i5MessageTlvExtract(pmsg, i5TlvDeviceInformationType, &length, &pvalue, i5MessageTlvExtractWithoutReset) == 0) {
    if (length >= MAC_ADDR_LEN) {
      memcpy(neighbor_al_mac_address, pvalue, MAC_ADDR_LEN);
      retval = 0;
    }
  }

  return retval;
}

int i5TlvDeviceInformationTypeExtract(i5_message_type *pmsg, unsigned char *neighbor_al_mac_address, unsigned char *deviceHasGenericPhy)
{
  unsigned char *pvalue;
  unsigned int length, pos, n;
  unsigned char interface_id[MAC_ADDR_LEN];
  unsigned short if_media_type;
  unsigned char if_media_specific_info[I5_MEDIA_SPECIFIC_INFO_MAX_SIZE];
  unsigned char *pif_media_specific_info = NULL;
  unsigned int if_media_specific_info_size = 0;
  int interfacesCleared = 0;
  int retval = -1;

  *deviceHasGenericPhy = 0;
  i5MessageReset(pmsg);
  while (i5MessageTlvExtract(pmsg, i5TlvDeviceInformationType, &length, &pvalue, i5MessageTlvExtractWithoutReset) == 0) {
    if (length >= MAC_ADDR_LEN) {
      if ( memcmp(neighbor_al_mac_address, pvalue, MAC_ADDR_LEN) != 0 ) {
        retval = -1;
        break;
      }
      pos = MAC_ADDR_LEN;
      pos++;

      if (0 == interfacesCleared) {
        i5DmInterfacePending(neighbor_al_mac_address);
        interfacesCleared = 1;
      }
      while (length >= pos + MAC_ADDR_LEN + 3) {
        memcpy(interface_id, &pvalue[pos], MAC_ADDR_LEN);
        pos += MAC_ADDR_LEN;
        if_media_type = ntohs(*((unsigned short *)&pvalue[pos]));
        pos+=2;
        n = pvalue[pos];
        pos++;
        if (if_media_type == I5_MEDIA_TYPE_UNKNOWN) {
          *deviceHasGenericPhy = 1;
        }
        if (((if_media_type >= 0x10) && (if_media_type <= 0x16) && (n == i5TlvMediaSpecificInfoWiFi_Length)) ||
            ((if_media_type >= 0x20) && (if_media_type <= 0x21) && (n == i5TlvMediaSpecificInfo1901_Length))) {
          if (length >= pos + n) {
            memcpy(if_media_specific_info, &pvalue[pos], n);
            pif_media_specific_info = if_media_specific_info;
            if_media_specific_info_size = n;
          }
        }
        else {
          pif_media_specific_info = NULL;
          if_media_specific_info_size = 0;
        }
        pos += n;
        i5DmInterfaceUpdate(neighbor_al_mac_address, interface_id, i5MessageVersionGet(pmsg), if_media_type,
                            pif_media_specific_info, if_media_specific_info_size, NULL, NULL, 0);
      }
      retval = 0;
    }
  }

  if (0 == retval) {
    i5DmInterfaceDone(neighbor_al_mac_address);
  }

  return retval;
}

int i5TlvGenericPhyTypeInsert (i5_message_type *pmsg)
{
  i5_dm_device_type *selfDevice = i5DmDeviceFind(i5_config.i5_mac_address);
  i5_dm_interface_type *currIf = (i5_dm_interface_type *)(selfDevice->interface_list.ll.next);

  unsigned char buf[1024]; // This needs to be calculated somehow
  i5_tlv_t *ptlv = (i5_tlv_t *)buf;
  unsigned int index = 0;
  unsigned int remaining, fullPacket;
  unsigned char *numPhyEntries = NULL;
  unsigned int phyIfIndex = 0;

  i5MessageGetPacketSpace(pmsg, &remaining, &fullPacket);  
  ptlv->type = i5TlvGenericPhyDevInfoType;
  ptlv->length = 0; /* this is a variable length packet, add as we go */
  index += sizeof(i5_tlv_t);
  remaining -= sizeof(i5_tlv_t);

  memcpy(&buf[index], i5_config.i5_mac_address, MAC_ADDR_LEN);
  index += MAC_ADDR_LEN;

  buf[index] = 0;
  numPhyEntries = &buf[index];
  index += 1;

  for ( ; phyIfIndex < selfDevice->InterfaceNumberOfEntries; phyIfIndex ++ ) {
    if (currIf->MediaType == I5_MEDIA_TYPE_UNKNOWN) {
      (*numPhyEntries) ++;
      
      // Local Interface MAC
      memcpy(&buf[index], currIf->InterfaceId, MAC_ADDR_LEN);
      index += MAC_ADDR_LEN;
        
      // OUI
      memcpy(&buf[index], currIf->netTechOui, I5_PHY_INTERFACE_NETTECHOUI_SIZE);
      index += I5_PHY_INTERFACE_NETTECHOUI_SIZE;
        
      // variant index
      buf[index] = currIf->netTechVariant;
      index ++;
  
      // UTF-8 string [32]
      memcpy(&buf[index], &currIf->netTechName, I5_PHY_INTERFACE_NETTECHNAME_SIZE);
      index += I5_PHY_INTERFACE_NETTECHNAME_SIZE;
  
      // u (sizeof URL)
      buf[index] = strlen((char *)currIf->url);
      index ++;
  
      // s (sizeof media spec info field)
      buf[index] = currIf->MediaSpecificInfoSize;
      index ++;
  
      // URL (copy without NULL, since we have a length recorded above)
      memcpy(&buf[index], &currIf->url, strlen((char *)currIf->url));
      index += strlen((char *)currIf->url);
  
      // Media spec info field
      memcpy(&buf[index], &currIf->MediaSpecificInfo, currIf->MediaSpecificInfoSize);
      index += currIf->MediaSpecificInfoSize;
    }
    currIf = (i5_dm_interface_type *)(currIf->ll.next);
  }

  i5TraceInfo("total length = %d\n", index);
  ptlv->length = htons(index-3);
  return i5MessageInsertTlv(pmsg, buf, index);
}

int i5TlvGenericPhyTypeExtract (i5_message_type *pmsg)
{
  unsigned char *pvalue;
  unsigned int length;

  if (i5MessageTlvExtract(pmsg, i5TlvGenericPhyDevInfoType, &length, &pvalue, i5MessageTlvExtractWithReset) == 0) {
    unsigned char mac_address[MAC_ADDR_LEN];
    int numPhyInterfaces = 0;
    int phyIfIndex = 0;
    int remaining = length;
    i5_dm_device_type *reportingDevice = NULL;

    if (length < MAC_ADDR_LEN + 1) {
      i5TraceInfo("Minimum size 7 bytes not met (%d bytes rx'd)\n", length);
      return -1;
    }
    
    memcpy(mac_address, pvalue, MAC_ADDR_LEN);
    pvalue += MAC_ADDR_LEN;
    remaining -= MAC_ADDR_LEN;

    reportingDevice = i5DmDeviceFind(mac_address);
    if (!reportingDevice) {
      i5TraceInfo("Reporting device " I5_MAC_DELIM_FMT " not found\n", I5_MAC_PRM(mac_address));
      return -1;
    }

    numPhyInterfaces = (int)*pvalue;
    pvalue ++;
    remaining --;
    i5TraceInfo("Gen Phy TLV contains %d PHY interfaces\n", numPhyInterfaces);
    for ( ; phyIfIndex < numPhyInterfaces ; phyIfIndex++) {
      unsigned char ifAddress[MAC_ADDR_LEN];
      unsigned char netTechOui[I5_PHY_INTERFACE_NETTECHOUI_SIZE];
      unsigned char netTechVariant;
      unsigned char netTechName[I5_PHY_INTERFACE_NETTECHNAME_SIZE];
      unsigned char u, s;
      unsigned char url[I5_PHY_INTERFACE_URL_MAX_SIZE];
      unsigned char mediaSpecInfo[I5_MEDIA_SPECIFIC_INFO_MAX_SIZE];
      
      // Check for min length (6+3+1+32+1+1+0+0) assuming url and media spec info could be zero length
      if (remaining < 44) {
        i5TraceInfo("Minimum size of GenPhy record = 44 bytes not met (%d bytes remain)\n", remaining);
        return -1;
      }
      
      // Fetch interface ADDR
      memcpy(ifAddress, pvalue, MAC_ADDR_LEN);
      pvalue += MAC_ADDR_LEN;
      remaining -= MAC_ADDR_LEN;

      // Fetch OUI
      memcpy(netTechOui, pvalue, I5_PHY_INTERFACE_NETTECHOUI_SIZE);
      pvalue += I5_PHY_INTERFACE_NETTECHOUI_SIZE;
      remaining -= I5_PHY_INTERFACE_NETTECHOUI_SIZE;

      // Fetch Variant
      netTechVariant = *pvalue;
      pvalue ++;
      remaining --;
      
      // Fetch Variant name
      memcpy(netTechName, pvalue, I5_PHY_INTERFACE_NETTECHNAME_SIZE);
      pvalue += I5_PHY_INTERFACE_NETTECHNAME_SIZE;
      remaining -= I5_PHY_INTERFACE_NETTECHNAME_SIZE;

      // Fetch u, s
      u = *pvalue;
      pvalue ++;
      remaining --;
      s = *pvalue;
      pvalue ++;
      remaining --;

      if (remaining < u + s) {
        i5TraceInfo("Url and media spec info want %d bytes but only %d bytes remain\n", u+s, remaining);
        return -1;
      }

      // Fetch url
      memcpy(url, pvalue, u < I5_PHY_INTERFACE_URL_MAX_SIZE ? u : I5_PHY_INTERFACE_URL_MAX_SIZE);
      pvalue += u;
      remaining -= u;
      url[(u < I5_PHY_INTERFACE_URL_MAX_SIZE-1) ? u : I5_PHY_INTERFACE_URL_MAX_SIZE-1] = '\0';
      
      // Fetch media spec info
      memcpy(mediaSpecInfo, pvalue, s < I5_MEDIA_SPECIFIC_INFO_MAX_SIZE ? s : I5_MEDIA_SPECIFIC_INFO_MAX_SIZE);
      pvalue += s;
      remaining -= s;      
      mediaSpecInfo[(s < I5_MEDIA_SPECIFIC_INFO_MAX_SIZE-1) ? s : I5_MEDIA_SPECIFIC_INFO_MAX_SIZE-1] = '\0';
      i5DmInterfacePhyUpdate(reportingDevice->DeviceId, ifAddress, netTechOui, &netTechVariant, 
                             netTechName, url);      
    }
  }
  return 0;
}

int i5TlvDeviceBridgingCapabilityTypeInsert(i5_message_type *pmsg)
{
  unsigned char *pbuf, *pmem;
  i5_tlv_t *ptlv;
  i5_dm_device_type *pdmdev;
  i5_dm_bridging_tuple_info_type *pdmbrtuple;
  int rc = 0;

  pdmdev = i5DmDeviceFind(i5_config.i5_mac_address);
  if ( NULL == pdmdev ) {
    return -1;
  }

  if ( 0 == pdmdev->BridgingTuplesNumberOfEntries ) {
    return 0;
  }

  if ((pmem = (unsigned char *)malloc(I5_PACKET_BUF_LEN)) == NULL) {
    i5TraceError("Out of memory error\n");
    return -1;
  }

  pbuf = pmem + sizeof(i5_tlv_t);
  *pbuf = pdmdev->BridgingTuplesNumberOfEntries;
  pbuf++;
 
  pdmbrtuple = (i5_dm_bridging_tuple_info_type *)pdmdev->bridging_tuple_list.ll.next;
  while (pdmbrtuple != NULL) {
    int len = pdmbrtuple->forwardingInterfaceListNumEntries * MAC_ADDR_LEN;
    *pbuf = pdmbrtuple->forwardingInterfaceListNumEntries;
    pbuf++;
    memcpy(pbuf, &pdmbrtuple->ForwardingInterfaceList[0], len);
    pbuf += len;
    pdmbrtuple = pdmbrtuple->ll.next;
  }
 
  ptlv = (i5_tlv_t *)pmem;
  ptlv->type = i5TlvDeviceBridgingCapabilityType;
  ptlv->length = htons(pbuf-pmem-3);
  rc = i5MessageInsertTlv(pmsg, pmem, pbuf-pmem);

  free(pmem);
  return (rc);
}

int i5TlvDeviceBridgingCapabilityTypeExtract(i5_message_type *pmsg, unsigned char *pdevid)
{
  unsigned char *pvalue;
  unsigned int length = 0;
  
  int rc = 0;

  i5MessageReset(pmsg);
  i5DmBridgingTuplePending(pdevid);
  while ((rc = i5MessageTlvExtract(pmsg, i5TlvDeviceBridgingCapabilityType, &length, &pvalue, i5MessageTlvExtractWithoutReset)) == 0) {
    if (length > 0) {
      unsigned char  i;
      unsigned int pos = 0;
      unsigned char device_num_tuples;

      device_num_tuples = pvalue[pos];
      pos++;

      for (i = 0; i < device_num_tuples; i++) {
        i5Trace("Index = %d\n", i);
        if (length >= pos + 1) {
          unsigned char tuple_num_macaddrs = pvalue[pos];
          pos++;

          if (length >= pos + tuple_num_macaddrs*MAC_ADDR_LEN) {
              i5DmBridgingTupleUpdate(pdevid, i5MessageVersionGet(pmsg), NULL, tuple_num_macaddrs, &pvalue[pos]);
              pos += tuple_num_macaddrs*MAC_ADDR_LEN;
          }
        }
      }
    } 
    else {
      rc = -1;
      break;
    }
  }
  i5DmBridgingTupleDone(pdevid);

  /* This is an optional TLV */
  if (rc == -2) {
    rc = 0;
  }

  return rc;
}

int i5TlvLegacyNeighborDeviceTypeInsert(i5_message_type *pmsg)
{
  unsigned char *pbuf, *pmem;
  i5_tlv_t *ptlv;
  i5_dm_device_type *pdmdev;
  i5_dm_legacy_neighbor_type *pdmnbor;
  i5_dm_interface_type *pdmif;
  int rc = 0;
  unsigned int remaining, fullPacket;

  if ((pmem = (unsigned char *)malloc(I5_PACKET_BUF_LEN)) == NULL) {
    printf("malloc error\n");
    return -1;
  }

  i5MessageGetPacketSpace(pmsg, &remaining, &fullPacket);

  i5TraceInfo ("remain = %d  fullPacket = %d\n", remaining, fullPacket);

  if (remaining < sizeof(i5_tlv_t) + MAC_ADDR_LEN + MAC_ADDR_LEN) {
    /* Can't fit even one interface MAC ADDR + 1 legacy neighbor MAC ADDR */
    remaining = fullPacket;
    i5TraceInfo("Set remain = %d full packet size\n", remaining);
  }


  if ((pdmdev = i5DmDeviceFind(i5_config.i5_mac_address)) != NULL) {
    pdmif = (i5_dm_interface_type *)pdmdev->interface_list.ll.next;
    while ((pdmif != NULL) && (rc == 0)) {

      pbuf = pmem + sizeof(i5_tlv_t); // Header filled at the end
      remaining -= sizeof(i5_tlv_t);
      memcpy(pbuf, pdmif->InterfaceId, MAC_ADDR_LEN);
      
      pbuf += MAC_ADDR_LEN;
      pdmnbor = (i5_dm_legacy_neighbor_type *)pdmdev->legacy_list.ll.next;
      remaining -= MAC_ADDR_LEN;

      while (pdmnbor != NULL) {
        if (memcmp(pdmif->InterfaceId, pdmnbor->LocalInterfaceId, MAC_ADDR_LEN) == 0) {
          memcpy(pbuf, pdmnbor->NeighborInterfaceId, MAC_ADDR_LEN);
          pbuf += MAC_ADDR_LEN;
          remaining -= MAC_ADDR_LEN;

          if (remaining < MAC_ADDR_LEN) {
            i5TraceInfo ("Out of room\n");
            ptlv = (i5_tlv_t *)pmem;
            ptlv->type = i5TlvLegacyNeighborDeviceType;
            ptlv->length = htons(pbuf-pmem-3);
            rc = i5MessageInsertTlv(pmsg, pmem, pbuf-pmem);

            i5TraceInfo ("Resetting for full packet size \n");
            remaining = fullPacket - sizeof(i5_tlv_t) - MAC_ADDR_LEN;
            pbuf = pmem + sizeof(i5_tlv_t) + MAC_ADDR_LEN;
          }
        }
        pdmnbor = pdmnbor->ll.next;
      }

      if (pbuf - pmem > MAC_ADDR_LEN + 3) {
        /* This interface has neighbors */
        ptlv = (i5_tlv_t *)pmem;
        ptlv->type = i5TlvLegacyNeighborDeviceType;
        ptlv->length = htons(pbuf-pmem-3);
        rc = i5MessageInsertTlv(pmsg, pmem, pbuf-pmem);
      }
      else {
        /* This interface had no legacy neighbors, so rewind the "remaining" count */
        remaining += 3 + MAC_ADDR_LEN;
      }
      pdmif = pdmif->ll.next;
    }
  }

  free(pmem);
  return (rc);
}

int i5TlvLegacyNeighborDeviceTypeExtract(i5_message_type *pmsg, unsigned char *pdevid)
{
  unsigned char *pvalue;
  unsigned int length, pos;
  unsigned char local_interface[MAC_ADDR_LEN];
  unsigned char neighbor_interface[MAC_ADDR_LEN];
  int rc = 0;

  i5Trace("\n");
  i5MessageReset(pmsg);
  i5DmLegacyNeighborPending(pdevid);
  while ((rc = i5MessageTlvExtract(pmsg, i5TlvLegacyNeighborDeviceType, &length, &pvalue, i5MessageTlvExtractWithoutReset)) == 0) {
    pos = 0;
    if (length >= MAC_ADDR_LEN) {
      memcpy(local_interface, &pvalue[pos], MAC_ADDR_LEN);
      pos += MAC_ADDR_LEN;
      while (length >= pos + MAC_ADDR_LEN) {
        memcpy(neighbor_interface, &pvalue[pos], MAC_ADDR_LEN);
        pos += MAC_ADDR_LEN;
        i5DmLegacyNeighborUpdate(pdevid, local_interface, neighbor_interface);
      }
    }
  }
  i5DmLegacyNeighborDone(pdevid);

  /* This is an optional TLV */
  return 0;
}

int i5Tlv1905NeighborDeviceTypeInsert(i5_message_type *pmsg)
{
  unsigned char *pbuf, *pmem;
  i5_tlv_t *ptlv;
  i5_dm_device_type *pdmdev;
  i5_dm_1905_neighbor_type *pdmnbor;
  i5_dm_interface_type *pdmif;
  int rc = 0;

  if ((pmem = (unsigned char *)malloc(I5_PACKET_BUF_LEN)) == NULL) {
    printf("malloc error\n");
    return -1;
  }

  if ((pdmdev = i5DmDeviceFind(i5_config.i5_mac_address)) != NULL) {
    pdmif = (i5_dm_interface_type *)pdmdev->interface_list.ll.next;
    while ((pdmif != NULL) && (rc == 0)) {
      pbuf = pmem + 3; // Header filled at the end
      memcpy(pbuf, pdmif->InterfaceId, MAC_ADDR_LEN);
      pbuf += MAC_ADDR_LEN;
      pdmnbor = (i5_dm_1905_neighbor_type *)pdmdev->neighbor1905_list.ll.next;
      while (pdmnbor != NULL) {
        if (memcmp(pdmif->InterfaceId, pdmnbor->LocalInterfaceId, MAC_ADDR_LEN) == 0) {
          memcpy(pbuf, pdmnbor->Ieee1905Id, MAC_ADDR_LEN);
          pbuf += MAC_ADDR_LEN;
          *pbuf = pdmnbor->IntermediateLegacyBridge;
          pbuf++;
        }
        pdmnbor = pdmnbor->ll.next;
      }
      if (pbuf - pmem > MAC_ADDR_LEN + 3) {
        // This interface has neighbors
        ptlv = (i5_tlv_t *)pmem;
        ptlv->type = i5Tlv1905NeighborDeviceType;
        ptlv->length = htons(pbuf-pmem-3);
        rc = i5MessageInsertTlv(pmsg, pmem, pbuf-pmem);
      }
      pdmif = pdmif->ll.next;
    }
  }

  free(pmem);
  return (rc);
}

int i5Tlv1905NeighborDeviceTypeExtract(i5_message_type *pmsg, unsigned char *pdevid)
{
  unsigned char *pvalue;
  unsigned int length, pos;
  unsigned char local_interface[MAC_ADDR_LEN];
  unsigned char neighbor_al_mac_address[MAC_ADDR_LEN];
  unsigned char intermediate_legacy_bridge;
  int rc = 0;

  i5Trace("\n");
  i5Dm1905NeighborPending(pdevid);
  i5MessageReset(pmsg);
  while (i5MessageTlvExtract(pmsg, i5Tlv1905NeighborDeviceType, &length, &pvalue, i5MessageTlvExtractWithoutReset) == 0) {
    if (length == 0)
        continue;
    if ((length >= MAC_ADDR_LEN) && ((length - MAC_ADDR_LEN) % (MAC_ADDR_LEN + 1) == 0)) {      
      pos = 0;
      memcpy(local_interface, &pvalue[pos], MAC_ADDR_LEN);
      pos += MAC_ADDR_LEN;

      while (length >= pos + MAC_ADDR_LEN + 1) {
        memcpy(neighbor_al_mac_address, &pvalue[pos], MAC_ADDR_LEN);
        pos += MAC_ADDR_LEN;
        intermediate_legacy_bridge = pvalue[pos] & 0x01;
        pos++;

        /* if pdevid has a neigbour pointing back to us then we can use the
           rx interface to fill in the neighbour id for pdevid's neighbour entry */
        i5Dm1905NeighborUpdate(pdevid, local_interface, neighbor_al_mac_address, NULL, &intermediate_legacy_bridge,
                               i5SocketGetIfName(pmsg->psock), i5SocketGetIfIndex(pmsg->psock), 1);
        i5DmDeviceNewIfNew(neighbor_al_mac_address);
      }
    } else {
      i5TraceError("Bad TLV length: i5Tlv1905NeighborDeviceType: Length: %d\n", length);
      rc = -1;
      break;
    }
  }
  i5Dm1905NeighborDone(pdevid);
  i5DmTopologyFreeUnreachableDevices();

  return rc;
}

int i5TlvSearchedRoleTypeInsert(i5_message_type *pmsg)
{
    unsigned char buf[sizeof(i5_tlv_t) + i5TlvSearchRole_Length];
    i5_tlv_t *ptlv = (i5_tlv_t *)buf;
    int len = 0;
    
    ptlv->type = i5TlvSearchedRoleType;
    ptlv->length = htons(i5TlvSearchRole_Length);
    len += sizeof(i5_tlv_t);
    buf[len] = (unsigned char)i5TlvRole_Registrar;
    len += i5TlvSearchRole_Length;
    
    return (i5MessageInsertTlv(pmsg, buf, len));
}

int i5TlvSearchedRoleTypeExtract(i5_message_type *pmsg, unsigned char *searchRole)
{
    unsigned char *pvalue;
    unsigned int length;
    
    if (i5MessageTlvExtract(pmsg, i5TlvSearchedRoleType, &length, &pvalue, i5MessageTlvExtractWithReset) == 0) {
      if (length == i5TlvSearchRole_Length) {
        memcpy(searchRole, pvalue, i5TlvSearchRole_Length);
        return 0;
      }
    }
    return -1;
}

unsigned int i5TlvGetFreqBandFromMediaType(unsigned short mediaType)
{
    switch (mediaType) {
      case I5_MEDIA_TYPE_WIFI_B:
      case I5_MEDIA_TYPE_WIFI_G:
      case I5_MEDIA_TYPE_WIFI_N24:
        return i5MessageFreqBand_802_11_2_4Ghz;
      case I5_MEDIA_TYPE_WIFI_A:
      case I5_MEDIA_TYPE_WIFI_N5:
      case I5_MEDIA_TYPE_WIFI_AC:
        return i5MessageFreqBand_802_11_5Ghz;
      case I5_MEDIA_TYPE_WIFI_AD:
        return i5MessageFreqBand_802_11_60Ghz;
    }
    return i5MessageFreqBand_Reserved;
}

int i5TlvAutoconfigFreqBandTypeInsert(i5_message_type *pmsg, unsigned int freqBand)
{    
    unsigned char buf[sizeof(i5_tlv_t) + i5TlvAutoConfigFreqBand_Length];
    i5_tlv_t *ptlv = (i5_tlv_t *)buf;
    int len = 0;

    ptlv->type = i5TlvAutoconfigFreqBandType;
    ptlv->length = htons(i5TlvAutoConfigFreqBand_Length);
    len += sizeof(i5_tlv_t);

    buf[len] = (unsigned char)freqBand;  
    len += i5TlvAutoConfigFreqBand_Length;

    return (i5MessageInsertTlv(pmsg, buf, len));
}

int i5TlvAutoconfigFreqBandTypeExtract(i5_message_type *pmsg, unsigned char *autoconfigFreqBand)
{
    unsigned char *pvalue;
    unsigned int length;
    
    if (i5MessageTlvExtract(pmsg, i5TlvAutoconfigFreqBandType, &length, &pvalue, i5MessageTlvExtractWithReset) == 0) {
      if (length == i5TlvAutoConfigFreqBand_Length) {
        memcpy(autoconfigFreqBand, pvalue, i5TlvAutoConfigFreqBand_Length);
        return 0;
      }
    }
    return -1;
}

int i5TlvSupportedRoleTypeInsert(i5_message_type *pmsg)
{
    unsigned char buf[sizeof(i5_tlv_t) + i5TlvSupportedRole_Length];
    i5_tlv_t *ptlv = (i5_tlv_t *)buf;
    int len = 0;
    
    ptlv->type = i5TlvSupportedRoleType;
    ptlv->length = htons(i5TlvSupportedRole_Length);
    len += sizeof(i5_tlv_t);
    buf[len] = (unsigned char)i5TlvRole_Registrar;
    len += i5TlvSupportedRole_Length;
    
    return (i5MessageInsertTlv(pmsg, buf, len));
}

int i5TlvSupportedRoleTypeExtract(i5_message_type *pmsg, unsigned char *supportedRole)
{
    unsigned char *pvalue;
    unsigned int length;
    
    if (i5MessageTlvExtract(pmsg, i5TlvSupportedRoleType, &length, &pvalue, i5MessageTlvExtractWithReset) == 0) {
      if (length == i5TlvSupportedRole_Length) {
        memcpy(supportedRole, pvalue, i5TlvSupportedRole_Length);
        return 0;
      }
    }
    return -1;
}

int i5TlvSupportedFreqBandTypeInsert(i5_message_type *pmsg, unsigned int freqBand)
{
    unsigned char buf[sizeof(i5_tlv_t) + i5TlvSupportedFreqBand_Length];
    i5_tlv_t *ptlv = (i5_tlv_t *)buf;
    int len = 0;

    ptlv->type = i5TlvSupportedFreqBandType;
    ptlv->length = htons(i5TlvSupportedFreqBand_Length);
    len += sizeof(i5_tlv_t);

    buf[len] = (unsigned char)freqBand;  
    len += i5TlvSupportedFreqBand_Length;

    return (i5MessageInsertTlv(pmsg, buf, len));
}

int i5TlvSupportedFreqBandTypeExtract(i5_message_type *pmsg, unsigned char *supportedFreqBand)
{
    unsigned char *pvalue;
    unsigned int length;
    
    if (i5MessageTlvExtract(pmsg, i5TlvSupportedFreqBandType, &length, &pvalue, i5MessageTlvExtractWithReset) == 0) {
      if (length == i5TlvSupportedFreqBand_Length) {
        memcpy(supportedFreqBand, pvalue, i5TlvSupportedFreqBand_Length);
        return 0;
      }
    }
    return -1;
}

int i5TlvWscTypeInsert(i5_message_type *pmsg, unsigned char const * wscPacket, unsigned wscLength)
{
    int totalLength = sizeof(i5_tlv_t) + wscLength;
    unsigned char buf[totalLength];
    i5_tlv_t *ptlv = (i5_tlv_t *)buf;

    ptlv->type = i5TlvWscType;
    ptlv->length = htons(wscLength);
    memcpy(&buf[sizeof(i5_tlv_t)], wscPacket, wscLength);

    return (i5MessageInsertTlv(pmsg, buf, totalLength));
}

int i5TlvWscTypeExtract(i5_message_type *pmsg, unsigned char * wscPacket, unsigned maxWscLength, unsigned *pactualWscLength)
{
    unsigned char *pvalue;
    
    if (i5MessageTlvExtract(pmsg, i5TlvWscType, pactualWscLength, &pvalue, i5MessageTlvExtractWithReset) == 0) {
      if (*pactualWscLength < maxWscLength) {
        memcpy(wscPacket, pvalue, *pactualWscLength);
        return 0;
      }
    }
    return -1;
}

int i5TlvPushButtonEventNotificationTypeInsert(i5_message_type *pmsg, unsigned char* genericPhyIncluded)
{
  unsigned char *pBuf;
  i5_tlv_t *ptlv;
  unsigned int index = 0;
  i5_dm_device_type *pdmdev;
  i5_dm_interface_type *pdmif;
  unsigned int interfaceCount = 0;
  unsigned int bufLength = sizeof(i5_tlv_t) + i5TlvPushButtonNotificationMediaCount_Length;
  int rc;
  
  pdmdev = i5DmDeviceFind(i5_config.i5_mac_address);
  if ( NULL == pdmdev ) {
    return -1;
  }
  
  pdmif = (i5_dm_interface_type *)pdmdev->interface_list.ll.next;
  while (pdmif != NULL) {
    i5Trace("%p, len %d, sec %d\n", pdmif, pdmif->MediaSpecificInfoSize, pdmif->SecurityStatus);
    if ((1 == pdmif->SecurityStatus) &&
        ((I5_MEDIA_TYPE_UNKNOWN != pdmif->MediaType) ||
         ((i5DmIsInterfacePlc(pdmif->MediaType, pdmif->netTechOui)) && (i5DmAreThereNodesWithVersion(I5_DM_NODE_VERSION_1905) ) )
        )
       ) {
      bufLength += pdmif->MediaSpecificInfoSize + 3;
      interfaceCount++;
    }
    pdmif = pdmif->ll.next;
  }

  pBuf = (unsigned char *)malloc(bufLength);
  ptlv = (i5_tlv_t *)pBuf;
  ptlv->type   = i5TlvPushButtonEventNotificationType;
  ptlv->length = htons(bufLength - sizeof(i5_tlv_t));
  index+=sizeof(i5_tlv_t);
  i5Trace("Insert PB - count is %d, message Len = %d, index %d\n", interfaceCount, bufLength, index);
  pBuf[index] = interfaceCount;
  index++;

  pdmif = (i5_dm_interface_type *)pdmdev->interface_list.ll.next;
  while (pdmif != NULL) {
    if (1 == pdmif->SecurityStatus) {
      i5Trace("Mediatype 0x%04x Media Info Size %d\n", pdmif->MediaType, pdmif->MediaSpecificInfoSize);
      if (I5_MEDIA_TYPE_UNKNOWN == pdmif->MediaType) {
        *genericPhyIncluded = 1;
        if ((i5DmIsInterfacePlc(pdmif->MediaType, pdmif->netTechOui)) && (i5DmAreThereNodesWithVersion(I5_DM_NODE_VERSION_1905) )) {
          *((unsigned short *)&pBuf[index]) = I5_MEDIA_TYPE_1901_FFT;
          index+=2;
          pBuf[index] = (unsigned char)pdmif->MediaSpecificInfoSize;
          index++;
          memcpy(&pBuf[index], pdmif->MediaSpecificInfo, pdmif->MediaSpecificInfoSize);
          index+=pdmif->MediaSpecificInfoSize;
        }
      }
      else {
        *((unsigned short *)&pBuf[index]) = htons(pdmif->MediaType);
        index+=2;
        pBuf[index] = (unsigned char)pdmif->MediaSpecificInfoSize;
        index++;
        memcpy(&pBuf[index], pdmif->MediaSpecificInfo, pdmif->MediaSpecificInfoSize);
        index+=pdmif->MediaSpecificInfoSize;
      }
    }
    pdmif = pdmif->ll.next;
  }

  rc = i5MessageInsertTlv(pmsg, pBuf, bufLength);
  free(pBuf);
  
  return rc;
}

int i5TlvPushButtonEventNotificationTypeExtract(i5_message_type * pmsg, unsigned int *pMediaCount, unsigned short **pMediaList)
{
  int             rc;
  unsigned char  *pValue;
  unsigned int    length;
  unsigned int    i;
  unsigned int    mediaCount = 0;
  unsigned short *pBuf = NULL;

  rc = i5MessageTlvExtract(pmsg, i5TlvPushButtonEventNotificationType, &length, &pValue, i5MessageTlvExtractWithReset);
  if (rc == 0) {
    mediaCount = *pValue;
    pValue += 1;
    
    i5Trace("MediaCount is %u\n", mediaCount);
    pBuf = (unsigned short *)malloc(mediaCount * 2);
    for(i=0; i<mediaCount; i++) {
      pBuf[i] = ntohs(*((unsigned short *)pValue));
      i5Trace("MediaType is %x\n", pBuf[i]);
      pValue += 2;
      pValue += (*pValue) + 1;
    }
  }
  *pMediaCount = mediaCount;
  *pMediaList = pBuf;

  return 0;
}

int i5TlvPushButtonGenericPhyEventNotificationTypeExtract (i5_message_type * pmsg, unsigned int *pMediaCount, unsigned char **pMediaList)
{
  int             rc;
  unsigned char  *pValue;
  unsigned int    length;
  unsigned int    i;
  unsigned int    mediaCount = 0;
  unsigned char  *pBuf = NULL;
  unsigned char   mediaInfoSize = 0;

  rc = i5MessageTlvExtract(pmsg, i5TlvPushButtonGenericPhyEventNotificationType, &length, &pValue, i5MessageTlvExtractWithReset);
  if (rc == 0) {
    mediaCount = *pValue;
    pValue += 1;
    
    i5Trace("MediaCount is %u\n", mediaCount);
    pBuf = (unsigned char *)malloc(mediaCount * 4);
    for(i=0; i<mediaCount; i++) {
      memcpy (&pBuf[4*i], pValue, 4);
      i5Trace("OUI is %x:%x:%x Variant %x\n", pBuf[4*i],pBuf[4*i+1],pBuf[4*i+2],pBuf[4*i+3] );
      pValue += 4;
      mediaInfoSize = *pValue;
      pValue += 1;
      // Ignore "Media Specific Info"
      pValue += mediaInfoSize;
    }
  }
  *pMediaCount = mediaCount;
  *pMediaList = pBuf;

  return 0;
}

int i5TlvPushButtonEventNotificationTypeExtractFree(unsigned short *pMediaList)
{
  if ( pMediaList ) {
    free(pMediaList);
  }
  return 0;
}

int i5TlvPushButtonGenericPhyEventNotificationTypeExtractFree(unsigned char *pPhyMediaList)
{
  if ( pPhyMediaList ) {
    free(pPhyMediaList);
  }
  return 0;
}

int i5TlvPushButtonGenericPhyEventNotificationTypeInsert(i5_message_type *pmsg)
{
  unsigned char *pBuf;
  i5_tlv_t *ptlv;
  unsigned int index = 0;
  i5_dm_device_type *pdmdev;
  i5_dm_interface_type *pdmif;
  unsigned int interfaceCount = 0;
  unsigned int bufLength = sizeof(i5_tlv_t) + i5TlvPushButtonNotificationMediaCount_Length;
  int rc;
  
  pdmdev = i5DmDeviceFind(i5_config.i5_mac_address);
  if ( NULL == pdmdev ) {
    return -1;
  }
  
  pdmif = (i5_dm_interface_type *)pdmdev->interface_list.ll.next;
  while (pdmif != NULL) {
    i5Trace("%p, len %d, sec %d\n", pdmif, pdmif->MediaSpecificInfoSize, pdmif->SecurityStatus);
    if ((1 == pdmif->SecurityStatus) && (i5DmIsInterfacePlc(pdmif->MediaType, pdmif->netTechOui) )) {
      bufLength += pdmif->MediaSpecificInfoSize + 5;
      interfaceCount++;
    }
    pdmif = pdmif->ll.next;
  }

  pBuf = (unsigned char *)malloc(bufLength);
  ptlv = (i5_tlv_t *)pBuf;
  ptlv->type   = i5TlvPushButtonGenericPhyEventNotificationType;
  ptlv->length = htons(bufLength - sizeof(i5_tlv_t));
  index += sizeof(i5_tlv_t);
  i5Trace("Insert Gen PHY PB - count is %d, message Len = %d, index %d\n", interfaceCount, bufLength, index);
  pBuf[index] = interfaceCount;
  index++;

  pdmif = (i5_dm_interface_type *)pdmdev->interface_list.ll.next;
  while (pdmif != NULL) {
    if (1 == pdmif->SecurityStatus) {
      if (I5_MEDIA_TYPE_UNKNOWN == pdmif->MediaType) {
        memcpy( &pBuf[index], pdmif->netTechOui, 3);
        index+=3;
        pBuf[index] = (unsigned char)pdmif->netTechVariant;
        index++;
        pBuf[index] = (unsigned char)pdmif->MediaSpecificInfoSize;
        index++;
        memcpy(&pBuf[index], pdmif->MediaSpecificInfo, pdmif->MediaSpecificInfoSize);
        index+=pdmif->MediaSpecificInfoSize;
      }
    }
    pdmif = pdmif->ll.next;
  }

  rc = i5MessageInsertTlv(pmsg, pBuf, bufLength);
  free(pBuf);
  
  return rc;
}

static int i5Tlv_brcm_getRoutingTlvSize (i5_routing_table_type *table) 
{
  int totalLength = sizeof(i5_tlv_t) + sizeof(table->numEntries);
  int entry = 0;

  i5_routing_table_entry *currEntry = (i5_routing_table_entry *)table->entryList.ll.next;
  
  for ( ; (entry < table->numEntries) && currEntry ; entry ++) {
    totalLength += MAC_ADDR_LEN;                                    /* 6 bytes for the interface MAC */
    totalLength += sizeof(table->entryList.numDestinations);        /* 1 byte for the number of Destinations */

    totalLength += table->entryList.numDestinations * MAC_ADDR_LEN; /* 6 bytes for each Destination */

    currEntry = (i5_routing_table_entry *)currEntry->ll.next;
  }
  return totalLength;
}

/* This TLV should only be inserted nested in the Vendor Specific TLV */
int i5Tlv_brcm_RoutingTableInsert (i5_message_type * pmsg, i5_routing_table_type *table)
{
  int totalLength = i5Tlv_brcm_getRoutingTlvSize(table);  
  unsigned char buf[totalLength];
  i5_tlv_t *ptlv = (i5_tlv_t *)buf;
  unsigned int entryIndex = 0;
  int index = 0;

  i5_routing_table_entry *currEntry = (i5_routing_table_entry *)table->entryList.ll.next;

  ptlv->type = i5TlvBrcmRoutingTableType;
  ptlv->length = htons(totalLength - sizeof(i5_tlv_t));
  index += sizeof(i5_tlv_t);
   
  buf[index] = table->numEntries;
  index += sizeof(table->numEntries);

  for ( ; (entryIndex < table->numEntries) && currEntry; entryIndex++) {
    unsigned int destIndex = 0;
    i5_routing_destination *currDest = (i5_routing_destination *)currEntry->destinationList.ll.next;

    memcpy (&buf[index], currEntry->interfaceMac, MAC_ADDR_LEN);
    index += MAC_ADDR_LEN;
    
    buf[index] = currEntry->numDestinations;
    index += sizeof(currEntry->numDestinations);

    for ( ; (destIndex < currEntry->numDestinations) && currDest; destIndex ++) {
      memcpy (&buf[index], currDest->macAddress, MAC_ADDR_LEN);
      index += MAC_ADDR_LEN;

      currDest = (i5_routing_destination *)currDest->ll.next;
    }

    currEntry = (i5_routing_table_entry *)currEntry->ll.next;
  }

  return (i5MessageInsertTlv(pmsg, buf, totalLength));
}


int i5TlvLinkMetricQueryInsert (i5_message_type * pmsg, enum i5TlvLinkMetricNeighbour_Values specifyAddress, 
                                unsigned char const * mac_address, enum i5TlvLinkMetricType_Values metricTypes)
{
  int totalLength = sizeof(i5_tlv_t) + i5TlvLinkMetricNeighbour_Length + 
                    ((specifyAddress == i5TlvLinkMetricNeighbour_Specify) ? MAC_ADDR_LEN : 0) + 
                    i5TlvLinkMetricType_Length;
  unsigned char buf[totalLength];
  i5_tlv_t *ptlv = (i5_tlv_t *)buf;
  unsigned int index = 0;

  ptlv->type = i5TlvLinkMetricQueryType;
  ptlv->length = htons(totalLength - sizeof(i5_tlv_t));
  index += sizeof(i5_tlv_t);
   
  buf[index] = (unsigned char)specifyAddress;
  index += i5TlvLinkMetricNeighbour_Length;

  if (specifyAddress == i5TlvLinkMetricNeighbour_Specify) {
    if (mac_address) {
      memcpy (&buf[index], mac_address, MAC_ADDR_LEN);
      index += MAC_ADDR_LEN;
    } else {
      /* The caller wants to specify a MAC, but didn't provide one */
      return -1;
    }
  }

  buf[index] = (unsigned char)metricTypes;

  return (i5MessageInsertTlv(pmsg, buf, totalLength));
}

int i5TlvLinkMetricQueryExtract(i5_message_type * pmsg,
                                unsigned char * neighbours,
                                unsigned char * alMacAddress,
                                enum i5TlvLinkMetricType_Values * metricsRequested)
{
  int             rc;
  unsigned char  *pValue;
  unsigned int    length;

  rc = i5MessageTlvExtract(pmsg, i5TlvLinkMetricQueryType, &length, &pValue, i5MessageTlvExtractWithReset);
  if (rc == 0) {
    if (length == 8) {
      *neighbours = pValue[0];
      pValue += i5TlvLinkMetricNeighbour_Length;
      memcpy(alMacAddress, pValue, MAC_ADDR_LEN);
      pValue += MAC_ADDR_LEN;
      *metricsRequested = (enum i5TlvLinkMetricType_Values) pValue[0];
    }
    else if (length == 2) {
      *neighbours = pValue[0];
      pValue += i5TlvLinkMetricNeighbour_Length;
      if (*neighbours == i5TlvLinkMetricNeighbour_Specify) {
        i5TraceError("Illegal Packet: length is 2 but 'specify neighbour' is chosen.\n");
        return -1;
      }
      /* There is no MAC address field in the 2-byte version of the query */
      *metricsRequested = (enum i5TlvLinkMetricType_Values) pValue[0];      
    }
    else {
      i5TraceError("Read Error: packet length must be 8 or 2.\n");
    }
  } 
  else {
    i5Trace("Read failure rc=%d length=%d\n",rc,length);
    return -1;
  }

  return 0;
}

/* The only "result code" is "Invalid" meaning that the neighbor info was requested for a non-neighbor */
int i5TlvLinkMetricResultCodeInsert (i5_message_type * pmsg)
{
  int totalLength = sizeof(i5_tlv_t) + i5TlvLinkMetricResultCode_Length;
  unsigned char buf[totalLength];
  i5_tlv_t *ptlv = (i5_tlv_t *)buf;

  ptlv->type = i5TlvLinkMetricResultCodeType;
  ptlv->length = htons(i5TlvLinkMetricResultCode_Length);
  buf[sizeof(i5_tlv_t)] = i5TlvLinkMetricResultCode_InvalidNeighbor;

  return (i5MessageInsertTlv(pmsg, buf, totalLength));
}

/* This function is more about future-proofing
 * The only current "result code" is "Invalid" meaning that the request for neighbor info has been rejected because we don't have that neighbor
 * So really, someone receiving a "Link Metric Result Code" Packet has no need to process the TLV
 * since the only "resultCode" they'll ever get is "0"
 */
int i5TlvLinkMetricResultCodeExtract (i5_message_type * pmsg, enum i5TlvLinkMetricResultCode_Values * resultCode)
{
  int             rc;
  unsigned char  *pValue;
  unsigned int    length;
   
  rc = i5MessageTlvExtract(pmsg, i5TlvLinkMetricResultCodeType, &length, &pValue, i5MessageTlvExtractWithReset);
  if ((rc == 0) && (length == i5TlvLinkMetricResultCode_Length)) {
    *resultCode = pValue[0];
  } else {
    i5Trace("Read failure rc=%d length=%d\n",rc,length);
    return -1;
  }

  return 0;
}

int i5TlvLinkMetricTxInsert (i5_message_type * pmsg, 
                             unsigned char const * local_al_mac, unsigned char const * neighbor_al_mac, 
                             i5_tlv_linkMetricTx_t const * txStats, int numLinks)
{
  int totalLength = sizeof(i5_tlv_t) + i5TlvLinkMetricTxOverhead_Length + i5TlvLinkMetricTxPerLink_Length * numLinks;
  unsigned char buf[totalLength];
  i5_tlv_t *ptlv = (i5_tlv_t *)buf;
  unsigned int index = 0;
  unsigned int linkCounter = 0;
  unsigned int remaining, fullPacket;

  i5Trace("\n");

  i5MessageGetPacketSpace(pmsg, &remaining, &fullPacket);

  i5TraceInfo("remain = %d  fullPacket = %d\n", remaining, fullPacket);

  if (remaining < sizeof(i5_tlv_t) + i5TlvLinkMetricTxOverhead_Length + i5TlvLinkMetricTxPerLink_Length) {
    /* Can't fit even one metric with what's left */
    remaining = fullPacket;
    i5TraceInfo("Setting remaining = %d full packet size\n", remaining);
  }
  
  ptlv->type = i5TlvTransmitterLinkMetricType;
  ptlv->length = 0; /* this is a variable length packet, add as we go */
  index += sizeof(i5_tlv_t);
  remaining -= sizeof(i5_tlv_t);

  memcpy(&buf[index], local_al_mac, MAC_ADDR_LEN);
  index += MAC_ADDR_LEN;

  memcpy(&buf[index], neighbor_al_mac, MAC_ADDR_LEN);
  index += MAC_ADDR_LEN;
  ptlv->length = htons( ntohs(ptlv->length) + i5TlvLinkMetricTxOverhead_Length);
  remaining -= i5TlvLinkMetricTxOverhead_Length;

  for (;linkCounter < numLinks;linkCounter++) {
    memcpy (&buf[index], &txStats[linkCounter], (MAC_ADDR_LEN * 2));
    index += (MAC_ADDR_LEN * 2);
    index += i5_cpy_host16_to_netbuf(&buf[index], txStats[linkCounter].intfType);
    buf[index] = txStats[linkCounter].ieee8021BridgeFlag;
    index += 1;
    index += i5_cpy_host32_to_netbuf(&buf[index], txStats[linkCounter].packetErrors);
    index += i5_cpy_host32_to_netbuf(&buf[index], txStats[linkCounter].transmittedPackets);
    index += i5_cpy_host16_to_netbuf(&buf[index], txStats[linkCounter].macThroughPutCapacity);
    index += i5_cpy_host16_to_netbuf(&buf[index], txStats[linkCounter].linkAvailability);
    index += i5_cpy_host16_to_netbuf(&buf[index], txStats[linkCounter].phyRate);

    ptlv->length = htons( ntohs(ptlv->length) + i5TlvLinkMetricTxPerLink_Length);
    remaining -= i5TlvLinkMetricTxPerLink_Length;

    i5TraceInfo("Compare remain = %d  i5TlvLinkMetricTxPerLink_Length = %d\n", remaining, i5TlvLinkMetricTxPerLink_Length);
    if (remaining < i5TlvLinkMetricTxPerLink_Length)
    {
      /* Send the TLV */
      if (i5MessageInsertTlv(pmsg, buf, ntohs(ptlv->length) + sizeof(i5_tlv_t) ) ) {
        return -1;
      }
      i5TraceInfo("Resetting\n");
      /* Reset the TLV */
      ptlv->length = htons(i5TlvLinkMetricTxOverhead_Length);
      index = sizeof(i5_tlv_t) + i5TlvLinkMetricTxOverhead_Length;
      remaining = fullPacket - index;
    }    
  }

  if (ntohs(ptlv->length) > sizeof(i5_tlv_t) + i5TlvLinkMetricTxOverhead_Length) {
    return (i5MessageInsertTlv(pmsg, buf, ntohs(ptlv->length) + sizeof(i5_tlv_t) ) );
  }
  return 0;
}

/* return of 0  : okay
 * return of -1 : an error
 * return of -2 : not found
 * if numLinksReturned > maxLinks, this means that there were more links available, but they could not be returned
 *   (in this situation, the function returns 0 anyway, but the caller can tell the information is incomplete)
 */
int i5TlvLinkMetricTxExtract (i5_message_type * pmsg, 
                              unsigned char * reporter_al_mac, unsigned char * neighbor_al_mac, 
                              i5_tlv_linkMetricTx_t * txStats, int maxLinks, int *numLinksReturned)
{
  int             rc;
  unsigned char  *pValue;
  unsigned int    length;
  int linkCounter = 0;
   
  rc = i5MessageTlvExtract(pmsg, i5TlvTransmitterLinkMetricType, &length, &pValue, i5MessageTlvExtractWithoutReset);

  /* Validate the length */
  if (rc != 0) {
    return rc;
  }
  else if ((length - i5TlvLinkMetricTxOverhead_Length) % i5TlvLinkMetricTxPerLink_Length != 0) {
    i5Trace("Read failure rc=%d length=%d (must be %d + %d n) \n",rc,length,i5TlvLinkMetricTxOverhead_Length,i5TlvLinkMetricTxPerLink_Length);
    return -1;
  }

  *numLinksReturned = (length-i5TlvLinkMetricTxOverhead_Length) / i5TlvLinkMetricTxPerLink_Length;
  
  memcpy (reporter_al_mac, pValue, MAC_ADDR_LEN);
  pValue += MAC_ADDR_LEN;

  memcpy (neighbor_al_mac, pValue, MAC_ADDR_LEN);
  pValue += MAC_ADDR_LEN;

  for ( ; (linkCounter < *numLinksReturned) && (linkCounter < maxLinks) ; linkCounter++) {
    memcpy (&txStats[linkCounter], pValue, (MAC_ADDR_LEN * 2));
    pValue += (MAC_ADDR_LEN * 2);
    pValue += i5_cpy_netbuf_to_host16(&txStats[linkCounter].intfType, pValue);
    txStats[linkCounter].ieee8021BridgeFlag = pValue[0];
    pValue += 1;
    pValue += i5_cpy_netbuf_to_host32(&txStats[linkCounter].packetErrors, pValue);
    pValue += i5_cpy_netbuf_to_host32(&txStats[linkCounter].transmittedPackets, pValue);
    pValue += i5_cpy_netbuf_to_host16(&txStats[linkCounter].macThroughPutCapacity, pValue);
    pValue += i5_cpy_netbuf_to_host16(&txStats[linkCounter].linkAvailability, pValue);
    pValue += i5_cpy_netbuf_to_host16(&txStats[linkCounter].phyRate, pValue);
  }

  /* Everything is fine, numLinksReturned vs maxLinks will tell the caller anything else */
  return 0;
}

int i5TlvLinkMetricRxInsert (i5_message_type * pmsg, 
                             unsigned char const * local_al_mac, unsigned char const * neighbor_al_mac, 
                             i5_tlv_linkMetricRx_t const * rxStats, int numLinks)
{
  int totalLength = sizeof(i5_tlv_t) + i5TlvLinkMetricRxOverhead_Length + i5TlvLinkMetricRxPerLink_Length * numLinks;
  unsigned char buf[totalLength];
  i5_tlv_t *ptlv = (i5_tlv_t *)buf;
  unsigned int index = 0;
  unsigned int linkCounter = 0;
  unsigned int remaining, fullPacket;

  i5Trace("\n");

  i5MessageGetPacketSpace(pmsg, &remaining, &fullPacket);

  i5TraceInfo("remain = %d  fullPacket = %d\n", remaining, fullPacket);

  if (remaining < sizeof(i5_tlv_t) + i5TlvLinkMetricRxOverhead_Length + i5TlvLinkMetricRxPerLink_Length) {
    /* Can't fit even one metric with what's left */
    remaining = fullPacket;
    i5TraceInfo("Setting remain = %d full packet size\n", remaining);
  }

  ptlv->type = i5TlvReceiverLinkMetricType;
  ptlv->length = 0; /* this is a variable length packet */
  index += sizeof(i5_tlv_t);
  remaining -= sizeof(i5_tlv_t);

  memcpy(&buf[index], local_al_mac, MAC_ADDR_LEN);
  index += MAC_ADDR_LEN;
 
  memcpy(&buf[index], neighbor_al_mac, MAC_ADDR_LEN);
  index += MAC_ADDR_LEN;
  ptlv->length = htons( ntohs(ptlv->length) + i5TlvLinkMetricRxOverhead_Length);
  remaining -= i5TlvLinkMetricRxOverhead_Length;

  for (;linkCounter < numLinks; linkCounter++) {
    memcpy (&buf[index], &rxStats[linkCounter], (MAC_ADDR_LEN * 2));
    index += (MAC_ADDR_LEN * 2);
    index += i5_cpy_host16_to_netbuf(&buf[index], rxStats[linkCounter].intfType);
    index += i5_cpy_host32_to_netbuf(&buf[index], rxStats[linkCounter].packetErrors);
    index += i5_cpy_host32_to_netbuf(&buf[index], rxStats[linkCounter].receivedPackets);
    buf[index] = rxStats[linkCounter].rssi;
    index += 1;

    ptlv->length = htons( ntohs(ptlv->length) + i5TlvLinkMetricRxPerLink_Length);
    remaining -= i5TlvLinkMetricRxPerLink_Length;

    i5TraceInfo("Compare remain = %d  i5TlvLinkMetricRxPerLink_Length = %d\n", remaining, i5TlvLinkMetricRxPerLink_Length);
    if (remaining < i5TlvLinkMetricRxPerLink_Length)
    {
      /* Send the TLV */
      if (i5MessageInsertTlv(pmsg, buf, ntohs(ptlv->length) + sizeof(i5_tlv_t) ) ) {
        return -1;
      }
      i5TraceInfo("Resetting\n");
      /* Reset the TLV */
      ptlv->length = htons(i5TlvLinkMetricRxOverhead_Length);
      index = sizeof(i5_tlv_t) + i5TlvLinkMetricRxOverhead_Length;
      remaining = fullPacket - index;
    }    

  }

  if (ntohs(ptlv->length) > sizeof(i5_tlv_t) + i5TlvLinkMetricRxOverhead_Length) {
    return (i5MessageInsertTlv(pmsg, buf, ntohs(ptlv->length) + sizeof(i5_tlv_t) ) );
  }
  return 0;
}

/* return of 0  : okay
 * return of -1 : an error
 * return of -2 : not found
 * if numLinksReturned > maxLinks, this means that there were more links available, but they could not be returned
 *   (in this situation, the function returns 0 anyway, but the caller can tell the information is incomplete)
 */
int i5TlvLinkMetricRxExtract (i5_message_type * pmsg, 
                              unsigned char * reporter_al_mac, unsigned char * neighbor_al_mac, 
                              i5_tlv_linkMetricRx_t * rxStats, int maxLinks, int *numLinksReturned)
{
  int             rc;
  unsigned char  *pValue;
  unsigned int    length;
  int linkCounter = 0;
   
  rc = i5MessageTlvExtract(pmsg, i5TlvReceiverLinkMetricType, &length, &pValue, i5MessageTlvExtractWithoutReset);

  /* Validate the length */
  if (rc != 0) {
    return rc;
  }
  else if ((length - i5TlvLinkMetricRxOverhead_Length) % i5TlvLinkMetricRxPerLink_Length != 0) {
    i5Trace("Read failure rc=%d length=%d (must be %d + %d n) \n",rc,length,i5TlvLinkMetricRxOverhead_Length,i5TlvLinkMetricRxPerLink_Length);
    return -1;
  }

  *numLinksReturned = (length-i5TlvLinkMetricRxOverhead_Length) / i5TlvLinkMetricRxPerLink_Length;
  
  memcpy (reporter_al_mac, pValue, MAC_ADDR_LEN);
  pValue += MAC_ADDR_LEN;

  memcpy (neighbor_al_mac, pValue, MAC_ADDR_LEN);
  pValue += MAC_ADDR_LEN;

  for ( ; (linkCounter < *numLinksReturned) && (linkCounter < maxLinks) ; linkCounter++) {
    memcpy (&rxStats[linkCounter], pValue, (MAC_ADDR_LEN * 2));
    pValue += (MAC_ADDR_LEN * 2);
    pValue += i5_cpy_netbuf_to_host16(&rxStats[linkCounter].intfType, pValue);
    pValue += i5_cpy_netbuf_to_host32(&rxStats[linkCounter].packetErrors, pValue);
    pValue += i5_cpy_netbuf_to_host32(&rxStats[linkCounter].receivedPackets, pValue);
    rxStats[linkCounter].rssi = *pValue;
    pValue += 1;
  }

  /* Everything is fine, numLinksReturned vs maxLinks will tell the caller anything else */
  return 0;
}

void i5TlvLinkMetricResponseExtract(i5_message_type * pmsg)
{
  i5_dm_device_type *reportingDevice = NULL;

  i5MessageReset(pmsg);
  
  /* loop through all TLVs */
  while (1) {
    unsigned char reporter_al_mac[MAC_ADDR_LEN];
    unsigned char neighbor_al_mac[MAC_ADDR_LEN];
    i5_tlv_linkMetricTx_t txStats[3];
    int numLinks = 0;
    int linkIndex = 0;

    int rc = i5TlvLinkMetricTxExtract(pmsg, reporter_al_mac, neighbor_al_mac, txStats, 3, &numLinks);
    i5DmRefreshDeviceTimer(reporter_al_mac, 0);

    if (-2 == rc) {
      i5Trace("No more TxStats TLVs.\n");
      break;
    }
    else if (-1 == rc) {
      i5TraceError("Error in Tx TLV\n");
      break;
    }
    /* if txstats TLV, extract addr, addr, macthroughput, linkavail */
    i5Trace("TxStats received, processing.\n");

    reportingDevice = i5DmDeviceFind(reporter_al_mac);
    if (!reportingDevice) {
      i5TraceInfo("Reporting device " I5_MAC_DELIM_FMT " not found\n", I5_MAC_PRM(reporter_al_mac));
      continue;
    }

    i5TraceInfo("Reporter: " I5_MAC_DELIM_FMT " Regarding: " I5_MAC_DELIM_FMT " \n",
      I5_MAC_PRM(reporter_al_mac),
      I5_MAC_PRM(neighbor_al_mac));
    if (numLinks > 3) {
      numLinks = 3;
    }
    for ( ; linkIndex < numLinks ; linkIndex ++) {
      i5TraceInfo("Rep's If: " I5_MAC_DELIM_FMT " Other I/f: " I5_MAC_DELIM_FMT " %d/%d \n",
        I5_MAC_PRM(txStats[linkIndex].localInterface),
        I5_MAC_PRM(txStats[linkIndex].neighborInterface),
        txStats[linkIndex].linkAvailability,
        txStats[linkIndex].macThroughPutCapacity);
      i5_dm_1905_neighbor_type *neighbor = i5Dm1905NeighborFind(reportingDevice, txStats[linkIndex].localInterface, neighbor_al_mac);
      if (!neighbor) {
        i5TraceInfo("Neighbor device " I5_MAC_DELIM_FMT " not found\n", I5_MAC_PRM(neighbor_al_mac));
        continue;
      }
      i5Dm1905NeighborBandwidthUpdate(neighbor, txStats[linkIndex].macThroughPutCapacity, txStats[linkIndex].linkAvailability, 0,
                                      reporter_al_mac);
    }
  }
  
  i5MessageReset(pmsg);
  while (1) {
    unsigned char reporter_al_mac[MAC_ADDR_LEN];
    unsigned char neighbor_al_mac[MAC_ADDR_LEN];
    i5_tlv_linkMetricRx_t rxStats[3];
    int numLinks = 0;
    int rc = i5TlvLinkMetricRxExtract(pmsg, reporter_al_mac, neighbor_al_mac, rxStats, 3, &numLinks);
    i5DmRefreshDeviceTimer(reporter_al_mac, 0);
    if (rc != 0) {
      break;
    }
    i5Trace("RxStats received, ignoring.\n");
  }
}

int i5TlvLldpTypeInsert(i5_message_type *pmsg, const unsigned char *chassis_mac, const unsigned char *portid_mac)
{  
  unsigned char buf[32];
  int len = 0;
  unsigned short val;

  /* for LLDP - TLV type is 7 bits and TLV length is 9 bits */

  /* chassis ID */
  buf[len] = 0x2;              /* chassis ID type is 1 */
  len++;
  buf[len] = MAC_ADDR_LEN + 1; /* MAC addres + chassis subtype */
  len++;
  buf[len] = 0x4;              /* chassis ID subtype - 4 - MAC address */
  len++;
  memcpy(&buf[len], chassis_mac, MAC_ADDR_LEN);
  len+=MAC_ADDR_LEN;
   
  /* port ID */
  buf[len] = 0x4;              /* port ID type is 2 */
  len++;
  buf[len] = MAC_ADDR_LEN + 1; /* MAC addres + port id subtype */
  len++;
  buf[len] = 0x3;              /* port id subtype - 3 - MAC address */
  len++;
  memcpy(&buf[len], portid_mac, MAC_ADDR_LEN);
  len+=MAC_ADDR_LEN;

  /* TTL */
  buf[len] = 0x6;              /* port ID type is 3 */
  len++;
  buf[len] = 2;                /* ttl vlaue is two bytes  - value is 180 as per 1905 spec */
  len++;
  val = 180;
  i5_cpy_host16_to_netbuf(&buf[len], val);
  len+=2;

  /* End of LLDP */
  buf[len] = 0;                /* end of lldp type is 0 */
  len++;
  buf[len] = 0;                /* end of lldp lenght is 0 */
  len++;

  return (i5MessageInsertTlv(pmsg, buf, len));
}

int i5TlvLldpTypeExtract(i5_message_type *pmsg, unsigned char *neighbor_al_mac, unsigned char *neighbor_interface_mac)
{  
  unsigned char *ptr = &pmsg->ppkt->pbuf[0];
  int            index = sizeof(struct ethhdr);
  int            tlvType;
  int            tlvLen;

  /* for LLDP - TLV type is 7 bits and TLV length is 9 bits */

  while ( index < pmsg->ppkt->length ) {
    tlvType = ptr[index] >> 1;
    tlvLen  = (ptr[index] & 0x01) << 8;
    index++;
    tlvLen |= ptr[index];
    index++;
    if ( tlvLen + index > pmsg->ppkt->length ) {
      return -1;
    }

    if ( 0x00 == tlvType ) {
      break;
    }

    /* chassis ID */
    if ( 0x1 == tlvType ) {
      /* if chassis id sub type is not MAC address break */
      if (ptr[index] != 0x4) {
        break;
      }
      else {
        memcpy(neighbor_al_mac, &ptr[index+1], MAC_ADDR_LEN);
      }
    }

    /* port ID */
    if ( 0x2 == tlvType ) {
      /* if port id sub type is not MAC address break */
      if (ptr[index] != 0x3) {
        break;
      }
      else {
        memcpy(neighbor_interface_mac, &ptr[index+1], MAC_ADDR_LEN);
      }
    }
    
    index += tlvLen;
  }
   
  return 0;
}


/* Friendly Name TLV
 * Variable bytes: friendlyName
 */
int i5TlvFriendlyNameInsert(i5_message_type *pmsg, char const *friendlyName)
{
  int totalLength = sizeof(i5_tlv_t) + strlen((char *)friendlyName) + 1; /* room for the null char */
  char buf[totalLength];
  i5_tlv_t *ptlv = (i5_tlv_t *)buf;
  int index = 0;

  i5Trace("\n");

  ptlv->type = i5TlvBrcmFriendlyNameType;
  ptlv->length = htons(totalLength - sizeof(i5_tlv_t));
  index += sizeof(i5_tlv_t);
 
  strcpy(&buf[index], (char *) friendlyName);
  buf[totalLength-1] = '\0';

  return (i5MessageInsertTlv(pmsg, (unsigned char *)buf, totalLength));
}

int i5TlvFriendlyNameExtract(i5_message_type *pmsg, char *friendlyName, int maxFriendlyNameSize)
{
  int             rc;
  unsigned char  *pValue;
  unsigned int    length;
  int index = 0;

  rc = i5MessageTlvExtract(pmsg, i5TlvBrcmFriendlyNameType, &length, &pValue, i5MessageTlvExtractWithReset);
  if (rc == 0) {
    strncpy( friendlyName, (char *)&pValue[index], maxFriendlyNameSize);
    friendlyName[maxFriendlyNameSize-1] = '\0';   
  }
  else {
    i5Trace("Read failure rc=%d length=%d\n",rc,length);
    return -1;
  }

  return 0;
}

/* Friendly URL TLV
 * Variable bytes: Control URL
 */
int i5TlvFriendlyUrlInsert(i5_message_type *pmsg, unsigned char const *controlUrl)
{
  int totalLength = sizeof(i5_tlv_t) + strlen((char *)controlUrl) + 1; /* room for the null char */
  char buf[totalLength];
  i5_tlv_t *ptlv = (i5_tlv_t *)buf;
  int index = 0;

  i5Trace("\n");

  ptlv->type = i5TlvBrcmFriendlyUrlType;
  ptlv->length = htons(totalLength - sizeof(i5_tlv_t));
  index += sizeof(i5_tlv_t);
 
  strcpy(&buf[index], (char *) controlUrl);
  buf[totalLength-1] = '\0';

  return (i5MessageInsertTlv(pmsg, (unsigned char *)buf, totalLength));
}

int i5TlvFriendlyUrlExtract(i5_message_type *pmsg, unsigned char *controlUrl, int maxControlUrlSize)
{
  int             rc;
  unsigned char  *pValue;
  unsigned int    length;
  int index = 0;

  rc = i5MessageTlvExtract(pmsg, i5TlvBrcmFriendlyUrlType, &length, &pValue, i5MessageTlvExtractWithReset);
  if (rc == 0) {
    strncpy( (char *)controlUrl, (char *)&pValue[index], maxControlUrlSize);
    controlUrl[maxControlUrlSize-1] = '\0';   
  } else {
    i5Trace("Read failure rc=%d length=%d\n",rc,length);
    return -1;
  }

  return 0;
}

int i5TlvFriendlyIpv4Insert (i5_message_type * pmsg, i5_tlv_ipv4Type_t const *ipv4Info, unsigned char numEntries)
{
  int totalLength = sizeof(i5_tlv_t) + sizeof(char) + (sizeof(i5_tlv_ipv4Type_t) * numEntries);
  char buf[totalLength];
  i5_tlv_t *ptlv = (i5_tlv_t *)buf;
  int index = 0;
  int entryCounter = 0;

  i5Trace("Inserting %d entries of size %d\n", numEntries, (int)sizeof(i5_tlv_ipv4Type_t) );

  ptlv->type = i5TlvBrcmFriendlyIpv4Type;
  ptlv->length = htons(totalLength - sizeof(i5_tlv_t));
  index += sizeof(i5_tlv_t);

  buf[index] = (char) numEntries;
  index ++;

  for ( ; entryCounter < numEntries; entryCounter ++) {
    // TBD - may have to use htonl() on these IP addresses.
    memcpy(&buf[index], (char *) (&ipv4Info[entryCounter]), sizeof(i5_tlv_ipv4Type_t) );
    index += sizeof(i5_tlv_ipv4Type_t);
  }

  return (i5MessageInsertTlv(pmsg, (unsigned char *)buf, totalLength));
}

int i5TlvFriendlyIpv4Extract (i5_message_type * pmsg, i5_tlv_ipv4Type_t *ipv4Info, unsigned char const maxEntries, unsigned char *numEntriesReturned)
{
  int             rc;
  unsigned char  *pValue;
  unsigned int    length;
  int index = 0;
  int entryCounter = 0;
  numEntriesReturned = 0;

  rc = i5MessageTlvExtract(pmsg, i5TlvBrcmFriendlyIpv4Type, &length, &pValue, i5MessageTlvExtractWithReset);
  if (rc == 0) {
    
    int entries = pValue[index];
    index ++;
    
    for ( ; (entryCounter < entries) && (entryCounter < maxEntries); entryCounter++) {
      // separate check for length violation
      if ( (index + sizeof(i5_tlv_ipv4Type_t) ) > length ) {
        i5TraceError("Packet not long enough for entry #%d\n", entryCounter + 1);
        if (numEntriesReturned) {
          return 0;
        }
        else {
          return -1;
        }
      }
      
      // TBD - probably some htonl() here, too
      memcpy( (char *)(&ipv4Info[entryCounter]), (char *)(&pValue[index]), sizeof(i5_tlv_ipv4Type_t) );
      numEntriesReturned ++;
      index += sizeof(i5_tlv_ipv4Type_t);
    }
    
  } else {
    i5Trace("Read failure rc=%d length=%d\n",rc,length);
    return -1;
  }

  return 0;
}


int i5TlvVendorSpecificTypeInsert(i5_message_type *pmsg, unsigned char *vendorSpec_msg, unsigned int vendorSpec_len)
{
  unsigned char *pbuf;
  i5_tlv_t *ptlv = NULL;
  unsigned short len = i5TlvVendorSpecificOui_Length + vendorSpec_len;
  int index = 0;
  int rc = 0;

  if ((pbuf = (unsigned char *)malloc(len + sizeof(i5_tlv_t))) == NULL) {
    printf("malloc error\n");
    return -1;
  }
  ptlv = (i5_tlv_t *)pbuf;
  ptlv->type = i5TlvVendorSpecificType;
  ptlv->length = htons(len);
  index += sizeof(i5_tlv_t);
  
  /* copy the OUI */
  pbuf [index]   = i5TlvVendorSpecificOui_Byte1;
  pbuf [index+1] = i5TlvVendorSpecificOui_Byte2;
  pbuf [index+2] = i5TlvVendorSpecificOui_Byte3;
  index += i5TlvVendorSpecificOui_Length;
  
  /* copy the vendor specific info */
  memcpy(&pbuf[index], vendorSpec_msg, vendorSpec_len);
  index += vendorSpec_len;

  rc = i5MessageInsertTlv(pmsg, pbuf, index);
  free (pbuf);
  return rc;  
}

/* This function returns the vendor specific part of the message in its entirety
 * (not including the OUI, but otherwise unparsed) 
 */
int i5TlvVendorSpecificTypeExtract(i5_message_type *pmsg, unsigned char **vendorSpec_data, unsigned int * vendorSpec_len)
{
  unsigned int length;

  if (i5MessageTlvExtract(pmsg, i5TlvVendorSpecificType, &length, vendorSpec_data, i5MessageTlvExtractWithReset) == 0) {
    i5TraceInfo("Extracted %d bytes at %p\n", length, *vendorSpec_data);
    if (((*vendorSpec_data)[0] == i5TlvVendorSpecificOui_Byte1) && 
        ((*vendorSpec_data)[1] == i5TlvVendorSpecificOui_Byte2) &&
        ((*vendorSpec_data)[2] == i5TlvVendorSpecificOui_Byte3)) {
      /* The user doesn't want to know about the OUI, so move the pointer past it */
      *vendorSpec_data += i5TlvVendorSpecificOui_Length;
      *vendorSpec_len = length - i5TlvVendorSpecificOui_Length;
      return 0;
    } 
    else {
      i5TraceInfo("Non Broadcom Vendor specific ID\n");
      return -1;
    }
     
  }
  return -1;
}


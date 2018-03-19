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

#ifndef _IEEE1905_TLV_H_
#define _IEEE1905_TLV_H_

#include "ieee1905_message.h"
/*
 * IEEE1905 TLVs
 */ 
enum i5TlvRole_Values {
  i5TlvRole_Registrar = 0,
  i5TlvRole_Reserved, /* All values 0x01 through through 0xff are reserved */
};

#define i5TlvMediaSpecificInfoWiFi_Length    10
#define i5TlvMediaSpecificInfo1901_Length    7

#define i5TlvSearchRole_Length    1
#define i5TlvSupportedRole_Length 1

#define i5TlvAutoConfigFreqBand_Length 1
#define i5TlvSupportedFreqBand_Length  1

#define i5TlvPushButtonNotificationMediaCount_Length  1

enum i5TlvLinkMetricNeighbour_Values {
  i5TlvLinkMetricNeighbour_All = 0,
  i5TlvLinkMetricNeighbour_Specify,
  i5TlvLinkMetricNeighbour_Invalid, /* All values 0x02 through through 0xff are reserved */
};
#define i5TlvLinkMetricNeighbour_Length 1

enum i5TlvLinkMetricType_Values {
  i5TlvLinkMetricType_TxOnly = 0,
  i5TlvLinkMetricType_RxOnly,
  i5TlvLinkMetricType_TxAndRx,
  i5TlvLinkMetricType_Invalid, /* All values 0x03 through through 0xff are reserved */
};
#define i5TlvLinkMetricType_Length 1

enum i5TlvLinkMetricResultCode_Values {
  i5TlvLinkMetricResultCode_InvalidNeighbor = 0,
  i5TlvLinkMetricResultCode_Invalid, /* All values 0x01 through through 0xff are reserved */
};
#define i5TlvLinkMetricResultCode_Length 1

#define i5TlvLinkMetricTxOverhead_Length 12
#define i5TlvLinkMetricTxPerLink_Length 29

#define i5TlvLinkMetricRxOverhead_Length 12
#define i5TlvLinkMetricRxPerLink_Length 23

typedef struct {
  unsigned char type;
  unsigned short length;
} __attribute__((__packed__)) i5_tlv_t;

/* Interface Structures */
typedef struct {
  unsigned char  localInterface[6];
  unsigned char  neighborInterface[6];
  unsigned short intfType;
  unsigned char  ieee8021BridgeFlag;
  unsigned int   packetErrors;
  unsigned int   transmittedPackets;
  unsigned short macThroughPutCapacity;
  unsigned short linkAvailability;
  unsigned short phyRate;
} i5_tlv_linkMetricTx_t;

typedef struct {
  unsigned char  localInterface[6];
  unsigned char  neighborInterface[6];
  unsigned short intfType;
  unsigned int   packetErrors;
  unsigned int   receivedPackets;
  unsigned char  rssi;
} i5_tlv_linkMetricRx_t;

typedef struct {
  unsigned char interfaceMac [ETH_ALEN];
  int address;
  int netmask;
  int gateway;
  int dhcpServer;  
} i5_tlv_ipv4Type_t;

typedef struct {
  unsigned char interfaceMac [ETH_ALEN];
  /* TBD - kiwin */
} i5_tlv_ipv6Type_t;

int i5TlvIsEndOfMessageType(int tlvType);
char const *i5TlvGetTlvTypeString(int tlvType);
int i5TlvEndOfMessageTypeInsert(i5_message_type *pmsg);
int i5TlvEndOfMessageTypeExtract(i5_message_type *pmsg);
int i5TlvAlMacAddressTypeInsert(i5_message_type *pmsg);
int i5TlvAlMacAddressTypeExtract(i5_message_type *pmsg, unsigned char *mac_address);
int i5TlvMacAddressTypeInsert(i5_message_type *pmsg, unsigned char *mac_address);
int i5TlvMacAddressTypeExtract(i5_message_type *pmsg, unsigned char *mac_address);
int i5TlvDeviceInformationTypeInsert(i5_message_type *pmsg, unsigned char useLegacyHpav, char* containsGenericPhy);
int i5TlvDeviceInformationTypeExtract(i5_message_type *pmsg, unsigned char *neighbor_al_mac_address, unsigned char *deviceHasGenericPhy);
int i5TlvDeviceInformationTypeExtractAlMac(i5_message_type *pmsg, unsigned char *neighbor_al_mac_address);
int i5TlvGenericPhyTypeInsert (i5_message_type *pmsg);
int i5TlvGenericPhyTypeExtract (i5_message_type *pmsg);
int i5TlvDeviceBridgingCapabilityTypeInsert(i5_message_type *pmsg);
int i5TlvDeviceBridgingCapabilityTypeExtract(i5_message_type *pmsg, unsigned char *pdevid);
int i5TlvLegacyNeighborDeviceTypeInsert(i5_message_type *pmsg);
int i5TlvLegacyNeighborDeviceTypeExtract(i5_message_type *pmsg, unsigned char *pdevid);
int i5Tlv1905NeighborDeviceTypeInsert(i5_message_type *pmsg);
int i5Tlv1905NeighborDeviceTypeExtract(i5_message_type *pmsg, unsigned char *pdevid);
int i5TlvSearchedRoleTypeInsert(i5_message_type *pmsg);
int i5TlvSearchedRoleTypeExtract(i5_message_type *pmsg, unsigned char *searchRole);
int i5TlvDoesMediaTypeMatchLocalFreqBand(unsigned short mediaType);
int i5TlvDoesFreqBandMatchLocalFreqBand(unsigned char *incomingFreqBandValue);
int i5TlvAutoconfigFreqBandTypeInsert(i5_message_type *pmsg, unsigned int freqBand);
int i5TlvAutoconfigFreqBandTypeExtract(i5_message_type *pmsg, unsigned char *autoconfigFreqBand);
int i5TlvSupportedRoleTypeInsert(i5_message_type *pmsg);
int i5TlvSupportedRoleTypeExtract(i5_message_type *pmsg, unsigned char *supportedRole);
int i5TlvSupportedFreqBandTypeInsert(i5_message_type *pmsg, unsigned int freqBand);
int i5TlvSupportedFreqBandTypeExtract(i5_message_type *pmsg, unsigned char *supportedFreqBand);
unsigned int i5TlvGetFreqBandFromMediaType(unsigned short mediaType);
int i5TlvWscTypeInsert(i5_message_type *pmsg, unsigned char const * wscPacket, unsigned wscLength);
int i5TlvWscTypeExtract(i5_message_type *pmsg, unsigned char * wscPacket, unsigned maxWscLength, unsigned *actualWscLength);
int i5TlvPushButtonEventNotificationTypeInsert(i5_message_type *pmsg, unsigned char* genericPhyIncluded);
int i5TlvPushButtonEventNotificationTypeExtract(i5_message_type * pmsg, unsigned int *pMediaCount, unsigned short **pMediaList);
int i5TlvPushButtonGenericPhyEventNotificationTypeExtract (i5_message_type * pmsg, unsigned int *pMediaCount, unsigned char **pMediaList);
int i5TlvPushButtonEventNotificationTypeExtractFree(unsigned short *pMediaList);
int i5TlvPushButtonGenericPhyEventNotificationTypeExtractFree(unsigned char *pPhyMediaList);
int i5TlvPushButtonGenericPhyEventNotificationTypeInsert(i5_message_type *pmsg);
int i5TlvLinkMetricQueryInsert (i5_message_type * pmsg, enum i5TlvLinkMetricNeighbour_Values specifyAddress, 
                                unsigned char const * mac_address, enum i5TlvLinkMetricType_Values metricTypes);
int i5TlvLinkMetricQueryExtract(i5_message_type * pmsg,
                                unsigned char * neighbours,
                                unsigned char * alMacAddress,
                                enum i5TlvLinkMetricType_Values * metricsRequested);
int i5TlvLinkMetricResultCodeInsert (i5_message_type * pmsg);
void i5TlvLinkMetricResponseExtract(i5_message_type * pmsg);
int i5TlvLinkMetricTxInsert (i5_message_type * pmsg,
                             unsigned char const * local_al_mac, unsigned char const * neighbor_al_mac,
                             i5_tlv_linkMetricTx_t const * txStats, int numLinks);
int i5TlvLinkMetricTxExtract (i5_message_type * pmsg, 
                              unsigned char * reporter_al_mac, unsigned char * neighbor_al_mac, 
                              i5_tlv_linkMetricTx_t * txStats, int maxLinks, int *numLinksReturned);
int i5TlvLinkMetricRxInsert (i5_message_type * pmsg,
                             unsigned char const * local_al_mac, unsigned char const * neighbor_al_mac,
                             i5_tlv_linkMetricRx_t const * rxStats, int numLinks);
int i5TlvLinkMetricRxExtract (i5_message_type * pmsg, 
                              unsigned char * reporter_al_mac, unsigned char * neighbor_al_mac, 
                              i5_tlv_linkMetricRx_t * rxStats, int maxLinks, int *numLinksReturned);

int i5TlvLldpTypeInsert(i5_message_type *pmsg, const unsigned char *chassis_mac, const unsigned char *portid_mac);
int i5TlvLldpTypeExtract(i5_message_type *pmsg, unsigned char *neighbor_al_mac, unsigned char *neighbor_interface_mac);
int i5TlvVendorSpecificTypeInsert(i5_message_type *pmsg, unsigned char *vendorSpec_msg, unsigned int vendorSpec_len);
int i5TlvVendorSpecificTypeExtract(i5_message_type *pmsg, unsigned char **vendorSpec_data, unsigned int * vendorSpec_len);

/* Broadcom vendor specific TLVs */
int i5Tlv_brcm_RoutingTableInsert (i5_message_type * pmsg, i5_routing_table_type *table);
int i5TlvFriendlyNameInsert(i5_message_type *pmsg, char const *friendlyName);
int i5TlvFriendlyNameExtract(i5_message_type *pmsg, char *friendlyName, int maxFriendlyNameSize);
int i5TlvFriendlyUrlInsert(i5_message_type *pmsg, unsigned char const *controlUrl);
int i5TlvFriendlyUrlExtract(i5_message_type *pmsg, unsigned char *controlUrl, int maxControlUrlSize);

#endif


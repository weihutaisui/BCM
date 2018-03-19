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

#ifndef _IEEE1905_MESSAGE_H_
#define _IEEE1905_MESSAGE_H_

/*
 * IEEE1905 Message
 */
#include <asm/byteorder.h>
#include "ieee1905_socket.h"
#include "ieee1905_linkedlist.h"
#include "ieee1905_packet.h"
#include "ieee1905_timer.h"

/* Standard Values */
#define I5_MESSAGE_STANDARD_TOPOLOGY_DISCOVERY_PERIOD_MSEC  60000

/* Not standard, BRCM values.  Do not use for interaction */
#define I5_MESSAGE_TOPOLOGY_DISCOVERY_PERIOD_MSEC     10000 
#define I5_MESSAGE_TOPOLOGY_DISCOVERY_RETRY_MSEC      50    
#define I5_MESSAGE_FRAGMENT_TIMEOUT_MSEC              20000  
#define I5_MESSAGE_TOPOLOGY_QUERY_TIMEOUT_MSEC        5000
#define I5_MESSAGE_TOPOLOGY_QUERY_RETRY_COUNT         10
#define I5_MESSAGE_RELAY_WAIT_TIMEOUT_MSEC            5000  
#define I5_MESSAGE_AP_SEARCH_NOT_READY_INTERVAL_MSEC  2000  
#define I5_MESSAGE_AP_SEARCH_START_INTERVAL_MSEC      5000  
#define I5_MESSAGE_AP_SEARCH_PERIODIC_INTERVAL_MSEC   30000 
#define I5_MESSAGE_AP_SEARCH_M1_M2_WAITING_MSEC       60000 
#define I5_MESSAGE_AP_SEARCH_START_COUNT              10    

#define I5_MESSAGE_MAX_LINKMETRICS_INTERFACES_PER_NEIGHBOR 3

#define I5_MESSAGE_DIR_RX       0x1
#define I5_MESSAGE_DIR_TX       0x2
/* this will match DIR TX as far as trace enable is concerned but will show "Relayed" instead of "Sent" in packet trace */
#define I5_MESSAGE_DIR_TX_RELAY 0x6

extern unsigned char I5_MULTICAST_MAC[];
extern unsigned char LLDP_MULTICAST_MAC[];

typedef struct {
  i5_ll_listitem ll;
  i5_packet_type packet_list;
  i5_packet_type *ppkt; // Current packet (tail when creating)
  i5_socket_type *psock;
  timer_elem_type *ptmr;
  unsigned char fragment_identifier_count;
} i5_message_type;

typedef struct {
  unsigned char message_version;
  unsigned char reserved_field;
  unsigned short message_type;
  unsigned short message_identifier;
  unsigned char fragment_identifier;
  union {
#if defined(__BIG_ENDIAN_BITFIELD)
    struct {
      unsigned char last_fragment_indicator :1;
      unsigned char relay_indicator         :1;
      unsigned char reserved_field_2        :6;
    };
#elif defined(__LITTLE_ENDIAN_BITFIELD)
    struct {
      unsigned char reserved_field_2        :6;
      unsigned char relay_indicator         :1;
      unsigned char last_fragment_indicator :1;
    };
#else
#error "BITFIELD ORDER NOT DEFINED"
#endif
    unsigned char indicators;
  };
} __attribute__((__packed__)) i5_message_header_type;

enum {
  i5MessageTopologyDiscoveryValue = 0, 
  i5MessageTopologyNotificationValue,
  i5MessageTopologyQueryValue,
  i5MessageTopologyResponseValue,
  i5MessageVendorSpecificValue,
  i5MessageLinkMetricQueryValue,
  i5MessageLinkMetricResponseValue,
  i5MessageApAutoconfigurationSearchValue,
  i5MessageApAutoconfigurationResponseValue,
  i5MessageApAutoconfigurationWscValue,
  i5MessageApAutoconfigurationRenewValue,
  i5MessagePushButtonEventNotificationValue,
  i5MessagePushButtonJoinNotificationValue,  
  i5MessageHigherLayerQueryValue,            // Unsupported
  i5MessageHigherLayerResponseValue,         // Unsupported
  i5MessagePowerChangeRequestValue,          // Unsupported
  i5MessagePowerChangeResponseValue,         // Unsupported
  i5MessageGenericPhyQueryValue,
  i5MessageGenericPhyResponseValue,
};

enum {
  i5MessageTlvExtractWithoutReset = 0,
  i5MessageTlvExtractWithReset,
};

enum {
  i5MessageFreqBand_802_11_2_4Ghz = 0,
  i5MessageFreqBand_802_11_5Ghz,    
  i5MessageFreqBand_802_11_60Ghz,
  i5MessageFreqBand_Reserved, /* All values 0x03 through through 0xff are reserved */
};

/* Routing structures */
typedef struct {
  i5_ll_listitem ll;
  unsigned char macAddress[MAC_ADDR_LEN];
} i5_routing_destination;

typedef struct {
  i5_ll_listitem ll;
  unsigned char interfaceMac [MAC_ADDR_LEN];
  unsigned char numDestinations;
  i5_routing_destination destinationList; 
} i5_routing_table_entry;

typedef struct {
  unsigned char numEntries;
  i5_routing_table_entry entryList;
} i5_routing_table_type;

void i5MessageDumpMessages(void);
void i5MessageCancel(i5_socket_type *psock);
int i5MessageRawMessageSend(unsigned char *outputInterfaceMac, unsigned char *msgData, int msgLength);
void i5MessageReset(i5_message_type *pmsg);
int i5MessageGetNextTlvType(i5_message_type *pmsg);
i5_message_type *i5MessageNew(void);
int i5MessageCheckForQueryOnDeviceAndSocket(i5_socket_type *srcSock, unsigned char *srcAddr, int queryType);
void i5MessageTopologyDiscoverySend(i5_socket_type *psock);
void i5MessageRawTopologyQuerySend (i5_socket_type *psock, unsigned char *neighbor_al_mac_address, int withRetries, int queryType);
void i5MessageGenericPhyTopologyQuerySend(i5_socket_type *psock, unsigned char *neighbor_al_mac_address);
void i5MessageTopologyQuerySend(i5_socket_type *psock, unsigned char *neighbor_al_mac_address);
void i5MessageLinkMetricQuerySend(i5_socket_type *psock, unsigned char const * destAddr, 
                                  unsigned char specifyNeighbor, unsigned char const * neighbor);
void i5MessageTopologyDiscoveryTimeout(void *arg);
unsigned char *i5MessageSrcMacAddressGet(i5_message_type *pmsg);
int i5MessageGetPacketSpace(i5_message_type *pmsg, unsigned int *currPacketSpace, unsigned int *nextPacketSpace);
int i5MessageInsertTlv(i5_message_type *pmsg, unsigned char const *buf, unsigned int len);
int i5MessageTlvExtract(i5_message_type *pmsg, unsigned int type, unsigned int *plength, unsigned char **ppvalue, char withReset);
unsigned short i5MessageVersionGet(i5_message_type *pmsg);
unsigned short i5MessageTypeGet(i5_message_type *pmsg);
void i5MessagePacketReceive(i5_socket_type *psock, i5_packet_type *ppkt);
void i5MessageTopologyQuerySend(i5_socket_type *psock, unsigned char *neighbor_al_mac_address);
void i5MessageTopologyNotificationSend(void);
void i5MessageApAutoconfigurationWscSend(i5_socket_type * psock, unsigned char * macAddr, unsigned char const * wscPacket, unsigned wscLen);
void i5MessageApAutoconfigurationSearchSend(unsigned int freqBand);
void i5MessageApAutoconfigurationResponseSend(i5_message_type * pmsg_req, unsigned int freqBand);
void i5MessageApAutoconfigurationRenewSend(unsigned int freqBand);
void i5MessagePushButtonEventNotificationSend(void);
unsigned int i5MessageSendLinkQueries(void);
void i5MessageSendRoutingTableMessage(i5_socket_type *psock, unsigned char const * destAddr, i5_routing_table_type *table);
void i5MessageDeinit( void );

#endif


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
 * IEEE1905 Message
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <linux/if_ether.h>
#include "ieee1905_timer.h"
#include "ieee1905_tlv.h"
#include "ieee1905_message.h"
#include "ieee1905_socket.h"
#include "ieee1905_datamodel.h"
#include "ieee1905_interface.h"
#include "ieee1905_json.h"
#include "ieee1905_trace.h"
#include "ieee1905_wlcfg.h"
#include "ieee1905_security.h"
#include "ieee1905_flowmanager.h"
#include "ieee1905_glue.h"

#define I5_TRACE_MODULE i5TraceMessage
#define I5_MESSAGE_MAX_TLV_SIZE (I5_PACKET_BUF_LEN - ETH_HLEN - sizeof(i5_message_header_type) )

void i5MessageTopologyDiscoveryReceive(i5_message_type *pmsg);
void i5MessageTopologyNotificationReceive(i5_message_type *pmsg);
void i5MessageTopologyQueryReceive(i5_message_type *pmsg);
void i5MessageTopologyResponseReceive(i5_message_type *pmsg);
void i5MessageVendorSpecificReceive(i5_message_type *pmsg);
void i5MessageLinkMetricQueryReceive(i5_message_type *pmsg);
void i5MessageLinkMetricResponseReceive(i5_message_type *pmsg);
void i5MessageApAutoconfigurationSearchReceive(i5_message_type *pmsg);
void i5MessageApAutoconfigurationResponseReceive(i5_message_type *pmsg);
void i5MessageApAutoconfigurationWscReceive(i5_message_type *pmsg);
void i5MessageApAutoconfigurationRenewReceive(i5_message_type *pmsg);
void i5MessagePushButtonEventNotificationReceive(i5_message_type *pmsg);
void i5MessagePushButtonJoinNotificationReceive(i5_message_type *pmsg);
void i5MessageHigherLayerQueryReceive(i5_message_type *pmsg);
void i5MessageHigherLayerResponseReceive(i5_message_type *pmsg);
void i5MessagePowerChangeRequestReceive(i5_message_type *pmsg);
void i5MessagePowerChangeResponseReceive(i5_message_type *pmsg);
void i5MessageGenericPhyTopologyResponseSend(i5_message_type *pmsg_req);
void i5MessageGenericPhyQueryReceive(i5_message_type *pmsg);
void i5MessageGenericPhyResponseReceive(i5_message_type *pmsg);

void i5MessageBridgeDiscoverySend(i5_socket_type *psock);
void i5MessageTopologyResponseSend(i5_message_type *pmsg_req);
//void i5MessageVendorSpecificSend(i5_socket_type *psock);
void i5MessageLinkMetricQuerySend(i5_socket_type *psock, unsigned char const * destAddr, 
                                  unsigned char specifyNeighbor, unsigned char const * neighbor);
//void i5MessageLinkMetricResponseSend(i5_socket_type *psock);
void i5MessageApAutoconfigurationResponseSend(i5_message_type *pmsg_req, unsigned int freqBand);
void i5MessageApAutoconfigurationWscSend(i5_socket_type *psock, unsigned char *macAddr, unsigned char const * wscPacket, unsigned wscLen);
//void i5MessageApAutoconfigurationRenewSend(i5_socket_type *psock);
void i5MessagePushButtonEventNotificationSend();

void i5MessageTopologyQueryTimeout(void *arg);
//void i5MessageLinkMetricQueryTimeout(i5_message_type *pmsg);
//void i5MessageApAutoconfigurationSearchTimeout(i5_message_type *pmsg);

void i5MessageGetVendorSpecificTlv(i5_message_type *pmsg, i5_message_type **vendorSpecMsg);
void i5MessageAddVendorSpecificTlv(i5_message_type *pmsg, i5_message_type *vendorSpecMsg);

void (*i5_message_process[])(i5_message_type *pmsg) = {
  i5MessageTopologyDiscoveryReceive, 
  i5MessageTopologyNotificationReceive,
  i5MessageTopologyQueryReceive,
  i5MessageTopologyResponseReceive,
  i5MessageVendorSpecificReceive,
  i5MessageLinkMetricQueryReceive,
  i5MessageLinkMetricResponseReceive,
  i5MessageApAutoconfigurationSearchReceive,
  i5MessageApAutoconfigurationResponseReceive,
  i5MessageApAutoconfigurationWscReceive,
  i5MessageApAutoconfigurationRenewReceive,
  i5MessagePushButtonEventNotificationReceive,
  i5MessagePushButtonJoinNotificationReceive,
  i5MessageHigherLayerQueryReceive,
  i5MessageHigherLayerResponseReceive,
  i5MessagePowerChangeRequestReceive,
  i5MessagePowerChangeResponseReceive,
  i5MessageGenericPhyQueryReceive,
  i5MessageGenericPhyResponseReceive,  
};

static char const i5MessageNames [][24] = {
   "Topology Discovery",
   "Topology Notification",
   "Topology Query",
   "Topology Response",
   "Vendor Specific",
   "Link Metric Query",
   "Link Metric Response",
   "ApAutoconfig Search",
   "ApAutoconfig Response",
   "ApAutoconfig Wsc",
   "ApAutoconfig Renew",
   "Push Button Event",
   "Push Button Join",
   "Higher Layer Query",
   "Higher Layer Response",
   "Power Change Request",
   "Power Change Response",
   "Generic PHY Query",
   "Generic Phy Response",
};

enum {
  i5MessageApSentM1_no = 0,
  i5MessageApSentM1_yes = 1,
};

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

static char const * getI5MessageName( unsigned int type) {
    if (type < ARRAY_SIZE(i5MessageNames))
        return i5MessageNames[type];
    else
        return "Message Type out of range";
}

unsigned char I5_MULTICAST_MAC[6] = {0x01, 0x80, 0xC2, 0x00, 0x00, 0x13};
unsigned char LLDP_MULTICAST_MAC[6] = {0x01, 0x80, 0xC2, 0x00, 0x00, 0x0E};

i5_message_type i5_message_list;

i5_message_type *i5MessageNew(void)
{
  i5_message_type *pmsg;

  if ((pmsg = (i5_message_type *)malloc(sizeof(i5_message_type))) == NULL) {
    printf("Malloc error\n");
    return NULL;
  }

  memset(pmsg, 0, sizeof(i5_message_type));
  i5LlItemAdd(NULL, &i5_message_list, pmsg);
  pmsg->ppkt = &pmsg->packet_list;

  return pmsg;
}

#if 0
void i5MessagePrint(i5_packet_type *ppkt) //i5_message_type *pmsg)
{
   int i;
//  i5_packet_type *ppkt = pmsg->ppkt;

   printf("Length %d\n", ppkt->length);
   for (i=0;i<ppkt->length;i++) {
      printf(" %02x", ppkt->pbuf[i]);
   }
   printf("\n");
}

typedef struct {
   int bytes;
   char name[16];
} i5Interpret_t;

static i5Interpret_t interpretations[] =
{{6,"dest mac"},
 {6,"src mac"},
 {2,"proto"},
 {1,"version"},
 {1,"reserved"},
 {2,"message type"},
 {2,"message id"},
 {1,"fragment"},   
 {1,"indicators"},
 {0,""} /* 0 means stop interpreting */
};

void i5MessageInterpret(i5_packet_type *ppkt) 
{
   int i = 0;
   int interpretIndex = 0;
   int interpretationSum = interpretations[interpretIndex].bytes - 1;

   printf("Length %d\n", ppkt->length);
   while (i<ppkt->length) {
      if (interpretations[interpretIndex].bytes) {
         /* non-zero means we're still interpreting the header */
         printf(" %02x", ppkt->pbuf[i]);
         if (i == interpretationSum) {
            printf(" %s",interpretations[interpretIndex].name);
            if (interpretIndex == 5) {
               /* interpret message type */
               int messageType = ppkt->pbuf[i] + (ppkt->pbuf[i-1] << 8);
               printf(": %s",i5MessageNames[messageType]);
            }
            printf("\n");
            interpretIndex ++;
            interpretationSum += interpretations[interpretIndex].bytes;
         }
         i++;
      } else {
         /* processing a TLV */
         i5_tlv_t *tlvHeader = (i5_tlv_t *)&ppkt->pbuf[i];
         int tlvIndex = 0;
         i+=3;
         printf("TLV Type:(%d) %s\nLength: %d\nData (hex): ",tlvHeader->type,i5TlvGetTlvTypeString(tlvHeader->type),tlvHeader->length);
         for ( ; (tlvIndex < tlvHeader->length) && (i < ppkt->length); tlvIndex++) {
            printf("%02x ",ppkt->pbuf[i]);
            i++;
         }
         printf("\n");
         if (tlvHeader->type == 0) {
            /* the rest of the packet is nonsense */
            break;
         }
      }
   }
}

#endif

static void i5MessageDumpHex(i5_packet_type *ppkt, unsigned int dir, i5_socket_type *psock)
{
  int i;
  unsigned long           addr;
  unsigned int            rem;
  unsigned char          *curPtr;
  unsigned int            ifindex = psock->u.sll.sa.sll_ifindex;
  struct ethhdr          *pEthHdr;
  const char             *pMsgName;
  i5_message_header_type *phdr;
  struct timeval          tv;
  time_t                  nowtime;
  struct tm               nowtm;

  if ( 0 == i5TracePacketGetDepth() ) {
    return;
  }

  pEthHdr = (struct ethhdr *)&ppkt->pbuf[0];
  if (pEthHdr->h_proto == htons(LLDP_PROTO)) {
    pMsgName = "Bridge Discovery";
  }
  else {
   phdr = (i5_message_header_type *)&ppkt->pbuf[sizeof(struct ethhdr)];
   if (ntohs(phdr->message_type) >= sizeof(i5MessageNames)/sizeof(&i5MessageNames[0])) {
     pMsgName = "Unknown Message Type";
   }
   else {
     pMsgName = i5MessageNames[ntohs(phdr->message_type)];
   }
  }
  
  gettimeofday(&tv, NULL);

  nowtime = tv.tv_sec;
  localtime_r(&nowtime, &nowtm);
  i5TracePacket(dir, ifindex, "%02d:%02d:%02d.%03d %s %s: if:%s-" I5_MAC_DELIM_FMT "\n",
             nowtm.tm_hour, nowtm.tm_min, nowtm.tm_sec, (unsigned int)(tv.tv_usec/1000),
             ((dir == I5_MESSAGE_DIR_RX) ? "Received" : ((dir == I5_MESSAGE_DIR_TX) ? "Sent" : "Relayed")), pMsgName, 
             psock->u.sll.ifname,  I5_MAC_PRM(psock->u.sll.mac_address));

  addr   = 0;
  curPtr = &ppkt->pbuf[0];
  while ( addr < ppkt->length )
  {
    i5TracePacket(dir, ifindex, "%08lx  ", addr);

    rem = ((ppkt->length - addr) > 16) ? 16 : (ppkt->length - addr);
    for (i = 0; i < 16; ++i) {
      if ( i < rem ) {
        i5TracePacket(dir, ifindex, "%02x ", curPtr[i]);
      }
      else {
        i5TracePacket(dir, ifindex, "   ");
      }
    }

    for (i = 0; i < rem; ++i) {
      if ( ( curPtr[i] < 32 ) || ( curPtr[i] > 126 ) ) {
        i5TracePacket (dir, ifindex, "%c", '.');
      }
      else {
        i5TracePacket(dir, ifindex, "%c", curPtr[i] );
      }
    }
    addr = addr + 16;
    curPtr = curPtr + 16;
    i5TracePacket(dir, ifindex, "\n");
  }
}

void i5MessageFree(i5_message_type *pmsg)
{
  while (pmsg->packet_list.ll.next != NULL) {
    free(((i5_packet_type *)pmsg->packet_list.ll.next)->pbuf);
    i5LlItemFree(&pmsg->packet_list, pmsg->packet_list.ll.next);
  }

  i5LlItemFree(&i5_message_list, pmsg);
}

unsigned int i5MessageSendLinkQueries(void)
{
   i5_dm_device_type *currDevice = (i5_dm_device_type *)(i5_dm_network_topology.device_list.ll.next);
   unsigned int numQueriesSent = 0;

   i5Trace("\n");
   
   while(currDevice) {
     /* loop through all devices that aren't self */
     if (!i5DmDeviceIsSelf(currDevice->DeviceId)) {
       /* send a link query message */
       /* TBD - could have multiple bridge sockets */
       i5_socket_type *bridgeSocket = i5SocketFindDevSocketByType(i5_socket_type_bridge_ll);
       if ( bridgeSocket ) {
         i5MessageLinkMetricQuerySend(bridgeSocket, currDevice->DeviceId, 0, NULL);
         numQueriesSent ++;
       }
     }
     currDevice = (i5_dm_device_type *)(currDevice->ll.next);
   }
   return numQueriesSent;
}

i5_message_type *i5MessageCreate(i5_socket_type *psock, unsigned char const *dst_addr, unsigned short proto)
{
  i5_message_type *pmsg;
  i5_packet_type *ppkt;
  struct ethhdr *peh;

  if ((pmsg = i5MessageNew()) == NULL) {
    return NULL;
  }

  if ((ppkt = i5PacketNew()) == NULL) {
    i5MessageFree(pmsg);
    return NULL;
  }

  i5LlItemAdd(pmsg, pmsg->ppkt, ppkt);
  pmsg->ppkt = ppkt;

  /* set the ethernet header */
  peh = (struct ethhdr *)ppkt->pbuf;
  memcpy((void*)(peh->h_dest),   (void*)dst_addr, ETH_ALEN);
  memcpy((void*)(peh->h_source), (void*)(i5_config.i5_mac_address), ETH_ALEN);
  peh->h_proto = htons(proto);
  ppkt->length = sizeof(struct ethhdr);
  pmsg->psock = psock;

  return pmsg;
}

void i5MessageReset(i5_message_type *pmsg)
{
  pmsg->ppkt = pmsg->packet_list.ll.next;
  pmsg->ppkt->offset = sizeof(struct ethhdr) + sizeof(i5_message_header_type);
}


void i5MessageMoveOffset(i5_message_type *pmsg, unsigned int length)
{
  i5_packet_type *ppkt = pmsg->ppkt;

  ppkt->offset += length;
  if (ppkt->offset >= ppkt->length) {
    pmsg->ppkt = (i5_packet_type *)ppkt->ll.next;
    ppkt = pmsg->ppkt;
    if (ppkt) {
      ppkt->offset = sizeof(struct ethhdr) + sizeof(i5_message_header_type);
    }
  }
  return;
}

int i5MessageGetNextTlvType(i5_message_type *pmsg)
{
  i5_packet_type *ppkt = pmsg->ppkt;

  if ((ppkt) && (ppkt->length >= ppkt->offset + sizeof(i5_tlv_t))) {
    i5_tlv_t *ptlv_header = (i5_tlv_t *)&ppkt->pbuf[ppkt->offset];
    return ptlv_header->type;
  }
  return 0;
}

/*
 * Scan through the entire message (all packets), searching for the requested "type"
 * return  0 : found
 *        -1 : found type, could not extract value
 *        -2 : not found
 */
int i5MessageTlvExtract(i5_message_type *pmsg, unsigned int type, unsigned int *plength, unsigned char **ppvalue, char withReset)
{
  i5_tlv_t *ptlv_header;
  i5TraceInfo("Looking for type %d (%s)\n", type, i5TlvGetTlvTypeString(type));
  *plength = 0;
  *ppvalue = NULL;

  if (withReset == i5MessageTlvExtractWithReset) {
    i5MessageReset(pmsg);
  }

  while (pmsg->ppkt) {
    if (pmsg->ppkt->length >= pmsg->ppkt->offset + sizeof(i5_tlv_t)) {
      ptlv_header = (i5_tlv_t *)&pmsg->ppkt->pbuf[pmsg->ppkt->offset];
      *plength = ntohs(ptlv_header->length);
      if (i5TlvIsEndOfMessageType(ptlv_header->type)) {
        return -2;
      }
      if (ptlv_header->type == type) {
        if (pmsg->ppkt->length >= (pmsg->ppkt->offset + sizeof(i5_tlv_t) + *plength)) {
          *ppvalue = &pmsg->ppkt->pbuf[pmsg->ppkt->offset + sizeof(i5_tlv_t)];
          /* The TLV was found at full length */
          i5MessageMoveOffset(pmsg, sizeof(i5_tlv_t) + *plength);     
          return 0;
        } else {
          /* The TLV *type* was found, but there weren't enough bytes in the packet */
          return -1;
        }
      }
      /* This was the wrong TLV, so move along */
      i5MessageMoveOffset(pmsg, sizeof(i5_tlv_t) + *plength);
    }
    else { 
      /* There wasn't enough room in the packet for even a TLV header, so move the offset off the end of the packet */
      i5MessageMoveOffset(pmsg, sizeof(i5_tlv_t));     
    } 
  }
  /* We searched all the way through and didn't find the TLV */
  return -2;
}

int i5MessageAddFragment(i5_message_type *pmsg)
{
  i5_packet_type *ppkt;
  i5_message_header_type *phdr;

  if ((ppkt = i5PacketNew()) == NULL) {
    return -1;
  }

  memcpy(ppkt->pbuf, pmsg->ppkt->pbuf, sizeof(struct ethhdr) + sizeof(i5_message_header_type));
  ppkt->length = sizeof(struct ethhdr) + sizeof(i5_message_header_type);

  /* change last fragment indicator of previous packet */
  phdr = (i5_message_header_type *)&pmsg->ppkt->pbuf[sizeof(struct ethhdr)];
  phdr->last_fragment_indicator = 0;

  i5LlItemAdd(pmsg, pmsg->ppkt, ppkt);
  pmsg->ppkt = ppkt;

  phdr = (i5_message_header_type *)&ppkt->pbuf[sizeof(struct ethhdr)];
  phdr->fragment_identifier = ++pmsg->fragment_identifier_count;

  return 0;  
}

int i5MessageGetPacketSpace(i5_message_type *pmsg, unsigned int *currPacketSpace, unsigned int *nextPacketSpace)
{
  if (NULL == pmsg) {
    return -1;
  }

  if (currPacketSpace) {
    *currPacketSpace = I5_PACKET_BUF_LEN - pmsg->ppkt->length;
  }
  if (nextPacketSpace) {
    *nextPacketSpace = I5_MESSAGE_MAX_TLV_SIZE; // Subtract ETH Header and start of 1905 packet
  }
  return 0;
}

int i5MessageInsertTlv(i5_message_type *pmsg, unsigned char const *buf, unsigned int len)
{
  i5_packet_type *ppkt;

  if (len > I5_PACKET_BUF_LEN) {
    return -1;
  }

  if ((pmsg->ppkt->length > ETH_ZLEN) && ((pmsg->ppkt->length + len) > I5_PACKET_BUF_LEN)) {
    if (i5MessageAddFragment(pmsg) == -1) {
      return -1;
    }
  }

  ppkt = pmsg->ppkt; 
  memcpy(&ppkt->pbuf[ppkt->length], buf, len);
  ppkt->length += len;

  return 0;  
}

void i5MessageSend(i5_message_type *pmsg, int relay)
{
  i5_packet_type *ppkt = (i5_packet_type *)pmsg->packet_list.ll.next;

  while (ppkt != NULL) {
     i5MessageDumpHex(ppkt, relay ? I5_MESSAGE_DIR_TX_RELAY : I5_MESSAGE_DIR_TX, pmsg->psock);
     i5InterfacePacketSend(pmsg->psock, ppkt);
     ppkt = (i5_packet_type *)ppkt->ll.next;
  }  
}

static inline unsigned char* i5MessageGetFirstPbuf(i5_message_type *pmsg)
{
  i5_packet_type *firstPacket =  (i5_packet_type *)(pmsg->packet_list.ll.next);
  return firstPacket->pbuf;
}

unsigned short i5MessageVersionGet(i5_message_type *pmsg) {
  unsigned char* firstPbuf = i5MessageGetFirstPbuf(pmsg);
  i5_message_header_type *phdr = (i5_message_header_type *)&firstPbuf[sizeof(struct ethhdr)];
  return (phdr->message_version);
}

unsigned short i5MessageIdentifierGet(i5_message_type *pmsg) {
  unsigned char* firstPbuf = i5MessageGetFirstPbuf(pmsg);
  i5_message_header_type *phdr = (i5_message_header_type *)&firstPbuf[sizeof(struct ethhdr)];
  return (ntohs(phdr->message_identifier));
}

unsigned short i5MessageTypeGet(i5_message_type *pmsg) {
  unsigned char* firstPbuf = i5MessageGetFirstPbuf(pmsg);
  i5_message_header_type *phdr = (i5_message_header_type *)&firstPbuf[sizeof(struct ethhdr)];
  return (ntohs(phdr->message_type));
}

unsigned char *i5MessageSrcMacAddressGet(i5_message_type *pmsg) {
  unsigned char* firstPbuf = i5MessageGetFirstPbuf(pmsg);
  struct ethhdr *peh = (struct ethhdr *)firstPbuf;
  return (peh->h_source);
}

unsigned char *i5MessageDstMacAddressGet(i5_message_type *pmsg) {
  unsigned char* firstPbuf = i5MessageGetFirstPbuf(pmsg);
  struct ethhdr *peh = (struct ethhdr *)firstPbuf;
  return (peh->h_dest);
}

unsigned short i5MessageProtoGet(i5_message_type *pmsg) {
  unsigned char* firstPbuf = i5MessageGetFirstPbuf(pmsg);
  struct ethhdr *pEthHdr = (struct ethhdr *)&firstPbuf[0];
  return (ntohs(pEthHdr->h_proto));
}

unsigned char i5MessageLastPacketFragmentIdentifierGet(i5_message_type *pmsg) {
  i5_packet_type *lastPacket = (i5_packet_type *)(pmsg->packet_list.ll.next);
  i5_message_header_type *phdr;

  if ( NULL == lastPacket ) {
   return -1;
  }
  else {
    while ( lastPacket->ll.next != NULL ) {
      lastPacket = lastPacket->ll.next;
    }
  }
  phdr = (i5_message_header_type *)&lastPacket->pbuf[sizeof(struct ethhdr)];
  return phdr->fragment_identifier;
}

void i5MessageDumpMessages(void)
{
  i5_message_type *item = (i5_message_type *)i5_message_list.ll.next;

  while (item != NULL) {
    printf("%p type=%d dest: " I5_MAC_DELIM_FMT " Timer : %s \n", 
      item->psock, i5MessageTypeGet(item), I5_MAC_PRM(i5MessageDstMacAddressGet(item)),
      (item->ptmr != NULL) ? "YES" : "NO");
    item = (i5_message_type *)item->ll.next;
  }
  return;
}

void i5MessageCancel(i5_socket_type *psock)
{
  unsigned char *neighbor_al_mac_address;
  i5_message_type *item = (i5_message_type *)i5_message_list.ll.next;
  i5_message_type *next;

  i5Trace("\n");
  while (item != NULL) {
    next = (i5_message_type *)item->ll.next;
    if (item->psock == psock) {
      if (item->ptmr) {
        i5TimerFree(item->ptmr);
      }
      if ((i5MessageTypeGet(item) == i5MessageTopologyQueryValue) || (i5MessageTypeGet(item) == i5MessageGenericPhyQueryValue)) {
        neighbor_al_mac_address = i5MessageDstMacAddressGet(item);
        i5DmDeviceQueryStateSet(neighbor_al_mac_address, i5DmStateNew);
      }
      i5MessageFree(item);
    }
    item = next;
  }
  return;
}

i5_message_type *i5MessageMatch(unsigned char *src_mac_addr, unsigned short message_identifier, unsigned short message_type)
{
  i5_message_type *item = (i5_message_type *)i5_message_list.ll.next;

  i5Trace("\n");
  while (item != NULL) {
    unsigned char *pmac = i5MessageSrcMacAddressGet(item);
    if ((i5MessageProtoGet(item) == I5_PROTO) &&
        (i5MessageIdentifierGet(item) == message_identifier) &&
        (i5MessageTypeGet(item) == message_type) &&
        (memcmp(src_mac_addr, pmac, ETH_ALEN) == 0) &&
        (item->ptmr != NULL)) {
      break;
    }
    item = (i5_message_type *)item->ll.next;
  }
  return (item);
}
   
void i5MessageRelayWaitTimeout(void *arg)
{
  i5_message_type *pmsg = (i5_message_type *)arg;
  i5TimerFree(pmsg->ptmr);
  i5MessageFree(pmsg);
}

void i5MessageRelayMulticastSend(i5_message_type *pmsg, i5_socket_type const *butNotThisSocket, unsigned char *pneighbor_al_mac_address)
{    
  i5_socket_type *psock = (i5_socket_type *)i5_config.i5_socket_list.ll.next;
  while (psock != NULL) {
    if ((psock->type == i5_socket_type_ll) && (psock != butNotThisSocket)) {
      pmsg->psock = psock;
      i5Trace("Sending Relay Multicast Message %x on %s\n", i5MessageIdentifierGet(pmsg), pmsg->psock->u.sll.ifname);
      i5MessageSend(pmsg, ((NULL == butNotThisSocket) ? 0 : 1));
    }
    psock = psock->ll.next;
  }

  /* replace src mac with AL mac so that we can match packets as required by spec
     pneighbor_al_mac_address may be NULL for local messages which means AL MAC is already used and a copy is not needed */
  if ( pneighbor_al_mac_address ) {
    struct ethhdr *peh = (struct ethhdr *)pmsg->ppkt->pbuf;
    memcpy(peh->h_source, pneighbor_al_mac_address, MAC_ADDR_LEN);
  }

  pmsg->ptmr = i5TimerNew(I5_MESSAGE_RELAY_WAIT_TIMEOUT_MSEC, i5MessageRelayWaitTimeout, pmsg);
  if ( NULL == pmsg->ptmr ) {
    i5MessageFree(pmsg);
  }

}

int i5MessageRelayMulticastCheck(i5_message_type *pmsg, unsigned char *pmac)
{
  unsigned short message_identifier = i5MessageIdentifierGet(pmsg);
  unsigned short message_type = i5MessageTypeGet(pmsg);
  i5_message_type *pmsg_match;
  int rc = 0;

  i5Trace("Received Relay Multicast Message %x on %s\n", message_identifier, pmsg->psock->u.sll.ifname);
  pmsg->ppkt->offset = sizeof(struct ethhdr) + sizeof(i5_message_header_type);
  rc |= i5TlvAlMacAddressTypeExtract(pmsg, pmac);
  if (rc == 0) {
    if ((pmsg_match = i5MessageMatch(pmac, message_identifier, message_type)) != NULL) {
      // Received an existing message, don't relay it and restart the timer since the message continues to be received
      i5TimerFree(pmsg_match->ptmr);
      pmsg_match->ptmr = i5TimerNew(I5_MESSAGE_RELAY_WAIT_TIMEOUT_MSEC, i5MessageRelayWaitTimeout, pmsg_match);
      i5Trace("Multicast Message %x discarded on %s\n", message_identifier, pmsg->psock->u.sll.ifname);      
      i5MessageFree(pmsg);
      return -1;
    }
  } 
  else {
    i5MessageFree(pmsg);
    return -1;
  }

  return 0;
}

int i5MessageRawMessageSend(unsigned char *outputInterfaceMac, unsigned char *msgData, int msgLength)
{
  i5_socket_type *pifSocket = i5SocketFindDevSocketByAddr(outputInterfaceMac, NULL);

  /* there could be multiple interfaces sharing the same address
     this message will be sent to the first one found */
  if ( pifSocket ) {
    i5_packet_type ppkt;
    ppkt.length = msgLength;
    ppkt.offset = 0;
    ppkt.pbuf = msgData;
    i5MessageDumpHex(&ppkt, I5_MESSAGE_DIR_TX, pifSocket);
    i5InterfacePacketSend(pifSocket, &ppkt);
    return 0;
  }
  return -1;
}

/* return 1 if device is already being queried on this socket
 * return 0 if not
 */
int i5MessageCheckForQueryOnDeviceAndSocket(i5_socket_type *srcSock, unsigned char *srcAddr, int queryType)
{
  i5_message_type *item = (i5_message_type *)i5_message_list.ll.next;
  i5_message_type *next;

  i5Trace("Source addr: " I5_MAC_DELIM_FMT "\n", I5_MAC_PRM(srcAddr));
  while (item != NULL) {
    next = (i5_message_type *)item->ll.next;
    unsigned char *neighbor_al_mac_address = i5MessageDstMacAddressGet(item);
    if ((item->psock == srcSock) && (i5MessageTypeGet(item) == queryType)&&
        (memcmp(neighbor_al_mac_address, srcAddr, 6) == 0) && (item->ptmr != NULL)) {
      return 1;
    }
    item = next;
  }
  return 0;
}

static void i5MessageUpdateDeviceName (unsigned char * neighbor_al_mac_address, int rcVendName, char * friendlyName)
{
  i5_dm_device_type *device = i5DmDeviceFind(neighbor_al_mac_address);
  if (device != NULL) {
    if (rcVendName == 0) {
      strncpy( device->friendlyName, friendlyName, I5_DEVICE_FRIENDLY_NAME_LEN);
      i5Trace("Received Friendly Name %s\n", friendlyName);
      i5JsonDeviceUpdate(I5_JSON_ALL_CLIENTS, device);
    } 
    else {
      i5GlueAssignFriendlyName(neighbor_al_mac_address, (char *)friendlyName, I5_DEVICE_FRIENDLY_NAME_LEN);
      strncpy( (char*)device->friendlyName, (char*)friendlyName, I5_DEVICE_FRIENDLY_NAME_LEN);
      i5Trace("No Friendly Name or IP\n");
    }
  }
}

void i5MessageTopologyDiscoveryReceive(i5_message_type *pmsg)
{
  unsigned char neighbor_al_mac_address[MAC_ADDR_LEN];
  unsigned char neighbor_interface_id[MAC_ADDR_LEN];
  int rc = 0;
  i5_dm_device_type *pDevice;
  
  i5_message_type *vendorSpecMsg = NULL;
  char friendlyName[I5_DEVICE_FRIENDLY_NAME_LEN] = "";

  i5Trace("Received Topology Discovery Message %04x on %s\n", i5MessageIdentifierGet(pmsg), pmsg->psock->u.sll.ifname);

  pmsg->ppkt->offset = sizeof(struct ethhdr) + sizeof(i5_message_header_type);
  rc |= i5TlvAlMacAddressTypeExtract(pmsg, neighbor_al_mac_address);
  rc |= i5TlvMacAddressTypeExtract(pmsg, neighbor_interface_id);
  if (rc == 0) {
    if ( i5DmDeviceIsSelf(neighbor_al_mac_address) ) {
      // Received our own topology message
      i5Trace("Loopback Topology Discovery Message %x discarded on %s\n", i5MessageIdentifierGet(pmsg), pmsg->psock->u.sll.ifname);      
      i5MessageFree(pmsg);
      return;
    }
    /* This is the only place in the code where we allow the device timer to be created */
    i5DmRefreshDeviceTimer(neighbor_al_mac_address, 1);

    i5MessageGetVendorSpecificTlv(pmsg, &vendorSpecMsg);
    if (vendorSpecMsg != NULL) {
      i5TlvFriendlyNameExtract(vendorSpecMsg, friendlyName, sizeof(friendlyName));
      i5MessageFree(vendorSpecMsg);
    }

    pDevice = i5DmDeviceFind(neighbor_al_mac_address);
    if ( pDevice == NULL ) {
      i5DmDeviceNew(neighbor_al_mac_address, i5MessageVersionGet(pmsg), friendlyName);
    }
    i5MessageTopologyQuerySend(pmsg->psock, neighbor_al_mac_address);

    /* if a neighbor entry does not exist for this device send a topology discovery */
    pDevice = i5DmDeviceFind(i5_config.i5_mac_address);
    if ( pDevice ) {
      if ( NULL == i5Dm1905NeighborFind(pDevice, pmsg->psock->u.sll.mac_address, neighbor_al_mac_address) ) {
        i5MessageTopologyDiscoveryTimeout(pmsg->psock);
      }
    }
    i5Dm1905NeighborUpdate(i5_config.i5_mac_address, pmsg->psock->u.sll.mac_address, neighbor_al_mac_address, neighbor_interface_id, 
                           NULL, i5SocketGetIfName(pmsg->psock), i5SocketGetIfIndex(pmsg->psock), 1);
  }

  i5MessageFree(pmsg);
}

void i5MessageTopologyDiscoveryTimeout(void *arg)
{
  i5_socket_type *psock = (i5_socket_type *)arg;
  if (psock->ptmr != NULL) {
    i5TimerFree(psock->ptmr);
  }
  i5MessageTopologyDiscoverySend(psock);
  i5MessageBridgeDiscoverySend(psock->u.sll.pLldpProtoSock);
  if (psock->u.sll.discoveryRetryPeriod == 0) {
    psock->u.sll.discoveryRetryPeriod = I5_MESSAGE_TOPOLOGY_DISCOVERY_RETRY_MSEC;
  }
  else if (psock->u.sll.discoveryRetryPeriod < I5_MESSAGE_TOPOLOGY_DISCOVERY_PERIOD_MSEC) {
    psock->u.sll.discoveryRetryPeriod = 4*psock->u.sll.discoveryRetryPeriod;
    if (psock->u.sll.discoveryRetryPeriod > I5_MESSAGE_TOPOLOGY_DISCOVERY_PERIOD_MSEC) {
      psock->u.sll.discoveryRetryPeriod = I5_MESSAGE_TOPOLOGY_DISCOVERY_PERIOD_MSEC;
    }
  }
  psock->ptmr = i5TimerNew(psock->u.sll.discoveryRetryPeriod, i5MessageTopologyDiscoveryTimeout, psock);
}

void i5MessageTopologyDiscoverySend(i5_socket_type *psock)
{
  i5_message_type *pmsg;
  i5_message_type *vendMsg;
  i5_dm_device_type *pdevice;

  pdevice = i5DmDeviceFind(i5_config.i5_mac_address);
  if ( pdevice == NULL ) {
    i5TraceError("Local device does not exist\n");
  }

  pmsg = i5MessageCreate(psock, I5_MULTICAST_MAC, I5_PROTO);
  if (NULL == pmsg) {
    return;
  }
  vendMsg = i5MessageCreate(psock, I5_MULTICAST_MAC, I5_PROTO);
  if (vendMsg == NULL) {
    i5MessageFree(pmsg);
    return;
  }

  i5_config.last_message_identifier++;
  i5Trace("Sending Topology Discovery Message %04x on %s\n", i5_config.last_message_identifier, psock->u.sll.ifname);

  i5PacketHeaderInit(pmsg->ppkt, i5MessageTopologyDiscoveryValue, i5_config.last_message_identifier);
  i5TlvAlMacAddressTypeInsert(pmsg);
  i5TlvMacAddressTypeInsert(pmsg, psock->u.sll.mac_address);
  i5TlvFriendlyNameInsert(vendMsg, pdevice->friendlyName);
  i5TlvEndOfMessageTypeInsert(vendMsg);
  i5MessageAddVendorSpecificTlv(pmsg, vendMsg);
  i5TlvEndOfMessageTypeInsert(pmsg);
  i5MessageSend(pmsg, 0);
  i5MessageFree(pmsg);
  i5MessageFree(vendMsg);
}

void i5MessageBridgeDiscoverySend(i5_socket_type *psock)
{
  i5_message_type *pmsg;

  i5Trace("Sending Bridge Discovery Message %s\n", psock->u.sll.ifname);
  pmsg = i5MessageCreate(psock, LLDP_MULTICAST_MAC, LLDP_PROTO); 
  if (pmsg != NULL) {
    i5TlvLldpTypeInsert(pmsg, &i5_config.i5_mac_address[0], &psock->u.sll.mac_address[0]);
    i5MessageSend(pmsg, 0);
    i5MessageFree(pmsg);
  }
}

void i5MessageBridgeDiscoveryReceive(i5_message_type *pmsg)
{
  unsigned char neighbor1905_al_mac_address[MAC_ADDR_LEN];
  unsigned char neighbor1905_interface_id[MAC_ADDR_LEN];
  unsigned char bridgeFlag = 0;
  int rc;

  i5Trace("\n");
  
  /* parse the packet */
  memset(&neighbor1905_al_mac_address[0], 0, MAC_ADDR_LEN);
  memset(&neighbor1905_interface_id[0], 0, MAC_ADDR_LEN);
  rc = i5TlvLldpTypeExtract(pmsg, &neighbor1905_al_mac_address[0], &neighbor1905_interface_id[0]);
  if ( 0 == rc ) {
    i5Dm1905NeighborUpdate(i5_config.i5_mac_address, pmsg->psock->u.sll.mac_address, neighbor1905_al_mac_address, neighbor1905_interface_id, 
                           &bridgeFlag, i5SocketGetIfName(pmsg->psock), i5SocketGetIfIndex(pmsg->psock), 0);
    i5MessageFree(pmsg);
  }
}

void i5MessageTopologyNotificationReceive(i5_message_type *pmsg)
{
  unsigned char neighbor_al_mac_address[6];
  int rc = 0;

  i5Trace("Received Topology Notification Message on %s\n", pmsg->psock->u.sll.ifname);

  rc = i5MessageRelayMulticastCheck(pmsg, neighbor_al_mac_address);
  if (rc == 0) {
    i5DmDeviceQueryStateSet(neighbor_al_mac_address, i5DmStatePending);
    i5MessageTopologyQuerySend(pmsg->psock, neighbor_al_mac_address);
    i5MessageRelayMulticastSend(pmsg, pmsg->psock /* exclude socket */, neighbor_al_mac_address);
  }
}

void i5MessageTopologyNotificationSend(void)
{
  i5_message_type *pmsg;
 
  i5Trace("Sending Topology Notification Message\n");

  pmsg = i5MessageCreate((i5_socket_type *)i5_config.i5_socket_list.ll.next, I5_MULTICAST_MAC, I5_PROTO);
  if (NULL != pmsg) {
    i5_config.last_message_identifier++;
    i5PacketHeaderInit(pmsg->ppkt, i5MessageTopologyNotificationValue, i5_config.last_message_identifier);
    i5TlvAlMacAddressTypeInsert(pmsg);
    i5TlvEndOfMessageTypeInsert(pmsg);
  
    i5MessageRelayMulticastSend(pmsg, NULL /* all sockets */, NULL);
  }
}

void i5MessageTopologyQueryReceive(i5_message_type *pmsg)
{
  i5Trace("Received Topology Query Message %04x on %s\n", i5MessageIdentifierGet(pmsg), pmsg->psock->u.sll.ifname); 
  i5MessageTopologyResponseSend(pmsg);
  i5MessageFree(pmsg);
}

void i5MessageTopologyQueryTimeout(void *arg)
{
  unsigned char neighbor_al_mac_address[MAC_ADDR_LEN];
  i5_socket_type *psock = NULL;
  i5_dm_device_type *deviceQueried = NULL; 
  
  i5_message_type *pmsg = (i5_message_type *)arg;
  int queryType = i5MessageTypeGet(pmsg);
  
  psock = pmsg->psock;
  i5Trace("%s Query Message Timeout %04x\n", (queryType==i5MessageGenericPhyQueryValue) ? "Generic PHY" : "Topology", i5MessageIdentifierGet(pmsg));
  i5TimerFree(pmsg->ptmr);

  memcpy (neighbor_al_mac_address, i5MessageDstMacAddressGet(pmsg), MAC_ADDR_LEN);
  i5MessageFree(pmsg);
  
  deviceQueried = i5DmDeviceFind(neighbor_al_mac_address);
  if (NULL == deviceQueried) {
    i5TraceError("%s Query Message Timer for nonexistent " I5_MAC_DELIM_FMT " \n",
                 (queryType==i5MessageGenericPhyQueryValue) ? "Generic PHY" : "Topology",
                 I5_MAC_PRM(neighbor_al_mac_address));
    return;
  }
  if (0 == deviceQueried->validated) {
    deviceQueried->numTopQueryFailures ++;
    if (deviceQueried->numTopQueryFailures >= I5_MESSAGE_TOPOLOGY_QUERY_RETRY_COUNT) {
      i5TraceError("%s Query Message Timers stopped for " I5_MAC_DELIM_FMT " after %d failures \n",
                   (queryType==i5MessageGenericPhyQueryValue) ? "Generic PHY" : "Topology",
                   I5_MAC_PRM(neighbor_al_mac_address), deviceQueried->numTopQueryFailures);
      i5DmDeviceFree(deviceQueried);
      return;
    }
  }
  if (queryType==i5MessageGenericPhyQueryValue) {
    i5MessageGenericPhyTopologyQuerySend(psock, neighbor_al_mac_address);    
  } 
  else {
    i5MessageTopologyQuerySend(psock, neighbor_al_mac_address);    
  }
  
}

void i5MessageRawTopologyQuerySend (i5_socket_type *psock, unsigned char *neighbor_al_mac_address, int withRetries, int queryType)
{
  i5_message_type *pmsg = i5MessageCreate(psock, neighbor_al_mac_address, I5_PROTO);
  
  if (pmsg != NULL) {
    i5_config.last_message_identifier++;
    i5Trace("Sending Topology Query Message to " I5_MAC_DELIM_FMT " %04x on %s\n", 
            I5_MAC_PRM(neighbor_al_mac_address),
            i5_config.last_message_identifier, psock->u.sll.ifname);
    i5DmDeviceQueryStateSet(neighbor_al_mac_address, i5DmStatePending);
    i5PacketHeaderInit(pmsg->ppkt, queryType, i5_config.last_message_identifier); 
    i5TlvEndOfMessageTypeInsert(pmsg);
    i5MessageSend(pmsg, 0);
    if (withRetries) {
      pmsg->ptmr = i5TimerNew(I5_MESSAGE_TOPOLOGY_QUERY_TIMEOUT_MSEC, i5MessageTopologyQueryTimeout, pmsg);
    }
    else {
      i5MessageFree(pmsg);
    }
  }
}

void i5MessageGenericPhyTopologyQuerySend(i5_socket_type *psock, unsigned char *neighbor_al_mac_address)
{
  if (i5DmDeviceQueryStateGet(neighbor_al_mac_address) == i5DmStateDone) {
    i5TraceInfo("Skipping Generic PHY Topology Query Message for device " I5_MAC_DELIM_FMT "\n", 
                I5_MAC_PRM(neighbor_al_mac_address));
    return;
  }
  if (i5MessageCheckForQueryOnDeviceAndSocket(psock, neighbor_al_mac_address, i5MessageGenericPhyQueryValue) != 0) {
    i5TraceInfo("Already Doing Generic PHY topology Query Messages for device " I5_MAC_DELIM_FMT "on socket %p \n", 
                I5_MAC_PRM(neighbor_al_mac_address), psock);
    return;
  }

  i5MessageRawTopologyQuerySend (psock, neighbor_al_mac_address, 1 /* with retries */, i5MessageGenericPhyQueryValue );
}

void i5MessageTopologyQuerySend(i5_socket_type *psock, unsigned char *neighbor_al_mac_address)
{
  if (i5DmDeviceQueryStateGet(neighbor_al_mac_address) == i5DmStateDone) {
    i5TraceInfo("Skipping Topology Query Message for device " I5_MAC_DELIM_FMT "\n", 
                I5_MAC_PRM(neighbor_al_mac_address));
    return;
  }
  if (i5MessageCheckForQueryOnDeviceAndSocket(psock, neighbor_al_mac_address, i5MessageTopologyQueryValue) != 0) {
    i5TraceInfo("Already Doing Topology Query Messages for device " I5_MAC_DELIM_FMT "on socket %p \n", 
                I5_MAC_PRM(neighbor_al_mac_address), psock);
    return;
  }

  i5MessageRawTopologyQuerySend (psock, neighbor_al_mac_address, 1 /* with retries */, i5MessageTopologyQueryValue);
}

void i5MessageTopologyResponseReceive(i5_message_type *pmsg)
{
  unsigned short message_identifier = i5MessageIdentifierGet(pmsg);
  i5_message_type *pmsg_req;
  unsigned char neighbor_al_mac_address[MAC_ADDR_LEN];
  int rc = 0;

  i5Trace("Received Topology Response Message %04x on %s\n", message_identifier, pmsg->psock->u.sll.ifname);

  if ((pmsg_req = i5MessageMatch(i5_config.i5_mac_address, message_identifier, i5MessageTopologyQueryValue)) != NULL) {
    i5_dm_device_type *pdevice;
    unsigned char genericPhysFound = 0;
#if defined(WIRELESS)
    i5_dm_interface_type *pinterface;
    char prevWlIfPresent[i5MessageFreqBand_Reserved] = { 0 };
    char newWlIfPresent[i5MessageFreqBand_Reserved] = { 0 };
    int i;
#endif
    i5_message_type *vendorSpecMsg = NULL;
    int rcVendName = -1;
    char friendlyName[I5_DEVICE_FRIENDLY_NAME_LEN] = "";

    i5TimerFree(pmsg_req->ptmr);
    i5MessageFree(pmsg_req);
    pmsg->ppkt->offset = sizeof(struct ethhdr) + sizeof(i5_message_header_type);
    rc |= i5TlvDeviceInformationTypeExtractAlMac(pmsg, neighbor_al_mac_address);
    if ( 0 == rc )
    {
      i5MessageGetVendorSpecificTlv(pmsg, &vendorSpecMsg);
      if (vendorSpecMsg != NULL) {
        rcVendName = i5TlvFriendlyNameExtract(vendorSpecMsg, friendlyName, sizeof(friendlyName));
        i5MessageFree(vendorSpecMsg);
      }
    }

    pdevice = i5DmDeviceFind(neighbor_al_mac_address);
    if ( pdevice == NULL ) {
      pdevice = i5DmDeviceNew(neighbor_al_mac_address, i5MessageVersionGet(pmsg), friendlyName);
    }

    if ( pdevice == NULL ) {
      return;
    }

#if defined(WIRELESS)
    pinterface = pdevice->interface_list.ll.next;
    while( pinterface ) {
      unsigned int index = i5TlvGetFreqBandFromMediaType(pinterface->MediaType);
      if ( index != i5MessageFreqBand_Reserved ) {
        prevWlIfPresent[index]++;
      }
      pinterface = pinterface->ll.next;
    }
#endif
    rc |= i5TlvDeviceInformationTypeExtract(pmsg, neighbor_al_mac_address, &genericPhysFound);
    rc |= i5TlvDeviceBridgingCapabilityTypeExtract(pmsg, neighbor_al_mac_address);
    rc |= i5TlvLegacyNeighborDeviceTypeExtract(pmsg, neighbor_al_mac_address);
    rc |= i5Tlv1905NeighborDeviceTypeExtract(pmsg, neighbor_al_mac_address);
    if ( 0 == rc ) {
      i5DmDeviceQueryStateSet(neighbor_al_mac_address, i5DmStateDone);
      if ( genericPhysFound != 0 ) {
        // Send the request to the node that told us about it, not the node the message is talking about
        i5TraceInfo("Detected UNKNOWN interface.  Sending Generic Phy Request\n");
        i5DmDeviceQueryStateSet(neighbor_al_mac_address, i5DmStatePending);
        i5MessageGenericPhyTopologyQuerySend(pmsg->psock, neighbor_al_mac_address);
      }
      i5DmDeviceTopologyQuerySendToAllNew(pmsg->psock);
  
      i5MessageLinkMetricQuerySend(pmsg->psock, 
                                   i5MessageSrcMacAddressGet(pmsg), 
                                   0, /* specific neighbor */
                                   i5_config.i5_mac_address);
      i5MessageUpdateDeviceName(neighbor_al_mac_address, rcVendName, friendlyName);

#if defined(WIRELESS)
      pinterface = pdevice->interface_list.ll.next;
      while( pinterface ) {
        unsigned int index = i5TlvGetFreqBandFromMediaType(pinterface->MediaType);
        if ( index != i5MessageFreqBand_Reserved ) {
          newWlIfPresent[index]++;
        }
        pinterface = pinterface->ll.next;
      }

      /* if a new wireless interface is present then restart AP auto configuration */
      for( i = 0; i < i5MessageFreqBand_Reserved; i++ ) {
        if ( newWlIfPresent[i] > prevWlIfPresent[i] ) {
          i5WlcfgApAutoconfigurationStart(NULL);
          break;
        }
      }
#endif
    }
  }
  else {
    i5Trace("Error: Received Unsolicited Topology Response Message %04x\n", message_identifier);
  }
  i5MessageFree(pmsg);
}

void i5MessageTopologyResponseSend(i5_message_type *pmsg_req)
{
  i5_message_type *pmsg;
  i5_message_type *vendMsg;
  i5_dm_device_type *pdevice;
  char containsGenericPhy = 0;
  i5_dm_device_type *destDev;
  unsigned char useLegacyHpav = 0;

  destDev = i5DmDeviceFind(i5MessageSrcMacAddressGet(pmsg_req));
  if (destDev && (destDev->nodeVersion == I5_DM_NODE_VERSION_1905) ) {
    useLegacyHpav = 1;
  } 

  pdevice = i5DmDeviceFind(i5_config.i5_mac_address);
  if ( pdevice == NULL ) {
    i5TraceError("Local device does not exist\n");
  }

  i5Trace("Sending Topology Response Message %04x on %s\n", i5MessageIdentifierGet(pmsg_req), pmsg_req->psock->u.sll.ifname);
  pmsg = i5MessageCreate(pmsg_req->psock, i5MessageSrcMacAddressGet(pmsg_req), I5_PROTO);
  if (NULL == pmsg) {
    return;
  }
  vendMsg = i5MessageCreate(pmsg_req->psock, I5_MULTICAST_MAC, I5_PROTO);
  if (vendMsg == NULL) {
    i5MessageFree(pmsg);
    return;
  }

  i5PacketHeaderInit(pmsg->ppkt, i5MessageTopologyResponseValue, i5MessageIdentifierGet(pmsg_req)); 
  i5TlvDeviceInformationTypeInsert(pmsg, useLegacyHpav, &containsGenericPhy);
  i5TlvDeviceBridgingCapabilityTypeInsert(pmsg);
  i5TlvLegacyNeighborDeviceTypeInsert(pmsg);
  i5Tlv1905NeighborDeviceTypeInsert(pmsg);
  i5TlvFriendlyNameInsert(vendMsg, pdevice->friendlyName);
  i5TlvEndOfMessageTypeInsert(vendMsg);
  i5MessageAddVendorSpecificTlv(pmsg, vendMsg);
  i5TlvEndOfMessageTypeInsert(pmsg);
  i5MessageSend(pmsg, 0);
  i5MessageFree(pmsg);
  i5MessageFree(vendMsg);

  if ( (containsGenericPhy) && destDev && (I5_DM_NODE_VERSION_UNKNOWN == destDev->nodeVersion) ) {
    i5DmWaitForGenericPhyQuery(destDev);
  }
}

void i5MessageApAutoconfigurationSearchSend(unsigned int freqBand)
{
#if defined(WIRELESS)
    i5_message_type *pmsg;

    i5Trace("Sending AP Autoconfiguration Search Message for Freq Band = %x\n", freqBand);

    pmsg = i5MessageCreate(NULL, I5_MULTICAST_MAC, I5_PROTO);
    if (pmsg != NULL) {
        i5_config.last_message_identifier++;
        i5PacketHeaderInit(pmsg->ppkt, i5MessageApAutoconfigurationSearchValue, i5_config.last_message_identifier); 
        i5TlvAlMacAddressTypeInsert (pmsg);
        i5TlvSearchedRoleTypeInsert (pmsg);
        i5TlvAutoconfigFreqBandTypeInsert (pmsg, freqBand);
        i5TlvEndOfMessageTypeInsert (pmsg);
        i5MessageRelayMulticastSend (pmsg, NULL /* all sockets */, NULL);
    }
#endif
}

void i5MessageApAutoconfigurationRenewSend(unsigned int freqBand)
{
  i5_message_type *pmsg;
   
  i5Trace("Sending AP Autoconfiguration Renew Message for Freq Band = %x\n", freqBand);
  pmsg = i5MessageCreate(NULL, I5_MULTICAST_MAC, I5_PROTO);
    if (pmsg != NULL) {
        i5_config.last_message_identifier++;
        i5PacketHeaderInit(pmsg->ppkt, i5MessageApAutoconfigurationRenewValue, i5_config.last_message_identifier); 
        i5TlvAlMacAddressTypeInsert(pmsg);
        i5TlvSupportedRoleTypeInsert(pmsg);
        i5TlvSupportedFreqBandTypeInsert (pmsg, freqBand);
        i5TlvEndOfMessageTypeInsert(pmsg);
        i5MessageRelayMulticastSend(pmsg, NULL /* all sockets */, NULL);
    }
}

void i5MessageApAutoconfigurationResponseSend(i5_message_type *pmsg_req, unsigned int freqBand)
{
    i5_message_type *pmsg;
    
    i5Trace("Sending AP Autoconfiguration Response Message %04x on %s\n", i5MessageIdentifierGet(pmsg_req), pmsg_req->psock->u.sll.ifname);
    pmsg = i5MessageCreate(pmsg_req->psock, i5MessageSrcMacAddressGet(pmsg_req), I5_PROTO);
    if (pmsg != NULL) {
        i5PacketHeaderInit(pmsg->ppkt, i5MessageApAutoconfigurationResponseValue, i5MessageIdentifierGet(pmsg_req)); 
        i5TlvSupportedRoleTypeInsert (pmsg);
        i5TlvSupportedFreqBandTypeInsert (pmsg, freqBand);
        i5TlvEndOfMessageTypeInsert (pmsg);
        i5MessageSend(pmsg, 0);
        i5MessageFree(pmsg);
    }
}

void i5MessageApAutoconfigurationWscSend(i5_socket_type *psock, unsigned char *macAddr, unsigned char const * wscPacket, unsigned wscLen)
{
    i5_message_type *pmsg;
    i5Trace("Sending AP Autoconfiguration Wsc Message on %s\n", psock->u.sll.ifname);
    pmsg = i5MessageCreate(psock, macAddr, I5_PROTO);
    if (pmsg != NULL) {
        i5_config.last_message_identifier ++;
        i5PacketHeaderInit(pmsg->ppkt, i5MessageApAutoconfigurationWscValue, i5_config.last_message_identifier); 
        i5TlvWscTypeInsert(pmsg, wscPacket, wscLen);
        i5TlvEndOfMessageTypeInsert (pmsg);
        i5MessageSend(pmsg, 0);
        i5MessageFree(pmsg);
    }
}

void inline i5MessageGetLinkMetricsWanted(enum i5TlvLinkMetricType_Values metricTypes, char *rxWanted, char *txWanted)
{
   *rxWanted = (metricTypes == i5TlvLinkMetricType_RxOnly) || (metricTypes == i5TlvLinkMetricType_TxAndRx);
   *txWanted = (metricTypes == i5TlvLinkMetricType_TxOnly) || (metricTypes == i5TlvLinkMetricType_TxAndRx);
}

void i5MessageSetTxStats (i5_tlv_linkMetricTx_t* txStats, unsigned char const * local_interface_mac_addrs, 
                          unsigned char const * neighbor_interface_mac_addrs, int numLinks)
{
  int linkIndex = 0;

  i5Trace("\n");

  for ( ; linkIndex < numLinks ; linkIndex ++) {
    unsigned char const * currNeighborMac = &neighbor_interface_mac_addrs[linkIndex * ETH_ALEN];
    i5_dm_1905_neighbor_type *thisNeighbor = i5Dm1905GetLocalNeighbor(currNeighborMac);
    memset(&txStats[linkIndex], 0, sizeof(i5_tlv_linkMetricTx_t));
    
    memcpy(&txStats[linkIndex].localInterface, &local_interface_mac_addrs[linkIndex * ETH_ALEN], ETH_ALEN);
    memcpy(&txStats[linkIndex].neighborInterface, &neighbor_interface_mac_addrs[linkIndex * ETH_ALEN], ETH_ALEN);
    
    txStats[linkIndex].macThroughPutCapacity = thisNeighbor->MacThroughputCapacity;
    txStats[linkIndex].linkAvailability = thisNeighbor->availableThroughputCapacity;

    txStats[linkIndex].ieee8021BridgeFlag = thisNeighbor->IntermediateLegacyBridge;
    
    i5TraceInfo("Adding %d/%d \n", thisNeighbor->availableThroughputCapacity,thisNeighbor->MacThroughputCapacity);
  }
}

void i5MessageSetRxStats (i5_tlv_linkMetricRx_t* rxStats, unsigned char const * local_interface_mac_addrs, 
                          unsigned char const * neighbor_interface_mac_addrs, int numLinks)
{
  int linkIndex = 0;
  
  i5Trace("\n");

  for ( ; linkIndex < numLinks ; linkIndex ++) {
    memset(&rxStats[linkIndex], 0, sizeof(i5_tlv_linkMetricTx_t));
    
    memcpy(&rxStats[linkIndex].localInterface, &local_interface_mac_addrs[linkIndex * ETH_ALEN], ETH_ALEN);
    memcpy(&rxStats[linkIndex].neighborInterface, &neighbor_interface_mac_addrs[linkIndex * ETH_ALEN], ETH_ALEN);
  }
}
        
/* This should be called if the Querier asked for one specific Mac address */
void i5MessageLinkMetricResponseSendOne(i5_message_type *pmsg_req, unsigned char *macaddr, enum i5TlvLinkMetricType_Values metricTypes)
{
   char rxWanted, txWanted;
   int numLinksFound = 0;
   unsigned char local_interface_mac[I5_MESSAGE_MAX_LINKMETRICS_INTERFACES_PER_NEIGHBOR][ETH_ALEN];
   unsigned char neighbor_interface_mac[I5_MESSAGE_MAX_LINKMETRICS_INTERFACES_PER_NEIGHBOR][ETH_ALEN];

   i5_message_type *pmsg = i5MessageCreate(pmsg_req->psock, i5MessageSrcMacAddressGet(pmsg_req), I5_PROTO);
   if (pmsg == NULL) {
      i5Trace("Unable to allocate pmsg for link metric response");
      return;
   }

   i5MessageGetLinkMetricsWanted(metricTypes, &rxWanted, &txWanted);

   numLinksFound = i5DmGetInterfacesWithNeighbor(macaddr, local_interface_mac[0], neighbor_interface_mac[0], I5_MESSAGE_MAX_LINKMETRICS_INTERFACES_PER_NEIGHBOR);
   i5TraceInfo("Found %d links to neighbor " I5_MAC_DELIM_FMT "\n", numLinksFound, I5_MAC_PRM(macaddr));
   
   if (numLinksFound > 0) {
      /* get socket for local_interface_mac */
      i5PacketHeaderInit(pmsg->ppkt, i5MessageLinkMetricResponseValue, i5MessageIdentifierGet(pmsg_req));
      if (txWanted) {
        i5_tlv_linkMetricTx_t txStats[numLinksFound];
        i5MessageSetTxStats (txStats, local_interface_mac[0], neighbor_interface_mac[0], numLinksFound);
        i5TlvLinkMetricTxInsert(pmsg, i5_config.i5_mac_address, macaddr, txStats, numLinksFound);
      }
      if (rxWanted) {
        i5_tlv_linkMetricRx_t rxStats[numLinksFound];
        i5MessageSetRxStats(rxStats, local_interface_mac[0], neighbor_interface_mac[0], numLinksFound);
        i5TlvLinkMetricRxInsert(pmsg, i5_config.i5_mac_address, macaddr, rxStats, numLinksFound);
      }
      i5TlvEndOfMessageTypeInsert (pmsg);
      /* create Link Metric TLV */
   }
   else {
      /* create link metric result code TLV (indicates failure) */
      i5PacketHeaderInit(pmsg->ppkt, i5MessageLinkMetricResponseValue, i5MessageIdentifierGet(pmsg_req));
      i5TlvLinkMetricResultCodeInsert(pmsg);
      i5TlvEndOfMessageTypeInsert (pmsg);
   } 

   /* send TLV */
   i5MessageSend(pmsg, 0);
   i5MessageFree(pmsg);      
}

/* This should be called if the Querier asked for all available link metrics */
void i5MessageLinkMetricResponseSendAll(i5_message_type *pmsg_req, enum i5TlvLinkMetricType_Values metricTypes)
{
   char rxWanted, txWanted;
   i5_dm_device_type *currDevice = (i5_dm_device_type *)(i5_dm_network_topology.device_list.ll.next);
   i5_message_type *pmsg = i5MessageCreate(pmsg_req->psock, i5MessageSrcMacAddressGet(pmsg_req), I5_PROTO);
   if (pmsg == NULL) {
      i5Trace("Unable to allocate pmsg for link metric response");
      return;
   }

   i5MessageGetLinkMetricsWanted(metricTypes, &rxWanted, &txWanted);
   i5PacketHeaderInit(pmsg->ppkt, i5MessageLinkMetricResponseValue, i5MessageIdentifierGet(pmsg_req));

   while(currDevice) {

     /* loop through all devices that aren't self */
     if (!i5DmDeviceIsSelf(currDevice->DeviceId)) {
       /* get interfaces for that device */
       unsigned char local_interface_mac[I5_MESSAGE_MAX_LINKMETRICS_INTERFACES_PER_NEIGHBOR][ETH_ALEN];
       unsigned char neighbor_interface_mac[I5_MESSAGE_MAX_LINKMETRICS_INTERFACES_PER_NEIGHBOR][ETH_ALEN];

       int numLinksFound = i5DmGetInterfacesWithNeighbor(currDevice->DeviceId, local_interface_mac[0], neighbor_interface_mac[0], I5_MESSAGE_MAX_LINKMETRICS_INTERFACES_PER_NEIGHBOR);
       i5TraceInfo("Found %d links to neighbor " I5_MAC_DELIM_FMT "\n", numLinksFound, I5_MAC_PRM(currDevice->DeviceId));
       if (txWanted) {
         i5_tlv_linkMetricTx_t txStats[numLinksFound];
         i5MessageSetTxStats (txStats, local_interface_mac[0], neighbor_interface_mac[0], numLinksFound);
         i5TlvLinkMetricTxInsert(pmsg, i5_config.i5_mac_address, currDevice->DeviceId, txStats, numLinksFound);
       }
       if (rxWanted) {
         i5_tlv_linkMetricRx_t rxStats[numLinksFound];
         i5MessageSetRxStats(rxStats, local_interface_mac[0], neighbor_interface_mac[0], numLinksFound);
         i5TlvLinkMetricRxInsert(pmsg, i5_config.i5_mac_address, currDevice->DeviceId, rxStats, numLinksFound);
       }
     }

     currDevice = (i5_dm_device_type *)(currDevice->ll.next);
     
   }
   i5TlvEndOfMessageTypeInsert (pmsg);
   
   /* send TLV */
   i5MessageSend(pmsg, 0);
   i5MessageFree(pmsg);
}

void i5MessageLinkMetricQuerySend(i5_socket_type *psock, unsigned char const * destAddr, 
                                  unsigned char specifyNeighbor, unsigned char const * neighbor)
{
   i5_message_type *pmsg;

   i5Trace("Send Link Metric Query Message\n");

   if (specifyNeighbor && !neighbor) {
      i5TraceError("Neighbor not specified\n");
      return;
   }

   pmsg = i5MessageCreate(psock, destAddr, I5_PROTO);

   if (pmsg != NULL) {
     i5_config.last_message_identifier ++;
     i5PacketHeaderInit(pmsg->ppkt, i5MessageLinkMetricQueryValue, i5_config.last_message_identifier);
     i5TlvLinkMetricQueryInsert(pmsg, 
                                specifyNeighbor ? i5TlvLinkMetricNeighbour_Specify : i5TlvLinkMetricNeighbour_All, 
                                neighbor, 
                                i5TlvLinkMetricType_TxAndRx);
     i5TlvEndOfMessageTypeInsert(pmsg);
     i5Trace("Send Link Query\n");
     i5MessageSend(pmsg, 0);
     i5MessageFree(pmsg);      
   } else {
     i5Trace("Unable to allocate pmsg for link metric query.\n");
   }
}

void i5MessageLinkMetricQueryReceive(i5_message_type *pmsg)
{
  int rc = 0;
  unsigned char neighboursQueried;
  unsigned char alMacAddressQueried[MAC_ADDR_LEN];
  enum i5TlvLinkMetricType_Values linkMetricsQueried;

  i5Trace("Received Link Metric Query Message\n");
  pmsg->ppkt->offset = sizeof(struct ethhdr) + sizeof(i5_message_header_type);
  rc |= i5TlvLinkMetricQueryExtract(pmsg, &neighboursQueried, alMacAddressQueried , &linkMetricsQueried);

  if (rc == 0) {
    if (neighboursQueried == i5TlvLinkMetricNeighbour_All) { 
      i5Trace("Asked about: All neighbours for %s\n", 
         (linkMetricsQueried == i5TlvLinkMetricType_TxOnly) ? "Tx Metrics" :
         (linkMetricsQueried == i5TlvLinkMetricType_RxOnly) ? "Rx Metrics" :
            "All Metrics");
      i5MessageLinkMetricResponseSendAll(pmsg, linkMetricsQueried);
    } else {
      i5Trace("Asked about:  %02x:%02x:%02x:%02x:%02x:%02x %s\n",
         alMacAddressQueried[0],alMacAddressQueried[1],alMacAddressQueried[2],
         alMacAddressQueried[3],alMacAddressQueried[4],alMacAddressQueried[5],
         (linkMetricsQueried == i5TlvLinkMetricType_TxOnly) ? "Tx Metrics" :
         (linkMetricsQueried == i5TlvLinkMetricType_RxOnly) ? "Rx Metrics" :
            "All Metrics");
      i5MessageLinkMetricResponseSendOne(pmsg, alMacAddressQueried, linkMetricsQueried);
    }
    
  } else {
    i5Trace("Invalid Link Metric Message\n");
  }
  i5MessageFree(pmsg);
}

void i5MessageLinkMetricResponseReceive(i5_message_type *pmsg)
{
  i5Trace("Received Link Metric Response Message\n");

  pmsg->ppkt->offset = sizeof(struct ethhdr) + sizeof(i5_message_header_type);
  i5TlvLinkMetricResponseExtract(pmsg);
  
  i5MessageFree(pmsg);
}

void i5MessageApAutoconfigurationSearchReceive(i5_message_type *pmsg)
{  
  int            rc = 0;
  unsigned char  searcher_al_mac_address[MAC_ADDR_LEN];
  unsigned short message_identifier = i5MessageIdentifierGet(pmsg);
#if defined(WIRELESS)
  unsigned char  searchedRole[i5TlvSearchRole_Length];
  unsigned char  searchedBand[i5TlvAutoConfigFreqBand_Length];
#endif
    
  i5Trace("Received AP Autoconfiguration Search Message %04x on %s\n", message_identifier, pmsg->psock->u.sll.ifname);

  rc = i5MessageRelayMulticastCheck(pmsg, searcher_al_mac_address); 
  if (0 == rc) {
#if defined(WIRELESS)
    rc |= i5TlvSearchedRoleTypeExtract (pmsg, searchedRole);
    rc |= i5TlvAutoconfigFreqBandTypeExtract (pmsg, searchedBand);
    if ( 0 == rc )
    {
      i5WlcfgApAutoConfigProcessMessage(pmsg, (unsigned int)searchedBand[0], NULL, 0);//->psock, i5MessageApAutoconfigurationSearchValue
    }
#endif
    i5MessageRelayMulticastSend(pmsg, pmsg->psock /* exclude socket */, searcher_al_mac_address); 
  }
}

void i5MessageApAutoconfigurationResponseReceive(i5_message_type *pmsg)
{
#if defined(WIRELESS)
  unsigned char supportedRole[i5TlvSearchRole_Length];
  unsigned char supportedBand[i5TlvAutoConfigFreqBand_Length];
  int rc = 0;

  i5Trace("Received AP Autoconfiguration Response %04x on %s\n", i5MessageIdentifierGet(pmsg), pmsg->psock->u.sll.ifname);

  do {
    pmsg->ppkt->offset = sizeof(struct ethhdr) + sizeof(i5_message_header_type);
    rc |= i5TlvSupportedRoleTypeExtract(pmsg, supportedRole);
    rc |= i5TlvSupportedFreqBandTypeExtract(pmsg, supportedBand);
    if (rc) {
      break;
    }

    rc = i5WlcfgApAutoConfigProcessMessage(pmsg, (unsigned int)supportedBand[0], NULL, 0);//->psock, i5MessageApAutoconfigurationResponseValue
    if ( rc < 0 ) {
      break;
    }
  } while (0);
#endif
  i5MessageFree(pmsg);
}

/* This function should be used if the contents of a the Vendor Specific TLV have been
   constructed using an i5_message_type.
   This function will locate the data and use the TLV code to add it to the real message

   If you are constructing the Vendor Specific TLV contents yourself, just use 
   i5TlvVendorSpecificTypeInsert() to add them to your i5_message_type */
void i5MessageAddVendorSpecificTlv(i5_message_type *pmsg, i5_message_type *vendorSpecMsg)
{
   unsigned char *vendorSpec_data = NULL;
    
   i5Trace("Adding Data from Vendor Specific Msg as a TLV\n");

   if ((NULL == pmsg) || (NULL == vendorSpecMsg)) {
     return;
   }
   
   vendorSpec_data = vendorSpecMsg->ppkt->pbuf + sizeof(struct ethhdr);

   i5TlvVendorSpecificTypeInsert (pmsg, vendorSpec_data, vendorSpecMsg->ppkt->length - sizeof(struct ethhdr));
}

/* If this function returns a pointer to vendorSpecMsg, the caller must free that msg */
void i5MessageGetVendorSpecificTlv(i5_message_type *pmsg, i5_message_type **vendorSpecMsg)
{
   i5Trace("\n");
   if (NULL == pmsg) {
     return;
   }

   /* create a local message to make processing the internal TLVs easier */
   *vendorSpecMsg = i5MessageCreate(pmsg->psock, i5_config.i5_mac_address, I5_PROTO);

   if (*vendorSpecMsg != NULL) {
     unsigned char * vendorSpec_data;
     unsigned int vendorSpec_len;
     i5PacketHeaderInit((*vendorSpecMsg)->ppkt, i5MessageVendorSpecificValue, 0);
     int rc = i5TlvVendorSpecificTypeExtract(pmsg, &vendorSpec_data, &vendorSpec_len);
     if ((rc == 0) && (vendorSpec_data != NULL)) {
       i5MessageInsertTlv(*vendorSpecMsg, vendorSpec_data, vendorSpec_len);
       /* At this point, vendorSpecMsg looks just like a normal message, 
          with the internal TLVs placed as if they're external and
          the offset pointed to the beginning of the IEEE1905 data */
     }   
     else {
       i5MessageFree(*vendorSpecMsg);
       *vendorSpecMsg = NULL;
     }
   }
}

void i5MessageVendorSpecificReceive(i5_message_type *pmsg)
{
  i5_message_type *vendorSpecMsg;
    
  i5MessageGetVendorSpecificTlv(pmsg, &vendorSpecMsg);

  /* vendorSpecMsg can now be treated like a normal message
     (with nonsense ethernet headers)
     ready for vendor specific TLV extraction */
  if (vendorSpecMsg != NULL) {
    i5MessageFree(vendorSpecMsg);
  }
  
  i5MessageFree(pmsg);
}

/* Used when the data model decides it is time to generate a new routing table for client 1905 nodes */
void i5MessageSendRoutingTableMessage(i5_socket_type *psock, unsigned char const * destAddr, i5_routing_table_type *table)
{
   i5_message_type *pmsg = NULL;
   i5_message_type *vendMsg = NULL;
  
   i5Trace("Send Proprietary Routing Table Message\n");

   if (!psock || !destAddr || !table) {
      i5TraceError("Null pointer(s) %p %p %p!\n", psock, destAddr, table);
      return;
   }

   pmsg = i5MessageCreate(psock, destAddr, I5_PROTO);
   if (pmsg == NULL) {
     i5Trace("Unable to allocate pmsg for Routing Table Send.\n");
     return;
   }
   
   vendMsg = i5MessageCreate(psock, destAddr, I5_PROTO);
   if (vendMsg != NULL) {
     i5_config.last_message_identifier ++;
     i5PacketHeaderInit(pmsg->ppkt, i5MessageVendorSpecificValue, i5_config.last_message_identifier);

     /* Put internal TLV into fake message */
     i5Tlv_brcm_RoutingTableInsert(vendMsg, table);
     /* Copy fake message into real TLV in real message */
     i5MessageAddVendorSpecificTlv(pmsg, vendMsg);
     i5TlvEndOfMessageTypeInsert(pmsg);

     i5MessageSend(pmsg, 0);
     i5MessageFree(vendMsg);
   } else {     
     i5Trace("Unable to allocate pmsg for Routing Table Send.\n");
   }
   i5MessageFree(pmsg);
}

void i5MessageApAutoconfigurationWscReceive(i5_message_type *pmsg)
{
#if defined(WIRELESS)
  unsigned char  mxMsg[2048];
  unsigned int   mxLength = 0;
  int            rc = 0;

  i5Trace("Received AP Autoconfiguration WSC Message\n");

  do {
    /* Extract TLV */
    pmsg->ppkt->offset = sizeof(struct ethhdr) + sizeof(i5_message_header_type);
    rc |= i5TlvWscTypeExtract(pmsg, mxMsg, sizeof(mxMsg), &mxLength);
    if (rc) {
      i5Trace("Unable to extract WSC TLV\n");
      break;
    }

    i5WlcfgApAutoConfigProcessMessage(pmsg, -1, &mxMsg[0], mxLength);
  } while (0);
#endif
  i5MessageFree(pmsg);
}

void i5MessageApAutoconfigurationRenewReceive(i5_message_type *pmsg)
{
#if defined(WIRELESS)
  unsigned char supportedRole[i5TlvSearchRole_Length];
  unsigned char supportedBand[i5TlvAutoConfigFreqBand_Length];
  int rc = 0;

  i5Trace("Received AP Autoconfiguration Renew Message on %s\n", pmsg->psock->u.sll.ifname);

  do {
    pmsg->ppkt->offset = sizeof(struct ethhdr) + sizeof(i5_message_header_type);
    rc |= i5TlvSupportedRoleTypeExtract(pmsg, supportedRole);
    rc |= i5TlvSupportedFreqBandTypeExtract(pmsg, supportedBand);
    if (rc) {
      break;
    }

    i5WlcfgApAutoconfigurationRenewProcess(pmsg, (unsigned int)supportedBand[0]);
  } while (0);  
#endif
  i5MessageFree(pmsg);
}

void i5MessagePushButtonEventNotificationReceive(i5_message_type *pmsg)
{
  int             rc;
  unsigned char   neighbor_al_mac_address[6];
  unsigned int    mediaCount;
  unsigned short *pMediaList = NULL;
  unsigned char  *pPhyMediaList = NULL;
  
  i5Trace("Received i5Message Push Button Event Notification Message\n");

  rc = i5MessageRelayMulticastCheck(pmsg, neighbor_al_mac_address);
  if ( 0 == rc ) {
    /* extract mediatype information */
    i5TlvPushButtonEventNotificationTypeExtract(pmsg, &mediaCount, &pMediaList);
    if (mediaCount) {
      i5SecurityProcessExternalPushButtonEvent(mediaCount, pMediaList);
    }
    i5TlvPushButtonEventNotificationTypeExtractFree(pMediaList);
    i5TlvPushButtonGenericPhyEventNotificationTypeExtract(pmsg, &mediaCount, &pPhyMediaList);
    if (mediaCount) {
      i5SecurityProcessGenericPhyExternalPushButtonEvent(mediaCount, pPhyMediaList);
    }
    i5TlvPushButtonGenericPhyEventNotificationTypeExtractFree(pPhyMediaList);
    i5MessageRelayMulticastSend(pmsg, pmsg->psock /* exclude socket */, neighbor_al_mac_address);
  }
}

void i5MessagePushButtonEventNotificationSend( )
{
  i5_message_type *pmsg;
    
  pmsg = i5MessageCreate(NULL, I5_MULTICAST_MAC, I5_PROTO);
  if (pmsg != NULL) {
    unsigned char genericPhyIncluded = 0;
    i5_config.last_message_identifier++;
    i5Trace("Sending i5Message Push Button Event Notification Message %04x\n", i5_config.last_message_identifier);
    i5PacketHeaderInit(pmsg->ppkt, i5MessagePushButtonEventNotificationValue, i5_config.last_message_identifier); 
    i5TlvAlMacAddressTypeInsert(pmsg);
    i5TlvPushButtonEventNotificationTypeInsert(pmsg, &genericPhyIncluded);
    if (genericPhyIncluded) {
      i5TlvPushButtonGenericPhyEventNotificationTypeInsert(pmsg);
    }
    i5TlvEndOfMessageTypeInsert (pmsg);
    i5MessageRelayMulticastSend (pmsg, NULL /* all sockets */, NULL);
  }
}

void i5MessagePushButtonJoinNotificationReceive(i5_message_type *pmsg)
{
  (void) pmsg;
  i5Trace("Message not handled\n");
}

void i5MessageHigherLayerQueryReceive(i5_message_type *pmsg)
{
  (void) pmsg;
  i5Trace("Message not handled\n");
}

void i5MessageHigherLayerResponseReceive(i5_message_type *pmsg)
{
  (void) pmsg;
  i5Trace("Message not handled\n");
}

void i5MessagePowerChangeRequestReceive(i5_message_type *pmsg)
{
  (void) pmsg;
  i5Trace("Message not handled\n");
}

void i5MessagePowerChangeResponseReceive(i5_message_type *pmsg)
{
  (void) pmsg;
  i5Trace("Message not handled\n");
}

void i5MessageGenericPhyTopologyResponseSend(i5_message_type *pmsg_req)
{
   i5Trace("Send Generic Phy Topology Response\n");

   i5_message_type *pmsg = i5MessageCreate(pmsg_req->psock, i5MessageSrcMacAddressGet(pmsg_req), I5_PROTO);
   if (pmsg == NULL) {
      i5Trace("Unable to allocate pmsg for link metric response");
      return;
   }

   i5PacketHeaderInit(pmsg->ppkt, i5MessageGenericPhyResponseValue, i5MessageIdentifierGet(pmsg_req));
   i5TlvGenericPhyTypeInsert (pmsg);
   i5TlvEndOfMessageTypeInsert (pmsg);
   
   /* send TLV */
   i5MessageSend(pmsg, 0);
   i5MessageFree(pmsg);  
}

void i5MessageGenericPhyQueryReceive(i5_message_type *pmsg)
{
  i5_dm_device_type *destDev = i5DmDeviceFind(i5MessageSrcMacAddressGet(pmsg));
  i5Trace("Rx'd Generic PHY query\n");

  if (destDev) {
    destDev->nodeVersion = I5_DM_NODE_VERSION_19051A;
    if (destDev->nodeVersionTimer) {
      i5TimerFree(destDev->nodeVersionTimer);
    }
  }
  i5MessageGenericPhyTopologyResponseSend(pmsg);
  i5MessageFree(pmsg);
}

void i5MessageGenericPhyResponseReceive(i5_message_type *pmsg)
{ 
  i5_dm_device_type *destDev = i5DmDeviceFind(i5MessageSrcMacAddressGet(pmsg));

  i5Trace("Rx'd Generic PHY Response\n");
  if (destDev) {
    destDev->nodeVersion = I5_DM_NODE_VERSION_19051A;
    if (destDev->nodeVersionTimer) {
      i5TimerFree(destDev->nodeVersionTimer);
    }
  }
  i5TlvGenericPhyTypeExtract(pmsg);
  i5MessageFree(pmsg);
}

void i5MessageFragmentTimeout(void *arg)
{
  i5_message_type *pmsg = (i5_message_type *)arg;
  printf("Error: Timeout receiving all fragments\n");
  i5TimerFree(pmsg->ptmr);
  i5MessageFree(pmsg);
}

void i5MessagePacketReceive(i5_socket_type *psock, i5_packet_type *ppkt)
{
  struct ethhdr *peh;
  i5_message_header_type *phdr;
  unsigned short message_type, message_identifier;
  i5_message_type *pmsg;
  unsigned char knownPacket = 1;
  
  i5TraceInfo("\n");

  if (ppkt->length < sizeof(i5_message_header_type)) {
    printf("Received Invalid Packet Length %d\n", ppkt->length);
    i5PacketFree(ppkt);
    return;
  }

  peh = (struct ethhdr *)ppkt->pbuf;
  if ( i5DmDeviceIsSelf(peh->h_source) ) {
    i5TraceInfo("Received packet using local MAC as source: proto 0x%04x, if %s " I5_MAC_DELIM_FMT "\n"
                 , ntohs(peh->h_proto), psock->u.sll.ifname, I5_MAC_PRM(peh->h_source) );
    i5PacketFree(ppkt);
    return;
  }

  if (ntohs(peh->h_proto) == LLDP_PROTO) {
    if ((pmsg = i5MessageNew()) == NULL) {
      i5PacketFree(ppkt);
      return;
    }
    i5LlItemAdd(pmsg, pmsg->ppkt, ppkt);
    pmsg->psock = psock;
    pmsg->ppkt = ppkt;
    i5MessageDumpHex(ppkt, I5_MESSAGE_DIR_RX, psock);
    i5MessageBridgeDiscoveryReceive(pmsg);
  }
  else {
    if (ntohs(peh->h_proto) != I5_PROTO) {
      i5TraceInfo("Received Invalid Protocol 0x%04x, if %s\n", ntohs(peh->h_proto), psock->u.sll.ifname);
      i5PacketFree(ppkt);
      return;
    }

    phdr = (i5_message_header_type *)&ppkt->pbuf[sizeof(struct ethhdr)];
    message_type = ntohs(phdr->message_type);
    message_identifier = ntohs(phdr->message_identifier);
    
    if (phdr->message_version != I5_MESSAGE_VERSION) {
      i5TraceError("Received Invalid Version 0x%02x\n", phdr->message_version);
      knownPacket = 0;
    } 
    else if (message_type >= sizeof(i5_message_process)/sizeof(i5_message_process[0])) {
      i5TraceError("Received Invalid Message Type 0x%04x\n", message_type);
      knownPacket = 0;
    } 

    if (!knownPacket) {
      if (phdr->relay_indicator) {
        i5TraceInfo("Possibly relaying unknown message\n");
        pmsg = i5MessageNew();
        if (pmsg == NULL) {
          i5PacketFree(ppkt);
          return;
        }
        i5LlItemAdd(pmsg, pmsg->ppkt, ppkt);
        pmsg->psock = psock;
        pmsg->ppkt = ppkt;
        if (i5MessageMatch(peh->h_source, message_identifier, message_type) == NULL) {
          i5TraceInfo("RELAYING!\n");
          i5MessageRelayMulticastSend(pmsg, pmsg->psock, peh->h_source);
        }
        else {
          i5TraceInfo("NOT RELAYING ==-\n");
          i5PacketFree(ppkt);
        }
      }
      else {
        i5TraceInfo("dropping unknown message\n");
        i5PacketFree(ppkt);
      }
      i5MessageDumpHex(ppkt, I5_MESSAGE_DIR_RX, psock);
      return;
    }
    
    i5TraceInfo("Received I5PROTO - %s on %s (frag%d/%02x)\n", getI5MessageName(message_type), psock->u.sll.ifname, phdr->fragment_identifier, phdr->indicators);
    
    if (phdr->fragment_identifier == 0) {
      pmsg = i5MessageNew();
      if (pmsg == NULL) {
        i5PacketFree(ppkt);
        return;
      }
      i5LlItemAdd(pmsg, pmsg->ppkt, ppkt);
      pmsg->psock = psock;
      pmsg->ppkt = ppkt;
      i5MessageDumpHex(ppkt, I5_MESSAGE_DIR_RX, psock);
    }
    else {
      if ((pmsg = i5MessageMatch(peh->h_source, message_identifier, message_type)) == NULL) {
        printf("Error: Received a fragment with no previous message identifier 0x%04x\n", message_identifier);
        i5PacketFree(ppkt);
        return;
      }
  
      // Cancel the timer
      i5TimerFree(pmsg->ptmr);

      if (i5MessageLastPacketFragmentIdentifierGet(pmsg) != (phdr->fragment_identifier - 1)) {
        i5MessageDumpHex(ppkt, I5_MESSAGE_DIR_RX, psock);
        printf("Error: Received an out-of-order fragment id %d\n", phdr->fragment_identifier);
        i5PacketFree(ppkt);
        i5MessageFree(pmsg);
        return;
      }
      i5LlItemAdd(pmsg, pmsg->ppkt, ppkt);
      pmsg->ppkt = ppkt;
      i5MessageDumpHex(ppkt, I5_MESSAGE_DIR_RX, psock);
    }

    if (phdr->last_fragment_indicator == 1) {
      i5MessageReset(pmsg);
      (i5_message_process[message_type])(pmsg);
    } else {
      // Allow a certain time to receive all fragments
      pmsg->ptmr = i5TimerNew(I5_MESSAGE_FRAGMENT_TIMEOUT_MSEC, i5MessageFragmentTimeout, pmsg);
    }
  }

  return;
}

void i5MessageDeinit( void )
{
   i5_message_type *pmsg;
   while (i5_message_list.ll.next != NULL ) {
     pmsg = i5_message_list.ll.next;
     if ( pmsg->ptmr ) {
       i5TimerFree(pmsg->ptmr);
     }
     i5MessageFree(pmsg);
   }
}


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

#ifndef _IEEE1905_SOCKET_H_
#define _IEEE1905_SOCKET_H_

/*
 * IEEE1905 Socket
 */

#include <stdio.h>
#include <sys/socket.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <netinet/in.h>
#include <sys/un.h>
#include "i5api.h"
#include "ieee1905_linkedlist.h"
#include "ieee1905_timer.h"

#if (FD_SETSIZE < 64)
#define MAX_I5_SOCKETS FD_SETSIZE
#else
#define MAX_I5_SOCKETS 64
#endif

#define MAX_PATH_NAME 64
#define MAC_ADDR_LEN 6
#define MAX_FREQ_BANDS 3

enum {
  i5_socket_type_ll = 0,
  i5_socket_type_bridge_ll,
  i5_socket_type_in_listen,
  i5_socket_type_json,
  i5_socket_type_netlink,
  i5_socket_type_udp,
  i5_socket_type_stream,
  i5_socket_type_raweth
};

typedef struct _i5_socket_type {
  i5_ll_listitem ll;
  int sd;
  void (*process)(struct _i5_socket_type *psock);
  int type;
  timer_elem_type *ptmr;
  union {
    struct {
      struct sockaddr_ll      sa;
      unsigned char           mac_address[ETH_ALEN];
      char                    ifname[I5_MAX_IFNAME];
      unsigned int            options;
      void (*notify)(struct _i5_socket_type *psock, unsigned char oper_status);
      timer_elem_type        *pnltmr;
      unsigned int            discoveryRetryPeriod;
      struct _i5_socket_type *pMetricSock;
      struct _i5_socket_type *pLldpProtoSock;
      unsigned short          hystBwMbps;
      unsigned char           hystBandwidthDataValid;
      void                   *pInterfaceCtx;
    } sll;
    struct {
      struct sockaddr_in sa;
    } sinl;
    struct {
      FILE *fp;
      int isGet;
    } sinr;
    struct {
      struct sockaddr_nl sa;
    } snl;
    struct {
      struct sockaddr_un sa;
    } sun;
  } u;
} i5_socket_type;

typedef struct {
  i5_ll_listitem  ll;
  unsigned int    freqBand;
  char            ifname[I5_MAX_IFNAME];
  int             renew;
  int             expectedMsg;
  unsigned int    callCounter;
  unsigned int    expiryTime;
  char            rxIfname[I5_MAX_IFNAME];
  unsigned char   registrarMac[MAC_ADDR_LEN];
} apSearchEntry;

typedef struct  {
  timer_elem_type  *timer;
  apSearchEntry    *activeSearchEntry;
  apSearchEntry     searchEntryList;
} apSearchSendStruct;

typedef struct {
  timer_elem_type               *timer;
  unsigned int                   count;
} secStatusStruct;

typedef struct  {
  timer_elem_type    *ptmr;
  i5_socket_type     *psock;
  union {
    struct {
      int           macAddrSet;
      unsigned char plcDevMac[MAC_ADDR_LEN];
    };
  };
} controlSockStruct;

typedef struct {
  unsigned char         i5_mac_address[ETH_ALEN];
  char                  friendlyName[I5_DEVICE_FRIENDLY_NAME_LEN];
  unsigned char         socketRemovedFlag;
  i5_socket_type        i5_socket_list;
  int                   num_i5_sockets;
  unsigned short        last_message_identifier;
  controlSockStruct     plc_control_socket;
  controlSockStruct     wl_control_socket;
  apSearchSendStruct    apSearch;
  int                   isRegistrar;
  char                  freqBandEnable[MAX_FREQ_BANDS];
  char                  jsonLegacyDisplayEnabled;
  secStatusStruct       ptmrSecStatus;
  unsigned int          deviceWatchdogTimeoutMsec;
  int                   running;
  int                   networkTopEnabled;
} i5_config_type;

extern i5_config_type i5_config;

i5_socket_type *i5SocketStreamNew(unsigned short port, void(* process)(i5_socket_type *));
void i5SocketDumpSockets(void);
i5_socket_type *i5SocketNew(int sd, int socket_type, void (*process)(i5_socket_type *));
void i5SocketNetlink(void);
int i5SocketClose(i5_socket_type *psock);
void i5SocketMain(void);
void i5SocketInit(void);
void i5SocketDeinit(void);
i5_socket_type *i5SocketFindDevSocketByType( unsigned int socketType );
i5_socket_type *i5SocketFindDevSocketByIndex( unsigned int ifindex );
i5_socket_type *i5SocketFindDevSocketByName( const char *ifname );
i5_socket_type *i5SocketFindDevSocketByAddr( unsigned char *macAddr, i5_socket_type *pifstart );
i5_socket_type *i5SocketFindUdpSocketByProcess( void *process );

char *i5SocketGetIfName(i5_socket_type *psock);
int   i5SocketGetIfIndex(i5_socket_type *psock);
unsigned char *i5SocketGetAddress(i5_socket_type *psock);

#endif


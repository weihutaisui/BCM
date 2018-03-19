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
 * IEEE1905 Sockets
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if_arp.h>
#include <linux/if_ether.h>
#include <sys/select.h>
#include <errno.h>
#include <poll.h>
#include "ieee1905_timer.h"
#include "ieee1905_socket.h"
#include "ieee1905_control.h"
#include "ieee1905_trace.h"
#include "ieee1905_ethstat.h"
#include "ieee1905_datamodel.h"
#include "ieee1905_message.h"
#if defined(BRCM_CMS_BUILD)
#include "ieee1905_cmsutil.h"
#endif

#define I5_TRACE_MODULE                     i5TraceSocket

#ifndef FD_COPY
#define FD_COPY(src,dest) memcpy((dest),(src),sizeof(dest))
#endif

i5_config_type i5_config;

i5_socket_type *i5SocketFindDevSocketByType( unsigned int socketType )
{
  i5_socket_type *pif;
  pif = (i5_socket_type *)i5_config.i5_socket_list.ll.next;
  while (pif != NULL)
  {
    if ((pif->type == socketType) )
    {
      return pif;
    }
    pif=pif->ll.next;
  }
  return NULL;
}

i5_socket_type *i5SocketFindDevSocketByAddr( unsigned char *macAddr, i5_socket_type *pifstart)
{
  i5_socket_type *pif;
  if ( NULL == pifstart ) {
    pif = (i5_socket_type *)i5_config.i5_socket_list.ll.next;
  }
  else {
    pif = pifstart->ll.next;
  }
  while (pif != NULL)
  {
    if ((pif->type == i5_socket_type_ll) && (0 == memcmp(pif->u.sll.mac_address, macAddr, ETH_ALEN)) )
    {
      return pif;
    }
    pif=pif->ll.next;
  }
  return NULL;
}

i5_socket_type *i5SocketFindDevSocketByIndex( unsigned int ifindex )
{
  i5_socket_type *pif;
  pif = (i5_socket_type *)i5_config.i5_socket_list.ll.next;
  while (pif != NULL)
  {
    if (((pif->type == i5_socket_type_ll) || (pif->type == i5_socket_type_bridge_ll)) && (pif->u.sll.sa.sll_ifindex == ifindex) )
    {
      return pif;
    }
    pif=pif->ll.next;
  }
  return NULL;
}

i5_socket_type *i5SocketFindDevSocketByName( const char *ifname )
{
  i5_socket_type *pif;
  pif = (i5_socket_type *)i5_config.i5_socket_list.ll.next;
  while (pif != NULL)
  {
    if (((pif->type == i5_socket_type_ll) || (pif->type == i5_socket_type_bridge_ll)) &&
        (0 == strcmp(pif->u.sll.ifname, ifname)) )
    {
      return pif;
    }
    pif=pif->ll.next;
  }
  return NULL;
}

i5_socket_type *i5SocketFindUdpSocketByProcess( void *process )
{
  i5_socket_type *psock;
  psock = (i5_socket_type *)i5_config.i5_socket_list.ll.next;
  while (psock != NULL)
  {
    if ((psock->type == i5_socket_type_udp) && ((void *)psock->process == process))
    {
      return psock;
    }
    psock=psock->ll.next;
  }
  return NULL;
}

i5_socket_type *i5SocketStreamNew(unsigned short port, void(* process)(i5_socket_type *))
{
  int sd;
  i5_socket_type *psock;
  struct sockaddr_in sa = { 0 };

  i5Trace("\n");

  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    i5TraceError("Could not create socket - %s\n", strerror(errno));
    return NULL;
  }

  psock = i5SocketNew(sd, i5_socket_type_stream, process);
  if ( NULL == psock ) {
    i5TraceError("i5SockNew failed\n");
    close(sd);
    return NULL;
  }

  psock->u.sinl.sa.sin_family      = AF_INET;
  psock->u.sinl.sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  if (bind(psock->sd, (struct sockaddr *)&(psock->u.sinl.sa), sizeof(struct sockaddr_in)) < 0) {
    i5TraceError("Failed to bind socket -  %s\n", strerror(errno));
    i5SocketClose(psock);
    return NULL;
  }

  sa.sin_family      = AF_INET;
  sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  sa.sin_port        = htons(port);
  if (connect(psock->sd, (struct sockaddr *)&sa, sizeof(struct sockaddr_in)) == -1) {
    i5TraceError("connect to %d failed - %s\n", port, strerror(errno));
    i5SocketClose(psock);
    return NULL;
  }

  return psock;
}

void i5SocketDumpSockets(void)
{
    int i = 0;
    i5_socket_type *p;
    p = (i5_socket_type *)i5_config.i5_socket_list.ll.next;
    while (p != NULL) {
       printf("  [%2d:%p]: %2d %p() %04x", i, p, p->sd, p->process, p->type);
       if ( (i5_socket_type_ll == p->type) ||
            (i5_socket_type_bridge_ll == p->type) ||
            (i5_socket_type_raweth == p->type) ) {
         printf(" if (%s/%d), proto %x, MAC " I5_MAC_DELIM_FMT "\n", 
              p->u.sll.ifname, p->u.sll.sa.sll_ifindex, htons(p->u.sll.sa.sll_protocol), I5_MAC_PRM(&p->u.sll.mac_address[0]));
       }
       else {
         printf("\n");
       }
       i++;
       p = p->ll.next;
    }
}

i5_socket_type *i5SocketNew(int sd, int socket_type, void (*process)(i5_socket_type *))
{
  i5_socket_type *psock;
  
  i5Trace("\n");
  if (i5_config.num_i5_sockets > MAX_I5_SOCKETS) {
    printf("i5SocketNew: Error - too many sockets\n");
    return (NULL);
  }
  if ((psock = (i5_socket_type *)malloc(sizeof(i5_socket_type))) == NULL) {
    printf("i5SocketNew: malloc error\n");
    return (NULL);
  }
  memset(psock, 0, sizeof(i5_socket_type));
  psock->sd = sd;
  psock->process = process;
  psock->type = socket_type;

  i5LlItemAdd(&i5_config, &i5_config.i5_socket_list, psock);
  ++i5_config.num_i5_sockets;
  i5Trace("num_i5_sockets: %d (mapping %d -> %p(), type %d)\n", i5_config.num_i5_sockets, sd, process, socket_type);
  
  return(psock);
}

int i5SocketClose(i5_socket_type *psock)
{
  i5_config.socketRemovedFlag = 1;
  i5Trace("%d -> %p(), type %d\n", psock->sd, psock->process, psock->type);

  if (psock->ptmr != NULL) {
    i5TimerFree(psock->ptmr);
  }
  if ( psock->type == i5_socket_type_ll) {
    if ( psock->u.sll.pnltmr != NULL ) {
      i5TimerFree(psock->u.sll.pnltmr);
    }
    if ( psock->u.sll.pMetricSock ) {
      i5SocketClose(psock->u.sll.pMetricSock);
    }
    if ( psock->u.sll.pLldpProtoSock ) {
      i5SocketClose(psock->u.sll.pLldpProtoSock);
    }
    if ( psock->u.sll.pInterfaceCtx != NULL ) {
      i5EthStatFreeCtx(psock->u.sll.pInterfaceCtx);
    }
  }

  close(psock->sd);

  if (i5LlItemFree(&i5_config.i5_socket_list, psock) == 0) {
    --i5_config.num_i5_sockets;
    i5Trace("num_i5_sockets: %d\n", i5_config.num_i5_sockets);
    return 0;
  }
  return -1;
}

void i5SocketMain(void)
{
  struct timeval  tv, *ptv;
  int             rc = 0;
  int             pollTimeout;
  struct pollfd   pollfd[MAX_I5_SOCKETS];
  int             i;
  i5_socket_type *psock;

  i5Trace("\n");
  while (i5_config.running) {
    ptv = i5TimerExpires(&tv);
    if (ptv != NULL) {
      pollTimeout = (ptv->tv_sec * 1000) + (ptv->tv_usec / 1000);
      if ( 0 == pollTimeout ) {
        pollTimeout = 1;
      }
    }
    else {
      pollTimeout = -1;
    }

    i = 0;
    psock = (i5_socket_type *)i5_config.i5_socket_list.ll.next;
    while (psock != NULL) {
      pollfd[i].fd      = psock->sd;
      pollfd[i].events  = POLLIN;
      pollfd[i].revents = 0;
      psock = psock->ll.next;
      i++;
    }

    i5_config.socketRemovedFlag = 0;
    i5TraceInfo("pollTimeout=%d\n", pollTimeout);
    rc = poll(&pollfd[0], i5_config.num_i5_sockets, pollTimeout);
    i5TraceInfo("--- Finished poll: rc=%d ---\n", rc);
    
    if (rc == -1) {
      printf("poll returned an error %d, %s\n", errno, strerror(errno));
    }
    else if ( rc )
    {
      for ( i = 0; i < i5_config.num_i5_sockets; i++ )
      {
        if ( 0 == pollfd[i].revents ) {
          continue;
        }

        /* need to search for matching socket as previously processed
           events may have modified the socket list */
        psock = (i5_socket_type *)i5_config.i5_socket_list.ll.next;
        while (psock != NULL) {
          if (psock->sd == pollfd[i].fd) {
            break;
          }
          psock = psock->ll.next;
        }

        /* socket could have been removed by a previous event */
        if ( NULL == psock ) {
          i5Trace("Socket not found %d\n", pollfd[i].fd);
          continue;
        }

        if ( pollfd[i].revents & ~(POLLIN|POLLHUP|POLLPRI)) {
          if ( (psock->type == i5_socket_type_ll) || (psock->type == i5_socket_type_bridge_ll) ) {
            i5TraceError("Unexpected socket event: 0x%02x, socket: type %d, process %p, if %s\n", pollfd[i].revents, psock->type, psock->process, psock->u.sll.ifname);
          } 
          else { 
            i5TraceError("Unexpected socket event: 0x%02x, socket: type %d, process %p\n", pollfd[i].revents, psock->type, psock->process);
          }
        }
        psock->process(psock);
        if ( 1 == i5_config.socketRemovedFlag ) {
          i5Trace("Break out and repoll\n");
          break;
        }
      }
    }
    if (i5DmDeviceTopologyChangeProcess(&i5_config.i5_mac_address[0])) {
      i5MessageTopologyNotificationSend();
    }
  }
}

char *i5SocketGetIfName(i5_socket_type *psock) {
    switch(psock->type) {
        case i5_socket_type_ll:
            return psock->u.sll.ifname;
            break;            
        default: 
            return NULL;
    }
}

int i5SocketGetIfIndex(i5_socket_type *psock) {
    switch(psock->type) {
        case i5_socket_type_ll:
            return psock->u.sll.sa.sll_ifindex;
            break;            
        default: 
            return 0;
    }
}

unsigned char *i5SocketGetAddress(i5_socket_type *psock) {
    switch(psock->type) {
        case i5_socket_type_ll:
            return psock->u.sll.mac_address;
            break;
        default: 
            return NULL;
    }
}

void i5SocketDeinit( void )
{
  if (i5_config.plc_control_socket.ptmr) {
    i5TimerFree(i5_config.plc_control_socket.ptmr);
  }
  if (i5_config.plc_control_socket.psock) {
    i5SocketClose(i5_config.plc_control_socket.psock);
  }
  if (i5_config.wl_control_socket.ptmr) {
    i5TimerFree(i5_config.wl_control_socket.ptmr);
  }
  if (i5_config.wl_control_socket.psock) {
    i5SocketClose(i5_config.wl_control_socket.psock);
  }
  if (i5_config.apSearch.timer) {
    i5TimerFree(i5_config.apSearch.timer);
  }
  while ( i5_config.apSearch.searchEntryList.ll.next ) {
    i5LlItemFree(&i5_config.apSearch.searchEntryList, i5_config.apSearch.searchEntryList.ll.next);
  }
  if (i5_config.ptmrSecStatus.timer) {
    i5TimerFree(i5_config.ptmrSecStatus.timer);
  }
  while ( i5_config.i5_socket_list.ll.next ) {
    i5SocketClose(i5_config.i5_socket_list.ll.next);
  }
}

void i5SocketInit()
{
  i5Trace("\n");

  if ((i5ControlSocketCreate()) == NULL) {
    printf("failed to create the control socket\n");
    exit(-1);
  }
}

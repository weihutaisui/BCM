/***************************************************************************
** <:copyright-BRCM:2017:proprietary:standard
** 
**    Copyright (c) 2017 Broadcom 
**    All Rights Reserved
** 
**  This program is the proprietary software of Broadcom and/or its
**  licensors, and may only be used, duplicated, modified or distributed pursuant
**  to the terms and conditions of a separate, written license agreement executed
**  between you and Broadcom (an "Authorized License").  Except as set forth in
**  an Authorized License, Broadcom grants no license (express or implied), right
**  to use, or waiver of any kind with respect to the Software, and Broadcom
**  expressly reserves all rights in and to the Software and all intellectual
**  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
**  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
**  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
** 
**  Except as expressly set forth in the Authorized License,
** 
**  1. This program, including its structure, sequence and organization,
**     constitutes the valuable trade secrets of Broadcom, and you shall use
**     all reasonable efforts to protect the confidentiality thereof, and to
**     use this information only in connection with your use of Broadcom
**     integrated circuit products.
** 
**  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
**     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
**     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
**     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
**     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
**     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
**     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
**     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
**     PERFORMANCE OF THE SOFTWARE.
** 
**  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
**     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
**     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
**     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
**     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
**     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
**     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
**     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
**     LIMITED REMEDY.
** :>
 *
 * $brcm_Workfile:$
 * $brcm_Revision:$
 * $brcm_Date:$
 *
 * [File Description:]
 *       UDP, ATM PVC and ADSL EOC transports implemention.
 * Revision History:
 *       author:  ytran
 * $brcm_Log:$
 *
 ***************************************************************************/

#include        <stdio.h>
#include	<sys/types.h>
#include	<sys/socket.h>
#include        <sys/file.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<errno.h>
#include	<ctype.h>
#include	<string.h>
#include        <netdb.h>
#include        <linux/atm.h>
#include "asn1.h"
#include "snmp.h"
#include "agtMib.h"
#include "agtEngine.h"
#include "agtTransport.h"

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"

#ifdef BUILD_SNMP_DEBUG
#include "snmpDebug.h"
#endif

static int snmpUdpReceive(pTRANSPORT_INFO transport, raw_snmp_info_t *message);
static int snmpUdpSend(pTRANSPORT_INFO pTransport, void* msg);
static int snmpUdpClose(void);
static int snmpUdpCreateSocket(int port);
static int snmpUdpOpen(void);
static int snmpEocReceive(pTRANSPORT_INFO transport, raw_snmp_info_t *message);
static int snmpEocSend(pTRANSPORT_INFO pTransport, void* msg);
static int snmpEocClose(void);
static int snmpEocOpen(void);
static int snmpAal5Receive(pTRANSPORT_INFO transport, raw_snmp_info_t *message);
static int snmpAal5Send(pTRANSPORT_INFO pTransport, void* msg);
static int snmpAal5Close(void);
static int snmpAal5Open(void);
static int snmpSmdOpen(void);

pTRANSPORT_LIST pSnmpTransportList = NULL;
extern int trapCode;


void snmpOpenTransport() 
{
  if (pSnmpTransportList != NULL)
    return;

  if ((pSnmpTransportList = malloc(sizeof(TRANSPORT_LIST))) == NULL) {
    cmsLog_error("Error: snmpOpenTransport(): unable to allocate TRANSPORT_LIST\n");
    exit(0);
  }
  memset(pSnmpTransportList,0,sizeof(TRANSPORT_LIST));

  snmpSmdOpen();

#if defined(BUILD_SNMP_UDP) || defined(BUILD_SNMP_AUTO)
  snmpUdpOpen();
#endif
#if defined(BUILD_SNMP_EOC) || defined(BUILD_SNMP_AUTO)
  snmpEocOpen();
#endif
#if defined(BUILD_SNMP_AAL5) || defined(BUILD_SNMP_AUTO)
  snmpAal5Open();
#endif  
}

void snmpCloseTransport()
{
  pTRANSPORT_INFO ptr=NULL, freePtr;

  /* go through transport list, and call close_fn, and then free trasnport_info */
#ifdef BUILD_SNMP_TRANSPORT_DEBUG
  cmsLog_debug("snmpCloseTransport(entry)\n");
#endif

  if (pSnmpTransportList != NULL) {
    /* should be clean, but just in case */
    ptr = pSnmpTransportList->transport;
    while (ptr) {
      ptr->close_fn();
      if (ptr->send_fn != NULL)
      {
      close(ptr->socket);
      }
      freePtr = ptr;
      ptr = ptr->next;
      free(freePtr);
    }
    free(pSnmpTransportList);
  }
  return;
}

void snmpAddTransportToList(pTRANSPORT_INFO pNew)
{
#ifdef BUILD_SNMP_TRANSPORT_DEBUG
  pTRANSPORT_INFO ptr;
  int debug = 0;
  if (snmpDebug.transport)
    debug = 1;
#endif
  if (pSnmpTransportList->transport == NULL) {
    pSnmpTransportList->transport = pNew;
    pSnmpTransportList->tail = pNew;
  }
  else {
    pSnmpTransportList->tail->next = pNew;
    pSnmpTransportList->tail = pNew;
  }
#ifdef BUILD_SNMP_TRANSPORT_DEBUG
  if (debug) {
    ptr = pSnmpTransportList->transport;
    cmsLog_debug("snmpAddTransportToList(): List:");
    while (ptr) {
      cmsLog_debug("socket %d, name %s",ptr->socket,ptr->name);
      ptr = ptr->next;
    }
  }
#endif  /* BUILD_SNMP_TRANSPORT_DEBUG */
} 

pTRANSPORT_INFO snmpRemoveTransportFromList(char *name)
{
  pTRANSPORT_INFO ptr = pSnmpTransportList->transport;
  pTRANSPORT_INFO ptrPrev = ptr;
  pTRANSPORT_INFO old=NULL;
#ifdef BUILD_SNMP_TRANSPORT_DEBUG
  int debug = 0;
  if (snmpDebug.transport)
    debug = 1;
#endif

  while (ptr != NULL) {
    if (strncmp(ptr->name,name,TRANSPORT_NAME_LEN_MAX) == 0) {
      old = ptr;
      if (ptr == ptrPrev) 
        pSnmpTransportList->transport = ptr->next;
	  else if (ptr == pSnmpTransportList->tail) {
        pSnmpTransportList->tail = ptrPrev;
		pSnmpTransportList->tail->next = NULL;
	  }
      else 
        ptrPrev->next = ptr->next;
      break;
    }
    else {
      ptrPrev = ptr;
      ptr = ptr->next;
    }
  } /* while transport list */
#ifdef BUILD_SNMP_TRANSPORT_DEBUG
  if (debug) {
    ptr = pSnmpTransportList->transport;
    cmsLog_debug("snmpRemoveTransportFromList(): List:");
    while (ptr) {
      cmsLog_debug("socket %d, name %s",ptr->socket,ptr->name);
      ptr = ptr->next;
    }
  }
#endif  /* BUILD_SNMP_TRANSPORT_DEBUG */
  return old;
} 

/* go through transport list to see which one has data for processing */
/* return fdSet count */
int snmpTransportSelect(fd_set *readfds)
{
  int numfds=0, count;
  struct timeval timo;
  pTRANSPORT_INFO ptr = pSnmpTransportList->transport;

  FD_ZERO(readfds);
  
  while (ptr != NULL) {
    /* does it matter what socket + 1 is?  depending on list of ptr->socket? */
    numfds = ptr->socket + 1;
    FD_SET(ptr->socket, readfds);
    ptr = ptr->next;
  }
  timo.tv_sec = 0;
  timo.tv_usec = 500000; /* what should this be ? 5 msec; after time, request
                          should be resent again */
  count = select(numfds,readfds,0,0,&timo);
  return count;
}

int snmpTransportRead(fd_set *readfds)
{
  pTRANSPORT_INFO ptr = pSnmpTransportList->transport;
  int ret;

  cmsLog_debug("readfds(%p)", readfds);

  while (ptr != NULL) {
    if (FD_ISSET(ptr->socket, readfds)) 
    {
       ret = snmp_process_message(ptr);
       cmsLog_debug("snmp_process_message returned %d", ret);
    }
    ptr = ptr->next;
  }

  cmsLog_debug("======= end =======");

  return 1;
}

int snmpTransportSend(pTRANSPORT_INFO transport, void *msg)
{
  pTRANSPORT_INFO ptr;
  int ret = 0;

  cmsLog_debug("getting ready to send: transport %p",transport);

  if (transport == NULL)
  {
     ptr= pSnmpTransportList->transport;
     while (ptr != NULL)
     {
        if (ptr->send_fn != NULL)
        {
           ret = ptr->send_fn(ptr,msg);
        }
        ptr = ptr->next;
     }
  } /* transport == NULL */
  else
  {
     if (transport->send_fn != NULL)
     {
        ptr= transport;
        ret = transport->send_fn(ptr,msg);
     }
  }

  cmsLog_debug("End");

  return ret;
}

/* ==============================UDP Transport ============================== */
#if defined(BUILD_SNMP_UDP) || defined(BUILD_SNMP_AUTO)
static int snmpUdpReceive(pTRANSPORT_INFO transport, raw_snmp_info_t *message)
{

    int snmp_socket = transport->socket;

    bzero(message, sizeof(raw_snmp_info_t));

    /* Read SNMP request */
    message->originatorAddressLength = sizeof(message->originatorAddress);
    message->request_length = recvfrom(snmp_socket, message->request, SNMP_MAX_MSG_LENGTH, 0,
                (struct sockaddr *)&message->originatorAddress, 
                &message->originatorAddressLength);

    if (message->request_length < 0) {
        cmsLog_error("recvfrom");
        return(0);
    }

#ifdef BUILD_SNMP_TRANSPORT_DEBUG
    if ((snmpDebug.transport & SNMP_DEBUG_LEVEL_TRANSPORT_UDP) == 
        SNMP_DEBUG_LEVEL_TRANSPORT_UDP) {
        cmsLog_debug("Received %d bytes from %s:", message->request_length,
              inet_ntoa(message->originatorAddress.sin_addr));
        snmp_print_message(message->request, message->request_length);
    }
#endif

    return (message->request_length);
}

static int snmpUdpSend(pTRANSPORT_INFO pTransport, void* msg)
{
  extern unsigned char g_trap_addr[4];
  int socket = pTransport->socket;
  raw_snmp_info_t *message = (raw_snmp_info_t*)msg;
  struct sockaddr_in dest;
  int i;

  if ((message->mesg.pdutype == SNMP_TRP2_REQ_PDU) || 
      (message->mesg.pdutype == SNMP_TRP_REQ_PDU)) 
  {
     if (pTransport->trapEnable)
     {
        /* don't do any sending if trap_addr is not set */
        if (g_trap_addr[0] == 0 && g_trap_addr[1] == 0 &&
            g_trap_addr[2] == 0 && g_trap_addr[3] == 0)
           return (0);

        memcpy(&dest.sin_addr,g_trap_addr,sizeof(g_trap_addr));

        dest.sin_port = htons((short)SNMP_TRAP_PORT);
        dest.sin_family = AF_INET;
        if (sendto(socket, (char *)message->response,
                   message->response_length, 0,
                   (struct sockaddr*)&dest,
                   sizeof(dest)) < 0)
        {
           cmsLog_error("sendto trapSocket");
           return (0);
        }
     }      
  }
  else
  {
     if (sendto(socket, (char *)message->response,
                message->response_length, 0,
                (struct sockaddr *)&message->originatorAddress, 
                message->originatorAddressLength) < 0)
     {
        cmsLog_error("snmpUdpSend():sendto");
        return(0);
     }
  }
  return(message->response_length);
}

static int snmpUdpClose(void)
{
  pTRANSPORT_INFO ptr=NULL;
  
  /* data */
  ptr = snmpRemoveTransportFromList(TRANSPORT_NAME_UDP);
  if (ptr != NULL) {
    close(ptr->socket);
    free(ptr);
  }
  /* trap */
  ptr = snmpRemoveTransportFromList(TRANSPORT_NAME_UDP);
  if (ptr != NULL) {
    close(ptr->socket);
    free(ptr);
  }

  return 0;
}

static int snmpUdpCreateSocket(int port)
{
  struct sockaddr_in mySocketAddress;
  pTRANSPORT_INFO pNew = NULL;
  
  cmsLog_debug("port %d", port);

  if ((pNew = malloc(sizeof(TRANSPORT_INFO))) == NULL) {
    cmsLog_error("snmpUdpOpen(): unable to allocate TRANSPORT_INFO\n");
    return -1;
  }
  /* create data port */
  pNew->socket = socket(AF_INET, SOCK_DGRAM, 0);
  if (pNew->socket  < 0){
    cmsLog_error("socket");
    cmsLog_error("error calling \"socket()\"\n.");
    free(pNew);
    return -1;
  }
  mySocketAddress.sin_family = AF_INET;
  mySocketAddress.sin_addr.s_addr = INADDR_ANY;
  if (sizeof (mySocketAddress.sin_port) == 2) {
    *(short *)&mySocketAddress.sin_port = htons((short)port);
  } else if (sizeof (mySocketAddress.sin_port) == 4) {
    *(long *)&mySocketAddress.sin_port = htonl(port);
  } else {
    return -1;
  }
  if (port ==  SNMP_PORT) {
    pNew->dataEnable = 1;
    pNew->trapEnable = 0;
    strcpy(pNew->name,TRANSPORT_NAME_UDP);
  }
  else {
    pNew->dataEnable = 0;
    pNew->trapEnable = 1;
    strcpy(pNew->name,TRANSPORT_NAME_UDP_TRAP);
  }
  if (bind(pNew->socket, (struct sockaddr *)&mySocketAddress,
           sizeof(mySocketAddress))){
    cmsLog_error("bind");
    cmsLog_error("error calling \"bind()\"\n.");
    free(pNew);
    return -1;
  }
#ifdef BUILD_SNMP_TRANSPORT_DEBUG
  if ((snmpDebug.transport & SNMP_DEBUG_LEVEL_TRANSPORT_UDP) == 
      SNMP_DEBUG_LEVEL_TRANSPORT_UDP) {
    cmsLog_debug("Port %u succesfully opened.", port);
  }
#endif

  pNew->send_fn = snmpUdpSend;
  pNew->receive_fn = snmpUdpReceive;
  pNew->close_fn = snmpUdpClose;
  pNew->next = NULL;
  pNew->temp = 0;
  snmpAddTransportToList(pNew);
  return 0;
}

static int snmpUdpOpen(void)
{
  int ret;
  ret = snmpUdpCreateSocket(SNMP_PORT);
  ret = snmpUdpCreateSocket(SNMP_TRAP_PORT);
  return (ret);
}
#endif /* defined(BUILD_SNMP_UDP) || defined(BUILD_SNMP_AUTO) */
/* ==============================UDP Transport End============================== */

#if defined(BUILD_SNMP_EOC) || defined(BUILD_SNMP_AUTO)
/* ==============================EOC Transport ================================= */
static int snmpEocReceive(pTRANSPORT_INFO transport, raw_snmp_info_t *message)
{
    int fd = transport->socket;
    int len, bytesRead;

    len = sizeof(message->request);
    bzero(message, sizeof(raw_snmp_info_t));

    bytesRead = read(fd,(void*)message->request,len);

	/* this applies to CT */
#if defined(BUILD_SNMP_AAL5) || defined(BUILD_SNMP_AUTO)
	if (transport->temp) {
#if BUILD_SNMP_TRANSPORT_DEBUG
	    cmsLog_debug("eocReceive(): shutting down aal5");
#endif
		transport->temp = 0;
        snmpAal5Close();
	}
#endif

    if (bytesRead <= ADSL_EOC_LINKUP_MSG_LEN)
      return 0;
    
    message->request_length = bytesRead;
#ifdef BUILD_SNMP_TRANSPORT_DEBUG
    if ((snmpDebug.transport & SNMP_DEBUG_LEVEL_TRANSPORT_EOC) == 
        SNMP_DEBUG_LEVEL_TRANSPORT_EOC) {
      cmsLog_debug("Received %d bytes from EOC channel:", message->request_length);
      snmp_print_message(message->request, message->request_length);
    }
#endif
    return bytesRead;
}

static int snmpEocSend(pTRANSPORT_INFO pTransport, void* msg)
{
  int fd = pTransport->socket;
  int bytesWrote;
  raw_snmp_info_t *message = (raw_snmp_info_t*)msg;

#ifdef BUILD_SNMP_TRANSPORT_DEBUG
  if ((snmpDebug.transport & SNMP_DEBUG_LEVEL_TRANSPORT_EOC) == 
          SNMP_DEBUG_LEVEL_TRANSPORT_EOC) {
    cmsLog_debug("Sending %d bytes to EOC:", message->response_length);
  }
#endif
  bytesWrote= write(fd,message->response,message->response_length);
  if (bytesWrote <= 0) {
    cmsLog_error("snmpEocSend(): error writing data to device, write returns %d\n",
           bytesWrote);
  }
  return bytesWrote;
}

static int snmpEocClose(void)
{
  pTRANSPORT_INFO ptr=NULL;
  
  ptr = snmpRemoveTransportFromList(TRANSPORT_NAME_EOC);
  if (ptr != NULL) {
    close(ptr->socket);
    free(ptr);
  }
}

static int snmpEocOpen(void)
{
  pTRANSPORT_INFO pNew = NULL;

  if ((pNew = malloc(sizeof(TRANSPORT_INFO))) == NULL) {
    cmsLog_error("snmpEocOpen(): unable to allocate TRANSPORT_INFO\n");
    exit(0);
  }

  pNew->socket = open(ADSL_DEVICE, O_RDWR);
  strcpy(pNew->name,TRANSPORT_NAME_EOC);
  pNew->dataEnable = 1;
  pNew->trapEnable = 1;
  pNew->temp = 1;
  if (pNew->socket  < 0){
    cmsLog_error("socket");
    cmsLog_error("error calling \"socket()\"\n.");
    free(pNew);
    exit(2);
  }
#ifdef BUILD_SNMP_TRANSPORT_DEBUG
  if ((snmpDebug.transport & SNMP_DEBUG_LEVEL_TRANSPORT_EOC) == 
      SNMP_DEBUG_LEVEL_TRANSPORT_EOC) {
    cmsLog_debug("ADSL device %s succesfully opened, fd %d.\n",ADSL_DEVICE,pNew->socket);
  }
#endif
  pNew->send_fn = snmpEocSend;
  pNew->receive_fn = snmpEocReceive;
  pNew->close_fn = snmpEocClose;
  pNew->next = NULL;
  //  pNew->formataddr_fn = NULL;
  snmpAddTransportToList(pNew);
  return (pNew->socket);
}
#endif /* defined(BUILD_SNMP_EOC) || defined(BUILD_SNMP_AUTO) */
/* ==============================EOC Transport End============================== */

/* ==============================AAL5 Transport ================================ */
#if defined(BUILD_SNMP_AAL5) || defined(BUILD_SNMP_AUTO)
static int snmpAal5Receive(pTRANSPORT_INFO transport, raw_snmp_info_t *message)
{
  int socket = transport->socket;
  int bytesRead = -1;

  bzero(message, sizeof(raw_snmp_info_t));
  if (socket >= 0) {
    while (bytesRead < 0) {
      bytesRead = recv(socket,(void*)message->request, SNMP_MAX_MSG_LENGTH, 0);
      if (bytesRead < 0 && errno != EINTR) {
        break;
      }
    } /* while */
  }

  if (bytesRead >= 0) {
    message->request_length = bytesRead;
#ifdef BUILD_SNMP_TRANSPORT_DEBUG
    if ((snmpDebug.transport & SNMP_DEBUG_LEVEL_TRANSPORT_AAL5) == 
        SNMP_DEBUG_LEVEL_TRANSPORT_AAL5) {
      cmsLog_debug("Received %d bytes from ATM:", message->request_length);
      snmp_print_message(message->request, message->request_length);
    }
#endif

#if defined(BUILD_SNMP_EOC) || defined(BUILD_SNMP_AUTO)
	/* this applies to CT */
	if (transport->temp) {
#if BUILD_SNMP_TRANSPORT_DEBUG
	cmsLog_debug("aal5Receive(): shutting down EOC");
#endif
		transport->temp = 0;
		snmpEocClose();
	}
#endif
  }    
  else {
#ifdef BUILD_SNMP_TRANSPORT_DEBUG
    if ((snmpDebug.transport & SNMP_DEBUG_LEVEL_TRANSPORT_AAL5) == 
        SNMP_DEBUG_LEVEL_TRANSPORT_AAL5) {
      cmsLog_error("Error receiving from ATM\n");
    }
#endif
    return 0;
  }
  return bytesRead;
}

static int snmpAal5Send(pTRANSPORT_INFO pTransport, void* msg)
{
  int bytesWrote = -1;
  int socket = pTransport->socket;
  raw_snmp_info_t *message = (raw_snmp_info_t*)msg;

#ifdef BUILD_SNMP_TRANSPORT_DEBUG
  if ((snmpDebug.transport & SNMP_DEBUG_LEVEL_TRANSPORT_AAL5) == 
          SNMP_DEBUG_LEVEL_TRANSPORT_AAL5) {
    cmsLog_debug("Sending %d bytes to ATM:\n", message->response_length);

  }
#endif
  while (bytesWrote < 0) {
    bytesWrote = send(socket, message->response, message->response_length, 0);
    if (bytesWrote < 0 && errno != EINTR) {
      break;
    }
  } /* while */
  return bytesWrote;
}

int agtIsAal5VcCreated(char *addr) 
{

#ifdef NEED_CHANGE
  FILE *fs;
  char line[SYS_CMD_LEN];
  int found = 0;
  char cmd[SYS_CMD_LEN];

  if (access("/var/vclerr",F_OK) == 0)
    bcmSystemMute("rm /var/vclerr");

  sprintf(cmd, "atmctl vcc --show > /var/vcl");
  bcmSystemMute(cmd);

  fs = fopen("/var/vcl", "r");
  if (fs == NULL)
    return found;

  /* when nothing is created, format looks like this :
     vcc         status      type            tdte_index  q_size  q_priority  encapsulation
     when a VCC is created, format looks like this:
     vcc         status      type            tdte_index  q_size  q_priority  encapsulation
     0.0.16      enabled     aal5            1           10      1           llcencaps
  */
  /* skip heading line */
  fgets(line, SYS_CMD_LEN, fs);
  while (fgets(line, SYS_CMD_LEN, fs) != NULL) {
    if (strstr(line,addr) != NULL) {
      found = 1;
      break;
    }
  } /* while */
  fclose(fs);
  bcmSystemMute("rm /var/vcl");
  return (found);
#else
  return 0;
#endif
}

static int snmpAal5Close(void)
{
  pTRANSPORT_INFO ptr=NULL;
  char cmd[SYS_CMD_LEN];
  char vccAddr[16];
  
  ptr = snmpRemoveTransportFromList(TRANSPORT_NAME_ATM);
  if (ptr != NULL) {
    close(ptr->socket);
    free(ptr);
    memset(vccAddr,0,sizeof(vccAddr));
    sprintf(vccAddr,"%d.%d.%d",0,AAL5_TRANSPORT_VPI,AAL5_TRANSPORT_VCI);

#ifdef NEED_CHANGE
    /* delete VCC if it exists */
    if (agtIsAal5VcCreated(vccAddr)) {
        /* atmctl vcc --delete port.vpi.vci */ 
		sprintf(cmd, "atmctl vcc --delete %s 2> /var/vclerr",
			    vccAddr);
		bcmSystemMute(cmd);
	}
#endif /* NEED_CHANGE */
  }
}

static int snmpAal5Open()
{
  struct sockaddr_atmpvc addr;
  struct atm_qos  qos;
  pTRANSPORT_INFO pNew = NULL;
  char cmd[SYS_CMD_LEN];
  char vccAddr[16];
  FILE* fs = NULL;

  if ((pNew = malloc(sizeof(TRANSPORT_INFO))) == NULL) {
    cmsLog_error("snmpAal5Open(): unable to allocate TRANSPORT_INFO\n");
    exit(0);
  }
  memset(vccAddr,0,sizeof(vccAddr));
  sprintf(vccAddr,"%d.%d.%d",0,AAL5_TRANSPORT_VPI,AAL5_TRANSPORT_VCI);

#ifdef NEED_CHANGE
  /* create VCC  AAL5_TRANSPORT_VPI/AAL5_TRANSPORT_VCI if it doesn't exist */
  if (!agtIsAal5VcCreated(vccAddr)) {
      /* atmctl vcc --add port.vpi.vci aal5 tdIndex llcencaps --addq port.vpi.vci size=10 priority=1 */ 
    sprintf(cmd, "atmctl vcc --add %s aal5 1 llcencaps --addq  %s 10 1 2> /var/vclerr",
            vccAddr,vccAddr);
    bcmSystemMute(cmd);
    sleep(1);
    if (!agtIsAal5VcCreated(vccAddr)) {
      cmsLog_error("unable to create VCC %s\n",vccAddr);
      return -1;
    }
  }
#endif /* #ifdef NEED_CHANGE */

  addr.sap_family = AF_ATMPVC;
  addr.sap_addr.itf = 0;
  addr.sap_addr.vpi = AAL5_TRANSPORT_VPI;
  addr.sap_addr.vci = AAL5_TRANSPORT_VCI;  
  strcpy(pNew->name,TRANSPORT_NAME_ATM);
  pNew->dataEnable = 1;
  pNew->trapEnable = 1;
  pNew->socket = socket(PF_ATMPVC, SOCK_DGRAM, 0);
  if (pNew->socket  < 0){
    cmsLog_error("socket");
    cmsLog_error("error calling \"socket()\"\n.");
    free(pNew);
    exit(2);
  }

  memset(&qos, 0, sizeof(struct atm_qos));
  qos.aal = ATM_AAL5;
  qos.rxtp.traffic_class = ATM_UBR;
  qos.rxtp.max_sdu = SNMP_MAX_MSG_LENGTH;
  qos.txtp = qos.rxtp;

  if (setsockopt(pNew->socket, SOL_ATM, SO_ATMQOS, &qos, sizeof(qos)) < 0) {
    cmsLog_error("snmpAal5Open(): error setting ATM qos socket\n");
    goto aalOpenError;
  }
  if (bind(pNew->socket, (struct sockaddr *)&addr,
           sizeof(struct sockaddr_atmpvc)) < 0) {
    cmsLog_error("snmpAal5Open(): bind socket failed.\n");
    goto aalOpenError;
  }

#if 0
  if (connect(pNew->sock, (struct sockaddr *) addr,
              sizeof(struct sockaddr_atmpvc)) < 0) {
    cmsLog_error("snmpAal5Open(): connect socket failed.\n");
    goto aalOpenError;
  }
#endif

#ifdef BUILD_SNMP_TRANSPORT_DEBUG
  if ((snmpDebug.transport & SNMP_DEBUG_LEVEL_TRANSPORT_AAL5) == 
      SNMP_DEBUG_LEVEL_TRANSPORT_AAL5) {
    cmsLog_debug("ATM transport vpi/vci %d/%d succesfully opened, fd %d.",
           addr.sap_addr.vpi,addr.sap_addr.vci,pNew->socket);
  }
#endif

  pNew->send_fn = snmpAal5Send;
  pNew->receive_fn = snmpAal5Receive;
  pNew->close_fn = snmpAal5Close;
  pNew->next = NULL;
  pNew->temp = 1;
  snmpAddTransportToList(pNew);
  return (pNew->socket);

 aalOpenError:
  close(pNew->socket);
  free(pNew);
  return -1;
}

#endif /* defined(BUILD_SNMP_AAL5) || defined(BUILD_SNMP_AUTO) */
/* ==============================AAL5 Transport End============================= */


extern void *msgHandle;
static int snmpRxMsgFromSmd(pTRANSPORT_INFO transport, raw_snmp_info_t *message)
{
   CmsMsgHeader *msg=NULL;
   int byteRead = 0;
   CmsRet ret;

   cmsLog_debug("Entered");

   if ((ret = cmsMsg_receive(msgHandle, &msg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not receive msg, ret=%d", ret);
   }

   switch (msg->type)
   {
   case CMS_MSG_SET_LOG_LEVEL:
      cmsLog_setLevel(msg->wordData);
      if ((ret = cmsMsg_sendReply(msgHandle, msg, CMSRET_SUCCESS)) != CMSRET_SUCCESS)
      {
         cmsLog_error("send response for msg 0x%x failed, ret=%d", msg->type, ret);
      }
      break;

   case CMS_MSG_SET_LOG_DESTINATION:
      cmsLog_setDestination(msg->wordData);
      if ((ret = cmsMsg_sendReply(msgHandle, msg, CMSRET_SUCCESS)) != CMSRET_SUCCESS)
      {
         cmsLog_error("send response for msg 0x%x failed, ret=%d", msg->type, ret);
      }
      break;

   case CMS_MSG_WAN_CONNECTION_UP:
      cmsLog_debug("receive CMS_MSG_WAN_CONNECTION_UP! sendStartTrap (trapcode %d)",trapCode);
      sendStartTrap(trapCode);
      break;

   }

   cmsLog_debug("Ended");
   return byteRead;   
}

static int snmpSmdClose(void)
{
   cmsLog_debug("Entered");

   cmsMsg_cleanup(&msgHandle);

   cmsLog_debug("End");
   return (CMSRET_SUCCESS);
}

static int snmpSmdOpen(void)
{
  struct sockaddr_in mySocketAddress;
  pTRANSPORT_INFO pNew = NULL;
  int commFd;
  
  cmsLog_debug("Entered");

  if ((pNew = malloc(sizeof(TRANSPORT_INFO))) == NULL)
  {
    cmsLog_error("unable to allocate TRANSPORT_INFO\n");
  }
  /* create data port */
  /* get a file descriptor for SMD and SNMP communication */
  cmsMsg_getEventHandle(msgHandle, &commFd);
  strcpy(pNew->name,TRANSPORT_NAME_SMD);
  pNew->socket = commFd;
  pNew->send_fn = NULL;
  pNew->receive_fn = snmpRxMsgFromSmd;
  pNew->close_fn = snmpSmdClose;
  pNew->next = NULL;
  pNew->temp = 0;
  snmpAddTransportToList(pNew);

  cmsLog_debug("Ended");
  return (CMSRET_SUCCESS);
}

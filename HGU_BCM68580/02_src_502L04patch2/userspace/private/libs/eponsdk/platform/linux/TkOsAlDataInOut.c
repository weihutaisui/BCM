/*
* <:copyright-BRCM:2013:proprietary:epon
* 
*    Copyright (c) 2013 Broadcom 
*    All Rights Reserved
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
*/
#ifdef DESKTOP_LINUX
#include <bits/sockaddr.h>
#include <sys/un.h>
#endif
#include <linux/netlink.h>
#include <sys/sysctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include "bcmnetlink.h"
#include "TkPlatform.h"
#include "TkOamCommon.h"
#include "TkOamMem.h"
#include "TkOs.h"
#include "cms_log.h"
#include "OsAstMsgQ.h"
#include "OntmTimer.h"
#include "eponctl_api.h"
#include "bcm_epon_cfg.h"
#include "EponDevInfo.h"
#ifdef BRCM_CMS_BUILD
#include "cms_msg.h"
#include "cms_mem.h"
#include "../../../cms_core/linux/rut_eponwan.h"
#include "../../../cms_core/linux/rut_util.h"
#endif

#include <linux/rtnetlink.h>
#ifdef DPOE_OAM_BUILD
#include "EapTls.h"
#include <sys/time.h>
#endif
#include "OamCmdsPro.h"
#include <sys/un.h>

static int fd;
static int eponMonitorFd = CMS_INVALID_FD;
static BOOL (*TkRxDataDispatchHandle)( TkMsgBuff *pSrcMsg, TkMsgBuff *pResMsg, U16 len );
static int if_index;
static TkOsThread RxDataThreadId = 0;
extern void *msgHandle;
extern BOOL stand_alone_mode;
extern EponDevInfo eponDevInfo;  


int (*eponsdk_igmp_process_input_func)(t_BCM_MCAST_PKT_INFO *pkt_info);
int (*eponsdk_mld_process_input_func)(t_BCM_MCAST_PKT_INFO *pkt_info);

static BOOL TkOsDataRx(U8 *buf, U16 *len );
static void TkDataRxThread(void);

#define EPONSDK_PERIODIC_TASK_INTERVAL 5

static int netlinkRouteFd = CMS_INVALID_FD;
int netlinkSnoopFd = CMS_INVALID_FD;
static char *netlinkSnoopBuff;

static SINT32 OamCmdsServerFd = CMS_INVALID_FD;
static SINT32 OamCmdsDataFd = CMS_INVALID_FD;

#ifdef DPOE_OAM_BUILD
static int eapolFd = CMS_INVALID_FD;
#define CAP_EAPOL_PKT_TYPE 0x888e
Bool initEapolFd(void)
{
    struct ifreq req;
    struct sockaddr_ll sll;

    if ( (eapolFd = socket(AF_PACKET, SOCK_RAW, htons( CAP_EAPOL_PKT_TYPE ) ) ) < 0 )
    {
        cmsLog_error( "failed to create raw socket!" );
        return FALSE;
    }

    strcpy( req.ifr_name, EPON_OAM_IF_NAME );

    if ( ioctl( eapolFd, SIOCGIFFLAGS, &req ) < 0 )
    {
        cmsLog_error( "failed to do ioctl!" );
        return FALSE;
    }

    req.ifr_flags |= IFF_PROMISC;

    if ( ioctl( eapolFd, SIOCSIFFLAGS, &req ) < 0 )
    {
        cmsLog_error( "failed to set %s into promisc mode!", EPON_OAM_IF_NAME );
        return FALSE;
    }

    bzero(&sll, sizeof(sll));

    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = if_index;
    sll.sll_protocol = htons(CAP_EAPOL_PKT_TYPE);
    
    if((bind(eapolFd, (struct sockaddr *)&sll, sizeof(sll)))== -1)
    {
        cmsLog_error("Error binding raw socket to interface");
        return FALSE;
    }

    return TRUE;
}


BOOL EapolDataTx(U8 *buf, U16 len )
{
    struct sockaddr_ll link;

    link.sll_family = AF_PACKET;
    link.sll_pkttype = PACKET_OUTGOING;
    link.sll_ifindex = if_index;
    link.sll_halen = 6;
            
    memcpy( link.sll_addr, buf, link.sll_halen );
    if (len < 60)
        {// finish out packet with zeros
        memset (buf + len, 0, 60 - len);
        len = 60;
        }
    
    if ( sendto( eapolFd, buf, len, 0, (struct sockaddr *)&link, sizeof(link) ) < 0 )
    {
        cmsLog_error( "failed to send to RAW socket!" );
                    
        return FALSE;
    }
    return TRUE;
}


static BOOL EapolDataRx(U8 *buf, U16 *len )
{
    S32 length = 0;

    length = recvfrom(eapolFd, buf, TK_MAX_RX_TX_DATA_LENGTH, 0, NULL, NULL );
    if ( length < 0 )
    {
        cmsLog_error("recv data error");
        return FALSE;
    }   
    else
    {
        *len = length;
        return TRUE;
    }                           
}

struct DpoeOamRecvRespMonitor
{
    pthread_mutex_t mutex_recv_resp;
    pthread_cond_t cond_recv;
    pthread_cond_t cond_resp;
};

static struct DpoeOamRecvRespMonitor oam_mon =
{
    .mutex_recv_resp = PTHREAD_MUTEX_INITIALIZER,
    .cond_recv = PTHREAD_COND_INITIALIZER,
    .cond_resp = PTHREAD_COND_INITIALIZER,
};

void OamRecvNotify(void)
{
    pthread_mutex_lock(&oam_mon.mutex_recv_resp);
    pthread_cond_signal(&oam_mon.cond_recv);
    pthread_mutex_unlock(&oam_mon.mutex_recv_resp);
}

void OamRespDoneNotify(void)
{
    pthread_mutex_lock(&oam_mon.mutex_recv_resp);
    pthread_cond_signal(&oam_mon.cond_resp);
    pthread_mutex_unlock(&oam_mon.mutex_recv_resp);
}

void WaitOamRecvNotify(void)
{
    pthread_mutex_lock(&oam_mon.mutex_recv_resp);
    pthread_cond_wait(&oam_mon.cond_recv, &oam_mon.mutex_recv_resp);
    pthread_mutex_unlock(&oam_mon.mutex_recv_resp);
}

static void TkOnuBusyAlarmSend(BOOL set)
{
    U8 dpoe_onu_busy_set[] = {
        0x01, 0x80, 0xC2, 0x00, 0x00, 0x02,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x88, 0x09, 0x03, 0x00, 0x50, 0x01,
        0x00, 0x01, 0xFE, 0x09, 0x00, 0x10,
        0x00, 0x82, 0x01, 0x00, 0x00, 0x00, 0x00};
    U8 dpoe_onu_busy_clr[] = {
        0x01, 0x80, 0xC2, 0x00, 0x00, 0x02,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x88, 0x09, 0x03, 0x00, 0x50, 0x01,
        0x00, 0x01, 0xFE, 0x09, 0x00, 0x10,
        0x00, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00};
        
    if (set)
    {
        TkOamEthSend(0, (MacAddr *)&dpoe_onu_busy_set[0],
            &dpoe_onu_busy_set[0], sizeof(dpoe_onu_busy_set));
    }
    else
    {
        TkOamEthSend(0, (MacAddr *)&dpoe_onu_busy_clr[0],
            &dpoe_onu_busy_clr[0], sizeof(dpoe_onu_busy_clr));
    }
}

BOOL WaitOamRespDoneNotifyTimeout(void) //1-2s
{
    struct timespec timer;
    int ret;
    BOOL timeout = FALSE;
    
    timer.tv_sec = time(NULL) + 2;
    timer.tv_nsec = 0;
    pthread_mutex_lock(&oam_mon.mutex_recv_resp);
    ret = pthread_cond_timedwait(&oam_mon.cond_resp, &oam_mon.mutex_recv_resp, &timer);
    if (ret == ETIMEDOUT)
    {
        TkOnuBusyAlarmSend(TRUE);
        timeout = TRUE;
    }
    pthread_mutex_unlock(&oam_mon.mutex_recv_resp);
    return timeout;
}

void WaitOamRespDoneNotify(void)
{
    pthread_mutex_lock(&oam_mon.mutex_recv_resp);
    pthread_cond_wait(&oam_mon.cond_resp, &oam_mon.mutex_recv_resp);
    TkOnuBusyAlarmSend(FALSE);
    pthread_mutex_unlock(&oam_mon.mutex_recv_resp);
}

void TkOnuBusyMonitorThread(void)
{
    while(1)
    {
        WaitOamRecvNotify();
        if (WaitOamRespDoneNotifyTimeout())
        {
            WaitOamRespDoneNotify();
        }
    }
}

BOOL TkOsOnuBusyMonitorThreadInit(void)
{
    TkOsThreadCreate(
        110,
        TkOnuBusyMonitorThread,
        0,
        0,
        (U8 *)"TkOnuBusyMonitorThread",
        0,0,0,0,0);
    return TRUE;
}
#endif


static CmsRet initEponStackMonitorFd(void)
{
    CmsRet ret = CMSRET_SUCCESS;

    struct sockaddr_nl addr;

    if((eponMonitorFd = socket(AF_NETLINK, SOCK_RAW, NETLINK_BRCM_EPON)) < 0)
    {
        cmsLog_error("failed to creat netlink socket");
        eponMonitorFd = 0;
        return CMSRET_INTERNAL_ERROR;
    }
    else
    {
        cmsLog_debug("Epon monitor Fd=%d", eponMonitorFd);
    }

    addr.nl_family = AF_NETLINK;
    addr.nl_pid = getpid();
    addr.nl_groups = 0;

    if(bind(eponMonitorFd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        cmsLog_error("could not bind netlink socket for EPON monitor");
        close(eponMonitorFd);
        eponMonitorFd = 0;
        return CMSRET_INTERNAL_ERROR;
    }
    eponStack_CtlSetPid(getpid());
    return ret;
}


static CmsRet initNetlinkRouteFd(void)
{
    CmsRet ret = CMSRET_SUCCESS;

    struct sockaddr_nl addr;

    if((netlinkRouteFd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE)) < 0)
    {
        cmsLog_error("failed to creat netlink Route socket");
        netlinkRouteFd = 0;
        return CMSRET_INTERNAL_ERROR;
    }
    else
    {
        cmsLog_debug("netlink Route Fd=%d", netlinkRouteFd);
    }
    memset (&addr, 0, sizeof (addr));
    addr.nl_family = AF_NETLINK;
    addr.nl_groups = RTMGRP_LINK;//network interface create/delete/up/down events
    

    if(bind(netlinkRouteFd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        cmsLog_error("could not bind netlink socket");
        close(netlinkRouteFd);
        netlinkRouteFd = 0;
        return CMSRET_INTERNAL_ERROR;
    }
    
    return ret;
}

void snoop_netlink_process_msg(int type, unsigned char *pdata, int data_len)
{
    t_BCM_MCAST_PKT_INFO *pkt_info;

    pkt_info = (t_BCM_MCAST_PKT_INFO *)pdata;
    if ((type == BCM_MCAST_MSG_IGMP_PKT) && (NULL != eponsdk_igmp_process_input_func))
    {
        eponsdk_igmp_process_input_func(pkt_info);
    }
    else if ((type == BCM_MCAST_MSG_MLD_PKT) && (NULL != eponsdk_mld_process_input_func))
    {    
        eponsdk_mld_process_input_func(pkt_info);
    }
}

#if defined(DMP_X_BROADCOM_COM_IGMPSNOOP_1) || defined(DMP_X_BROADCOM_COM_MLDSNOOP_1)
static int snoop_netlink_recv_mesg(void)
{
    if ( bcm_mcast_api_nl_recv(netlinkSnoopFd, netlinkSnoopBuff, BCM_MCAST_NL_RX_BUF_SIZE, snoop_netlink_process_msg) < 0 )
    {
        cmsLog_error("Error receiving message\n");
        return CMSRET_INTERNAL_ERROR;
    }

    return CMSRET_SUCCESS;
}

static CmsRet initnetlinkSnoopFd(void)
{
    netlinkSnoopBuff = (char *)malloc(BCM_MCAST_NL_RX_BUF_SIZE);
    if ( NULL == netlinkSnoopBuff )
    {
        return CMSRET_INTERNAL_ERROR;
    }

    /* create socket with portid "EPON" */
    if ( bcm_mcast_api_socket_create(&netlinkSnoopFd, 0x45504F4E) < 0 )
    {
        free(netlinkSnoopBuff);
        netlinkSnoopFd = CMS_INVALID_FD;
        netlinkSnoopBuff = NULL;
        return CMSRET_INTERNAL_ERROR;
    }
   
    return CMSRET_SUCCESS;
}
#endif

typedef BOOL (* NetlinkMsgHandler)(struct msghdr *msg, U16 len);
BOOL TkDataProcessHandle_NetlinkMsg(int socketFd, TkMsgBuff *pSrcMsgBuf,NetlinkMsgHandler NetlinkMsgProcess )
{
    int recvLen = 0;
    struct iovec iov;
    struct sockaddr_nl nl_dstAddr;
    struct msghdr msg;

    if ((NULL == pSrcMsgBuf) || (NULL == NetlinkMsgProcess))
    {
        cmsLog_error("Error buffer or Msghandler of : %d", socketFd);
        return FALSE;
    }

    memset((U8 *)&iov, 0x00, sizeof(struct iovec));
    memset((U8 *)&msg, 0x00, sizeof(struct msghdr));
    memset((U8 *)pSrcMsgBuf->buff, 0x00, sizeof(pSrcMsgBuf->buff));

    iov.iov_base = (void*)pSrcMsgBuf->buff;
    iov.iov_len = sizeof(pSrcMsgBuf->buff);

    msg.msg_name = (void*)&nl_dstAddr;
    msg.msg_namelen = sizeof(nl_dstAddr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;


    recvLen = recvmsg(socketFd, &msg, 0);
    if(recvLen < 0)
    {
        cmsLog_error("Error recvmsg:scoket=%d,len=%d", socketFd,recvLen);
        return  FALSE;
    }
    return NetlinkMsgProcess(&msg, recvLen);
}


static UINT32 OamCmdsHandleConnection(int socketFd)
{
    struct sockaddr_un clientAddr;
    UINT32 sockAddrSize;
    UINT32 fd = CMS_INVALID_FD;

    sockAddrSize = sizeof(clientAddr);
    if ((fd = accept(socketFd, (struct sockaddr *)&clientAddr, &sockAddrSize)) < 0)
        cmsLog_error("accept eponctl connection failed. errno=%d", errno);

    return fd;
}


static void OamCmdsHandleMsg(int socketFd)
{
    OamCmdsMsg cmdsBuf;
    SINT32 rc = 0;

    rc = read(socketFd, &cmdsBuf, sizeof(OamCmdsMsg));

    if ((rc == 0) ||
        ((rc == -1) && ((errno == 131) || (errno == ECONNRESET))))  /* new 2.6.21 kernel seems to give us this before rc==0 */
    {
        /* broken connection */
        return;
    }
    else if (rc < 0 || rc != sizeof(OamCmdsMsg))
    {
        return ;
    }
    
    if (cmdsBuf.cmdType < EponCmdTotalCount)
    {
        OamCmdsProcess(&cmdsBuf);
    }   
}


static void initOamCmdsServerSocket(void)
{
   struct sockaddr_un serverAddr;
   SINT32 rc = 0;
   SINT32 fd = CMS_INVALID_FD;

   unlink(EPON_USER_MSG_ADDR);

   if ((fd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0)
   {
      cmsLog_error("Could not create socket");
      return fd;
   }

   memset(&serverAddr, 0, sizeof(serverAddr));
   serverAddr.sun_family = AF_LOCAL;
   strncpy(serverAddr.sun_path, EPON_USER_MSG_ADDR, sizeof(serverAddr.sun_path));

   rc = bind(fd, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
   if (rc != 0)
   {
      cmsLog_error("bind to %s failed, rc=%d errno=%d", EPON_USER_MSG_ADDR, rc, errno);
      close(fd);
      return;
   }

   rc = listen(fd, EPON_USER_MSG_ADDR);
   if (rc != 0)
   {
      cmsLog_error("listen to %s failed, rc=%d errno=%d", EPON_USER_MSG_ADDR, rc, errno);
      close(fd);
      return;
   }

   OamCmdsServerFd = fd;
   cmsLog_notice("oam cmds server socket opened and ready (fd=%d)", OamCmdsServerFd);
}


static void TkDataRxThread(void)
{
   U16 len = 0;
   TkMsgBuff *pSrcMsgBuf = (TkMsgBuff *)TkOamMemGet();
   TkMsgBuff *pResMsgBuf = (TkMsgBuff *)TkOamMemGet();
   BOOL rxStatus = FALSE;
   SINT32 rc;
   fd_set readFds;
   
#ifdef BRCM_CMS_BUILD
   SINT32 commFd = 0;
   CmsRet cmsRet;

   if(!stand_alone_mode)
   {
       cmsMsg_getEventHandle(msgHandle, &commFd);

       /* start port loop detect app */
       if (rut_sendMsgToSmd(CMS_MSG_START_APP, EID_EPON_OAM_PORT_LOOP_DETECT, NULL, 0)
                  == CMS_INVALID_PID)
       {
          cmsLog_error("failed to start port loop detect app");
       }
   }
#endif
   
   initEponStackMonitorFd();	
   initNetlinkRouteFd();
#if defined(DMP_X_BROADCOM_COM_IGMPSNOOP_1) || defined(DMP_X_BROADCOM_COM_MLDSNOOP_1)
   initnetlinkSnoopFd();
#endif
   initOamCmdsServerSocket();

	while ( 1 )
	{
		struct timeval tv;
		int max_fd = 0;

		FD_ZERO(&readFds);

		max_fd = TkOsTimerInitFdSet(&readFds);

		FD_SET(fd, &readFds);
		max_fd = (max_fd > fd)? max_fd:fd;

#ifdef BRCM_CMS_BUILD
        if(commFd > 0)
        {    
		    FD_SET(commFd, &readFds);
		    max_fd = (max_fd > commFd)? max_fd:commFd;
        }
#endif
   
        if(eponMonitorFd > 0)
        {    
		    FD_SET(eponMonitorFd, &readFds);
		    max_fd = (max_fd > eponMonitorFd)? max_fd:eponMonitorFd;
        }

        if(netlinkRouteFd > 0)
        {    
            FD_SET(netlinkRouteFd, &readFds);
            max_fd = (max_fd > netlinkRouteFd)? max_fd:netlinkRouteFd;
        }
        
        if(netlinkSnoopFd > 0)
        {    
            FD_SET(netlinkSnoopFd, &readFds);
            max_fd = (max_fd > netlinkSnoopFd)? max_fd:netlinkSnoopFd;
        }

#ifdef DPOE_OAM_BUILD
        if(eapolFd > 0)
        {    
		    FD_SET(eapolFd, &readFds);
		    max_fd = (max_fd > eapolFd)? max_fd : eapolFd;
        }
#endif
        if(OamCmdsServerFd > 0)
        {    
		    FD_SET(OamCmdsServerFd, &readFds);
		    max_fd = (max_fd > OamCmdsServerFd)? max_fd : OamCmdsServerFd;
        }

        if(OamCmdsDataFd > 0)
        {    
		    FD_SET(OamCmdsDataFd, &readFds);
		    max_fd = (max_fd > OamCmdsDataFd)? max_fd : OamCmdsDataFd;
        }

        tv.tv_sec = EPONSDK_PERIODIC_TASK_INTERVAL;
        tv.tv_usec = 0;

		rc = select(max_fd+1, &readFds, NULL, NULL, &tv);
		if (rc > 0) 
		{
#ifdef BRCM_CMS_BUILD
			if(FD_ISSET(commFd, &readFds))
			{
				 CmsMsgHeader *pMsg;
				 if ((cmsRet = cmsMsg_receiveWithTimeout(msgHandle, &pMsg, 100))== CMSRET_SUCCESS)
				 {
				     TkDataProcessHandle_CmsMsg(pMsg);
				     CMSMEM_FREE_BUF_AND_NULL_PTR(pMsg);
				 }
			}
#endif

			if(FD_ISSET(fd, &readFds))
			{
				 rxStatus = TkOsDataRx(pSrcMsgBuf->buff, &len);
				 if(!rxStatus)
				 {
				     continue;
				 }

				 if(NULL != TkRxDataDispatchHandle)
				 {
				     TkRxDataDispatchHandle(pSrcMsgBuf, pResMsgBuf, len);
				 }
			}

            if(FD_ISSET(eponMonitorFd, &readFds))
            {
                TkDataProcessHandle_NetlinkMsg(eponMonitorFd, pSrcMsgBuf,TkDataProcessHandle_NetlinkEponMsg );
            }

            if(FD_ISSET(netlinkRouteFd, &readFds))
            {
                TkDataProcessHandle_NetlinkMsg(netlinkRouteFd, pSrcMsgBuf,TkDataProcessHandle_NetlinkRouteMsg );
            }
#if defined(DMP_X_BROADCOM_COM_IGMPSNOOP_1) || defined(DMP_X_BROADCOM_COM_MLDSNOOP_1)
            if(FD_ISSET(netlinkSnoopFd, &readFds))
            {
                snoop_netlink_recv_mesg();
            }
#endif
#ifdef DPOE_OAM_BUILD
            if(eapolFd > 0 && FD_ISSET(eapolFd, &readFds))
            {
                rxStatus = EapolDataRx(pSrcMsgBuf->buff, &len);
                if(!rxStatus)
                {
                    continue;
                }
                else
                {
                    EapolHandle(pSrcMsgBuf, pResMsgBuf, len);
                }
            }
#endif
            if(FD_ISSET(OamCmdsServerFd, &readFds))
            {
                OamCmdsDataFd = OamCmdsHandleConnection(OamCmdsServerFd);
            }

            if (OamCmdsDataFd > 0)
            {
                if (FD_ISSET(OamCmdsDataFd, &readFds))
                {
                    OamCmdsHandleMsg(OamCmdsDataFd);
                    close(OamCmdsDataFd);
                    OamCmdsDataFd = CMS_INVALID_FD;
                }
            }

            TkOsTimerCheckExpiry(&readFds);
            OntMgrPollTimer();
            OsAstMsgQPoll(); 
        }
    }
    TkOamMemPut ((void *)pSrcMsgBuf);
    TkOamMemPut ((void *)pResMsgBuf); 
}


BOOL TkOsDataRxTxInit(void)
{
#ifdef DESKTOP_LINUX
    struct sockaddr_un local_addr;
    U32 addr_len = 0;
    
    #define OAM_MESSAGE_SERVER  "oam_messaging_server_addr"
    
    remove(OAM_MESSAGE_SERVER);
    
    if ( (fd = socket(PF_LOCAL, SOCK_RAW, 0) ) < 0 )
    {
        cmsLog_error( "failed to create local socket for desktop!" );
        return FALSE;
    }
    
    local_addr.sun_family = AF_LOCAL;
    strcpy(local_addr.sun_path, OAM_MESSAGE_SERVER);
    
    addr_len = strlen(OAM_MESSAGE_SERVER)+offsetof(struct sockaddr_un, sun_path) + 1;
    if((bind(fd, (struct sockaddr *)&local_addr, addr_len)== -1))
    {
        cmsLog_error("Error binding local socket for desktop to interface");
        exit(-1);
    }
    
    cmsLog_setLevel(LOG_LEVEL_DEBUG);
#else
    struct ifreq req;
    struct sockaddr_ll sll;

    if ( (fd = socket(AF_PACKET, SOCK_RAW, htons( TK_CAP_PKT_TYPE ) ) ) < 0 )
    {
        cmsLog_error( "failed to create raw socket!" );
        return FALSE;
    }

    strcpy( req.ifr_name, EPON_OAM_IF_NAME );

    if ( ioctl( fd, SIOCGIFINDEX, &req ) < 0 )
    {
        cmsLog_error( "failed to get IF index!" );
        return FALSE;
    }

    if_index = req.ifr_ifindex;

    bzero(&sll, sizeof(sll));

    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = req.ifr_ifindex;
    sll.sll_protocol = htons( TK_CAP_PKT_TYPE ); 

    if((bind(fd, (struct sockaddr *)&sll, sizeof(sll)))== -1)
    {
        cmsLog_error("Error binding raw socket to interface");
        exit(-1);
    }
#endif	
    return TRUE;
}

BOOL TkOsDataTx(U8 *buf, U16 len )
{
#ifndef DESKTOP_LINUX
    struct sockaddr_ll link;

    link.sll_family = AF_PACKET;
    link.sll_pkttype = PACKET_OUTGOING;
    link.sll_ifindex = if_index;
    link.sll_halen = 6;
            
    memcpy( link.sll_addr, buf, link.sll_halen );
    
    if ( sendto( fd, buf, len, 0, (struct sockaddr *)&link, sizeof(link) ) < 0 )
    {
        cmsLog_error( "failed to send to RAW socket!" );
                    
        return FALSE;
    }
    return TRUE;
#else
    struct sockaddr_un local_addr;
    int addr_len = 0, error;
    	
    #define OAM_MESSAGE_CLIENT  "oam_messaging_client_addr"
    
    local_addr.sun_family = AF_LOCAL;
    strcpy(local_addr.sun_path, OAM_MESSAGE_CLIENT);
    addr_len = strlen(OAM_MESSAGE_CLIENT)+offsetof(struct sockaddr_un, sun_path) + 1;

	error = sendto(fd, buf, len, 0, (struct sockaddr *)&local_addr, addr_len);
    if(error < 0)
    {
        printf("Fail to send to desktop client, errno is %d\n", error);
        return FALSE;
    }
    return TRUE;
#endif
}
    
static BOOL TkOsDataRx(U8 *buf, U16 *len )
{
    S32 length = 0;
    
    length = recvfrom(fd, buf, TK_MAX_RX_TX_DATA_LENGTH, 0, NULL, NULL );
    if ( length < 0 )
    {
        cmsLog_error("recv data error");
        return FALSE;
    }   
    else
    {
        *len = length;
        return TRUE;
    }                           
}

BOOL TkOsDataRxThreadInit(BOOL (*pFunc)(TkMsgBuff *pSrcMsg, TkMsgBuff *pResMsg, U16 len ))
{   
    if(NULL != pFunc)
    {
        TkRxDataDispatchHandle = pFunc;
    }

    RxDataThreadId = TkOsThreadCreate( 
                        110, 
                        TkDataRxThread, 
                        0, 
                        0, 
                        (U8 *)"TkDataRxThread",
                        0,0,0,0,0);
    return TRUE;
}

BOOL TkOsDataRxTxRelease(void)
{
    return TRUE;
}

void TkOsGetIfMac(U8 link, U8 *mac)
{
    MacAddr oamMac;
    U16 macLow16;
    
    memcpy(&oamMac, &eponDevInfo.resbaseEponMac, sizeof(MacAddr) );
    macLow16 = OAM_NTOHS(oamMac.word[2]);
    macLow16 += link;
    oamMac.word[2] = OAM_HTONS(macLow16);
    memcpy( mac, &oamMac, sizeof(oamMac) );
    return ;
}


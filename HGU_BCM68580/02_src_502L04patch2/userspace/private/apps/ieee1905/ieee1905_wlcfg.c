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
 * IEEE1905 WLCFG AUTO CONFIGURATION 
 */
#if defined(WIRELESS)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/time.h>
#include "i5ctl.h"
#include "i5ctl_wlcfg.h"
#include "ieee1905_wlcfg.h"
#include "ieee1905_trace.h"
#include "ieee1905_wlmetric.h"
#include "ieee1905_message.h"
#include "ieee1905_datamodel.h"
#include "ieee1905_glue.h"
#include "ieee1905_tlv.h"
#include "ieee1905_interface.h"
#include <wlcsm_defs.h>
#if defined(DSLCPE_WLCSM_EXT)
#include <stdarg.h>
#include <wlcsm_lib_api.h>
#endif
#include <proto/wps.h>
#include <wps_1905.h>

#define WPS_1905_SOCKET_MAX_RECV_BUFFER_SIZE 4096
#define WPS_1905_RECEIVE_TIMEOUT_MS          3000
#define WPS_1905_RESTART_TIMEOUT_MS          15000

#define I5_TRACE_MODULE i5TraceWlcfg

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  wps1905ControlSocketReady
 *  Description:  check if connnection to wps monitor is available
 * =====================================================================================
 */
static int wps1905ControlSocketReady( void )
{
    return (i5_config.wl_control_socket.ptmr ? 0 : 1);
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  wps1905InitMessage
 *  Description:  Init a WPS_1905_Message structure with data of len appended
 *   return:  WPS_1905_MESSAGE structure or NULL
 * =====================================================================================
 */
static WPS_1905_MESSAGE *wps1905InitMessage(char const *ifname,WPS_1905_CTL_CMD cmd,int len)
{
    WPS_1905_MESSAGE * pmsg=(WPS_1905_MESSAGE *)malloc(len+sizeof(WPS_1905_MESSAGE));
    if(pmsg)
    {
        if ( ifname )
        {
            strncpy(pmsg->ifName, ifname, I5_MAX_IFNAME-1);
            pmsg->ifName[I5_MAX_IFNAME-1] = '\0';
        }
        else
        {
            pmsg->ifName[0] = '\0';
        }
        pmsg->cmd=cmd;
        pmsg->len=len;
    }
    return pmsg;
}

static int wps1905SendMsg( i5_socket_type *psock, WPS_1905_MESSAGE *pMsg )
{
    struct sockaddr_in sockAddr;
    int                sendLen;
    int                uiLen;

    /* kernel address */
    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family      = AF_INET;
    sockAddr.sin_addr.s_addr = inet_addr(WPS_1905_ADDR);
    sockAddr.sin_port        = htons(WPS_1905_PORT);

    uiLen   = pMsg->len+sizeof(WPS_1905_MESSAGE);
    sendLen = sendto(psock->sd, pMsg, uiLen, 0, (struct sockaddr *)&sockAddr, sizeof(sockAddr));
    if (uiLen != sendLen)
    {
        printf("%s: sendto failed", __FUNCTION__);
        return -1;
    }

    return 0;
}

/* ===  FUNCTION  ======================================================================
*         Name:  wps1905Register
*  Description:  register with the wps monitor. Retry interval is
*                WPS_1905_RECEIVE_TIMEOUT_MS
*       return:
* =====================================================================================
*/
static void wps1905Register( void *arg )
{
    struct sockaddr_in  sockAddr;
    WPS_1905_MESSAGE   *pMsg;
    controlSockStruct  *pctrlsock = (controlSockStruct*)arg;
    socklen_t           addrLen = sizeof(sockAddr);

    i5TimerFree(pctrlsock->ptmr);
    pctrlsock->ptmr = NULL;

    memset(&sockAddr, 0, sizeof(sockAddr));
    if ( getsockname(pctrlsock->psock->sd, (struct sockaddr *)&sockAddr, &addrLen) < 0)
    {
        i5TraceError("getsockname failed\n");
    }
    else
    {
        unsigned short portNo = ntohs(sockAddr.sin_port);
        i5Trace("Registering UDP port %d with WPS\n", portNo);
        pMsg = wps1905InitMessage(NULL, WPS_1905_CTL_CLIENT_REGISTER, sizeof(unsigned short));
        if(pMsg!=NULL)
        {
            memcpy(WPS1905MSG_DATAPTR(pMsg),&portNo, sizeof(unsigned short));
            wps1905SendMsg(pctrlsock->psock, pMsg);
            free(pMsg);
        }
    }

    pctrlsock->ptmr = i5TimerNew(WPS_1905_RECEIVE_TIMEOUT_MS, wps1905Register, pctrlsock);

}

static WPS_1905_MESSAGE *wps1905ReceiveMsg(i5_socket_type *psock, unsigned char *pBuf, unsigned int maxLen)
{
    int                  recvlen;
    struct sockaddr      src;
    unsigned int         addrlen = sizeof(struct sockaddr);

    recvlen = recvfrom(psock->sd, pBuf, maxLen, 0, &src, &addrlen);
    if (recvlen < 0)
    {
        i5TraceInfo("wps1905ReceiveMsg: receive error (errno=%d, %s)\n", errno, strerror(errno));
        /* likely error is a timeout - try to register with kernel again */
        if ( i5_config.wl_control_socket.ptmr != NULL ) {
          i5TimerFree(i5_config.wl_control_socket.ptmr);
        }
        i5_config.wl_control_socket.ptmr = i5TimerNew(0, wps1905Register, &i5_config.wl_control_socket);
        return NULL;
    }
    else if (recvlen < sizeof(WPS_1905_MESSAGE))
    {
        printf("wps1905ReceiveMsg: invalid receive length\n");
        return NULL;
    }

    return (WPS_1905_MESSAGE *)pBuf;

}

static int wps1905ProcessUnsolicited(i5_socket_type *psock, WPS_1905_MESSAGE *pMsg)
{
    int messageProcessed = 0;
    switch ( pMsg->cmd ) {
        case WPS_1905_CTL_CLIENT_REGISTER:
        {
            i5_dm_device_type *pdevice;

            if ( i5_config.wl_control_socket.ptmr ) {
              i5TimerFree(i5_config.wl_control_socket.ptmr);
              i5_config.wl_control_socket.ptmr = NULL;
            }
  
            i5Trace("WPS_1905_CTL_CLIENT_REGISTER received\n");

            pdevice = i5DmDeviceFind(&i5_config.i5_mac_address[0]);
            if ( pdevice != NULL )
            {
                i5_dm_interface_type *pinterface = pdevice->interface_list.ll.next;
                while (pinterface != NULL)
                {
                    if ( i5DmIsInterfaceWireless(pinterface->MediaType) )
                    {
                        i5Trace("wlparent %s, Renew %d, registrar %d, wsc %d, change %d, mediaType %x\n", 
                               pinterface->wlParentName, pinterface->isRenewPending, i5_config.isRegistrar,
                               pinterface->confStatus, pinterface->credChanged, pinterface->MediaType);

                        if ( (1 == pinterface->isRenewPending) && 
                             (WPS_1905_CONF_NOCHANGE_CONFIGURED == pinterface->confStatus) &&
                             (1 == i5_config.isRegistrar) &&
                             (1 == pinterface->credChanged) )
                        {
                            unsigned int freqBand = i5TlvGetFreqBandFromMediaType(pinterface->MediaType);
                            i5Trace("sending AP renew - band %d\n", freqBand);
                            i5MessageApAutoconfigurationRenewSend(freqBand);
                        }
                        pinterface->isRenewPending = 0;
                    }
                    pinterface = pinterface->ll.next;
                }
            }

            /* check to see if any interfaces need configuration */
            i5WlcfgApAutoconfigurationStart( NULL );
            messageProcessed = 1;
            break;
        }

        case WPS_1905_NOTIFY_CLIENT_RESTART:
        {
            WPS_1905_NOTIFY_MESSAGE *msg=(WPS_1905_NOTIFY_MESSAGE *)(pMsg+1);
            i5_dm_device_type *pdevice;
            i5_dm_interface_type *pinterface;

            i5Trace("WPS_1905_NOTIFY_CLIENT_RESTART received\n");

            pdevice = i5DmDeviceFind(&i5_config.i5_mac_address[0]);
            if ( pdevice != NULL )
            {
                pinterface = pdevice->interface_list.ll.next;
                while (pinterface != NULL)
                {
                    if ( i5DmIsInterfaceWireless(pinterface->MediaType) &&
                         (0 == strcmp(msg->ifName, pinterface->wlParentName)) )
                    {
                        pinterface->isRenewPending = 1;
                        pinterface->credChanged    = msg->credentialChanged;
                        pinterface->confStatus     = msg->confStatus;
                        if ( i5_config.wl_control_socket.ptmr != NULL ) {
                            i5TimerFree(i5_config.wl_control_socket.ptmr);
                        }
                        i5_config.wl_control_socket.ptmr = i5TimerNew(WPS_1905_RESTART_TIMEOUT_MS, wps1905Register, &i5_config.wl_control_socket);
                    }
                    pinterface = pinterface->ll.next;
                }
            }
            messageProcessed = 1;
            break;
        }

        default:
            break;
    }
    return messageProcessed;
}

static void wps1905ProcessSocket(i5_socket_type *psock)
{
    unsigned char     buffer[WPS_1905_SOCKET_MAX_RECV_BUFFER_SIZE];
    WPS_1905_MESSAGE *pMsg=NULL;
    int               ret;

    memset(buffer, 0, WPS_1905_SOCKET_MAX_RECV_BUFFER_SIZE);
    pMsg = wps1905ReceiveMsg(psock, buffer, WPS_1905_SOCKET_MAX_RECV_BUFFER_SIZE );
    
    if (pMsg == NULL) {
        i5TraceInfo("wps1905ReceiveMsg returned NULL\n");
        return;
    }   
    
    ret  = wps1905ProcessUnsolicited(psock, pMsg);
    if ( 0 == ret )
    {
        i5TraceInfo("Unexpected unsolicited wl message received: cmd %d\n", (int)pMsg->cmd);
    }
    else
    {
        i5TraceInfo("Message cmd %d, len %d, status %d\n", (int)pMsg->cmd, (int)pMsg->len, (int)pMsg->status);
    }
}

static WPS_1905_MESSAGE *wps1905GetResponse(i5_socket_type *psock, int cmdExpected, unsigned char *pBuf, int maxLen)
{
    WPS_1905_MESSAGE *pMsg=NULL;
    int               ret;
    struct timespec   ts;
    struct timeval    end_tv;
    struct timeval    now_tv;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    end_tv.tv_sec  = ts.tv_sec + (WPS_1905_RECEIVE_TIMEOUT_MS / 1000);
    end_tv.tv_usec = ts.tv_nsec/1000 + ((WPS_1905_RECEIVE_TIMEOUT_MS % 1000) * 1000);

    while ( 1 )
    {
        pMsg = wps1905ReceiveMsg(psock, pBuf, maxLen );
        if ( NULL == pMsg )
        {
            return NULL;
        }
        ret  = wps1905ProcessUnsolicited(psock, pMsg);
        if ( 0 == ret )
        {
            if ( (cmdExpected == pMsg->cmd) ||
                    ((cmdExpected == WPS_1905_CTL_WSCPROCESS) && (WPS_1905_CTL_GETM2 == pMsg->cmd)) ||
                    ((cmdExpected == WPS_1905_CTL_WSCPROCESS) && (WPS_1905_CTL_CONFIGUREAP == pMsg->cmd)) )
            {
                break;
            }
            else
            {
                i5Trace("Unexpected wl message received: cmd %d, exp %d\n", (int)pMsg->cmd, cmdExpected);
            }
        }

        clock_gettime(CLOCK_MONOTONIC, &ts);
        now_tv.tv_sec  = ts.tv_sec;
        now_tv.tv_usec = ts.tv_nsec/1000;
        if (timercmp(&end_tv, &now_tv, <))
        {
            i5TraceInfo("Timeout waiting for response\n");
            if ( i5_config.wl_control_socket.ptmr != NULL ) {
              i5TimerFree(i5_config.wl_control_socket.ptmr);
            }
            i5_config.wl_control_socket.ptmr = i5TimerNew(WPS_1905_RECEIVE_TIMEOUT_MS, wps1905Register, &i5_config.wl_control_socket);
            return NULL;
        }
    }

    i5Trace("Message cmd %d, len %d, status %d\n", (int)pMsg->cmd, (int)pMsg->len, (int)pMsg->status);
    return pMsg;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  wps1905SendRequest
 *  Description:  Send specific request to WPS daemon with content.
 *   Parameters:  ifname- wirelss interface name
 *         cmd-   command of request
 *         rmsg-   content of the requst
 *         len-   the length of content.
 * =====================================================================================
 */
static int wps1905SendRequest(i5_socket_type *psock,char const *ifname,WPS_1905_CTL_CMD cmd, unsigned char *rmsg,unsigned int len)
{
    WPS_1905_MESSAGE *pMsg=NULL;

    i5Trace("sending request - cmd %d\n", cmd);

    /* if command has no message, can use NULL as rmsg,len is ignored then */
    if(wps1905ControlSocketReady())
    {
        if(rmsg==NULL)
            pMsg=wps1905InitMessage(ifname,cmd,0);
        else
        {
            pMsg=wps1905InitMessage(ifname,cmd,len);
            if(pMsg!=NULL)
            {
                memcpy(WPS1905MSG_DATAPTR(pMsg),rmsg,len);
            }
            else
            {
                return -1;
            }
        }
        wps1905SendMsg(psock, pMsg);
        free(pMsg);
    }
    else
    {
        return -1;
    }
    return 0;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  wps1905Commander
 *  Description:  sending WPS request and expect matching repsonse.
 *   Parameters:  ifname- wireles interface name
 *         cmd-   command of request
 *         msg-   content of the requst
 *         len-   the length of content.
 *         respmsg- the response content
 *         len-   the length of the return content
 *       return:  return command execution status
 * =====================================================================================
 */
static int wps1905Commander(char const *ifname,WPS_1905_CTL_CMD cmd,unsigned char *msg, int len,unsigned char **respmsg,int *rlen)
{
    WPS_1905_MESSAGE *pRespMsg=NULL;
    int status=-1;

    *rlen = 0;
    if ( 0 == wps1905ControlSocketReady() )
    {
        return status;
    }

    if(0 == wps1905SendRequest(i5_config.wl_control_socket.psock, ifname, cmd, msg, len))
    {
        unsigned char  buffer[WPS_1905_SOCKET_MAX_RECV_BUFFER_SIZE];

        memset(buffer, 0, WPS_1905_SOCKET_MAX_RECV_BUFFER_SIZE);
        pRespMsg = wps1905GetResponse(i5_config.wl_control_socket.psock, cmd, buffer, WPS_1905_SOCKET_MAX_RECV_BUFFER_SIZE);
        if ( NULL == pRespMsg )
        {
            return status;
        }
        if ( pRespMsg->len > 0 )
        {
            unsigned char *retmsg=malloc(pRespMsg->len);
            if (retmsg)
            {
                memcpy(retmsg,(void *)(pRespMsg+1),pRespMsg->len);
                *respmsg=retmsg;
                *rlen=pRespMsg->len;
            }
        }
        status = pRespMsg->status;
    }
    else
    {
        printf("%s: %d  error sending message to WPS \n",__FUNCTION__,__LINE__ );
    }
    return status;
}

/*-----------------------------------------------------------------------------
 *
 *  The following functions are Wireless Autoconfiguraiton APIs which should be
 *  called in 1905 process. For Unit testing, i5ctl commands are developed to
 *  use these functions for verification
 *-----------------------------------------------------------------------------*/
/*
 * ===  FUNCTION  ======================================================================
 *         Name:  wps1905WlInstance
 *  Description:  get numbers of wirelesss interfaces on board
 *       return:  <0 error, likely WPS is not running or did not get response from WPS in 10 seconds
 *           >=0 number of wireless interfaces.
 * =====================================================================================
 */
static int wps1905WlInstance()
{
    unsigned char* retmsg=NULL;
    int length=0;
    int status=wps1905Commander(NULL,WPS_1905_CTL_WLINSTANCE,NULL,0,&retmsg,&length);
    if(retmsg)
        free(retmsg);
    return status;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  wps1905WscStatus
 *  Description:  tell what is current WSC s/atus
 *       return:  <0 error, likely WPS is not running or did not get response from WPS in 10 seconds
 *           WPS_1905_WSCAP_CONFIGURED: WPS enabled, WSC AP is configured by either M2
 *                      manually set.
 *           WPS_1905_WSCAP_UNCONFIGURED: WPS enabled, WSC AP is not configured.
 *           WPS_1905_WSCAP_SESSIONONGOING: There is WPS session is on going
 * =====================================================================================
 */
static int wps1905WscStatus( )
{
    unsigned char* retmsg=NULL;
    int length=0;
    int status=wps1905Commander(NULL,WPS_1905_CTL_WSCSTATUS,NULL,0,&retmsg,&length);
    if(retmsg)
        free(retmsg);
    return status;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  wps1905WscEnabled
 *  Description:  check if WSC is enabled for the given inteface
 *       return:  <0 error, likely WPS is not running or time out occurred
 *           WPS_1905_WSCAP_ENABLED
 *           WPS_1905_WSCAP_DISABLED
 * =====================================================================================
 */
static int wps1905WscEnabled(char const *ifname)
{
    unsigned char* retmsg=NULL;
    int length=0;
    int status=wps1905Commander(ifname,WPS_1905_CTL_WSCENABLED,NULL,0,&retmsg,&length);
    if(retmsg)
        free(retmsg);
    return status;
}

/* ===  FUNCTION  ======================================================================
*         Name:  wps1905StopApAutoConf
*  Description:  action to stop AP autoconfiguration
*       return:  <0 error, likely WPS is not running or did not get response from WPS in 10 seconds
*           1: successful
*           0: failure
* =====================================================================================
*/
static int wps1905StopApAutoConf( )
{
    unsigned char* retmsg=NULL;
    int length=0;
    int status=wps1905Commander(NULL,WPS_1905_CTL_WSCCANCEL,NULL,0,&retmsg,&length);
    if(retmsg)
        free(retmsg);
    return status;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  wps1905GetMessageInfo
 *  Description:  This function is used to get the message type of a received WSC message
 *   Parameters:
 *         msg-    incoming 1905 WSC message
 *         len-    incoming message length
 *
 *       return:  error or message type

 * =====================================================================================
 */
static int wps1905GetMessageInfo(unsigned char *msg, unsigned int len, int *pmtype, int *prfband)
{
    WPS_1905_M_MESSAGE_INFO *pminfo=NULL;
    int length=0;
    int status=wps1905Commander(NULL,WPS_1905_CTL_GETMINFO,msg,len,(unsigned char **)&pminfo,&length);

    if ( (pminfo == NULL ) || (length != sizeof(WPS_1905_M_MESSAGE_INFO)) || (status != 0) )
    {
        return -1;
    }

    *pmtype  = pminfo->mtype;
    *prfband = pminfo->rfband;
    free(pminfo);
    return 0;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  wps1905WscProcess
 *  Description:  This function is used to handle all 1905 WSC messages,now we expect
 *        this message to be either M1 or M2 message,in standard, it is not clear
 *        it is in EAP format or not, but for now,we will handle only bare M1/M2
 *        message without EAP header(which is generated from other APIs).We can
 *        change it later with clarification for this.
 *   Parameters:  ifname-   wireless interface name
 *         msg-    incoming 1905 WSC message
 *         len-    incoming message length
 *         retmsg-  pointer to a possible returned message [remember to release
 *             if it is not NULL in your caller]
 *         length-   returned message length
 *
 *       return:  <0 error, likely WPS is not running or did not get response from WPS in 10 seconds
 *         For M2 mesage:
 *           WPS_1905_M2SET_DONE:    Successfully set AP.
 *           WPS_1905_M2SET_NOMATCHING:  M2 information does not match to M1
 *           WPS_1905_M2SET_NOSESSION:   There is no session to use this M2 information
 *           WPS_1905_M2SET_NOAUTHENTICATOR:  M2 message does not have Authentication info
 *
 *           For M1 Message:
 *             WPS_1905_M1HANDLE_M2DATA:   successful get M2 information
 *           WPS_1905_M1HANDLE_NOTREGISRAR : CPE can not act as Registrar.
 *
 *           For UNKNOW Message:
 *             1)WPS_1905_UNKNOWNWSCMESSAGE.
 *
 *         other: error happen in WPS.
 * =====================================================================================
 */
static int wps1905WscProcess(char const *ifname,int cmd,unsigned char *msg,unsigned int len,unsigned char **retmsg, int *length)
{
    int status=wps1905Commander(ifname,cmd,msg,len,retmsg,length);
    return status;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  _i5WlcfgCtlResponse
 *  Description:  internal function to send response to i5ctl command line.
 * =====================================================================================
 */
static void _i5WlcfgCtlResponse(i5_socket_type *psock, t_I5_API_CMD_NAME cmd, char subcmd, int status, char *buf, int len)
{
    char               *pMsgBuf;
    int                 retMsgLen;
    t_I5_API_WLCFG_MSG *pRetMsg;

    if((buf!=NULL)&&len==0)
        len=strlen(buf)+1; /*  this is for for sending string case */
    retMsgLen = len + sizeof(t_I5_API_WLCFG_MSG);
    pMsgBuf = malloc(retMsgLen);
    if((buf!=NULL) && (len>0))
    {
        memcpy(pMsgBuf + sizeof(t_I5_API_WLCFG_MSG), buf, len);
    }
    pRetMsg = (t_I5_API_WLCFG_MSG *)pMsgBuf;
    pRetMsg->subcmd = subcmd;
    pRetMsg->status = status;
    i5apiSendMessage(psock->sd, cmd, pMsgBuf, retMsgLen);
    free(pMsgBuf);
}

unsigned int i5WlCfgAreMediaTypesCompatible(unsigned short mediaType1, unsigned short mediaType2)
{
  if (mediaType1 == mediaType2) {
    return 1;
  }
  else if ((mediaType1 == I5_MEDIA_TYPE_WIFI_N24) &&
           ((mediaType2 == I5_MEDIA_TYPE_WIFI_A) || (mediaType2 == I5_MEDIA_TYPE_WIFI_B) || (mediaType2 == I5_MEDIA_TYPE_WIFI_G))
          ) {
    return 1;
  } 
  else if ((mediaType2 == I5_MEDIA_TYPE_WIFI_N24) &&
           ((mediaType1 == I5_MEDIA_TYPE_WIFI_A) || (mediaType1 == I5_MEDIA_TYPE_WIFI_B) || (mediaType1 == I5_MEDIA_TYPE_WIFI_G))
          ) {
    return 1;
  } 
  else if (((mediaType2 == I5_MEDIA_TYPE_WIFI_N5) && (mediaType1 == I5_MEDIA_TYPE_WIFI_AC)) || 
             ((mediaType1 == I5_MEDIA_TYPE_WIFI_N5) && (mediaType2 == I5_MEDIA_TYPE_WIFI_AC)) 
            ) {
    return 1;
  }
  return 0;            
}

static unsigned short i5WlCfgFetchWirelessNBand(char const *ifname)
{
  char cmd[64];
  char line[64];
  FILE* fp = NULL;

  sprintf(cmd, "nvram get %s_nband > /tmp/ieee1905file", ifname);
  system(cmd);
  
  fp = fopen("/tmp/ieee1905file","r");
  if (fp == NULL) {
    i5Trace("No such file\n");
    return -1;
  }
  
  if (fgets(line, sizeof(line)-1, fp) != NULL) {
    fclose(fp);
    i5TraceInfo("nband: %s", line);
    switch(line[0]) {
      case '2':
        return I5_MEDIA_TYPE_WIFI_N24;
      case '1':
        return I5_MEDIA_TYPE_WIFI_N5;
      default:
        return I5_MEDIA_TYPE_UNKNOWN;
    }
  }
  
  fclose(fp);
  return I5_MEDIA_TYPE_UNKNOWN;
}

/* returns : media_type on success,
 *         : I5_MEDIA_TYPE_UNKNOWN on failure
 */
unsigned short i5WlCfgFetchWirelessIfInfo(char const *ifname, unsigned char *pMediaInfo, int *pMediaLen,
                                          unsigned char *netTechOui, unsigned char *netTechVariant, unsigned char *netTechName, unsigned char *url, int sizeUrl)
{
  char cmd[64];
  char line[64];
  FILE* fp = NULL;
  const char *wlName;
  char wlParentIf[I5_MAX_IFNAME];

  if ( pMediaInfo ) {
    if ( *pMediaLen < i5TlvMediaSpecificInfoWiFi_Length ) {
      i5TraceError("invalid media info length\n");
      return I5_MEDIA_TYPE_UNKNOWN;
    }
    else {
      /* all zero media info for now */
      memset(pMediaInfo, 0, i5TlvMediaSpecificInfoWiFi_Length);
      *pMediaLen = i5TlvMediaSpecificInfoWiFi_Length;
    }
  }

  if ( 0 == strncmp(I5_GLUE_WLCFG_WDS_NAME_STRING, ifname, strlen(I5_GLUE_WLCFG_WL_NAME_STRING)) ) {
    strcpy(&wlParentIf[0], I5_GLUE_WLCFG_WL_NAME_STRING);
    wlParentIf[strlen(I5_GLUE_WLCFG_WL_NAME_STRING)] = ifname[strlen(I5_GLUE_WLCFG_WDS_NAME_STRING)];
    wlParentIf[strlen(I5_GLUE_WLCFG_WL_NAME_STRING) + 1] = '\0';
    wlName = wlParentIf;
  }
  else {
    wlName = (char *)ifname;
  }
  sprintf(cmd, "nvram get %s_phytype > /tmp/ieee1905file", wlName);
  system(cmd);
  
  fp = fopen("/tmp/ieee1905file","r");
  if (fp == NULL) {
    i5Trace("No such file\n");
    return -1;
  }

  if (fgets(line, sizeof(line)-1, fp) != NULL) {
    fclose(fp);
    i5TraceInfo("phytype: %s", line);
    if (strncmp(line,WL_PHY_TYPE_A,1) == 0) {
      return I5_MEDIA_TYPE_WIFI_A;
    } 
    else if (strncmp(line,WL_PHY_TYPE_B,1) == 0) {
      return I5_MEDIA_TYPE_WIFI_B;
    } 
    else if (strncmp(line,WL_PHY_TYPE_G,1) == 0) {
      return I5_MEDIA_TYPE_WIFI_G;
    } 
    else if ( (strncmp(line,WL_PHY_TYPE_N,1) == 0) ||
              (strncmp(line,WL_PHY_TYPE_H,1) == 0) ||
              (strncmp(line,WL_PHY_TYPE_LP,1) == 0) ) {
      return i5WlCfgFetchWirelessNBand(wlName);
    } 
    else if (strncmp(line,WL_PHY_TYPE_AC,1) == 0) {
      return I5_MEDIA_TYPE_WIFI_AC;
    }
 /* The codes for AD and AF, supported by 1905, are not supported in wldefs.h
    } else if (strncmp(line,WL_PHY_TYPE_AD,1)) {
      return I5_MEDIA_TYPE_WIFI_AD;
    } else if (strncmp(line,WL_PHY_TYPE_AF,1)) {
        return I5_MEDIA_TYPE_WIFI_AF;
    } */
    return I5_MEDIA_TYPE_UNKNOWN;
  }
  fclose(fp);
  return I5_MEDIA_TYPE_UNKNOWN;
}

char *i5WlcfgGetWlParentInterface(char const *ifname, char *wlParentIf)
{
  /* if ifname matches the wds string then we want to return the wl name string plus the wlindex 
     (first number after name string) - ifname = wdsx.y, return wlx */
  if ( 0 == strncmp(I5_GLUE_WLCFG_WDS_NAME_STRING, ifname, strlen(I5_GLUE_WLCFG_WDS_NAME_STRING)) ) {
    snprintf(wlParentIf, I5_MAX_IFNAME, "%s%c", I5_GLUE_WLCFG_WL_NAME_STRING, ifname[strlen(I5_GLUE_WLCFG_WDS_NAME_STRING)]);
    return wlParentIf;
  }
  else {
    if ( 0 == strncmp(I5_GLUE_WLCFG_WL_NAME_STRING, ifname, strlen(I5_GLUE_WLCFG_WL_NAME_STRING)) ) {
      return (char *)ifname;
    }
    else {
      return NULL;
    }
  }
}

int i5WlCfgGetWdsMacFromName(char const *ifname, char *macString, int maxLen)
{
  char const *wdsIdxStr;
  int         wlIndex;
  int         wdsIndex;
  char       *endptr;

  /* format is <name><wlidx>.<wdsidx> */
  if ( 0 == strncmp(I5_GLUE_WLCFG_WDS_NAME_STRING, ifname, strlen(I5_GLUE_WLCFG_WDS_NAME_STRING)) ) {
    int i;
    char cmdStr[256] = "";
    
    /* find first digit in string */
    for ( i = 0; i < strlen(ifname); i++ )
    {
       if ( isdigit(ifname[i]) ) {
         break;
       }
    }

    if ( i == strlen(ifname) ) {
      return -1;
    }

    wdsIdxStr = strstr(ifname, ".");
    if ( NULL == wdsIdxStr ) {
      return -1;
    }

    errno = 0;
    wlIndex = strtol(&ifname[i], &endptr, 10);
    if ( (errno != 0) || (endptr == &ifname[i])) {
      return -1;
    }

    errno = 0;
    wdsIndex = strtol(&wdsIdxStr[1], &endptr, 10);
    if ( (errno != 0) || (endptr == wdsIdxStr)) {
      return -1;
    }

    snprintf(cmdStr, sizeof(cmdStr), "wl -i %s%d wds > /tmp/wlwds", I5_GLUE_WLCFG_WL_NAME_STRING, wlIndex);
    system(cmdStr);
    return i5InterfaceSearchFileForIndex("/tmp/wlwds", wdsIndex, macString, maxLen);
  }
  return -1;
}

unsigned int i5WlCfgIsApConfigured(char const *ifname)
{
    i5_dm_device_type *pdevice = i5DmDeviceFind(i5_config.i5_mac_address);

    if ( pdevice != NULL )
    {
        if (i5_config.isRegistrar)
        {
            return 1;
        }
        else
        {
            i5_dm_interface_type *pinterface = pdevice->interface_list.ll.next;
            while ( pinterface )
            {
                if ( 0 == strcmp(pinterface->wlParentName, ifname) )
                {
                    if ( pinterface->isConfigured )
                    {
                        return 1;
                    }
                    break;
                }
                pinterface = pinterface->ll.next;
            }
        }
    }
    return 0;
}

void i5WlcfgApAutoConfigTimer(void * arg)
{   
    apSearchSendStruct *pApSearch = (apSearchSendStruct *)arg;
    apSearchEntry      *pentry;
    int                 timeout;

    i5Trace("AP timeout occurred\n");

    if ( pApSearch->timer != NULL )
    {
        i5TimerFree(pApSearch->timer);
        pApSearch->timer = NULL;
    }

    if ( NULL == pApSearch->searchEntryList.ll.next )
    {
        i5TraceInfo("no entries - cancel timer\n");
        return;
    }

    if ( 0 == wps1905ControlSocketReady() )
    {
        pApSearch->timer = i5TimerNew(I5_MESSAGE_AP_SEARCH_NOT_READY_INTERVAL_MSEC, i5WlcfgApAutoConfigTimer, pApSearch);
        return;
    }

    /* a timeout has occurred or one of the entries is finished,
       move to the next search entry, send the search message
       for the entry, and start the timer */
    pentry = pApSearch->activeSearchEntry;
    if ( pentry != NULL )
    {
        pentry = pentry->ll.next;
    }

    if ( pentry == NULL )
    {
        pentry = pApSearch->searchEntryList.ll.next;
    }

    timeout = I5_MESSAGE_AP_SEARCH_NOT_READY_INTERVAL_MSEC;
    pApSearch->activeSearchEntry = pentry;
    pentry->callCounter++;
    do 
    {
        if ( 0 == pentry->renew )
        {
            if ( pentry->callCounter < I5_MESSAGE_AP_SEARCH_START_COUNT )
            {
                pentry->expiryTime = I5_MESSAGE_AP_SEARCH_START_INTERVAL_MSEC;
            }
            else
            {
                pentry->expiryTime = I5_MESSAGE_AP_SEARCH_PERIODIC_INTERVAL_MSEC;
            }
            timeout = pentry->expiryTime;
            i5MessageApAutoconfigurationSearchSend(pentry->freqBand);
            pentry->expectedMsg = i5MessageApAutoconfigurationResponseValue;
        }
        else
        {
            i5_socket_type *psock;
            int             rc;
            unsigned char  *pData;
            int             dataLen = 0;
  
            psock = i5SocketFindDevSocketByName(pentry->rxIfname);
            if ( NULL == psock )
            {
                i5TraceError("RX interface no longer available going to send search\n");
                pentry->renew = 0;
                pentry->callCounter = 0;
                continue;
            }
            else
            {
                /* clear active session */
                wps1905StopApAutoConf();
      
                /* Fetch M1 from Wireless */
                rc = wps1905WscProcess(pentry->ifname, WPS_1905_CTL_STARTAUTOCONF, NULL, 0, &pData, &dataLen);
                if (WPS_1905_RESHANDLE_M1DATA == rc)
                {
                    i5MessageApAutoconfigurationWscSend(psock, pentry->registrarMac, pData, dataLen);
                    pentry->expiryTime = I5_MESSAGE_AP_SEARCH_M1_M2_WAITING_MSEC;
                    timeout = pentry->expiryTime;
                }
                pentry->expectedMsg = i5MessageApAutoconfigurationWscValue;
                pentry->renew = 0;
                pentry->callCounter = I5_MESSAGE_AP_SEARCH_START_COUNT;
            }
        }
    } while( 0 );
    pApSearch->timer = i5TimerNew(timeout, i5WlcfgApAutoConfigTimer, pApSearch);
}

int i5WlcfgApAutoConfigProcessMessage( i5_message_type *pmsg, unsigned int freqband, unsigned char *pWscData, int wscDataLen )
{
    i5_dm_device_type  *pdevice = i5DmDeviceFind(i5_config.i5_mac_address);
    apSearchSendStruct *pApSearch = &i5_config.apSearch;
    int                 rc;
    int                 msgType;
    unsigned char      *pData;
    int                 dataLen;
    int                 matchingIf;
    char                ifname[I5_MAX_IFNAME];

    if ( (NULL == pmsg) || (NULL == pdevice) ) {
      return -1;
    }

    if (0 == wps1905ControlSocketReady())
    {
        i5WlcfgApAutoConfigTimer(pApSearch);
        return -1;
    }

    msgType = i5MessageTypeGet(pmsg);
    i5Trace(" msg=%d, reg %d\n", msgType, i5_config.isRegistrar);
    if ( 1 == i5_config.isRegistrar ) 
    {
        if ( WPS_1905_WSCAP_CONFIGURED == wps1905WscStatus(0) )
        {
            switch (msgType)
            {
                case i5MessageApAutoconfigurationSearchValue:
                {
                    if (0 == i5_config.freqBandEnable[freqband] )
                    {
                        return -1;
                    }

                    matchingIf = i5DmIsWifiBandSupported(&ifname[0], freqband);
                    /* AP autoconfiguration is enabled for the provided band
                       if matchingif is 0 then respond using a different band if available */
                    if ( 0 == matchingIf )
                    {
                        i5_dm_interface_type *pinterface = pdevice->interface_list.ll.next;
                        while ( pinterface )
                        {
                            if ( i5DmIsInterfaceWireless(pinterface->MediaType) &&
                                 i5WlCfgIsApConfigured(pinterface->wlParentName) &&
                                 (WPS_1905_WSCAP_ENABLED == wps1905WscEnabled(pinterface->wlParentName)))
                            {
                                freqband = i5TlvGetFreqBandFromMediaType(pinterface->MediaType);
                                break;
                            }
                            pinterface = pinterface->ll.next;
                        }
                        if ( NULL == pinterface )
                        {
                            return -1;
                        }
                    }
                    else if ( i5WlCfgIsApConfigured(ifname) )
                    {
                        if ( WPS_1905_WSCAP_ENABLED != wps1905WscEnabled(ifname) )
                        {
                            i5TraceInfo("WSC is not enabled on interface %s matching band %d\n", ifname, freqband);
                            return -1;
                        }
                    }
                    i5MessageApAutoconfigurationResponseSend(pmsg, freqband);
                    break;
                }

                case i5MessageApAutoconfigurationWscValue:
                {
                    int mtype;
                    int rfband;

                    rc = wps1905GetMessageInfo(pWscData, wscDataLen, &mtype, &rfband);
                    if ( (rc != 0) || (mtype != WPS_ID_MESSAGE_M1) ) {
                        return -1;
                    }

                    if ( (rfband < WPS_RFBAND_24GHZ) || (rfband > WPS_RFBAND_50GHZ) )
                    {
                        return -1;
                    }
                    /* convert rfband to i5MessageFreqBand */
                    rfband--;

                    if (0 == i5_config.freqBandEnable[rfband] )
                    {
                        return -1;
                    }

                    matchingIf = i5DmIsWifiBandSupported(&ifname[0], rfband);
                    if ( 0 == matchingIf )
                    {
                        i5_dm_interface_type *pinterface = pdevice->interface_list.ll.next;
                        while ( pinterface )
                        {
                            if ( i5DmIsInterfaceWireless(pinterface->MediaType) &&
                                 i5WlCfgIsApConfigured(pinterface->wlParentName) &&
                                 (WPS_1905_WSCAP_ENABLED == wps1905WscEnabled(pinterface->wlParentName)) )
                            {
                                rfband = i5TlvGetFreqBandFromMediaType(pinterface->MediaType);
                                strncpy(ifname, pinterface->wlParentName, I5_MAX_IFNAME);
                                ifname[I5_MAX_IFNAME-1] = '\0';
                                break;
                            }
                            pinterface = pinterface->ll.next;
                        }
                        if ( NULL == pinterface )
                        {
                            return -1;
                        }
                    }
                    else if (!i5WlCfgIsApConfigured(ifname))
                    {
                        i5TraceInfo("band not supported or wl not configured\n");
                        return -1;
                    }

                    if ( WPS_1905_WSCAP_ENABLED != wps1905WscEnabled(ifname) )
                    {
                        i5TraceInfo("WSC is not enabled on interface %s matching band %d\n", ifname, rfband);
                        return -1;
                    }

                    wps1905StopApAutoConf();
                    rc = wps1905WscProcess(ifname, WPS_1905_CTL_GETM2, pWscData, wscDataLen, &pData, &dataLen);
                    if ( rc != WPS_1905_M1HANDLE_M2DATA ) {
                        return -1;
                    }
                    i5MessageApAutoconfigurationWscSend(pmsg->psock, i5MessageSrcMacAddressGet(pmsg), pData, dataLen);
                    free(pData);
                    break;
                }

                default:
                    break;
            }
        }
    }
    else
    {
        apSearchEntry *pentry = pApSearch->activeSearchEntry;
        if ( (NULL == pentry) || (msgType != pentry->expectedMsg) )
        {
            /* unexpected message - ignore */
            return -1;
        }

        switch ( msgType )
        {
            case i5MessageApAutoconfigurationResponseValue:
            {
                /* band does not match active entry */
                if ( freqband != pentry->freqBand )
                {
                    return -1;
                }

                /* clear active session */
                wps1905StopApAutoConf();

                /* Fetch M1 from Wireless */
                rc = wps1905WscProcess(pentry->ifname, WPS_1905_CTL_STARTAUTOCONF, NULL, 0, &pData, &dataLen);
                if (WPS_1905_RESHANDLE_M1DATA != rc)
                {
                    return -1;
                }
                pentry->expectedMsg = i5MessageApAutoconfigurationWscValue;

                i5MessageApAutoconfigurationWscSend(pmsg->psock, i5MessageSrcMacAddressGet(pmsg), pData, dataLen);
                free(pData);
                if ( pApSearch->timer )
                {
                    i5TimerFree(pApSearch->timer);
                }
                
                pApSearch->timer = i5TimerNew(I5_MESSAGE_AP_SEARCH_M1_M2_WAITING_MSEC, i5WlcfgApAutoConfigTimer, pApSearch);
                break;
            }

            case i5MessageApAutoconfigurationWscValue:
            {
                i5_dm_interface_type *pinterface;
                int mtype;
                int rfband;
                
                rc = wps1905GetMessageInfo(pWscData, wscDataLen, &mtype, &rfband);
                if ( (rc != 0) || (mtype != WPS_ID_MESSAGE_M2) )
                {
                    return -1;
                }

                if ( (rfband < WPS_RFBAND_24GHZ) || (rfband > WPS_RFBAND_50GHZ) )
                {
                    return -1;
                }
                /* convert rfband to i5MessageFreqBand */
                rfband--;

                if ( pentry->freqBand != rfband )
                {
                    return -1;
                }

                rc = wps1905WscProcess(pentry->ifname, WPS_1905_CTL_CONFIGUREAP, pWscData, wscDataLen, &pData, &dataLen);
                if (rc != WPS_1905_M2SET_DONE)
                {
                    i5WlcfgApAutoConfigTimer(pApSearch);
                    return -1;
                }


                /* set wl interface to configured */
                pinterface = pdevice->interface_list.ll.next;
                while ( pinterface != NULL )
                {
                    if ( i5DmIsInterfaceWireless(pinterface->MediaType) &&
                         (0 == strcmp(pinterface->wlParentName, pentry->ifname)) )
                    {
                        pinterface->isConfigured = 1;
                        break;
                    }
                    pinterface = pinterface->ll.next;
                }

                /* remove entry from list */
                pApSearch->activeSearchEntry = NULL;
                i5LlItemFree(&pApSearch->searchEntryList, pentry);

                i5WlcfgApAutoConfigTimer(pApSearch);
                break;
            }

            default:
                break;
        }
    }

    return 0; 
}

void i5WlcfgApAutoconfigurationStart(const char *ifname)
{
    apSearchSendStruct   *pApSearch = &i5_config.apSearch;
    apSearchEntry        *pentry;
    i5_dm_device_type    *pdevice;
    i5_dm_interface_type *pinterface;

    i5Trace("\n");

    if ( 0 == wps1905ControlSocketReady() )
    {
        return;
    }

    pdevice = i5DmDeviceFind(&i5_config.i5_mac_address[0]);
    if ( NULL == pdevice)
    {
        return;
    }

    pinterface = pdevice->interface_list.ll.next;
    while ( pinterface )
    {
        if ( i5DmIsInterfaceWireless(pinterface->MediaType) &&
             ((NULL == ifname) ||
              (0 == strcmp(ifname, pinterface->wlParentName))) )
        {
            int freqBand = i5TlvGetFreqBandFromMediaType(pinterface->MediaType);
            if ( (0 == i5_config.isRegistrar) &&
                 (0 == i5WlCfgIsApConfigured(pinterface->wlParentName)) &&
                 (WPS_1905_WSCAP_ENABLED == wps1905WscEnabled(pinterface->wlParentName)) &&
                 (i5MessageFreqBand_Reserved != freqBand) && (1 == i5_config.freqBandEnable[freqBand]) )
            {
                /* add interface to the search list if it is not already present */
                pentry = pApSearch->searchEntryList.ll.next;
                while ( pentry != NULL )
                {
                   if ( 0 == strcmp(pinterface->wlParentName, pentry->ifname) )
                   {
                       break;
                   }
                   pentry = pentry->ll.next;
                }
                if ( NULL == pentry )
                {
                    pentry = (apSearchEntry *)malloc(sizeof(apSearchEntry));
                    if (pentry)
                    {
                        memset(pentry, 0, sizeof(apSearchEntry));
                        strncpy(pentry->ifname, pinterface->wlParentName, I5_MAX_IFNAME-1);
                        pentry->ifname[I5_MAX_IFNAME-1] = '\0';
                        pentry->freqBand = i5TlvGetFreqBandFromMediaType(pinterface->MediaType);
                        pentry->expectedMsg = i5MessageApAutoconfigurationResponseValue;
                        i5Trace("adding interface %s to the ap search list - band %d\n", pinterface->wlParentName, pentry->freqBand);
                        i5LlItemAdd(NULL, &pApSearch->searchEntryList, pentry);
                    }
                }
            }
        }
        pinterface = pinterface->ll.next;
    }

    pentry = pApSearch->searchEntryList.ll.next;
    while ( pentry != NULL )
    {
        if ( i5MessageApAutoconfigurationResponseValue == pentry->expectedMsg )
        {
            pentry->callCounter = 0;
        }
        pentry = pentry->ll.next;
    }
    
    if ( (i5_config.apSearch.timer == NULL) ||
         ((pApSearch->activeSearchEntry != NULL) && 
          (i5MessageApAutoconfigurationResponseValue == pApSearch->activeSearchEntry->expectedMsg)) )
    {
        i5WlcfgApAutoConfigTimer(pApSearch);
    }
}

void i5WlcfgApAutoconfigurationStop(char const *ifname)
{
    apSearchSendStruct *pApSearch = &i5_config.apSearch;
    apSearchEntry *pentry = pApSearch->searchEntryList.ll.next;
    apSearchEntry *pnext;

    i5Trace("\n");

    while ( pentry != NULL )
    {
       pnext = pentry->ll.next;
       if ( (NULL == ifname) || (0 == strcmp(ifname, pentry->ifname)) )
       {
           i5Trace("Removing entry for %s\n", pentry->ifname);
           i5LlItemRemove(&pApSearch->searchEntryList, pentry);
           if ( pentry == pApSearch->activeSearchEntry )
           {
               pApSearch->activeSearchEntry = NULL;
               i5WlcfgApAutoConfigTimer(pApSearch);
           }

           if ( ifname != NULL )
           {
               break;
           }
       }
       pentry = pnext;
    }
    /* active entry was removed so reconfigure the timer */ 
    if ( NULL == pApSearch->activeSearchEntry )
    {
       i5WlcfgApAutoConfigTimer(pApSearch);
    }
}

int i5WlcfgApAutoconfigurationRenewProcess(i5_message_type *pmsg, unsigned int freqband)
{
    i5_dm_device_type *pdevice = i5DmDeviceFind(i5_config.i5_mac_address);
    apSearchEntry *pentry;
    char           wlname[I5_MAX_IFNAME];
    int            matchingIf = i5DmIsWifiBandSupported(wlname, freqband);

    i5Trace(" band %d, match %d, name %s\n", freqband, matchingIf, (matchingIf ? wlname : "NULL"));

    if ( (NULL == pdevice) || (0 == matchingIf) )
    {
        return -1;
    }
 
    if ( 1 == i5_config.isRegistrar)
    {
        i5Trace(" Device is registrar - ignore renew\n");
        return -1;
    }
 
    /* ignore message if configuration is already in progress */
    pentry = i5_config.apSearch.searchEntryList.ll.next;
    while ( pentry != NULL )
    {
       if ( 0 == strcmp(wlname, pentry->ifname) ) {
          break;
       }
       pentry = pentry->ll.next;
    }
 
    if ( pentry )
    {
        i5Trace(" Configuration already in progress\n");
        return -1;
    }

    if ( (WPS_1905_WSCAP_ENABLED == wps1905WscEnabled(wlname)) &&
         (1 == i5_config.freqBandEnable[freqband]) )
    {
        pentry = (apSearchEntry *)malloc(sizeof(apSearchEntry));
        if (pentry) {
            strncpy(pentry->ifname, wlname, I5_MAX_IFNAME-1);
            pentry->ifname[I5_MAX_IFNAME-1] = '\0';
            pentry->freqBand = freqband;
            pentry->renew = 1;
            pentry->expectedMsg = i5MessageApAutoconfigurationWscValue;
            strncpy(pentry->rxIfname, pmsg->psock->u.sll.ifname, I5_MAX_IFNAME-1);
            pentry->rxIfname[I5_MAX_IFNAME-1] = '\0';
            memcpy(pentry->registrarMac, i5MessageSrcMacAddressGet(pmsg), MAC_ADDR_LEN);
            i5LlItemAdd(NULL, &i5_config.apSearch.searchEntryList, pentry);
            i5Trace(" Adding interface %s for renew\n", wlname);
        }

        if ( i5_config.apSearch.timer == NULL ) {
            i5WlcfgApAutoConfigTimer(&i5_config.apSearch);
        }
    }

    return 0;
}

#if defined(DSLCPE_WLCSM_EXT)
void wlcsm_event_handler(t_WLCSM_EVENT_TYPE type,...) {
   va_list arglist;
   va_start(arglist,type);

   switch ( type ) {
      case WLCSM_EVT_NVRAM_CHANGED:
         {
            char * name=va_arg(arglist,char *);
            char * value=va_arg(arglist,char *); 
            char * oldvalue=va_arg(arglist,char *); 
            printf("--:%s:%d  nvram change received name:%s,value:%s,oldvalue:%s\r\n",__FUNCTION__,__LINE__ ,name,value,oldvalue);

         }
         break;
      case WLCSM_EVT_NVRAM_COMMITTED:
         printf("---:%s:%d  nvram committed \r\n",__FUNCTION__,__LINE__ );
         break;

      default:   
         break;
   }

   va_end(arglist);
}
#endif

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  i5ctlWlcfgHandler
 *  Description:  function handler to handle i5ctl command
 * =====================================================================================
 */
int i5ctlWlcfgHandler(i5_socket_type *psock, t_I5_API_WLCFG_MSG *pMsg)
{
    int status = 0;
    int cmd    = I5_API_CMD_WLCFG;

    i5Trace("sub command is %d, interface %s\n", pMsg->subcmd, pMsg->ifname);
    switch(pMsg->subcmd) {
#if defined(DSLCPE_WLCSM_EXT)
    case  I5_CTL_WLCFG_NVRAM_SET: {
        char buf[100];
        char *name,*value;
        strncpy(value = buf,(char *)(pMsg+1), 100);
        name = strsep(&value, "=");
        wlcsm_nvram_set(name, value);
        _i5WlcfgCtlResponse(psock,cmd,pMsg->subcmd,status,NULL,0);
    }
    break;
    case  I5_CTL_WLCFG_NVRAM_UNSET: {
        wlcsm_nvram_unset((char *)(pMsg+1));
        _i5WlcfgCtlResponse(psock,cmd,pMsg->subcmd,status,"OK",3);
    }
    break;
    case  I5_CTL_WLCFG_NVRAM_COMMIT: {
        wlcsm_nvram_commit();
        _i5WlcfgCtlResponse(psock,cmd,pMsg->subcmd,status,"OK",3);
    }
    break;
    case  I5_CTL_WLCFG_NVRAM_GET: {
        char *value=wlcsm_nvram_get((char *)(pMsg+1));
        if(value)
            _i5WlcfgCtlResponse(psock,cmd,pMsg->subcmd,status,value,strlen(value)+1);
        else
            _i5WlcfgCtlResponse(psock,cmd,pMsg->subcmd,status,NULL,0);
    }
    break;
    case I5_CTL_WLCFG_NVRAM_TRACE:
        wlcsm_set_trace_level((char *)(pMsg+1));
        _i5WlcfgCtlResponse(psock,cmd,pMsg->subcmd,status,"OK",3);
        break;
#endif
    case  I5_CTL_WLCFG_WSCSTATUS:
        status=wps1905WscStatus();
        _i5WlcfgCtlResponse(psock,cmd,pMsg->subcmd,status,NULL,0);
        break;
    case  I5_CTL_WLCFG_WSCCANCEL:
        status=wps1905StopApAutoConf();
        _i5WlcfgCtlResponse(psock,cmd,pMsg->subcmd,status,NULL,0);
        break;
    case  I5_CTL_WLCFG_WSCENABLED:
        status=wps1905WscEnabled(pMsg->ifname);
        _i5WlcfgCtlResponse(psock,cmd,pMsg->subcmd,status,NULL,0);
        break;
    case  I5_CTL_WLCFG_WLINSTANCE:
        status=wps1905WlInstance();
        _i5WlcfgCtlResponse(psock,cmd,pMsg->subcmd,status,NULL,0);
        break;
    case  I5_CTL_WLCFG_APISCONFIGURED:
        status=i5WlCfgIsApConfigured(pMsg->ifname);
        _i5WlcfgCtlResponse(psock,cmd,pMsg->subcmd,status,NULL,0);
        break;
    default:
        status=wlm_1905_i5ctl_handler(psock,pMsg);
        if(status<0)
        {
            i5Trace("Unsupported wlcfg sub command\n");
            _i5WlcfgCtlResponse(psock,cmd,pMsg->subcmd,1,"unsupported sub command",0);
        }
        else
            _i5WlcfgCtlResponse(psock,cmd,pMsg->subcmd,1,"success",0);
        break;
    }
    return status;
}

void i5WlCfgInit( void )
{
    int                sd;
    struct timeval     tv;

    sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if ( sd < 0 )
    {
        printf("i5WlCfgInit: failed to create udp socket\n");
        return;
    }

    tv.tv_sec  = WPS_1905_RECEIVE_TIMEOUT_MS / 1000;
    tv.tv_usec = (WPS_1905_RECEIVE_TIMEOUT_MS % 1000) * 1000;
    if (setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval)) < 0)
    {
        printf("could not set receive timeout socket option: errno %d\n", errno);
    }

    i5_config.wl_control_socket.psock = i5SocketNew(sd, i5_socket_type_udp, wps1905ProcessSocket);
    if ( NULL == i5_config.wl_control_socket.psock )
    {
        printf("i5WlCfgInit: failed to create i5_socket_type_udp\n");
        close(sd);
        return;
    }

    /* userspace address*/
    i5_config.wl_control_socket.psock->u.sinl.sa.sin_family      = AF_INET;
    i5_config.wl_control_socket.psock->u.sinl.sa.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(i5_config.wl_control_socket.psock->sd, (struct sockaddr *)&(i5_config.wl_control_socket.psock->u.sinl.sa), sizeof(struct sockaddr_in)) < 0)
    {
        printf("Failed to bind to udp receive socket: errno %d\n", errno);
        i5SocketClose(i5_config.wl_control_socket.psock);
        return;
    }

    i5_config.wl_control_socket.ptmr = i5TimerNew(WPS_1905_RECEIVE_TIMEOUT_MS, wps1905Register, &i5_config.wl_control_socket);

#if defined(DSLCPE_WLCSM_EXT)
    wlcsm_register_event_generic_hook(wlcsm_event_handler);
    wlcsm_set_trace(3);
#endif
    
}

void i5WlCfgDeInit( void )
{
#if defined(DSLCPE_WLCSM_EXT)
   wlcsm_shutdown();
#endif
}

#endif

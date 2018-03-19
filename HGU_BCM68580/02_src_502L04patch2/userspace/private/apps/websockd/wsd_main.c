/***********************************************************************
 *
 *  Copyright (c) 2014  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2014:proprietary:standard 

 This program is the proprietary software of Broadcom and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").  Except as set forth in
 an Authorized License, Broadcom grants no license (express or implied), right
 to use, or waiver of any kind with respect to the Software, and Broadcom
 expressly reserves all rights in and to the Software and all intellectual
 property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,

 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use
    all reasonable efforts to protect the confidentiality thereof, and to
    use this information only in connection with your use of Broadcom
    integrated circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
    PERFORMANCE OF THE SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
    LIMITED REMEDY.
:>
 *
 ************************************************************************/

#ifdef SUPPORT_WEB_SOCKETS


#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <syslog.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <poll.h>

#include "websockets/libwebsockets.h"

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"

#include "wsd_main.h"

#ifndef BRCM_CMS_BUILD
#include "httpdshared.h"
#endif

/*
 * global variables
 */
void *wsd_msg_hndl = NULL;
void *wsd_tmr_hndl = NULL;
/* epoll fd */
int pfd;

static struct lws_context *context;


/* List of supported protocols and callbacks
 * The order of "struct lws_protocols
 * protocols" should match with the order of
 * "enum wsd_protocols" that is defined in
 * wsd_main.h
 */

static struct lws_protocols protocols[] = {
    /* first protocol must always be HTTP handler */
    {
        "http-only",    /* name */
        wsd_callback_http,  /* callback */
        0, /* per_session_data_size */
        0, /* max frame size / rx buffer */
    },
    {
        "cpu-mem",
        wsd_callback_cpu_mem,
        sizeof(struct psd_session_key),
        0,
    },
#ifdef SUPPORT_DPI
    {
        "dpi",
        wsd_callback_dpi,
        sizeof(struct wsd_dpi_state),
        4096,
    },
#endif    // SUPPORT_DPI
#ifdef DMP_X_BROADCOM_COM_SPDSVC_1
    {
        "speed-service",
        wsd_callback_speed_service,
        sizeof(struct psd_speed_service),
        256,
    },
#endif    // DMP_X_BROADCOM_COM_SPDSVC_1
    { NULL, NULL, 0, 0 } /* terminator */
};


static int wsd_msg_handler(const CmsMsgHeader *msg)
{
    int rv = 0;
    CmsRet ret = CMSRET_SUCCESS;

    switch (msg->type)
    {
#ifdef DMP_X_BROADCOM_COM_SPDSVC_1
        case CMS_MSG_SPDSVC_DIAG_COMPLETE:
            lws_callback_on_writable_all_protocol(context, &protocols[WSD_PROTOCOL_SPEED_SERVICE]);
            rv = 0;
            break;
#endif   // DMP_X_BROADCOM_COM_SPDSVC_1

        case CMS_MSG_WAN_CONNECTION_UP:
            rv = 0;
            break;

        case CMS_MSG_SET_LOG_LEVEL:
            cmsLog_debug("got set log level to %d", msg->wordData);
            cmsLog_setLevel(msg->wordData);
            if ((ret = cmsMsg_sendReply(wsd_msg_hndl, msg, CMSRET_SUCCESS)) != CMSRET_SUCCESS)
            {
                cmsLog_error("send response for msg 0x%x failed, ret=%d", msg->type, ret);
            }
            rv = 0;
            break;

        case CMS_MSG_SET_LOG_DESTINATION:
            cmsLog_debug("got set log destination to %d", msg->wordData);
            cmsLog_setDestination(msg->wordData);
            if ((ret = cmsMsg_sendReply(wsd_msg_hndl, msg, CMSRET_SUCCESS)) != CMSRET_SUCCESS)
            {
                cmsLog_error("send response for msg 0x%x failed, ret=%d", msg->type, ret);
            }
            rv = 0;
            break;

#ifdef CMS_MEM_LEAK_TRACING
        case CMS_MSG_MEM_DUMP_TRACEALL:
            cmsMem_dumpTraceAll();
            rv = 0;
            break;

        case CMS_MSG_MEM_DUMP_TRACE50:
            cmsMem_dumpTrace50();
            rv = 0;
            break;

        case CMS_MSG_MEM_DUMP_TRACECLONES:
            cmsMem_dumpTraceClones();
            rv = 0;
            break;
#endif

        default:
            cmsLog_notice("Invalid message type (%x)", (unsigned int)msg->type);
            // make sure we don't bomb out for this reason.
            rv = 0;
    }

    return rv;
}


static int wsd_stack(void)
{
    struct epoll_event evts[WSD_EPOLL_NUM_EVTS];
    struct epoll_event evt = {0};
    int commFd = 0;
    int rv, i;

    /* initialize timer events  */
    if (cmsTmr_getNumberOfEvents(wsd_tmr_hndl) > 0)
        cmsTmr_dumpEvents(wsd_tmr_hndl);

    /* add the CMS messaging handle to our epoll */
    cmsMsg_getEventHandle(wsd_msg_hndl, &commFd);
    evt.events = EPOLLIN;
    evt.data.fd = commFd;
    epoll_ctl(pfd, EPOLL_CTL_ADD, commFd, &evt);

    while (1)
    {
        /*
        * service all timer events that are due (there may be no events due
        * if we woke up from select because of activity on the fds).
        */
        cmsTmr_executeExpiredEvents(wsd_tmr_hndl);

        rv = epoll_wait(pfd, evts, WSD_EPOLL_NUM_EVTS, WSD_TIMEOUT_MSEC * 1000);
        if (rv <= 0)
            continue;

        /* Iterate through all events */
        for (i = 0; i < rv; i++)
        {
            struct pollfd fd = {0};

            /* setup the pollfd data for lws */
            fd.fd = evts[i].data.fd;
            if (evts[i].events & EPOLLIN)  fd.events |= LWS_POLLIN;
            if (evts[i].events & EPOLLOUT) fd.events |= LWS_POLLOUT;
            if (evts[i].events & EPOLLHUP) fd.events |= LWS_POLLHUP;
            fd.revents = fd.events;

            /* Let libwebsockets try to handle the socket event. If it sets the
             * revents field to 0, it means the event was handled and we can
             * safely skip it. */
            lws_service_fd(context, &fd);
            if (!fd.revents)
                continue;

            /* Handle CMS message */
            if (evts[i].data.fd == commFd && evts[i].events & EPOLLIN)
            {
                CmsMsgHeader *msg = NULL;
                CmsRet ret;

                ret = cmsMsg_receiveWithTimeout(wsd_msg_hndl, &msg, WSD_TIMEOUT_MSEC);
                if (ret != CMSRET_SUCCESS)
                {
                    if (!cmsFil_isFilePresent(SMD_SHUTDOWN_IN_PROGRESS))
                        cmsLog_error("Failed to receive message (ret=%d)", ret);
                    break;
                }
                else
                {
                    rv = wsd_msg_handler(msg);
                    CMSMEM_FREE_BUF_AND_NULL_PTR(msg);
                    if(rv)
                        break;
                }
            }
        }
    }

    printf("\n%s: EXITING (%d)\n\n", __FUNCTION__, rv);
    return rv;
}


static int wsd_websocket_init(void)
{
    int ret = 0;
    int debug_level = LLL_ERR | LLL_WARN | LLL_NOTICE;
//    int debug_level = LLL_ERR | LLL_WARN | LLL_NOTICE | LLL_INFO | LLL_DEBUG | LLL_PARSER | LLL_HEADER;
    int opts = LWS_SERVER_OPTION_ALLOW_NON_SSL_ON_SSL_PORT;
//    int opts = LWS_SERVER_OPTION_ALLOW_NON_SSL_ON_SSL_PORT | LWS_SERVER_OPTION_DISABLE_OS_CA_CERTS | LWS_SERVER_OPTION_PEER_CERT_NOT_REQUIRED;
    char cert_path[BUFLEN_1024], key_path[BUFLEN_1024];
    const char *iface = NULL;
    struct lws_context_creation_info info;

    memset(&info, 0, sizeof(info));
    info.port = WSD_PORT;

    /* tell the library what debug level to emit and to send it to syslog */
    lws_set_log_level(debug_level, lwsl_emit_syslog);
//    lws_set_log_level(debug_level, NULL);

    info.iface = iface;
    info.protocols = protocols;

    cmsUtl_getRunTimePath("/var/tmp/newkey.cert", cert_path, sizeof(cert_path));
    cmsUtl_getRunTimePath("/var/tmp/newkey", key_path, sizeof(key_path));
    info.ssl_cert_filepath = cert_path;
    info.ssl_private_key_filepath = key_path;

    info.gid = -1;
    info.uid = -1;
    info.options = opts;

    context = lws_create_context(&info);
    if (context == NULL)
    {
        lwsl_err("Websockets server initialization failed\n");
        ret = -1;
    }

    return ret;
}


static void wsd_websocket_cleanup(void)
{
    if (context != NULL)
    {
        lws_context_destroy(context);
    }

    lwsl_notice("Websockets server exited cleanly\n");
}


/** Register or unregister our interest for some event events with smd.
 *
 * @param msgType (IN) The notification message/event that we are
 *                     interested in or no longer interested in.
 * @param positive (IN) If true, then register, else unregister.
 * @param data     (IN) Any optional data to send with the message.
 * @param dataLength (IN) Length of the data
 */
static void wsd_register_event
    (CmsMsgType msgType,
     UBOOL8 positive,
     void *msgData,
     UINT32 msgDataLen)
{
    CmsMsgHeader *msg = NULL;
    char *data = NULL;
    void *msgBuf = NULL;
    char *action __attribute__ ((unused)) = (positive) ? "REGISTER" : "UNREGISTER";
    CmsRet ret = CMSRET_SUCCESS;

    if (msgData != NULL && msgDataLen != 0)
    {
        /* for msg with user data */
        msgBuf = cmsMem_alloc(sizeof(CmsMsgHeader) + msgDataLen, ALLOC_ZEROIZE);
    } 
    else
    {
        msgBuf = cmsMem_alloc(sizeof(CmsMsgHeader), ALLOC_ZEROIZE);
    }

    msg = (CmsMsgHeader *)msgBuf;

    /* fill in the msg header */
    msg->type = (positive) ? CMS_MSG_REGISTER_EVENT_INTEREST : CMS_MSG_UNREGISTER_EVENT_INTEREST;
    msg->src = EID_WEBSOCKD;
    msg->dst = EID_SMD;
    msg->flags_request = 1;
    msg->wordData = msgType;

    if (msgData != NULL && msgDataLen != 0)
    {
        data = (char *) (msg + 1);
        msg->dataLength = msgDataLen;
        memcpy(data, (char *)msgData, msgDataLen);
    }      

    ret = cmsMsg_sendAndGetReply(wsd_msg_hndl, msg);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("%s_EVENT_INTEREST for 0x%x failed, ret=%d", action, msgType, ret);
    }
    else
    {
        cmsLog_debug("%s_EVENT_INTEREST for 0x%x succeeded", action, msgType);
    }

    cmsMem_free(msgBuf);
}


static CmsRet cms_init(const CmsEntityId entityId)
{
    int sessionPid = 0, shmId = 0;
    CmsRet ret = CMSRET_SUCCESS;
    CmsMsgHeader *buf = NULL;

    /*
    * Detach myself from the terminal so I don't get any control-c/sigint.
    * On the desktop, it is smd's job to catch control-c and exit.
    * When websockd detects that smd has exited, websockd will also exit.
    */
    if ((sessionPid = setsid()) == -1)
    {
        cmsLog_error("Could not detach from terminal");
    }
    else
    {
        cmsLog_debug("Detached from terminal");
    }

    if ((ret = cmsMsg_init(entityId, &wsd_msg_hndl)) != CMSRET_SUCCESS)
    {
        cmsLog_error("msg initialization failed, ret=%d", ret);
        return ret;
    }

    ret = cmsMsg_receiveWithTimeout(wsd_msg_hndl, &buf, 100);
    if(ret == CMSRET_SUCCESS)
    {
        CMSMEM_FREE_BUF_AND_NULL_PTR(buf);
    }

    if ((ret = cmsMdm_init(entityId, wsd_msg_hndl, &shmId)) != CMSRET_SUCCESS)
    {
        cmsLog_error("cmsMdm_init failed, ret=%d", ret);
        return ret;
    }

    cmsLog_notice("cmsMdm_init successful, shmId=%d", shmId);

    if ((ret = cmsTmr_init(&wsd_tmr_hndl)) != CMSRET_SUCCESS)
    {
        cmsLog_error("cmsTmr_init failed, ret=%d", ret);
        return ret;
    }

    cmsLog_notice("cmsTmr_init successful");

    // Register WSD interest events with smd.
    wsd_register_event(CMS_MSG_WAN_CONNECTION_UP, TRUE, NULL, 0);

    return ret;
}


static void cms_cleanup(void)
{
    // Unregister WSD interest events with smd.
    wsd_register_event(CMS_MSG_WAN_CONNECTION_UP, FALSE, NULL, 0);

    cmsTmr_cleanup(&wsd_tmr_hndl);
    cmsMdm_cleanup();
    cmsMsg_cleanup(&wsd_msg_hndl);
}


static int wsd_init(const CmsEntityId entityId)
{
    int rv = 0;
    CmsRet ret = CMSRET_SUCCESS;

    /* Ignore broken pipes */
    signal(SIGPIPE, SIG_IGN);

    /* configure epoll */
    pfd = epoll_create1(0);
    if (pfd < 0)
    {
        printf("%s: unable to create epoll fd - %s\n", __func__, strerror(errno));
        return -1;
    }

    /* initialize the CMS framework */
    if ((ret = cms_init(entityId)) != CMSRET_SUCCESS)
    {
        cmsLog_error("CMS initialization failed (%d), exit.", ret);
        rv = -1;
    }

    /* initialize websocket context */
    if ((rv = wsd_websocket_init()) < CMSRET_SUCCESS)
    {
        cmsLog_error("Websockets server initialization failed (%d), exit.", rv);
    }

    return rv;
}


static void wsd_exit(void)
{
    /* clean websocket resources */
    wsd_websocket_cleanup();
    /* clean CMS resources */
    cms_cleanup();
    /* cleanup epoll */
    close(pfd);
}


int main(int argc, char **argv)
{
    int rv = 0;
    CmsEntityId entityId = EID_WEBSOCKD;

    /* initialize CMS logging */
    cmsLog_init(entityId);

    cmsLog_debug("initializing %s...", *argv);

    rv = wsd_init(entityId);

    if (!rv)
    {
        CmsLogLevel logLevel = cmsLog_getLevel();

        cmsLog_setHeaderMask(0);
        cmsLog_setLevel(LOG_LEVEL_NOTICE);
        cmsLog_notice("Broadcom Websocket Server Stack v%s (%s, %s) is based in part on the work of the libwebsockets project (http://libwebsockets.org)",
                WSD_VERSION, __DATE__, __TIME__);
        cmsLog_setLevel(logLevel);
        cmsLog_setHeaderMask(DEFAULT_LOG_HEADER_MASK);

        rv = wsd_stack();
    }

    wsd_exit();

    cmsLog_notice("exiting with code %d", rv);

    /* cleanup  CMS logging */
    cmsLog_cleanup();

    return rv;
}


/*
 * this is just an example of parsing handshake headers, you don't need this
 * in your code unless you will filter allowing connections by the header
 * content
 */

void dump_handshake_info(struct lws *wsi)
{
    int n;
    static const char *token_names[] = {
        /*[WSI_TOKEN_GET_URI]		=*/ "GET URI",
        /*[WSI_TOKEN_POST_URI]		=*/ "POST URI",
        /*[WSI_TOKEN_HOST]		=*/ "Host",
        /*[WSI_TOKEN_CONNECTION]	=*/ "Connection",
        /*[WSI_TOKEN_KEY1]		=*/ "key 1",
        /*[WSI_TOKEN_KEY2]		=*/ "key 2",
        /*[WSI_TOKEN_PROTOCOL]		=*/ "Protocol",
        /*[WSI_TOKEN_UPGRADE]		=*/ "Upgrade",
        /*[WSI_TOKEN_ORIGIN]		=*/ "Origin",
        /*[WSI_TOKEN_DRAFT]		=*/ "Draft",
        /*[WSI_TOKEN_CHALLENGE]		=*/ "Challenge",

        /* new for 04 */
        /*[WSI_TOKEN_KEY]		=*/ "Key",
        /*[WSI_TOKEN_VERSION]		=*/ "Version",
        /*[WSI_TOKEN_SWORIGIN]		=*/ "Sworigin",

        /* new for 05 */
        /*[WSI_TOKEN_EXTENSIONS]	=*/ "Extensions",

        /* client receives these */
        /*[WSI_TOKEN_ACCEPT]		=*/ "Accept",
        /*[WSI_TOKEN_NONCE]		=*/ "Nonce",
        /*[WSI_TOKEN_HTTP]		=*/ "Http",

        "Accept:",
        "If-Modified-Since:",
        "Accept-Encoding:",
        "Accept-Language:",
        "Pragma:",
        "Cache-Control:",
        "Authorization:",
        "Cookie:",
        "Content-Length:",
        "Content-Type:",
        "Date:",
        "Range:",
        "Referer:",
        "Uri-Args:",

        /*[WSI_TOKEN_MUXURL]	=*/ "MuxURL",
    };

    char buf[256];

    for (n = 0; n < sizeof(token_names) / sizeof(token_names[0]); n++)
    {
        if (!lws_hdr_total_length(wsi, n))
            continue;

        lws_hdr_copy(wsi, buf, sizeof buf, n);

        fprintf(stderr, "    %s = %s\n", token_names[n], buf);
    }
}


int wsd_callback_http
    (struct lws *wsi,
     enum lws_callback_reasons reason,
     void *user,
     void *in,
     size_t len)
{
    struct lws_pollargs *pa = (struct lws_pollargs *)in;
    struct epoll_event evt = {0};

    switch(reason)
    {
      /* Handle external poll fd callbacks for all sockets */
      case LWS_CALLBACK_ADD_POLL_FD:
      case LWS_CALLBACK_CHANGE_MODE_POLL_FD:
          evt.data.fd = pa->fd;
          if(pa->events & LWS_POLLIN)  evt.events |= EPOLLIN;
          if(pa->events & LWS_POLLOUT) evt.events |= EPOLLOUT;
          return epoll_ctl(pfd, (reason == LWS_CALLBACK_ADD_POLL_FD) ?
                  EPOLL_CTL_ADD : EPOLL_CTL_MOD, pa->fd, &evt);

      case LWS_CALLBACK_DEL_POLL_FD:
          return epoll_ctl(pfd, EPOLL_CTL_DEL, pa->fd, NULL);

      default:
          break;
    }
    return 0;
}


UBOOL8 wsd_validate_session_key(int sessionKey)
{
   UBOOL8 result = FALSE;

#ifdef BRCM_CMS_BUILD
   CmsMsgHeader msg = EMPTY_MSG_HEADER;
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_notice("Sending message to httpd for session key validation");

   msg.type = CMS_MSG_VALIDATE_SESSION_KEY;
   msg.src = EID_WEBSOCKD;
   msg.dst = EID_HTTPD;
   msg.wordData = (UINT32) sessionKey;
   msg.flags_request = 1;

   ret = cmsMsg_sendAndGetReplyWithTimeout(wsd_msg_hndl, &msg, 5*MSECS_IN_SEC);
   if (ret == CMSRET_SUCCESS)
       result = TRUE;
   else if (ret == CMSRET_INVALID_PARAM_VALUE)
      cmsLog_error("Session key %d is invalid", sessionKey);
   else
      cmsLog_error("Sending message to httpd for session key validation FAILED, ret %d", ret);
#else
   result = hsl_checkSessionKey();
#endif    // BRCM_CMS_BUILD

   return result;
}


#endif   // SUPPORT_WEB_SOCKETS


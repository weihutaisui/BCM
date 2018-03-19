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

#include <pthread.h>
#include <signal.h>

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"
#include "cms_qdm.h"
#include "xmppc_conn.h"

#define XMPPC_VERSION "0.1"


/*
 * global variables
 */
void *xmppc_msg_hndl = NULL;
void *xmppc_tmr_hndl = NULL;


/*
 * mutex, signal data for synchronization between
 * xmppc main and xmppc thread. They're used in
 * xmppc_main.c, xmppc_conn.c
 */
pthread_mutex_t comm_mutex = PTHREAD_MUTEX_INITIALIZER;
UBOOL8 delete_list_is_empty = TRUE;
char *delete_jabberIDs[XMPPC_MAX_NUMBER_OF_CONNECTIONS];


CmsRet xmppc_conn_delete(const char *jabberID)
{
    UINT32 i = 0;
    CmsRet ret = CMSRET_SUCCESS;

    pthread_mutex_lock(&comm_mutex);

    delete_list_is_empty = FALSE;

    for (i = 0; i < XMPPC_MAX_NUMBER_OF_CONNECTIONS; i++)
    {
        if (delete_jabberIDs[i] == NULL)
        {
            delete_jabberIDs[i] = cmsMem_strdupFlags(jabberID, ALLOC_ZEROIZE | ALLOC_SHARED_MEM);
            break;
        }
    }

    if (i == XMPPC_MAX_NUMBER_OF_CONNECTIONS)
    {
        cmsLog_error("XMPP connection remove list is FULL");
        ret = CMSRET_RESOURCE_EXCEEDED;
    }

    pthread_mutex_unlock(&comm_mutex);

    return ret;
}


CmsRet xmppc_conn_create
    (const char *jabberID,
     const char *password,
     const char *serverAddress,
     const UINT32 serverPort)
{
    int rv = 0;
    struct sched_param t_sched;
    pthread_t t_id;
    pthread_attr_t t_attr;
    XMPP_THREAD_DATA data;
    CmsRet ret = CMSRET_SUCCESS;

    if (jabberID == NULL)
    {
        cmsLog_error("Error in creating client thread for XMPP connection: jabberID is NULL");
        return CMSRET_INVALID_ARGUMENTS;
    }

    if (password == NULL)
    {
        cmsLog_error("Error in creating client thread for XMPP connection: password is NULL");
        return CMSRET_INVALID_ARGUMENTS;
    }

    pthread_attr_init(&t_attr);
    t_sched.sched_priority = 1;  /* low priority */
    pthread_attr_setschedparam(&t_attr, &t_sched);
    pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);

    memset(&data, 0, sizeof(data));

    cmsUtl_strcpy(data.jabberID, jabberID);
    cmsUtl_strcpy(data.password, password);
    if (serverAddress[0] != '\0')
    {
       cmsUtl_strcpy(data.serverAddress, serverAddress);
       data.serverPort = serverPort;
    }

    rv = pthread_create(&t_id, &t_attr, (void *) &xmppc_conn_stack, (void *) &data);
    if (rv != 0)
    {
        cmsLog_error("Error in creating thread for XMPP connection with jabberID='%s', password='%s', rv=%d",
                     jabberID, password, rv);
        ret = CMSRET_INTERNAL_ERROR;
    }
    else
    {
        cmsLog_notice("Successfully creating thread for XMPP connection with jabberID='%s', password='%s', server %s/%d",
                      jabberID, password,serverAddress,serverPort);
        ret = CMSRET_SUCCESS;
    }

    sleep(2);

    return ret;
}


static CmsRet xmppc_connect_all(void)
{
    Dev2XmppConnObject *xmppConn = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_SUCCESS;
    char serverAddress[BUFLEN_256]={0};
    UINT32 serverPort=0;

    ret = cmsLck_acquireLockWithTimeout(6*MSECS_IN_SEC);
    if (ret != CMSRET_SUCCESS)
    {
       cmsLog_error("Could not acquire CMS MDM lock, ret=%d"
                    "  Dumping debug info:", ret);
       cmsLck_dumpInfo();
       return ret;
    }

    /* loop through XMPP connection table to bring up connections if any */
    while (ret == CMSRET_SUCCESS &&
           cmsObj_getNext(MDMOID_DEV2_XMPP_CONN,
                          &iidStack,
                          (void **) &xmppConn) == CMSRET_SUCCESS)
    {
        if (xmppConn->enable == TRUE &&
            cmsUtl_strcmp(xmppConn->status, "Down") == 0)
        {
            qdmXmpp_getServerForConnectionLocked_dev2(&iidStack,serverAddress,&serverPort);
            ret = xmppc_conn_create(xmppConn->jabberID,
                                    xmppConn->password,
                                    serverAddress,serverPort);
        }

        cmsObj_free((void **) &xmppConn);
    }

    cmsLck_releaseLock();

    return ret;
}


static int xmppc_msg_handler(const CmsMsgHeader *msg)
{
    int rv = 0;
    XmppConnMsgBody *msgData = NULL;
    CmsRet ret = CMSRET_SUCCESS;

    switch (msg->type)
    {
        case CMS_MSG_XMPP_CONNECTION_ENABLE:
            msgData = (XmppConnMsgBody *) (msg + 1);
            if (msgData->enable == TRUE)
            {
                xmppc_conn_create(msgData->jabberID,
                                  msgData->password,
                                  msgData->serverAddress,msgData->serverPort);
            }
            else
            {
                xmppc_conn_delete(msgData->jabberID);
            }
            rv = 0;
            break;

        case CMS_MSG_XMPP_CONNECTION_DELETE:
            msgData = (XmppConnMsgBody *) (msg + 1);
            xmppc_conn_delete(msgData->jabberID);
            rv = 0;
            break;

        case CMS_MSG_WAN_CONNECTION_UP:
            xmppc_connect_all();
            rv = 0;
            break;

        case CMS_MSG_SET_LOG_LEVEL:
            cmsLog_debug("got set log level to %d", msg->wordData);
            cmsLog_setLevel(msg->wordData);
            if ((ret = cmsMsg_sendReply(xmppc_msg_hndl, msg, CMSRET_SUCCESS)) != CMSRET_SUCCESS)
            {
                cmsLog_error("send response for msg 0x%x failed, ret=%d", msg->type, ret);
            }
            rv = 0;
            break;

        case CMS_MSG_SET_LOG_DESTINATION:
            cmsLog_debug("got set log destination to %d", msg->wordData);
            cmsLog_setDestination(msg->wordData);
            if ((ret = cmsMsg_sendReply(xmppc_msg_hndl, msg, CMSRET_SUCCESS)) != CMSRET_SUCCESS)
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


static int xmppc_stack(void)
{
    int rv = 0;
    CmsLogLevel logLevel = cmsLog_getLevel();
    CmsRet ret = CMSRET_SUCCESS;
    SINT32 commFd = 0;
    fd_set readFds;
    CmsMsgHeader *msg = NULL;
    struct timeval tm;
    UINT32 nextEventMs = 0;  /* number of milli-seconds until next event */

    cmsLog_setHeaderMask(0);

    cmsLog_setLevel(LOG_LEVEL_NOTICE);
    cmsLog_notice("Broadcom XMPP Client Stack v%s (%s, %s)", XMPPC_VERSION,
        __DATE__, __TIME__);
    cmsLog_setLevel(logLevel);

    cmsLog_setHeaderMask(DEFAULT_LOG_HEADER_MASK);

    /* initialize timer events  */
    if (cmsTmr_getNumberOfEvents(xmppc_tmr_hndl) > 0)
    {
        cmsTmr_dumpEvents(xmppc_tmr_hndl);
    }

    /* get the CMS messaging handle */
    cmsMsg_getEventHandle(xmppc_msg_hndl, &commFd);

    while (!rv)
    {
        /*
        * Set our select timeout based on time to next event
        * in our timer handle.
        */
        const UINT32 sched_lag_time = 10;

        cmsTmr_getTimeToNextEvent(xmppc_tmr_hndl, &nextEventMs);
        if (nextEventMs < UINT32_MAX - sched_lag_time)
        {
            nextEventMs += sched_lag_time;
        }

        tm.tv_sec = nextEventMs / MSECS_IN_SEC;
        tm.tv_usec = (nextEventMs % MSECS_IN_SEC) * USECS_IN_MSEC;

        /* set up all the fd stuff for select */
        FD_ZERO(&readFds);
        FD_SET(commFd, &readFds);

        rv = select(commFd+1, &readFds, NULL, NULL, &tm);
        if (rv < 0)
        {
            /* interrupted by signal or something, continue */
            rv = 0;

            continue;
        }

        /*
        * service all timer events that are due (there may be no events due
        * if we woke up from select because of activity on the fds).
        */
        cmsTmr_executeExpiredEvents(xmppc_tmr_hndl);

        if (FD_ISSET(commFd, &readFds))
        {
            if ((ret = cmsMsg_receiveWithTimeout(xmppc_msg_hndl, &msg, 500)) != CMSRET_SUCCESS)
            {
                if (!cmsFil_isFilePresent(SMD_SHUTDOWN_IN_PROGRESS))
                {
                    cmsLog_error("Failed to receive message (ret=%d)", ret);
                }
                rv = -1;
            }
            else
            {
                rv = xmppc_msg_handler(msg);
                CMSMEM_FREE_BUF_AND_NULL_PTR(msg);
            }
        }
    }

    printf("\n%s: EXITING (%d)\n\n", __FUNCTION__, rv);

    return rv;
}


/** Register or unregister our interest for some event events with smd.
 *
 * @param msgType (IN) The notification message/event that we are
 *                     interested in or no longer interested in.
 * @param positive (IN) If true, then register, else unregister.
 * @param data     (IN) Any optional data to send with the message.
 * @param dataLength (IN) Length of the data
 */
static void xmppc_register_event
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
    msg->src = EID_XMPPC;
    msg->dst = EID_SMD;
    msg->flags_request = 1;
    msg->wordData = msgType;

    if (msgData != NULL && msgDataLen != 0)
    {
        data = (char *) (msg + 1);
        msg->dataLength = msgDataLen;
        memcpy(data, (char *)msgData, msgDataLen);
    }      

    ret = cmsMsg_sendAndGetReply(xmppc_msg_hndl, msg);
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
    SINT32 sessionPid = 0, shmId = 0;
    CmsRet ret = CMSRET_SUCCESS;
    CmsMsgHeader *buf = NULL;

    /*
    * Detach myself from the terminal so I don't get any control-c/sigint.
    * On the desktop, it is smd's job to catch control-c and exit.
    * When omcid detects that smd has exited, omcid will also exit.
    */
    if ((sessionPid = setsid()) == -1)
    {
        cmsLog_error("Could not detach from terminal");
    }
    else
    {
        cmsLog_debug("Detached from terminal");
    }

    if ((ret = cmsMsg_init(entityId, &xmppc_msg_hndl)) != CMSRET_SUCCESS)
    {
        cmsLog_error("msg initialization failed, ret=%d", ret);
        return ret;
    }

    ret = cmsMsg_receiveWithTimeout(xmppc_msg_hndl, &buf, 100);
    if(ret == CMSRET_SUCCESS)
    {
        CMSMEM_FREE_BUF_AND_NULL_PTR(buf);
    }

    if ((ret = cmsMdm_init(entityId, xmppc_msg_hndl, &shmId)) != CMSRET_SUCCESS)
    {
        cmsLog_error("cmsMdm_init failed, ret=%d", ret);
        return ret;
    }

    cmsLog_notice("cmsMdm_init successful, shmId=%d", shmId);

    if ((ret = cmsTmr_init(&xmppc_tmr_hndl)) != CMSRET_SUCCESS)
    {
        cmsLog_error("cmsTmr_init failed, ret=%d", ret);
        return ret;
    }

    cmsLog_notice("cmsTmr_init successful");

    // Register XMPP interest events with smd.
    xmppc_register_event(CMS_MSG_XMPP_CONNECTION_ENABLE, TRUE, NULL, 0);
    xmppc_register_event(CMS_MSG_XMPP_CONNECTION_DELETE, TRUE, NULL, 0);
    xmppc_register_event(CMS_MSG_WAN_CONNECTION_UP, TRUE, NULL, 0);

    return ret;
}


static void cms_cleanup(void)
{
    // Unregister XMPP interest events with smd.
    xmppc_register_event(CMS_MSG_XMPP_CONNECTION_ENABLE, FALSE, NULL, 0);
    xmppc_register_event(CMS_MSG_XMPP_CONNECTION_DELETE, FALSE, NULL, 0);
    xmppc_register_event(CMS_MSG_WAN_CONNECTION_UP, FALSE, NULL, 0);

    cmsTmr_cleanup(&xmppc_tmr_hndl);
    cmsMdm_cleanup();
    cmsMsg_cleanup(&xmppc_msg_hndl);
}


static SINT32 xmppc_init(const CmsEntityId entityId)
{
    SINT32 rv = 0, i = 0;
    CmsRet ret = CMSRET_SUCCESS;

    /* Ignore broken pipes */
    signal(SIGPIPE, SIG_IGN);

    /* initialize the CMS framework */
    if ((ret = cms_init(entityId)) != CMSRET_SUCCESS)
    {
        cmsLog_error("CMS initialization failed (%d), exit.", ret);
        rv = -1;
    }

    // initialize remove jabberIDs list
    for (i = 0; i < XMPPC_MAX_NUMBER_OF_CONNECTIONS; i++)
        delete_jabberIDs[i] = NULL;

    return rv;
}


static void xmppc_exit(void)
{
    /* clean CMS resources */
    cms_cleanup();
}


SINT32 main(SINT32 argc, char **argv)
{
    SINT32 rv = 0;
    CmsEntityId entityId = EID_XMPPC;

    /* initialize CMS logging */
    cmsLog_init(entityId);

    cmsLog_debug("initializing %s...", *argv);

    rv = xmppc_init(entityId);

    if (!rv)
    {
        /* loop through XMPP connection table to
           bring up connections if any */
        xmppc_connect_all();

        /* xmppc main while loop */
        rv = xmppc_stack();
    }

    xmppc_exit();

    cmsLog_notice("exiting with code %d", rv);

    /* cleanup  CMS logging */
    cmsLog_cleanup();

    return rv;
}


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

#ifdef SUPPORT_XMPP

//#include <errno.h>
#include <pthread.h>
#include <strophe/strophe.h>

#include "cms_core.h"
#include "cms_msg.h"
#include "cms_qdm.h"
#include "cms_util.h"

#include "xmppc_conn.h"


/*
 * extern variables
 */

extern void *xmppc_msg_hndl;

/*
 * mutex, signal data for synchronization between
 * xmppc main and xmppc thread. They're used in
 * xmppc_main.c, xmppc_conn.c
 */
extern pthread_mutex_t comm_mutex;
extern int delete_list_is_empty;
extern char *delete_jabberIDs[XMPPC_MAX_NUMBER_OF_CONNECTIONS];


/*========================= PRIVATE FUNCTIONS ==========================*/


/* XMPP sends a message to TR69c asking it to establish a connection with
 * its predetermined ACS connection with "6 Connection Request"
 */
static CmsRet xmpp_send_request_msg_to_tr69c(void)
{
    CmsMsgHeader msg;
    CmsRet ret;

    memset(&msg, 0, sizeof(CmsMsgHeader));

    msg.type = CMS_MSG_XMPP_REQUEST_SEND_CONNREQUEST_EVENT;
    msg.src = EID_XMPPC;
    msg.dst = EID_TR69C;
    msg.flags_request = 1;

    pthread_mutex_lock(&comm_mutex);

    ret = cmsMsg_sendAndGetReply(xmppc_msg_hndl, &msg);
    if (ret != CMSRET_SUCCESS)
    {
       cmsLog_error("Request to send connection request event was denied, ret=%d", ret);
    }
    else
    {
       cmsLog_notice("Request to send connection request event was successful, ret=%d", ret);
    }

    pthread_mutex_unlock(&comm_mutex);

    return ret;
}


static CmsRet xmppc_update_conn_info
    (const char *jabberID,
     UBOOL8 connected)
{
    char   dateTimeBuf[BUFLEN_64];
    UBOOL8 found = FALSE;
    CmsRet ret = CMSRET_OBJECT_NOT_FOUND;
    Dev2XmppConnObject *xmppConn = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

    if ((ret = cmsLck_acquireLockWithTimeout(XMPPC_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
    {
       cmsLog_error("could not get lock, ret=%d", ret);
       return ret;
    }

    while ((!found) &&
           (cmsObj_getNext(MDMOID_DEV2_XMPP_CONN,
                           &iidStack,
                           (void **) &xmppConn) == CMSRET_SUCCESS))
    {
        if (cmsUtl_strcmp(jabberID, xmppConn->jabberID) == 0)
        {
            if (connected == TRUE)
            {
                CMSMEM_REPLACE_STRING(xmppConn->status, "Up");
                if (xmppConn->useTLS == TRUE)
                    xmppConn->TLSEstablished = TRUE;
                else
                    xmppConn->TLSEstablished = FALSE;
            }
            else
            {
                CMSMEM_REPLACE_STRING(xmppConn->status, "Down");
                xmppConn->TLSEstablished = FALSE;
            }

            cmsTms_getXSIDateTime(0, dateTimeBuf, sizeof(dateTimeBuf));
            CMSMEM_REPLACE_STRING(xmppConn->lastChangeDate, dateTimeBuf);

            if ((ret = cmsObj_setFlags(xmppConn,
                                       &iidStack,
                                       OSF_NO_RCL_CALLBACK |
                                       OSF_NO_ACCESSPERM_CHECK)) != CMSRET_SUCCESS)
            {
                cmsLog_error("Update xmppc connection with jabberID %s failed, ret=%d",
                             jabberID, ret);
            }

            found = TRUE;
        }

        cmsObj_free((void **) &xmppConn);
    }

    cmsLck_releaseLock();

    return ret;
}


static int xmppc_version_handler
    (xmpp_conn_t * const conn,
     xmpp_stanza_t * const stanza,
     void * const userdata)
{
    xmpp_stanza_t *reply, *query, *name, *version, *text;
    char *ns = NULL;
    xmpp_ctx_t *ctx = (xmpp_ctx_t*)userdata;
    cmsLog_notice("Received version request from %s", xmpp_stanza_get_attribute(stanza, "from"));

    reply = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(reply, "iq");
    xmpp_stanza_set_type(reply, "result");
    xmpp_stanza_set_id(reply, xmpp_stanza_get_id(stanza));
    xmpp_stanza_set_attribute(reply, "to", xmpp_stanza_get_attribute(stanza, "from"));

    query = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(query, "query");
    ns = xmpp_stanza_get_ns(xmpp_stanza_get_children(stanza));
    if (ns)
    {
        xmpp_stanza_set_ns(query, ns);
    }

    name = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(name, "name");
    xmpp_stanza_add_child(query, name);

    text = xmpp_stanza_new(ctx);
    xmpp_stanza_set_text(text, "xmppc");
    xmpp_stanza_add_child(name, text);

    version = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(version, "version");
    xmpp_stanza_add_child(query, version);

    text = xmpp_stanza_new(ctx);
    xmpp_stanza_set_text(text, "1.0");
    xmpp_stanza_add_child(version, text);

    xmpp_stanza_add_child(reply, query);

    xmpp_send(conn, reply);
    xmpp_stanza_release(reply);
    
    return 1;
}


static int xmppc_message_handler
    (xmpp_conn_t * const conn,
     xmpp_stanza_t * const stanza,
     void * const userdata)
{
    xmpp_stanza_t *reply, *body, *text;
    char *intext = NULL, *replytext = NULL;
    xmpp_ctx_t *ctx = (xmpp_ctx_t*)userdata;

    if (!xmpp_stanza_get_child_by_name(stanza, "body")) return 1;
    if (!strcmp(xmpp_stanza_get_attribute(stanza, "type"), "error")) return 1;

    intext = xmpp_stanza_get_text(xmpp_stanza_get_child_by_name(stanza, "body"));

    cmsLog_notice("Incoming message from %s: %s\n", xmpp_stanza_get_attribute(stanza, "from"), intext);

    reply = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(reply, "message");
    xmpp_stanza_set_type(reply, xmpp_stanza_get_type(stanza) ? xmpp_stanza_get_type(stanza) : "chat");
    xmpp_stanza_set_attribute(reply, "to", xmpp_stanza_get_attribute(stanza, "from"));

    body = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(body, "body");

    replytext = cmsMem_alloc(cmsUtl_strlen(" to you too!") + cmsUtl_strlen(intext) + 1, ALLOC_ZEROIZE);
    cmsUtl_strcpy(replytext, intext);
    cmsUtl_strcat(replytext, " to you too!");

    text = xmpp_stanza_new(ctx);
    xmpp_stanza_set_text(text, replytext);
    xmpp_stanza_add_child(body, text);
    xmpp_stanza_add_child(reply, body);

    xmpp_send(conn, reply);
    xmpp_stanza_release(reply);
    cmsMem_free(replytext);

    return 1;
}


/* define a handler for connection events */
static void xmppc_conn_handler
    (xmpp_conn_t * const conn,
     const xmpp_conn_event_t status,
     const int error,
     xmpp_stream_error_t * const stream_error,
     void * const userdata)
{
    xmpp_ctx_t *ctx = (xmpp_ctx_t *)userdata;

    if (status == XMPP_CONN_CONNECT)
    {
        xmpp_stanza_t* pres = xmpp_stanza_new(ctx);

        cmsLog_notice("DEBUG: connected\n");

        xmpp_handler_add(conn,
                         xmppc_version_handler,
                         "jabber:iq:version",
                         "iq",
                         NULL,
                         ctx);
        xmpp_handler_add(conn,
                         xmppc_message_handler,
                         NULL,
                         "message",
                         NULL,
                         ctx);

        xmpp_stanza_set_name(pres, "presence");
        /* Send initial <presence/> to appear online to contacts */
        xmpp_send(conn, pres);
        xmpp_stanza_release(pres);

        /* Update XMPPC connection status */
        xmppc_update_conn_info(xmpp_conn_get_jid(conn), TRUE);
    }
    else
    {
        cmsLog_notice("DEBUG: disconnected\n");
        xmpp_stop(ctx);

        /* Update XMPPC connection status */
        xmppc_update_conn_info(xmpp_conn_get_jid(conn), FALSE);
    }
}


/* define a handler for acs connection request */
static void xmppc_acs_conn_handler
    (xmpp_conn_t * const conn,
     const xmpp_conn_event_t status,
     const int error,
     xmpp_stream_error_t * const stream_error,
     void * const userdata)
{
    xmpp_ctx_t *ctx = (xmpp_ctx_t *)userdata;

    if (status == XMPP_CONN_CONNECT)
    {
        cmsLog_notice("DEBUG: connected\n");

        xmpp_handler_add(conn,
                         xmppc_version_handler,
                         "jabber:iq:version",
                         "iq",
                         NULL,
                         ctx);
        xmpp_handler_add(conn,
                         xmppc_message_handler,
                         NULL,
                         "message",
                         NULL,
                         ctx);

#if 1
        xmpp_stanza_t *stz_pres;
        xmpp_stanza_t *stz_txt_uname, *stz_txt_pwd;
        xmpp_stanza_t *stz_conn_uname, *stz_conn_pwd;
        xmpp_stanza_t *stz_conn_req;
        xmpp_stanza_t *stz_msg, *stz_body;
        char url[BUFLEN_256];
        char mngrServerConnReqUsername[BUFLEN_256];
        char mngrServerConnReqPwd[BUFLEN_256];
        char mngrServerUsername[BUFLEN_256];
        char mngrServerPwd[BUFLEN_256];
        char connectionPath[BUFLEN_256];
        char allowedJabberIds[BUFLEN_1024]; /* smaller than allowed */
        char connReqJabberId[BUFLEN_256];
        CmsRet ret;

        stz_pres = xmpp_stanza_new(ctx);
        xmpp_stanza_set_name(stz_pres, "presence");
        /* Send initial <presence/> to appear online to contacts */
        xmpp_send(conn, stz_pres);
        xmpp_stanza_release(stz_pres);

        /* retrieve username and password from  ManagementServer.Username/Password */
        if ((ret = cmsLck_acquireLockWithTimeout(XMPPC_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
        {
           cmsLog_error("could not get lock, ret=%d", ret);
           strcpy(mngrServerUsername,"admin");
           strcpy(mngrServerPwd,"password");
        }
        if ((ret = qdmTr69c_getManagementServerCfgLocked(url,mngrServerConnReqUsername,mngrServerConnReqPwd,mngrServerUsername,mngrServerPwd)) != CMSRET_SUCCESS)
        {
           cmsLog_error("could not get managementServer CFG info, ret=%d", ret);
           strcpy(mngrServerUsername,"admin");
           strcpy(mngrServerPwd,"password");
        }

        stz_txt_uname = xmpp_stanza_new(ctx);
        xmpp_stanza_set_text(stz_txt_uname, mngrServerConnReqUsername);

        stz_conn_uname = xmpp_stanza_new(ctx);
        xmpp_stanza_set_name(stz_conn_uname, "username");
        xmpp_stanza_add_child(stz_conn_uname, stz_txt_uname);

        stz_txt_pwd = xmpp_stanza_new(ctx);
        xmpp_stanza_set_text(stz_txt_pwd, mngrServerConnReqPwd);

        stz_conn_pwd = xmpp_stanza_new(ctx);
        xmpp_stanza_set_name(stz_conn_pwd, "password");
        xmpp_stanza_add_child(stz_conn_pwd, stz_txt_pwd);

        stz_conn_req = xmpp_stanza_new(ctx);
        xmpp_stanza_set_name(stz_conn_req, "connectionRequest");
        xmpp_stanza_set_ns(stz_conn_req, "urn:broadband-forum-org:xmpp:connectionRequest-1-0");
        xmpp_stanza_add_child(stz_conn_req, stz_conn_uname);
        xmpp_stanza_add_child(stz_conn_req, stz_conn_pwd);

        stz_body = xmpp_stanza_new(ctx);
        xmpp_stanza_set_name(stz_body, "body");
        xmpp_stanza_add_child(stz_body, stz_conn_req);

        stz_msg = xmpp_stanza_new(ctx);
        xmpp_stanza_set_name(stz_msg, "message");

        if ((ret = qdmTr69c_getManagementServerXmppCfgLocked(connectionPath,allowedJabberIds,connReqJabberId)) == CMSRET_SUCCESS)
        {
           /* conn->jid is essentially the same as one of the allowedJabberIds; or the jid with acs */
           xmpp_stanza_set_attribute(stz_msg, "from", xmpp_conn_get_jid(conn));
           xmpp_stanza_set_attribute(stz_msg, "to", connReqJabberId);
        }
        cmsLck_releaseLock();
        if (ret != CMSRET_SUCCESS)
        {
           xmpp_stanza_set_attribute(stz_msg, "from", xmpp_conn_get_jid(conn));
           xmpp_stanza_set_attribute(stz_msg, "to", "cpe@mouse-T3500");
        }
        xmpp_stanza_set_id(stz_msg, "cr001");
        xmpp_stanza_set_type(stz_msg, "chat");
        xmpp_stanza_add_child(stz_msg, stz_body);

        xmpp_send(conn, stz_msg);
        xmpp_stanza_release(stz_msg);
#else
        xmpp_stanza_t* stz_pres;
        xmpp_stanza_t *stz_txt_uname, *stz_txt_pwd;
        xmpp_stanza_t *stz_conn_uname, *stz_conn_pwd;
        xmpp_stanza_t *stz_iq, *stz_conn_req;

        stz_pres = xmpp_stanza_new(ctx);
        xmpp_stanza_set_name(stz_pres, "presence");
        /* Send initial <presence/> to appear online to contacts */
        xmpp_send(conn, stz_pres);
        xmpp_stanza_release(stz_pres);

        stz_txt_uname = xmpp_stanza_new(ctx);
        xmpp_stanza_set_text(stz_txt_uname, "admin");

        stz_conn_uname = xmpp_stanza_new(ctx);
        xmpp_stanza_set_name(stz_conn_uname, "username");
        xmpp_stanza_add_child(stz_conn_uname, stz_txt_uname);

        stz_txt_pwd = xmpp_stanza_new(ctx);
        xmpp_stanza_set_text(stz_txt_pwd, "admin");

        stz_conn_pwd = xmpp_stanza_new(ctx);
        xmpp_stanza_set_name(stz_conn_pwd, "password");
        xmpp_stanza_add_child(stz_conn_pwd, stz_txt_pwd);

        stz_conn_req = xmpp_stanza_new(ctx);
        xmpp_stanza_set_name(stz_conn_req, "connectionRequest");
        xmpp_stanza_set_ns(stz_conn_req, "urn:broadband-forum-org:xmpp:connectionRequest-1-0");
        xmpp_stanza_add_child(stz_conn_req, stz_conn_uname);
        xmpp_stanza_add_child(stz_conn_req, stz_conn_pwd);

        stz_iq = xmpp_stanza_new(ctx);
        xmpp_stanza_set_name(stz_iq, "iq");
        xmpp_stanza_set_attribute(stz_iq, "from", xmpp_conn_get_jid(conn));
        xmpp_stanza_set_attribute(stz_iq, "to", "cpe@mouse-T3500");
        xmpp_stanza_set_id(stz_iq, "cr001");
        xmpp_stanza_set_type(stz_iq, "get");
        xmpp_stanza_add_child(stz_iq, stz_conn_req);

        xmpp_send(conn, stz_iq);
        xmpp_stanza_release(stz_iq);
#endif

        /* Update XMPPC connection status */
        xmppc_update_conn_info(xmpp_conn_get_jid(conn), TRUE);
    }
    else
    {
        cmsLog_notice("DEBUG: disconnected\n");
        xmpp_stop(ctx);

        /* Update XMPPC connection status */
        xmppc_update_conn_info(xmpp_conn_get_jid(conn), FALSE);
    }
}


static void xmppc_do_conn_req
    (xmpp_conn_t * const conn,
     xmpp_stanza_t * const stanza,
     xmpp_stanza_t * const stz_conn_req,
     void * const userdata)
{
    char *pInText = NULL;
    char username[BUFLEN_256], pwd[BUFLEN_256];
    char mngrServerConnReqUsername[BUFLEN_256] = {0};
    char mngrServerConnReqPassword[BUFLEN_256] = {0};
    char mngrServerUsername[BUFLEN_256] = {0};
    char mngrServerPassword[BUFLEN_256] = {0};
    char url[BUFLEN_256] = {0};
    CmsRet ret;
    UBOOL8 hasError = FALSE;
    xmpp_stanza_t *stz_resp;
    xmpp_ctx_t *ctx = (xmpp_ctx_t*)userdata;

    if (xmpp_stanza_get_child_by_name(stz_conn_req, "username") &&
        xmpp_stanza_get_child_by_name(stz_conn_req, "password"))
    {
        pInText = xmpp_stanza_get_text(xmpp_stanza_get_child_by_name(stz_conn_req, "username"));
        cmsUtl_strcpy(username, pInText);
        pInText = xmpp_stanza_get_text(xmpp_stanza_get_child_by_name(stz_conn_req, "password"));
        cmsUtl_strcpy(pwd, pInText);

        /* TR69 requirement: username/pwd in this stanza must match ManagementServer.ConnectionRequestUsername/Password */
        if ((ret = cmsLck_acquireLockWithTimeout(XMPPC_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
        {
           cmsLog_error("could not get lock, ret=%d", ret);
           hasError = TRUE;
        }
        if ((ret = qdmTr69c_getManagementServerCfgLocked(url,mngrServerConnReqUsername,mngrServerConnReqPassword,mngrServerUsername,mngrServerPassword)) != CMSRET_SUCCESS)
        {
           cmsLog_error("could not get management server configuration, ret=%d", ret);
           hasError = TRUE;
        }
        cmsLck_releaseLock();
        if (strcmp(username, mngrServerConnReqUsername) != 0)
        {
           cmsLog_error("username comparison fails %s", username);
           hasError = TRUE;
        }
        if (strcmp(pwd, mngrServerConnReqPassword) != 0)
        {
           cmsLog_error("password comparison fails %s", pwd);
           hasError = TRUE;
        }
    }
    else
    {
        hasError = TRUE;
    }

    if (hasError == FALSE)
    {
       /* send message to tr69c client to send connReq to ACS */
       if (xmpp_send_request_msg_to_tr69c() != CMSRET_SUCCESS)
       {
          hasError = TRUE;
       }
    }
    if (hasError == FALSE)
    {
        // create response message with empty body
        stz_resp = xmpp_stanza_new(ctx);
        xmpp_stanza_set_name(stz_resp, "iq");
        xmpp_stanza_set_type(stz_resp, "result");
        xmpp_stanza_set_id(stz_resp, xmpp_stanza_get_id(stanza));
        xmpp_stanza_set_attribute(stz_resp, "to", xmpp_stanza_get_attribute(stanza, "from"));
    }
    else
    {
        xmpp_stanza_t *stz_conn_req, *stz_error, *stz_not_auth;

        stz_conn_req = xmpp_stanza_new(ctx);
        xmpp_stanza_set_name(stz_conn_req, "connectionRequest");
        xmpp_stanza_set_ns(stz_conn_req, "urn:broadband-forum-org:xmpp:connectionRequest-1-0");

        stz_not_auth = xmpp_stanza_new(ctx);
        xmpp_stanza_set_name(stz_not_auth, "not-authorized");
        xmpp_stanza_set_ns(stz_not_auth, "urn:ietf:params:xml:ns:xmpp-stanzas");

        stz_error = xmpp_stanza_new(ctx);
        xmpp_stanza_set_name(stz_error, "error");
        xmpp_stanza_set_type(stz_error, "cancel");
        xmpp_stanza_add_child(stz_error, stz_not_auth);

        // create response message with error body
        stz_resp = xmpp_stanza_new(ctx);
        xmpp_stanza_set_name(stz_resp, "iq");
        xmpp_stanza_set_attribute(stz_resp, "from", xmpp_conn_get_jid(conn));
        xmpp_stanza_set_attribute(stz_resp, "to", xmpp_stanza_get_attribute(stanza, "from"));
        xmpp_stanza_set_id(stz_resp, xmpp_stanza_get_id(stanza));
        xmpp_stanza_set_type(stz_resp, "error");
        xmpp_stanza_add_child(stz_resp, stz_conn_req);
        xmpp_stanza_add_child(stz_resp, stz_error);
    }

    xmpp_send(conn, stz_resp);
    xmpp_stanza_release(stz_resp);
}


static int xmppc_conn_req_handler
    (xmpp_conn_t * const conn,
     xmpp_stanza_t * const stanza,
     void * const userdata)
{
    xmpp_stanza_t *stz_body, *stz_conn_req;

    if (!xmpp_stanza_get_attribute(stanza, "from")) return 1;
    cmsLog_notice("Received connection request from %s", xmpp_stanza_get_attribute(stanza, "from"));

    stz_body = xmpp_stanza_get_child_by_name(stanza, "body");
    if (stz_body == NULL) return 1;
 
    stz_conn_req = xmpp_stanza_get_child_by_name(stz_body, "connectionRequest");
    if (stz_conn_req == NULL)
    {
        return xmppc_message_handler(conn, stanza, userdata);
    }

    xmppc_do_conn_req(conn, stanza, stz_conn_req, userdata);

    return 1;
}


/* define a handler for cpe connection request response */
static void xmppc_cpe_conn_handler
    (xmpp_conn_t * const conn,
     const xmpp_conn_event_t status,
     const int error,
     xmpp_stream_error_t * const stream_error,
     void * const userdata)
{
    xmpp_ctx_t *ctx = (xmpp_ctx_t *)userdata;

    if (status == XMPP_CONN_CONNECT)
    {
        xmpp_stanza_t* stz_pres;

        cmsLog_notice("DEBUG: connected\n");

        xmpp_handler_add(conn,
                         xmppc_version_handler,
                         "jabber:iq:version",
                         "iq",
                         NULL,
                         ctx);
        xmpp_handler_add(conn,
                         xmppc_conn_req_handler,
                         NULL,
                         "message",
                         NULL,
                         ctx);

        stz_pres = xmpp_stanza_new(ctx);
        xmpp_stanza_set_name(stz_pres, "presence");
        /* Send initial <presence/> to appear online to contacts */
        xmpp_send(conn, stz_pres);
        xmpp_stanza_release(stz_pres);

        /* Update XMPPC connection status */
        xmppc_update_conn_info(xmpp_conn_get_jid(conn), TRUE);
    }
    else
    {
        cmsLog_notice("DEBUG: disconnected\n");
        xmpp_stop(ctx);

        /* Update XMPPC connection status */
        xmppc_update_conn_info(xmpp_conn_get_jid(conn), FALSE);
    }
}


static int xmppc_iop_conn_req_handler
    (xmpp_conn_t * const conn,
     xmpp_stanza_t * const stanza,
     void * const userdata)
{
    xmpp_stanza_t *stz_conn_req;

    if (!xmpp_stanza_get_attribute(stanza, "from")) return 1;
    cmsLog_notice("Received iq from %s", xmpp_stanza_get_attribute(stanza, "from"));

    stz_conn_req = xmpp_stanza_get_child_by_name(stanza, "connectionRequest");
    if (stz_conn_req != NULL)
    {
        xmppc_do_conn_req(conn, stanza, stz_conn_req, userdata);
    }
    else
    {
        cmsLog_error("Received iq from %s without connectionRequest", xmpp_stanza_get_attribute(stanza, "from"));
    }

    return 1;
}


/* define a handler for cpe-iop connection request response */
static void xmppc_cpe_iop_conn_handler
    (xmpp_conn_t * const conn,
     const xmpp_conn_event_t status,
     const int error,
     xmpp_stream_error_t * const stream_error,
     void * const userdata)
{
    xmpp_ctx_t *ctx = (xmpp_ctx_t *)userdata;

    if (status == XMPP_CONN_CONNECT)
    {
        xmpp_stanza_t* stz_pres;

        cmsLog_notice("DEBUG: connected\n");

        xmpp_handler_add(conn,
                         xmppc_iop_conn_req_handler,
                         NULL,
                         "iq",
                         NULL,
                         ctx);

        stz_pres = xmpp_stanza_new(ctx);
        xmpp_stanza_set_name(stz_pres, "presence");
        /* Send initial <presence/> to appear online to contacts */
        xmpp_send(conn, stz_pres);
        xmpp_stanza_release(stz_pres);

        /* Update XMPPC connection status */
        xmppc_update_conn_info(xmpp_conn_get_jid(conn), TRUE);
    }
    else
    {
        cmsLog_notice("DEBUG: disconnected\n");
        xmpp_stop(ctx);

        /* Update XMPPC connection status */
        xmppc_update_conn_info(xmpp_conn_get_jid(conn), FALSE);
    }
}


static void xmppc_conn_init
    (xmpp_ctx_t * const ctx,
     xmpp_conn_t * const conn,
     const PXMPP_THREAD_DATA xmpp_data)
{
    /* setup authentication information */
    xmpp_conn_set_jid(conn, xmpp_data->jabberID);
    xmpp_conn_set_pass(conn, xmpp_data->password);

    /* initiate connection */
    if (strstr(xmpp_data->jabberID, "cpe-iop") != NULL)
       xmpp_connect_client(conn, xmpp_data->serverAddress, xmpp_data->serverPort, xmppc_cpe_iop_conn_handler, ctx);
    else if (strstr(xmpp_data->jabberID, "cpe") != NULL)
       xmpp_connect_client(conn, xmpp_data->serverAddress, xmpp_data->serverPort, xmppc_cpe_conn_handler, ctx);
    else if (strstr(xmpp_data->jabberID, "acs") != NULL)
       xmpp_connect_client(conn, xmpp_data->serverAddress, xmpp_data->serverPort, xmppc_acs_conn_handler, ctx);
    else
       xmpp_connect_client(conn, xmpp_data->serverAddress, xmpp_data->serverPort, xmppc_conn_handler, ctx);
}


/*========================= PUBLIC FUNCTIONS ==========================*/


void xmppc_conn_stack(void *pData)
{
    char jid[BUFLEN_256];
    SINT32 commFd = 0, rv = 0, i = 0;
    fd_set readFds;
    struct timeval tm;
    UBOOL8 running = TRUE;
    xmpp_log_t *log = NULL;
    xmpp_ctx_t *ctx = NULL;
    xmpp_conn_t *conn = NULL;
    PXMPP_THREAD_DATA xmpp_data = (PXMPP_THREAD_DATA)pData;

    cmsLog_notice("Enter client thread for XMPP connection with jabberID='%s', password='%s', serverAddr =%s, serverPort %d", 
                  xmpp_data->jabberID, xmpp_data->password, xmpp_data->serverAddress, xmpp_data->serverPort);

    cmsUtl_strcpy(jid, xmpp_data->jabberID);

    /* init XMPP library */
    xmpp_initialize();

    /* create a XMPP context */
    log = xmpp_get_default_logger(XMPP_LEVEL_DEBUG); /* pass NULL instead to silence output */

    ctx = xmpp_ctx_new(NULL, log);

    /* create a connection */
    conn = xmpp_conn_new(ctx);

    /* initiate connection */
    xmppc_conn_init(ctx, conn, xmpp_data);

    while (running)
    {
        tm.tv_sec = XMPPC_SCHED_TIMEOUT_IN_MSECS / MSECS_IN_SEC;
        tm.tv_usec = XMPPC_SCHED_TIMEOUT_IN_MSECS * USECS_IN_MSEC;
        cmsLog_debug("set select timeout to %ld.%ld secs", tm.tv_sec, (tm.tv_usec / USECS_IN_MSEC));

        /* set up all the fd stuff for select */
        FD_ZERO(&readFds);
        FD_SET(commFd, &readFds);

        /* use select as timer since commFd is 0 */
        rv = select(commFd+1, &readFds, NULL, NULL, &tm);
        if (rv < 0)
        {
            /* interrupted by signal or something, continue */
            rv = 0;

            continue;
        }

        /* send any data that has been queued by xmpp_send
         * and related functions and run through the Strophe
         * even loop a single time, and will not wait more
         * than timeout milliseconds for events.
         */
        xmpp_run_once(ctx, 1 * MSECS_IN_SEC);

        pthread_mutex_lock(&comm_mutex);

        /* should thread continue to run? */
        /* only check if remove list is NOT empty */
        if (delete_list_is_empty == FALSE)
        {
            /* assume remove list is empty before checking */
            delete_list_is_empty = TRUE;
            for (i = 0; i < XMPPC_MAX_NUMBER_OF_CONNECTIONS; i++)
            {
                if (delete_jabberIDs[i] != NULL)
                {
                    /* is jabberID matched with any jabberID in remove list? */
                    if (cmsUtl_strcmp(delete_jabberIDs[i], jid) == 0)
                    {
                        CMSMEM_FREE_BUF_AND_NULL_PTR(delete_jabberIDs[i]);
                        /* exit while loop by making running be FALSE */
                        running = FALSE;
                    }
                    /* otherwise mark remove list is not empty */
                    else
                    {
                        delete_list_is_empty = FALSE;
                    }
                } 
            }
        }

        pthread_mutex_unlock(&comm_mutex);
    }

    cmsLog_notice("Exit client thread for XMPP connection with jabberID='%s', password='%s'",
        xmpp_conn_get_jid(conn), xmpp_conn_get_pass(conn));

    /* release our connection and context */
    xmpp_conn_release(conn);
    xmpp_ctx_free(ctx);

    /* final shutdown of the library */
    xmpp_shutdown();

    pthread_exit(0); /* exit */
} 


#endif    // SUPPORT_XMPP



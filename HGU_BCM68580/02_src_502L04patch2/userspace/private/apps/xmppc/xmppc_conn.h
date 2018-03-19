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

#ifndef __XMPPC_CONN_H__
#define __XMPPC_CONN_H__

/*!\file xmppc_conn.h
 * Main functions for XMPPC connections.
 *
 */


#include "cms.h"

#define XMPP_BROADBAND_FORUM_NS    "urn:broadband-forum-org:cwmp:xmppConnReq-1-0"
#define XMPP_IETF_NS               "urn:ietf:params:xml:ns:xmpp-stanzas"
#define XMPP_ACS_IDENTITY          "acsServer"
#define XMPP_TYPE_GET              "get"
#define XMPP_TYPE_RESULT           "result"
#define XMPP_TYPE_ERROR            "error"
#define XMPP_CONN_REQUEST_RESPONSE_ERROR_TYPE  "cancel"
#define XMPP_CONN_REQUEST_RESPONSE_SUCCESS               0
#define XMPP_CONN_REQUEST_RESPONSE_ERROR_NO_SERVICE      1
#define XMPP_CONN_REQUEST_RESPONSE_ERROR_NOT_AUTHORIZIED 2
#define XMPP_CONN_REQUEST_NO_SERVICE_STR       "service-unavailable"
#define XMPP_CONN_REQUEST_NO_SERVICE_CODE      503
#define XMPP_CONN_REQUEST_NOT_AUTHORIZED_STR   "not-authorized"
#define XMPPC_SCHED_TIMEOUT_IN_MSECS    100
#define XMPPC_LOCK_TIMEOUT              5*MSECS_IN_SEC

// XMPPC_NUMBER_OF_CONNECTIONS should match with
// MAX_XMPP_CONN_ENTRY defined in cgi_xmpp_conn.c
#define XMPPC_MAX_NUMBER_OF_CONNECTIONS 32   

typedef struct XMPP_THDATA
{
    char jabberID[BUFLEN_1024];
    char password[BUFLEN_256];
    char serverAddress[BUFLEN_256];
    UINT32 serverPort;
} XMPP_THREAD_DATA, *PXMPP_THREAD_DATA;


void xmppc_conn_stack(void *pData);

#endif  // __OMCI_PM_H__

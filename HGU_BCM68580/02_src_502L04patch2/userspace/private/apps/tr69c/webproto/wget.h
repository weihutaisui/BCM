/***********************************************************************
 *
 *  Copyright (c) 2005-2010  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2011:proprietary:standard

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

#ifndef _WGET_H_
#define _WGET_H_

#include "protocol.h"
//#include "main/event.h"
#include "cms_tmr.h"
#include "cms_strconv.h"  // for UrlProto

/*----------------------------------------------------------------------*/
typedef enum
{
   iWgetStatus_Ok = 0,
   iWgetStatus_InternalError,
   iWgetStatus_ConnectionError,
   iWgetStatus_Error,
   iWgetStatus_HttpError
} tWgetStatus;

typedef struct
{
   tWgetStatus status;
   tProtoCtx   *pc;
   tHttpHdrs   *hdrs;
   const char  *msg;  /* Msg associated with status */
   void        *handle;
} tWget;

typedef enum
{
   eCloseConnection=0,
   eKeepConnectionOpen  /* used by wConnect and wClose */
} tConnState;

typedef enum
{
   eUndefined,
   eConnect,
   ePostData,
   eGetData,
   ePutData,
   eDisconnect
} tRequest;

typedef struct XtraPostHdr
{
   struct XtraPostHdr *next;
   char               *hdr;   /* header string */
   char               *value; /* value string*/
} XtraPostHdr;

typedef struct
{
   tConnState        keepConnection;
   int               status;
   tRequest          request;
   int               cbActive; /* set to 1 if callback from report status */
   tProtoCtx         *pc;
   CmsEventHandler   cb;
   void              *handle;
   UrlProto          urlProto;
   char              *host;
   struct sockaddr_storage  host_addr;
   unsigned short    port;
   char              *uri;
   tHttpHdrs         *hdrs;
   CookieHdr         *cookieHdrs;
   XtraPostHdr       *xtraPostHdrs;
   char              *content_type;
   char              *postdata;
   int               datalen;
} tWgetInternal;

/*----------------------------------------------------------------------*
 * returns
 *   0 if sending request succeded
 *  -1 on URL syntax error
 *
 * The argument to the callback is of type (tWget *)
 */
tWgetInternal *wget_Connect(const char *url, CmsEventHandler callback, void *handle);
int wget_GetData(tWgetInternal *wg, CmsEventHandler callback, void *handle);
int wget_PostData(tWgetInternal *,char *data, int datalen, char *contenttype,
                  CmsEventHandler callback, void *handle);
int wget_PostDataClose(tWgetInternal *,char *data, int datalen, char *contenttype,
                       CmsEventHandler callback, void *handle);
int wget_PutData(tWgetInternal *,char *data, int datalen, char *contenttype, CmsEventHandler callback, void *handle);
int wget_Disconnect(tWgetInternal *);
const char *wget_LastErrorMsg(void);
int wget_AddPostHdr(tWgetInternal *wio, char *xhdrname, char *value);
void wget_ClearPostHdrs(tWgetInternal *wio);
#endif


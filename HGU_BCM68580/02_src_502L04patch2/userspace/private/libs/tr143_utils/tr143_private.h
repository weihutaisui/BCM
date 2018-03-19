/***********************************************************************
 *
 *  Copyright (c) 2012-2013  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2013:proprietary:standard

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

#ifndef _TR143_PRIVATE_H_
#define _TR143_PRIVATE_H_


/*
 * These are the definitions used within the tr143_utils library.
 */

#include "tr143_defs.h"


typedef enum {
   iUnknown,
   iNormal,
   i__Last
} tPostCtxType;

typedef struct {
   tPostCtxType   type;
   int            fd;      /* filedescriptor */
   void           *data;
} tProtoCtx;

typedef struct CookieHdr {
   struct CookieHdr *next;
   char	*name;
   char	*value;
} CookieHdr;

/*--------------------*/
typedef struct {
   /* common */
   char *content_type;
   char *protocol;
   char *wwwAuthenticate;
   char *Authorization;
   char *TransferEncoding;
   char *Connection;
   /* request */
   char *method;
   char *path;
   char *host;
   int  port;
   int  content_length;

   /* result */
   int  status_code;
   CookieHdr	*setCookies;
   char *message;
   char *locationHdr;		/* from 3xx status response */

   /* request derived */
   //  tIpAddr addr;  /* IP-address of communicating entity */
   // tZone zone;    /* zone in which communicating entity is */
   char *filename;
   char *arg;
} tHttpHdrs;


//common.c
int open_conn_socket(struct sockaddr_in *s_in);
int safe_strtoul(char *arg, unsigned long* value);
int select_with_timeout(int fd, int flag);
size_t read_with_timeout(int fd, char *buf, int len);
int get_if_stats(unsigned int *rx, unsigned int *tx);
void set_sockopt_nocopy(int fd);


//proto.c
int parseUrl(const char *url, char *proto, char *host, int *port, char *uri);
int proto_Writen(tProtoCtx *pc, const char *ptr, int nbytes);
int proto_SendRequest(tProtoCtx *pc, const char *method, const char *url);
void proto_SendHeader(tProtoCtx *pc,  const char *header, const char *value);
tHttpHdrs *proto_NewHttpHdrs();
int proto_ParseResponse(tProtoCtx *pc, tHttpHdrs *hdrs);
int proto_Skip(tProtoCtx *pc);
tProtoCtx *proto_NewCtx(int fd);
void proto_FreeCtx(tProtoCtx *pc);
int proto_Readn(tProtoCtx *pc, char *ptr, int nbytes);
int proto_Readline(tProtoCtx *pc, char *buf, int maxlen);
void proto_FreeHttpHdrs(tHttpHdrs *p);
void proto_SendRaw(tProtoCtx *pc, const char *arg, int len);
int proto_ParseHdrs(tProtoCtx *pc, tHttpHdrs *hdrs);


#endif  /* _TR143_PRIVATE_H_ */

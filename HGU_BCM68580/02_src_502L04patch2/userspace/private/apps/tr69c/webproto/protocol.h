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

#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#ifdef USE_SSL
#include <openssl/ssl.h>
#endif

/*----------------------------------------------------------------------*
 * typedefs
 */

typedef enum {
  iZone_Unknown,
  iZone_Lan,
  iZone_Ihz
} tZone;



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
  tZone zone;    /* zone in which communicating entity is */
  char *filename;
  char *arg;
} tHttpHdrs;

typedef void (*tProtoHandler)(void *, int lth);

typedef enum {
    sslRead,
    sslWrite
} tSSLIO;
/*--------------------*/
typedef enum {
  iUnknown,
  iNormal,
#ifdef USE_SSL
  iSsl,
#endif
  i__Last
} tPostCtxType;

/*--------------------*/
typedef struct {
   tPostCtxType   type;
   int            fd;      /* filedescriptor */

   /* internal use */
#ifdef USE_SSL
   SSL            *ssl;
   int            sslConn;
#endif
   tProtoHandler  cb;
   void           *data;
} tProtoCtx;


/* convenient naming */
#define fdgets   proto_Readline
#define fdprintf proto_Printline

#define PROTO_OK                0
#define PROTO_ERROR            -1
#define PROTO_ERROR_SSL        -2

/*----------------------------------------------------------------------*/
extern void proto_Init(void);

extern tHttpHdrs *proto_NewHttpHdrs(void);
extern void proto_FreeHttpHdrs(tHttpHdrs *p);

extern tProtoCtx *proto_NewCtx(int fd);
#ifdef USE_SSL
extern void proto_SetSslCtx(tProtoCtx *pc, tProtoHandler cb, void *data);
#endif
extern void proto_FreeCtx(tProtoCtx *pc);

extern int  proto_ReadWait(tProtoCtx *pc, char *ptr, int nbytes);
extern int  proto_Readn(tProtoCtx *pc, char *ptr, int nbytes);
extern int  proto_Writen(tProtoCtx *pc, const char *ptr, int nbytes);
extern int  proto_Readline(tProtoCtx *pc, char *ptr, int maxlen);
extern void proto_Printline(tProtoCtx *pc, const char *fmt, ...);
extern int	proto_Skip(tProtoCtx *pc);
extern int  proto_SSL_IO(tSSLIO iofunc, tProtoCtx *pc, char *ptr, int nbytes, tProtoHandler cb, void *data);
extern void proto_SendRequest(tProtoCtx *pc, const char *method, const char *url);
void proto_SendCookie(tProtoCtx *pc, CookieHdr *c);
extern void proto_SendHeader(tProtoCtx *pc,  const char *header, const char *value);
extern void proto_SendEndHeaders(tProtoCtx *pc);
extern void proto_SendRaw(tProtoCtx *pc, const char *arg, int len);
extern void proto_SendHeaders(tProtoCtx *pc, int status, const char* title, const char* extra_header, const char* content_type);

extern void proto_SendRedirect(tProtoCtx *pc, const char *host, const char* location);
extern void proto_SendRedirectProtoHost(tProtoCtx *pc, const char *protohost, const char* location);
extern void proto_SendRedirectViaRefresh(tProtoCtx *pc, const char *host, const char* location);
extern void proto_SendError(tProtoCtx *pc, int status, const char* title, const char* extra_header, const char* text);

extern int  proto_ParseResponse(tProtoCtx *pc, tHttpHdrs *hdrs);
extern int  proto_ParseRequest(tProtoCtx *pc, tHttpHdrs *hdrs);
extern void proto_ParseHdrs(tProtoCtx *pc, tHttpHdrs *hdrs);
extern void proto_ParsePost(tProtoCtx *pc, tHttpHdrs *hdrs);

#endif

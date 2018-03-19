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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <signal.h>
#include <ctype.h>
#include <syslog.h>

#include "cms_util.h"
#include "inc/appdefs.h"
#include "inc/utils.h"
#ifdef USE_SSL
    #include <openssl/ssl.h>
#endif
#include "main/event.h"

#include "protocol.h"
#include "www.h"
#include "wget.h"
#include "main/httpProto.h"
#include "bcmLibIF/bcmWrapper.h"


#define BUF_SIZE 1024
//#define DEBUG

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
#ifdef  DEBUG
    #define mkstr(S) # S
    #define setListener(A,B,C) {fprintf(stderr,mkstr(%s setListener B fd=%d\n), getticks(), A);\
setListener( A,B,C);}

    #define setListenerType(A,B,C,E) {fprintf(stderr,mkstr(%s setListenerType B-E fd=%d\n), getticks(), A);\
setListenerType( A,B,C,E);}

    #define stopListener(A) {fprintf(stderr,"%s stopListener fd=%d\n", getticks(), A);\
stopListener( A );}

static char timestr[40];
static char *getticks()
{
    struct timeval now;
    gettimeofday( &now,NULL);
    sprintf(timestr,"%04ld.%06ld", now.tv_sec%1000, now.tv_usec);
    return timestr;
}
#endif
/*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

/*----------------------------------------------------------------------*
 * forwards
 *----------------------------------------------------------------------*/
static void timer_connect(void *p);
static void do_connect(void *p);
#ifdef USE_SSL
static void do_connect_ssl(void *p, int errorcode);
#endif
static void timer_response(void *p);
static void do_response(void *p);
static void do_send_request(void *p, int errorcode);
static char noHostConnectMsg[] = "Could not establish connection to host %s(%s):%d";
static char noHostResolve[] = "Could not resolve host %s";
static char lastErrorMsg[255];

static void freeHdrs( XtraPostHdr **p )
{
	XtraPostHdr *next = *p;
        while( next ){
            XtraPostHdr *temp;
            temp = next->next;
            cmsMem_free(next->hdr);
            cmsMem_free(next->value);
            cmsMem_free(next);
            next = temp;
        }
}

static void freeCookies( CookieHdr **p )
{
	CookieHdr *next = *p;
	while( next ){
		CookieHdr *temp;
		temp = next->next;
		cmsMem_free(next->name);
		cmsMem_free(next->value);
		cmsMem_free(next);
		next = temp;
	}
}
/*----------------------------------------------------------------------*/
static void freeData(tWgetInternal *wg)
{
    if (wg != NULL) {
        if (wg->pc != NULL) {
            proto_FreeCtx(wg->pc);
            wg->pc = NULL;
        }
        cmsMem_free(wg->host);
        cmsMem_free(wg->uri);
        if (wg->hdrs != NULL) 
            proto_FreeHttpHdrs(wg->hdrs);
        freeCookies( &wg->cookieHdrs );
        freeHdrs( &wg->xtraPostHdrs );
        // don't free content_type and postdata since they're only pointers, there are no strdup for them.
        cmsMem_free(wg);
    }
}

typedef enum {
	REPLACE,
	ADDTOLIST
} eHdrOp;

static int addCookieHdr( CookieHdr **hdrQp, char *cookieName, char *value, eHdrOp replaceDups)
{
    CookieHdr *xh;
    CookieHdr **p = hdrQp;
    xh= (CookieHdr *)cmsMem_alloc(sizeof(struct CookieHdr), ALLOC_ZEROIZE);
    if (xh) {
        xh->name = cmsMem_strdup(cookieName);
        xh->value = cmsMem_strdup(value);
		if (replaceDups == REPLACE) {
			while (*p) {
				CookieHdr *xp = *p;
				if ( strcasecmp(xp->name, xh->name)==0) {
					/* replace header */
					xh->next = xp->next;
					cmsMem_free(xp->name);
					cmsMem_free(xp->value);
					cmsMem_free(xp);
					*p = xh;
					return 1;
				}
				p = &xp->next;
			}
		}
		/* just stick it at beginning of list */
        xh->next = *hdrQp;
        *hdrQp = xh;
        return 1;
    }
    return 0;
}

static int addPostHdr( XtraPostHdr **hdrQp, char *xhdrname, char *value, eHdrOp replaceDups)
{
    XtraPostHdr *xh;
    XtraPostHdr **p = hdrQp;
    xh= (XtraPostHdr *)cmsMem_alloc(sizeof(struct XtraPostHdr), ALLOC_ZEROIZE);
    if (xh) {
        xh->hdr = cmsMem_strdup(xhdrname);
        xh->value = cmsMem_strdup(value);
		if (replaceDups == REPLACE) {
			while (*p) {
				XtraPostHdr *xp = *p;
				if ( strcmp(xp->hdr, xh->hdr)==0) {
					/* replace header */
					xh->next = xp->next;
					cmsMem_free(xp->hdr);
					cmsMem_free(xp->value);
					cmsMem_free(xp);
					*p = xh;
					return 1;
				}
				p = &xp->next;
			}
		}
		/* just stick it at beginning of list */
        xh->next = *hdrQp;
        *hdrQp = xh;
        return 1;
    }
    return 0;
}
/*----------------------------------------------------------------------*/
static void report_status(tWgetInternal *data, tWgetStatus status, 
                          const char *msg)
{
   tWget wg;
   /* internal error, call callback */
   wg.status = status;
   wg.pc = data->pc;
   wg.hdrs = data->hdrs;
   wg.msg = msg;
   wg.handle = data->handle;
   data->cbActive = 1;
   (*data->cb)(&wg);
   data->cbActive = 0;
   if (data->keepConnection==eCloseConnection)
   {
      freeData(data);
   }
}

/*----------------------------------------------------------------------*
 * returns
 *   0 if ok
 *  -1 if WAN interface is not active
 */
static int send_get_request(tWgetInternal *p,
                            const char *host,
                            int port __attribute__((unused)),
                            const char *uri)
{
   tProtoCtx   *pc = p->pc;
   XtraPostHdr *next;

   cmsLog_debug("=====>ENTER");
   if (getWanState() == eWAN_INACTIVE)
   {
      return -1;
   }

   proto_SendRequest(pc, "GET", uri);
   proto_SendHeader(pc,  "Host", host);
   proto_SendHeader(pc,  "User-Agent", USER_AGENT_NAME);
   if (p->keepConnection==eCloseConnection)
   {
      proto_SendHeader(pc,  "Connection", "close");
   }
   else
   {
      proto_SendHeader(pc,  "Connection", "keep-alive");
   }
   next = p->xtraPostHdrs;
   while (next)
   {
      proto_SendHeader(pc,next->hdr, next->value);
      next = next->next;
   }
   proto_SendEndHeaders(pc);
   return 0;

}  /* End of send_get_request() */

static int send_request(tWgetInternal *p, const char *host, int port, const char *data,
                        int datalen, const char *content_type, int content_len)
{
   tProtoCtx   *pc = p->pc;
   char        buf[BUF_SIZE];
   XtraPostHdr *next;
   CookieHdr   *cookie;

   if (strchr(host,':') != NULL)
   {
      /* there needs to be a bracket around ipv6 address */
      sprintf(buf, "[%s]:%d", host, port);
   }
   else
   {
      sprintf(buf, "%s:%d", host, port);
   }

   proto_SendHeader(pc,  "Host", buf);
   proto_SendHeader(pc,  "User-Agent", USER_AGENT_NAME);
   if (p->keepConnection==eCloseConnection)
   {
      proto_SendHeader(pc,  "Connection", "close");
   }
   else
   {
      proto_SendHeader(pc,  "Connection", "keep-alive");
   }
   next = p->xtraPostHdrs;
   while (next)
   {
      proto_SendHeader(pc,next->hdr, next->value);
      next = next->next;
   }
   cookie = p->cookieHdrs;
   while(cookie)
   {
      proto_SendCookie(pc, cookie);
      cookie = cookie->next;
   }
   // only send "Content-Type" header when it is NOT EMPTY POST
   if (content_len != 0)
   {
      proto_SendHeader(pc,  "Content-Type", content_type);
   }
   sprintf(buf, "%d", content_len);
   proto_SendHeader(pc,  "Content-Length", buf);

   proto_SendEndHeaders(pc);
   if (data && datalen)
   {
      proto_SendRaw(pc, data, datalen);
   }
   return 0;

}  /* End of send_request() */

/*----------------------------------------------------------------------*
 * returns
 *   0 if ok
 *  -1 if WAN interface is not active
 *  arg_keys is a NULL terminated array of (char *)
 *  arg_values is a NULL terminated array of (char *) of same length as arg_keys
 */
static int send_post_request(tWgetInternal *p, const char *host, int port, const char *uri,
                             const char *data, int datalen, const char *content_type,
                             int content_len)
{
   tProtoCtx   *pc = p->pc;

   if (getWanState()== eWAN_INACTIVE)
   {
      return -1;
   }

   proto_SendRequest(pc, "POST", uri);
   
   return send_request(p, host, port, data, datalen, content_type, content_len);
}

/*----------------------------------------------------------------------*
 * returns
 *   0 if ok
 *  -1 if WAN interface is not active
 *  arg_keys is a NULL terminated array of (char *)
 *  arg_values is a NULL terminated array of (char *) of same length as arg_keys
 */
static int send_put_request(tWgetInternal *p, const char *host, int port, const char *uri,
                            const char *data, int datalen, const char *content_type,
                            int content_len)
{
   tProtoCtx   *pc = p->pc;

   if (getWanState()== eWAN_INACTIVE)
   {
      return -1;
   }

   proto_SendRequest(pc, "PUT", uri);
   
   return send_request(p, host, port, data, datalen, content_type, content_len);
}

/*----------------------------------------------------------------------
 * connect timeout
 */
static void timer_connect(void *p)
{
   tWgetInternal *data = (tWgetInternal *) p;
   char          buf[256];

   stopListener(data->pc->fd);

   sprintf(buf, "Connection timed out to host %s:%d", data->host, data->port);
   report_status(data, iWgetStatus_ConnectionError, buf);
}

/*----------------------------------------------------------------------*/
static void timer_response(void *p)
{
   tWgetInternal *data = (tWgetInternal *) p;
   char          buf[512];
   stopListener(data->pc->fd);
   sprintf(buf, "Host (%s:%d) is not responding, timeout", data->host, data->port);
   report_status(data, iWgetStatus_ConnectionError, buf);
}

/*----------------------------------------------------------------------*/
static void do_connect(void *p)
{
   tWgetInternal  *data = (tWgetInternal *) p;
   int            err;
   u_int          n;

   cmsLog_debug("=====>ENTER");
   cmsTmr_cancel(tmrHandle, timer_connect, data);
   stopListener(data->pc->fd);

   /* check fd status */
   n = sizeof(int);
   if (getsockopt(data->pc->fd, SOL_SOCKET, SO_ERROR, &err, &n) < 0)
   {
      report_status(data, iWgetStatus_InternalError,
                     "internal error: do_connect(): getsockopt failed");
      return;
   }

   if (err != 0)
   {
      /* connection not established */
      char buf[256];

      snprintf(buf,sizeof(buf), "Connection to host %s(%s):%d failed %d (%s)", 
               data->host, writeIp((struct sockaddr *)&(data->host_addr)), data->port, 
               err, strerror(err));
      report_status(data, iWgetStatus_ConnectionError, buf);
      return;
   }

#ifdef USE_SSL
   /* init ssl if proto is https */
   if ((URL_PROTO_HTTPS == data->urlProto) && (data->pc->sslConn <= 0))
   {
      proto_SetSslCtx(data->pc, do_connect_ssl, data);
      return;
   }
#endif

   /* return at this point if function is connect only */
   if (data->request==eConnect)
   {
      report_status(data, iWgetStatus_Ok, NULL);
      return;
   }
}  /* End of do_connect() */

/*----------------------------------------------------------------------*/
#ifdef USE_SSL
static void do_connect_ssl(void *p, int errorcode)
{
   tWgetInternal *data = (tWgetInternal *) p;

   if (errorcode < 0)
   {
      report_status(data, iWgetStatus_ConnectionError, 
                      "Failed to establish SSL connection");
   }
   else
   {
      do_connect(p);
   }
}  /* End of do_connect_ssl() */
#endif     /* USE_SSL */

/*----------------------------------------------------------------------*/
static void do_send_request(void *p, int errorcode)
{
   tWgetInternal  *data = (tWgetInternal *) p;
   HttpTask       *ht   = (HttpTask *)(data->handle);
   int            res;
   CmsRet         ret;

   cmsLog_debug("=====>ENTER. keepConn=%d status=%d", data->keepConnection, data->status);
   if (errorcode < 0)
   {
      report_status(data, iWgetStatus_ConnectionError, 
                      "Failed to establish SSL connection");
      return;
   }

   /* send request */
   switch (data->request)
   {
      case eGetData:
         res = send_get_request(p, data->host, data->port, data->uri);
         break;
      case ePostData:
         res = send_post_request(p, data->host, data->port, data->uri, data->postdata, 
                                 data->datalen, data->content_type, ht->content_len);
         break;
      case ePutData:
         res = send_put_request(p, data->host, data->port, data->uri, data->postdata, 
                                data->datalen, data->content_type, ht->content_len);
         break;
      default:
         res = -1;
         break;
   }

   if (res < 0)
   {
      report_status(data, iWgetStatus_ConnectionError, 
                    "Failed to send request on connection");
      return;
   }

   /* wait for response */
   setListener(data->pc->fd, do_response, data);
   ret = cmsTmr_set(tmrHandle, timer_response, data, ACSRESPONSETIME, "timer_response"); /* response timeout is 60 sec */
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not set ACS response timer, ret=%d", ret);
   }

}  /* End of do_send_request() */

/*----------------------------------------------------------------------*/
static void do_response(void *p)
{
   CookieHdr	*cp;
   tWgetInternal *data = (tWgetInternal *) p;

   cmsLog_debug("=====>ENTER.  data->pc=%p",data->pc);
   cmsTmr_cancel(tmrHandle, timer_response, data);

   if (data->pc == NULL)
   {
      cmsLog_error("wget %s", "Internal Error");
      report_status(data, iWgetStatus_InternalError, 
                   "internal error: no filedescriptor");
      return;
   }
   if (data->pc->fd <= 0)
   {
      report_status(data, iWgetStatus_InternalError, 
                   "internal error: no filedescriptor");
      return;
   }
   if (data->hdrs)
   {
      proto_FreeHttpHdrs(data->hdrs);
   }
   data->hdrs = proto_NewHttpHdrs();
   if (data->hdrs == NULL)
   {
      /* memory exhausted?!? */
      cmsLog_error("wget %s", "Memory exhausted");
   }

   if (proto_ParseResponse(data->pc, data->hdrs) < 0)
   {
      report_status(data, iWgetStatus_Error, 
                   "error: illegal http response or read failure");
      return;
   }

   proto_ParseHdrs(data->pc, data->hdrs);
   cp = data->hdrs->setCookies;
   while (cp)
   { /* save new cookies if present*/
      addCookieHdr( &data->cookieHdrs,cp->name, cp->value, REPLACE );
      cp = cp->next;
   }

   if (data->hdrs->status_code >= 100 && data->hdrs->status_code < 600 )
   {
      report_status(data, iWgetStatus_Ok, NULL);
   }
   else
   {
      char buf[1024];
      sprintf(buf, "Host %s returned error \"%s\"(%d)", 
             data->host, data->hdrs->message, data->hdrs->status_code);
      report_status(data, iWgetStatus_HttpError, buf);
   }
}


/*----------------------------------------------------------------------*
 * wget_GetFile
 *----------------------------------------------------------------------*/
typedef struct
{
   CmsEventHandler cb;
   void            *handle;
   char            *filename;
} tWgetFile;

/*----------------------------------------------------------------------*
 * returns
 *    0 Ok
 *   -1 File could not be open for writing
 *   -2 Error when reading from fd
 *   -3 Error when writing to file
 */
#if 0 //not used
static int copy_to_file(tProtoCtx *pc, const char *filename)
{
    int ofd;
    char buf[BUF_SIZE];
    int rlen, wlen;
    mode_t mode;
    int flags, bflags;

    /* 664 */
    mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;
    if ((ofd = creat(filename, mode)) < 0) {
        return -1;
    }

    /* TBD: Temporary fix part 1, turn on synchroneous I/O, this call will block. */
    {
        flags = (long) fcntl(pc->fd, F_GETFL);
        bflags = flags & ~O_NONBLOCK; /* clear non-block flag, i.e. block */
        fcntl(pc->fd, F_SETFL, bflags);
    }

    while ((rlen = proto_Readn(pc, buf, BUF_SIZE)) != 0) {
        if (rlen < 0) {
            if (errno == EAGAIN) {
                cmsLog_debug("wget, data not ready, let's busy wait");
                continue;
            }
            /* error when reading from fd */
            return -2;
        } else if (rlen > 0) {
            /* Bytes read */
            wlen = write(ofd, buf, rlen);
            if (wlen != rlen) {
                /* error on writing */
                return -3;
            }
        }
    }

    /* TBD: Temp fix part 2. remove blocking flags */
    {
        fcntl(pc->fd, F_SETFL, flags);
    }
    /* EOF */
    close(ofd);
    return 0;
}
#endif

/*----------------------------------------------------------------------
 * argument of callback is of type (tWget *)
 */
#if 0 //not used
static void save_to_file(void *p)
{
    tWgetFile *wf;
    tWget *w;
    tWget cbw;
    int res;

    cmsLog_debug("wget.save_to_file()...");
    w = (tWget *) p;
    wf = (tWgetFile *) w->handle;

    memset(&cbw, 0, sizeof(tWget));

    if (w->status != iWgetStatus_Ok) {
        cmsLog_debug("wget.save_to_file, status NOT ok (%d)", w->status);
        cbw.status = iWgetStatus_InternalError;
    } else if ((res = copy_to_file(w->pc, wf->filename)) < 0) {
        cmsLog_debug("wget.save_to_file, copy_to_file failed (%d), status NOT ok (%d)",
                res, w->status);
        cbw.status = iWgetStatus_InternalError;
    } else {
        cmsLog_debug("wget.save_to_file, status OK (%d)", w->status);
        cbw.status = iWgetStatus_Ok;
    }

    /* call callback */
    cbw.msg = w->msg;
    cbw.pc = w->pc;
    cbw.hdrs = w->hdrs;
    cbw.handle = wf->handle;
    cmsLog_debug("wget.save_to_file() calling callback = %p...", (void *) wf->cb);
    (*wf->cb)(&cbw);

    cmsMem_free(wf->filename);
    cmsMem_free(wf);
    cmsLog_debug("wget.save_to_file() done.");
}
#endif

/*
* Connect to the specified url
* Returns: NULL  failed allocate memory or immediate connection error.
 * 			     Call wget_LastErrorMsg() to retrieve last error msg. 
 *         pointer to Connection descriptor tWgetInternal. 
*/
tWgetInternal *wget_Connect(const char *url, CmsEventHandler callback, void *handle)
{
   tWgetInternal  *wg;

   cmsLog_debug("=====>ENTER. (\"%s\", ...)", url);
   if ((wg = (tWgetInternal*)cmsMem_alloc(sizeof(tWgetInternal), ALLOC_ZEROIZE)) == NULL)
   {
      return NULL;
   }
   lastErrorMsg[0] = '\0';

   wg->request        = eConnect;
   wg->keepConnection = eKeepConnectionOpen;

   if (cmsUtl_parseUrl(url, &(wg->urlProto), &(wg->host), &(wg->port), &(wg->uri)) != CMSRET_SUCCESS)
   {
      freeData(wg);
      return NULL;
   }

   if (wg->port == 0)
   {
      switch (wg->urlProto)
      {
      case URL_PROTO_HTTP:
         wg->port = 80;
         break;
      case URL_PROTO_HTTPS:
         wg->port = 443;
         break;
      default:
         cmsLog_error("unsupported protocol urlProto=%d (default to port 80)", wg->urlProto);
         wg->port = 80; /* guess http and port 80 */
      }
   }

   wg->pc         = NULL;
   wg->cb         = callback;
   wg->handle     = handle;

   if (strlen(wg->uri) == 0)
   {
      wg->uri = cmsMem_strdup("/");
   }

   wg->host_addr.ss_family = 0;

   /*
    * dns_lookup always returns 1, so this "if" will always evaluate to TRUE.
    * There was some code here that attempted to do a non-blocking DNS
    * lookup if dns_lookup returned 0, but that code did not appear complete and
    * we never executed it.  So I just deleted the dead code.  --mwang 2/1/07
    */
   if (dns_lookup(wg->host, &(wg->host_addr)))
   {
      /* immediate return requires special handling. */
      int res;
      int fd;

      if (wg->host_addr.ss_family == AF_INET)
      {
         ((struct sockaddr_in *)&(wg->host_addr))->sin_port = htons(wg->port);
      }
      else if (wg->host_addr.ss_family == AF_INET6)
      {
         ((struct sockaddr_in6 *)&(wg->host_addr))->sin6_port = htons(wg->port);
      }
      else
      {
         snprintf(lastErrorMsg, sizeof(lastErrorMsg), noHostResolve, wg->host);
         freeData(wg);
         wg = NULL;
      }

      if (wg == NULL)
      {
         cmsLog_debug("%s", lastErrorMsg);
      }
      else if ((res = www_EstablishConnection(wg->host_addr, &fd)) < 0)
      {
         if (res == -1)
         {
            strncpy(lastErrorMsg, "Socket creation error", sizeof(lastErrorMsg));
         }
         else
         { 
            snprintf(lastErrorMsg, sizeof(lastErrorMsg), noHostConnectMsg, 
                     wg->host, writeIp((struct sockaddr *)&(wg->host_addr)), wg->port);
         }
         cmsLog_debug("%s", lastErrorMsg);
         freeData(wg);
         wg = NULL;
      }
      else
      { /* connection complete- start it */
         cmsLog_debug("connection complete- start it");
         wg->pc = proto_NewCtx(fd);
         setListenerType(fd, do_connect, wg, iListener_Write);
      }
   }

   return wg;
}

int wget_GetData(tWgetInternal *wg, CmsEventHandler callback, void *handle)
{
   cmsLog_debug("=====>ENTER");
   wg->request = eGetData;
   wg->handle = handle;
   wg->cb = callback;
   if (wg->hdrs)
   {
      wg->hdrs->status_code    = 0; /* reset status_code */
      wg->hdrs->content_length = 0;
   }
    
   do_send_request(wg, PROTO_OK);
   return 0;

}  /* End of wget_GetData() */

int wget_PostData(tWgetInternal *wg,char *postdata, int datalen, char *content_type,
                  CmsEventHandler callback, void *handle)
{
   cmsLog_debug("=====>ENTER");
   wg->request = ePostData;
   wg->content_type = content_type;
   wg->postdata = postdata;
   wg->datalen = datalen;
   wg->handle = handle;
   wg->cb = callback;
   if (wg->hdrs)
   {
      wg->hdrs->status_code    = 0; /* reset status_code */
      wg->hdrs->content_length = 0;
   }
    
   do_send_request(wg, PROTO_OK);
   return 0;

}  /* End of wget_PostData() */

int wget_PostDataClose(tWgetInternal *wg, char *postdata, int datalen, char *content_type,
                       CmsEventHandler callback, void *handle)
{
   cmsLog_debug("=====>ENTER");
   wg->request      = ePostData;
   wg->content_type = content_type;
   wg->postdata     = postdata;
   wg->datalen      = datalen;
   wg->handle       = handle;
   wg->cb = callback;
   if (wg->hdrs)
   {
      wg->hdrs->status_code    = 0; /* reset status_code */
      wg->hdrs->content_length = 0;
   }
   wg->keepConnection = eCloseConnection;

   do_send_request(wg, PROTO_OK);
   return 0;

}  /* End of wget_PostDataClose() */

int wget_PutData(tWgetInternal *wg, char *putdata, int datalen, char *content_type,
                 CmsEventHandler callback, void *handle)
{
   cmsLog_debug("=====>ENTER");
   wg->request      = ePutData;
   wg->content_type = content_type;
   wg->postdata     = putdata;
   wg->datalen      = datalen;
   wg->handle       = handle;
   wg->cb           = callback;
   if (wg->hdrs)
   {
      wg->hdrs->status_code    = 0; /* reset status_code */
      wg->hdrs->content_length = 0;
   }

   do_send_request(wg, PROTO_OK);
   return 0;

}  /* End of wget_PutData() */

/*
* Disconnect maybe called from within a callback called
 * by report_status. Don't freeData() if cbActive is set.
 * Setting cCloseConnection will cause report_status
 * to free up the data on return by the callback.
* 
*/

int wget_Disconnect(tWgetInternal *wio)
{
    if (wio!=NULL)
    { 
        cmsTmr_cancel(tmrHandle, timer_response, wio); /* may be running */
        wio->request = eDisconnect;
        wio->keepConnection = eCloseConnection;
        if (!wio->cbActive)
        {
            freeData(wio);
        }
    }
    return 0;
}

int wget_AddPostHdr( tWgetInternal *wio, char *xhdrname, char *value)
{
   XtraPostHdr   **p = &wio->xtraPostHdrs;

   cmsLog_debug("=====>ENTER");
   return addPostHdr(p, xhdrname,value, REPLACE );
}

void wget_ClearPostHdrs( tWgetInternal *wio)
{
   XtraPostHdr *xh = wio->xtraPostHdrs;

   cmsLog_debug("=====>ENTER");
   while (xh)
   {
      XtraPostHdr *nxt;
      cmsMem_free( xh->hdr);
      cmsMem_free(xh->value);
      nxt = xh->next;
      cmsMem_free(xh);
      xh= nxt;
   }
   wio->xtraPostHdrs = NULL;
}

const char *wget_LastErrorMsg(void)
{
   return lastErrorMsg;
}


/*
 * Copyright (c) 2003-2012 Broadcom Corporation
 * All Rights Reserved
 *
 * <:label-BRCM:2012:proprietary:standard
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
#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "tr64defs.h"
#include "session.h"
#include <ctype.h>

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"

extern const char* rfc1123_fmt;
extern struct net_connection *net_connections;
extern PDevice root_devices;

extern char *strip_chars(char *str, char *reject);
extern void fd_http_notify_set(fd_set *fds);
extern PService find_svc_by_url(char *url);
extern void remove_net_connection(int fd);
extern void service_xml(PService psvc, UFILE *);
extern void device_xml(PDevice pdev, UFILE *);
extern PDevice find_dev_by_udn(char *udn);
extern void gena_renew_subscription(UFILE *, PService psvc, char *cb, char *sid, char *to, char *nt);
extern void gena_new_subscription(UFILE *, PService psvc, char *cb, char *sid, char *to, char *nt);
extern void soap_action(UFILE *, PService , char *, char *);
extern void sendChallenge(struct http_connection *c);
extern void sendOK(struct http_connection *c);
extern void sendAuthFailed(struct http_connection *c);
extern int testChallenge(struct http_connection *c, char *msg);

extern const char *global_lan_ifname;



// forward declarations of routines defined in this file.

static void process_subscribe(UFILE *, char *, char *);
extern void process_unsubscribe(UFILE *, char *, char *);
static void process_post(struct http_connection *, char *, char *);
static void process_get( UFILE *up, char *fname, char *msg );
static void strdecode( char* to, char* from );
static void dispatch_http_request(struct http_connection *c, char *request, int request_len);
static PService find_service_by_name(char *name);
static void process_allxml( UFILE *up, char *fname );

void accept_http_connection(caction_t, struct net_connection *, void *);
void http_receive(caction_t, struct http_connection *, void *);
int process_http_request(struct http_connection *c);
int do_substitions(var_entry_t *, int, char *, char *);

Error HTTPErrors[] = {
    { HTTP_OK, "OK" },
    { HTTP_SERVER_ERROR, "Internal Server Error" },
    { HTTP_BAD_REQUEST, "Bad Request" },
    { HTTP_NOT_FOUND, "Not Found" },
    { HTTP_NOT_IMPLEMENTED, "Not Implemented" },
    { HTTP_FORBIDDEN, "Forbidden" },
    { HTTP_PRECONDITION, "Precondition Failed" },
    { 0, NULL }
};

//#define  SUPPORT_HTTP_DIGEST  1
#ifdef SUPPORT_HTTP_DIGEST
authState doAuthentication(struct http_connection *c, PService psvc, char *soapaction, char *body);
#endif


/** Before calling this routine, the ip address should have been set by
 *  looking up the address bound to a particular interface we want to run
 *  the server on.  
 */
struct net_connection *make_http_socket(struct iface *pif)
{
   struct net_connection *c = NULL;
   struct sockaddr_in sockaddr;
   int fd, flags;

   /* create our HTTP socket. */
   fd = socket( AF_INET, SOCK_STREAM, 0);
   UPNP_SOCKET(("%s: socket returns %d\n", __FUNCTION__, fd));
    
   if (fd < 0) 
      goto error;

   flags = TRUE;
   setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&flags, sizeof(flags));
   
   memcpy(&sockaddr.sin_addr, &pif->inaddr, sizeof(sockaddr.sin_addr));
   sockaddr.sin_family = AF_INET;
   sockaddr.sin_port = htons( TR64C_HTTP_CONN_PORT );
	
   if ( bind(fd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0 ) 
   {
      UPNP_ERROR(("make_http_socket cannot bind to port %d - err %d.\n", TR64C_HTTP_CONN_PORT, errno));
      goto error;
   }
   if ( listen(fd, 10) ) 
   {
      UPNP_ERROR(("make_http_socket cannot listen - err %d\n", errno));
      goto error;
   }
	
   c = (struct net_connection *) malloc(sizeof(struct net_connection));
   if (c == NULL) 
   {
      UPNP_ERROR(("make_http_socket cannot malloc.\n"));
      goto error;
   }
	
   memset(c, 0, sizeof(struct net_connection));

   c->fd = fd;
   c->expires = 0;  /* never expires. */
   c->func = (CONNECTION_HANDLER) accept_http_connection;
   c->arg = (void *)(intptr_t) fd;
   
   c->next = net_connections;
   net_connections = (struct net_connection *) c;

   return c;

 error:
   /* cleanup code in case we need to bail out. */
   UPNP_ERROR(("%s failed - err %d\n", __FUNCTION__, errno));
   if (fd >= 0) 
   {
      UPNP_SOCKET(("%s: close %d\n", __FUNCTION__, fd));
      close(fd);
   }
	
   return NULL;
}


void accept_http_connection(caction_t flag, struct net_connection *nc, void *arg)
{
   int sock = nc->fd;
   struct http_connection *hc;
   struct sockaddr addr;
   int addrlen;
   int s;

   switch (flag) 
   {
   case CONNECTION_RECV: 
      memset(&addr, 0, sizeof(addr));
      addrlen = sizeof(addr);
      s = accept(sock, &addr, (socklen_t *)&addrlen);
      UPNP_SOCKET(("%s: accept returns %d\n", __FUNCTION__, s));
      UPNP_HTTP(("connection %d received on server %d\n", s, sock));
      if (s == -1) 
      {
         if (errno != EAGAIN)
            perror("accept");
      } 
      else
      {
         UPNP_TRACE(("Accepting http connection %d\n", s));
         hc = (struct http_connection *) malloc(sizeof(struct http_connection));
         if (!hc)
         {
            UPNP_SOCKET(("%s: close %d\n", __FUNCTION__, s));
            close(s);
         } 
         else
         {
            memset(hc, 0, sizeof(struct http_connection));
		    
            hc->net.fd = s;
            hc->net.expires = time(NULL) + HTTP_REQUEST_TIMEOUT;
            hc->net.func = (CONNECTION_HANDLER) http_receive;
            hc->net.arg = NULL;
            
            hc->up = udopen(hc->net.fd);
            hc->buf = malloc(HTTP_BUFSIZE);
            memset(hc->buf, 0, HTTP_BUFSIZE);
            hc->maxbytes = HTTP_BUFSIZE;
            hc->bytes_recvd = 0;
            hc->state = IDLE;
            hc->net.next = net_connections;
            net_connections = (struct net_connection *) hc;
         } 
      }
      break;

   case CONNECTION_DELETE: 
      UPNP_HTTP(("http server %d deleted\n", nc->fd));
      UPNP_SOCKET(("%s: close %d\n", __FUNCTION__, nc->fd));
      close(nc->fd);
      free(nc);
      break;
   } /* end switch */
}

void http_receive(caction_t flag, struct http_connection *c, void *arg)
{
   int nbytes, tear_down;
    
   switch (flag) 
   {
   case CONNECTION_RECV:
      if ((c->maxbytes - c->bytes_recvd) < HTTP_BUF_LOWATER) 
      {
         c->maxbytes += HTTP_BUF_INCREMENT;
         c->buf = realloc(c->buf, c->maxbytes);
         assert(c->buf != NULL);
      }

      tear_down = TRUE;
      nbytes = read(c->net.fd, c->buf + c->bytes_recvd, c->maxbytes - c->bytes_recvd - 1);
      if (nbytes > 0) 
      {
         UPNP_HTTP(("data received on http connection %d\n", c->net.fd));
         c->bytes_recvd += nbytes;

         /* reset the expiration counter each time we receive some data. */
         c->net.expires = time(NULL) + HTTP_REQUEST_TIMEOUT;

         CmsRet ret;
         if ((ret = cmsLck_acquireLockWithTimeout(TR64C_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
         {
            cmsLog_error("failed to get lock, ret=%d", ret);
            return;
         }

         /** See if we have enough information to process this request yet.  
          *  If not, process_http_request() will return FALSE without doing much of anything.
          */
         if (process_http_request(c)) {
            /** we are going to keep this connection alive, so 
             * reset the input count of this connection. 
             */
            c->bytes_recvd = 0;
         }
         cmsLck_releaseLock();
      } 
      else if (nbytes == 0) 
      {
         /** if we have read EOF on the connection or if the http request was complete,
          * or if read() returned an error, tear down this connection.
          */
         UPNP_HTTP(("removing http connection %d\n", c->net.fd));
         remove_net_connection(c->net.fd);
      } 
      else if (tear_down) 
      {
         /* Keep-alive */
         c->bytes_recvd = 0;
      }
      break;
	
   case CONNECTION_DELETE:
      UPNP_HTTP(("connection %d closed\n", c->net.fd));
      UPNP_SOCKET(("%s: close %d\n", __FUNCTION__, c->net.fd));
      close(c->net.fd);
      uclose(c->up);
      free(c->buf);

      if (c->method)
         free(c->method);
      if (c->auth.nonce)
         free(c->auth.nonce);
      if (c->auth.orignonce)
         free(c->auth.orignonce);
      if (c->auth.realm)
         free(c->auth.realm);
      if (c->auth.domain)
         free(c->auth.domain);
      if (c->auth.method)
         free(c->auth.method);

      free(c);
      break;
   } /* end switch */
}

		    
/** See if we have enough information to process this request yet.  
 *  Return TRUE if this connection has been completed and should be 
 *  released.
 */
int process_http_request(struct http_connection *c)
{
   int header_length, content_length, request_complete;
   char *p, *end, *body;
    
   request_complete = FALSE;   /* assume we will fail this routine... */
   c->buf[c->bytes_recvd] = '\0';
   if ( (body = strstr( c->buf, "\r\n\r\n" )) != NULL ||
        (body = strstr( c->buf, "\n\n" )) != NULL ) 
   {
      /* We've got the complete header.   */
      /* scan the header for a content-length header.	 */
      content_length = 0;
      p = c->buf;
      while (p < body)
      {
         end = strpbrk(p, "\r\n");
         if (end > p && IMATCH_PREFIX(p, "content-length:")) 
         {
            /* found a content-length header */
            content_length = atol(p+15);
            break;
         }
         p = end+1;
      }

      header_length = (body - c->buf) + ((*body == '\r') ? 4 : 2);
      if (content_length <= c->bytes_recvd - header_length) 
      {
         dispatch_http_request(c, c->buf, c->bytes_recvd);
         request_complete = TRUE;
      }
   }

   return request_complete;
}


/* return TRUE if the socket should be close, FALSE otherwise. */
static void dispatch_http_request(struct http_connection *c, char *request, int request_len)
{
   char* methodstr;
   char* path;
   char *fname;
   char tmpfname[] = "./";
   char * lines = request, *line;
    
   /* process first line here to find out what kind of request it is... */
   line = strsep(&lines, "\r\n");

   methodstr = strsep(&line, " \t");
   path = strsep(&line, " \t");

   if (path)
      strdecode( path, path );
    
   UPNP_HTTP(("dispatching method \"%s\"  path \"%s\"\n", methodstr, path));
   if (!line || !methodstr || !path || path[0] != '/') 
   {
      UPNP_HTTP(("Bad HTTP request\n"));
      http_error( c->up, HTTP_BAD_REQUEST );
   } 
   else 
   {
      fname = &(path[1]);
      if ( fname[0] == '\0' )
	  {
         fname = tmpfname;
      }
      /* method used to generate authenticate header */
      c->method = strdup(methodstr);   

      if ( strcasecmp(methodstr, "GET") == 0 ) 
      {
         process_get(c->up, fname, lines);
         remove_net_connection(c->net.fd);
      }
      else if ( strcasecmp(methodstr, "SUBSCRIBE") == 0 ) 
      {
         process_subscribe(c->up, fname, lines);
      }
      else if ( strcasecmp(methodstr, "UNSUBSCRIBE") == 0 ) 
      {
         process_unsubscribe(c->up, fname, lines);
      }
      else if ( strcasecmp(methodstr, "POST" ) == 0 ) 
      {
         UPNP_HTTP(("POST %s\n", fname));
         process_post(c, fname, lines);
      }
      else {
         http_error( c->up, HTTP_NOT_IMPLEMENTED );
      }
   } 
}


void http_error( UFILE *up, http_error_t error_code )
{
   http_response(up, error_code, NULL, 0);
}

static int hexit( char c )
{
   if ( c >= '0' && c <= '9' )
      return c - '0';
   if ( c >= 'a' && c <= 'f' )
      return c - 'a' + 10;
   if ( c >= 'A' && c <= 'F' )
      return c - 'A' + 10;
   return 0;           /* shouldn't happen, we're guarded by isxdigit() */
}


/* Copies and decodes a hexadecimal string.  It's ok for from and to to be the
** same string.
*/
static void strdecode( char* to, char* from )
{
   for ( ; *from != '\0'; ++to, ++from )
   {
      if ( from[0] == '%' && isxdigit( (int) from[1] ) && isxdigit( (int) from[2] ) ) 
      {
	    *to = hexit( from[1] ) * 16 + hexit( from[2] );
	    from += 2;
      }
      else
         *to = *from;
   }
   *to = '\0';
}


void http_response(UFILE *up, http_error_t error_code, const char *body, int body_len)
{
   static Error last_ditch = (Error) { 900, "Unknown HTTP error!" };
   char date[100];
   time_t now;
   PError pe;

   if (body == NULL) 
   {
      body = "";
      body_len = 0;
   }

   for (pe = HTTPErrors; pe->error_string; pe++) 
   {
      if (pe->error_code == error_code)
         break;
   }
    
   if (pe->error_string == NULL) 
      pe = &last_ditch;

   uprintf(up, "HTTP/1.1 %d %s\r\n", pe->error_code, pe->error_string);
   now = time( (time_t*) 0 );
   (void) strftime( date, sizeof(date), rfc1123_fmt, gmtime( &now ) );
   uprintf(up, "DATE: %s\r\n", date);
   uprintf(up, "Connection: Keep-Alive\r\n");
   uprintf(up, "Server: %s\r\n", SERVER);
   uprintf(up, "Content-Length: %d\r\n", body_len);
   uprintf(up, "Content-Type: text/xml; charset=\"utf-8\"\r\n");
   uprintf(up, "EXT:\r\n");
   uprintf(up, "\r\n" );

   uflush(up);

#if 0
   if (write(ufileno(up), (char *) body, body_len) == -1) 
   {
      UPNP_ERROR(("%s write error %d\n", __FUNCTION__, errno));
}
#else
    {
        long bytes_sent = 0, byte_left = 0, num_written;
        byte_left = body_len;
        bytes_sent = 0;
        while( byte_left > 0 ) 
        {
            // write data
            num_written = send( ufileno(up), body + bytes_sent, byte_left, MSG_DONTROUTE | MSG_NOSIGNAL );
            if( num_written == -1 ) 
            {
                return ;
            }
            byte_left = byte_left - num_written;
            bytes_sent += num_written;
        }
    }
#endif
}

static void process_post(struct http_connection *c, char *fname, char *msg)
{
   PService psvc;
   char *p, *line, *body, *sa = NULL;
#ifdef SUPPORT_HTTP_DIGEST
   authState ret;
#endif

   UPNP_HTTP(("process_post(entry)\n"));

   if ((psvc = find_svc_by_url(fname)) == NULL) 
   {
      http_error( c->up, HTTP_NOT_FOUND );
   } 
   else
   {
      if ( (body = strstr(msg, "\r\n\r\n" )) != NULL )
         body += 4;
      else if ( (body = strstr(msg, "\n\n" )) != NULL )
         body += 2;
	
      p = msg;
      while (p != NULL && p < body) 
      {
         line = strsep(&p, "\r\n");
         /** Search for a soapaction header, which looks like this,
          * SOAPACTION: urn:schemas-upnp-org:service:WANPPPConnection:1#GetExternalIPAddress
          */
         if (IMATCH_PREFIX(line, "SOAPACTION:")) {
            sa = strip_chars(&line[11], " \t<>\"");
            break;
         }
      }
	
#ifdef SUPPORT_HTTP_DIGEST
      ret = doAuthentication(c,psvc,sa,msg);
      UPNP_HTTP(("doAuthentication() returns ret %d\n",ret));
      if ((ret == AUTHENTICATED)  || (ret == IDLE))
      {
#endif


      soap_action(c->up, psvc, sa, body);

#ifdef SUPPORT_HTTP_DIGEST
      }

#endif
   } 
}

/** Process a GENA SUBSCRIBE message that looks like this,
 *  SUBSCRIBE /upnp/event/lanhostconfigmanagement1 HTTP/1.1
 *    NT: upnp:event
 *    Callback: <http://10.19.13.45:5000/notify>
 *    Timeout: Second-1800
 *    User-Agent: Mozilla/4.0 (compatible; UPnP/1.0; Windows NT/5.1)
 *    Host: 10.19.13.136:5431
 *    Content-Length: 0
 *    Pragma: no-cache
 */
static void process_subscribe(UFILE *up, char *fname, char *msg)
{
   PService psvc;
   char *p, *body, *line, *cb = NULL, *sid = NULL, *to = NULL, *nt = NULL;
   
   if ((psvc = find_svc_by_url(fname)) == NULL) 
   {
      UPNP_ERROR(("Cannot subscribe - service \"%s\" not found.\n", fname));
      http_error( up, HTTP_NOT_FOUND );
   } 
   else
   {
      if ( (body = strstr(msg, "\r\n\r\n" )) != NULL )
         body += 4;
      else if ( (body = strstr(msg, "\n\n" )) != NULL )
         body += 2;
	
      p = msg;
      while (p != NULL && p < body) 
      {
         line = strsep(&p, "\r\n");
         if (IMATCH_PREFIX(line, "Callback:")) 
         {
            cb = strip_chars(&line[9], " \t<>");
         } 
         else if (IMATCH_PREFIX(line, "SID:")) 
         {
            sid = strip_chars(&line[4], " \t");
         } 
         else if (IMATCH_PREFIX(line, "TIMEOUT:")) 
         {
            to = strip_chars(&line[8], " \t");
         } 
         else if (IMATCH_PREFIX(line, "NT:")) 
         {
            nt = strip_chars(&line[3], " \t");
         }
      }
	
      if (nt)
      {
         gena_new_subscription(up, psvc, cb, sid, to, nt);
      } 
      else 
      {
         gena_renew_subscription(up, psvc, cb, sid, to, nt);
      }
   } 
}

/* return TRUE if the socket should be close, FALSE otherwise. */
static void process_get( UFILE *up, char *fname, char *msg )
{
   PService psvc = NULL;
   PDevice pdev = NULL;
   char date[500], *name;
   time_t now;
   UFILE *unull;
   int content_length = 0;

   /* strip leading '/' characters */
   while (*fname == '/')
      fname++;

   /** path should now have the form 'dynsvc/<svcname>.xml' or 
    * 'dyndev/<devname>.xml'. 
    *  Parse that path and look up the appropriate device or service pointer. 
    */
   name = rindex(fname, '/');
   if (name) 
   {
      name++;  /* advance past the '/' */
      name = strsep(&name, ".");
      if (IMATCH_PREFIX(fname, "dynsvc")) 
      {
         psvc = find_service_by_name(name);
      } 
      else if (IMATCH_PREFIX(fname, "dyndev")) 
      {
         pdev = find_dev_by_udn(name);
      } 
      else if (IMATCH_PREFIX(fname, "allxml")) 
      {
         process_allxml(up, fname );
         return;
      }
   } 
   else if (IMATCH_PREFIX(fname, TR64_ROOTDEV_XML_NAME)) 
   {
      pdev = root_devices;
   } 

   /* If we failed to look up a device or service pointer then this request fails. */
   if (!psvc && !pdev) 
   {
      http_error( up, HTTP_NOT_FOUND );
      return;
   }

   /* Open a null UFILE in order to calculate the length of the response. 
      This value will be used on the Content-Length: HTTP header. */
   unull = ufopen(OSL_NULL_FILE);
   if (unull)
   {
      if (psvc) 
      {
         UPNP_HTTP(("calling service_xml for \"%s\"\n", name));
         service_xml(psvc, unull);
      } 
      else if (pdev)
      {
         UPNP_HTTP(("calling device_xml for \"%s\"\n", name));
         device_xml(pdev, unull);
      }
      content_length = utell(unull);
      uclose(unull);
    }

    /* Fail if we could not generate the expected content. */
    if (content_length == 0)
    {
       http_error( up, HTTP_SERVER_ERROR );
       return;
    }

    /* Prepare a GMT date string for use in the http header. */
    now = time( (time_t*) 0 );
    (void) strftime( date, sizeof(date), rfc1123_fmt, gmtime( &now ) );
    
    /* Send the HTTP response header */
    uprintf(up, "HTTP/1.0 200 OK\r\n");
    uprintf(up, "SERVER: %s\r\n", SERVER);
    uprintf(up, "DATE: %s\r\n", date);
    /* uprintf(up, "CONTENT-TYPE: text/xml\r\n"); */
    uprintf(up, "CONTENT-TYPE: application/octet-stream\r\n");
    uprintf(up, "Cache-Control: max-age=1\r\n");
    uprintf(up, "PRAGMA: no-cache\r\n");
    uprintf(up, "Connection: Close\r\n" );
    uprintf(up, "\r\n" );
    uflush(up);
    
    /* Send the HTTP response body */
    if (psvc)
    {
       UPNP_HTTP(("calling service_xml for \"%s\"\n", name));
       service_xml(psvc, up);
    } 
    else if (pdev)
    {
       UPNP_HTTP(("calling device_xml for \"%s\"\n", name));
       device_xml(pdev, up);
    }
    uflush(up);
}

static PService find_service_by_name(char *name)
{
   PService psvc = NULL;
   PDevice pdev;

   forall_devices(pdev) {
      forall_services(pdev, psvc) {
         if (strcmp(name, psvc->template->name) == 0)
         {
            break;
         }
      }
      if (psvc)
         break;
   }
   
   return psvc;
}

/* return TRUE if the socket should be close, FALSE otherwise. */
static void process_allxml( UFILE *up, char *fname )
{
   PService psvc;
   PDevice pdev;
   UFILE *unull;
   int content_length = 0;

   /* Open a null UFILE in order to calculate the length of the response. 
      This value will be used on the Content-Length: HTTP header. */
   unull = ufopen(OSL_NULL_FILE);
   if (unull) 
   {
      uprintf(unull, "<allxml>");

      pdev = NULL;
      forall_devices(pdev) {
         uprintf(unull, "<devicedesc>%s</devicedesc>\r\n", pdev->template->type); 
         device_xml(pdev, unull);
         break;
      }

      pdev = NULL;
      forall_devices(pdev) {
         forall_services(pdev, psvc) {
            uprintf(unull, "<servicedesc>/dynsvc/%s.xml</servicedesc>\r\n",  psvc->template->name); 
            service_xml(psvc, unull);
         }
      }
      uprintf(unull, "</allxml>");

      content_length = utell(unull);
      uclose(unull);
   }


   /* Send the HTTP response header */
   uprintf(up, "HTTP/1.1 200 OK\r\n");
   uprintf(up, "SERVER: %s\r\n", SERVER);
   /* uprintf(up, "CONTENT-TYPE: text/xml\r\n"); */
   uprintf(up, "CONTENT-TYPE: application/octet-stream\r\n");
   uprintf(up, "Cache-Control: max-age=1\r\n");
   uprintf(up, "PRAGMA: no-cache\r\n");
   uprintf(up, "Content-Length: %d\r\n", content_length);
   uprintf(up, "Connection: Keep-Alive\r\n" );
   uprintf(up, "\r\n" );
   uflush(up);

   uprintf(up, "<allxml>");

   pdev = NULL;
   forall_devices(pdev) {
      uprintf(up, "<devicedesc>%s</devicedesc>\r\n", pdev->template->type); 
      device_xml(pdev, up);
      uflush(up);
      break;
   }
 
   pdev = NULL;
   forall_devices(pdev) {
      forall_services(pdev, psvc) {
         uprintf(up, "<servicedesc>/dynsvc/%s.xml</servicedesc>\r\n",  psvc->template->name); 
         service_xml(psvc, up);
      }
   }
   uprintf(up, "</allxml>");
   uflush(up);
}

#ifdef SUPPORT_HTTP_DIGEST
/** doAuthentication()
 *  This function is called to do HTTP digest auenthication if necessary.
 *      Challege is sent out, wait for reply and return OK if passed.
 *      Otherwise, return error code: SOAP_SESSIONIDEXPIRED, 

 */
extern tr64PersistentData *pTr64Data;
extern pSessionInfo pCurrentSession;
authState doAuthentication(struct http_connection *c, PService psvc, char *soapaction, char *msg)
{
   sessionWaitEventStatus ret;
   char *ac;           /* ac stores the action line */
   char *p, *line;           
   char *body;
   char *sidStr = NULL, *sidStr_end;;   /* sid stores the session ID if any */
   char sid[TR64_SSID_LEN]; /* uuid format */
   int sidFound = 0; 
   int sidLen;
   int i;

   memset(sid,0,TR64_SSID_LEN);

   /* look for either <NewSessionID> or dslf:SessionID */
   /** for SessionID:
    *
    * <s:Header>
    *  <dslf:SessionID xmlns:dslf="http://dslforum-org" s:mustUnderstand="1">2c0a2110-30BA-444e-......</dslf:SessionID>
    *  </s:Header>
    *  <s:Body>
    *  </s:Body>
    *  </s:Envelope>
    *
    *  for newSessionID:
    *   
    *  <Envelope
    *  ....
    *  <u:ConfigurationStarted xmlns:u="urn:dslforum-org:service:DeviceConfig:1">
    *  <NewSessionID>2c0a2110-30BA-444e-......80</NewSessionId>
    *  </u:ConfigurationStarted>
    *  </s:Body>
    * </s:Envelope>
    */

   /* body or header?  */
   if ( (body = strstr(msg, "\r\n\r\n" )) != NULL )
      body += 4;
   else if ( (body = strstr(msg, "\n\n" )) != NULL )
      body += 2;
	
   p = msg;
   while (p != NULL && p < body) 
   {
      /* parse one line at a time */
      line = strsep(&p, "\r\n");
      if ((IMATCH_PREFIX(line, "<dslf:SessionID")) ||
          (IMATCH_PREFIX(line, "<NewSessionId:")))
      {
         /*  <dslf:SessionID xmlns:dslf="http://dslforum-org" s:mustUnderstand="1">2c0a2110-30BA-444e-......</dslf:SessionID> */
         /*  <NewSessionID>2c0a2110-30BA-444e-......80</NewSessionId> */
         /* find first >, ssid points to >, skip over >, so ssid points at the actual SID*/
         sidStr = strstr(line,">") + 1;
         /* look for </, ssid_end points to <, we replace it with NULL. */
         sidStr_end = strstr(sidStr,"</");
         /* sidStr_end includes < */
         sidLen = sidStr_end - sidStr - 1; 
         sidStr[sidLen] = '\0';
         if (sidStrToUuid(sidStr,sid) == 0)
         {
            UPNP_TRACE(("Invalid sessionId %s\n", sidStr));
            soap_error( c->up, SOAP_INVALIDARGS );
            return ACCESS_NOT_ALLOW;
         }
         sidFound = 1;
         break;
      }  /* sessionId */
   } /* while */

   /** what's allowed:
    *  if (passwordState == Factory)
    *  {
    *     if (action == GETxx) no authentication required.
    *     else (action == SetConfigPassword) no authentication required
    *     else (action == SETxx) return ACTION_DENY
    *  }
    *  else (passwordState == Normal)
    *  {
    *     if (action == GETxx) no authentication required.
    *     else (action == SetConfigPassword) authentication required
    *     else ((action == SETxx) 
    *     { 
    *        if (user == dslf-reset)
    *           return ACTION_DENY
    *        else
    *           authentication required
    *  }
    *
    *  authentication_required()
    *    Look for "SessionID" in Header, if found extract SessionID.   Else, do sendChallenge().
    *    if (SessionID  != NULL) 
    *    {
    *        if ((currentSession != NULL) && (sessionID == currentSession))
    *        {
    *           if (currentSession->state == Authenticated)
    *                 return no auenthication required.
    *           else 
    *                 sendChallenge();
    *        } 
    *        else 
    *        {
    *           return SESSION EXPIRED ERROR;
    *        }
    *    }
    *    else 
    *       doChallenge()
    **/

   /* 3 states: idle, authenticating and authenticated */
   if (c->state == IDLE) 
   {
      /* is there a session going on?   If yes, queue this action up for later */
      if (pCurrentSession != NULL)
      {
         /* after finish debugging, to be removed: 
            we don't want this going for every transaction */
         UPNP_HTTP(("doAuthentication(): session lock, state %d, currentSessionId is: ",
                   pCurrentSession->state));   
#ifdef UUID_TYPE
#else
         for (i= 0; i< TR64_SSID_LEN; i++)
         {
            UPNP_HTTP(("%02x ",pCurrentSession->sessionId[i]));
         }
         UPNP_HTTP(("\n"));
#endif
         if (sidFound)
         {
            if (((memcmp(pCurrentSession->sessionId,sid,sizeof(sid)) == 0) &&
                 (pCurrentSession->state != SESSION_ACTIVE))  ||
                (memcmp(pCurrentSession->sessionId,sid,sizeof(sid)) != 0))
            {
               soap_error(c->up,SOAP_SESSIONIDEXPIRED);
               return ACCESS_NOT_ALLOW;
            }
            /* otherwise, continue doing authentication */
            /* reset expire time */
            pCurrentSession->expires = time(NULL) + TR64_SESSION_TIMEOUT;
         } /* Sidfound in request */
         else  
         {
            /* no session Id in request, but there is session lock, queue up event */
            ret = sessionEnqueueWaitEvent(c);
            if (ret == WAIT_EVENT_RESOURCE_ERROR)
            {
               soap_error( c->up, SOAP_OUTOFMEMEORY);
            }
            /* either error or no error because event is queued up, just don't process it */
            /* no need to do soap response because event is queued up ok */
            return ACCESS_NOT_ALLOW;
         } /* sid is not the same */
      } /* pCurrentSession != NULL */

      UPNP_HTTP(("doAuthentication(): no session lock.\n"));   
      
      /* no session lock now */
      if (soapaction != NULL && (ac = index(soapaction, '#')) != NULL) 
      {
         UPNP_HTTP(("doAuthentication(ac %s), pTr64Data %x.\n",ac,(int)pTr64Data));

         if (IMATCH_PREFIX(ac, "#Get")) 
         {
            UPNP_HTTP(("doAuthentication(GetAction): return authPassed.\n"));   
            return AUTHENTICATED;
         }

         assert(pTr64Data != NULL);

         if (pTr64Data->passwordState == FACTORY)
         {
            if (MATCH_PREFIX(ac, "#SetConfigPassword")) 
            {
               UPNP_HTTP(("doAuthentication(SetConfigPassword-- factory): return authPassed.\n"));   
               c->state = AUTHENTICATED;
               return AUTHENTICATED;
            }
            else
            {
               UPNP_HTTP(("doAuthentication(factory): return accessNotAllow.\n"));   
               c->state = IDLE;
               //http_error( c->up, HTTP_FORBIDDEN );
               soap_error( c->up, SOAP_ACTIONNOTAUTHORIZED );
               return ACCESS_NOT_ALLOW;
            }
         } /* factory */
         else 
         {
            UPNP_HTTP(("doAuthentication(normal mode): sendingChallenge....\n"));   
            /* Normal mode, dsl-config or dslf-reset for password; dslf-config for set */
            /* dslf-reset is allowed to do SetConfigPassword to reset password only */
            c->state = AUTHENTICATING;
            if (MATCH_PREFIX(ac, "#SetConfigPassword")) 
            {
               c->setPasswordFlag = 1;
            }
            else 
            {
               c->setPasswordFlag = 0;
            }
            sendChallenge(c);
         } /* normal */
      } /* action */
   } /* IDLE */
   else if (c->state == AUTHENTICATING)
   {
      if (testChallenge(c,msg))
      {
         sendOK(c);
         c->state = AUTHENTICATED;
      }
      else
      {
         sendAuthFailed(c);
         c->state =  ACCESS_NOT_ALLOW;
      }
   } /* AUTHENTICATING */
   return (c->state);
} /* doAuthentication */


#endif /* SUPPORT_HTTP_DIGEST */

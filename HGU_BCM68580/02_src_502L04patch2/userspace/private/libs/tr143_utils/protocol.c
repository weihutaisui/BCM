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

#include <errno.h>

#include "tr143_defs.h"
#include "tr143_private.h"


/*XXX there is a cmsUtl_parseUrl, should use that common function instead
 *    of using this local copy.
 */
int parseUrl(const char *url, char *proto, char *host, int *port, char *uri)
{
   int n;
   char *p;

   *port = 0;
   strcpy(uri, "");

   /* proto */
   p = (char *) url;
   if ((p = strchr(url, ':')) == NULL) {
      return -1;
   }
   n = p - url;
   strncpy(proto, url, n);
   proto[n] = '\0';

   /* skip "://" */
   if (*p++ != ':') return -1;
   if (*p++ != '/') return -1;
   if (*p++ != '/') return -1;

   /* host */
   {
      char *hp = host;

      while (*p && *p != ':' && *p != '/') {
         *hp++ = *p++;
      }
      *hp = '\0';
   }
   if (strlen(host) == 0)
      return -1;

   /* end */
   if (*p == '\0') {
      *port = 0;
      strcpy(uri, "");
      return 0;
   }

   /* port */
   if (*p == ':') {
      char buf[10];
      char *pp = buf;

      p++;
      while (isdigit(*p)) {
         *pp++ = *p++;
      }
      *pp = '\0';
      if (strlen(buf) == 0)
         return -1;
      *port = atoi(buf);
   }

   /* uri */
   if (*p == '/') {
      char *up = uri;
      while ((*up++ = *p++));
   }

   return 0;
}

void stripTail(char *s)
{
   if (*s != '\0') {
      while(*s) s++;
      s--;
      while(*s == '\r' || *s == '\n' || *s == ' ' || *s == '\t') {
         *s = '\0';
         s--;
      }
   }
}


int proto_Writen(tProtoCtx *pc, const char *ptr, int nbytes)
{
   int  nwritten=0;

   errno = 0;
   switch (pc->type)
   {
   case iNormal:
      nwritten = write(pc->fd, ptr, nbytes);
      break;
   default:
      cmsLog_error("Impossible error; writen() illegal ProtoCtx type (%d)", pc->type);
      break;
   }

   if (nwritten <= 0)
   {
      if (errno!=EAGAIN)
         return nwritten;
   }
   return nwritten;
}

int proto_SendRequest(tProtoCtx *pc, const char *method, const char *url)
{
   char buf[TR143_BUF_SIZE_MAX];
   int len;

   len = snprintf(buf, sizeof(buf), "%s %s HTTP/1.1\r\n", method, url);
   if (len != proto_Writen(pc, buf, len))
   {
      cmsLog_error("proto_Writen error", buf);
      return -1;
   }

   return 0;
}  


void proto_SendHeader(tProtoCtx *pc,  const char *header, const char *value)
{
   char buf[TR143_BUF_SIZE_MAX];
   int len;

   if (header == NULL || value == NULL)
   {
      return;
   }

   len = snprintf(buf, sizeof(buf), "%s: %s\r\n", header, value);
   if (len != proto_Writen(pc, buf, len))
   {
      /* error in sending */
      ;
   }
}

tHttpHdrs *proto_NewHttpHdrs()
{
   return ((tHttpHdrs *)cmsMem_alloc(sizeof(tHttpHdrs), ALLOC_ZEROIZE));
}

int proto_ParseResponse(tProtoCtx *pc, tHttpHdrs *hdrs)
{
   char buf[TR143_BUF_SIZE_MAX];
   char protocol[TR143_BUF_SIZE_MAX];
   char status[TR143_BUF_SIZE_MAX];
   char message[TR143_BUF_SIZE_MAX];
   int len;

   if ((len = proto_Readline(pc, buf, TR143_BUF_SIZE_MAX)) <= 0)
   {
      cmsLog_error("sscanf error on >>%s<<",buf);
      return -1;
   }

   if (sscanf(buf, "%[^ ] %[^ ] %[^\r]", protocol, status, message ) != 3) 
   {
      cmsLog_error("sscanf error on >>%s<<",buf);
      return -1;
   }

   stripTail(protocol);
   stripTail(status);
   stripTail(message);
   cmsMem_free(hdrs->protocol);
   hdrs->protocol = cmsMem_strdup(protocol);
   hdrs->status_code = atoi(status); /* TBD: add sanity check */
   cmsMem_free(hdrs->message);
   hdrs->message = cmsMem_strdup(message);
   cmsLog_debug("protocol=\"%s\", status=%d message=\"%s\"",
         hdrs->protocol, hdrs->status_code, hdrs->message);

   return len; /* OK */
}



tProtoCtx *proto_NewCtx(int fd)
{
   tProtoCtx *pc;

   pc = (tProtoCtx *)cmsMem_alloc(sizeof(tProtoCtx), ALLOC_ZEROIZE);
   pc->type = iNormal;
   pc->fd   = fd;
   return pc;

}  /* End of proto_NewCtx() */



void proto_FreeCtx(tProtoCtx *pc)
{
   switch (pc->type)
   {
   case iNormal:
      close(pc->fd);
      cmsMem_free(pc);
      break;
   default:
      cmsLog_error("Impossible error; proto_FreeCtx() illegal ProtoCtx type (%d)",
            pc->type);
      cmsMem_free(pc);
      break;
   }
}  /* End of proto_FreeCtx() */

/*======================================================================*
 * Util
 *======================================================================*/
/*----------------------------------------------------------------------*/
/* blocking read */


/*----------------------------------------------------------------------*/
int proto_Readn(tProtoCtx *pc, char *ptr, int nbytes)
{
   int nleft, nread=0;
   int   errnoval;

   nleft = nbytes;
   while (nleft > 0) {
      errno =0;
      switch (pc->type) {
      case iNormal:
         nread = read(pc->fd, ptr, nleft);
         break;
      default:
         cmsLog_error("Impossible error; readn() illegal ProtoCtx type (%d)", pc->type);
         break;
      }

      if (nread < 0) {                            /* This function will read until the byte cnt*/
         errnoval=errno;                         /* is reached or the return is <0. In the case*/
         if (errnoval==EAGAIN )                  /* of non-blocking reads this may happen after*/
            return nbytes-nleft;                /* some bytes have been retrieved. The EAGAIN*/
         else                                    /* status indicates that more are coming */
            /* Other possibilites are ECONNRESET indicating*/
            /* that the tcp connection is broken */
            fprintf(stderr,"!!!!!!!! read(fd=%d) error=%d\n",
                  pc->fd, errnoval);
         return nread; /* error, return < 0 */

      } else if (nread == 0) {
         break; /* EOF */
      }

      nleft -= nread;
      ptr += nread;
   }

   return nbytes - nleft; /* return >= 0 */
}
/*
 * Return number of bytes written or -1.
 * If -1 check for errno for EAGAIN and recall.
 *----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*
 * Read a line from a descriptor. Read the line one byte at a time,
 * looking for the newline. We store the newline in the buffer,
 * then follow it with a \0 (the same as fgets).
 * We return the number of characters up to, but not including,
 * the \0 (the same as strlen).
 */

int proto_Readline(tProtoCtx *pc, char *buf, int maxlen)
{
   int n, rc;
   char   *ptr = buf;
   char c;

   for (n = 1; n < maxlen; n++) {
      rc = proto_Readn(pc, &c, 1);
      if (rc == 1) {
         *ptr++ = c;
         if (c == '\n')
            break;
      } else if (rc == 0) {
         if (n == 1) {
            return 0; /* EOF, no data read */
         } else
            break;    /* EOF, some data was read */
      } else {
         cmsLog_debug("ERROR: proto_Readline fd=%d (%d)", pc->fd, errno);
         return -1; /* ERROR */
      }
   }

   *ptr = '\0';
   return n;
}


void proto_FreeHttpHdrs(tHttpHdrs *p)
{
   CookieHdr   *cp, *last;
   cmsMem_free(p->content_type);
   cmsMem_free(p->protocol);
   cmsMem_free(p->wwwAuthenticate);
   cmsMem_free(p->Authorization);
   cmsMem_free(p->TransferEncoding);
   cmsMem_free(p->Connection);
   cmsMem_free(p->method);
   cmsMem_free(p->path);
   cmsMem_free(p->host);
   cp = p->setCookies;
   while (cp)
   {
      last = cp->next;
      cmsMem_free(cp->name);
      cmsMem_free(cp->value);
      cmsMem_free(cp);
      cp = last;
   }
   cmsMem_free(p->message);
   cmsMem_free(p->locationHdr);
   cmsMem_free(p->filename);
   cmsMem_free(p->arg);
   cmsMem_free(p);

}  /* End of proto_FreeHttpHdrs() */


/*----------------------------------------------------------------------*/
void proto_SendRaw(tProtoCtx *pc, const char *arg, int len)
{
   int   wlth;
   int   totWlth = 0;
   int   fault = CMSRET_SUCCESS;



   while (totWlth < len && fault == CMSRET_SUCCESS)
   {
      if ((wlth = proto_Writen(pc, arg+totWlth, len-totWlth)) >= 0)
      {
         /* some or all data sent*/
         totWlth += wlth;
         continue;
      }
      else
      {
         if (errno == EAGAIN)
         {
            //deal with interrupt
         }
         else /* must be a socket error, just set acsState.fault to CMSRET_INTERNAL_ERROR */
         {
            cmsLog_error("Serious socket error.  errno=%d", errno);
         }
      }
   }

}  /* End of proto_SendRaw() */


static char HostStr[]="Host:";
static char ConnectionStr[]="Connection:";
static char SetCookieStr[]="Set-Cookie:";
static char SetCookieStr2[]="Set-Cookie2:";
static char ContentLthStr[]="Content-Length:";
static char ContentTypeStr[]="Content-Type:";
static char WWWAuthenticateStr[]="WWW-Authenticate:";
static char AuthorizationStr[]="Authorization:";
static char TransferEncoding[]="Transfer-Encoding:";
static char LocationStr[]="Location:";

static void addCookieHdr( CookieHdr **p, char *c)
{
   CookieHdr   *newCookie = (CookieHdr*)cmsMem_alloc(sizeof (CookieHdr), 0);
   char   *cp;

   if ( (cp = strchr(c,'='))){
      newCookie->next = *p;
      newCookie->name =  (char *)cmsMem_strndup(c,cp-c);
      newCookie->value = cmsMem_strdup(cp+1);
      *p = newCookie;
   } else
      cmsMem_free(newCookie);
}

/*----------------------------------------------------------------------*
 * hdrs->type needs to be initiated
 * Only read headers according to type
 * Reads all headers until an empty '\r\n" is found.
 */
int  proto_ParseHdrs(tProtoCtx *pc, tHttpHdrs *hdrs)
{
   char buf[TR143_BUF_SIZE_MAX];
   char *cp;
   int n, len = 0;

   /* Parse the rest of the request headers. */
   while ((n = proto_Readline(pc, buf, TR143_BUF_SIZE_MAX)) > 0)
   {
      len += n;
      stripTail(buf);
      if (strcmp(buf, "") == 0) 
         break;
      else if (strncasecmp(buf, HostStr,sizeof(HostStr)-1) == 0) 
      {
         cp = &buf[sizeof(HostStr)-1];
         cp += strspn(cp, " \t");
         cmsMem_free(hdrs->host);
         hdrs->host = cmsMem_strdup(cp);
      } 
      else if (strncasecmp(buf, ContentLthStr,sizeof(ContentLthStr)-1) == 0) 
      {
         cp = &buf[sizeof(ContentLthStr)-1];
         cp += strspn(cp, " \t");
         hdrs->content_length = atoi(cp);
      } 
      else if (strncasecmp(buf, ContentTypeStr,sizeof(ContentTypeStr)-1) == 0) 
      {
         cp = &buf[sizeof(ContentTypeStr)-1];
         cp += strspn(cp, " \t");
         cmsMem_free(hdrs->content_type);
         hdrs->content_type = cmsMem_strdup(cp);
      } 
      else if (strncasecmp(buf, ConnectionStr,sizeof(ConnectionStr)-1) == 0) 
      {
         cp = &buf[sizeof(ConnectionStr)-1];
         cp += strspn(cp, " \t");
         cmsMem_free(hdrs->Connection);
         hdrs->Connection = cmsMem_strdup(cp);
      } 
      else if (strncasecmp(buf, WWWAuthenticateStr, sizeof(WWWAuthenticateStr)-1)==0) 
      {
         cp =&buf[sizeof(WWWAuthenticateStr)-1];
         cp += strspn(cp, " \t");
         cmsMem_free(hdrs->wwwAuthenticate);
         hdrs->wwwAuthenticate = cmsMem_strdup(cp);
      } 
      else if (strncasecmp(buf, AuthorizationStr, sizeof(AuthorizationStr)-1)==0) 
      {
         cp =&buf[sizeof(AuthorizationStr)-1];
         cp += strspn(cp, " \t");
         cmsMem_free(hdrs->Authorization);
         hdrs->Authorization = cmsMem_strdup(cp);
      } 
      else if (strncasecmp(buf, TransferEncoding, sizeof(TransferEncoding)-1)==0) 
      {
         cp =&buf[sizeof(TransferEncoding)-1];
         cp += strspn(cp, " \t");
         cmsMem_free(hdrs->TransferEncoding);
         hdrs->TransferEncoding = cmsMem_strdup(cp);
      } 
      else if (strncasecmp(buf, LocationStr, sizeof(LocationStr)-1)==0) 
      {
         cp =&buf[sizeof(LocationStr)-1];
         cp += strspn(cp, " \t");
         cmsMem_free(hdrs->locationHdr);
         hdrs->locationHdr = cmsMem_strdup(cp);
      }  
      else if ( (strncasecmp(buf, SetCookieStr, sizeof(SetCookieStr)-1)==0)
            || (strncasecmp(buf, SetCookieStr2, sizeof(SetCookieStr2)-1)==0) ) 
      {
         char *c;
         cp =&buf[sizeof(SetCookieStr)-1];
         cp += strspn(cp, " \t:");   /* colon is added to skip : in SetCookieStr2 str*/
         /* don't need anything after ";" if it exists */
         if ( (c = strstr(cp,";")))
            *c = '\0';
         addCookieHdr( &hdrs->setCookies, cp );
      }
   }
   return len;
}

int proto_Skip(tProtoCtx *pc)
{
   char c;
   int nread = 0;
   int ret = 0;

   cmsLog_debug("proto_Skip() read all from fd and ignore");


   do {
      switch (pc->type) {
      case iNormal:
         if (select_with_timeout(pc->fd, 1)) return -1; 
         nread = read(pc->fd, &c, 1);
         break;
      default:
         cmsLog_error("Impossible error; illegal ProtoCtx type (%d)", pc->type);
         break;
      }
      if (nread<0) {
         ret = errno == EAGAIN? 1: -1;
         break;
      }
   } while (nread>0);

   cmsLog_debug("proto_Skip() done.ret=%d", ret);
   return ret;
}

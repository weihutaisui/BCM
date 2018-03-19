/***********************************************************************
 *
 *  Copyright (c) 2004-2010  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2012:proprietary:standard

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
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include "ddnsd-update.h"

#define show_message(...) 
#define dprintf(...)
#define BUFFER_SIZE 4096
#define OPT_QUIET 0
int options = 0;

static char table64[]=
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int dyndns_update( char *service, char *hostname, char *address, char *username, char *password );
int tzo_update( char *service, char *hostname, char *address, char *username, char *password );
int noip_update( char *service, char *hostname, char *addr, char *username, char *password );
int do_connect(char *host, char *port);


int
do_update( char *service, char *hostname, char *address, char *username, char *password ) {
  if( !strcmp( service, "dyndns" ) || !strcmp( service, "dyndns-static" ) || !strcmp( service, "dyndns-custom" ) ) {
    return dyndns_update( service, hostname, address, username, password );
  } else if (!strcmp( service, "tzo" ) ) {
    return tzo_update( service, hostname, address, username, password );
  } else if (!strcmp( service, "noip" ) ) {
     return noip_update( service, hostname, address, username, password );
  }
  return UPDATERES_BADSERVICE;
}

void
output( int fd, void *buf ) {
  fd_set writefds;
  int max_fd;
  struct timeval tv;
  int ret;
                                                                                
  //dprintf((stderr, "I say: %s\n", (char *)buf));
                                                                                
  // set up our fdset and timeout
  FD_ZERO(&writefds);
  FD_SET(fd, &writefds);
  max_fd = fd;
  //memcpy(&tv, &timeout, sizeof(struct timeval));
  tv.tv_sec = 2;
  tv.tv_usec = 0;
                                                                                
  ret = select(max_fd + 1, NULL, &writefds, NULL, &tv);
  //dprintf((stderr, "ret: %d\n", ret));
                                                                                
  if(ret == -1)
  {
    perror( "select" );
  }
  else if(ret == 0)
  {
    fprintf(stderr, "timeout\n");
  }
  else
  {
    /* if we woke up on client_sockfd do the data passing */
    if(FD_ISSET(fd, &writefds))
    {
      if(send(fd, buf, strlen(buf), 0) == -1)
      {
        perror( "send" );
      }
      if( write( 1, buf, strlen( buf )) != (int) strlen( buf ) )
      {
        fprintf(stderr, "write() is incomplete!\n");
      }
    }
    else
    {
      //dprintf((stderr, "error: case not handled."));
    }
  }
}

int read_input(int fd, char *buf, int len) {
  fd_set readfds;
  int max_fd;
  struct timeval tv;
  int ret;
  int bread = -1;
                                                                                
  // set up our fdset and timeout
  FD_ZERO(&readfds);
  FD_SET(fd, &readfds);
  max_fd = fd;
  // memcpy(&tv, &timeout, sizeof(struct timeval));
  tv.tv_sec = 4;
  tv.tv_usec = 0;
                                                                                
  ret = select(max_fd + 1, &readfds, NULL, NULL, &tv);
  dprintf((stderr, "ret: %d\n", ret));
                                                                                
  if(ret == -1)
  {
    perror( "select" );
  }
  else if(ret == 0)
  {
    fprintf(stderr, "timeout\n");
  }
  else
  {
    /* if we woke up on client_sockfd do the data passing */
    if(FD_ISSET(fd, &readfds))
    {
      bread = recv(fd, buf, len-1, 0);
      //dprintf((stderr, "bread: %d\n", bread));
      buf[bread] = '\0';
      //dprintf((stderr, "got: %s\n", buf));
      if(bread == -1)
      {
        perror( "recv" );
      }
    }
    else
    {
      //dprintf((stderr, "error: case not handled."));
    }
  }
                                                                                
  return(bread);
}


void
base64Encode( char *intext, char *output ) {
  unsigned char ibuf[3];
  unsigned char obuf[4];
  int i;
  int inputparts;
                                                                                
  while(*intext) {
    for (i = inputparts = 0; i < 3; i++) {
      if(*intext) {
        inputparts++;
        ibuf[i] = *intext;
        intext++;
      }
      else
        ibuf[i] = 0;
    }
                                                                                
    obuf [0] = (ibuf [0] & 0xFC) >> 2;
    obuf [1] = ((ibuf [0] & 0x03) << 4) | ((ibuf [1] & 0xF0) >> 4);
    obuf [2] = ((ibuf [1] & 0x0F) << 2) | ((ibuf [2] & 0xC0) >> 6);
    obuf [3] = ibuf [2] & 0x3F;
                                                                                
    switch(inputparts) {
      case 1: /* only one byte read */
        sprintf(output, "%c%c==",
            table64[obuf[0]],
            table64[obuf[1]]);
        break;
      case 2: /* two bytes read */
        sprintf(output, "%c%c%c=",
            table64[obuf[0]],
            table64[obuf[1]],
            table64[obuf[2]]);
        break;
      default:
        sprintf(output, "%c%c%c%c",
            table64[obuf[0]],
            table64[obuf[1]],
            table64[obuf[2]],
            table64[obuf[3]] );
        break;
    }
    output += 4;
  }
  *output=0;
}

int 
do_connect(char *host, char *port)
{
  struct sockaddr_in address;
  int len;
  int result;
  int sock;
  struct hostent *hostinfo;
  struct servent *servinfo;
                                                                                
  // set up the socket
  if((sock=socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    if(!(options & OPT_QUIET)) {
      perror("socket");
    }
    return -1;
  }
  address.sin_family = AF_INET;
                                                                                
  // get the host address
  hostinfo = gethostbyname(host);
  if(!hostinfo) {
    if(!(options & OPT_QUIET)) {
      herror("gethostbyname");
    }
    close(sock);
    return -1;
  }
  address.sin_addr = *(struct in_addr *)*hostinfo -> h_addr_list;
                                                                                
  // get the host port
  servinfo = getservbyname(port, "tcp");
  if(servinfo) {
    address.sin_port = servinfo -> s_port;
  } else {
    address.sin_port = htons(atoi(port));
  }
                                                                                
  // connect the socket
  len = sizeof(address);
  if((result=connect(sock, (struct sockaddr *)&address, len)) == -1)
  {
    if(!(options & OPT_QUIET)) {
      perror("connect");
    }
    close(sock);
    return -1;
  }
                                                                                
  // print out some info
  if(!(options & OPT_QUIET)) {
    fprintf(stderr,
        "connected to %s (%s) on port %d.\n",
        host,
        inet_ntoa(address.sin_addr),
        ntohs(address.sin_port));
  }

  return sock;
}


int
dyndns_update( char *service, char *hostname, char *addr, char *username, char *password ) {
  char buf[BUFFER_SIZE+1];
  char *bp = buf;
  int bytes;
  int btot;
  int ret;
  int retval = UPDATERES_OK;
  int fd = -1;
  char *server = "members.dyndns.org";
  char *port = "http";
  char *request = "/nic/update";
  char userpass[512];
  char auth[512];

  snprintf( userpass, sizeof( userpass ), "%s:%s", username, password );
  base64Encode( userpass, auth );

  buf[BUFFER_SIZE] = '\0';

  if(( fd = do_connect( server, port) ) < 0) {
    if(!(options & OPT_QUIET)) {
      show_message("error connecting to %s:%s\n", server, port);
    }
    return(UPDATERES_ERROR);
  }

  snprintf(buf,sizeof(buf), "GET %s?", request);
  output( fd, buf );

  if(!strcmp( service, "dyndns-static" ) ) {
    snprintf(buf, sizeof(buf), "system=statdns&");
    output(fd, buf );
  } else if(!strcmp( service, "dyndns-custom" )) {
    snprintf(buf, sizeof( buf ), "system=custom&");
    output( fd, buf );
  }

  snprintf( buf, sizeof( buf), "hostname=%s&", hostname );
  output( fd, buf );
  snprintf( buf, sizeof( buf), "myip=%s&", addr );
  output( fd, buf );
  snprintf( buf, sizeof( buf), "wildcard=ON" );
  output( fd, buf );
  snprintf( buf, sizeof( buf), " HTTP/1.0\015\012" );
  output( fd, buf );
  snprintf( buf, sizeof( buf), "Authorization: Basic %s\015\012", auth );
  output( fd, buf );
  snprintf( buf, sizeof( buf), "User-Agent: %s-%s [%s]\015\012", "bcm-daemon", "0.1", "Linux" );
  output( fd, buf );
  snprintf( buf, sizeof( buf), "Host: %s\015\012", server );
  output( fd, buf );
  snprintf( buf, sizeof( buf ), "\015\012" );
  output( fd, buf );

  bp = buf;
  bytes = 0;
  btot = 0;
  while((bytes=read_input(fd, bp, BUFFER_SIZE-btot)) > 0) {
    bp += bytes;
    btot += bytes;
    //dprintf((stderr, "btot: %d\n", btot));
  }
  close(fd);
  buf[btot] = '\0';

  //dprintf((stderr, "server output: %s\n", buf));

  if(sscanf(buf, " HTTP/1.%*c %3d", &ret) != 1) {
    ret = -1;
  }

  switch(ret)
  {
    case -1:
      if(!(options & OPT_QUIET)) {
        show_message("strange server response, are you connecting to the right server?\n");
      }
      retval = UPDATERES_ERROR;
      break;
                                                                                
    case 200:
      if(strstr(buf, "\ngood ") != NULL) {
        if(!(options & OPT_QUIET)) {
          printf("request successful\n");
        }
      } else {
        if(strstr(buf, "\nnohost") != NULL) {
          show_message("invalid hostname: %s\n", hostname);
          retval = UPDATERES_SHUTDOWN;
        } else if(strstr(buf, "\nnotfqdn") != NULL) {
          show_message("malformed hostname: %s\n", hostname);
          retval = UPDATERES_SHUTDOWN;
        } else if(strstr(buf, "\n!yours") != NULL) {
          show_message("host \"%s\" is not under your control\n", hostname);
          retval = UPDATERES_SHUTDOWN;
        } else if(strstr(buf, "\nabuse") != NULL) {
          show_message("host \"%s\" has been blocked for abuse\n", hostname);
          retval = UPDATERES_SHUTDOWN;
        } else if(strstr(buf, "\nnochg") != NULL) {
          show_message("%s says that your IP address has not changed since the last update\n", server);
          // lets say that this counts as a successful update
          // but we'll roll back the last update time to max_interval/2
          // if(max_interval > 0)
          // {
          //   last_update = time(NULL) - max_interval/2;
          // }
          retval = UPDATERES_OK;
        } else if(strstr(buf, "\nbadauth") != NULL) {
          show_message("authentication failure\n");
          retval = UPDATERES_SHUTDOWN;
        } else if(strstr(buf, "\nbadsys") != NULL) {
          show_message("invalid system parameter\n");
          retval = UPDATERES_SHUTDOWN;
        } else if(strstr(buf, "\nbadagent") != NULL) {
          show_message("this useragent has been blocked\n");
          retval = UPDATERES_SHUTDOWN;
        } else if(strstr(buf, "\nnumhost") != NULL) {
          show_message("Too many or too few hosts found\n");
          retval = UPDATERES_SHUTDOWN;
        } else if(strstr(buf, "\ndnserr") != NULL) {
//          char *p = strstr(buf, "\ndnserr");
          show_message("dyndns internal error, please report this number to their support people: %s\n", p);
          retval = UPDATERES_ERROR;
        } else if(strstr(buf, "\n911") != NULL) {
          show_message("Ahhhh! call 911!\n");
          retval = UPDATERES_SHUTDOWN;
        } else if(strstr(buf, "\n999") != NULL) {
          show_message("Ahhhh! call 999!\n");
          retval = UPDATERES_SHUTDOWN;
        } else if(strstr(buf, "\n!donator") != NULL) {
          show_message("a feature requested is only available to donators, please donate.\n", hostname);
          retval = UPDATERES_OK;
        }
        // this one should be last as it is a stupid string to signify waits
        // with as it is so short
        else if(strstr(buf, "\nw") != NULL) {
#if 0
          int howlong = 0;
          char *p = strstr(buf, "\nw");
          char reason[256];
          char mult;
                                                                                
          // get time and reason
          if(strlen(p) >= 2)
          {
            sscanf(p, "%d%c %255[^\r\n]", &howlong, &mult, reason);
            if(mult == 'h')
            {
              howlong *= 3600;
            }
            else if(mult == 'm')
            {
              howlong *= 60;
            }
            if(howlong > MAX_WAITRESPONSE_WAIT)
            {
              howlong = MAX_WAITRESPONSE_WAIT;
            };
          }
          else
          {
            sprintf(reason, "problem parsing reason for wait response");
          }
                                                                                
          show_message("Wait response received, waiting for %s before next update.\n",
              format_time(howlong));
          show_message("Wait response reason: %d\n", N_STR(reason));
          sleep(howlong);
          retval = UPDATERES_ERROR;
#endif
        } else {
          show_message("error processing request\n");
          if(!(options & OPT_QUIET))
          {
            fprintf(stderr, "==== server output: ====\n%s\n", buf);
          }
          retval = UPDATERES_ERROR;
        }
      }
      break;
                                                                                
    case 401:
      if(!(options & OPT_QUIET)) {
        show_message("authentication failure\n");
      }
      retval = UPDATERES_SHUTDOWN;
      break;
                                                                                
    default:
      if(!(options & OPT_QUIET)) {
        // reuse the auth buffer
        *auth = '\0';
        sscanf(buf, " HTTP/1.%*c %*3d %255[^\r\n]", auth);
        show_message("unknown return code: %d\n", ret);
        fprintf(stderr, "server response: %s\n", auth);
      }
      retval = UPDATERES_ERROR;
      break;
  }
                                                                                
  return(retval);
}

int
tzo_update( char *service __attribute__ ((unused)), char *hostname, char *addr, char *username, char *password ) {
  int fd;
  char buf[BUFFER_SIZE+1];
  char *bp = buf;
  int bytes;
  int btot;
  int ret;
  char *server = "cgi.tzo.com";
  char *port = "http";
  char *request = "/webclient/signedon.html";
  char userpass[512];
  char auth[512];
                                                                                
  snprintf( userpass, sizeof( userpass ), "%s:%s", username, password );
  base64Encode( userpass, auth );

                                                                                
  buf[BUFFER_SIZE] = '\0';
                                                                                
  if((fd = do_connect( server, port)) < 0 )
  {
    if(!(options & OPT_QUIET))
    {
      show_message("error connecting to %s:%s\n", server, port);
    }
    return(UPDATERES_ERROR);
  }
                                                                                
  snprintf(buf, sizeof( buf ), "GET %s?", request);
  output( fd, buf );
  snprintf(buf, sizeof( buf ), "TZOName=%s&", hostname);
  output( fd, buf );
  snprintf(buf, sizeof( buf ), "Email=%s&", username);
  output( fd, buf );
  snprintf(buf, sizeof( buf ), "TZOKey=%s&", password);
  output( fd, buf );
  snprintf(buf, sizeof( buf ), "IPAddress=%s&", addr);
  output( fd, buf );
  snprintf(buf, sizeof( buf ), " HTTP/1.0\015\012");
  output( fd, buf );
  snprintf(buf, sizeof( buf ), "User-Agent: bcm-daemon-%s %s\015\012",
      "0.1", "Linux" );
  output( fd, buf );
  snprintf(buf, sizeof( buf ), "Host: %s\015\012", server);
  output( fd, buf );
  snprintf(buf, sizeof( buf ), "\015\012");
  output( fd, buf );
                                                                                
  bp = buf;
  bytes = 0;
  btot = 0;
  while((bytes=read_input( fd, bp, BUFFER_SIZE-btot)) > 0)
  {
    bp += bytes;
    btot += bytes;
    fprintf(stderr, "btot: %d\n", btot);
  }
  close(fd);
  buf[btot] = '\0';
                                                                                
  //fprintf(stderr, "server output: %s\n", buf);
                                                                                
  if(sscanf(buf, " HTTP/1.%*c %3d", &ret) != 1)
  {
    ret = -1;
  }
                                                                                
  switch(ret)
  {
    case -1:
      if(!(options & OPT_QUIET))
      {
        show_message("strange server response, are you connecting to the right server?\n");
      }
      return(UPDATERES_ERROR);
      break;
                                                                                
    case 200:
      if(!(options & OPT_QUIET))
      {
        printf("request successful\n");
      }
      break;
                                                                                
    case 302:
      // There is no neat way to determine the exact error other than to
      // parse the Location part of the mime header to find where we're
      // being redirected.
      if(!(options & OPT_QUIET))
      {
        // reuse the auth buffer
        *auth = '\0';
        bp = strstr(buf, "Location: ");
        if((bp < strstr(buf, "\r\n\r\n")) && (sscanf(bp, "Location: http://%*[^/]%255[^\r\n]", auth) == 1))
        {
          bp = strrchr(auth, '/') + 1;
        }
        else
        {
          bp = "";
        }
        //dprintf((stderr, "location: %s\n", bp));
                                                                                
        if(!(strncmp(bp, "domainmismatch.htm", strlen(bp)) && strncmp(bp, "invname.htm", strlen(bp))))
        {
          show_message("invalid host name\n");
        }
        else if(!strncmp(bp, "invkey.htm", strlen(bp)))
        {
          show_message("invalid password(tzo key)\n");
        }
        else if(!(strncmp(bp, "emailmismatch.htm", strlen(bp)) && strncmp(bp, "invemail.htm", strlen(bp))))
        {
          show_message("invalid user name(email address)\n");
        }
        else
        {
          show_message("unknown error\n");
        }
      }
      return(UPDATERES_ERROR);
      break;
                                                                                
    default:
      if(!(options & OPT_QUIET))
      {
        // reuse the auth buffer
        *auth = '\0';
        sscanf(buf, " HTTP/1.%*c %*3d %255[^\r\n]", auth);
        show_message("unknown return code: %d\n", ret);
        show_message("server response: %s\n", auth);
      }
      return(UPDATERES_ERROR);
      break;
  }
                                                                                
  return(UPDATERES_OK);
}

int
noip_update( char *service __attribute__ ((unused)), char *hostname, char *addr, char *username, char *password ) {
  char buf[BUFFER_SIZE+1];
  char *bp = buf;
  int bytes;
  int btot;
  int ret;
  int retval = UPDATERES_OK;
  int fd = -1;
  char *server = "dynupdate.no-ip.com";
  char *port = "http";
  char *request = "/nic/update";
  char userpass[512];
  char auth[512];

  snprintf( userpass, sizeof( userpass ), "%s:%s", username, password );
  base64Encode ( userpass, auth );

  buf[BUFFER_SIZE] = '\0';

  if(( fd = do_connect( server, port) ) < 0) {
    if(!(options & OPT_QUIET)) {
      show_message("error connecting to %s:%s\n", server, port);
    }
    return(UPDATERES_ERROR);
  }

  snprintf(buf,sizeof( buf ), "GET %s?", request);
  output( fd, buf );
  snprintf(buf, sizeof( buf ), "hostname=%s&", hostname);
  output( fd, buf );
  snprintf(buf, sizeof( buf ), "myip=%s", addr);
  output( fd, buf );
  snprintf(buf, sizeof( buf ), " HTTP/1.0\015\012");
  output( fd, buf );
  snprintf(buf, sizeof( buf ), "Host: %s\015\012", server);
  output( fd, buf );
  snprintf( buf, sizeof( buf), "Authorization: Basic %s\015\012", auth );
  output( fd, buf );
  snprintf( buf, sizeof( buf), "User-Agent: %s/%s %s\015\012", "bcm-daemon", "0.1" , username);
  output( fd, buf );
  snprintf( buf, sizeof( buf ), "\015\012" );
  output( fd, buf );

  bp = buf;
  bytes = 0;
  btot = 0;
  while((bytes=read_input(fd, bp, BUFFER_SIZE-btot)) > 0) {
    bp += bytes;
    btot += bytes;
  }
  close(fd);
  buf[btot] = '\0';

  if(sscanf(buf, " HTTP/1.%*c %3d", &ret) != 1) {
    ret = -1;
  }

  switch(ret)
  {
    case -1:
      if(!(options & OPT_QUIET)) {
        show_message("strange server response, are you connecting to the right server?\n");
      }
      retval = UPDATERES_ERROR;
      break;
                                                                                
    case 200:
      if(strstr(buf, "\ngood ") != NULL) {
        if(!(options & OPT_QUIET)) {
          printf("request successful\n");
        }
      } else {
        if(strstr(buf, "\nnohost") != NULL) {
          show_message("invalid hostname: %s\n", hostname);
          retval = UPDATERES_SHUTDOWN;
        } else if(strstr(buf, "\nnotfqdn") != NULL) {
          show_message("malformed hostname: %s\n", hostname);
          retval = UPDATERES_SHUTDOWN;
        } else if(strstr(buf, "\n!yours") != NULL) {
          show_message("host \"%s\" is not under your control\n", hostname);
          retval = UPDATERES_SHUTDOWN;
        } else if(strstr(buf, "\nabuse") != NULL) {
          show_message("host \"%s\" has been blocked for abuse\n", hostname);
          retval = UPDATERES_SHUTDOWN;
        } else if(strstr(buf, "\nnochg") != NULL) {
          show_message("%s says that your IP address has not changed since the last update\n", server);
          // lets say that this counts as a successful update
          // but we'll roll back the last update time to max_interval/2
          // if(max_interval > 0)
          // {
          //   last_update = time(NULL) - max_interval/2;
          // }
          retval = UPDATERES_OK;
        } else if(strstr(buf, "\nbadauth") != NULL) {
          show_message("authentication failure\n");
          retval = UPDATERES_SHUTDOWN;
        } else if(strstr(buf, "\nbadsys") != NULL) {
          show_message("invalid system parameter\n");
          retval = UPDATERES_SHUTDOWN;
        } else if(strstr(buf, "\nbadagent") != NULL) {
          show_message("this useragent has been blocked\n");
          retval = UPDATERES_SHUTDOWN;
        } else if(strstr(buf, "\nnumhost") != NULL) {
          show_message("Too many or too few hosts found\n");
          retval = UPDATERES_SHUTDOWN;
        } else if(strstr(buf, "\ndnserr") != NULL) {
          show_message("dyndns internal error, please report this number to their support people: %s\n", p);
          retval = UPDATERES_ERROR;
        } else if(strstr(buf, "\n911") != NULL) {
          show_message("Ahhhh! call 911!\n");
          retval = UPDATERES_SHUTDOWN;
        } else if(strstr(buf, "\n999") != NULL) {
          show_message("Ahhhh! call 999!\n");
          retval = UPDATERES_SHUTDOWN;
        } else if(strstr(buf, "\n!donator") != NULL) {
          show_message("a feature requested is only available to donators, please donate.\n", hostname);
          retval = UPDATERES_OK;
        }
        // this one should be last as it is a stupid string to signify waits
        // with as it is so short
        else if(strstr(buf, "\nw") != NULL) {
        } else {
          show_message("error processing request\n");
          if(!(options & OPT_QUIET))
          {
            fprintf(stderr, "==== server output: ====\n%s\n", buf);
          }
          retval = UPDATERES_ERROR;
        }
      }
      break;
                                                                                
    case 401:
      if(!(options & OPT_QUIET)) {
        show_message("authentication failure\n");
      }
      retval = UPDATERES_SHUTDOWN;
      break;
                                                                                
    default:
      if(!(options & OPT_QUIET)) {
        // reuse the auth buffer
        *auth = '\0';
        sscanf(buf, " HTTP/1.%*c %*3d %255[^\r\n]", auth);
        show_message("unknown return code: %d\n", ret);
        fprintf(stderr, "server response: %s\n", auth);
      }
      retval = UPDATERES_ERROR;
      break;
  }
                                                                                
  return(retval);
}

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
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <signal.h>
#include <string.h>

#include "ddnsd-conf.h"
#include "ddnsd-update.h"
#include "cms.h"
#include "cms_msg.h"

/* Time to wait in seconds between checking the interfaces */
#define TIME_WAIT 10

FILE *status = NULL;

int time_to_quit = 0;

void *msgHandle=NULL; /* handle to communications link to smd */

extern void BcmDDnsCache_update( char *hostname, char *address );
extern void BcmDDnsConfig_remove( char *hostname );
extern void BcmDDnsCache_save( char *pszFilename );


void quit_handler(int signal __attribute__ ((unused)))
{
  time_to_quit = 1;
}


int get_if_addr(int sock, char *name, struct sockaddr_in *sin)
{
  struct ifreq ifr;

  memset(&ifr, 0, sizeof(ifr));
  if (!name) {
    return -1;
  } else {
    strcpy(ifr.ifr_name, name);
  }
  /* why does this need to be done twice? */
  if(ioctl(sock, SIOCGIFADDR, &ifr) < 0)
  {
    /* Don't print out error because the interface might not be up yet */
    // perror("ioctl(SIOCGIFADDR)");
    memset(sin, 0, sizeof(struct sockaddr_in));
    return -1;
  }
  if(ioctl(sock, SIOCGIFADDR, &ifr) < 0)
  {
    perror("ioctl(SIOCGIFADDR)");
    memset(sin, 0, sizeof(struct sockaddr_in));
    return -1;
  }
  if(ifr.ifr_addr.sa_family == AF_INET)
  {
    memcpy(sin, &(ifr.ifr_addr), sizeof(struct sockaddr_in));
    return 0;
  }
  else
  {
    memset(sin, 0, sizeof(struct sockaddr_in));
    return -1;
  }
  return -1;
}

void update_hostname_addrs( char *hostname __attribute__ ((unused))) {

}

void print_hostname_addrs( char *hostname ) {
  char *interface, *cache_ip;
  struct sockaddr_in sin;
  char *service, *address, *username, *password;
  int retval;
  int sock = socket( AF_INET, SOCK_STREAM, 0 );

  interface = BcmDDnsConfig_getInterface( hostname );

  if (interface && get_if_addr( sock, interface, &sin ) == 0 ) {
    if ( ( ( cache_ip = BcmDDnsCache_get( hostname ) ) != NULL ) &&
         !strcmp( cache_ip, inet_ntoa( sin.sin_addr ) ) ) {
      // P.T. clean up resource
      close( sock );
      return;
    }
    address = strdup( inet_ntoa( sin.sin_addr ) );
#ifdef BRCM_DEBUG
    printf( "ddnsd: %s (%s) IP has changed from %s to %s\n", hostname, interface, cache_ip, address );
#endif
    service = BcmDDnsConfig_getService( hostname );
    username = BcmDDnsConfig_getUsername( hostname );
    password = BcmDDnsConfig_getPassword( hostname );
    retval = do_update( service, hostname, address, username, password );
    switch( retval ) {
      case UPDATERES_OK:
        BcmDDnsCache_update( hostname, inet_ntoa( sin.sin_addr ) );
        break;
      case UPDATERES_SHUTDOWN:
      case UPDATERES_BADSERVICE:
        BcmDDnsConfig_remove( hostname );
        break;
      case UPDATERES_ERROR:
        break;
    }
    free( address );
  }
  close( sock );
}

#ifdef BUILD_STATIC
int ddnsd_main(int argc, char **argv) {
#else
int main(int argc __attribute__ ((unused)), char **argv ) {
#endif	
//  struct ddns_cache *cache;
//  struct sockaddr_in sin;
//  int i, sock;

  cmsMsg_init(EID_DDNSD, &msgHandle);

  if( BcmDDnsConfig_init( argv[1] ) == -1 ) {
#ifdef BRCM_DEBUG
    printf("ddnsd: No interfaces defined.\n");
#endif
    exit(1);
  }

  BcmDDnsCache_init( argv[2] );

  signal( SIGHUP, &quit_handler );
  signal( SIGTERM, &quit_handler );
  signal( SIGQUIT, &quit_handler );

  for(;!time_to_quit;) {
    BcmDDnsConfig_iter( print_hostname_addrs );
    sleep(TIME_WAIT);
  }
  BcmDDnsConfig_close();
  BcmDDnsCache_save( argv[2] );
#ifdef BRCM_DEBUG
  printf( "ddnsd: Exiting...\n");
#endif
  return 0;
}

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
 *
 * $Id: linux_osl.c,v 1.6.20.2 2003/10/31 21:31:36 mthawani Exp $
 */

#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/socket.h>
#include "upnp_osl.h"
#include "upnp_dbg.h"
#include "upnp.h"

struct in_addr *osl_ifaddr(const char *ifname, struct in_addr *inaddr)
{
   int sockfd;
   struct ifreq ifreq;

   if ((sockfd = socket( AF_INET, SOCK_DGRAM, 0 )) < 0) 
   {
      perror("socket");
      return NULL;
   }

   strncpy(ifreq.ifr_name, ifname, IFNAMSIZ);
   if (ioctl(sockfd, SIOCGIFADDR, &ifreq) < 0) 
   {
      inaddr = NULL;
   }
   else 
   {
      memcpy(inaddr, &(((struct sockaddr_in *) &ifreq.ifr_addr)->sin_addr), sizeof(struct in_addr));
   }
   close(sockfd);
   return inaddr;
}

int osl_join_multicast(struct iface *pif, int fd, ulong ipaddr, ushort port)
{
   struct ip_mreqn    mcreqn;
   struct ifreq       ifreq;
   struct sockaddr_in mcaddr;
   int success = FALSE;
   int flag;

   do 
   {

      /* make sure this interface is capable of MULTICAST... */
      memset(&ifreq, 0, sizeof(ifreq));
      strcpy(ifreq.ifr_name, pif->ifname);
      if (ioctl(fd, SIOCGIFFLAGS, (uintptr_t) &ifreq)) 
      {
         break;
      }
      if ((ifreq.ifr_flags & IFF_MULTICAST) == 0) 
      {
         break;
      }
      /* bind the socket to an address and port. */
      flag = 1;
      setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*) &flag, sizeof(flag));

      memset(&mcaddr, 0, sizeof(mcaddr));
      mcaddr.sin_addr.s_addr = htonl( INADDR_ANY );
      mcaddr.sin_family = AF_INET;
      mcaddr.sin_port = htons(port);
      if ( bind(fd, (struct sockaddr *) &mcaddr, sizeof(mcaddr)) ) 
      {
         break;
      }
      /* join the multicast group. */
      memset(&ifreq, 0, sizeof(ifreq));
      strcpy(ifreq.ifr_name, pif->ifname);
      if (ioctl(fd, SIOCGIFINDEX, &ifreq)) 
      {
         break;
      }
      memset(&mcreqn, 0, sizeof(mcreqn));
      mcreqn.imr_multiaddr.s_addr = ipaddr;
      /* if we get to use struct ip_mreqn, delete the previous line and uncomment the next two */
      mcreqn.imr_address.s_addr = mcaddr.sin_addr.s_addr;
      mcreqn.imr_ifindex = ifreq.ifr_ifindex;
      if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mcreqn, sizeof(mcreqn))) 
      {
         break;
      }
      /* restrict multicast messages sent on this socket 
         to only go out this interface and no other
         (doesn't say anything about multicast receives.)
      */
      if (setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, (char*) &pif->inaddr, sizeof(pif->inaddr))) 
      {
         break;
      }
      success = TRUE;
      
   } while (0);

   /* TRUE == success, FALSE otherwise. */
   return success;
}


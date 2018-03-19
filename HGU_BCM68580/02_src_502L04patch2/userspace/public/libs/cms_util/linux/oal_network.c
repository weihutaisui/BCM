/***********************************************************************
 *
<:copyright-BRCM:2007:DUAL/GPL:standard

   Copyright (c) 2007 Broadcom 
   All Rights Reserved

Unless you and Broadcom execute a separate written software license
agreement governing use of this software, this software is licensed
to you under the terms of the GNU General Public License version 2
(the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
with the following added to such license:

   As a special exception, the copyright holders of this software give
   you permission to link this software with independent modules, and
   to copy and distribute the resulting executable under terms of your
   choice, provided that you also meet, for each linked independent
   module, the terms and conditions of the license of that module.
   An independent module is a module which is not derived from this
   software.  The special exception does not apply to any modifications
   of the software.

Not withstanding the above, under no circumstances may you combine
this software in any way with any other Broadcom software provided
under a license other than the GPL, without Broadcom's express prior
written consent.

:>
 *
 ************************************************************************/

#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include "bcmnet.h"
#include "cms.h"
#include "cms_util.h"

/** Ported from getLanInfo
 *
 */
CmsRet oal_getLanInfo(const char *lan_ifname __attribute__((unused)), struct in_addr *lan_ip, struct in_addr *lan_subnetmask)
{
#ifdef DESKTOP_LINUX

   cmsLog_debug("fake ip info for interface %s", lan_ifname);
   lan_ip->s_addr = 0xc0a80100; /* 192.168.1.0 */
   lan_subnetmask->s_addr = 0xffffff00; /* 255.255.255.0 */
   return CMSRET_SUCCESS;

#else

   int socketfd;
   struct ifreq lan;

   if ((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      cmsLog_error("failed to open socket, errno=%d", errno);
      return CMSRET_INTERNAL_ERROR;
   }

   strcpy(lan.ifr_name,lan_ifname);
   if (ioctl(socketfd,SIOCGIFADDR,&lan) < 0) {
      cmsLog_error("SIOCGIFADDR failed, errno=%d", errno);
      close(socketfd);
      return CMSRET_INTERNAL_ERROR;
   }
   *lan_ip = ((struct sockaddr_in *)&(lan.ifr_addr))->sin_addr;

   if (ioctl(socketfd,SIOCGIFNETMASK,&lan) < 0) {
      cmsLog_error("SIOCGIFNETMASK failed, errno=%d", errno);
      close(socketfd);
      return CMSRET_INTERNAL_ERROR;
   }

   *lan_subnetmask = ((struct sockaddr_in *)&(lan.ifr_netmask))->sin_addr;

   close(socketfd);
   return CMSRET_SUCCESS;
   
#endif
}


UBOOL8 oal_isInterfaceUp(const char *ifname)
{
   int  skfd;
   struct ifreq intf;
   UBOOL8 isUp = FALSE;


   if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      return FALSE;
   }

   strcpy(intf.ifr_name, ifname);

   // if interface is br0:0 and
   // there is no binding IP address then return down
   if ( strchr(ifname, ':') != NULL ) {
      if (ioctl(skfd, SIOCGIFADDR, &intf) < 0) {
         close(skfd);
         return FALSE;
      }
   }

   // if interface flag is down then return down
   // NOTE: IFF_UP indicates administrative state, not link state
   if (ioctl(skfd, SIOCGIFFLAGS, &intf) == -1) {
      isUp = 0;
   } else {
      isUp = (intf.ifr_flags & IFF_UP) ? TRUE : FALSE;
   }

   close(skfd);

   return isUp;
}


UBOOL8 oal_isInterfaceLinkUp(const char *ifname)
{
   int  skfd;
   struct ifreq intf;
   UBOOL8 isUp = FALSE;

   if(ifname == NULL) {
      return FALSE;
   }

   if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      return FALSE;
   }

   strcpy(intf.ifr_name, ifname);

   if (ioctl(skfd, SIOCGIFFLAGS, &intf) == -1) {
      isUp = 0;
   } else {
      isUp = (intf.ifr_flags & IFF_RUNNING) ? TRUE : FALSE;
   }

   close(skfd);

#ifdef alt_method
   /* there is also some old code which uses SIOCGLINKSTATE.  Don't know
    * which way is better...  Should look in kernel: maybe the info comes
    * from the same place.
    */
   if (ioctl(socketFd, SIOCGLINKSTATE, &intf) != -1)
   {
      status = *(int*)(intf.ifr_data);
      if (status) { /* link is up */ }
   }
#endif /* alt_method */

   return isUp;
}


int oal_getIfindexByIfname(const char *ifname)
{
   int sockfd;
   int ifindex = -1;
   struct ifreq ifr;

   memset(&ifr, 0, sizeof(ifr));

   if (ifname == NULL)
   {
      cmsLog_error("NULL ifname, ignored");
      return -1;
   }

   /* open socket to get INET info */
   if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
   {
      cmsLog_error("socket returns error. sockfd=%d", sockfd);
      return -1;
   }

   strcpy(ifr.ifr_name, ifname);
   if (ioctl(sockfd, SIOCGIFINDEX, &ifr) >= 0)
   {
      ifindex = ifr.ifr_ifindex;
   }

   close(sockfd);

   return ifindex;
}


int oal_getIfnameByIndex(int index, char *intfName)
{
   struct ifreq ifr;
   SINT32 s = 0;

   memset(&ifr, 0, sizeof(ifr));
   ifr.ifr_ifindex = index;

   if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
      return -1;
   if (ioctl(s, SIOCGIFNAME, &ifr) < 0)
   {
      close(s);
      return -1;
   }
   close(s);

   cmsUtl_strncpy(intfName, ifr.ifr_name, CMS_IFNAME_LENGTH);

   cmsLog_debug("%d => %s", index, intfName);

   return 0;
}


/* Get the existing interface names in the kernel, regardless they're active
 * or not. If success, the ifNameList will be assigned a new allocated string
 * containing names separated by commas. It may look like
 * "lo,dsl0,eth0,eth1,usb0,wl0".
 *
 * Caller should free ifNameList by cmsMem_free() after use.
 *
 * Return CMSRET_SUCCESS if success, error code otherwise.
 */
CmsRet oal_Net_getIfNameList(char **ifNameList)
{
#ifdef DESKTOP_LINUX

   *ifNameList = cmsMem_alloc(512, ALLOC_ZEROIZE);
   sprintf(*ifNameList, "lo,dsl0,eth0,eth1,usb0,moca0,moca1,plc0");

#else
   struct if_nameindex *ni_list = if_nameindex();
   struct if_nameindex *ni_list2 = ni_list;
   char buf[1024]={0};
   char *pbuf = buf;
   int len;

   if (ni_list == NULL)
      return CMSRET_INTERNAL_ERROR;

   /* Iterate through the array of interfaces to concatenate interface
    * names, separated by commas */
   while(ni_list->if_index) {
      len = strlen(ni_list->if_name);
      memcpy(pbuf, ni_list->if_name, len);
      pbuf += len;
      *pbuf++ = ',';
      ni_list++;
   }
   len = pbuf - buf;
   buf[len-1] = 0;

   if_freenameindex(ni_list2);

   /* Allocate dynamic memory for interface name list */
   if ((*ifNameList = cmsMem_alloc(len, 0)) == NULL)
      return CMSRET_RESOURCE_EXCEEDED;
   memcpy(*ifNameList, buf, len);
   
#endif /* DESKTOP_LINUX */

   return CMSRET_SUCCESS;
}


#ifndef DESKTOP_LINUX
static SINT32 openNetSock(struct ifreq *ifr)
{
   SINT32  skfd;

   memset((void *) ifr, 0, sizeof(*ifr));
   if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
   {
      cmsLog_error("Error openning socket for getting the LAN Only enet port list");
      return 0;
   }
   
    /* Get the name -> if_index mapping for ethswctl */
    strcpy(ifr->ifr_name, "bcmsw");
    if (ioctl(skfd, SIOCGIFINDEX, ifr) < 0) 
    {
        strcpy(ifr->ifr_name, "eth0");
        if (ioctl(skfd, SIOCGIFINDEX, ifr) < 0) 
        {
            close(skfd);
            cmsLog_debug("bcmsw/eth0 interface does not exist.  Error: %d", errno);
            return 0;
        }
    }
    return skfd;
}

CmsRet oal_Net_GetWanLanAttrList(char **nameList, int opcode)
{
   SINT32  skfd;
   struct ifreq ifr;
   struct ifreq_ext ifx;

   if ((skfd = openNetSock(&ifr)) == 0)
   {
      cmsLog_error("Error openning socket for getting the opcode %x", opcode);
      return CMSRET_INTERNAL_ERROR;
   }
   
   /* Allocate dynamic memory to hold max interface names (eth0,eth1,..eth10<cr>)*/
   ifx.bufLen = (MAX_GMAC_ETH_PORT * (IFNAMSIZ+1)) + 2;
   if ((*nameList = cmsMem_alloc(ifx.bufLen, ALLOC_ZEROIZE)) == NULL)
   {
      cmsLog_error("Fail to alloc mem in getting the opcode %x", opcode);
      close(skfd);      
      return CMSRET_RESOURCE_EXCEEDED;
   }

   ifx.stringBuf = *nameList;
   ifx.opcode = opcode;
   ifr.ifr_data = (void *)&ifx;
   if (ioctl(skfd, SIOCIFREQ_EXT, &ifr) < 0)
   {
      cmsLog_error("ioct error in getting WAN/LAN attribute enet port list.  Error: %d", errno);
      close(skfd);
      CMSMEM_FREE_BUF_AND_NULL_PTR(*nameList);
      return CMSRET_INTERNAL_ERROR;
   }

   close(skfd);

   cmsLog_debug("Opcode: %d, WAN/LAN If list=%s, strlen=%d", opcode, *nameList, strlen(*nameList));

   return CMSRET_SUCCESS;
}
#endif

CmsRet oal_Net_LANOnlyEthPortIfNameList(char **LANOnlyEthPortIfNameList __attribute__((unused)))
{
#ifdef DESKTOP_LINUX
   *LANOnlyEthPortIfNameList = cmsMem_alloc(512, 0);
   strcpy(*LANOnlyEthPortIfNameList, "eth4");
   return CMSRET_SUCCESS;
#else
   return oal_Net_GetWanLanAttrList(LANOnlyEthPortIfNameList, SIOCGPORTLANONLY);
#endif
}

CmsRet oal_Net_WANOnlyEthPortIfNameList(char **WANOnlyEthPortIfNameList __attribute__((unused)))
{
#ifdef DESKTOP_LINUX
   *WANOnlyEthPortIfNameList = cmsMem_alloc(512, 0);
   strcpy(*WANOnlyEthPortIfNameList, "eth0");
   return CMSRET_SUCCESS;

#else
   return oal_Net_GetWanLanAttrList(WANOnlyEthPortIfNameList, SIOCGPORTWANONLY);
#endif
}

CmsRet oal_Net_getGMACPortIfNameList(char **WANPreferredPortIfNameList __attribute__((unused)))
{
#ifdef DESKTOP_LINUX
   *WANPreferredPortIfNameList = cmsMem_alloc(512, 0);
   strcpy(*WANPreferredPortIfNameList, "eth1,eth3");
   return CMSRET_SUCCESS;
#else
   return oal_Net_GetWanLanAttrList(WANPreferredPortIfNameList, SIOCGPORTWANPREFERRED);
#endif
}


#ifdef SUPPORT_IPV6

/** Get the global unicast ipv6 address of the interface.
 *
 */
CmsRet oal_getLanAddr6(const char *ifname __attribute__((unused)), char *ipAddr)
{

#ifdef DESKTOP_LINUX
   *ipAddr = '\0';
   cmsLog_debug("fake lanip address %s for intf %s", ipAddr, ifname);
   return CMSRET_SUCCESS;
   
#else

   FILE *fp;
   char *space, *p1, *p2;
   char line[BUFLEN_64];
   SINT32 i;

   *ipAddr = '\0';

   if ((fp = fopen("/proc/net/if_inet6", "r")) == NULL)
   {
      /* error */
      cmsLog_error("failed to open /proc/net/if_inet6");
      return CMSRET_INTERNAL_ERROR;
   }

   while (fgets(line, sizeof(line), fp) != NULL)
   {
      if (strstr(line, ifname) != NULL && strncmp(line, "fe80", 4) != 0)
      {
         /* the first string in the line is the ip address */
         if ((space = strchr(line, ' ')) != NULL)
         {
            /* terminate the ip address string */
            *space = '\0';
         }

         /* insert a colon every 4 digits in the address string */
         p2 = ipAddr;
         for (i = 0, p1 = line; *p1 != '\0'; i++)
         {
            if (i == 4)
            {
               i = 0;
               *p2++ = ':';
            }
            *p2++ = *p1++;
         }

         /* append prefix length 64 */
         *p2++ = '/';
         *p2++ = '6';
         *p2++ = '4';
         *p2 = '\0';
         break;   /* done */
      }
   }

   fclose(fp);

   return CMSRET_SUCCESS;

#endif /* DESKTOP_LINUX */

}  /* End of oal_getLanAddr6() */

/** Get the ipv6 address of the interface.
 *
 */
 #ifdef DESKTOP_LINUX
CmsRet oal_getIfAddr6(const char *ifname __attribute__((unused)), UINT32 addrIdx __attribute__((unused)), 
                      char *ipAddr __attribute__((unused)), UINT32 *ifIndex __attribute__((unused)), 
                      UINT32 *prefixLen __attribute__((unused)), UINT32 *scope __attribute__((unused)), UINT32 *ifaFlags __attribute__((unused)))
{
   cmsLog_debug("skip this for interface %s", ifname);
   return CMSRET_NO_MORE_INSTANCES;   
}
#else

CmsRet oal_getIfAddr6(const char *ifname , UINT32 addrIdx,
                      char *ipAddr, UINT32 *ifIndex, UINT32 *prefixLen, UINT32 *scope, UINT32 *ifaFlags)
{
   CmsRet   ret = CMSRET_NO_MORE_INSTANCES;
   FILE     *fp;
   UINT32   count = 0;
   char     line[BUFLEN_64];

   *ipAddr = '\0';

   if ((fp = fopen("/proc/net/if_inet6", "r")) == NULL)
   {
      cmsLog_error("failed to open /proc/net/if_inet6");
      return CMSRET_INTERNAL_ERROR;
   }

   while (fgets(line, sizeof(line), fp) != NULL)
   {
      /* remove the carriage return char */
      line[strlen(line)-1] = '\0';

      if (strstr(line, ifname) != NULL)
      {
         char *addr, *ifidx, *plen, *scp, *flags, *devname; 
         char *nextToken = NULL;

         /* the first token in the line is the ip address */
         addr = strtok_r(line, " ", &nextToken);

         /* the second token is the Netlink device number (interface index) in hexadecimal */
         ifidx = strtok_r(NULL, " ", &nextToken);
         if (ifidx == NULL)
         {
            cmsLog_error("Invalid /proc/net/if_inet6 line");
            ret = CMSRET_INTERNAL_ERROR;
            break;
         }
            
         /* the third token is the Prefix length in hexadecimal */
         plen = strtok_r(NULL, " ", &nextToken);
         if (plen == NULL)
         {
            cmsLog_error("Invalid /proc/net/if_inet6 line");
            ret = CMSRET_INTERNAL_ERROR;
            break;
         }
            
         /* the forth token is the Scope value */
         scp = strtok_r(NULL, " ", &nextToken);
         if (scp == NULL)
         {
            cmsLog_error("Invalid /proc/net/if_inet6 line");
            ret = CMSRET_INTERNAL_ERROR;
            break;
         }
            
         /* the fifth token is the ifa flags */
         flags = strtok_r(NULL, " ", &nextToken);
         if (flags == NULL)
         {
            cmsLog_error("Invalid /proc/net/if_inet6 line");
            ret = CMSRET_INTERNAL_ERROR;
            break;
         }
            
         /* the sixth token is the device name */
         devname = strtok_r(NULL, " ", &nextToken);
         if (devname == NULL)
         {
            cmsLog_error("Invalid /proc/net/if_inet6 line");
            ret = CMSRET_INTERNAL_ERROR;
            break;
         }
         else
         {
            if (strcmp(devname, ifname) != 0)
            {
               continue;
            }
            else if (count == addrIdx)
            {
               SINT32   i;
               char     *p1, *p2;

               *ifIndex   = strtoul(ifidx, NULL, 16);
               *prefixLen = strtoul(plen, NULL, 16);
               *scope     = strtoul(scp, NULL, 16);
               *ifaFlags  = strtoul(flags, NULL, 16);

               /* insert a colon every 4 digits in the address string */
               p2 = ipAddr;
               for (i = 0, p1 = addr; *p1 != '\0'; i++)
               {
                  if (i == 4)
                  {
                     i = 0;
                     *p2++ = ':';
                  }
                  *p2++ = *p1++;
               }
               *p2 = '\0';

               ret = CMSRET_SUCCESS;
               break;   /* done */
            }
            else
            {
               count++;
            }
         }
      }
   }  /* while */

   fclose(fp);

   return ret;

}  /* End of oal_getIfAddr6() */

#endif /* DESKTOP_LINUX */

#endif

CmsRet oal_saveIfNameFromSocket(SINT32 socketfd, char *connIfName)
{
   SINT32 i = 0;
   SINT32 fd = 0;
   SINT32 numifs = 0;
   UINT32 bufsize = 0;
   struct ifreq *all_ifr = NULL;
   struct ifconf ifc;
   int ret;
   struct sockaddr_storage local_addr;
   char intfIpAddrBuf[CMS_IPADDR_LENGTH]={0};
   char localIpAddrBuf[CMS_IPADDR_LENGTH]={0};
   char tmpIpAddrBuf[CMS_IPADDR_LENGTH]={0};
   UINT32 local_len = sizeof(local_addr);
   char brlist[BUFLEN_64] = {0};

   if (socketfd < 0 || connIfName == NULL)
   {
      cmsLog_error("cmsImg_saveIfNameFromSocket: Invalid parameters: socket=%d, connIfName=%s", socketfd, connIfName);
      return CMSRET_INTERNAL_ERROR;
   }

   memset(&local_addr, 0, local_len);
   if ((ret=getsockname(socketfd, (struct sockaddr*)&local_addr,&local_len)) < 0) 
   {
      cmsLog_error("cmsImg_saveIfNameFromSocket: Error in getsockname.");
      return CMSRET_INTERNAL_ERROR;
   }

   if (local_addr.ss_family == AF_INET)
   {
      inet_ntop(AF_INET, &((struct sockaddr_in *)&local_addr)->sin_addr, localIpAddrBuf, sizeof(localIpAddrBuf));
   }
   else if (local_addr.ss_family == AF_INET6)
   {
      inet_ntop(AF_INET6, &((struct sockaddr_in6 *)&local_addr)->sin6_addr, tmpIpAddrBuf, sizeof(tmpIpAddrBuf));

      /* if this is a IPv4-mapped IPv6 address (::ffff:xxx.xxx.xxx.xxx), convert it to an IPv4 address. */
      if (strchr(tmpIpAddrBuf, '.') && strstr(tmpIpAddrBuf, ":ffff:"))
      {
         /* IPv4 client */
         char *v4addr;
         
         /* convert address to clean ipv4 address */
         v4addr = strrchr(tmpIpAddrBuf, ':') + 1;
         strcpy(localIpAddrBuf, v4addr);
      }
      else
      {
         strcpy(localIpAddrBuf,tmpIpAddrBuf);
      }
   }

   /* this should be replaced with getifaddrs() and getnameinfo() which handles ipv4/ipv6 */
   if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
   {
      cmsLog_error("cmsImg_saveIfNameFromSocket: Error openning socket when getting socket intface info");
      return CMSRET_INTERNAL_ERROR;
   }
   
   numifs = 16;

   bufsize = numifs*sizeof(struct ifreq);
   all_ifr = (struct ifreq *)cmsMem_alloc(bufsize, ALLOC_ZEROIZE);
   if (all_ifr == NULL) 
   {
      cmsLog_error("cmsImg_saveIfNameFromSocket: out of memory");
      close(fd);
      return CMSRET_INTERNAL_ERROR;
   }

   memset(&ifc, 0, sizeof(struct ifconf));
   ifc.ifc_len = bufsize;
   ifc.ifc_buf = (char *)all_ifr;
   if (ioctl(fd, SIOCGIFCONF, &ifc) < 0) 
   {
      cmsLog_error("cmsImg_saveIfNameFromSocket: Error getting interfaces\n");
      close(fd);
      cmsMem_free(all_ifr);
      return CMSRET_INTERNAL_ERROR;
   }

   numifs = ifc.ifc_len/sizeof(struct ifreq);

   /* cmsLog_error("cmsImg_saveIfNameFromSocket: numifs=%d\n",numifs);  */

   for (i = 0; i < numifs; i ++) 
   {
      /* cmsLog_error("cmsImg_saveIfNameFromSocket: intface name=%s\n", all_ifr[i].ifr_name); */
      inet_ntop(AF_INET, &((struct sockaddr_in *)&(all_ifr[i].ifr_addr))->sin_addr, intfIpAddrBuf, sizeof(intfIpAddrBuf));
      if (strcmp(intfIpAddrBuf, localIpAddrBuf) == 0)
      {
	      strcpy(connIfName, all_ifr[i].ifr_name);
         goto done;
      }
   }

   for (i = 0; i < 2; i ++)
   {
      FILE *fp;
      int scope, plen;
      char dname[CMS_IFNAME_LENGTH];
      char path[BUFLEN_64];
      struct stat st;

      /* Using an ioctl to get the current IP address only works for
       * IPv4; for IPv6, we use /proc/net/if_inet6.
       */
      if ((fp = fopen("/proc/net/if_inet6", "r")) == NULL)
      {
         cmsLog_error("cmsImg_saveIfNameFromSocket: Error openning /proc/net/if_inet6");
         close(fd);
         cmsMem_free(all_ifr);
         return CMSRET_INTERNAL_ERROR;
      }

      while (EOF != fscanf(fp,
                           " %2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx %*x %x %x %*x %s",
                           &tmpIpAddrBuf[0],
                           &tmpIpAddrBuf[1],
                           &tmpIpAddrBuf[2],
                           &tmpIpAddrBuf[3],
                           &tmpIpAddrBuf[4],
                           &tmpIpAddrBuf[5],
                           &tmpIpAddrBuf[6],
                           &tmpIpAddrBuf[7],
                           &tmpIpAddrBuf[8],
                           &tmpIpAddrBuf[9],
                           &tmpIpAddrBuf[10],
                           &tmpIpAddrBuf[11],
                           &tmpIpAddrBuf[12],
                           &tmpIpAddrBuf[13],
                           &tmpIpAddrBuf[14],
                           &tmpIpAddrBuf[15],
                           &plen,
                           &scope,
                           dname))
      {
         if (i == 0)
         {
            sprintf(path, "/sys/class/net/%s/brport", dname);
            if (!stat(path, &st) && S_ISDIR(st.st_mode))
            {
               strcat(brlist, dname);
            }
         }
         if (i == 1)
         {
            inet_ntop(AF_INET6, tmpIpAddrBuf, intfIpAddrBuf, sizeof(intfIpAddrBuf));
            if (!strcmp(intfIpAddrBuf, localIpAddrBuf) && !strstr(brlist, dname))
            {
               strcpy(connIfName, dname);
               break;
            }
         }
      }
      fclose(fp);
   }

done:
   close(fd);
   cmsMem_free(all_ifr);
   cmsLog_debug("connIfName=%s", connIfName);

   return CMSRET_SUCCESS;
}

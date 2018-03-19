/***********************************************************************
 *
 *  Copyright (c) 2007  Broadcom Corporation
 *  All Rights Reserved
 *
 * <:label-BRCM:2011:DUAL/GPL:standard
 * 
 * Unless you and Broadcom execute a separate written software license
 * agreement governing use of this software, this software is licensed
 * to you under the terms of the GNU General Public License version 2
 * (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
 * with the following added to such license:
 * 
 *    As a special exception, the copyright holders of this software give
 *    you permission to link this software with independent modules, and
 *    to copy and distribute the resulting executable under terms of your
 *    choice, provided that you also meet, for each linked independent
 *    module, the terms and conditions of the license of that module.
 *    An independent module is a module which is not derived from this
 *    software.  The special exception does not apply to any modifications
 *    of the software.
 * 
 * Not withstanding the above, under no circumstances may you combine
 * this software in any way with any other Broadcom software provided
 * under a license other than the GPL, without Broadcom's express prior
 * written consent.
 * 
:>
 * 
 ************************************************************************/

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/mii.h>
#include "bcmnet.h"

#include "cms.h"
#include "cms_util.h"
#include "oal.h"

/* Local functions */
static UINT16 cmsNet_mdio_read(int skfd, struct ifreq *ifr, int phy_id, int location);
static CmsRet cmsNet_getSpeedSetting(int skfd, struct ifreq *ifr, int phy_id,
                                     int *speed, UBOOL8 *fullDuplex, UBOOL8 *linkUp);


CmsRet cmsNet_getLanInfo(const char *lan_ifname, struct in_addr *lan_ip, struct in_addr *lan_subnetmask)
{
   return (oal_getLanInfo(lan_ifname, lan_ip, lan_subnetmask));
}


UBOOL8 cmsNet_isInterfaceUp(const char *ifname)
{
   return (oal_isInterfaceUp(ifname));
}


UBOOL8 cmsNet_isInterfaceLinkUp(const char *ifname)
{
   return (oal_isInterfaceLinkUp(ifname));
}


UBOOL8 cmsNet_isAddressOnLanSide(const char *ipAddr)
{
   UBOOL8 onLanSide = FALSE;

   /* determine the address family of ipAddr */
   if ((ipAddr != NULL) && (strchr(ipAddr, ':') == NULL))
   {
      /* ipv4 address */
      struct in_addr clntAddr, inAddr, inMask;

#ifdef DESKTOP_LINUX
      /*
       * Many desktop linux tests occur on loopback interface.  Consider that
       * as LAN side.
       */
      if (!strcmp(ipAddr, "127.0.0.1"))
      {
         return TRUE;
      }
#endif

      clntAddr.s_addr = inet_addr(ipAddr);

      oal_getLanInfo("br0", &inAddr, &inMask);
      /* check ip address of support user to see it is in LAN or not */
      if ( (clntAddr.s_addr & inMask.s_addr) == (inAddr.s_addr & inMask.s_addr) )
         onLanSide = TRUE;
      else {
         /* check ip address of support user to see if it is from secondary LAN */
         if (oal_isInterfaceUp("br0:0")) {
            oal_getLanInfo("br0:0", &inAddr, &inMask);
            if ( (clntAddr.s_addr & inMask.s_addr) == (inAddr.s_addr & inMask.s_addr) )
               onLanSide = TRUE;
         }
         /* This function will not work for ppp ip extension.  See dal_auth.c for detail */
      }
   }
#ifdef SUPPORT_IPV6
   else
   {
      /* ipv6 address */
      char lanAddr6[CMS_IPADDR_LENGTH];

      if (oal_getLanAddr6("br0", lanAddr6) != CMSRET_SUCCESS)
      {
         cmsLog_error("oal_getLanAddr6 returns error.");
      }
      else
      {
         /* see if the client addr is in the same subnet as br0. */
#ifdef DESKTOP_LINUX
         /* For desktop linux, always assume it's on the lan side */
         onLanSide = TRUE;
#else   
         onLanSide = cmsNet_isHostInSameSubnet(ipAddr, lanAddr6);
#endif /* DESKTOP_LINUX */
      }
   }
#endif

   return onLanSide;

}  /* End of cmsNet_isAddressOnLanSide() */

#if 0 /* not used */
static SINT32 cmsNet_getLeftMostOneBits(SINT32 num);

/***************************************************************************
// Function Name: cmsNet_getLeftMostOneBits.
// Description  : get the left most one bit number in the given number.
// Parameters   : num -- the given number.
// Returns      : the left most one bit number in the given number.
****************************************************************************/
SINT32 cmsNet_getLeftMostOneBits(SINT32 num) 
{
   int pos = 0;
   int numArr[8] = {128, 64, 32, 16, 8, 4, 2, 1};

   // find the left most zero bit position
   for ( pos = 0; pos < 8; pos++ )
   {
      if ( (num & numArr[pos]) == 0 )
         break;
   }

   return pos;
}
#endif


UINT32 cmsNet_getLeftMostOneBitsInMask(const char *ipMask)
{
   UINT32 num = 0;
   UINT32 mask;

   if (ipMask != NULL)
   {
      struct in_addr inetAddr;

      if (inet_aton(ipMask, &inetAddr) != 0)
      {
         SINT32 i;

         /* bit mask algo below assumes network order, so must convert
          * in case we are on a little endian system.
          */
         mask = htonl(inetAddr.s_addr);
         for (i = 31; i >= 0; i--)
         {
            if (mask & (UINT32)(1<<i))
            {
               num++;
            }
            else
            {
               break;
            }
         }
      }
   }
   return num;
}


void cmsNet_inet_cidrton(const char *cp, struct in_addr *ipAddr, struct in_addr *ipMask)
{
   char addrStr[BUFLEN_32];
   char *addr = NULL;
   char *last = NULL;

   ipAddr->s_addr = 0;
   ipMask->s_addr = 0;

   strncpy(addrStr, cp, sizeof(addrStr));

   if ((addr = strtok_r((char *)cp, "/", &last)) != NULL)
   {
      if (inet_aton(addr, ipAddr) != 0)
      {
         char *mask = NULL;

         if ((mask = strtok_r(NULL, "/", &last)) != NULL)
         {
            SINT32 i;
            SINT32 cidrLen = atoi(mask);

            for (i = 0; (i < cidrLen) && (i < 32); i++)
            {
               ipMask->s_addr |= (UINT32)(1<<(31-i));
            }
            ipMask->s_addr = htonl(ipMask->s_addr);
         }
      }
   }
}


void cmsNet_inet_ipv4AddrStrtoCidr4(const char *ipv4Addr, const char *ipv4Netmask,
                             char *ipv4Cidr)
{
   struct in_addr tmp_ip, tmp_mask, tmp_subnet;
   UINT32 left;

   ipv4Cidr[0] = '\0';

   if (!inet_aton(ipv4Addr, &tmp_ip))
   {
      cmsLog_error("inet_aton failed on %s", ipv4Addr);
      return;
   }

   if (!inet_aton(ipv4Netmask, &tmp_mask))
   {
       cmsLog_error("inet_aton failed on %s", ipv4Netmask);
       return;
   }

   tmp_subnet.s_addr = tmp_ip.s_addr & tmp_mask.s_addr;

   left = cmsNet_getLeftMostOneBitsInMask(ipv4Netmask);

   sprintf(ipv4Cidr, "%s/%d", inet_ntoa(tmp_subnet), left);

   return;
}


int cmsNet_getIfindexByIfname(const char *ifname)
{
   return (oal_getIfindexByIfname(ifname));
}


int cmsNet_getIfnameByIndex(int index, char *intfName)
{
   return (oal_getIfnameByIndex(index, intfName));
}


int cmsNet_isInterfaceExist(const char *intfName)
{
   if (cmsNet_getIfindexByIfname(intfName) >= 0)
   {
      return 1;
   }
   else
   {
      return 0;
   }
}


CmsRet cmsNet_getMacAddrByIfname(const char *ifname, unsigned char *macAddr)
{
   CmsRet         ret = CMSRET_SUCCESS;
   int            sockfd;
   struct ifreq   ifr;

   if (ifname == NULL || macAddr == NULL)
   {
      return CMSRET_INVALID_ARGUMENTS;
   }
   
#ifdef DESKTOP_LINUX
   macAddr[0]=0x11;
   macAddr[1]=0x22;
   macAddr[2]=0x33;
   macAddr[3]=0x44;
   macAddr[4]=0x55;
   macAddr[5]=0x66;
   return CMSRET_SUCCESS;
#endif


   /* open socket to get INET info */
   if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) <= 0)
   {
      cmsLog_error("socket returns error. sockfd=%d", sockfd);
      return CMSRET_SOCKET_ERROR;
   }

   memset(&ifr, 0, sizeof(struct ifreq));
   cmsUtl_strncpy(ifr.ifr_name, ifname, CMS_IFNAME_LENGTH);

   if (ioctl(sockfd, SIOCGIFHWADDR, &ifr) >= 0)
   {
      memcpy(macAddr, (unsigned char *)&ifr.ifr_hwaddr.sa_data, MAC_ADDR_LEN);
   }
   else
   {
      cmsLog_error("SIOCGIFHWADDR returns error.");
      ret = CMSRET_INTERNAL_ERROR;
   }
   
   close(sockfd);

   return ret;

}  /* End of cmsNet_getMacAddrByIfname() */


CmsRet cmsNet_getMacAddrStringByIfname(const char *ifname, char *macAddrStr)
{
   unsigned char macAddr[MAC_ADDR_LEN]={0};
   CmsRet ret;

   ret = cmsNet_getMacAddrByIfname(ifname, macAddr);
   if (ret == CMSRET_SUCCESS)
   {
      sprintf(macAddrStr,"%02x:%02x:%02x:%02x:%02x:%02x",
              (UINT8)macAddr[0],(UINT8)macAddr[1],
              (UINT8)macAddr[2],(UINT8)macAddr[3],
              (UINT8)macAddr[4],(UINT8)macAddr[5]);
   }

   return ret;
}


UINT16 cmsNet_mdio_read(int skfd, struct ifreq *ifr, int phy_id, int location)
{
    UINT16 *data = (UINT16 *)(&ifr->ifr_data);

    data[0] = phy_id;
    data[1] = location;
    data[3] = phy_id  >> 24;
    if (ioctl(skfd, SIOCGMIIREG, ifr) < 0)
    {
        cmsLog_error("SIOCGMIIREG on %s failed", ifr->ifr_name);
        return 0;
    }
    
    return data[3];

}  /* End of cmsNet_mdio_read() */

CmsRet cmsNet_getSpeedSetting(int skfd, struct ifreq *ifr, int phy_id,
                              int *speed, UBOOL8 *fullDuplex, UBOOL8 *linkUp)
{
   UINT16 bmcr = cmsNet_mdio_read(skfd, ifr, phy_id, MII_BMCR);
   UINT16 bmsr = cmsNet_mdio_read(skfd, ifr, phy_id, MII_BMSR);

   if (bmcr == 0xffff  ||  bmsr == 0x0000)
   {
      cmsLog_error("No MII transceiver present!.");
      return CMSRET_INTERNAL_ERROR;
   }

   if (bmcr & BMCR_ANENABLE)
   {
      cmsLog_debug("Interface %s Auto-negotiation enabled.", ifr->ifr_name);
      
      UINT16 nway_advert = cmsNet_mdio_read(skfd, ifr, phy_id, MII_ADVERTISE);
      UINT16 lkpar       = cmsNet_mdio_read(skfd, ifr, phy_id, MII_LPA);
      UINT16 gig_ctrl    = cmsNet_mdio_read(skfd, ifr, phy_id, MII_CTRL1000);
      UINT16 gig_status  = cmsNet_mdio_read(skfd, ifr, phy_id, MII_STAT1000);
      
      if ((gig_ctrl & ADVERTISE_1000FULL) && (gig_status & LPA_1000FULL))
      {
         *speed      = 1000;
         *fullDuplex = TRUE;
         
         cmsLog_debug("The autonegotiated media type is 1000BT Full Duplex");
      }
      else if ((gig_ctrl & ADVERTISE_1000HALF) && (gig_status & LPA_1000HALF))
      {
         *speed      = 1000;
         *fullDuplex = FALSE;
         
         cmsLog_debug("The autonegotiated media type is 1000BT Half Duplex");
      }
      else if (lkpar & ADVERTISE_LPACK)
      {
         const char *media_names[] = {"10baseT",
                                      "10baseT-FD",
                                      "100baseTx",
                                      "100baseTx-FD",
                                      "100baseT4",
                                      "Flow-control"};
         int negotiated = nway_advert & lkpar & 
                          (ADVERTISE_100BASE4 |
                           ADVERTISE_100FULL |
                           ADVERTISE_100HALF |
                           ADVERTISE_10FULL |
                           ADVERTISE_10HALF);
         int max_capability = 0;

         /* Scan for the highest negotiated capability, highest priority
          * (100baseTx-FDX) to lowest (10baseT-HDX).
          */
         int media_priority[] = {8, 9, 7, 6, 5, 0};     /* media_names[i-5] */
         int i;
            
         for (i = 0; media_priority[i]; i++)
         {
            if (negotiated & (1 << media_priority[i]))
            {
               max_capability = media_priority[i];
               break;
            }
         }
         if (max_capability)
         {
            cmsLog_debug("The autonegotiated media type is %s", media_names[max_capability - 5]);
            
            if (cmsUtl_strstr((char *)&media_names[max_capability - 5], "10base") != NULL)
            {
               *speed = 10;
            }
            else
            {
               *speed = 100;
            }
            if (cmsUtl_strstr((char *)&media_names[max_capability - 5], "FD") != NULL)
            {
               *fullDuplex = TRUE;
            }
            else
            {
               *fullDuplex = FALSE;    /* half */
            } 
         }
         else
         {
            cmsLog_error("No common media type was autonegotiated!\n"
                         "This is extremely unusual and typically indicates a "
                         "configuration error.\n" "Perhaps the advertised "
                         "capability set was intentionally limited.");
            return CMSRET_INTERNAL_ERROR;
         }
      }
   }
   else
   {
      *speed      = (bmcr & BMCR_SPEED100) ? 100 : 10;
      *fullDuplex = (bmcr & BMCR_FULLDPLX) ? TRUE : FALSE;
      
      cmsLog_debug("Auto-negotiation disabled. Speed fixed at %d mbps %s-duplex",
                   *speed, *fullDuplex ? "full":"half");
   }
   
   bmsr = cmsNet_mdio_read(skfd, ifr, phy_id, MII_BMSR);
   bmsr = cmsNet_mdio_read(skfd, ifr, phy_id, MII_BMSR);
    
   *linkUp = (bmsr & BMSR_LSTATUS) ? TRUE : FALSE;
   
   cmsLog_debug("Link is %s.", *linkUp ? "up" : "down");

   return CMSRET_SUCCESS;

}  /* End of cmsNet_getSpeedSetting() */

CmsRet cmsNet_getEthLinkStatus(const char *ifname, int port, int *speed, UBOOL8 *fullDuplex, UBOOL8 *linkUp)
{
   CmsRet ret = CMSRET_SUCCESS;
   struct ifreq   ifr;
   int skfd;
   int emac_ports = 0;
   int phy_id     = 0;
   UINT16 *data;

   if (ifname == NULL)
   {
      return CMSRET_INVALID_ARGUMENTS;
   }
   
   /* open socket to get INET info */
   if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) <= 0)
   {
      cmsLog_error("socket returns error. skfd=%d", skfd);
      return CMSRET_SOCKET_ERROR;
   }
   
   memset(&ifr, 0, sizeof(struct ifreq));
   cmsUtl_strncpy(ifr.ifr_name, ifname, CMS_IFNAME_LENGTH);

   if (ioctl(skfd, SIOCGIFINDEX, &ifr) < 0 )
   {
      cmsLog_error("ioctl failed. check if %s exists.", ifr.ifr_name);
      close(skfd);
      return CMSRET_SOCKET_ERROR;
   }

   /* Find out whether the interface is Ethernet Switch or not. */
   ifr.ifr_data = (char*)&emac_ports;
   if (ioctl(skfd, SIOCGQUERYNUMPORTS, &ifr) < 0)
   {
      cmsLog_error("interface %s ioctl SIOCGQUERYNUMPORTS error!", ifr.ifr_name);
      close(skfd);
      return CMSRET_SOCKET_ERROR;
   }
   if (emac_ports < 0)
   {
      cmsLog_error("ioctl SIOCGQUERYNUMPORTS returns invalid port number %d", emac_ports);
      close(skfd);
      return CMSRET_INTERNAL_ERROR;
   }
   if (emac_ports > 1)
   {
      cmsLog_debug("interface %s is Ethernet Switch, use port argument");

      if (port >= emac_ports)
      {
         cmsLog_error("Invalid port, interface %s Ethernet Switch port range 0 to %d.",
                      ifr.ifr_name, emac_ports-1);
         close(skfd);
         return CMSRET_INVALID_ARGUMENTS;
      }
      phy_id = port;
   }
   else
   {
      /* get phy id */
      data = (UINT16 *)(&ifr.ifr_data);
      data[0] = 0;
      data[3] = 0;
      if (ioctl(skfd, SIOCGMIIPHY, &ifr) < 0)
      {
         cmsLog_error("Interface %s ioctl SIOCGMIIPHY error!", ifr.ifr_name);
         close(skfd);
         return CMSRET_SOCKET_ERROR;
      }

      /* returned phy id carries n data[3] flags if phy is
       * internal/external phy/phy on ext switch.
       * we save it in higher byte to pass to kernel when 
       * phy is accessed.
       */
      phy_id = data[0] | (((int)data[3]) << 24);
   }
   
   ret = cmsNet_getSpeedSetting(skfd, &ifr, phy_id, speed, fullDuplex, linkUp);
   
   close(skfd);
   
   return ret;
   
}  /* End of cmsNet_getEthLinkStatus() */

CmsRet cmsNet_getIfNameList(char **ifNameList)
{
   return (oal_Net_getIfNameList(ifNameList));
}

CmsRet cmsNet_getGMACPortIfNameList(char **GMACPortIfNameList)
{
   return (oal_Net_getGMACPortIfNameList(GMACPortIfNameList));
}

CmsRet cmsNet_getWANOnlyEthPortIfNameList(char **WANOnlyPortIfNameList)
{
   return (oal_Net_WANOnlyEthPortIfNameList(WANOnlyPortIfNameList));
}


CmsRet cmsNet_getLANOnlyEthPortIfNameList(char **LANOnlyPortIfNameList)
{
   return (oal_Net_LANOnlyEthPortIfNameList(LANOnlyPortIfNameList));
}


UBOOL8 cmsNet_isHostInSameSubnet(const char *addrHost, const char *addrPrefix)
{
   char address[INET6_ADDRSTRLEN];
   char prefix1[INET6_ADDRSTRLEN];
   char prefix2[INET6_ADDRSTRLEN];
   UINT32 plen = 0;
   CmsRet ret;

   *address = '\0';
   *prefix1 = '\0';
   *prefix2 = '\0';

   if (IS_EMPTY_STRING(addrHost) && IS_EMPTY_STRING(addrPrefix))
   {
      return TRUE;
   }
   if (addrHost == NULL || addrPrefix == NULL)
   {
      return FALSE;
   }

   if (strchr(addrHost, '/') != NULL)
   {
      cmsLog_error("Invalid host address %s", addrHost);
      return FALSE;
   }
   if (strchr(addrPrefix, '/') == NULL)
   {
      cmsLog_error("Invalid address prefix %s", addrPrefix);
      return FALSE;
   }

   if ((ret = cmsUtl_parsePrefixAddress(addrPrefix, address, &plen)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsUtl_parsePrefixAddress returns error. ret=%d", ret);
      return FALSE;
   }

   if ((ret = cmsUtl_getAddrPrefix(addrHost, plen, prefix1)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsUtl_getAddrPrefix returns error. ret=%d", ret);
      return FALSE;
   }
   if ((ret = cmsUtl_getAddrPrefix(address, plen, prefix2)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsUtl_getAddrPrefix returns error. ret=%d", ret);
      return FALSE;
   }

   return (cmsNet_areIp6AddrEqual(prefix1, prefix2));
   
}  /* cmsNet_isHostInSameSubnet() */


UBOOL8 cmsNet_areIp6AddrEqual(const char *ip6Addr1, const char *ip6Addr2)
{
   char address1[CMS_IPADDR_LENGTH];
   char address2[CMS_IPADDR_LENGTH];
   UINT32 plen1 = 0;
   UINT32 plen2 = 0;
   struct in6_addr   in6Addr1, in6Addr2;
   CmsRet ret;

   if (IS_EMPTY_STRING(ip6Addr1) && IS_EMPTY_STRING(ip6Addr2))
   {
      return TRUE;
   }
   if (ip6Addr1 == NULL || ip6Addr2 == NULL)
   {
      return FALSE;
   }

   if ((ret = cmsUtl_parsePrefixAddress(ip6Addr1, address1, &plen1)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsUtl_parsePrefixAddress returns error. ret=%d", ret);
      return FALSE;
   }
   if ((ret = cmsUtl_parsePrefixAddress(ip6Addr2, address2, &plen2)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsUtl_parsePrefixAddress returns error. ret=%d", ret);
      return FALSE;
   }

   if (inet_pton(AF_INET6, address1, &in6Addr1) <= 0)
   {
      cmsLog_error("Invalid address1=%s", address1);
      return FALSE;
   }
   if (inet_pton(AF_INET6, address2, &in6Addr2) <= 0)
   {
      cmsLog_error("Invalid address2=%s", address2);
      return FALSE;
   }

   return ((memcmp(&in6Addr1, &in6Addr2, sizeof(struct in6_addr)) == 0) && (plen1 == plen2));

}  /* cmsNet_areIp6AddrEqual() */


#ifdef SUPPORT_IPV6

CmsRet cmsNet_getIfAddr6(const char *ifname, UINT32 addrIdx,
                         char *ipAddr, UINT32 *ifIndex, UINT32 *prefixLen, UINT32 *scope, UINT32 *ifaFlags)
{
   return oal_getIfAddr6(ifname, addrIdx, ipAddr, ifIndex, prefixLen, scope, ifaFlags);
}


CmsRet cmsNet_getGloballyUniqueIfAddr6(const char *ifname, char *ipAddr, UINT32 *prefixLen)
{
   UINT32 addrIdx=0;
   UINT32 netlinkIndex=0;
   UINT32 scope=0;
   UINT32 ifaflags=0;
   CmsRet ret=CMSRET_SUCCESS;

   while (CMSRET_SUCCESS == ret)
   {
      ret = cmsNet_getIfAddr6(ifname, addrIdx, ipAddr, &netlinkIndex,
                              prefixLen, &scope, &ifaflags);
      if ((CMSRET_SUCCESS == ret) && (0 == scope))  // found it
         return CMSRET_SUCCESS;

      addrIdx++;
   }

   return CMSRET_NO_MORE_INSTANCES;
}



UBOOL8 cmsNet_areIp6DnsEqual(const char *dnsServers1, const char *dnsServers2)
{
   char dnsPri1[CMS_IPADDR_LENGTH];
   char dnsSec1[CMS_IPADDR_LENGTH];
   char dnsPri2[CMS_IPADDR_LENGTH];
   char dnsSec2[CMS_IPADDR_LENGTH];
   CmsRet ret;

   *dnsPri1 = '\0';
   *dnsSec1 = '\0';
   *dnsPri2 = '\0';
   *dnsSec2 = '\0';

   if (IS_EMPTY_STRING(dnsServers1) && IS_EMPTY_STRING(dnsServers2))
   {
      return TRUE;
   }
   if (dnsServers1 == NULL || dnsServers2 == NULL)
   {
      return FALSE;
   }

   if ((ret = cmsUtl_parseDNS(dnsServers1, dnsPri1, dnsSec1, FALSE)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsUtl_parseDNS returns error. ret=%d", ret);
      return FALSE;
   }
   if ((ret = cmsUtl_parseDNS(dnsServers2, dnsPri2, dnsSec2, FALSE)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsUtl_parseDNS returns error. ret=%d", ret);
      return FALSE;
   }

   if (!cmsNet_areIp6AddrEqual(dnsPri1, dnsPri2) ||
       !cmsNet_areIp6AddrEqual(dnsSec2, dnsSec2))
   {
      return FALSE;
   }

   return TRUE;

}  /* cmsNet_areIp6DnsEqual() */

CmsRet cmsNet_subnetIp6SitePrefix(const char *sp, UINT8 subnetId, UINT32 snPlen, char *snPrefix)
{
   char prefix[CMS_IPADDR_LENGTH];
   char address[CMS_IPADDR_LENGTH];
   UINT32 plen;
   struct in6_addr   in6Addr;
   CmsRet ret;

   if (snPrefix == NULL)
   {
      cmsLog_error("snPrefix is NULL.");
      return CMSRET_INVALID_ARGUMENTS;
   }
   *snPrefix = '\0';

   if (IS_EMPTY_STRING(sp))
   {
      cmsLog_error("sp is empty. do nothing.");
      return CMSRET_SUCCESS;
   }

   /* set a limitation to subnet prefix length to be at 8 bit boundary */
   if (snPlen % 8)
   {
      cmsLog_error("snPlen is not at 8 bit boundary. snPlen=%d", snPlen);
      return CMSRET_INVALID_ARGUMENTS;
   }

   if ((ret = cmsUtl_parsePrefixAddress(sp, address, &plen)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsUtl_parsePrefixAddress returns error. ret=%d", ret);
      return CMSRET_INVALID_ARGUMENTS;
   }

   if ((snPlen < plen) || (subnetId >= (1<<(snPlen-plen))))
   {
      cmsLog_error("plen=%d snPlen=%d subnetId=%d", plen, snPlen, subnetId);
      return CMSRET_INVALID_ARGUMENTS;
   }
   
   if ((ret = cmsUtl_getAddrPrefix(address, plen, prefix)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsUtl_getAddrPrefix returns error. ret=%d", ret);
      return ret;
   }

   if (inet_pton(AF_INET6, prefix, &in6Addr) <= 0)
   {
      cmsLog_error("inet_pton returns error");
      return CMSRET_INVALID_ARGUMENTS;
   }

   /* subnet the site prefix */
   in6Addr.s6_addr[(snPlen-1)/8] |= subnetId;

   if (inet_ntop(AF_INET6, &in6Addr, snPrefix, CMS_IPADDR_LENGTH) == NULL)
   {
      cmsLog_error("inet_ntop returns error");
      return CMSRET_INTERNAL_ERROR;
   }

   return CMSRET_SUCCESS;

}  /* cmsNet_subnetIp6SitePrefix() */

CmsRet cmsUtl_prefixMacToAddress(const char *prefix, UINT8 *mac, char *addr)
{
   struct in6_addr in6_addr;
   SINT32 i;

   cmsLog_debug("prefix<%s>", prefix);

   if (inet_pton(AF_INET6, prefix, &in6_addr) <= 0)
   {
      cmsLog_error("inet_pton returns error");
      return CMSRET_INVALID_ARGUMENTS;
   }

   for ( i = 8; i <= 15; i++ ) 
   {
      if (in6_addr.s6_addr[i] != 0)
      {
         cmsLog_error("prefix is not 0 at 64 LSB");
         return CMSRET_INVALID_ARGUMENTS;
      };
   };

   /* create EUI-64 from MAC-48 */
   in6_addr.s6_addr[ 8] = mac[0] ^ 0x02;
   in6_addr.s6_addr[ 9] = mac[1];
   in6_addr.s6_addr[10] = mac[2];
   in6_addr.s6_addr[11] = 0xff;
   in6_addr.s6_addr[12] = 0xfe;
   in6_addr.s6_addr[13] = mac[3];
   in6_addr.s6_addr[14] = mac[4];
   in6_addr.s6_addr[15] = mac[5];

   if (inet_ntop(AF_INET6, &in6_addr, addr, CMS_IPADDR_LENGTH) == NULL)
   {
      cmsLog_error("inet_ntop returns error");
      return CMSRET_INTERNAL_ERROR;
   }

   cmsLog_debug("addr<%s>", addr);

   return CMSRET_SUCCESS;
}
#endif  /* SUPPORT_IPV6 */



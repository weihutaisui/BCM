/***********************************************************************
 *
 *  Copyright (c) 2006  Broadcom Corporation
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
#ifdef SUPPORT_IPSEC

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/route.h>
#include <fcntl.h>
#include <unistd.h>

#include "cms_core.h"
#include "cms_util.h"
#include "cms_eid.h"
#include "cms_dal.h"
#include "rcl.h"
#include "prctl.h"
#include "rut_util.h"
#include "rut_ipsec.h"
#include "rut_wan.h"
#include "qdm_intf.h"

#define CERT_PATH "/var/cert"

int rutIPSec_numTunnelEntries(void);
int rutIPSec_numTunnelEntries_igd(void);
int rutIPSec_numTunnelEntries_dev2(void);
#if defined(SUPPORT_DM_LEGACY98)
#define rutIPSec_numTunnelEntries()  rutIPSec_numTunnelEntries_igd()
#elif defined(SUPPORT_DM_HYBRID)
#define rutIPSec_numTunnelEntries()  rutIPSec_numTunnelEntries_igd()
#elif defined(SUPPORT_DM_PURE181)
#define rutIPSec_numTunnelEntries()  rutIPSec_numTunnelEntries_dev2()
#elif defined(SUPPORT_DM_DETECT)
#define rutIPSec_numTunnelEntries()   (cmsMdm_isDataModelDevice2() ?   \
                                  rutIPSec_numTunnelEntries_dev2()   : \
                                  rutIPSec_numTunnelEntries_igd())
#endif

int rutIPSec_numTunnelEntries_igd()
{
   int numTunCfg = 0;
#ifdef DMP_X_BROADCOM_COM_IPSEC_1
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   IPSecCfgObject *ipsecObj = NULL;

   while ( (ret = cmsObj_getNext
         (MDMOID_IP_SEC_CFG, &iidStack, (void **) &ipsecObj)) == CMSRET_SUCCESS)
   {
      numTunCfg++;
      // Free the mem allocated this object by the get API.
      cmsObj_free((void **) &ipsecObj);
   }
#endif
   return numTunCfg;
}

int rutIPSec_numTunnelEntries_dev2()
{
   int numTunCfg = 0;
#ifdef DMP_DEVICE2_IPSEC_1
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2IpsecFilterObject *filterObj = NULL;

   while (cmsObj_getNextFlags(MDMOID_DEV2_IPSEC_FILTER, &iidStack, 
                    OGF_NO_VALUE_UPDATE, (void **)&filterObj) == CMSRET_SUCCESS)
   {
      if (filterObj->enable)
      {
         numTunCfg++;
      }
      cmsObj_free((void **)&filterObj);
   }
#endif

   return numTunCfg;
}

CmsRet rutIPSec_activateTunnel_igd(void)
{
   rutIPSec_doSetkeyRacoon();
   rutIPSec_doRestartRacoon();

   return CMSRET_SUCCESS;
}

CmsRet rutIPSec_getWanIP(const char *wanIntf, char *ipaddr, int *firewall, UBOOL8 isIPv4)
{
   char ifNameBuf[CMS_IFNAME_LENGTH]={0};
   const char *searchIfName;
   char *ptr;

   if ( (wanIntf == NULL) || (0 == strlen(wanIntf)))
   {
      if ( FALSE == rutWan_findFirstIpvxRoutedAndConnected(isIPv4 ? CMS_AF_SELECT_IPV4 : CMS_AF_SELECT_IPV6, ifNameBuf) )
      {
         return CMSRET_OBJECT_NOT_FOUND;
      }
      searchIfName = ifNameBuf;
   }
   else
   {
      searchIfName = wanIntf;
      if ( FALSE == qdmIpIntf_isWanInterfaceUpLocked(searchIfName, isIPv4) )
      {
         /* the provided interface is not up */
         return CMSRET_OBJECT_NOT_FOUND;
      }
   
      if ( qdmIpIntf_isWanInterfaceBridgedLocked(searchIfName) )
      {
         /* the provided interface is not routed */
         return CMSRET_INVALID_ARGUMENTS;
      }
   }

   if ( isIPv4 )
   {
      if ( CMSRET_SUCCESS != qdmIpIntf_getIpv4AddressByNameLocked(searchIfName, ipaddr) )
      {
         return CMSRET_OBJECT_NOT_FOUND;
      }
   }
   else
   {
      if ( CMSRET_SUCCESS != qdmIpIntf_getIpv6AddressByNameLocked(searchIfName, ipaddr) )
      {
         return CMSRET_OBJECT_NOT_FOUND;
      }
   }

   ptr = cmsUtl_strstr(ipaddr, "/");
   if ( ptr )
   {
      *ptr = '\0';
   }

   *firewall = qdmIpIntf_isFirewallEnabledOnIntfnameLocked(searchIfName) || 
               qdmIpIntf_isNatEnabledOnIntfNameLocked(searchIfName);

   return CMSRET_SUCCESS;

}

int rutIPSec_calPrefixLen(char *ipAddr)
{
   UINT32 addr = ntohl(inet_addr(ipAddr));
   int i;

   if (addr == INADDR_NONE) {
      return 32;
   } else {
      for (i=0; i<=32;i++) {
         if ((addr>>i) & 1) {
            break;
         }
     }
     return 32 - i;
   }
}

CmsRet rutIPSec_doSetkeyRacoon_igd(void)
{
#ifdef DMP_X_BROADCOM_COM_IPSEC_1
   CmsRet ret = CMSRET_SUCCESS;
   FILE *fp;
   int local_prefixlen;
   int remote_prefixlen;
   char wanIP[CMS_IPADDR_LENGTH];
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   IPSecCfgObject *ipsecObj = NULL;
   int firewall;
   int isIpV4;

   /* write out setkey file */
   fp = fopen("/var/setkey.conf", "w");
   if (fp == NULL) {
      fprintf(stderr, "app/ipsec: unable to open file\n");
      return CMSRET_INTERNAL_ERROR;
   }
   fprintf(fp, "flush;\n");
   fprintf(fp, "spdflush;\n");

   INIT_INSTANCE_ID_STACK(&iidStack);
   while ( 1 )
   {
      ret = cmsObj_getNext(MDMOID_IP_SEC_CFG, &iidStack, (void **)&ipsecObj);
      if ( ret != CMSRET_SUCCESS )
      {
         break;
      }

      wanIP[0] = '\0';
      if ( 0 == cmsUtl_strcmp(ipsecObj->ipVer, "6") )
      {
         isIpV4 = 0;
      }
      else
      {
         isIpV4 = 1;
      }
      ret = rutIPSec_getWanIP(ipsecObj->localGwIf, wanIP, &firewall, isIpV4);
      if (ret != CMSRET_SUCCESS)
      {
         cmsObj_free((void **)&ipsecObj);
         continue;
      }

      if (strcmp(ipsecObj->keyExM, "manual") == 0)
      {
         if(strcmp(ipsecObj->tunMode, "esp") == 0)
         {
            fprintf(fp, "add %s %s esp 0x%s -m tunnel -E %s 0x%s -A %s 0x%s;\n",
                 wanIP, ipsecObj->remoteGWAddress, ipsecObj->SPI,
                 ipsecObj->manualEncryptionAlgo, ipsecObj->manualEncryptionKey,
                 ipsecObj->manualAuthAlgo, ipsecObj->manualAthKey);
            fprintf(fp, "add %s %s esp 0x%s -m tunnel -E %s 0x%s -A %s 0x%s;\n",
                 ipsecObj->remoteGWAddress, wanIP, ipsecObj->SPI,
                 ipsecObj->manualEncryptionAlgo, ipsecObj->manualEncryptionKey,
                 ipsecObj->manualAuthAlgo, ipsecObj->manualAthKey);
         }
         else
         {
            fprintf(fp, "add %s %s ah 0x%s -m tunnel -E %s 0x%s -A %s 0x%s;\n",
                 wanIP, ipsecObj->remoteGWAddress, ipsecObj->SPI,
                 ipsecObj->manualEncryptionAlgo, ipsecObj->manualEncryptionKey,
                 ipsecObj->manualAuthAlgo, ipsecObj->manualAthKey);
            fprintf(fp, "add %s %s ah 0x%s -m tunnel -E %s 0x%s -A %s 0x%s;\n",
                 ipsecObj->remoteGWAddress, wanIP, ipsecObj->SPI,
                 ipsecObj->manualEncryptionAlgo, ipsecObj->manualEncryptionKey,
                 ipsecObj->manualAuthAlgo, ipsecObj->manualAthKey);
         }
      }

      // Free the mem allocated this object by the get API.
      cmsObj_free((void **) &ipsecObj);
   }

   INIT_INSTANCE_ID_STACK(&iidStack);
   while ( 1 )
   {
      ret = cmsObj_getNext(MDMOID_IP_SEC_CFG, &iidStack, (void **)&ipsecObj);
      if ( ret != CMSRET_SUCCESS )
      {
         break;
      }

      wanIP[0] = '\0';
      if ( 0 == cmsUtl_strcmp(ipsecObj->ipVer, "6") )
      {
         isIpV4 = 0;
      }
      else
      {
         isIpV4 = 1;
      }
      ret = rutIPSec_getWanIP(ipsecObj->localGwIf, wanIP, &firewall, isIpV4);
      if (ret != CMSRET_SUCCESS)
      {
         cmsObj_free((void **)&ipsecObj);
         continue;
      }

      if ( 1 == isIpV4 )
      {
         if (strcmp(ipsecObj->localIPMode, "subnet") == 0) {
            local_prefixlen = rutIPSec_calPrefixLen(ipsecObj->localMask);
         }
         else {
            local_prefixlen = 32;
         }
         if (strcmp(ipsecObj->remoteIPMode, "subnet") == 0) {
            remote_prefixlen = rutIPSec_calPrefixLen(ipsecObj->remoteMask);
         }
         else {
            remote_prefixlen = 32;
         }
      }
      else
      {
         if (strcmp(ipsecObj->localIPMode, "subnet") == 0) {
            local_prefixlen = atoi(ipsecObj->localMask);
         }
         else {
            local_prefixlen = 128;
         }
         if (strcmp(ipsecObj->remoteIPMode, "subnet") == 0) {
            remote_prefixlen = atoi(ipsecObj->remoteMask);
         }
         else {
            remote_prefixlen = 128;
         }
      }

      if(strcmp(ipsecObj->tunMode, "esp") == 0) {
         fprintf(fp, "spdadd %s/%d %s/%d any -P out ipsec esp/tunnel/%s-%s/require;\n",
              ipsecObj->localIPAddress, local_prefixlen, ipsecObj->remoteIPAddress, 
              remote_prefixlen, wanIP, ipsecObj->remoteGWAddress);
         fprintf(fp, "spdadd %s/%d %s/%d any -P in ipsec esp/tunnel/%s-%s/require;\n",
              ipsecObj->remoteIPAddress, remote_prefixlen, ipsecObj->localIPAddress, 
              local_prefixlen, ipsecObj->remoteGWAddress, wanIP);
      }
      else {
         fprintf(fp, "spdadd %s/%d %s/%d any -P out ipsec ah/tunnel/%s-%s/require;\n",
              ipsecObj->localIPAddress, local_prefixlen, ipsecObj->remoteIPAddress, 
              remote_prefixlen, wanIP, ipsecObj->remoteGWAddress);
         fprintf(fp, "spdadd %s/%d %s/%d any -P in ipsec ah/tunnel/%s-%s/require;\n",
              ipsecObj->remoteIPAddress, remote_prefixlen, ipsecObj->localIPAddress, 
              local_prefixlen, ipsecObj->remoteGWAddress, wanIP);
      }
      if ( (1 == isIpV4) && (1 == firewall) )
      {
          char cmd[BUFLEN_256];
          sprintf(cmd, "iptables -t nat -I POSTROUTING 1 -s %s/%d -o %s -d %s/%d -j ACCEPT",
                       ipsecObj->localIPAddress, local_prefixlen, ipsecObj->localGwIf, 
                       ipsecObj->remoteIPAddress, remote_prefixlen);
          rut_doSystemAction("rut", cmd);
      }

      // Free the mem allocated this object by the get API.
      cmsObj_free((void **) &ipsecObj);

   }
   fclose(fp);

   //racoon
   fp = fopen("/var/racoon.conf", "w");
   if (fp == NULL) {
      fprintf(stderr, "app/ipsec: unable to open file\n");
      return CMSRET_INTERNAL_ERROR;
   }
   fprintf(fp, "path pre_shared_key \"/var/psk.txt\";\n");
   fprintf(fp, "path certificate \"%s\";\n\n", CERT_PATH);

   INIT_INSTANCE_ID_STACK(&iidStack);
   while ( 1 )
   {
      ret = cmsObj_getNext(MDMOID_IP_SEC_CFG, &iidStack, (void **)&ipsecObj);
      if ( ret != CMSRET_SUCCESS )
      {
         break;
      }

      wanIP[0] = '\0';
      if ( 0 == cmsUtl_strcmp(ipsecObj->ipVer, "6") )
      {
         isIpV4 = 0;
      }
      else
      {
         isIpV4 = 1;
      }
      ret = rutIPSec_getWanIP(ipsecObj->localGwIf, wanIP, &firewall, isIpV4);
      if (ret != CMSRET_SUCCESS)
      {
         cmsObj_free((void **)&ipsecObj);
         continue;
      }
      
      if (strcmp(ipsecObj->keyExM, "auto") == 0) {
         if ( 1 == isIpV4 )
         {
            if (strcmp(ipsecObj->localIPMode, "subnet") == 0) {
               local_prefixlen = rutIPSec_calPrefixLen(ipsecObj->localMask);
            }
            else {
               local_prefixlen = 32;
            }
            if (strcmp(ipsecObj->remoteIPMode, "subnet") == 0) {
               remote_prefixlen = rutIPSec_calPrefixLen(ipsecObj->remoteMask);
            }
            else {
               remote_prefixlen = 32;
            }
         }
         else
         {
            if (strcmp(ipsecObj->localIPMode, "subnet") == 0) {
               local_prefixlen = atoi(ipsecObj->localMask);
            }
            else {
               local_prefixlen = 128;
            }
            if (strcmp(ipsecObj->remoteIPMode, "subnet") == 0) {
               remote_prefixlen = atoi(ipsecObj->remoteMask);
            }
            else {
               remote_prefixlen = 128;
            }
         }

         fprintf(fp, "remote %s {\n", ipsecObj->remoteGWAddress);
         fprintf(fp, "exchange_mode %s;\n", ipsecObj->ph1Mode);
         fprintf(fp, "lifetime time %d sec;\n", ipsecObj->ph1KeyTime);
         fprintf(fp, "proposal_check obey;\n");
         fprintf(fp, "dpd_delay 30;\n");
         fprintf(fp, "dpd_retry 5;\n");
         fprintf(fp, "dpd_maxfail 5;\n");
         if (strcmp(ipsecObj->authM, "pre_shared_key") == 0) {
            fprintf(fp, "proposal {\n");
            fprintf(fp, "encryption_algorithm %s;\n", ipsecObj->ph1EncryptionAlo);
            fprintf(fp, "hash_algorithm %s;\n", ipsecObj->ph1IntegrityAlgo);
            fprintf(fp, "authentication_method %s;\n", "pre_shared_key");
            fprintf(fp, "dh_group %s;\n", ipsecObj->ph1DHGroup);
            fprintf(fp, "}\n");
            fprintf(fp, "}\n\n");
         }
         else if (strcmp(ipsecObj->authM, "certificate") == 0) {
            fprintf(fp, "verify_cert on;\n");
            fprintf(fp, "my_identifier asn1dn;\n");
            fprintf(fp, "peers_identifier asn1dn;\n");
            fprintf(fp, "certificate_type x509 \"%s.cert\" \"%s.priv\";\n",
                    ipsecObj->certificateName, ipsecObj->certificateName);
            fprintf(fp, "proposal {\n");
            fprintf(fp, "encryption_algorithm %s;\n", ipsecObj->ph1EncryptionAlo);
            fprintf(fp, "hash_algorithm %s;\n", ipsecObj->ph1IntegrityAlgo);
            fprintf(fp, "authentication_method %s;\n", "rsasig");
            fprintf(fp, "dh_group %s;\n", ipsecObj->ph1DHGroup);
            fprintf(fp, "}\n");
            fprintf(fp, "}\n\n");
         }
         /* unknown type */
         fprintf(fp, "sainfo address %s/%d any address %s/%d any {\n",
                 ipsecObj->localIPAddress, local_prefixlen, 
                 ipsecObj->remoteIPAddress, remote_prefixlen);
         if (strcmp(ipsecObj->perfectFSEn, "enable") == 0) {
            fprintf(fp, "pfs_group %s;\n", ipsecObj->ph2DHGroup);
         }
         fprintf(fp, "lifetime time %d sec;\n", ipsecObj->ph2KeyTime);
         fprintf(fp, "encryption_algorithm %s;\n", ipsecObj->ph2EncryptionAlo);
         fprintf(fp, "authentication_algorithm %s;\n", ipsecObj->ph2IntegrityAlgo);
         fprintf(fp, "compression_algorithm deflate;\n");
         fprintf(fp, "}\n\n");
         fprintf(fp, "listen\n");
         fprintf(fp, "{\n");
         fprintf(fp, "isakmp %s [500];\n", wanIP);
         fprintf(fp, "isakmp_natt %s [4500];\n", wanIP);
         fprintf(fp, "}\n");
      }
      // Free the mem allocated this object by the get API.
      cmsObj_free((void **) &ipsecObj);
   }
   fclose(fp);

   // psk
   fp = fopen("/var/psk.txt", "w");
   if (fp == NULL) {
      fprintf(stderr, "app/ipsec: unable to open file\n");
      return CMSRET_INTERNAL_ERROR;
   }

   INIT_INSTANCE_ID_STACK(&iidStack);
   while ( 1 )
   {
      ret = cmsObj_getNext(MDMOID_IP_SEC_CFG, &iidStack, (void **)&ipsecObj);
      if ( ret != CMSRET_SUCCESS )
      {
         break;
      }

      if (strcmp(ipsecObj->keyExM, "auto") == 0 &&
          strcmp(ipsecObj->authM, "pre_shared_key") == 0) {
         fprintf(fp, "%s %s\n", ipsecObj->remoteGWAddress, ipsecObj->PSK);
      }
      // Free the mem allocated this object by the get API.
      cmsObj_free((void **) &ipsecObj);
   }
   fclose(fp);
   prctl_runCommandInShellBlocking("chmod 600 /var/psk.txt");
#endif

   return CMSRET_SUCCESS;
}

CmsRet rutIPSec_doRestartRacoon(void)
{
   CmsRet ret = CMSRET_SUCCESS;

   prctl_runCommandInShellWithTimeout("killall racoon 2>/dev/null");

   sleep(1);

   if (rutIPSec_numTunnelEntries() == 0) {
#ifdef SUPPORT_SPUCTL
      prctl_runCommandInShellBlocking("spuctl stop");
#else
      prctl_runCommandInShellBlocking("setkey -FP");
      prctl_runCommandInShellBlocking("setkey -F");
#endif
      return ret;
   }

#ifdef SUPPORT_SPUCTL
   prctl_runCommandInShellBlocking("spuctl start");
#else
   prctl_runCommandInShellBlocking("setkey -FP");
   prctl_runCommandInShellBlocking("setkey -F");
   prctl_runCommandInShellBlocking("setkey -f /var/setkey.conf");
#endif

   prctl_runCommandInShellWithTimeout("racoon -f /var/racoon.conf");

#ifdef tmp_debug
   // turn on logging with debug msg
   // mwang: be careful when using the -dddd option.  The debug file can use up all memory
   // on the modem.
   prctl_runCommandInShellBlocking("rm /var/racoon.log");
   prctl_runCommandInShellWithTimeout("racoon -v -dddd -l /var/racoon.log -f /var/racoon.conf");
#endif

   prctl_runCommandInShellBlocking("setkey -f /var/setkey.conf");
   return ret;
}

#endif /* SUPPORT_IPSEC */

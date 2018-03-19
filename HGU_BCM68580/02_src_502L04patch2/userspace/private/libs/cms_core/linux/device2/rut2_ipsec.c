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


CmsRet rutIPSec_doSetkeyRacoon_dev2(void)
{
#ifdef DMP_DEVICE2_IPSEC_1
   FILE *fp, *fp1, *fp2;
   int local_prefixlen;
   int remote_prefixlen;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2IpsecFilterObject *filterObj = NULL;

   fp = fopen("/var/setkey.conf", "w");
   if (fp == NULL) {
      fprintf(stderr, "app/ipsec: unable to open file\n");
      return CMSRET_INTERNAL_ERROR;
   }
   fprintf(fp, "flush;\n");
   fprintf(fp, "spdflush;\n");

   fp1 = fopen("/var/racoon.conf", "w");
   if (fp1 == NULL) {
      fprintf(stderr, "app/ipsec: unable to open file\n");
      return CMSRET_INTERNAL_ERROR;
   }
   fprintf(fp1, "path pre_shared_key \"/var/psk.txt\";\n");
   fprintf(fp1, "path certificate \"%s\";\n\n", CERT_PATH);

   fp2 = fopen("/var/psk.txt", "w");
   if (fp2 == NULL) {
      fprintf(stderr, "app/ipsec: unable to open file\n");
      return CMSRET_INTERNAL_ERROR;
   }

   INIT_INSTANCE_ID_STACK(&iidStack);

   while (cmsObj_getNextFlags(MDMOID_DEV2_IPSEC_FILTER, &iidStack, 
                    OGF_NO_VALUE_UPDATE, (void **)&filterObj) == CMSRET_SUCCESS)
   {
      Dev2IpsecProfileObject *profileObj = NULL;
      MdmPathDescriptor pathDesc;
      char wanIP[CMS_IPADDR_LENGTH];
      int firewall;
      int isIpV4;

      if (filterObj->enable != TRUE)
      {
         cmsObj_free((void **)&filterObj);
         continue;
      }

      INIT_PATH_DESCRIPTOR(&pathDesc);
      if (cmsMdm_fullPathToPathDescriptor(filterObj->profile, &pathDesc) != 
          CMSRET_SUCCESS)
      {
         cmsLog_error("cmsMdm_fullPathToPathDescriptor failed for %s", 
                      filterObj->profile);
         cmsObj_free((void **)&filterObj);
         continue;
      }
      if (cmsObj_get(pathDesc.oid, &pathDesc.iidStack, OGF_NO_VALUE_UPDATE,
                           (void *)&profileObj) != CMSRET_SUCCESS)
      {
         cmsLog_error("cannot get associated profileObj");
         cmsObj_free((void **)&filterObj);
         continue;
      }

      isIpV4 = (strchr(profileObj->remoteEndpoints, ':') == NULL);
      if (rutIPSec_getWanIP(profileObj->X_BROADCOM_COM_LocalIfName, wanIP,
                            &firewall, isIpV4) != CMSRET_SUCCESS)
      {
         cmsObj_free((void **)&filterObj);
         cmsObj_free((void **)&profileObj);
         continue;
      }

      if ( isIpV4 == TRUE )
      {
         local_prefixlen = rutIPSec_calPrefixLen(filterObj->sourceMask);
         remote_prefixlen = rutIPSec_calPrefixLen(filterObj->destMask);
      }
      else
      {
         local_prefixlen = atoi(filterObj->sourceMask);
         remote_prefixlen = atoi(filterObj->destMask);
      }

      if(!cmsUtl_strcmp(profileObj->X_BROADCOM_COM_KeyExchangeMode, "auto"))
      {
         Ikev1CfgObject *ikeObj = NULL;

         if (cmsObj_get(MDMOID_IKEV1_CFG, &pathDesc.iidStack,
                     OGF_NO_VALUE_UPDATE, (void *)&ikeObj) != CMSRET_SUCCESS)
         {
            cmsLog_error("cannot get ikeObj");
            cmsObj_free((void **)&filterObj);
            cmsObj_free((void **)&profileObj);
            continue;
         }

         fprintf(fp1, "remote %s {\n", profileObj->remoteEndpoints);
         fprintf(fp1, "exchange_mode %s;\n", ikeObj->phase1Mode);
         fprintf(fp1, "lifetime time %d sec;\n", ikeObj->phase1KeyTime);
         fprintf(fp1, "proposal_check obey;\n");
         fprintf(fp,  "dpd_delay 30;\n");
         fprintf(fp,  "dpd_retry 5;\n");
         fprintf(fp,  "dpd_maxfail 5;\n");
         if (!cmsUtl_strcmp(ikeObj->authenticationMethod, "pre_shared_key")) 
         {
            fprintf(fp1, "proposal {\n");
            fprintf(fp1, "encryption_algorithm %s;\n",
                    ikeObj->phase1AllowedEncryptionAlgorithms);
            fprintf(fp1, "hash_algorithm %s;\n",
                    ikeObj->phase1AllowedIntegrityAlgorithms);
            fprintf(fp1, "authentication_method %s;\n", "pre_shared_key");
            fprintf(fp1, "dh_group %s;\n",
                    ikeObj->phase1AllowedDiffieHellmanGroupTransforms);
            fprintf(fp1, "}\n");
            fprintf(fp1, "}\n\n");

            /* psk */
            fprintf(fp2, "%s %s\n",
                    profileObj->remoteEndpoints, ikeObj->preSharedKey);
         }
         else if (!cmsUtl_strcmp(ikeObj->authenticationMethod, "certificate"))
         {
            fprintf(fp1, "verify_cert on;\n");
            fprintf(fp1, "my_identifier asn1dn;\n");
            fprintf(fp1, "peers_identifier asn1dn;\n");
            fprintf(fp1, "certificate_type x509 \"%s.cert\" \"%s.priv\";\n",
                    ikeObj->certificateName, ikeObj->certificateName);
            fprintf(fp1, "proposal {\n");
            fprintf(fp1, "encryption_algorithm %s;\n",
                    ikeObj->phase1AllowedEncryptionAlgorithms);
            fprintf(fp1, "hash_algorithm %s;\n",
                    ikeObj->phase1AllowedIntegrityAlgorithms);
            fprintf(fp1, "authentication_method %s;\n", "rsasig");
            fprintf(fp1, "dh_group %s;\n",
                    ikeObj->phase1AllowedDiffieHellmanGroupTransforms);
            fprintf(fp1, "}\n");
            fprintf(fp1, "}\n\n");
         }
         /* unknown type */
         fprintf(fp1, "sainfo address %s/%d any address %s/%d any {\n",
                 filterObj->sourceIP, local_prefixlen, 
                 filterObj->destIP, remote_prefixlen);
         if (ikeObj->perfectFSEn)
         {
            fprintf(fp1, "pfs_group %s;\n",
                    ikeObj->phase2AllowedDiffieHellmanGroupTransforms);
         }
         fprintf(fp1, "lifetime time %d sec;\n", ikeObj->phase2KeyTime);
         fprintf(fp1, "encryption_algorithm %s;\n",
                 ikeObj->phase2AllowedEncryptionAlgorithms);
         fprintf(fp1, "authentication_algorithm %s;\n",
                 ikeObj->phase2AllowedIntegrityAlgorithms);
         fprintf(fp1, "compression_algorithm deflate;\n");
         fprintf(fp1, "}\n\n");
         fprintf(fp, "listen\n");
         fprintf(fp, "{\n");
         fprintf(fp, "isakmp %s [500];\n", wanIP);
         fprintf(fp, "isakmp_natt %s [4500];\n", wanIP);
         fprintf(fp, "}\n");

         cmsObj_free((void **)&ikeObj);
      }
      else if (!cmsUtl_strcmp(profileObj->X_BROADCOM_COM_KeyExchangeMode,
                              "manual"))
      {
         ManualModeCfgObject *manualObj = NULL;

         if (cmsObj_get(MDMOID_MANUAL_MODE_CFG, &pathDesc.iidStack,
                     OGF_NO_VALUE_UPDATE, (void *)&manualObj) != CMSRET_SUCCESS)
         {
            cmsLog_error("cannot get manualObj");
            cmsObj_free((void **)&filterObj);
            cmsObj_free((void **)&profileObj);
            continue;
         }

         if (!cmsUtl_strcmp(profileObj->protocol, "ESP"))
         {
            fprintf(fp, "add %s %s esp 0x%d -m tunnel -E %s 0x%s -A %s 0x%s;\n",
                 wanIP, profileObj->remoteEndpoints, manualObj->SPI,
                 manualObj->allowedEncryptionAlgorithms,
                 manualObj->encryptionKey,
                 manualObj->allowedIntegrityAlgorithms,
                 manualObj->authenticationKey);
            fprintf(fp, "add %s %s esp 0x%d -m tunnel -E %s 0x%s -A %s 0x%s;\n",
                 profileObj->remoteEndpoints, wanIP, manualObj->SPI,
                 manualObj->allowedEncryptionAlgorithms,
                 manualObj->encryptionKey,
                 manualObj->allowedIntegrityAlgorithms,
                 manualObj->authenticationKey);
         }
         else
         {
            fprintf(fp, "add %s %s ah 0x%d -m tunnel -E %s 0x%s -A %s 0x%s;\n",
                 wanIP, profileObj->remoteEndpoints, manualObj->SPI,
                 manualObj->allowedEncryptionAlgorithms,
                 manualObj->encryptionKey,
                 manualObj->allowedIntegrityAlgorithms,
                 manualObj->authenticationKey);
            fprintf(fp, "add %s %s ah 0x%d -m tunnel -E %s 0x%s -A %s 0x%s;\n",
                 profileObj->remoteEndpoints, wanIP, manualObj->SPI,
                 manualObj->allowedEncryptionAlgorithms,
                 manualObj->encryptionKey,
                 manualObj->allowedIntegrityAlgorithms,
                 manualObj->authenticationKey);
         }

         cmsObj_free((void **)&manualObj);
      }

      /* Write spd to setkey.conf */
      if (!cmsUtl_strcmp(profileObj->protocol, "ESP"))
      {
         fprintf(fp, "spdadd %s/%d %s/%d any -P out ipsec esp/tunnel/%s-%s/require;\n",
              filterObj->sourceIP, local_prefixlen, filterObj->destIP, 
              remote_prefixlen, wanIP, profileObj->remoteEndpoints);
         fprintf(fp, "spdadd %s/%d %s/%d any -P in ipsec esp/tunnel/%s-%s/require;\n",
              filterObj->destIP, remote_prefixlen, filterObj->sourceIP, 
              local_prefixlen, profileObj->remoteEndpoints, wanIP);
      }
      else
      {
         fprintf(fp, "spdadd %s/%d %s/%d any -P out ipsec ah/tunnel/%s-%s/require;\n",
              filterObj->sourceIP, local_prefixlen, filterObj->destIP, 
              remote_prefixlen, wanIP, profileObj->remoteEndpoints);
         fprintf(fp, "spdadd %s/%d %s/%d any -P in ipsec ah/tunnel/%s-%s/require;\n",
              filterObj->destIP, remote_prefixlen, filterObj->sourceIP, 
              local_prefixlen, profileObj->remoteEndpoints, wanIP);
      }

      if ( (isIpV4 == TRUE) && (1 == firewall) )
      {
          char cmd[BUFLEN_256];
          sprintf(cmd, "iptables -t nat -I POSTROUTING 1 -s %s/%d -o %s -d %s/%d -j ACCEPT",
                       filterObj->sourceIP, local_prefixlen,
                       profileObj->X_BROADCOM_COM_LocalIfName, 
                       filterObj->destIP, remote_prefixlen);
          rut_doSystemAction("rut", cmd);
      }

      cmsObj_free((void **)&profileObj);
      cmsObj_free((void **)&filterObj);
   }

   fclose(fp);
   fclose(fp1);
   fclose(fp2);

   prctl_runCommandInShellBlocking("chmod 600 /var/psk.txt");
#endif
   return CMSRET_SUCCESS;
}

CmsRet rutIPSec_activateTunnel_dev2(void)
{
   CmsRet ret = CMSRET_SUCCESS;

#ifdef DMP_DEVICE2_IPSEC_1
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2IpsecFilterObject *filterObj = NULL;

   while (cmsObj_getNextFlags(MDMOID_DEV2_IPSEC_FILTER, &iidStack, 
                 OGF_NO_VALUE_UPDATE, (void **)&filterObj) == CMSRET_SUCCESS)
   {
      if (filterObj->enable)
      {
         /* rcl handler will activate all tunnels. So break after found one */
         if ( (ret = cmsObj_set(filterObj, &iidStack)) != CMSRET_SUCCESS )
         {
            cmsLog_error("Failed to set filterObj");
         }

         cmsObj_free((void **) &filterObj);
         break;
      }

      cmsObj_free((void **)&filterObj);
   }

#endif
   return ret;
}

#endif /* SUPPORT_IPSEC */

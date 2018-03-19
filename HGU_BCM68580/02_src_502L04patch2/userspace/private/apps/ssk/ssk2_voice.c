/* 
* <:copyright-BRCM:2011:proprietary:standard
* 
*    Copyright (c) 2011 Broadcom 
*    All Rights Reserved
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


#ifdef DMP_DEVICE2_BASELINE_1

#ifdef BRCM_VOICE_SUPPORT

#include "cms.h"
#include "cms_util.h"
#include "cms_core.h"
#include "ssk.h"
#include "dal_voice.h"
#include "cms_qdm.h"


void initVoiceOnIntfUpLocked_dev2(UINT32 ipvx, const char *intfName, UBOOL8 isWan)
{
   char ipAddrBuf[CMS_IPADDR_LENGTH]={0};
   char ipFamilyBuf[BUFLEN_16]={0};
   DAL_VOICE_PARMS voiceParms;
   CmsRet ret;

   if ((ipvx & CMS_AF_SELECT_IPVX) == CMS_AF_SELECT_IPVX)
   {
      cmsLog_error("Cannot select both IPv4 and IPv6");
      return;
   }

   /*
    * Make sure the IPv4/IPv6 service that came up is supported by the
    * voice stack.
    */
   ret = dalVoice_GetIpFamily(&voiceParms, ipFamilyBuf, sizeof(ipFamilyBuf));
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get the IP address family for voice, ret=%d", ret);
      return;
   }
   else
   {
      if ((ipvx == CMS_AF_SELECT_IPV6) &&
          !cmsUtl_strstr( ipFamilyBuf, MDMVS_IPV6 ))
      {
         cmsLog_debug("IPv6 service %s is UP, but voice stack does not support/is not configured for IPV6, ignored",
                      intfName);
         return;
      }

      if ((ipvx == CMS_AF_SELECT_IPV4) &&
          !cmsUtl_strstr( ipFamilyBuf, MDMVS_IPV4 ))
      {
         cmsLog_debug("IPv4 service %s is UP, but voice stack is not configured for IPV4, ignored",
                      intfName);
         return;
      }
   }

   /* Get the IpAddr of the interface that just came up */
   ret = qdmIpIntf_getIpvxAddressByNameLocked(ipvx, intfName, ipAddrBuf);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get IPAddr (ipvx=%d) for %s, ret=%d", ipvx, intfName, ret);
      return;
   }

   if (isWan)
   {
      cmsLog_debug("WAN: intfName=%s %s isVoiceOnAnyWan=%d voiceWanIfName=%s",
                    intfName, ipAddrBuf, isVoiceOnAnyWan, voiceWanIfName);
      if (isVoiceOnAnyWan ||
          !cmsUtl_strcmp(intfName, voiceWanIfName))
      {
         setVoiceBoundIp(ipAddrBuf);
      }
   }
   else
   {
      /*
       * LAN side interface has come up.  TR98 code restricts LAN side intf
       * to br0 only, so check for that here.  Unlike TR98 code, the intfName
       * is already the bridge name, so we don't have to query the MDM to
       * go from layer 2 intfName to bridgeName.
       */
      cmsLog_debug("LAN: intfName=%s %s", intfName, ipAddrBuf);
      if (!cmsUtl_strcmp(intfName, "br0"))
      {
         setVoiceBoundIp(ipAddrBuf);
      }
   }
}


void initVoiceBoundIpIfWanSideUpLocked_dev2(const char *ifName)
{
   UBOOL8 found = FALSE;
   Dev2IpInterfaceObject *ipIntfObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UINT32 ipvx=0;
   char ipAddrBuf[CMS_IPADDR_LENGTH] = {0};
   char ipFamilyBuf[BUFLEN_16]={0};
   DAL_VOICE_PARMS voiceParms;
   CmsRet ret;

   cmsLog_debug("Entered: ifName=%s", ifName);

   /* First see if voice stacks supports IPv4 and IPv6 */
   ret = dalVoice_GetIpFamily(&voiceParms, ipFamilyBuf, sizeof(ipFamilyBuf));
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get the IP address family for voice, ret=%d", ret);
      return;
   }
   else
   {
      if (!cmsUtl_strstr( ipFamilyBuf, MDMVS_IPV6 ))
      {
         ipvx |= CMS_AF_SELECT_IPV6;
      }

      if (!cmsUtl_strstr( ipFamilyBuf, MDMVS_IPV4 ))
      {
         ipvx |= CMS_AF_SELECT_IPV4;
      }
   }

   /*
    * Walk through all IP.Interface objects, looking for a WAN/upstream,
    * non-bridged, matching ifName, and in service up state.
    */
   while (!found &&
          cmsObj_getNextFlags(MDMOID_DEV2_IP_INTERFACE, &iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **) &ipIntfObj) == CMSRET_SUCCESS)
   {
      if (ipIntfObj->X_BROADCOM_COM_Upstream &&
          !ipIntfObj->X_BROADCOM_COM_BridgeService &&
          (!cmsUtl_strcmp(ifName, MDMVS_ANY_WAN) || !cmsUtl_strcmp(ifName, ipIntfObj->name)))
      {
#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
         if ((ipvx & CMS_AF_SELECT_IPV6) &&
             !cmsUtl_strcmp(ipIntfObj->X_BROADCOM_COM_IPv6ServiceStatus, MDMVS_SERVICEUP))
         {
            found = TRUE;
            qdmIpIntf_getIpvxAddressByNameLocked(CMS_AF_SELECT_IPV6,
                                                 ipIntfObj->name, ipAddrBuf);
         }
#endif

         if (!found &&
             (ipvx & CMS_AF_SELECT_IPV4) &&
             !cmsUtl_strcmp(ipIntfObj->X_BROADCOM_COM_IPv4ServiceStatus, MDMVS_SERVICEUP))
         {
            found = TRUE;
            qdmIpIntf_getIpvxAddressByNameLocked(CMS_AF_SELECT_IPV4,
                                                 ipIntfObj->name, ipAddrBuf);
         }
      }

      cmsObj_free((void **) &ipIntfObj);
   }

   if( found == TRUE )
   {
      /* Set IP address for VOICE */
      setVoiceBoundIp(ipAddrBuf);
   }
   else
   {
      cmsLog_debug("No WAN intf or matching WAN intf up (%s)", ifName);
   }

   return;
}


void initVoiceBoundIpIfLanSideUpLocked_dev2(void)
{
   UINT32 ipvx=0;
   char ipAddrBuf[CMS_IPADDR_LENGTH] = {0};
   char ipFamilyBuf[BUFLEN_16]={0};
   DAL_VOICE_PARMS voiceParms;
   CmsRet ret;

   cmsLog_debug("Entered:");

   /* First see if voice stacks supports IPv4 and IPv6 */
   ret = dalVoice_GetIpFamily(&voiceParms, ipFamilyBuf, sizeof(ipFamilyBuf));
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get the IP address family for voice, ret=%d", ret);
      return;
   }
   else
   {
      if (!cmsUtl_strstr( ipFamilyBuf, MDMVS_IPV6 ))
      {
         ipvx |= CMS_AF_SELECT_IPV6;
      }

      if (!cmsUtl_strstr( ipFamilyBuf, MDMVS_IPV4 ))
      {
         ipvx |= CMS_AF_SELECT_IPV4;
      }
   }

   /* if br0 is UP, get IP addr based on ipvx flag.  Follow convention in
    * other parts of CMS and prefer IPv6 over IPv4 if both are available.
    */
#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
   if ((ipvx & CMS_AF_SELECT_IPV6) &&
       (qdmIpIntf_isIpv6ServiceUpLocked_dev2("br0", QDM_IPINTF_DIR_LAN)))
   {
      qdmIpIntf_getIpvxAddressByNameLocked(CMS_AF_SELECT_IPV6, "br0", ipAddrBuf);
   }
#endif

   if ((ipAddrBuf[0] == '\0') &&
       (ipvx & CMS_AF_SELECT_IPV4) &&
       (qdmIpIntf_isIpv4ServiceUpLocked_dev2("br0", QDM_IPINTF_DIR_LAN)))
   {
      qdmIpIntf_getIpvxAddressByNameLocked(CMS_AF_SELECT_IPV4, "br0", ipAddrBuf);
   }

   if(ipAddrBuf[0] != '\0')
   {
      /* Set IP address for VOICE */
      setVoiceBoundIp(ipAddrBuf);
   }

   return;
}

#endif   /* BRCM_VOICE_SUPPORT */

#endif   /* DMP_DEVICE2_BASELINE_1 */


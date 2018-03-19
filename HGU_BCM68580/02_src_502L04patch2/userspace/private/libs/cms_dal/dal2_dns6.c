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
:>
*/

#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"
#include "cms_qdm.h"
#include "dal.h"
#include "dal2_wan.h"


#ifdef DMP_DEVICE2_BASELINE_1    /* this file touches TR181 objects */

/* this file touches TR181 IPv6 objects */
#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1


CmsRet dalDns_setIpv6DnsInfo_dev2(const WEB_NTWK_VAR *webVar)
{
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("Enter: dnsType=%s dns6Ifc=%s dns6Pri=%s",
                webVar->dns6Type, webVar->dns6Ifc, webVar->dns6Pri);
   
   if (IS_EMPTY_STRING(webVar->dns6Type))
   {
      cmsLog_error("dnsType is empty");
      return CMSRET_INVALID_ARGUMENTS;
   }

   if (!cmsUtl_strcmp(webVar->dns6Type, MDMVS_STATIC) && IS_EMPTY_STRING(webVar->dns6Pri))
   {
      cmsLog_notice("dnsType==MDMVS_STATIC without primary DNS server");
      return ret;
   }
   
   if (!cmsUtl_strcmp(webVar->dns6Type, MDMVS_DHCP) && IS_EMPTY_STRING(webVar->dns6Ifc))
   {
      cmsLog_error("dnsType==DHCP but dnsIfc is empty.");
      return CMSRET_INVALID_ARGUMENTS;
   }

   if (!cmsUtl_strcmp(webVar->dns6Type, MDMVS_STATIC))
   {
      ret = dalDns_setStaticIpvxDnsServers(CMS_AF_SELECT_IPV6, webVar->dns6Pri, webVar->dns6Sec);
   }
   else if (!cmsUtl_strcmp(webVar->dns6Type, MDMVS_DHCP)  && !IS_EMPTY_STRING(webVar->dns6Ifc))
   {
      /* set this list of ifNames to search for default system DNS */
      /*
       * If we want to use DnsIfNameList, we must delete any static DNS
       * servers because static DNS servers have higher precedence.
       */
      dalDns_deleteAllStaticIpvxDnsServers(CMS_AF_SELECT_IPV6);
      ret = dalDns_setIpvxDnsIfNameList(CMS_AF_SELECT_IPV6, webVar->dns6Ifc);
   }
   else
   {
      cmsLog_error("Not enough dns info, ifcsList=%s primary=%s", webVar->dns6Ifc, webVar->dns6Pri);
      ret = CMSRET_INVALID_PARAM_VALUE;
   }

   cmsLog_debug("Exit. ret %d", ret);
   
   return ret;
   
}

#endif  /* DMP_X_BROADCOM_COM_DEV2_IPV6_1 */

#endif /* DMP_DEVICE2_BASELINE_1 */


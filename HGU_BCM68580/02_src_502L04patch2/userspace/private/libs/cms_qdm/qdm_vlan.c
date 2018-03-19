/***********************************************************************
 *
 *  Copyright (c) 2013  Broadcom Corporation
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
 
#ifdef DMP_BASELINE_1


#include "cms.h"
#include "cms_mdm.h"
#include "cms_obj.h"
#include "cms_phl.h"
#include "cms_qdm.h"
#include "cms_util.h"
#include "cms_core.h"


/* in qdm_ipintf.c */
extern CmsRet qdmPrivate_getLanWanConnObj(const char *intfName,
                                          WanIpConnObject **wanIpConnObj,
                                          WanPppConnObject **wanPppConnObj,
                                          LanIpIntfObject **lanIpIntfObj,
                                    IPv6LanIntfAddrObject **ipv6LanIpAddrObj);




#ifdef SUPPORT_LANVLAN

UBOOL8 qdmVlan_isLanVlanPresentLocked_igd()
{
   L2BridgingFilterObject    *bObj = NULL;
   InstanceIdStack           iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 present=FALSE;

   cmsLog_debug("Entered:");

   while (!present &&
          cmsObj_getNextFlags(MDMOID_L2_BRIDGING_FILTER, &iidStack,
                   OGF_NO_VALUE_UPDATE, (void **)&bObj) == CMSRET_SUCCESS)
   {
      if (bObj->X_BROADCOM_COM_VLANIDFilter > 0 &&
          bObj->filterBridgeReference >= 0)
      {
         cmsLog_debug("Found LAN VLAN filter=%d bridgeRef=%d",
                      bObj->X_BROADCOM_COM_VLANIDFilter,
                      bObj->filterBridgeReference);
         present = TRUE;
      }
      cmsObj_free((void **)&bObj);
   }

   return present;
}

#endif  /* SUPPORT_LANVLAN */


#ifdef SUPPORT_WANVLANMUX
SINT32 qdmVlan_getVlanIdByIntfNameLocked_igd(const char *intfName)
{
   WanIpConnObject *wanIpConnObj=NULL;
   WanPppConnObject *wanPppConnObj=NULL;
   CmsRet ret;
   SINT32 vlanId=-1;

   cmsLog_debug("Entered: intfName=%s", intfName);

   ret = qdmPrivate_getLanWanConnObj(intfName,
                                   &wanIpConnObj, &wanPppConnObj, NULL, NULL);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not find object for %s", intfName);
      return -1;
   }

   if (wanIpConnObj)
   {
      vlanId = wanIpConnObj->X_BROADCOM_COM_VlanMuxID;
      cmsObj_free((void **)&wanIpConnObj);
   }
   else if (wanPppConnObj)
   {
      vlanId = wanPppConnObj->X_BROADCOM_COM_VlanMuxID;
      cmsObj_free((void **)&wanPppConnObj);
   }

   return vlanId;
}
#endif  /* SUPPORT_WANVLANMUX */




#endif /* DMP_BASELINE_1 */


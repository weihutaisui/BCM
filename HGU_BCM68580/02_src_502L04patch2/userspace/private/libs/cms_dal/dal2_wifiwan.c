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


#ifdef DMP_DEVICE2_BASELINE_1

#ifdef DMP_X_BROADCOM_COM_DEV2_WIFIWAN_1

#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"
#include "dal.h"


CmsRet dalWifiWan_getAvailableL2WlIntf_dev2(NameList **ifList)
{
   CmsRet            ret;
   Dev2WifiSsidObject *ssidObj = NULL;
   InstanceIdStack   iidStack = EMPTY_INSTANCE_ID_STACK;
   NameList          head;
   NameList          *tail, *nameListElement;


   *ifList = NULL;
   head.name = NULL;
   head.next = NULL;
   tail = &head;

   cmsLog_debug("Enter:");

   while ((ret = cmsObj_getNext(MDMOID_DEV2_WIFI_SSID, &iidStack, (void **)&ssidObj)) == CMSRET_SUCCESS)
   {
      if (ssidObj->enable != TRUE)
      {
         cmsObj_free((void **)&ssidObj);
         continue;
      }

      /* XXX need to check if this SSID is already WAN/upstream.
       * Upstream param is in the radio object. Can a single radio support
       * both upstream(WAN) and downstream(LAN) SSIDs?
       */
      nameListElement = cmsMem_alloc(sizeof(NameList), ALLOC_ZEROIZE);
      if (nameListElement == NULL)
      {
         cmsLog_error("could not allocate nameListElement");
         cmsObj_free((void **)&ssidObj);
         cmsDal_freeNameList(head.next);
         return CMSRET_RESOURCE_EXCEEDED;
      }
      /* append to name list */
      nameListElement->name = cmsMem_strdup(ssidObj->name);
      nameListElement->next = NULL;
      tail->next = nameListElement;
      tail = nameListElement;

      cmsObj_free((void **)&ssidObj);
   }  /* while */

   if (ret != CMSRET_NO_MORE_INSTANCES)
   {
      cmsLog_error("cmsObj_getNextInSubTree returns error. ret=%d", ret);
      cmsDal_freeNameList(head.next);
      return ret;
   }

   /* return the name list */
   *ifList = head.next;

   return CMSRET_SUCCESS;
}


CmsRet dalWifiWan_addWlInterface_dev2(const WEB_NTWK_VAR *webVar)
{
   cmsLog_error("XXX not implemented yet");

   cmsLog_debug("Entering....wanL2IfName=%s", webVar->wanL2IfName);
   
   return CMSRET_SUCCESS;
}


CmsRet dalWifiWan_deleteWlInterface_dev2(const WEB_NTWK_VAR *webVar)
{

   cmsLog_error("XXX not implemented yet");
   
   cmsLog_debug("Deleting %s", webVar->wanL2IfName);

   
   return CMSRET_SUCCESS;
}
#endif /*DMP_X_BROADCOM_COM_DEV2_WIFIWAN_1*/

#endif  /*  DMP_DEVICE2_BASELINE_1 */




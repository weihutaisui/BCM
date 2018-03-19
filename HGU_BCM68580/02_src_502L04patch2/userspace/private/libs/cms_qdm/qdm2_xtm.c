/***********************************************************************
 *
 *  Copyright (c) 2012  Broadcom Corporation
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
 
#ifdef DMP_DEVICE2_BASELINE_1 /* this file touches TR181 objects */

#include "cms.h"
#include "cms_mdm.h"
#include "cms_obj.h"
#include "cms_phl.h"
#include "cms_qdm.h"
#include "cms_util.h"
#include "cms_core.h"


#ifdef DMP_DEVICE2_ATMLINK_1
CmsRet qdmXtm_getAtmIntfInfoByNameLocked_dev2(char *ifName,
                                              UINT16 *vpi,
                                              UINT16 *vci,
                                              UINT32 *interfaceId)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2AtmLinkObject *atmLink=NULL;
   SINT32 vpidx=0, vcidx=0;
   UBOOL8 found=FALSE;
   CmsRet ret=CMSRET_INTERNAL_ERROR;
   
   if (cmsUtl_strlen(ifName) == 0)
   {
      cmsLog_error("no ATM ifName, just return");
      return CMSRET_INVALID_ARGUMENTS;
   }
   
   cmsLog_debug("Looking for ATM interface info for %s", ifName);

   while (!found &&
          (ret = cmsObj_getNext(MDMOID_DEV2_ATM_LINK, 
                                &iidStack,
                                (void **) &atmLink)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(atmLink->name, ifName))
      {
         found = TRUE;
   
         cmsUtl_atmVpiVciStrToNum_dev2(atmLink->destinationAddress, &vpidx, &vcidx);
         *vpi = (UINT16)vpidx;
         *vci = (UINT16)vcidx;
         *interfaceId = atmLink->X_BROADCOM_COM_ATMInterfaceId;
      }
      cmsObj_free((void **) &atmLink);
   }

   if (!found)
   {
      cmsLog_error("Cannot find ATM interface %s", ifName);
      return CMSRET_OBJECT_NOT_FOUND;
   }
   
   cmsLog_debug("Exit. ret %d", ret);

   return ret;
   
}
#endif

#ifdef DMP_DEVICE2_PTMLINK_1
CmsRet qdmXtm_getPtmIntfInfoByNameLocked_dev2(char *ifName, 
                                              UINT32 *ptmPortId,
                                              UBOOL8 *ptmPriorityLow,
                                              UBOOL8 *ptmPriorityHigh)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2PtmLinkObject *ptmLink=NULL;
   UBOOL8 found=FALSE;
   CmsRet ret=CMSRET_INTERNAL_ERROR;
   
   if (cmsUtl_strlen(ifName) == 0)
   {
      cmsLog_error("no PTM ifName, just return");
      return CMSRET_INVALID_ARGUMENTS;
   }
   
   cmsLog_debug("Looking for PTM interface info for %s", ifName);

   while (!found &&
          (ret = cmsObj_getNext(MDMOID_DEV2_PTM_LINK, 
                                &iidStack,
                                (void **) &ptmLink)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(ptmLink->name, ifName))
      {
         found = TRUE;
   
         *ptmPortId       = ptmLink->X_BROADCOM_COM_PTMPortId;
         *ptmPriorityLow  = ptmLink->X_BROADCOM_COM_PTMPriorityLow;
         *ptmPriorityHigh = ptmLink->X_BROADCOM_COM_PTMPriorityHigh;
      }
      cmsObj_free((void **) &ptmLink);
   }

   if (!found)
   {
      cmsLog_error("Cannot find PTM interface %s", ifName);
      return CMSRET_OBJECT_NOT_FOUND;
   }
   
   cmsLog_debug("Exit. ret %d", ret);

   return ret;
   
}
#endif

#endif /* DMP_DEVICE2_BASELINE_1 */


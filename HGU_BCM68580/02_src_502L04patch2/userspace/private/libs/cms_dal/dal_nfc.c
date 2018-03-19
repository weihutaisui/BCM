#ifdef DMP_X_BROADCOM_COM_NFC_1
/***********************************************************************
 *
 *  Copyright (c) 2008-2010  Broadcom Corporation
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

#include "cms.h"
#include "cms_msg.h"
#include "mdm_validstrings.h"

#include <fcntl.h>

#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"

#include "cms_msg.h"
#include "cms_util.h"
#include "cms_boardioctl.h"


CmsRet dalNfcCfg(const char *enable)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   _NfcObject *nfcObj = NULL;

   cmsLog_debug("configure NFC to %s", enable);

   if ((ret = cmsObj_get(MDMOID_NFC, &iidStack, 0, (void **) &nfcObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get NfcObject, ret=%d", ret);
      return ret;
   }

   if (strcmp(enable, "1") == 0)
   {
      nfcObj->enable = TRUE;
   }
   else
   {
      nfcObj->enable = FALSE;
   }

   ret = cmsObj_set(nfcObj, &iidStack);
   cmsObj_free((void **) &nfcObj);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set NfcObject, ret = %d", ret);
   }

   return ret;

}


CmsRet dalGetNfc(char *info)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   _NfcObject *nfcObj = NULL;

   if(info == NULL)
   {
      return CMSRET_INVALID_PARAM_TYPE;
   }

   if ((ret = cmsObj_get(MDMOID_NFC, &iidStack, 0, (void **) &nfcObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get NfcObject, ret=%d", ret);
      return ret;
   }

   sprintf(info, "%d", nfcObj->enable);

   cmsObj_free((void **) &nfcObj);

   return ret;

}
#endif

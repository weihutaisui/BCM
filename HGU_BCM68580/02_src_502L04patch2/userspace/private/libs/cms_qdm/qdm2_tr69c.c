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
#ifdef SUPPORT_TR69C

#include "cms.h"
#include "cms_mdm.h"
#include "cms_obj.h"
#include "cms_phl.h"
#include "cms_qdm.h"
#include "cms_util.h"
#include "cms_core.h"

CmsRet qdmTr69c_getManagementServerCfgLocked_dev2(char *url, char *connReqUser, char *connReqPwd, char *user, char *pwd)
{
   Dev2ManagementServerObject *obj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   ret = cmsObj_get(MDMOID_DEV2_MANAGEMENT_SERVER, &iidStack, 0, (void **) &obj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get management server object!");
   }
   else 
   {
      strcpy(url, obj->URL);
      strcpy(connReqUser,obj->connectionRequestUsername);
      strcpy(connReqPwd,obj->connectionRequestPassword);
      strcpy(user,obj->username);
      strcpy(pwd,obj->password);
      cmsObj_free((void **) &obj);
   }
   return ret;
}

CmsRet qdmTr69c_getBoundIfNameLocked_dev2(char *ifName)
{
   Dev2ManagementServerObject *mgmtObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret=CMSRET_SUCCESS;

   ret = cmsObj_get(MDMOID_DEV2_MANAGEMENT_SERVER, &iidStack, 0, (void **) &mgmtObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get management server object!, ret=%d", ret);
      return ret;
   }

   strcpy(ifName, mgmtObj->X_BROADCOM_COM_BoundIfName);

   cmsObj_free((void **) &mgmtObj);

   return ret;
}

#ifdef DMP_DEVICE2_AUTONXFERCOMPLPOLICY_1
CmsRet qdmTr69c_getAutonXferCompletePolicyLocked_dev2(UBOOL8 *enable, char *fileTypeFilter, char *resultTypeFilter, 
                                                      char *transferTypeFilter)
{
   Dev2AutonXferCompletePolicyObject *policyObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret=CMSRET_SUCCESS;

   ret = cmsObj_get(MDMOID_DEV2_AUTON_XFER_COMPLETE_POLICY, &iidStack, 0, (void **) &policyObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get transfer complete policy object!, ret=%d", ret);
      return ret;
   }

   *enable = policyObj->enable;
   strcpy(fileTypeFilter, policyObj->fileTypeFilter);
   strcpy(resultTypeFilter, policyObj->resultTypeFilter);
   strcpy(transferTypeFilter, policyObj->transferTypeFilter);

   cmsObj_free((void **) &policyObj);

   return ret;
}
#endif /* DMP_DEVICE2_AUTONXFERCOMPLPOLICY_1 */

#ifdef DMP_DEVICE2_DUSTATECHNGCOMPLPOLICY_1
CmsRet qdmTr69c_getDuStateChangePolicyLocked_dev2(UBOOL8 *enable, char *opTypeFilter, char *resultTypeFilter, 
                                                  char *faultCodeFilter)
{
   Dev2DuStateChangeComplPolicyObject *policyObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret=CMSRET_SUCCESS;

   ret = cmsObj_get(MDMOID_DEV2_DU_STATE_CHANGE_COMPL_POLICY, &iidStack, 0, (void **) &policyObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get transfer complete policy object!, ret=%d", ret);
      return ret;
   }

   *enable = policyObj->enable;
   strcpy(opTypeFilter, policyObj->operationTypeFilter);
   strcpy(resultTypeFilter, policyObj->resultTypeFilter);
   strcpy(faultCodeFilter, policyObj->faultCodeFilter);

   cmsObj_free((void **) &policyObj);

   return ret;
}
#endif /* DMP_DEVICE2_DUSTATECHNGCOMPLPOLICY_1 */

#ifdef DMP_DEVICE2_XMPPCONNREQ_1
CmsRet qdmTr69c_getManagementServerXmppCfgLocked_dev2(char *connectionPath, char *allowedJabberIds, char *connReqJabberId)
{
   Dev2ManagementServerObject *obj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   ret = cmsObj_get(MDMOID_DEV2_MANAGEMENT_SERVER, &iidStack, 0, (void **) &obj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get management server object!");
   }
   else 
   {
      strcpy(connectionPath, obj->connReqXMPPConnection);
      strcpy(allowedJabberIds,obj->connReqAllowedJabberIDs);
      strcpy(connReqJabberId,obj->connReqJabberID);
      cmsObj_free((void **) &obj);
   }
   return ret;   
}
#endif /* DMP_DEVICE2_XMPPCONNREQ_1 */

#endif /* SUPPORT_TR69C */
#endif /* DMP_DEVICE2_BASELINE_1 */


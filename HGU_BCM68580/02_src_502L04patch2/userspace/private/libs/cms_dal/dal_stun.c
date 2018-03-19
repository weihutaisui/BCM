/***********************************************************************
 *
 *  Copyright (c) 2009  Broadcom Corporation
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

#ifdef SUPPORT_STUN

#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"
#include "dal.h"

CmsRet dalStun_StunConfig(const PWEB_NTWK_VAR pWebVar)
{
  InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
  CmsRet ret=CMSRET_SUCCESS;

  cmsLog_debug("set STUN configuration");

#if defined(SUPPORT_DM_LEGACY98) || defined(SUPPORT_DM_HYBRID)
  ManagementServerObject* mgmtObj = NULL;         
  ret = cmsObj_get(MDMOID_MANAGEMENT_SERVER, &iidStack, 0, (void **) &mgmtObj);
#else
  Dev2ManagementServerObject* mgmtObj = NULL;
  ret = cmsObj_get(MDMOID_DEV2_MANAGEMENT_SERVER, &iidStack, 0, (void **) &mgmtObj);   
#endif 

  if (ret != CMSRET_SUCCESS)
  {
    cmsLog_error("could not get management server object!, ret=%d", ret);
    return ret;
  }

  mgmtObj->STUNEnable = pWebVar->stunEnable;

  REPLACE_STRING_IF_NOT_EQUAL(mgmtObj->STUNServerAddress, pWebVar->stunServerAddress);
  mgmtObj->STUNServerPort = pWebVar->stunServerPort;
  REPLACE_STRING_IF_NOT_EQUAL(mgmtObj->STUNUsername, pWebVar->stunUser);
  REPLACE_STRING_IF_NOT_EQUAL(mgmtObj->STUNPassword, pWebVar->stunPwd);
  mgmtObj->STUNMaximumKeepAlivePeriod = pWebVar->stunMaxKeepAlivePeriod;
  mgmtObj->STUNMinimumKeepAlivePeriod = pWebVar->stunMinKeepAlivePeriod;

  if ((ret = cmsObj_set(mgmtObj, &iidStack)) != CMSRET_SUCCESS)
  {
    cmsLog_error("failed to set STUN obj, ret=%d", ret);
  }

  cmsObj_free((void **) &mgmtObj);

  return ret;
}

CmsRet dalStun_getStunCfg(WEB_NTWK_VAR* webVar)
{
  CmsRet ret=CMSRET_SUCCESS;
  InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

  cmsLog_debug("get STUN configuration");

#if defined(SUPPORT_DM_LEGACY98) || defined(SUPPORT_DM_HYBRID)
  ManagementServerObject* mgmtObj = NULL;         
  ret = cmsObj_get(MDMOID_MANAGEMENT_SERVER, &iidStack, 0, (void**) &mgmtObj);
#else
  Dev2ManagementServerObject* mgmtObj = NULL;
  ret = cmsObj_get(MDMOID_DEV2_MANAGEMENT_SERVER, &iidStack, 0, (void**) &mgmtObj); 
#endif

  if (ret != CMSRET_SUCCESS)
  {
    cmsLog_error("could not get management server object!, ret=%d", ret);
    return ret;
  }

  /* clean STUN webVar strings */
  memset(webVar->stunServerAddress, 0, sizeof(webVar->stunServerAddress));
  memset(webVar->stunUser, 0, sizeof(webVar->stunUser));
  memset(webVar->stunPwd, 0, sizeof(webVar->stunPwd));

  /* set STUN webVar parameters */
  webVar->stunEnable = mgmtObj->STUNEnable;

  if (mgmtObj->STUNServerAddress != NULL)
  {
    strncpy(webVar->stunServerAddress, mgmtObj->STUNServerAddress, sizeof(webVar->stunServerAddress) - 1);
  }

  webVar->stunServerPort = mgmtObj->STUNServerPort;

  if (mgmtObj->STUNUsername != NULL)
  {
    strncpy(webVar->stunUser, mgmtObj->STUNUsername, sizeof(webVar->stunUser) - 1);
  }

  if (mgmtObj->STUNPassword != NULL)
  {
    strncpy(webVar->stunPwd, mgmtObj->STUNPassword, sizeof(webVar->stunPwd) - 1);
  }

  webVar->stunMaxKeepAlivePeriod = mgmtObj->STUNMaximumKeepAlivePeriod;
  webVar->stunMinKeepAlivePeriod = mgmtObj->STUNMinimumKeepAlivePeriod;

  cmsObj_free((void **) &mgmtObj);

  return ret;
}

#endif

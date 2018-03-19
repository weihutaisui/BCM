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
#if defined(DMP_X_BROADCOM_COM_IGMP_1) || defined(DMP_X_BROADCOM_COM_MLD_1) || defined(DMP_X_BROADCOM_COM_MCAST_1)
#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"

#if defined(DMP_X_BROADCOM_COM_MCAST_1)
CmsRet dalMulticast_AddException(char* ip, char* mask)
{
  CmsRet ret = CMSRET_SUCCESS;
  McastCfgObject *mcastObj = NULL;
  InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
  char *newExceptionList = NULL;
  int currentLength = 0;

  if ((ret = cmsObj_get(MDMOID_MCAST_CFG, &iidStack, 0, (void **) &mcastObj)) != CMSRET_SUCCESS)
  {
    cmsLog_error("Failed to get McastCfgObject, ret=%d", ret);
    return ret;
  }

  if (mcastObj->mcastIgmpSnoopExceptions) {
    currentLength = cmsMem_strnlen(mcastObj->mcastIgmpSnoopExceptions, BUFLEN_1024, NULL);
  }

  newExceptionList = (char*) cmsMem_alloc(currentLength + 33, 0);
  
  if (!newExceptionList) {
    cmsObj_free((void **) &mcastObj);
    return CMSRET_INVALID_ARGUMENTS;
  }    
  
  cmsUtl_strcpy(newExceptionList, mcastObj->mcastIgmpSnoopExceptions);

  /* We just need to check that the length of the list is non-zero */
  if (cmsMem_strnlen(newExceptionList, 1, NULL)) {
    cmsUtl_strcat (newExceptionList, " ");
  }
  cmsUtl_strcat (newExceptionList, ip);
  cmsUtl_strcat (newExceptionList, "/");
  cmsUtl_strcat (newExceptionList, mask);

  CMSMEM_REPLACE_STRING (mcastObj->mcastIgmpSnoopExceptions, newExceptionList);

  ret = cmsObj_set(mcastObj, &iidStack);
  cmsObj_free((void **) &mcastObj);
  cmsMem_free(newExceptionList);

  if (ret != CMSRET_SUCCESS)
  {
    cmsLog_error("Failed to set igmpObj , ret = %d", ret);
  }
  return ret;
}

CmsRet dalMulticast_RemoveException(char* rmList)
{
  CmsRet ret = CMSRET_SUCCESS;
  McastCfgObject *mcastObj = NULL;
  InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
  char* copiedExceptionList = NULL;
  char* newExceptionList = NULL;
  char *ptr1;
  char *ptr2;

  if ((ret = cmsObj_get(MDMOID_MCAST_CFG, &iidStack, 0, (void **) &mcastObj)) != CMSRET_SUCCESS)
  {
    cmsLog_error("Failed to get igmpObj, ret=%d", ret);
    return ret;
  }

  copiedExceptionList = cmsMem_alloc(strlen(mcastObj->mcastIgmpSnoopExceptions)+1, 0);
  if (!copiedExceptionList) {
    cmsLog_error("Not enough memory");
    cmsObj_free((void **) &mcastObj);
    return CMSRET_INVALID_ARGUMENTS;
  }
  newExceptionList = cmsMem_alloc(strlen(mcastObj->mcastIgmpSnoopExceptions)+1, 0);
  if (!newExceptionList) {
    cmsLog_error("Not enough memory");
    cmsMem_free(copiedExceptionList);
    cmsObj_free((void **) &mcastObj);
    return CMSRET_INVALID_ARGUMENTS;
  }
  newExceptionList[0] = '\0';

  cmsUtl_strcpy(copiedExceptionList, mcastObj->mcastIgmpSnoopExceptions);

  // Go through the original list and skip the ones in the remove list while copying?
  ptr1 = strtok_r(copiedExceptionList, " ", &ptr2);
  while (ptr1)
  {
    if (NULL == strstr(rmList, ptr1) ) 
    {
      cmsLog_debug("Keeping %s", ptr1);
      if (cmsMem_strnlen(newExceptionList, BUFLEN_1024, NULL) )
      {
        cmsUtl_strcat(newExceptionList, " ");
      }
      cmsUtl_strcat(newExceptionList, ptr1);
    }
    else
    {
      cmsLog_debug("Removing %s", ptr1);
    }
    ptr1 = strtok_r(NULL, " ", &ptr2);
  }

  CMSMEM_REPLACE_STRING (mcastObj->mcastIgmpSnoopExceptions, newExceptionList);

  ret = cmsObj_set(mcastObj, &iidStack);
  cmsObj_free((void **) &mcastObj);

  if (ret != CMSRET_SUCCESS)
  {
    cmsLog_error("Failed to set igmpObj , ret = %d", ret);
  }

  cmsMem_free(copiedExceptionList);
  cmsMem_free(newExceptionList);
  
  return ret;
}

CmsRet dalMulticast_AddExceptionMld(char* ipv6, char* mask)
{
  CmsRet ret = CMSRET_SUCCESS;

  McastCfgObject *mcastObj = NULL;
  InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
  char *newExceptionList = NULL;
  int currentLength = 0;

  if ((ret = cmsObj_get(MDMOID_MCAST_CFG, &iidStack, 0, (void **) &mcastObj)) != CMSRET_SUCCESS)
  {
    cmsLog_error("Failed to get McastCfgObject, ret=%d", ret);
    return ret;
  }

  if (mcastObj->mcastMldSnoopExceptions) {
    currentLength = cmsMem_strnlen(mcastObj->mcastMldSnoopExceptions, BUFLEN_1024, NULL);
  }

  newExceptionList = (char*) cmsMem_alloc(currentLength + 82, 0);
  
  if (!newExceptionList) {
    cmsObj_free((void **) &mcastObj);
    return CMSRET_INVALID_ARGUMENTS;
  }    
  
  cmsUtl_strcpy(newExceptionList, mcastObj->mcastMldSnoopExceptions);

  /* We just need to check that the length of the list is non-zero */
  if (cmsMem_strnlen(newExceptionList, 1, NULL)) {
    cmsUtl_strcat (newExceptionList, " ");
  }
  cmsUtl_strcat (newExceptionList, ipv6);
  cmsUtl_strcat (newExceptionList, "/");
  cmsUtl_strcat (newExceptionList, mask);

  CMSMEM_REPLACE_STRING (mcastObj->mcastMldSnoopExceptions, newExceptionList);

  ret = cmsObj_set(mcastObj, &iidStack);
  cmsObj_free((void **) &mcastObj);
  cmsMem_free(newExceptionList);

  if (ret != CMSRET_SUCCESS)
  {
    cmsLog_error("Failed to set igmpObj , ret = %d", ret);
  }
  return ret;
}

CmsRet dalMulticast_RemoveExceptionMld(char* rmList)
{
  CmsRet ret = CMSRET_SUCCESS;
  McastCfgObject *mcastObj = NULL;
  InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
  char* copiedExceptionList = NULL;
  char* newExceptionList = NULL;
  char *ptr1;
  char *ptr2;

  if ((ret = cmsObj_get(MDMOID_MCAST_CFG, &iidStack, 0, (void **) &mcastObj)) != CMSRET_SUCCESS)
  {
    cmsLog_error("Failed to get igmpObj, ret=%d", ret);
    return ret;
  }

  copiedExceptionList = cmsMem_alloc(strlen(mcastObj->mcastMldSnoopExceptions)+1, 0);
  if (!copiedExceptionList) {
    cmsLog_error("Not enough memory");
    cmsObj_free((void **) &mcastObj);
    return CMSRET_INVALID_ARGUMENTS;
  }
  newExceptionList = cmsMem_alloc(strlen(mcastObj->mcastMldSnoopExceptions)+1, 0);
  if (!newExceptionList) {
    cmsLog_error("Not enough memory");
    cmsMem_free(copiedExceptionList);
    cmsObj_free((void **) &mcastObj);
    return CMSRET_INVALID_ARGUMENTS;
  }
  newExceptionList[0] = '\0';

  cmsUtl_strcpy(copiedExceptionList, mcastObj->mcastMldSnoopExceptions);

  // Go through the original list and skip the ones in the remove list while copying?
  ptr1 = strtok_r(copiedExceptionList, " ", &ptr2);
  while (ptr1)
  {
    if (NULL == strstr(rmList, ptr1) ) 
    {
      cmsLog_debug("Keeping %s", ptr1);
      if (cmsMem_strnlen(newExceptionList, BUFLEN_1024, NULL) )
      {
        cmsUtl_strcat(newExceptionList, " ");
      }
      cmsUtl_strcat(newExceptionList, ptr1);
    }
    else
    {
      cmsLog_debug("Removing %s", ptr1);
    }
    ptr1 = strtok_r(NULL, " ", &ptr2);
  }

  CMSMEM_REPLACE_STRING (mcastObj->mcastMldSnoopExceptions, newExceptionList);

  ret = cmsObj_set(mcastObj, &iidStack);
  cmsObj_free((void **) &mcastObj);

  if (ret != CMSRET_SUCCESS)
  {
    cmsLog_error("Failed to set igmpObj , ret = %d", ret);
  }

  cmsMem_free(copiedExceptionList);
  cmsMem_free(newExceptionList);
  
  return ret;
}
#endif

CmsRet dalSetMulticastCfg(const PWEB_NTWK_VAR pWebVar)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
#ifdef DMP_X_BROADCOM_COM_MCAST_1   
   McastCfgObject *mcastObj = NULL;
#endif
#ifdef DMP_X_BROADCOM_COM_IGMP_1
   IgmpCfgObject *igmpObj = NULL;
#endif
#ifdef DMP_X_BROADCOM_COM_MLD_1
   MldCfgObject *mldObj = NULL;
#endif

#ifdef DMP_X_BROADCOM_COM_MCAST_1
   if ((ret = cmsObj_get(MDMOID_MCAST_CFG, &iidStack, 0, (void **) &mcastObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get mcastObj, ret=%d", ret);
      return ret;
   }
   mcastObj->mcastPrecedence = pWebVar->mcastPrecedence;
   mcastObj->mcastStrictWan = pWebVar->mcastStrictWan;
#ifdef DMP_X_BROADCOM_COM_DCSP_MCAST_REMARK_1
   mcastObj->mcastDscpRemarkEnable = pWebVar->mcastDscpRemarkEnable;
   if (mcastObj->mcastDscpRemarkEnable)
       mcastObj->mcastDscpRemarkVal= pWebVar->mcastDscpRemarkVal;
#endif

   ret = cmsObj_set(mcastObj, &iidStack);
   cmsObj_free((void **) &mcastObj);
#endif

#ifdef DMP_X_BROADCOM_COM_IGMP_1
   if ((ret = cmsObj_get(MDMOID_IGMP_CFG, &iidStack, 0, (void **) &igmpObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get igmpObj, ret=%d", ret);
      return ret;
   }
               
   igmpObj->igmpVer = pWebVar->igmpVer;
   igmpObj->igmpQI = pWebVar->igmpQI;
   igmpObj->igmpQRI = pWebVar->igmpQRI;
   igmpObj->igmpLMQI = pWebVar->igmpLMQI;
   igmpObj->igmpRV = pWebVar->igmpRV;
   igmpObj->igmpMaxGroups = pWebVar->igmpMaxGroups;
   igmpObj->igmpMaxSources = pWebVar->igmpMaxSources;
   igmpObj->igmpMaxMembers = pWebVar->igmpMaxMembers;
   igmpObj->igmpFastLeaveEnable = pWebVar->igmpFastLeaveEnable;

   ret = cmsObj_set(igmpObj, &iidStack);
   cmsObj_free((void **) &igmpObj);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set igmpObj , ret = %d", ret);
   }   
#endif

#ifdef DMP_X_BROADCOM_COM_MLD_1
   if ((ret = cmsObj_get(MDMOID_MLD_CFG, &iidStack, 0, (void **) &mldObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get mldObj, ret=%d", ret);
      return ret;
   }
               
   mldObj->mldVer = pWebVar->mldVer;
   mldObj->mldQI = pWebVar->mldQI;
   mldObj->mldQRI = pWebVar->mldQRI;
   mldObj->mldLMQI = pWebVar->mldLMQI;
   mldObj->mldRV = pWebVar->mldRV;
   mldObj->mldMaxGroups = pWebVar->mldMaxGroups;
   mldObj->mldMaxSources = pWebVar->mldMaxSources;
   mldObj->mldMaxMembers = pWebVar->mldMaxMembers;
   mldObj->mldFastLeaveEnable = pWebVar->mldFastLeaveEnable;

   ret = cmsObj_set(mldObj, &iidStack);
   cmsObj_free((void **) &mldObj);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set mldObj , ret = %d", ret);
   }   
#endif


   return ret;

} /* dalSetMulticastCfg */

CmsRet dalGetMulticastCfg(WEB_NTWK_VAR *pWebVar)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
#ifdef DMP_X_BROADCOM_COM_MCAST_1
   McastCfgObject *mcastObj = NULL;
#endif
#ifdef DMP_X_BROADCOM_COM_IGMP_1
   IgmpCfgObject *igmpObj = NULL; 
#endif
#ifdef DMP_X_BROADCOM_COM_MLD_1
   MldCfgObject *mldObj = NULL; 
#endif

#ifdef DMP_X_BROADCOM_COM_MCAST_1
   if ((ret = cmsObj_get(MDMOID_MCAST_CFG, &iidStack, 0, (void **) &mcastObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get mcastObj, ret=%d", ret);
      return ret;
   }  
   pWebVar->mcastPrecedence = mcastObj->mcastPrecedence;
   pWebVar->mcastStrictWan = mcastObj->mcastStrictWan;
#ifdef DMP_X_BROADCOM_COM_DCSP_MCAST_REMARK_1
   pWebVar->mcastDscpRemarkEnable = mcastObj->mcastDscpRemarkEnable;
   pWebVar->mcastDscpRemarkVal= mcastObj->mcastDscpRemarkVal;
#endif
   cmsUtl_strcpy(pWebVar->igmpExceptAddressList, mcastObj->mcastIgmpSnoopExceptions);
   cmsUtl_strcpy(pWebVar->mldExceptAddressList, mcastObj->mcastMldSnoopExceptions);
   cmsObj_free((void **)&mcastObj); 
#endif   

#ifdef DMP_X_BROADCOM_COM_IGMP_1
   if ((ret = cmsObj_get(MDMOID_IGMP_CFG, &iidStack, 0, (void **) &igmpObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get igmpObj, ret=%d", ret);
      return ret;
   }  

   pWebVar->igmpVer = igmpObj->igmpVer;
   pWebVar->igmpQI = igmpObj->igmpQI;
   pWebVar->igmpQRI = igmpObj->igmpQRI;
   pWebVar->igmpLMQI = igmpObj->igmpLMQI;
   pWebVar->igmpRV = igmpObj->igmpRV;
   pWebVar->igmpMaxGroups = igmpObj->igmpMaxGroups;
   pWebVar->igmpMaxSources = igmpObj->igmpMaxSources;
   pWebVar->igmpMaxMembers = igmpObj->igmpMaxMembers;
   pWebVar->igmpFastLeaveEnable = igmpObj->igmpFastLeaveEnable;

   cmsObj_free((void **) &igmpObj);  
#endif

#ifdef DMP_X_BROADCOM_COM_MLD_1
   if ((ret = cmsObj_get(MDMOID_MLD_CFG, &iidStack, 0, (void **) &mldObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get mldObj, ret=%d", ret);
      return ret;
   }  

   pWebVar->mldVer = mldObj->mldVer;
   pWebVar->mldQI = mldObj->mldQI;
   pWebVar->mldQRI = mldObj->mldQRI;
   pWebVar->mldLMQI = mldObj->mldLMQI;
   pWebVar->mldRV = mldObj->mldRV;
   pWebVar->mldMaxGroups = mldObj->mldMaxGroups;
   pWebVar->mldMaxSources = mldObj->mldMaxSources;
   pWebVar->mldMaxMembers = mldObj->mldMaxMembers;
   pWebVar->mldFastLeaveEnable = mldObj->mldFastLeaveEnable;

   cmsObj_free((void **) &mldObj);  
#endif


   return ret;

} /* dalGetMulticastCfg */
#endif

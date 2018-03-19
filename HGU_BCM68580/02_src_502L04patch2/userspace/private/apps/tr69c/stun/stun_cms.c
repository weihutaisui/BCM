/***********************************************************************
 * <:copyright-BRCM:2007-2013:proprietary:standard
 * 
 *    Copyright (c) 2007-2013 Broadcom 
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
 * :> *
 * $Change: 109793 $
 ***********************************************************************/

#ifdef SUPPORT_STUN

#include "stun_cms.h"
#include "cms_log.h"
#include "cms_core.h"
#include "cms_util.h"
#include "../inc/utils.h"
#include "../bcmLibIF/bcmConfig.h"

#define CLIENT_PORT              (50001)
#define CLIENT_TIME_WAIT         (30)
#define CLIENT_SEND_RETRY_TIMES  (3)
#define CLIENT_RECV_TIMEOUT      (2)

/*
 * Get CPE LAN IP address
 */
CmsRet stunCms_getCPELanIPAddr(int* ip)
{
   CmsRet ret=CMSRET_SUCCESS;
   
   ret = cmsLck_acquireLock();  
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      return ret;
   }

  *ip = 0;
  char* ipAddr = NULL;
  ret = getLanIPAddressInfo(&ipAddr, NULL);
  if (ret != CMSRET_SUCCESS)
  {
     cmsLog_error("failed to get LAN ip addr, ret=%d", ret);
     cmsLck_releaseLock();
     return ret;
  }
  else
  {
    *ip = readIp(ipAddr);
    CMSMEM_FREE_BUF_AND_NULL_PTR(ipAddr);
  }

  cmsLck_releaseLock();

  return ret;
}

/*
 * Update "UDPConnectionRequestAddress" and "NATDetected" to MDM
 */
CmsRet stunCms_updateParameters(STUNData* stun_data)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret=CMSRET_SUCCESS;

   ret = cmsLck_acquireLock();  
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      return ret;
   }

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
      cmsLck_releaseLock();
      return ret;
   }

   REPLACE_STRING_IF_NOT_EQUAL(mgmtObj->UDPConnectionRequestAddress, stun_data->UDPConnReqAddress);
   mgmtObj->NATDetected = stun_data->NATDetected;

   ret = cmsObj_setFlags(mgmtObj, &iidStack, OSF_NO_RCL_CALLBACK|OSF_NO_ACCESSPERM_CHECK);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not set management server object!, ret=%d", ret);
      cmsLck_releaseLock();
      return ret;
   }

   cmsObj_free((void **) &mgmtObj);

   cmsLck_releaseLock();
   return ret;
}

/*
 * Read STUN configuration from MDM
 */
CmsRet stunCms_getSTUNConfig(STUNData* stun_data)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret=CMSRET_SUCCESS;

   ret = cmsLck_acquireLock();  
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      return ret;
   }

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
      cmsLck_releaseLock();
      return ret;
   }

   if (mgmtObj->UDPConnectionRequestAddress)
   {
      REPLACE_STRING_IF_NOT_EQUAL(stun_data->UDPConnReqAddress, mgmtObj->UDPConnectionRequestAddress);
   }
   
   #if defined(SUPPORT_DM_LEGACY98) || defined(SUPPORT_DM_HYBRID)
   stun_data->AddressNotificationLimit = mgmtObj->UDPConnectionRequestAddressNotificationLimit;
   #else
   /* "UDPConnectionRequestAddressNotificationLimit" is not defined in tr-181 data-model */
   stun_data->AddressNotificationLimit = 0;
   #endif

   stun_data->Enable = mgmtObj->STUNEnable;

   if (mgmtObj->STUNServerAddress)
   {
      CMSMEM_REPLACE_STRING(stun_data->ServerAddress, mgmtObj->STUNServerAddress);
   }

   stun_data->ServerPort = mgmtObj->STUNServerPort;

   if (mgmtObj->STUNUsername)
   {
      REPLACE_STRING_IF_NOT_EQUAL(stun_data->Username, mgmtObj->STUNUsername);
   }

   if (mgmtObj->STUNPassword)
   {
      REPLACE_STRING_IF_NOT_EQUAL(stun_data->Password, mgmtObj->STUNPassword); 
   }

   stun_data->MaxKeepAlivePeriod = mgmtObj->STUNMaximumKeepAlivePeriod;

   stun_data->MinKeepAlivePeriod = mgmtObj->STUNMinimumKeepAlivePeriod;

   stun_data->NATDetected = mgmtObj->NATDetected;  

   /* Set internal parameters */
   stun_data->ClientRecvTimeOut = CLIENT_RECV_TIMEOUT;
   stun_data->ClientSendRetryTimes  = CLIENT_SEND_RETRY_TIMES;
   stun_data->ClientTimeWait = CLIENT_TIME_WAIT;
   stun_data->ClientPort = CLIENT_PORT;

   cmsObj_free((void **) &mgmtObj);

   cmsLck_releaseLock();
   return ret;
}

/*
 * STUN configuration has been changed, update cached data from MDM
 */
CmsRet stunCms_configChanged(STUNData* stun_data)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret=CMSRET_SUCCESS;

   ret = cmsLck_acquireLock();  
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      return ret;
   }

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
      cmsLck_releaseLock();
      return ret;
   }

   #if defined(SUPPORT_DM_LEGACY98) || defined(SUPPORT_DM_HYBRID)
   stun_data->AddressNotificationLimit = mgmtObj->UDPConnectionRequestAddressNotificationLimit;
   #else
   /* "UDPConnectionRequestAddressNotificationLimit" is not defined in tr-181 data-model */
   stun_data->AddressNotificationLimit = 0;
   #endif

   stun_data->Enable = mgmtObj->STUNEnable;

   if (mgmtObj->STUNServerAddress)
   {
      CMSMEM_REPLACE_STRING(stun_data->ServerAddress, mgmtObj->STUNServerAddress);
   }

   stun_data->ServerPort = mgmtObj->STUNServerPort;

   if (mgmtObj->STUNUsername)
   {
      REPLACE_STRING_IF_NOT_EQUAL(stun_data->Username, mgmtObj->STUNUsername);
   }

   if (mgmtObj->STUNPassword)
   {
      REPLACE_STRING_IF_NOT_EQUAL(stun_data->Password, mgmtObj->STUNPassword); 
   }

   stun_data->MaxKeepAlivePeriod = mgmtObj->STUNMaximumKeepAlivePeriod;

   stun_data->MinKeepAlivePeriod = mgmtObj->STUNMinimumKeepAlivePeriod;

   cmsObj_free((void **) &mgmtObj);

   cmsLck_releaseLock();
   return ret;
}

/*
 * Check if Active Notification is enabled for the given paramName
 */
CmsRet stunCms_isActiveNotificationEnabled(char* paramName, UBOOL8* enabled)
{
   CmsRet ret=CMSRET_SUCCESS;
   *enabled = FALSE;
   MdmPathDescriptor path;

   memset(&path, 0, sizeof(path));
   #if defined(SUPPORT_DM_LEGACY98) || defined(SUPPORT_DM_HYBRID)
   path.oid = MDMOID_MANAGEMENT_SERVER;
   #else
   path.oid = MDMOID_DEV2_MANAGEMENT_SERVER;
   #endif
   strcpy(path.paramName, paramName);

   PhlGetParamAttr_t *pParamAttr = NULL;

   ret = cmsLck_acquireLock();  
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      return ret;
   }

   ret = cmsPhl_getParamAttr(&path, &pParamAttr);
   if (ret == CMSRET_SUCCESS)
   {
      *enabled = (pParamAttr->attributes.notification == NDA_TR69_ACTIVE_NOTIFICATION);
   }
   else
   {
      cmsLog_error("failed to get ParamAttr, ret=%d", ret);
   }
   CMSMEM_FREE_BUF_AND_NULL_PTR(pParamAttr);

   cmsLck_releaseLock();
   return ret;
}

#endif // SUPPORT_STUN         
         




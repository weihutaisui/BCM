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

#include "cms.h"
#include "cms_util.h"
#include "cms_core.h"
#include "ssk.h"

#ifdef DMP_DEVICE2_IPPING_1
void processPingStateChanged_dev2(CmsMsgHeader *msg)
{
   PingDataMsgBody *pingInfo = (PingDataMsgBody *) (msg + 1);
   Dev2IpPingDiagObject *ipPingObj= NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   cmsLog_debug("dataLength=%d state=%s",msg->dataLength, pingInfo->diagnosticsState);
   if (pingInfo->interface)
   {
      cmsLog_debug("interface %s",pingInfo->interface);
   }
   if (pingInfo->host)
   {
      cmsLog_debug("host %s",pingInfo->host);
   }
   cmsLog_debug("success %d, fail %d, avg/min/max %d/%d/%d requestId %d",pingInfo->successCount,
                pingInfo->failureCount,pingInfo->averageResponseTime,pingInfo->minimumResponseTime,
                pingInfo->maximumResponseTime,(int)pingInfo->requesterId);
      
      
   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get lock, ret=%d", ret);
      cmsLck_dumpInfo();
      return;
   }

   cmsLog_debug("calling cmsObjGet");

   if ((ret = cmsObj_get(MDMOID_DEV2_IP_PING_DIAG, &iidStack, 0, (void **) &ipPingObj)) == CMSRET_SUCCESS)
   {
      cmsLog_debug("cmsObj_get, MDMOID_DEV2_IP_PING_DIAG success");

      CMSMEM_FREE_BUF_AND_NULL_PTR(ipPingObj->diagnosticsState);
      CMSMEM_FREE_BUF_AND_NULL_PTR(ipPingObj->host);
      ipPingObj->diagnosticsState = cmsMem_strdup(pingInfo->diagnosticsState);
      ipPingObj->host = cmsMem_strdup(pingInfo->host);
      ipPingObj->successCount = pingInfo->successCount;
      ipPingObj->failureCount = pingInfo->failureCount;
      ipPingObj->averageResponseTime = pingInfo->averageResponseTime;
      ipPingObj->maximumResponseTime = pingInfo->maximumResponseTime;
      ipPingObj->minimumResponseTime = pingInfo->minimumResponseTime;

      if ((ret = cmsObj_set(ipPingObj, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("set of ipPingObj failed, ret=%d", ret);
      }
      else
      {
         cmsLog_debug("set ipPingObj OK, successCount %d, failCount %d",
                      ipPingObj->successCount,ipPingObj->failureCount);
      }     
      
      cmsObj_free((void **) &ipPingObj);
   } 
   else
   {
      cmsLog_debug("cmsObj_get, MDMOID_DEV2_IP_PING_DIAG ret %d",ret);
   }

   cmsLck_releaseLock();
   return;
}
#endif /* #ifdef DMP_DEVICE2_IPPING_1 */


#ifdef DMP_DEVICE2_TRACEROUTE_1
void processTracertStateChanged_dev2(CmsMsgHeader *msg)
{
   TracertDataMsgBody *tracertInfo = (TracertDataMsgBody *)(msg + 1);
   Dev2IpDiagTraceRouteObject *ipTracertObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   cmsLog_debug("dataLength=%d state=%s", msg->dataLength, tracertInfo->diagnosticsState);
   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get lock, ret=%d", ret);
      cmsLck_dumpInfo();
      return;
   }

   cmsLog_debug("calling cmsObjGet");

   if (cmsUtl_strcmp(tracertInfo->diagnosticsState, MDMVS_REQUESTED) != 0)
   {
      if ((ret = cmsObj_get(MDMOID_DEV2_IP_DIAG_TRACE_ROUTE, &iidStack, 0, (void **) &ipTracertObj)) == CMSRET_SUCCESS)
      {
         cmsLog_debug("cmsObj_get, MDMOID_DEV2_IP_DIAG_TRACE_ROUTE success");

         ipTracertObj->diagnosticsState = cmsMem_strdup(tracertInfo->diagnosticsState);
         ipTracertObj->responseTime = tracertInfo->averageResponseTime;

         if ((ret = cmsObj_set(ipTracertObj, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("set of ipTracertObj failed, ret=%d", ret);
         }
         else
         {
            cmsLog_debug("set ipTracertObj OK");
         }
         cmsObj_free((void **) &ipTracertObj);
      } 
      else
      {
         cmsLog_debug("cmsObj_get, MDMOID_DEV2_IP_DIAG_TRACE_ROUTE failed, ret=%d",ret);
      }
   }
   else
   {
      if ((ret = cmsObj_addInstance(MDMOID_DEV2_IP_DIAG_TRACE_ROUTE_ROUTE_HOPS, &iidStack)) == CMSRET_SUCCESS)
      {
         Dev2IpDiagTraceRouteRouteHopsObject *routeHopsObj = NULL;
         if ((ret = cmsObj_get(MDMOID_DEV2_IP_DIAG_TRACE_ROUTE_ROUTE_HOPS, &iidStack, 0, (void **) &routeHopsObj)) == CMSRET_SUCCESS)
         {
            cmsLog_debug("cmsObj_get, MDMOID_DEV2_IP_DIAG_TRACE_ROUTE_ROUTE_HOPS success");
            routeHopsObj->hostAddress = cmsMem_strdup(tracertInfo->hostAddrOfRouteHop);
            routeHopsObj->host = cmsMem_strdup(tracertInfo->hostOfRouteHop);
            routeHopsObj->RTTimes = cmsMem_strdup(tracertInfo->rtTimes);
            routeHopsObj->errorCode = tracertInfo->errorCode;

            if ((ret = cmsObj_set(routeHopsObj, &iidStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("set of routeHopsObj failed, ret=%d", ret);
            }
            else
            {
               cmsLog_debug("set routeHopsObj OK");
            }
            cmsObj_free((void **) &routeHopsObj);
         }
         else
         {
            cmsLog_debug("cmsObj_get, MDMOID_DEV2_IP_DIAG_TRACE_ROUTE_ROUTE_HOPS failed, ret=%d",ret);
         }
      }
   }
   cmsLck_releaseLock();

   return;
}
#endif /* DMP_DEVICE2_TRACEROUTE_1 */

#endif   /* DMP_DEVICE2_BASELINE_1 */


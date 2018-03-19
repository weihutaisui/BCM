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


#include "cms.h"
#include "cms_util.h"
#include "cms_core.h"
#include "ssk.h"
#include "ssk_time.h"

#ifdef DMP_TIME_1

void setNtpServers_igd(const char *ntpServerList)
{
    char server[BUFLEN_128];
    int start = 0, end = 0;
    int i = 0, serverNum = 1;
    UINT32 len = 0;
    TimeServerCfgObject *timeObj = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_SUCCESS;

    if (CMSRET_SUCCESS != (ret = cmsObj_get(MDMOID_TIME_SERVER_CFG, &iidStack,
                                            0, (void **) &timeObj)))
    {
       cmsLog_error("Could not get obj, ret=%d", ret);
       return;
    }

    // The InternetGatewayDevice.Time object allows up to 5 NTP servers
    while (serverNum <= 5)
    {
        start = i;
        len = 0;
        memset(server, 0, sizeof(server));
        while (i < end && ntpServerList[i] != ',')
        {
            i++;
            len++;
        }
        if (len == 0)
            break;
        if (len > sizeof(server)-1)
        {
            cmsLog_error("ntpserver length too long, %d", len);
            break;
        }

        memcpy(server, &(ntpServerList[start]), len);
        cmsLog_debug("Got ntpserver %d = %s", serverNum, server);
        if (serverNum == 1)
           CMSMEM_REPLACE_STRING(timeObj->NTPServer1, server);
        if (serverNum == 2)
           CMSMEM_REPLACE_STRING(timeObj->NTPServer2, server);
        if (serverNum == 3)
           CMSMEM_REPLACE_STRING(timeObj->NTPServer3, server);
        if (serverNum == 4)
           CMSMEM_REPLACE_STRING(timeObj->NTPServer4, server);
        if (serverNum == 5)
           CMSMEM_REPLACE_STRING(timeObj->NTPServer5, server);

        serverNum++;
        i++;  // go get next server
    }

    if (CMSRET_SUCCESS != (ret = cmsObj_set(timeObj, &iidStack)))
    {
       cmsLog_error("Could not set timeObj, ret=%d", ret);
    }
    else
    {
       cmsLog_debug("set of NTP %d servers success", serverNum-1);
    }

    cmsObj_free((void **) &timeObj);
}

void processTimeStateChanged_igd(UINT32 state)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   TimeServerCfgObject *timeObj = NULL;
   IGDDeviceInfoObject *deviceObj = NULL;
   CmsRet ret = CMSRET_SUCCESS;
   char timeBuf[BUFLEN_64];


   cmsLog_debug("state=%d",state);
      
   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get lock, ret=%d", ret);
      cmsLck_dumpInfo();
      return;
   }

   cmsLog_debug("calling cmsObjGet");
   if (state == TIME_STATE_SYNCHRONIZED)
   {
      if ((ret = cmsObj_get(MDMOID_IGD_DEVICE_INFO, &iidStack, 0, (void **) &deviceObj)) == CMSRET_SUCCESS)
      {
         /* only updated once, the first time NTP time is set */
         if (cmsUtl_strcmp(deviceObj->firstUseDate,UNKNOWN_DATETIME_STRING) == 0)
         {
            cmsTms_getXSIDateTime(0,timeBuf,sizeof(timeBuf));
            CMSMEM_FREE_BUF_AND_NULL_PTR(deviceObj->firstUseDate);
            deviceObj->firstUseDate = cmsMem_strdup(timeBuf);
            if ((ret = cmsObj_set(deviceObj, &iidStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("fail to set deviceObj, ret %d",ret);
            }
            else
            {
               /* need to save to flash */
               cmsMgm_saveConfigToFlash();
            }
         }
         cmsObj_free((void **) &deviceObj);
      }
   } /* synchronized */

   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_get(MDMOID_TIME_SERVER_CFG, &iidStack, 0, (void **) &timeObj)) == CMSRET_SUCCESS)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(timeObj->status);
      switch (state)
      {
      case TIME_STATE_UNSYNCHRONIZED:
         timeObj->status = cmsMem_strdup(MDMVS_UNSYNCHRONIZED);
         break;
      case TIME_STATE_DISABLED:
         timeObj->status = cmsMem_strdup(MDMVS_DISABLED);
         break;
      case TIME_STATE_SYNCHRONIZED:
         timeObj->status = cmsMem_strdup(MDMVS_SYNCHRONIZED);
         break;
      case TIME_STATE_FAIL_TO_SYNCHRONIZE:
         timeObj->status = cmsMem_strdup(MDMVS_ERROR_FAILEDTOSYNCHRONIZED);
         break;
      default:
         timeObj->status = cmsMem_strdup(MDMVS_ERROR);
         break;
      }

      if ((ret = cmsObj_set(timeObj, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("set of timeObj failed, ret=%d", ret);
      }
      cmsObj_free((void **) &timeObj);
   } 
   cmsLck_releaseLock();
}

#endif   /* DMP_TIME_1 */


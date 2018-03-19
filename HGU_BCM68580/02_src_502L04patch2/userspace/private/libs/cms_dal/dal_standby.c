#ifdef DMP_X_BROADCOM_COM_STANDBY_1
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



void processPeriodicStandby(StandbyCfgObject *standbyCfgObj)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   StandbyCfgObject *my_standbyCfgObj = NULL;
   CmsRet ret = CMSRET_SUCCESS;
   char newStatus[BUFLEN_64];
   time_t this_tm;
   struct tm *tm_time;
   unsigned int secondsToday;
   unsigned int secondsStandbyTime;

   if (standbyCfgObj == NULL)
   {
      if (CMSRET_SUCCESS != (ret = cmsObj_get(MDMOID_STANDBY_CFG, &iidStack, 0, (void **) &my_standbyCfgObj)))
      {
         cmsLog_error("Failed to get pwrMngtObj %d\n", ret);
         return;
      }
   }
   else
   {
      my_standbyCfgObj = standbyCfgObj;
   }  

   /* Only process standby feature if enabled */
   if (my_standbyCfgObj->standbyEnable)
   {
      /* Only process standby if one of the time value was set */
      if ((0 != my_standbyCfgObj->standbyHour) ||
          (0 != my_standbyCfgObj->standbyMinutes) ||
          (0 != my_standbyCfgObj->wakeupHour) ||
          (0 != my_standbyCfgObj->wakeupMinutes))
      {
         /* Can't standby for 0 amount of time */
         if ((my_standbyCfgObj->standbyHour != my_standbyCfgObj->wakeupHour) ||
             (my_standbyCfgObj->standbyMinutes != my_standbyCfgObj->wakeupMinutes))
         {      
            time(&this_tm);
            /* Can only go to standby if local time has been acquired */
            if (this_tm > 0x41D593A0) // 35 years or so past jan 1, 1970
            {
               tm_time = localtime(&this_tm);
               secondsToday = tm_time->tm_hour*24*60 + tm_time->tm_min*60; // Don't care about seconds + tm_time->tm_sec;
               secondsStandbyTime = my_standbyCfgObj->standbyHour*24*60 + my_standbyCfgObj->standbyMinutes*60;

               // Go in standby mode and wait for wakeup time only is we are within 2 minutes of standby time
               if (secondsToday == secondsStandbyTime) 
               {
                  strcpy(newStatus, MDMVS_WAITING_FOR_WAKE_UP_TIME);
               }
               else
               {
                  strcpy(newStatus, MDMVS_WAITING_FOR_STANDBY_TIME);
               }
            }
            else
            {
                  strcpy(newStatus, MDMVS_LOCAL_TIME_HAS_NOT_BEEN_ACQUIRED);
            }
         }
         else
         {
            strcpy(newStatus, MDMVS_STANDBY_TIME_MUST_BE_DIFFERENT_FROM_WAKE_UP_TIME);
         }
      }
      else
      {
         strcpy(newStatus, MDMVS_STANDBY_OR_WAKE_UP_TIME_IS_NOT_SET);
      }
   }
   else
   {
      strcpy(newStatus, MDMVS_DISABLED);
   }

   /* See if the status has changed */
   if( 0 != cmsUtl_strcmp(my_standbyCfgObj->standbyStatusString, newStatus) )
   {
      CMSMEM_REPLACE_STRING(my_standbyCfgObj->standbyStatusString, newStatus);
      if (CMSRET_SUCCESS != (ret = cmsObj_set(my_standbyCfgObj, &iidStack)))
      {
         cmsLog_error("Failed to set standbyCfgObj %d\n", ret);
      }
   }

   if (standbyCfgObj == NULL)
   {
      cmsObj_free((void **) &my_standbyCfgObj);
   }

   return;
}

CmsRet dalStandby(const PWEB_NTWK_VAR pWebVar)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   StandbyCfgObject *standbyCfgObj = NULL;

   cmsLog_debug("configure Standby ");

   if ((ret = cmsObj_get(MDMOID_STANDBY_CFG, &iidStack, 0, (void **) &standbyCfgObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get standbyCfgObj, ret=%d", ret);
      return ret;
   }

   standbyCfgObj->standbyEnable    = pWebVar->pmStandbyEnable;
   // RO: standbyCfgObj->standbyStatusString           = pWebVar->pmStandbyStatusString;
   standbyCfgObj->standbyHour      = pWebVar->pmStandbyHour;
   standbyCfgObj->standbyMinutes   = pWebVar->pmStandbyMinutes;
   standbyCfgObj->wakeupHour       = pWebVar->pmWakeupHour;
   standbyCfgObj->wakeupMinutes    = pWebVar->pmWakeupMinutes;

   ret = cmsObj_set(standbyCfgObj, &iidStack);
   processPeriodicStandby(standbyCfgObj);

   cmsObj_free((void **) &standbyCfgObj);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set standbyCfgObj, ret = %d", ret);
   }   

   return ret;
} /* dalStandby */


CmsRet dalStandbyDemo(void *msgHandleArg)
{

   CmsRet ret = CMSRET_SUCCESS;

   // Program standby duration
   if (CMSRET_SUCCESS == (ret = devCtl_boardIoctl(BOARD_IOCTL_SET_STANDBY_TIMER, 0, NULL, 0, 3, NULL))) {

      // Program shutdown to go to standby
      if (CMSRET_SUCCESS == (ret = devCtl_boardIoctl(BOARD_IOCTL_SET_SHUTDOWN_MODE, 0, NULL, 0, 0, NULL))) {
         // Do a clean shutdown
         cmsUtil_sendRequestRebootMsg(msgHandleArg);
      }
   }

   return ret;
} /* dalStandbyDemo */

CmsRet dalGetStandby(WEB_NTWK_VAR *pWebVar)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   StandbyCfgObject *standbyCfgObj = NULL; 

   if ((ret = cmsObj_get(MDMOID_STANDBY_CFG, &iidStack, 0, (void **) &standbyCfgObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get pwrMngtObj, ret=%d", ret);
      return ret;
   }  

   pWebVar->pmStandbyEnable      = standbyCfgObj->standbyEnable;
   strcpy(pWebVar->pmStandbyStatusString, standbyCfgObj->standbyStatusString);
   pWebVar->pmStandbyHour        = standbyCfgObj->standbyHour;
   pWebVar->pmStandbyMinutes     = standbyCfgObj->standbyMinutes;
   pWebVar->pmWakeupHour         = standbyCfgObj->wakeupHour;
   pWebVar->pmWakeupMinutes      = standbyCfgObj->wakeupMinutes;

   processPeriodicStandby(standbyCfgObj);
   cmsObj_free((void **) &standbyCfgObj);  

   return ret;

} /* dalGetPowerManagementCfg */
#endif

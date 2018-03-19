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

#ifdef DMP_BASELINE_1

#include "cms.h"
#include "cms_util.h"
#include "cms_core.h"
#include "ssk.h"


/*!\file ssk_periodic.c
 *
 * This file contains functions which are called from the ssk
 * processPeriodicTask.  The functions in this file assume an IGD based
 * data model (Legacy TR98 or Hybrid).
 */

void sskPeriodic_updateUptime_igd()
{
   IGDDeviceInfoObject *obj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   cmsLog_debug("Get IGD.DeviceInfo");
   if ((ret = cmsObj_get(MDMOID_IGD_DEVICE_INFO, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of DEVICE_INFO object failed, ret=%d", ret);
   }
   else
   {
      cmsObj_free((void **) &obj);
   }
}


void sskPeriodic_expirePortMappings_igd(void)
{
   CmsRet ret;
   InstanceIdStack iidStack;
   InstanceIdStack savedIidStack;
   // We use pppConnPortMapping object even when dealing with IpConnPortmapping.
   // This is OK because these two objects have the exact same layout.  Only
   // the oid is different.
   WanPppConnPortmappingObject *port_mapping = NULL;
   MdmObjectId oid=MDMOID_WAN_PPP_CONN_PORTMAPPING;
   UBOOL8 done=FALSE;
   UINT32 upTime;
   UINT32 deltaSeconds;
   static UINT32 lastSeconds=0;

   /*
    * Calculate the number of seconds elapsed since the last time we ran.
    * The first time we enter this function after boot, lastSeconds=0, so
    * just pretend 1 second has elapsed.
    */
   upTime=cmsTms_getSeconds();
   if (lastSeconds == 0)
   {
      deltaSeconds = 1;
   }
   else
   {
      deltaSeconds = upTime - lastSeconds;
   }
   lastSeconds = upTime;

   cmsLog_debug("Entered: deltaSeconds=%d", deltaSeconds);


   /* Do this loop twice: one for the PPP portmappings and one for IP portmappings */
   while (!done)
   {
      INIT_INSTANCE_ID_STACK(&iidStack);
      INIT_INSTANCE_ID_STACK(&savedIidStack);

      while (cmsObj_getNext(oid, &iidStack, (void **)&port_mapping) == CMSRET_SUCCESS)
      {
         if (port_mapping->portMappingLeaseDuration > 0)
         {
            /* this is a dynamic (non-static) lease.  Age and possibly delete */
            if (port_mapping->portMappingLeaseDuration > deltaSeconds)
            {
               port_mapping->portMappingLeaseDuration -= deltaSeconds;
               cmsLog_debug("age %s %d", port_mapping->portMappingDescription,
                                     port_mapping->portMappingLeaseDuration);
               ret = cmsObj_set(port_mapping, &iidStack);
               if (ret != CMSRET_SUCCESS)
               {
                  cmsLog_error("could not set portmapping, ret=%d", ret);
                  /* complain but keep going */
               }
            }
            else
            {
               ret = cmsObj_deleteInstance(oid, &iidStack);
               if (ret != CMSRET_SUCCESS)
               {
                  cmsLog_error("could not delete portmapping, ret=%d", ret);
                  /* complain but keep going */
               }
               /* restore iidStack to last non-deleted instance */
               iidStack = savedIidStack;
            }
         }

         cmsObj_free((void **) &port_mapping);
         savedIidStack = iidStack;  // in case we delete the next one
      }

      if (oid == MDMOID_WAN_PPP_CONN_PORTMAPPING)
      {
         oid = MDMOID_WAN_IP_CONN_PORTMAPPING;
      }
      else
      {
         done = TRUE;
      }
   }

   cmsLog_debug("Exit:");
}



#endif  /* DMP_BASELINE_1 */

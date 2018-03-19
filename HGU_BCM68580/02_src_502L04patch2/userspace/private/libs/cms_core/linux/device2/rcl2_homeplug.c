/***********************************************************************
 *
 *  Copyright (c) 2006-2009  Broadcom Corporation
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


#ifdef DMP_DEVICE2_BASELINE_1

#ifdef DMP_DEVICE2_HOMEPLUG_1

#include "odl.h"
#include "cms_core.h"
#include "cms_util.h"
#include "rcl.h"
#include "rut_util.h"
#include "rut_wan.h"
#include "rut_lan.h"
#include "rut2_homeplug.h"
#include "rut_ethintf.h"


/*!\file rcl_homeplug.c
 * \brief This file contains homeplug WAN and LAN related functions.
 *
 */

CmsRet rcl_deviceHomePlugObject( _DeviceHomePlugObject *newObj __attribute__((unused)),
                const _DeviceHomePlugObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   return CMSRET_SUCCESS;
}

CmsRet rcl_homePlugInterfaceObject( _HomePlugInterfaceObject *newObj,
                const _HomePlugInterfaceObject *currObj,
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   /* Add and enable HPAV interface, or enable existing HPAV interface.

      If an object is added, but the enable parameter is set to false, this
      first block will not be executed.
      Later, if the enable parameter is set to true, then the
      ENABLE_NEW_OR_ENABLE_EXISTING macro would test true, and this block would
      be executed.
   */
   cmsLog_debug("Enter");
   if (ENABLE_NEW_OR_ENABLE_EXISTING(newObj, currObj))
   {
      cmsLog_debug("ENABLE_NEW_OR_ENABLE_EXISTING");
      rutLan_enableInterface(newObj->name);
      if (!cmsMdm_isDataModelDevice2())
      {
         /*
          * If we are using a Hybrid TR98+TR181 data model, then just
          * add the plc interface to br0.  However, in PURE181 mode, the
          * interface stack will dictate where this interface is added and
          * the adding of this interface is done elsewhere.
          */
         rutLan_addInterfaceToBridge(newObj->name, FALSE, "br0");
      }

      /* During bootup (newObj != NULL && currObj==NULL), homeplug firmware
       * has not been downloaded to hardware yet so do not try to set the
       * InterfaceStatus to HPAV_ENABLED yet.
       * When the homeplug firmware is downloaded, homeplugd will enable the
       * homeplug interface by itself, not through this function because
       * newObj->enable == TRUE and currObj->enable == TRUE, the
       * ENABLE_NEW_OR_ENABLE_EXISTING test above will fail, so we will
       * never come into this block.  But the code
       * below is correctly written even though it won't be executed.
       */
      if (newObj && currObj && newObj->enable)
      {    
        ret = rutHomeplug_setInterfaceStatus(HPAV_ENABLED);
      }
   }

   /* Edit existing HPAV interface.

      POTENTIAL_CHANGE_OF_EXISTING() macro verifies that newObj and currObj are
      both not NULL. More importantly, this macro also verifies that the enable
      parameter in both objects are set to TRUE.
   */
   else if (POTENTIAL_CHANGE_OF_EXISTING(newObj, currObj) &&
            rutHomeplug_isValuesChanged(newObj, currObj))
   {
      cmsLog_debug("POTENTIAL_CHANGE_OF_EXISTING");
      if ((ret = rutHomeplug_setInterfaceAlias(newObj, currObj)) == CMSRET_INTERNAL_ERROR)
      {
        return ret;
      }

      if ((ret = rutHomeplug_setInterfaceLogicalNetwork(newObj, currObj)) == CMSRET_INTERNAL_ERROR)
      {
        return ret;
      }

      if ((ret = rutHomeplug_setInterfaceNetworkPassword(newObj, currObj)) == CMSRET_INTERNAL_ERROR)
      {
        return ret;
      }
#ifdef DMP_DEVICE2_HOMEPLUG_DEBUG_1
      if ((ret = rutHomeplug_setInterfaceDiagPeriodicInterval(newObj, currObj)) == CMSRET_INTERNAL_ERROR)
      {
        return ret;
      }
#endif /*  DMP_DEVICE2_HOMEPLUG_DEBUG_1 */
   }

   /* disable HPAV interface or remove existing HPAV interface */
   else if (DELETE_OR_DISABLE_EXISTING(newObj, currObj))
   {
      ret = rutHomeplug_setInterfaceStatus(HPAV_DISABLED);
   }

   return ret;
}

CmsRet rcl_homePlugInterfaceStatsObject( _HomePlugInterfaceStatsObject *newObj __attribute__((unused)),
                const _HomePlugInterfaceStatsObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   return CMSRET_SUCCESS;
}

CmsRet rcl_homePlugInterfaceAssociatedDeviceObject( _HomePlugInterfaceAssociatedDeviceObject *newObj __attribute__((unused)),
                const _HomePlugInterfaceAssociatedDeviceObject *currObj __attribute__((unused)),
                const InstanceIdStack *iidStack __attribute__((unused)),
                char **errorParam __attribute__((unused)),
                CmsRet *errorCode __attribute__((unused)))
{
   return CMSRET_SUCCESS;
}

#endif /* DMP_DEVICE2_HOMEPLUG_1 */

#else
/* DMP_DEVICE2_BASELINE_1 is not defined */

#ifdef DMP_DEVICE2_HOMEPLUG_1
#error "Homeplug objects incompatible with current Data Model mode, go to make menuconfig to fix"
#endif

#endif  /* DMP_DEVICE2_BASELINE_1 */

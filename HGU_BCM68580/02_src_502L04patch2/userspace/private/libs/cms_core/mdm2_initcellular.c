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


#include "cms.h"
#include "cms_util.h"
#include "cms_core.h"
#include "mdm.h"
#include "mdm_private.h"
#include "odl.h"
#include "oal.h"


/*!\file mdm2_initcellular.c
 * \brief This file contains Cellular mdm init related functions.
 *
 */


#ifdef SUPPORT_CELLULAR
#ifdef DMP_CELLULARINTERFACEBASE_1

CmsRet mdm_addDefaultCellularIntfObjects(void)
{
   Dev2CellularObject *cellularObject = NULL;
   Dev2CellularInterfaceObject *cellularIntfObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   MdmPathDescriptor pathDesc;
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("enter");

   /* first check if there is a Cellular Interface instance  first */
   ret = mdm_getNextObject(MDMOID_DEV2_CELLULAR_INTERFACE, &iidStack, (void **)&cellularIntfObj);
   if (ret == CMSRET_SUCCESS)
   {
      /* Cellular Interface is already present, no action needed */
      mdm_freeObject((void **)&cellularIntfObj);
      return ret;
   }

   INIT_PATH_DESCRIPTOR(&pathDesc);
   pathDesc.oid = MDMOID_DEV2_CELLULAR_INTERFACE;
   if ((ret = mdm_addObjectInstance(&pathDesc, NULL, NULL)) != CMSRET_SUCCESS)
   {
      cmsLog_error("mdm_addObjectInstance for MDMOID_DEV2_CELLULAR_INTERFACE failed, ret=%d", ret);
      return ret;
   }

   if ((ret = mdm_getObject(pathDesc.oid, &pathDesc.iidStack, (void **) &cellularIntfObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get cellularIntfObj object, ret=%d", ret);
      return ret;
   }

   CMSMEM_REPLACE_STRING_FLAGS(cellularIntfObj->name, "lte0", mdmLibCtx.allocFlags);

   ret = mdm_setObject((void **) &cellularIntfObj, &pathDesc.iidStack, FALSE);
   mdm_freeObject((void **)&cellularIntfObj);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set cellularIntfObj. ret=%d", ret);
   }

   /* need to manually update the count when adding objects during mdm_init */
   INIT_INSTANCE_ID_STACK(&iidStack);
 
   if ((ret = mdm_getObject(MDMOID_DEV2_CELLULAR, &iidStack, (void **) &cellularObject)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get cellularObject. ret=%d", ret);
      return ret;
   }
 
   cellularObject->interfaceNumberOfEntries++;
   ret = mdm_setObject((void **) &cellularObject, &iidStack,  FALSE);
   mdm_freeObject((void **)&cellularObject);
 
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to set cellularObject. ret=%d", ret);
   }

   return ret;
}


#else /* DMP_CELLULARINTERFACEBASE_1 is not defined */
#error "Cellular objects incompatible with current Data Model mode, go to make menuconfig to fix"
#endif
#endif  /* SUPPORT_CELLULAR */


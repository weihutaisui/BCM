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
#include "cms_params_modsw.h"
#include "cms_util.h"
#include "cms_core.h"
#include "mdm.h"
#include "mdm_private.h"
#include "odl.h"
#include "oal.h"
#include "osgid.h"


/*!\file mdm_initmodsw.c
 * \brief This file calls other Exe Env functions (osgi, linux) to initialize
 *        the MDM.
 *
 */


#ifdef DMP_DEVICE2_SM_BASELINE_1

#ifdef DMP_DEVICE2_X_BROADCOM_COM_MODSW_OSGIEE_1
CmsRet mdm_addDefaultModSwOsgiEeObjects(UINT32 *eeAddedCount);
#endif

#ifdef DMP_DEVICE2_X_BROADCOM_COM_MODSW_LINUXEE_1
CmsRet mdm_addDefaultModSwLinuxEeObjects(UINT32 *eeAddedCount);
#endif

#ifdef DMP_DEVICE2_X_BROADCOM_COM_MODSW_OPENWRTEE_1
CmsRet mdm_addDefaultModSwOpenwrtEeObjects(UINT32 *eeAddedCount);
#endif

#ifdef DMP_DEVICE2_X_BROADCOM_COM_MODSW_DOCKEREE_1
CmsRet mdm_addDefaultModSwDockerEeObjects(UINT32 *eeAddedCount);
#endif

#if !defined(DMP_DEVICE2_X_BROADCOM_COM_MODSW_OSGIEE_1) || !defined(DMP_DEVICE2_X_BROADCOM_COM_MODSW_LINUXEE_1) || !defined(DMP_DEVICE2_X_BROADCOM_COM_MODSW_OPENWRTEE_1) || !defined(SUPPORT_BEEP)
static CmsRet mdm_delModSwEeObjects(const char *eeName);
#endif


CmsRet mdm_addDefaultModSwObjects()
{
   CmsRet ret=CMSRET_SUCCESS;
   UINT32 eeDefinedCount=0;
   UINT32 eeAddedCount=0;

#ifdef DMP_DEVICE2_X_BROADCOM_COM_MODSW_OSGIEE_1
   eeDefinedCount++;
   if (ret == CMSRET_SUCCESS)
   {
      ret = mdm_addDefaultModSwOsgiEeObjects(&eeAddedCount);
   }
#else
   mdm_delModSwEeObjects(OSGI_NAME);
#endif

#ifdef DMP_DEVICE2_X_BROADCOM_COM_MODSW_LINUXEE_1
   eeDefinedCount++;
   if (ret == CMSRET_SUCCESS)
   {
      ret = mdm_addDefaultModSwLinuxEeObjects(&eeAddedCount);
   }
#else
   mdm_delModSwEeObjects(LINUXEE_NAME);
#endif

#ifdef DMP_DEVICE2_X_BROADCOM_COM_MODSW_OPENWRTEE_1
   eeDefinedCount++;
   if (ret == CMSRET_SUCCESS)
   {
      ret = mdm_addDefaultModSwOpenwrtEeObjects(&eeAddedCount);
   }
#else
   mdm_delModSwEeObjects(OPENWRTEE_NAME);
#endif

#ifdef DMP_DEVICE2_X_BROADCOM_COM_MODSW_DOCKEREE_1
   eeDefinedCount++;
   if (ret == CMSRET_SUCCESS)
   {
      ret = mdm_addDefaultModSwDockerEeObjects(&eeAddedCount);
   }
#else
   mdm_delModSwEeObjects(DOCKEREE_NAME);
#endif

   if (eeDefinedCount == 0)
   {
      cmsLog_notice("No Exec Envs were built into the system.");
   }

   if (eeAddedCount > 0)
   {
      SwModulesObject *swObj=NULL;
      InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;

      ret = mdm_getObject(MDMOID_SW_MODULES, &iidStack, (void **) &swObj);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not get SW_MODULES object, ret=%d", ret);
      }
      else
      {
         swObj->execEnvNumberOfEntries += eeAddedCount;
         cmsLog_notice("setting exe env num entries=%d", swObj->execEnvNumberOfEntries);

         ret = mdm_setObject((void **) &swObj, &iidStack, FALSE);
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("could not set SW_MODULES object, ret=%d", ret);
         }
      }
   }

   return ret;

}  /* End of mdm_addDefaultModSwObjects() */


#if !defined(DMP_DEVICE2_X_BROADCOM_COM_MODSW_OSGIEE_1) || !defined(DMP_DEVICE2_X_BROADCOM_COM_MODSW_LINUXEE_1) || !defined(DMP_DEVICE2_X_BROADCOM_COM_MODSW_OPENWRTEE_1) || !defined(DMP_DEVICE2_X_BROADCOM_COM_MODSW_DOCKEREE_1) || !defined(SUPPORT_BEEP)
static CmsRet mdm_delModSwEeObjects(const char *eeName)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   ExecEnvObject *eeObj = NULL;
   char path[CMS_MAX_FULLPATH_LENGTH];
   char *duDir = NULL;

   cmsLog_debug("Enter. eeName=%s", eeName);

   /* Clear out any old DU files (pkgs) that may be on the filesystem. */
   if (!cmsUtl_strcmp(eeName, LINUXEE_NAME))
   {
      duDir = CMS_MODSW_LINUXEE_DU_DIR;
   }
   else if (!cmsUtl_strcmp(eeName, OPENWRTEE_NAME))
   {
      duDir = CMS_MODSW_OPENWRTEE_DU_DIR;
   }
   else if (!cmsUtl_strcmp(eeName, OSGI_NAME))
   {
      duDir = CMS_MODSW_OSGIEE_DU_DIR;
   }
   else if (!cmsUtl_strcmp(eeName, DOCKEREE_NAME))
   {
      duDir = CMS_MODSW_DOCKEREE_DU_DIR;
   }
   else
   {
      cmsLog_error("Invalid eeName %s", eeName);
      return CMSRET_INVALID_ARGUMENTS;
   }

   if (cmsUtl_getRunTimePath(duDir, path, sizeof(path)) == CMSRET_SUCCESS)
   {
      if (cmsFil_isFilePresent(path))
      {
         cmsFil_removeDir(path);
      }
   }
   else
   {
      cmsLog_error("Could not make path for %s. ret=%d", duDir, ret);
   }

   /* Delete the EE object instance if exist */
   while ((ret = mdm_getNextObject(MDMOID_EXEC_ENV, &iidStack, (void **)&eeObj)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(eeObj->name, eeName))
      {
         MdmPathDescriptor pathDesc;

         cmsLog_debug("Deleting %s Execution Environment", eeName);

         INIT_PATH_DESCRIPTOR(&pathDesc);
         pathDesc.oid      = MDMOID_EXEC_ENV;
         pathDesc.iidStack = iidStack;

         if ((ret = mdm_deleteObjectInstance(&pathDesc, NULL, NULL)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Could not delete %s Execution Environment, ret=%d", eeName, ret);
         }
         cmsObj_free((void **)&eeObj);
         break;
      }
      cmsObj_free((void **)&eeObj);
   }

   if (ret == CMSRET_NO_MORE_INSTANCES)
   {
      ret = CMSRET_SUCCESS;
   }

   return ret;

}  /* End of mdm_delModSwEeObjects() */
#endif

#endif /* DMP_DEVICE2_SM_BASELINE_1 */



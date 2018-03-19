/***********************************************************************
 *
 *  Copyright (c) 2012  Broadcom Corporation
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


/* temporarily compile this unconditionally until I figure out how
 * to wrap these funcs for tr69c
 #ifdef DMP_DEVICE2_SM_BASELINE_1
 */

#include "cms.h"
#include "cms_params_modsw.h"
#include "cms_mdm.h"
#include "cms_obj.h"
#include "cms_phl.h"
#include "cms_qdm.h"
#include "cms_util.h"
#include "modsw.h"


/*!\file qdm_modsw_ee.c
 * \brief This file contains code to handle Modular Software Execution
 *        Environments (EE).  Both OSGI and Linux EE's are handled.
 */


static CmsRet getEuObjectByEuidLocked(const char *euid,
                             EUObject **euObj, InstanceIdStack *iidStack);


static UINT32 getNumExecEnvEntriesLocked()
{
   SwModulesObject *modObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UINT32 numEntries;
   CmsRet ret;

   ret = cmsObj_get(MDMOID_SW_MODULES, &iidStack, OGF_NORMAL_UPDATE,
                    (void **) &modObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get SwModulesObject! ret=%d", ret);
      return 0;
   }

   numEntries = modObj->execEnvNumberOfEntries;
   cmsObj_free((void **)&modObj);

   return numEntries;
}


static CmsRet getDuObjectByUuidLocked(const char *uuid, DUObject **duObj, InstanceIdStack *iidStack)
{
   CmsRet ret;

   if (uuid == NULL || uuid[0] == '\0')
   {
      cmsLog_error("uuid must be provided");
      return CMSRET_INVALID_ARGUMENTS;
   }

   INIT_INSTANCE_ID_STACK(iidStack);
   while ((ret = cmsObj_getNext(MDMOID_DU, iidStack, (void **) duObj)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp((*duObj)->UUID, uuid))
      {
         /* found matching DU.  Return now.  caller is responsible for freeing
          * DuObject.
          */
         return CMSRET_SUCCESS;
      }
      cmsObj_free((void **)duObj);
   }

   cmsLog_error("Could not find DUObject with uuid %s", uuid);

   return ret;
}


static CmsRet getDuObjectByDuidLocked(const char *duid, DUObject **duObj, InstanceIdStack *iidStack)
{
   CmsRet ret;

   if (duid == NULL || duid[0] == '\0')
   {
      cmsLog_error("duid must be provided");
      return CMSRET_INVALID_ARGUMENTS;
   }

   INIT_INSTANCE_ID_STACK(iidStack);
   while ((ret = cmsObj_getNext(MDMOID_DU, iidStack, (void **) duObj)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp((*duObj)->DUID, duid))
      {
         /* found matching DU.  Return now.  caller is responsible for freeing
          * DuObject.
          */
         return CMSRET_SUCCESS;
      }
      cmsObj_free((void **)duObj);
   }

   cmsLog_error("Could not find DUObject with duid %s", duid);

   return ret;
}


CmsRet qdmModsw_getExecEnvFullPathByNameLocked(const char *name,
                                      char *fullPathBuf, UINT32 bufLen)
{
   ExecEnvObject *execEnvObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret=CMSRET_SUCCESS;
   UBOOL8 found=FALSE;
   UINT32 numEntries;

   if (fullPathBuf == NULL)
   {
      cmsLog_error("fullPathBuf is NULL!");
      return CMSRET_INVALID_ARGUMENTS;
   }

   if (name == NULL || name[0] == '\0')
   {
      /* no exec env name was specified, this is OK as long as there is
       * only 1 exec env.
       */
      numEntries = getNumExecEnvEntriesLocked();
      if (numEntries > 1)
      {
         cmsLog_error("No ExecEnv name specified when there are %d ExecEnv in system",
                      numEntries);
         return CMSRET_UNKNOWN_EE;
      }
   }

   while (!found &&
          cmsObj_getNext(MDMOID_EXEC_ENV, &iidStack, (void **)&execEnvObj) == CMSRET_SUCCESS)
   {
      if (name && name[0] != '\0')
      {
         found = !cmsUtl_strcmp(name, execEnvObj->name);
      }
      else
      {
         /* no name was specified, and there is only 1 exec env, so found */
         found = TRUE;
      }

      if (found)
      {
         MdmPathDescriptor pathDesc;
         char *fullPathString=NULL;

         memset(&pathDesc, 0, sizeof(MdmPathDescriptor));
         pathDesc.oid = MDMOID_EXEC_ENV;
         pathDesc.iidStack = iidStack;
         ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &fullPathString);
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsMdm_pathDescriptorToFullPath failed with %d", ret);
         }
         else
         {
            if (cmsUtl_strlen(fullPathString)+1 > (SINT32) bufLen)
            {
               cmsLog_error("bufLen of %d is too short to hold %s", bufLen, fullPathString);
               ret = CMSRET_RESOURCE_EXCEEDED;
            }
            else
            {
               cmsUtl_strncpy(fullPathBuf, fullPathString, bufLen);
            }
            CMSMEM_FREE_BUF_AND_NULL_PTR(fullPathString);
         }
      }

      cmsObj_free((void **)&execEnvObj);
   }

   if (!found)
      ret = CMSRET_UNKNOWN_EE;

   return ret;
}


CmsRet qdmModsw_getExecEnvFullPathByUuidLocked(const char *uuid,
                                        char *fullPathBuf, UINT32 bufLen)
{
   DUObject *duObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   ret = getDuObjectByUuidLocked(uuid, &duObj, &iidStack);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not find DUObject using uuid %s", uuid);
   }
   else
   {
      if (cmsUtl_strlen(duObj->executionEnvRef)+1 > (SINT32) bufLen)
      {
         cmsLog_error("bufLen of %d is too short to hold %s", bufLen, duObj->executionEnvRef);
         ret = CMSRET_RESOURCE_EXCEEDED;
      }
      else
      {
         cmsUtl_strncpy(fullPathBuf, duObj->executionEnvRef, bufLen);
      }
      cmsObj_free((void **) &duObj);
   }

   return ret;
}


CmsRet qdmModsw_getExecEnvFullPathByContainerNameLocked(const char *containerName,
                                       char *fullPathBuf, UINT32 bufLen)
{
   ExecEnvObject *execEnvObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret=CMSRET_SUCCESS;
   UBOOL8 found=FALSE;
   UINT32 numEntries;

   if (fullPathBuf == NULL)
   {
      cmsLog_error("fullPathBuf is NULL!");
      return CMSRET_INVALID_ARGUMENTS;
   }

   if (containerName == NULL || containerName[0] == '\0')
   {
      /* no exec env container name was specified, this is OK as long as there is
       * only 1 exec env.
       */
      numEntries = getNumExecEnvEntriesLocked();
      if (numEntries > 1)
      {
         cmsLog_error("No ExecEnv container name specified when there are %d ExecEnv in system",
                      numEntries);
         return CMSRET_UNKNOWN_EE;
      }
   }

   while (!found &&
          cmsObj_getNext(MDMOID_EXEC_ENV, &iidStack, (void **)&execEnvObj) == CMSRET_SUCCESS)
   {
      if (containerName && containerName[0] != '\0')
      {
         found = !cmsUtl_strcmp(containerName, execEnvObj->X_BROADCOM_COM_ContainerName);
      }
      else
      {
         /* no container name was specified, and there is only 1 exec env, so found */
         found = TRUE;
      }

      if (found)
      {
         MdmPathDescriptor pathDesc;
         char *fullPathString=NULL;

         memset(&pathDesc, 0, sizeof(MdmPathDescriptor));
         pathDesc.oid = MDMOID_EXEC_ENV;
         pathDesc.iidStack = iidStack;
         ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &fullPathString);
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsMdm_pathDescriptorToFullPath failed with %d", ret);
         }
         else
         {
            if (cmsUtl_strlen(fullPathString)+1 > (SINT32) bufLen)
            {
               cmsLog_error("bufLen of %d is too short to hold %s", bufLen, fullPathString);
               ret = CMSRET_RESOURCE_EXCEEDED;
            }
            else
            {
               cmsUtl_strncpy(fullPathBuf, fullPathString, bufLen);
            }
            CMSMEM_FREE_BUF_AND_NULL_PTR(fullPathString);
         }
      }

      cmsObj_free((void **)&execEnvObj);
   }

   if (!found)
      ret = CMSRET_UNKNOWN_EE;

   return ret;
}


CmsRet qdmModsw_getMngrEidByExecEnvFullPathLocked(const char *fullPath,
                                                    CmsEntityId *mngrEid)
{
   ExecEnvObject *execEnvObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret=CMSRET_SUCCESS;
   UINT32 numEntries;

   if (mngrEid == NULL)
   {
      cmsLog_error("mngrEid is NULL!");
      return CMSRET_INVALID_ARGUMENTS;
   }

   if (fullPath == NULL || fullPath[0] == '\0')
   {
      /* no exec env fullpath was specified, this is OK as long as there is
       * only 1 exec env.
       */
      numEntries = getNumExecEnvEntriesLocked();
      if (numEntries > 1)
      {
         cmsLog_error("No ExecEnv fullpath specified when there are %d ExecEnv in system",
                      numEntries);
         return CMSRET_INVALID_ARGUMENTS;
      }

      /* get the first and only execEnv */
      ret = cmsObj_getNext(MDMOID_EXEC_ENV, &iidStack, (void **)&execEnvObj);
      if (ret == CMSRET_SUCCESS)
      {
         *mngrEid = execEnvObj->X_BROADCOM_COM_MngrEid;
         cmsObj_free((void **)&execEnvObj);
      }
      else
      {
         cmsLog_error("failed to get single ExecEnv obj, ret=%d", ret);
      }
   }
   else
   {
      /* fullPath was specified, so first convert it to MdmDescriptor, and
       * then get the object at that specific iidStack */
      MdmPathDescriptor pathDesc;

      memset(&pathDesc, 0, sizeof(MdmPathDescriptor));
      ret = cmsMdm_fullPathToPathDescriptor(fullPath, &pathDesc);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not convert %s, ret=%d", fullPath, ret);
      }
      else
      {
         ret = cmsObj_get(MDMOID_EXEC_ENV, &(pathDesc.iidStack),
                          OGF_NO_VALUE_UPDATE, (void **)&execEnvObj);
         if (ret == CMSRET_SUCCESS)
         {
            *mngrEid = execEnvObj->X_BROADCOM_COM_MngrEid;
            cmsObj_free((void **)&execEnvObj);
         }
         else
         {
            cmsLog_error("failed to get ExecEnv obj, ret=%d", ret);
         }
      }
   }

   return ret;
}


CmsRet qdmModsw_getExecEnvObjectByFullPathLocked(const char *fullPath,
                                                 ExecEnvObject **eeObject,
                                                 InstanceIdStack *iidStack)
{
   CmsRet ret = CMSRET_SUCCESS;
   MdmPathDescriptor pathDesc;

   cmsLog_debug("Enter - full path %s", fullPath);

   memset(&pathDesc, 0, sizeof(MdmPathDescriptor));

   ret = cmsMdm_fullPathToPathDescriptor(fullPath, &pathDesc);

   if (ret == CMSRET_SUCCESS)
   {
      ret = cmsObj_get(pathDesc.oid,
                       &(pathDesc.iidStack),
                       OGF_NO_VALUE_UPDATE, 
                       (void **)eeObject);

      if (ret == CMSRET_SUCCESS)
      {
         memcpy(iidStack, &(pathDesc.iidStack), sizeof(InstanceIdStack));
      }
      else
      {
         cmsLog_error("Cannot get ExecEnvObject by full path %s. Ret = %d",
                      fullPath, ret);
      }
   }
   else
   {
      cmsLog_error("Cannot convert full path to path descriptor. Ret = %d", ret);
   }

   return ret;
}


CmsRet qdmModsw_getExecEnvEnableByFullPathLocked(const char *fullPath,
                                                 UBOOL8 *enable)
{
   CmsRet ret = CMSRET_SUCCESS;
   ExecEnvObject *eeObject = NULL;
   InstanceIdStack iidStack;

   INIT_INSTANCE_ID_STACK(&iidStack);

   ret = qdmModsw_getExecEnvObjectByFullPathLocked(fullPath,
                                                   &eeObject,
                                                   &iidStack);

   if (ret == CMSRET_SUCCESS)
   {
      *enable = eeObject->enable;

      cmsObj_free((void **)&eeObject);
   }

   return ret;
}


CmsRet qdmModsw_getExecEnvStatusByFullPathLocked(const char *fullPath,
                                                 char *statusBuf,
                                                 UINT32 statusBufLen)
{
   CmsRet ret = CMSRET_SUCCESS;
   ExecEnvObject *eeObject = NULL;
   InstanceIdStack iidStack;

   INIT_INSTANCE_ID_STACK(&iidStack);

   ret = qdmModsw_getExecEnvObjectByFullPathLocked(fullPath,
                                                   &eeObject,
                                                   &iidStack);

   if (ret == CMSRET_SUCCESS)
   {
      cmsUtl_strncpy(statusBuf, eeObject->status, statusBufLen);

      cmsObj_free((void **)&eeObject);
   }

   return ret;
}


CmsRet qdmModsw_getExecEnvNameByFullPathLocked(const char *fullPath,
                                               char *nameBuf,
                                               UINT32 nameBufLen)
{
   ExecEnvObject *execEnvObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret=CMSRET_SUCCESS;
   UINT32 numEntries;

   if (fullPath == NULL || fullPath[0] == '\0')
   {
      /* no exec env fullpath was specified, this is OK as long as there is
       * only 1 exec env.
       */
      numEntries = getNumExecEnvEntriesLocked();
      if (numEntries > 1)
      {
         cmsLog_error("No ExecEnv fullpath specified when there are %d ExecEnv in system",
                      numEntries);
         return CMSRET_INVALID_ARGUMENTS;
      }

      /* get the first and only execEnv */
      ret = cmsObj_getNext(MDMOID_EXEC_ENV, &iidStack, (void **)&execEnvObj);
      if (ret == CMSRET_SUCCESS)
      {
         cmsUtl_strncpy(nameBuf, execEnvObj->name, nameBufLen);
         cmsObj_free((void **)&execEnvObj);
      }
      else
      {
         cmsLog_error("failed to get single ExecEnv obj, ret=%d", ret);
      }
   }
   else
   {
      /* fullPath was specified, so first convert it to MdmDescriptor, and
       * then get the object at that specific iidStack */

      INIT_INSTANCE_ID_STACK(&iidStack);

      ret = qdmModsw_getExecEnvObjectByFullPathLocked(fullPath,
                                                      &execEnvObj,
                                                      &iidStack);

      if (ret == CMSRET_SUCCESS)
      {
         cmsUtl_strncpy(nameBuf, execEnvObj->name, nameBufLen);

         cmsObj_free((void **)&execEnvObj);
      }
      else
      {
         cmsLog_error("Failed to get ExecEnv obj, ret=%d", ret);
      }
   }

   return ret;
}

/*************************************************************************
 *
 * This section contains the getDeployUnit functions.
 *
 *************************************************************************
 */

CmsRet qdmModsw_getDeployUnitFullPathByDuidLocked(const char *duid,
                                        char *fullPathBuf, UINT32 bufLen)
{
   DUObject *duObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   ret = getDuObjectByDuidLocked(duid, &duObj, &iidStack);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not find DUObject using duid %s", duid);
   }
   else
   {
      MdmPathDescriptor pathDesc;
      char *fullStr=NULL;

      memset(&pathDesc, 0, sizeof(MdmPathDescriptor));
      pathDesc.oid = MDMOID_DU;
      pathDesc.iidStack = iidStack;
      cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &fullStr);
      if (cmsUtl_strlen(fullStr)+1 > (SINT32) bufLen)
      {
         cmsLog_error("fullpath %s too long for provided buffer (len=%d)",
                       fullStr, bufLen);
      }
      else
      {
         cmsUtl_strncpy(fullPathBuf, fullStr, bufLen);
      }
      CMSMEM_FREE_BUF_AND_NULL_PTR(fullStr);
   }

   return ret;
}


CmsRet qdmModsw_getDeployUnitNameByEuFullPathLocked(const char *euFullPath,
                                           char *duName, UINT32 duNameLen)
{
   DUObject *duObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;
   UBOOL8 found=FALSE;

   while (!found &&
          (ret = cmsObj_getNext(MDMOID_DU, &iidStack, (void **) &duObj)) == CMSRET_SUCCESS)
   {
      char *string, *token;

      string = duObj->executionUnitList;
      while (string != NULL)
      {
         token = strsep(&string, ",");

         if (!cmsUtl_strcmp(token, euFullPath))
         {
            found = TRUE;

            if (cmsUtl_strlen(duObj->name)+1 > (SINT32) duNameLen)
            {
                cmsLog_error("duName %s too long for provided buffer (len=%d)",
                              duObj->name, duNameLen);
                ret = CMSRET_RESOURCE_EXCEEDED;
            }
            else
            {
               cmsUtl_strncpy(duName, duObj->name, duNameLen);
            }

            break;
         }
      }
      cmsObj_free((void **)&duObj);
   }

   return ret;
}


CmsRet qdmModsw_getDeployUnitStatusByUuidEeFullPathLocked(const char *uuid,
                                                          const char *eeFullPath,
                                                          char *status,
                                                          UINT32 statusLen)
{
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 found = FALSE;
   DUObject *duObject = NULL;
   InstanceIdStack iidStack;

   cmsLog_debug("uuid=%s eeFullPath=%s", uuid, eeFullPath);

   INIT_INSTANCE_ID_STACK(&iidStack);

   while (found == FALSE &&
          cmsObj_getNextFlags(MDMOID_DU,
                              &iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **)&duObject) == CMSRET_SUCCESS)
   {
      found = (cmsUtl_strcmp(duObject->UUID, uuid) == 0 &&
               cmsUtl_strcmp(duObject->executionEnvRef, eeFullPath) == 0);

      if (found == TRUE)
      {
         cmsUtl_strncpy(status, duObject->status, statusLen);
      }

      cmsObj_free((void **)&duObject);
   }

   return ret;
}


/*************************************************************************
 *
 * This section contains the getExecUnit functions.
 *
 *************************************************************************
 */

CmsRet getEuObjectByEuidLocked(const char *euid, EUObject **euObj, InstanceIdStack *iidStack)
{
   CmsRet ret;

   if (euid == NULL || euid[0] == '\0')
   {
      cmsLog_error("euid must be provided");
      return CMSRET_INVALID_ARGUMENTS;
   }

   INIT_INSTANCE_ID_STACK(iidStack);
   while ((ret = cmsObj_getNextFlags(MDMOID_EU,
                                     iidStack,
                                     OGF_NO_VALUE_UPDATE,
                                     (void **) euObj)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp((*euObj)->EUID, euid))
      {
         /* found matching EU.  Return now.  Caller is responsible for freeing
          * EuObject.
          */
         return CMSRET_SUCCESS;
      }
      cmsObj_free((void **)euObj);
   }

   cmsLog_error("Could not find EUObject with euid %s", euid);

   return ret;
}


CmsRet qdmModsw_getExecUnitFullPathByEuidLocked(const char *euid,
                                        char *fullPathBuf, UINT32 bufLen)
{
   EUObject *euObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   ret = getEuObjectByEuidLocked(euid, &euObj, &iidStack);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not find EUObject using euid %s", euid);
   }
   else
   {
      MdmPathDescriptor pathDesc;
      char *fullStr=NULL;

      memset(&pathDesc, 0, sizeof(MdmPathDescriptor));
      pathDesc.oid = MDMOID_EU;
      pathDesc.iidStack = iidStack;
      cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &fullStr);
      if (cmsUtl_strlen(fullStr)+1 > (SINT32) bufLen)
      {
         cmsLog_error("fullpath %s too long for provided buffer (len=%d)",
                       fullStr, bufLen);
         ret = CMSRET_RESOURCE_EXCEEDED;
      }
      else
      {
         cmsUtl_strncpy(fullPathBuf, fullStr, bufLen);
      }

      CMSMEM_FREE_BUF_AND_NULL_PTR(fullStr);
      cmsObj_free((void **)&euObj);
   }

   return ret;
}


CmsRet qdmModsw_getExecutionUnitParamsByEuFullPathLocked(const char *euFullPath,
                     char *euName, UINT32 euNameLen,
                     char *euid, UINT32 euidLen,
                     char *username, UINT32 usernameLen,
                     char *status, UINT32 statusLen,
                     char *mngrAppName, UINT32 mngrAppNameLen)
{
   EUObject *euObj=NULL;
   MdmPathDescriptor pathDesc;    
   CmsRet ret = CMSRET_SUCCESS;

   INIT_PATH_DESCRIPTOR(&pathDesc);

   ret = cmsMdm_fullPathToPathDescriptor(euFullPath, &pathDesc);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_fullPathToPathDescriptor failed for %s, ret=%d", euFullPath, ret);
      return ret;
   }

   ret= cmsObj_get(pathDesc.oid, &(pathDesc.iidStack), OGF_NO_VALUE_UPDATE, (void **)&euObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get euObject object , ret=%d", ret);
      return ret;
   }

   if (euName)
   {
      cmsUtl_strncpy(euName, euObj->name, euNameLen-1);
   }
   if (euid)
   {
      cmsUtl_strncpy(euid, euObj->EUID, euidLen-1);
   }
   if (username)
   {
      cmsUtl_strncpy(username, euObj->X_BROADCOM_COM_Username, usernameLen);
   }
   if (status)
   {
      cmsUtl_strncpy(status, euObj->status, statusLen-1);
   }
   if (mngrAppName)
   {
      cmsUtl_strncpy(mngrAppName, euObj->X_BROADCOM_COM_MngrAppName, mngrAppNameLen-1);
   }
   cmsObj_free((void **) &euObj);

   return ret;
}

CmsRet qdmModsw_getEEVersionByEEFullPathLocked(const char *eeFullPath, char *eeVersion, UINT32  eeVerLen)
{
   CmsRet ret = CMSRET_SUCCESS;
   ExecEnvObject *eeObject = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   ret = qdmModsw_getExecEnvObjectByFullPathLocked(eeFullPath,
                                                   &eeObject,
                                                   &iidStack);

   if (ret == CMSRET_SUCCESS)
   {
      cmsUtl_strncpy(eeVersion, eeObject->version, eeVerLen);
      cmsObj_free((void **)&eeObject);
   }

   return ret;
}


CmsRet qdmModsw_getDuNameDuInstanceFromDuid(const char *duid, char *duName, UINT32 duNameLen, char *duInstString, UINT32 duInstStringLen)
{
   char *pToken = NULL;
   char *pLast = NULL;
   UINT32 count=0;
   CmsRet ret = CMSRET_SUCCESS;
   char localDuid[BUFLEN_64];
   
   memset(duName, 0, duNameLen);
   memset(duInstString, 0, duInstStringLen);
   cmsUtl_strncpy(localDuid, duid, sizeof(localDuid)-1);
   
   pToken = strtok_r(localDuid, ":", &pLast);
    /* need to find the  duName and du instance number from beep:1.0:cwmp:1  */
   while (( pToken != NULL) && (count < 4))
   {
      if (count ==2)
      {
         cmsUtl_strncpy(duName, pToken, duNameLen);
      }
      if (count ==3)
      {
         cmsUtl_strncpy(duInstString, pToken, duInstStringLen);
      }      
      count++;
      pToken = strtok_r(NULL, ":", &pLast);
   }

   if (IS_EMPTY_STRING(duName) || IS_EMPTY_STRING(duInstString))
   {
      cmsLog_error("Invalid duid %s", duid);
      ret = CMSRET_INVALID_ARGUMENTS;
   }

   return ret;
   
}


CmsRet qdmModsw_getDuNameDuInstanceFromUuidVersionEeFullPathLocked(const char *uuid,
                                                                   const char *version,
                                                                   const char *eeFullPath,
                                                                   char *duName,
                                                                   UINT32 duNameLen,
                                                                   char *duInst,
                                                                   UINT32 duInstLen)
{
   CmsRet ret = CMSRET_OBJECT_NOT_FOUND;
   UBOOL8 found = FALSE;
   DUObject *duObject = NULL;
   InstanceIdStack iidStack;
   
   cmsLog_debug("uuid=%s version=%s eeFullPath=%s", uuid, version, eeFullPath);

   INIT_INSTANCE_ID_STACK(&iidStack);
   
   while (found == FALSE &&
          cmsObj_getNextFlags(MDMOID_DU,
                              &iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **)&duObject) == CMSRET_SUCCESS)
   {
      if (version[0] == '\0')
      {
         found = (cmsUtl_strcmp(duObject->UUID, uuid) == 0 &&
                  cmsUtl_strcmp(duObject->executionEnvRef, eeFullPath) == 0);
      }
      else
      {
         found = (cmsUtl_strcmp(duObject->UUID, uuid) == 0 &&
                  cmsUtl_strcmp(duObject->version, version) == 0 &&
                  cmsUtl_strcmp(duObject->executionEnvRef, eeFullPath) == 0);
      }
      
      if (found == TRUE)
      {
         cmsUtl_strncpy(duName, duObject->name, duNameLen);
         snprintf(duInst, duInstLen, "%d", PEEK_INSTANCE_ID(&iidStack));
         ret = CMSRET_SUCCESS;
      }
      
      cmsObj_free((void **)&duObject);
   }
   
   cmsLog_debug("duName=%s duInst=%s ret=%d", duName, duInst, ret);
   
   return ret;
}


CmsRet qdmModsw_getBeepPkgAppNamesLocked(const char *pkgName, char *pkgAppNames, UINT32 pkgAppNamesLen)
{
   DUObject *duObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;
   UBOOL8 found=FALSE;

   memset(pkgAppNames, 0, pkgAppNamesLen);

   INIT_INSTANCE_ID_STACK(&iidStack);
   while (!found &&
          (ret = cmsObj_getNext(MDMOID_DU, &iidStack, (void **) &duObj)) == CMSRET_SUCCESS)
   {
      /* We only support multiple exec units per deployment unit (du name is the pkgName). 
       * Keep adding the EU (appName) sperated by ','.
       */
      if  (!cmsUtl_strcmp(duObj->name, pkgName))
      {
         char allEuFullPathBuf[MDM_MULTI_FULLPATH_BUFLEN];         
         char euFullPath[MDM_SINGLE_FULLPATH_BUFLEN];
         char *pToken = NULL;
         char *pLast = NULL;
         MdmPathDescriptor pathDesc;    
         EUObject *euObject=NULL;
               
         found = TRUE;
         cmsUtl_strncpy(allEuFullPathBuf, duObj->executionUnitList, sizeof(allEuFullPathBuf)-1);
               
         pToken = strtok_r(allEuFullPathBuf, ", ", &pLast);

         while (pToken != NULL)
         {
            cmsUtl_strcpy(euFullPath, pToken);
            INIT_PATH_DESCRIPTOR(&pathDesc);
            ret = cmsMdm_fullPathToPathDescriptor(euFullPath, &pathDesc);
            if (ret != CMSRET_SUCCESS)
            {
               cmsLog_error("cmsMdm_fullPathToPathDescriptor failed for %s, ret=%d", euFullPath, ret);
               return ret;
            }

            ret= cmsObj_get(pathDesc.oid, &(pathDesc.iidStack), OGF_NO_VALUE_UPDATE, (void **)&euObject);
            if (ret != CMSRET_SUCCESS)
            {
               cmsLog_error("could not get euObject object , ret=%d", ret);
               return ret;
            }
            cmsUtl_strncat(pkgAppNames, pkgAppNamesLen, euObject->name);
            cmsUtl_strncat(pkgAppNames, pkgAppNamesLen, ",");
            
            cmsObj_free((void **) &euObject);
            
            pToken = strtok_r(NULL, ", ", &pLast);

         }

      }
      cmsObj_free((void **)&duObj);
   }

   if (pkgAppNames[cmsUtl_strlen(pkgAppNames)-1] == ',')
   {
      pkgAppNames[cmsUtl_strlen(pkgAppNames)-1] = '\0';
   }
   cmsLog_debug("pkgAppNames %s, ret = %d", pkgAppNames, ret);

   return ret;
}



UBOOL8 qdmModsw_isDuResolvedByEuIidStackLocked(const InstanceIdStack *euIidStack)
{
   CmsRet ret = CMSRET_SUCCESS;
   DUObject *duObject = NULL;
   UBOOL8 isResolved = FALSE;
   UBOOL8 found = FALSE;
   MdmPathDescriptor pathDesc;
   char *euFullPath;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   
   
   pathDesc.oid = MDMOID_EU;
   pathDesc.iidStack = *euIidStack;
   pathDesc.paramName[0] = '\0';

   if ((ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &euFullPath)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_pathDescriptorToFullPath returns error. ret=%d", ret);
   }
   else
   {
      while (!found  &&
             cmsObj_getNextFlags(MDMOID_DU,
                                 &iidStack,
                                 OGF_NO_VALUE_UPDATE,
                                 (void **)&duObject) == CMSRET_SUCCESS)
      {
         if (cmsUtl_isFullPathInCSL(euFullPath, duObject->executionUnitList) == TRUE)
         {
            found = TRUE;
            isResolved = duObject->resolved;
         }
         cmsObj_free((void **)&duObject);
      }
      cmsLog_debug("euFullPath %s",  euFullPath);
      CMSMEM_FREE_BUF_AND_NULL_PTR(euFullPath);      
   }

   cmsLog_debug("resolved = %d",  isResolved);

   return isResolved;
   
}


CmsRet qdmModsw_getBeepEuManifestInfoByEuidLocked(const char *euid, char *network, UINT32 ntwkLen, char *ports, int portsLen, UINT32 *intfidx)
{
   EUObject *euObj = NULL;
   UBOOL8 found = FALSE;
   InstanceIdStack iidStack;
   CmsRet ret = CMSRET_SUCCESS;

   if (IS_EMPTY_STRING(euid))
   {
      cmsLog_error("euid is empty string");
      return CMSRET_INVALID_ARGUMENTS;
   }

   *network = *ports = '\0';
   *intfidx = 0;

   INIT_INSTANCE_ID_STACK(&iidStack);
   while (!found && cmsObj_getNextFlags(MDMOID_EU, &iidStack, OGF_NO_VALUE_UPDATE, (void **) &euObj) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(euObj->EUID, euid))
      {
         found = TRUE;
      }
      cmsObj_free((void **)&euObj);
   }

   if (found)
   {
      ManifestObject *manifest = NULL;
      InstanceIdStack iidStackMan = EMPTY_INSTANCE_ID_STACK;

      if (cmsObj_getNextInSubTreeFlags(MDMOID_MANIFEST, &iidStack, &iidStackMan, OGF_NO_VALUE_UPDATE, (void **)&manifest) == CMSRET_SUCCESS)
      {
         *intfidx = manifest->interfaceIdentifier;
         cmsUtl_strncpy(network, manifest->networkMode, ntwkLen);
         cmsUtl_strncpy(ports, manifest->exposedPorts, portsLen);

         cmsLog_debug("ntwk<%s> ports<%s> idx<%u>", manifest->networkMode, 
                      manifest->exposedPorts, manifest->interfaceIdentifier);
         cmsObj_free((void **)&manifest);
      }
      else
      {
         cmsLog_error("cannot get manifest object for euid<%s>", euid);
         ret = CMSRET_INTERNAL_ERROR;
      }
   }
   else
   {
      cmsLog_error("cannot find EUObj with euid<%s>", euid);
      ret = CMSRET_INVALID_ARGUMENTS;
   }

   return ret;
}


CmsRet qdmModsw_getBeepEuAddrInfoByEuidLocked(const char *euid, 
                                              char *addr, int addrLen)
{
   EUObject *euObj = NULL;
   UBOOL8 found = FALSE;
   InstanceIdStack iidStack;
   CmsRet ret = CMSRET_SUCCESS;

   if (IS_EMPTY_STRING(euid))
   {
      cmsLog_error("euid is empty string");
      return CMSRET_INVALID_ARGUMENTS;
   }

   *addr = '\0';

   INIT_INSTANCE_ID_STACK(&iidStack);
   while (!found && cmsObj_getNextFlags(MDMOID_EU, &iidStack, OGF_NO_VALUE_UPDATE, (void **) &euObj) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(euObj->EUID, euid))
      {
         found = TRUE;
         cmsUtl_strncpy(addr, euObj->X_BROADCOM_COM_IPAddress, addrLen);
      }
      cmsObj_free((void **)&euObj);
   }

   if (found)
   {
      cmsLog_debug("euid<%s> with addr<%s>", euid, addr);
   }
   else
   {
      cmsLog_error("cannot find EUObj with euid<%s>", euid);
      ret = CMSRET_INVALID_ARGUMENTS;
   }

   return ret;
}


CmsRet qdmModsw_getBeepEuMacIdxLocked(const char *euid, UINT32 *idx)
{
   char ntwk[BUFLEN_32];
   char ports[BUFLEN_128];
   CmsRet ret;

   *idx = 0;

   ret = qdmModsw_getBeepEuManifestInfoByEuidLocked(euid, ntwk, sizeof(ntwk), 
                                                ports, sizeof(ports), idx);

   return ret;
}

CmsRet qdmModsw_getBeepEuNetworkInfoByEuidLocked(const char *euid, 
                      char *ntwkMode, int modeLen, char *ports, int portLen,
                      char *ipaddr, int addrLen)
{
   UINT32 idx;
   CmsRet ret;

   *ports = '\0';

   ret = qdmModsw_getBeepEuManifestInfoByEuidLocked(euid, ntwkMode, modeLen, 
                                                ports, portLen, &idx);
   if (ret == CMSRET_SUCCESS)
   {
      qdmModsw_getBeepEuAddrInfoByEuidLocked(euid, ipaddr, addrLen);
   }

   cmsLog_debug("mode<%s> ports<%s> addr<%s>", ntwkMode, ports, ipaddr);

   return ret;
}


CmsRet qdmModsw_getDuidByEuFullPathLocked (const char *euFullPath, 
                                           char *duid, 
                                           UINT32 duidLen)
                                           
{
   DUObject *duObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret=CMSRET_DU_UNKNOWN;
   UBOOL8 found=FALSE;

   while (!found &&
          (ret = cmsObj_getNext(MDMOID_DU, &iidStack, (void **) &duObj)) == CMSRET_SUCCESS)
   {

      if (cmsUtl_isFullPathInCSL(euFullPath, duObj->executionUnitList) == TRUE)
      {
         found = TRUE;
         cmsUtl_strncpy(duid, duObj->DUID, duidLen);
         ret = CMSRET_SUCCESS;
      }
      cmsObj_free((void **)&duObj);
   }

   cmsLog_debug("duid %s.  ret %d", duid, ret);
   
   return ret;
   
}



CmsRet qdmModsw_getEeDirByDuidLocked(const char *duid, int *flash,
                               char *eeDirName, 
                               UINT32 eeDirNameLen)
{
   DUObject *duObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found=FALSE;
   CmsRet ret;
   char eeName[BUFLEN_32]={0};
   char rootDuDir[CMS_MAX_FULLPATH_LENGTH]={0};   /* /local/modsw/tr157du */

   while (!found &&
          (ret = cmsObj_getNext(MDMOID_DU, &iidStack, (void **) &duObj)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(duObj->DUID, duid))
      {
         ExecEnvObject *eeObject = NULL;
         InstanceIdStack eeIid = EMPTY_INSTANCE_ID_STACK;

         ret = qdmModsw_getExecEnvObjectByFullPathLocked(duObj->executionEnvRef,
                                                         &eeObject,
                                                         &eeIid);

         if (ret == CMSRET_SUCCESS)
         {
            *flash = eeObject->allocatedDiskSpace;
            cmsUtl_strncpy(eeName, eeObject->name, sizeof(eeName)-1);
            cmsObj_free((void **)&eeObject);
            found = TRUE;

         }
      }
      cmsObj_free((void **)&duObj);
   }

   if (!found)
   {
      cmsLog_error("Could not find eeName from duid %s", duid);
   }

   cmsUtl_getRunTimePath(CMS_MODSW_DU_DIR, rootDuDir, sizeof(rootDuDir)-1);
   snprintf(eeDirName, eeDirNameLen, "%s/%s", rootDuDir, eeName);

   cmsLog_debug("duid %s. eeDirName %s,  ret %d", duid, eeDirName, ret);
      
   return ret;

}


CmsRet qdmModsw_getEeNameByDuUuidLocked(const char *duUuid, 
                                        char *eeName, 
                                        UINT32 eeNameLen)
{
   DUObject *duObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found=FALSE;
   CmsRet ret;
   
   while (!found &&
          (ret = cmsObj_getNext(MDMOID_DU, &iidStack, (void **) &duObj)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(duObj->UUID, duUuid))
      {
         ExecEnvObject *eeObject = NULL;
         InstanceIdStack eeIid = EMPTY_INSTANCE_ID_STACK;

         ret = qdmModsw_getExecEnvObjectByFullPathLocked(duObj->executionEnvRef,
                                                         &eeObject,
                                                         &eeIid);

         if (ret == CMSRET_SUCCESS)
         {
            cmsUtl_strncpy(eeName, eeObject->name, eeNameLen);
            cmsObj_free((void **)&eeObject);
            found = TRUE;
            cmsLog_debug("duUuid %s, eeName %s. ret %d.", duUuid, eeName, ret);
         }
      }
      cmsObj_free((void **)&duObj);
   }

   if (!found)
   {
      cmsLog_error("Could not find eeName from duUuid %s", duUuid);
   }

   cmsLog_debug("duUuid %s, eeName %s. ret %d.", duUuid, eeName, ret);
   
   return ret;

}


CmsRet qdmModsw_getEeNameFlashByEuFullPathLocked(const char *euFullPath,
                                                 int *flash,
                                                 char *eeName, 
                                                 UINT32 eeNameLen)
{
   MdmPathDescriptor pathDesc;
   EUObject *euObj = NULL; 
   ExecEnvObject *eeObject = NULL;
   InstanceIdStack eeIid = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   INIT_PATH_DESCRIPTOR(&pathDesc);

   ret = cmsMdm_fullPathToPathDescriptor(euFullPath, &pathDesc);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed for %s, ret=%d", euFullPath, ret);
      return ret;
   }
   
   ret= cmsObj_get(pathDesc.oid, &(pathDesc.iidStack),
                   OGF_NO_VALUE_UPDATE, (void **)&euObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get euObject object , ret=%d", ret);
      return ret;
   }

   ret = qdmModsw_getExecEnvObjectByFullPathLocked(euObj->executionEnvRef,
                                                   &eeObject,
                                                   &eeIid);
   cmsObj_free((void **)&euObj);

   if (ret == CMSRET_SUCCESS)
   {
      cmsUtl_strncpy(eeName, eeObject->name, eeNameLen);
      *flash = eeObject->allocatedDiskSpace;
   }
   else
   {
      cmsLog_error("Failed for %s, ret=%d", euFullPath, ret);
      return ret;
   }
   
   cmsObj_free((void **)&eeObject);
   
   cmsLog_debug("eeName<%s> flash<%d> from euFullpath %s. ret %d",
                eeName, *flash, euFullPath, ret);
      
   return ret;
}


CmsRet qdmModsw_getEeNameByEuFullPathLocked(const char *euFullPath, 
                                            char *eeName, 
                                            UINT32 eeNameLen)
{
   MdmPathDescriptor pathDesc;
   EUObject *euObj = NULL; 
   ExecEnvObject *eeObject = NULL;
   InstanceIdStack eeIid = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   INIT_PATH_DESCRIPTOR(&pathDesc);

   ret = cmsMdm_fullPathToPathDescriptor(euFullPath, &pathDesc);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_fullPathToPathDescriptor failed for %s, ret=%d", euFullPath, ret);
      return ret;
   }
   
   ret= cmsObj_get(pathDesc.oid, &(pathDesc.iidStack), OGF_NO_VALUE_UPDATE, (void **)&euObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get euObject object , ret=%d", ret);
      return ret;
   }

   ret = qdmModsw_getExecEnvObjectByFullPathLocked(euObj->executionEnvRef,
                                                   &eeObject,
                                                   &eeIid);
   cmsObj_free((void **)&euObj);

   if (ret == CMSRET_SUCCESS)
   {
      cmsUtl_strncpy(eeName, eeObject->name, eeNameLen);
   }
   else
   {
      cmsLog_error("qdmModsw_getExecEnvObjectByFullPathLocked failed for %s, ret=%d", euFullPath, ret);
      return ret;
   }
   
   cmsObj_free((void **)&eeObject);
   
   cmsLog_debug("eeName %s from euFullpath %s.  ret %d", eeName, euFullPath, ret);
      
   return ret;
   

}

CmsRet qdmModsw_getEeDirByEuFullPathLocked(const char *euFullPath, int *flash,
                                           char *eeDirName, 
                                           UINT32 eeDirNameLen)
{                                      
   CmsRet ret = CMSRET_SUCCESS;
   char eeName[BUFLEN_32]={0};
   char rootDuDir[CMS_MAX_FULLPATH_LENGTH]={0};   /* /local/modsw/tr157du */

   if ((ret = qdmModsw_getEeNameFlashByEuFullPathLocked(euFullPath, flash,
                                eeName, sizeof(eeName)-1)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed with euFullpath %s, ret=%d", euFullPath, ret);
      return ret;
   }

   cmsUtl_getRunTimePath(CMS_MODSW_DU_DIR, rootDuDir, sizeof(rootDuDir)-1);   
   snprintf(eeDirName, eeDirNameLen, "%s/%s", rootDuDir, eeName);

   cmsLog_debug("eeDirName %s for eullpath %s  ", eeDirName, euFullPath);
   
   return ret;
   
}

CmsRet qdmModsw_getEeDirByEeFullPathLocked(const char *eeFullPath, 
                                           char *eeDirName, 
                                           UINT32 eeDirNameLen)
{                                      
   /* fullPath was specified, so first convert it to MdmDescriptor, and
    * then get the object at that specific iidStack */
   MdmPathDescriptor pathDesc;
   CmsRet ret = CMSRET_SUCCESS;
   ExecEnvObject *eeObject = NULL;
   char eeName[BUFLEN_32]={0};
   char rootDuDir[CMS_MAX_FULLPATH_LENGTH]={0};   /* /local/modsw/tr157du */

   memset(&pathDesc, 0, sizeof(MdmPathDescriptor));
   ret = cmsMdm_fullPathToPathDescriptor(eeFullPath, &pathDesc);
   
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not convert %s, ret=%d", eeFullPath, ret);
   }
   else
   {
      ret = cmsObj_get(MDMOID_EXEC_ENV, &(pathDesc.iidStack),
                       OGF_NO_VALUE_UPDATE, (void **)&eeObject);
      if (ret == CMSRET_SUCCESS)
      {
         cmsUtl_strncpy(eeName, eeObject->name, sizeof(eeName)-1);
         cmsObj_free((void **)&eeObject);

         cmsUtl_getRunTimePath(CMS_MODSW_DU_DIR, rootDuDir, sizeof(rootDuDir)-1);
         snprintf(eeDirName, eeDirNameLen, "%s/%s", rootDuDir, eeName);
         cmsLog_debug("eeDirName %s for eeFullpath %s  ", eeDirName, eeFullPath);
      }
      else
      {
         cmsLog_error("failed to get ExecEnv obj, ret=%d", ret);
      }

   }
   
   return ret;
   
}

CmsRet qdmModsw_getDuVendorStringByEuNameLocked(const char *euName, 
                                                    char *duVendor, 
                                                    UINT32 duVendorLen)
                                                     
{
   DUObject *duObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret=CMSRET_DU_UNKNOWN;
   UBOOL8 found=FALSE;
   EUObject *euObj = NULL;
   char euFullPath[MDM_SINGLE_FULLPATH_BUFLEN]={0};

   /* Need to find the EU and it's euFullpaht to find the DU it belongs to */
   while (!found && cmsObj_getNextFlags(MDMOID_EU, &iidStack, OGF_NO_VALUE_UPDATE, (void **) &euObj) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(euObj->name, euName))
      {
         found = TRUE;
      }
      cmsObj_free((void **)&euObj);
   }
   
   if (found)
   {
      MdmPathDescriptor pathDesc;
      char *fullStr=NULL;

      memset(&pathDesc, 0, sizeof(MdmPathDescriptor));
      pathDesc.oid = MDMOID_EU;
      pathDesc.iidStack = iidStack;
      cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &fullStr);
      if (cmsUtl_strlen(fullStr)+1 > (SINT32) sizeof(euFullPath)-1)
      {
         cmsLog_error("fullpath %s too long for provided buffer (len=%d)", fullStr, sizeof(euFullPath));
         ret = CMSRET_RESOURCE_EXCEEDED;
      }
      else
      {
         cmsUtl_strncpy(euFullPath, fullStr, sizeof(euFullPath)-1);
      }

      CMSMEM_FREE_BUF_AND_NULL_PTR(fullStr);
   }  
   else
   {
      cmsLog_error("Failed to find eu object with euName %s", euName);
      return CMSRET_INTERNAL_ERROR;
   }

   /* Need to find again for duObj to get the vendor string */
   found = FALSE;
   INIT_INSTANCE_ID_STACK(&iidStack);

   while (!found &&
   
          (ret = cmsObj_getNext(MDMOID_DU, &iidStack, (void **) &duObj)) == CMSRET_SUCCESS)
   {

      if (cmsUtl_isFullPathInCSL(euFullPath, duObj->executionUnitList) == TRUE)
      {
         found = TRUE;
         cmsUtl_strncpy(duVendor, duObj->vendor, duVendorLen);
         ret = CMSRET_SUCCESS;
      }
      cmsObj_free((void **)&duObj);
   }

   cmsLog_debug("duVendor %s. ret %d, found %d", duVendor, ret, found);
   
   return ret;
   
}


CmsRet qdmModsw_getDuVendorStringFromDuid(const char *duUuid, char *duVendor, UINT32 duVendorLen)
{
   DUObject *duObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret=CMSRET_DU_UNKNOWN;
   UBOOL8 found=FALSE;

   if (IS_EMPTY_STRING(duUuid))
   {
      cmsLog_error("uuid must be provided");
      return CMSRET_INVALID_ARGUMENTS;
   }

   while (!found &&
         (ret = cmsObj_getNext(MDMOID_DU, &iidStack, (void **) &duObj)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(duObj->UUID, duUuid))
      {
         cmsUtl_strncpy(duVendor, duObj->vendor, duVendorLen);
         found = TRUE;
         ret = CMSRET_SUCCESS;
      }
      cmsObj_free((void **)&duObj);
   }

   cmsLog_debug("duVendor %s", duVendor);

   return ret;
}


/*
#endif DMP_DEVICE2_SM_BASELINE_1 */


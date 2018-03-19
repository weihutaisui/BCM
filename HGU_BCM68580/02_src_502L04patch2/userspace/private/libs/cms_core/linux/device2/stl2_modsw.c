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

#ifdef DMP_DEVICE2_SM_BASELINE_1

#include "cms_core.h"
#include "cms_fil.h"
#include "cms_util.h"
#include "rcl.h"
#include "rut_util.h"
#include "qdm_modsw_ee.h"

/*!\file stl_modsw.c
 * \brief This file contains generic modular sw functions (not specific to
 *        any specific execution env.)
 *
 */

#ifdef SUPPORT_BEEP
#define DU_TMP_FILE "/var/tmp_du"

static CmsRet getDiskSpaceInUse(const char *directory, SINT32 *diskSpaceInUse);
static CmsRet getDiskSpaceInUseByEu(_EUObject *obj, SINT32 *diskSpaceInUse);
static CmsRet getMemoryInUseByEu(_EUObject *obj, SINT32 *memoryInUse);
static CmsRet getBeepPkgDirectoryByEuFullPathLocked(const char *euFullPath,
                                    char *pkgDirectory, UINT32 pkgDirectoryLen);

#ifdef DMP_DEVICE2_PROCESSSTATUS_1
static UINT32 getParentPid(UINT32 pid);
static CmsRet getAssociatedProcessList(_EUObject *obj);
#endif

#define SPD_LOOP_UPPER_PATH "/lxc/loop/upper/"
#define SPD_UPPER_PATH "/lxc/upper/"
#define SPD_ROOTFS_DU_DIR_NAME "/du/"

static CmsRet getDiskSpaceInUse(const char *directory, SINT32 *diskSpaceInUse)
{
   CmsRet ret = CMSRET_SUCCESS;
   char   buf[CMS_MAX_FULLPATH_LENGTH+BUFLEN_64]={0};
   FILE   *fp = NULL;
   
   *diskSpaceInUse = -1;

#if 0
   sprintf(buf, "du -k -s %s > %s", directory, DU_TMP_FILE);
   rut_doSystemAction("getDiskSpaceInUse", buf);

   if ((fp = fopen(DU_TMP_FILE, "r")) == NULL)
   {
      cmsLog_error("fopen %s failed", DU_TMP_FILE);
      return CMSRET_INTERNAL_ERROR;
   }

   if (fgets(buf, sizeof(buf), fp) == NULL)
   {
      cmsLog_notice("fgets error from %s", DU_TMP_FILE);
      ret = CMSRET_INTERNAL_ERROR;
   }
   else
   {
      char   *ptr;
      SINT32 inUse;

      /* strip eol character */
      ptr = strchr(buf, 0xa);
      if (ptr)
      {
         *ptr = '\0';
      }

      /* get the size of disk space in used in KB */ 
      if ((inUse = (SINT32)strtol(buf, NULL, 10)))
      {
         *diskSpaceInUse = inUse;
      }
   }

   fclose(fp);
   sprintf(buf, "rm %s", DU_TMP_FILE);
   rut_doSystemAction("getDiskSpaceInUse", buf);
#endif

   return ret;

}  /* End of getDiskSpaceInUse() */


CmsRet getBeepPkgDirectoryByEuFullPathLocked(const char *euFullPath,
                                     char *pkgDirectory, UINT32 pkgDirectoryLen)
{
   CmsRet ret;
   char duid[BUFLEN_64]={0};

   cmsLog_debug("Enter.  euFullPath %s", euFullPath);

   if ((ret = qdmModsw_getDuidByEuFullPathLocked(euFullPath,  duid, sizeof(duid)-1)) == CMSRET_SUCCESS)
   {
      char duInstString[BUFLEN_8]={0};
      char duNameFromDuid[BUFLEN_32]={0};

      /* need to have this format, duName_DUinstanceNumber (cwmp-1)  for the new directory */
      if ((ret = qdmModsw_getDuNameDuInstanceFromDuid(duid,
                                                      duNameFromDuid,
                                                      sizeof(duNameFromDuid)-1,
                                                      duInstString,
                                                      sizeof(duInstString)-1)) == CMSRET_SUCCESS)
      {
         char eeDirName[CMS_MAX_FULLPATH_LENGTH]={0};   /* eg. ../local/modsw/tr157du/BEE */
         int flash;

         if ((ret = qdmModsw_getEeDirByEuFullPathLocked(euFullPath, &flash, eeDirName, sizeof(eeDirName)-1)) == CMSRET_SUCCESS)
         {
            char eeDuDir[CMS_MAX_FULLPATH_LENGTH]={0};

            /* 
             * FIXME: Should include modsw_container library when it is 
             * moved out of modsw library
             */
            snprintf(eeDuDir, sizeof(eeDuDir)-1, "%s/%s/%s",
               eeDirName, (flash>0)?SPD_LOOP_UPPER_PATH : SPD_UPPER_PATH,
               SPD_ROOTFS_DU_DIR_NAME);
            snprintf(pkgDirectory, pkgDirectoryLen, "%s/%s-%s", eeDuDir, duNameFromDuid, duInstString);
         }
         else
         {
            cmsLog_error("Failed to get EE directory by euFullPath %s", euFullPath);
            ret = CMSRET_INVALID_ARGUMENTS;
         }
      }
      else
      {
         cmsLog_error("duid has no instance id.");
         ret = CMSRET_INVALID_ARGUMENTS;
      }
   }
   else
   {
      cmsLog_error("Failed to get duid from %s", euFullPath);
      ret = CMSRET_INVALID_ARGUMENTS;
   }

   cmsLog_debug("pkgDirectory %s.  ret %d", pkgDirectory, ret);

   return ret;
}


static CmsRet getDiskSpaceInUseByEu(_EUObject *obj, SINT32 *diskSpaceInUse)
{
   CmsRet ret = CMSRET_SUCCESS;
   char   euFullPath[MDM_SINGLE_FULLPATH_BUFLEN]={0};
   char   pkgDirName[CMS_MAX_FULLPATH_LENGTH]={0};
   char   directory[CMS_MAX_FULLPATH_LENGTH+BUFLEN_32]={0};  
   SINT32 inUse = -1;
   SINT32 inUseByRootfs = -1;
   
   *diskSpaceInUse = obj->diskSpaceInUse;

   ret = qdmModsw_getExecUnitFullPathByEuidLocked(obj->EUID, euFullPath,
                                                  sizeof(euFullPath));
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("qdmModsw_getExecUnitFullPathByEuidLocked failed, ret=%d", ret);
      return ret;
   }

   ret = getBeepPkgDirectoryByEuFullPathLocked(euFullPath, pkgDirName,
                                               sizeof(pkgDirName));
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("getBeepPkgDirectoryByEuFullPathLocked failed, ret=%d", ret);
      return ret;
   }

   snprintf(directory, sizeof(directory),
            "%s/app_%s/lxc/loop/upper", pkgDirName, obj->name);

   /* check if directory exists or not */
   if (cmsFil_isDirPresent(directory))
   {
      if ((ret = getDiskSpaceInUse(directory, &inUse)) == CMSRET_SUCCESS)
      {
//         cmsLog_error("directory<%s> inUse<%d>", directory, inUse);
      }
   }
   else
   {
      snprintf(directory, sizeof(directory),
               "%s/app_%s/lxc/upper", pkgDirName, obj->name);
      if ((ret = getDiskSpaceInUse(directory, &inUse)) == CMSRET_SUCCESS)
      {
//         cmsLog_error("directory<%s> inUse<%d>", directory, inUse);
      }
   }

   if (ret == CMSRET_SUCCESS)
   {
      if ((inUse >= 0) && (inUseByRootfs >= 0) && (inUse >= inUseByRootfs))
      {
         *diskSpaceInUse = inUse - inUseByRootfs;
      }
   }

out:
   return ret;

}  /* End of getDiskSpaceInUseByEu() */


static CmsRet getMemoryInUseByEu(_EUObject *obj, SINT32 *memoryInUse)
{
   CmsRet ret = CMSRET_SUCCESS;
   char   buf[BUFLEN_128]={0};
   FILE   *fp = NULL;

   *memoryInUse = obj->memoryInUse;

   /* open cgroup memory usage file */
   sprintf(buf, "/sys/fs/cgroup/memory/lxc/%s/memory.usage_in_bytes", obj->EUID);

   if ((fp = fopen(buf, "r")) == NULL)
   {
      cmsLog_notice("fopen %s failed", buf);
      *memoryInUse = -1;
      return CMSRET_SUCCESS;
   }

   if (fgets(buf, sizeof(buf), fp) == NULL)
   {
      snprintf(buf, sizeof(buf), "/sys/fs/cgroup/memory/lxc/%s/memory.usage_in_bytes",
               obj->EUID);
      cmsLog_error("fgets error from %s", buf);
      *memoryInUse = -1;
      ret = CMSRET_INTERNAL_ERROR;
   }
   else
   {
      char *ptr;
      SINT32 inUse;

      /* strip eol character */
      ptr = strchr(buf, 0xa);
      if (ptr)
      {
         *ptr = '\0';
      }

      /* get the size of memory in used in KB */ 
      if ((inUse = (SINT32)strtol(buf, NULL, 10)))
      {
         *memoryInUse = inUse / 1024;
      }
      else
      {
         *memoryInUse = -1;
      }
   }

   fclose(fp);
   return ret;

}  /* End of getMemoryInUseByEu() */


#ifdef DMP_DEVICE2_PROCESSSTATUS_1
static UINT32 getParentPid(UINT32 pid)
{
   UINT32   ppid = 0;
   char     *ptr;
   char     buf[81];
   FILE     *fp = NULL;
   
   /* open /proc/pid/status file */
   snprintf(buf, sizeof(buf), "/proc/%d/status", pid);
     
   if ((fp = fopen(buf, "r")) == NULL)
   {
      cmsLog_notice("fopen %s failed", buf);
      return ppid;
   }

   while (fgets(buf, sizeof(buf), fp))
   {
      /* strip eol character */
      ptr = strchr(buf, 0xa);
      if (ptr)
      {
         *ptr = '\0';
      }

      if ((ptr = strstr(buf, "PPid:")))
      {
         ptr += sizeof("PPid:");
         ppid = strtol(ptr, NULL, 10);
         break;
      }
   }

   fclose(fp);
   return ppid;

}  /* End of getParentPid() */


static CmsRet getAssociatedProcessList(_EUObject *obj)
{
   CmsRet ret = CMSRET_SUCCESS;
   MdmPathDescriptor pathDesc;
   InstanceIdStack   iidStack, iidStackSave;
   Dev2ProcessStatusObject       *processStatusObj      = NULL;
   Dev2ProcessStatusEntryObject  *processStatusEntryObj = NULL;
   char   *buf, *fullStr, *ptr;
   UINT32 size;
   UINT32 pid = 0, ppid = 0;
   UBOOL8 found = FALSE;      

   /* get DeviceInfo.ProcessStatus object to update the
    * DeviceInfo.ProcessStatus.Process.{i}. table
    */
   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_get(MDMOID_DEV2_PROCESS_STATUS, &iidStack, 0,
                         (void **)&processStatusObj)) == CMSRET_SUCCESS)
   {
      cmsObj_free((void **)&processStatusObj);
   }
   else
   {
      cmsLog_error("cmsObj_get MDMOID_DEV2_PROCESS_STATUS failed, ret=%d", ret);
      return ret;
   }

   /* first find the application process, then its parents if any */
   iidStackSave = iidStack;
   while ((ret = cmsObj_getNext(MDMOID_DEV2_PROCESS_STATUS_ENTRY, &iidStack,
                        (void **)&processStatusEntryObj)) == CMSRET_SUCCESS)
   {
      if (!found)
      {
         if (processStatusEntryObj->command)
         {
            if ((ptr = strrchr(processStatusEntryObj->command, '/')))
               ptr++;
            else
               ptr = processStatusEntryObj->command;

            if (cmsUtl_strcmp(obj->name, ptr) == 0)
            {
               /* found the application process */
               found = TRUE;
               /* free the associatedProcessList before update */
               CMSMEM_FREE_BUF_AND_NULL_PTR(obj->associatedProcessList);
            }
         }

         if (!found)
         {
            cmsObj_free((void **)&processStatusEntryObj);
            continue;
         }
      }
      else
      {
         /* the application process had been found. Now we look
          * for its parent processes.
          */
         if (processStatusEntryObj->PID != ppid)
         {
            cmsObj_free((void **)&processStatusEntryObj);
            continue;
         }
      }

      pid = processStatusEntryObj->PID;
      cmsObj_free((void **)&processStatusEntryObj);

      memset(&pathDesc, 0, sizeof(MdmPathDescriptor));
      pathDesc.oid      = MDMOID_DEV2_PROCESS_STATUS_ENTRY;
      pathDesc.iidStack = iidStack;
      fullStr = NULL;
      cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &fullStr);

      /* allocate buffer for the associatedProcessList */
      size = cmsUtl_strlen(obj->associatedProcessList) +
             cmsUtl_strlen(fullStr) +
             2;  /* plus comma and null terminator */

      if ((buf = cmsMem_alloc(size, ALLOC_ZEROIZE)) == NULL)
      {
         CMSMEM_FREE_BUF_AND_NULL_PTR(fullStr);
         cmsLog_error("cmsMem_alloc failed. size=%d", size);
         ret = CMSRET_RESOURCE_EXCEEDED;
         break;
      }
      
      if (obj->associatedProcessList == NULL)
         sprintf(buf, "%s", fullStr);
      else
         sprintf(buf, "%s,%s", fullStr, obj->associatedProcessList);

      CMSMEM_REPLACE_STRING_FLAGS(obj->associatedProcessList, buf, ALLOC_SHARED_MEM);
      CMSMEM_FREE_BUF_AND_NULL_PTR(fullStr);
      CMSMEM_FREE_BUF_AND_NULL_PTR(buf);

      /* Look for the parent processes */
      if ((ppid = getParentPid(pid)) > 1)
         iidStack = iidStackSave;
      else
         break;   /* done */
   }

   if (ret == CMSRET_NO_MORE_INSTANCES)
      ret = CMSRET_SUCCESS;
   return ret;

}  /* End of getAssociatedProcessList() */
#endif
#endif   /* SUPPORT_BEEP */


CmsRet stl_swModulesObject(_SwModulesObject *obj __attribute__((unused)),
                  const InstanceIdStack *iidStack __attribute__((unused)))
{
   return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}


CmsRet stl_execEnvObject(_ExecEnvObject *obj __attribute__((unused)),
                  const InstanceIdStack *iidStack __attribute__((unused)))
{
   /* see comments in stl_dUObject */
   return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}


CmsRet stl_dUObject(_DUObject *obj __attribute__((unused)),
                  const InstanceIdStack *iidStack __attribute__((unused)))
{
/*
 * In the "classical" architecture of CMS, when the STL handler function is
 * called, the function will do an ioctl or send a message to a daemon to
 * get the latest up-to-date info for this object.  However, in the case
 * of modular software, osgid and linmosd will always update the DUstatus
 * object with the latest info, so the data in this object is always
 * up-to-date.  So no need to do anything in this function.
 */
   return (CMSRET_SUCCESS_OBJECT_UNCHANGED);
}


#ifdef SUPPORT_BEEP
CmsRet stl_eUObject(_EUObject *obj,
                    const InstanceIdStack *iidStack __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS_OBJECT_UNCHANGED;
   SINT32 diskSpaceInUse = -1;
   SINT32 memoryInUse    = -1;

   if (obj->name == NULL || obj->name[0] == '\0')
   {
      return CMSRET_SUCCESS_OBJECT_UNCHANGED;
   }

   getDiskSpaceInUseByEu(obj, &diskSpaceInUse);
   if (diskSpaceInUse != obj->diskSpaceInUse)
   {
      obj->diskSpaceInUse = diskSpaceInUse;
      ret = CMSRET_SUCCESS;
   }

   if (cmsUtl_strcmp(obj->status, MDMVS_ACTIVE) != 0)
   {
      if (obj->memoryInUse != -1)
      {
         obj->memoryInUse = -1;
         ret = CMSRET_SUCCESS;
      }
#ifdef DMP_DEVICE2_PROCESSSTATUS_1
      /* free the associatedProcessList */
      if (obj->associatedProcessList)
      {
         CMSMEM_FREE_BUF_AND_NULL_PTR(obj->associatedProcessList);
         ret = CMSRET_SUCCESS;
      }
#endif
      return ret;
   }

   getMemoryInUseByEu(obj, &memoryInUse);
   if (memoryInUse != obj->memoryInUse)
   {
      obj->memoryInUse = memoryInUse;
      ret = CMSRET_SUCCESS;
   }
   
#ifdef DMP_DEVICE2_PROCESSSTATUS_1
   {
      CmsRet ret1;

      ret1 = getAssociatedProcessList(obj);
      if (ret1 != CMSRET_SUCCESS_OBJECT_UNCHANGED)
      {
         ret = ret1;
      }
   }   
#endif
   return ret;

}  /* End of stl_eUObject() */
#else
CmsRet stl_eUObject(_EUObject *obj __attribute__((unused)),
                    const InstanceIdStack *iidStack __attribute__((unused)))
{
   /* see comments in stl_dUObject */
   return (CMSRET_SUCCESS_OBJECT_UNCHANGED);
}
#endif

CmsRet stl_extensionsObject(_ExtensionsObject *obj __attribute__((unused)),
                    const InstanceIdStack *iidStack __attribute__((unused)))
{
   return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}


CmsRet stl_busObject(_BusObject *obj __attribute__((unused)),
                     const InstanceIdStack *iidStack __attribute__((unused)))
{
   return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}


CmsRet stl_busObjectPathObject(_BusObjectPathObject *obj __attribute__((unused)),
                               const InstanceIdStack *iidStack __attribute__((unused)))
{
   return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}


CmsRet stl_busInterfaceObject(_BusInterfaceObject *obj __attribute__((unused)),
                              const InstanceIdStack *iidStack __attribute__((unused)))
{
   return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}


CmsRet stl_busMethodObject(_BusMethodObject *obj __attribute__((unused)),
                           const InstanceIdStack *iidStack __attribute__((unused)))
{
   return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}


CmsRet stl_busSignalObject(_BusSignalObject *obj __attribute__((unused)),
                           const InstanceIdStack *iidStack __attribute__((unused)))
{
   return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}


CmsRet stl_busPropertyObject(_BusPropertyObject *obj __attribute__((unused)),
                             const InstanceIdStack *iidStack __attribute__((unused)))
{
   return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}


CmsRet stl_busClientObject(_BusClientObject *obj __attribute__((unused)),
                           const InstanceIdStack *iidStack __attribute__((unused)))
{
   return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}


CmsRet stl_busClientPrivilegeObject(_BusClientPrivilegeObject *obj __attribute__((unused)),
                                    const InstanceIdStack *iidStack __attribute__((unused)))
{
   return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}


CmsRet stl_manifestObject(_ManifestObject *obj __attribute__((unused)),
                          const InstanceIdStack *iidStack __attribute__((unused)))
{
   return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}


CmsRet stl_dmAccessObject(_DmAccessObject *obj, const InstanceIdStack *iidStack)
{
   return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}


CmsRet stl_sdmAgentObject(_SdmAgentObject *obj, const InstanceIdStack *iidStack)
{
   return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}


CmsRet stl_sdmObject(_SdmObject *obj, const InstanceIdStack *iidStack)
{
   return CMSRET_SUCCESS_OBJECT_UNCHANGED;
}
#endif /* DMP_DEVICE2_SM_BASELINE_1 */

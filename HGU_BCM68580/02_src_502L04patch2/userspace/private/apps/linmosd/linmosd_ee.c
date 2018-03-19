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

#include <errno.h>

#include "cms.h"
#include "cms_params_modsw.h"
#include "cms_mdm.h"
#include "cms_obj.h"
#include "cms_lck.h"
#include "cms_qdm.h"
#include "cms_util.h"
#include "cms_msg.h"
#include "cms_msg_modsw.h"
#include "modsw.h"

#include "linmosd_private.h"


/*!\file linmosd_ee.c
 * \brief This file contains code specific to Linux Execution Environments.
 *        It handles both Linux Execution Environments (EE) and Linux Primary
 *        Firmware Patchs (PFP)
 *
 */

static CmsRet installDu(DUrequestStateChangedMsgBody *duChangeMsg,
                        CmsImageFormat *imageFormat,
                        char *header2, char *duVersion, char *duDesc,
                        char *innerName, char *euVersion, char *euDesc)
{
   char tmpFile[CMS_MAX_FULLPATH_LENGTH]={0};
   char destDir[CMS_MAX_FULLPATH_LENGTH]={0};
   char duid[BUFLEN_64]={0};
   char newDuid[BUFLEN_64]={0};
   char *namePtr;
   int rc;
   CmsRet ret;

   ret = cmsUtl_getRunTimePath(CMS_MODSW_TMP_DIR, destDir, sizeof(destDir));
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("path is too long for destDir (len=%d)", sizeof(destDir));
      return ret;
   }

   namePtr = strrchr(duChangeMsg->URL, '/');
   if (namePtr == NULL)
   {
      cmsLog_error("Could not find name in URL %s", duChangeMsg->URL);
      return ret;
   }
   namePtr = namePtr + 1;  /* move past the / */

   rc = snprintf(tmpFile, sizeof(tmpFile), "%s/%s",
                                            destDir, namePtr);
   if (rc >= (int) sizeof(tmpFile))
   {
      cmsLog_error("path is too long for tmpFile (len=%d)", sizeof(tmpFile));
      return ret;
   }

   /* Add DU object in MDM */
   sprintf(duid, "%s::{i}", LINUXEE_NAME);
   ret = modsw_addDuEntry(duChangeMsg->UUID, duChangeMsg->version, duid, duChangeMsg->URL, 
                          duChangeMsg->execEnvFullPath, newDuid, sizeof(newDuid)-1);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not add new DU entry, ret=%d", ret);
      return ret;
   }

   modsw_setDuStatus(duChangeMsg->UUID, duChangeMsg->version, newDuid, MDMVS_X_BROADCOM_COM_DOWNLOADING);

   cmsLog_debug("starting download from URL=%s destDir=%s",
                duChangeMsg->URL, destDir);

   /* Currently, this is a synchronous interface, meaning we block here
    * until the entire download is complete.  In the future, it would be
    * better if this was an async interface.
    */
   ret = modswDld_startDownload(duChangeMsg->URL,
         duChangeMsg->username,
         duChangeMsg->password,
         destDir);
   if (ret != CMSRET_SUCCESS)
   {
      /* set DU status to download failed, let ACS delete the entry */
      modsw_setDuStatus(duChangeMsg->UUID, duChangeMsg->version, newDuid, MDMVS_X_BROADCOM_COM_DNLDFAILED);
   }
   else
   {
      cmsLog_debug("calling modsw_unpackLinuxPkg with tmpFile=%s", tmpFile);
      ret = modsw_unpackLinuxPkg(tmpFile, imageFormat,
                                 header2, duVersion, duDesc,
                                 innerName, euVersion, euDesc);
      if (ret == CMSRET_SUCCESS)
      {
         cmsLog_debug("package successfully installed!");
         modsw_setDuStatus(duChangeMsg->UUID, newDuid, duChangeMsg->version, MDMVS_INSTALLED);
         /* our install process also does the resolve, so if successful, then also resolved */
         modsw_setDuResolved(duChangeMsg->UUID, duChangeMsg->version, TRUE);
      }
      else
      {
         cmsLog_error("package failed to install, ret=%d", ret);
         if (ret == CMSRET_FILE_TRANSFER_FILE_CORRUPTED)
         {
            modsw_setDuStatus(duChangeMsg->UUID, duChangeMsg->version, newDuid, MDMVS_X_BROADCOM_COM_DNLDCORRUPTED);
         }
         else
         {
            modsw_setDuStatus(duChangeMsg->UUID, duChangeMsg->version, newDuid, MDMVS_X_BROADCOM_COM_DNLDBADFORMAT);
         }
      }
   }

   cmsLog_debug("ret=%d unlink(%s)", ret, tmpFile);
   if (unlink(tmpFile))
   {
      cmsLog_error("unlink of %s failed, errno=%d", tmpFile, errno);
   }

   return ret;
}


static UBOOL8 executeScript(const char *duName, const char *euName, const char *script)
{
   char baseDir[CMS_MAX_FULLPATH_LENGTH];
   char fullPath[CMS_MAX_FULLPATH_LENGTH];
   int rc;

   cmsUtl_getRunTimeRootDir(baseDir, sizeof(baseDir));

   rc = snprintf(fullPath, sizeof(fullPath), "%s%s/%s/%s/%s",
                baseDir, CMS_MODSW_LINUXEE_DU_DIR, duName, euName, script);
   if (rc >= (int) sizeof(fullPath))
   {
      cmsLog_error("script file pathlen greater than buflen=%d", sizeof(fullPath));
      return FALSE;
   }

   cmsLog_debug("trying to execute=%s", fullPath);
   if (cmsFil_isFilePresent(fullPath))
   {
      rc = system(fullPath);
      if (rc < 0)
      {
         cmsLog_error("%s returned rc=%d", fullPath, rc);
         return FALSE;
      }
      else
      {
         return TRUE;
      }
   }
   else
   {
      cmsLog_error("%s not present", fullPath);
      return FALSE;
   }
}


static UBOOL8 resolveDu(const char *duName, const char *euName)
{
   return (executeScript(duName, euName, "resolve"));
}


static UBOOL8 startEuLocked(const char *duName, const char *euName,
                            const char *euFullPath)
{
   UBOOL8 success;

   modsw_setEuStatusLocked(euFullPath, MDMVS_STARTING);

   success = executeScript(duName, euName, "start");
   if (success)
   {
      cmsLog_debug("%s/%s started", duName, euName);
      modsw_setEuStatusLocked(euFullPath, MDMVS_ACTIVE);
   }
   else
   {
      cmsLog_error("%s/%s failed to start", duName, euName);
      modsw_setEuStatusLocked(euFullPath, MDMVS_IDLE);
   }

   return success;
}


void stopEuLocked(const char *duName, const char *euName,
                  const char *euFullPath)
{
   UBOOL8 success;

   modsw_setEuStatusLocked(euFullPath, MDMVS_STOPPING);

   success = executeScript(duName, euName, "stop");
   if (success)
   {
      cmsLog_debug("%s/%s stopped", duName, euName);
      modsw_setEuStatusLocked(euFullPath, MDMVS_IDLE);
   }
   else
   {
      cmsLog_error("%s/%s failed to stop", duName, euName);
      modsw_setEuStatusLocked(euFullPath, MDMVS_ACTIVE);
   }

   return;
}


void addEuNameAndDuLink(const char *uuid, const char *version,
              const char *euName, const char *euVersion, const char *euDesc)
{
   CmsRet ret;
   char euidBuf[BUFLEN_64]={0};
   char aliasBuf[BUFLEN_64]={0};
   char euFullPath[MDM_SINGLE_FULLPATH_BUFLEN]={0};
   char execEnvFullPath[MDM_SINGLE_FULLPATH_BUFLEN]={0};

   sprintf(euidBuf, "%s::%s::{i}", LINUXEE_NAME, euName);

   /* alias can be anything, but then ACS can change it later.
    * Use same format as euid for alias.
    */
   sprintf(aliasBuf, "%s::%s::{i}", LINUXEE_NAME, euName);

   if ((ret = cmsLck_acquireLockWithTimeout(LINMOSD_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
       cmsLog_error("failed to get lock, ret=%d", ret);
       cmsLck_dumpInfo();
       return;
   }

   /* execEnv which this EU belongs to */
   qdmModsw_getExecEnvFullPathByNameLocked(LINUXEE_NAME, execEnvFullPath, sizeof(execEnvFullPath));

   ret = modsw_addEuEntryLocked(euidBuf, aliasBuf, euName,
                                LINUXEE_NAME, execEnvFullPath,
                                 NULL, euVersion, euDesc, NULL, -1,
                                 euFullPath, sizeof(euFullPath));
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("modsw_addEuEntry ret=%d", ret);
   }
   else
   {
      modsw_addEuPathToDuLocked(uuid, version, euFullPath);
   }

   cmsLck_releaseLock();

   return;
}


static void uninstallDu(const char *uuid, const char *version)
{
   char euFullPath[MDM_SINGLE_FULLPATH_BUFLEN]={0};
   char duName[BUFLEN_64+1]={0};
   char euName[BUFLEN_32+1]={0};
   CmsRet ret;

   cmsLog_debug("uuid=%s version=%s", uuid, version);

   if ((ret = cmsLck_acquireLockWithTimeout(LINMOSD_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
       cmsLog_error("failed to get lock, ret=%d", ret);
       cmsLck_dumpInfo();
       return;
   }

   /* find DUobject and set state to Uninstalling */
   modsw_setDuStatusLocked(uuid, version, NULL, MDMVS_UNINSTALLING);

   modsw_getDuNameLocked(uuid, version, duName, sizeof(duName));

   /* get the ExecUnit ref and stop it */
   ret = modsw_getExcutionUnitListFromDuLocked(uuid, version, euFullPath, sizeof(euFullPath));
   if (ret != CMSRET_SUCCESS)
   {
      /*
       * euFullPath might be NULL if the download failed, so going into this
       * block is not bad enough to warrant a error message.
       */
      cmsLog_debug("Bad uuid %s version %s", uuid, version);
      /* keep going to delete the stuff on filesystem */
   }
   else
   {
      /* got a valid euFullPath */
      /* XXX miwang: need to stop the EU first. hook to code once I implement
       * start/stop/autostart of EU.
       */

      /* get the EU name.  Need it later for file system delete */
      modsw_getEuNameLocked(euFullPath, euName, sizeof(euName));

      /* delete exec unit ref */
      modsw_deleteEuEntryLocked(euFullPath);
   }

   /* delete DUobject */
   modsw_deleteDuEntryLocked(uuid, version);

   cmsLck_releaseLock();

   /* do the filesystem operations */
   if (!IS_EMPTY_STRING(duName))
   {
      char destDir[CMS_MAX_FULLPATH_LENGTH];
      CmsRet ret;

      ret = cmsUtl_getRunTimePath(CMS_MODSW_LINUXEE_DU_DIR, destDir, sizeof(destDir));
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("path is too long for destDir (len=%d)", sizeof(destDir));
         return;
      }

      cmsUtl_strncat(destDir, sizeof(destDir), "/");
      ret = cmsUtl_strncat(destDir, sizeof(destDir), duName);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("path is too long for destDir (len=%d)", sizeof(destDir));
      }

      modsw_deleteLinuxPkg(destDir, euName);
   }
}


CmsRet processDuStateChange(CmsMsgHeader *msg)
{
   CmsRet ret=CMSRET_SUCCESS;
   DUrequestStateChangedMsgBody *duChangeMsg = (DUrequestStateChangedMsgBody *) (msg+1);
   char header2[CMS_MODSW_LINUX_HEADER2_LEN+1]={0};
   char duVersion[TR157_VERSION_STR_LEN+1]={0};
   char duDesc[TR157_DESCRIPTION_STR_LEN+1]={0};
   char innerName[BUFLEN_32+1]={0};
   char euVersion[TR157_VERSION_STR_LEN+1]={0};
   char euDesc[TR157_DESCRIPTION_STR_LEN+1]={0};
   CmsImageFormat imageFormat=CMS_IMAGE_FORMAT_INVALID;

   cmsLog_debug("operation=%s", duChangeMsg->operation);
   cmsLog_debug("UUID     =%s", duChangeMsg->UUID);
   cmsLog_debug("URL      =%s", duChangeMsg->URL);
   cmsLog_debug("version  =%s", duChangeMsg->version);
   cmsLog_debug("username =%s", duChangeMsg->username);
   cmsLog_debug("password =%s", duChangeMsg->password);
   cmsLog_debug("exec env =%s", duChangeMsg->execEnvFullPath);

   if (!cmsUtl_strcmp(duChangeMsg->operation,SW_MODULES_OPERATION_INSTALL))
   {
      createDuResponse(msg, duChangeMsg->operation, duChangeMsg->UUID,
                       duChangeMsg->reqId);

      ret = installDu(duChangeMsg, &imageFormat,
            header2, duVersion, duDesc,
            innerName, euVersion, euDesc);

      if (imageFormat != CMS_IMAGE_FORMAT_MODSW_LINUXEE_DU)
      {
         cmsLog_error("Unexpected image format! got %d expected %d",
                      imageFormat, CMS_IMAGE_FORMAT_MODSW_LINUXEE_DU);
         ret = CMSRET_INVALID_ARGUMENTS;
      }

      if (header2[0] != '\0')
      {
         char duAliasBuf[BUFLEN_64];
         sprintf(duAliasBuf, "%s::{i}", LINUXEE_NAME);

         /* regardless of the result of the install, if we managed to get
          * header2, populate the DU name and alias fields.
          */
         modsw_setDuInfo(duChangeMsg->UUID, duChangeMsg->version,
                         header2, NULL, duAliasBuf, duVersion, duDesc, NULL);
      }

      if (ret == CMSRET_SUCCESS)
      {
         /* for the addEuName and resolveDU, do not change the overall
          * return value.  Any errors encountered in these two steps will
          * be reflected in the data model anyways.
          */
         addEuNameAndDuLink(duChangeMsg->UUID, duChangeMsg->version,
                            innerName, euVersion, euDesc);

         if (resolveDu(header2, innerName))
         {
            cmsLog_debug("Setting DU %s resolved to TRUE", duChangeMsg->UUID);
            modsw_setDuResolved(duChangeMsg->UUID, duChangeMsg->version, TRUE);
         }
      }

      /* since download and install are synchronous operations right now,
       * we can send the response here.  Later, when download is an async
       * operation, these two calls must be moved to where download and install
       * are completed.
       */
      populateDuResponse(duChangeMsg->UUID);
      sendDuResponseByUuid(duChangeMsg->UUID, ret);

      saveConfigNeeded=TRUE;
   }
   else if (!cmsUtl_strcmp(duChangeMsg->operation, SW_MODULES_OPERATION_UNINSTALL))
   {
      createDuResponse(msg, duChangeMsg->operation, duChangeMsg->UUID,
                       duChangeMsg->reqId);

      /* populate the fields before we delete the DU */
      populateDuResponse(duChangeMsg->UUID);

      uninstallDu(duChangeMsg->UUID, duChangeMsg->version);

      sendDuResponseByUuid(duChangeMsg->UUID, CMSRET_SUCCESS);

      saveConfigNeeded = TRUE;
   }
   else
   {
      cmsLog_error("operation %s not supported yet", duChangeMsg->operation);
   }

   return ret;
}


CmsRet processEuStateChange(CmsMsgHeader *msg)
{
   char duName[BUFLEN_32]={0};
   char euFullPath[MDM_SINGLE_FULLPATH_BUFLEN]={0};
   CmsRet ret=CMSRET_SUCCESS;
   EUrequestStateChangedMsgBody *euChangeMsg = (EUrequestStateChangedMsgBody *) (msg+1);

   cmsLog_debug("operation=%s", euChangeMsg->operation);
   cmsLog_debug("name     =%s", euChangeMsg->name);
   cmsLog_debug("euid     =%s", euChangeMsg->euid);

   if ((ret = cmsLck_acquireLockWithTimeout(LINMOSD_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
       cmsLog_error("failed to get lock, ret=%d", ret);
       cmsLck_dumpInfo();
       return ret;
   }

   ret = qdmModsw_getExecUnitFullPathByEuidLocked(euChangeMsg->euid,
                                            euFullPath, sizeof(euFullPath));
   if (ret == CMSRET_SUCCESS)
   {
      ret = qdmModsw_getDeployUnitNameByEuFullPathLocked(euFullPath,
                                                       duName, sizeof(duName));
      if (ret == CMSRET_SUCCESS)
      {
         if (!cmsUtl_strcmp(euChangeMsg->operation, SW_MODULES_OPERATION_START))
         {
            startEuLocked(duName, euChangeMsg->name, euFullPath);
         }
         else if (!cmsUtl_strcmp(euChangeMsg->operation, SW_MODULES_OPERATION_STOP))
         {
            stopEuLocked(duName, euChangeMsg->name, euFullPath);
         }
         else
         {
            cmsLog_error("Invalid operation %s", euChangeMsg->operation);
            ret = CMSRET_INVALID_ARGUMENTS;
         }
      }
      else
      {
         cmsLog_error("Could not find parent DeployUnit for EUID %s", euChangeMsg->euid);
      }
   }
   else
   {
      cmsLog_error("Could not get fullpath to EUID %s", euChangeMsg->euid);
   }

   cmsLck_releaseLock();

   return ret;
}


void startExecEnv()
{
   UBOOL8 resolved;
   DUObject *duObj;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsEntityId mngrEid;
   CmsRet ret;

   /* No action needed to start the Linux Execution Environment (the
    * Linux Execution enviornment is the Linux that the whole system is
    * running on.)
    */


   if ((ret = cmsLck_acquireLockWithTimeout(LINMOSD_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
       cmsLog_error("failed to get lock, ret=%d", ret);
       cmsLck_dumpInfo();
       return;
   }

   modsw_setExecEnvStatusLocked(LINUXEE_NAME, MDMVS_X_BROADCOM_COM_STARTING);

   /* re-resolve all Linux Deployment Units and auto-start the
    * execution units that have auto-start enabled.
    */
   while ((ret = cmsObj_getNext(MDMOID_DU, &iidStack, (void **)&duObj)) == CMSRET_SUCCESS)
   {
      cmsLog_debug("got ExecEnvRef=%s", duObj->executionEnvRef);

      if (qdmModsw_getMngrEidByExecEnvFullPathLocked(duObj->executionEnvRef,
                                     &mngrEid) == CMSRET_SUCCESS)
      {
         if (mngrEid == EID_LINMOSD)
         {
            char euFullPath[BUFLEN_256]={0};
            char euName[BUFLEN_32]={0};
            CmsRet r2;

            r2 = modsw_getExcutionUnitListFromDuLocked(duObj->UUID, duObj->version, euFullPath, sizeof(euFullPath));
            if (r2 == CMSRET_SUCCESS)
            {
               modsw_getEuNameLocked(euFullPath, euName, sizeof(euName));

               cmsLog_debug("resolving %s %s", duObj->name, euName);
               resolved = resolveDu(duObj->name, euName);
               if (resolved)
               {
                  modsw_setDuResolvedLocked(duObj->UUID, duObj->version, resolved);

                  if (modsw_isEuAutoStartLocked(euFullPath))
                  {
                     printf("linmosd: Exec Env starting, autostarting %s...", euName);
                     startEuLocked(duObj->name, euName, euFullPath);
                  }
               }
               else
               {
                  cmsLog_error("Failed to resolve DU %s", duObj->name);
               }
            }
         }
      }
      else
      {
         cmsLog_error("Bad ExecEnvRef %s", duObj->executionEnvRef);
      }

      cmsObj_free((void **)&duObj);
   }

   modsw_setExecEnvStatusLocked(LINUXEE_NAME, MDMVS_UP);

   cmsLck_releaseLock();

   return;
}


void stopExecEnv()
{
   DUObject *duObj;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsEntityId mngrEid;
   CmsRet ret;

   if ((ret = cmsLck_acquireLockWithTimeout(LINMOSD_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
       cmsLog_error("failed to get lock, ret=%d", ret);
       cmsLck_dumpInfo();
       return;
   }

   modsw_setExecEnvStatusLocked(LINUXEE_NAME, MDMVS_X_BROADCOM_COM_STOPPING);

   /* stop all execution units */
   while ((ret = cmsObj_getNext(MDMOID_DU, &iidStack, (void **)&duObj)) == CMSRET_SUCCESS)
   {
      cmsLog_debug("got ExecEnvRef=%s", duObj->executionEnvRef);

      if (qdmModsw_getMngrEidByExecEnvFullPathLocked(duObj->executionEnvRef,
                                     &mngrEid) == CMSRET_SUCCESS)
      {
         if (mngrEid == EID_LINMOSD)
         {
            char euFullPath[BUFLEN_256]={0};
            char euName[BUFLEN_32]={0};
            CmsRet r2;

            r2 = modsw_getExcutionUnitListFromDuLocked(duObj->UUID, duObj->version, euFullPath, sizeof(euFullPath));
            if (r2 == CMSRET_SUCCESS)
            {
               modsw_getEuNameLocked(euFullPath, euName, sizeof(euName));

               printf("linmosd: Exec Env is shutting down, stopping %s...", euName);
               stopEuLocked(duObj->name, euName, euFullPath);
            }
         }
      }
      else
      {
         cmsLog_error("Bad ExecEnvRef %s", duObj->executionEnvRef);
      }

      cmsObj_free((void **)&duObj);
   }

   /* no action needed to stop the execution enviornment itself, other than
    * cause linmosd to exit.
    */
   modsw_setExecEnvStatusLocked(LINUXEE_NAME, MDMVS_DISABLED);
   keepRunning = FALSE;

   cmsLck_releaseLock();

   return;
}


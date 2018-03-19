/***********************************************************************
 *
 *  Copyright (c) 2006-2007  Broadcom Corporation
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
#include "mdm.h"
#include "oal.h"
#include "cms_lck.h"
#include "prctl.h"


CmsRet cmsLck_acquireLockTraced(const char* callerFuncName)
{
   CmsRet ret;

   if (mdmLibCtx.locked)
   {
      cmsLog_error("already have lock! callerFuncName %s", callerFuncName);
      return CMSRET_WOULD_DEADLOCK;
   }

   if ((ret = MLOCK) == CMSRET_SUCCESS)
   {
      //save acquired timestamp and caller function name;
      cmsTms_get(&mdmShmCtx->timeAquired);
      strncpy(mdmShmCtx->callerFuncName, callerFuncName, sizeof(mdmShmCtx->callerFuncName)-1);
      cmsLog_debug("acquired lock. callerFuncName %s", callerFuncName);
   }

   return ret;
}

CmsRet cmsLck_acquireLockWithTimeoutTraced(const char* callerFuncName, UINT32 timeoutMilliSeconds)
{
   UINT32 to = timeoutMilliSeconds;
   CmsRet ret;

   if (mdmLibCtx.locked)
   {
      cmsLog_error("already have lock! callerFuncName %s", callerFuncName);
      return CMSRET_WOULD_DEADLOCK;
   }

   if ((ret = MLOCK_WITH_TIMEOUT(&to)) == CMSRET_SUCCESS)
   {
      //save acquired timestamp and caller function name;
      cmsTms_get(&mdmShmCtx->timeAquired);
      strncpy(mdmShmCtx->callerFuncName, callerFuncName, sizeof(mdmShmCtx->callerFuncName)-1);
      cmsLog_debug("acquired lock. callerFuncName %s; timeout %d milliseconds", callerFuncName, timeoutMilliSeconds);
   }

   return ret;
}

void cmsLck_releaseLockTraced(const char* callerFuncName)
{
   CmsTimestamp acquiredTimeStamp, releaseTimeStamp;
   char funcName[BUFLEN_64]={0};
   UINT32 timeHold = 0; 

   if (!mdmLibCtx.locked)
   {
      cmsLog_error("do not have lock! callerFuncName %s", callerFuncName);
      return;
   }

   /*
    * Grab the lock timestamp before releasing the lock, because once the
    * lock is released, other apps can set those variables.
    */
   cmsTms_get(&releaseTimeStamp);
   acquiredTimeStamp = mdmShmCtx->timeAquired;
   timeHold = cmsTms_deltaInMilliSeconds(&releaseTimeStamp, &acquiredTimeStamp);  
   strncpy(funcName, mdmShmCtx->callerFuncName, sizeof(funcName)-1);

   //wipe off callerFuncName and timeAquired information
   mdmShmCtx->timeAquired.nsec   = 0;
   mdmShmCtx->timeAquired.sec    = 0;
   mdmShmCtx->callerFuncName[0]  = 0;

   /* this should always work */
   MUNLOCK;

   /* do all this other stuff after we released the lock */
   if(timeHold > CMSLCK_MAX_HOLDTIME)
   {
      cmsLog_notice("released lock. lock occupied %d milliseconds; exceeds max timeHold %d;",
                         timeHold, CMSLCK_MAX_HOLDTIME);
      cmsLog_notice("acquiring lock callerFuncName %s; releasing lock callerFuncName %s;",
                         funcName, callerFuncName);
   }
   else
   {
      cmsLog_debug("lock hold time=%dms, acquiring lock callerFuncName %s; releasing lock callerFuncName %s;", timeHold, funcName, callerFuncName);
   }

   return;
}


void cmsLck_getInfo(CmsLckInfo *lockInfo)
{

   if (lockInfo == NULL)
   {
      cmsLog_error("lockInfo must not be NULL");
      return;
   }

   lockInfo->locked = mdmShmCtx->locked;
   lockInfo->funcCode = mdmShmCtx->lockFuncCode;
   lockInfo->oid = mdmShmCtx->oid;
   lockInfo->lockOwner = mdmShmCtx->lockOwner;
   lockInfo->timeAquired = mdmShmCtx->timeAquired;
   strcpy(lockInfo->callerFuncName, mdmShmCtx->callerFuncName);

   return;
}


void cmsLck_dumpInfo(void)
{
   CmsLckInfo info;
   UINT32 deltaMs;
   CmsTimestamp ts;
   char nameBuf[BUFLEN_32]={0};
   int rval;

   cmsLck_getInfo(&info);

   if (!info.locked)
   {
      printf("CMS MDM is unlocked.\n");
   }

   cmsTms_get(&ts);
   deltaMs = cmsTms_deltaInMilliSeconds(&ts, &info.timeAquired);

   rval = prctl_getNameByPid(info.lockOwner, nameBuf, sizeof(nameBuf));
   if (rval == 0)
   {
      printf("CMS MDM lock is held by %s (pid=%d) for %d seconds.\n",
            nameBuf, info.lockOwner, deltaMs/1000);
   }
   else
   {
      printf("CMS MDM lock is held by possible dead process (pid=%d) "
             "for %d seconds.\n",
             info.lockOwner, deltaMs/1000);
   }

   printf("Lock acquired in function %s\n", info.callerFuncName);
   if (info.oid != 0)
   {
      if (info.funcCode == (UINT8) 's')
      {
         printf("Currently in stl handler function for oid %d\n", info.oid);
      }
      else if (info.funcCode == (UINT8) 'r')
      {
         printf("Currently in rcl handler function for oid %d\n", info.oid);
      }
      else
      {
         printf("Currently in unknown function (0x%x) for oid %d\n",
               info.funcCode, info.oid);
      }
   }
}


CmsRet lck_checkBeforeEntry(const char *where, UINT32 maxEntries)
{
   CmsRet ret=CMSRET_SUCCESS;

   if (!mdmLibCtx.locked)
   {
      /* must have lock before calling CMS core functions */
      cmsLog_error("lock required during %s", where);
      return CMSRET_LOCK_REQUIRED;
   }
   
   if (mdmLibCtx.entryCount >= maxEntries)
   {
      cmsLog_error("%s cannot be called from RCL/STL", where);
      ret = CMSRET_INTERNAL_ERROR;  
   }
   else if (mdmLibCtx.entryCount == MDM_MAX_ENTRY_COUNT)
   {
      /* check for out-of-control recursions */
      cmsLog_error("entry count is %d, possible infinite recursion", mdmLibCtx.entryCount);
      ret = CMSRET_RECURSION_ERROR;
   }
   else
   {
      /* success, keep track of the number of times we entered */
      mdmLibCtx.entryCount++;
   }

   return ret;
}


void lck_trackExit(const char *where)
{
   if (mdmLibCtx.entryCount == 0)
   {
      /* we have no choice but to exit anyways */
      cmsLog_error("entry count is already 0 while exiting %s", where);
      cmsAst_assert(0);
   }
   else
   {
      mdmLibCtx.entryCount--;
   }
   
   return;
}




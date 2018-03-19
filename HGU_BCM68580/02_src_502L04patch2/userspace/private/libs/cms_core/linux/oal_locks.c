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
#include "../oal.h"
#include <errno.h>
#include <sys/sem.h>

#if defined(DESKTOP_LINUX) || defined(__ARM_EABI__) || defined(__LP64__)
/*
 * In EABI-ARM gcc compiler, it does not support __NR_ipc, so we have to make
 * an exception here.
 */
/*
 * On DESKTOP_LINUX, semtimedop is hidden under the symbol __USE_GNU.
 * Just define it here for symetry with what I have to do below.
 */
extern int semtimedop (int __semid, struct sembuf *__sops, size_t __nsops,
                       __const struct timespec *__timeout);
#else

/* 
 * On the modem, our kernel acutally supports semtimedop, but our uclibc does not.
 * So all I am doing below is providing the path from the user level to the kernel.
 */
#include <sys/syscall.h>
#include <unistd.h> 

#define IPCOP_semtimedop  4
int semtimedop(int semid, struct sembuf *sops, size_t nsops, const struct timespec *timeout)
{
   return syscall(__NR_ipc, IPCOP_semtimedop, semid, (int) nsops, 0, (unsigned long) sops, (unsigned long) timeout);
}

#endif /* DESKTOP_LINUX */


/** Linux kernel semaphore identifier. */
static SINT32 semid=-1;


/** Number of sempahores in the semphore array to create.
 * 
 * We only need 1.
 */
#define NUM_SEMAPHORES  1

/** Index into semaphore array.
 *
 * We created a semaphore array with only 1 element, so the index is always 0.
 */
static SINT32 semIndex=0;


CmsRet oalLck_init(UBOOL8 attachExisting)
{
   UINT32 flags;

   flags = (attachExisting) ? 0 : IPC_CREAT;

   if ((semid = semget(MDM_LOCK_SEMAPHORE_KEY, NUM_SEMAPHORES, flags|0666)) == -1)
   {
      cmsLog_error("semget failed, errno=%d", errno);
      return CMSRET_INTERNAL_ERROR;
   }

   mdmLibCtx.locked = FALSE;

   if (attachExisting)
   {
      cmsLog_notice("attach existing done, semid=%d", semid);
      return CMSRET_SUCCESS;
   }

   /*
    * We are creating new semaphore, so initialize semaphore to 0.
    */
   if(semctl(semid, semIndex, SETVAL, 0) == -1)
   {
      cmsLog_error("setctl setval 0 failed, errno=%d", errno);
      oalLck_cleanup();
      return CMSRET_INTERNAL_ERROR;
   }

   /* initialize system-wide lock debug tracking */
   mdmShmCtx->locked = FALSE;
   mdmShmCtx->lockOwner = CMS_INVALID_PID;
   mdmShmCtx->lockFuncCode = 0;
   mdmShmCtx->oid = 0;
   mdmShmCtx->timeAquired.nsec   = 0;
   mdmShmCtx->timeAquired.sec    = 0;
   memset(mdmShmCtx->callerFuncName, 0, sizeof(mdmShmCtx->callerFuncName));

   return CMSRET_SUCCESS;
}


void oalLck_cleanup(void)
{
   SINT32 rc;

   if ((mdmShmCtx->locked) || (mdmShmCtx->lockOwner != CMS_INVALID_PID))
   {
      cmsLog_error("lock is still held by %d, abort delete", mdmShmCtx->lockOwner);
      return;
   }


   if ((rc = semctl(semid, 0, IPC_RMID)) < 0)
   {
      cmsLog_error("IPC_RMID failed, errno=%d", errno);
   }
   else
   {
      cmsLog_notice("Semid %d deleted.", semid);
      semid = -1;
   }
}


CmsRet oal_lock(const UINT32 *timeoutMs)
{
   struct sembuf lockOp[2];
   SINT32 rc=-1;
   UINT32 timeRemainingMs=0;
   CmsTimestamp startTms, stopTms;
   CmsRet ret=CMSRET_SUCCESS;
   UBOOL8 extraDebug=FALSE;

   lockOp[0].sem_num = semIndex;
   lockOp[0].sem_op = 0; /* wait for zero: block until write count goes to 0. */
   lockOp[0].sem_flg = 0;

   lockOp[1].sem_num = semIndex;
   lockOp[1].sem_op = 1; /* incr sem count by 1 */
   lockOp[1].sem_flg = SEM_UNDO; /* automatically undo this op if process terminates. */

   if (mdmShmCtx->locked)
   {
      cmsLog_debug("lock currently held by pid=%d func=%s", mdmShmCtx->lockOwner, mdmShmCtx->callerFuncName);
      extraDebug = TRUE;
   }

   if (timeoutMs != NULL)
   {
      timeRemainingMs = *timeoutMs;
   }


   while (TRUE)
   {
      /*
       * If user specified a timeout, initialize pTimeout and pass it to semtimedop.
       * If fourth arg to semtimedop is NULL, then it blocks indefinately.
       */
      if (timeoutMs != NULL)
      {
         struct timespec timeout;

         cmsTms_get(&startTms);
         timeout.tv_sec = timeRemainingMs / MSECS_IN_SEC;
         timeout.tv_nsec = (timeRemainingMs % MSECS_IN_SEC) * NSECS_IN_MSEC;
         rc = semtimedop(semid, lockOp, sizeof(lockOp)/sizeof(struct sembuf), &timeout);
      }
      else
      {
         rc = semop(semid, lockOp, sizeof(lockOp)/sizeof(struct sembuf));
      }

      /*
       * Our new 2.6.21 MIPS kernel returns the errno in the rc, but my Fedora 7 
       * with 2.6.22 kernel still returns -1 and sets the errno.  So check for both.
       */
      if ((rc == -1 && errno == EINTR) ||
          (rc > 0 && rc == EINTR))
      {
         /*
          * Our semaphore operation was interrupted by a signal or something,
          * go back to the top of while loop and keep trying.
          * But if user has specified a timeout, we have to calculate how long
          * we have waited already, and how much longer we need to wait.
          */
         if (timeoutMs != NULL)
         {
            UINT32 elapsedMs;

            cmsTms_get(&stopTms);
            elapsedMs = cmsTms_deltaInMilliSeconds(&stopTms, &startTms);

            if (elapsedMs >= timeRemainingMs)
            {
               /* even though we woke up because of EINTR, we have waited long enough */
               rc = EAGAIN;
               break;
            }
            else
            {
               /* subtract the time we already waited and wait some more */
               timeRemainingMs -= elapsedMs;
            }
         }
      }
      else
      {
         /* If we get any error other than EINTR, break out of the loop */
         break;
      }
   }

   if (extraDebug)
   {
      cmsLog_debug("lock grab result, rc=%d errno=%d", rc, errno);
   }

   if (rc != 0)
   {
      /*
       * most likely cause of error is caught signal, we could also
       * get EIDRM if someone deletes the semphore while we are waiting
       * for it (that indicates programming error.)
       */
      if (errno == EINTR || rc == EINTR)
      {
         cmsLog_notice("lock interrupted by signal");
         ret = CMSRET_OP_INTR;
      }
      else if (errno == EAGAIN || rc == EAGAIN)
      {
         /* the new 2.6.21 kernel seems to return the errno in the rc */
         cmsLog_debug("timed out, errno=%d rc=%d", errno, rc);
         return CMSRET_TIMED_OUT;
      }
      else
      {
         cmsLog_error("lock failed, errno=%d rc=%d", errno, rc);
         ret = CMSRET_INTERNAL_ERROR;
      }
   }
   else
   {
      /* I got the lock! */

      /*
       * Because of the SEM_UNDO feature, when I acquire a lock,
       * if I notice that my mdmShmCtx does not have the same info,
       * then that means the previous owner died suddenly and did not clean up.
       * Update my mdmShmCtx structure to reflect reality.
       */
      if ((mdmShmCtx->locked) || (mdmShmCtx->lockOwner != CMS_INVALID_PID))
      {
         cmsLog_notice("correcting stale lock data from pid %d", mdmShmCtx->lockOwner);
         mdmShmCtx->locked = FALSE;
         mdmShmCtx->lockOwner = CMS_INVALID_PID;
      }

      /* update my lock tracking variables */
      mdmLibCtx.locked = TRUE;
      mdmShmCtx->locked = TRUE;
      mdmShmCtx->lockOwner = cmsEid_getPid();
   }

   return ret;
}


CmsRet oal_unlock()
{
   struct sembuf unlockOp[1];
   SINT32 semval;
   CmsRet ret=CMSRET_SUCCESS;

   unlockOp[0].sem_num = semIndex;
   unlockOp[0].sem_op = -1; /* decr sem count by 1 */
   unlockOp[0].sem_flg = SEM_UNDO; /* undo the undo state built up in the kernel during the lockOp */


   /* kernel should have semval of 1 */
   if ((semval = semctl(semid, semIndex, GETVAL, 0)) != 1)
   {
      cmsLog_error("kernel has semval=%d", semval);
      cmsAst_assert(0);
   }

   /* clear my lock tracking variables before the actual release*/
   mdmLibCtx.locked = FALSE;
   mdmShmCtx->locked = FALSE;
   mdmShmCtx->lockOwner = CMS_INVALID_PID;

   /* now do the actual release */
   if (semop(semid, unlockOp, sizeof(unlockOp)/sizeof(struct sembuf)) == -1)
   {
      cmsLog_error("release lock failed, errno=%d", errno);
      ret = CMSRET_INTERNAL_ERROR;
   }

   return ret;
}

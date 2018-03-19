/***********************************************************************
 *
 * <:copyright-BRCM:2006:DUAL/GPL:standard
 * 
 *    Copyright (c) 2006 Broadcom 
 *    All Rights Reserved
 * 
 * Unless you and Broadcom execute a separate written software license
 * agreement governing use of this software, this software is licensed
 * to you under the terms of the GNU General Public License version 2
 * (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
 * with the following added to such license:
 * 
 *    As a special exception, the copyright holders of this software give
 *    you permission to link this software with independent modules, and
 *    to copy and distribute the resulting executable under terms of your
 *    choice, provided that you also meet, for each linked independent
 *    module, the terms and conditions of the license of that module.
 *    An independent module is a module which is not derived from this
 *    software.  The special exception does not apply to any modifications
 *    of the software.
 * 
 * Not withstanding the above, under no circumstances may you combine
 * this software in any way with any other Broadcom software provided
 * under a license other than the GPL, without Broadcom's express prior
 * written consent.
 * 
 * :>
 *
 ************************************************************************/

#define _GNU_SOURCE
#include "sched.h"  /* for CPU_ZERO and CPU_SET */

#include "../oal.h"
#include "cms_util.h"
#include <sys/wait.h>  /* for waitpid */
#include <fcntl.h>     /* for open */
#include <errno.h>
#include <dirent.h>

/* amount of time to sleep between collect process attempt if timeout was specified. */
#define COLLECT_WAIT_INTERVAL_MS 40


static void freeArgs(char **argv);
CmsRet parseArgs(const char *cmd, const char *args, char ***argv);


CmsRet oal_spawnProcess(const SpawnProcessInfo *spawnInfo, SpawnedProcessInfo *procInfo)
{
   char **argv=NULL;
   SINT32 pid, i;
   CmsRet ret=CMSRET_SUCCESS;

   if ((ret = parseArgs(spawnInfo->exe, spawnInfo->args, &argv)) != CMSRET_SUCCESS)
   {
      return ret;
   }

   pid = fork();
   if (pid == 0)
   {
      SINT32 devNullFd=-1, fd;

      /*
       * This is the child.
       */

      /*
       * If user gave us specific instructions on fd re-routing,
       * do it before execv.
       */
      if ((spawnInfo->stdinFd == -1) ||
          (spawnInfo->stdoutFd == -1) ||
          (spawnInfo->stderrFd == -1)) {
         devNullFd = open("/dev/null", O_RDWR);
         if (devNullFd == -1)
         {
            cmsLog_error("open of /dev/null failed");
            exit(-1);
         }
      }

      if (spawnInfo->stdinFd != 0)
      {
         close(0);
         fd = (spawnInfo->stdinFd == -1) ? devNullFd : spawnInfo->stdinFd;
         dup2(fd, 0);
      }

      if (spawnInfo->stdoutFd != 1)
      {
         close(1);
         fd = (spawnInfo->stdoutFd == -1) ? devNullFd : spawnInfo->stdoutFd;
         dup2(fd, 1);
      }

      if (spawnInfo->stderrFd != 2)
      {
         close(2);
         fd = (spawnInfo->stderrFd == -1) ? devNullFd : spawnInfo->stderrFd;
         dup2(fd, 2);
      }

      if (devNullFd != -1)
      {
         close(devNullFd);
      }

      /* if child has a serverFd, dup it to the fixed number */
      if (spawnInfo->serverFd != -1)
      {
         close(CMS_DYNAMIC_LAUNCH_SERVER_FD);         
         dup2(spawnInfo->serverFd, CMS_DYNAMIC_LAUNCH_SERVER_FD);
      }

      if (spawnInfo->exe != NULL &&
          cmsUtl_strstr(spawnInfo->exe, "httpd") != NULL)
      {
         /* if httpd has a serverFd2, dup it to the fixed number */
         if (spawnInfo->serverFd2 != -1)
         {
            close(CMS_DYNAMIC_LAUNCH_SERVER_FD_2);         
            dup2(spawnInfo->serverFd2, CMS_DYNAMIC_LAUNCH_SERVER_FD_2);
         }
      }

      /* close all of the child's other fd's */
      for (i=3; i <= spawnInfo->maxFd; i++)
      {
         if ((spawnInfo->serverFd != -1 && i == CMS_DYNAMIC_LAUNCH_SERVER_FD) ||
             (spawnInfo->serverFd2 != -1 && i == CMS_DYNAMIC_LAUNCH_SERVER_FD_2))
         {
            continue;
         }
         close(i);
      }

      /*
       * Set all signal handlers back to default action,
       * in case the parent had set them to SIG_IGN or something.
       * See kernel/linux/include/asm-mips/signal.h
       */
      signal(SIGHUP, SIG_DFL);
      if (spawnInfo->inheritSigint == FALSE)
      {
         /*
          * Note: there is a bug in pthread library in the 4.4.2 toolchain
          * which breaks SIG_IGN inheritance.  Apps are advised to set
          * SIGINT handler explicitly in the app itself.
          */
         signal(SIGINT, SIG_DFL);
      }
      signal(SIGQUIT, SIG_DFL);
      signal(SIGILL, SIG_DFL);
      signal(SIGTRAP, SIG_DFL);
      signal(SIGABRT, SIG_DFL);  /* same as SIGIOT */
#if !defined(DESKTOP_LINUX) && !defined(__arm__) && !defined(__aarch64__)
      /* ARM doesn't have SIGEMT definition */
      signal(SIGEMT, SIG_DFL);
#endif
      signal(SIGFPE, SIG_DFL);
      signal(SIGBUS, SIG_DFL);
      signal(SIGSEGV, SIG_DFL);
      signal(SIGSYS, SIG_DFL);
      signal(SIGPIPE, SIG_DFL);
      signal(SIGALRM, SIG_DFL);
      signal(SIGTERM, SIG_DFL);
      signal(SIGUSR1, SIG_DFL);
      signal(SIGUSR2, SIG_DFL);
      signal(SIGCHLD, SIG_DFL);  /* same as SIGCLD */
      signal(SIGPWR, SIG_DFL);
      signal(SIGWINCH, SIG_DFL);
      signal(SIGURG, SIG_DFL);
      signal(SIGIO, SIG_DFL);    /* same as SIGPOLL */
      signal(SIGSTOP, SIG_DFL);
      signal(SIGTSTP, SIG_DFL);
      signal(SIGCONT, SIG_DFL);
      signal(SIGTTIN, SIG_DFL);
      signal(SIGTTOU, SIG_DFL);
      signal(SIGVTALRM, SIG_DFL);
      signal(SIGPROF, SIG_DFL);
      signal(SIGXCPU, SIG_DFL);
      signal(SIGXFSZ, SIG_DFL);


      /* overlay child executable over myself */
      execv(spawnInfo->exe, argv);

      /* We should not reach this line.  If we do, exec has failed. */
      exit(-1);
   }

   /* this is the parent */

   freeArgs(argv); /* don't need these anymore */

   /* set additional params in child if requested */
   if (spawnInfo->cpuGroupName)
   {
      CmsRet r2;
      r2 = oal_setCgroup(pid, CGROUP_CPUTREEDIR, spawnInfo->cpuGroupName);
      if (CMSRET_SUCCESS != r2)
      {
         cmsLog_error("Failed to assign pid %d to group %s",
               pid, spawnInfo->cpuGroupName);
      }
   }

   /*
    * Work around a bug(?) in the kernel by setting the cpu binding before
    * the RT priority.  Otherwise, rt_nr_migratory and nr_cpus_allowed
    * becomes inconsistent.
    */
   if (spawnInfo->cpuMask)
  {
	 CmsRet r2;
	 r2 = oal_setCpuMask(pid, spawnInfo->cpuMask);
	 if (CMSRET_SUCCESS != r2)
	 {
		cmsLog_error("Failed to set pid %d to cpuMask 0x%x",
			  pid, spawnInfo->cpuMask);
	 }
  }

   if (spawnInfo->setSched)
   {
#ifdef DESKTOP_LINUX
      cmsLog_debug("Fake DESKTOP_LINUX set pid %d to policy %d prio %d",
                    pid, spawnInfo->schedPolicy, spawnInfo->schedPriority);
#else
      CmsRet r2;
      r2 = oal_setScheduler(pid, spawnInfo->schedPolicy, spawnInfo->schedPriority);
      if (CMSRET_SUCCESS != r2)
      {
         cmsLog_error("Failed to set scheduler for %d to policy %d priority %d",
               pid, spawnInfo->schedPolicy, spawnInfo->schedPriority);
      }
#endif
   }

   /* fill in return info struct */
   memset(procInfo, 0, sizeof(SpawnedProcessInfo));
   procInfo->pid = pid;
   procInfo->status = PSTAT_RUNNING;

   if (spawnInfo->spawnMode == SPAWN_AND_WAIT)
   {
      CollectProcessInfo collectInfo;

      collectInfo.collectMode = COLLECT_PID_TIMEOUT;
      collectInfo.pid = pid;
      collectInfo.timeout = spawnInfo->timeout;

      ret = oal_collectProcess(&collectInfo, procInfo);
      if (ret == CMSRET_TIMED_OUT)
      {
         CmsRet r2;

         cmsLog_error("pid %d has not exited in %d ms, SIGKILL", pid, spawnInfo->timeout);
         r2 = oal_signalProcess(pid, SIGKILL);
         if (r2 != CMSRET_SUCCESS)
         {
            cmsLog_error("SIGKILL to pid %d failed with ret=%d", pid, r2);
         }

         /* try one more time to collect it */
         collectInfo.collectMode = COLLECT_PID_TIMEOUT;
         collectInfo.pid = pid;
         collectInfo.timeout = COLLECT_WAIT_INTERVAL_MS;
         r2 = oal_collectProcess(&collectInfo, procInfo);
         if (r2 == CMSRET_SUCCESS)
         {
            /* we finally go it, otherwise, leave ret at CMSRET_TIMED_OUT */
            ret = CMSRET_SUCCESS;
            cmsLog_debug("Successful collect of pid %d after SIGKILL", pid);
         }
         else
         {
            cmsLog_error("Still unable to collect pid %d after SIGKILL, ret=%d", pid, r2);
         }
      }
   }

   return ret;
}


CmsRet oal_collectProcess(const CollectProcessInfo *collectInfo, SpawnedProcessInfo *procInfo)
{
   SINT32 rc, status, waitOption=0;
   SINT32 requestedPid=-1;
   UINT32 timeoutRemaining=0;
   UINT32 sleepTime;
   CmsRet ret=CMSRET_TIMED_OUT;


   switch(collectInfo->collectMode)
   {
   case COLLECT_PID:
      requestedPid = collectInfo->pid;
      break;

   case COLLECT_PID_TIMEOUT:
      requestedPid = collectInfo->pid;
      timeoutRemaining = collectInfo->timeout;
      waitOption = WNOHANG;
      break;

   case COLLECT_ANY:
      requestedPid = -1;
      break;

   case COLLECT_ANY_TIMEOUT:
      requestedPid = -1;
      timeoutRemaining = collectInfo->timeout;
      waitOption = WNOHANG;
      break;
   }


   /*   
    * Linux does not offer a "wait for pid to exit for this amount of time"
    * system call, so I simulate that feature by doing a waitpid with 
    * WNOHANG and then sleeping for a short amount of time (50ms) between
    * checks.  The loop is a bit tricky because it has to handle callers
    * who don't want to wait at all and callers who do want to wait for
    * a specified time interval.  For callers who don't want to wait
    * (collectInfo->timeout == 0), timeoutRemaining will get set to 1,
    * which means they go through the loop exactly once.
    * For callers who want to wait a specified number of milliseconds,
    * we must go through the loop at least twice, once for the initial waitpid,
    * then a sleep, then one more waitpids before timing out (hence the weird
    * +/- 1 adjustments in the timeoutRemaining and sleepTime calculations).
    */
   timeoutRemaining = (timeoutRemaining <= 1) ?
                      (timeoutRemaining + 1) : timeoutRemaining;
   while (timeoutRemaining > 0)
   {
      rc = waitpid(requestedPid, &status, waitOption);
      if (rc == 0)
      {
         /*
          * requested process or any process has not exited.
          * Possibly sleep so we can check again, and calculate time remaining.
          */
         if (timeoutRemaining > 1)
         {
            sleepTime = (timeoutRemaining > COLLECT_WAIT_INTERVAL_MS) ?
                         COLLECT_WAIT_INTERVAL_MS : timeoutRemaining - 1;
            usleep(sleepTime * USECS_IN_MSEC);
            timeoutRemaining -= sleepTime;
         }
         else
         {
            timeoutRemaining = 0;
         }
      }
      else if (rc > 0)
      {
         /* OK, we got a process.  Fill out info. */
         procInfo->pid = rc;
         procInfo->status = PSTAT_EXITED;
         procInfo->signalNumber = WIFSIGNALED(status) ? WTERMSIG(status) : 0;
         procInfo->exitCode = WEXITSTATUS(status);

         timeoutRemaining = 0;
         ret = CMSRET_SUCCESS;
      }
      else
      {
         if (errno == ECHILD)
         {
            cmsLog_notice("Could not collect child pid %d, possibly stolen by SIGCHLD handler?", requestedPid);
            procInfo->status = PSTAT_EXITED;
            ret = CMSRET_OBJECT_NOT_FOUND;
         }
         else
         {
            cmsLog_error("bad pid %d, errno=%d", requestedPid, errno);
            procInfo->status = PSTAT_UNKNOWN;
            ret = CMSRET_INVALID_ARGUMENTS;
         }

         timeoutRemaining = 0;
      }
   }

   return ret;
}

CmsRet oal_signalProcess(SINT32 pid, SINT32 sig)
{
   SINT32 rc;

   if (pid <= 0)
   {
      cmsLog_error("bad pid %d", pid);
      return CMSRET_INVALID_ARGUMENTS;
   }

   if ((rc = kill(pid, sig)) < 0)
   {
      cmsLog_error("invalid signal(%d) or pid(%d)", sig, pid);
      return CMSRET_INVALID_ARGUMENTS;
   }

   return CMSRET_SUCCESS;
}



/** Give a single string, allocate and fill in an array of char *'s
 * each pointing to an individually malloc'd buffer containing a single
 * argument in the string; the array will end with a char * slot containing NULL.
 *
 * This array can be passed to execv.
 * This array must be freed by calling freeArgs.
 */
CmsRet parseArgs(const char *cmd, const char *args, char ***argv)
{
   UINT32 numArgs=3, i, len, argIndex=0;
   UBOOL8 inSpace=TRUE;
   const char *cmdStr;
   char **array;

   len = (args == NULL) ? 0 : strlen(args);

   /*
    * First count the number of spaces to determine the number of args
    * there are in the string.
    */
   for (i=0; i < len; i++)
   {
      if ((args[i] == ' ') && (!inSpace))
      {
         numArgs++;
         inSpace = TRUE;
      }
      else
      {
         inSpace = FALSE;
      }
   }

   array = (char **) cmsMem_alloc((numArgs) * sizeof(char *), ALLOC_ZEROIZE);
   if (array == NULL)
   {
      return CMSRET_RESOURCE_EXCEEDED;
   }

   /* locate the command name, last part of string */
   cmdStr = strrchr(cmd, '/');
   if (cmdStr == NULL)
   {
      cmdStr = cmd;
   }
   else
   {
      cmdStr++;  /* move past the / */
   }

   /* copy the command into argv[0] */
   array[argIndex] = cmsMem_alloc(strlen(cmdStr) + 1, ALLOC_ZEROIZE);
   if (array[argIndex] == NULL)
   {
      cmsLog_error("malloc of %d failed", strlen(cmdStr) + 1);
      freeArgs(array);
      return CMSRET_RESOURCE_EXCEEDED;
   }
   else
   {
      strcpy(array[argIndex], cmdStr);
      argIndex++;
   }


   /*
    * Wow, this is going to be incredibly messy.  I have to malloc a buffer
    * for each arg and copy them in individually.
    */
   inSpace = TRUE;
   for (i=0; i < len; i++)
   {
      if ((args[i] == ' ') && (!inSpace))
      {
         numArgs++;
         inSpace = TRUE;
      }
      else if ((args[i] != ' ') && (inSpace))
      {
            UINT32 startIndex, endIndex;

            /*
             * this is the first character I've seen after a space.
             * Figure out how many letters this arg is, malloc a buffer
             * to hold it, and copy it into the buffer.
             */
            startIndex = i;
            endIndex = i;
            while ((endIndex < len) && (args[endIndex] != ' '))
            {
               endIndex++;
            }

            array[argIndex] = cmsMem_alloc(endIndex - startIndex + 1, ALLOC_ZEROIZE);
            if (array[argIndex] == NULL)
            {
               cmsLog_error("malloc of %d failed", endIndex - startIndex + 1);
               freeArgs(array);
               return CMSRET_RESOURCE_EXCEEDED;
            }

            memcpy(array[argIndex], &args[startIndex], endIndex - startIndex);
            /*
            cmsLog_debug("index=%d len=%d (%s)", argIndex, endIndex - startIndex, &args[startIndex]);
            */

            argIndex++;

            inSpace = FALSE;
      }
   }

   /* check what we did */
   i = 0;
   while (array[i] != NULL)
   {
      cmsLog_debug("argv[%d] = %s", i, array[i]);
      i++;
   }

   (*argv) = array;


   return CMSRET_SUCCESS;
}


/** Free all the arg buffers in the argv, and the argv array itself.
 *
 */
void freeArgs(char **argv)
{
   UINT32 i=0;

   while (argv[i] != NULL)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(argv[i]);
      i++;
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(argv);
}


int oal_getPidByName(const char *name)
{
   DIR *dir;
   FILE *fp;
   struct dirent *dent;
   UBOOL8 found=FALSE;
   int pid, rc, p, i;
   int rval = CMS_INVALID_PID;
   char processName[BUFLEN_256];
   char filename[BUFLEN_256];

   if (NULL == (dir = opendir("/proc")))
   {
      cmsLog_error("could not open /proc");
      return rval;
   }

   while (!found && (dent = readdir(dir)) != NULL)
   {
      /*
       * Each process has its own directory under /proc, the name of the
       * directory is the pid number.
       */
      if ((dent->d_type == DT_DIR) &&
          (CMSRET_SUCCESS == cmsUtl_strtol(dent->d_name, NULL, 10, &pid)))
      {
         snprintf(filename, sizeof(filename), "/proc/%d/stat", pid);
         if ((fp = fopen(filename, "r")) == NULL)
         {
            cmsLog_error("could not open %s", filename);
         }
         else
         {
            /* Get the process name, format: 913 (consoled) */
            memset(processName, 0, sizeof(processName));
            rc = fscanf(fp, "%d (%s", &p, processName);
            fclose(fp);

            if (rc >= 2)
            {
               i = strlen(processName);
               if (i > 0)
               {
                  /* strip out the trailing ) character */
                  if (processName[i-1] == ')')
                     processName[i-1] = 0;
               }
            }

            if (!cmsUtl_strcmp(processName, name))
            {
               rval = pid;
               found = TRUE;
            }
         }
      }
   }

   closedir(dir);

   return rval;
}


int oal_getNameByPid(int pid, char *nameBuf, int nameBufLen)
{
   FILE *fp;
   char processName[BUFLEN_256];
   char filename[BUFLEN_256];
   int rval=-1;

   if (nameBuf == NULL || nameBufLen <= 0)
   {
      cmsLog_error("invalid args, nameBuf=%p nameBufLen=%d", nameBuf, nameBufLen);
      return rval;
   }

   snprintf(filename, sizeof(filename), "/proc/%d/stat", pid);
   if ((fp = fopen(filename, "r")) == NULL)
   {
      cmsLog_error("could not open %s", filename);
   }
   else
   {
      int rc, p;
      /* Get the process name, format: 913 (consoled) */
      memset(nameBuf, 0, nameBufLen);
      memset(processName, 0, sizeof(processName));
      rc = fscanf(fp, "%d (%s", &p, processName);
      fclose(fp);

      if (rc >= 2)
      {
         int c=0;
         while (c < nameBufLen-1 &&
                processName[c] != 0 && processName[c] != ')')
         {
            nameBuf[c] = processName[c];
            c++;
         }
         rval = 0;
      }
      else
      {
         cmsLog_error("fscanf of %s failed", filename);
      }
   }

   return rval;
}


CmsRet oal_setScheduler(SINT32 pid, SINT32 policy, SINT32 priority)
{
   struct sched_param sp;
   CmsRet ret=CMSRET_SUCCESS;

   sp.sched_priority = priority;

   if (sched_setscheduler(pid, policy, &sp) == -1)
   {
      ret = CMSRET_INTERNAL_ERROR;
   }

   return ret;
}


CmsRet oal_setCpuMask(SINT32 pid, UINT32 cpuMask)
{
   CmsRet ret=CMSRET_SUCCESS;
   cpu_set_t cpuSet;
   UINT32 cpu=0;
   UINT32 bit;

   CPU_ZERO(&cpuSet);

   for (cpu=0; cpu < 16; cpu++)  //I'm only testing for 16 CPU's, should be enough
   {
      bit = (1 << cpu);

      if (bit & cpuMask)
      {
         CPU_SET(cpu, &cpuSet);
      }
   }

   if (sched_setaffinity(pid, sizeof(cpuSet), &cpuSet) == -1)
   {
      ret = CMSRET_INTERNAL_ERROR;
   }

   return ret;
}


CmsRet oal_setCgroup(SINT32 pid, const char *groupBase, const char *groupName)
{
   CmsRet ret;
   char taskPath[CMS_MAX_FULLPATH_LENGTH]={0};
   char pidBuf[BUFLEN_16]={0};

   if (0 == pid)
   {
      pid = getpid();
   }

   snprintf(pidBuf, sizeof(pidBuf)-1, "%d", pid);
   snprintf(taskPath, sizeof(taskPath)-1, "%s/%s/tasks", groupBase, groupName);
   ret = cmsFil_writeToProc(taskPath, pidBuf);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not write %s to %s, ret=%d",
            pidBuf, taskPath, ret);
   }

   return ret;
}

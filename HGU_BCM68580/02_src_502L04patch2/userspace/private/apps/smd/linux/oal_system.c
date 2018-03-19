/***********************************************************************
 *
 *  Copyright (c) 2006-2007  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2006:proprietary:standard

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
#include <fcntl.h>     /* for O_RDRW */
#include <sys/stat.h>  /* for mkdir */
#include <dirent.h>
#include <errno.h>
#include "cms_util.h"
#include "prctl.h"
#include "../oal.h"

/* declared in main.c */
extern UBOOL8 keepLooping;
extern SINT32 shmId;
extern SINT32 maxFd;


/* not used any more
* static CmsRet updateAndReloadRandomSeed(void); 
*/


static CmsRet init_ksched_properties(void);


#ifdef DESKTOP_LINUX
static void terminalSignalHandler(SINT32 sig);
#endif


CmsRet oalSystem_init()
{
   CmsRet ret=CMSRET_SUCCESS;

   /* Set up signal handlers */

   signal(SIGPIPE, SIG_IGN);

   /*
    * Don't do this => signal(SIGCHLD, SIG_IGN); <==
    * because that will prevent smd's child processes from becoming zombies.
    * Smd wants the children to become zombies when they die so that it can
    * collect info about why they exited.  See discussion in man 2 waitpid.
    */

#ifdef DESKTOP_LINUX
   /*
    * On desktop, catch SIGINT and SIGTERM and cleanly exit.
    */
   signal(SIGINT, terminalSignalHandler);
   signal(SIGTERM, terminalSignalHandler);

   /*
    * Also on desktop, we need to create a /var directory in fs.install
    */
   {
      char varPath[CMS_MAX_FULLPATH_LENGTH]={0};
      SINT32 rc;

      ret = cmsUtl_getRunTimePath("/var", varPath, sizeof(varPath));
      if (ret != CMSRET_SUCCESS)
      {
         return ret;
      }

      rc = mkdir(varPath, 0777);
      if (rc < 0 && errno != EEXIST)
      {
         cmsLog_debug("mkdir of %s failed with errno=%d", varPath, errno);
         return CMSRET_INTERNAL_ERROR;
      }

      ret = cmsUtl_getRunTimePath("/data", varPath, sizeof(varPath));
      if (ret != CMSRET_SUCCESS)
      {
         return ret;
      }

      rc = mkdir(varPath, 0777);
      if (rc < 0 && errno != EEXIST)
      {
         cmsLog_debug("mkdir of %s failed with errno=%d", varPath, errno);
         return CMSRET_INTERNAL_ERROR;
      }
   }
#else
   /*
    * On the modem, block SIGINT because user might press control-c to stop
    * a ping command or something.
    */
   signal(SIGINT, SIG_IGN);

   /*
    * When busybox does reboot, first a SIGTERM is sent to all processes.
    * I think I want smd to stay alive so that other apps do not notice smd
    * is dead and print an error message.
    */
   signal(SIGTERM, SIG_IGN);


   if ((ret = cmsFil_writeToProc("/proc/sys/kernel/print-fatal-signals", "1")) != CMSRET_SUCCESS)
      {
         return ret;
      }

   if ((ret = cmsFil_writeToProc("/proc/sys/net/ipv4/conf/all/force_igmp_version", "3")) != CMSRET_SUCCESS)
   {
      return ret;
   }

   if ((ret = cmsFil_writeToProc("/proc/sys/net/ipv4/ip_forward", "1")) != CMSRET_SUCCESS)
   {
      return ret;
   }
#ifdef SUPPORT_IPV6
#ifdef DMP_DEVICE2_HOMEPLUG_1
  /* disable IPv6 forwarding for HomePlug devices */
   if ((ret = cmsFil_writeToProc("/proc/sys/net/ipv6/conf/all/forwarding", "0")) != CMSRET_SUCCESS)
   {
      return ret;
   }
#else
//#if 0 /* IPv6Ready- for Core Protocol (Host) testing, don't set forwarding to 1. */
   if ((ret = cmsFil_writeToProc("/proc/sys/net/ipv6/conf/all/forwarding", "1")) != CMSRET_SUCCESS)
   {
      return ret;
   }
//#endif
#endif /* DMP_DEVICE2_HOMEPLUG_1 */
#endif 

   if ((ret = cmsFil_writeToProc("/proc/sys/net/ipv4/ip_dynaddr", "1")) != CMSRET_SUCCESS)
   {
      return ret;
   }

#if 0
   /* The ipfrag_high_thresh tells the kernel the maximum amount of memory to use to reassemble IP fragments.
    * When and if the high threshold is reached, the fragment handler will toss all packets until 
    * the memory usage reaches ipfrag_low_thresh instead.
    */

   if ((ret = cmsFil_writeToProc("/proc/sys/net/ipv4/ipfrag_low_thresh", "1024")) != CMSRET_SUCCESS)
   {
      return ret;
   }

   if ((ret = cmsFil_writeToProc("/proc/sys/net/ipv4/ipfrag_high_thresh", "2048")) != CMSRET_SUCCESS)
   {
      return ret;
   }
#endif

   /*
   *  It's not a good idea to write to scrach pad on boot time for random seed.
   *  Keep it here for a while until a beter methond found OR
   *  to be moved  later on if not needed.
   *
   * Initialize the Random seed 
   *
   * updateAndReloadRandomSeed();
   */
#endif  /* DESKTOP_LINUX */

   if ((ret = init_ksched_properties()) != CMSRET_SUCCESS)
   {
      return ret;
   }

   return ret;
}


#ifdef DESKTOP_LINUX
void terminalSignalHandler(SINT32 sig)
{
   printf("smd got sig %d, terminating by setting keepLooping to FALSE\n\n",
          sig);

   keepLooping = FALSE;
}
#endif /* DESKTOP_LINUX */


void oalSystem_cleanup()
{
   return;
}



CmsRet init_ksched_properties(void)
{
   DIR *dir;
   FILE *fp;
   struct dirent *dent;
   int pid;
   char processName[BUFLEN_256]={0};
   char cmdLine[512]={0};
   const CmsEntityInfo *eInfo;

   cmsLog_notice("config the kthreads and apps that have already started");

   if (NULL == (dir = opendir("/proc")))
   {
      cmsLog_error("could not open /proc");
      return CMSRET_INTERNAL_ERROR;
   }

   while ((dent = readdir(dir)) != NULL)
   {
      /*
       * Each process has its own directory under /proc, the name of the
       * directory is the pid number.
       */
      if ((dent->d_type == DT_DIR) &&
          (CMSRET_SUCCESS == cmsUtl_strtol(dent->d_name, NULL, 10, &pid)))
      {
         snprintf(cmdLine, sizeof(cmdLine), "/proc/%d/stat", pid);
         if ((fp = fopen(cmdLine, "r")) == NULL)
         {
            cmsLog_error("could not open %s", cmdLine);
         }
         else
         {
            int rc, p, i;
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

            eInfo = cmsEid_getEntityInfoByStringName(processName);
            if (eInfo)
            {
               if (eInfo->flags & (EIF_SET_CGROUPS|EIF_SET_SCHED|EIF_SET_CPUMASK))
               {
                  cmsLog_debug("found (%d) %s: eInfo->flags=0x%x %d/%d 0x%x %s",
                               pid, processName, eInfo->flags,
                               eInfo->schedPolicy, eInfo->schedPriority,
                               eInfo->cpuMask, eInfo->cpuGroupName);
               }

#ifdef SUPPORT_CGROUPS
               if (eInfo->flags & EIF_SET_CGROUPS)
               {
                  if (CMSRET_SUCCESS != prctl_setCgroup(pid, CGROUP_CPUTREEDIR, eInfo->cpuGroupName))
                  {
                     cmsLog_error("Could not set pid %d name %s to group %s",
                                  pid, eInfo->name, eInfo->cpuGroupName);
                  }
               }
#endif

               if (eInfo->flags & EIF_SET_SCHED)
               {
                  cmsLog_debug("setScheduler for pid=%d policy=%d priority=%d",
                                pid, eInfo->schedPolicy, eInfo->schedPriority);
                  if (CMSRET_SUCCESS != prctl_setScheduler(pid, eInfo->schedPolicy, eInfo->schedPriority))
                  {
                     cmsLog_error("Could not set pid %d name %s to policy %d priority %d",
                                  pid, eInfo->name, eInfo->schedPolicy, eInfo->schedPriority);
                  }
               }

               if (eInfo->flags & EIF_SET_CPUMASK)
               {
                  cmsLog_debug("setCpuMask for pid=%d mask=0x%x",
                                pid, eInfo->cpuMask);
                  if (CMSRET_SUCCESS != prctl_setCpuMask(pid, eInfo->cpuMask))
                  {
                     cmsLog_error("Could not set pid %d name %s to cpuMask 0x%x",
                                  pid, eInfo->name, eInfo->cpuMask);
                  }
               }
            }
         }
      }
   }

   return CMSRET_SUCCESS;
}


#if Not_used
#define RAND_DEV_NAME "/dev/urandom"
#define RAND_SEED_LEN 512

static CmsRet updateAndReloadRandomSeed(void)
{
   int fd = 0;
   unsigned char buf[RAND_SEED_LEN];

   fd = open(RAND_DEV_NAME, O_RDWR);

   if(fd < 0)
     cmsLog_error("can't open %s for read\n", RAND_DEV_NAME);

   /* get the previous random seed and send it urandom device */
   if (cmsPsp_get("URandomSeed", buf, RAND_SEED_LEN) <= 0) 
   {
      cmsLog_error("can't find random seed\n");
   }
   else
   {
      if(write(fd, buf, RAND_SEED_LEN) < 0)
        cmsLog_error("can't write random seed\n");
   }

   /* read and store the new random seed for next boot time*/
   if(read(fd, buf, RAND_SEED_LEN) < 0)
   {
      cmsLog_error("can't read random seed\n");
   }
   else
   {
      if(CMSRET_SUCCESS != cmsPsp_set("URandomSeed", buf,  RAND_SEED_LEN))
      {
         cmsLog_error("error while calling cmsPsp_set for URandomSeed\n");
      }
   }

   close(fd);

   return CMSRET_SUCCESS;
}
#endif

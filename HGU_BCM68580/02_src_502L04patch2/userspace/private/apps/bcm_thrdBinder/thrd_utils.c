/*
#
#  Copyright 2011, Broadcom Corporation
#
# <:label-BRCM:2011:proprietary:standard
# 
#  This program is the proprietary software of Broadcom and/or its
#  licensors, and may only be used, duplicated, modified or distributed pursuant
#  to the terms and conditions of a separate, written license agreement executed
#  between you and Broadcom (an "Authorized License").  Except as set forth in
#  an Authorized License, Broadcom grants no license (express or implied), right
#  to use, or waiver of any kind with respect to the Software, and Broadcom
#  expressly reserves all rights in and to the Software and all intellectual
#  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
#  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
#  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
# 
#  Except as expressly set forth in the Authorized License,
# 
#  1. This program, including its structure, sequence and organization,
#     constitutes the valuable trade secrets of Broadcom, and you shall use
#     all reasonable efforts to protect the confidentiality thereof, and to
#     use this information only in connection with your use of Broadcom
#     integrated circuit products.
# 
#  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
#     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
#     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
#     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
#     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
#     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
#     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
#     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
#     PERFORMANCE OF THE SOFTWARE.
# 
#  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
#     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
#     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
#     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
#     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
#     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
#     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
#     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
#     LIMITED REMEDY.
# :>
*/

#define _GNU_SOURCE
#include <sched.h>  /* for CPU_ZERO and CPU_SET */


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <time.h>



#include "thrd_binder.h"




/** local helper function to push the latest CPU utilization value into
 * the array and shift all other values to the right.
 */
static void push_cpuUtil(unsigned int idx, int percentUtil)
{
   int i;

   for (i=TB_NUM_HISTORY_SLOTS-1; i > 0; i--)
   {
      /* shift all values one to the right */
      cpu_util_array[idx][i] = cpu_util_array[idx][i-1];
   }

   /* push in the new value at 0 */
   cpu_util_array[idx][0] = percentUtil;

   /* debug dump */
#ifdef DEBUG
   printf("[%d] ", idx);
   for (i=0; i < TB_NUM_HISTORY_SLOTS; i++)
   {
      printf("%02d ", cpu_util_array[idx][i]);
   }
   printf("\n");
#endif
}


void update_cpuUtil()
{
   static unsigned long long total_util_array[TB_NUM_CPUS]={0};
   static unsigned long long total_jiffies_array[TB_NUM_CPUS]={0};

   FILE *fp;
   char lineBuf[256]={0};
   unsigned int idx;
   unsigned long long usr, nic, sys, idle, iowait, irq, softirq, steal, total, util;
   int percentUtil;

#ifdef DEBUG
   printf("\n\n[%s] Updating CPU utilization map\n", get_timestamp());
#endif

   idx=usr=nic=sys=idle=iowait=irq=softirq=steal=total=util=0;

   fp = fopen("/proc/stat", "r");
   if (fp == NULL)
   {
      printf("%s: could not open /proc/stat\n", TB_PROG_NAME);
      return;
   }

   while (fgets(lineBuf, sizeof(lineBuf), fp) &&
          (lineBuf[0] == 'c') &&
          (lineBuf[1] == 'p') &&
          (lineBuf[2] == 'u'))
   {
      /* looking for cpu0, cpu1, etc */
      if (lineBuf[3] >= '0' && lineBuf[3] <= '9')
      {
         sscanf(lineBuf, "cpu%u %llu %llu %llu %llu %llu %llu %llu %llu",
                &idx, &usr, &nic, &sys, &idle,
                &iowait, &irq, &softirq, &steal);

         if (idx >= TB_NUM_CPUS)
         {
            // for testing on DESKTOP, we might see 8 CPUs, but our array only
            // holds 2 CPUs.  Just ignore the extra ones.
            continue;
         }

         total = usr+nic+sys+idle+iowait+irq+softirq+steal;
         util = total-idle-iowait;

         /* percent CPU utilization is number of busy jiffies divided by
          * number of elapsed jiffies.  This algorithm is independent of
          * how long a jiffie is.
          */
         // printf("%d: %d / %d\n", idx, (int) (util - total_util_array[idx]),
         //                         (int) (total - total_jiffies_array[idx]));
         percentUtil = (int) (((util - total_util_array[idx]) *100) /
                               (total - total_jiffies_array[idx]));
         total_util_array[idx] = util;
         total_jiffies_array[idx] = total;

         push_cpuUtil(idx, percentUtil);
      }
   }


   fclose(fp);
}


static void set_cpuMask(int pid, unsigned int mask)
{
   cpu_set_t cpuSet;
   unsigned int cpu=0;
   unsigned int bit;

   CPU_ZERO(&cpuSet);

   for (cpu=0; cpu < 16; cpu++)  //I'm only testing for 16 CPU's, should be enough
   {
      bit = (1 << cpu);

      if (bit & mask)
      {
         CPU_SET(cpu, &cpuSet);
      }
   }

   if (sched_setaffinity(pid, sizeof(cpuSet), &cpuSet) == -1)
   {
      printf("%s : Could not set mask 0x%x for pid %d\n",
             TB_PROG_NAME, mask, pid);
   }
}

void bind_thread(int pid)
{
   set_cpuMask(pid, 0x1);  // bind to CPU 0
}

void unbind_thread(int pid)
{
   set_cpuMask(pid, 0x3);  // bind to CPU 0 and 1, which basically means
                           // unbound since we only have 2 CPUs
}


/* this function is cut-and-pasted from oal_prctl.c (in cms_util lib).
 * Because we do not want to tie this app to CMS, it cannot link against
 * the cms_util library.
 */
int get_pidByName(const char *name)
{
   DIR *dir;
   FILE *fp;
   struct dirent *dent;
   int found=0;
   int pid, rc, p, i;
   int rval = 0;
   char processName[256]={0};
   char filename[256]={0};

   if (NULL == (dir = opendir("/proc")))
   {
      printf("could not open /proc\n");
      return 0;
   }

   while (!found && (dent = readdir(dir)) != NULL)
   {
      /*
       * Each process has its own directory under /proc, the name of the
       * directory is the pid number.
       */
      if ((dent->d_type == DT_DIR) &&
          (pid = strtol(dent->d_name, NULL, 10)))
      {
         snprintf(filename, sizeof(filename), "/proc/%d/stat", pid);
         if ((fp = fopen(filename, "r")) == NULL)
         {
            printf("could not open %s\n", filename);
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

            if (!strcmp(processName, name))
            {
               rval = pid;
               found = 1;
            }
         }
      }
   }

   closedir(dir);

   if (found == 0)
   {
      printf("\n%s: could not find thread named %s, ignored\n", TB_PROG_NAME, name);
   }

   return rval;
}



char timestampBuf[256];

const char *get_timestamp()
{
   struct tm tm;
   time_t t;

   time(&t);
   localtime_r(&t, &tm);
   strftime(timestampBuf, sizeof(timestampBuf), "%m/%d %H:%M:%S", &tm);

   return timestampBuf;
}


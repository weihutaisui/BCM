/***********************************************************************
 *
 *  Copyright (c) 2007  Broadcom Corporation
 *  All Rights Reserved
 *
 * <:label-BRCM:2012:proprietary:standard
 * 
 *  This program is the proprietary software of Broadcom and/or its
 *  licensors, and may only be used, duplicated, modified or distributed pursuant
 *  to the terms and conditions of a separate, written license agreement executed
 *  between you and Broadcom (an "Authorized License").  Except as set forth in
 *  an Authorized License, Broadcom grants no license (express or implied), right
 *  to use, or waiver of any kind with respect to the Software, and Broadcom
 *  expressly reserves all rights in and to the Software and all intellectual
 *  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 *  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 *  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 * 
 *  Except as expressly set forth in the Authorized License,
 * 
 *  1. This program, including its structure, sequence and organization,
 *     constitutes the valuable trade secrets of Broadcom, and you shall use
 *     all reasonable efforts to protect the confidentiality thereof, and to
 *     use this information only in connection with your use of Broadcom
 *     integrated circuit products.
 * 
 *  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
 *     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
 *     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
 *     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
 *     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
 *     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
 *     PERFORMANCE OF THE SOFTWARE.
 * 
 *  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
 *     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
 *     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
 *     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 *     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
 *     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
 *     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
 *     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
 *     LIMITED REMEDY.
 * :>
 *
 ************************************************************************/


#include "cms.h"
#include "cms_util.h"
#include "cms_core.h"
#include "prmgr.h"
#include "event_interest.h"
#include "sched_events.h"
#include "oal.h"
#include <signal.h>

/*
 * build_timestamp is in cms_core/linux/build_timestamp.c
 * I really shouldn't grab a symbol out of the lib like this,
 * but I need it for early startup before the API has been initialized.
 */
extern char *build_timestamp;


/* globals */
SINT32         shmId = UNINITIALIZED_SHM_ID;

SINT32         maxFd = 0;  /* Updated by UPDATE_MAXFD in oal.h */
UBOOL8           keepLooping=TRUE;
UBOOL8 useConfiguredLogLevel=TRUE;  /* smd cannot read MDM until after ssk has initialized it. */

static SINT32 smd_main(void);

static CmsRet smd_init(void);
static CmsRet system_init(void);

static void smd_cleanup(void);
static void system_cleanup(void);

/*
 * When smd starts up, it will immediately create a child smd, which is the
 * "real" smd.  The parent smd, i.e. "sync smd", will wait for a SIGHUP from
 * the child/real smd to indicate that the system is initialized enough for
 * other apps to start running.  The parent/sync smd will then exit,
 * which allows other apps to get started by bcm_init_runner.
 */
SINT32 sync_smd_pid;
static SINT32 sync_smd_got_hup=0;
void sighup_catcher()
{
   sync_smd_got_hup=1;
}


void usage(char *progName)
{
   printf("usage: %s [-v num]\n", progName);
   printf("       v: set verbosity, where num==0 is LOG_ERROR, 1 is LOG_NOTICE, all others is LOG_DEBUG\n");
}


SINT32 main(SINT32 argc, char *argv[])
{
   SINT32 c, logLevelNum;
   CmsLogLevel logLevel=DEFAULT_LOG_LEVEL;
   CmsRet ret;
   SINT32 exitCode=0;
   SINT32 pid;

   sync_smd_pid = getpid();
   pid = fork();
   if (pid < 0)
   {
      printf("smd could not fork!  Startup failed.\n");
      exit(-1);
   }

   if (pid  > 0)
   {
      /* this is the parent (sync smd) */
      signal(SIGHUP, sighup_catcher);
      signal(SIGCHLD, sighup_catcher);  // in case smd dies
      while (sync_smd_got_hup == 0)
      {
         sleep(1);
      }
      exit(0);
   }

   printf("\n");
   printf("===== Release Version %s (build timestamp %s) =====\n\n", CMS_RELEASE_VERSION, build_timestamp);

   cmsLog_initWithName(EID_SMD, argv[0]);

#ifdef CMS_STARTUP_DEBUG
   logLevel=LOG_LEVEL_DEBUG;
   cmsLog_setLevel(logLevel);
   useConfiguredLogLevel = FALSE;
#endif

   /* parse command line args */
   while ((c = getopt(argc, argv, "v:")) != -1)
   {
      switch(c)
      {
      case 'v':
         logLevelNum = atoi(optarg);
         if (logLevelNum == 0)
         {
            logLevel = LOG_LEVEL_ERR;
         }
         else if (logLevelNum == 1)
         {
            logLevel = LOG_LEVEL_NOTICE;
         }
         else
         {
            logLevel = LOG_LEVEL_DEBUG;
         }
         cmsLog_setLevel(logLevel);
         useConfiguredLogLevel = FALSE;
         break;

      default:
         usage(argv[0]);
         cmsLog_error("bad arguments, exit");
         exit(-1);
      }
   }



   if ((ret = smd_init()) != CMSRET_SUCCESS)
   {
      cmsLog_error("initialization failed (%d), exit.", ret);
      return -1;
   }

   exitCode = smd_main();


   cmsLog_notice("exiting with code %d", exitCode);
   smd_cleanup();

   cmsLog_cleanup();

   return exitCode;
}


/** This is the main loop of smd.
 */
SINT32 smd_main()
{
   CmsRet ret;
   SINT32 exitCode=0;

   while (keepLooping)
   {
      /* do periodic/timer event processing */
      sched_process();

      /*
       * wait for events (using OS dependent select), and proccess them.
       */
      if ((ret = oal_processEvents()) != CMSRET_SUCCESS)
      {
         /* time to exit, mwang_todo: set exit code based on return value */
         keepLooping = FALSE;
      }
   }

   return exitCode;
}


CmsRet smd_init(void)
{
   CmsRet ret;

   if ((ret = system_init()) != CMSRET_SUCCESS)
   {
      return ret;
   }

   if ((ret = prmgr_init()) != CMSRET_SUCCESS)
   {
      return ret;
   }

   if ((ret = oalEvent_init()) != CMSRET_SUCCESS)
   {
      return ret;
   }


   /*
    * Start the first stage of launch on boot.  In the
    * first stage, we only launch ssk, which will initialize
    * the MDM.  Once that happens, smd will get an event msg,
    * which allows it execute the second stage of launchOnBoot.
    */
   oal_launchOnBoot(1);


   cmsLog_notice("done, ret=%d", ret);

   return ret;
}


CmsRet system_init()
{
   CmsRet ret;

   cmsLog_notice("entered");

   ret = oalSystem_init();

   cmsLog_notice("done, ret=%d", ret);

   return ret;
}



void dumpMemStats()
{
#ifdef verbose_mem_stats
   CmsMemStats stats;

   cmsMem_getStats(&stats);

   printf("\n\n=========== smd mem stats =================\n");
   printf("shmBytesAllocd=%u (total=%u) shmNumAllocs=%u shmNumFrees=%u delta=%u\n",
          stats.shmBytesAllocd, stats.shmTotalBytes,
          stats.shmNumAllocs, stats.shmNumFrees, stats.shmNumAllocs - stats.shmNumFrees);
   printf("bytesAllocd=%u numAllocs=%u numFrees=%u delta=%u\n",
          stats.bytesAllocd, stats.numAllocs, stats.numFrees,
          stats.numAllocs-stats.numFrees);
   printf("==============================================\n\n");
#endif
}



void system_cleanup()
{
   oalSystem_cleanup();
}


void smd_cleanup(void)
{

   oalEvent_cleanup();
   prmgr_cleanup();
   system_cleanup();


   /*
    * Must call this before cmsMdm_cleanup because cmsMdm_cleanup
    * will destroy the shared memory region.
    */
   dumpMemStats();


   cmsMdm_cleanup();
}




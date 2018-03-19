/******************************************************************************
 *
 *  Copyright (c) 2009  Broadcom Corporation
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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"
#include "cms_dal.h"
#include "cms_cli.h"
#include "prctl.h"


#ifdef BUILD_BCMIPC
#include "omci_ipc.h"
#endif

/* global vars */
UBOOL8 keepLooping = TRUE;

/* file local vars */
static void *msgHandle=NULL;


void terminalsignalhandler(SINT32 signum)
{
   cmsLog_notice("caught signal %d, set keepLooping to FALSE", signum);
   keepLooping = FALSE;
}


static void initLoggingFromConfig(UBOOL8 useConfiguredLogLevel)
{
   ConsoledCfgObject *obj;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   if ((ret = cmsLck_acquireLockWithTimeout(CLI_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      cmsLck_dumpInfo();
      return;
   }

   if ((ret = cmsObj_get(MDMOID_CONSOLED_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of CONSOLED_CFG object failed, ret=%d", ret);
   }
   else
   {
      if (useConfiguredLogLevel)
      {
         cmsLog_setLevel(cmsUtl_logLevelStringToEnum(obj->loggingLevel));
      }

      cmsLog_setDestination(cmsUtl_logDestinationStringToEnum(obj->loggingDestination));

      cmsObj_free((void **) &obj);
   }

   cmsLck_releaseLock();
}


static void spawnShellForDebug(void)
{
/* copied from cli_cmd.c */
   SpawnProcessInfo spawnInfo;
   SpawnedProcessInfo procInfo;
   CmsRet r2;

   memset(&spawnInfo, 0, sizeof(spawnInfo));

   spawnInfo.exe = "/bin/sh";
   spawnInfo.args = "-c sh";
   spawnInfo.spawnMode = SPAWN_AND_RETURN;
   spawnInfo.stdinFd = 0;
   spawnInfo.stdoutFd = 1;
   spawnInfo.stderrFd = 2;
   spawnInfo.serverFd = -1;
   spawnInfo.maxFd = 50;
   spawnInfo.inheritSigint = FALSE;  /* set SIGINT back to default */

   memset(&procInfo, 0, sizeof(procInfo));

   r2 = prctl_spawnProcess(&spawnInfo, &procInfo);
   if (r2 == CMSRET_SUCCESS)
   {
       /* now wait for sh to finish */
       CollectProcessInfo collectInfo;

       collectInfo.collectMode = COLLECT_PID;
       collectInfo.pid = procInfo.pid;

       r2 = prctl_collectProcess(&collectInfo, &procInfo);
   }
   else
   {
       cmsLog_error("failed to spawn sh, r2=%d", r2);
   }
}


void usage(char *progName)
{
   printf("usage: %s [-v num] [-m shmId]\n", progName);
   printf("       v: set verbosity, where num==0 is LOG_ERROR, 1 is LOG_NOTICE, all others is LOG_DEBUG\n");
   printf("       m: shared memory id, -1 if standalone or not using shared mem.\n");
}


SINT32 main(SINT32 argc, char *argv[]) 
{
   SINT32 c, logLevelNum;
   SINT32 shmId=UNINITIALIZED_SHM_ID;
   CmsLogLevel logLevel=DEFAULT_LOG_LEVEL;
   UBOOL8 useConfiguredLogLevel=TRUE;
   UBOOL8 cmsIsUp=FALSE;
   CmsRet ret;


   cmsLog_initWithName(EID_CONSOLED, argv[0]);

   /* update cli lib with the application data */
   cmsCli_setAppData("Consoled", NULL, NULL, 0);

#ifdef DESKTOP_LINUX
   /*
    * On desktop, catch SIGINT and cleanly exit.
    */
   signal(SIGINT, terminalsignalhandler);
#else
   /*
    * On the modem, block SIGINT because user might press control-c to stop
    * a ping command or something.
    */
   signal(SIGINT, SIG_IGN);
#endif


   while ((c = getopt(argc, argv, "v:m:")) != -1)
   {
      switch(c)
      {
      case 'm':
         shmId = atoi(optarg);
         break;

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
         cmsLog_error("bad arguments, exit");
         usage(argv[0]);
         cmsLog_cleanup();
         exit(-1);
      }
   }

#ifdef BUILD_BCMIPC
   omciIpc_clientInit(CONSOLED_CLIENT);
#endif
   while (!cmsIsUp)
   {
      /* Try to connect to smd (main control app of CMS).  If cannot
       * connect, then drop to busybox shell for debug.
       */
      if ((ret = cmsMsg_initWithFlags(EID_CONSOLED, 0, &msgHandle)) != CMSRET_SUCCESS)
      {
         cmsLog_error("could not connect to CMS smd (ret=%d) == drop to shell for debug", ret);
         spawnShellForDebug();
         /* once user has exited from shell, try to contact CMS smd again */
         continue;
      }

      if (shmId == UNINITIALIZED_SHM_ID)
      {
         UINT32 timeoutMs=5000;
         CmsRet r2;
         CmsMsgHeader msg = EMPTY_MSG_HEADER;
         msg.src = EID_CONSOLED;
         msg.dst = EID_SMD;
         msg.type = CMS_MSG_GET_SHMID;
         msg.flags_request = 1;

         r2 = cmsMsg_sendAndGetReplyWithTimeout(msgHandle, &msg, timeoutMs);
         if (r2 == CMSRET_TIMED_OUT)  /* assumes shmId is never 9809, which is value of CMSRET_TIMED_OUT */
         {
            cmsLog_error("could not get shmId from smd (r2=%d) == drop to shell for debug", r2);
            spawnShellForDebug();
            /* once user has exited from shell, try to contact CMS smd again */
            continue;
         }

         shmId = (SINT32) r2;
         cmsLog_debug("got smdId=%d", shmId);
         cmsIsUp = TRUE;
      }
      else
      {
         /* shmId is correctly set, so we must have been launched by smd
          * with the "-m shmId" command line arg.
          * So CMS is up and we can continue.
          */
         cmsIsUp = TRUE;
      }
   }


   if ((ret = cmsMdm_initWithAcc(EID_CONSOLED, NDA_ACCESS_CONSOLED, msgHandle, &shmId)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not initialize mdm, ret=%d", ret);
      cmsMsg_cleanup(&msgHandle);
      cmsLog_cleanup();
      exit(-1);
   }

   initLoggingFromConfig(useConfiguredLogLevel);


   cmsCli_printWelcomeBanner();

   if (cmsCli_authenticate(NETWORK_ACCESS_CONSOLE, CONSOLED_EXIT_ON_IDLE_TIMEOUT) == CMSRET_SUCCESS)
   {
      cmsCli_run(msgHandle, CONSOLED_EXIT_ON_IDLE_TIMEOUT);
   }

   cmsMdm_cleanup();
   cmsMsg_cleanup(&msgHandle);
   cmsLog_cleanup();

#ifdef BUILD_BCMIPC
   omciIpc_clientShutdown();
#endif
   return 0;
}



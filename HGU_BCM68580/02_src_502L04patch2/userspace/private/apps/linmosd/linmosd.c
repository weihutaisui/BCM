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

#include <stdio.h>
#include <signal.h>
#include <errno.h>

#include "cms.h"
#include "cms_obj.h"
#include "cms_mdm.h"
#include "cms_lck.h"
#include "cms_mgm.h"
#include "cms_util.h"
#include "cms_msg.h"
#include "cms_msg_modsw.h"
#include "modsw.h"

#include "linmosd_private.h"

static char version_string[64];
static char *timestamp="miwang@vbhost 20121016_1823";  // move this to dynamic generated later
void *msgHandle=NULL;
UBOOL8 keepRunning=TRUE;
UBOOL8 saveConfigNeeded=TRUE;

static int linmosd_system_init(void);
static CmsRet linmosd_main_loop(void);
static void processCmsMsg(void);

static void usage(char *progName)
{
   /* use print because I don't know the state of the log (?) */
   printf("usage: %s [-v num] [-m shmId] \n", progName);
   printf("       v: set verbosity, where num==0 is LOG_ERROR, 1 is LOG_NOTICE, all others is LOG_DEBUG\n");
   printf("       m: shared memory id, -1 if standalone or not using shared mem.\n");
   exit(1);
}


static void initLoggingFromConfig(UBOOL8 useConfiguredLogLevel)
{
   LinmosdCfgObject *obj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   if ((ret = cmsLck_acquireLockWithTimeout(LINMOSD_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      cmsLck_dumpInfo();
      return;
   }

   if ((ret = cmsObj_get(MDMOID_LINMOSD_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of LINMOSD_CFG object failed, ret=%d", ret);
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


int main(int argc, char **argv)
{
   SINT32 c, logLevelNum;
   SINT32 shmId=UNINITIALIZED_SHM_ID;
   UBOOL8 useConfiguredLogLevel=TRUE;
   CmsRet ret;

   /* init log */
   cmsLog_initWithName(EID_LINMOSD, "linmosd");

   /* parse command line args */
   while ((c = getopt(argc, argv, "v:m:")) != -1)
   {
      switch(c)
      {
      case 'v':
      {
         CmsLogLevel logLevel;
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
      }

      case 'm':
         shmId = atoi(optarg);
         break;

      default:
         usage(argv[0]);
         break;
      }
   }

   snprintf(version_string, sizeof(version_string), "%d.%d.%d",
                LINMOSD_VERSION_MAJOR,
                LINMOSD_VERSION_MINOR,
                LINMOSD_VERSION_PATCH);
   printf("linmosd (version=%s  build_timestamp=%s)\n", version_string, timestamp);

   if (linmosd_system_init())
   {
      cmsLog_error("failed to initialize system, abort!");
      return -1;
   }

   /*
    * Detach myself from the terminal so I don't get any control-c/sigint.
    * On the desktop, it is smd's job to catch control-c and exit.
    * When osgid detects that smd has exited, it will also exit.
    */
   if (setsid() == -1)
   {
      cmsLog_error("Could not detach from terminal");
   }
   else
   {
      cmsLog_debug("detached from terminal");
   }

   /* set signal masks */
   signal(SIGPIPE, SIG_IGN);
   signal(SIGINT, SIG_IGN);

   if ((ret = cmsMsg_initWithFlags(EID_LINMOSD, 0, &msgHandle)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMsg_init failed, ret=%d", ret);
      return 0;
   }

   if ((ret = cmsMdm_initWithAcc(EID_LINMOSD, 0, msgHandle, &shmId)) != CMSRET_SUCCESS)
   {
      cmsMsg_cleanup(&msgHandle);
      cmsLog_error("cmsMdm_init error ret=%d", ret);
      return 0;
   }

   initLoggingFromConfig(useConfiguredLogLevel);

   ret = linmosd_main_loop();

   cmsLog_notice("linmosd is exiting now (ret=%d)", ret);

   cmsMdm_cleanup();
   cmsMsg_cleanup(&msgHandle);
   cmsLog_cleanup();

   return (int) ret;
}


int linmosd_system_init(void)
{
   CmsRet ret;

   ret = modsw_makeRequiredDirs();
   if (ret != CMSRET_SUCCESS)
   {
      return -1;
   }

   return 0;
}

CmsRet linmosd_main_loop()
{
   int comm_fd, max_fd;
   int nready;
   int errorRepeated=0;
   fd_set rset;
   CmsRet ret = CMSRET_SUCCESS;

   /* comm_fd to listen message from SMD */
   cmsMsg_getEventHandle(msgHandle,&comm_fd);
   max_fd = comm_fd;


   while (keepRunning)
   {
      FD_ZERO(&rset);
      FD_SET(comm_fd,&rset);

      nready = select(max_fd+1,&rset,NULL,NULL,NULL);
      if (nready == -1)
      {
         if (errorRepeated < 20)
         {
            /* errno-base.h, errno.h */
            cmsLog_notice("error on select, errno=%d",errno);
            errorRepeated++;
         }
         else
         {
            cmsLog_error("Too many errors from select (errno=%d), abort!", errno);
            keepRunning = 0;
            ret = CMSRET_INTERNAL_ERROR;
         }
         usleep(1000);
         continue;
      }
      errorRepeated = 0;
      ret = CMSRET_SUCCESS;

      if (FD_ISSET(comm_fd,&rset))
      {
         processCmsMsg();
      }
   }

   return ret;
}


void processCmsMsg()
{
   CmsMsgHeader *msg;
   CmsRet ret;
   UBOOL8 isGenericCmsMsg;

   while ((ret = cmsMsg_receiveWithTimeout(msgHandle, &msg,0)) == CMSRET_SUCCESS)
   {
      isGenericCmsMsg=TRUE;

      /*
       * Because Modular Software specific messages are now their own enum,
       * we have to do the switch(msg->type) on them separately or else
       * gcc 4.6 will complain about invalid enum value.
       */
      {
         CmsModSwMsgType modswMsgType = (CmsModSwMsgType) msg->type;
         switch(modswMsgType)
         {
         CmsRet r2;

         case CMS_MSG_REQUEST_DU_STATE_CHANGE:
            cmsLog_debug("got CMS_MSG_REQUEST_DU_STATE_CHANGE");
            r2 = processDuStateChange(msg);
            if (r2 != CMSRET_SUCCESS)
            {
               cmsLog_error("processDuStateChange returned %d", r2);
               /* surprisingly, httpd and tr69c do not expect a response
                * to this message.  I guess download could take a while, so
                * they will check back later.
                */
            }
            isGenericCmsMsg=FALSE;
            break;

         case CMS_MSG_REQUEST_EU_STATE_CHANGE:
            cmsLog_debug("got CMS_MSG_REQUEST_EU_STATE_CHANGE");
            r2 = processEuStateChange(msg);
            if (r2 != CMSRET_SUCCESS)
            {
               cmsLog_error("processEuStateChange returned %d", r2);
            }
            isGenericCmsMsg=FALSE;
            break;

         case CMS_MSG_START_EE:
            cmsLog_debug("got startEE msg, calling startExecEnv ");
            startExecEnv();
            isGenericCmsMsg=FALSE;
            break;

         case CMS_MSG_STOP_EE:
            cmsLog_debug("got stopEE msg, calling stopExecEnv ");
            stopExecEnv();
            isGenericCmsMsg=FALSE;
            break;

         default:
            /* could be a generic message, so just break and let the code
             * below look at msgType.
             */
            break;
         }
      }

      if (isGenericCmsMsg)
      {
         switch(msg->type)
         {
         case CMS_MSG_SYSTEM_BOOT:
            /*
             * only apps which set EIF_LAUNCH_IN_STAGE_1 or EIF_LAUNCH_ON_BOOT
             * should get this message.  Handle it here anyways as an example.
             */
            cmsLog_debug("got CMS_MSG_SYSTEM_BOOT");
            break;

         case CMS_MSG_INTERNAL_NOOP:
            /* just ignore this message. */
            break;

         case CMS_MSG_SET_LOG_LEVEL:
            cmsLog_debug("got set log level to %d", msg->wordData);
            cmsLog_setLevel(msg->wordData);
            if ((ret = cmsMsg_sendReply(msgHandle, msg, CMSRET_SUCCESS)) != CMSRET_SUCCESS)
            {
               cmsLog_error("send response for msg 0x%x failed, ret=%d", msg->type, ret);
            }
            break;

         case CMS_MSG_SET_LOG_DESTINATION:
            cmsLog_debug("got set log destination to %d", msg->wordData);
            cmsLog_setDestination(msg->wordData);
            if ((ret = cmsMsg_sendReply(msgHandle, msg, CMSRET_SUCCESS)) != CMSRET_SUCCESS)
            {
               cmsLog_error("send response for msg 0x%x failed, ret=%d", msg->type, ret);
            }
            break;

         default:
            cmsLog_error("unrecognized msg 0x%x from src=0x%x",
                         msg->type, msg->src);
            break;
         }
      }

      CMSMEM_FREE_BUF_AND_NULL_PTR(msg);
   } /* while */

   if (ret == CMSRET_DISCONNECTED)
   {
      if (!cmsFil_isFilePresent(SMD_SHUTDOWN_IN_PROGRESS))
      {
         cmsLog_error("lost connection to smd, exiting now.");
      }
      keepRunning = FALSE;
   }

   /*
    * While processing some of the modular software messages, the code might
    * have modified the MDM and needs to save it.  Do it here.
    */
   if (saveConfigNeeded)
   {
      if ((ret = cmsLck_acquireLockWithTimeout(LINMOSD_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get lock, ret=%d", ret);
         cmsLck_dumpInfo();
      }
      else
      {
         cmsMgm_saveConfigToFlash();
         saveConfigNeeded = FALSE;
         cmsLck_releaseLock();
      }
   }
}

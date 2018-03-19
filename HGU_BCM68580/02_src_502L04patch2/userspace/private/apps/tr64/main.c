/*----------------------------------------------------------------------*
 * Copyright (c) 2006-2012 Broadcom Corporation
 * All Rights Reserved
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
 *----------------------------------------------------------------------*
 * File Name  : main.c 
 *
 * Description:  entry point of CPE tr64 application
 *   
 *   
 *----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <syslog.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>


#include "upnp.h"
#include "tr64defs.h"
#include "upnp_dbg.h"
#include "session.h"

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"

void *msgHandle = NULL;

pSessionInfo pCurrentSession = NULL;
tr64PersistentData *pTr64Data = NULL;

extern DeviceTemplate IGDeviceTemplate;
extern void init_event_queue(int n);
void init_static_igd_devices(void);

static void reap(int sig)
{
   pid_t pid;

   while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) 
   {
      UPNP_TRACE(("Reaped %d\n", pid));
   }
}

void getCurrentState(pTr64PersistentData *pData)
{

   int count = 0;

   if(*pData == NULL)
   {
      *pData = malloc(sizeof(tr64PersistentData));
   }

   count = cmsPsp_get(TR64_STATE_PERSISTENT_TOKEN,*pData,sizeof(tr64PersistentData));

   if (count == 0)
   {
      if (*pData != NULL) 
      {
         memset(*pData,0,(sizeof(tr64PersistentData)));
         (*pData)->passwordState = FACTORY;
         strcpy((*pData)->password,TR64_DSLF_RESET_PWD);
      }
   } 
} 

void setCurrentState(pTr64PersistentData pData)
{

   int len = sizeof(tr64PersistentData);

   if (cmsPsp_set(TR64_STATE_PERSISTENT_TOKEN,(void*)pData,len) != CMSRET_SUCCESS)
   {
      cmsLog_error(("setCurrentState: unable to set scratch pad"));
   }
}

static void initLoggingFromConfig(UBOOL8 useConfiguredLogLevel)
{
   SnmpdCfgObject *obj;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   if ((ret = cmsObj_get(MDMOID_TR64C_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of MDMOID_TR64C_CFG object failed, ret=%d", ret);
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
}


int tr64_main(void)
{   
   syslog(LOG_DEBUG,"%s: entry\n",__FUNCTION__);

   init_event_queue(TR64_NUMBER_EVENT_QUEUE);

   /* We need to have a reaper for child processes we may create.
      That happens when we send signals to the dhcp process to
      release an renew a lease on the external interface. */
   signal(SIGCHLD, reap);

   getCurrentState(&pTr64Data);

   syslog(LOG_DEBUG,"pTr64Data %" PRIxPTR "\n",(intptr_t)pTr64Data);

   /* read from persistent storage */

   /* init devices, services, sub-devices and their services */ 
   init_static_igd_devices();

   upnp_main(TR64_LAN_INTF_NAME);
   return 0;
}

/*
 * main() Entry point of TR64 application.    
 */
int main(int argc, char** argv)
{
   SINT32      c, logLevelNum;
   SINT32      shmId=UNINITIALIZED_SHM_ID;
   CmsLogLevel logLevel=DEFAULT_LOG_LEVEL;
   UBOOL8      useConfiguredLogLevel=TRUE;
   CmsRet      ret;

   /* init log util */
   cmsLog_init(EID_TR64C);

   /* parse command line args */
   while ((c = getopt(argc, argv, "v:m:")) != -1)
   {
      switch(c)
      {
         case 'v':
            logLevelNum = atoi(optarg);

            cmsLog_debug("option v with logLevelNum %d",logLevelNum);

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

         case 'm':
            cmsLog_debug("option v with shmId %d",shmId);

            shmId = atoi(optarg);
            break;

         default:
            //            usage(argv[0]);
            break;
      }
   }

   /*
    * Detach myself from the terminal so I don't get any control-c/sigint.
    * On the desktop, it is smd's job to catch control-c and exit.
    * When tr69c detects that smd has exited, tr69c will also exit.
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
   signal(SIGPIPE, SIG_IGN); /* Ignore SIGPIPE signals */

   cmsLog_notice("calling cmsMsg_init");
   if ((ret = cmsMsg_init(EID_TR64C, &msgHandle)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMsg_init failed, ret=%d", ret);
      return 0;
   }

   cmsLog_notice("calling cmsMdm_init with shmId=%d", shmId);
   if ((ret = cmsMdm_init(EID_TR64C, msgHandle, &shmId)) != CMSRET_SUCCESS)
   {
      cmsMsg_cleanup(&msgHandle);
      cmsLog_error("cmsMdm_init error ret=%d", ret);
      return 0;
   }
   if ((ret = cmsLck_acquireLockWithTimeout(TR64C_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      return 0;
   }

   initLoggingFromConfig(useConfiguredLogLevel);

   cmsLck_releaseLock();

   tr64_main();

   return 1;
} /* main */



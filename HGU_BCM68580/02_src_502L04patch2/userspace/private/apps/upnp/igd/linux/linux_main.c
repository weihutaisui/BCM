/*
 *
 * Copyright (c) 2003-2012  Broadcom Corporation
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
 * $Id: linux_main.c,v 1.8.48.2 2003/10/31 21:31:36 mthawani Exp $
 */

#include <errno.h>	    // for errno, of course.
#include <error.h>	    // for perror
#include <signal.h>	    // for signal, etc.
#include <assert.h>	    // for assert, of course.
#include <stdlib.h>	    // for malloc, free, etc.
#include <string.h>	    // for memset, strncasecmp, etc.
#include <stdarg.h>	    // for va_list, etc.
#include <stdio.h>	    // for printf, perror, fopen, fclose, etc.
#include <net/if.h>	    // for struct ifreq, etc.
#include <sys/ioctl.h>	    // for SIOCGIFCONF, etc.
#include <fcntl.h>	    // for fcntl, F_GETFL, etc.
#include <unistd.h>	    // for read, write, etc.
#include <arpa/inet.h>	    // for inet_aton, inet_addr, etc.
#include <time.h>	    // for time
#include <netinet/in.h>	    // for sockaddr_in
#include <wait.h>	    // for sockaddr_in


#include "ctype.h"
#include "upnp_dbg.h"
#include "upnp.h"
#include "cms_log.h"
#include "cms_msg.h"
#include "cms_util.h"
#include "cms_core.h"

void *g_msgHandle = NULL;

/* voice library(dal_voice.c) expects msgHandle variable to be defined,
 * this has to be fixed in voice code, till then we will just maintian
 * the "msgHandle" variable
 */ 
void *msgHandle = NULL;

char *g_upnpAppName = NULL;

extern void define_variable(char *name, char *value);
extern void uuidstr_create(char *);
extern char *strip_chars(char *, char *);
extern void init_devices();
void init_event_queue(int);

extern struct net_connection *net_connections;
extern struct iface *global_lans;



static void
reap(int sig)
{
   pid_t pid;

   while ((pid = waitpid(-1, NULL, WNOHANG)) > 0)
      UPNP_TRACE(("Reaped %d\n", pid));
}

static void initLoggingFromConfig(UBOOL8 useConfiguredLogLevel)
{
   UpnpCfgObject *obj;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   if ((ret = cmsLck_acquireLockWithTimeout(UPNP_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      return;
   }

   if ((ret = cmsObj_get(MDMOID_UPNP_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of UPNP_CFG object failed, ret=%d", ret);
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

int main(int argc, char *argv[])
{
   extern DeviceTemplate IGDeviceTemplate;
   char **argp = &argv[1];
   char *wanif = NULL;
   char *wanif_l2 = NULL;
   char *lanif = NULL;
   int daemonize = 0;

   CmsLogLevel logLevel=DEFAULT_LOG_LEVEL;
   SINT32 logLevelNum;
   UBOOL8 useConfiguredLogLevel = TRUE;

   SINT32      shmId=UNINITIALIZED_SHM_ID;
   CmsRet      ret;

   /* init log util */
   cmsLog_initWithName(EID_UPNP, argv[0]);

   /*Intialize the app name */
   if (NULL == (g_upnpAppName = cmsMem_strdup(argv[0])))
   {
      cmsLog_error("strdup of name %s failed", argv[0]);
      exit(-1);
   }

   while (argp < &argv[argc]) {
      if (strcasecmp(*argp, "-L") == 0) {
         lanif = *++argp;
      } 
      else if (strcasecmp(*argp, "-W") == 0) {
         wanif = *++argp;
      } 
      else if (strcasecmp(*argp, "-W2") == 0) {
         wanif_l2 = *++argp;
      } 
      else if (strcasecmp(*argp, "-D") == 0) {
         daemonize = 1;
      }
#ifdef BCMDBG
      else if (strcmp(*argp, "-M") == 0) {
         upnp_msg_level = strtoul(*++argp, NULL, 0);
         printf("upnp_msg_level = 0x%x (%d)\n", upnp_msg_level, upnp_msg_level);
      }
#endif
      else if (strcmp(*argp, "-m") == 0) {
            shmId = atoi(*++argp);
      }
      else if (strcmp(*argp, "-mode") == 0) {
            mode = atoi(*++argp);
      }
      else if (strcmp(*argp, "-pcpsrv") == 0) {
            strcpy(pcpsrv, *++argp);
      }
      else if (strcmp(*argp, "-pcplocal") == 0) {
            strcpy(pcplocal, *++argp);
      }
      else if (strcasecmp(*argp, "-v") == 0) {
         logLevelNum = atoi(*++argp);
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
      }
      argp++;
   }



   /* init message util */
   if ((ret = cmsMsg_initWithFlags(EID_UPNP, 0, &g_msgHandle)) != CMSRET_SUCCESS)
   {
      cmsLog_cleanup();
      cmsLog_error("cmsMsg_init error ret=%d", ret);
      return 0;
   }
   
   /*just to keep dal_voice.c happy!! */
   msgHandle = g_msgHandle;

   cmsLog_notice("calling cmsMdm_init with shmId=%d", shmId);
   if ((ret = cmsMdm_initWithAcc(EID_UPNP, NDA_ACCESS_UPNP, g_msgHandle, &shmId)) != CMSRET_SUCCESS)
   {
      cmsMsg_cleanup(&g_msgHandle);
      cmsLog_cleanup();
      cmsLog_error("cmsMdm_init error ret=%d", ret);
      return 0;
   }

   /* set loglevel from MDM if not set from command line  */	
   initLoggingFromConfig(useConfiguredLogLevel);

   init_event_queue(40);

   if (lanif == NULL || wanif == NULL || wanif_l2 == NULL) {
	   fprintf(stderr, "usage: %s -L lan_ifname -W wan_ifname -W2 wan_l2ifname -m shmid\n", argv[0]);
   } else {
	   if (daemonize && daemon(1, 1) == -1) {
	      perror("daemon");
	      exit(errno);
      }

	   /* We need to have a reaper for child processes we may create.
	      That happens when we send signals to the dhcp process to
	      release an renew a lease on the external interface. */
      signal(SIGCHLD, reap);

      /* For some reason that I do not understand, this process gets
         a SIGTERM after sending SIGUSR1 to the dhcp process (to
         renew a lease).  Ignore SIGTERM to avoid being killed when
         this happens.  */
      //	signal(SIGTERM, SIG_IGN);
      signal(SIGUSR1, SIG_IGN);

      /*
       * ignore SIGPIPE so we don't die when trying to send a message on a broken socket.
       * This happens when smd sends a SIGTERM to UPnP to kill it.  UPnP then ends up
       * trying to send a message to smd, and gets the SIGPIPE.
       */
      signal(SIGPIPE, SIG_IGN);

      UPNP_TRACE(("calling upnp_main\n"));
      upnp_main(&IGDeviceTemplate, lanif, wanif, wanif_l2);
   }
    
   cmsMdm_cleanup();
   cmsMsg_cleanup(&g_msgHandle);
   cmsLog_cleanup();
   return 0;
}


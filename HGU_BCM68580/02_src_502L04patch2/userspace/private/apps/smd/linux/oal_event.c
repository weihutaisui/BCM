/***********************************************************************
 *
 *  Copyright (c) 2007  Broadcom Corporation
 *  All Rights Reserved
 *
 * <:label-BRCM:2007:proprietary:standard
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
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "cms.h"
#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"
#include "prctl.h"
#include "cms_boardcmds.h"
#include "cms_boardioctl.h"
#include "../oal.h"
#include "../event_interest.h"
#include "../sched_events.h"

/* from main.c */
extern UBOOL8 keepLooping;
extern SINT32 shmId;
extern UBOOL8 useConfiguredLogLevel;
extern SINT32 sync_smd_pid;


/* small hack, use the send and receive function in the oal layer of the msg lib. */
extern CmsRet oalMsg_send(SINT32 fd, const CmsMsgHeader *buf);
extern CmsRet oalMsg_receive(SINT32 fd, CmsMsgHeader **buf, UINT32 *timeout);



/** Max number of milliseconds between the communications socket connect and
 *  getting the APP_LAUNCHED message.
 */
#define CONNECT_TO_LAUNCH_MSG_TIMEOUT  200


/** Max number of milliseconds between the launch of the app and getting the
 *  APP_LAUNCHED message.
 */
#define LAUNCH_TO_LAUNCH_CONF_TIMEOUT  (2 * MSECS_IN_SEC)

/** Max time (in milliseconds) to wait for an app to terminate.
 */
#define SMD_TERMINATE_TIMEOUT   (5 * MSECS_IN_SEC)


/*!\enum DlsState
 * \brief States of a dynamically launched process, used in DlsInfo.state.
 */
typedef enum
{
   DLS_NOT_RUNNING=0,  /**< not running */
   DLS_LAUNCHED=1,     /**< launched, but waiting for confirmation */
   DLS_RUNNING=2,      /**< fully up and running. */
   DLS_TERMINATE_REQUESTED=3 /**< Requested termination, but waiting for confirmation. */

} DlsState;


/** Maximum length of optional arguments to a dynamically launched app. */
#define DLS_OPT_ARGS_LENGTH   128


/** This structure contains dynamic information about a dynamically 
 *  launched process.  Static information is contained in the CmsEntityInfo
 *  structure.
 */
typedef struct
{
   DlistNode      dlist;    /** handle for doubly-linked list */
   const CmsEntityInfo *eInfo;    /**< Pointer to entity info entry */
   DlsState       state;    /**< Current state of this dynamically launched process. */
   SINT32         serverFd; /**< Server/listening socket fd */
   SINT32         serverFd2; /**< Server/listening socket fd for https connection */
   SINT32         commFd;   /**< Unix domain TCP connection socket file
                             *   descriptor on the smd process for
                             * inter-process communication with the
                             * application process.
                             */
   SINT32          pid;          /**< pid of the app */
   UINT32          specificEid;  /**< if this app can have multiple instances, combined pid and eid */
   SINT32          numDelayedMsgRequested; /**< number of delayed msgs requested by this app */
   SINT32          numEventInterestRequested; /**< number of event interests requested by this app */
   CmsMsgHeader *  msgQueue; /**< queue of messages waiting to be delivered to this app. */
   char   optArgs[DLS_OPT_ARGS_LENGTH];  /**< additional dynamic args, usually sent in by restart msg */

} DlsInfo;



/** external data **/
extern SINT32        shmId;
extern SINT32        maxFd;

/** dlsInfoHead is the head of a doubly linked list of DlsInfo structs.
 * 
 * See cms_dlist.h
 */
DLIST_HEAD(dlsInfoHead);


SINT32   ipcListenFd;               /* SMD Unix domain IPC listening socket fd */
fd_set readFdsMaster;


/** private data **/

#ifdef DESKTOP_LINUX
UINT16 desktopFakePid = 30;
#endif

#define SMD_ZOMBIE_CHECK_INTERVAL       (30 * MSECS_IN_SEC)
static CmsTimestamp lastZombieCheckTs;

static UBOOL8  shutdownInProgress=FALSE;
DlsInfo *shutdownSourceDinfo=NULL;

static UINT32 isDataModelDevice2=0;

/** private functions **/
static CmsRet initDls(UINT32 stage);
static void cleanupDls(void);
static DlsInfo *insertDlsInfoEntry(CmsEntityId eid);
SINT32 initInetServerSocket(SINT32 domain, SINT32 port, SINT32 type, SINT32 backlog);
static SINT32 initUnixDomainServerSocket(void);
static void initLoggingFromConfig(UINT32 logSettings);

static DlsInfo *validateAppLaunchedMsg(const CmsMsgHeader *msg);
static void processLaunchConfirmation(DlsInfo *dInfo);
static DlsInfo *getExistingDlsInfo(CmsEntityId eid);
static DlsInfo *getNewDlsInfo(CmsEntityId eid);
static DlsInfo *getDlsInfo(CmsEntityId eid);
static DlsInfo *getDlsInfoByServerFd(SINT32 serverFd);
static DlsInfo *getDlsInfoByCommFd(SINT32 commFd);

static CmsRet getPathToApp(const char *name, char *buf, UINT32 len);

static void launchApp(DlsInfo *appInfo);
static void terminateApp(DlsInfo **appInfo);
static void createAppTerminatedMsg(CmsEntityId eid, SINT32 sigNum, SINT32 exitCode);
CmsRet terminateAppWithMsg(DlsInfo *dInfo);
static void collectApp(DlsInfo **appInfo);
static void collectZombies(void);

static void sendMessageByState(DlsInfo *dInfo, CmsMsgHeader **msg);
static void routeMessage(CmsMsgHeader **msg);
static void distributeEventMessage(CmsMsgHeader **msg, const DlsInfo *srcDInfo);

static void processMessage(DlsInfo *dInfo, CmsMsgHeader **msg);
static void processRegisterEventInterest(DlsInfo *dInfo, CmsMsgHeader **msg, UBOOL8 positive);
static void processRegisterDelayedMsg(DlsInfo *dInfo, CmsMsgHeader **msg, UBOOL8 positive);
static void stopApp(UINT32 specificEid);
static SINT32 startApp(CmsEntityId eid, const char *args, UINT32 argsLength);
static void processStartAppMsg(DlsInfo *dInfo, CmsMsgHeader **msg);
static void processRestartAppMsg(DlsInfo *dInfo, CmsMsgHeader **msg);
static void processStopAppMsg(DlsInfo *dInfo, CmsMsgHeader **msg);
static void processRebootMsg(DlsInfo *dInfo, CmsMsgHeader **msg);
static void processLoadImageStarting(DlsInfo *dInfo, CmsMsgHeader **msg);
static void processLoadImageDone(DlsInfo *dInfo, CmsMsgHeader **msg);
static void processGetConfigFile(DlsInfo *dInfo, CmsMsgHeader **msg);
static void processValidateConfigFile(DlsInfo *dInfo, CmsMsgHeader **msg);
static void processWriteConfigFile(DlsInfo *dInfo, CmsMsgHeader **msg);
static void processMsgSimpleFooter(DlsInfo *dInfo, CmsRet rv, CmsMsgHeader **msg);
static void processIsAppRuning(DlsInfo * dInfo, CmsMsgHeader **msg);
static void processGetShmId(DlsInfo * dInfo, CmsMsgHeader **msg);
static void processDumpEidInfo(DlsInfo * dInfo, CmsMsgHeader **msg);
static void processTerminateMsg(DlsInfo * dInfo, CmsMsgHeader **msg);
static void queueMsg(DlsInfo *dInfo, CmsMsgHeader **msg);



CmsRet oalEvent_init(void)
{
   UINT32 stage=1;
   CmsRet ret;

   interest_init();
   sched_init();

   FD_ZERO(&readFdsMaster);


   /*
    * initialize my own Unix domain listening socket for other apps to connect
    * to me.
    */
   if ((ipcListenFd = initUnixDomainServerSocket()) < 0)
   {
      return CMSRET_INTERNAL_ERROR;
   }

   FD_SET(ipcListenFd, &readFdsMaster);
   UPDATE_MAXFD(ipcListenFd);


   /*
    * Initialize dynamic launch entries for the stage 1 apps.
    */
   if ((ret = initDls(stage)) != CMSRET_SUCCESS)
   {
      close(ipcListenFd);
      return ret;
   }

   return CMSRET_SUCCESS;

}  /* End of oalEvent_init() */


CmsRet initDls(UINT32 stage)
{
   DlsInfo *dInfo=NULL;
   const CmsEntityInfo *eInfo;
   char fullPath[CMS_MAX_FULLPATH_LENGTH]={0};

   /*
    * Populate dlsInfo dlist in stages because a side effect of adding
    * an entry in the dlist is that we start monitoring the server fd
    * (if the app has one).  In stage 1, we only want to launch
    * ssk and let ssk finish initializing the MDM.  Once that is complete,
    * we can start monitoring server fd's and accepting connections from
    * other apps.  (This fixes a very small race condition where httpd may
    * connect to smd before ssk has finished initializing the MDM.)
    *
    * All apps which have server fd's or which are to be launched on boot
    * must be put into this list in this function.  Apps which are launched
    * via a request from the rcl handler functions will get an entry put in
    * for them on an as needed basis by smd.
    */


   /*
    * Go through the entire entityInfoArray from the first entity,
    * and look for entities that have the LAUNCH_IN_STAGE_1 flag set.
    * Doing it this way technically violates the information hiding of the
    * entityInfoArray, but now users do not have to modify this code when
    * they add a new entity.
    */
   eInfo = cmsEid_getFirstEntityInfo();

   if (stage == 1)
   {
      while (eInfo)
      {
         if ((eInfo->flags & EIF_LAUNCH_IN_STAGE_1) &&
             ((eInfo->path && cmsFil_isFilePresent(eInfo->path)) ||
              (CMSRET_SUCCESS == getPathToApp(eInfo->name, fullPath, sizeof(fullPath)-1))))
         {
            cmsLog_debug("inserting stage 1 entity: %s (%d)", eInfo->name, eInfo->eid);
            dInfo = insertDlsInfoEntry(eInfo->eid);
            if (dInfo == NULL)
            {
               cmsLog_error("failed to initialize dInfo for %s", eInfo->name);
               return CMSRET_INTERNAL_ERROR;
            }
         }

         eInfo = cmsEid_getNextEntityInfo(eInfo);
      }
   }
   else
   {
      /* initialize stage 2 apps (which include the launch-on-boot apps) */
      while (eInfo)
      {
         if ((eInfo->flags & (EIF_LAUNCH_ON_BOOT|EIF_SERVER)) &&
             ((eInfo->path && cmsFil_isFilePresent(eInfo->path)) ||
              (CMSRET_SUCCESS == getPathToApp(eInfo->name, fullPath, sizeof(fullPath)-1)) ||
             ((eInfo->altPath && cmsFil_isFilePresent(eInfo->altPath))))) 
         {
            if ((dInfo = getExistingDlsInfo(eInfo->eid)) != NULL)
            {
               /*
                * A server app, such as tr69c, could have been launched
                * during ssk initialization of the MDM.  So if there is
                * a dInfo structure already, don't insert it again.
                */
               cmsLog_debug("dInfo entry for %s already inserted", eInfo->name);
            }
            else
            {
               cmsLog_debug("inserting stage 2 entity: %s (%d)", eInfo->name, eInfo->eid);

               dInfo = insertDlsInfoEntry(eInfo->eid);
               if (dInfo == NULL)
               {
                  cmsLog_error("failed to initialize dInfo for %s", eInfo->name);
                  return CMSRET_INTERNAL_ERROR;
               }
            }
         }

         eInfo = cmsEid_getNextEntityInfo(eInfo);
      }
   
   } /* end of if (stage == 2) */

   return CMSRET_SUCCESS;
}


/** Allocate a new DlsInfo entry for the specified eid, initialize its
 *  fields, including creating server sockets if appropriate, and link it
 *  into the dlsInfo dlist.
 */
DlsInfo *insertDlsInfoEntry(CmsEntityId eid)
{
   SINT32 socketType;
   DlsInfo *dInfo;

   cmsLog_debug("eid=%d (0x%x)", eid, eid);

   if ((dInfo = cmsMem_alloc(sizeof(DlsInfo), ALLOC_ZEROIZE)) == NULL)
   {
      cmsLog_error("could not allocate DlsInfo for eid=%d", eid);
      return NULL;
   }

   dInfo->eInfo = cmsEid_getEntityInfoAutoCreate(GENERIC_EID(eid));
   if (dInfo->eInfo == NULL)
   {
      cmsLog_error("unrecognized eid %d", eid);
      cmsMem_free(dInfo);
      return NULL;
   }

   dInfo->state = DLS_NOT_RUNNING;
   dInfo->commFd = CMS_INVALID_FD;
   dInfo->serverFd = CMS_INVALID_FD;
   dInfo->serverFd2 = CMS_INVALID_FD;
   dInfo->pid = CMS_INVALID_PID;
   if (dInfo->eInfo->flags & (EIF_MULTIPLE_INSTANCES|EIF_MULTIPLE_THREADS))
   {
      /* For multi-instance or multi-thread entity, we need to refer to
       * the specificEid. For app that was not launched by smd, eid is
       * coming from the msg source of CMS_MSG_APP_LAUNCHED sent from
       * oalMsg_initWithFlags() where msg source was set to either eid
       * or tid+eid. Init specificEid to eid here to cover the case
       * where the app might not be launched by smd. It is ok to be
       * overwritten when the app is actually launched by smd later on.
       */
      dInfo->specificEid = eid;
   }

   if (dInfo->eInfo->flags & EIF_SERVER)
   {
      SINT32   domain;

      socketType = (dInfo->eInfo->flags & EIF_SERVER_TCP) ? SOCK_STREAM : SOCK_DGRAM;
      domain     = AF_INET;

   /* The desktop linux may not support IPV6. Use AF_INET6 as domain ONLY if DESKTOP_LINUX is NOT defined for IPV4 only desktop linux
   * If IPv6 is enabled in desktop linux, this portion "&& !defined(DESKTOP_LINUX)" below can be removed.
   */
   
#if (defined(SUPPORT_IPV6)  && !defined(DESKTOP_LINUX))
      domain     = (dInfo->eInfo->flags & EIF_IPV6) ? AF_INET6 : AF_INET;
#endif
      dInfo->serverFd = initInetServerSocket(domain,
                                             dInfo->eInfo->port,
                                             socketType,
                                             dInfo->eInfo->backLog);
      if (dInfo->serverFd == CMS_INVALID_FD)
      {
         cmsMem_free(dInfo);
         return NULL;
      }
      else
      {
         cmsLog_debug("server socket for %s opened at port %d fd=%d",
                      dInfo->eInfo->name,
                      dInfo->eInfo->port,
                      dInfo->serverFd);

         FD_SET(dInfo->serverFd, &readFdsMaster);
         UPDATE_MAXFD(dInfo->serverFd);
      }

      if (eid == EID_HTTPD)
      {
         dInfo->serverFd2 = initInetServerSocket(domain,
#ifdef DESKTOP_LINUX
                                                 CMS_DESKTOP_SERVER_PORT_OFFSET + HTTPD_PORT_SSL,
#else
                                                 HTTPD_PORT_SSL,
#endif /* DESKTOP_LINUX */
                                                 socketType,
                                                 dInfo->eInfo->backLog);
         if (dInfo->serverFd2 == CMS_INVALID_FD)
         {
            cmsMem_free(dInfo);
            return NULL;
         }
         else
         {
            cmsLog_debug("server socket for %s opened at port %d fd=%d",
                         dInfo->eInfo->name,
#ifdef DESKTOP_LINUX
                         CMS_DESKTOP_SERVER_PORT_OFFSET + HTTPD_PORT_SSL,
#else
                         HTTPD_PORT_SSL,
#endif /* DESKTOP_LINUX */
                         dInfo->serverFd2);

            FD_SET(dInfo->serverFd2, &readFdsMaster);
            UPDATE_MAXFD(dInfo->serverFd2);
         }
      }
   }

   /* prepending to the head is equivalent to append to tail */
   dlist_prepend((DlistNode *) dInfo, &dlsInfoHead);

   return dInfo;
}


void oal_launchOnBoot(UINT32 stage)
{
   DlsInfo *dInfo;

   cmsLog_notice("stage=%d", stage);


   dlist_for_each_entry(dInfo, &dlsInfoHead, dlist)
   {
      if (((stage == 1) && (dInfo->eInfo->flags & EIF_LAUNCH_IN_STAGE_1)) ||
          ((stage == 2) && (dInfo->eInfo->flags & EIF_LAUNCH_ON_BOOT)))
      {
         CmsMsgHeader *msg;

         /*
          * queue a SYSTEM_BOOT message on the msg queue for the process.
          * this will be delivered to the process once is connects back to smd.
          */
         if (dInfo->eInfo->flags & EIF_MESSAGING_CAPABLE)
         {
            msg = (CmsMsgHeader *) cmsMem_alloc(sizeof(CmsMsgHeader), ALLOC_ZEROIZE);
            if (msg == NULL)
            {
               cmsLog_error("malloc of msg header failed.");
               break;
            }

            /* fill in the message */
            msg->type = CMS_MSG_SYSTEM_BOOT;
            msg->src = EID_SMD;
            msg->dst = dInfo->eInfo->eid;
            msg->flags_event = 1;
         
            /*
             * An app which was marked as launch-on-boot during stage 2 could
             * have been already launched during ssk's call to cmsMdm_init.
             * So must use sendMessageByState to deliver the message, which
             * will either queue or send the message depending on the app's state.
             */
            sendMessageByState(dInfo, &msg);
         }

         /* see comment above */
         if (dInfo->state == DLS_NOT_RUNNING)
         {
            launchApp(dInfo);
         }
         else if (dInfo->state == DLS_TERMINATE_REQUESTED)
         {
            /* Hmm, this is a weird situation.  Should we start another copy now? */
            cmsLog_error("app in terminate requested state.  launching app again?");
            launchApp(dInfo);
         }
      }
   }

   return;
}


void cleanupDls()
{
   DlsInfo *dInfo;

   while (dlist_empty(&dlsInfoHead) == 0)
   {
      dInfo = (DlsInfo *) dlsInfoHead.next;

      if (dInfo == shutdownSourceDinfo)
      {
         /* unlink it but don't close it since we need to send a response */
         dlist_unlink((DlistNode *) dInfo);
      }
      else if (dInfo->state == DLS_NOT_RUNNING)
      {
         dlist_unlink((DlistNode *) dInfo);

         if (dInfo->serverFd != CMS_INVALID_FD)
         {
            close(dInfo->serverFd);
            dInfo->serverFd = CMS_INVALID_FD;
         }

         if (dInfo->serverFd2 != CMS_INVALID_FD)
         {
            close(dInfo->serverFd2);
            dInfo->serverFd2 = CMS_INVALID_FD;
         }

         cmsMem_free(dInfo);
      }
      else
      {
         // App is running, terminate it, which will also free dInfo
         terminateApp(&dInfo);
      }
   }

   return;
}


void oalEvent_cleanup(void)
{
   cleanupDls();

   close(ipcListenFd);
   ipcListenFd = CMS_INVALID_FD;
   unlink(SMD_MESSAGE_ADDR);

   sched_cleanup();
   interest_cleanup();
}


void initLoggingFromConfig(UINT32 logSettings)
{
   CmsLogLevel level;
   CmsLogDestination dest;

   /* logSettings was constructed by getSmdLogSettingsFromConfig in ssk.c */
   level = (CmsLogLevel) (logSettings & 0xffff);
   dest = (CmsLogDestination) ((logSettings >> 16) & 0xffff);

   if (useConfiguredLogLevel)
   {
      cmsLog_setLevel(level);
   }

   cmsLog_setDestination(dest);
}


/**   This function contains the main select loop for smd.
 *
 */
CmsRet oal_processEvents(void)
{
   struct timeval tm;
   fd_set         readFds;
   DlsInfo        *dInfo;
   SINT32         i;
   UINT32         sleepMs;
   const UINT32   sched_lag_time=10;
   SINT32         rv;
   CmsTimestamp   nowTs;


   /*
    * Reinitialize timer each iteration (you need to do this on Linux).
    * The key factor in deciding how long to sleep is when our next
    * timer event is supposed to be executed.  Get that from the "sched"
    * sub-system.  Add a SCHED_LAG_TIME (10ms) to make sure that when we
    * wake up, the next event will have definately expired.  If there are
    * no events, getTimeToNextEvent will return UINT32_MAX.
    */
   sleepMs = sched_getTimeToNextEvent();
   if (sleepMs < UINT32_MAX - sched_lag_time)
   {
      sleepMs += sched_lag_time;
   }
   cmsLog_debug("sleeping for %u ms", sleepMs);
      
   tm.tv_sec  = sleepMs / MSECS_IN_SEC;
   tm.tv_usec = (sleepMs % MSECS_IN_SEC) * USECS_IN_MSEC;


   /* set our bit masks according to the master */
   readFds = readFdsMaster;

   /* pend, waiting for one or more fds to become ready */
   rv = select(maxFd+1, &readFds, NULL, NULL, &tm);
   if (rv < 0)
   {
      if (errno != EINTR)
      {
         cmsLog_error("select returned %d errno=%d", rv, errno);
      }
      /* return success anyways, the main loop will call us again. */
      return CMSRET_SUCCESS;
   }
   else if (rv == 0)
   {
      /* timed out */
      return CMSRET_SUCCESS;
   }  


   /* step through fds array and act on fds that are ready */
   for (i = 0; i < maxFd+1; i++)
   {
      if (!FD_ISSET(i, &readFds))
      {
         continue;
      }

      /* there is activity on fd 'i', now I need to figure out who owns fd 'i' */
      cmsLog_debug("fd %d is set", i);


      /* check for launching apps based on activity on server socket */
      if ((dInfo = getDlsInfoByServerFd(i)) != NULL)
      {
         if (dInfo->state != DLS_NOT_RUNNING)
         {
            cmsLog_error("got activity on server socket while app %s is in state %d",
                         dInfo->eInfo->name, dInfo->state);
         }
         else
         {
            /* launch the application process */
            cmsLog_debug("got activity on server socket %d for %s", dInfo->serverFd, dInfo->eInfo->name);
            launchApp(dInfo);
         }
      }

      /* check for activity on the ipc comm sockets to the individual apps */
      if ((dInfo = getDlsInfoByCommFd(i)) != NULL)
      {
         CmsMsgHeader *msg;
         CmsRet ret;

         cmsLog_notice("detected message on fd %d from %s", dInfo->commFd, dInfo->eInfo->name);

         ret = oalMsg_receive(dInfo->commFd, &msg, NULL);
         if (ret == CMSRET_SUCCESS)
         {
            processMessage(dInfo, &msg);
         }
         else if (ret == CMSRET_DISCONNECTED)
         {
            CmsEntityId exitEid = dInfo->eInfo->eid;
            cmsLog_notice("detected exit of %s (pid=%d) on fd %d", dInfo->eInfo->name, dInfo->pid, dInfo->commFd);
            collectApp(&dInfo);
            if (exitEid == EID_SSK)
            {
               cmsLog_error("ssk has died.  smd must exit now!");
               return CMSRET_INTERNAL_ERROR;
            }
         }
         else
         {
            cmsLog_error("error on read from fd %d for %s", dInfo->commFd, dInfo->eInfo->name);
         }
      }

      /*
       * check for newly launched processes (either launched by smd or on
       * command line) connecting back to smd.
       */
      if (i == ipcListenFd)
      {
         struct sockaddr_un clientAddr;
         UINT32 sockAddrSize;
         SINT32 fd;
         CmsMsgHeader *msg=NULL;
         UINT32 timeout = CONNECT_TO_LAUNCH_MSG_TIMEOUT;
         CmsRet ret;

         sockAddrSize = sizeof(clientAddr);
         if ((fd = accept(ipcListenFd, (struct sockaddr *)&clientAddr, &sockAddrSize)) < 0)
         {
            cmsLog_error("accept IPC connection failed. errno=%d", errno);
         }
         else
         {
            cmsLog_debug("accepted new connection from app on fd %d", fd);

            /*
             * When a newly launched app calls cmsMsg_init(), that function will send
             * us an APP_LAUNCHED message to identify themself and to confirm that my
             * launchApp really worked.  Apps which do not use the CMS messaging system,
             * and therefore do not call cmsMsg_init(), are marked as RUNNING immediately.
             */
            ret = oalMsg_receive(fd, &msg, &timeout);
            if (ret != CMSRET_SUCCESS)
            {
               cmsLog_error("could not receive launch msg, ret=%d", ret);
               close(fd);
            }
            else
            {
               dInfo = validateAppLaunchedMsg(msg);
               if (NULL == dInfo)
               {
                  close(fd);
               }
               else
               {
                  cmsLog_debug("got APP_LAUNCHED from %s (eid=%d, pid=%d) on fd %d",
                               dInfo->eInfo->name, dInfo->eInfo->eid, dInfo->pid, fd);
                  dInfo->commFd = fd;
                  processLaunchConfirmation(dInfo);
                  /* mwang_todo: stop launch conf timer */
               }

               CMSMEM_FREE_BUF_AND_NULL_PTR(msg);
            }
         }
      }

      /*
       * Has it been SMD_ZOMBIE_CHECK_INTERVAL ms since we checked for Zombies?
       * Zombies occur when an app launched by smd does not create a
       * CMS Message handle/pipe back to smd.  As a result, smd does not
       * know if it exits.  I don't want to wake up smd just to check for
       * zombies because it will affect performance benchmarks, so only check
       * if smd is already awake doing other stuff.
       */
      cmsTms_get(&nowTs);
      if (SMD_ZOMBIE_CHECK_INTERVAL < cmsTms_deltaInMilliSeconds(&nowTs, &lastZombieCheckTs))
      {
         cmsTms_get(&lastZombieCheckTs);
         collectZombies();
      }

   }

   return CMSRET_SUCCESS;

}  /* End of oal_processEvents() */


/** Verify that this message is a good APP_LAUNCHED message
 *
 * @return dlsInfo if the message is good.
 */
DlsInfo *validateAppLaunchedMsg(const CmsMsgHeader *msg)
{
   DlsInfo *dInfo;
   UBOOL8 isNewDinfo;

   /* this must be the first message */
   if (msg->type != CMS_MSG_APP_LAUNCHED || msg->dst != EID_SMD)
   {
      cmsLog_error("wrong message type or dest eid");
      return NULL;
   }

   /*
    * getDlsInfo will create a new dlsInfo entry for this eid if
    * an entry for it does not exist.  This feature is useful
    * if a process is launched by something other than smd,
    * e.g. unittests or command line.
    */
    cmsLog_debug("new connection from src=0x%x (pid=%d)", msg->src, msg->wordData);
    if (NULL == (dInfo = getDlsInfo(msg->src)))
    {
       cmsLog_error("Could not get DlsInfo struct for eid=%d", msg->src);
       return NULL;
    }

    isNewDinfo = (dInfo->pid == CMS_INVALID_PID);

    /* next block is expected pid check */
    if (dInfo->eInfo->flags & EIF_MULTIPLE_INSTANCES)
    {
       /*
        * I cannot think of a way to verify this case.
        * I do not know if the app was launched by smd or not.  If not
        * launched by smd, then this will be a new dInfo entry with no info
        * for me to check against.  But if it was
        * launched by smd, but with an unexpected pid or specific eid, then
        * it will still look like a new dInfo entry with no info for me to
        * check against.
        */
    }
    else
    {
       if (isNewDinfo)
       {
          /*
           * This app was not launched by smd, so I don't know what pid
           * to expect.  No checking can be done here.
           */
       }
       else
       {
          /*
           * This app was launched by smd, so we should have an existing
           * dInfo entry and the pid should match.
           */
           if (msg->wordData != (UINT32) dInfo->pid)
           {
              cmsLog_notice("expected pid %d but got %d for %s, ignore this one",
                           dInfo->pid, msg->wordData, dInfo->eInfo->name);
#ifndef DESKTOP_LINUX
              return NULL;
#endif
           }
       }
    }

    /* We should not already have a commfd to this app */
    if (dInfo->commFd != CMS_INVALID_FD)
    {
       cmsLog_error("dInfo for %s already has a commFd %d, ignore this one",
                     dInfo->eInfo->name, dInfo->commFd);
       return NULL;
    }

    return dInfo;
}


/** Now that an app has confirmed its launch, send any queued messages we have
 *  waiting for it.
 */
void processLaunchConfirmation(DlsInfo *dInfo)
{
   CmsMsgHeader *msg;

   dInfo->state = DLS_RUNNING;
   cmsLog_debug("%s (eid=%d) transitioning to state=%d",
                dInfo->eInfo->name, dInfo->eInfo->eid, dInfo->state);

   /* start monitoring this fd */
   FD_SET(dInfo->commFd, &readFdsMaster);
   UPDATE_MAXFD(dInfo->commFd);

   while ((msg = dInfo->msgQueue) != NULL)
   {
      dInfo->msgQueue = dInfo->msgQueue->next;
      msg->next = NULL;
      cmsLog_debug("sending queued msg 0x%x", msg->type);
      oalMsg_send(dInfo->commFd, msg);

      CMSMEM_FREE_BUF_AND_NULL_PTR(msg);
   }
}


DlsInfo *getExistingDlsInfo(CmsEntityId eid)
{ 
   DlsInfo  *dInfo;

   dlist_for_each_entry(dInfo, &dlsInfoHead, dlist)
   {
      if (dInfo->eInfo->flags & (EIF_MULTIPLE_INSTANCES|EIF_MULTIPLE_THREADS))
      {
         /*
          * This entity can have multiple instances or threads, so must 
          * match on the specificEid field (which includes the pid or tid).
          */
         if (dInfo->specificEid == eid)
         {
            return dInfo;
         }
      }
      else
      {
         if (dInfo->eInfo->eid == eid)
         {
            return dInfo;
         }
      }
   }

   return NULL;
}

DlsInfo *getNewDlsInfo(CmsEntityId eid)
{ 
   cmsLog_notice("Creating dynamically allocated dlsInfo entry for eid=0x%x", eid);
   return (insertDlsInfoEntry(eid));
}

DlsInfo *getDlsInfo(CmsEntityId eid)
{ 
   DlsInfo  *dInfo;

   if ((dInfo = getExistingDlsInfo(eid)) != NULL)
   {
      return dInfo;
   }

   /*
    * this eid does not have an entry created at startup.  No problem.
    * create one now and add it to the list.
    */
   return getNewDlsInfo(eid);
}

DlsInfo *getDlsInfoByServerFd(SINT32 serverFd)
{ 
   DlsInfo  *dInfo;

   dlist_for_each_entry(dInfo, &dlsInfoHead, dlist)
   {
      if (dInfo->serverFd == serverFd ||
          dInfo->serverFd2 == serverFd)
      {
         return dInfo;
      }
   }

   return NULL;
}

DlsInfo *getDlsInfoByCommFd(SINT32 commFd)
{ 
   DlsInfo  *dInfo;

   dlist_for_each_entry(dInfo, &dlsInfoHead, dlist)
   {
      if (dInfo->commFd == commFd)
      {
         return dInfo;
      }
   }

   return NULL;
}


CmsRet getPathToApp(const char *name, char *buf, UINT32 len)
{
   char envPathBuf[BUFLEN_1024]={0};
   char rootDirBuf[CMS_MAX_FULLPATH_LENGTH]={0};
   UINT32 i=0;
   UINT32 envPathLen;
   SINT32 rc;
   char *start;
   CmsRet ret;

   ret = cmsUtl_getRunTimeRootDir(rootDirBuf, sizeof(rootDirBuf));
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsUtl_getRunTimeRootDir returns error ret=%d", ret);
      return ret;
   }

   rc = snprintf(envPathBuf, sizeof(envPathBuf), "%s", getenv("PATH"));
   if (rc >= (SINT32) sizeof(envPathBuf))
   {
      cmsLog_error("envPathBuf (size=%d) is too small to hold PATH env %s",
                   sizeof(envPathBuf), getenv("PATH"));
      return CMSRET_RESOURCE_EXCEEDED;
   }

   envPathLen = strlen(envPathBuf);
   start = envPathBuf;

   while (i <= envPathLen)
   {
      if (envPathBuf[i] == ':' || envPathBuf[i] == '\0')
      {
         envPathBuf[i] = 0;
         rc = snprintf(buf, len, "%s%s/%s", rootDirBuf, start, name);
         if (rc >= (SINT32) len)
         {
            cmsLog_error("CMS_MAX_FULLPATH_LENGTH (%d) exceeded on path %s",
                         CMS_MAX_FULLPATH_LENGTH, buf);
            return CMSRET_RESOURCE_EXCEEDED;
         }

         if (cmsFil_isFilePresent(buf))
         {
            return CMSRET_SUCCESS;
         }
         i++;
         start = &(envPathBuf[i]);
      }
      else
      {
         i++;
      }
   }

   cmsLog_debug("not found==>%s", name);
   return CMSRET_OBJECT_NOT_FOUND;
}


void launchApp(DlsInfo *dInfo)
{
   SpawnProcessInfo     spawnInfo;
   SpawnedProcessInfo   procInfo;
   char                 exeBuf[CMS_MAX_FULLPATH_LENGTH]={0};
   char                 argsBuf[BUFLEN_1024]={0};
   CmsRet               ret;

#ifdef DESKTOP_LINUX
   if ((dInfo->eInfo->flags & EIF_DESKTOP_LINUX_CAPABLE) == 0)
   {
      cmsLog_notice("pretend to launch %s, args=%s", exeBuf, argsBuf);
      dInfo->state = DLS_RUNNING;
      desktopFakePid++;
      dInfo->pid = desktopFakePid;
      if (dInfo->eInfo->flags & EIF_MULTIPLE_INSTANCES)
      {
         dInfo->specificEid = MAKE_SPECIFIC_EID(desktopFakePid, dInfo->eInfo->eid);
      }
      /* todo for multi-thread */
      return;
   }
#endif

   dInfo->pid = CMS_INVALID_PID;

   if (isDataModelDevice2 &&
       (dInfo->eInfo->flags & EIF_USE_ALTPATH_FOR_TR181))
   {
      cmsLog_debug("Detected EIF_USE_ALTPATH_FOR_TR181 flag, altpath=%s",
                    dInfo->eInfo->altPath);
      if (dInfo->eInfo->altPath)
      {
         snprintf(exeBuf, sizeof(exeBuf), "%s", dInfo->eInfo->altPath);
      }
      else
      {
         cmsLog_error("altPath is NULL!  app %s not launched", dInfo->eInfo->name);
         return;
      }
   }
   else if (dInfo->eInfo->path)
   {
#ifdef WLAN_UNIFIED_WLMNGR
     if(dInfo->eInfo->eid == EID_WLMNGR && dInfo->eInfo->altPath) 
       snprintf(exeBuf, sizeof(exeBuf), "%s", dInfo->eInfo->altPath);
     else
#endif
      snprintf(exeBuf, sizeof(exeBuf), "%s", dInfo->eInfo->path);
   }
   else
   {
      if (CMSRET_SUCCESS != getPathToApp(dInfo->eInfo->name, exeBuf, sizeof(exeBuf)-1))
      {
         cmsLog_error("Could not find requested app %s (eid=%d), not launched!",
               dInfo->eInfo->name, dInfo->eInfo->eid);
         return;
      }
   }

   if (dInfo->eInfo->flags & EIF_MDM)
   {
      snprintf(argsBuf, sizeof(argsBuf), "-m %d", shmId);
   }

   if (dInfo->eInfo->runArgs)
   {
      UINT32 alen = strlen(argsBuf);
      if (alen > 0)
      {
         snprintf(&argsBuf[alen], sizeof(argsBuf)-alen, " %s", dInfo->eInfo->runArgs);
      }
      else
      {
         snprintf(argsBuf, sizeof(argsBuf), "%s", dInfo->eInfo->runArgs);
      }
   }

   if (dInfo->optArgs[0] != 0)
   {
      UINT32 index, remaining, wrote;

      index = strlen(argsBuf);
      remaining = sizeof(argsBuf) - index;
      wrote = snprintf(&(argsBuf[index]), remaining, " %s", dInfo->optArgs);
      if (wrote >= remaining)
      {
         cmsLog_error("args buf overflow, wrote=%d remaining=%d (%s)", wrote, remaining, argsBuf);
         /* the app will probably not start correctly because the command line
          * is messed up.  All we can do is report it at this point.  */
      }
   }

#if defined(DESKTOP_LINUX) && !defined(BUILD_DESKTOP_BEEP)
   /*
    * We need to change the exeBuf path to point to the CommEngine
    * build directory.
    */
   ret = cmsUtl_getBaseDir(exeBuf, sizeof(exeBuf));
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not determine baseDir");
      return;
   }

   /* Most apps that we spawn in the DESKTOP are here */
   sprintf(&(exeBuf[strlen(exeBuf)]), "/userspace/private/apps/%s/%s",
           dInfo->eInfo->name, dInfo->eInfo->name);

   if (!cmsFil_isFilePresent(exeBuf))
   {
      cmsLog_debug("not at %s", exeBuf);

      /* check alternate locations for the desktop app */
      cmsUtl_getBaseDir(exeBuf, sizeof(exeBuf));
      sprintf(&(exeBuf[strlen(exeBuf)]), "/userspace/public/apps/%s/%s",
              dInfo->eInfo->name, dInfo->eInfo->name);

      if (!cmsFil_isFilePresent(exeBuf))
      {
         cmsLog_debug("could not find %s at %s", dInfo->eInfo->name, exeBuf);

         /* finally, to check targets/{profile}/fs.install/bin/ */
         if (CMSRET_SUCCESS != getPathToApp(dInfo->eInfo->name, exeBuf, sizeof(exeBuf)-1))
         {
            cmsLog_error("could not find %s at any location, give up",
                          dInfo->eInfo->name);
            return;
         }
      }
   }
#endif


   memset(&spawnInfo, 0, sizeof(spawnInfo));

   spawnInfo.exe = exeBuf;
   spawnInfo.args = argsBuf;
   spawnInfo.spawnMode = SPAWN_AND_RETURN;
   spawnInfo.stdinFd = 0;
   spawnInfo.stdoutFd = 1;
   spawnInfo.stderrFd = 2;
   spawnInfo.serverFd = dInfo->serverFd;
   if (dInfo->eInfo->eid == EID_HTTPD)
   {
      spawnInfo.serverFd2 = dInfo->serverFd2;
   }
   spawnInfo.maxFd = maxFd;
   spawnInfo.inheritSigint = TRUE;
   if (dInfo->eInfo->flags & EIF_SET_SCHED)
   {
      spawnInfo.setSched = 1;
      spawnInfo.schedPolicy = dInfo->eInfo->schedPolicy;
      spawnInfo.schedPriority = dInfo->eInfo->schedPriority;
   }
   if (dInfo->eInfo->flags & EIF_SET_CPUMASK)
   {
      spawnInfo.cpuMask = dInfo->eInfo->cpuMask;
   }
#ifdef SUPPORT_CGROUPS
   if (dInfo->eInfo->flags & EIF_SET_CGROUPS)
   {
      cmsLog_debug("setting %s cpuGroupName %s",
                   dInfo->eInfo->name, dInfo->eInfo->cpuGroupName);
      spawnInfo.cpuGroupName = dInfo->eInfo->cpuGroupName;
   }
#endif

   memset(&procInfo, 0, sizeof(procInfo));

   cmsLog_debug("spawning %s args %s", spawnInfo.exe, spawnInfo.args);
   ret = prctl_spawnProcess(&spawnInfo, &procInfo);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not spawn child %s args %s", exeBuf, argsBuf);
      /* smd timer will detect the child is not running */
   }
   else
   {
      dInfo->pid = procInfo.pid;
      if (dInfo->eInfo->flags & EIF_MULTIPLE_INSTANCES)
      {
         dInfo->specificEid = MAKE_SPECIFIC_EID(procInfo.pid, dInfo->eInfo->eid);
      }

      /* why not set specificEid to the generic eid when the app does not support multiple instances?
       * It will reduce a lot of checking elsewhere in the code.
       */
       
      cmsLog_debug("%s launched, pid %d", dInfo->eInfo->name, dInfo->pid);
   }


   if (dInfo->eInfo->flags & EIF_MESSAGING_CAPABLE)
   {
      dInfo->state = DLS_LAUNCHED;
   }
   else
   {
      cmsLog_debug("%s is not message capable, mark it as RUNNING without conf.", dInfo->eInfo->name);
      dInfo->state = DLS_RUNNING;
   }


   /* once we launch an app, stop monitoring its fd */
   if (dInfo->eInfo->flags & EIF_SERVER)
   {
      FD_CLR(dInfo->serverFd, &readFdsMaster);
      if (dInfo->eInfo->eid == EID_HTTPD)
         FD_CLR(dInfo->serverFd2, &readFdsMaster);
   }

   return;
}


/** Terminate and collect the app.
 * 
 *  This is a synchronous function, meaning that it will not return until the app
 *  has been collected and data structures cleaned up.  This function must not
 *  return before everything has been cleaned up because the caller may immediately
 *  start the same app, so we could possibly have one app on the way out and
 *  the same app being launched, confusion could result.  So play it safe (at the
 *  expense of some lag time) and make sure the app is fully exited and cleaned
 *  up before returning from here.
 */
void terminateApp(DlsInfo **dInfo)
{
   CmsRet ret=CMSRET_TIMED_OUT;

   cmsLog_debug("name=%s pid=%d state=%d", (*dInfo)->eInfo->name, (*dInfo)->pid, (*dInfo)->state);

#ifdef DESKTOP_LINUX
   if (((*dInfo)->eInfo->flags & EIF_DESKTOP_LINUX_CAPABLE) == 0)
   {
      /* this process was never launched, just pretend to collect it. */
      cmsLog_debug("pretend to terminate and collect %s", (*dInfo)->eInfo->name);
      (*dInfo)->state = DLS_NOT_RUNNING;
      return;
   }
#endif 

   if ((*dInfo)->state == DLS_NOT_RUNNING)
   {
      cmsLog_debug("app is not running, do nothing");
      return;
   }

   switch ( (*dInfo)->eInfo->eid )
   {
   case EID_PPP:
      /*
       * Special case for PPP.  Because we are terminating from smd,
       * it means rcl handler function has requested it, which means
       * the connection is being deleted.  Send SIGWINCH to ppp so that
       * it will erase its session info from the persistent scratch pad.
       */
      prctl_signalProcess((*dInfo)->pid, SIGWINCH);
      break;

   case EID_VOICE:
      /*
       * Special case for voice. We want to reserve SIGTERM only
       * for case when voice is deinitilized through shell command
       * line (kill or reboot). For normal deinitialization we 
       * will apply other non-used signal.
       */
      prctl_signalProcess((*dInfo)->pid, SIGUSR1);
      break;

   default:
      /*
       * If app supports CMS_MSG_TERMINATE, send that message to it.
       * If that does not work, or app does not support that msg, then
       * terminate "forcefully", i.e. by signal.
       */
      if ((*dInfo)->eInfo->flags & EIF_MSG_TERMINATE)
      {
         ret = terminateAppWithMsg(*dInfo);
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("%s returned error %d to CMS_MSG_TERMINATE.",
                  (*dInfo)->eInfo->name, ret);
         }
      }

      if (ret != CMSRET_SUCCESS)
      {
         if ((*dInfo)->pid != CMS_INVALID_PID)
         {
            prctl_terminateProcessForcefully((*dInfo)->pid);
         }
      }
      break;
   }

   collectApp(dInfo);

   return;
}


CmsRet terminateAppWithMsg(DlsInfo *dInfo)
{
   CmsMsgHeader msg = EMPTY_MSG_HEADER;
   CmsRet ret=CMSRET_TIMED_OUT;

   msg.type = CMS_MSG_TERMINATE;
   msg.src = EID_SMD;
   msg.dst = (dInfo->eInfo->flags & (EIF_MULTIPLE_INSTANCES|EIF_MULTIPLE_THREADS)) ?
                      dInfo->specificEid : dInfo->eInfo->eid;
   msg.flags_request = 1;

   if (CMSRET_SUCCESS == oalMsg_send(dInfo->commFd, &msg))
   {
      CmsMsgHeader *rxMsg=NULL;
      UINT32 timeout = SMD_TERMINATE_TIMEOUT;

      if (CMSRET_SUCCESS == oalMsg_receive(dInfo->commFd, &rxMsg, &timeout))
      {
         ret = rxMsg->wordData;
         CMSMEM_FREE_BUF_AND_NULL_PTR(rxMsg);
      }
   }

   return ret;
}


void createAppTerminatedMsg(CmsEntityId eid, SINT32 sigNum, SINT32 exitCode)
{
   CmsMsgHeader *msg;
   void *msgBuf;
   UINT32 msgDataLen = sizeof(appTermiatedMsgBody);
   appTermiatedMsgBody*appTerminatedMsg = NULL; 

   if (shutdownInProgress)
   {
      cmsLog_debug("shutdownInProgress, don't distribute msg for eid=%d", eid);
      return;
   }

   cmsLog_debug("eid=%d, signNum=%d, exitCode=%d", eid, sigNum, exitCode);

   if ((msgBuf = cmsMem_alloc(sizeof(CmsMsgHeader) + msgDataLen, ALLOC_ZEROIZE)) == NULL)
   {
      cmsLog_error("Could not allocate %u bytes", sizeof(CmsMsgHeader) + msgDataLen);
      return;
   } 
   
   msg = (CmsMsgHeader *)msgBuf;
   msg->type = CMS_MSG_APP_TERMINATED;
   msg->flags_request = 0;
   msg->flags_response = 0;
   msg->flags_event = 1;
   msg->flags_bounceIfNotRunning = 1;
   msg->dataLength = msgDataLen;
   
   appTerminatedMsg = (appTermiatedMsgBody *) (msg + 1);
   appTerminatedMsg->eid = eid;
   appTerminatedMsg->sigNum = sigNum;
   appTerminatedMsg->exitCode = exitCode;
   

   distributeEventMessage(&msg, NULL);
   
   cmsLog_debug("Done distributeEventMessage CMS_MSG_APP_TERMINATED");

   return;
}


void collectApp(DlsInfo **dInfo)
{
   CollectProcessInfo collectInfo;
   SpawnedProcessInfo procInfo;
   CmsMsgHeader *msg;
   CmsRet ret;


   /*
    * Free any messages that are queued up for this app before
    * we (potentially) free the dInfo struct.
    */
   while ((msg = (*dInfo)->msgQueue) != NULL)
   {
      (*dInfo)->msgQueue = (*dInfo)->msgQueue->next;
      msg->next = NULL;
      CMSMEM_FREE_BUF_AND_NULL_PTR(msg);
   }
   

   if ((*dInfo)->commFd != CMS_INVALID_FD)
   {
      FD_CLR((*dInfo)->commFd, &readFdsMaster);
      close((*dInfo)->commFd);
      (*dInfo)->commFd = CMS_INVALID_FD;
   }


   /*
    * Collect the process if smd was the one that launched it.
    * If the process started by itself (e.g. tftp), its pid will be 
    * CMS_INVALID_PID, so smd should not bother collecting it.
    */
   if ((*dInfo)->pid == CMS_INVALID_PID)
   {
      cmsLog_debug("Do not collect %s, it was not launched by smd", (*dInfo)->eInfo->name);
   }
   else
   {
      collectInfo.collectMode = COLLECT_PID_TIMEOUT;
      collectInfo.pid = (*dInfo)->pid;
      collectInfo.timeout = SMD_TERMINATE_TIMEOUT;
      
      if ( (*dInfo)->eInfo->eid == EID_VOICE )
      {
         /* Voice can take much longer to shutdown. */
         collectInfo.timeout = 2 * SMD_TERMINATE_TIMEOUT;
      }      

#ifdef OMCIPROV_WORKAROUND
      if ( (*dInfo)->eInfo->eid == EID_PPP )
      { 
        /* PPP does not shutdown gracefully, SMD has to SIGKILL it and introduces delay. */
         collectInfo.timeout = 2 * MSECS_IN_SEC; 
      }
#endif /* OMCIPROV_WORKAROUND */

      if ((ret = prctl_collectProcess(&collectInfo, &procInfo)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not collect %s (pid=%d timeout=%dms), ret=%d.  Kill it with SIGKILL.",
                      (*dInfo)->eInfo->name, (*dInfo)->pid, collectInfo.timeout, ret);
         /*
          * Send SIGKILL and collect the process, otherwise,
          * we end up with a zombie process.
          */
         prctl_signalProcess((*dInfo)->pid, SIGKILL);
         if ((ret = prctl_collectProcess(&collectInfo, &procInfo)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Still could not collect %s (pid=%d) after SIGKILL, ret=%d",
                         (*dInfo)->eInfo->name, (*dInfo)->pid, ret);
            /* this process is really stuck.  Not much I can do now.
             * Just leave it running I guess. */
         }
         else
         {
            cmsLog_debug("collected %s (pid %d) after SIGKILL", (*dInfo)->eInfo->name, (*dInfo)->pid);
            /* need create and distribute  the app termicated message to event registered applications */
            createAppTerminatedMsg((*dInfo)->eInfo->eid, procInfo.signalNumber, procInfo.exitCode);              
         }
      }
      else
      {
         cmsLog_debug("collected %s (pid %d) signalNum=%d", (*dInfo)->eInfo->name, (*dInfo)->pid, procInfo.signalNumber);

         /* need create and distribute  the app termicated message to event registered applications */
         createAppTerminatedMsg((*dInfo)->eInfo->eid, procInfo.signalNumber, procInfo.exitCode);  
         
         if (procInfo.signalNumber == SIGILL ||
             procInfo.signalNumber == SIGABRT ||
             procInfo.signalNumber == SIGFPE ||
             procInfo.signalNumber == SIGSEGV ||
             procInfo.signalNumber == SIGPIPE ||
             procInfo.signalNumber == SIGINT ||
             procInfo.signalNumber == SIGKILL ||
             procInfo.signalNumber == SIGBUS ||
             procInfo.signalNumber == SIGXCPU ||
             procInfo.signalNumber == SIGXFSZ)
         {
            cmsLog_error("%s (pid %d) exited due to uncaught signal number %d", 
                         (*dInfo)->eInfo->name,
                         (*dInfo)->pid,
                         procInfo.signalNumber);

            if ((*dInfo)->eInfo->flags & EIF_AUTO_RELAUNCH)
            {
               cmsLog_error("Auto relaunching %s", (*dInfo)->eInfo->name);
               launchApp(*dInfo);
               return;
            }
         }
      }
   }


   /* start monitoring server fd again (if this app is a server) */
   if ((*dInfo)->eInfo->flags & EIF_SERVER)
   {
      FD_SET((*dInfo)->serverFd, &readFdsMaster);
      if ((*dInfo)->eInfo->eid == EID_HTTPD)
         FD_SET((*dInfo)->serverFd2, &readFdsMaster);
   }


   /*
    * Free this dInfo if
    * - it does not monitor a server fd, and
    * - it is for a multiple instance or thread app or it cannot have multiple instances or threads
    *   and it currently has no registration for event interests and delayed messages.
    *
    * Multiple instance or thread apps cannot hold on to event interest or
    * delayed messages once they exit, so we always delete their
    * dInfo structures.
    */
   if ((((*dInfo)->eInfo->flags & EIF_SERVER) == 0) &&
       (((*dInfo)->eInfo->flags & (EIF_MULTIPLE_INSTANCES|EIF_MULTIPLE_THREADS)) ||
        ((((*dInfo)->eInfo->flags & (EIF_MULTIPLE_INSTANCES|EIF_MULTIPLE_THREADS)) == 0) &&
         ((*dInfo)->numDelayedMsgRequested == 0) &&
         ((*dInfo)->numEventInterestRequested == 0))))
   {
      CmsEntityId eid = ((*dInfo)->eInfo->flags & (EIF_MULTIPLE_INSTANCES|EIF_MULTIPLE_THREADS)) ?
                        (*dInfo)->specificEid : (*dInfo)->eInfo->eid;
      cmsLog_debug("unlink and free dInfo structure at %p for %s eid=%d (0x%x)", (*dInfo), (*dInfo)->eInfo->name, eid, eid);
      interest_unregisterAll(eid);
      dlist_unlink((DlistNode *) (*dInfo));
      CMSMEM_FREE_BUF_AND_NULL_PTR((*dInfo));
   }
   else
   {
      /* we did not free dInfo, clear some key fields */
      (*dInfo)->state = DLS_NOT_RUNNING;
      (*dInfo)->pid = CMS_INVALID_PID;
      (*dInfo)->specificEid = 0;   
   }

#ifdef BRCM_VOICE_SUPPORT
   /* Unregister interest in all events when voice shuts down */
   if ( ((*dInfo) != NULL) && ((*dInfo)->eInfo->eid == EID_VOICE) )
   {
      interest_unregisterAll((*dInfo)->eInfo->eid);
   }
#endif /* BRCM_VOICE_SUPPORT */

   return;
}


void collectZombies()
{
   DlsInfo  *dInfo;
   int items;

   cmsLog_notice("Entered");

   dlist_for_each_entry(dInfo, &dlsInfoHead, dlist)
   {
      if ((dInfo->pid != 0) &&
          (dInfo->state == DLS_RUNNING || dInfo->state == DLS_LAUNCHED))
      {
         char filename[BUFLEN_128];
         char state[BUFLEN_128];
         char cmd[BUFLEN_256];
         int pid;
         FILE *fp;

         snprintf(filename, sizeof(filename)-1, "/proc/%d/stat", dInfo->pid);
         if (NULL == (fp = fopen(filename, "r")))
         {
            cmsLog_error("could not open %s", filename);
            continue;
         }
         items = fscanf(fp, "%d %s %s", &pid, cmd, state);
         fclose(fp);
         cmsLog_debug("items=%d checking %s state=%s", items, filename, state);
         if (items >= 3 && state[0] == 'Z')
         {
            cmsLog_debug("found zombie: cmd=%s pid=%d", cmd, pid);
            collectApp(&dInfo);
            /*
             * collectApp probably changed the state of the DlsInfo
             * linked list, so it is better to just return now.
             * So this means we can collect at most 1 zombie per entry into
             * this function, but I don't expect to have that many zombies
             * to collect anyways.
             */
            return;
         }
      }
   }

   return;
}


/***************************************************************************
 * Function:
 *    static SINT32 initSocket(SINT32 side, SINT32 port, SINT32 type, SINT32 backlog)
 * Description:
 *    This function creates and initializes a TCP or UDP listening socket
 *    for an application.
 * Parameters:
 *    side     (IN) specifies whether it is a client-side socket or 
 *                  server-side socket.
 *    port     (IN) the application TCP or UDP port.
 *    type     (IN) the socket type, either SOCK_STREAM or SOCK_DGRAM.
 *    backlog  (IN) number of connections to queue. 
 * Returns:
 *    the socket file descriptor
 ***************************************************************************/
SINT32 initInetServerSocket(SINT32 domain, SINT32 port, SINT32 type, SINT32 backlog) 
{
   SINT32 sFd;
   SINT32 optVal;

   /* Create a TCP or UDP based socket */
   if ((sFd = socket(domain, type, 0)) < 0)
   {
      cmsLog_error("socket errno=%d port=%d", errno, port);
      return CMS_INVALID_FD;
   }

   /* Set socket options */
   optVal = 1;
   if (setsockopt(sFd, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(optVal)) < 0)
   {
      cmsLog_error("setsockopt errno=%d port=%d fd=%d", errno, port, sFd);
      close(sFd);
      return CMS_INVALID_FD;
   }

   /* Set up the local address */
   if (domain == AF_INET)
   {
      struct sockaddr_in   serverAddr;
      
      if (type == SOCK_DGRAM)
      {
         /* set option for getting the to ip address. */
         if (setsockopt(sFd, IPPROTO_IP, IP_PKTINFO, &optVal, sizeof(optVal)) < 0)
         {
            cmsLog_error("setsockopt errno=%d port=%d fd=%d", errno, port, sFd);
            close(sFd);
            return CMS_INVALID_FD;
         }
      }

      memset(&serverAddr, 0, sizeof(serverAddr));
      serverAddr.sin_family = AF_INET;
      serverAddr.sin_port   = htons(port);
      serverAddr.sin_addr.s_addr  = htonl(INADDR_ANY);

      /* Bind socket to local address */
      if (bind(sFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
      {
         cmsLog_error("bind errno=%d port=%d fd=%d", errno, port, sFd);
         close(sFd);
         return CMS_INVALID_FD;
      }
   }
   else
   {
      struct sockaddr_in6   serverAddr;

      if (type == SOCK_DGRAM)
      {
         /* set option for getting the to ip address. */
#ifdef IPV6_RECVPKTINFO
         if (setsockopt(sFd, IPPROTO_IPV6, IPV6_RECVPKTINFO, &optVal, sizeof(optVal)) < 0)
#else
         if (setsockopt(sFd, IPPROTO_IPV6, IPV6_PKTINFO, &optVal, sizeof(optVal)) < 0)
#endif
         {
            cmsLog_error("setsockopt errno=%d port=%d fd=%d", errno, port, sFd);
            close(sFd);
            return CMS_INVALID_FD;
         }
      }

      memset(&serverAddr, 0, sizeof(serverAddr));
      serverAddr.sin6_family = AF_INET6;
      serverAddr.sin6_port   = htons(port);
      serverAddr.sin6_addr   = in6addr_any;

      /* Bind socket to local address */
      if (bind(sFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
      {
         cmsLog_error("bind errno=%d port=%d fd=%d", errno, port, sFd);
         close(sFd);
         return CMS_INVALID_FD;
      }
   }

   if (type == SOCK_STREAM)
   {
      /* Enable connection to SOCK_STREAM socket */
      if (listen(sFd, backlog) < 0)
      {
         cmsLog_error("listen errno=%d port=%d fd=%d", errno, port, sFd);
         close(sFd);
         return CMS_INVALID_FD;
      }
   }

   return (sFd);
}


SINT32 initUnixDomainServerSocket()
{
   struct sockaddr_un serverAddr;
   SINT32 fd, rc;

   /* remove this in case it is still present from last smd shutdown */
   unlink(SMD_SHUTDOWN_IN_PROGRESS);

   /* mwang_todo:this is rather brute force, if we see the file here, maybe 
    * another smd is running...
    */
   unlink(SMD_MESSAGE_ADDR);


   if ((fd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0)
   {
      cmsLog_error("Could not create socket");
      return fd;
   }

   /*
    * Bind my server address and listen.
    */
   memset(&serverAddr, 0, sizeof(serverAddr));
   serverAddr.sun_family = AF_LOCAL;
   strncpy(serverAddr.sun_path, SMD_MESSAGE_ADDR, sizeof(serverAddr.sun_path));

   rc = bind(fd, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
   if (rc != 0)
   {
      cmsLog_error("bind to %s failed, rc=%d errno=%d", SMD_MESSAGE_ADDR, rc, errno);
      close(fd);
      return -1;
   }

   rc = listen(fd, SMD_MESSAGE_BACKLOG);
   if (rc != 0)
   {
      cmsLog_error("listen to %s failed, rc=%d errno=%d", SMD_MESSAGE_ADDR, rc, errno);
      close(fd);
      return -1;
   }

   /* add group write permisson to SMD_MESSAGE_ADDR so that user
    * applications that are in the same group as the owner can
    * communicate with SMD.
    */
//   chmod(SMD_MESSAGE_ADDR, (S_IRWXU | S_IRWXG | (S_IROTH|S_IXOTH))); 
   cmsLog_notice("smd msg socket opened and ready (fd=%d)", fd);

   return fd;
}


/** Send a message to the given dInfo structure.  
 *
 * How we send the message, and what actions we take are based on
 * the state of the receiving app, as indicated in the dInfo structure.
 *
 * @param dInfo (IN)  The dInfo structure for the receiving app.
 * @param msg   (IN)  The message that needs to be sent.  This function
 *                    will take ownership of this message, either queueing
 *                    it for freeing it.  So the caller must pass in a 
 *                    cmsMem_alloc'd buffer, not one from the stack.
 *
 */
void sendMessageByState(DlsInfo *dInfo, CmsMsgHeader **msg)
{

   /* mwang_todo: this looks a lot like logic in processEventMessage and routemessage
    * and distributeEventMessage, consolidate */
   switch(dInfo->state)
   {
   case DLS_NOT_RUNNING:
      cmsLog_notice("launching %s to receive msg 0x%x",
                    dInfo->eInfo->name, (*msg)->type);
      queueMsg(dInfo, msg);
      launchApp(dInfo);
      break;

   case DLS_LAUNCHED:
      cmsLog_notice("%s already launched but waiting for confirm, just queue msg 0x%x",
                    dInfo->eInfo->name, (*msg)->type);
      queueMsg(dInfo, msg);
      break;

   case DLS_RUNNING:
      cmsLog_notice("%s is already running, send message 0x%x now",
                    dInfo->eInfo->name, (*msg)->type);
#ifdef DESKTOP_LINUX
      if (dInfo->eInfo->flags & EIF_DESKTOP_LINUX_CAPABLE)
      {
         /*
          * only send the msg if the app is desktop capable.  otherwise it is
          * not actually running.
          */
         oalMsg_send(dInfo->commFd, *msg);
      }
#else
      oalMsg_send(dInfo->commFd, *msg);
#endif
      CMSMEM_FREE_BUF_AND_NULL_PTR(*msg);
      break;

   case DLS_TERMINATE_REQUESTED:
      /* are we shutting down the system? No need to send event messages?
       * mwang_todo: No, actually, we should use an "ACK" mechanism to make sure
       * that the app receives the event message.  It could be something
       * important that the app needs to act upon. */
      cmsLog_error("%s is in terminate requested state, cannot deliver msg 0x%x lost!",
                   dInfo->eInfo->name, (*msg)->type);
      CMSMEM_FREE_BUF_AND_NULL_PTR(*msg);
      break;
   }

   return;
}


void oal_sendDelayedMsg(UINT32 id, void *ctx)
{
   DlsInfo *dInfo = (DlsInfo *) ctx;
   CmsMsgHeader *msg;

   msg = (CmsMsgHeader *) cmsMem_alloc(sizeof(CmsMsgHeader), ALLOC_ZEROIZE);

   if (msg != NULL) {
      msg->type = CMS_MSG_DELAYED_MSG;
      msg->src = EID_SMD;
      msg->dst = (dInfo->eInfo->flags & (EIF_MULTIPLE_INSTANCES|EIF_MULTIPLE_THREADS)) ?
                 dInfo->specificEid : dInfo->eInfo->eid;
      msg->flags_event = 1;
      msg->wordData = id;

      cmsLog_debug("sending delayed msg to eid 0x%x id=%d", msg->dst, msg->wordData);

      sendMessageByState(dInfo, &msg);
   }

   return;
}


/** Queue the message onto the dInfo structure of the specified app.
 *
 * @param dInfo (IN)  The dInfo structure for the receiving app.
 * @param msg   (IN)  The message to be queued.  Since the message is
 *                    queued for later delivery, this function
 *                    will take ownership of this message. So the caller
 *                    must pass in a cmsMem_alloc'd buffer, not one from
 *                    the stack.  On return, the caller must not touch
 *                    or free this message.
 *
 */
void queueMsg(DlsInfo *dInfo, CmsMsgHeader **msg)
{

   CmsMsgHeader *msgHeader = dInfo->msgQueue;

   (*msg)->next = NULL;

   if  (msgHeader == NULL)
   {
      /* This is the first msg in the queue */
      dInfo->msgQueue = (*msg);
   }
   else
   {
      /* Need to apppend msg to the end of the queue */
      while (msgHeader->next  != NULL)
      {
         msgHeader = msgHeader->next;
      }
      msgHeader->next = (*msg);
   }

   *msg = NULL;

   return;
}


/** Process a message received on the communications link.
 *
 * @param dInfo (IN) The DlsInfo struct for the sender of the message.
 * @param msg   (IN) The received message.
 * 
 */
void processMessage(DlsInfo *dInfo, CmsMsgHeader **msg)
{
   cmsLog_notice("Got msg type 0x%x src=0x%x dst=0x%x",
                (*msg)->type, (*msg)->src, (*msg)->dst);


   if ((*msg)->dst == EID_SMD)
   {
      if ((*msg)->flags_event)
      {
         /* this is an event message sent to smd */
         if ((*msg)->type == CMS_MSG_SHMID)
         {
            /*
             * Special case, ssk is telling smd the shmId.
             * This event msg is only intended for smd and is not distributed.
             * smd needs to get the shmId early in case activateObjects
             * causes rcl handler functions to request app launch.
             */
            if ((shmId = (*msg)->wordData) == UNINITIALIZED_SHM_ID)
            {
               cmsLog_error("ssk could not initialize MDM, smd must exit now.");
               keepLooping = FALSE;
            }
            else
            {
               cmsLog_notice("got shmId=%d from ssk", shmId);
            }

            CMSMEM_FREE_BUF_AND_NULL_PTR(*msg);
         }
         else if ((*msg)->type == CMS_MSG_MDM_INITIALIZED)
         {
            UINT32 stage=2;
            CmsRet ret;

            /*
             * Special case, ssk is telling me that it has
             * finished initializing the MDM.  Now I can
             * initialize my own MDM and launch the other processes.
             * This event msg is only intended for smd and is not distributed.
             * mwang_todo: should put a timeout on getting this message
             * in case ssk gets stuck in an infinite loop during startup.
             */
            cmsLog_debug("ssk has initialized MDM, do my own mdm_init and stage2 launch");
            if ((ret = cmsMdm_init(EID_SMD, NULL, &shmId)) != CMSRET_SUCCESS)
            {
               cmsLog_error("cmsMdm_init failed, ret=%d", ret);
            }

            initLoggingFromConfig((*msg)->wordData);

            /*
             * I could get data model type by calling
             * cmsMdm_isDataModelDevice2() directly, but I am trying to
             * reduce (and eventually eliminate) smd access to MDM.
             */
            isDataModelDevice2 = *((UINT32 *)((*msg)+1));
            cmsLog_debug("isDataModelDevice2=%d", isDataModelDevice2);

            if ((ret = initDls(stage)) != CMSRET_SUCCESS)
            {
               cmsLog_error("initDls failed, ret=%d, smd must exit now", ret);
               keepLooping = FALSE;
            }
               
            oal_launchOnBoot(stage);

            CMSMEM_FREE_BUF_AND_NULL_PTR(*msg);

            /*
             * Now signal my parent (sync smd).  sync smd will exit,
             * which will allow the rest of the bootup sequence to proceed.
             */
            cmsLog_notice("send SIGHUP to sync smd (%d)", sync_smd_pid);
            prctl_signalProcess(sync_smd_pid, SIGHUP);
         }
         else
         {
            distributeEventMessage(msg, dInfo);
         }
      }
      else
      {
         /* this is a request message sent to smd */
         switch((*msg)->type)
         {
         case CMS_MSG_REGISTER_EVENT_INTEREST:
            processRegisterEventInterest(dInfo, msg, TRUE);
            break;

         case CMS_MSG_UNREGISTER_EVENT_INTEREST:
            processRegisterEventInterest(dInfo, msg, FALSE);
            break;

         case CMS_MSG_REGISTER_DELAYED_MSG:
            processRegisterDelayedMsg(dInfo, msg, TRUE);
            break;

         case CMS_MSG_UNREGISTER_DELAYED_MSG:
            processRegisterDelayedMsg(dInfo, msg, FALSE);
            break;

         case CMS_MSG_REBOOT_SYSTEM:
            processRebootMsg(dInfo, msg);
            break;

         case CMS_MSG_TERMINATE:
            processTerminateMsg(dInfo, msg);
            break;

         case CMS_MSG_START_APP:
            processStartAppMsg(dInfo, msg);
            break;
            
         case CMS_MSG_RESTART_APP:
            processRestartAppMsg(dInfo, msg);
            break;

         case CMS_MSG_STOP_APP:
            processStopAppMsg(dInfo, msg);
            break;
            
         case CMS_MSG_LOAD_IMAGE_STARTING:
            processLoadImageStarting(dInfo, msg);
            break;
            
         case CMS_MSG_LOAD_IMAGE_DONE:
            processLoadImageDone(dInfo, msg);
            break;
            
         case CMS_MSG_GET_CONFIG_FILE:
            processGetConfigFile(dInfo, msg);
            break;

         case CMS_MSG_VALIDATE_CONFIG_FILE:
            processValidateConfigFile(dInfo, msg);
            break;
            
         case CMS_MSG_WRITE_CONFIG_FILE:
            processWriteConfigFile(dInfo, msg);
            break;

         case CMS_MSG_SET_LOG_LEVEL:
            cmsLog_setLevel((*msg)->wordData);
            processMsgSimpleFooter(dInfo, CMSRET_SUCCESS, msg);
            break;

         case CMS_MSG_SET_LOG_DESTINATION:
            cmsLog_setDestination((*msg)->wordData);
            processMsgSimpleFooter(dInfo, CMSRET_SUCCESS, msg);
            break;

         case CMS_MSG_IS_APP_RUNNING:
            processIsAppRuning(dInfo, msg);
            break;
            
         case CMS_MSG_GET_SHMID:
            processGetShmId(dInfo, msg);
            break;

         case CMS_MSG_DUMP_EID_INFO:
            processDumpEidInfo(dInfo, msg);
            break;

         case CMS_MSG_INTERNAL_NOOP:
            /* just ignore this message and free it */
            CMSMEM_FREE_BUF_AND_NULL_PTR(*msg);
            break;

         default:
            cmsLog_error("cannot handle request type 0x%x from %d (flags=0x%x)",
                         (*msg)->type, (*msg)->src, (*msg)->flags);
            processMsgSimpleFooter(dInfo, CMSRET_METHOD_NOT_SUPPORTED, msg);
            CMSMEM_FREE_BUF_AND_NULL_PTR(*msg);
            break;
         }
      }
   }
   else
   {
      /* this is a message intended for some other app. */
      routeMessage(msg);
   }

   return;
}


/** Send a request/response message to another process.
 *
 * @param msg (IN) Message to route.  This function will steal the message
 *                 from the caller.  Caller does not need to deal with/free
 *                 the message after this.
 *
 */
void routeMessage(CmsMsgHeader **msg)
{
   CmsEntityId dstEid = (*msg)->dst;
   DlsInfo *dstDInfo;

   cmsLog_notice("routing msg 0x%x from 0x%x to 0x%x (req=%d resp=%d event=%d)",
                 (*msg)->type, (*msg)->src, (*msg)->dst,
                 (*msg)->flags_request, (*msg)->flags_response, (*msg)->flags_event);


   dstDInfo = getExistingDlsInfo(dstEid);
   if ((*msg)->flags_bounceIfNotRunning)
   {
      if ((dstDInfo == NULL) ||
          (dstDInfo->state == DLS_NOT_RUNNING) ||
          (dstDInfo->state == DLS_TERMINATE_REQUESTED))
      {
         /* sender does not want us to launch the app if not running */
         if ((*msg)->flags_event)
         {
            /*
             * The bounceIfNotRunning bit and the event bit are both set,
             * so sender does not want us to launch the app, and also does not
             * expect a response.  So just drop the message.
             */
            cmsLog_notice("bouncing event msg by dropping it");
            CMSMEM_FREE_BUF_AND_NULL_PTR(*msg);
         }
         else
         {
            DlsInfo *srcDinfo = getExistingDlsInfo((*msg)->src);

            cmsLog_notice("bouncing msg to eid=0x%x back to %s",
                          dstEid, srcDinfo->eInfo->name);
            processMsgSimpleFooter(srcDinfo, CMSRET_MSG_BOUNCED, msg);
         }
         return;
      }
   }


   if (dstDInfo == NULL)
   {
      /*
       * we might need to dynamically create a dInfo for this msg dst.
       * This is the "launch app to receive message case".
       */
      if ((dstDInfo = getDlsInfo(dstEid)) == NULL)
      {
         cmsLog_error("Cannot find dest eid %d, drop msg", dstEid);
         CMSMEM_FREE_BUF_AND_NULL_PTR(*msg);
         return;
      }

      /* We successfully created a new dlsInfo struct, but
       * does it make sense to launch a multiple instance app to receive a msg?
       * print a warning if this condition is detected.
       */
      if (dstDInfo->eInfo->flags & EIF_MULTIPLE_INSTANCES)
      {
         cmsLog_notice("launching multiple instance app (dst=0x%x) to receive msg (src=0x%x, msgType=0x%x)", dstEid, (*msg)->src, (*msg)->type);
         if (dstDInfo->eInfo->eid == EID_PPP)
         {
            cmsLog_notice("This is a staled pppd message and just ignore it so that pppd will not be lanched in sendMessageByState below");  
         }
         if ((*msg)->flags_request)
         {
            DlsInfo *srcDinfo = getExistingDlsInfo((*msg)->src);
            processMsgSimpleFooter(srcDinfo, CMSRET_NO_MORE_INSTANCES, msg);
         }
         else
         {
            CMSMEM_FREE_BUF_AND_NULL_PTR(*msg);
         }
         dlist_unlink((DlistNode *) (dstDInfo));
         CMSMEM_FREE_BUF_AND_NULL_PTR((dstDInfo));
         return;
      }
   }

   
   sendMessageByState(dstDInfo, msg);

   return;
}



/** Given an event msg, send a copy of this event message to
 *  all apps which have registered an interest in this event msg.
 *
 * We do not send the same msg to the creator/sender of the initial event msg.
 *
 * @param msg      (IN) The received event msg.  This function will distribute
 *                      a copy of this msg to all interested parties and
 *                      also free the msg at the end of this function.
 * @param srcDInfo (IN) The sender of the msg.  srcDinfo may be NULL if
 *                      the event was generated by smd itself.
 */
void distributeEventMessage(CmsMsgHeader **msg, const DlsInfo *srcDInfo)
{
   CmsEntityId eventSrc;
   DlsInfo *targetDInfo;
   EvtInterest *evtInterest;
   EvtInterestInfo *evtInfo;
   CmsMsgHeader *newMsg = NULL;

   if (srcDInfo != NULL)
   {
      cmsLog_notice("eventType=0x%x from %s", (*msg)->type, srcDInfo->eInfo->name);
      eventSrc = (*msg)->src;
   }
   else
   {
      cmsLog_notice("eventType=0x%x from smd", (*msg)->type);
      eventSrc = EID_SMD;
   }


   /*
    * Look up evtInterest for this event msg type
    * loop through all interested parties,
    *  --if they are currently running, send it to them,
    *  --if they are not currently running, queue the message and launch them.
    * 
    */          
   evtInterest = interest_findType((*msg)->type);
   if (evtInterest != NULL)
   {
      dlist_for_each_entry(evtInfo, &(evtInterest->evtInfoHead), dlist)
      {
         targetDInfo = getDlsInfo(evtInfo->eid);

         if (targetDInfo == NULL)
         {
            cmsLog_error("cannot find targetDInfo for eid 0x%x, skip it", evtInfo->eid);
         }
         else if (evtInfo->matchData && (((*msg)->dataLength == 0) ||
                                         cmsUtl_strcmp(evtInfo->matchData, ((char *) ((*msg)+1)))))
         {
            /* interested party specified additional match data, which this event did not match */
            cmsLog_debug("no match for eid 0x%x matchData=%s", evtInfo->eid, evtInfo->matchData);
            cmsLog_debug("src event dataLen=%d", (*msg)->dataLength);
            if ((*msg)->dataLength > 0)
            {
               cmsLog_debug("src data=%s", (char *) ((*msg)+1));
            }
         }
         else
         {
            /* OK, we can send this event */
            cmsLog_debug("dup and send event msg");
            newMsg = cmsMsg_duplicate(*msg);
            if (newMsg == NULL)
            {
               cmsLog_error("could not allocate msg, dropping event");
            }
            else
            {
               newMsg->src = eventSrc;
               newMsg->dst = (targetDInfo->eInfo->flags & (EIF_MULTIPLE_INSTANCES|EIF_MULTIPLE_THREADS)) ?
                           targetDInfo->specificEid : targetDInfo->eInfo->eid;

               sendMessageByState(targetDInfo, &newMsg);
            }
         }
      }
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(*msg);

   return;
}



void processRegisterEventInterest(DlsInfo *dInfo, CmsMsgHeader **msg, UBOOL8 positive)
{
   CmsRet ret;
   SINT32 delta = 0;
   char *matchData=NULL;
   
   if (positive)
   {
      cmsLog_notice("%s (eid=0x%x) registering interest for event 0x%x",
                    dInfo->eInfo->name, (*msg)->src, (*msg)->wordData);

      if ((*msg)->dataLength > 0)
      {
         matchData = (char *) ((*msg)+1);
      }

      if ((ret = interest_register((*msg)->wordData, (*msg)->src, matchData)) == CMSRET_SUCCESS)
      {
         delta = 1;
      }
   }
   else
   {
      cmsLog_notice("%s (eid=0x%x) unregistering interest for event 0x%x",
                    dInfo->eInfo->name, (*msg)->src, (*msg)->wordData);

      if ((ret = interest_unregister((*msg)->wordData, (*msg)->src)) == CMSRET_SUCCESS)
      {
        delta = -1;
      }
   }

  /* 
   * Let apps create duplicate registrations, we just ignore them.
   * So always return SUCCESS here.
   */
   processMsgSimpleFooter(dInfo, CMSRET_SUCCESS, msg);

   dInfo->numEventInterestRequested += delta;
   cmsAst_assert(dInfo->numEventInterestRequested >= 0);
   
   return;
}


void processRegisterDelayedMsg(DlsInfo *dInfo, CmsMsgHeader **msg, UBOOL8 positive)
{
   RegisterDelayedMsgBody *body=NULL;
   CmsRet ret = CMSRET_SUCCESS;
   UINT32 *ms = NULL;

   if (positive)
   {
      body = (RegisterDelayedMsgBody *) ((*msg) + 1);

      cmsLog_notice("%s (eid=0x%x) registering delayed msg 0x%x in %d ms",
                    dInfo->eInfo->name, dInfo->eInfo->eid, (*msg)->wordData, body->delayMs);
   }
   else
   {
      cmsLog_notice("%s (eid=0x%x) unregistering delayed msg 0x%x",
                    dInfo->eInfo->name, dInfo->eInfo->eid, (*msg)->wordData);
      if( ((*msg)->dataLength == sizeof(UINT32)) ) 
      {  //Sync Back Unregister.
          ms = (UINT32 *) ((*msg) + 1);
          if(sched_getTimeRemaining((*msg)->wordData, dInfo, ms) == CMSRET_SUCCESS)
          {
              cmsLog_notice("%s (eid=0x%x) unregistering delayed msg 0x%x TimeRemaining 0x%08x (%d ms)",
                            dInfo->eInfo->name, dInfo->eInfo->eid, (*msg)->wordData, *ms, *ms);
          }
          else
          {
            *ms = 0xFFFFFFFF;
          }
      }
   }


   /* For both register and unregister delayed msg, cancel the existing one first. */
   if ((ret = sched_cancel((*msg)->wordData, dInfo)) == CMSRET_SUCCESS)
   {
      dInfo->numDelayedMsgRequested--;
   }


   if (positive)
   {
      /* If this is a register, create a new event */
      if ((ret = sched_set((*msg)->wordData, dInfo, body->delayMs)) == CMSRET_SUCCESS)
      {
         dInfo->numDelayedMsgRequested++;
      }
   }

   if(ms)
   {
       //Sync Back Unregister , msg include remind ms body 
       UINT32 tmpSrc;
       
       tmpSrc = (*msg)->src;
       (*msg)->src = (*msg)->dst;
       (*msg)->dst = tmpSrc;
       (*msg)->flags_request = 0;
       (*msg)->flags_response = 1;
       (*msg)->wordData = ret;
       //(*msg)->dataLength = (*msg)->dataLength;

       ret = oalMsg_send(dInfo->commFd, *msg);
       if (ret != CMSRET_SUCCESS)
       {
          cmsLog_error("send response for msg 0x%x failed, ret=%d", (*msg)->type, ret);
       }
       else
       {
          cmsLog_debug("sent response for msg 0x%x dst=%d data=0x%x",
                       (*msg)->type, (*msg)->dst, (*msg)->wordData);
       }
       
       CMSMEM_FREE_BUF_AND_NULL_PTR(*msg);
   }
   else
   {
       processMsgSimpleFooter(dInfo, ret, msg);
   }

   return;
}


/** Common function for processStopAppMsg and procesRestartAppMsg.
 * This function stops/terminates the specified app.
 *
 * @param specificEid (IN) If we want to stop a multi-instance app, then
 *             the specific eid (pid and eid) must be passed in.
 *             For apps that only have one instance, then only the eid needs
 *             to be passed in.
 *
 */
void stopApp(UINT32 specificEid)
{
   const CmsEntityInfo *eInfo;
   DlsInfo *targetDinfo;
   CmsEntityId eid;
   UINT16 pid;

   /*
    * Extract the generic EID and the pid from the specific EID.
    */
   eid = GENERIC_EID(specificEid);
   pid = PID_IN_EID(specificEid);


   eInfo = cmsEid_getEntityInfo(eid);
   if (eInfo == NULL)
   {
      cmsLog_error("invalid eid %d", eid);
      return;
   }

   targetDinfo = getExistingDlsInfo(specificEid);
   if (targetDinfo == NULL)
   {
      if ((eInfo->flags & EIF_MULTIPLE_INSTANCES) && (pid != CMS_INVALID_PID))
      {
         /* caller gave us a valid pid for a multiple-instance app,
          * if we don't have a dInfo structure for it, something is wrong. */
         cmsLog_error("could not find specificEid=0x%x (pid=%d eid=%d)", specificEid, pid, eid);
      }
      else if ((eInfo->flags & EIF_MULTIPLE_INSTANCES) == 0)
      {
         /* for single instance apps, this function could have been
          * called as part of restartApp, so this app may not be running. */
         cmsLog_debug("no dinfo for %s (eid=%d), no need to stop it", eInfo->name, eid);
      }
   }
   else
   {
      /* Dont stop if app not running */
      if ( targetDinfo->state == DLS_NOT_RUNNING )
      {
         cmsLog_debug("dinfo found for %s, but already in DLS_NOT_RUNNING state", eInfo->name);
      }
      else
      {         
         terminateApp(&targetDinfo);
      }
   }

   return;
}


/** Common function for processStartAppMsg and processRestartAppMsg.
 * This functions starts an app.
 *
 * @param eid (IN) The entity id to start.
 * @param args (IN) Pointer to optional args for the app.  If argsLength is 0,
 *                  this parameter is ignored.
 * @param argsLength (IN) Length of the args.  0 if no args are provided.
 *
 * @return pid of the app that was started, or CMS_INVALID_PID(0) on failure.
 */
SINT32 startApp(CmsEntityId eid, const char *args, UINT32 argsLength)
{
   const CmsEntityInfo *eInfo;
   DlsInfo *targetDinfo;


   eInfo = cmsEid_getEntityInfo(eid);
   if (eInfo == NULL)
   {
      cmsLog_error("invalid eid %d", eid);
      return CMS_INVALID_PID;
   }

   if (eInfo->flags & EIF_MULTIPLE_INSTANCES)
   {
      /*
       * We must explicitly get a new DlsInfo in this case because
       * at this point, there cannot possibly be a dInfo in the 
       * dlsInfo dlist for this app.
       */
      targetDinfo = getNewDlsInfo(eid);
   }
   else
   {
      /*
       * getDlsInfo will return a dInfo structure if one already exists,
       * otherwise, it will create a new one.
       */
      targetDinfo = getDlsInfo(eid);
   }

   if (targetDinfo == NULL)
   {
      cmsLog_error("Could not create new DlsInfo");
      return CMS_INVALID_PID;
   }

   /*
    * OK, we are now ready to launch targetDinfo
    */
   if (argsLength > 0)
   {
      if (argsLength > DLS_OPT_ARGS_LENGTH)
      {
         cmsLog_error("optional args length %d is greater than max %d, ignored",
                      argsLength, DLS_OPT_ARGS_LENGTH);
      }
      else
      {
         cmsLog_debug("setting optional args %s", args);
         sprintf(targetDinfo->optArgs, "%s", args);
      }
   }

   if ( targetDinfo->state == DLS_NOT_RUNNING )
   {
   launchApp(targetDinfo);
   }
   else
   {
      cmsLog_error("Trying to launch multiple instance of single instance app");
      return CMS_INVALID_PID;      
   }

   return targetDinfo->pid;
}


void processStartAppMsg(DlsInfo *dInfo, CmsMsgHeader **msg)
{
   SINT32 pid;
   char *args = (char *) ((*msg)+1);

   /*
    * The wordData in a CMS_MSG_START_APP should only contain the eid
    * (should not have pid in it).
    */
   pid = startApp((*msg)->wordData, args, (*msg)->dataLength);


   /* We return the pid of the process in this reply msg, not CmsRet code */
   processMsgSimpleFooter(dInfo, pid, msg);

   return;
}


void processRestartAppMsg(DlsInfo *dInfo, CmsMsgHeader **msg)
{
   SINT32 pid;
   char *args = (char *) ((*msg)+1);


   /*
    * First we have to possibly stop an existing instance of this app.
    */
   stopApp((*msg)->wordData);


   /*
    * Now start the app.  When starting an app, we just want the generic eid.
    * The pid (in the specific eid) was only needed to stop the app.
    */
   pid = startApp(GENERIC_EID((*msg)->wordData), args, (*msg)->dataLength);

   processMsgSimpleFooter(dInfo, pid, msg);

   return;
}


void processStopAppMsg(DlsInfo *dInfo, CmsMsgHeader **msg)
{

   stopApp((*msg)->wordData);

   /* always tell the caller that we stopped the APP */
   processMsgSimpleFooter(dInfo, CMSRET_SUCCESS, msg);

   return;
}


/** Process a request to reboot the system.
 *
 *
 */
void processRebootMsg(DlsInfo *dInfo, CmsMsgHeader **msg)
{

   cmsLog_notice("Starting reboot sequence");
   
   /*
    * OK, if we get here, we are cleared for reboot.
    * send response to user before actually doing reboot.
    */
   processMsgSimpleFooter(dInfo, CMSRET_SUCCESS, msg);


#ifndef DESKTOP_LINUX
   /*
    * Use the busybox shell reboot command.  It will run the sequence of
    * commands as defined in the "shutdown" line in /etc/initab, which by
    * default "bcm_init_runner stop".
    */
   prctl_runCommandInShellBlocking("reboot");


   /*
    * The reboot command will return immediately.  smd should continue to
    * run because the bcm_init_runner stop will eventually send a
    * CMS_MSG_TERMINATE to smd, which tells it to terminate all CMS apps.
    */
#endif

   cmsLog_notice("reboot sequence started....");
}


void processLoadImageStarting(DlsInfo *dInfo, CmsMsgHeader **msg)
{

   char *connIfNamePtr=NULL;
   UBOOL8 isWanConnection = TRUE;
   UINT32 sdramSz, smallMemSzThresh;

#ifdef BRCM_VOICE_SUPPORT
   smallMemSzThresh = SZ_128MB;
#else
   smallMemSzThresh = SZ_16MB;
#endif /* BRCM_VOICE_SUPPORT */

   /* get connection interface name to determine if it is from WAN or LAN */
   if ((*msg)->dataLength > 0)
   {
      connIfNamePtr = (char *) (*msg + 1);
      if (cmsUtl_strstr(connIfNamePtr, "br") != NULL )
      {
         isWanConnection = FALSE;
      }
   }

   /*
   * See if we are running on a <128MB board.  If it is,
   * stop processes and remove some modules to free memory.
   */
   
   sdramSz = devCtl_getSdramSize();
   if (sdramSz <= smallMemSzThresh)
   {
      cmsLog_debug("This is a low memory system, sdramSz=%d", sdramSz);
      /* stop the dhcpd and dnsproxy to have more memory
       * seanl.  TODO: More can be done later on here for other applications or
       * the removal of the not used iptable rules.
       */

      /* asking kernel to free up all pages first before the following memory free operations */
      prctl_runCommandInShellBlocking("echo 3 > /proc/sys/vm/drop_caches");  

      stopApp(EID_DHCPD);
      stopApp(EID_DNSPROXY);
      stopApp(EID_UPNP);
      stopApp(EID_MCPD);
      stopApp(EID_DSLDIAGD);
      stopApp(EID_SWMDK);
      stopApp(EID_SAMBA);

      /* remove the flow cach since it is not needed here */
      // prctl_runCommandInShellBlocking("rmmod pktflow");

      if (isWanConnection)
      {
         /* for WAN connection, LAN side of the driver module can be freed.
         * seanl. todo: more can be done to remove some iptalbe modules
         */
         if (!cmsUtl_strstr(connIfNamePtr, "eth"))
         {
            /* If eth? is used as WAN, don't remove it from memory */
            prctl_runCommandInShellBlocking("rmmod bcm_enet");
         }      
         prctl_runCommandInShellBlocking("rmmod bcm_usb");
      }
      else
      {
         /* seanl. This is LAN connection and add code to get rid of WAN services can
         * free more memory if needed.
         */
      }

#ifdef BRCM_VOICE_SUPPORT
      /* Shut down voice to free up memory */
      stopApp(EID_DECT);
      stopApp(EID_VOICE);
      sleep(2);
      prctl_runCommandInShellBlocking("rmmod dect");
      prctl_runCommandInShellBlocking("rmmod dsphal");
      prctl_runCommandInShellBlocking("rmmod slicslac");
#endif /* BRCM_VOICE_SUPPORT */

      sleep(1);

      /* asking kernel to free up all pages one more time here after the above operations */
      prctl_runCommandInShellBlocking("echo 3 > /proc/sys/vm/drop_caches");
   }
   
   /* for debugging to see how much free memory left and can to be removed. seanl */
   prctl_runCommandInShellBlocking("cat /proc/meminfo");

   processMsgSimpleFooter(dInfo, CMSRET_SUCCESS, msg);    
   
}

void processLoadImageDone(DlsInfo *dInfo, CmsMsgHeader **msg)
{
   cmsLog_debug("got LOAD_IMAGE_DONE msg, do nothing for now");
   processMsgSimpleFooter(dInfo, CMSRET_SUCCESS, msg);    
}

void processGetConfigFile(DlsInfo *dInfo, CmsMsgHeader **msg)
{
   char *buf;
   CmsMsgHeader *hdr;
   char *body;
   UINT32 totalLen, cfgLen=0;
   CmsRet ret;
   
   totalLen = cmsImg_getConfigFlashSize() + sizeof(CmsMsgHeader);
   
   if ((buf = cmsMem_alloc(totalLen, ALLOC_ZEROIZE)) == NULL)
   {
      cmsLog_error("could not allocate %u bytes to hold config file", totalLen);
      processMsgSimpleFooter(dInfo, CMSRET_RESOURCE_EXCEEDED, msg);
      return;    
   }
   
   /*
    * Ideally, the smd should not access the MDM.  But smd already is the
    * recipient of the LOAD_IMAGE_STARTING and LOAD_IMAGE_DONE messages, so
    * keep these image related functions together.  Also, if ssk handles this
    * message, the config buffer will have to cross the kernel/user boundary
    * one more time, which is slightly inefficient.
    */
   if ((ret = cmsLck_acquireLockWithTimeout(SMD_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      cmsMem_free(buf);
      processMsgSimpleFooter(dInfo, ret, msg);
      return;
   }
   
   hdr = (CmsMsgHeader *) buf;
   body = (char *) (hdr + 1);
   
   cfgLen = totalLen - sizeof(CmsMsgHeader);
   ret = cmsMgm_readConfigFlashToBuf(body, &cfgLen);
   cmsLog_debug("cmsMgm_readConfigFlashToBuf returned %d cfgLen=%u", ret, cfgLen);
   
   cmsLck_releaseLock();
   
   if (ret != CMSRET_SUCCESS)
   {
      processMsgSimpleFooter(dInfo, ret, msg);
   }
   else
   {
      hdr->src = EID_SMD;
      hdr->dst = (*msg)->src;
      hdr->flags_response = 1;
      hdr->dataLength = cfgLen;
      
      if ((ret = oalMsg_send(dInfo->commFd, hdr)) != CMSRET_SUCCESS)
      {
         cmsLog_error("send of config file failed, ret=%d", ret);
      }

      CMSMEM_FREE_BUF_AND_NULL_PTR(*msg);
   }
   
   cmsMem_free(buf);
}

/*
 * This should get moved into lock.c.  Put here for now just before the 4.02L.02 release.
 */
static void dumpLockInfo(void)
{
   CmsLckInfo info;
   UINT32 deltaMs __attribute__ ((unused));
   CmsTimestamp ts;

   cmsLck_getInfo(&info);

   cmsLog_error("locked=%d lockOwner=%d", info.locked, info.lockOwner);

   if (info.locked)
   {
      cmsTms_get(&ts);
      deltaMs = cmsTms_deltaInMilliSeconds(&ts, &info.timeAquired);
      cmsLog_error("held for %dms by function %s", deltaMs, info.callerFuncName);
   }

   return;
}

void processValidateConfigFile(DlsInfo *dInfo, CmsMsgHeader **msg)
{
   char *body;
   CmsRet ret;
   
   /*
    * Ideally, the smd should not access the MDM.  But smd already is the
    * recipient of the LOAD_IMAGE_STARTING and LOAD_IMAGE_DONE messages, so
    * keep these image related functions together.  Also, if ssk handles this
    * message, the config buffer will have to cross the kernel/user boundary
    * one more time, which is slightly inefficient.
    */
   if ((ret = cmsLck_acquireLockWithTimeout(SMD_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      dumpLockInfo();
      processMsgSimpleFooter(dInfo, ret, msg);
      return;
   }
    
   body = (char *) ((*msg) + 1);
   
   ret = cmsMgm_validateConfigBuf(body, (*msg)->dataLength);
   
   cmsLck_releaseLock();
   
   cmsLog_debug("cmsMgm_validateConfigBuf ret=%d", ret);
   
   processMsgSimpleFooter(dInfo, ret, msg);
   
   return;
}

static void processWriteConfigFile(DlsInfo *dInfo, CmsMsgHeader **msg)
{
   char *body;
   CmsRet ret;

   /*
    * Ideally, the smd should not access the MDM.  But smd already is the
    * recipient of the LOAD_IMAGE_STARTING and LOAD_IMAGE_DONE messages, so
    * keep these image related functions together.  Also, if ssk handles this
    * message, the config buffer will have to cross the kernel/user boundary
    * one more time, which is slightly inefficient.
    *
    * Make the timeout 5 times longer here for the LAN image/configuration data update while
    * WAN link is just up and that process might take more than SMD_LOCK_TIMEOUT to 
    * finish the iptable rules.
    */
   if ((ret = cmsLck_acquireLockWithTimeout(SMD_LOCK_TIMEOUT * 5)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      dumpLockInfo();
      processMsgSimpleFooter(dInfo, ret, msg);
      return;
   }
   
   body = (char *) ((*msg) + 1);
   
   /* I don't know if this buffer was already validated or not, validate again just to be safe. */
   ret = cmsMgm_validateConfigBuf(body, (*msg)->dataLength);
   if (ret == CMSRET_SUCCESS)
   {
      ret = cmsMgm_writeValidatedBufToConfigFlash(body, (*msg)->dataLength);
      cmsLog_debug("cmsMgm_writeValidatedBufToConfigFlash ret=%d", ret);
   }
   
   cmsLck_releaseLock();
   
   processMsgSimpleFooter(dInfo, ret, msg);
   
   return;   
}



/** A common send response function used by various processXXX msg functions.
 *
 * Sends a reply using the same msg buffer that held the request and frees
 * the msg buffer.
 */
void processMsgSimpleFooter(DlsInfo *dInfo, CmsRet rv, CmsMsgHeader **msg)
{
   CmsRet ret;
   UINT32 tmpSrc;

   tmpSrc = (*msg)->src;
   (*msg)->src = (*msg)->dst;
   (*msg)->dst = tmpSrc;

   (*msg)->flags_request = 0;
   (*msg)->flags_response = 1;
   (*msg)->dataLength = 0;
   (*msg)->wordData = rv;

   ret = oalMsg_send(dInfo->commFd, (*msg));
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("send response for msg 0x%x failed, ret=%d", (*msg)->type, ret);
   }
   else
   {
      cmsLog_debug("sent response for msg 0x%x dst=%d data=0x%x",
                   (*msg)->type, (*msg)->dst, (*msg)->wordData);
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(*msg);

   return;
}


void processIsAppRuning(DlsInfo *dInfo, CmsMsgHeader **msg)
{
   DlsInfo *targetDinfo;
   CmsEntityId targetEid;
   CmsRet rv;

   rv = CMSRET_OBJECT_NOT_FOUND;
   
   targetEid = (CmsEntityId) (*msg)->wordData;

   cmsLog_debug("targetEid =%d", (*msg)->wordData);
   
   targetDinfo = getExistingDlsInfo(targetEid);

   if (targetDinfo && targetDinfo->state == DLS_RUNNING)
   {
      rv = CMSRET_SUCCESS;
   }

   cmsLog_debug("ret = %d -- if ret ==0, appl. is running.", rv);
   processMsgSimpleFooter(dInfo, rv, msg);

}


void processGetShmId(DlsInfo *dInfo, CmsMsgHeader **msg)
{
   cmsLog_debug("returning ShmId=%d", shmId);

   processMsgSimpleFooter(dInfo, shmId, msg);
}


void processDumpEidInfo(DlsInfo *dInfo, CmsMsgHeader **msg)
{
   const CmsEntityInfo *eInfo;
   CmsRet ret=CMSRET_SUCCESS;

   if ((*msg)->wordData)
   {
      if (NULL == (eInfo = cmsEid_getEntityInfo((*msg)->wordData)))
      {
         printf("eInfo for %d (0x%x) not found!\n",
               (*msg)->wordData, (*msg)->wordData);
         ret = CMSRET_INVALID_PARAM_NAME;
      }
      else
      {
         cmsEid_dumpEntityInfo(eInfo);
      }
   }
   else
   {
      eInfo = cmsEid_getFirstEntityInfo();
      while (eInfo)
      {
         cmsEid_dumpEntityInfo(eInfo);
         eInfo = cmsEid_getNextEntityInfo(eInfo);
      }
   }

   processMsgSimpleFooter(dInfo, ret, msg);
}


/* terminate all CMS apps and exit smd.
 *
 * Shutting down the system gracefully can prevent hangs (see Jira-10576)
 * and is also useful for modular software update.
 */
void processTerminateMsg(DlsInfo *dInfo, CmsMsgHeader **msg)
{
   CmsRet ret;

   printf("smd received Terminate msg!!  Terminate all apps and then exit.\n");

   ret = cmsFil_writeBufferToFile(SMD_SHUTDOWN_IN_PROGRESS,
                                              (unsigned char *) "1", 1);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not write to file %s, ret=%d",
                   SMD_SHUTDOWN_IN_PROGRESS, ret);
      /* this write is not absolutely essential, so just keep going */
   }

   shutdownInProgress=TRUE;
   shutdownSourceDinfo = dInfo;

   /* send out terminate messages to all processes we know about */
   cleanupDls();

   processMsgSimpleFooter(dInfo, CMSRET_SUCCESS, msg);

   cmsMem_free(dInfo);

   keepLooping = FALSE;
}


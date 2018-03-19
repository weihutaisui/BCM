/***********************************************************************
 *
 *  Copyright (c) 2006-2007  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2011:proprietary:standard

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
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <string.h>
#include <syslog.h>
#include <netinet/in.h>
#include <netdb.h>
#include <linux/if.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"
#include "cms_msg_modsw.h"
#include "cms_params_modsw.h"
#define CPEVARNAMEINSTANCE
#include "inc/tr69cdefs.h"
#undef CPEVARNAMEINSTANCE
#include "cms_tms.h"

#include "inc/appdefs.h"
#include "inc/utils.h"
#include "bcmLibIF/bcmWrapper.h"
#include "bcmLibIF/bcmConfig.h"
#include "event.h"
#include "informer_public.h"

#include "cms_linklist.h"
#include "osgid.h"


extern void proto_Init(void);  /* in protocol.h */
extern void freeAllListeners(void);  /* in event.c */
extern void changeNameSpaceCwmpVersionURL(int version);

#ifdef SUPPORT_STUN
extern void stun_configChanged(void);
#endif

#ifdef DMALLOC
#include "dmalloc.h"
#endif

/** external data **/
extern int sendGETRPC;              /* send a GetRPCMetods */
extern UBOOL8 rebootingFlag;        /*set if we are doing system reboot or factoryreset*/
extern UBOOL8 needDisconnect;

/** public data **/
ACSState       acsState;
int tr69cTerm = 0;
void *tmrHandle = NULL;
const char *RootDevice;
LimitNotificationQInfo limitNotificationList;
UBOOL8 openConnReqServerSocket=FALSE;

#ifdef SUPPORT_TR69C_AUTONOMOUS_TRANSFER_COMPLETE
AutonomousFileTransferStats autonomousCompleteStats;
extern UBOOL8 doSendAutonTransferComplete;
CmsRet autonomousFileTypeToFileTypeStr(UINT32 fileType, char **fileStr);
void freeInitAutonomousCompleteStats(AutonomousFileTransferStats *pAutonomousCompleteStats);
#endif

/*
  * display SOAP messages on serial console.
  * This flag is initialize, enabled or disabled in main.c,
  * and perform action in protocol.c
  */
UBOOL8 loggingSOAP = FALSE; 

/* forward delcaration */
void acsState_cleanup(void);
extern void deregisterEvents(void);
ChangeDuStateOpReqInfo *pAutonResult = NULL;

void delayedTermFunc(void *handle __attribute__((unused)))
{
   CmsRet ret;

   cmsLog_notice("got terminal signal, cancel periodic timer and trigger final inform");

   /*
    * Terminal signal is handled differently than normal exit from
    * the event loop.  Under normal exit, we still allow smd to wake us
    * up sometime in the future for periodic inform or ACS changed.
    * But if we get a terminal signal, that means the user wants us to
    * completely exit and never come back, so clean up all delayed actions
    * in smd.
    */

   /* cancel any periodic inform I have in the smd */
   cancelPeriodicInform();

   if (!rebootingFlag)
   {
      /* cancel interest in various notifications that we might have registered for. */
      deregisterEvents();
   }

	/*
    * Set the timer to send out an inform to ACS.  The purpose is to
	 * create an ACS disconnect event so that the tr69c termination
	 * flag will be examined and acted on.
    * mwang: again, passing in a handle of NULL to sendInform does not make sense.
    * See comments in acsDisconnect().
    */
   ret = cmsTmr_set(tmrHandle, sendInform, NULL, DELAYED_TERMINAL_ACTION_DELAY, "terminal_inform");
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("setting terminal sendInform timer failed, ret=%d", ret);
   }

   return;
}


void manageableDeviceNotificationLimitFunc(void *handle __attribute__((unused)))
{
   cmsLog_notice("Manageable Device Limit Notification: time to send notification!");

   addInformEventToList(INFORM_EVENT_VALUE_CHANGE);
   sendInform(NULL);
}

void handleNotificationLimit(char *notificationLimitName, int notificationLimitValue, CmsEventHandler limitInformFunc)
{
   LimitNotificationInfo *entry, *ptr;
   int found = 0, i;

   cmsLog_debug("notificationLimitName %s, limitValue %d, limitInformFunc %p",
                notificationLimitName,notificationLimitValue,limitInformFunc);

   /* loop through the notificationList queue to look for this parameter name (notificationLimitName).
    * We register this limitName, and value to the queue if it doesn't exist.
    * If there is such an item already, this mean notificationLimitValue is changed, just update the limitValue.
    */

   if ((entry = cmsMem_alloc(sizeof(LimitNotificationInfo),ALLOC_ZEROIZE)) != NULL)         
   {
      entry->parameterFullPathName = cmsMem_strdup(notificationLimitName);
      entry->limitValue = notificationLimitValue * MSECS_IN_SEC;
      entry->func = limitInformFunc;
   }
   else
   {
      return;
   }

   if (limitNotificationList.count != 0)
   {
      ptr = limitNotificationList.limitEntry;
      for (i = 0; i < (limitNotificationList.count) && (ptr!=NULL); i++)
      {
         if (cmsUtl_strcmp(ptr->parameterFullPathName, notificationLimitName) == 0) 
         {
            ptr->limitValue = notificationLimitValue * MSECS_IN_SEC;
            found = 1;
            cmsMem_free(entry->parameterFullPathName);
            cmsMem_free(entry);
            break;
         } /* found entry */
         else
         {
            ptr = ptr->next;
         }
      } /* walk through limitNotificationList */
      if (!found)
      {
         entry->next = limitNotificationList.limitEntry;
         limitNotificationList.limitEntry = entry;
         limitNotificationList.count += 1;
      }
   } /* limitNotification list not empty */
   else
   {
      limitNotificationList.limitEntry = entry;
      limitNotificationList.count = 1;
   } /* list is empty */
} /* handleNotificationLimit */

void tr69c_sigTermHandler(int sig __attribute__((unused)))
{
   CmsRet ret;

   /*
    * Try not to do too much in a signal handler.
    * Calling cmsTmr_set() will result in a malloc.
    */
   cmsLog_debug("got sig %d", sig);

   tr69cTerm = 1;

   ret = cmsTmr_set(tmrHandle, delayedTermFunc, 0, DELAYED_TERMINAL_ACTION_DELAY, "sig_delayed_proc");
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("setting delayed signal processing timer failed, ret=%d", ret);
   }

   return;
}







void usage(char *progName)
{
   /* use print because I don't know the state of the log (?) */
   printf("usage: %s [-v num] [-m shmId] [-e num] [-u acsURL] [-i informInterval]\n", progName);
   printf("       v: set verbosity, where num==0 is LOG_ERROR, 1 is LOG_NOTICE, all others is LOG_DEBUG\n");
   printf("       m: shared memory id, -1 if standalone or not using shared mem.\n");
   printf("       u: for url for the ACS, otherwise, URL is obtained from mdm\n");
   printf("       r: connection request URL, otherwise, URL is obtained from mdm\n");
   printf("       o: open connection request server socket, normally this server socket is inheritted from smd\n");
   printf("       i: inform interval, otherwise, informInterval is obtained from mdm\n");
   printf("       b: informEnable, 1 is true, otherwise, informEnable is obtained from mdm\n");
   printf("       f: boundIfName, otherwise, boundIfName is obtained from mdm\n");
   exit(1);
}

void acsState_cleanup(void)
{
   cmsMem_free(acsState.acsURL);
   cmsMem_free(acsState.acsUser);
   cmsMem_free(acsState.acsPwd);
   cmsMem_free(acsState.redirectURL);
   cmsMem_free(acsState.rebootCommandKey);
   cmsMem_free(acsState.downloadCommandKey);
   cmsMem_free(acsState.boundIfName);
   cmsMem_free(acsState.connReqURL);
   cmsMem_free(acsState.connReqIpAddr);
   cmsMem_free(acsState.connReqIpAddrFullPath);
   cmsMem_free(acsState.connReqPath);
   cmsMem_free(acsState.connReqUser);
   cmsMem_free(acsState.connReqPwd);
   cmsMem_free(acsState.kickURL);
   cmsMem_free(acsState.provisioningCode);
   cmsMem_free(acsState.dlFaultMsg);
   cmsMem_free(acsState.scheduleInformCommandKey);
   cmsMem_free(acsState.manufacturer);
   cmsMem_free(acsState.manufacturerOUI);
   cmsMem_free(acsState.productClass);
   cmsMem_free(acsState.serialNumber);
}


/* Receive response from Osgi about a requested operation.
 * The response contains result of the operation, update it in acsState structure.
 * Autonomous means the DU state change wasn't triggered by TR69 ACS.
 */
void freeOsgiResultList(ChangeDuStateOpReqInfo *pResult)
{
   ChangeDuStateOpReqInfo *p;

   /* XXX miwang: is this the same as freeOpReqList ? */
   while (pResult != NULL)
   {
      p = pResult;
      CMSMEM_FREE_BUF_AND_NULL_PTR(p->url);
      CMSMEM_FREE_BUF_AND_NULL_PTR(p->version);
      CMSMEM_FREE_BUF_AND_NULL_PTR(p->euRef);
      CMSMEM_FREE_BUF_AND_NULL_PTR(p->duRef);
      pResult = pResult->next;
      CMSMEM_FREE_BUF_AND_NULL_PTR(p);
   }
}


#ifdef SUPPORT_TR69C_AUTONOMOUS_TRANSFER_COMPLETE
void freeInitAutonomousCompleteStats(AutonomousFileTransferStats *pAutonomousCompleteStats)
{
   if (pAutonomousCompleteStats == NULL)
   {
      return;
   }
   if (pAutonomousCompleteStats->fileType == NULL)
   {
      /* this means structure has been used yet */
      return;
   }
   CMSMEM_FREE_BUF_AND_NULL_PTR(pAutonomousCompleteStats->fileType);
   CMSMEM_FREE_BUF_AND_NULL_PTR(pAutonomousCompleteStats->fault.faultString);
   CMSMEM_REPLACE_STRING(pAutonomousCompleteStats->startTime,UNKNOWN_DATETIME_STRING);
   CMSMEM_REPLACE_STRING(pAutonomousCompleteStats->completeTime,UNKNOWN_DATETIME_STRING);
}

CmsRet autonomousFileTypeToFileTypeStr(UINT32 fileType, char **fileStr)
{
   CmsRet ret = CMSRET_SUCCESS;

   switch (fileType)
   {
   case CMS_IMAGE_FORMAT_BROADCOM:
   case CMS_IMAGE_FORMAT_FLASH:
      *fileStr = cmsMem_strdup(FILETYPE_FIRMWARE_UPGRADE);
      break;
   case CMS_IMAGE_FORMAT_XML_CFG:
      *fileStr = cmsMem_strdup(FILETYPE_VENDOR_CONFIG);
      break;
   default:
      ret = CMSRET_INTERNAL_ERROR;
   }
   return(ret);
}

#endif /* SUPPORT_TR69C_AUTONOMOUS_TRANSFER_COMPLETE */

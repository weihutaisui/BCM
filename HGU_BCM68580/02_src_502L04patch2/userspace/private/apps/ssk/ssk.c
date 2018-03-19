/*
* <:copyright-BRCM:2011:proprietary:standard
* 
*    Copyright (c) 2011 Broadcom 
*    All Rights Reserved
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
:>
*/

#include "cms.h"
#include "cms_util.h"
#include "cms_core.h"
#include "cms_dal.h"
#include "cms_msg.h"
#include "cms_qdm.h"
#include "cms_boardioctl.h"
#include "cms_boardcmds.h"
#include "bcmnet.h"
#include "ssk.h"
#include "AdslMibDef.h"
#include "devctl_adsl.h"

#include "bcmnetlink.h"
#include <errno.h>

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/if_addr.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <time.h>

#if defined (DMP_TIME_1) || defined (DMP_DEVICE2_TIME_1)
#include "ssk_time.h"
#endif

/* globals */
SINT32   shmId = UNINITIALIZED_SHM_ID;
SINT32   kernelMonitorFd = CMS_INVALID_FD;  /* BRCM proprietary way to report events -- only really needed for DSL */
SINT32   stdNetlinkMonitorFd = CMS_INVALID_FD;  /* standard way to get link events */
UBOOL8     keepLooping=TRUE;
void *   msgHandle=NULL;
const CmsEntityId myEid=EID_SSK;
UBOOL8 useConfiguredLogLevel=TRUE;
UBOOL8 isLowMemorySystem=FALSE;
CmsTimestamp bootTimestamp;

#ifdef SUPPORT_MOCA
SINT32 mocaMonitorFd = CMS_INVALID_FD;
#endif

#ifdef DMP_DSLDIAGNOSTICS_1
extern dslLoopDiag dslLoopDiagInfo;
#endif
#ifdef DMP_X_BROADCOM_COM_SELT_1
extern dslDiag dslDiagInfo;
#endif
UBOOL8 dslDiagInProgress=FALSE;

#ifdef DMP_X_BROADCOM_COM_AUTODETECTION_1
/* This is flag initialied when ssk starts and updated when it is 
* changed by user.  It is also used in connstatus.c 
*/
UBOOL8 isAutoDetectionEnabled=FALSE;
#endif

/* forward function declarations */
static SINT32 ssk_main(void);
static CmsRet ssk_init(void);
static CmsRet initKernelMonitorFd(void);
static CmsRet initStdNetlinkMonitorFd(void);
static void initLoggingFromConfig(void);
static UINT32 getSmdLogSettingsFromConfig(void);
static void processKernelMonitor(void);
static void processStdNetlinkMonitor(void);
static void processPeriodicTask(void);
static void processGetDeviceInfo(CmsMsgHeader *msg);

static void processWanPortEnable(CmsMsgHeader *msg);
static void processPostMdmAct(CmsMsgHeader *msg);
#ifdef DMP_DEVICE2_GATEWAYINFO_1
static void processDhcpcGatewayInfo(CmsMsgHeader *msg);
#endif

static void registerInterestInEvent(CmsMsgType msgType, UBOOL8 positive, void *msgData, UINT32 msgDataLen);

#if defined (DMP_X_BROADCOM_COM_STANDBY_1)
extern void processPeriodicStandby(StandbyCfgObject *standbyCfgObj);
#endif

#ifdef DMP_X_BROADCOM_COM_EPON_1
static void processEponSetQueueConfig(CmsMsgHeader *msg);
static void processEponGetQueueConfig(CmsMsgHeader *msg);
#endif

void dumpLockInfo(void);

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


   cmsLog_initWithName(myEid, argv[0]);

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

   if ((ret = ssk_init()) != CMSRET_SUCCESS)
   {
      cmsLog_error("initialization failed (%d), exit.", ret);
      return -1;
   }

   /*
    * Main body of code here.
    */
   exitCode = ssk_main();


   cmsLog_notice("exiting with code %d", exitCode);

   if (kernelMonitorFd != CMS_INVALID_FD)
   {
      close(kernelMonitorFd);
   }

   if (stdNetlinkMonitorFd != CMS_INVALID_FD)
   {
      close(stdNetlinkMonitorFd);
   }

#ifdef SUPPORT_MOCA
#ifndef BRCM_MOCA_DAEMON
   cleanupMocaMonitorFd();
#endif
#endif


   cmsMsg_cleanup(&msgHandle);
   cmsLog_cleanup();

   freeWanLinkInfoList();
   cleanupLinkStatusRecords();
   
   return exitCode;
}


/** This is the main loop of ssk.
 *
 * Just read messages and events and process them.
 * The DSL link-up, link-down detection should be done here and
 * not in smd.
 */
SINT32 ssk_main()
{
   CmsRet ret;
   CmsMsgHeader *msg=NULL;
   SINT32 exitCode=0;
   SINT32 commFd;
   SINT32 n, maxFd;
   fd_set readFdsMaster, readFds;
   fd_set errorFdsMaster, errorFds;
   struct timeval tv;
   CmsTimestamp nowTs, expireTs, deltaTs;
   CmsTimestamp lastPeriodicTaskTs, deltaPeriodicTaskTs;
#ifdef BRCM_WATCHDOG_TIMER
   FILE *fptr = fopen("/proc/watchdog", "w");
#endif
   

   cmsTms_get(&nowTs);

   lastPeriodicTaskTs.sec = nowTs.sec;
   lastPeriodicTaskTs.nsec = nowTs.nsec;


   /* set up all the fd stuff for select */
   FD_ZERO(&readFdsMaster);
   FD_ZERO(&errorFdsMaster);

   cmsMsg_getEventHandle(msgHandle, &commFd);
   FD_SET(commFd, &readFdsMaster);
   maxFd = commFd;

   /* in DESKTOP_LINUX mode, kernelMonitorFd is not opened, so must check */
   if (kernelMonitorFd != CMS_INVALID_FD)
   {
      FD_SET(kernelMonitorFd, &readFdsMaster);
      maxFd = (maxFd > kernelMonitorFd) ? maxFd : kernelMonitorFd;
   }

   if (stdNetlinkMonitorFd != CMS_INVALID_FD)
   {
      FD_SET(stdNetlinkMonitorFd, &readFdsMaster);
      maxFd = (maxFd > stdNetlinkMonitorFd) ? maxFd : stdNetlinkMonitorFd;
   }

#ifdef SUPPORT_MOCA
   if (mocaMonitorFd != CMS_INVALID_FD)
   {
      FD_SET(mocaMonitorFd, &readFdsMaster);
      maxFd = (maxFd > mocaMonitorFd) ? maxFd : mocaMonitorFd;
   }
#endif

#ifdef BRCM_WATCHDOG_TIMER
   /*  enable watchdog timer 
    *  1st param: enable watchdog timer (0/1)
    *  2nd param: timer period, unit is microsecond
    *  3rd param: enable user mode watchdog timer (0/1)
    *  4th param: user mode watchdog timer threshold
    *  When enable user mode WD, write "ok" to
    *  /proc/watchdog to reset user mode WD timer. 
    */
   if (fptr)
   {
       fprintf(fptr, "1 5000000 0 0");
       fclose(fptr);
   }
#endif

   while (keepLooping)
   {
      readFds = readFdsMaster;
      errorFds = errorFdsMaster;

      cmsTms_get(&nowTs);

#if defined(DMP_DSLDIAGNOSTICS_1) || defined(DMP_X_BROADCOM_COM_SELT_TEST_1)
      if (dslDiagInProgress == TRUE)
      {
         /* we need to check the result every 2 seconds */
         expireTs.sec = nowTs.sec + 2;
         expireTs.nsec = nowTs.nsec;
         cmsLog_debug("diag in progress, go to fast monitor loop mode");
      }
      else
#endif
      {
         expireTs.sec = lastPeriodicTaskTs.sec + PERIODIC_TASK_INTERVAL;
         expireTs.nsec = lastPeriodicTaskTs.nsec;
      }


      /* calculate how long we should sleep */
      cmsTms_delta(&expireTs, &nowTs, &deltaTs);
      tv.tv_sec = deltaTs.sec;
      tv.tv_usec = deltaTs.nsec / NSECS_IN_USEC;

      /* If we spent too much time processing the message
      * nowTs will be greater than the expireTs and we will have
      * rollover.  Force timeout to 0 
      */
      if (tv.tv_sec > PERIODIC_TASK_INTERVAL * 2 || tv.tv_sec < 0)
      {
         cmsLog_debug("Rollover detected. tv.tv_sec =%d", tv.tv_sec);
         cmsLog_debug("nowTS=%u.%u lastPeriodicTs=%u.%u expireTS=%u.%u",
                      nowTs.sec, nowTs.nsec,
                      lastPeriodicTaskTs.sec, lastPeriodicTaskTs.nsec,
                      expireTs.sec, expireTs.nsec);
         tv.tv_sec = 0;
         tv.tv_usec = 0;
      }

      cmsLog_debug("calling select with tv=%d.%d", tv.tv_sec, tv.tv_usec);
      n = select(maxFd+1, &readFds, NULL, &errorFds, &tv);
      
      if (n < 0)
      {
         /* interrupted by signal or something, continue */
         continue;
      }

      if ( 0 == n )
      {
         cmsTms_get(&nowTs);
         cmsTms_delta(&nowTs,&lastPeriodicTaskTs,&deltaPeriodicTaskTs);
         cmsLog_debug("nowTS=%u.%u lastPeriodicTs=%u.%u deltaTS=%u.%u",
                      nowTs.sec, nowTs.nsec,
                      lastPeriodicTaskTs.sec, lastPeriodicTaskTs.nsec,
                      deltaPeriodicTaskTs.sec, deltaPeriodicTaskTs.nsec);
         if (deltaPeriodicTaskTs.sec >= PERIODIC_TASK_INTERVAL)
         {
            processPeriodicTask();

            /*
             * Update lastPeriodicTaskTs based on when it SHOULD have been called,
             * rather than when it WAS called.  This will eliminate drifting of when
             * the periodic tasks are run due to other scheduling/locking delays.
             */
            lastPeriodicTaskTs.sec += PERIODIC_TASK_INTERVAL;

            /*
             * On the other hand, if we are running hopelessly behind, then
             * don't try to catch up and start at the next periodic interval.
             */
            if (deltaPeriodicTaskTs.sec > (20* PERIODIC_TASK_INTERVAL))
            {
               cmsLog_error("hopelessly behind: "
                            "nowTs=%u.%u lastPeriodicTs=%u.%u deltaTS=%u.%u",
                            nowTs.sec, nowTs.nsec,
                            lastPeriodicTaskTs.sec, lastPeriodicTaskTs.nsec,
                            deltaPeriodicTaskTs.sec, deltaPeriodicTaskTs.nsec);
               cmsTms_get(&nowTs);
               lastPeriodicTaskTs.sec = nowTs.sec + PERIODIC_TASK_INTERVAL;
               lastPeriodicTaskTs.nsec = 0;
               cmsLog_error("catch up, jump to lastPeriodicTs=%u.%u",
                            lastPeriodicTaskTs.sec, lastPeriodicTaskTs.nsec);
            }
         }
      }

      if ((kernelMonitorFd != CMS_INVALID_FD) &&
          FD_ISSET(kernelMonitorFd, &readFds))
      {
         processKernelMonitor();
      }

      if ((stdNetlinkMonitorFd != CMS_INVALID_FD) &&
          FD_ISSET(stdNetlinkMonitorFd, &readFds))
      {
         cmsLog_debug("Detected Linux netlink event!");
         processStdNetlinkMonitor();
      }

      if (FD_ISSET(commFd, &readFds))
      {
         if ((ret = cmsMsg_receive(msgHandle, &msg)) != CMSRET_SUCCESS)
         {
            if (ret == CMSRET_DISCONNECTED)
            {
               if (cmsFil_isFilePresent(SMD_SHUTDOWN_IN_PROGRESS))
               {
                  /* smd is shutting down, I should quietly exit too */
                  return 0;
               }
               else
               {
                  cmsLog_error("detected exit of smd, ssk will also exit");
                  /* unexpectedly lost connection to smd, maybe I should
                   * reboot the system here to recover.... */
                  return -1;
               }
            }
            else
            {
               struct timespec sleep_ts={0, 100*NSECS_IN_MSEC};
               
               /* try to keep running after a 100ms pause */
               cmsLog_error("error during cmsMsg_receive, ret=%d", ret);
               nanosleep(&sleep_ts, NULL);
               continue;
            }
         }
         switch(msg->type)
         {
         case CMS_MSG_WAN_PORT_ENABLE:
            processWanPortEnable(msg);
            break;
         case CMS_MSG_DHCPC_STATE_CHANGED:
            processDhcpcStateChanged(msg);
            break;

#ifdef SUPPORT_CELLULAR
         case CMS_MSG_CELLULARAPP_NOTIFY_EVENT:
            processCellularStateChanged(msg);
            break;
#endif

#ifdef DMP_DEVICE2_GATEWAYINFO_1
         case CMS_MSG_DHCPC_GATEWAY_INFO:
            processDhcpcGatewayInfo(msg);
            break;
#endif

         case CMS_MSG_DHCPC_REQ_OPTION_REPORT:
            processDhcpcReqOptionReport(msg);
            break;

         case CMS_MSG_DHCPD_HOST_INFO:
            processLanHostInfoMsg(msg);
            break;

         case CMS_MSG_PPPOE_STATE_CHANGED:
            processPppStateChanged(msg);
            break;

         case CMS_MSG_GET_WAN_LINK_STATUS:
            processGetWanLinkStatus(msg);
            break;

         case CMS_MSG_GET_LAN_LINK_STATUS:
            processGetLanLinkStatus(msg);
            break;
#ifdef SUPPORT_HOMEPLUG
         case CMS_MSG_GET_IF_LINK_STATUS:
            processGetIFLinkStatus(msg);
            break;
#endif
         case CMS_MSG_GET_WAN_CONN_STATUS:
            processGetWanConnStatus(msg);
            break;
            
         case CMS_MSG_SET_LOG_LEVEL:
            cmsLog_setLevel(msg->wordData);
            if ((ret = cmsMsg_sendReply(msgHandle, msg, CMSRET_SUCCESS)) != CMSRET_SUCCESS)
            {
               cmsLog_error("send response for msg 0x%x failed, ret=%d", msg->type, ret);
            }
            break;

         case CMS_MSG_SET_LOG_DESTINATION:
            cmsLog_setDestination(msg->wordData);
            if ((ret = cmsMsg_sendReply(msgHandle, msg, CMSRET_SUCCESS)) != CMSRET_SUCCESS)
            {
               cmsLog_error("send response for msg 0x%x failed, ret=%d", msg->type, ret);
            }
            break;

         case CMS_MSG_TERMINATE:
            if ((ret = cmsMsg_sendReply(msgHandle, msg, CMSRET_SUCCESS)) != CMSRET_SUCCESS)
            {
               cmsLog_error("send response for msg 0x%x failed, ret=%d", msg->type, ret);
            }
            keepLooping = FALSE;
            break;

#ifdef DMP_DEVICE2_IPPING_1
         case CMS_MSG_PING_STATE_CHANGED:
            processPingStateChanged(msg);
            break;
#endif

#ifdef DMP_DEVICE2_TRACEROUTE_1
         case CMS_MSG_TRACERT_STATE_CHANGED:
            processTracertStateChanged_dev2(msg);
            break;
#endif

#ifdef DMP_X_BROADCOM_COM_SPDSVC_1
         case CMS_MSG_SPDSVC_DIAG_COMPLETE:
            processSpeedServiceComplete(msg);
            break;
#endif

         case CMS_MSG_WATCH_WAN_CONNECTION:
            processWatchWanConnectionMsg(msg);
            break;

#ifdef SUPPORT_IPV6
         case CMS_MSG_DHCP6C_STATE_CHANGED:
            processDhcp6cStateChanged(msg);
            break;

         case CMS_MSG_RASTATUS6_INFO:
            processRAStatus6Info(msg);
            break;
#endif

#ifdef BRCM_VOICE_SUPPORT
         case CMS_MSG_SHUTDOWN_VOICE:
            processVoiceShutdown();
            break;

         case CMS_MSG_START_VOICE:
            processVoiceStart();
            break;

         case CMS_MSG_RESTART_VOICE:
            processVoiceRestart();
            break;

         case CMS_MSG_CONFIG_UPLOAD_COMPLETE:
            /* TODO: Unregister: SSK only cares about the first configuration */
            processConfigUploadComplete();
            break;

         case CMS_MSG_OMCI_VOIP_MIB_RESET:
            processMibReset();
            msg->wordData = 0;
            if ((ret = cmsMsg_sendReply(msgHandle, msg, CMSRET_SUCCESS)) != CMSRET_SUCCESS)
            {
               cmsLog_error("send response for msg 0x%x failed, ret=%d", msg->type, ret);
            }
            break;

         case CMS_MSG_INIT_VOICE:
            initializeVoice();
            break;

         case CMS_MSG_DEINIT_VOICE:
            deInitializeVoice();
            break;

         case CMS_MSG_DEFAULT_VOICE:
            defaultVoice();
            break;
#endif /* BRCM_VOICE_SUPPORT */

#ifdef DMP_X_BROADCOM_COM_EPON_1
         case CMS_MSG_EPONMAC_BOOT_COMPLETE:
#ifdef BRCM_VOICE_SUPPORT
            processEponMacBootInd();
#endif /* BRCM_VOICE_SUPPORT */
            break;
         case CMS_MSG_EPON_SET_QUEUE_CONFIG:
            processEponSetQueueConfig(msg);
            break;
         case CMS_MSG_EPON_GET_QUEUE_CONFIG:
            processEponGetQueueConfig(msg);
            break;
#endif /* DMP_X_BROADCOM_COM_EPON_1 */


#ifdef DMP_STORAGESERVICE_1
         case CMS_MSG_STORAGE_ADD_PHYSICAL_MEDIUM:
            processAddPhysicalMediumMsg(msg);
            break;
         case CMS_MSG_STORAGE_REMOVE_PHYSICAL_MEDIUM:
            processRemovePhysicalMediumMsg(msg);
            break;
         case CMS_MSG_STORAGE_ADD_LOGICAL_VOLUME:
            processAddLogicalVolumeMsg(msg);
            break;
         case CMS_MSG_STORAGE_REMOVE_LOGICAL_VOLUME:
            processRemoveLogicalVolumeMsg(msg);
            break;
#endif

#ifdef DMP_DEVICE2_USBHOSTSBASIC_1
         case CMS_MSG_USB_DEVICE_STATE_CHANGE:
            processUsbDeviceStateChange(msg);
            break;
#endif

         case CMS_MSG_GET_DEVICE_INFO:
            processGetDeviceInfo(msg);
            break;

#ifdef DMP_BRIDGING_1
         case CMS_MSG_DHCPD_DENY_VENDOR_ID:
            processDhcpdDenyVendorId(msg);
            break;
#endif
         case CMS_MSG_REQUEST_FOR_PPP_CHANGE:
            processRequestPppChange(msg);
            break;

#ifdef DMP_DSLDIAGNOSTICS_1
         case CMS_MSG_WATCH_DSL_LOOP_DIAG:
            processWatchDslLoopDiag(msg);
            break;
#endif 
#ifdef DMP_X_BROADCOM_COM_SELT_1
         case CMS_MSG_WATCH_DSL_SELT_DIAG:
            processWatchDslSeltDiag(msg);
            break;
#endif 

#if defined (DMP_TIME_1) || defined (DMP_DEVICE2_TIME_1)
         case CMS_MSG_TIME_STATE_CHANGED:
            processTimeStateChanged(msg->wordData);
            break;
#endif

         case CMS_MSG_VENDOR_CONFIG_UPDATE:
            processVendorConfigUpdate(msg);
            break;

#ifdef SUPPORT_DEBUG_TOOLS
         case CMS_MSG_MEM_DUMP_STATS:
            cmsMem_dumpMemStats();
            break;
#endif

#ifdef CMS_MEM_LEAK_TRACING
         case CMS_MSG_MEM_DUMP_TRACEALL:
            cmsMem_dumpTraceAll();
            break;

         case CMS_MSG_MEM_DUMP_TRACE50:
            cmsMem_dumpTrace50();
            break;

         case CMS_MSG_MEM_DUMP_TRACECLONES:
            cmsMem_dumpTraceClones();
            break;
#endif

#ifdef SUPPORT_MOCA
#ifdef BRCM_MOCA_DAEMON
         case CMS_MSG_MOCA_WRITE_LOF:
            processMoCAWriteLof(msg);
            break;

         case CMS_MSG_MOCA_READ_LOF:
            processMoCAReadLof(msg);
            break;

         case CMS_MSG_MOCA_WRITE_MRNONDEFSEQNUM:
            processMoCAWriteMRNonDefSeqNum(msg);
            break;

         case CMS_MSG_MOCA_READ_MRNONDEFSEQNUM:
            processMoCAReadMRNonDefSeqNum(msg);
            break;

         case CMS_MSG_MOCA_NOTIFICATION:
            processMoCANotification(msg);
            break;
#endif
#endif

#ifdef DMP_X_BROADCOM_COM_GPONWAN_1
         case CMS_MSG_OMCI_GPON_WAN_SERVICE_STATUS_CHANGE:
            processGponWanServiceStatusChange(msg);
            break;
#endif /* DMP_X_BROADCOM_COM_GPONWAN_1 */

#ifdef DMP_X_BROADCOM_COM_EPONWAN_1
         case CMS_MSG_EPON_LINK_STATUS_CHANGED:
            processEponWanLinkChange(msg);
            break;
#endif /* DMP_X_BROADCOM_COM_EPONWAN_1 */

#if defined (DMP_X_BROADCOM_COM_GPONWAN_1) || defined (DMP_X_BROADCOM_COM_EPONWAN_1)
         case CMS_MSG_WAN_PORT_SET_OPSTATE:
            processWanLinkOpState(msg);
            break;
#endif /* (DMP_X_BROADCOM_COM_GPONWAN_1) || (DMP_X_BROADCOM_COM_EPONWAN_1) */

#ifdef DMP_DEVICE2_HOMEPLUG_1
         case CMS_MSG_HOMEPLUG_LINK_STATUS_CHANGED:
            /* homeplug intfName is in the body of the msg */
            if (msg->dataLength > strlen(HOMEPLUG_IFC_STR))
            {
               updateLinkStatus((char *)(msg+1));
            }
            else
            {
               cmsLog_error("Expected plc intfName in msgBody, got length %d",
                             msg->dataLength);
            }
            break;
#endif

#ifdef DMP_DEVICE2_BASELINE_1
         case CMS_MSG_INTFSTACK_LOWERLAYERS_CHANGED:
            processIntfStackLowerLayersChangedMsg(msg);
            break;

         case CMS_MSG_INTFSTACK_OBJECT_DELETED:
            processIntfStackObjectDeletedMsg(msg);
            break;

         case CMS_MSG_INTFSTACK_ALIAS_CHANGED:
            processIntfStackAliasChangedMsg(msg);
            break;

         case CMS_MSG_INTFSTACK_PROPAGATE_STATUS:
            processIntfStackPropagateMsg(msg);
            break;

         case CMS_MSG_INTFSTACK_STATIC_ADDRESS_CONFIG:
            processIntfStackStaticAddressConfigdMsg(msg);
            break;
#endif /* DMP_DEVICE2_BASELINE_1 */

#if defined(DMP_PERIODICSTATSADV_1) || defined(DMP_DEVICE2_PERIODICSTATSADV_1)
         case CMS_MSG_PERIODICSTAT_STATUS_CHANGE:
            processSampleSetStatusChanged(msg);
            break;
#endif

#if defined(DMP_DEVICE2_WIFIACCESSPOINT_1)
         case CMS_MSG_WIFI_UPDATE_ASSOCIATEDDEVICE:
            processAssociatedDeviceUpdated_dev2(msg);
            break;
#endif /* DMP_DEVICE2_WIFIACCESSPOINT_1 */

         case CMS_MSG_INTERNAL_NOOP:
            /* just ignore this message.  It will get freed below. */
            break;

         case CMS_MSG_MDM_POST_ACTIVATING:            
            processPostMdmAct(msg);
            break;
            
         default:
            cmsLog_error("cannot handle msg type 0x%x from %d (flags=0x%x)",
                         msg->type, msg->src, msg->flags);
            break;
         }

         CMSMEM_FREE_BUF_AND_NULL_PTR(msg);
      }  /* end of if (FD_ISSET(commFd, &readFds)) */

#ifdef SUPPORT_MOCA
#ifndef BRCM_MOCA_DAEMON
      if (mocaMonitorFd != CMS_INVALID_FD && FD_ISSET(mocaMonitorFd, &readFds))
      {
         processMocaMonitor();
      }
#endif
#endif

#if defined(DMP_DSLDIAGNOSTICS_1) || defined(DMP_X_BROADCOM_COM_SELT_1)
      if (dslDiagInProgress == TRUE)
      {
         getDslDiagResults();
      }      
#endif /* DMP_DSLDIAGNOSTICS_1 */

   }

   return exitCode;
}


CmsRet ssk_init(void)
{
   CmsRet ret;
   CmsMsgHeader *msg=NULL;
   SINT32 sessionPid;
   UINT32 sdramSz;


   cmsTms_get(&bootTimestamp);

   /*
    * Detach myself from the terminal so I don't get any control-c/sigint.
    * On the desktop, it is smd's job to catch control-c and exit.
    * When ssk detects that smd has exited, ssk will also exit.
    */
   if ((sessionPid = setsid()) == -1)
   {
      cmsLog_error("Could not detach from terminal");
   }
   else
   {
      cmsLog_debug("detached from terminal");
   }

   if ((ret = cmsMsg_initWithFlags(myEid, 0, &msgHandle)) != CMSRET_SUCCESS)
   {
      cmsLog_error("msg initialization failed, ret=%d", ret);
      return ret;
   }

   /*
    * ssk will get an event message on system boot.
    */
   if ((ret = cmsMsg_receive(msgHandle, &msg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not receive initial msg, ret=%d", ret);
      return ret;
   }

   if (msg->type != CMS_MSG_SYSTEM_BOOT)
   {
      cmsLog_error("Unexpected initial msg, got 0x%x", msg->type);
      cmsMem_free(msg);
      return CMSRET_INTERNAL_ERROR;
   }


#if defined(SUPPORT_SAMBA)
   registerInterestInEvent(CMS_MSG_STORAGE_ADD_PHYSICAL_MEDIUM, TRUE, NULL, 0);
   registerInterestInEvent(CMS_MSG_STORAGE_REMOVE_PHYSICAL_MEDIUM, TRUE, NULL, 0);
#endif
   registerInterestInEvent(CMS_MSG_STORAGE_ADD_LOGICAL_VOLUME, TRUE, NULL, 0);
   registerInterestInEvent(CMS_MSG_STORAGE_REMOVE_LOGICAL_VOLUME, TRUE, NULL, 0);

#if defined(SUPPORT_DM_LEGACY98)
   printf("Initializing CMS MDM in Legacy98 mode\n");
#elif defined(SUPPORT_DM_HYBRID)
   printf("Initializing CMS MDM in Hybrid98+181 mode\n");
#elif defined(SUPPORT_DM_PURE181)
   printf("Initializing CMS MDM in Pure181 mode\n");
#elif defined(SUPPORT_DM_DETECT)
   printf("Initializing CMS MDM in Data Model Detect mode\n");
#endif

   /*
    * The first call to cmsMdm_init is done by ssk because MDM initialization
    * may cause RCL handler functions to send messages to smd.  So smd 
    * cannot call any MDM functions which would cause RCL or STL handler 
    * functions to send messagse to itself.
    */
   if ((ret = cmsMdm_initWithAcc(myEid, 0, msgHandle, &shmId)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_init failed, ret=%d", ret);
      cmsMem_free(msg);
      return ret;
   }

#ifdef SUPPORT_DM_DETECT
   printf("CMS MDM initialized in %s mode\n",
           cmsMdm_isDataModelDevice2() ? "Pure181" : "Hybrid98+181");
#endif

   cmsLog_debug("cmsMdm_init successful, shmId=%d", shmId);


   initLoggingFromConfig();


   /*
    * Send a CMS_MSG_MDM_INITIALIZED event msg back to smd.  This will
    * trigger stage 2 of the smd oal_launchOnBoot process.  Also use this
    * message to pass some info back to smd.
    */
   {
      char initMsgBuf[sizeof(CmsMsgHeader)+sizeof(UINT32)]={0};
      CmsMsgHeader *initMsg = (CmsMsgHeader *) initMsgBuf;
      UINT32 *dmPtr = (UINT32 *) (initMsg+1);

      initMsg->type = CMS_MSG_MDM_INITIALIZED;
      initMsg->src = myEid;
      initMsg->dst = EID_SMD;
      initMsg->flags_event = 1;
      initMsg->wordData = getSmdLogSettingsFromConfig();
      initMsg->dataLength = sizeof(UINT32);
      *dmPtr = (UINT32) cmsMdm_isDataModelDevice2();

      if ((ret = cmsMsg_send(msgHandle, initMsg)) != CMSRET_SUCCESS)
      {
         cmsLog_error("MDM init event msg failed. ret=%d", ret);
      }
   }


   /*
    * Initialize special netlink socket to kernel to receive proprietary
    * link-up, link-down, and link status change events.  We are migrating
    * away from this mechanism, although it is still needed for DSL.
    */
   if ((ret = initKernelMonitorFd()) != CMSRET_SUCCESS)
   {
      return ret;
   }

   /* initialize standard netlink socket to receive link up/down events */
   if ((ret = initStdNetlinkMonitorFd()) != CMSRET_SUCCESS)
   {
      return ret;
   }

#ifdef SUPPORT_MOCA
#ifndef BRCM_MOCA_DAEMON
   initMocaMonitorFd();
#endif
#endif

#ifdef SUPPORT_IPV6
   /*
    * Register interest for the CMS_MSG_DHCP6C_STATE_CHANGED event msg.
    * mwang: this needs to change, dhcpc should send event change directly
    * to ssk, ssk then sends event change to smd for general broadcast.
    */
   cmsLog_debug("registering interest for DHCP6C_STATE_CHANGED event msg");
   registerInterestInEvent(CMS_MSG_DHCP6C_STATE_CHANGED, TRUE, NULL, 0);
#endif

#if defined (DMP_TIME_1) || defined (DMP_DEVICE2_TIME_1)
   /*
    * Register interest for the CMS_MSG_TIME_STATE_CHANGED event msg.
    */
   cmsLog_debug("registering interest for TIME_STATE_CHANGED event msg");
   registerInterestInEvent(CMS_MSG_TIME_STATE_CHANGED, TRUE, NULL, 0);
#endif /* DMP_TIME_1 or DMP_DEVICE2_TIME_1 */

   CMSMEM_FREE_BUF_AND_NULL_PTR(msg);


#ifdef BRCM_VOICE_SUPPORT
   /*
    * Register interest for the CMS_MSG_CONFIG_UPLOAD_COMPLETE event msg.
    * (reuse previous msg)
    */
   registerInterestInEvent(CMS_MSG_CONFIG_UPLOAD_COMPLETE, TRUE, NULL, 0);

   /*
    * Initialize ssk_voice state 
    */
   initializeVoice();
#endif

   /*
    * See if we are running on a 8MB board.
    */
   sdramSz = devCtl_getSdramSize();
   if (sdramSz <= SZ_8MB)
   {
      cmsLog_debug("This is a low memory system, sdramSz=%d", sdramSz);
      isLowMemorySystem = TRUE;
   }


   /*
    * Do an initial scan on the link status of all the WAN and LAN interfaces.
    * We have to scan the LAN interfaces because we might not get a kernel
    * event if the eth link comes up during kernel boot.  We have to scan
    * the WAN interface because we could be configured for ethWan and
    * may not get a kernel event if the ethWan link comes up during kernel boot.
    * This has the side effect of starting voice if the BoundIfName and
    * the link that is up matches.
    */
   updateLinkStatus(NULL);


#ifdef DMP_STORAGESERVICE_1
   initStorageService();
#endif

   cmsLog_notice("done, ret=%d", ret);

   return ret;
}


static void initLoggingFromConfig()
{
   SskCfgObject *obj;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      cmsLck_dumpInfo();
      return;
   }

   if ((ret = cmsObj_get(MDMOID_SSK_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of SSK_CFG object failed, ret=%d", ret);
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


/** Get the SMD log settings from the MDM so that it can be sent to smd.
 *
 * We do this so smd does not have to acquire the CMS lock to get its log
 * settings.  Smd should avoid getting the CMS lock to avoid deadlock
 * situations.
 */
UINT32 getSmdLogSettingsFromConfig()
{
   SmdCfgObject *obj;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;
   UINT32 logSettings=0;

   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      cmsLck_dumpInfo();
      return logSettings;
   }

   if ((ret = cmsObj_get(MDMOID_SMD_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of SMD_CFG object failed, ret=%d", ret);
   }
   else
   {
      UINT32 level;
      UINT32 dest;

      level = cmsUtl_logLevelStringToEnum(obj->loggingLevel);
      dest = cmsUtl_logDestinationStringToEnum(obj->loggingDestination);
      logSettings = (dest & 0xffff) << 16;
      logSettings |= (level & 0xffff);

      cmsObj_free((void **) &obj);
   }

   cmsLck_releaseLock();

   return logSettings;
}

static void registerInterestInEvent(CmsMsgType msgType, UBOOL8 positive, void *msgData, UINT32 msgDataLen)
{
   CmsMsgHeader *msg = NULL;
   char *data = NULL;
   void *msgBuf = NULL;
   CmsRet ret;

   if (msgData != NULL && msgDataLen != 0)
   {
      /* for msg with user data */
      msgBuf = cmsMem_alloc(sizeof(CmsMsgHeader) + msgDataLen, ALLOC_ZEROIZE);
   }
   else
   {
      msgBuf = cmsMem_alloc(sizeof(CmsMsgHeader), ALLOC_ZEROIZE);
   }

   msg = (CmsMsgHeader *)msgBuf;

   /* fill in the msg header */
   msg->type = (positive) ? CMS_MSG_REGISTER_EVENT_INTEREST : CMS_MSG_UNREGISTER_EVENT_INTEREST;
   msg->src = myEid;
   msg->dst = EID_SMD;
   msg->flags_request = 1;
   msg->wordData = msgType;

   if (msgData != NULL && msgDataLen != 0)
   {
      data = (char *) (msg + 1);
      msg->dataLength = msgDataLen;
      memcpy(data, (char *)msgData, msgDataLen);
   }

   ret = cmsMsg_sendAndGetReply(msgHandle, msg);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("%s_EVENT_INTEREST for 0x%x failed, ret=%d", (positive) ? "REGISTER" : "UNREGISTER", msgType, ret);
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(msgBuf);

   return;
}

#ifdef DMP_BASELINE_1
void updateLinkStatus_igd(const char *intfName)
{
   UBOOL8 found;
   UBOOL8 gotLock=FALSE;
   CmsRet ret;

   cmsLog_debug("Entered: intfName=%s", intfName);

   /*
    * Very bad if we lose a link status change event.  Keep looping until
    * we get the lock.
    */
   while (!gotLock)
   {
      if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
      {
         cmsLog_error("could not get lock, ret=%d", ret);
         cmsLck_dumpInfo();
      }
      else
      {
         gotLock = TRUE;
      }
   }

   /* Check WAN side interfaces */
   found = checkWanLinkStatusLocked_igd(intfName);
   if (found)
   {
      cmsLck_releaseLock();
      return;
   }

   /* Check LAN side interfaces */
   found = checkLanLinkStatusLocked_igd(intfName);
   if (found)
   {
      cmsLck_releaseLock();
      return;
   }

   cmsLck_releaseLock();

   return;
}
#endif  /* DMP_BASELINE_1 */


/** Kernel traffic type mismatch , call handler function to determine if we
 *  need to reboot the system taking account of the traffic type mismatch.
 */
void processTrafficMismatchMessage(unsigned int msgData __attribute__((unused)))
{
   CmsRet ret;   

   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get lock, ret=%d", ret);
      cmsLck_dumpInfo();
      /* just a kernel event, I guess we can try later. */
      return;
   }

#ifdef SUPPORT_DSL_BONDING
       setWanDslTrafficType () ;
#endif

   cmsLck_releaseLock();

   return;
}



    /* For the 63138 and 63148, implement a workaround to strip bytes and
       allow OAM traffic due to JIRA HW63138-12 */
#if defined (CONFIG_BCM963138) || defined (CONFIG_BCM963148)

/** Stub routine to run when the OAM strip byte workaround is
 *  activated and deactivated.
 */
void processOamStripByte(UINT32 msgId)
{
    /* If parameter is zero, workaround is activated.  If not, it is deactivated. */
    if(msgId == 0)
    {
        cmsLog_debug("OAM Strip Byte Workaround Activated");
    }
    else
    {
        cmsLog_debug("OAM Strip Byte Workaround Deactivated");
    }

   return;
}

#endif


#ifdef DMP_ADSLWAN_1
void processXdslCfgSaveMessage_igd(UINT32 msgId)
{
    long    dataLen;
    char    oidStr[] = { 95 };      /* kOidAdslPhyCfg */
    adslCfgProfile  adslCfg;
    CmsRet          cmsRet;
    WanDslIntfCfgObject *dslIntfCfg = NULL;
    InstanceIdStack         iidStack = EMPTY_INSTANCE_ID_STACK;
    
    dataLen = sizeof(adslCfgProfile);
    cmsRet = xdslCtl_GetObjectValue(0, oidStr, sizeof(oidStr), (char *)&adslCfg, &dataLen);
    
    if( cmsRet != (CmsRet) BCMADSL_STATUS_SUCCESS) {
        cmsLog_error("Could not get adsCfg, ret=%d", cmsRet);
        return;
    }
    
    if ((cmsRet = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS) {
        cmsLog_error("Could not get lock, ret=%d", cmsRet);
        cmsLck_dumpInfo();
        /* just a kernel event, I guess we can try later. */
        return;
    }
    
    cmsRet = cmsObj_getNext(MDMOID_WAN_DSL_INTF_CFG, &iidStack, (void **) &dslIntfCfg);
    if (cmsRet != CMSRET_SUCCESS) {
        cmsLck_releaseLock();
        cmsLog_error("Could not get DSL intf cfg, ret=%d", cmsRet);
        return;
    }
    
    if(MSG_ID_BRCM_SAVE_DSL_CFG_ALL == msgId)
        xdslUtil_IntfCfgInit(&adslCfg, dslIntfCfg);
#if defined(SUPPORT_MULTI_PHY) || defined(SUPPORT_DSL_GFAST)
    else if (MSG_ID_BRCM_SAVE_DSL_PREFERRED_LINE == msgId) {
        dslIntfCfg->X_BROADCOM_COM_DslPhyMiscCfgParam &= ~(BCM_PREFERREDTYPE_FOUND | BCM_MEDIATYPE_MSK);
        dslIntfCfg->X_BROADCOM_COM_DslPhyMiscCfgParam |= (adslCfg.xdslMiscCfgParam & (BCM_PREFERREDTYPE_FOUND | BCM_MEDIATYPE_MSK));
    }
#endif
    else {
        cmsObj_free((void **) &dslIntfCfg);
        cmsLck_releaseLock();
        return;
    }
    
    cmsRet = cmsObj_set(dslIntfCfg, &iidStack);
    if (cmsRet != CMSRET_SUCCESS)
        cmsLog_error("Could not set DSL intf cfg, ret=%d", cmsRet);
    else
        cmsRet = cmsMgm_saveConfigToFlash();
    
    cmsObj_free((void **) &dslIntfCfg);
    
    cmsLck_releaseLock();
    
    if(cmsRet != CMSRET_SUCCESS)
        cmsLog_error("Writing  Xdsl Cfg to flash.failed!");
}
#endif  /* DMP_ADSLWAN_1 */


/* opens a netlink socket and intiliaze it to recieve messages from
 * kernel for protocol NETLINK_BRCM_MONITOR
 */
CmsRet initKernelMonitorFd()
{
   CmsRet ret=CMSRET_SUCCESS;

#ifndef DESKTOP_LINUX
   struct sockaddr_nl addr;

   if ((kernelMonitorFd = socket(AF_NETLINK, SOCK_RAW, NETLINK_BRCM_MONITOR)) < 0)
   //if ((kernelMonitorFd = socket(AF_NETLINK, SOCK_RAW, NETLINK_UNUSED)) < 0)
   {
      cmsLog_error("Could not open netlink socket for kernel monitor");
      return CMSRET_INTERNAL_ERROR;
   }
   else
   {
      cmsLog_debug("kernelMonitorFd=%d", kernelMonitorFd);
   }

    addr.nl_family = AF_NETLINK;
    addr.nl_pid = getpid();
    addr.nl_groups = 0;

    if (bind(kernelMonitorFd,(struct sockaddr *)&addr,sizeof(addr))<0)
    {
       cmsLog_error("Could not bind netlink socket for kernel monitor");
       close(kernelMonitorFd);
       kernelMonitorFd = CMS_INVALID_FD;
       return CMSRET_INTERNAL_ERROR;
    }

   /*send the pid of ssk to kernel,so that it can send events */

   ret = devCtl_boardIoctl(BOARD_IOCTL_SET_MONITOR_FD, 0, "", 0, getpid(), "");
   if (ret == CMSRET_SUCCESS)
   {
      cmsLog_debug("registered fd %d with kernel monitor", kernelMonitorFd);
   }
   else
   {
      cmsLog_error("failed to register fd %d with kernel monitor, ret=%d", kernelMonitorFd, ret);
      close(kernelMonitorFd);
      kernelMonitorFd = CMS_INVALID_FD;
   }
#endif  /* DESKTOP_LINUX */

   return ret;
}

/** Receive and process netlink messages from Broadcom proprietary/legacy
 * netlink socket.
 */
void processKernelMonitor(void)
{
   int recvLen;
   char buf[4096];
   struct iovec iov = { buf, sizeof buf };
   struct sockaddr_nl nl_srcAddr;
   struct msghdr msg ;
   struct nlmsghdr *nl_msgHdr;
   unsigned int nl_msgData ;

   memset(&msg,0,  sizeof(struct msghdr));

   msg.msg_name = (void*)&nl_srcAddr;
   msg.msg_namelen = sizeof(nl_srcAddr);
   msg.msg_iov = &iov;
   msg.msg_iovlen = 1 ;


   cmsLog_debug("Enter\n");

   recvLen = recvmsg(kernelMonitorFd, &msg, 0);

   if(recvLen < 0)
   {
      if (errno == EWOULDBLOCK || errno == EAGAIN)
         return ;

      /* Anything else is an error */
      cmsLog_error("read_netlink: Error recvmsg: %d\n", recvLen);
      perror("read_netlink: Error: ");
      return ;
   }

   if(recvLen == 0)
   {
      cmsLog_error("read_netlink: EOF\n");
   }

   /* There can be  more than one message per recvmsg */
   for(nl_msgHdr = (struct nlmsghdr *) buf; NLMSG_OK (nl_msgHdr, (unsigned int)recvLen); 
         nl_msgHdr = NLMSG_NEXT (nl_msgHdr, recvLen))
   {
      /* Finish reading */
      if (nl_msgHdr->nlmsg_type == NLMSG_DONE)
         return ;

      /* Message is some kind of error */
      if (nl_msgHdr->nlmsg_type == NLMSG_ERROR)
      {
         cmsLog_error("read_netlink: Message is an error \n");
         return ; // Error
      }

      /*Currently we expect messages only from kernel, make sure
       * the message is from kernel 
       */
      if(nl_msgHdr->nlmsg_pid !=0)
      {
         cmsLog_error("netlink message source(%d)is not kernel",nl_msgHdr->nlmsg_pid);
         return;
      }

      /* Call message handler */
      switch (nl_msgHdr->nlmsg_type)
      {
         case MSG_NETLINK_BRCM_WAKEUP_MONITOR_TASK:
         case MSG_NETLINK_BRCM_LINK_STATUS_CHANGED:
            /*process the message */
            cmsLog_debug("received LINK_STATUS_CHANGED message\n"); 
            updateLinkStatus(NULL);
            break;
         case MSG_NETLINK_BRCM_LINK_TRAFFIC_TYPE_MISMATCH   :
            /*process the message */
            cmsLog_debug("received LINK_TRAFFIC_TYPE_MISMATCH message\n");
            memcpy ((char *) &nl_msgData, NLMSG_DATA(nl_msgHdr), sizeof (nl_msgData)) ;
            processTrafficMismatchMessage(nl_msgData) ;
            break;

    /* For the 63138 and 63148, implement a workaround to strip bytes and
       allow OAM traffic due to JIRA HW63138-12 */
#if defined (CONFIG_BCM963138) || defined (CONFIG_BCM963148)
          case MSG_NETLINK_BRCM_LINK_OAM_STRIP_BYTE   :
             /*process the message */
              cmsLog_debug("received LINK_OAM_STRIP_BYTE message\n");
              memcpy ((char *) &nl_msgData, NLMSG_DATA(nl_msgHdr), sizeof (nl_msgData)) ;
              processOamStripByte(nl_msgData) ;
             break;
#endif

#ifdef SUPPORT_DSL
         case MSG_NETLINK_BRCM_SAVE_DSL_CFG:
            cmsLog_debug("received SAVE_DSL_CFG  message\n");
            nl_msgData = MSG_ID_BRCM_SAVE_DSL_CFG_ALL;
            if(nl_msgHdr->nlmsg_len > NLMSG_HDRLEN)
                memcpy ((char *) &nl_msgData, NLMSG_DATA(nl_msgHdr), sizeof (nl_msgData));
            processXdslCfgSaveMessage(nl_msgData);
            break;
         case MSG_NETLINK_BRCM_CALLBACK_DSL_DRV:
            cmsLog_debug("received CALLBACK_DSL_DRV  message\n");
            xdslCtl_CallBackDrv(0);
            break;
#endif
         default:
            cmsLog_error(" Unknown netlink nlmsg_type %d\n",
                  nl_msgHdr->nlmsg_type);
            break;
      }
   }

   cmsLog_debug("Exit\n");
   return;
}


/* open a netlink socket to receive standard link up/down events from kernel.
 */
CmsRet initStdNetlinkMonitorFd()
{
   CmsRet ret=CMSRET_SUCCESS;

#ifndef DESKTOP_LINUX
   struct sockaddr_nl addr;

   /* Open a socket for generic Linux link status monitoring */
    stdNetlinkMonitorFd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (stdNetlinkMonitorFd < 0)
    {
       cmsLog_error("Could not open stdNetlink socket!");
       return CMSRET_INTERNAL_ERROR;
    }
    cmsLog_debug("opened stdNetlinkMonitorFd at %d", stdNetlinkMonitorFd);

    memset(&addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    addr.nl_groups = RTMGRP_LINK;
    if (0 > bind(stdNetlinkMonitorFd, (struct sockaddr *) &addr, sizeof(addr)))
    {
       cmsLog_error("Could not bind stdNetlinkMonitorFd");
       close(stdNetlinkMonitorFd);
       return CMSRET_INTERNAL_ERROR;
    }
    cmsLog_debug("bound stdNetlinkMonitorFd to 0x%x", addr.nl_groups);
#endif  /* DESKTOP_LINUX */

    return ret;
}


void processStdNetlinkMonitor()
{
   int recvLen;
   char buf[4096];
   struct iovec iov = { buf, sizeof buf };
   struct sockaddr_nl nl_srcAddr;
   struct msghdr msg ;
   struct nlmsghdr *nl_msgHdr;

   memset(&msg,0,  sizeof(struct msghdr));

   msg.msg_name = (void*)&nl_srcAddr;
   msg.msg_namelen = sizeof(nl_srcAddr);
   msg.msg_iov = &iov;
   msg.msg_iovlen = 1 ;


   cmsLog_debug("Enter");

   recvLen = recvmsg(stdNetlinkMonitorFd, &msg, 0);

   cmsLog_debug("recvLen=%d", recvLen);

   if (recvLen <= 0)
   {
      return;
   }

   /* There can be  more than one message per recvmsg */
   for(nl_msgHdr = (struct nlmsghdr *) buf;
       NLMSG_OK (nl_msgHdr, (unsigned int)recvLen);
       nl_msgHdr = NLMSG_NEXT (nl_msgHdr, recvLen))
   {
      char intfNameBuf[CMS_IFNAME_LENGTH];

      memset(intfNameBuf, 0, sizeof(intfNameBuf));

      /* Finish reading */
      if (nl_msgHdr->nlmsg_type == NLMSG_DONE)
      {
         cmsLog_debug("got DONE msg");
         return ;
      }

      /* Message is some kind of error */
      if (nl_msgHdr->nlmsg_type == NLMSG_ERROR)
      {
         cmsLog_error("read_netlink: Message is an error \n");
         return ; // Error
      }

      switch (nl_msgHdr->nlmsg_type)
      {
         case RTM_NEWLINK:
         {
            struct ifinfomsg *info = (struct ifinfomsg *) NLMSG_DATA(nl_msgHdr);
            // cmsLog_debug("got NEWLINK! (len=%d)", nl_msgHdr->nlmsg_len);
            if (0 > cmsNet_getIfnameByIndex(info->ifi_index, intfNameBuf))
            {
               /* ptm0 interface disappears when link goes down, so cannot
                * complain too loudly here.
                */
               cmsLog_debug("Could not find intfName for index %d", info->ifi_index);
            }
            else
            {
               cmsLog_debug("NEWLINK index=%d => %s (flags=0x%x %s %s)",
                            info->ifi_index, intfNameBuf,
                            info->ifi_flags,
                     ((info->ifi_flags & IFF_UP) ? "IFF_UP" : ""),
                     ((info->ifi_flags & IFF_RUNNING) ? "IFF_RUNNING" : ""));
            }

            break;
         }

         case RTM_DELLINK:
         {
            struct ifinfomsg *info = (struct ifinfomsg *) NLMSG_DATA(nl_msgHdr);
            // cmsLog_debug("got DELLINK! (len=%d)", nl_msgHdr->nlmsg_len);
            if (0 > cmsNet_getIfnameByIndex(info->ifi_index, intfNameBuf))
            {
               /* ptm0 interface disappears when link goes down, so cannot
                * complain too loudly here.
                */
               cmsLog_debug("Could not find intfName for index %d", info->ifi_index);
            }
            else
            {
               cmsLog_debug("DELLINK index=%d => %s (flags=0x%x)",
                            info->ifi_index, intfNameBuf, info->ifi_flags);
            }
            break;
         }

         default:
            cmsLog_debug("got %d (len=%d)", nl_msgHdr->nlmsg_type, nl_msgHdr->nlmsg_len);
            break;
      }

      /*
       * Do a little filtering before we do more with this link event.
       * We can definitely handle eth and wl link events.
       * Homeplug/plc and GPON notifies ssk of linkstatus using a CMS msg,
       * so does not need to be handled here.
       * USB, EPON, and Moca, not yet, but we should also use this mechanism.
       * All new interfaces should use this mechanism.
       * Do not use this mechanism for everything else (DSL, ppp).
       */
      if (!cmsUtl_strncmp(intfNameBuf, ETH_IFC_STR, strlen(ETH_IFC_STR)) ||
          !cmsUtl_strncmp(intfNameBuf, WLAN_IFC_STR, strlen(WLAN_IFC_STR)) ||
          !cmsUtl_strncmp(intfNameBuf, USB_IFC_STR, strlen(USB_IFC_STR)) ||
          !cmsUtl_strncmp(intfNameBuf, EPON_IFC_STR, strlen(EPON_IFC_STR)) ||
          !cmsUtl_strncmp(intfNameBuf, MOCA_IFC_STR, strlen(MOCA_IFC_STR)) ||
          !cmsUtl_strncmp(intfNameBuf, HOMEPLUG_IFC_STR, strlen(HOMEPLUG_IFC_STR)))
      {
         updateLinkStatus(intfNameBuf);
      }
   }

   cmsLog_debug("exit");

   return;
}


void processPingStateChanged_igd(CmsMsgHeader *msg)
{
   PingDataMsgBody *pingInfo = (PingDataMsgBody *) (msg + 1);
   IPPingDiagObject *ipPingObj= NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   cmsLog_debug("dataLength=%d state=%s",msg->dataLength, pingInfo->diagnosticsState);
   if (pingInfo->interface)
   {
      cmsLog_debug("interface %s",pingInfo->interface);
   }
   if (pingInfo->host)
   {
      cmsLog_debug("host %s",pingInfo->host);
   }
   cmsLog_debug("success %d, fail %d, avg/min/max %d/%d/%d requestId %d",pingInfo->successCount,
                pingInfo->failureCount,pingInfo->averageResponseTime,pingInfo->minimumResponseTime,
                pingInfo->maximumResponseTime,(int)pingInfo->requesterId);
      
      
   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get lock, ret=%d", ret);
      cmsLck_dumpInfo();
      return;
   }

   cmsLog_debug("calling cmsObjGet");

   if ((ret = cmsObj_get(MDMOID_IP_PING_DIAG, &iidStack, 0, (void **) &ipPingObj)) == CMSRET_SUCCESS)
   {
      cmsLog_debug("cmsObj_get, MDMOID_IP_PING_DIAG success");

      CMSMEM_FREE_BUF_AND_NULL_PTR(ipPingObj->diagnosticsState);
      CMSMEM_FREE_BUF_AND_NULL_PTR(ipPingObj->host);
      ipPingObj->diagnosticsState = cmsMem_strdup(pingInfo->diagnosticsState);
      ipPingObj->host = cmsMem_strdup(pingInfo->host);
      ipPingObj->successCount = pingInfo->successCount;
      ipPingObj->failureCount = pingInfo->failureCount;
      ipPingObj->averageResponseTime = pingInfo->averageResponseTime;
      ipPingObj->maximumResponseTime = pingInfo->maximumResponseTime;
      ipPingObj->minimumResponseTime = pingInfo->minimumResponseTime;

      if ((ret = cmsObj_set(ipPingObj, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("set of ipPingObj failed, ret=%d", ret);
      }
      else
      {
         cmsLog_debug("set ipPingObj OK, successCount %d, failCount %d",
                      ipPingObj->successCount,ipPingObj->failureCount);
      }     
      
      cmsObj_free((void **) &ipPingObj);
   } 
   else
   {
      cmsLog_debug("cmsObj_get, MDMOID_IP_PING_DIAG ret %d",ret);
   }

   cmsLck_releaseLock();

   return;
}


#ifdef DMP_X_BROADCOM_COM_SPDSVC_1
#define SPDSVC_RESULT_FILE "/tmp/spdsvcResult"

void processSpeedServiceComplete(CmsMsgHeader *msg __attribute__((unused)))
{
   FILE *result;
   SpeedServiceObject *spdsvcObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   char status[BUFLEN_32] = {'\0'};
   char runTime[BUFLEN_64] = {'\0'};
   UINT32 goodPut, packetLoss, avgLatency;
   UINT32 payloadRate, receivedRate, receivedTime, overhead;
   int rc;
   CmsRet ret;

   result = fopen(SPDSVC_RESULT_FILE,"r");
   if (result == NULL)
   {
      return;
   }
   rc = fscanf(result, "%s", status);
   if (rc < 1)
   {
      cmsLog_error("fscanf returned %d, expected 1", rc);
   }

   if (status[strlen(status)-1] == '\n')
      status[strlen(status)-1] = '\0';

   if (strcmp(status, MDMVS_COMPLETED) == 0)
   {
      rc = fscanf(result, "%u %u %u %u %u %u %u\n",
                  &goodPut, &payloadRate, &packetLoss, &avgLatency,
                  &receivedRate, &receivedTime, &overhead);
      if (rc < 3)
      {
         cmsLog_error("fscanf returned %d, expected 3", rc);
      }
      rc = fscanf(result, "%s", runTime);
      if (rc < 1)
      {
         cmsLog_error("fscanf returned %d, expected 1", rc);
      }
   }

   fclose(result);
   unlink(SPDSVC_RESULT_FILE);
   
   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get lock, ret=%d", ret);
      cmsLck_dumpInfo();
      return;
   }
   
   cmsLog_debug("calling cmsObj_get");

   if ((ret = cmsObj_get(MDMOID_SPEED_SERVICE, &iidStack, 0, (void **) &spdsvcObj)) == CMSRET_SUCCESS)
   {
      CMSMEM_REPLACE_STRING(spdsvcObj->diagnosticsState, status);
      if (strcmp(status, MDMVS_COMPLETED) == 0)
      {
         CMSMEM_REPLACE_STRING(spdsvcObj->lastRunTime, runTime);
         spdsvcObj->goodPut = goodPut;
         spdsvcObj->payloadRate = payloadRate;
         spdsvcObj->packetLoss = packetLoss;
         spdsvcObj->avgLatency = avgLatency;
         spdsvcObj->adjustReceivedRate = receivedRate;
         spdsvcObj->receivedTime = receivedTime;
         spdsvcObj->overhead = overhead;
      }
      
      if ((ret = cmsObj_set(spdsvcObj, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("set of spdsvcObj failed, ret=%d", ret);
      }
      else
      {
         cmsLog_debug("set spdsvcObj OK, goodPut:%u payloadRate:%u packetLoss:%u avgLatency:%u adjustReceivedRate:%u receivedTime:%u overhead:%u",
                      spdsvcObj->goodPut, spdsvcObj->payloadRate,
                      spdsvcObj->packetLoss, spdsvcObj->avgLatency,
                      spdsvcObj->adjustReceivedRate, spdsvcObj->receivedTime,
                      spdsvcObj->overhead);
      }     
      cmsObj_free((void **) &spdsvcObj);
   }
   else
   {
      cmsLog_error("cmsObj_get, MDMOID_SPEED_SERVICE ret %d",ret);
   }

   cmsLck_releaseLock();

#ifdef SUPPORT_WEB_SOCKETS
   {
      CmsMsgHeader cmsMsg;

      memset(&cmsMsg, 0, sizeof(CmsMsgHeader));

      cmsMsg.type = CMS_MSG_SPDSVC_DIAG_COMPLETE;
      cmsMsg.src = EID_SSK;
      cmsMsg.dst = EID_WEBSOCKD;
      cmsMsg.flags_request = 1;

      if ((ret = cmsMsg_send(msgHandle, &cmsMsg)) != CMSRET_SUCCESS)
      {
         cmsLog_error("could not send out CMS_MSG_SPDSVC_DIAG_COMPLETE to requestId %d, ret=%d", ret, EID_WEBSOCKD);
      }
      else
      {
         cmsLog_notice("sent out CMS_MSG_SPDSVC_DIAG_COMPLETE to requesterId %d", EID_WEBSOCKD);
      }   
   }   
#endif    /* SUPPORT_WEB_SOCKETS */

   return;
}
#endif /* DMP_X_BROADCOM_COM_SPDSVC_1 */

void sendStatusMsgToSmd(CmsMsgType msgType, const char *ifName)
{
   CmsMsgHeader *msgHdr;
   char *dataPtr;
   UINT32 dataLen=0;
   CmsRet ret;

   cmsLog_debug("sending status msg 0x%x", msgType);

   if (ifName != NULL)
   {
      dataLen = strlen(ifName)+1;
      cmsLog_debug("ifName=%s", ifName);
   }



   msgHdr = (CmsMsgHeader *) cmsMem_alloc(sizeof(CmsMsgHeader) + dataLen, ALLOC_ZEROIZE);
   if (msgHdr == NULL)
   {
      cmsLog_error("message header allocation failed, len of ifName=%d", strlen(ifName));
      return;
   }

   msgHdr->src = EID_SSK;
   msgHdr->dst = EID_SMD;
   msgHdr->type = msgType;
   msgHdr->flags_event = 1;
   msgHdr->dataLength = dataLen;
   
   if (ifName != NULL)
   {
      dataPtr = (char *) (msgHdr+1);
      strcpy(dataPtr, ifName);
   }

   if ((ret = cmsMsg_send(msgHandle, msgHdr)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to send event msg 0x%x to smd, ret=%d", msgType, ret);
   }

   cmsMem_free(msgHdr);   

   return;
}


/* For period task you need to handle in ssk, please put it into
 * this routine
 */
void processPeriodicTask()
{
   CmsRet ret;

   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      cmsLck_dumpInfo();
      return;
   }

#ifdef DMP_X_BROADCOM_COM_AUTODETECTION_1
   isAutoDetectionEnabled = dalAutoDetect_isAutoDetectEnabled();
   if (isAutoDetectionEnabled)
   {
      processAutoDetectTask();
   }      
#endif

   /*
    * The reason we call cmsObj_get on the DEVICE_INFO object is to
    * is force the stl_igdDeviceInfoObject() to update the upTime field
    * in the DeviceInfo object in the MDM.
    * If ssk does not periodically call cmsObj_get on this object,
    * then the field would never be updated and when a user types
    * dumpMdm, the uptime will be a very old one.  If this feature
    * is not important to your product, you can simply delete this
    * call to cmsObj_get.
    * A getParameterValues from tr69c will always trigger an update
    * to the upTime field and return the latest value.
    */
   sskPeriodic_updateUptime();

   /*update lease Times of portmapping entries */	
   sskPeriodic_expirePortMappings();

#ifdef DMP_X_BROADCOM_COM_STANDBY_1
   processPeriodicStandby(NULL);
#endif

   cmsLck_releaseLock();
}


void processGetDeviceInfo(CmsMsgHeader *msg)
{
   char buf[sizeof(CmsMsgHeader) + sizeof(GetDeviceInfoMsgBody)]={0};
   CmsMsgHeader *replyMsg=(CmsMsgHeader *) buf;
   GetDeviceInfoMsgBody *body = (GetDeviceInfoMsgBody*) (replyMsg+1);
   CmsRet ret;

   replyMsg->type = msg->type;
   replyMsg->src = EID_SSK;
   replyMsg->dst = msg->src;
   replyMsg->flags_request = 0;
   replyMsg->flags_response = 1;
   replyMsg->dataLength = sizeof(GetDeviceInfoMsgBody);

   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get lock, ret=%d", ret);
      cmsLck_dumpInfo();
      return;
   }

   qdmSystem_getDeviceInfoLocked(body->oui,body->serialNum,body->productClass);
   cmsLck_releaseLock();

   if ((ret = cmsMsg_send(msgHandle, replyMsg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("sending msg to %d failed, ret=%d", msg->dst,ret);
   }
   else
   {
      cmsLog_debug("sending msg to %d done, ret=%d", msg->dst,ret);
   }
} /* processGetDeviceInfo */

#if defined(DMP_ETHERNETWAN_1)
extern CmsRet addPersistentWanEthInterfaceObject(const char *ifName, const char *pGMACPortList);
extern CmsRet delLanEthInterfaceObject(const char *ifName); 
#endif
extern void rutLan_enableInterface(const char *lanIfName);

void processWanPortEnable(CmsMsgHeader *msg)
{
    CmsRet ret;
    char *if_name = (char *)(msg + 1);

    ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT);
    if (ret != CMSRET_SUCCESS)
        goto exit;

#if defined(DMP_ETHERNETWAN_1)
    /* Delete the lan port from the mdm and configure wan instead */
    ret = delLanEthInterfaceObject(if_name);
    if (ret)
        goto release_lock;
    ret = addPersistentWanEthInterfaceObject(if_name, NULL);
#endif
    rutLan_enableInterface(if_name);
#if defined(DMP_ETHERNETWAN_1)
release_lock:
#endif
    cmsLck_releaseLock();
exit:
    if (ret)
        cmsLog_error("Failed to enable lan port (if_name=%s, ret=%d)\n", if_name, ret);
}

void processPostMdmAct(CmsMsgHeader *msg)
{
   CmsRet ret = CMSRET_SUCCESS;
   MdmPostActNodeInfo *info = (MdmPostActNodeInfo *) (msg + 1);

   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get lock, ret=%d", ret);
      cmsLck_dumpInfo();
      return;
   }

   cmsMdm_postHandle(info->subType);
   cmsLck_releaseLock();

} /* processPostMdmAct */

#ifdef DMP_DEVICE2_GATEWAYINFO_1

void processDhcpcGatewayInfo(CmsMsgHeader *msg)
{
   Dev2GatewayInfoObject *pGatewayInfoObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;
   DhcpcGatewayInfoMsgBody *gatewayInfo = (DhcpcGatewayInfoMsgBody *) (msg + 1);

   ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT);
   if (ret != CMSRET_SUCCESS)
   {
      return;
   }
   if (cmsObj_get(MDMOID_DEV2_GATEWAY_INFO, &iidStack, 0, (void **) &pGatewayInfoObj) != CMSRET_SUCCESS)
   {
      cmsLck_releaseLock();
      return;
   }

   CMSMEM_REPLACE_STRING(pGatewayInfoObj->manufacturerOUI, gatewayInfo->ManufacturerOUI);
   CMSMEM_REPLACE_STRING(pGatewayInfoObj->serialNumber, gatewayInfo->SerialNumber);    
   CMSMEM_REPLACE_STRING(pGatewayInfoObj->productClass, gatewayInfo->ProductClass); 

   cmsObj_set(pGatewayInfoObj, &iidStack);
   cmsObj_free((void **) &pGatewayInfoObj);

   cmsLck_releaseLock();

} /* processDhcpcGatewayInfo */

#endif  /* DMP_DEVICE2_GATEWAYINFO_1 */


void processVendorConfigUpdate_igd(CmsMsgHeader *msg)
{
   VendorConfigFileObject *pVendorConfigObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;
   IGDDeviceInfoObject *pDevInfo;
   int found = 0;
   char fileName[BUFLEN_64];
   int instance=0;
   int numberOfVendorFiles;
   vendorConfigUpdateMsgBody *vendorConfig = (vendorConfigUpdateMsgBody *) (msg + 1);
   CmsMsgHeader replyMsg = EMPTY_MSG_HEADER;

   replyMsg.type = msg->type;
   replyMsg.src = EID_SSK;
   replyMsg.dst = msg->src;
   replyMsg.flags_request = 0;
   replyMsg.flags_response = 1;

   ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT);
   if (ret != CMSRET_SUCCESS)
   {
      return;
   }
   if (cmsObj_get(MDMOID_IGD_DEVICE_INFO, &iidStack, 0, (void **) &pDevInfo) != CMSRET_SUCCESS)
   {
      cmsLck_releaseLock();
      return;
   }
   numberOfVendorFiles = pDevInfo->vendorConfigFileNumberOfEntries;

   cmsLog_debug("numberOfVendorFiles %d",numberOfVendorFiles);

   cmsObj_free((void **) &pDevInfo);

   /* first look for instance of existing file */
   if (numberOfVendorFiles > 0)
   {
      if (vendorConfig->name[0] != '\0')
      {
         INIT_INSTANCE_ID_STACK(&iidStack);
         while (!found && (cmsObj_getNext(MDMOID_VENDOR_CONFIG_FILE,&iidStack, (void **) &pVendorConfigObj)) == CMSRET_SUCCESS)
         {
            if ((cmsUtl_strcmp(pVendorConfigObj->name,vendorConfig->name) == 0))
            {
               cmsLog_debug("Same entry found, update vendor config entry");
               CMSMEM_REPLACE_STRING(pVendorConfigObj->version,vendorConfig->version);
               CMSMEM_REPLACE_STRING(pVendorConfigObj->date,vendorConfig->date);
               CMSMEM_REPLACE_STRING(pVendorConfigObj->description,vendorConfig->description);
               found = 1;
               cmsObj_set(pVendorConfigObj,&iidStack);
            }
            cmsObj_free((void **) &pVendorConfigObj);
         } /* while */
      } /* vendorConfig->name is not empty */
   }
   if (!found)
   {
      if (numberOfVendorFiles == MAX_NUMBER_OF_VENDOR_CONFIG_RECORD) 
      {
         INIT_INSTANCE_ID_STACK(&iidStack);
         while ((cmsObj_getNext(MDMOID_VENDOR_CONFIG_FILE,&iidStack, (void **) &pVendorConfigObj)) == CMSRET_SUCCESS)
         {
            /* if the list is full, we delete the first instance, and add another */
            cmsLog_debug("numberOfVendorFiles has reached %d. Deleteing vendor config entry",numberOfVendorFiles);
            cmsObj_deleteInstance(MDMOID_VENDOR_CONFIG_FILE, &iidStack);               
            cmsObj_free((void **) &pVendorConfigObj);
            break;
         }
      }
      /* add the instance */
      INIT_INSTANCE_ID_STACK(&iidStack);
      if ((ret = cmsObj_addInstance(MDMOID_VENDOR_CONFIG_FILE,&iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("add instance of vendor config file returned %d", ret);
      }
      else
      {
         cmsObj_get(MDMOID_VENDOR_CONFIG_FILE, &iidStack, 0, (void **) &pVendorConfigObj);
         if (vendorConfig->name[0] == '\0')
         {
            strcpy(fileName,"ConfigFile");
            instance = iidStack.instance[iidStack.currentDepth-1];
            sprintf(pVendorConfigObj->name,"%s%d",fileName,instance);
         }
         else
         {
            CMSMEM_REPLACE_STRING(pVendorConfigObj->name,vendorConfig->name);
         }
         CMSMEM_REPLACE_STRING(pVendorConfigObj->version,vendorConfig->version);
         CMSMEM_REPLACE_STRING(pVendorConfigObj->date,vendorConfig->date);
         CMSMEM_REPLACE_STRING(pVendorConfigObj->description,vendorConfig->description);

         if ((ret = cmsObj_set(pVendorConfigObj, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("set of vendor config object failed, ret=%d", ret);
         }
         else
         {
            cmsLog_debug("vendor config entry added");
         }
         cmsObj_free((void **) &pVendorConfigObj);
      } /* add instance, ok */
   } /* !found */
   cmsMgm_saveConfigToFlash();
   cmsLck_releaseLock();

   if ((ret = cmsMsg_send(msgHandle, &replyMsg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("msg send failed, ret=%d",ret);
   }
   
} /* processVendorConfigUpdate */

#ifdef DMP_X_BROADCOM_COM_EPON_1
void processEponSetQueueConfig(CmsMsgHeader *msg)
{
   CmsMsgHeader replyMsg = EMPTY_MSG_HEADER;
   XponObject *obj = NULL;
   UpLinkObject *up_obj = NULL;
   DownPortObject *dn_obj = NULL;
   InstanceIdStack xponiidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack, iidStackPrev;
   CmsRet ret;
   int i;
   EponSetQueueConfigMsgBody *queueConfig = (EponSetQueueConfigMsgBody *)(msg + 1);

   replyMsg.type = msg->type;
   replyMsg.src = EID_SSK;
   replyMsg.dst = msg->src;
   replyMsg.flags_request = 0;
   replyMsg.flags_response = 1;
   replyMsg.wordData = CMSRET_INTERNAL_ERROR;
    
   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get lock, ret=%d", ret);
      cmsLck_dumpInfo();
      return;
   }
   
   if ((ret = cmsObj_get(MDMOID_XPON, &xponiidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
   {
       cmsLog_error("get of MDMOID_IGD_DEVICE_INFO object failed, ret=%d", ret);
       goto exit;
   }
   obj->linkNum = queueConfig->bcmQueueCfg.linkNum;
   obj->portNum = queueConfig->bcmQueueCfg.portNum;
   ret = cmsObj_set(obj, &xponiidStack);
   if( ret != CMSRET_SUCCESS)
   {
       cmsLog_error("set cms error (%d)\n", ret);
       goto exit;
   }

   /* loop through the upstream link array to find the instances to delete */
   INIT_INSTANCE_ID_STACK(&iidStack);
   iidStackPrev = iidStack;
   while ((ret = cmsObj_getNext(MDMOID_UP_LINK, &iidStack, (void **)&up_obj)) == CMSRET_SUCCESS)
   {                   
       if ((ret = cmsObj_deleteInstance(MDMOID_UP_LINK, &iidStack)) != CMSRET_SUCCESS)
       {
           cmsLog_error("cmsObj_deleteInstance MDMOID_UP_LINK returns error, ret=%d", ret);
           cmsObj_free((void **)&up_obj);
           goto exit;
       }
        /* since this instance has been deleted, we want to set the iidStack to
              * the previous instance, so that we can continue to do getNext.
              */
       iidStack = iidStackPrev;
       cmsObj_free((void **)&up_obj);
   }

   for (i = 0; i < queueConfig->bcmQueueCfg.linkNum; i++)
   {
       /* add new object instance */
       INIT_INSTANCE_ID_STACK(&iidStack);
       if ((ret = cmsObj_addInstance(MDMOID_UP_LINK, &iidStack)) != CMSRET_SUCCESS)
       {
           cmsLog_error("Failed to add MDMOID_UP_LINK inst %d, ret=%d", i, ret);
           goto exit;
       }
       if ((ret = cmsObj_get(MDMOID_UP_LINK, &iidStack, 0, (void **)&up_obj)) != CMSRET_SUCCESS)
       {
           cmsLog_error("Failed to get MDMOID_UP_LINK inst %d, ret=%d", i, ret);
           goto exit;
       }
       /* update obj value */
       up_obj->queueNum = queueConfig->bcmQueueCfg.linkQ[i].qNum;
       /* set the object */
       ret = cmsObj_set((void *)up_obj, &iidStack);
       if (ret != CMSRET_SUCCESS)
       {
           cmsLog_error("Failed to set MDMOID_UP_LINK inst %d, ret = %d", i, ret);
           /* mdm_setObject will steal the object on success, but on failure,
                     * we need to free it.
                     */
           cmsObj_free((void **) &up_obj);
           goto exit;
       }
   }

   /* loop through the downstream port array to find the instances to delete */
   INIT_INSTANCE_ID_STACK(&iidStack);
   iidStackPrev = iidStack;
   while ((ret = cmsObj_getNext(MDMOID_DOWN_PORT, &iidStack, (void **)&dn_obj)) == CMSRET_SUCCESS)
   {                   
       if ((ret = cmsObj_deleteInstance(MDMOID_DOWN_PORT, &iidStack)) != CMSRET_SUCCESS)
       {
           cmsLog_error("cmsObj_deleteInstance MDMOID_DOWN_PORT returns error, ret=%d", ret);
           cmsObj_free((void **)&dn_obj);
           goto exit;
       }
       /* since this instance has been deleted, we want to set the iidStack to
              * the previous instance, so that we can continue to do getNext.
              */
       iidStack = iidStackPrev;
       cmsObj_free((void **)&dn_obj);
   }

   for (i = 0; i < queueConfig->bcmQueueCfg.portNum; i++)
   {
       /* add new object instance */
       INIT_INSTANCE_ID_STACK(&iidStack);
       if ((ret = cmsObj_addInstance(MDMOID_DOWN_PORT, &iidStack)) != CMSRET_SUCCESS)
       {
           cmsLog_error("Failed to add MDMOID_UP_LINK inst %d, ret=%d", i, ret);
           goto exit;
       }
       /* get the object we just added */
       if ((ret = cmsObj_get(MDMOID_DOWN_PORT, &iidStack, 0, (void **)&dn_obj)) != CMSRET_SUCCESS)
       {
           cmsLog_error("Failed to get MDMOID_UP_LINK inst %d, ret=%d", i, ret);
           goto exit;
       }
       /* update obj value */
       dn_obj->queueNum = queueConfig->bcmQueueCfg.portQ[i].qNum;
       /* set the object */
       ret = cmsObj_set((void *)dn_obj, &iidStack);
       if (ret != CMSRET_SUCCESS)
       {
           cmsLog_error("Failed to set MDMOID_DOWN_PORT inst %d, ret = %d", i, ret);
           /* mdm_setObject will steal the object on success, but on failure,
                     * we need to free it.
                     */
           cmsObj_free((void **) &dn_obj);
           goto exit;
       }
   }

   ret = cmsMgm_saveConfigToFlash();
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not save config to flash, ret=%d\n", ret);
      goto exit;
   }

   replyMsg.wordData = ret;

exit:
   cmsObj_free((void **)&obj);
   cmsLck_releaseLock();
   if ((ret = cmsMsg_send(msgHandle, &replyMsg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("sending msg to %d failed, ret=%d", msg->dst,ret);
   }
   else
   {
      cmsLog_debug("sending msg to %d done, ret=%d", msg->dst,ret);
   }
}

void processEponGetQueueConfig(CmsMsgHeader *msg)
{
   char buf[sizeof(CmsMsgHeader) + 200]={0};
   CmsMsgHeader *replyMsg=(CmsMsgHeader *) buf;
   unsigned char *body = (unsigned char *) (replyMsg+1);
   CmsRet ret;
   XponObject *obj = NULL;
   UpLinkObject *up_obj = NULL;
   DownPortObject *dn_obj = NULL;
   InstanceIdStack xponiidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack;
   UINT32 i;

   replyMsg->type = msg->type;
   replyMsg->src = EID_SSK;
   replyMsg->dst = msg->src;
   replyMsg->flags_request = 0;
   replyMsg->flags_response = 1;
   replyMsg->dataLength = 200;
   replyMsg->wordData = CMSRET_INTERNAL_ERROR;

   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get lock, ret=%d", ret);
      cmsLck_dumpInfo();
      return;
   }

   if ((ret = cmsObj_get(MDMOID_XPON, &xponiidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
   {
       cmsLog_error("get of MDMOID_IGD_DEVICE_INFO object failed, ret=%d", ret);
       goto exit;
   }
   *(body++) = obj->linkNum;
    
   /* loop through the upstream link array to get obj value */
   INIT_INSTANCE_ID_STACK(&iidStack);
   i = 0;
   while ((ret = cmsObj_getNext(MDMOID_UP_LINK, &iidStack, (void **)&up_obj)) == CMSRET_SUCCESS)
   {
       *(body++) = up_obj->queueNum;
       memset(body, 0xFF, up_obj->queueNum);
       body += up_obj->queueNum;
       i++;
       cmsObj_free((void **)&up_obj);
   }
   if (i < obj->linkNum)
   {
       cmsLog_notice("iterate MDMOID_UP_LINK not ok", ret);
       goto exit;
   }

   *(body++) = obj->portNum;
   /* loop through the downstream port array to get obj value */
   INIT_INSTANCE_ID_STACK(&iidStack);
   i = 0;
   while ((ret = cmsObj_getNext(MDMOID_DOWN_PORT, &iidStack, (void **)&dn_obj)) == CMSRET_SUCCESS)
   {
       *(body++) = dn_obj->queueNum;
       memset(body, 0xFF, dn_obj->queueNum);
       body += dn_obj->queueNum;
       i++;
       cmsObj_free((void **)&dn_obj);
   }
   if (i < obj->portNum)
   {
       cmsLog_error("iterate MDMOID_DOWN_PORT not ok", ret);
       goto exit;
   }

   replyMsg->wordData = CMSRET_SUCCESS;

exit:
   cmsObj_free((void **)&obj);
   cmsLck_releaseLock();
   if ((ret = cmsMsg_send(msgHandle, replyMsg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("sending msg to %d failed, ret=%d", msg->dst,ret);
   }
   else
   {
      cmsLog_debug("sending msg to %d done, ret=%d", msg->dst,ret);
   }
}
#endif


/***********************************************************************
 *
 *  Copyright (c) 2010  Broadcom Corporation
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

#include <sys/un.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "cms.h"
#include "cms_log.h"
#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"
#include "cms_msg_modsw.h"
#include "mdm_object.h"
#include "cms_linklist.h"
#include "cms_qdm.h"
#include "osgid.h"
#include "uuid.h"
#include "modsw.h"
#include "cms_params_modsw.h"

/*
 * global variables
 */
int osgid_exit = 0;
void *msgHandle=NULL;
void *tmrHandle = NULL;
int felix_fd = -1;
int felixRetry=0;
UBOOL8 usock_connected = FALSE;
CmsTimestamp Ts;
unsigned char macAddress[6];
/* this is the list of all outstanding request, whether install, uninstall, ...
 * We can only process one request at all time.  So, if we are busy, we just
 * queue the request up in this list for later processing */
LIST_TYPE requestList= {NULL, NULL};
LIST_TYPE processingList= {NULL, NULL};
static UBOOL8 requestListProcessing = FALSE;
static UBOOL8 osgiExecUp = FALSE;
void osgidDump(void);

/*
 * local variables
 */


/*
 * static functions
 */
static CmsRet osgidDoUpdate(const char *url, char *username, char *password, 
                            char *destdir, char *version, void *pDu);
static void osgidReadMessageFromSmd(void);
static int initUnixSocket(void);
static void osgidfelixListener(void);
static int connect_felix_listen_server(char* server);
static int sendto_felix_listen_server(char* buf, int bufLen);
static int recvfrom_felix_listen_server(char* buf, int bufLen);
static void osgidProcessFelixUninstallResponse(char *buf, int len);
static void osgidProcessFelixUpdateResponse(char *buf, int len);
static void osgidProcessFelixStartResponse(char *buf, int len);
static void osgidProcessFelixStopResponse(char *buf, int len);
static void osgidProcessFelixInstallResponse(char *buf, int len);
static CmsRet osgidParseLBResponseFromFelix(char *buf, pLIST_TYPE pBundleList);
static void osgidProcessFelixLBResponse(char *buf, int len);
static CmsRet osgidFreeEntry(pENTRY_TYPE pEntry);

static CmsRet osgidProcessInstallRequest(REQUEST_DATA *reqdata);
static CmsRet osgidProcessUpdateRequest(REQUEST_DATA *reqdata);
static CmsRet osgidProcessUninstallRequest(REQUEST_DATA *reqdata);
static CmsRet osgidProcessRequestBundleList(void);
static CmsRet osgidProcessStartorStopRequest(UBOOL8 startEU, char *euid);


static UBOOL8 osgidIsDuType(char *statusStr);
static CmsRet osgidAddRequestDataToProcessingList(REQUEST_DATA *reqdata);
static void osgidCleanProcessingList(void);
static int osgidProcessProcessingList(const char *operation);
static CmsRet osgidAddLBRequestToList(void);

static void osgidAddEUinstance(pLB_DATA pData, char *euFullPathStr, UINT32 euFllPathStrLen, char* duDescription);
static void osgidDeleteEUinstance(char *euFullPathStr);
static void osgidUpdateExistingEUinstance(pLB_DATA pData, char *euExecutionUnit);
void osgidAddEuStateChangeToList(CmsMsgHeader *msg);
void osgidAddDuStateChangeToList(CmsMsgHeader *msg);
void osgidDeleteAllEuInstances(char *euList);

static void osgidProcessModulesAtBootup(void);

void osgidProcessRequestList(void);
CmsRet osgidAddRequestToList(pREQUEST_DATA pRequestData);


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
   OsgidCfgObject *obj;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   if ((ret = cmsLck_acquireLockWithTimeout(OSGID_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      return;
   }

   if ((ret = cmsObj_get(MDMOID_OSGID_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of OSGID_CFG object failed, ret=%d", ret);
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

static int osgid_system_init()
{
   int rc;
   CmsRet ret;

   rc = mkdir(OSGID_WORK_DIR, 0777);
   if (rc && errno != EEXIST)
   {
      cmsLog_error("failed to make workdir %s, errno=%d", OSGID_WORK_DIR, errno);
      return -1;
   }

   ret = modsw_makeRequiredDirs();
   if (ret != CMSRET_SUCCESS)
   {
      return -1;
   }

   return 0;
}

int main(int argc, char **argv)
{
   SINT32 c, logLevelNum;
   SINT32 shmId=UNINITIALIZED_SHM_ID;
   CmsLogLevel logLevel=DEFAULT_LOG_LEVEL;
   UBOOL8 useConfiguredLogLevel=TRUE;
   int comm_fd, max_fd;
   fd_set rset;
   CmsRet ret;
   int nready;
   struct timeval to;
   int errorRepeated = 0;

   /* init log */
   cmsLog_initWithName(EID_OSGID, "osgid");

   /* parse command line args */
   while ((c = getopt(argc, argv, "v:m:")) != -1)
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

      case 'm':
         shmId = atoi(optarg);
         break;
         
      default:
         usage(argv[0]);
         break;
      }
   }

   if (osgid_system_init())
   {
      exit(-1);
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
   //   signal(SIGPIPE, SIG_IGN); /* Ignore SIGPIPE signals */
   //    signal(SIGTERM, osgid_sigTermHandler);
   //    signal(SIGINT, osgid_sigTermHandler);

   /* msgHandle is used to communicate with SMD */
   cmsLog_notice("calling cmsMsg_init");
   if ((ret = cmsMsg_initWithFlags(EID_OSGID, 0, &msgHandle)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMsg_init failed, ret=%d", ret);
      return 0;
   }

   cmsLog_notice("calling cmsMdm_init with shmId=%d", shmId);
   if ((ret = cmsMdm_initWithAcc(EID_OSGID, 0, msgHandle, &shmId)) != CMSRET_SUCCESS)
   {
      cmsMsg_cleanup(&msgHandle);
      cmsLog_error("cmsMdm_init error ret=%d", ret);
      return 0;
   }
   
   initLoggingFromConfig(useConfiguredLogLevel);

   cmsLog_notice("initializing timers");
   if ((ret = cmsTmr_init(&tmrHandle)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsTmr_init failed, ret=%d", ret);
      return -1;
   }

   /* comm_fd to listen message from SMD */
   cmsMsg_getEventHandle(msgHandle,&comm_fd);
   if (comm_fd == CMS_INVALID_FD)
   {
      cmsLog_error("comm_fd is invalid.");
      return -1;
   }

   /* felix_fd to listen message from Felix listener bundle */
   /* felix_fd = ;*/
   /* whichever fd is bigger, that's the max_fd */
   max_fd = comm_fd;
   
   /* loop forever here */
   for (;;)
   {      
      FD_ZERO(&rset);
      FD_SET(comm_fd,&rset);
   
      if(usock_connected)
      {
         FD_SET(felix_fd,&rset);
         if(felix_fd >= comm_fd)
            max_fd = felix_fd;
      }
   
      to.tv_sec = 1;
      to.tv_usec = 0;
      nready = select(max_fd+1,&rset,NULL,NULL,&to);
      if (nready == -1)
      {
         if (errorRepeated < MAX_ERROR_MSG_PRINTOUT)
         {
            /* errno-base.h, errno.h */
            cmsLog_notice("error on select, errno=%d",errno);
            errorRepeated++;
         }
         usleep(100);
         continue;
      }
      errorRepeated = 0;

      if (FD_ISSET(comm_fd,&rset))
      {
         osgidReadMessageFromSmd();
      }

      if((usock_connected) && (FD_ISSET(felix_fd,&rset)))
      {
         /*this is for listen Hello msg from felix; 
         if got msg is due to sendtoFelixandGetReply timeout,
         we will ingore it now, just print it.*/
         osgidfelixListener();
      }
   
      if (osgid_exit)
      {
         break;
      }
   }/* loop */

   cmsLog_notice("exiting with code, osgid_exit %d",osgid_exit);
   cmsTmr_cleanup(&tmrHandle);
   cmsMdm_cleanup();
   cmsMsg_cleanup(&msgHandle);
   cmsLog_cleanup();

   return 0;
}


void osgidSendReplyToRequestApp(REQUEST_DATA *requestdata)
{
   CmsRet ret = CMSRET_SUCCESS;
   CmsMsgHeader *reqMsg;
   DUresponseStateChangedMsgBody *msgPayload;

   if(strcasecmp(requestdata->operation, SW_MODULES_OPERATION_INSTALL_AT_BOOTUP) == 0)
   {
      /* it's a installation at bootup, we don't need to send repy to any App. Just print error */
      if (requestdata->FaultCode != CMSRET_SUCCESS)
      {
         cmsLog_error("Error on install at bootup, error code : %d", requestdata->FaultCode);
      }
      return; 
   }

   if(requestdata->type != (CmsMsgType) CMS_MSG_REQUEST_DU_STATE_CHANGE)
   {
      /*Now we just send reply to a DU request; display a more generic message for others */
      cmsLog_debug("Unable to fullfill request at this time or error encountered, request type 0x%x error code %d",
              requestdata->type, requestdata->FaultCode);
      return; 
   }
   
   reqMsg = cmsMem_alloc(sizeof(CmsMsgHeader) + sizeof(DUresponseStateChangedMsgBody),
                       ALLOC_ZEROIZE);
   if(reqMsg == NULL)
   {
      cmsLog_error("Alloc msg memory error.");
      return;
   }
   
   /* initialize header fields */
   reqMsg->type = (CmsMsgType) CMS_MSG_RESPONSE_DU_STATE_CHANGE;
   reqMsg->src = EID_OSGID;
   reqMsg->dst = requestdata->src;
   reqMsg->flags_response = 1;
   reqMsg->wordData = 1;
   reqMsg->dataLength = sizeof(DUresponseStateChangedMsgBody);

   /* copy file into the payload and send message */
   msgPayload = (DUresponseStateChangedMsgBody*) (reqMsg + 1);

   strcpy(msgPayload->operation,requestdata->operation);
   strcpy(msgPayload->URL,requestdata->URL);
   strcpy(msgPayload->UUID,requestdata->UUID);
   strcpy(msgPayload->version,requestdata->version);
   msgPayload->reqId = requestdata->reqId;
   msgPayload->faultCode = requestdata->FaultCode;
   if (requestdata->FaultCode != CMSRET_SUCCESS)
   {
      cmsLog_debug("requestdata->FaultCode %d, requestdata->FaultMsg %s\n",
                   requestdata->FaultCode,requestdata->ErrMsg);
   }

   /*
    * According to TR69, A.4.2.3, the EUlist and DUlist must still
    * be filled in for an Uninstall operation.  OSGID must record it
    * and send it back to tr69c, because tr69c will no longer be
    * able to query the MDM to get it since these objects are already
    * deleted.
    */
   strcpy(msgPayload->EUlist,requestdata->EUlist);
   strcpy(msgPayload->DUlist,requestdata->DUlist);
   cmsLog_debug("responseMsg EUlist=%s DUlist=%s", msgPayload->EUlist, msgPayload->DUlist);

   if((ret = cmsMsg_send(msgHandle, reqMsg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to send message (ret=%d)", ret);      
   }
   else
   {
      cmsLog_debug("send reply message to %d (0x%08X), size %d.\n", 
                   (unsigned int)(reqMsg->dst), (unsigned int)(reqMsg), reqMsg->dataLength);
   }
   CMSMEM_FREE_BUF_AND_NULL_PTR(reqMsg);

   return;
}

void osgidSendReplyListToRequestApp(pLIST_TYPE pList)
{
   CmsRet ret = CMSRET_SUCCESS;
   CmsMsgHeader *reqMsg;
   DUresponseStateChangedMsgBody *msgPayload;
   pREQUEST_DATA pRequestData;
   pENTRY_TYPE pEntry, ptr;
   int num_req = 0, body_size = 0;

   cmsLog_debug("Entered");

   /* need to remove one request from list to find out the operation type */
   pEntry = (pENTRY_TYPE)(removeFront(pList));
   if (pEntry == NULL)
   {
      return;
   }

   pRequestData = (pREQUEST_DATA)pEntry->data;
   
   cmsLog_debug("operation =%s", pRequestData->operation);

   if(strcasecmp(pRequestData->operation, SW_MODULES_OPERATION_INSTALL_AT_BOOTUP) == 0)
   {
      /*it's a installation at bootup, we don't need to send repy to any App.*/
      cmsLog_debug("It's an installation at bootup.");
      osgidFreeEntry(pEntry);
      return; 
   }

   if(pRequestData->type != (CmsMsgType) CMS_MSG_REQUEST_DU_STATE_CHANGE)
   {
      /*Now we just send reply only if the request is DU change. */
      cmsLog_debug("It's not a DU state change request.");
      osgidFreeEntry(pEntry);
      return; 
   }

   ptr = pList->head;
   while(ptr != NULL)
   {
      num_req++;
      ptr = ptr->next;      
   }

   /* num_req + 1 because one was previously removed to examine the operation type */
   body_size = sizeof(DUresponseStateChangedMsgBody)*(num_req+1);
   reqMsg = cmsMem_alloc(sizeof(CmsMsgHeader) + body_size, ALLOC_ZEROIZE);
   if(reqMsg == NULL)
   {
      cmsLog_error("Alloc msg memory error.");
      osgidFreeEntry(pEntry);
      return;
   }  
   
   /* initialize header fields */
   reqMsg->type = (CmsMsgType) CMS_MSG_RESPONSE_DU_STATE_CHANGE;
   reqMsg->src = EID_OSGID;
   reqMsg->dst = pRequestData->src;
   reqMsg->flags_response = 1;
   reqMsg->wordData = num_req+1;
   reqMsg->dataLength = body_size;

   /* copy file into the payload and send message */
   msgPayload = (DUresponseStateChangedMsgBody *)(reqMsg + 1);
   while(pEntry != NULL)
   {
      pRequestData = (pREQUEST_DATA)pEntry->data;

      strcpy(msgPayload->operation,pRequestData->operation);
      strcpy(msgPayload->URL,pRequestData->URL);
      strcpy(msgPayload->UUID,pRequestData->UUID);
      strcpy(msgPayload->version,pRequestData->version);
      msgPayload->reqId = pRequestData->reqId;
      msgPayload->faultCode = pRequestData->FaultCode;

      /*
       * According to TR69, A.4.2.3, the EUlist and DUlist must still
       * be filled in for an Uninstall operation.  OSGID must record it
       * and send it back to tr69c, because tr69c will no longer be
       * able to query the MDM to get it since these objects are already
       * deleted.
       */
      strcpy(msgPayload->EUlist,pRequestData->EUlist);
      strcpy(msgPayload->DUlist,pRequestData->DUlist);
      cmsLog_debug("responseMsg (multi) EUlist=%s DUlist=%s", msgPayload->EUlist, msgPayload->DUlist);

      osgidFreeEntry(pEntry);
      pEntry = (pENTRY_TYPE)(removeFront(pList));

      msgPayload++;
   }
   
      
   if((ret = cmsMsg_send(msgHandle, reqMsg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to send message (ret=%d)", ret);      
   }
   else
   {
      cmsLog_debug("send reply message to %d (0x%08X), size %d\n", (unsigned int)(reqMsg->dst), (unsigned int)(reqMsg), reqMsg->dataLength);
   }
   CMSMEM_FREE_BUF_AND_NULL_PTR(reqMsg);

   return;
}


static CmsRet startOsgiFramework(void)
{
   char cmd[256];
   cmd[0] = '\0';
   int initialHeap, maxHeap, stackSize;
   char initialStr[8];
   char maxStr[8];
   char stackStr[8];
   int rc;

   cmsLog_debug("XXX === making osgi tmp dir (%s) ===", OSGID_WORK_DIR);
   rc = mkdir(OSGID_WORK_DIR, 0777);
   if (rc && errno != EEXIST)
   {
      cmsLog_error("failed to make workdir %s, errno=%d", OSGID_WORK_DIR, errno);
      return -1;
   }

   /* Start OSGI framework */
   cmsLog_debug("XXX === cp -rf %s/* -> %s ===", OSGID_FELIX_DIR, OSGID_WORK_DIR);
   sprintf(cmd,"cp -rf %s/* %s",OSGID_FELIX_DIR,OSGID_WORK_DIR);
   system(cmd);
   cmd[0] = '\0';

#if defined(JAMVM_MIN_HEAP) && defined(JAMVM_MAX_HEAP) && defined(JAMVM_STACK)
   initialHeap = JAMVM_MIN_HEAP;
   maxHeap = JAMVM_MAX_HEAP;
   stackSize = JAMVM_STACK;
#else
   initialHeap = JVM_DEFAULT_MIN_HEAP;
   maxHeap = JVM_DEFAULT_MAX_HEAP;
   stackSize = JVM_DEFAULT_STACK_SIZE;
#endif

   if (initialHeap < 1000)
   {
      sprintf(initialStr,"%dK",initialHeap);
   }
   else
   {
      sprintf(initialStr,"%dM",initialHeap/1000);
   }

   if (maxHeap < 1000)
   {
      sprintf(maxStr,"%dK",maxHeap);
   }
   else
   {
      sprintf(maxStr,"%dM",maxHeap/1000);
   }

   if (stackSize < 1000)
   {
      sprintf(stackStr,"%dK",stackSize);
   }
   else
   {
      sprintf(stackStr,"%dM",stackSize/1000);
   }

   sprintf(cmd,"cd %s;jamvm -cp %s:%s -Xms%s -Xmx%s -Xss%s %s &",OSGID_WORK_DIR,OSGID_FELIX_EXEC,OSGID_FELIX_CP,initialStr,maxStr,stackStr, OSGID_FELIX_ACTIVATOR);

   cmsLog_debug("XXX starting jamvm+felix-osgi with cmd=%s", cmd);
   system(cmd);

   return (CMSRET_SUCCESS);
}


static void osgidWaitForFelixToComeUp(void)
{
   signal(SIGALRM,(__sighandler_t)initUnixSocket);
   alarm(TIME_WAIT_FOR_FELIX_UP);
}

void osgidProcessStartEE(void)
{

   modsw_setExecEnvStatus(OSGI_NAME, MDMVS_X_BROADCOM_COM_STARTING);

   if (startOsgiFramework() == CMSRET_SUCCESS)
   {
      osgidWaitForFelixToComeUp();
   }
   else
   {
      modsw_setExecEnvStatus(OSGI_NAME, MDMVS_ERROR);
   }

   return;
}


static void osgidReadMessageFromSmd(void)
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
         CmsModSwMsgType modswMsg = (CmsModSwMsgType) msg->type;
         switch(modswMsg)
         {
         case CMS_MSG_REQUEST_DU_STATE_CHANGE:
            cmsLog_debug("got CMS_MSG_REQUEST_DU_STATE_CHANGE");
            osgidAddDuStateChangeToList(msg);
            osgidProcessRequestList();
            isGenericCmsMsg=FALSE;
            break;

         case CMS_MSG_REQUEST_EU_STATE_CHANGE:
            cmsLog_debug("got CMS_MSG_REQUEST_EU_STATE_CHANGE");
            osgidAddEuStateChangeToList(msg);
            osgidProcessRequestList();
            isGenericCmsMsg=FALSE;
            break;

         case CMS_MSG_REQUEST_BUNDLE_LIST:
            cmsLog_debug("got CMS_MSG_REQUEST_BUNDLE_LIST");
            osgidAddLBRequestToList();
            osgidProcessRequestList();
            isGenericCmsMsg=FALSE;
            break;

         case CMS_MSG_OSGID_PRINT:
            cmsLog_debug("got CMS_MSG_OSGID_PRINT");
            osgidDump();
            isGenericCmsMsg=FALSE;
            break;

         case CMS_MSG_START_EE:
            cmsLog_debug("got CMS_MSG_START_EE");
            osgidProcessStartEE();
            isGenericCmsMsg=FALSE;
            break;

         case CMS_MSG_STOP_EE:
            cmsLog_error("osgid does not support STOP_EE yet");
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
} /* osgidReadMessageFromSmd */

CmsRet osgidGetMacAddressOrRandomBytes(void)
{
   int error = 0;
   CmsRet ret;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanEthIntfObject *ethObj=NULL;
   unsigned char *cp;
   int i;

   /* UUID generation routine tries to use MAC address as part of the UUID. */
   if ((ret = cmsLck_acquireLockWithTimeout(OSGID_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      error = 1;
   }
   else {
      if ((ret = cmsObj_getNext(MDMOID_LAN_ETH_INTF, &iidStack, (void **) &ethObj)) == CMSRET_SUCCESS)
      {
         ret = cmsUtl_macStrToNum(ethObj->MACAddress, macAddress);
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("Unable to convert macAddrString=%s, unable to release mac addr", ethObj->MACAddress);
         }
         cmsObj_free((void **) &ethObj);
      }
      else 
      {
         error = 1;
      }
      cmsLck_releaseLock();
   }
   if (error)
   {
      /* get random bytes */
      for (cp =&macAddress[0], i = 0; i < 6; i++)
      {
         *cp++ ^= (rand() >> 7) & 0xFF;
      }
   }
   return (CMSRET_SUCCESS);
}

void osgidRetrieveDUorEUState(char *operation)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   DUObject *duObject;
   CmsRet ret = CMSRET_SUCCESS;
   UBOOL8 found = FALSE, UpdateCase = FALSE, UninstallCase = FALSE;

   if(cmsUtl_strcmp(operation,SW_MODULES_OPERATION_UPDATE) == 0)
      UpdateCase = TRUE;
   if(cmsUtl_strcmp(operation,SW_MODULES_OPERATION_UNINSTALL) == 0)
      UninstallCase = TRUE;
   
   if ((ret = cmsLck_acquireLockWithTimeout(OSGID_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      return;
   }

   if (cmsUtl_strcmp(operation,SW_MODULES_OPERATION_INSTALL_AT_BOOTUP) != 0)
   {
      /* delete the DU entry  */
      cmsLog_notice("new install case, delete the new added DU.");
      
      while( !found && (cmsObj_get(MDMOID_DU, &iidStack, 0, (void **) &duObject) ==
            CMSRET_SUCCESS))
      {
         if(cmsUtl_strcmp(duObject->status, MDMVS_INSTALLING) == 0)
            found = TRUE;
         
         cmsObj_free((void **) &duObject);
      }

      /* both edit and add new will need to do a set and free */
      if(found)
      {
         if ((ret = cmsObj_deleteInstance(MDMOID_DU, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("delete DU entry failed, ret=%d", ret);
         }
         else
         {
            cmsLog_debug("DU entry deleted");
            cmsMgm_saveConfigToFlash();
         }
      }
   } /* newly added via management application */
   else if(cmsUtl_strcmp(operation,SW_MODULES_OPERATION_INSTALL_AT_BOOTUP) == 0)
   {
      cmsLog_notice("Bootup case, retrieving status of .SoftwareModules.DU.{i}. instance to Uninstalled");

      /* bootup case: with bootup, we do not need to delete the DU, we just retrieve status to UNINSTALLED */
      INIT_INSTANCE_ID_STACK(&iidStack);
      while (!found &&
             (ret = cmsObj_getNext(MDMOID_DU, &iidStack, (void **)&duObject)) == CMSRET_SUCCESS)
      {
         /* next testing UUID */
         if (cmsUtl_strcmp(duObject->status, MDMVS_INSTALLING) == 0)
         {
            found = TRUE;
            CMSMEM_REPLACE_STRING(duObject->status,MDMVS_UNINSTALLED);
            
            /* both edit and add new will need to do a set and free */
            if ((ret = cmsObj_set(duObject, &iidStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("set of DU entry (bootup)failed, ret=%d, du->uuid %s", ret, duObject->UUID);
            }
            else
            {
               cmsLog_debug("DU status retrieved successfully.");
               cmsMgm_saveConfigToFlash();
            }
            cmsObj_free((void **) &duObject);
         }
         else 
         {
            cmsObj_free((void **) &duObject);
            continue;
         }
      } /* while loop of all DU */
   } /* bootup install case */
   else if(UpdateCase || UninstallCase)
   {
      cmsLog_notice("Update/Uninstall case, retrieving status of .SoftwareModules.DU.{i}. instance to Installed");

      INIT_INSTANCE_ID_STACK(&iidStack);
      while (!found &&
             (ret = cmsObj_getNext(MDMOID_DU, &iidStack, (void **)&duObject)) == CMSRET_SUCCESS)
      {
         /* next testing UUID */
         if ((UpdateCase &&(cmsUtl_strcmp(duObject->status, MDMVS_UPDATING) == 0))
            || (UninstallCase &&(cmsUtl_strcmp(duObject->status, MDMVS_UNINSTALLING) == 0)))
         {
            found = TRUE;
            CMSMEM_REPLACE_STRING(duObject->status,MDMVS_INSTALLED);
            
            /* both edit and add new will need to do a set and free */
            if ((ret = cmsObj_set(duObject, &iidStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("set of DU entry failed, ret=%d, du->uuid %s", ret, duObject->UUID);
            }
            else
            {
               cmsLog_debug("DU status retrieved successfully.");
               cmsMgm_saveConfigToFlash();
            }
            cmsObj_free((void **) &duObject);
         }
         else 
         {
            cmsObj_free((void **) &duObject);
            continue;
         }
      } /* while loop of all DU */
   }/* update/uninstall case */
   else
   {
      /*Skip: we shouldn't go here*/
   }

   cmsLck_releaseLock();   

   return;
}


/*
Function: osgidGetNumOfDU
1. UUID != empty, URL == empty: get num of DUs with same UUID and status
2. UUID == empy, URL != empty: get num of DUs with same URL and status
3. UUID == empy, URL == empty: get num of all DU with status
4. UUID != empty, URL != empty: return 0
NOTICE: before call this function, MUST get the lock.
*/
int osgidGetNumOfDU(char *uuid, char *url, const char *status)
{
   InstanceIdStack idStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 matchUUID = FALSE;
   UBOOL8 matchURL = FALSE;
   DUObject *duObject;
   CmsRet ret;
   int numDU = 0;
   
   cmsLog_debug("UUID=%s, url=%s, status=%s", uuid, url, status);

   if(((uuid != NULL) && (uuid[0] == '\0'))
      && ((url != NULL) && (url[0] != '\0')))
   {
      return numDU;
   }

   if((uuid != NULL) && (uuid[0] == '\0'))
      matchUUID = TRUE;
   else if((url != NULL) && (url[0] != '\0'))
      matchURL = TRUE;
   
   INIT_INSTANCE_ID_STACK(&idStack);
   while ((ret = cmsObj_getNext(MDMOID_DU, &idStack, (void **)&duObject)) == CMSRET_SUCCESS)
   {
      cmsLog_debug("du->uuid %s, du->url %s\n", duObject->UUID, duObject->URL);

      if(matchUUID)
      {
         if (cmsUtl_strcmp(duObject->UUID, uuid) == 0)
         {
            if(status != NULL)
            {
               if (cmsUtl_strcmp(duObject->status, status) == 0)
                  numDU++;
            }
            else
               numDU++;
         }
      }
      else if(matchURL)
      {
         if (cmsUtl_strcmp(duObject->URL, url) == 0)
         {
            if(status != NULL)
            {
               if (cmsUtl_strcmp(duObject->status, status) == 0)
                  numDU++;
            }
            else
               numDU++;
         }
      }
      else
      {
         if(status != NULL)
         {
            if (cmsUtl_strcmp(duObject->status, status) == 0)
               numDU++;
         }
         else
            numDU++;
      }
      cmsObj_free((void **) &duObject);
   } /* while */

   return numDU;
}

CmsRet osgidProcessInstallRequest(REQUEST_DATA *reqdata)
{
   char bundleDir[CMS_MAX_FULLPATH_LENGTH]={0};
   char cmdbuf[CMS_MAX_FULLPATH_LENGTH]={0};
   char duid[BUFLEN_64]={0};
   char *filename = NULL;
   CmsRet ret;
   int rc;
   char newDuid[BUFLEN_64]={0};
   
   ret = cmsUtl_getRunTimePath(CMS_MODSW_OSGIEE_DU_DIR, bundleDir, sizeof(bundleDir));
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("path is too long for bundleDir (len=%d)", sizeof(bundleDir));
      return ret;
   }

   filename = strrchr(reqdata->URL, '/');
   if(filename == NULL)
   {
      cmsLog_error("Could not find filename in URL %s", reqdata->URL);
      return CMSRET_INTERNAL_ERROR;
   }
   filename = filename + 1;  /* move past the / */


   if (!cmsUtl_strcmp(reqdata->operation,SW_MODULES_OPERATION_INSTALL))
   {
      /* newly added via management application (not bootup).
       * Add new DU object in MDM */
      sprintf(duid, "%s::{i}", OSGI_NAME);
      ret = modsw_addDuEntry(reqdata->UUID, reqdata->version, duid, reqdata->URL, 
                             reqdata->executionEnv, newDuid, sizeof(newDuid)-1);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not add new DU entry, ret=%d", ret);
         return ret;
      }

      modsw_setDuStatus(reqdata->UUID, reqdata->version, newDuid, MDMVS_X_BROADCOM_COM_DOWNLOADING);

      ret = modswDld_startDownload(reqdata->URL,reqdata->username,reqdata->password,bundleDir);
      if (ret != CMSRET_SUCCESS)
      {
         /* set DU status to download failed, let ACS delete the entry */
         modsw_setDuStatus(reqdata->UUID, reqdata->version, newDuid, MDMVS_X_BROADCOM_COM_DNLDFAILED);
         return ret;
      }
   }

   /*
    * We have either successfully downloaded a new bundle, or this is
    * a INSTALL_AT_BOOTUP, which means the bundle is already on the
    * filesystem.  Now now move the state to "installing" and tell Felix
    * to install it.
    */
   modsw_setDuStatus(reqdata->UUID, reqdata->version, newDuid, MDMVS_INSTALLING);

   rc = snprintf(cmdbuf, sizeof(cmdbuf), "install file:%s/%s", bundleDir, filename);
   if (rc >= (int) sizeof(cmdbuf))
   {
      cmsLog_error("cmdbuf too small to hold felix command (max=%d)", sizeof(cmdbuf));
      ret = CMSRET_RESOURCE_EXCEEDED;
   }
   else
   {
      if (sendto_felix_listen_server(cmdbuf,strlen(cmdbuf)))
      {
         cmsLog_debug("cmdbuf %s sent to felix", cmdbuf);
         reqdata->requestStatus = OSGID_REQUEST_PROCESSING;

         /* add request data back? to list so we can expect an answer ?*/
         ret=osgidAddRequestDataToProcessingList(reqdata);
      }
      else
      {
         cmsLog_debug("error sending install filename %s message sent to felix", filename);
         ret = CMSRET_INTERNAL_ERROR;

         /*fail to send request to felix, we have to retrieve DU state and clean processingList*/
         osgidRetrieveDUorEUState(reqdata->operation);
      }
   }

   return (ret);
}


CmsRet osgidProcessUpdateRequest(REQUEST_DATA *reqdata)
{
   InstanceIdStack idStack = EMPTY_INSTANCE_ID_STACK;
   DUObject *duObject;
   OSGI_UPDATE_ACTION action;
   CmsRet ret;
   int du_num = 0, retval = 0;

   /* 1. UUID populated, URL empty: update base matching UUID and use internal URL
    * 2. UUID not populated, URL populated: find matching URL, update duObject->UUID
    * 3. UUID and URL both are not populated, go through all DUs, and update all based on each du->url 
    * 4. both UUID and URL populate: update DU with matching UUID, and update internal URL
    */   
   if (((reqdata->UUID[0] != '\0') && (reqdata->URL[0] == '\0')) ||
       ((reqdata->UUID[0] != '\0') && (reqdata->URL[0] != '\0')))
   {
      action = OSGID_UPDATE_ONLY_UUID;
   }
   else if ((reqdata->UUID[0] == '\0') && (reqdata->URL[0] == '\0'))
   {
      /*Note that because this option [UUID empty, URL empty] is intended to update 
      all DUs, the Version MUST NOT be specified.  If the Version is specified, the CPE 
      SHOULD consider this operation in fault using 9003 as the fault code.*/
      if(reqdata->version[0] == '\0')
      {
         action = OSGID_UPDATE_ALL_DU;
      }
      else
      {
         return CMSRET_INVALID_ARGUMENTS;
      }
   }   
   else if ((reqdata->UUID[0] == '\0') && (reqdata->URL[0] != '\0'))
   {
      action = OSGID_UPDATE_ALL_MATCH_URL;
   }
   else
   {
      return CMSRET_DU_STATE_INVALID;
   }
 
   cmsLog_debug("action=%d", (int)action);

   if ((ret = cmsLck_acquireLockWithTimeout(OSGID_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d, drop message", ret);
      return (ret);
   }

   /* first check if we are in the valid state to do this operation */
   INIT_INSTANCE_ID_STACK(&idStack);
   while ((ret = cmsObj_getNext(MDMOID_DU, &idStack, (void **)&duObject)) == CMSRET_SUCCESS)
   {
      if (action == OSGID_UPDATE_ONLY_UUID)
      {
         cmsLog_debug("update_only_uuid: du->uuid %s, uuid %s\n",
                      duObject->UUID, reqdata->UUID);

         /* next testing UUID */
         if (cmsUtl_strcmp(duObject->UUID, reqdata->UUID) != 0)
         {
            cmsObj_free((void **) &duObject);
            continue;
         }         
         else if(reqdata->version[0] != '\0') 
         {
            cmsLog_debug("update_only_uuid: du->version %s, req->version %s, du->status %s\n",
                         duObject->version, reqdata->version,duObject->status);

            /*Should ONLY have one DU matched with the UUID and Version*/
            if (cmsUtl_strcmp(duObject->version, reqdata->version) == 0)               
            {
               if(cmsUtl_strcmp(duObject->status, MDMVS_INSTALLED) == 0)
               {
                  /*queue the req data into processingList*/
                  ret = osgidAddRequestDataToProcessingList(reqdata);
               }
               else
               {
                  ret = CMSRET_DU_STATE_INVALID;
               }
               if(ret != CMSRET_SUCCESS)
               {
                  goto updateError;
               }
               du_num++;
               cmsObj_free((void **) &duObject);
               break;
            }
            cmsObj_free((void **) &duObject);
            continue;
         }         
         else 
         {
            cmsLog_debug("update_only_uuid: req->version empty, du->status %s, du_num %d\n",
                         duObject->status,du_num);

            /*version is empty, so should ONLY have one DU matched with the UUID. 
            If have more than one, we should return error: CMSRET_DU_UPDATE_VERSION_NOT_SPECIFIED.*/
            if((du_num+1) > 1)
            {
               osgidCleanProcessingList();
               ret = CMSRET_DU_UPDATE_VERSION_NOT_SPECIFIED;
               goto updateError;
            }
            else
            {

               cmsLog_debug("update_only_uuid: req->version empty, du->status %s, du->version %s\n",
                            duObject->status,duObject->version);

               if(cmsUtl_strcmp(duObject->status, MDMVS_INSTALLED) == 0)
               {
                  /*queue the req data into processingList*/
                  strcpy(reqdata->version,duObject->version);
                  ret = osgidAddRequestDataToProcessingList(reqdata);

                  if(ret != CMSRET_SUCCESS)
                  {
                     osgidCleanProcessingList();
                     goto updateError;
                  }
                  du_num++;
               }
               cmsObj_free((void **) &duObject);
               continue;
            }
         }
      } /* OSGID_UPDATE_ONLY_UUID */
      else if (action == OSGID_UPDATE_ALL_MATCH_URL)
      {
         cmsLog_debug("update_all_match_url: du->url %s, req->url %s, du->status %s",
                      duObject->URL, reqdata->URL,duObject->status);

         if ((cmsUtl_strcmp(duObject->URL, reqdata->URL) == 0)
            && (cmsUtl_strcmp(duObject->status, MDMVS_INSTALLED) == 0))
         {
            /*queue the req data into processingList*/
            strcpy(reqdata->UUID,duObject->UUID);
            strcpy(reqdata->version,duObject->version);
            ret = osgidAddRequestDataToProcessingList(reqdata);

            if(ret != CMSRET_SUCCESS)
            {
               osgidCleanProcessingList();
               goto updateError;
            }
               
            du_num++;
         }
         cmsObj_free((void **) &duObject);
         continue;
      } /* OSGID_UPDATE_ALL_MATCH_URL */
      else
      {
         cmsLog_debug("update_ALL: du->status %s, du->uuid %s, du->version %s",
                      duObject->status,duObject->UUID,duObject->version);

         if(cmsUtl_strcmp(duObject->status, MDMVS_INSTALLED) == 0)
         {
            /*queue the req data into processingList*/
            strcpy(reqdata->UUID,duObject->UUID);
            strcpy(reqdata->version,duObject->version);
            ret = osgidAddRequestDataToProcessingList(reqdata);

            if(ret != CMSRET_SUCCESS)
            {
               osgidCleanProcessingList();
               goto updateError;
            }

            du_num++;
         }
         cmsObj_free((void **) &duObject);
         continue;
      } /* OSGID_UPDATE_ALL */
   } /* while */
   cmsLck_releaseLock();
   
   if (du_num == 0)
   {      
      cmsLog_debug("update: du_num == 0, return DU_UNKNOWN error");

      ret = CMSRET_DU_UNKNOWN;
      goto updateError;
   }
   
   /*Now we will get the request from processingList to proceed.*/
   cmsLog_debug("update: osgidProcessprocessingList begins, du_num %d",du_num);
   retval = osgidProcessProcessingList(SW_MODULES_OPERATION_UPDATE);

   if (retval == 0)
   {
      ret = CMSRET_INTERNAL_ERROR;
      osgidCleanProcessingList();
   }
   else
   {
      ret = CMSRET_SUCCESS;
   }

   return ret;
   
 updateError:
   cmsObj_free((void **) &duObject);
   cmsLck_releaseLock();
   return (ret);
}

CmsRet osgidProcessUninstallRequest(REQUEST_DATA *reqdata)
{
   InstanceIdStack idStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found = FALSE;
   DUObject *duObject=NULL;
   CmsRet ret;
   char buf[BUFLEN_256];
   int retval = 0;
   int bundleId=0;
   UBOOL8 UninstallAllDU = FALSE;

   cmsLog_debug("reqdata->UUID=%s reqdata->version=%s",
                 reqdata->UUID, reqdata->version);

   if(reqdata->UUID[0] == '\0')
      return CMSRET_DU_UNKNOWN;

   if(reqdata->version[0] == '\0')
      UninstallAllDU = TRUE;
      
   if ((ret = cmsLck_acquireLockWithTimeout(OSGID_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d, drop message", ret);
      return ret;
   }

   if(!UninstallAllDU){
      /* first check if we are in the valid state to do this operation */
      INIT_INSTANCE_ID_STACK(&idStack);
      while (!found &&
             (ret = cmsObj_getNext(MDMOID_DU, &idStack, (void **)&duObject)) == CMSRET_SUCCESS)
      {
         if ((cmsUtl_strcmp(duObject->UUID, reqdata->UUID) == 0)  &&
             (cmsUtl_strcmp(duObject->version, reqdata->version) == 0) )
         {
            found = TRUE;         
         }
         else
         {
            cmsObj_free((void **) &duObject);
         }
      }
      
      if (!found)
      {
         ret = CMSRET_DU_UNKNOWN;
      }
      else if ((cmsUtl_strcmp(duObject->status, MDMVS_INSTALLED) != 0))
      {
         cmsObj_free((void **) &duObject);
         ret =  CMSRET_DU_STATE_INVALID;
      }

      /*Now we queue the request into processingList*/
      if(ret == CMSRET_SUCCESS)
      {
         bundleId = duObject->X_BROADCOM_COM_bundleId;
         reqdata->requestStatus = OSGID_REQUEST_PROCESSING;
         
         ret=osgidAddRequestDataToProcessingList(reqdata);
      }
      
      if (ret == CMSRET_SUCCESS)
      {
         /* update the status */
         CMSMEM_REPLACE_STRING(duObject->status, MDMVS_UNINSTALLING);
         if ((ret = cmsObj_set(duObject, &idStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("cannot change state to uninstalling, ret=%d", ret);
            /*we have to clean processingList*/
            osgidCleanProcessingList();
         }
         else
         {
            cmsMgm_saveConfigToFlash();
         }
         cmsObj_free((void **) &duObject);
      }
      cmsLck_releaseLock();

      /* continue with felix bundle listener */
      if (ret == CMSRET_SUCCESS)
      {           
         sprintf(buf,"%s %d","uninstall", bundleId);
         if (sendto_felix_listen_server(buf,strlen(buf)))
         {
            cmsLog_debug("%s message sent to felix", buf);
            ret = CMSRET_SUCCESS;
         }
         else
         {
            cmsLog_debug("error sending %d message sent to felix", buf);
            ret = CMSRET_INTERNAL_ERROR;

            /*fail to send request to felix, we have to retrieve DU state and clean processingList*/
            osgidRetrieveDUorEUState(reqdata->operation);
            osgidCleanProcessingList();
         }
      }
   }
   else /*if(!UninstallAllDU), we need to uninstall all DU with the same UUID*/
   {
      UINT16 num_du;
      UINT16 du_index = 0;

      num_du = osgidGetNumOfDU(reqdata->UUID, NULL, MDMVS_INSTALLED);

      if(num_du == 0)
      {
         cmsLck_releaseLock();
         ret = CMSRET_DU_UNKNOWN;
         return ret;
      }
      
      /*first, get all DU with the same UUID, and queue them into processlingList*/
      INIT_INSTANCE_ID_STACK(&idStack);
      while (((ret = cmsObj_getNext(MDMOID_DU, &idStack, (void **)&duObject)) == CMSRET_SUCCESS)
         && (du_index < num_du))
      {
         if ((cmsUtl_strcmp(duObject->UUID, reqdata->UUID) == 0)  &&
             (cmsUtl_strcmp(duObject->status, MDMVS_INSTALLED) != 0) )
         {
            /*quequ this DU into processingList*/ 

            du_index++;
            strcpy(reqdata->URL,duObject->URL);
            strcpy(reqdata->version,duObject->version);

            ret = osgidAddRequestDataToProcessingList(reqdata);
         }
         
         cmsObj_free((void **) &duObject);         
      }
      cmsLck_releaseLock();
      
      if(ret != CMSRET_SUCCESS)
      {
         osgidCleanProcessingList();
         if(ret != CMSRET_RESOURCE_EXCEEDED)
            ret = CMSRET_DU_UNKNOWN;
      }

      /*Now we will get the request from processingList to proceed.*/
      if(ret == CMSRET_SUCCESS)
      {
         retval = osgidProcessProcessingList(SW_MODULES_OPERATION_UNINSTALL);

         if(retval == 0){
            ret = CMSRET_INTERNAL_ERROR;
            osgidCleanProcessingList();
         }
      }      
   }
   return (ret);
}

CmsRet osgidProcessStartorStopRequest(UBOOL8 startEU, char *euid)
{
   InstanceIdStack idStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found = FALSE;
   EUObject *euObject=NULL;
   CmsRet ret;

   if(euid == NULL || euid[0] == '\0')
   {
      cmsLog_error("euid is null or empty string");
      return CMSRET_INTERNAL_ERROR;
   }
 
   cmsLog_debug("startEU=%d, euid=%s", startEU, euid);

   if ((ret = cmsLck_acquireLockWithTimeout(OSGID_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d, drop message", ret);
      cmsLck_dumpInfo();
      return (ret);
   }

   /* first check if we are in the valid state to do this operation */
   while (!found &&
          (ret = cmsObj_getNext(MDMOID_EU, &idStack, (void **)&euObject)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(euObject->EUID, euid) != 0)
      {
         char buf[BUFLEN_256]={0};
         int bundleId = 0;

         /* EU status versus requested operation checking was already done
          * in rcl_euObject.  So just do the requested op now.
          */
         found = TRUE;

         if(startEU)
         {
            bundleId = euObject->X_BROADCOM_COM_bundleId;
            sprintf(buf,"%s %d","start", bundleId);
            CMSMEM_REPLACE_STRING(euObject->status, MDMVS_STARTING);
         }
         else
         {
            bundleId = euObject->X_BROADCOM_COM_bundleId;
            sprintf(buf,"%s %d","stop", bundleId);
            CMSMEM_REPLACE_STRING(euObject->status, MDMVS_STOPPING);
         }

         if (sendto_felix_listen_server(buf,strlen(buf)))
         {
            cmsLog_debug("%s message sent to felix", buf);
            cmsLog_debug("now set EU status to %s", euObject->status);
            if ((ret = cmsObj_set(euObject, &idStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("cannot change EU status to to %s ret=%d",
                            euObject->status, ret);
            }
         }
         else
         {
            cmsLog_debug("error sending %s message sent to felix", buf);
            ret = CMSRET_INTERNAL_ERROR;
         }
      }

      cmsObj_free((void **) &euObject);
   } /* while */

   cmsLck_releaseLock();
   return (ret);
}

static CmsRet osgidDoUpdate(const char *url, char *username, char *password, 
                            char *destdir, char *version, void *pDu)
{
   DUObject *pDuObject = (DUObject*)pDu;

   if (pDuObject == NULL)
   {
      cmsLog_debug("pDuObject is NULL!");
      return CMSRET_INTERNAL_ERROR;
   }

   cmsLog_debug("osgidDoUpdate(): duObject->version/version=%s/%s", pDuObject->version, version);

   /* before doing update, do some more checks */
   if ((cmsUtl_strcmp(pDuObject->status, MDMVS_INSTALLED) != 0))
   {
      return(CMSRET_DU_STATE_INVALID);
   }   

   return (modswDld_startDownload(url,username,password,destdir));
}

int connect_felix_listen_server(char* server)
{
   int ret = 0;
   struct sockaddr_un felix_svr;

   felix_svr.sun_family = AF_UNIX;
   strcpy(felix_svr.sun_path, server);

   if ((ret = connect(felix_fd, (struct sockaddr *)&felix_svr, 
               sizeof(felix_svr.sun_family) + strlen(felix_svr.sun_path))) < 0) {
      cmsLog_error("failed (errno=%d) will retry... (retry count=%d)",
                   errno, felixRetry);
   }
   else
   {
      printf("*** osgid connected to felix! \n");
   }

   return ret;
}

int sendto_felix_listen_server(char* buf, int bufLen)
{
   int cnt;
   cnt = send(felix_fd, buf, bufLen,0);
   cmsLog_debug("sendto_felix_listen_server: send...%s, cnt == %d\n", buf, cnt);
   return cnt;
}

int recvfrom_felix_listen_server(char* buf, int bufLen)
{
   int cnt;

   cnt = recv(felix_fd, buf, bufLen, 0);

   if (cnt <=0)
   {
      cmsLog_debug("Error receiving message from felixListener, rx cnt %d, bufLen %d\n", cnt,bufLen);
      return cnt;
   }

   buf[cnt] = '\0';


   cmsLog_debug("rx %d bytes=>%s<=", cnt, buf);


   return cnt;
}

static int initUnixSocket(void)
{
   int cnt=0;
   char buf[BUFLEN_256];

   if(felix_fd == -1)
      felix_fd = socket(AF_UNIX, SOCK_STREAM, 0);

   if(felix_fd != -1 && connect_felix_listen_server(OSGID_UNIX_SOCK) == 0)
   {
      cmsLog_debug("initosgidToFelixClient: osgid to felix socket is initialized %d\n", felix_fd);   
      usock_connected = TRUE;

      /* send a test message (hello) to felix, expect a hello response from felix listener */
      cmsLog_debug("osgidfelixListener: listener has been actived.\n");   
      cnt = recvfrom_felix_listen_server(buf, BUFLEN_256);
      if(cnt <= 0)
      {
         if(cnt < 0)
         {
            close(felix_fd);
            felix_fd = -1;
            usock_connected = FALSE;
         }
         return -1; 
      }

      /* update osgid execution environment status */
      modsw_setExecEnvStatus(OSGI_NAME, MDMVS_UP);

      osgiExecUp = TRUE;

      /* check to see if this is a bootup case.  Take of existing modules if needed */
      osgidProcessModulesAtBootup();
      if (requestList.head != NULL)
      {
         osgidProcessRequestList();
      }
   }
   else
   {
      if (felixRetry < RETRY_WAIT_FOR_FELIX_UP)
      {
         osgidWaitForFelixToComeUp();
         felixRetry++;
      }
      else
      {
         cmsLog_error("initosgidToFelixClient: osgid to felix socket fail to initialize, retry count %d\n",felixRetry);   
      }
      return -1;
   }
   return 0;
}

void osgidfelixListener(void)
{
   int   fd = felix_fd;
   int   cnt, n =0, max_len = 0;
   char *buf;
   char *p, *op;
   

   if ((buf = malloc(MAX_MSG_LEN)) == NULL)
   {
      return; /* should return resource exceeded */
   }
   memset(buf, 0,  MAX_MSG_LEN);

   //cmsLog_debug("osgidfelixListener: listener has been actived.\n");   
   cnt = recvfrom_felix_listen_server(buf, MAX_MSG_LEN);


   if(cnt <= 0)
   {
      if(cnt < 0)
      {
         close(fd);
         felix_fd = -1;
         usock_connected = FALSE;
      }
      free(buf);
      return; 
   }

   /* start processing messages from felix listener.
    * Ideally, meassage in format of DUresponseStateChangedMsgBody is returned.
    * Now, commands are called from osgid, and output of such commands is sent back
    * in a buffer.  It's up to osgid to extract and fill in the info to MDM.
    * To distinguish the different messages, the buffer is in the following format:
    * messageType (such as install, start, stop...):output of command
    *   
    *   install: bundleID, output of lb command 
    *    (when rx this, osgid processes bundle ID and output of lb command, fill in DU object)
    *
    *   start: bundleID,output of lb command
    *    (when rx this, osgid process lb command, look for status of bundleId and fill in status of EU with bundleID
    *   stop : bundleID, output of lb command
    *    (when rx this, osgid process lb command, look for status of bundleId and fill in status of EU with bundleID
    *   uninstall: bundleId, output of lb command
    *    (when rx this, osgid process lb command and remove all EU and DU from MDM)
    *
    *   update: bundleID, output of lb command
    *    (when rx this, osgid processes bundle ID and output of lb command, fill in DU object)
    *   
    *  msg buffer should be as followed:
    *    firstString:outputbufferOfLB
    *    firstString is messeageType which can contain install, start, stop, uninstall, update
    *    :  colon seperate messageType and output of LB
    *    lb output buffer such as
    */
   /* Felix takes some times to do install, uninstall operations.   Management application can
    * issue a lot of requests.   We will need to queue up the operations, and process them one 
    * at a time as felix response.
    * 1. how do we know which response is referring to what request? 
    * 2. Queue has pending requests, but there is ONLY one waiting for respond--there is timeout
    *    waiting for response from felix.  If this timeouts, error is responded to management
    *    applications that issue this request.  Next pending request is served.
    * 
    * Right now, for testing purpose, one operation is done at a time.
    */

   /* this is a workaround for felixd sending debug messages over along with LB error */
   if ( ((p = (strstr(buf, "uninstall:"))) == NULL) &&
        ((p = (strstr(buf, "install:"))) == NULL) &&
        ((p = (strstr(buf, "update:"))) == NULL) &&
        ((p = (strstr(buf, "start:"))) == NULL) &&
        ((p = (strstr(buf, "stop:"))) == NULL) &&
        ((p = (strstr(buf, "LB:"))) == NULL) )
   {
      cmsLog_debug("can't find command in response %s (len %d), do nothing.",buf,cnt);
      free(buf);
      return; /* format is wrong */
   }
   /* so now, p points to the operation */
   if ((op = strchr(p, ':')) == NULL)
   {
      cmsLog_debug("can't find ':' in response p %s (len %d), do nothing.",p,cnt);
      free(buf);
      return; /* format is wrong */
   }

   max_len = strlen("uninstall")+1;
      
   if((n = (op - p)) < max_len) 
   {
      char operation[max_len];
      
      strncpy(operation,p,n);
      operation[n] = '\0';
      
      if (strcmp(operation,SW_MODULES_OPERATION_UNINSTALL) == 0)
      {
         osgidProcessFelixUninstallResponse(p,cnt);
         /* uninstall could be done for more than one DUs at a time, requestListProcessing is set to FALSE after
          * all DUs are uninstalled.
          */
      }
      else if (strcmp(operation,SW_MODULES_OPERATION_INSTALL) == 0)
      {
         osgidProcessFelixInstallResponse(p,cnt);
         requestListProcessing = FALSE;
      }
      else if (strcmp(operation,SW_MODULES_OPERATION_UPDATE) == 0)
      {
         osgidProcessFelixUpdateResponse(p,cnt);
         /* update could be for more than one DUs at a time, requestListProcessing is set to FALSE after
          * all DUs are updated.
          */
      }
      else if (strcmp(operation,SW_MODULES_OPERATION_START) == 0)
      {
         osgidProcessFelixStartResponse(p,cnt);
         requestListProcessing = FALSE;
      }
      else if (strcmp(operation,SW_MODULES_OPERATION_STOP) == 0)
      {
         osgidProcessFelixStopResponse(p,cnt);
         requestListProcessing = FALSE;
      }
      else if (strcmp(operation,SW_MODULES_OPERATION_LB) == 0)
      {
         osgidProcessFelixLBResponse(p,cnt);
         requestListProcessing = FALSE;
      }
      else
      {
         cmsLog_debug("ignoring unrecognizable response %s (len %d), operation is %s, do nothing.",buf,cnt, operation);
      }
   }
   else
      cmsLog_debug("operation len %d is too long, response %s (len %d), do nothing.",n,buf,cnt);
   free(buf);
   osgidProcessRequestList();
}

CmsRet osgidAddBundleIdToList(char *bundleIdStr, pLIST_TYPE pBundleList)
{
   pLB_DATA pLbData;
   pENTRY_TYPE pEntry;
   int *pKey;

   /* allocate and initialize data first */
   pLbData = (pLB_DATA)malloc(sizeof(LB_DATA));
   pKey = (int *)malloc(sizeof(int));
   pEntry = (pENTRY_TYPE)malloc(sizeof(ENTRY_TYPE));
   if (pLbData == NULL)
   {
      cmsLog_error("resource error, cannot alloc new lb data");
      return CMSRET_RESOURCE_EXCEEDED;
   }
   if (pKey == NULL)
   {
      cmsLog_error("resource error, cannot alloc key");
      free(pLbData);
      return CMSRET_RESOURCE_EXCEEDED;
   }
   if (pEntry == NULL)
   {
      cmsLog_error("resource error, cannot alloc list entry structure");
      free(pLbData);
      free(pKey);
      return CMSRET_RESOURCE_EXCEEDED;
   }
   memset(pLbData, 0,sizeof(LB_DATA));
   pLbData->bundleId = atoi(bundleIdStr);
   *pKey = pLbData->bundleId;

   pEntry->next = NULL;
   pEntry->keyType = KEY_INT;
   pEntry->key = (void*)pKey;
   pEntry->data = (void*)pLbData;
   
   return(addEnd(pEntry,pBundleList));
}

CmsRet osgidFreeEntry(pENTRY_TYPE pEntry)
{
   if (pEntry)
   {
      if (pEntry->key)
      {
         free(pEntry->key);
      }
      if (pEntry->data)
      {
         free(pEntry->data);
      }
      free(pEntry);
   }
   return(CMSRET_SUCCESS);
}

CmsRet osgidFreeList(pLIST_TYPE pList)
{
   pENTRY_TYPE ptr, delPtr;

   if (pList == NULL)
   {
      return (CMSRET_SUCCESS);
   }
   for (ptr = pList->head; ptr != NULL ;)
   {
      delPtr = ptr;
      ptr = ptr->next;
      osgidFreeEntry(delPtr);
   }
   return(CMSRET_SUCCESS);
}

CmsRet osgidParseLBResponseFromFelix(char *buf, pLIST_TYPE pBundleList)
{
   CmsRet ret = CMSRET_SUCCESS;
   char *pToken;
   char *nextToken;
   int bundleId;
   int startLevel=0;
   char statusStr[BUFLEN_32];
   char lbBundleStr[BUFLEN_32];
   char *descriptionStr;
   char *pBeginofVersionStr, *pEndofVersionStr;
   int descCount, versionCount;
   pENTRY_TYPE pBundleEntry;
   pLB_DATA pLbData;
   char *bundleIdStr;
   int firstBundleId = -1;
   int errMsgReceived = 0;
   char *saveBundleStr;
   pENTRY_TYPE prevPtr, ptr;
   int len;
   UBOOL8 lbCommand = FALSE;

   /*
    * install return 1 or more bundles' status:
    * 
    *install:Bundle ID:index:LB:START LEVEL:1
    *install:Bundle IDs:index1, index2, indexN:LB:START LEVEL:1
    *0|Starting|0|System Bundle (3.1.0.SNAPSHOT):
    *1|Active|1|Apache Felix Bundle Repository (1.6.2):
    *2|Active|1|Apache Felix Gogo Command (0.7.0.SNAPSHOT):
    *3|Active|1|Apache Felix Gogo Runtime (0.7.0.SNAPSHOT):
    *4|Starting|1|Apache Felix Gogo USockListener (1.0.0.SNAPSHOT):
    *5|Installed|1|HelloWorld Plug-in (1.0.0)
    *
    * OR
    *    LB:START LEVEL:1:...
    */   
   /* parse LB to get several things: mainBundleId, bundleId, status, startLevel, description str */
   
   
   cmsLog_debug("enter");

   if ((pToken = strtok_r(buf,":",&nextToken)) != NULL)
   {
      /* expect operation to be one of these; otherwise, we just ignore this whole message */

      if ( (strcmp(pToken,SW_MODULES_OPERATION_UNINSTALL) != 0) &&
           (strcmp(pToken,SW_MODULES_OPERATION_INSTALL) != 0) &&
           (strcmp(pToken,SW_MODULES_OPERATION_UPDATE) != 0) &&
           (strcmp(pToken,SW_MODULES_OPERATION_START) != 0) &&
           (strcmp(pToken,SW_MODULES_OPERATION_STOP) != 0) &&
           (strcmp(pToken,SW_MODULES_OPERATION_LB) != 0) )
      {
         return CMSRET_INTERNAL_ERROR;
      }

      cmsLog_debug("operation of response is %s",pToken);

      if ((pToken = strtok_r(NULL,":",&nextToken)) != NULL)
      {
         if (strcmp(pToken,"Bundle ID") == 0)
         {
            pToken = strtok_r(NULL,":",&nextToken);
            /* pToken contains a single bundleID, or a bundle list */
            bundleIdStr = strtok_r(pToken, ",", &saveBundleStr);
            if (bundleIdStr == NULL)
            {
               /* this is a single bundleID */
               firstBundleId = atoi(pToken);
               ret = osgidAddBundleIdToList(pToken,pBundleList); 
            }
            else
            {
               firstBundleId = atoi(bundleIdStr);
               while (bundleIdStr != NULL)
               {
                  if ((ret = osgidAddBundleIdToList(bundleIdStr,pBundleList)) != CMSRET_SUCCESS)
                  {
                     break;
                  }
                  bundleIdStr = strtok_r(NULL, ",", &saveBundleStr);
               } /* for all bundleID */
            } /* bundle List */
         } /* Bundle ID */
         
        
         
         /* next we will see
          * ERR: errorMessage (signal operation--install,update...-- error OR
          * LB:START LEVEL:1: (signal operation successfull with LB result following) OR  
          * LB:ERR (signal operation success but LB command error  
          */
         pToken = strtok_r(NULL,":",&nextToken);
         if (strcmp(pToken,"ERR") == 0)
         {
            /* next token is error message */
            errMsgReceived = 1;
         }
         else
         {
            /* token is LB */
            /* determine if the next one is START LEVEL or ERR */
            pToken = strtok_r(NULL,":",&nextToken);
            if (strcmp(pToken,"ERR") == 0)
            {
               /* LB error, the next token is the error message */
               errMsgReceived = 1;               
            }
            else 
            {
               pToken = strtok_r(NULL,":",&nextToken);
               startLevel = atoi(pToken);
               if (pBundleList->head == NULL)
               {
                  lbCommand = TRUE;
               }
            }
         }
      }
      
      if (errMsgReceived)
      {
         /* retrieve operation error message */
         pToken = strtok_r(NULL,":",&nextToken);

         /* we just store the message to the first bundle entry */
         if (findEntry(pBundleList,&firstBundleId,KEY_INT,&prevPtr,&ptr) != 0)
         {
            pBundleEntry = (pENTRY_TYPE)(ptr);
            pLbData = (pLB_DATA)(pBundleEntry->data);
            len = strlen(pToken);
            if (len > BUFLEN_64)
            {
               len = BUFLEN_64;
            }
            pLbData->faultCode = CMSRET_DU_EE_MISMATCH;
            strncpy(pLbData->errMsg,pToken,len);
            pLbData->errMsg[len] = '\0';
            pLbData->status[0] = '\0';
         } /* store to the first entry's */         
      }
      else 
      {
         /* start parsing the bundles' status */
         while ((pToken = strtok_r(NULL,":",&nextToken)) != NULL)
         {
            /* skip all bundles' status until bundleId is reached */
            memset(statusStr,0,sizeof(statusStr));

            /* each token will containt 1 bundle's status */
            if (startLevel == -1)
            {
               sscanf(pToken,"%d | %s",&bundleId,statusStr);
            }
            else
            {
               sscanf(pToken,"%d | %s | %d",&bundleId,statusStr,&startLevel);
            }
            
            descriptionStr = strrchr(pToken,'|');

            if (lbCommand == TRUE)
            {
               /* this is and LB response */
               sprintf(lbBundleStr,"%d",bundleId);
               ret = osgidAddBundleIdToList(lbBundleStr,pBundleList); 
            }
            if (findEntry(pBundleList,&bundleId,KEY_INT,&prevPtr,&ptr) == 0)
            {
               /* is this the bundle we are interested in since LB always return
                  status of all bundles */   
               continue;
            }
            pBundleEntry = (pENTRY_TYPE)(ptr);
            pLbData = (pLB_DATA)(pBundleEntry->data);

            /* convert LB status to MDM status 
             * EU MDM valid strings are:  Idle, starting, active and stopping.
             * Corresponding LB response: Resolved, starting, active, and stopping
             *
             * DU MDM valid strings are:  Installing, Installed, updating, uninstalling and uninstalled 
             * Corresponding LB response: ----------, Installed, --------, ----------------------------
             */
            if (strcmp(statusStr,"Resolved") == 0)
            {
               len = strlen(MDMVS_IDLE);
               /* EU stopped, moved to Resolved state */
               strncpy(pLbData->status,MDMVS_IDLE,len);
            }
            else 
            {
               len = strlen(statusStr);
               /* active, installed, starting, stopping */
               strncpy(pLbData->status,statusStr,len);
            }

            pLbData->status[len] = '\0';
            pLbData->startLevel = startLevel;
            if (descriptionStr != NULL)
            {
               /* skip over  |: |space = 2 characters */
               descriptionStr += 2;
               pBeginofVersionStr = strchr(descriptionStr,'(');
               if (pBeginofVersionStr != NULL)
               {
                  /* skip over the ( */
                  pBeginofVersionStr += 1;
                  pEndofVersionStr = strrchr(descriptionStr,')');
                  /* -1 to skip ')' character */
                  descCount = pBeginofVersionStr-descriptionStr-1;
                  versionCount = pEndofVersionStr-pBeginofVersionStr;
               }
               else
               {
                  descCount = strlen(descriptionStr);
                  versionCount = 0;
               }
            }
            else
            {
               descCount=0;
               versionCount = 0;
            }
            if (descCount != 0)
            {
               strncpy(pLbData->description,descriptionStr,descCount);
               pLbData->description[descCount] = '\0';
               if (versionCount != 0)
               {
                  strncpy(pLbData->version,pBeginofVersionStr,versionCount);
                  pLbData->version[versionCount] = '\0';
               }
               else
               {
                  pLbData->version[0] = '\0';
               }
            } /* desc is found */
            else
            {
               /* this shouldn't happen, but we expect a name */
               sprintf(pLbData->description,"DevelopmentUnit%d",bundleId);
            }
         }/* while token */
      } /* not error */
      return (CMSRET_SUCCESS);
   } /* ret */
   return (CMSRET_INTERNAL_ERROR);
} /* osgidParseLBResponseInstallFromFelix */

void osgidProcessFelixInstallResponse(char *buf, int len)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack duIdStack;
   InstanceIdStack idStack = EMPTY_INSTANCE_ID_STACK;
   UINT32 instanceNum;
   DUObject *duObject;
   UBOOL8 found = FALSE;
   LIST_TYPE bundleList= {NULL,NULL};
   pENTRY_TYPE pEntry, pEntryToBeDeleted;
   pLB_DATA pData;
   char aliasStr[BUFLEN_64];
   char *euListStr = NULL;
   char euFullPathStr[BUFLEN_256];
   int length, curEuListLen=0;
   char *pEu = NULL, *nextEu = NULL;
   pREQUEST_DATA pRequestData;
   pENTRY_TYPE pProcessingEntry;
   char dateTimeBuf[BUFLEN_64];
   int faultCode = CMSRET_SUCCESS;
   char faultMsg[BUFLEN_256];

   /*
    *install:Bundle ID:5:LB:START LEVEL:1:
    *0|Starting|0|System Bundle (3.1.0.SNAPSHOT):
    *1|Active|1|Apache Felix Bundle Repository (1.6.2):
    *2|Active|1|Apache Felix Gogo Command (0.7.0.SNAPSHOT):
    *3|Active|1|Apache Felix Gogo Runtime (0.7.0.SNAPSHOT):
    *4|Starting|1|Apache Felix Gogo USockListener (1.0.0.SNAPSHOT):
    *5|Installed|1|HelloWorld Plug-in (1.0.0)
    */   
   /* parse LB to get several things: mainBundleId, bundleId, status, startLevel, description str */

   cmsLog_debug("enter");

   /* first parse the response */
   ret = osgidParseLBResponseFromFelix(buf, &bundleList);
   if (ret != CMSRET_SUCCESS)
   {
      /* if it's not a LB response, we just ignore it and wait for the next message */
      return;
   }
   if ((ret = cmsLck_acquireLockWithTimeout(OSGID_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d, drop message", ret);
      faultCode = CMSRET_INTERNAL_ERROR;
      strcpy(faultMsg,"unable to acquire lock.");
      osgidFreeList(&bundleList);
      goto Update_ProcessingList;
   }

   /* the very first entry in the bundle list is the DU, subsequent ones are EUs */
   pEntry = (pENTRY_TYPE)(removeFront(&bundleList));
   pData = (pLB_DATA)pEntry->data;

   /* first check if we are in the valid state to update this operation */
   INIT_INSTANCE_ID_STACK(&idStack);
   while (!found &&
          (ret = cmsObj_getNext(MDMOID_DU, &idStack, (void **)&duObject)) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strcmp(duObject->status, MDMVS_INSTALLING) == 0)
      {
         found = TRUE;
         break;
      }
      else
      {
         cmsObj_free((void **) &duObject);
         continue;
      }
   } /* while */

   if (!found) 
   {      
      ret = CMSRET_DU_UNKNOWN;
   }
   duIdStack = idStack;
   if (ret == CMSRET_SUCCESS)
   {      
      if (pData->faultCode != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to install, faultCode=%d, faultMessage %s", pData->faultCode,pData->errMsg);
         faultCode = pData->faultCode;
         strcpy(faultMsg,pData->errMsg);
         /* remove the DU created since this is a failure installation */
         if ((ret = cmsObj_deleteInstance(MDMOID_DU, &duIdStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("delete DU entry failed, ret=%d", ret);
         }
         else
         {
            cmsLog_debug("Installation failed, DU entry deleted");
            cmsMgm_saveConfigToFlash();
         }
         cmsObj_free((void **) &duObject);   
      } /* fail to install */
      else 
      {
         duObject->X_BROADCOM_COM_bundleId = pData->bundleId;

         CMSMEM_REPLACE_STRING(duObject->status, pData->status);
         if (cmsUtl_strcmp(duObject->status, MDMVS_INSTALLED) == 0)
         {
            duObject->resolved = TRUE;
         }
         /* Alias needs to be unique as across all DU, and can be changed by ACS, so it shouldn't be changed for reboot case */
         if ((IS_EMPTY_STRING(duObject->alias)) || (cmsUtl_strcmp(duObject->alias,"(null)") == 0))
         {
            /* Alias needs to be unique as across all DU, and can be changed by ACS */
            instanceNum = PEEK_INSTANCE_ID(&duIdStack);
            snprintf(aliasStr,sizeof(aliasStr),"%s::%d", OSGI_NAME, instanceNum);
            CMSMEM_REPLACE_STRING(duObject->alias, aliasStr);
         }
         /* Name is the name of DU which the author decides */
         CMSMEM_REPLACE_STRING(duObject->name, pData->description);
         CMSMEM_REPLACE_STRING(duObject->version, pData->version);
         CMSMEM_REPLACE_STRING(duObject->description, pData->description);
         cmsTms_getXSIDateTime(0, dateTimeBuf, sizeof(dateTimeBuf));
         CMSMEM_REPLACE_STRING(duObject->X_BROADCOM_COM_completeTime, dateTimeBuf);

         /* now we have to create the EU instances for this */
         pEntryToBeDeleted = pEntry;
         pEntry = (pENTRY_TYPE)(removeFront(&bundleList));
         INIT_INSTANCE_ID_STACK(&idStack);
         found = FALSE;
         if (pEntry == NULL)
         {
            if (!IS_EMPTY_STRING(duObject->executionUnitList) && (cmsUtl_strcmp(duObject->executionUnitList,"(null)") != 0))
            {
               /* this is from the config file, for reboot case */
               osgidUpdateExistingEUinstance(pData,duObject->executionUnitList);
            }
            else
            {
               osgidAddEUinstance(pData, euFullPathStr,sizeof(euFullPathStr),duObject->description);
               CMSMEM_REPLACE_STRING(duObject->executionUnitList, euFullPathStr);
            }
         } /* one DU/one EU */
         else
         {
            if (duObject->executionUnitList == NULL)
            {
               euListStr = (char*)malloc(MAX_EU_LIST_FULL_PATH_LEN);
               memset(euListStr,0,MAX_EU_LIST_FULL_PATH_LEN);
            }
            else
            {
               euListStr = strdup(duObject->executionUnitList);
               pEu = strtok_r(euListStr,",",&nextEu);
               found = 1;
            }
            while (pEntry != NULL)
            {
               pData = (pLB_DATA)pEntry->data;
               /* add EU , until NULL */
               if (!found)
               {
                  osgidAddEUinstance(pData, euFullPathStr,sizeof(euFullPathStr),duObject->description);
                  length = strlen(euFullPathStr);
                  if ((curEuListLen+length+1) < MAX_EU_LIST_FULL_PATH_LEN)
                  {
                     if (curEuListLen != 0)
                     {
                        strcat(euListStr,",");
                        length +=1;
                     }
                     strcat(euListStr,euFullPathStr);
                     curEuListLen += length;
                  }
               }
               else
               {
                  /* this is from the config file, for reboot case */
                  osgidUpdateExistingEUinstance(pData,pEu);
                  pEu = strtok_r(NULL,",",&nextEu);               
               }
               osgidFreeEntry(pEntry);
               pEntry = (pENTRY_TYPE)(removeFront(&bundleList));
            } /* while */
            if (!found)
            {
               CMSMEM_REPLACE_STRING(duObject->executionUnitList, euListStr);
            }
         } /* else one DU/multiple EU */

         if ((ret = cmsObj_set(duObject, &duIdStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("cannot change set duobject->executionUnitList %s, ret %d", 
                         duObject->executionUnitList,ret);
         }
         else
         {
            cmsMgm_saveConfigToFlash();
         }
         cmsObj_free((void **) &duObject);   
         osgidFreeEntry(pEntryToBeDeleted);
         free(euListStr);
      }
   } /* DU found  and faultCode == CMSRET_SUCCESS */
   osgidFreeList(&bundleList);
   cmsLck_releaseLock();

Update_ProcessingList:
   /*update processingList and send reply to App*/
   pProcessingEntry = (pENTRY_TYPE)(removeFront(&processingList));
   if (pProcessingEntry == NULL)
   {
      return; /* should return an error code TO-DO */
   }

   pRequestData = (pREQUEST_DATA)pProcessingEntry->data;
   pRequestData->FaultCode = faultCode;
   if (faultCode != CMSRET_SUCCESS)
   {
      strcpy(pRequestData->ErrMsg,faultMsg);
   }
   osgidSendReplyToRequestApp(pRequestData);
   osgidFreeEntry(pProcessingEntry);
   osgidCleanProcessingList();
} /* process install response */

void osgidProcessFelixUninstallResponse(char *buf, int len)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack idStack = EMPTY_INSTANCE_ID_STACK;
   DUObject *duObject;
   UBOOL8 found = FALSE;
   LIST_TYPE bundleList= {NULL,NULL};
   pENTRY_TYPE pEntry;
   pENTRY_TYPE prevpEntry, pProcessingEntry;
   pREQUEST_DATA pRequestData;
   pLB_DATA pData;
   int retval =0;;
   char *euList = NULL;
   int faultCode = CMSRET_SUCCESS;
   char faultMsg[BUFLEN_256]={0};
   char *duFullPath=NULL;
   MdmPathDescriptor pathDesc;

   cmsLog_debug("enter");

   /* first parse the response */
   osgidParseLBResponseFromFelix(buf, &bundleList);

   if ((ret = cmsLck_acquireLockWithTimeout(OSGID_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d, drop message", ret);
      osgidFreeList(&bundleList);
      //return; /* should return an error code TO-DO */
      faultCode = CMSRET_INTERNAL_ERROR;
      strcpy(faultMsg,"unable to acquire lock.");
      goto Update_ProcessingList;
   }

   /* the very first entry in the bundle list is the DU, subsequent ones are EUs */
   pEntry = (pENTRY_TYPE)(removeFront(&bundleList));
   pData = (pLB_DATA)pEntry->data;
  
   /* first check if we are in the valid state to update this operation */
   INIT_INSTANCE_ID_STACK(&idStack);
   while (!found &&
          (ret = cmsObj_getNext(MDMOID_DU, &idStack, (void **)&duObject)) == CMSRET_SUCCESS)
   {
      if (pData->bundleId == duObject->X_BROADCOM_COM_bundleId)
      {
         found = TRUE;
      }
      else
      {
         cmsObj_free((void **) &duObject);
      }
   } /* while */

   if ((!found) || (cmsUtl_strcmp(duObject->status, MDMVS_UNINSTALLING) != 0))
   {      
      ret = CMSRET_DU_UNKNOWN;
      faultCode = CMSRET_DU_UNKNOWN;
      strcpy(faultMsg, "unknown DU");
   }
   else if (found)
   {
      if (pData->faultCode != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to uninstall, faultCode=%d, faultMessage %s", pData->faultCode,pData->errMsg);
         faultCode = pData->faultCode;
         strcpy(faultMsg,pData->errMsg);
         /* fail to uinstalled, set back to installed */
         CMSMEM_REPLACE_STRING(duObject->status, MDMVS_INSTALLED);
         if ((ret = cmsObj_set(duObject, &idStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("cannot change set duobject->status back to installed; unistall failed. ret %d", 
                         ret);
         }
         else
         {
            cmsMgm_saveConfigToFlash();
         }
         cmsObj_free((void **) &duObject);   
      }
      else
      {      
         /* found, and uninstall is succesful */
         char *du_name;

         
         /* now we have to remove the EU instances of this DU */
         /* when uninstall is done, felix stop all apps already, we just need to clean up the data model */
         euList = strdup(duObject->executionUnitList);

         cmsLog_debug("Uninstall is successful, delete EUs %s", euList);

         osgidDeleteAllEuInstances(euList);

         if(duObject->URL != NULL){
            /*remove DU bundle from data partition*/
            if((du_name = strrchr(duObject->URL, '/')) != NULL)
            {
               du_name = du_name + 1; //skip '/'
               
               if((du_name != NULL) && (strlen(du_name) >0))
               {
                  char cmd[CMS_MAX_FULLPATH_LENGTH]={0};
                  char bundleDir[CMS_MAX_FULLPATH_LENGTH]={0};
                  int rc;

                  ret = cmsUtl_getRunTimePath(CMS_MODSW_OSGIEE_DU_DIR, bundleDir, sizeof(bundleDir));
                  if (ret != CMSRET_SUCCESS)
                  {
                     cmsLog_error("path is too long for bundleDir (len=%d)", sizeof(bundleDir));
                     /* continue on anyways, the command will fail */
                  }

                  rc = snprintf(cmd, sizeof(cmd), "rm -f %s/%s", bundleDir, du_name);
                  if (rc >= (int) sizeof(cmd))
                  {
                     cmsLog_error("cmd is too long for buffer, len=%d", sizeof(cmd));
                  }

                  system(cmd);
                  cmsLog_debug("removed DU %s/%s because it has been uninstalled.", bundleDir, du_name);
               }
            }         
         }      
    
         /* before deleting the DU instance, save the full path for error reporting */
         memset(&pathDesc, 0, sizeof(MdmPathDescriptor));
         pathDesc.oid = MDMOID_DU;
         pathDesc.iidStack = idStack;
         cmsMdm_pathDescriptorToFullPath(&pathDesc, &duFullPath);
         if ((ret = cmsObj_deleteInstance(MDMOID_DU,&idStack)) !=CMSRET_SUCCESS)
         {
            cmsLog_error("cannot delete DU after uninstall ret=%d", ret);
         }
         else
         {
            cmsMgm_saveConfigToFlash();
         }
         cmsObj_free((void **) &duObject);   
      }  /* install successful */
   } /* DU found */
   
   osgidFreeList(&bundleList);

   cmsLck_releaseLock();

Update_ProcessingList:
   /*update processingList*/
   cmsLog_debug("call findIntEntry for next item");
   found = findIntEntry(&processingList, OSGID_REQUEST_PROCESSING, &prevpEntry, &pProcessingEntry);
   if (found == 0)
   {
      retval = osgidProcessProcessingList(SW_MODULES_OPERATION_UNINSTALL);
   }
   else
   {
      pRequestData = (pREQUEST_DATA)pProcessingEntry->data;
      pRequestData->requestStatus = OSGID_REQUEST_DONE;
      if (euList)
      {
         strcpy(pRequestData->EUlist,euList);
      }
      if (duFullPath)
      {
         strcpy(pRequestData->DUlist,duFullPath);
      }
      pRequestData->FaultCode = faultCode;
      strcpy(pRequestData->ErrMsg,faultMsg);
      *(int *)pProcessingEntry->key = (int)pRequestData->requestStatus;
      retval = osgidProcessProcessingList(SW_MODULES_OPERATION_UNINSTALL);
   }

   if (euList)
   {
      free(euList);
   }
   if (duFullPath)
   {
      cmsMem_free(duFullPath);
   }

   if(retval == 0)
   {
      osgidSendReplyListToRequestApp(&processingList);
      osgidCleanProcessingList();
      requestListProcessing = FALSE;
   }

   cmsLog_debug("Done!");

   return;
}


void osgidProcessFelixUpdateResponse(char *buf, int len)
{
   /* this should be the same as install?
    * maybe all the EU of the previous version need to be deleted,
    * and new EU should be added.   Newer version of EU may have different versions and name.
   */
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack duIdStack;
   InstanceIdStack idStack = EMPTY_INSTANCE_ID_STACK;
   DUObject *duObject;
   UBOOL8 found = FALSE;
   LIST_TYPE bundleList= {NULL,NULL};
   pENTRY_TYPE pEntry, pEntryToBeDeleted;
   pLB_DATA pData;
   char *euListStr = NULL;
   char euFullPathStr[BUFLEN_256];
   int length, curEuListLen=0, foundEntry = 0, retval = 0;
   char *pEu = NULL, *nextEu = NULL;
   pREQUEST_DATA pRequestData;
   pENTRY_TYPE pProcessingEntry, prevpEntry;
   char dateTimeBuf[BUFLEN_64];
   int faultCode = CMSRET_SUCCESS;
   char faultMsg[BUFLEN_256];

   /*
    *install:Bundle ID:5:LB:START LEVEL:1:
    *0|Starting|0|System Bundle (3.1.0.SNAPSHOT):
    *1|Active|1|Apache Felix Bundle Repository (1.6.2):
    *2|Active|1|Apache Felix Gogo Command (0.7.0.SNAPSHOT):
    *3|Active|1|Apache Felix Gogo Runtime (0.7.0.SNAPSHOT):
    *4|Starting|1|Apache Felix Gogo USockListener (1.0.0.SNAPSHOT):
    *5|Installed|1|HelloWorld Plug-in (1.0.0)
    */   
   /* parse LB to get several things: mainBundleId, bundleId, status, startLevel, description str */

   cmsLog_debug("enter");

   /* first parse the response */
   ret = osgidParseLBResponseFromFelix(buf, &bundleList);
   if (ret != CMSRET_SUCCESS)
   {
      /* if it's not a LB response, we just ignore it and wait for the next message */
      return;
   }
   if ((ret = cmsLck_acquireLockWithTimeout(OSGID_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d, drop message", ret);
      osgidFreeList(&bundleList);
      faultCode = CMSRET_INTERNAL_ERROR;
      strcpy(faultMsg,"failed to get lock");
      goto Update_ProcessingList;
   }

   /* the very first entry in the bundle list is the DU, subsequent ones are EUs */
   pEntry = (pENTRY_TYPE)(removeFront(&bundleList));
   pData = (pLB_DATA)pEntry->data;

  
   /* first check if we are in the valid state to update this operation */
   INIT_INSTANCE_ID_STACK(&idStack);
   while (!found &&
          (ret = cmsObj_getNext(MDMOID_DU, &idStack, (void **)&duObject)) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strcmp(duObject->status, MDMVS_UPDATING) == 0)
      {
         found = TRUE;
         break;
      }
      else
      {
         cmsObj_free((void **) &duObject);
         continue;
      }
   } /* while */

   if (!found) 
   {      
      ret = CMSRET_DU_UNKNOWN;
      faultCode = pData->faultCode;
      strcpy(faultMsg,pData->errMsg);
   }
   else if (pData->faultCode != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to update, faultCode=%d, faultMessage %s", pData->faultCode,pData->errMsg);
      osgidFreeList(&bundleList);
      faultCode = pData->faultCode;
      strcpy(faultMsg,pData->errMsg);
      /* set back to INSTALLED since Update fails */
      CMSMEM_REPLACE_STRING(duObject->status, MDMVS_INSTALLED);
      if ((ret = cmsObj_set(duObject, &duIdStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cannot change set duobject->executionUnitList %s, ret %d", 
                      duObject->executionUnitList,ret);
      }
      else
      {
         cmsMgm_saveConfigToFlash();
      }
      cmsObj_free((void **) &duObject);   
   }
   else 
   {
      duIdStack = idStack;
      if (ret == CMSRET_SUCCESS)
      {      
         CMSMEM_REPLACE_STRING(duObject->status, pData->status);
         /* Name is the name of DU which the author decides */
         CMSMEM_REPLACE_STRING(duObject->name, pData->description);
         CMSMEM_REPLACE_STRING(duObject->version, pData->version);
         CMSMEM_REPLACE_STRING(duObject->description, pData->description);
         cmsTms_getXSIDateTime(0, dateTimeBuf, sizeof(dateTimeBuf));
         CMSMEM_REPLACE_STRING(duObject->X_BROADCOM_COM_completeTime, dateTimeBuf);                        
         
         /* now we have to create the EU instances for this */
         pEntryToBeDeleted = pEntry;
         pEntry = (pENTRY_TYPE)(removeFront(&bundleList));
         INIT_INSTANCE_ID_STACK(&idStack);
         found = FALSE;
         if (pEntry == NULL)
         {
            /* add this DU as an EU too */
            /* check to see if this EU has already existed -- in the case of install of DU from config file */
            if (duObject->executionUnitList != NULL)
            {
               osgidUpdateExistingEUinstance(pData,duObject->executionUnitList);
            }
            else
            {
               osgidAddEUinstance(pData, euFullPathStr,sizeof(euFullPathStr),duObject->description);
               CMSMEM_REPLACE_STRING(duObject->executionUnitList, euFullPathStr);
            }
         } /* one DU/one EU */
         else
         {
            if (duObject->executionUnitList == NULL)
            {
               euListStr = (char*)malloc(MAX_EU_LIST_FULL_PATH_LEN);
               memset(euListStr,0,MAX_EU_LIST_FULL_PATH_LEN);
            }
            else
            {
               euListStr = strdup(duObject->executionUnitList);
               pEu = strtok_r(euListStr,",",&nextEu);
               found = 1;
            }
            while (pEntry != NULL)
            {
               pData = (pLB_DATA)pEntry->data;
               /* add EU , until NULL */
               if (!found)
               {
                  osgidAddEUinstance(pData,euFullPathStr,sizeof(euFullPathStr),duObject->description);
                  length = strlen(euFullPathStr);
                  if ((curEuListLen+length+1) < MAX_EU_LIST_FULL_PATH_LEN)
                  {
                     if (curEuListLen != 0)
                     {
                        strcat(euListStr,",");
                        length +=1;
                     }
                     strcat(euListStr,euFullPathStr);
                     curEuListLen += length;
                  }
               }
               else
               {
                  osgidUpdateExistingEUinstance(pData,pEu);
                  pEu = strtok_r(NULL,",",&nextEu);               
               }
               osgidFreeEntry(pEntry);
               pEntry = (pENTRY_TYPE)(removeFront(&bundleList));
            } /* while */
            if (!found)
            {
               CMSMEM_REPLACE_STRING(duObject->executionUnitList, euListStr);
            }
         } /* one DU/multiple DU */
         if ((ret = cmsObj_set(duObject, &duIdStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("cannot change set duobject->executionUnitList %s, ret %d", 
                         duObject->executionUnitList,ret);
         }
         else
         {
            cmsMgm_saveConfigToFlash();
         }
         cmsObj_free((void **) &duObject);   
         osgidFreeEntry(pEntryToBeDeleted);
         free(euListStr);
      } /* no error */
   } /* DU found */
   osgidFreeList(&bundleList);
   cmsLck_releaseLock();

Update_ProcessingList:
   /*update processingList*/
   foundEntry = findIntEntry(&processingList, OSGID_REQUEST_PROCESSING, &prevpEntry, &pProcessingEntry);
   if (foundEntry == 0)
   {
      retval = osgidProcessProcessingList(SW_MODULES_OPERATION_UPDATE);
   }
   else
   {
      pRequestData = (pREQUEST_DATA)pProcessingEntry->data;

      pRequestData->requestStatus = OSGID_REQUEST_DONE;
      pRequestData->FaultCode = faultCode;
      strcpy(pRequestData->ErrMsg,faultMsg);
      *(int *)pProcessingEntry->key = (int)pRequestData->requestStatus;
      retval = osgidProcessProcessingList(SW_MODULES_OPERATION_UPDATE);
   }

   if(retval == 0)
   {
      osgidSendReplyListToRequestApp(&processingList);
      osgidCleanProcessingList();
      requestListProcessing = FALSE;
   }
   return;
}


void osgidProcessFelixStartResponse(char *buf, int len)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack idStack = EMPTY_INSTANCE_ID_STACK;
   EUObject *euObject;
   UBOOL8 found = FALSE;
   LIST_TYPE bundleList= {NULL,NULL};
   pENTRY_TYPE pEntry;
   pLB_DATA pData;

   cmsLog_debug("enter");

   /* first parse the response */
   osgidParseLBResponseFromFelix(buf, &bundleList);

   if ((ret = cmsLck_acquireLockWithTimeout(OSGID_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d, drop message", ret);
      osgidFreeList(&bundleList);
      return; /* should return an error code TO-DO */
   }

   /* the very first entry in the bundle list is the DU, subsequent ones are EUs */
   pEntry = (pENTRY_TYPE)(removeFront(&bundleList));
   pData = (pLB_DATA)pEntry->data;

   /* first check if we are in the valid state to update this operation */
   INIT_INSTANCE_ID_STACK(&idStack);
   while (!found &&
          (ret = cmsObj_getNext(MDMOID_EU, &idStack, (void **)&euObject)) == CMSRET_SUCCESS)
   {
      if (pData->bundleId == euObject->X_BROADCOM_COM_bundleId)
      {
         found = TRUE;
      }
      else
      {
         cmsObj_free((void **) &euObject);
      }
   } /* while */

   if ((!found) || (cmsUtl_strcmp(euObject->status, MDMVS_STARTING) != 0))
   {      
      cmsLog_error("couldn't find the right EU in mdm: bundle = %d", pData->bundleId);
      ret = CMSRET_INTERNAL_ERROR;
   }

   if (ret == CMSRET_SUCCESS)
   {      
      /* now we have to update the EU status*/
      if(pData->errMsg[0] != '\0')
      {
         cmsLog_error("start EU %d err from felix: %s, curr status is %s", 
            pData->bundleId, pData->errMsg, euObject->status);
         CMSMEM_REPLACE_STRING(euObject->executionFaultCode, MDMVS_FAILUREONSTART);
         CMSMEM_REPLACE_STRING(euObject->executionFaultMessage, pData->errMsg);
         CMSMEM_REPLACE_STRING(euObject->requestedState, MDMVS_IDLE);
         CMSMEM_REPLACE_STRING(euObject->status, MDMVS_IDLE);
      }
      else
      {
         cmsLog_debug("start EU %d successful from felix, curr status is %s", 
                      pData->bundleId, pData->status);
         CMSMEM_REPLACE_STRING(euObject->executionFaultCode, MDMVS_NOFAULT);
         
         if(euObject->executionFaultMessage != NULL)
         {
            CMSMEM_REPLACE_STRING(euObject->executionFaultMessage, "");
         }
         CMSMEM_REPLACE_STRING(euObject->status, pData->status);       
      }     

      if ((ret = cmsObj_set(euObject, &idStack)) !=CMSRET_SUCCESS)
      {
         cmsLog_error("cannot set EU after start ret=%d", ret);
      }
      else
      {
         cmsMgm_saveConfigToFlash();
      }
      cmsObj_free((void **) &euObject);   
   } /* DU found */

   
   osgidFreeList(&bundleList);

   cmsLck_releaseLock();
   return;
}

void osgidProcessFelixStopResponse(char *buf, int len)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack idStack = EMPTY_INSTANCE_ID_STACK;
   EUObject *euObject;
   UBOOL8 found = FALSE;
   LIST_TYPE bundleList= {NULL,NULL};
   pENTRY_TYPE pEntry;
   pLB_DATA pData;

   cmsLog_debug("enter");

   /* first parse the response */
   osgidParseLBResponseFromFelix(buf, &bundleList);

   if ((ret = cmsLck_acquireLockWithTimeout(OSGID_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d, drop message", ret);
      osgidFreeList(&bundleList);
      return; /* should return an error code TO-DO */
   }

   /* the very first entry in the bundle list is the DU, subsequent ones are EUs */
   pEntry = (pENTRY_TYPE)(removeFront(&bundleList));
   pData = (pLB_DATA)pEntry->data;

   /* first check if we are in the valid state to update this operation */
   INIT_INSTANCE_ID_STACK(&idStack);
   while (!found &&
          (ret = cmsObj_getNext(MDMOID_EU, &idStack, (void **)&euObject)) == CMSRET_SUCCESS)
   {
      if (pData->bundleId == euObject->X_BROADCOM_COM_bundleId)
      {
         found = TRUE;
      }
      else
      {
         cmsObj_free((void **) &euObject);
      }
   } /* while */

   if ((!found) || (cmsUtl_strcmp(euObject->status, MDMVS_STOPPING) != 0))
   {     
      cmsLog_error("couldn't find the right EU in mdm: bundle = %d", pData->bundleId);
      ret = CMSRET_INTERNAL_ERROR;
   }

   if (ret == CMSRET_SUCCESS)
   {      
      /* now we have to update the EU status*/
      if(pData->errMsg[0] != '\0')
      {
         cmsLog_error("stop EU %d err from felix: %s, curr status is %s", 
            pData->bundleId, pData->errMsg, pData->status);
         CMSMEM_REPLACE_STRING(euObject->executionFaultCode, MDMVS_FAILUREONSTOP);
         CMSMEM_REPLACE_STRING(euObject->executionFaultMessage, pData->errMsg);
      }
      else
      {
         cmsLog_debug("stop EU %d successful from felix, curr status is %s", 
            pData->bundleId, pData->status);
         CMSMEM_REPLACE_STRING(euObject->executionFaultCode, MDMVS_NOFAULT);
         
         if(euObject->executionFaultMessage != NULL)
            CMSMEM_REPLACE_STRING(euObject->executionFaultMessage, "");
      }

      if ((cmsUtl_strcmp(pData->status, "Resolved") == 0) ||
          (cmsUtl_strcmp(pData->status, "Installed") == 0) )
      {
         CMSMEM_REPLACE_STRING(euObject->status, MDMVS_IDLE);
      }
      else
      {
         CMSMEM_REPLACE_STRING(euObject->status, pData->status);
      }

      if ((ret = cmsObj_set(euObject, &idStack)) !=CMSRET_SUCCESS)
      {
         cmsLog_error("cannot set EU after start ret=%d", ret);
      }
      else
      {
         cmsMgm_saveConfigToFlash();
      }
      cmsObj_free((void **) &euObject);   
   } /* DU found */

   
   osgidFreeList(&bundleList);

   cmsLck_releaseLock();
   return;
}


void osgidAddEUinstance(pLB_DATA pData, char *euFullPathStr, UINT32 euFullPathStrLen, char *duDescription)
{
   CmsRet ret = CMSRET_SUCCESS;
   char execEnvFullPath[BUFLEN_256];
   char euIdStr[BUFLEN_64]={0};  // length matches data model
   char aliasStr[BUFLEN_64]={0}; // length matches data model
   char nameBuf[BUFLEN_32]={0};  // length matches data model

   cmsLog_debug("enter: duDescription=%s", duDescription);

   /* cram a potentially long description into 32 byte name field */
   snprintf(nameBuf,sizeof(nameBuf),"%s", duDescription);

   sprintf(euIdStr,"%s::%s::{i}",OSGI_NAME, nameBuf);

   /* alias should be uniquely assigned by CPE first, and then can be changed by ACS */
   /* Initially, make alias the same as euId */
   sprintf(aliasStr,"%s::%s::{i}", OSGI_NAME, nameBuf);

   /* execEnv which this EU belongs to */
   qdmModsw_getExecEnvFullPathByNameLocked(OSGI_NAME, execEnvFullPath, sizeof(execEnvFullPath));

   ret = modsw_addEuEntryLocked(euIdStr, aliasStr, nameBuf,
              OSGI_NAME, execEnvFullPath,
              NULL,
              pData->description, pData->version,
              pData->description, NULL, pData->bundleId,
              euFullPathStr, euFullPathStrLen);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("modsw_addEuEntry failed, ret=%d", ret);
   }

   return;
}

void osgidDeleteEUinstance(char *euFullPathStr)
{

   MdmPathDescriptor pathDesc;
   char tmpEuFullPathStr[BUFLEN_512];
   char *fullStr;
   int len;
   CmsRet ret;

   cmsLog_debug("Enter: euFullPathStr: %s",euFullPathStr);

   if ( (IS_EMPTY_STRING(euFullPathStr)) || (cmsUtl_strcmp(euFullPathStr,"(null)") == 0) )
   {
      return;
   }
   fullStr = strdup(euFullPathStr);
   if (fullStr == NULL)
   {
      return;
   }
   len = strlen(fullStr);
   if (fullStr[len-1] == ',')
   {
      /* remove , */
      fullStr[len-1] = '\0';
      len-=1;
   }
   if (fullStr[len-1] == '.')
   {
      ret = cmsMdm_fullPathToPathDescriptor(fullStr, &pathDesc);
   }
   else
   {
      /* add a dot at the end to indicate that the path is an object path */
      snprintf(tmpEuFullPathStr, sizeof(tmpEuFullPathStr), "%s.", fullStr);
      ret = cmsMdm_fullPathToPathDescriptor(tmpEuFullPathStr, &pathDesc);
   }
   if ((ret = cmsObj_deleteInstance(pathDesc.oid, &(pathDesc.iidStack))) !=CMSRET_SUCCESS)
   {
      cmsLog_error("unable to delete %s",euFullPathStr);
   }
   else
   {
      cmsLog_debug("deleted %s",euFullPathStr);
   }
   free(fullStr);
}

void osgidDeleteAllEuInstances(char *euList)
{
   char *pEu = NULL, *nextEu = NULL;

   pEu = strtok_r(euList,",",&nextEu);
   osgidDeleteEUinstance(pEu);
   while ((pEu = strtok_r(NULL,",",&nextEu)) != NULL)
   {
      osgidDeleteEUinstance(pEu); 
   }
}

void osgidAddDuStateChangeToList(CmsMsgHeader *msg)
{
   CmsRet ret=CMSRET_SUCCESS;
   DUObject *duObject;
   InstanceIdStack idStack = EMPTY_INSTANCE_ID_STACK;
   DUrequestStateChangedMsgBody *pDuInfo = NULL;
   pREQUEST_DATA pRequestData;
   int found = 0;
   int err = 0;
   char errMsg[BUFLEN_64];

   /* queue this up for processing */
   /* upon finishing the request--when status is sent back from felix, requestData is freed */
   pRequestData = malloc(sizeof(REQUEST_DATA));   
   if (pRequestData == NULL)
   {
      ret = CMSRET_RESOURCE_EXCEEDED;
   }
   else
   {
      memset(pRequestData,0,sizeof(REQUEST_DATA));
      pDuInfo = (DUrequestStateChangedMsgBody *) (msg + 1);
      strcpy(pRequestData->URL,pDuInfo->URL);
      strcpy(pRequestData->operation,pDuInfo->operation);
      strcpy(pRequestData->UUID,pDuInfo->UUID);
      strcpy(pRequestData->version,pDuInfo->version);
      strcpy(pRequestData->username,pDuInfo->username);
      strcpy(pRequestData->password,pDuInfo->password);
      strcpy(pRequestData->executionEnv,pDuInfo->execEnvFullPath);
      pRequestData->reqId = pDuInfo->reqId;
      pRequestData->requestStatus = OSGID_REQUEST_NOT_START;
      pRequestData->type = msg->type;
      pRequestData->src = msg->src;
      pRequestData->FaultCode = 0;

      /* first check to see if this is a good request or not */
      if ((cmsLck_acquireLockWithTimeout(OSGID_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get lock, ret=%d", ret);
         cmsLck_dumpInfo();
         return;
      }

      INIT_INSTANCE_ID_STACK(&idStack);
      while (!found &&
             (ret = cmsObj_getNext(MDMOID_DU, &idStack, (void **)&duObject)) == CMSRET_SUCCESS)
      {
         if (cmsUtl_strcmp(duObject->UUID, pDuInfo->UUID) != 0)
         {
            cmsObj_free((void **) &duObject);
            continue;
         }
         else
         {
            found = 1;
            /* checking for error (du status: installing, installed, updating and uninstalled):
             * if operation is install, duObject is installing or installed, ignore request.
             * if operation is uninstall, duObject is uninstalling or uninstalll, ignore request 
             */
            if (strcmp(pRequestData->operation,"install") == 0)
            {
               if ((strcmp(duObject->status,"installing") == 0) ||
                   (strcmp(duObject->status,"installed") == 0))
               {
                  ret = CMSRET_DU_STATE_INVALID;
               }
            }
            else if (strcmp(pRequestData->operation,"uninstall") == 0)
            {
               if ((strcmp(duObject->status,"uninstalling") == 0) ||
                   (strcmp(duObject->status,"uninstalled") == 0))
               {
                  ret = CMSRET_DU_STATE_INVALID;
               }
            }
            /* check for update: the duObject should be installed */

         } /* else uuid found */
      } /* while */

      /*if this an uninstall operation, and we can not find this DU in MDM, it's error.*/
      if (!found)
      {
         if (strcmp(pRequestData->operation,"uninstall") == 0)
         {
            err = 1;
            ret = CMSRET_DU_UNKNOWN;
         }
      }

      if ((!found && !err) || ((found) && (ret == CMSRET_SUCCESS)))
      {
         ret = osgidAddRequestToList(pRequestData);      
      }

      if (((found) && (ret != CMSRET_SUCCESS)))
      {
         /* and there is error with install, there is no error indication? */
         sprintf(errMsg,"cannot add request to list, return code %d",ret);
#ifdef dufaultmsg
         /* miwang: DUObject does not have a faultMessage param */
         CMSMEM_REPLACE_STRING(duObject->faultMessage,errMsg);
#endif
         cmsObj_set(duObject, &idStack);
         cmsObj_free((void **) &duObject);
      }
      cmsLck_releaseLock();
   } /* reqData is not NULL */
} /* osgidAddDuStateChangeToList */

void osgidAddEuStateChangeToList(CmsMsgHeader *msg)
{
   CmsRet ret=CMSRET_SUCCESS;
   EUrequestStateChangedMsgBody *pEuInfo = NULL;
   pREQUEST_DATA pRequestData;


   /* queue this up for processing */
   /* upon finishing the request--when status is sent back from felix, requestData is freed */
   pRequestData = malloc(sizeof(REQUEST_DATA));   
   if (pRequestData == NULL)
   {
      ret = CMSRET_RESOURCE_EXCEEDED;
   }
   else
   {
      memset(pRequestData, 0, sizeof(REQUEST_DATA));
      pEuInfo = (EUrequestStateChangedMsgBody *) (msg + 1);
      strcpy(pRequestData->operation,pEuInfo->operation);
      strcpy(pRequestData->euid,pEuInfo->euid);
      strcpy(pRequestData->name,pEuInfo->name);
      pRequestData->requestStatus = OSGID_REQUEST_NOT_START;
      pRequestData->type = msg->type;
      pRequestData->src = msg->src;
      pRequestData->FaultCode = 0;

      ret = osgidAddRequestToList(pRequestData);
   }


   /* this is for the case when addRequestToList fails */
   if (ret != CMSRET_SUCCESS)
   {
      char errMsg[BUFLEN_32];
      UBOOL8 found = FALSE;
      EUObject *euObject=NULL;
      InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

      if ((cmsLck_acquireLockWithTimeout(OSGID_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
      {
         return;
      }

      while (!found &&
             (ret = cmsObj_getNext(MDMOID_EU, &iidStack, (void **)&euObject)) == CMSRET_SUCCESS)
      {
         if (!cmsUtl_strcmp(euObject->EUID, pEuInfo->euid))
         {
            CmsRet r2;

            found=TRUE;
            snprintf(errMsg, sizeof(errMsg), "return code %d",ret);
            CMSMEM_REPLACE_STRING(euObject->executionFaultMessage,errMsg);
            r2 = cmsObj_set(euObject, &iidStack);
            if (r2 != CMSRET_SUCCESS)
            {
               cmsLog_error("set of executionFaultMessage failed, r2=%d", r2);
            }
         }

         cmsObj_free((void **) &euObject);
      }

      cmsLck_releaseLock();
   }

   return;
}


/* This is called when a request message is received.
 * Or, when the modem just boots up, and OSGI/JVM execution environment
 * is up. 
 */
CmsRet osgidAddRequestToList(pREQUEST_DATA pRequestData)
{
   pENTRY_TYPE pEntry;

   /* allocate and initialize data first */
   pEntry = (pENTRY_TYPE)malloc(sizeof(ENTRY_TYPE));
   if (pRequestData == NULL)
   {
      cmsLog_error("resource error, cannot alloc new request data");
      return CMSRET_RESOURCE_EXCEEDED;
   }
   if (pEntry == NULL)
   {
      cmsLog_error("resource error, cannot alloc list entry structure");
      free(pRequestData);
      return CMSRET_RESOURCE_EXCEEDED;
   }
   memset(pEntry,0,sizeof(ENTRY_TYPE));
   pEntry->next = NULL;
   pEntry->data = (void*)pRequestData;
   pEntry->key = NULL;
   return(addEnd(pEntry,&requestList));
}

CmsRet osgidAddRequestDataToProcessingList(REQUEST_DATA *reqdata)
{
   CmsRet ret=CMSRET_SUCCESS;
   pREQUEST_DATA pRequestData;
   pENTRY_TYPE pEntry;
   int *pKey=NULL;

   pRequestData = malloc(sizeof(REQUEST_DATA));   
   if (pRequestData == NULL)
   {
      ret = CMSRET_RESOURCE_EXCEEDED;
   }
   else
   {
      memset(pRequestData, 0, sizeof(REQUEST_DATA));
      strcpy(pRequestData->URL,reqdata->URL);
      strcpy(pRequestData->operation,reqdata->operation);
      strcpy(pRequestData->UUID,reqdata->UUID);
      strcpy(pRequestData->version,reqdata->version);
      strcpy(pRequestData->username,reqdata->username);
      strcpy(pRequestData->password,reqdata->password);
      pRequestData->reqId = reqdata->reqId;
      pRequestData->requestStatus = reqdata->requestStatus;
      pRequestData->type = reqdata->type;
      pRequestData->src = reqdata->src;
      pRequestData->FaultCode = reqdata->FaultCode;

      /* allocate and initialize data first */
      pEntry = (pENTRY_TYPE)malloc(sizeof(ENTRY_TYPE));
      if (pEntry == NULL)
      {
         cmsLog_error("resource error, cannot alloc list entry structure");
         free(pRequestData);
         return CMSRET_RESOURCE_EXCEEDED;
      }

      pKey = malloc(sizeof(int));
      if (pKey == NULL)
      {
         cmsLog_error("resource error,can't allocating pKey");
         free(pRequestData);
         free(pEntry);
         return CMSRET_RESOURCE_EXCEEDED;
      }
      *pKey = (int)reqdata->requestStatus;
      

      pEntry->next = NULL;
      pEntry->data = (void*)pRequestData;
      pEntry->key = (void*)pKey;
      ret = addEnd(pEntry,&processingList);
   }
   return ret;
}

void osgidCleanProcessingList(void)
{
   pENTRY_TYPE pEntry= NULL;   

   while((pEntry = (pENTRY_TYPE)removeFront(&processingList)) != NULL)
   {
      osgidFreeEntry(pEntry);
      pEntry = NULL;
   }
}

/*
Function: osgidProcessProcessingList
This function will be called when we have multiple DUs to handle for Update/Uninstall operation.
*/
int osgidProcessProcessingList(const char *operation)
{
   pREQUEST_DATA pRequestData = NULL;
   pENTRY_TYPE pEntry=NULL;
   pENTRY_TYPE prevpEntry=NULL;
   CmsRet ret = CMSRET_SUCCESS;
   int found = 0;
   InstanceIdStack idStack = EMPTY_INSTANCE_ID_STACK;
   DUObject *duObject = NULL;
   int duid = 0, retval = 0;
   char buf[CMS_MAX_FULLPATH_LENGTH]={0};
   char * filename = NULL;
   char dateTimeBuf[BUFLEN_64];

   cmsLog_debug("operation=%s", operation);

   found = findIntEntry(&processingList, OSGID_REQUEST_NOT_START, &prevpEntry, &pEntry);
   if (found == 0)
   {
      return 0;
   }

   found = 0; /*reset found for next loop*/

   if ((ret = cmsLck_acquireLockWithTimeout(OSGID_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d, drop message", ret);
      goto Error;
   }
   
   pRequestData = (pREQUEST_DATA)pEntry->data;

   if(strcasecmp(operation, SW_MODULES_OPERATION_UNINSTALL) == 0)
   {
      INIT_INSTANCE_ID_STACK(&idStack);
      while (!found &&
             (ret = cmsObj_getNext(MDMOID_DU, &idStack, (void **)&duObject)) == CMSRET_SUCCESS)
      {
         if ((cmsUtl_strcmp(duObject->UUID, pRequestData->UUID) == 0)  &&
             (cmsUtl_strcmp(duObject->version, pRequestData->version) == 0) )
         {
            found = 1;         
         }
         else
         {
            cmsObj_free((void **) &duObject);
         }
      }
      
      if (!found)
      {
         ret = CMSRET_DU_UNKNOWN;
      }
      else if ((cmsUtl_strcmp(duObject->status, MDMVS_INSTALLED) != 0))
      {
         cmsObj_free((void **) &duObject);
         ret =  CMSRET_DU_STATE_INVALID;
      }

      if (ret == CMSRET_SUCCESS)
      {
         duid = atoi(duObject->DUID);
         /* update the status */
         CMSMEM_REPLACE_STRING(duObject->status, MDMVS_UNINSTALLING);
         cmsTms_getXSIDateTime(0, dateTimeBuf, sizeof(dateTimeBuf));
         CMSMEM_REPLACE_STRING(duObject->X_BROADCOM_COM_startTime, dateTimeBuf);
         if ((ret = cmsObj_set(duObject, &idStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("cannot change state to uninstalling, ret=%d", ret);            
         }
         else
         {
            cmsMgm_saveConfigToFlash();
         }
         cmsObj_free((void **) &duObject);
      }
      cmsLck_releaseLock();

      /* continue with felix bundle listener */
      if (ret == CMSRET_SUCCESS)
      {           
         sprintf(buf,"%s %d","uninstall", duid);
         if (sendto_felix_listen_server(buf,strlen(buf)))
         {
            cmsLog_debug("uninstall bundle %d message sent to felix", duid);
            pRequestData->requestStatus = OSGID_REQUEST_PROCESSING;
            *(int *)pEntry->key = (int)pRequestData->requestStatus;
            ret = CMSRET_SUCCESS;
            retval = 1;
         }
         else
         {
            cmsLog_debug("error sending uninstall bundle %d message sent to felix", duid);
            ret = CMSRET_INTERNAL_ERROR;

            /*fail to send request to felix, we have to retrieve DU state*/
            osgidRetrieveDUorEUState(pRequestData->operation);
         }
      }

      if(ret != CMSRET_SUCCESS)
      {
            goto Error;
      }
   }/*SW_MODULES_OPERATION_UNINSTALL*/
   else if(strcasecmp(operation, SW_MODULES_OPERATION_UPDATE) == 0)
   {
      char bundleDir[CMS_MAX_FULLPATH_LENGTH]={0};

      INIT_INSTANCE_ID_STACK(&idStack);
      while (!found &&
             (ret = cmsObj_getNext(MDMOID_DU, &idStack, (void **)&duObject)) == CMSRET_SUCCESS)
      {         
         cmsLog_debug("update_only_uuid: du->uuid %s, uuid %s\n",
                      duObject->UUID, pRequestData->UUID);

         /* next testing UUID */
         if ((cmsUtl_strcmp(duObject->UUID, pRequestData->UUID) == 0)
            && (cmsUtl_strcmp(duObject->version, pRequestData->version) == 0))
         {
            found = 1;
            
            if (pRequestData->URL[0] == '\0')
            {
               strcpy(pRequestData->URL,duObject->URL);
            }
            if((filename = strrchr(pRequestData->URL, '/')) == NULL)
            {
               cmsLog_debug("update_only_uuid: filename in pRequestData->url %s is NULL\n",
                            pRequestData->URL);
               ret = CMSRET_INTERNAL_ERROR;
            }
            else
            {

               ret = cmsUtl_getRunTimePath(CMS_MODSW_OSGIEE_DU_DIR, bundleDir, sizeof(bundleDir));
               if (ret != CMSRET_SUCCESS)
               {
                  cmsLog_error("path is too long for bundleDir (len=%d)", sizeof(bundleDir));
               }
               else
               {
                  ret = osgidDoUpdate(pRequestData->URL,pRequestData->username, pRequestData->password,
                     bundleDir,pRequestData->version,(void*)duObject);
               }
            }
            break;
            
         }
         else 
         {
            cmsObj_free((void **) &duObject);
            continue;
         }         
      } /* while */
      
      if (!found)
      {      
         ret = CMSRET_DU_UNKNOWN;
      }
      
      /* update the status */
      if (ret == CMSRET_SUCCESS)
      {
         duid = atoi(duObject->DUID);
         CMSMEM_REPLACE_STRING(duObject->status, MDMVS_UPDATING);
         cmsTms_getXSIDateTime(0, dateTimeBuf, sizeof(dateTimeBuf));
         CMSMEM_REPLACE_STRING(duObject->X_BROADCOM_COM_startTime, dateTimeBuf);
         if ((ret = cmsObj_set(duObject, &idStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("cannot change state to updating ret=%d", ret);
         }
         else 
         {
            cmsMgm_saveConfigToFlash();
         }
      }
      else
      {
         /* else, if osgidDoUpdate returns error, duOjbect should be set to something? */
         cmsLog_error("osgidDoUpdate returns error ret=%d, stop updating", ret);
      }


      if (found)
      {
         cmsObj_free((void **) &duObject);
      }
      cmsLck_releaseLock();
      
      /* call felix update */
      if (ret == CMSRET_SUCCESS)
      {
         filename++;  /* move past the / */

         sprintf(buf,"%s %d file:%s/%s","update", duid, bundleDir, filename);
         if (sendto_felix_listen_server(buf,strlen(buf)))
         {
            cmsLog_debug("update filename %s message sent to felix", filename);
            pRequestData->requestStatus = OSGID_REQUEST_PROCESSING;
            *(int *)pEntry->key = (int)pRequestData->requestStatus;
            ret = CMSRET_SUCCESS;
            retval = 1;
         }
         else
         {
            cmsLog_debug("error sending update filename %s message sent to felix", filename);
            ret = CMSRET_INTERNAL_ERROR;
            /*fail to send request to felix, we have to retrieve DU state*/
            osgidRetrieveDUorEUState(pRequestData->operation);

         }
      }

      if(ret != CMSRET_SUCCESS)
      {
         goto Error;
      }

   }/*SW_MODULES_OPERATION_UPDATE*/
   else
   { 
      /*Should not arrive here.*/
      return 0;
   }

   return retval;

Error:
   pRequestData->FaultCode = (int)(ret);
   pRequestData->requestStatus = OSGID_REQUEST_DONE;
   *(int *)pEntry->key = (int)pRequestData->requestStatus;
   /*get next req data from processingList*/
   retval = osgidProcessProcessingList(operation);

   return retval;
}


/* go through the list and process all request */
/* this is called at when execEnv is up, or when a request is done */
void osgidProcessRequestList(void)
{
   pREQUEST_DATA pRequestData;
   pENTRY_TYPE pEntry;
   CmsRet ret = CMSRET_SUCCESS;

   /* TODO: time out mechanism should be added here.
    * if a request has been in the request list and pending for response
    * for 1 minute(?), mark it error, remove from processing list, and clear
    * requestListProcessing.
    */
   if (requestListProcessing == TRUE)
   {
      cmsLog_debug("requestListProcessing...");
      osgidDump();
      return;
   }
   pEntry = (pENTRY_TYPE)(removeFront(&requestList));
   if (pEntry == NULL)
   {
      return;
   }

   pRequestData = (pREQUEST_DATA)pEntry->data;

   /* if EXEC ENV is not UP, and it's a bootup case, queue the request.
    * if EXEC ENV is not UP, and not a bootup case, return EXEC disable error.
    * if busy, then just queue it and return OK 
    */
   if (osgiExecUp == FALSE)
   {
      if (strcasecmp(pRequestData->operation, SW_MODULES_OPERATION_INSTALL_AT_BOOTUP) != 0)
      {
         /* somehow, we need to update the data model to let application know
          * there is this error: (CMSRET_EE_DISABLED)
          * We need to update the MDM and send a message back to management app...
          */
         
         pRequestData->FaultCode = CMSRET_EE_DISABLED;
         osgidSendReplyToRequestApp(pRequestData);
         osgidFreeEntry(pEntry);
         return;
      }
      else
      {
         /* other wise, we just queue this back in the request list until execEnv is up */
         addFront(pEntry,&requestList);
         return;
      }
   } /* exec env is disabled */
   else
   {
      /* now, start processing this request */
      if ((strcasecmp(pRequestData->operation, SW_MODULES_OPERATION_INSTALL_AT_BOOTUP) == 0) ||
          (strcasecmp(pRequestData->operation, SW_MODULES_OPERATION_INSTALL) == 0))
      {
         cmsLog_debug("operation %s, uuid %s, url %s, username %s, password %s",
                      pRequestData->operation,pRequestData->UUID,pRequestData->URL,pRequestData->username,pRequestData->password);
         
         ret = osgidProcessInstallRequest(pRequestData);
      }
      else if (strcasecmp(pRequestData->operation, SW_MODULES_OPERATION_UNINSTALL) == 0)
      {
         cmsLog_debug("got CMS_MSG_DU_STATE_CHANGE: uninstall, UUID %s, version %s",
                      pRequestData->UUID,pRequestData->version);
         ret = osgidProcessUninstallRequest(pRequestData);
      }
      else if (strcasecmp(pRequestData->operation, SW_MODULES_OPERATION_UPDATE) == 0)
      {
         cmsLog_debug("got CMS_MSG_DU_STATE_CHANGE: update, url %s, UUID %s, username %s, password %s, version %s",
                      pRequestData->URL,pRequestData->UUID,pRequestData->username,pRequestData->password,
                      pRequestData->version);
         ret = osgidProcessUpdateRequest(pRequestData);

      }
      else if (strcasecmp(pRequestData->operation, SW_MODULES_OPERATION_START) == 0)
      {
         cmsLog_debug("got CMS_MSG_EU_STATE_CHANGE: start, EUID %s",
                      pRequestData->euid);
         
         ret = osgidProcessStartorStopRequest(TRUE,pRequestData->euid);
      }
      else if (strcasecmp(pRequestData->operation, SW_MODULES_OPERATION_STOP) == 0)
      {
         cmsLog_debug("got CMS_MSG_EU_STATE_CHANGE: stop, EUID %s, name %s",
                      pRequestData->euid);
         ret = osgidProcessStartorStopRequest(FALSE,pRequestData->euid);
      }
      else if (strcasecmp(pRequestData->operation, SW_MODULES_OPERATION_LB) == 0)
      {
         cmsLog_debug("got CMS_MSG_LB_REQUEST");
         ret = osgidProcessRequestBundleList();
      }
      else 
      {
         /* In fact, we shouldn't arrive at here, TR69c or Http shouldn't send an unsupported request type to osgid. */
         osgidFreeEntry(pEntry);
         ret = CMSRET_METHOD_NOT_SUPPORTED;
         return;
         /* this should never happen though */
      }

      if(ret == CMSRET_SUCCESS)
      {
         requestListProcessing = TRUE;
      }
      else
      {
         /*if get error at here, we should send reply to src App with FaultCode.
         Now just work on DU state change*/
         pRequestData->FaultCode = (int)ret;
         osgidSendReplyToRequestApp(pRequestData);
      }
      osgidFreeEntry(pEntry);
   } /* exec env is UP */
} /* osgidProcessRequestList */


void osgidProcessModulesAtBootup(void)
{
   CmsRet ret = CMSRET_SUCCESS;
   pREQUEST_DATA pRequestData;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   DUObject *duObject=NULL;
   CmsEntityId mngrEid;


   ret = cmsLck_acquireLockWithTimeout(OSGID_LOCK_TIMEOUT);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      cmsLck_dumpInfo();
      return;
   }


   /* the OSGI/JVM execution environment just came up.
    * Just go through the data model to look for Deployment Units that
    * belong to this Exec Env and re-install them.
    * Note new behavior in 4.14: the DU status is saved to config file,
    * which means that if the DU was installed, on reboot, it will still
    * be in the installed state.  Don't check the DU status, if there
    * is a DU entry, and it belongs to OSGID, try to install it.
    */
   while ((ret = cmsObj_getNext(MDMOID_DU, &iidStack, (void **)&duObject)) == CMSRET_SUCCESS)
   {
      if (qdmModsw_getMngrEidByExecEnvFullPathLocked(duObject->executionEnvRef,
                                     &mngrEid) == CMSRET_SUCCESS)
      {
         if (mngrEid == EID_OSGID)
         {
            cmsLog_debug("Bootup case: install DU UUID %s\n",duObject->UUID);
            pRequestData = (pREQUEST_DATA)malloc(sizeof(REQUEST_DATA));
            if (pRequestData == NULL)
            {
               cmsLog_error("failed to allocate REQUEST_DATA, skip DU");
            }
            else
            {
               memset(pRequestData, 0, sizeof(REQUEST_DATA));
               strcpy(pRequestData->operation,SW_MODULES_OPERATION_INSTALL_AT_BOOTUP);
               strcpy(pRequestData->URL,duObject->URL);
               strcpy(pRequestData->UUID,duObject->UUID);
               if(duObject->version != NULL)
                  strcpy(pRequestData->version,duObject->version);
               osgidAddRequestToList(pRequestData);
            }
         }
      }

      cmsObj_free((void **) &duObject);
   } /* while */

   cmsLck_releaseLock();

   return;
}

void osgidUpdateExistingEUinstance(pLB_DATA pData, char *euExecutionUnit)
{
   CmsRet ret = CMSRET_SUCCESS;
   EUObject *euObject;
   char *fullStr=NULL;
   char tmpEuFullPathStr[BUFLEN_512];
   MdmPathDescriptor pathDesc;
   int len;
   UBOOL8 isAutoStart=FALSE;

   cmsLog_debug("Enter: euExecutionUnit %s",euExecutionUnit);

   memset(&pathDesc, 0, sizeof(pathDesc));

   if ( (IS_EMPTY_STRING(euExecutionUnit)) || (cmsUtl_strcmp(euExecutionUnit,"(null)") == 0))
   {
      return;
   }
   fullStr = strdup(euExecutionUnit);
   if (fullStr == NULL)
   {
      return;
   }
   len = strlen(fullStr);
   if (fullStr[len-1] == ',')
   {
      /* remove , */
      fullStr[len-1] = '\0';
      len-=1;
   }
   if (fullStr[len-1] == '.')
   {
      ret = cmsMdm_fullPathToPathDescriptor(fullStr, &pathDesc);
      isAutoStart = modsw_isEuAutoStartLocked(fullStr);
   }
   else
   {
      /* add a dot at the end to indicate that the path is an object path */
      snprintf(tmpEuFullPathStr, sizeof(tmpEuFullPathStr), "%s.", fullStr);
      ret = cmsMdm_fullPathToPathDescriptor(tmpEuFullPathStr, &pathDesc);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("bad EU fullpath %s", tmpEuFullPathStr);
      }
      isAutoStart = modsw_isEuAutoStartLocked(tmpEuFullPathStr);
   }
   free(fullStr);


   if ((ret = cmsObj_get(pathDesc.oid, &(pathDesc.iidStack), 0, (void **) &euObject)) == CMSRET_SUCCESS)
   {
      cmsLog_debug("update EUID=%s bundleId=%d", euObject->EUID, pData->bundleId);
      euObject->X_BROADCOM_COM_bundleId = pData->bundleId;
      if (isAutoStart)
      {
         cmsLog_debug("%s isAutostart, requesting active state", euObject->EUID);
         CMSMEM_REPLACE_STRING(euObject->requestedState, MDMVS_ACTIVE);
      }
      if ((ret = cmsObj_set(euObject, &(pathDesc.iidStack))) != CMSRET_SUCCESS)
      {
         cmsLog_error("set of EU entry failed, ret=%d, eu->description %s", ret, euObject->description);
      }
      cmsObj_free((void **) &euObject);
   }

}

CmsRet osgidProcessRequestBundleList(void)
{
   CmsRet ret;
   char buf[BUFLEN_256];

   strcpy(buf,SW_MODULES_OPERATION_LB);
   if (sendto_felix_listen_server(buf,strlen(buf)))
   {
      cmsLog_debug("LB command sent to felix");
      ret = CMSRET_SUCCESS;
   }
   else
   {
      cmsLog_debug("error sending LB command sent to felix");
      ret = CMSRET_INTERNAL_ERROR;
   }
   return (ret);
}

void osgidProcessFelixLBResponse(char *buf, int len)
{
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack idStack = EMPTY_INSTANCE_ID_STACK;
   DUObject *duObject;
   EUObject *euObject;
   UBOOL8 found = FALSE;
   UBOOL8 saveToConfig = FALSE;
   pENTRY_TYPE pEntry;
   LIST_TYPE bundleList= {NULL,NULL};
   pLB_DATA pData;

   /*
    *install:Bundle ID:5:LB:START LEVEL:1:
    *0|Starting|0|System Bundle (3.1.0.SNAPSHOT):
    *1|Active|1|Apache Felix Bundle Repository (1.6.2):
    *2|Active|1|Apache Felix Gogo Command (0.7.0.SNAPSHOT):
    *3|Active|1|Apache Felix Gogo Runtime (0.7.0.SNAPSHOT):
    *4|Starting|1|Apache Felix Gogo USockListener (1.0.0.SNAPSHOT):
    *5|Installed|1|HelloWorld Plug-in (1.0.0)
    * OR
    *    *LB:START LEVEL:1:...
    */   
   /* parse LB to get several things: mainBundleId, bundleId, status, startLevel, description str */

   cmsLog_debug("enter");

   printf("\nOSGI: Current Bundle List:\n");
   printf("%s\n",buf);

   /* first parse the response */
   ret = osgidParseLBResponseFromFelix(buf, &bundleList);
   if (ret != CMSRET_SUCCESS)
   {
      /* if it's not a LB response, we just ignore it and wait for the next message */
      return;
   }
   if ((ret = cmsLck_acquireLockWithTimeout(OSGID_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d, drop message", ret);
      osgidFreeList(&bundleList);
      return; /* should return an error code TO-DO */
   }

   /* the very first entry in the bundle list is the DU, subsequent ones are EUs */
   pEntry = (pENTRY_TYPE)(removeFront(&bundleList));

   while (pEntry != NULL)
   {
      pData = (pLB_DATA)pEntry->data;
      INIT_INSTANCE_ID_STACK(&idStack);
      found = FALSE;
      if (osgidIsDuType(pData->status) == TRUE)
      {
         /* look for the DU, and update it's status */
         while (!found &&
                (ret = cmsObj_getNext(MDMOID_DU, &idStack, (void **)&duObject)) == CMSRET_SUCCESS)
         {
            if (duObject->X_BROADCOM_COM_bundleId == pData->bundleId)
            {
               found = TRUE;
               break;
            }
            else
            {
               cmsObj_free((void **) &duObject);
               continue;
            }
         } /* while DU */

         if (found) 
         {   

            if (strcmp(duObject->status,pData->status) != 0)
            {
               CMSMEM_REPLACE_STRING(duObject->status,pData->status);
               
               /* set DU status */
               if ((ret = cmsObj_set(duObject, &idStack)) != CMSRET_SUCCESS)
               {
                  cmsLog_error("cannot change state of DU ret=%d, status %d", ret, pData->status);
               }
               else 
               {
                  saveToConfig = TRUE;
               }
               cmsObj_free((void **) &duObject);   
            }
         } /* found DU */
      } /* DU status */
      /* a DU status can also be an EU if the DU is also an EU */
      found = 0;
      INIT_INSTANCE_ID_STACK(&idStack);
      while (!found &&
             (ret = cmsObj_getNext(MDMOID_EU, &idStack, (void **)&euObject)) == CMSRET_SUCCESS)
      {
         if ((cmsUtl_strcmp(euObject->alias, pData->alias) == 0) ||
             (cmsUtl_strcmp(euObject->description, pData->description) == 0))
         {
            found = TRUE;
            break;
         }
         else
         {
            cmsObj_free((void **) &euObject);
            continue;
         }
      } /* while of EU*/
      if (found) 
      {      
         cmsLog_debug("found EU desc=%s bundleId=%d status=%s pData->bundleId=%d pData->status=%s",
                      euObject->description, euObject->X_BROADCOM_COM_bundleId,
                      euObject->status,
                      pData->bundleId, pData->status);
         if (strcmp(euObject->status,pData->status) != 0)
         {
            if ((strcmp(pData->status, "resolved") == 0) ||
                (strcmp(pData->status, "Installed") == 0))
            {
               CMSMEM_REPLACE_STRING(euObject->status,MDMVS_IDLE);
            }
            else
            {
               CMSMEM_REPLACE_STRING(euObject->status,pData->status);
            }

            cmsLog_debug("setting EU %s status %s", euObject->description, euObject->status);
            if ((ret = cmsObj_set(euObject, &idStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("cannot change status of EU to %s ret=%d",
                             euObject->status, ret);
            }
            else
            {
               saveToConfig = TRUE;
            }
            cmsObj_free((void **) &euObject);
         } /* change in status */
      } /* try EU */

      osgidFreeEntry(pEntry);
      pEntry = (pENTRY_TYPE)(removeFront(&bundleList));
   } /* pEntry of LB bundles command */
   osgidFreeList(&bundleList);   

   if (saveToConfig == TRUE)
   {
      cmsMgm_saveConfigToFlash();
   }
   cmsLck_releaseLock();
}

UBOOL8 osgidIsDuType(char *statusStr)
{
   /* EU status: Idle, starting, active and stopping.
    * DU status: Installing, installed, updating, uninstalling and uninstalled
    */
   if ( (cmsUtl_strcmp(statusStr, MDMVS_IDLE) == 0) ||
        (cmsUtl_strcmp(statusStr, MDMVS_STARTING) == 0) ||
        (cmsUtl_strcmp(statusStr, MDMVS_STOPPING) == 0) ||
        (cmsUtl_strcmp(statusStr, MDMVS_ACTIVE) == 0) )
   {
      return FALSE;
   }
   return TRUE;
}

CmsRet osgidAddLBRequestToList(void)
{

   CmsRet ret=CMSRET_SUCCESS;
   pREQUEST_DATA pRequestData;

   /* queue this up for processing */
   /* upon finishing the request--when status is sent back from felix, requestData is freed */
   pRequestData = malloc(sizeof(REQUEST_DATA));   
   if (pRequestData == NULL)
   {
      ret = CMSRET_RESOURCE_EXCEEDED;
   }
   else
   {
      memset(pRequestData, 0, sizeof(REQUEST_DATA));
      strcpy(pRequestData->operation,SW_MODULES_OPERATION_LB);
      ret = osgidAddRequestToList(pRequestData);
   }
   return (ret);
}

void osgidDumpList(LIST_TYPE *list)
{
   pENTRY_TYPE pEntry;
   pREQUEST_DATA pRequestData;

   pEntry = list->head;

   if (pEntry == NULL)
   {
      printf("List is empty\n");
   }

   while (pEntry !=NULL)
   {
      pRequestData = pEntry->data;
      if (pRequestData != NULL)
      {
         printf("operation %s",pRequestData->operation);
         if (((strcmp(pRequestData->operation,SW_MODULES_OPERATION_LB) != 0) &&
              (strcmp(pRequestData->operation,SW_MODULES_OPERATION_UNINSTALL) != 0)) && 
             (pRequestData->URL != NULL))
         {
            printf(" URL %s\n",pRequestData->URL);
         }
         else
         {
            printf("\n");
         }
      } /* pRequestData != NULL*/
      pRequestData = NULL;
      pEntry = pEntry->next;
   } /* while */
   printf("\n");
}
void osgidDump(void)
{
    if(cmsLog_getLevel() < LOG_LEVEL_DEBUG)
        return;
   printf("\n--------Request List----------\n");
   osgidDumpList(&requestList);
   printf("\n--------Processing List----------\n");
   osgidDumpList(&processingList);
   printf("osgiExecUp %d\n",osgiExecUp);
   printf("requestListProcessing %d\n",requestListProcessing);
}

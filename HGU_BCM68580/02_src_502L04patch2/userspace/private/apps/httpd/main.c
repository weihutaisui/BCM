/***********************************************************************
 *
 *  Copyright (c) 2006  Broadcom Corporation
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

#include "cms.h"
#include "cms_eid.h"
#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"
#include "httpd.h"
#ifdef DMP_X_ITU_ORG_GPON_1 /* aka SUPPORT_OMCI */
#include "omciutl_cmn.h"
#ifdef BUILD_BCMIPC
#include "omci_ipc.h"
#endif /* BUILD_BCMIPC */
#endif /* DMP_X_ITU_ORG_GPON_1 */


UINT32 exitOnIdleTimeout=HTTPD_EXIT_ON_IDLE_TIMEOUT;
SINT32 serverPort=HTTP_SERVER_PORT;
void *msgHandle=NULL;
UBOOL8 glbSaveConfigNeeded=FALSE;

#ifdef SUPPORT_QUICKSETUP
UBOOL8 glbQuickSetupEnabled=FALSE;
#endif

void usage(char *progName)
{
   printf("usage: %s [-o] [-p port_num] [-m shmid] [-v num] [-x secs]\n", progName);
   printf("       o: open httpd server socket, normally server socket is inheritted from smd\n");
   printf("       p: TCP port number for httpd to listen on\n");
   printf("       m: shared memory id, -1 if standalone or not using shared mem.\n");
   printf("       v: set verbosity, where num==0 is LOG_ERROR, 1 is LOG_NOTICE, all others is LOG_DEBUG\n");
   printf("       x: set exit on idle timeout, in seconds.\n");

}


static void initLoggingFromConfig(UBOOL8 useConfiguredLogLevel)
{
   HttpdCfgObject *obj;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   if ((ret = cmsLck_acquireLockWithTimeout(HTTPD_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      cmsLck_dumpInfo();
      return;
   }

   if ((ret = cmsObj_get(MDMOID_HTTPD_CFG, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of HTTPD_CFG object failed, ret=%d", ret);
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

#ifdef SUPPORT_QUICKSETUP
static void initQuickSetupStatus(void)
{
   HttpdCfgObject *httpdObj=NULL;
   WanDevObject *wanDev=NULL;
   WanEthIntfObject *ethWanObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack1 = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;
   UBOOL8 isWanConfigured = FALSE;


   if ((ret = cmsLck_acquireLockWithTimeout(HTTPD_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      return;
   }

   if ((ret = cmsObj_get(MDMOID_HTTPD_CFG, &iidStack, 0, (void **) &httpdObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of HTTPD_CFG object failed, ret=%d", ret);
   }
   else
   {
      if(httpdObj->quickSetupEnabled == TRUE)
      {
         /* we have to enable quick setup only for the first time when no wan interfaces are
          * configured.To handle the case of just upgrading the software but keeping the existing
          * configuration we need to disable quicksetup when a wan interface exists
          */

         while((!isWanConfigured) &&
               (CMSRET_SUCCESS == cmsObj_getNext(MDMOID_WAN_DEV, &iidStack1, (void **)&wanDev)))
         {
            if(wanDev->WANConnectionNumberOfEntries >0)
            {
               isWanConfigured = TRUE;
            } 
            else
            {
               if ((ret = cmsObj_get(MDMOID_WAN_ETH_INTF, &iidStack1, 0, (void **) &ethWanObj)) != CMSRET_SUCCESS)
               {
                  cmsLog_error("get of MDMOID_WAN_ETH_INTF object failed, ret=%d", ret);
               }
               else
               {
                  /*check for ethernet wan */
                  if(ethWanObj->X_BROADCOM_COM_IfName != NULL)
                  {
                     isWanConfigured = TRUE;
                  } 

                  cmsObj_free((void **) &ethWanObj);
               }
            }

            cmsObj_free((void **) &wanDev);
         }/*while*/

         if(isWanConfigured == TRUE)
         {

            httpdObj->quickSetupEnabled = FALSE;

            if(cmsObj_set(httpdObj, &iidStack) !=CMSRET_SUCCESS)
            {
               cmsLog_error("set of HTTPD_CFG object failed, ret=%d", ret);
            }

         }

      }
      glbQuickSetupEnabled = httpdObj->quickSetupEnabled;
      cmsObj_free((void **) &httpdObj);
   }

   cmsLck_releaseLock();
}
#endif

SINT32 main(SINT32 argc, char *argv[])
{
   SINT32 c, logLevelNum;
   SINT32 shmId=UNINITIALIZED_SHM_ID;
   CmsRet ret;
   CmsLogLevel logLevel=DEFAULT_LOG_LEVEL;
   UBOOL8 useConfiguredLogLevel=TRUE;
   UBOOL8 openServerSocket=FALSE;
#ifdef BUILD_WLAN
   WLCSM_SET_TRACE("httpd");
#endif
   cmsLog_initWithName(EID_HTTPD, argv[0]);


   while ((c = getopt(argc, argv, "ov:p:m:x:")) != -1)
   {
      switch(c)
      {
      case 'o':
         openServerSocket = TRUE;
         break;

      case 'p':
         serverPort = atoi(optarg);
         break;

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

      case 'x':
         exitOnIdleTimeout = atoi(optarg);
         break;

      default:
         cmsLog_error("bad arguments, exit");
         usage(argv[0]);
         exit(-1);
      }
   }


   cmsLog_debug("using http serverPort=%d exitOnIdleTimeout=%d", serverPort, exitOnIdleTimeout);
   cmsLog_debug("using shmId=%d", shmId);

   if ((ret = cmsMsg_initWithFlags(EID_HTTPD, 0, &msgHandle)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMsg_init failed, ret=%d", ret);
      cmsLog_cleanup();
      exit(-1);
   }

   if ((ret = cmsMdm_initWithAcc(EID_HTTPD, NDA_ACCESS_HTTPD, msgHandle, &shmId)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMdm_init failed, ret=%d", ret);
      cmsMsg_cleanup(&msgHandle);
      cmsLog_cleanup();
      exit(-1);
   }

   initLoggingFromConfig(useConfiguredLogLevel);
#ifdef SUPPORT_QUICKSETUP
   initQuickSetupStatus();
#endif

#ifdef DMP_X_ITU_ORG_GPON_1 /* aka SUPPORT_OMCI */
#ifdef BRCM_OMCI
   omciUtl_initCrc32Table();
#ifdef BUILD_BCMIPC
   omciIpc_clientInit(HTTPD_CLIENT);
#endif // BUILD_BCMIPC
#endif // BRCM_OMCI
#endif // DMP_X_ITU_ORG_GPON_1

   /*
    * Here is where the real httpd does its work.
    */
   web_main(openServerSocket);

   cmsMdm_cleanup();
   cmsMsg_cleanup(&msgHandle);
   cmsLog_cleanup();
#ifdef BUILD_BCMIPC
   omciIpc_clientShutdown();
#endif

   return 0;
}


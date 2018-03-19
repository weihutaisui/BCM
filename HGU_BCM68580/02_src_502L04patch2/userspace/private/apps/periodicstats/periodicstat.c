/***********************************************************************
 *
 *  Copyright (c) 2013  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2013:proprietary:standard

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
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include "cms.h"
#include "cms_core.h"
#include "cms_qdm.h"
#include "cms_msg.h"
#include "cms_tmr.h"
#include "cms_tms.h"

#include "periodicstat.h"
#include "sampleset.h"
#include "parameter.h"


//#define PERIODICSTATS_DEBUG 1
#ifdef PERIODICSTATS_DEBUG
static void dumpResults(void);
#endif
pthread_mutex_t glbMutex;

void *msgHandle=NULL;
void *tmrHandle=NULL;

static SampleSetInfo *sampleList = NULL;

static SampleSetInfo * findSampleSet(SampleSetInfo *smpInfo)
{
   SampleSetInfo *info;
   for ( info = sampleList ; info != NULL ; info = info->next )
   {
      if ( info->id == smpInfo->id )
         return info;
   }
   return NULL;
}

static SampleParameterInfo * findParameterInSampleSet(SampleSetInfo *smpset, SampleParameterInfo *paramInfo)
{
   SampleParameterInfo *param = NULL;
   for ( param = smpset->parameters ; param != NULL ; param = param->next )
   {
      if ( param->id == paramInfo->id )
         return param;
   }
   return NULL;
}

static void fetchParameter(SampleParameterInfo *param, SampleSetInfo *smpset)
{
   CmsRet result;
   MdmPathDescriptor path;
   int valueCount;
   time_t currTime;
#ifdef SUPPORT_PERIODICSTATSADV
   int fail = FALSE;
#endif
  
   if ( param == NULL )
      return;

   valueCount = param->valueCount;
   param->valueCount ++;
   cmsLog_debug("fetch %s", param->name);
   currTime = time(NULL);
   result = cmsMdm_fullPathToPathDescriptor(param->name, &path);
   if ( result == CMSRET_SUCCESS )
   {
      PhlGetParamValue_t *pParamValue = NULL;
      if (path.paramName[0] != 0)
      {
         result = cmsPhl_getParamValue(&path, &pParamValue);
         if ( result != CMSRET_SUCCESS)
         {
            cmsLog_notice("get value of parameter (%s) failed!", param->name);
         }
         else
         {
            cmsLog_debug("get parameter value : %s(type:%s)",pParamValue->pValue, pParamValue->pParamType);
#ifdef SUPPORT_PERIODICSTATSADV
            if (cmsUtl_strcmp(pParamValue->pParamType,"unsignedInt") == 0)
            {
               UINT32 currValue = atol(pParamValue->pValue);
               if ( param->sampleMode == SAMPLEMODE_CURRENT )
               {
                  param->values[valueCount] = strdup(pParamValue->pValue);
                  if ((param->lowThreshold != param->highThreshold) && (currValue < param->lowThreshold || currValue > param->highThreshold))
                     fail = TRUE;
               }
               else if (param->sampleMode == SAMPLEMODE_CHANGE)
               {
                  char strValue[BUFLEN_16];
                  long diffValue = currValue - param->previousValue;
                  sprintf(strValue, "%ld", diffValue);
                  param->values[valueCount] = strdup(strValue);
                  param->previousValue = currValue; 
                  if ((param->lowThreshold != param->highThreshold) && (diffValue < param->lowThreshold || diffValue > param->highThreshold))
                     fail = TRUE;
               }
               else
                  cmsLog_error("Unknow Sample Mode detected!");
            }
            else
            {
               param->values[valueCount] = strdup(pParamValue->pValue);
            }
#else
            param->values[valueCount] = strdup(pParamValue->pValue);
#endif
            // TBD -- SuspectData have to remember the value's status of each parameter.
            param->suspectData[valueCount] = 0;
            param->sampleSeconds[valueCount] = currTime - param->previousTime;
            cmsPhl_freeGetParamValueBuf(pParamValue, 1);
         }
      }
   }
   else
   {
      cmsLog_notice("Could not find parameter (%s)", param->name);
   }
   param->previousTime = currTime;


#ifdef SUPPORT_PERIODICSTATSADV 
   if (fail == TRUE)
      param->failure ++;
#endif /* SUPPORT_PERIODICSTATSADV */
   return;
}

static void sampleSetHandler(void *ctx)
{
   SampleSetInfo *smpset = (SampleSetInfo *)ctx;
   SampleParameterInfo *param;
   time_t currTime;

   pthread_mutex_lock(&glbMutex);
   if (smpset->sampleCount >= smpset->reportSamples)
   {
      // clear statistics.
      smpset->sampleCount = 0;
      memset(smpset->sampleSeconds, 0, sizeof(UINT32)*(smpset->reportSamples));
      memset(smpset->endTime, 0, BUFLEN_64);
      for ( param = smpset->parameters ; param != NULL ; param = param->next )
      {
         resetParameter(param, smpset);
      }
      cmsTms_getXSIDateTime(smpset->previousTime, smpset->startTime, BUFLEN_64);
   }
   
   /* restart a timer in sampleInterval without any delay */
   cmsTmr_set(tmrHandle, sampleSetHandler, smpset, smpset->sampleInterval*MSECS_IN_SEC, "");

   /* Fetch parameters listed in thie sample set */
   for ( param = smpset->parameters ; param != NULL ; param = param->next )
   {
      fetchParameter(param, smpset);
   }

   currTime = time(NULL);
   cmsTms_getXSIDateTime(0, smpset->endTime, BUFLEN_64);
   smpset->sampleSeconds[smpset->sampleCount++] = currTime - smpset->previousTime;
   sampleOutput(smpset);

#ifdef PERIODICSTATS_DEBUG
   dumpResults();
#endif

   smpset->previousTime = currTime;

   /** SampleSet stored in CPE is reach the ReportSamples
    *  send a message to inform ACS server.
    *  fetchSamples should always less or equal to reportSamples
    */  
#ifdef SUPPORT_PERIODICSTATSADV 
   if ( smpset->fetchSamples != 0 && smpset->sampleCount >= smpset->fetchSamples )
   {
      CmsMsgHeader *msg = cmsMem_alloc(sizeof(CmsMsgHeader), ALLOC_ZEROIZE);
      if (msg != NULL)
      {
         msg->src = EID_PERIODICSTAT;
         msg->dst = EID_SSK;
         msg->type = CMS_MSG_PERIODICSTAT_STATUS_CHANGE;
         msg->wordData = smpset->id;

         cmsMsg_send(msgHandle, msg);
         CMSMEM_FREE_BUF_AND_NULL_PTR(msg);
      }
      else
      {
         cmsLog_error("message allocation failed");
      }
   }
#else
   if (smpset->sampleCount >= smpset->reportSamples)
   {
      CmsMsgHeader *msg = cmsMem_alloc(sizeof(CmsMsgHeader), ALLOC_ZEROIZE);
      if (msg != NULL)
      {
         msg->src = EID_PERIODICSTAT;
         msg->dst = EID_SSK;
         msg->type = CMS_MSG_PERIODICSTAT_STATUS_CHANGE;
         msg->wordData = smpset->id;

         cmsMsg_send(msgHandle, msg);
         CMSMEM_FREE_BUF_AND_NULL_PTR(msg);
      }
      else
      {
         cmsLog_error("message allocation failed");
      }
   }
#endif /* SUPPORT_PERIODICSTATSADV */

   pthread_mutex_unlock(&glbMutex);
} 

static void periodicstatCleanup(void)
{
   while ( sampleList != NULL )
   {
      SampleSetInfo *temp = sampleList;
      sampleList = temp->next;
      destroySampleSet(temp);
   }
}

#ifdef PERIODICSTATS_DEBUG
static void dumpResults(void)
{
   SampleSetInfo *tempSampleSet;
   SampleParameterInfo *tempParam;
   for ( tempSampleSet = sampleList ; tempSampleSet != NULL ; tempSampleSet = tempSampleSet->next )
   {
      printf("[SampleSet %p] (%d)(%d)------------\n", tempSampleSet, tempSampleSet->reportSamples, tempSampleSet->sampleCount);
      for ( tempParam = tempSampleSet->parameters ; tempParam != NULL ; tempParam = tempParam->next )
         printf("\t[Param %p]%d[v:%p][s:%p][d:%p]\n", tempParam, tempParam->valueCount,
               tempParam->values, tempParam->sampleSeconds, tempParam->suspectData);
   }
}
#endif

static void *cmsMsgHandler(void* arg)
{
   CmsRet ret;
   CmsMsgHeader *msg=NULL;
   SINT32 commFd;
   SINT32 n, maxFd;
   fd_set readFdsMaster, readFds;
   struct timeval tv;

   FD_ZERO(&readFdsMaster);
   cmsMsg_getEventHandle(msgHandle, &commFd);
   FD_SET(commFd, &readFdsMaster);
   maxFd = commFd;

   while(1)
   {
      readFds = readFdsMaster;
      int forceFetchNow = FALSE;
      SampleSetInfo *fetchSampleSet;

      tv.tv_sec = PERIODIC_STATS_CHECK_INTERVAL;
      tv.tv_usec = 0;

      n = select(maxFd+1, &readFds, NULL, NULL, &tv);
      
      if ( n < 0 )
      {
         cmsLog_error("error on select, errno=%d", errno);
         usleep(100);
         continue;
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
                  break;
               }
               else
               {
                  cmsLog_error("detected exit of smd, ssk will also exit");
                  /* unexpectedly lost connection to smd, maybe I should
                   * reboot the system here to recover.... */
                  break;
               }
            }
            else
            {
               /* try to keep running after a 100ms pause */
               cmsLog_error("error during cmsMsg_receive, ret=%d", ret);
               usleep(100);
               continue;
            }
         }

         cmsLog_debug("receive cms message type 0x%x from %d (flags=0x%x)", msg->type, msg->src, msg->flags);
         pthread_mutex_lock(&glbMutex);

         switch(msg->type)
         {
         case CMS_MSG_PERIODICSTAT_START_SAMPLESET:
            {
               SampleSetInfo *smpsetInfo = NULL;
               SampleSetInfo *smpset = (SampleSetInfo *)(msg + 1);
               smpsetInfo = findSampleSet(smpset);

               if ( smpsetInfo != NULL )
               {
                  cmsTmr_cancel(tmrHandle, sampleSetHandler, smpsetInfo);
                  updateSampleSet(smpsetInfo, smpset);
                  cmsLog_debug("restart smpset(%d):interval:%u, reportSamples:%u\n", smpsetInfo->id, smpsetInfo->sampleInterval, smpsetInfo->reportSamples);
               }
               else 
               {
                  smpsetInfo = (SampleSetInfo *)malloc(sizeof(SampleSetInfo));
                  if ( smpsetInfo == NULL )
                  {
                     cmsLog_error("Unable to create a new Sample Set!");
                     ret = CMSRET_RESOURCE_EXCEEDED;
                     break;
                  }
                  memcpy(smpsetInfo, smpset, sizeof(SampleSetInfo));
                  cmsLog_debug("new smpset(%d):interval:%u, reportSamples:%u\n", smpsetInfo->id, smpsetInfo->sampleInterval, smpsetInfo->reportSamples);
                  smpsetInfo->sampleSeconds = (UINT32 *)malloc(sizeof(UINT32)*smpset->reportSamples);
                  smpsetInfo->parameters = NULL;
                  /* Insert samplsetinfo into the sampleList */
                  smpsetInfo->next = sampleList;
                  sampleList = smpsetInfo;
               }

               cmsTms_getXSIDateTime(0, smpsetInfo->startTime, BUFLEN_64);
               smpsetInfo->previousTime = time(NULL);
               ret = cmsTmr_set(tmrHandle, sampleSetHandler, smpsetInfo, smpsetInfo->sampleInterval*MSECS_IN_SEC, "");
            }
            break;

         case CMS_MSG_PERIODICSTAT_STOP_SAMPLESET:
            {
               SampleSetInfo *smpsetInfo = NULL;
               SampleSetInfo *smpset = (SampleSetInfo *)(msg + 1);
               smpsetInfo = findSampleSet(smpset);
               if ( smpsetInfo != NULL )
               {
                  SampleSetInfo *temp;
                  cmsTmr_cancel(tmrHandle, sampleSetHandler, smpsetInfo);
                  /* Delete samplesetinfo from the sampleList */
                  if ( smpsetInfo == sampleList )
                     sampleList = smpsetInfo->next;
                  for ( temp = sampleList ; temp != NULL ; temp = temp->next)
                     if ( temp->next == smpsetInfo )
                        temp->next = smpsetInfo->next;
                  destroySampleSet(smpsetInfo);
               }
            }
            break;

         case CMS_MSG_PERIODICSTAT_UPDATE_SAMPLESET:
            {
               SampleSetInfo *smpsetInfo = NULL;
               SampleSetInfo *smpset = (SampleSetInfo *)(msg + 1);
               smpsetInfo = findSampleSet(smpset);
               if ( smpsetInfo == NULL )
               {
                  cmsLog_error("Update SampleSet fail. SampleSet Info does not exist");
                  ret = CMSRET_INVALID_ARGUMENTS;
               }
               else
               {
                  cmsTmr_cancel(tmrHandle, sampleSetHandler, smpsetInfo);
                  updateSampleSet(smpsetInfo, smpset);
                  cmsLog_debug("restart smpset(%s):interval:%u, reportSamples:%u\n", smpsetInfo->name, smpsetInfo->sampleInterval, smpsetInfo->reportSamples);
                  ret = cmsTmr_set(tmrHandle, sampleSetHandler, smpsetInfo, smpsetInfo->sampleInterval*MSECS_IN_SEC, "");
               }
            }

#ifdef SUPPORT_PERIODICSTATSADV 
         case CMS_MSG_PERIODICSTAT_PAUSE_SAMPLESET:
            break;

         case CMS_MSG_PERIODICSTAT_FORCE_SAMPLE:
            /** Find the SampleSetInfo and reply message to release lock in RCL layer.
             * and then execute fetch sample function to avoid dead lock.*/
            {
               SampleSetInfo *smpset = (SampleSetInfo *)(msg + 1);
               fetchSampleSet = findSampleSet(smpset);
               if ( fetchSampleSet != NULL ) 
               {
                  forceFetchNow = TRUE;
                  ret = CMSRET_SUCCESS;
               }
               else
                  ret = CMSRET_INVALID_PARAM_VALUE;
            }
            break;

#endif /* SUPPORT_PERIODICSTATSADV */

         case CMS_MSG_PERIODICSTAT_ADD_PARAMETER:
            {
               SampleSetInfo *smpsetInfo = NULL;
               SampleParameterInfo *parameterInfo = NULL;
               SampleSetInfo *smpset = (SampleSetInfo *)(msg + 1);
               SampleParameterInfo *param = (SampleParameterInfo *)(smpset + 1);
               smpsetInfo = findSampleSet(smpset);
               if ( smpsetInfo == NULL )
               {
                  cmsLog_error("Could not find related Sample Set");
                  ret = CMSRET_INVALID_ARGUMENTS;
                  break;
               }
               parameterInfo = (SampleParameterInfo *)malloc(sizeof(SampleParameterInfo));
               if ( parameterInfo != NULL )
               {
                  memcpy(parameterInfo, param, sizeof(SampleParameterInfo));
                  insertParameterIntoSampleSet(smpsetInfo, parameterInfo);
               }
               else
               {
                  cmsLog_error("Unable to insert a parameter into SampleSet.");
                  ret = CMSRET_RESOURCE_EXCEEDED;
               }
            }
            break;

         case CMS_MSG_PERIODICSTAT_DELETE_PARAMETER:
            {
               SampleSetInfo *smpsetInfo = NULL;
               SampleParameterInfo *parameterInfo = NULL;
               SampleSetInfo *smpset = (SampleSetInfo *)(msg + 1);
               SampleParameterInfo *param = (SampleParameterInfo *)(smpset + 1);
               smpsetInfo = findSampleSet(smpset);

               if ( smpsetInfo == NULL )
                  break;

               parameterInfo = findParameterInSampleSet(smpsetInfo, param);
               if ( parameterInfo != NULL )
                  deleteParameterFromSampleSet(smpsetInfo, parameterInfo);
            }
            break;

         default:
            cmsLog_error("cannot handle msg type 0x%x from %d (flags=0x%x)",
                         msg->type, msg->src, msg->flags);
            break;
         }

         pthread_mutex_unlock(&glbMutex);
         cmsMsg_sendReply(msgHandle, msg, ret);
         CMSMEM_FREE_BUF_AND_NULL_PTR(msg);
      }  /* end of if (FD_ISSET(commFd, &readFds)) */

      if (forceFetchNow)
      {
         if ((ret = cmsLck_acquireLockWithTimeout(PERIODIC_STATS_CHECK_INTERVAL*MSECS_IN_SEC)) != CMSRET_SUCCESS)
         {
            cmsLog_debug("failed to get lock, ret=%d", ret);
         }
         else
         {
            cmsTmr_cancel(tmrHandle, sampleSetHandler, fetchSampleSet);
            sampleSetHandler((void*)fetchSampleSet);
            forceFetchNow = FALSE;
            fetchSampleSet = NULL;
            cmsLck_releaseLock();
         }
      }

   }

   pthread_exit(0);
}

static int periodicstatLoop(void)
{
   CmsRet ret;
   UINT32 nextTimeout;
   while(1)
   {
      
      /* process the periodic statistics */
      if ((ret = cmsLck_acquireLockWithTimeout(PERIODIC_STATS_CHECK_INTERVAL*MSECS_IN_SEC)) != CMSRET_SUCCESS)
      {
         cmsLog_debug("failed to get lock, ret=%d", ret);
         sleep(PERIODIC_STATS_CHECK_INTERVAL);
      }
      else
      {
         cmsTmr_executeExpiredEvents(tmrHandle);
         cmsLck_releaseLock();
         ret = cmsTmr_getTimeToNextEvent(tmrHandle, &nextTimeout);
         if (ret != CMSRET_SUCCESS)
            sleep(PERIODIC_STATS_CHECK_INTERVAL);
         else
            usleep(nextTimeout * USECS_IN_MSEC);
      }

   }
   return 0;
}

int main(int argc, char *argv[])
{
   pthread_t msgThread;
   SINT32 shmId=0;
   int ret;

   cmsLog_init(EID_PERIODICSTAT);
   cmsLog_notice("initializing timers");
   if ((ret = cmsTmr_init(&tmrHandle)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsTmr_init failed, ret=%d", ret);
      return -1;
   }

   cmsLog_notice("calling cmsMsg_init");
   if ((ret = cmsMsg_initWithFlags(EID_PERIODICSTAT, 0, &msgHandle)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsMsg_init failed, ret=%d", ret);
      return 0;
   }

   if ((ret = cmsMdm_initWithAcc(EID_PERIODICSTAT, 0, msgHandle, &shmId)) != CMSRET_SUCCESS)
   {
      cmsMsg_cleanup(&msgHandle);
      cmsLog_error("cmsMdm_init error ret=%d", ret);
      return 0;
   }

   // Create a folder to hold statistics
   mkdir(PERIODIC_STATS_DIRECTORY, S_IRWXU | S_IRWXG | S_IRWXO);
   cmsLog_notice("periodic statistic start ......\n");

   pthread_mutex_init(&glbMutex, NULL);
   ret = pthread_create(&msgThread, NULL, cmsMsgHandler, NULL);
   if (ret)
   {
      cmsLog_error("ERROR! create thread failed, ret=%d", ret);
      goto p_exit;
   }

   ret = periodicstatLoop();

   periodicstatCleanup();

   pthread_exit(NULL);
p_exit:
   pthread_mutex_destroy(&glbMutex);

   cmsMdm_cleanup();
   cmsMsg_cleanup(&msgHandle);
   cmsTmr_cleanup(&tmrHandle);
   cmsLog_cleanup();
   return ret;
}

/*
* <:copyright-BRCM:2014:proprietary:standard
* 
*    Copyright (c) 2014 Broadcom 
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
#ifdef DMP_X_BROADCOM_COM_SELT_1

#include "cms.h"
#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"
#include "ssk.h"
#include "adslctlapi.h"
#include "devctl_adsl.h"
#include <time.h>

/*!\file ssk_selt.c
 * \brief This file contains functions specifically for DSL SELT test
 *
 */

extern UBOOL8 dslDiagInProgress;

#ifdef DMP_X_BROADCOM_COM_SELT_1
extern dslDiag dslDiagInfo;
#endif

void processWatchDslSeltDiag(CmsMsgHeader *msg)
{
   SeltCfgObject *dslDiagObj;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   /* I do not care about the body, no info is needed */
   /*   dslDiagMsgBody *info = (dslDiagMsgBody*) (msg+1); */
   CmsRet ret;

   if ((ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) == CMSRET_SUCCESS)
   {
      if (cmsObj_get(MDMOID_SELT_CFG, &iidStack, 0, (void **) &dslDiagObj) == CMSRET_SUCCESS)
      {
         CMSMEM_REPLACE_STRING(dslDiagObj->seltTestState,MDMVS_REQUESTED);
         if ((ret = cmsObj_set(dslDiagObj, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsObj_set returns error. ret=%d", ret);
         }
         else
         {
            /* global flag for main loop */
            dslDiagInProgress = TRUE;
            dslDiagInfo.dslDiagInProgress = TRUE;
            /* need to make this more generic in case we have more dsl diag to add in the future */
            /*            dslDiagInfo.testType = info->type; */
            dslDiagInfo.pollRetries = 0;
            dslDiagInfo.src = msg->src;
            dslDiagInfo.testStartTime = time(NULL);
         }
         cmsObj_free((void **) &dslDiagObj);
      }
      cmsLck_releaseLock();
   } /* aquire lock ok */
}


CmsRet addDiagInfoToResultFile(char *fileFullPath, UINT32 n, const char *cid, UINT32 lineNumber)
{
   FILE *fp;
   char line[BUFLEN_128]= {0};
   char timeBuf[BUFLEN_64];

   fp = fopen(fileFullPath,"a+");
   if (fp != NULL)
   {
      sprintf(line,"<N>%d</N>\n",n);
      fprintf(fp,"%s",line);
      
      sprintf(line,"<CID>%s</CID>\n",cid);
      fprintf(fp,"%s",line);
      
      sprintf(line,"<Line>%d</Line>\n",lineNumber);
      fprintf(fp,"%s",line);
      
      cmsTms_getXSIDateTime(dslDiagInfo.testStartTime,timeBuf,sizeof(timeBuf));
      sprintf(line,"<Start_TimeStamp>%s</Start_TimeStamp>\n",timeBuf);
      fprintf(fp,"%s",line);
      
      cmsTms_getXSIDateTime(dslDiagInfo.testEndTime,timeBuf,sizeof(timeBuf));
      sprintf(line,"<End_TimeStamp>%s</End_TimeStamp>\n",timeBuf);
      fprintf(fp,"%s",line);
      fclose(fp);
      return(CMSRET_SUCCESS);
   }
   return(CMSRET_INTERNAL_ERROR);
}

CmsRet storeSeltResult(const char *cid, UINT32 lineNumber)
{
   CmsRet ret = CMSRET_SUCCESS;
   char firstFileFullPath[BUFLEN_64]= {0};
   char resultFileFullPath[BUFLEN_64]= {0};
   char resultFileName[BUFLEN_16]= {0};
   char newNameFullPath[BUFLEN_64]= {0};   /* name: n from 0..2^32-1 which is 10 digits, 10 for cid, lineNumber is 1, 2 dots */
   DLIST_HEAD(dirHead);
   DlistNode *pFirstFileNode, *pLastFileNode;
   CmsFileListEntry *fent;
   char *pLastFileName;
   UINT32 n=0, numFile=0;
   int errorCode = 0;
   int testCompleted=DIAG_DSL_SELT_STATE_TEST_COMPLETE;
   int retry = 0;

   if (cmsFil_isDirPresent(DIAG_DSL_SELT_PERSISTENT_DIR_PATH) == FALSE)
   {
      if (cmsFil_makeDir(DIAG_DSL_SELT_PERSISTENT_DIR_PATH) != CMSRET_SUCCESS)
      {
         cmsLog_error("Unable create %s.  AuxFS needs to be enabled, writable /data must exist.",DIAG_DSL_SELT_PERSISTENT_DIR_PATH);
         return CMSRET_INTERNAL_ERROR;
      }
   }

   /* pass in the absolute path, driver stores post processed result in the result subfolder later */
   ret = xdslCtl_SeltFetchAndPostProcess((unsigned char)lineNumber,DIAG_DSL_SELT_CALIBRATION_NO,
                                         DIAG_DSL_SELT_RESULT_PROCESS_YES,
                                         NULL,DIAG_DSL_SELT_PERSISTENT_DIR_PATH,&errorCode);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Error fetching and post process data.  Error code %d\n",errorCode);
      return CMSRET_INTERNAL_ERROR;
   }
   else
   {
      /* check to see if post processing is completed */
      /* I do not really know how long a post processing will take.
       * Right now, I just give it a fix amount of time.   But ideally, this timeout should be
       * calculated with the measurement duration for better accuracy.
       */
      while ((ret == CMSRET_SUCCESS) && (testCompleted != DIAG_DSL_SELT_STATE_POST_PROCESS_COMPLETE) &&
             (retry < DIAG_DSL_SELT_TIMEOUT_PERIOD))
      {
         retry++;
         sleep(1);
         ret = xdslCtl_isSeltTestComplete(lineNumber, &testCompleted);
      }
   }
   if (testCompleted != DIAG_DSL_SELT_STATE_POST_PROCESS_COMPLETE)
   {
      return CMSRET_INTERNAL_ERROR;
   }
   /* I am assuming test complete time includes post processing time. */
   dslDiagInfo.testEndTime = time(NULL);

   /* need to tweak the file name a little:
      driver puts the result in file name "dataOut_lineId.txt where lineId is 0/1 */
   sprintf(resultFileName,"%s_%d.txt",DIAG_DSL_SELT_RESULT_FILE_NAME_PREFIX,lineNumber);

   /* the post processing data is stored in a file; it needs to be moved to files with name in this convention:
    * n.cid.lineNumber where n is from 0..max(unsignedInt).   When the files reaches max file saved, we purged out the file with 
    * smallest value n, and replace it with the new file 
    */
   /* first, let's try ausfs */
   sprintf(resultFileFullPath,"%s/%s",DIAG_DSL_SELT_RESULT_PERSISTENT_DIR_PATH,resultFileName);

   if (cmsFil_isFilePresent(resultFileFullPath) == TRUE)
   {
      /* need to figure out n */
      cmsFil_getNumFilesInDir(DIAG_DSL_SELT_RESULT_PERSISTENT_DIR_PATH,&numFile);
      /* there is also a result file file which drivers put in, so number of historical file is actually one less */
      if (numFile <= DIAG_DSL_SELT_MAX_TESTS_SAVED)
      {
         /* n starts from 0; so numFile (including the temp result file) - 1 will be the number used for new file */
         n = numFile-1;
      }
      else
      {
         if (cmsFil_getNumericalOrderedFileList(DIAG_DSL_SELT_RESULT_PERSISTENT_DIR_PATH, &dirHead) == CMSRET_SUCCESS)
         {
            /* I am only interested in last file (before the result file) for n, and first file to remove */
            pFirstFileNode = dirHead.next;
            pLastFileNode = (dirHead.prev);
            fent = (CmsFileListEntry *) pFirstFileNode;
            sprintf(firstFileFullPath,"%s/%s",DIAG_DSL_SELT_RESULT_PERSISTENT_DIR_PATH,fent->filename);
            fent = (CmsFileListEntry *) pLastFileNode;
            pLastFileName = fent->filename;
            cmsFil_getIntPrefixFromFileName(pLastFileName,&n);
            n = n+1;
            cmsFil_freeFileList(&dirHead);   
            unlink(firstFileFullPath);
         }
      }
      sprintf(newNameFullPath,"%s/%d.%s.%d",DIAG_DSL_SELT_RESULT_PERSISTENT_DIR_PATH,n,cid,lineNumber);
      if (cmsFil_renameFile(resultFileFullPath,newNameFullPath) != CMSRET_SUCCESS)
      {
         cmsLog_error("Unable to store newName %s.",newNameFullPath);
         ret = CMSRET_INTERNAL_ERROR;
      }
      else
      {
         addDiagInfoToResultFile(newNameFullPath,n,cid,lineNumber);
      }
   } /* result file present */
   else
   {
      cmsLog_error("Unable to file SELT result file %s.",resultFileFullPath);
   }

   return ret;
}

void getDslSeltDiagResults(void)
{
   SeltCfgObject *dslDiagObj;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   int timeout=0;
   int lockReleased=0;

   cmsLog_debug("Enter: dslDiagInfo.pollRetries %d, inProgress %d",dslDiagInfo.pollRetries,
                dslDiagInfo.dslDiagInProgress);

   if ((cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) == CMSRET_SUCCESS)
   {   
      if (cmsObj_get(MDMOID_SELT_CFG, &iidStack, 0, (void **) &dslDiagObj) == CMSRET_SUCCESS)
      {
         /* poll about every 2 seconds */
         timeout = (int)((atof(dslDiagObj->maxSeltT))/2.0) + DIAG_DSL_SELT_TIMEOUT_PERIOD; 

         if ((cmsUtl_strcmp(dslDiagObj->seltTestState, MDMVS_REQUESTED) == 0) &&
             (dslDiagInfo.pollRetries < timeout))
         {
            dslDiagInfo.pollRetries++;
         }
         else
         {
            if (dslDiagInfo.pollRetries >= timeout)
            {
               CMSMEM_REPLACE_STRING(dslDiagObj->seltTestState,MDMVS_ERROR);
               cmsObj_set(dslDiagObj, &iidStack);
            }
            else if (cmsUtl_strcmp(dslDiagObj->seltTestState,MDMVS_COMPLETE) == 0)
            {
               /* selt processing may take long, so release the lock first */
               cmsLck_releaseLock();
               lockReleased = 1;

               /* if the SELT test is completed, we need to move this test result into persistent storage */
               if  (storeSeltResult(dslDiagObj->CID, dslDiagObj->lineNumber) != CMSRET_SUCCESS)
               {
                  if ((cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) == CMSRET_SUCCESS)
                  {
                     cmsLog_error("Unable to store SELT result for cid %s, line %d\n",dslDiagObj->CID, dslDiagObj->lineNumber);
                     /* unable to get result is considered an error */
                     CMSMEM_REPLACE_STRING(dslDiagObj->seltTestState,MDMVS_ERROR);
                     cmsObj_set(dslDiagObj, &iidStack);
                     lockReleased = 0;
                  }
               }
               else
               {
                  cmsLog_debug("Selt test completed, result stored successfully.");
               }
            }
            CmsMsgHeader msg = EMPTY_MSG_HEADER;
            dslDiagInProgress=FALSE;
            dslDiagInfo.dslDiagInProgress = FALSE;
            dslDiagInfo.pollRetries = 0;

            /* we may wan to send to someone else as well here */
            if (dslDiagInfo.src == EID_TR69C)
            {
               msg.type = CMS_MSG_DSL_SELT_DIAG_COMPLETE;
               msg.src =  EID_SSK;
               msg.dst = EID_TR69C;
               msg.flags_event = 1;
               if (cmsMsg_send(msgHandle, &msg) != CMSRET_SUCCESS)
               {
                  cmsLog_error("could not send out CMS_MSG_DSL_LOOP_DIAG_COMPLETE event msg");
               }
               else
               {
                  cmsLog_debug("Send out CMS_MSG_DSL_LOOP_DIAG_COMPLETE event msg.");
               }
            }
         }
         cmsObj_free((void **) &dslDiagObj);
      } /* get obj ok */
      if (!lockReleased)
      {
         cmsLck_releaseLock();
      }
   } /* lock requested ok */
}

#endif /* DMP_X_BROADCOM_COM_SELT_1 */

/***********************************************************************
 *
 *  Copyright (c) 2014  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2014:proprietary:standard

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

#ifdef DMP_X_BROADCOM_COM_SELT_1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>

#include "syscall.h"
#include "httpd.h"
#include "cgi_main.h"
#include "adslctlapi.h"

/* Requirement: the test needs to be invoked in one command only; so all parameters need to be parse and set first,
 *              before the test is started.
 */
void cgiSeltMsg(FILE *fs, char *message)
{
    cgiWriteMessagePage(fs, " SELT Test", message, NULL);
}

void cgiSeltParseSet(char *query, FILE *fs) {
   SeltCfgObject *seltCfgObj = NULL;
   InstanceIdStack cfgIidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;
#ifdef UNAUTHENICATED_SELT
   /* path is the 1st input parameter if not a *.cmd page */
   char *query = strchr(path, '?');
#endif
   char maxSeltT[BUFLEN_16], maxSeltF[BUFLEN_16];
   char maxSeltPsd[BUFLEN_16], CID[BUFLEN_16];
   char line[BUFLEN_8];

   cgiGetValueByName(query,"maxSeltT",maxSeltT);
   cgiGetValueByName(query,"maxSeltF",maxSeltF);
   cgiGetValueByName(query,"maxSeltPsd",maxSeltPsd);
   cgiGetValueByName(query,"CID",CID);
   cgiGetValueByName(query,"line",line);
   
   if ((ret = cmsObj_get(MDMOID_SELT_CFG, &cfgIidStack, 0, (void **) &seltCfgObj)) == CMSRET_SUCCESS)
   {
      /* it's already running, don't allow parameter change it; only one SELT test can be done at a time */
      if (cmsUtl_strcmp(seltCfgObj->seltTestState,MDMVS_RUNNING) == 0)
      {
         cmsLog_error("current seltState is %s, cannot set it now.",seltCfgObj->seltTestState);
         cmsObj_free((void **) &seltCfgObj);
         cgiSeltMsg(fs, "Error running SELT test.   There is a SELT test running");
         return;
      }

      REPLACE_STRING_IF_NOT_EQUAL(seltCfgObj->maxSeltT,maxSeltT);
      REPLACE_STRING_IF_NOT_EQUAL(seltCfgObj->maxSeltPSD,maxSeltPsd);
      REPLACE_STRING_IF_NOT_EQUAL(seltCfgObj->maxSeltF,maxSeltF);
      REPLACE_STRING_IF_NOT_EQUAL(seltCfgObj->CID,CID);
      REPLACE_STRING_IF_NOT_EQUAL(seltCfgObj->seltTestState,MDMVS_REQUESTED);
      seltCfgObj->lineNumber= atoi(line);

      if ((cmsObj_set(seltCfgObj, &cfgIidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsObj_set of SELT CFG object returns error ret %d",ret);
         cgiSeltMsg(fs, "Error running SELT test.   There might be error setting the parameters.");
      }
      else
      {
         cgiSeltMsg(fs, "Selt Test in progress, view status with seltcfg.html.");
      }
      cmsObj_free((void **) &seltCfgObj);
   } /* seltcfgobj */
   return;
}

void do_selt_cgi(char *path, FILE *fs) {
   char filename[WEB_BUF_SIZE_MAX];
#ifdef UNAUTHENICATED_SELT
   char* query = NULL;
#endif
   char* ext = NULL;

#ifdef UNAUTHENICATED_SELT
   query = strchr(path, '?');
   if ( query != NULL )
      cgiSeltParseSet(path,fs);
#endif 

   filename[0] = '\0';
   ext = strchr(path, '.');
   if ( ext != NULL ) {
      *ext = '\0';
      strcpy(filename, path);

#ifdef UNAUTHENICATED_SELT
      if (strstr(filename, "seltresult") != NULL)
      {
         cgiPrintSeltResult(fs);
      }
      else
#endif
      {
         strcat(filename, ".html");
         do_ej(filename, fs);
      }
   }
}

void cgiGetSeltVar(char *varName, char *varValue) 
{
   SeltCfgObject *seltCfgObj = NULL;
   InstanceIdStack cfgIidStack = EMPTY_INSTANCE_ID_STACK;

   cmsLog_debug("enter: varName=%s", varName);

   if (cmsObj_get(MDMOID_SELT_CFG, &cfgIidStack, 0, (void **) &seltCfgObj) == CMSRET_SUCCESS)
   {
      if ( cmsUtl_strcmp(varName, "seltState") == 0 )
      {
         sprintf(varValue, "%s", seltCfgObj->seltTestState);
      }
      else if ( cmsUtl_strcmp(varName, "seltMaxT") == 0 )
      {
         sprintf(varValue, "%s", seltCfgObj->maxSeltT);
      }
      else if ( cmsUtl_strcmp(varName, "seltMaxF") == 0 )
      {
         sprintf(varValue, "%s", seltCfgObj->maxSeltF);
      }
      else if ( cmsUtl_strcmp(varName, "seltMaxPsd") == 0 )
      {
         sprintf(varValue, "%s", seltCfgObj->maxSeltPSD);
      }
      else if ( cmsUtl_strcmp(varName, "seltCid") == 0 ) 
      {
         sprintf(varValue, "%s", seltCfgObj->CID);
      }
      else if ( cmsUtl_strcmp(varName, "seltLine") == 0 )
      {
         sprintf(varValue, "%d", seltCfgObj->lineNumber);
      }
      else
      {
         strcpy(varValue, "");
      }
      cmsObj_free((void **) &seltCfgObj);
   } /* if (found) */

   cmsLog_debug("enter: varValue=%s", varValue);

   return;
}

UBOOL8 isSeltFileNameCorrect(char *fileName)
{
   char cid[BUFLEN_16]={0};
   char *last, *tokStr;
   int n=0;
   int lineId=0;
   
   tokStr=strtok_r(fileName,".",&last);

   if (tokStr == NULL)
   {
      return FALSE;
   }
   n = atoi(tokStr);

   tokStr=strtok_r(NULL,".",&last);
   if (tokStr == NULL)
   {
      return FALSE;
   }
   strcpy(cid,tokStr);

   tokStr=strtok_r(NULL,".",&last);
   if (tokStr == NULL)
   {
      return FALSE;
   }   
   lineId = atoi(tokStr);
   if (n < 0)
   {
      return FALSE;
   }
   if (strlen(cid) != 10)
   {
      return FALSE;
   }
   if ((lineId != 0) && (lineId != 1))
   {
      return FALSE;
   }
   return TRUE;
}

void cgiPrintSeltResult(char *query __attribute__((unused)),FILE *fs)
{
   CmsRet ret = CMSRET_SUCCESS;
   SINT32 rc;
   DLIST_HEAD(dirHead);
   DlistNode *tmp = NULL;
   char fullpathname[BUFLEN_64]={0};   
   char filename[BUFLEN_32]={0};   
   UINT32 fileSize=0, displayHtmlBufLen=0;
   char *pDisplayBuf=NULL;
   char *pDisplayHtmlBuf=NULL;
   char cid[BUFLEN_16]={0};
   char *last, *tokStr;
   int n=0;
   int lineId=0;

   fprintf(fs, "<html>\n");
   fprintf(fs, "<head>\n");
   fprintf(fs, "<title>SELT Historical Result</title>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
   fprintf(fs, "</head>\n");
   fprintf(fs, "<body>\n");
   fprintf(fs, "<br>\n");
   fprintf(fs, "<br>\n");

   if (cmsFil_getNumericalOrderedFileList(DIAG_DSL_SELT_RESULT_PERSISTENT_DIR_PATH,&dirHead) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not locate result files");
      fprintf(fs, "Could not locate result\n");
   }
   else
   {
      tmp = dirHead.next;
      while (ret == CMSRET_SUCCESS && tmp != &dirHead)
      {
         CmsFileListEntry *fent = (CmsFileListEntry *) tmp;

         rc = snprintf(fullpathname, sizeof(fullpathname), "%s/%s",
                       DIAG_DSL_SELT_RESULT_PERSISTENT_DIR_PATH, fent->filename);

         if (rc >= BUFLEN_64)
         {
            cmsLog_error("result file name %s with full path (%d) exceeded max len allowed %d",
                         fent->filename, rc, BUFLEN_64);
         }
         else
         {
            /* add one for the \0 */
            fileSize = cmsFil_getSize(fullpathname) + 1;
            strcpy(filename,fent->filename);
            if (fileSize <= 1)
            {
               fprintf(fs, "Result file is empty.\n");
            }
            else if (isSeltFileNameCorrect(filename) == TRUE)
            {
               cmsLog_debug("resultFile (%s) = %d", fent->filename,fileSize);
               pDisplayBuf = cmsMem_alloc(fileSize, ALLOC_ZEROIZE);
               if (pDisplayBuf == NULL)
               {
                  cmsLog_error("malloc of %d bytes failed", fileSize);
                  fprintf(fs, "Malloc of %d bytes for result failed\n", fileSize);
                  
               }
               else
               {
                  ret = cmsFil_copyToBuffer(fullpathname,(UINT8*)pDisplayBuf,&fileSize);
                  if (ret != CMSRET_SUCCESS)
                  {
                     cmsLog_error("read result failed, ret=%d displayBufLen=%d", ret, fileSize);
                     fprintf(fs, "Read result file failed, ret=%d displayBufLen=%d\n", ret, fileSize);
                  }
                  else
                  {
                     displayHtmlBufLen = calcLenOfMappingTxtToHtml(pDisplayBuf);
                     pDisplayHtmlBuf = cmsMem_alloc(displayHtmlBufLen+1, ALLOC_ZEROIZE);
                     if( displayHtmlBufLen > 0 && pDisplayHtmlBuf != NULL)
                     {
                        tokStr=strtok_r(fent->filename,".",&last);
                        n = atoi(tokStr);
                        tokStr=strtok_r(NULL,".",&last);
                        strcpy(cid,tokStr);
                        tokStr=strtok_r(NULL,".",&last);
                        lineId = atoi(tokStr);

                        mappingTxtToHtml(pDisplayBuf, pDisplayHtmlBuf);
                        fprintf(fs, "<br>\n");
                        fprintf(fs, "--------- N %d, CID %s, Line %d ---------\n",n,cid,lineId);
                        fprintf(fs, "<br>\n");
                        fprintf(fs, "%s\n", pDisplayHtmlBuf);
                        fprintf(fs, "<br>\n");
                        fprintf(fs, "<br>\n");
                        cmsMem_free(pDisplayHtmlBuf);
                     }
                     else
                     {
                        fprintf(fs, "Failed to write result to html file\n");
                     }
                  }
                  cmsMem_free(pDisplayBuf);
               } /* pDisplayBuf allocated */
            } /* fileSize has something to display */
         } /* result file name ok */
         /* next file */
         tmp = tmp->next;
      } /* while all files */
      cmsFil_freeFileList(&dirHead);
   
      fprintf(fs, "</body>\n");
      fprintf(fs, "</html>\n");
      fflush(fs);
   } /* located result dir */
} /* print result files */
#endif /* DMP_X_BROADCOM_COM_SELT_1 */

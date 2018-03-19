/***********************************************************************
 *
 *  Copyright (c) 2007  Broadcom Corporation
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
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>

#include "syscall.h"
#include "httpd.h"
#include "cgi_main.h"
#include "cgi_dump.h"
#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"

int calcLenOfMappingTxtToHtml(char* txtStr);
int mappingTxtToHtml(char* txtStr, char* htmlStr);

//Support generic html entities mapping. If you need more, you can update the table.
TXTTOHTMLCHARENTITIES txtToHtmlCharEntityTbl[] = 
{
   {' ',    "&nbsp;",         6},
   {'<',    "&lt;",           4},
   {'>',    "&gt;",           4},
   {'&',    "&amp;",          5},
   {'<',    "&quot;",         6},
   {'\n',   "<br>",           4}, 
};

int numOfHtmlCharEntities = sizeof(txtToHtmlCharEntityTbl)/sizeof(TXTTOHTMLCHARENTITIES);

int calcLenOfMappingTxtToHtml(char* txtStr)
{
   int ret = 0;
   int len = 0;
   int i; 
   char* curToTxt = NULL;
   if(txtStr == NULL)
      return ret;

   curToTxt = txtStr;

   while( *curToTxt != '\0' )
   {
      for( i = 0; i < numOfHtmlCharEntities; i++)
      {
         if( *curToTxt == txtToHtmlCharEntityTbl[i].txtChar)
         {
            len += txtToHtmlCharEntityTbl[i].htmlEntityLen;
            break;
         }
      }

      //not match, plus one
      if( i == numOfHtmlCharEntities )
         len++;
      
      curToTxt++;
   }

   //plus one for '\0';
   len++;

   return ret = len;
}

int mappingTxtToHtml(char* txtStr, char* htmlStr)
{
   int ret = 0;
   int i; 
   char *curToTxt = NULL, *curToHtml = NULL;
   if(txtStr == NULL || htmlStr == NULL)
      return ret;

   curToTxt = txtStr;
   curToHtml = htmlStr;

   while( *curToTxt != '\0' )
   {
      for( i = 0; i < numOfHtmlCharEntities; i++)
      {
         if( *curToTxt == txtToHtmlCharEntityTbl[i].txtChar)
         {
            //copy entities string
            strncpy(curToHtml, txtToHtmlCharEntityTbl[i].htmlEntity, txtToHtmlCharEntityTbl[i].htmlEntityLen);
            curToHtml += txtToHtmlCharEntityTbl[i].htmlEntityLen;
            break;
         }
      }

      //not match, copy it directly
      if( i == numOfHtmlCharEntities )
      {
         *curToHtml = *curToTxt;
         curToHtml++;         
      }

      curToTxt++;
   }

   *curToHtml = '\0';
   
   return ret = 1;
}

#ifdef SUPPORT_DEBUG_TOOLS
void cgiDumpCfg(char *query __attribute__((unused)), FILE *fs)
{
   UINT32 origCfgBufLen, cfgBufLen, cfgHtmlBufLen;
   char *cfgBuf;
   char *cfgHtmlBuf;
   CmsRet ret;
   
   fprintf(fs, "<html>\n");
   fprintf(fs, "<head>\n");
   fprintf(fs, "<title>Dump Configuration</title>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
   fprintf(fs, "</head>\n");
   fprintf(fs, "<body>\n");
   fprintf(fs, "<br>\n");
   fprintf(fs, "<br>\n");

   origCfgBufLen = cfgBufLen = cmsImg_getConfigFlashSize();
   if (cfgBufLen == 0)
   {
      cmsLog_error("Could not get config flash size");
      fprintf(fs, "Could not get config flash size\n");
   }
   else
   {
      cmsLog_debug("configBufLen=%d", cfgBufLen);
      cfgBuf = cmsMem_alloc(cfgBufLen, 0);
      if (cfgBuf == NULL)
      {
         cmsLog_error("malloc of %d bytes failed", cfgBufLen);
         fprintf(fs, "Malloc of %d bytes failed\n", cfgBufLen);
         
      }
      else
      {
         ret = cmsMgm_readConfigFlashToBuf(cfgBuf, &cfgBufLen);
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("read config failed, ret=%d cfgBufLen=%d", ret, cfgBufLen);
            fprintf(fs, "Read config failed, ret=%d cfgBufLen=%d\n", ret, cfgBufLen);
         }
         else if (cfgBufLen <= 1)
         {
            fprintf(fs, "Config flash is empty.\n");
         }
         else
         {
            cfgHtmlBufLen = calcLenOfMappingTxtToHtml(cfgBuf);
            cfgHtmlBuf = cmsMem_alloc(cfgHtmlBufLen, 0);
            if( cfgHtmlBufLen > 0 && cfgHtmlBuf != NULL)
            {
               mappingTxtToHtml(cfgBuf, cfgHtmlBuf);
               fprintf(fs, "%s\n", cfgHtmlBuf);
               fprintf(fs, "<br>\n");
               fprintf(fs, "<br>\n");
               fprintf(fs, "Dump bytes allocated=%d used=%d\n", origCfgBufLen, cfgBufLen);
               cmsMem_free(cfgHtmlBuf);
            }
            else
            {
               fprintf(fs, "Failed to write infomation to html file\n");
            }
         }

         cmsMem_free(cfgBuf);
      }
   }
   
   fprintf(fs, "</body>\n");
   fprintf(fs, "</html>\n");
   fflush(fs);
}


void cgiDumpCfgDync(char *query __attribute__((unused)), FILE *fs)
{
   UINT32 origCfgBufLen, cfgBufLen, cfgHtmlBufLen;
   char *cfgBuf;
   char *cfgHtmlBuf;
   CmsRet ret;
   
   fprintf(fs, "<html>\n");
   fprintf(fs, "<head>\n");
   fprintf(fs, "<title>Dump Configuration from MDM</title>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
   fprintf(fs, "</head>\n");
   fprintf(fs, "<body>\n");

   origCfgBufLen = cfgBufLen = cmsImg_getConfigFlashSize();
   if (cfgBufLen == 0)
   {
      cmsLog_error("Could not get config flash size");
      fprintf(fs, "Could not get config flash size\n");
   }
   else
   {
      cmsLog_debug("configBufLen=%d", cfgBufLen);
      cfgBuf = cmsMem_alloc(cfgBufLen, 0);
      if (cfgBuf == NULL)
      {
         cmsLog_error("malloc of %d bytes failed", cfgBufLen);
         fprintf(fs, "Malloc of %d bytes failed\n", cfgBufLen);
         
      }
      else
      {
         ret = cmsMgm_writeConfigToBuf(cfgBuf, &cfgBufLen);
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("read config failed, ret=%d cfgBufLen=%d", ret, cfgBufLen);
            fprintf(fs, "Read config failed, ret=%d cfgBufLen=%d\n", ret, cfgBufLen);
         }
         else if (cfgBufLen <= 1)
         {
            fprintf(fs, "Config flash is empty.\n");
         }
         else
         {
            cfgHtmlBufLen = calcLenOfMappingTxtToHtml(cfgBuf);
            cfgHtmlBuf = cmsMem_alloc(cfgHtmlBufLen, 0);
            if( cfgHtmlBufLen > 0 && cfgHtmlBuf != NULL)
            {
               mappingTxtToHtml(cfgBuf, cfgHtmlBuf);
               fprintf(fs, "%s\n", cfgHtmlBuf);
               fprintf(fs, "<br>\n");
               fprintf(fs, "<br>\n");
               fprintf(fs, "Dump bytes allocated=%d used=%d\n", origCfgBufLen, cfgBufLen);
               cmsMem_free(cfgHtmlBuf);
            }
            else
            {
               fprintf(fs, "Failed to write infomation to html file\n");
            }
         }

         cmsMem_free(cfgBuf);
      }
   }
   
   fprintf(fs, "</body>\n");
   fprintf(fs, "</html>\n");
   fflush(fs);
}

void cgiDumpMdm(char *query __attribute__((unused)), FILE *fs)
{
   UINT32 origCfgBufLen, cfgBufLen, cfgHtmlBufLen;
   char *cfgBuf;
   char *cfgHtmlBuf;
   CmsMemStats memStats;
   UINT32 shmBytesInUse;
   CmsRet ret;
   
   fprintf(fs, "<html>\n");
   fprintf(fs, "<head>\n");
   fprintf(fs, "<title>Dump MDM</title>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
   fprintf(fs, "</head>\n");
   fprintf(fs, "<body>\n");


   /*
    * The size of the buffer to hold the MDM is directly proportional to the amount
    * of shared memory in use.  But since the data in the MDM is more compact than
    * when it is dumped out, use a multipling factor of 3.
    */
   cmsMem_getStats(&memStats);
   shmBytesInUse = (memStats.shmAllocStart - MDM_SHM_ATTACH_ADDR) + memStats.shmBytesAllocd;

   origCfgBufLen = (shmBytesInUse * 3);
   cfgBufLen = origCfgBufLen;
   
   cfgBuf = cmsMem_alloc(cfgBufLen, 0);
   if (cfgBuf == NULL)
   {
      cmsLog_error("malloc of %d bytes failed", cfgBufLen);
      fprintf(fs, "Malloc of %d bytes failed\n", cfgBufLen);
      
   }
   else
   {
      ret = cmsMgm_writeMdmToBuf(cfgBuf, &cfgBufLen);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("read config failed, ret=%d cfgBufLen=%d", ret, cfgBufLen);
         fprintf(fs, "Read MDM failed, ret=%d cfgBufLen=%d\n", ret, cfgBufLen);
      }
      else if (cfgBufLen <= 1)
      {
         fprintf(fs, "Error getting MDM.\n");
      }
      else
      {
         cfgHtmlBufLen = calcLenOfMappingTxtToHtml(cfgBuf);
         cfgHtmlBuf = cmsMem_alloc(cfgHtmlBufLen, 0);
         if( cfgHtmlBufLen > 0 && cfgHtmlBuf != NULL)
         {
            mappingTxtToHtml(cfgBuf, cfgHtmlBuf);
            fprintf(fs, "%s\n", cfgHtmlBuf);
            fprintf(fs, "<br>\n");
            fprintf(fs, "<br>\n");
            fprintf(fs, "Dump bytes allocated=%d used=%d\n", origCfgBufLen, cfgBufLen);
            cmsMem_free(cfgHtmlBuf);
         }
         else
         {
            fprintf(fs, "Failed to write infomation to html file\n");
         }
      }

      cmsMem_free(cfgBuf);
   }

   fprintf(fs, "</body>\n");
   fprintf(fs, "</html>\n");
   fflush(fs);
}


#endif /* SUPPORT_DEBUG_TOOLS */

/***********************************************************************
 *
 *  Copyright (c) 2008-2010  Broadcom Corporation
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

#ifdef DMP_X_ITU_ORG_GPON_1 /* aka SUPPORT_OMCI */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>

#include "cms.h"
#include "cms_msg.h"
#include "cms_util.h"
#include "cms_phl.h"
#include "httpd.h"
#include "cgi_main.h"
#include "cgi_omci.h"


// global variables
// in cgi_omci_cfg
extern UINT16 tcIdCur;
extern UINT16 numMsgRx;
extern UINT16 numMsgTx;
extern UINT16 numRetry;

omciMacroState macroState = OMCI_MARCO_OFF;

/*
 * local variables
 */
static char action[BUFLEN_264] = {0};

static void cgiOmci_runCmdInCfgFile(void);
static void cgiOmci_saveSettings(FILE *pFileWrite);
static void cgiOmciMacroView(FILE *fs);
static void writeOmciMacroHeader(FILE *fs);
static void writeOmciMacroScript(FILE *fs);
static void writeOmciMacroButton(FILE *fs);

// Main entry for OMCI macro web page
void cgiOmciMacro(char *query, FILE *fs) 
{
   action[0] = '\0';

   cgiGetValueByName(query, "action", action);

   if (cmsUtl_strcmp(action, "on") == 0)
   {
      char fileName[BUFLEN_264];

      cgiOmci_makePathToOmci(fileName, BUFLEN_264, OMCI_CONF_NAME);
      unlink(fileName);
      macroState = OMCI_MARCO_ON;
   }
   else if (cmsUtl_strcmp(action, "off") == 0)
   {
      macroState = OMCI_MARCO_OFF;
   }
   else if (cmsUtl_strcmp(action, "run") == 0)
   {
      if (numMsgTx == 0)
      {
         numMsgRx = 0;
         cgiOmci_runCmdInCfgFile();
      }
   }
   else if (cmsUtl_strcmp(action, "save") == 0)
   {
      cgiOmci_saveSettings(fs);
   }

   // only write html code when action is not save
   // so that html codes will not be saved to omci settings file
   if (cmsUtl_strcmp(action, "save") != 0)
   {
      cgiOmciMacroView(fs);
   }
}

int cgiOmciUpload(FILE *fs) 
{
   int ret = WEB_STS_OK, boundSize = 0;
   char fileName[BUFLEN_264]={0};
   char buf[BUFLEN_1024]={0}, bound[BUFLEN_256]={0};
   char *pdest = NULL;
   FILE *pFileWrite = NULL;
   UBOOL8 isTerminated = FALSE;

   // Read the first UPLOAD_BOUND_SIZE(256) bytes of the image.
   httpd_gets(bound, sizeof(bound));
   boundSize = cmsMem_strnlen(bound, sizeof(bound), &isTerminated) - 2;
   if (!isTerminated || boundSize <= 0)
   {
      cmsLog_error("Failed to find bound within the first %d bytes", sizeof(bound));
      return WEB_STS_ERROR;
   }
   bound[boundSize] = '\0';

   // get Content-Disposition: form-data; name="filename"; filename="test"
   // check filename, if "", user click no filename and report
   httpd_gets(buf, sizeof(buf));
   if ((pdest = strstr(buf, FN_UPLOAD)) == NULL)
   {
      cmsLog_error("could not find filename within the first %d bytes", sizeof(buf));
      return WEB_STS_ERROR;
   }
   pdest += (strlen(FN_UPLOAD) - 1);
   if (*pdest == '"' && *(pdest + 1) == '"')
   {
      cmsLog_error("No filename selected");
      return WEB_STS_ERROR;
   }
   else
   {
      cmsLog_debug("filename %s", pdest);
   }

   // get [Content-Type: application/octet-stream] and NL (cr/lf) and discard them
   httpd_gets(buf, sizeof(buf));
   httpd_gets(buf, sizeof(buf));

   // open file in memory
   cgiOmci_makePathToOmci(fileName, BUFLEN_264, OMCI_CONF_NAME);
   pFileWrite = fopen(fileName, "wb");

   if (pFileWrite != NULL)
   {
      UINT8 *binBuf = NULL;
      UINT16 i = 0;
      UINT16 msgSize = sizeof(CmsMsgHeader) + sizeof(omciPacket);
      UINT32 bufSize = 0;
      // OMCI message is stored in file as hex string that has
      // double size of OMCI message
      UINT32 strSize = (msgSize * 2) + 1;
      char hexStr[strSize];
      CmsRet rv = CMSRET_SUCCESS;

      // initialize hexStr
      memset(hexStr, 0, strSize);

      // read the OMCI commands in host file and write them to memory
      while (!feof(fs))
      {
         if (fgets(hexStr, strSize, fs) != NULL)
         {
            // if it reaches the last upload bound then stop
            if (strstr(hexStr, "-----") != NULL)
               break;
            if (strlen(hexStr) == (msgSize * 2))
            {
               rv = cmsUtl_hexStringToBinaryBuf(hexStr, &binBuf, &bufSize);
               if (rv == CMSRET_SUCCESS)
               {
                  for (i = 0; i < bufSize; i++)
                     fputc(binBuf[i], pFileWrite);
                  CMSMEM_FREE_BUF_AND_NULL_PTR(binBuf);
               }
               // read pass newline character
               if( fgets(hexStr, strSize, fs) == NULL)
                  cmsLog_error("Failed to get new line");
            }
            // initialize hexStr
            memset(hexStr, 0, strSize);
         }
      }

      // close memory file
      fclose(pFileWrite);
      // read pass the last upload bound
      while (httpd_gets(hexStr, sizeof(strSize)) == TRUE);
   }
   else
   {
      cmsLog_error("Failed to open %s for write", fileName);
      ret = WEB_STS_ERROR;		
   }

   // execute OMCI commands that are saved in memory
   cgiOmci_runCmdInCfgFile();

   // display the OMCI macro WEB UI
   cgiOmciMacroView(fs);

   return ret;
}

// run OMCI commands that are stored in configuration file
static void cgiOmci_runCmdInCfgFile(void)
{
   UINT16 i = 0, j = 0;
   UINT16 msgSize = sizeof(CmsMsgHeader) + sizeof(omciPacket);
   char fileName[BUFLEN_264];
   char buf[msgSize];
   CmsMsgHeader *msg = (CmsMsgHeader *) buf;
   CmsRet ret = CMSRET_SUCCESS;
   FILE  *fs = NULL;

   cgiOmci_makePathToOmci(fileName, BUFLEN_264, OMCI_CONF_NAME);
   if ((fs = fopen(fileName, "rb")) == NULL)
   {
      cmsLog_error("Failed to open %s for read", fileName);
      return;
   }

   while (!feof(fs))
   {
      buf[i++] = fgetc(fs);
      if (i == msgSize)
      {
         j++;

         // fixed JIRA (SWBCACPE-8034)
         // assign the current eids values to src and dst
         // since eids that are saved in script might have old values
         msg = (CmsMsgHeader *) buf;
         msg->src = EID_HTTPD;
         msg->dst = EID_OMCID;

         if ((ret = cmsMsg_send(msgHandle, (CmsMsgHeader *) buf)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Could not send out CMS_MSG_OMCI_COMMAND_REQUEST, ret=%d, id = %d from HTTPD to OMCID", ret, j);
         }
         else
         {
            cmsLog_notice("Send CMS_MSG_OMCI_COMMAND_REQUEST with id = %d from HTTPD to OMCID", j);
            numMsgTx++;		
         }
         i = 0;
      }
   }

   fclose(fs);
}

static void cgiOmci_saveSettings(FILE *pFileWrite)
{
   UINT16 i = 0, msgSize = sizeof(CmsMsgHeader) + sizeof(omciPacket);
   UINT8 msgStr[msgSize+1];
   char fileName[BUFLEN_264];
   char *hexStr = NULL;
   FILE *pFileRead = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   cgiOmci_makePathToOmci(fileName, BUFLEN_264, OMCI_CONF_NAME);
   pFileRead = fopen(fileName, "rb");

   if (pFileRead != NULL)
   {
      // initialize msgStr
      memset(msgStr, 0, msgSize+1);
      while (!feof(pFileRead))
      {
         msgStr[i++] = fgetc(pFileRead);
         if (i == msgSize)
         {
            ret = cmsUtl_binaryBufToHexString(msgStr, msgSize, &hexStr);
            if (ret == CMSRET_SUCCESS)
            {
               fputs(hexStr, pFileWrite);
               CMSMEM_FREE_BUF_AND_NULL_PTR(hexStr);
            }
            i = 0;
            // initialize msgStr
            memset(msgStr, 0, msgSize+1);
            // write newline character to mark the end of message
            fputs("\n", pFileWrite);
         }
      }
      // write newline character to mark the end of file
      fputs("\n", pFileWrite);
      fclose(pFileRead);
   }
   else
   {
      cmsLog_error("Could not open %s to read", fileName);
   }

   fflush(pFileWrite);
   pFileWrite = NULL;
}

// OMCI macro main page
static void cgiOmciMacroView(FILE *fs)
{
#ifdef OMCI_CFG_DEBUG
   printf("===> cgiOmciMacroView: action = %s, macroState = %d\n", action, macroState);
#endif

   writeOmciMacroHeader(fs);
   writeOmciMacroScript(fs);
   writeOmciMacroButton(fs);
   cgiOmci_writeOmciEnd(fs);
}

// Header
static void writeOmciMacroHeader(FILE *fs) 
{
   fprintf(fs, "<html>\n   <head>\n");
   fprintf(fs, "      <meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
   fprintf(fs, "         <link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "         <link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "            <script language=\"javascript\" src=\"util.js\"></script>\n");
   fprintf(fs, "   </head>\n");
   fprintf(fs, "   <body onLoad='frmLoad()'>\n");
   fprintf(fs, "      <blockquote>\n");
   fprintf(fs, "         <form>\n");
}

// Scripts
static void writeOmciMacroScript(FILE *fs) 
{
#ifdef OMCI_CFG_DEBUG
   printf("===> writeOmciMacroScript: action = %s\n", action);
#endif

   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   fprintf(fs, "var RECORD_OFF = 'Click to turn OFF record';\n");
   fprintf(fs, "var RECORD_ON = 'Click to turn ON record';\n");
   fprintf(fs, "var progress = 0;\n\n");

   fprintf(fs, "function frmLoad() {\n");
   fprintf(fs, "   with ( document.forms[0] ) {\n");
   if (strcmp(action, "run") == 0 &&
       numMsgTx != numMsgRx &&
       numRetry < NUM_RETRIES_MAX)
   {
      numRetry++;
      fprintf(fs, "      setTimeout('refreshPage()', 1000);\n");
   }
   else
   {
      // reset counters if action is not "run"
      numMsgTx = numMsgRx = numRetry = 0;
   }
   if (macroState == OMCI_MARCO_ON)
   {
      fprintf(fs, "      macroBtn.value = RECORD_OFF;\n");
   }
   else
   {
      fprintf(fs, "      macroBtn.value = RECORD_ON;\n");
   }
   fprintf(fs, "   }\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function btnMacro() {\n");
   fprintf(fs, "   var loc = 'omcimacro.cmd?';\n\n");
   fprintf(fs, "   with ( document.forms[0] ) {\n");
   fprintf(fs, "      if (macroBtn.value == RECORD_ON)\n");
   fprintf(fs, "         loc += 'action=on';\n");
   fprintf(fs, "      else\n");
   fprintf(fs, "         loc += 'action=off';\n");
   fprintf(fs, "   }\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function btnRun() {\n");
   fprintf(fs, "   var loc = 'omcimacro.cmd?action=run';\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function btnSave() {\n");
   fprintf(fs, "   var loc = 'omcimacro.conf?action=save';\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function refreshPage() {\n");
   fprintf(fs, "   var loc = 'omcimacro.cmd?';\n\n");
   fprintf(fs, "   with ( document.forms[0] ) {\n");
   if (macroState == OMCI_MARCO_ON)
   {
      fprintf(fs, "         loc += 'action=on';\n");
   }
   else
   {
      fprintf(fs, "         loc += 'action=off';\n");
   }
   fprintf(fs, "   }\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function isInProgress() {\n");
   fprintf(fs, "   if ( progress == 0 )  {\n");
   fprintf(fs, "      progress = 1;\n");
   fprintf(fs, "      return true;\n");
   fprintf(fs, "   }\n");
   fprintf(fs, "   alert('Updating settings is in progress and the router will reboot. Please wait for a minute.');\n");
   fprintf(fs, "   return false;\n");
   fprintf(fs, "}\n\n");


   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");
}

static void writeOmciMacroButton(FILE *fs) 
{
   fprintf(fs, "            <b>OMCI -- Macro</b><br><br>\n");
   fprintf(fs, "            This page allows you to record OMCI commands to file, run these commands, or save them to local host.\n");
   fprintf(fs, "            <br><br><hr size=4 noshade><br>\n");
   fprintf(fs, "            <b>OMCI -- Toggle Macro</b><br><br>\n");
   if (macroState == OMCI_MARCO_ON)
   {
      fprintf(fs, "            <font color='red'><b>%s</b></font>\n", OMCI_MSG_MACRO_ON);
   }
   else
   {
      fprintf(fs, "            <font color='green'><b>%s</b></font>\n", OMCI_MSG_MACRO_OFF);
   }
   fprintf(fs, "            <br><br>\n");
   fprintf(fs, "            Click on button below to toggle its macro state.<br><br>\n");
   fprintf(fs, "            <p align='center'><input type='button' onClick='btnMacro()' name='macroBtn'></p>\n");

   fprintf(fs, "            <hr size=4 noshade><br>\n");
   fprintf(fs, "            <b>OMCI -- Execute Macro Script</b><br><br>\n");
   fprintf(fs, "            Click on Run button to execute recorded OMCI commands.<br><br>\n");
   fprintf(fs, "            <p align='center'>\n");
   fprintf(fs, "               <input type='button' onClick='btnRun()' value='Run' name='runBtn'>\n");
   fprintf(fs, "            </p>\n");

   fprintf(fs, "            <hr size=4 noshade><br>\n");
   fprintf(fs, "            <b>OMCI -- Save Macro Script</b><br><br>\n");
   fprintf(fs, "            Click on Save button to save recorded OMCI commands to macro script on the local host.<br><br>\n");
   fprintf(fs, "            <p align='center'>\n");
   fprintf(fs, "               <input type='button' onClick='btnSave()' value='Save' name='saveBtn'>\n");
   fprintf(fs, "            </p>\n");
   fprintf(fs, "         </form>\n");	

   fprintf(fs, "         <hr size=4 noshade><br>\n");
   fprintf(fs, "         <b>OMCI -- Upload Macro Script</b><br><br>\n");
   fprintf(fs, "         <form method='post' ENCTYPE='multipart/form-data' action='uploadomci.cgi' onSubmit='return isInProgress();'>\n");
   fprintf(fs, "            Select your macro script file then click on Upload Script button to execute OMCI router settings from this macro script.<br><br>\n");
   fprintf(fs, "            <table border='0' cellpadding='0' cellspacing='0'>\n");
   fprintf(fs, "               <tr>\n");
   fprintf(fs, "                  <td>Settings File Name:&nbsp;</td>\n");
   fprintf(fs, "                  <td><input type='file' name='filename' size='15'></td>\n");
   fprintf(fs, "               </tr>\n");
   fprintf(fs, "            </table>\n");
   fprintf(fs, "            <p align='center'><input type='submit' value='Upload Script'></p>\n");
}

#endif   //DMP_X_ITU_ORG_GPON_1

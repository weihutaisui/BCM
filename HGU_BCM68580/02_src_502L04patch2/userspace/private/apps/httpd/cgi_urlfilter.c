/***********************************************************************
 *
 *  Copyright (c) 2000-2010  Broadcom Corporation
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

#ifdef SUPPORT_URLFILTER
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>

#include "httpd.h"
#include "cgi_main.h"
#include "syscall.h"
#include "cms.h"
#include "cms_util.h"
#include "cms_core.h"
#include "cms_dal.h"

extern int glbEntryCt;			// used for web page to know how many entries can be added. In cgimain.c

const int TOD_URL_FILTER_MAX_ENTRY = 100;

void writeTodUrlScript(FILE *fs)
{
   char type[BUFLEN_16];

   fprintf(fs, "<script language='javascript' src='util.js'></script>\n");
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");
                                                                                
   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   fprintf(fs, "function addClick() {\n");
   fprintf(fs, "   var loc = 'url_add.html';\n\n");
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function modeClick() {\n");
   fprintf(fs, "   var loc = 'urlfilter.cmd?action=save';\n\n");
   fprintf(fs, "   with ( document.forms[0] ) {\n");
   fprintf(fs, "      if ( listtype[0].checked == true )\n");
   fprintf(fs, "         loc += '&listtype=Exclude';\n");
   fprintf(fs, "      else if ( listtype[1].checked == true )\n");
   fprintf(fs, "         loc += '&listtype=Include';\n");
   fprintf(fs, "   }\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   dalUrlFilter_getType(type);

   fprintf(fs, "function frmLoad() {\n");
   fprintf(fs, "   with ( document.forms[0] ) {\n");
   fprintf(fs, "      if ( '%s' == 'Exclude' )\n", type);
   fprintf(fs, "         listtype[0].checked = true;\n");
   fprintf(fs, "      else if ( '%s' == 'Include' )\n", type);
   fprintf(fs, "         listtype[1].checked = true;\n");
   fprintf(fs, "   }\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function removeClick(rml) {\n");
   fprintf(fs, "   var lst = '';\n");
   fprintf(fs, "   if (rml.length > 0)\n");
   fprintf(fs, "      for (i = 0; i < rml.length; i++) {\n");
   fprintf(fs, "         if ( rml[i].checked == true )\n");
   fprintf(fs, "            lst += rml[i].value + ' ';\n");
   fprintf(fs, "      }\n");
   fprintf(fs, "   else if ( rml.checked == true )\n");
   fprintf(fs, "      lst = rml.value;\n");
   fprintf(fs, "   var loc = 'urlfilter.cmd?action=remove_url&rmLst=' + lst;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");
   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");

}


void cgiTod_Url_View(FILE *fs)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UrlFilterListObject *urlListCfg = NULL;
   char url_address[BUFLEN_128];

   url_address[0] = '\0';
   // write header
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
                                                                                
   // write Java Script
   writeTodUrlScript(fs);
                                                                                
   // write body
   fprintf(fs, "</head>\n<body onLoad='frmLoad()'>\n<blockquote>\n");
   fprintf(fs, "<form>\n");
   fprintf(fs, "<b>URL Filter -- Please select the list type first then configure the list entries. Maximum %d entries can be configured.</b><br><br>\n", TOD_URL_FILTER_MAX_ENTRY);

   fprintf(fs, "<table border='0' cellpadding='4' cellspacing='0'>\n");
   fprintf(fs, "   <tr>\n");
   fprintf(fs, "      <td>URL List Type:</td>\n");
   fprintf(fs, "      <td><input type='radio' name='listtype' onClick='modeClick()'>&nbsp;&nbsp;Exclude</td>\n");
   fprintf(fs, "      <td><input type='radio' name='listtype' onClick='modeClick()'>&nbsp;&nbsp;Include</td>\n");
   fprintf(fs, "   </tr>\n");
   fprintf(fs, "</table><br><br>\n");

   fprintf(fs, "<br>\n" );
   fprintf(fs, "<center>\n");
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");
   // write table header
   fprintf(fs, "   <tr>\n");
   fprintf(fs, "      <td class='hd'>Address</td>\n");
   fprintf(fs, "      <td class='hd'>Port</td>\n");
   fprintf(fs, "      <td class='hd'>Remove</td>\n");
   fprintf(fs, "   </tr>\n");

   glbEntryCt = 0;

   /* Get the objects one after another till we fail. */
   while (cmsObj_getNext(MDMOID_URL_FILTER_LIST, &iidStack, (void **) &urlListCfg) == CMSRET_SUCCESS)
   {
      fprintf(fs, "   <tr>\n");
      strcpy(url_address, urlListCfg->urlAddress);
      fprintf(fs, "      <td>%s</td>\n", url_address);
      fprintf(fs, "      <td>%02d</td>\n", urlListCfg->portNumber);
      fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s'></td>\n", url_address );
      fprintf(fs, "   </tr>\n");
      fflush(fs);	  

      glbEntryCt++;

      /* free urlListCfg */
      cmsObj_free((void **) &urlListCfg);
   }

   fprintf(fs,"</table><br>\n");

   if (glbEntryCt < TOD_URL_FILTER_MAX_ENTRY) 
   {
      fprintf(fs, "<input type='button' onClick='addClick()' value='Add'>\n");
   }

   fprintf(fs, "<input type='button' onClick='removeClick(this.form.rml)' value='Remove'>\n");
   fprintf(fs, "</center>\n");
   fprintf(fs, "</form>\n");
   fprintf(fs, "</blockquote>\n");
   fprintf(fs, "</body>\n");
   fprintf(fs, "</html>\n");
   fflush(fs);
}


void cgiTodRemoveURL(char *query, FILE *fs) 
{
   char *url_address = NULL;
   char lst[WEB_BUF_SIZE_MAX];
   char err_str[BUFLEN_264];
   CmsRet sts = CMSRET_SUCCESS;

   lst[0] = '\0';

   cgiGetValueByName( query, "rmLst", lst );

   for( url_address = strtok( lst, " " ); url_address != NULL; url_address = strtok( NULL, " " ) ) 
   {
      sts = dalUrlFilter_removeEntry( url_address );
      if (sts != CMSRET_SUCCESS ) 
      {
         strcpy( err_str, "Unable to remove." );
         break;
      }
   }

   if ( sts == CMSRET_SUCCESS ) 
   {
      cgiTod_Url_View(fs);
      glbSaveConfigNeeded = TRUE;
   } 
   else
   {
      cgiWriteMessagePage(fs, "Url Filter Entry Remove Error", err_str,
                          "urlfilter.cmd?action=viewcfg");
   }
}


void cgiTodSetURL(char *query, FILE *fs)
{
   char temp_num[BUFLEN_8];
   char url_address[BUFLEN_128];
   unsigned int url_port;
   CmsRet sts = CMSRET_SUCCESS;
   char cmd[WEB_BUF_SIZE_MAX];

   url_address[0]  = '\0';

   cgiGetValueByName(query, "TodUrlAdd", url_address);
   cgiGetValueByName(query, "port_num", temp_num );
   url_port = atoi( temp_num );


   if ((sts = dalUrlFilter_addEntry(url_address, url_port)) != CMSRET_SUCCESS) 
   {
      cmsLog_error("dalUrlFilter_addEntry failed, ret=%d", sts);
      sprintf(cmd, "Configure url filter: url = %s and port number = %u failed", url_address, url_port);
      cgiWriteMessagePage(fs, "Url Filter Entry Add Error", cmd,
 			                            "urlfilter.cmd?action=viewcfg");
      return;
   }
   else 
   {
      glbSaveConfigNeeded = TRUE;
      cgiTod_Url_View(fs);
   }

}

void cgiTodUrlType (char *query, FILE *fs)
{
  char listtype[BUFLEN_16];
  char cmd[BUFLEN_64];

  cgiGetValueByName(query, "listtype", listtype);

   if (dalUrlFilter_setType(listtype) != CMSRET_SUCCESS) 
   {
      cmsLog_error("dalUrlFilter_setType failed");
      sprintf(cmd, "Configure url filter type: %s failed", listtype);
      cgiWriteMessagePage(fs, "Url Filter Type Set Error", cmd,
 			                            "urlfilter.cmd?action=viewcfg");
      return;
   }
   else 
   {
      glbSaveConfigNeeded = TRUE;
      cgiTod_Url_View(fs);
   }
}

void do_urlfilter_cgi(char *query, FILE *fs) 
{
   char action[BUFLEN_256];

   cgiGetValueByName(query, "action", action);

   if ( strcmp(action, "set_url") == 0 )
   {
      cgiTodSetURL(query, fs);
   }
   else if ( strcmp(action, "remove_url") == 0 )
   {
      cgiTodRemoveURL(query, fs);
   }
   else if ( strcmp(action, "save") == 0 )
   {
      cgiTodUrlType(query, fs);
   }
   else
   {
      cgiTod_Url_View(fs);
   }
}

#endif

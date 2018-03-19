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

#ifdef SUPPORT_TOD

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>

#include "cms.h"
#include "httpd.h"
#include "cgi_main.h"
#include "syscall.h"
#include "cms_util.h"
#include "cms_core.h"
#include "cms_dal.h"


extern int glbEntryCt;					// used for web page to know how many entries can be added. In cgimain.c
#define MAX_TOD_ENTRY		   16    // max tod entries.

void writeTodScript(FILE *fs) 
{
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");
                                                                                
   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   fprintf(fs, "function addClick() {\n");
   fprintf(fs, "   var loc = 'todadd.html';\n\n");
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function removeClick(rml) {\n");
   fprintf(fs, "   var lst = '';\n");
   fprintf(fs, "   if (rml.length > 0)\n");
   fprintf(fs, "      for (i = 0; i < rml.length; i++) {\n");
   fprintf(fs, "         if ( rml[i].checked == true )\n");
   fprintf(fs, "            lst += rml[i].value + ', ';\n");
   fprintf(fs, "      }\n");
   fprintf(fs, "   else if ( rml.checked == true )\n");
   fprintf(fs, "      lst = rml.value;\n");
                                                                                
   fprintf(fs, "   var loc = 'todmngr.tod?action=remove&rmLst=' + lst;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");
   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");

}

void cgiTodView( FILE *fs ) 
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   AccessTimeRestrictionObject *todCfg = NULL;
   CmsRet ret = CMSRET_SUCCESS;
   char *tmp = NULL;

   // write header
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
                                                                                
   // write Java Script
   writeTodScript(fs);
                                                                                
   // write body
   fprintf(fs, "</head>\n<body>\n<blockquote>\n");
                                                                                
   fprintf(fs, 
      "<b>Access Time Restriction -- A maximum %d entries can be configured.</b><br><br>\n", MAX_TOD_ENTRY);
   fprintf(fs, "<center>\n");
   fprintf(fs, "<br>\n" );
   fprintf(fs, "<center>\n");
   fprintf(fs, "<form>\n");
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");
   // write table header
   fprintf(fs, "   <tr>\n");
   fprintf(fs, "      <td class='hd'>Username</td>\n");
   fprintf(fs, "      <td class='hd'>MAC</td>\n");
   fprintf(fs, "      <td class='hd'>Mon</td>\n");
   fprintf(fs, "      <td class='hd'>Tue</td>\n");
   fprintf(fs, "      <td class='hd'>Wed</td>\n");
   fprintf(fs, "      <td class='hd'>Thu</td>\n");
   fprintf(fs, "      <td class='hd'>Fri</td>\n");
   fprintf(fs, "      <td class='hd'>Sat</td>\n");
   fprintf(fs, "      <td class='hd'>Sun</td>\n");
   fprintf(fs, "      <td class='hd'>Start</td>\n");
   fprintf(fs, "      <td class='hd'>Stop</td>\n");
   fprintf(fs, "      <td class='hd'>Remove</td>\n");
   fprintf(fs, "   </tr>\n");

   glbEntryCt = 0;

   while ((ret = cmsObj_getNext(MDMOID_ACCESS_TIME_RESTRICTION, &iidStack, (void **) &todCfg)) == CMSRET_SUCCESS)
   {
      fprintf(fs, "    <tr>\n");
      fprintf(fs, "      <td>%s</td>\n", todCfg->username);
      fprintf(fs, "      <td>%s</td>\n", todCfg->MACAddress);
	  
      fprintf(fs, "      <td align='center'>");
      if( (tmp = strstr(todCfg->days, "Mon" )) != NULL )
      {
         fprintf(fs, "x");
      }  
      else
      {
         fprintf(fs, "&nbsp;");   
      }		 	
      fprintf(fs, "</td>\n");		 

      fprintf(fs, "      <td align='center'>");
      if((tmp = strstr(todCfg->days, "Tue" )) != NULL)
      {
         fprintf(fs, "x");
      }
      else
      {
         fprintf(fs, "&nbsp;");   
      }			
      fprintf(fs, "</td>\n");				

      fprintf(fs, "      <td align='center'>");
      if((tmp = strstr(todCfg->days, "Wed" )) != NULL)
      {
         fprintf(fs, "x");
      }
      else
      {
         fprintf(fs, "&nbsp;");   
      }			
      fprintf(fs, "</td>\n");				

      fprintf(fs, "      <td align='center'>");
      if ((tmp = strstr(todCfg->days, "Thu" )) != NULL )
      {
         fprintf(fs, "x");
      }
      else
      {
         fprintf(fs, "&nbsp;");   
      }		
      fprintf(fs, "</td>\n");				

      fprintf(fs, "      <td align='center'>");
      if ((tmp = strstr(todCfg->days, "Fri" )) != NULL )
      {
         fprintf(fs, "x");
      }
      else
      {
         fprintf(fs, "&nbsp;");   
      }			
      fprintf(fs, "</td>\n");				

      fprintf(fs, "      <td align='center'>");
      if ((tmp = strstr(todCfg->days, "Sat" )) != NULL)
      {
         fprintf(fs, "x");
      }
      else
      {
         fprintf(fs, "&nbsp;");   
      }			
      fprintf(fs, "</td>\n");			

      fprintf(fs, "      <td align='center'>");
      if ((tmp = strstr(todCfg->days, "Sun" )) != NULL)
      {
         fprintf(fs, "x");
      }
      else
      {
         fprintf(fs, "&nbsp;");   
      }			
      fprintf(fs, "</td>\n");				

	  
      fprintf(fs, "      <td>%s</td>", todCfg->startTime);
      fprintf(fs, "      <td>%s</td>", todCfg->endTime );
      fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s'></td>\n", todCfg->username );
      fprintf(fs, "  </tr>\n");
      fflush(fs);


      cmsObj_free((void **) &todCfg);

      glbEntryCt++;
   }
  

   fprintf(fs,"</table><br>\n");

   if (glbEntryCt < MAX_TOD_ENTRY) 
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


void cgiTodAdd( char *query, FILE *fs ) 
{
   CmsRet sts = CMSRET_SUCCESS;
   char username[BUFLEN_40];
   char mac[BUFLEN_18];
   unsigned char days;
   unsigned short int start_time;
   unsigned short int end_time;
   char temp_num[BUFLEN_128];
   char cmd[WEB_BUF_SIZE_MAX];

   username[0] = mac[0] = temp_num[0] = '\0';

   cgiGetValueByName(query, "username", username );
   cgiGetValueByName(query, "mac", mac );

   cgiGetValueByName(query, "days", temp_num );

   days = atoi( temp_num );

   cgiGetValueByName(query, "start_time", temp_num );
   start_time = atoi( temp_num );

   cgiGetValueByName(query, "end_time", temp_num );
   end_time = atoi( temp_num );

   sts = dalAccessTimeRestriction_addEntry( username, mac, days, start_time, end_time );
   if ( sts != CMSRET_SUCCESS )
   {
      // If we get here, we've fallen through on an error
      switch( sts ) 
      {
        case CMSRET_INVALID_ARGUMENTS:
          strcpy( cmd, "That rule already exists." );
          break;
        case CMSRET_INVALID_PARAM_VALUE:
	  strcpy( cmd, "A rule with that name already exists." );
          break;
        default:
          strcpy( cmd, "Unable to add rule." );
          break;
      }
      cgiWriteMessagePage(fs, "ToD rule add error", cmd, "todmngr.tod?action=view" );
   }
   else 
   {
      glbSaveConfigNeeded = TRUE;
      cgiTodView(fs);
   }
}

void cgiTodRemove(char *query, FILE *fs) 
{
   char *username = NULL;
   char lst[BUFLEN_1024];
   char err_str[BUFLEN_264];
   CmsRet sts = CMSRET_SUCCESS;

   lst[0] = '\0';

   cgiGetValueByName( query, "rmLst", lst );

   for( username = strtok( lst, ", " ); username != NULL; username = strtok( NULL, ", " ) ) 
   {
      sts = dalAccessTimeRestriction_deleteEntry( username );
      if (sts != CMSRET_SUCCESS) 
      {
         sprintf( err_str, "Unable to remove %s.", username );
         break;
      }
   }

   if( sts != CMSRET_SUCCESS ) 
   {
      cgiWriteMessagePage(fs, "Restriction remove error", err_str, "todmngr.tod?action=view" );
   } 
   else 
   {
      glbSaveConfigNeeded = TRUE;
      cgiTodView( fs );
   }
}

void do_tod_cgi(char *query, FILE *fs) 
{
   char action[BUFLEN_264];

   cgiGetValueByName(query, "action", action );

   if (1) 
   {
      char tmpBuf[BUFLEN_64];

      if((action[0] != '\0') &&(strcmp(action,"view"))) 
      {
         cgiGetValueByName(query, "sessionKey", tmpBuf);
         cgiUrlDecode(tmpBuf);
         cgiSetVar("sessionKey", tmpBuf);

         if(cgiValidateSessionKey(fs) != CMSRET_SUCCESS)
         {
            /*sessionkey validation failed*/
            return;
         }

      }
   }

   if ( strcmp(action, "add") == 0 )
   {
      cgiTodAdd( query, fs );
   }
   else if ( strcmp(action, "remove") == 0 )
   {
      cgiTodRemove( query, fs );
   }
   else
   {
      cgiTodView(fs);
   }
}

#endif /* SUPPORT_TOD */


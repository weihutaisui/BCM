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
#define MAX_DDNS_ENTRY		   16    // max ddns entries.


void writeDDnsCfgScript(FILE *fs, int refresh) {
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   if ( refresh == TRUE ) {
      fprintf(fs, "var code = 'location=\"ddnsmngr.cmd\"';\n");
      fprintf(fs, "eval(code);\n\n");
   }
   fprintf(fs, "function addClick() {\n");
   fprintf(fs, "   var loc = 'ddnsadd.html';\n\n");
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
                                                                                
   fprintf(fs, "   var loc = 'ddnsmngr.cmd?action=remove&rmLst=' + lst;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");
   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");

}

void cgiDDnsView( FILE *fs, int refresh ) {
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   DDnsCfgObject *ddnsCfg = NULL;
   CmsRet ret = CMSRET_SUCCESS;
 
   // write header
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");                                                                   
   // write Java Script
   writeDDnsCfgScript(fs, refresh);
   // write body
   fprintf(fs, "</head>\n<body>\n<blockquote>\n<form>\n");
   fprintf(fs, "<b>Dynamic DNS</b><br><br>\n");
   fprintf(fs, "The Dynamic DNS service allows you to alias a dynamic IP address to \n");
   fprintf(fs, "a static hostname in any of the many domains, allowing your Broadband Router \n");
   fprintf(fs, "to be more easily accessed from various locations on the Internet.<br><br>\n");
   fprintf(fs, "Choose Add or Remove to configure Dynamic DNS.<br><br>\n");
   fprintf(fs, "<center>\n");
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");
   // write table header
   fprintf(fs, "   <tr>\n");
   fprintf(fs, "      <td class='hd'>Hostname</td>\n");
   fprintf(fs, "      <td class='hd'>Username</td>\n");
   fprintf(fs, "      <td class='hd'>Service</td>\n");
   fprintf(fs, "      <td class='hd'>Interface</td>\n");
   fprintf(fs, "      <td class='hd'>Remove</td>\n");
   fprintf(fs, "   </tr>\n");

   glbEntryCt = 0;
   
   while((ret=cmsObj_getNext( MDMOID_D_DNS_CFG, &iidStack, (void **) &ddnsCfg))==CMSRET_SUCCESS){
     fprintf(fs, "    <tr>\n");
     fprintf(fs, "      <td>%s</td>\n", ddnsCfg->fullyQualifiedDomainName);
     fprintf(fs, "      <td>%s</td>\n", ddnsCfg->userName);
     fprintf(fs, "      <td>%s</td>\n", ddnsCfg->providerName);
     fprintf(fs, "      <td>%s</td>\n", ddnsCfg->ifName);
     fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s'></td>\n", ddnsCfg->fullyQualifiedDomainName);
     fprintf(fs, "  </tr>\n");
     cmsObj_free((void **)&ddnsCfg);

     glbEntryCt++;
     	}

   fprintf(fs,"</table><br>\n");
   if ( glbEntryCt<MAX_DDNS_ENTRY) {
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

void cgiDDnsAdd( char *query, FILE *fs ) {
  CmsRet sts = CMSRET_SUCCESS;
  char fullyQualifiedDomainName[BUFLEN_40];
  char userName[BUFLEN_40];
  char password[BUFLEN_24];
  char providerName_str[BUFLEN_16];
  char interface[BUFLEN_16];
  char cmd[WEB_BUF_SIZE_MAX];
  unsigned short int providerName;

  fullyQualifiedDomainName[0] = userName[0] = password[0] = interface[0] = providerName_str[0] = '\0';

  cgiGetValueByName(query, "hostname", fullyQualifiedDomainName );
  cgiGetValueByName(query, "username", userName );
  cgiGetValueByName(query, "password", password );
  cgiGetValueByName(query, "service", providerName_str );
  cgiGetValueByName(query, "iface", interface );

  cmsLog_debug("FQDN=%s username=%s passwd=%s provider=%s intf=%s\n",
               fullyQualifiedDomainName, userName, password,
               providerName_str, interface );

 
  providerName = atoi( providerName_str );
  sts = dalDDns_addEntry( fullyQualifiedDomainName, userName, password, interface, providerName );
   cmsLog_debug("sts = %d", sts);
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
      cgiWriteMessagePage(fs, "DDns service add error", cmd, "ddnsmngr.cmd?action=view" );
   }
   else 
   {
      glbSaveConfigNeeded = TRUE;
      cgiDDnsView(fs,TRUE);
   }
}

void cgiDDnsRemove(char *query, FILE *fs) {
  char *fullyQualifiedDomainName = NULL;
  char lst[WEB_BUF_SIZE_MAX];
  char err_str[BUFLEN_264];
  CmsRet sts = CMSRET_SUCCESS;

  lst[0] = '\0';

  cgiGetValueByName( query, "rmLst", lst );

  for( fullyQualifiedDomainName = strtok( lst, ", " ); fullyQualifiedDomainName != NULL; fullyQualifiedDomainName = strtok( NULL, ", " ) ) {
    sts = dalDDns_deleteEntry( fullyQualifiedDomainName );
    if (sts != CMSRET_SUCCESS ) {
       cmsLog_error("Unable to remove %s.", fullyQualifiedDomainName );
       break;
    }
  }

   if( sts != CMSRET_SUCCESS ) {
      cgiWriteMessagePage(fs, "Restriction remove error", err_str, "ddnsmngr.cmd?action=view" );
   } 
   else 
   {
      glbSaveConfigNeeded = TRUE;
      cgiDDnsView( fs, TRUE );
   }
}

void cgiDDnsMngr(char *query, FILE *fs) {
   char action[BUFLEN_264];
   cgiGetValueByName(query, "action", action );

   if ( cmsUtl_strcmp(action, "add") == 0 )
      cgiDDnsAdd( query, fs );
   else if ( cmsUtl_strcmp(action, "remove" ) == 0 )  
      cgiDDnsRemove( query, fs );
   else
      cgiDDnsView(fs, FALSE);	
}

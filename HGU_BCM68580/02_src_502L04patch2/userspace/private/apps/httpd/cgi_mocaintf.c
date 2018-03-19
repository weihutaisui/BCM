/***********************************************************************
 *
 *  Copyright (c) 2009  Broadcom Corporation
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
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/syslog.h>
#include <net/if_arp.h>
#include <net/route.h>
#include "cgi_cmd.h"
#include "cgi_ntwk.h"
#include "cgi_main.h"
#include "syscall.h"
#include "httpd.h"
#include "cms_util.h"
#include "cgi_main.h"
#include "cms_dal.h"
#include "cgi_util.h"



#if defined(DMP_X_BROADCOM_COM_MOCAWAN_1) || defined(DMP_DEVICE2_MOCA_1)

static void writeMocaCfgScript(FILE *fs, char *addLoc, char *removeLoc)
{

   /* write html header */
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");

   /* show eth label of front/rear panel */
   fprintf(fs, "<script language='javascript' src='portName.js'></script>\n");
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   /* show eth label of front/rear panel */
   fprintf(fs, "var brdId = '%s';\n", glbWebVar.boardID);
   fprintf(fs, "var intfDisp = '';\n");
   fprintf(fs, "var brdIntf = '';\n");

   fprintf(fs, "function addClick() {\n");
   fprintf(fs, "   var code = 'location=\"' + '%s.html' + '\"';\n", addLoc);    /* for atm, cfgatm, ptm, cfgptm */
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

   fprintf(fs, "   var loc = '%s.cmd?action=remove&rmLst=' + lst;\n\n", removeLoc);
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");
   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");

   /* write body title */
   fprintf(fs, "<title></title>\n</head>\n<body>\n<blockquote>\n<form>\n");
   fprintf(fs, "<center>\n");
}

static void cgiMocaWanCfgView(FILE *fs) 
{
   char intfNameBuf[CMS_IFNAME_LENGTH]={0};
   char connModeBuf[BUFLEN_32]={0};
   UBOOL8 isSet = FALSE;
   CmsRet ret;


   /* write Java Script */
   writeMocaCfgScript(fs, "cfgmoca", "mocawan");

   /* write table */
   fprintf(fs, "<b>MoCA WAN Interface Configuration</b><br><br>\n");
   fprintf(fs, "Choose Add, or Remove to configure MoCA WAN interfaces.<br>\n");
   fprintf(fs, "Only one MoCA interface is allowed as WAN interface.<br><br>\n");
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");
   /* write table header */
   fprintf(fs, "   <tr align='center'>\n");
   fprintf(fs, "      <td class='hd'>Interface/(Name)</td>\n");
   fprintf(fs, "      <td class='hd'>Connection Mode</td>\n");
   fprintf(fs, "      <td class='hd'>Remove</td>\n");
   fprintf(fs, "   </tr>\n");


   if ((ret = dalMoca_getWanIntfInfo(intfNameBuf, connModeBuf)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not get mocaWAN object");
   }
   else
   {
      if (!IS_EMPTY_STRING(intfNameBuf))
      {
         fprintf(fs, "   <tr align='center'>\n");
         fprintf(fs, "      <td>%s</td>\n", intfNameBuf);

         fprintf(fs, "      <td>%s</td>\n", connModeBuf);
         
         /* show the remove check box */
         fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s'></td>\n",
                 intfNameBuf);
         fprintf(fs, "   </tr>\n");

         isSet = TRUE; /* yes, a moca wan interface is defined */
      }
   }
   
   fprintf(fs, "</table><br>\n");

   /* For now, only allow one moca layer2 interface */ 
   if (!isSet)
   {
      fprintf(fs, "<input type='button' onClick='addClick()' value='Add'>\n");
   }

   fprintf(fs, "<input type='button' onClick='removeClick(this.form.rml)' value='Remove'>\n");
   fprintf(fs, "</center>\n</form>\n</blockquote>\n</body>\n</html>\n");
   fflush(fs);
}

static CmsRet cgiMocaWanAdd(char *query, FILE *fs) 
{
   char connectionMode[BUFLEN_8];
   
   cgiGetValueByName(query, "ifname",  glbWebVar.wanL2IfName);
   cgiGetValueByName(query, "connMode",  connectionMode);
   glbWebVar.connMode = atoi(connectionMode);

   if (dalMoca_addMocaInterface(&glbWebVar) != CMSRET_SUCCESS)
   {
      do_ej("/webs/ethadderr.html", fs);
      return CMSRET_INTERNAL_ERROR;
   }
   else
   {
      cmsLog_debug("dalMoca_addMocaWanInterface ok, ifName=%s", glbWebVar.wanL2IfName);
      /*
       * ETH add was successful, tell handle_request to save the config
       * before releasing the lock.
       */
      glbSaveConfigNeeded = TRUE;
   }

   return CMSRET_SUCCESS;
   
}

static CmsRet cgiMocaWanRemove(char *query, FILE *fs) 
{
   char *pToken = NULL, *pLast = NULL;
   char lst[BUFLEN_1024];
   CmsRet ret=CMSRET_SUCCESS;
   
   cgiGetValueByName(query, "rmLst", lst);
   pToken = strtok_r(lst, ", ", &pLast);

   while (pToken != NULL)
   {
      strcpy(glbWebVar.wanL2IfName, pToken);
   
      if ((ret = dalMoca_deleteMocaInterface(&glbWebVar)) == CMSRET_REQUEST_DENIED)
      {
         do_ej("/webs/ethdelerr.html", fs);      
         return ret;
      }
      else if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("dalMoca_deleteMocaWanInterface failed for  failed for %s (ret=%d)", glbWebVar.wanL2IfName, ret);
         return ret;
      }      

      pToken = strtok_r(NULL, ", ", &pLast);

   } /* end of while loop over list of connections to delete */

   /*
    * Whether or not there were errors during the delete,
    * save our config.
    */
   glbSaveConfigNeeded = TRUE;

   return ret;
}

void cgiMocaWanCfg(char *query, FILE *fs) 
{
   char action[BUFLEN_256];

   cgiGetValueByName(query, "action", action);
   if (cmsUtl_strcmp(action, "add") == 0)
   {
      if (cgiMocaWanAdd(query, fs) != CMSRET_SUCCESS) 
      {
         return;
      }
   }
   else if (cmsUtl_strcmp(action, "remove") == 0)
   {
      if (cgiMocaWanRemove(query, fs) != CMSRET_SUCCESS)
      {
         return;
      }
   }

   /* for Moca WAN Interface display */
   cgiMocaWanCfgView(fs);
}

void cgiGetAvailableL2MocaIntf(int argc __attribute__((unused)), char **argv __attribute__((unused)), char *varValue) 
{
   NameList *nl, *ifList = NULL;
   int first = 1;

   varValue[0] = '\0';
   if (cmsDal_getAvailableL2MocaIntf(&ifList) != CMSRET_SUCCESS || ifList == NULL)
   {
      return;
   }

   nl = ifList;
   while (nl != NULL)
   {
      if (!first)
         strcat(varValue,"|");
      else
         first = 0;
      strcat(varValue, nl->name);
      nl = nl->next;
   }
   cmsDal_freeNameList(ifList);
}

#endif  /* DMP_X_BROADCOM_COM_MOCAWAN_1 || DEVICE2_MOCA */


#ifdef SUPPORT_MOCA

/* this is a data model independent function */
void cgiPrintMocaList(char *print)
{
   int  wsize; 
   char *p=print;
   char *pToken, *pLast=NULL;
   char ifNameListBuf[CMS_IFNAME_LENGTH*2]={0};

   dalMoca_getIntfNameList(ifNameListBuf, sizeof(ifNameListBuf));
   cmsLog_debug("got intfNameList=%s", ifNameListBuf);

   sprintf(p, "<select size=\"1\" name=\"mocaIfName\" onChange='changeMocaName()'>\n%n", &wsize); 
   p +=wsize;

   pToken = strtok_r(ifNameListBuf, ",", &pLast);

   while (pToken != NULL)
   {
      sprintf(p,"<option value=\"%s\"> %s\n%n", pToken, pToken, &wsize);
      p +=wsize;
      pToken = strtok_r(NULL, ",", &pLast);
   }

   sprintf(p, " </option>\n%n", &wsize); p +=wsize;
   sprintf(p, "  </select>\n%n", &wsize); p +=wsize;
   
}
#endif  /* SUPPORT_MOCA */

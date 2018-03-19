/***********************************************************************
 *
 *  Copyright (c) 2010  Broadcom Corporation
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
#include "sysdiag.h"
#include "httpd.h"
#include "cms_util.h"
#include "cgi_main.h"
#include "cms_dal.h"
#include "cms_boardcmds.h"
#include "cgi_util.h"

#ifdef DMP_X_BROADCOM_COM_GPONWAN_1  

static void writeGponCfgScript(FILE *fs, char *addLoc, char *removeLoc) 
{

   /* write html header */
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");

   fprintf(fs, "<script language='javascript' src='portName.js'></script>\n");
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

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

#ifdef SUPPORT_DM_PURE181
static void cgiGponWanCfgView_dev2(FILE *fs) 
{
    OpticalInterfaceObject *optIntfObj = NULL;
    InstanceIdStack optIntfIid = EMPTY_INSTANCE_ID_STACK;

   /* Find the GPON optical interface config object for the given interface name */
   if (dalOptical_getIntfByIfNameEnabled(GPON_WAN_IF_NAME, &optIntfIid, &optIntfObj, TRUE) == FALSE)
       return;

   fprintf(fs, "   <tr align='center'>\n");

   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n");
   fprintf(fs, "brdIntf = brdId + '|' + '%s';\n", optIntfObj->name);
   fprintf(fs, "intfDisp = getUNameByLName(brdIntf);\n");
   fprintf(fs, "document.write('<td>%s/' + intfDisp + '</td>');\n", optIntfObj->name);
   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");
   fprintf(fs, "      <td>%s</td>\n", MDMVS_VLANMUXMODE);
   /* remove check box */
   fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s'></td>\n", optIntfObj->name);
   fprintf(fs, "   </tr>\n");

   cmsObj_free((void **) &optIntfObj);
}
#else
static void cgiGponWanCfgView_igd(FILE *fs)
{
   InstanceIdStack gponWanIid = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack gponLinkIid = EMPTY_INSTANCE_ID_STACK;   
   WanGponLinkCfgObject *gponLinkCfg = NULL;

   /* Get Gpon WanDevice iidStack first */      
   if (dalGpon_getGponWanIidStatck(&gponWanIid) != CMSRET_SUCCESS)
   {
      return;
   }

   while (cmsObj_getNextInSubTree(MDMOID_WAN_GPON_LINK_CFG, &gponWanIid, &gponLinkIid, (void **)&gponLinkCfg) == CMSRET_SUCCESS)
   {
      if (gponLinkCfg->enable)
      {
         fprintf(fs, "   <tr align='center'>\n");
         
         fprintf(fs, "<script language='javascript'>\n");
         fprintf(fs, "<!-- hide\n");
         fprintf(fs, "brdIntf = brdId + '|' + '%s';\n", gponLinkCfg->ifName);
         fprintf(fs, "intfDisp = getUNameByLName(brdIntf);\n");
         fprintf(fs, "document.write('<td>%s/' + intfDisp + '</td>');\n", gponLinkCfg->ifName);
         fprintf(fs, "// done hiding -->\n");
         fprintf(fs, "</script>\n");
         fprintf(fs, "      <td>%s</td>\n", gponLinkCfg->connectionMode);
         /* remove check box */
         fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s'></td>\n", gponLinkCfg->ifName);
         fprintf(fs, "   </tr>\n");
      }
      cmsObj_free((void **) &gponLinkCfg);
   }
}
#endif

static void cgiGponWanCfgView(FILE *fs) 
{
   /* write Java Script */
   writeGponCfgScript(fs, "cfggpon", "gponwan");

   /* write table */
   fprintf(fs, "<b>GPON WAN Interface Configuration</b><br><br>\n");
   fprintf(fs, "Choose Add, or Remove to configure GPON WAN interfaces.<br>\n");
   fprintf(fs, "Allow one GPON as layer 2 wan interface.<br><br>\n");
   fprintf(fs, "<b>NOTE: Create interfaces in order (example - create %s0 first, then %s1 and so on).</b><br><br>\n",GPON_IFC_STR,GPON_IFC_STR);
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");
   /* write table header */
   fprintf(fs, "   <tr align='center'>\n");
   fprintf(fs, "      <td class='hd'>Interface/(Name)</td>\n");
   fprintf(fs, "      <td class='hd'>Connection Mode</td>\n");
   fprintf(fs, "      <td class='hd'>Remove</td>\n");
   fprintf(fs, "   </tr>\n");

#ifdef SUPPORT_DM_PURE181
    cgiGponWanCfgView_dev2(fs);
#else
    cgiGponWanCfgView_igd(fs);
#endif

   fprintf(fs, "</table><br>\n");

   fprintf(fs, "<input type='button' onClick='addClick()' value='Add'>\n");
   fprintf(fs, "<input type='button' onClick='removeClick(this.form.rml)' value='Remove'>\n");
   
   fprintf(fs, "</center>\n</form>\n</blockquote>\n</body>\n</html>\n");
   fflush(fs);
}

static CmsRet cgiGponWanAdd(char *query, FILE *fs) 
{
   char connectionMode[BUFLEN_8];
   
   cgiGetValueByName(query, "ifname",  glbWebVar.wanL2IfName);
   cgiGetValueByName(query, "connMode",  connectionMode);
   glbWebVar.connMode = atoi(connectionMode);

   if (dalGpon_configGponInterface(&glbWebVar, TRUE) != CMSRET_SUCCESS)
   {
      do_ej("/webs/gponadderr.html", fs);
      return CMSRET_INTERNAL_ERROR;
   }
   else
   {
      cmsLog_debug("cgiGponWanAdd ok.");
      /*
       * GPON add was successful, tell handle_request to save the config
       * before releasing the lock.
       */
      glbSaveConfigNeeded = TRUE;
   }

   return CMSRET_SUCCESS;
   
}

static CmsRet cgiGponWanRemove(char *query, FILE *fs) 
{
   char *pToken = NULL, *pLast = NULL;
   char lst[BUFLEN_1024];
   CmsRet ret=CMSRET_SUCCESS;
   
   cgiGetValueByName(query, "rmLst", lst);
   pToken = strtok_r(lst, ", ", &pLast);

   while (pToken != NULL)
   {
      strcpy(glbWebVar.wanL2IfName, pToken);
   
      if ((ret = dalGpon_configGponInterface(&glbWebVar, FALSE)) == CMSRET_REQUEST_DENIED)
      {
         do_ej("/webs/gpondelerr.html", fs);      
         return ret;
      }
      else if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("cgiGponWanRemove failed for  failed for %s (ret=%d)", glbWebVar.wanL2IfName, ret);
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

void cgiGponWanCfg(char *query, FILE *fs) 
{
   char action[BUFLEN_256];

   cgiGetValueByName(query, "action", action);
   if (cmsUtl_strcmp(action, "add") == 0)
   {
      if (cgiGponWanAdd(query, fs) != CMSRET_SUCCESS) 
      {
         return;
      }
   }
   else if (cmsUtl_strcmp(action, "remove") == 0)
   {
      if (cgiGponWanRemove(query, fs) != CMSRET_SUCCESS)
      {
         return;
      }
   }

   /* for GPON WAN Interface display */
   cgiGponWanCfgView(fs);
}


void cgiGetAvailableL2GponIntf(int argc __attribute__((unused)), char **argv __attribute__((unused)), char *varValue) 
{
   NameList *nl, *ifList = NULL;
   int first = 1;

   varValue[0] = '\0';
   if (cmsDal_getAvailableL2GponIntf(&ifList) != CMSRET_SUCCESS || ifList == NULL)
   {
      cmsLog_error("cmsDal_getAvailableL2GponIntf failed");
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

#endif /* DMP_X_BROADCOM_COM_GPONWAN_1 */


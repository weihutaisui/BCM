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

#ifdef DMP_X_BROADCOM_COM_EPONWAN_1  

static void writeEponCfgScript(FILE *fs, char *addLoc, char *removeLoc) 
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
static void cgiEponWanCfgView_dev2(FILE *fs) 
{
    OpticalInterfaceObject *optIntfObj = NULL;
    InstanceIdStack optIntfIid = EMPTY_INSTANCE_ID_STACK;

   /* Find the EPON optical interface config object for the given interface name */
   if (dalOptical_getIntfByIfNameEnabled(EPON_WAN_IF_NAME, &optIntfIid, &optIntfObj, TRUE) == FALSE)
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
static void cgiEponWanCfgView_igd(FILE *fs) 
{
   InstanceIdStack eponWanIid = EMPTY_INSTANCE_ID_STACK;

   /* Get Epon WanDevice iidStack first */      
   if (dalEpon_getEponWanIidStatck(&eponWanIid) != CMSRET_SUCCESS)
   {
      return;
   }

#ifdef EPON_SFU
   WanEponIntfObject *eponIntf = NULL;
   CmsRet ret;
   if ((ret = cmsObj_get(MDMOID_WAN_EPON_INTF, &eponWanIid, 0, (void **)&eponIntf)) != CMSRET_SUCCESS )
   {
      cmsLog_error("Cannot get WanEponIntfObject, ret = %d", ret);
   }
   else
   {
      if (eponIntf->enable && eponIntf->ifName != NULL)
      {
         fprintf(fs, "   <tr align='center'>\n");

         /* show epon label of front/rear panel */
         fprintf(fs, "<script language='javascript'>\n");
         fprintf(fs, "<!-- hide\n");
         fprintf(fs, "brdIntf = brdId + '|' + '%s';\n", eponIntf->ifName);
         fprintf(fs, "intfDisp = getUNameByLName(brdIntf);\n");
         fprintf(fs, "document.write('<td>%s/' + intfDisp + '</td>');\n", eponIntf->ifName);
         fprintf(fs, "// done hiding -->\n");
         fprintf(fs, "</script>\n");

         fprintf(fs, "      <td>%s</td>\n", eponIntf->connectionMode);
   
         /* remove check box */
         fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s'></td>\n", eponIntf->ifName);
         fprintf(fs, "   </tr>\n");

      }
      /* free eponIntf */
      cmsObj_free((void **) &eponIntf);
    }
#else
    InstanceIdStack eponLinkIid = EMPTY_INSTANCE_ID_STACK;   
    WanEponLinkCfgObject *eponLinkCfg = NULL;

    while (cmsObj_getNextInSubTree(MDMOID_WAN_EPON_LINK_CFG, &eponWanIid, &eponLinkIid, (void **)&eponLinkCfg) == CMSRET_SUCCESS)
    {
       if (eponLinkCfg->enable)
       {
          fprintf(fs, "   <tr align='center'>\n");
         
          fprintf(fs, "<script language='javascript'>\n");
          fprintf(fs, "<!-- hide\n");
          fprintf(fs, "brdIntf = brdId + '|' + '%s';\n", eponLinkCfg->ifName);
          fprintf(fs, "intfDisp = getUNameByLName(brdIntf);\n");
          fprintf(fs, "document.write('<td>%s/' + intfDisp + '</td>');\n", eponLinkCfg->ifName);
          fprintf(fs, "// done hiding -->\n");
          fprintf(fs, "</script>\n");
          fprintf(fs, "      <td>%s</td>\n", eponLinkCfg->connectionMode);
          /* remove check box */
          fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s'></td>\n", eponLinkCfg->ifName);
          fprintf(fs, "   </tr>\n");
       }
       cmsObj_free((void **) &eponLinkCfg);
   }
#endif
}
#endif

static void cgiEponWanCfgView(FILE *fs) 
   {
    /* write Java Script */
    writeEponCfgScript(fs, "cfgepon", "eponwan");

    /* write table */
    fprintf(fs, "<b>EPON WAN Interface Configuration</b><br><br>\n");
    fprintf(fs, "Choose Add, or Remove to configure EPON WAN interfaces.<br>\n");
    fprintf(fs, "Allow one EPON as layer 2 wan interface.<br><br>\n");
    fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");
    /* write table header */
    fprintf(fs, "   <tr align='center'>\n");
    fprintf(fs, "      <td class='hd'>Interface/(Name)</td>\n");
    fprintf(fs, "      <td class='hd'>Connection Mode</td>\n");
    fprintf(fs, "      <td class='hd'>Remove</td>\n");
    fprintf(fs, "   </tr>\n");

#ifdef SUPPORT_DM_PURE181
    cgiEponWanCfgView_dev2(fs);
#else
    cgiEponWanCfgView_igd(fs);
#endif

    fprintf(fs, "</table><br>\n");

    fprintf(fs, "<input type='button' onClick='addClick()' value='Add'>\n");
    fprintf(fs, "<input type='button' onClick='removeClick(this.form.rml)' value='Remove'>\n");

    fprintf(fs, "</center>\n</form>\n</blockquote>\n</body>\n</html>\n");
    fflush(fs);
}

static CmsRet cgiEponWanAdd(char *query, FILE *fs) 
{
   char connectionMode[BUFLEN_8];

   cgiGetValueByName(query, "ifname",  glbWebVar.wanL2IfName);
   cgiGetValueByName(query, "connMode",  connectionMode);
   glbWebVar.connMode = atoi(connectionMode);

   if (dalEpon_addEponInterface(&glbWebVar) != CMSRET_SUCCESS)
   {
      do_ej("/webs/eponadderr.html", fs);
      return CMSRET_INTERNAL_ERROR;
   }
   else
   {
      cmsLog_debug("cgiEponWanAdd ok.");
      /*
       * EPON add was successful, tell handle_request to save the config
       * before releasing the lock.
       */
      glbSaveConfigNeeded = TRUE;
   }

   return CMSRET_SUCCESS;
   
}

static CmsRet cgiEponWanRemove(char *query, FILE *fs) 
{
   char *pToken = NULL, *pLast = NULL;
   char lst[BUFLEN_1024];
   CmsRet ret=CMSRET_SUCCESS;
   
   cgiGetValueByName(query, "rmLst", lst);
   pToken = strtok_r(lst, ", ", &pLast);

   while (pToken != NULL)
   {
      strcpy(glbWebVar.wanL2IfName, pToken);
   
      if ((ret = dalEpon_deleteEponInterface(&glbWebVar)) == CMSRET_REQUEST_DENIED)
      {
         do_ej("/webs/epondelerr.html", fs);      
         return ret;
      }
      else if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("cgiEponWanRemove failed for  failed for %s (ret=%d)", glbWebVar.wanL2IfName, ret);
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

void cgiEponWanCfg(char *query, FILE *fs) 
{
   char action[BUFLEN_256];

   cgiGetValueByName(query, "action", action);
   if (cmsUtl_strcmp(action, "add") == 0)
   {
      if (cgiEponWanAdd(query, fs) != CMSRET_SUCCESS) 
      {
         return;
      }
   }
   else if (cmsUtl_strcmp(action, "remove") == 0)
   {
      if (cgiEponWanRemove(query, fs) != CMSRET_SUCCESS)
      {
         return;
      }
   }

   /* for EPON WAN Interface display */
   cgiEponWanCfgView(fs);
}


void cgiGetAvailableL2EponIntf(int argc __attribute__((unused)), char **argv __attribute__((unused)), char *varValue) 
{
   NameList *nl, *ifList = NULL;
   int first = 1;

   varValue[0] = '\0';
   if (cmsDal_getAvailableL2EponIntf(&ifList) != CMSRET_SUCCESS || ifList == NULL)
   {
      cmsLog_error("cmsDal_getAvailableL2EponIntf failed");
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

#endif /* DMP_X_BROADCOM_COM_EPONWAN_1 */


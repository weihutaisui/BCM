/***********************************************************************
 *
 *  Copyright (c) 2006  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2012:proprietary:standard

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


#ifdef SUPPORT_MODSW_WEBUI

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
#include "cms_msg.h"
#include "cms_msg_modsw.h"
#include "cms_qdm.h"
#include "modsw.h"
#include "osgid.h"
#include "cms_params_modsw.h"


UINT16 SMReqId=0+DU_CHANGE_REQ_ID_WEBUI_BASE;

void cgiModSwEEView(FILE *fs, UBOOL8 refresh);
void cgiModSwEEApply(char *query, FILE *fs, UBOOL8 enable);
void cgiModSwEEInstallCfg(char *query, FILE *fs);
void cgiModSwEEInstallApply(char *query, FILE *fs);
void cgiModSwEEUninstall(char *query, FILE *fs);
void cgiModSwEEUpdateCfg(char *query, FILE *fs);
void cgiModSwEEUpdateApply(char *query, FILE *fs);

void cgiModSwDUInstallSelectEE(FILE *fs);
void cgiModSwDUInstallCfg(char *query, FILE *fs);
void cgiModSwDUInstallApply(char *query, FILE *fs);
void cgiModSwDUUninstall(char *query, FILE *fs);
void cgiModSwDUUpdateCfg(char *query, FILE *fs);
void cgiModSwDUUpdateApply(char *query, FILE *fs);
void cgiModSwDUView(FILE *fs, UBOOL8 refresh);

void cgiModSwEUView(FILE *fs, UBOOL8 refresh);
void cgiModSwEURequestState(char *query, FILE *fs, UBOOL8 Start);
void cgiModSwEUSetAutoStart(char *query, FILE *fs, UBOOL8 autoStart);
void cgiModSwEUExtensionView(char *query, FILE *fs);
void cgiModSwEUPrivilegeAdd(char *query, FILE *fs);
void cgiModSwEUPrivilegeRemove(char *query, FILE *fs);

static void cgiModSw_writeToDebugLog(const char *operation, CmsEntityId destEid);
static void cgiModSw_writeToDebugLogEE(const char *operation,
                                       CmsEntityId destEid, UBOOL8 pass,
                                       const char *errlog);
#ifdef SUPPORT_OSGI_FELIX
static void cgiSendLbCommandToOsgid(void);
#endif

const char *DUActionName[] = {"Noop", "Install", "Update", "Uninstall"};

void cgiModSwEE(char *query, FILE *fs)
{
   char action[BUFLEN_256];

   cgiGetValueByName(query, "action", action);

   if ( cmsUtl_strcmp(action, "Disable") == 0 )
      cgiModSwEEApply(query, fs, FALSE);
   else if ( cmsUtl_strcmp(action, "Enable") == 0 )
      cgiModSwEEApply(query, fs, TRUE);
   else if ( cmsUtl_strcmp(action, "InstallCfg") == 0 )
      cgiModSwEEInstallCfg(query, fs);
   else if ( cmsUtl_strcmp(action, "InstallApply") == 0 )
      cgiModSwEEInstallApply(query, fs);
   else if ( cmsUtl_strcmp(action, "Uninstall") == 0 )
      cgiModSwEEUninstall(query, fs);
   else if ( cmsUtl_strcmp(action, "UpdateCfg") == 0 )
      cgiModSwEEUpdateCfg(query, fs);
   else if ( cmsUtl_strcmp(action, "UpdateApply") == 0 )
      cgiModSwEEUpdateApply(query, fs);
   else if ( cmsUtl_strcmp(action, "view") == 0 )
      cgiModSwEEView(fs, TRUE);
   else
      cgiModSwEEView(fs, TRUE);
}

void cgiModSwEEApply(char *query, FILE *fs, UBOOL8 enable)
{
   CmsRet ret = CMSRET_SUCCESS;
   char fullPath[BUFLEN_256]={0};
   MdmPathDescriptor pathDesc;
   ExecEnvObject *eeObj = NULL;
   char msg[BUFLEN_64];
   
   cgiGetValueByName(query, "fullPath", fullPath);
   
   /* Increase message ID before log message to log file */
   ++SMReqId;

   /* just need to figure the name of the EE to call the lib API */
   /* there is no reason for any of these to fail since WEBUI fix them; it is not a user input */
   cmsMdm_fullPathToPathDescriptor(fullPath, &pathDesc);
   ret = cmsObj_get(pathDesc.oid, &(pathDesc.iidStack), OGF_NO_VALUE_UPDATE,
                    (void **)&eeObj);

   if (ret != CMSRET_SUCCESS)
   {
      if (enable == TRUE)
      {
         cgiModSw_writeToDebugLogEE(SW_MODULES_OPERATION_START,
                                    eeObj->X_BROADCOM_COM_MngrEid, FALSE,
                                    "Could not get ExecEnvObject");
      }
      else
      {
         cgiModSw_writeToDebugLogEE(SW_MODULES_OPERATION_STOP,
                                    eeObj->X_BROADCOM_COM_MngrEid, FALSE,
                                    "Could not get ExecEnvObject");
      }

      sprintf(msg, "Could not get ExecEnvObject, ret=%d", ret);
      cgiWriteMessagePage(fs, "EE Start/Stop Error", msg, "modSwEE.cmd");
      return;
   }

   ret = modsw_setExecEnvEnableLocked(eeObj->name,enable);
   if (ret != CMSRET_SUCCESS)
   {
      if (enable == TRUE)
      {
         cgiModSw_writeToDebugLogEE(SW_MODULES_OPERATION_START,
                                    eeObj->X_BROADCOM_COM_MngrEid, FALSE,
                                    "Start failed, maybe EE is still up");
         sprintf(msg, "Start EE<%s> failed."
                 " Maybe EE is shutting down, please try again", eeObj->name);
      }
      else
      {
         cgiModSw_writeToDebugLogEE(SW_MODULES_OPERATION_STOP,
                                    eeObj->X_BROADCOM_COM_MngrEid, FALSE,
                                    "Stop failed, maybe EE is not up yet");
         sprintf(msg, "Stop EE<%s> failed."
                 " Maybe EE is starting up, please try again", eeObj->name);
      }

      cgiWriteMessagePage(fs, "EE Start/Stop Error", msg, "modSwEE.cmd");
      cmsObj_free((void **)&eeObj);
      return;
   }

   if (enable == TRUE)
   {
      cgiModSw_writeToDebugLogEE(SW_MODULES_OPERATION_START,
                                 eeObj->X_BROADCOM_COM_MngrEid, TRUE, "");
   }
   else
   {
      cgiModSw_writeToDebugLogEE(SW_MODULES_OPERATION_STOP,
                                 eeObj->X_BROADCOM_COM_MngrEid, TRUE, "");
   }

   cmsObj_free((void **)&eeObj);

   glbSaveConfigNeeded = TRUE;
   
   cgiModSwEEView(fs, TRUE);
}

void writeEECfgScript(FILE *fs, UBOOL8 refresh)
{
   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   fprintf(fs, "function InstallClick() {\n");
   fprintf(fs, "   var loc = 'modSwEE.cmd?action=InstallCfg';\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function UpdateClick(eeFullPath) {\n");
   fprintf(fs, "   var loc = 'modSwEE.cmd?action=UpdateCfg&fullPath=' + eeFullPath;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d>';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function UninstallClick(uninstall) {\n");
   fprintf(fs, "   var lst = '';\n");
   fprintf(fs, "   if (uninstall.length > 0)\n");
   fprintf(fs, "      for (i = 0; i < uninstall.length; i++) {\n");
   fprintf(fs, "         if ( uninstall[i].checked == true )\n");
   fprintf(fs, "            lst += uninstall[i].value + ', ';\n");
   fprintf(fs, "      }\n");
   fprintf(fs, "   else if ( uninstall.checked == true )\n");
   fprintf(fs, "      lst = uninstall.value;\n");

   fprintf(fs, "   var loc = 'modSwEE.cmd?action=Uninstall&uninstallLst=' + lst;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function DisableClick(obj, eeFullPath) {\n");
   fprintf(fs, "   var loc = 'modSwEE.cmd?action=Disable&fullPath=' + eeFullPath;\n\n");
   fprintf(fs, "   obj.disabled = true;\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function EnableClick(obj, eeFullPath) {\n");
   fprintf(fs, "   var loc = 'modSwEE.cmd?action=Enable&fullPath=' + eeFullPath;\n\n");
   fprintf(fs, "   obj.disabled = true;\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function EElogClick() {\n");
   fprintf(fs, "   var loc = 'modSwLogEE.cmd?action=view';\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   if(refresh)
   {
      fprintf(fs, "function refresh() {\n");
      fprintf(fs, "   var loc = 'modSwEE.cmd';\n");
      fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
      fprintf(fs, "   eval(code);\n");
      fprintf(fs, "}\n\n");

      fprintf(fs, "function frmLoad() {\n");
      fprintf(fs, "   setTimeout(\"refresh()\",10000);\n");
      fprintf(fs, "}\n\n");
   }
   
   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");
}  /* End of writeEECfgScript() */

void cgiModSwEEView(FILE *fs, UBOOL8 refresh)
{
   char *eeFullPath = NULL;
   MdmPathDescriptor pathDesc;
   ExecEnvObject *ExecEnv=NULL;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   
   /* write html header */
   fprintf(fs, "<html><head>\n");
   //fprintf(fs, "<meta http-equiv='refresh' content='5'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
   fprintf(fs, "<link rel='stylesheet' href='tabletab.css' type='text/css'>\n");

   /* write Java Script */
   writeEECfgScript(fs, refresh);

   /* write body */
   if(refresh)
      fprintf(fs, "<title></title>\n</head>\n<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");
   else
      fprintf(fs, "<title></title>\n</head>\n<body>\n<blockquote>\n<form>\n");
   
   /* write table */
   fprintf(fs, "<center>\n");
   fprintf(fs, "<b>Execution Environment on Broadcom Execution Environment Platform (BEEP)</b><br><br>\n");
   fprintf(fs, "Enable or disable a supported Execution Environment.<br><br>\n");
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");

   /* write table header */
   fprintf(fs, "   <tr align='center'>\n");
   fprintf(fs, "      <td class='hd'>Name</td>\n");
   fprintf(fs, "      <td class='hd'>Alias</td>\n");
   fprintf(fs, "      <td class='hd'>Version</td>\n");
   fprintf(fs, "      <td class='hd'>Type</td>\n");
   fprintf(fs, "      <td class='hd'>Status</td>\n");
   fprintf(fs, "      <td class='hd'>Enabled</td>\n");
   fprintf(fs, "      <td class='hd'>Enable/Disable</td>\n");   
   fprintf(fs, "      <td class='hd'>Uninstall</td>\n");
   fprintf(fs, "      <td class='hd'>Update</td>\n");
   fprintf(fs, "   </tr>\n");

   INIT_INSTANCE_ID_STACK(&iidStack);
   while (cmsObj_getNext(MDMOID_EXEC_ENV, &iidStack, (void **)&ExecEnv) == CMSRET_SUCCESS)
   {
      fprintf(fs, "   <tr align='center'>\n");

      /* Exec Env name */
      fprintf(fs, "      <td>%s</td>\n", ExecEnv->name);

      /* Exec Env alias */
      fprintf(fs, "      <td>%s</td>\n", (ExecEnv->alias ? ExecEnv->alias:"(null)"));

      /* Exec Env version */
      fprintf(fs, "      <td>%s</td>\n", (ExecEnv->version ? ExecEnv->version:"(null)"));

      /* Exec Env Type */
      fprintf(fs, "      <td>%s</td>\n", (ExecEnv->type ? ExecEnv->type:"(null)"));

      /* Exec Env status */
      fprintf(fs, "      <td>%s</td>\n", (ExecEnv->status ? ExecEnv->status:"(Unknown)"));

      /* Is this Exec Env enabled */
      fprintf(fs, "      <td>%s</td>\n", (ExecEnv->enable ? "TRUE":"FALSE"));

      /* get eeFullPath */
      memset(&pathDesc, 0, sizeof(MdmPathDescriptor));
      pathDesc.oid = MDMOID_EXEC_ENV;
      memcpy(&(pathDesc.iidStack), &iidStack, sizeof(InstanceIdStack));
      cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &eeFullPath);
   
      /* ExecEnv Enable/Disable button */
      if(ExecEnv->enable)
      {
         /* Enable/Disable button */
         if (cmsUtl_strcmp(ExecEnv->name, OPENWRTEE_NAME))
         {
            /* Enable/Disable button */
            fprintf(fs, "      <td align='center'><input type='button' onClick='DisableClick(this, \"%s\")' value='Disable'></td>\n",
                    eeFullPath);
         }
         else
         {
            /* Make 'Disable' button be disable if EE name is OPENWRTEE_NAME */
            fprintf(fs, "      <td align='center'><input type='button' disabled='1' onClick='DisableClick(this, \"%s\")' value='Disable'></td>\n",
                    eeFullPath);
         }
      }
      else
      {
         /* Enable/Disable button */
         fprintf(fs, "      <td align='center'><input type='button' onClick='EnableClick(this, \"%s\")' value='Enable'></td>\n",
                 eeFullPath);
      }

      /* Uninstall checkbox and Update button are enabled if EE is NOT OpenWRT */
      if (cmsUtl_strcmp(ExecEnv->name, OPENWRTEE_NAME))
      {
         /* Uninstall button */
         fprintf(fs, "      <td align='center'><input type='checkbox' name='uninstall' value='%s'></td>\n",
                 eeFullPath);

         /* Update button*/
         fprintf(fs, "      <td align='center'><input type='button' onClick='UpdateClick(\"%s\")' value='Update'></td>\n",
                 eeFullPath);
      }
      /* Uninstall checkbox and Update button are disabled if EE is OpenWRT */
      else
      {
         /* Uninstall button */
         fprintf(fs, "      <td align='center'><input type='checkbox' disabled='1' name='uninstall' value='%s'></td>\n",
                 eeFullPath);

         /* Update button*/
         fprintf(fs, "      <td align='center'><input type='button' disabled='1' onClick='UpdateClick(\"%s\")' value='Update'></td>\n",
                 eeFullPath);
      }

      fprintf(fs, "   </tr>\n");

      /* free eeFullPath */
      CMSMEM_FREE_BUF_AND_NULL_PTR(eeFullPath);

      /* free ExecEnvObject */
      cmsObj_free((void **)&ExecEnv);      
   }  
 
   fprintf(fs, "</table><br><br>\n");   

   fprintf(fs, "<input type='button' onClick='InstallClick()' value='Install'>\n");
   
   fprintf(fs, "<input type='button' onClick='UninstallClick(this.form.uninstall)' value='Uninstall'>\n");

   fprintf(fs, "<input type='button' onClick='EElogClick()' value='View Log'>\n");
   
   fprintf(fs, "</center>\n</form>\n</blockquote>\n</body>\n</html>\n");

   fflush(fs);
}

void writeEEInstallCfgScript(FILE *fs)
{
   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   fprintf(fs, "<script language='javascript' src='util.js'></script>\n");
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*
    * btnApply()
    */
   fprintf(fs, "function btnApply() {\n");
   fprintf(fs, "  var loc = 'modSwEE.cmd?action=InstallApply';\n");

   fprintf(fs, "  with ( document.forms[0] ) {\n");

   fprintf(fs, "     // validate URL\n");
   fprintf(fs, "     if (EEurl.value == '') {\n");
   fprintf(fs, "        alert('EE Installation URL is required.');\n");
   fprintf(fs, "        return;\n");
   fprintf(fs, "     };\n");
   
   fprintf(fs, "     if ( EEurl.value.length > 1024 ) {\n");
   fprintf(fs, "        alert('The length of URL (' + EEurl.value.length + ') is too long [0-1024].');\n");
   fprintf(fs, "        return;\n");
   fprintf(fs, "     };\n");
   fprintf(fs, "     loc += '&EEurl=' + encodeUrl(EEurl.value);\n");

   fprintf(fs, "     // validate user name\n");
   fprintf(fs, "     if ( EEusrname.value.length > 256 ) {\n");
   fprintf(fs, "        alert('The length of user name (' + EEusrname.value.length + ') is too long [0-256].');\n");
   fprintf(fs, "        return;\n");
   fprintf(fs, "     };\n");
   fprintf(fs, "     loc += '&EEuser=' + encodeUrl(EEusrname.value);\n");

   fprintf(fs, "     // validate password\n");
   fprintf(fs, "     if ( EEpwd.value.length > 256 ) {\n");
   fprintf(fs, "        alert('The length of password (' + EEpwd.value.length + ') is too long [0-256].');\n");
   fprintf(fs, "        return;\n");
   fprintf(fs, "     };\n");
   fprintf(fs, "     loc += '&EEpwd=' + encodeUrl(EEpwd.value);\n");      
   fprintf(fs, "  }\n");
   
   fprintf(fs, "  loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "  var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "  eval(code);\n");
   fprintf(fs, "}\n\n");   /* End of btnApply() */

   /*
    * btnBack()
    */
   fprintf(fs, "function btnCancel() {\n");
   fprintf(fs, "   var loc = 'modSwEE.cmd?action=view';\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");   /* End of btnCancel() */

   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");
}  /* End of writeEEInstallCfgScript() */

void cgiModSwEEInstallCfg(char *query __attribute__((unused)), FILE *fs)
{
   /* write html header */
   fprintf(fs, "<html>\n  <head>\n");
   fprintf(fs, "    <meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
   fprintf(fs, "    <link rel='stylesheet' href='tabletab.css' type='text/css'>\n");

   /* write Java Script */
   writeEEInstallCfgScript(fs);

   /* write body */
   fprintf(fs, "    <title></title>\n");
   fprintf(fs, "  </head>\n");
   fprintf(fs, "  <body>\n  <blockquote>\n  <form>\n");

   /* write table */
   fprintf(fs, "    <b>Installation EE configuration</b><br><br>\n");
   fprintf(fs, "    <table border='0' cellpadding='0' cellspacing='0'>\n");
   fprintf(fs, "      <tr>\n");
   fprintf(fs, "        <td class='hd'>EE URL:</td>\n");
   fprintf(fs, "        <td><input type='text' size='32' name='EEurl'></td>\n");
   fprintf(fs, "      </tr>\n");
   fprintf(fs, "      <tr>\n");
   fprintf(fs, "        <td class='hd'>EE username:</td>\n");
   fprintf(fs, "        <td><input type='text' size='32' name='EEusrname'></td>\n");
   fprintf(fs, "      </tr>\n");
   fprintf(fs, "      <tr>\n");
   fprintf(fs, "        <td class='hd'>EE password:</td>\n");
   fprintf(fs, "        <td><input type='password' size='32' name='EEpwd'></td>\n");
   fprintf(fs, "      </tr>\n");
   fprintf(fs, "    </table><br><br>\n");

   /* write buttons */
   fprintf(fs, "    <center>\n");
   fprintf(fs, "      <input type='button' onClick='btnApply()' value='Install'>\n");
   fprintf(fs, "      <input type='button' onClick='btnCancel()' value='Cancel'>\n");
   fprintf(fs, "    </center>\n\n");

   fprintf(fs, "  </form>\n  </blockquote>\n  </body>\n</html>\n");

   fflush(fs);
}

void cgiModSwEEInstallApply(char *query, FILE *fs)
{
   CmsRet ret = CMSRET_SUCCESS;
   CmsMsgHeader *reqMsg;
   EErequestStateChangedMsgBody *msgPayload;
   char url[BUFLEN_1024];
   char username[BUFLEN_256];
   char password[BUFLEN_256];

   cgiGetValueByName(query, "EEurl", url);
   cgiGetValueByName(query, "EEuser", username);
   cgiGetValueByName(query, "EEpwd", password);

   cmsLog_debug("web EE operation=install, url=%s, username=%s, password=%s",
                url, username, password);
   
   reqMsg = cmsMem_alloc(sizeof(CmsMsgHeader) + sizeof(EErequestStateChangedMsgBody),
                         ALLOC_ZEROIZE);
   if(reqMsg == NULL)
   {
      char msg[BUFLEN_64];

      sprintf(msg, "Alloc msg memory error.");
      cgiWriteMessagePage(fs, "EE Installation Error", msg, "modSwEE.cmd");
      return;
   }

   /* initialize header fields */
   reqMsg->type = CMS_MSG_REQUEST_EE_STATE_CHANGE;
   reqMsg->src = cmsMsg_getHandleEid(msgHandle);
   reqMsg->dst = EID_SPD;
   reqMsg->flags_request = 1;
   reqMsg->dataLength = sizeof(EErequestStateChangedMsgBody);

   /* fill in the message body */
   msgPayload = (EErequestStateChangedMsgBody *)(reqMsg + 1);

   cmsUtl_strncpy(msgPayload->operation, SW_MODULES_OPERATION_INSTALL, sizeof(msgPayload->operation));
   cmsUtl_strncpy(msgPayload->URL, url, sizeof(msgPayload->URL));
   cmsUtl_strncpy(msgPayload->username, username, sizeof(msgPayload->username));
   cmsUtl_strncpy(msgPayload->password, password, sizeof(msgPayload->password));

   msgPayload->reqId = ++SMReqId;

   if((ret = cmsMsg_send(msgHandle, reqMsg)) != CMSRET_SUCCESS)
   {
      char msg[BUFLEN_64];

      CMSMEM_FREE_BUF_AND_NULL_PTR(reqMsg);
      cmsLog_error("Failed to send message (ret=%d)", ret);
      sprintf(msg, "Error while sending msg to Service Platform Daemon.");
      cgiWriteMessagePage(fs, "EE Installation Error", msg, "modSwEE.cmd");
      return;      
   }
   else
   {
      cmsLog_debug("Sent req message op=%s dstEid=%d",
                    msgPayload->operation, reqMsg->dst);
   }

   cgiModSw_writeToDebugLogEE(SW_MODULES_OPERATION_INSTALL, reqMsg->dst, TRUE, "");

   CMSMEM_FREE_BUF_AND_NULL_PTR(reqMsg);

   cgiModSwEEView(fs, TRUE);
}

CmsRet cgiModSwUninstallEEAction(const char *fullPath)
{
   CmsRet ret = CMSRET_SUCCESS;
   CmsMsgHeader *reqMsg;
   EErequestStateChangedMsgBody *msgPayload;

   reqMsg = cmsMem_alloc(sizeof(CmsMsgHeader) + sizeof(EErequestStateChangedMsgBody),
                         ALLOC_ZEROIZE);
   if(reqMsg == NULL)
   {
      cmsLog_error("Could not allocate memory");
      ret = CMSRET_RESOURCE_EXCEEDED;
   }

   /* initialize header fields */
   reqMsg->type = CMS_MSG_REQUEST_EE_STATE_CHANGE;
   reqMsg->src = cmsMsg_getHandleEid(msgHandle);
   reqMsg->dst = EID_SPD;
   reqMsg->flags_request = 1;
   reqMsg->dataLength = sizeof(EErequestStateChangedMsgBody);

   /* fill in the message body */
   msgPayload = (EErequestStateChangedMsgBody*) (reqMsg + 1);
   cmsUtl_strncpy(msgPayload->operation, SW_MODULES_OPERATION_UNINSTALL, sizeof(msgPayload->operation));
   cmsUtl_strncpy(msgPayload->fullPath, fullPath, sizeof(msgPayload->fullPath));

   msgPayload->reqId = ++SMReqId;
   
   if((ret = cmsMsg_send(msgHandle, reqMsg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to send message (ret=%d)", ret);
   }
   else
   {
      cmsLog_debug("Send %s to %d EE full path %s",
                   msgPayload->operation, reqMsg->dst, msgPayload->fullPath);
   }

   cgiModSw_writeToDebugLogEE(SW_MODULES_OPERATION_UNINSTALL, reqMsg->dst,
                              TRUE, "");

   CMSMEM_FREE_BUF_AND_NULL_PTR(reqMsg);

   return ret;
}

void cgiModSwEEUninstall(char *query, FILE *fs)
{
   char *pToken = NULL, *pLast = NULL;
   char lst[BUFLEN_1024*2];
   CmsRet ret=CMSRET_SUCCESS;
   char fullPath[BUFLEN_256];
   
   cgiGetValueByName(query, "uninstallLst", lst);
   pToken = strtok_r(lst, ", ", &pLast);

   while (pToken != NULL)
   {
      strncpy(fullPath, pToken, BUFLEN_256);

      if (cmsUtl_strcmp(fullPath, "(null)") == 0)
      {
         cmsLog_error("cgiModSwEEUninstall failed for fullPath == NULL");
         pToken = strtok_r(NULL, ", ", &pLast);
         continue;
      }
      
      if ((ret = cgiModSwUninstallEEAction(fullPath)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cgiModSwUninstallEEAction failed for EE fullPath %d (ret=%d)", fullPath, ret);
      }      

      pToken = strtok_r(NULL, ", ", &pLast);

   } /* end of while loop over list of connections to delete */

   cgiModSwEEView(fs, TRUE);
}

void writeEEUpdateCfgScript(FILE *fs)
{
   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   fprintf(fs, "<script language='javascript' src='util.js'></script>\n");
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*
    * btnApply()
    */
   fprintf(fs, "function btnApply(fullPath) {\n");
   fprintf(fs, "  var loc = 'modSwEE.cmd?action=UpdateApply&EEfullPath=' + fullPath;\n");

   fprintf(fs, "  with ( document.forms[0] ) {\n");

   fprintf(fs, "     // validate URL\n");
   fprintf(fs, "     if ( EEurl.value.length > 1024 ) {\n");
   fprintf(fs, "        alert('The length of URL (' + EEurl.value.length + ') is too long [0-1024].');\n");
   fprintf(fs, "        return;\n");
   fprintf(fs, "     };\n");
   fprintf(fs, "     loc += '&EEurl=' + encodeUrl(EEurl.value);\n");

   fprintf(fs, "     // validate user name\n");
   fprintf(fs, "     if ( EEusrname.value.length > 256 ) {\n");
   fprintf(fs, "        alert('The length of user name (' + EEusrname.value.length + ') is too long [0-256].');\n");
   fprintf(fs, "        return;\n");
   fprintf(fs, "     };\n");
   fprintf(fs, "     loc += '&EEuser=' + encodeUrl(EEusrname.value);\n");

   fprintf(fs, "     // validate password\n");
   fprintf(fs, "     if ( EEpwd.value.length > 256 ) {\n");
   fprintf(fs, "        alert('The length of password (' + EEpwd.value.length + ') is too long [0-256].');\n");
   fprintf(fs, "        return;\n");
   fprintf(fs, "     };\n");
   fprintf(fs, "     loc += '&EEpwd=' + encodeUrl(EEpwd.value);\n");      
         
   fprintf(fs, "     // validate version\n");
   fprintf(fs, "     if ( EEver.value.length > 32 ) {\n");
   fprintf(fs, "        alert('The length of version (' + EEver.value.length + ') is too long [0-32].');\n");
   fprintf(fs, "        return;\n");
   fprintf(fs, "     };\n");
   fprintf(fs, "     loc += '&EEver=' + encodeUrl(EEver.value);\n");     
   fprintf(fs, "  };\n");
   
   fprintf(fs, "  loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "  var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "  eval(code);\n");
   fprintf(fs, "}\n\n");   /* End of btnApply() */

   /*
    * btnCancel()
    */
   fprintf(fs, "function btnCancel() {\n");
   fprintf(fs, "  var loc = 'modSwEE.cmd?action=view';\n\n");
   fprintf(fs, "  loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "  var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "  eval(code);\n");
   fprintf(fs, "}\n\n");   /* End of btnCancel() */

   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");
}  /* End of writeEEUpdateCfgScript() */

void cgiModSwEEUpdateCfg(char *query, FILE *fs)
{
   CmsRet ret = CMSRET_SUCCESS;
   char fullPath[BUFLEN_256];
   char msg[BUFLEN_64];
   char version[BUFLEN_32];
   MdmPathDescriptor pathDesc;
   ExecEnvObject *eeObj = NULL;
   
   cgiGetValueByName(query, "fullPath", fullPath);

   if (cmsUtl_strcmp(fullPath, "(null)") == 0)
   {
      sprintf(msg, "Error: EE full path == NULL.");
      cgiWriteMessagePage(fs, "EE Update Error", msg, "modSwEE.cmd");
      return;
   }

   ret = cmsMdm_fullPathToPathDescriptor(fullPath, &pathDesc);
   if (ret != CMSRET_SUCCESS)
   {
      sprintf(msg, "Could not convert %s to its path descriptor, ret=%d", fullPath, ret);
      cgiWriteMessagePage(fs, "EE Update Error", msg, "modSwEE.cmd");
      return;
   }

   ret = cmsObj_get(pathDesc.oid, &(pathDesc.iidStack), OGF_NO_VALUE_UPDATE,
                    (void **)&eeObj);

   if (ret != CMSRET_SUCCESS)
   {
      sprintf(msg, "Could not get ExecEnvObject, ret=%d", ret);
      cgiWriteMessagePage(fs, "EE Update Error", msg, "modSwEE.cmd");
      return;
   }

   cmsUtl_strncpy(version, eeObj->version, sizeof(version));
   cmsObj_free((void **)&eeObj);      

   /* write html header */
   fprintf(fs, "<html>\n  <head>\n");
   fprintf(fs, "    <meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
   fprintf(fs, "    <link rel='stylesheet' href='tabletab.css' type='text/css'>\n");

   /* write Java Script */
   writeEEUpdateCfgScript(fs);

   /* write body */
   fprintf(fs, "    <title></title>\n");
   fprintf(fs, "  </head>\n");
   fprintf(fs, "  <body>\n  <blockquote>\n  <form>\n");

   /* write table */
   fprintf(fs, "    <b>Update EE configuration</b><br><br>\n");
   fprintf(fs, "    <table border='0' cellpadding='0' cellspacing='0'>\n");
   fprintf(fs, "      <tr>\n");
   fprintf(fs, "        <td class='hd'>EE URL:</td>\n");
   fprintf(fs, "        <td><input type='text' size='32' name='EEurl'></td>\n");
   fprintf(fs, "      </tr>\n");
   fprintf(fs, "      <tr>\n");
   fprintf(fs, "        <td class='hd'>EE username:</td>\n");
   fprintf(fs, "        <td><input type='text' size='32' name='EEusrname'></td>\n");
   fprintf(fs, "      </tr>\n");
   fprintf(fs, "      <tr>\n");
   fprintf(fs, "        <td class='hd'>EE password:</td>\n");
   fprintf(fs, "        <td><input type='password' size='32' name='EEpwd'></td>\n");
   fprintf(fs, "      </tr>\n");
   fprintf(fs, "      <tr>\n");
   fprintf(fs, "        <td class='hd'>EE version:</td>\n");
   fprintf(fs, "        <td><input type='text' size='32' name='EEver' value='%s' disabled='1'></td>\n", version);
   fprintf(fs, "      </tr>\n");
   fprintf(fs, "    </table><br><br>\n");

   /* write buttons */
   fprintf(fs, "    <center>\n");
   fprintf(fs, "      <input type='button' onClick='btnApply(\"%s\")' value='Update'>\n", fullPath);
   fprintf(fs, "      <input type='button' onClick='btnCancel()' value='Cancel'>\n");
   fprintf(fs, "    </center>\n\n");

   fprintf(fs, "  </form>\n  </blockquote>\n  </body>\n</html>\n");

   fflush(fs);
}

void cgiModSwEEUpdateApply(char *query, FILE *fs)
{
   CmsRet ret = CMSRET_SUCCESS;
   char fullPath[BUFLEN_256];
   char url[BUFLEN_1024];
   char username[BUFLEN_256];
   char password[BUFLEN_256];
   char version[BUFLEN_32];
   char msg[BUFLEN_64];
   CmsMsgHeader *reqMsg;
   EErequestStateChangedMsgBody *msgPayload;

   cgiGetValueByName(query, "EEfullPath", fullPath);
   cgiGetValueByName(query, "EEurl", url);
   cgiGetValueByName(query, "EEuser", username);
   cgiGetValueByName(query, "EEpwd", password);
   cgiGetValueByName(query, "EEver", version);

   cmsLog_debug("web update fullpath=%s, url=%s, username=%s, password=%s, version=%s",
                fullPath, url, username, password, version);
   
   reqMsg = cmsMem_alloc(sizeof(CmsMsgHeader) + sizeof(EErequestStateChangedMsgBody),
                       ALLOC_ZEROIZE);
   if(reqMsg == NULL)
   {
      sprintf(msg, "Alloc msg memory error.");
      cgiWriteMessagePage(fs, "DU Installation Error", msg, "modSwDU.cmd");
      return;
   }

   /* initialize header fields */
   reqMsg->type = CMS_MSG_REQUEST_EE_STATE_CHANGE;
   reqMsg->src = cmsMsg_getHandleEid(msgHandle);
   reqMsg->dst = EID_SPD;
   reqMsg->flags_request = 1;
   reqMsg->dataLength = sizeof(EErequestStateChangedMsgBody);

   /* copy info into the payload and send message */
   msgPayload = (EErequestStateChangedMsgBody*)(reqMsg + 1);

   strncpy(msgPayload->operation, SW_MODULES_OPERATION_UPDATE, strlen(SW_MODULES_OPERATION_UPDATE));
   strncpy(msgPayload->URL, url, strlen(url));
   strncpy(msgPayload->username, username, strlen(username));
   strncpy(msgPayload->password, password, strlen(password));
   strncpy(msgPayload->version, version, strlen(version));
   strncpy(msgPayload->fullPath, fullPath, strlen(fullPath));

   msgPayload->reqId = ++SMReqId;

   if((ret = cmsMsg_send(msgHandle, reqMsg)) != CMSRET_SUCCESS)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(reqMsg);
      cmsLog_error("Failed to send message (ret=%d)", ret);
      sprintf(msg, "Failed to send message (ret=%d).", ret);
      cgiWriteMessagePage(fs, "EE Update Error", msg, "modSwEE.cmd");
      return;      
   }
   else
   {
      cmsLog_debug("Send %s to %d with full path %s", msgPayload->operation,
                                                      reqMsg->dst, msgPayload->fullPath);
   }

   cgiModSw_writeToDebugLogEE(SW_MODULES_OPERATION_UPDATE, reqMsg->dst,
                              TRUE, "");

   CMSMEM_FREE_BUF_AND_NULL_PTR(reqMsg);

   cgiModSwEEView(fs, TRUE);
}

void cgiModSwDU(char *query, FILE *fs)
{
   char action[BUFLEN_256];

   cgiGetValueByName(query, "action", action);

   if ( cmsUtl_strcmp(action, "InstallSelectEE") == 0 )
      cgiModSwDUInstallSelectEE(fs);
   else if ( cmsUtl_strcmp(action, "InstallCfg") == 0 )
      cgiModSwDUInstallCfg(query, fs);
   else if ( cmsUtl_strcmp(action, "InstallApply") == 0 )
      cgiModSwDUInstallApply(query, fs);
   else if ( cmsUtl_strcmp(action, "Uninstall") == 0 )
      cgiModSwDUUninstall(query, fs);
   else if ( cmsUtl_strcmp(action, "UpdateCfg") == 0 )
      cgiModSwDUUpdateCfg(query, fs);
   else if ( cmsUtl_strcmp(action, "UpdateApply") == 0 )
      cgiModSwDUUpdateApply(query, fs);
   else if ( cmsUtl_strcmp(action, "view") == 0 )
      cgiModSwDUView(fs, FALSE);
   else
      cgiModSwDUView(fs, FALSE);
}

void writeDUCfgScript(FILE *fs, UBOOL8 refresh)
{
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   fprintf(fs, "function installClick() {\n");
   fprintf(fs, "   var loc = 'modSwDU.cmd?action=InstallSelectEE';\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function UpdateClick(DUID) {\n");
   fprintf(fs, "   var loc = 'modSwDU.cmd?action=UpdateCfg&DUID=' + DUID;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function uninstallClick(uninstall) {\n");
   fprintf(fs, "   var lst = '';\n");
   fprintf(fs, "   if (uninstall.length > 0)\n");
   fprintf(fs, "      for (i = 0; i < uninstall.length; i++) {\n");
   fprintf(fs, "         if ( uninstall[i].checked == true )\n");
   fprintf(fs, "            lst += uninstall[i].value + ', ';\n");
   fprintf(fs, "      }\n");
   fprintf(fs, "   else if ( uninstall.checked == true )\n");
   fprintf(fs, "      lst = uninstall.value;\n");

   fprintf(fs, "   var loc = 'modSwDU.cmd?action=Uninstall&uninstallLst=' + lst;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function DUlogClick() {\n");
   fprintf(fs, "   var loc = 'modSwLogDU.cmd?action=view';\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   if(refresh)
   {
      fprintf(fs, "function refresh() {\n");
      fprintf(fs, "   var loc = 'modSwDU.cmd';\n");
      fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
      fprintf(fs, "   eval(code);\n");
      fprintf(fs, "}\n\n");

      fprintf(fs, "function frmLoad() {\n");
      fprintf(fs, "   setTimeout(\"refresh()\",5000);\n");
      fprintf(fs, "}\n\n");
   }
   
   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");
}  /* End of writeDUCfgScript() */

static void getDUDisplayStatus(const char *status,
                               char *str,
                               UINT32 len)
{
   memset(str, 0, len);

   len = ((len-1) > strlen(status)) ? (len-1) : strlen(status);

   if (cmsUtl_strcmp(status, MDMVS_X_BROADCOM_COM_DOWNLOADING) == 0)
   {
      cmsUtl_strncpy(str, "Downloading", len);
   }
   else if (cmsUtl_strcmp(status, MDMVS_X_BROADCOM_COM_DNLDFAILED) == 0)
   {
      cmsUtl_strncpy(str, "Download Failed", len);
   }
   else if (cmsUtl_strcmp(status, MDMVS_X_BROADCOM_COM_DNLDBADFORMAT) == 0)
   {
      cmsUtl_strncpy(str, "Download Bad Format", len);
   }
   else if (cmsUtl_strcmp(status, MDMVS_X_BROADCOM_COM_DNLDCORRUPTED) == 0)
   {
      cmsUtl_strncpy(str, "Download Corrupted", len);
   }
   else if (cmsUtl_strcmp(status, MDMVS_X_BROADCOM_COM_DNLDBADSIGNTR) == 0)
   {
      cmsUtl_strncpy(str, "Download Bad Signature", len);
   }
   else
   {
      cmsUtl_strncpy(str, status, len);
   }
}
 
void cgiModSwDUView(FILE *fs, UBOOL8 refresh)
{
   char duStatus[BUFLEN_64]={0};
   char eeStatus[BUFLEN_64]={0};
   UBOOL8 enable = FALSE;
   DUObject *DU=NULL;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;

#ifdef SUPPORT_OSGI_FELIX
   /* first get an update-- this should be done in stl TODO */
   cgiSendLbCommandToOsgid();
#endif
   
   /* write html header */
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
   fprintf(fs, "<link rel='stylesheet' href='tabletab.css' type='text/css'>\n");

   /* write Java Script */
   writeDUCfgScript(fs, refresh);

   /* write body */
   if(refresh)
      fprintf(fs, "<title></title>\n</head>\n<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");
   else
      fprintf(fs, "<title></title>\n</head>\n<body>\n<blockquote>\n<form>\n");

   /* write table */
   fprintf(fs, "<center>\n");
   fprintf(fs, "<b>Modular Software Deployment Unit (DU)</b><br><br>\n");
   fprintf(fs, "Install, Uninstall or Update Deployment Units.<br><br>\n");
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");

   /* write table header */
   fprintf(fs, "   <tr align='center'>\n");
   fprintf(fs, "      <td class='hd'>Name</td>\n");
   fprintf(fs, "      <td class='hd'>Alias</td>\n");
   fprintf(fs, "      <td class='hd'>UUID</td>\n");
   fprintf(fs, "      <td class='hd'>DUID</td>\n");
   fprintf(fs, "      <td class='hd'>Version</td>\n");
   fprintf(fs, "      <td class='hd'>Description</td>\n");
   fprintf(fs, "      <td class='hd'>Status</td>\n");
   fprintf(fs, "      <td class='hd'>Resolved</td>\n");
   fprintf(fs, "      <td class='hd'>Uninstall</td>\n");
   fprintf(fs, "      <td class='hd'>Update</td>\n");
   fprintf(fs, "   </tr>\n");

   INIT_INSTANCE_ID_STACK(&iidStack);
   while (cmsObj_getNext(MDMOID_DU, &iidStack, (void **)&DU) == CMSRET_SUCCESS)
   {
      fprintf(fs, "   <tr align='center'>\n");

      fprintf(fs, "      <td>%s</td>\n", (DU->name ? DU->name:"(null)"));
      fprintf(fs, "      <td>%s</td>\n", (DU->alias ? DU->alias:"(null)"));

      /* DU UUID */
      fprintf(fs, "      <td>%s</td>\n", (DU->UUID? DU->UUID:"(null)"));

      /* DU DUID */
      fprintf(fs, "      <td>%s</td>\n", (DU->DUID ? DU->DUID:"(null)"));

      fprintf(fs, "      <td>%s</td>\n", (DU->version ? DU->version:"(null)"));
      fprintf(fs, "      <td>%s</td>\n", (DU->description ? DU->description:"(null)"));

      /* DU Status */
      if (DU->status != NULL)
      {
         getDUDisplayStatus(DU->status, duStatus, BUFLEN_64);
         fprintf(fs, "      <td>%s</td>\n", duStatus);
      }
      else
      {
         fprintf(fs, "      <td>(Unknown)</td>\n");
      }

      /* DU Resolved */
      fprintf(fs, "      <td>%s</td>\n", (DU->resolved==TRUE) ? "TRUE" :"FALSE");

      if ((qdmModsw_getExecEnvStatusByFullPathLocked(DU->executionEnvRef, eeStatus, sizeof(eeStatus)) != CMSRET_SUCCESS) ||
          (qdmModsw_getExecEnvEnableByFullPathLocked(DU->executionEnvRef, &enable) != CMSRET_SUCCESS))
      {
         /* Cannot get EE status --> disable Uninstall checkbox */
         fprintf(fs, "      <td align='center'><input type='checkbox' name='uninstall' disabled='1' value='%s'></td>\n",
                 DU->DUID);

         /* Cannot get EE status --> disable Update button */
         fprintf(fs, "      <td align='center'><input type='button' disabled='1' onClick='UpdateClick(\"%s\")' value='Update'></td>\n",
                 DU->DUID);
      }
      else
      {
         if (cmsUtl_strcmp(eeStatus, MDMVS_UP) != 0 || enable == FALSE)
         {
            /* EE status is not UP or disable --> disable Uninstall checkbox */
            fprintf(fs, "      <td align='center'><input type='checkbox' name='uninstall' disabled='1' value='%s'></td>\n",
                    DU->DUID);

            /* EE status is not UP or disables --> disable Update button */
            fprintf(fs, "      <td align='center'><input type='button' disabled='1' onClick='UpdateClick(\"%s\")' value='Update'></td>\n",
                    DU->DUID);
         }
         else
         {
            /* Always allow user to uninstall, regardless of status field, but
             * there must be a DUID */
            fprintf(fs, "      <td align='center'><input type='checkbox' name='uninstall' value='%s'></td>\n",
                    DU->DUID);

            /* DU Update button*/
            if(cmsUtl_strcmp(DU->status,"Installed") == 0)
               fprintf(fs, "      <td align='center'><input type='button' onClick='UpdateClick(\"%s\")' value='Update'></td>\n",
                       DU->DUID);
            else
               fprintf(fs, "      <td align='center'><input type='button' disabled='1' onClick='UpdateClick(\"%s\")' value='Update'></td>\n",
                       DU->DUID);
         }
      }
   
      fprintf(fs, "   </tr>\n");
      cmsObj_free((void **)&DU);      
   }  
     
 
   fprintf(fs, "</table><br><br>\n");

   fprintf(fs, "<input type='button' onClick='installClick()' value='Install'>\n");
   
   fprintf(fs, "<input type='button' onClick='uninstallClick(this.form.uninstall)' value='Uninstall'>\n");

   fprintf(fs, "<input type='button' onClick='DUlogClick()' value='View Log'>\n");
   
   fprintf(fs, "</center>\n</form>\n</blockquote>\n</body>\n</html>\n");

   fflush(fs);
}

void writeDUSelectEEScript(FILE *fs)
{
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   /*
    * btnNext()
    */
   fprintf(fs, "function btnNext() {\n");
   fprintf(fs, "  var loc = 'modSwDU.cmd?action=InstallCfg&';\n");

   fprintf(fs, "  with ( document.forms[0] ) {\n");
   fprintf(fs, "    var idx = eeName.selectedIndex;\n");
   fprintf(fs, "    var ifc = eeName.options[idx].value;\n");
   fprintf(fs, "    loc += 'EEName=' + ifc;\n");
   fprintf(fs, "  }\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "  var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "  eval(code);\n");
   fprintf(fs, "}\n\n");   /* End of btnNext() */

   /*
    * btnBack()
    */
   fprintf(fs, "function btnBack() {\n");
   fprintf(fs, "  var code = 'location=\"' + 'modSwDU.cmd' + '\"';\n");
   fprintf(fs, "  eval(code);\n");
   fprintf(fs, "}\n\n");   /* End of btnBack() */

   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");
}  /* End of writeDUSelectEEScript() */

void cgiModSwDUInstallSelectEE(FILE *fs)
{
   NameList *nl, *ifList = NULL;
   UBOOL8 first = TRUE;

   if (dalModSw_getAvailableEE(&ifList) != CMSRET_SUCCESS)
   {
      char msg[BUFLEN_64];

      sprintf(msg, "Can not get available Execution Environment.");
      cgiWriteMessagePage(fs, "DU Installation Error", msg, "modSwDU.cmd");
      return;
   }
   
   if (ifList == NULL)
   {
      char msg[BUFLEN_64];

      sprintf(msg, "Can not get available Execution Environment.");
      cgiWriteMessagePage(fs, "DU Installation Error", msg, "modSwDU.cmd");
      return;
   }

   /* write html header */
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
   fprintf(fs, "<link rel='stylesheet' href='tabletab.css' type='text/css'>\n");

   /* write Java Script */
   writeDUSelectEEScript(fs);

   /* write body */
   fprintf(fs, "<title></title>\n</head>\n<body>\n<blockquote>\n<form>\n");

   /* write table */
   fprintf(fs, "<center>\n");
   fprintf(fs, "<b>Execution Environment Configuration</b><br><br>\n");
   fprintf(fs, "<p>Select a Exec. Env. for DU installation</p>\n");
   fprintf(fs, "<table border='0' cellpadding='0' cellspacing='0'>\n");
   fprintf(fs, "  <tr>\n");
   fprintf(fs, "  <td><select name='eeName'>\n");

   nl = ifList;   
   while (nl != NULL)
   {      
      if (first)
      {
        fprintf(fs, "    <option value='%s' selected>%s\n", nl->name, nl->name);
        first = FALSE;
      }
      else
      {
         fprintf(fs, "    <option value='%s'>%s\n", nl->name, nl->name);
      }
      
      nl = nl->next;
   }
   cmsDal_freeNameList(ifList);
   
   fprintf(fs, "  </select></td>\n");
   fprintf(fs, "  </tr>\n");
   fprintf(fs, "</table><br><br>\n");

   fprintf(fs, "<input type='button' onClick='btnBack()' value='Back'>\n");
   fprintf(fs, "<input type='button' onClick='btnNext()' value='Next'>\n");
   fprintf(fs, "</center>\n</form>\n</blockquote>\n</body>\n</html>\n");

   fflush(fs);
}

void writeDUInstallCfgScript(FILE *fs)
{
   fprintf(fs, "<script language='javascript' src='util.js'></script>\n");
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   /*
    * btnNext()
    */
   fprintf(fs, "function btnapply() {\n");
   fprintf(fs, "  var loc = 'modSwDU.cmd?action=InstallApply';\n");

   fprintf(fs, "  with ( document.forms[0] ) {\n");

   fprintf(fs, "     // validate URL\n");
   fprintf(fs, "     if (DUurl.value == '') {\n");
   fprintf(fs, "        alert('DU Installation URL is required.');\n");
   fprintf(fs, "        return;\n");
   fprintf(fs, "     };\n");
   
   fprintf(fs, "     if ( DUurl.value.length > 1024 ) {\n");
   fprintf(fs, "        alert('The length of URL (' + DUurl.value.length + ') is too long [0-1024].');\n");
   fprintf(fs, "        return;\n");
   fprintf(fs, "     };\n");
   fprintf(fs, "     loc += '&DUurl=' + encodeUrl(DUurl.value);\n");
   
   fprintf(fs, "     // validate uuid\n");
   fprintf(fs, "     var uuid = DUuuid.value;\n");
   fprintf(fs, "     if ( uuid.length != 0 && uuid.length != 36 ) {\n");
   fprintf(fs, "        alert('The length of uuid (' + uuid.length + ') must be 36 or 0.');\n");
   fprintf(fs, "        return;\n");
   fprintf(fs, "     };\n");
   fprintf(fs, "     if ( (uuid.length != 0) &&\n");
   fprintf(fs, "          (uuid.charAt(8) != '-' || uuid.charAt(13) != '-' ||\n");
   fprintf(fs, "           uuid.charAt(18) != '-' || uuid.charAt(23) != '-') ) {\n");
   fprintf(fs, "        alert('uuid (' + uuid + ') has invalid format. (xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx)');\n");
   fprintf(fs, "        return;\n");
   fprintf(fs, "     };\n");
   fprintf(fs, "     loc += '&DUuuid=' + encodeUrl(uuid);\n");

   fprintf(fs, "     // validate user name\n");
   fprintf(fs, "     if ( DUusrname.value.length > 256 ) {\n");
   fprintf(fs, "        alert('The length of user name (' + DUusrname.value.length + ') is too long [0-256].');\n");
   fprintf(fs, "        return;\n");
   fprintf(fs, "     };\n");
   fprintf(fs, "     loc += '&DUuser=' + encodeUrl(DUusrname.value);\n");

   fprintf(fs, "     // validate password\n");
   fprintf(fs, "     if ( DUpwd.value.length > 256 ) {\n");
   fprintf(fs, "        alert('The length of password (' + DUpwd.value.length + ') is too long [0-256].');\n");
   fprintf(fs, "        return;\n");
   fprintf(fs, "     };\n");
   fprintf(fs, "     loc += '&DUpwd=' + encodeUrl(DUpwd.value);\n");      
         
   //add EE name
   fprintf(fs, "     loc += '&EEName=' + encodeUrl(SelectedEE.value);\n");
   fprintf(fs, "  }\n");
   
   fprintf(fs, "  loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "  var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "  eval(code);\n");
   fprintf(fs, "}\n\n");   /* End of btnapply() */

   /*
    * btnBack()
    */
   fprintf(fs, "function btnBack() {\n");
   fprintf(fs, "   var loc = 'modSwDU.cmd?action=InstallSelectEE';\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");   /* End of btnBack() */

   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");
}  /* End of writeDUInstallCfgScript() */

void cgiModSwDUInstallCfg(char *query, FILE *fs)
{
   char eename[BUFLEN_256];

   cgiGetValueByName(query, "EEName", eename);
   
   /* write html header */
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
   fprintf(fs, "<link rel='stylesheet' href='tabletab.css' type='text/css'>\n");

   /* write Java Script */
   writeDUInstallCfgScript(fs);

   /* write body */
   fprintf(fs, "<title></title>\n</head>\n<body>\n<body>\n<blockquote>\n<form>\n");

   /* write table */
   fprintf(fs, "<b>Installation DU configuration</b><br><br>\n");
   fprintf(fs, "<table border='0' cellpadding='0' cellspacing='0'>\n");
   fprintf(fs, "<tr>\n");
   fprintf(fs, "<td class='hd'>DU URL:&nbsp;</td>\n");
   fprintf(fs, "<td><input type='text' size='32' name='DUurl'></td></tr>\n");

   fprintf(fs, "<tr><td class='hd'>DU UUID:&nbsp;</td>\n");
   fprintf(fs, "<td><input type='text' size='32' name='DUuuid'></td></tr>\n");
   fprintf(fs, "<tr><td class='hd'>DU username:&nbsp;</td>\n");
   fprintf(fs, "<td><input type='text' size='32' name='DUusrname'></td></tr>\n");
   fprintf(fs, "<tr><td class='hd'>DU password:&nbsp;</td>\n");
   fprintf(fs, "<td><input type='password' size='32' name='DUpwd'></td></tr>\n");
   fprintf(fs, "<tr><td class='hd'>Selected EE:&nbsp;</td>\n");
   fprintf(fs, "<td><input type='text' ReadOnly =true size='32' name='SelectedEE' value='%s'></td>\n", eename);
   fprintf(fs, "</tr>\n");
   fprintf(fs, "</table><br><br>\n");

   fprintf(fs, "<center>\n");
   fprintf(fs, "<input type='button' onClick='btnBack()' value='Back'>\n");
   fprintf(fs, "<input type='button' onClick='btnapply()' value='Apply/Install'>\n");
   fprintf(fs, "</center>\n</form>\n</blockquote>\n</body>\n</html>\n");

   fflush(fs);
}

void cgiModSwDUInstallApply(char *query, FILE *fs)
{
   CmsRet ret = CMSRET_SUCCESS;
   CmsMsgHeader *reqMsg;
   unsigned char *body;
   DUrequestStateChangedMsgBody *msgPayload;
   char url[BUFLEN_1024];
   char uuid[BUFLEN_40];
   char username[BUFLEN_256];
   char password[BUFLEN_256];
   char EEName[BUFLEN_256];
   CmsEntityId destEid=EID_INVALID;

   cgiGetValueByName(query, "DUurl", url);
   cgiGetValueByName(query, "DUuuid", uuid);
   cgiGetValueByName(query, "DUuser", username);
   cgiGetValueByName(query, "DUpwd", password);
   cgiGetValueByName(query, "EEName", EEName);

   cmsLog_debug("XXX=>web install url=%s, uuid=%s, username=%s, password=%s, EEname=%s",
      url,uuid,username,password,EEName);

   
   reqMsg = cmsMem_alloc(sizeof(CmsMsgHeader) + sizeof(DUrequestStateChangedMsgBody),
                       ALLOC_ZEROIZE);
   if(reqMsg == NULL)
   {
      char msg[BUFLEN_64];

      sprintf(msg, "Alloc msg memory error.");
      cgiWriteMessagePage(fs, "DU Installation Error", msg, "modSwDU.cmd");
      return;
   }

   /* initialize header fields */
   reqMsg->type = CMS_MSG_REQUEST_DU_STATE_CHANGE;
   reqMsg->src = cmsMsg_getHandleEid(msgHandle);
   reqMsg->flags_event = 1;
   reqMsg->flags_bounceIfNotRunning = 1;
   reqMsg->dataLength = sizeof(DUrequestStateChangedMsgBody);

   /* copy file into the payload and send message */
   body = (unsigned char *)(reqMsg + 1);
   msgPayload = (DUrequestStateChangedMsgBody*)body;

   cmsUtl_strncpy(msgPayload->operation,SW_MODULES_OPERATION_INSTALL,sizeof(msgPayload->operation));
   cmsUtl_strncpy(msgPayload->URL,url,sizeof(msgPayload->URL));
   cmsUtl_strncpy(msgPayload->UUID,uuid,sizeof(msgPayload->UUID));
   cmsUtl_strncpy(msgPayload->username,username,sizeof(msgPayload->username));
   cmsUtl_strncpy(msgPayload->password,password,sizeof(msgPayload->password));

   ret = qdmModsw_getExecEnvFullPathByNameLocked(EEName,
                                     msgPayload->execEnvFullPath,
                                     sizeof(msgPayload->execEnvFullPath));
   if (ret != CMSRET_SUCCESS)
   {
      /* this should not fail since the WebUI presented a drop down list
       * of known EE names, so we must have a good EE name at this point */
      cmsLog_error("Could not get ExecEnvFullPath for %s (ret=%d)", EEName, ret);
   }

   ret = qdmModsw_getMngrEidByExecEnvFullPathLocked(msgPayload->execEnvFullPath,
                                                    &destEid);
   if (ret != CMSRET_SUCCESS)
   {
      /* this also should not fail for the same reason as above. */
      cmsLog_error("Could not get Mngr Eid for %s (ret=%d)",
                    msgPayload->execEnvFullPath, ret);
   }
   else
   {
      reqMsg->dst = destEid;
   }

   msgPayload->reqId = ++SMReqId;

   if((ret = cmsMsg_send(msgHandle, reqMsg)) != CMSRET_SUCCESS)
   {
      char msg[BUFLEN_64];

      CMSMEM_FREE_BUF_AND_NULL_PTR(reqMsg);
      cmsLog_error("Failed to send message (ret=%d)", ret);
      sprintf(msg, "Error while sending msg to Exec Env Mngr.");
      cgiWriteMessagePage(fs, "DU Installation Error", msg, "modSwDU.cmd");
      return;      
   }
   else
   {
      cmsLog_debug("Sent req message op=%s dstEid=%d",
                    msgPayload->operation, reqMsg->dst);
   }
   CMSMEM_FREE_BUF_AND_NULL_PTR(reqMsg);

   cgiModSw_writeToDebugLog("install", destEid);

   cgiModSwDUView(fs, TRUE);
}

CmsRet cgiModSwUninstallDUAction(const char *DUID)
{
   CmsRet ret = CMSRET_SUCCESS;
   CmsMsgHeader *reqMsg;
   DUrequestStateChangedMsgBody *msgPayload;
   DUObject *duObj=NULL;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found = FALSE;
   CmsEntityId destEid = EID_INVALID;

   reqMsg = cmsMem_alloc(sizeof(CmsMsgHeader) + sizeof(DUrequestStateChangedMsgBody),
                         ALLOC_ZEROIZE);
   if(reqMsg == NULL)
   {
      cmsLog_error("Could not allocate memory");
      ret = CMSRET_RESOURCE_EXCEEDED;
   }

   /* initialize header fields */
   reqMsg->type = CMS_MSG_REQUEST_DU_STATE_CHANGE;
   reqMsg->src = cmsMsg_getHandleEid(msgHandle);
   reqMsg->flags_event = 1;
   reqMsg->flags_bounceIfNotRunning = 1;
   reqMsg->dataLength = sizeof(DUrequestStateChangedMsgBody);

   /* fill in the message body, some info must come from DuObj */
   msgPayload = (DUrequestStateChangedMsgBody*) (reqMsg + 1);
   cmsUtl_strncpy(msgPayload->operation,SW_MODULES_OPERATION_UNINSTALL,sizeof(msgPayload->operation));
   msgPayload->reqId = ++SMReqId;

   while (!found &&
          (cmsObj_getNextFlags(MDMOID_DU, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&duObj) == CMSRET_SUCCESS))
   {
      if(cmsUtl_strcmp(duObj->DUID, DUID) == 0)
      {
         found = TRUE;

         if (cmsUtl_strlen(duObj->UUID) == 0)
         {
            cmsUtl_strncpy(msgPayload->UUID, duObj->URL, sizeof(msgPayload->UUID));
         }
         else
         {
            cmsUtl_strncpy(msgPayload->UUID, duObj->UUID, sizeof(msgPayload->UUID));
         }

         if (duObj->version)
         {
            cmsUtl_strncpy(msgPayload->version, duObj->version, sizeof(msgPayload->version));
         }

         ret = qdmModsw_getMngrEidByExecEnvFullPathLocked(duObj->executionEnvRef,
                                                &destEid);
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("could not get mngrEid for execEnvRef=%s duid=%s",
                  duObj->executionEnvRef, duObj->DUID);
         }
         else
         {
            reqMsg->dst = destEid;
         }
      }
      cmsObj_free((void **) &duObj);
   }  

   if(!found)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(reqMsg);
      cmsLog_error("Could not find duid %s", DUID);
      return CMSRET_INTERNAL_ERROR;
   }

   
   if((ret = cmsMsg_send(msgHandle, reqMsg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to send message (ret=%d)", ret);
   }
   else
   {
      cmsLog_debug("Send %s to %d uuid=%s", msgPayload->operation,
                                            reqMsg->dst, msgPayload->UUID);
   }
   CMSMEM_FREE_BUF_AND_NULL_PTR(reqMsg);

   cgiModSw_writeToDebugLog("uninstall", destEid);

   return ret;
}

void cgiModSwDUUninstall(char *query, FILE *fs)
{
   char *pToken = NULL, *pLast = NULL;
   char lst[BUFLEN_1024];
   CmsRet ret=CMSRET_SUCCESS;
   char DUID[BUFLEN_64];
   
   cgiGetValueByName(query, "uninstallLst", lst);
   pToken = strtok_r(lst, ", ", &pLast);

   while (pToken != NULL)
   {
      strncpy(DUID, pToken, BUFLEN_64);

      if (cmsUtl_strcmp(DUID, "(null)") == 0)
      {
         cmsLog_error("cgiModSwDUUninstall failed for DUID == NULL");
         pToken = strtok_r(NULL, ", ", &pLast);
         continue;
      }
      
      if ((ret = cgiModSwUninstallDUAction(DUID)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cgiModSwUninstallDUAction failed for DUID=%d (ret=%d)", atoi(DUID), ret);
      }      

      pToken = strtok_r(NULL, ", ", &pLast);

   } /* end of while loop over list of connections to delete */

   cgiModSwDUView(fs, TRUE);
}

void writeDUUpdateCfgScript(FILE *fs)
{
   fprintf(fs, "<script language='javascript' src='util.js'></script>\n");
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   /*
    * btnNext()
    */
   fprintf(fs, "function btnapply() {\n");
   fprintf(fs, "  var loc = 'modSwDU.cmd?action=UpdateApply';\n");

   fprintf(fs, "  with ( document.forms[0] ) {\n");

   fprintf(fs, "     // validate URL\n");
   fprintf(fs, "     if ( DUurl.value.length > 1024 ) {\n");
   fprintf(fs, "        alert('The length of URL (' + DUurl.value.length + ') is too long [0-1024].');\n");
   fprintf(fs, "        return;\n");
   fprintf(fs, "     };\n");
   fprintf(fs, "     loc += '&DUurl=' + encodeUrl(DUurl.value);\n");
   
   fprintf(fs, "     // validate uuid\n");
   fprintf(fs, "     if ( DUuuid.value.length > 64 ) {\n");
   fprintf(fs, "        alert('The length of uuid (' + DUuuid.value.length + ') is too long [0-64].');\n");
   fprintf(fs, "        return;\n");
   fprintf(fs, "     };\n");
   fprintf(fs, "     loc += '&DUuuid=' + encodeUrl(DUuuid.value);\n");

   fprintf(fs, "     // validate user name\n");
   fprintf(fs, "     if ( DUusrname.value.length > 256 ) {\n");
   fprintf(fs, "        alert('The length of user name (' + DUusrname.value.length + ') is too long [0-256].');\n");
   fprintf(fs, "        return;\n");
   fprintf(fs, "     };\n");
   fprintf(fs, "     loc += '&DUuser=' + encodeUrl(DUusrname.value);\n");

   fprintf(fs, "     // validate password\n");
   fprintf(fs, "     if ( DUpwd.value.length > 256 ) {\n");
   fprintf(fs, "        alert('The length of password (' + DUpwd.value.length + ') is too long [0-256].');\n");
   fprintf(fs, "        return;\n");
   fprintf(fs, "     };\n");
   fprintf(fs, "     loc += '&DUpwd=' + encodeUrl(DUpwd.value);\n");      
         
   fprintf(fs, "     // validate version\n");
   fprintf(fs, "     if ( DUver.value.length > 32 ) {\n");
   fprintf(fs, "        alert('The length of version (' + DUver.value.length + ') is too long [0-32].');\n");
   fprintf(fs, "        return;\n");
   fprintf(fs, "     };\n");
   fprintf(fs, "     loc += '&DUver=' + encodeUrl(DUver.value);\n");     
   fprintf(fs, "  };\n");
   
   fprintf(fs, "  loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "  var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "  eval(code);\n");
   fprintf(fs, "}\n\n");   /* End of btnapply() */

   /*
    * btnCancel()
    */
   fprintf(fs, "function btnCancel() {\n");
   fprintf(fs, "  var code = 'location=\"' + 'modSwDU.cmd' + '\"';\n");
   fprintf(fs, "  eval(code);\n");
   fprintf(fs, "}\n\n");   /* End of btnCancel() */

   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");
}  /* End of writeWanIfcScript() */

void cgiModSwDUUpdateCfg(char *query, FILE *fs)
{
   char DUID[BUFLEN_64];
   DUObject *DU=NULL;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found = FALSE;
   char version[BUFLEN_32];
   char UUID[BUFLEN_40];
   char url[BUFLEN_1024];
   
   cgiGetValueByName(query, "DUID", DUID);

   if (cmsUtl_strcmp(DUID, "(null)") == 0)
   {
      char msg[BUFLEN_64];

      sprintf(msg, "Error: DUID == NULL.");
      cgiWriteMessagePage(fs, "DU Update Error", msg, "modSwDU.cmd");
      return;
   }

   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((!found) &&(cmsObj_getNext(MDMOID_DU, &iidStack, (void **)&DU) == CMSRET_SUCCESS))
   {
      if(cmsUtl_strcmp(DU->DUID,DUID) == 0)
      {
         cmsUtl_strncpy(version, DU->version, BUFLEN_32);
         cmsUtl_strncpy(UUID, DU->UUID, BUFLEN_40);
         cmsUtl_strncpy(url, DU->URL, BUFLEN_1024);
         found = TRUE;
      }
      cmsObj_free((void **)&DU);      
   }  

   if(!found)
   {
      char msg[BUFLEN_64];

      sprintf(msg, "Error: Can't find DU with DUID == %s.", DUID);
      cgiWriteMessagePage(fs, "DU Update Error", msg, "modSwDU.cmd");
      return;
   }
   
   /* write html header */
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
   fprintf(fs, "<link rel='stylesheet' href='tabletab.css' type='text/css'>\n");

   /* write Java Script */
   writeDUUpdateCfgScript(fs);

   /* write body */
   fprintf(fs, "<title></title>\n</head>\n<body>\n<body>\n<blockquote>\n<form>\n");

   /* write table */
   fprintf(fs, "<b>Update DU configuration</b><br><br>\n");
   fprintf(fs, "<table border='0' cellpadding='0' cellspacing='0'>\n");
   fprintf(fs, "<tr>\n");
   fprintf(fs, "<td class='hd'>DU URL:&nbsp;</td>\n");
   fprintf(fs, "<td><input type='text' size='32' name='DUurl' value='%s'></td></tr>\n",url);
   fprintf(fs, "<tr><td class='hd'>DU UUID:&nbsp;</td>\n");
   fprintf(fs, "<td><input type='text' size='32' name='DUuuid' value='%s'></td></tr>\n",UUID);
   fprintf(fs, "<tr><td class='hd'>DU username:&nbsp;</td>\n");
   fprintf(fs, "<td><input type='text' size='32' name='DUusrname'></td></tr>\n");
   fprintf(fs, "<tr><td class='hd'>DU password:&nbsp;</td>\n");
   fprintf(fs, "<td><input type='password' size='32' name='DUpwd'></td></tr>\n");
   fprintf(fs, "<tr><td class='hd'>DU version:&nbsp;</td>\n");
   fprintf(fs, "<td><input type='text' size='32' name='DUver' value='%s'></td>\n", version);
   fprintf(fs, "</tr>\n");
   fprintf(fs, "</table><br><br>\n");

   fprintf(fs, "<center>\n");
   fprintf(fs, "<input type='button' onClick='btnapply()' value='Apply/Update'>\n");
   fprintf(fs, "<input type='button' onClick='btnCancel()' value='Cancel'>\n");
   fprintf(fs, "</center>\n</form>\n</blockquote>\n</body>\n</html>\n");

   fflush(fs);
}

void cgiModSwDUUpdateApply(char *query, FILE *fs)
{
   char url[BUFLEN_1024];
   char uuid[BUFLEN_40];
   char username[BUFLEN_256];
   char password[BUFLEN_256];
   char version[BUFLEN_32];
   char msg[BUFLEN_64];
   CmsRet ret = CMSRET_SUCCESS;
   CmsMsgHeader *reqMsg;
   unsigned char *body;
   DUrequestStateChangedMsgBody *msgPayload;
   DUObject *duObj=NULL;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found = FALSE;
   CmsEntityId destEid = EID_INVALID;

   cgiGetValueByName(query, "DUurl", url);
   cgiGetValueByName(query, "DUuuid", uuid);
   cgiGetValueByName(query, "DUuser", username);
   cgiGetValueByName(query, "DUpwd", password);
   cgiGetValueByName(query, "DUver", version);

   cmsLog_debug("web install url=%s, uuid=%s, username=%s, password=%s, version=%s",
      url,uuid,username,password,version);

   /*TODO: we need to check EEName to decide DU should run on which EE,
                 becasue we only have one EE(osgi) now, we skip the check.*/
   
   reqMsg = cmsMem_alloc(sizeof(CmsMsgHeader) + sizeof(DUrequestStateChangedMsgBody),
                       ALLOC_ZEROIZE);
   if(reqMsg == NULL)
   {
      sprintf(msg, "Alloc msg memory error.");
      cgiWriteMessagePage(fs, "DU Installation Error", msg, "modSwDU.cmd");
      return;
   }

   /* initialize header fields */
   reqMsg->type = CMS_MSG_REQUEST_DU_STATE_CHANGE;
   reqMsg->src = cmsMsg_getHandleEid(msgHandle);
   reqMsg->flags_event = 1;
   reqMsg->flags_bounceIfNotRunning = 1;
   reqMsg->dataLength = sizeof(DUrequestStateChangedMsgBody);

   /* copy file into the payload and send message */
   body = (unsigned char *)(reqMsg + 1);
   msgPayload = (DUrequestStateChangedMsgBody*)body;

   strncpy(msgPayload->operation,SW_MODULES_OPERATION_UPDATE,strlen(SW_MODULES_OPERATION_UPDATE));
   strncpy(msgPayload->URL,url,strlen(url));
   strncpy(msgPayload->UUID,uuid,strlen(uuid));
   strncpy(msgPayload->username,username,strlen(username));
   strncpy(msgPayload->password,password,strlen(password));
   strncpy(msgPayload->version,version,strlen(version));
   msgPayload->reqId = ++SMReqId;

   while (!found &&
          (cmsObj_getNextFlags(MDMOID_DU, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&duObj) == CMSRET_SUCCESS))
   {
      if (cmsUtl_strcmp(duObj->UUID, uuid) == 0)
      {
         found = TRUE;

         cmsUtl_strncpy(msgPayload->UUID,
                        duObj->UUID,
                        sizeof(msgPayload->UUID));

         if (version[0] == '\0' && duObj->version)
         {
            cmsUtl_strncpy(msgPayload->version,
                           duObj->version,
                           sizeof(msgPayload->version));
         }

         cmsUtl_strncpy(msgPayload->execEnvFullPath,
                        duObj->executionEnvRef,
                        sizeof(msgPayload->execEnvFullPath));

         ret = qdmModsw_getMngrEidByExecEnvFullPathLocked(duObj->executionEnvRef,
                                                          &destEid);
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("could not get mngrEid for execEnvRef=%s duid=%s",
                  duObj->executionEnvRef, duObj->DUID);
         }
         else
         {
            reqMsg->dst = destEid;
         }
      }

      cmsObj_free((void **) &duObj);
   }  

   if(!found)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(reqMsg);
      cmsLog_error("Could not find uuid %s", uuid);
      sprintf(msg, "Could not find uuid %s.", uuid);
      cgiWriteMessagePage(fs, "DU Update Error", msg, "modSwDU.cmd");
      return;
   }

   if((ret = cmsMsg_send(msgHandle, reqMsg)) != CMSRET_SUCCESS)
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(reqMsg);
      cmsLog_error("Failed to send message (ret=%d)", ret);
      sprintf(msg, "Failed to send message (ret=%d).", ret);
      cgiWriteMessagePage(fs, "DU Update Error", msg, "modSwDU.cmd");
      return;      
   }
   else
   {
      cmsLog_debug("Send %s to %d uuid=%s", msgPayload->operation,
                                            reqMsg->dst, msgPayload->UUID);
   }

   cgiModSw_writeToDebugLog("update", reqMsg->dst);

   CMSMEM_FREE_BUF_AND_NULL_PTR(reqMsg);

   cgiModSwDUView(fs, TRUE);
}

void cgiModSwEU(char *query, FILE *fs)
{
   char action[BUFLEN_256];

   cgiGetValueByName(query, "action", action);

   if ( cmsUtl_strcmp(action, "modsw-start") == 0 )
      cgiModSwEURequestState(query, fs, TRUE);
   else if ( cmsUtl_strcmp(action, "modsw-stop") == 0 )
      cgiModSwEURequestState(query, fs, FALSE);
   else if ( cmsUtl_strcmp(action, "modsw-autostart") == 0 )
      cgiModSwEUSetAutoStart(query, fs, TRUE);
   else if ( cmsUtl_strcmp(action, "modsw-autostop") == 0 )
      cgiModSwEUSetAutoStart(query, fs, FALSE);
   else if ( cmsUtl_strcmp(action, "modsw-extension") == 0 )
      cgiModSwEUExtensionView(query, fs);
   else if ( cmsUtl_strcmp(action, "modsw-addPrivilege") == 0 )
      cgiModSwEUPrivilegeAdd(query, fs);
   else if ( cmsUtl_strcmp(action, "modsw-removePrivilege") == 0 )
      cgiModSwEUPrivilegeRemove(query, fs);
   else if ( cmsUtl_strcmp(action, "modsw-view") == 0 )
      cgiModSwEUView(fs, FALSE);
   else
      cgiModSwEUView(fs, FALSE);
}

void writeEUCfgScript(FILE *fs, UBOOL8 refresh)
{
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   fprintf(fs, "function stopClick(EUID) {\n");
   fprintf(fs, "   var loc = 'modSwEU.cmd?action=modsw-stop&EUID=' + EUID;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function startClick(EUID) {\n");
   fprintf(fs, "   var loc = 'modSwEU.cmd?action=modsw-start&EUID=' + EUID;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function autostopClick(EUID) {\n");
   fprintf(fs, "   var loc = 'modSwEU.cmd?action=modsw-autostop&EUID=' + EUID;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function autostartClick(EUID) {\n");
   fprintf(fs, "   var loc = 'modSwEU.cmd?action=modsw-autostart&EUID=' + EUID;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function extensionClick(info) {\n");
   fprintf(fs, "   var loc = 'modSwEU.cmd?action=modsw-extension&' + info;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");


   fprintf(fs, "function EUlogClick() {\n");
   fprintf(fs, "   var loc = 'modSwLogEU.cmd?action=view';\n\n");
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   if(refresh)
   {
      fprintf(fs, "function refresh() {\n");
      fprintf(fs, "   var loc = 'modSwEU.cmd?action=view';\n\n");
      fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
      fprintf(fs, "   eval(code);\n");
      fprintf(fs, "}\n\n");

      fprintf(fs, "function frmLoad() {\n");
      fprintf(fs, "   setTimeout(\"refresh()\",5000);\n");
      fprintf(fs, "}\n\n");
   }
   
   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");
}  /* End of writeEUCfgScript() */

void cgiModSwEUView(FILE *fs, UBOOL8 refresh)
{
   char buf[BUFLEN_64]={0};
   char status[BUFLEN_64]={0};
   UBOOL8 enable = FALSE;
   EUObject  *EU=NULL;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   
#ifdef SUPPORT_OSGI_FELIX
   /* first get an update-- this should be done in stl TODO */
   cgiSendLbCommandToOsgid();
#endif

   /* write html header */
   fprintf(fs, "<html><head>\n");
   //fprintf(fs, "<meta http-equiv='refresh' content='5'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
   fprintf(fs, "<link rel='stylesheet' href='tabletab.css' type='text/css'>\n");

   /* write Java Script */
   writeEUCfgScript(fs, refresh);

   /* write body */
   if(refresh)
      fprintf(fs, "<title></title>\n</head>\n<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");
   else
      fprintf(fs, "<title></title>\n</head>\n<body>\n<blockquote>\n<form>\n");
   
   /* write table */
   fprintf(fs, "<center>\n");
   fprintf(fs, "<b>Modular Software Execution Unit (EU)</b><br><br>\n");
   fprintf(fs, "Auto-start, start, or stop Execution Units <br><br>\n");
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");

   /* write table header */
   fprintf(fs, "   <tr align='center'>\n");
   fprintf(fs, "      <td class='hd'>Name</td>\n");
   fprintf(fs, "      <td class='hd'>Alias</td>\n");
   fprintf(fs, "      <td class='hd'>EUID</td>\n");
   fprintf(fs, "      <td class='hd'>Vendor</td>\n");
   fprintf(fs, "      <td class='hd'>Version</td>\n");
   fprintf(fs, "      <td class='hd'>Description</td>\n");
   fprintf(fs, "      <td class='hd'>Status</td>\n");
   fprintf(fs, "      <td class='hd'>Error Code</td>\n");
   fprintf(fs, "      <td class='hd'>Start/Stop</td>\n");
   fprintf(fs, "      <td class='hd'>Auto-start</td>\n");
   fprintf(fs, "      <td class='hd'>Auto-start control</td>\n");
   fprintf(fs, "      <td class='hd'>Extension</td>\n");
   fprintf(fs, "   </tr>\n");

   INIT_INSTANCE_ID_STACK(&iidStack);
   while (cmsObj_getNextFlags(MDMOID_EU, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&EU) == CMSRET_SUCCESS)
   {
      fprintf(fs, "   <tr align='center'>\n");

      fprintf(fs, "      <td>%s</td>\n", (EU->name ? EU->name:"(null)"));
      fprintf(fs, "      <td>%s</td>\n", (EU->alias ? EU->alias:"(null)"));

      /* EUID */
      fprintf(fs, "      <td>%s</td>\n", (EU->EUID ? EU->EUID :"(null)"));

      fprintf(fs, "      <td>%s</td>\n", (EU->vendor ? EU->vendor :"(null)"));
      fprintf(fs, "      <td>%s</td>\n", (EU->version ? EU->version :"(null)"));
      fprintf(fs, "      <td>%s</td>\n", (EU->description ? EU->description :"(null)"));

      /* EU Status */
      fprintf(fs, "      <td>%s</td>\n", (EU->status ? EU->status :"(Unknown)"));

      /* EU Error code */
      fprintf(fs, "      <td>%s</td>\n", (EU->executionFaultCode ? EU->executionFaultCode :"(null)"));

      /* EU Start or Stop button*/
      if ((qdmModsw_getExecEnvStatusByFullPathLocked(EU->executionEnvRef, status, sizeof(status)) != CMSRET_SUCCESS) ||
          (qdmModsw_getExecEnvEnableByFullPathLocked(EU->executionEnvRef, &enable) != CMSRET_SUCCESS))
      {
         /* Cannot get EE status --> disable Start/Stop button */
         if(cmsUtl_strcmp(EU->status,"Idle") == 0)
            fprintf(fs, "      <td align='center'><input type='button' disabled='1' onClick='startClick(\"%s\")' value='Start'></td>\n", 
                    (EU->EUID ? EU->EUID:"(null)"));
         else
            fprintf(fs, "      <td align='center'><input type='button' disabled='1' onClick='stopClick(\"%s\")' value='Stop'></td>\n",
                    (EU->EUID ? EU->EUID:"(null)"));
      }
      else
      {
         if (cmsUtl_strcmp(status, MDMVS_UP) != 0 || enable == FALSE)
         {
            /* EE status is not UP or disable --> disable Start/Stop button */
            if(cmsUtl_strcmp(EU->status,"Idle") == 0)
               fprintf(fs, "      <td align='center'><input type='button' disabled='1' onClick='startClick(\"%s\")' value='Start'></td>\n", 
                       (EU->EUID ? EU->EUID:"(null)"));
            else
               fprintf(fs, "      <td align='center'><input type='button' disabled='1' onClick='stopClick(\"%s\")' value='Stop'></td>\n",
                       (EU->EUID ? EU->EUID:"(null)"));
         }
         else
         {
            /* EE status is UP --> enable Start/Stop button */
            if(cmsUtl_strcmp(EU->status,"Idle") == 0)
               fprintf(fs, "      <td align='center'><input type='button' %s onClick='startClick(\"%s\")' value='Start'></td>\n", 
                  qdmModsw_isDuResolvedByEuIidStackLocked(&iidStack) ? "" : "disabled='1'", (EU->EUID ? EU->EUID:"(null)"));
            else if(cmsUtl_strcmp(EU->status,"Stopping") == 0)
               fprintf(fs, "      <td align='center'><input type='button'  disabled='1' onClick='stopClick(\"%s\")' value='Stop'></td>\n",
                       (EU->EUID ? EU->EUID:"(null)"));
            else
               fprintf(fs, "      <td align='center'><input type='button' onClick='stopClick(\"%s\")' value='Stop'></td>\n",
                        (EU->EUID ? EU->EUID:"(null)"));
         }
      }
      
      /* Auto-start */
      fprintf(fs, "      <td>%s</td>\n", (EU->autoStart ? "TRUE":"FALSE"));

      /* Toggle auto-start setting */
      if(EU->autoStart)
         fprintf(fs, "      <td align='center'><input type='button' onClick='autostopClick(\"%s\")' value='Disable'></td>\n",
                     EU->EUID);
      else
         fprintf(fs, "      <td align='center'><input type='button' onClick='autostartClick(\"%s\")' value='Enable'></td>\n",
                     EU->EUID);

      /* EU Extension button */
      sprintf(buf, "EUID=%s&name=%s", (EU->EUID ? EU->EUID:"(null)"), (EU->name ? EU->name:"(null)"));
      fprintf(fs, "      <td align='center'><input type='button' onClick='extensionClick(\"%s\")' value='Show'></td>\n",
              buf);

      fprintf(fs, "   </tr>\n");
      cmsObj_free((void **)&EU);      
   }  

   fprintf(fs, "</table><br><br>\n");   

   fprintf(fs, "<input type='button' onClick='EUlogClick()' value='View Log'>\n");
    
   fprintf(fs, "</center>\n</form>\n</blockquote>\n</body>\n</html>\n");

   fflush(fs);
}

void cgiModSwEURequestState(char *query, FILE *fs, UBOOL8 Start)
{
   CmsRet ret = CMSRET_SUCCESS;
   char EUID[BUFLEN_64]={0};
   char euFullPath[BUFLEN_256]={0};
   
   cgiGetValueByName(query, "EUID", EUID);

   ret = qdmModsw_getExecUnitFullPathByEuidLocked(EUID, euFullPath, sizeof(euFullPath));
   if(ret != CMSRET_SUCCESS)
   {
      char msg[BUFLEN_64];

      sprintf(msg, "Error: Can't find EU with EUID == %s.", EUID);
      cgiWriteMessagePage(fs, "EU Start/Stop Error", msg, "modSwEU.cmd");
      return;
   }
   
   modsw_setEuRequestedStateLocked(euFullPath,
                                   (Start ? MDMVS_ACTIVE : MDMVS_IDLE));

   cgiModSwEUView(fs, TRUE);
}

void cgiModSwEUSetAutoStart(char *query, FILE *fs, UBOOL8 autoStart)
{
   CmsRet ret = CMSRET_SUCCESS;
   char EUID[BUFLEN_64]={0};
   char euFullPath[BUFLEN_256]={0};

   cgiGetValueByName(query, "EUID", EUID);

   ret = qdmModsw_getExecUnitFullPathByEuidLocked(EUID, euFullPath, sizeof(euFullPath));
   if(ret != CMSRET_SUCCESS)
   {
      char msg[BUFLEN_64];

      sprintf(msg, "Error: Can't find EU with EUID == %s.", EUID);
      cgiWriteMessagePage(fs, "EU Auto-start Error", msg, "modSwEU.cmd");
      return;
   }

   modsw_setEuAutoStartLocked(euFullPath, autoStart);

   glbSaveConfigNeeded = TRUE;

   cgiModSwEUView(fs, TRUE);
}

void cgiModSwEUServiceView(FILE *fs, 
                           InstanceIdStack *pIidStackBus)
{
   UINT32 busId = 0, pathId = 0, interfaceId = 0, id = 0;
   BusObject *objectBus = NULL;
   BusObjectPathObject *objectPath = NULL;
   BusInterfaceObject *objectInterface = NULL;
   BusMethodObject *objectMethod = NULL;
   BusSignalObject *objectSignal = NULL;
   BusPropertyObject *objectProperty = NULL;
   InstanceIdStack iidStackPath = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStackInterface = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStackMethod = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStackSignal = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStackProperty = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;

   /* get bus object */
   ret = cmsObj_get(MDMOID_BUS,
                    pIidStackBus,
                    OGF_NO_VALUE_UPDATE,
                    (void **)&objectBus);

   /* do nothing if application does not have service bus info */
   if (ret != CMSRET_SUCCESS)
   {
      return;
   }
   if (objectBus != NULL &&
       objectBus->name == NULL)
   {
      cmsObj_free((void **) &objectBus);
      return;
   }

   /* write div for layout */
   fprintf(fs, "    <div id='service' class='tabcontent'><center>\n");

   /* write table */
   fprintf(fs, "    <table id='tblSrv' border='1' cellpadding='4' cellspacing='0'>\n");
   fprintf(fs, "      <tbody>\n");
   fprintf(fs, "        <tr data-tt-id='%u'>\n", busId);
   fprintf(fs, "          <td>Bus wellknown name: %s</td>\n", objectBus->name);
   fprintf(fs, "        </tr>\n");
   fprintf(fs, "        <tr data-tt-id='%u' data-tt-parent-id='%u'>\n", ++id, busId);
   fprintf(fs, "          <td>Service Status: %s</td>\n", objectBus->serviceStatus);
   fprintf(fs, "        </tr>\n");

   /* free bus object */
   cmsObj_free((void **) &objectBus);

   /* get object path object */
   while (cmsObj_getNextInSubTreeFlags(MDMOID_BUS_OBJECT_PATH,
                                       pIidStackBus,
                                       &iidStackPath,
                                       OGF_NO_VALUE_UPDATE,
                                       (void **) &objectPath) == CMSRET_SUCCESS)
   {
      pathId = ++id;

      fprintf(fs, "        <tr  data-tt-id='%u' data-tt-parent-id='%u'>\n",
              pathId, busId);
      fprintf(fs, "          <td>Object path: %s</td>\n", objectPath->path);
      fprintf(fs, "        </tr>\n");

      /* get interface object */
      while (cmsObj_getNextInSubTreeFlags(MDMOID_BUS_INTERFACE,
                                          &iidStackPath,
                                          &iidStackInterface,
                                          OGF_NO_VALUE_UPDATE,
                                          (void **) &objectInterface) == CMSRET_SUCCESS)
      {
         interfaceId = ++id;

         fprintf(fs, "        <tr  data-tt-id='%u' data-tt-parent-id='%u'>\n",
                 interfaceId, pathId);
         fprintf(fs, "          <td>Interface: %s</td>\n", objectInterface->name);
         fprintf(fs, "        </tr>\n");

         /* get method object */
         while (cmsObj_getNextInSubTreeFlags(MDMOID_BUS_METHOD,
                                             &iidStackInterface,
                                             &iidStackMethod,
                                             OGF_NO_VALUE_UPDATE,
                                            (void **) &objectMethod) == CMSRET_SUCCESS)
         {
            id++;

            fprintf(fs, "        <tr  data-tt-id='%u' data-tt-parent-id='%u'>\n",
                    id, interfaceId);
            fprintf(fs, "          <td>Method: %s</td>\n", objectMethod->name);
            fprintf(fs, "        </tr>\n");

            /* free method object */
            cmsObj_free((void **) &objectMethod);
         }

         /* get signal object */
         while (cmsObj_getNextInSubTreeFlags(MDMOID_BUS_SIGNAL,
                                             &iidStackInterface,
                                             &iidStackSignal,
                                             OGF_NO_VALUE_UPDATE,
                                             (void **) &objectSignal) == CMSRET_SUCCESS)
         {
            id++;

            fprintf(fs, "        <tr  data-tt-id='%u' data-tt-parent-id='%u'>\n",
                    id, interfaceId);
            fprintf(fs, "          <td>Signal: %s</td>\n", objectSignal->name);
            fprintf(fs, "        </tr>\n");

            /* free signal object */
            cmsObj_free((void **) &objectSignal);
         }

         /* get property object */
         while (cmsObj_getNextInSubTreeFlags(MDMOID_BUS_PROPERTY,
                                             &iidStackInterface,
                                             &iidStackProperty,
                                             OGF_NO_VALUE_UPDATE,
                                             (void **) &objectProperty) == CMSRET_SUCCESS)
         {
            id++;

            fprintf(fs, "        <tr  data-tt-id='%u' data-tt-parent-id='%u'>\n",
                    id, interfaceId);
            fprintf(fs, "          <td>Property: %s</td>\n", objectProperty->name);
            fprintf(fs, "        </tr>\n");

            /* free property object */
            cmsObj_free((void **) &objectProperty);
         }

         /* free interface object */
         cmsObj_free((void **) &objectInterface);
      }

      /* free object path object */
      cmsObj_free((void **) &objectPath);
   }

   /* write end table body */
   fprintf(fs, "      </tbody>\n");
   fprintf(fs, "    </table>\n");

   /* write div for layout */
   fprintf(fs, "    </div>\n");
}

void cgiModSwEUClientView(FILE *fs, 
                          InstanceIdStack *pIidStackBusClient)
{
   const char *nullstr = "(null)";
   BusClientObject *objectBusClient = NULL;
   BusClientPrivilegeObject *objectPrivilege = NULL;
   InstanceIdStack iidStackPrivilege = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;

   /* get bus client object */
   ret = cmsObj_get(MDMOID_BUS_CLIENT,
                    pIidStackBusClient,
                    OGF_NO_VALUE_UPDATE,
                    (void **)&objectBusClient);

   /* do nothing if application does not have service bus info */
   if (ret != CMSRET_SUCCESS)
   {
      return;
   }
   if (objectBusClient != NULL &&
       objectBusClient->name == NULL)
   {
      cmsObj_free((void **) &objectBusClient);
      return;
   }

   /* free bus client object */
   cmsObj_free((void **) &objectBusClient);

   /* write div for layout */
   fprintf(fs, "    <div id='client' class='tabcontent'>\n");
   fprintf(fs, "    <div id='cltTblAdd'><center>\n");

   /* write table */
   fprintf(fs, "    <table id='tblCltAdd' border='1' cellpadding='4' cellspacing='0'>\n");
   fprintf(fs, "      <tbody>\n");
   fprintf(fs, "        <tr>\n");
   fprintf(fs, "          <td class='hd' width='150'>Wellknown Name</td>\n");
   fprintf(fs, "          <td><input type='text' id='wellknownName' style='width: 200'></td>\n");
   fprintf(fs, "        </tr>\n");
   fprintf(fs, "        <tr>\n");
   fprintf(fs, "          <td class='hd'>Object Path</td>\n");
   fprintf(fs, "          <td><input type='text' id='objectPath' style='width: 200'></td>\n");
   fprintf(fs, "        </tr>\n");
   fprintf(fs, "        <tr>\n");
   fprintf(fs, "          <td class='hd'>Interface</td>\n");
   fprintf(fs, "          <td><input type='text' id='interface' style='width: 200'></td>\n");
   fprintf(fs, "        </tr>\n");
   fprintf(fs, "        <tr>\n");
   fprintf(fs, "          <td class='hd'>Member</td>\n");
   fprintf(fs, "          <td><input type='text' id='member' style='width: 200'></td>\n");
   fprintf(fs, "        </tr>\n");
   fprintf(fs, "        <tr>\n");
   fprintf(fs, "          <td class='hd'>Member Type</td>\n");
   fprintf(fs, "          <td><select id='memberType' style='width: 200'>\n");
   fprintf(fs, "                 <option value='Method'>Method\n");
   fprintf(fs, "                 <option value='Signal'>Signal\n");
   fprintf(fs, "              </select></td>\n");
   fprintf(fs, "        </tr>\n");
   fprintf(fs, "      </tbody>\n");
   fprintf(fs, "    </table>\n");
   fprintf(fs, "    <br>\n");

   /* write div for layout */
   fprintf(fs, "    </div>\n");
   fprintf(fs, "    <center><input type='button' onClick='addClick()' value='Add'>\n");
   fprintf(fs, "    <br><br>\n");

   fprintf(fs, "    <div id='cltTable'><center>\n");

   /* write table */
   fprintf(fs, "    <table id='tblClt' border='1' cellpadding='4' cellspacing='0'>\n");
   fprintf(fs, "      <thead>\n");
   fprintf(fs, "        <tr>\n");
   fprintf(fs, "          <th>Wellknown Name</th>\n");
   fprintf(fs, "          <th>Object Path</th>\n");
   fprintf(fs, "          <th>Interface</th>\n");
   fprintf(fs, "          <th>Member</th>\n");
   fprintf(fs, "          <th>Member Type</th>\n");
   fprintf(fs, "          <th>Remove</th>\n");
   fprintf(fs, "        </tr>\n");
   fprintf(fs, "      </thead>\n");

   /* write start table body */
   fprintf(fs, "      <tbody>\n");

   /* get bus client privilege object */
   while (cmsObj_getNextInSubTreeFlags(MDMOID_BUS_CLIENT_PRIVILEGE,
                                       pIidStackBusClient,
                                       &iidStackPrivilege,
                                       OGF_NO_VALUE_UPDATE,
                                       (void **) &objectPrivilege) == CMSRET_SUCCESS)
   {
      fprintf(fs, "        <tr>\n");
      fprintf(fs, "          <td>%s</td>\n", objectPrivilege->wellknownName);
      fprintf(fs, "          <td>%s</td>\n", objectPrivilege->objectPath);
      fprintf(fs, "          <td>%s</td>\n", objectPrivilege->interface);
      fprintf(fs, "          <td>%s</td>\n", objectPrivilege->member);
      fprintf(fs, "          <td>%s</td>\n", objectPrivilege->memberType);
      fprintf(fs, "          <td><input type='checkbox' name='rml' value='%s|%s|%s|%s|%s'></td>\n",
              cmsUtl_strlen(objectPrivilege->wellknownName)? objectPrivilege->wellknownName : nullstr,
              cmsUtl_strlen(objectPrivilege->objectPath)? objectPrivilege->objectPath : nullstr,
              cmsUtl_strlen(objectPrivilege->interface)? objectPrivilege->interface : nullstr,
              cmsUtl_strlen(objectPrivilege->member)? objectPrivilege->member : nullstr,
              cmsUtl_strlen(objectPrivilege->memberType)? objectPrivilege->memberType : nullstr);
      fprintf(fs, "        </tr>\n");

      /* free bus client privilege object */
      cmsObj_free((void **) &objectPrivilege);
   }

   /* write end table body */
   fprintf(fs, "      </tbody>\n");
   fprintf(fs, "    </table>\n");
   fprintf(fs, "    <br>\n");

   /* write div for layout */
   fprintf(fs, "    </div>\n");
   fprintf(fs, "    <center><input type='button' onClick='removeClick(this.form.rml)' value='Remove'>\n");
   fprintf(fs, "    </div>\n");
}

static void writeEuExtensionStyle(FILE *fs)
{
   fprintf(fs, "    <style>\n");
   fprintf(fs, "      table#tblSrv tr:nth-child(even) {\n");
   fprintf(fs, "        background-color: #f2f2f2\n");
   fprintf(fs, "      }\n");
   fprintf(fs, "      table#tblSrv tr:nth-child(odd) {\n");
   fprintf(fs, "        background-color: #f1f1c1\n");
   fprintf(fs, "      }\n");
   fprintf(fs, "      table#tblClt tr:nth-child(even) {\n");
   fprintf(fs, "        background-color: #f2f2f2\n");
   fprintf(fs, "      }\n");
   fprintf(fs, "      table#tblClt tr:nth-child(odd) {\n");
   fprintf(fs, "        background-color: #f1f1c1\n");
   fprintf(fs, "      }\n");
   fprintf(fs, "      table#tblClt th {\n");
   fprintf(fs, "        border: 1px solid #888;\n");
   fprintf(fs, "        font-size: x-small;\n");
   fprintf(fs, "        font-weight: bold;\n");
   fprintf(fs, "        font-family: 'tahoma', 'sans-serif', 'arial', 'helvetica';\n");
   fprintf(fs, "        padding: .3em 1em .1em 1em;\n");
   fprintf(fs, "        text-align: center;\n");
   fprintf(fs, "        background-color: #bbb\n");
   fprintf(fs, "      }\n");
   fprintf(fs, "    </style>\n");
}

static void writeEuExtensionScript(FILE *fs, const char *EUID, const char *name)
{
   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   fprintf(fs, "    <script>\n");
   fprintf(fs, "      $(document).ready(function() {\n");
   fprintf(fs, "        var tablinks = document.getElementsByClassName('tablinks');\n\n");
   fprintf(fs, "        tablinks[0].className += ' active';\n");
   fprintf(fs, "        document.getElementById('client').style.display = 'block';\n");
   fprintf(fs, "      });\n\n");

   fprintf(fs, "      function openTab(evt, tabName) {\n");
   fprintf(fs, "        var i, tabcontent, tablinks;\n\n");
   fprintf(fs, "        tabcontent = document.getElementsByClassName('tabcontent');\n\n");
   fprintf(fs, "        for (i = 0; i < tabcontent.length; i++) {\n");
   fprintf(fs, "          tabcontent[i].style.display = 'none';\n");
   fprintf(fs, "        }\n\n");
   fprintf(fs, "        tablinks = document.getElementsByClassName('tablinks');\n\n");
   fprintf(fs, "        for (i = 0; i < tablinks.length; i++) {\n");
   fprintf(fs, "          tablinks[i].className = tablinks[i].className.replace(' active', '');\n");
   fprintf(fs, "        }\n\n");
   fprintf(fs, "        document.getElementById(tabName).style.display = 'block';\n");
   fprintf(fs, "        evt.currentTarget.className += ' active';\n\n");
   fprintf(fs, "        if (tabName == 'service')\n");
   fprintf(fs, "        {\n");
   fprintf(fs, "          $('#tblSrv').treetable({ expandable: true });\n");
   fprintf(fs, "          $('#tblSrv').treetable('expandAll');\n");
   fprintf(fs, "        }\n");
   fprintf(fs, "      }\n\n");

   fprintf(fs, "      function addClick() {\n");
   fprintf(fs, "        var loc = 'modSwEU.cmd?action=modsw-addPrivilege&EUID=%s&name=%s';\n", EUID, name);
   fprintf(fs, "        var name = $('#wellknownName').val();\n");
   fprintf(fs, "        var path = $('#objectPath').val();\n");
   fprintf(fs, "        var interface = $('#interface').val();\n");
   fprintf(fs, "        var member = $('#member').val();\n");
   fprintf(fs, "        var memberType = $('#memberType').val();\n\n");
   fprintf(fs, "        if (interface != '' && name == '')\n");
   fprintf(fs, "          alert('interface ' + interface + ' has its wellknown name empty.');\n");
   fprintf(fs, "        else if (member != '' && interface == '')\n");
   fprintf(fs, "          alert('member ' + member + ' has its interface empty.');\n");
   fprintf(fs, "        else if (member != '' && memberType == '')\n");
   fprintf(fs, "          alert('member ' + member + ' has its type empty.');\n");
   fprintf(fs, "        else if (name == '' && path == '' &&\n");
   fprintf(fs, "                 interface == '' && member == '')\n");
   fprintf(fs, "          alert('Name, object path, interface, and member cannot be all empty.');\n");
   fprintf(fs, "        else\n");
   fprintf(fs, "        {\n");
   fprintf(fs, "          loc += '&wellknownName=' + name + '&path=' + path + '&interface=' + interface;\n");
   fprintf(fs, "          loc += '&member=' + member + '&type=' + memberType;\n");
   fprintf(fs, "          loc += '&sessionKey=%d';\n", glbCurrSessionKey);
   fprintf(fs, "          var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "          eval(code);\n");
   fprintf(fs, "        }\n");
   fprintf(fs, "      }\n\n");

   fprintf(fs, "      function removeClick(rml) {\n");
   fprintf(fs, "        var lst = '';\n");
   fprintf(fs, "        if (rml.length > 0)\n");
   fprintf(fs, "          for (i = 0; i < rml.length; i++) {\n");
   fprintf(fs, "            if ( rml[i].checked == true )\n");
   fprintf(fs, "              lst += rml[i].value + ',';\n");
   fprintf(fs, "          }\n");
   fprintf(fs, "        else if ( rml.checked == true )\n");
   fprintf(fs, "          lst = rml.value;\n\n");
   fprintf(fs, "        var loc = 'modSwEU.cmd?action=modsw-removePrivilege';\n\n");
   fprintf(fs, "        loc += '&EUID=%s&name=%s';\n", EUID, name);
   fprintf(fs, "        loc += '&rmLst=' + lst;\n");
   fprintf(fs, "        loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "        var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "        eval(code);\n");
   fprintf(fs, "      }\n\n");
   fprintf(fs, "    </script>\n");
}

void cgiModSwEUExtensionView(char *query, FILE *fs)
{
   char EUID[BUFLEN_64]={0};
   char name[BUFLEN_64]={0};
   char euFullPath[BUFLEN_256]={0};
   char msg[BUFLEN_64]={0};
   MdmPathDescriptor pathDesc;
   CmsRet ret = CMSRET_SUCCESS;

   cgiGetValueByName(query, "EUID", EUID);
   cgiGetValueByName(query, "name", name);

   /* get EU fullpath by EUID */
   ret = qdmModsw_getExecUnitFullPathByEuidLocked(EUID, euFullPath, sizeof(euFullPath));
   if(ret != CMSRET_SUCCESS)
   {

      sprintf(msg, "Error: Can't find EU with EUID == %s.", EUID);
      cgiWriteMessagePage(fs, "EU Extension Error", msg, "modSwEU.cmd");
      return;
   }

   /* iidStack of bus object has same value of iidStack of EU */
   ret = cmsMdm_fullPathToPathDescriptor(euFullPath, &pathDesc);
   if (ret != CMSRET_SUCCESS)
   {
      sprintf(msg,"Error: Could not convert %s to path descriptor, return %d",
              euFullPath, ret);
      cgiWriteMessagePage(fs, "EU Extension Error", msg, "modSwEU.cmd");
      return;
   }

   /* write html header */
   fprintf(fs, "<html>\n");
   fprintf(fs, "  <head>\n");
   fprintf(fs, "    <meta charset='utf-8'>\n");
   fprintf(fs, "    <title>Execution Unit Extension</title>\n");
   fprintf(fs, "    <link rel='stylesheet' href='tabletab.css' type='text/css'>\n");
   fprintf(fs, "    <link rel='stylesheet' href='treetable/css/jquery.treetable.css' />\n");
   fprintf(fs, "    <link rel='stylesheet' href='treetable/css/jquery.treetable.theme.default.css' />\n");
   fprintf(fs, "    <script src='jquery.js'></script>\n");
   fprintf(fs, "    <script src='treetable/jquery.treetable.js'></script>\n");

   /* write HTML5 style */
   writeEuExtensionStyle(fs);

   /* write Java Script */
   writeEuExtensionScript(fs, EUID, name);

   fprintf(fs, "  </head>\n");

   /* write start body */
   fprintf(fs, "  <body>\n");
   fprintf(fs, "  <form>\n");
   fprintf(fs, "    <center><h3>Execution Unit Extension %s</h3><br>\n", name);
   
   /* write tab bar */
   fprintf(fs, "    <ul class='tab'>\n");
   fprintf(fs, "      <li><a href='#' class='tablinks' onclick=\"openTab(event, 'client')\">Client</a></li>\n");
   fprintf(fs, "      <li><a href='#' class='tablinks' onclick=\"openTab(event, 'service')\">Service</a></li>\n");
   fprintf(fs, "    </ul>\n");
   fprintf(fs, "    <br>\n");
   
   /* write client information */
   cgiModSwEUClientView(fs, &(pathDesc.iidStack));

   /* write service information */
   cgiModSwEUServiceView(fs, &(pathDesc.iidStack));

   /* write end body */
   fprintf(fs, "  </form>\n");
   fprintf(fs, "  </body>\n");
   fprintf(fs, "</html>\n");

   fflush(fs);
}

void cgiModSwEUPrivilegeAdd(char *query, FILE *fs)
{
   char EUID[BUFLEN_64]={0};
   char euFullPath[BUFLEN_256]={0};
   char name[BUFLEN_64]={0};
   char path[BUFLEN_64]={0};
   char interface[BUFLEN_128]={0};
   char member[BUFLEN_64]={0};
   char type[BUFLEN_32]={0};
   char msg[BUFLEN_64]={0};
   MdmPathDescriptor pathDesc;
   BusClientPrivilegeObject *clientPrivilege = NULL;
   InstanceIdStack iidStackClient = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStackClientPrivilege = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;

   cgiGetValueByName(query, "EUID", EUID);

   /* get EU fullpath by EUID */
   ret = qdmModsw_getExecUnitFullPathByEuidLocked(EUID, euFullPath, sizeof(euFullPath));
   if(ret != CMSRET_SUCCESS)
   {

      sprintf(msg, "Error: Can't find EU with EUID == %s.", EUID);
      cgiWriteMessagePage(fs, "EU Extension Error", msg, "modSwEU.cmd");
      return;
   }

   /* get iidStack of EU */
   ret = cmsMdm_fullPathToPathDescriptor(euFullPath, &pathDesc);
   if (ret != CMSRET_SUCCESS)
   {
      sprintf(msg, "Error: Could not convert %s to path descriptor, return %d.",
              euFullPath, ret);
      cgiWriteMessagePage(fs, "EU Extension Error", msg, "modSwEU.cmd");
      return;
   }

   /* iidStack of bus client object has same value of iidStack of EU */
   memcpy(&iidStackClient, &(pathDesc.iidStack), sizeof(InstanceIdStack));

   /* iidStack of bus client privilege object is
      started from iidStack of bus client object */
   memcpy(&iidStackClientPrivilege, &iidStackClient, sizeof(InstanceIdStack));

   /* create bus client privilege object */
   ret = cmsObj_addInstance(MDMOID_BUS_CLIENT_PRIVILEGE, &iidStackClientPrivilege);
   if (ret != CMSRET_SUCCESS)
   {
      sprintf(msg, "Could not add instance of Bus Client Privilege, return %d.", ret);
      cgiWriteMessagePage(fs, "EU Extension Error", msg, "modSwEU.cmd");
      return;
   }

   /* get bus client privilege object */
   ret = cmsObj_get(MDMOID_BUS_CLIENT_PRIVILEGE,
                    &iidStackClientPrivilege,
                    OGF_NO_VALUE_UPDATE,
                    (void **) &clientPrivilege);
   if (ret != CMSRET_SUCCESS)
   {
      sprintf(msg, "Could not get newly created Bus Client Privilege object, return %d.", ret);
      cgiWriteMessagePage(fs, "EU Extension Error", msg, "modSwEU.cmd");
      return;
   }

   cgiGetValueByName(query, "wellknownName", name);
   cgiGetValueByName(query, "path", path);
   cgiGetValueByName(query, "interface", interface);
   cgiGetValueByName(query, "member", member);
   cgiGetValueByName(query, "type", type);

   cmsLog_debug("\nAdd Privilege:\n  Wellknown name = %s\n  Object path = %s\n  Interface = %s\n  Member = %s\n  Member Type = %s\n\n",
                name, path, interface, member, type);

   /* update parameters in bus client privilege object */
   REPLACE_STRING_IF_NOT_EQUAL(clientPrivilege->wellknownName, name);
   REPLACE_STRING_IF_NOT_EQUAL(clientPrivilege->objectPath, path);
   REPLACE_STRING_IF_NOT_EQUAL(clientPrivilege->interface, interface);
   REPLACE_STRING_IF_NOT_EQUAL(clientPrivilege->member, member);
   REPLACE_STRING_IF_NOT_EQUAL(clientPrivilege->memberType, type);

   /* set bus client privilege object */
   ret = cmsObj_set(clientPrivilege, &iidStackClientPrivilege);

   /* free bus client privilege object */
   cmsObj_free((void **) &clientPrivilege);

   if (ret != CMSRET_SUCCESS)
   {
      sprintf(msg, "Could not set BusClientPrivilegeObject, return %d", ret);
      cgiWriteMessagePage(fs, "EU Extension Error", msg, "modSwEU.cmd");
      return;
   }

   /* show EU extension */
   cgiModSwEUExtensionView(query, fs);

   /* save changes to flash */
   cmsMgm_saveConfigToFlash();
}

static CmsRet removeBusClientPrivilege(
   const char *EUID,
   const char *name,
   const char *path,
   const char *interface,
   const char *member,
   const char *type)
{
   char euFullPath[BUFLEN_256]={0};
   MdmPathDescriptor pathDesc;
   BusClientPrivilegeObject *clientPrivilege = NULL;
   InstanceIdStack iidStackClientPrivilege = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("EUID='%s', name='%s', path='%s', interface='%s', member='%s', type='%s'",
                EUID, name, path, interface, member, type);

   /* get EU fullpath by EUID */
   ret = qdmModsw_getExecUnitFullPathByEuidLocked(EUID, euFullPath, sizeof(euFullPath));
   if (ret != CMSRET_SUCCESS)
   {
      return CMSRET_INVALID_ARGUMENTS;
   }

   /* get iidStack of EU */
   ret = cmsMdm_fullPathToPathDescriptor(euFullPath, &pathDesc);
   if (ret != CMSRET_SUCCESS)
   {
      return CMSRET_INVALID_ARGUMENTS;
   }

   INIT_INSTANCE_ID_STACK(&iidStackClientPrivilege);

   /* iidStack of bus client object has same value of iidStack of EU */
   while (cmsObj_getNextInSubTreeFlags(MDMOID_BUS_CLIENT_PRIVILEGE,
                                       &(pathDesc.iidStack),
                                       &iidStackClientPrivilege,
                                       OGF_NO_VALUE_UPDATE,
                                       (void **)&clientPrivilege) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strcmp(clientPrivilege->wellknownName, name) == 0 &&
          cmsUtl_strcmp(clientPrivilege->objectPath, path) == 0 &&
          cmsUtl_strcmp(clientPrivilege->interface, interface) == 0 &&
          cmsUtl_strcmp(clientPrivilege->member, member) == 0 &&
          cmsUtl_strcmp(clientPrivilege->memberType, type) == 0)
      {
         /* delete bus client privilege */
         ret = cmsObj_deleteInstance(MDMOID_BUS_CLIENT_PRIVILEGE, &iidStackClientPrivilege);
         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsObj_deleteInstance returns error. ret=%d", ret);
         }

         /* free bus client privilege object before break */
         cmsObj_free((void **)&clientPrivilege);

         break;
      }

      /* free bus client privilege object */
      cmsObj_free((void **)&clientPrivilege);
   }

   return ret;
}

void cgiModSwEUPrivilegeRemove(char *query, FILE *fs)
{
   char EUID[BUFLEN_64]={0};
   char name[BUFLEN_64]={0};
   char path[BUFLEN_64]={0};
   char interface[BUFLEN_128]={0};
   char member[BUFLEN_64]={0};
   char type[BUFLEN_32]={0};
   const char *nullstr = "(null)";
   char *pTokenObj = NULL, *pTokenParam = NULL;
   char *pLastObj = NULL, *pLastParam = NULL;
   char lst[WEB_BUF_SIZE_MAX]={0};
   char msg[BUFLEN_64]={0};
   UINT32 i = 0;
   CmsRet ret = CMSRET_SUCCESS;

   cgiGetValueByName(query, "EUID", EUID);

   cgiGetValueByName(query, "rmLst", lst);

   pTokenObj = strtok_r(lst, ",", &pLastObj);

   while (pTokenObj != NULL)
   {
      i = 0;
      memset(name, 0, BUFLEN_64);
      memset(path, 0, BUFLEN_64);
      memset(interface, 0, BUFLEN_128);
      memset(member, 0, BUFLEN_64);
      memset(type, 0, BUFLEN_32);

      pTokenParam = strtok_r(pTokenObj, "|", &pLastParam);

      while (pTokenParam != NULL)
      {
         switch (i)
         {
            case 0:
               if (cmsUtl_strcmp(pTokenParam, nullstr))
                  strncpy(name, pTokenParam, BUFLEN_64);
               break;
            case 1:
               if (cmsUtl_strcmp(pTokenParam, nullstr))
                  strncpy(path, pTokenParam, BUFLEN_64);
               break;
            case 2:
               if (cmsUtl_strcmp(pTokenParam, nullstr))
                  strncpy(interface, pTokenParam, BUFLEN_128);
               break;
            case 3:
               if (cmsUtl_strcmp(pTokenParam, nullstr))
                  strncpy(member, pTokenParam, BUFLEN_64);
               break;
            case 4:
               if (cmsUtl_strcmp(pTokenParam, nullstr))
                  strncpy(type, pTokenParam, BUFLEN_32);
               break;
            default:
               break;
         }

         i++;
         pTokenParam = strtok_r(NULL, "|", &pLastParam);
      }

      ret = removeBusClientPrivilege(EUID, name, path, interface, member, type);
      if (ret != CMSRET_SUCCESS)
      {
         break;
      }

      pTokenObj = strtok_r(NULL, ",", &pLastObj);
   }

   /* show EU extension */
   if (ret == CMSRET_SUCCESS)
   {
      /* show EU extension */
      cgiModSwEUExtensionView(query, fs);
      /* save changes to flash */
      cmsMgm_saveConfigToFlash();
   }
   else
   {
      sprintf(msg, "Error: Can't delete Bus Client Privilege, name='%s', path='%s', interface='%s', member='%s', type='%s'.",
              name, path, interface, member, type);
      cgiWriteMessagePage(fs, "EU Extension Error", msg, "modSwEU.cmd");
   }
}


#ifdef SUPPORT_OSGI_FELIX
void cgiSendLbCommandToOsgid(void)
{
   CmsRet ret = CMSRET_SUCCESS;
   CmsMsgHeader msg = EMPTY_MSG_HEADER;

   /* for each EU object, message needs to be sent to felix to get status
      of the EU, and update MDM */

   /* initialize header fields */
   msg.type = CMS_MSG_REQUEST_BUNDLE_LIST;
   msg.src = cmsMsg_getHandleEid(msgHandle);
   msg.dst = EID_OSGID;
   msg.flags_request = 1;

   if((ret = cmsMsg_send(msgHandle, &msg)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to send message (ret=%d)", ret);
   }
   else
   {
      cmsLog_debug("Sent REQUEST_BUNDLE_LIST to osgid");
   }
}
#endif


void cgiModSwLogDU(char *query, FILE *fs)
{
   char clean[BUFLEN_256];
   FILE *fp;
   char logfile[CMS_MAX_FULLPATH_LENGTH]={0};
   CmsRet ret;

   ret = cmsUtl_getRunTimePath(CMS_MODSW_DEBUG_LOG, logfile, sizeof(logfile));
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("debug logfile name too long (max=%d)", sizeof(logfile));
   }

   cgiGetValueByName(query, "clean", clean);

   if ( cmsUtl_strcmp(clean, "true") == 0 )
   {
      int sys_rval;

      sys_rval = unlink(logfile);
      if (sys_rval != 0)
      {
         cmsLog_error("unlink(%s) returned %d, errno=%d",
                       logfile, sys_rval, errno);
      }
   }

   /* write html header */
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
   fprintf(fs, "<link rel='stylesheet' href='tabletab.css' type='text/css'>\n");

   fprintf(fs, "<title></title>\n");
   fprintf(fs, "</head>\n");

   fprintf(fs, "<center>\n<form>\n");
   fprintf(fs, "<b>SoftwareModules DU Operation Log</b><br><br>\n");
   fprintf(fs, "<table border='1' cellpadding='1' cellspacing='0'>\n");   
   fprintf(fs, "   <tr align='center'>\n");
   fprintf(fs, "      <td class='hd'>Time</td>\n");
   fprintf(fs, "      <td class='hd'>Action ID</td>\n");
   fprintf(fs, "      <td class='hd'>Action Name</td>\n");
   fprintf(fs, "      <td class='hd'>Message</td>\n");
   fprintf(fs, "   </tr>\n");

   /*Get all log and print them*/
   if ((fp = fopen(logfile, "r")) == NULL)
   {
      cmsLog_debug("%s not found (may be empty?)", logfile);
   }
   else
   {
      char line[BUFLEN_256];
      char dateTime[BUFLEN_64];
      char operStr[BUFLEN_32];
      char logMsg[BUFLEN_128], *log;
      int reqId;

      while (fgets(line, sizeof(line), fp) != NULL)
      {
         memset(dateTime, 0, sizeof(dateTime));
         memset(operStr, 0, sizeof(operStr));
         memset(logMsg, 0, sizeof(logMsg));

         sscanf(line, "%s | %d | %s", dateTime, &reqId, operStr);
         log = strrchr(line,'|');
         if(log != NULL)
         {
            log += 2;
            strcpy(logMsg, log);
         }
         else
            strcpy(logMsg, "");
         
         fprintf(fs, "   <tr align='center'>\n");
         fprintf(fs, "      <td>%s</td>\n", dateTime);
         fprintf(fs, "      <td>%d</td>\n", reqId);
         fprintf(fs, "      <td>%s</td>\n", operStr);
         fprintf(fs, "      <td>%s</td>\n", logMsg);
         fprintf(fs, "   </tr>\n"); 
      }
      fclose(fp);
   }
   
   fprintf(fs, "</table>\n");
   fprintf(fs, "<p align='center'>\n");
   fprintf(fs, "<input type='button' onClick='location.href=\"modSwLogDU.cmd?action=view&clean=false\"' value='Refresh'>\n");
   fprintf(fs, "<input type='button' onClick='location.href=\"modSwLogDU.cmd?action=view&clean=true\"' value='Clean'>\n");
   fprintf(fs, "<input type='button' onClick='location.href=\"modSwDU.cmd?action=view\"' value='Back'>\n");
   fprintf(fs, "</form>\n</center>\n</body>\n</html>\n");
   fflush(fs);
}

void cgiModSwLogEU(char *query, FILE *fs)
{
   char clean[BUFLEN_256];
   FILE *fp;
   char logfile[CMS_MAX_FULLPATH_LENGTH]={0};
   CmsRet ret;

   ret = cmsUtl_getRunTimePath(CMS_MODSW_DEBUG_EU_LOG, logfile, sizeof(logfile));
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("debug logfile name too long (max=%d)", sizeof(logfile));
   }

   cgiGetValueByName(query, "clean", clean);

   if ( cmsUtl_strcmp(clean, "true") == 0 )
   {
      int sys_rval;

      sys_rval = unlink(logfile);
      if (sys_rval != 0)
      {
         cmsLog_error("unlink(%s) returned %d, errno=%d",
                       logfile, sys_rval, errno);
      }
   }

   /* write html header */
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
   fprintf(fs, "<link rel='stylesheet' href='tabletab.css' type='text/css'>\n");

   fprintf(fs, "<title></title>\n");
   fprintf(fs, "</head>\n");

   fprintf(fs, "<center>\n<form>\n");
   fprintf(fs, "<b>SoftwareModules EU Operation Log</b><br><br>\n");
   fprintf(fs, "<table border='1' cellpadding='1' cellspacing='0'>\n");   
   fprintf(fs, "   <tr align='center'>\n");
   fprintf(fs, "      <td class='hd'>Time</td>\n");
   fprintf(fs, "      <td class='hd'>Application</td>\n");
   fprintf(fs, "      <td class='hd'>Operation</td>\n");
   fprintf(fs, "      <td class='hd'>Message</td>\n");
   fprintf(fs, "   </tr>\n");

   /*Get all log and print them*/
   if ((fp = fopen(logfile, "r")) == NULL)
   {
      cmsLog_debug("%s not found (may be empty?)", logfile);
   }
   else
   {
      char line[BUFLEN_256];
      char dateTime[BUFLEN_64];
      char operStr[BUFLEN_32];
      char appNameStr[BUFLEN_64];
      char logMsg[BUFLEN_128], *log;

      while (fgets(line, sizeof(line), fp) != NULL)
      {
         memset(dateTime, 0, sizeof(dateTime));
         memset(appNameStr, 0, sizeof(appNameStr));
         memset(operStr, 0, sizeof(operStr));
         memset(logMsg, 0, sizeof(logMsg));

         sscanf(line, "%s | %s | %s", dateTime, appNameStr, operStr);
         log = strrchr(line,'|');
         if(log != NULL)
         {
            log += 2;
            strcpy(logMsg, log);
         }
         else
            strcpy(logMsg, "");
         
         fprintf(fs, "   <tr align='center'>\n");
         fprintf(fs, "      <td>%s</td>\n", dateTime);
         fprintf(fs, "      <td>%s</td>\n", appNameStr);
         fprintf(fs, "      <td>%s</td>\n", operStr);
         fprintf(fs, "      <td>%s</td>\n", logMsg);
         fprintf(fs, "   </tr>\n"); 
      }
      fclose(fp);
   }
   
   fprintf(fs, "</table>\n");
   fprintf(fs, "<p align='center'>\n");
   fprintf(fs, "<input type='button' onClick='location.href=\"modSwLogEU.cmd?action=view&clean=false\"' value='Refresh'>\n");
   fprintf(fs, "<input type='button' onClick='location.href=\"modSwLogEU.cmd?action=view&clean=true\"' value='Clean'>\n");
   fprintf(fs, "<input type='button' onClick='location.href=\"modSwEU.cmd?action=view\"' value='Back'>\n");
   fprintf(fs, "</form>\n</center>\n</body>\n</html>\n");
   fflush(fs);
}


void cgiModSwLogEE(char *query, FILE *fs)
{
   char clean[BUFLEN_256];
   FILE *fp;
   char logfile[CMS_MAX_FULLPATH_LENGTH]={0};
   CmsRet ret;

   ret = cmsUtl_getRunTimePath(CMS_MODSW_DEBUG_EE_LOG, logfile, sizeof(logfile));
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("debug logfile name too long (max=%d)", sizeof(logfile));
   }

   cgiGetValueByName(query, "clean", clean);

   if ( cmsUtl_strcmp(clean, "true") == 0 )
   {
      int sys_rval;

      sys_rval = unlink(logfile);
      if (sys_rval != 0)
      {
         cmsLog_error("unlink(%s) returned %d, errno=%d",
                       logfile, sys_rval, errno);
      }
   }

   /* write html header */
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
   fprintf(fs, "<link rel='stylesheet' href='tabletab.css' type='text/css'>\n");

   fprintf(fs, "<title></title>\n");
   fprintf(fs, "</head>\n");

   fprintf(fs, "<center>\n<form>\n");
   fprintf(fs, "<b>SoftwareModules EE Operation Log</b><br><br>\n");
   fprintf(fs, "<table border='1' cellpadding='1' cellspacing='0'>\n");   
   fprintf(fs, "   <tr align='center'>\n");
   fprintf(fs, "      <td class='hd'>Time</td>\n");
   fprintf(fs, "      <td class='hd'>Action ID</td>\n");
   fprintf(fs, "      <td class='hd'>Action Name</td>\n");
   fprintf(fs, "      <td class='hd'>Message</td>\n");
   fprintf(fs, "   </tr>\n");

   /*Get all log and print them*/
   if ((fp = fopen(logfile, "r")) == NULL)
   {
      cmsLog_debug("%s not found (may be empty?)", logfile);
   }
   else
   {
      char line[BUFLEN_256];
      char dateTime[BUFLEN_64];
      char operStr[BUFLEN_32];
      char logMsg[BUFLEN_128], *log;
      int reqId;

      while (fgets(line, sizeof(line), fp) != NULL)
      {
         memset(dateTime, 0, sizeof(dateTime));
         memset(operStr, 0, sizeof(operStr));
         memset(logMsg, 0, sizeof(logMsg));

         sscanf(line, "%s | %d | %s", dateTime, &reqId, operStr);
         log = strrchr(line,'|');
         if(log != NULL)
         {
            log += 2;
            strcpy(logMsg, log);
         }
         else
            strcpy(logMsg, "");
         
         fprintf(fs, "   <tr align='center'>\n");
         fprintf(fs, "      <td>%s</td>\n", dateTime);
         fprintf(fs, "      <td>%d</td>\n", reqId);
         fprintf(fs, "      <td>%s</td>\n", operStr);
         fprintf(fs, "      <td>%s</td>\n", logMsg);
         fprintf(fs, "   </tr>\n"); 
      }
      fclose(fp);
   }

   fprintf(fs, "</table>\n");
   fprintf(fs, "<p align='center'>\n");
   fprintf(fs, "<input type='button' onClick='location.href=\"modSwLogEE.cmd?action=view&clean=false\"' value='Refresh'>\n");
   fprintf(fs, "<input type='button' onClick='location.href=\"modSwLogEE.cmd?action=view&clean=true\"' value='Clean'>\n");
   fprintf(fs, "<input type='button' onClick='location.href=\"modSwEE.cmd?action=view\"' value='Back'>\n");
   fprintf(fs, "</form>\n</center>\n</body>\n</html>\n");
   fflush(fs);
}


#define CGI_MODSW_EE_LOG 0
#define CGI_MODSW_DU_LOG 1


void cgiModSw_writeLine(int logType, const char *line)
{
   FILE *fpLog = NULL, *fpTmpLog = NULL;
   char tmpLog[BUFLEN_1024]={0};
   char logfile[BUFLEN_1024]={0};
   char *buffer = NULL;
   size_t len = 0;

   switch (logType)
   {
      case CGI_MODSW_DU_LOG:
         /* get DU log file name ie.  /local/modsw/tmp/DuLog */
         cmsUtl_getRunTimePath(CMS_MODSW_DEBUG_LOG, logfile, sizeof(logfile));

         /* generate tmp DU log file name ie.  /local/modsw/tmp/DuLog.tmp */
         snprintf(tmpLog, sizeof(tmpLog), "%s.%s", logfile, "tmp");

         break;

      case CGI_MODSW_EE_LOG:
         /* get EE log file name ie.  /local/modsw/tmp/EeLog */
         cmsUtl_getRunTimePath(CMS_MODSW_DEBUG_EE_LOG, logfile, sizeof(logfile));

         /* generate tmp EE log file name ie.  /local/modsw/tmp/EeLog.tmp */
         snprintf(tmpLog, sizeof(tmpLog), "%s.%s", logfile, "tmp");

         break;

      default:
         cmsLog_error("Log type %d is not supported yet.", logType);
         return;
   }
   
   /* move /local/modsw/tmp/DuLog to /local/modsw/tmp/DuLog.tmp */
   rename(logfile, tmpLog);

   /* open /local/modsw/tmp/DuLog to write from beginning of file */
   fpLog = fopen(logfile, "w");
   if (fpLog == NULL)
   {
      cmsLog_error("can't open file: %s", logfile);
      return;
   }

   /* open /local/modsw/tmp/DuLog.tmp to read from beginning of file */
   fpTmpLog = fopen(tmpLog, "a+");
   if (fpTmpLog == NULL)
   {
      cmsLog_error("can't open file: %s", tmpLog);
      return;
   }

   /* write new line to beginning of /local/modsw/tmp/DuLog */
   fprintf(fpLog, "%s", line);

   /* read each line from /local/modsw/tmp/DuLog.tmp
      then write it to /local/modsw/tmp/DuLog  */
   while (getline(&buffer, &len, fpTmpLog) != -1)
   {
      fprintf(fpLog, "%s", buffer);
   }

   /* release memory if any */
   if (buffer)
   {
      free(buffer);
   }

   /* close open files */
   fclose(fpTmpLog);
   fclose(fpLog);

   /* remove tmp log file */
   unlink(tmpLog);
}


void cgiModSw_writeToDebugLog(const char *operation, CmsEntityId destEid)
{
   char dateTime[BUFLEN_64]={0};
   char line[BUFLEN_1024]={0};
   const CmsEntityInfo *eInfo=NULL;
   const char *destName=NULL;

   cmsTms_getXSIDateTime(0, dateTime, sizeof(dateTime));

   if ((eInfo = cmsEid_getEntityInfo(destEid)) == NULL)
   {
      cmsLog_error("failed to get Entity Info for EID %d", destEid);
      destName = "(unknown)";
   }
   else
   {
      destName = eInfo->name;
   }

   sprintf(line, "%s | %d | %s | Send to %s successfully: %s DU.\n",
           dateTime, SMReqId, operation, destName, operation);

   cgiModSw_writeLine(CGI_MODSW_DU_LOG, line);
}


void cgiModSw_handleResponse(CmsMsgHeader *msg)
{
   int i = 0;
   int num_log = msg->wordData;
   char dateTime[BUFLEN_64]={0};
   char errorStr[BUFLEN_1024]={0};
   char line[BUFLEN_1024]={0};
   DUresponseStateChangedMsgBody *responseBody = (DUresponseStateChangedMsgBody *) (msg+1);

   cmsTms_getXSIDateTime(0, dateTime, sizeof(dateTime));

   for( i = 0; i < num_log; i++)
   {
      cmsErr_getStringBuf(responseBody->faultCode, errorStr, sizeof(errorStr));

      sprintf(line, "%s | %d | %s | %s\n",
              dateTime, responseBody->reqId, responseBody->operation, errorStr);

      cgiModSw_writeLine(CGI_MODSW_DU_LOG, line);

      responseBody++;
   }
}


void cgiModSw_writeToDebugLogEE(const char *operation, CmsEntityId destEid,
                                UBOOL8 pass, const char *errlog)
{
   char dateTime[BUFLEN_64]={0};
   char line[BUFLEN_1024]={0};
   const CmsEntityInfo *eInfo=NULL;
   const char *destName=NULL;

   cmsTms_getXSIDateTime(0, dateTime, sizeof(dateTime));

   if ((eInfo = cmsEid_getEntityInfo(destEid)) == NULL)
   {
      cmsLog_error("failed to get Entity Info for EID %d", destEid);
      destName = "(unknown)";
   }
   else
   {
      destName = eInfo->name;
   }

   if (pass)
   {
      sprintf(line, "%s | %d | %s | Send to %s successfully: %s EE.\n",
              dateTime, SMReqId, operation, destName, operation);
   }
   else
   {
      sprintf(line, "%s | %d | %s | Send to %s failed: %s.\n",
              dateTime, SMReqId, operation, destName, errlog);
   }

   cgiModSw_writeLine(CGI_MODSW_EE_LOG, line);
}


void cgiModSw_handleResponseEE(CmsMsgHeader *msg)
{
   int i = 0;
   int num_log = msg->wordData;
   char dateTime[BUFLEN_64]={0};
   char errorStr[BUFLEN_1024]={0};
   char line[BUFLEN_1024]={0};
   EEresponseStateChangedMsgBody *responseBody = (EEresponseStateChangedMsgBody *) (msg+1);

   cmsTms_getXSIDateTime(0, dateTime, sizeof(dateTime));

   for( i = 0; i < num_log; i++)
   {
      cmsErr_getStringBuf(responseBody->faultCode, errorStr, sizeof(errorStr));

      sprintf(line, "%s | %d | %s | %s\n",
              dateTime, responseBody->reqId, responseBody->operation, errorStr);

      cgiModSw_writeLine(CGI_MODSW_EE_LOG, line);

      responseBody++;
   }
}


#endif  /* SUPPORT_MODSW_WEBUI */

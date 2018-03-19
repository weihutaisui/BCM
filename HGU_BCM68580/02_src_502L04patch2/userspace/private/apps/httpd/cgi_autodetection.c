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

#ifdef DMP_X_BROADCOM_COM_AUTODETECTION_1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>

#include "cms.h"
#include "httpd.h"
#include "cgi_main.h"
#include "cms_util.h"
#include "cms_core.h"
#include "cms_dal.h"

#define CHECKEDSTRING   "checked"
#define UNCHECKEDSTRING " "

static void cgiAutoDetectionFunctions(FILE *fs) 
{
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   fprintf(fs, "function applySaveClick(cbl) {\n");
   fprintf(fs, "   var loc = 'autodetection.cmd?action=apply';\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   if (cbl.length > 0) {\n");
   fprintf(fs, "     for (i = 0; i < cbl.length; i++) {\n");
   fprintf(fs, "       loc += ',' + cbl[i].value + '=';\n");
   fprintf(fs, "       if (cbl[i].checked)\n");
   fprintf(fs, "         loc += '1';\n");
   fprintf(fs, "       else\n");  
   fprintf(fs, "         loc += '0';\n");
   fprintf(fs, "     }\n");
   fprintf(fs, "   }\n");   
   fprintf(fs, "   else {\n");
   fprintf(fs, "      if (cbl.checked)\n");
   fprintf(fs, "        loc += ',' + cbl.value + '=1';\n");
   fprintf(fs, "      else\n");
   fprintf(fs, "        loc += ',' + cbl.value + '=0';\n\n");
   fprintf(fs, "   }\n");   
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function cbClick(cbl, autoDetect) {\n");
   fprintf(fs, "   if (cbl.length > 0) {\n");
   fprintf(fs, "     if (autoDetect == \"1\")\n");
   fprintf(fs, "       if (cbl[0].checked)\n");
   fprintf(fs, "         for (i = 1; i < cbl.length; i++)\n");
   fprintf(fs, "           cbl[i].checked = true;\n");
   fprintf(fs, "     else if (autoDetect == \"0\");\n");
   fprintf(fs, "       for (i = 1; i < cbl.length; i++)\n");
   fprintf(fs, "         if (!cbl[i].checked)\n");
   fprintf(fs, "           cbl[0].checked = false;\n");
   fprintf(fs, "   }\n");   
   fprintf(fs, "}\n\n");
   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");

}



static void cgiAutoDetectionDisplayList(FILE *fs)
{

   WanDevObject *wanDev=NULL;
   InstanceIdStack wanDevIid=EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   WanPppConnObject *pppCon=NULL;   
   WanIpConnObject *ipCon=NULL;
   char interfaceStr[CMS_IFNAME_LENGTH];
   char *displayCheckedPtr;

   displayCheckedPtr = (dalAutoDetect_isAutoDetectEnabled() == TRUE)  ?  CHECKEDSTRING :  UNCHECKEDSTRING;
   
   fprintf(fs, "      <td align='center'><input type='checkbox' %s name='cbList' onClick='cbClick(this.form.cbList, \"1\")', value=auto>    Auto Detection</td><br><br>\n", displayCheckedPtr);

   while (cmsObj_getNextFlags(MDMOID_WAN_DEV, &wanDevIid, OGF_NO_VALUE_UPDATE, (void **)&wanDev) == CMSRET_SUCCESS)
   {
      cmsObj_free((void **)&wanDev);  /* no longer needed */

      /* get the related ipCon obj */
      INIT_INSTANCE_ID_STACK(&iidStack);
      while (cmsObj_getNextInSubTreeFlags(MDMOID_WAN_IP_CONN, &wanDevIid, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&ipCon) == CMSRET_SUCCESS)      
      {
         if (!cmsUtl_strcmp(ipCon->connectionType, MDMVS_IP_ROUTED))
         {
            snprintf(interfaceStr, sizeof(interfaceStr), (ipCon->X_BROADCOM_COM_IfName ? ipCon->X_BROADCOM_COM_IfName : "(null)"));
            /*  check box and interface name */
            displayCheckedPtr = (ipCon->enable == TRUE) ?  CHECKEDSTRING : UNCHECKEDSTRING;
            fprintf(fs, "      <td align='center'><input type='checkbox' %s name='cbList' onClick='cbClick(this.form.cbList, \"0\")', value=%s>    %s</td><br><br>\n", 
            displayCheckedPtr, interfaceStr, interfaceStr);
         }
         else
         {
            /* skip None IPoE connection */
            continue;
         }                   
         cmsObj_free((void **)&ipCon);
      }
   
      /* get the related pppCon obj */
      INIT_INSTANCE_ID_STACK(&iidStack);
      while (cmsObj_getNextInSubTreeFlags(MDMOID_WAN_PPP_CONN, &wanDevIid, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&pppCon) == CMSRET_SUCCESS)
      {
         if (!dalWan_isPPPoA(&iidStack))
         {     
            snprintf(interfaceStr, sizeof(interfaceStr), (pppCon->X_BROADCOM_COM_IfName ? pppCon->X_BROADCOM_COM_IfName : "(null)"));
            /*  check box and interface name */
            displayCheckedPtr = (pppCon->enable == TRUE) ?  CHECKEDSTRING : UNCHECKEDSTRING;
            fprintf(fs, "      <td align='center'><input type='checkbox' %s name='cbList' onClick='cbClick(this.form.cbList, \"0\")', value=%s>    %s</td><br><br>\n",
            displayCheckedPtr, interfaceStr, interfaceStr);
         }
         else
         {
            /* skip PPPoA interface */
            continue;
         }
         cmsObj_free((void **)&pppCon);
      }
   }
   
}   

static void cgiAutoDetectionView(FILE *fs) 
{

   /* write html header */
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");

   /* Java Script functions */
   cgiAutoDetectionFunctions(fs);

   /* write body */
   fprintf(fs, "<title></title>\n</head>\n<body>\n<blockquote>\n<form>\n");
   fprintf(fs, "<b>Connection Auto Detection Configuration</b><br><br>\n");
   fprintf(fs, "If 'Auto Detection' is checked,  all WAN connections will be enabled but only one of the DHCP/PPPoE WAN connection will be connected.  For Manual Selection, \
   select or unselect the WAN connection. If there are unselected WAN connections in the list, \
                Auto Detection will be disabled.  NOTE:  Auto Detection and Manual Selection are mutually exclusive.<br><br><br>\n");

   cgiAutoDetectionDisplayList(fs);
    
   fprintf(fs, "<input type='button' onClick='applySaveClick(this.form.cbList)', value='Apply/Save'>\n");
   fprintf(fs, "</center>\n</form>\n</blockquote>\n</body>\n</html>\n");

   
}

static void cgiAutoDetechtionApplySave(char *query, FILE *fs)
{

   char *pToken = NULL;
   char *pLast = NULL;
   char tempStr[BUFLEN_32];
   char *tempPtr=NULL;
   char ifName[CMS_IFNAME_LENGTH];
   UBOOL8 newAutoDetectFlag=FALSE;
   UBOOL8 isWanEnabled=FALSE;
   CmsRet ret;
   UBOOL8 failed=FALSE;
   UBOOL8 currAutoDetectFlag=dalAutoDetect_isAutoDetectEnabled();
   
   pToken = strtok_r(query, ",", &pLast);
   pToken = strtok_r(NULL, ",", &pLast);

   newAutoDetectFlag = (cmsUtl_strcmp( "auto=1", pToken) == 0) ? 1 : 0;

   if (currAutoDetectFlag && newAutoDetectFlag)
   {
      /* Case 1 Enable -> Enable 
      * Do nothing and just return 
      */
      cmsLog_debug("No change in auto detection and do nothing");
      return;
   }
   else if (!currAutoDetectFlag && newAutoDetectFlag)
   {
      /* Case 2:  Disable -> Enable
      *  Need to enable all WAN connection objects first
      *   and then change the autoDetectFlag
      */
      if (dalAutoDetect_enableAllWanConn() != CMSRET_SUCCESS)
      {
         cmsLog_error("Fail to dalAutoDetect_enableAllWanConn");
         failed = TRUE;
      }
   }
   else 
   {
      /* Case 3: Enable -> Disable  
      *   Case 4: Disable -> Disable (Change on the enable/disable flags on  WAN connection object only)
      *   Need to set the wan conn object according to the manual setting one by one first
      *   and then change the autoDetectFlag if it is changed.
      */
   
      pToken = strtok_r(NULL, ",", &pLast);   

      while (!failed && pToken != NULL)
      {
         strncpy(tempStr, pToken, sizeof(tempStr)-1);
         tempPtr = strchr(tempStr, '=');
         isWanEnabled = (*(tempPtr+1)  == '1');
         *tempPtr = '\0';
         strncpy(ifName, tempStr, sizeof(ifName)-1);
         if ((ret = dalWan_enableDisableWanConnObj(ifName, isWanEnabled)) != CMSRET_SUCCESS)
         {
            cmsLog_error("dalWan_enableDisableWanConnObj failed for %s (ret=%d)", ifName, ret);
            failed = TRUE;
         }    
         else
         {
            cmsLog_debug("Done dalWan_enableDisableWanConnObj");
         }
         
         pToken = strtok_r(NULL, ",", &pLast);
      } /* end of while loop over list of connections to configure */
   }

   if (currAutoDetectFlag != newAutoDetectFlag)
   {
      /* If auto detection flag is changed. Do a set on that */
      if ((ret = dalAutoDetect_setAutoDetectionFlag(newAutoDetectFlag)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Fail to enable/disable auto detect flag(%d). ret=%d", newAutoDetectFlag, ret);
         cgiWriteMessagePage(fs, "Auto Detection", "Failed to enable/disable auto detect.", "autodetection.cmd?action=view");
         failed = TRUE;
      }
   }
   
   /*
    * Need to save configuration of  auto detection and wan conn object 
    * parameters if not failed.
    */
   if (!failed)
   {
      glbSaveConfigNeeded = TRUE;
   }

   return;
   
}



void cgiAutoDetection(char *query, FILE *fs)
{
   char action[BUFLEN_256];

   cgiGetValueByName(query, "action", action);
      
   if (cmsUtl_strcmp(action, "apply") == 0)
   {  
      cgiAutoDetechtionApplySave(query, fs);
   }         
   cgiAutoDetectionView(fs);       
}

#endif /* DMP_X_BROADCOM_COM_AUTODETECTION_1 */


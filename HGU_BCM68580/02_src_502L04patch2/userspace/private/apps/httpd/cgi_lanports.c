/***
 * This source model is commented out in 4.06 release.  Starting from 4.06, "Lan Port" feature is not needed.
 * This feature is intended for 96358GW/48GW/38GW platform only and is not removed from the source tree just in case
 * it is needed later on.
 *  
 */
#if 0
/***********************************************************************
 *
 *  Copyright (c) 2006  Broadcom Corporation
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

#include "cms.h"

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
#include <sys/utsname.h>
#include <net/if_arp.h>
#include <net/route.h>
#include <linux/if.h>
#include "cgi_cmd.h"
#include "cgi_ntwk.h"
#include "cgi_main.h"
#include "syscall.h"
#include "sysdiag.h"
#include "httpd.h"

#include "cms_util.h"
#include "cms_core.h"
#include "cms_dal.h"

struct tmpVirtualPortsInfo
{
   char ifName[BUFLEN_32];
   UBOOL8 enabled;
};

#define MAX_TEMP_VIRTUAL_PORTS 8

static struct tmpVirtualPortsInfo tmpVirtualPortsArray[MAX_TEMP_VIRTUAL_PORTS];
static UBOOL8 tmpVirtualPortsInitialized=FALSE;
static SINT32 tmpVirtualPortsState=0;

static void populateTmpVirtualPorts(void);
static void clearTmpVirtualPorts(void);
static void lanPortsApplySave(FILE *fs);
static void lanPortsConfig(char *query, FILE *fs);
static void lanPortsView(FILE *fs);
static void writePortMapScript(FILE *fs, UBOOL8 checked);


void cgiLanPortsCfg(char *query, FILE *fs) {
   char action[BUFLEN_32]={0};


   cgiGetValueByName(query, "action", action);

   cmsLog_debug("action=%s", action);

   dalEsw_getEthernetSwitchInfo(&glbWebVar);
   if (!tmpVirtualPortsInitialized)
   {
      clearTmpVirtualPorts();
      tmpVirtualPortsInitialized = TRUE;
   }


   if ( strcmp(action, "applysave") == 0 )
      lanPortsApplySave(fs);
   else if ( strcmp(action, "config") == 0)
      lanPortsConfig(query, fs);
   else if ( strcmp(action, "view") == 0 )
      lanPortsView(fs);
   else
      lanPortsView(fs);
}

void lanPortsApplySave(FILE *fs)
{
   /*
    * Update our glbWebvar to the current state.
    * Enable or disable virtual ports in the MDM.
    * Clear our temp state variable.
    */
   glbWebVar.virtualPortsEnabled = (tmpVirtualPortsState == 1);
   cmsLog_debug("calling DAL layer to set virtual ports enabled to %d", glbWebVar.virtualPortsEnabled);
   dalEsw_enableVirtualPorts(glbWebVar.virtualPortsEnabled);
   tmpVirtualPortsState = 0;
   clearTmpVirtualPorts();
   
   glbSaveConfigNeeded = TRUE;
   
   lanPortsView(fs);
   
   return;
}


/*
 * User has clicked on the enabled virtual ports checkbox, but has
 * not saved it to the MDM yet.  We have to make the WebUI reflect the
 * user's choice even though that choice is not visible in the MDM yet.
 */
void lanPortsConfig(char *query, FILE *fs)
{
   char virtualPortsEnabled[BUFLEN_16]={0};

   cgiGetValueByName(query, "virtPortsEnabled", virtualPortsEnabled);
   cmsLog_debug("virtualPortsEnabled=%s", virtualPortsEnabled);

   if (!strcmp(virtualPortsEnabled, "1"))
   {
      tmpVirtualPortsState = 1;
      populateTmpVirtualPorts();
   }
   else
   {
      tmpVirtualPortsState = -1;
      clearTmpVirtualPorts();
   }

   lanPortsView(fs);
}


void populateTmpVirtualPorts(void)
{
   UINT32 i = 0;

   memset(tmpVirtualPortsArray, 0, sizeof(tmpVirtualPortsArray));
   
   for (i = 0; i < glbWebVar.numberOfVirtualPorts; i++)
   {
      sprintf(tmpVirtualPortsArray[i].ifName, "%s.%d", glbWebVar.ethSwitchIfName, START_PMAP_ID + i);
      tmpVirtualPortsArray[i].enabled = TRUE;
   }
}


void clearTmpVirtualPorts(void)
{
   /* clear out our temp virtual ports info */   
   memset(tmpVirtualPortsArray, 0, sizeof(tmpVirtualPortsArray));
}


void lanPortsView(FILE *fs)
{
   char   intfList[BUFLEN_264];
   UINT16 numOfIntf = 0;

   glbEntryCt = 0;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   LanEthIntfObject *ethObj=NULL;
   LanUsbIntfObject *usbObj=NULL;
#ifdef BRCM_WLAN
   LanWlanObject *lanWlanObj = NULL;
#endif          
   CmsRet ret = CMSRET_SUCCESS;

  fprintf(fs, "<html><head>\n");
  fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
  fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
  fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
  fprintf(fs, "<script language=\"javascript\" src=\"portName.js\"></script>\n");

  // write Java Script
  writePortMapScript(fs, ((tmpVirtualPortsState == 1) ||
                          (tmpVirtualPortsState == 0 && glbWebVar.virtualPortsEnabled == 1)));

  // write body
  fprintf(fs, "</head>\n<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");

  fprintf(fs, "<b>LAN Ports Configuration</b><br><br>\n");
  fprintf(fs, "Use this page to enable/disable the Virtual LAN Ports feature.<br><br><br>\n");

  if (glbWebVar.numberOfVirtualPorts > 1) {
     fprintf(fs, "<table border='0' cellpadding='0' cellspacing='0'>\n");
     fprintf(fs, "    <tr>\n");
     fprintf(fs, "       <td><input type='checkbox' name='chkVirtPortsCfg' onClick='virtualPortsEnableClick()'>&nbsp;&nbsp;</td>\n");
     fprintf(fs, "<script language=\"javascript\">\n");
     fprintf(fs, "<!-- hide\n");
     fprintf(fs, "{\n");
     fprintf(fs, "   var i = 0;\n");
     fprintf(fs, "   var ifName = '';\n");
     fprintf(fs, "   var brdId = '%s';\n", glbWebVar.boardID);
     fprintf(fs, "   ifName = getUNameByLName(brdId + '|' + '%s');\n", glbWebVar.ethSwitchIfName);
     fprintf(fs, "   document.write(\"<td><input type='textbox' name='pmapCfgName' value=\"+ifName+\" size='16'></td>\");\n");
     fprintf(fs, "}\n");
     fprintf(fs, "// done hiding -->\n");
     fprintf(fs, "</script>\n");
     fprintf(fs, "    </tr>\n");
     fprintf(fs, "</table>\n");
  }
  fprintf(fs, "<br>\n");

  fprintf(fs, "<input type='button' onClick='applySaveClick()' value='Apply/Save'>\n");
  fprintf(fs, "<br><br>\n");

  fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");
  fprintf(fs, "   <tr>\n");
  fprintf(fs, "      <td class='hd'>LAN Port</td>\n");
  fprintf(fs, "   </tr>\n");


     //Initialization for getting interfaces information
     memset(intfList, 0, sizeof(intfList));
         
     while ((ret = cmsObj_getNext(MDMOID_LAN_ETH_INTF, &iidStack, (void **) &ethObj)) == CMSRET_SUCCESS)
     {
        /*
         * There are several states that we could be in when writing out
         * the list of ethernet ports:
         * case 1: virtual ports checkbox state and MDM state is sync'd.
         *         virtual ports is enabled. Action-> suppress eth1
         * case 2: virtual ports checkbox state and MDM state is sync'd.
         *         virtual ports is disabled. Action-> do nothing.
         * case 3: virtual ports checkbox state and MDM state not sync'd.
         *         virtual ports checkbox is enabled.  Action-> suppress eth1
         *         and show tmpVirtualPorts
         * case 4: virtual ports checkbox state and MDM state not sync'd.
         *         virtual ports checkbox is either enabled or disabled.
         *         Action-> suppress eth1.2, 1.3, 1.4
         * 
         */
        if (tmpVirtualPortsState == 0 && glbWebVar.virtualPortsEnabled && !cmsUtl_strcmp(ethObj->X_BROADCOM_COM_IfName, glbWebVar.ethSwitchIfName))
        {
            /* case 1 */ 
            /* suppress eth1 by not doing anything in this block. */
        }
        else if (tmpVirtualPortsState == 1 && !cmsUtl_strcmp(ethObj->X_BROADCOM_COM_IfName, glbWebVar.ethSwitchIfName))
        {
           int i=0;
           
           /*
            * case 3
            * Do not show this interface because it has been broken out into virtual ports.
            * Instead, replace it with the broken out interface ports.
            */
           cmsLog_debug("found normal port, replace with virtual ports");
           while (tmpVirtualPortsArray[i].ifName[0] != '\0')
           {
              if (intfList[0] != '\0')
              {
                 strcat(intfList, "|");
              }
              
              strcat(intfList, tmpVirtualPortsArray[i].ifName);
              
              numOfIntf++; 
              i++;
           }
        }
        else if ( (tmpVirtualPortsState == -1 || tmpVirtualPortsState == 1) && (ethObj->X_BROADCOM_COM_IfName[4] == '.') )
        {
           /* case 4:
            * suppress any interface that has a . at the right place.
            * (e.g. eth1.2)
            */
        }
        else
        {
              if (intfList[0] != '\0')
              {
                 strcat(intfList, "|");
              }
              
              strcat(intfList, ethObj->X_BROADCOM_COM_IfName);
              
              numOfIntf++; 
         }

         cmsObj_free((void **)&ethObj);
     }

     INIT_INSTANCE_ID_STACK(&iidStack);
     while ((ret = cmsObj_getNext(MDMOID_LAN_USB_INTF, &iidStack, (void **) &usbObj)) == CMSRET_SUCCESS)
     {
        if(intfList[0] != '\0')
        {
           strcat(intfList, "|");
        }
        
        strcat(intfList, usbObj->X_BROADCOM_COM_IfName);
              
        numOfIntf++; 

        cmsObj_free((void **)&usbObj);
     }
#ifdef BRCM_WLAN
     INIT_INSTANCE_ID_STACK(&iidStack);
     while ((ret = cmsObj_getNext(MDMOID_LAN_WLAN, &iidStack, (void **) &lanWlanObj)) == CMSRET_SUCCESS)
     {
        if ( lanWlanObj->X_BROADCOM_COM_IfName != NULL ) {
            if(intfList[0] != '\0')
           {
               strcat(intfList, "|");
            }
        
            strcat(intfList, lanWlanObj->X_BROADCOM_COM_IfName);
		
            numOfIntf++; 
        }

        cmsObj_free((void **)&lanWlanObj);
     }
#endif          
   cmsLog_debug("intfList=%s", intfList);

   fprintf(fs, "<script language=\"javascript\">\n");
   fprintf(fs, "<!-- hide\n");
   fprintf(fs, "{\n");
   fprintf(fs, "   var i = 0;\n");
   fprintf(fs, "   var brdId = '%s';\n", glbWebVar.boardID);
   fprintf(fs, "   var interfaceInfo = '%s';\n", intfList);
   fprintf(fs, "   var interfaces = interfaceInfo.split('|');\n");
   fprintf(fs, "   var intfDisp = '';\n");
   fprintf(fs, "   var brdIntf = '';\n");
   
   fprintf(fs, "   for (i = 0; i < interfaces.length; i++) {\n");
   fprintf(fs, "      brdIntf = brdId + '|' + interfaces[i];\n");
   fprintf(fs, "      intfDisp = getUNameByLName(brdIntf);\n");
   fprintf(fs, "      brdIntf = '';\n");
    
   fprintf(fs, "      document.write(\"<tr>\");\n");
   fprintf(fs, "      document.write(\"<td align='center'>\"+intfDisp+\"</td>\");\n");
   fprintf(fs, "      document.write(\"</tr>\");\n");
   fprintf(fs, "   }\n");

   fprintf(fs, "}\n");
   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");
   
   fprintf(fs, "</table><br>\n");

   fprintf(fs, "</form>\n");
   fprintf(fs, "</blockquote>\n");
   fprintf(fs, "</body>\n");
   fprintf(fs, "</html>\n");
   fflush(fs);
}

static void writePortMapScript(FILE *fs, UBOOL8 checked)
{
   cmsLog_debug("tmpVirtualPorts=%d", checked);


   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   fprintf(fs, "function frmLoad() {\n");
   fprintf(fs, "   with ( document.forms[0] ) {\n");
   if (glbWebVar.numberOfVirtualPorts > 1) {
      if (checked) 
         fprintf(fs, "      chkVirtPortsCfg.checked = true;\n");
      else
         fprintf(fs, "      chkVirtPortsCfg.checked = false;\n");
      fprintf(fs, "      pmapCfgName.disabled = true;\n");
   }
   fprintf(fs, "   }\n");
   fprintf(fs, "}\n\n");

   //Save and Apply Click
   fprintf(fs, "function applySaveClick() {\n");

   fprintf(fs, "   var loc = 'lanportscfg.cmd?action=applysave';\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   // Config Enable/Disable
   fprintf(fs, "function virtualPortsEnableClick() {\n");
   fprintf(fs, "with ( document.forms[0] ) {\n");
   fprintf(fs, "   var lName = '';\n");
   fprintf(fs, "   var brdId = '%s';\n", glbWebVar.boardID);
   fprintf(fs, "   var ifName = pmapCfgName.value;\n");
   fprintf(fs, "   var loc = 'lanports.cmd?action=config';\n\n");
   fprintf(fs, "   lName = getLNameByUName(brdId + '|' + ifName);\n");
   fprintf(fs, "   if (chkVirtPortsCfg.checked == true)\n");
   fprintf(fs, "       loc += '&virtPortsEnabled=' + '1';\n");
   fprintf(fs, "     else\n");
   fprintf(fs, "       loc += '&virtPortsEnabled=' + '0';\n");
   fprintf(fs, "   loc += '&pmapCfgName=' + lName;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n");
   fprintf(fs, "}\n\n");
   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");
   
}
#endif

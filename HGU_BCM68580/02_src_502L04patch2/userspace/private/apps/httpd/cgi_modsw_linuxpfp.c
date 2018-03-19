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


#ifdef DMP_DEVICE2_X_BROADCOM_COM_MODSW_LINUXPFP_1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "cms_util.h"
#include "cgi_main.h"
#include "cgi_util.h"
#include "cms_msg.h"
#include "cms_msg_modsw.h"




void writeModSwPFPScript(FILE *fs, UBOOL8 refresh)
{
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   fprintf(fs, "function DisableClick(EEName) {\n");
   fprintf(fs, "   var loc = 'swmodulesEE.cmd?action=Disable&EEName=' + EEName;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function EnableClick(EEName) {\n");
   fprintf(fs, "   var loc = 'swmodulesEE.cmd?action=Enable&EEName=' + EEName;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   if(refresh)
   {
      fprintf(fs, "function refresh() {\n");
      fprintf(fs, "   var loc = 'swmodulesEE.cmd';\n");
      fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
      fprintf(fs, "   eval(code);\n");
      fprintf(fs, "}\n\n");

      fprintf(fs, "function frmLoad() {\n");
      fprintf(fs, "   setTimeout(\"refresh()\",10000);\n");
      fprintf(fs, "}\n\n");
   }
   
   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");

}  /* End of writeWanCfgScript() */

void cgiModSwPFPView(FILE *fs, UBOOL8 refresh)
{
   
   /* write html header */
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta http-equiv='refresh' content='20'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");

   /* write Java Script */
   writeModSwPFPScript(fs, refresh);

   /* write body */
   if(refresh)
      fprintf(fs, "<title></title>\n</head>\n<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");
   else
      fprintf(fs, "<title></title>\n</head>\n<body>\n<blockquote>\n<form>\n");
   
   /* write table */
   fprintf(fs, "<center>\n");
   fprintf(fs, "<b>Modular Software Linux Primary Firmware Patch (PFP) </b><br><br>\n");
   fprintf(fs, "Under development: you should be able to view and manage Primary Firmware Patches on this page<br><br>\n");
   fprintf(fs, "Display some fake info for now...<br><br>\n");
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");

   /* write table header */
   fprintf(fs, "   <tr align='center'>\n");
   fprintf(fs, "      <td class='hd'>Name</td>\n");
   fprintf(fs, "      <td class='hd'>Install Date</td>\n");
   fprintf(fs, "      <td class='hd'>Status</td>\n");
   fprintf(fs, "      <td class='hd'>Rollback</td>\n");

   {
      fprintf(fs, "   <tr align='center'>\n");

      /* Patch name */
      fprintf(fs, "      <td>%s</td>\n", "Patch1");

      fprintf(fs, "      <td>%s</td>\n", "10/4/2012");

      /* fake a status for now */
      fprintf(fs, "      <td>%s</td>\n", "Installed");
   
      /* Some button*/
      fprintf(fs, "      <td align='center'><input type='button' disabled='1' onClick='DisableClick(\"%s\")' value='Rollback'></td>\n",
         "Patch1");
   
      fprintf(fs, "   </tr>\n");
   }  
 
   fprintf(fs, "</table><br><br>\n");   
   
   fprintf(fs, "</center>\n</form>\n</blockquote>\n</body>\n</html>\n");

   fflush(fs);
   
}


void cgiModSwPFP(char *query, FILE *fs)
{
   char action[BUFLEN_256];

   cgiGetValueByName(query, "action", action);

#ifdef later
   if ( cmsUtl_strcmp(action, "Disable") == 0 )
      cgiSwModulesEEApply(query, fs, FALSE);
   else if ( cmsUtl_strcmp(action, "Enable") == 0 )
      cgiSwModulesEEApply(query, fs, TRUE);
   else if ( cmsUtl_strcmp(action, "view") == 0 )
      cgiSwModulesEEView(fs, FALSE);
   else
#endif

   cgiModSwPFPView(fs, FALSE);
}


#endif  /* DMP_DEVICE2_X_BROADCOM_COM_MODSW_LINUXPFP_1 */

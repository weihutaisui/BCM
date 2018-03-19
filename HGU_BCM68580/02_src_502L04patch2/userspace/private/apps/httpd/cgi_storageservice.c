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

#ifdef DMP_STORAGESERVICE_1

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


void cgiStorageServiceView( FILE *fs, int refresh __attribute__((unused)))
{

   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

//   PhysicalMediumObject *physicalMediumObj =NULL;
   LogicalVolumeObject *logicalVolumeObj = NULL;
//   StorageServiceObject *storageServiceObj=NULL;

   CmsRet ret = CMSRET_SUCCESS;

   // write header
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");                                                                   
   // write Java Script
   //
   // write body
   fprintf(fs, "</head>\n<body>\n<blockquote>\n<form>\n");
   fprintf(fs, "<b>Storage Service</b><br><br>\n");
   fprintf(fs, "The Storage service allows you to use Storage devices with modem \n");
   fprintf(fs, "to be more easily accessed <br><br>\n");
   fprintf(fs, "<center>\n");
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");
   // write table header
   fprintf(fs, "   <tr>\n");
   fprintf(fs, "      <td class='hd'>Volumename</td>\n");
   //fprintf(fs, "      <td class='hd'>PhysicalMedium</td>\n");
   fprintf(fs, "      <td class='hd'>FileSystem</td>\n");
   fprintf(fs, "      <td class='hd'>Total Space</td>\n");
   fprintf(fs, "      <td class='hd'>Used Space</td>\n");
   fprintf(fs, "   </tr>\n");


   while((ret=cmsObj_getNext( MDMOID_LOGICAL_VOLUME, &iidStack, (void **) &logicalVolumeObj))==CMSRET_SUCCESS){
      fprintf(fs, "    <tr>\n");
      fprintf(fs, "      <td>%s</td>\n", logicalVolumeObj->name);
      //fprintf(fs, "      <td>%s</td>\n", logicalVolumeObj->physicalReference);
      fprintf(fs, "      <td>%s</td>\n", logicalVolumeObj->fileSystem);
      fprintf(fs, "      <td>%d</td>\n", logicalVolumeObj->capacity);
      fprintf(fs, "      <td>%d</td>\n", logicalVolumeObj->usedSpace);
      fprintf(fs, "  </tr>\n");
      cmsObj_free((void **)&logicalVolumeObj);
   }

   fprintf(fs,"</table><br>\n");
   fprintf(fs, "</center>\n");
   fprintf(fs, "</form>\n");
   fprintf(fs, "</blockquote>\n");
   fprintf(fs, "</body>\n");
   fprintf(fs, "</html>\n");
   fflush(fs);
}

void cgiStorageServiceCfg(char *query, FILE *fs) 
{
   char action[BUFLEN_264];
   cgiGetValueByName(query, "action", action );

   if ( cmsUtl_strcmp(action, "add") == 0 )
      printf("not yet supported\n");
   else
      cgiStorageServiceView(fs, FALSE);	
}

#ifdef SUPPORT_SAMBA

static void writeStorageCfgScript(FILE *fs, char *addLoc, char *removeLoc) 
{

   /* write html header */
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");

   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   fprintf(fs, "function addClick() {\n");
   fprintf(fs, "   var code = 'location=\"' + '%s.html' + '\"';\n", addLoc); 
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

static void writeXtmFooter(FILE *fs)
{
   fprintf(fs, "</table><br>\n");
   fprintf(fs, "<input type='button' onClick='addClick()' value='Add'>\n");
   fprintf(fs, "<input type='button' onClick='removeClick(this.form.rml)' value='Remove'>\n");
   fprintf(fs, "</center>\n</form>\n</blockquote>\n</body>\n</html>\n");
   fflush(fs);
}

static void cgiStorageUserAccountCfgView(FILE *fs) 
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack childIidStack = EMPTY_INSTANCE_ID_STACK;

   StorageServiceObject *storageServiceObj=NULL;
   UserAccountObject *userAccountObj=NULL;

   CmsRet ret = CMSRET_SUCCESS;

   if(( ret =cmsObj_getNext(MDMOID_STORAGE_SERVICE, &iidStack, (void **) &storageServiceObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get StorageService object, ret=%d", ret);
      return ;
   } 

   /* write Java Script */
   writeStorageCfgScript(fs,"storageusraccadd", "storageuseraccountcfg");

   /* write table */
   fprintf(fs, "<b>Storage UserAccount Configuration</b><br><br>\n");
   fprintf(fs, "Choose Add, or Remove to configure User Accounts.<br><br>\n");
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");
   /* write table header */
   fprintf(fs, "   <tr align='center'>\n");
   fprintf(fs, "      <td class='hd'>UserName</td>\n");
   fprintf(fs, "      <td class='hd'>HomeDir</td>\n");
   //fprintf(fs, "      <td class='hd'>Samba Access</td>\n");
   //fprintf(fs, "      <td class='hd'>Ftp Access</td>\n");
   fprintf(fs, "      <td class='hd'>Remove</td>\n");
   fprintf(fs, "   </tr>\n");

   while(( ret =cmsObj_getNextInSubTree(MDMOID_USER_ACCOUNT, &iidStack, &childIidStack, (void **) &userAccountObj)) == CMSRET_SUCCESS)
   {

      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td>%s</td>\n", userAccountObj->username);
      fprintf(fs, "      <td>%s/%s</td>\n", userAccountObj->X_BROADCOM_volumeName, userAccountObj->username);
      //fprintf(fs, "      <td>%s</td>\n", userAccountObj->X_BROADCOM_AllowSMBAccess ? "Enabled" : "Disabled");
      //fprintf(fs, "      <td>%s</td>\n", userAccountObj->AllowFTPAccess ? "Enabled" : "Disabled");

      /* remove check box */
      fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s'></td>\n", userAccountObj->username);
      fprintf(fs, "   </tr>\n");

      cmsObj_free((void **) &userAccountObj);

   }

   cmsObj_free((void **) &storageServiceObj);

   writeXtmFooter(fs);


   /* reset atm to default value for next add  */
   //getDefaultWanDslLinkCfg(&glbWebVar);

}

static CmsRet cgiStorageUserAccountAdd(char *query, FILE *fs) 
{

   char *errorStr;

   cgiGetValueByName(query, "userName", glbWebVar.storageuserName);
   cgiGetValueByName(query, "Password", glbWebVar.storagePassword);
   cgiGetValueByName(query, "volumeName", glbWebVar.storagevolumeName);
   
   if (dalStorage_addUserAccount(&glbWebVar, &errorStr) != CMSRET_SUCCESS)
   {
      do_ej("/webs/storageusraccadderr.html", fs);
      return CMSRET_INTERNAL_ERROR;
   }
   else
   {
      cmsLog_debug("dalStorage_addUserAccount successful");
      /*
       * storage user account add was successful, tell handle_request to save the config
       * before releasing the lock.
       */
      glbSaveConfigNeeded = TRUE;
   }

   return CMSRET_SUCCESS;
}


static CmsRet cgiStorageUserAccountRemove(char *query, FILE *fs __attribute__((unused))) 
{
   char *pToken = NULL, *pLast = NULL;
   char lst[BUFLEN_1024];
   CmsRet ret=CMSRET_SUCCESS;
   
   cgiGetValueByName(query, "rmLst", lst);
   pToken = strtok_r(lst, ", ", &pLast);

   while (pToken != NULL)
   {

      if ((ret = dalStorage_deleteUserAccount(pToken)) != CMSRET_SUCCESS)
      {
         cmsLog_error("dalStorage_deleteUserAccount failed for %s (ret=%d)", pToken, ret);
         return ret;
      }

      pToken = strtok_r(NULL, ", ", &pLast);

   } /* end of while loop over list of user accounts to delete */

   /*
    * Whether or not there were errors during the delete,
    * save our config.
    */
   glbSaveConfigNeeded = TRUE;

   return ret;
}

void cgiStorageUserAccountCfg(char *query, FILE *fs) 
{
   char action[BUFLEN_256];

   cgiGetValueByName(query, "action", action);
   if (cmsUtl_strcmp(action, "add") == 0)
   {
      if (cgiStorageUserAccountAdd(query, fs) != CMSRET_SUCCESS) 
      {
         return;
      }
   }
   else if (cmsUtl_strcmp(action, "remove") == 0)
   {
      if (cgiStorageUserAccountRemove(query, fs) != CMSRET_SUCCESS)
      {
         return;
      }
   }

   /* for DSL ATM Interface display */
   cgiStorageUserAccountCfgView(fs);

}

#endif /*SUPPORT_SAMBA*/

#endif

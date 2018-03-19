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


#ifdef SUPPORT_QOS

/*!\file cgi_qos_policer.c
 * \brief This file contains common, data-model independent QoS code as well
 *        as TR98 specific QOS code.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <linux/if_ether.h>

#include "cms.h"
#include "cms_qdm.h"
#include "cms_dal.h"
#include "cms_qos.h"
#include "httpd.h"
#include "cgi_cmd.h"
#include "cgi_sec.h"
#include "cgi_main.h"
#include "cgi_qos.h"
#include "secapi.h"
#include "syscall.h"



/* Local functions */
static void cgiQosPolicerAdd(char *query, FILE *fs);
static void writeQosPolicerScript(FILE *fs, int numOfEntry);


void cgiQosPolicer(char *query, FILE *fs)
{
   char action[BUFLEN_264];

   cgiGetValueByName(query, "action", action);
   if (strcmp(action, "add") == 0)
   {
      cgiQosPolicerAdd( query, fs );
   }
   else if (strcmp(action, "enable") == 0)
   {
      cgiQosPolicerEnable(query, fs);

      /* refresh the page */
      cgiQosPolicerView(fs);
   }
   else if (strcmp(action, "remove") == 0)
   {
      cgiQosPolicerRemove(query, fs);

      /* refresh the page */
      cgiQosPolicerView(fs);
   }
   else
   {
      cgiQosPolicerView(fs);
   }
}  /* End of cgiQosPolicer() */

void writeQosPolicerScript(FILE *fs, int numOfEntry)
{
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   fprintf(fs, "function addClick() {\n");
   fprintf(fs, "   var loc = 'qospoliceradd.html';\n\n");
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function enableClick(ebl) {\n");
   fprintf(fs, "   var lst = '';\n");
   if (numOfEntry > 0)
   {
      fprintf(fs, "   if (ebl.length > 0)\n");
      fprintf(fs, "      for (i = 0; i < ebl.length; i++) {\n");
      fprintf(fs, "         if ( ebl[i].checked == true )\n");
      fprintf(fs, "            lst += '1,';\n");
      fprintf(fs, "         else\n");
      fprintf(fs, "            lst += '0,';\n");
      fprintf(fs, "      }\n");
      fprintf(fs, "   else if ( ebl.checked == true )\n");
      fprintf(fs, "      lst = '1';\n");
      fprintf(fs, "   else\n");
      fprintf(fs, "      lst = '0';\n");
   }
   fprintf(fs, "   var loc = 'qospolicer.cmd?action=enable&eblLst=' + lst;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function removeClick(rml) {\n");
   fprintf(fs, "   var lst = '';\n");
   if (numOfEntry > 0)
   {
      fprintf(fs, "   if (rml.length > 0)\n");
      fprintf(fs, "      for (i = 0; i < rml.length; i++) {\n");
      fprintf(fs, "         if ( rml[i].checked == true )\n");
      fprintf(fs, "            lst += '1,';\n");
      fprintf(fs, "         else\n");
      fprintf(fs, "            lst += '0,';\n");
      fprintf(fs, "      }\n");
      fprintf(fs, "   else if ( rml.checked == true )\n");
      fprintf(fs, "      lst = '1';\n");
      fprintf(fs, "   else\n");
      fprintf(fs, "      lst = '0';\n");
   }
   fprintf(fs, "   var loc = 'qospolicer.cmd?action=remove&rmLst=' + lst;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");

}  /* End of writeQosPolicerScript() */

void cgiQosPolicerView( FILE *fs )
{
   // write header
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
                                                                                
   // write body
   fprintf(fs, "</head>\n<body>\n<blockquote>\n");
                                                                                
   fprintf(fs, "<form>\n");                                                                             
   fprintf(fs, "<b>QoS Policer Setup -- maximum %d policers can be configured.</b><br><br>\n", QOS_POLICER_MAX_ENTRY);
   fprintf(fs, "To add a policer, click the <b>Add</b> button.<br>\n");
   fprintf(fs, "To remove policers, check their remove-checkboxes, then click the <b>Remove</b> button.<br>\n");
   fprintf(fs, "The <b>Enable</b> button will scan through every policers in the table. \n");
   fprintf(fs, "Policers with enable-checkbox checked will be enabled. Policers with enable-checkbox un-checked will be disabled.<br>\n");
   fprintf(fs, "The enable-checkbox also shows status of the policer after page reload.<br>\n");
   fprintf(fs, "<br>\n");

   /* write alert info */
   if (qdmQos_isQosEnabled() == FALSE)
   {
      fprintf(fs, "<b>The QoS function has been disabled. Policers would not take effects.</b><br><br>\n");
   }
      
   fprintf(fs, "<table border='1' cellpadding='2' cellspacing='0'>\n");
   // write table header
   fprintf(fs, "   <tr align='center'>\n");
   fprintf(fs, "      <td class='hd'>Name</td>\n");
   fprintf(fs, "      <td class='hd'>Key</td>\n");
   fprintf(fs, "      <td class='hd'>MeteringType</td>\n");
   fprintf(fs, "      <td class='hd'>Committed Rate(kbps)</td>\n");
   fprintf(fs, "      <td class='hd'>Committed BurstSize(bytes)</td>\n");
   fprintf(fs, "      <td class='hd'>Excess BurstSize(bytes)</td>\n");
   fprintf(fs, "      <td class='hd'>Peak Rate(kbps)</td>\n");
   fprintf(fs, "      <td class='hd'>Peak BurstSize(bytes)</td>\n");
   fprintf(fs, "      <td class='hd'>Conform Action</td>\n");
   fprintf(fs, "      <td class='hd'>PartialConform Action</td>\n");
   fprintf(fs, "      <td class='hd'>NonConform Action</td>\n");
   fprintf(fs, "      <td class='hd'>Enable</td>\n");
   fprintf(fs, "      <td class='hd'>Remove</td>\n");
   fprintf(fs, "   </tr>\n");

   cgiQosPolicerViewBody(fs);

   fprintf(fs,"</table><br>\n");
   fprintf(fs, "<input type='button' onClick='addClick()' value='Add'>\n");
   fprintf(fs, "<input type='button' onClick='enableClick(this.form.enbl)' value='Enable'>\n");
   fprintf(fs, "<input type='button' onClick='removeClick(this.form.rml)' value='Remove'>\n");
   fprintf(fs, "</form>\n");

   // write Java Script
   writeQosPolicerScript(fs, glbEntryCt);

   fprintf(fs, "</blockquote>\n");
   fprintf(fs, "</body>\n");
   fprintf(fs, "</html>\n");
   fflush(fs);

}  /* End of cgiQosPolicerView() */



#ifdef DMP_QOS_1

void cgiQosPolicerViewBody_igd(FILE *fs)
{
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   QMgmtPolicerObject *pObj = NULL;
   CmsRet ret;

   while ((ret = cmsObj_getNext(MDMOID_Q_MGMT_POLICER, &iidStack, (void **)&pObj)) == CMSRET_SUCCESS)
   {
      fprintf(fs, "    <tr align='center'>\n");

      /* policer name */
      fprintf(fs, "      <td>%s</td>\n", pObj->X_BROADCOM_COM_PolicerName);
      
      /* policer key */
      fprintf(fs, "      <td>%d</td>\n", iidStack.instance[iidStack.currentDepth-1]);
      
      /* metering type */
      fprintf(fs, "      <td>%s</td>\n", pObj->meterType);
      
      /* committed rate */
      fprintf(fs, "      <td>%d</td>\n", pObj->committedRate/1000);

      /* committed burst size */
      fprintf(fs, "      <td>%d</td>\n", pObj->committedBurstSize);

      if (cmsUtl_strcmp(MDMVS_SINGLERATETHREECOLOR, pObj->meterType) == 0)
      {
         /* excess burst size */
         fprintf(fs, "      <td>%d</td>\n", pObj->excessBurstSize);
      }
      else
      {
         fprintf(fs, "      <td>-</td>\n");
      }

      if (cmsUtl_strcmp(MDMVS_TWORATETHREECOLOR, pObj->meterType) == 0)
      {
         /* peak rate */
         fprintf(fs, "      <td>%d</td>\n", pObj->peakRate/1000);

         /* peak burst size */
         fprintf(fs, "      <td>%d</td>\n", pObj->peakBurstSize);
      }
      else
      {
         fprintf(fs, "      <td>-</td>\n");
         fprintf(fs, "      <td>-</td>\n");
      }

      /* conforming action */
      if (cmsUtl_strcmp(MDMVS_NULL, pObj->conformingAction) == 0 ||
          cmsUtl_strcmp(MDMVS_DROP, pObj->conformingAction) == 0)
      {
         fprintf(fs, "      <td>%s</td>\n", pObj->conformingAction);
      }
      else
      {
         fprintf(fs, "      <td>%s</td>\n", cgiQosDscpMarkToName(atoi(pObj->conformingAction)));
      }

      if (cmsUtl_strcmp(MDMVS_SIMPLETOKENBUCKET, pObj->meterType) != 0)
      {
         /* partial conforming action */
         if (cmsUtl_strcmp(MDMVS_NULL, pObj->partialConformingAction) == 0 ||
             cmsUtl_strcmp(MDMVS_DROP, pObj->partialConformingAction) == 0)
         {
            fprintf(fs, "      <td>%s</td>\n", pObj->partialConformingAction);
         }
         else
         {
            fprintf(fs, "      <td>%s</td>\n", cgiQosDscpMarkToName(atoi(pObj->partialConformingAction)));
         }
      }
      else
      {
         fprintf(fs, "      <td>-</td>\n");
      }

      /* non-conforming action */
      if (cmsUtl_strcmp(MDMVS_NULL, pObj->nonConformingAction) == 0 ||
          cmsUtl_strcmp(MDMVS_DROP, pObj->nonConformingAction) == 0)
      {
         fprintf(fs, "      <td>%s</td>\n", pObj->nonConformingAction);
      }
      else
      {
         fprintf(fs, "      <td>%s</td>\n", cgiQosDscpMarkToName(atoi(pObj->nonConformingAction)));
      }

      /* policer enable */
      fprintf(fs, "      <td align='center'><input type='checkbox' name='enbl' %s></td>\n", pObj->policerEnable? "checked" : "");
      fprintf(fs, "      <td align='center'><input type='checkbox' name='rml'></td>\n");

      fprintf(fs, "  </tr>\n");
      fflush(fs);
      cmsObj_free((void **)&pObj);
      glbEntryCt++;
   }
}
#endif  /* DMP_QOS_1 */


#ifdef SUPPORT_QOS

void cgiQosPolicerAdd( char *query, FILE *fs )
{
   char cmd[WEB_MD_BUF_SIZE_MAX];
   CmsQosPolicerInfo policerInfo;
   UBOOL8 isDuplicate;
   CmsRet ret;

   cmd[0] = '\0';
   memset((void *) &policerInfo, 0, sizeof(policerInfo));

   /* committed rate */    
   if (cgiGetValueByName(query, "cir", cmd) == CGI_STS_OK)
   {
      policerInfo.committedRate = atoi(cmd) * 1000;
   }

   /* committed burst size */    
   if (cgiGetValueByName(query, "cbs", cmd) == CGI_STS_OK)
   {
      policerInfo.committedBurstSize = atoi(cmd);
   }

   /* excess burst size */    
   if (cgiGetValueByName(query, "ebs", cmd) == CGI_STS_OK)
   {
      policerInfo.excessBurstSize = atoi(cmd);
   }

   /* peak rate */    
   if (cgiGetValueByName(query, "pir", cmd) == CGI_STS_OK)
   {
      policerInfo.peakRate = atoi(cmd) * 1000;
   }

   /* peak burst size */    
   if (cgiGetValueByName(query, "pbs", cmd) == CGI_STS_OK)
   {
      policerInfo.peakBurstSize = atoi(cmd);
   }
   
   /* meter type */    
   if (cgiGetValueByName(query, "meter", cmd) == CGI_STS_OK)
   {
      cmsUtl_strncpy(policerInfo.meterType, cmd,
                     sizeof(policerInfo.meterType));
   }

   /* conforming action */    
   if (cgiGetValueByName(query, "conform", cmd) == CGI_STS_OK)
   {
      cmsUtl_strncpy(policerInfo.conformingAction, cmd,
                     sizeof(policerInfo.conformingAction));
   }

   /* partial-conforming action */    
   if (cgiGetValueByName(query, "partialconform", cmd) == CGI_STS_OK)
   {
      cmsUtl_strncpy(policerInfo.partialConformingAction, cmd,
                     sizeof(policerInfo.partialConformingAction));
   }

   /* non-conforming action */    
   if (cgiGetValueByName(query, "nonconform", cmd) == CGI_STS_OK)
   {
      cmsUtl_strncpy(policerInfo.nonConformingAction, cmd,
                     sizeof(policerInfo.nonConformingAction));
   }

   /* policer name */
   if (cgiGetValueByName(query, "policerName", cmd) == CGI_STS_OK)
   {
      cmsUtl_strncpy(policerInfo.name, cmd, sizeof(policerInfo.name));
   }

   /* enable */
   if (cgiGetValueByName(query, "enable", cmd) == CGI_STS_OK)
   {
      policerInfo.enable = atoi(cmd);
   }

   /* check to see if the policer already exists */
   if ((ret = dalQos_duplicatePolicerCheck(&policerInfo, &isDuplicate)) != CMSRET_SUCCESS)
   {
      cmsLog_error("dalQos_duplicatePolicerCheck returns error. ret=%d", ret);
      sprintf(cmd, "Duplicate check failed. ret=%d", ret);
      cgiWriteMessagePage(fs, "QoS Policer Config Error", cmd, "qospolicer.cmd?action=viewcfg");
      return;
   }
   if (isDuplicate)
   {
      sprintf(cmd, "Policer already exists.");
      cmsLog_error(cmd);
      cgiWriteMessagePage(fs, "QoS Policer Config Error", cmd, "qospolicer.cmd?action=viewcfg");
      return;
   }

   /* Create the policer object instance */
   if ((ret = dalQos_policerAdd(&policerInfo)) != CMSRET_SUCCESS)
   {
      cmsLog_error("dalQos_policerAdd returns error. ret=%d", ret);
      sprintf(cmd, "Add policer failed. ret=%d", ret);
      cgiWriteMessagePage(fs, "QoS Policer Config Error", cmd, "qospolicer.cmd?action=viewcfg");
   }
   else
   {
      glbSaveConfigNeeded = TRUE;
      cgiQosPolicerView(fs);
   }

}  /* End of cgiQosPolicerAdd() */
#endif  /* SUPPORT_QOS */


#ifdef DMP_QOS_1

void cgiQosPolicerRemove_igd(char *query, FILE *fs)
{
   char cmd[WEB_MD_BUF_SIZE_MAX];
   char lst[WEB_BUF_SIZE_MAX];
   char *last = NULL;
   char *check;
   UBOOL8 isRefered;
   InstanceIdStack iidStack, iidStackPrev;
   QMgmtPolicerObject *pObj = NULL;
   CmsRet ret;

   lst[0] = '\0';
   if (cgiGetValueByName(query, "rmLst", lst) != CGI_STS_OK)
   {
      return;
   }

   /* loop through the policer table */
   INIT_INSTANCE_ID_STACK(&iidStack);
   iidStackPrev = iidStack;
   while ((ret = cmsObj_getNext(MDMOID_Q_MGMT_POLICER, &iidStack, (void **)&pObj)) == CMSRET_SUCCESS)
   {
      if (last == NULL)
      {
         check = strtok_r(lst, ",", &last);
      }
      else
      {
         check = strtok_r(NULL, ",", &last);
      }
      if (check == NULL)
      {
         cmsObj_free((void **)&pObj);
         break;   /* done */
      }
      if (atoi(check) == 1)
      {
         /* check to see if the policer is being refered by a class rule */
         if ((ret = qdmQos_referenceCheckLocked(CMS_QOS_REF_TARGET_POLICER,
                                          PEEK_INSTANCE_ID(&iidStack),
                                          &isRefered)) != CMSRET_SUCCESS)
         {
            cmsLog_error("qdmQos_referenceCheck returns error. ret=%d", ret);
            sprintf(cmd, "Reference check failed. ret=%d", ret);
            cgiWriteMessagePage(fs, "QoS Policer Config Error", cmd, "qospolicer.cmd?action=viewcfg");
            cmsObj_free((void **)&pObj);
            break;
         }
         if (isRefered)
         {
            sprintf(cmd, "Policer can not be deleted. It is being refered by a Class rule.");
            cmsLog_error(cmd);
            cgiWriteMessagePage(fs, "QoS Policer Config Error", cmd, "qospolicer.cmd?action=viewcfg");
            cmsObj_free((void **)&pObj);
            break;
         }

         cmsLog_debug("Deleting policer instance %d", PEEK_INSTANCE_ID(&iidStack));
         if ((ret = cmsObj_deleteInstance(MDMOID_Q_MGMT_POLICER, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Delete policer returns error, ret = %d", ret);
            sprintf(cmd, "Delete policer failed. ret=%d", ret);
            cgiWriteMessagePage(fs, "QoS Policer Config Error", cmd, "qospolicer.cmd?action=viewcfg");
            cmsObj_free((void **)&pObj);
            break;
         }
         glbSaveConfigNeeded = TRUE;

         /* since this instance has been deleted, we want to set the iidStack to
          * the previous instance, so that we can continue to do getNext.
          */
         iidStack = iidStackPrev;
      }
      /* save this iidStack in case we want to do a getNext from this instance */
      iidStackPrev = iidStack;
      
      cmsObj_free((void **)&pObj);
   }
}  /* End of cgiQosPolicerRemove() */

void cgiQosPolicerEnable_igd(char *query, FILE *fs)
{
   char cmd[WEB_MD_BUF_SIZE_MAX];
   char lst[WEB_BUF_SIZE_MAX];
   char *last = NULL;
   char *check;
   UBOOL8 isRefered;
   InstanceIdStack iidStack;
   QMgmtPolicerObject *pObj = NULL;
   CmsRet ret;

   lst[0] = '\0';
   if (cgiGetValueByName(query, "eblLst", lst) != CGI_STS_OK)
   {
      return;
   }

   /* loop through the policer table */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_Q_MGMT_POLICER, &iidStack, (void **)&pObj)) == CMSRET_SUCCESS)
   {
      if (last == NULL)
      {
         check = strtok_r(lst, ",", &last);
      }
      else
      {
         check = strtok_r(NULL, ",", &last);
      }
   
      if (check == NULL)
      {
         cmsObj_free((void **)&pObj);
         break;   /* done */
      }
   
      if (atoi(check) == 1)
      {
         if (!pObj->policerEnable)
         {
            /* enable policer */
            pObj->policerEnable = TRUE;
            if ((ret = cmsObj_set(pObj, &iidStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("cmsObj_set returns error. ret=%d", ret);
               sprintf(cmd, "Enable policer failed. ret=%d", ret);
               cgiWriteMessagePage(fs, "QoS Policer Config Error", cmd, "qospolicer.cmd?action=viewcfg");
               cmsObj_free((void **)&pObj);
               break;
            }
            else
            {
               glbSaveConfigNeeded = TRUE;
            }
         }
      }
      else
      {
         if (pObj->policerEnable)
         {
            /* disable policer */
            /* check to see if the policer is being refered by a class rule */
            if ((ret = qdmQos_referenceCheckLocked(CMS_QOS_REF_TARGET_POLICER,
                                             iidStack.instance[iidStack.currentDepth - 1],
                                             &isRefered)) != CMSRET_SUCCESS)
            {
               cmsLog_error("qdmQos_referenceCheck returns error. ret=%d", ret);
               sprintf(cmd, "Reference check failed. ret=%d", ret);
               cgiWriteMessagePage(fs, "QoS Policer Config Error", cmd, "qospolicer.cmd?action=viewcfg");
               cmsObj_free((void **)&pObj);
               break;
            }
            if (isRefered)
            {
               sprintf(cmd, "Policer can not be disabled. It is being refered by a Class rule.");
               cmsLog_error(cmd);
               cgiWriteMessagePage(fs, "QoS Policer Config Error", cmd, "qospolicer.cmd?action=viewcfg");
               cmsObj_free((void **)&pObj);
               break;
            }

            pObj->policerEnable = FALSE;
            if ((ret = cmsObj_set(pObj, &iidStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("cmsObj_set returns error. ret=%d", ret);
               sprintf(cmd, "Disable policer failed. ret=%d", ret);
               cgiWriteMessagePage(fs, "QoS Policer Config Error", cmd, "qospolicer.cmd?action=viewcfg");
               cmsObj_free((void **)&pObj);
               break;
            }
            else
            {
               glbSaveConfigNeeded = TRUE;
            }
         }
      }
      cmsObj_free((void **)&pObj);
   }
}  /* End of cgiQosPolicerEnable() */

void cgiGetQosPolicerInfo_igd(int argc __attribute__((unused)),
                          char **argv __attribute__((unused)),
                          char *varValue)
{
   InstanceIdStack iidStack;
   QMgmtPolicerObject *pObj = NULL;
   char varValue1[BUFLEN_512];
   char meterType[BUFLEN_4];

   varValue[0]  = '\0';

   /* loop through the policer table */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while (cmsObj_getNext(MDMOID_Q_MGMT_POLICER, &iidStack, (void **)&pObj) == CMSRET_SUCCESS)
   {
      if (pObj->policerEnable)
      {
         if (cmsUtl_strcmp(MDMVS_SIMPLETOKENBUCKET, pObj->meterType) == 0)
         {
            strcpy(meterType, "tb");
         }
         else if (cmsUtl_strcmp(MDMVS_SINGLERATETHREECOLOR, pObj->meterType) == 0)
         {
            strcpy(meterType, "1r");
         }
         else
         {
            strcpy(meterType, "2r");
         }

         varValue1[0] = '\0';
         sprintf(varValue1, "%d:Key%d&%s,cir%dcbs%debs%dpir%dpbs%d,%s-%s-%s|",
                 iidStack.instance[iidStack.currentDepth-1],
                 iidStack.instance[iidStack.currentDepth-1],
                 meterType,
                 pObj->committedRate/1000,
                 pObj->committedBurstSize,
                 pObj->excessBurstSize,
                 pObj->peakRate/1000,
                 pObj->peakBurstSize,
                 (cmsUtl_strcmp(MDMVS_NULL, pObj->conformingAction) == 0 ||
                  cmsUtl_strcmp(MDMVS_DROP, pObj->conformingAction) == 0)? 
                     pObj->conformingAction :
                     cgiQosDscpMarkToName(atoi(pObj->conformingAction)),
                 (cmsUtl_strcmp(MDMVS_NULL, pObj->partialConformingAction) == 0 ||
                  cmsUtl_strcmp(MDMVS_DROP, pObj->partialConformingAction) == 0)? 
                     pObj->partialConformingAction :
                     cgiQosDscpMarkToName(atoi(pObj->partialConformingAction)),
                 (cmsUtl_strcmp(MDMVS_NULL, pObj->nonConformingAction) == 0 ||
                  cmsUtl_strcmp(MDMVS_DROP, pObj->nonConformingAction) == 0)? 
                     pObj->nonConformingAction :
                     cgiQosDscpMarkToName(atoi(pObj->nonConformingAction)));
         strcat(varValue, varValue1);
      }
      cmsObj_free((void **)&pObj);
   }
}  /* End of cgiGetQosPolicerInfo() */
#endif  /* DMP_QOS_1 */


void cgiGetQosPolicerInfo(int argc, char **argv, char *varValue)
{
#if defined(SUPPORT_DM_LEGACY98)

   cgiGetQosPolicerInfo_igd(argc, argv, varValue);

#elif defined(SUPPORT_DM_HYBRID)

   cgiGetQosPolicerInfo_igd(argc, argv, varValue);

#elif defined(SUPPORT_DM_PURE181)

   cgiGetQosPolicerInfo_dev2(argc, argv, varValue);

#elif defined(SUPPORT_DM_DETECT)

   if (cmsMdm_isDataModelDevice2())
   {
      cgiGetQosPolicerInfo_dev2(argc, argv, varValue);
   }
   else
   {
      cgiGetQosPolicerInfo_dev2(argc, argv, varValue);
   }

#endif
}
#endif  /* SUPPORT_QOS */



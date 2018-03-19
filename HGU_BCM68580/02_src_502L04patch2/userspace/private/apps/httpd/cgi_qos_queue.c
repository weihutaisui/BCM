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


/* this file contains a mix of common QoS code and TR98 specific QoS code.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <linux/if_ether.h>

#include "cms.h"
#include "cms_dal.h"
#include "cms_qos.h"
#include "cms_qdm.h"
#include "httpd.h"
#include "cgi_cmd.h"
#include "cgi_sec.h"
#include "cgi_main.h"
#include "cgi_qos.h"
#include "secapi.h"
#include "syscall.h"
#include "bcmxtmcfg.h"

/* local constants */


/* Local functions */
static void cgiQosQueueAdd(char *query, FILE *fs);
static void cgiQosQueueView(FILE *fs);
static void writeQosQueueScript(FILE *fs, int numOfEntry);
#ifdef BRCM_WLAN
static void cgiQosWlQueueView(FILE *fs);
#endif

void cgiQosQueue(char *query, FILE *fs)
{
   char action[BUFLEN_264];

   cgiGetValueByName(query, "action", action);
   if (strcmp(action, "add") == 0)
   {
      cgiQosQueueAdd( query, fs );
   }
   else if (strcmp(action, "enable") == 0)
   {
      cgiQosQueueEnable(query, fs);

      /* refresh the page */
      cgiQosQueueView(fs);
   }
   else if (strcmp(action, "remove") == 0)
   {
      cgiQosQueueRemove(query, fs);

      /* refresh the page */
      cgiQosQueueView(fs);
   }
#ifdef BRCM_WLAN
   else if (strcmp(action, "view_wlq") == 0)
   {
      cgiQosWlQueueView(fs);
   }
#endif   
   else
   {
      cgiQosQueueView(fs);
   }
}  /* End of cgiQosQueue() */

void writeQosQueueScript(FILE *fs, int numOfEntry)
{
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   fprintf(fs, "function addClick() {\n");
   fprintf(fs, "   var loc = 'qosqueueadd.html';\n\n");
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
   fprintf(fs, "   var loc = 'qosqueue.cmd?action=enable&eblLst=' + lst;\n\n");
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
   fprintf(fs, "   var loc = 'qosqueue.cmd?action=remove&rmLst=' + lst;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");

}  /* End of writeQosQueueScript() */

void cgiQosQueueView( FILE *fs )
{
   char qMgmtEnbleBuf[BUFLEN_8]={0};
   UBOOL8 qMgmtEnable;


   // write header
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
                                                                                
   // write body
   fprintf(fs, "</head>\n<body>\n<blockquote>\n");
                                                                                
   fprintf(fs, "<form>\n");                                                                             
   fprintf(fs, "<b>QoS Queue Setup</b><br><br>\n");
   fprintf(fs, "In ATM mode, maximum %d queues can be configured.<br>\n", MAX_ATM_TRANSMIT_QUEUES);
#if defined(BCM_PON) || defined(CHIP_63268) || defined(CHIP_63138) || defined(CHIP_63381) || defined(CHIP_63148) || defined(CHIP_63158)
   fprintf(fs, "In PTM mode, maximum %d queues can be configured.<br>\n", MAX_PTM_TRANSMIT_QUEUES);
#endif
   fprintf(fs, "For each Ethernet interface, maximum %d queues can be configured.<br>\n", ETH_QOS_LEVELS);
   fprintf(fs, "For each Ethernet WAN interface, maximum %d queues can be configured.<br>\n", ETHWAN_QOS_LEVELS);
   fprintf(fs, "To add a queue, click the <b>Add</b> button.<br>\n");
   fprintf(fs, "To remove queues, check their remove-checkboxes, then click the <b>Remove</b> button.<br>\n");
   fprintf(fs, "The <b>Enable</b> button will scan through every queues in the table. \n");
   fprintf(fs, "Queues with enable-checkbox checked will be enabled. Queues with enable-checkbox un-checked will be disabled.<br>\n");
   fprintf(fs, "The enable-checkbox also shows status of the queue after page reload.<br>\n");
#if defined(CHIP_63138) || defined(CHIP_63381) || defined(CHIP_63148) || defined(CHIP_63158) || defined(CHIP_4908)
   fprintf(fs, "<br>\n");
   fprintf(fs, "<b>Note:<b> Ethernet LAN queue configuration only takes effect when all the queues of the interface have been configured.<br>\n");
#endif   
   fprintf(fs, "<br>\n");


   cgiGetQosMgmtEnbl(0, NULL, qMgmtEnbleBuf);
   qMgmtEnable = (qMgmtEnbleBuf[0] == '1');

   /* write alert info */
   if (!qMgmtEnable)
   {
      fprintf(fs, "<b>The QoS function has been disabled. Queues would not take effects.</b><br><br>\n");
   }
      
   fprintf(fs, "<table border='1' cellpadding='6' cellspacing='0'>\n");
   // write table header
   fprintf(fs, "   <tr align='center'>\n");
   fprintf(fs, "      <td class='hd'>&nbsp;&nbsp;Name&nbsp;&nbsp;</td>\n");
   fprintf(fs, "      <td class='hd'>Key</td>\n");
   fprintf(fs, "      <td class='hd'>Interface</td>\n");
   fprintf(fs, "      <td class='hd'>Qid</td>\n");
   fprintf(fs, "      <td class='hd'>Prec/Alg/Wght</td>\n");
#if defined(DMP_ADSLWAN_1)
   fprintf(fs, "      <td class='hd'>DslLatency</td>\n");
#endif
#if defined(SUPPORT_PTM)
   fprintf(fs, "      <td class='hd'>PtmPrio</td>\n");
#endif
#if defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_63158) || defined(CHIP_4908) || defined(CHIP_63268) 
   fprintf(fs, "      <td class='hd'>DropAlg/ LoMin/LoMax/HiMin/HiMax</td>\n");
#endif
#if defined(BCM_PON) || defined(CHIP_63268) || defined(CHIP_63381) || defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_63158) || defined(CHIP_4908) || (defined(DMP_ADSLWAN_1) && defined(SUPPORT_FAPCTL)) || defined(SUPPORT_RDPA) || defined(SUPPORT_BCMTM)
   fprintf(fs, "      <td class='hd'>ShapingRate (bps)</td>\n");
#endif
#if defined(BCM_PON) || defined(CHIP_63268) || defined(CHIP_63381) || defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_63158) || defined(CHIP_4908) || (defined(DMP_ADSLWAN_1) && defined(SUPPORT_FAPCTL)) || defined(SUPPORT_BCMTM)
   fprintf(fs, "      <td class='hd'>MinBitRate(bps)</td>\n");
   fprintf(fs, "      <td class='hd'>BurstSize(bytes)</td>\n");
#endif
   fprintf(fs, "      <td class='hd'>Enable</td>\n");
   fprintf(fs, "      <td class='hd'>Remove</td>\n");
   fprintf(fs, "   </tr>\n");

   glbEntryCt = 0;


   /*
    * Traverse the MDM to dump out each QoS queue.  This code is data model
    * dependent, so has been moved to a separate function.
    */
   cgiQosQueueViewBody(fs);


   fprintf(fs,"</table><br>\n");
   fprintf(fs, "<input type='button' onClick='addClick()' value='Add'>\n");
   fprintf(fs, "<input type='button' onClick='enableClick(this.form.enbl)' value='Enable'>\n");
   fprintf(fs, "<input type='button' onClick='removeClick(this.form.rml)' value='Remove'>\n");
   fprintf(fs, "</form>\n");

   // write Java Script
   writeQosQueueScript(fs, glbEntryCt);

   fprintf(fs, "</blockquote>\n");
   fprintf(fs, "</body>\n");
   fprintf(fs, "</html>\n");
   fflush(fs);

}  /* End of cgiQosQueueView() */


#ifdef BRCM_WLAN
void cgiQosWlQueueView( FILE *fs )
{
   char qMgmtEnbleBuf[BUFLEN_8]={0};
   UBOOL8 qMgmtEnable;


   // write header
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
                                                                                
   // write body
   fprintf(fs, "</head>\n<body>\n<blockquote>\n");
                                                                                
   fprintf(fs, "<form>\n");                                                                             
   fprintf(fs, "<b>QoS Wlan Queue Setup</b><br><br>\n");
   fprintf(fs, "Note: If WMM function is disabled in Wireless Page, queues related to wireless will not take effects.<br>\n");
   fprintf(fs, "<br>\n");


   cgiGetQosMgmtEnbl(0, NULL, qMgmtEnbleBuf);
   qMgmtEnable = (qMgmtEnbleBuf[0] == '1');

   /* write alert info */
   if (!qMgmtEnable)
   {
      fprintf(fs, "<b>The QoS function has been disabled. Queues would not take effects.</b><br><br>\n");
   }
      
   fprintf(fs, "<table border='1' cellpadding='6' cellspacing='0'>\n");
   // write table header
   fprintf(fs, "   <tr align='center'>\n");
   fprintf(fs, "      <td class='hd'>Name</td>\n");
   fprintf(fs, "      <td class='hd'>Key</td>\n");
   fprintf(fs, "      <td class='hd'>Interface</td>\n");
   fprintf(fs, "      <td class='hd'>Qid</td>\n");
   fprintf(fs, "      <td class='hd'>Prec/Alg/Wght</td>\n");
   fprintf(fs, "      <td class='hd'>Enable</td>\n");
   fprintf(fs, "   </tr>\n");

   /*
    * Traverse the MDM to dump out each QoS queue.  This code is data model
    * dependent, so has been moved to a separate function.
    */
   cgiQosWlQueueViewBody(fs);

   fprintf(fs,"</table><br>\n");
   fprintf(fs, "</form>\n");
   fprintf(fs, "</blockquote>\n");
   fprintf(fs, "</body>\n");
   fprintf(fs, "</html>\n");
   
   fflush(fs);

}  /* End of cgiQosWlQueueView() */
#endif


#ifdef DMP_QOS_1

void cgiQosQueueViewBody_igd(FILE *fs)
{
   char intfname[CMS_IFNAME_LENGTH];
   InstanceIdStack iidStack;
   QMgmtQueueObject *qObj = NULL;
   CmsRet ret;

   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_Q_MGMT_QUEUE, &iidStack, (void **)&qObj)) == CMSRET_SUCCESS)
   {
      if (strstr(qObj->queueInterface, "WLANConfiguration") != NULL)
      {
         /* skip Wifi queues */
         cmsObj_free((void **)&qObj);
         continue;
      }

      if ((ret = qdmIntf_fullPathToIntfnameLocked(qObj->queueInterface, intfname)) != CMSRET_SUCCESS)
      {
         cmsLog_error("qdmIntf_fullPathToIntfnameLocked returns error. ret=%d", ret);
         cmsObj_free((void **)&qObj);
         continue;
      }
      fprintf(fs, "    <tr align='center'>\n");

      /* queue name */
      fprintf(fs, "      <td>%s</td>\n", qObj->X_BROADCOM_COM_QueueName);
      
      /* queue key */
      fprintf(fs, "      <td>%d</td>\n", PEEK_INSTANCE_ID(&iidStack));
      
      /* queue interface */
      fprintf(fs, "      <td>%s</td>\n", intfname);

      /* queue id */
      fprintf(fs, "      <td>%d</td>\n", qObj->X_BROADCOM_COM_QueueId);

      /* queue precedence/algorithm/weight */
      if (strcmp(qObj->schedulerAlgorithm, MDMVS_SP) == 0)
      {
         fprintf(fs, "      <td>%d/%s</td>\n",
                 qObj->queuePrecedence, qObj->schedulerAlgorithm);
      }
      else
      {
         fprintf(fs, "      <td>%d/%s/%d</td>\n",
                 qObj->queuePrecedence, qObj->schedulerAlgorithm, qObj->queueWeight);
      }

#if defined(DMP_ADSLWAN_1)
      /* queue latency path */
      if (qObj->X_BROADCOM_COM_DslLatency >= 0)
      {
         fprintf(fs, "      <td>%s</td>\n", qObj->X_BROADCOM_COM_DslLatency? "Path1" : "Path0");
      }
      else
      {
         fprintf(fs, "      <td>&nbsp;</td>\n");
      }
#endif
             
#if defined(SUPPORT_PTM)
      /* queue PTM priority */
      if (qObj->X_BROADCOM_COM_PtmPriority >= 0)
      {
         fprintf(fs, "      <td>%s</td>\n", qObj->X_BROADCOM_COM_PtmPriority? "High" : "Low");
      }
      else
      {
         fprintf(fs, "      <td>&nbsp;</td>\n");
      }
#endif

#if defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_63158) || defined(CHIP_4908) || defined(CHIP_63268) 
      /* drop algorithm */
      if (cmsUtl_strcmp(qObj->dropAlgorithm, MDMVS_DT) == 0)
      {
         fprintf(fs, "      <td>%s</td>\n", MDMVS_DT);
      }
      else if (cmsUtl_strcmp(qObj->dropAlgorithm, MDMVS_RED) == 0)
      {
         fprintf(fs, "      <td>%s/%d/%d</td>\n", MDMVS_RED,
                 qObj->REDThreshold, qObj->X_BROADCOM_COM_LowClassMaxThreshold);
      }
      else if (cmsUtl_strcmp(qObj->dropAlgorithm, MDMVS_WRED) == 0)
      {
         fprintf(fs, "      <td>%s/%d/%d/%d/%d</td>\n", MDMVS_WRED,
                 qObj->REDThreshold, qObj->X_BROADCOM_COM_LowClassMaxThreshold,
                 qObj->X_BROADCOM_COM_HighClassMinThreshold, qObj->X_BROADCOM_COM_HighClassMaxThreshold);
      }
      else
      {
         fprintf(fs, "      <td>&nbsp;</td>\n");
      }
#endif

#if defined(BCM_PON) || defined(CHIP_63268) || defined(CHIP_63381) || defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_63158) || defined(CHIP_4908) || (defined(DMP_ADSLWAN_1) && defined(SUPPORT_FAPCTL)) || defined(SUPPORT_RDPA) || defined(SUPPORT_BCMTM)
      if (qObj->shapingRate != QOS_QUEUE_NO_SHAPING)
      {
         fprintf(fs, "      <td>%d</td>\n", qObj->shapingRate);
      }
      else
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }
#endif
#if defined(BCM_PON) || defined(CHIP_63268) || defined(CHIP_63381) || defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_63158) || defined(CHIP_4908) || (defined(DMP_ADSLWAN_1) && defined(SUPPORT_FAPCTL)) || defined(SUPPORT_BCMTM)
      if (qObj->X_BROADCOM_COM_MinBitRate != QOS_QUEUE_NO_SHAPING)
      {
         fprintf(fs, "      <td>%d</td>\n", qObj->X_BROADCOM_COM_MinBitRate);
      }
      else
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }
      if ((qObj->shapingRate               != QOS_QUEUE_NO_SHAPING ||
           qObj->X_BROADCOM_COM_MinBitRate != QOS_QUEUE_NO_SHAPING) &&
          qObj->shapingBurstSize > 0)
      {
         fprintf(fs, "      <td>%d</td>\n", qObj->shapingBurstSize);
      }
      else
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }
#endif             

      /* queue enable */
      if (qObj->X_BROADCOM_COM_DslLatency >= 0 && qObj->X_BROADCOM_COM_QueueId == 1)
      {
         /* this is the default queue */
         fprintf(fs, "      <td align='center'><input type='checkbox' name='enbl' %s></td>\n", qObj->queueEnable? "checked" : "");
         fprintf(fs, "      <td align='center'>&nbsp</td>\n");
      }
      else
      {
         fprintf(fs, "      <td align='center'><input type='checkbox' name='enbl' %s></td>\n", qObj->queueEnable? "checked" : "");
         fprintf(fs, "      <td align='center'><input type='checkbox' name='rml'></td>\n");
      }

      fprintf(fs, "  </tr>\n");
      fflush(fs);
      cmsObj_free((void **)&qObj);
      glbEntryCt++;
   }
}


#ifdef BRCM_WLAN
void cgiQosWlQueueViewBody_igd(FILE *fs)
{
   char intfname[CMS_IFNAME_LENGTH];
   InstanceIdStack iidStack;
   QMgmtQueueObject *qObj = NULL;
   CmsRet ret;

   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_Q_MGMT_QUEUE, &iidStack, (void **)&qObj)) == CMSRET_SUCCESS)
   {
      if ((strstr(qObj->queueInterface, "WLANConfiguration") == NULL) || !qObj->queueEnable)
      {
         /* skip non-Wifi queues or disabled Wifi queues. */
         cmsObj_free((void **)&qObj);
         continue;
      }

      if ((ret = qdmIntf_fullPathToIntfnameLocked(qObj->queueInterface, intfname)) != CMSRET_SUCCESS)
      {
         cmsLog_error("qdmIntf_fullPathToIntfnameLocked returns error. ret=%d", ret);
         cmsObj_free((void **)&qObj);
         continue;
      }
      fprintf(fs, "    <tr align='center'>\n");

      /* queue name */
      fprintf(fs, "      <td>%s</td>\n", qObj->X_BROADCOM_COM_QueueName);
      
      /* queue key */
      fprintf(fs, "      <td>%d</td>\n", PEEK_INSTANCE_ID(&iidStack));
      
      /* queue interface */
      fprintf(fs, "      <td>%s</td>\n", intfname);

      /* queue id */
      fprintf(fs, "      <td>%d</td>\n", qObj->X_BROADCOM_COM_QueueId);

      /* queue precedence/algorithm/weight */
      if (strcmp(qObj->schedulerAlgorithm, MDMVS_SP) == 0)
      {
         fprintf(fs, "      <td>%d/%s</td>\n",
                 qObj->queuePrecedence, qObj->schedulerAlgorithm);
      }
      else
      {
         fprintf(fs, "      <td>%d/%s/%d</td>\n",
                 qObj->queuePrecedence, qObj->schedulerAlgorithm, qObj->queueWeight);
      }

      /* queue enable */
      fprintf(fs, "      <td align='center'>%s</td>\n", qObj->queueEnable? "Enabled" : "Disabled");

      fprintf(fs, "  </tr>\n");
      fflush(fs);
      cmsObj_free((void **)&qObj);
   }
}
#endif

#endif  /* DMP_QOS_1 */


void cgiQosQueueAdd( char *query, FILE *fs )
{
   char cmd[WEB_MD_BUF_SIZE_MAX];
   UBOOL8 isDuplicate;
   CmsRet ret;

   /* default values come from data model */
   UBOOL8 enable=FALSE;
   UINT32 precedence=1;
   UINT32 weight=1;  // 1 comes from the original code, even though data model says 0
   SINT32 dslLatency=-1;
   SINT32 ptmPriority=-1;
   SINT32 minBitRate=-1;
   SINT32 shapingRate=-1;
   UINT32 burstSize=0;
   UINT32 queueId=0;
   UINT32 loMinThreshold=0;
   UINT32 loMaxThreshold=0;
   UINT32 hiMinThreshold=0;
   UINT32 hiMaxThreshold=0;
   char intfNameBuf[CMS_IFNAME_LENGTH]={0};
   char queueNameBuf[BUFLEN_64]={0};
   char algorithmBuf[BUFLEN_8]=MDMVS_SP;
   char dropAlgBuf[BUFLEN_8]=MDMVS_DT;


   cmd[0] = '\0';

   /* queue precedence */    
   if (cgiGetValueByName(query, "precedence", cmd) == CGI_STS_OK)
   {
      precedence = atoi(cmd);
   }
   else
   {
      cmsLog_error("Could not get precedence value from query %s", query);
   }

   /* scheduler algorithm */    
   if (cgiGetValueByName(query, "alg", algorithmBuf) != CGI_STS_OK)
   {
      cmsLog_error("Could not get alg value from query %s", query);
   }

   /* queue weight */    
   if (cgiGetValueByName(query, "wght", cmd) == CGI_STS_OK)
   {
      weight = atoi(cmd);
   }
   else
   {
      cmsLog_error("Could not get weight value from query %s", query);
   }

   /* drop algorithm */    
#if defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_63158) || defined(CHIP_4908) || defined(CHIP_63268) 
   if (cgiGetValueByName(query, "dropAlg", dropAlgBuf) == CGI_STS_OK)
   {
      if (cmsUtl_strcmp(dropAlgBuf, MDMVS_RED) == 0)
      {
         if (cgiGetValueByName(query, "loMinThr", cmd) == CGI_STS_OK)
         {
            hiMinThreshold = loMinThreshold = atoi(cmd);

            if (cgiGetValueByName(query, "loMaxThr", cmd) == CGI_STS_OK)
            {
               hiMaxThreshold = loMaxThreshold = atoi(cmd);
            }
            else
            {
               cmsLog_error("Could not get RED loMaxThr value from query %s", query);
            }
         }
         else
         {
            cmsLog_error("Could not get RED loMinThr value from query %s", query);
         }
      }
      else if (cmsUtl_strcmp(dropAlgBuf, MDMVS_WRED) == 0)
      {
         if (cgiGetValueByName(query, "loMinThr", cmd) == CGI_STS_OK)
         {
            loMinThreshold = atoi(cmd);

            if (cgiGetValueByName(query, "loMaxThr", cmd) == CGI_STS_OK)
            {
               loMaxThreshold = atoi(cmd);
            }
            else
            {
               cmsLog_error("Could not get WRED loMaxThr value from query %s", query);
            }
         }
         else
         {
            cmsLog_error("Could not get WRED loMinThr value from query %s", query);
         }
         
         if (cgiGetValueByName(query, "hiMinThr", cmd) == CGI_STS_OK)
         {
            hiMinThreshold = atoi(cmd);

            if (cgiGetValueByName(query, "hiMaxThr", cmd) == CGI_STS_OK)
            {
               hiMaxThreshold = atoi(cmd);
            }
            else
            {
               cmsLog_error("Could not get WRED hiMaxThr value from query %s", query);
            }
         }
         else
         {
            cmsLog_error("Could not get WRED hiMinThr value from query %s", query);
         }
      }
   }
#endif

   /* latency path */
   if (cgiGetValueByName(query, "latency", cmd) == CGI_STS_OK)
   {
      dslLatency = atoi(cmd);
   }
   else
   {
      cmsLog_error("Could not get latency value from query %s", query);
   }

   /* PTM priority */
   if (cgiGetValueByName(query, "ptmpriority", cmd) == CGI_STS_OK)
   {
      ptmPriority = atoi(cmd);
   }
   else
   {
      cmsLog_error("Could not get ptmpriority value from query %s", query);
   }

   /* traffic shaping (not specified on ATM links) */
   if (cgiGetValueByName(query, "minrate", cmd) == CGI_STS_OK)
   {
      minBitRate = atoi(cmd);
      if (minBitRate > 0)
      {
         minBitRate = minBitRate * 1000;
      }
   }

   if (cgiGetValueByName(query, "shaperate", cmd) == CGI_STS_OK)
   {
      shapingRate = atoi(cmd);
      if (shapingRate > 0)
      {
         shapingRate = shapingRate * 1000;
      }
   }

   if (minBitRate > 0 || shapingRate > 0)
   {
      if (cgiGetValueByName(query, "burstsize", cmd) == CGI_STS_OK)
      {
         burstSize = atoi(cmd);
      }
   }

   /* queueIntf */
   if (cgiGetValueByName(query, "queueintf", intfNameBuf) == CGI_STS_OK)
   {
      UINT32 prio;

      /* If the queue precedence is not the lowest, we want to check 
       * if the number of transmit queues is already max. Since the lowest
       * precedence queue uses the same unclassified queue that is
       * statically associated with the interface, it does not take
       * up another transmit queue.
       */
      if ((strstr(intfNameBuf, ATM_IFC_STR) != NULL || strstr(intfNameBuf, PTM_IFC_STR) != NULL) &&
          precedence < XTM_QOS_LEVELS)
      {
         UINT32   unusedQueues;

         if (ptmPriority >= 0)
         {
            ret = cmsDal_getNumUnusedQueues(PTM, &unusedQueues);
         }
         else
         {
            ret = cmsDal_getNumUnusedQueues(ATM, &unusedQueues);
         }

         if (ret != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsDal_getNumUnusedQueues returns error. ret=%d", ret);
            sprintf(cmd, "Configure qos queue failed. ret=%d", ret);
            cgiWriteMessagePage(fs, "Queue Config Add Error", cmd, "qosqueue.cmd?action=view");
            return;
         }
         if (unusedQueues == 0)
         {
            cmsLog_debug("out of hardware transmit queue.");
            sprintf(cmd, "Configure qos queue failed. ret=%d", ret);
            cgiWriteMessagePage(fs, "Out of hardware transmit queue.", cmd, "qosqueue.cmd?action=view");
            return;
         }
      }

      
      if ((ret = dalQos_convertPrecedenceToPriority(intfNameBuf,
                                                    precedence,
                                                    &prio)) != CMSRET_SUCCESS)
      {
         cmsLog_error("dalQos_convertPrecedenceToPriority returns error. ret=%d", ret);
         sprintf(cmd, "Configure qos queue failed. ret=%d", ret);
         cgiWriteMessagePage(fs, "Queue Config Add Error", cmd, "qosqueue.cmd?action=view");
         return;
      }
      
      if ((ret = dalQos_getAvailableQueueId(intfNameBuf, prio, algorithmBuf,
                                            &queueId)) != CMSRET_SUCCESS)
      {
         cmsLog_error("dalQos_getAvailableQueueId returns error. ret=%d", ret);
         sprintf(cmd, "Configure qos queue failed. ret=%d", ret);
         cgiWriteMessagePage(fs, "Queue Config Add Error", cmd, "qosqueue.cmd?action=view");
         return;
      }
   }
   else
   {
      cmsLog_error("Could not get queueintf value from query %s", query);
   }

   /* queueName */
   if (cgiGetValueByName(query, "queueName", queueNameBuf) != CGI_STS_OK)
   {
      cmsLog_error("Could not get queueName value from query %s", query);
   }

   /* enable */
   if (cgiGetValueByName(query, "enable", cmd) == CGI_STS_OK)
   {
      enable = atoi(cmd);
   }
   else
   {
      cmsLog_error("Could not get enable value from query %s", query);
   }

   /* check to see if the queue already exists */
   if ((ret = dalQos_duplicateQueueCheck(queueId, dslLatency,
                                         ptmPriority, intfNameBuf,
                                         &isDuplicate)) != CMSRET_SUCCESS)
   {
      cmsLog_error("dalQos_duplicateQueueCheck returns error. ret=%d", ret);
      sprintf(cmd, "Duplicate check failed. ret=%d", ret);
      cgiWriteMessagePage(fs, "QoS Queue Config Error", cmd, "qosqueue.cmd?action=viewcfg");
      return;
   }
   if (isDuplicate)
   {
      sprintf(cmd, "Queue already exists.");
      cmsLog_error(cmd);
      cgiWriteMessagePage(fs, "QoS Queue Config Error", cmd, "qosqueue.cmd?action=viewcfg");
      return;
   }


   /* Create the queue object instance */
   ret = dalQos_queueAdd(intfNameBuf, algorithmBuf,
                         enable, queueNameBuf, queueId,
                         weight, precedence,
                         minBitRate, shapingRate, burstSize,
                         dslLatency, ptmPriority, dropAlgBuf,
                         loMinThreshold, loMaxThreshold,
                         hiMinThreshold, hiMaxThreshold);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("dalQos_queueAdd returns error. ret=%d", ret);
      sprintf(cmd, "Add queue failed. ret=%d", ret);
      cgiWriteMessagePage(fs, "QoS Queue Config Error", cmd, "qosqueue.cmd?action=viewcfg");
   }
   else
   {
      glbSaveConfigNeeded = TRUE;
      cgiQosQueueView(fs);
   }
}  /* End of cgiQosQueueAdd() */


#ifdef DMP_QOS_1

void cgiQosQueueRemove_igd(char *query, FILE *fs)
{
   char cmd[WEB_MD_BUF_SIZE_MAX];
   char lst[WEB_BUF_SIZE_MAX];
   char *last = NULL;
   char *check;
   InstanceIdStack iidStack, iidStackPrev;
   QMgmtQueueObject *qObj = NULL;
   CmsRet ret;

   lst[0] = '\0';
   if (cgiGetValueByName(query, "rmLst", lst) != CGI_STS_OK)
   {
      return;
   }

   /* loop through the queue table */
   INIT_INSTANCE_ID_STACK(&iidStack);
   iidStackPrev = iidStack;
   while ((ret = cmsObj_getNext(MDMOID_Q_MGMT_QUEUE, &iidStack, (void **)&qObj)) == CMSRET_SUCCESS)
   {
      if (strstr(qObj->queueInterface, "WLANConfiguration") != NULL ||
          (qObj->X_BROADCOM_COM_DslLatency >= 0 && qObj->X_BROADCOM_COM_QueueId == 1))
      {
         /* wlan and dsl default queues are static. Can not be removed. */
         cmsObj_free((void **)&qObj);
         continue;
      }

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
         cmsObj_free((void **)&qObj);
         break;   /* done */
      }
      if (atoi(check) == 1)
      {
         cmsLog_debug("Deleting instance=%d QID=%d intf=%s",
                      PEEK_INSTANCE_ID(&iidStack),
                      qObj->X_BROADCOM_COM_QueueId,
                      qObj->queueInterface);

         if ((ret = cmsObj_deleteInstance(MDMOID_Q_MGMT_QUEUE, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("deleteInstance returns error, ret = %d", ret);
            sprintf(cmd, "Delete queue failed. ret=%d", ret);
            cgiWriteMessagePage(fs, "QoS Queue Config Error", cmd, "qosqueue.cmd?action=viewcfg");
            cmsObj_free((void **)&qObj);
            break;
         }
#ifdef DMP_X_BROADCOM_COM_RDPA_1
         if ((ret = dalQos_delAccordingQueueStatsobject(qObj)) != CMSRET_SUCCESS)
         {
            cmsLog_error("dalQos_delAccordingQueueStatsobject returns error, ret = %d", ret);
         }
#endif
         glbSaveConfigNeeded = TRUE;

         /* since this instance has been deleted, we want to set the iidStack to
          * the previous instance, so that we can continue to do getNext.
          */
         iidStack = iidStackPrev;
      }
      /* save this iidStack in case we want to do a getNext from this instance */
      iidStackPrev = iidStack;
      
      cmsObj_free((void **)&qObj);
   }
}  /* End of cgiQosQueueRemove() */


void cgiQosQueueEnable_igd(char *query, FILE *fs)
{
   char cmd[WEB_MD_BUF_SIZE_MAX];
   char lst[WEB_BUF_SIZE_MAX];
   char *last = NULL;
   char *check;
   InstanceIdStack iidStack;
   QMgmtQueueObject *qObj = NULL;
   CmsRet ret;

   lst[0] = '\0';
   if (cgiGetValueByName(query, "eblLst", lst) != CGI_STS_OK)
   {
      return;
   }

   /* loop through the queue table */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_Q_MGMT_QUEUE, &iidStack, (void **)&qObj)) == CMSRET_SUCCESS)
   {
      if (strstr(qObj->queueInterface, "WLANConfiguration") != NULL)
      {
         /* wlan are static. Can not be enabled or disabled. */
         cmsObj_free((void **)&qObj);
         continue;
      }

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
         cmsObj_free((void **)&qObj);
         break;   /* done */
      }
   
      if (atoi(check) == 1)
      {
         if (!qObj->queueEnable)
         {
            qObj->queueEnable = TRUE;
            if ((ret = cmsObj_set(qObj, &iidStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("cmsObj_set returns error. ret=%d", ret);
               sprintf(cmd, "Enable queue failed. ret=%d", ret);
               cgiWriteMessagePage(fs, "QoS Queue Config Error", cmd, "qosqueue.cmd?action=viewcfg");
               cmsObj_free((void **)&qObj);
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
         if (qObj->queueEnable)
         {
            qObj->queueEnable = FALSE;
            if ((ret = cmsObj_set(qObj, &iidStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("cmsObj_set returns error. ret=%d", ret);
               sprintf(cmd, "Disable queue failed. ret=%d", ret);
               cgiWriteMessagePage(fs, "QoS Queue Config Error", cmd, "qosqueue.cmd?action=viewcfg");
               cmsObj_free((void **)&qObj);
               break;
            }
            else
            {
               glbSaveConfigNeeded = TRUE;
            }
         }
      }
      cmsObj_free((void **)&qObj);
   }
}  /* End of cgiQosQueueEnable() */


void cgiGetQosQueueInfo_igd(int argc __attribute__((unused)),
                        char **argv,
                        char *varValue)
{
   InstanceIdStack iidStack, iidStack2;
   MdmPathDescriptor pathDesc;
   QMgmtQueueObject *qObj = NULL;
   void *mdmObj = NULL;
   char varValue1[BUFLEN_8];
   char varValue2[BUFLEN_64];
   char intfFullPath[BUFLEN_512];
   char info[BUFLEN_32];
   char ifname[CMS_IFNAME_LENGTH];
   CmsRet ret;

   varValue[0]  = '\0';

   /* loop through the queue table */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_Q_MGMT_QUEUE, &iidStack, (void **)&qObj)) == CMSRET_SUCCESS)
   {
      if (!qObj->queueEnable)
      {
         cmsObj_free((void **)&qObj);
         continue;
      }
         
      /* add a dot at the end to indicate that the path is an object path */
      snprintf(intfFullPath, sizeof(intfFullPath), "%s.", qObj->queueInterface);
      if ((ret = cmsMdm_fullPathToPathDescriptor(intfFullPath, &pathDesc)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsMdm_fullPathToPathDescriptor returns error. ret=%d", ret);
         cmsObj_free((void **)&qObj);
         continue;
      }

      /* convert mdm full path string to queue interface name */
      if ((ret = qdmIntf_fullPathToIntfnameLocked(qObj->queueInterface, ifname)) != CMSRET_SUCCESS)
      {
         cmsLog_error("qdmIntf_fullPathToIntfnameLocked returns error. ret=%d", ret);
         cmsObj_free((void **)&qObj);
         continue;
      }

      sprintf(varValue1, "%d:", PEEK_INSTANCE_ID(&iidStack));

      varValue2[0] = '\0';
      sprintf(info, "&Key%d", PEEK_INSTANCE_ID(&iidStack));
      strcat(varValue2, info);
      if (qObj->X_BROADCOM_COM_DslLatency != -1)
      {
         sprintf(info, "&Path%d", qObj->X_BROADCOM_COM_DslLatency);
         strcat(varValue2, info);
      }
      if (qObj->X_BROADCOM_COM_PtmPriority != -1)
      {
         sprintf(info, "&%s", qObj->X_BROADCOM_COM_PtmPriority? "Hi" : "Lo");
         strcat(varValue2, info);
      }
      sprintf(info, "&Pre%d", qObj->queuePrecedence);
      strcat(varValue2, info);
      if (strcmp(qObj->schedulerAlgorithm, MDMVS_SP))
      {
         sprintf(info, "&Wt%d", qObj->queueWeight);
         strcat(varValue2, info);
      }
      if (dalWan_isInterfaceVlanEnabled(ifname))
      {
         strcat(varValue2, ":1"); /* vlanmux enabled */
      }
      else
      {
         strcat(varValue2, ":0"); /* vlanmux disabled */
      }
      strcat(varValue2, "|");

      if (strstr(qObj->queueInterface, "WANDevice") != NULL)
      {
         INIT_INSTANCE_ID_STACK(&iidStack2);
         while ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_IP_CONN, &pathDesc.iidStack, &iidStack2, &mdmObj)) == CMSRET_SUCCESS)
         {
            sprintf(info, "%s", ((WanIpConnObject *)mdmObj)->X_BROADCOM_COM_IfName);

            if ((strcmp(argv[2], "all") == 0) || (strstr(info, argv[2]) != NULL))
            {
               strcat(info, "(wan)");
               strcat(varValue, varValue1);
               strcat(varValue, info);
               strcat(varValue, varValue2);
            }
            cmsObj_free(&mdmObj);
         }
         if (ret != CMSRET_NO_MORE_INSTANCES && ret != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsObj_getNextInSubTree <MDMOID_WAN_IP_CONN> returns error. ret=%d", ret);
            cmsObj_free((void **)&qObj);
            continue;
         }

         INIT_INSTANCE_ID_STACK(&iidStack2);
         while ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_PPP_CONN, &pathDesc.iidStack, &iidStack2, &mdmObj)) == CMSRET_SUCCESS)
         {
            sprintf(info, "%s", ((WanPppConnObject *)mdmObj)->X_BROADCOM_COM_IfName);

            if ((strcmp(argv[2], "all") == 0) || (strstr(info, argv[2]) != NULL))
            {
               strcat(info, "(wan)");
               strcat(varValue, varValue1);
               strcat(varValue, info);
               strcat(varValue, varValue2);
            }
            cmsObj_free(&mdmObj);
         }
         if (ret != CMSRET_NO_MORE_INSTANCES && ret != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsObj_getNextInSubTree <MDMOID_WAN_PPP_CONN> returns error. ret=%d", ret);
            cmsObj_free((void **)&qObj);
            continue;
         }
      }
      else if ((strcmp(argv[2], "all") == 0) || (strstr(ifname, argv[2]) != NULL))
      {
         strcat(varValue, varValue1);
         strcat(varValue, ifname);
         strcat(varValue, varValue2);
      }

      cmsObj_free((void **)&qObj);
   }
}  /* End of cgiGetQosQueueInfo() */

#endif  /* DMP_QOS_1 */



void cgiGetQosQueueInfo(int argc, char **argv, char *varValue)
{
#if defined(SUPPORT_DM_LEGACY98)

   cgiGetQosQueueInfo_igd(argc, argv, varValue);

#elif defined(SUPPORT_DM_HYBRID)

   cgiGetQosQueueInfo_igd(argc, argv, varValue);

#elif defined(SUPPORT_DM_PURE181)

   cgiGetQosQueueInfo_dev2(argc, argv, varValue);

#elif defined(SUPPORT_DM_DETECT)

   if (cmsMdm_isDataModelDevice2())
   {
      cgiGetQosQueueInfo_dev2(argc, argv, varValue);
   }
   else
   {
      cgiGetQosQueueInfo_dev2(argc, argv, varValue);
   }

#endif
}


void cgiGetQueueIntf(int argc, char **argv, char *varValue)
{
#if defined(SUPPORT_DM_LEGACY98)

   cgiGetQueueIntf_igd(argc, argv, varValue);

#elif defined(SUPPORT_DM_HYBRID)

   cgiGetQueueIntf_igd(argc, argv, varValue);

#elif defined(SUPPORT_DM_PURE181)

   cgiGetQueueIntf_dev2(argc, argv, varValue);

#elif defined(SUPPORT_DM_DETECT)

   if (cmsMdm_isDataModelDevice2())
   {
      cgiGetQueueIntf_dev2(argc, argv, varValue);
   }
   else
   {
      cgiGetQueueIntf_dev2(argc, argv, varValue);
   }

#endif
}


#ifdef DMP_QOS_1
// Fills in all the interfaces that supports QoS on the LAN and the WAN side.
void cgiGetQueueIntf_igd(int argc __attribute__((unused)),
                     char **argv __attribute__((unused)),
                     char *varValue)
{
   InstanceIdStack iidStack;
   void *mdmObj  = NULL;
#ifdef DMP_ADSLWAN_1
   void *linkCfg = NULL;
#endif
   char desc[BUFLEN_32];
   CmsRet ret;

   varValue[0] = '\0';
   desc[0]     = '\0';

   /* get LAN Ethernet interfaces */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_LAN_ETH_INTF, &iidStack, &mdmObj)) == CMSRET_SUCCESS)
   {
      if (((LanEthIntfObject *)mdmObj)->enable)
      {
         strcat(varValue, ((LanEthIntfObject *)mdmObj)->X_BROADCOM_COM_IfName);
         strcat(varValue, "&");
         cgiGetIntfQueuePrec(((LanEthIntfObject *)mdmObj)->X_BROADCOM_COM_IfName, FALSE, desc);
         strcat(varValue, desc);
         strcat(varValue, "|");
      }
      cmsObj_free(&mdmObj);
   }
   if (ret != CMSRET_NO_MORE_INSTANCES && ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_getNext <MDMOID_LAN_ETH_INTF> returns error. ret=%d", ret);
      return;
   }

#ifdef DMP_ETHERNETWAN_1
   /* get WAN Ethernet interfaces */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_WAN_ETH_INTF, &iidStack, &mdmObj)) == CMSRET_SUCCESS)
   {
      if (((WanEthIntfObject *)mdmObj)->enable)
      {
         strcat(varValue, ((WanEthIntfObject *)mdmObj)->X_BROADCOM_COM_IfName);
         strcat(varValue, "(wan)");
         strcat(varValue, "&");
         cgiGetIntfQueuePrec(((WanEthIntfObject *)mdmObj)->X_BROADCOM_COM_IfName, TRUE, desc);
         strcat(varValue, desc);
         strcat(varValue, "|");
      }
      cmsObj_free(&mdmObj);
   }
   if (ret != CMSRET_NO_MORE_INSTANCES && ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_getNext <MDMOID_WAN_ETH_INTF> returns error. ret=%d", ret);
      return;
   }
#endif

#ifdef DMP_X_BROADCOM_COM_CELLULARWAN_1 
{
   InstanceIdStack cellularLinkIid = EMPTY_INSTANCE_ID_STACK;
   
   INIT_INSTANCE_ID_STACK(&iidStack);
   PUSH_INSTANCE_ID(&cellularLinkIid, CMS_WANDEVICE_CELLULAR);

   while ((ret = cmsObj_getNextInSubTree(MDMOID_WAN_IP_CONN, &cellularLinkIid, &iidStack, &mdmObj)) == CMSRET_SUCCESS)
   {
      strcat(varValue, ((WanIpConnObject *)mdmObj)->X_BROADCOM_COM_IfName);
      strcat(varValue, "(wan)");
      strcat(varValue, "&");
      cgiGetIntfQueuePrec(((WanIpConnObject *)mdmObj)->X_BROADCOM_COM_IfName, TRUE, desc);
      strcat(varValue, desc);
      strcat(varValue, "|");
      
      cmsObj_free(&mdmObj);
   }
		 
   if (ret != CMSRET_NO_MORE_INSTANCES && ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_getNextInSubTree <MDMOID_WAN_IP_CONN> returns error. ret=%d", ret);
      return;
   }
}
#endif

#ifdef DMP_X_BROADCOM_COM_GPONWAN_1
   /* get enabled gpon wan interface */
   INIT_INSTANCE_ID_STACK(&iidStack);
   /* Get Gpon WanDevice iidStack first */      
   if (dalGpon_getGponWanIidStatck(&iidStack) == CMSRET_SUCCESS)
   {
      InstanceIdStack gponLinkIid = EMPTY_INSTANCE_ID_STACK;   
      WanGponLinkCfgObject *gponLinkCfg = NULL;
      
      /* go over all the wan gpon link config objects to find the enabled gpon interface and add it to the list  */
      while  ((ret = cmsObj_getNextInSubTreeFlags
         (MDMOID_WAN_GPON_LINK_CFG, &iidStack, &gponLinkIid, OGF_NO_VALUE_UPDATE, (void **)&gponLinkCfg)) == CMSRET_SUCCESS)
      {
         if (gponLinkCfg->enable)
         {
            strcat(varValue, gponLinkCfg->ifName);
            strcat(varValue, "(wan)");
            strcat(varValue, "&");
            cgiGetIntfQueuePrec(gponLinkCfg->ifName, TRUE, desc);
            strcat(varValue, desc);
            strcat(varValue, "|");
         }
         cmsObj_free((void **)&gponLinkCfg);
      }
   }
#endif /* DMP_X_BROADCOM_COM_GPONWAN_1 */

#ifdef DMP_X_BROADCOM_COM_EPONWAN_1
   /* get enabled epon wan interface */
   INIT_INSTANCE_ID_STACK(&iidStack);
   /* Get Epon WanDevice iidStack first */      
   if (dalEpon_getEponWanIidStatck(&iidStack) == CMSRET_SUCCESS)
   {
      InstanceIdStack eponLinkIid = EMPTY_INSTANCE_ID_STACK;   
      WanEponLinkCfgObject *eponLinkCfg = NULL;

      /* go over all the wan epon link config objects to find the enabled epon interface and add it to the list  */
      while  ((ret = cmsObj_getNextInSubTreeFlags
         (MDMOID_WAN_EPON_LINK_CFG, &iidStack, &eponLinkIid, OGF_NO_VALUE_UPDATE, (void **)&eponLinkCfg)) == CMSRET_SUCCESS)
      {
         if (eponLinkCfg->enable)
         {
            strcat(varValue, eponLinkCfg->ifName);
            strcat(varValue, "(wan)");
            strcat(varValue, "&");
            cgiGetIntfQueuePrec(eponLinkCfg->ifName, TRUE, desc);
            strcat(varValue, desc);			
            strcat(varValue, "|");
            cmsObj_free((void **)&eponLinkCfg);
            break;			
         }
         cmsObj_free((void **)&eponLinkCfg);
      }
   }
#endif /* DMP_X_BROADCOM_COM_EPONWAN_1 */

#ifdef SUPPORT_MOCA
#ifdef DMP_X_BROADCOM_COM_MOCALAN_1
   /* get LAN Ethernet interfaces */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_LAN_MOCA_INTF, &iidStack, &mdmObj)) == CMSRET_SUCCESS)
   {
      if (((LanMocaIntfObject *)mdmObj)->enable)
      {
         strcat(varValue, ((LanMocaIntfObject *)mdmObj)->ifName);
         strcat(varValue, "&");
         cgiGetIntfQueuePrec(((LanMocaIntfObject *)mdmObj)->ifName, FALSE, desc);
         strcat(varValue, desc);
         strcat(varValue, "|");
      }
      cmsObj_free(&mdmObj);
   }
   if (ret != CMSRET_NO_MORE_INSTANCES && ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_getNext <MDMOID_LAN_MOCA_INTF> returns error. ret=%d", ret);
      return;
   }
#endif
#ifdef DMP_X_BROADCOM_COM_MOCAWAN_1
   /* get WAN Moca interfaces */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_WAN_MOCA_INTF, &iidStack, &mdmObj)) == CMSRET_SUCCESS)
   {
      if (((WanMocaIntfObject *)mdmObj)->enable)
      {
         strcat(varValue, ((WanMocaIntfObject *)mdmObj)->ifName);
         strcat(varValue, "(wan)");
         strcat(varValue, "&");
         cgiGetIntfQueuePrec(((WanMocaIntfObject *)mdmObj)->ifName, TRUE, desc);
         strcat(varValue, desc);
         strcat(varValue, "|");
      }
      cmsObj_free(&mdmObj);
   }
   if (ret != CMSRET_NO_MORE_INSTANCES && ret != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_getNext <MDMOID_WAN_MOCA_INTF> returns error. ret=%d", ret);
      return;
   }
#endif
#endif

#ifdef DMP_ADSLWAN_1
   /* get WAN ATM and PTM interfaces */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_WAN_DSL_LINK_CFG, &iidStack, (void **)&linkCfg)) == CMSRET_SUCCESS)
   {
      if (!((WanDslLinkCfgObject *)linkCfg)->enable)
      {
         cmsObj_free(&linkCfg);
#ifdef DMP_PTMWAN_1
         if ((ret = cmsObj_get(MDMOID_WAN_PTM_LINK_CFG, &iidStack, 0, &linkCfg)) != CMSRET_SUCCESS)
         {
            break;
         }
         if (!(((WanPtmLinkCfgObject *)linkCfg)->enable &&
               ((WanPtmLinkCfgObject *)linkCfg)->X_BROADCOM_COM_PTMEnbQos))
         {
            cmsObj_free(&linkCfg);
            continue;
         }
         else
         {
            sprintf(desc, "%s[%d_%d_%d]",
                           ((WanPtmLinkCfgObject *)linkCfg)->X_BROADCOM_COM_IfName,
                           ((WanPtmLinkCfgObject *)linkCfg)->X_BROADCOM_COM_PTMPortId,
                           ((WanPtmLinkCfgObject *)linkCfg)->X_BROADCOM_COM_PTMPriorityHigh,
                           ((WanPtmLinkCfgObject *)linkCfg)->X_BROADCOM_COM_PTMPriorityLow);
            strcat(varValue, desc);
            strcat(varValue, "&");
            cgiGetIntfQueuePrec(((WanPtmLinkCfgObject *)linkCfg)->X_BROADCOM_COM_IfName, TRUE, desc);
            strcat(varValue, desc);
            strcat(varValue, "|");
            cmsObj_free(&linkCfg);
         }
#else
         continue;
#endif
      }
      else if (!((WanDslLinkCfgObject *)linkCfg)->X_BROADCOM_COM_ATMEnbQos)
      {
         cmsObj_free(&linkCfg);
         continue;
      }
      else
      {
         SINT32 vpi, vci;

         cmsUtl_atmVpiVciStrToNum(((WanDslLinkCfgObject *)linkCfg)->destinationAddress, &vpi, &vci);
         
         sprintf(desc, "%s[%d_%d_%d]",
                        ((WanDslLinkCfgObject *)linkCfg)->X_BROADCOM_COM_IfName,
                        ((WanDslLinkCfgObject *)linkCfg)->X_BROADCOM_COM_ATMInterfaceId,
                        vpi,
                        vci);
         strcat(varValue, desc);
         strcat(varValue, "&");
         cgiGetIntfQueuePrec(((WanDslLinkCfgObject *)linkCfg)->X_BROADCOM_COM_IfName, TRUE, desc);
         strcat(varValue, desc);
         strcat(varValue, "|");
         cmsObj_free(&linkCfg);
      }
   }
#endif /* DMP_ADSLWAN_1 */
   if (strlen(varValue) > 0)
   {
      /* get rid of the last '|' */
      varValue[strlen(varValue)-1] = '\0';
   }
   cmsLog_debug("varValue=%s", varValue);

}  /* End of cgiGetQueueIntf() */


CmsRet cgiGetQosQueueSchedulerInfo_igd(const char *l2Ifname,
                                       char *precArray, SINT32 *numSP,
                                       SINT32 *numWFQ, SINT32 *numWRR)
{
   InstanceIdStack iidStack;
   QMgmtQueueObject *qObj = NULL;
   char ifname[CMS_IFNAME_LENGTH]={0};
   CmsRet ret;

   *numSP = *numWFQ = *numWRR = 0;
   
   /* loop through the queue table */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_Q_MGMT_QUEUE, &iidStack, (void **)&qObj)) == CMSRET_SUCCESS)
   {
      /* convert mdm full path string to queue interface name */
      if ((ret = qdmIntf_fullPathToIntfnameLocked(qObj->queueInterface, ifname)) != CMSRET_SUCCESS)
      {
         cmsLog_error("qdmIntf_fullPathToIntfnameLocked returns error. ret=%d", ret);
         cmsObj_free((void **)&qObj);
         continue;
      }

      if (cmsUtl_strcmp(l2Ifname, ifname) == 0)
      {
         if (cmsUtl_strcmp(qObj->schedulerAlgorithm, MDMVS_WFQ) == 0)
         {
            (*numWFQ)++;
            precArray[qObj->queuePrecedence] = CMS_QUEUE_SCHED_WFQ;
         }
         else if (cmsUtl_strcmp(qObj->schedulerAlgorithm, MDMVS_WRR) == 0)
         {
            (*numWRR)++;
            
            precArray[qObj->queuePrecedence] = CMS_QUEUE_SCHED_WRR;
         }
         else
         {
            (*numSP)++;
            precArray[qObj->queuePrecedence] = CMS_QUEUE_SCHED_SP;
         }
      }
      cmsObj_free((void **)&qObj);
   }

   return ret;
}

#endif  /* DMP_QOS_1 */


/** Returns queue precedence list of the specified interface
 *  This function is used by both TR98 and TR181 code (the middle part
 *  of the function, which traverses through the MDM queue objects, is
 *  extracted to separate _igd and _dev2 functions.)
 */
void cgiGetIntfQueuePrec(const char *l2Ifname, UBOOL8 isWan, char *varValue)
{
   char precArray[MAX_QOS_LEVELS + 1] = {CMS_QUEUE_SCHED_UNSPEC};
   UBOOL8 xtmQueue = FALSE;
   UBOOL8 fapTmEth = FALSE;
   UBOOL8 rddTmEth = FALSE;
   UINT32 maxLevel;
   SINT32 numSP  = 0;
   SINT32 numWRR = 0;
   SINT32 numWFQ = 0;
   CmsRet ret = CMSRET_SUCCESS;

   varValue[0] = '\0';

   // assuage the 4.6.2 compiler
   if (numWRR) {}

   if (IS_EMPTY_STRING(l2Ifname))
   {
      return;
   }

   dalQos_getIntfNumQueuesAndLevels(l2Ifname, NULL, &maxLevel);

   if (strstr(l2Ifname, ATM_IFC_STR) ||
       strstr(l2Ifname, PTM_IFC_STR) ||
       strstr(l2Ifname, IPOA_IFC_STR))
   {
      xtmQueue = TRUE;
   }

#if defined(DMP_ADSLWAN_1) && defined(SUPPORT_FAPCTL) || defined(SUPPORT_BCMTM)
   if (strstr(l2Ifname, ETH_IFC_STR))
   {
     fapTmEth = TRUE;
   }
#endif

#if defined(SUPPORT_RDPA)
   if (strstr(l2Ifname, ETH_IFC_STR) ||
       strstr(l2Ifname, GPON_IFC_STR) ||
       strstr(l2Ifname, EPON_IFC_STR))
   {
      rddTmEth = TRUE;
   }
#endif

   /* Get info about which scheduling algs are used in the various queues */
   ret = cgiGetQosQueueSchedulerInfo(l2Ifname, precArray, &numSP, &numWFQ, &numWRR);

   if (ret == CMSRET_NO_MORE_INSTANCES)
   {
#if !defined(CHIP_63138) && !defined(CHIP_63148) && !defined(CHIP_63158) && !defined(CHIP_4908) && !defined (BCM_PON)
      char  onlyMode = CMS_QUEUE_SCHED_SP_WRR;
#endif
      char  prec[BUFLEN_8];
      UINT32 i;

      cmsLog_debug("numSP=%d numWRR=%d numWFQ=%d", numSP, numWRR, numWFQ);

      if (rddTmEth) 
      {
#if !defined(CHIP_63138) && !defined(CHIP_63148) && !defined(CHIP_63158) && !defined(CHIP_4908) && !defined (BCM_PON)
         for (i = 1; i <= maxLevel; i++)
         {
            if (precArray[i] == CMS_QUEUE_SCHED_SP)
               onlyMode = CMS_QUEUE_SCHED_SP;

            if (precArray[i] == CMS_QUEUE_SCHED_WRR)
            	onlyMode = CMS_QUEUE_SCHED_WRR;
         }
#endif
      }

      for (i = 1; i <= maxLevel; i++)
      {
         if (precArray[i] == CMS_QUEUE_SCHED_SP)
         {
            /* Can not have multiple SP queues at the same precedence level.
             * So, make this precedence level unavailable for selection.
             */
            sprintf(prec, "%c", CMS_QUEUE_SCHED_BLOCK);
         }
         else if (precArray[i] == CMS_QUEUE_SCHED_RR)
         {
            sprintf(prec, "%c", CMS_QUEUE_SCHED_RR);
         }
         else if (precArray[i] == CMS_QUEUE_SCHED_WRR)
         {
            if (rddTmEth)
	        {
#if defined(BCM_PON) || defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_63158) || defined(CHIP_4908)
               if (i < maxLevel || numWRR == (SINT32) maxLevel)
               {
                  sprintf(prec, "%c", CMS_QUEUE_SCHED_BLOCK);
               }
               else if (precArray[maxLevel - numWRR] == CMS_QUEUE_SCHED_SP)
               {
                  /* For rddTmEth, the number of WRR queues is bound by the lowest
                   * priority SP queue number. Can not add anymore WRR queue.
                   * Therefore, mark it as BLOCK.
                   */
                  sprintf(prec, "%c", CMS_QUEUE_SCHED_BLOCK);
               }
               else
               {
                  sprintf(prec, "%c", CMS_QUEUE_SCHED_WRR);
               }
#else
               sprintf(prec, "%c", CMS_QUEUE_SCHED_BLOCK);
#endif           
            }
            else if (fapTmEth && precArray[maxLevel - numWRR] == CMS_QUEUE_SCHED_SP)
            {
               /* For fapTmEth, the number of WRR queues is bound by the lowest
                * priority SP queue number. Can not add anymore WRR queue.
                * Therefore, mark it as BLOCK.
                * Note that for fapTmEth interface, the SP queue number is
                * indicated by its priority level. i.e., SPQ1 has priority 1,
                * SPQ2 has priority 2.
                */
               sprintf(prec, "%c", CMS_QUEUE_SCHED_BLOCK);
            }
            else
            {
               sprintf(prec, "%c", CMS_QUEUE_SCHED_WRR);
            }
         }
         else if (precArray[i] == CMS_QUEUE_SCHED_WFQ)
         {
            sprintf(prec, "%c", CMS_QUEUE_SCHED_WFQ);
         }
         else
         {
            /* queue scheduler has not been selected */
            if (fapTmEth)
            {
               if (i == maxLevel)
               {
                  if (numSP)
                  {
                     sprintf(prec, "%c", CMS_QUEUE_SCHED_SP_WRR);
                  }
                  else
                  {
                     sprintf(prec, "%c", CMS_QUEUE_SCHED_SP_WRR_WFQ);
                  }
               }
               else
               {
                  if (numWFQ)
                  {
                     sprintf(prec, "%c", CMS_QUEUE_SCHED_BLOCK);
                  }
                  else if (i >= (maxLevel - numWRR + 1))
                  {
                     sprintf(prec, "%c", CMS_QUEUE_SCHED_BLOCK);
                  }
                  else
                  {
                     sprintf(prec, "%c", CMS_QUEUE_SCHED_SP);
                  }
               }
            }
            else if (rddTmEth)
            {
#if defined(BCM_PON) || defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_63158) || defined(CHIP_4908)
               if (isWan)
               {
                  if (numSP == ((SINT32) maxLevel) - 1)
                  {
                     /* the last choice has to be SP */
                     sprintf(prec, "%c", CMS_QUEUE_SCHED_SP);
                  }
                  else if (i == maxLevel)
                  {
                     if (numWRR == ((SINT32) maxLevel) - 1)
                        sprintf(prec, "%c", CMS_QUEUE_SCHED_WRR);
                     else
                        sprintf(prec, "%c", CMS_QUEUE_SCHED_SP_WRR);
                  }
                  else
                  {
                     if (i >= (maxLevel - numWRR + 1))
                     {
                        sprintf(prec, "%c", CMS_QUEUE_SCHED_BLOCK);
                     }
                     else
                     {
                        sprintf(prec, "%c", CMS_QUEUE_SCHED_SP);
                     }
                  }
               }
               else
               {
#if defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_63158) || defined(CHIP_4908)
                  if (numSP > 4 ||
                      precArray[5] == CMS_QUEUE_SCHED_SP ||
                      precArray[6] == CMS_QUEUE_SCHED_SP ||
                      precArray[7] == CMS_QUEUE_SCHED_SP ||
                      precArray[8] == CMS_QUEUE_SCHED_SP)
                  {
                     /* all SP */
                     sprintf(prec, "%c", CMS_QUEUE_SCHED_SP);
                  }
                  else if (i == maxLevel)
                  {
                     if (numWRR == ((SINT32) maxLevel) - 1)
                        sprintf(prec, "%c", CMS_QUEUE_SCHED_WRR);
                     else
                        sprintf(prec, "%c", CMS_QUEUE_SCHED_SP_WRR);
                  }
                  else
                  {
                     if (i >= (maxLevel - numWRR + 1))
                     {
                        sprintf(prec, "%c", CMS_QUEUE_SCHED_BLOCK);
                     }
                     else
                     {
                        sprintf(prec, "%c", CMS_QUEUE_SCHED_SP);
                     }
                  }
                  
#else               
                  /* 6838/6848/6858 LAN supports SP only */
                  sprintf(prec, "%c", CMS_QUEUE_SCHED_SP);
#endif                  
               }
#else               
               if (isWan)
               {
                  if (onlyMode == CMS_QUEUE_SCHED_SP)
                  {
                     sprintf(prec, "%c", CMS_QUEUE_SCHED_SP);
                  }
                  else if (onlyMode == CMS_QUEUE_SCHED_WRR)
                  {
                     sprintf(prec, "%c", CMS_QUEUE_SCHED_WRR);
                  }
                  else
                  {
                     sprintf(prec, "%c", CMS_QUEUE_SCHED_SP_WRR);
                  }
               }
               else
               {
                  sprintf(prec, "%c", CMS_QUEUE_SCHED_SP);
               }
#endif               
            }
            else if (xtmQueue)
            {
               /* For 63268/63138/63381/63148/63158/6828, each MPAAL group can have multiple queues at
                * different precedence levels. Queues of equal precedence
                * can be scheduled as either WRR or WFQ.
                */
               sprintf(prec, "%c", CMS_QUEUE_SCHED_WRR_WFQ);
            }
            else
            {
               /* Currently only SP is supported for non-xtm queue scheduling, */
               sprintf(prec, "%c", CMS_QUEUE_SCHED_SP);
            }
         }
         strcat(varValue, prec);
      }
   }
}  /* End of cgiGetIntfQueuePrec() */


/* Returns precedence levels */
void cgiGetPrecedence(int argc __attribute__((unused)), char **argv, char *varValue)
{
   char         desc[BUFLEN_4];
   int          i;

   varValue[0] = '\0';
   if (cmsUtl_strcmp(argv[2], "eth") == 0)
   {
      for (i = 1; i <= ETH_QOS_LEVELS; i++)
      {
         if (i > 1 )
             strcat(varValue, "|");
         sprintf(desc, "%d", i);
         strcat(varValue, desc);
      }
   }
}  /* End of cgiGetPrecedence() */

#endif  /* SUPPORT_QOS */


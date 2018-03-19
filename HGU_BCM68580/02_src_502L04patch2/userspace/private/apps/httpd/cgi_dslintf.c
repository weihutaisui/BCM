/***********************************************************************
 *
 *  Copyright (c) 2008  Broadcom Corporation
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

#ifdef SUPPORT_DSL

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
#include "cms_qos.h"
#include "cms_boardcmds.h"
#include "cgi_util.h"
#include "devctl_xtm.h"

/*
 * ATM configuration
 */
void writeDslXtmCfgScript(FILE *fs, char *addLoc, char *removeLoc) 
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

   fprintf(fs, "   var loc = '%s.cmd?action=remove&rmLst=' + lst;\n\n", removeLoc); /* for atm, dslatm, ptm, dslptm */
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

void writeXtmFooter(FILE *fs, SINT32 layer2Count)
{
   UINT32 unusedQueues;
   
   fprintf(fs, "</table><br>\n");

   cmsDal_getNumUnusedQueues(ATM, &unusedQueues);
   cmsLog_debug("unusedQueues = %d", unusedQueues);
   
   /* disable 'Add' button if layer 2 inteface is greater thatn IFC_WAN_MAX
   * there is no queuse left for dsl configuration.
   */
   if (layer2Count >= IFC_WAN_MAX || unusedQueues < 1)
   {
      fprintf(fs, "<input type='button' onClick='addClick()' value='Add' disabled='1'>\n");
   }
   else
   {
      fprintf(fs, "<input type='button' onClick='addClick()' value='Add'>\n");
   }
   fprintf(fs, "<input type='button' onClick='removeClick(this.form.rml)' value='Remove'>\n");
   fprintf(fs, "</center>\n</form>\n</blockquote>\n</body>\n</html>\n");
   fflush(fs);
}

#ifdef DMP_ADSLWAN_1
void cgiDslAtmCfgView_igd(FILE *fs) 
{
   WanDslLinkCfgObject *dslLinkCfg = NULL;   
   UINT32 portId;
   SINT32 vpi;
   SINT32 vci;
   InstanceIdStack wanDevIid = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack dslLinkIid = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 isAtm;
   SINT32 layer2Count = 0;
   
   /* get the dsl WanDev iidStack */
   isAtm = TRUE;
   if (dalDsl_getDslWanDevIidStack(isAtm, &wanDevIid) == FALSE)
   {
      return;
   }
   
   /* write Java Script */
   writeDslXtmCfgScript(fs, "cfgatm", "dslatm");

   /* write table */
   fprintf(fs, "<b>DSL ATM Interface Configuration</b><br><br>\n");
   fprintf(fs, "Choose Add, or Remove to configure DSL ATM interfaces.<br><br>\n");
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");
   /* write table header */
   fprintf(fs, "   <tr align='center'>\n");
   fprintf(fs, "      <td class='hd'>Interface</td>\n");
   fprintf(fs, "      <td class='hd'>Vpi</td>\n");
   fprintf(fs, "      <td class='hd'>Vci</td>\n");
   fprintf(fs, "      <td class='hd'>DSL Latency</td>\n");
   fprintf(fs, "      <td class='hd'>Category</td>\n");
   fprintf(fs, "      <td class='hd'>Peak Cell Rate(cells/s)</td>\n");
   fprintf(fs, "      <td class='hd'>Sustainable Cell Rate(cells/s)</td>\n");
   fprintf(fs, "      <td class='hd'>Max Burst Size(bytes)</td>\n");
#if defined (CHIP_63268) || defined(CHIP_63138) || defined(CHIP_63381) || defined(CHIP_63148) || defined(CHIP_63158)
   fprintf(fs, "      <td class='hd'>Min Cell Rate(cells/s)</td>\n");
#endif   
   fprintf(fs, "      <td class='hd'>Link Type</td>\n");
   fprintf(fs, "      <td class='hd'>Conn Mode</td>\n");
   fprintf(fs, "      <td class='hd'>IP QoS</td>\n");
#if defined (CHIP_63268) || defined(CHIP_63138) || defined(CHIP_63381) || defined(CHIP_63148) || defined(CHIP_63158)
   fprintf(fs, "      <td class='hd'>MPAAL Prec/Alg/Wght</td>\n");
#endif
   fprintf(fs, "      <td class='hd'>Remove</td>\n");
   fprintf(fs, "   </tr>\n");

   /* Normal ATM PVC */
   while (layer2Count < IFC_WAN_MAX &&
          cmsObj_getNextInSubTree(MDMOID_WAN_DSL_LINK_CFG, &wanDevIid, &dslLinkIid, (void **)&dslLinkCfg) == CMSRET_SUCCESS)
   {
      /* get vpi/vci string */
      cmsUtl_atmVpiVciStrToNum(dslLinkCfg->destinationAddress, &vpi, &vci);
      portId = dslLinkCfg->X_BROADCOM_COM_ATMInterfaceId;

      fprintf(fs, "   <tr align='center'>\n");
      fprintf(fs, "      <td>%s</td>\n", dslLinkCfg->X_BROADCOM_COM_IfName);
      fprintf(fs, "      <td>%d</td>\n", vpi);
      fprintf(fs, "      <td>%d</td>\n", vci);
      fprintf(fs, "      <td>%s</td>\n", (portId == PHY0_PATH0)? "Path0" :
                                         (portId == PHY0_PATH1)? "Path1" :
                                         (portId == PHY0_PATH0_PATH1)? "Path0&1" : "");
      fprintf(fs, "      <td>%s</td>\n", dslLinkCfg->ATMQoS);
      if (cmsUtl_strcmp(dslLinkCfg->ATMQoS, MDMVS_UBR) == 0)
      {
         fprintf(fs, "      <td>&nbsp;</td>\n");
         fprintf(fs, "      <td>&nbsp;</td>\n");
         fprintf(fs, "      <td>&nbsp;</td>\n");
      }
      else if (cmsUtl_strcmp(dslLinkCfg->ATMQoS, MDMVS_UBRWPCR) == 0 ||
               cmsUtl_strcmp(dslLinkCfg->ATMQoS, MDMVS_CBR)     == 0)
      {
         fprintf(fs, "      <td>%d</td>\n", dslLinkCfg->ATMPeakCellRate);
         fprintf(fs, "      <td>&nbsp;</td>\n");
         fprintf(fs, "      <td>&nbsp;</td>\n");
      }
      else if (cmsUtl_strcmp(dslLinkCfg->ATMQoS, MDMVS_VBR_NRT) == 0 ||
               cmsUtl_strcmp(dslLinkCfg->ATMQoS, MDMVS_VBR_RT)  == 0)
      {
         fprintf(fs, "      <td>%d</td>\n", dslLinkCfg->ATMPeakCellRate);
         fprintf(fs, "      <td>%d</td>\n", dslLinkCfg->ATMSustainableCellRate);
         fprintf(fs, "      <td>%d</td>\n", dslLinkCfg->ATMMaximumBurstSize);
      }
#if defined (CHIP_63268) || defined(CHIP_63138) || defined(CHIP_63381) || defined(CHIP_63148) || defined(CHIP_63158)
      if ((dslLinkCfg->X_BROADCOM_COM_ATMMinimumCellRate > 0) &&
          (cmsUtl_strcmp(dslLinkCfg->ATMQoS, MDMVS_UBR)     == 0 ||
           cmsUtl_strcmp(dslLinkCfg->ATMQoS, MDMVS_UBRWPCR) == 0))
      {
         fprintf(fs, "      <td>%d</td>\n", dslLinkCfg->X_BROADCOM_COM_ATMMinimumCellRate);
      }
      else
      {
         fprintf(fs, "      <td>&nbsp;</td>\n");
      }
#endif      
      fprintf(fs, "      <td>%s</td>\n", dslLinkCfg->linkType);
      fprintf(fs, "      <td>%s</td>\n", dslLinkCfg->X_BROADCOM_COM_ConnectionMode);
      fprintf(fs, "      <td>%s</td>\n", dslLinkCfg->X_BROADCOM_COM_ATMEnbQos ? "Support" : "NoSupport");
#if defined (CHIP_63268) || defined(CHIP_63138) || defined(CHIP_63381) || defined(CHIP_63148) || defined(CHIP_63158)
      if (strcmp(dslLinkCfg->X_BROADCOM_COM_GrpScheduler, MDMVS_SP) == 0)
      {
         fprintf(fs, "      <td>&nbsp;</td>\n");
      }
      else
      {
         fprintf(fs, "      <td>%d/WRR/%d</td>\n",
                 dslLinkCfg->X_BROADCOM_COM_GrpPrecedence,
                 dslLinkCfg->X_BROADCOM_COM_GrpWeight);
      }
#endif
      /* remove check box */
      fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s'></td>\n", dslLinkCfg->X_BROADCOM_COM_IfName);
      fprintf(fs, "   </tr>\n");
      /* free dslLinkCfg */
      cmsObj_free((void **) &dslLinkCfg);
      layer2Count++;
   }

   writeXtmFooter(fs, layer2Count);
   
   /* reset atm to default value for next add  */
   getDefaultAtmLinkCfg(&glbWebVar);
   
}
#endif /* DMP_ADSLWAN_1 */

static CmsRet cgiDslAtmAdd(char *query, FILE *fs) 
{
   char str[BUFLEN_32];

   cgiGetValueByName(query, "portId", str);
   glbWebVar.portId = atoi(str);
   cgiGetValueByName(query, "atmVpi", str);
   glbWebVar.atmVpi = atoi(str);
   cgiGetValueByName(query, "atmVci", str);
   glbWebVar.atmVci = atoi(str);
   cgiGetValueByName(query, "connMode",  str);
   glbWebVar.connMode = atoi(str);
   cgiGetValueByName(query, "linkType", glbWebVar.linkType);
   cgiGetValueByName(query, "encapMode", str);
   glbWebVar.encapMode = atoi(str);
   cgiGetValueByName(query, "atmServiceCategory", glbWebVar.atmServiceCategory);
   cgiGetValueByName(query, "atmPeakCellRate", str);
   glbWebVar.atmPeakCellRate = atoi(str);
   cgiGetValueByName(query, "atmSustainedCellRate", str);
   glbWebVar.atmSustainedCellRate = atoi(str);
   cgiGetValueByName(query, "atmMaxBurstSize", str);
   glbWebVar.atmMaxBurstSize = atoi(str);
   cgiGetValueByName(query, "atmMinCellRate", str);
   glbWebVar.atmMinCellRate = atoi(str);
   cgiGetValueByName(query, "enblQos", str);
   glbWebVar.enblQos = atoi(str);
   
   if (cgiGetValueByName(query, "grpAlg", glbWebVar.grpScheduler) != CGI_STS_OK ||
       strcmp(glbWebVar.grpScheduler, MDMVS_SP) == 0)
   {
      /* set to RR as default */
      strcpy(glbWebVar.grpScheduler, MDMVS_WRR);
      glbWebVar.grpWeight = 1;
   }
   else
   {
      if (cgiGetValueByName(query, "grpWght", str) == CGI_STS_OK)
      {
         glbWebVar.grpWeight = atoi(str);
      }
      else
      {
         glbWebVar.grpWeight = 1; /* set to default queue weight */
      }
   }
   if (cgiGetValueByName(query, "grpPrec", str) == CGI_STS_OK)
   {
      glbWebVar.grpPrecedence = atoi(str);
   }
   else
   {
      glbWebVar.grpPrecedence = glbQosPrioLevel;  /* set to the lowest precedence */
   }

   if (cgiGetValueByName(query, "alg", glbWebVar.schedulerAlgorithm) != CGI_STS_OK ||
       strcmp(glbWebVar.schedulerAlgorithm, MDMVS_SP) == 0)
   {
      /* set to RR as default */
      strcpy(glbWebVar.schedulerAlgorithm, MDMVS_WRR);
      glbWebVar.queueWeight     = 1;
      glbWebVar.queuePrecedence = glbQosPrioLevel;  /* set to the lowest precedence */
   }
   else
   {
      if (cgiGetValueByName(query, "wght", str) == CGI_STS_OK)
      {
         glbWebVar.queueWeight = atoi(str);
      }
      else
      {
         glbWebVar.queueWeight = 1; /* set to default queue weight */
      }
   }
   if (cgiGetValueByName(query, "prec", str) == CGI_STS_OK)
   {
      glbWebVar.queuePrecedence = atoi(str);
   }
   else
   {
      glbWebVar.queuePrecedence = glbQosPrioLevel;  /* set to the lowest precedence */
   }
   
   if (cgiGetValueByName(query, "dropAlg", glbWebVar.dropAlgorithm) == CGI_STS_OK)
   {
      if (cmsUtl_strcmp(glbWebVar.dropAlgorithm, MDMVS_RED) == 0)
      {
         if (cgiGetValueByName(query, "loMinThr", str) == CGI_STS_OK)
         {
            glbWebVar.hiMinThreshold = glbWebVar.loMinThreshold = atoi(str);

            if (cgiGetValueByName(query, "loMaxThr", str) == CGI_STS_OK)
            {
               glbWebVar.hiMaxThreshold = glbWebVar.loMaxThreshold = atoi(str);
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
      else if (cmsUtl_strcmp(glbWebVar.dropAlgorithm, MDMVS_WRED) == 0)
      {
         if (cgiGetValueByName(query, "loMinThr", str) == CGI_STS_OK)
         {
            glbWebVar.loMinThreshold = atoi(str);

            if (cgiGetValueByName(query, "loMaxThr", str) == CGI_STS_OK)
            {
               glbWebVar.loMaxThreshold = atoi(str);
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
         
         if (cgiGetValueByName(query, "hiMinThr", str) == CGI_STS_OK)
         {
            glbWebVar.hiMinThreshold = atoi(str);

            if (cgiGetValueByName(query, "hiMaxThr", str) == CGI_STS_OK)
            {
               glbWebVar.hiMaxThreshold = atoi(str);
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
   else
   {
      cmsUtl_strcpy(glbWebVar.dropAlgorithm, MDMVS_DT);
   }

   if (dalDsl_addAtmInterface(&glbWebVar) != CMSRET_SUCCESS)
   {
      do_ej("/webs/dsladderr.html", fs);
      return CMSRET_INTERNAL_ERROR;
   }
   else
   {
      cmsLog_debug("dalDsl_addAtmInterface ok.");
      /*
       * dsl intf add was successful, tell handle_request to save the config
       * before releasing the lock.
       */
      glbSaveConfigNeeded = TRUE;
   }

   return CMSRET_SUCCESS;
}


static CmsRet cgiDslAtmRemove(char *query, FILE *fs) 
{
   char *pToken = NULL, *pLast = NULL;
   char lst[BUFLEN_1024];
   CmsRet ret=CMSRET_SUCCESS;
   
   cgiGetValueByName(query, "rmLst", lst);
   pToken = strtok_r(lst, ", ", &pLast);

   while (pToken != NULL)
   {
      strcpy(glbWebVar.wanL2IfName, pToken);

      if ((ret = dalDsl_deleteAtmInterface(&glbWebVar)) == CMSRET_REQUEST_DENIED)
      {
         do_ej("/webs/atmdelerr.html", fs);      
         return ret;
      }
      else if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("dalDsl_deleteAtmInterface failed for %s (ret=%d)", glbWebVar.wanL2IfName, ret);
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



void cgiDslAtmCfg(char *query, FILE *fs) 
{
   char action[BUFLEN_256];

   cgiGetValueByName(query, "action", action);
   if (cmsUtl_strcmp(action, "add") == 0)
   {
      if (cgiDslAtmAdd(query, fs) != CMSRET_SUCCESS) 
      {
         return;
      }
   }
   else if (cmsUtl_strcmp(action, "remove") == 0)
   {
      if (cgiDslAtmRemove(query, fs) != CMSRET_SUCCESS)
      {
         return;
      }
   }

   /* for DSL ATM Interface display */
   cgiDslAtmCfgView(fs);

}

#ifdef SUPPORT_PTM

#ifdef DMP_PTMWAN_1
/* 
 * PTM configuration
 */

void cgiDslPtmCfgView_igd(FILE *fs) 
{
   UINT32 portId;
   WanPtmLinkCfgObject *ptmLink = NULL;   
   InstanceIdStack parentIidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack ptmIidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 isAtm;

   /* For PTM layer 2 interface, the max layer 2 count is 4 and
   * since writeXtmFooter function check layer2Count against 8,
   * just have ptm layer2Count starts with 4.
   */
   SINT32 layer2Count = 4;
   
   /* write Java Script */
   writeDslXtmCfgScript(fs, "cfgptm", "dslptm");

   /* write table */
   fprintf(fs, "<b>DSL PTM Interface Configuration</b><br><br>\n");
   fprintf(fs, "Choose Add, or Remove to configure DSL PTM interfaces.<br><br>\n");
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");
   /* write table header */
   fprintf(fs, "   <tr align='center'>\n");
   fprintf(fs, "      <td class='hd'>Interface</td>\n");
   fprintf(fs, "      <td class='hd'>DSL Latency</td>\n");
   fprintf(fs, "      <td class='hd'>PTM Priority</td>\n");
   fprintf(fs, "      <td class='hd'>Conn Mode</td>\n");
   fprintf(fs, "      <td class='hd'>IP QoS</td>\n");
   fprintf(fs, "      <td class='hd'>Remove</td>\n");
   fprintf(fs, "   </tr>\n");

   isAtm = FALSE;
   if (dalDsl_getDslWanDevIidStack(isAtm, &parentIidStack) == TRUE)
   {
      while (layer2Count < IFC_WAN_MAX &&
             cmsObj_getNextInSubTree(MDMOID_WAN_PTM_LINK_CFG, 
             &parentIidStack,
             &ptmIidStack,
             (void **)&ptmLink) == CMSRET_SUCCESS)
      {
         portId = ptmLink->X_BROADCOM_COM_PTMPortId;

         fprintf(fs, "   <tr align='center'>\n");
         fprintf(fs, "      <td>%s</td>\n", ptmLink->X_BROADCOM_COM_IfName);
         fprintf(fs, "      <td>%s</td>\n", (portId == PHY0_PATH0)? "Path0" :
                                            (portId == PHY0_PATH1)? "Path1" :
                                            (portId == PHY0_PATH0_PATH1)? "Path0&1" : "");
         if (ptmLink->X_BROADCOM_COM_PTMPriorityLow && ptmLink->X_BROADCOM_COM_PTMPriorityHigh)
         {
            fprintf(fs, "      <td>%s</td>\n", "Normal&High");
         }
         else if (ptmLink->X_BROADCOM_COM_PTMPriorityLow)
         {
            fprintf(fs, "      <td>%s</td>\n", "Normal");
         }
         else if (ptmLink->X_BROADCOM_COM_PTMPriorityHigh)
         {
            fprintf(fs, "      <td>%s</td>\n", "High");
         }
         else
         {
            fprintf(fs, "      <td>%s</td>\n", "");
         }
         fprintf(fs, "      <td>%s</td>\n", ptmLink->X_BROADCOM_COM_ConnectionMode);
         fprintf(fs, "      <td>%s</td>\n", ptmLink->X_BROADCOM_COM_PTMEnbQos ? "Support" : "NoSupport");
         /* remove check box */
         fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s'></td>\n",
         ptmLink->X_BROADCOM_COM_IfName);
         fprintf(fs, "   </tr>\n");
         
         /* free ptmLink */
         cmsObj_free((void **) &ptmLink);
         layer2Count++;
      }
   }
   
   writeXtmFooter(fs, layer2Count);

   /* reset ptm to default value for next add  */
   getDefaultPtmLinkCfg(&glbWebVar);
   
}

#endif  /*  DMP_PTMWAN_1 */

static CmsRet cgiDslPtmAdd(char *query, FILE *fs) 
{
   char str[BUFLEN_16];
   
   cgiGetValueByName(query, "portId", str);
   glbWebVar.portId = atoi(str);
   cgiGetValueByName(query, "ptmPriorityNorm", str);
   glbWebVar.ptmPriorityNorm = atoi(str);
   cgiGetValueByName(query, "ptmPriorityHigh", str);
   glbWebVar.ptmPriorityHigh = atoi(str);
   cgiGetValueByName(query, "connMode",  str);
   glbWebVar.connMode = atoi(str);
   cgiGetValueByName(query, "enblQos", str);
   glbWebVar.enblQos = atoi(str);

   if (cgiGetValueByName(query, "grpAlg", glbWebVar.grpScheduler) != CGI_STS_OK ||
       strcmp(glbWebVar.grpScheduler, MDMVS_SP) == 0)
   {
      /* set to RR as default */
      strcpy(glbWebVar.grpScheduler, MDMVS_WRR);
      glbWebVar.grpWeight = 1;
   }
   else
   {
      if (cgiGetValueByName(query, "grpWght", str) == CGI_STS_OK)
      {
         glbWebVar.grpWeight = atoi(str);
      }
      else
      {
         glbWebVar.grpWeight = 1; /* set to default queue weight */
      }
   }
   if (cgiGetValueByName(query, "grpPrec", str) == CGI_STS_OK)
   {
      glbWebVar.grpPrecedence = atoi(str);
   }
   else
   {
      glbWebVar.grpPrecedence = glbQosPrioLevel;  /* set to the lowest precedence */
   }

   if (cgiGetValueByName(query, "alg", glbWebVar.schedulerAlgorithm) != CGI_STS_OK ||
       strcmp(glbWebVar.schedulerAlgorithm, MDMVS_SP) == 0)
   {
      /* set to WRR as default */
      strcpy(glbWebVar.schedulerAlgorithm, MDMVS_WRR);
      glbWebVar.queueWeight     = 1;
      glbWebVar.queuePrecedence = glbQosPrioLevel;  /* set to the lowest precedence */
   }
   else
   {
      if (cgiGetValueByName(query, "wght", str) == CGI_STS_OK)
      {
         glbWebVar.queueWeight = atoi(str);
      }
      else
      {
         glbWebVar.queueWeight = 1;
      }
   }
   if (cgiGetValueByName(query, "prec", str) == CGI_STS_OK)
   {
      glbWebVar.queuePrecedence = atoi(str);
   }
   else
   {
      glbWebVar.queuePrecedence = glbQosPrioLevel;  /* set to the lowest precedence */
   }

   if (cgiGetValueByName(query, "dropAlg", glbWebVar.dropAlgorithm) == CGI_STS_OK)
   {
      if (cmsUtl_strcmp(glbWebVar.dropAlgorithm, MDMVS_RED) == 0)
      {
         if (cgiGetValueByName(query, "loMinThr", str) == CGI_STS_OK)
         {
            glbWebVar.hiMinThreshold = glbWebVar.loMinThreshold = atoi(str);

            if (cgiGetValueByName(query, "loMaxThr", str) == CGI_STS_OK)
            {
               glbWebVar.hiMaxThreshold = glbWebVar.loMaxThreshold = atoi(str);
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
      else if (cmsUtl_strcmp(glbWebVar.dropAlgorithm, MDMVS_WRED) == 0)
      {
         if (cgiGetValueByName(query, "loMinThr", str) == CGI_STS_OK)
         {
            glbWebVar.loMinThreshold = atoi(str);

            if (cgiGetValueByName(query, "loMaxThr", str) == CGI_STS_OK)
            {
               glbWebVar.loMaxThreshold = atoi(str);
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
         
         if (cgiGetValueByName(query, "hiMinThr", str) == CGI_STS_OK)
         {
            glbWebVar.hiMinThreshold = atoi(str);

            if (cgiGetValueByName(query, "hiMaxThr", str) == CGI_STS_OK)
            {
               glbWebVar.hiMaxThreshold = atoi(str);
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
   else
   {
      cmsUtl_strcpy(glbWebVar.dropAlgorithm, MDMVS_DT);
   }
   
   glbWebVar.queueMinimumRate = QOS_QUEUE_NO_SHAPING; /* pre-set to default */
   glbWebVar.queueShapingRate = QOS_QUEUE_NO_SHAPING; /* pre-set to default */
   glbWebVar.queueShapingBurstSize = 0;               /* pre-set to default */
   if (cgiGetValueByName(query, "minrate", str) == CGI_STS_OK)
   {
      glbWebVar.queueMinimumRate = atoi(str) * 1000;
   }
   if (cgiGetValueByName(query, "shaperate", str) == CGI_STS_OK)
   {
      glbWebVar.queueShapingRate = atoi(str) * 1000;
   }
   if (glbWebVar.queueMinimumRate > QOS_QUEUE_NO_SHAPING ||
       glbWebVar.queueShapingRate > QOS_QUEUE_NO_SHAPING)
   {
      if (cgiGetValueByName(query, "burstsize", str) == CGI_STS_OK)
      {
         glbWebVar.queueShapingBurstSize = atoi(str);
      }
   }
   
   if (dalDsl_addPtmInterface(&glbWebVar) != CMSRET_SUCCESS)
   {
      do_ej("/webs/ptmadderr.html", fs);
      return CMSRET_INTERNAL_ERROR;
   }
   else
   {
      cmsLog_debug("dalDsl_addPtmInterface ok.");
      /*
       * dsl PTM add was successful, tell handle_request to save the config
       * before releasing the lock.
       */
      glbSaveConfigNeeded = TRUE;
   }

   return CMSRET_SUCCESS;
   
}

static CmsRet cgiDslPtmRemove(char *query, FILE *fs) 
{
   char *pToken = NULL, *pLast = NULL;
   char lst[BUFLEN_1024];
   CmsRet ret=CMSRET_SUCCESS;
   
   cgiGetValueByName(query, "rmLst", lst);
   pToken = strtok_r(lst, ", ", &pLast);

   while (pToken != NULL)
   {
      strcpy(glbWebVar.wanL2IfName, pToken);
   
      if ((ret = dalDsl_deletePtmInterface(&glbWebVar)) == CMSRET_REQUEST_DENIED)
      {
         do_ej("/webs/ptmdelerr.html", fs);      
         return ret;
      }
      else if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("dalDsl_deletePtmInterface failed for  failed for %s (ret=%d)", glbWebVar.wanL2IfName, ret);
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


void cgiDslPtmCfg(char *query, FILE *fs) 
{
   char action[BUFLEN_256];

   cgiGetValueByName(query, "action", action);
   if (cmsUtl_strcmp(action, "add") == 0)
   {
      if (cgiDslPtmAdd(query, fs) != CMSRET_SUCCESS) 
      {
         return;
      }
   }
   else if (cmsUtl_strcmp(action, "remove") == 0)
   {
      if (cgiDslPtmRemove(query, fs) != CMSRET_SUCCESS)
      {
         return;
      }
   }

   /* for DSL PTM Interface display */
   cgiDslPtmCfgView(fs);
   
}

#endif  /*  SUPPORT_PTM */


#endif  /*  SUPPORT_DSL */

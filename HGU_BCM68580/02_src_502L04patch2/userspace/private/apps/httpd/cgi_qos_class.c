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

/*!\file cgi_qos_class.c
 * \brief this file contains all the data model independent code
 *        for QoS classification as well as the TR98 specific code for
 *        QoS classification.
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

/* local constants */


/* Local functions */
// static void cgiQosClsEdit(char *query, FILE *fs);
void cgiQosClsView(FILE *fs);
static void writeQosClsScript(FILE *fs);


void cgiQosCls(char *query, FILE *fs)
{
   char action[WEB_MD_BUF_SIZE_MAX];

   cgiGetValueByName(query, "action", action);

   if (strcmp(action, "add") == 0)
   {
      cgiQosClsAdd(query, fs);
   }
#ifdef not_supported
   else if (strcmp(action, "edit") == 0)
   {
      cgiQosClsEdit(query, fs);
   }
#endif
   else if (strcmp(action, "enable") == 0)
   {
      cgiQosClsEnable(query, fs);

      /* refresh the page */
      cgiQosClsView(fs);
   }
   else if (strcmp(action, "remove") == 0)
   {
      cgiQosClsRemove(query, fs);

      /* refresh the page */
      cgiQosClsView(fs);
   }
   else
   {
      cgiQosClsView(fs);
   }
}  /* End of cgiQosCls() */

void writeQosClsScript(FILE *fs)
{
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   fprintf(fs, "function addClick() {\n");
   fprintf(fs, "   var loc = 'qoscls.html';\n\n");
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function enableClick(ebl) {\n");
   fprintf(fs, "   var lst = '';\n");
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
   fprintf(fs, "   var loc = 'qoscls.cmd?action=enable&eblLst=' + lst;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function removeClick(rml) {\n");
   fprintf(fs, "   var lst = '';\n");
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
   fprintf(fs, "   var loc = 'qoscls.cmd?action=remove&rmLst=' + lst;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function editClick(clsKey) {\n");
   fprintf(fs, "   var gName = '';\n");
   fprintf(fs, "   gName += name;\n");
   fprintf(fs, "   var loc = 'qosclsedit.cgi?qosClsKey=' + clsKey;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");

}  /* End of writeQosClsScript() */

void cgiQosClsView(FILE *fs)
{
   char qMgmtEnbleBuf[BUFLEN_8]={0};
   UBOOL8 qMgmtEnable;


   writeSecurityHeader(fs);
   writeQosClsScript(fs);
   writeSecurityBody(fs);

   fprintf(fs, "<b>QoS Classification Setup -- maximum %d rules can be configured.</b><br><br>\n", QOS_CLS_MAX_ENTRY);
   fprintf(fs, "To add a rule, click the <b>Add</b> button.<br>\n");
   fprintf(fs, "To remove rules, check their remove-checkboxes, then click the <b>Remove</b> button.<br>\n");
   fprintf(fs, "The <b>Enable</b> button will scan through every rules in the table. \n");
   fprintf(fs, "Rules with enable-checkbox checked will be enabled. Rules with enable-checkbox un-checked will be disabled.<br>\n");
   fprintf(fs, "The enable-checkbox also shows status of the rule after page reload.<br>\n");
#ifdef BRCM_WLAN
   fprintf(fs, "If you disable WMM function in Wireless Page, classification related to wireless will not take effects<br>\n");
#endif
   fprintf(fs, "<br>\n");


   cgiGetQosMgmtEnbl(0, NULL, qMgmtEnbleBuf);
   qMgmtEnable = (qMgmtEnbleBuf[0] == '1');

   /* write alert info */
   if (!qMgmtEnable)
   {
      fprintf(fs, "<b>The QoS function has been disabled. Classification rules would not take effects.</b><br><br>\n");
   }
      
   fprintf(fs, "<center>\n");
   fprintf(fs, "<table border='1' cellpadding='2' cellspacing='0'>\n");
   // write table header for DSCP configuration display
   fprintf(fs, "    <tr>\n");
   fprintf(fs, "      <td class='hd' colspan='2'>&nbsp</td>\n");
   fprintf(fs, "      <td class='hd' colspan='11'><center>CLASSIFICATION CRITERIA</center></td>\n");
#if defined (SUPPORT_RATE_LIMIT) && defined (SUPPORT_POLICING)
   fprintf(fs, "      <td class='hd' colspan='5'><center>CLASSIFICATION RESULTS</center></td>\n");
#elif defined (SUPPORT_RATE_LIMIT) || defined (SUPPORT_POLICING)
   fprintf(fs, "      <td class='hd' colspan='4'><center>CLASSIFICATION RESULTS</center></td>\n");
#else
   fprintf(fs, "      <td class='hd' colspan='3'><center>CLASSIFICATION RESULTS</center></td>\n");
#endif
   fprintf(fs, "      <td class='hd' colspan='2'>&nbsp</td>\n");
   fprintf(fs, "    </tr>\n");
   fprintf(fs, "    <tr>\n");
   fprintf(fs, "      <td class='hd'>Class Name</td>\n");
   fprintf(fs, "      <td class='hd'>Order</td>\n");
   fprintf(fs, "      <td class='hd'>Class Intf</td>\n");
   fprintf(fs, "      <td class='hd'>Ether Type</td>\n");
   fprintf(fs, "      <td class='hd'>SrcMAC/ Mask</td>\n");
   fprintf(fs, "      <td class='hd'>DstMAC/ Mask</td>\n");
   fprintf(fs, "      <td class='hd'>SrcIP/ PrefixLength</td>\n");
   fprintf(fs, "      <td class='hd'>DstIP/ PrefixLength</td>\n");
   fprintf(fs, "      <td class='hd'>Proto</td>\n");
   fprintf(fs, "      <td class='hd'>SrcPort</td>\n");
   fprintf(fs, "      <td class='hd'>DstPort</td>\n");
   fprintf(fs, "      <td class='hd'>DSCP Check</td>\n");
   fprintf(fs, "      <td class='hd'>802.1P Check</td>\n");   
   fprintf(fs, "      <td class='hd'>Queue Key</td>\n");
#ifdef SUPPORT_POLICING
   fprintf(fs, "      <td class='hd'>Policer Key</td>\n");
#endif
   fprintf(fs, "      <td class='hd'>DSCP Mark</td>\n");
   fprintf(fs, "      <td class='hd'>802.1P Mark</td>\n");   
#ifdef SUPPORT_RATE_LIMIT
   fprintf(fs, "      <td class='hd'>Rate Limit(kbps)</td>\n");
#endif
   fprintf(fs, "      <td class='hd'>Enable</td>\n");   
   fprintf(fs, "      <td class='hd'>Remove</td>\n");
//   fprintf(fs, "      <td class='hd'>Edit</td>\n");
   fprintf(fs, "   </tr>\n");

   // write table body
   cgiQosClsViewBody(fs);

   fprintf(fs, "</table><br>\n");
   fprintf(fs, "<input type='button' onClick='addClick()' value='Add'>\n");
   fprintf(fs, "<input type='button' onClick='enableClick(this.form.enbl)' value='Enable'>\n");
   fprintf(fs, "<input type='button' onClick='removeClick(this.form.rml)' value='Remove'>\n");
   fprintf(fs, "</center>\n");
   fprintf(fs, "</form>\n");
   fprintf(fs, "</blockquote>\n");
   fprintf(fs, "</body>\n");
   fprintf(fs, "</html>\n");
   fflush(fs);

}  /* End of cgiQosClsView() */


#ifdef DMP_QOS_1

void cgiQosClsViewBody_igd(FILE *fs)
{
   const char *excludeStr = "(E)";
   InstanceIdStack iidStack;
   QMgmtClassificationObject *cObj = NULL;
   CmsRet ret;

   /* display each entry in the classification table. */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_Q_MGMT_CLASSIFICATION, &iidStack, (void **)&cObj)) == CMSRET_SUCCESS)
   {
//      BOOL queueEnable = FALSE;

//      if (cObj->classQueue > 0)
//      {
//         InstanceIdStack iidStack2;
//         QMgmtQueueObject *qObj = NULL;

//         INIT_INSTANCE_ID_STACK(&iidStack2);
//         iidStack2.instance[0] = cObj->classQueue;
//         iidStack2.currentDepth = 1;
//         if ((ret = cmsObj_get(MDMOID_Q_MGMT_QUEUE, &iidStack2, 0, (void **)&qObj)) != CMSRET_SUCCESS)
//         {
//            cmsLog_error("cmsObj_get <MDMOID_Q_MGMT_QUEUE> returns error. ret=%d", ret);
//            return;
//         }
//         queueEnable = qObj->queueEnable;
//         cmsObj_free((void **)&qObj);
//      }

      fprintf(fs, "   <tr>\n");

      /* display class name */
      fprintf(fs, "      <td>%s</td>\n", cObj->X_BROADCOM_COM_ClassName);

      /* display ClassificationOrder */
      fprintf(fs, "      <td>%d</td>\n", cObj->classificationOrder);

      /* display classification criterion- ClassInterface */
      if (!IS_EMPTY_STRING(cObj->classInterface))
      {
         char classIf[BUFLEN_16];

         /* find intfname from class interface full path */
         if (cmsUtl_strcmp(cObj->classInterface, MDMVS_LOCAL) == 0 ||
             cmsUtl_strcmp(cObj->classInterface, MDMVS_LAN)   == 0 ||
             cmsUtl_strcmp(cObj->classInterface, MDMVS_WAN)   == 0)
         {
            fprintf(fs, "      <td>%s</td>\n", cObj->classInterface);
         }
         else if ((ret = qdmIntf_fullPathToIntfnameLocked(cObj->classInterface, classIf)) == CMSRET_SUCCESS)
         {
#if 0 //JJC
            InstanceIdStack iidStackDevInfo;
            IGDDeviceInfoObject *devInfoObj;

            /* get the board id */
            INIT_INSTANCE_ID_STACK(&iidStackDevInfo);
            if ((ret = cmsObj_get(MDMOID_IGD_DEVICE_INFO, &iidStackDevInfo, 0, (void **)&devInfoObj)) == CMSRET_SUCCESS)
            {
               /* need Uname from Lname by calling getUNameByLName jscript */
               fprintf(fs, "<script language=\"javascript\">\n");
               fprintf(fs, "<!-- hide\n");
               fprintf(fs, "{\n");
               fprintf(fs, "   var i = 0;\n");
               fprintf(fs, "   var ifName = '';\n");
               fprintf(fs, "   var brdId = '%s';\n", devInfoObj->X_BROADCOM_COM_Board_Id);
               fprintf(fs, "   ifName = getUNameByLName(brdId + '|' + '%s');\n", classIf);
               fprintf(fs, "   document.write(\"<td>\" + ifName + \"</td>\");\n");
               fprintf(fs, "}\n");
               fprintf(fs, "// done hiding -->\n");
               fprintf(fs, "</script>\n");
               cmsObj_free((void **)&devInfoObj);
            }
            else
            {
               fprintf(fs, "      <td>&nbsp</td>\n");
            }
#endif
            fprintf(fs, "      <td>%s</td>\n", classIf);
         }
         else
         {
            fprintf(fs, "      <td>&nbsp</td>\n");
         }
      }
      else
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }

      /* display classification criterion- Ethertype */
      if (cObj->ethertype == ETH_P_IP)
      {
         fprintf(fs, "      <td>IP%s</td>\n", cObj->protocolExclude? excludeStr : "");
      }
      else if (cObj->ethertype == ETH_P_ARP)
      {
         fprintf(fs, "      <td>ARP%s</td>\n", cObj->protocolExclude? excludeStr : "");
      }
      else if (cObj->ethertype == ETH_P_IPV6)
      {
         fprintf(fs, "      <td>IPv6%s</td>\n", cObj->protocolExclude? excludeStr : "");
      }
      else if (cObj->ethertype == ETH_P_PPP_DISC)
      {
         fprintf(fs, "      <td>PPPoE_DISC%s</td>\n", cObj->protocolExclude? excludeStr : "");
      }
      else if (cObj->ethertype == ETH_P_PPP_SES)
      {
         fprintf(fs, "      <td>PPPoE_SES%s</td>\n", cObj->protocolExclude? excludeStr : "");
      }
      else if (cObj->ethertype == 0x8865)
      {
         fprintf(fs, "      <td>0x8865%s</td>\n", cObj->protocolExclude? excludeStr : "");
      }
      else if (cObj->ethertype == 0x8866)
      {
         fprintf(fs, "      <td>0x8866%s</td>\n", cObj->protocolExclude? excludeStr : "");
      }
      else if (cObj->ethertype == ETH_P_8021Q)
      {
         fprintf(fs, "      <td>8021Q%s</td>\n", cObj->protocolExclude? excludeStr : "");
      }
      else
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }

      /* display classification criterion- SourceMACAddress */
      if (!IS_EMPTY_STRING(cObj->sourceMACAddress))
      {
         if (!IS_EMPTY_STRING(cObj->sourceMACMask))
         {
            fprintf(fs, "      <td>%s/ %s%s</td>\n", cObj->sourceMACAddress, cObj->sourceMACMask, cObj->sourceMACExclude? excludeStr : "");
         }
         else
         {
            fprintf(fs, "      <td>%s%s</td>\n", cObj->sourceMACAddress, cObj->sourceMACExclude? excludeStr : "");
         }
      }
      else
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }

      /* display classification criterion- DestMACAddress */
      if (!IS_EMPTY_STRING(cObj->destMACAddress))
      {
         if (!IS_EMPTY_STRING(cObj->destMACMask))
         {
            fprintf(fs, "      <td>%s/ %s%s</td>\n", cObj->destMACAddress, cObj->destMACMask, cObj->destMACExclude? excludeStr : "");
         }
         else
         {
            fprintf(fs, "      <td>%s%s</td>\n", cObj->destMACAddress, cObj->destMACExclude? excludeStr : "");
         }
      }
      else
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }

      /* display classification criterion- either SourceIP or SourceVendorClassID or SourceUserClassID */
      if (!IS_EMPTY_STRING(cObj->sourceIP))
      {
         if (strchr(cObj->sourceIP, ':') != NULL)
         {
            /* IPv6 address */
            fprintf(fs, "      <td>%s%s</td>\n", cObj->sourceIP, cObj->sourceIPExclude? excludeStr : "");
         }
         else
         {
            /* IPv4 address */
            if (!IS_EMPTY_STRING(cObj->sourceMask))
            {
               fprintf(fs, "      <td>%s/%d%s</td>\n", cObj->sourceIP, cmsNet_getLeftMostOneBitsInMask(cObj->sourceMask), cObj->sourceIPExclude? excludeStr : "");
            }
            else
            {
               fprintf(fs, "      <td>%s%s</td>\n", cObj->sourceIP, cObj->sourceIPExclude? excludeStr : "");
            }
         }
      }
      else if (!IS_EMPTY_STRING(cObj->sourceVendorClassID))
      {
         fprintf(fs, "      <td>DHCP OP%d:%s%s</td>\n", DHCP_VENDOR_CLASS_OPTION, cObj->sourceVendorClassID, cObj->sourceVendorClassIDExclude? excludeStr : "");
      }
      else if (!IS_EMPTY_STRING(cObj->sourceUserClassID))
      {
         fprintf(fs, "      <td>DHCP OP%d:%s%s</td>\n", DHCP_USER_CLASS_OPTION, cObj->sourceUserClassID, cObj->sourceUserClassIDExclude? excludeStr : "");
      }
      else
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }

      /* display classification criterion- DestIP */
      if (!IS_EMPTY_STRING(cObj->destIP))
      {
         if (strchr(cObj->destIP, ':') != NULL)
         {
            /* IPv6 address */
            fprintf(fs, "      <td>%s%s</td>\n", cObj->destIP, cObj->destIPExclude? excludeStr : "");
         }
         else
         {
            /* IPv4 address */
            if (!IS_EMPTY_STRING(cObj->destMask))
            {
               fprintf(fs, "      <td>%s/%d%s</td>\n", cObj->destIP, cmsNet_getLeftMostOneBitsInMask(cObj->destMask), cObj->destIPExclude? excludeStr : "");
            }
            else
            {
               fprintf(fs, "      <td>%s%s</td>\n", cObj->destIP, cObj->destIPExclude? excludeStr : "");
            }
         }
      }
      else
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }

      /* display classification criterion- Protocol */
      if (cObj->protocol == IPPROTO_TCP)
      {
         fprintf(fs, "      <td>TCP%s</td>\n", cObj->protocolExclude? excludeStr : "");
      }
      else if (cObj->protocol == IPPROTO_UDP)
      {
         fprintf(fs, "      <td>UDP%s</td>\n", cObj->protocolExclude? excludeStr : "");
      }
      else if (cObj->protocol == IPPROTO_ICMP)
      {
         fprintf(fs, "      <td>ICMP%s</td>\n", cObj->protocolExclude? excludeStr : "");
      }
      else if (cObj->protocol == IPPROTO_IGMP)
      {
         fprintf(fs, "      <td>IGMP%s</td>\n", cObj->protocolExclude? excludeStr : "");
      }
      else if (cObj->protocol == IPPROTO_ICMPV6)
      {
         fprintf(fs, "      <td>ICMPV6%s</td>\n", cObj->protocolExclude? excludeStr : "");
      }
      else
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }

      /* display classification criterion- SourcePort, SourcePortRangeMax */
      if (cObj->sourcePort != QOS_CRITERION_UNUSED)
      {
         if (cObj->sourcePortRangeMax != QOS_CRITERION_UNUSED)
         {
            fprintf(fs, "      <td>%d:%d%s</td>\n", cObj->sourcePort, cObj->sourcePortRangeMax, cObj->sourcePortExclude? excludeStr : "");
         }
         else
         {
            fprintf(fs, "      <td>%d%s</td>\n", cObj->sourcePort, cObj->sourcePortExclude? excludeStr : "");
         }
      }
      else
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }

      /* display classification criterion- DestPort, DestPortRangeMax */
      if (cObj->destPort != QOS_CRITERION_UNUSED)
      {
         if (cObj->destPortRangeMax != QOS_CRITERION_UNUSED)
         {
            fprintf(fs, "      <td>%d:%d%s</td>\n", cObj->destPort, cObj->destPortRangeMax, cObj->destPortExclude? excludeStr : "");
         }
         else
         {
            fprintf(fs, "      <td>%d%s</td>\n", cObj->destPort, cObj->destPortExclude? excludeStr : "");
         }
      }
      else
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }

      /* display classification criterion- DSCPCheck */
      fprintf(fs, "      <td>%s%s</td>\n", cgiQosDscpMarkToName(cObj->DSCPCheck), cObj->DSCPExclude ? excludeStr : "");

      /* display classification criterion- EthernetPriorityCheck */
      if (cObj->ethernetPriorityCheck != QOS_CRITERION_UNUSED)
      {
         fprintf(fs, "      <td>%d%s</td>\n", cObj->ethernetPriorityCheck, cObj->ethernetPriorityExclude ? excludeStr : "");
      }
      else
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }

      /* display classification result- ClassQueue */
      fprintf(fs, "      <td>%d</td>\n", cObj->classQueue);

#ifdef SUPPORT_POLICING
      /* display classification result- ClassPolicer */
      if (cObj->classPolicer != QOS_RESULT_NO_CHANGE)
      {
         fprintf(fs, "      <td>%d</td>\n", cObj->classPolicer);
      }
      else
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }
#endif

      /* display classification result- DSCPMark */
      fprintf(fs, "      <td>%s</td>\n", cgiQosDscpMarkToName(cObj->DSCPMark));

      /* display classification result- EthernetPriorityMark */
      if (cObj->ethernetPriorityMark != QOS_RESULT_NO_CHANGE)
      {
         fprintf(fs, "      <td>%d</td>\n", cObj->ethernetPriorityMark);
      }
      else
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }

#ifdef SUPPORT_RATE_LIMIT
      /* display classification result- ClassRate */
      if (cObj->X_BROADCOM_COM_ClassRate != QOS_RESULT_NO_CHANGE)
      {
         fprintf(fs, "      <td>%d</td>\n", cObj->X_BROADCOM_COM_ClassRate);
      }
      else
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }
#endif

      fprintf(fs, "      <td align='center'><input type='checkbox' name='enbl' %s></td>\n", cObj->classificationEnable? "checked" : "");
      fprintf(fs, "      <td align='center'><input type='checkbox' name='rml'></td>\n");
//      fprintf(fs, "      <td align='center'><input type='button' onClick='editClick(\"%d\")' value='&nbsp;Edit&nbsp;'\n></td>\n", cObj->classificationKey);

      fprintf(fs, "   </tr>\n");

      cmsObj_free((void **)&cObj);
   }
}


void cgiQosClsAdd_igd(char *query, FILE *fs)
{
   char cmd[WEB_MD_BUF_SIZE_MAX];
   QMgmtClassificationObject *cObj;
   InstanceIdStack iidStack;
   UBOOL8 isDuplicate;
   CmsRet ret;

   /* get default values for the classification object instance */
   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_get(MDMOID_Q_MGMT_CLASSIFICATION, &iidStack, OGF_DEFAULT_VALUES, (void **)&cObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_get returns error. ret=%d", ret);
      sprintf(cmd, "Add qos class failed. ret=%d", ret);
      cgiWriteMessagePage(fs, "QoS Classification Config Error", cmd, "qoscls.cmd?action=view");
      return;
   }

   /* get an unused classification key */
   if ((ret = dalQos_getAvailableClsKey(&(cObj->X_BROADCOM_COM_ClassKey))) != CMSRET_SUCCESS)
   {
      cmsLog_error("dalQos_getAvailableClsKey returns error. ret=%d", ret);
      sprintf(cmd, "Add qos class failed. ret=%d", ret);
      cgiWriteMessagePage(fs, "QoS Classification Config Error", cmd, "qoscls.cmd?action=view");
      cmsObj_free((void **)&cObj);
      return;
   }

   cmd[0] = '\0';

   /* className */
   if (cgiGetValueByName(query, "clsName", cmd) == CGI_STS_OK)
   {
      cmsMem_free(cObj->X_BROADCOM_COM_ClassName);
      cObj->X_BROADCOM_COM_ClassName = cmsMem_strdup(cmd);
   }
    
   /* ClassificationEnable */
   if (cgiGetValueByName(query, "rulsEnbl", cmd) == CGI_STS_OK)
   {
      cObj->classificationEnable = atoi(cmd);
   }

   /* ClassificationOrder */
   if (cgiGetValueByName(query, "rulsOrder", cmd) == CGI_STS_OK)
   {
      cObj->classificationOrder = atoi(cmd);
   }

   /* Ether type */
   if (cgiGetValueByName(query, "ethertype", cmd) == CGI_STS_OK)
   {
      cObj->ethertype = strtol(cmd, (char **)NULL, 16);
   }

   /* Protocol */
   if (cgiGetValueByName(query, "protocol", cmd) == CGI_STS_OK)
   {
      cObj->protocol = atoi(cmd);    /* TCP or UDP or ICMP or IGMP */ 
   }

   /* either SourceIP,SourceMask or SourceVendorClassID or SourceUserClassID */      
   if (cgiGetValueByName(query, "ipoptionlist", cmd) == CGI_STS_OK)
   {
      SINT32 ipoptionlist = atoi(cmd);

      if (cgiGetValueByName(query, "srcAddr", cmd) == CGI_STS_OK)
      {
         if (ipoptionlist == 0)
         {
            if (strchr(cmd, ':') != NULL)
            {
               /* IPv6 address */
               cmsMem_free(cObj->sourceIP);
               cObj->sourceIP = cmsMem_strdup(cmd);
            }
            else
            {
               /* IPv4 address */
               struct in_addr addr, mask;

               cmsNet_inet_cidrton(cmd, &addr, &mask);
               if (addr.s_addr != 0)
               {
                  cmsMem_free(cObj->sourceIP);
                  cObj->sourceIP = cmsMem_strdup(inet_ntoa(addr));

                  if (mask.s_addr != 0)
                  {
                     cmsMem_free(cObj->sourceMask);
                     cObj->sourceMask = cmsMem_strdup(inet_ntoa(mask));
                  }
               }
            }
         }
         else if (ipoptionlist == 1)
         {
            cmsMem_free(cObj->sourceVendorClassID);
            cObj->sourceVendorClassID = cmsMem_strdup(cmd);
         }
         else if (ipoptionlist == 2)
         {
            cmsMem_free(cObj->sourceUserClassID);
            cObj->sourceUserClassID = cmsMem_strdup(cmd);
         }
      }
   }

   /* EthernetPriorityCheck */
   if (cgiGetValueByName(query, "vlan8021p", cmd) == CGI_STS_OK)
   {
      cObj->ethernetPriorityCheck = atoi(cmd);
   }
  
   /* EthernetPriorityMark */
   if (cgiGetValueByName(query, "wanVlan8021p", cmd) == CGI_STS_OK)
   {
      cObj->ethernetPriorityMark = atoi(cmd);
   }
    
   /* DSCPCheck */
   if ((cgiGetValueByName(query, "dscpCheck", cmd)) == CGI_STS_OK)
   {
      cObj->DSCPCheck = atoi(cmd);
   }

   /* DSCPMark */
   if ((cgiGetValueByName(query, "dscpMark", cmd)) == CGI_STS_OK)
   {
      cObj->DSCPMark = atoi(cmd);
   }

   /* SourcePort and SourcePortRangeMax */
   if (cgiGetValueByName(query, "srcPort", cmd) == CGI_STS_OK)
   {
      char *portStr = NULL, *last = NULL;

      if ((portStr = strtok_r(cmd, ":", &last)) != NULL)
      {
         cObj->sourcePort = atoi(portStr);
      }
      if (cObj->sourcePort != QOS_CRITERION_UNUSED)
      {
         if ((portStr = strtok_r(NULL, ":", &last)) != NULL)
         {
            cObj->sourcePortRangeMax = atoi(portStr);
         }
      }
   }

   /* DestPort and DestPortRangeMax */
   if (cgiGetValueByName(query, "dstPort", cmd) == CGI_STS_OK)
   {
      char *portStr = NULL, *last = NULL;

      if ((portStr = strtok_r(cmd, ":", &last)) != NULL)
      {
         cObj->destPort = atoi(portStr);
      }
      if (cObj->destPort != QOS_CRITERION_UNUSED)
      {
         if ((portStr = strtok_r(NULL, ":", &last)) != NULL)
         {
            cObj->destPortRangeMax = atoi(portStr);
         }
      }
   }

   /* DestIP and DestMask */
   if ((cgiGetValueByName(query, "dstAddr", cmd)) == CGI_STS_OK)
   {
      if (strchr(cmd, ':') != NULL)
      {
         /* IPv6 address */
         cmsMem_free(cObj->destIP);
         cObj->destIP = cmsMem_strdup(cmd);
      }
      else
      {
         /* IPv4 address */
         struct in_addr addr, mask;

         cmsNet_inet_cidrton(cmd, &addr, &mask);
         if (addr.s_addr != 0)
         {
            cmsMem_free(cObj->destIP);
            cObj->destIP = cmsMem_strdup(inet_ntoa(addr));

            if (mask.s_addr != 0)
            {
               cmsMem_free(cObj->destMask);
               cObj->destMask = cmsMem_strdup(inet_ntoa(mask));
            }
         }
      }
   }

   /* ProtocolExclude */
   if (cgiGetValueByName(query, "protExclude", cmd) == CGI_STS_OK)
   {
      cObj->protocolExclude = atoi(cmd);
   }

   /* SourceIPExclude */
   if (cgiGetValueByName(query, "srcIpExclude", cmd) == CGI_STS_OK)
   {
      cObj->sourceIPExclude = atoi(cmd);
   }

   /* DestIPExclude */
   if (cgiGetValueByName(query, "destIpExclude", cmd) == CGI_STS_OK)
   {
      cObj->destIPExclude = atoi(cmd);
   }

   /* SourcePortExclude */
   if (cgiGetValueByName(query, "srcPortExclude", cmd) == CGI_STS_OK)
   {
      cObj->sourcePortExclude = atoi(cmd);
   }

   /* DestPortExclude */
   if (cgiGetValueByName(query, "destportExclude", cmd) == CGI_STS_OK)
   {
      cObj->destPortExclude = atoi(cmd);
   }

   /* EthernetPriorityExclude */
   if (cgiGetValueByName(query, "etherPrioExclude", cmd) == CGI_STS_OK)
   {
      cObj->ethernetPriorityExclude = atoi(cmd);
   }

   /* DSCPExclude */
   if (cgiGetValueByName(query, "dscpExclude", cmd) == CGI_STS_OK)
   {
      cObj->DSCPExclude = atoi(cmd);
   }

   /* ClassQueue */
   if (cgiGetValueByName(query, "queuekey", cmd) == CGI_STS_OK)
   {
      cObj->classQueue = atoi(cmd);   
   }

#ifdef SUPPORT_POLICING
   /* ClassPolicer */
   if (cgiGetValueByName(query, "policerkey", cmd) == CGI_STS_OK)
   {
      cObj->classPolicer = atoi(cmd);   
   }
#endif

#ifdef SUPPORT_RATE_LIMIT
    /* rate control */
   if (cgiGetValueByName(query, "rate", cmd) == CGI_STS_OK)
   {
      cObj->X_BROADCOM_COM_ClassRate = atoi(cmd);
   }
#endif

   /* egress interface */
   if (cgiGetValueByName(query, "egress", cmd) == CGI_STS_OK)
   {
      cmsMem_free(cObj->X_BROADCOM_COM_egressInterface);
      cObj->X_BROADCOM_COM_egressInterface = cmsMem_strdup(cmd);
   }

   /* ClassInterface */
   if (cgiGetValueByName(query, "classIf", cmd) == CGI_STS_OK)
   {
      if (cmd[0] != '\0')
      {
         char *classInterface=NULL;

         if (cmsUtl_strcmp(cmd, MDMVS_LOCAL) == 0 ||
             cmsUtl_strcmp(cmd, MDMVS_LAN)   == 0 ||
             cmsUtl_strcmp(cmd, MDMVS_WAN)   == 0)
         {
            cmsMem_free(cObj->classInterface);
            cObj->classInterface = cmsMem_strdup(cmd);
         }
         else
         {
            /* convert L2 or L3 intfName to fullPath */
            ret = qdmIntf_intfnameToFullPathLocked(cmd, FALSE, &classInterface);
            if (ret != CMSRET_SUCCESS)
            {
               ret = qdmIntf_intfnameToFullPathLocked(cmd, TRUE, &classInterface);
            }

            if (ret != CMSRET_SUCCESS)
            {
               cmsLog_error("Could not convert %s to L2 or L3 fullPath", cmd);
            }
            else
            {
               /* strip the ending '.' */
               classInterface[strlen(classInterface)-1] = '\0';
               cmsMem_free(cObj->classInterface);
               cObj->classInterface = classInterface;
            }
         }
      }
   }

   /* SourceMACAddress and SourceMACMask */
   if ((cgiGetValueByName(query, "srcMacAddr", cmd)) == CGI_STS_OK)
   {
      cmsMem_free(cObj->sourceMACAddress);
      cObj->sourceMACAddress = cmsMem_strdup(cmd);
      if ((cgiGetValueByName(query, "srcMacMask", cmd)) == CGI_STS_OK)
      {
         cmsMem_free(cObj->sourceMACMask);
         cObj->sourceMACMask = cmsMem_strdup(cmd);
      }
   }

   /* DestMACAddress and DestMACMask */
   if ((cgiGetValueByName(query, "dstMacAddr", cmd)) == CGI_STS_OK)
   {
      cmsMem_free(cObj->destMACAddress);
      cObj->destMACAddress = cmsMem_strdup(cmd);
      if ((cgiGetValueByName(query, "dstMacMask", cmd)) == CGI_STS_OK)
      {
         cmsMem_free(cObj->destMACMask);
         cObj->destMACMask = cmsMem_strdup(cmd);
      }
   }
    
   /* check to see if the classification already exists */
   if ((ret = dalQos_duplicateClassCheck(cObj, &isDuplicate)) != CMSRET_SUCCESS)
   {
      cmsLog_error("dalQos_duplicateClassCheck returns error. ret=%d", ret);
      sprintf(cmd, "Duplicate check failed. ret=%d", ret);
      cgiWriteMessagePage(fs, "QoS Classification Config Error", cmd, "qoscls.cmd?action=viewcfg");
      cmsObj_free((void **)&cObj);
      return;
   }
   if (isDuplicate)
   {
      sprintf(cmd, "Classification already exists.");
      cmsLog_error(cmd);
      cgiWriteMessagePage(fs, "QoS Classification Config Error", cmd, "qoscls.cmd?action=viewcfg");
      cmsObj_free((void **)&cObj);
      return;
   }

   if (cObj->classQueue > 0)
   {
      UBOOL8 enabled=FALSE;

      ret = qdmQos_getQueueInfoByClassQueueLocked(cObj->classQueue,
                                                  &enabled, NULL, NULL);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("qdmQos_getQueueInfoByClassQueueLocked returns error. ret=%d", ret);
         sprintf(cmd, "Get class queue info failed. ret=%d", ret);
         cgiWriteMessagePage(fs, "QoS Classification Config Error", cmd, "qoscls.cmd?action=viewcfg");
         cmsObj_free((void **)&cObj);
         return;
      }

      if (cObj->classificationEnable && !enabled)
      {
         sprintf(cmd, "Class queue is not enabled.");
         cmsLog_error(cmd);
         cgiWriteMessagePage(fs, "QoS Classification Config Error", cmd, "qoscls.cmd?action=viewcfg");
         cmsObj_free((void **)&cObj);
         return;
      }
   }

#ifdef SUPPORT_POLICING
   if (cObj->classPolicer > 0)
   {
      CmsQosPolicerInfo policerInfo;

      if ((ret = qdmQos_getClassPolicerInfoLocked(cObj->classPolicer, &policerInfo)) != CMSRET_SUCCESS)
      {
         cmsLog_error("qdmQos_getClassPolicerInfo returns error. ret=%d", ret);
         sprintf(cmd, "Get class policer info failed. ret=%d", ret);
         cgiWriteMessagePage(fs, "QoS Classification Config Error", cmd, "qoscls.cmd?action=viewcfg");
         cmsObj_free((void **)&cObj);
         return;
      }

      if (cObj->classificationEnable && !policerInfo.enable)
      {
         sprintf(cmd, "Class policer is not enabled.");
         cmsLog_error(cmd);
         cgiWriteMessagePage(fs, "QoS Classification Config Error", cmd, "qoscls.cmd?action=viewcfg");
         cmsObj_free((void **)&cObj);
         return;
      }
   }
#endif

   /* Set this to the configuration */
   cmsLog_debug("Adding new qos classification: name=%s order=%d classQueue=%d egressIntf=%s",
            cObj->X_BROADCOM_COM_ClassName, cObj->classificationOrder,
            cObj->classQueue, cObj->X_BROADCOM_COM_egressInterface);

   INIT_INSTANCE_ID_STACK(&iidStack);
   if ((ret = cmsObj_addInstance(MDMOID_Q_MGMT_CLASSIFICATION, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("cmsObj_addInstance <MDMOID_Q_MGMT_CLASSIFICATION> returns error, ret=%d", ret);
   }
   else
   {
      if ((ret = cmsObj_set((void *)cObj, &iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("cmsObj_set returns error, ret = %d", ret);
         cmsObj_deleteInstance(MDMOID_Q_MGMT_CLASSIFICATION, &iidStack);
      }
   }

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not add Classification obj, ret=%d", ret);
      sprintf(cmd, "Add qos class failed. ret=%d", ret);
      cgiWriteMessagePage(fs, "QoS Classification Config Error", cmd, "qoscls.cmd?action=view");
   }
   else
   {
      /* set flag to save mdm to flash */
      glbSaveConfigNeeded = TRUE;
      /* refresh page */
      cgiQosClsView(fs);
   }

   cmsObj_free((void **)&cObj);

}  /* End of cgiQosClsAdd() */


#ifdef not_supported
void cgiQosClsEdit(char *query __attribute__((unused)),
                   FILE *fs __attribute__((unused)))
{
    char cmd[WEB_BUF_SIZE_MAX];
    void *objValue;
    PBcmCfm_PerfQosCfg_t pQObj;
    BcmCfm_Status status = BcmCfm_Fail;
    int protocol;
    uint32 getIndex;
    struct in_addr srcIpAddr;
    struct in_addr srcIpMask;
    struct in_addr dstIpAddr;
    struct in_addr dstIpMask;

    cmd[0] = '\0';

    getIndex = glbWebVar.qosClsKey;
    // Get the default created object
    if ( (status = BcmCfm_objGet(BCMCFM_OBJ_PERF_QOS_CLS,
                                &objValue, &getIndex)) == BcmCfm_Fail ) {
      sprintf(cmd, "Retrieve qos class entry for index = %lu failed." \
         " Status: %d", getIndex, status);
      cgiWriteMessagePage(fs, "Qos Class Edit Error", cmd,
                          "qoscls.cmd?action=view");
      return;
    }

    pQObj = (PBcmCfm_PerfQosCfg_t)objValue;

    //clsName
    cmd[0] = '\0';
    cgiGetValueByName(query, "clsName", cmd );
    if (cmd[0] != '\0')
        strcpy(pQObj->clsName, cmd);
    
    //status  
    cgiGetValueByName(query, "rulsEnbl", cmd );
    pQObj->status = (BcmCfm_CfgStatus)(atoi(cmd) - 1);

    //rule order
    cgiGetValueByName(query, "rulsOrder", cmd);
    pQObj->order= atoi(cmd);
    if(pQObj->order == 0)
        pQObj->order = -1; //last

    //protocol
   cgiGetValueByName(query, "protocol", cmd);
    protocol = atoi(cmd);
    pQObj->protocol.tcp = 0;
    pQObj->protocol.udp = 0;
    pQObj->protocol.icmp = 0;
    if(protocol == 1)
    {
        pQObj->protocol.udp = TRUE;
        pQObj->protocol.tcp = TRUE;
    }
    else if (protocol == 2)
    {
        pQObj->protocol.tcp = TRUE;
    }
    else if (protocol == 3)
    {
        pQObj->protocol.udp = TRUE; 
    }
    else if (protocol == 4)
    {
        pQObj->protocol.icmp = TRUE;
    }

    //vlan8021p
    cgiGetValueByName(query, "vlan8021p", cmd);
    pQObj->vlan8021p = atoi(cmd);

    //wanVlan8021p
    cgiGetValueByName(query, "wanVlan8021p", cmd);
    pQObj->wanVlan8021p = atoi(cmd);

    //dscp Check
    if ((cgiGetValueByName(query, "dscpCheck", cmd)) != CGI_STS_ERR_FIND)
        pQObj->dscpCheck = atoi(cmd);
    else
        pQObj->dscpCheck = QOS_RESULT_NO_CHANGE;

    //dscp Mark
    if ((cgiGetValueByName(query, "dscpMark", cmd)) != CGI_STS_ERR_FIND)
        pQObj->dscpMark = atoi(cmd);
    else
        pQObj->dscpMark = QOS_RESULT_NO_CHANGE;

    //src port
    if (cgiGetValueByName(query, "srcPort", cmd) != CGI_STS_ERR_FIND)
    {
        BcmDb_getPortRange(cmd, &(pQObj->srcPortStart), &(pQObj->srcPortEnd));
    }
    else
    {
        pQObj->srcPortStart = QOS_DEFAULT_PORT;
        pQObj->dstPortEnd   = QOS_DEFAULT_PORT;   
    }

    //dst port
    if (cgiGetValueByName(query, "dstPort", cmd) != CGI_STS_ERR_FIND)
    {
        BcmDb_getPortRange(cmd, &(pQObj->dstPortStart), &(pQObj->dstPortEnd)); 
    }
    else
    {   
        pQObj->dstPortStart = QOS_DEFAULT_PORT;
        pQObj->dstPortEnd   = QOS_DEFAULT_PORT;
    }


    //src ipaddr/mask
   cgiGetValueByName(query, "ipoptionlist", cmd);
   pQObj->ipoptionlist = atoi(cmd); 

   if( pQObj->ipoptionlist ){
        cgiGetValueByName(query, "srcAddr", cmd);
        strcpy(pQObj->optionval, cmd); 
    }else{
    if ((cgiGetValueByName(query, "srcAddr", cmd)) == CGI_STS_ERR_FIND)
        srcIpAddr.s_addr = 0;
    else
        inet_aton(cmd, &srcIpAddr);
    pQObj->srcAddr = srcIpAddr.s_addr;
    }
    if ((cgiGetValueByName(query, "srcMask", cmd)) == CGI_STS_ERR_FIND)
        srcIpMask.s_addr = 0;
    else
        inet_aton(cmd, &srcIpMask);
    pQObj->srcMask = srcIpMask.s_addr;

    //dst ipaddr/mask
    if ((cgiGetValueByName(query, "dstAddr", cmd)) == CGI_STS_ERR_FIND)
        dstIpAddr.s_addr = 0;
    else
        inet_aton(cmd, &dstIpAddr);
    pQObj->dstAddr = dstIpAddr.s_addr;

    if ((cgiGetValueByName(query, "dstMask", cmd)) == CGI_STS_ERR_FIND)
        dstIpMask.s_addr = 0;
    else
        inet_aton(cmd, &dstIpMask);
    pQObj->dstMask = dstIpMask.s_addr;   

    //protExclude
    if(cgiGetValueByName(query, "protExclude", cmd) != CGI_STS_ERR_FIND)
        pQObj->protExclude= atoi(cmd);
    else
        pQObj->protExclude = 0;

    //srcIpExclude
    if(cgiGetValueByName(query, "srcIpExclude", cmd) != CGI_STS_ERR_FIND)
        pQObj->srcIpExclude = atoi(cmd);
    else
        pQObj->srcIpExclude = 0;

    //destportExclude
    if(cgiGetValueByName(query, "destIpExclude", cmd) != CGI_STS_ERR_FIND)
        pQObj->destIpExclude = atoi(cmd);
    else
        pQObj->destIpExclude = 0;

    //srcPortExclude
    if(cgiGetValueByName(query, "srcPortExclude", cmd) != CGI_STS_ERR_FIND)
        pQObj->srcPortExclude = atoi(cmd);
    else
        pQObj->srcPortExclude = 0;

    //destportExclude
    if(cgiGetValueByName(query, "destportExclude", cmd) != CGI_STS_ERR_FIND)
        pQObj->destportExclude = atoi(cmd);
    else
        pQObj->destportExclude = 0;
    

    //etherPrioExclude
    if(cgiGetValueByName(query, "etherPrioExclude", cmd) != CGI_STS_ERR_FIND)
        pQObj->etherPrioExclude = atoi(cmd);
    else
        pQObj->etherPrioExclude = 0;

    //dscpExclude
    if(cgiGetValueByName(query, "dscpExclude", cmd) != CGI_STS_ERR_FIND)
        pQObj->dscpExclude = atoi(cmd);
    else
        pQObj->dscpExclude = 0;
 
    //qos queue key
    cgiGetValueByName(query, "queuekey", cmd);
    pQObj->clsQueueKey = atoi(cmd);

    //Lan IfcName
    cgiGetValueByName(query, "classIf", pQObj->classIf);

      if ((cgiGetValueByName(query, "srcMacAddr", cmd)) != CGI_STS_ERR_FIND)
        bcmMacStrToNum(pQObj->srcMacAddr, cmd);
      else
        strcpy(pQObj->srcMacAddr, WEB_SPACE);

      if ((cgiGetValueByName(query, "dstMacAddr", cmd)) != CGI_STS_ERR_FIND)
        bcmMacStrToNum(pQObj->dstMacAddr, cmd);
      else
        strcpy(pQObj->dstMacAddr, WEB_SPACE);
      if ((cgiGetValueByName(query, "srcMacMask", cmd)) != CGI_STS_ERR_FIND)
        bcmMacStrToNum(pQObj->srcMacMask, cmd);
      else
        strcpy(pQObj->srcMacMask, WEB_SPACE);
      if ((cgiGetValueByName(query, "dstMacMask", cmd)) != CGI_STS_ERR_FIND)
        bcmMacStrToNum(pQObj->dstMacMask, cmd);
      else
        strcpy(pQObj->dstMacMask, WEB_SPACE);
    
    // Set this to the configuration
    if ( (status = BcmCfm_objSet(BCMCFM_OBJ_PERF_QOS_CLS,
                                objValue, getIndex)) == BcmCfm_Fail ) {
     sprintf(cmd, "Configure qos class for %s failed." \
             " Status: %d", pQObj->clsName, status);
     cgiWriteMessagePage(fs, "Queue Config Edit Error", cmd,
                         "qoscls.cmd?action=view");
     return;
    }
    
    cgiQosClsView(fs);
    BcmPsi_flush();
}  /* End of cgiQosClsEdit() */

#endif  /* not_supported */


void cgiQosClsRemove_igd(char *query, FILE *fs)
{
   char cmd[WEB_MD_BUF_SIZE_MAX];
   char lst[WEB_BUF_SIZE_MAX];
   char *last = NULL;
   InstanceIdStack iidStack, iidStackPrev;
   QMgmtClassificationObject *cObj = NULL;
   CmsRet ret;

   lst[0] = '\0';
   if (cgiGetValueByName(query, "rmLst", lst) != CGI_STS_OK)
   {
      return;
   }

   /* loop through the classification table to find the instances to delete */
   INIT_INSTANCE_ID_STACK(&iidStack);
   iidStackPrev = iidStack;
   while ((ret = cmsObj_getNext(MDMOID_Q_MGMT_CLASSIFICATION, &iidStack, (void **)&cObj)) == CMSRET_SUCCESS)
   {
      char *check;

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
         cmsObj_free((void **)&cObj);
         break;   /* done */
      }

      if (atoi(check) == 1)
      {
         /* delete this instance */
         cmsLog_debug("deleting classification: name=%s order=%d classQueue=%d egressIntf=%s",
                  cObj->X_BROADCOM_COM_ClassName, cObj->classificationOrder,
                  cObj->classQueue, cObj->X_BROADCOM_COM_egressInterface);
         if ((ret = cmsObj_deleteInstance(MDMOID_Q_MGMT_CLASSIFICATION, &iidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("cmsObj_deleteInstance returns error, ret=%d", ret);
            sprintf(cmd, "Delete qos class failed. ret=%d", ret);
            cgiWriteMessagePage(fs, "QoS Classification Config Error", cmd, "qoscls.cmd?action=view");
            cmsObj_free((void **)&cObj);
            break;
         }
         glbSaveConfigNeeded = TRUE;

         /* since this instance has been deleted, we want to set the iidStack to
          * the previous instance, so that we can continue to do getNext.
          */
         iidStack = iidStackPrev;
      }
      cmsObj_free((void **)&cObj);

      /* save this iidStack in case we want to do a getNext from this instance */
      iidStackPrev = iidStack;
   }
}  /* End of cgiQosClsRemove() */

void cgiQosClsEnable_igd(char *query, FILE *fs)
{
   char cmd[WEB_MD_BUF_SIZE_MAX];
   char lst[WEB_BUF_SIZE_MAX];
   char *last = NULL;
   char *check;
   InstanceIdStack iidStack;
   QMgmtClassificationObject *cObj = NULL;
   CmsRet ret;

   lst[0] = '\0';
   if (cgiGetValueByName( query, "eblLst", lst ) != CGI_STS_OK)
   {
      return;
   }
   /* loop through the classification table */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_Q_MGMT_CLASSIFICATION, &iidStack, (void **)&cObj)) == CMSRET_SUCCESS)
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
         cmsObj_free((void **)&cObj);
         break;   /* done */
      }
      
      if (atoi(check) == 1)
      {
         if (!cObj->classificationEnable)
         {
            cObj->classificationEnable = TRUE;
            if ((ret = cmsObj_set(cObj, &iidStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("cmsObj_set returns error. ret=%d", ret);
               sprintf(cmd, "Enable qos class failed. ret=%d", ret);
               cgiWriteMessagePage(fs, "QoS Classification Config Error", cmd, "qoscls.cmd?action=view");
               cmsObj_free((void **)&cObj);
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
         if (cObj->classificationEnable)
         {
            cObj->classificationEnable = FALSE;
            if ((ret = cmsObj_set(cObj, &iidStack)) != CMSRET_SUCCESS)
            {
               cmsLog_error("cmsObj_set returns error. ret=%d", ret);
               sprintf(cmd, "Enable qos class failed. ret=%d", ret);
               cgiWriteMessagePage(fs, "QoS Classification Config Error", cmd, "qoscls.cmd?action=view");
               cmsObj_free((void **)&cObj);
               break;
            }
            else
            {
               glbSaveConfigNeeded = TRUE;
            }
         }
      }

      cmsObj_free((void **)&cObj);
   }
}  /* End of cgiQosClsEnable() */ 

void cgiGetQosClsRulsOrder_igd(int argc __attribute__((unused)),
                           char **argv __attribute__((unused)),
                           char *varValue)
{
   InstanceIdStack iidStack;
   QMgmtClassificationObject *qObj = NULL;
   CmsRet ret;

   varValue[0] = '\0';

   /* loop through the queue table */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_Q_MGMT_CLASSIFICATION, &iidStack, (void **)&qObj)) == CMSRET_SUCCESS)
   {
      char order[BUFLEN_16];

      sprintf(order, "%d|", qObj->classificationOrder);
      strcat(varValue, order);
      cmsObj_free((void **)&qObj);
   }

   strcat(varValue, "Last");

}  /* End of cgiGetQosClsRulsOrder() */

#endif  /* DMP_QOS_1 */


void cgiGetQosClsRulsOrder(int argc, char **argv, char *varValue)
{
#if defined(SUPPORT_DM_LEGACY98)

   cgiGetQosClsRulsOrder_igd(argc, argv, varValue);

#elif defined(SUPPORT_DM_HYBRID)

   cgiGetQosClsRulsOrder_igd(argc, argv, varValue);

#elif defined(SUPPORT_DM_PURE181)

   cgiGetQosClsRulsOrder_dev2(argc, argv, varValue);

#elif defined(SUPPORT_DM_DETECT)

   if (cmsMdm_isDataModelDevice2())
   {
      cgiGetQosClsRulsOrder_dev2(argc, argv, varValue);
   }
   else
   {
      cgiGetQosClsRulsOrder_dev2(argc, argv, varValue);
   }

#endif
}


#endif  /* SUPPORT_QOS */


/***********************************************************************
 *
 *  Copyright (c) 2007  Broadcom Corporation
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
#ifdef SUPPORT_IPV6

#include "cms.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "cgi_cmd.h"
#include "cgi_ntwk.h"
#include "cgi_iptunnel.h"
#include "cgi_main.h"
#include "httpd.h"

#include "cms_util.h"
#include "cms_core.h"
#include "cms_dal.h"
#include "cms_qdm.h"

extern WEB_NTWK_VAR glbWebVar; 

#define MAX_6IN4_ENTRY 1
#define MAX_4IN6_ENTRY 1
#define MAX_MAPT_ENTRY 1

void write4in6TunnelScript(FILE *fs) 
{
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   fprintf(fs, "function addClick() {\n");
   fprintf(fs, "   var loc = 'tunnel4in6.html';\n\n");
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
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

   fprintf(fs, "   var loc = 'tunnelcfg.cmd?action=remove4in6&rmLst=' + lst;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");
   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");
}


void write6in4TunnelScript(FILE *fs) 
{
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   fprintf(fs, "function addClick() {\n");
   fprintf(fs, "   var loc = 'tunnel6in4.html';\n\n");
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
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

   fprintf(fs, "   var loc = 'tunnelcfg.cmd?action=remove6in4&rmLst=' + lst;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");
   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");
}


#ifdef SUPPORT_MAPT
void writeMaptScript(FILE *fs) 
{
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   fprintf(fs, "function addClick() {\n");
   fprintf(fs, "   var loc = 'mapt.html';\n\n");
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
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

   fprintf(fs, "   var loc = 'tunnelcfg.cmd?action=removemapt&rmLst=' + lst;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");
   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");
}
#endif

void cgi6in4TunnelView(FILE *fs) 
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
#ifdef DMP_X_BROADCOM_COM_IPV6_1 /* aka SUPPORT_IPV6 */
   void *obj = NULL;
   CmsRet ret = CMSRET_SUCCESS;
#endif
#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
   Dev2Ipv6rdInterfaceSettingObject *ipv6rdObj = NULL;
#endif
   
   /* write header */
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");

   /* write Java Script */
   write6in4TunnelScript(fs);

   /* write body */
   fprintf(fs, "</head>\n<body>\n<blockquote>\n<form>\n");

   fprintf(fs, "<b>IP Tunneling -- 6in4 Tunnel Configuration</b><br><br>\n");
   fprintf(fs, "<center>\n");
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");
   /* write table header */
   fprintf(fs, "   <tr>\n");
   fprintf(fs, "      <td class='hd'>Name</td>\n");
   fprintf(fs, "      <td class='hd'>WAN</td>\n");
   fprintf(fs, "      <td class='hd'>LAN</td>\n");
   fprintf(fs, "      <td class='hd'>Dynamic</td>\n");
   fprintf(fs, "      <td class='hd'>IPv4 Mask Length</td>\n");
   fprintf(fs, "      <td class='hd'>6rd Prefix</td>\n");
   fprintf(fs, "      <td class='hd'>Border Relay Address</td>\n");
   fprintf(fs, "      <td class='hd'>Remove</td>\n");
   fprintf(fs, "   </tr>\n");

   glbEntryCt = 0;
   
#ifdef DMP_X_BROADCOM_COM_IPV6_1 /* aka SUPPORT_IPV6 */
   while ((ret = cmsObj_getNextFlags(MDMOID_IP_TUNNEL, &iidStack, OGF_NO_VALUE_UPDATE, &obj)) == CMSRET_SUCCESS)
   {
      IPTunnelObject *tunnelCfg = (IPTunnelObject *)obj;
      Ipv6inIpv4TunnelObject *ipv6in4Obj;

      if ( !cmsUtl_strcmp(tunnelCfg->mode, MDMVS_IPV6INIPV4) )
      {
         InstanceIdStack iidStack1 = EMPTY_INSTANCE_ID_STACK;

         if ( (ret = cmsObj_getNextInSubTreeFlags(MDMOID_IPV6IN_IPV4_TUNNEL, &iidStack, &iidStack1,
                                                                              OGF_NO_VALUE_UPDATE, (void **)&ipv6in4Obj) != CMSRET_SUCCESS ) )
         {
            cmsLog_error("Failed at getting 6in4 object!");
            cmsObj_free((void **) &tunnelCfg);
            return;
         }

         fprintf(fs, "   <tr>\n");
         fprintf(fs, "      <td>%s</td>\n", tunnelCfg->tunnelName);

         if (IS_EMPTY_STRING(tunnelCfg->associatedWanIfName))
         {
            fprintf(fs, "      <td>&nbsp</td>\n");
         }
         else
         {
            fprintf(fs, "      <td>%s</td>\n", tunnelCfg->associatedWanIfName);
         }

         if (IS_EMPTY_STRING(tunnelCfg->associatedLanIfName))
         {
            fprintf(fs, "      <td>&nbsp</td>\n");
         }
         else
         {
            fprintf(fs, "      <td>%s</td>\n", tunnelCfg->associatedLanIfName);
         }

         if ( ipv6in4Obj->dynamic )
         {
            fprintf(fs, "      <td>Dynamic</td>\n");
         }
         else
         {
            fprintf(fs, "      <td>Static</td>\n");
         }

         fprintf(fs, "      <td>%d</td>\n", ipv6in4Obj->ipv4MaskLen);

         if (IS_EMPTY_STRING(ipv6in4Obj->prefix))
         {
            fprintf(fs, "      <td>&nbsp</td>\n");
         }
         else
         {
            fprintf(fs, "      <td>%s</td>\n", ipv6in4Obj->prefix);
         }

         if (IS_EMPTY_STRING(ipv6in4Obj->borderRelayAddress))
         {
            fprintf(fs, "      <td>&nbsp</td>\n");
         }
         else
         {
            fprintf(fs, "      <td>%s</td>\n", ipv6in4Obj->borderRelayAddress);
         }

         fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s'></td>\n",
                 tunnelCfg->tunnelName);

         fprintf(fs, "   </tr>\n");

         cmsObj_free((void **) &ipv6in4Obj);
         glbEntryCt++;
      }

      cmsObj_free((void **) &tunnelCfg);
   }
#endif
#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
   while (cmsObj_getNextFlags(MDMOID_DEV2_IPV6RD_INTERFACE_SETTING, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&ipv6rdObj) == CMSRET_SUCCESS)
   {
      fprintf(fs, "   <tr>\n");
      fprintf(fs, "      <td>%s</td>\n", ipv6rdObj->X_BROADCOM_COM_TunnelName);

      if (IS_EMPTY_STRING(ipv6rdObj->X_BROADCOM_COM_TunneledInterface))
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }
      else
      {
         char ifname[CMS_IFNAME_LENGTH] = {0};

         qdmIntf_getIntfnameFromFullPathLocked_dev2(ipv6rdObj->X_BROADCOM_COM_TunneledInterface, ifname, sizeof(ifname));
         fprintf(fs, "      <td>%s</td>\n", ifname);
      }

      if (IS_EMPTY_STRING(ipv6rdObj->X_BROADCOM_COM_TunnelInterface))
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }
      else
      {
         char ifname[CMS_IFNAME_LENGTH] = {0};

         qdmIntf_getIntfnameFromFullPathLocked_dev2(ipv6rdObj->X_BROADCOM_COM_TunnelInterface, ifname, sizeof(ifname));
         fprintf(fs, "      <td>%s</td>\n", ifname);
      }

      if ( ipv6rdObj->X_BROADCOM_COM_Dynamic )
      {
         fprintf(fs, "      <td>Dynamic</td>\n");
      }
      else
      {
         fprintf(fs, "      <td>Static</td>\n");
      }

      fprintf(fs, "      <td>%d</td>\n", ipv6rdObj->IPv4MaskLength);

      if (IS_EMPTY_STRING(ipv6rdObj->SPIPv6Prefix))
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }
      else
      {
         fprintf(fs, "      <td>%s</td>\n", ipv6rdObj->SPIPv6Prefix);
      }

      if (IS_EMPTY_STRING(ipv6rdObj->borderRelayIPv4Addresses))
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }
      else
      {
         fprintf(fs, "      <td>%s</td>\n", ipv6rdObj->borderRelayIPv4Addresses);
      }

      fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s'></td>\n",
              ipv6rdObj->X_BROADCOM_COM_TunnelName);

      fprintf(fs, "   </tr>\n");

      cmsObj_free((void **) &ipv6rdObj);
      glbEntryCt++;
   }
#endif

   fprintf(fs, "</table><br>\n");

   if (glbEntryCt < MAX_6IN4_ENTRY) 
   {
      fprintf(fs, "<input type='button' onClick='addClick()' value='Add'>\n");
   }
   fprintf(fs, "<input type='button' onClick='removeClick(this.form.rml)' value='Remove'>\n");

   fprintf(fs, "</center>\n");
   fprintf(fs, "</form>\n");
   fprintf(fs, "</blockquote>\n");
   fprintf(fs, "</body>\n");
   fprintf(fs, "</html>\n");
   fflush(fs);

}

void cgi4in6TunnelView(FILE *fs) 
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
#ifdef DMP_X_BROADCOM_COM_IPV6_1
   CmsRet ret = CMSRET_SUCCESS;
   void *obj = NULL;
#endif
#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
   Dev2DsliteInterfaceSettingObject *dsLiteObj = NULL;
#endif
   
   /* write header */
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");

   /* write Java Script */
   write4in6TunnelScript(fs);

   /* write body */
   fprintf(fs, "</head>\n<body>\n<blockquote>\n<form>\n");

   fprintf(fs, "<b>IP Tunneling -- 4in6 Tunnel Configuration</b><br><br>\n");
   fprintf(fs, "<center>\n");
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");
   /* write table header */
   fprintf(fs, "   <tr>\n");
   fprintf(fs, "      <td class='hd'>Name</td>\n");
   fprintf(fs, "      <td class='hd'>WAN</td>\n");
   fprintf(fs, "      <td class='hd'>LAN</td>\n");
   fprintf(fs, "      <td class='hd'>Dynamic</td>\n");
   fprintf(fs, "      <td class='hd'>AFTR</td>\n");
   fprintf(fs, "      <td class='hd'>Remove</td>\n");
   fprintf(fs, "   </tr>\n");

   glbEntryCt = 0;

#ifdef DMP_X_BROADCOM_COM_IPV6_1 /* aka SUPPORT_IPV6 */
   while ((ret = cmsObj_getNextFlags(MDMOID_IP_TUNNEL, &iidStack, OGF_NO_VALUE_UPDATE, &obj)) == CMSRET_SUCCESS)
   {
      IPTunnelObject *tunnelCfg = (IPTunnelObject *)obj;
      Ipv4inIpv6TunnelObject *ipv4in6Obj;

      if ( !cmsUtl_strcmp(tunnelCfg->mode, MDMVS_IPV4INIPV6) )
      {
         InstanceIdStack iidStack1 = EMPTY_INSTANCE_ID_STACK;

         if ( (ret = cmsObj_getNextInSubTreeFlags(MDMOID_IPV4IN_IPV6_TUNNEL, &iidStack, &iidStack1,
                                                                              OGF_NO_VALUE_UPDATE, (void **)&ipv4in6Obj) != CMSRET_SUCCESS ) )
         {
            cmsLog_error("Failed at getting 4in6 object!");
            cmsObj_free((void **) &tunnelCfg);
            return;
         }

         fprintf(fs, "   <tr>\n");
         fprintf(fs, "      <td>%s</td>\n", tunnelCfg->tunnelName);

         if (IS_EMPTY_STRING(tunnelCfg->associatedWanIfName))
         {
            fprintf(fs, "      <td>&nbsp</td>\n");
         }
         else
         {
            fprintf(fs, "      <td>%s</td>\n", tunnelCfg->associatedWanIfName);
         }

         if (IS_EMPTY_STRING(tunnelCfg->associatedLanIfName))
         {
            fprintf(fs, "      <td>&nbsp</td>\n");
         }
         else
         {
            fprintf(fs, "      <td>%s</td>\n", tunnelCfg->associatedLanIfName);
         }

         if ( ipv4in6Obj->dynamic )
         {
            fprintf(fs, "      <td>Dynamic</td>\n");
         }
         else
         {
            fprintf(fs, "      <td>Static</td>\n");
         }

         if (IS_EMPTY_STRING(ipv4in6Obj->remoteIpv6Address))
         {
            fprintf(fs, "      <td>&nbsp</td>\n");
         }
         else
         {
            fprintf(fs, "      <td>%s</td>\n", ipv4in6Obj->remoteIpv6Address);
         }

         fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s'></td>\n",
                 tunnelCfg->tunnelName);

         fprintf(fs, "   </tr>\n");

         cmsObj_free((void **) &ipv4in6Obj);
         glbEntryCt++;
      }

      cmsObj_free((void **) &tunnelCfg);
   }
#endif
#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
   while (cmsObj_getNextFlags(MDMOID_DEV2_DSLITE_INTERFACE_SETTING, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&dsLiteObj) == CMSRET_SUCCESS)
   {
      fprintf(fs, "   <tr>\n");
      fprintf(fs, "      <td>%s</td>\n", dsLiteObj->X_BROADCOM_COM_TunnelName);

      if (IS_EMPTY_STRING(dsLiteObj->X_BROADCOM_COM_TunneledInterface))
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }
      else
      {
         char ifname[CMS_IFNAME_LENGTH] = {0};

         qdmIntf_getIntfnameFromFullPathLocked_dev2(dsLiteObj->X_BROADCOM_COM_TunneledInterface, ifname, sizeof(ifname));
         fprintf(fs, "      <td>%s</td>\n", ifname);
      }

      if (IS_EMPTY_STRING(dsLiteObj->X_BROADCOM_COM_TunnelInterface))
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }
      else
      {
         char ifname[CMS_IFNAME_LENGTH] = {0};

         qdmIntf_getIntfnameFromFullPathLocked_dev2(dsLiteObj->X_BROADCOM_COM_TunnelInterface, ifname, sizeof(ifname));
         fprintf(fs, "      <td>%s</td>\n", ifname);
      }

      if ( cmsUtl_strcmp(dsLiteObj->origin, MDMVS_STATIC) )
      {
         fprintf(fs, "      <td>Dynamic</td>\n");
      }
      else
      {
         fprintf(fs, "      <td>Static</td>\n");
      }

      if (IS_EMPTY_STRING(dsLiteObj->endpointAddressInUse))
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }
      else
      {
         fprintf(fs, "      <td>%s</td>\n", dsLiteObj->endpointAddressInUse);
      }

      fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s'></td>\n",
              dsLiteObj->X_BROADCOM_COM_TunnelName);

      fprintf(fs, "   </tr>\n");

      cmsObj_free((void **) &dsLiteObj);
      glbEntryCt++;
   }
#endif

   fprintf(fs, "</table><br>\n");

   if (glbEntryCt < MAX_4IN6_ENTRY) 
   {
      fprintf(fs, "<input type='button' onClick='addClick()' value='Add'>\n");
   }
   fprintf(fs, "<input type='button' onClick='removeClick(this.form.rml)' value='Remove'>\n");

   fprintf(fs, "</center>\n");
   fprintf(fs, "</form>\n");
   fprintf(fs, "</blockquote>\n");
   fprintf(fs, "</body>\n");
   fprintf(fs, "</html>\n");
   fflush(fs);

}

#ifdef SUPPORT_MAPT
void cgiMaptView(FILE *fs) 
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2MapDomainObject *domain = NULL;
   
   /* write header */
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");

   /* write Java Script */
   writeMaptScript(fs);

   /* write body */
   fprintf(fs, "</head>\n<body>\n<blockquote>\n<form>\n");

   fprintf(fs, "<b>MAP-T -- MAP-T Configuration</b><br><br>\n");
   fprintf(fs, "<center>\n");
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");
   /* write table header */
   fprintf(fs, "   <tr>\n");
   fprintf(fs, "      <td class='hd'>WAN</td>\n");
   fprintf(fs, "      <td class='hd'>Dynamic</td>\n");
   fprintf(fs, "      <td class='hd'>BR Prefix</td>\n");
   fprintf(fs, "      <td class='hd'>BMR IPv6 Prefix</td>\n");
   fprintf(fs, "      <td class='hd'>BMR IPv4 Prefix</td>\n");
   fprintf(fs, "      <td class='hd'>PSID Offset</td>\n");
   fprintf(fs, "      <td class='hd'>PSID Length</td>\n");
   fprintf(fs, "      <td class='hd'>PSID</td>\n");
   fprintf(fs, "      <td class='hd'>Remove</td>\n");
   fprintf(fs, "   </tr>\n");

   glbEntryCt = 0;

   while (cmsObj_getNextFlags(MDMOID_DEV2_MAP_DOMAIN, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&domain) == CMSRET_SUCCESS)
   {
      InstanceIdStack iidStack_rule = EMPTY_INSTANCE_ID_STACK;
      Dev2MapDomainRuleObject *rule = NULL;

      if (cmsObj_getNextInSubTreeFlags(MDMOID_DEV2_MAP_DOMAIN_RULE,
                           &iidStack, &iidStack_rule,
                           OGF_NO_VALUE_UPDATE,
                           (void **)&rule) != CMSRET_SUCCESS)
      {
         cmsLog_notice("No rule obj associated with domain!");
         cmsObj_free((void **) &domain);
         break;
      }

      fprintf(fs, "   <tr>\n");

      if (IS_EMPTY_STRING(domain->WANInterface))
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }
      else
      {
         char ifname[CMS_IFNAME_LENGTH] = {0};

         qdmIntf_getIntfnameFromFullPathLocked_dev2(domain->WANInterface, ifname, sizeof(ifname));
         fprintf(fs, "      <td>%s</td>\n", ifname);
      }

      if ( cmsUtl_strcmp(rule->origin, MDMVS_STATIC) )
      {
         fprintf(fs, "      <td>Dynamic</td>\n");
      }
      else
      {
         fprintf(fs, "      <td>Static</td>\n");
      }

      if (IS_EMPTY_STRING(domain->BRIPv6Prefix))
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }
      else
      {
         fprintf(fs, "      <td>%s</td>\n", domain->BRIPv6Prefix);
      }

      if (IS_EMPTY_STRING(rule->IPv6Prefix))
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }
      else
      {
         fprintf(fs, "      <td>%s</td>\n", rule->IPv6Prefix);
      }

      if (IS_EMPTY_STRING(rule->IPv4Prefix))
      {
         fprintf(fs, "      <td>&nbsp</td>\n");
      }
      else
      {
         fprintf(fs, "      <td>%s</td>\n", rule->IPv4Prefix);
      }

      fprintf(fs, "      <td>%d</td>\n", domain->PSIDOffset);
      fprintf(fs, "      <td>%d</td>\n", domain->PSIDLength);
      fprintf(fs, "      <td>%d</td>\n", domain->PSID);

      fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s'></td>\n",
              domain->WANInterface);

      fprintf(fs, "   </tr>\n");

      cmsObj_free((void **) &domain);
      cmsObj_free((void **) &domain);
      glbEntryCt++;
   }

   fprintf(fs, "</table><br>\n");

   if (glbEntryCt < MAX_MAPT_ENTRY) 
   {
      fprintf(fs, "<input type='button' onClick='addClick()' value='Add'>\n");
   }
   fprintf(fs, "<input type='button' onClick='removeClick(this.form.rml)' value='Remove'>\n");

   fprintf(fs, "</center>\n");
   fprintf(fs, "</form>\n");
   fprintf(fs, "</blockquote>\n");
   fprintf(fs, "</body>\n");
   fprintf(fs, "</html>\n");
   fflush(fs);
}
#endif

void cgiIpTunnelCfg(char *query, FILE *fs) 
{
   char action[BUFLEN_256];
   cmsLog_debug("query = %s", query);

   cgiGetValueByName(query, "action", action);

   cmsLog_debug("action = %s", action);

   if ( strcmp(action, "add6in4") == 0 )
   {
      cgi6in4TunnelAdd(query, fs);
   }
   else if ( strcmp(action, "remove6in4") == 0 )
   {
      cgi6in4TunnelRemove(query, fs);
   }
   else if ( strcmp(action, "viewcfg") == 0 )
   {
      cgi6in4TunnelView(fs);
   }
   else if ( strcmp(action, "add4in6") == 0 )
   {
      cgi4in6TunnelAdd(query, fs);
   }
   else if ( strcmp(action, "remove4in6") == 0 )
   {
      cgi4in6TunnelRemove(query, fs);
   }
   else if ( strcmp(action, "view") == 0 )
   {
      cgi4in6TunnelView(fs);
   }
#ifdef SUPPORT_MAPT
   else if ( strcmp(action, "addmapt") == 0 )
   {
      cgiMaptAdd(query, fs);
   }
   else if ( strcmp(action, "removemapt") == 0 )
   {
      cgiMaptRemove(query, fs);
   }
   else if ( strcmp(action, "viewmapt") == 0 )
   {
      cgiMaptView(fs);
   }
#endif
}

void cgi6in4TunnelAdd(char *query, FILE *fs) 
{
   CmsRet ret = CMSRET_SUCCESS;
   char tunnelName[BUFLEN_32];
   char mechanism[BUFLEN_24];
   char wanIntf[BUFLEN_32];
   char lanIntf[BUFLEN_32];
   char dynamicInfo[BUFLEN_4];
   UBOOL8 dynamic;
   char cmd[WEB_BUF_SIZE_MAX];

   tunnelName[0] = mechanism[0] = wanIntf[0] = lanIntf[0] = dynamicInfo[0] = cmd[0] = '\0';

   cgiGetValueByName(query, "6in4TunnelName", tunnelName);
   cgiGetValueByName(query, "6in4Mechanism", mechanism);
   cgiGetValueByName(query, "associatedWanIntf", wanIntf);
   cgiGetValueByName(query, "associatedLanIntf", lanIntf);
   cgiGetValueByName(query, "dynamic", dynamicInfo);

   cmsLog_debug("tunnelName=%s mechanism=%s, wanIntf=%s, lanIntf=%s dynamicInfo=%s", 
                             tunnelName, mechanism, wanIntf, lanIntf, dynamicInfo);

   dynamic = atoi(dynamicInfo);
   if ( dynamic == 0 )
   {
      char ipv6rdPrefix[CMS_IPADDR_LENGTH];
      char ipv4MaskLen[BUFLEN_4];
      char brAddr[CMS_IPADDR_LENGTH];
      SINT8 masklen;

      cgiGetValueByName(query, "6rdPrefix", ipv6rdPrefix);
      cgiGetValueByName(query, "ipv4MaskLen", ipv4MaskLen);
      cgiGetValueByName(query, "brAddr", brAddr);

      cmsLog_debug("6rdPrefix=%s ipv4MaskLen=%s, brAddr=%s", ipv6rdPrefix, ipv4MaskLen, brAddr);

      masklen = atoi(ipv4MaskLen);
      if (cmsUtl_isValidIpAddress(AF_INET, brAddr) == FALSE || !strcmp(brAddr, "0.0.0.0"))
      {
         cmsLog_error("Invalid border relay IP address");
         sprintf(cmd, "6in4 tunnel with invalid BRAddress = %s failed", brAddr);
         cgiWriteMessagePage(fs, "6in4 Tunnel Add Error", cmd, "tunnel6in4.html");
         return;
      }

      if ( (masklen < 0) ||(masklen > 32) )
      {
         cmsLog_error("Invalid IPv4 mask length");
         sprintf(cmd, "6in4 tunnel with invalid IPv4 mask len = %s failed", ipv4MaskLen);
         cgiWriteMessagePage(fs, "6in4 Tunnel Add Error", cmd, "tunnel6in4.html");
         return;
      }

      if (cmsUtl_isValidIpAddress(AF_INET6, ipv6rdPrefix) == FALSE)
      {
         cmsLog_error("Invalid border relay IP address");
         sprintf(cmd, "6in4 tunnel with invalid 6rd prefix = %s failed", brAddr);
         cgiWriteMessagePage(fs, "6in4 Tunnel Add Error", cmd, "tunnel6in4.html");
         return;
      }

      if ((ret = dal6in4Tunnel_add(tunnelName, mechanism, wanIntf, lanIntf, dynamic, ipv6rdPrefix, masklen, brAddr)) != CMSRET_SUCCESS) 
      {
         cmsLog_error("dal6in4Tunnel_add failed, ret=%d", ret);
         sprintf(cmd, "Adding 6in4 tunnel <%s> failed", tunnelName);
         cgiWriteMessagePage(fs, "6in4 Tunnel Add Error", cmd, "tunnelcfg.cmd?action=viewcfg");
         return;
      }
   }
   else
   {
      if ((ret = dal6in4Tunnel_add(tunnelName, mechanism, wanIntf, lanIntf, dynamic, NULL, 0, NULL)) != CMSRET_SUCCESS) 
      {
         cmsLog_error("dal6in4Tunnel_add failed, ret=%d", ret);
         sprintf(cmd, "Adding 6in4 tunnel <%s> failed", tunnelName);
         cgiWriteMessagePage(fs, "6in4 Tunnel Add Error", cmd, "tunnelcfg.cmd?action=viewcfg");
         return;
      }
   }

   glbSaveConfigNeeded = TRUE;
   cgi6in4TunnelView(fs);
}

void cgi6in4TunnelRemove(char *query, FILE *fs) 
{
   char *pToken = NULL, *pLast = NULL;
   char lst[WEB_BUF_SIZE_MAX], str[BUFLEN_256];
   char tunnelName[BUFLEN_32];
   CmsRet ret = CMSRET_SUCCESS;
   
   tunnelName[0] = '\0'; lst[0] = '\0'; str[0] = '\0';

   cgiGetValueByName(query, "rmLst", lst);
   pToken = strtok_r(lst, ", ", &pLast);
   
   while ( pToken != NULL ) 
   {
      cmsUtl_strncpy(tunnelName, pToken, sizeof(tunnelName));
      if ((ret = dalTunnel_delete(tunnelName, MDMVS_IPV6INIPV4)) != CMSRET_SUCCESS)
      {
         cmsLog_error("dalTunnel_delete failed for %s (ret=%d)", tunnelName, ret);
         sprintf(str, "dalTunnel_delete failed for %s ", tunnelName);
      }

      pToken = strtok_r(NULL, ", ", &pLast);
   }
   
   if ( ret == CMSRET_SUCCESS ) 
   {
      cgi6in4TunnelView(fs);
      glbSaveConfigNeeded = TRUE;
   } else
   {
      cgiWriteMessagePage(fs, "6in4 Tunnel Remove Error", str,
                          "tunnelcfg.cmd?action=viewcfg");
   }

}

void cgi4in6TunnelAdd(char *query, FILE *fs) 
{
   CmsRet ret = CMSRET_SUCCESS;
   char tunnelName[BUFLEN_32];
   char mechanism[BUFLEN_24];
   char wanIntf[BUFLEN_32];
   char lanIntf[BUFLEN_32];
   char dynamicInfo[BUFLEN_4];
   UBOOL8 dynamic;
   char cmd[WEB_BUF_SIZE_MAX];

   tunnelName[0] = mechanism[0] = wanIntf[0] = lanIntf[0] = dynamicInfo[0] = cmd[0] = '\0';

   cgiGetValueByName(query, "4in6TunnelName", tunnelName);
   cgiGetValueByName(query, "4in6mechanism", mechanism);
   cgiGetValueByName(query, "associatedWanIntf", wanIntf);
   cgiGetValueByName(query, "associatedLanIntf", lanIntf);
   cgiGetValueByName(query, "dynamic", dynamicInfo);

   cmsLog_debug("tunnelName=%s mechanism=%s, wanIntf=%s, lanIntf=%s dynamicInfo=%s", 
                             tunnelName, mechanism, wanIntf, lanIntf, dynamicInfo);

   dynamic = atoi(dynamicInfo);
   if ( dynamic == 0 )
   {
      char remoteIp[CMS_AFTR_NAME_LENGTH];

      cgiGetValueByName(query, "remoteIpv6Addr", remoteIp);
      cmsLog_debug("AFTR=%s", remoteIp);

      if ((ret = dal4in6Tunnel_add(tunnelName, mechanism, wanIntf, lanIntf, dynamic, remoteIp)) != CMSRET_SUCCESS) 
      {
         cmsLog_error("dal4in6Tunnel_add failed, ret=%d", ret);
         sprintf(cmd, "Adding 4in6 tunnel <%s> failed", tunnelName);
         cgiWriteMessagePage(fs, "4in6 Tunnel Add Error", cmd, "tunnelcfg.cmd?action=view");
         return;
      }
   }
   else
   {
      if ((ret = dal4in6Tunnel_add(tunnelName, mechanism, wanIntf, lanIntf, dynamic, NULL)) != CMSRET_SUCCESS) 
      {
         cmsLog_error("dal4in6Tunnel_add failed, ret=%d", ret);
         sprintf(cmd, "Adding 4in6 tunnel <%s> failed", tunnelName);
         cgiWriteMessagePage(fs, "4in6 Tunnel Add Error", cmd, "tunnelcfg.cmd?action=view");
         return;
      }
   }

   glbSaveConfigNeeded = TRUE;
   cgi4in6TunnelView(fs);
}

void cgi4in6TunnelRemove(char *query, FILE *fs) 
{
   char *pToken = NULL, *pLast = NULL;
   char lst[WEB_BUF_SIZE_MAX], str[BUFLEN_256];
   char tunnelName[BUFLEN_32];
   CmsRet ret = CMSRET_SUCCESS;
   
   tunnelName[0] = '\0'; lst[0] = '\0'; str[0] = '\0';

   cgiGetValueByName(query, "rmLst", lst);
   pToken = strtok_r(lst, ", ", &pLast);
   
   while ( pToken != NULL ) 
   {
      cmsUtl_strncpy(tunnelName, pToken, sizeof(tunnelName));
      if ((ret = dalTunnel_delete(tunnelName, MDMVS_IPV4INIPV6)) != CMSRET_SUCCESS)
      {
         cmsLog_error("dalTunnel_delete failed for %s (ret=%d)", tunnelName, ret);
         sprintf(str, "dalTunnel_delete failed for %s ", tunnelName);
      }

      pToken = strtok_r(NULL, ", ", &pLast);
   }
   
   if ( ret == CMSRET_SUCCESS ) 
   {
      cgi4in6TunnelView(fs);
      glbSaveConfigNeeded = TRUE;
   } else
   {
      cgiWriteMessagePage(fs, "4in6 Tunnel Remove Error", str,
                          "tunnelcfg.cmd?action=view");
   }
}

#ifdef SUPPORT_MAPT
void cgiMaptAdd(char *query, FILE *fs) 
{
   CmsRet ret = CMSRET_SUCCESS;
   char wanIntf[BUFLEN_32];
   char lanIntf[BUFLEN_32];
   char dynamicInfo[BUFLEN_4];
   UBOOL8 dynamic;
   char cmd[WEB_BUF_SIZE_MAX];

   wanIntf[0] = lanIntf[0] = dynamicInfo[0] = cmd[0] = '\0';

   cgiGetValueByName(query, "associatedWanIntf", wanIntf);
   cgiGetValueByName(query, "associatedLanIntf", lanIntf);
   cgiGetValueByName(query, "dynamic", dynamicInfo);

   cmsLog_debug("wanIntf=%s, lanIntf=%s dynamicInfo=%s", 
                 wanIntf, lanIntf, dynamicInfo);

   dynamic = atoi(dynamicInfo);
   if ( dynamic == 0 )
   {
      char BRPrefix[CMS_IPADDR_LENGTH];
      char ipv6Prefix[CMS_IPADDR_LENGTH];
      char ipv4Prefix[CMS_IPADDR_LENGTH];
      char psidOffsetInfo[BUFLEN_4];
      char psidLenInfo[BUFLEN_4];
      char psidValue[BUFLEN_8];
      UINT32 psidOffset, psidLen, psid;

      cgiGetValueByName(query, "remoteIpv6Addr", BRPrefix);
      cgiGetValueByName(query, "ruleIpv6Prefix", ipv6Prefix);
      cgiGetValueByName(query, "ruleIpv4Prefix", ipv4Prefix);
      cgiGetValueByName(query, "psidOffset", psidOffsetInfo);
      cgiGetValueByName(query, "psidLen", psidLenInfo);
      cgiGetValueByName(query, "psid", psidValue);

      psidOffset = atoi(psidOffsetInfo);
      psidLen = atoi(psidLenInfo);
      psid = atoi(psidValue);

      cmsLog_debug("BRPrefix<%s> ipv6Prefix<%s> ipv4Prefix<%s> psidOffset<%u> psidLen<%u> psid<0x%x> ", BRPrefix, ipv6Prefix, ipv4Prefix, psidOffset, psidLen, psid);

      if ((ret = dalMapt_add_dev2(wanIntf, lanIntf, dynamic, BRPrefix, ipv6Prefix, ipv4Prefix, psidOffset, psidLen, psid)) != CMSRET_SUCCESS) 
      {
         cmsLog_error("dalMapt_add failed, ret=%d", ret);
         sprintf(cmd, "Adding MAP-T <%s> failed", wanIntf);
         cgiWriteMessagePage(fs, "MAP-T configuration error", cmd, "tunnelcfg.cmd?action=viewmapt");
         return;
      }
   }
   else
   {
      cmsLog_debug("config dynamic MAP-T");

      if ((ret = dalMapt_add_dev2(wanIntf, lanIntf, dynamic, NULL, NULL, NULL, 0, 0, 0)) != CMSRET_SUCCESS) 
      {
         cmsLog_error("dalMapt_add failed, ret=%d", ret);
         sprintf(cmd, "Adding MAP-T <%s> failed", wanIntf);
         cgiWriteMessagePage(fs, "MAP-T configuration error", cmd, "tunnelcfg.cmd?action=viewmapt");
         return;
      }
   }

   glbSaveConfigNeeded = TRUE;
   cgiMaptView(fs);
}

void cgiMaptRemove(char *query, FILE *fs) 
{
   char *pToken = NULL, *pLast = NULL;
   char lst[WEB_BUF_SIZE_MAX], str[BUFLEN_256];
   char wanIntf[BUFLEN_64];
   CmsRet ret = CMSRET_SUCCESS;
   
   wanIntf[0] = '\0'; lst[0] = '\0'; str[0] = '\0';

   cgiGetValueByName(query, "rmLst", lst);
   pToken = strtok_r(lst, ", ", &pLast);
   
   while ( pToken != NULL ) 
   {
      cmsUtl_strncpy(wanIntf, pToken, sizeof(wanIntf));
      if ((ret = dalMapt_delete_dev2(wanIntf)) != CMSRET_SUCCESS)
      {
         cmsLog_error("dalMapt_delete_dev2 failed for %s (ret=%d)", wanIntf, ret);
         sprintf(str, "dalMapt_delete_dev2 failed for %s ", wanIntf);
      }

      pToken = strtok_r(NULL, ", ", &pLast);
   }
   
   if ( ret == CMSRET_SUCCESS ) 
   {
      cgiMaptView(fs);
      glbSaveConfigNeeded = TRUE;
   } else
   {
      cgiWriteMessagePage(fs, "MAP-T Remove Error", str,
                          "tunnelcfg.cmd?action=viewmapt");
   }
}
#endif

#endif /* SUPPORT_IPV6 */


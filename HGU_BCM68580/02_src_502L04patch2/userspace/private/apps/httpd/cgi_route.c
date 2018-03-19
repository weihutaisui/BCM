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
#include <net/if_arp.h>
#include <net/route.h>
#include "cgi_cmd.h"
#include "cgi_ntwk.h"
#include "cgi_route.h"
#include "cgi_main.h"
#include "syscall.h"
#include "sysdiag.h"
#include "httpd.h"


#include "cms_util.h"
#include "cms_core.h"
#include "cms_dal.h"
#include "cms_qdm.h"

extern WEB_NTWK_VAR glbWebVar; 
extern char glbErrMsg[BUFLEN_256];
extern int glbEntryCt;                   // used for web page to know how many entries can be added. In cgimain.c

#ifdef SUPPORT_RIP
void writeRtRipScript(FILE *fs) {
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n")    ;

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   fprintf(fs, "function applyClick(riplist) {\n");
   fprintf(fs, "   var lst = '';\n");
   fprintf(fs, "   var loc = 'ripcfg.cmd?action=apply';\n");
   fprintf(fs, "   with ( document.forms[0] ) {\n");
   fprintf(fs, "     if (riplist.length > 0) {\n");
   fprintf(fs, "       for (i = 0; i < riplist.length; i++)\n");
   fprintf(fs, "         if ( riplist[i].checked == true )\n");
   fprintf(fs, "           lst = lst + riplist[i].value + '|' + ripVer[i].value + '|' + \
                                 ripOp[i].value + '|' +  '1' + ', ';\n");
   fprintf(fs, "         else\n");
   fprintf(fs, "           lst = lst + riplist[i].value + '|' + ripVer[i].value + '|' + \
                                 ripOp[i].value + '|' + '0' + ', ';\n");
   fprintf(fs, "     }\n");
   fprintf(fs, "     else if ( riplist.checked == true )\n");
   fprintf(fs, "       lst = lst + riplist.value + '|' + ripVer.value + '|' + ripOp.value + \
                           '|' + '1' + ', ';\n");
   fprintf(fs, "     else if ( riplist.checked == false )\n");
   fprintf(fs, "       lst = lst + riplist.value + '|' + ripVer.value + '|' + ripOp.value + \
                           '|' + '0' + ', ';\n");
   fprintf(fs, "   }\n\n");
   fprintf(fs, "   loc += '&ripLst=' + lst;\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");
   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");
}


#ifdef DMP_BASELINE_1
UBOOL8 cgiRtRipViewBody_igd(FILE *fs)
{
   CmsRet ret;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanIpConnObject *ipConn = NULL;
   UBOOL8 foundIfc = FALSE;

   /* add lan interface as well for CDRoute tests ? or just enable that in cli?  TODO*/
   /* cgi_ripdLanInterface(fs); */
   
   while ((ret = cmsObj_getNext(MDMOID_WAN_IP_CONN, &iidStack, (void **) &ipConn)) == CMSRET_SUCCESS)
   {
      /* only routed connections (not bridged) can have RIP running on it */
      if (!cmsUtl_strcmp(ipConn->connectionType, MDMVS_IP_ROUTED))
      {
         fprintf(fs, "    <tr align='center'>\n");
         fprintf(fs, "      <td>%s</td>\n", ipConn->X_BROADCOM_COM_IfName);
         fprintf(fs, "      <td><select name='ripVer'>\n");
         /* if "Off", default to version 2 */
         if ((cmsUtl_strcmp(ipConn->routeProtocolRx, MDMVS_OFF) == 0) ||
            (cmsUtl_strcmp(ipConn->routeProtocolRx, MDMVS_RIPV2) == 0))
         {
            fprintf(fs, "            <option value='1'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;1&nbsp;&nbsp;\n");
            fprintf(fs, "            <option value='2' selected>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;2&nbsp;&nbsp;\n");
            fprintf(fs, "            <option value='3'>&nbsp;&nbsp;Both&nbsp;&nbsp;</option>\n");
         }
         else if (cmsUtl_strcmp(ipConn->routeProtocolRx, MDMVS_RIPV1) == 0)
         {
            fprintf(fs, "            <option value='1' selected>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;1&nbsp;&nbsp;\n");
            fprintf(fs, "            <option value='2'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;2&nbsp;&nbsp;\n");
            fprintf(fs, "            <option value='3'>&nbsp;&nbsp;Both&nbsp;&nbsp;</option>\n");
         }
         else if (cmsUtl_strcmp(ipConn->routeProtocolRx, MDMVS_RIPV1V2) ==0)
         {
            fprintf(fs, "            <option value='1'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;1&nbsp;&nbsp;\n");
            fprintf(fs, "            <option value='2'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;2&nbsp;&nbsp;\n");
            fprintf(fs, "            <option value='3' selected>&nbsp;&nbsp;Both&nbsp;&nbsp;</option>\n");
         }
         else
         {
            cmsLog_error("Error rip version");
            cmsObj_free((void **)&ipConn);
         }
         
         fprintf(fs, "         </select></td>\n");
         fprintf(fs, "      <td>&nbsp;&nbsp;&nbsp;<select name='ripOp'>\n");

         if (ipConn->NATEnabled)
         {
             /* if NAT enabled, only passive is allowed */
             fprintf(fs, "         <option value='1' selected>Passive</option>\n");
         }
         else
         {
            if (cmsUtl_strcmp(ipConn->X_BROADCOM_COM_RipOperationMode, MDMVS_PASSIVE) == 0)
            {
               fprintf(fs, "         <option value='0'>Active\n");
               fprintf(fs, "         <option value='1' selected>Passive</option>\n");
            }
            else
            {
               fprintf(fs, "         <option value='0' selected>Active\n");
               fprintf(fs, "         <option value='1'>Passive</option>\n");
            }
         }

         fprintf(fs, "         </select></td>\n");

         if (cmsUtl_strcmp(ipConn->routeProtocolRx, MDMVS_OFF) == 0)
         {
            fprintf(fs, "      <td align='center'><input type='checkbox' name='riplist' value='%s'></td>\n", \
                        ipConn->X_BROADCOM_COM_IfName);
         }
         else
         {
            fprintf(fs, "      <td align='center'><input type='checkbox' name='riplist' value='%s' \
                        checked></td>\n", ipConn->X_BROADCOM_COM_IfName);
         }
         fprintf(fs, "    </tr>\n");
         
         foundIfc = TRUE;
      }
      cmsObj_free((void **) &ipConn);
   }
   
   return foundIfc;
}
#endif



void cgiRtRipView(FILE *fs) 
{   
   UBOOL8 foundIfc;

   // write header
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");

   // write Java Script
   writeRtRipScript(fs);

   // write body
   fprintf(fs, "</head>\n<body>\n<blockquote>\n<form>\n");
   fprintf(fs, "<b>Routing -- RIP Configuration</b><br><br>\n");
   fprintf(fs, "<b>NOTE: If selected interface has NAT enabled, only Passive mode is allowed.</b><br><br>\n");
   fprintf(fs, "To activate RIP for the WAN Interface, select the desired RIP version and operation \
      and place a check in the 'Enabled' checkbox.  To stop RIP on the WAN Interface, uncheck the 'Enabled' \
      checkbox. Click the 'Apply/Save' button to star/stop RIP and save the configuration.<br><br>\n");
   fprintf(fs, "<br><br>\n");
   fprintf(fs, "<table border='1' cellpadding='0' cellspacing='0'>\n");
   // write table header
   fprintf(fs, "   <tr align='center'>\n");
   fprintf(fs, "      <td class='hd'>Interface</td>\n");
   fprintf(fs, "      <td class='hd'>Version</td>\n");
   fprintf(fs, "      <td class='hd'>Operation</td>\n");
   fprintf(fs, "      <td class='hd'>Enabled</td>\n");
   fprintf(fs, "   </tr>\n");

   foundIfc = cgiRtRipViewBody(fs);

   fprintf(fs, "  </table>\n");
   fprintf(fs, "  <br>\n");
   if (foundIfc == FALSE)
   {
      fprintf(fs, "<br><br>WAN Interface not exist for RIP.<br><br>\n");
   }
   else
   {
      fprintf(fs, "<center><input type='button' onClick='applyClick(this.form.riplist)' value='Apply/Save'></center>\n");
      fprintf(fs, "</center>\n");
   }
   fprintf(fs, "</form>\n");
   fprintf(fs, "</blockquote>\n");
   fprintf(fs, "</body>\n");
   fprintf(fs, "</html>\n");
   fflush(fs);
}


void cgiConfigRip(char *query, FILE *fs) {
   char action[BUFLEN_256];
   CmsRet ret = CMSRET_SUCCESS;

   cgiGetValueByName(query, "action", action);
   if (cmsUtl_strcmp(action, "apply") == 0) 
   {
      char ripList[BUFLEN_1024];
      char *pIfcName = NULL;
      char *pLast = NULL;
      char *pRipVer = NULL;
      char *pOperation = NULL;
      char *pEnabled = NULL;

      cgiGetValueByName(query, "ripLst", ripList);
      pIfcName = strtok_r(ripList, ", ", &pLast);
      while (pIfcName != NULL) 
      {
         pRipVer = strstr(pIfcName, "|");
         if (pRipVer == NULL)
         {
            continue;
         }
         pOperation = strstr(pRipVer + 1, "|");
         if (pOperation == NULL)         
         {
            continue;
         }         
         pEnabled = strstr(pOperation + 1, "|");
         if (pEnabled == NULL)
         {
            continue;
         }
         
         *pRipVer = *pOperation = *pEnabled = '\0';

         cmsLog_debug("ifcName=%s, version=%s, operation=%s, enabled=%s", pIfcName, pRipVer+1, pOperation+1, pEnabled+1);

         if ((ret = dalRip_setRipEntry(pIfcName, pRipVer+1, pOperation+1, pEnabled+1)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Failed to set ipConn with new RIP info, ret=%d", ret);
         }
         else
         {
            cmsLog_debug("set rip operation oked on %s", pIfcName);
         }
            
         pIfcName = strtok_r(NULL, ", ", &pLast);
         
      }

      if (ret == CMSRET_SUCCESS) 
      {
         glbSaveConfigNeeded = TRUE;
         cmsLog_debug("set RIP OK.");
      } 
      else
      {
         cmsLog_error("set RIP failed.");
      }

   }

   cgiRtRipView(fs);

}
#endif /* SUPPORT_RIP */



void cgiRtRouteRemove(char *query, FILE *fs) 
{
   char *pToken = NULL, *pLast = NULL, *cp = NULL;
   char lst[WEB_BUF_SIZE_MAX], str[BUFLEN_256];
   char addr[BUFLEN_40], mask[BUFLEN_40];
   CmsRet ret = CMSRET_SUCCESS;
   
   addr[0] = '\0'; mask[0] = '\0'; lst[0] = '\0'; str[0] = '\0';

   cgiGetValueByName(query, "rmLst", lst);
   pToken = strtok_r(lst, ", ", &pLast);
   
   while ( pToken != NULL ) 
   {
      if (strchr(pToken, ':') == NULL)
      {
         /* ipv4 */
         cp = strstr(pToken, "|");
         if ( cp == NULL ) 
         {
            continue;
         }
         *cp = '\0';
         strcpy(addr, pToken);
         strcpy(mask, cp + 1);
         if ((ret = dalStaticRoute_deleteEntry(addr, mask)) != CMSRET_SUCCESS)
         {
            cmsLog_error("dalStaticRoute_deleteInterface failed for %s/%s (ret=%d)", addr, mask, ret);
         }
      }
#ifdef SUPPORT_IPV6
      else
      {
         /* ipv6 */
         strcpy(addr, pToken);
         if ((ret = dalStaticRoute6_deleteEntry(addr)) != CMSRET_SUCCESS)
         {
            cmsLog_error("dalStaticRoute6_deleteInterface failed for %s (ret=%d)", addr, ret);
         }
      }
#endif
      pToken = strtok_r(NULL, ", ", &pLast);
   }
   
   if ( ret == CMSRET_SUCCESS ) 
   {
      cgiRtRouteView(fs);
      glbSaveConfigNeeded = TRUE;
   } else
   {
      cgiWriteMessagePage(fs, "Static Route Remove Error", str,
                          "rtroutecfg.cmd?action=viewcfg");
   }

}


void writeRtRouteScript(FILE *fs) 
{
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   fprintf(fs, "function addClick() {\n");
   fprintf(fs, "   var loc = 'routeadd.html';\n\n");
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

   fprintf(fs, "   var loc = 'rtroutecfg.cmd?action=remove&rmLst=' + lst;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");
   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");
}


void displaySystemRouteIfMatched(FILE *fs, const char *wanIfName, const char *DNSServers, const char *defaultGateway)
{
   SINT32 i = 0;   
   char destination[CMS_IPADDR_LENGTH] = {0};
   FILE* fsRoute = NULL;
   
   if (wanIfName == NULL || DNSServers == NULL || defaultGateway == NULL)
   {
      cmsLog_error("Invalid parameters");
      return;
   }

   /* Only use the primary dns - the first one in the list if multiple dns existed with ',' as the seperator */
   while (DNSServers[i] != ',' && i < CMS_IPADDR_LENGTH)
   {
      destination[i] = DNSServers[i];
      i++;
   }     

   if ((fsRoute= fopen("/proc/net/route", "r")) != NULL) 
   {
      char col[11][32];
      char line[512];
      SINT32 flag = 0;
      struct in_addr addr[3];
      SINT32 count = 0;

      while (fgets(line, sizeof(line), fsRoute)) 
      {
         /* read pass header line */
         if (count++ < 1)
         {
            continue;
         }
         
         sscanf(line, "%s %s %s %s %s %s %s %s %s %s %s",
                col[0], col[1], col[2], col[3], col[4], col[5],
                col[6], col[7], col[8], col[9], col[10]);
        flag = strtol(col[3], (char**)NULL, 16);
        if ((flag & RTF_UP) == RTF_UP) 
        {
            addr[0].s_addr = strtoul(col[1], (char**)NULL, 16);   /* destination */
            addr[1].s_addr = strtoul(col[2], (char**)NULL, 16);   /* gateway */
            addr[2].s_addr = strtoul(col[7], (char**)NULL, 16);   /* mask */

            if (!cmsUtl_strcmp(destination, inet_ntoa(addr[0])) &&
               !cmsUtl_strcmp(defaultGateway,  inet_ntoa(addr[1])) &&
               !cmsUtl_strcmp(wanIfName, col[0]))
            {
               /* found the match and need to dispay */
               fprintf(fs, "   <tr>\n");
               fprintf(fs, "      <td>4</td>\n");
               fprintf(fs, "      <td>%s/%d</td>\n", destination,  cmsNet_getLeftMostOneBitsInMask(inet_ntoa(addr[2])));
               fprintf(fs, "      <td>%s</td>\n", defaultGateway);
               fprintf(fs, "      <td>%s</td>\n", col[0]);

               /* No forwardingMetric for system static route */
               fprintf(fs, "      <td>&nbsp</td>\n");

               fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' disabled='1'></td>\n");
               fprintf(fs, "   </tr>\n");
            }
        }
      }
      fclose(fsRoute);
      
   }


   
}



void cgiRtRouteView(FILE *fs) 
{   
   /* write header */
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");

   /* write Java Script */
   writeRtRouteScript(fs);

   /* write body */
   fprintf(fs, "</head>\n<body>\n<blockquote>\n<form>\n");

   fprintf(fs, "<b>Routing -- Static Route (A maximum %d entries can be configured)</b><br><br>\n", \
            RT_MAX_STATIC_ENTRY);
   fprintf(fs, "<b>NOTE: For system created route, the 'Remove' checkbox is disabled.<b><br><br>");
   fprintf(fs, "<center>\n");
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");
   /* write table header */
   
   fprintf(fs, "   <tr>\n");
   fprintf(fs, "      <td class='hd'>IP Version</td>\n");
   fprintf(fs, "      <td class='hd'>DstIP/ PrefixLength</td>\n");
   fprintf(fs, "      <td class='hd'>Gateway</td>\n");
   fprintf(fs, "      <td class='hd'>Interface</td>\n");
   fprintf(fs, "      <td class='hd'>metric</td>\n");
   fprintf(fs, "      <td class='hd'>Remove</td>\n");
   fprintf(fs, "   </tr>\n");

   glbEntryCt = 0;
   
   cgiRtRouteViewBody(fs);

   fprintf(fs, "</table><br>\n");

   if (glbEntryCt < RT_MAX_STATIC_ENTRY) 
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


#ifdef DMP_BASELINE_1
void cgiRtRouteViewBody_igd(FILE *fs) 
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   void *obj = NULL;
   CmsRet ret = CMSRET_SUCCESS;
   WanPppConnObject *pppConn = NULL;   
   WanIpConnObject  *ipConn = NULL;
   
   /* Get the objects one after another till we fail. */
   while ((ret = cmsObj_getNextFlags(MDMOID_L3_FORWARDING_ENTRY, &iidStack, OGF_NO_VALUE_UPDATE, &obj)) == CMSRET_SUCCESS)
   {
      L3ForwardingEntryObject *routeCfg = (L3ForwardingEntryObject *)obj;

      if (IS_EMPTY_STRING(routeCfg->X_BROADCOM_COM_PolicyRoutingName))
      {
         fprintf(fs, "   <tr>\n");
         fprintf(fs, "      <td>4</td>\n");

         if (IS_EMPTY_STRING(routeCfg->destIPAddress))
         {
            fprintf(fs, "      <td>&nbsp</td>\n");
         }
         else
         {
            if (IS_EMPTY_STRING(routeCfg->destSubnetMask))
            {
               fprintf(fs, "      <td>%s</td>\n", routeCfg->destIPAddress);
            } 
            else
            {
               fprintf(fs, "      <td>%s/%d</td>\n", routeCfg->destIPAddress,
                       cmsNet_getLeftMostOneBitsInMask(routeCfg->destSubnetMask));
            }
         } 

         if (IS_EMPTY_STRING(routeCfg->gatewayIPAddress))
         {
            fprintf(fs, "      <td>&nbsp</td>\n");
         }
         else
         {
            fprintf(fs, "      <td>%s</td>\n", routeCfg->gatewayIPAddress);
         }

         if (IS_EMPTY_STRING(routeCfg->interface))
         {
            fprintf(fs, "      <td>&nbsp</td>\n");
         }
         else
         {
            fprintf(fs, "      <td>%s</td>\n", routeCfg->interface);
         }

         if (routeCfg->forwardingMetric < 0)
         {
            fprintf(fs, "      <td>&nbsp</td>\n");
         }
         else
         {
            fprintf(fs, "      <td>%d</td>\n", routeCfg->forwardingMetric);
         }

         fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s|%s'></td>\n",
                    routeCfg->destIPAddress, routeCfg->destSubnetMask);
         fprintf(fs, "   </tr>\n");

         glbEntryCt++;
      }

      /* free routeCfg */
      cmsObj_free((void **) &routeCfg);
   }

   INIT_INSTANCE_ID_STACK(&iidStack);

#if defined(DMP_X_BROADCOM_COM_IPV6_1)
   while ((ret = cmsObj_getNextFlags(MDMOID_I_PV6_L3_FORWARDING_ENTRY, &iidStack, OGF_NO_VALUE_UPDATE, &obj)) == CMSRET_SUCCESS)
   {
      IPv6L3ForwardingEntryObject *route6Cfg = (IPv6L3ForwardingEntryObject *)obj;

      if (IS_EMPTY_STRING(route6Cfg->policyRoutingName))
      {
         fprintf(fs, "   <tr>\n");
         fprintf(fs, "      <td>6</td>\n");

         if (IS_EMPTY_STRING(route6Cfg->destIPv6Address))
         {
            fprintf(fs, "      <td>&nbsp</td>\n");
         }
         else
         {
            fprintf(fs, "      <td>%s</td>\n", route6Cfg->destIPv6Address);
         } 

         if (IS_EMPTY_STRING(route6Cfg->gatewayIPv6Address))
         {
            fprintf(fs, "      <td>&nbsp</td>\n");
         }
         else
         {
            fprintf(fs, "      <td>%s</td>\n", route6Cfg->gatewayIPv6Address);
         }

         if (IS_EMPTY_STRING(route6Cfg->interface))
         {
            fprintf(fs, "      <td>&nbsp</td>\n");
         }
         else
         {
            fprintf(fs, "      <td>%s</td>\n", route6Cfg->interface);
         }

         if (route6Cfg->forwardingMetric < 0)
         {
            fprintf(fs, "      <td>&nbsp</td>\n");
         }
         else
         {
            fprintf(fs, "      <td>%d</td>\n", route6Cfg->forwardingMetric);
         }

         fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s'></td>\n",
                 route6Cfg->destIPv6Address);
         fprintf(fs, "   </tr>\n");

         glbEntryCt++;
      }

      /* free routeCfg */
      cmsObj_free((void **) &route6Cfg);
   }
#elif defined(DMP_X_BROADCOM_COM_DEV2_IPV6_1)
   while ((ret = cmsObj_getNextFlags(MDMOID_DEV2_IPV6_FORWARDING, &iidStack, OGF_NO_VALUE_UPDATE, &obj)) == CMSRET_SUCCESS)
   {
      Dev2Ipv6ForwardingObject *route6Cfg = (Dev2Ipv6ForwardingObject *)obj;
	  
      if (!IS_EMPTY_STRING(route6Cfg->destIPPrefix))
      {
         fprintf(fs, "   <tr>\n");
         fprintf(fs, "      <td>6</td>\n");

         if (IS_EMPTY_STRING(route6Cfg->destIPPrefix))
         {
            fprintf(fs, "      <td>&nbsp</td>\n");
         }
         else
         {
            fprintf(fs, "      <td>%s</td>\n", route6Cfg->destIPPrefix);
         } 

         if (IS_EMPTY_STRING(route6Cfg->nextHop))
         {
            fprintf(fs, "      <td>&nbsp</td>\n");
         }
         else
         {
            fprintf(fs, "      <td>%s</td>\n", route6Cfg->nextHop);
         }

         if (IS_EMPTY_STRING(route6Cfg->interface))
         {
            fprintf(fs, "      <td>&nbsp</td>\n");
         }
         else
         {
            char l3IntfNameBuf[CMS_IFNAME_LENGTH]={0};
            /* convert mdm full path string to queue interface name */
            if ((ret = qdmIntf_fullPathToIntfnameLocked_dev2(route6Cfg->interface, l3IntfNameBuf)) != CMSRET_SUCCESS)
            {
               cmsLog_error("qdmIntf_fullPathToIntfnameLocked_dev2 on %s ret=%d",
                             route6Cfg->interface, ret);
            }
            else
               fprintf(fs, "      <td>%s</td>\n", l3IntfNameBuf);
         }

         if (route6Cfg->forwardingMetric < 0)
         {
            fprintf(fs, "      <td>&nbsp</td>\n");
         }
         else
         {
            fprintf(fs, "      <td>%d</td>\n", route6Cfg->forwardingMetric);
         }

         fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s'></td>\n",
                 route6Cfg->destIPPrefix);
         fprintf(fs, "   </tr>\n");

         glbEntryCt++;
      }

      /* free routeCfg */
      cmsObj_free((void **) &route6Cfg);
   }
#endif


   /*  Need to display dns system static route at the end of user defined static route
    * those system static route cannot be removed and is for display only.
    * Find any connected routed Layer 3 interface and search the proc/net with interface name
    * primary dns ip and gateway to find a match and then display.
    */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_WAN_IP_CONN, &iidStack, (void **) &ipConn)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(ipConn->connectionType, MDMVS_IP_ROUTED) && 
         !cmsUtl_strcmp(ipConn->connectionStatus, MDMVS_CONNECTED))
      {
         displaySystemRouteIfMatched(fs, ipConn->X_BROADCOM_COM_IfName, ipConn->DNSServers, ipConn->defaultGateway);
      }
      cmsObj_free((void **) &ipConn);
   }

   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_WAN_PPP_CONN, &iidStack, (void **) &pppConn)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(pppConn->connectionStatus, MDMVS_CONNECTED))
      {
         /* for PPPoE, the defaultGateway (next hop) is always "0.0.0.0" in the routing table. */
         displaySystemRouteIfMatched(fs, pppConn->X_BROADCOM_COM_IfName,  pppConn->DNSServers, "0.0.0.0");
      }
      cmsObj_free((void **) &pppConn);
   }
}


void cgiRtRouteViewGetServiceName_igd(const char *ifName, char *serviceName)
{
   WanIpConnObject *wanIpConn = NULL;
   WanPppConnObject *wanPppConn = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found=FALSE;
   CmsRet ret;


   if (ifName == NULL || serviceName == NULL)
   {
      cmsLog_error("input args is NULL");
      return;
   }


   while (!found &&
          (ret = cmsObj_getNext(MDMOID_WAN_IP_CONN, &iidStack, (void **) &wanIpConn)) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strcmp(wanIpConn->X_BROADCOM_COM_IfName, ifName) == 0)
      {
         strcpy(serviceName, wanIpConn->name);
         found = TRUE;
      }
      cmsObj_free((void **) &wanIpConn);
   }

   if (found == TRUE)
   {
      return;
   }


   INIT_INSTANCE_ID_STACK(&iidStack);
   while (!found &&
          (ret = cmsObj_getNext(MDMOID_WAN_PPP_CONN, &iidStack, (void **) &wanPppConn)) == CMSRET_SUCCESS)
   {
      if (cmsUtl_strcmp(wanPppConn->X_BROADCOM_COM_IfName, ifName) == 0)
      {
         strcpy(serviceName, wanPppConn->name);
         found = TRUE;
      }
      cmsObj_free((void **) &wanPppConn);
   }

   if (found == TRUE)
   {
      return;
   }


   /* if not found, just add blank.  The ifName could be br0 */

   strcpy(serviceName, "&nbsp;");
}
#endif


void cgiRouteView(FILE *fs) {
   char col[11][32];
   char line[512];
   struct in_addr addr[3];
   int count = 0;
   int flag = 0;
   char flagString[64];
   int flagIndex;
   char serviceName[BUFLEN_40];

   writeNtwkHeader(fs);
   fprintf(fs, "<b>Device Info -- Route</b><br><br>\n");
   // write headers
   fprintf(fs, "Flags: U - up, ! - reject, G - gateway, H - host, R - reinstate<br>\n");
   fprintf(fs, "D - dynamic (redirect), M - modified (redirect).<br><br>\n");

   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");
   fprintf(fs, "   <tr>\n");
   fprintf(fs, "      <td class='hd'>Destination</td>\n");
   fprintf(fs, "      <td class='hd'>Gateway</td>\n");
   fprintf(fs, "      <td class='hd'>Subnet Mask</td>\n");
   fprintf(fs, "      <td class='hd'>Flag</td>\n");
   fprintf(fs, "      <td class='hd'>Metric</td>\n");
   fprintf(fs, "      <td class='hd'>Service</td>\n");
   fprintf(fs, "      <td class='hd'>Interface</td>\n");
   fprintf(fs, "   </tr>\n");
   // write body
   FILE* fsRoute = fopen("/proc/net/route", "r");
   if ( fsRoute != NULL ) {
      while ( fgets(line, sizeof(line), fsRoute) ) {
         // read pass header line
        if ( count++ < 1 ) continue;
        sscanf(line, "%s %s %s %s %s %s %s %s %s %s %s",
               col[0], col[1], col[2], col[3], col[4], col[5],
               col[6], col[7], col[8], col[9], col[10]);
        flag = strtol(col[3], (char**)NULL, 16);
        if ((flag & RTF_UP) == RTF_UP) {
          addr[0].s_addr = strtoul(col[1], (char**)NULL, 16);
          addr[1].s_addr = strtoul(col[2], (char**)NULL, 16);
          addr[2].s_addr = strtoul(col[7], (char**)NULL, 16);

          flagIndex = 0;
          if (flag & RTF_REJECT)
            flagString[flagIndex++]='!';
          else
            flagString[flagIndex++]='U';
          if (flag & RTF_GATEWAY)
            flagString[flagIndex++]='G';
          if (flag & RTF_HOST)
            flagString[flagIndex++]='H';
          if(flag & RTF_REINSTATE)
            flagString[flagIndex++]='R';
          if(flag & RTF_DYNAMIC)
            flagString[flagIndex++]='D';
          if(flag & RTF_MODIFIED)
            flagString[flagIndex++]='M';
          flagString[flagIndex]=0;
            
          fprintf(fs, "   <tr>\n");
          fprintf(fs, "      <td>%s</td>\n", inet_ntoa(addr[0]));
          fprintf(fs, "      <td>%s</td>\n", inet_ntoa(addr[1]));
          fprintf(fs, "      <td>%s</td>\n", inet_ntoa(addr[2]));           
          fprintf(fs, "      <td>%s</td>\n", flagString);
          fprintf(fs, "      <td>%s</td>\n", col[6]);

          cgiRtRouteViewGetServiceName(col[0], serviceName);

          fprintf(fs, "      <td>%s</td>\n", serviceName);
          fprintf(fs, "      <td>%s</td>\n", col[0]);
          fprintf(fs, "   </tr>\n");
        }
      }
      fclose(fsRoute);
   }
   fprintf(fs, "</table><br>\n");
   fprintf(fs, "</form>\n");
   fprintf(fs, "</blockquote>\n");
   fprintf(fs, "</body>\n");
   fprintf(fs, "</html>\n");

   fflush(fs);
}

void cgiRtRouteCfg(char *query, FILE *fs) 
{
   char action[BUFLEN_256];
   cmsLog_debug("query = %s", query);

   cgiGetValueByName(query, "action", action);

   cmsLog_debug("action = %s", action);

   if ( strcmp(action, "add") == 0 )
   {
      cgiRtRouteAdd(query, fs);
   }
   else if ( strcmp(action, "remove") == 0 )
   {
      cgiRtRouteRemove(query, fs);
   }
   else if ( strcmp(action, "view") == 0 )
   {
      /* This is from device/summary/route display */
      cgiRouteView(fs);
   }
   else
   {
      /* This is for static route display */
      cgiRtRouteView(fs);
   }
}

void cgiRtRouteAdd(char *query, FILE *fs) 
{
   CmsRet ret = CMSRET_SUCCESS;
   char ipver[BUFLEN_4];
   char addr[BUFLEN_64];
   char mask[BUFLEN_32];
   char gtwy[BUFLEN_40];
   char wanIf[BUFLEN_32];
   char metric[BUFLEN_16];
   char cmd[WEB_BUF_SIZE_MAX];

   ipver[0] = addr[0] = mask[0] = gtwy[0] = wanIf[0] = metric[0] = cmd[0] = '\0';

   cgiGetValueByName(query, "ipver", ipver);
   cgiGetValueByName(query, "dstAddr", addr);
   cgiGetValueByName(query, "dstGtwy", gtwy);
   cgiGetValueByName(query, "dstWanIf", wanIf);
   cgiGetValueByName(query, "metric", metric);

   cmsLog_debug("ipver=%s addr=%s, gtwy=%s, wanIf=%s metric=%s", ipver, addr, gtwy, wanIf, metric);

   if (atoi(ipver) == 4)
   {
      /* IPv4 address */
      struct in_addr dstAddr, dstMask;

      cmsNet_inet_cidrton(addr, &dstAddr, &dstMask);
      if (dstAddr.s_addr != 0)
      {
         strncpy(addr, inet_ntoa(dstAddr), sizeof(addr));

         if (dstMask.s_addr != 0)
         {
            strncpy(mask, inet_ntoa(dstMask), sizeof(mask));
         }
      }

      if (cmsUtl_isValidIpAddress(AF_INET, addr) == FALSE || !strcmp(addr, "0.0.0.0"))
      {
         cmsLog_error("Invalid destination IP address");
         sprintf(cmd, "Configure route for address = %s failed", addr);
         cgiWriteMessagePage(fs, "Static Route Add Error", cmd, "routeadd.html");
         return;		
      }

      if (cmsUtl_isValidIpAddress(AF_INET, mask) == FALSE || !strcmp(mask, "0.0.0.0"))
      {
         cmsLog_error("Invalid destination subnet mask");
         sprintf(cmd, "Configure route for subnet mask = %s failed", mask);
         cgiWriteMessagePage(fs, "Static Route Add Error", cmd, "routeadd.html");
         return;		
      }
      
      if (cmsUtl_strcmp(gtwy, "") != 0 && cmsUtl_isValidIpAddress(AF_INET, gtwy) == FALSE)
      {
         cmsLog_error("Invalid gateway IP address");
         sprintf(cmd, "Configure route for address = %s failed", gtwy);
         cgiWriteMessagePage(fs, "Static Route Add Error", cmd, "routeadd.html");
         return;		
		
      }

      if ((ret = dalStaticRoute_addEntry(addr, mask, gtwy, wanIf, metric)) != CMSRET_SUCCESS) 
      {
         cmsLog_error("dalStaticRoute_addEntry failed, ret=%d", ret);
         sprintf(cmd, "Configure route for address = %s and subnet mask = %s failed", addr, mask);
         cgiWriteMessagePage(fs, "Static Route Add Error", cmd, "rtroutecfg.cmd?action=viewcfg");
         return;
      }
   }
#ifdef SUPPORT_IPV6
   else
   {
      /* IPv6 address */
      char stdAddr[BUFLEN_64];
      char stdGtwy[BUFLEN_64];

      stdAddr[0] = stdGtwy[0] = '\0';

      if (cmsUtl_isValidIpAddress(AF_INET6, addr) == FALSE)
      {
         cmsLog_error("Invalid destination IPv6 address");
         sprintf(cmd, "Configure route for address = %s failed", addr);
         cgiWriteMessagePage(fs, "Static Route Add Error", cmd, "routeadd.html");
         return;		
      }

      /* convert the address to standard ipv6 format */
      if (cmsUtl_standardizeIp6Addr(addr, stdAddr) != CMSRET_SUCCESS)
      {
         cmsLog_error("Invalid destination IPv6 address");
         sprintf(cmd, "Configure route for address = %s failed", addr);
         cgiWriteMessagePage(fs, "Static Route Add Error", cmd, "routeadd.html");
         return;		
      }

      if (gtwy[0] != '\0')
      {
         if (cmsUtl_isValidIpAddress(AF_INET6, gtwy) == FALSE)
         {
            cmsLog_error("Invalid gateway IPv6 address");
            sprintf(cmd, "Configure route for address = %s failed", gtwy);
            cgiWriteMessagePage(fs, "Static Route Add Error", cmd, "routeadd.html");
            return;		
         }

         /* convert the address to standard ipv6 format */
         if (cmsUtl_standardizeIp6Addr(gtwy, stdGtwy) != CMSRET_SUCCESS)
         {
            cmsLog_error("Invalid gateway IPv6 address");
            sprintf(cmd, "Configure route for address = %s failed", gtwy);
            cgiWriteMessagePage(fs, "Static Route Add Error", cmd, "routeadd.html");
            return;		
         }
      }

      if ((ret = dalStaticRoute6_addEntry(stdAddr, stdGtwy, wanIf, metric)) != CMSRET_SUCCESS) 
      {
         cmsLog_error("dalStaticRoute6_addEntry failed, ret=%d", ret);
         sprintf(cmd, "Configure route for address = %s failed", addr);
         cgiWriteMessagePage(fs, "Static Route Add Error", cmd, "rtroutecfg.cmd?action=viewcfg");
         return;
      }
   }
#endif

   glbSaveConfigNeeded = TRUE;
   cgiRtRouteView(fs);
}
	

void cgiConfigDefaultGatewayList(char *filename __attribute__((unused)))
{

   cmsLog_debug("Enter. glbWebVar->defaultGatewayList=%s", glbWebVar.defaultGatewayList);

   dalRt_setDefaultGatewayList(glbWebVar.defaultGatewayList);

#ifdef SUPPORT_IPV6
   cmsLog_debug("glbWebVar->dfltGw6Ifc=%s", glbWebVar.dfltGw6Ifc);

   cmsDal_setSysDfltGw6Ifc(glbWebVar.dfltGw6Ifc);
#endif

   glbSaveConfigNeeded = TRUE;
}


void cgiGetDefaultGatewList(char *varValue)
{
   char saveDefaultGatewayList[CMS_MAX_DEFAULT_GATEWAY * CMS_IFNAME_LENGTH];

   dalRt_getDefaultGatewayList(saveDefaultGatewayList);

   /* If go back button is pressed and list is modified, need to use
   * the one in glbWebVar->defaultGatewayList
   */
   if (cmsUtl_strcmp(glbWebVar.defaultGatewayList, saveDefaultGatewayList))
   {
      strcpy(varValue, glbWebVar.defaultGatewayList);
   }
   else
   {
      strcpy(varValue, saveDefaultGatewayList);
   }
}

#ifdef SUPPORT_POLICYROUTING

#define MAX_PR_ENTRY	   7    // max pr entries.

void writePrScript(FILE *fs) 
{
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");
                                                                                
   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   fprintf(fs, "function addClick() {\n");
   fprintf(fs, "   var loc = 'pradd.html';\n\n");
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
                                                                                
   fprintf(fs, "   var loc = 'prmngr.cmd?action=remove&rmLst=' + lst;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");
   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");
}

#ifdef DMP_BASELINE_1
void cgiPrViewBody_igd( FILE *fs ) 
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   L3ForwardingEntryObject *routeCfg = NULL;

   /* Get the objects one after another till we fail. */
   while (cmsObj_getNext(MDMOID_L3_FORWARDING_ENTRY, &iidStack, (void **) &routeCfg) == CMSRET_SUCCESS)
   {
      if (routeCfg->X_BROADCOM_COM_PolicyRoutingName)
      {
         fprintf(fs, "   <tr>\n");

         fprintf(fs, "      <td>%s</td>\n", routeCfg->X_BROADCOM_COM_PolicyRoutingName);

         if (cmsUtl_strcmp(routeCfg->sourceIPAddress, "") == 0)
            fprintf(fs, "      <td>%s</td>\n", "&nbsp");
         else
            fprintf(fs, "      <td>%s</td>\n", routeCfg->sourceIPAddress);

         if (cmsUtl_strcmp(routeCfg->X_BROADCOM_COM_SourceIfName, "") == 0)
            fprintf(fs, "      <td>%s</td>\n", "&nbsp");
         else
            fprintf(fs, "      <td>%s</td>\n", routeCfg->X_BROADCOM_COM_SourceIfName);

         fprintf(fs, "      <td>%s</td>\n", routeCfg->interface);

         if (cmsUtl_strcmp(routeCfg->gatewayIPAddress, "") == 0)
            fprintf(fs, "      <td>%s</td>\n", "&nbsp");
         else
            fprintf(fs, "      <td>%s</td>\n", routeCfg->gatewayIPAddress);

         fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s'></td>\n", routeCfg->X_BROADCOM_COM_PolicyRoutingName);
         fprintf(fs, "   </tr>\n");
   
         glbEntryCt++;
      }
      /* free routeCfg */
      cmsObj_free((void **) &routeCfg);
   }
}
#endif

void cgiPrView( FILE *fs ) 
{
   // write header
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
                                                                                
   // write Java Script
   writePrScript(fs);
                                                                                
   // write body
   fprintf(fs, "</head>\n<body>\n<blockquote>\n");                                                                                
   fprintf(fs, 
      "<b>Policy Routing Setting -- A maximum %d entries can be configured.</b><br><br>\n", MAX_PR_ENTRY);
   fprintf(fs, "<center>\n");
   fprintf(fs, "<br>\n" );
   fprintf(fs, "<center>\n");
   fprintf(fs, "<form>\n");
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");
   // write table header
   fprintf(fs, "   <tr>\n");
   fprintf(fs, "      <td class='hd'>Policy Name</td>\n");
   fprintf(fs, "      <td class='hd'>Source IP</td>\n");
   fprintf(fs, "      <td class='hd'>LAN Port</td>\n");
   fprintf(fs, "      <td class='hd'>WAN</td>\n");
   fprintf(fs, "      <td class='hd'>Default GW</td>\n");
   fprintf(fs, "      <td class='hd'>Remove</td>\n");
   fprintf(fs, "   </tr>\n");

   glbEntryCt = 0;

   cgiPrViewBody(fs);

   fprintf(fs, "</table><br>\n");
   
   if (glbEntryCt < MAX_PR_ENTRY) 
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

void cgiPrAdd( char *query, FILE *fs ) 
{
   CmsRet sts = CMSRET_SUCCESS;
   char PolicyName[BUFLEN_16];
   char SourceIp[BUFLEN_16];
   char InPort[BUFLEN_8];
   char wanIf[BUFLEN_32];
   char defaultgw[BUFLEN_16];
   char cmd[WEB_BUF_SIZE_MAX];

   cgiGetValueByName(query, "PolicyName", PolicyName);
   cgiGetValueByName(query, "SourceIp", SourceIp);
   cgiGetValueByName(query, "lanIfcName", InPort);
   cgiGetValueByName(query, "wanIf", wanIf);
   cgiGetValueByName(query, "defaultgw", defaultgw);

   if ((sts = dalPolicyRouting_addEntry(PolicyName, SourceIp, InPort, wanIf, defaultgw)) != CMSRET_SUCCESS) 
   {
      cmsLog_error("dalPolicyRouting_addEntry failed, ret=%d", sts);
      sprintf(cmd, "Configure policy routing with PolicyName/wanIf: %s/%s failed", PolicyName, wanIf);
      cgiWriteMessagePage(fs, "Policy Routing Entry Add Error", cmd,
 			                            "prmngr.cmd?action=view");
      return;
   }
   else 
   {
      cgiPrView(fs);
      glbSaveConfigNeeded = TRUE;
   }

}

void cgiPrRemove(char *query, FILE *fs) 
{
   char *PolicyName = NULL;
   char lst[WEB_BUF_SIZE_MAX];
   char err_str[BUFLEN_256];
   CmsRet sts = CMSRET_SUCCESS;

   lst[0] = '\0';

   cgiGetValueByName( query, "rmLst", lst );

   for( PolicyName = strtok( lst, ", " ); PolicyName != NULL; PolicyName = strtok( NULL, ", " ) ) 
   {
      sts = dalPolicyRouting_deleteEntry( PolicyName );
      if (sts != CMSRET_SUCCESS ) 
      {
         snprintf( err_str, sizeof(err_str), "Unable to remove %s.", PolicyName);
         break;
      }
   }

   if( sts != CMSRET_SUCCESS ) 
   {
      cgiWriteMessagePage(fs, "Policy routing entry remove error", err_str, "prmngr.pr?action=view" );
   } 
   else 
   {
      cgiPrView(fs);
      glbSaveConfigNeeded = TRUE;   
   }
}

void cgiPolicyRouting(char *query, FILE *fs) 
{
   char action[BUFLEN_256];

   cgiGetValueByName(query, "action", action );

   if ( strcmp(action, "add") == 0 )
   {
      cgiPrAdd( query, fs );
   }
   else if ( strcmp(action, "remove") == 0 )
   {
      cgiPrRemove( query, fs );
   }
   else
   {
      cgiPrView(fs);
   }
}

#endif


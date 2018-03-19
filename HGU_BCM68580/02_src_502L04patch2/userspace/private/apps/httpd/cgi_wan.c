/*
* <:copyright-BRCM:2011:proprietary:standard
* 
*    Copyright (c) 2011 Broadcom 
*    All Rights Reserved
* 
*  This program is the proprietary software of Broadcom and/or its
*  licensors, and may only be used, duplicated, modified or distributed pursuant
*  to the terms and conditions of a separate, written license agreement executed
*  between you and Broadcom (an "Authorized License").  Except as set forth in
*  an Authorized License, Broadcom grants no license (express or implied), right
*  to use, or waiver of any kind with respect to the Software, and Broadcom
*  expressly reserves all rights in and to the Software and all intellectual
*  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
*  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
*  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
* 
*  Except as expressly set forth in the Authorized License,
* 
*  1. This program, including its structure, sequence and organization,
*     constitutes the valuable trade secrets of Broadcom, and you shall use
*     all reasonable efforts to protect the confidentiality thereof, and to
*     use this information only in connection with your use of Broadcom
*     integrated circuit products.
* 
*  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
*     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
*     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
*     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
*     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
*     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
*     PERFORMANCE OF THE SOFTWARE.
* 
*  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
*     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
*     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
*     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
*     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
*     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
*     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
*     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
*     LIMITED REMEDY.
:>
*/

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

#include "cms_dal.h"
#include "cms_boardcmds.h"
#include "cms_util.h"
#include "cms_qdm.h"

#include "cgi_cmd.h"
#include "cgi_ntwk.h"
#include "cgi_main.h"
#include "syscall.h"
#include "sysdiag.h"
#include "httpd.h"
#include "cgi_main.h"
#include "cgi_util.h"
#include "cgi_sts.h"

/*rut_util.h*/
extern UBOOL8 rut_isWanTypeEpon(void);

static void cgiWanRemove(char *path, FILE *fs);
static void cgiWanAdd(FILE *fs);

void cgiWanServiceDisplay(FILE *fs, char *srvcStr, char *ifcStr, char *protoStr, SINT32 vlanId, SINT32 vlanPr, UINT32 vlanTpid,
               UBOOL8 ipv6Enabled, UBOOL8 igmpEnabled, UBOOL8 igmpSourceEnabled, UBOOL8 natEnabled, UBOOL8 fwEnabled, 
               UBOOL8 mldEnabled, UBOOL8 mldSourceEnabled);
void writeWanCfgScript(FILE *fs);
static void writeWanIfcScript(FILE *fs);
void writeWanSrvcScript(FILE *fs, const WanIfcType iftype);
void cgiFormServiceName(char *serviceName);
void cgiFormLayer3IfName(char *L3IfName);


void cgiWanCfgView_igd(FILE *fs);
/* in cgi2_wan.c */
void cgiWanCfgView_dev2(FILE *fs);

void cgiWanCfgView(FILE *fs)  
{
#if defined(SUPPORT_DM_LEGACY98)
    cgiWanCfgView_igd(fs);
#elif defined(SUPPORT_DM_HYBRID)
    cgiWanCfgView_igd(fs);
#elif defined(SUPPORT_DM_PURE181)
    cgiWanCfgView_dev2(fs);
#elif defined(SUPPORT_DM_DETECT)
   if (cmsMdm_isDataModelDevice2())
   {
     cgiWanCfgView_dev2(fs);
   }     
   else
   {
      cgiWanCfgView_igd(fs);
   }
#endif
}


void cgiWanCfg(char *query, FILE *fs)
{
   char action[BUFLEN_256];

   cgiGetValueByName(query, "action", action);

   if ( cmsUtl_strcmp(action, "add") == 0 )
      cgiWanAdd(fs);      
   else if ( cmsUtl_strcmp(action, "remove") == 0 )
      cgiWanRemove(query, fs);
   else if ( cmsUtl_strcmp(action, "view") == 0 )
      cgiDeviceInfoWanStatus(fs);  /* for device info/wan connection status */
   else
      cgiWanCfgView(fs);
}

void cgiWanAdd(FILE *fs) 
{
   CmsRet ret = CMSRET_SUCCESS;

   cmsLog_debug("glbWebVar.serviceId=%d (==1 is Edit), cfgL2tpAc=%d", glbWebVar.serviceId, glbWebVar.cfgL2tpAc); 
   
#ifdef DMP_X_BROADCOM_COM_L2TPAC_1 
   if (glbWebVar.cfgL2tpAc == 1)
   {
      if ((ret = dalL2tpAc_addL2tpAcInterface(&glbWebVar)) != CMSRET_SUCCESS)
      {
         cmsLog_error("dalL2tpAc_addL2tpAcInterface failed. ret=%d", ret);
      }
   }
   else 
#endif /* DMP_X_BROADCOM_COM_L2TPAC_1 */   
   //This line should be marked
   //cmsLog_debug("glbWebVar.serviceId=%d (==1 is Edit), cfgPptpAc=%d", glbWebVar.serviceId, glbWebVar.cfgPptpAc); 
   
#ifdef DMP_X_BROADCOM_COM_PPTPAC_1 
   if (glbWebVar.cfgPptpAc == 1)
   {
      if ((ret = dalPptpAc_addPptpAcInterface(&glbWebVar)) != CMSRET_SUCCESS)
      {
         cmsLog_error("dalPptpAc_addPptpAcInterface failed. ret=%d", ret);
      }
   }
   else 
#endif /* DMP_X_BROADCOM_COM_PPTPAC_1 */   

   /*  glbWebVar.serviceId == 0 is add button, == 1 is edit click */
   if (glbWebVar.serviceId == 0) 
   {
      /*
      * This is an add operation.
      */
      if ((ret = dalWan_addService(&glbWebVar)) != CMSRET_SUCCESS)
      {
          cmsLog_error("dalWan_addService failed, ret=%d", ret);
      }
   }      
   else if (glbWebVar.serviceId == 1)
   {
      /* Edit on layer 3 wan connection editable information only */
      if ((ret = dalWan_editInterface(&glbWebVar)) != CMSRET_SUCCESS)
      {
         cmsLog_error("dalWan_editInterface failed in edit failed, ret=%d", ret);
      }
   }
   
   else
   {
      cmsLog_error("unrecognized serviceId %d", glbWebVar.serviceId);
      ret = CMSRET_SUCCESS; /* oh well, just display WAN connection list again. */
   }

   if (ret == CMSRET_SUCCESS)
   {
      /*
       * Wan add was successful, tell handle_request to save the config
       * before releasing the lock.
       */
      glbSaveConfigNeeded = TRUE;

      /* reload the default gateway and dns list from MDM
      * in case th Route/DNS in Advanced menu is clicked
      */
      cmsDal_getDefaultGatewayAndDnsCfg(&glbWebVar);
      /*
       * Display a list of WAN connections.
       */
      cgiWanCfgView(fs);
   }
   else
   {
      do_ej("/webs/wanadderr.html", fs);
   }

   return;   
}

void cgiWanRemove(char *query, FILE *fs) 
{
   char *pToken = NULL, *pLast = NULL;
   char lst[BUFLEN_1024];
   CmsRet ret=CMSRET_SUCCESS;
   
   cgiGetValueByName(query, "rmLst", lst);
   pToken = strtok_r(lst, ", ", &pLast);

   while (pToken != NULL)
   {
      strncpy(glbWebVar.wanIfName, pToken, sizeof(glbWebVar.wanIfName));

      if ((ret = dalWan_deleteService(&glbWebVar)) != CMSRET_SUCCESS)
      {
         cmsLog_error("dalWan_deleteService failed for %s (ret=%d)", glbWebVar.wanIfName);
      }      

      pToken = strtok_r(NULL, ", ", &pLast);

   } /* end of while loop over list of connections to delete */

   /* reload the default gateway and dns list from MDM
   * in case th Route/DNS in Advanced menu is clicked
   */
   cmsDal_getDefaultGatewayAndDnsCfg(&glbWebVar);
   
   /*
    * Whether or not there were errors during the delete,
    * save our config.
    */
   glbSaveConfigNeeded = TRUE;

   /*
    * Display remaining WAN connections.
    */
   cgiWanCfgView(fs);

   return;
}


void cgiWanServiceDisplay(FILE *fs, char *srvcStr,
                     char *ifcStr, char *protoStr,
                     SINT32 vlanId __attribute__((unused)),
                     SINT32 vlanPr __attribute__((unused)),
                     UINT32 vlanTpid __attribute__((unused)),
                     UBOOL8 ipv6Enabled __attribute__((unused)),
                     UBOOL8 igmpEnabled, UBOOL8 igmpSourceEnabled, UBOOL8 natEnabled,
                     UBOOL8 fwEnabled,
                     UBOOL8 mldEnabled __attribute__((unused)), UBOOL8 mldSourceEnabled __attribute__((unused)))
{
   fprintf(fs, "   <tr align='center'>\n");

   /* interface name */
   fprintf(fs, "      <td>%s</td>\n", ifcStr);

   /* service name */
   if (srvcStr[0] != '\0')
      fprintf(fs, "      <td>%s</td>\n", srvcStr);
   else
      fprintf(fs, "      <td>&nbsp;</td>\n");

   /* protocal */
   fprintf(fs, "      <td>%s</td>\n", protoStr);
   if (!cmsUtl_strcmp(protoStr, MDMVS_IP_BRIDGED))
   {
      strncpy(protoStr,  BRIDGE_PROTO_STR, sizeof(protoStr)-1);
   }

#ifdef SUPPORT_WANVLANMUX
   if (vlanId == VLANMUX_DISABLE)
   {
      fprintf(fs, "      <td>%s</td>\n", "N/A");
      fprintf(fs, "      <td>%s</td>\n", "N/A");
      fprintf(fs, "      <td>%s</td>\n", "N/A");
   }
   else
   {
      fprintf(fs, "      <td>%d</td>\n", vlanPr);
      fprintf(fs, "      <td>%d</td>\n", vlanId);
      fprintf(fs, "      <td>0x%X</td>\n", vlanTpid);
   }
#endif

   fprintf(fs, "      <td>%s</td>\n", igmpEnabled? MDMVS_ENABLED : MDMVS_DISABLED);

   fprintf(fs, "      <td>%s</td>\n", igmpSourceEnabled? MDMVS_ENABLED : MDMVS_DISABLED);

   fprintf(fs, "      <td>%s</td>\n", natEnabled? MDMVS_ENABLED : MDMVS_DISABLED);

   fprintf(fs, "      <td>%s</td>\n", fwEnabled? MDMVS_ENABLED : MDMVS_DISABLED);

#ifdef SUPPORT_IPV6
   fprintf(fs, "      <td>%s</td>\n", ipv6Enabled? MDMVS_ENABLED : MDMVS_DISABLED);
   fprintf(fs, "      <td>%s</td>\n", mldEnabled? MDMVS_ENABLED : MDMVS_DISABLED);
   fprintf(fs, "      <td>%s</td>\n", mldSourceEnabled? MDMVS_ENABLED : MDMVS_DISABLED);
#endif

#ifdef SUPPORT_CELLULAR
   /* cellular interface is related to APN entry, can only Edit in this page*/
   if (cmsUtl_strstr(ifcStr, CELLULAR_IFC_STR))
   {
      fprintf(fs, "      <td align='center'>&nbsp</td>\n");
   }
   else	
#endif	
   /* remove check box */
   fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s'></td>\n",
           ifcStr);

   /* edit button */
   if (!cmsUtl_strcmp(protoStr, PPPOE_PROTO_STR))
   {
      glbWebVar.ntwkPrtcl = CMS_WAN_TYPE_PPPOE;
   }
   else if (!cmsUtl_strcmp(protoStr, PPPOA_PROTO_STR))
   {
      glbWebVar.ntwkPrtcl = CMS_WAN_TYPE_PPPOA;
   }
   else if (!cmsUtl_strcmp(protoStr, IPOE_PROTO_STR))
   {
      glbWebVar.ntwkPrtcl = CMS_WAN_TYPE_DYNAMIC_IPOE;
   }
   else if (!cmsUtl_strcmp(protoStr, IPOA_PROTO_STR))
   {
      glbWebVar.ntwkPrtcl = CMS_WAN_TYPE_IPOA;
   }
   else if (!cmsUtl_strcmp(protoStr, BRIDGE_PROTO_STR))
   {
      glbWebVar.ntwkPrtcl = CMS_WAN_TYPE_BRIDGE;
   }   

   /* bridge Edit does not mak sense and will be disabled */
   if ( glbWebVar.ntwkPrtcl == CMS_WAN_TYPE_BRIDGE)
   {
      fprintf(fs, "      <td align='center'><input type='button' disabled='1' onClick='editClick(\"%s\", \"%d\")', value='&nbsp;Edit&nbsp;'\n></td>\n", ifcStr, glbWebVar.ntwkPrtcl);
   }
   else
   {
      fprintf(fs, "      <td align='center'><input type='button' onClick='editClick(\"%s\", \"%d\")', value='&nbsp;Edit&nbsp;'\n></td>\n", ifcStr, glbWebVar.ntwkPrtcl);
   }
   
   fprintf(fs, "   </tr>\n");

}




void cgiWanCfgView_igd(FILE *fs) 
{
   WanDevObject *wanDev=NULL;
   InstanceIdStack wanDevIid=EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   WanPppConnObject *pppCon=NULL;   
   WanIpConnObject *ipCon=NULL;
   SINT32 vlanId, vlanPr;
   UINT32 vlanTpid = 0;
   char serviceStr[BUFLEN_256 + 1];
   char interfaceStr[BUFLEN_32];
   char protocalStr[BUFLEN_16];
   UBOOL8 ipv6Enabled=FALSE;
   UBOOL8 igmpProxyEnabled=FALSE;
   UBOOL8 igmpSourceEnabled=FALSE;
   UBOOL8 mldProxyEnabled=FALSE;
   UBOOL8 mldSourceEnabled=FALSE;
   SINT32 wanCount = 0;
   
   /* write html header */
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");

   /* write Java Script */
   writeWanCfgScript(fs);

   /* write body */
   fprintf(fs, "<title></title>\n</head>\n<body>\n<blockquote>\n<form>\n");

   /* write table */
   fprintf(fs, "<center>\n");
   fprintf(fs, "<b>Wide Area Network (WAN) Service Setup</b><br><br>\n");
   fprintf(fs, "Choose Add, Remove or Edit to configure a WAN service over a selected interface.<br><br>\n");
#ifdef DMP_X_BROADCOM_COM_CELLULARWAN_1
   fprintf(fs, "Goto Cellular Access Point page to Add, Remove a Celluar interface.<br><br>\n");
#endif
   fprintf(fs, "<table border='1' cellpadding='4' cellspacing='0'>\n");

   /* write table header */
   fprintf(fs, "   <tr align='center'>\n");
   fprintf(fs, "      <td class='hd'>Interface</td>\n");
   fprintf(fs, "      <td class='hd'>Description</td>\n");
   fprintf(fs, "      <td class='hd'>Type</td>\n");
#ifdef SUPPORT_WANVLANMUX
   fprintf(fs, "      <td class='hd'>Vlan8021p</td>\n");
   fprintf(fs, "      <td class='hd'>VlanMuxId</td>\n");
   fprintf(fs, "      <td class='hd'>VlanTpid</td>\n");
#endif
   fprintf(fs, "      <td class='hd'>Igmp Proxy</td>\n");
   fprintf(fs, "      <td class='hd'>Igmp Source</td>\n");
   fprintf(fs, "      <td class='hd'>NAT</td>\n");
   fprintf(fs, "      <td class='hd'>Firewall</td>\n");
#ifdef SUPPORT_IPV6
   fprintf(fs, "      <td class='hd'>IPv6</td>\n");
   fprintf(fs, "      <td class='hd'>Mld Proxy</td>\n");
   fprintf(fs, "      <td class='hd'>Mld Source</td>\n");
#endif
   fprintf(fs, "      <td class='hd'>Remove</td>\n");
   fprintf(fs, "      <td class='hd'>Edit</td>\n");   
   fprintf(fs, "   </tr>\n");

   while (cmsObj_getNextFlags(MDMOID_WAN_DEV, &wanDevIid, OGF_NO_VALUE_UPDATE, (void **)&wanDev) == CMSRET_SUCCESS)
   {
      cmsObj_free((void **)&wanDev);  /* no longer needed */

      /* get the related ipCon obj */
      INIT_INSTANCE_ID_STACK(&iidStack);
      while (cmsObj_getNextInSubTreeFlags(MDMOID_WAN_IP_CONN, &wanDevIid, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&ipCon) 
         == CMSRET_SUCCESS)      
      {
         /* form serviceStr (br_0_2_35) string */
         snprintf(serviceStr, sizeof(serviceStr) - 1, "%s",
                                (ipCon->name ? ipCon->name : "(null)"));
         /* form interfaceStr (nas_0_2_35) */
         snprintf(interfaceStr, sizeof(interfaceStr), "%s",
                                (ipCon->X_BROADCOM_COM_IfName ?
                                    ipCon->X_BROADCOM_COM_IfName : "(null)"));

         /* form protocolStr */
         if (dalWan_isIPoA(&iidStack))
         {
            strcpy(protocalStr, IPOA_PROTO_STR);
         }
         else 
         {
            if (!cmsUtl_strcmp(ipCon->connectionType, MDMVS_IP_BRIDGED))
            {
               strcpy(protocalStr, BRIDGE_PROTO_STR);
            }
            else if (!cmsUtl_strcmp(ipCon->connectionType, MDMVS_IP_ROUTED))
            {
               strcpy(protocalStr, IPOE_PROTO_STR);
            }
         }

         vlanPr = dalWan_isVlanMuxEnabled(MDMOID_WAN_IP_CONN, &iidStack) ? ipCon->X_BROADCOM_COM_VlanMux8021p : VLANMUX_DISABLE;
         vlanId = dalWan_isVlanMuxEnabled(MDMOID_WAN_IP_CONN, &iidStack) ? ipCon->X_BROADCOM_COM_VlanMuxID : VLANMUX_DISABLE;
         vlanTpid = dalWan_isVlanMuxEnabled(MDMOID_WAN_IP_CONN, &iidStack) ? ipCon->X_BROADCOM_COM_VlanTpid : (UINT32)VLANMUX_DISABLE;
         
         /* Note IPv6 is tricky, this function is called in Hybrid mode,
          * but we still need to look in TR181 tree for IPv6 params.
          * Fortunately, this is all hidden by QDM.
          */
         ipv6Enabled = qdmIpIntf_isIpv6EnabledOnIntfNameLocked(ipCon->X_BROADCOM_COM_IfName);
         mldProxyEnabled = qdmMulti_isMldProxyEnabledOnIntfNameLocked(ipCon->X_BROADCOM_COM_IfName);
         mldSourceEnabled = qdmMulti_isMldSourceEnabledOnIntfNameLocked(ipCon->X_BROADCOM_COM_IfName);
         igmpProxyEnabled  = qdmMulti_isIgmpProxyEnabledOnIntfNameLocked(ipCon->X_BROADCOM_COM_IfName);
         igmpSourceEnabled = qdmMulti_isIgmpSourceEnabledOnIntfNameLocked(ipCon->X_BROADCOM_COM_IfName);

         cgiWanServiceDisplay(fs, serviceStr, interfaceStr, protocalStr, vlanId, vlanPr, vlanTpid,
               ipv6Enabled,
               igmpProxyEnabled, igmpSourceEnabled,
               ipCon->NATEnabled, ipCon->X_BROADCOM_COM_FirewallEnabled, 
               mldProxyEnabled, mldSourceEnabled);

         wanCount++;
         cmsObj_free((void **)&ipCon);
      }
   
      /* get the related pppCon obj */
      INIT_INSTANCE_ID_STACK(&iidStack);
      while (cmsObj_getNextInSubTreeFlags(MDMOID_WAN_PPP_CONN, &wanDevIid, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&pppCon) 
         == CMSRET_SUCCESS)
      {
         /* service string (pppoe_0_0_35) */
         snprintf(serviceStr, sizeof(serviceStr), "%s",
                             (pppCon->name ? pppCon->name : "(null)"));
         /* get interfaceStr (nas_0_0_35) */
         snprintf(interfaceStr, sizeof(interfaceStr), "%s",
                             (pppCon->X_BROADCOM_COM_IfName ?
                              pppCon->X_BROADCOM_COM_IfName : "(null)"));
         if (dalWan_isPPPoA(&iidStack))
         {     
            strcpy(protocalStr, PPPOA_PROTO_STR);
         }
         else
         {
            strcpy(protocalStr, PPPOE_PROTO_STR);
         }

         vlanPr = dalWan_isVlanMuxEnabled(MDMOID_WAN_PPP_CONN, &iidStack) ? pppCon->X_BROADCOM_COM_VlanMux8021p : VLANMUX_DISABLE;
         vlanId = dalWan_isVlanMuxEnabled(MDMOID_WAN_PPP_CONN, &iidStack) ? pppCon->X_BROADCOM_COM_VlanMuxID : VLANMUX_DISABLE;
         vlanTpid = dalWan_isVlanMuxEnabled(MDMOID_WAN_PPP_CONN, &iidStack) ? pppCon->X_BROADCOM_COM_VlanTpid : (UINT32)VLANMUX_DISABLE;
         
         /* Note IPv6 is tricky, this function is called in Hybrid mode,
          * but we still need to look in TR181 tree for IPv6 params.
          * Fortunately, this is all hidden by QDM.
          */
         ipv6Enabled = qdmIpIntf_isIpv6EnabledOnIntfNameLocked(pppCon->X_BROADCOM_COM_IfName);
         mldProxyEnabled = qdmMulti_isMldProxyEnabledOnIntfNameLocked(pppCon->X_BROADCOM_COM_IfName);
         mldSourceEnabled = qdmMulti_isMldSourceEnabledOnIntfNameLocked(pppCon->X_BROADCOM_COM_IfName);
         igmpProxyEnabled = qdmMulti_isIgmpProxyEnabledOnIntfNameLocked(pppCon->X_BROADCOM_COM_IfName);
         igmpSourceEnabled = qdmMulti_isIgmpSourceEnabledOnIntfNameLocked(pppCon->X_BROADCOM_COM_IfName);

         cgiWanServiceDisplay(fs, serviceStr, interfaceStr, protocalStr, vlanId, vlanPr, vlanTpid,
                  ipv6Enabled, igmpProxyEnabled, igmpSourceEnabled,
                  pppCon->NATEnabled, pppCon->X_BROADCOM_COM_FirewallEnabled,
                  mldProxyEnabled, mldSourceEnabled);

         wanCount++;
         cmsObj_free((void **)&pppCon);
      }
   }  
 
   fprintf(fs, "</table><br><br>\n");

   if (dalWan_isAdvancedDmzEnabled() == TRUE || dalWan_isPPPIpExtension() || wanCount >= IFC_WAN_MAX)
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


void writeWanCfgScript(FILE *fs)
{
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

   fprintf(fs, "function addClick() {\n");
   fprintf(fs, "   var loc = 'wanifc.cmd?serviceId=0';\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "function editClick(ifName, ntwkPrtcl) {\n");
   fprintf(fs, "   var loc = 'wanL3Edit.cmd?serviceId=1&wanIfName=' + ifName + '&ntwkPrtcl=' + ntwkPrtcl;\n\n");
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

   fprintf(fs, "   var loc = 'wancfg.cmd?action=remove&rmLst=' + lst;\n\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "   var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n\n");

   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");

}  /* End of writeWanCfgScript() */


/* Edit WAN layer 3 configuration */
void cgiWanL3Edit(char *query, FILE *fs)
{
   char serviceId[BUFLEN_8]={0};
   char ntwkPrtclStr[BUFLEN_32]={0};
 
   cgiGetValueByName(query, "serviceId", serviceId);
   glbWebVar.serviceId = atoi(serviceId);
   cgiGetValueByName(query, "wanIfName", glbWebVar.wanIfName);     
   cgiGetValueByName(query, "ntwkPrtcl", ntwkPrtclStr);

   glbWebVar.ntwkPrtcl=atoi(ntwkPrtclStr);

   if (dalWan_getWanConInfoForEdit(&glbWebVar) != CMSRET_SUCCESS)
   {
      cgiWriteMessagePage(fs, "WAN Service Edit", "Failed to get WAN info for Edit.", "wancfg.cmd");
      cmsLog_error("Failed to get wan info for edit");
      return;
   }

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);
   
   switch (glbWebVar.ntwkPrtcl)
   {
   case CMS_WAN_TYPE_PPPOE:
   case CMS_WAN_TYPE_ETHERNET_PPPOE:
   case CMS_WAN_TYPE_PPPOA:
      do_ej("/webs/pppoe.html", fs);
      break;
      
   case CMS_WAN_TYPE_STATIC_IPOE:
   case CMS_WAN_TYPE_DYNAMIC_IPOE:
   case CMS_WAN_TYPE_DYNAMIC_ETHERNET_IP:
      do_ej("/webs/natcfg2.html", fs);
      break;
      
   case CMS_WAN_TYPE_IPOA:
      do_ej("/webs/natcfg2.html", fs);
      break;   

   /* cannot edit bridge since there is nothing to be edited */
   case CMS_WAN_TYPE_BRIDGE:
      cmsLog_error("Cannot edit bridge.");
      break;   
   
   default:
      cmsLog_error("Invalid WAN Connection type %d", glbWebVar.ntwkPrtcl);
   }
   
}



/* create WAN Sevice Interface Configuration web page with dropdown box with interfaces */
void cgiWanIfc(char *query, FILE *fs) 
{
   NameList *nl, *ifList = NULL;
   char     serviceId[BUFLEN_8];

   serviceId[0] = '\0';

   if (cgiGetValueByName(query, "serviceId", serviceId) == CGI_STS_OK)
   {
      glbWebVar.serviceId = atoi(serviceId);

      /* init glbWebVar with default values */
      cgiGetWanInfo();
   }
   if (cmsDal_getAvailableIfForWanService(&ifList, TRUE) != CMSRET_SUCCESS)
   {
      char msg[BUFLEN_64];

      sprintf(msg, "Can not get available interfaces.");
      cgiWriteMessagePage(fs, "WAN Configuration Error", msg, "wancfg.cmd");
      return;
   }
   
   if (ifList == NULL)
   {
      char msg[BUFLEN_64];

      sprintf(msg, "No available interfaces.");
      cgiWriteMessagePage(fs, "WAN Configuration", msg, "wancfg.cmd");
      return;
   }

   /* write html header */
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");

   /* write Java Script */
   writeWanIfcScript(fs);

   /* write body */
   fprintf(fs, "<title></title>\n</head>\n<body>\n<blockquote>\n<form>\n");

   /* write table */
   fprintf(fs, "<center>\n");
   fprintf(fs, "<b>WAN Service Interface Configuration</b><br><br>\n");
   fprintf(fs, "<p>Select a layer 2 interface for this service</p>\n");
   
#ifdef SUPPORT_DSL
   fprintf(fs, "Note: For ATM interface, the descriptor string is (portId_vpi_vci)<br>\n");
   fprintf(fs, "      For PTM interface, the descriptor string is (portId_high_low)<br>\n");
   fprintf(fs, "          Where portId=0 --> DSL Latency PATH0<br>\n");
   fprintf(fs, "                portId=1 --> DSL Latency PATH1<br>\n");
   fprintf(fs, "                portId=4 --> DSL Latency PATH0&1<br>\n");
   fprintf(fs, "                low   =0 --> Low  PTM Priority not set<br>\n");
   fprintf(fs, "                low   =1 --> Low  PTM Priority set<br>\n");
   fprintf(fs, "                high  =0 --> High PTM Priority not set<br>\n");
   fprintf(fs, "                high  =1 --> High PTM Priority set<br><br>\n");
#endif /* SUPPORT_DSL */
   
   fprintf(fs, "<table border='0' cellpadding='0' cellspacing='0'>\n");
   fprintf(fs, "  <tr>\n");
   fprintf(fs, "  <td><select name='wanIf'>\n");

   nl = ifList;
   while (nl != NULL)
   {
#if defined(SUPPORT_ETHWAN) || defined(SUPPORT_MOCA) || \
    defined(DMP_X_BROADCOM_COM_GPONWAN_1) || defined(DMP_X_BROADCOM_COM_EPONWAN_1) 
      if (cmsUtl_strstr(nl->name, ETH_IFC_STR) || 
         cmsUtl_strstr(nl->name, MOCA_IFC_STR) ||
         cmsUtl_strstr(nl->name, GPON_IFC_STR) ||
         cmsUtl_strstr(nl->name, EPON_IFC_STR))
         
      {
            fprintf(fs, "<script language='javascript'>\n");
            fprintf(fs, "<!-- hide\n");

            fprintf(fs, "brdIntf = brdId + '|' + '%s';\n", nl->name);
            fprintf(fs, "intfDisp = getUNameByLName(brdIntf);\n");
            fprintf(fs, "document.write(\"<option value='%s\" + \"' %s>%s/\" + intfDisp);\n", 
                         nl->name,
                         cmsUtl_strcmp(nl->name, glbWebVar.wanL2IfName) == 0 ? "selected" : "",
                         nl->name);
            fprintf(fs, "// done hiding -->\n");
            fprintf(fs, "</script>\n");
      }
      else
#endif
      {
         if (cmsUtl_strcmp(nl->name, glbWebVar.wanL2IfName) == 0)
         {
           fprintf(fs, "    <option value='%s' selected>%s\n", nl->name, nl->name);
         }
         else
         {
            fprintf(fs, "    <option value='%s'>%s\n", nl->name, nl->name);
         }
      }
      nl = nl->next;
   }
   cmsDal_freeNameList(ifList);
   
   fprintf(fs, "  </select></td>\n");
   fprintf(fs, "  </tr>\n");
   fprintf(fs, "</table><br><br>\n");

   fprintf(fs, "<input type='button' onClick='btnBack()' value='Back'>\n");
   fprintf(fs, "<input type='button' onClick='btnNext()' value='Next'>\n");
   fprintf(fs, "</center>\n</form>\n</blockquote>\n</body>\n</html>\n");

   fflush(fs);

}  /* End of cgiWanIfc() */

void writeWanIfcScript(FILE *fs)
{
#if defined(SUPPORT_ETHWAN) || defined(SUPPORT_MOCA) || \
    defined(DMP_X_BROADCOM_COM_GPONWAN_1) || defined(DMP_X_BROADCOM_COM_EPONWAN_1) 
   fprintf(fs, "<script language='javascript' src='portName.js'></script>\n");
#endif

   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);

#if defined(SUPPORT_ETHWAN) || defined(SUPPORT_MOCA) || \
    defined(DMP_X_BROADCOM_COM_GPONWAN_1) || defined(DMP_X_BROADCOM_COM_EPONWAN_1) 
   fprintf(fs, "var brdId = '%s';\n", glbWebVar.boardID);
   fprintf(fs, "var intfDisp = '';\n");
   fprintf(fs, "var brdIntf = '';\n");
#endif


   /*
    * btnNext()
    */
   fprintf(fs, "function btnNext() {\n");
   fprintf(fs, "  var loc = 'wansrvc.cmd?';\n");

   fprintf(fs, "  with ( document.forms[0] ) {\n");
   fprintf(fs, "    var idx = wanIf.selectedIndex;\n");
   fprintf(fs, "    var ifc = wanIf.options[idx].value;\n");
   fprintf(fs, "    loc += 'wanL2IfName=' + ifc;\n");
   fprintf(fs, "  }\n");
   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "  var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "  eval(code);\n");
   fprintf(fs, "}\n\n");   /* End of btnNext() */


   /*
    * btnBack()
    */
   fprintf(fs, "function btnBack() {\n");
   fprintf(fs, "  var code = 'location=\"' + 'wancfg.cmd' + '\"';\n");
   fprintf(fs, "  eval(code);\n");
   fprintf(fs, "}\n\n");   /* End of btnBack() */

   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");

}  /* End of writeWanIfcScript() */



#ifdef DMP_BASELINE_1

UBOOL8 cgiWan_getInfoFromWanL2IfName_igd(const char *wanL2IfName,
                                         WanIfcType *iftype, SINT32 *connMode)
{
   UBOOL8 found=FALSE;

#ifdef DMP_ETHERNETWAN_1
   if (cmsUtl_strstr(wanL2IfName, ETH_IFC_STR))
   {
      InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
      WanEthIntfObject *ethIntfCfg=NULL;

      if ((found = dalEth_getEthIntfByIfName((char *)wanL2IfName, &iidStack, &ethIntfCfg)) == TRUE)
      {
         *iftype = WAN_IFC_ETH;
         *connMode = cmsUtl_connectionModeStrToNum(ethIntfCfg->X_BROADCOM_COM_ConnectionMode);
         cmsObj_free((void**) &ethIntfCfg);
      }
   }
#endif

#ifdef DMP_X_BROADCOM_COM_CELLULARWAN_1
   if (cmsUtl_strstr(wanL2IfName, CELLULAR_IFC_STR))
   {
      *iftype = WAN_IFC_CELLULAR;
      *connMode = CMS_CONNECTION_MODE_DEFAULT;
   }
#endif

#ifdef DMP_X_BROADCOM_COM_MOCAWAN_1
   if (cmsUtl_strstr(wanL2IfName, MOCA_IFC_STR))
   {
      InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
      WanMocaIntfObject *mocaIntfCfg=NULL;

      if ((found = dalMoca_getWanMocaIntfByIfName((char *)wanL2IfName, &iidStack, (void **)&mocaIntfCfg)) == TRUE)
      {
         /* mwang: leave ifType as WAN_IFC_ETH for now, eventually should change to WAN_IFC_MOCA and
          * change all checks for it downstream. */
         *iftype = WAN_IFC_ETH;
         *connMode = cmsUtl_connectionModeStrToNum(mocaIntfCfg->connectionMode);
         cmsObj_free((void**) &mocaIntfCfg);
      }
   }
#endif

#ifdef DMP_X_BROADCOM_COM_WIFIWAN_1
   if (cmsUtl_strstr(wanL2IfName, WLAN_IFC_STR))
   {
      InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
      WanWifiIntfObject *wifiIntfCfg=NULL;

      if ((found = dalWifiWan_getWlIntfByIfName((char *)wanL2IfName, &iidStack, &wifiIntfCfg)) == TRUE)
      {
         *iftype = WAN_IFC_WIFI;
         *connMode = cmsUtl_connectionModeStrToNum(wifiIntfCfg->connectionMode);
         cmsObj_free((void**) &wifiIntfCfg);
      }
   }
#endif

#ifdef DMP_X_BROADCOM_COM_GPONWAN_1
   if (cmsUtl_strstr(wanL2IfName, GPON_IFC_STR) && !rut_isWanTypeEpon())
   {
      InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
      WanGponLinkCfgObject *gponLinkCfg=NULL;

      if ((found  = dalGpon_getGponLinkByIfName((char *)wanL2IfName, &iidStack, &gponLinkCfg)) == TRUE)
      {
         /* Leave ifType as WAN_IFC_ETH for now, eventually should change to WAN_IFC_GPON */
         *iftype = WAN_IFC_ETH;
         *connMode = cmsUtl_connectionModeStrToNum(gponLinkCfg->connectionMode);
         cmsObj_free((void**) &gponLinkCfg);
      }
   }
#endif /* DMP_X_BROADCOM_COM_GPONWAN_1 */

#ifdef DMP_X_BROADCOM_COM_EPONWAN_1
   if (cmsUtl_strstr(wanL2IfName, EPON_IFC_STR) && rut_isWanTypeEpon())
   {
      InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
#ifdef EPON_SFU
      WanEponIntfObject *eponCfg=NULL;
      if ((found  = dalEpon_getEponIntfByIfName((char *)wanL2IfName, &iidStack, &eponCfg)) == TRUE)
      {
         /* Leave ifType as WAN_IFC_ETH for now, eventually should change to WAN_IFC_EPON */
         *iftype = WAN_IFC_ETH;
         *connMode = cmsUtl_connectionModeStrToNum(eponCfg->connectionMode);
         cmsObj_free((void**) &eponCfg);
      }
#else
      WanEponLinkCfgObject *eponLinkCfg=NULL;

      if ((found  = dalEpon_getEponLinkByIfName((char *)wanL2IfName, &iidStack, &eponLinkCfg)) == TRUE)
      {
         /* Leave ifType as WAN_IFC_ETH for now, eventually should change to WAN_IFC_EPON */
         *iftype = WAN_IFC_ETH;
         *connMode = cmsUtl_connectionModeStrToNum(eponLinkCfg->connectionMode);
         cmsObj_free((void**) &eponLinkCfg);
      }
#endif
   }
#endif /* DMP_X_BROADCOM_COM_EPONWAN_1 */

#ifdef DMP_ADSLWAN_1
   if (!found)
   {
      if (cmsUtl_strstr(wanL2IfName, IPOA_IFC_STR))
      {
         *iftype = WAN_IFC_IPOA;
         found = TRUE;
      }
#ifdef DMP_PTMWAN_1
      else if (cmsUtl_strstr(wanL2IfName, PTM_IFC_STR))
      {
         InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
         WanPtmLinkCfgObject *ptmLinkCfg=NULL;

         if ((found  = dalDsl_getPtmLinkByIfName((char *)wanL2IfName, &iidStack, &ptmLinkCfg)) == TRUE)
         {
            *iftype = WAN_IFC_PTM;
            *connMode = cmsUtl_connectionModeStrToNum(ptmLinkCfg->X_BROADCOM_COM_ConnectionMode);
            cmsObj_free((void**) &ptmLinkCfg);
         }
      }
#endif
      else
      {
         InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
         WanDslLinkCfgObject *dslLinkCfg=NULL;

         if ((found =dalDsl_getDslLinkByIfName((char *)wanL2IfName, &iidStack, &dslLinkCfg)) == TRUE)
         {
            if (cmsUtl_strcmp(dslLinkCfg->linkType, MDMVS_PPPOA) == 0)
            {
               *iftype = WAN_IFC_PPPOA;
            }
            else
            {
               *iftype = WAN_IFC_ATM;
               *connMode = cmsUtl_connectionModeStrToNum(dslLinkCfg->X_BROADCOM_COM_ConnectionMode);
            }
            cmsObj_free((void**) &dslLinkCfg);
         }
      }
   }
#endif /* DMP_ADSLWAN_1 */

   return found;
}

#endif /* DMP_BASELINE_1 */


void cgiWanSrvc(char *query, FILE *fs)
{
   UBOOL8 found = FALSE;
   UBOOL8 add = FALSE;
   WanIfcType iftype=WAN_IFC_ATM;  /* fix compiler warning, iftype will definately get set */
   char   wanL2IfName[CMS_IFNAME_LENGTH]={0};
   char *p;


   if (cgiGetValueByName(query, "wanL2IfName", wanL2IfName) == CGI_STS_OK)
   {
      /* this must be from wan service interface page */
      add = TRUE;
      strcpy(glbWebVar.wanL2IfName, wanL2IfName);
   }
   else
   {
      cmsLog_error("Did not get wanL2IfName?!?");
   }

   cmsLog_debug("wanL2IfName=%s", wanL2IfName);

   if ((p = strchr(wanL2IfName, '/')) != NULL)
   {
      char del[] = "_)";
      char *tmp = NULL;
      
      *p = '\0';

      tmp = strtok(p+1, del);
      glbWebVar.portId= atoi(tmp);

      tmp = strtok( NULL, del );
      glbWebVar.atmVpi = atoi(tmp);

      tmp = strtok( NULL, del );
      glbWebVar.atmVci = atoi(tmp);
   }
   /* eth0,...has no /(parameters) */
   else if (!(cmsUtl_strstr(wanL2IfName, ETH_IFC_STR) || 
      cmsUtl_strstr(wanL2IfName, MOCA_IFC_STR) ||
      cmsUtl_strstr(wanL2IfName, GPON_IFC_STR) ||
      cmsUtl_strstr(wanL2IfName, EPON_IFC_STR) ||
      cmsUtl_strstr(wanL2IfName, WLAN_IFC_STR)))      
   {
      char msg[BUFLEN_64]={0};
      sprintf(msg, "Invalid WAN interface.");
      cgiWriteMessagePage(fs, "WAN Configuration Error", msg, "wanifc.cmd");
      return;
   }      

   cmsLog_debug("after strip: wanL2IfName=%s", wanL2IfName);

   /* reset to default mode */
   glbWebVar.connMode = CMS_CONNECTION_MODE_DEFAULT;

   found = cgiWan_getInfoFromWanL2IfName(wanL2IfName,
                                         &iftype, &glbWebVar.connMode);
   if (!found)
   {
      char msg[BUFLEN_64];

      sprintf(msg, "Invalid WAN interface.");
      cgiWriteMessagePage(fs, "WAN Configuration Error", msg, "wanifc.cmd");
      return;
   }

   if (add)
   {
      if (iftype == WAN_IFC_PPPOA)
      {
         glbWebVar.ntwkPrtcl = CMS_WAN_TYPE_PPPOA;
      }
      else if (iftype == WAN_IFC_IPOA)
      {
         glbWebVar.ntwkPrtcl = CMS_WAN_TYPE_IPOA;
      }
      else
      {
         glbWebVar.ntwkPrtcl = CMS_WAN_TYPE_PPPOE;  /* default to pppoe */
      }
   }

   /* write html header */
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");

   /* write Java Script */
   writeWanSrvcScript(fs, iftype);
   glbWebVar.vlanMuxPr = (glbWebVar.connMode == CMS_CONNECTION_MODE_VLANMUX) ? 0:VLANMUX_DISABLE;

   /* write body */
   fprintf(fs, "<title></title>\n</head>\n<body>\n<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");

   /* write table */
   fprintf(fs, "<b>WAN Service Configuration</b><br><br>\n");
   if (iftype == WAN_IFC_ATM || iftype == WAN_IFC_ETH || iftype == WAN_IFC_PTM || iftype == WAN_IFC_WIFI)
   {
      fprintf(fs, "<table border='0' cellpadding='0' cellspacing='0'>\n");
      fprintf(fs, "  <tr><td>Select WAN service type:</td></tr>\n");
      fprintf(fs, "  <tr><td><input type='radio' name='ntwkPrtcl' onClick='prtclClick()'>&nbsp;PPP over Ethernet (PPPoE)</td></tr>\n");
      fprintf(fs, "  <tr><td><input type='radio' name='ntwkPrtcl' onClick='prtclClick()'>&nbsp;IP over Ethernet</td></tr>\n");

      if ((iftype == WAN_IFC_ATM || iftype == WAN_IFC_PTM || iftype == WAN_IFC_ETH || iftype == WAN_IFC_WIFI) &&
          (glbWebVar.connMode == CMS_CONNECTION_MODE_VLANMUX))
      {
         fprintf(fs, "  <tr><td><input type='radio' name='ntwkPrtcl' onClick='prtclClick()'>&nbsp;Bridging</td></tr>\n");
         fprintf(fs, "   ");
#ifdef DMP_X_BROADCOM_COM_IGMP_1
         fprintf(fs, "  <tr><td><div id='IgmpMcastSourceEnabler'><input type='checkbox' name='enblIgmpMcastSource' visible='false'>&nbsp;Allow as IGMP Multicast Source</div></td></tr>\n");
#endif
#ifdef DMP_X_BROADCOM_COM_MLD_1
         fprintf(fs, "  <tr><td><div id='MldMcastSourceEnabler'><input type='checkbox' name='enblMldMcastSource' visible='false'>&nbsp;Allow as MLD Multicast Source</div></td></tr>\n");
#endif
      }
      fprintf(fs, "</table>\n");
   }

   fprintf(fs, "<br><br>\n");
   fprintf(fs, "<table border='0' cellpadding='0' cellspacing='0'>\n");
   fprintf(fs, "<tr>\n");
   fprintf(fs, "<td>Enter Service Description:&nbsp;</td>\n");
   fprintf(fs, "<td><input type='text' size='32' name='servicename'></td>\n");
   fprintf(fs, "</tr>\n");
   fprintf(fs, "</table><br><br>\n");

#ifdef SUPPORT_WANVLANMUX
   if ((iftype == WAN_IFC_ATM  || iftype == WAN_IFC_PTM || iftype == WAN_IFC_ETH || iftype == WAN_IFC_WIFI)&&
       (glbWebVar.connMode == CMS_CONNECTION_MODE_VLANMUX))
   {
      fprintf(fs, "<table border='0' cellpadding='0' cellspacing='0'>\n");
      fprintf(fs, "  <tr><td>For tagged service, enter valid 802.1P Priority and 802.1Q VLAN ID.</td></tr>\n");
      fprintf(fs, "  <tr><td>For untagged service, set -1 to both 802.1P Priority and 802.1Q VLAN ID.</td></tr>\n");
      fprintf(fs, "  <tr><td>&nbsp;</td></tr>\n");
      fprintf(fs, "  <tr>\n");
      fprintf(fs, "  <td>Enter 802.1P Priority [0-7]:&nbsp;</td>\n");
      fprintf(fs, "  <td><input type='text' size='4' name='vlanMuxPr'></td>\n");
      fprintf(fs, "  </tr>\n");
      fprintf(fs, "  <tr>\n");
      fprintf(fs, "  <td>Enter 802.1Q VLAN ID [0-4094]:&nbsp;</td>\n");
      fprintf(fs, "  <td><input type='text' size='4' name='vlanMuxId'></td>\n");
      fprintf(fs, "  </tr>\n");
      fprintf(fs, "  <tr>\n");
      fprintf(fs, "  <td>Select VLAN TPID:&nbsp;</td>\n");
      fprintf(fs, "  <td><select name='vlanTpid' id='vlanTpid'>\n");
      fprintf(fs, "  <option value=\"0\">Select a TPID</option>\n");
      fprintf(fs, "  <option value=\"33024\">0x8100</option>\n");
      fprintf(fs, "  <option value=\"34984\">0x88A8</option>\n");
      fprintf(fs, "  <option value=\"37120\">0x9100</option>\n");
      fprintf(fs, "  </select></td>\n");
      fprintf(fs, "  </tr>\n");
      fprintf(fs, "</table><br><br>\n");
   }
#endif

   if (iftype == WAN_IFC_ATM || iftype == WAN_IFC_PTM || iftype == WAN_IFC_ETH || iftype == WAN_IFC_WIFI)
   {
      fprintf(fs, "<table border='0' cellpadding='0' cellspacing='0' id='IpProtocalModeTable'>\n");
      fprintf(fs, "  <tr><td>Internet Protocol Selection:</td></tr>\n");
      fprintf(fs, "  <tr><td>\n");
      fprintf(fs, "  <select name='IpProtocalMode' id='IpProtocalMode'>\n");
      fprintf(fs, "  <option value=\"IPV4\">IPV4 Only</option>\n");
#ifdef SUPPORT_IPV6
      fprintf(fs, "  <option value=\"IPv4&IPv6\">IPv4&IPv6(Dual Stack)</option>\n");
      fprintf(fs, "  <option value=\"IPv6\">IPv6 Only</option>\n");
#endif
         fprintf(fs, "  </select>\n");
      fprintf(fs, "  </td></tr>\n");
      fprintf(fs, "</table>\n");
      fprintf(fs, "<br><br>\n");
   }

   if (iftype == WAN_IFC_PPPOA)
   {
      fprintf(fs, "<table border='0' cellpadding='0' cellspacing='0' id='IpProtocalModeTable'>\n");
      fprintf(fs, "  <tr><td>Internet Protocol Selection:</td></tr>\n");
      fprintf(fs, "  <tr><td>\n");
      fprintf(fs, "  <select name='IpProtocalMode' id='IpProtocalMode'>\n");
      fprintf(fs, "  <option value=\"IPV4\">IPV4 Only</option>\n");
#ifdef SUPPORT_IPV6
      fprintf(fs, "  <option value=\"IPv4&IPv6\">IPv4&IPv6(Dual Stack)</option>\n");
      fprintf(fs, "  <option value=\"IPv6\">IPv6 Only</option>\n");
#endif
      fprintf(fs, "  </select>\n");
      fprintf(fs, "  </td></tr>\n");
      fprintf(fs, "</table>\n");
      fprintf(fs, "<br><br>\n");
   }

   fprintf(fs, "<center>\n");
   fprintf(fs, "<input type='button' onClick='btnBack()' value='Back'>\n");
   fprintf(fs, "<input type='button' onClick='btnNext()' value='Next'>\n");
   fprintf(fs, "</center>\n</form>\n</blockquote>\n</body>\n</html>\n");

   fflush(fs);

}


void writeWanSrvcScript(FILE *fs, const WanIfcType iftype)
{
   SINT32 prtcl;
   char serviceNameSufix[CMS_IFNAME_LENGTH];
   char *p;
   
   /* wanL2IfName looks like ptm0/(0_0_1) or atm1/(0_0_35) */
   if ((p = strchr(glbWebVar.wanL2IfName, '(')))
   {
   strncpy(serviceNameSufix, p+1, sizeof(serviceNameSufix)-1);
   p = strchr(serviceNameSufix, ')');
   *p = '\0';
   }
   else if (cmsUtl_strstr(glbWebVar.wanL2IfName, ETH_IFC_STR) || 
      cmsUtl_strstr(glbWebVar.wanL2IfName, MOCA_IFC_STR) ||
      cmsUtl_strstr(glbWebVar.wanL2IfName, GPON_IFC_STR) ||
      cmsUtl_strstr(glbWebVar.wanL2IfName, EPON_IFC_STR) ||
      cmsUtl_strstr(glbWebVar.wanL2IfName, WLAN_IFC_STR))
   {
      strcpy(serviceNameSufix, glbWebVar.wanL2IfName);
   }
   else
   {
      cmsLog_error("Wrong wanL2IfName %s", glbWebVar.wanL2IfName);
      return;
   }

   prtcl  = glbWebVar.ntwkPrtcl;

   fprintf(fs, "<script language='javascript' src='util.js'></script>\n");
   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n\n");

   /*generate new session key in glbCurrSessionKey*/
   cgiGetCurrSessionKey(0,NULL,NULL);


   fprintf(fs, "var defaultname = '';\n");
   /*
    * frmLoad()
    */
   fprintf(fs, "function frmLoad() {\n");
   fprintf(fs, "  with ( document.forms[0] ) {\n");

   if (prtcl == CMS_WAN_TYPE_PPPOE || prtcl == CMS_WAN_TYPE_ETHERNET_PPPOE)
   {
      fprintf(fs, "    ntwkPrtcl[0].checked = true;\n");
   }
   else if (prtcl == CMS_WAN_TYPE_DYNAMIC_IPOE || prtcl == CMS_WAN_TYPE_STATIC_IPOE
            || prtcl == CMS_WAN_TYPE_DYNAMIC_ETHERNET_IP|| prtcl == CMS_WAN_TYPE_STATIC_ETHERNET_IP
           )
   {
      fprintf(fs, "    ntwkPrtcl[1].checked = true;\n");
   }
   else if (prtcl == CMS_WAN_TYPE_BRIDGE || prtcl == CMS_WAN_TYPE_ETHERNET_BRIDGE)
   {
      fprintf(fs, "    ntwkPrtcl[2].checked = true;\n");
   }
   else
   {
      /* must be pppoa or ipoa */
   }

   fprintf(fs, "    servicename.value = '';\n");

   /* VLAN Mux */
#ifdef SUPPORT_WANVLANMUX
   if (iftype == WAN_IFC_ATM || iftype == WAN_IFC_PTM || iftype == WAN_IFC_ETH || iftype == WAN_IFC_WIFI)
   {
      if (glbWebVar.connMode == CMS_CONNECTION_MODE_VLANMUX)
      {
         fprintf(fs, "    vlanMuxPr.value = %d;\n", glbWebVar.vlanMuxPr);
         fprintf(fs, "    vlanMuxId.value = %d;\n", glbWebVar.vlanMuxId);
         fprintf(fs, "    if ( %d == '33024' )\n", glbWebVar.vlanTpid);
         fprintf(fs, "        vlanTpid.selectedIndex = 1;\n");
         fprintf(fs, "    else if ( %d == '34984' )\n", glbWebVar.vlanTpid);
         fprintf(fs, "        vlanTpid.selectedIndex = 2;\n");
         fprintf(fs, "    else if ( %d == '37120' )\n", glbWebVar.vlanTpid);
         fprintf(fs, "        vlanTpid.selectedIndex = 3;\n");
         fprintf(fs, "    else\n");
         fprintf(fs, "        vlanTpid.selectedIndex = 0;\n");
      }
   }
#endif
    
   fprintf(fs, "  }\n");
   fprintf(fs, "  prtclClick();\n");
   fprintf(fs, "}\n\n");   /* End of frmLoad() */

   /*
    * hideIpProModeTable()
    */
   fprintf(fs, "function hideIpProModeTable(hide) {\n");
   fprintf(fs, "var status = 'visible';\n");
   fprintf(fs, "if ( hide == 1 )\n");
   fprintf(fs, "   status = 'hidden';\n");
   fprintf(fs, "if (document.getElementById)  // DOM3 = IE5, NS6\n");
   fprintf(fs, "   document.getElementById('IpProtocalModeTable').style.visibility = status;\n");
   fprintf(fs, "else {\n");
   fprintf(fs, "   if (document.layers == false) // IE4\n");
   fprintf(fs, "      document.all.IpProtocalModeTable.style.visibility = status;\n");
   fprintf(fs, "   }\n");
   fprintf(fs, "}\n");

   /*
    * hideMulticastSourceEnableOption()
    */
   fprintf(fs, "function hideMulticastSourceEnableOption(hide) {\n");
   fprintf(fs, "   var status = 'visible';\n");
   fprintf(fs, "   if ( hide == 1 )\n");
   fprintf(fs, "      status = 'hidden';\n");
   fprintf(fs, "   if (document.getElementById) {  // DOM3 = IE5, NS6\n");
   fprintf(fs, "      document.getElementById('IgmpMcastSourceEnabler').style.visibility = status;\n");
#ifdef SUPPORT_IPV6
   fprintf(fs, "      document.getElementById('MldMcastSourceEnabler').style.visibility = status;\n");
#endif
   fprintf(fs, "   } else {\n");
   fprintf(fs, "      if (document.layers == false) { // IE4\n");
   fprintf(fs, "         document.all.IgmpMcastSourceEnabler.style.visibility = status;\n");
#ifdef SUPPORT_IPV6
   fprintf(fs, "         document.all.MldMcastSourceEnabler.style.visibility = status;\n");
#endif
   fprintf(fs, "      }\n");
   fprintf(fs, "   }\n");
   fprintf(fs, "}\n");

   /*
    * prtclClick()
    */
   fprintf(fs, "function prtclClick() {\n");
   fprintf(fs, "  with ( document.forms[0] ) {\n");

   switch (iftype)
   {
      /* defaultname here is the default service name */
      case WAN_IFC_ATM:
      case WAN_IFC_PTM:
      fprintf(fs, "    if (servicename.value == '' || servicename.value == defaultname) {\n");
      fprintf(fs, "      if ( ntwkPrtcl[0].checked == true )\n");
      fprintf(fs, "      {\n");
      fprintf(fs, "          defaultname = '%s_%s';\n", PPPOE_IFC_STR, serviceNameSufix);
      fprintf(fs, "          hideIpProModeTable(0);\n");
      fprintf(fs, "          hideMulticastSourceEnableOption(1);\n");
      fprintf(fs, "      }\n");
      fprintf(fs, "      else if ( ntwkPrtcl[1].checked == true )\n");
      fprintf(fs, "      {\n");
      fprintf(fs, "          defaultname = '%s_%s';\n", IPOE_IFC_STR, serviceNameSufix);
      fprintf(fs, "          hideIpProModeTable(0);\n");
      fprintf(fs, "          hideMulticastSourceEnableOption(1);\n");
      fprintf(fs, "      }\n");
      fprintf(fs, "      else\n");
      fprintf(fs, "      {\n");
      fprintf(fs, "          defaultname = '%s_%s';\n", BRIDGE_IFC_STR, serviceNameSufix);
      fprintf(fs, "          hideIpProModeTable(1);\n");
      fprintf(fs, "          hideMulticastSourceEnableOption(0);\n");
      fprintf(fs, "      }\n");
      fprintf(fs, "      servicename.value = defaultname;\n");
      fprintf(fs, "    }\n");
      break;

      case WAN_IFC_PPPOA:
      fprintf(fs, "    if (servicename.value == '' || servicename.value == defaultname) {\n");
      fprintf(fs, "      defaultname = '%s_%s';\n", PPPOA_IFC_STR, serviceNameSufix);
      fprintf(fs, "      servicename.value = defaultname;\n");
      fprintf(fs, "      hideMulticastSourceEnableOption(1);\n");
      fprintf(fs, "    }\n");
      break;

      case WAN_IFC_IPOA:
      fprintf(fs, "    if (servicename.value == '' || servicename.value == defaultname) {\n");
      fprintf(fs, "      defaultname = '%s_%s';\n", IPOA_IFC_STR, serviceNameSufix);
      fprintf(fs, "      servicename.value = defaultname;\n");
      fprintf(fs, "      hideMulticastSourceEnableOption(1);\n");
      fprintf(fs, "    }\n");
      break;

      case WAN_IFC_ETH:
      case WAN_IFC_WIFI:
#if defined(SUPPORT_ETHWAN) || defined(SUPPORT_MOCA) || \
    defined(DMP_X_BROADCOM_COM_GPONWAN_1) || defined(DMP_X_BROADCOM_COM_EPONWAN_1) 
      fprintf(fs, "    if (servicename.value == '' || servicename.value == defaultname) {\n");
      fprintf(fs, "      if ( ntwkPrtcl[0].checked == true )\n");
      fprintf(fs, "      {\n");
      fprintf(fs, "          defaultname = '%s_%s';\n", PPPOE_IFC_STR, serviceNameSufix);
      fprintf(fs, "          hideIpProModeTable(0);\n");
      fprintf(fs, "          hideMulticastSourceEnableOption(1);\n");
      fprintf(fs, "      }\n");
      if (glbWebVar.connMode == CMS_CONNECTION_MODE_VLANMUX)
      {
         fprintf(fs, "      else if ( ntwkPrtcl[1].checked == true )\n");
         fprintf(fs, "      {\n");
         fprintf(fs, "          defaultname = '%s_%s';\n", IPOE_IFC_STR, serviceNameSufix);
         fprintf(fs, "          hideIpProModeTable(0);\n");
         fprintf(fs, "          hideMulticastSourceEnableOption(1);\n");
         fprintf(fs, "      }\n");
         fprintf(fs, "      else\n");
         fprintf(fs, "      {\n");
         fprintf(fs, "          defaultname = '%s_%s';\n", BRIDGE_IFC_STR, serviceNameSufix);
         fprintf(fs, "          hideIpProModeTable(1);\n");
         fprintf(fs, "          hideMulticastSourceEnableOption(0);\n");
         fprintf(fs, "      }\n");
         fprintf(fs, "      servicename.value = defaultname;\n");
         fprintf(fs, "    }\n");
      }
      else
      {
         fprintf(fs, "      else\n");
         fprintf(fs, "      {\n");
         fprintf(fs, "        defaultname = '%s_%s';\n", IPOE_IFC_STR, serviceNameSufix);
         fprintf(fs, "        hideMulticastSourceEnableOption(1);\n");
         fprintf(fs, "      }\n");
         fprintf(fs, "      servicename.value = defaultname;\n");
         fprintf(fs, "    }\n");
      }
#endif
      break;

#ifdef SUPPORT_CELLULAR
      case WAN_IFC_CELLULAR:
      cmsLog_error("Unexpected cellular wan");
      break;
#endif

      case WAN_IFC_NONE:
      break;
   }

   fprintf(fs, "  }\n");
   fprintf(fs, "}\n\n");   /* End of prtclClick() */

   /*
    * btnNext()
    */
   fprintf(fs, "function btnNext() {\n");
   fprintf(fs, "  var loc = '';\n");

   fprintf(fs, "  with ( document.forms[0] ) {\n");

   switch (iftype)
   {
      case WAN_IFC_ATM:
      case WAN_IFC_PTM:
         
      fprintf(fs, "    if ( ntwkPrtcl[0].checked == true )\n");
      fprintf(fs, "      loc = 'pppoe.cgi?enblEnetWan=0&ntwkPrtcl=%d';\n", CMS_WAN_TYPE_PPPOE);
      fprintf(fs, "    else if ( ntwkPrtcl[1].checked == true )\n");
      fprintf(fs, "      loc = 'wancfg.cgi?enblEnetWan=0&ntwkPrtcl=%d';\n", CMS_WAN_TYPE_DYNAMIC_IPOE);
      fprintf(fs, "    else\n");
      fprintf(fs, "    {\n");
      fprintf(fs, "      loc = 'ntwksum2.cgi?enblEnetWan=0&ntwkPrtcl=%d';\n", CMS_WAN_TYPE_BRIDGE);
#ifdef DMP_X_BROADCOM_COM_IGMP_1
      fprintf(fs, "      if ( enblIgmpMcastSource.checked ) {\n");
      fprintf(fs, "        loc += '&enblIgmpMcastSource=1';\n");
      fprintf(fs, "      } else { \n");
      fprintf(fs, "        loc += '&enblIgmpMcastSource=0';\n");
      fprintf(fs, "      }\n");
#endif
#ifdef DMP_X_BROADCOM_COM_MLD_1
      fprintf(fs, "      if ( enblMldMcastSource.checked ) {\n");
      fprintf(fs, "        loc += '&enblMldMcastSource=1';\n");
      fprintf(fs, "      } else { \n");
      fprintf(fs, "        loc += '&enblMldMcastSource=0';\n");
      fprintf(fs, "      }\n");
#endif
      fprintf(fs, "    }\n");
#ifdef SUPPORT_WANVLANMUX
      if (glbWebVar.connMode == CMS_CONNECTION_MODE_VLANMUX)
      {
         fprintf(fs, "    var vlmpr = parseInt(vlanMuxPr.value);\n");
         fprintf(fs, "    var vlmid = parseInt(vlanMuxId.value);\n");
         fprintf(fs, "    if ( vlmpr != -1 || vlmid != -1 ) {\n");
         fprintf(fs, "    if ( vlmpr < 0 || vlmpr > 7 ) {\n");
         fprintf(fs, "      alert('VLAN 8021p Priority \"' + vlmpr + '\" is out of range [0-7].');\n");
         fprintf(fs, "      return;\n");
         fprintf(fs, "    }\n");
         fprintf(fs, "    if ( vlmid < 0 || vlmid > 4094 ) {\n");
         fprintf(fs, "      alert('VLAN ID \"' + vlmid + '\" is out of range [0-4094].');\n");
         fprintf(fs, "      return;\n");
         fprintf(fs, "    }\n");
         fprintf(fs, "    servicename.value =  servicename.value + '.' + vlanMuxId.value\n");
         fprintf(fs, "    }\n");
         fprintf(fs, "    loc += '&enVlanMux=1&vlanMuxId='+vlmid;\n");
         fprintf(fs, "    loc += '&vlanMuxPr='+vlmpr;\n");
         fprintf(fs, "    loc += '&vlanTpid='+vlanTpid.options[vlanTpid.selectedIndex].value;\n");
      }
#endif
#ifdef SUPPORT_IPV6
      fprintf(fs, "        if ( IpProtocalMode.selectedIndex == 0 )\n");
      fprintf(fs, "        {\n");
      fprintf(fs, "           loc += '&enblIpVer=0';\n");
      fprintf(fs, "        }\n");
      fprintf(fs, "        else if ( IpProtocalMode.selectedIndex == 1 )\n");
      fprintf(fs, "        {\n");
      fprintf(fs, "           loc += '&enblIpVer=1';\n");
      fprintf(fs, "        }\n"); 
      fprintf(fs, "        else if ( IpProtocalMode.selectedIndex == 2 )\n");
      fprintf(fs, "        {\n");
      fprintf(fs, "           loc += '&enblIpVer=2';\n");
      fprintf(fs, "        }\n"); 

#endif
      break;

      case WAN_IFC_PPPOA:
      fprintf(fs, "    loc = 'pppoe.cgi?enblEnetWan=0&ntwkPrtcl=%d';\n", CMS_WAN_TYPE_PPPOA);
#ifdef SUPPORT_IPV6
      fprintf(fs, "        if ( IpProtocalMode.selectedIndex == 0 )\n");
      fprintf(fs, "        {\n");
      fprintf(fs, "           loc += '&enblIpVer=0';\n");
      fprintf(fs, "        }\n");
      fprintf(fs, "        else if ( IpProtocalMode.selectedIndex == 1 )\n");
      fprintf(fs, "        {\n");
      fprintf(fs, "           loc += '&enblIpVer=1';\n");
      fprintf(fs, "        }\n"); 
      fprintf(fs, "        else if ( IpProtocalMode.selectedIndex == 2 )\n");
      fprintf(fs, "        {\n");
      fprintf(fs, "           loc += '&enblIpVer=2';\n");
      fprintf(fs, "        }\n"); 
#endif
      break;

      case WAN_IFC_IPOA:
      fprintf(fs, "    loc = 'ipoacfg.cgi?enblEnetWan=0&ntwkPrtcl=%d';\n", CMS_WAN_TYPE_IPOA);
      break;

      case WAN_IFC_ETH:

      fprintf(fs, "    if ( ntwkPrtcl[0].checked == true )\n");
      fprintf(fs, "      loc = 'pppoe.cgi?enblEnetWan=0&ntwkPrtcl=%d';\n", CMS_WAN_TYPE_ETHERNET_PPPOE);
      fprintf(fs, "    else if ( ntwkPrtcl[1].checked == true )\n");
      fprintf(fs, "      loc = 'wancfg.cgi?enblEnetWan=0&ntwkPrtcl=%d';\n", CMS_WAN_TYPE_DYNAMIC_ETHERNET_IP);
      fprintf(fs, "    else\n");
      fprintf(fs, "    {\n");
      fprintf(fs, "      loc = 'ntwksum2.cgi?enblEnetWan=0&ntwkPrtcl=%d';\n", CMS_WAN_TYPE_ETHERNET_BRIDGE);
#ifdef DMP_X_BROADCOM_COM_IGMP_1
      fprintf(fs, "      if ( enblIgmpMcastSource.checked ) {\n");
      fprintf(fs, "        loc += '&enblIgmpMcastSource=1';\n");
      fprintf(fs, "      } else { \n");
      fprintf(fs, "        loc += '&enblIgmpMcastSource=0';\n");
      fprintf(fs, "      }\n");
#endif
#ifdef DMP_X_BROADCOM_COM_MLD_1
      fprintf(fs, "      if ( enblMldMcastSource.checked ) {\n");
      fprintf(fs, "        loc += '&enblMldMcastSource=1';\n");
      fprintf(fs, "      } else { \n");
      fprintf(fs, "        loc += '&enblMldMcastSource=0';\n");
      fprintf(fs, "      }\n");
#endif
      fprintf(fs, "    }\n");
#ifdef SUPPORT_WANVLANMUX
      if (glbWebVar.connMode == CMS_CONNECTION_MODE_VLANMUX)
      {
         fprintf(fs, "    var vlmpr = parseInt(vlanMuxPr.value);\n");
         fprintf(fs, "    var vlmid = parseInt(vlanMuxId.value);\n");
         fprintf(fs, "    if ( vlmpr != -1 || vlmid != -1 ) {\n");
         fprintf(fs, "    if ( vlmpr < 0 || vlmpr > 7 ) {\n");
         fprintf(fs, "      alert('VLAN 8021p Priority \"' + vlmpr + '\" is out of range [0-7].');\n");
         fprintf(fs, "      return;\n");
         fprintf(fs, "    }\n");
         fprintf(fs, "    if ( vlmid < 0 || vlmid > 4094 ) {\n");
         fprintf(fs, "      alert('VLAN ID \"' + vlmid + '\" is out of range [0-4094].');\n");
         fprintf(fs, "      return;\n");
         fprintf(fs, "    }\n");
         fprintf(fs, "    servicename.value =  servicename.value + '.' + vlanMuxId.value\n");
         fprintf(fs, "    }\n");
         fprintf(fs, "    loc += '&vlanMuxId='+vlmid;\n");
         fprintf(fs, "    loc += '&vlanMuxPr='+vlmpr;\n");
         fprintf(fs, "    loc += '&vlanTpid='+vlanTpid.options[vlanTpid.selectedIndex].value;\n");
      }
#endif
#ifdef SUPPORT_IPV6
      fprintf(fs, "        if ( IpProtocalMode.selectedIndex == 0 )\n");
      fprintf(fs, "        {\n");
      fprintf(fs, "           loc += '&enblIpVer=0';\n");
      fprintf(fs, "        }\n");
      fprintf(fs, "        else if ( IpProtocalMode.selectedIndex == 1 )\n");
      fprintf(fs, "        {\n");
      fprintf(fs, "           loc += '&enblIpVer=1';\n");
      fprintf(fs, "        }\n"); 
      fprintf(fs, "        else if ( IpProtocalMode.selectedIndex == 2 )\n");
      fprintf(fs, "        {\n");
      fprintf(fs, "           loc += '&enblIpVer=2';\n");
      fprintf(fs, "        }\n"); 
#endif
      break;

      case WAN_IFC_WIFI:

      fprintf(fs, "    if ( ntwkPrtcl[0].checked == true )\n");
      fprintf(fs, "      loc = 'pppoe.cgi?enblEnetWan=0&ntwkPrtcl=%d';\n", CMS_WAN_TYPE_PPPOE);
      fprintf(fs, "    else if ( ntwkPrtcl[1].checked == true )\n");
      fprintf(fs, "      loc = 'wancfg.cgi?enblEnetWan=0&ntwkPrtcl=%d';\n", CMS_WAN_TYPE_DYNAMIC_IPOE);
      fprintf(fs, "    else\n");
      fprintf(fs, "    {\n");
      fprintf(fs, "      loc = 'ntwksum2.cgi?enblEnetWan=0&ntwkPrtcl=%d';\n", CMS_WAN_TYPE_BRIDGE);
#ifdef DMP_X_BROADCOM_COM_IGMP_1
      fprintf(fs, "      if ( enblIgmpMcastSource.checked ) {\n");
      fprintf(fs, "        loc += '&enblIgmpMcastSource=1';\n");
      fprintf(fs, "      } else { \n");
      fprintf(fs, "        loc += '&enblIgmpMcastSource=0';\n");
      fprintf(fs, "      }\n");
#endif
#ifdef DMP_X_BROADCOM_COM_MLD_1
      fprintf(fs, "      if ( enblMldMcastSource.checked ) {\n");
      fprintf(fs, "        loc += '&enblMldMcastSource=1';\n");
      fprintf(fs, "      } else { \n");
      fprintf(fs, "        loc += '&enblMldMcastSource=0';\n");
      fprintf(fs, "      }\n");
#endif
      fprintf(fs, "    }\n");
#ifdef SUPPORT_WANVLANMUX
      if (glbWebVar.connMode == CMS_CONNECTION_MODE_VLANMUX)
      {
         fprintf(fs, "    var vlmpr = parseInt(vlanMuxPr.value);\n");
         fprintf(fs, "    var vlmid = parseInt(vlanMuxId.value);\n");
         fprintf(fs, "    if ( vlmpr != -1 || vlmid != -1 ) {\n");
         fprintf(fs, "    if ( vlmpr < 0 || vlmpr > 7 ) {\n");
         fprintf(fs, "      alert('VLAN 8021p Priority \"' + vlmpr + '\" is out of range [0-7].');\n");
         fprintf(fs, "      return;\n");
         fprintf(fs, "    }\n");
         fprintf(fs, "    if ( vlmid < 0 || vlmid > 4094 ) {\n");
         fprintf(fs, "      alert('VLAN ID \"' + vlmid + '\" is out of range [0-4094].');\n");
         fprintf(fs, "      return;\n");
         fprintf(fs, "    }\n");
         fprintf(fs, "    servicename.value =  servicename.value + '.' + vlanMuxId.value\n");
         fprintf(fs, "    }\n");
         fprintf(fs, "    loc += '&vlanMuxId='+vlmid;\n");
         fprintf(fs, "    loc += '&vlanMuxPr='+vlmpr;\n");
         fprintf(fs, "    loc += '&vlanTpid='+vlanTpid.options[vlanTpid.selectedIndex].value;\n");
      }
#endif
#ifdef SUPPORT_IPV6
      fprintf(fs, "        if ( IpProtocalMode.selectedIndex == 0 )\n");
      fprintf(fs, "        {\n");
      fprintf(fs, "           loc += '&enblv6=0';\n");
      fprintf(fs, "        }\n");
      fprintf(fs, "        else if ( IpProtocalMode.selectedIndex == 1 )\n");
      fprintf(fs, "        {\n");
      fprintf(fs, "           loc += '&enblv6=1';\n");
      fprintf(fs, "        }\n"); 
#endif
      break;

#ifdef SUPPORT_CELLULAR
      case WAN_IFC_CELLULAR:
      cmsLog_error("Unexpected cellular wan");
      break;
#endif

      case WAN_IFC_NONE:
      break;
   }

   fprintf(fs, "    loc += '&serviceName=' + servicename.value;\n");

   fprintf(fs, "  }\n");

   fprintf(fs, "   loc += '&sessionKey=%d';\n",glbCurrSessionKey);
   fprintf(fs, "  var code = 'location=\"' + loc + '\"';\n");
   fprintf(fs, "  eval(code);\n");
   fprintf(fs, "}\n\n");   /* End of btnNext() */


   /*
    * btnBack()
    */
   fprintf(fs, "function btnBack() {\n");
   fprintf(fs, "  var code = 'location=\"' + 'wanifc.cmd' + '\"';\n");
   fprintf(fs, "  eval(code);\n");
   fprintf(fs, "}\n\n");   /* End of btnBack() */

   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");

}  /* End of writeWanSrvcScript() */


void cgiGetWanInfo(void) {

   cmsLog_notice("serviceId=%d", glbWebVar.serviceId);

   if (glbWebVar.serviceId == 0)
   {
      /* just load glbWebVar with default info for the user */
      cmsDal_getAllInfo(&glbWebVar);
   }
   
#ifdef noEdit   
   else
   {
      /*
       * Load the glbWebVar with info for that specific service id.
       */

#ifdef SUPPORT_IPV6
      qdmIpv6_getDns6Info(glbWebVar.dns6Type, glbWebVar.dns6Ifc, glbWebVar.dns6Pri, glbWebVar.dns6Sec);
      qdmRt_getSysDfltGw6(glbWebVar.dfltGw6Ifc, NULL);
#endif

      glbWebVar.pppAuthErrorRetry = 0;
/* TODO:  in memory or flash X_BROADCOM_AUTHErrorRetry      glbWebVar.pppShowAuthErrorRetry = getShowAuthErrorRetry(); */

      if (dalWan_getWanConInfoForWebEdit(&glbWebVar) != CMSRET_SUCCESS)
      {
         cmsLog_error("Failed to get wan info for edit");
         return;
      }
   }
#endif /* noEdit */

}


void cgiGetWanInterfaceInfo_igd(int argc __attribute__((unused)), char **argv, char *varValue) 
{
   char currIfc[BUFLEN_64];
   char serviceName[BUFLEN_32];
   char defaultGwStr[BUFLEN_32];
   char tempStr[BUFLEN_64];
   char ifcListStr[BUFLEN_1024]={0};
   UBOOL8 usedForDns = FALSE;
   UBOOL8 needEoAIntf = FALSE;
   UBOOL8 firewallEnabledInfo = FALSE;
   UBOOL8 bridgeIfcInfo = FALSE;
#ifdef SUPPORT_IPV6
   UBOOL8 needIPv6 = FALSE;
#endif
   InstanceIdStack iidStack;
   WanPppConnObject *pppCon=NULL;   
   WanIpConnObject *ipCon=NULL;
   CmsRet ret;

   varValue[0] = currIfc[0] = defaultGwStr[0] =  '\0'; 
   
   cmsLog_debug("argv[0]=%s argv[1]=%s argv[2]=%s", argv[0], argv[1], argv[2]);

   /* for interface grouping, all pvc including bridge are needed */
   if (cmsUtl_strcmp(argv[2], "allEoAPvc") == 0)
   {
      needEoAIntf = TRUE;
   }
   
   /* for port mirroring, all Layer 2 EoA interfaces are needed */
   if (cmsUtl_strcmp(argv[2], "portMirror") == 0)
   {
      /* try search for DSL interfae for port mirror entries */
      dalPMirror_getPMirrorList(varValue);
      return;
   }

#ifdef DMP_X_ITU_ORG_GPON_1
   if (cmsUtl_strcmp(argv[2], "gponPortMirror") == 0)
   {
      /* try search for DSL interfae for port mirror entries */
      dalPMirror_getGponPMirrorList(varValue);
      return;
   }
#endif
   
   /* for checking interfaces with firewall enabled */
   if (cmsUtl_strcmp(argv[2], "firewall") == 0)
   {
      firewallEnabledInfo = TRUE;
   }
   /* for checking bridged PVCs */
   else if (cmsUtl_strcmp(argv[2], "bridge") == 0)
   {
      bridgeIfcInfo = TRUE;
   }

#ifdef SUPPORT_IPV6
   /* for checking IPv6 enabled connection */
   if (cmsUtl_strcmp(argv[2], "route6") == 0 ||
       cmsUtl_strcmp(argv[2], "route6PlusOne") == 0 ||
       cmsUtl_strcmp(argv[2], "forDns6") == 0 ||
       cmsUtl_strcmp(argv[2], "forDns6PlusOne") == 0)
   {
      needIPv6 = TRUE;
cmsLog_debug("needIPv6<%d>", needIPv6); //FIXME: no need!!
   }
#endif
   
   /* for dns info only */
   if (cmsUtl_strcmp(argv[2], "forDns") == 0 ||
       cmsUtl_strcmp(argv[2], "forDnsPlusOne") == 0 ||
       cmsUtl_strcmp(argv[2], "forDns6") == 0 ||
       cmsUtl_strcmp(argv[2], "forDns6PlusOne") == 0)
   {
      usedForDns = TRUE;
   }


   /* for pvc creatation, need to put in the current pvc as one of the selection */
   if ((cmsUtl_strcmp(argv[2], "routePlusOne")   == 0) ||
       (cmsUtl_strcmp(argv[2], "route6PlusOne")  == 0) ||
       (cmsUtl_strcmp(argv[2], "forDnsPlusOne")  == 0 && !isStaticPVC()) ||
       (cmsUtl_strcmp(argv[2], "forDns6PlusOne") == 0 && !isStaticPVC()))  /* for dns only if not ipoa and static MER */
   {
      cgiFormLayer3IfName(tempStr);
      cgiFormServiceName(serviceName);
      snprintf(currIfc, sizeof(currIfc), "%s/%s|", serviceName, tempStr);
      cmsLog_debug("Current ifc = %s", currIfc);      
   }

   /* get the related ipCon obj */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ( cmsObj_getNextFlags(MDMOID_WAN_IP_CONN, &iidStack, OGF_NO_VALUE_UPDATE, (void **) &ipCon) == CMSRET_SUCCESS)
   {
       /* for checking interfaces with firewall enabled */
       if ( firewallEnabledInfo == TRUE )
       {
          if ( ipCon->X_BROADCOM_COM_FirewallEnabled == TRUE )
          {
             snprintf(tempStr, sizeof(tempStr), "%s/%s|", ipCon->name, ipCon->X_BROADCOM_COM_IfName);
             strncat(ifcListStr, tempStr, sizeof(ifcListStr)-1);
          }
          cmsObj_free((void **)&ipCon);
          continue;
       }
       /* for checking bridged PVCs */
       else if ( bridgeIfcInfo == TRUE )
       {
          if ( cmsUtl_strcmp(ipCon->connectionType, MDMVS_IP_BRIDGED) == 0 )
          {
             snprintf(tempStr, sizeof(tempStr), "%s/%s|", ipCon->name, ipCon->X_BROADCOM_COM_IfName);
             strncat(ifcListStr, tempStr, sizeof(ifcListStr)-1);
          }
          cmsObj_free((void **)&ipCon);
          continue;
       }
	    
       /* if not all EoA interface needed, do the following
        * skip 1) bridge protocol  
        * OR 2) used for dns and protocol is protocol is IPoA
        * OR 3) used for dns and static IPOE (not IPoA)
        */
       if (!needEoAIntf &&
           ((!cmsUtl_strcmp(ipCon->connectionType, MDMVS_IP_BRIDGED)) ||
            (usedForDns && dalWan_isEoAInterface(&iidStack)) ||
            (usedForDns && !dalWan_isEoAInterface(&iidStack) && !cmsUtl_strcmp(ipCon->addressingType, MDMVS_STATIC))))
       {
          cmsObj_free((void **)&ipCon);
          continue;
       }
       else
       {
          /*  skip on
           * 1). if needEoA is TRUE but it is IpoA 
           * 2). if needEoA is TRUE and the wan interface is already in the interface group (not br0)
           */
          if ((needEoAIntf && dalWan_isEoAInterface(&iidStack)) ||
              (needEoAIntf && (dalPMap_isWanUsedForIntfGroup(ipCon->X_BROADCOM_COM_IfName) == TRUE)))
          {
             cmsObj_free((void **)&ipCon);
             continue;
          }

#ifdef DMP_X_BROADCOM_COM_IPV6_1
          /* FIXME IPV6 */
          /* skip on
           * 1) if needIPv6 is TRUE but it is not ipv6 enabled.
           */
          if (needIPv6 && !ipCon->X_BROADCOM_COM_IPv6Enabled)
          {
             cmsObj_free((void **)&ipCon);
             continue;
          }
#endif
          snprintf(tempStr, sizeof(tempStr), "%s/%s|", ipCon->name, ipCon->X_BROADCOM_COM_IfName);
          strncat(ifcListStr, tempStr, sizeof(ifcListStr)-1);
          cmsObj_free((void **)&ipCon);
       }
   }

   
   /* get the related pppCon obj */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while (cmsObj_getNextFlags(MDMOID_WAN_PPP_CONN, &iidStack, OGF_NO_VALUE_UPDATE, (void **) &pppCon) == CMSRET_SUCCESS)
   {
      /* for checking interfaces with firewall enabled */
      if ( firewallEnabledInfo == TRUE )
      {
         if ( pppCon->X_BROADCOM_COM_FirewallEnabled == TRUE )
         {
            snprintf(tempStr, sizeof(tempStr), "%s/%s|", pppCon->name, pppCon->X_BROADCOM_COM_IfName);
            strncat(ifcListStr, tempStr, sizeof(ifcListStr)-1);
         }
         cmsObj_free((void **)&pppCon);
         continue;
      }
   	
       /* for checking bridged PVCs */
      else if ( bridgeIfcInfo == TRUE )
      {
         if ( cmsUtl_strcmp(pppCon->connectionType, MDMVS_IP_BRIDGED) == 0 )
         {
            snprintf(tempStr, sizeof(tempStr), "%s/%s|", pppCon->name, pppCon->X_BROADCOM_COM_IfName);
            strncat(ifcListStr, tempStr, sizeof(ifcListStr)-1);
         }
         cmsObj_free((void **)&pppCon);
         continue;
      }

      /*  skip on
       * 1). if needEoA is TRUE but it is PPPoA (not EoA)
       * 2). if needEoA is TRUE and the wan interface is already in the interface group (not br0)
       */
      if ((needEoAIntf && dalWan_isEoAInterface(&iidStack)) ||
          (needEoAIntf && (dalPMap_isWanUsedForIntfGroup(pppCon->X_BROADCOM_COM_IfName) == TRUE)))
      {
         cmsObj_free((void **)&pppCon);
         continue;
      }

#ifdef DMP_X_BROADCOM_COM_IPV6_1
          /* FIXME IPV6 */
      /* skip on
       * 1) if needIPv6 is TRUE but it is not ipv6 enabled.
       */
      if (needIPv6 && !pppCon->X_BROADCOM_COM_IPv6Enabled)
      {
         cmsObj_free((void **)&pppCon);
         continue;
      }
#endif
      snprintf(tempStr, sizeof(tempStr), "%s/%s|", pppCon->name, pppCon->X_BROADCOM_COM_IfName);
      strncat(ifcListStr, tempStr, sizeof(ifcListStr)-1);
      cmsObj_free((void **)&pppCon);
   }

   if (firewallEnabledInfo)
   {
      InstanceIdStack lanIidStack = EMPTY_INSTANCE_ID_STACK;
      LanIpIntfObject *ipIntfObj=NULL;

      while ((ret = cmsObj_getNext(MDMOID_LAN_IP_INTF, &lanIidStack, (void **)&ipIntfObj)) == CMSRET_SUCCESS)
      {

         if (ipIntfObj->X_BROADCOM_COM_FirewallEnabled)
         {
            /*
             * The second name really should be from the layer 2 bridging
             * bridge entry object, which has the bridge name/interface group name.
             * But for now, just repeat the bridge ifName again.
             */
            snprintf(tempStr, sizeof(tempStr), "%s/%s|",  ipIntfObj->X_BROADCOM_COM_IfName,
                                                          ipIntfObj->X_BROADCOM_COM_IfName);
            strncat(ifcListStr, tempStr, sizeof(ifcListStr)-1);
         }
         cmsObj_free((void **)&ipIntfObj);
      }
   }

   /* for checking interfaces with firewall enabled or bridged PVCs */
   if (firewallEnabledInfo == TRUE || bridgeIfcInfo == TRUE)
   {
      if (ifcListStr[0] != '\0')
      {
         strcat(varValue, ifcListStr);
      }  

      int len = strlen(varValue);
      if (len > 0)
      {
         varValue[len-1] = '\0';   /* remove the last '|' character   */
      }
      
      return;
   }

   /* defautl gateway string first, currIfc second, and then the list from ip/ppp objects */
   if (defaultGwStr[0] != '\0')
   {
      strcpy(varValue, defaultGwStr);
   }

   if (currIfc[0] != '\0')
   {
      strcat(varValue, currIfc);
   }  

   /* append the No Interface for multiSubnet */
   if (needEoAIntf == TRUE)
   {
      strncat(ifcListStr, "No Interface/None|", sizeof(ifcListStr));      
   }
   
   if (ifcListStr[0] != '\0')
   {
      strcat(varValue, ifcListStr);
   }  

      
   int len = strlen(varValue);
   if (len > 0)
   {
      varValue[len-1] = '\0';   /* remove the last '|' character   */
   }
   
   cmsLog_debug("Wan interface list: %s", varValue);
   
}



/* cgiGetWanInterfaceInfo_dev2 is in cgi2_wan.c */
void cgiGetWanInterfaceInfo_dev2(int argc __attribute__((unused)), char **argv, char *varValue);

void cgiGetWanInterfaceInfo(int argc __attribute__((unused)), char **argv, char *varValue) 
{
#if defined(SUPPORT_DM_LEGACY98)
    cgiGetWanInterfaceInfo_igd(argc, argv, varValue);
#elif defined(SUPPORT_DM_HYBRID)
    cgiGetWanInterfaceInfo_igd(argc, argv, varValue);
#elif defined(SUPPORT_DM_PURE181)
    cgiGetWanInterfaceInfo_dev2(argc, argv, varValue);
#elif defined(SUPPORT_DM_DETECT)
   if (cmsMdm_isDataModelDevice2())
   {
      cgiGetWanInterfaceInfo_dev2(argc, argv, varValue);
   }     
   else
   {
      cgiGetWanInterfaceInfo_igd(argc, argv, varValue);
   }
#endif
}


#ifdef SUPPORT_TR69C
void cgiGetTr69cInterfaceList_igd(int argc __attribute__((unused)), char **argv __attribute__((unused)), char *varValue) 
{
   WanPppConnObject *pppConnObj = NULL;   
   WanIpConnObject  *ipConnObj = NULL;
   InstanceIdStack iidStack;
   CmsRet ret;

#ifdef OMCI_TR69_DUAL_STACK
   ManagementServerObject *acsCfg = NULL;
   BcmOmciRtdIpHostConfigDataExtObject *bcmIpHost = NULL;
   UBOOL8 found = FALSE;

   ret = CMSRET_OBJECT_NOT_FOUND;
   if (cmsObj_get(MDMOID_MANAGEMENT_SERVER, 
                  &iidStack, 0, (void *) &acsCfg) == CMSRET_SUCCESS)
   {
#ifdef SUPPORT_IPV6
       BcmOmciRtdIpv6HostConfigDataExtObject *bcmIpv6Host = NULL;

       INIT_INSTANCE_ID_STACK(&iidStack);
       // get bound interface name
       while (!found && 
              cmsObj_getNext(MDMOID_BCM_OMCI_RTD_IPV6_HOST_CONFIG_DATA_EXT,
                              &iidStack, (void **) &bcmIpv6Host) == CMSRET_SUCCESS)
       {
           found = (cmsUtl_strcmp(bcmIpv6Host->interfaceName, acsCfg->X_BROADCOM_COM_BoundIfName) == 0);
           cmsObj_free((void **) &bcmIpv6Host);
       }
#endif

       INIT_INSTANCE_ID_STACK(&iidStack);
       while(!found && 
             cmsObj_getNext(MDMOID_BCM_OMCI_RTD_IP_HOST_CONFIG_DATA_EXT,
                               &iidStack, (void **) &bcmIpHost) == CMSRET_SUCCESS)
       {
           found = (cmsUtl_strcmp(bcmIpHost->interfaceName, acsCfg->X_BROADCOM_COM_BoundIfName) == 0);
           cmsObj_free((void **) &bcmIpHost);
       }

       if (found)
           sprintf(varValue, "%s", acsCfg->X_BROADCOM_COM_BoundIfName);      

       cmsObj_free((void **) &acsCfg);
   }

   if (found)
   {
       strcat(varValue, "|");
       strcat(varValue, MDMVS_ANY_WAN);
   }
   else
#endif   
   sprintf(varValue, "%s", MDMVS_ANY_WAN);

   /*
    * Find any routed Layer 3 interface and add it to the list.
    */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_WAN_IP_CONN, &iidStack, (void **) &ipConnObj)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(ipConnObj->connectionType, MDMVS_IP_ROUTED))
      {
         strcat(varValue, "|");
         strcat(varValue, ipConnObj->X_BROADCOM_COM_IfName);
      }

      cmsObj_free((void **) &ipConnObj);
   }


   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((ret = cmsObj_getNext(MDMOID_WAN_PPP_CONN, &iidStack, (void **) &pppConnObj)) == CMSRET_SUCCESS)
   {
      strcat(varValue, "|");
      strcat(varValue, pppConnObj->X_BROADCOM_COM_IfName);

      cmsObj_free((void **) &pppConnObj);
   }


   strcat(varValue, "|");
   strcat(varValue, MDMVS_LAN);
   strcat(varValue, "|");
   strcat(varValue, MDMVS_LOOPBACK);

   cmsLog_debug("varValue=%s", varValue);
}


/* cgiGetTr69cInterfaceList_dev2 is in cgi2_wan.c */
void cgiGetTr69cInterfaceList_dev2(int argc __attribute__((unused)), char **argv, char *varValue);


void cgiGetTr69cInterfaceList(int argc __attribute__((unused)), char **argv __attribute__((unused)), char *varValue) 
{
#if defined(SUPPORT_DM_LEGACY98)
    cgiGetTr69cInterfaceList_igd(argc, argv, varValue);
#elif defined(SUPPORT_DM_HYBRID)
    cgiGetTr69cInterfaceList_igd(argc, argv, varValue);
#elif defined(SUPPORT_DM_PURE181)
    cgiGetTr69cInterfaceList_dev2(argc, argv, varValue);
#elif defined(SUPPORT_DM_DETECT)
   if (cmsMdm_isDataModelDevice2())
   {
      cgiGetTr69cInterfaceList_dev2(argc, argv, varValue);
   }     
   else
   {
      cgiGetTr69cInterfaceList_igd(argc, argv, varValue);
   }
#endif
}

#endif /* SUPPORT_TR69C */


/* for the display L3IfName on web/cli only */
void cgiFormLayer3IfName(char *L3IfName)
{
   char tempL3IfName[CMS_IFNAME_LENGTH]={0};
   char pppName[CMS_IFNAME_LENGTH];
   UBOOL8 isPPPoE = TRUE;
   
   if (L3IfName == NULL)
   {
      cmsLog_error("L3IfName is NULL");
      return;
   }
   
#ifdef SUPPORT_CELLULAR
   /* Cellular wan use same inteface for L2 and L3 */
   if (cmsUtl_strstr(glbWebVar.wanL2IfName, CELLULAR_IFC_STR))
   {
      strcpy(L3IfName, glbWebVar.wanL2IfName);
      return;
   }  
#endif

   strcpy(tempL3IfName, glbWebVar.wanL2IfName);

   if  (cmsUtl_strstr(tempL3IfName, ETH_IFC_STR) || 
      cmsUtl_strstr(tempL3IfName, MOCA_IFC_STR) ||
      cmsUtl_strstr(glbWebVar.wanL2IfName, GPON_IFC_STR) ||
      cmsUtl_strstr(glbWebVar.wanL2IfName, EPON_IFC_STR) ||
      cmsUtl_strstr(tempL3IfName, WLAN_IFC_STR))
   {
      /* get default ppp device name */
      if (glbWebVar.ntwkPrtcl == CMS_WAN_TYPE_ETHERNET_PPPOE)
      {
         if (dalWan_fillPppIfName(isPPPoE, pppName) != CMSRET_SUCCESS)
         {
            return;
         }
      }  
      /* for IPoE, layer 2 and layer 3 are same ethernet as wan */
   }
   else /* ATM/PTM interface */
   {
      /* make tempL3IfName look like "ptm0" */
      char *p = strchr(tempL3IfName, '/');
      if (p == NULL)
      {
         cmsLog_error(" glbWebVar.wanL2IfName %s wrong format",  glbWebVar.wanL2IfName);
         return;
      }
      *p = '\0';

      /* get default ppp device name */
      if (glbWebVar.ntwkPrtcl == CMS_WAN_TYPE_PPPOA)
      {
         isPPPoE = FALSE;
      }
      if (dalWan_fillPppIfName(isPPPoE, pppName) != CMSRET_SUCCESS)
      {
         return;
      }
   }
   
   switch (glbWebVar.connMode)
   {
   case CMS_CONNECTION_MODE_DEFAULT:
      if (glbWebVar.ntwkPrtcl == CMS_WAN_TYPE_PPPOA ||
          glbWebVar.ntwkPrtcl == CMS_WAN_TYPE_PPPOE ||
          glbWebVar.ntwkPrtcl == CMS_WAN_TYPE_ETHERNET_PPPOE)
      {
         strcpy(L3IfName, pppName);
      }
      else
      {
         /* for other mode, layer 2 and layer 3 ifName is same */
         strcpy(L3IfName, tempL3IfName);
      }
      break;
      
   case CMS_CONNECTION_MODE_VLANMUX:
      {
         SINT32 conId;
         if (cmsDal_getAvailableConIdForMSC(tempL3IfName, &conId) != CMSRET_SUCCESS)
         {
            return;
         }
         if (glbWebVar.ntwkPrtcl == CMS_WAN_TYPE_PPPOE ||
            glbWebVar.ntwkPrtcl == CMS_WAN_TYPE_ETHERNET_PPPOE)
         {
            sprintf(L3IfName, "%s.%d", pppName, conId);
         }
         else
         {
#ifdef SUPPORT_WIFIWAN
            if(cmsUtl_strstr(tempL3IfName, WLAN_IFC_STR))
               sprintf(L3IfName, "wifi%d",atoi(&tempL3IfName[2]));
            else
#endif
               sprintf(L3IfName, "%s.%d", tempL3IfName, conId);
         }
      }
      break;

   default:
      cmsLog_error("Wrong connMode=%d", glbWebVar.connMode);
   }
}

/* for the display WAN service name on web/cli only */
void cgiFormServiceName(char *serviceName)
{
   char serviceNameSufix[CMS_IFNAME_LENGTH];
   char *p;
   
   /* wanL2IfName looks like ptm0/(0_0_1) or atm1/(0_0_35) */
   if ((p = strchr(glbWebVar.wanL2IfName, '(')))
   {
      strncpy(serviceNameSufix, p+1, CMS_IFNAME_LENGTH-1);
      p = strchr(serviceNameSufix, ')');
      *p = '\0';
      /* serviceNameSufix looks like 0_0_35 now */
   }
   else if (cmsUtl_strstr(glbWebVar.wanL2IfName, ETH_IFC_STR) || 
      cmsUtl_strstr(glbWebVar.wanL2IfName, MOCA_IFC_STR) ||
      cmsUtl_strstr(glbWebVar.wanL2IfName, GPON_IFC_STR) ||
      cmsUtl_strstr(glbWebVar.wanL2IfName, EPON_IFC_STR) ||
      cmsUtl_strstr(glbWebVar.wanL2IfName, WLAN_IFC_STR))
   {
      strcpy(serviceNameSufix, glbWebVar.wanL2IfName);
   }
   else
   {
      cmsLog_error("wrong wanL2IfName=%s", glbWebVar.wanL2IfName);
      return;
   }

   switch ((CmsWanConnectionType) glbWebVar.ntwkPrtcl)
   {
   case CMS_WAN_TYPE_DYNAMIC_IPOE:
   case CMS_WAN_TYPE_DYNAMIC_ETHERNET_IP:
      /* eg. get  ipoe_0_2_35 */
      sprintf(serviceName, "%s_%s", IPOE_IFC_STR,  serviceNameSufix);
      break;
     
   case CMS_WAN_TYPE_IPOA:
      sprintf(serviceName, "%s_%s", IPOA_IFC_STR,  serviceNameSufix);
      break;
     
   case CMS_WAN_TYPE_PPPOE:
   case CMS_WAN_TYPE_ETHERNET_PPPOE:
      sprintf(serviceName, "%s_%s", PPPOE_IFC_STR,  serviceNameSufix);
      break;
     
   case CMS_WAN_TYPE_PPPOA:
      sprintf(serviceName, "%s_%s", PPPOA_IFC_STR,  serviceNameSufix);
      break;
     
   case CMS_WAN_TYPE_BRIDGE:
   case CMS_WAN_TYPE_ETHERNET_BRIDGE:
      sprintf(serviceName, "%s_%s", BRIDGE_IFC_STR,  serviceNameSufix);
      break;

   default:
      cmsLog_error("Wrong protocol = %d", glbWebVar.ntwkPrtcl);
      break;
      
   }

   if (glbWebVar.connMode == CMS_CONNECTION_MODE_VLANMUX)
   {
      if (glbWebVar.vlanMuxId >= 0)
      {
         sprintf(serviceName, "%s.%d", serviceName, glbWebVar.vlanMuxId);
      }
   }

   cmsLog_debug("serviceName=%s", serviceName);
      
}  /* End of cgiFormServiceName() */





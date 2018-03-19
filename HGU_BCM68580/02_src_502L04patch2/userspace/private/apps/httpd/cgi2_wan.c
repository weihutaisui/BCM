/***********************************************************************
 *
 *  Copyright (c) 2013  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2013:proprietary:standard

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
#ifdef DMP_DEVICE2_BASELINE_1

/* DMP_DEVICE2_BRIDGE_1 is only in PURE__181 build, not in Hybrid */
#ifdef DMP_DEVICE2_BRIDGE_1

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
#include "cms_qdm.h"
#include "cms_boardcmds.h"
#include "cgi_util.h"
#include "cgi_sts.h"

/* in cgi_wan.c.  TODO: later on need a cgi_wan_util.c for shared functions between cgi_wan.c cgi2_wan.c */

extern void writeWanSrvcScript(FILE *fs, const WanIfcType iftype);




UBOOL8 cgiWan_getInfoFromWanL2IfName_dev2(const char *wanL2IfName,
                                          WanIfcType *iftype, SINT32 *connMode)
{
   UBOOL8 found = FALSE;

   /* For now, TR18 connMode is always vlanMux */
   *connMode = CMS_CONNECTION_MODE_VLANMUX;

#ifdef DMP_X_BROADCOM_COM_GPONWAN_1
   if (cmsUtl_strstr(wanL2IfName, GPON_IFC_STR))
   {
       *iftype = WAN_IFC_ETH;
       found = TRUE;
   }
#endif

#ifdef DMP_X_BROADCOM_COM_EPONWAN_1
   if (cmsUtl_strstr(wanL2IfName, EPON_IFC_STR))
   {
       *iftype = WAN_IFC_ETH;
       found = TRUE;
   }
#endif

#ifdef DMP_DEVICE2_ETHERNETINTERFACE_1
   if (cmsUtl_strstr(wanL2IfName, ETH_IFC_STR))
   {
      /* do we actually need to look through the EthernetInterface objects
       * and confirm a match with the wanL2IfName?
       */
      *iftype = WAN_IFC_ETH;
      found = TRUE;
   }
#endif

#ifdef DMP_X_BROADCOM_COM_MOCAWAN_1
   if (cmsUtl_strstr(wanL2IfName, MOCA_IFC_STR))
   {
      /* do just like _igd code and set to ETH */
      *iftype = WAN_IFC_ETH;
      found=TRUE;
   }
#endif

   /* XXX TODO: Support Wifi WAN */


#ifdef DMP_DEVICE2_DSL_1
   if (!found)
   {
      if (cmsUtl_strstr(glbWebVar.wanL2IfName, IPOA_IFC_STR))
      {
         *iftype = WAN_IFC_IPOA;
         found = TRUE;
         *connMode = CMS_CONNECTION_MODE_DEFAULT;
      }
      else if (cmsUtl_strstr(glbWebVar.wanL2IfName, ATM_IFC_STR))
      {
          InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
          Dev2AtmLinkObject *atmLink=NULL;
          
          while (!found &&
                 cmsObj_getNext(MDMOID_DEV2_ATM_LINK, &iidStack, (void **) &atmLink) == CMSRET_SUCCESS)
          {
             if (!cmsUtl_strcmp(atmLink->name, wanL2IfName))
             {
                found = TRUE;
                if (cmsUtl_strcmp(atmLink->linkType, MDMVS_PPPOA) == 0)
                {
                   *iftype = WAN_IFC_PPPOA;
                   *connMode = cmsUtl_connectionModeStrToNum(atmLink->X_BROADCOM_COM_ConnectionMode);
                }
                else
                {
                   *iftype = WAN_IFC_ATM;
                   *connMode = cmsUtl_connectionModeStrToNum(atmLink->X_BROADCOM_COM_ConnectionMode);
                }
             }
             cmsObj_free((void**) &atmLink);
          }
      }
#ifdef DMP_DEVICE2_PTMLINK_1
      else if (cmsUtl_strstr(wanL2IfName, PTM_IFC_STR))
      {
         InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
         Dev2PtmLinkObject *ptmLink=NULL;

         while (!found &&
                cmsObj_getNext(MDMOID_DEV2_PTM_LINK, &iidStack, (void **) &ptmLink) == CMSRET_SUCCESS)
         {
            if (!cmsUtl_strcmp(ptmLink->name, wanL2IfName))
            {
               found = TRUE;
               *iftype = WAN_IFC_PTM;
            }
            cmsObj_free((void**) &ptmLink);
         }
      }
#endif  /* DMP_DEVICE2_PTMLINK_1 */
   }
#endif  /* DMP_DEVICE2_DSL_1 */

   return found;
}


/* in cgi_wan.c */
void writeWanCfgScript(FILE *fs);
void cgiWanServiceDisplay(FILE *fs, char *srvcStr, char *ifcStr, char *protoStr, SINT32 vlanId, SINT32 vlanPr, UINT32 vlanTpid,
               UBOOL8 ipv6Enabled, UBOOL8 igmpEnabled, UBOOL8 igmpSourceEnabled, UBOOL8 natEnabled, UBOOL8 fwEnabled, 
               UBOOL8 mldEnabled, UBOOL8 mldSourceEnabled);
               
void cgiWanCfgView_dev2(FILE *fs) 
{
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   Dev2IpInterfaceObject *ipIntf=NULL;
   SINT32 vlanId, vlanPr;
   UINT32 vlanTpid = 0;
   char serviceStr[BUFLEN_32];
   char interfaceStr[BUFLEN_32];
   char protocalStr[BUFLEN_16];
   UBOOL8 fwEnabled;
   UBOOL8 natEnabled;
   UBOOL8 igmpProxyEnabled=FALSE;
   UBOOL8 igmpSourceEnabled=FALSE;
   UBOOL8 ipv6Enabled=FALSE;
   UBOOL8 mldProxyEnabled=FALSE;
   UBOOL8 mldSourceEnabled=FALSE;
   UBOOL8 PPPIPoA=FALSE;   
//later   SINT32 wanCount = 0;

   cmsLog_debug(" Enter");
   
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


   while (cmsObj_getNext(MDMOID_DEV2_IP_INTERFACE, &iidStack, (void **)&ipIntf) == CMSRET_SUCCESS)
   {
      cmsLog_debug(" ipIntf->name %s, ipIntf->X_BROADCOM_COM_Upstream %d", ipIntf->name, ipIntf->X_BROADCOM_COM_Upstream);

      if (!ipIntf->X_BROADCOM_COM_Upstream)
      {
         /* skip non wan ip interfaces */
         cmsObj_free((void **)&ipIntf);  
         continue;
      }

      {
         /* form serviceStr (br_0_2_35) string XXX should not use alias*/
         snprintf(serviceStr, sizeof(serviceStr), "%s",  ipIntf->alias? ipIntf->alias: "(null)");
         /* form interfaceStr (ptm0) */
         snprintf(interfaceStr, sizeof(interfaceStr), "%s", (ipIntf->name ? ipIntf->name : "(null)"));

         /* form protocolStr */
         if (ipIntf->X_BROADCOM_COM_BridgeService)
         {
            strcpy(protocalStr, BRIDGE_PROTO_STR);
         }
         else if (strstr(ipIntf->lowerLayers,"Device.PPP."))  // a bit of a hack, but it works
         {
            if(strstr(ipIntf->name,PPPOA_IFC_STR))
            {
                strcpy(protocalStr, PPPOA_PROTO_STR);
                PPPIPoA=TRUE;
            }
            else
            strcpy(protocalStr, PPPOE_PROTO_STR);
         }
         else
         {
            if (strstr(ipIntf->lowerLayers,"Device.ATM."))
            {
                strcpy(protocalStr, IPOA_PROTO_STR);
                PPPIPoA=TRUE;                
            }else
            strcpy(protocalStr, IPOE_PROTO_STR);
         }

         /* ip interface name is same as ethernet.vlanterminatione name */
         if(PPPIPoA == FALSE)
         {
         vlanId =  qdmVlan_getVlanIdByIntfNameLocked_dev2(ipIntf->name);
         vlanPr = qdmVlan_getVlan801pByIntfNameLocked_dev2(ipIntf->name);
         vlanTpid = qdmVlan_getVlanTPIDByIntfNameLocked_dev2(ipIntf->name);
         }else
         {   /* For PPPoA/IPoA , not vlan support... */
             vlanId = -1;
             vlanPr = -1;
             vlanTpid = -1;
         }
         
         fwEnabled = ipIntf->X_BROADCOM_COM_FirewallEnabled;
         natEnabled = qdmIpIntf_isNatEnabledOnIntfNameLocked(ipIntf->name);
         
#ifdef DMP_X_BROADCOM_COM_IGMP_1
         igmpProxyEnabled = ipIntf->X_BROADCOM_COM_IGMPEnabled;
         igmpSourceEnabled = ipIntf->X_BROADCOM_COM_IGMP_SOURCEEnabled;
#endif

#ifdef SUPPORT_IPV6
         ipv6Enabled = ipIntf->IPv6Enable;
#ifdef DMP_X_BROADCOM_COM_MLD_1
         mldProxyEnabled = ipIntf->X_BROADCOM_COM_MLDEnabled;
         mldSourceEnabled = ipIntf->X_BROADCOM_COM_MLD_SOURCEEnabled;
#endif
#endif
         cgiWanServiceDisplay(fs, serviceStr, interfaceStr, protocalStr, vlanId, vlanPr, vlanTpid,
            ipv6Enabled,
            igmpProxyEnabled, igmpSourceEnabled,
            natEnabled, fwEnabled,
            mldProxyEnabled, mldSourceEnabled);

         cmsObj_free((void **)&ipIntf);
      }
   }  
 
   fprintf(fs, "</table><br><br>\n");

/* later
   if (dalWan_isAdvancedDmzEnabled() == TRUE || dalWan_isPPPIpExtension() || wanCount >= IFC_WAN_MAX)
   {
      fprintf(fs, "<input type='button' onClick='addClick()' value='Add' disabled='1'>\n");
   }
   else
*/   
   {
      fprintf(fs, "<input type='button' onClick='addClick()' value='Add'>\n");
   }
   
   fprintf(fs, "<input type='button' onClick='removeClick(this.form.rml)' value='Remove'>\n");
   fprintf(fs, "</center>\n</form>\n</blockquote>\n</body>\n</html>\n");

   fflush(fs);

   cmsLog_debug(" Exit");

   
}


CmsRet getAvailableConId_dev2(SINT32 *outConId)
{
   Dev2IpInterfaceObject *ipIntf=NULL;
   CmsRet ret = CMSRET_SUCCESS;
   SINT32 conIdIndex=1;
   SINT32 conIdArray[IFC_WAN_MAX+1] = {0};
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   
   /* need go thru to find out all the connection Id used in ip and ppp connection objects */
   while (cmsObj_getNextFlags(MDMOID_DEV2_IP_INTERFACE, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&ipIntf) == CMSRET_SUCCESS)
   {
      char *p;
      SINT32 conId = 0;
      if (ipIntf->X_BROADCOM_COM_Upstream)
      {
         /* Mark  WAN IP intf  to 1 in the array for later use.  */
         p = strchr(ipIntf->name, '.');
         if (p)
         {
            if (isdigit(*(p+1)))
            {
               conId = atoi(p+1);
            }            
            cmsLog_debug(" conId %d", conId);
            if (conId > IFC_WAN_MAX)
            {
               cmsLog_debug(" Max conId is %d, current conId %d", IFC_WAN_MAX, conId);            
            }
            else
            {
               conIdArray[conId] = 1;
            }
         }
         else
         {
            cmsLog_debug(" Wrong ifName %s (no '.' and digit?)", ipIntf->name);
         }
      }         
      cmsObj_free((void **) &ipIntf);
   }

   for (conIdIndex = 0; conIdIndex <= IFC_WAN_MAX; conIdIndex++)
   {
      cmsLog_debug("conIdArray[%d]=%d", conIdIndex, conIdArray[conIdIndex]);
   }

   /* connection id starts at 1 */
   for (conIdIndex = 1; conIdIndex <= IFC_WAN_MAX; conIdIndex++)
   {
      if (conIdArray[conIdIndex] == 0)
      {
         cmsLog_debug("found available conIdIn=%d", conIdIndex);
         break;
      }
   }

   *outConId = conIdIndex;
   
   return ret;

}






/* for the display L3IfName on web/cli only */
/* miwang TODO: this looks the same as cgiFormLayer3IfName, merge them? */
void cgiFormLayer3IfName_dev2(char *L3IfName)
{
   char tempL3IfName[CMS_IFNAME_LENGTH]={0};
   char pppName[CMS_IFNAME_LENGTH];
   UBOOL8 isPPPoE = TRUE;
   
   if (L3IfName == NULL)
   {
      cmsLog_error("L3IfName is NULL");
      return;
   }

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
      {// XXX TODO ppp? later
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
         if (getAvailableConId_dev2(&conId) != CMSRET_SUCCESS)
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

#ifdef DMP_X_BROADCOM_COM_OPENVSWITCH_1
extern UBOOL8 rutOpenVS_isEnabled(void);
extern UBOOL8 rutOpenVS_isOpenVSPorts(const char *ifName);
#endif

/* in cgi_wan.c */
void cgiFormServiceName(char *serviceName);

void cgiGetWanInterfaceInfo_dev2(int argc __attribute__((unused)), char **argv, char *varValue) 
{
   char currIfc[BUFLEN_64];
   char serviceName[BUFLEN_32];
   char defaultGwStr[BUFLEN_32];
   char tempStr[BUFLEN_64];
   char ifcListStr[BUFLEN_1024]={0};
   UBOOL8 usedForDns __attribute__((unused)) = FALSE;
   UBOOL8 needEoAIntf = FALSE;
   UBOOL8 firewallEnabledInfo = FALSE;
   UBOOL8 bridgeIfcInfo = FALSE;
   InstanceIdStack iidStack;
   Dev2IpInterfaceObject *ipIntf=NULL;

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
      cgiFormLayer3IfName_dev2(tempStr);
      cgiFormServiceName(serviceName);
      snprintf(currIfc, sizeof(currIfc), "%s/%s|", serviceName, tempStr);
    }

    cmsLog_debug("Current ifc = %s, usedForDns = %d", currIfc, usedForDns);      

   
   /* get the related ipIntf obj  XXX need to work on ppp names */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ( cmsObj_getNextFlags(MDMOID_DEV2_IP_INTERFACE, &iidStack, OGF_NO_VALUE_UPDATE, (void **) &ipIntf) == CMSRET_SUCCESS)
   {
#ifdef DMP_X_BROADCOM_COM_OPENVSWITCH_1
      if( (rutOpenVS_isEnabled()) && (rutOpenVS_isOpenVSPorts(ipIntf->name)))
         ;
      else
      {
         if(bridgeIfcInfo)
         {
            if (ipIntf->X_BROADCOM_COM_Upstream  && ipIntf->X_BROADCOM_COM_BridgeService)
            {
              snprintf(tempStr, sizeof(tempStr), "%s/%s|", ipIntf->name, ipIntf->name);
              strncat(ifcListStr, tempStr, sizeof(ifcListStr)-1);
            }
         }
         else if (ipIntf->X_BROADCOM_COM_Upstream || (firewallEnabledInfo && ipIntf->X_BROADCOM_COM_FirewallEnabled))
         {
            snprintf(tempStr, sizeof(tempStr), "%s/%s|", ipIntf->name, ipIntf->name);
            strncat(ifcListStr, tempStr, sizeof(ifcListStr)-1);
         }          
      }
#else	  
      if (ipIntf->X_BROADCOM_COM_Upstream || (firewallEnabledInfo && ipIntf->X_BROADCOM_COM_FirewallEnabled))
      {
          snprintf(tempStr, sizeof(tempStr), "%s/%s|", ipIntf->name, ipIntf->name);
          strncat(ifcListStr, tempStr, sizeof(ifcListStr)-1);
      }          
#endif	  
      cmsObj_free((void **)&ipIntf);
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

void cgiStsWanBody_dev2(char *query __attribute__((unused)), FILE *fs) 
{
   Dev2IpInterfaceObject *ipIntf=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;       

   /* need go thru to find out all the connection Id used in ip and ppp connection objects */
   while (cmsObj_getNextFlags(MDMOID_DEV2_IP_INTERFACE, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&ipIntf) == CMSRET_SUCCESS)
   {
      /* Only interested in WAN interface and the service is up (for both IPv4/IPv6) */
      if (ipIntf->X_BROADCOM_COM_Upstream && 
         qdmIpIntf_isIpvxServiceUpLocked_dev2(ipIntf->name, QDM_IPINTF_DIR_WAN, CMS_AF_SELECT_IPVX))
               
      {
         Dev2IpStatsObject *ipIntfStats=NULL;
         if ((ret = cmsObj_get(MDMOID_DEV2_IP_STATS, &iidStack, 0, (void **)&ipIntfStats)) == CMSRET_SUCCESS)
         {
            cgiWriteWanSts(fs, ipIntf->name, ipIntf->alias,
                           ipIntfStats->bytesReceived, 
                           ipIntfStats->packetsReceived,
                           ipIntfStats->errorsReceived,
                           ipIntfStats->discardPacketsReceived,
                           ipIntfStats->X_BROADCOM_COM_MulticastBytesReceived, 
                           ipIntfStats->multicastPacketsReceived,
                           ipIntfStats->unicastPacketsReceived, 
                           ipIntfStats->broadcastPacketsReceived,
                           ipIntfStats->bytesSent, 
                           ipIntfStats->packetsSent, 
                           ipIntfStats->errorsSent,
                           ipIntfStats->discardPacketsSent, 
                           ipIntfStats->X_BROADCOM_COM_MulticastBytesSent, 
                           ipIntfStats->multicastPacketsSent, 
                           ipIntfStats->unicastPacketsSent, 
                           ipIntfStats->broadcastPacketsSent);    
            cmsObj_free((void **)&ipIntfStats);
         }
         else
         {
            cgiWriteWanSts(fs, ipIntf->name, ipIntf->alias, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);    
            cmsLog_error("Failed to get MDMOID_DEV2_IP_STATS object, ret=%d", ret);
         }         
      }
      cmsObj_free((void **) &ipIntf);
   }
}

void cgiResetStsWan_dev2(void)
{
   Dev2IpInterfaceObject *ipIntf=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   
   /* need go thru all ip interfaces to find out the WAN interface with Service status is up */
   while (cmsObj_getNextFlags(MDMOID_DEV2_IP_INTERFACE, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&ipIntf) == CMSRET_SUCCESS)
   {
      /* Only interested in WAN interface and the service is up (for both IPv4/IPv6) */
      if (ipIntf->X_BROADCOM_COM_Upstream && 
          qdmIpIntf_isIpvxServiceUpLocked_dev2(ipIntf->name, QDM_IPINTF_DIR_WAN, CMS_AF_SELECT_IPVX))
      {
         cmsObj_clearStatistics(MDMOID_DEV2_IP_STATS, &iidStack);
      }      
      cmsObj_free((void **) &ipIntf);
   }
  
}



#ifdef SUPPORT_TR69C
void cgiGetTr69cInterfaceList_dev2(int argc __attribute__((unused)), char **argv __attribute__((unused)), char *varValue)
{
   Dev2IpInterfaceObject *ipIntf=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

#ifdef OMCI_TR69_DUAL_STACK
   Dev2ManagementServerObject *acsCfg = NULL;
   BCM_IpHostConfigDataObject *bcmIpHost = NULL;
   UBOOL8 found = FALSE;

   ret = CMSRET_OBJECT_NOT_FOUND;
   if (cmsObj_get(MDMOID_DEV2_MANAGEMENT_SERVER, 
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

   INIT_INSTANCE_ID_STACK(&iidStack);
   
   /* need go thru all ip interfaces to find out the WAN interface */
   while (cmsObj_getNextFlags(MDMOID_DEV2_IP_INTERFACE, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&ipIntf) == CMSRET_SUCCESS)
   {
      /* Only interested in routed WAN interface */
      if (ipIntf->X_BROADCOM_COM_Upstream && 
          !ipIntf->X_BROADCOM_COM_BridgeService)
      {
         cmsUtl_strcat(varValue, "|");
         cmsUtl_strcat(varValue, ipIntf->name);
      }      
      cmsObj_free((void **) &ipIntf);
   }

   cmsUtl_strcat(varValue, "|");
   cmsUtl_strcat(varValue, MDMVS_LAN);
   cmsUtl_strcat(varValue, "|");
   cmsUtl_strcat(varValue, MDMVS_LOOPBACK);

   cmsLog_debug("varValue=%s", varValue);
   
}

#endif /* SUPPORT_TR69C */

#endif /* DMP_DEVICE2_BRIDGE_1 */

#endif /* DMP_DEVICE2_BASELINE_1 */



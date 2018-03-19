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

#include "cgi_cmd.h"
#include "cgi_main.h"
#include "cgi_ntwk.h"
#include "cgi_qos.h"
#include "cgi_route.h"
#include "cgi_eng.h"
#include "cgi_dump.h"

#ifdef SUPPORT_IPV6
#include "cgi_lan6.h"
#endif
#ifdef SUPPORT_IPP
#include "cgi_ipp.h"
#endif
#ifdef DMP_X_BROADCOM_COM_DLNA_1
#include "cgi_dlna.h"
#endif
#ifdef DMP_X_ITU_ORG_GPON_1
#include "cgi_omci.h"
#endif
#ifdef SUPPORT_CELLULAR
#include "cgi_cellular.h"
#endif
#ifdef DMP_X_BROADCOM_COM_PWRMNGT_1
#include "cgi_pwrmngt.h"
#endif /* aka SUPPORT_PWRMNGT */

#ifdef DMP_X_BROADCOM_COM_STANDBY_1
#include "cgi_standby.h"
#endif

#if defined(DMP_X_BROADCOM_COM_IGMP_1) || defined(DMP_X_BROADCOM_COM_MLD_1) || defined(DMP_X_BROADCOM_COM_MCAST_1)
#include "cgi_multicast.h"
#endif 

#ifdef DMP_X_BROADCOM_COM_ETHERNETOAM_1
#include "cgi_diag.h"
#endif

#ifdef DMP_X_BROADCOM_COM_NFC_1
#include "cgi_nfc.h"
#endif

#if defined(DMP_X_BROADCOM_COM_RDPA_1)
#include "cgi_ingress_filters.h"
#endif

CGI_CMD WebCmdTable[] = {
   { "arpview", cgiArpView, LOGIN_USER},
   { "pmdbackupsettings", cgiBackupPmdSettings, LOGIN_SUPPORT}, /* needs to be before "backupsettings" entry because of strstr below */
   { "backupsettings", cgiBackupSettings, LOGIN_SUPPORT},
   { "pmdsettings", cgiPmdSettings, LOGIN_SUPPORT},
   { "rtroutecfg", cgiRtRouteCfg, LOGIN_SUPPORT},
#ifdef SUPPORT_WAN_HTML
   { "seclogreset", cgiSecLogReset, LOGIN_SUPPORT},
   { "security_log", cgiSecLogText, LOGIN_SUPPORT },
   { "seclogview", cgiSecLogView, LOGIN_SUPPORT }, /* needs to be before "logview" entry because of strstr below */
   { "scvrtsrv", cgiScVrtSrv, LOGIN_ADMIN },
   { "scprttrg", cgiScPrtTrg, LOGIN_ADMIN },
#ifndef BUILD_DM_WLAN_RETAIL
   { "scdmz", cgiScDmzHost, LOGIN_ADMIN },
#endif /* BUILD_DM_WLAN_RETAIL */
#endif
   { "logview", cgiLogView, LOGIN_USER }, /* needs to be after "seclogview" entry because of strstr below */
#ifdef DMP_X_BROADCOM_COM_SECURITY_1
   { "scoutflt", cgiScOutFlt, LOGIN_ADMIN },
   { "scinflt", cgiScInFlt, LOGIN_ADMIN },
   { "scmacflt", cgiScMacFlt, LOGIN_ADMIN },
#endif
#ifdef SUPPORT_DSL
   { "dslatm", cgiDslAtmCfg, LOGIN_SUPPORT },
#ifdef SUPPORT_PTM
   { "dslptm", cgiDslPtmCfg, LOGIN_SUPPORT },      
#endif
#endif  
#ifdef SUPPORT_CELLULAR
   { "cellularapn", cgiCellularApnCfg, LOGIN_SUPPORT },      
   { "cellularsms", cgiCellularSmsCfg, LOGIN_SUPPORT },      
#endif
#ifdef SUPPORT_ETHWAN
   { "ethwan", cgiEthWanCfg, LOGIN_SUPPORT },      
#endif
#ifdef DMP_X_BROADCOM_COM_GPONWAN_1
   { "gponwan", cgiGponWanCfg, LOGIN_SUPPORT }, 
#endif
#if defined(DMP_X_BROADCOM_COM_MOCAWAN_1) || defined(DMP_DEVICE2_MOCA_1)
   { "mocawan", cgiMocaWanCfg, LOGIN_SUPPORT }, 
#endif
#ifdef DMP_X_BROADCOM_COM_EPONWAN_1
   { "eponwan", cgiEponWanCfg, LOGIN_SUPPORT }, 
#endif
#ifdef SUPPORT_MOCA
   { "statsmoca", cgiStsMoca, LOGIN_SUPPORT },   
#endif
#ifdef DMP_X_BROADCOM_COM_L2TPAC_1
   { "l2tpacwan", cgiL2tpAcWanCfg, LOGIN_SUPPORT },      
#endif
#ifdef DMP_X_BROADCOM_COM_PPTPAC_1
   { "pptpacwan", cgiPptpAcWanCfg, LOGIN_SUPPORT },      
#endif
#ifdef SUPPORT_QUICKSETUP
   { "quicksetup", cgiAutoAtmCfg, LOGIN_ADMIN },
#endif
#ifdef DMP_STORAGESERVICE_1
   { "storageservicecfg", cgiStorageServiceCfg, LOGIN_SUPPORT },
#ifdef SUPPORT_SAMBA
   { "storageuseraccountcfg", cgiStorageUserAccountCfg, LOGIN_SUPPORT },
#endif
 #endif
#ifdef DMP_X_BROADCOM_COM_AUTODETECTION_1
   { "autodetection", cgiAutoDetection, LOGIN_SUPPORT },
#endif
#ifdef SUPPORT_WAN_HTML
   { "wancfg", cgiWanCfg, LOGIN_SUPPORT },
   { "wanifc", cgiWanIfc, LOGIN_SUPPORT },
   { "wansrvc", cgiWanSrvc, LOGIN_SUPPORT },
   { "wanL3Edit", cgiWanL3Edit, LOGIN_SUPPORT },   
#endif
#ifdef SUPPORT_DSL
   { "statsxtm", cgiStsXtm, LOGIN_USER },
#endif
   { "statswan", cgiStsWan, LOGIN_USER },
   { "adslcfgadv", cgiAdslCfgTestMode, LOGIN_SUPPORT },
   { "adslcfgtone", cgiAdslCfgTones, LOGIN_SUPPORT },
#ifdef SUPPORT_DEBUG_TOOLS
   { "engdebug", cgiEngDebug, LOGIN_SUPPORT },
   { "enginfo", cgiEngInfo, LOGIN_SUPPORT },
   { "dumpcfgdynamic", cgiDumpCfgDync, LOGIN_SUPPORT },
   { "dumpcfg", cgiDumpCfg, LOGIN_SUPPORT },
   { "dumpmdm", cgiDumpMdm, LOGIN_ADMIN },
#endif
#ifdef SUPPORT_WAN_HTML
#ifdef SUPPORT_QOS
   { "qosmgmt", cgiQosMgmt, LOGIN_SUPPORT},
   { "qosqueue", cgiQosQueue, LOGIN_SUPPORT },
   { "qospolicer", cgiQosPolicer, LOGIN_SUPPORT },
   { "qoscls", cgiQosCls, LOGIN_SUPPORT },
#if defined(DMP_X_BROADCOM_COM_RDPA_1)
   { "statsqueuereset", cgiResetStsQueue, LOGIN_SUPPORT},
#endif
#if (defined(SUPPORT_DSL) && defined(SUPPORT_FAPCTL)) || (defined(SUPPORT_RDPA) && (defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_63158) || defined(CHIP_4908))) || defined(SUPPORT_BCMTM)
   { "qosportshaping", cgiQosPortShaping, LOGIN_SUPPORT },
#endif
#endif  /* SUPPORT_QOS */
#endif /* SUPPORT_WAN_HTML */
   { "dhcpdstaticlease", do_dhcpd_cgi, LOGIN_SUPPORT},
#ifdef SUPPORT_POLICYROUTING
   { "prmngr", cgiPolicyRouting, LOGIN_ADMIN },
#endif
#ifdef SUPPORT_URLFILTER
   { "urlfilter", do_urlfilter_cgi, LOGIN_ADMIN },
#endif
#ifdef DMP_X_BROADCOM_COM_DNSPROXY_1 /* aka SUPPORT_DNSPROXY */
   { "dnsProxy", cgiDnsProxyCfg, LOGIN_SUPPORT},
#endif
#if SUPPORT_INTF_GROUPING
   { "portmap", cgiPortMapCfg, LOGIN_ADMIN },
#endif
#if SUPPORT_HOMEPLUG
   { "homeplug", cgiHomePlugCfg, LOGIN_SUPPORT },
#endif
#if defined(SUPPORT_IEEE1905) && defined(DMP_DEVICE2_IEEE1905BASELINE_1)
   { "ieee1905sts", cgiIeee1905Status, LOGIN_ADMIN },
#endif
#if defined(DMP_X_BROADCOM_COM_SPDSVC_1)
   { "speedsvc", cgiSpeedTestCmd, LOGIN_ADMIN},
#endif


/***
 * The following line is commented out in 4.06 release.  Starting from 4.06, "Lan Port" feature is not needed.
 * This feature is intended for 96358GW/48GW/38GW platform only and is not removed from the source tree just in case
 * it is needed later on.
 */
 
/*   { "lanports", cgiLanPortsCfg }, */

#ifdef SUPPORT_RIP
   { "ripcfg", cgiConfigRip, LOGIN_SUPPORT },
#endif
#ifdef BRCM_WLAN
   { "wlmacflt", cgiWlMacFlt, LOGIN_ADMIN },
   { "wlwds", cgiWlWds, LOGIN_SUPPORT },
   { "wlstationlist", cgiWlStationList, LOGIN_SUPPORT },  
#ifdef BCMWAPI_WAI
   { "wlwapias", cgiWlWapiAs, LOGIN_SUPPORT },
   { "wlwapiusr", cgiWlWapiAs, LOGIN_SUPPORT},
#endif /* BCMWAPI_WAI */
#endif /* BRCM_WLAN */
#ifdef BRCM_VOICE_SUPPORT
#ifdef DMP_X_BROADCOM_COM_NTR_1
   /* Voice stop/start for NTR independent of SIP */
   { "voicentrapply",  cgiVoiceApply, LOGIN_SUPPORT},
   { "voicentrstart",  cgiVoiceStart, LOGIN_SUPPORT },
   { "voicentrstop",   cgiVoiceStop, LOGIN_SUPPORT },
   { "voicentrrefresh", cgiVoiceRefresh, LOGIN_SUPPORT },
   { "voicentrrestore", cgiVoiceRestore, LOGIN_SUPPORT },
#endif
#ifdef SIPLOAD
   { "voicesipapply",   cgiVoiceApply, LOGIN_SUPPORT},
   { "voicesipstart",   cgiVoiceStart, LOGIN_SUPPORT},
   { "voicesipstop",    cgiVoiceStop, LOGIN_SUPPORT},
   { "voicerefresh",    cgiVoiceRefresh, LOGIN_SUPPORT},
   { "voicesipsetdefault",  cgiVoiceSetDefault, LOGIN_SUPPORT},
   { "voicesipcctkupload",  cgiVoiceCctkUpload, LOGIN_SUPPORT},
#ifdef DMP_VOICE_SERVICE_2   
   { "voicevoipprofselect",    cgiVoiceVoipProfSelect, LOGIN_SUPPORT},
   { "voicecodecprofselect",   cgiVoiceCodecProfSelect, LOGIN_SUPPORT},
#endif   
#endif
#ifdef DMP_X_BROADCOM_COM_DECTENDPOINT_1
   { "voicedectstart",    cgiDectStart, LOGIN_SUPPORT },
   { "voicedectstop",     cgiDectStop, LOGIN_SUPPORT },
   { "voicedectsetrfid",  cgiDectSetRFID, LOGIN_SUPPORT },
   { "voicedectopenreg",  cgiVoiceDectOpenRegWnd, LOGIN_SUPPORT },
   { "voicedectclosereg", cgiVoiceDectCloseRegWnd, LOGIN_SUPPORT },
   { "voicedectsetac",    cgiVoiceDectSetAc, LOGIN_SUPPORT },
   { "voicedectdelhset",  cgiVoiceDectDelHset, LOGIN_SUPPORT },
   { "voicedectpinghset", cgiVoiceDectPingHset, LOGIN_SUPPORT },
   { "voicedectpingallhset", cgiVoiceDectPingAllHset, LOGIN_SUPPORT },
#endif
#endif
#ifdef SUPPORT_DDNSD
   { "ddnsmngr", cgiDDnsMngr, LOGIN_SUPPORT },
#endif
#ifdef SUPPORT_IPSEC
   { "ipsec", cgiIPSec, LOGIN_ADMIN},
#endif
#ifdef SUPPORT_CERT
   { "certlocal", cgiCertLocal, LOGIN_ADMIN},
   { "certca", cgiCertCA, LOGIN_ADMIN },
#endif
#ifdef SUPPORT_IPV6
   { "ipv6lancfg", cgiIPv6LanCfg, LOGIN_SUPPORT },
   { "tunnelcfg", cgiIpTunnelCfg, LOGIN_SUPPORT },
#endif
#ifdef SUPPORT_IPP
   { "ippcfg", cgiIppCfg, LOGIN_SUPPORT },
#endif
#ifdef DMP_X_BROADCOM_COM_DLNA_1
   { "dlnacfg", cgiDlnaCfg, LOGIN_SUPPORT },
#endif
#ifdef DMP_X_ITU_ORG_GPON_1
#ifdef BRCM_OMCI
   { "omcicfg", cgiOmciCfg, LOGIN_USER },
   { "omcicreate", cgiOmciCreate , LOGIN_SUPPORT},
   { "omcigetnext", cgiOmciGetNext, LOGIN_SUPPORT },
   { "omcimacro", cgiOmciMacro, LOGIN_SUPPORT },
   { "omciraw", cgiOmciRaw, LOGIN_SUPPORT },
#endif // BRCM_OMCI
#endif // DMP_X_ITU_ORG_GPON_1
#ifdef DMP_X_BROADCOM_COM_PWRMNGT_1
   { "pwrmngt", cgiPowerManagement, LOGIN_SUPPORT},
#endif /* aka SUPPORT_PWRMNGT */
#ifdef DMP_X_BROADCOM_COM_STANDBY_1
   { "standby", cgiStandby, LOGIN_SUPPORT},
   { "demostby", cgiStandbyDemo, LOGIN_SUPPORT},
#endif 
#ifdef SUPPORT_MODSW_WEBUI
#ifdef DMP_DEVICE2_SM_BASELINE_1
   { "modSwEE", cgiModSwEE, LOGIN_SUPPORT},
   { "modSwDU", cgiModSwDU, LOGIN_SUPPORT},
   { "modSwEU", cgiModSwEU, LOGIN_SUPPORT},
   { "modSwLogDU", cgiModSwLogDU, LOGIN_SUPPORT},
   { "modSwLogEU", cgiModSwLogEU, LOGIN_SUPPORT},
   { "modSwLogEE", cgiModSwLogEE, LOGIN_SUPPORT},
#endif
#ifdef DMP_DEVICE2_X_BROADCOM_COM_MODSW_LINUXPFP_1
   { "modSwPFP", cgiModSwPFP, LOGIN_SUPPORT},
#endif
#endif 
#if defined(DMP_X_BROADCOM_COM_IGMP_1) || defined(DMP_X_BROADCOM_COM_MLD_1) || defined(DMP_X_BROADCOM_COM_MCAST_1)
   { "multicast", cgiMulticast, LOGIN_SUPPORT},
#endif
#if defined(DMP_X_BROADCOM_COM_MCAST_1)
   { "mcastexception", cgiMulticastException, LOGIN_SUPPORT},
#endif
#ifdef DMP_X_BROADCOM_COM_ETHERNETOAM_1
   { "diagethoam", cgiEthOam, LOGIN_SUPPORT},
#endif
#ifdef SUPPORT_WIFIWAN
   { "wifiwan", cgiWifiWanCfg , LOGIN_SUPPORT},
#endif
#ifdef DMP_X_BROADCOM_COM_SELT_1
   { "seltresult", cgiPrintSeltResult, LOGIN_ADMIN },
   { "seltcfgc", cgiSeltParseSet, LOGIN_ADMIN},
#endif
#ifdef SUPPORT_XMPP
   { "xmppconncfg", cgiXmppConnCfg, LOGIN_SUPPORT },      
#endif
#ifdef DMP_X_BROADCOM_COM_NFC_1
   { "nfc", cgiNfcCfg, LOGIN_SUPPORT },
#endif
#ifdef DMP_DEVICE2_USBHOSTSBASIC_1
   { "usbhosts", cgiUsbHostsCfg, LOGIN_SUPPORT },      
#endif
#ifdef DMP_X_BROADCOM_COM_EPON_1
   { "eponloid", cgiEponLoidCfg, LOGIN_SUPPORT},
#endif
#if defined(DMP_X_BROADCOM_COM_RDPA_1)
   { "ingressfilters", cgiSetIngressFiltersCfg, LOGIN_SUPPORT},
#endif
#ifdef DMP_X_BROADCOM_COM_OPENVSWITCH_1
   { "openVS", cgiOpenVSCfg, LOGIN_SUPPORT},
#endif
};

#define NUM_WEB_CMDS (sizeof(WebCmdTable) / sizeof(CGI_CMD))

/* 
 *  Array of files with .html extension and login level for checking the page authentication 
 *  NOTE: If new pages with .html are added, this table needs to be updated for the proper 
 *        login assocation.
 */
CGI_HtlmPage htmlPageAuthTable[] =
{
   {"info", LOGIN_USER},
   {"lancfg2", LOGIN_SUPPORT},
   {"lanvlancfg", LOGIN_SUPPORT},
   {"ipv6lancfg", LOGIN_SUPPORT},
   {"mocacfg", LOGIN_SUPPORT},
   {"qosqmgmt", LOGIN_SUPPORT},
   {"rtdefaultcfg", LOGIN_SUPPORT},   
   {"adslcfgc", LOGIN_SUPPORT},
   {"xdslcfg", LOGIN_SUPPORT},
   {"dslbondingcfg", LOGIN_SUPPORT},
   {"ippcfg", LOGIN_SUPPORT},
   {"upnpcfg", LOGIN_SUPPORT},   
   {"dnsproxycfg", LOGIN_SUPPORT},
#ifdef DMP_X_BROADCOM_COM_OPENVSWITCH_1
   {"openvswitchcfg", LOGIN_SUPPORT},
#endif
   {"standby", LOGIN_SUPPORT},
   {"pwrmngt", LOGIN_SUPPORT},
   {"bmu", LOGIN_SUPPORT},
   {"wlcfg", LOGIN_SUPPORT},   
   {"wlsecurity", LOGIN_SUPPORT},
   {"wlcfgadv", LOGIN_SUPPORT},
   {"wlses", LOGIN_SUPPORT},
   {"wlwapias", LOGIN_SUPPORT},
   {"voicentr", LOGIN_SUPPORT},
   {"voicesip_basic", LOGIN_SUPPORT},
   {"voicesip_basic", LOGIN_SUPPORT},
   {"voicesip_advanced", LOGIN_SUPPORT},   
   {"voicesip_debug", LOGIN_SUPPORT},
   {"voicentr", LOGIN_SUPPORT},
   {"voicedect", LOGIN_SUPPORT},
   {"backupsettings", LOGIN_SUPPORT},
   {"updatesettings", LOGIN_SUPPORT},   
   {"defaultsettings", LOGIN_SUPPORT},
   {"seclogintro", LOGIN_SUPPORT},
   {"sntpcfg", LOGIN_SUPPORT},
   {"resetrouter", LOGIN_SUPPORT},
   {"password", LOGIN_ADMIN},
   {"scvrtsrv", LOGIN_ADMIN},
   {"scoutflt", LOGIN_ADMIN},
#if defined(DMP_X_BROADCOM_COM_IGMP_1) || defined(DMP_X_BROADCOM_COM_MLD_1) || defined(DMP_X_BROADCOM_COM_MCAST_1)
   { "multicast", LOGIN_ADMIN},
#endif    
#ifdef SUPPORT_TOD
   { "todmngr", LOGIN_ADMIN},
#endif
   {"qsmain", LOGIN_ADMIN},
#if SUPPORT_HOMEPLUG
   { "homeplug", LOGIN_SUPPORT}
#endif

};


#define NUM_HTML_PAGES (sizeof(htmlPageAuthTable) / sizeof(CGI_HtlmPage))


void do_cmd_cgi(char *path, FILE *fs) {
   UINT32 i = NUM_WEB_CMDS;
   char filename[WEB_BUF_SIZE_MAX]={0};
   char *query;
   char* cp = NULL;
   char* ext = NULL;
   char empty = 0;
   cmsLog_debug("path=%s",path);

   cp = strchr(path, '?');
   if ( cp != NULL ) 
   {
      query = cp + 1; // reuse big buffer in httpd.c
      if (*(cp - 4) == '.')
         ext = cp - 4;
      else if (*(cp - 5) == '.')
         ext = cp - 5;
   }
   else
   {
      query = &empty;
      ext = strrchr(path, '.');
   }
   /*
    * Figure out which page the user is requesting.
    */
   //ext = strchr(path, '.');

   if ( ext != NULL ) {
      *ext = '\0';
      strncpy(filename, path, sizeof(filename)-1);

      for (i=0; i < NUM_WEB_CMDS; i++ ) {
         if (strstr(filename, WebCmdTable[i].cgiCmdName))
            break;
      }
   }

   if ( i >= NUM_WEB_CMDS ) {

      // Badly formatted post string or unrecognized page
      cmsLog_error("unsupported filename %s", filename);

      // write header
      fprintf(fs, "<html><head>\n");
      fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
      fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
      fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");

      // write body
      fprintf(fs, "</head>\n<body>\n<blockquote>\n<form>\n");
      fprintf(fs, "<b><font color='blue'>This page is not supported.</font></b><br><br>\n") ;

      fprintf(fs, "<tr>\n");
      fprintf(fs, "<tr>\n");
      fprintf(fs, "<br><br>\n");
   }
   else
   {
      /*
       * Protect against cross site forgery attacks 
       */
      if (1) 
      {
         char tmpBuf[BUFLEN_256]={0};

         cgiGetValueByName(query, "action", tmpBuf);

         if((tmpBuf[0] != '\0') &&
            (strcmp(tmpBuf,"view")) &&
            (strcmp(tmpBuf,"viewcfg")) &&
#ifdef SUPPORT_MAPT
            (strcmp(tmpBuf,"viewmapt")) &&
#endif
            (strcmp(tmpBuf,"view_wlq"))) 
         {
            cgiGetValueByName(query, "sessionKey", tmpBuf);
            cgiUrlDecode(tmpBuf);
            cgiSetVar("sessionKey", tmpBuf);

            if(cgiValidateSessionKey(fs) != CMSRET_SUCCESS)
            {
               /*sessionkey validation failed*/
               return;
            }

         }
      }

      /*
       * Call the cmd function handler.
       */
      cmsLog_debug("query = %s", query);
      (*(WebCmdTable[i].cgiCmdHdlr))(query, fs);
   }
}

static UBOOL8 specialCaseHandle(char *fileName, char *query)
{
   UBOOL8 isSpecialCase = FALSE;
   if ((!cmsUtl_strcmp(fileName, "wancfg") ||
         !cmsUtl_strcmp(fileName, "rtroutecfg"))
      && !cmsUtl_strcmp("action=view", query))
   {
      /* special case where filename is "wancfg.cmd?action=view" OR "rtroutecfg?action=view", 
       * return TRUE for special handling
       */
      cmsLog_debug("'wancfg.cmd?action=view'  is allowed for LOGIN_USER");
      isSpecialCase = TRUE;
   }

   return isSpecialCase;
}

static void getPageIndex(const char *inPath, TableType tabType, UINT32 *index)
{
   UINT32 i = 0;
   char filename[WEB_BUF_SIZE_MAX]={0};
   char *query;
   char* cp = NULL;
   char* ext = NULL;
   char empty = 0;
   char path[HTTPD_BUFLEN_10K];
//   char path[BUFLEN_1024];
   
   strcpy(path, inPath);
   cmsLog_debug("path=%s",path);

   cp = strchr(path, '?');
   if ( cp != NULL ) 
   {
      query = cp + 1; // reuse big buffer in httpd.c
      if (*(cp - 4) == '.')
         ext = cp - 4;
      else if (*(cp - 5) == '.')
         ext = cp - 5;
   }
   else
   {
      query = &empty;
      ext = strrchr(path, '.');
   }

   if ( ext != NULL ) 
   {
      *ext = '\0';
      strncpy(filename, path, sizeof(filename)-1);
      cmsLog_debug("query = %s, filename = %s, tabType=%d, NUM_WEB_CMDS=%d, NUM_HTML_PAGES=%d", query, filename, tabType, NUM_WEB_CMDS, NUM_HTML_PAGES);

      switch (tabType)
      {
         case CMD_TABLE:

            for (i=0; i < NUM_WEB_CMDS; i++ ) 
            {
               if (strstr(filename, WebCmdTable[i].cgiCmdName))
               {
                  cmsLog_debug("Found %s with authLevel %d", WebCmdTable[i].cgiCmdName, WebCmdTable[i].authLevel);
                  break;
               }               
            } 
            if (i >= NUM_WEB_CMDS)
            {
               cmsLog_error("Could not find %s in WebCmdTable", filename);
               break;
            }
            if (specialCaseHandle(WebCmdTable[i].cgiCmdName, query))
            {
               /* special case where filename is "wancfg.cmd?action=view" OR "rtroutecfg?action=view", etc.  use index 0 is for LOGIN_USER 
                * since the first in the array is allowed for LOGIN_USER 
                */
               i = 0;
            }
            break;

         case HTML_TABLE:
         
            for (i=0; i < NUM_HTML_PAGES; i++ ) 
            {
               if (strstr(filename, htmlPageAuthTable[i].htmlPageName))
               {
                  cmsLog_debug("Found %s with authLevel %d", htmlPageAuthTable[i].htmlPageName, htmlPageAuthTable[i].authLevel);
                  break;
               }               
            }    
            break;         

         default:
            cmsLog_error("Not support table type %d", tabType);
            break;
            
      }

      *index = i;
   }
   
   cmsLog_debug("index=%d", *index);
   
}




UBOOL8 isPageAllowed(const char *path, HttpLoginType authLevel)
{
   UBOOL8 allowed = FALSE;
   UINT32 i = 0;

   if (strstr(path, ".cmd"))
   {
      getPageIndex(path, CMD_TABLE, &i);
      
      if ( i >= NUM_WEB_CMDS ) 
      {
         // Badly formatted post string or unrecognized page
         cmsLog_error("unsupported filename %s", path);
      }
      else
      {
//         cmsLog_debug("cmdName=%s, authLevel=%d, WebCmdTable[i].authLevel = %d", WebCmdTable[i].cgiCmdName, authLevel, WebCmdTable[i].authLevel);
         allowed = (authLevel >= WebCmdTable[i].authLevel);
      }
   }
   /* Only one .tod for time of day page and just put that in the html array */
   else if (strstr(path, ".html") || strstr(path, ".tod"))
   {
      getPageIndex(path, HTML_TABLE, &i);      
      
      if ( i >= NUM_HTML_PAGES) 
      {
         cmsLog_debug("allowed html filename %s", path);
         /* for all other html page, index default to 0 for info.html which is allowed for LOGIN_USER */
         i = 0; 
      }
      cmsLog_debug("htmlPageName=%s, authLevel=%d, htmlPageAuthTable[i].authLevel = %d", htmlPageAuthTable[i].htmlPageName, authLevel, htmlPageAuthTable[i].authLevel);
      allowed = (authLevel >= htmlPageAuthTable[i].authLevel);
   }
   
   cmsLog_debug("Allowed = %d", allowed);
   
   return allowed;
   
}



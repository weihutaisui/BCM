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
#include <time.h>
#include <unistd.h>
#include <net/route.h>

#include "httpd.h"
#include "cgi_main.h"
#include "cgi_sts.h"
#include "cgi_ntwk.h"
#include "cgi_route.h"
#include "cgi_qos.h"
#include "cgi_diag.h"
#include "cgi_pwrmngt.h"
#include "cgi_bmu.h"
#include "cgi_standby.h"
#include "cgi_util.h"
#include "cgi_ipsec.h"
#include "cgi_cellular.h"
#include "cgi_multicast.h"
#include "cgi_ingress_filters.h"

#if defined(SUPPORT_QRCODE)
#include "cgi_qrcode.h"
#endif

#ifdef SUPPORT_IPV6
#include "cgi_lan6.h"
#endif

#ifdef BRCM_WLAN
#ifndef SUPPORT_UNIFIED_WLMNGR 
#include "wlapi.h"
#endif
#endif


#include "cms_dal.h"
#include "cms_msg.h"
#include "cms_qos.h"
#include "cms_boardcmds.h"
#include "cms_seclog.h"

#if defined(SUPPORT_WEB_SOCKETS)
#include "httpdshared.h"
#endif

#ifdef SUPPORT_LANVLAN
#include "cms_qdm.h"
#endif

void bcmProcessMarkStrChars(char *str);
int  bcmIsMarkStrChar(char c);

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))


/*
 * This is a table that associates ejGetOther variables with functions.
 * It is used by cgiGetVarOther.
 */
CGI_GET_VAR WebVarTable[] = {
   { "sysInfo", cgiGetSysInfo },
   { "memInfo", cgiGetMemInfo },
   { "sessionKey", cgiGetCurrSessionKey },
#ifdef SUPPORT_WAN_HTML
   { "wanInterfaceInfo", cgiGetWanInterfaceInfo },
#endif
#ifdef SUPPORT_TR69C
   { "tr69cInterfaceList", cgiGetTr69cInterfaceList },
#endif   
   { "diagInfo", cgiGetDiagInfo },
#ifdef SUPPORT_DSL
   { "lineRate", cgiGetLineRate },
#endif /* support_dsl */
   { "diagnostic", cgiGetDiagnostic },
   { "dmzHost", cgiGetDmzHost },
#ifdef DMP_X_BROADCOM_COM_DNSPROXY_1 /* aka SUPPORT_DNSPROXY */
   { "dnsProxy", cgiGetDnsProxy },
#endif
   { "stsifcget", cgiGetStsIfc },
#ifdef SUPPORT_DSL 
   { "stsadslget", cgiGetStsAdsl },
#endif
#ifdef SUPPORT_CELLULAR
   { "cellularInfo", cgiGetCellularInfo },
#endif
   { "staticiplease", cgiGetStaticIpLease },
#if SUPPORT_PORT_MAP
   { "bridgeInterfaceInfo", cgiGetBridgeInterfaceInfo },
   { "getPMapGroupName", cgiGetPMapGroupName },
   { "vendorid",   cgiGetDhcpVendorId },
#else
   { "bridgeInterfaceInfo", cgiGetBridgeInterfaceInfo },
#endif
#ifdef SUPPORT_DSL
   { "isVdsl", cgiIsVdslSupported },
   { "isDslBondingEnabled", cgiGetIsDslBondingEnabled },
#endif /* SUPPORT_DSL */
#ifdef SUPPORT_QOS
#ifdef SUPPORT_WAN_HTML
#if (defined(SUPPORT_DSL) && defined(SUPPORT_FAPCTL)) || (defined(SUPPORT_RDPA) && (defined(CHIP_63138) || defined(CHIP_63148) || defined(CHIP_63158) || defined(CHIP_4908))) || defined(SUPPORT_BCMTM)
   { "qosportshapingInfo", cgiGetQosPortShapingInfo },
#endif
   { "queueIntf", cgiGetQueueIntf },
   { "qosqueueInfo", cgiGetQosQueueInfo },
   { "qospolicerInfo", cgiGetQosPolicerInfo },
   { "qosclsRulsOrder", cgiGetQosClsRulsOrder },
   { "qosPrecedence", cgiGetPrecedence },
   { "qosIntf", cgiGetQosIntf },
   { "qosMgmtDefEnbl", cgiGetQosMgmtEnbl},
   { "qosMgmtDefQueue", cgiGetQosMgmtDefQueue },
   { "qosMgmtDefMark", cgiGetQosMgmtDefMark },
#endif  /* SUPPORT_WAN_HTML */
#endif  /* SUPPORT_QOS */
#ifdef SUPPORT_IPV6
   { "enblRadvd", cgiGetEnblRadvd},
   { "enblDhcp6s", cgiGetEnblDhcp6s},
#endif
#ifdef SUPPORT_DSL
   { "trafficType", cgiGetVdslType },
#endif
#ifdef DMP_X_BROADCOM_COM_ETHERNETOAM_1
   { "eoamIntfList", cgiGetEoamIntfList },
   { "eoam1agLbmResult", cgiGetEoam1agLbmResult},
   { "eoam1agLtmResult", cgiGetEoam1agLtmResult },
#endif
#ifdef SUPPORT_ETHWAN
   { "ethL2AvaIntf", cgiGetAvailableL2EthIntf },
   { "gmacIntf", cgiGetGMACEthIntf },
   { "wanOnlyEthIntf", cgiGetWanOnlyEthIntf },   
#endif
#if defined(DMP_X_BROADCOM_COM_MOCAWAN_1) || defined(DMP_DEVICE2_MOCA_1)
   { "mocaL2AvaIntf", cgiGetAvailableL2MocaIntf },
#endif
#ifdef DMP_X_BROADCOM_COM_GPONWAN_1
   { "gponL2AvaIntf", cgiGetAvailableL2GponIntf },
#endif
#ifdef DMP_X_BROADCOM_COM_EPONWAN_1
   { "eponL2AvaIntf", cgiGetAvailableL2EponIntf },
#endif
#ifdef DMP_X_BROADCOM_COM_PWRMNGT_1 /* aka SUPPORT_PWRMNGT */
   { "pwrmngt", cgiGetPowerManagement },
#endif
#ifdef SUPPORT_BMU
   { "bmu", cgiGetBmu },
#endif
#ifdef DMP_X_BROADCOM_COM_STANDBY_1
   { "standby", cgiGetStandby },
#endif 
#if defined(DMP_X_BROADCOM_COM_IGMP_1) || defined(DMP_X_BROADCOM_COM_MLD_1) || defined(DMP_X_BROADCOM_COM_MCAST_1)
   { "multicast", cgiGetMulticastInfo },
#endif 
#if defined(DMP_X_BROADCOM_COM_RDPA_1)
   { "ingressFiltersGet", cgiGetIngressFiltersCfg },
   { "ingressFiltersSysPortsGet", cgiIngressFiltersSysPortsGet },
   { "stsqueueget", cgiGetStsQueue },
#endif
#ifdef SUPPORT_LANVLAN
   { "ethL2AllIntf", cgiGetAllL2EthIntf },
#endif 
#ifdef DMP_DEVICE2_OPTICAL_1

#ifdef DMP_X_ITU_ORG_GPON_1
   { "stsopticget", cgiGetStsOptical },
#endif    // DMP_X_ITU_ORG_GPON_1

#endif    // DMP_DEVICE2_OPTICAL_1

#ifdef SUPPORT_WIFIWAN
   { "wlL2AvaIntf", cgiGetAvailableL2WlIntf },
#endif
#ifdef SUPPORT_HOMEPLUG 
   { "homeplugInfo", cgiHomePlugAssoc },
#endif
#if defined(DMP_X_BROADCOM_COM_SPDSVC_1)
   { "spdsvcInfo", cgiGetSpdsvcInfo },
#endif
#if defined(SUPPORT_QRCODE)
   { "qrcodeTxt", cgiGetQrcodeTxt },
#endif
#if defined(DMP_X_BROADCOM_COM_NFC_1)
   { "nfcInfo", cgiGetNfc},
#endif
#if defined(DMP_X_BROADCOM_COM_OPENVSWITCH_1)
   { "openVS",  cgiGetOpenVSCfg },
#endif   
#ifdef DMP_X_BROADCOM_COM_CONTAINER_1
   { "container-table", cgiGetContainerTable },
   { "container-detail", cgiGetContainerDetail },
#endif /* DMP_X_BROADCOM_COM_CONTAINER_1 */
};

CGI_FNC_CMD WebFncTable[] = {
   { "stsifcreset",   cgiResetStsIfc },
#ifdef SUPPORT_MOCA   
   { "stsmocareset",   cgiResetStsMoca },
#if defined(DMP_X_BROADCOM_COM_MOCALAN_1) || defined(DMP_DEVICE2_MOCA_1)
   { "stsmocalanreset",   cgiResetStsMocaLan },
#endif
#if defined(DMP_X_BROADCOM_COM_MOCAWAN_1) || defined(DMP_DEVICE2_MOCA_1)
   { "stsmocawanreset",   cgiResetStsMocaWan },
#endif
#endif   
#ifdef DMP_DEVICE2_OPTICAL_1

#ifdef DMP_X_ITU_ORG_GPON_1
   { "stsopticreset", cgiResetStsOptical },
#endif    // DMP_X_ITU_ORG_GPON_1

#endif    // DMP_DEVICE2_OPTICAL_1
#ifdef SUPPORT_DSL
   { "stsxtmreset",   cgiResetStsXtm },
   { "stsadslreset",  cgiResetStsAdsl },
#endif /* SUPPORT_DSL */
   { "stswanreset",   cgiResetStsWan }
};

WEB_NTWK_VAR glbWebVar;
int glbDiagInfoClear=1; // use to indicate clean glbDiagInfo or not.
char glbDiagInfo[WEB_DIAG_MAX][BUFLEN_64]; // use to transfer diagnostics info between WEB client & server
int glbAdslTestMode;    // this global var is accessed from cgintwk.c
char glbAdslTestTones[257]; // this global var is accessed from cgintwk.c
int glbReset = 0;           // flag for atm vcc reset
char glbErrMsg[BUFLEN_256] = "";  // use to return error message
int glbRefresh = 0;  // flag for SNMP configuration HTML to be refreshed
int glbEntryCt = 0;  // used for web page (port trigger) to know how many entries can be added
int glbUploadMode;     // used for web page upload image or updating settings.
int glbOldPort = 0;  // used for validate pvc configuration and remove old pvc. It is accessed from ifcuiweb.cpp
int glbOldVpi = 0;   // used for validate pvc configuration and remove old pvc. It is accessed from ifcuiweb.cpp
int glbOldVci = 0;   // used for validate pvc configuration and remove old pvc. It is accessed from ifcuiweb.cpp
int glbExistedProtocol = 0;   // used for validate pvc configuration 
int glbCgiTrue = 1;
int glbCgiFalse = 0;
int glbEnblEnetWan = 2; // default 2 = it is configured out in menuconfig
int glbQosPrioLevel = MAX_QOS_LEVELS;
int glbEthQosPrioLevel = ETH_QOS_LEVELS;
SINT32 glbCurrSessionKey=0;  // The current valid session key
CmsTimestamp glbSessionKeyIssuedTms={0,0};  // The timestamp when the current session key was created.

/* for default gateway way/dns configuration to share the same html page.  If == 1, it is in pvc creation, == 0, in advaced setup */
//TODO int glbPvcCreation = 0;

extern void do_ej(char *path, FILE *stream);
extern void do_file(char *path, FILE *stream);
extern void destroy(void);

/* defined in cgi_dslintf.c */
extern void cgiDslAdd(FILE *fs) ; 



void do_cgi(char *path, FILE *fs) {
   char filename[WEB_BUF_SIZE_MAX];
   char* query = NULL;
   char* ext = NULL;
   CmsRet ret=CMSRET_SUCCESS;


   /*
    * First thing we do is find the ? charater in the input string.
    * Take all the values that the user has provided to us and store it
    * into glbWebVar.
    */
   if ((query = strchr(path, '?')) != NULL)
   {
      cgiParseSet(path);
   }


   /*
    * Then for the response to the user, chop off the .cgi suffix
    * in the filename and change it to .html.
    * As we send the .html file to the user, we also substitute
    * variables inside the html page to their actual values in the
    * do_ej() function.
    */
   filename[0] = '\0';
   ext = strrchr(path, '.');
   if ( ext != NULL ) {
      *ext = '\0';
      strcpy(filename, path);
      strcat(filename, ".html");


      /*
       * Protect against cross site forgery attacks
       * statsadsl.html may need to set the line number, but that is not
       * a configuration value, so skip session key check for that file.
       */
      if (!strstr(filename, "statsadsl.html"))
      {
         if(cgiValidateSessionKey(fs) != CMSRET_SUCCESS)
         {
            /*sessionkey validation failed*/
            return;
         }
      }

      /*
       * check for specific cgi filenames and act upon them.
       */
#ifdef SUPPORT_HOMEPLUG 
      if (strstr(filename, "homeplugpassword.html") != NULL)
      {
         ret = cgiConfigHomePlugPassword();
         if (ret == CMSRET_SUCCESS)
         {
            cgiWriteMessagePage(fs, 
                                "Message", 
                                "HomePlug Network Password change successful",
                                0);
         }
         else
         {
            cgiWriteMessagePage(fs, 
                                "Message", 
                                "HomePlug Network Password change failed", 
                                0);
         }
         return;
      }
      if (strstr(filename, "homeplug.html") != NULL)
      {
         ret = cgiConfigHomePlug();
         if (ret != CMSRET_SUCCESS)
         {
            cgiWriteMessagePage(fs, 
                                "Message", 
                                "HomePlug change failed", 
                                "homeplug.html");
            return;
         }
      }
#endif
#if defined(SUPPORT_IEEE1905) && defined(DMP_DEVICE2_IEEE1905BASELINE_1)
      if ( strstr(filename, "ieee1905cfg.html") != NULL )
      {
         ret = cgiDoIeee1905Config();
         glbSaveConfigNeeded = (ret == CMSRET_SUCCESS) ? TRUE : FALSE;
      }
#endif
      if ( strstr(filename, "password.html") != NULL )
      {
         ret = cgiConfigPassword();
         if ( CMSRET_SUCCESS == ret )
         {
            glbSaveConfigNeeded = 1;
            cgiWriteMessagePage(fs, "Message", "Password change successful", 0);
         }
         else
         {
            glbSaveConfigNeeded = 0;
            cgiWriteMessagePage(fs, "Message", "Password change failed", 0);
         }
         return;
      }
      else if ( strstr(filename, "logconfig.html") != NULL )
      {
         glbSaveConfigNeeded = ((ret = cgiConfigSysLog()) == CMSRET_SUCCESS);
      }

#if defined(DMP_TIME_1) || defined(DMP_DEVICE2_TIME_1) 
      else if ( strstr(filename, "sntpcfg.html") != NULL )
      {
         glbSaveConfigNeeded = ((ret = cgiConfigNtp()) == CMSRET_SUCCESS);
         glbRefresh = FALSE;
      }
#endif
#ifdef DMP_X_BROADCOM_COM_SNMP_1
      else if ( strstr(filename, "snmpconfig.html") != NULL )
      {
         ret = cgiConfigSnmp();
         if (ret == CMSRET_SUCCESS)
         {
            glbSaveConfigNeeded = TRUE;
         }
      }
#endif
#ifdef SUPPORT_TR69C
      else if ( strstr(filename, "tr69cfg.html") != NULL )
      {
         glbSaveConfigNeeded = ((ret = cgiTr69cConfig()) == CMSRET_SUCCESS) ? TRUE : FALSE;
      }
      else if ( strstr(filename, "tr69get.html") != NULL )
      {
         cgiTr69cGetRpcMethods();
         makePathToWebPage(filename, HTTPD_BUFLEN_10K, "tr69cfg.html");
      }
#endif
#ifdef DMP_X_BROADCOM_COM_DBUSREMOTE_1
      else if ( strstr(filename, "dbusremotecfg.html") != NULL )
      {
         ret = cgiConfigDbusremote();
         if (ret == CMSRET_SUCCESS)
         {
            glbSaveConfigNeeded = TRUE;
         }
      }
#endif
#ifdef DMP_X_ITU_ORG_GPON_1 /* aka SUPPORT_OMCI */
      else if ( strstr(filename, "omcisystem.html") != NULL )
      {
         glbSaveConfigNeeded = ((ret = cgiOmciSystem()) == CMSRET_SUCCESS) ? TRUE : FALSE;
      }
#endif

#ifdef not_implemented_yet
#ifdef SUPPORT_PORT_MAP
      else if ( strstr(filename, "portmapedit.html") != NULL )
         cgiConfigPortMap();
#endif
#endif  /* not_implemented_yet */


#ifdef SUPPORT_DSL

#ifdef ANNEX_C  
      else if ( strstr(filename, "adslcfgc.html") != NULL )
#else    // ANNEX_A

#ifdef DMP_VDSL2WAN_1
      else if ( strstr(filename, "xdslcfg.html") != NULL )
#else
      else if ( strstr(filename, "adslcfg.html") != NULL )
#endif

#endif  /* ANNEX_A */
      {
         /*
          * All the settings that the user has set has been parsed
          * and put into glbWebVar.  Now we call the DAL layer to push
          * those values into the MDM.
          */
         cmsLog_debug("setting AdslFlags/VdslFlags");
         if ((ret = dalWan_setAdslFlags(glbWebVar.adslFlag)) != CMSRET_SUCCESS)
         {
            cmsLog_error("config adsl failed, ret=%d flags=0x%x", ret, glbWebVar.adslFlag);
         }
         else
         {
            glbSaveConfigNeeded = TRUE;
         }
      }
#endif /* SUPPORT_DSL */

#ifdef SUPPORT_CELLULAR
      /*Order of strstr cellular.html must before cellularinterface.html*/
      else if ( strstr(filename, "cellular.html") != NULL ) 
      {
         glbSaveConfigNeeded = ((ret = cgiCellularCfg()) == CMSRET_SUCCESS) ? TRUE : FALSE;
      }
      else if ( strstr(filename, "cellularinterface.html") != NULL )
      {
         glbSaveConfigNeeded = ((ret = cgiCellularIntfCfg()) == CMSRET_SUCCESS) ? TRUE : FALSE;
      }
#endif

       else if ( strstr(filename, "lancfg2.html") != NULL )
      {
         /* mwang: talked to Sean and Yen and its not clear what glbRefresh
          * is supposed to indicate.  Maybe refresh page? or something to
          * do with portmapping.  Sean says original code had it set to false.
          */
         UBOOL8 lanIpChanged;
         glbRefresh = FALSE;

         cmsLog_debug("setting lancfg2");

         lanIpChanged = cgiWillLanIpChange(glbWebVar.ethIpAddress, glbWebVar.brName);

         if(isUsedByOtherBridge(glbWebVar.brName, glbWebVar.ethIpAddress, glbWebVar.ethSubnetMask))
         {
            cmsLog_debug("This IP[%s] Mask[%s] is used", glbWebVar.ethIpAddress, glbWebVar.ethSubnetMask);

            cgiWriteMessagePage(fs, "Message", "This IP Address is used by other Group.", "lancfg2.html");

            filename[0] = '\0';

         }
         else
         {
            if (lanIpChanged)
            {
               /*
                * write out the message before we do the set.  Do a sleep 1 to
                * allow the packets to get out of the kernel.  Once we do the set,
                * the br0 address will change and we cannot talk to the browser anymore.
                * We have to assume the set will succeed.
                */
               cgiWriteMessagePage(fs, "Message", "The LAN side IP address of the modem has changed.<br>Force your LAN side computer(s) to acquire the new address.", "lancfg2.html");
               sleep(1);
               filename[0] = '\0';
            }

            if ((ret = dalLan_setLanDevice(&glbWebVar)) != CMSRET_SUCCESS)
            {
               cmsLog_error("dalLan_setLanDevice failed, ret=%d", ret);
            }

#if defined(DMP_X_BROADCOM_COM_IGMPSNOOP_1) 
            else if ((ret = dalLan_setIgmpSnooping(glbWebVar.brName,
                                                   glbWebVar.enblIgmpSnp,
                                                   glbWebVar.enblIgmpMode,
                                 glbWebVar.enblIgmpLanToLanMcast)) != CMSRET_SUCCESS)
            {
               cmsLog_error("set of Igmp snooping failed, ret=%d", ret);
            }
#endif

            else
            {
               glbSaveConfigNeeded = TRUE;
            }

#ifndef DESKTOP_LINUX
         /*
          * On the modem, httpd must exit so that it can bind to the new 
          * lan side IP address.  But on desktop linux, we are not actually
          * changing anything, so let httpd stay running so the web browser
          * will not act weird.
          */
            if (lanIpChanged)
            {
               cmsLog_notice("changed LAN IP address, unconditionally exit");
               glbStsFlag = WEB_STS_LAN_CHANGED_EXIT;
            }
#endif
         }
      }
      else if ( strstr(filename, "lancfg2get.html") != NULL )
      {
         cmsLog_debug("getting lancfg2get brName=%s", glbWebVar.brName);

         /*
          * Don't need to call any functions here.
          * All we need to do is set the glbWebVar.brName with the bridge
          * name that was selected on the LAN config web page.
          * The code will call do_ej on lancfg2.html, which will
          * call ejRefreshGlbWebVar, which will call dalLan_getLanPageInfo,
          * which will load the glbWebVar with all the info associated
          * with this bridge.
          */

         makePathToWebPage(filename, sizeof(filename), "lancfg2.html");
      }      
#ifdef SUPPORT_LANVLAN
      else if ( strstr(filename, "lanvlancfg.html") != NULL )
      {
         glbRefresh = FALSE;

         cmsLog_debug("setting lanvlancfg");
         dalLan_setLanVlan(&glbWebVar);
         glbSaveConfigNeeded = TRUE;
      }
      else if ( strstr(filename, "lanvlancfgget.html") != NULL )
      {
         cmsLog_debug("getting lancfgvlanget lanName=%s", glbWebVar.lanName);

         /*
          * Don't need to call any functions here.
          * All we need to do is set the glbWebVar.lanName with the bridge
          * name that was selected on the LAN config web page.
          * The code will call do_ej on lanvlancfg.html, which will
          * call ejRefreshGlbWebVar, which will call dalLan_getLanVlanPageInfo,
          * which will load the glbWebVar with all the info associated
          * with this bridge.
          */

         makePathToWebPage(filename, sizeof(filename), "lanvlancfg.html");
      }      
#endif
#ifdef SUPPORT_MOCA
      else if ( strstr(filename, "mocacfg.html") != NULL )
      {
         cmsLog_debug("setting mocacfg");

         
         if ((ret = dalMoca_setWebParams(&glbWebVar)) != CMSRET_SUCCESS)
         {
            cmsLog_error("dalMoca_setWebParams on '%s' failed, ret=%d",
                          glbWebVar.mocaIfName, ret);
         }
         else
         {
            glbSaveConfigNeeded = TRUE;
         }
      }
      else if ( strstr(filename, "mocacfgget.html") != NULL )
      {
         cmsLog_debug("getting mocacfgget ifName=%s", glbWebVar.mocaIfName);

         /*
          * Don't need to call any functions here.
          * All we need to do is set the glbWebVar.mocaIfName with the MoCA
          * i/f name that was selected on the MoCA config web page.
          * The code will call do_ej on mocacfg.html, which will
          * call ejRefreshGlbWebVar, which will call dalMoca_getCurrentCfg,
          * which will load the glbWebVar with all the info associated
          * with this MoCA interface.
          */

         makePathToWebPage(filename, sizeof(filename), "mocacfg.html");
      }      
#endif
#ifdef SUPPORT_UPNP   
      else if (strstr(filename, "upnpcfg.html") != NULL)
      {
         UBOOL8 needSave = FALSE;
         if ((ret = dalUpnp_config((UBOOL8) glbWebVar.enblUpnp, &needSave)) == CMSRET_SUCCESS && needSave == TRUE)
         {
            cmsLog_debug("set upnpcfg done.");
            glbSaveConfigNeeded = TRUE;
         }
      }
#endif /* SUPPORT_UPNP */
      
      else if ( strstr(filename, "restoreinfo.html") != NULL )
      {
         CmsSecurityLogData logData = EMPTY_CMS_SECURITY_LOG_DATA;

         cmsLog_notice("starting user requested restore to defaults and reboot");

         CMSLOG_SEC_SET_PORT(&logData, HTTPD_PORT);
         CMSLOG_SEC_SET_APP_NAME(&logData, "HTTP");
         CMSLOG_SEC_SET_USER(&logData, &glbWebVar.curUserName[0]);
         CMSLOG_SEC_SET_SRC_IP(&logData, &glbWebVar.pcIpAddr[0]);
         cmsLog_security(LOG_SECURITY_AUTH_RESOURCES, &logData, "Restore default settings");         

         /* this invalidates the config flash, which will cause us to use
          * default values in MDM next time we reboot. */
         cmsMgm_invalidateConfigFlash();
         glbStsFlag = WEB_STS_REBOOT;
      }
      else if ( strstr(filename, "rebootinfo.html") != NULL )
      {
         cmsLog_notice("starting user requested reboot");
         glbStsFlag = WEB_STS_REBOOT;
      }
#ifdef SUPPORT_WAN_HTML
      else if ( strstr(filename, "rtdefaultcfg.html") != NULL )
         cgiConfigDefaultGatewayList(filename);
#endif
      else if ( strstr(filename, "dnscfg.html") != NULL )
         cgiConfigDns(filename);
      //      else if ( strstr(filename, "ppppasswordinfo") != NULL )
      //         cgiSetPppUserNameAndPassword();
#ifdef SUPPORT_DSL_BONDING   
      else if (strstr(filename, "dslbondingcfg.html") != NULL)
      {
         if ((ret = dalDsl_setDslBonding(glbWebVar.dslBonding) == CMSRET_SUCCESS))
         {
            char *r;
            cmsLog_debug("set dsl bonding done.");
            glbSaveConfigNeeded = TRUE;
            glbStsFlag = WEB_STS_REBOOT;
            /*
             * replace dslbondingcfg.html with rebootinfo.html so user sees
             * a web page saying we are rebooting the modem now.
             */
            r = strstr(filename, "dslbondingcfg.html");
            sprintf(r, "rebootinfo.html");
         }
      }
#endif /* SUPPORT_DSL_BONDING */
      else if (strstr(filename, "diag.html") != NULL)
      {
         // use to indicate clean glbDiagInfo or not.
         glbDiagInfoClear = 0;
      }
#ifdef SUPPORT_STUN
      else if (strstr(filename, "stuncfg.html") != NULL)
      {
         if (dalStun_StunConfig(&glbWebVar) != CMSRET_SUCCESS)
         {
            cmsLog_error("dalStun_StunConfig '%s' failed, ret=%d", ret);
         }
         else
         {
            glbSaveConfigNeeded = TRUE;
         }
      }
#endif
               
      /*
       * Send page back to user, substituting variables inside
       * the .html page with their real values.  But don't do it
       * if WEB_STS_LAN_CHANGED_EXIT because our connection to the
       * user is lost and fflush on the file stream will block.
       */
      if (glbStsFlag != WEB_STS_LAN_CHANGED_EXIT)
      {
         do_ej(filename, fs);
      }
   }
   else
   {
      cgiWriteMessagePage(fs, "Message",
         "The selected web page is not implemented yet.", 0);
   }
}


void cgiGetVarOther(int argc, char **argv, char *varValue) {
   UINT32 i;

   if ( argc < 2) return;

   for (i=0 ; i < ARRAY_SIZE(WebVarTable); i++ ) 
   
      if ( strcmp(argv[1], WebVarTable[i].cgiGetName) == 0 )
         break;

   if ( i >= ARRAY_SIZE(WebVarTable) )
      return;

   (*(WebVarTable[i].cgiGetHdlr))(argc, argv, varValue);
}

void cgiFncCmd(int argc __attribute__((unused)), char **argv) {
   UINT32 i;

   for (i=0; i < ARRAY_SIZE(WebFncTable); i++ )
      if ( strcmp(argv[1], WebFncTable[i].cgiFncName) == 0 )
         break;
   if ( i >= ARRAY_SIZE(WebFncTable) )
      return;

   (*(WebFncTable[i].cgiFncHdlr))();
}


/*
 * This table is used by cgiGetVar(), which is called by ejGet().
 * For html variables that look like <%ejGet(xxx)%>
 *
 * mwang_todo: the cgiGetTable and cgiSetTable should be just one table,
 * with all the entries exactly as they appear in WEB_NTWK_VARS
 */
typedef struct {
   char *variable;
   void *value;
   CGI_TYPE type;
} CGI_ITEM;

CGI_ITEM CgiGetTable[] = {
   { "adminUserName", (void *)glbWebVar.adminUserName, CGI_TYPE_STR },
   { "adminPassword", (void *)glbWebVar.adminPassword, CGI_TYPE_MARK_STR },
   { "sptUserName", (void *)glbWebVar.sptUserName, CGI_TYPE_STR },
   { "sptPassword", (void *)glbWebVar.sptPassword, CGI_TYPE_MARK_STR },
   { "usrUserName", (void *)glbWebVar.usrUserName, CGI_TYPE_STR },
   { "usrPassword", (void *)glbWebVar.usrPassword, CGI_TYPE_MARK_STR },
   { "curUserName", (void *)glbWebVar.curUserName, CGI_TYPE_STR },
   { "inUserName", (void *)glbWebVar.inUserName, CGI_TYPE_STR },
   { "inPassword", (void *)glbWebVar.inPassword, CGI_TYPE_STR },
   { "inOrgPassword", (void *)glbWebVar.inOrgPassword, CGI_TYPE_STR },
   { "brName", (void *)glbWebVar.brName, CGI_TYPE_STR },
   { "ethIpAddress", (void *)glbWebVar.ethIpAddress, CGI_TYPE_STR },
   { "ethSubnetMask", (void *)glbWebVar.ethSubnetMask, CGI_TYPE_STR },
   { "enblLanFirewall", (void *)&glbWebVar.enblLanFirewall, CGI_TYPE_BOOL },
   { "lan2IpAddress", (void *)glbWebVar.lan2IpAddress, CGI_TYPE_STR },
   { "lan2SubnetMask", (void *)glbWebVar.lan2SubnetMask, CGI_TYPE_STR },
   { "enblLan2", (void *)&glbWebVar.enblLan2, CGI_TYPE_NUM },
   { "dataModelDevice2", (void *)&glbWebVar.dataModelDevice2, CGI_TYPE_NUM },
   { "enblIpVer", (void *)&glbWebVar.enblIpVer, CGI_TYPE_NUM },
   { "pcpMode", (void *)&glbWebVar.pcpMode, CGI_TYPE_NUM },
   { "pcpServer", (void *)glbWebVar.pcpServer, CGI_TYPE_STR },
#ifdef SUPPORT_IPV6
   { "unnumberedModel", (void *)&glbWebVar.unnumberedModel, CGI_TYPE_BOOL},
   { "enblDhcp6sStateful", (void *)&glbWebVar.enblDhcp6sStateful, CGI_TYPE_BOOL},
   { "ipv6IntfIDStart", (void *)glbWebVar.ipv6IntfIDStart, CGI_TYPE_STR },
   { "ipv6IntfIDEnd", (void *)glbWebVar.ipv6IntfIDEnd, CGI_TYPE_STR },
   { "dhcp6LeasedTime", (void *)&glbWebVar.dhcp6LeasedTime, CGI_TYPE_NUM },
   { "dns6Type", (void *)glbWebVar.dns6Type, CGI_TYPE_STR },
   { "dns6Ifc", (void *)glbWebVar.dns6Ifc, CGI_TYPE_STR },
   { "dns6Pri", (void *)glbWebVar.dns6Pri, CGI_TYPE_STR },
   { "dns6Sec", (void *)glbWebVar.dns6Sec, CGI_TYPE_STR },
   { "enblRadvdUla", (void *)&glbWebVar.enblRadvdUla, CGI_TYPE_BOOL },
   { "enblRandomULA", (void *)&glbWebVar.enblRandomULA, CGI_TYPE_BOOL },
   { "ipv6UlaPrefix", (void *)glbWebVar.ipv6UlaPrefix, CGI_TYPE_STR },
   { "ipv6UlaPlt", (void *)&glbWebVar.ipv6UlaPlt, CGI_TYPE_NUM },
   { "ipv6UlaVlt", (void *)&glbWebVar.ipv6UlaVlt, CGI_TYPE_NUM },
   { "wanAddr6Type", (void *)glbWebVar.wanAddr6Type, CGI_TYPE_STR },
   { "dhcp6cForAddr", (void *)&glbWebVar.dhcp6cForAddr, CGI_TYPE_BOOL},
   { "dhcp6cForPd", (void *)&glbWebVar.dhcp6cForPd, CGI_TYPE_BOOL},
   { "wanAddr6", (void *)glbWebVar.wanAddr6, CGI_TYPE_STR },
   { "wanGtwy6", (void *)glbWebVar.wanGtwy6, CGI_TYPE_STR },
   { "dfltGw6Ifc", (void *)glbWebVar.dfltGw6Ifc, CGI_TYPE_STR },
   { "lanIntfAddr6", (void *)glbWebVar.lanIntfAddr6, CGI_TYPE_STR },
#endif
#ifdef SUPPORT_LANVLAN
   { "lanName", (void *)glbWebVar.lanName, CGI_TYPE_STR },
   { "lanVlanEnable", (void *)&glbWebVar.lanVlanEnable, CGI_TYPE_NUM },
   { "lanVlanTagList", (void *)glbWebVar.lanVlanTagList, CGI_TYPE_STR },
#endif
   { "wanL2IfName", (void *)glbWebVar.wanL2IfName, CGI_TYPE_STR },
   { "wanIpAddress", (void *)glbWebVar.wanIpAddress, CGI_TYPE_STR },
   { "wanSubnetMask", (void *)glbWebVar.wanSubnetMask, CGI_TYPE_STR },
   { "wanIntfGateway", (void *)glbWebVar.wanIntfGateway, CGI_TYPE_STR },
   { "defaultGatewayList", (void *)glbWebVar.defaultGatewayList, CGI_TYPE_STR },
   { "wanIfName", (void *)glbWebVar.wanIfName, CGI_TYPE_STR },
   { "dnsPrimary", (void *)glbWebVar.dnsPrimary, CGI_TYPE_STR },
   { "dnsSecondary", (void *)glbWebVar.dnsSecondary, CGI_TYPE_STR },
   { "dnsIfcsList", (void *)&glbWebVar.dnsIfcsList, CGI_TYPE_STR },
   { "dnsHostName",(void *)glbWebVar.dnsHostName, CGI_TYPE_STR},
   { "dnsDomainName", (void *)glbWebVar.dnsDomainName, CGI_TYPE_STR},   
   { "dhcpEthStart", (void *)glbWebVar.dhcpEthStart, CGI_TYPE_STR },
   { "dhcpEthEnd", (void *)glbWebVar.dhcpEthEnd, CGI_TYPE_STR },
   { "dhcpSubnetMask", (void *)glbWebVar.dhcpSubnetMask, CGI_TYPE_STR },   
   { "dhcpLeasedTime", (void *)&glbWebVar.dhcpLeasedTime, CGI_TYPE_NUM },
   { "dhcpRelayServer", (void *)glbWebVar.dhcpRelayServer, CGI_TYPE_STR },
   { "pppUserName", (void *)glbWebVar.pppUserName, CGI_TYPE_MARK_STR },
   { "pppPassword", (void *)glbWebVar.pppPassword, CGI_TYPE_MARK_STR },
   { "pppServerName", (void *)glbWebVar.pppServerName, CGI_TYPE_MARK_STR },
   { "sysVersion", (void *)glbWebVar.swVers, CGI_TYPE_STR },
   { "boardID", (void *)glbWebVar.boardID, CGI_TYPE_STR },
   { "numCpuThreads", (void *)&glbWebVar.numCpuThreads, CGI_TYPE_NUM },
   { "cfeVersion", (void *)glbWebVar.cfeVers, CGI_TYPE_STR },
   { "cmsBuildTimestamp", (void *)glbWebVar.cmsBuildTimestamp, CGI_TYPE_STR },
   { "serviceName", (void *)glbWebVar.serviceName, CGI_TYPE_MARK_STR },
   { "serviceId", (void *)&glbWebVar.serviceId, CGI_TYPE_NUM },
#if defined(DMP_X_BROADCOM_COM_GPONWAN_1)
   { "noMcastVlanFilter", (void *)&glbWebVar.noMcastVlanFilter, CGI_TYPE_NUM },
#endif /* DMP_X_BROADCOM_COM_GPONWAN_1 */
#ifdef SUPPORT_CELLULAR
   { "cellularRoamingEnbl", (void *)&glbWebVar.cellularRoamingEnbl, CGI_TYPE_BOOL },
   { "cellularIntfEnable", (void *)&glbWebVar.cellularIntfEnable, CGI_TYPE_BOOL },
   { "cellularSupportedAccTech", (void *)&glbWebVar.cellularSupportedAccTech, CGI_TYPE_STR },
   { "cellularPrefdAccTech", (void *)&glbWebVar.cellularPrefdAccTech, CGI_TYPE_STR },
   { "cellularNtwkReq", (void *)&glbWebVar.cellularNtwkReq, CGI_TYPE_STR },
   { "cellularNtwkReq_MCCMNC", (void *)&glbWebVar.cellularNtwkReq_MCCMNC, CGI_TYPE_STR },
   { "cellularPinChk", (void *)&glbWebVar.cellularPinChk, CGI_TYPE_STR },
   { "cellularPin", (void *)&glbWebVar.cellularPin, CGI_TYPE_STR },
#endif
   { "enblService", (void *)&glbWebVar.enblService, CGI_TYPE_NUM },
   { "ntwkPrtcl", (void *)&glbWebVar.ntwkPrtcl, CGI_TYPE_NUM },
#ifdef SUPPORT_PORT_MAP
   { "editNtwkPrtcl", (void *)&glbWebVar.editNtwkPrtcl, CGI_TYPE_NUM },
   { "editPortId", (void *)&glbWebVar.editPortId, CGI_TYPE_NUM },
   { "editAtmVpi", (void *)&glbWebVar.editAtmVpi, CGI_TYPE_NUM },
   { "editAtmVci", (void *)&glbWebVar.editAtmVci, CGI_TYPE_NUM },
#endif   
   { "encapMode", (void *)&glbWebVar.encapMode, CGI_TYPE_NUM },
   { "enblDhcpClnt", (void *)&glbWebVar.enblDhcpClnt, CGI_TYPE_NUM },
   { "dhcpcOp60VID", (void *)glbWebVar.dhcpcOp60VID, CGI_TYPE_STR },
   { "dhcpcOp61DUID", (void *)glbWebVar.dhcpcOp61DUID, CGI_TYPE_STR },
   { "dhcpcOp61IAID", (void *)glbWebVar.dhcpcOp61IAID, CGI_TYPE_STR },
   { "dhcpcOp77UID", (void *)glbWebVar.dhcpcOp77UID, CGI_TYPE_STR },
   { "dhcpcOp125Enabled", (void *)&glbWebVar.dhcpcOp125Enabled, CGI_TYPE_BOOL },
   { "dhcpcOp50IpAddress", (void *)glbWebVar.dhcpcOp50IpAddress, CGI_TYPE_STR },
   { "dhcpcOp51LeasedTime", (void *)&glbWebVar.dhcpcOp51LeasedTime, CGI_TYPE_NUM },
   { "dhcpcOp54ServerIpAddress", (void *)glbWebVar.dhcpcOp54ServerIpAddress, CGI_TYPE_STR },
   { "enblDhcpSrv", (void *)&glbWebVar.enblDhcpSrv, CGI_TYPE_NUM },
   { "enblNat", (void *)&glbWebVar.enblNat, CGI_TYPE_NUM },
   { "enblFullcone", (void *)&glbWebVar.enblFullcone, CGI_TYPE_NUM },
   { "enblFirewall", (void *)&glbWebVar.enblFirewall, CGI_TYPE_NUM },
   { "enblOnDemand", (void *)&glbWebVar.enblOnDemand, CGI_TYPE_NUM },
   { "pppTimeOut", (void *)&glbWebVar.pppTimeOut, CGI_TYPE_NUM },
   { "pppIpExtension", (void *)&glbWebVar.pppIpExtension, CGI_TYPE_NUM },
   { "pppAuthMethod", (void *)&glbWebVar.pppAuthMethod, CGI_TYPE_NUM },
#ifdef SUPPORT_ADVANCED_DMZ
   { "enableAdvancedDmz", (void *)&glbWebVar.enableAdvancedDmz, CGI_TYPE_NUM },
   { "nonDmzIpAddress", (void *)glbWebVar.nonDmzIpAddress, CGI_TYPE_STR },
   { "nonDmzIpMask", (void *)glbWebVar.nonDmzIpMask, CGI_TYPE_STR },
#endif  /* SUPPORT_ADVANCED_DMZ */
   { "useStaticIpAddress", (void *)&glbWebVar.useStaticIpAddress, CGI_TYPE_NUM },
   { "pppLocalIpAddress", (void *)glbWebVar.pppLocalIpAddress, CGI_TYPE_STR },
   { "pppShowAuthErrorRetry", (void *)&glbWebVar.pppShowAuthErrorRetry, CGI_TYPE_NUM },
   { "pppAuthErrorRetry", (void *)&glbWebVar.pppAuthErrorRetry, CGI_TYPE_NUM },
   { "enblPppDebug", (void *)&glbWebVar.enblPppDebug, CGI_TYPE_NUM },
   { "pppToBridge", (void *)&glbWebVar.pppToBridge, CGI_TYPE_NUM },
   { "logLevel", (void *)&glbWebVar.logLevel, CGI_TYPE_NUM },
   { "logDisplay", (void *)&glbWebVar.logDisplay, CGI_TYPE_NUM },
   { "logMode", (void *)&glbWebVar.logMode, CGI_TYPE_NUM },
   { "logPort", (void *)&glbWebVar.logPort, CGI_TYPE_NUM },
   { "logStatus", (void *)&glbWebVar.logStatus, CGI_TYPE_NUM },
   { "logIpAddress", (void *)glbWebVar.logIpAddress, CGI_TYPE_STR },
   { "adslFlag", (void *)NULL, CGI_TYPE_ADSL_FLAG },
#ifdef SUPPORT_RIP
   { "ripMode", (void *)&glbWebVar.ripMode, CGI_TYPE_NUM },
   { "ripVersion", (void *)&glbWebVar.ripVersion, CGI_TYPE_NUM },
   { "ripOperation", (void *)&glbWebVar.ripOperation, CGI_TYPE_NUM },
#endif
   { "portId", (void *)&glbWebVar.portId, CGI_TYPE_NUM },
   { "atmVpi", (void *)&glbWebVar.atmVpi, CGI_TYPE_NUM },
   { "atmVci", (void *)&glbWebVar.atmVci, CGI_TYPE_NUM },
   { "connMode", (void *)&glbWebVar.connMode, CGI_TYPE_NUM },
   { "vlanMuxPr", (void *)&glbWebVar.vlanMuxPr, CGI_TYPE_NUM },
   { "vlanMuxId", (void *)&glbWebVar.vlanMuxId, CGI_TYPE_NUM },
   { "vlanTpid", (void *)&glbWebVar.vlanTpid, CGI_TYPE_NUM },
   { "atmPeakCellRate", (void *)&glbWebVar.atmPeakCellRate, CGI_TYPE_NUM },
   { "atmSustainedCellRate", (void *)&glbWebVar.atmSustainedCellRate, CGI_TYPE_NUM },
   { "atmMaxBurstSize", (void *)&glbWebVar.atmMaxBurstSize, CGI_TYPE_NUM },
   { "atmMinCellRate", (void *)&glbWebVar.atmMinCellRate, CGI_TYPE_NUM },
   { "atmServiceCategory", (void *)glbWebVar.atmServiceCategory, CGI_TYPE_STR },
   { "linkType", (void *)glbWebVar.linkType, CGI_TYPE_STR },
   { "ptmPriorityNorm", (void *)&glbWebVar.ptmPriorityNorm, CGI_TYPE_NUM },
   { "ptmPriorityHigh", (void *)&glbWebVar.ptmPriorityHigh, CGI_TYPE_NUM },
#ifdef DMP_X_BROADCOM_COM_SNMP_1
   { "snmpStatus", (void *)&glbWebVar.snmpStatus, CGI_TYPE_NUM },
   { "snmpRefresh", (void *)&glbRefresh, CGI_TYPE_NUM },
   { "snmpRoCommunity", (void *)glbWebVar.snmpRoCommunity, CGI_TYPE_MARK_STR },
   { "snmpRwCommunity", (void *)glbWebVar.snmpRwCommunity, CGI_TYPE_MARK_STR },
   { "snmpSysName", (void *)glbWebVar.snmpSysName, CGI_TYPE_MARK_STR },
   { "snmpSysLocation", (void *)glbWebVar.snmpSysLocation, CGI_TYPE_MARK_STR },
   { "snmpSysContact", (void *)glbWebVar.snmpSysContact, CGI_TYPE_MARK_STR },
   { "snmpTrapIp", (void *)glbWebVar.snmpTrapIp, CGI_TYPE_STR },
#endif
   { "lanRefresh", (void *)&glbRefresh, CGI_TYPE_NUM },
   { "dnsRefresh", (void *)&glbRefresh, CGI_TYPE_NUM },
   { "macPolicy", (void *)&glbWebVar.macPolicy, CGI_TYPE_NUM },
   { "enblQos", (void *)&glbWebVar.enblQos, CGI_TYPE_NUM },
   { "enblDiffServ", (void *)&glbWebVar.enblDiffServ, CGI_TYPE_NUM },
   { "qosClsKey", (void *)&glbWebVar.qosClsKey, CGI_TYPE_NUM },
   { "qosClsName", (void *)&glbWebVar.qosClsName, CGI_TYPE_STR },                      
   { "qosClsOrder", (void *)&glbWebVar.qosClsOrder, CGI_TYPE_NUM },  
   { "qosClsEnable", (void *)&glbWebVar.qosClsEnable, CGI_TYPE_NUM },                     
   { "qosProtocol", (void *)&glbWebVar.qosProtocol, CGI_TYPE_NUM },                  
   { "qosWanVlan8021p", (void *)&glbWebVar.qosWanVlan8021p, CGI_TYPE_NUM },
   { "qosipoptionlist", (void *)&glbWebVar.ipoptionlist, CGI_TYPE_NUM}, 
   { "qosVlan8021p", (void *)&glbWebVar.qosVlan8021p, CGI_TYPE_NUM },               
   { "qosSrcAddr", (void *)&glbWebVar.qosSrcAddr, CGI_TYPE_STR },                   
   { "qosSrcMask", (void *)&glbWebVar.qosSrcMask, CGI_TYPE_STR },
   { "qosSrcPort", (void *)&glbWebVar.qosSrcPort, CGI_TYPE_STR },
   { "qosDstMask", (void *)&glbWebVar.qosDstMask, CGI_TYPE_STR },
   { "qosDstAddr", (void *)&glbWebVar.qosDstAddr, CGI_TYPE_STR },
   { "qosDstPort", (void *)&glbWebVar.qosDstPort, CGI_TYPE_STR },
   { "qosDscpMark", (void *)&glbWebVar.qosDscpMark, CGI_TYPE_NUM }, 
   { "qosDscpCheck", (void *)&glbWebVar.qosDscpCheck, CGI_TYPE_NUM },
   { "qosSrcMacAddr", (void *)&glbWebVar.qosSrcMacAddr, CGI_TYPE_STR },
   { "qosDstMacAddr", (void *)&glbWebVar.qosDstMacAddr, CGI_TYPE_STR },
   { "qosSrcMacMask", (void *)&glbWebVar.qosSrcMacMask, CGI_TYPE_STR },
   { "qosDstMacMask", (void *)&glbWebVar.qosDstMacMask, CGI_TYPE_STR },
   { "qosDscpCheck", (void *)&glbWebVar.qosDscpCheck, CGI_TYPE_NUM },
   { "qosClsQueueKey", (void *)&glbWebVar.qosClsQueueKey, CGI_TYPE_NUM },
   { "qosProtocolExclude", (void *)&glbWebVar.qosProtocolExclude, CGI_TYPE_NUM },
   { "qosSrcPortExclude", (void *)&glbWebVar.qosSrcPortExclude, CGI_TYPE_NUM },
   { "qosSrcIPExclude", (void *)&glbWebVar.qosSrcIPExclude, CGI_TYPE_NUM },
   { "qosDstIPExclude", (void *)&glbWebVar.qosDstIPExclude, CGI_TYPE_NUM },
   { "qosDstPortExclude", (void *)&glbWebVar.qosDstPortExclude, CGI_TYPE_NUM },
   { "qosDscpChkExclude", (void *)&glbWebVar.qosDscpChkExclude, CGI_TYPE_NUM },
   { "qosEtherPrioExclude", (void *)&glbWebVar.qosEtherPrioExclude, CGI_TYPE_NUM },
   { "qosLanIfcName", (void *)&glbWebVar.qosLanIfcName, CGI_TYPE_STR },
#ifdef SUPPORT_QUICKSETUP
   { "quicksetupErrCode", (void *)&glbWebVar.quicksetupErrCode, CGI_TYPE_NUM },
#endif
#ifdef SUPPORT_UPNP
   { "enblUpnp", (void *)&glbWebVar.enblUpnp, CGI_TYPE_NUM },
#endif
   { "glbEntryCt", (void *)&glbEntryCt, CGI_TYPE_NUM },
   { "glbUploadMode", (void *)&glbUploadMode, CGI_TYPE_NUM },
   { "existedProtocol", (void *)NULL, CGI_TYPE_EXIST_PROTOCOL },
#ifdef ETH_CFG
   { "ethSpeed", (void *)&glbWebVar.ethSpeed, CGI_TYPE_NUM },
   { "ethType", (void *)&glbWebVar.ethType, CGI_TYPE_NUM },
   { "ethMtu", (void *)&glbWebVar.ethMtu, CGI_TYPE_NUM },
#endif
#ifdef BRCM_WLAN
   { "enblWireless", (void *)&glbCgiTrue, CGI_TYPE_NUM },
#else
   { "enblWireless", (void *)&glbCgiFalse, CGI_TYPE_NUM },
#endif
#ifdef ETH_CFG
   { "enblEthCfg", (void *)&glbCgiTrue, CGI_TYPE_NUM },
#else
   { "enblEthCfg", (void *)&glbCgiFalse, CGI_TYPE_NUM },
#endif
#ifdef DMP_USBLAN_1
   { "enblUsb", (void *)&glbCgiTrue, CGI_TYPE_NUM },
#else
   { "enblUsb", (void *)&glbCgiFalse, CGI_TYPE_NUM },
#endif
#ifdef WEB_POPUP
   { "enblPopWin", (void *)&glbCgiTrue, CGI_TYPE_NUM },
#else
   { "enblPopWin", (void *)&glbCgiFalse, CGI_TYPE_NUM },
#endif
   { "numEnet", (void *)NULL, CGI_TYPE_NUM_ENET },
   { "numPvc", (void *)NULL, CGI_TYPE_NUM_PVC },
   { "dhcpLeases", (void *)NULL, CGI_TYPE_DHCP_LEASES },
   { "enblEnetWan", (void *)&glbWebVar.enableEthWan, CGI_TYPE_NUM },
   { "glbRefresh", (void *)&glbRefresh, CGI_TYPE_NUM },
   { "enblVlan", (void *)&glbCgiFalse, CGI_TYPE_NUM },
#ifdef SUPPORT_IPSEC
   { "ipsecTable", (void *)NULL, CGI_TYPE_IPSEC_TABLE },
   { "ipsTableIndex", &glbWebVar.ipsTableIndex, CGI_TYPE_NUM},   
   { "ipsConnName", &glbWebVar.ipsConnName, CGI_TYPE_STR},
   { "ipsTunMode", &glbWebVar.ipsTunMode, CGI_TYPE_STR},
   { "ipsIpver", &glbWebVar.ipsIpver, CGI_TYPE_STR},
   { "ipsLocalGwIf", &glbWebVar.ipsLocalGwIf, CGI_TYPE_STR},
   { "ipsRemoteGWAddr", &glbWebVar.ipsRemoteGWAddr, CGI_TYPE_STR},
   { "ipsLocalIPMode", &glbWebVar.ipsLocalIPMode, CGI_TYPE_STR},
   { "ipsLocalIP", &glbWebVar.ipsLocalIP, CGI_TYPE_STR},
   { "ipsLocalMask", &glbWebVar.ipsLocalMask, CGI_TYPE_STR},
   { "ipsRemoteIPMode", &glbWebVar.ipsRemoteIPMode, CGI_TYPE_STR},
   { "ipsRemoteIP", &glbWebVar.ipsRemoteIP, CGI_TYPE_STR},
   { "ipsRemoteMask", &glbWebVar.ipsRemoteMask, CGI_TYPE_STR},
   { "ipsKeyExM", &glbWebVar.ipsKeyExM, CGI_TYPE_STR},
   { "ipsAuthM", &glbWebVar.ipsAuthM, CGI_TYPE_STR}, 
   { "ipsPSK", &glbWebVar.ipsPSK, CGI_TYPE_STR},
   { "ipsCertificateName", &glbWebVar.ipsCertificateName, CGI_TYPE_STR},
   { "ipsPerfectFSEn", &glbWebVar.ipsPerfectFSEn, CGI_TYPE_STR},
   { "ipsManualEncryptionAlgo", &glbWebVar.ipsManualEncryptionAlgo, CGI_TYPE_STR},
   { "ipsManualEncryptionKey", &glbWebVar.ipsManualEncryptionKey, CGI_TYPE_STR},
   { "ipsManualAuthAlgo", &glbWebVar.ipsManualAuthAlgo, CGI_TYPE_STR},
   { "ipsManualAuthKey", &glbWebVar.ipsManualAuthKey, CGI_TYPE_STR},
   { "ipsSPI", &glbWebVar.ipsSPI, CGI_TYPE_STR},
   { "ipsPh1Mode", &glbWebVar.ipsPh1Mode, CGI_TYPE_STR},
   { "ipsPh1EncryptionAlgo", &glbWebVar.ipsPh1EncryptionAlgo, CGI_TYPE_STR},
   { "ipsPh1IntegrityAlgo", &glbWebVar.ipsPh1IntegrityAlgo, CGI_TYPE_STR},
   { "ipsPh1DHGroup", &glbWebVar.ipsPh1DHGroup, CGI_TYPE_STR},
   { "ipsPh1KeyTime", &glbWebVar.ipsPh1KeyTime, CGI_TYPE_NUM},
   { "ipsPh2EncryptionAlgo", &glbWebVar.ipsPh2EncryptionAlgo, CGI_TYPE_STR},
   { "ipsPh2IntegrityAlgo", &glbWebVar.ipsPh2IntegrityAlgo, CGI_TYPE_STR},
   { "ipsPh2DHGroup", &glbWebVar.ipsPh2DHGroup, CGI_TYPE_STR},
   { "ipsPh2KeyTime", &glbWebVar.ipsPh2KeyTime, CGI_TYPE_NUM},
   { "ipsCertList", (void *)NULL, CGI_TYPE_CERT_LIST },
#endif
#if defined(DMP_TIME_1) || defined(DMP_DEVICE2_TIME_1) 
   { "ntp_enabled", &glbWebVar.NTPEnable, CGI_TYPE_BOOL},
   { "use_dst", &glbWebVar.daylightSavingsUsed, CGI_TYPE_BOOL},
   { "ntpServer1", &glbWebVar.NTPServer1, CGI_TYPE_STR},
   { "ntpServer2", &glbWebVar.NTPServer2, CGI_TYPE_STR},
   { "ntpServer3", &glbWebVar.NTPServer3, CGI_TYPE_STR},
   { "ntpServer4", &glbWebVar.NTPServer4, CGI_TYPE_STR},
   { "ntpServer5", &glbWebVar.NTPServer5, CGI_TYPE_STR},
   { "timezone", &glbWebVar.localTimeZoneName, CGI_TYPE_STR},
   { "timezone_offset", &glbWebVar.localTimeZone, CGI_TYPE_STR},
   { "dst_start", &glbWebVar.daylightSavingsStart, CGI_TYPE_STR},
   { "dst_end", &glbWebVar.daylightSavingsEnd, CGI_TYPE_STR},
   { "sntpRefresh", &glbRefresh, CGI_TYPE_NUM },
#endif
#ifdef SUPPORT_CERT
   { "certCategory", &glbWebVar.certCategory, CGI_TYPE_STR},
   { "certName", &glbWebVar.certName, CGI_TYPE_STR},
#endif
#ifdef SUPPORT_TR69C
   { "tr69cInformEnable", &glbWebVar.tr69cInformEnable, CGI_TYPE_BOOL},
   { "tr69cInformInterval", &glbWebVar.tr69cInformInterval, CGI_TYPE_NUM},
   { "tr69cNoneConnReqAuth", &glbWebVar.tr69cNoneConnReqAuth, CGI_TYPE_BOOL},
   { "tr69cDebugEnable", &glbWebVar.tr69cDebugEnable, CGI_TYPE_BOOL},
   { "tr69cAcsURL", &glbWebVar.tr69cAcsURL, CGI_TYPE_STR},
   { "tr69cAcsUser", &glbWebVar.tr69cAcsUser, CGI_TYPE_STR},
   { "tr69cAcsPwd", &glbWebVar.tr69cAcsPwd, CGI_TYPE_STR},
   { "tr69cConnReqURL", &glbWebVar.tr69cConnReqURL, CGI_TYPE_STR},
   { "tr69cConnReqUser", &glbWebVar.tr69cConnReqUser, CGI_TYPE_STR},
   { "tr69cConnReqPwd", &glbWebVar.tr69cConnReqPwd, CGI_TYPE_STR},
   { "tr69cBoundIfName", &glbWebVar.tr69cBoundIfName, CGI_TYPE_STR},
#endif
#ifdef DMP_X_ITU_ORG_GPON_1 /* aka SUPPORT_OMCI */
   { "omciTcontNum", &glbWebVar.omciTcontNum, CGI_TYPE_NUM},
   { "omciTcontMeId", &glbWebVar.omciTcontMeId, CGI_TYPE_NUM},
   { "omciEthNum", &glbWebVar.omciEthNum, CGI_TYPE_NUM},
   { "omciEthMeId1", &glbWebVar.omciEthMeId1, CGI_TYPE_NUM},
   { "omciEthMeId2", &glbWebVar.omciEthMeId2, CGI_TYPE_NUM},
   { "omciEthMeId3", &glbWebVar.omciEthMeId3, CGI_TYPE_NUM},
   { "omciEthMeId4", &glbWebVar.omciEthMeId4, CGI_TYPE_NUM},
   { "omciMocaNum", &glbWebVar.omciMocaNum, CGI_TYPE_NUM},
   { "omciMocaMeId1", &glbWebVar.omciMocaMeId1, CGI_TYPE_NUM},
   { "omciDsPrioQueueNum", &glbWebVar.omciDsPrioQueueNum, CGI_TYPE_NUM},
   { "omciDbgOmciEnable", &glbWebVar.omciDbgOmciEnable, CGI_TYPE_BOOL},
   { "omciDbgModelEnable", &glbWebVar.omciDbgModelEnable, CGI_TYPE_BOOL},
   { "omciDbgVlanEnable", &glbWebVar.omciDbgVlanEnable, CGI_TYPE_BOOL},
   { "omciDbgCmfEnable", &glbWebVar.omciDbgCmfEnable, CGI_TYPE_BOOL},
   { "omciDbgFlowEnable", &glbWebVar.omciDbgFlowEnable, CGI_TYPE_BOOL},
   { "omciDbgRuleEnable", &glbWebVar.omciDbgRuleEnable, CGI_TYPE_BOOL},
   { "omciDbgMcastEnable", &glbWebVar.omciDbgMcastEnable, CGI_TYPE_BOOL},
   { "omciDbgFileEnable", &glbWebVar.omciDbgFileEnable, CGI_TYPE_BOOL},
   { "omciRawEnable", &glbWebVar.omciRawEnable, CGI_TYPE_NUM},
#endif
   { "enetDiag", (void *)NULL, CGI_TYPE_ENET_DIAG },
   { "glbQosPrioLevel", (void *)&glbQosPrioLevel, CGI_TYPE_NUM },
   { "glbEthQosPrioLevel", (void *)&glbEthQosPrioLevel, CGI_TYPE_NUM },
   { "syslogRefresh", (void *)&glbRefresh, CGI_TYPE_NUM },
   { "seclogRefresh", (void *)&glbRefresh, CGI_TYPE_NUM },
#ifdef SUPPORT_ADVANCED_DMZ
   { "enblAdvDmz", (void *)&glbCgiTrue, CGI_TYPE_NUM },
#else
   { "enblAdvDmz", (void *)&glbCgiFalse, CGI_TYPE_NUM },
#endif /* SUPPORT_ADVANCED_DMZ */
   { "diagType", (void *)glbDiagInfo[WEB_DIAG_TYPE], CGI_TYPE_STR },

#ifdef DMP_X_BROADCOM_COM_PWRMNGT_1 
   { "pmCPUSpeed", (void *) &glbWebVar.pmCPUSpeed, CGI_TYPE_NUM },
   { "pmCPUr4kWaitEn", (void *) &glbWebVar.pmCPUr4kWaitEn, CGI_TYPE_BOOL},
   { "pmDRAMSelfRefreshEn", (void *) &glbWebVar.pmDRAMSelfRefreshEn, CGI_TYPE_BOOL},
   { "pmEthAutoPwrDwnEn", (void *) &glbWebVar.pmEthAutoPwrDwnEn, CGI_TYPE_BOOL},
   { "pmEthEEE", (void *) &glbWebVar.pmEthEEE, CGI_TYPE_BOOL},
   { "pmAvsEn", (void *) &glbWebVar.pmAvsEn, CGI_TYPE_BOOL},
#endif  /* aka SUPPORT_PWRMNGT */
#ifdef SUPPORT_BMU 
   { "bmuVersion", (void *) &glbWebVar.bmuVersion, CGI_TYPE_STR },
   { "bmuBuildDateTime", (void *) &glbWebVar.bmuBuildDateTime, CGI_TYPE_STR },
   { "bmuPowerSource", (void *) &glbWebVar.bmuPowerSource, CGI_TYPE_STR },
   { "bmuState", (void *) &glbWebVar.bmuState, CGI_TYPE_STR },
   { "bmuNumberOfPresentBatteries", (void *) &glbWebVar.bmuNumberOfPresentBatteries, CGI_TYPE_NUM },
   { "bmuInputVoltage", (void *) &glbWebVar.bmuInputVoltage, CGI_TYPE_STR },
   { "bmuTemperature", (void *) &glbWebVar.bmuTemperature, CGI_TYPE_NUM },
   { "bmuEstimatedMinutesRemaining", (void *) &glbWebVar.bmuEstimatedMinutesRemaining, CGI_TYPE_NUM },
   { "bmuUpsSecondsOnBattery", (void *) &glbWebVar.bmuUpsSecondsOnBattery, CGI_TYPE_NUM },
   { "bmuStatusBatteryA", (void *) &glbWebVar.bmuStatusBatteryA, CGI_TYPE_STR },
   { "bmuStatusBatteryB", (void *) &glbWebVar.bmuStatusBatteryB, CGI_TYPE_STR },
   { "bmuCapacityBatteryA", (void *) &glbWebVar.bmuCapacityBatteryA, CGI_TYPE_NUM },
   { "bmuCapacityBatteryB", (void *) &glbWebVar.bmuCapacityBatteryB, CGI_TYPE_NUM },
   { "bmuMeasuredVoltageBatteryA", (void *) &glbWebVar.bmuMeasuredVoltageBatteryA, CGI_TYPE_STR },
   { "bmuMeasuredVoltageBatteryB", (void *) &glbWebVar.bmuMeasuredVoltageBatteryB, CGI_TYPE_STR },
   { "bmuEstimatedTimeRemainingBatteryA", (void *) &glbWebVar.bmuEstimatedTimeRemainingBatteryA, CGI_TYPE_NUM },
   { "bmuEstimatedTimeRemainingBatteryB", (void *) &glbWebVar.bmuEstimatedTimeRemainingBatteryB, CGI_TYPE_NUM },
#endif
#ifdef DMP_X_BROADCOM_COM_STANDBY_1
   { "pmStandbyEnable",   (void *) &glbWebVar.pmStandbyEnable, CGI_TYPE_BOOL},
   { "pmStandbyStatusString", (void *) &glbWebVar.pmStandbyStatusString, CGI_TYPE_STR },
   { "pmStandbyHour",     (void *) &glbWebVar.pmStandbyHour, CGI_TYPE_NUM },
   { "pmStandbyMinutes",  (void *) &glbWebVar.pmStandbyMinutes, CGI_TYPE_NUM },
   { "pmWakeupHour",      (void *) &glbWebVar.pmWakeupHour, CGI_TYPE_NUM },
   { "pmWakeupMinutes",   (void *) &glbWebVar.pmWakeupMinutes, CGI_TYPE_NUM },
#endif
#ifdef DMP_X_BROADCOM_COM_MCAST_1
   { "mcastPrecedence", (void *) &glbWebVar.mcastPrecedence, CGI_TYPE_NUM },
   { "mcastStrictWan", (void *) &glbWebVar.mcastStrictWan, CGI_TYPE_NUM },
   { "igmpExceptAddressList", (void *) &glbWebVar.igmpExceptAddressList, CGI_TYPE_STR },
   { "mldExceptAddressList", (void *) &glbWebVar.mldExceptAddressList, CGI_TYPE_STR },
#ifdef DMP_X_BROADCOM_COM_DCSP_MCAST_REMARK_1
   { "mcastDscpRemarkEnable", (void *) &glbWebVar.mcastDscpRemarkEnable, CGI_TYPE_BOOL},
   { "mcastDscpRemarkVal", (void *) &glbWebVar.mcastDscpRemarkVal, CGI_TYPE_NUM },
#endif
#endif
#ifdef DMP_X_BROADCOM_COM_IGMP_1
   { "igmpProxySup", "1", CGI_TYPE_STR},
   { "enblIgmp", (void *)&glbWebVar.enblIgmp, CGI_TYPE_NUM },
   { "enblIgmpMcastSource", (void *)&glbWebVar.enblIgmpMcastSource, CGI_TYPE_NUM},
   { "igmpVer", (void *) &glbWebVar.igmpVer, CGI_TYPE_NUM },
   { "igmpQI", (void *) &glbWebVar.igmpQI, CGI_TYPE_NUM },
   { "igmpQRI", (void *) &glbWebVar.igmpQRI, CGI_TYPE_NUM },
   { "igmpLMQI", (void *) &glbWebVar.igmpLMQI, CGI_TYPE_NUM },
   { "igmpRV", (void *) &glbWebVar.igmpRV, CGI_TYPE_NUM },
   { "igmpMaxGroups", (void *) &glbWebVar.igmpMaxGroups, CGI_TYPE_NUM },
   { "igmpMaxSources", (void *) &glbWebVar.igmpMaxSources, CGI_TYPE_NUM },
   { "igmpMaxMembers", (void *) &glbWebVar.igmpMaxMembers, CGI_TYPE_NUM },
   { "igmpFastLeaveEnable", (void *) &glbWebVar.igmpFastLeaveEnable, CGI_TYPE_BOOL },
#else
   { "igmpProxySup", "0", CGI_TYPE_STR},
#endif
   { "igmpSnpSup", (void *)&glbWebVar.igmpSnpSup, CGI_TYPE_BOOL },
#if defined(DMP_X_BROADCOM_COM_IGMPSNOOP_1)
   { "enblIgmpSnp", (void *)&glbWebVar.enblIgmpSnp, CGI_TYPE_NUM },
   { "enblIgmpMode", (void *)&glbWebVar.enblIgmpMode, CGI_TYPE_NUM },
   { "enblIgmpLanToLanMcast", (void *)&glbWebVar.enblIgmpLanToLanMcast, CGI_TYPE_NUM },
#endif
#ifdef DMP_X_BROADCOM_COM_MLD_1
   { "mldProxySup", "1", CGI_TYPE_STR },
   { "enblMld", (void *)&glbWebVar.enblMld, CGI_TYPE_NUM },
   { "enblMldMcastSource", (void *)&glbWebVar.enblMldMcastSource, CGI_TYPE_NUM},
   { "mldVer", (void *) &glbWebVar.mldVer, CGI_TYPE_NUM },
   { "mldQI", (void *) &glbWebVar.mldQI, CGI_TYPE_NUM },
   { "mldQRI", (void *) &glbWebVar.mldQRI, CGI_TYPE_NUM },
   { "mldLMQI", (void *) &glbWebVar.mldLMQI, CGI_TYPE_NUM },
   { "mldRV", (void *) &glbWebVar.mldRV, CGI_TYPE_NUM },
   { "mldMaxGroups", (void *) &glbWebVar.mldMaxGroups, CGI_TYPE_NUM },
   { "mldMaxSources", (void *) &glbWebVar.mldMaxSources, CGI_TYPE_NUM },
   { "mldMaxMembers", (void *) &glbWebVar.mldMaxMembers, CGI_TYPE_NUM },
   { "mldFastLeaveEnable", (void *) &glbWebVar.mldFastLeaveEnable, CGI_TYPE_BOOL },
#else
   { "mldProxySup", "0", CGI_TYPE_STR },
#endif
   { "mldSnpSup", (void *)&glbWebVar.mldSnpSup, CGI_TYPE_BOOL },
#if defined(DMP_X_BROADCOM_COM_MLDSNOOP_1)
   { "enblMldSnp", (void *)&glbWebVar.enblMldSnp, CGI_TYPE_NUM },
   { "enblMldMode", (void *)&glbWebVar.enblMldMode, CGI_TYPE_NUM },
   { "enblMldLanToLanMcast", (void *)&glbWebVar.enblMldLanToLanMcast, CGI_TYPE_NUM },
#endif
   { "sysVdslVersion", (void *)glbWebVar.vdslSoftwareVersion, CGI_TYPE_STR },
#ifdef DMP_X_BROADCOM_COM_ETHERNETOAM_1
   { "eoam3ahEnbl", (void *) &glbWebVar.eoam3ahEnbl, CGI_TYPE_BOOL },
   { "eoam3ahIntf", (void *)glbWebVar.eoam3ahIntf, CGI_TYPE_STR },
   { "eoam3ahOamId", (void *)&glbWebVar.eoam3ahOamId, CGI_TYPE_NUM },
   { "eoam3ahAeEnbl", (void *) &glbWebVar.eoam3ahAeEnbl, CGI_TYPE_BOOL },
   { "eoam3ahVrEnbl", (void *) &glbWebVar.eoam3ahVrEnbl, CGI_TYPE_BOOL },
   { "eoam3ahLeEnbl", (void *) &glbWebVar.eoam3ahLeEnbl, CGI_TYPE_BOOL },
   { "eoam3ahRlEnbl", (void *) &glbWebVar.eoam3ahRlEnbl, CGI_TYPE_BOOL },
   { "eoam3ahAmEnbl", (void *) &glbWebVar.eoam3ahAmEnbl, CGI_TYPE_BOOL },
   { "eoam1agEnbl", (void *) &glbWebVar.eoam1agEnbl, CGI_TYPE_BOOL },
   { "eoam1731Enbl", (void *) &glbWebVar.eoam1731Enbl, CGI_TYPE_BOOL },
   { "eoam1agMdId", (void *)glbWebVar.eoam1agMdId, CGI_TYPE_STR },
   { "eoam1agMdLvl", (void *)&glbWebVar.eoam1agMdLvl, CGI_TYPE_NUM },
   { "eoam1agMaId", (void *)glbWebVar.eoam1agMaId, CGI_TYPE_STR },
   { "eoam1agCcmInterval", (void *)&glbWebVar.eoam1agCcmInterval, CGI_TYPE_NUM },
   { "eoam1agLocIntf", (void *)glbWebVar.eoam1agLocIntf, CGI_TYPE_STR },
   { "eoam1agLocMepId", (void *)&glbWebVar.eoam1agLocMepId, CGI_TYPE_NUM },
   { "eoam1agLocVlanId", (void *)&glbWebVar.eoam1agLocVlanId, CGI_TYPE_NUM },
   { "eoam1agLocCcmEnbl", (void *) &glbWebVar.eoam1agLocCcmEnbl, CGI_TYPE_BOOL },
   { "eoam1agRemMepId", (void *)&glbWebVar.eoam1agRemMepId, CGI_TYPE_NUM },
#endif
#ifdef SUPPORT_DSL_BONDING
   { "dslBonding", (void *)&glbWebVar.dslBonding, CGI_TYPE_NUM },
   { "dslBondingStatus", (void *)&glbWebVar.dslBondingStatus, CGI_TYPE_NUM },
#endif
   { "bondingLineNum", (void *)&glbWebVar.bondingLineNum, CGI_TYPE_NUM },
   { "groupList", (void *)NULL, CGI_TYPE_GROUP_LIST },

   { "cfgL2tpAc", (void *)&glbWebVar.cfgL2tpAc, CGI_TYPE_NUM },
#ifdef DMP_X_BROADCOM_COM_L2TPAC_1
   { "tunnelName", (void *)glbWebVar.tunnelName, CGI_TYPE_STR },
   { "lnsIpAddress", (void *)glbWebVar.lnsIpAddress, CGI_TYPE_STR },
#endif
   { "cfgPptpAc", (void *)&glbWebVar.cfgPptpAc, CGI_TYPE_NUM },
#ifdef DMP_X_BROADCOM_COM_PPTPAC_1
   { "pptpTunnelName", (void *)glbWebVar.pptpTunnelName, CGI_TYPE_STR },
   { "pnsIpAddress", (void *)glbWebVar.pnsIpAddress, CGI_TYPE_STR },
#endif

#ifdef SUPPORT_DSL
   { "dslPhyDrvVersion", (void *)glbWebVar.dslPhyDrvVersion, CGI_TYPE_STR },
#endif

#ifdef SUPPORT_MOCA
   { "mocaVersion", (void *)glbWebVar.mocaVersion, CGI_TYPE_STR },
   { "mocaList", (void *)NULL, CGI_TYPE_MOCA_LIST },
   { "mocaIfName", (void *)glbWebVar.mocaIfName, CGI_TYPE_STR },
   { "enblMocaPrivacy", (void *)&glbWebVar.enblMocaPrivacy, CGI_TYPE_NUM },
   { "mocaPassword", (void *)glbWebVar.mocaPassword, CGI_TYPE_STR },
   { "enblMocaAutoScan", (void *)&glbWebVar.enblMocaAutoScan, CGI_TYPE_NUM },
   { "mocaFrequency", (void *)&glbWebVar.mocaFrequency, CGI_TYPE_NUM },
#endif

#ifdef SUPPORT_HOMEPLUG
   { "homeplugStatus", (void *)&glbWebVar.homeplugStatus, CGI_TYPE_STR },
   { "homeplugMaxBitRate", (void *)&glbWebVar.homeplugMaxBitRate, CGI_TYPE_NUM },
   { "homeplugMACAddress", (void *)&glbWebVar.homeplugMACAddress, CGI_TYPE_STR },
   { "plcVersion", (void *)&glbWebVar.plcVersion, CGI_TYPE_STR },
   { "homeplugVersion", (void *)&glbWebVar.homeplugVersion, CGI_TYPE_STR },
   { "homeplugAlias", (void *)&glbWebVar.homeplugAlias, CGI_TYPE_STR },
#endif

#ifdef SUPPORT_IEEE1905
   { "ieee1905IsEnabled",       (void *)&glbWebVar.ieee1905IsEnabled,       CGI_TYPE_NUM },
   { "ieee1905Dfname",          (void *)&glbWebVar.ieee1905Dfname,          CGI_TYPE_STR },
   { "ieee1905IsRegistrar",     (void *)&glbWebVar.ieee1905IsRegistrar,     CGI_TYPE_NUM },
   { "ieee1905ShowApFreq24",    (void *)&glbWebVar.ieee1905ShowApFreq24,    CGI_TYPE_NUM },
   { "ieee1905ApFreq24En",      (void *)&glbWebVar.ieee1905ApFreq24En,      CGI_TYPE_NUM },
   { "ieee1905ShowApFreq5",     (void *)&glbWebVar.ieee1905ShowApFreq5,     CGI_TYPE_NUM },
   { "ieee1905ApFreq5En",       (void *)&glbWebVar.ieee1905ApFreq5En,       CGI_TYPE_NUM },
#endif


#ifdef DMP_X_BROADCOM_COM_PSTNENDPOINT_1
   { "voiceServiceVersion", (void *)glbWebVar.voiceServiceVersion, CGI_TYPE_STR },
#endif
#ifdef DMP_X_BROADCOM_COM_DBUSREMOTE_1
   { "dbusRemoteEnable", (void *)&glbWebVar.dbusRemoteEnable, CGI_TYPE_BOOL },
   { "dbusPort", (void *)&glbWebVar.dbusPort, CGI_TYPE_NUM },
#endif
#ifdef DMP_X_BROADCOM_COM_EPON_1
   { "oamAuthLoid",    (void *)glbWebVar.oamAuthLoid,       CGI_TYPE_STR },
   { "oamAuthPass",    (void *)glbWebVar.oamAuthPass,       CGI_TYPE_STR },
#endif

#ifdef SUPPORT_STUN
   { "stunEnable", &glbWebVar.stunEnable, CGI_TYPE_BOOL},
   { "stunServerAddress", &glbWebVar.stunServerAddress, CGI_TYPE_STR},
   { "stunServerPort", &glbWebVar.stunServerPort, CGI_TYPE_NUM},
   { "stunUser", &glbWebVar.stunUser, CGI_TYPE_STR},
   { "stunPwd", &glbWebVar.stunPwd, CGI_TYPE_STR},
   { "stunMaxKeepAlivePeriod", &glbWebVar.stunMaxKeepAlivePeriod, CGI_TYPE_NUM},
   { "stunMinKeepAlivePeriod", &glbWebVar.stunMinKeepAlivePeriod, CGI_TYPE_NUM},
#endif

#ifdef DMP_X_BROADCOM_COM_CONTAINER_1
   { "containerName", &glbWebVar.containerName, CGI_TYPE_STR},
#endif

   { NULL, NULL, CGI_TYPE_NONE }
};

void cgiGetVar(char *varName, char *varValue, FILE *fs __attribute__((unused))) {
   int i = 0;
#ifdef SUPPORT_DSL
   CmsRet ret;
#endif

   varValue[0] = '\0';

   for ( ; CgiGetTable[i].variable != NULL; i++ )
      if ( strcmp(varName, CgiGetTable[i].variable) == 0 )
         break;
         
   if ( CgiGetTable[i].variable != NULL ) {
      switch ( CgiGetTable[i].type ) {
      case CGI_TYPE_STR:
         strcpy(varValue, (char *)CgiGetTable[i].value);
         break;
      case CGI_TYPE_MARK_STR:
         strcpy(varValue, (char *)CgiGetTable[i].value);
         bcmProcessMarkStrChars(varValue);
         break;
      case CGI_TYPE_BOOL:
         sprintf(varValue, "%d", *((UBOOL8 *)CgiGetTable[i].value));
         break;
      case CGI_TYPE_SHORT:
         sprintf(varValue, "%d", *((SINT16 *)CgiGetTable[i].value));
         break;
      case CGI_TYPE_NUM:
         sprintf(varValue, "%d", *((SINT32 *)CgiGetTable[i].value));
         break;
#ifdef SUPPORT_DSL
      case CGI_TYPE_ADSL_FLAG:
         if ((ret = dalWan_getAdslFlags(&(glbWebVar.adslFlag)) != CMSRET_SUCCESS))
         {
            cmsLog_error("Reading of adslFlags failed, use fake value of adslFlag.");
            glbWebVar.adslFlag = 0;
         }
         sprintf(varValue, "%d", glbWebVar.adslFlag);
         break;
#endif
      case CGI_TYPE_EXIST_PROTOCOL:
         cmsUtl_getWanProtocolName(glbExistedProtocol, varValue);
         break;
      case CGI_TYPE_NUM_ENET:
         sprintf(varValue, "%d", devCtl_getNumEnetMacs());	
         break;
      case CGI_TYPE_NUM_PVC:
         sprintf(varValue, "%d", dalWan_getNumberOfUsedPvc());
         break;
      case CGI_TYPE_DHCP_LEASES:
         if ( glbWebVar.enblDhcpSrv == DHCP_SRV_ENABLE )
            cgiPrintDHCPLeases(fs);
         break;
#ifdef SUPPORT_IPSEC         
      case CGI_TYPE_IPSEC_TABLE:
         cgiPrintIPSecTable(varValue);
         break;
      case CGI_TYPE_CERT_LIST:
         cgiPrintCertList(varValue);
         break;
 #endif
      case CGI_TYPE_ENET_DIAG:
         cgiPrintEnetDiag(varValue);
         break;
      case CGI_TYPE_GROUP_LIST:
         cgiPrintGroupList(varValue);
         break;
#ifdef SUPPORT_MOCA
      case CGI_TYPE_MOCA_LIST:
         cgiPrintMocaList(varValue);
         break;
#endif
      default:
         varValue[0] = '\0';
         break;
      }
   }
}


CGI_ITEM CgiSetTable[] = {
   { "adminUserName", (void *)glbWebVar.adminUserName, CGI_TYPE_STR },
   { "adminPassword", (void *)glbWebVar.adminPassword, CGI_TYPE_STR },
   { "sptUserName", (void *)glbWebVar.sptUserName, CGI_TYPE_STR },
   { "sptPassword", (void *)glbWebVar.sptPassword, CGI_TYPE_STR },
   { "usrUserName", (void *)glbWebVar.usrUserName, CGI_TYPE_STR },
   { "usrPassword", (void *)glbWebVar.usrPassword, CGI_TYPE_STR },
   { "curUserName", (void *)glbWebVar.curUserName, CGI_TYPE_STR },
   { "inUserName", (void *)glbWebVar.inUserName, CGI_TYPE_STR },
   { "inPassword", (void *)glbWebVar.inPassword, CGI_TYPE_STR },
   { "inOrgPassword", (void *)glbWebVar.inOrgPassword, CGI_TYPE_STR },
   { "sessionKey", (void *)&glbWebVar.recvSessionKey, CGI_TYPE_NUM },
   { "brName", (void *)glbWebVar.brName, CGI_TYPE_STR },
   { "ethIpAddress", (void *)glbWebVar.ethIpAddress, CGI_TYPE_STR },
   { "ethSubnetMask", (void *)glbWebVar.ethSubnetMask, CGI_TYPE_STR },
   { "enblLanFirewall", (void *)&glbWebVar.enblLanFirewall, CGI_TYPE_BOOL },
   { "lan2IpAddress", (void *)glbWebVar.lan2IpAddress, CGI_TYPE_STR },
   { "lan2SubnetMask", (void *)glbWebVar.lan2SubnetMask, CGI_TYPE_STR },
   { "enblLan2", (void *)&glbWebVar.enblLan2, CGI_TYPE_NUM },
   { "enblIpVer", (void *)&glbWebVar.enblIpVer, CGI_TYPE_NUM },
   { "pcpMode", (void *)&glbWebVar.pcpMode, CGI_TYPE_NUM },
   { "pcpServer", (void *)glbWebVar.pcpServer, CGI_TYPE_STR },
#ifdef SUPPORT_IPV6
   { "unnumberedModel", (void *)&glbWebVar.unnumberedModel, CGI_TYPE_BOOL},
   { "enblDhcp6sStateful", (void *)&glbWebVar.enblDhcp6sStateful, CGI_TYPE_BOOL},
   { "ipv6IntfIDStart", (void *)glbWebVar.ipv6IntfIDStart, CGI_TYPE_STR },
   { "ipv6IntfIDEnd", (void *)glbWebVar.ipv6IntfIDEnd, CGI_TYPE_STR },
   { "dhcp6LeasedTime", (void *)&glbWebVar.dhcp6LeasedTime, CGI_TYPE_NUM },
   { "dns6Type", (void *)glbWebVar.dns6Type, CGI_TYPE_STR },
   { "dns6Ifc", (void *)glbWebVar.dns6Ifc, CGI_TYPE_STR },
   { "dns6Pri", (void *)glbWebVar.dns6Pri, CGI_TYPE_STR },
   { "dns6Sec", (void *)glbWebVar.dns6Sec, CGI_TYPE_STR },
   { "enblRadvdUla", (void *)&glbWebVar.enblRadvdUla, CGI_TYPE_BOOL },
   { "enblRandomULA", (void *)&glbWebVar.enblRandomULA, CGI_TYPE_BOOL },
   { "ipv6UlaPrefix", (void *)glbWebVar.ipv6UlaPrefix, CGI_TYPE_STR },
   { "ipv6UlaPlt", (void *)&glbWebVar.ipv6UlaPlt, CGI_TYPE_NUM },
   { "ipv6UlaVlt", (void *)&glbWebVar.ipv6UlaVlt, CGI_TYPE_NUM },
   { "wanAddr6Type", (void *)glbWebVar.wanAddr6Type, CGI_TYPE_STR },
   { "dhcp6cForAddr", (void *)&glbWebVar.dhcp6cForAddr, CGI_TYPE_BOOL},
   { "dhcp6cForPd", (void *)&glbWebVar.dhcp6cForPd, CGI_TYPE_BOOL},
   { "wanAddr6", (void *)glbWebVar.wanAddr6, CGI_TYPE_STR },
   { "wanGtwy6", (void *)glbWebVar.wanGtwy6, CGI_TYPE_STR },
   { "dfltGw6Ifc", (void *)glbWebVar.dfltGw6Ifc, CGI_TYPE_STR },
   { "lanIntfAddr6", (void *)glbWebVar.lanIntfAddr6, CGI_TYPE_STR },
#endif
#ifdef SUPPORT_LANVLAN
   { "lanName", (void *)glbWebVar.lanName, CGI_TYPE_STR },
   { "lanVlanEnable", (void *)&glbWebVar.lanVlanEnable, CGI_TYPE_NUM },
   { "lanVlanTagList", (void *)glbWebVar.lanVlanTagList, CGI_TYPE_STR },
#endif
   { "wanL2IfName", (void *)glbWebVar.wanL2IfName, CGI_TYPE_STR },
   { "wanIpAddress", (void *)glbWebVar.wanIpAddress, CGI_TYPE_STR },
   { "wanSubnetMask", (void *)glbWebVar.wanSubnetMask, CGI_TYPE_STR },
   { "wanIntfGateway", (void *)glbWebVar.wanIntfGateway, CGI_TYPE_STR },
   { "defaultGatewayList", (void *)glbWebVar.defaultGatewayList, CGI_TYPE_STR },
   { "wanIfName", (void *)glbWebVar.wanIfName, CGI_TYPE_STR },
   { "dnsPrimary", (void *)glbWebVar.dnsPrimary, CGI_TYPE_STR },
   { "dnsSecondary", (void *)glbWebVar.dnsSecondary, CGI_TYPE_STR },
   { "dnsIfcsList", (void *)&glbWebVar.dnsIfcsList, CGI_TYPE_STR },
   { "dnsHostName",(void *)glbWebVar.dnsHostName, CGI_TYPE_STR},
   { "dnsDomainName", (void *)glbWebVar.dnsDomainName, CGI_TYPE_STR},
   { "dhcpEthStart", (void *)glbWebVar.dhcpEthStart, CGI_TYPE_STR },
   { "dhcpEthEnd", (void *)glbWebVar.dhcpEthEnd, CGI_TYPE_STR },
   { "dhcpSubnetMask", (void *)glbWebVar.dhcpSubnetMask, CGI_TYPE_STR },
   { "dhcpLeasedTime", (void *)&glbWebVar.dhcpLeasedTime, CGI_TYPE_NUM },
   { "dhcpRelayServer", (void *)glbWebVar.dhcpRelayServer, CGI_TYPE_STR },
   { "pppUserName", (void *)glbWebVar.pppUserName, CGI_TYPE_STR },
   { "pppPassword", (void *)glbWebVar.pppPassword, CGI_TYPE_STR },
   { "pppServerName", (void *)glbWebVar.pppServerName, CGI_TYPE_STR },
   { "serviceName", (void *)glbWebVar.serviceName, CGI_TYPE_STR },
   { "serviceId", (void *)&glbWebVar.serviceId, CGI_TYPE_NUM },
#if defined(DMP_X_BROADCOM_COM_GPONWAN_1)
   { "noMcastVlanFilter", (void *)&glbWebVar.noMcastVlanFilter, CGI_TYPE_NUM },
#endif /* DMP_X_BROADCOM_COM_GPONWAN_1 */
#ifdef SUPPORT_CELLULAR
   { "cellularRoamingEnbl", (void *)&glbWebVar.cellularRoamingEnbl, CGI_TYPE_BOOL },
   { "cellularIntfEnable", (void *)&glbWebVar.cellularIntfEnable, CGI_TYPE_BOOL },
   { "cellularSupportedAccTech", (void *)&glbWebVar.cellularSupportedAccTech, CGI_TYPE_STR },
   { "cellularPrefdAccTech", (void *)&glbWebVar.cellularPrefdAccTech, CGI_TYPE_STR },
   { "cellularNtwkReq", (void *)&glbWebVar.cellularNtwkReq, CGI_TYPE_STR },
   { "cellularNtwkReq_MCCMNC", (void *)&glbWebVar.cellularNtwkReq_MCCMNC, CGI_TYPE_STR },
   { "cellularPinChk", (void *)&glbWebVar.cellularPinChk, CGI_TYPE_STR },
   { "cellularPin", (void *)&glbWebVar.cellularPin, CGI_TYPE_STR },
#endif
   { "enblService", (void *)&glbWebVar.enblService, CGI_TYPE_NUM },
   { "ntwkPrtcl", (void *)&glbWebVar.ntwkPrtcl, CGI_TYPE_NUM },
#if SUPPORT_PORT_MAP
   { "editNtwkPrtcl", (void *)&glbWebVar.editNtwkPrtcl, CGI_TYPE_NUM },
   { "editPortId", (void *)&glbWebVar.editPortId, CGI_TYPE_PORT },
   { "editAtmVpi", (void *)&glbWebVar.editAtmVpi, CGI_TYPE_VPI },
   { "editAtmVci", (void *)&glbWebVar.editAtmVci, CGI_TYPE_VCI },
#endif   
   { "encapMode", (void *)&glbWebVar.encapMode, CGI_TYPE_NUM },
   { "enblDhcpClnt", (void *)&glbWebVar.enblDhcpClnt, CGI_TYPE_NUM },
   { "dhcpcOp60VID", (void *)glbWebVar.dhcpcOp60VID, CGI_TYPE_STR },
   { "dhcpcOp61DUID", (void *)glbWebVar.dhcpcOp61DUID, CGI_TYPE_STR },
   { "dhcpcOp61IAID", (void *)glbWebVar.dhcpcOp61IAID, CGI_TYPE_STR },
   { "dhcpcOp77UID", (void *)glbWebVar.dhcpcOp77UID, CGI_TYPE_STR },
   { "dhcpcOp125Enabled", (void *)&glbWebVar.dhcpcOp125Enabled, CGI_TYPE_BOOL },
   { "dhcpcOp50IpAddress", (void *)glbWebVar.dhcpcOp50IpAddress, CGI_TYPE_STR },
   { "dhcpcOp51LeasedTime", (void *)&glbWebVar.dhcpcOp51LeasedTime, CGI_TYPE_NUM },
   { "dhcpcOp54ServerIpAddress", (void *)glbWebVar.dhcpcOp54ServerIpAddress, CGI_TYPE_STR },
   { "enblDhcpSrv", (void *)&glbWebVar.enblDhcpSrv, CGI_TYPE_NUM },
   { "enblNat", (void *)&glbWebVar.enblNat, CGI_TYPE_NUM },
   { "enblFullcone", (void *)&glbWebVar.enblFullcone, CGI_TYPE_NUM },
   { "enblFirewall", (void *)&glbWebVar.enblFirewall, CGI_TYPE_NUM },
   { "enblOnDemand", (void *)&glbWebVar.enblOnDemand, CGI_TYPE_NUM },
   { "pppTimeOut", (void *)&glbWebVar.pppTimeOut, CGI_TYPE_NUM },
   { "pppIpExtension", (void *)&glbWebVar.pppIpExtension, CGI_TYPE_NUM },
   { "pppAuthMethod", (void *)&glbWebVar.pppAuthMethod, CGI_TYPE_NUM },
#ifdef SUPPORT_ADVANCED_DMZ
   { "enableAdvancedDmz", (void *)&glbWebVar.enableAdvancedDmz, CGI_TYPE_NUM },
   { "nonDmzIpAddress", (void *)glbWebVar.nonDmzIpAddress, CGI_TYPE_STR },
   { "nonDmzIpMask", (void *)glbWebVar.nonDmzIpMask, CGI_TYPE_STR },
#endif  /* SUPPORT_ADVANCED_DMZ */
   { "useStaticIpAddress", (void *)&glbWebVar.useStaticIpAddress, CGI_TYPE_NUM },
   { "pppLocalIpAddress", (void *)glbWebVar.pppLocalIpAddress, CGI_TYPE_STR },
   { "pppShowAuthErrorRetry", (void *)&glbWebVar.pppShowAuthErrorRetry, CGI_TYPE_NUM },
   { "pppAuthErrorRetry", (void *)&glbWebVar.pppAuthErrorRetry, CGI_TYPE_NUM },
   { "enblPppDebug", (void *)&glbWebVar.enblPppDebug, CGI_TYPE_NUM },
   { "pppToBridge", (void *)&glbWebVar.pppToBridge, CGI_TYPE_NUM },
   { "logIpAddress", (void *)glbWebVar.logIpAddress, CGI_TYPE_STR },
   { "logLevel", (void *)&glbWebVar.logLevel, CGI_TYPE_NUM },
   { "logDisplay", (void *)&glbWebVar.logDisplay, CGI_TYPE_NUM },
   { "logMode", (void *)&glbWebVar.logMode, CGI_TYPE_NUM },
   { "logPort", (void *)&glbWebVar.logPort, CGI_TYPE_NUM },
   { "logStatus", (void *)&glbWebVar.logStatus, CGI_TYPE_NUM },
   { "adslFlag", (void *)&glbWebVar.adslFlag, CGI_TYPE_NUM },
#ifdef SUPPORT_RIP
   { "ripMode", (void *)&glbWebVar.ripMode, CGI_TYPE_NUM },
   { "ripVersion", (void *)&glbWebVar.ripVersion, CGI_TYPE_NUM },
   { "ripOperation", (void *)&glbWebVar.ripOperation, CGI_TYPE_NUM },
#endif
   { "portId", (void *)&glbWebVar.portId, CGI_TYPE_PORT },
   { "atmVpi", (void *)&glbWebVar.atmVpi, CGI_TYPE_VPI },
   { "atmVci", (void *)&glbWebVar.atmVci, CGI_TYPE_VCI },
   { "connMode", (void *)&glbWebVar.connMode, CGI_TYPE_NUM },
   { "enVlanMux", (void *)&glbWebVar.enVlanMux, CGI_TYPE_NUM },
   { "vlanMuxPr", (void *)&glbWebVar.vlanMuxPr, CGI_TYPE_NUM },
   { "vlanMuxId", (void *)&glbWebVar.vlanMuxId, CGI_TYPE_NUM },
   { "vlanTpid", (void *)&glbWebVar.vlanTpid, CGI_TYPE_NUM },
   { "atmPeakCellRate", (void *)&glbWebVar.atmPeakCellRate, CGI_TYPE_NUM },
   { "atmSustainedCellRate", (void *)&glbWebVar.atmSustainedCellRate, CGI_TYPE_NUM },
   { "atmMaxBurstSize", (void *)&glbWebVar.atmMaxBurstSize, CGI_TYPE_NUM },
   { "atmMinCellRate", (void *)&glbWebVar.atmMinCellRate, CGI_TYPE_NUM },
   { "atmServiceCategory", (void *)glbWebVar.atmServiceCategory, CGI_TYPE_STR },
   { "linkType", (void *)glbWebVar.linkType, CGI_TYPE_STR },
   { "ptmPriorityNorm", (void *)&glbWebVar.ptmPriorityNorm, CGI_TYPE_NUM },
   { "ptmPriorityHigh", (void *)&glbWebVar.ptmPriorityHigh, CGI_TYPE_NUM },
   { "diagType", (void *)glbDiagInfo[WEB_DIAG_TYPE], CGI_TYPE_STR },
   { "diagPrev", (void *)glbDiagInfo[WEB_DIAG_PREV], CGI_TYPE_STR },
   { "diagCurr", (void *)glbDiagInfo[WEB_DIAG_CURR], CGI_TYPE_STR },
   { "diagNext", (void *)glbDiagInfo[WEB_DIAG_NEXT], CGI_TYPE_STR },
#ifdef DMP_X_BROADCOM_COM_SNMP_1
   { "snmpStatus", (void *)&glbWebVar.snmpStatus, CGI_TYPE_NUM },
   { "snmpRefresh", (void *)&glbRefresh, CGI_TYPE_NUM },
   { "snmpRoCommunity", (void *)glbWebVar.snmpRoCommunity, CGI_TYPE_STR },
   { "snmpRwCommunity", (void *)glbWebVar.snmpRwCommunity, CGI_TYPE_STR },
   { "snmpSysName", (void *)glbWebVar.snmpSysName, CGI_TYPE_STR },
   { "snmpSysLocation", (void *)glbWebVar.snmpSysLocation, CGI_TYPE_STR },
   { "snmpSysContact", (void *)glbWebVar.snmpSysContact, CGI_TYPE_STR },
   { "snmpTrapIp", (void *)glbWebVar.snmpTrapIp, CGI_TYPE_STR },
#endif
   { "lanRefresh", (void *)&glbRefresh, CGI_TYPE_NUM },
   { "dnsRefresh", (void *)&glbRefresh, CGI_TYPE_NUM },
   { "macPolicy", (void *)&glbWebVar.macPolicy, CGI_TYPE_NUM },
   { "enblQos", (void *)&glbWebVar.enblQos, CGI_TYPE_NUM },
   { "enblDiffServ", (void *)&glbWebVar.enblDiffServ, CGI_TYPE_NUM },
   { "qosClsKey", (void *)&glbWebVar.qosClsKey, CGI_TYPE_NUM },
#ifdef SUPPORT_UPNP
   { "enblUpnp", (void *)&glbWebVar.enblUpnp, CGI_TYPE_NUM },
#endif
#ifdef ETH_CFG
   { "ethSpeed", (void *)&glbWebVar.ethSpeed, CGI_TYPE_NUM },
   { "ethType", (void *)&glbWebVar.ethType, CGI_TYPE_NUM },
   { "ethMtu", (void *)&glbWebVar.ethMtu, CGI_TYPE_NUM },
#endif
#if SUPPORT_PORT_MAP
   { "groupName", (void *)glbWebVar.groupName, CGI_TYPE_STR },
#endif
   { "enblEnetWan", (void *)&glbWebVar.enableEthWan, CGI_TYPE_NUM },
   { "glbRefresh", (void *)&glbRefresh, CGI_TYPE_NUM },
#ifdef SUPPORT_IPSEC
   { "ipsTableIndex", &glbWebVar.ipsTableIndex, CGI_TYPE_NUM},   
   { "ipsConnName", &glbWebVar.ipsConnName, CGI_TYPE_STR},
   { "ipsTunMode", &glbWebVar.ipsTunMode, CGI_TYPE_STR},
   { "ipsIpver", &glbWebVar.ipsIpver, CGI_TYPE_STR},
   { "ipsLocalGwIf", &glbWebVar.ipsLocalGwIf, CGI_TYPE_STR},
   { "ipsRemoteGWAddr", &glbWebVar.ipsRemoteGWAddr, CGI_TYPE_STR},
   { "ipsLocalIPMode", &glbWebVar.ipsLocalIPMode, CGI_TYPE_STR},
   { "ipsLocalIP", &glbWebVar.ipsLocalIP, CGI_TYPE_STR},
   { "ipsLocalMask", &glbWebVar.ipsLocalMask, CGI_TYPE_STR},
   { "ipsRemoteIPMode", &glbWebVar.ipsRemoteIPMode, CGI_TYPE_STR},
   { "ipsRemoteIP", &glbWebVar.ipsRemoteIP, CGI_TYPE_STR},
   { "ipsRemoteMask", &glbWebVar.ipsRemoteMask, CGI_TYPE_STR},
   { "ipsKeyExM", &glbWebVar.ipsKeyExM, CGI_TYPE_STR},
   { "ipsAuthM", &glbWebVar.ipsAuthM, CGI_TYPE_STR}, 
   { "ipsPSK", &glbWebVar.ipsPSK, CGI_TYPE_STR},
   { "ipsCertificateName", &glbWebVar.ipsCertificateName, CGI_TYPE_STR},
   { "ipsPerfectFSEn", &glbWebVar.ipsPerfectFSEn, CGI_TYPE_STR},
   { "ipsManualEncryptionAlgo", &glbWebVar.ipsManualEncryptionAlgo, CGI_TYPE_STR},
   { "ipsManualEncryptionKey", &glbWebVar.ipsManualEncryptionKey, CGI_TYPE_STR},
   { "ipsManualAuthAlgo", &glbWebVar.ipsManualAuthAlgo, CGI_TYPE_STR},
   { "ipsManualAuthKey", &glbWebVar.ipsManualAuthKey, CGI_TYPE_STR},
   { "ipsSPI", &glbWebVar.ipsSPI, CGI_TYPE_STR},
   { "ipsPh1Mode", &glbWebVar.ipsPh1Mode, CGI_TYPE_STR},
   { "ipsPh1EncryptionAlgo", &glbWebVar.ipsPh1EncryptionAlgo, CGI_TYPE_STR},
   { "ipsPh1IntegrityAlgo", &glbWebVar.ipsPh1IntegrityAlgo, CGI_TYPE_STR},
   { "ipsPh1DHGroup", &glbWebVar.ipsPh1DHGroup, CGI_TYPE_STR},
   { "ipsPh1KeyTime", &glbWebVar.ipsPh1KeyTime, CGI_TYPE_NUM},
   { "ipsPh2EncryptionAlgo", &glbWebVar.ipsPh2EncryptionAlgo, CGI_TYPE_STR},
   { "ipsPh2IntegrityAlgo", &glbWebVar.ipsPh2IntegrityAlgo, CGI_TYPE_STR},
   { "ipsPh2DHGroup", &glbWebVar.ipsPh2DHGroup, CGI_TYPE_STR},
   { "ipsPh2KeyTime", &glbWebVar.ipsPh2KeyTime, CGI_TYPE_NUM},
#endif
#if defined(DMP_TIME_1) || defined(DMP_DEVICE2_TIME_1) 
   { "ntp_enabled", (void *)&glbWebVar.NTPEnable, CGI_TYPE_BOOL},
   { "use_dst", (void *)&glbWebVar.daylightSavingsUsed, CGI_TYPE_BOOL},
   { "ntpServer1", (void *)glbWebVar.NTPServer1, CGI_TYPE_STR},
   { "ntpServer2", (void *)glbWebVar.NTPServer2, CGI_TYPE_STR},
   { "ntpServer3", (void *)glbWebVar.NTPServer3, CGI_TYPE_STR},
   { "ntpServer4", (void *)glbWebVar.NTPServer4, CGI_TYPE_STR},
   { "ntpServer5", (void *)glbWebVar.NTPServer5, CGI_TYPE_STR},
   { "timezone", (void *)glbWebVar.localTimeZoneName, CGI_TYPE_STR},
   { "timezone_offset", (void *)glbWebVar.localTimeZone, CGI_TYPE_STR},
   { "dst_start", (void *)glbWebVar.daylightSavingsStart, CGI_TYPE_STR},
   { "dst_end", (void *)glbWebVar.daylightSavingsEnd, CGI_TYPE_STR},
   { "sntpRefresh", (void *)&glbRefresh, CGI_TYPE_NUM },
#endif
#ifdef SUPPORT_TR69C
   { "tr69cInformEnable", (void *)&glbWebVar.tr69cInformEnable, CGI_TYPE_BOOL },
   { "tr69cInformInterval", (void *)&glbWebVar.tr69cInformInterval, CGI_TYPE_NUM },
   { "tr69cNoneConnReqAuth", (void *)&glbWebVar.tr69cNoneConnReqAuth, CGI_TYPE_BOOL },
   { "tr69cDebugEnable", (void *)&glbWebVar.tr69cDebugEnable, CGI_TYPE_BOOL },
   { "tr69cAcsURL", (void *)glbWebVar.tr69cAcsURL, CGI_TYPE_STR },
   { "tr69cAcsUser", (void *)glbWebVar.tr69cAcsUser, CGI_TYPE_STR },
   { "tr69cAcsPwd", (void *)glbWebVar.tr69cAcsPwd, CGI_TYPE_STR },
   { "tr69cConnReqURL", (void *)glbWebVar.tr69cConnReqURL, CGI_TYPE_STR },
   { "tr69cConnReqUser", (void *)glbWebVar.tr69cConnReqUser, CGI_TYPE_STR },
   { "tr69cConnReqPwd", (void *)glbWebVar.tr69cConnReqPwd, CGI_TYPE_STR },
   { "tr69cBoundIfName", (void *)glbWebVar.tr69cBoundIfName, CGI_TYPE_STR},
#endif
#ifdef DMP_X_ITU_ORG_GPON_1 /* aka SUPPORT_OMCI */
   { "omciTcontNum", (void *)&glbWebVar.omciTcontNum, CGI_TYPE_NUM },
   { "omciTcontMeId", (void *)&glbWebVar.omciTcontMeId, CGI_TYPE_NUM},
   { "omciEthNum", (void *)&glbWebVar.omciEthNum, CGI_TYPE_NUM},
   { "omciEthMeId1", (void *)&glbWebVar.omciEthMeId1, CGI_TYPE_NUM},
   { "omciEthMeId2", (void *)&glbWebVar.omciEthMeId2, CGI_TYPE_NUM},
   { "omciEthMeId3", (void *)&glbWebVar.omciEthMeId3, CGI_TYPE_NUM},
   { "omciEthMeId4", (void *)&glbWebVar.omciEthMeId4, CGI_TYPE_NUM},
   { "omciMocaNum", (void *)&glbWebVar.omciMocaNum, CGI_TYPE_NUM},
   { "omciMocaMeId1", (void *)&glbWebVar.omciMocaMeId1, CGI_TYPE_NUM},
   { "omciDsPrioQueueNum", (void *)&glbWebVar.omciDsPrioQueueNum, CGI_TYPE_NUM},
   { "omciDbgOmciEnable", (void *)&glbWebVar.omciDbgOmciEnable, CGI_TYPE_BOOL},
   { "omciDbgModelEnable", (void *)&glbWebVar.omciDbgModelEnable, CGI_TYPE_BOOL},
   { "omciDbgVlanEnable", (void *)&glbWebVar.omciDbgVlanEnable, CGI_TYPE_BOOL},
   { "omciDbgCmfEnable", (void *)&glbWebVar.omciDbgCmfEnable, CGI_TYPE_BOOL},
   { "omciDbgFlowEnable", (void *)&glbWebVar.omciDbgFlowEnable, CGI_TYPE_BOOL},
   { "omciDbgRuleEnable", (void *)&glbWebVar.omciDbgRuleEnable, CGI_TYPE_BOOL},
   { "omciDbgMcastEnable", (void *)&glbWebVar.omciDbgMcastEnable, CGI_TYPE_BOOL},
   { "omciDbgFileEnable", (void *)&glbWebVar.omciDbgFileEnable, CGI_TYPE_BOOL},
   { "omciRawEnable", (void *)&glbWebVar.omciRawEnable, CGI_TYPE_NUM},
#endif
   { "syslogRefresh", (void *)&glbRefresh, CGI_TYPE_NUM },
   { "seclogRefresh", (void *)&glbRefresh, CGI_TYPE_NUM },

#ifdef DMP_X_BROADCOM_COM_PWRMNGT_1 /* aka SUPPORT_PWRMNGT */
   { "pmCPUSpeed", (void *) &glbWebVar.pmCPUSpeed, CGI_TYPE_NUM},
   { "pmCPUr4kWaitEn", (void *) &glbWebVar.pmCPUr4kWaitEn, CGI_TYPE_BOOL},
   { "pmDRAMSelfRefreshEn", (void *) &glbWebVar.pmDRAMSelfRefreshEn, CGI_TYPE_BOOL},
   { "pmEthAutoPwrDwnEn", (void *) &glbWebVar.pmEthAutoPwrDwnEn, CGI_TYPE_BOOL},
   { "pmEthEEE", (void *) &glbWebVar.pmEthEEE, CGI_TYPE_BOOL},
   { "pmAvsEn", (void *) &glbWebVar.pmAvsEn, CGI_TYPE_BOOL},
#endif /* aka SUPPORT_PWRMNGT */
#ifdef DMP_X_BROADCOM_COM_STANDBY_1
   { "pmStandbyEnable",   (void *) &glbWebVar.pmStandbyEnable, CGI_TYPE_BOOL},
   // R/O: { "pmStandbyStatusString", (void *) &glbWebVar.pmStandbyStatusString, CGI_TYPE_STR },
   { "pmStandbyHour",     (void *) &glbWebVar.pmStandbyHour, CGI_TYPE_NUM },
   { "pmStandbyMinutes",  (void *) &glbWebVar.pmStandbyMinutes, CGI_TYPE_NUM },
   { "pmWakeupHour",      (void *) &glbWebVar.pmWakeupHour, CGI_TYPE_NUM },
   { "pmWakeupMinutes",   (void *) &glbWebVar.pmWakeupMinutes, CGI_TYPE_NUM },
#endif
#ifdef DMP_X_BROADCOM_COM_MCAST_1
   { "mcastPrecedence", (void *) &glbWebVar.mcastPrecedence, CGI_TYPE_NUM },
   { "mcastStrictWan", (void *) &glbWebVar.mcastStrictWan, CGI_TYPE_NUM },
   { "igmpExceptAddressList", (void *) &glbWebVar.igmpExceptAddressList, CGI_TYPE_STR },
   { "mldExceptAddressList", (void *) &glbWebVar.mldExceptAddressList, CGI_TYPE_STR },
#endif      
#ifdef DMP_X_BROADCOM_COM_IGMP_1
   { "enblIgmp", (void *)&glbWebVar.enblIgmp, CGI_TYPE_NUM },
   { "enblIgmpMcastSource", (void *)&glbWebVar.enblIgmpMcastSource, CGI_TYPE_NUM},
   { "igmpVer", (void *) &glbWebVar.igmpVer, CGI_TYPE_NUM },
   { "igmpQI", (void *) &glbWebVar.igmpQI, CGI_TYPE_NUM },
   { "igmpQRI", (void *) &glbWebVar.igmpQRI, CGI_TYPE_NUM },
   { "igmpLMQI", (void *) &glbWebVar.igmpLMQI, CGI_TYPE_NUM },
   { "igmpRV", (void *) &glbWebVar.igmpRV, CGI_TYPE_NUM },
   { "igmpMaxGroups", (void *) &glbWebVar.igmpMaxGroups, CGI_TYPE_NUM },
   { "igmpMaxSources", (void *) &glbWebVar.igmpMaxSources, CGI_TYPE_NUM },
   { "igmpMaxMembers", (void *) &glbWebVar.igmpMaxMembers, CGI_TYPE_NUM },
   { "igmpFastLeaveEnable", (void *) &glbWebVar.igmpFastLeaveEnable, CGI_TYPE_BOOL },
#endif
#if defined(DMP_X_BROADCOM_COM_IGMPSNOOP_1)
   { "enblIgmpSnp", (void *)&glbWebVar.enblIgmpSnp, CGI_TYPE_NUM },
   { "enblIgmpMode", (void *)&glbWebVar.enblIgmpMode, CGI_TYPE_NUM },
   { "enblIgmpLanToLanMcast", (void *)&glbWebVar.enblIgmpLanToLanMcast, CGI_TYPE_NUM },
#endif
#ifdef DMP_X_BROADCOM_COM_MLD_1
   { "enblMld", (void *)&glbWebVar.enblMld, CGI_TYPE_NUM },
   { "enblMldMcastSource", (void *)&glbWebVar.enblMldMcastSource, CGI_TYPE_NUM},
   { "mldVer", (void *) &glbWebVar.mldVer, CGI_TYPE_NUM },
   { "mldQI", (void *) &glbWebVar.mldQI, CGI_TYPE_NUM },
   { "mldQRI", (void *) &glbWebVar.mldQRI, CGI_TYPE_NUM },
   { "mldLMQI", (void *) &glbWebVar.mldLMQI, CGI_TYPE_NUM },
   { "mldRV", (void *) &glbWebVar.mldRV, CGI_TYPE_NUM },
   { "mldMaxGroups", (void *) &glbWebVar.mldMaxGroups, CGI_TYPE_NUM },
   { "mldMaxSources", (void *) &glbWebVar.mldMaxSources, CGI_TYPE_NUM },
   { "mldMaxMembers", (void *) &glbWebVar.mldMaxMembers, CGI_TYPE_NUM },
   { "mldFastLeaveEnable", (void *) &glbWebVar.mldFastLeaveEnable, CGI_TYPE_BOOL },
#endif
#if defined(DMP_X_BROADCOM_COM_MLDSNOOP_1)
   { "enblMldSnp", (void *)&glbWebVar.enblMldSnp, CGI_TYPE_NUM },
   { "enblMldMode", (void *)&glbWebVar.enblMldMode, CGI_TYPE_NUM },
   { "enblMldLanToLanMcast", (void *)&glbWebVar.enblMldLanToLanMcast, CGI_TYPE_NUM },
#endif
#ifdef SUPPORT_DSL_BONDING
   { "dslBonding", (void *)&glbWebVar.dslBonding, CGI_TYPE_NUM },
#endif
   { "bondingLineNum", (void *)&glbWebVar.bondingLineNum, CGI_TYPE_NUM },

   { "cfgL2tpAc", (void *)&glbWebVar.cfgL2tpAc, CGI_TYPE_NUM },   /* if set to 1, in create PPPoL2tpAC mode */
#ifdef DMP_X_BROADCOM_COM_L2TPAC_1
   { "tunnelName", (void *)glbWebVar.tunnelName, CGI_TYPE_STR },
   { "lnsIpAddress", (void *)glbWebVar.lnsIpAddress, CGI_TYPE_STR },
#endif

   { "cfgPptpAc", (void *)&glbWebVar.cfgPptpAc, CGI_TYPE_NUM },   /* if set to 1, in create PPTP mode */
#ifdef DMP_X_BROADCOM_COM_PPTPAC_1
   { "pptpTunnelName", (void *)glbWebVar.pptpTunnelName, CGI_TYPE_STR },
   { "pnsIpAddress", (void *)glbWebVar.pnsIpAddress, CGI_TYPE_STR },
#endif

#ifdef SUPPORT_MOCA
   { "mocaIfName", (void *)glbWebVar.mocaIfName, CGI_TYPE_STR },
   { "enblMocaPrivacy", (void *)&glbWebVar.enblMocaPrivacy, CGI_TYPE_NUM },
   { "mocaPassword", (void *)glbWebVar.mocaPassword, CGI_TYPE_STR },
   { "enblMocaAutoScan", (void *)&glbWebVar.enblMocaAutoScan, CGI_TYPE_NUM },
   { "mocaFrequency", (void *)&glbWebVar.mocaFrequency, CGI_TYPE_NUM },
#endif

#ifdef DMP_X_BROADCOM_COM_DBUSREMOTE_1
   { "dbusRemoteEnable", (void *)&glbWebVar.dbusRemoteEnable, CGI_TYPE_BOOL },
   { "dbusPort", (void *)&glbWebVar.dbusPort, CGI_TYPE_NUM },
#endif

#ifdef SUPPORT_HOMEPLUG
   { "homeplugNetworkPassword", (void *)&glbWebVar.homeplugNetworkPassword, CGI_TYPE_STR },
   { "homeplugAlias", (void *)&glbWebVar.homeplugAlias, CGI_TYPE_STR },
#endif

#ifdef SUPPORT_IEEE1905
   { "ieee1905IsEnabled",          (void *)&glbWebVar.ieee1905IsEnabled,    CGI_TYPE_NUM },
   { "ieee1905DeviceFriendlyName", (void *)&glbWebVar.ieee1905Dfname,       CGI_TYPE_STR },
   { "ieee1905IsRegistrar",        (void *)&glbWebVar.ieee1905IsRegistrar,  CGI_TYPE_NUM },
   { "ieee1905ApFreq24En",         (void *)&glbWebVar.ieee1905ApFreq24En,   CGI_TYPE_NUM },
   { "ieee1905ApFreq5En",          (void *)&glbWebVar.ieee1905ApFreq5En,    CGI_TYPE_NUM },
#endif

#ifdef SUPPORT_STUN
   { "stunEnable", (void *)&glbWebVar.stunEnable, CGI_TYPE_BOOL},
   { "stunServerAddress", (void *)&glbWebVar.stunServerAddress, CGI_TYPE_STR},
   { "stunServerPort", (void *)&glbWebVar.stunServerPort, CGI_TYPE_NUM},
   { "stunUser", (void *)&glbWebVar.stunUser, CGI_TYPE_STR},
   { "stunPwd", (void *)&glbWebVar.stunPwd, CGI_TYPE_STR},
   { "stunMaxKeepAlivePeriod", (void *)&glbWebVar.stunMaxKeepAlivePeriod, CGI_TYPE_NUM},
   { "stunMinKeepAlivePeriod", &glbWebVar.stunMinKeepAlivePeriod, CGI_TYPE_NUM},
#endif

#ifdef DMP_X_BROADCOM_COM_CONTAINER_1
   { "containerName", (void *)&glbWebVar.containerName, CGI_TYPE_STR},
#endif

   { NULL, NULL, CGI_TYPE_NONE }
};

void cgiSetVar(char *varName, char *varValue) {
   int i = 0;
   
   for ( ; CgiSetTable[i].variable != NULL; i++ )
   {
      if ( strcmp(varName, CgiSetTable[i].variable) == 0 ) {
         break;
      }
   }
         
   if ( CgiSetTable[i].variable != NULL ) {
      switch ( CgiSetTable[i].type ) {
      case CGI_TYPE_STR:
         strcpy((char *)CgiSetTable[i].value, varValue);
         break;
      case CGI_TYPE_BOOL:
         *((UBOOL8 *)CgiSetTable[i].value) = atoi(varValue);
         break;
      case CGI_TYPE_SHORT:
         *((SINT16 *)CgiSetTable[i].value) = atoi(varValue);
         break;
      case CGI_TYPE_NUM:
         *((SINT32 *)CgiSetTable[i].value) = atoi(varValue);
         break;
      case CGI_TYPE_VPI:
         glbOldVpi = glbWebVar.atmVpi;
         glbWebVar.atmVpi = atoi(varValue);
         break;
      case CGI_TYPE_VCI:
         glbOldVci = glbWebVar.atmVci;
         glbWebVar.atmVci = atoi(varValue);
         break;
      case CGI_TYPE_PORT:
         glbOldPort = glbWebVar.portId;
         glbWebVar.portId = atoi(varValue);
         /* mwang: there is some more port mapping code that needs to be ported over from cgimain.c */
         break;
      default:
         cmsLog_error("Cannot handle cgivar type %d", CgiSetTable[i].type);
         break;
      }
   }
   else {
     cmsLog_error("Could not find var %s", varName);
   }
}


void cgiGetCurrSessionKey(int argc __attribute__((unused)),
                          char **argv __attribute__((unused)),
                          char *varValue)
{

   do {
       glbCurrSessionKey = rand();
   } while (!glbCurrSessionKey);

   /* keep track of when the session key was issued, so we can detect old ones */
   cmsTms_get(&glbSessionKeyIssuedTms);

   if(varValue !=NULL)
      sprintf(varValue, "%d", glbCurrSessionKey);

#if defined(SUPPORT_WEB_SOCKETS)
   hsl_setSessionKey(glbCurrSessionKey, rand());
#endif

   return;
}

/*
 * Protect against cross site forgery attacks by checking if the browser
 * send us the last session key that we sent out.  Also, the session key
 * must not be too old. 
 */
CmsRet cgiValidateSessionKey(FILE *fs)
{
   UINT32 sessionKeyAge=0;
   CmsTimestamp nowTms;

   cmsTms_get(&nowTms);

   if ( !glbCurrSessionKey || (glbWebVar.recvSessionKey != glbCurrSessionKey) ||
         (sessionKeyAge = cmsTms_deltaInMilliSeconds(&nowTms, &glbSessionKeyIssuedTms)) > HTTPD_SESSION_KEY_VALID)
   {
      cmsLog_error("failed session key check.  Got %d, expected %d, age=%d max=%d",
            glbWebVar.recvSessionKey, glbCurrSessionKey,
            sessionKeyAge, HTTPD_SESSION_KEY_VALID);

      cgiWriteMessagePage(fs, "Message",
            "Invalid Session Key, please try again", 0);

      return CMSRET_REQUEST_DENIED;
   }
   glbWebVar.recvSessionKey = 0;
   glbCurrSessionKey =0;
   return CMSRET_SUCCESS;
}




/*
 * This section contains data model independent interface grouping code
 */

/* This first function must be outside of ifdefs due to reference from webVarTable */
void cgiGetBridgeInterfaceInfo(int argc __attribute__((unused)),
                               char **argv, char *varValue)
{
#if defined(SUPPORT_DM_LEGACY98)
    cgiGetBridgeInterfaceInfo_igd(argv, varValue);
#elif defined(SUPPORT_DM_HYBRID)
    cgiGetBridgeInterfaceInfo_igd(argv, varValue);
#elif defined(SUPPORT_DM_PURE181)
    cgiGetBridgeInterfaceInfo_dev2(argv, varValue);
#elif defined(SUPPORT_DM_DETECT)
   if (cmsMdm_isDataModelDevice2())
   {
     cgiGetBridgeInterfaceInfo_dev2(argv, varValue);
   }
   else
   {
      cgiGetBridgeInterfaceInfo_igd(argv, varValue);
   }
#endif
}


#ifdef SUPPORT_PORT_MAP

void cgiGetPMapGroupName(int argc __attribute__((unused)), char **argv __attribute__((unused)), char *varValue) {
   char info[BUFLEN_64];

   varValue[0] = info[0] = '\0';

   sprintf(info, "%s", glbWebVar.groupName);
   if ( info[0] != '\0' )
      strcat(varValue, info);
}


void cgiGetDhcpVendorId(int argc __attribute__((unused)),
                        char **argv __attribute__((unused)),
                        char *varValue)
{
#if defined(SUPPORT_DM_LEGACY98)
    cgiGetDhcpVendorId_igd(varValue);
#elif defined(SUPPORT_DM_HYBRID)
    cgiGetDhcpVendorId_igd(varValue);
#elif defined(SUPPORT_DM_PURE181)
    cgiGetDhcpVendorId_dev2(varValue);
#elif defined(SUPPORT_DM_DETECT)
   if (cmsMdm_isDataModelDevice2())
   {
     cgiGetDhcpVendorId_dev2(varValue);
   }
   else
   {
      cgiGetDhcpVendorId_igd(varValue);
   }
#endif
}


void cgiConfigPortMap(void) {
   cmsLog_error("not implemented yet");
}

#endif  /* SUPPORT_PORT_MAP */


#ifdef DMP_BRIDGING_1
/*
 * This section contains TR98 specific interface grouping code
 */
void cgiGetBridgeInterfaceInfo_igd(char **argv, char *varValue)
{
   char info[BUFLEN_64];
   char lanIfName[BUFLEN_32];
   L2BridgingFilterObject *filterObj=NULL;
   L2BridgingIntfObject *availIntfObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   SINT32 defaultBridgeRef = 0;
   UINT32 key;
   CmsRet          ret = CMSRET_SUCCESS;

   varValue[0] = '\0';

   if (strcmp(argv[2], "all") == 0) {
      InstanceIdStack availIntfIidStack = EMPTY_INSTANCE_ID_STACK;
      CmsRet r3;
      
      /*
       * List all the LAN interfaces that are available for adding to a new bridge,
       * i.e. only list filters that belong to the default bridge group.
       */
      INIT_INSTANCE_ID_STACK(&iidStack);
      while((ret = cmsObj_getNext(MDMOID_L2_BRIDGING_FILTER, &iidStack, (void **) &filterObj)) == CMSRET_SUCCESS)
      {
         if (filterObj->filterBridgeReference == defaultBridgeRef
            /*Lan side not being groupped virtual interface is also a candidate */
#ifdef SUPPORT_LANVLAN
            || filterObj->filterBridgeReference == -1
#endif
		 	)
         {
             cmsUtl_strtoul(filterObj->filterInterface, NULL, 0, &key);
             if ((r3 = dalPMap_getAvailableInterfaceByKey(key, &availIntfIidStack, &availIntfObj)) != CMSRET_SUCCESS)
             {
                cmsLog_error("could not find avail interface key %u", key);
             }
             else
             {
                /* only list available LAN interfaces */
                if (!strcmp(availIntfObj->interfaceType, MDMVS_LANINTERFACE))
                {
                   dalPMap_availableInterfaceReferenceToIfName(availIntfObj->interfaceReference, lanIfName);
#ifdef SUPPORT_LANVLAN
                   if (strstr(lanIfName, "eth"))
                      sprintf(info, "%s.%d|", lanIfName, filterObj->X_BROADCOM_COM_VLANIDFilter);
                   else				   
#endif
                   sprintf(info, "%s|", lanIfName);
                   cmsLog_debug("copying up %s->%s", availIntfObj->interfaceReference, lanIfName);
                   strcat(varValue, info);\
                }
             }
          }
          
          cmsObj_free((void **) &filterObj);
      }
   }
   else if (strcmp(argv[2], "group") == 0)
   {
      /*
       * Get all the interfaces on the bridge group in glbWebVar.groupName.
       * Is this used for edit?  We don't support edit anymore, so delete this code.
       */
      cmsLog_error("get group %s not supported", glbWebVar.groupName);
   }

   if (strlen(varValue) > 0)
   {
      // Remove the trailing | token seperator symbol
      *(varValue + strlen(varValue) - 1) = '\0';
   }

   cmsLog_debug("returning %s", varValue);
}


void cgiGetDhcpVendorId_igd(char *varValue)
{
   L2BridgingFilterObject  *filterObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret = CMSRET_SUCCESS;
   char *vendorId = NULL;
   char *currVendorId;
   int i;

   varValue[0] = '\0';

   if ((ret = dalPMap_getFilterDhcpVendorIdByBridgeName(glbWebVar.groupName, &iidStack, &filterObj)) != CMSRET_SUCCESS)
   {
      /* there is no DHCP Vendor Id filter for this bridge, so just return. */
      return;
   }
   
   vendorId = cmsUtl_getDhcpVendorIdsFromAggregateString(filterObj->sourceMACFromVendorClassIDFilter);

   /* I'm done with the filterObj */
   cmsObj_free((void **) &filterObj);
   
   for (i=0; i < MAX_PORTMAPPING_DHCP_VENDOR_IDS; i++)
   {
      currVendorId = &(vendorId[i*(DHCP_VENDOR_ID_LEN + 1)]);
      if (*currVendorId != '\0')
      { 
         strcat(varValue, currVendorId);
         strcat(varValue, "|");
      }
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(vendorId);

   if (strlen(varValue) > 0)
   {
      // Remove the trailing | token seperator symbol
      *(varValue + strlen(varValue) - 1) = '\0';
   }

   cmsLog_debug("returning %s", varValue);
}


#else


void cgiGetBridgeInterfaceInfo_igd(char **argv, char *varValue) {
   char info[BUFLEN_64];
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   _LanEthIntfObject *ethIntfObj=NULL;
   CmsRet          ret = CMSRET_SUCCESS;

   varValue[0] = '\0';

   if (strcmp(argv[2], "all") == 0) {
      
      /*
       * List all the ETH LAN interfaces 
       */
      while ((ret = cmsObj_getNext(MDMOID_LAN_ETH_INTF, &iidStack, (void **) &ethIntfObj)) == CMSRET_SUCCESS)
      {
         if (cmsUtl_strncmp(ethIntfObj->X_BROADCOM_COM_IfName, "eth", strlen("eth")) == 0)
         {
            sprintf(info, "%s|", ethIntfObj->X_BROADCOM_COM_IfName);
            strcat(varValue, info);
         }
         cmsObj_free((void **) &ethIntfObj);
      }
   }

   // Remove the trailing | token seperator symbol
   *(varValue + strlen(varValue) - 1) = '\0';

}

#endif /* DMP_BRIDGING_1 */


#ifdef SUPPORT_LANVLAN
void cgiGetAllL2EthIntf(int argc __attribute__((unused)), char **argv __attribute__((unused)), char *varValue) 
{
   cmsLog_debug("Enter");

   varValue[0] = '\0';
   qdmEth_getAllL2EthIntfNameLocked(varValue, BUFLEN_64);
   cmsLog_debug("varValue=%s", varValue);

   return;
}
#endif


//**************************************************************************
// Function Name: bcmIsMarkStrChar
// Description  : verify the given character is used to mark the begining or
//                ending of string or not.
// Parameters   : c -- the given character.
// Returns      : TRUE or FALSE.
//**************************************************************************
int bcmIsMarkStrChar(char c) {
   // need to add '\0' as termination character to speChars[]
   char specChars[] = { '\'', '"', '\\', '\0' };
   int len = strlen(specChars);
   int i = 0;
   int ret = FALSE;

   for ( i = 0; i < len; i++ )
      if ( c == specChars[i] )
         break;

   if ( i < len )
      ret = TRUE;

   return ret;
}



//**************************************************************************
// Function Name: bcmProcessMarkStrChars
// Description  : use backslash in front one of the escape codes to process
//                marked string characters.
//                (a'b"c => a\'b\"c)
// Parameters   : str - the string that needs to process its special chars.
// Returns      : none.
//**************************************************************************
void bcmProcessMarkStrChars(char *str) {
   if ( str == NULL ) return;
   if ( str[0] == '\0' ) return;

   char buf[BUFLEN_256];
   int len = strlen(str);
   int i = 0, j = 0;

   for ( i = 0; i < len; i++ ) {
      if ( bcmIsMarkStrChar(str[i]) == TRUE )
         buf[j++] = '\\';
      buf[j++] = str[i];
   }

   buf[j] = '\0';
   strcpy(str, buf);
}

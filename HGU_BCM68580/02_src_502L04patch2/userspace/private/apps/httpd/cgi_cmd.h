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

#ifndef __CGI_CMD_H__
#define __CGI_CMD_H__

#include <stdio.h>
#include <fcntl.h>
#include "httpd.h"

/********************** Global Types ****************************************/

typedef void (*CGI_CMD_HDLR) (char *path, FILE *fs);

typedef struct {
   char *cgiCmdName;
   CGI_CMD_HDLR cgiCmdHdlr;
   HttpLoginType authLevel;
} CGI_CMD, *PCGI_CMD;

typedef struct {
   char *htmlPageName;
   HttpLoginType authLevel;
} CGI_HtlmPage, *PCGI_HtlmPage;

typedef enum 
{
   CMD_TABLE=0,
   HTML_TABLE=1
} TableType;

void do_cmd_cgi(char *path, FILE *fs);
void do_dhcpd_cgi(char *path, FILE *fs);
void cgiRtRouteCfg(char *path, FILE *fs);
void cgiArpView(char *path, FILE *fs);
void cgiBackupSettings(char *path, FILE *fs);
void cgiPmdSettings(char *path, FILE *fs);
void cgiBackupPmdSettings(char *path, FILE *fs);
void cgiLogView(char *path, FILE *fs);
void cgiScVrtSrv(char *path, FILE *fs);
void cgiScPrtTrg(char *path, FILE *fs);
void cgiScOutFlt(char *path, FILE *fs);
void cgiScInFlt(char *path, FILE *fs);
void cgiScAccCntr(char *path, FILE *fs);
void cgiScSrvCntr(char *path, FILE *fs);
void cgiScMacFlt(char *path, FILE *fs);
void cgiScDmzHost(char *path, FILE *fs);
#ifdef SUPPORT_QUICKSETUP
void cgiAutoAtmCfg(char *path, FILE *fs);
#endif
void cgiWanCfg(char *path, FILE *fs);
void cgiWanIfc(char *path, FILE *fs);
void cgiWanL3Edit(char *path, FILE *fs);
void cgiWanSrvc(char *query, FILE *fs);


#ifdef SUPPORT_DSL
void writeDslXtmCfgScript(FILE *fs, char *addLoc, char *removeLoc);
void writeStsXtmScript(FILE *fs);
void writeXtmFooter(FILE *fs, SINT32 layer2Count);
void cgiDslAtmCfg(char *path, FILE *fs);

/** use C code to handle ATM config web pages */
void cgiDslAtmCfgView(FILE *fs);
void cgiDslAtmCfgView_igd(FILE *fs);
void cgiDslAtmCfgView_dev2(FILE *fs);

#if defined(SUPPORT_DM_LEGACY98)
#define cgiDslAtmCfgView(fs)   cgiDslAtmCfgView_igd((fs))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiDslAtmCfgView(fs)   cgiDslAtmCfgView_igd((fs))
#elif defined(SUPPORT_DM_PURE181)
#define cgiDslAtmCfgView(fs)   cgiDslAtmCfgView_dev2((fs))
#elif defined(SUPPORT_DM_DETECT)
#define cgiDslAtmCfgView(fs)   (cmsMdm_isDataModelDevice2() ? \
                                cgiDslAtmCfgView_dev2((fs)) : \
                                cgiDslAtmCfgView_igd((fs)))
#endif

#endif   /* SUPPORT_DSL */



#ifdef SUPPORT_PTM
void cgiDslPtmCfg(char *path, FILE *fs);

/** Use C code to handle PTM config web pages */
void cgiDslPtmCfgView(FILE *fs);
void cgiDslPtmCfgView_igd(FILE *fs);
void cgiDslPtmCfgView_dev2(FILE *fs);

#if defined(SUPPORT_DM_LEGACY98)
#define cgiDslPtmCfgView(fs)   cgiDslPtmCfgView_igd((fs))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiDslPtmCfgView(fs)   cgiDslPtmCfgView_igd((fs))
#elif defined(SUPPORT_DM_PURE181)
#define cgiDslPtmCfgView(fs)   cgiDslPtmCfgView_dev2((fs))
#elif defined(SUPPORT_DM_DETECT)
#define cgiDslPtmCfgView(fs)   (cmsMdm_isDataModelDevice2() ? \
                                cgiDslPtmCfgView_dev2((fs)) : \
                                cgiDslPtmCfgView_igd((fs)))
#endif

#endif   /* SUPPORT_PTM */

#ifdef SUPPORT_ETHWAN
void cgiEthWanCfg(char *path, FILE *fs);
#endif
#if defined(DMP_X_BROADCOM_COM_MOCAWAN_1) || defined(DMP_DEVICE2_MOCA_1)
void cgiMocaWanCfg(char *path, FILE *fs);
#endif
#ifdef DMP_X_BROADCOM_COM_GPONWAN_1
void cgiGponWanCfg(char *path, FILE *fs);
#endif
#ifdef DMP_X_BROADCOM_COM_EPONWAN_1
void cgiEponWanCfg(char *path, FILE *fs);
#endif
#ifdef DMP_X_BROADCOM_COM_L2TPAC_1
void cgiL2tpAcWanCfg(char *path, FILE *fs);
#endif
#ifdef DMP_X_BROADCOM_COM_PPTPAC_1
void cgiPptpAcWanCfg(char *path, FILE *fs);
#endif
#ifdef SUPPORT_URLFILTER
void do_urlfilter_cgi(char *path, FILE *fs);
#endif
#ifdef SUPPORT_POLICYROUTING
void cgiPolicyRouting(char *path, FILE *fs);
#endif

#ifdef DMP_X_BROADCOM_COM_DNSPROXY_1 /* aka SUPPORT_DNSPROXY */
void cgiDnsProxyCfg(char *path, FILE *fs);
#endif
#ifdef SUPPORT_PORT_MAP
void cgiPortMapCfg(char *path, FILE *fs);
#endif
void cgiLanPortsCfg(char *path, FILE *fs);
#ifdef SUPPORT_HOMEPLUG
void cgiHomePlugCfg(char *path, FILE *fs);
#endif
#if defined(SUPPORT_IEEE1905) && defined(DMP_DEVICE2_IEEE1905BASELINE_1)
void cgiIeee1905Status(char *path, FILE *fs);
#endif

void cgiStsXtm(char *path, FILE *fs);
void cgiStsWan(char *path, FILE *fs);
void bcmUpdateStsAtm(int reset);
void cgiDisplayStsXtm_igd(FILE *fs);
void cgiDisplayStsXtm_dev2(FILE *fs);

#if defined(SUPPORT_DM_LEGACY98)
#define cgiDisplayStsXtm(fs)   cgiDisplayStsXtm_igd((fs))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiDisplayStsXtm(fs)   cgiDisplayStsXtm_igd((fs))
#elif defined(SUPPORT_DM_PURE181)
#define cgiDisplayStsXtm(fs)   cgiDisplayStsXtm_dev2((fs))
#elif defined(SUPPORT_DM_DETECT)
#define cgiDisplayStsXtm(fs)   (cmsMdm_isDataModelDevice2() ? \
                                cgiDisplayStsXtm_dev2((fs)) : \
                                cgiDisplayStsXtm_igd((fs)))
#endif

#ifdef BRCM_WLAN
void cgiWlMacFlt(char *query, FILE *fs);
void cgiWlWds(char *query, FILE *fs);
void cgiWlStationList(char *query, FILE *fs);
void cgiWlQos(char *path, FILE *fs);
#ifdef BCMWAPI_WAI
void cgiWlWapiAs(char *path, FILE *fs);
#endif
#endif /* BRCM_WLAN */
#ifdef BRCM_VOICE_SUPPORT
void cgiVoiceApply(char *path, FILE *fs);
void cgiVoiceStart(char *path, FILE *fs);
void cgiVoiceStop(char *path, FILE *fs);
void cgiVoiceRefresh(char *path, FILE *fs);
void cgiVoiceRestore(char *path, FILE *fs);
void cgiVoiceSetDefault(char *path, FILE *fs);
void cgiVoiceCctkUpload(char *path, FILE *fs);
#ifdef DMP_VOICE_SERVICE_2
void cgiVoiceVoipProfSelect(char *path, FILE *fs);
void cgiVoiceCodecProfSelect(char *path, FILE *fs);
#endif
#   ifdef DMP_X_BROADCOM_COM_DECTENDPOINT_1
void cgiDectStart(char *path, FILE *fs);
void cgiDectStop(char *path, FILE *fs);
void cgiDectSetRFID(char *path, FILE *fs);
void cgiVoiceDectOpenRegWnd(char *path, FILE *fs);
void cgiVoiceDectCloseRegWnd(char *path, FILE *fs);
void cgiVoiceDectSetAc(char *path, FILE *fs);
void cgiVoiceDectDelHset(char *path, FILE *fs);
void cgiVoiceDectPingHset(char *path, FILE *fs);
void cgiVoiceDectPingAllHset(char *path, FILE *fs);
#   endif
#endif
#ifdef DMP_STORAGESERVICE_1
void cgiStorageServiceCfg(char *query, FILE *fs); 
#ifdef SUPPORT_SAMBA
void cgiStorageUserAccountCfg(char *query, FILE *fs); 
#endif
#endif
#ifdef SUPPORT_DDNSD
void cgiDDnsMngr(char *query, FILE *fs);
#endif
void cgiAdslCfgTestMode(char *query, FILE *fs);
void cgiAdslCfgTones(char *query, FILE *fs);
void cgiEngInfo(char *query, FILE *fs);

#ifdef SUPPORT_IPSEC
void cgiIPSec(char *query, FILE *fs);
void cgiIPSecSettings(char *query, FILE *fs);
#endif
#ifdef SUPPORT_CERT
void cgiCertLocal(char *path, FILE *fs);
void cgiCertCA(char *path, FILE *fs);
#endif
#ifdef SUPPORT_MOCA
void cgiStsMoca(char *path, FILE *fs);
void cgiStsMoca_igd(char *path, FILE *fs);
void cgiStsMoca_dev2(char *path, FILE *fs);
void writeStsMocaScript(FILE *fs);
#endif
#ifdef DMP_X_BROADCOM_COM_PWRMNGT_1 
void cgiPowerManagement(char *path, FILE *fs);
#endif
#ifdef DMP_X_BROADCOM_COM_STANDBY_1
void cgiStandby(char *path, FILE *fs);
void cgiStandbyDemo(char *path, FILE *fs);
#endif 

#ifdef SUPPORT_MODSW_WEBUI
#ifdef DMP_DEVICE2_SM_BASELINE_1
void cgiModSwEE(char *query, FILE *fs);
void cgiModSwDU(char *query, FILE *fs);
void cgiModSwEU(char *query, FILE *fs);
void cgiModSwLogDU(char *query, FILE *fs);
void cgiModSwLogEU(char *query, FILE *fs);
void cgiModSwLogEE(char *query, FILE *fs);
#include "cms_msg.h"
void cgiModSw_handleResponse(CmsMsgHeader *msgRes);
#endif
#ifdef DMP_DEVICE2_X_BROADCOM_COM_MODSW_LINUXPFP_1
void cgiModSwPFP(char *query, FILE *fs);
#endif
#endif  /* SUPPORT_MODSW_WEBUI */

void cgiSecLogView(char *query __attribute__((unused)), FILE *fs);
void cgiSecLogText(char *query __attribute__((unused)), FILE *fs);
void cgiSecLogReset(char *query __attribute__((unused)), FILE *fs);
#ifdef DMP_X_BROADCOM_COM_AUTODETECTION_1 
void cgiAutoDetection(char *query, FILE *fs);
#endif /*  DMP_X_BROADCOM_COM_AUTODETECTION_1 */

#ifdef SUPPORT_IPV6
void cgiIpTunnelCfg(char *query, FILE *fs) ;
#endif

void cgiWifiWanCfg(char * path, FILE * fs);

#if defined(DMP_X_BROADCOM_COM_SPDSVC_1)
void cgiSpeedTestCmd(char *query, FILE * fs);
#endif

#if defined(SUPPORT_XMPP)
void cgiXmppConnCfg(char *query, FILE * fs);
#endif

#ifdef DMP_DEVICE2_USBHOSTSBASIC_1
void cgiUsbHostsCfg(char *query, FILE *fs);
#endif

#ifdef DMP_X_BROADCOM_COM_EPON_1  
void cgiEponLoidCfg(char *query, FILE *fs);
#endif

#ifdef DMP_X_BROADCOM_COM_OPENVSWITCH_1
void cgiOpenVSCfg(char *query, FILE * fs);
#endif
#endif /* __CGI_CMD_H__ */

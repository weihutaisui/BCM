/*
* <:copyright-BRCM:2016:proprietary:standard
*
*    Copyright (c) 2016 Broadcom
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
* :>
*
****************************************************************************
*
*  Filename: cli2_voice.c
*
****************************************************************************
*  Description:
*
*
****************************************************************************/

/****************************************************************************
*
*  cli2_voice.c
*
*  PURPOSE:
*
*  NOTES:
*
****************************************************************************/

#ifdef SUPPORT_CLI_CMD

#ifdef BRCM_VOICE_SUPPORT
#ifdef DMP_VOICE_SERVICE_2

/* ---- Include Files ---------------------------------------------------- */

#include "cms_util.h"
#include "cms_core.h"
#include "cms_cli.h"
#include "cli.h"
#include "cms_msg.h"
#include "dal_voice.h"


/* ---- Public Variables ------------------------------------------------- */

/* ---- Constants and Types ---------------------------------------------- */

#define VOIP_PARMNAME_ARG_INDEX  1  /* Index of parameter name on the commandline string */
#define VOIP_FIRST_PARMARG_INDEX  2     /* Index of first argument after param name on the commandline string */

#define MAX_CLICMD_NAME_SIZE     30

/* DAL function <--> set parameter mapping element */
typedef struct  {
   const char *name;
   CmsRet (*dalFunc)( DAL_VOICE_PARMS *args, char *value );
   CmsRet (*convertFunc)( DAL_VOICE_PARMS *args, int *value ); /* convert second parameter from number to instance */
   int numArgs;
   UBOOL8 global;     /* Indicates a parameter as per voice profile for MDM but a global parameter for Call manager */
   const char *info;
   const char *syntax;
} VOICE_DAL_MAP;

/* ---- Private Variables ------------------------------------------------ */

/* Table of DAL function mapping */
static VOICE_DAL_MAP voiceDalCliMap[] =
{

   { "-------------",
     NULL,          NULL,
     0, FALSE,
     "-------------",  "---- Global parameters ----"
   },

#ifdef SIPLOAD
   { "cctktracelvl",    dalVoice_SetCCTKTraceLevel,      NULL,
     1, TRUE,
     "CCTK tracelevel",   "<Info|Warn|Debug|Off>"
   },

   { "cctktracegrp",    dalVoice_SetCCTKTraceGroup,      NULL,
     1, TRUE,
     "CCTK concat tracegroups", "<CCTK|SCE|Trans|SDP|SIP|Misc|All|None>"
   },
#endif /* SIPLOAD */

#ifdef DMP_X_ITU_ORG_GPON_1
   { "mgtProt",        dalVoice_SetManagementProtocol,   NULL,
     1, TRUE,
     "Protocol used to manage Voice",
     "<TR69|OMCI>"
   },
#endif /* DMP_X_ITU_ORG_GPON_1 */

   { "loglevel",       dalVoice_SetLoggingLevel,       NULL,
     2, TRUE,
     "Voice module-specific log level",    "<general|dsphal|slicslac|cmgr|disp|sipcctk|bos|ept|cms|prov|lhapi> <0-7>" },


   { "defaults",
     dalVoice_SetDefaults,       NULL,
      0, FALSE,
     "Default VoIP setup", "<None>" },

   { "boundIfname",
     dalVoice_SetBoundIfName,    NULL,
     2, FALSE,
     "voice network interface",
     "<srvPrv#> <LAN|Any_WAN|(WAN IfName, e.g. nas_0_0_35)>"},

   { "ipAddrFamily",
     dalVoice_SetIpFamily,       NULL,
     2, FALSE,
     "IP address family", "<srvPrv#> <IPv4|IPv6>"},

#ifdef BRCM_SIP_VOICE_DNS
   { "dnsAddr",     dalVoice_SetDnsServerAddr,  NULL,
     2, FALSE,
     "Voice DNS IP addresss", "<srvPrv#> <IP>" },
#endif // BRCM_SIP_VOICE_DNS

   /* Service provider specific parameters */
   { "locale",      dalVoice_SetRegion,         NULL,
     2, FALSE,
     "2 or 3 character code", "<srvPrv#> <region>" },

#ifdef SIPLOAD
   /* sip network setting */
   { "-------------",
     NULL,          NULL,
     0, FALSE,
     "-------------",  "---- SIP Network parameters ----"
   },

   { "transport",
     dalVoice_SetSipTransport,    dalVoice_mapNetworkNumToInst,
     3,  FALSE,
     "transport protocol", "<srvPrv#> <network#> <UDP|TCP|TLS>"
   },

   { "reg",
     dalVoice_SetSipRegistrarServer,  dalVoice_mapNetworkNumToInst,
     3, FALSE,
     "SIP registrar server", "<srvPrv#> <network#> <hostName|IP>"
   },

   { "regPort",
     dalVoice_SetSipRegistrarServerPort,  dalVoice_mapNetworkNumToInst,
     3, FALSE,
     "SIP registrar server port",  "<srvPrv#> <network#> <port>"
   },

   { "proxy",
     dalVoice_SetSipProxyServer,     dalVoice_mapNetworkNumToInst,
     3, FALSE,
     "SIP proxy server",   "<srvPrv#> <network#> <hostName|IP>"
   },

   { "proxyPort",
     dalVoice_SetSipProxyServerPort,      dalVoice_mapNetworkNumToInst,
     3, FALSE,
     "SIP proxy server port", "<srvPrv#> <network#> <port>"
   },

   { "obProxy",
     dalVoice_SetSipOutboundProxy,   dalVoice_mapNetworkNumToInst,
     3, FALSE,
     "SIP outbound proxy",  "<srvPrv#> <network#> <hostName|IP>"
   },

   { "obProxyPort",
     dalVoice_SetSipOutboundProxyPort,  dalVoice_mapNetworkNumToInst,
     3, FALSE,
     "SIP outbound proxy port",  "<srvPrv#> <network#> <port>"
   },

   { "sipDomain",
     dalVoice_SetSipUserAgentDomain,    dalVoice_mapNetworkNumToInst,
     3, FALSE,
     "SIP user agent domain",  "<srvPrv#> <network#> <CPE_domainName>"
   },

   { "sipPort",
     dalVoice_SetSipUserAgentPort,      dalVoice_mapNetworkNumToInst,
     3, FALSE,
     "SIP user agent port",       "<srvPrv#> <network#> <port>"
   },

   { "secSipDomain",
     dalVoice_SetSipSecDomainName, dalVoice_mapNetworkNumToInst,
     3, FALSE,
     "SIP Secondary domain name",   "<srvPrv#> <network#> <CPE_domainName>"
   },

   { "secProxyAddr",
     dalVoice_SetSipSecProxyAddr, dalVoice_mapNetworkNumToInst,
     3, FALSE,
     "SIP Secondary proxy IP",   "<srvPrv#> <network#> <IP>"
   },

   { "secProxyPort",
     dalVoice_SetSipSecProxyPort, dalVoice_mapNetworkNumToInst,
     3, FALSE,
     "SIP Secondary proxy port",  "<srvPrv#> <network#> <port>"
   },

   { "secObProxyAddr",
     dalVoice_SetSipSecOutboundProxyAddr, dalVoice_mapNetworkNumToInst,
     3, FALSE,
     "SIP Secondary outbound proxy IP",   "<srvPrv#> <network#> <IP>"
   },

   { "secObProxyPort",
     dalVoice_SetSipSecOutboundProxyPort, dalVoice_mapNetworkNumToInst,
     3, FALSE,
     "SIP Secondary outbound proxy port",  "<srvPrv#> <network#> <port>"
   },

   { "secRegistrarAddr",
     dalVoice_SetSipSecRegistrarAddr, dalVoice_mapNetworkNumToInst,
     3, FALSE,
     "SIP Secondary registrar IP",   "<srvPrv#> <network#> <IP>"
   },

   { "secRegistrarPort",
     dalVoice_SetSipSecRegistrarPort, dalVoice_mapNetworkNumToInst,
     3, FALSE,
     "SIP Secondary registrar port",  "<srvPrv#> <network#> <port>"
   },

   { "confURI",
     dalVoice_SetSipConferencingURI,    dalVoice_mapNetworkNumToInst,
     3, FALSE,
     "SIP conferencing URI",      "<srvPrv#> <network#> <hostName>"
   },

   { "confOption",
     dalVoice_SetSipConferencingOption,  dalVoice_mapNetworkNumToInst,
     3, FALSE,
     "SIP conferencing option",   "<srvPrv#> <network#> <Local|Refer participants|Refer server>"
   },

   { "codecList",
     dalVoice_SetSipNetworkCodecList,      dalVoice_mapNetworkNumToInst,
     3, FALSE,
     "codec priority list",         "<srvPrv#> <network#> <codec(1)[,codec(2)]>"
   },

   { "sipDSCPMark",
     dalVoice_SetSipDSCPMark,    dalVoice_mapNetworkNumToInst,
     3, FALSE,
     "SIP outgoing DSCP mark",   "<srvPrv#> <network#> <mark>"
   },

   { "timerB",
     dalVoice_SetSipTimerB,      dalVoice_mapNetworkNumToInst,
     3, FALSE,
     "SIP protocol B timer",    "<srvPrv#> <network#> <time in ms>"
   },

   { "timerF",
     dalVoice_SetSipTimerF,      dalVoice_mapNetworkNumToInst,
     3, FALSE,
     "SIP protocol F timer",     "<srvPrv#> <network#> <time in ms>"
   },

   { "regRetryInt",
     dalVoice_SetSipRegisterRetryInterval, dalVoice_mapNetworkNumToInst,
     3, FALSE,
     "SIP register retryinterval",   "<srvPrv#> <network#> <seconds>"
   },

   { "regExpires",
     dalVoice_SetSipRegisterExpires, dalVoice_mapNetworkNumToInst,
     3, FALSE,  "Register expires hdr val",    "<srvPrv#> <network#> <seconds>"
   },

   { "tagMatching",
     dalVoice_SetSipToTagMatching,   dalVoice_mapNetworkNumToInst,
     3, FALSE,
     "SIP to tag matching",         "<srvPrv#> <on|off>"
   },

   /* sip client parameters */
   { "-------------",
     NULL,          NULL,
     0, FALSE,
     "-------------",  "---- SIP client parameters ----"
   },

   { "lineStatus",
     dalVoice_SetVlEnable,          dalVoice_mapAcntNumToClientInst,
     3, FALSE,
     "Activate sip line",      "<srvPrv#> <client#> <on|off>"
   },

   { "extension",
     dalVoice_SetVlSipURI,          dalVoice_mapAcntNumToClientInst,
     3, FALSE,
     "SIP extension",    "<srvPrv#> <client#> <URI>"
   },

   { "dispName",
     dalVoice_SetVlCFCallerIDName,  dalVoice_mapAcntNumToClientInst,
     3, FALSE,
     "SIP Display Name",  "<srvPrv#> <client#> <Name>"
   },

   { "authName",
     dalVoice_SetVlSipAuthUserName, dalVoice_mapAcntNumToClientInst,
     3, FALSE,
     "SIP auth name",   "<srvPrv#> <client#> <name>"
   },

   { "authPwd",
     dalVoice_SetVlSipAuthPassword, dalVoice_mapAcntNumToClientInst,
     3, FALSE,
     "SIP auth password",   "<srvPrv#> <client#> <pwd>"
   },

   { "T38",
     dalVoice_SetT38Enable,         dalVoice_mapAcntNumToClientInst,
     3, FALSE,
     "enable/disable T38",  "<srvPrv#> <client#> on|off"
   },

   { "Contact",
     dalVoice_SetSipContactUri,     dalVoice_mapAcntNumToClientInst,
     3, FALSE,
     "set override SIP contact header", "<srvPrv#> <client#> <URI>"
   },
#endif /* SIPLOAD */

   /* voip profile parameters */
   { "-------------",
     NULL,          NULL,
     0, FALSE,
     "-------------",  "---- VoIP Profile parameters ----"
   },

   { "DTMFMethod",
     dalVoice_SetDTMFMethod,        dalVoice_mapVoipProfNumToInst,
     3, FALSE,
     "DTMF digit passing method",   "<srvPrv#> <profile#> <InBand|RFC4733|SIPInfo>"
   },

   { "hookFlashMethod",
     dalVoice_SetHookFlashMethod,   dalVoice_mapVoipProfNumToInst,
     3, FALSE,
     "Hook flash method",            "<srvPrv#> <profile#> <SIPInfo|None>"
   },

   { "MinRtpPort",
     dalVoice_SetRtpLocalPortMin,       dalVoice_mapVoipProfNumToInst,
     3, FALSE,
     "RTP port min",  "<srvPrv#> <profile#> <min port>"
   },

   { "MaxRtpPort",
     dalVoice_SetRtpLocalPortMax,       dalVoice_mapVoipProfNumToInst,
     3, FALSE,
     "RTP port max",  "<srvPrv#> <profile#> <max port>"
   },


   { "rtpDSCPMark",
     dalVoice_SetRtpDSCPMark,       dalVoice_mapVoipProfNumToInst,
     3, FALSE,
     "RTP outgoing DSCP mark",  "<srvPrv#> <profile#> <mark>"
   },

   { "srtpEnable",
     dalVoice_SetSrtpEnable,        dalVoice_mapVoipProfNumToInst,
     3, FALSE,
     "SRTP enable",       "<srvPrv#> <profile#> <yes|no>"
   },

   { "srtpOption",
     dalVoice_SetSrtpOption,        dalVoice_mapVoipProfNumToInst,
     3, FALSE,
     "SRTP usage option",       "<srvPrv#> <profile#> <Mandatory|Optional>"
   },

   { "V18",
     dalVoice_SetV18Enable,         dalVoice_mapVoipProfNumToInst,
     3, FALSE,
     "enable/disable V.18 detection",  "<srvPrv#> <profile#> on|off"
   },

   /* codec profile parameters */
   { "-------------",
     NULL,          NULL,
     0, FALSE,
     "-------------",  "---- Codec Profile parameters ----"
   },

   { "vad",
     dalVoice_SetSilenceSuppression,  NULL,
     2, TRUE,
     "enable vad",  "<srvPrv#> <on|off>"
   },

   { "pTime",
     dalVoice_SetCodecProfPacketPeriod,  dalVoice_mapCpNumToInst,
     3, FALSE,
     "packetization period",    "<srvPrv#> <codecProfile#> <pTime>"
   },

   /* calling feature parameters */
   { "-------------",
     NULL,          NULL,
     0, FALSE,
     "-------------",  "---- Call Feature parameters ----"
   },

   { "MWIEnable",
     dalVoice_SetVlCFMWIEnable,     dalVoice_mapCallFeatureSetNumToInst,
     3, FALSE,
     "Msg Waiting Indication",  "<srvPrv#> <Feature#> <on|off>"
   },
   { "cfwdNum",
     dalVoice_SetVlCFCallFwdNum,    dalVoice_mapCallFeatureSetNumToInst,
     3, FALSE,
     "call forward number",     "<srvPrv#> <Feature#> <number>"
   },

   { "cfwdAll",
     dalVoice_SetVlCFCallFwdAll,    dalVoice_mapCallFeatureSetNumToInst,
     3, FALSE,
     "call forward all",         "<srvPrv#> <feature#> <on|off>"
   },

   { "cfwdNoAns",
     dalVoice_SetVlCFCallFwdNoAns,  dalVoice_mapCallFeatureSetNumToInst,
     3, FALSE,
     "call forward no answer",   "<srvPrv#> <feature#> <on|off>"
   },

   { "cfwdBusy",
     dalVoice_SetVlCFCallFwdBusy,   dalVoice_mapCallFeatureSetNumToInst,
     3, FALSE,
     "call forward busy",         "<srvPrv#> <feature#> <on|off>"
   },

   { "callWait",
     dalVoice_SetVlCFCallWaiting,   dalVoice_mapCallFeatureSetNumToInst,
     3, FALSE,
     "call waiting",               "<srvPrv#> <feature#> <on|off>"
   },

   { "callreturn",
     dalVoice_SetVlCFCallReturn,   dalVoice_mapCallFeatureSetNumToInst,
     3, FALSE,
     "call return",               "<srvPrv#> <feature#> <on|off>"
   },

   { "anonBlck",
     dalVoice_SetVlCFAnonCallBlck,  dalVoice_mapCallFeatureSetNumToInst,
     3, FALSE,
     "Anonymous call rcv blcking", "<srvPrv#> <feature#> <on|off>"
   },

   { "anonCall",
     dalVoice_SetVlCFAnonymousCalling,  dalVoice_mapCallFeatureSetNumToInst,
     3, FALSE,
     "Anonymous outgng calls",      "<srvPrv#> <feature#> <on|off>"
   },

   { "DND",
     dalVoice_SetVlCFDoNotDisturb,  dalVoice_mapCallFeatureSetNumToInst,
     3, FALSE,
     "do not disturb",      "<srvPrv#> <feature#> <on|off>"
   },

   { "CCBS",
     dalVoice_SetVlCFCallCompletionOnBusy, dalVoice_mapCallFeatureSetNumToInst,
     3, FALSE,
     "Call completion on busy",    "<srvPrv#> <feature#> <on|off>"
   },

   { "speedDial",
     dalVoice_SetVlCFSpeedDial,       dalVoice_mapCallFeatureSetNumToInst,
     3, FALSE,
     "Speed dial",         "<srvPrv#> <feature#> <on|off>"
   },

   { "warmLine",
     dalVoice_SetVlCFWarmLine ,        dalVoice_mapCallFeatureSetNumToInst,
     3, FALSE,
     "Warm line",          "<srvPrv#> <feature#> <on|off>"
   },

   { "warmLineNum",
     dalVoice_SetVlCFWarmLineNum,       dalVoice_mapCallFeatureSetNumToInst,
     3, FALSE,
     "Warm line number",   "<srvPrv#> <feature#> <number>"
   },

   { "callBarring",
     dalVoice_SetVlCFCallBarring,        dalVoice_mapCallFeatureSetNumToInst,
     3, FALSE,
     "Call barring",       "<srvPrv#> <feature#> <on|off>"
   },

   { "callBarrPin",
     dalVoice_SetVlCFCallBarringPin,      dalVoice_mapCallFeatureSetNumToInst,
     3, FALSE,
     "Call barring pin",   "<srvPrv#> <feature#> <number>"
   },

   { "callBarrDigMap",
     dalVoice_SetVlCFCallBarringDigitMap, dalVoice_mapCallFeatureSetNumToInst,
     3, FALSE,
     "Call barring digit map",   "<srvPrv#> <feature#> <digitmap>"
   },

   { "netPrivacy",      dalVoice_SetVlCFNetworkPrivacy,      dalVoice_mapCallFeatureSetNumToInst,
     3, FALSE,
     "Network privacy",           "<srvPrv#> <feature#> <on|off>"
   },

   { "callredial",      dalVoice_SetVlCFCallRedial,      dalVoice_mapCallFeatureSetNumToInst,
     3, FALSE,
     "Call Redial",           "<srvPrv#> <feature#> <on|off>"
   },

   { "euroFlashEnable",
     dalVoice_SetEuroFlashEnable, NULL,
     2, FALSE,
     "European flash enable", "<srvPrv#> <on|off>"
   },

   { "-------------",
     NULL,          NULL,
     0, FALSE,
     "-------------",  "---- POTS FXS parameters ----"
   },

   { "rxGain",          dalVoice_SetVlVPReceiveGain,         dalVoice_mapPotsFxsNumToInst,
     3, FALSE,
     "rxGain (dB)",     "<srvPrv#> <FXS#> <rxGain>"
   },

   { "txGain",          dalVoice_SetVlVPTransmitGain,        dalVoice_mapPotsFxsNumToInst,
     3, FALSE,
     "txGain (dB)",     "<srvPrv#> <FXS#> <txGain>"
   },

   { "linetest",        dalVoice_SetFxsLineTest,         dalVoice_mapPotsFxsNumToInst,
     3, FALSE,
     "MLT test",         "<srvPrv#> <fxs#> <hazardv|foreignv|resistance|off-hook|REN>"
   },


   /* call control extension parameters */
   { NULL,            NULL,                                NULL, 0, FALSE, "ERROR",                              "ERROR"                                         }
};


/* ---- Private Function Prototypes -------------------------------------- */

static void voiceShowCmdSyntax();
static void processVoiceCtlSetCmd(char *cmdLine);
static void processVoiceCtlShowCmd(char *cmdLine);
static void processVoiceSaveCmd(char *cmdLine);
static CmsRet dumpSipNetworkParams(int spNum);
static CmsRet dumpSipClientParams(int spNum);
static CmsRet dumpVoipProfileParams(int spNum);
static CmsRet dumpVoiceCodecList(int spNum);
static CmsRet dumpCallCtlLineExtParams(int spNum);
static CmsRet dumpCallCtlMappingParams(int spNum);
static CmsRet dumpCallCtlFeatureSetParams(int spNum);
static CmsRet dumpVoiceParams( void );
static CmsRet dumpServiceProviderParams(int spNum);
static CmsRet dumpVoiceStats(int spNum);
static CmsRet dumpVoiceCallLogs(int spNum);
static CmsRet dumpVoiceAllParams( void );
static CmsRet dumpVoicePotsFxsDiag(int spNum);
static CmsRet dumpVoicePotsFxs(int spNum);

/* common Voice Diagnostic processing */
static void processVoiceDiagCmd(VoiceDiagType diagType, char *cmdLine);
static int buildCmsMsg(CmsMsgHeader *msg, VoiceDiagMsgBody *info, char *cmdLine);

/* Endpoint Application specific processing */
static void processEptAppCmd(char *cmdLine);

#ifdef BRCM_PROFILER_ENABLED
static void processVoiceCtlProfilerCmd(char *cmdLine);
#endif

static int initCfgPtrArray( char ** argArray, char * argBuffer , int len);

#if DMP_EPON_VOICE_OAM || DMP_X_ITU_ORG_GPON_1
static void processVoiceSendUpldComplete(void);
#endif /* DMP_EPON_VOICE_OAM */

/* ---- Function implementations ----------------------------------------- */

static void voiceShowCmdSyntax()
{
   int i = 0;
   printf("Command syntax: \n");
   printf("voice --help                      -  show the voice command syntax\n");
   printf("voice show                        -  show the voice parameters\n");
#ifdef SIPLOAD
   printf("voice show all                    -  show all voice parameters\n");
   printf("voice show stats                  -  show call statistics\n");
   printf("voice show memstats               -  shows memory allocation statistics\n");
   printf("voice show cctkcmstats            -  shows Call Manager & CCTK statistics\n");
#endif /* SIPLOAD */

   printf("voice show network                -  show sip networks\n");
   printf("voice show client                 -  show sip clients\n");
   printf("voice show voipprofile            -  show voip profiles\n");
   printf("voice show codecs                 -  show codec profiles\n");
   printf("voice show callctl                -  show call control line/extension\n");
   printf("voice show map                    -  show call control mapping\n");
   printf("voice show lines                  -  show FXS lines\n");
   printf("voice show feature                -  show call feature\n");
   printf("voice show log                    -  show call logs\n");

   printf("voice start                       -  start the voice application\n");
#if DMP_EPON_VOICE_OAM || DMP_X_ITU_ORG_GPON_1
   printf("voice sendUpldComplete            -  send the upload complete message to ssk\n");
#endif /* DMP_EPON_VOICE_OAM */
   printf("voice stop                        -  stop the voice application\n");
   printf("voice save                        -  store voice params to flash\n");
   printf("voice reboot                      -  restart the voice application\n");
   printf("voice set <param> <arg1> <arg2>.. -  set a provisionable parameter\n");
   printf("List of voice set params and args:                                          \n");
   /* TODO: Go through DAL mapping tables and print all voice set commands */
   while( voiceDalCliMap[i].name != NULL )
   {
      /* left-aligned */
      printf("%-15s %-27s - %-25s\n",voiceDalCliMap[i].name, voiceDalCliMap[i].syntax, voiceDalCliMap[i].info);
      i++;
   }
}

void processVoiceCtlCmd(char *cmdLine)
{
   cmsLog_debug("%s() called with cmdLine(%s)\n", __FUNCTION__, cmdLine);

   if ( strstr(cmdLine, "eptcmd") != NULL )
      processVoiceDiagCmd(VOICE_DIAG_EPTCMD, cmdLine);
   else if ( strstr(cmdLine, "eptprov") != NULL )
      processVoiceDiagCmd(VOICE_DIAG_EPTPROV, cmdLine);
   else if ( strstr(cmdLine, "eptprobe") != NULL )
      processVoiceDiagCmd(VOICE_DIAG_EPTPROBE, cmdLine);
#ifdef EPTAPPLOAD
   else if ( strstr(cmdLine, "eptapp") != NULL )
      processEptAppCmd(cmdLine);
#endif /* EPTAPPLOAD */
#ifdef BRCM_PROFILER_ENABLED
   else if ( strstr(cmdLine, "profiler") != NULL)
      processVoiceCtlProfilerCmd(cmdLine);
#endif
   else if ( strstr(cmdLine, "start") != NULL )
      dalVoice_voiceStart(cliPrvtMsgHandle);
#if DMP_EPON_VOICE_OAM || DMP_X_ITU_ORG_GPON_1
   else if ( strstr(cmdLine, "sendUpldComplete") != NULL )
      processVoiceSendUpldComplete();
#endif /* DMP_EPON_VOICE_OAM       */
   else if ( strstr(cmdLine, "stop") != NULL )
      dalVoice_voiceStop(cliPrvtMsgHandle);
   else if ( strstr(cmdLine, "set") != NULL )
      processVoiceCtlSetCmd(cmdLine);
   else if ( strstr(cmdLine, "show") != NULL )
      processVoiceCtlShowCmd(cmdLine);
   else if ( strstr(cmdLine, "save") != NULL )
      processVoiceSaveCmd(cmdLine);
   else if ( strstr(cmdLine, "reboot") != NULL )
      dalVoice_voiceReboot(cliPrvtMsgHandle);
#ifdef later
   /* todo: voice team will have to port this */
#ifdef STUN_CLIENT
   else if ( strstr(cmdLine, "stunlkup") != NULL )
      processVoiceCtlStunLkupCmd(cmdLine);
#endif /* STUN_CLIENT */
#endif /* later */
   else
      voiceShowCmdSyntax();


   cmsLog_debug("processVoiceCtlCmd called %s end\n", cmdLine);

   return;
}

#if DMP_EPON_VOICE_OAM || DMP_X_ITU_ORG_GPON_1
/***************************************************************************
* Function Name: processVoiceSendUpldComplete
* Description  : process the voice sendUpldComplete command.
* Parameters   : none.
* Returns      : none.
****************************************************************************/
static void processVoiceSendUpldComplete(void)
{
   CmsMsgHeader *msgHdr;
   CmsRet ret = CMSRET_INTERNAL_ERROR;

   /* allocate a message body big enough to hold the header */
   msgHdr = (CmsMsgHeader *) cmsMem_alloc(sizeof(CmsMsgHeader) , ALLOC_ZEROIZE);
   if (msgHdr == NULL)
   {
      cmsLog_error("message header allocation failed");
      return;
   }

   msgHdr->src = cmsMsg_getHandleEid(cliPrvtMsgHandle);;
   msgHdr->dst = EID_SSK;
   msgHdr->type = CMS_MSG_CONFIG_UPLOAD_COMPLETE;
   msgHdr->wordData = 0;
   msgHdr->dataLength = 0;

   ret = cmsMsg_send(cliPrvtMsgHandle, msgHdr);

   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not send CMS_MSG_CONFIG_UPLOAD_COMPLETE msg from CLI to SSK, ret=%d", ret);
   }

   cmsMem_free(msgHdr);
}
#endif /* DMP_EPON_VOICE_OAM */

/***************************************************************************
* Function Name: processVoiceCtlSetCmd
* Description  : process the voice set command.
* Parameters   : cmdLine - command line.
* Returns      : none.
****************************************************************************/
static void processVoiceCtlSetCmd(char *cmdLine)
{

   DAL_VOICE_PARMS argBlk;
   char * arguments[DAL_VOICE_MAX_VOIP_ARGS];
   int i=0, j;
   int vpInst, spNum,accNum,indexOfLastCliArg;
   char *value = NULL;
#ifdef DMP_X_BROADCOM_COM_PSTNENDPOINT_1
   int pstnNum, pstnInst;
#endif
   int  numSvc;

   /* Parse arguments into a ptr array */
   memset( &argBlk, 0, sizeof(argBlk) );
   memset( arguments, 0, sizeof(arguments) );
   int numArgs = initCfgPtrArray( arguments, cmdLine , strlen(cmdLine)+1 );

    /* Print all arguments */
    cmsLog_debug("\n");
    cmsLog_debug("Total arguments: %d",numArgs);
    cmsLog_debug("---------------------");
    for ( i = 0; i< numArgs; i++ )
    {
       cmsLog_debug("Arg%d:%s", i, arguments[i]);
    }
    cmsLog_debug("---------------------");
    cmsLog_debug("\n");

   /* Calculate index of the last cli argument */
   indexOfLastCliArg = numArgs - 1;

   /* If value is an empty string then set it to NULL */
   if ( !strncmp(arguments[indexOfLastCliArg], "\"\"", strlen(arguments[indexOfLastCliArg]) + 1) )
   {
      strncpy(arguments[indexOfLastCliArg], "", strlen("") + 1);
   }

   /* Check if parameter name was entered on the cli */
   if ( indexOfLastCliArg < VOIP_PARMNAME_ARG_INDEX )
   {
      printf("%s:: Incomplete voice set command\n",__FUNCTION__);
      /* Show command list */
      voiceShowCmdSyntax();
      return;
   }

   /* Look for command name match in table */
   i = 0;
   while ( voiceDalCliMap[i].name != NULL && arguments[VOIP_PARMNAME_ARG_INDEX] != NULL )
   {
      if ( strncasecmp(arguments[VOIP_PARMNAME_ARG_INDEX], voiceDalCliMap[i].name, MAX_CLICMD_NAME_SIZE) == 0 )
      {
         break;
      }
      i++;
   }

   /* Check if we searched through entire table without finding a match */
   if ( voiceDalCliMap[i].name == NULL )
   {
      cmsLog_error("%ss:: Invalid Parameter Name\n",__FUNCTION__);
      /* Show command list */
      voiceShowCmdSyntax();
      return;
   }

   /* Check if complete arguments are provided for the specific parameter */
   if ( VOIP_FIRST_PARMARG_INDEX + voiceDalCliMap[i].numArgs == numArgs )
   {
#ifdef DMP_X_BROADCOM_COM_PSTNENDPOINT_1
      /* pstn parameters, special processing */
      if ( strncasecmp(arguments[VOIP_FIRST_PARMARG_INDEX-1],"pstnDialPlan",  MAX_CLICMD_NAME_SIZE )== 0 ||
           strncasecmp(arguments[VOIP_FIRST_PARMARG_INDEX-1],"pstnRouteRule", MAX_CLICMD_NAME_SIZE)== 0  ||
           strncasecmp(arguments[VOIP_FIRST_PARMARG_INDEX-1],"pstnRouteData", MAX_CLICMD_NAME_SIZE)== 0   )
      {
         pstnNum = atoi(arguments[VOIP_FIRST_PARMARG_INDEX]);
         dalVoice_mapCmPstnLineToPstnInst( pstnNum , &pstnInst);
         argBlk.op[0] = pstnInst;
         value = arguments[VOIP_FIRST_PARMARG_INDEX+1];

      }
      else
#endif /* DMP_X_BROADCOM_COM_PSTNENDPOINT_1 */
      if ( strncasecmp(arguments[VOIP_FIRST_PARMARG_INDEX-1], "loglevel", MAX_CLICMD_NAME_SIZE) == 0 )
      {
         /* Set the loglevel right here */
         cmsLck_acquireLock();
         dalVoice_GetNumSrvProv( &numSvc );
         for( j = 0; j < numSvc ; j++)
         {
            dalVoice_mapSpNumToSvcInst( j, &vpInst );
            argBlk.op[0] = vpInst;
            dalVoice_SetModuleLoggingLevel( &argBlk, arguments[VOIP_FIRST_PARMARG_INDEX], arguments[VOIP_FIRST_PARMARG_INDEX+1] );
         }
         cmsLck_releaseLock();

         return;
      }
      else
      {
         /* Fillout arguments structure based on number of arguments */
         switch (voiceDalCliMap[i].numArgs)
         {
            case 0:
            {
               /* Assign dummy values so that check for null passes at the end */
               argBlk.op[0] = 0;
               value = "";
            }
            break;

            case 1:
            {
               /* Global parameter, set vpInst = 0, this will indicate parameter *
                * needs to be changed in all voice profiles if required          */
               argBlk.op[0] = 0;
               value = arguments[VOIP_FIRST_PARMARG_INDEX];
            }
            break;

            case 2:
            {
               spNum = atoi(arguments[VOIP_FIRST_PARMARG_INDEX]);

               /* Map to vpInst */
               cmsLck_acquireLock();
               dalVoice_mapSpNumToSvcInst( spNum, &vpInst );
               cmsLck_releaseLock();

               argBlk.op[0] = vpInst;
               value = arguments[VOIP_FIRST_PARMARG_INDEX + 1];
            }
            break;

            case 3:
            {
               /* Account parameter */
               spNum = atoi(arguments[VOIP_FIRST_PARMARG_INDEX]);
               accNum = atoi(arguments[VOIP_FIRST_PARMARG_INDEX + 1]);

               /* Map to voice service instance */
               cmsLck_acquireLock();
               dalVoice_mapSpNumToSvcInst( spNum, &vpInst );
               cmsLck_releaseLock();

               argBlk.op[0] = vpInst;
               argBlk.op[1] = accNum; /* pass in accNum, so associated function will convert it to instance */
               if( voiceDalCliMap[i].convertFunc != NULL )
               {
                  int  Inst = 0;
                  CmsRet  ret ;
                  cmsLck_acquireLock();
                  ret = voiceDalCliMap[i].convertFunc( &argBlk, &Inst );
                  cmsLck_releaseLock();
                  if( ret == CMSRET_SUCCESS && Inst > 0 ){
                     argBlk.op[1] = Inst;
                  }
               }
               value = arguments[VOIP_FIRST_PARMARG_INDEX + 2];
            }
            break;

            default:
            break;
         }
      }
   }
   else
   {
      printf("%s:: Invalid Number of Arguments\n",__FUNCTION__);
      printf("%s:: Syntax: voice set %s %s \n",__FUNCTION__,voiceDalCliMap[i].name,voiceDalCliMap[i].syntax);
      return;
   }

   /* Execute associated DAL function */
   if ( value != NULL && voiceDalCliMap[i].dalFunc != NULL )
   {
      if ( voiceDalCliMap[i].global && voiceDalCliMap[i].numArgs == 1) /* global setting for all voice services */
      {
         cmsLck_acquireLock();
         dalVoice_GetNumSrvProv( &numSvc );
         /* Set the loglevel right here */
         for( j = 0; j < numSvc ; j++)
         {
            dalVoice_mapSpNumToSvcInst( j, &vpInst );
            argBlk.op[0] = vpInst;
            voiceDalCliMap[i].dalFunc( &argBlk, value );
         }
         cmsLck_releaseLock();
         return;
      }
      else
      {
         /* Aquire Lock */
         cmsLck_acquireLock();

         voiceDalCliMap[i].dalFunc( &argBlk, value );

         /* Release Lock */
         cmsLck_releaseLock();
      }
   }

   return;
}


#ifdef BRCM_PROFILER_ENABLED
/***************************************************************************
* Function Name: processVoiceCtlProfilerCmd
* Description  : process the voice profiler command
* Parameters   : cmdLine - command line.
* Returns      : none.
****************************************************************************/
static void processVoiceCtlProfilerCmd(char *cmdLine)
{
   /* Need to send message to voice app with profiler command */
   char buf[sizeof(CmsMsgHeader) + sizeof(VoiceDiagMsgBody)]={0};
   CmsMsgHeader *msg = (CmsMsgHeader *) buf;
   VoiceDiagMsgBody *info = (VoiceDiagMsgBody *) &(buf[sizeof(CmsMsgHeader)]);
   CmsRet ret;

   char buffer[CLI_MAX_BUF_SZ];
   char *pToken = NULL, *pLast = NULL;

   /* if no cmdLine then return false */
   if ( cmdLine == NULL )
   {
      cmsLog_error("No arguments for Endpt cmd");
      return;
   }

   strncpy(buffer, cmdLine, CLI_MAX_BUF_SZ);

   pToken = strtok_r( buffer, " ", &pLast );
   /* skip the voice and profiler portions of the command */
   if ( pToken == NULL )
   {
      cmsLog_error("Wrong number of arguments");
      return;
   }

   if ( pLast == NULL )
   {
      cmsLog_error("Invalid number of arguments for Endpt cmd");
      return;
   }

   /* Compose the diag message */
   msg->type = CMS_MSG_VOICE_DIAG;
   msg->src = cmsMsg_getHandleEid(cliPrvtMsgHandle);
   msg->dst = EID_VOICE;
   msg->flags_request = 1;
   msg->flags_bounceIfNotRunning = 1;
   msg->dataLength = sizeof(VoiceDiagMsgBody);

   /* now fill in the data section */
   strncpy(info->cmdLine,pLast,sizeof(info->cmdLine));
   info->type = VOICE_DIAG_PROFILE;

   if ((ret = cmsMsg_sendAndGetReplyWithTimeout(cliPrvtMsgHandle, msg,10000)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not send CMS_MSG_VOICE_DIAG msg to voice, ret=%d", ret);
   }
}
#endif  /* BRCM_PROFILER_ENABLED */

/***************************************************************************
* Function Name: processVoiceCtlShowCmd
* Description  : process the voice show command.
* Parameters   : cmdLine - command line.
* Returns      : none.
****************************************************************************/
static void processVoiceCtlShowCmd(char *cmdLine)
{
#ifdef SIPLOAD
   /* In CCTK stats need to be retrieved first */
   if( strstr( cmdLine, "memstats") != NULL ||
       strstr( cmdLine, "cctkcmstats" ) != NULL )
   {
      /* Need to send message to voice app */
      char buf[sizeof(CmsMsgHeader) + sizeof(VoiceDiagMsgBody)]={0};
      CmsMsgHeader *msg = (CmsMsgHeader *) buf;
      VoiceDiagMsgBody *info = (VoiceDiagMsgBody *) &(buf[sizeof(CmsMsgHeader)]);
      msg->type = CMS_MSG_VOICE_DIAG;
      msg->src = cmsMsg_getHandleEid(cliPrvtMsgHandle);
      msg->dst = EID_VOICE;
      msg->flags_request = 1;
      msg->flags_bounceIfNotRunning = 1;
      msg->dataLength = sizeof( VoiceDiagMsgBody );
      info->type = VOICE_DIAG_STATS_SHOW;
      strncpy( (char*)info->cmdLine, cmdLine, BUFLEN_128 );

      if( cmsMsg_sendAndGetReplyWithTimeout(cliPrvtMsgHandle, msg, 2000) != CMSRET_SUCCESS )
      {
         cmsLog_error("Could not send CMS_MSG_VOICE_DIAG msg to voice" );
      }

      return;
   }
   else if( strstr( cmdLine, "network") != NULL )
   {
      int i,  spNum = 0;

      /* Aquire Lock */
      cmsLck_acquireLock();
      dalVoice_GetNumSrvProv( &spNum );
      for( i = 0; i<spNum; i++ )
      {
         dumpSipNetworkParams( i );
      }
      cmsLck_releaseLock();
      return;
   }
   else if( strstr( cmdLine, "client") != NULL )
   {
      int i,  spNum = 0;

      /* Aquire Lock */
      cmsLck_acquireLock();
      dalVoice_GetNumSrvProv( &spNum );
      for( i = 0; i<spNum; i++ )
      {
         dumpSipClientParams( i );
      }
      cmsLck_releaseLock();
      return;
   }
   else if( strstr( cmdLine, "voipprofile") != NULL )
   {
      int i,  spNum = 0;

      /* Aquire Lock */
      cmsLck_acquireLock();
      dalVoice_GetNumSrvProv( &spNum );
      for( i = 0; i<spNum; i++ )
      {
         dumpVoipProfileParams( i );
      }
      cmsLck_releaseLock();
      return;
   }
   else if( strstr( cmdLine, "codec") != NULL )
   {
      int i,  spNum = 0;

      /* Aquire Lock */
      cmsLck_acquireLock();
      dalVoice_GetNumSrvProv( &spNum );
      for( i = 0; i<spNum; i++ )
      {
         dumpVoiceCodecList( i );
      }
      cmsLck_releaseLock();
      return;
   }
   else if( strstr( cmdLine, "callctl") != NULL )
   {
      int i,  spNum = 0;

      /* Aquire Lock */
      cmsLck_acquireLock();
      dalVoice_GetNumSrvProv( &spNum );
      for( i = 0; i<spNum; i++ )
      {
         dumpCallCtlLineExtParams( i );
      }
      cmsLck_releaseLock();
      return;
   }
   else if( strstr( cmdLine, "feature") != NULL )
   {
      int i,  spNum = 0;

      /* Aquire Lock */
      cmsLck_acquireLock();
      dalVoice_GetNumSrvProv( &spNum );
      for( i = 0; i<spNum; i++ )
      {
         dumpCallCtlFeatureSetParams( i );
      }
      cmsLck_releaseLock();
      return;
   }
   else if( strstr( cmdLine, "map") != NULL )
   {
      int i,  spNum = 0;

      /* Aquire Lock */
      cmsLck_acquireLock();
      dalVoice_GetNumSrvProv( &spNum );
      for( i = 0; i<spNum; i++ )
      {
         dumpCallCtlMappingParams( i );
      }
      cmsLck_releaseLock();
      return;
   }
   /* Show call statistics */
   else if( strstr(cmdLine, "stats") != NULL )
   {
      int i,  spNum = 0;
      /* Aquire Lock */
      cmsLck_acquireLock();

      dalVoice_GetNumSrvProv( &spNum );
      for( i = 0; i<spNum; i++ )
      {
         dumpVoiceStats( i );
      }

      /* Release Lock */
      cmsLck_releaseLock();
      return;
   }
   /* Show all of voice parameters */
   else if( strstr(cmdLine, "log") != NULL )
   {
      int i,  spNum = 0;
      /* Aquire Lock */
      cmsLck_acquireLock();

      dalVoice_GetNumSrvProv( &spNum );
      for( i = 0; i<spNum; i++ )
      {
         dumpVoiceCallLogs( i );
      }

      /* Release Lock */
      cmsLck_releaseLock();
      return;
   }
   /* Show all of voice parameters */
   else if( strstr(cmdLine, "linetest") != NULL )
   {
      int i,  spNum = 0;
      /* Aquire Lock */
      cmsLck_acquireLock();

      dalVoice_GetNumSrvProv( &spNum );
      for( i = 0; i<spNum; i++ )
      {
         dumpVoicePotsFxsDiag( i );
      }

      /* Release Lock */
      cmsLck_releaseLock();
      return;
   }
   /* Show all of voice parameters */
   else if( strstr(cmdLine, "lines") != NULL )
   {
      int i,  spNum = 0;
      /* Aquire Lock */
      cmsLck_acquireLock();

      dalVoice_GetNumSrvProv( &spNum );
      for( i = 0; i<spNum; i++ )
      {
         dumpVoicePotsFxs( i );
      }

      /* Release Lock */
      cmsLck_releaseLock();
      return;
   }

   /* Show all of voice parameters */
   else if( strstr(cmdLine, "all") != NULL )
   {
      int i,  spNum = 0;
      /* Aquire Lock */
      cmsLck_acquireLock();

      dalVoice_GetNumSrvProv( &spNum );
      for( i = 0; i<spNum; i++ )
      {
         dumpVoiceAllParams();
      }

      /* Release Lock */
      cmsLck_releaseLock();
      return;
   }
#endif /* SIPLOAD */
   /* show voice parameters */

   /* Aquire Lock */
   cmsLck_acquireLock();

   dumpVoiceParams();

   /* Release Lock */
   cmsLck_releaseLock();
}

/***************************************************************************
* Function Name: processVoiceSaveCmd
* Description  : process the voice save command.
* Parameters   : cmdLine - command line.
* Returns      : none.
****************************************************************************/
static void processVoiceSaveCmd(char *cmdLine)
{
   /* Aquire Lock */
   cmsLck_acquireLock();

   dalVoice_Save();

   /* Release Lock */
   cmsLck_releaseLock();
}

#ifdef later
#ifdef STUN_CLIENT
/***************************************************************************
* Function Name: processVoiceCtlStunLkupCmd
* Description  : process the voice stunlkup command.
* Parameters   : cmdLine - command line.
* Returns      : none.
****************************************************************************/
void CliShellCmd::processVoiceCtlStunLkupCmd( char *cmdLine )
{
#ifdef CLI_CMD
   BcmVoice_StunLkup( cmdLine );
#endif
}
#endif /* STUN_CLIENT */
#endif /* later */

/***************************************************************************
* Function Name: processEptAppCmd
* Description  : process the endpoint demo app cmds.
* Parameters   : cmdLine - command line.
* Returns      : none.
****************************************************************************/

static void processEptAppCmd(char *cmdLine)
{
   char *cp = NULL;
   char *cmd = NULL;

   cp = strdup( cmdLine );

   /* Strip off "eptapp" from cp (cmdLine) */
   strtok(cp, " ");

   /* get the command from cp (cmdLine) */
   cmd = strtok( NULL, " " );

   if ( cmd == NULL )
   {
      voiceShowCmdSyntax();
      return;
   }

   /* search for command and calls appropriate process command */
   if ( strcasecmp(cmd, "show") == 0 )
      processVoiceDiagCmd(VOICE_DIAG_EPTAPP_SHOW, cmdLine);
   else if ( strcasecmp(cmd, "createcnx") == 0 )
      processVoiceDiagCmd(VOICE_DIAG_EPTAPP_CREATECNX, cmdLine);
   else if ( strcasecmp(cmd, "deletecnx") == 0 )
      processVoiceDiagCmd(VOICE_DIAG_EPTAPP_DELETECNX, cmdLine);
   else if ( strcasecmp(cmd, "modifycnx") == 0 )
      processVoiceDiagCmd(VOICE_DIAG_EPTAPP_MODIFYCNX, cmdLine);
   else if ( strcasecmp(cmd, "eptsig") == 0 )
      processVoiceDiagCmd(VOICE_DIAG_EPTAPP_EPTSIG, cmdLine);
   else if ( strcasecmp(cmd, "set") == 0 )
      processVoiceDiagCmd(VOICE_DIAG_EPTAPP_SET, cmdLine);
   else if ( strcasecmp(cmd, "decttest") == 0 )
      processVoiceDiagCmd(VOICE_DIAG_EPTAPP_DECTTEST, cmdLine);
   else if ( strcasecmp(cmd, "vas") == 0 )
      processVoiceDiagCmd(VOICE_DIAG_EPTAPP_VAS, cmdLine);
   else if ( strcasecmp(cmd, "vrs") == 0 )
      processVoiceDiagCmd(VOICE_DIAG_EPTAPP_VRS, cmdLine);
   else
   {
      /* Is either "help" or malformed */
      processVoiceDiagCmd(VOICE_DIAG_EPTAPP_HELP, cmdLine);
   }

   return;
}

/***************************************************************************
* Function Name: buildCmsMsg
* Description  : common code for populating cms structures
* Parameters   : msg - message to forward.
*              : info - body portion of message
*              : cmdLine - command line.
* Returns      : 0 if success, otherwise non-zero
****************************************************************************/
static int buildCmsMsg(CmsMsgHeader *msg, VoiceDiagMsgBody *info, char *cmdLine)
{
   char buffer[CLI_MAX_BUF_SZ];
   char *pToken = NULL, *pLast = NULL;

   /* if no cmdLine then return false */
   if ( cmdLine == NULL )
   {
      cmsLog_error("No arguments for Endpt cmd");
      return( -1 );
   }

   strncpy(buffer, cmdLine, CLI_MAX_BUF_SZ);

   /* skip the voice and eptcmd portions of the command */
   pToken = strtok_r( buffer, " ", &pLast );
   /* skip the voice and profiler portions of the command */
   if ( pToken == NULL )
   {
      cmsLog_error("Wrong number of arguments");
      return ( -1 );
   }

   if ( pLast == NULL )
   {
      cmsLog_error("Invalid number of arguments for Endpt cmd");
      return( -1 );
   }

   /* Compose the diag message */
   msg->type = CMS_MSG_VOICE_DIAG;
   msg->src = cmsMsg_getHandleEid(cliPrvtMsgHandle);
   msg->dst = EID_VOICE;
   msg->flags_request = 1;
   msg->flags_bounceIfNotRunning = 1;
   msg->dataLength = sizeof(VoiceDiagMsgBody);

   /* now fill in the data section */
   strncpy(info->cmdLine,pLast,sizeof(info->cmdLine));

   return( 0 );
}

/***************************************************************************
* Function Name: processVoiceDiagCmd
* Description  : common code for messaging Endpoint Application
* Parameters   : diagType - message type used to route the message
* Returns      : none.
****************************************************************************/
static void processVoiceDiagCmd(VoiceDiagType diagType, char *cmdLine)
{
   /* Need to send message to voice app with eptcmd */
   char buf[sizeof(CmsMsgHeader) + sizeof(VoiceDiagMsgBody)]={0};
   CmsMsgHeader *msg = (CmsMsgHeader *) buf;
   VoiceDiagMsgBody *info = (VoiceDiagMsgBody *) &(buf[sizeof(CmsMsgHeader)]);

   int status = buildCmsMsg(msg, info, cmdLine);
   if (status != 0)
   {
      return;
   }

   info->type = diagType;

   CmsRet ret = cmsMsg_sendAndGetReplyWithTimeout(cliPrvtMsgHandle, msg, 10000);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("could not send CMS_MSG_VOICE_DIAG msg to voice, ret=%d", ret);
   }
}

/***********************************************************
 * This function takes a buffer full of space delimited    *
 * voip configuration parameters and populates an array of *
 * strings as follows:                                     *
 *                argBuffer =  -p 0.0.0.0:5060 -l none     *
 *                             ^  ^            ^  ^        *
 *                             |  |            |  |        *
 * argArray[0][0] <------------+  |            |  |        *
 * argArray[1][0] <---------------+            |  |        *
 * argArray[2][0] <----------------------------+  |        *
 * argArray[3][0] <-------------------------------+        *
 *                                                         *
 * Args: argArray    - Array of strings to hold all args   *
 *       argBuffer   - cmdLine string                      *
 *       len         - length of cmdLine string including  *
 *                     null terminator                     *
 *                                                         *
 * Returns: Number of arguments parsed                     *
 ***********************************************************/
static int initCfgPtrArray( char ** argArray, char * argBuffer , int len)
{
   int i = 0;
   int j = 0;

   argArray[j] = &argBuffer[i];

   for ( i=0; i<len; i++ )
   {
      /* Find first matching space or null terminator*/
      if ( argBuffer[i]==' ' || argBuffer[i]=='\0')
      {
         j++;

         /* Replace space with null terminator */
         argBuffer[i] = '\0';

         /* Check if there are two consecutive spaces or eofstring */
         for ( ; i<(len-1); i++ )
         {
            if( argBuffer[i+1] != ' ' && argBuffer[i+1] != '\0') { break; }
         }

         /* if we have reached end of string, return */
         if ( i == (len - 1) )
         {
            break;
         }

         /* assign to pointer array */
         argArray[j] = &argBuffer[i+1];
      }
   }
   /* return total number of parameters detected */
   return j;
}

/***************************************************************************
* Function Name: dumpServiceProviderParams
* Description  : Dump service provider specific parmaters.
* These parameters have a per line scope in callmanager and are stored
* at the voice profile level in TR104
****************************************************************************/
static CmsRet dumpServiceProviderParams(int spNum)
{
   DAL_VOICE_PARMS parms;
   CmsRet ret = CMSRET_SUCCESS;
   char* objValue = NULL;
   int   vpInst = 0;
   unsigned int   numLines = 0, numExt = 0;
   unsigned int   numFxsLines, numFxoLines, numSipClients, numSipNetworks;

   /* Allocate memory for the object to be obtained through DAL APIs. */
   objValue = cmsMem_alloc( MAX_TR104_OBJ_SIZE, ALLOC_ZEROIZE );
   if ( objValue == NULL )
   {
      cmsLog_error( "Could not get allocate memory for object.\n" );
      return ( CMSRET_RESOURCE_EXCEEDED );
   }

   /* Fill in the parameter structure needed for dalVoice_GetXYZ.
   ** Since these are global parameters, the voice profile and
   ** line instance are irrelevant so we hard-code them. */
   dalVoice_mapSpNumToSvcInst( spNum, &vpInst );
   parms.op[0] = vpInst;

   printf( "\n" );
   printf( "Service provider: %d\n", spNum );
   printf( "------------------\n" );

   if ( dalVoice_GetBoundIfName( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "BoundIfName               : %s\n", objValue );
   }

   memset(objValue, 0, MAX_TR104_OBJ_SIZE);
   if ( dalVoice_GetIpFamily( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "IP address family         : %s\n", objValue );
   }

   memset(objValue, 0, MAX_TR104_OBJ_SIZE);
   if ( dalVoice_GetLoggingLevel( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "Voice logLevel            : %s\n", objValue );
   }

   memset(objValue, 0, MAX_TR104_OBJ_SIZE);
   if ( dalVoice_GetModuleLoggingLevels( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "Voice Module logLevel     : %s\n", objValue );
   }

   memset(objValue, 0, MAX_TR104_OBJ_SIZE);
   if ( dalVoice_GetCCTKTraceLevel( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "CCTK logLevel             : %s\n", objValue );
   }

   memset(objValue, 0, MAX_TR104_OBJ_SIZE);
   if ( dalVoice_GetCCTKTraceGroup( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "CCTK logGroup             : %s\n", objValue );
   }


   memset(objValue, 0, MAX_TR104_OBJ_SIZE);
   if ( dalVoice_GetManagementProtocol( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "Management Protocol       : %s\n", objValue );
   }

   memset(objValue, 0, MAX_TR104_OBJ_SIZE);
   if ( dalVoice_GetRegion( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "Region                    : %s\n", objValue );
   }

#ifdef BRCM_SIP_VOICE_DNS
   memset(objValue, 0, MAX_TR104_OBJ_SIZE);
   if ( dalVoice_GetDnsServerAddr( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "Voice DNS Server          : %s\n", objValue );
   }
#endif // BRCM_SIP_VOICE_DNS

   if ( dalVoice_GetNumPhysFxsEndpt( &numFxsLines ) == CMSRET_SUCCESS )
   {
      printf( "Number of FXS lines       : %d\n", numFxsLines );
   }

   if ( dalVoice_GetNumPhysFxoEndpt( &numFxoLines ) == CMSRET_SUCCESS )
   {
      printf( "Number of FXO lines       : %d\n", numFxoLines );
   }

   if ( dalVoice_GetNumSipClient( &parms, &numSipClients ) == CMSRET_SUCCESS )
   {
      printf( "Number of SIP clients     : %d\n", numSipClients );
   }

   if ( dalVoice_GetNumSipNetwork( &parms, &numSipNetworks ) == CMSRET_SUCCESS )
   {
      printf( "Number of SIP networks    : %d\n", numSipNetworks );
   }

   if ( dalVoice_GetNumOfExtension( &parms, &numExt ) == CMSRET_SUCCESS )
   {
      printf( "Number of CC extensions   : %d\n", numExt );
   }

   if ( dalVoice_GetNumOfLine( &parms, &numLines ) == CMSRET_SUCCESS )
   {
      printf( "Number of CC lines        : %d\n", numLines );
   }

   cmsMem_free( objValue );

   return ret;
}


/***************************************************************************
* Function Name: dumpVoiceAllParams
* Description  : Dump all voice params
* NOTE: This function is used for the 'voice show all' CLI command
****************************************************************************/
static CmsRet dumpVoiceAllParams( void )
{
    CmsRet ret;
    int    spNum = 0;

    /* Dump global parameters */
    ret = dumpServiceProviderParams( spNum );
    if ( ret != CMSRET_SUCCESS )
    {
        cmsLog_error( "ret = %d \n", ret);
        return(ret);
    }

    ret = dumpSipNetworkParams( spNum );
    if ( ret != CMSRET_SUCCESS )
    {
        cmsLog_error( "ret = %d \n", ret);
        return(ret);
    }

    ret = dumpSipClientParams( spNum );
    if ( ret != CMSRET_SUCCESS )
    {
        cmsLog_error( "ret = %d \n", ret);
        return(ret);
    }
    ret = dumpVoipProfileParams(spNum);
    if ( ret != CMSRET_SUCCESS )
    {
        cmsLog_error( "ret = %d \n", ret);
        return(ret);
    }

    ret = dumpCallCtlLineExtParams( spNum );
    if ( ret != CMSRET_SUCCESS )
    {
        cmsLog_error( "ret = %d \n", ret);
        return(ret);
    }

    ret = dumpCallCtlFeatureSetParams( spNum);
    if ( ret != CMSRET_SUCCESS )
    {
        cmsLog_error( "ret = %d \n", ret);
        return(ret);
    }

    ret = dumpCallCtlMappingParams( spNum );
    if ( ret != CMSRET_SUCCESS )
    {
        cmsLog_error( "ret = %d \n", ret);
        return(ret);
    }

    ret = dumpVoicePotsFxs( spNum );
    if ( ret != CMSRET_SUCCESS )
    {
        cmsLog_error( "ret = %d \n", ret);
        return(ret);
    }

    return ret;

}


/***************************************************************************
* Function Name: dumpVoiceParams
* Description  : Dump global voice params
* NOTE: This function is used for the 'voice show' CLI command
****************************************************************************/
static CmsRet dumpVoiceParams( void )
{
    CmsRet ret;

    /* Dump global parameters */
    ret = dumpServiceProviderParams(0);
    if ( ret != CMSRET_SUCCESS )
    {
        cmsLog_error( "ret = %d \n", ret);
        return(ret);
    }
    return ret;

}

/***************************************************************************
* Function Name: dumpVoiceCodecList
* Description  : Dump service provider specific parmaters.
* These parameters have a per line scope in callmanager and are stored
* at the voice profile level in TR104
****************************************************************************/
static CmsRet dumpVoiceCodecList(int spNum)
{
   int cpInst=0,  vpInst = 0;
   int numOfCodecProfile = 0;
   int i;
   DAL_VOICE_PARMS parms;
   CmsRet ret = CMSRET_SUCCESS;
   char* objValue = NULL;

   /* Mapping spnum to vpInst */
   dalVoice_mapSpNumToSvcInst( spNum, &vpInst );

   /* Fill in the parameter structure needed */
   parms.op[0] = vpInst;
   dalVoice_GetNumCodecProfile( &parms, &numOfCodecProfile );
   if( numOfCodecProfile <= 0 )
   {
      printf( "\n" );
      printf( "No valid VoIP profile\n");
      return  ret;
   }

   /* Allocate memory for the object to be obtained through DAL APIs. */
   objValue = cmsMem_alloc( MAX_TR104_OBJ_SIZE, ALLOC_ZEROIZE );
   if ( objValue == NULL )
   {
      cmsLog_error( "Could not get allocate memory for object.\n" );
      return ( CMSRET_RESOURCE_EXCEEDED );
   }

   for( i = 0; i < numOfCodecProfile; i++ )
   {
      printf( "\n" );
      parms.op[1] = i;

      if( dalVoice_mapCpNumToInst ( &parms, &cpInst ) == CMSRET_SUCCESS )
      {
         parms.op[1] = cpInst;
         printf( "Codec Profile %d:\n", i);
         printf( "--------------------\n" );
         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetCodecProfileName( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   Profile Name: %s\n",  objValue );
         }

         if ( dalVoice_GetCodecProfileEnable( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   Profile Enabled: %s\n",  objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetCodecProfPacketPeriod( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   ptime: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetCodecProfSilSupp( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   Vad Enabled: %s\n", objValue );
         }
      }
   }

   memset( objValue, 0, MAX_TR104_OBJ_SIZE);
   ret = dalVoice_GetVoiceSvcCodecList(&parms, objValue, MAX_TR104_OBJ_SIZE);
   if( ret == CMSRET_SUCCESS && strlen(objValue) > 0)
   {
      printf( "Supported Codec List\n");
      printf( "--------------------\n" );
      printf( "   %s\n", objValue );
   }
   else
   {
      printf( "\n" );
      printf( "No valid codec\n");
   }
   /* Get voiceProfile object */
   cmsMem_free( objValue );

   return (ret);
}



/***************************************************************************
* Function Name: dumpVoipProfileParams
* Description  : Dump service provider specific parmaters.
* These parameters have a per line scope in callmanager and are stored
* at the voice profile level in TR104
****************************************************************************/
static CmsRet dumpVoipProfileParams(int spNum)
{
   int profileInst=0, numOfProfile, i, vpInst;
   DAL_VOICE_PARMS parms;
   CmsRet ret = CMSRET_SUCCESS;
   char* objValue = NULL;

   /* Mapping spnum to vpInst */
   dalVoice_mapSpNumToSvcInst( spNum, &vpInst );

   /* Fill in the parameter structure needed */
   parms.op[0] = vpInst;
   dalVoice_GetNumVoipProfile( &parms, &numOfProfile );
   if( numOfProfile <= 0 )
   {
      printf( "\n" );
      printf( "No valid VoIP profile\n");
      return  ret;
   }

   /* Allocate memory for the object to be obtained through DAL APIs. */
   objValue = cmsMem_alloc( MAX_TR104_OBJ_SIZE, ALLOC_ZEROIZE );
   if ( objValue == NULL )
   {
      cmsLog_error( "Could not get allocate memory for object.\n" );
      return ( CMSRET_RESOURCE_EXCEEDED );
   }

   for( i = 0; i < numOfProfile; i++ )
   {
      printf( "\n" );
      parms.op[1] = i;

      if( dalVoice_mapVoipProfNumToInst ( &parms, &profileInst ) == CMSRET_SUCCESS )
      {
         parms.op[1] = profileInst;
         printf( "VOIP Profile %d:\n", i);
         printf( "--------------------\n" );

         if ( dalVoice_GetVoipProfileEnable( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   Profile Enabled: %s\n",  objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetVoipProfileName( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   Profile Name: %s\n",  objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetDTMFMethod( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   DTMF method : %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetRtpLocalPortMin( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   RTP port min: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetRtpLocalPortMax( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   RTP port max: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetRtpDSCPMark( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   DSCP mark: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetRtcpEnabled( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   RTCP enabled: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetRtcpInterval( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   RTCP send interval: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetSrtpEnabled( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   SRTP enabled: %s\n", objValue );
         }
         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if (dalVoice_GetSrtpOptionString( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   SRTP option: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if (dalVoice_GetV18Enable( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   V.18 enabled: %s\n", objValue );
         }
      }
   }
   cmsMem_free( objValue );

   return (ret);
}


/***************************************************************************
* Function Name: dumpCallCtlLineExtParams
* Description  : Dump service provider specific parmaters.
* These parameters have a per line scope in callmanager and are stored
* at the voice profile level in TR104
****************************************************************************/
static CmsRet dumpCallCtlLineExtParams(int spNum)
{
   int numOfExt, numOfLines;
   int i, j;
   int lineInst = 0, extInst = 0, vpInst, mapInst;
   DAL_VOICE_PARMS parms;
   CmsRet ret = CMSRET_SUCCESS;
   char* objValue  = NULL;

   /* Mapping spnum to vpInst */
   dalVoice_mapSpNumToSvcInst( spNum, &vpInst );

   /* Fill in the parameter structure needed */
   parms.op[0] = vpInst;
   dalVoice_GetNumOfExtension( &parms, &numOfExt );
   if( numOfExt <= 0 )
   {
      printf( "\n" );
      printf( "No valid Call Control Extension\n");
      return  ret;
   }

   dalVoice_GetNumOfLine( &parms, &numOfLines );
   if( numOfLines <= 0 )
   {
      printf( "\n" );
      printf( "No valid Call Control Line\n");
      return  ret;
   }

   /* Allocate memory for the object to be obtained through DAL APIs. */
   objValue = cmsMem_alloc( MAX_TR104_OBJ_SIZE, ALLOC_ZEROIZE );
   if ( objValue == NULL )
   {
      cmsLog_error( "Could not get allocate memory for object.\n" );
      return ( CMSRET_RESOURCE_EXCEEDED );
   }

   for( i = 0; i < numOfExt; i++ )
   {
      printf( "\n" );
      parms.op[1] = i;

      if( dalVoice_mapExtNumToExtInst( vpInst, i, &extInst ) == CMSRET_SUCCESS)
      {
         parms.op[1] = extInst;
         printf( "Call Control Extension %d:\n", i);
         printf( "--------------------\n" );

         if ( dalVoice_GetCallCtrlExtEnable( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   Enabled: %s\n",  objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetCallCtrlExtName( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   Name: %s\n",  objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetCallCtrlExtStatus( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   Status: %s\n",  objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetCallCtrlExtNumber( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   Extension Number: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetCallCtrlExtCallStatus( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   Call Status: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetCallCtrlExtConfStatus( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   Conf Call Status: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetCallCtrlExtProvider( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   Provider Name: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetCallCtrlExtCallFeatureSet( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   Call Feature: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         /* Not applicable at the moment.
          * if ( dalVoice_GetCallCtrlExtNumberPlan( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
          * {
          *  printf( "   Numbering Plan: %s\n", objValue );
          * }
         */
         printf( "   Numbering Plan: Not applicable\n" );

      }
   }

   for( i = 0; i < numOfLines; i++ )
   {
      printf( "\n" );

      if( dalVoice_mapAcntNumToLineInst( 1, i, &lineInst ) == CMSRET_SUCCESS)
      {
         printf( "Call Control Line %d (inst %d):\n", i, lineInst);
         printf( "--------------------\n" );
         printf( "Associated extensions: ");
         for (j = 0; j < numOfExt; j++)
         {
            parms.op[0] = vpInst;
            parms.op[1] = i;
            parms.op[2] = j;
            if( dalVoice_mapLineExtToIncomingMapInst( &parms, &mapInst ) == CMSRET_SUCCESS && mapInst > 0)
            {
               printf( "EXT_%d ", j);
            }
         }
      }

      printf( "\n" );
   }

   cmsMem_free( objValue );

   return (ret);
}




/***************************************************************************
* Function Name: dumpSipClientParams
* Description  : Dump service provider specific parmaters.
* These parameters have a per line scope in callmanager and are stored
* at the voice profile level in TR104
****************************************************************************/
static CmsRet dumpSipClientParams(int spNum)
{
   int clientInst=0, numOfClient, i, vpInst;
   DAL_VOICE_PARMS parms;
   CmsRet ret = CMSRET_SUCCESS;
   char* objValue = NULL;

   /* Mapping spnum to vpInst */
   dalVoice_mapSpNumToSvcInst( spNum, &vpInst );

   /* Fill in the parameter structure needed */
   parms.op[0] = vpInst;
   dalVoice_GetNumSipClient( &parms, &numOfClient );
   if( numOfClient <= 0 )
   {
      printf( "\n" );
      printf( "No valid Sip Client\n");
      return  ret;
   }

   /* Allocate memory for the object to be obtained through DAL APIs. */
   objValue = cmsMem_alloc( MAX_TR104_OBJ_SIZE, ALLOC_ZEROIZE );
   if ( objValue == NULL )
   {
      cmsLog_error( "Could not get allocate memory for object.\n" );
      return ( CMSRET_RESOURCE_EXCEEDED );
   }

   for( i = 0; i < numOfClient; i++ )
   {
      printf( "\n" );
      parms.op[1] = i;

      if( dalVoice_mapAcntNumToClientInst ( &parms, &clientInst ) == CMSRET_SUCCESS )
      {
         parms.op[1] = clientInst;
         printf( "SIP Client %d:\n", i);
         printf( "--------------------\n" );

         if ( dalVoice_GetSipClientEnable( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   Client Enabled: %s\n",  objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetSipClientStatus( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   Status: %s\n",  objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetVlCFCallerIDName( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   DisplayName : %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetVlSipURI( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   Extension: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetVlSipAuthUserName( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   Authentication Name: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetVlSipAuthPassword( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   Password: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetT38Enable( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   T.38 enabled: %s\n", objValue );
         }


         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetSipContactUri( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   SIP override contact URI: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetSipClientAttachedNetworkIdx( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   Attached Network: SIP_Network_%s\n", objValue );
         }
      }
   }

   cmsMem_free( objValue );

   return (ret);
}



/***************************************************************************
* Function Name: dumpSipNetworkParams
* Description  : Dump service provider specific parmaters.
* These parameters have a per line scope in callmanager and are stored
* at the voice profile level in TR104
****************************************************************************/
static CmsRet dumpSipNetworkParams(int spNum)
{
   int vpInst=0, networkInst, numOfNetwork, i;
   DAL_VOICE_PARMS parms;
   CmsRet ret = CMSRET_SUCCESS;
   char* objValue = NULL;

   /* Mapping spnum to vpInst */
   dalVoice_mapSpNumToSvcInst( spNum, &vpInst );

   /* Fill in the parameter structure needed */
   parms.op[0] = vpInst;
   dalVoice_GetNumSipNetwork( &parms, &numOfNetwork );
   if( numOfNetwork <= 0 )
   {
      printf( "\n" );
      printf( "No valid Sip Network\n");
      return  ret;
   }

   /* Allocate memory for the object to be obtained through DAL APIs. */
   objValue = cmsMem_alloc( MAX_TR104_OBJ_SIZE, ALLOC_ZEROIZE );
   if ( objValue == NULL )
   {
      cmsLog_error( "Could not get allocate memory for object.\n" );
      return ( CMSRET_RESOURCE_EXCEEDED );
   }

   for( i = 0; i < numOfNetwork; i++ )
   {
      printf( "\n" );
      parms.op[1] = i;

      memset( objValue, 0, MAX_TR104_OBJ_SIZE);
      if( dalVoice_mapNetworkNumToInst( &parms , &networkInst ) == CMSRET_SUCCESS )
      {
         parms.op[1] = networkInst;
         printf( "SIP Network %d:\n", i);
         printf( "--------------------\n" );
         printf( "   Network status: %s\n",  objValue );
#ifndef VOICE_IMS_SUPPORT
         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetSipTransportString( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   SIP Transport: %s\n",  objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetSipRegistrarServer( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   Registrar Server: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetSipRegistrarServerPort( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   Registrar Port: %s\n", objValue );
         }
#else
	     memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetSipOutboundProxy( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   PCSCF: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetSipOutboundProxyPort( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   PCSCF Port: %s\n", objValue );
         }
#endif
         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetSipRegisterExpires( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   Registration Expiry: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetSipRegisterRetryInterval( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   Registration Retry Interval: %s\n", objValue );
         }
#ifndef VOICE_IMS_SUPPORT
         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetSipProxyServer( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   Proxy Server: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetSipProxyServerPort( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   Proxy Port: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetSipOutboundProxy( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   Outbound Proxy: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetSipOutboundProxyPort( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   Outbound Port: %s\n", objValue );
         }
#endif
         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetSipUserAgentDomain( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   UserAgent Domain: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetSipUserAgentPort( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   UserAgent Port: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if( dalVoice_GetSipConferencingURI( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS)
         {
            printf( "   Conf Call URI: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if( dalVoice_GetSipConferencingOption( &parms, objValue, MAX_TR104_OBJ_SIZE) == CMSRET_SUCCESS)
         {
            printf( "   Conf Call option: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if( dalVoice_GetSipNetworkCodecList( &parms, objValue, MAX_TR104_OBJ_SIZE) == CMSRET_SUCCESS)
         {
            printf( "   Codec List: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if( dalVoice_GetSipNetworkVoipProfileIdx( &parms, objValue, MAX_TR104_OBJ_SIZE) == CMSRET_SUCCESS)
         {
            printf( "   VoIP Profile: VoIP_Profile_%s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetSipDSCPMark( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   DSCP: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetSipTimerB( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   Timer B: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetSipTimerF( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   Timer F: %s\n", objValue );
         }
#ifndef VOICE_IMS_SUPPORT
         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetSipToTagMatching( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   SIP TagMatch: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetSipFailoverEnable( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   SIP Failover: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetSipOptionsEnable( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   SIP OPTIONS ping: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetSipSecRegistrarAddr( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   Secondary Registrar: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetSipSecRegistrarPort( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   Secondary Registrar Port: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetSipSecProxyAddr( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   Secondary Proxy: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetSipSecProxyPort( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   Secondary Proxy Port: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetSipSecOutboundProxyAddr( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   Secondary Outbound Proxy: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetSipSecOutboundProxyPort( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   Secondary Outbound Proxy Port: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetSipSecDomainName( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   Secondary UserAgent Domain: %s\n", objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if ( dalVoice_GetSipBackToPrimOption( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            printf( "   Back-To-Primary Mode: %s\n", objValue );
         }
#endif
      }
   }

   cmsMem_free( objValue );

   return (ret);
}

/***************************************************************************
* Function Name: dumpSipMappingParams
* Description  : Dump service provider specific parmaters.
* These parameters have a per line scope in callmanager and are stored
* at the voice profile level in TR104
****************************************************************************/
static CmsRet dumpCallCtlMappingParams(int spNum)
{
   int vpInst=0, mapInst, numOfMap, i;
   DAL_VOICE_PARMS parms;
   CmsRet ret = CMSRET_SUCCESS;
   char* objValue = NULL;

   /* Allocate memory for the object to be obtained through DAL APIs. */
   objValue = cmsMem_alloc( MAX_TR104_OBJ_SIZE, ALLOC_ZEROIZE );
   if ( objValue == NULL )
   {
      cmsLog_error( "Could not get allocate memory for object.\n" );
      return ( CMSRET_RESOURCE_EXCEEDED );
   }

   /* Mapping spnum to vpInst */
   dalVoice_mapSpNumToSvcInst( spNum, &vpInst );

   /* Fill in the parameter structure needed */
   parms.op[0] = vpInst;
   dalVoice_GetNumIncomingMap( &parms, &numOfMap );
   if( numOfMap <= 0 )
   {
      printf( "\n" );
      printf( "No valid Incoming Map\n");
   }
   else
   {
      for( i = 0; i < numOfMap; i++ )
      {
         printf( "\n" );
         parms.op[1] = i;

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if( dalVoice_mapIncomingMapNumToInst( &parms , &mapInst ) == CMSRET_SUCCESS )
         {
            parms.op[1] = mapInst;
            printf( "Incoming Map %d:\n", i);
            printf( "--------------------\n" );

            memset( objValue, 0, MAX_TR104_OBJ_SIZE);
            if ( dalVoice_GetIncomingMapEnable( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
            {
               printf( "   Map enable: %s\n",  objValue );
            }

            memset( objValue, 0, MAX_TR104_OBJ_SIZE);
            if ( dalVoice_GetIncomingMapLineNum( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
            {
               printf( "   Line: %s\n",  objValue );
            }

            memset( objValue, 0, MAX_TR104_OBJ_SIZE);
            if ( dalVoice_GetIncomingMapExtNum( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
            {
               printf( "   Extension: %s\n", objValue );
            }

            memset( objValue, 0, MAX_TR104_OBJ_SIZE);
            if ( dalVoice_GetIncomingMapOrder( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
            {
               printf( "   Order: %s\n", objValue );
            }

            memset( objValue, 0, MAX_TR104_OBJ_SIZE);
            if ( dalVoice_GetIncomingMapTimeout( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
            {
               printf( "   Timeout: %s\n", objValue );
            }
         }
      }
   }

   parms.op[0] = vpInst;
   dalVoice_GetNumOutgoingMap( &parms, &numOfMap );
   if( numOfMap <= 0 )
   {
      printf( "\n" );
      printf( "No valid Outgoing Map\n");
   }
   else
   {
      for( i = 0; i < numOfMap; i++ )
      {
         printf( "\n" );
         parms.op[1] = i;

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if( dalVoice_mapOutgoingMapNumToInst( &parms , &mapInst ) == CMSRET_SUCCESS )
         {
            parms.op[1] = mapInst;
            printf( "Outgoing Map %d:\n", i);
            printf( "--------------------\n" );

            memset( objValue, 0, MAX_TR104_OBJ_SIZE);
            if ( dalVoice_GetOutgoingMapEnable( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
            {
               printf( "   Map enable: %s\n",  objValue );
            }

            memset( objValue, 0, MAX_TR104_OBJ_SIZE);
            if ( dalVoice_GetOutgoingMapLineNum( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
            {
               printf( "   Line: %s\n",  objValue );
            }

            memset( objValue, 0, MAX_TR104_OBJ_SIZE);
            if ( dalVoice_GetOutgoingMapExtNum( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
            {
               printf( "   Extension: %s\n", objValue );
            }

            memset( objValue, 0, MAX_TR104_OBJ_SIZE);
            if ( dalVoice_GetOutgoingMapOrder( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
            {
               printf( "   Order: %s\n", objValue );
            }
         }
      }
   }


   cmsMem_free( objValue );

   return (ret);
}

/***************************************************************************
* Function Name: dumpCallCtlFeatureSetParams
* Description  : Dump service provider specific parmaters.
* These parameters have a per line scope in callmanager and are stored
* at the voice profile level in TR104
****************************************************************************/
static CmsRet dumpCallCtlFeatureSetParams(int spNum)
{
   int vpInst=0, setInst, numOfFeatureSet, i;
   DAL_VOICE_PARMS parms;
   CmsRet ret = CMSRET_SUCCESS;
   char* objValue = NULL;

   /* Allocate memory for the object to be obtained through DAL APIs. */
   objValue = cmsMem_alloc( MAX_TR104_OBJ_SIZE, ALLOC_ZEROIZE );
   if ( objValue == NULL )
   {
      cmsLog_error( "Could not get allocate memory for object.\n" );
      return ( CMSRET_RESOURCE_EXCEEDED );
   }

   /* Mapping spnum to vpInst */
   dalVoice_mapSpNumToSvcInst( spNum, &vpInst );

   /* Fill in the parameter structure needed */
   parms.op[0] = vpInst;
   dalVoice_GetNumCallFeatureSet( &parms, &numOfFeatureSet );
   if( numOfFeatureSet <= 0 )
   {
      printf( "\n" );
      printf( "No valid calling feature set\n");
   }
   else
   {
      for( i = 0; i < numOfFeatureSet; i++ )
      {
         printf( "\n" );
         parms.op[1] = i;

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if( dalVoice_mapCallFeatureSetNumToInst( &parms , &setInst ) == CMSRET_SUCCESS )
         {
            parms.op[1] = setInst;
            printf( "Calling Feature Set %d:\n", i);
            printf( "----------------------\n" );

            memset( objValue, 0, MAX_TR104_OBJ_SIZE);
            if ( dalVoice_GetVlCFCallId( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
            {
               printf( "   CallID enabled: %s\n",  objValue );
            }

            memset( objValue, 0, MAX_TR104_OBJ_SIZE);
            if ( dalVoice_GetVlCFCallIdName( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
            {
               printf( "   CallID Name enabled: %s\n",  objValue );
            }

            memset( objValue, 0, MAX_TR104_OBJ_SIZE);
            if ( dalVoice_GetVlCFCallWaiting( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
            {
               printf( "   Call Waiting enabled: %s\n", objValue );
            }

            memset( objValue, 0, MAX_TR104_OBJ_SIZE);
            if ( dalVoice_GetVlCFCallFwdAll( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
            {
               printf( "   CallFwd All enabled: %s\n",  objValue );
            }

            memset( objValue, 0, MAX_TR104_OBJ_SIZE);
            if ( dalVoice_GetVlCFCallFwdNum( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
            {
               printf( "   CallFwd Number: %s\n",  objValue );
            }

            memset( objValue, 0, MAX_TR104_OBJ_SIZE);
            if ( dalVoice_GetVlCFCallFwdBusy( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
            {
               printf( "   CallFwd OnBusy enabled: %s\n",  objValue );
            }

            memset( objValue, 0, MAX_TR104_OBJ_SIZE);
            if ( dalVoice_GetVlCFCallFwdNoAns( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
            {
               printf( "   CallFwd NoAnswer enabled: %s\n",  objValue );
            }

            memset( objValue, 0, MAX_TR104_OBJ_SIZE);
            if ( dalVoice_GetVlCFCallBarring( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
            {
               printf( "   Outgoing call barring enabled: %s\n",  objValue );
            }

            memset( objValue, 0, MAX_TR104_OBJ_SIZE);
            if ( dalVoice_GetVlCFCallBarringPin( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
            {
               printf( "   Outgoing call barring PIN: %s\n",  objValue );
            }

            memset( objValue, 0, MAX_TR104_OBJ_SIZE);
            if ( dalVoice_GetVlCFCallBarringDigitMap( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
            {
               printf( "   Outgoing call barring digit map: %s\n",  objValue );
            }

            memset( objValue, 0, MAX_TR104_OBJ_SIZE);
            if ( dalVoice_GetVlCFMWIEnable( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
            {
               printf( "   MWI enabled: %s\n",  objValue );
            }

            memset( objValue, 0, MAX_TR104_OBJ_SIZE);
            if ( dalVoice_GetVlCFCallTransfer( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
            {
               printf( "   CallTransfer enabled: %s\n",  objValue );
            }

            memset( objValue, 0, MAX_TR104_OBJ_SIZE);
            if ( dalVoice_GetVlCFDoNotDisturb( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
            {
               printf( "   DoNotDisturb enabled: %s\n", objValue );
            }

            memset( objValue, 0, MAX_TR104_OBJ_SIZE);
            if ( dalVoice_GetVlCFAnonymousCalling( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
            {
               printf( "   AnonymousCall enabled: %s\n", objValue );
            }

            memset( objValue, 0, MAX_TR104_OBJ_SIZE);
            if ( dalVoice_GetVlCFAnonCallBlck( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
            {
               printf( "   Anonymous CallBlock enabled: %s\n", objValue );
            }

            memset( objValue, 0, MAX_TR104_OBJ_SIZE);
            if ( dalVoice_GetVlCFWarmLine( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
            {
               printf( "   WarmLine enabled: %s\n", objValue );
            }

            memset( objValue, 0, MAX_TR104_OBJ_SIZE);
            if ( dalVoice_GetVlCFWarmLineNum( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
            {
               printf( "   WarmLine Number: %s\n", objValue );
            }

            parms.op[2] = DAL_VOICE_FEATURE_CODE_CALLRETURN;        /* last call return */
            memset( objValue, 0, MAX_TR104_OBJ_SIZE);
            if ( dalVoice_GetVlCFFeatureEnabled( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
            {
               printf( "   Call Return enabled: %s\n", objValue );
            }

            parms.op[2] = DAL_VOICE_FEATURE_CODE_CALLREDIAL;        /* last call return */
            memset( objValue, 0, MAX_TR104_OBJ_SIZE);
            if ( dalVoice_GetVlCFFeatureEnabled( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
            {
               printf( "   Call Redial enabled: %s\n", objValue );
            }
         }
      }

      printf( "\n" );
      printf( "Global Calling Features:\n" );
      printf( "------------------------\n" );

      memset( objValue, 0, MAX_TR104_OBJ_SIZE);
      if ( dalVoice_GetEuroFlashEnable( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
      {
         printf( "   European flash enable: %s\n", objValue );
      }
   }


   cmsMem_free( objValue );

   return (ret);
}

static CmsRet dumpVoiceStats(int spNum)
{
   int numOfLines, i;
   int lineInst = 0, vpInst;
   DAL_VOICE_PARMS parms;
   CmsRet ret = CMSRET_SUCCESS;
   char* objValue  = NULL;

   /* Mapping spnum to vpInst */
   dalVoice_mapSpNumToSvcInst( spNum, &vpInst );

   /* Fill in the parameter structure needed */
   parms.op[0] = vpInst;
   dalVoice_GetNumOfLine( &parms, &numOfLines );
   if( numOfLines <= 0 )
   {
      printf( "\n" );
      printf( "No valid Call Control Line\n");
      return  ret;
   }

   /* Allocate memory for the object to be obtained through DAL APIs. */
   objValue = cmsMem_alloc( MAX_TR104_OBJ_SIZE, ALLOC_ZEROIZE );
   if ( objValue == NULL )
   {
      cmsLog_error( "Could not get allocate memory for object.\n" );
      return ( CMSRET_RESOURCE_EXCEEDED );
   }

   for( i = 0; i < numOfLines; i++ )
   {
      printf( "\n" );

      if( dalVoice_mapAcntNumToLineInst( vpInst, i, &lineInst ) == CMSRET_SUCCESS)
      {
         printf( "Call Control Line %d:\n", i);
         printf( "--- RTP statistics ----\n" );

         parms.op[1] = lineInst;
         memset(objValue, 0, MAX_TR104_OBJ_SIZE);
         dalVoice_GetCcLineStatsRtpPacketSentString(&parms, objValue, MAX_TR104_OBJ_SIZE);
         printf( "   Packets Sent    : %s\n", objValue );

         memset(objValue, 0, MAX_TR104_OBJ_SIZE);
         dalVoice_GetCcLineStatsRtpPacketRecvString(&parms, objValue, MAX_TR104_OBJ_SIZE);
         printf( "   Packets Received: %s\n", objValue );

         memset(objValue, 0, MAX_TR104_OBJ_SIZE);
         dalVoice_GetCcLineStatsRtpPacketLostString(&parms, objValue, MAX_TR104_OBJ_SIZE);
         printf( "   Packets Lost    : %s\n", objValue );

         memset(objValue, 0, MAX_TR104_OBJ_SIZE);
         dalVoice_GetCcLineStatsRtpBytesSentString(&parms, objValue, MAX_TR104_OBJ_SIZE);
         printf( "   Total Bytes Sent: %s\n", objValue);

         memset(objValue, 0, MAX_TR104_OBJ_SIZE);
         dalVoice_GetCcLineStatsRtpBytesRecvString(&parms, objValue, MAX_TR104_OBJ_SIZE);
         printf( "   Total Bytes Received: %s\n", objValue);

         memset(objValue, 0, MAX_TR104_OBJ_SIZE);
         dalVoice_GetCcLineStatsInCallRecvString(&parms, objValue, MAX_TR104_OBJ_SIZE);
         printf( "   Incoming Call Received: %s\n", objValue);

         memset(objValue, 0, MAX_TR104_OBJ_SIZE);
         dalVoice_GetCcLineStatsInCallConnString(&parms, objValue, MAX_TR104_OBJ_SIZE);
         printf( "   Incoming Call Connected: %s\n", objValue);

         memset(objValue, 0, MAX_TR104_OBJ_SIZE);
         dalVoice_GetCcLineStatsInCallFailedString(&parms, objValue, MAX_TR104_OBJ_SIZE);
         printf( "   Incoming Call Failed: %s\n", objValue);

         memset(objValue, 0, MAX_TR104_OBJ_SIZE);
         dalVoice_GetCcLineStatsInCallDropString(&parms, objValue, MAX_TR104_OBJ_SIZE);
         printf( "   Incoming Call Dropped: %s\n", objValue);

         memset(objValue, 0, MAX_TR104_OBJ_SIZE);
         dalVoice_GetCcLineStatsOutCallAttemptString(&parms, objValue, MAX_TR104_OBJ_SIZE);
         printf( "   Outgoing Call Attempted: %s\n", objValue);

         memset(objValue, 0, MAX_TR104_OBJ_SIZE);
         dalVoice_GetCcLineStatsOutCallConnString(&parms, objValue, MAX_TR104_OBJ_SIZE);
         printf( "   Outgoing Call Connected: %s\n", objValue);

         memset(objValue, 0, MAX_TR104_OBJ_SIZE);
         dalVoice_GetCcLineStatsOutCallFailedString(&parms, objValue, MAX_TR104_OBJ_SIZE);
         printf( "   Outgoing Call Failed: %s\n", objValue);

         memset(objValue, 0, MAX_TR104_OBJ_SIZE);
         dalVoice_GetCcLineStatsOutCallDropString(&parms, objValue, MAX_TR104_OBJ_SIZE);
         printf( "   Outgoing Call Dropped: %s\n", objValue);

      }

      printf( "\n" );
   }

   cmsMem_free( objValue );

   return ret;
}

static CmsRet dumpVoiceCallLogs(int spNum)
{
   int numOfLogs, maxCallLogs, i;
   int logInst = 0, vpInst;
   DAL_VOICE_PARMS parms;
   CmsRet ret = CMSRET_SUCCESS;
   char* objValue  = NULL;

   /* Mapping spnum to vpInst */
   dalVoice_mapSpNumToSvcInst( spNum, &vpInst );

   /* Fill in the parameter structure needed */
   parms.op[0] = vpInst;
   dalVoice_GetMaxCallLogCount( &parms, &maxCallLogs );
   dalVoice_GetNumVoiceCallLog( &parms, &numOfLogs );
   if( numOfLogs <= 0 )
   {
      printf( "\n" );
      printf( "No Calls\n");
      return  ret;
   }

   /* Allocate memory for the object to be obtained through DAL APIs. */
   objValue = cmsMem_alloc( MAX_TR104_OBJ_SIZE, ALLOC_ZEROIZE );
   if ( objValue == NULL )
   {
      cmsLog_error( "Could not get allocate memory for object.\n" );
      return ( CMSRET_RESOURCE_EXCEEDED );
   }

   for( i = 0; i < numOfLogs; i++ )
   {
      printf( "\n" );

      parms.op[1] = i;
      if( dalVoice_mapCallLogNumToInst( &parms, &logInst ) == CMSRET_SUCCESS)
      {
         parms.op[1] = logInst;
         printf( "Maxium Call Log %d:\n", maxCallLogs);
         printf( "Call Log %d:\n", i);
         printf( "--------------------------------\n" );

         memset(objValue, 0, MAX_TR104_OBJ_SIZE);
         dalVoice_GetVoiceCallLogDirection(&parms, objValue, MAX_TR104_OBJ_SIZE);
         printf( "    %s Call\n", objValue );

         memset(objValue, 0, MAX_TR104_OBJ_SIZE);
         dalVoice_GetVoiceCallLogCaller(&parms, objValue, MAX_TR104_OBJ_SIZE);
         printf( "    Calling Number : %s\n", objValue );

         memset(objValue, 0, MAX_TR104_OBJ_SIZE);
         dalVoice_GetVoiceCallLogStartTime(&parms, objValue, MAX_TR104_OBJ_SIZE);
         printf( "    Call Starts    : %s\n", objValue );

         memset(objValue, 0, MAX_TR104_OBJ_SIZE);
         dalVoice_GetVoiceCallLogDuration(&parms, objValue, MAX_TR104_OBJ_SIZE);
         printf( "    Call Duration  : %s\n", objValue );

         memset(objValue, 0, MAX_TR104_OBJ_SIZE);
         dalVoice_GetVoiceCallLogReason(&parms, objValue, MAX_TR104_OBJ_SIZE);
         printf( "    Call Terminate : %s\n", objValue);
      }

      printf( "\n" );
   }

   cmsMem_free( objValue );

   return ret;
}

/***************************************************************************
* Function Name: dumpVoicePotsFxsDiag
* Description  : Dump FXS Diag state and result
****************************************************************************/
static CmsRet dumpVoicePotsFxsDiag(int spNum)
{
   int numOfFxs, i;
   int fxsInst, vpInst;
   DAL_VOICE_PARMS parms;
   CmsRet ret = CMSRET_SUCCESS;
   char* objValue  = NULL;

   /* Mapping spnum to vpInst */
   dalVoice_mapSpNumToSvcInst( spNum, &vpInst );

   /* Fill in the parameter structure needed */
   parms.op[0] = vpInst;
   ret = dalVoice_GetNumPhysFxsEndpt( &numOfFxs );
   if( ret != CMSRET_SUCCESS || numOfFxs <= 0 )
   {
      printf( "\n" );
      printf( "No valid FXS line\n");
      return  ret;
   }

   /* Allocate memory for the object to be obtained through DAL APIs. */
   objValue = cmsMem_alloc( MAX_TR104_OBJ_SIZE, ALLOC_ZEROIZE );
   if ( objValue == NULL )
   {
      cmsLog_error( "Could not get allocate memory for object.\n" );
      return ( CMSRET_RESOURCE_EXCEEDED );
   }

   for( i = 0; i < numOfFxs; i++ )
   {
      printf( "\n" );
      parms.op[1] = i;

      if( dalVoice_mapPotsFxsNumToInst( &parms, &fxsInst ) == CMSRET_SUCCESS)
      {
         parms.op[1] = fxsInst;
         printf( "FXS  line %d:\n", i);
         printf( "--------------------\n" );

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if( dalVoice_GetVoiceFxsLineTxGainStr( &parms, objValue, MAX_TR104_OBJ_SIZE) == CMSRET_SUCCESS )
         {
            printf( "   txGain: %s\n",  objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if( dalVoice_GetVoiceFxsLineRxGainStr( &parms, objValue, MAX_TR104_OBJ_SIZE) == CMSRET_SUCCESS )
         {
            printf( "   rxGain: %s\n",  objValue );
         }

         if( dalVoice_GetFxsDiagTestState( &parms, objValue, MAX_TR104_OBJ_SIZE) == CMSRET_SUCCESS )
         {
            printf( "   Diag State: %s\n",  objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if( dalVoice_GetFxsDiagTestSelector( &parms, objValue, MAX_TR104_OBJ_SIZE) == CMSRET_SUCCESS )
         {
            printf( "   Select Test: %s\n",  objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if( dalVoice_GetFxsDiagTestResult( &parms, objValue, MAX_TR104_OBJ_SIZE) == CMSRET_SUCCESS )
         {
            printf( "   Result: %s\n",  objValue );
         }
      }
   }

   cmsMem_free( objValue );

   return (ret);
}

/***************************************************************************
* Function Name: dumpVoicePotsFxs
* Description  : Dump FXS state and result
****************************************************************************/
static CmsRet dumpVoicePotsFxs(int spNum)
{
   int numOfFxs, i;
   int fxsInst, vpInst;
   DAL_VOICE_PARMS parms;
   CmsRet ret = CMSRET_SUCCESS;
   char* objValue  = NULL;

   /* Mapping spnum to vpInst */
   dalVoice_mapSpNumToSvcInst( spNum, &vpInst );

   /* Fill in the parameter structure needed */
   parms.op[0] = vpInst;
   ret = dalVoice_GetNumPhysFxsEndpt( &numOfFxs );
   if( ret != CMSRET_SUCCESS || numOfFxs <= 0 )
   {
      printf( "\n" );
      printf( "No valid FXS line\n");
      return  ret;
   }

   /* Allocate memory for the object to be obtained through DAL APIs. */
   objValue = cmsMem_alloc( MAX_TR104_OBJ_SIZE, ALLOC_ZEROIZE );
   if ( objValue == NULL )
   {
      cmsLog_error( "Could not get allocate memory for object.\n" );
      return ( CMSRET_RESOURCE_EXCEEDED );
   }

   for( i = 0; i < numOfFxs; i++ )
   {
      printf( "\n" );
      parms.op[1] = i;

      if( dalVoice_mapPotsFxsNumToInst( &parms, &fxsInst ) == CMSRET_SUCCESS)
      {
         parms.op[1] = fxsInst;
         printf( "FXS  line %d:\n", i);
         printf( "--------------------\n" );

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if( dalVoice_GetVoiceFxsLineTxGainStr( &parms, objValue, MAX_TR104_OBJ_SIZE) == CMSRET_SUCCESS )
         {
            printf( "   txGain: %s\n",  objValue );
         }

         memset( objValue, 0, MAX_TR104_OBJ_SIZE);
         if( dalVoice_GetVoiceFxsLineRxGainStr( &parms, objValue, MAX_TR104_OBJ_SIZE) == CMSRET_SUCCESS )
         {
            printf( "   rxGain: %s\n",  objValue );
         }
      }
   }

   cmsMem_free( objValue );

   return (ret);
}


#endif /* DMP_VOICE_SERVICE_2 */
#endif  /* BRCM_VOICE_SUPPORT */

#endif /* SUPPORT_CLI_CMD */


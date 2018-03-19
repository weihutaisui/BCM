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
* :>
*
****************************************************************************
*
*  Filename: cli_voice.c
*
****************************************************************************
*  Description:
*
*
****************************************************************************/

/****************************************************************************
*
*  cli_voice.c
*
*  PURPOSE:
*
*  NOTES:
*
****************************************************************************/

#ifdef SUPPORT_CLI_CMD

#ifdef BRCM_VOICE_SUPPORT

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
   int numArgs;
   UBOOL8 global;     /* Indicates a parameter as per voice profile for MDM but a global parameter for Call manager */
   const char *info;
   const char *syntax;
} VOICE_DAL_MAP;

/* ---- Private Variables ------------------------------------------------ */

/* Table of DAL function mapping */
static VOICE_DAL_MAP voiceDalCliMap[] =
{
   /*name               dalFunc                              numArgs  global info                                  syntax */
   { "defaults",        dalVoice_SetDefaults,                      0, FALSE, "Default VoIP setup",                 "<None>"                                        },
   { "boundIfname",     dalVoice_SetBoundIfName,                   1, FALSE, "voice network interface",            "<LAN|Any_WAN|(WAN IfName, e.g. nas_0_0_35)>"   },
   { "ipAddrFamily",    dalVoice_SetIpFamily,                      1, FALSE, "IP address family",                  "<IPv4|IPv6>"                                   },
#ifdef BRCM_SIP_VOICE_DNS
   { "dnsAddr",         dalVoice_SetDnsServerAddr,                 1, FALSE, "Voice DNS IP addresss",              "<IP>"                                          },
#endif // BRCM_SIP_VOICE_DNS

#ifdef DMP_X_BROADCOM_COM_PSTNENDPOINT_1
   { "pstnDialPlan",    dalVoice_SetPstnDialPlan,                  2, FALSE, "PSTN dial plan",                     "<pstn line#> <dialPlan>"                       },
   { "pstnRouteRule",   dalVoice_SetPstnRouteRule,                 2, FALSE, "PSTN Route rule",                    "<pstn line#> <Auto|Voip|Line>"                 },
   { "pstnRouteData",   dalVoice_SetPstnRouteData,                 2, FALSE, "PSTN Route data",                    "<pstn line#> <line #|URL for VOIP>"            },
#endif
#ifdef DMP_X_BROADCOM_COM_NTR_1
/* NTR Task variables independent of SIP/MGCP */
   { "ntrEnable",       dalVoice_SetNtrEnable,                     2, FALSE, "NTR Feature",                        "<srvPrv#> <on|off>"                            },
   { "ntrAuto",         dalVoice_SetNtrAutoModeEnable,             2, FALSE, "NTR Auto Mode",                      "<srvPrv#> <on|off>"                            },
   { "ntrManualOffset", dalVoice_SetNtrManualOffset,               2, FALSE, "NTR Manual Offset (Hz)",             "<srvPrv#> <signed Hz adjustment>"              },
   { "ntrDebug",        dalVoice_SetNtrDebugEnable,                2, FALSE, "NTR Debug",                          "<srvPrv#> <on|off>"                            },
   { "ntrSampleRate",   dalVoice_SetNtrSampleRate,                 2, FALSE, "NTR Sample Rate (ms)",               "<srvPrv#> <ms (Range: 10 ~ 10000)>"            },
   { "ntrPllBandwidth", dalVoice_SetNtrPllBandwidth,               2, FALSE, "NTR PLL Bandwidth (Hz)",             "<srvPrv#> <Hz (Range: 0 ~ [1/Sample Rate])>"   },
   { "ntrDampingFactor",dalVoice_SetNtrDampingFactor,              2, FALSE, "NTR Damping Factor",                 "<srvPrv#> <Range: 0 ~ 1>"                      },
#endif /* DMP_X_BROADCOM_COM_NTR_1 */

   /* Service provider specific parameters */
   { "locale",          dalVoice_SetRegion,                        2, TRUE,  "2 or 3 character code",              "<srvPrv#> <region>"                            },
#ifdef SIPLOAD
   { "DTMFMethod",      dalVoice_SetDTMFMethod,                    2, TRUE,  "DTMF digit passing method",          "<srvPrv#> <InBand|RFC2833|SIPInfo>"            },
   { "hookFlashMethod", dalVoice_SetHookFlashMethod,               2, TRUE,  "Hook flash method",                  "<srvPrv#> <SIPInfo|None>"                      },
#ifdef STUN_CLIENT
   { "STUNServer",      dalVoice_SetSTUNServer,                    2, TRUE,  "STUN server",                        "<srvPrv#> <domainName|IP>"                     },
   { "STUNSrvPort",     dalVoice_SetSTUNServerPort,                2, TRUE,  "STUN server port",                   "<srvPrv#> <port>"                              },
#endif /* STUN_CLIENT */
   { "transport",       dalVoice_SetSipTransport,                  2, TRUE,  "transport protocol",                 "<srvPrv#> <UDP|TCP|TLS>"                       },
   { "backToPrimary",   dalVoice_SetSipBackToPrimOption,           2, TRUE,  "back-to-primary option",             "<srvPrv#> <Disabled|Silent|Deregistration|SilentDeregistration>"                       },
   { "srtpOption",      dalVoice_SetSrtpOption,                    2, TRUE,  "SRTP usage option",                  "<srvPrv#> <Mandatory|Optional|Disabled>"       },
   { "regRetryInt",     dalVoice_SetSipRegisterRetryInterval,      2, FALSE, "SIP register retryinterval",         "<srvPrv#> <seconds>"                           },
   { "regExpires",      dalVoice_SetSipRegisterExpires,            2, TRUE,  "Register expires hdr val",           "<srvPrv#> <seconds>"                           },
   { "rtpDSCPMark",     dalVoice_SetRtpDSCPMark,                   2, TRUE,  "RTP outgoing DSCP mark",             "<srvPrv#> <mark>"                              },
   { "logServer",       dalVoice_SetLogServer,                     2, TRUE,  "Log server",                         "<srvPrv#> <hostName|IP>"                       },
   { "logPort",         dalVoice_SetLogServerPort,                 2, TRUE,  "Log server port",                    "<srvPrv#> <port>"                              },
   { "digitMap",        dalVoice_SetDigitMap,                      2, FALSE, "dial digit map",                     "<srvPrv#> <digitmap>"                          },
   { "T38",             dalVoice_SetT38Enable,                     2, FALSE, "enable/disable T38",                 "<srvPrv#> on|off"                              },
   { "V18",             dalVoice_SetV18Enable,                     2, FALSE, "enable/disable V.18 detection",      "<srvPrv#> on|off"                              },
   { "reg",             dalVoice_SetSipRegistrarServer,            2, FALSE, "SIP registrar server",               "<srvPrv#> <hostName|IP>"                       },
   { "regPort",         dalVoice_SetSipRegistrarServerPort,        2, FALSE, "SIP registrar server port",          "<srvPrv#> <port>"                              },
   { "proxy",           dalVoice_SetSipProxyServer,                2, FALSE, "SIP proxy server",                   "<srvPrv#> <hostName|IP>"                       },
   { "proxyPort",       dalVoice_SetSipProxyServerPort,            2, FALSE, "SIP proxy server port",              "<srvPrv#> <port>"                              },
   { "obProx",          dalVoice_SetSipOutboundProxy,              2, FALSE, "SIP outbound proxy",                 "<srvPrv#> <hostName|IP>"                       },
   { "obProxPort",      dalVoice_SetSipOutboundProxyPort,          2, FALSE, "SIP outbound proxy port",            "<srvPrv#> <port>"                              },
   { "sipDomain",       dalVoice_SetSipUserAgentDomain,            2, TRUE,  "SIP user agent domain",              "<srvPrv#> <CPE_domainName>"                    },
   { "sipPort",         dalVoice_SetSipUserAgentPort,              2, TRUE,  "SIP user agent port",                "<srvPrv#> <port>"                              },
   { "sipDSCPMark",     dalVoice_SetSipDSCPMark,                   2, TRUE,  "SIP outgoing DSCP mark",             "<srvPrv#> <mark>"                              },
   { "musicServer",     dalVoice_SetSipMusicServer,                2, TRUE,  "SIP music server",                   "<srvPrv#> <hostName|IP>"                       },
   { "musicSrvPort",    dalVoice_SetSipMusicServerPort,            2, TRUE,  "SIP music server port",              "<srvPrv#> <port>"                              },
   { "confURI",         dalVoice_SetSipConferencingURI,            2, TRUE,  "SIP conferencing URI",               "<srvPrv#> <hostName>"                          },
   { "confOption",      dalVoice_SetSipConferencingOption,         2, TRUE,  "SIP conferencing option",            "<srvPrv#> <Local|Refer participants|Refer server>"},
   { "secSipDomain",    dalVoice_SetSipSecDomainName,              2, TRUE,  "SIP Secondary Domain Name",          "<srvPrv#> <name>"                              },
   { "secProxyAddr",    dalVoice_SetSipSecProxyAddr,               2, TRUE,  "SIP Secondary proxy IP",             "<srvPrv#> <IP>"                                },
   { "secProxyPort",    dalVoice_SetSipSecProxyPort,               2, TRUE,  "SIP Secondary proxy port",           "<srvPrv#> <port>"                              },
   { "secObProxyAddr",  dalVoice_SetSipSecOutboundProxyAddr,       2, TRUE,  "SIP Secondary outbound proxy IP",    "<srvPrv#> <IP>"                                },
   { "secObProxyPort",  dalVoice_SetSipSecOutboundProxyPort,       2, TRUE,  "SIP Secondary outbound proxy port",  "<srvPrv#> <port>"                              },
   { "secRegistrarAddr",dalVoice_SetSipSecRegistrarAddr,           2, TRUE,  "SIP Secondary registrar IP",         "<srvPrv#> <IP>"                                },
   { "secRegistrarPort",dalVoice_SetSipSecRegistrarPort,           2, TRUE,  "SIP Secondary registrar port",       "<srvPrv#> <port>"                              },
   { "failoverEnable",  dalVoice_SetSipFailoverEnable,             2, TRUE,  "SIP failover enable",                "<srvPrv#> <on|off>"                            },
   { "sipOptions",      dalVoice_SetSipOptionsEnable,              2, TRUE,  "SIP OPTIONS ping enable",            "<srvPrv#> <on|off>"                            },
   { "tagMatching",     dalVoice_SetSipToTagMatching,              2, TRUE,  "SIP to tag matching",                "<srvPrv#> <on|off>"                            },
   { "timerB",          dalVoice_SetSipTimerB,                     2, TRUE,  "SIP protocol B timer",               "<srvPrv#> <time in ms>"                        },
   { "timerF",          dalVoice_SetSipTimerF,                     2, TRUE,  "SIP protocol F timer",               "<srvPrv#> <time in ms>"                        },
   { "euroFlashEnable", dalVoice_SetEuroFlashEnable,               2, TRUE,  "European flash enable",              "<srvPrv#> <on|off>"                            },

   /* Account specific parameters */
   { "lineStatus",      dalVoice_SetVlEnable,                      3, FALSE, "Activate line",                      "<srvPrv#> <accnt#> <on|off>"                   },
   { "physEndpt",       dalVoice_SetVlPhyReferenceList,            3, FALSE, "Phys Endpt",                         "<srvPrv#> <accnt#> <id>"                       },
   { "extension",       dalVoice_SetVlSipURI,                      3, FALSE, "SIP extension",                      "<srvPrv#> <accnt#> <URI>"                      },
   { "dispName",        dalVoice_SetVlCFCallerIDName,              3, FALSE, "SIP Display Name",                   "<srvPrv#> <accnt#> <Name>"                     },
   { "authName",        dalVoice_SetVlSipAuthUserName,             3, FALSE, "SIP auth name",                      "<srvPrv#> <accnt#> <name>"                     },
   { "authPwd",         dalVoice_SetVlSipAuthPassword,             3, FALSE, "SIP auth password",                  "<srvPrv#> <accnt#> <pwd>"                      },
   { "MWIEnable",       dalVoice_SetVlCFMWIEnable,                 3, FALSE, "Msg Waiting Indication",             "<srvPrv#> <accnt#> <on|off>"                   },
   { "cfwdNum",         dalVoice_SetVlCFCallFwdNum,                3, FALSE, "call forward number",                "<srvPrv#> <accnt#> <number>"                   },
   { "cfwdAll",         dalVoice_SetVlCFCallFwdAll,                3, FALSE, "call forward all",                   "<srvPrv#> <accnt#> <on|off>"                   },
   { "cfwdNoAns",       dalVoice_SetVlCFCallFwdNoAns,              3, FALSE, "call forward no answer",             "<srvPrv#> <accnt#> <on|off>"                   },
   { "cfwdBusy",        dalVoice_SetVlCFCallFwdBusy,               3, FALSE, "call forward busy",                  "<srvPrv#> <accnt#> <on|off>"                   },
   { "callWait",        dalVoice_SetVlCFCallWaiting,               3, FALSE, "call waiting",                       "<srvPrv#> <accnt#> <on|off>"                   },
   { "anonBlck",        dalVoice_SetVlCFAnonCallBlck,              3, FALSE, "Anonymous call rcv blcking",         "<srvPrv#> <accnt#> <on|off>"                   },
   { "anonCall",        dalVoice_SetVlCFAnonymousCalling,          3, FALSE, "Anonymous outgng calls",             "<srvPrv#> <accnt#> <on|off>"                   },
   { "DND",             dalVoice_SetVlCFDoNotDisturb,              3, FALSE, "do not disturb",                     "<srvPrv#> <accnt#> <on|off>"                   },
   { "CCBS",            dalVoice_SetVlCFCallCompletionOnBusy,      3, FALSE, "Call completion on busy",            "<srvPrv#> <accnt#> <on|off>"                   },
   { "speedDial",       dalVoice_SetVlCFSpeedDial,                 3, FALSE, "Speed dial",                         "<srvPrv#> <accnt#> <on|off>"                   },
   { "warmLine",        dalVoice_SetVlCFWarmLine ,                 3, FALSE, "Warm line",                          "<srvPrv#> <accnt#> <on|off>"                   },
   { "warmLineNum",     dalVoice_SetVlCFWarmLineNum ,              3, FALSE, "Warm line number",                   "<srvPrv#> <accnt#> <number>"                   },
   { "callBarring",     dalVoice_SetVlCFCallBarring,               3, FALSE, "Call barring",                       "<srvPrv#> <accnt#> <on|off>"                   },
   { "callBarrPin",     dalVoice_SetVlCFCallBarringPin,            3, FALSE, "Call barring pin",                   "<srvPrv#> <accnt#> <number>"                   },
   { "callBarrDigMap",  dalVoice_SetVlCFCallBarringDigitMap,       3, FALSE, "Call barring digit map",             "<srvPrv#> <accnt#> <digitmap>"                 },
   { "netPrivacy",      dalVoice_SetVlCFNetworkPrivacy,            3, FALSE, "Network privacy",                    "<srvPrv#> <accnt#> <on|off>"                   },
   { "vad",             dalVoice_SetVlCLSilenceSuppression,        3, FALSE, "enable vad",                         "<srvPrv#> <accnt#> <on|off>"                   },
   { "pTime",           dalVoice_SetVlCLPacketizationPeriod,       3, FALSE, "packetization period",               "<srvPrv#> <accnt#> <pTime>"                    },
   { "codecList",       dalVoice_SetVlCLCodecList,                 3, FALSE, "codec priority list",                "<srvPrv#> <accnt#> <codec(1)[,codec(2)]>"      },
   { "rxGain",          dalVoice_SetVlVPReceiveGain,               3, FALSE, "rxGain (dB)",                        "<srvPrv#> <accnt#> <rxGain>"                   },
   { "txGain",          dalVoice_SetVlVPTransmitGain,              3, FALSE, "txGain (dB)",                        "<srvPrv#> <accnt#> <txGain>"                   },
#endif /* SIPLOAD */

#ifdef MGCPLOAD
   { "callAgent",       dalVoice_SetMgcpCallAgentIpAddress,        2, FALSE, "call agent ip address",              "<srvPrv#><ipaddress>"                          },
   { "gateway",         dalVoice_SetMgcpGatewayName,               2, FALSE, "domain/gateway name",                "<srvPrv#><gateway>"                            },
#endif /* MGCPLOAD */
#ifdef SIPLOAD
   { "cctktracelvl",    dalVoice_SetCCTKTraceLevel,                1, FALSE, "CCTK tracelevel (stop/start reqd)",         "<Info|Warn|Debug|Off>"                  },
   { "cctktracegrp",    dalVoice_SetCCTKTraceGroup,                1, FALSE, "CCTK concat tracegroups (stop/start reqd)", "<CCTK|SCE|Trans|SDP|SIP|Misc|All|None>" },
#endif /* SIPLOAD */
#ifdef DMP_X_ITU_ORG_GPON_1
   { "mgtProt",        dalVoice_SetManagementProtocol,             1, FALSE, "Protocol used to manage Voice",      "<TR69|OMCI>"                                   },
#endif /* DMP_X_ITU_ORG_GPON_1 */

   { "loglevel",       dalVoice_SetModuleLoggingLevel,             2, FALSE, "Voice module-specific log level",    "<general|dsphal|slicslac|cmgr|disp|sipcctk|bos|ept|cms|prov|lhapi|istw> <0-7>" },

   { "NULL",            NULL,                                      0, FALSE, "ERROR",                              "ERROR"                                         }
};


/* ---- Private Function Prototypes -------------------------------------- */

static void voiceShowCmdSyntax();
static void processVoiceCtlSetCmd(char *cmdLine);
static void processVoiceCtlShowCmd(char *cmdLine);
static void processVoiceSaveCmd(char *cmdLine);

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
   printf("voice show stats                  -  show call statistics\n");
   printf("voice show memstats               -  shows memory allocation statistics\n");
   printf("voice show cctkcmstats            -  shows Call Manager & CCTK statistics\n");
#endif /* SIPLOAD */

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
   while( voiceDalCliMap[i].dalFunc != NULL )
   {
      /* left-aligned */
      printf("%-15s %-27s - %-25s\n",voiceDalCliMap[i].name, voiceDalCliMap[i].syntax, voiceDalCliMap[i].info);
      i++;
   }
}

void processVoiceCtlCmd(char *cmdLine)
{
   cmsLog_debug("processVoiceCtlCmd called %s start\n", cmdLine);

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
   int i=0;
   int vpInst, lineInst,spNum,accNum,indexOfLastCliArg;
   char *value = NULL;
#ifdef DMP_X_BROADCOM_COM_PSTNENDPOINT_1
   int pstnNum, pstnInst;
#endif
   memset( &argBlk, 0, sizeof(argBlk) );

   /* Parse arguments into a ptr array */
   memset( arguments, 0, sizeof(arguments) );
   int numArgs = initCfgPtrArray( arguments, cmdLine , strlen(cmdLine)+1 );

    /* Print all arguments */
    cmsLog_debug("Total arguments: %d\n",numArgs);
    for ( i = 0; i< numArgs; i++ )
    {
       cmsLog_debug("Arg%d:%s ", i, arguments[i]);
       if ( i == numArgs-1 )
       {
          cmsLog_debug("\n");
       }
    }

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
   while ( voiceDalCliMap[i].dalFunc != NULL && arguments[VOIP_PARMNAME_ARG_INDEX] != NULL )
   {
      if ( strncasecmp(arguments[VOIP_PARMNAME_ARG_INDEX], voiceDalCliMap[i].name, MAX_CLICMD_NAME_SIZE) == 0 )
      {
         break;
      }
      i++;
   }

   /* Check if we searched through entire table without finding a match */
   if ( voiceDalCliMap[i].dalFunc == NULL )
   {
      cmsLog_error("%ss:: Invalid Parameter Name\n",__FUNCTION__);
      /* Show command list */
      voiceShowCmdSyntax();
      return;
   }

   /* Check if complete arguments are provided for the specific parameter */
   if ( VOIP_FIRST_PARMARG_INDEX + (voiceDalCliMap[i].numArgs - 1) == indexOfLastCliArg )
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
         argBlk.op[0] = 0;

         cmsLck_acquireLock();
         dalVoice_SetModuleLoggingLevel( &argBlk, arguments[VOIP_FIRST_PARMARG_INDEX], arguments[VOIP_FIRST_PARMARG_INDEX+1] );
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
               /* Service provider parameter */
               int maxSp, j;

               cmsLck_acquireLock();
               dalVoice_GetNumSrvProv( &maxSp );
               cmsLck_releaseLock();

               int numSp = (voiceDalCliMap[i].global) ? maxSp : 1;

               for ( j = 0; j < numSp; j++ )
               {
                  spNum = (numSp == 1) ? atoi(arguments[VOIP_FIRST_PARMARG_INDEX]) : j;

                  /* Map to vpInst */
                  cmsLck_acquireLock();
                  dalVoice_mapSpNumToVpInst( spNum, &vpInst );
                  cmsLck_releaseLock();

                  argBlk.op[0] = vpInst;
                  argBlk.op[1] = 0;
                  value = arguments[VOIP_FIRST_PARMARG_INDEX + 1];

                  /* Execute associated DAL function */
                  if ( value != NULL )
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

            break;

            case 3:
            {
               /* Account parameter */
               spNum = atoi(arguments[VOIP_FIRST_PARMARG_INDEX]);
               accNum = atoi(arguments[VOIP_FIRST_PARMARG_INDEX + 1]);

               cmsLck_acquireLock();

               /* Map to vpInst */
               dalVoice_mapSpNumToVpInst( spNum, &vpInst );
               /* Map to lineInst */
               dalVoice_mapAcntNumToLineInst( vpInst, accNum, &lineInst );

               cmsLck_releaseLock();

               argBlk.op[0] = vpInst;
               argBlk.op[1] = lineInst;
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
   if ( value != NULL )
   {
      /* Aquire Lock */
      cmsLck_acquireLock();

      voiceDalCliMap[i].dalFunc( &argBlk, value );

      /* Release Lock */
      cmsLck_releaseLock();
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

   /* Show call statistics */
   if( strstr(cmdLine, "stats") != NULL )
   {
      /* Aquire Lock */
      cmsLck_acquireLock();

      dalVoice_cliDumpStats();

      /* Release Lock */
      cmsLck_releaseLock();
      return;
   }
#endif /* SIPLOAD */
   /* show voice parameters */

   /* Aquire Lock */
   cmsLck_acquireLock();

   dalVoice_cliDumpParams();

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

#endif  /* BRCM_VOICE_SUPPORT */

#endif /* SUPPORT_CLI_CMD */


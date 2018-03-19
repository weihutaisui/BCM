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
*****************************************************************************/


/****************************************************************************
*
*  cgi_voice.c
*
*  PURPOSE:
*
*  NOTES:
*
****************************************************************************/

/* ---- Include Files ---------------------------------------------------- */

#ifdef BRCM_VOICE_SUPPORT

#ifdef DMP_VOICE_SERVICE_2
#include <cms_log.h>
#include <cms_mem.h>
#include <cms_msg.h>

#include <dal_voice.h>
#include <cgi_main.h>
#include <cgi_voice.h>

#include <httpd.h>

/* ---- Public Variables ------------------------------------------------- */

/* ---- Constants and Types ---------------------------------------------- */

#define VIEW_CURRENT          "currentview"
#define VIEW_GLOBAL           "basic"
#define VIEW_SRVPROV          "srvprov"
#define VIEW_CALLMGT          "callmgt"
#define VIEW_STATS            "stats"
#define VIEW_DEBUG            "debug"
#define VIEW_DECT             "dect"

#define HTMLPAGE_BASIC        "voice2_basic.html"
#ifdef VOICE_IMS_SUPPORT
#define HTMLPAGE_SRVPROV      "voice2_srvprov_ims.html"
#else
#define HTMLPAGE_SRVPROV      "voice2_srvprov_rfc3261.html"
#endif
#define HTMLPAGE_CALLMGT      "voice2_callmgt.html"
#define HTMLPAGE_STATS        "voice2_stats.html"
#if defined( DMP_X_BROADCOM_COM_DECTENDPOINT_1 )
#define HTMLPAGE_DECT         "voice2_dect.html"
#endif /* DMP_X_BROADCOM_COM_DECTENDPOINT_1 */
#define HTMLPAGE_DEBUG        "voice2_debug.html"

#define MAX_VIEW_LEN          20
#define TEMP_CHARBUF_SIZE     64
#define MAX_WEBVAR_NAME_LEN   40
#define SLEEP_100mSEC         100*1000
#define SLEEP_300mSEC         300*1000
#define SLEEP_1SEC            1000*1000
#define SLEEP_2SEC            2000*1000
#define SLEEP_3SEC            3500*1000
#define DECT_MAX_WAIT_ITERATION  10

/* ---- Private Variables ------------------------------------------------ */
static int cpInst = -1;
static int vpInst = -1;
static int selectedBasicTab = 0;
static int selectedCallMgtTab = 0;

static CmsRet getVoiceSipImpl(DAL_VOICE_PARMS *parms, char *sipImpl, unsigned int length );
static CmsRet getVoiceDnsImpl(DAL_VOICE_PARMS *parms, char *dnsImpl, unsigned int length );
static CmsRet getVoiceProvImpl(DAL_VOICE_PARMS *parms, char *provImpl, unsigned int length );
static CmsRet getVoiceSipMode(DAL_VOICE_PARMS *parms, char *sipMode, unsigned int length );

static VOICE_PARM_TABLE voiceParmTable[] =
{
   /*                 (unused)                                                                                                                      */
   /*paramName        numArgs definitionBitmap           JSvarname  GET function                                 SET function                       */
   { "mgtProtList",     0,    VOICE_PER_SERVICE_PARAM,     "",      dalVoice_GetMgtProtList,                     NULL                                },
   { "mgtProt",         0,    VOICE_PER_SERVICE_PARAM,     "",      dalVoice_GetManagementProtocol,              dalVoice_SetManagementProtocol      },   
   { "voiceProtocol",   0,    VOICE_PER_SERVICE_PARAM,     "",      dalVoice_GetSignalingProtocol,               NULL                                },
   { "voiceProvImpl",   0,    VOICE_PER_SERVICE_PARAM,     "",      getVoiceProvImpl,                            NULL                                },
   { "voiceSipImpl",    0,    VOICE_PER_SERVICE_PARAM,     "",      getVoiceSipImpl,                             NULL                                },
   { "voiceSipMode",    0,    VOICE_PER_SERVICE_PARAM,     "",      getVoiceSipMode,                             NULL                                },
   { "voiceStatus",     0,    VOICE_PER_SERVICE_PARAM,     "",      dalVoice_GetStatus,                          NULL                                },
   { "dectSupport",     0,    VOICE_PER_SERVICE_PARAM,     "",      dalVoice_GetDectSupport,                     NULL                                },
   { "numFxo",          0,    VOICE_PER_SERVICE_PARAM,     "",      dalVoice_GetNumPhysFxoEndptStr,              NULL                                },
   { "numFxs",          0,    VOICE_PER_SERVICE_PARAM,     "",      dalVoice_GetNumPhysFxsEndptStr,              NULL                                },
   { "ifName",          0,    VOICE_PER_SERVICE_PARAM,     "",      dalVoice_GetBoundIfName,                     dalVoice_SetBoundIfName             },
   { "ifList",          0,    VOICE_PER_SERVICE_PARAM,     "",      dalVoice_GetNetworkIntfList,                 NULL                                },
   { "ipAddrFamily",    0,    VOICE_PER_SERVICE_PARAM,     "",      dalVoice_GetIpFamily,                        dalVoice_SetIpFamily                },
   { "ipFamilyList",    0,    VOICE_PER_SERVICE_PARAM,     "",      dalVoice_GetIpFamilyList,                    NULL                                },
   { "voipProfileList", 0,    VOICE_PER_SERVICE_PARAM,     "",      dalVoice_GetVoipProfileList,                 NULL                                },
   { "codecProfileList",0,    VOICE_PER_SERVICE_PARAM,     "",      dalVoice_GetCodecProfileList,                NULL                                },
   { "localeList",      0,    VOICE_PER_SERVICE_PARAM,     "",      dalVoice_GetRegionSuppString,                NULL                                },
   { "localeName",      0,    VOICE_PER_SERVICE_PARAM,     "",      dalVoice_GetRegion,                          dalVoice_SetRegion                  },
   { "dnsImpl",         0,    VOICE_PER_SERVICE_PARAM,     "",      getVoiceDnsImpl,                             NULL                                },
   { "numSipClients",   0,    VOICE_PER_SERVICE_PARAM,     "",      dalVoice_GetNumSipClientStr,                 NULL                                },
   { "numCcLines",      0,    VOICE_PER_SERVICE_PARAM,     "",      dalVoice_GetNumOfLineStr,                    NULL                                },
   { "numCcExt",        0,    VOICE_PER_SERVICE_PARAM,     "",      dalVoice_GetNumOfExtensionStr,               NULL                                },
   { "numFeatSets",     0,    VOICE_PER_SERVICE_PARAM,     "",      dalVoice_GetNumCallFeatureSetStr,            NULL                                },
   { "transportList",   0,    VOICE_PER_SERVICE_PARAM,     "",      dalVoice_GetSupportedTransports,             NULL                                },
   { "srtpOptionList",  0,    VOICE_PER_SERVICE_PARAM,     "",      dalVoice_GetSupportedSrtpOptions,            NULL                                },
   { "dtmfMethodList",  0,    VOICE_PER_SERVICE_PARAM,     "",      dalVoice_GetSupportedDtmfMethods,            NULL                                },
   { "backToPrimList",  0,    VOICE_PER_SERVICE_PARAM,     "",      dalVoice_GetSupportedBackToPrimOptions,      NULL                                },   
   { "confOptionList",  0,    VOICE_PER_SERVICE_PARAM,     "",      dalVoice_GetSupportedConfOptions,            NULL                                },   
   { "genLogLvl",       0,    VOICE_PER_SERVICE_PARAM,     "",      dalVoice_GetLoggingLevel,                    dalVoice_SetLoggingLevel            },
   { "modLogLvls",      0,    VOICE_PER_SERVICE_PARAM,     "",      dalVoice_GetModuleLoggingLevels,             dalVoice_SetModuleLoggingLevels     },
   { "cctkTraceLvl",    0,    VOICE_PER_SERVICE_PARAM,     "",      dalVoice_GetCCTKTraceLevel,                  dalVoice_SetCCTKTraceLevel          },
   { "cctkTraceGrp",    0,    VOICE_PER_SERVICE_PARAM,     "",      dalVoice_GetCCTKTraceGroup,                  dalVoice_SetCCTKTraceGroup          },
   { "voiceDnsEnable",  0,    VOICE_PER_SERVICE_PARAM,     "",      dalVoice_GetDnsEnable,                       NULL                                },
   { "voiceDnsAddr",    0,    VOICE_PER_SERVICE_PARAM,     "",      dalVoice_GetDnsServerAddr,                   dalVoice_SetDnsServerAddr           },
   { "dialPlan",        0,    VOICE_PER_SERVICE_PARAM,     "",      dalVoice_GetDigitMap,                        dalVoice_SetDigitMap                },
#ifdef EPTAPPLOAD
   { "eptAppStatus",    0,    VOICE_PER_SERVICE_PARAM,     "",      dalVoice_GetEptAppStatus,                    NULL                                },
#endif /* EPTAPPLOAD */

   { "ingressGain",     1,    VOICE_PER_FXS_PARAM,         "ig",    dalVoice_GetVoiceFxsLineTxGainStr,           dalVoice_SetVoiceFxsLineTxGainStr   },
   { "egressGain",      1,    VOICE_PER_FXS_PARAM,         "eg",    dalVoice_GetVoiceFxsLineRxGainStr,           dalVoice_SetVoiceFxsLineRxGainStr   },   

   { "cProfEnabled",    1,    VOICE_PER_CODECPROF_PARAM,   "",      dalVoice_GetCodecProfileEnable,              NULL                                },   
   { "cProfName",       1,    VOICE_PER_CODECPROF_PARAM,   "",      dalVoice_GetCodecProfileName,                NULL                                },   
   { "cProfPtime",      1,    VOICE_PER_CODECPROF_PARAM,   "",      dalVoice_GetCodecProfPacketPeriod,           dalVoice_SetCodecProfPacketPeriod   },   
   { "cProfVadEnabled", 1,    VOICE_PER_CODECPROF_PARAM,   "",      dalVoice_GetCodecProfSilSupp,                dalVoice_SetSilenceSuppression      },   

   { "vProfEnabled",    1,    VOICE_PER_VOIPPROF_PARAM,    "",      dalVoice_GetVoipProfileEnable,               dalVoice_SetVoipProfEnable          },   
   { "vProfName",       1,    VOICE_PER_VOIPPROF_PARAM,    "",      dalVoice_GetVoipProfileName,                 dalVoice_SetVoipProfName            },   
   { "vProfDtmf",       1,    VOICE_PER_VOIPPROF_PARAM,    "",      dalVoice_GetDTMFMethod,                      dalVoice_SetDTMFMethod              },   
   { "vProfRtpMin",     1,    VOICE_PER_VOIPPROF_PARAM,    "",      dalVoice_GetRtpLocalPortMin,                 dalVoice_SetRtpLocalPortMin         },   
   { "vProfRtpMax",     1,    VOICE_PER_VOIPPROF_PARAM,    "",      dalVoice_GetRtpLocalPortMax,                 dalVoice_SetRtpLocalPortMax         },
   { "vProfDscp",       1,    VOICE_PER_VOIPPROF_PARAM,    "",      dalVoice_GetRtpDSCPMark,                     dalVoice_SetRtpDSCPMark             },   
   { "vProfRtcpEn",     1,    VOICE_PER_VOIPPROF_PARAM,    "",      dalVoice_GetRtcpEnabled,                     dalVoice_SetRtcpEnable              },   
   { "vProfRtcpInt",    1,    VOICE_PER_VOIPPROF_PARAM,    "",      dalVoice_GetRtcpInterval,                    dalVoice_SetRtcpInterval            },   
   { "vProfSrtpEn",     1,    VOICE_PER_VOIPPROF_PARAM,    "",      dalVoice_GetSrtpEnabled,                     dalVoice_SetSrtpEnable              },   
   { "vProfSrtpOpt",    1,    VOICE_PER_VOIPPROF_PARAM,    "",      dalVoice_GetSrtpOptionString,                dalVoice_SetSrtpOption              },   
   { "vProfV18En",      1,    VOICE_PER_VOIPPROF_PARAM,    "",      dalVoice_GetV18Enable,                       dalVoice_SetV18Enable               },     

   { "sipDomain",       1,    VOICE_PER_NETWORK_PARAM,     "",      dalVoice_GetSipUserAgentDomain,              dalVoice_SetSipUserAgentDomain      },    
   { "proxyAddr",       1,    VOICE_PER_NETWORK_PARAM,     "",      dalVoice_GetSipProxyServer,                  dalVoice_SetSipProxyServer          },   
   { "proxyPort",       1,    VOICE_PER_NETWORK_PARAM,     "",      dalVoice_GetSipProxyServerPort,              dalVoice_SetSipProxyServerPort      },   
#ifndef VOICE_IMS_SUPPORT
   { "obProxyAddr",     1,    VOICE_PER_NETWORK_PARAM,     "",      dalVoice_GetSipOutboundProxy,                dalVoice_SetSipOutboundProxy        },   
   { "obProxyPort",     1,    VOICE_PER_NETWORK_PARAM,     "",      dalVoice_GetSipOutboundProxyPort,            dalVoice_SetSipOutboundProxyPort    },   
#else
   { "pcscfAddr",       1,    VOICE_PER_NETWORK_PARAM,     "",      dalVoice_GetSipOutboundProxy,                dalVoice_SetSipOutboundProxy        },   
   { "pcscfPort",       1,    VOICE_PER_NETWORK_PARAM,     "",      dalVoice_GetSipOutboundProxyPort,            dalVoice_SetSipOutboundProxyPort    },   	
#endif
   { "regAddr",         1,    VOICE_PER_NETWORK_PARAM,     "",      dalVoice_GetSipRegistrarServer,              dalVoice_SetSipRegistrarServer      },   
   { "regPort",         1,    VOICE_PER_NETWORK_PARAM,     "",      dalVoice_GetSipRegistrarServerPort,          dalVoice_SetSipRegistrarServerPort  },   
   { "prefCodecList",   1,    VOICE_PER_NETWORK_PARAM,     "",      dalVoice_GetSipNetworkCodecList,             dalVoice_SetSipNetworkCodecList     },   
   { "regExp",          1,    VOICE_PER_NETWORK_PARAM,     "",      dalVoice_GetSipRegisterExpires,              dalVoice_SetSipRegisterExpires      },   
   { "regRetry",        1,    VOICE_PER_NETWORK_PARAM,     "",      dalVoice_GetSipRegisterRetryInterval,        dalVoice_SetSipRegisterRetryInterval},   
#ifndef VOICE_IMS_SUPPORT
   { "failoverEnbl",    1,    VOICE_PER_NETWORK_PARAM,     "",      dalVoice_GetSipFailoverEnable,               dalVoice_SetSipFailoverEnable       },
   { "sipOptionsEnbl",  1,    VOICE_PER_NETWORK_PARAM,     "",      dalVoice_GetSipOptionsEnable,                dalVoice_SetSipOptionsEnable        },
   { "secDomain",       1,    VOICE_PER_NETWORK_PARAM,     "",      dalVoice_GetSipSecDomainName,                dalVoice_SetSipSecDomainName        },
   { "secProxyAddr",    1,    VOICE_PER_NETWORK_PARAM,     "",      dalVoice_GetSipSecProxyAddr,                 dalVoice_SetSipSecProxyAddr         },
   { "secProxyPort",    1,    VOICE_PER_NETWORK_PARAM,     "",      dalVoice_GetSipSecProxyPort,                 dalVoice_SetSipSecProxyPort         },
   { "secObProxyAddr",  1,    VOICE_PER_NETWORK_PARAM,     "",      dalVoice_GetSipSecOutboundProxyAddr,         dalVoice_SetSipSecOutboundProxyAddr },
   { "secObProxyPort",  1,    VOICE_PER_NETWORK_PARAM,     "",      dalVoice_GetSipSecOutboundProxyPort,         dalVoice_SetSipSecOutboundProxyPort },
   { "secRegistrarAddr",1,    VOICE_PER_NETWORK_PARAM,     "",      dalVoice_GetSipSecRegistrarAddr,             dalVoice_SetSipSecRegistrarAddr     },
   { "secRegistrarPort",1,    VOICE_PER_NETWORK_PARAM,     "",      dalVoice_GetSipSecRegistrarPort,             dalVoice_SetSipSecRegistrarPort     },
   { "backToPrimMode",  1,    VOICE_PER_NETWORK_PARAM,     "",      dalVoice_GetSipBackToPrimOption,             dalVoice_SetSipBackToPrimOption     },   
   { "transportOption", 1,    VOICE_PER_NETWORK_PARAM,     "",      dalVoice_GetSipTransportString,              dalVoice_SetSipTransport            },   
#endif
   { "confOption",      1,    VOICE_PER_NETWORK_PARAM,     "",      dalVoice_GetSipConferencingOption,           dalVoice_SetSipConferencingOption   },   
   { "confUri",         1,    VOICE_PER_NETWORK_PARAM,     "",      dalVoice_GetSipConferencingURI,              dalVoice_SetSipConferencingURI      },   
 
   { "accntEnable",     1,    VOICE_PER_CLIENT_PARAM,      "aen",   dalVoice_GetVlEnable,                        dalVoice_SetVlEnable                },   
   { "accntStatus",     1,    VOICE_PER_CLIENT_PARAM,      "ast",   dalVoice_GetSipClientStatus,                 NULL                                },
   { "accntUsername",   1,    VOICE_PER_CLIENT_PARAM,      "un",    dalVoice_GetVlSipURI,                        dalVoice_SetVlSipURI                },   
   { "accntDispName",   1,    VOICE_PER_CLIENT_PARAM,      "dn",    dalVoice_GetVlCFCallerIDName,                dalVoice_SetVlCFCallerIDName        },   
   { "accntAuthName",   1,    VOICE_PER_CLIENT_PARAM,      "an",    dalVoice_GetVlSipAuthUserName,               dalVoice_SetVlSipAuthUserName       },   
   { "accntPassword",   1,    VOICE_PER_CLIENT_PARAM,      "pw",    dalVoice_GetVlSipAuthPassword,               dalVoice_SetVlSipAuthPassword       },   
   { "t38Enable",       1,    VOICE_PER_CLIENT_PARAM,      "t38en", dalVoice_GetT38Enable,                       dalVoice_SetT38Enable               },

   { "rtpPacketSent",   1,    VOICE_PER_CCLINE_PARAM,      "rs",    dalVoice_GetCcLineStatsRtpPacketSentString,  NULL                                },   
   { "rtpPacketRecv",   1,    VOICE_PER_CCLINE_PARAM,      "rr",    dalVoice_GetCcLineStatsRtpPacketRecvString,  NULL                                },
   { "rtpPacketLost",   1,    VOICE_PER_CCLINE_PARAM,      "rl",    dalVoice_GetCcLineStatsRtpPacketLostString,  NULL                                },
   { "totalBytesRecv",  1,    VOICE_PER_CCLINE_PARAM,      "br",    dalVoice_GetCcLineStatsRtpBytesRecvString,   NULL                                },
   { "totalBytesSent",  1,    VOICE_PER_CCLINE_PARAM,      "bs",    dalVoice_GetCcLineStatsRtpBytesSentString,   NULL                                },

   { "incCallReceived", 1,    VOICE_PER_CCLINE_PARAM,      "icr",   dalVoice_GetCcLineStatsInCallRecvString,     NULL                                },
   { "incCallConnected",1,    VOICE_PER_CCLINE_PARAM,      "icc",   dalVoice_GetCcLineStatsInCallConnString,     NULL                                },
   { "incCallFailed",   1,    VOICE_PER_CCLINE_PARAM,      "icf",   dalVoice_GetCcLineStatsInCallFailedString,   NULL                                },
   { "incCallDropped",  1,    VOICE_PER_CCLINE_PARAM,      "icd",   dalVoice_GetCcLineStatsInCallDropString,     NULL                                },

   { "outCallAttempted",1,    VOICE_PER_CCLINE_PARAM,      "oca",   dalVoice_GetCcLineStatsOutCallAttemptString, NULL                                },
   { "outCallConnected",1,    VOICE_PER_CCLINE_PARAM,      "occ",   dalVoice_GetCcLineStatsOutCallConnString,    NULL                                },
   { "outCallFailed",   1,    VOICE_PER_CCLINE_PARAM,      "ocf",   dalVoice_GetCcLineStatsOutCallFailedString,  NULL                                },
   { "outCallDropped",  1,    VOICE_PER_CCLINE_PARAM,      "ocd",   dalVoice_GetCcLineStatsOutCallDropString,    NULL                                },
   
   { "clidEnable",      1,    VOICE_PER_FEATURESET_PARAM,  "clen",  dalVoice_GetVlCFCallId,                      dalVoice_SetVlCFCallId              },   
   { "clidNameEnable",  1,    VOICE_PER_FEATURESET_PARAM,  "clnen", dalVoice_GetVlCFCallIdName,                  dalVoice_SetVlCFCallIdName          },   
   { "callWait",        1,    VOICE_PER_FEATURESET_PARAM,  "cw",    dalVoice_GetVlCFCallWaiting,                 dalVoice_SetVlCFCallWaiting         },   
   { "callFwdAll",      1,    VOICE_PER_FEATURESET_PARAM,  "cfa",   dalVoice_GetVlCFCallFwdAll,                  dalVoice_SetVlCFCallFwdAll          },   
   { "callFwdNum",      1,    VOICE_PER_FEATURESET_PARAM,  "cfn",   dalVoice_GetVlCFCallFwdNum,                  dalVoice_SetVlCFCallFwdNum          },   
   { "callFwdBusy",     1,    VOICE_PER_FEATURESET_PARAM,  "cfb",   dalVoice_GetVlCFCallFwdBusy,                 dalVoice_SetVlCFCallFwdBusy         },   
   { "callFwdNoAns",    1,    VOICE_PER_FEATURESET_PARAM,  "cfna",  dalVoice_GetVlCFCallFwdNoAns,                dalVoice_SetVlCFCallFwdNoAns        },   
   { "callBarringEn",   1,    VOICE_PER_FEATURESET_PARAM,  "cb",    dalVoice_GetVlCFCallBarring,                 dalVoice_SetVlCFCallBarring         },   
   { "callBarringPin",  1,    VOICE_PER_FEATURESET_PARAM,  "cbpin", dalVoice_GetVlCFCallBarringPin,              dalVoice_SetVlCFCallBarringPin      },   
   { "callBarringMap",  1,    VOICE_PER_FEATURESET_PARAM,  "cbmap", dalVoice_GetVlCFCallBarringDigitMap,         dalVoice_SetVlCFCallBarringDigitMap },   
   { "mwiEnable",       1,    VOICE_PER_FEATURESET_PARAM,  "mwi",   dalVoice_GetVlCFMWIEnable,                   dalVoice_SetVlCFMWIEnable           },   
   { "callXfer",        1,    VOICE_PER_FEATURESET_PARAM,  "cx",    dalVoice_GetVlCFCallTransfer,                dalVoice_SetVlCFCallTransfer        },   
   { "doNotDisturb",    1,    VOICE_PER_FEATURESET_PARAM,  "dnd",   dalVoice_GetVlCFDoNotDisturb,                dalVoice_SetVlCFDoNotDisturb        },   
   { "anonCall",        1,    VOICE_PER_FEATURESET_PARAM,  "ac",    dalVoice_GetVlCFAnonymousCalling,            dalVoice_SetVlCFAnonymousCalling    },   
   { "anonCallBlock",   1,    VOICE_PER_FEATURESET_PARAM,  "acb",   dalVoice_GetVlCFAnonCallBlck,                dalVoice_SetVlCFAnonCallBlck        },   
   { "warmline",        1,    VOICE_PER_FEATURESET_PARAM,  "wlen",  dalVoice_GetVlCFWarmLine,                    dalVoice_SetVlCFWarmLine            },   
   { "callReturn",      1,    VOICE_PER_FEATURESET_PARAM,  "crn",   dalVoice_GetVlCFCallReturn,                  dalVoice_SetVlCFCallReturn          },   
   { "callRedial",      1,    VOICE_PER_FEATURESET_PARAM,  "crd",   dalVoice_GetVlCFCallRedial,                  dalVoice_SetVlCFCallRedial          },   
   { "warmlineNum",     1,    VOICE_PER_FEATURESET_PARAM,  "wln",   dalVoice_GetVlCFWarmLineNum,                 dalVoice_SetVlCFWarmLineNum         },   
   { "euFlashEn",       0,    VOICE_PER_SERVICE_PARAM,     "",      dalVoice_GetEuroFlashEnable,                 dalVoice_SetEuroFlashEnable         },

   { "routingMap",      2,    VOICE_ROUTING_PARAM,         "rm",    NULL,                                        NULL                                },   

   { "providerMap",     2,    VOICE_PROVIDER_PARAM,        "pm",    NULL,                                        NULL                                },   
   
   { "ntrEnabled",      0,    VOICE_PER_SERVICE_PARAM,     "",      dalVoice_GetNtrEnable,                       NULL                                },
   { NULL,              0,    0,                           "",      NULL,                                        NULL                                }
};

static char *cctk_upload = NULL;
static int   upload_size = 0;

/* ---- Private Function Prototypes -------------------------------------- */
static void setMdmParameters( char* query );
static void reDisplayPage( char* query, FILE* fs );

CmsRet dectStart( void );
CmsRet dectStop( void );

char hexToDecimal(char c);
unsigned char hexCharToChar (char * str );

/* ---- Function implementations ----------------------------------------- */


/****************************************************************************
* FUNCTION:    cgiGetVarVoice
*
* PURPOSE:     Get the value of a voice variable
*
* PARAMETERS:  varName  [IN]  -  voice variable string
*              varValue [OUT] -  value of the voice variable indicated by
*                                the voice variable string
*
* RETURNS:
*
* NOTE:        The function assumes that memory for varValue is allocated by the caller.
*              Function needs to change when multiple voice profiles are supported
****************************************************************************/
void cgiGetVarVoice(char *varName, char *varValue)
{
   void* varObj = NULL;
   UBOOL8 varFound = FALSE;
   DAL_VOICE_PARMS parms;
   
   char tmpStr[WEB_BUF_SIZE_MAX];
   
   int i, numFxs, numSipClients, numMap, numFeatureSets, numCcLines;
   int tmpInst, vsInst, assocType;
   char line[LINE_EXT_BUF_SIZE_MAX], ext[LINE_EXT_BUF_SIZE_MAX];
       
   if (CMSRET_SUCCESS != dalVoice_mapSpNumToSvcInst( 0, &vsInst ))
   {
      return;
   }
   
   parms.op[0] = vsInst;
   parms.op[1] = 0;
   parms.msgHandle = msgHandle;

   VOICE_PARM_TABLE* pVoiceParmTable = voiceParmTable;
   
   /* Allocate memory for the object to be obtained through the DAL layer. */
   varObj = malloc(WEB_BUF_SIZE_MAX);
   if ( varObj == NULL )
   {
      cmsLog_error( "Could not get allocate memory for object for variable %s. \n", varName );
      return;
   }
   memset(varObj, 0, WEB_BUF_SIZE_MAX);

   if( strncmp ( varName, "voiceProtocol", strlen(varName)) == 0){
      varFound = TRUE;
      snprintf(varValue,  WEB_BUF_SIZE_MAX, "SIP");
   }
   else if( strncmp ( varName, "voiceSipImpl", strlen(varName)) == 0){
      varFound = TRUE;
      snprintf(varValue,  WEB_BUF_SIZE_MAX, "1");
   }
   else if( strncmp ( varName, "voiceProvImpl", strlen(varName)) == 0){
      varFound = TRUE;
      snprintf(varValue,  WEB_BUF_SIZE_MAX, "2");
   }
   else if( strncmp ( varName, "eptAppStatus", strlen(varName)) == 0){
      varFound = TRUE;
      snprintf(varValue,  WEB_BUF_SIZE_MAX, "0");
   }
   else if( strncmp ( varName, "selectedBasicTab", strlen(varName)) == 0){
      varFound = TRUE;
      snprintf(varValue,  WEB_BUF_SIZE_MAX, "%d", selectedBasicTab);
   }
   else if( strncmp ( varName, "selectedCallMgtTab", strlen(varName)) == 0){
      varFound = TRUE;
      snprintf(varValue,  WEB_BUF_SIZE_MAX, "%d", selectedCallMgtTab);
   }

   if (varFound == TRUE)
   {
	  return;
   }
   
   /* Compare the passed web variable with the one in the SIP parameters table.
   ** If match is found, get the value for that variable through the DAL layer */
   while ( pVoiceParmTable->webVarName != NULL )
   {
      if ( strcmp( varName, pVoiceParmTable->webVarName ) == 0 )
      {
         if ( pVoiceParmTable->dalVoice_getFunc != NULL )
		 {
            if(pVoiceParmTable->parmDefinitionBitmap & VOICE_PER_FXS_PARAM)
			{
               dalVoice_GetNumPhysFxsEndpt( &numFxs );
			   
			   for (i = 0; i < numFxs; i++)
               {
				   parms.op[1] = i;
  
			       if (CMSRET_SUCCESS != dalVoice_mapPotsFxsNumToInst(&parms, &tmpInst))
                   {
                      continue;
                   }
				   
                   parms.op[1] = tmpInst;
				   
                   if ( pVoiceParmTable->dalVoice_getFunc( &parms, varObj, WEB_BUF_SIZE_MAX) == CMSRET_SUCCESS )
                   {
			          sprintf(tmpStr, "%s%d = \"%s\";", pVoiceParmTable->perLineJsVarString, i+1, (char*)varObj );
                      strncat(tmpStr, " ", WEB_BUF_SIZE_MAX);
					  varFound = TRUE;
					  strncat(varValue, tmpStr, WEB_BUF_SIZE_MAX);
                   }
				   else
                   {
					  cmsLog_error( "Failed to obtain voice parameter %s\n", varName);
				   }
			   }
			   cmsLog_debug( "voice parameter %s, varValue %s\n", varName, varValue);
			}
            else if(pVoiceParmTable->parmDefinitionBitmap & VOICE_PER_CODECPROF_PARAM)
			{
               if(cpInst == -1)
               {
                   varFound = TRUE;
				   strcpy(varValue, "");
			   }
			   else
               {
                  parms.op[1] = cpInst;
                  if ( pVoiceParmTable->dalVoice_getFunc( &parms, varObj, WEB_BUF_SIZE_MAX) == CMSRET_SUCCESS )
                  {
                     varFound = TRUE;
                  }
                  else
                  {
                     cmsLog_error( "Failed to obtain voice parameter %s\n", varName);
			      }
                  strcpy(varValue, varObj);
               }	  
			   cmsLog_debug( "voice parameter %s, varValue %s\n", varName, varValue);
			}
            else if(pVoiceParmTable->parmDefinitionBitmap & VOICE_PER_VOIPPROF_PARAM)
			{
               if(vpInst == -1)
               {
                   varFound = TRUE;
				   strcpy(varValue, "");
			   }
			   else
               {
                  parms.op[1] = vpInst;
                  if ( pVoiceParmTable->dalVoice_getFunc( &parms, varObj, WEB_BUF_SIZE_MAX) == CMSRET_SUCCESS )
                  {
                     varFound = TRUE;
                  }
                  else
                  {
                     cmsLog_error( "Failed to obtain voice parameter %s\n", varName);
			      }
                  strcpy(varValue, varObj);
               }	  
			   cmsLog_debug( "voice parameter %s, varValue %s\n", varName, varValue);
			}
            else if(pVoiceParmTable->parmDefinitionBitmap & VOICE_PER_NETWORK_PARAM)
			{
			   parms.op[1] = 0; /* Only one network for now */
			   if( dalVoice_mapNetworkNumToInst( &parms , &tmpInst ) != CMSRET_SUCCESS) 
               {				   
			      cmsLog_debug("Could not map network 0");
				  continue;
               }
			
		       parms.op[1] = tmpInst;
			
               if ( pVoiceParmTable->dalVoice_getFunc( &parms, varObj, WEB_BUF_SIZE_MAX) == CMSRET_SUCCESS )
               {
                  varFound = TRUE;
               }
               else
               {
                  cmsLog_error( "Failed to obtain voice parameter %s\n", varName);
			   }
			   strncpy( varValue, (char*)varObj, WEB_BUF_SIZE_MAX );
			   cmsLog_debug( "voice parameter %s, varValue %s\n", varName, varValue);
			}
            else if(pVoiceParmTable->parmDefinitionBitmap & VOICE_PER_CLIENT_PARAM)
			{
               dalVoice_GetNumSipClient( &parms, &numSipClients );
			   
			   for (i = 0; i < numSipClients; i++)
               {
				   parms.op[1] = i;
  
			       if (CMSRET_SUCCESS != dalVoice_mapAcntNumToClientInst(&parms, &tmpInst))
                   {
                      cmsLog_error( "Failed to map num %d to inst\n", i);
					  continue;
                   }
				   
                   parms.op[1] = tmpInst;
				   
                   if ( pVoiceParmTable->dalVoice_getFunc( &parms, varObj, WEB_BUF_SIZE_MAX) == CMSRET_SUCCESS )
                   {
			          sprintf(tmpStr, "%s%d = \"%s\";", pVoiceParmTable->perLineJsVarString, i+1, (char*)varObj );
                      strncat(tmpStr, " ", WEB_BUF_SIZE_MAX);
					  strncat(varValue, tmpStr, WEB_BUF_SIZE_MAX);
					  varFound = TRUE;
                   }
				   else
                   {
					  cmsLog_error( "Failed to obtain voice parameter %s\n", varName);
				   }
			   }

			   cmsLog_debug( "voice parameter %s, varValue %s\n", varName, varValue);
			}
            else if(pVoiceParmTable->parmDefinitionBitmap & VOICE_PER_CCLINE_PARAM)
			{
               /* 1:1 mapping between SIP clients and CC lines */
               dalVoice_GetNumOfLine( &parms, &numCcLines ); 
			   
			   for (i = 0; i < numCcLines; i++)
               {
			       if (CMSRET_SUCCESS != dalVoice_mapAcntNumToLineInst(vsInst, i, &tmpInst))
                   {
                      cmsLog_error( "Failed to map num %d to inst\n", i);
					  continue;
                   }
				   
                   parms.op[1] = tmpInst;
				   
                   if ( pVoiceParmTable->dalVoice_getFunc( &parms, varObj, WEB_BUF_SIZE_MAX) == CMSRET_SUCCESS )
                   {
			          sprintf(tmpStr, "%s%d = \"%s\";", pVoiceParmTable->perLineJsVarString, i+1, (char*)varObj );
                      strncat(tmpStr, " ", WEB_BUF_SIZE_MAX);
					  strncat(varValue, tmpStr, WEB_BUF_SIZE_MAX);
					  varFound = TRUE;
                   }
				   else
                   {
					  cmsLog_error( "Failed to obtain voice parameter %s\n", varName);
				   }
			   }

			   cmsLog_debug( "voice parameter %s, varValue %s\n", varName, varValue);
			}
			else if(pVoiceParmTable->parmDefinitionBitmap & VOICE_PER_FEATURESET_PARAM)
			{
               dalVoice_GetNumSipClient( &parms, &numFeatureSets );
			   
			   for (i = 0; i < numFeatureSets; i++)
               {
				   parms.op[1] = i;
  
			       if (CMSRET_SUCCESS != dalVoice_mapCallFeatureSetNumToInst(&parms, &tmpInst))
                   {
                      cmsLog_error( "Failed to map num %d to inst\n", i);
					  continue;
                   }
				   
                   parms.op[1] = tmpInst;
				   
                   if ( pVoiceParmTable->dalVoice_getFunc( &parms, varObj, WEB_BUF_SIZE_MAX) == CMSRET_SUCCESS )
                   {
			          sprintf(tmpStr, "%s%d = \"%s\";", pVoiceParmTable->perLineJsVarString, i+1, (char*)varObj );
                      strncat(tmpStr, " ", WEB_BUF_SIZE_MAX);
					  strncat(varValue, tmpStr, WEB_BUF_SIZE_MAX);
					  varFound = TRUE;
                   }
				   else
                   {
					  cmsLog_error( "Failed to obtain voice parameter %s\n", varName);
				   }
			   }

			   cmsLog_debug( "voice parameter %s, varValue %s\n", varName, varValue);
			}
            else
            {
               if ( pVoiceParmTable->dalVoice_getFunc( &parms, varObj, WEB_BUF_SIZE_MAX) == CMSRET_SUCCESS )
               {
                  cmsLog_debug( "DAL GET for variable %s = %s.\n", varName, varObj );
			      varFound = TRUE;
			      strncpy( varValue, (char*)varObj, WEB_BUF_SIZE_MAX );
			      break;
               }
               else
               {
                  cmsLog_debug( "ERROR: Could not get object for variable %s \n", varName);
	           }
			}
         }
		 else
         {
			/* DAL Get function is NULL. Processing only for special cases such as routing/provider maps */
            if(pVoiceParmTable->parmDefinitionBitmap & VOICE_ROUTING_PARAM)
			{
               dalVoice_GetNumIncomingMap( &parms, &numMap );
			   for (i = 0; i < numMap; i++)
               {
				   parms.op[1] = i;
  
			       if (CMSRET_SUCCESS != dalVoice_mapIncomingMapNumToInst(&parms, &tmpInst))
                   {
					  cmsLog_error( "could not map %d to inst\n", i);
                      continue;
                   }
				   
                   parms.op[1] = tmpInst;
				   if ( (CMSRET_SUCCESS != dalVoice_GetIncomingMapEnable(&parms, tmpStr, WEB_BUF_SIZE_MAX) ) 
				      || (strcmp(tmpStr, "Yes")) )
                   {
					  cmsLog_error( "map %d disabled\n", i, tmpStr);
                      continue;
                   }
				   
				   varFound = TRUE;
				   dalVoice_GetIncomingMapLineNum(&parms, line, LINE_EXT_BUF_SIZE_MAX);
				   dalVoice_GetIncomingMapExtNum(&parms, ext, LINE_EXT_BUF_SIZE_MAX);
			          
				   sprintf(tmpStr, "rm%s_%s = \"true\";", line, ext );
				   strncat(varValue, tmpStr, WEB_BUF_SIZE_MAX);				   
			   }
			   cmsLog_debug( "voice parameter %s, varValue %s\n", varName, varValue);
			}
			else if(pVoiceParmTable->parmDefinitionBitmap & VOICE_PROVIDER_PARAM)
			{
               dalVoice_GetNumOfLine( &parms, &numCcLines );

			   for (i = 0; i < numCcLines; i++)
               {
                  if( dalVoice_mapAcntNumToLineInst( parms.op[0], i, &tmpInst ) == CMSRET_SUCCESS)
                  {
                     parms.op[1] = tmpInst;
					 if (dalVoice_GetCallCtrlLineAssocType(&parms, &assocType) == CMSRET_SUCCESS)
                     {
						if (assocType == TERMTYPE_FXO)
						{
                           sprintf(tmpStr, "prov%d = \"FXO\"; ", i );
				           strncat(varValue, tmpStr, WEB_BUF_SIZE_MAX);
                        }
                        else /* we are dealing with TERMTYPE_SIP here */
						{
                           sprintf(tmpStr, "prov%d = \"SIP\"; ", i );
				           strncat(varValue, tmpStr, WEB_BUF_SIZE_MAX);
                        }						
					 }
                  }			   
			   }
			   
			   varFound = TRUE;
			   
			   cmsLog_debug( "voice parameter %s, varValue %s\n", varName, varValue);
			}
		 }
      }

      /* Reset varObj */
      *(char*)varObj = '\0';

      pVoiceParmTable++;
   }

   free( varObj );

   /* Report error if the requested variable was not found in the table */
   if ( !varFound )
   {
      cmsLog_error("Var %s NOT found in the var table. \n", varName );
   }
}
#if DMP_X_BROADCOM_COM_DECTENDPOINT_1

void cgiDectStart( char *query, FILE *fs )
{
  /* Start dect app */
   dectStart();

   reDisplayPage( query, fs );
}

CmsRet dectStart( void )
{
   CmsRet          ret = CMSRET_SUCCESS;
   CmsMsgHeader    *msg;
   void            *msgBuf;
   int             i;
   DAL_VOICE_PARMS parms;
   char            result[TEMP_CHARBUF_SIZE];

   memset(&parms, 0, sizeof(parms));
   memset(&result, 0, sizeof(result));
   parms.op[0] = 1;

   if ( (msgBuf = cmsMem_alloc( sizeof(CmsMsgHeader),
                                ALLOC_ZEROIZE)) == NULL)
   {
      cmsLog_error("%s(): msg send failed, can't allocate memory ", __FUNCTION__);
      return CMSRET_INTERNAL_ERROR;
   }

   /* prepared message header and body */
   msg  = (CmsMsgHeader *) msgBuf;

   /* Fill in request message */
   msg->type = CMS_MSG_VOICE_DECT_START;
   msg->src = cmsMsg_getHandleEid(msgHandle);
   msg->dst = EID_DECT;
   msg->flags_request = 1;
   msg->flags_bounceIfNotRunning = 0;
   msg->dataLength = 0;

   /* Release lock so that dect msg could be handled */
   cmsLck_releaseLock();

   if(( ret = cmsMsg_send(msgHandle, msg)) != CMSRET_SUCCESS )
   {
      cmsLog_error("%s() msg send failed, ret=%d",__FUNCTION__, ret);
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(msgBuf);

   /* Sleep a bit until dect app starts */
   for( i=0; i<DECT_MAX_WAIT_ITERATION; i++ )
   {
      cmsLck_acquireLock();

      if( dalVoice_GetDectStatus(&parms, result, TEMP_CHARBUF_SIZE) ==  CMSRET_SUCCESS )
      {
         /* Check if status is enabled, if not then wait */
         if( !cmsUtl_strcasecmp(result, MDMVS_ENABLED) )
         {
            cmsLck_releaseLock();
            break;
         }
         else
         {
            cmsLck_releaseLock();
            usleep( SLEEP_100mSEC  );
         }
      }
      else
      {
         cmsLog_error("%s(): DECT Start cannot get status! ", __FUNCTION__);
         cmsLck_releaseLock();
         break;
      }
   }

   if( i == DECT_MAX_WAIT_ITERATION )
   {
      cmsLog_error("%s(): DECT Start timed out! ", __FUNCTION__);
   }

   /* Re-aquire lock for further httpd processing */
   cmsLck_acquireLock();

   return ret;
}

void cgiDectStop( char *query, FILE *fs )
{
   /* Stop dect app */
   dectStop();

   reDisplayPage( query, fs );
}

CmsRet dectStop( void )
{
   CmsRet          ret = CMSRET_SUCCESS;
   CmsMsgHeader    *msg;
   void            *msgBuf;
   int             i;
   DAL_VOICE_PARMS parms;
   char            result[TEMP_CHARBUF_SIZE];

   memset(&parms, 0, sizeof(parms));
   memset(&result, 0, sizeof(result));
   parms.op[0] = 1;

   if ( (msgBuf = cmsMem_alloc( sizeof(CmsMsgHeader),
                                ALLOC_ZEROIZE)) == NULL)
   {
      cmsLog_error("%s(): msg send failed, can't allocate memory ", __FUNCTION__);
      return CMSRET_INTERNAL_ERROR;
   }

   /* prepared message header and body */
   msg  = (CmsMsgHeader *) msgBuf;

   /* Fill in request message */
   msg->type = CMS_MSG_VOICE_DECT_STOP;
   msg->src = cmsMsg_getHandleEid(msgHandle);
   msg->dst = EID_DECT;
   msg->flags_request = 1;
   msg->flags_bounceIfNotRunning = 1;
   msg->dataLength = 0;

   /* Release lock so that dect msg could be handled */
   cmsLck_releaseLock();

   if(( ret = cmsMsg_sendAndGetReplyWithTimeout(msgHandle, msg, 1000)) != CMSRET_SUCCESS )
   {
      cmsLog_error("%s() msg send failed, ret=%d",__FUNCTION__, ret);
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(msgBuf);

   /* Sleep a bit until dect app has stopped */
   for( i=0; i<DECT_MAX_WAIT_ITERATION; i++ )
   {
      cmsLck_acquireLock();

      if( dalVoice_GetDectStatus(&parms, result, TEMP_CHARBUF_SIZE) ==  CMSRET_SUCCESS )
      {
         /* Check if status is disabled, if not then wait */
         if( !cmsUtl_strcasecmp(result, MDMVS_DISABLED) )
         {
            cmsLck_releaseLock();
            break;
         }
         else
         {
            cmsLck_releaseLock();
            usleep( SLEEP_100mSEC  );
         }
      }
      else
      {
         cmsLog_error("%s(): DECT Start cannot get status! ", __FUNCTION__);
         cmsLck_releaseLock();
         break;
      }
   }

   if( i == DECT_MAX_WAIT_ITERATION )
   {
      cmsLog_error("%s(): DECT Stop timed out! ", __FUNCTION__);
   }

   /* Re-aquire lock for further httpd processing */
   cmsLck_acquireLock();

   return ret;
}

/* Converts hex char into char.
** Example: "ff" into ff
*/
unsigned char hexCharToChar( char * str )
{
   int first = hexToDecimal(*str);
   int second = hexToDecimal(*(str + 1));

   first <<= 4;

   return( first | second);
}

/* Converts hex to decimal */
char hexToDecimal(char c)
{
   if ( c>= '0' && c<= '9')
   {
      return(c - '0');
   }
   if (c>= 'a' && c<= 'f')
   {
      return(c - 'a' + 10);
   }
   if (c>= 'A' && c<= 'F')
   {
      return(c - 'A' + 10);
   }
   return(0);//not hex
}

void cgiDectSetRFID( char *query, FILE *fs )
{
   CmsRet          ret = CMSRET_SUCCESS;
   CmsMsgHeader    *msg;
   char            *data;
   void            *msgBuf;
   VoiceDiagMsgBody* info;

   char newWebVarName[MAX_WEBVAR_NAME_LEN];
   void * webVarValue = NULL;
   CGI_STATUS cgiStatus;

   int i = 0;
   int j = 0;

   webVarValue = cmsMem_alloc( WEB_BUF_SIZE_MAX, ALLOC_ZEROIZE );

   sprintf( newWebVarName, "dectId" );

   /* webVarValue contains 5 bytes */
   if( (cgiStatus = cgiGetValueByNameSafe( query,
                                           newWebVarName,
                                           NULL,
                                           (char*)webVarValue,
                                           WEB_BUF_SIZE_MAX )) == CGI_STS_OK )
   {
      if ( (msgBuf = cmsMem_alloc( sizeof(CmsMsgHeader) +
                                   (sizeof(VoiceDiagMsgBody)),
                                   ALLOC_ZEROIZE)) == NULL)
      {
         cmsLog_error("%s(): msg send failed, can't allocate memory ", __FUNCTION__);
         return;
      }

      msg  = (CmsMsgHeader *) msgBuf;
      data = (char *) (msg + 1);
      info = (VoiceDiagMsgBody *)data;

      /* Fill in request message */
      info->type  =  VOICE_DIAG_DECT_MEM_SET;

      msg->type = CMS_MSG_VOICE_DECT_MEM_SET;
      msg->src = cmsMsg_getHandleEid(msgHandle);
      msg->dst = EID_DECT;
      msg->flags_request = 1;
      msg->flags_bounceIfNotRunning = 1;
      msg->dataLength = sizeof(VoiceDiagMsgBody);

      /* The cmdLine that we need to pass in the message contains two arguments
      ** an address and a byte value. The RFPI is 5 bytes long and starts at address 0
      ** therefore the address rangs is 0 - 4, whereas the number of characters needed
      ** to represent the 5byte RFPI are 10
      */

      /* Javasript ensure's first case will be 0, and that it'll be 10 hex in length */
      for (i = 0; i<10; i+=2)
      {
         /* Seperate hex char into individual char and pass into set parm */
         sprintf(info->cmdLine, "%d %d", j, (int)hexCharToChar( (char*)webVarValue+i ) );

         /* Send message */
         if ((ret = cmsMsg_send( msgHandle, msg )) != CMSRET_SUCCESS)
         {
            cmsLog_error("%s() msg send failed, ret=%d",__FUNCTION__, ret);
         }
         /* Increment address */
         j++;
      }

      /* Release lock so that dect msg could be handled */
      cmsLck_releaseLock();

      /* Sleep a bit before stopping dect */
      usleep( SLEEP_1SEC  );

      /* Re-aquire lock for further httpd processing */
      cmsLck_acquireLock();

      /* Stop dect app */
      dectStop();

      /* Start dect app */
      dectStart();

      CMSMEM_FREE_BUF_AND_NULL_PTR(msgBuf);

   }

   else
   {
      /* Error */
   }

   cmsMem_free( webVarValue );

   usleep( SLEEP_100mSEC  );

   reDisplayPage( query, fs );

}
#endif /* DMP_X_BROADCOM_COM_DECTENDPOINT_1*/

int cgiVoiceCctkDigitMapUpload(FILE *fs, int cl)
{
   int ret = WEB_STS_OK, boundSize = 0;
   char buf[BUFLEN_1024]={0}, bound[BUFLEN_256]={0};
   char *pdest = NULL;
   UBOOL8 isTerminated = FALSE;

   cmsLog_debug("%s() #####", __FUNCTION__);
   // Read the first UPLOAD_BOUND_SIZE(256) bytes of the image.
   httpd_gets(bound, sizeof(bound));

   boundSize = cmsMem_strnlen(bound, sizeof(bound), &isTerminated) - 2;
   if (!isTerminated || boundSize <= 0)
   {
      cmsLog_error("Failed to find bound within the first %d bytes", sizeof(bound));
      return WEB_STS_ERROR;
   }
   bound[boundSize] = '\0';

   // get Content-Disposition: form-data; name="filename"; filename="test"
   // check filename, if "", user click no filename and report
   httpd_gets(buf, sizeof(buf));

   if ((pdest = strstr(buf, "filename=")) == NULL)
   {
      cmsLog_error("could not find filename within the first %d bytes", sizeof(buf));
      cgiWriteMessagePage(fs, "Message", "Invalid file name, please try again", 0);
      return WEB_STS_ERROR;
   }
   pdest += (strlen("filename=") - 1);
   if (*pdest == '"' && *(pdest + 1) == '"')
   {
      cmsLog_error("No filename selected");
      cgiWriteMessagePage(fs, "Message", "Invalid file name, please try again", 0);
      return WEB_STS_ERROR;
   }
   else
   {
      cmsLog_debug("filename %s", pdest);
   }

   // get [Content-Type: text/plain] and NL (cr/lf) and discard them
   httpd_gets(buf, sizeof(buf));
   if ((pdest = strstr(buf, "text/plain")) == NULL)
   {
      cmsLog_error("cctk digit map should be plain text format");
      cgiWriteMessagePage(fs, "Message", "Invalid format, must be plain text file, please try again", 0);
      return WEB_STS_ERROR;
   }

   httpd_gets(buf, sizeof(buf));

   cctk_upload = (char *)cmsMem_alloc( cl+1, ALLOC_ZEROIZE );
   if ( cctk_upload == NULL )
   {
      cmsLog_error( "cgiVoiceCctkDigitMapUpload: could not allocate memory. \n");
      return WEB_STS_ERROR;
   }
   else
   {
      upload_size = cl;

      // read the CCTK digitmap from file and write them to memory
      while (!feof(fs))
      {
         // initialize line buffer
         memset(buf, 0, BUFLEN_1024);

         if (httpd_gets(buf, BUFLEN_1024) == TRUE)
         {
            // if it reaches the last upload bound then stop
            if (strstr(buf, "-----") != NULL)
               break;

            if(strlen(cctk_upload)+strlen(buf) <= cl)
            {
               strncat(cctk_upload, buf, strlen(buf));
            }
            else
            {
               cmsLog_error("The digit map size is over the content length %d, trunked", cl);
            }
         }
      }

      // read pass the last upload bound
      while (httpd_gets(buf, BUFLEN_1024) == TRUE);
   }

   return ret;
}


void cgiVoiceStart(char *query, FILE *fs)
{
   char  basicTabName[MAX_WEBVAR_NAME_LEN] = "currentBasicTab";
   char  callMgtTabName[MAX_WEBVAR_NAME_LEN] = "currentCallMgtTab";   
   char  webVarValue[TEMP_CHARBUF_SIZE];

   memset((char *)webVarValue, 0, sizeof(webVarValue));
   /*
    * Protect against cross site forgery attacks
    * Note: this check is skipped for voice in do_cgi_cmd() as voice
    * commands does not have an action parameter to differentiate
    * between View/Set.
    */
   if (1)
   {
      char tmpBuf[BUFLEN_64];

      cgiGetValueByName(query, "sessionKey", tmpBuf);
      cgiUrlDecode(tmpBuf);
      cgiSetVar("sessionKey", tmpBuf);

      if(cgiValidateSessionKey(fs) != CMSRET_SUCCESS)
      {
         /*sessionkey validation failed*/
         return;
      }

   }

   if( cgiGetValueByNameSafe(query, basicTabName, NULL, (char*)webVarValue, WEB_BUF_SIZE_MAX) == CGI_STS_OK )
   {
      cmsLog_debug( "%s, webVarValue %s", basicTabName, webVarValue);
      selectedBasicTab = atoi(webVarValue);
   } 
   else
   {
      cmsLog_debug("Cannot get the parameter from the web query for variable %s\n", basicTabName);
   }
   
   if( cgiGetValueByNameSafe(query, callMgtTabName, NULL, (char*)webVarValue, WEB_BUF_SIZE_MAX) == CGI_STS_OK )
   {
      cmsLog_debug( "%s, webVarValue %s", callMgtTabName, webVarValue);
      selectedCallMgtTab = atoi(webVarValue);
   } 
   else
   {
      cmsLog_debug("Cannot get the parameter from the web query for variable %s\n", callMgtTabName);
   }
   
   /* Set parameters in MDM */
   setMdmParameters( query );

   /* Write datamodel to flash */
   dalVoice_Save();

   /* Start Voice */
   dalVoice_voiceStart(msgHandle);

   /* Release lock so that voice start msg could be handled */
   cmsLck_releaseLock();

   /* Sleep a bit before redrawing the page
    ** in order to read current status of the voice app (up or down)*/
   usleep( SLEEP_2SEC  );

   /* Re-aquire lock for further httpd processing */
   cmsLck_acquireLock();

   reDisplayPage( query, fs );
}

void cgiVoiceStop(char *query, FILE *fs)
{
   char  basicTabName[MAX_WEBVAR_NAME_LEN] = "currentBasicTab";
   char  callMgtTabName[MAX_WEBVAR_NAME_LEN] = "currentCallMgtTab";
   char  webVarValue[TEMP_CHARBUF_SIZE];
   
   memset((char *)webVarValue, 0, sizeof(webVarValue));
   /*
    * Protect against cross site forgery attacks
    * Note: this check is skipped for voice in do_cgi_cmd() as voice
    * commands does not have an action parameter to differentiate
    * between View/Set.
    */
   if (1)
   {
      char tmpBuf[BUFLEN_64];

      cgiGetValueByName(query, "sessionKey", tmpBuf);
      cgiUrlDecode(tmpBuf);
      cgiSetVar("sessionKey", tmpBuf);

      if(cgiValidateSessionKey(fs) != CMSRET_SUCCESS)
      {
         /*sessionkey validation failed*/
         return;
      }

   }
   
   if( cgiGetValueByNameSafe(query, basicTabName, NULL, (char*)webVarValue, WEB_BUF_SIZE_MAX) == CGI_STS_OK )
   {
      cmsLog_debug( "%s, webVarValue %s", basicTabName, webVarValue);
      selectedBasicTab = atoi(webVarValue);
   } 
   else
   {
      cmsLog_debug("Cannot get the parameter from the web query for variable %s\n", basicTabName);
   }
   
   if( cgiGetValueByNameSafe(query, callMgtTabName, NULL, (char*)webVarValue, WEB_BUF_SIZE_MAX) == CGI_STS_OK )
   {
      cmsLog_debug( "%s, webVarValue %s", callMgtTabName, webVarValue);
      selectedCallMgtTab = atoi(webVarValue);
   } 
   else
   {
      cmsLog_debug("Cannot get the parameter from the web query for variable %s\n", callMgtTabName);
   }
   
   /* Set parameters in MDM */
   setMdmParameters( query );

   /* Stop Voice */
   dalVoice_voiceStop(msgHandle);

   /* Release lock so that voice stop msg could be handled */
   cmsLck_releaseLock();

   /* Sleep a bit before redrawing the page
   ** in order to read current status of the voice app (up or down)*/
   usleep( SLEEP_3SEC  );

   /* Re-aquire lock for further httpd processing */
   cmsLck_acquireLock();

   /* Write datamodel to flash */
   dalVoice_Save();

   /* Redraw page */
   reDisplayPage( query, fs );
}

void cgiVoiceApply(char *query, FILE *fs)
{
   char  basicTabName[MAX_WEBVAR_NAME_LEN] = "currentBasicTab";
   char  callMgtTabName[MAX_WEBVAR_NAME_LEN] = "currentCallMgtTab";
   char  webVarValue[TEMP_CHARBUF_SIZE];

   memset((char *)webVarValue, 0, sizeof(webVarValue));
   /*
    * Protect against cross site forgery attacks
    * Note: this check is skipped for voice in do_cgi_cmd() as voice
    * commands does not have an action parameter to differentiate
    * between View/Set.
    */
   if (1)
   {
      char tmpBuf[BUFLEN_64];

      cgiGetValueByName(query, "sessionKey", tmpBuf);
      cgiUrlDecode(tmpBuf);
      cgiSetVar("sessionKey", tmpBuf);

      if(cgiValidateSessionKey(fs) != CMSRET_SUCCESS)
      {
         /*sessionkey validation failed*/
         return;
      }

   }
   
   if( cgiGetValueByNameSafe(query, basicTabName, NULL, (char*)webVarValue, WEB_BUF_SIZE_MAX) == CGI_STS_OK )
   {
      cmsLog_debug( "%s, webVarValue %s", basicTabName, webVarValue);
      selectedBasicTab = atoi(webVarValue);
   } 
   else
   {
      cmsLog_debug("Cannot get the parameter from the web query for variable %s\n", basicTabName);
   }
   
   if( cgiGetValueByNameSafe(query, callMgtTabName, NULL, (char*)webVarValue, WEB_BUF_SIZE_MAX) == CGI_STS_OK )
   {
      cmsLog_debug( "%s, webVarValue %s", callMgtTabName, webVarValue);
      selectedCallMgtTab = atoi(webVarValue);
   } 
   else
   {
      cmsLog_debug("Cannot get the parameter from the web query for variable %s\n", callMgtTabName);
   }  

   /* Set parameters in MDM */
   setMdmParameters( query );

   /* Write datamodel to flash */
   dalVoice_Save();

   /* Release lock so that voice stop msg could be handled */
   cmsLck_releaseLock();

   /* Sleep a bit before redrawing the page
   ** in order to read current status of the voice app (up or down)*/
   usleep( SLEEP_100mSEC  );

   /* Re-aquire lock for further httpd processing */
   cmsLck_acquireLock();

   /* Redraw page */
   reDisplayPage( query, fs );
}

void cgiVoiceCctkUpload(char *query, FILE *fs)
{
   /*
    * Protect against cross site forgery attacks 
    * Note: this check is skipped for voice in do_cgi_cmd() as voice 
    * commands does not have an action parameter to differentiate
    * between View/Set.
    */
   if (1) 
   {
      char tmpBuf[BUFLEN_64];

      cgiGetValueByName(query, "sessionKey", tmpBuf);
      cgiUrlDecode(tmpBuf);
      cgiSetVar("sessionKey", tmpBuf);

      if(cgiValidateSessionKey(fs) != CMSRET_SUCCESS)
      {
         /*sessionkey validation failed*/
         return;
      }

   }
   /* Set parameters in MDM */
   setMdmParameters( query );

   /* Write datamodel to flash */
   dalVoice_Save();

   /* Release lock so that voice stop msg could be handled */
   cmsLck_releaseLock(); 
   
   /* Sleep a bit before redrawing the page
   ** in order to read current status of the voice app (up or down)*/
   usleep( SLEEP_100mSEC  );

   /* Re-aquire lock for further httpd processing */
   cmsLck_acquireLock();

   /* Redraw page */
   reDisplayPage( query, fs );
}


void cgiVoiceRefresh(char *query, FILE *fs)
{   
   reDisplayPage( query, fs );
}

#if defined( DMP_X_BROADCOM_COM_NTR_1 )
void cgiVoiceRestore(char *query, FILE *fs)
{
   dalVoice_SetNtrDefaults(NULL, NULL);

   reDisplayPage( query, fs );
}
#endif

void cgiVoiceSetDefault(char *query, FILE *fs)
{
   char  basicTabName[MAX_WEBVAR_NAME_LEN] = "currentBasicTab";
   char  callMgtTabName[MAX_WEBVAR_NAME_LEN] = "currentCallMgtTab";
   char  webVarValue[TEMP_CHARBUF_SIZE];

   cpInst = -1;
   vpInst = -1;
   
   memset((char *)webVarValue, 0, sizeof(webVarValue));
   /*
    * Protect against cross site forgery attacks
    * Note: this check is skipped for voice in do_cgi_cmd() as voice
    * commands does not have an action parameter to differentiate
    * between View/Set.
    */
   if (1)
   {
      char tmpBuf[BUFLEN_64];

      cgiGetValueByName(query, "sessionKey", tmpBuf);
      cgiUrlDecode(tmpBuf);
      cgiSetVar("sessionKey", tmpBuf);

      if(cgiValidateSessionKey(fs) != CMSRET_SUCCESS)
      {
         /*sessionkey validation failed*/
         return;
      }

   }
   
   if( cgiGetValueByNameSafe(query, basicTabName, NULL, (char*)webVarValue, WEB_BUF_SIZE_MAX) == CGI_STS_OK )
   {
      cmsLog_debug( "%s, webVarValue %s", basicTabName, webVarValue);
      selectedBasicTab = atoi(webVarValue);
   } 
   else
   {
      cmsLog_debug("Cannot get the parameter from the web query for variable %s\n", basicTabName);
   }
   
   if( cgiGetValueByNameSafe(query, callMgtTabName, NULL, (char*)webVarValue, WEB_BUF_SIZE_MAX) == CGI_STS_OK )
   {
      cmsLog_debug( "%s, webVarValue %s", callMgtTabName, webVarValue);
      selectedCallMgtTab = atoi(webVarValue);
   } 
   else
   {
      cmsLog_debug("Cannot get the parameter from the web query for variable %s\n", callMgtTabName);
   }
   
   /* Write datamodel to flash */
   dalVoice_SetDefaults(NULL, NULL);

   dalVoice_Save();

   /* Release lock so that voice stop msg could be handled */
   cmsLck_releaseLock();

   /* Sleep a bit before redrawing the page
   ** in order to read current status of the voice app (up or down)*/
   usleep( SLEEP_2SEC  );

   /* Re-aquire lock for further httpd processing */
   cmsLck_acquireLock();

   /* Redraw page */
   reDisplayPage( query, fs );
}

void cgiVoiceVoipProfSelect(char *query, FILE *fs)
{
   void* webVarValue;
   int vsInst;
   DAL_VOICE_PARMS parms;
   char  varName[MAX_WEBVAR_NAME_LEN] = "voipProf";
   char  tabName[MAX_WEBVAR_NAME_LEN] = "currentBasicTab";

   /* Allocate memory for the parameter to be obtained from the Web GUI query. */
   webVarValue = cmsMem_alloc( WEB_BIG_BUF_SIZE_MAX_WLAN, ALLOC_ZEROIZE );
   if ( webVarValue == NULL )
   {
      cmsLog_error( "Could not get allocate memory for parameter. \n");
      return;
   }
   
   cmsLog_debug("query %s\n", query);

   if (CMSRET_SUCCESS != dalVoice_mapSpNumToSvcInst( 0, &vsInst ))
   {
      return;
   }
   
   parms.op[0] = vsInst;
   parms.op[1] = 0;
   parms.msgHandle = msgHandle;

   if( cgiGetValueByNameSafe(query, varName, NULL, (char*)webVarValue, WEB_BUF_SIZE_MAX) == CGI_STS_OK )
   {
      cmsLog_debug( "Getting per-VoipProfile param, webVarValue %s", webVarValue);
   } 
   else
   {
      cmsLog_error("Cannot get the parameter from the web query for variable %s\n", varName);
   }	
   
   if(CMSRET_SUCCESS == dalVoice_MapVoIPProfileNameToVoIPProfileInst(&parms, webVarValue, &vpInst))
   {
	  cmsLog_debug("Successfully found VoIP instance %d for name %s", vpInst, webVarValue); 
   }
   else
   {
	   vpInst = -1;
   }	     
   
   if( cgiGetValueByNameSafe(query, tabName, NULL, (char*)webVarValue, WEB_BUF_SIZE_MAX) == CGI_STS_OK )
   {
      cmsLog_debug( "%s, webVarValue %s", tabName, webVarValue);
      selectedBasicTab = atoi(webVarValue);
   } 
   else
   {
      cmsLog_error("Cannot get the parameter from the web query for variable %s\n", varName);
   }
   
   cmsMem_free( webVarValue );
   /*
    * Protect against cross site forgery attacks
    * Note: this check is skipped for voice in do_cgi_cmd() as voice
    * commands does not have an action parameter to differentiate
    * between View/Set.
    */
   if (1)
   {
      char tmpBuf[BUFLEN_64];

      cgiGetValueByName(query, "sessionKey", tmpBuf);
      cgiUrlDecode(tmpBuf);
      cgiSetVar("sessionKey", tmpBuf);

      if(cgiValidateSessionKey(fs) != CMSRET_SUCCESS)
      {
         /*sessionkey validation failed*/
         return;
      }

   }
	  
   /* Release lock so that voice stop msg could be handled */
   cmsLck_releaseLock();

   /* Sleep a bit before redrawing the page
   ** in order to read current status of the voice app (up or down)*/
   usleep( SLEEP_100mSEC  );

   /* Re-aquire lock for further httpd processing */
   cmsLck_acquireLock();

   /* Redraw page */
   reDisplayPage( query, fs );
}

void cgiVoiceCodecProfSelect(char *query, FILE *fs)
{
   void* webVarValue;
   int vsInst;
   char  varName[MAX_WEBVAR_NAME_LEN] = "codecProf";
   char  tabName[MAX_WEBVAR_NAME_LEN] = "currentBasicTab";
   DAL_VOICE_PARMS parms;

   /* Allocate memory for the parameter to be obtained from the Web GUI query. */
   webVarValue = cmsMem_alloc( WEB_BIG_BUF_SIZE_MAX_WLAN, ALLOC_ZEROIZE );
   if ( webVarValue == NULL )
   {
      cmsLog_error( "Could not get allocate memory for parameter. \n");
      return;
   }
   cmsLog_debug("query %s\n", query);

   if (CMSRET_SUCCESS != dalVoice_mapSpNumToSvcInst( 0, &vsInst ))
   {
      return;
   }
   
   parms.op[0] = vsInst;
   parms.op[1] = 0;
   parms.msgHandle = msgHandle;

   if( cgiGetValueByNameSafe(query, varName, NULL, (char*)webVarValue, WEB_BUF_SIZE_MAX) == CGI_STS_OK )
   {
      cmsLog_debug( "Getting per-CodecProfile param, webVarValue %s", webVarValue);
   } 
   else
   {
      cmsLog_error("Cannot get the parameter from the web query for variable %s\n", varName);
   }	
   
   if(CMSRET_SUCCESS == dalVoice_MapCodecNameToCodecProfileInst(&parms, webVarValue, &cpInst))
   {
	  cmsLog_debug("Successfully found codec instance %d for name %s", cpInst, webVarValue); 
   }
   else
   {
	   cpInst = -1;
   }	   

   if( cgiGetValueByNameSafe(query, tabName, NULL, (char*)webVarValue, WEB_BUF_SIZE_MAX) == CGI_STS_OK )
   {
      cmsLog_debug( "%s, webVarValue %s", tabName, webVarValue);
      selectedBasicTab = atoi(webVarValue);
   } 
   else
   {
      cmsLog_error("Cannot get the parameter from the web query for variable %s\n", varName);
   }
   
   cmsMem_free( webVarValue );
   /*
    * Protect against cross site forgery attacks
    * Note: this check is skipped for voice in do_cgi_cmd() as voice
    * commands does not have an action parameter to differentiate
    * between View/Set.
    */
   if (1)
   {
      char tmpBuf[BUFLEN_64];

      cgiGetValueByName(query, "sessionKey", tmpBuf);
      cgiUrlDecode(tmpBuf);
      cgiSetVar("sessionKey", tmpBuf);

      if(cgiValidateSessionKey(fs) != CMSRET_SUCCESS)
      {
         /*sessionkey validation failed*/
         return;
      }

   }
	  
   /* Release lock so that voice stop msg could be handled */
   cmsLck_releaseLock();

   /* Sleep a bit before redrawing the page
   ** in order to read current status of the voice app (up or down)*/
   usleep( SLEEP_100mSEC  );

   /* Re-aquire lock for further httpd processing */
   cmsLck_acquireLock();

   /* Redraw page */
   reDisplayPage( query, fs );
}

#ifdef DMP_X_BROADCOM_COM_DECTENDPOINT_1
/****************************************************************************
* FUNCTION:    cgiVoiceDectOpenRegWnd
*
* PURPOSE:     Send command to open the registration window for DECT
*
* PARAMETERS:  query  -  the query string from the HTTPD
*              fs     -  the file handler from HTTPD
*
* RETURNS:     Nothing
*
* NOTE:
****************************************************************************/
void cgiVoiceDectOpenRegWnd(char *query, FILE *fs)
{
   CmsRet          ret;
   int             repeatCount = 10;
   char            windowStatus[WEB_BUF_SIZE_MAX];
   DAL_VOICE_PARMS parms = { { 1, OGF_NORMAL_UPDATE, } };

   /* Send command to open the DECT registration window */
   dalVoice_dectCtlRegWnd( 1 /* Open */, msgHandle );

   /* Release lock so that voice msg could be handled */
   cmsLck_releaseLock();

   do
   {
      /* Sleep a bit before redrawing the page
      ** in order to read current data
      */
      usleep( SLEEP_100mSEC );

      cmsLck_acquireLock();
      ret = dalVoice_GetDectRegWnd( &parms, (void*)windowStatus, WEB_BUF_SIZE_MAX );
      cmsLck_releaseLock();

      if ( ret != CMSRET_SUCCESS )
      {
         cmsLog_error( "Failed to get DECT registration window status\n" );
         continue;
      }
   }
   while( --repeatCount && (strncmp(windowStatus, "Closed", WEB_BUF_SIZE_MAX) == 0) );

   if ( repeatCount <= 0 )
   {
      cmsLog_error( "Failed all attempts to get DECT registration window status\n" );
   }

   /* Re-aquire lock for further httpd processing */
   cmsLck_acquireLock();

   /* Redraw page */
   reDisplayPage( query, fs );
}


/****************************************************************************
* FUNCTION:    cgiVoiceDectCloseRegWnd
*
* PURPOSE:     Send command to close the registration window for DECT
*
* PARAMETERS:  query  -  the query string from the HTTPD
*              fs     -  the file handler from HTTPD
*
* RETURNS:     Nothing
*
* NOTE:
****************************************************************************/
void cgiVoiceDectCloseRegWnd(char *query, FILE *fs)
{
   CmsRet          ret;
   int             repeatCount = 10;
   char            windowStatus[WEB_BUF_SIZE_MAX];
   DAL_VOICE_PARMS parms = { { 1, OGF_NORMAL_UPDATE, } };

   /* Send command to close the DECT registration window */
   dalVoice_dectCtlRegWnd( 0 /* Close */, msgHandle );

   /* Release lock so that voice msg could be handled */
   cmsLck_releaseLock();

   do
   {
      /* Sleep a bit before redrawing the page
      ** in order to read current data
      */
      usleep( SLEEP_100mSEC );

      cmsLck_acquireLock();
      ret = dalVoice_GetDectRegWnd( &parms, (void*)windowStatus, WEB_BUF_SIZE_MAX );
      cmsLck_releaseLock();

      if ( ret != CMSRET_SUCCESS )
      {
         cmsLog_error( "Failed to get DECT registration window status\n" );
         continue;
      }
   }
   while( --repeatCount && (strncmp(windowStatus, "Open", WEB_BUF_SIZE_MAX) == 0) );

   if ( repeatCount <= 0 )
   {
      cmsLog_error( "Failed all attempts to get DECT registration window status\n" );
   }

   /* Re-aquire lock for further httpd processing */
   cmsLck_acquireLock();

   /* Redraw page */
   reDisplayPage( query, fs );
}


/****************************************************************************
* FUNCTION:    cgiVoiceDectSetAc
*
* PURPOSE:     Send command to set the access code for DECT registration
*
* PARAMETERS:  query  -  the query string from the HTTPD
*              fs     -  the file handler from HTTPD
*
* RETURNS:     Nothing
*
* NOTE:
****************************************************************************/
void cgiVoiceDectSetAc(char *query, FILE *fs)
{
   /* Set parameters in MDM, this will trigger the STL function to
   ** be invoked accordingly.
   */
   setMdmParameters( query );

   /* Release lock so that voice stop msg could be handled */
   cmsLck_releaseLock();

   /* Sleep a bit before redrawing the page
   ** in order to read current Access code*/
   usleep( SLEEP_100mSEC  );

   /* Re-aquire lock for further httpd processing */
   cmsLck_acquireLock();

   /* Redraw page */
   reDisplayPage( query, fs );
}


/****************************************************************************
* FUNCTION:    cgiVoiceDectDelHset
*
* PURPOSE:     Send command to delete a handset registered with the DECT
*              base station
*
* PARAMETERS:  query  -  the query string from the HTTPD
*              fs     -  the file handler from HTTPD
*
* RETURNS:     Nothing
*
* NOTE:
****************************************************************************/
void cgiVoiceDectDelHset(char *query, FILE *fs)
{
   unsigned int hset = 0;
   void* webVarValue = NULL;
   char  newWebVarName[MAX_WEBVAR_NAME_LEN];
   CGI_STATUS cgiStatus;

   webVarValue = cmsMem_alloc( WEB_BUF_SIZE_MAX, ALLOC_ZEROIZE );
   if ( webVarValue == NULL )
   {
      cmsLog_error( "cgiVoiceDectDelHset: could not allocate memory. \n");
      return;
   }

   /* The variable name passed by the web page in the query which contains the
   ** handset identifier.
   */
   sprintf( newWebVarName, "dectHset" );

   if( (cgiStatus = cgiGetValueByNameSafe( query,
                                           newWebVarName,
                                           NULL,
                                           (char*)webVarValue,
                                           WEB_BUF_SIZE_MAX )) == CGI_STS_OK )
   {
      /* Send command to delete the handset registration */
      hset = strtoul ( (char *) webVarValue, NULL, 10 );
      dalVoice_dectCtlDelHset( hset, msgHandle );
   }
   else
   {
      cmsLog_error( "cgiVoiceDectDelHset: cgiGetValueByNameSafe - error %d. \n",
                    cgiStatus );
   }

   cmsMem_free( webVarValue );


   /* Release lock so that voice msg could be handled */
   cmsLck_releaseLock();

   /* Sleep a bit before redrawing the page
   ** in order to read current data*/
   usleep( SLEEP_100mSEC  );

   /* Re-aquire lock for further httpd processing */
   cmsLck_acquireLock();

   /* Redraw page */
   reDisplayPage( query, fs );
}


/****************************************************************************
* FUNCTION:    cgiVoiceDectPingHset
*
* PURPOSE:     Send command to 'ping' a handset registered with the DECT
*              base station.  Pinging the handset will result in applying
*              a ring on this handset to identify which one it is.
*
* PARAMETERS:  query  -  the query string from the HTTPD
*              fs     -  the file handler from HTTPD
*
* RETURNS:     Nothing
*
* NOTE:
****************************************************************************/
void cgiVoiceDectPingHset(char *query, FILE *fs)
{
   unsigned int hset = 0;
   void* webVarValue = NULL;
   char  newWebVarName[MAX_WEBVAR_NAME_LEN];
   CGI_STATUS cgiStatus;

   webVarValue = cmsMem_alloc( WEB_BUF_SIZE_MAX, ALLOC_ZEROIZE );
   if ( webVarValue == NULL )
   {
      cmsLog_error( "cgiVoiceDectPingHset: could not allocate memory. \n");
      return;
   }

   /* The variable name passed by the web page in the query which contains the
   ** handset identifier.
   */
   sprintf( newWebVarName, "dectHset" );

   if( (cgiStatus = cgiGetValueByNameSafe( query,
                                           newWebVarName,
                                           NULL,
                                           (char*)webVarValue,
                                           WEB_BUF_SIZE_MAX )) == CGI_STS_OK )
   {
      /* Send command to delete the handset registration */
      hset = strtoul ( (char *) webVarValue, NULL, 10 );
      dalVoice_dectCtlPingHset( hset, msgHandle );
   }
   else
   {
      cmsLog_error( "cgiVoiceDectPingHset: cgiGetValueByNameSafe - error %d. \n",
                    cgiStatus );
   }

   cmsMem_free( webVarValue );

   /* Redraw page */
   reDisplayPage( query, fs );
}

/****************************************************************************
* FUNCTION:    cgiVoiceDectPingAllHset
*
* PURPOSE:     Send command to 'ping' all handsets registered with the DECT
*              base station.  Pinging the handsets will result in applying
*              a ring on each handset.
*
* PARAMETERS:  query  -  the query string from the HTTPD
*              fs     -  the file handler from HTTPD
*
* RETURNS:     Nothing
*
* NOTE:
****************************************************************************/
void cgiVoiceDectPingAllHset(char *query, FILE *fs)
{
    /* Send command to ping the handsets */
   dalVoice_dectCtlPingHset(0, msgHandle); /* use 0 as "all handsets" id */

   /* Release lock so that voice msg could be handled */
   cmsLck_releaseLock();

   /* Sleep a bit before redrawing the page
   ** in order to read current data*/
   usleep( SLEEP_100mSEC  );

   /* Re-aquire lock for further httpd processing */
   cmsLck_acquireLock();

   /* Redraw page */
   reDisplayPage( query, fs );
}
#endif


/****************************************************************************
* FUNCTION:    setMdmParameters
*
* PURPOSE:     Set the parameters in MDM based on the query given by the Web page.
*
* PARAMETERS:  query    [IN]  -  query given by the Web page
*
* RETURNS:
*
* NOTE:
*
****************************************************************************/
static void setMdmParameters( char* query )
{
   void* webVarValue;
   char  newWebVarName[MAX_WEBVAR_NAME_LEN];
   int numFxs = 0, numSipClients = 0, numFeatureSets = 0;
   int numCcLines = 0, numCcExt = 0;
   int tmpInst;

   VOICE_PARM_TABLE* pVoiceParmTable;

   /* Allocate memory for the parameter to be obtained from the Web GUI query. */
   webVarValue = cmsMem_alloc( WEB_BIG_BUF_SIZE_MAX_WLAN, ALLOC_ZEROIZE );
   if ( webVarValue == NULL )
   {
      cmsLog_error( "Could not get allocate memory for parameter. \n");
      return;
   }
   cmsLog_debug("Setting MDM, query %s\n", query);
   /* Iterate through the table to read the parameters passed through the query */
   pVoiceParmTable = voiceParmTable;
   DAL_VOICE_PARMS parms;
   parms.op[0] = 1;
   parms.op[1] = 0;
   parms.msgHandle = msgHandle;

   while ( pVoiceParmTable->webVarName != NULL )
   {  
      if(pVoiceParmTable->parmDefinitionBitmap == 0)
	  {
         snprintf( newWebVarName, MAX_WEBVAR_NAME_LEN, "%s", pVoiceParmTable->webVarName);
         /* Get the parameter from the Web query */
         if( cgiGetValueByNameSafe(query, newWebVarName, NULL, (char*)webVarValue, WEB_BUF_SIZE_MAX) == CGI_STS_OK )
         {
            /* Set the values in MDM */
            if ( pVoiceParmTable->dalVoice_setFunc != NULL )
            {
              pVoiceParmTable->dalVoice_setFunc( &parms, webVarValue );
            }
         } 
         else
         {
            cmsLog_debug("Cannot get the parameter from the web query for variable %s\n", newWebVarName);
         }
      } 
      else if (pVoiceParmTable->parmDefinitionBitmap & VOICE_PER_FXS_PARAM)
      {
         dalVoice_GetNumPhysFxsEndpt( &numFxs );
         
         for (int i = 1; i <= numFxs; i++)
		 {
			snprintf( newWebVarName, MAX_WEBVAR_NAME_LEN, "%s%d", pVoiceParmTable->webVarName, i);
            if( cgiGetValueByNameSafe(query, newWebVarName, NULL, (char*)webVarValue, WEB_BUF_SIZE_MAX) == CGI_STS_OK )
            {
               parms.op[1] = i-1;  
			   if (CMSRET_SUCCESS != dalVoice_mapPotsFxsNumToInst(&parms, &tmpInst))
               {
				   cmsLog_debug("Could not map FXS %d", i-1);
				   continue;
			   }
			   parms.op[1] = tmpInst;
               
			   cmsLog_debug( "Setting per-FXS param, webVarValue %s, inst %d", webVarValue, tmpInst);
               /* Set the values in MDM */
               if ( pVoiceParmTable->dalVoice_setFunc != NULL )
               {
                  pVoiceParmTable->dalVoice_setFunc( &parms, webVarValue );
               }
            } 
            else
            {
               cmsLog_debug("Cannot get the parameter from the web query for variable %s\n", newWebVarName);
            }			
		 }			 
	  }
	  else if (pVoiceParmTable->parmDefinitionBitmap & VOICE_PER_CLIENT_PARAM)
      {
         dalVoice_GetNumSipClient( &parms, &numSipClients );
         
         for (int i = 1; i <= numSipClients; i++)
		 {
			snprintf( newWebVarName, MAX_WEBVAR_NAME_LEN, "%s%d", pVoiceParmTable->webVarName, i);
            if( cgiGetValueByNameSafe(query, newWebVarName, NULL, (char*)webVarValue, WEB_BUF_SIZE_MAX) == CGI_STS_OK )
            {
               parms.op[1] = i-1;  
			   if (CMSRET_SUCCESS != dalVoice_mapAcntNumToClientInst(&parms, &tmpInst))
               {
				   cmsLog_error("Could not map FXS %d", i-1);
				   continue;
			   }
			   parms.op[1] = tmpInst;
               
			   cmsLog_debug( "Setting per-client param, webVarValue %s, inst %d", webVarValue, tmpInst);
               /* Set the values in MDM */
               if ( pVoiceParmTable->dalVoice_setFunc != NULL )
               {
                  pVoiceParmTable->dalVoice_setFunc( &parms, webVarValue );
               }
            } 
            else
            {
               cmsLog_debug("Cannot get the parameter from the web query for variable %s\n", newWebVarName);
            }			
		 }			 
	  }
	  else if (pVoiceParmTable->parmDefinitionBitmap & VOICE_PER_NETWORK_PARAM)
      {
         snprintf( newWebVarName, MAX_WEBVAR_NAME_LEN, "%s", pVoiceParmTable->webVarName);
         if( cgiGetValueByNameSafe(query, newWebVarName, NULL, (char*)webVarValue, WEB_BUF_SIZE_MAX) == CGI_STS_OK )
         {
			parms.op[1] = 0; /* Only one network for now */
			if( dalVoice_mapNetworkNumToInst( &parms , &tmpInst ) != CMSRET_SUCCESS) 
            {				   
			    cmsLog_debug("Could not map network 0");
				continue;
            }
			
		    parms.op[1] = tmpInst;    
               
		    cmsLog_debug( "Setting per-network param, webVarValue %s", webVarValue);
            /* Set the values in MDM */
            if ( pVoiceParmTable->dalVoice_setFunc != NULL )
            {
               pVoiceParmTable->dalVoice_setFunc( &parms, webVarValue );
            }
         } 
         else
         {
            cmsLog_debug("Cannot get the parameter from the web query for variable %s\n", newWebVarName);			
		 }			 
	  }
	  else if (pVoiceParmTable->parmDefinitionBitmap & VOICE_ROUTING_PARAM)
      {
		  dalVoice_GetNumOfLine( &parms, &numCcLines );
		  dalVoice_GetNumOfExtension( &parms, &numCcExt );

		  for (int i = 0; i < numCcLines; i++)
          {
			  for (int j = 0; j < numCcExt; j++)
              {
			      snprintf( newWebVarName, MAX_WEBVAR_NAME_LEN, "%s%d_%d", pVoiceParmTable->perLineJsVarString, i, j);

                  if( cgiGetValueByNameSafe(query, newWebVarName, NULL, (char*)webVarValue, WEB_BUF_SIZE_MAX) == CGI_STS_OK )
                  {
					 parms.op[1] = i; 
					 parms.op[2] = j;
					 dalVoice_mapLineExtToIncomingMapInst(&parms, &tmpInst);
					 if (!strcmp(webVarValue, "true") && tmpInst < 0)
			         {
						 /* Add a map instance. The user requested a new mapping (not in MDM) to be added */
                         dalVoice_AddCallCtrlIncomingMap( &parms, &tmpInst );
						 parms.op[1] = tmpInst;
                         dalVoice_SetIncomingMapEnabled( &parms, "enabled" );
						 dalVoice_mapAcntNumToLineInst(parms.op[0], i, &tmpInst);
						 parms.op[2] = tmpInst;
						 dalVoice_mapExtNumToExtInst(parms.op[0], j, &tmpInst);
                         parms.op[3] = tmpInst;
                         dalVoice_SetIncomingMapLineExt( &parms );
					 }
					 else if (!strcmp(webVarValue, "false") && tmpInst < 0)
			         {
						 /* Do nothing. User asked to disable a mapping that already does not exist - nothing changed */
						 ;
					 }
					 else if (!strcmp(webVarValue, "true") && tmpInst >= 0)
			         {
						 /* Do nothing. User asked to enable a mapping that already exists - nothing changed */
						 ;
					 }
					 else if (!strcmp(webVarValue, "false") && tmpInst >= 0)
			         {
						 /* Remove a map instance. The user requested to remove an existing mapping */
						 parms.op[1] = tmpInst;
                         dalVoice_DeleteCallCtrlIncomingMap( &parms );
					 }
					 
					 /* Apply similar changes to Outgoing map */
					 parms.op[1] = i; 
					 parms.op[2] = j;
					 dalVoice_mapLineExtToOutgoingMapInst(&parms, &tmpInst);
					 if (!strcmp(webVarValue, "true") && tmpInst < 0)
			         {
						 /* Add a map instance. The user requested a new mapping (not in MDM) to be added */
                         dalVoice_AddCallCtrlOutgoingMap( &parms, &tmpInst );
						 parms.op[1] = tmpInst;
                         dalVoice_SetOutgoingMapEnabled( &parms, "enabled" );
						 dalVoice_mapAcntNumToLineInst(parms.op[0], i, &tmpInst);
						 parms.op[2] = tmpInst;
						 dalVoice_mapExtNumToExtInst(parms.op[0], j, &tmpInst);
                         parms.op[3] = tmpInst;
                         dalVoice_SetOutgoingMapLineExt( &parms );
					 }
					 else if (!strcmp(webVarValue, "false") && tmpInst < 0)
			         {
						 /* Do nothing. User asked to disable a mapping that already does not exist - nothing changed */
						 ;
					 }
					 else if (!strcmp(webVarValue, "true") && tmpInst >= 0)
			         {
						 /* Do nothing. User asked to enable a mapping that already exists - nothing changed */
						 ;
					 }
					 else if (!strcmp(webVarValue, "false") && tmpInst >= 0)
			         {
						 /* Remove a map instance. The user requested to remove an existing mapping */
						 parms.op[1] = tmpInst;
                         dalVoice_DeleteCallCtrlOutgoingMap( &parms );
					 }
                  }				  
			  }
		  }
	  }
	  else if (pVoiceParmTable->parmDefinitionBitmap & VOICE_PER_FEATURESET_PARAM)
      {
         dalVoice_GetNumCallFeatureSet( &parms, &numFeatureSets );
         
         for (int i = 1; i <= numFeatureSets; i++)
		 {
			snprintf( newWebVarName, MAX_WEBVAR_NAME_LEN, "%s%d", pVoiceParmTable->perLineJsVarString, i);
            if( cgiGetValueByNameSafe(query, newWebVarName, NULL, (char*)webVarValue, WEB_BUF_SIZE_MAX) == CGI_STS_OK )
            {
               parms.op[1] = i-1;  
			   if (CMSRET_SUCCESS != dalVoice_mapCallFeatureSetNumToInst(&parms, &tmpInst))
               {
				   cmsLog_error("Could not map FXS %d", i-1);
				   continue;
			   }
			   parms.op[1] = tmpInst;
               
			   cmsLog_debug( "Setting per-featureset param %s, webVarValue %s, inst %d", newWebVarName, webVarValue, tmpInst);
               /* Set the values in MDM */
               if ( pVoiceParmTable->dalVoice_setFunc != NULL )
               {
                  pVoiceParmTable->dalVoice_setFunc( &parms, webVarValue );
               }
            } 
            else
            {
               cmsLog_debug("Cannot get the parameter from the web query for variable %s\n", newWebVarName);
            }			
		 }			 
	  }
	  else if (pVoiceParmTable->parmDefinitionBitmap & VOICE_PER_VOIPPROF_PARAM)
      {
         snprintf( newWebVarName, MAX_WEBVAR_NAME_LEN, "%s", pVoiceParmTable->webVarName);
         if( cgiGetValueByNameSafe(query, newWebVarName, NULL, (char*)webVarValue, WEB_BUF_SIZE_MAX) == CGI_STS_OK )
         {
            if (vpInst != -1)
            {
               parms.op[1] = vpInst; /* use the vp instance currently loaded by user */ 
               
		       cmsLog_debug( "Setting per-voipprofile param, webVarValue %s", webVarValue);
               /* Set the values in MDM */
               if ( pVoiceParmTable->dalVoice_setFunc != NULL )
               {
                  pVoiceParmTable->dalVoice_setFunc( &parms, webVarValue );
               }
            }
         } 
         else
         {
            cmsLog_debug("Cannot get the parameter from the web query for variable %s\n", newWebVarName);			
		 }			 
	  }
	  else if (pVoiceParmTable->parmDefinitionBitmap & VOICE_PER_CODECPROF_PARAM)
      {
         snprintf( newWebVarName, MAX_WEBVAR_NAME_LEN, "%s", pVoiceParmTable->webVarName);
         if( cgiGetValueByNameSafe(query, newWebVarName, NULL, (char*)webVarValue, WEB_BUF_SIZE_MAX) == CGI_STS_OK )
         {
            if (cpInst != -1)
            {
               parms.op[1] = cpInst; /* use the cp instance currently loaded by user */ 
               
		       cmsLog_debug( "Setting per-codecprofile param, webVarValue %s", webVarValue);
               /* Set the values in MDM */
               if ( pVoiceParmTable->dalVoice_setFunc != NULL )
               {
                  pVoiceParmTable->dalVoice_setFunc( &parms, webVarValue );
               }
            }
         } 
         else
         {
            cmsLog_debug("Cannot get the parameter from the web query for variable %s\n", newWebVarName);			
		 }			 
	  }
      pVoiceParmTable++;
   }

   cmsMem_free( webVarValue );
}


/****************************************************************************
* FUNCTION:    reDisplayPage
*
* PURPOSE:     Re-display the page based on the query submitted by the Web page.
*              This function will re-display the view from which the query
*              was submitted, For example, if the Web page submitted a form from a
*              basic view, this function will redisplay the basic view.
*
* PARAMETERS:  query    [IN]  -  query given by the Web page
*              fs       [IN]  -  pointer to the web file stream
*
* RETURNS:
*
* NOTE:
*
****************************************************************************/
static void reDisplayPage( char* query, FILE* fs )
{
   char currentView[MAX_VIEW_LEN];
   char path[BUFLEN_1024];

   memset( currentView, 0, sizeof(currentView) );

   /* Get the current view (it's passed through the query by the HTML/Javascript code) */
   if ( cgiGetValueByName( query, VIEW_CURRENT, currentView ) != CGI_STS_OK )
   {
      cmsLog_error( "Could not extract value for parameter from the query. \n" );
   }

   /* Re-display the page based on the current view */
   if ( strncmp( currentView, VIEW_GLOBAL, MAX_VIEW_LEN ) == 0 )
   {
      makePathToWebPage( path, HTTPD_BUFLEN_10K, HTMLPAGE_BASIC );
   }
   else if ( strncmp( currentView, VIEW_SRVPROV, MAX_VIEW_LEN ) == 0 )
   {
      makePathToWebPage( path, HTTPD_BUFLEN_10K, HTMLPAGE_SRVPROV );
   }
   else if ( strncmp( currentView, VIEW_CALLMGT, MAX_VIEW_LEN ) == 0 )
   {
      makePathToWebPage( path, HTTPD_BUFLEN_10K, HTMLPAGE_CALLMGT );
   }
   else if ( strncmp( currentView, VIEW_STATS, MAX_VIEW_LEN ) == 0 )
   {
      makePathToWebPage( path, HTTPD_BUFLEN_10K, HTMLPAGE_STATS );
   }
   else
#if defined( DMP_X_BROADCOM_COM_DECTENDPOINT_1 )	   
   if ( strncmp( currentView, VIEW_DECT, MAX_VIEW_LEN ) == 0 )
   {
      makePathToWebPage( path, HTTPD_BUFLEN_10K, HTMLPAGE_DECT );
   }
   else
#endif
   if ( strncmp( currentView, VIEW_DEBUG, MAX_VIEW_LEN ) == 0 )
   {
      makePathToWebPage( path, HTTPD_BUFLEN_10K, HTMLPAGE_DEBUG );
   }
   else
   {
      /* Default view is basic */
      makePathToWebPage( path, HTTPD_BUFLEN_10K, HTMLPAGE_BASIC );
   }

   do_ej( path, fs );   
}
 

/****************************************************************************
* FUNCTION:    getVoiceSipImpl
*
* PURPOSE:     Get whether SIP is enabled with CMGR/CCTK support.
*
* PARAMETERS:
*
**  RETURNS:   CMSRET_SUCCESS
*
* NOTE:
*
****************************************************************************/
static CmsRet getVoiceSipImpl(DAL_VOICE_PARMS *parms, char *sipImpl, unsigned int length )
{
   snprintf( (char*)sipImpl, length, "1" );
   return CMSRET_SUCCESS;
}

/****************************************************************************
* FUNCTION:    getVoiceDnsImpl
*
* PURPOSE:     Get whether voice DNS build is enabled.
*
* PARAMETERS:  dnsImpl - Writes "1" if enabled, "0" otherwise
*
**RETURNS:     CMSRET_SUCCESS
*
* NOTE:
*
****************************************************************************/
static CmsRet getVoiceDnsImpl(DAL_VOICE_PARMS *parms, char *dnsImpl, unsigned int length )
{
#ifdef BRCM_SIP_VOICE_DNS
   snprintf( (char*)dnsImpl, length, "1" );
#else
   snprintf( (char*)dnsImpl, length, "0" );
#endif // BRCM_SIP_VOICE_DNS
   return CMSRET_SUCCESS;
}

/****************************************************************************
* FUNCTION:    getVoiceProvImpl
*
* PURPOSE:     Get the voice provisioning implementation
*
* PARAMETERS:  provImpl - Writes "1" if TR-104v1, "2" if TR-104v2
*
**RETURNS:     CMSRET_SUCCESS
*
* NOTE:
*
****************************************************************************/
static CmsRet getVoiceProvImpl(DAL_VOICE_PARMS *parms, char *provImpl, unsigned int length )
{
   snprintf( (char*)provImpl, length, "2" );

   return CMSRET_SUCCESS;
}

/****************************************************************************
* FUNCTION:    getVoiceSipMode
*
* PURPOSE:     Get the voice SIP operation mode
*
* PARAMETERS:  sipMode - Writes "RFC3261" for RFC3261, "IMS" for IMS
*
**RETURNS:     CMSRET_SUCCESS
*
* NOTE:
*
****************************************************************************/
static CmsRet getVoiceSipMode(DAL_VOICE_PARMS *parms, char *sipMode, unsigned int length )
{
#ifdef VOICE_IMS_SUPPORT
   snprintf( (char*)sipMode, length, "IMS" );
#else
   snprintf( (char*)sipMode, length, "RFC3261" );
#endif

   return CMSRET_SUCCESS;
}

#endif /* DMP_VOICE_SERVICE_2 */
#endif /* BRCM_VOICE_SUPPORT */


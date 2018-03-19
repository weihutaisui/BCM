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

#include <cms_log.h>
#include <cms_mem.h>
#include <cms_msg.h>

#include <cgi_main.h>
#include <cgi_voice.h>

#include <httpd.h>

/* ---- Public Variables ------------------------------------------------- */

/* ---- Constants and Types ---------------------------------------------- */

#define VIEW_CURRENT          "currentview"
#define VIEW_BASIC            "basic"
#define VIEW_ADVANCED         "advanced"
#define VIEW_DEBUG            "debug"
#define VIEW_CCTK             "cctk"
#define MAX_VIEW_LEN          20

#ifdef SIPLOAD
#  define HTMLPAGE_BASIC        "voicesip_basic.html"
#  define HTMLPAGE_ADVANCED     "voicesip_advanced.html"
#  define HTMLPAGE_DEBUG        "voicesip_debug.html"
#  define HTMLPAGE_CCTK         "voicesip_cctk.html"
#else
#  define HTMLPAGE_BASIC        ""
#  define HTMLPAGE_ADVANCED     ""
#  define HTMLPAGE_DEBUG        ""
#  define HTMLPAGE_CCTK         ""
#endif /* SIPLOAD */

#if defined( DMP_X_BROADCOM_COM_NTR_1 )
#  define VIEW_NTR            "ntr"
#  define HTMLPAGE_NTR        "voicentr.html"
#endif /* DMP_X_BROADCOM_COM_NTR_1 */

#if defined( DMP_X_BROADCOM_COM_DECTENDPOINT_1 )
#   define VIEW_DECT          "dect"
#   define HTMLPAGE_DECT      "voicedect.html"
#endif /* DMP_X_BROADCOM_COM_DECTENDPOINT_1 */

#define TEMP_CHARBUF_SIZE     64
#define MAX_WEBVAR_NAME_LEN   40
#define SLEEP_100mSEC         100*1000
#define SLEEP_300mSEC         300*1000
#define SLEEP_1SEC            1000*1000
#define SLEEP_2SEC            2000*1000
#define SLEEP_3SEC            3500*1000
#define DECT_MAX_WAIT_ITERATION  10
/* ---- Private Variables ------------------------------------------------ */
static CmsRet getVoiceSipImpl(DAL_VOICE_PARMS *parms, char *sipImpl, unsigned int length );
static CmsRet getVoiceDnsImpl(DAL_VOICE_PARMS *parms, char *dnsImpl, unsigned int length );
static CmsRet getEptAppStatus(DAL_VOICE_PARMS *parms, char *data, unsigned int length );
static CmsRet getVoiceProvImpl(DAL_VOICE_PARMS *parms, char *provImpl, unsigned int length );
static CmsRet getVoiceSipMode(DAL_VOICE_PARMS *parms, char *sipMode, unsigned int length );

static SIP_PARM_TABLE sipParmTable[] =
{
   /*paramName        numArgs perLine   perSP     JSvarname  GET function                                SET function                       */
   { "voiceProtocol",   0,    FALSE,    FALSE,      "",      dalVoice_GetSignalingProtocol,               NULL                                },
   { "voiceProvImpl",   0,    FALSE,    FALSE,      "",      getVoiceProvImpl,                            NULL                                },
   { "voiceSipImpl",    0,    FALSE,    FALSE,      "",      getVoiceSipImpl,                             NULL                                },
   { "voiceSipMode",    0,    FALSE,    FALSE,      "",      getVoiceSipMode,                             NULL                                },
   { "voiceStatus",     0,    FALSE,    FALSE,      "",      dalVoice_GetStatus,                          NULL                                },
   { "consolelogLvl",   0,    FALSE,    FALSE,      "",      dalVoice_GetLoggingLevel,                    dalVoice_SetLoggingLevel            },
   { "maxSp",           0,    FALSE,    FALSE,      "",      dalVoice_GetNumVoiceProfiles,                NULL                                },
   { "numFxo",          0,    FALSE,    FALSE,      "",      dalVoice_GetNumPhysFxoEndptStr,              NULL                                },
   { "numFxs",          0,    FALSE,    FALSE,      "",      dalVoice_GetNumPhysFxsEndptStr,              NULL                                },
   { "ifName",          0,    FALSE,    FALSE,      "",      dalVoice_GetBoundIfName,                     dalVoice_SetBoundIfName             },
   { "dnsAddr",         0,    FALSE,    FALSE,      "",      dalVoice_GetDnsServerAddr,                   dalVoice_SetDnsServerAddr           },
   { "dnsImpl",         0,    FALSE,    FALSE,      "",      getVoiceDnsImpl,                             NULL                                },
   { "ifList",          0,    FALSE,    FALSE,      "",      dalVoice_GetNetworkIntfList,                 NULL                                },
   { "ipAddrFamily",    0,    FALSE,    FALSE,      "",      dalVoice_GetIpFamily,                        dalVoice_SetIpFamily                },
   { "ipFamilyList",    0,    FALSE,    FALSE,      "",      dalVoice_GetIpFamilyList,                    NULL                                },
   { "transportList",   0,    FALSE,    FALSE,      "",      dalVoice_GetSupportedTransports,             NULL                                },
   { "backToPrimList",  0,    FALSE,    FALSE,      "",      dalVoice_GetSupportedBackToPrimOptions,      NULL                                },
   { "srtpOptionList",  0,    FALSE,    FALSE,      "",      dalVoice_GetSupportedSrtpOptions,            NULL                                },
   { "redOptionList",   0,    FALSE,    FALSE,      "",      dalVoice_GetSupportedRedOptions,             NULL                                },
   { "confOptionList",  0,    FALSE,    FALSE,      "",      dalVoice_GetSupportedConfOptions,            NULL                                },
   { "localeList",      0,    FALSE,    FALSE,      "",      dalVoice_GetRegionSuppString,                NULL                                },
   { "maxLines",        1,    FALSE,    TRUE,       "ml",    dalVoice_GetNumLinesPerVoiceProfile,         NULL                                },
   { "localeName",      1,    FALSE,    TRUE,       "sl",    dalVoice_GetRegion,                          dalVoice_SetRegion                  },
   { "flexTerms",       0,    FALSE,    FALSE,      "",      dalVoice_GetFlexTermSupport,                 NULL                                },

#if defined( DMP_X_BROADCOM_COM_NTR_1 )
   { "ntrEnabled",      1,    FALSE,    FALSE,      "",      dalVoice_GetNtrEnable,                       dalVoice_SetNtrEnable               },
   { "ntrAuto",         1,    FALSE,    FALSE,      "",      dalVoice_GetNtrAutoModeEnable,               dalVoice_SetNtrAutoModeEnable       },
   { "ntrAutoOffset",   1,    FALSE,    FALSE,      "",      dalVoice_GetNtrAutoOffset,                   NULL                                },
   { "ntrAutoPpm",      1,    FALSE,    FALSE,      "",      dalVoice_GetNtrAutoPpm,                      NULL                                },
   { "ntrManualOffset", 1,    FALSE,    FALSE,      "",      dalVoice_GetNtrManualOffset,                 dalVoice_SetNtrManualOffset         },
   { "ntrManualPpm",    1,    FALSE,    FALSE,      "",      dalVoice_GetNtrManualPpm,                    NULL                                },
   { "ntrManualSteps",  1,    FALSE,    FALSE,      "",      dalVoice_GetNtrManualSteps,                  NULL                                },
   { "ntrDebug",        1,    FALSE,    FALSE,      "",      dalVoice_GetNtrDebugEnable,                  dalVoice_SetNtrDebugEnable          },
   { "ntrPcmMipsCurr",  1,    FALSE,    FALSE,      "",      dalVoice_GetNtrPcmMipsTallyCurrent,          NULL                                },
   { "ntrPcmMipsPrev",  1,    FALSE,    FALSE,      "",      dalVoice_GetNtrPcmMipsTallyPrevious,         NULL                                },
   { "ntrPcmNtrCurr",   1,    FALSE,    FALSE,      "",      dalVoice_GetNtrPcmNtrTallyCurrent,           NULL                                },
   { "ntrPcmNtrPrev",   1,    FALSE,    FALSE,      "",      dalVoice_GetNtrPcmNtrTallyPrevious,          NULL                                },
   { "ntrDslMipsCurr",  1,    FALSE,    FALSE,      "",      dalVoice_GetNtrDslMipsTallyCurrent,          NULL                                },
   { "ntrDslMipsPrev",  1,    FALSE,    FALSE,      "",      dalVoice_GetNtrDslMipsTallyPrevious,         NULL                                },
   { "ntrDslNtrCurr",   1,    FALSE,    FALSE,      "",      dalVoice_GetNtrDslNtrTallyCurrent,           NULL                                },
   { "ntrDslNtrPrev",   1,    FALSE,    FALSE,      "",      dalVoice_GetNtrDslNtrTallyPrevious,          NULL                                },
   { "ntrSampleRate",   1,    FALSE,    FALSE,      "",      dalVoice_GetNtrSampleRate,                   dalVoice_SetNtrSampleRate           },
   { "ntrPllBandwidth", 1,    FALSE,    FALSE,      "",      dalVoice_GetNtrPllBandwidth,                 dalVoice_SetNtrPllBandwidth         },
   { "ntrDampingFactor",1,    FALSE,    FALSE,      "",      dalVoice_GetNtrDampingFactor,                dalVoice_SetNtrDampingFactor        },
   { "ntrAutoOffsetHistory",1,FALSE,    FALSE,      "",      dalVoice_GetNtrAutoOffsetHistory,            NULL                                },
   { "ntrAutoPpmHistory",1,   FALSE,    FALSE,      "",      dalVoice_GetNtrAutoPpmHistory,               NULL                                },
   { "ntrAutoStepsHistory",1, FALSE,    FALSE,      "",      dalVoice_GetNtrAutoStepsHistory,             NULL                                },
   { "ntrAutoPhaseErrorHistory",1, FALSE, FALSE,    "",      dalVoice_GetNtrAutoPhaseErrorHistory,        NULL                                },
#else /* !DMP_X_BROADCOM_COM_NTR_1 */
/* Used even if NTR support is off to find out if NTR webpage should be displayed */
   { "ntrEnabled",      1,    FALSE,    FALSE,      "",      dalVoice_GetNtrEnable,                       NULL                                },
#endif /* DMP_X_BROADCOM_COM_NTR_1 */
   { "eptAppStatus",    1,    FALSE,    FALSE,      "",      getEptAppStatus,                             NULL                                },

#ifdef SIPLOAD
   { "maxPrefCodecs",   0,    FALSE,    FALSE,      "",      dalVoice_GetMaxPrefCodecs,                   NULL                                },
   { "suppCodecList",   0,    FALSE,    FALSE,      "",      dalVoice_GetSuppCodecsString,                NULL                                },
   { "proxyAddr",       1,    FALSE,    TRUE,       "pa",    dalVoice_GetSipProxyServer,                  dalVoice_SetSipProxyServer          },
   { "proxyPort",       1,    FALSE,    TRUE,       "pp",    dalVoice_GetSipProxyServerPort,              dalVoice_SetSipProxyServerPort      },
   { "obProxyAddr",     1,    FALSE,    TRUE,       "oa",    dalVoice_GetSipOutboundProxy,                dalVoice_SetSipOutboundProxy        },
   { "obProxyPort",     1,    FALSE,    TRUE,       "op",    dalVoice_GetSipOutboundProxyPort,            dalVoice_SetSipOutboundProxyPort    },
   { "regAddr",         1,    FALSE,    TRUE,       "ra",    dalVoice_GetSipRegistrarServer,              dalVoice_SetSipRegistrarServer      },
   { "regPort",         1,    FALSE,    TRUE,       "rp",    dalVoice_GetSipRegistrarServerPort,          dalVoice_SetSipRegistrarServerPort  },
   { "digiMap",         1,    FALSE,    FALSE,      "",      dalVoice_GetDigitMap,                        dalVoice_SetDigitMap                },
   { "domainName",      1,    FALSE,    TRUE,       "de",    dalVoice_GetSipUserAgentDomain,              dalVoice_SetSipUserAgentDomain      },

   { "extension",       2,    TRUE,     TRUE,       "ex",    dalVoice_GetVlSipURI,                        dalVoice_SetVlSipURI                },
   { "dispName",        2,    TRUE,     TRUE,       "dn",    dalVoice_GetVlCFCallerIDName,                dalVoice_SetVlCFCallerIDName        },
   { "authName",        2,    TRUE,     TRUE,       "an",    dalVoice_GetVlSipAuthUserName,               dalVoice_SetVlSipAuthUserName       },
   { "password",        2,    TRUE,     TRUE,       "pw",    dalVoice_GetVlSipAuthPassword,               dalVoice_SetVlSipAuthPassword       },
   { "physEndpt",       2,    TRUE,     TRUE,       "pe",    dalVoice_GetVlPhyReferenceList,              dalVoice_SetVlPhyReferenceList      },
   { "physFxoEndpt",    2,    TRUE,     TRUE,       "pfxoe", dalVoice_GetVlFxoPhyReferenceList,           dalVoice_SetVlFxoPhyReferenceList   },
#if DMP_X_BROADCOM_COM_DECTENDPOINT_1
   { "dectHsList",      0,    FALSE,    FALSE,      "hs",    dalVoice_GetDectRegisteredHsList,            NULL                                }, // hack
   { "dectAttachedHsList", 2, TRUE,     TRUE,       "hs",    dalVoice_GetLineAttachedHandsetList,         dalVoice_SetLineAttachedHandsetList },
#endif /* DMP_X_BROADCOM_COM_DECTENDPOINT_1 */
   { "codecList",       2,    TRUE,     TRUE,       "cl",    dalVoice_GetVlCLCodecList,                   dalVoice_SetVlCLCodecList           },
   { "voicePtime",      2,    TRUE,     TRUE,       "vp",    dalVoice_GetVlCLPacketizationPeriod,         dalVoice_SetVlCLPacketizationPeriod },
   { "lineEnabled",     2,    TRUE,     TRUE,       "le",    dalVoice_GetVlEnable,                        dalVoice_SetVlEnable                },
   { "lineStatus",      2,    TRUE,     TRUE,       "ls",    dalVoice_GetVlVoipStatus,                    NULL                                },
   { "vadEnable",       2,    TRUE,     TRUE,       "ve",    dalVoice_GetVlCLSilenceSuppression,          dalVoice_SetVlCLSilenceSuppression  },
   { "ingressGain",     2,    TRUE,     TRUE,       "ig",    dalVoice_GetVlVPTransmitGain,                dalVoice_SetVlVPTransmitGain        },
   { "egressGain",      2,    TRUE,     TRUE,       "eg",    dalVoice_GetVlVPRecieveGain,                 dalVoice_SetVlVPReceiveGain         },
   { "sipLogAddr",      1,    FALSE,    TRUE,       "la",    dalVoice_GetLogServer,                       dalVoice_SetLogServer               },
   { "sipLogPort",      1,    FALSE,    TRUE,       "lp",    dalVoice_GetLogServerPort,                   dalVoice_SetLogServerPort           },

   { "mwi",             2,    TRUE,     TRUE,       "mw",    dalVoice_GetVlCFMWIEnable,                   dalVoice_SetVlCFMWIEnable           },
   { "fwdNum",          2,    TRUE,     TRUE,       "cfnum", dalVoice_GetVlCFCallFwdNum,                  dalVoice_SetVlCFCallFwdNum          },
   { "fwdAll",          2,    TRUE,     TRUE,       "cfall", dalVoice_GetVlCFCallFwdAll,                  dalVoice_SetVlCFCallFwdAll          },
   { "fwdBusy",         2,    TRUE,     TRUE,       "cfbus", dalVoice_GetVlCFCallFwdBusy,                 dalVoice_SetVlCFCallFwdBusy         },
   { "fwdNoAns",        2,    TRUE,     TRUE,       "cfnoa", dalVoice_GetVlCFCallFwdNoAns,                dalVoice_SetVlCFCallFwdNoAns        },
   { "callwait",        2,    TRUE,     TRUE,       "cw",    dalVoice_GetVlCFCallWaiting,                 dalVoice_SetVlCFCallWaiting         },
   { "barEnabled",      2,    TRUE,     TRUE,       "be",    dalVoice_GetVlCFCallBarring,                 dalVoice_SetVlCFCallBarring         },
   { "barPin",          2,    TRUE,     TRUE,       "bpin",  dalVoice_GetVlCFCallBarringPin,              dalVoice_SetVlCFCallBarringPin      },
   { "barDmap",         2,    TRUE,     TRUE,       "bdmp",  dalVoice_GetVlCFCallBarringDigitMap,         dalVoice_SetVlCFCallBarringDigitMap },
   { "wlEnabled",       2,    TRUE,     TRUE,       "wle",   dalVoice_GetVlCFWarmLine,                    dalVoice_SetVlCFWarmLine            },
   { "wlNum",           2,    TRUE,     TRUE,       "wln",   dalVoice_GetVlCFWarmLineNum,                 dalVoice_SetVlCFWarmLineNum         },
   { "anonBlock",       2,    TRUE,     TRUE,       "acb",   dalVoice_GetVlCFAnonCallBlck,                dalVoice_SetVlCFAnonCallBlck        },
   { "anonCall",        2,    TRUE,     TRUE,       "anc",   dalVoice_GetVlCFAnonymousCalling,            dalVoice_SetVlCFAnonymousCalling    },
   { "doNotDisturb",    2,    TRUE,     TRUE,       "dnd",   dalVoice_GetVlCFDoNotDisturb,                dalVoice_SetVlCFDoNotDisturb        },

   { "euFlashEn",       1,    FALSE,    TRUE,      "euflen", dalVoice_GetEuroFlashEnable,                 dalVoice_SetEuroFlashEnable         },
   { "musicServerAddr", 1,    FALSE,    TRUE,      "mss",    dalVoice_GetSipMusicServer,                  dalVoice_SetSipMusicServer          },
   { "musicServerPort", 1,    FALSE,    TRUE,      "msp",    dalVoice_GetSipMusicServerPort,              dalVoice_SetSipMusicServerPort      },
   { "confUri",         1,    FALSE,    TRUE,      "confuri",dalVoice_GetSipConferencingURI,              dalVoice_SetSipConferencingURI      },
   { "confOption",      1,    FALSE,    TRUE,      "confopt",dalVoice_GetSipConferencingOption,           dalVoice_SetSipConferencingOption   },
   { "secDomainName",   1,    FALSE,    TRUE,      "sdn",    dalVoice_GetSipSecDomainName,                dalVoice_SetSipSecDomainName        },
   { "secProxyAddr",    1,    FALSE,    TRUE,      "spa",    dalVoice_GetSipSecProxyAddr,                 dalVoice_SetSipSecProxyAddr         },
   { "secProxyPort",    1,    FALSE,    TRUE,      "spp",    dalVoice_GetSipSecProxyPort,                 dalVoice_SetSipSecProxyPort         },
   { "secObProxyAddr",  1,    FALSE,    TRUE,      "sopa",   dalVoice_GetSipSecOutboundProxyAddr,         dalVoice_SetSipSecOutboundProxyAddr },
   { "secObProxyPort",  1,    FALSE,    TRUE,      "sopp",   dalVoice_GetSipSecOutboundProxyPort,         dalVoice_SetSipSecOutboundProxyPort },
   { "secRegistrarAddr",1,    FALSE,    TRUE,      "sra",    dalVoice_GetSipSecRegistrarAddr,             dalVoice_SetSipSecRegistrarAddr     },
   { "secRegistrarPort",1,    FALSE,    TRUE,      "srp",    dalVoice_GetSipSecRegistrarPort,             dalVoice_SetSipSecRegistrarPort     },
   { "regExpTmr",       1,    FALSE,    TRUE,      "rt",     dalVoice_GetSipRegisterExpires,              dalVoice_SetSipRegisterExpires      },
   { "regRetryInt",     1,    FALSE,    TRUE,      "rrt",    dalVoice_GetSipRegisterRetryInterval,        dalVoice_SetSipRegisterRetryInterval},
   { "transport",       1,    FALSE,    TRUE,      "trp",    dalVoice_GetSipTransportString,              dalVoice_SetSipTransport            },
   { "backToPrim",      1,    FALSE,    TRUE,      "toprim", dalVoice_GetSipBackToPrimOptionString,       dalVoice_SetSipBackToPrimOption     },
   { "dialPlan",        1,    FALSE,    TRUE,      "dm",     dalVoice_GetDigitMap,                        dalVoice_SetDigitMap                },
   { "CctkDigitMap",    1,    FALSE,    TRUE,      "cctkdm", dalVoice_GetCCTKDigitMap,                    dalVoice_SetCCTKDigitMap            },
   { "vbdEnabled",      1,    FALSE,    TRUE,      "vbd",    dalVoice_GetVBDEnable,                       NULL                                },
   { "t38Enable",       1,    FALSE,    TRUE,      "",       NULL,                                        dalVoice_SetT38Enable               },
   { "v18Enable",       1,    FALSE,    TRUE,      "vet",    dalVoice_GetV18Enable,                       dalVoice_SetV18Enable               },
   { "dtmfRelay",       1,    FALSE,    TRUE,      "dr",     dalVoice_GetDTMFMethod,                      dalVoice_SetDTMFMethod              },
   { "hookFlashRelay",  1,    FALSE,    TRUE,      "hr",     dalVoice_GetHookFlashMethodString,           dalVoice_SetHookFlashMethod         },
   { "tosByteSip",      1,    FALSE,    TRUE,      "tosSip", dalVoice_GetSipDSCPMark,                     dalVoice_SetSipDSCPMark             },
   { "tosByteRtp",      1,    FALSE,    TRUE,      "tosRtp", dalVoice_GetRtpDSCPMark,                     dalVoice_SetRtpDSCPMark             },
   { "enblTagMat",      1,    FALSE,    TRUE,      "tagMatch", dalVoice_GetSipToTagMatching,              dalVoice_SetSipToTagMatching        },
   { "enblFailover",    1,    FALSE,    TRUE,      "failen", dalVoice_GetSipFailoverEnable,               dalVoice_SetSipFailoverEnable       },
   { "enblSipOptions",  1,    FALSE,    TRUE,      "sipopt", dalVoice_GetSipOptionsEnable,                dalVoice_SetSipOptionsEnable        },
#endif /* SIPLOAD */
   { "srtpOption",      1,    FALSE,    TRUE,      "srtp",   dalVoice_GetSrtpOptionString,                dalVoice_SetSrtpOption              },
   { "redOption",       1,    FALSE,    TRUE,       "red",   dalVoice_GetRedOptionString,                 dalVoice_SetRedOption               },
#if DMP_X_BROADCOM_COM_PSTNENDPOINT_1
   { "pstnDialPlan",    1,    FALSE,    FALSE,      "",      dalVoice_GetPstnDialPlan,                    dalVoice_SetPstnDialPlan            },
   { "pstnRouteRule",   1,    FALSE,    FALSE,      "",      dalVoice_GetPstnRouteRule,                   dalVoice_SetPstnRouteRule           },
   { "pstnRouteData",   1,    FALSE,    FALSE,      "",      dalVoice_GetPstnRouteData,                   dalVoice_SetPstnRouteData           },
#endif /* DMP_X_BROADCOM_COM_PSTNENDPOINT_1 */

#ifdef STUN_CLIENT
   { "stunSrvAddr",     1,    FALSE,    FALSE,      "",      dalVoice_GetSTUNServer,                      NULL                                },
   { "stunSrvPort",     1,    FALSE,    FALSE,      "",      dalVoice_GetSTUNServerPort,                  NULL                                },
#endif /* STUN_CLIENT */

#if DMP_X_BROADCOM_COM_DECTENDPOINT_1
   /* DECT Base Station */
   { "dectStatus",      1,    FALSE,    FALSE,      "",      dalVoice_GetDectStatus,                      NULL                                },
   { "dectRegWdn",      1,    FALSE,    FALSE,      "",      dalVoice_GetDectRegWnd,                      NULL                                },
   { "dectAc",          1,    FALSE,    FALSE,      "",      dalVoice_GetDectAc,                          dalVoice_SetDectAc                  },
   { "dectFwVer",       1,    FALSE,    FALSE,      "",      dalVoice_GetDectFwVer,                       NULL                                },
   { "dectLnk",         1,    FALSE,    FALSE,      "",      dalVoice_GetDectLnk,                         NULL                                },
   { "dectType",        1,    FALSE,    FALSE,      "",      dalVoice_GetDectType,                        NULL                                },
   { "dectId",          1,    FALSE,    FALSE,      "",      dalVoice_GetDectId,                          NULL                                },
   { "dectManic",       1,    FALSE,    FALSE,      "",      dalVoice_GetDectManic,                       NULL                                },
   { "dectModic",       1,    FALSE,    FALSE,      "",      dalVoice_GetDectModic,                       NULL                                },
   { "dectMaxHset",     1,    FALSE,    FALSE,      "",      dalVoice_GetDectMaxHset,                     NULL                                },
   { "dectCurHset",     1,    FALSE,    FALSE,      "",      dalVoice_GetDectCurHset,                     NULL                                },
   /* DECT Handset(s) */
   { "dectHsStatus",    2,    TRUE,     FALSE,      "st",    dalVoice_GetDectHsStatus,                    NULL                                },
   { "dectHsName",      2,    TRUE,     FALSE,      "hn",    dalVoice_GetDectHsetName,                    NULL                                },
   { "dectHsManic",     2,    TRUE,     FALSE,      "ma",    dalVoice_GetDectHsManic,                     NULL                                },
   { "dectHsModic",     2,    TRUE,     FALSE,      "mo",    dalVoice_GetDectHsModic,                     NULL                                },
   { "dectHsIpei",      2,    TRUE,     FALSE,      "ip",    dalVoice_GetDectHsIpei,                      NULL                                },
   { "dectHsSubTime",   2,    TRUE,     FALSE,      "su",    dalVoice_GetDectHsSubTime,                   NULL                                },
   { "dectHsIdent",     2,    TRUE,     FALSE,      "id",    dalVoice_GetDectHsId,                        NULL                                },
#endif
   { "dectSupport",     1,    FALSE,    FALSE,      "",      dalVoice_GetDectSupport,                     NULL                                },

   { NULL,              0,    FALSE,    FALSE,      "",      NULL,                                        NULL                                }
};

static char *cctk_upload = NULL;
static int   upload_size = 0;

/* ---- Private Function Prototypes -------------------------------------- */

static void formatCodecList( char* varObj );
static void setMdmParameters( char* query );
static void prepareGetSetParms( int numArgs, int lineIndex, int vpIndex, DAL_VOICE_PARMS* parms );
static void reDisplayPage( char* query, FILE* fs );
static int  getNumLinesPerSp( int spNum );
static int  getNumSp( void );

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
   SIP_PARM_TABLE* pSipParmTable = sipParmTable;
   UBOOL8 varRet = FALSE;

   /* Allocate memory for the object to be obtained through the DAL layer. */
   varObj = malloc(WEB_BUF_SIZE_MAX);
   if ( varObj == NULL )
   {
      cmsLog_error( "Could not get allocate memory for object for variable %s. \n", varName );
      return;
   }
   memset(varObj, 0, WEB_BUF_SIZE_MAX);

   /* Compare the passed web variable with the one in the SIP parameters table.
   ** If match is found, get the value for that variable through the DAL layer */
   while ( pSipParmTable->webVarName != NULL )
   {
      if ( strcmp( varName, pSipParmTable->webVarName ) == 0 )
      {
         int acnt, line, sp, vp;
         int numLines, numSp;
         DAL_VOICE_PARMS parms;
         char tmp[100];
#if DMP_X_BROADCOM_COM_DECTENDPOINT_1
         unsigned int       hslist[10];
#endif /* DMP_X_BROADCOM_COM_DECTENDPOINT_1 */

         varFound = TRUE;

         /* Get the parameter/object value from DAL */
         if ( pSipParmTable->dalVoice_getFunc == NULL )
            break;

         numSp = (pSipParmTable->perSP == TRUE) ? getNumSp() : 1;

         for ( sp = 0; sp < numSp ; sp++ )
         {
            numLines = (pSipParmTable->perLine == TRUE) ? getNumLinesPerSp(sp) : 1;

            /* If there are no FXO endpoints on the board, then make sure the
             ** dal functions are NULL for each pstn lookup. */
            if( strcmp( varName, "numFxo" ) )
            {
               char* numFxo = malloc(3);
               dalVoice_GetNumPhysFxoEndptStr( NULL, numFxo, 3 );
               if( numFxo[0] == '0' )
               {
                  SIP_PARM_TABLE* pSPT = sipParmTable;
                  while(pSPT->webVarName != NULL)
                  {
                     if( strcmp("pstnDialPlan", pSPT->webVarName) == 0
                           || strcmp("pstnRouteRule", pSPT->webVarName) == 0
                           || strcmp("pstnRouteData", pSPT->webVarName) == 0 )
                     {
                        pSPT->dalVoice_getFunc = NULL;
                        pSPT->dalVoice_setFunc = NULL;
                     }
                     pSPT++;
                  }
               }
               free(numFxo);
            }

            /* Map spNum to vpInst */
            if ( (dalVoice_mapSpNumToVpInst( sp, &vp)) != CMSRET_SUCCESS )
            {
               cmsLog_error( "Cannot map spNum to vpInst for sp: %d, using vp of 1!\n", sp );
               vp = 1;
            }

#if DMP_X_BROADCOM_COM_DECTENDPOINT_1
            /* If we are getting a parameter related to DECT handset, we adjust the
            ** maximum value of 'lines' here to make it correspond to the maximum
            ** value of DECT handset supported.
            */
            parms.op[0]=vp;
            if ( strstr ( varName, "dect" ) && 
                 pSipParmTable->perLine && 
                 strncmp ( varName, "dectHsList", 10 ) &&
                 strncmp ( varName, "dectAttachedHsList", 18 ) )
            {
               dalVoice_GetDectCurHsetList( &parms, (unsigned int *)&numLines, hslist );
            }
#endif /* DMP_X_BROADCOM_COM_DECTENDPOINT_1 */

            /* Get the values for all the lines for the requested variable.
             ** For example, if "extension" is requested, get the values
             ** for the extensions of all lines. */
            for ( acnt = 0; acnt < numLines; acnt++ )
            {

#if DMP_X_BROADCOM_COM_DECTENDPOINT_1
               /* If we are getting a parameter related to DECT
               ** we dont need to map account numbers to line instances
               ** Since all dect dal functions refer to the dect handset number
               ** instead
               */
               if ( strstr( varName, "dect" ) && 
                    strncmp ( varName, "dectHsList", 10 ) &&
                    strncmp ( varName, "dectAttachedHsList", 18 ) )
               {
                  /* Since line is now representing the handset id, and numLines represents
                  ** the maximum number of handsets, we need to increment line by acnt + 1
                  */
                  line = hslist[acnt];
               }
               else
#endif /* DMP_X_BROADCOM_COM_DECTENDPOINT_1 */
               {
                  /* Map acnt */
                  if ( (dalVoice_mapAcntNumToLineInst( vp, acnt, &line)) != CMSRET_SUCCESS )
                  {
                     cmsLog_error( "Cannot map acnt %d to line for sp: %d, using line of 1!\n", acnt, sp );
                     line = 1;
                  }
               }

               prepareGetSetParms( pSipParmTable->numArgs, line, vp, &parms );

               cmsLog_debug( "DAL GET using buffer at %X\n", varObj);
               if ( pSipParmTable->dalVoice_getFunc( &parms, varObj, WEB_BUF_SIZE_MAX) == CMSRET_SUCCESS )
               {
                  cmsLog_debug( "DAL GET for variable %s = %s. for line %d, vp %d\n", varName, varObj, line, vp );
                  varRet = TRUE;
               }
               else
               {
                  cmsLog_error( "ERROR: Could not get object for variable %s for line %d, vp %d. \n", varName, line, vp );
               }

               if( (!strcmp( pSipParmTable->webVarName, "codecList" )) && varRet )
               {
                  formatCodecList( (char*)varObj );
               }

               if( (!strcmp( pSipParmTable->webVarName, "suppCodecList" )) && varRet )
               {
                  formatCodecList( (char*)varObj );
               }

               if ( pSipParmTable->perSP )
               {
                  if ( strlen( (char*)varObj ) > 0 )
                  {
                     if ( pSipParmTable->perLine )
                        sprintf(tmp, "%s%d_%d = \"%s\";", pSipParmTable->perLineJsVarString, sp, acnt, (char*)varObj );
                     else
                        sprintf(tmp, "%s%d = \"%s\";", pSipParmTable->perLineJsVarString, sp, (char*)varObj );
                     strncat(varValue, " ", WEB_BUF_SIZE_MAX);
                     strncat(varValue, tmp, WEB_BUF_SIZE_MAX);
                  }
               }
               else
               {
                  if ( pSipParmTable->perLine )
                  {
                     if ( strlen( (char*)varObj ) > 0 )
                     {
                        sprintf(tmp, "%s%d = \"%s\";", pSipParmTable->perLineJsVarString, acnt, (char*)varObj );
                        strncat(varValue, " ", WEB_BUF_SIZE_MAX);
                        strncat(varValue, tmp, WEB_BUF_SIZE_MAX);
                     }
                  }
                  else
                  {
                     if ( strlen( (char*)varObj ) > 0 && !strncmp ( varName, "dectHsList", 10 ) )
                     {
                        sprintf(tmp, "%s = \"%s\";", pSipParmTable->perLineJsVarString, (char*)varObj  );
                        strncat(varValue, " ", WEB_BUF_SIZE_MAX);
                        strncat(varValue, tmp, WEB_BUF_SIZE_MAX);
                     }
                     else
                     {
                        strncpy( varValue, (char*)varObj, WEB_BUF_SIZE_MAX );
                     }
                  }
               }
               varRet = FALSE;

               /* Reset varObj */
               *(char*)varObj = '\0';
            }
         }

         break;
      }

      /* Reset varObj */
      *(char*)varObj = '\0';

      pSipParmTable++;
   }

   free( varObj );

   /* Report error if the requested variable was not found in the table */
   if ( !varFound )
   {
      cmsLog_notice("Var %s NOT found in the var table. \n", varName );
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
   for( i=0; i< 5 * DECT_MAX_WAIT_ITERATION; i++ )
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
      
       /* Release lock so that dect msg could be handled */
      cmsLck_releaseLock();

      /* Sleep a bit before start dect */
      usleep( 3 * (SLEEP_1SEC) );

      /* Re-aquire lock for further httpd processing */
      cmsLck_acquireLock();
      
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
* FUNCTION:    formatCodecList
*
* PURPOSE:     Remove X_BROADCOM_COM_ prefix from codecs that contain it for display
*              purposes.
*
* PARAMETERS:  varObj [IN/OUT] -  codec list to be reformatted
*
* RETURNS:
*
* NOTE:
*
****************************************************************************/
static void formatCodecList( char* varObj )
{
   char  delim[2] = ",";
   char *pToken = NULL, *pLast = NULL, *pPrefix = NULL;;
   char  tempBuf[TEMP_CHARBUF_SIZE];
   char  tempCodecList[strlen(varObj)+ 1]; /* The redone list can only be smaller */

   memset( tempBuf, 0, TEMP_CHARBUF_SIZE );

   /* Redo the codec list, one codec at a time */
   pToken = strtok_r( varObj, delim, &pLast );
   if ( pToken != NULL )
   {
      /* Check for X_BROADCOM_COM_ prefix, remove, print 1st codec */
      if( (pPrefix = strstr( pToken, "X_BROADCOM_COM_" )) != NULL )
      {
         strcpy( tempBuf, (pPrefix + strlen( "X_BROADCOM_COM_" )) );
         sprintf(tempCodecList, "%s", tempBuf);
         /* Reset buffer */
         memset( tempBuf, 0, TEMP_CHARBUF_SIZE );
      }
      else
      {
         sprintf(tempCodecList, "%s", pToken);
      }
   }

   while ( pToken != NULL )
   {
      /* Find next Codec in list */
      pToken = strtok_r( NULL, delim, &pLast );
      if ( pToken != NULL )
      {
         /* If found, check for X_BROADCOM_COM_ prefix, remove, reprint Codec */
         strcat(tempCodecList, ",");
         if( (pPrefix = strstr( pToken, "X_BROADCOM_COM_" )) != NULL )
         {
            strcpy( tempBuf, (pPrefix + strlen( "X_BROADCOM_COM_" )) );
            strcat(tempCodecList, tempBuf);
            /* Reset buffer */
            memset( tempBuf, 0, TEMP_CHARBUF_SIZE );
         }
         else
         {
            strcat(tempCodecList, pToken);
         }
      }
   }
   varObj[0] = '\0';
   strcpy( varObj, tempCodecList );
}

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
   SIP_PARM_TABLE* pSipParmTable;
   int numLines, numSP;

   /* Allocate memory for the parameter to be obtained from the Web GUI query. */
   webVarValue = cmsMem_alloc( WEB_BIG_BUF_SIZE_MAX_WLAN, ALLOC_ZEROIZE );
   if ( webVarValue == NULL )
   {
      cmsLog_error( "Could not get allocate memory for parameter. \n");
      return;
   }

   /* Iterate through the table to read the parameters passed through the query */
   pSipParmTable = sipParmTable;
   while ( pSipParmTable->webVarName != NULL )
   {
      DAL_VOICE_PARMS parms;
      int line, vp, acnt, sp;

      numSP = (pSipParmTable->perSP == TRUE) ? getNumSp() : 1;

      for ( sp = 0; sp < numSP; sp++ )
      {
         numLines = (pSipParmTable->perLine == TRUE) ? getNumLinesPerSp(sp) : 1;

         /* Map spNum to vpInst */
         if ( (dalVoice_mapSpNumToVpInst( sp, &vp)) != CMSRET_SUCCESS )
         {
            cmsLog_error( "Cannot map spNum to vpInst for sp: %d, using vp of 1!\n", sp );
            vp = 1;
         }

         /* Set the values in MDM for all lines (if the parameter is per line). */
         for ( acnt = 0; acnt < numLines; acnt++ )
         {
            /* Map acnt */
            if ( (dalVoice_mapAcntNumToLineInst( vp, acnt, &line)) != CMSRET_SUCCESS )
            {
               cmsLog_error( "Cannot map spNum to vpInst for sp: %d, using vp of 1!\n", sp );
               vp = 1;
            }

            if ( pSipParmTable->perSP == TRUE )
            {
               if ( pSipParmTable->perLine == TRUE )
               {
                  /* Append the service provider id and line id to the name in the table if the parameter is per
                  ** service provider and per line id.  For example, "extension" (name in table) becomes extension0_0
                  ** extension0_1, extension1_0, etc. so that it matches the names passed through the query and thus
                  ** cgiGetValueByName succeeds. */
                  snprintf( newWebVarName, MAX_WEBVAR_NAME_LEN, "%s%d_%d", pSipParmTable->webVarName, sp, acnt );
               }
               else
               {
                  /* Append the service provider id to the name in the table if the parameter is per service provider
                  ** For example, "proxyAddr" (name in table) becomes proxyAddr0, proxyAddr1, etc.
                  ** so that it matches the names passed through the query and thus cgiGetValueByName succeeds. */
                  snprintf( newWebVarName, MAX_WEBVAR_NAME_LEN, "%s%d", pSipParmTable->webVarName, sp);
               }
            }
            else
            {
               /* Use the same name from the table if the parameter is not per service provider and per line .*/
               snprintf( newWebVarName, MAX_WEBVAR_NAME_LEN, "%s", pSipParmTable->webVarName);
            }

            /* Get the parameter from the Web query */
            if( cgiGetValueByNameSafe(query, newWebVarName, NULL, (char*)webVarValue, WEB_BUF_SIZE_MAX) == CGI_STS_OK )
            {
               prepareGetSetParms( pSipParmTable->numArgs, line, vp, &parms );

               /* handle upload cctk digitmap */
               if( !strcmp( pSipParmTable->webVarName, "CctkDigitMap" ) && cctk_upload != NULL && upload_size > 0 )
               {
               cmsLog_debug(" replace variable %s for sp %d acnt %d with upload content\n", newWebVarName, sp, acnt);
                  memcpy(webVarValue, cctk_upload, (upload_size >= WEB_BIG_BUF_SIZE_MAX_WLAN)?WEB_BIG_BUF_SIZE_MAX_WLAN-1:upload_size);
                  cmsMem_free( cctk_upload );
                  upload_size = 0;
               }

               /* Set the values in MDM */
               if ( pSipParmTable->dalVoice_setFunc != NULL )
               {
                  pSipParmTable->dalVoice_setFunc( &parms, webVarValue );
               }
               memset( &parms, 0, sizeof(DAL_VOICE_PARMS) );
            }
            else
            {
               cmsLog_debug("Cannot get the parameter from the web query for variable %s for sp %d acnt %d\n", newWebVarName, sp, acnt);
            }
         }
      }
      pSipParmTable++;
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
   if ( strncmp( currentView, VIEW_ADVANCED, MAX_VIEW_LEN ) == 0 )
   {
      makePathToWebPage( path, HTTPD_BUFLEN_10K, HTMLPAGE_ADVANCED );
   }
   else
   if ( strncmp( currentView, VIEW_DEBUG, MAX_VIEW_LEN ) == 0 )
   {
      makePathToWebPage( path, HTTPD_BUFLEN_10K, HTMLPAGE_DEBUG );
   }
#if defined( DMP_X_BROADCOM_COM_NTR_1 )
   else
   if ( strncmp( currentView, VIEW_NTR, MAX_VIEW_LEN ) == 0 )
   {
      makePathToWebPage( path, HTTPD_BUFLEN_10K, HTMLPAGE_NTR );
   }
#endif /* DMP_X_BROADCOM_COM_NTR_1 */
#if defined( DMP_X_BROADCOM_COM_DECTENDPOINT_1 )
   else
   if ( strncmp( currentView, VIEW_DECT, MAX_VIEW_LEN ) == 0 )
   {
      makePathToWebPage( path, HTTPD_BUFLEN_10K, HTMLPAGE_DECT );
   }
#endif /* DMP_X_BROADCOM_COM_DECTENDPOINT_1 */
   else
   if ( strncmp( currentView, VIEW_CCTK, MAX_VIEW_LEN ) == 0 )
   {
      makePathToWebPage( path, HTTPD_BUFLEN_10K, HTMLPAGE_CCTK );
   }
   else
   {
      /* Default view is basic */
      makePathToWebPage( path, HTTPD_BUFLEN_10K, HTMLPAGE_BASIC );
   }

   do_ej( path, fs );
}


/****************************************************************************
* FUNCTION:    prepareGetSetParms
*
* PURPOSE:     Prepare the parameter structure before getting/setting a value.
*
* PARAMETERS:  numArgs  [IN]  -  number of arguments for the specific get/set
*              lineIndex[IN]  -  index of the line for the specific get/set (1-based)
*              parms    [OUT] -  parameter structure used for get/set
*
* RETURNS:
*
* NOTE: THIS FUNCTION MUST BE CHANGED WHEN MULTIPLE VOICE PROFILE/ADD/DELETE LINE IS SUPPORTED
*
****************************************************************************/
static void prepareGetSetParms( int numArgs, int lineIndex, int vpIndex, DAL_VOICE_PARMS* parms )
{

   memset((void *)parms, 0, sizeof(DAL_VOICE_PARMS));

   /* Use HTTPD's message handle */
   parms->msgHandle = msgHandle;

   /* Check if we need to provide a voiceprofile instance number */
   if ( numArgs > 0 )
   {
      /* Hard code voice profile instance to 1 */
      if (vpIndex >= 0)
      {
         parms->op[0] = vpIndex;
      }
      else{
         parms->op[0] = 1;
      }
   }

   /* Check if we need to provide a line instance number */
   if ( numArgs > 1 )
   {
      /* Assign line instance number */
      parms->op[1] = lineIndex;
   }
}

/****************************************************************************
* FUNCTION:    getNumLinesPerSp
*
* PURPOSE:     Get number of configured service providers
*
* PARAMETERS:
*
* RETURNS:     number of configured service providers
*
* NOTE: Function needs to change if multiple voice profiles are supported
*
****************************************************************************/
static int getNumSp( void )
{
   int numSp;

   if ( dalVoice_GetNumSrvProv( &numSp ) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Could not get max service provider value Hard-coding to 1 sp! \n" );
      return 1;
   }
   else
   {
      return numSp;
   }
}


/****************************************************************************
* FUNCTION:    getNumLinesPerSp
*
* PURPOSE:     Get the number of configured lines per service provider
*
* PARAMETERS:
*
* RETURNS:     number of configured lines
*
* NOTE: Function needs to change if multiple voice profiles are supported
*
****************************************************************************/
static int getNumLinesPerSp( int spNum )
{
   int numAcc;

   if ( dalVoice_GetNumAccPerSrvProv( spNum, &numAcc ) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Could not get max line value for sp: %d. Hard-coding to 0 lines! \n", spNum );
      return 0;
   }
   else
   {
      return numAcc;
   }
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

static CmsRet getEptAppStatus(DAL_VOICE_PARMS *parms, char *data, unsigned int length )
{
   snprintf(data, length, "0");
#ifdef EPTAPPLOAD
   snprintf(data, length, "1");
#endif
   return CMSRET_SUCCESS;
}

/****************************************************************************
* FUNCTION:    getVoiceProvImpl
*
* PURPOSE:     Get the voice provisioning implementation
*
* PARAMETERS:  provImpl - Writes "1" for TR-104v1, "2" for TR-104v2
*
**RETURNS:     CMSRET_SUCCESS
*
* NOTE:
*
****************************************************************************/
static CmsRet getVoiceProvImpl(DAL_VOICE_PARMS *parms, char *provImpl, unsigned int length )
{
   snprintf( (char*)provImpl, length, "1" );

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
#ifdef BRCM_SIP_CCTK_IMS
   snprintf( (char*)sipMode, length, "IMS" );
#else
   snprintf( (char*)sipMode, length, "RFC3261" );
#endif

   return CMSRET_SUCCESS;
}

#endif /* BRCM_VOICE_SUPPORT */


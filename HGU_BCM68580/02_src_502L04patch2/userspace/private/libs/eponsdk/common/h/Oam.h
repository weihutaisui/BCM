/*
* <:copyright-BRCM:2013:proprietary:epon
* 
*    Copyright (c) 2013 Broadcom 
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
*/


#if !defined(Oam_h)
#define Oam_h

/*
// \file Oam.h
// \brief Definitions for IEEE 802.3ah OAM protocol
//
// These definitions match draft 1.414 of IEEE 802.3ah.
//
*/
#include "Teknovus.h"
#include "Ethernet.h"

#if defined(__cplusplus)
extern "C" {
#endif

extern EthernetFrame *TxFrame;
extern EthernetFrame *RxFrame;
extern U16 rxFrameLength;

#define MaxPktSize 1536

/* pack structures for ARM */
#if defined(CPU_ARCH_ARM) || defined(CPU_ARCH_X86)
#pragma pack (1)
#endif

#if !defined(OAM_VERSION)
#define OAM_VERSION 320
#endif

#define OamMaxTlvLength 128
#define OamMultiPartLastMask  0x8000
/* An IEEE Organization Unique Identifier */
typedef struct
{
    U8  byte[3];
} PACK IeeeOui;

/* An IETF Internet Assigned Number Authority Enterprise ID */
typedef U32 IanaEnterprise;


typedef enum
{
    LacpSlowProtocolSubtype     = 0x01,     /* Link Agregation Control Protocol */
    LampSlowProtocolSubtype     = 0x02,     /* Link Agregation Marker Protocol */
    OamSlowProtocolSubtype         = 0x03,
    Oam8021asProtocolSubtype       = 0x0A,
    OrgSpecSlowProtocolSubtype     = 0x0A   // Organization Specific Subtype
} PACK SlowProtocolSubtype;


/* standard OAM opcodes */
typedef enum
{
    OamOpInfo,
    OamOpEventNotification,
    OamOpVarRequest,
    OamOpVarResponse,
    OamOpLoopback,
    OamLegacyOpVendorExt     = 0x80,
    OamLegacyOpPingRequest   = 0x8B,
    OamLegacyOpPingResponse  = 0x8C,
    OamOpVendorOui           = 0xfe
} PACK OamOpcode;

/* common structure for all OAM PDUs */
typedef struct
{
    U8  subtype;       /* should be 0x03 for OAM */
    U16 flags;      
    U8  opcode;
} PACK OamMsg;

/// common structure for all OAM PDUs
typedef struct
{
    U8  subtype;     ///< should be 0x03 for OAM
    U16 flags;
}PACK OamSubtypeFlags;
	
typedef struct
{
    OamMsg OamHead;		// Excluding SA,DA,EtherType, the last field is "OpCode"
    U8     OamData[1];
} PACK OamPdu;

typedef struct
{
    EthernetFrame EthHeader;
    OamPdu        Data;
} PACK OamFrame;


#define OamMsgFlagLinkFault     	0x0001
#define OamMsgFlagDyingGasp     	0x0002
#define OamMsgFlagCriticalEvent 	0x0004

#define OamMsgFlagLocalEval     0x0008
#define OamMsgFlagLocalStable   0x0010
#define OamMsgFlagRemoteEval    0x0020
#define OamMsgFlagRemoteStable  0x0040

#if OAM_VERSION < 200
// no extra PDU flags fields; see Info TLV state

#elif OAM_VERSION < 220

#define OamMsgFlagLocalDiscMsk  0x0010
#define OamMsgFlagLocalDiscSft  4
#define OamMsgFlagRemoteDiscMsk 0x0008
#define OamMsgFlagRemoteDiscSft 3
// values for Local/RemoteDisc fields
#define OamMsgFlagDiscFailed        0
#define OamMsgFlagDiscUnsatisfied   0
#define OamMsgFlagDiscComplete      1
#define OamMsgFlagDiscInvalid       2  // not 0 or 1, never actually happens

#elif OAM_VERSION <= 320

#define OamMsgFlagLocalDiscMsk  	0x0018
#define OamMsgFlagLocalDiscSft  	3
#define OamMsgFlagRemoteDiscMsk 	0x0060
#define OamMsgFlagRemoteDiscSft 	5

/* values for Local/RemoteDisc fields */
#define OamMsgFlagDiscFailed        0
#define OamMsgFlagDiscUnsatisfied   1
#define OamMsgFlagDiscComplete      2
#define OamMsgFlagDiscInvalid       3

#else
#error "Unsupported OAM OAM_VERSION"
#endif

/*
// Info PDU
*/
/* Info PDUs contain one or two TLVs following the common msg header */

typedef enum
{
    OamTlvNull              = 0x00,
    OamTlvLocalInfo         = 0x01,
    OamTlvRemoteInfo        = 0x02,
    OamTlvOrgSpecific       = 0xfe
} PACK OamTlvType;


typedef struct
{
    U8 flags;
} PACK OamInfoState;

#if OAM_VERSION < 200

#define OamInfoStateFlagStable      0x0001
#define OamInfoStateActionMsk       0x0006
#define OamInfoStateActionSft       1
#define OamInfoStateMuxDiscard      0x0008

#elif OAM_VERSION <= 310

#define OamInfoStateActionMsk       0x0006
#define OamInfoStateActionSft       1
#define OamInfoStateMuxDiscard      0x0008

#elif OAM_VERSION <= 320

#define OamInfoStateActionMsk       0x0003
#define OamInfoStateActionSft       0
#define OamInfoStateMuxDiscard      0x0004

#else
#error "Unsupported OAM OAM_VERSION"
#endif // OAM_VERSION
/// possible values for the OAM info state flags action field
typedef enum
{
    OamInfoStateActionForward,
    OamInfoStateActionLoopback,
    OamInfoStateActionDiscard
}PACK OamInfoStateAction;

/* IEEE 802.3ah ID information */
typedef struct
{
    IeeeOui   vendor;
    U16       product;    /*
< format up to the vendor */
    U16       version;    /*
< format up to the vendor */
} PACK OamId;

#if OAM_VERSION == 320

typedef struct
{
    U8              type;
    U8              length;
    U8              version;
    U16             revision;
    OamInfoState    state;
    U8              configuration;
    U16             pduConfiguration;
    OamId           id;
} PACK OamInfoTlv;

typedef struct
{
    OamTlvType      type;
    U8              length;
    IeeeOui         oui;
    U8              vendorData[1]; // depends on OUI
} PACK OamInfoVendorTlv;

#else
#error "Unsupported OAM OAM_VERSION"
#endif /* OAM_VERSION */

#define OamInfoVersion              1

/* OAM configuration bits */
#define OamInfoConfigModeActive     0x0001
#define OamInfoConfigUnidir         0x0002
#define OamInfoConfigLoopCapable    0x0004
#define OamInfoConfigLoopEvents     0x0008
#define OamInfoConfigVarResp        0x0010

/* OAMPDU configuration bits */
#define OamInfoPduConfigSizeMsk     0x07ff
#define OamInfoPduConfigSizeSft     0


/* identifies which type of TK-extended TLV this is */
typedef enum
{
    TkInfoTlvLink,             /*
< EPON laser parameters */
    TkInfoTlvKey,              /*
< Key exchange */
    TkInfoTlvAlarmRequest,     /*
< Alarm audit request */
	TkInfoTlvTeknovus,		   /*
< Identify OLT/ONU as Teknovus */

    TkInfoTlvNumTlvs
} PACK OamInfoTkTlvType;

/* Teknovus-specific TLV header */
typedef struct
{
    U8          type;       /*
< FE for org-specific extension */
    U8          length;     /*
< includes type/len bytes */
    IeeeOui     oui;        /*
< 00-0D-B6 for Teknovus */
    U8          tkType;
} PACK OamInfoTkTlvHeader;

/* TkInfoTlvLink: EPON overhead data */
typedef struct
{
    U16     burstCap;
    U16     minOam;
    U16     maxOam;
    U16     laserOn;
    U16     laserOff;
} PACK OamInfoTkLink;

/* TkInfoTlvKey: key exchange during discovery */
/* followed by keyLength bytes of key data, MSB first */
typedef struct
{
    U8  keyNumber;
    U8  keyLength;
} PACK OamInfoTkKey;

/* TkInfoTlvAlarmRequest: request alarm message */
/* no parameters */

/* OAM Info msg */
typedef struct
{
    OamMsg common;
} PACK OamInfoMsg;


/*
// Event Notification
*/

/* events have a number of event TLVs following the message header */

typedef struct
{                         
    OamMsg  common;
    U16     seqNum;
} PACK OamEventNotificationMsg;

/* Event Notification TLVs */

typedef enum
{
    OamEventEndOfTlvMarker			= 0,
    OamEventErrSymbolPeriod         = 1,
    OamEventErrFrameCount           = 2,
    OamEventErrFramePeriod          = 3,
    OamEventErrFrameSecSummary      = 4,
    OamEventErrVendor               = 0xfe,
    OamEventErrVendorOld            = 0xff
} PACK OamEventType;

/* All TLVs start like this */
typedef struct
{
    U8  type;
    U8  length;
} PACK OamEventTlv;

/*  OAM errored symbol period */
typedef struct
{
    OamEventTlv     tlv;
    U16             timestamp;      /* < 100 ms intervals */
    U64             window;         /* < time, in number of symbols (1s) */
    U64             threshold;      /* < min errors to cause event (1) */
    U64             errors;         /* < errors this period */
    U64             runningTotal;   /* < all errors since reset */
    U32             eventCount;     /* < number of these events generated */
} PACK OamEventErrSymPeriod;

/*  OAM errored frame  */
typedef struct
{
    OamEventTlv     tlv;
    U16             timestamp;      /* < 100 ms intervals */
    U16             window;         /* < time, in 100ms intervals (1s) */
    U32             threshold;      /* < min errors to cause event (1) */
    U32             errors;         /* < errors this period */
    U64             runningTotal;   /* < all errors since reset */
    U32             eventCount;     /* < number of these events generated */
} PACK OamEventErrFrame;

/*  OAM errored frame period */
typedef struct
{
    OamEventTlv     tlv;
    U16             timestamp;      /* < 100 ms intervals */
    U32             window;         /* < time, in min size frame times (1s) */
    U32             threshold;      /* < min errors to cause event (1) */
    U32             errors;         /* < errors this period */
    U64             runningTotal;   /* < all errors since reset */
    U32             eventCount;     /* < number of these events generated */
} PACK OamEventErrFrPeriod;

/*  OAM errored frame seconds summary */
typedef struct
{
    OamEventTlv     tlv;
    U16             timestamp;      /* < 100 ms intervals */
    U16             window;         /* < time, in min size frame times (60s) */
    U16             threshold;      /* < min errors to cause event(1) */
    U16             errors;         /* < errors this period */
    U32             runningTotal;   /* < all errors since reset */
    U32             eventCount;     /* < number of these events generated */
} PACK OamEventErrFrSecondsSum;

/*  Vendor-extended TLV */
typedef struct
{
    OamEventTlv     tlv;
    IeeeOui         oui;
} PACK OamEventExt;


/*
 * Variable Request/Response    
 */

/*  Allowed values for "branch" */
typedef enum 
{
	OamBranchTermination	            = 0x00,
    OamBranchObject                     = 0x03,
	OamBranchPackage 		            = 0x04,
	OamBranchNameBinding	            = 0x06,
	OamBranchAttribute 	                = 0x07,
    OamBranchAction                     = 0x09,
    OamBranchBrcmNameBinding            = 0x46,
    OamBranchBrcmAttr                   = 0x47,
    OamBranchForce8                     = 0xFF
} PACK OamVarBranch;

/*  Leaf values for branch == OamBranchObject */
typedef enum
{
    OamObjLeafMacEntity                 = 0x01,
    OamObjLeafPhyEntity                 = 0x02,
    OamObjLeafMacCtrl                   = 0x08

#if OAM_VERSION == 320
    ,
    OamObjForce16                       = 0x7fff
#else
#error "Unsupported OAM OAM_VERSION"
#endif
} PACK OamObjLeaf;

/*  Leaf values for branch == OamBranchPackage	 */
typedef enum 
{
	OamPkgLeafIllegal       			= 0x00,
	OamPkgLeafMacMandatory 			    = 0x01,
	OamPkgLeafMacRecommended 			= 0x02,
	OamPkgLeafMacOptional			    = 0x03,
	OamPkgLeafMacArray				    = 0x04,
	OamPkgLeafMacExcessiveDeferral 	    = 0x05,
	OamPkgLeafPhyRecommended			= 0x06,
	OamPkgLeafPhyMultiplePhy			= 0x07,
    OamPkgLeafPhy100MpbsMonitor         = 0x08,
	OamPkgLeafRepeaterPerfMonitor	    = 0x09,
	OamPkgLeafPortPerfMonitor		    = 0x0a,
	OamPkgLeafPortAddrTrack			    = 0x0b,
	OamPkgLeafMauControl				= 0x0d,
	OamPkgLeafMediaLowTracking		    = 0x0e,
	OamPkgLeafBroadbandMau			    = 0x0f,
	OamPkgLeafMacControlRecommended	    = 0x11

#if OAM_VERSION == 320
    ,
    OamPkgForce16                       = 0x7fff
#else
#error "Unsupported OAM OAM_VERSION"
#endif
} PACK OamPkgLeaf;


/*  Leaf values for branch == OamBranchNameBinding */
typedef enum
{
    OamNameMacName                      = 1,
    OamNameMacMonitor                   = 2,
	OamNameQueueName                    = 2, /* duplicate value OK */
    OamNamePhyName                      = 3,
    OamNamePhyMonitor                   = 4,
    OamNameAllQueues                    = 4, /* reused value */
    OamNameMacCtrlSystem                = 18,
    OamNameMacCtrlMonitor               = 15,
#if 0
    OamNameRepeaterName                 = 5,
    OamNameRepeaterMonitor              = 6,
    OamNameGroupName                    = 7,
    OamNamePortName                     = 8,
#endif
    OamNameMauRptName                   = 9,
    OamNameMauDteName                   = 10,

    OamNameRsrcMac                      = 12,
    OamNameRsrcRepeater                 = 13,
    OamNameRsrcMau                      = 14,

    OamNameNetworkPon                   = 5,
    OamNameUserPon                      = 6,
    OamNameFlowDirection                = 7

#if OAM_VERSION == 320
    ,
    OamNameForce16                      = 0x7fff
#else
#error "Unsupported OAM OAM_VERSION"
#endif
} PACK OamNameLeaf;

/*  Leaf values for branch == OamBranchAttribute */
typedef enum
{
    OamAttrLeafIllegal                  = 0x00,

    /* MAC attributes */
    OamAttrMacId                        = 1,
    OamAttrMacFramesTxOk                = 2,
    OamAttrMacSingleCollFrames          = 3,
    OamAttrMacMultipleCollFrames        = 4,
    OamAttrMacFramesRxOk                = 5,
    OamAttrMacFcsErr                    = 6,
    OamAttrMacAlignErr                  = 7,
    OamAttrMacOctetsTxOk                = 8,
    OamAttrMacFramesDeferred            = 9,
    OamAttrMacLateCollisions            = 10,
    OamAttrMacExcessiveCollisions       = 11,
    OamAttrMacFramesLostMacTxErr        = 12,
    OamAttrMacCarrierSenseErr           = 13,
    OamAttrMacOctetsRxOk                = 14,
    OamAttrMacFramesLostMacRxErr        = 15,
    OamAttrMacPromiscuousStatus         = 16,
    OamAttrMacMcastAddrList             = 17,
    OamAttrMacMcastFramesTxOk           = 18,
    OamAttrMacBcastFramesTxOk           = 19,
    OamAttrMacFrExcessiveDeferral       = 20,
    OamAttrMacMcastFramesRxOk           = 21,
    OamAttrMacBcastFramesRxOk           = 22,
    OamAttrMacInRangeLenErr             = 23,
    OamAttrMacOutOfRangeLenErr          = 24,
    OamAttrMacFrameTooLong              = 25,
    OamAttrMacEnableStatus              = 26,
    OamAttrMacTxEnable                  = 27,
    OamAttrMacMcastRxStatus             = 28,
    OamAttrMacAddr                      = 29,
    OamAttrMacCollisonFrames            = 30,

	/* RSTP related definitions */
	/* WARNING: The leaf values overlap with previously defined values and need */
	/* to be interpreted based on the context. In the case of RSTP, it is the */
	/* bridge/bridge-port context */
	OamAttrBridgeAddress                = 0x00,
	OamAttrBridgeNumPorts               = 0x02,
	OamAttrBridgePortAddresses	        = 0x03,
	OamAttrRstpBridgeIdentifier         = 0x05,
	OamAttrRstpTimeSinceTopologyChange  = 0x06,
	OamAttrRstpTopologyChangeCount      = 0x07,
	OamAttrRstpTopologyChange           = 0x08,
	OamAttrRstpDesignatedRoot           = 0x09,
	OamAttrRstpRootPathCost             = 0x0A,
	OamAttrRstpRootPort                 = 0x0B,
	OamAttrRstpMaxAge                   = 0x0C,
	OamAttrRstpHelloTime                = 0x0D,
	OamAttrRstpForwardDelay             = 0x0E,
	OamAttrRstpBridgeMaxAge             = 0x0F,
	OamAttrRstpBridgeHelloTime          = 0x10,
	OamAttrRstpBridgeForwardDelay       = 0x11,
	OamAttrRstpHoldTime                 = 0x12,
	OamAttrRstpBridgePriority           = 0x13,

	OamAttrRstpPortIdentifier           = 0x22,
	OamAttrRstpPortPriority             = 0x23,
	OamAttrRstpPathCost                 = 0x24,
	OamAttrRstpPortDesignatedRoot       = 0x25,
	OamAttrRstpDesignatedCost           = 0x26,
	OamAttrRstpDesignatedBridge         = 0x27,
	OamAttrRstpDesignatedPort           = 0x28,
	OamAttrRstpPortState                = 0x33,

    /* PHY */
    OamAttrPhyId                        = 31,
    OamAttrPhyType                      = 32,
    OamAttrPhyTypeList                  = 33,
    OamAttrPhySqeTestErr                = 34,
    OamAttrPhySymbolErrDuringCarrier    = 35,
    OamAttrPhyMiiDetect                 = 36,
    OamAttrPhyAdminState                = 37,

    /* MAU attributes */
    OamAttrMauId                        = 68,
    OamAttrMauType                      = 69,
    OamAttrMauTypeList                  = 70,
    OamAttrMauMediaAvail                = 71,
    OamAttrLoseMediaCtr                 = 72,
    OamAttrJabber                       = 73,
    OamAttrMauAdminState                = 74,
    OamAttrMauBandSplitType             = 75,
    OamAttrMauBandSplitFreq             = 76,
    OamAttrMauFalseCarriers             = 77,

    /* Auto-Neg attributes */
    OamAttrAutoNegId                    = 78,
    OamAttrAutoNegAdminState            = 79,
    OamAttrAutoNegRemoteSig             = 80,
    OamAttrAutoNegAutoCfg               = 81,
    OamAttrAutoNegLocalTech             = 82,
    OamAttrAutoNegAdTech                = 83,
    OamAttrAutoNegRxTech                = 84,
    OamAttrAutoNegLocalSelectAble       = 85,
    OamAttrAutoNegAdSelectAble          = 86,
    OamAttrAutoNegRxSelectAble          = 87,

    /* MAC */
    OamAttrMacCapabilities              = 89,
    OamAttrMacDuplexStatus              = 90,

    /* MAU */
    OamAttrMauIdleErrorCount            = 91,

    /* DTE MAC Control */
    OamAttrMacCtrlId                    = 92,
    OamAttrMacCtrlFuncsSupported        = 93,
    OamAttrMacCtrlFramesTx              = 94,
    OamAttrMacCtrlFramesRx              = 95,
    OamAttrMacCtrlUnsupportedOpRx       = 96,
    OamAttrMacCtrlPauseDelay            = 97,
    OamAttrMacCtrlPauseTx               = 98,
    OamAttrMacCtrlPauseRx               = 99,

    /* Aggregate */
    /* 101 - 132 */

    /* aggregation port attributes */
    /* 133 - 156 */

    /* agg port stats */
    /* 157 - 165   */

    /* agg port debug info */
    /* 166 - 178 */

    /* 802.3ah, new stuff, no official registration arcs yet */

    /* OAM */
    OamAttrOamId                         = 200,
    OamAttrOamAdminState                 = 201,
    OamAttrOamMode                       = 202,
    OamAttrOamRemoteMacAddr              = 203,
    OamAttrOamRemoteConfig               = 204,
    OamAttrOamRemotePduConfig            = 205,
    OamAttrOamLocalFlags                 = 206,
    OamAttrOamRemoteFlags                = 207,
    OamAttrOamRemoteState                = 208,
    OamAttrOamRemoteVendorOui            = 209,
    OamAttrOamRemoteVendorDevice         = 210,
    OamAttrOamRemoteVendorVersion        = 211,
    OamAttrOamPduTx                      = 212,
    OamAttrOamPduRx                      = 213,
    OamAttrOamUnsupportedOpcodes         = 214,

    OamAttrOamInfoTx                     = 215,
    OamAttrOamInfoRx                     = 216,
    OamAttrOamEventTx                    = 217,
    OamAttrOamUniqueEventRx              = 218,
    OamAttrOamDupEventRx                 = 219,
    OamAttrOamLoopTx                     = 220,
    OamAttrOamLoopRx                     = 221,
    OamAttrOamVarReqTx                   = 222,
    OamAttrOamVarReqRx                   = 223,
    OamAttrOamVarRespTx                  = 224,
    OamAttrOamVarRespRx                  = 225,
    OamAttrOamOrgSpecificTx              = 226,
    OamAttrOamOrgSpecificRx              = 227,

    OamAttrOamLocalErrSymPeriodWin       = 228,
    OamAttrOamLocalErrSymPeriodThresh    = 229,
    OamAttrOamLocalErrSymPeriodEvent     = 230,

    OamAttrOamLocalErrFrameSecsWin       = 231,
    OamAttrOamLocalErrFrameSecsThresh    = 232,
    OamAttrOamLocalErrFrameSecsEvent     = 233,

    OamAttrOamLocalErrFramePeriodWin     = 234,
    OamAttrOamLocalErrFramePeriodThresh  = 235,
    OamAttrOamLocalErrFramePeriodEvent   = 236,

    OamAttrOamLocalErrFrSecSumWin        = 237,
    OamAttrOamLocalErrFrSecSumThresh     = 238,
    OamAttrOamLocalErrFrSecSumEvent      = 239,

    OamAttrOamRemoteErrFrameSecsWin      = 240,
    OamAttrOamRemoteErrFrameSecsThresh   = 241,
    OamAttrOamRemoteErrFrameSecsEvent    = 242,

    OamAttrOamRemoteErrFramePeriodWin    = 243,
    OamAttrOamRemoteErrFramePeriodThresh = 244,
    OamAttrOamRemoteErrFramePeriodEvent  = 245,
    
    OamAttrOamFramesLostOamErr           = 246,

    /* OMP Muxing */
    /* no attrs yet defined */

    /* OMP Emulation */
    OamAttrOamEmulId                     = 247,
    OamAttrOamEmulSpdErr                 = 248,
    OamAttrOamEmulCrc8Err                = 249,
    OamAttrOamEmulBadLlidErr             = 250,

    OamAttrMpcpMACCtrlFramesTx           = 280,
    OamAttrMpcpMACCtrlFramesRx           = 281,
    OamAttrMpcpDiscoveryWindowTx         = 288,
    OamAttrMpcpDiscoveryTimeout          = 290,
	OamAttrFecCorrectedBlocks            = 292,
	OamAttrFecUncorrectableBlocks        = 293,
	OamAttrFecAbility                    = 313,
	OamAttrFecMode                       = 314,
    OamAttrMpcpTxGate                    = 315,
    OamAttrMpcpTxRegAck                  = 316,
    OamAttrMpcpTxRegister                = 317,
    OamAttrMpcpTxRegRequest              = 318,
    OamAttrMpcpTxReport                  = 319,
    OamAttrMpcpRxGate                    = 320,
    OamAttrMpcpRxRegAck                  = 321,
    OamAttrMpcpRxRegister                = 322,
    OamAttrMpcpRxRegRequest              = 323,
    OamAttrMpcpRxReport                  = 324, // 0x144

    OamAttrLeafLast

#if OAM_VERSION == 320
    ,
    OamAttrForce16                       = 0x7fff
#else
#error "Unsupported OAM OAM_VERSION"
#endif
} PACK OamAttrLeaf;


/* Enumerated types for attributes */

typedef enum
{
    OamFalse,
    OamTrue
} PACK OamTrueFalse;

typedef enum
{
	OamStateDisabled						= 1,
	OamStateEnabled							= 2
} PACK OamEnableStatus;

typedef enum
{
    OamFlowDirectionIngress                 = 0,
    OamFlowDirectionEgress                  = 1,
    OamFlowDirectionUpstream                = 2,
    OamFlowDirectionDownstream              = 3,
    OamFlowDirectionEgressLocal             = 4,
} PACK OamFlowDirection;
typedef enum
{
    OamMacDuplexHalf                        = 1,
    OamMacDuplexFull,
    OamMacDuplexUnknown
} PACK OamMacDuplexStatus;

typedef enum
{
    OamPhyTypeOther                         = 1,
    OamPhyTypeUnknown                       = 2,
    OamPhyTypeNone                          = 3,
    OamPhyType10                            = 7,
    OamPhyType100T4                         = 23,
    OamPhyType100X                          = 24,
    OamPhyType100T2                         = 32,
    OamPhyType1000X                         = 36,
    OamPhyType1000T                         = 40,
    OamPhyType10GX                          = 48,
    OamPhyType10GR                          = 49,
    OamPhyType10W                           = 50,
} PACK OamPhyType;

typedef enum
{
    OamMauMediaAvailOther                   = 1,
    OamMauMediaAvailUnknown                 = 2,
    OamMauMediaAvailAvailable               = 3,
    OamMauMediaAvailNotAvailable            = 4,
    OamMauMediaAvailRemoteFault             = 5,
    OamMauMediaAvailInvSignal               = 6,
    OamMauMediaAvailRemoteJabber            = 7,
    OamMauMediaAvailRemoteLinkLoss          = 8,
    OamMauMediaAvailRemoteTest              = 9,
    OamMauMediaAvailOffline                 = 10,
    OamMauMediaAvailAutoNegError            = 11
} PACK OamMauMediaAvail;

typedef enum
{
    OamAutoCapGlobal                        = 0,
    OamAutoCapOther                         = 1,    
    OamAutoCapUnknown                       = 2,
    OamAutoCap10T                           = 14,
    OamAutoCap10TFD                         = 142,
    OamAutoCap100T4                         = 23,
    OamAutoCap100TX                         = 25,
    OamAutoCap100TXFD                       = 252,
    OamAutoCapFdxPause                      = 312,
    OamAutoCapFdxAPause                     = 313,
    OamAutoCapFdxSPause                     = 314,
    OamAutoCapFdxBPause                     = 315,
    OamAutoCap100T2                         = 32,
    OamAutoCap100T2FD                       = 322,
    OamAutoCap1000X                         = 36,
    OamAutoCap1000XFD                       = 362,
    OamAutoCap1000T                         = 40,
    OamAutoCap1000TFD                       = 402,
    OamAutoCapRemFault1                     = 37,
    OamAutoCapRemFault2                     = 372,
    OamAutoCap10GBaseX                      = 48,
    OamAutoCap10GBaseLX4                    = 481,
    OamAutoCap10GBaseR                      = 49,
    OamAutoCap10GBaseER                     = 491,
    OamAutoCap10GBaseLR                     = 492,
    OamAutoCap10GBaseSR                     = 493,
    OamAutoCap10GBaseW                      = 50,
    OamAutoCap10GBaseEW                     = 501,
    OamAutoCap10GBaseLW                     = 502,
    OamAutoCap10GBaseSW                     = 503,
    OamAutoCapIsoEthernet                   = 8029,
} PACK OamAutoNegCapability;

typedef enum
{
	OamPortHalfDuplex						= 0x0001,
	OamPortFullDuplex						= 0x0002,
	OamPort10Speed							= 0x0004,
	OamPort100Speed							= 0x0008,
	OamPort1GSpeed							= 0x0010,
	OamPort10GSpeed							= 0x0020,
	OamPortFlowControl					   	= 0x0040,
	OamPortAutoMDIMDIX					   	= 0x0080,
	OamPort2GSpeed							= 0x8000,
	OamPortMaxCap						   	= 0xFFFF
} PACK OamPortCapability;

typedef enum
{
    OamAutoAdminDisabled                    = 1,
    OamAutoAdminEnabled             
} PACK OamAutoNegAdminState;

typedef enum
    {
    OamAutoConfigOther                      = 1,
    OamAutoConfigConfiguring,
    OamAutoConfigComplete,
    OamAutoConfigDisabled,
    OamAutoConfigParallelFail    
} PACK OamAutoNegAutoConfig;

typedef enum
{
    OamAutoSelectOther                      = 1,
    OamAutoSelectEthernet,                          /* 802.3 */
    OamAutoSelectIsoEthernet                        /* 802.9 */
} PACK OamAutoSelector;

typedef enum
{
    OamAutoRemoteDetected                   = 1,
    OamAutoRemoteNotDetected           
} PACK OamAutoRemoteSig;

typedef enum
{
    OamMacCtrlFuncPause                     = 312
} PACK OamMacCtrlFuncs;


// The definition is found in the IEEE Std 802.3-2005 in section 30.5.1.1.13
// "aFECAbility".
typedef enum
{
    FecAbilityUnknown       = 1,
    FecAbilitySupported     = 2,
    FecAbilityNotSupported  = 3
} PACK OamFecAbility;
// In case you are wondering were on earth these values came from I'll tell you,
// IEEE Std 802.3-2005 Section 2 Annex 30B.2 "ANS.1 module for CSMA/CD managed
// objects" page 719.
typedef enum
{
    FecModeUnknown          = 1,
    FecModeEnabled          = 2,
    FecModeDisabled         = 3
} PACK OamFecMode;
/* Actions */

typedef enum
{
    OamActMacInit                           = 1,
    OamActMacAddGroupAddr                   = 2,
    OamActMacDelGroupAddr                   = 3,
    OamActMacSelfTest                       = 4,

    OamActPhyAdminControl                   = 5,

    OamActRptReset                          = 6,
    OamActRptInServiceTest                  = 7,

    OamActPortAdminCtrl                     = 8,

    OamActMauReset                          = 9,
    OamActMauAdminCtrl                      = 10,

    OamActAutoRenegotiate                   = 11,
    OamActAutoAdminCtrl                     = 12,

    OamActionLeafLast
#if OAM_VERSION == 320
    ,
    OamActForce16                           = 0x7fff
#else
#error "Unsupported OAM OAM_VERSION"
#endif
} PACK OamActionLeaf;


/// Variable Descriptor, as found in OAM Var Request msgs
typedef struct
{
    U8  branch;
#if OAM_VERSION == 320
    U16 leaf;
#else
#error "Unsupported OAM OAM_VERSION"
#endif
} PACK OamVarDesc;

typedef struct
{
    OamVarBranch  Branch;
	U16           Leaf;
	U8            Width;
	U8          * pValue;
} PACK tGenOamVar;

/*  Variable Descriptor, as found in OAM Var Request msgs */
typedef struct
{
    U8  branch;
#if OAM_VERSION == 320
    U16 leaf;
#else
#error "Unsupported OAM OAM_VERSION"
#endif
    U8 ackVal;
} PACK OamSetRespDesc;

/*  Variable Container error codes, which occur in place of the length field */
typedef enum
{
#if OAM_VERSION == 320

    OamVarErrReserved                   = 0x80,
    OamVarErrNoError                    = 0x80,
    OamVarErrTooLong                    = 0x81,

    /* Teknovus extended error codes */
    OamVarErrAttrToBeContinued          = 0x85,
    OamVarErrActBadParameters           = 0x86,
    OamVarErrActNoResources             = 0x87,
    OamVarErrAttrSystemBusy             = 0x88,

    /* standard attribute error codes */
    OamVarErrAttrUndetermined           = 0xa0,
    OamVarErrAttrUnsupported            = 0xa1,
    OamVarErrAttrMayBeCorrupted         = 0xa2,
    OamVarErrAttrHwFailure              = 0xa3,
    OamVarErrAttrOverflow               = 0xa4,

    /* standard object error codes */
    OamVarErrObjEnd                     = 0xc0,
    OamVarErrObjUndetermined            = 0xc1,
    OamVarErrObjUnsupported             = 0xc2,
    OamVarErrObjMayBeCorrupted          = 0xc3,
    OamVarErrObjHwFailure               = 0xc4,

    /* standard package error codes */
    OamVarErrPkgEnd                     = 0xe0,
    OamVarErrPkgUndetermined            = 0xe1,
    OamVarErrPkgUnsupported             = 0xe2,
    OamVarErrPkgMayBeCorrupted          = 0xe3,
    OamVarErrPkgHwFailure               = 0xe4,
    
    OamVarErrUnknow                     = 0xf0, /* Added for ONU SDK by Qiang Jiang */
    SdkNoResource                       = 0xf1
#else
#error "Unsupported OAM OAM_VERSION"
#endif

} PACK OamVarErrorCode;

/*  Variable Container, as found in OAM Var Response msgs */
typedef struct
{
    U8  branch;
#if OAM_VERSION == 320
    U16 leaf;
#else
#error "Unsupported OAM OAM_VERSION"
#endif
    U8  length;
    U8  value[1];    /* actually length bytes long */
} PACK OamVarContainer;    


#define OamVarContMaxLength ((sizeof(OamVarContainer) - 1) + OamMaxTlvLength)


/// Var Request
typedef struct
{
    OamMsg common;
} PACK OamVarReqMsg;

/*  Var Response */
typedef struct
{
    OamMsg common;
} PACK OamVarRespMsg;


/*
// Loopback
*/

typedef enum
{
    OamLoopCmdEnable   = 1,
    OamLoopCmdDisable  = 2
} PACK OamLoopCmd;

typedef struct
{
    OamMsg  common;
    U8      cmd;
} PACK OamLoopbackMsg;


/*
// Spanning Tree
*/

typedef enum
    {
	OamRstpBridgeModeDisabled		= 0,
	OamRstpBridgeModePassThrough	= 1,
	OamRstpBridgeModeNormal			= 2
    } PACK OamRstpBridgeMode;

typedef enum
    {
    OamRstpPortDisabled		= 1,
    OamRstpPortBlocking		= 2,
    OamRstpPortListening	= 3,
    OamRstpPortLearning		= 4,
    OamRstpPortForwarding	= 5,
    OamRstpPortBroken		= 6
    } PACK OamRstpPortState;


/*
// Vendor Extension
*/

/* Vendor-extended message types (opcodes) */

/*  OUI-type vendor extension header */
typedef struct
    {
    OamMsg common;
    IeeeOui oui;
    /* the remainer of the vendor extension PDU is unspecified, up */
    /* to the vendor with the given OUI */
    } PACK OamOuiVendorExt;

typedef struct
{
    OamOpcode   opcode;
    IeeeOui     oui;
} PACK OamVendorExt;

/// Teknovus OUI extended opcodes
typedef enum
    {
    OamExtOpInfo,
    OamExtOpVarRequest,
    OamExtOpVarResponse,
    OamExtOpVarSet,
    OamExtOpVarSetResponse,

    /* multicast */
    OamExtOpMcastRequest,
    OamExtOpMcastReg,
    OamExtOpMcastRegResponse,

    /* encryption */
    OamExtOpKeyExchange,

    /* file transfer */
    OamExtOpFileRdReq,
    OamExtOpFileWrReq,
    OamExtOpFileData,
    OamExtOpFileAck,

    OamExtOpI2cRdReq,
    OamExtOpI2cRdResp,
    OamExtOpI2cWrReq,
    OamExtOpI2cWrResp,

    OamExtOpUnackedAction,

	/* Loop Detect */
	OamExtOpLoopDetect,

    OamExtOpOsAccessRdReq,
    OamExtOpOsAccessRdResp,
    OamExtOpOsAccessWrReq,
    OamExtOpOsAccessWrResp,

    // 802.1AS
    OamExtOp802Dot1,

    OamExtNumOpcodes
    } PACK OamExtOpcode;

/*  Teknovus extended message common format */
typedef struct
    {
    U8 opcode;
    } PACK OamTkExt;

typedef enum
	{
	SimpleReportMode = 0,
	TeknovusReportMode = 1<<0,
	CtcReportMode = 1<<1,
	NttReportMode = 1<<2,
    ManualReportMode = 1<<3,
    TdmNoForceReportMode = 1<<4
	} PACK EponReportModes;


typedef struct
	{
	U8 op; /* 0xFE -- vendor specific */
	U8 length; /* length of TLV; */
	IeeeOui oui; /* set to teknovus OUI.  */
	U8 tlvType; /* Set to TkInfoTlvTeknovus  */
	U8 version; /* Set to 0 */
	EponReportModes supportedReportModes; /* OLT will do these */
	EponReportModes perferredReportModes; /* perferes these */
	} PACK TekOltOamDiscoveryTlv; 

typedef struct
	{
	U8 op; /* 0xFE -- vendor specific */
	U8 length; /* length of TLV; */
	IeeeOui oui; /* set to teknovus OUI.  */
	U8 tlvType; /* Set to TkInfoTlvTeknovus  */
	U8 version;
	EponReportModes currentReportMode; /* mode ONU is currently reporting  */
	} PACK TekOnuOamDiscoveryTlv; 

typedef struct
	{
	U8 op; /* should be 0xFE */
	U8 length;
	IeeeOui oui; 
	} PACK VendorSpecificInfoOamHeader;

typedef enum
    {
    LaserCapsNone       = 0,
    LaserCapsTxPower    = (1 << 0), // TX power control
    LaserCapsRxPower    = (1 << 1), // RX power control
    LaserCapsTxPower2   = (1 << 2), // TX power control on secondary xcvr
    LaserCapsRxPower2   = (1 << 3)  // RX power control on secondary xcvr
    } PACK OptCtrlLaserCaps;

////////////////////////////////////////////////////////////////////////////////
// Extended Info
////////////////////////////////////////////////////////////////////////////////

typedef struct
    {
    OamTkExt        ext;
    U16             firmwareVersion;    /*  TK firmware version */
    OamId           oamId;              /*  repeat of std info */
    U8              extendedId[64];     /*  as supplied by vendor */
    U8              baseMac[6];         /*  base MAC address for this ONU */

    U8              numLinks;           /*  max number of links supported */
    U8              numPorts;           /*  including the EPON port */

    /* upstream queue info */
    U8              numUpQueues;        /*  number of queues assignable */
    U8              maxUpQueuesPerLink; /*  maximum queues per link */
    U8              upQueueIncrement;   /*  Kbytes per increment */

    /* downstream queue info */
    U8              numDnQueues;        /*  number of queues assignable */
    U8              maxDnQueuesPerLink; /*  maximum queues per link */
    U8              dnQueueIncrement;   /*  Kbytes per increment */

    /* buffer sizes */
    U16             upBuffer;           /*  Kbytes upstream buffer avail */
    U16             dnBuffer;           /*  Kbytes downstream buffer avail */

	U16				jedecId;			/*  Teknovus JEDEC Manufacturer ID */
	U16				chipId;				/*  Chip ID */
	U32				chipVersion;		/*  Chip Version */

    OptCtrlLaserCaps    laserCaps;          /// Laser Capabilities
    PsConfig            psCaps;             /// Protection Switch Capabilities
    } PACK OamTkInfo;

/*
 * Extended Var Request, Response, Set
*/

/* extended var requests / responses have the same format as standard
/@ var req / resp beyond the header; they consist of a series of 
/@ var branch+leaf TLV structures.  They differ in that the values
/@ can also be one of these extended values, in addition to the standard
/@ ones. */

typedef enum
    {
    OamExtBranchFirst            = 0x80
    } PACK OamExtBranch;

typedef enum
    {
    OamExtObjFirst               = 0x80

#if OAM_VERSION < 320
    /* one octet leaf */
#elif OAM_VERSION == 320
    , OamExtObjForce16             = 0x7fff
#else
#error "Unsupported OAM OAM_VERSION"
#endif
    } PACK OamExtObjLeaf;

typedef enum
    {
    OamExtPkgFirst               = 0x80,
    OamExtPkgId                  = 0x80,     /* extended ID information */
    OamExtPkgSizeBins            = 0x81,
    OamExtPkgEponDelayStats      = 0x82

#if OAM_VERSION < 320
    /* one octet leaf */
#elif OAM_VERSION == 320
    ,
    OamExtPkgForce16             = 0x7fff
#else
#error "Unsupported OAM OAM_VERSION"
#endif
    } PACK OamExtPkgLeaf;


/*
 Extended Name Bindings
*/


typedef enum
    {
    OamExtNameFirst              = 0x80,
    OamExtNameLink               = 0x80,
    OamExtNameQueue              = 0x81,
    OamExtNameBridge             = 0x82,
    OamExtNameBridgePort         = 0x83

#if OAM_VERSION < 320
    /* one octet leaf */
#elif OAM_VERSION == 320
    , OamExtNameForce16             = 0x7fff
#else
#error "Unsupported OAM OAM_VERSION"
#endif
    } PACK OamExtNameLeaf;

/*  value used to name a port */
typedef struct
    {
    U16 port;
    } PACK OamNamePort;

/*  value used to name a link: port.link */
typedef struct
    {
    U16 port;
    U16 link;
    } PACK OamNameLink;

/*  value used to name a queue: port.link.queue */
typedef struct
    {
    U16 port;
    U16 link;
    U16 queue;
    } PACK OamNameQueue;

/*  value used to name a bridge */
typedef struct
	{
	U16	bridge;
	} PACK OamNameBridge;

/*  value used to name a bridge port */
typedef struct
	{
	U16	port;
	} PACK OamNameBridgePort;


/*  value used to name a queue: port_link.queue */
typedef struct
	{
	U8 port_link;
	U8 queue;
	} PACK OamNewNameQueue;

typedef union
	{
	OamNewNameQueue	ndest;
	U16				vid_cos;
	} PACK OamNewParam;

typedef U8 OamRuleAttr;
#define	OAMRULE_NONVOLATILE	0
#define	OAMRULE_VOLATILE	1

typedef struct
	{
	/* Currently only Volatile Rule bit 0 (not saved in NVS) */
	OamRuleAttr ruleflag; 
	OamNewParam nparam;
	U8			onureserve;	 	/* Reserved for onu internal use. */
	} PACK OamNameParam;

typedef union
	{
	OamNameQueue 	dest;
	OamNameParam 	new;
	} PACK OamRuleParam;

/*
Extended Attributes
*/
typedef enum
    {
    OamExtAttrFirst                = 0x80,

    /* additional info   */
    OamExtAttrFirmwareVer          = 0x80,
    OamExtAttrExtendedId           = 0x81,

    /* bridging attributes */
    OamExtAttrDynLearnTblSize      = 0x82,
    OamExtAttrDynLearnAgeLimit     = 0x83,

    /* additional link attributes */
    OamExtAttrRxUnicastFrames      = 0x84,
    OamExtAttrTxUnicastFrames      = 0x85,
    OamExtAttrRxFrameTooShort      = 0x86,

    /* Frame size bins, as per RFC 1757 */
    OamExtAttrRxFrame64            = 0x87,
    OamExtAttrRxFrame65_127        = 0x88,
    OamExtAttrRxFrame128_255       = 0x89,
    OamExtAttrRxFrame256_511       = 0x8A,
    OamExtAttrRxFrame512_1023      = 0x8B,
    OamExtAttrRxFrame1024_1518     = 0x8C,
    OamExtAttrRxFrame1519Plus      = 0x8D,

    OamExtAttrTxFrame64            = 0x8E,
    OamExtAttrTxFrame65_127        = 0x8F,
    OamExtAttrTxFrame128_255       = 0x90,
    OamExtAttrTxFrame256_511       = 0x91,
    OamExtAttrTxFrame512_1023      = 0x92,
    OamExtAttrTxFrame1024_1518     = 0x93,
    OamExtAttrTxFrame1519Plus      = 0x94,

    /* addtional per-LLID statistics */
    OamExtAttrTxDelayThreshold     = 0x95,
    OamExtAttrTxDelay              = 0x96,
    OamExtAttrTxFramesDropped      = 0x97,
    OamExtAttrTxBytesDropped       = 0x98,  /* dropped in queue, that is  */
    OamExtAttrTxBytesDelayed       = 0x99,
    OamExtAttrTxBytesUnused        = 0x9A,  /* granted but not sent */

    OamExtAttrRxDelayThreshold     = 0x9B,
    OamExtAttrRxDelay              = 0x9C,
    OamExtAttrRxFramesDropped      = 0x9D,
    OamExtAttrRxBytesDropped       = 0x9E,  /* dropped in queue, that is */
    OamExtAttrRxBytesDelayed       = 0x9F,

    /* Statistics thresholds (see also 0xD9) */
    OamExtAttrPortStatThreshold    = 0xA0,
    OamExtAttrLinkStatThreshold    = 0xA1,

    /* Encryption */
    OamExtAttrEncryptKeyExpiryTime = 0xA2,

    /* Additional port attributes */
    OamExtAttrLenErrorDiscard      = 0xA3,

    OamExtAttrDynMacTbl            = 0xA4,
    OamExtAttrStaticMacTbl         = 0xA5,

    OamExtAttrUpFilterTbl          = 0xA6,
    OamExtAttrDnFilterTbl          = 0xA7,

    /* Threshold levels for report messsages */
    OamExtAttrReportThresholds     = 0xA8,

    /* broadcast frames per second allowed through a port */
    OamExtAttrBcastRateLimit       = 0xA9,

    /* Path configuration for this LLID */
    OamExtAttrLinkConfig           = 0xAA,

	/* Path and queue configuration for the entire ONU */
	OamExtAttrOnuConfig            = 0xAB,

    /* VLAN tag Ethertype (in addition to 8100) */
    OamExtAttrVlanEthertype        = 0xAC,

    /* Output shaping (3711 only) */
    OamExtAttrOutputShaping        = 0xAD,
	
	/* UNI Port Capability (3711 only) */
    OamExtAttrPortCapability       = 0xAE,

	/* Control VLAN Tag (3711 only) use for delete/copy */
    OamExtAttrCtlVlanID            = 0xAF,

	/* MDI extended setting */
	OamExtAttrMdiCrossover         = 0xB0,

	OamNewExtAttrDnFilterTbl	   = 0xB1,

	OamNewExtAttrUpFilterTbl	   = 0xB2,

	OamExtAttrJedecId			   = 0xB3,
	OamExtAttrChipId			   = 0xB4,
	OamExtAttrChipVersion		   = 0xB5,

    /* OLT MPCP clock value to generate 1 second phase locked pulse at ONU */
    OamExtAttrMpcpClock            = 0xB6,
    OamExtAttrMpcpClockCompensate  = 0xB7,

    OamExtAttrEgressShaping        = 0xB8,
    OamExtAttrIngressPolicing      = 0xB9,
    OamExtAttrCosTranslation       = 0xBA,
    OamExtAttrPriEnqueuing         = 0xBB,

    OamExtAttrPortVlanPolicy       = 0xBC,
    OamExtAttrPortVlanMembership   = 0xBD,

    /* ARP replication destination bitmap */
    OamExtAttrArpReplicateDest	   = 0xBE,

	OamExtAttrLacpDest             = 0xBF,

	/* RSTP Attributes */
	OamExtAttrRstpPortMode         = 0xC0,
	OamExtAttrRstpPortOperEdge     = 0xC1,
	OamExtAttrRstpBridgeOptions    = 0xC2,
	OamExtAttrRstpBridgeMode       = 0xC3,

	OamExtAttrOnuIgmpVlan	       = 0xC4,

    /* ASCII coded Time of Day string */
    OamExtAttrTimeOfDay            = 0xC5,

    // VLAN tag Ethertype (in addition to 88a8)
    OamExtAttrSVlanEthertype       = 0xC6,
	/* Learning Mode Configuration */
	OamExtAttrDynLearningMode      = 0xD0,
	OamExtAttrMinMacLimit          = 0xD1,
	OamExtAttrOnuAggregateLimit    = 0xD2,

	OamExtAttrNvsScratchpad        = 0xD3,

	/* More learning mode */
	OamExtAttrFloodUnknown         = 0xD4,
	OamExtAttrLocalSwitching       = 0xD5,
	OamExtAttrDownBurstToll        = 0xD6,

	/* FEC attribute */
	OamExtAttrFECMode		       = 0xD7,
	OamExtAttrSequenceNumber	   = 0xD8,

    /* Statistics thresholds (see also 0xA0) */
    OamExtAttrQueueStatThreshold   = 0xD9,
	/* Even mode learning mode */
	OamExtAttrLearnModeRuleUpdate  = 0xDA,

	/* Power Monitoring statistics */
	OamExtAttrPowerMonTemperature  = 0xDB,
	OamExtAttrPowerMonVcc          = 0xDC,
	OamExtAttrPowerMonTxBias       = 0xDD,
	OamExtAttrPowerMonTxPower      = 0xDE,
	OamExtAttrPowerMonRxPower      = 0xDF,

    // MLD Configuration
    OamExtAttrMcastDomainConfig    = 0xE0,
    OamExtAttrMcastGlobalConfig    = 0xE1,

    // MLD Group Settings and information
    OamExtAttrMcastGroupInfo       = 0xE2,
    OamExtAttrLinkState            = 0xE3,
    
    // Legacy IGMP frame rate limit
    OamExtAttrIgmpFrameRateLimit   = 0xE4,

    // Legacy IGMP profile (unimplemented)
    OamExtAttrIgmpProfile          = 0xE5,

    OamExtAttrOnuHoldover          = 0xE6,

    OamExtAttrIpmcUnknownLeaveFwd  = 0xE7,

    OamExtAttrLocalEgressShaping   = 0xE8,

    OamExtAttrNetworkPonMap        = 0xEA,

    // ONU Protection Switching state info
    OamExtAttrOnuPSState           = 0xEB,

    OamExtAttrSleModeField         = 0xEC,

    OamExtAttrLrnTableMode         = 0xED,

    OamExtAttrSysRuleOpts          = 0xEE,

    OamExtAttrVlanDest             = 0xF0,

    OamExtAttrOnuTemps             = 0xF1,

    // IPN Specific OAM
    OamExtAttrXcvrSignaletect     = 0xF2,

    OamExtAttrCrossbarConfig       = 0xF3,

    OamExtAttrXcvrBurstActivity    = 0xF4,

    OamExtAttrControlPort          = 0xF5,

    OamExtAttrOnuFailsafe          = 0xF6,

    OamExtAttrNetworkOltReport     = 0xF7,

    OamExtAttrEncryptMode          = 0xF8,

    OamExtAttrEpocUpAmplStats      = 0xFA,
    OamExtAttrInternalVersion      = 0xFB,
    OamExtAttrFirmwareTimestamp    = 0xFC,
    OamExtAttrEpocDownBitLoading   = 0xFD,
    OamExtAttrEpocUpBitLoading     = 0xFE,
    OamExtAttrEpocUpPhaseStats     = 0xFF,
    OamExtAttrMinError             = 0xEF,
    OamExtAttrFecError             = 0xE5,
    OamExtAttrQesBandStats         = 0xCC,
    OamExtAttrQesSymStats          = 0xCD,
    OamExtAttrSymbolErrorStats     = 0xCE,
    OamExtAttrBitErrorStats        = 0xCF,



    OamExtAttrKtStaticMac          = 0xFF,
    // Conflict with customer TIP, OamExtAttrCtcAuthLoid = 0x100
    OamExtAttrKtMacLimit           = 0x100,

    OamExtAttrCtcAuthPassword      = 0x101,

    // Enable/Disable switch's Bypass Software Learning
    OamExtAttrBypassSoftLearn      = 0x145,

    // Statistics thresholds interval
    OamExtAttrPortStatThresholdInv = 0x1A0,
    OamExtAttrLinkStatThresholdInv = 0x1A1,

    ////////////////////////////////////////////////////////////////////////////
    ///Extended by Teknovus China ONU Team
    ///make the OAM in firmware is consistent with SDK
    ///which uses these OAM
    ////////////////////////////////////////////////////////////////////////////
    //
    OamExtAttrAddiFirst            = 0x1080,
    OamExtAttrCtcAuthLoidComp      = 0x1081,
    OamExtAttrCtcAuthPasswordComp  = 0x1082,

    OamExtAttrDnFwdMode            = 0x1083,
    OamExtAttrMacAgingTimeHardLearn= 0x1084,
    OamExtAttrCtcAlarmState        = 0x1085,

    OamExtAttrOamDiscState         = 0x10E8,
    OamExtAttrCtcOamDiscState      = 0x10E9,
    OamExtAttrLinkInfo             = 0x10EA,
    OamExtAttrSwitchPortMir        = 0x10EB,
    OamExtAttrOnuSn                = 0x10EC,
    // EPON port admin state
    OamExtAttrEponPortAdmin        = 0x10ED,
    OamExtAttrPonRegisterStatus    = 0x10EE,
    OamExtAttrMacLearning          = 0x10EF,
    OamExtAttrMacFlush             = 0x10F0,
    OamExtAttrMtuSize              = 0x10F1,
    OamExtAttrMacLearningSwitch    = 0x10F2,
    OamExtAttrBatteryBackupCap     = 0x10F3,
    // Conflict with customer TIP, OamExtAttrCtcOamVerion = 0x10F4
    OamExtAttrOamVerion            = 0x10F4,
    OamExtAttrLaserAlwaysOn        = 0x10FF,
    OamExtAttrCtcChurningState     = 0x1100,
    OamExtAttrCtcOamVersionList    = 0x1101,
    OamExtAtrrCtcOamSleepCtrl        = 0x1102,

    // Extend for China SDK
    // Extend from ZTE_MDU and to be common
//  OamExtAttrLinkInfo             = 0x12E6, // already defined to 0x10EA,
    OamExtAttrMpcpTimeout          = 0x12E7,
    OamExtAttrIfAllowReg           = 0x12E8,
    OamExtAttrDownSharping         = 0x12E9,
//  OamExtAttrLaserAlwaysOn        = 0x12EA, // already defined to 0x10FF,
    OamExtAttrLlidQueueConfig      = 0x12EB,
    OamExtAttrCTCLoidAuthResult    = 0x12EC, // for CTC LOID auth result from external CPU
    OamExtAttrCTCIfOamDiscComp     = 0x12ED, // for CTC OAM discovery state
    OamExtAttrOnuCtcHoldover       = 0x12EF,
    OamExtAttrEpldStatus           = 0x12F0, // the EPLD status of 9806H
    OamExtAttrEponCtcIn            = 0x12F1, // the EPON last 10 CTC OAMs for 9806H
    OamExtAttrUniCtcIn             = 0x12F2, // the UNI last 10 CTC OAMs for 9806H
    OamExtAttrGatherStatus         = 0x12F3, // the statistic gather status for F820
    OamExtAttrL2CtrlFramePass      = 0x12F4,  // pass through the L2 Control Frames for 9806H
    OamExtAttrPersItem             = 0x12F5,

    // To avoid conflict with standard OAM ATTR, we might as well start from
    // the last and go up.
    OamExtAttrAlarmIdThreshold     = 0x7FE5,
    OamExtAttrPersVariantSelector  = 0x7FE6,
    OamExtAttrReserved1            = 0x7FE7, //OamExtAttrUniPllStatus,EASW-16475
    OamExtAttrL2CPConfig           = 0x7FE8,
    OamExtAttrCtcPowerSaveState    = 0x7FE9, //get CTC power save state
    OamExtAttrLueFieldSelect       = 0x7FEA,
    OamExtAttrPortStaticEntries    = 0x7FEB,
    OamExtAttrPortDynEntries       = 0x7FEC,
    OamExtAttrPonCfgInfo           = 0x7FED,
    OamExtAttrHostCtlGpioOut       = 0x7FEE,
    OamExtAttrLearningConfig       = 0x7FEF,
    OamExtAttrSearchConfig         = 0x7FF0,
    OamExtAttrQueueConfigV2        = 0x7FF1,
    OamExtAttrDnSecEncryptInfo     = 0x7FF2,
    OamExtAttrMpcpForward          = 0x7FF3,
    OamExtAttrQueuePriMap          = 0x7FF4,
    OamExtAttrNvsState             = 0x7FF5,
    OamExtAttrMtu                  = 0x7FF6,
    OamExtAttrFileInfo             = 0x7FF7,
    OamExtAttrEncryptKey           = 0x7FF8,
    OamExtAttrExtFirmwarVersion    = 0x7FF9,
    OamExtAttrFeatureSet           = 0x7FFA,
    OamExtAttrUpVlanStrip          = 0x7FFB,
    OamExtAttrShaper               = 0x7FFC,
    OamExtAttrPolicer              = 0x7FFD,
    OamExtAttrRule                 = 0x7FFE,

    OamExtAttrLeafLast
#if OAM_VERSION < 320
    /* one octet leaf */
#elif OAM_VERSION == 320
    , OamExtAttrForce16             = 0x7fff    
#else
#error "Unsupported OAM OAM_VERSION"
#endif
    } PACK OamExtAttrLeaf;

typedef enum
    {
    OamExtActFirst                 = 0x80,

    OamExtActClearDynLearnTbl      = 0x80,
    
    /* static bridging entries */
    OamExtActAddStaticEntry        = 0x81,
    OamExtActDelStaticEntry        = 0x82,

    /* user-defined packet filtering rules */
    OamExtActClearUpFilterTbl      = 0x83,
    OamExtActClearDnFilterTbl      = 0x84,

    /* rules */
    OamExtActAddRule               = 0x85,
    OamExtActDelRule               = 0x86,

    /* reset the ONU */
    OamExtActResetOnu              = 0x87,

    /* clear all stats to 0 */
    OamExtActClearStats            = 0x88,

    /* read/write GPIO register */
    OamExtActGetGpioConfig         = 0x89,
    OamExtActSetGpioConfig         = 0x8A,

    OamExtActGetGpioValue          = 0x8B,
    OamExtActSetGpioValue          = 0x8C,

    /* MDIO register control */
    OamExtActGetMdio               = 0x8D,
    OamExtActSetMdio               = 0x8E,

    /* Extended loopback */
    OamExtActLoopbackEnable        = 0x8F,
    OamExtActLoopbackDisable       = 0x90,

    /* Classification rules */
    OamExtActClrUpClass            = 0x91,    /* clears current in-context link */
    OamExtActClrDnClass            = 0x92,    /* clears current in-context port */

    /* Queue configuration */
    OamExtActSetQueueConfig        = 0x93,
    OamExtActGetQueueConfig        = 0x94,

    /* Erase Non-volatile Store */
    OamExtActEraseNvs              = 0x95,

    /* IGMP configuration */
    OamExtActSetIgmpConfig         = 0x96,
    OamExtActGetIgmpConfig         = 0x97,

	/* IGMP Group Settings and information */
	OamExtActGetIgmpGroupInfo      = 0x98,
	OamExtActDelIgmpGroup          = 0x99,
	OamExtActAddIgmpGroup          = 0x9A,

    /* Queue output rate control -- 3711 only */
    OamExtActSetQueueRateControl   = 0x9B,
    OamExtActGetQueueRateControl   = 0x9C,

    /* Port output rate control -- 3711 only */
    OamExtActSetPortRateControl    = 0x9D,
    OamExtActGetPortRateControl    = 0x9E,

    /* Phy addressable MDIO register control */
    OamExtActSetPhyAddrMdio        = 0x9F,
    OamExtActGetPhyAddrMdio        = 0xA0,

    OamExtActNewAddRule		       = 0xA1, 

    OamExtActOnuEnableUserTraffic  = 0xA2,
    OamExtActOnuDisableUserTraffic = 0xA3,

	OamExtActSetDnBcastQueue       = 0xA4,
	OamExtActGetDnBcastQueue       = 0xA5,

	OamExtActSetOamRate            = 0xA6,
	OamExtActGetOamRate            = 0xA7,

	OamExtActSetLUEField           = 0xA8,
	OamExtActGetLUEField           = 0xA9,

    OamExtActGetLoadInfo           = 0xAA,
	OamExtActNewDelRule			   = 0xAB,

    /* Classification rules */
    OamExtActClrUpUserRules        = 0xAC,
    OamExtActClrDnUserRules        = 0xAD,

    // MLD Configuration
    OamExtActDelMcastDomainConfig  = 0xAE,

    // MLD Group Settings and information
    OamExtActDelMcastGroup         = 0xAF,

    // Legacy power control: Dasan IOP
    OamExtActLaserTxPowerOff       = 0xB0,

    // For Optical Leakage Detection feature.
    OamExtActBcLaserPowerOff       = 0xB1,

    OamExtActSleep                 = 0xB2,

    OamExtActProtSwitch            = 0xB3,

    OamExtActSearchMac             = 0xB4,

    OamExtActAddLueRule            = 0xB5,

    OamExtActDelLueRule            = 0xB6,

    OamExtActEnablePolicer         = 0xB7,

    OamExtActDisablePolicer        = 0xB8,

    // read/write GPIO register
    OamExtActGetGpioConfigVar      = 0xB9,
    OamExtActSetGpioConfigVar      = 0xBA,

    OamExtActGetGpioValueVar       = 0xBB,
    OamExtActSetGpioValueVar       = 0xBC,

    // For ONU to genetate traffic.
    OamExtActOnuGenTraffic         = 0xBD,


    OamExtActArmQes                 = 0xCA,
    OamExtActArmSymbolErrorStats    = 0xCB,

    OamExtActFindLueRule             = 0xCF,
    OamExtActClearLueRules           = 0xD0,
    OamExtActEpocSdm250Stats            = 0xD1,
    OamExtActEpocCmcStats            = 0xD2,
    OamExtActEpocCnuStats            = 0xD3,

    ////////////////////////////////////////////////////////////////////////////
    ///Extended by Teknovus China ONU Team
    ////////////////////////////////////////////////////////////////////////////
    OamExtActAddiFirst             = 0x1080,
    OamExtActClrMacFilterTbl       = 0x1081,
    //Whether the Pon signal is lose or not
    //the extended OAM has been added in the SDK, in order to consist with SDK,
    // we also make it as this
    OamExtActGetPonLos             = 0x10FF,

    OamExtActPartExtendId          = 0x7F03,
    //Set the PON and UNI base Mac of ONU
    // keep the same with 1G firmware which was extended long ago
    OamExtActBaseMac               = 0x7F04,
    OamExtActOnuCommit             = 0x7F05,

    ////////////////////////////////////////////////////////////////////////////
    ///10G ONU /OLT Extension
    ////////////////////////////////////////////////////////////////////////////

    OamExtActionLeafLast
#if OAM_VERSION < 320
    /* one octet leaf */
#elif OAM_VERSION == 320
    , OamExtActForce16             = 0x7fff
#else
#error "Unsupported OAM OAM_VERSION"
#endif
    } PACK OamExtActionLeaf;



/*  value of the "stat threshold" attribute */
typedef struct
    {
    U16     stat;       /* < stat affected by this threshold */
    U32     rising;     /* < rising threshold that sets alarm */
    U32     falling;    /* < falling threshold that clears alarm */
    } PACK OamExtStatThreshold;

/// value of the "stat interval" attribute
typedef struct
    {
    OamExtAttrLeaf  stat;       ///< stat affected by this threshold
    U16             inv;        ///< stat threshold interval, 100ms increment
    } PACK OamExtStatThresholdInv;
/*  value of OutputShaping attribute */
typedef struct
    {
    U8   outputBurstSize;       /* < units of 256 bytes; 0 to disable shaping */
    U16  rate;                  /* < units of 1/(2^15) Gbps (~30,518 bps) */
    U16  queueMap;              /* < 1s indicate queues involved in shaping */
    } PACK OamExtOutputShaping;

typedef struct
    {
    U16             burstSize;   /* < units of 256 bytes; 0 to disable shaping */
    U32             rate;        /* < units of 1 Kbps  */
    U8              numQs;
    OamNameQueue    queue[1];
    } PACK OamExtPortOutputShaping;

typedef struct
    {
    OamNameQueue    queue;
    U16             burstSize;    /* < units of 256 bytes; 0 to disable shaping */
    U32             rate;         /* < units of 1 Kbps  */
    } PACK OamExtPerQueueShaping;

typedef struct
    {
    U8                      numQs;
    OamExtPerQueueShaping   queueRate[1];
    } PACK OamExtQueueOutputShaping;

/*  value of the "OamExtAttrVlanEthertype"  attribute */
/*  For both TK3701 and TK3711 but dnEtype/upEtype only affects TK3711 */
typedef struct
    {
    U16	ethertype;/* < User-defined Ethertype for VLAN */
    U8	upEtype;  /* < 0=0x8100, 1=user Ethertype when adding VLAN in upstream */
    U8	dnEtype;  /* < 0=0x8100, 1=user Ethertype when adding VLAN in downstream */
    } PACK OamExtVlanEthertype;


/* Ext FEC mode  */
typedef struct
    {
    U8 rxFEC;        /* < non-zero if ONU rx/downstream FEC is enabled */
    U8 txFEC;        /* < non-zero if ONU tx/upstream FEC is enabled */
    } PACK OamExtFECMode;

// Ext Temperatures
typedef struct
    {
    U8  tempCnt;         /// number of temperatures being returned.
    U32 temps[1];        /// The temperatures being returned.
    } PACK OamExtTemps;

// Ext failsafe modes
typedef struct
    {
    U8  failsafeCount;      /// number of failsafes being returned.
    U8 failsafes[1];        /// The failsafes being returned.
    } PACK OamExtFailsafes;

/// Ext Link State
typedef enum
    {
    LinkStateDisable = 0,
    LinkStateEnable  = 1
    } PACK OamLinkState;

typedef struct
    {
    OamLinkState state;
    } PACK OamExtLinkState;

typedef enum
    {
    LinkUnRegister  = 1,
    LinkRegistering = 2,
    LinkRegistered  = 3
    } PACK OamLinkRegState;

typedef struct
    {
    OamLinkRegState     state;
    U16                 phyLlid;
    } PACK OamEponLinkInfo;
typedef struct
    {
    U8                  los;   // 0 - normal, 1 - Los
    U8                  linkNum;
    OamEponLinkInfo     linkInfo[1];
    } PACK OamEponPortInfo;

typedef enum
    {
    OamPonSpeed1G      = 1,
    OamPonSpeed2G      = 2,
    OamPonSpeed10G     = 3
    } PACK OamPonSpeed;
	
typedef struct
    {
    U8                  pon;   // 0 - main, 1 - backup
    OamPonSpeed         upSpd;
    OamPonSpeed         dnSpd;
    } PACK OamPonCfgInfo;

/*  Defines to control UNI Port VLAN configuration */
typedef enum
    {
    VlanPolicyDisabled     = 0,
    VlanPolicyFallback     = 1,
    VlanPolicyCheck        = 2,
    VlanPolicySecure       = 3,

    VlanPolicyMaxPolicy    = 4
    } PACK OamUniVlanPolicy;


/*  Defines to control per VLAN ingress/egress tagging behavior */
typedef enum
    {
    VlanUnmodified  	   = 0,
    VlanAddDefTagIfNone    = 1,
    VlanRemoveTag          = 2,
    VlanOverwriteTag       = 3,

    VlanTagBehaveMaxBehave = 4,
    VlanStackDefaultTag    = 4	/* FOR FUTURE USE - DO NOT USE AT THIS TIME */
    } PACK OamVlanTagBehave;

typedef struct
	{
	U16     		 options;	/* reserved - NOT IN USE AT THE MOMENT */

	OamUniVlanPolicy ingressPolicy;
	OamUniVlanPolicy egressPolicy;
	U16				 defaultVlan;
	} PACK OamVlanPolicyCfg;

typedef struct
	{
	U16				 ingressVlan;
	U16				 egressVlan;
	OamVlanTagBehave ingressTagging;
	OamVlanTagBehave egressTagging;

	U8				 flags;	/* reserved - NOT IN USE AT THE MOMENT */
	} PACK OamVlanMemberCfg;

typedef struct OamBitLoad
    {
    U8 idx;
    U8 val;
    } PACK OamBitLoad;

typedef struct SymErrStat
    {
    U32 durationMs;
    U16 symbolStart;
    U16 symbolEnd;
    BOOL cliMode;
    BOOL logMode;
    BOOL printBandError;
    } PACK SymErrStat;

typedef enum SdmStat
    {
    SdmStatQuantizer,
    SdmStatAmplitude,
    SdmStatPhase
    } PACK SdmStat;

typedef struct QuantErrStat
    {
    U16 sFrameCount;
    U16 symbolStart;
    U16 symbolEnd;
    BOOL cliMode;
    SdmStat stat;
    BOOL maxHold;
    } PACK QuantErrStat;
#define OamEpocDownBitLoadingLoadCount 32
typedef struct
    {
    OamBitLoad load[OamEpocDownBitLoadingLoadCount];
    }PACK OamEpocDownBitLoading;

typedef struct
    {
    U16 index;
    union
        {
        U32 u32;
        U24 u24;
        U16 u16;
        U8 u8;
        }stat;
    } PACK OamExtAttrEpocStats;

// Shaping/Policing related stuff
typedef enum
    {
    OamTrafficBitNone            = 0,
    OamTrafficBitBroadcast       = 0x1,
    OamTrafficBitMulticast       = 0x2,
    OamTrafficBitReservedGroup   = 0x4,
    OamTrafficBitUnicast         = 0x8,
    OamTrafficBitFloodUnicast    = 0x10,
    OamTrafficBitAll             = 0x1F
    } PACK OamTrafficBits;

typedef enum
    {
    OamSchedWeigtedFair,
    OamSchedStricPri
    } PACK OamSchedAlgorithm;

typedef enum
    {
    OamRateKbps  = 0,    /* < Kilo bits per second */
    OamRateFps   = 1     /* < frames per second */
    } PACK OamRateUnits;

typedef struct
    {
    U8      queueBits;      /* < Queues to which shaping applies */
    U32     maxRate;        /* < Rate in terms of OamRateUnits */
    U16     burstSize;      /* < KBytes */
    U8      schedAlgo;      /* < Scheduling algorithm */
    U8      numWeights;     /* < Number of priority levels */
    U8      weight[1];      /* < Weights per priority (high first) */
    } PACK OamUniShaperData;

typedef struct
    {
    U8               trafficBits;
    OamRateUnits     unit;
    U8               numShapers;
    OamUniShaperData shaper[1];
    } PACK OamEgressShaping;

typedef struct
    {
    U8      trafficBits;    /* < what kind of traffic to police */
    U32     maxRate;        /* < Rate in terms of OamRateUnits */
    } PACK OamOnuPolicerData;

typedef struct
    {
    OamRateUnits         unit;
    U8                   numRateLevels;
    OamOnuPolicerData    policer[1];
    } PACK OamIngressPolicing;


typedef enum
    {
    OamPriCos        = 0,
    OamPriTos        = 1,
    OamPriDiffServTc = 2
    } PACK OamPriType;

typedef struct
    {
    U8 lo;          /* < lower value of range */
    U8 hi;          /* < higher value of range */
    } PACK OamPriRange;

typedef struct
    {
    U8           inputType;
    OamPriRange  inPri[8];  /* < input priority range for each COS output  */
    } PACK OamPriCosTable;

typedef struct
    {
    U8             inputPriOptions; /* < reserved */
    U8             numTables;       /* < number of priority mapping tables */
    OamPriCosTable mapTable;
    } PACK OamCosTranslation;


typedef struct
    {
    U8  inPri;
    U8  outQueue;
    } PACK OamPriToQueue;

typedef struct
    {
    U8             inPri;       /* < input priority type */
    U8             numMaps;     /* < Number of values to map */
    OamPriToQueue  map[1];
    } PACK OamPriEnqueueTable;

typedef struct
    {
    U8                  portDfltPri;
    U8                  inpPriOptions;
    U8                  numPriTables;
    OamPriEnqueueTable  enqTable[1];
    } PACK OamPriTrafficEnqueuing;


/*
* Extended Actions
*/

/* clear MAC table - no payload required */

/* Get MAC table */
typedef struct
    {
    U8 seqNum;          /* sequence number of this frame */
    U16 numEntries;     /* number of MAC addresses (6 bytes) to follow */
    } PACK OamMacTable;

/* Add/Delete filter */

typedef enum
    {
    OamRuleDirDownstream = 0,
    OamRuleDirUpstream   = 1
    } PACK OamRuleDirection;

typedef enum
    {
    OamFieldDa,
    OamFieldSa,
    OamFieldLlidIndex,
    OamFieldEthertype,
    OamFieldVlan,

    OamFieldPriority,
    OamFieldIpProtocol,
    OamFieldUserDefined

    } PACK OamRuleFields;

/*  possible actions for filters. */
typedef enum
	{
	OamRuleActDiscard,			/* Old format */
	OamNewRuleActNoOp = 0,
	OamRuleActForward,			/* Old format */
	OamNewRuleActReserve1 = 1,
	OamNewRuleActForward,
	OamNewRuleActSetVlanAdd,
	OamNewRuleActSetVlanDel,
	OamNewRuleActSetVlanAddVid,
	OamNewRuleActSetVlanCos,
	OamNewRuleActSetVlanAddDel,
	OamNewRuleActSetVlanAddDelVid,
	OamNewRuleActClrVlanAdd,
	OamNewRuleActClrVlanDel,
	OamNewRuleActClrVlanAddDel,
	OamNewRuleActCpVlanCos,
	OamNewRuleActCpVlanVid,
	OamNewRuleActDiscard,
	OamNewRuleActReserve2,
	OamNewRuleActNoOpClrDisc,
	OamNewRuleActReserve3,
	OamNewRuleActForwardClrDisc,
	OamNewRuleActSetVlanAddClrDisc,
	OamNewRuleActSetVlanDelClrDisc,
	OamNewRuleActSetVlanAddVidClrDisc,
	OamNewRuleActSetVlanCosClrDisc,
	OamNewRuleActSetVlanAddDelClrDisc,
	OamNewRuleActSetVlanAddDelVidClrDisc,
	OamNewRuleActClrVlanAddClrDisc,
	OamNewRuleActClrVlanDelClrDisc,
	OamNewRuleActClrVlanAddDelClrDisc,
	OamNewRuleActCpVlanCosClrDisc,
	OamNewRuleActCpVlanVidClrDisc,
	OamNewRuleActDiscardClrDisc,
	OamNewRuleActReserve4,
	OamNumberRuleActions
    } PACK OamRuleAction;

#define OAMRULE_CLRDISCARD	0x10

/*  possible operators for filter rules */
typedef enum
    {
    OamRuleOpNeverMatch,
    OamRuleOpEq,
    OamRuleOpNotEq,
    OamRuleOpLessEq,
    OamRuleOpGreaterEq,
    OamRuleOpExist,
    OamRuleOpNotExist,
    OamRuleOpAlwaysMatch,

    OamRuleOpNumOps
    } PACK OamRuleOp;

/*  describes a single filter rule condition */
typedef struct
    {
    U8  field;
    U8  value[8];
    U8  operator;
    } PACK OamNewRuleCondition;

/* multiple conditions in a single action are considered to be logically
 * ANDed together.  ORs can be accomplished by adding multiple filter rules.
 * specification for a single rule */
typedef struct
	{
	U8  	        dir;
	OamRuleParam    param;
	U8              pri;
	U8     	        action;
	U8              numConds;
	OamNewRuleCondition	cond[1];
	} PACK OamNewRuleSpec;

/*  describes a single filter rule condition */
typedef struct
    {
    U8  field;
    U8  value[6];
    U8  operator;
    } PACK OamRuleCondition;

/* multiple conditions in a single action are considered to be logically */
/* ANDed together.  ORs can be accomplished by adding multiple filter rules. */
/*  specification for a single rule */
typedef struct
    {
    U8                dir;
    OamNameQueue      dest;
    U8                pri;
    U8                action;
    U8                numConds;
    OamRuleCondition  cond[1];
    } PACK OamRuleSpec;

/*  Layers available to lookup engine */
typedef enum
    {
    OamFieldLayerNull,
    OamFieldLayerL2,            /* < Preamble and Ethernet DA, SA */
    OamFieldLayerL2TypeLen,     /* < Ethernet type/length field */
    OamFieldLayerMpls,          /* < MPLS tags */
    OamFieldLayerIpv4,          /* < IP version 4 */
    OamFieldLayerL4Ip,          /* < Layer 4 on IP, e.g. TCP / UDP */
    OamFieldLayerIpv6,          /* < IP version 6 */

    OamFieldNumLayers
    } PACK OamFieldLayer;

/*  define a lookup engine field pointer */
typedef struct
    {
    U8  layer;          /* < layer containing field */
    U8  dwordOffset;    /* < 32-bit words from start of layer */
    U8  bitOffset;      /* < first bit within dword */
    U8  bitWidth;       /* < width of field in bits */
    U8  inUse;          /* < true if rules use this field */
    } PACK OamFieldDef;


typedef enum
    {
    OamRuleSubtypeTerminator = 0,
    OamRuleSubtypeHeader     = 1,
    OamRuleSubtypeClause     = 2,
    OamRuleSubtypeResult     = 3,
    OamRuleSubtypeEntry      = 4
    } PACK OamRuleSubtype;
typedef struct
    {
    OamRuleSubtype subtype;  // Header, Clause, Result or Terminator
    U8             rule[1];
    } PACK OamRule;

typedef struct
    {
    U8  priority;   /// Priority of the rule
    } PACK OamRuleHeader;


typedef struct
    {
    U8  fieldSelect;
    U8  fieldInstance;
    U8  msbMask;
    U8  lsbMask;
    U8  operator;
    U8  matchValue[1];
    } PACK OamRuleClause;


typedef struct
    {
    OamNameLeaf portType;
    U8          portIndex;
    U8          link;
    U8          queue;
    } PACK OamRuleQueueValue;


typedef struct
    {
    U8  action;
    U8  value[1];
    } PACK OamRuleResult;


typedef enum
    {
    OamEntryFieldNone                 = 0,
    OamEntryFieldSwDomainL2Da         = 1,
    OamEntryFieldSwDomainL2Sa         = 2,
    OamEntryFieldSwDomainSsmSrcIpDa   = 3,
    OamEntryFieldProgrammable         = 4,
    OamEntryFieldCVid                 = 5,
    OamEntryFieldSvidCvid             = 6
    } PACK OamRuleEntryField;


typedef struct
    {
    U16             switchDomain;
    MacAddr         macAddr;
    U8              linkId;
    } PACK OamEntryDa;
typedef OamEntryDa OamEntrySa;

typedef struct
    {
    U16             switchDomain;
    U16             ssm;
    U32             ipDa;
    } PACK OamEntrySsmSrcId;

typedef struct
    {
    U32             field0;
    U32             field1;
    } PACK OamEntryProg;

typedef struct
    {
    U16             cvid;
    } PACK OamEntryCVid;

typedef struct
    {
    U16             svid;
    U16             cvid;
    } PACK OamEntrySVidCVid;

typedef struct
    {
    U16               learningDomain;
    BOOL              isStatic;
    U8                rightMask;
    OamRuleEntryField fieldDataType;
    U8                entryData[1];
    } PACK OamRuleEntry;



////////////////////////////////////////////////////////////////////////////////
// Tek Queue Config V2
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    OamTekQCfgSubtypeTerm   = 0,
    OamTekQCfgSubtypeHeader = 1,
    OamTekQCfgSubtypeLink   = 2,
    OamTekQCfgSubtypePort   = 3
    } PACK OamTekQCfgSubtype;


typedef struct
    {
    OamTekQCfgSubtype   subtype;
    U8                  cfg[1];
    } PACK OamTekQCfg;


typedef enum
    {
    OamTekRepCfgByTkOltPrefer 	= 0x00,
    OamTekRepCfgFrameAligned    = 0x01,
    OamTekRepCfgMultiPri3       = 0x13,
    OamTekRepCfgMultiPri4       = 0x14,
    OamTekRepCfgMultiPri8       = 0x18,
    } PACK OamTekReportConfig;


typedef struct
    {
    OamTekReportConfig reportMode;
    } PACK OamTekQCfgHeader;
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    OamPolicerDirDownstream,
    OamPolicerDirUpstream,
    OamPolicerNumDirections
    } PACK OamPolicerDirection;

typedef struct
    {
    OamPolicerDirection dir;
    U8                  id;
    U32                 burstSize;
    U32                 rate;
    } PACK OamPolicer;

typedef struct
    {
    U32 portMap;
    U32 rate;
    U16 burstSize;
    } PACK OamShaper;

typedef struct
    {
    U8  shaperCount;
    OamShaper shaper[1];
    } PACK OamShapers;
//OamExtAttrHostCtlGpioOut
typedef struct
    {
    U16                 id;
    U8                  inst;
    U8                  nums;
    U8                  action[1];
    } PACK OamGpioHco;
/* GPIO configuration structure */
typedef struct
    {
    U32 autoReport; /* < 1 if autonomous message generated on changes */
    } PACK OamGpioConfig;

/* GPIO control structure */
/* Write specifies mask and value.  Write response specified actual mask  */
/* used and values after write.  Read request specifies mask; read response  */
/* specifies mask and value. */
typedef struct
    {
    U32 mask;       /* < 1s indicate bits to affect */
    U32 value;      /* < valid where mask is 1 */
    } PACK OamGpioValue;

// ONU generate traffic structure
typedef struct
    {
    U32 cnt;
    U8  pkt[1];
    } PACK OamOnuGenTraffic;

/* MDIO control */
/* the MDIO device is specified by the port of the context in effect */
/* when this action is processed */
typedef struct
    {
    U16 reg;        /* < register (0..31) */
    U16 mask;       /* < 1 indicates bit in value to affect */
    U16 value;      /* < value to read; value read in read response */
    } PACK OamMdioValue;

/*  PHY addressable MDIO control  */
typedef struct
    {
    U8              phy;        /* < PHY address */
    OamMdioValue    mdio;
    } PACK OamPhyAddrMdioValue;

// Extended MDIO settings pertaining to crossover: MDI, MDI-X, MDI Auto
typedef enum
    {
    OamMdiAuto = 0x0,// Automatic MDI detection and adaptation
    OamMdi     = 0x1,// MDI normal
    OamMdiX    = 0x2 // Crossover MDI
    } PACK OamMdiCrossoverValue;

/* Extended loopback */
/* TK ONUs support UNI PHY, UNI MAC, and 802.3ah EPON link loopbacks */
typedef enum
    {
    OamLoopLocUniPhy,
    OamLoopLocUniMac,
    OamLoopLoc8023ah,
    OamLoopLocNowhere,	/* used in ONU to shutoff traffic without loopback */

    OamLoopLocNumLocs
    } PACK OamLoopbackLoc;

/* Loopback action parameters */
typedef struct
    {
    U8  loc;
    } PACK OamExtLoopback;


/* IGMP Snooping per-port configuration */
typedef struct
    {
    U8  numGroups;      /* < # IGMP groups (0 = Snooping disabled) */
    U8  dnQueue;        /* < Queue for downstream classification */
    } PACK OamIgmpPortConfig;

/* IGMP Snooping overall configuration */
typedef struct
    {
    U8                 robustnessCount; /* < # Leaves before a group is dropped */
    U8                 lmqCount;        /* < Last member query count */
    U8                 numPorts;        /* < Number of UNI ports to configure */
    OamIgmpPortConfig  port[1];         /* < Port specific parameters */
	/* IgmpGrpFwdQualifier     grpFwdQual; */	/* < Group Forwarding Qualifier */
	/* IgmpSnoopOption         snoopOpt; */	    /* < IGMP Snooping Options */
    } PACK OamIgmpConfig;


/* Structure used to convey information about an IP MC group */
/* and also the ports involved */
typedef struct
	{
	U32 ipAddr;  /* Group Address */
	U8  portMap; /* this is a bitmap indicating which of the */
                     /* 8 ports are involved in the IGMP group */
	} PACK OamIgmpGroupInfo;


/* Structure used to convey information about multiple IP MC groups
   and also the ports involved */
typedef struct
	{
	U8               numGroups;  /* < Number of IGMP groups */
	OamIgmpGroupInfo group[1];   /* < Group specific information */
	} PACK OamIgmpGroupConfig;

/* Structure used to set the logical queue number for multicast/broadcast
   Llids */
typedef struct
	{
	U8      numPorts;  /* < Number of UNI Ports */
	U8      queue[1];   /* < Logical Queueu number per port */
	} PACK OamDnBcastQueue;

/* Structure used to set the upstream min (not implemented) and max OAM rate
   in units of 100ms */
typedef struct
	{
	U8      maxRate;
	U8      minRate;   /* < Not implemented */
	} PACK OamUpOamRate;


typedef struct
	{
	U8      refCnt;
	U8      layerSelect;
	U8      dwordOffset;
	U8      lsb;
	U8      bitWidth;
	} PACK OamFieldDescriptor;


typedef struct
	{
	U8					numField;  /* < Number of Field Select */
	OamFieldDescriptor	field[1];  /* < Field Entries */
	} PACK OamLUEField;


typedef struct
    {
    U16   bootVer;        /* < Boot version */
    U32   bootCrc;        /* < Boot crc 32 */
    U16   persVer;        /* < Personality version */
    U32   persCrc;        /* < Personality crc 32 */
    U16   app0Ver;        /* < App0 version */
    U32   app0Crc;        /* < App0 crc 32 */
    U16   app1Ver;        /* < App1 version */
    U32   app1Crc;        /* < App1 crc 32 */
    U16   diagVer;        /* < Diagnostic version */
    U32   diagCrc;        /* < Diagnostic crc 32 */
    } PACK OamLoadInfo;


/*
 * ONU Configuration 
 *
 * The ONU configuration message body is (all TKU8):
 *     num up links
 *          num up queues+
 *              queue size+
 *     num dn ports
 *          num dn queues+
 *              queue size+
 *     num dn multicast queues
 *          queue size+
 *
*/


/*
 * Extended TLVs for standard Info
*/


/* Vendor-extended TLVs in standard Info message */
typedef enum
    {
    OamAlmNone                  = 0x00,

    OamAlmCodeLinkFault         = 0x10,
    OamAlmCodeLos               = 0x11,
    OamAlmCodeTxFail            = 0x12,  /* OLT only */
    OamAlmCodeTransmitDegrade   = 0x13,  /* OLT only */
    OamAlmCodeQueueOv           = 0x14,
    OamAlmCodeLearnTblOv        = 0x15,
    OamAlmCodeFlowCtrlTimeout   = 0x16,
    OamAlmCodeReportFail        = 0x17,  /* OLT only */
    OamAlmCodeGateTimeout       = 0x18,  /* ONU only */
    OamAlmCodeOamTimeout        = 0x19,
    OamAlmCodeKeyExchange       = 0x1A,
    OamAlmCodeAutoNegFailure    = 0x1B,  /* OLT only */
    OamAlmCodeGpioLinkFault     = 0x1C,
    OamAlmCodeLearnEvent        = 0x1D,

    OamAlmCodeLoopback          = 0x20,
    OamAlmCodePortDisabled      = 0x21,

    OamAlmCodeDyingGasp         = 0x40,
    OamAlmCodePower             = 0x41,
    OamAlmCodeGpioDying         = 0x42,

    OamAlmCodeCriticalEvent     = 0x60,
    OamAlmCodeRegister          = 0x61,
    OamAlmCodeGpioCritical      = 0x62,

    OamAlmCodeSystem            = 0x80,
    OamAlmCodeTemperature       = 0x81,
    OamAlmCodeGpio              = 0x82,
    OamAlmCodeAuthUnavail       = 0x83,
//    OamAlmCodeReserved84        = 0x84,
//    OamAlmCodeReserved85        = 0x85,
    OamAlmCodeStatAlarm         = 0x86,
	OamAlmCodeFlashBusy         = 0x87,
	
    OamAlmCodeStandbyLos        = 0x88,
	OamAlmCodeProtSwitch        = 0x89,
    OamAlmCodeCtcAlarm          = 0x90,
    OamAlmCodeLinkOamDiscComplete  = 0x91,
    OamAlmCodeIpnLos            = 0x92,
    OamAlmCodeDiagLoad          = 0x93,
    OamAlmCodeBootInvalid       = 0xA0,
    // Alarm code defined by Teknovus China Team
    // Starting from 0xC0
    OamAlmCodeOnuReady          = 0xB0,
    // for Alpha's special requirement
    OamAlmCodeOnuPonDisable     = 0xB1,
    OamAlmCodeCtcDiscover       = 0xB2,
    OamAlmCodeLaserAlwaysOn     = 0xB3,

    OamAlmCodeLinkRegStart      = 0xB8,
    OamAlmCodeLinkRegSucc       = 0xB9,
    OamAlmCodeLinkRegFail       = 0xBA,
    OamAlmCodeBipErr			= 0xBB,
	OamAlmCodeUniPllUnlock      = 0xBC,
	OamAlmCodeFanFault          = 0xBD,
	OamAlmCodePowerMonTempHigh     = 0xBE,
	OamAlmCodePowerMonTempLow      = 0xBF,
	OamAlmCodePowerMonVccHigh      = 0xC0,
	OamAlmCodePowerMonVccLow       = 0xC1,
	OamAlmCodePowerMonTxBiasHigh   = 0xC2,
	OamAlmCodePowerMonTxBiasLow    = 0xC3,
	OamAlmCodePowerMonTxPowerHigh  = 0xC4,
	OamAlmCodePowerMonTxPowerLow   = 0xC5,
	OamAlmCodePowerMonRxPowerHigh  = 0xC6,
	OamAlmCodePowerMonRxPowerLow   = 0xC7,
    }PACK OamTkAlarmCode;

typedef enum
    {
    OamAlarmClear,
    OamAlarmRaised
    } PACK OamTkAlarmState;

typedef enum
    {
    OamAlarmContextOnu,
    OamAlarmContextPort,
    OamAlarmContextLink,
    OamAlarmContextQueue,
    OamAlarmContextUserPon,
    OamAlarmContextNetworkPon
    } PACK OamTkAlarmContext;

/*  Teknovus extended alarm report */
typedef struct
    {
    OamEventExt     ext;
    U8              alm;        /* < which alarm */
    U8              state;      /* < raised or cleared */
    OamNameQueue    which;      /* < affected entity */
    U8              context;    /* < type of affected entity */
    } PACK OamEventTkAlarm;

/// Teknovus IPN alarm report
typedef struct
    {
    OamEventExt         ext;
    U8                  alm;       ///< which alarm
    U8                  state;      ///< raised or cleared
    OamNameQueue        which;      ///< affected entity
    OamTkAlarmContext   context;    ///< type of affected entity
    U8                  port;       ///< port number
    } PACK OamEventTkIpnAlarm;
	
/*  Teknovus extended statistics alarm report */
typedef struct
    {
    OamEventExt         ext;
    U8                  alm;
    U8                  state;      /* < raised or cleared */
    OamNameQueue        which;      /* < affected entity */
    OamTkAlarmContext   context;    /* < type of affected entity */
    U16                 stat;       /* < which stat crossed threshold */
    } PACK OamEventTkStatAlarm;

/*  Teknovus extended GPIO alarm report */
typedef struct
    {
    OamEventExt         ext;
    U8                  alm;
    U8                  state;      /* < raised or cleared */
    OamNameQueue        which;      /* < affected entity */
    OamTkAlarmContext   context;    /* < type of affected entity */
    U32                 changes;    /* < new alarms/clears that caused msg */
    U32                 current;    /* < current state of GPIO */
    } PACK OamEventTkGpioAlarm;


/// Teknovus extended GPIO alarm report
typedef struct
    {
    OamEventExt         ext;
    OamTkAlarmCode      alm;
    OamTkAlarmState     state;      ///< raised or cleared
    OamNameQueue        which;      ///< affected entity
    OamTkAlarmContext   context;    ///< type of affected entity
    U64                 changes;    ///< new alarms/clears that caused msg
    U64                 current;    ///< current state of GPIO
    } PACK OamEventTkGpioAlarm10G;

typedef enum
    {
    PsReasonHost,
    PsReasonLos,
    PsReasonMpcp,
    PsReasonHighBer,

    PsReasonMax,

    PsReasonNone = 255
    } PACK PsReason;


typedef struct
    {
    OamEventExt         ext;
    OamTkAlarmCode      alm;
    PsReason            reason;
    PsOnuState          state;
    } PACK OamEventTkProtSwitch;
////////////////////////////////////////////////////////////////////////////////
// Extended Multicast Register and Register Response
//
// The multicast register message is used to assign a multicast LLID
// to an ONU.  The message contains the new LLID, as well as the
// associated unicast LLID, so that the ONU can make the proper queue
// associations.  Note that unlike MPCP registers, which are requested
// by the ONU, the Multicast Register can be pushed from the OLT.
//
////////////////////////////////////////////////////////////////////////////////

/// Teknovus extended multicast LLID request
typedef struct
    {
    OamTkExt        ext;
    U8              flags;          // as for MPCP register message
    U16             assocUnicast;   // associated LLID
    } PACK OamTkMcastRequest;

/// Teknovus extended multicast LLID register
typedef struct
    {
    OamTkExt        ext;
    U8              flags;   // as for MPCP register message
    U16             assignedPort;
    U16             assocUnicast;
    } PACK OamTkMcastReg;
/// Teknovus extended multicast LLID reg ack
typedef struct
    {
    OamTkExt        ext;
    U8              flags;         // as per MPCP register ack
    U16             assignedPort;  // echoed
    U16             assocUnicast;  // echoed
    } PACK OamTkMcastRegResponse;


////////////////////////////////////////////////////////////////////////////////
// Key Exchange
////////////////////////////////////////////////////////////////////////////////

typedef struct
    {
    OamTkExt    ext;
    U8          keyNumber;              /*  key 0 or key 1 */
    U8          keyLength;              /*  number of bytes of key to follow */
    } PACK OamTkKeyExchange;


/*
*  File Transfer
*/

/* Read / Write Request */
typedef enum
    {
    OamTkFileBoot,
    OamTkFileApp,
    OamTkFilePers,
    OamTkFileDiag,

    OamTkFileNumTypes,
    OamTkFileInvalid
    } PACK OamTkFileType;

typedef struct
    {
    OamTkExt    ext;
    U8          file;
    } PACK OamTkFileRequest;

/* Data blocks are a sequence number followed by a number of bytes of data */
typedef struct
    {
    U16         blockNum;  /*  sequence number for this block */
    U16         size;      /*  number of bytes of data to follow this block */
    } PACK OamTkFileData;

typedef enum
    {
    OamTkFileErrOk,

    OamTkFileErrUndefined,
    OamTkFileErrNotFound,
    OamTkFileErrNoAccess,
    OamTkFileErrFull,
    OamTkFileErrIllegalOp,
    OamTkFileErrUnknownId,

    OamTkFileErrBadBlock,
    OamTkFileErrTimeout,
    OamTkFileErrBusy,
    OamTkFileErrIncompat

    } PACK OamTkFileErr;

/* acknowledge receipt of block blockNum */
typedef struct
    {
    OamTkExt    ext;
    U16         blockNum;       /* sequence number for this block */
    U8          err;
    } PACK OamTkFileAck;


/*
* I2C commands
*/

typedef enum
    {
    OamTkI2cErrOk,

    OamTkI2cErrFailed
    } PACK OamTkI2cErr;

typedef struct
    {
    OamTkExt         ext;
    U8               i2cDevId;  /* < Address of the I2C device */
    U16              readLen;   /* < # bytes to read  */

    U8               addrLen;   /* < # bytes of addr[] */
    U8               addr[1];   /* < address within the I2C device to read from */
    } PACK OamTkI2cRdReq;


typedef struct
    {
    OamTkExt        ext;
    U8              err;
    U16             len;        /* < # bytes in rdData[] */
    U8              rdData[1];  /* < bytes read from the I2c device  */
    } PACK OamTkI2cRdResp;


typedef struct
    {
    OamTkExt         ext;
    U8               i2cDevId;  /* < Address of the I2C device */
    U16              dataLen;   /* < # bytes of wrData[] */
    U8               wrData[1]; /* < data to write to the device */
    } PACK OamTkI2cWrReq;

typedef struct
    {
    OamTkExt        ext;
    OamTkI2cErr     err;
    } PACK OamTkI2cWrResp;



////////////////////////////////////////////////////////////////////////////////
// Os Access commands
////////////////////////////////////////////////////////////////////////////////

typedef enum
    {
    OamTkOsAccessNoErr,

    OamTkOsAccessErr
    } PACK OamTkOsAccessErrCode;

typedef struct
    {
    OamTkExt                ext;
    U8                      port;
    U32                     regAddr;
    U32                     len;
    } PACK OamTkOsAccessRdReq;

typedef struct
    {
    OamTkExt                ext;
    OamTkOsAccessErrCode    err;
    U8                      port;
    U32                     regAddr;
    U32                     len;
    U8                      value[1];
    } PACK OamTkOsAccessRdResp;

typedef struct
    {
    OamTkExt                ext;
    U8                      port;
    U32                     regAddr;
    U32                     len;
    U8                      value[1];
    } PACK OamTkOsAccessWrReq;

typedef struct
    {
    OamTkExt                ext;
    OamTkOsAccessErrCode    err;
    U8                      port;
    U32                     regAddr;
    U32                     len;
    } PACK OamTkOsAccessWrResp;


typedef struct
    {
    U16         Ver;        /* < Boot version */
    U32         Crc;        /* < Boot crc 32 */
	} PACK LoadInfo;

typedef struct
	{
	U16			eponVid;    		/* EPON VLAN ID */
	U16			userVid;    		/* User VLAN ID */
	U8			maxGroups;			/* Max allowed IGMP groups for this VLAN */
	} PACK IgmpVlanCfg;

typedef struct
	{
	BOOL			ignoreUnmanaged;    /* Action for unmanaged groups */
	U8				numVlans;    		/* Number of IGMP VLANs */
	IgmpVlanCfg	    vlanCfg[1];         /* IGMP port settings */
	} PACK IgmpVlanRecord;
	
	///value of the PON and user port mac address
typedef struct
    {
    U8      ponBaseMac[6];   // ONU base MAC address
    U8      userBaseMac[6];  // user port base MAC address
    } PACK  OamEponBaseMac;

///the oui and version of the oam which is running now
typedef struct
    {
    U8      oui[3];         // the oui of the oam running
    U16     version;        //the version of the oam running
    } PACK OamVersion;

typedef enum
    {
    EponAdminEnable  = 0,
    EponAdminDisable = 1,
    EponAdminCount
    } PACK OamEponAdmin;


typedef struct
    {
    U8 netEponPort;
    MacAddr mac;
    } PACK OamExtAttrNetworkOltReportContentsPortInfo;

typedef struct
    {
    U8 count;
    OamExtAttrNetworkOltReportContentsPortInfo portInfo[4];
    } PACK OamExtAttrNetworkOltReportContents;

typedef struct
    {
    U16 msToWait;
    MacAddr searchMac;
    } PACK OamExtActSearchMacRequest;


typedef struct
    {
    U8 count;
    U8 isOnPort[4];
    } PACK OamExtActSearchMacResponse;

typedef struct
    {
    U8      oamLinkEstablished;    // 1 - established, 0 - not
    U8      authorizationState;    // 1 - authorized, 0 - not
    U8      loopBack;              // 1 - loopback, 0 - normal
    U16     linkLlid;              // the LLID for the link
    } PACK OamLinkRegInfo;

typedef enum
    {
    OamRxOpticalLos       = 0,
    OamRxOpticalUnLock    = 1,
    OamRxOpticalLock      = 2,
    OamRxOpticalStateNum
    } PACK OamRxOpticalState;
	
	
typedef struct
    {
    BOOL                connection;   // True : connect, False : disconnect
    OamRxOpticalState   rxOptState;   // the state of Rx Optical
    U8                  linkNum;
    OamLinkRegInfo      linkInfo[1];
    } PACK OamEponStatus;


typedef struct
    {
    U8 queueCount;      //Count of queues / priority
    U8 priority[1];     //List of queue priority
    } PACK OamUniFifPriMap;


typedef struct
    {
    U16 time;
    } PACK OamMpcpTimeOut;


typedef struct
    {
    U8 loid[24];
    } PACK OamCtcAuthLoid;

typedef struct
    {
    U8 pwd[12];
    } PACK OamCtcAuthPwd;


typedef struct
    {
    U8 result;
    } PACK OamLoidAuthStauts;

typedef struct
    {
    U8 index;
    U8 layerSelect;
    U8 dwordOffset;
    U8 fldLength;
    U8 bitOffset;
    } PACK OamOnuFieldSelectV2;

typedef struct
    {
    U8 group;
    U8 record;
    U8 info[1];
    } PACK OamPersItem;



////////////////////////////////////////////////////////////////////////////////
// Chip version and ID enumerations
////////////////////////////////////////////////////////////////////////////////

#ifndef TeknovusChipTypes

#define TeknovusChipTypes

typedef enum
	{
	Tk3701ChipType = 0x3701,
	Tk3711ChipType = 0x3711,
	Tk3713ChipType = 0x3713,
	Tk3721ChipType = 0x3721
	} PACK TkChipTypes;

#endif /* TeknovusChipTypes */

#ifndef Tk3701XChipVersion
#define Tk3701XChipVersion 0xF0030130
#endif

#ifndef Tk3701YChipVersion
#define Tk3701YChipVersion 0xA0040513
#endif

    // Same as 3711-Y no software diffs
    // not possible for software to determine.
#ifndef Tk3701IpgChipVersion
#define Tk3701IpgChipVersion Tk3701YChipVersion
#endif

#ifndef Tk3711YChipVersion
#define Tk3711YChipVersion 0xA0040816
#endif

// WARNING: reported version different from register read
#ifndef Tk3711ZChipVersion
#define Tk3711ZChipVersion 0xA0050510
#endif

#ifndef Tk3721ChipVersion
#define Tk3721ChipVersion = 0xA0040219
#endif

typedef struct
    {
    U8  EponMac[6];     /* EPON MAC address */
    U32 onuModel;       /* ONU model */
    U16 chipModel;      /* chip ID */

    U16 app0Ver;        /* App0 version */
    U16 app1Ver;        /* App1 version */
    U16 bootVer;        /* Boot version */
    U16 persVer;        /* Personality version */
} PACK ONU_CFG_INFO;


typedef struct
    {
	U16	threshold[8];
} PACK DbaQueueSets;

typedef struct
    {
	U8 indexes;
	DbaQueueSets QueueSets[1];
} PACK CTC_DBA_CFG;


typedef struct
    {
	U8  report;
	U16	threshold[1];
} PACK CtcDbaQueueHdr;


#if defined(CPU_ARCH_ARM) || defined(CPU_ARCH_X86)
/* Restore packing to previous setting */
#pragma pack ()
#endif

#if defined(__cplusplus)
}
#endif

#endif /* Oam.h */

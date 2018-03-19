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


#if !defined(OamDpoe_h)
#define OamDpoe_h
////////////////////////////////////////////////////////////////////////////////
/// \file OamDpoe.h
/// \brief Definitions for DPoE extensions to 802.3ah OAM
////////////////////////////////////////////////////////////////////////////////


#include "Teknovus.h"
#include "Oam.h"
#include "Mpcp.h"


#if defined(__cplusplus)
extern "C" {
#endif

typedef enum
    {
    OamDpoeOpReserved       = 0,
    OamDpoeOpGetRequest     = 1,
    OamDpoeOpGetResponse    = 2,
    OamDpoeOpSetRequest     = 3,
    OamDpoeOpSetResponse    = 4,
    OamDpoeOpMcastRequest   = 5,
    OamDpoeOpMcastReg       = 6,
    OamDpoeOpMcastRegResp   = 7,
    OamDpoeOpKeyExchange    = 8,
    OamDpoeOpFileTransfer   = 9
    } PACK OamDpoeOpcode;


typedef enum
    {
    OamDpoeBranchTerminator = 0x00,
    OamDpoeBranchObject     = 0xD6,
    OamDpoeBranchAttr       = 0xD7,
    OamDpoeBranchAct        = 0xD9
    } PACK OamDpoeBranch;


typedef enum
    {
    OamDpoeObjOnu           = 0x0000,
    OamDpoeObjNetPon        = 0x0001,
    OamDpoeObjLink          = 0x0002,
    OamDpoeObjUserPort      = 0x0003,
    OamDpoeObjQueue         = 0x0004,
    OamDpoeObjMLink         = 0x0006,

    OamDpoeObjForce16       = 0x7FFF
    } PACK OamDpoeObjCtxt;


typedef struct
    {
    U8  link;
    } PACK OamDpoeLink;


typedef struct
    {
    OamDpoeObjCtxt  type;
    U8              inst;
    U8              queue;
    } PACK OamDpoeQueue;


typedef struct
    {
    U8  pon;
    } PACK OamDpoeNetPon;


typedef struct
    {
    U8  port;
    } PACK OamDpoeUserPort;

typedef struct
    {
    U16  mLlid;
    } PACK OamDpoeMLink;


typedef enum
    {
    // ONU Management
    OamDpoeAttrSequenceNumber       = 0x0001,
    OamDpoeAttrOnuId                = 0x0002,
    OamDpoeAttrFirmwareInfo         = 0x0003,
    OamDpoeAttrChipInfo             = 0x0004,
    OamDpoeAttrDateOfManufacture    = 0x0005,
    OamDpoeAttrManufacturerInfo     = 0x0006,
    OamDpoeAttrMaxLogicalLinks      = 0x0007,
    OamDpoeAttrNumEponPorts         = 0x0008,
    OamDpoeAttrNumUniPorts          = 0x0009,
    OamDpoeAttrPacketBuffer         = 0x000A,
    OamDpoeAttrReportThresholds     = 0x000B,
    OamDpoeAttrLinkState            = 0x000C,
    OamDpoeAttrOamRate              = 0x000D,
    OamDpoeAttrMfrName              = 0x000E,
    OamDpoeAttrFwareMfgTimeVarCtrl  = 0x000F,
    OamDpoeAttrPortType	            = 0x0010,
    OamDpoeAttrVendorName           = 0x0011,
    OamDpoeAttrModelNumber          = 0x0012,
    OamDpoeAttrHwVersion            = 0x0013,
    OamDpoeAttrEponMode             = 0x0014,

    // Bridging
    OamDpoeAttrDynLearnTableSize    = 0x0101,
    OamDpoeAttrDynLearnAgeLimit     = 0x0102,
    OamDpoeAttrDynMacTable          = 0x0103,
    OamDpoeAttrStaticMacTable       = 0x0104,
    OamDpoeAttrPortCapability       = 0x0105,
    OamDpoeAttrDynLearnMode         = 0x0106,
    OamDpoeAttrMinMacLimit          = 0x0107,
    OamDpoeAttrMaxMacAllowed        = 0x0108,
    OamDpoeAttrAggMacLimit          = 0x0109,
    OamDpoeAttrLenErrDiscard        = 0x010A,
    OamDpoeAttrFloodUnknown         = 0x010B,
    OamDpoeAttrLocalSwitching       = 0x010C,
    OamDpoeAttrQueueConfig          = 0x010D,
    OamDpoeAttrFirmwareFilename     = 0x010E,
    OamDpoeAttrMacFullBehavior      = 0x010F,
    OamDpoeAttrMulticastLlid        = 0x0110,  //DPoE2.0
    OamDpoeAttrUniMacLearned        = 0x0111,  //DPoE2.0

    // Statistics
    OamDpoeAttrRxMulticastFrames    = 0x0015,
    OamDpoeAttrRxBroadcastFrames    = 0x0016,
    OamDpoeAttrRxLenErrorFrames     = 0x0017,
    OamDpoeAttrRxOverSizeFrames     = 0x0019,    
    OamDpoeAttrRxPauseFrames        = 0x0063,        
    OamDpoeAttrRxUnicastFrames      = 0x0201,
    OamDpoeAttrTxUnicastFrames      = 0x0202,
    OamDpoeAttrRxFrameTooShort      = 0x0203,
    OamDpoeAttrRxFrame64            = 0x0204,
    OamDpoeAttrRxFrame65_127        = 0x0205,
    OamDpoeAttrRxFrame128_255       = 0x0206,
    OamDpoeAttrRxFrame256_511       = 0x0207,
    OamDpoeAttrRxFrame512_1023      = 0x0208,
    OamDpoeAttrRxFrame1024_1518     = 0x0209,
    OamDpoeAttrRxFrame1519Plus      = 0x020A,
    OamDpoeAttrTxFrame64            = 0x020B,
    OamDpoeAttrTxFrame65_127        = 0x020C,
    OamDpoeAttrTxFrame128_255       = 0x020D,
    OamDpoeAttrTxFrame256_511       = 0x020E,
    OamDpoeAttrTxFrame512_1023      = 0x020F,
    OamDpoeAttrTxFrame1024_1518     = 0x0210,
    OamDpoeAttrTxFrame1519Plus      = 0x0211,
    OamDpoeAttrDelayThresh          = 0x0212,
    OamDpoeAttrDelay                = 0x0213,
    OamDpoeAttrFramesDropped        = 0x0214,
    OamDpoeAttrBytesDropped         = 0x0215,
    OamDpoeAttrBytesDelayed         = 0x0216,
    OamDpoeAttrBytesUnused          = 0x0217,
    OamDpoeAttrRxDelayThresh        = 0x0218,
    OamDpoeAttrRxDelay              = 0x0219,
    RemOamDpoeAttrRxFramesDropped   = 0x021A,
    RemOamDpoeAttrRxBytesDropped    = 0x021B,
    OamDpoeAttrRxBytesDelayed       = 0x021C,
    OamDpoeAttrOptMonTemperature    = 0x021D,
    OamDpoeAttrOptMonVcc            = 0x021E,
    OamDpoeAttrOptMonTxBias         = 0x021F,
    OamDpoeAttrOptMonTxPower        = 0x0220,
    OamDpoeAttrOptMonRxPower        = 0x0221,
    
    OamDpoeAttrRxFramesYellow       = 0x0222,
    OamDpoeAttrTxFramesYellow       = 0x0223,
    OamDpoeAttrTxBytesGreen         = 0x0224,
    OamDpoeAttrRxBytesYellow        = 0x0225,
    OamDpoeAttrRxBytesGreen         = 0x0226,
    OamDpoeAttrTxBytesYellow        = 0x0227,
    
    OamDpoeAttrTxFramesUnicastL2    = 0x0228,
    OamDpoeAttrTxFramesMulticastL2  = 0x0229,
    OamDpoeAttrTxFramesBroadcastL2  = 0x022A,
    OamDpoeAttrRxFramesUnicastL2    = 0x022B,
    OamDpoeAttrRxFramesMulticastL2  = 0x022C,
    OamDpoeAttrRxFramesBroadcastL2  = 0x022D,
    OamDpoeAttrProgrammableCounters = 0x022E,
    
    OamDpoeAttrRxFramesL2cp         = 0x022F,
    OamDpoeAttrRxOctetsL2cp         = 0x0230,
    OamDpoeAttrTxFramesL2cp         = 0x0231,
    OamDpoeAttrTxOctetsL2cp         = 0x0232,
    OamDpoeAttrFramesDiscardedL2cp  = 0x0233,
    OamDpoeAttrOctetsDiscardedL2cp  = 0x0234,
    OamDpoeAttrTxL2Errors           = 0x0235,
    OamDpoeAttrRxL2Errors           = 0x0236,

    // Alarms
    OamDpoeAttrPortStatThresh       = 0x0301,
    OamDpoeAttrLinkStatThresh       = 0x0302,
    OamDpoeAttrSuspendResumeAlmRpt  = 0x0303, //DPoE 2.0

    // Security
    OamDpoeAttrKeyExpiryTime        = 0x0401,
    OamDpoeAttrEncryptMode          = 0x0402,

    // Frame Processing
    OamDpoeAttrPortIngressRule      = 0x0501,
    OamDpoeAttrLueField             = 0x0502,
    OamDpoeAttrAltCVlanEthertype    = 0x0503,
    OamDpoeAttrAltSVlanEthertype    = 0x0504,

    // SLA
    OamDpoeAttrBcRateLimit          = 0x0601,
    OamDpoeAttrEgressShaping        = 0x0602,
    OamDpoeAttrIngressPolicing      = 0x0603,
    OamDpoeAttrQueueCIR             = 0x0604,  //OamDpoeAttrQueueRateControl in DPoE1.0
    OamDpoeAttrQueueEIR             = 0x0606,
    OamDpoeAttrFecMode              = 0x0605,
    OamDpoeAttrQueueRLCapability    = 0x0608,

    //Clock
    OamDpoeAttrClockTransportCapability = 0x0701,
    OamDpoeAttrClockTransportEnable     = 0x0702,
    OamDpoeAttrTransferTime             = 0x0703,
    OamDpoeAttrPropagationParameters    = 0x0704,
    OamDpoeAttrRtt                      = 0x0705,

    // DEMARC
    OamDpoeAttrDacParams           = 0x0800,
    OamDpoeAttrDacFlags            = 0x0801,  //Not used for DEMARC 1.0
    OamDpoeAttrDacPwd              = 0x0802,  //Not used for DEMARC 1.0
    OamDpoeAttrDacEnable           = 0x0803,

    // UNI
    OamDpoeAttrUniEEEStatus        = 0x0820,
    OamDpoeAttrUniPoEStatus        = 0x0821,
    OamDpoeAttrUniMediaType        = 0x0822,

    // Protection
    OamDpoeAttrPsCap               = 0x0900,
    OamDpoeAttrPsCfg               = 0x0901,
    OamDpoeAttrHoldover            = 0x0903,

    OamDpoeAttrForce16              = 0x7FFF
    } PACK  OamDpoeAttrLeaf;

typedef enum
    {
    // ONU Management
    OamDpoeActResetOnu              = 0x0001,

    // Bridging
    OamDpoeActClearDynLearnTable    = 0x0101,
    OamDpoeActAddDynMacAddr         = 0x0102,
    OamDpoeActDelDynMacAddr         = 0x0103,
    OamDpoeActClearStaticLearnTable = 0x0104,
    OamDpoeActAddStaticMacAddr      = 0x0105,
    OamDpoeActDelStaticMacAddr      = 0x0106,
    OamDpoeActCfgMcastLLID          = 0x0107,

    // Statistics
    OamDpoeActClearStats            = 0x0201,

    // Alarms
    OamDpoeActRetrieveAlarms        = 0x0301, //DPoE 2.0

    // Frame Processing
    OamDpoeActClearIngressRules     = 0x0501,
    OamDpoeActAddIngressRules       = 0x0502,
    OamDpoeActDelIngressRules       = 0x0503,

    // SLA
    OamDpoeActEnableUserTraffic     = 0x0601,
    OamDpoeActDisableUserTraffic    = 0x0602,
    OamDpoeActLoopbackEnable        = 0x0603,
    OamDpoeActLoopbackDisable       = 0x0604,
    OamDpoeActLaserTxPowerOff       = 0x0605,

    OamDpoeActForce16               = 0x7FFF
    } PACK OamDpoeActLeaf;


typedef enum
    {
    OamDpoeErrNoError       = 0x80,
    OamDpoeErrTooLong       = 0x81,
    OamDpoeErrBadParams     = 0x86,
    OamDpoeErrNoResources   = 0x87,
    OamDpoeErrSystemBusy    = 0x88,
    OamDpoeErrUndetermined  = 0xA0,
    OamDpoeErrUnsupported   = 0xA1,
    OamDpoeErrCorrupted     = 0xA2,
    OamDpoeErrHwFailure     = 0xA3,
    OamDpoeErrOverflow      = 0xA4
    } PACK OamDpoeError;


#define OamDpoeMultipartLast    0x8000


typedef struct
    {
    OamDpoeOpcode   opCode;
    } PACK OamDpoeMsg;


typedef struct
    {
    OamDpoeMsg  msg;
    U8          flags;
    MpcpPhyLlid mcast;
    MpcpPhyLlid uni;
    } PACK OamDpoeMcastReg;


typedef struct
    {
    OamDpoeMsg  msg;
    U8          keyNumber;
    U8          keyLength;
    } PACK OamDpoeKeyExchange;


typedef enum
    {
    OamDpoeEncryptNone,
    OamDpoeEncrypt1Down,
    OamDpoeEncrypt10Down,
    OamDpoeEncrypt10Bi
    } PACK  OamDpoeEncryptMode;


typedef enum
    {
    OamInfoDpoeExtSupport   = 0
    } PACK OamInfoDpoeTlvType;


typedef struct
    {
    OamOpcode           type;
    U8                  length;
    IeeeOui             oui;
    OamInfoDpoeTlvType  tlvType;
    } PACK OamDpoeDiscoveryTlv;


typedef struct
    {
    U8  ifVer;
    } PACK OamDpoeExtSupportTlv;


typedef enum
    {
    OamDpoeAlmLos           = 0x11,
    OamDpoeAlmKeyExFail     = 0x12,
    OamDpoeAlmPortDisabled  = 0x21,
    OamDpoeAlmPowerFail     = 0x41,
    OamDpoeAlmStat          = 0x81,
    OamDpoeAlmOnuBusy       = 0x82,
    OamDpoeAlmMacTableOverflow = 0x83,

    OamDpoeAlmNone          = 0x00
    } PACK OamDpoeAlmCode;


typedef struct
    {
    OamEventType    type;
    U8              length;
    IeeeOui         oui;
    OamDpoeAlmCode  alm;
    U8              raised;
    OamDpoeObjCtxt  objType;
    U16             objInst;
    } PACK OamDpoeEventPdu;


typedef struct
    {
    OamDpoeEventPdu     pdu;
    U8  branch;
    U16 leaf;
    } PACK OamDpoeStatEvent;


typedef enum
    {
    OamDpoeFileReserved = 0,
    OamDpoeFileWrite    = 1,
    OamDpoeFileData     = 2,
    OamDpoeFileAck      = 3
    } PACK OamDpoeFileOpcode;


typedef struct
    {
    OamDpoeOpcode       opcode;
    OamDpoeFileOpcode   type;
    } PACK OamDpoeFileMsg;


typedef enum
    {
    OamDpoeFileBoot     = 0,
    OamDpoeFileApp      = 1,
    OamDpoeFileConfig   = 2,
    OamDpoeFileDiag     = 3,

    OamDpoeFileNumTypes
    } PACK OamDpoeFileType;


typedef struct
    {
    OamDpoeFileType type;
    } PACK OamDpoeFileRequest;


typedef struct
    {
    U16 num;
    U16 length;
    } PACK OamDpoeFileBlock;


typedef enum
    {
    OamDpoeFileErrOk        = 0,
    OamDpoeFileErrUndefined = 1,
    OamDpoeFileErrNotFound  = 2,
    OamDpoeFileErrNoAccess  = 3,
    OamDpoeFileErrFull      = 4,
    OamDpoeFileErrIllegalOp = 5,
    OamDpoeFileErrUnknownId = 6,
    OamDpoeFileErrBadBlock  = 7,
    OamDpoeFileErrTimeout   = 8,
    OamDpoeFileErrBusy      = 9,
    OamDpoeFileErrIncompat  = 10,
    OamDpoeFileErrCorrupt   = 11
    } PACK OamDpoeFileErr;


typedef struct
    {
    U16             blockNum;
    OamDpoeFileErr  err;
    } PACK OamDpoeFileAckPdu;

typedef enum
    {
    OamDpoeLenErrDisPassed,
    OamDpoeLenErrDisDiscard
    } PACK OamDpoeLenErrDis;

typedef struct
    {
    OamPortCapability maxCaps;
    OamPortCapability curCaps;
    } PACK OamDpoeI05PortCapability;

typedef enum
    {
    OamDpoeCfgMcastLlidActAdd     = 0x00,
    OamDpoeCfgMcastLlidActDel     = 0x01,
    OamDpoeCfgMcastLlidActDelAll  = 0x02
    } PACK OamDpoeCfgMcastLlidAct;

typedef enum
    {
    OamDpoeCfgMcastLlidTlvLenActOnly    = 0x01,
    OamDpoeCfgMcastLlidTlvLenActAndVal  = 0x03
    } PACK OamDpoeCfgMcastLlidTlvLen;


#define DpoeOamRateMaxRangeCheckHigh 25
#define DpoeOamRateMinRangeCheckHigh 10

#if defined(__cplusplus)
}
#endif


#endif // Oam.h


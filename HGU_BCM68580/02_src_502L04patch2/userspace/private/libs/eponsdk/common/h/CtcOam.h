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


#if !defined(CtcOam_h)
#define CtcOam_h
////////////////////////////////////////////////////////////////////////////////
/// \file CtcOam.c
/// \brief China Telecom extended OAM generic module
/// \author Jason Armstrong
/// \author Joshua Melcon
/// \date March 2, 2006
///
/// \todo
/// Finish this header.
////////////////////////////////////////////////////////////////////////////////

#include "Teknovus.h"
#include "Oam.h"
#include "Mpcp.h"
#include "Ethernet.h"
#include "SysInfo.h"

#include "TkPlatform.h"
#include "oam_util.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define OamCtc21AllEthPorts			0xFFFF
#define isAllEtherPorts(val) (((val == OamCtcAllEthPorts) || (val == OamCtc21AllEthPorts))?1:0)

////////////////////////////////////////////////////////////////////////////////
/// OamCtcOpcode - OAM opcodes for CTC extended OAM
///
/// The following codes define the extended OAM opcodes as defined in the China
/// Telecom EPON Technical Specification and Requirement version 1.0 Final.
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    OamCtcReserved              = 0x00,
    OamCtcExtVarRequest         = 0x01,
    OamCtcExtVarResponse        = 0x02,
    OamCtcSetRequest            = 0x03,
    OamCtcSetResponse           = 0x04,
    OamCtcAuthentication        = 0x05,
    OamCtcFileUpgrade           = 0x06,
    OamCtcChurning              = 0x09,
    OamCtcDba                   = 0x0A,
    OamCtcEvent                 = 0xFF
    } PACK OamCtcOpcode;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcBranch - OAM TLV Branch Identifiers
///
///
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    OamCtcBranchTermination     = 0x00,                 // Not in specification
    OamCtcBranchAttribute       = OamBranchAttribute,   // IEEE 802.3 Clause 30
    OamCtcBranchAction          = OamBranchAction,      // IEEE 802.3 Clause 30
    OamCtcBranchObjInst         = 0x36,                 // OAM attr/act context
    OamCtcBranchObjInst21       = 0x37,                 // OAM CTC2.1 attr/act context
    OamCtcBranchKtExtAttribute  = 0xA7,                 // KT ext. attribute
    OamCtcBranchKtExtAction     = 0xA9,                 // KT ext. action
    OamCtcBranchExtAttribute    = 0xC7,                 // CTC extended attr
    OamKtBranchExtAttribute     = 0xA7,                 // KT extended attr
    OamCtcBranchExtAction       = 0xC9,                 // CTC extended action
    OamKtBranchExtAction        = 0xA9,                 // KT extended action
    OamCtcBranchMax             = OamCtcBranchExtAction + 1
    } PACK OamCtcBranch;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcContext - OAM TLV Action/Attribute Context
///
///
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    OamCtcContextPort           = 0x01,
    OamCtcContextCard           = 0x02,
    OamCtcContextLlid           = 0x03,
    OamCtcContextPonIf          = 0x04,
    OamCtcContextCnu            = 0x05,
    OamCtcContextToU16          = 0x7fff
    } PACK OamCtcContext;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcAttr - OAM TLV Leaf Attribute Identifiers
///
///
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    ////////////////////////////////////////////////////////////////////////////
    // ONU Object Class Attributes, Leaf 0x0001-0x000F
    ////////////////////////////////////////////////////////////////////////////
    OamCtcAttrOnuSn             = 0x0001,
    OamCtcAttrFirmwareVer       = 0x0002,
    OamCtcAttrChipsetId         = 0x0003,
    OamCtcAttrOnuCap            = 0x0004,
    OamCtcAttrPowerMonDiagnosis = 0x0005,
    OamCtcAttrServiceSla        = 0x0006,
    OamCtcAttrOnuCap2           = 0x0007,
    OamCtcAttrOnuHoldover       = 0x0008,
    OamCtcAttrMxUMngGlobalParam = 0x0009,
    OamCtcAttrMxUMngSnmpParam   = 0x000A,
    OamCtcAttrActPonIFAdmin     = 0x000B,
    OamCtcAttrOnuCap3           = 0x000C,
    OamCtcAttrPowerSavingCap    = 0x000D,
    OamCtcAttrPowerSavingCfg    = 0x000E,
    OamCtcAttrOnuProtParam      = 0x000F,
    OamCtcAttrOnuTxPower        = 0x00A1,
    OamCtcAttrOnuMacAgingTime   = 0x00A4,

    ////////////////////////////////////////////////////////////////////////////
    // Port Object Class Attributes, Leaf 0x0011-0x001F
    ////////////////////////////////////////////////////////////////////////////
    OamCtcAttrEthLinkState      = 0x0011,
    OamCtcAttrEthPortPause      = 0x0012,
    OamCtcAttrEthPortPolice     = 0x0013,
    OamCtcAttrVoipPort          = 0x0014,
    OamCtcAttrE1Port            = 0x0015,
    OamCtcAttrEthPortDsRateLimit= 0x0016,
    OamCtcAttrPortLoopDetect    = 0x0017,
    OamCtcAttrPortDisableLooped = 0x0018,

    ////////////////////////////////////////////////////////////////////////////
    // VLAN Object Class Attributes, Leaf 0x0021-0x002F
    ////////////////////////////////////////////////////////////////////////////
    OamCtcAttrVlan              = 0x0021,

    ////////////////////////////////////////////////////////////////////////////
    // QoS Object Class Attributes, Leaf 0x0031-0x003F
    ////////////////////////////////////////////////////////////////////////////
    OamCtcAttrClassMarking      = 0x0031,

    ////////////////////////////////////////////////////////////////////////////
    // Mcast Object Class Attribute, Leaf 0x0041-0x004F
    ////////////////////////////////////////////////////////////////////////////
    OamCtcAttrMcastVlan         = 0x0041,
    OamCtcAttrMcastStrip        = 0x0042,
    OamCtcAttrMcastSwitch       = 0x0043,
    OamCtcAttrMcastCtrl         = 0x0044,
    OamCtcAttrGroupMax          = 0x0045,

    ////////////////////////////////////////////////////////////////////////////
    // Fast Leave Class Attribute, Leaf 0x0046-0x0047
    ////////////////////////////////////////////////////////////////////////////
    OamCtcAttrFastLeaveAbility  = 0x0046,
    OamCtcAttrFastLeaveState    = 0x0047,

    ////////////////////////////////////////////////////////////////////////////
    // LLID object Class Attribute, Leaf 0x0051-0x005F
    ////////////////////////////////////////////////////////////////////////////
    OamCtcAttrLlidQueueConfig   = 0x0051,

    ////////////////////////////////////////////////////////////////////////////
    // Alarm Class Attribute, Leaf 0x0046-0x0047
    ////////////////////////////////////////////////////////////////////////////
    OamCtcAttrAlarmAdminState   = 0x0081,
    OamCtcAttrPowerMonThreshold = 0x0082,

    ////////////////////////////////////////////////////////////////////////////
    // VoIP Class Attribute, Leaf 0x0061 - 0x006D
    ////////////////////////////////////////////////////////////////////////////
    OamCtcAttrIadInfo           = 0x0061,
    OamCtcAttrVoIPFirst             = OamCtcAttrIadInfo,
    OamCtcAttrGlobalParaConfig  = 0x0062,
    OamCtcAttrH248ParaConfig    = 0x0063,
    OamCtcAttrH248UserTidInfo   = 0x0064,
    OamCtcAttrH248RtpTidConfig  = 0x0065,
    OamCtcAttrH248RtpTidInfo    = 0x0066,
    OamCtcAttrSipParaConfig     = 0x0067,
    OamCtcAttrSipUserParaConfig = 0x0068,
    OamCtcAttrFaxModemConfig    = 0x0069,
    OamCtcAttrH248IadOperationStatus    = 0x006A,
    OamCtcAttrPotsStatus        = 0x006B,
    OamCtcAttrIadOperation      = 0x006C,
    OamCtcAttrSipDigitMap       = 0x006D,
    OamCtcAttrVoIPLast                  = 0x007F,

    ////////////////////////////////////////////////////////////////////////////
    // CMC Attribute
    ////////////////////////////////////////////////////////////////////////////
    OamCtcAttrRfTransceiverDiag = 0x0100,
    OamCtcAttrCnuList           = 0x0101,
    OamCtcAttrDnRfConfig        = 0x0102,
    OamCtcAttrUpRfConfig        = 0x0103,
    OamCtcAttrMaxUpRfAttenu     = 0x0104,
    OamCtcAttrMoveCnuDn         = 0x0105,
    OamCtcAttrMoveCnuUp         = 0x0106,
    OamCtcAttrDownSla           = 0x0107,

    ////////////////////////////////////////////////////////////////////////////
    // Performance Attribute
    ////////////////////////////////////////////////////////////////////////////
    OamCtcAttrPrfrmMontrStatus  = 0x00B1,
    OamCtcAttrPrfrmCurrtData    = 0x00B2,
    OamCtcAttrPrfrmHistrData    = 0x00B3,

    OamCtcAttrU16               = 0x7FFF
    } PACK OamCtcAttr;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcAlarmId - OAM CTC AlarmId
///
///
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    ////////////////////////////////////////////////////////////////////////////
    // ONU Object Class Attributes, AlarmId 0x0001-0x00FF
    ////////////////////////////////////////////////////////////////////////////
    OamCtcOnuAlarmBase                  = 0x0001,
    OamCtcAttrEquipmentAlarm            = OamCtcOnuAlarmBase,
    OamCtcAttrPowerAlarm                = 0x0002,
    OamCtcAttrBatteryMissing            = 0x0003,
    OamCtcAttrBatteryFailure            = 0x0004,
    OamCtcAttrBatteryVoltLow            = 0x0005,
    OamCtcAttrPhysicalInstusionAlarm    = 0x0006,
    OamCtcAttrOnuSelfTestFailure        = 0x0007,
    OamCtcAttrOnuTempHighAlarm          = 0x0009,
    OamCtcAttrOnuTempLowAlarm           = 0x000A,
    OamCtcAttrIadConnectionFailure      = 0x000B,
    OamCtcAttrPonIfSwitch               = 0x000C,
    OamCtcSleepStatusUpdate             = 0x000D,
    OamCtcOnuAlarmEnd                   = OamCtcSleepStatusUpdate,
    OamCtcOnuAlarmNums                  = OamCtcOnuAlarmEnd
                                        - OamCtcOnuAlarmBase + 1,

    ////////////////////////////////////////////////////////////////////////////
    // PON IF Object Class Attributes, AlarmId 0x0101-0x01FF
    ////////////////////////////////////////////////////////////////////////////
    //********* Alarm Section***********//
    OamCtcPonIfAlarmBase                = 0x0101,
    //======== RxPower ==========//
    OamCtcAttrPowerMonRxPowerAlmHigh    = OamCtcPonIfAlarmBase,
    OamCtcAttrPowerMonRxPowerAlmLow     = 0x0102,
    //======== TxPower ==========//
    OamCtcAttrPowerMonTxPowerAlmHigh    = 0x0103,
    OamCtcAttrPowerMonTxPowerAlmLow     = 0x0104,
    //========= TxBias ==========//
    OamCtcAttrPowerMonTxBiasAlmHigh     = 0x0105,
    OamCtcAttrPowerMonTxBiasAlmLow      = 0x0106,
    //=========== Vcc ==========//
    OamCtcAttrPowerMonVccAlmHigh        = 0x0107,
    OamCtcAttrPowerMonVccAlmLow         = 0x0108,
    //======= Temperature ========//
    OamCtcAttrPowerMonTempAlmHigh       = 0x0109,
    OamCtcAttrPowerMonTempAlmLow        = 0x010A,
    //********* Warn Section***********//
    //======== RxPower ==========//
    OamCtcAttrPowerMonRxPowerWarnHigh   = 0x010B,
    OamCtcAttrPowerMonRxPowerWarnLow    = 0x010C,
    //======== TxPower ==========//
    OamCtcAttrPowerMonTxPowerWarnHigh   = 0x010D,
    OamCtcAttrPowerMonTxPowerWarnLow    = 0x010E,
    //========= TxBias ==========//
    OamCtcAttrPowerMonTxBiasWarnHigh    = 0x010F,
    OamCtcAttrPowerMonTxBiasWarnLow     = 0x0110,
    //========== Vcc ===========//
    OamCtcAttrPowerMonVccWarnHigh       = 0x0111,
    OamCtcAttrPowerMonVccWarnLow        = 0x0112,
    //======= Temperature ========//
    OamCtcAttrPowerMonTempWarnHigh      = 0x0113,
    OamCtcAttrPowerMonTempWarnLow       = 0x0114,
    OamCtcPonIfAlarmEnd                 = OamCtcAttrPowerMonTempWarnLow,
    //Performance monitor
    OamCtcAttrDnStreamDropEvent         = 0x0115,
    OamCtcAttrUpStreamDropEvent         = 0x0116,
    OamCtcAttrDnStreamCRCErrFrames      = 0x0117,
    OamCtcAttrUpStreamCRCErrFrames      = 0x0118,
    OamCtcAttrDnStreamUndersizeFrames   = 0x0119,
    OamCtcAttrUpStreamUndersizeFrames   = 0x011A,
    OamCtcAttrDnStreamOversizeFrames    = 0x011B,
    OamCtcAttrUpStreamOversizeFrames    = 0x011C,
    OamCtcAttrDnStreamFragments         = 0x011D,
    OamCtcAttrUpStreamFragments         = 0x011E,
    OamCtcAttrDnStreamJabbers           = 0x011F,
    OamCtcAttrUpStreamJabbers           = 0x0120,
    OamCtcAttrDnStreamDiscards          = 0x0121,
    OamCtcAttrUpStreamDiscards          = 0x0122,
    OamCtcAttrDnStreamErrors            = 0x0123,
    OamCtcAttrUpStreamErrors            = 0x0124,
    OamCtcAttrDnStreamDropEventsWarn    = 0x0125,
    OamCtcAttrUpStreamDropEventsWarn    = 0x0126,
    OamCtcAttrDnStreamCRCErrFramesWarn  = 0x0127,
    OamCtcAttrUpStreamCRCErrFramesWarn  = 0x0128,
    OamCtcAttrDnStreamUndersizeFrmsWarn = 0x0129,
    OamCtcAttrUpStreamUndersizeFrmsWarn = 0x012A,
    OamCtcAttrDnStreamOversizeFrmsWarn  = 0x012B,
    OamCtcAttrUpStreamOversizeFrmsWarn  = 0x012C,
    OamCtcAttrDnStreamFragmentsWarn     = 0x012D,
    OamCtcAttrUpStreamFragmentsWarn     = 0x012E,
    OamCtcAttrDnStreamJabbersWarn       = 0x012F,
    OamCtcAttrUpStreamJabbersWarn       = 0x0130,
    OamCtcAttrDnStreamDiscardsWarn      = 0x0131,
    OamCtcAttrUpStreamDiscardsWarn      = 0x0132,
    OamCtcAttrDnStreamErrorsWarn        = 0x0133,
    OamCtcAttrUpStreamErrorsWarn        = 0x0134,
    OamCtcPonIfAlarmEnd30               = OamCtcAttrUpStreamErrorsWarn,
    OamCtcPonIfAlarmNums                = OamCtcPonIfAlarmEnd30
                                        - OamCtcPonIfAlarmBase + 1,

    ////////////////////////////////////////////////////////////////////////////
    // Card Object Class Attributes, AlarmId 0x0201-0x02FF
    ////////////////////////////////////////////////////////////////////////////
    OamCtcCardAlarmBase                 = 0x0201,
    OamCtcAttrCardAlarm                 = OamCtcCardAlarmBase,
    OamCtcAttrSelfTestFailure           = 0x0202,
    OamCtcCardAlarmEnd                  = OamCtcAttrSelfTestFailure,
    OamCtcCardAlarmNums                 = OamCtcCardAlarmEnd
                                        - OamCtcCardAlarmBase + 1,

    ////////////////////////////////////////////////////////////////////////////
    // Port Object Class Attributes, AlarmId 0x0301-0x05FF
    ////////////////////////////////////////////////////////////////////////////
    //********** Eth Section***********//
    OamCtcPortAlarmBase                 = 0x0301,
    OamCtcAttrEthPortAutoNegFailure     = OamCtcPortAlarmBase,
    OamCtcAttrEthPortLos                = 0x0302,
    OamCtcAttrEthPortFailure            = 0x0303,
    OamCtcAttrEthPortLoopback           = 0x0304,
    OamCtcAttrEthPortCongestion         = 0x0305,
    OamCtcPortAlarmEnd                  = OamCtcAttrEthPortCongestion,

    OamCtcAttrEthPortDnDropEvents       = 0x0306,
    OamCtcAttrEthPortUpDropEvents       = 0x0307,
    OamCtcAttrEthPortDnCRCErrFrms       = 0x0308,
    OamCtcAttrEthPortUpCRCErrFrms       = 0x0309,
    OamCtcAttrEthPortDnUndersizeFrms    = 0x030A,
    OamCtcAttrEthPortUpUndersizeFrms    = 0x030B,
    OamCtcAttrEthPortDnOversizeFrms     = 0x030C,
    OamCtcAttrEthPortUpOversizeFrms     = 0x030D,
    OamCtcAttrEthPortDnFragments        = 0x030E,
    OamCtcAttrEthPortUpFragments        = 0x030F,
    OamCtcAttrEthPortDnJabbers          = 0x0310,
    OamCtcAttrEthPortUpJabbers          = 0x0311,
    OamCtcAttrEthPortDnDiscards         = 0x0312,
    OamCtcAttrEthPortUpDiscards         = 0x0313,
    OamCtcAttrEthPortDnErrors           = 0x0314,
    OamCtcAttrEthPortUpErrors           = 0x0315,
    OamCtcAttrEthPortStatsChangeTimes   = 0x0316,
    OamCtcAttrEthPortDnDropEventsWarn   = 0x0317,
    OamCtcAttrEthPortUpDropEventsWarn   = 0x0318,
    OamCtcAttrEthPortDnCRCErrFrmsWarn   = 0x0319,
    OamCtcAttrEthPortUpCRCErrFrmsWarn   = 0x031A,
    OamCtcAttrEthPortDnUndersizeFrmsWarn= 0x031B,
    OamCtcAttrEthPortUpUndersizeFrmsWarn= 0x031C,
    OamCtcAttrEthPortDnOversizeFrmsWarn = 0x031D,
    OamCtcAttrEthPortUpOversizeFrmsWarn = 0x031E,
    OamCtcAttrEthPortDnFragmentsWarn    = 0x031F,
    OamCtcAttrEthPortUpFragmentsWarn    = 0x0320,
    OamCtcAttrEthPortDnJabbersWarn      = 0x0321,
    OamCtcAttrEthPortUpJabbersWarn      = 0x0322,
    OamCtcAttrEthPortDnDiscardsWarn     = 0x0323,
    OamCtcAttrEthPortUpDiscardsWarn     = 0x0324,
    OamCtcAttrEthPortDnErrorsWarn       = 0x0325,
    OamCtcAttrEthPortUpErrorsWarn       = 0x0326,
    OamCtcAttrEthPortStatChangeTimesWarn= 0x0327,
    OamCtcPortAlarmEnd30                = OamCtcAttrEthPortStatChangeTimesWarn,
    OamCtcPortAlarmNums                 = OamCtcPortAlarmEnd30
                                        - OamCtcPortAlarmBase + 1,

    //********* Pots Section***********//
    OamCtcPotsAlarmBase                 = 0x0401,
    OamCtcAttrPotsPortFailure           = OamCtcPotsAlarmBase,
    OamCtcPotsAlarmEnd                  = OamCtcAttrPotsPortFailure,
    OamCtcPotsAlarmNums                 = OamCtcPotsAlarmEnd
                                        - OamCtcPotsAlarmBase + 1,
    //********** E1 Section***********//
    OamCtcE1AlarmBase                   = 0x0501,
    OamCtcAttrE1PortFailure             = OamCtcE1AlarmBase,
    OamCtcAttrE1TimingUnlock            = 0x0502,
    OamCtcAttrE1Los                     = 0x0503,
    OamCtcE1AlarmEnd                    = OamCtcAttrE1Los,
    OamCtcE1AlarmNums                   = OamCtcE1AlarmEnd
                                        - OamCtcE1AlarmBase + 1,

    ////////////////////////////////////////////////////////////////////////////
    // CMC Events
    ////////////////////////////////////////////////////////////////////////////
    OamCtcAttrCnuRegEvent               = 0x0601,

    OamCtcAlarmInvalid                  = 0x7FFF
    } PACK OamCtcAlarmId;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcOnuPowerAlarmCode - The Failure code of the CTC ONU Power alarm
///
///
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    OamCtcOnuPowerAlarmPowerDown        = 0x00000001,
    OamCtcOnuPowerAlarmVoltOver         = 0x00000002,
    OamCtcOnuPowerAlarmVoltLess         = 0x00000003,

    OamCtcOnuPowerAlarmCodeNum
    } PACK OamCtcOnuPowerAlarmCode;

typedef enum
    {
    OamCtcPonIfSwichAlarmSigLos         = 0x00000001,
    OamCtcPonIfSwichAlarmSigDegrade     = 0x00000002,
    OamCtcPonIfSwichAlarmHwFault        = 0x00000003,
    } PACK OamCtcPonIfSwitchAlarmCode;

////////////////////////////////////////////////////////////////////////////////
/// OamCtcAct - OAM TLV Leaf Action Identifiers
///
///
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    OamCtcActReset              = 0x0001,
    OamCtcActSleepControl       = 0x0002,

    // New Attribute in CTC2.0, Leaf 0x0048
    OamCtcAttrFastLeaveAdminCtl = 0x0048,

    OamCtcActMultiLlidAdminCtl  = 0x0202,

    OamCtcActResetCard          = 0x0401,

    OamCtcActU16                = 0x7FFF
    } PACK OamCtcAct;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcKtExtAttr : KT extension attribute
///
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    OamCtcKtExtAttrOnuDsShaping           = 0x0001,
    OamCtcKtExtAttrOnuMacLimit            = 0x0002,
    OamCtcKtExtAttrBlockOnu               = 0x0003,
    OamCtcKtExtAttrOnuDiagnostics         = 0x0004,
    OamCtcKtExtAttrOnuGetQueueDropCounter = 0x0005,
    OamCtcKtExtAttrEtherPortGetCounter    = 0x0011,
    OamCtcKtExtAttrEtherPortRstp          = 0x0012,
    OamCtcKtExtAttrLoopDetect             = 0x0013,
    OamCtcKtExtAttrGetOnuMpcpCounter      = 0x0014,
    } PACK OamCtcKtExtAttr;

////////////////////////////////////////////////////////////////////////////////
/// OamCtcKtExtAct : KT extension actions
///
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    OamCtcKtExtActOnuCounterClear   = 0x0001,
    OamCtcKtExtActDefaultRestoreOnu = 0x0002,
    OamCtcKtExtActTxPowerOff        = 0x0003,
    } PACK OamCtcKtExtAct;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcFastLeaveMode - the CTC fast leave mode
///
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    OamCtcIgmpSnoopNoFast       = 0x00000000,
    OamCtcIgmpSnoopFastLeave    = 0x00000001,
    OamCtcHostCtlNoFast         = 0x00000010,
    OamCtcHostCtlFastLeave      = 0x00000011
    } PACK OamCtcFastLeaveMode;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcFastLeaveAdminState - the CTC fast leave admin state
///
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    OamCtcFastLeaveDisable      = 0x01,
    OamCtcFastLeaveEnable       = 0x02
    } PACK  OamCtcFastLeaveAdmin;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcActionU16 - the CTC action
///
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    OamCtcActionU16Disable      = 0x0001,
    OamCtcActionU16Enable       = 0x0002
    } PACK  OamCtcActionU16;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcAction - the CTC action
///
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    OamCtcActionDisable         = 0x00000001UL,
    OamCtcActionEnable          = 0x00000002UL
    } PACK OamCtcAction;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcAlarmState - the CTC alarm state
///
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    OamCtcAlarmDisable          = 0x00000001UL,
    OamCtcAlarmEnable           = 0x00000002UL
    } PACK OamCtcAlarmState;



////////////////////////////////////////////////////////////////////////////////
/// OamCtcObjType - the CTC object type
///
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    OamCtcObjPort               = 0x0001,
    OamCtcObjCard,
    OamCtcObjLlid,
    OamCtcObjPon,
    OamCtcObjOnu                = 0xFFFF,
    OamCtcObjErr                = 0xABCD,
    OamCtcObjUnused             = 0x7FFF
    } PACK OamCtcObjType;

////////////////////////////////////////////////////////////////////////////////
/// OamCtcAlarmType - the CTC alarm type
///
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    OamCtcAlarmOnu,
    OamCtcAlarmPonIf,
    OamCtcAlarmCard,
    OamCtcAlarmEth,
    OamCtcAlarmPots,
    OamCtcAlarmE1
    } PACK OamCtcAlarmType;
////////////////////////////////////////////////////////////////////////////////
/// OamCtcPortType - the CTC port type
///
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    OamCtcPortEth               = 0x01,
    OamCtcPortVoIP,
    OamCtcPortADSL2,
    OamCtcPortVDSL2,
    OamCtcPortE1
    } PACK OamCtcPortType;

//##############################################################################
// CTC OAM TLV Auxillary Type Definitions
//##############################################################################

#define OamCtcEponPortNum           0x00
#define OamCtcEthPortStart          0x01
#define OamCtcEthPortEnd            0x4F
#define OamCtcPotsPortStart         0x50
#define OamCtcPotsPortEnd           0x8F
#define OamCtcPotsPortExtLenStart   0x4050
#define OamCtcPotsPortExtLenEnd     0x408F
#define OamCtcE1PortStart           0x90
#define OamCtcE1PortEnd             0x9F
#define OamCtcE1PortExtLenStart     0x4090
#define OamCtcE1PortExtLenEnd       0x409F
#define OamCtcAllEthPorts           0xFF

#define OamCtcAlmOnuStart           0x0001
#define OamCtcAlmOnuEnd             0x00FF
#define OamCtcAlmPonIfStart         0x0101
#define OamCtcAlmPonIfEnd           0x01FF
#define OamCtcAlmCardStart          0x0201
#define OamCtcAlmCardEnd            0x02FF
#define OamCtcAlmEthStart           0x0301
#define OamCtcAlmEthEnd             0x03FF
#define OamCtcAlmPotsStart          0x0401
#define OamCtcAlmPotsEnd            0x04FF
#define OamCtcAlmE1Start            0x0501
#define OamCtcAlmE1End              0x05FF
#define OamCtcAllUniPorts           0xFFFF
#define OamCtcOnuInstNum            0xFFFFFFFFUL
#define OamCtcAlarmNoAlarmInfo      0x7FFFFFFFUL


////////////////////////////////////////////////////////////////////////////////
/// OamCtcPortNum - Port number used in CTC port object TLVs
///
/// There is an offset port number scheme for port messages in CTC OAM as
/// follows:
///
///     - 0x00 to 0x4F: Ethernet ports, EPON port is port 0
///     - 0x50 to 0x8F: VoIP ports
///     - 0x90 to 0x9F: E1 ports
////////////////////////////////////////////////////////////////////////////////
typedef U8 OamCtcPortNum;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcLinkState - Ethernet port status
///
/// The following is reported on EthLinkStatus message (leaf 0x11).
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    OamCtcLinkDown              = 0x00,
    OamCtcLinkUp                = 0x01
    } PACK  OamCtcLinkState;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcServices - ONU services supported
///
/// The following is used by the ONU Capabilities message (leaf 0x0004).  These
/// values are ORed together to determine the possible capabities of an ONU.
/// The number of ports that a system has for each service is in a separate
/// portion of the message.  It would seems that a service can be supported even
/// if there is not a dedicated port on the system to handle the service.
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    OamCtcServNoServices        = 0x00,
    OamCtcServSupportGe         = 0x01,
    OamCtcServSupportFe         = 0x02,
    OamCtcServSupportVoip       = 0x04,
    OamCtcServSupportTdm        = 0x08
    } PACK  OamCtcServices;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcOnuType - ONU Type
///
/// This type is used to present Onu type
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    OamCtcOnuSfu                = 0x00,
    OamCtcOnuHgu                = 0x01,
    OamCtcOnuSbu                = 0x02,
    OamCtcOnuCassetteMdu        = 0x03,
    OamCtcOnuMiniCardMdu        = 0x04,
    OamCtcOnuCassetteCardMdu    = 0x05,
    OamCtcOnuFrameCardMdu       = 0x06,
    OamCtcOnuCardMdu            = 0x07,
    OamCtcOnuMtu                = 0x08,
    OamCtcOnuCmc                = 0x09,
    OamCtcOnuNum                = 0x0A
    } PACK  OamCtcOnuType;

#define CtcCmcOnuVidBit 0x800

#define onuCapV2OnuTypeMsk      0xff000000
#define onuCapV2OnuTypeSft      24

////////////////////////////////////////////////////////////////////////////////
/// OamCtcOnuMultiLlid - ONU Multi Llid capability
///
/// This type is used to present ONU Multi Llid capability
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    OamCtcOnumultipleLlid       = 0x00,
    OamCtcOnuSingleLlid         = 0x01
    } PACK  OamCtcOnuMultiLlid;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcOnuProtectionType - ONU Protection Type
///
/// This type is used to present Onu protection type
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    OamCtcOnuUnSupport          = 0x00,
    OamCtcOnuTypeC              = 0x01,
    OamCtcOnuTypeD              = 0x02,
    OamCtcOnuTypeNum            = 0x03
    } PACK  OamCtcOnuProtectionType;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcOnuInterfaceType - ONU Interface Type
///
/// This type is used to present Onu interface type
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    OamCtcOnuIfGe               = 0x00,
    OamCtcOnuIfFe               = 0x01,
    OamCtcOnuIfVoIP             = 0x02,
    OamCtcOnuIfTdm              = 0x03,
    OamCtcOnuIfAdsl2            = 0x04,
    OamCtcOnuIfVdsl2            = 0x05,
    OamCtcOnuIfWlan             = 0x06,
    OamCtcOnuIfUsb              = 0x07,
    OamCtcOnuIfCatvRf           = 0x08,
    OamCtcOnuIfNum              = 0x09
    } PACK  OamCtcOnuInterfaceType;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcPortLock - CTC OAM port enable/disable type
///
/// This type is passed in the VoIP and E1 port management message to enable or
/// disable a given port.
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    OamCtcPortLock              = 0x00,
    OamCtcPortUnlock            = 0x01
    } PACK  OamCtcPortState;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcVlanMode - CTC OAM VLAN mode
///
/// These are the various VLAN modes that need to be supported by CTC ONUs.  The
/// VLAN stacking mode is currently options.
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    OamCtcVlanTransparent       = 0x00,
    OamCtcVlanTag               = 0x01,
    OamCtcVlanTranslation       = 0x02,
    OamCtcVlanAggregation       = 0x03,
    OamCtcVlanTrunk                 = 0x04,
    OamCtcVlanInit                     = 0x05,
    OamCtcVlanSpecialTransparent= 0xFF,
    OamCtcNumVlans = 0x06
    } PACK  OamCtcVlanMode;


typedef enum
    {
    OamCtcFieldDaMac,
    OamCtcFieldSaMac,
    OamCtcFieldVlanPri,
    OamCtcFieldVlanId,
    OamCtcFieldEthertype,
    OamCtcFieldDestIp,
    OamCtcFieldSrcIp,
    OamCtcFieldIpType,
    OamCtcFieldIpTos,
    OamCtcFieldIpPrec,
    OamCtcFieldL4SrcPort,
    OamCtcFieldL4DestPort,
    OamCtcFieldIPv4v6Version,
    OamCtcFieldIPv6FlowLable,
    OamCtcFieldIPv6DestAddr,
    OamCtcFieldIPv6SrcAddr,
    OamCtcFieldIPv6DestPrefix,
    OamCtcFieldIPv6SrcPrefix,
    OamCtcFieldIPv6NextHeader,
    OamCtcNumFieldSelects,
    } PACK  OamCtcFieldSelect;

typedef enum
    {
    OamCtcRuleOpFalse,
    OamCtcRuleOpEqual,
    OamCtcRuleOpNotEq,
    OamCtcRuleOpLtEq,
    OamCtcRuleOpGtEq,
    OamCtcRuleOpExist,
    OamCtcRuleOpNotEx,
    OamCtcRuleOpTrue,
    OamCtcNumRuleOp
    } PACK  OamCtcRuleOp;

typedef union
	{
	U8      byte[16];
	U16     word[8];
	U8      u8[16]; 
	} PACK OamCtcMatchValue;

typedef struct
    {
    OamCtcFieldSelect       field;
    OamCtcMatchValue        matchVal;
    OamCtcRuleOp            op;
    } PACK  OamCtcRuleCondition;

typedef enum
    {
    OamCtcRuleActionDel         = 0x00,
    OamCtcRuleActionAdd         = 0x01,
    OamCtcRuleActionClear       = 0x02,
    OamCtcRuleActionList        = 0x03
    } PACK  OamCtcRuleAction;

typedef struct
	{
	U8						prec;
	U8						length;
	U8						queueMapped;
	U8						pri;
	U8						numClause;
	} PACK OamCtcRuleNoClause;

typedef struct
    {
    U8                      prec;
    U8                      length;
    U8                      queueMapped;
    U8                      pri;
    U8                      numClause;
    U8                      clause[1];
    } PACK  OamCtcRule;

////////////////////////////////////////////////////////////////////////////////
/// OamCtcMcastVlanOp - CTC OAM multicast vlan operations
///
/// A group of VIDs may be added or deleted from a port at one type.  This type
/// defines the operation of that message.
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    OamCtcMcastVlanDel          = 0x00,
    OamCtcMcastVlanAdd          = 0x01,
    OamCtcMcastVlanClear        = 0x02,
    OamCtcMcastVlanList         = 0x03
    } PACK  OamCtcMcastVlanOp;

typedef enum
    {
    OamCtcMcastGroupDel         = 0x00,
    OamCtcMcastGroupAdd         = 0x01,
    OamCtcMcastGroupClear       = 0x02,
    OamCtcMcastGroupList        = 0x03
    } PACK OamCtcMcastGroupOp;

typedef enum
    {
    OamCtcMcastIgmpMode         = 0x00,
    OamCtcMcastHostMode         = 0x01,
    OamCtcMcastInvalidMode      = 0x7F
    } PACK  OamCtcMcastMode;

typedef enum
    {
    OamCtcMcastStripTagDisable          = 0x00,
    OamCtcMcastStripTagEnable           = 0x01,
    OamCtcMcastSwitchTagMode            = 0x02
    } PACK  OamCtcMcastTagMode;

#define OamCtcMaxMcastVlanSwitch             8
#define OamCtcMcastCtrlToBeContinued        0x80

/// TODO: find generic place to put these types ################################
typedef enum
    {
    McastDaMacOnly                = 0x00,
    McastDaMacVid                 = 0x01,
    McastDaMacSaMac               = 0x02,
    McastDaMacSipv4v6OamType      = 0x02,
    McastDipVid                   = 0x03,
    McastDipv6Vid                 = 0x04,
    McastDaMacSipv4v6HostType     = 0x05,
    McastDaMacSipv4v6HostTypeV2   = 0x06,
    McastDaMacSipv6HostType       = 0x07,
    McastCtrlNumModes
    } PACK  McastCtrl;

/// END TODO ###################################################################


//##############################################################################
// CTC OAM TLV Data Definitions
//##############################################################################

////////////////////////////////////////////////////////////////////////////////
/// OamCtcInstNumS - the CTC instance  number struct
///
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    U8                  portType;
    U8                  slotNum;
    U16                 portNum;
    } PACK OamCtcInstNumS;

////////////////////////////////////////////////////////////////////////////////
/// OamCtcInstNumU - the CTC instance number union
///
////////////////////////////////////////////////////////////////////////////////
typedef union
    {
    OamCtcInstNumS      sNum;
    U32                 uNum;
    } PACK OamCtcInstNumU;

////////////////////////////////////////////////////////////////////////////////
/// OamCtcObject - the CTC current object
///
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    CtcObjVer20,
    CtcObjVer21
    } PACK  CtcObjVer;
typedef struct
    {
    CtcObjVer           ctcVer;
    OamCtcObjType       objType;
    OamCtcInstNumU      instNum;
    } PACK OamCtcObject;


// Onu Alarm Bitmap Base Definitions
#define CtcAlarmBase                0x00000001UL
#define TkOnuPortNotUsed            0
#define TkOnuPonNumIndex            0

#if OAM_SWITCH
#define CurOnuNumEthPorts           8
#else
#define CurOnuNumEthPorts           MAX_UNI_PORTS
#endif

#define CurOnuNumPonPorts           1


////////////////////////////////////////////////////////////////////////////////
/// OamEventCtcAlarm - the CTC Alarm
///
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    OamEventExt         ext;
    U16                 objType;
    OamCtcInstNumU      instNum;
    U16                 almID;
    U16                 timeStamp;
    U8                  state;
    } PACK OamCtcEventTlvHead;


typedef struct
    {
    OamCtcEventTlvHead  head;
    U8                  info[6];
    U32                 dnPrimaryChannel;
    U32                 upPrimaryChannel;
    } PACK  OamCtcEventAlarm;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcTlvOnuSn - ONU serial number
///
/// Object: ONU
/// Leaf:   OamCtcAttrOnuSn (0x0001)
////////////////////////////////////////////////////////////////////////////////
typedef struct
      {
      U32                           vendorId;
      U32                           onuModel;
      MacAddr                       onuId;
      U8                            hardVersion[8];
      U8                            softVersion[16];
      U8                            ExtOnuModel[16];
      } PACK  OamCtcTlvOnuSn;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcTlvFirmwareVer - ONU firmware version
///
/// Object: ONU
/// Leaf:   OamCtcAttrFirmwareVer (0x0002)
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    SwVersion           version;
    } PACK  OamCtcTlvFirmwareVer;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcTlvChipsetId - ONU chipset identification
///
/// Object: ONU
/// Leaf:   OamCtcAttrChipsetId (0x0003)
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    TkJedecId           vendorId;
    TkChipId            chipModel;
    TkChipVersion       revisionDate;
    } PACK  OamCtcTlvChipsetId;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcTlvOnuCap - ONU capabilities
///
/// Object: ONU
/// Leaf:   OamCtcAttrOnuCap (0x0004)
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    OamCtcServices      services;
    U8                  numGePorts;
    U8                  geBitmap[8];
    U8                  numFePorts;
    U8                  feBitmap[8];
    U8                  numPotsPorts;
    U8                  numE1Ports;
    U8                  numUpQueue;
    U8                  upQueueMax;
    U8                  numDnQueue;
    U8                  dnQueueMax;
    BOOL                batteryBack;
    } PACK  OamCtcTlvOnuCap;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcTlvOnuIf - ONU interface type
///
/// Object: ONU
/// Leaf:   OamCtcAttrOnuCap (0x0007)
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    U32                 ifType;
    U16                 numPort;
    } PACK  OamCtcTlvOnuIf;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcTlvOnuCap - ONU capabilities2
///
/// Object: ONU
/// Leaf:   OamCtcAttrOnuCap2 (0x0007)
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    U32                 onuType; /* this is NOT byte-swapped! */
    U8                  multiLlid; /* 0x0 = invalid;
                                                          0x1 = single;
                                                          n = max num*/
    U8                  protection;
    U8                  numPonIf;
    U8                  numSlot;
    U8                  numIfType;
    OamCtcTlvOnuIf      ifs[1];
    } PACK  OamCtcTlvOnuCap2;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcTlvOnuCap3 - ONU capabilities 3
///
/// Object: ONU
/// Leaf:   OamCtcAttrOnuCap3 (0x000C)
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    OamCtcPowerNoSupported        = 0x00,
    OamCtcPowerJointCtrl          = 0x01,
    OamCtcPowerIndepentCtrl       = 0x02
   } PACK  OamCtcPowerCtrl;

typedef struct
    {
    BOOL                ip6Supported;
    OamCtcPowerCtrl     powerCtrl;
    U8                  ServiceSla;
    } PACK  OamCtcTlvOnuCap3;


////////////////////////////////////////////////////////////////////////////////
/// old version: OamCtcTlvOnuCap3 - ONU capabilities3
/// for CTC<3.0 and CUC<=3.0 use
/// Object: onu
/// Leaf:  OamCtcAttrOnuCap3 (0x000C)
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    BOOL                ip6Supported;
    OamCtcPowerCtrl     powerCtrl;
    } PACK  OamCtcTlvOnuCap3old;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcTlvOltCap - ONU capabilities2
///
/// Object: ONU
/// Leaf:   OamCtcAttrOltCap2 (0x0007)
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    U32                 onuType; /* this is NOT byte-swapped! */
    U8                  multiLlidAbility; /* 0x0 = multi; 0x1 = single*/
    U8                  protection;
    U8                  numPonIf;
    U8                  numSlot;
    U8                  numIfType;
    struct
        {
        U32 ifType;
        U16 numPort;
        } PACK  ifs[9];
    BOOL                batteryBack;
    U8                  numOfUpChannels;
    U8                  numOfDnChannels;
    U8                  numOfSupportedCnus;
    } PACK  OamCtcTlvOltCap2;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcTlvOnuPowerSavingCap - ONU power saving capabilities
///
/// Object:  ONU
/// Leaf:     OamCtcAttrPowerSavingCap (0x000D)
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    OamCtcSleepModeCapNotSupport    = 0x00,
    OamCtcSleepModeCapTx            = 0x01,
    OamCtcSleepModeCapTRx           = 0x02,
    OamCtcSleepModeCapTx_TRx        = 0x03
    } PACK  OamCtcSleepModeCap;

typedef enum
    {
    OamCtcEarlyWakeupCapSupport     = 0x00,
    OamCtcEarlyWakeupCapNotSupport  = 0x01
    } PACK  OamCtcEarlyWakeupCap;


typedef struct
    {
    OamCtcSleepModeCap      sleepModeCap;
    OamCtcEarlyWakeupCap    earlyWakeCap;
    } PACK  OamCtcTlvPowerSavingCap;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcTlvOnuPowerSavingCfg - ONU power saving Configuration
///
/// Object:  ONU
/// Leaf:     OamCtcAttrPowerSavingCfg (0x000E)
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    OamCtcEarlyWakeEnable           = 0x00,
    OamCtcEarlyWakeDisable          = 0x01,
    OamCtcEarlyWakeNotSupport       = 0xFF
    } PACK  OamCtcEarlyWakeCfg;


typedef struct
    {
    OamCtcEarlyWakeCfg  earlyWake;
    U48                 maxDuration;
    } PACK  OamCtcTlvPowerSavingCfg;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcTlvOnuSleepCtrl - ONU sleep control Configuration
///
/// Object:  ONU
/// Leaf:     OamCtcActSleepCtrl (0x0002)
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    OamCtcSleepFlagLeave            = 0x00,
    OamCtcSleepFlagEnter            = 0x01,
    OamCtcSleepFlagModify           = 0x02
    } PACK  OamCtcSleepFlag;


typedef enum
    {
    OamCtcSleepModeNone             = 0x00,
    OamCtcSleepModeTxOnly           = 0x01,
    OamCtcSleepModeTRx              = 0x02
    } PACK  OamCtcSleepMode;


typedef struct
    {
    U32              sleepDuration;
    U32              wakeDuration;
    OamCtcSleepFlag  sleepCtrlFlag;
    OamCtcSleepMode  sleepCtrlMode;
    } PACK  OamCtcTlvActSleepCtrl;


typedef enum
    {
    SleepFailEarlyWake  = 0x01,
    SleepFailTimeout    = 0x02,
    SleepFailDisable    = 0x03
    } PACK  OamCtcSleepFailureCode;


////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    OamCtcTlvOnuSn sn;
    U8 fwVer[129];
    OamCtcTlvChipsetId chipId;
    union
        {
        OamCtcTlvOnuCap v1;
        OamCtcTlvOltCap2 v2;
        } PACK  onuCap;
    OamCtcTlvOnuCap3 onuCap3;//ctc2.1 and above, CU
    OamCtcTlvPowerSavingCap powerSavingCap;
    } PACK HostCtcOamInfo;


////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    U32 holdoverFlag;    // the holdover flag
    U32 holdoverTime;    // the holdover time
    } PACK  OamCtcHoldover;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcTlvEthLinkState - ONU ethernet port status
/// Object: Port
/// Leaf:   OamCtcAttrEthLinkState (0x11)
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    OamCtcLinkState     state;
    } PACK  OamCtcTlvEthLinkState;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcTlvEthPortPause - ONU port flow control configuration
///
/// Object: Port
/// Leaf:   OamCtcAttrEthPortPause (0x0012)
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    BOOL                enabled;
    } PACK  OamCtcTlvEthPortPause;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcTlvEthPortPolice - ONU port rate policying configuration
///
/// Object: Port
/// Leaf:   OamCtcAttrEthPortPolice (0x0013)
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    BOOL                enabled;
    U8                  cir[3];
    U8                  cbs[3];
    U8                  ebs[3];
    } PACK  OamCtcTlvEthPortPolice;

////////////////////////////////////////////////////////////////////////////////
/// OamCtcTlvEthPortDsRateLimiting - ONU port downstream rate limiting configuration
///
/// Object: Port
/// Leaf:   OamCtcAttrEthPortDsRateLimiting (0x0016)
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    BOOL                enabled;
    U8                  cir[3];
    U8                  pir[3];
    } PACK  OamCtcTlvEthPortDsRateLimiting;

////////////////////////////////////////////////////////////////////////////////
/// OamCtcTlvPortLoopDetect - ONU port loop detect configuration
///
/// Object: Port
/// Leaf:   OamCtcAttrPortLoopDetect (0x0017)
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    U32                 enabled;
    } PACK  OamCtcTlvPortLoopDetect;


typedef enum
    {
    OamCtcLoopedPortEnable = 0x00,
    OamCtcLoopedPortDisable = 0x01
    } PACK  OamCtcLoopedPortAction;

////////////////////////////////////////////////////////////////////////////////
/// OamCtcTlvLoopedPortDisable - ONU looped port disable configuration
///
/// Object: Port
/// Leaf:   OamCtcAttrPortLoopDetect (0x0018)
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    U32                 disable;
    } PACK  OamCtcTlvLoopedPortDisable;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcTlvVoipPort - ONU VoIP port configuration
///
/// Object: Port
/// Leaf:   OamCtcAttrVoipPort (0x0014)
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    OamCtcPortState     lock;
    } PACK  OamCtcTlvVoipPort;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcTlvE1Port - ONU E1 port configuration
///
/// Object: Port
/// Leaf:   OamCtcAttrE1Port (0x0015)
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    OamCtcPortState     lock;
    } PACK  OamCtcTlvE1Port;



typedef struct  {
    U8 num_permitted_vlans;
    EthernetVlanData permitted_vlans[MAX_PERMITTED_VLANS];
} CtcVlanTrunkEntries;

typedef struct 
	{
	EthernetVlanData	fromVid;
	EthernetVlanData	toVid;
	}PACK CtcVlanTranslatate; 
		
typedef struct
	{
//	U8 CtcOamVlanMode;
	U8 CtcOamVlanData[0];
	}PACK CtcOamVlanEntry;

typedef struct
{
    EthernetVlanData tag[0];
}PACK CtcOamVlanTag;

typedef struct 
{
    EthernetVlanData   defaultVlan;
    CtcVlanTranslatate vlanTranslateArry[0];
}PACK CtcOamVlanTranslate;

typedef struct 
{
    U16                 numAggrEntries;
    EthernetVlanData    aggregatedVlan;
    EthernetVlanData    vlanAggrArry[0];
}PACK CtcOamVlanN21Table;

typedef struct 
{
    EthernetVlanData    defaultVlan;
    U16                 numVlanAggrTables;
    CtcOamVlanN21Table  vlanAggrTable[0];
}PACK CtcOamVlanN21Translate;

typedef struct 
{
    EthernetVlanData    defaultVlan;
    EthernetVlanData    vlanTrunkArry[0];
}PACK CtcOamVlanTrunk;

/*
/// OamCtcTlvVlan - ONU VLAN configuration
/// 
/// Object:	VLAN
/// Leaf:	OamCtcAttrVlan (0x0021)
*/
typedef struct
{
    U8 mode;
    union
    {
        CtcOamVlanTag           tagParams;
        CtcOamVlanTranslate     xlateParams;
        CtcOamVlanN21Translate  aggrN21Params;
        CtcOamVlanTrunk         trunkParams;
    }CtcVlanParams;
} PACK OamCtcTlvVlan;


typedef enum
    {
    OamCtcClassRuleDel          = 0x00,
    OamCtcClassRuleAdd          = 0x01,
    OamCtcClassRuleClear        = 0x02,
    OamCtcClassRuleList         = 0x03
    } PACK OamCtcClassRuleOp;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcTlvClassMarking - ONU traffic classification configuration
///
/// Object: QoS
/// Leaf:   OamCtcAttrClassMarking (0x0031)
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    OamCtcRuleAction    action;
    U8                  numRules;
    OamCtcRule          rule[1];
    } PACK  OamCtcTlvClassMarking;

typedef struct
    {
    OamCtcRuleAction    action;
    U8                  numPrec;
    U8                  precToDelete[1];
    } PACK  OamCtcTlvClassMarkingDelete;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcTlvMcastVlan - Add/delete ONU multicast VLAN
///
/// Object: Multicast
/// Leaf:   OamCtcAttrMcastVlan (0x0041)
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    OamCtcMcastVlanOp   operation;
    VlanTag             vid[1];
    } PACK  OamCtcTlvMcastVlan;

typedef struct 
{
    VlanTag 	mcastVlan;
    VlanTag 	userVlan;
}PACK CtcMcastVlanSwitchingEntry; 

////////////////////////////////////////////////////////////////////////////////
/// OamCtcTlvMcastVlanStrip - Strip tags from multicast frames
///
/// Object: Multicast
/// Leaf:   OamCtcAttrMcastVlanStrip (0x0042)
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    OamCtcMcastTagMode  tagStripped;
    } PACK  OamCtcTlvMcastVlanStrip;
////////////////////////////////////////////////////////////////////////////////
/// CtcMcastVlanSwitch - Mucast VLAN switch
///
/// Object: Multicast
/// Leaf:   OamCtcAttrMcastSwitch (0x0042)
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    VlanTag             Mcasttag;
    VlanTag             IptvTag;
    } PACK  CtcMcastVlanSwitch;
////////////////////////////////////////////////////////////////////////////////
/// OamCtcTlvMcastSwitch - IGMP snooping or host controled IGMP
///
/// Object: Multicast
/// Leaf:   OamCtcAttrMcastSwitch (0x0043)
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    OamCtcMcastMode     mode;
    } PACK  OamCtcTlvMcastSwitch;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcTlvMcastCtrl - ONU multicast control
///
/// Object: Multicast
/// Leaf:   OamCtcAttrMcastCtrl (0x0044)
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    OamCtcMcastGroupOp  action;
    McastCtrl           ctrlType;
    U8                  numEntries;
    } PACK  OamCtcTlvMcastCtrlHdr;        // this one is just for sizeof calls

typedef struct
{
    U8                  action;     //OamCtcMcastGroupOp -> U8
    U8                  ctrlType;   //McastCtrl -> U8
    U8                  numEntries;
    U8                  entry[1];
} PACK  OamCtcTlvMcastCtrl;

////////////////////////////////////////////////////////////////////////////////
/// OamCtcTlvGroupMax - Multicast group maximums
///
/// Object: Multicast
/// Leaf:   OamCtcTlvGroupMax (0x0043)
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    U8                  maxGroup;
    } PACK  OamCtcTlvGroupMax;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcTlvPowerMonDiag - Power monitor diagnosis
///
/// Object: ONU/Port/Card
/// Leaf:   OamCtcTlvPowerMonDiagnosis (0x0005)
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    U16                 temp;
    U16                 vcc;
    U16                 txBias;
    U16                 txPower;
    U16                 rxPower;
    } PACK  OamCtcTlvPowerMonDiag;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcTlvAlarmAdminState - Alarm Admin State
///
/// Object: ONU/Port/Card
/// Leaf:   OamCtcTlvPowerMonDiagnosis (0x0081)
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    U16                 alarmId;
    U32                 config;
    } PACK  OamCtcTlvAlarmAdminState;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcTlvPowerMonThd - Power monitor threshold
///
/// Object: ONU/Port/Card
/// Leaf:   OamCtcTlvPowerMonThreshold (0x0082)
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    U16                 alarmId;
    U32                 thdRaise;
    U32                 thdClear;
    } PACK  OamCtcTlvPowerMonThd;

////////////////////////////////////////////////////////////////////////////////
/// OamCtcOnuTxPowerCtrlOpId - ONU Tx Power Supply Control Optical ID
///
/// This type is used to present Onu tx power supply control optical id
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    OamCtcMainOptical            = 0x00000000UL,
    OamCtcStandbyOptical         = 0x00000001UL,
    OamCtcBothOptical            = 0x00000002UL
#if defined(_ARC)
    ,OamCtcOpticalForceWidth     = 0x7FFFFFFFUL
#endif
    } PACK  OamCtcOnuTxPowerCtrlOpId;

////////////////////////////////////////////////////////////////////////////////
/// OamCtcTlvTxPowerCtrl - Tx Power Supply Control
///
/// Object: ONU
/// Leaf:   OamCtcAttrOnuTxPowerSupplyControl (0x00A1)
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    U16                             action;
    MacAddr                         onuId;
    OamCtcOnuTxPowerCtrlOpId        optId;
    } PACK  OamCtcTlvTxPowerCtrl;

////////////////////////////////////////////////////////////////////////////////
/// OamCtcTlvTxPowerCtrl - Tx Power Supply Control
///
/// Object: ONU
/// Leaf:   OamCtcAttrOnuTxPowerSupplyControl (0x00A1)
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    U32                             action;
    MacAddr                         onuId;
    OamCtcOnuTxPowerCtrlOpId        optId;
    } PACK  OamCtcTlvTxPowerCtrlPre300;

////////////////////////////////////////////////////////////////////////////////
/// OamCtcAttrPrfrmMontrStatus - Performance monitor status
///
/// Object: PON IF/Port
/// Leaf:   OamCtcAttrPrfrmMontrStatus (0x00B1)
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    U16                             enable;
    U32                             period;
    } PACK  OamCtcTlvPfmStatus;


//##############################################################################
// CTC OAM TLV for KT extension
//##############################################################################

typedef struct
    {
    U8 portDsShapingOperation;
    U8 portDsShapingRate[3];
    } PACK  OamCtcKtExtTlvDsShaping;

typedef struct
    {
    U8 macLimitOperation;
    U8 numOfEntry;
    } PACK  OamCtcKtExtTlvOnuMacLimit;

typedef struct
    {
    U8 stpOperation;
    } PACK  OamCtcKtExtTlvEtherPortRstp;

typedef struct
    {
    U8 value;
    } PACK  OamCtcKtExtTlvBlockOnu;

#if 0
typedef struct
    {
    U8 varWidth;
    U16 opticModuleTemp;
    U16 txOpticPower;
    U16 rxOpticPower;
    } PACK  OamCtcKtExtTlvOnuDiagnostics;

typedef struct
    {
    U8 varWindth;
    U16 numOfUpstreamQueues;
    } PACK  OamCtcKtExtTlvQDropCounter;

typedef struct
    {
    U8 varWidth;
    } PACK  OamCtcKtExtTlvEtherPortCounter;
#endif

typedef struct
    {
    U8 operation;
    U8 interval;
    U8 blockTime;
    } PACK  OamCtcKtExtTlvLoopDetect;

typedef struct
    {
    U8 value;
    U8 duration;
    } PACK  OamCtcKtExtTlvOpticTxPowerOff;



////////////////////////////////////////////////////////////////////////////////
/// OamCtcTlvOnuProtParam - ONU Protection Parameters
///
/// Object: ONU
/// Leaf:   OamCtcAttrOnuProtParam (0x000F)
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    U16                 tLosOptical;
    U16                 tLosMpcp;
    } PACK  OamCtcTlvOnuProtParam;

//##############################################################################
//##############################################################################


////////////////////////////////////////////////////////////////////////////////
/// OamCtcExt - CTC extension OAMPDU header
///
/// This record comes at the begining of an China Telecom extended OAMPDU, just
/// after the CTC OUI.  It contains the OAM opcode.
////////////////////////////////////////////////////////////////////////////////
//extern
typedef struct
    {
    OamCtcOpcode    opcode;
    } PACK  OamCtcExt;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcVersion - CTC version
///
////////////////////////////////////////////////////////////////////////////////

typedef enum
    {
    OamCtcVer01         = 0x01,
    OamCtcVer13         = 0x13,
    OamCtcVer20         = 0x20,
    OamCtcVer21         = 0x21,
    OamCtcVer30         = 0x30
    } PACK OamCtcVersion;


////////////////////////////////////////////////////////////////////////////////
///  OamCtcOamVersion - OAM version type
///
/// The following type represents the OAM version number used in the CTC
/// extended OAM discovery sequence.  It defines the version of the CTC OAM as
/// well as the version number of any other extended OAM as well.
////////////////////////////////////////////////////////////////////////////////
typedef U8 OamCtcOamVersion;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcVersionInfoState - Version negotiation state
///
/// This type defines the states that an OLT or ONU may be in when performing
/// CTC OAM version negotiation.
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    OamCtcDiscWait,
    OamCtcDiscNegotiating,
    OamCtcDiscNegotiated,
    OamCtcDiscSuccess,
    OamCtcDiscFailed
    } PACK  OamCtcDiscoveryState;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcInfoTlvExtData - Extended OAM version support
///
/// This structure contains a single entry for extended OUI information durring
/// China Telecom OAM discovery.  This structure will be found at the end of an
/// CTC Organazation Specific Information TVL.  Inclusion of extended OUI
/// information is optional.
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    IeeeOui                 oui;        //< Vendor OUI
    OamCtcOamVersion        version;    //< Vendor OAM version number
    } PACK  OamCtcInfoTlvExtData;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcEncryptKey - CTC encryption key
///
/// The following record defines the China Telecom triple churning encryption
/// key.  The byte layout is as follows:
///
/// - Byte 0: X1..X8
/// - Byte 1: P1..P8
/// - Byte 2: P9..P16
////////////////////////////////////////////////////////////////////////////////
typedef union
    {
    struct
        {
        U8 xPart;
        U16 pPart;
        } PACK parts;
    U8  byte[3];
    } PACK  OamCtcEncryptKey1G;

#define CtcNumKeyStages     3

typedef struct
    {
    OamCtcEncryptKey1G  key[CtcNumKeyStages];
    } PACK  OamCtcEncryptKey10G;

typedef union
    {
    OamCtcEncryptKey1G  one;
    OamCtcEncryptKey10G ten;
    } PACK  OamCtcEncryptKey;

////////////////////////////////////////////////////////////////////////////////
/// OamChurningOpcode - CTC churning PDU opcode
///
/// The following defines the opcodes that are found in China Telecom Churning
/// PDUs.  Possilble operations are new_key_request (ChurningKeyRequest) which
/// the OLT sends the ONU to force the ONU to generate a new encryption key and
/// new_churning_key (ChurningNewKey) which the ONU send to the OLT to inform
/// the OLT of the new key.
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    ChurningKeyRequest,
    ChurningNewKey
    } PACK  OamCtcChurningOpcode;


////////////////////////////////////////////////////////////////////////////////
/// OamChurningPdu - CTC churning PDU
///
/// The following message is a China Telecom Churning OAMPDU (0x09).  This PDU
/// functions for both opcodes although the key is not used in a key request
/// message.
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    OamCtcOpcode            ext;
    OamCtcChurningOpcode    opcode;
    U8                      keyIndex;
    OamCtcEncryptKey1G      key;
    } PACK  OamCtcChurningPdu;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcChurning10GPdu - CTC 10 gig churning PDU
///
/// The following message is a China Telecom Churning OAMPDU (0x09).  This PDU
/// functions for both opcodes although the key is not used in a key request
/// message.
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    OamCtcOpcode            ext;
    OamCtcChurningOpcode    opcode;
    U8                      keyIndex;
    OamCtcEncryptKey10G     key;
    } PACK  OamCtcChurning10GPdu;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcDbaOpcode - CTC DBA PDU opcode
///
/// The following defines all of the various operation that may be found in a
/// China Telecom extended Dynamic Bandwidth Allocation PDU.
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    DbaGetRequest,
    DbaGetResponse,
    DbaSetRequest,
    DbaSetResponse
    } PACK  OamCtcDbaOpcode;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcEventOpcode - CTC Event PDU opcode
///
/// Defines the various sub type of event operation that defined in CTC3.0
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    EventStatusRequest = 1,
    EventStatusSet,
    EventStatusResponse,
    EventThresholdRequest,
    EventThresholdSet,
    EventThresholdResponse
    } PACK  OamCtcEventOpcode;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcQueueSetCount - Number of queue sets
///
/// The 3714 hardware supports two queue sets as defined by EponBurstCap and
/// EponBurstCap2.  Setting or requesting any queue sets beyond two will lead
/// to very undesirable behavior.
////////////////////////////////////////////////////////////////////////////////
extern
U8 oamCtcQueueSetCount;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcDbaQueueSet - CTC DBA queue set information
///
/// The following record contains the infomation to set the queue thresholds
/// for a set of queues.  A threshold only applies to those queues who have
/// their bit set in the report bit map.
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    U8                  report;
    U16                 threshold[8];
    } PACK  OamCtcDbaQueueSet;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcDbaQueueHdr - CTC DBA queue set information
///
/// The following record contains the infomation to set the queue thresholds
/// for a set of queues.  A threshold only applies to those queues who have
/// their bit set in the report bit map.
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    U8                  report;
    U16                 threshold[1];
    } PACK  OamCtcDbaQueueHdr;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcDbaPdu - CTC DBA PDU
///
/// The following defines the structure of a China Telecom Dynamic Bandwidth
/// Allocation PDU (0x0A).  The opcode determise whether the message is a
/// get/set request or a get/set response.  The indexes field is the number of
/// queue sets in the message.
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    OamCtcOpcode        ext;
    OamCtcDbaOpcode     opcode;
    U8                  indexes;
    OamCtcDbaQueueHdr   set[1];
    } PACK  OamCtcDbaPdu;


////////////////////////////////////////////////////////////////////////////////
/// OamCtcDbaPduSetResponse - Response structure to a CTC DBA set message
///
/// Here is the platypus in the system.  The set message response is the same
/// as the rest of the message with exception of a random ACK field in the
/// middle.  The ACK is set to TRUE on the response if everything went OK.
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    OamCtcOpcode        ext;
    OamCtcDbaOpcode     opcode;
    BOOL                ack;
    U8                  indexes;
    } PACK  OamCtcDbaPduSetResponse;

typedef enum
    {
    OamCtcAuthRequest       = 0x01,
    OamCtcAuthResponse,
    OamCtcAuthAckSuccess,
    OamCtcAuthAckFailure
    } PACK  OamCtcAuthOpcode;

typedef struct
    {
    OamCtcOpcode        ext;
    OamCtcAuthOpcode    opcode;
    U16                 length;
    } PACK  OamCtcAuthHead;

typedef enum
    {
    AuthTypeLoid      = 0x01,
    AuthTypeOther
    } PACK  OamCtcAuthType;

typedef struct
    {
    OamCtcAuthType authType;
    } PACK OamCtcAuthRequestPdu;

typedef struct
    {
    OamCtcAuthType authType;
    U8  loid[MaxLoidLen];
    U8  password[MaxAuthPassLen];
    } PACK OamCtcAuthResponsePdu;

typedef struct
    {
    OamCtcAuthType reqAuthType;
    OamCtcAuthType desiredAuthType;
    } PACK OamCtcAuthResNackPdu;

typedef enum
    {
    OamCtcAuthErrNoLoid      =  0x01,
    OamCtcAuthErrWrongPass
    } PACK OamCtcAuthErrCode;





////////////////////////////////////////////////////////////////////////////////
/// OamCtcInfoTlvHeader - Header for CTC information TVL
///
/// This structure is the header that will be found on China Telecom
/// Organazation Specific Information TVL messages.  This message should be
/// exchanged by the OLT and ONU during OAM discovery.
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    OamTlvType              type;       //< OAM opcode (0xFE)
    U8                      length;     //< TLV size including this header
    IeeeOui                 oui;        //< CTC OUI
    BOOL                    support;    //< Is CTC OAM supported
    OamCtcOamVersion        version;    //< CTC OAM version
    OamCtcInfoTlvExtData    ext[1];     //< Other vendor extended data
    } PACK  OamCtcInfoTlvHeader;

////////////////////////////////////////////////////////////////////////////////
/// CtcInfoTvlHdrSize - CTC Information TVL header size
///
/// This constant is the size of the China Telecom Organazation Specific
/// Information TVL without any extended vendor OAM data.
////////////////////////////////////////////////////////////////////////////////
#define CtcInfoTvlHdrSize   \
        (sizeof(OamCtcInfoTlvHeader) - sizeof(OamCtcInfoTlvExtData))


////////////////////////////////////////////////////////////////////////////////
/// DefaultCtcOui - China Telecom Organizationally Unique Identifier
///
/// This defintion defines the China Telecom Organizationally Unique Identifier.
/// This OUI is found in all vendor specific OAM.
////////////////////////////////////////////////////////////////////////////////
#define DefaultCtcOui           { 0x11, 0x11, 0x11 } PACK 


////////////////////////////////////////////////////////////////////////////////
/// DefaultCtcOamVersion - China Telecom OAM version number
///
/// This defintion defines the China Telecom vendor extended OAM version number.
/// This version is used during vendor OAM discovery to determine OLT and ONU
/// OAM compatibility.
////////////////////////////////////////////////////////////////////////////////
#define DefaultCtcOamVersion        0x30

#define JedecTeknovusId         0x0203
#define JedecPmcId              0x4536


typedef enum
    {
    OamCtcServiceDbaOpDeactivate    = 0x00,
    OamCtcServiceDbaOpActivate      = 0x01,
    } PACK  OamCtcServiceDbaOp;


typedef enum
    {
    OamCtcScheduleSp    = 0,
    OamCtcScheduleWrr   = 1,
    OamCtcScheduleSpWrr = 2,
    } PACK  OamCtcScheduling;


typedef struct
    {
    U8  qNum;
    U16 pktSize;
    U16 fixedBw;
    U16 guarBw;
    U16 bestEffBw;
    U8  weight;
    } PACK  OamCtcService;


typedef struct
    {
    OamCtcServiceDbaOp  op;
    OamCtcScheduling    sched;
    U8                  highPriBound;
    U32                 cycleLen;
    U8                  numServices;
    OamCtcService       service[1];
    } PACK  OamCtcServiceSla;



////////////////////////////////////////////////////////////////////////////////
/// CnuInfo - for DOCSIS EOC.
/// This type stores basic CNU(Cable Modem) information
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    MacAddr cnuMac;
    MacAddr cmcMac;
    U16     cnuId;       // VLAN ID
    U32     primaryDn;
    U32     primaryUp;
    } CnuInfo;


#if defined(__cplusplus)
}
#endif

////////////////////////////////////////////////////////////////////////////////
BOOL DetermineCTC21OnuByLocalInfo(void *context);

////////////////////////////////////////////////////////////////////////////////
void ShowCtcOnuInfo(void);

////////////////////////////////////////////////////////////////////////////////
HostCtcOamInfo *CtcPerOnuOamInfoFind(const MacAddr *linkMac);

#endif // End of File CtcOam.h

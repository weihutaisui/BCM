/*
* <:copyright-BRCM:2012:proprietary:epon
* 
*    Copyright (c) 2012 Broadcom 
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
#if !defined(OnuOs_h)
#define OnuOs_h
////////////////////////////////////////////////////////////////////////////////
/// \file OnuOs.h
/// \brief ONU
/// \author Jason Armstrong
/// \date October 28, 2010
///
////////////////////////////////////////////////////////////////////////////////

#include "Teknovus.h"

#define OnuAssertIdInVld    0xFFFF


////////////////////////////////////////////////////////////////////////////////
/// \brief Interrupt callback function
////////////////////////////////////////////////////////////////////////////////
typedef void (*OnuOsIntHandler)(void);


//##############################################################################
// Assertion tracking
//##############################################################################
// For the stat: 0: Ont Stat, 1-15: Port Stat, 16-63: Link Stat
#define OsAstStatEponInst(inst)     (0)
#define OsAstStatPortInst(inst)     (((inst)%15) + 1)
#define OsAstStatLinkInst(inst)     (((inst)%48) + 16)
#define OsAstStatEponInstBase               (0)
#define OsAstStatEponInstEnd                (0)
#define OsAstStatPortInstBase               (1)
#define OsAstStatPortInstEnd                (15)
#define OsAstStatLinkInstBase               (16)
#define OsAstStatLinkInstEnd                (63)


typedef enum
    {
    OsAstAppBase                    = 0x01, //change to 0x00 might break other alarms especially power loss
    ////////////////////////////////////////////////////////////////////////////
    /// Os Alarm Assert Id
    ////////////////////////////////////////////////////////////////////////////
    // per-ONT alarms
    OsAstAlmBase                    = OsAstAppBase,
    OsAstAlmOntBase                 = OsAstAlmBase,
    OsAstAlmOntPowerLoss            = OsAstAlmOntBase,
    OsAstAlmOntBootInvalid,
    OsAstAlmOntSelfTestErr,
    OsAstAlmOntFlashBusy,
    OsAstAlmOntSysBusy,

    OsAstAlmOntFirmwareDownload,

    OsAstAlmOntAuthInfoUnvld,
    OsAstAlmOntAuthenticated,

    OsAstAlmOntGpioLinkFault,
    OsAstAlmOntGpioCritical,
    OsAstAlmOntGpioDyingGasp,
    OsAstAlmOntGpioOther,

    OsAstAlmOnt1GDnRegistered,
    OsAstAlmOnt1GDnActivity,
    OsAstAlmOnt10GDnRegistered,
    OsAstAlmOnt10GDnActivity,
    OsAstAlmOnt1GUpRegistered,
    OsAstAlmOnt1GUpActivity,
    OsAstAlmOnt10GUpRegistered,
    OsAstAlmOnt10GUpActivity,
    OsAstAlmOntSingleLinkReg,
    OsAstAlmOntMultiLinkReg,

    OsAstAlmOntFecUp,
    OsAstAlmOntFecDn,

    OsAstAlmOntSff8472Thd,

    OsAstAlmOntBatteryLost,
    OsAstAlmOntBatteryFail,
    OsAstAlmOntBatteryVoltLow,

    OsAstAlmOntPonPriSig,
    OsAstAlmOntPonSecSig,
    OsAstAlmOntLearnEvent,
    OsAstAlmOntWdt,
    OsAstAlmOntReset,
    OsAstAlmOntRestore,
    OsAstAlmOntProtSwitch,
    OsAstAlmOntRogueOnu,
    OsAstAlmOntEnd,
    OsAstAlmOntNums                 =  OsAstAlmOntEnd - OsAstAlmOntBase,

    // EPON port alarms
    OsAstAlmEponBase                = 0x40,
    OsAstAlmEponLos                 = OsAstAlmEponBase,
    OsAstAlmEponLostSync,
    OsAstAlmEponStatThreshold,
    OsAstAlmEponActLed,
    OsAstAlmEponStandbyLos,
    OsAstAlmEponLaserShutdownTemp,
    OsAstAlmEponLaserShutdownPerm,
    OsAstAlmEponLaserOn,
    OsAstPwrMonTempHigh,
    OsAstPwrMonTempLow,
    OsAstPwrMonVccHigh,
    OsAstPwrMonVccLow,
    OsAstPwrMonTxBiasHigh,
    OsAstPwrMonTxBiasLow,
    OsAstPwrMonTxPowerHigh,
    OsAstPwrMonTxPowerLow,
    OsAstPwrMonRxPowerHigh,
    OsAstPwrMonRxPowerLow,
    OsAstAlmEponEnd,
    OsAstAlmEponNums                = OsAstAlmEponEnd - OsAstAlmEponBase,

    // Ethernet port alarms
    OsAstAlmPortBase                = 0x80,
    OsAstAlmPortLearnTblOverflow    = OsAstAlmPortBase,
    OsAstAlmPortLoopback,
    OsAstAlmPortStatThreshold,
    OsAstAlmPortLinkDown,
    OsAstAlmPortBlocked,
    OsAstAlmPortTraffic,
    OsAstAlmPortLinkUp,
    OsAstAlmPortDisabled,
    OsAstAlmPortAutoNegFailure,
    OsAstAlmPortEnd,
    OsAstAlmPortNums                = OsAstAlmPortEnd - OsAstAlmPortBase,

    // per-LLID alarms
    OsAstAlmLinkBase                = 0xC0,
    OsAstAlmLinkNoGates             = OsAstAlmLinkBase,
    OsAstAlmLinkUnregistered,
    OsAstAlmLinkOamTimeout,
    OsAstAlmLinkOamDiscComplete,
    OsAstAlmLinkStatThreshold,
    OsAstAlmLinkKeyExchange,
    OsAstAlmLinkLoopback,
    OsAstAlmLinkAllDown,
    OsAstAlmLinkDisabled,
    OsAstAlmLinkRegStart,
    OsAstAlmLinkRegSucc,
    OsAstAlmLinkRegFail,
    OsAstAlmLinkRegTimeOut,
    OsAstAlmLinkCtcOamDiscComplete,
    OsAstAlmLinkEnd,
    OsAstAlmLinkNums                = OsAstAlmLinkEnd - OsAstAlmLinkBase,

    OsAstAlmQueueBase               = 0x100,
    OsAstAlmQueueStatThd            = OsAstAlmQueueBase,
    OsAstAlmQueueEnd,
    OsAstAlmQueueNums               = OsAstAlmQueueEnd - OsAstAlmQueueBase,

    OsAstAlmOtherBase               = 0x140,
    OsAstAlmGpioInput               = OsAstAlmOtherBase,
    OsAstAlmOtherEnd,
    OsAstAlmOtherNums               = OsAstAlmOtherEnd - OsAstAlmOtherBase,

    OsAstAlmEnd                     = OsAstAlmOtherEnd,
    OsAstAlmNums                    = OsAstAlmOntNums + OsAstAlmEponNums +
                                      OsAstAlmPortNums + OsAstAlmLinkNums +
                                      OsAstAlmQueueNums + OsAstAlmOtherNums,

    ////////////////////////////////////////////////////////////////////////////
    /// Os Stat Assert Id
    ////////////////////////////////////////////////////////////////////////////
    OsAstStatBase                   = 0x200,

    // Rx stats
    OsAstStatRxBase                 = OsAstStatBase,
    OsAstStatBytesRx                = OsAstStatRxBase,

    OsAstStatTotalFramesRx,
    OsAstStatUnicastFramesRx,
    OsAstStatMcastFramesRx,
    OsAstStatBcastFramesRx,

    OsAstStatFcsErr,
    OsAstStatCrc8Err,
    OsAstStatLineCodeErr,

    OsAstStatFrameTooShort,
    OsAstStatFrameTooLong,
    OsAstStatInRangeLenErr,
    OsAstStatOutRangeLenErr,
    OsAstStatAlignErr,

    // bin sizes available on Ethernet ports only
    OsAstStatRx64Bytes,
    OsAstStatRx65_127Bytes,
    OsAstStatRx128_255Bytes,
    OsAstStatRx256_511Bytes,
    OsAstStatRx512_1023Bytes,
    OsAstStatRx1024_1518Bytes,
    OsAstStatRx1519PlusBytes,

    OsAstStatRxFramesDropped,                 // dropped in queue, that is
    OsAstStatRxBytesDropped,                 // dropped in queue, that is
    OsAstStatRxBytesDelayed,
    OsAstStatRxMaxDelay,
    OsAstStatRxDelayThresh,

    OsAstStatRxPauseFrames,
    OsAstStatRxControlFrames,
    
    OsAstStatErrFrames,
    OsAstStatErrFramePeriods,
    OsAstStatErrFrameSummary,
    OsAstStatRxEnd,
    OsAstStatRxNums                 = OsAstStatRxEnd - OsAstStatRxBase,

    // Tx stats
    OsAstStatTxBase                 = 0x280,
    OsAstStatBytesTx                = OsAstStatTxBase,
    OsAstStatTotalFramesTx,
    OsAstStatUnicastFramesTx,
    OsAstStatMcastFramesTx,
    OsAstStatBcastFramesTx,

    OsAstStatSingleColl,
    OsAstStatMultiColl,
    OsAstStatLateColl,
    OsAstStatFrAbortXsColl,

    // bin sizes available on Ethernet ports only
    OsAstStatTx64Bytes,
    OsAstStatTx65_127Bytes,
    OsAstStatTx128_255Bytes,
    OsAstStatTx256_511Bytes,
    OsAstStatTx512_1023Bytes,
    OsAstStatTx1024_1518Bytes,
    OsAstStatTx1519PlusBytes,

    OsAstStatTxFramesDropped,                 // dropped in queue, that is
    OsAstStatTxBytesDropped,                 // dropped in queue, that is
    OsAstStatTxBytesDelayed,
    OsAstStatTxMaxDelay,
    OsAstStatTxDelayThresh,
    OsAstStatTxUpUnusedBytes,

    OsAstStatTxPauseFrames,
    OsAstStatTxControlFrames,
    OsAstStatTxDeferredFrames,
    OsAstStatTxExcessiveDeferralFrames,
    
    OsAstStatMpcpMACCtrlFramesTx,
    OsAstStatMpcpMACCtrlFramesRx,
    OsAstStatMpcpTxRegAck,
    OsAstStatMpcpTxRegRequest,
    OsAstStatMpcpTxReport,
    OsAstStatMpcpRxGate,
    OsAstStatMpcpRxRegister,
    OsAstStatTxEnd,
    OsAstStatTxNums                 = OsAstStatTxEnd - OsAstStatTxBase,

    OsAstStatEnd                    = OsAstStatTxEnd,
    OsAstStatNums                   = OsAstStatRxNums + OsAstStatTxNums,

    ////////////////////////////////////////////////////////////////////////////
    /// Os Diagnosis Assert Id
    ////////////////////////////////////////////////////////////////////////////
    OsAstDiagBase                   = 0x300,
    OsAstDiagState                  = OsAstDiagBase,
    OsAstDiagEnd,
    OsAstDiagNums                   = OsAstDiagEnd - OsAstDiagBase,

    ////////////////////////////////////////////////////////////////////////////
    /// Os System Assert Id
    ////////////////////////////////////////////////////////////////////////////
    OsAstSysBase                    = 0x1000,
    OsAstPowerFail                  = OsAstSysBase,
    OsAstOntDir,
    OsAstFifCmd,
    OsAstUniMgr,
    OsAstMaster,
    OsAstSysEnd,
    OsAstSysNums                    = OsAstSysEnd - OsAstSysBase,

    ////////////////////////////////////////////////////////////////////////////
    /// Os System Assert Id Number
    ////////////////////////////////////////////////////////////////////////////
    OsAstNums                       = OsAstAlmNums + OsAstStatNums +
                                      OsAstDiagNums + OsAstSysNums,
    OsAstInvalid                    = 0xFFFF,
    OsAstForceU16                   = 0x7FFF
    } OnuAssertId;


typedef void (*OnuOsAssertHandler)(U8);




// ** TODO **
////////////////////////////////////////////////////////////////////////////////
/// \brief Map an assertion id
///
/// This function maps an assertion id to internal store id.
///
/// \param id Assertion to raise
///
/// \return
/// mapped id
////////////////////////////////////////////////////////////////////////////////
extern
U16 OnuOsAssertMap (OnuAssertId id);



////////////////////////////////////////////////////////////////////////////////
/// \brief Set an assertion
///
/// This function sets an assertion state to TRUE.
///
/// \param id Assertion to raise
/// \param inst Instance number
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OnuOsAssertSet (OnuAssertId id, U8 inst);



////////////////////////////////////////////////////////////////////////////////
/// \brief Clean an assertion
///
/// This function sets an assertion state to FALSE.
///
/// \param id Assertion to clear
/// \param inst Instance number
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OnuOsAssertClr (OnuAssertId id, U8 inst);



////////////////////////////////////////////////////////////////////////////////
/// \brief Get the current state of an assertion
///
/// This function gets the active state of an assertion.
///
/// \param id Assertion to get
/// \param inst Instance number
///
/// \return
/// TRUE if the assert is active, FALSE if not
////////////////////////////////////////////////////////////////////////////////
extern
BOOL OnuOsAssertGet (OnuAssertId id, U8 inst);


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the current assert value of some id
///
/// This function gets the active state of an assertion.
///
/// \param id Assertion to get
///
/// \return
/// the whole value of this assert
////////////////////////////////////////////////////////////////////////////////
extern
U64 OnuOsAssertDbAll (OnuAssertId id);


////////////////////////////////////////////////////////////////////////////////
/// \brief Set a callback handle for assertion
///
/// This function associates a handler function with an assertion.  When an
/// assertion is raised (transitions from clear to set) the handler function
/// will be called.  The instance map is provided to the handler.
///
/// \param id Assertion to get
/// \param handler Callback to register
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OnuOsAssertHandleSet (OnuAssertId id, OnuOsAssertHandler handler);



////////////////////////////////////////////////////////////////////////////////
/// \brief Set a callback handle for assertion clear
///
/// This function associates a handler function with an assertion clear.
/// When an assertion is cleared (transitions from set to clear) the handler
/// function will be called.  The instance map is provided to the handler.
///
/// \param id Assertion to get
/// \param handler Callback to register
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OnuOsAssertClearHandleSet (OnuAssertId id, OnuOsAssertHandler handler);




////////////////////////////////////////////////////////////////////////////////
/// \brief Start the operating system
///
/// This functions starts the operating system.  It must be called before using
/// any operating system features.
///
/// \param caching Is data caching enabled
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OnuOsInit (void);



#endif // End of file OnuOs.h

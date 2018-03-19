/*
 *<:copyright-BRCM:2013:proprietary:epon 
 *
 *   Copyright (c) 2013 Broadcom 
 *   All Rights Reserved
 *
 * This program is the proprietary software of Broadcom and/or its
 * licensors, and may only be used, duplicated, modified or distributed pursuant
 * to the terms and conditions of a separate, written license agreement executed
 * between you and Broadcom (an "Authorized License").  Except as set forth in
 * an Authorized License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and Broadcom
 * expressly reserves all rights in and to the Software and all intellectual
 * property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 * NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 * BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1. This program, including its structure, sequence and organization,
 *    constitutes the valuable trade secrets of Broadcom, and you shall use
 *    all reasonable efforts to protect the confidentiality thereof, and to
 *    use this information only in connection with your use of Broadcom
 *    integrated circuit products.
 *
 * 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
 *    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
 *    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
 *    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
 *    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
 *    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
 *    PERFORMANCE OF THE SOFTWARE.
 *
 * 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
 *    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
 *    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
 *    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 *    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
 *    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
 *    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
 *    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
 *    LIMITED REMEDY.
:>
 */ 
#if !defined(UniManager_h)
#define UniManager_h
////////////////////////////////////////////////////////////////////////////////
/// \file UniManager.h
/// \brief Manages UNI specific Configuration and detection of UNI status
///
///
////////////////////////////////////////////////////////////////////////////////

#if defined(__cplusplus)
extern "C" {
#endif

#include "Ethernet.h"
#include "PortCapability.h"
#include "Teknovus.h"
#include "EponDevInfo.h"

#if defined(CLI_OUT) && CLI_OUT
#include "TkDebug.h"
#endif

#include <string.h>

#define DEFMAXFRAMESIZE         1536
#define MACMODE                 0

#define UniEmcId                4
#define UniXmcId                0


typedef enum
    {
    MacTypeUnused,
    MacTypeGmc,
    MacTypeXmc,
    MacTypeEmc,
    } PACK MacType;

typedef enum
    {
    PortIntXaui,
    PortIntXfi,
    PortIntSgmii,
    PortIntRgmii,
    PortIntS3mii,
    PortIntMii,
    PortInt1000BaseT,
    PortIntUnused,
    } PACK PortInterface;

typedef struct
    {
    U8 txErrPin;
    U8 colPin;
    U8 crsPin;
    U8 rxErrPin;
    } PACK Port5MpioConfig;


typedef struct
    {
    BOOL txDiffSwap;
    BOOL rxDiffSwap;
    U8 txLaneSelect;
    U8 rxLaneSelect;
    } PACK PortChannelConfig;

typedef struct
    {
    MacType mac;
    PortInterface pInt;
    Port port;
    EthPortCapability physCaps;
    U16 anLinkTimer;
    } PACK UniPortPhysCfg;

//This is a software configuration mode.  Enable the port on boot, disable
//the port, or do something "creative" with it.
typedef enum
    {
    Enabled,
    Disabled,
    } PACK PortOperMode;

typedef enum
    {
    Mdi,
    MdiX,
    } PACK MdiType;

typedef struct
    {
    PortOperMode PortCfgMode;
    BOOL AutoNegotiate;
    MdiType MdiMdiX;
    BOOL LenErr;
    EthPortCapability forceModeCaps;
    EthPortCapability portAdvCaps;
    U8 FlowControlOnThresh;
    U8 FlowControlOffThresh;
    U16 MaxFrameSize;
    } PACK UniPortOpMode;

typedef enum
    {
    PhyLpbk,
    MacLpbk,
    } PACK UniLpbkType;

typedef enum
    {
    UmDisabled,
    UmLinked,
    UmLoopback,
    UmRenegotiate,
    } PACK UniMgrPrtSt;

extern
UniMgrPrtSt IntPortSt[MAX_UNI_PORTS];


#define MiiAnAdvModeMask        MiiAuto100TxFull | MiiAuto100TxHalf | \
                                MiiAuto10Full | MiiAuto10Half | MiiAutoFlowCtrl

#define TkOnuUserPortCount()    MAX_UNI_PORTS


////////////////////////////////////////////////////////////////////////////////
/// \brief Validates downstream configuration from the UNI perspective
///
 // Parameters:
/// \param cfgPtr     configuration pointer
///
/// \return
/// TRUE if valid, FALSE if invalid
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL UniMgrValidateDownstreamConfig(U8 BULK * cfgPtr);





////////////////////////////////////////////////////////////////////////////////
/// \brief returns state of Port
///
 // Parameters:
/// \param port     port of which to return link status for
///
/// \return
/// link status of port
////////////////////////////////////////////////////////////////////////////////
extern
BOOL UniMgrFindPortStatus(TkOnuEthPort port);

////////////////////////////////////////////////////////////////////////////////
/// \brief sets state of Forced Capabilities
///
 // Parameters:
/// \param port     port of which to return Forced Capabilities for
/// \param caps     forced caps to set
///
/// \return
/// state of Forced Capabilities
////////////////////////////////////////////////////////////////////////////////
extern
void UniMgrApplyForcedCaps(TkOnuEthPort port, EthPortCapability caps);


////////////////////////////////////////////////////////////////////////////////
/// \brief sets state of length error discard
///
 // Parameters:
/// \param port     port of which to set len err discard for
/// \param mode     enable/disable indicator
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
extern
void UniMgrApplyLengthErrorDiscard(TkOnuEthPort port, BOOL mode);



////////////////////////////////////////////////////////////////////////////////
/// \brief Renegotiate port
///
/// Configures
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void UniMgrRestartPort(TkOnuEthPort port);


////////////////////////////////////////////////////////////////////////////////
/// \brief Get Stat for port
///
///
/// \return
/// stat value
////////////////////////////////////////////////////////////////////////////////
extern
U32 UniMgrGetPortStat(U8 port, U16 statId);


#if defined(__cplusplus)
}
#endif

#endif // UniManager.h

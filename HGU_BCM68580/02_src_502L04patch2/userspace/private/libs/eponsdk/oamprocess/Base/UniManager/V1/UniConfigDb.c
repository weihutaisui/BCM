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


////////////////////////////////////////////////////////////////////////////////
/// \file UniConfigDb.c
/// \brief Holds UNI specific configuration
///
///
////////////////////////////////////////////////////////////////////////////////

#include "Build.h"
#include <stdio.h>
#include "EthFrameUtil.h"
#include "Ethernet.h"
#include "UniManager.h"
#include "UniConfigDb.h"
#include "OntConfigDb.h"
#include "PortCapability.h"
#include "EponDevInfo.h"

typedef enum
    {
    UniCfgDbIdUniHwCfg,
    UniCfgDbIdBaseMac,
    UniCfgDbIdPortOptions,
    UniCfgDbIdMdioCustCmds,
    UniCfgDbIdMdioCustSeqs,
    UniCfgDbIdCount
    } UniCfgDbId;

//Data store
typedef struct
    {
    U8 physCapCount;
    UniPortPhysCfg PortPhysConfig[MAX_UNI_PORTS];
    PortChannelConfig chan[UNI_CHAN_COUNT];
    U32 options;
    } PACK PhysCapStore;
static PhysCapStore PhysStore;

typedef struct
    {
    U16 physCapCount;
    UniPortOpMode  PortOpConfig[MAX_UNI_PORTS];
    } PACK OpModeStore;
static OpModeStore OpStore;


MacAddr UniBaseMac;
U32 OnuSkuType;

////////////////////////////////////////////////////////////////////////////////
/// \brief Returns count of active ports based on personality
///
 // Parameters:
///
/// \return
///  count of ports
////////////////////////////////////////////////////////////////////////////////
//extern
U8 UniCfgDbGetActivePortCount(void)
    {
    return EponDevUniPortNum;   
    }

////////////////////////////////////////////////////////////////////////////////
/// \brief Returns count of external uni ports based on personality
/// \For DPoE, both hgu and sfu use EponDevDpoePortNum to support virtual interface or physical interface
/// \Doe CTC, HGU support 1 virtual port, SFU it is active ethernet port
/// \For other, keep original implementaion UniCfgDbGetActivePortCount
// Parameters:
///
/// \return
///  count of ports which can be seen outside
////////////////////////////////////////////////////////////////////////////////
//extern
U8 UniCfgDbGetOamManagedPortCount(void)
    {
    if (EponDevGetOamSel() == OAM_DPOE_SUPPORT)
        return EponDevDpoePortNum;
    if (EponDevGetOamSel() == OAM_CTC_SUPPORT)
        {
        #if defined(EPON_SFU)
        return UniCfgDbGetActivePortCount();
        #endif    
        #if defined(EPON_HGU)
        return 1;
        #endif
        }
    
    return UniCfgDbGetActivePortCount();
    }


////////////////////////////////////////////////////////////////////////////////
/// UniCfgDbGetUserBaseMacAddr:  Gets MAC address used for the User UNI
///
 // Parameters:
/// \param None
///
/// \return
/// Pointer to MAC address
////////////////////////////////////////////////////////////////////////////////
//extern
MacAddr const* UniCfgDbGetUserBaseMacAddr (void)
    {
    return &(UniBaseMac);
    } // UniCfgDbGetUserBaseMacAddr



////////////////////////////////////////////////////////////////////////////////
/// \brief returns Mac Address of Port
///
 // Parameters:
///     \param port             port of which to return Mac Address for
///     \param MacAddr*         MacAddr to set
///
/// \return
///  none
////////////////////////////////////////////////////////////////////////////////
//extern
void UniCfgDbGetPortMacAddress(TkOnuEthPort port, MacAddr *mac)
    {
    memcpy((void*)mac,(void*)&UniBaseMac, sizeof(MacAddr));
    AddToMacAddr(mac,port);
    } // UniCfgDbGetPortMacAddress


////////////////////////////////////////////////////////////////////////////////
/// \brief returns Mdio Phy Address of Port
///
 // Parameters:
///     \param port             port of which to return Mdio Phy Address for
///
/// \return
///  Port object for PHY
////////////////////////////////////////////////////////////////////////////////
//extern
Port UniCfgDbGetPhyPort(TkOnuEthPort port)
    {
    return PhysStore.PortPhysConfig[port].port;
    }  // UniCfgDbGetPhyPort


////////////////////////////////////////////////////////////////////////////////
/// \brief returns Physical capabilities of Port
///
 // Parameters:
///     \param port             port of which to return physical capabilities for
///
/// \return
/// physical capabilities of port
////////////////////////////////////////////////////////////////////////////////
//extern
EthPortCapability UniCfgDbGetPhysicalCapabilities(TkOnuEthPort port)
    {
    return PhysStore.PortPhysConfig[port].physCaps;
    }  // UniCfgDbGetPhysicalCapabilities



////////////////////////////////////////////////////////////////////////////////
/// \brief set Physical capabilities of Port
///
 // Parameters:
///     \param port             port of which to return physical capabilities for
///     \param portCaps      physical capabilities of port
///
/// \return
/// physical capabilities of port
////////////////////////////////////////////////////////////////////////////////
//extern
void UniCfgDbSetPhysicalCapabilities(TkOnuEthPort port, EthPortCapability portCaps)
    {
    PhysStore.PortPhysConfig[port].physCaps = portCaps;
    }  // UniCfgDbGetPhysicalCapabilities


////////////////////////////////////////////////////////////////////////////////
/// \brief returns MAC type of Port
///
 // Parameters:
///     \param port             port of which to return MAC type for
///
/// \return
/// Mac type of port
////////////////////////////////////////////////////////////////////////////////
//extern
MacType UniCfgDbGetMacType(TkOnuEthPort port)
    {
    return PhysStore.PortPhysConfig[port].mac;
    }  // UniCfgDbGetMacType

//Port operation NVS record setup:




////////////////////////////////////////////////////////////////////////////////
/// \brief returns AN mode of Port
///
 // Parameters:
///     \param port             port of which to return AN mode for
///
/// \return
/// mode of port
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL UniCfgDbGetAutoNegotiate(TkOnuEthPort port)
    {
    return OpStore.PortOpConfig[port].AutoNegotiate;
    }  // UniCfgDbGetAutoNegotiate




////////////////////////////////////////////////////////////////////////////////
/// \brief returns Len error mode of Port
///
 // Parameters:
///     \param port             port of which to return Len error mode for
///
/// \return
/// Len error mode of port
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL UniCfgDbGetLenError(TkOnuEthPort port)
    {
    return OpStore.PortOpConfig[port].LenErr;
    }  // UniCfgDbGetLenError

////////////////////////////////////////////////////////////////////////////////
/// \brief sets Length error of Port
///
 // Parameters:
///     \param port             port of which to set Length error  for
/// \param mode     Length error to set
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void UniCfgDbSetLenError(TkOnuEthPort port, BOOL mode)
    {
    OpStore.PortOpConfig[port].LenErr = mode;
    }  // UniCfgDbSetLenError

////////////////////////////////////////////////////////////////////////////////
/// \brief returns Forced capabilities of Port
///
 // Parameters:
///     \param port             port of which to return Forced capabilities for
///
/// \return
/// Forced capabilities of port
////////////////////////////////////////////////////////////////////////////////
//extern
EthPortCapability UniCfgDbGetForceModeCaps(TkOnuEthPort port)
    {
    return OpStore.PortOpConfig[port].forceModeCaps;
    }  // UniCfgDbGetForceModeCaps

////////////////////////////////////////////////////////////////////////////////
/// \brief sets Forced capabilities of Port
///
 // Parameters:
///     \param port             port of which to set Forced capabilities for
/// \param mode     Forced capabilities to set
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void UniCfgDbSetForceModeCaps(TkOnuEthPort port, EthPortCapability mode)
    {
    OpStore.PortOpConfig[port].forceModeCaps = mode;
    }  // UniCfgDbSetForceModeCaps

////////////////////////////////////////////////////////////////////////////////
/// \brief returns AN capabilities of Port
///
 // Parameters:
///     \param port             port of which to return AN capabilities for
///
/// \return
/// AN capabilities of port
////////////////////////////////////////////////////////////////////////////////
//extern
EthPortCapability UniCfgDbGetAnModeCaps(TkOnuEthPort port)
    {
    return OpStore.PortOpConfig[port].portAdvCaps;
    }  // UniCfgDbGetAnModeCaps

////////////////////////////////////////////////////////////////////////////////
/// \brief sets AN capabilities of Port
///
 // Parameters:
///     \param port             port of which to set AN capabilities for
/// \param mode     AN capabilities to set
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void UniCfgDbSetAnModeCaps(TkOnuEthPort port, EthPortCapability mode)
    {
    OpStore.PortOpConfig[port].portAdvCaps = mode;
    }  // UniCfgDbSetAnModeCaps




////////////////////////////////////////////////////////////////////////////////
/// \brief sets Flow control Off threshold of Port
///
 // Parameters:
///     \param port             port of which to set Flow control Off threshold for
/// \param mode     Flow control Off threshold to set
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void UniCfgDbSetFcOffThresh(TkOnuEthPort port, U8 thresh)
    {
    OpStore.PortOpConfig[port].FlowControlOffThresh = thresh;
    }  // UniCfgDbSetFcOffThresh

////////////////////////////////////////////////////////////////////////////////
/// \brief returns Max frame size of Port
///
 // Parameters:
///     \param port             port of which to return Max frame size for
///
/// \return
/// Max frame size of port
////////////////////////////////////////////////////////////////////////////////
//extern
U16 UniCfgDbGetMaxFrameSize(TkOnuEthPort port)
    {
    return OpStore.PortOpConfig[port].MaxFrameSize;
    }  // UniCfgDbGetMaxFrameSize

////////////////////////////////////////////////////////////////////////////////
/// \brief sets Max frame size Port
///
 // Parameters:
///     \param port             port of which to set Max frame size for
/// \param mode     Max frame size to set
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void UniCfgDbSetMaxFrameSize(TkOnuEthPort port, U16 maxSize)
    {
    OpStore.PortOpConfig[port].MaxFrameSize = maxSize;
    }  // UniCfgDbSetMaxFrameSize

////////////////////////////////////////////////////////////////////////////////
/// \brief clears capability from force mode caps
///
 // Parameters:
///     \param port             port of which to clr forced cap from
/// \param cap     cap to clear
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void UniCfgDbClrForceModeCap(TkOnuEthPort port,EthPortCapability cap)
    {
    OpStore.PortOpConfig[port].forceModeCaps &= (EthPortCapability)(~cap);
    }  // UniCfgDbClrForceModeCap

////////////////////////////////////////////////////////////////////////////////
/// \brief sets capability from force mode caps
///
 // Parameters:
///     \param port        port of which to set forced cap from
/// \param cap     cap to set
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void UniCfgDbSetForceModeCap(TkOnuEthPort port,EthPortCapability cap)
    {
    OpStore.PortOpConfig[port].forceModeCaps |= cap;
    }  // UniCfgDbSetForceModeCap

////////////////////////////////////////////////////////////////////////////////
/// \brief sets capability for both forced and AN caps
///
 // Parameters:
///     \param port        port of which to set cap for
/// \param cap     cap to set
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void UniCfgDbSetBothCaps(TkOnuEthPort port, EthPortCapability cap)
    {
    
    switch(cap)
        {
        case EthPortHalfDuplex:
        case EthPortFullDuplex:
            //remove any old duplex setting
            OpStore.PortOpConfig[port].forceModeCaps &=
                (EthPortCapability)(~EthPortCapDuplexMask);
            OpStore.PortOpConfig[port].portAdvCaps   &=
                (EthPortCapability)(~EthPortCapDuplexMask);
            //add the new one
            OpStore.PortOpConfig[port].forceModeCaps |= cap;
            OpStore.PortOpConfig[port].portAdvCaps   &= cap;
            break;
        case EthPort10Speed:
        case EthPort100Speed:
        case EthPort1GSpeed:
        case EthPort10GSpeed:
            //remove any old duplex setting
            OpStore.PortOpConfig[port].forceModeCaps &=
                (EthPortCapability)(~EthPortCapSpeedMask);
            OpStore.PortOpConfig[port].portAdvCaps   &=
                (EthPortCapability)(~EthPortCapSpeedMask);
            //add the new one
            OpStore.PortOpConfig[port].forceModeCaps |= cap;
            OpStore.PortOpConfig[port].portAdvCaps   |= cap;
            break;
        //These two are a single bit thus their is nothing to clear
        case EthPortFlowControl:
        case EthPortAutoMDIMDIX:
            OpStore.PortOpConfig[port].forceModeCaps &=
                (EthPortCapability)(~cap);
            OpStore.PortOpConfig[port].portAdvCaps   &=
                (EthPortCapability)(~cap);;
            OpStore.PortOpConfig[port].forceModeCaps |= cap;
            OpStore.PortOpConfig[port].portAdvCaps   |= cap;
            break;
        default:
            //should not happen
            break;
        }
    }  // UniCfgDbSetOption



////////////////////////////////////////////////////////////////////////////////
/// \brief sets up uni config DB in pers
///
 // Parameters:
/// none
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
//extern
void UniCfgDbInitUniPers(void)
    {
	
	
        {
        U8 idx;
        OpStore.physCapCount = PhysStore.physCapCount;
        for(idx = 0; idx < OpStore.physCapCount; ++idx)
            {
            OpStore.PortOpConfig[idx].PortCfgMode = Enabled;
            OpStore.PortOpConfig[idx].AutoNegotiate =
                (((PhysStore.PortPhysConfig[idx].physCaps&
                        EthPortAutoNegotiate)==EthPortAutoNegotiate));
            OpStore.PortOpConfig[idx].portAdvCaps =
                PhysStore.PortPhysConfig[idx].physCaps &
                (EthPortCapability)(~EthPortFlowControl);
            OpStore.PortOpConfig[idx].forceModeCaps =
                (PhysStore.PortPhysConfig[idx].physCaps
                & (~EthPortAutoNegotiate));
            OpStore.PortOpConfig[idx].MaxFrameSize = 0;
            }
        }

    }  // UniCfgDbInitUniPers

////////////////////////////////////////////////////////////////////////////////
/// \brief returns state of current capablities (note this accounts for
/// forced/an modes
///
 // Parameters:
/// \param port     port of which to return current capabilites for
///
/// \return
/// state of Auto-negotiation
////////////////////////////////////////////////////////////////////////////////
//extern
EthPortCapability UniCfgDbFindCurrentCaps(TkOnuEthPort port)
    {
    if(UniCfgDbGetAutoNegotiate(port))
        {
        return UniCfgDbGetAnModeCaps(port);
        }
    return UniCfgDbGetForceModeCaps(port);
    }  // UniMgrFindFindCurrentCaps



////////////////////////////////////////////////////////////////////////////////
/// \brief returns TX P/N differential swap option for channel
///
 // Parameters:
/// \param chan  Channel to swap
///
/// \return
/// TRUE if swapped FALSE if not swapped
///////////////////////////////////////////////////////////////////////////////
//extern
BOOL UniCfgDbGetTxDiffSwap (U8 chan)
    {
    return PhysStore.chan[chan].txDiffSwap;
    }  // UniCfgDbGetTxDiffSwap



////////////////////////////////////////////////////////////////////////////////
/// \brief returns RX P/N differential swap option for channel
///
 // Parameters:
/// \param chan  Channel to swap
///
/// \return
/// TRUE if swapped FALSE if not swapped
///////////////////////////////////////////////////////////////////////////////
//extern
BOOL UniCfgDbGetRxDiffSwap (U8 chan)
    {
    return PhysStore.chan[chan].rxDiffSwap;
    }  // UniCfgDbGetRxDiffSwap


// UniConfigDb.c





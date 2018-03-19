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
:>
*/

#if !defined(UniConfigDb_h)
#define UniConfigDb_h
////////////////////////////////////////////////////////////////////////////////
/// \file UniConfigDb.h
/// \brief Manages the UNI wide configuration
///
///
////////////////////////////////////////////////////////////////////////////////

#if defined(__cplusplus)
extern "C" {
#endif

#include "Build.h"

#include "Ethernet.h"
#include "Teknovus.h"
#include "UniManager.h"


typedef enum
    {
    //Starting options at the high bits so that
    //nobody is the wiser if we have to add customer options
    UniCfgOptsForceSinglePll = (1UL<<31),
    UniCfgOptsForceDoublePll = (1UL<<30),
    UniCfgOptsQuadXfiMode    = (1UL<<29),
    } UniCfgOpts;




////////////////////////////////////////////////////////////////////////////////
/// \brief Returns count of active ports based on personality
///
 // Parameters:
///
/// \return
///  count of ports
////////////////////////////////////////////////////////////////////////////////
extern
U8 UniCfgDbGetActivePortCount(void);

////////////////////////////////////////////////////////////////////////////////
/// \brief Returns count of external uni ports based on personality
///
// Parameters:
///
/// \return
///  count of ports which can be seen outside
////////////////////////////////////////////////////////////////////////////////
//extern
U8 UniCfgDbGetOamManagedPortCount(void);

////////////////////////////////////////////////////////////////////////////////
/// UniCfgDbGetUserBaseMacAddr:  Gets MAC address used for the User UNI
///
 // Parameters:
/// \param None
///
/// \return
/// Pointer to MAC address
////////////////////////////////////////////////////////////////////////////////
extern
MacAddr const* UniCfgDbGetUserBaseMacAddr (void);



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
extern
void UniCfgDbGetPortMacAddress(TkOnuEthPort port, MacAddr *mac);

////////////////////////////////////////////////////////////////////////////////
/// \brief returns Mdio Phy Address of Port
///
 // Parameters:
///     \param port             port of which to return Mdio Phy Address for
///
/// \return
///  Port object for PHY
////////////////////////////////////////////////////////////////////////////////
extern
Port UniCfgDbGetPhyPort(TkOnuEthPort port);



////////////////////////////////////////////////////////////////////////////////
/// \brief returns Physical capabilities of Port
///
 // Parameters:
/// \param port     port of which to return physical capabilities for
///
/// \return
/// physical capabilities of port
////////////////////////////////////////////////////////////////////////////////
extern
EthPortCapability UniCfgDbGetPhysicalCapabilities(TkOnuEthPort port);


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
extern
void UniCfgDbSetPhysicalCapabilities(TkOnuEthPort port, EthPortCapability portCaps);


////////////////////////////////////////////////////////////////////////////////
/// \brief returns MAC type of Port
///
 // Parameters:
/// \param port     port of which to return MAC type for
///
/// \return
/// Mac type of port
////////////////////////////////////////////////////////////////////////////////
extern
MacType UniCfgDbGetMacType(TkOnuEthPort port);



////////////////////////////////////////////////////////////////////////////////
/// \brief returns AN mode of Port
///
 // Parameters:
/// \param port     port of which to return AN mode for
///
/// \return
/// mode of port
////////////////////////////////////////////////////////////////////////////////
extern
BOOL UniCfgDbGetAutoNegotiate(TkOnuEthPort port);




////////////////////////////////////////////////////////////////////////////////
/// \brief returns Len error mode of Port
///
 // Parameters:
/// \param port     port of which to return Len error mode for
///
/// \return
/// Len error mode of port
////////////////////////////////////////////////////////////////////////////////
extern
BOOL UniCfgDbGetLenError(TkOnuEthPort port);

////////////////////////////////////////////////////////////////////////////////
/// \brief sets Length error of Port
///
 // Parameters:
/// \param port     port of which to set Length error  for
/// \param mode     Length error to set
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
extern
void UniCfgDbSetLenError(TkOnuEthPort port, BOOL mode);

////////////////////////////////////////////////////////////////////////////////
/// \brief returns Forced capabilities of Port
///
 // Parameters:
/// \param port     port of which to return Forced capabilities for
///
/// \return
/// Forced capabilities of port
////////////////////////////////////////////////////////////////////////////////
extern
EthPortCapability UniCfgDbGetForceModeCaps(TkOnuEthPort port);

////////////////////////////////////////////////////////////////////////////////
/// \brief sets Forced capabilities of Port
///
 // Parameters:
/// \param port     port of which to set Forced capabilities for
/// \param mode     Forced capabilities to set
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
extern
void UniCfgDbSetForceModeCaps(TkOnuEthPort port, EthPortCapability mode);

////////////////////////////////////////////////////////////////////////////////
/// \brief returns AN capabilities of Port
///
 // Parameters:
/// \param port     port of which to return AN capabilities for
///
/// \return
/// AN capabilities of port
////////////////////////////////////////////////////////////////////////////////
extern
EthPortCapability UniCfgDbGetAnModeCaps(TkOnuEthPort port);

////////////////////////////////////////////////////////////////////////////////
/// \brief sets AN capabilities of Port
///
 // Parameters:
/// \param port     port of which to set AN capabilities for
/// \param mode     AN capabilities to set
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
extern
void UniCfgDbSetAnModeCaps(TkOnuEthPort port, EthPortCapability mode);




////////////////////////////////////////////////////////////////////////////////
/// \brief sets Flow control Off threshold of Port
///
 // Parameters:
/// \param port     port of which to set Flow control Off threshold for
/// \param mode     Flow control Off threshold to set
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
extern
void UniCfgDbSetFcOffThresh(TkOnuEthPort port, U8 thresh);

////////////////////////////////////////////////////////////////////////////////
/// \brief returns Max frame size of Port
///
 // Parameters:
/// \param port     port of which to return Max frame size for
///
/// \return
/// Max frame size of port
////////////////////////////////////////////////////////////////////////////////
extern
U16 UniCfgDbGetMaxFrameSize(TkOnuEthPort port);

////////////////////////////////////////////////////////////////////////////////
/// \brief sets Max frame size Port
///
 // Parameters:
/// \param port     port of which to set Max frame size for
/// \param mode     Max frame size to set
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
extern
void UniCfgDbSetMaxFrameSize(TkOnuEthPort port, U16 maxSize);

////////////////////////////////////////////////////////////////////////////////
/// \brief clears capability from force mode caps
///
 // Parameters:
/// \param port     port of which to clr forced cap from
/// \param cap     cap to clear
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
extern
void UniCfgDbClrForceModeCap(TkOnuEthPort port,EthPortCapability cap);

////////////////////////////////////////////////////////////////////////////////
/// \brief sets capability from force mode caps
///
 // Parameters:
/// \param port    port of which to set forced cap from
/// \param cap     cap to set
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
extern
void UniCfgDbSetForceModeCap(TkOnuEthPort port,EthPortCapability cap);

////////////////////////////////////////////////////////////////////////////////
/// \brief clears capability from AN mode caps
///
 // Parameters:
/// \param port     port of which to clr AN cap from
/// \param cap     cap to clear
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
extern
void UniCfgDbClrAnModeCap(TkOnuEthPort,EthPortCapability);

////////////////////////////////////////////////////////////////////////////////
/// \brief set capability from AN mode caps
///
 // Parameters:
/// \param port    port of which to set AN cap from
/// \param cap     cap to set
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
extern
void UniCfgDbSetAnModeCap(TkOnuEthPort,EthPortCapability);

////////////////////////////////////////////////////////////////////////////////
/// \brief sets capability for both forced and AN caps
///
 // Parameters:
/// \param port    port of which to set cap for
/// \param cap     cap to set
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
extern
void UniCfgDbSetBothCaps(TkOnuEthPort port, EthPortCapability cap);




////////////////////////////////////////////////////////////////////////////////
/// \brief sets up uni config DB in pers
///
 // Parameters:
/// none
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
extern
void UniCfgDbInitUniPers(void);


////////////////////////////////////////////////////////////////////////////////
/// \brief sets up uni config DB in pers
///
 // Parameters:
/// none
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
extern
void UniCfgDbInitUniPers10g(void);


////////////////////////////////////////////////////////////////////////////////
/// \brief sets up uni config DB in pers
///
 // Parameters:
/// none
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
extern
void UniCfgDbInitUniPers1g(void);


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
extern
EthPortCapability UniCfgDbFindCurrentCaps(TkOnuEthPort port);


////////////////////////////////////////////////////////////////////////////////
/// \brief returns TX P/N differential swap option for channel
///
 // Parameters:
/// \param chan  Channel to swap
///
/// \return
/// TRUE if swapped FALSE if not swapped
///////////////////////////////////////////////////////////////////////////////
extern
BOOL UniCfgDbGetTxDiffSwap (U8 chan);


////////////////////////////////////////////////////////////////////////////////
/// \brief returns RX P/N differential swap option for channel
///
 // Parameters:
/// \param chan  Channel to swap
///
/// \return
/// TRUE if swapped FALSE if not swapped
///////////////////////////////////////////////////////////////////////////////
extern
BOOL UniCfgDbGetRxDiffSwap (U8 chan);




#if defined(__cplusplus)
}
#endif

#endif // UniConfigDb.h

/*
<:copyright-BRCM:2002-2003:proprietary:epon

   Copyright (c) 2002-2003 Broadcom 
   All Rights Reserved

 This program is the proprietary software of Broadcom and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").  Except as set forth in
 an Authorized License, Broadcom grants no license (express or implied), right
 to use, or waiver of any kind with respect to the Software, and Broadcom
 expressly reserves all rights in and to the Software and all intellectual
 property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,

 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use
    all reasonable efforts to protect the confidentiality thereof, and to
    use this information only in connection with your use of Broadcom
    integrated circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
    PERFORMANCE OF THE SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
    LIMITED REMEDY.
:>*/
#if !defined(CtcMcast_h)
#define CtcMcast_h
////////////////////////////////////////////////////////////////////////////////
/// \file CtcMcast.h
/// \brief China Telecom multicast module
///
///
////////////////////////////////////////////////////////////////////////////////

#include "Teknovus.h"
#include "McastDb.h"
#include "McastVlan.h"
#include "McastTypes.h"
#include "McastMgmt.h"
#include "TkPlatform.h"

#if defined(__cplusplus)
extern "C" {
#endif
typedef enum
{
    CtcMcastGrpDaMacOnly        = 0x00,
    CtcMcastGrpDaMacVid         = 0x01,
    CtcMcastGrpDaMacSaIpv4      = 0x02,
    CtcMcastGrpDaIpv4Vid        = 0x03,
    CtcMcastGrpDaIpv6Vid        = 0x04,
    CtcMcastGrpDaMacSaIpv6      = 0x05,
	
    CtcMcastGrpTypeNum          = 0x06,
    CtcMcastGrpTypeInvalid      = 0xFF       
} CtcMcastGroupType;

typedef enum
{
    CtcMcastIgmpSnoopNoFast     = 0x00000000UL,  // CTC2.1, CTC3.0
    CtcMcastIgmpSnoopFastLeave  = 0x00000001UL,  // CTC2.1, CTC3.0
    CtcMcastHostCtlNoFast       = 0x00000010UL,  // CTC2.1, CTC3.0
    CtcMcastHostCtlFastLeave    = 0x00000011UL,  // CTC2.1, CTC3.0
    CtcMcastMldSnoopNoFast      = 0x00000002UL,  // CTC3.0
    CtcMcastMldSnoopFastLeave   = 0x00000003UL,  // CTC3.0

    Ctc21McastFastModeNums      = 0x00000004UL,
    Ctc30McastFastModeNums      = 0x00000006UL
} CtcMcastFastLeaveMode;

typedef enum
{
    CtcMcastFastLeaveDisable	= 0x00000001UL,
    CtcMcastFastLeaveEnable     = 0x00000002UL
} CtcMcastFastLeaveState;

typedef struct
{
    U8               op;
    VlanTag          vlan[1];
} PACK CtcMcastVlanOpCfg;

typedef struct
{
    U8               op;        //McastVlanActType->U8
    U8               switchVlanNum;
    McastVlanEntry   entry[1];
} PACK CtcMcastTagOpCfg;

extern
U8 CtcMcastCtrlGrpGet(OamVarContainer *cont);

extern
int CtcMcastCtrlGrpSet(OamVarContainer BULK* src, OamVarContainer BULK *cont);

////////////////////////////////////////////////////////////////////////////////
/// \brief Set the multicast vlan tag process for the port
///
/// \Param port Port to set
/// \param cfg  Configuration to set
///
/// \return
/// TRUE if successful, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
int CtcMcastTagOpSet(TkOnuEthPort port, const CtcMcastTagOpCfg * cfg);


////////////////////////////////////////////////////////////////////////////////
/// \brief Set the multicast vlan tag process for the port
///
/// \Param port Port to get
/// \param cfg  Pointer to the configuration buffer to get
///
/// \return
/// The length of the configuration
////////////////////////////////////////////////////////////////////////////////
U8 CtcMcastTagOpGet(TkOnuEthPort port, CtcMcastTagOpCfg * cfg);


////////////////////////////////////////////////////////////////////////////////
/// \brief Set the multicast vlan
///
/// \Param port Port to set
/// \Param len  Length of the configuration
/// \param cfg	Configuration to set
///
/// \return
/// TRUE if successful, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
int CtcMcastVlanSet(TkOnuEthPort port, U8 len, 
                    const CtcMcastVlanOpCfg * cfg, int TriggeredByMcGroup);


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the multicast vlanfor the port
///
/// \Param port Port to get
/// \param cfg	Pointer to the configuration buffer to get
///
/// \return
/// The length of the configuration
////////////////////////////////////////////////////////////////////////////////
extern
U8 CtcMcastVlanGet(TkOnuEthPort port, CtcMcastVlanOpCfg * cfg);


////////////////////////////////////////////////////////////////////////////////
/// \brief Set the multicast vlan
///
/// \Param port Port to set
/// \Param max  Maximum number of groups for the port
///
/// \return
/// TRUE if successful, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
int CtcMcastGroupMaxSet(TkOnuEthPort port, U8 max);


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the multicast vlan
///
/// \Param port Port to get
///
/// \return
/// Maximum number of groups for the port
////////////////////////////////////////////////////////////////////////////////
extern
U8 CtcMcastGroupMaxGet(TkOnuEthPort port);


////////////////////////////////////////////////////////////////////////////////
/// \brief Set the fast leave admin control
///
/// \Param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
int CtcMcastFastLeaveStateSet(CtcMcastFastLeaveState admin);


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the fast leave state
///
/// \Param None
///
/// \return
/// The current fast leave state
////////////////////////////////////////////////////////////////////////////////
extern
CtcMcastFastLeaveState CtcMcastFastLeaveStateGet(void);


////////////////////////////////////////////////////////////////////////////////
/// \brief Set IP multicast control method
///
/// \Param mode The new mode to set
///
/// \return
/// TRUE if successful, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
int CtcMcastSwitchSetMode (OamCtcMcastMode mode);


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the IP multicast control method
///
/// \Param None
///
/// \return
/// The current IP multicast control method
////////////////////////////////////////////////////////////////////////////////
extern
U8 CtcMcastSwitchGetMode(void);

////////////////////////////////////////////////////////////////////////////////
/// \brief Return whether it is host control mode
///
/// \Param None
///
/// \return
/// TRUE if Yes, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
U8 CtcMcastIsHostCtrlMode(void);


////////////////////////////////////////////////////////////////////////////////
/// \brief Return whether it is snooping mode
///
/// \Param None
///
/// \return
/// TRUE if Yes, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
U8 CtcMcastIsSnoopingMode(void);


////////////////////////////////////////////////////////////////////////////////
/// \brief Initialize the CTC multicast module
///
/// \Param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void CtcMcastInit(void);

#if defined(__cplusplus)
}
#endif


#endif // End of File CtcMcast.h


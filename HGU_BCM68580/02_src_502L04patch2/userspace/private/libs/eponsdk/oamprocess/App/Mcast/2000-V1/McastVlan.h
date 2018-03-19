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


#if !defined(McastVlan_h)
#define McastVlan_h
////////////////////////////////////////////////////////////////////////////////
/// \file McastVlan.h
/// \brief IP Multicast VLAN module
///
/// Manages the IP multicast VLAN module
///
////////////////////////////////////////////////////////////////////////////////

#include "Teknovus.h"
#include "Ethernet.h"
#include "McastDb.h"


#if defined(__cplusplus)
extern "C" {
#endif

typedef enum
	{
	McastVlanOpDel,
	McastVlanOpAdd,
	McastVlanOpClr,
	McastVlanOpList,

	McastVlanOpNum
	} PACK McastVlanOp;

typedef struct
	{
	VlanTag   eponVid;
	VlanTag   iptvVid;
	} PACK McastVlanEntry;

typedef struct
	{
	McastVlanActType    op;
	U8               	vlanNum;
	McastVlanEntry   	entry[McastVlanCountPerPort];
	} PACK McastVlanPortCfg;

typedef struct
	{
	McastVlanPortCfg port[McastPortMaxNum];
	} PACK McastVlanCfg;

////////////////////////////////////////////////////////////////////////////////
/// \brief Check the validation of the vid.
///
/// \param vid Vid to check
///
/// \return
/// TRUE if valid, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
BOOL McastVlanValid(VlanTag vid);


////////////////////////////////////////////////////////////////////////////////
/// \brief Check the validation of the vid list.
///
/// \param vlanNum Number of the vlan to check
/// \param data    Pointer to the vlan list
///
/// \return
/// TRUE if valid, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
BOOL McastVlanListValid(U8 vlanNum, const U8 * data);


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the multicast vlan action type for the port
///
/// \param port  Port to find
///
/// \return
/// The multicast vlan action type
////////////////////////////////////////////////////////////////////////////////
extern
McastVlanActType McastVlanActTypeGet(TkOnuEthPort port);


////////////////////////////////////////////////////////////////////////////////
/// \brief Find a multicast vlan entry from port vlan configuration
///
/// \param port  Port to find
/// \param vid   Vid to find
///
/// \return
/// Vlan entry pointer if find, NULL otherwise
////////////////////////////////////////////////////////////////////////////////
extern
McastVlanEntry * McastVlanFind(TkOnuEthPort port, VlanTag vid);


////////////////////////////////////////////////////////////////////////////////
/// \brief Check if there has multicast vlan or not
///
/// \param port  Port to check
///
/// \return
/// TRUE if exist, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
BOOL McastVlanExist(TkOnuEthPort port);


////////////////////////////////////////////////////////////////////////////////
/// \brief Clear the port multicast vlan configuration.
///
/// \param port  Port to clear
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastVlanCfgClr(TkOnuEthPort port);


////////////////////////////////////////////////////////////////////////////////
/// \brief Set the port multicast vlan configuration.
///
/// \param port  Port to set
/// \param cfg   The configuration to set
///
/// \return
/// TRUE when successful, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL McastVlanCfgSet(TkOnuEthPort port, const McastVlanPortCfg BULK * cfg);


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the port multicast vlan configuration.
///
/// \param port  Port to set
///
/// \return
/// The port multicast vlan configuration.
////////////////////////////////////////////////////////////////////////////////
extern
McastVlanPortCfg * McastVlanCfgGet(TkOnuEthPort port);


////////////////////////////////////////////////////////////////////////////////
/// \brief Set the port multicast vlan actions.
///
/// \param port  Port to set
/// \param op    Operation to do(Add/Delete/Clear)
/// \param vlanNum Number of vlans to do
/// \param entry Vlan list
///
/// \return
/// TRUE when successful, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
BOOL McastVlanOpHandle(TkOnuEthPort port, McastVlanOp op,
                  U8 vlanNum, const U8 * data);


////////////////////////////////////////////////////////////////////////////////
/// \brief Set the port multicast vlan Tag operation.
///
/// \param port  Port to set
/// \param op    Operation to do(None/Strip/Switch)
/// \param vlanNum Number of vlans to do
/// \param entrt Vlan list
///
/// \return
/// TRUE when successful, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
BOOL McastVlanActHandle(TkOnuEthPort port, McastVlanActType op,
                     U8 vlanNum, const McastVlanEntry BULK * entry);


////////////////////////////////////////////////////////////////////////////////
/// \brief Initialize the multicast vlan module.
///
/// \param None
///
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastVlanInit(void);

#if defined(__cplusplus)
}
#endif

#endif // McastVlan.h



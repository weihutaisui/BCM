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

#if !defined(CtcVlan_h)
#define CtcVlan_h
////////////////////////////////////////////////////////////////////////////////
/// \file CtcVlan.h
/// \brief China Telecom vlan mode module
///
///
////////////////////////////////////////////////////////////////////////////////
#include "Teknovus.h"
#include "Build.h"
#include "Ethernet.h"
#include "CtcOam.h"


////////////////////////////////////////////////////////////////////////////////
/// CtcVlanSetMode - Set the port level VLAN mode
///
/// This function configures the port default VLAN mode as per the China
/// Telecom specification version 1.2.  Each mode has variable associated data
/// that can be passed through the optional vlan parameter.  If the desired
/// mode does not use this data, the parameter may be left NULL.
///
 // Parameters:
/// \param port Port number, including EPON
/// \param mode VLAN mode, see documentation
/// \param vlan Optional VLAN mode data
/// \param size Size of optional data in bytes
///
/// \return
/// True if operation was successful
////////////////////////////////////////////////////////////////////////////////
extern
BOOL CtcVlanSetMode (TkOnuEthPort uni_port,
                     OamCtcVlanMode mode,
                     const U8 BULK *provData,
                     U16 size);


////////////////////////////////////////////////////////////////////////////////
/// CtcVlanGetMode - Get the currently provisioned VLAN mode
///
/// This function returns the currently provisioned CTC VLAN mode.
///
 // Parameters:
/// \param port Port number, including EPON
///
/// \return
/// Active VLAN mode
////////////////////////////////////////////////////////////////////////////////
extern
OamCtcVlanMode CtcVlanGetMode (TkOnuEthPort port);


////////////////////////////////////////////////////////////////////////////////
/// CtcVlanGetData - Get extra data from the VLAN mode
///
/// This function loads the extra data from the VLAN mode into the supplied
/// VLAN tag buffer.  It returns the size, in byte, that were loaded.
///
 // Parameters:
/// \param port Port number including EPON
/// \param vlan Pointer to target VLAN buffer
///
/// \return
/// Size in bytes written to the VLAN buffer
////////////////////////////////////////////////////////////////////////////////
extern
U16 CtcVlanGetData (TkOnuEthPort port, U8 BULK *vlan);


////////////////////////////////////////////////////////////////////////////////
/// CtcVlanValidVid - Check if the vid is in the port vlan list based on the
///                   vlan mode.
///
 // Parameters:
/// \param port Port to be checked
/// \param vid  The Vid to be checked
///
/// \return
/// TRUE if exists, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
BOOL CtcVlanValidVid(TkOnuEthPort port, VlanTag vid);


////////////////////////////////////////////////////////////////////////////////
/// CtcVlanCommitConfig - Commit VLAN provisioning to NVS
///
/// This function configure the all port CTC VLAN provisioning to NVS.
///
 // Parameters:
///         None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void CtcVlanCommitConfig (void);


////////////////////////////////////////////////////////////////////////////////
/// CtcVlanGetPortDefaultVid - Get the default port VLAN
///
///
 // Parameters:
/// \param  port    Port for which default VLAN needs to be returned
///
/// \return
/// Port default VLAN ID
////////////////////////////////////////////////////////////////////////////////
extern
VlanTag CtcVlanGetPortDefaultVid (TkOnuEthPort port);


////////////////////////////////////////////////////////////////////////////////
/// CtcVlanGetPortSVidByCVid - Get the SVid by the CVid
///
///
 // Parameters:
/// \param  port    Port for checking
/// \param  cVid    CVid for checking
///
/// \return
/// The SVid related with the CVid
////////////////////////////////////////////////////////////////////////////////
extern
VlanTag CtcVlanGetPortSVidByCVid (TkOnuEthPort port, VlanTag cVid);


////////////////////////////////////////////////////////////////////////////////
/// CtcVlanInitDefault - Initialize CTC VLAN mode when NVS record is missing
///
/// This function starts up the CTC VLAN provisioning.  The system defaults to
/// transparent mode
///
 // Parameters:
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void CtcVlanInitDefault (void);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Register CTC VLAN FDS group
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void CtcVlanFdsGroupReg(void);


////////////////////////////////////////////////////////////////////////////////
/// CtcVlanInit - Initialize CTC VLAN mode
///
/// This function starts up the CTC VLAN provisioning if there are CTC Port
/// VLAN records in NVS.
///
 // Parameters:
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void CtcVlanInit (void);

#endif

// End of File CtcVlan.h


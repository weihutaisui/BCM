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

////////////////////////////////////////////////////////////////////////////////
/// \file VlanConfig.c
/// \brief VLAN configuration module
/// \author Lingyong Chen
/// \date Sep 15, 2011
///
/// This file contains the VLAN configuration module.
///
////////////////////////////////////////////////////////////////////////////////

#include "VlanConfig.h"



////////////////////////////////////////////////////////////////////////////////
/// Macro definition
////////////////////////////////////////////////////////////////////////////////
#define VlanInvalid         0


////////////////////////////////////////////////////////////////////////////////
/// Typedef struct definition
////////////////////////////////////////////////////////////////////////////////

typedef struct
	{
	U16				vid;
	U8			    portMap;
	} PACK VlanConfig;

////////////////////////////////////////////////////////////////////////////////
/// Global variable definition
////////////////////////////////////////////////////////////////////////////////
static VlanConfig BULK VlanCfg[VlanMaxCount];

////////////////////////////////////////////////////////////////////////////////
/// VlanFindFree - Find a free VID
///
/// This function scans the VLAN configuration table looking for an instance of
/// the free VID.  If the VID has not been previously allocated VlanMaxCount
/// is returns.  If found the index of the configuration table is returned.
///
 // Parameters:
/// \param None
///
/// \return
/// Index of found VID
////////////////////////////////////////////////////////////////////////////////
static
U8 VlanFindFree (void)
	{
	U8 FAST i;

	for (i = 0; i < VlanMaxCount; ++i)
		{
		if (VlanCfg[i].vid == VlanInvalid)
			{
			return i;
			}
		}
    
	return VlanMaxCount;
	} // VlanFindFree


////////////////////////////////////////////////////////////////////////////////
/// VlanFindVid - Find a non free VID
///
/// This function scans the VLAN configuration table looking for an instance of
/// the supplied VID.  If the VID has not been previously allocated VlanMaxCount
/// is returns.  If found the index of the configuration table is returned.
///
 // Parameters:
/// \param vid VLAN ID to lookup
///
/// \return
/// Index of found VID
////////////////////////////////////////////////////////////////////////////////
//extern
U8 VlanFindVid (U16 vid)
	{
	U8 FAST i;

	for (i = 0; i < VlanMaxCount; ++i)
		{
		if ((vid == VlanCfg[i].vid) && (vid != VlanInvalid))
			{
			return i;
			}
		}
    
	return VlanMaxCount;
	} // VlanFindVid


////////////////////////////////////////////////////////////////////////////////
/// VlanAdd - Add a VLAN ID to VLAN index table
///
/// Ths functions adds a new entry in the VLAN index table.  The VLAN ID is
/// places in the first available opening in the table.
///
 // Parameters:
/// \param vid VLAN ID to add
///
/// \return True if the ID was added, false if the table was full
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL VlanAdd (U8 port, U16 vid)
	{
	U8 FAST idx;

    idx = VlanFindVid(vid); 
    if (idx != VlanMaxCount)
        {
        VlanCfg[idx].portMap |= (U8)(1U << port);
        return TRUE;
        }
    else
        {
        idx = VlanFindFree();
        if (idx != VlanMaxCount)
            {
            VlanCfg[idx].vid = vid;
            VlanCfg[idx].portMap |= (U8)(1U << port);
            return TRUE;           
            }
        }

	return FALSE;
	} // VlanAdd


////////////////////////////////////////////////////////////////////////////////
/// VlanDel - Remove a VLAN ID from the index table
///
/// This function clears a single VLAN ID from the VLAN index table.
///
 // Parameters:
/// \param vid VLAN ID to remove
///
/// \return True is removal was successful, false otherwise
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL VlanDel (U8 port, U16 vid)
	{
	U8 FAST idx = VlanFindVid(vid);
    
	if (idx != VlanMaxCount)
        {
        VlanCfg[idx].portMap &=  ~((U8)(1U << port));
        if (VlanCfg[idx].portMap == 0)
            {
            VlanCfg[idx].vid = VlanInvalid;
            }
        return TRUE;
        }
    
	return FALSE;
	} // VlanDel


// End of File VlanConfig.c


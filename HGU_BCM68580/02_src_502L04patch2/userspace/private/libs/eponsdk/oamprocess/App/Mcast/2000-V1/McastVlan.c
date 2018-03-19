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
/// \file McastVlan.c
/// \brief IP Multicast VLAN module
///
/// Manages the IP multicast VLAN module
///
////////////////////////////////////////////////////////////////////////////////
#include <string.h>
#include "Teknovus.h"
#include "Stream.h"
#include "McastTypes.h"
#include "McastDb.h"
#include "McastVlan.h"
#include "McastBaseRule.h"

static McastVlanCfg  BULK mcastVlanCfg;

////////////////////////////////////////////////////////////////////////////////
/// \brief Balance the given port multicast vlan configuration
///
/// This function makes the multicast vlan itmes are in the first vlanNum 
/// position of the port multicast vlan list.
///
/// \param cfg  The port multicast vlan configuration
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void BalanceMcastVlanCfg(McastVlanPortCfg BULK * cfg)
	{
	U8 FAST i;
	U8 FAST j;
	for(i = 0; i < cfg->vlanNum; i++)
		{
		if(cfg->entry[i].eponVid == McastVlanNonVid)
			{
			for(j = (i + 1); j < McastVlanCountPerPort; j++)
				{
				if(cfg->entry[j].eponVid != McastVlanNonVid)
					{
					cfg->entry[i].eponVid = cfg->entry[j].eponVid;
					cfg->entry[i].iptvVid = cfg->entry[j].iptvVid;
					cfg->entry[j].eponVid = McastVlanNonVid;
					cfg->entry[j].iptvVid = McastVlanNonVid;
					break;
					}
				}
			}
		}
	} // BalanceMcastVlanCfg


////////////////////////////////////////////////////////////////////////////////
/// \brief Check if there is any duplicated configuration item
///
/// This function checks if there is any duplicated configuration item. If 
/// there is, strip them.
///
/// \param cfg  The port multicast vlan configuration
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void CheckDuplicatedCfg(McastVlanPortCfg BULK * cfg)
	{
	U8 FAST i;
	U8 FAST j;
	U8 FAST vlanNum;

	vlanNum = cfg->vlanNum;
	for(i = 0; i < vlanNum; i++)
		{
		if(cfg->entry[i].eponVid == McastVlanNonVid)
			{
			continue;
			}
		
		for(j = (i + 1); j < vlanNum; j++)
			{
			if(cfg->entry[i].eponVid == cfg->entry[j].eponVid)
				{
				cfg->entry[j].eponVid = McastVlanNonVid;
				cfg->entry[j].iptvVid = McastVlanNonVid;
				cfg->vlanNum--;
				}
			}
		}
	
	BalanceMcastVlanCfg(cfg);
	} // CheckDuplicatedCfg


////////////////////////////////////////////////////////////////////////////////
/// \brief Find the given multicast vlan in the port multicast vlan 
///        configuration
///
/// This function finds the given multicast vlan in the port multicast vlan 
/// configuration. If found, return the vlan index; if not found, but there has 
/// free spaces in the port multicast vlan configuration, return the first free
/// space index, otherwise return McastVlanCountPerPort.
///
/// \param cfg   The Configuration to find
/// \param vid   Vid to find
///
/// \return
/// Vlan index
////////////////////////////////////////////////////////////////////////////////
static
U8 FindMcastVlan(const McastVlanPortCfg BULK * BULK cfg, VlanTag vid)
	{
	U8 FAST i;
	for(i = 0; i < McastVlanCountPerPort; i++)
		{
		if(cfg->entry[i].eponVid == vid)
			{
			break;
			}
		}
	
	return i;
	} // FindMcastVlan


////////////////////////////////////////////////////////////////////////////////
/// \brief Build a new port multicast vlan configuration based on the deleting 
///        parameters.
///
/// This function builds the new port multicast vlan configuration for the 
/// deleting actions in the given buffer. During the deleting, it will delete 
/// the multicast vlan with the related the switch vlan
///
/// \param curCfg  Pointer to old configuration  before deleting
/// \param newCfg  Pointer to new configuration  after deleting
/// \param vlanNum The number of vlans for deleting
/// \param entry   Pointer to the vlans list for deleting
///
/// \return
/// TRUE when successful, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
static
BOOL BuildMcastVlanDel(const McastVlanPortCfg BULK * curCfg, 
                  			 McastVlanPortCfg BULK * newCfg, 
                  			 U8 vlanNum, const U8 BULK * data)
	{
	U8 FAST i;
	U8 FAST vlanIdx;
	Stream BULK strm;

	StreamInit(&strm, data);
	memcpy(newCfg, curCfg, sizeof(McastVlanPortCfg));
	for(i = 0; i < vlanNum; i++)
		{
		vlanIdx = FindMcastVlan(curCfg, StreamReadU16(&strm));
		if(vlanIdx != McastVlanCountPerPort)
			{
			newCfg->entry[vlanIdx].eponVid = McastVlanNonVid;
			newCfg->entry[vlanIdx].iptvVid = McastVlanNonVid;
			newCfg->vlanNum--;
			}
		}
	BalanceMcastVlanCfg(newCfg);
	return TRUE;
	} // BuildMcastVlanDel


////////////////////////////////////////////////////////////////////////////////
/// \brief Build a new port multicast vlan configuration for clearing all the 
/// multicast vlans.
///
/// This function builds the new port multicast vlan configuration for the 
/// clearing all the multicast vlans in the given buffer. During the deleting, 
/// it will delete the multicast vlan with the related the switch vlan
///
/// \param curCfg  Pointer to old configuration  before deleting
/// \param newCfg  Pointer to new configuration  after deleting
///
/// \return
/// TRUE when successful, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
static
BOOL BuildMcastVlanClr(const McastVlanPortCfg BULK * curCfg, 
                             McastVlanPortCfg BULK * newCfg)
	{
	newCfg->op = curCfg->op;
	return TRUE;
	} // BuildMcastVlanClr


////////////////////////////////////////////////////////////////////////////////
/// \brief Build a new port multicast vlan configuration for adding a given
/// multicast vlans list.
///
/// This function builds the new port multicast vlan configuration for adding 
/// a list of multicast vlans in the given buffer.
///
/// \param curCfg  Pointer to old configuration  before adding
/// \param newCfg  Pointer to new configuration  after adding
/// \param vlanNum The number of vlans for adding
/// \param entry   Pointer to the vlans list for adding
///
/// \return
/// TRUE when successful, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
static
BOOL BuildMcastVlanAdd(const McastVlanPortCfg BULK * curCfg, 
                  			 McastVlanPortCfg BULK * newCfg, 
                  			 U8 vlanNum, const U8 BULK * data)
	{
	U8 FAST i;
	U8 FAST vlanIdx;
	VlanTag BULK vid;
	Stream  BULK strm;

	StreamInit(&strm, data);
	memcpy(newCfg, curCfg, sizeof(McastVlanPortCfg));
	for(i = 0; i < vlanNum; i++)
		{
		vid = StreamReadU16(&strm);
		vlanIdx = FindMcastVlan(curCfg, vid);
		if(vlanIdx == McastVlanCountPerPort)
			{
			if(newCfg->vlanNum < McastVlanCountPerPort)
				{
				newCfg->entry[newCfg->vlanNum].eponVid = vid;
				newCfg->entry[newCfg->vlanNum].iptvVid = McastVlanNonVid;
				newCfg->vlanNum++;
				}
			else
				{
				return FALSE;
				}
			}
		}
	
	return TRUE;
	} // BuildMcastVlanAdd


////////////////////////////////////////////////////////////////////////////////
/// \brief Build a new port multicast vlan configuration for setting the 
/// multicast vlan tag to NONE.
///
/// \param curCfg  Pointer to old configuration  before setting
/// \param newCfg  Pointer to new configuration  after setting
///
/// \return
/// TRUE when successful, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
static
BOOL BuildMcastVlanActNone(const McastVlanPortCfg BULK * curCfg, 
                          		 McastVlanPortCfg BULK * newCfg)
	{
	U8 FAST i;
	if(McastDbRtHostCtrlGet())
		{
		memset(newCfg, 0, sizeof(McastVlanPortCfg));
		}
	else
		{
		memcpy(newCfg, curCfg, sizeof(McastVlanPortCfg));
		// delete the switch vlan
		for(i = 0; i < newCfg->vlanNum; i++)
			{
			if(newCfg->entry[i].iptvVid != McastVlanNonVid)
				{
				newCfg->entry[i].iptvVid = McastVlanNonVid;
				}
			}
		}
	newCfg->op = McastVlanActNone;
	return TRUE;
	} // BuildMcastVlanActNone


////////////////////////////////////////////////////////////////////////////////
/// \brief Build a new port multicast vlan configuration for setting the 
/// multicast vlan tag to Strip.
///
/// \param curCfg  Pointer to old configuration  before setting
/// \param newCfg  Pointer to new configuration  after setting
///
/// \return
/// TRUE when successful, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
static
BOOL BuildMcastVlanActDel(const McastVlanPortCfg BULK * curCfg, 
                          		  McastVlanPortCfg BULK * newCfg)
	{
	U8 FAST i;
	if(McastDbRtHostCtrlGet())
		{
		memset(newCfg, 0, sizeof(McastVlanPortCfg));
		}
	else
		{
		memcpy(newCfg, curCfg, sizeof(McastVlanPortCfg));
		// delete the switch vlan
		for(i = 0; i < newCfg->vlanNum; i++)
			{
			if(newCfg->entry[i].iptvVid != McastVlanNonVid)
				{
				newCfg->entry[i].iptvVid = McastVlanNonVid;
				}
			}
		}
	newCfg->op = McastVlanActDel;
	return TRUE;
	} // BuildMcastVlanActDel

	
////////////////////////////////////////////////////////////////////////////////
/// \brief Build a new port multicast vlan configuration for setting the 
/// multicast vlan tag to Switch.
///
/// \param curCfg  Pointer to old configuration  before setting
/// \param newCfg  Pointer to new configuration  after setting
/// \param vlanNum The number of vlans for setting
/// \param entry   Pointer to the vlans list for setting
///
/// \return
/// TRUE when successful, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
static
BOOL BuildMcastVlanActTrans(const McastVlanPortCfg BULK * curCfg, 
                          		   McastVlanPortCfg BULK * newCfg, 
                                   U8 vlanNum, 
                                   const McastVlanEntry BULK * entry)
	{
	U8 FAST i;
	U8 FAST vlanIdx;
	if(vlanNum > McastVlanCountPerPort)
		{
		return FALSE;
		}

	if(McastDbRtHostCtrlGet())
		{
		memset(newCfg, 0, sizeof(McastVlanPortCfg));
		newCfg->vlanNum = vlanNum;
		memcpy(newCfg->entry, entry, 
			                  sizeof(McastVlanEntry)*vlanNum);
		}
	else
		{
		memcpy(newCfg, curCfg, sizeof(McastVlanPortCfg));
		// delete the switch vlan
		for(vlanIdx = 0; vlanIdx < newCfg->vlanNum; vlanIdx++)
			{
			if(newCfg->entry[vlanIdx].iptvVid != McastVlanNonVid)
				{
				newCfg->entry[vlanIdx].iptvVid = McastVlanNonVid;
				}
			}
		
		// add new switch vlan
		for(i = 0; i < vlanNum; i++)
			{
			vlanIdx = FindMcastVlan(curCfg, entry[i].eponVid);
			if(vlanIdx != McastVlanCountPerPort)
				{
				// vlan has existed, and the switch vlan
				newCfg->entry[vlanIdx].iptvVid = entry[i].iptvVid;
				}
			else
				{
				if(newCfg->vlanNum < McastVlanCountPerPort)
					{
					// vlan doesn't exist and there has free space for this vlan
					newCfg->entry[newCfg->vlanNum].eponVid = 
				                                    entry[i].eponVid;
					newCfg->entry[newCfg->vlanNum].iptvVid = 
						                            entry[i].iptvVid;
					newCfg->vlanNum++;
					}
				else
					{
					return FALSE;
					}
				}
			}
		}
	newCfg->op = McastVlanActTrans;
	return TRUE;
	} // BuildMcastVlanActTrans


////////////////////////////////////////////////////////////////////////////////
/// \brief Check the validation of the vid.
///
/// \param vid Vid to check
///
/// \return
/// TRUE if valid, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL McastVlanValid(VlanTag vid)
	{
	if((vid > McastVlanNonVid) && (vid <= 0xFFF))
		{
		return TRUE;
		}
	
	return FALSE;
	} // McastVlanValid


////////////////////////////////////////////////////////////////////////////////
/// \brief Check the validation of the vid list.
///
/// \param vlanNum Number of the vlan to check
/// \param data    Pointer to the vlan list
///
/// \return
/// TRUE if valid, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL McastVlanListValid(U8 vlanNum, const U8 * data)
	{
	U8 FAST i;
	Stream BULK strm;
	StreamInit(&strm, data);
	for(i = 0; i < vlanNum; i++)
		{
		if(!McastVlanValid(StreamReadU16(&strm)))
			{
			return FALSE;
			}
		}
	return TRUE;
	} // McastVlanListValid


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the multicast vlan action type for the port
///
/// \param port  Port to find
///
/// \return
/// The multicast vlan action type
////////////////////////////////////////////////////////////////////////////////
//extern
McastVlanActType McastVlanActTypeGet(TkOnuEthPort port)
	{
	return mcastVlanCfg.port[port].op;
	} // McastVlanActTypeGet


////////////////////////////////////////////////////////////////////////////////
/// \brief Find a multicast vlan entry from port vlan configuration
///
/// \param port  Port to find
/// \param vid   Vid to find
///
/// \return
/// Vlan entry pointer if find, NULL otherwise
////////////////////////////////////////////////////////////////////////////////
//extern
McastVlanEntry * McastVlanFind(TkOnuEthPort port, VlanTag vid)
	{
	U8 FAST vlanIdx;
	BalanceMcastVlanCfg(&mcastVlanCfg.port[port]);
	vlanIdx = FindMcastVlan(&mcastVlanCfg.port[port], vid);
	if(vlanIdx != McastVlanCountPerPort)
		{
		return &mcastVlanCfg.port[port].entry[vlanIdx];
		}
	else
		{
		return NULL;
		}
	} // McastVlanFind


////////////////////////////////////////////////////////////////////////////////
/// \brief Check if there has multicast vlan or not
///
/// \param port  Port to check
///
/// \return
/// TRUE if exist, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL McastVlanExist(TkOnuEthPort port)
	{
	return (mcastVlanCfg.port[port].vlanNum != 0);
	} // McastVlanExist


////////////////////////////////////////////////////////////////////////////////
/// \brief Clear the port multicast vlan rules.
///
/// \param port  Port to clear
/// \param cfg   The configuration to clear
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void McastVlanClr(TkOnuEthPort port, const McastVlanPortCfg BULK * cfg)
	{
	U8 FAST vlanIdx;
	McastVlanEntry BULK * BULK entry;
	
	if(cfg->op == McastVlanActTrans)
		{
		for(vlanIdx = 0; vlanIdx < cfg->vlanNum; vlanIdx++)
			{
			entry = &cfg->entry[vlanIdx];
			(void)McastBaseVlan(port, entry->eponVid, 
				                      entry->iptvVid,
				                      FALSE);
			}
		}
	else
		{
		if(cfg->op == McastVlanActDel)
			{
			(void)McastBaseVlanStrip(port, FALSE);
			}
		
		for(vlanIdx = 0; vlanIdx < cfg->vlanNum; vlanIdx++)
			{
			entry = &cfg->entry[vlanIdx];
			(void)McastBaseVlan(port, entry->eponVid, 
				                      entry->iptvVid,
				                      FALSE);
			}
		}
	} // McastVlanClr


////////////////////////////////////////////////////////////////////////////////
/// \brief Set the port multicast vlan rules.
///
/// \param port  Port to set
/// \param cfg   The configuration to set
///
/// \return
/// TRUE when successful, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
static
BOOL McastVlanSet(TkOnuEthPort port, const McastVlanPortCfg BULK * cfg)
	{
	U8   FAST i;
	U8   FAST vlanIdx;
	BOOL FAST ret;
	McastVlanEntry BULK * entry;
	ret = TRUE;
	if(cfg->op == McastVlanActTrans)
		{
		for(vlanIdx = 0; vlanIdx < cfg->vlanNum; vlanIdx++)
			{
			entry = &cfg->entry[vlanIdx];
			if(!McastBaseVlan(port, entry->eponVid, 
				                    entry->iptvVid,
				                    TRUE))
				{
				ret = FALSE;
				break;
				}
			}
		
		if(!ret)
			{
			for(i = 0; i < vlanIdx; i++)
				{
				entry = &cfg->entry[i];
				(void)McastBaseVlan(port, entry->eponVid, 
				                      	  entry->iptvVid,
				                          FALSE);
				}
			}
		}
	else
		{
		if(cfg->op == McastVlanActDel)
			{
			if(!McastBaseVlanStrip(port, TRUE))
				{
				ret = FALSE;
				(void)McastBaseVlanStrip(port, FALSE);
				}
			}
		
		if(ret)
			{
			for(vlanIdx = 0; vlanIdx < cfg->vlanNum; vlanIdx++)
				{
				entry = &cfg->entry[vlanIdx];
				if(!McastBaseVlan(port, entry->eponVid, 
				                        entry->iptvVid,
				                        TRUE))
					{
					ret = FALSE;
					break;
					}
				}
			
			if(!ret)
				{
				for(i = 0; i < vlanIdx; i++)
					{
					entry = &cfg->entry[i];
					(void)McastBaseVlan(port, entry->eponVid, 
				                              entry->iptvVid,
				                              FALSE);
					}
				}
			}
		}
	
	return ret;
	} // McastVlanCfgSet


////////////////////////////////////////////////////////////////////////////////
/// \brief Clear the port multicast vlan configuration.
///
/// \param port  Port to clear
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void McastVlanCfgClr(TkOnuEthPort port)
	{
	if(port >= McastPortMaxNum)
		{
		return;
		}
	
	McastVlanClr(port, McastVlanCfgGet(port));
	memset(&mcastVlanCfg.port[port], 0, sizeof(McastVlanPortCfg));
	} // McastVlanCfgClr


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
BOOL McastVlanCfgSet(TkOnuEthPort port, const McastVlanPortCfg BULK * cfg)
	{
	BOOL FAST ret = TRUE;
	McastVlanPortCfg BULK curCfg;

	if(port >= McastPortMaxNum)
		{
		return FALSE;
		}

	// Backup the current configuration
	memcpy(&curCfg, McastVlanCfgGet(port), sizeof(McastVlanPortCfg));

	// Clear current configuration
	McastVlanCfgClr(port);

	if(!McastVlanSet(port, cfg))
		{
		// Fails when adding new configuration.
		// Restore current configuratioin!
		(void)McastVlanSet(port, &curCfg);
		memcpy(&mcastVlanCfg.port[port], &curCfg, sizeof(McastVlanPortCfg));
		ret = FALSE;
		}
	else
		{
		// Update the new configuration.
		memcpy(&mcastVlanCfg.port[port], cfg, sizeof(McastVlanPortCfg));
		}

	return ret;
	} // McastVlanCfgSet


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the port multicast vlan configuration.
///
/// \param port  Port to set
///
/// \return
/// The port multicast vlan configuration.
////////////////////////////////////////////////////////////////////////////////
//extern
McastVlanPortCfg * McastVlanCfgGet(TkOnuEthPort port)
	{
	if(port >= McastPortMaxNum)
		{
		return NULL;
		}
	
	BalanceMcastVlanCfg(&mcastVlanCfg.port[port]);
	return &mcastVlanCfg.port[port];
	} // McastVlanCfgGet


////////////////////////////////////////////////////////////////////////////////
/// \brief Set the port multicast vlan actions.
///
/// \param port  Port to set
/// \param op    Operation to do(Add/Delete/Clear)
/// \param vlanNum Number of vlans to do
/// \param entrt Vlan list
///
/// \return
/// TRUE when successful, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL McastVlanOpHandle(TkOnuEthPort port, McastVlanOp op,
                  U8 vlanNum, const U8 * data)
	{
	BOOL FAST ret;
	McastVlanPortCfg BULK newCfg;
	McastVlanPortCfg BULK * BULK curCfg;

	if((port >= McastPortMaxNum) || (!McastVlanListValid(vlanNum, data)))
		{
		return FALSE;
		}
	
	memset(&newCfg, 0, sizeof(McastVlanPortCfg));
	curCfg = McastVlanCfgGet(port);
	
	switch(op)
		{
		case McastVlanOpDel:
			ret = BuildMcastVlanDel(curCfg, &newCfg, 
				                             vlanNum, data);
			break;
			
		case McastVlanOpAdd:
			ret = BuildMcastVlanAdd(curCfg, &newCfg, 
				                             vlanNum, data);
			break;
			
		case McastVlanOpClr:
			ret = BuildMcastVlanClr(curCfg, &newCfg);
			break;
						
		default:
			ret = FALSE;
			break;
		}
	
	if(ret)
		{
		CheckDuplicatedCfg(&newCfg);
		ret = McastVlanCfgSet(port, &newCfg);
		}
	
	return ret;
	} // McastVlanOpHandle


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
//extern
BOOL McastVlanActHandle(TkOnuEthPort port, McastVlanActType op,
                     U8 vlanNum, const McastVlanEntry BULK * entry)
	{
	BOOL FAST ret;
	McastVlanPortCfg BULK newCfg;
	McastVlanPortCfg BULK * BULK curCfg;

	if((port >= McastPortMaxNum) || 
	   (!McastVlanListValid((vlanNum *2), (U8 *)entry)))
		{
		return FALSE;
		}

	memset(&newCfg, 0, sizeof(McastVlanPortCfg));
	curCfg = McastVlanCfgGet(port);

	switch(op)
		{
		case McastVlanActNone:
			ret = BuildMcastVlanActNone(curCfg, &newCfg);
			break;
		case McastVlanActDel:
			ret = BuildMcastVlanActDel(curCfg, &newCfg);
			break;
		case McastVlanActTrans:
			ret = BuildMcastVlanActTrans(curCfg, &newCfg,
				                                  vlanNum, entry);
			break;
		default:
			ret = FALSE;
			break;
		}
	
	if(ret)
		{
		CheckDuplicatedCfg(&newCfg);
		ret = McastVlanCfgSet(port, &newCfg);
		}
	
	return ret;
	} // McastVlanActHandle


////////////////////////////////////////////////////////////////////////////////
/// \brief Initialize the multicast vlan module.
///
/// \param None
///
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void McastVlanInit(void)
	{
	memset(&mcastVlanCfg, 0, sizeof(mcastVlanCfg));
	} // McastVlanInit

// McastVlan.c


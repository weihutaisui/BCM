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
/// \file McastDb.c
/// \brief IP Multicast DataBase module
///
////////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include "Teknovus.h"
#include "McastTypes.h"
#include "McastDb.h"

static McastParamCfg BULK mcastParamCfg;
static McastRunTimeInfo  BULK mcastRunTimeInfo;

////////////////////////////////////////////////////////////////////////////////
/// \brief Set snoop mode from pers
///
/// \param None
///
/// \return 
/// return the snoop mode from pers
////////////////////////////////////////////////////////////////////////////////
//extern
void McastDbParamCfgSet(const McastParamCfg * cfg)
	{
	memcpy(&mcastParamCfg, cfg, sizeof(McastParamCfg));
	} // McastDbParamCfgSet


////////////////////////////////////////////////////////////////////////////////
/// \brief Get snoop mode from pers
///
/// \param None
///
/// \return 
/// return the snoop mode from pers
////////////////////////////////////////////////////////////////////////////////
//extern
McastParamCfg * McastDbParamCfgGet(void)
	{
	return &mcastParamCfg;
	} // McastDbParamCfgGet


////////////////////////////////////////////////////////////////////////////////
/// \brief Get snoop mode from pers
///
/// \param None
///
/// \return 
/// return the snoop mode from pers
////////////////////////////////////////////////////////////////////////////////
//extern
McastSnoopMode McastDbSnoopModeGet(void)
	{
	return mcastParamCfg.snoopMode;
	} // McastDbProxyLeaveGet


////////////////////////////////////////////////////////////////////////////////
/// \brief Get snoop options from pers
///
/// \param None
///
/// \return 
/// return the snoop options from pers
////////////////////////////////////////////////////////////////////////////////
//extern
U8 McastDbSnoopOptGet(void)
	{
	return mcastParamCfg.snoopOpt;
	} // McastDbSnoopOptGet


////////////////////////////////////////////////////////////////////////////////
/// \brief Get multicast vlan type from pers
///
/// \param None
///
/// \return 
/// return the multicast vlan type from pers
////////////////////////////////////////////////////////////////////////////////
//extern
McastVlanType McastDbVlanTypeGet(void)
	{
	return mcastParamCfg.vlanType;
	} // McastDbVlanTypeGet


////////////////////////////////////////////////////////////////////////////////
/// \brief Get Proxy Leave ability
///
/// \param None
///
/// \return 
/// TRUE if proxy leave is supported, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL McastDbProxyLeaveGet(void)
	{
	return mcastParamCfg.proxyLeave;
	} // McastDbProxyLeaveGet 


////////////////////////////////////////////////////////////////////////////////
/// \brief Get Robustness Count from pers
///
/// \param None
///
/// \return 
/// Return the Robustness Count from pers
////////////////////////////////////////////////////////////////////////////////
//extern
U8 McastDbRobustnessCountGet(void)
	{
	if(mcastParamCfg.robustnessCount == 0)
		{
		return DefaultRobustness;
		}
	return mcastParamCfg.robustnessCount;
	} // McastDbRobustnessCountGet 


////////////////////////////////////////////////////////////////////////////////
/// \brief Get last member query count from pers
///
/// \param None
///
/// \return 
/// Return the last member query count from pers
////////////////////////////////////////////////////////////////////////////////
//extern
U8 McastDbLmqCountGet(void)
	{
	if(mcastParamCfg.lmqCount == 0)
		{
		return DefaultLmqCount;
		}
	
	return mcastParamCfg.lmqCount;
	} // McastDbLmqCountGet


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the limit groups number of the port
///
/// \param port    Port to be gotten
///
/// \return 
/// Limit number of groups
////////////////////////////////////////////////////////////////////////////////
//extern
U16 McastDbGrpLimitGet(TkOnuEthPort port)
	{
	return mcastParamCfg.groupLimit[port];
	} // McastDbGrpLimitGet 


////////////////////////////////////////////////////////////////////////////////
/// \brief Set the maximum groups number for the port
///
/// \param port    Port to be set
/// \param max     Maximum number of groups
///
/// \return 
/// TRUE when successful, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL McastDbGrpLimitSet(TkOnuEthPort port, U16 max)
	{
	if(max > McastGroupLimitPerPort)
		{
		return FALSE;
		}
	
	mcastParamCfg.groupLimit[port] = max;
	return TRUE;
	} // McastMgrGrpLimitSet 


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the rate limit for the port
///
/// \param port    Port to be gotten
///
/// \return 
/// Rate limit number of groups
////////////////////////////////////////////////////////////////////////////////
//extern
U16 McastDbRateLimitGet(TkOnuEthPort port)
	{
	return mcastParamCfg.rateLimit[port];
	} // McastDbRateLimitGet 


////////////////////////////////////////////////////////////////////////////////
/// \brief Return the port downstream queue for multicast traffic
///
/// \param port    Port to be gotten
///
/// \return 
/// Queue number
////////////////////////////////////////////////////////////////////////////////
//extern
U8 McastDbDnQueueGet(TkOnuEthPort port)
	{
	return mcastParamCfg.dnQueue[port];
	} // McastDbDnQueueGet 


////////////////////////////////////////////////////////////////////////////////
/// \brief Set the multicast mode configuration
///
/// \param cfg The mode configuration
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void McastDbRtModeCfgSet(const McastModeCfg BULK * cfg)
	{
	memcpy(&mcastRunTimeInfo.modeCfg, cfg, sizeof(McastModeCfg));
	} // McastDbRtModeCfgSet


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the multicast mode
///
/// \param None
///
/// \return
/// Mode configration
////////////////////////////////////////////////////////////////////////////////
//extern
McastSnoopMode McastDbRtSnoopModeGet(void)
	{
	return mcastRunTimeInfo.modeCfg.snoopMode;
	} // McastDbRtSnoopModeGet


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the multicast mode
///
/// \param None
///
/// \return
/// Mode configration
////////////////////////////////////////////////////////////////////////////////
//extern
U8 McastDbRtSnoopOptionGet(void)
	{
	return mcastRunTimeInfo.modeCfg.snoopOpt;
	} // McastDbRtSnoopOptionGet


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the multicast mode
///
/// \param None
///
/// \return
/// Mode configration
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL McastDbRtHostCtrlGet(void)
	{
	return mcastRunTimeInfo.modeCfg.hostCtrl;
	} // McastDbRtHostCtrlGet


////////////////////////////////////////////////////////////////////////////////
/// \brief Set the fast leave admin
///
/// \param en Enable/Disable
///
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void McastDbRtFastLeaveSet(BOOL en)
	{
	mcastRunTimeInfo.fastLeave = en;
	} // McastMgrFastLeaveSet 


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the fast leave state
///
/// \param None
///
/// \return 
/// The fast leave state
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL McastDbRtFastLeaveGet(void)
	{
	return mcastRunTimeInfo.fastLeave;
	} // McastDbRtFastLeaveGet 


////////////////////////////////////////////////////////////////////////////////
/// \brief Set active group number
///
/// \param count Count(zero) to set
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void McastDbRtOnuGrpCountSet(U16 val)
	{
	mcastRunTimeInfo.actGrpCount = val;
	} // McastDbRtOnuGrpCountSet


////////////////////////////////////////////////////////////////////////////////
/// \brief inceace one group for ONU
///
/// \param port Port to increase
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void McastDbRtOnuGrpCountInc(void)
	{
	mcastRunTimeInfo.actGrpCount++;
	} // McastDbRtOnuGrpCountInc


////////////////////////////////////////////////////////////////////////////////
/// \brief Decreace one group for ONU
///
/// \param port Port to Decreace
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void McastDbRtOnuGrpCountDec(void)
	{
	if(mcastRunTimeInfo.actGrpCount > 0)
		{
		mcastRunTimeInfo.actGrpCount--;
		}
	} // McastDbRtOnuGrpCountDec



////////////////////////////////////////////////////////////////////////////////
/// \brief Get active group number
///
/// \param port Port to get
///
/// \return
/// The active group number
////////////////////////////////////////////////////////////////////////////////
//extern
U16 McastDbRtOnuGrpCountGet(void)
	{
	return mcastRunTimeInfo.actGrpCount;
	} // McastDbRtOnuGrpCountGet


////////////////////////////////////////////////////////////////////////////////
/// \brief Set the rate limit counter
///
/// \param count Count(zero) to set
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void McastDbRtRateLimitCountSet(U16 val)
	{
	mcastRunTimeInfo.rateLimitCount = val;
	} // McastDbRtRateLimitCountSet


////////////////////////////////////////////////////////////////////////////////
/// \brief Get rate limit count
///
/// \param None
///
/// \return
/// The rate limit counter
////////////////////////////////////////////////////////////////////////////////
//extern
U16 McastDbRtRateLimitCountGet(void)
	{
	return mcastRunTimeInfo.rateLimitCount;
	} // McastDbRtRateLimitCountGet


////////////////////////////////////////////////////////////////////////////////
/// \brief Set the port qualifier
///
/// \param port Port to set
///
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void McastDbRtPortQualSet(TkOnuEthPort port, McastGrpQual qual)
	{
	mcastRunTimeInfo.portQual[port] = qual;
	} // McastDbRtPortQualSet 


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the port qualifier
///
/// \param port Port to get
///
/// \return 
/// The fast leave state
////////////////////////////////////////////////////////////////////////////////
//extern
McastGrpQual McastDbRtPortQualGet(TkOnuEthPort port)
	{
	return mcastRunTimeInfo.portQual[port];
	} // McastDbRtPortQualGet 


////////////////////////////////////////////////////////////////////////////////
/// \brief Set active group number for the port
///
/// \param port Port to set
/// \param count Count(zero) to set
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void McastDbRtPortGrpCountSet(TkOnuEthPort port, U16 val)
	{
	mcastRunTimeInfo.portGrpCount[port] = val;
	} // McastDbRtPortGrpCountSet


////////////////////////////////////////////////////////////////////////////////
/// \brief Get active group number for the port
///
/// \param port Port to get
///
/// \return
/// The active group number for the port
////////////////////////////////////////////////////////////////////////////////
//extern
U16 McastDbRtPortGrpCountGet(TkOnuEthPort port)
	{
	return mcastRunTimeInfo.portGrpCount[port];
	} // McastDbRtPortGrpCountGet


////////////////////////////////////////////////////////////////////////////////
/// \brief Check the group limit of the port
///
/// \param port Port to check
///
/// \return
/// TRUE if not excess, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL McastDbRtPortGrpLimitCheck(TkOnuEthPort port)
	{
	if((McastDbRtHostCtrlGet()) || 
	   (mcastRunTimeInfo.portGrpCount[port] < mcastParamCfg.groupLimit[port]))
		{
		return TRUE;
		}
	
	return FALSE;
	} // McastDbRtPortGrpLimitCheck



////////////////////////////////////////////////////////////////////////////////
/// \brief increace one group into the port
///
/// \param port Port to increase
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void McastDbRtPortGrpInc(TkOnuEthPort port)
	{
	mcastRunTimeInfo.portGrpCount[port]++;
	} // McastDbRtPortGrpInc


////////////////////////////////////////////////////////////////////////////////
/// \brief Decreace one group into the port
///
/// \param port Port to Decreace
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void McastDbRtPortGrpDec(TkOnuEthPort port)
	{
	if(mcastRunTimeInfo.portGrpCount[port] > 0)
		{
		mcastRunTimeInfo.portGrpCount[port]--;
		if(mcastRunTimeInfo.portGrpCount[port] == 0)
			{
			if(McastDbRtHostCtrlGet())
				{
				McastDbRtPortQualSet(port, McastGrpNone);
				}
			else
				{
				McastDbRtPortQualSet(port, McastGrpByL2DaIpSa);
				}
			}
		}
	} // McastDbRtPortGrpDec


////////////////////////////////////////////////////////////////////////////////
/// \brief Set the port frame credits
///
/// \param port Port to set
///
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void McastDbRtPortFrmCreditsSet(TkOnuEthPort port, U16 val)
	{
	mcastRunTimeInfo.portFrmCredits[port] = val;
	} // McastDbRtPortFrmCreditsSet 


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the port frame credits
///
/// \param port Port to get
///
/// \return 
/// the port frame credits
////////////////////////////////////////////////////////////////////////////////
//extern
U16 McastDbRtPortFrmCreditsGet(TkOnuEthPort port)
	{
	return mcastRunTimeInfo.portFrmCredits[port];
	} // McastDbRtPortFrmCreditsGet 


////////////////////////////////////////////////////////////////////////////////
/// \brief decrease the port frame credits
///
/// \param port Port to get
///
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void McastDbRtPortFrmCreditsDec(TkOnuEthPort port)
	{
	mcastRunTimeInfo.portFrmCredits[port]--;
	} // McastDbRtPortFrmCreditsDec 


////////////////////////////////////////////////////////////////////////////////
/// \brief Initialize the multicast data base module.
///
/// \param None
///
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void McastDbInit(void)
	{
	memset(&mcastParamCfg, 0, sizeof(mcastParamCfg));
	memset(&mcastRunTimeInfo, 0, sizeof(mcastRunTimeInfo));
	} // McastDbInit


// end of McastDb.c


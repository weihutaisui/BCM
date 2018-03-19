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


#if !defined(McastDb_h)
#define McastDb_h
////////////////////////////////////////////////////////////////////////////////
/// \file McastDb.h
/// \brief IP Multicast DataBase module
///
////////////////////////////////////////////////////////////////////////////////

#include "Teknovus.h"
#include "TkPlatform.h"
#include "PortCapability.h"
#include "Ethernet.h"
#include "McastTypes.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define McastPortMaxNum              MAX_UNI_PORTS
#define McastOnuGrpMaxNum            200
#define McastSsmSrcMaxNum            8
#define McastVlanCountPerPort        8
#define McastInvalidQueue            31
#define McastGroupLimitPerPort       200
#define McastGroupDefaultLimit       McastGroupLimitPerPort
#define McastFrmRateUnlimit          0
#define McastFrmRateDefaultLimit     10

#define McastDataIpv4DaMacLow        0x01005E000100
#define McastDataIpv4DaMacHigh       0x01005E7FFFFF
#define McastDataIpv6DaMacLow        0x333300000100
#define McastDataIpv6DaMacHigh       0x3333FFFFFFFF

#define McastDataDaIpV4Low           0xE0000100   // 224.0.1.0
#define McastDataDaIpV4High          0xEFFFFFFF   // 239.255.255.255


#define McastGrpAddrLength           (16)
#define McastGrpAddrMacOffset        (10)
#define McastGrpAddrIpv4Offset       (12)
#define McastGrpAddrIpv6Offset       (0)

typedef enum
    {
    McastSnoopNone          = 0,
    McastSnoopIgmpV1V2      = 1 << 0,
    McastSnoopIgmpV3        = 1 << 1,
    McastSnoopMldV1         = 1 << 2,
    McastSnoopMldV2         = 1 << 3,
    McastSnoopIgmp          = McastSnoopIgmpV1V2 | McastSnoopIgmpV3,
    McastSnoopMld           = McastSnoopMldV1 | McastSnoopMldV2,
    McastSnoopIgmpMld       = McastSnoopIgmp | McastSnoopMld
    } PACK McastSnoopMode;

typedef enum
	{
	McastVlanCVlan,
	McastVlanSVlan,
	
	McastVlanNum
	} PACK McastVlanType;

typedef struct
	{
	McastSnoopMode   snoopMode;
	U8       		 snoopOpt;
	McastVlanType    vlanType;
	BOOL             proxyLeave;
	U8    	         robustnessCount;
	U8               lmqCount;
	U16              groupLimit[McastPortMaxNum];
	U16              rateLimit[McastPortMaxNum];
	U8               dnQueue[McastPortMaxNum];
	} PACK McastParamCfg;

typedef struct
	{
	McastSnoopMode   snoopMode;
	U8       		 snoopOpt;
	BOOL             hostCtrl;
	U8       		 rsvd;
	} PACK McastModeCfg;

typedef enum
	{
	McastGrpNone                = 0x0000,
	McastGrpByL2Da				= 0x0001,	// Add by L2 DA
	McastGrpByVid 				= 0x0002,	// Add by VID
	McastGrpByIpDa			    = 0x0004,	// Add by IP Da
	McastGrpByIpSa    		    = 0x0008,   // Add by IP Sa
	McastGrpByLink      		= 0x0010,   // Add by Link
	McastGrpByL2DaVid           = McastGrpByL2Da|McastGrpByVid,
	McastGrpByIpDaVid           = McastGrpByIpDa|McastGrpByVid,
	McastGrpByL2DaIpSa          = McastGrpByL2Da|McastGrpByIpSa,
	McastGrpByIpDaIpSa          = McastGrpByIpDa|McastGrpByIpSa,

	// New values get added as powers of 2
	McastGrpQualForceU16		= 0x7FFF
	} PACK McastGrpQual;


typedef struct
	{
	McastModeCfg     modeCfg;
	BOOL             fastLeave;
	U16              actGrpCount;
	U16              rateLimitCount;
	McastGrpQual     portQual[McastPortMaxNum];
	U16              portGrpCount[McastPortMaxNum];
	U16              portFrmCredits[McastPortMaxNum];
	} PACK McastRunTimeInfo;

typedef struct
	{
	U8              byte[McastGrpAddrLength];
	} PACK McastGrpAddr;

typedef enum
	{
	McastGrpStateUnused,	// table entry unused
	McastGrpStatePending,	// awaiting reply to query
	McastGrpStateActive,	// currently forwarding traffic
	McastGrpStateReported,	// replied to query
	McastStateGrpMaxNum
	} PACK McastGrpState;

typedef struct
	{
	McastSsmFilterType  type;
	U32                 srcMap;
	U16                 srcTimer[McastSsmSrcMaxNum];  
	} PACK McastGrpSsmEntry;

typedef struct
	{
	McastGrpQual   		qual;
	VlanTag             grpVid;
	BOOL                v4;
	McastGrpAddr        grpAddr;
	U64                 ssmDomainMap;
	McastGrpSsmEntry    ssmEntry[McastPortMaxNum];
	McastGrpState       grpState[McastPortMaxNum];
	U16                 timerVal[McastPortMaxNum];
	U8                  proxyLmqCount[McastPortMaxNum];
	BOOL                proxyState[McastPortMaxNum];
	} PACK McastGrpEntry;


typedef struct
	{
	McastSsmFilterType  type;
	U16        			srcNum;
	U8                  byte[0];
	} PACK McastGrpSsmInfo;


typedef struct
	{
	BOOL                fromHost;
	McastGrpQual   		qual;
	VlanTag             grpVid;
	BOOL                v4;
	McastGrpAddr        grpAddr;
	McastGrpSsmInfo     ssmInfo;
	} PACK McastGrpInfo;


typedef union
	{
	IpAddr          ipv4;
	IpV6Addr        ipv6;
	} PACK SaIpAddr;


typedef enum
	{
	SsmSrcIdNone                = 0,
	SsmSrcIdBegin               = 1,
	
	SsmSrcIdEnd                 = (McastSsmSrcMaxNum + SsmSrcIdBegin),
	SsmSrcIdNum                 = SsmSrcIdEnd
	} PACK SsmSrcId;

typedef struct
	{
	BOOL          v4;
	U16           inUse;
	U16           domainId;
	SaIpAddr      srcIp;
	} PACK SsmSrcEntry;

typedef struct
	{
	SsmSrcEntry   entryList[SsmSrcIdNum];
	} PACK SsmSrcRecord;


typedef enum
	{
	McastVlanActNone,
	McastVlanActDel,
	McastVlanActTrans,
	McastVlanActAdd,
	McastVlanActDrop,
	
	McastVlanActNum
	} PACK McastVlanActType;

typedef struct
	{
	McastVlanActType  type;
	VlanTag           outVid;
	} PACK McastVlanActInfo;

typedef void (* McastVlanActCb) (U8, VlanTag, BOOL, McastVlanActInfo *);

typedef struct
	{
	McastVlanActCb     up;
	McastVlanActCb     dn;
	} PACK McastVlanActHdl;

////////////////////////////////////////////////////////////////////////////////
/// \brief Set snoop mode from pers
///
/// \param None
///
/// \return 
/// return the snoop mode from pers
////////////////////////////////////////////////////////////////////////////////
extern
void McastDbParamCfgSet(const McastParamCfg * cfg);


////////////////////////////////////////////////////////////////////////////////
/// \brief Get snoop mode from pers
///
/// \param None
///
/// \return 
/// return the snoop mode from pers
////////////////////////////////////////////////////////////////////////////////
extern
McastParamCfg * McastDbParamCfgGet(void);


////////////////////////////////////////////////////////////////////////////////
/// \brief Get snoop mode from pers
///
/// \param None
///
/// \return 
/// return the snoop mode from pers
////////////////////////////////////////////////////////////////////////////////
extern
McastSnoopMode McastDbSnoopModeGet(void);


////////////////////////////////////////////////////////////////////////////////
/// \brief Get snoop options from pers
///
/// \param None
///
/// \return 
/// return the snoop options from pers
////////////////////////////////////////////////////////////////////////////////
extern
U8 McastDbSnoopOptGet(void);


////////////////////////////////////////////////////////////////////////////////
/// \brief Get multicast vlan type from pers
///
/// \param None
///
/// \return 
/// return the multicast vlan type from pers
////////////////////////////////////////////////////////////////////////////////
extern
McastVlanType McastDbVlanTypeGet(void);


////////////////////////////////////////////////////////////////////////////////
/// \brief Get Proxy Leave ability
///
/// \param None
///
/// \return 
/// TRUE if proxy leave is supported, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
BOOL McastDbProxyLeaveGet(void);


////////////////////////////////////////////////////////////////////////////////
/// \brief Get Robustness Count from pers
///
/// \param None
///
/// \return 
/// Return the Robustness Count from pers
////////////////////////////////////////////////////////////////////////////////
extern
U8 McastDbRobustnessCountGet(void); 


////////////////////////////////////////////////////////////////////////////////
/// \brief Get last member query count from pers
///
/// \param None
///
/// \return 
/// Return the last member query count from pers
////////////////////////////////////////////////////////////////////////////////
extern
U8 McastDbLmqCountGet(void);


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the limit groups number of the port
///
/// \param port    Port to be gotten
///
/// \return 
/// Limit number of groups
////////////////////////////////////////////////////////////////////////////////
extern
U16 McastDbGrpLimitGet(TkOnuEthPort port);


////////////////////////////////////////////////////////////////////////////////
/// \brief Set the maximum groups number for the port
///
/// \param port    Port to be set
/// \param max     Maximum number of groups
///
/// \return 
/// TRUE when successful, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
BOOL McastDbGrpLimitSet(TkOnuEthPort port, U16 max);    


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the rate limit for the port
///
/// \param port    Port to be gotten
///
/// \return 
/// Rate limit number of groups
////////////////////////////////////////////////////////////////////////////////
extern
U16 McastDbRateLimitGet(TkOnuEthPort port); 


////////////////////////////////////////////////////////////////////////////////
/// \brief Return the port downstream queue for multicast traffic
///
/// \param port    Port to be gotten
///
/// \return 
/// Queue number
////////////////////////////////////////////////////////////////////////////////
extern
U8 McastDbDnQueueGet(TkOnuEthPort port);



////////////////////////////////////////////////////////////////////////////////
/// \brief Set the multicast mode configuration
///
/// \param cfg The mode configuration
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastDbRtModeCfgSet(const McastModeCfg BULK * cfg);


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the multicast mode
///
/// \param None
///
/// \return
/// Mode configration
////////////////////////////////////////////////////////////////////////////////
extern
McastSnoopMode McastDbRtSnoopModeGet(void);


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the multicast mode
///
/// \param None
///
/// \return
/// Mode configration
////////////////////////////////////////////////////////////////////////////////
extern
U8 McastDbRtSnoopOptionGet(void);


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the multicast mode
///
/// \param None
///
/// \return
/// Mode configration
////////////////////////////////////////////////////////////////////////////////
extern
BOOL McastDbRtHostCtrlGet(void);


////////////////////////////////////////////////////////////////////////////////
/// \brief Set the fast leave admin
///
/// \param en Enable/Disable
///
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastDbRtFastLeaveSet(BOOL en);


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the fast leave state
///
/// \param None
///
/// \return 
/// The fast leave state
////////////////////////////////////////////////////////////////////////////////
extern
BOOL McastDbRtFastLeaveGet(void);


////////////////////////////////////////////////////////////////////////////////
/// \brief Set active group number
///
/// \param count Count(zero) to set
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastDbRtOnuGrpCountSet(U16 val);


////////////////////////////////////////////////////////////////////////////////
/// \brief inceace one group for ONU
///
/// \param port Port to increase
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastDbRtOnuGrpCountInc(void);


////////////////////////////////////////////////////////////////////////////////
/// \brief Decreace one group for ONU
///
/// \param port Port to Decreace
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastDbRtOnuGrpCountDec(void);


////////////////////////////////////////////////////////////////////////////////
/// \brief Get active group number
///
/// \param port Port to get
///
/// \return
/// The active group number
////////////////////////////////////////////////////////////////////////////////
extern
U16 McastDbRtOnuGrpCountGet(void);


////////////////////////////////////////////////////////////////////////////////
/// \brief Set the rate limit counter
///
/// \param count Count(zero) to set
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastDbRtRateLimitCountSet(U16 val);


////////////////////////////////////////////////////////////////////////////////
/// \brief Get rate limit count
///
/// \param None
///
/// \return
/// The rate limit counter
////////////////////////////////////////////////////////////////////////////////
extern
U16 McastDbRtRateLimitCountGet(void);


////////////////////////////////////////////////////////////////////////////////
/// \brief Set the port qualifier
///
/// \param port Port to set
///
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastDbRtPortQualSet(TkOnuEthPort port, McastGrpQual qual);


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the port qualifier
///
/// \param port Port to get
///
/// \return 
/// The fast leave state
////////////////////////////////////////////////////////////////////////////////
extern
McastGrpQual McastDbRtPortQualGet(TkOnuEthPort port);


////////////////////////////////////////////////////////////////////////////////
/// \brief Set active group number for the port
///
/// \param port Port to set
/// \param count Count(zero) to set
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastDbRtPortGrpCountSet(TkOnuEthPort port, U16 val);


////////////////////////////////////////////////////////////////////////////////
/// \brief Get active group number for the port
///
/// \param port Port to get
///
/// \return
/// The active group number for the port
////////////////////////////////////////////////////////////////////////////////
extern
U16 McastDbRtPortGrpCountGet(TkOnuEthPort port);


////////////////////////////////////////////////////////////////////////////////
/// \brief Check the group limit of the port
///
/// \param port Port to check
///
/// \return
/// TRUE if not excess, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
BOOL McastDbRtPortGrpLimitCheck(TkOnuEthPort port);


////////////////////////////////////////////////////////////////////////////////
/// \brief increace one group into the port
///
/// \param port Port to increase
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastDbRtPortGrpInc(TkOnuEthPort port);


////////////////////////////////////////////////////////////////////////////////
/// \brief Decreace one group into the port
///
/// \param port Port to Decreace
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastDbRtPortGrpDec(TkOnuEthPort port);


////////////////////////////////////////////////////////////////////////////////
/// \brief Set the port frame credits
///
/// \param port Port to set
///
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastDbRtPortFrmCreditsSet(TkOnuEthPort port, U16 val);


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the port frame credits
///
/// \param port Port to get
///
/// \return 
/// the port frame credits
////////////////////////////////////////////////////////////////////////////////
extern
U16 McastDbRtPortFrmCreditsGet(TkOnuEthPort port);


////////////////////////////////////////////////////////////////////////////////
/// \brief decrease the port frame credits
///
/// \param port Port to get
///
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastDbRtPortFrmCreditsDec(TkOnuEthPort port);


////////////////////////////////////////////////////////////////////////////////
/// \brief Initialize the multicast data base module.
///
/// \param None
///
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void McastDbInit(void);


#if defined(__cplusplus)
}
#endif

#endif // McastDb.h


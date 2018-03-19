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


#if !defined(IgmpSnoop_h)
#define IgmpSnoop_h
////////////////////////////////////////////////////////////////////////////////
/// \file IgmpSnoop.h 
/// \brief IGMP Snooping for TK37xx ONUs
///
/// This file contains code and data structures for snooping IGMP frames in
/// the ONU data path, and adjusting forwarding rules for corresponding
/// Ethernet multicast frames appropriately.
///
////////////////////////////////////////////////////////////////////////////////

#include "Teknovus.h"
#include "Ip.h"
#include "Igmp.h"
#include "UserPortMap.h"
#include "OntMgr.h"

#if defined(__cplusplus)
extern "C" {
#endif

#if !defined(CTC_PROXY)
#define CTC_PROXY 0
#endif

extern U8 BULK curIgmpMaxGroups;

typedef struct
    {
	U8	groupLimit;	// number of groups the port is allowed to join
	U8	dnQ;		// downstream queue for classification
	} IgmpPortCfg;

typedef enum
	{
	IgmpAddGroupByL2Da		= 0x0001,	// Add by L2 DA
	IgmpAddGroupByVid 		= 0x0002,	// Add by VID
	IgmpAddGroupByIpDa		= 0x0004,	// Add by IP V4 DA
	IgmpAddGroupByIpSa		= 0x0008,	// Add by IP V4 SA

	// New values get added as powers of 2
	IgmpMaxFwdQualifier		= 0x8000	// To force this enum to U16
	} IgmpGrpFwdQualifier;

typedef enum
	{
	IgmpDefaultSnooping		= 0x0,	// Send to CPU if snooping enabled
	IgmpDisableDnAutoSnoop	= 0x1,	// No auto-forward Dn IGMP to CPU
	IgmpDisableUpAutoSnoop	= 0x2,	// No auto-forward Up IGMP to CPU
	IgmpBlockOnNoGroups		= 0x4,	// If #groups == 0, block IGMP from port

	// New values get added as powers of 2
	IgmpMaxSnoopOptions		= 0x8
	} IgmpSnoopOption;

typedef struct
	{
	U8					robustnessCount;    // IGMP robustness count
	U8					lmqCount;    		// IGMP Last Member Query count
	IgmpPortCfg			portCfg[TkOnuNumUserPorts];	// IGMP port settings
	IgmpGrpFwdQualifier	grpFwdQual;    		// Group Forwarding Qualifier
	IgmpSnoopOption		snoopOpt;			// IGMP Snooping Options
	} NvsIgmpRecord;

typedef enum
	{
	IgmpStateUnused,		// table entry unused
	IgmpStatePending,		// awaiting reply to query
	IgmpStateActive,		// currently forwarding traffic
	IgmpStateReported		// replied to query
	} IgmpState;

typedef struct
	{
	U32             ipAddr;
	U16             vid;
	Tk3701DluRuleId ruleId;	// rule which forwards traffic for this group
	U8              numClauses; // # rule clauses used for this group
    U8              uniUse[TkOnuNumUserPorts]; // # groups active per UNI port
	IgmpState       portState[TkOnuNumUserPorts];
    U16             timerVal[TkOnuNumUserPorts]; // Timer for this group
   
#if CTC_PROXY 
// for CTC2.0 2008.06.11
                    // set to some magic number if it's last member query
    U8              ifLastMemQuery[TkOnuNumUserPorts];
                    // The last member query counter
    U8              lastMemQueryCounter[TkOnuNumUserPorts];
#endif
	} IgmpFwd;

typedef struct
	{
	IgmpGrpFwdQualifier	grpFwdQual;    		// Group Forwarding Qualifier
	IgmpSnoopOption		snoopOpt;			// IGMP Snooping Options
	} IgmpExtendedInfo;


// IGMP VLAN Configuration
#define TkOnuMaxIgmpVlan	16

// Structure used to convey information about an IP MC group
// and also the ports involved
typedef struct
	{
	U32 ipAddr;  // Group Address
	U8  portMap; // Bitmap of ports involved in the IGMP group
	U16 vid;     // VID associated with this group
	} OnuIgmpGroupInfo;


typedef struct
	{
	U16			eponVid;    		// EPON VLAN ID
	U16			userVid;    		// User VLAN ID
	U8			maxGroups;			// Max allowed IGMP groups for this VLAN
	} IgmpVlanCfg;

typedef struct
	{
	Tk3701DluRuleId		rule;			// rule to drop IPMC for VID
	U8					activeCount;	// # groups active on this VID
	} IgmpVlanActiveCfg;

typedef struct
	{
	BOOL		ignoreUnmanaged;    		// Action for unmanaged groups
	U8			numVlans;    				// Number of IGMP VLANs
	IgmpVlanCfg	vlanCfg[TkOnuMaxIgmpVlan];  // IGMP port settings
	} NvsIgmpVlanRecord;

typedef struct
	{
	NvsIgmpVlanRecord	nvsConfig;
	Tk3701DluRuleId		rule[TkOnuMaxIgmpVlan]; // rule to drop IPMC for VID
	U8					activeCount[TkOnuMaxIgmpVlan]; // #groups active per VID
	} IgmpVlanRecord;

typedef enum
    {
    IgmpLeaveForwardingStandard,
    IgmpLeaveForwardingZero,
    IgmpLeaveForwardingAnyUnknown,
    // add new values here
    IgmpLeaveForwardingCount
    } IgmpLeaveForwarding;

typedef struct
	{
	U8	limit[TkOnuNumUserPorts];
	} NvsIgmpRateLimits;


////////////////////////////////////////////////////////////////////////////////
/// IgmpQState: return queue needed for group based on port states 
///
 // Parameters:
///	\param entry	Forwarding table entry to query
/// 
/// \return 
/// Queue number to use.  TkOnuNumDnQueues if none
////////////////////////////////////////////////////////////////////////////////
extern
TkOnuDnQset IgmpQState (IgmpFwd const BULK* entry);



////////////////////////////////////////////////////////////////////////////////
/// DnFwdSet:  Set downstream forwarding rule
///
 // Parameters:
///	\param rule		Rule to change
/// \param id		Rule id for this group
/// \param queue	Queue for rule
/// 
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void DnFwdSet (Tk3701DluRule BULK* rule, Tk3701DluRuleId id, TkOnuDnQset qset);



////////////////////////////////////////////////////////////////////////////////
/// ValidIgmpIpAddr:  Check if the address is a valid IGMP IP address
///
 // Parameters:
///	\param ipAddr	IP Address to validate
///
/// \return
/// TRUE if valid address, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
BOOL ValidIgmpIpAddr(IpAddr const BULK *ipAddr);



////////////////////////////////////////////////////////////////////////////////
/// IgmpDnHandle:  Handle downstream IGMP frames
///
 // Parameters:
/// \param link		Link on which IGMP frame was received
/// \param vid      VID on which this IGMP frame was received
///	\param igmp 	IGMP frame to process
/// 
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void IgmpDnHandle (LinkIndex link, U16 vid, IgmpFrame const BULK* igmp);



////////////////////////////////////////////////////////////////////////////////
/// IgmpUpHandle:  Handle upstream IGMP frames
///
 // Parameters:
///	\param igmp 	IGMP frame to process
/// \param vid      VID on which this IGMP frame was received
///	\param igmp		IGMP frame to process
/// 
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void IgmpUpHandle (TkOnuEthPort port, U16 vid, IgmpFrame const BULK* igmp);



////////////////////////////////////////////////////////////////////////////////
/// IgmpHandleTimer:  Handle Timer expiry events
///
 // Parameters:
///	\param timerId	Timer that has expired
/// 
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void IgmpHandleTimer (OntmTimerId timerId);



////////////////////////////////////////////////////////////////////////////////
/// RestoreMcastRules:  Restore multicast rules
///
/// Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
void RestoreMcastRules(void);



////////////////////////////////////////////////////////////////////////////////
/// RestoreIgmpRules:  Restore IGMP rules which was cleared during fiber-pull
///
 // Parameters:
/// \param id		
///	\param rule		
/// \param queue	
/// 
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void RestoreIgmpRules (void);


////////////////////////////////////////////////////////////////////////////////
/// SuspendIgmp:  Suspend IGMP traffic
///
 // Parameters:
///	\param None
/// 
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void SuspendIgmp (void);



////////////////////////////////////////////////////////////////////////////////
/// SuspendIgmp:  Resume IGMP traffic
///
 // Parameters:
///	\param None
/// 
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void ResumeIgmp (void);



////////////////////////////////////////////////////////////////////////////////
/// ClearIgmpLue:  Clear all LUE entries relating to IGMP
///
 // Parameters:
///	\param newCount		Number of IGMP Groups to support
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void ClearIgmpLue (U8 newCount);



////////////////////////////////////////////////////////////////////////////////
/// IgmpReconfigure:  Reconfigure IGMP snooping in the ONU
///
 // Parameters:
///	\param cont 	Pointer to the container containing OAM information
/// 
/// \return 
/// TRUE if reconfigure was successful, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
BOOL IgmpReconfigure(OamVarContainer const BULK *cont);



////////////////////////////////////////////////////////////////////////////////
/// IgmpGetVlanCfg:  Get IGMP VLAN configuration
///
 // Parameters:
///	\param reply	Pointer to the OAM reply container
/// 
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void IgmpGetVlanCfg(OamVarContainer BULK *reply);



////////////////////////////////////////////////////////////////////////////////
/// IgmpSetVlanCfg:  Set IGMP VLAN configuration
///
 // Parameters:
///	\param src	Pointer to OAM container with new configuration
/// 
/// \return 
/// TRUE if successful, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
BOOL IgmpSetVlanCfg(OamVarContainer BULK *src);



////////////////////////////////////////////////////////////////////////////////
/// IgmpGetGroupLimits:  Get IGMP configuration
///
 // Parameters:
///	\param cfg  Pointer to buffer to hold config
/// 
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void IgmpGetGroupLimits (OamIgmpConfig BULK* cfg);



////////////////////////////////////////////////////////////////////////////////
/// IgmpGetNextActiveGroup:  Starting at an index, get next active group
///
 // Parameters:
/// \param index	 Index to start from
/// \param groupInfo Pointer to memory where group info is stored
/// 
/// \return 
/// Index to be used as starting point for next call;
/// >= IgmpMaxGroups when no more active groups exist
////////////////////////////////////////////////////////////////////////////////
extern
U8 IgmpGetNextActiveGroup (U8 index, OnuIgmpGroupInfo BULK *groupInfo);



////////////////////////////////////////////////////////////////////////////////
/// IgmpGetActiveGroupCount:  Get count of groups that are currently active
///
 // Parameters:
/// \param port		Port for which group count needs to be returned
///
/// \return
/// Number of groups currently active for the specified port
////////////////////////////////////////////////////////////////////////////////
extern
U8 IgmpGetActiveGroupCount (TkOnuEthPort port);



////////////////////////////////////////////////////////////////////////////////
/// OamIgmpClrGroups:  Clear all IGMP groups associated with a given port
///
 // Parameters:
/// \param port		Port for which group need to be deleted
///
/// \return
/// TRUE if operation was successful, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
BOOL OamIgmpClrGroups (TkOnuEthPort port);



////////////////////////////////////////////////////////////////////////////////
/// OamIgmpDelGroup:  Forcefully delete an IGMP group
///
 // Parameters:
/// \param portMap  Bitmap of ports from which group is to be deleted
/// \param vid		VID for which group needs to be deleted
/// \param addr     The IGMP group that needs to be deleted
/// 
/// \return 
/// TRUE if delete was successful, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
BOOL OamIgmpDelGroup (U8 portMap, U16 vid, IpAddr BULK* addr);



////////////////////////////////////////////////////////////////////////////////
/// OamIgmpAddGroup:  Forcefully add an IGMP group
///
 // Parameters:
/// \param portMap  Bitmap of port(s) to which to add the group
/// \param vid		VID for which group needs to be added
/// \param addr     The IGMP group that needs to be added
/// 
/// \return 
/// TRUE if add was successful, FALSE otherwise (incl. if group already active)
////////////////////////////////////////////////////////////////////////////////
extern
BOOL OamIgmpAddGroup (U8 portMap, U16 vid, IpAddr const BULK* addr);



////////////////////////////////////////////////////////////////////////////////
/// IpmcFilterVid:  Setup VID filter for IPMC data frames
///
 // Parameters:
///	\param cfg	VLAN configuration to apply
/// 
/// \return 
/// TRUE if configuration was successful, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
BOOL IpmcFilterVid (IgmpVlanRecord BULK *cfg);



////////////////////////////////////////////////////////////////////////////////
/// IgmpAllocDnRule:  Allocate as many rules as required for IGMP group
///
 // Parameters:
///	\param entry	IGMP entry for which rules need to be allocated
/// 
/// \return 
/// Allocated rule ID if successful, CfgRuleNotFound otherwise
////////////////////////////////////////////////////////////////////////////////
extern
Tk3701DluRuleId IgmpAllocDnRule (IgmpFwd BULK* entry);



////////////////////////////////////////////////////////////////////////////////
/// UpdateRuleDest:  Update forwarding rule to include this port
///
 // Parameters:
///	\param entry	Entry for which to create forwarding rule
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void UpdateRuleDest (IgmpFwd const BULK* entry);



////////////////////////////////////////////////////////////////////////////////
/// CreateFwdRule:  Create forwarding rule for given entry
///
 // Parameters:
///	\param entry	Entry for which to create forwarding rule
/// 
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void CreateFwdRule (IgmpFwd const BULK* entry);



////////////////////////////////////////////////////////////////////////////////
/// IgmpConfigDlu:  Configure DLU rules for IGMP snooping
///
 // Parameters:
///	\param numGroups	Number of groups to configure
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void IgmpConfigDlu (U8 numGroups);



////////////////////////////////////////////////////////////////////////////////
/// IgmpInit:  Initialize IGMP module
///
 // Parameters:
/// \param None
/// 
/// \return 
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void IgmpInit (void);



////////////////////////////////////////////////////////////////////////////////
/// IgmpValidGrpFwdQual: Is Group Forward Qualifier OK for this platform?
///
 // Parameters:
///	\param map		Bit map of fields to qualify a group on
/// 
/// \return 
/// TRUE is OK, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
BOOL IgmpValidGrpFwdQual (IgmpGrpFwdQualifier map);



////////////////////////////////////////////////////////////////////////////////
/// IgmpValidSnoopOpt: Are IGMP Snooping Options OK for this platform
///
 // Parameters:
///	\param opt		Bit map of options
/// 
/// \return 
/// TRUE is OK, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
BOOL IgmpValidSnoopOpt (IgmpSnoopOption opt);



////////////////////////////////////////////////////////////////////////////////
/// IgmpLeaveForwardingSet: Set IGMP leave forwarding behavior
///
/// Parameters:
///	\param fwd		behavior to set
////////////////////////////////////////////////////////////////////////////////
extern
OamVarErrorCode IgmpUnknownLeaveFwdSet(IgmpLeaveForwarding fwd);



////////////////////////////////////////////////////////////////////////////////
/// IgmpLeaveForwardingGet: Get IGMP leave forwarding behavior
///
/// \return 
/// The currently provisioned behavior
////////////////////////////////////////////////////////////////////////////////
extern
IgmpLeaveForwarding IgmpUnknownLeaveFwdGet ();


////////////////////////////////////////////////////////////////////////////////
// IgmpConfigRateLimits:	Configure IGMP rate limiting
// 
// Parameters:
// \param cont	Pointer to the container containing OAM information
// 
// \return
// TRUE if provisioning was applied successfully, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
extern
BOOL IgmpConfigRateLimit(TkOnuEthPort port, OamVarContainer const BULK *cont);



////////////////////////////////////////////////////////////////////////////////
// IgmpGetRateLimit: returns the current rate limit for the specified port
//
// Parameters:
// \param port	The port to retrieve the rate limit for
//
// \return
// The current IGMP rate limit for the specified port
////////////////////////////////////////////////////////////////////////////////
extern
U8 IgmpGetRateLimit(TkOnuEthPort port);


////////////////////////////////////////////////////////////////////////////////
/// IgmpClearDluRules:  Remove DLU rules for IGMP snooping
///
 // Parameters:
///	\param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void IgmpClearDluRules (void);


////////////////////////////////////////////////////////////////////////////////
/// IgmpLeaveForwardingGet: Get IGMP leave forwarding behavior
///
/// \return
/// The currently provisioned behavior
////////////////////////////////////////////////////////////////////////////////
extern
IgmpLeaveForwarding IgmpUnknownLeaveFwdGet(void);


#if defined(__cplusplus)
}
#endif

#endif // IgmpSnoop.h

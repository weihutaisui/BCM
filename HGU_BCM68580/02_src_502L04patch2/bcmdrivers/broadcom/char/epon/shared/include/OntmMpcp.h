/*
*  Copyright 2011, Broadcom Corporation
*
* <:copyright-BRCM:2011:proprietary:epon
* 
*    Copyright (c) 2011 Broadcom 
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

/*                Copyright(c) 2002-2010 Broadcom Corporation                 */

#if !defined(OntmMpcp_h)
#define OntmMpcp_h
////////////////////////////////////////////////////////////////////////////////
/// \file OntmMpcp.h
/// \brief MPCP support for ONT Manager
///
/// Handles processing of MPCP layer events such as network clock
/// synchronization, discovery, and registration
///
////////////////////////////////////////////////////////////////////////////////

#if defined(__cplusplus)
extern "C" {
#endif

#include "Teknovus.h"
#include "PonManager.h"
#include "bcm_epon_cfg.h"
#include "EponUser.h"

#define REGISTER_NACK_BACKOFF_SUPPORT 1


typedef struct
    {
    rdpa_epon_link_mpcp_state state;
    LinkIndex associatedUniDirLink; // right now only one associated link
    U8        waitForGateWatchDog;
    } PACK BiDirLink;

typedef struct
    {
    U8 useCount; // how many Bidirectional links are associated with me?
    } PACK UniDirLink;


// Mpcp configuration
typedef struct
    {
    MacAddr           oltMacAddr;         // MAC Address of the OLT
    BiDirLink         biDirLinks[TkOnuMaxBiDirLlids];
    UniDirLink        uniDirLinks[TkOnuMaxRxOnlyLlids];
    LinkIndex         numToRegister;
    LinkIndex         numRegistered;
    LinkIndex         numDeregisteredSinceAudit;
    MpcpInterval16    syncTime;
    U16               idleTime;
    MpcpInterval16    windowRoom;
    U8                onuCapability;          // ONU capability
    U8                oltCapability;          // OLT capability
    U8                allLinksWatchDog;    // w.d. for all links being down
    U8                thrashingWatchDog;   // w.d. for too many deregister
    BOOL              failsafeEnabled;     // failsafe reset enable
#if REGISTER_NACK_BACKOFF_SUPPORT
    U32               silencedLinks;          // Links that failed LOID auth.
    U32               pendingSilencedLinks;   // are placed in silent mode.
    U8                silenceWatchDog;        // w.d. for the silent mode.
    BOOL              deallocatesilence;      //dealloc silence flag;
    U8                silencetime; 
#endif // REGISTER_NACK_BACKOFF_SUPPORT
    }PACK OntmMpcpCfg;


#define MaxMpcpGateCheck 5
#define MpcpGateCheckTimer (1000) //1S
#define MpcpOutTimer       10 //10S
#define PMC_NONSTANDARD_REG_REQUEST   0

extern U8 mpcpGateNum ;


#if REGISTER_NACK_BACKOFF_SUPPORT
extern
void SilenceTimeSet(U8 time);

extern
void SilenceDeallocFlagSet(BOOL flag);
#endif

///////////////////////////////////////////////////////////////////////////////
/// OntmMpcpIgnoreFailsafeSet - Enable/disable MPCP registration failsafe ignore
///
/// This function enables or disables the ignore failsafe option which will keep
/// the ONU from rebooting if it can not register.  The no register cycle count
/// will also be reset any time the ignore failsafe state is modified.
///
/// \param enable Enable/Disable ignore failsafe
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OntmMpcpSetFailsafeEnabled (BOOL enable);



////////////////////////////////////////////////////////////////////////////////
/// OntmMpcpIgnoreFailsafeGet - Get MPCP registration failsafe ignore safe
///
/// This function returns the state of the ignore failsafe option which will
/// prevent the ONU from rebooting if it can not register.
///
///
/// \return
/// TRUE if ignore failsafe is enabled
////////////////////////////////////////////////////////////////////////////////
extern
BOOL OntmMpcpIsFailsafeEnabled (void);


////////////////////////////////////////////////////////////////////////////////
/// OntmMpcpMpcpForwardSet: Set the mpcp forward control variable.
///
/// \param fwd   the value to set.
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OntmMpcpMpcpForwardSet(BOOL fwd);


////////////////////////////////////////////////////////////////////////////////
/// OntmMpcpMpcpForwardGet: Get the mpcp forward control variable.
///
/// \param none
///
/// \return
/// TRUE if need to forwarding mpcp to external CPU, otherwise FALSE
////////////////////////////////////////////////////////////////////////////////
extern
BOOL OntmMpcpMpcpForwardGet(void);


////////////////////////////////////////////////////////////////////////////////
/// OntmMpcpLinksRegistered:  counts number of links currently registered
///
/// None
///
/// \return
/// Number of links registered
////////////////////////////////////////////////////////////////////////////////
extern
U8 OntmMpcpLinksRegistered (void);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Change the number of links to register
///
/// \param  newNumLinks     New value for number of links
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void OntmMpcpAdjustNumLinks (U8 newNumLinks);

extern
U8 OntmMpcpGetNumLinks(void);

extern
void OntmMpcpSetNewMac(U8 link,const MacAddr *mac) ;


////////////////////////////////////////////////////////////////////////////////
/// \brief  Reset registration state of all links
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void MpcpReset(void);


////////////////////////////////////////////////////////////////////////////////
/// HandleMpcp:  Handle MPCP messages after sync is achieved
///
/// \param link     Link on which message arrived
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void HandleMpcp (U8 BULK *msg);

/*
 * OntmMpcpAssignMcast:  Assign mcast link
 *
 * op                      Operation type, refer to rdpa_epon_assign_mcast_op
 * unicast_link_id     Link of unicast, only useful in associate mode
 * mcast_link           Input: refer to rdpa_epon_mcast_link_t
 * mlink_idx             Output: multicast phy link index
 * return                 TRUE if assignment successful
 */
BOOL OntmMpcpAssignMcast (rdpa_epon_assign_mcast_op op, LinkIndex unicast_link_id, 
    rdpa_epon_mcast_link_t *mcast_link, uint32_t *mlink_idx);



////////////////////////////////////////////////////////////////////////////////
/// \brief  Register MPCP FDS group
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void OntmMpcpFdsGroupReg(void);


extern
BOOL OntmMpcpPacketsCheck(U8 *data);
extern
BOOL OntmOamPacketsCheck(U8 *data, U8 *vlanNum);


////////////////////////////////////////////////////////////////////////////////
/// OntmMpcpInit:  Initialize MPCP module
///
/// None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OntmMpcpInit (void);



////////////////////////////////////////////////////////////////////////////////
/// OntmMpcpTimeOutSet:  Background checking on links
///
 // Parameters:
/// None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OntmMpcpTimeOutSet (LinkIndex link, U16 time);



////////////////////////////////////////////////////////////////////////////////
/// OntmMpcpTimeOutGet:  Background checking on links
///
 // Parameters:
/// None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
U16 OntmMpcpTimeOutGet (LinkIndex link);



///////////////////////////////////////////////////////////////////////////////
/// IsLinkInService:  Checks if a link is in service
///
/// \param link     Link being queried
///
/// \return
/// TRUE if link is in service, FALSE otherwise
///////////////////////////////////////////////////////////////////////////////
extern
BOOL IsLinkInService (LinkIndex link);


////////////////////////////////////////////////////////////////////////////////
/// StartRegisteringNewLink:  Pick a new link and try register it
///
/// WARNING. This function should not be called when all links are registered.
///
 // Parameters:
///\param link     Link being operated
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void StartRegisteringNewLink(LinkIndex link);


///////////////////////////////////////////////////////////////////////////////
/// \brief  Get a bitmap of links which are in service
///
/// \return Bitmap of in service links
///////////////////////////////////////////////////////////////////////////////
extern
LinkMap MpcpActiveLinkMap(void);


////////////////////////////////////////////////////////////////////////////////
// Debug use only
////////////////////////////////////////////////////////////////////////////////


extern
void OntmMpcpCmdFlush (U8 argc,         //< arg count
                        const char BULK* const argv[]   //< arg values
                        );

/* set gate los time check interval, never fails */
void GateLosCheckTimeSet(U16 time);

/* get gate los time check interval, never fails */
U16 GateLosCheckTimeGet(void);

extern
void OntmMpcpHandleTimer (EponTimerModuleId id);

rdpa_epon_link_mpcp_state OntmMpcpLinkStateGet (LinkIndex link);

extern
void OntmMpcpCmdInfo (void);
    
#if defined(__cplusplus)
}
#endif

#endif // OntmMpcp.h

/*
* <:copyright-BRCM:2012:proprietary:epon
* 
*    Copyright (c) 2012 Broadcom 
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

#if !defined(OamIeeeCommon_h)
#define OamIeeeCommon_h
////////////////////////////////////////////////////////////////////////////////
/// \file   OamIeeeCommon.h
/// \brief  Shared IEEE OAM implementation
///
////////////////////////////////////////////////////////////////////////////////

#include "Teknovus.h"
#include "Oam.h"
#include "Oui.h"

typedef enum
    {
    OntOamStatTxFrames,
    OntOamStatRxFrames,

    OntOamStatNumStats
    } OntOamStat;


/// OAM discovery state
typedef enum
    {
    OamDiscNoOam,           // disable OAM
    OamDiscPassiveWait,
    OamDiscUnsatisfied,     // std send_remote_1 state
    OamDiscSatisfied,       // std send_remote_2 state
    OamDiscComplete,        // std send_any state
    OamLoopback             // in loopback, possibly discarding
    } OamDiscoveryState;


typedef enum
    {
    AlmRequestNone      = 0,
    AlmRequestOlt       = (1 << 0), // in case the OLT is not the master
    AlmRequestMaster    = (1 << 1),
    AlmRequestSecondary = (1 << 2)
    } AlmRequestIfs;


typedef struct
    {
    OamDiscoveryState   discState;
    OamInfoStateAction  loopbackState;
    U16                 maxPduSize;
    U8                  minRate;             ///< ticks between heartbeats
    U8                  ticksSinceSend;
    U8                  ticksUntilLinkFail;
    U8                  txCredits;
    U16                 eventSeq;
    U64                 framesRx;
    U64                 framesTx;
    U64                 unSuptCodesRx;
    U64                 orgSpecificTx;
    U64                 orgSpecificRx;
    U64                 informationRx;
    U64                 informationTx;
    U16                 localInfoRevision;
    OamInfoTlv          lastRemoteInfo; ///< our peer's info
    U16                 lastRemoteFlags;
    BOOL                needToStartEncrypt;
    AlmRequestIfs       alarmRequest;
    U16                 vendorNeg;
    OamInfoTlv          lastLocalInfo;
    } OamPerLinkData;


extern BOOL ctcIeeeOamProcessing;


////////////////////////////////////////////////////////////////////////////////
/// \brief  get the linkOamData of the link
///
/// \param  link     
///
/// \return pointer to linkOamData[link]
////////////////////////////////////////////////////////////////////////////////
extern
OamPerLinkData* OamIeeeGetData(U8 link);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Is the specified vendor negotiated on ONU?
///
/// \param  oui     Vendor to check
///
/// \return TRUE if vendor is negotiated on this ONU
////////////////////////////////////////////////////////////////////////////////
extern
BOOL OamIeeeIsVendorNegPerOnu(OuiVendor oui);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Is the specified vendor negotiated on a link?
///
/// \param  link    Index of the link
/// \param  oui     Vendor to check
///
/// \return TRUE if vendor is negotiated on this link
////////////////////////////////////////////////////////////////////////////////
extern
BOOL OamIeeeIsVendorNeg(LinkIndex link, OuiVendor oui);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Set the specified vendor as negotiated on a link
///
/// \param  link    Index of the link
/// \param  oui     Vendor to set
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void OamIeeeVendorNegSet(LinkIndex link, OuiVendor oui);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Clear the specified vendor as negotiated on a link
///
/// \param  link    Index of the link
/// \param  oui     Vendor to clear
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void OamIeeeVendorNegClr(LinkIndex link, OuiVendor oui);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Request an alarm audit on the specified interface
///
/// \param  link    Index of the link
/// \param  reqIf   Interface issuing the request
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void OamIeeeAlarmRequest(LinkIndex link, AlmRequestIfs reqIf);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Adjust OAM credits on port
///
/// \param port     Port to adjust credits for
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OamAdjustCredits(Port port);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Get the max size of an OAM message
///
/// \param  link    Index of the link
///
/// \return Max PDU size on this link
////////////////////////////////////////////////////////////////////////////////
extern
U16 MaxOamSpace(LinkIndex link);


////////////////////////////////////////////////////////////////////////////////
/// OamGetMsgFlags:  Gets current OAM message flags
///
/// The current state of the OAM message flags field is needed any time any
/// message goes out
///
 // Parameters:
/// \param link     Link to query
///
/// \return
/// OAM PDU flags field corresponding to current state of link
////////////////////////////////////////////////////////////////////////////////
extern
U16 OamGetMsgFlags (LinkIndex link);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Handle OAM frames
///
/// \param  link    Link of frame reception
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void OamHandle(LinkIndex link);


#endif // OamIeeeCommon_h

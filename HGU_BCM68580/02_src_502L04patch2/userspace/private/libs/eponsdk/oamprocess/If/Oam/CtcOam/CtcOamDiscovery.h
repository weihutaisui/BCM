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

#if !defined(CtcOamDiscovery_h)
#define CtcOamDiscovery_h
////////////////////////////////////////////////////////////////////////////////
/// \file CtcOamDiscovery.h
/// \brief China Telecom extended OAM discovery module
///
///
////////////////////////////////////////////////////////////////////////////////
#include "Teknovus.h"
#include "CtcOam.h"
#include "CtcOnuOam.h"


////////////////////////////////////////////////////////////////////////////////
/// CtcOamDiscoverySuccess - Further process after CTC discovery
///
/// Erase NVS first, because CTC reqire no save to NVS
/// Do CTC IGMP default setting here
///
/// Parameters:
/// None
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
extern
void CtcOamDiscoverySuccess(void);


///////////////////////////////////////////////////////////////////////////////
/// CtcOamNegotiatedVersion - Return the negotiated version
///
/// This function returns the negotiated version number of a the ONU.
///
/// \param None
///
/// \return
/// The negotiated version of the ONU
///////////////////////////////////////////////////////////////////////////////
extern
CtcOamVersion CtcOamNegotiatedVersion (void);


///////////////////////////////////////////////////////////////////////////////
/// CtcOamInfoTlvHandle:  Process CTC-specific TLV types
///
/// This function processes an incoming China Telecom specific OAM info TLV,
/// including parsing the TLV and driving the version negotiation state machine.
/// If the TLV is not an actual CTC info TLV the function will return FALSE.
///
 // Parameters:
/// \param link     Link on which info frame arrived
/// \param tlv      TLV to process
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void CtcOamInfoTlvHandle(LinkIndex link,
                         const OamCtcInfoTlvHeader BULK* tlv);


////////////////////////////////////////////////////////////////////////////////
/// CtcOamInfoTlvGet - Load a CTC info TLV
///
/// This function loads a CTC Organization Specific Information TVL into the
/// supplied buffer.  The size of the TLV is returned.
///
 // Parameters:
/// \param link Logical link ID
/// \param msg Buffer to load into
///
/// \return
/// Size of the info message
////////////////////////////////////////////////////////////////////////////////
extern
U16 CtcOamInfoTlvGet(LinkIndex link, U8 BULK *msg);


#endif

// End of File CtcOamDiscovery.h


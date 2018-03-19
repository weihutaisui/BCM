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
/// \file   OamIeeeCommon.c
/// \brief  Shared IEEE OAM implementation
///
////////////////////////////////////////////////////////////////////////////////

#include "Teknovus.h"
#include "OamIeeeCommon.h"
#include "OamUtil.h"
#include "Alarms.h"

#include "ApiResLayer.h" // for TkOnuNumTxLlids


BOOL BULK ctcIeeeOamProcessing;

OamPerLinkData BULK linkOamData[TkOnuNumTxLlids];


////////////////////////////////////////////////////////////////////////////////
/// \brief  get the linkOamData of the link
///
/// \param  link     
///
/// \return pointer to linkOamData[link]
////////////////////////////////////////////////////////////////////////////////
//extern
OamPerLinkData* OamIeeeGetData(U8 link)
{
    if (link < TkOnuNumTxLlids)
        return &linkOamData[link];
    else
        return NULL;
}


////////////////////////////////////////////////////////////////////////////////
/// \brief  Is the specified vendor negotiated on ONU?
///
/// \param  oui     Vendor to check
///
/// \return TRUE if vendor is negotiated on this ONU
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL OamIeeeIsVendorNegPerOnu(OuiVendor oui)
    {
    LinkIndex link;
    BOOL ret = FALSE;
    
    for(link = 0; link < TkOnuNumTxLlids; link++)
        {
        if(linkOamData[link].discState == OamDiscComplete)
            {
            ret = TestBitsSet(linkOamData[link].vendorNeg, 1U << oui);
            break;
            }
        }
    return ret;
    }

////////////////////////////////////////////////////////////////////////////////
/// \brief  Is the specified vendor negotiated on a link?
///
/// \param  link    Index of the link
/// \param  oui     Vendor to check
///
/// \return TRUE if vendor is negotiated on this link
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL OamIeeeIsVendorNeg(LinkIndex link, OuiVendor oui)
    {
    return TestBitsSet(linkOamData[link].vendorNeg, 1U << oui);
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Set the specified vendor as negotiated on a link
///
/// \param  link    Index of the link
/// \param  oui     Vendor to set
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamIeeeVendorNegSet(LinkIndex link, OuiVendor oui)
    {
    linkOamData[link].vendorNeg |= 1U << oui;
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Clear the specified vendor as negotiated on a link
///
/// \param  link    Index of the link
/// \param  oui     Vendor to clear
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamIeeeVendorNegClr(LinkIndex link, OuiVendor oui)
    {
    linkOamData[link].vendorNeg &= ~(1U << oui);
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Request an alarm audit on the specified interface
///
/// \param  link    Index of the link
/// \param  reqIf   Interface issuing the request
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamIeeeAlarmRequest(LinkIndex link, AlmRequestIfs reqIf)
    {
    linkOamData[link].alarmRequest |= reqIf;
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Adjust OAM credits on port
///
/// \param port     Port to adjust credits for
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
void OamAdjustCredits(Port port)
    {
    if ((port.pift == PortIfPon) &&
        (port.inst < TkOnuNumTxLlids))
        { // grant link an extra credit
        ++(linkOamData[port.inst].txCredits);
        }
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Get the max size of an OAM message
///
/// \param  link    Index of the link
///
/// \return Max PDU size on this link
////////////////////////////////////////////////////////////////////////////////
//extern
U16 MaxOamSpace(LinkIndex link)
    {
    // Useful for determining when to break a multipart OAM response
    // The maximum content space in an OAM message is the max PDU size
    // configured for a link minus space for: the ethernet header, the OAM
    // header, two OAM variable containers (one for the sequence, and one for
    // the terminator), and the CRC
    return linkOamData[link].maxPduSize - (sizeof(EthernetFrame) +
        sizeof(OamMsg) + (sizeof(OamVarContainer) * 2) + sizeof(U32));
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Gets current OAM message flags
///
/// The current state of the OAM message flags field is needed any time any
/// message goes out
///
/// \param  link    Link to query
///
/// \return OAM PDU flags field corresponding to current state of link
////////////////////////////////////////////////////////////////////////////////
//extern
U16 OamGetMsgFlags (LinkIndex link)
    {
    U16 FAST flags = 0;

    if (AlarmLinkFaultPresent (link))
        {
        flags |= OamMsgFlagLinkFault;
        }

    if (AlarmDyingGasp (link))
        {
        flags |= OamMsgFlagDyingGasp;
        }

    // We don't currently use the critical event flag for anything
    if (AlarmCriticalEvent (link))
        {
        flags |= OamMsgFlagCriticalEvent;
        }

#if OAM_VERSION < 200
    // no other message flags modification needed
#elif OAM_VERSION <= 320
    // local flags
    switch (linkOamData[link].discState)
        {
        case OamDiscPassiveWait :
        case OamDiscUnsatisfied :
        case OamDiscSatisfied :
            flags |= TkPutField (OamMsgFlagLocalDisc,
                                 OamMsgFlagDiscUnsatisfied);
            break;

        case OamDiscComplete :
        case OamLoopback :
            flags |= TkPutField (OamMsgFlagLocalDisc,
                                 OamMsgFlagDiscComplete);
            break;

        case OamDiscNoOam :
        default: // nothing in flags; failed
            break;
        }

    // remote flags
#if OAM_VERSION < 220
    flags |= TestBitsSet(linkOamData[link].lastRemoteFlags,
                         OamMsgFlagLocalDiscMsk) ?
        OamMsgFlagRemoteDiscMsk : 0;
#else // <= 320
    flags |=(linkOamData[link].lastRemoteFlags & OamMsgFlagLocalDiscMsk) <<
        (OamMsgFlagRemoteDiscSft - OamMsgFlagLocalDiscSft);
#endif
#else
#error "Unsupported OAM OAM_VERSION"
#endif

    return flags;
    } // OamGetMsgFlags


// end OamIeeeCommon.c

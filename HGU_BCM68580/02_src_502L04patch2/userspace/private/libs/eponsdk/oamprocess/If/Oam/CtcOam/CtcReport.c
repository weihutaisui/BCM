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
/// \file CtcReport.c
/// \brief China Telecom extended reporting and DBA module
/// \author Jason Armstrong
/// \date March 10, 2006
///
///
////////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include "Teknovus.h"
#include "Alarms.h"
#include "Oam.h"
#include "OamUtil.h"
#include "PonManager.h"
#include "CtcOam.h"
#include "CtcOnuUtils.h"
#include "CtcReport.h"
#include "cms_log.h"

#define OamCtcQueueSetMax    2


////////////////////////////////////////////////////////////////////////////////
/// OamCtcQueueSetCount - Number of queue sets
///
/// The 3714 hardware supports two queue sets as defined by EponBurstCap and
/// EponBurstCap2.  Setting or requesting any queue sets beyond two will lead
/// to very undesirable behavior.
////////////////////////////////////////////////////////////////////////////////
//extern
U8 oamCtcQueueSetCount;



////////////////////////////////////////////////////////////////////////////////
/// OamCtcThresholdCount - Number of threshold sets in a DBA message
///
/// The CTC get and set DBA OAM messages contain the number of queue sets minus
/// 1 queue theshold sets because the last queue set in a report is the total
/// queue depth.
////////////////////////////////////////////////////////////////////////////////
#define OamCtcThresholdCount(X) ((X) - 1)



////////////////////////////////////////////////////////////////////////////////
/// OamCtcReportQueue - Maximum number of queues per queue set
///
/// Although the OAM states that only the number of queues that are provisioned
/// need to be configured all of the queue information must be stored.  The
/// following defines the maximum number of queues that may be provisioned for
/// a link.
////////////////////////////////////////////////////////////////////////////////
#define OamCtcReportQueues      8



////////////////////////////////////////////////////////////////////////////////
/// OamCtcReportQueue - Is a report queue active
///
/// This macro returns TRUE if given a report bitmap and a queue index the queue
/// at the the given index has its bit set to one in the bitmap.
////////////////////////////////////////////////////////////////////////////////
#define OamCtcReportQueue(r,i)  (((r) & (1U << (i))) != 0)


typedef struct
    {
    U8                  report;
    U16                 threshold[8];
    } CtcDbaQueueSet;


////////////////////////////////////////////////////////////////////////////////
/// queueSet - Queue set provisioning
///
/// All provisioning of queue threshold and sets is stored in this structure.
/// The report bitmap has bits set to one as the queues corresponding to those
/// are provisioned.  Any value loaded from a default provisioning will have its
/// bit set to zero.
////////////////////////////////////////////////////////////////////////////////
static CtcDbaQueueSet BULK queueSet[TkOnuNumTxLlids][OamCtcQueueSetMax];



////////////////////////////////////////////////////////////////////////////////
/// OamCtcDbaHdrToSet - Write a variable queue set to a fixed queue set
///
/// The following function converts an OAM style variable size queue set
/// definition to a fixed size internal storage queue set.  The variable set is
/// traversed and the function returns a pointer to the end of it.
///
// Parameters:
/// \param hdr Pointer to start of variable queue set
/// \param set Pointer to start of fixed queue set
///
/// \return
/// Pointer to end of variable queue set
////////////////////////////////////////////////////////////////////////////////
static
OamCtcDbaQueueHdr BULK *OamCtcDbaHdrToSet (OamCtcDbaQueueHdr BULK *hdr,
                                           CtcDbaQueueSet BULK *set)
    {
    U8 FAST i;
    U8 FAST j;

    set->report = hdr->report;

    for ((i = 0), (j = 0); i < OamCtcReportQueues; ++i)
        {
        set->threshold[i] = (OamCtcReportQueue (hdr->report, i) ?
                             hdr->threshold[j++] : 0);
        }

    return(OamCtcDbaQueueHdr BULK *)&hdr->threshold[j];
    } // OamCtcDbaHdrToSet



////////////////////////////////////////////////////////////////////////////////
/// OamCtcDbaSetToHdr - Write a fixed queue set to a variable queue set
///
/// The following function converts a fixed size internal storage queue set to
/// an OAM style variable size queue set definition.  The variable set is
/// traversed and the function returns a pointer to the end of it.  Only queues
/// with their report bit set are copied to the variable queue set.
///
// Parameters:
/// \param set Pointer to start of fixed queue set
/// \param hdr Pointer to start of variable queue set
///
/// \return
/// Pointer to end of variable queue set
////////////////////////////////////////////////////////////////////////////////
static
OamCtcDbaQueueHdr BULK *OamCtcDbaSetToHdr (const CtcDbaQueueSet BULK *set,
                                           OamCtcDbaQueueHdr BULK *hdr)
    {
    U8 FAST i;
    U8 FAST j;

    hdr->report = set->report;
    for ((i = 0), (j = 0); i < OamCtcReportQueues; ++i)
        {
        if (OamCtcReportQueue (hdr->report, i))
            {
            hdr->threshold[j++] = set->threshold[i];
            }
        }

    return(OamCtcDbaQueueHdr BULK *)&hdr->threshold[j];
    } // OamCtcDbaSetToHdr



////////////////////////////////////////////////////////////////////////////////
/// OamCtcConfigureQueueThreshold - Configure a links threshold
///
/// This function configures the queue set threshold for a given link.
/// Configurations are taken from the software provisioning and the function
/// then returns TRUE if the configuration was a success.  It will return FALSE
/// if the configuration failed.
///
/// \todo
/// Hardware clarification is still needed to conplete this function.  For now
/// the function is just a stub that always returns TRUE.
///
// Parameters:
/// \param link Logical link ID
///
/// \return
/// TRUE if configuration of a links threshold settings was successful
////////////////////////////////////////////////////////////////////////////////
static
BOOL OamCtcConfigureQueueThreshold (LinkIndex link)
    {
    
    // TODO: if num thresholds > num priorities, change reporting mode
    PonMgrSetBurstCap(link, queueSet[link][0].threshold, OamCtcReportQueues); 
    return TRUE;
    } // OamCtcConfigureQueueThreshold



////////////////////////////////////////////////////////////////////////////////
/// OamCtcFillPduQueueSet - Fill an outgoing PDU with the current configuration
///
/// The following function fills an outgoing DBA PDU (get or set) with the
/// active provisioning on a link.  The number of queue sets returned is fixed
/// to the maximum number of queue sets even if those sets are empty.
///
// Parameters:
/// \param link Logical link ID
/// \param pdu Pointer to queue set section of an outgoing PDU
///
/// \return
/// The size in bytes of the queue sets added to the frame
////////////////////////////////////////////////////////////////////////////////
static
U16 OamCtcFillPduQueueSet (LinkIndex link, OamCtcDbaQueueHdr BULK *pdu)
    {
    U8 FAST i;
    OamCtcDbaQueueHdr BULK *hdr = pdu;

    for (i = 0; i < OamCtcThresholdCount (oamCtcQueueSetCount); ++i)
        {
        hdr = OamCtcDbaSetToHdr (&(queueSet[link][i]), hdr);
        }

    return(U16)(((U8 BULK *)hdr) - ((U8 BULK *)pdu));
    } // OamCtcFillPduQueueSet



////////////////////////////////////////////////////////////////////////////////
/// \brief  Handle a CTC DBA get request message
///
/// This function returns all of the currently provision queue sets to the OLT
/// in response to a China Telecom OAM DBA get request message.  The queue sets
/// that are returns are from the software provisioning buffer, not from
/// hardware.
///
/// \param link Logical link ID
/// \param msg Pointer to received CTC DBA PDU
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcHandleDbaGetRequest (LinkIndex link, OamCtcDbaPdu BULK *msg)
    {
    msg = (OamCtcDbaPdu BULK *)oamParser.reply.cur;

    msg->ext = OamCtcDba;
    msg->opcode = DbaGetResponse;
    msg->indexes = oamCtcQueueSetCount;

    StreamSkip(&oamParser.reply,
                   (sizeof (OamCtcDbaPdu) - sizeof (OamCtcDbaQueueHdr)) +
                   OamCtcFillPduQueueSet (link, &msg->set[0]));
    OamTransmit();
    } // OamCtcHandleDbaGetRequest



////////////////////////////////////////////////////////////////////////////////
/// \brief  Handle a CTC DBA set request message
///
/// This function handles any CTC OAM DBA set request message.  The values from
/// the OAM message are set in hardware.  If this configuration failes the ACK
/// in the return frame is set to FALSE and the original configuration is
/// restored.  All provisioned values are returned to the host as if the message
/// was a get response although it will contain the additional ACK field.  If
/// The number of queue sets provisioned is less than the maximum, the remaining
/// queue sets are left in their original state.
///
/// \todo
/// NVS support have not yet been added to this function.  Clarification is
/// required as to volatile nature of provisioned parameters.
///
/// \todo
/// The error checking and restoration has not been completed.  It will be
/// completed when the hardware configuration is finished.
///
/// \param link Logical link ID
/// \param msg  Pointer to received CTC DBA PDU
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcHandleDbaSetRequest (LinkIndex link, OamCtcDbaPdu BULK *msg)
    {
    U8 FAST i;
    OamCtcDbaPduSetResponse BULK *pdu;
    OamCtcDbaQueueHdr BULK * hdr;

    pdu = (OamCtcDbaPduSetResponse BULK *)oamParser.reply.cur;

    pdu->ext = OamCtcDba;
    pdu->opcode = DbaSetResponse;
    hdr = &msg->set[0];
    if ((msg->indexes <= OamCtcQueueSetMax) && (PonMgrPriCountGet () >= 1))
        {
        oamCtcQueueSetCount = msg->indexes;
        pdu->indexes = oamCtcQueueSetCount;
        for (i = 0; i < (msg->indexes - 1); ++i)
            {
            hdr = OamCtcDbaHdrToSet (hdr, &(queueSet[link][i]));
            }
        pdu->ack = OamCtcConfigureQueueThreshold (link);
        }
    else
        {
        hdr = OamCtcDbaHdrToSet (hdr, &(queueSet[link][0]));
        pdu->ack = FALSE;
        }

    StreamSkip(&oamParser.reply, sizeof (OamCtcDbaPduSetResponse) +
                   OamCtcFillPduQueueSet (
                       link,
                       (OamCtcDbaQueueHdr BULK *)(pdu+1)));
    OamTransmit();
    } // OamCtcHandleDbaSetRequest



////////////////////////////////////////////////////////////////////////////////
//extern
void OamCtcHandleDba (LinkIndex link, OamCtcDbaPdu BULK *msg)
    {
    cmsLog_notice("opcode = 0x%x", msg->opcode);

    switch (msg->opcode)
        {
        case DbaGetRequest:
            OamCtcHandleDbaGetRequest (link, msg);
            break;
        case DbaSetRequest:
            OamCtcHandleDbaSetRequest (link, msg);
            break;
        default:
            break;
        }
    } // OamCtcHandleDba


// End of File CtcReport.c

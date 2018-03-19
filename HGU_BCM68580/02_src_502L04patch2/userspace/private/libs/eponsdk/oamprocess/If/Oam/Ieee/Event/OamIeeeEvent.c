/*
<:copyright-BRCM:2013:proprietary:epon 

   Copyright (c) 2013 Broadcom 
   All Rights Reserved

 This program is the proprietary software of Broadcom and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").  Except as set forth in
 an Authorized License, Broadcom grants no license (express or implied), right
 to use, or waiver of any kind with respect to the Software, and Broadcom
 expressly reserves all rights in and to the Software and all intellectual
 property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,

 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use
    all reasonable efforts to protect the confidentiality thereof, and to
    use this information only in connection with your use of Broadcom
    integrated circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
    PERFORMANCE OF THE SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
    LIMITED REMEDY.
:>
*/

////////////////////////////////////////////////////////////////////////////////
/// \file OamIeeeEvent.c
/// \brief Handle IEEE OAM events notifications
///
////////////////////////////////////////////////////////////////////////////////

#include "Build.h"
#include "Alarms.h"
#include "Oam.h"
#include "OamIeeeEvent.h"
#include "OamUtil.h"
#include "OamIeee.h"
#include "OamIeeeInt.h"
#include "OamTekEvent.h"
#include "Stream.h"
#include "OamProcessInit.h"

#include "ApiResLayer.h"

extern OamHandlerCB *oamVenderHandlerHead;

#if ALARM_TO_BE_FINISHED

#define MaxGbESymbolsPer100Ms   (1000000000 / 40)
#define MaxGbEFramesPer100Ms    160000

/// number of times the given event TLV has been generated since OAM reset
U32 BULK eventCounts[OamEventErrFrameSecSummary + 1];

// running total counters -- all different sizes, so no array
MultiByte64 BULK eventRunningTotal[OamEventErrFrameSecSummary + 1];
#endif


////////////////////////////////////////////////////////////////////////////////
/// AlmStdEventErrSymbol:  Send std errored symbol period event
///
 // Parameters:
/// \param inst     Instance of condition (link index)
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void AlmStdEventErrSymbol (U8 inst)
    {
#if ALARM_TO_BE_FINISHED
    U32 FAST val;
    OamEventErrSymPeriod BULK* FAST almTlv =
        (OamEventErrSymPeriod BULK*)oamParser.reply.cur;

    almTlv->tlv.type    = OamEventErrSymbolPeriod;
    almTlv->tlv.length  = sizeof(OamEventErrSymPeriod);
    almTlv->timestamp   = Oam100MsTick();

    U64EqU32 (&almTlv->window, MaxGbESymbolsPer100Ms * 10);
    U64EqU32 (&almTlv->threshold, StatGetLinkThreshRise (inst,
                                                         StatIdLineCodeErr));
    val = StatGetLinkAlarmVal (inst, StatIdLineCodeErr);
    U64EqU32 (&almTlv->errors, val);
    U64AddEqU32 (&(eventRunningTotal[OamEventErrSymbolPeriod].u64), val);

    almTlv->runningTotal = eventRunningTotal[OamEventErrSymbolPeriod].u64;

    almTlv->eventCount = ++eventCounts[OamEventErrSymbolPeriod];

    StreamSkip(&oamParser.reply, sizeof(OamEventErrSymPeriod));
    OamTransmit();
#else
    UNUSED(inst);
#endif
    } // AlmStdEventErrSymbol



////////////////////////////////////////////////////////////////////////////////
/// AlmStdEventErrFrames:  Send std errored frames event
///
 // Parameters:
/// \param inst     Instance of condition (link index)
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void AlmStdEventErrFrames (U8 inst)
    {
#if ALARM_TO_BE_FINISHED
    U32 FAST val;
    OamEventErrFrame BULK* FAST almTlv =
        (OamEventErrFrame BULK*)oamParser.reply.cur;

    almTlv->tlv.type = OamEventErrFrameCount;
    almTlv->tlv.length = sizeof(OamEventErrFrame);

    almTlv->timestamp = Oam100MsTick();
    almTlv->window = StatGetLinkPeriod(inst, StatIdErrFrames);
    almTlv->threshold = StatGetLinkThreshRise (inst, StatIdErrFrames);

    val = StatGetLinkAlarmVal (inst, StatIdErrFrames);
    almTlv->errors = val;
    U64AddEqU32 (&(eventRunningTotal[OamEventErrFrameCount].u64), val);

    almTlv->runningTotal = eventRunningTotal[OamEventErrFrameCount].u64;

    almTlv->eventCount = ++eventCounts[OamEventErrFrameCount];

    StreamSkip(&oamParser.reply, sizeof(OamEventErrFrame));
    OamTransmit();
#else
    UNUSED(inst);
#endif
    } // AlmStdEventErrFrames



////////////////////////////////////////////////////////////////////////////////
/// AlmStdEventErrFrPeriod:  Send std errored frames period (second) event
///
 // Parameters:
/// \param inst     Instance of condition (link index)
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void AlmStdEventErrFrPeriod (U8 inst)
    {
#if ALARM_TO_BE_FINISHED
    U32 FAST val;
    OamEventErrFrPeriod BULK* FAST almTlv =
        (OamEventErrFrPeriod BULK*)oamParser.reply.cur;

    almTlv->tlv.type = OamEventErrFramePeriod;
    almTlv->tlv.length = sizeof(OamEventErrFrPeriod);

    almTlv->timestamp = Oam100MsTick();
    almTlv->window = MaxGbEFramesPer100Ms *
        StatGetLinkPeriod(inst, StatIdErrFramePeriods);
    almTlv->threshold = StatGetLinkThreshRise (inst, StatIdErrFramePeriods);

    val = StatGetLinkAlarmVal (inst, StatIdErrFramePeriods);
    almTlv->errors = val;
    U64AddEqU32 (&(eventRunningTotal[OamEventErrFramePeriod].u64), val);

    almTlv->runningTotal = eventRunningTotal[OamEventErrFramePeriod].u64;

    almTlv->eventCount = ++eventCounts[OamEventErrFramePeriod];

    StreamSkip(&oamParser.reply, sizeof(OamEventErrFrPeriod));
    OamTransmit();
#else
    UNUSED(inst);
#endif
    } // AlmStdEventErrFrPeriod



////////////////////////////////////////////////////////////////////////////////
/// AlmStdEventErrFrSummary:  Send std errored frames summary event
///
 // Parameters:
/// \param inst     Instance of condition (link index)
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void AlmStdEventErrFrSummary (U8 inst)
    {
#if ALARM_TO_BE_FINISHED
    U32 FAST val;
    OamEventErrFrSecondsSum BULK* FAST almTlv =
        (OamEventErrFrSecondsSum BULK*)oamParser.reply.cur;

    almTlv->tlv.type = OamEventErrFrameSecSummary;
    almTlv->tlv.length = sizeof(OamEventErrFrSecondsSum);


    almTlv->timestamp = Oam100MsTick();
    almTlv->window = StatGetLinkPeriod(inst, StatIdErrFrameSummary);
    almTlv->threshold = (U16)StatGetLinkThreshRise (inst,
                                StatIdErrFrameSummary);

    val = StatGetLinkAlarmVal (inst, StatIdErrFrameSummary);
    almTlv->errors = (U16)val;
    U64AddEqU32 (&(eventRunningTotal[OamEventErrFrameSecSummary].u64), val);

    almTlv->runningTotal =
            eventRunningTotal[OamEventErrFrameSecSummary].words.lsw.u32;

    almTlv->eventCount = ++eventCounts[OamEventErrFrameSecSummary];

    StreamSkip(&oamParser.reply, sizeof(OamEventErrFrSecondsSum));
    OamTransmit();
#else
    UNUSED(inst);
#endif
    } // AlmStdEventErrFrSummary



////////////////////////////////////////////////////////////////////////////////
/// AlmStdMsg:  Generate standard event message
///
 // Parameters:
/// \param inst     Instance of condition (link index)
/// \param stat     stat triggering event
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void AlmStdMsg (U8 inst, StatId stat)
    {
    switch (stat)
        {
        case StatIdLineCodeErr :
            AlmStdEventErrSymbol (inst);
            break;

        case StatIdErrFrames :
            AlmStdEventErrFrames (inst);
            break;

        case StatIdErrFramePeriods :
            AlmStdEventErrFrPeriod (inst);
            break;

        case StatIdErrFrameSummary :
            AlmStdEventErrFrSummary (inst);
            break;

        default : // do nothing
            break;
        }
    } // AlmStdMsg



////////////////////////////////////////////////////////////////////////////////
/// StdEvent:  determines if alarm should be reported via standard event TLV
///
/// See IEEE 802.3ah for definition of standard link fault events
///
 // Parameters:
/// \param cond     Alarm condition to raise
/// \param stat     Stat (if stat threshold crossing alarm)
///
/// \return
/// TRUE for standard events; FALSE for other events
////////////////////////////////////////////////////////////////////////////////
static
BOOL StdEvent (AlmCondition cond, StatId stat)
    {
    return (cond == AlmLinkStatThreshold) &&
           (
             (stat == StatIdLineCodeErr)     ||
             (stat == StatIdErrFrames)       ||
             (stat == StatIdErrFramePeriods) ||
             (stat == StatIdErrFrameSummary)
           );
    } // StdEvent


////////////////////////////////////////////////////////////////////////////////
/// AlmMsg:  Generate an alarm message
///
 // Parameters:
/// \param cond     Condition
/// \param inst     Instance of that condition
/// \param stat     Statistic involved in alarm (if any)
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void AlmMsg (AlmCondition cond, U8 inst, StatId stat)
    {
    U8 BULK * start = oamParser.reply.cur;

    VENDOR_ALARM_NOTIFY_HANDLE(cond, inst, stat);

    if (PointerDiff(oamParser.reply.cur, start) > 0)
        {
        OamTransmit();
        }
    } 


////////////////////////////////////////////////////////////////////////////////
//extern
void OamEventPrep(LinkIndex link)
    {
    OamHeaderFill(link);
    OamEventOpSeqNumFill(link);
    }


////////////////////////////////////////////////////////////////////////////////
//extern
void OamEventOpSeqNumFill(LinkIndex link)
    {
    StreamWriteU8(&oamParser.reply, OamOpEventNotification);
    StreamWriteU16(&oamParser.reply, linkOamData[link].eventSeq++);
    }


////////////////////////////////////////////////////////////////////////////////
//extern
void OamIeeeEventSend(AlmCondition cond, U8 inst, StatId stat)
    {
    if (StdEvent (cond, stat))
        {
        AlmStdMsg (inst, stat);
        }
    else
        {
        AlmMsg(cond, inst, stat);
        }
    }


// end of OamIeeeEvent.c

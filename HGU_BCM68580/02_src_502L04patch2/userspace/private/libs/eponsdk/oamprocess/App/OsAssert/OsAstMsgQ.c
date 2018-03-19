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
/// \file OsAstMsgQ.c
/// \brief Os assert message queue processing
///
/// The processing of os assert queue including push, pop, poll and init action.
///
////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include "TkOs.h"
#include "OsAstEvt.h"
#include "OsAstMsgQ.h"

#define OsAstEvtQDepth 40

U8 BULK osAstEvtQSpace[(sizeof(TkOsMsgQOverhead) + OsAstEvtQDepth) *
                           sizeof(TkOsMsg)];
TkOsMsgQ osAstEvtQ;


////////////////////////////////////////////////////////////////////////////////
/// OsAstMsgQSet:  Set os assert into message queue
///
 // Parameters:
/// \param id   Onu assert id
/// \param inst Instance of that condition
/// \param stat stat id
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void OsAstMsgQSet (OnuAssertId id, U8 inst, U8 stat) REENTRANT
    {
    U32 FAST msg;

    if (!OnuOsAssertGet(id, inst))
        {
        OnuOsAssertSet(id, inst);
        msg = MsgQRaiseSet + (((U32)id << MsgQIdSft) & MsgQIdMsk) +
              (((U32)inst << MsgQInstSft) & MsgQInstMsk) + stat;
        (void)TkOsMsgSend (osAstEvtQ, (TkOsMsg)msg);
        }
    } // OsAstMsgQSet



////////////////////////////////////////////////////////////////////////////////
/// OsAstMsgQClr:  Clear os assert from message queue
///
 // Parameters:
/// \param id   Onu assert id
/// \param inst Instance of that condition
/// \param stat stat id
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void OsAstMsgQClr (OnuAssertId id, U8 inst, U8 stat) REENTRANT
    {
    U32 FAST msg;

    if (OnuOsAssertGet(id, inst))
        {
        OnuOsAssertClr(id, inst);
        msg = (((U32)id << MsgQIdSft) & MsgQIdMsk) +
              (((U32)inst << MsgQInstSft) & MsgQInstMsk) + stat;
        (void)TkOsMsgSend (osAstEvtQ, (TkOsMsg)msg);
        }
    } // OsAstMsgQClr


////////////////////////////////////////////////////////////////////////////////
/// OsAstMsgQSpin:  Handle all events in os assert of message queue
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void OsAstMsgQSpin (void)
    {
    while(TkOsMsgAvailable (osAstEvtQ))
        {
        OsAstMsgQPoll ();
        }
    }


////////////////////////////////////////////////////////////////////////////////
/// OsAstMsgQFlush:  Flush the content of the osAstEvtQ
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void OsAstMsgQFlush (void)
    {
    U32 FAST msg;
    do
    	{
		msg = (U32)TkOsMsgReceive (osAstEvtQ);
		}
	while(msg != 0);
    } // OsAstMsgQFlush


////////////////////////////////////////////////////////////////////////////////
/// OsAstMsgQPoll:  Poll os assert of message queue
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void OsAstMsgQPoll (void)
    {
    U32 FAST msg;
    TkOsAstRec BULK rec;

    msg = (U32)TkOsMsgReceive (osAstEvtQ);
    if (msg != 0)
        {
        rec.raise = TestBitsSet(msg, MsgQRaiseSet);
        rec.id    = (msg & MsgQIdMsk) >> MsgQIdSft;
        rec.inst  = (msg & MsgQInstMsk) >> MsgQInstSft;
        rec.stat  = (msg & MsgQStatMsk) >> MsgQStatSft;

        if ((rec.id >= OsAstStatBase) && (rec.id < OsAstStatEnd))
            {
            if (rec.inst < OsAstStatPortInstBase)
                {
                rec.id = OsAstAlmEponStatThreshold;
                }
            else if (rec.inst < OsAstStatLinkInstBase)
                {
                rec.id = OsAstAlmPortStatThreshold;
                }
            else
                {
                rec.id = OsAstAlmLinkStatThreshold;
                }
            }

        OsAssertEvent(rec);
        }
    } // OsAstMsgQPoll



////////////////////////////////////////////////////////////////////////////////
/// OsAstMsgQInit:  Initialization for os assert message queue
///
 // Parameters:
/// None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void OsAstMsgQInit (void)
    {
    osAstEvtQ = TkOsMsgQueueCreate (osAstEvtQSpace, OsAstEvtQDepth, NULL);
    }//OsAstMsgQInit


// end OsAstMsgQ.c


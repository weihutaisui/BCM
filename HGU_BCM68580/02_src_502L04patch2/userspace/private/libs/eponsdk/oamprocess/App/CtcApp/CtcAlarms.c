/*
*<:copyright-BRCM:2013:proprietary:epon 
*
*   Copyright (c) 2013 Broadcom 
*   All Rights Reserved
*
* This program is the proprietary software of Broadcom and/or its
* licensors, and may only be used, duplicated, modified or distributed pursuant
* to the terms and conditions of a separate, written license agreement executed
* between you and Broadcom (an "Authorized License").  Except as set forth in
* an Authorized License, Broadcom grants no license (express or implied), right
* to use, or waiver of any kind with respect to the Software, and Broadcom
* expressly reserves all rights in and to the Software and all intellectual
* property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
* NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
* BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
*
* Except as expressly set forth in the Authorized License,
*
* 1. This program, including its structure, sequence and organization,
*    constitutes the valuable trade secrets of Broadcom, and you shall use
*    all reasonable efforts to protect the confidentiality thereof, and to
*    use this information only in connection with your use of Broadcom
*    integrated circuit products.
*
* 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
*    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
*    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
*    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
*    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
*    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
*    PERFORMANCE OF THE SOFTWARE.
*
* 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
*    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
*    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
*    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
*    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
*    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
*    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
*    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
*    LIMITED REMEDY.
:>
*/
////////////////////////////////////////////////////////////////////////////////
/// \file CtcAlarms.c
/// \brief Ctc Alarm conditions and reporting
/// \author Chen Lingyong
/// \date Nov 2, 2010
///
/// The Ctc Alarms code polls Ctc alarm to decide whether to raise/clear Ctc
/// alarm, it will first push the Ctc alarm to message queue, then send the
/// Ctc alarm one by one by pop the Ctc Alarm from message queue.
///
////////////////////////////////////////////////////////////////////////////////

#include <string.h>

#include "Build.h"
#include "CtcOnuOam.h"
#include "OamIeeeEvent.h"
#include "EponDevInfo.h"
#include "CtcAlarms.h"
#include "Alarms.h"

#include "cms_log.h"
#include "OamOnu.h"

////////////////////////////////////////////////////////////////////////////////
// Ctc Alarm Queuing functions
////////////////////////////////////////////////////////////////////////////////
#define CtcAlmMsgQSize              32
#define CtcAlmMsgQMsk               0x1F
#define CtcAlmMsgQNext(idx)         (((idx)+1)&CtcAlmMsgQMsk)


////////////////////////////////////////////////////////////////////////////////
// Ctc Alarm Ont/PonIf/Eth Index
////////////////////////////////////////////////////////////////////////////////
#define CtcAlmOntIndex(alm)         ((alm) - OamCtcOnuAlarmBase)
#define CtcAlmPonIfIndex(alm)       ((alm) - OamCtcPonIfAlarmBase)
#define CtcAlmEthIndex(alm)         ((alm) - OamCtcPortAlarmBase)


////////////////////////////////////////////////////////////////////////////////
// Ctc Alarm number that raised by teknovus
////////////////////////////////////////////////////////////////////////////////
#define CtcAlmAstNums               2


////////////////////////////////////////////////////////////////////////////////
// Ctc Alarm Message Queue
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    CtcAlmCont  queue[CtcAlmMsgQSize];
    U8          front;
    U8          back;
    } PACK CtcAlmMsgQ;

////////////////////////////////////////////////////////////////////////////////
// Ctc Alarm assert by teknovus
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    OamCtcAlarmId ctcAlm;
    AlmCondition  tekAlm;
    U8            nums;
    } PACK CtcAlmAst;


//##############################################################################
//                     Ctc Alarm Management module
//##############################################################################

static
OamCtcAlarmId const CODE OamAlarmBase[] =
    {
    OamCtcOnuAlarmBase,
    OamCtcPonIfAlarmBase,
    OamCtcCardAlarmBase,
    OamCtcPortAlarmBase,
    OamCtcPotsAlarmBase,
    OamCtcE1AlarmBase
    };

static
CtcAlmAst const CODE ctcAlmAst[CtcAlmAstNums] =
    {
        { OamCtcAttrEthPortLos, AlmPortLinkDown, MAX_UNI_PORTS},
        { OamCtcAttrEthPortAutoNegFailure, AlmPortAutoNegFailure, MAX_UNI_PORTS}
    };


static void CtcAlmClrState (OamCtcAlarmId almId, U8 inst);
static void CtcAlmPoll (void);

////////////////////////////////////////////////////////////////////////////////
/// CtcAlarmAdminState - CTC alarm admin state information
///
/// This varible use the bitmap to show which alarm is allowed to report
////////////////////////////////////////////////////////////////////////////////
static
CtcAlarmAdminState   ctcAlarmAdminState;


////////////////////////////////////////////////////////////////////////////////
/// AlarmAdminTypeGet - Get Alarm Admin Type
///
/// This function gets alarm admin type from alarm id
///
/// \param AlmId        alarm Id
///
/// \return:
/// Onu admin type
////////////////////////////////////////////////////////////////////////////////
static
OamCtcAlarmType AlarmAdminTypeGet(OamCtcAlarmId almId)
    {
    return (OamCtcAlarmType)(almId/CtcAlarmBlockLen);
    } // AlarmAdminTypeGet


////////////////////////////////////////////////////////////////////////////////
/// AlarmAdminStateMaskGet - Get Alarm Admin State Mask
///
/// This function gets alarm admin state bit mask from alarm id
///
/// Parameters:
/// \param AlmId        alarm Id
///
/// \return:
///           Onu admin state bit mask
////////////////////////////////////////////////////////////////////////////////
static
U64 AlarmAdminStateMaskGet(OamCtcAlarmId almId)
    {
    OamCtcAlarmType BULK almType;

    almType = AlarmAdminTypeGet(almId);
    return ((U64)CtcAlarmBase<<(almId - OamAlarmBase[almType]));
    }//AlarmAdminStateMaskGet


////////////////////////////////////////////////////////////////////////////////
/// AlarmAdminStateTypeGet- Get Alarm Admin State Type
///
/// This function gets alarm admin state type from alarm id
///
/// \param port     port number
/// \param AlmId    alarm Id
///
/// \return:
/// Onu admin state
////////////////////////////////////////////////////////////////////////////////
static
U64 BULK * AlarmAdminStateTypeGet(TkOnuEthPort port, OamCtcAlarmId almId)
    {
    OamCtcAlarmType BULK almType;

    almType = AlarmAdminTypeGet(almId);

    switch(almType)
        {
        case OamCtcAlarmOnu:
            return (U64 *)&(ctcAlarmAdminState.onu);
        case OamCtcAlarmPonIf:
            return (U64 *)&(ctcAlarmAdminState.ponIf[port]);
        case OamCtcAlarmCard:
            return (U64 *)&(ctcAlarmAdminState.card);
        case OamCtcAlarmEth:
            return (U64 *)&(ctcAlarmAdminState.eth[port]);
        case OamCtcAlarmPots:
            return (U64 *)&(ctcAlarmAdminState.pots);
        case OamCtcAlarmE1:
            return (U64 *)&(ctcAlarmAdminState.e1);
        default:
            return NULL;
        }
    } // AlarmAdminStateTypeGet


////////////////////////////////////////////////////////////////////////////////
/// CtcAlmIdValid - Check if the alarm ID is valid for this ONU
///
/// \param obj        the CTC object type
/// \param alarmId    alarm Id
///
/// \return
/// TRUE if this alamId is valid or FALSE
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL CtcAlmIdValid(OamCtcObjType obj, U16  alarmId)
    {
    // If this is a valid ONU alarm ID
    if (obj == OamCtcObjOnu)
        {
        if(alarmId <= OamCtcOnuAlarmEnd)
            {
            return TRUE;
            }
        }
    // If this is a valid UNI port alarm ID
    else if (obj == OamCtcObjPort)
        {
        if((alarmId >= OamCtcPortAlarmBase) &&
           (alarmId <= OamCtcPortAlarmEnd30))
            {
            return TRUE;
            }
        }
    // If this is a valid PON interface alarm ID
    else if (obj == OamCtcObjPon)
        {
        if((alarmId >= OamCtcPonIfAlarmBase) &&
           (alarmId <= OamCtcPonIfAlarmEnd30))
            {
            return TRUE;
            }
        }
    else
        {
        //for lint
        }
    // It's a invalid alarm ID
    return FALSE;
    } // CtcAlmIdValid


////////////////////////////////////////////////////////////////////////////////
/// CtcAlmAdminStateGet - Get Alarm Admin State
///
///
/// This function gets alarm admin state from alarm id
///
/// Parameters:
/// \param port     port number
/// \param AlmId        alarm Id
///
/// \return:
///           Onu admin state
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL CtcAlmAdminStateGet(TkOnuEthPort port, OamCtcAlarmId almId)
    {
    U64 BULK bitMask;
    U64 BULK *almState;

    bitMask = AlarmAdminStateMaskGet (almId);
    almState = AlarmAdminStateTypeGet (port, almId);
    if (almState != NULL)
        {
        return TestBitsSet(*almState, bitMask);
        }

    return FALSE;
    } // CtcAlmAdminStateGet


////////////////////////////////////////////////////////////////////////////////
/// CtcAlmAdminStateSet - Set Alarm Admin State
///
/// This function sets alarm admin state from alarm id
///
/// \param  port     TkOnuEthPort
/// \param  almId    TkOnuEthPort
/// \param  config   TkOnuEthPort
///
/// \return:
/// TRUE if success, otherwise FALSE
///////////////////////////////////////////////////////////////////////////////
//extern
BOOL CtcAlmAdminStateSet(TkOnuEthPort port,OamCtcAlarmId almId,U32 config)
    {
    U64 BULK bitMask;
    U64 BULK *almState;

    bitMask = AlarmAdminStateMaskGet (almId);
    almState = AlarmAdminStateTypeGet (port, almId);
    if (almState != NULL)
        {
        if (config == OamCtcActionDisable)
            {
            *almState &= ~bitMask;
            return TRUE;
            }
        else if (config == OamCtcActionEnable)
            {
            if (!CtcAlmAdminStateGet(port, almId))
                {
                CtcAlmClrState(almId, port);
                }
            *almState |= bitMask;
            return TRUE;
            }
        else
            {
            //for lint
            }
        }

    return FALSE;
    } // CtcAlmAdminStateSet


//##############################################################################
//                     Ctc Alarm raise and clear machanism
//##############################################################################

////////////////////////////////////////////////////////////////////////////////
// Static Varible
////////////////////////////////////////////////////////////////////////////////
static CtcAlmMsgQ ctcAlmMsgQ;
static CtcAlmInfo ctcAlmInfo;


////////////////////////////////////////////////////////////////////////////////
/// CtcAlmMakeCont: Make raise, alarm Id, instance and value to alarm Context.
///
/// This function makes raise, alarm Id, instance and value to alarm Context.
///
/// Parameters:
/// \param cont alarm content to add to queue
/// \param raised raised flag
/// \param almId Ctc alarm Id
/// \param inst Instance of that alarm Id
/// \param infoLen The length of the info
/// \param value current alarm Id value
///
/// \return
///     None
////////////////////////////////////////////////////////////////////////////////
static
void CtcAlmMakeCont (CtcAlmCont BULK* almCont,
                BOOL raised,
                OamCtcAlarmId almId,
                U8 inst,
                AlmInfoLen infoLen,
                U64 value )
    {
    almCont->raised = raised;
    almCont->almId  = almId;
    almCont->inst   = inst;
    almCont->infoLen = infoLen;
    almCont->value  = value;
    } // CtcAlmMakeCont


////////////////////////////////////////////////////////////////////////////////
/// CtcAlmPushMsgQ: Add another alarm content to an message queue
///
/// This function will attempt to add an alarm content to an message queue.
/// If the queue is full the function will return FALSE.  It will return TRUE
/// if the entry was added.
///
/// Parameters:
/// \param cont alarm content to add to queue
///
/// \return
/// TRUE if operation was successful
////////////////////////////////////////////////////////////////////////////////
static
BOOL CtcAlmPushMsgQ (const CtcAlmCont BULK *cont)
    {
    if (CtcAlmMsgQNext(ctcAlmMsgQ.back) != ctcAlmMsgQ.front)
        {
        memcpy (&ctcAlmMsgQ.queue[ctcAlmMsgQ.back],
                cont,
                sizeof (CtcAlmCont));
        ctcAlmMsgQ.back = CtcAlmMsgQNext(ctcAlmMsgQ.back);

        return TRUE;
        }
    else
        {
        cmsLog_notice("failed to push msg to queue");    
        }

    return FALSE;
    } // CtcAlmPushMsgQ



////////////////////////////////////////////////////////////////////////////////
/// CtcAlmPopMsgQ: Remove an entry from an message queue
///
/// This function dequeues an alarm content from the message queue into the
/// supplied container.  If there was an alarm content pending the function
/// will return TRUE. FALSE will be returned if the message queue was empty.
///
/// Parameters:
/// \param cont alarm content to delete from queue
///
/// \return
/// TRUE if operation was successful
////////////////////////////////////////////////////////////////////////////////
static
BOOL CtcAlmPopMsgQ (CtcAlmCont *cont)
    {
    if (ctcAlmMsgQ.front != ctcAlmMsgQ.back)
        {
        memcpy (cont,
                &ctcAlmMsgQ.queue[ctcAlmMsgQ.front],
                sizeof (CtcAlmCont));
        ctcAlmMsgQ.front = CtcAlmMsgQNext(ctcAlmMsgQ.front);

        return TRUE;
        }

    return FALSE;
    } // CtcAlmPopMsgQ



////////////////////////////////////////////////////////////////////////////////
/// CtcAlmGetItem- Get Alarm Item Address
///
///
/// This function gets alarm item address
///
/// Parameters:
/// \param almId alarm Id
/// \param inst Instance of that alarm Id
///
/// \return:
///     Ctc Alarm Item Address
////////////////////////////////////////////////////////////////////////////////
static
CtcAlmState BULK * CtcAlmGetItem(OamCtcAlarmId almId, U8 inst)
    {
    OamCtcAlarmType BULK almType = (OamCtcAlarmType)(almId/0x100);

    switch(almType)
        {
        case OamCtcAlarmOnu:
            return &(ctcAlmInfo.onu[CtcAlmOntIndex(almId)]);

        case OamCtcAlarmPonIf:
            return &(ctcAlmInfo.ponIf[CtcAlmPonIfIndex(almId)]);

        case OamCtcAlarmEth:
            return &(ctcAlmInfo.eth[inst][CtcAlmEthIndex(almId)]);

        default:
            return NULL;
        }
    }//CtcAlmGetItem


////////////////////////////////////////////////////////////////////////////////
/// CtcAlmClrState:  clear ctc alarm condition state
///
 // Parameters:
/// \param almId Ctc alarm Id
/// \param inst Instance of that alarm Id
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void CtcAlmClrState (OamCtcAlarmId almId, U8 inst)
    {
    CtcAlmState BULK *state = CtcAlmGetItem(almId, inst);

    state->raised = FALSE;
    }//CtcAlmClrState



////////////////////////////////////////////////////////////////////////////////
/// CtcAlmSetCond:  indicates ctc alarm condition has occurred
///
 // Parameters:
/// \param almId Ctc alarm Id
/// \param inst Instance of that alarm Id
/// \param infoLen The length of the info
/// \param value current alarm Id value
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void CtcAlmSetCond (OamCtcAlarmId almId, U8 inst,
    AlmInfoLen infoLen, U64 value) REENTRANT
    {
    CtcAlmCont  almCont;
    CtcAlmState *state = CtcAlmGetItem(almId, inst);

    if (!state->raised)
        {
        cmsLog_notice("push msg: almId[0x%x] raised", almId);    
        state->raised = TRUE;
        CtcAlmMakeCont(&almCont, TRUE, almId, inst, infoLen, value);
        CtcAlmPushMsgQ(&almCont);
        }

    }//CtcAlmSetCond



////////////////////////////////////////////////////////////////////////////////
/// CtcAlmClrCond:  indicates ctc alarm condition has gone away
///
 // Parameters:
/// \param almId Ctc alarm Id
/// \param inst Instance of that alarm Id
/// \param infoLen The length of the info
/// \param value current alarm Id value
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void CtcAlmClrCond (OamCtcAlarmId almId, U8 inst,
    AlmInfoLen infoLen, U64 value) REENTRANT
    {
    CtcAlmCont almCont;
    CtcAlmState *state = CtcAlmGetItem(almId, inst);

    if (state->raised)
        {
        cmsLog_notice("push msg: almId[%d] cleared", almId);      
        state->raised = FALSE;
        CtcAlmMakeCont(&almCont, FALSE, almId, inst, infoLen, value);
        CtcAlmPushMsgQ(&almCont);
        }
    }//CtcAlmClrCond


////////////////////////////////////////////////////////////////////////////////
/// CtcAlmTlvAdd:  Append a CTC alarm TLV in the reply buffer
///
 // Parameters:
/// \param cont Ctc Alarm container
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void CtcAlmTlvAdd(const CtcAlmCont BULK * cont)
    {
    OamCtcAlarmType BULK almType;
    OamCtcEventTlvHead BULK* FAST almTlv;
    almTlv = (OamCtcEventTlvHead BULK*)oamParser.reply.cur;
    almTlv->ext.tlv.type   = OamEventErrVendor;
    almTlv->ext.tlv.length = sizeof(OamCtcEventTlvHead);
    memcpy (almTlv->ext.oui.byte, &CtcOui, sizeof(IeeeOui));
    almType = (OamCtcAlarmType)AlarmAdminTypeGet(cont->almId);
    switch(almType)
        {
        case OamCtcAlarmOnu:
            almTlv->objType = OAM_HTONS(OamCtcObjOnu);
            almTlv->instNum.uNum = OAM_HTONL(OamCtcOnuInstNum);
            break;
        case OamCtcAlarmPonIf:
            almTlv->objType = OAM_HTONS(OamCtcObjPon);
            almTlv->instNum.uNum = OAM_HTONL(cont->inst);
            break;
        case OamCtcAlarmCard:
            almTlv->objType = OAM_HTONS(OamCtcObjCard);
            almTlv->instNum.uNum = 0x0;
            break;
        case OamCtcAlarmEth:
        case OamCtcAlarmPots:
        case OamCtcAlarmE1:
            almTlv->objType = OAM_HTONS(OamCtcObjPort);
            if (almType == OamCtcAlarmEth)
                {
                almTlv->instNum.sNum.portType = OamCtcPortEth;
                }
            else if (almType == OamCtcAlarmPots)
                {
                almTlv->instNum.sNum.portType = OamCtcPortVoIP;
                }
            else
                {
                almTlv->instNum.sNum.portType = OamCtcPortE1;
                }
            almTlv->instNum.sNum.slotNum  = 0x0;
            almTlv->instNum.sNum.portNum  = OAM_HTONS(cont->inst + 1);
            break;
        default:
            break;
        }
    almTlv->almID   = OAM_HTONS(cont->almId);
    almTlv->timeStamp = 0x0;
    //(EASW-10968)According to CTC spec, AlarmState is "Used to indicate event status.
    //0x00 indicates reporting alarm envent,0x01 indicates clearing reported alarm event.
    //In addition, cuc spec defines 0x00 fro clear and 0x01 for raise.
    almTlv->state = 
        (U8)(OamIeeeIsVendorNeg(0, OuiCtc)? (!cont->raised) : cont->raised);
    StreamSkip(&oamParser.reply, almTlv->ext.tlv.length);
    almTlv->ext.tlv.length += cont->infoLen;
    switch(cont->infoLen)
        {
        case AlmInfoLen4:
            StreamWriteU32(&oamParser.reply,
                (U32)(cont->value & 0xFFFFFFFFUL));
            break;

        case AlmInfoLen8:
            StreamWriteU64(&oamParser.reply, cont->value);
            break;

        default:
            break;
        }
    } // CtcAlmTlvAdd


////////////////////////////////////////////////////////////////////////////////
/// CtcAlmCondCreate:  Create a CTC alarm container from standard alarm info
///
 // Parameters:
/// \param almCont Ctc Alarm container
/// \param cond    Standard alarm condition
/// \param inst    Alarm instance
/// \param stat    Stat id
///
/// \return
/// TRUE if successful, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL CtcAlmCondCreate(CtcAlmCont BULK * almCont,
                      AlmCondition cond, U8 inst, StatId stat)
    {
    BOOL FAST ret = TRUE;
    UNUSED(stat);
    switch(cond)
        {
        case AlmOntPowerLoss:
            almCont->raised  = AlarmGetCondition(AlmOntPowerLoss, inst);
            almCont->almId   = OamCtcAttrPowerAlarm;
            almCont->inst    = inst;
            almCont->infoLen = AlmInfoLen4;
            almCont->value   = (U64)OamCtcOnuPowerAlarmPowerDown;
            break;
        default:
            ret = FALSE;
            break;
        }
    return ret;
    } // CtcAlmCondCreate


////////////////////////////////////////////////////////////////////////////////
//extern
BOOL CtcAlmMsg(const CtcAlmCont BULK * cont)
    {
    if (CtcAlmAdminStateGet((TkOnuEthPort)cont->inst, cont->almId))
        {
        cmsLog_notice("prepare to send alarm msg");    
        OamEventPrep(0);
        CtcAlmTlvAdd(cont);
        OamTransmit();
        return TRUE;
        }
    else
        {
        cmsLog_notice("ignore alarm msg");    
        }

    return FALSE;
    }//CtcAlmMsg

////////////////////////////////////////////////////////////////////////////////
//extern
void CtcAlmPonIfSwitchNotify(void)
    {
    CtcAlmCont BULK almCont;
    almCont.raised   = TRUE;
    almCont.almId    = OamCtcAttrPonIfSwitch;
    almCont.inst    = 0;
    almCont.infoLen = AlmInfoLen4;
    almCont.value   = (U64)OamCtcPonIfSwichAlarmSigLos;
    (void)CtcAlmMsg(&almCont);
    } // CtcAlmPonIfSwitchNotify


////////////////////////////////////////////////////////////////////////////////
/// CtcPerfMonAlmNotify - Notify Performance monitoring alarm
///
/// Parameters:
/// \param almId     alarm id
/// \param pAlmThd   pointer to alarm thershold
/// \param current   pointer to current value from performance monitoring
/// \param port      Ethernet port number
///
/// \return: None
////////////////////////////////////////////////////////////////////////////////
void CtcPerfMonAlmNotify
    (
     CtcAlmMonThd * pAlmThd,
     const U64 * current,
     OamCtcAlarmId almId,
     TkOnuEthPort port
    )
    {
    U32 high = (U32)((*current >> 32)& 0xFFFFFFFFUL);
    U32 low  = (U32)(*current & 0xFFFFFFFFUL);
    
    //alarm stats enable . only send clear alarm.
    if (CtcAlmAdminStateGet(port, almId))
        {
        // Clear the alarm if the current value less than the threshold.
        if ((low <= pAlmThd->CtcAlmClear) && (0 == high))
            {
            if (pAlmThd->CtcAlmFlag == TRUE)
                {
                CtcAlmClrCond(almId, port, AlmInfoLen8, *current);
                }
            pAlmThd->CtcAlmFlag = FALSE;
            }
        else
            {
            if ((low >= pAlmThd->CtcAlmRaise) || (high > 0))
                {
                // Raise the alarm if the current value exceeds the threshold.
                if (pAlmThd->CtcAlmFlag == FALSE)
                    {
                    CtcAlmSetCond(almId, port, AlmInfoLen8, *current);
                    }
                pAlmThd->CtcAlmFlag = TRUE;
                }


            }
        }

    return;
    }


CtcOnuTempThdUion  CtcOnuTemp = { {0x7FFFFFFF, 0x80000000, 0x7FFFFFFF, 0x80000000} };
///////////////////////////////////////////////////////////////////////////////
/// SetOnuTempThd - Set Onu temperature threshold
///                        
/// 
/// This function: Set Onu temperature threshold from alarm Id
/// 
/// Parameters: U8 BULK * val, U16 almId
/// 
/// \return:
///           Operation result
///////////////////////////////////////////////////////////////////////////////
//extern
BOOL SetOnuTempThd(U16 armId, U32  raise,U32 clear)
    { 
    switch(armId)
        {
        case OamCtcAttrOnuTempHighAlarm:
        case OamCtcAttrOnuTempLowAlarm:
            CtcOnuTemp.word[armId -OamCtcAttrOnuTempHighAlarm] = raise;
            CtcOnuTemp.word[2 + armId -OamCtcAttrOnuTempHighAlarm] = clear;
            break;
        default:
            return FALSE;
        }
    return TRUE;
    }


////////////////////////////////////////////////////////////////////////////////
/// GetOnuTempThd - Get Onu temperature threshold value 
///                        
/// 
/// This function gets Onu temperature threshold value from alarm Id
/// 
/// Parameters: 
///                  OamCtcTlvAlarmThd BULK* src
///                  U16 almId
/// 
/// \return:
///           The threshold value 
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL GetOnuTempThd( U16 armId, U32* raise,U32*clear)
    { 
    switch(armId)
        {
        case OamCtcAttrOnuTempHighAlarm:
        case OamCtcAttrOnuTempLowAlarm:
            *raise = CtcOnuTemp.word[armId -OamCtcAttrOnuTempHighAlarm];
            *clear = CtcOnuTemp.word[2 + armId -OamCtcAttrOnuTempHighAlarm] ;
            break;
        default:
            return FALSE;
        }
    return TRUE;
    }



////////////////////////////////////////////////////////////////////////////////
/// PollCtcOnuTemp - poll the  Onu temperature
/// 
/// This function polls the CTC Onu temperature by the optical module
/// 
/// Parameters: None
/// 
/// \return: None
////////////////////////////////////////////////////////////////////////////////
//extern
void PollCtcOnuTemp(void)
    {
    U8	 i;
    S16  val;
    S32  val2;
    U16 almId;

    optical_temp((U16 *)&val);
   

    if((val & 0x8000) != 0)
        {
        val2 = (S32)((U32)val | 0xFFFF0000UL);
        }
    else
        {
        val2 = (S32)((U32)val & 0x0000FFFFUL);
        }

    for( i=0; i<2; i++)
        { 
        almId = OamCtcAttrOnuTempHighAlarm + i;

        if((val2 > (S32)CtcOnuTemp.word[i] && (i == 0)) ||
            (val2 < (S32)CtcOnuTemp.word[i] && (i == 1)))
            {
            CtcAlmSetCond(almId,0, AlmInfoLen4, val2);
            }
        
        if((val2 < (S32)CtcOnuTemp.word[i + 2] && (i == 0)) ||
            (val2 > (S32)CtcOnuTemp.word[i + 2] && (i == 1)))
            { 
            CtcAlmClrCond(almId,0, AlmInfoLen4, val2);
            }
        }
    }


////////////////////////////////////////////////////////////////////////////////
/// CtcAlmAstPoll:  Poll CTC alarms that asserted by teknovus
///
 // Parameters:
/// \param rec  Tk os assert record
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void CtcAlmAstPoll (void)
    {
    U8 FAST inst;
    static U8 FAST almInx = 0;

    for (inst = 0; inst < ctcAlmAst[almInx].nums; inst++)
        {
        if (AlarmGetCondition(ctcAlmAst[almInx].tekAlm, inst))
            {
            CtcAlmSetCond(ctcAlmAst[almInx].ctcAlm, inst, AlmInfoLen0, 0ULL);
            }
        else
            {
            CtcAlmClrCond(ctcAlmAst[almInx].ctcAlm, inst, AlmInfoLen0, 0ULL);
            }
        }

    if (++almInx >= CtcAlmAstNums)
        {
        almInx = 0;
        }
    }///CtcAlmAstPoll


////////////////////////////////////////////////////////////////////////////////
/// CtcAlmPortCongestPoll:  Poll CTC port congestion alarms
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void CtcAlmPortCongestPoll (void)
    {
    }//CtcAlmPortCongestPoll


////////////////////////////////////////////////////////////////////////////////
/// CtcAlm1sTimer:  CTC alarm 1s timer
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void CtcAlm1sTimer (void)
    {
    CtcAlmPortCongestPoll();
    CtcAlmPoll();
    }//CtcAlm1sTimer


////////////////////////////////////////////////////////////////////////////////
/// CtcAlmPoll-Poll Ctc Alarm, Message Queue and Send Ctc Alarm
///
/// This function polls the Ctc alarm, message queue and send the Ctc alarm.
/// The polling must run in less than 500ms timer.
///
/// Parameters:
/// \param None
///
/// \return:
///     None
////////////////////////////////////////////////////////////////////////////////
//extern
static void CtcAlmPoll (void)
    {
    CtcAlmCont cont;

    CtcAlmAstPoll();
    
    PollCtcOnuTemp();

    if (CtcAlmPopMsgQ(&cont))
        {
        cmsLog_notice("pop msg from queue: alm[%d]=%s", 
                       cont.almId, cont.raised?"raised":"cleared");    
        CtcAlmMsg(&cont);
        }
    }//CtcAlmPoll


//end CtcAlarms.c


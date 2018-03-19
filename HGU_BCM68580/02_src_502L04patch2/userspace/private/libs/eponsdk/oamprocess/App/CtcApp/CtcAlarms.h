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

#if !defined(CtcAlarms_h)
#define CtcAlarms_h
////////////////////////////////////////////////////////////////////////////////
/// \file CtcAlarms.h
/// \brief Ctc Alarm conditions and reporting
/// \author Chen Lingyong
/// \date Nov 2, 2010
///
/// The Ctc Alarms code polls Ctc alarm to decide whether to raise/clear Ctc
/// alarm, it will first push the Ctc alarm to message queue, then send the
/// Ctc alarm one by one by pop the Ctc Alarm from message queue.
///
////////////////////////////////////////////////////////////////////////////////

#include "Teknovus.h"
#include "PortCapability.h"
#include "Alarms.h"
#include "CtcOam.h"
#include "OsAstMsgQ.h"

#if defined(__cplusplus)
extern "C" {
#endif


////////////////////////////////////////////////////////////////////////////////
// This spefic value presents the Ctc alarm don't contain the alam info
////////////////////////////////////////////////////////////////////////////////
#define OamCtcAlmNoInfo     0x7FFFFFFFUL
#define CtcPonPerfAlmNum 32
#define CtcEthPerfAlmNum 34


////////////////////////////////////////////////////////////////////////////////
// Ctc Alarm State
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    BOOL                raised;
    } PACK CtcAlmState;


////////////////////////////////////////////////////////////////////////////////
// Ctc Alarm Info
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    CtcAlmState         onu[OamCtcOnuAlarmNums];
    CtcAlmState         ponIf[OamCtcPonIfAlarmNums];
    CtcAlmState         eth[MAX_UNI_PORTS][OamCtcPortAlarmNums];
    } PACK CtcAlmInfo;


////////////////////////////////////////////////////////////////////////////////
// Ctc Alarm Content
////////////////////////////////////////////////////////////////////////////////
typedef enum
    {
    AlmInfoLen0         = 0,
    AlmInfoLen4         = 4,
    AlmInfoLen8         = 8
    } PACK AlmInfoLen;

typedef struct
    {
    BOOL                raised;
    OamCtcAlarmId       almId;
    U8                  inst;
    AlmInfoLen          infoLen;
    U64                 value;
    } PACK CtcAlmCont;

typedef struct
    {
    // alarm raise threshold
    U32        	CtcAlmRaise;
    U32        	CtcAlmClear;
	//alarm report mode
	BOOL        CtcAlmFlag;
    } PACK CtcAlmMonThd;

////////////////////////////////////////////////////////////////////////////////
/// CtcOnuTempThd - Ctc Onu temperature threshold
/// 
/// 
/// 
////////////////////////////////////////////////////////////////////////////////
typedef struct
	{
	// alarm raise threshold
	U32					onuTempHighRaise;
	U32					onuTempLowRaise;
	// alarm clear threshold
	U32					onuTempHighClear;
	U32					onuTempLowClear;			
	} PACK CtcOnuTempThd;

////////////////////////////////////////////////////////////////////////////////
/// CtcOnuTempThdUion - Ctc Onu temperature threshold uion
/// 
/// 
/// 
////////////////////////////////////////////////////////////////////////////////
typedef union
	{
	CtcOnuTempThd 	Thd;
	U32			   	word[4];
	} PACK CtcOnuTempThdUion;

#define CtcAlarmBlockLen    0x100

////////////////////////////////////////////////////////////////////////////////
/// OamCtcAlarmAdminState - the CTC alarm admin state
///
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    U64                 onu;
    U64                 ponIf[CurOnuNumPonPorts];
    U64                 card;
    U64                 eth[CurOnuNumEthPorts];
    U64                 pots;
    U64                 e1;
    } CtcAlarmAdminState;


////////////////////////////////////////////////////////////////////////////////
/// CtcAlmIdValid - Check if the alarm ID is valid for this ONU
///
/// \param obj        the CTC object type
/// \param alarmId    alarm Id
///
/// \return
/// TRUE if this alamId is valid or FALSE
////////////////////////////////////////////////////////////////////////////////
extern
BOOL CtcAlmIdValid(OamCtcObjType obj, U16  alarmId);


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
extern
BOOL CtcAlmAdminStateGet(TkOnuEthPort port, OamCtcAlarmId almId);


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
extern
BOOL CtcAlmAdminStateSet(TkOnuEthPort port,OamCtcAlarmId almId,U32 config);


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
extern
void CtcAlmSetCond (OamCtcAlarmId almId, U8 inst, 
    AlmInfoLen infoLen, U64 value) REENTRANT;


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
extern
void CtcAlmClrCond (OamCtcAlarmId almId, U8 inst, 
    AlmInfoLen infoLen, U64 value) REENTRANT;




////////////////////////////////////////////////////////////////////////////////
/// CtcAlmTlvAdd:  Append a CTC alarm TLV in the reply buffer
///
 // Parameters:
/// \param cont Ctc Alarm container
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void CtcAlmTlvAdd(const CtcAlmCont BULK * cont);


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
extern
BOOL CtcAlmCondCreate(CtcAlmCont BULK * almCont, 
                      AlmCondition cond, U8 inst, StatId stat);

////////////////////////////////////////////////////////////////////////////////
/// \brief  Send Ctc Alarm Message
///
/// \param cont Alarm content to add to queue
///
/// \return TRUE if the operation is success
////////////////////////////////////////////////////////////////////////////////
extern
BOOL CtcAlmMsg(const CtcAlmCont BULK * cont);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Send Ctc PON_IF Switch Alarm Message
///
/// \param None
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void CtcAlmPonIfSwitchNotify(void);


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
extern
void CtcPerfMonAlmNotify(CtcAlmMonThd BULK * pAlmThd, 
						     const U64 BULK * current,
						     OamCtcAlarmId almId,  
						     TkOnuEthPort port);


////////////////////////////////////////////////////////////////////////////////
/// CtcAlm1sTimer:  CTC alarm 1s timer
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void CtcAlm1sTimer (void);


/******************************************************************************/
/* GetOnuTempThd - Get Onu temperature threshold value                        */
/*                                                                            */
/*                                                                            */
/*   This function gets Onu temperature threshold value from alarm Id         */
/*                                                                            */
/*   Parameters:                                                              */
/*                   OamCtcTlvAlarmThd BULK* src                              */
/*                   U16 almId                                                */
/*                                                                            */ 
/*   \return:                                                                 */
/*             The threshold value                                            */
/******************************************************************************/
extern
BOOL GetOnuTempThd(U16 armId, U32 *raise, U32 *clear);

/******************************************************************************/
/*   SetOnuTempThd - Set Onu temperature threshold                            */
/*                                                                            */
/*                                                                            */
/*   This function: Set Onu temperature threshold from alarm Id               */
/*                                                                            */
/*   Parameters: U8 BULK * val, U16 almId                                     */
/*                                                                            */
/*   \return:                                                                 */
/*             Operation result                                               */
/******************************************************************************/
extern
BOOL SetOnuTempThd(U16 armId, U32 raise, U32 clear);

#if defined(__cplusplus)
}
#endif

#endif // CtcAlarms.h


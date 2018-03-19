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

#if !defined(OsAstMsgQ_h)
#define OsAstMsgQ_h
////////////////////////////////////////////////////////////////////////////////
/// \file OsAstMsgQ.h
/// \brief ONU
/// \author Lingyong Chen
/// \date May 24, 2011
///
////////////////////////////////////////////////////////////////////////////////

#include "Teknovus.h"
#include "OnuOs.h"

// message queue fields
#define   MsgQRaiseSet      0x80000000
#define   MsgQRaiseMsk      0x80000000
#define   MsgQRaiseSft      31
#define   MsgQIdMsk         0x7fff0000
#define   MsgQIdSft         16
#define   MsgQInstMsk       0x0000ff00
#define   MsgQInstSft       8
#define   MsgQStatMsk       0x000000ff
#define   MsgQStatSft       0

typedef struct
    {
    BOOL            raise;
    OnuAssertId     id;
    U8              inst;
    U8              stat;
    } PACK TkOsAstRec;


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
extern
void OsAstMsgQSet (OnuAssertId id, U8 inst, U8 stat) REENTRANT;


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
extern
void OsAstMsgQClr (OnuAssertId id, U8 inst, U8 stat) REENTRANT;

////////////////////////////////////////////////////////////////////////////////
/// OsAstMsgQSpin:  Handle all events in os assert of message queue
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OsAstMsgQSpin (void);


////////////////////////////////////////////////////////////////////////////////
/// OsAstMsgQFlush:  Flush the content of the osAstEvtQ
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OsAstMsgQFlush (void);


////////////////////////////////////////////////////////////////////////////////
/// OsAstMsgQPoll:  Poll os assert of message queue
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OsAstMsgQPoll (void);


////////////////////////////////////////////////////////////////////////////////
/// OsAstMsgQInit:  Initialization for os assert message queue
///
 // Parameters:
/// None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OsAstMsgQInit (void);


#endif // End of file OsAstMsgQ.h


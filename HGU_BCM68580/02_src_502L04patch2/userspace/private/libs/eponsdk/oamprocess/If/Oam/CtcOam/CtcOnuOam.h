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

#if !defined(CtcOnuOam_h)
#define CtcOnuOam_h
////////////////////////////////////////////////////////////////////////////////
/// \file CtcOnuOam.h
/// \brief Definitions for China Telecom extended OAM
/// \author Jason Armstrong
/// \date February 28, 2006
///
///
////////////////////////////////////////////////////////////////////////////////

#include "Teknovus.h"
#include "Build.h"
#include "CtcOam.h"
#include "CtcOnuUtils.h"
#include "CtcMLlid.h"
#include "bcm_epon_cfg.h"
#include "OntmTimer.h"
#include "CtcAlarms.h"

#define DEF_CTC_LINK_NUM           1

#define DEF_CTC_SLLID_Q_NUM        8

//##############################################################################
//                  Cuc Record 
//##############################################################################

// CucOamVersion - Cuc oam versions supported
typedef enum
    {
    CucOamVerDef        = 0x00,
    CucOamVer03         = 0xC3,
    CucOamVerNum        = 1
    } PACK CucOamVersion;


//##############################################################################
//                 CTC Miscellaneous Fds Record
//##############################################################################

typedef enum
    {
    CtcFdsUpQueueSize,
    CtcFdsLoidPswdCfg,
	CtcFdsFlowControl,
    CtcFdsMiscRecNums
    } CtcFdsMiscRecId;

typedef enum
    {
    CtcLlidModeNone,
    CtcLlidModeSllid,
    CtcLlidModeMllid,

    CtcLlidModeNums
    }CtcLlidMode;

//##############################################################################
//                  Ctc Link Record management module
//##############################################################################

// CtcOamVersion - Ctc oam versions supported
typedef enum
    {
    CtcOamVerDef        = 0x00,
    CtcOamVer01         = 0x01,
    CtcOamVer13         = 0x13,
    CtcOamVer20         = 0x20,
    CtcOamVer21         = 0x21,
    CtcOamVer30         = 0x30,
    CtcOamVerNum        = 5
    } PACK CtcOamVersion;


////////////////////////////////////////////////////////////////////////////////
/// CtcLinkOamRecord - Handy record for maintaining CTC link oam status
///
/// This structure maintains data for the mangement of links under the China
/// Telecom OAM environment.  It contains information for state and OAM version
/// management.
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    CtcOamVersion           version;
    OamCtcDiscoveryState    state;
    U8                      numExt;
    OamCtcInfoTlvExtData    support[CtcOamVerNum];
    } PACK CtcLinkRecord;

#define CtcSllidMaxQueues   8
#define CtcSllidMaxPri      8
#define MaxCtcQueueSizes    (TkOnuNumUpQueues - CtcMLlidSupport)
typedef U16                 CtcQueueSize;

typedef struct
    {
    U8 count;
	U8 priNum;
	U8           wrrOfPri[CtcSllidMaxPri];
	U8           qNumOfPri[CtcSllidMaxPri];
	CtcQueueSize size[CtcSllidMaxQueues];
    }  PACK CtcUpSllidQueueSizes;


typedef struct
    {
    U8 queueCount;
    CtcQueueSize ctcUpQueueSize[MaxCtcQueueSizes];
    }  PACK PersCtcUpQueueSizes;


typedef enum
    {
    OamCtcExtEvtStatReq = 0x01,
    OamCtcExtEvtStatSet = 0x02,
    OamCtcExtEvtStatRep = 0x03,
    OamCtcExtEvtThrdReq = 0x04,
    OamCtcExtEvtThrdSet = 0x05,
    OamCtcExtEvtThrdRep = 0x06
    } PACK OamCtcExtEvtSubtype;

typedef struct
    {
    OamCtcOpcode        ext;
    OamCtcExtEvtSubtype subtype;
    U16                 num;
    } PACK OamCtcExtEvtPdu;

typedef struct tagOamCtcAlarmObj
    {
    U16                 objType;
    OamCtcInstNumU      instNum;
    U16                 almID;
    } PACK OamCtcAlarmObj;

typedef struct tagOamCtcAlarmStat
    {
    OamCtcAlarmObj  almObj;
    U32             stat;
    } PACK OamCtcAlarmStat;

typedef struct tagOamCtcAlarmThrd
    {
    OamCtcAlarmObj  almObj;
    U32             raiseVal;
    U32             clearVal;
    } PACK OamCtcAlarmThrd;

#define CtcAlmEnable        0x00000001
#define CtcAlmDisable       0x00000000
#define CtcAlmInValid       0xFFFFFFFF

#define OamCtc30PonIfAlmNum ((OamCtcPonIfAlarmEnd30 - OamCtcPonIfAlarmEnd) + 1)
#define OamCtc30PortAlmNum ((OamCtcPortAlarmEnd30 - OamCtcPortAlarmEnd) + 1)

#define LuePolicerStatsPointer(port)  ((port) + 1 + (TkOnuNumRxOnlyLinks * \
                                      LinkRxOnlyBinNumStats))

#define CtcLinkIsNegotiating(link) ((CtcLinkStateGet(link) != OamCtcDiscWait) \
									&& (CtcLinkStateGet(link) \
									!= OamCtcDiscSuccess))


extern CtcAlmMonThd ctc30AlmThd[OamCtc30PonIfAlmNum];
extern CtcAlmMonThd ctc30EthAlmThd[MAX_UNI_PORTS][OamCtc30PortAlmNum];

extern CtcLlidMode BULK ctcLlidMode;
extern CtcUpSllidQueueSizes BULK sLlidUpQueueSizes;
extern PersCtcUpQueueSizes BULK ctcUpQueueSizes;

//CTC Aging time is tracked in seconds
extern U32 macLearnCtcAgingTime;
extern BOOL macCtcAgingTimeSet;


////////////////////////////////////////////////////////////////////////////////
/// CtcLinkInit - Initialize a link oam management record
///
/// This function initalizes a link management record to the default state.
/// The default state is unnegotiated with version 0 and no extended OUIs.
///
/// \param link  The index of the link to initialize
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void CtcLinkInit(LinkIndex link);


////////////////////////////////////////////////////////////////////////////////
/// CtcLinkDeregister - Deregister a link
///
/// This function restores a link management record to its default values.
///
/// \param ilink Logical link ID
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
#define CtcLinkDeregister(link)   CtcLinkInit(link)



////////////////////////////////////////////////////////////////////////////////
/// CtcLinkVersionGet - Get the negotiated version of the link
///
/// This function returns the negotiated version number of a link.  If
/// negotiation has not completed on this link, the return value is undefined.
///
/// \param link  The index of the link
///
/// \return
/// the version negotiated for the link
////////////////////////////////////////////////////////////////////////////////
extern
CtcOamVersion CtcLinkVersionGet(LinkIndex link);


////////////////////////////////////////////////////////////////////////////////
/// CtcLinkVersionSet - Set the negotiated version of the link
///
/// This function sets the negotiated China Telecom OAM version number of a
/// link.  The value may be queried later with CtcLinkGetVersion.
///
/// \param link    The index of the link
/// \param version Version number to set
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void CtcLinkVersionSet (LinkIndex link, CtcOamVersion version);


////////////////////////////////////////////////////////////////////////////////
/// CtcLinkStateGet - Get the links version negotiation state
///
/// This function returns the China Telecom extended OAM version negotiation
/// state for this link.
///
/// \param link  The index of the link
///
/// \return
/// Link OAM version negotiation state
////////////////////////////////////////////////////////////////////////////////
extern
OamCtcDiscoveryState CtcLinkStateGet (LinkIndex link);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Transition the link to the specified discovery state
///
/// \param  link    The index of the link
/// \param  next    The new state
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void CtcLinkStateSet(LinkIndex link, OamCtcDiscoveryState next);


////////////////////////////////////////////////////////////////////////////////
/// CtcOamDiscCompMap - Build map of links that have completed CTC OAM discovery
///
/// \return
/// Map of links that have completed CTC OAM discovery
////////////////////////////////////////////////////////////////////////////////
extern
U32 CtcOamDiscCompMap (void);


////////////////////////////////////////////////////////////////////////////////
/// CtcLinkExtDataGet - Get an extended version record for link
///
/// This function fetches a extended data record from the link management
/// record.  If the index is out of range FALSE is return the extended data
/// field is left alone.
///
 // Parameters:
/// \param link  The index of the link
/// \param index Extended data record to access
/// \param ext   Extended data record to copy to
///
/// \return
/// TRUE if index was in range
////////////////////////////////////////////////////////////////////////////////
extern
BOOL CtcLinkExtDataGet (LinkIndex link, U8 index,
                        OamCtcInfoTlvExtData BULK *ext);


////////////////////////////////////////////////////////////////////////////////
/// CtcLinkExtDataAdd - Add an extended data record to link
///
/// This function adds a new extended OAM support record to the link management
/// record.  If the new data will not fit in the record, FALSE is returned.
///
 // Parameters:
/// \param link  The index of the link
/// \param ext   Extended data record to add
///
/// \return
/// TRUE if the record was added
////////////////////////////////////////////////////////////////////////////////
extern
BOOL CtcLinkExtDataAdd (LinkIndex link,
                        const OamCtcInfoTlvExtData BULK *ext);


////////////////////////////////////////////////////////////////////////////////
/// CtcLinkExtDataCount - Get the number of extended data records
///
/// This function returns the number of extended OAM support records that are
/// currently stored for a link.
///
 // Parameters:
/// \param link  The index of the link
///
/// \return
/// Number of extended OAM support records
////////////////////////////////////////////////////////////////////////////////
extern
U8 CtcLinkExtDataCount (LinkIndex link);


////////////////////////////////////////////////////////////////////////////////
/// CtcSetMacAgingTime - Set Mac Aging TIme
/// \param aging Aging time
/// \return
/// TRUE if index was in range
////////////////////////////////////////////////////////////////////////////////
extern
BOOL CtcSetMacAgingTime(U32 aging);


////////////////////////////////////////////////////////////////////////////////
/// CtcGetMacAgingTime - Get Mac Aging TIme
/// \return
/// TRUE if index was in range
////////////////////////////////////////////////////////////////////////////////
extern
U32 CtcGetMacAgingTime(void);


//##############################################################################
// Generic Module Definitions
//##############################################################################



////////////////////////////////////////////////////////////////////////////////
/// OamCtcHandleEventExt - Handle CTC Event extending OAM
///
/// This function handles all incoming China Telecom extended OAM messages that
/// is for CTC Event OAM. The message type is 0xFF. Subtype is 0x01 to 0x06
///
 // Parameters:
/// \param source source port
/// \param link Link of frame reception
/// \param msg CTC extension body from message
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
extern
void OamCtcHandleEventExt (OamCtcExtEvtPdu BULK *msg);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Handle incoming CTC OAM messages
///
/// This function handles all incoming China Telecom extended OAM messages.
/// OAM messages are determined to belong to this OAM set if they contain an OAM
/// opcode of 0xFE (Vendor OUI) and a vendor OUI of China Telecom.
///
/// \param link     Link of frame reception
/// \param msg      CTC extension body from message
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
extern
void OamCtcHandle (LinkIndex link, U8 BULK* msg);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Load default CTC queue sizes from FDS
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
extern
void CtcInitQueueSizes(void);



////////////////////////////////////////////////////////////////////////////////
/// CtcConfigCommit - Commit any altered provisioning to NVS
///
/// Some change take too long to commit to NVS and still meet the OAM timing.
/// Any NVS write fitting that description should be added to this function.  It
/// will commit any changed provisioning scheme.
///
 // Parameters:
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void CtcConfigCommit (void);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Handle all links de-registering
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void OamCtcAllLinksDown(void);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Handle CTC module timers
///
/// \param timerId  ID of the timer that expired
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void OamCtcTimerHandle(OntmTimerId timerId);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Poll the CTC module
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void OamCtcPoll(void);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Register the CTC configuration FDS group
///
/// \return Nothing
////////////////////////////////////////////////////////////////////////////////
extern
void OamCtcFdsGroupReg(void);


////////////////////////////////////////////////////////////////////////////////
/// OamCtcInit - Initialize the CTC module
///
/// This function intializes all of the modules needed for China Telecom OAM
/// operation.
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
extern
void OamCtcInit (void);


//##############################################################################
// OAM Get Module Definitions
//##############################################################################

////////////////////////////////////////////////////////////////////////////////
/// \brief  Process extended variable request frame
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void OamCtcHandleExtVarReq(void);


//##############################################################################
// OAM Set Module Definitions
//##############################################################################

////////////////////////////////////////////////////////////////////////////////
/// \brief  Handle a CTC extended set request message
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void OamCtcHandleSetRequest(void);


#endif // End of File CtcOnuOam.h

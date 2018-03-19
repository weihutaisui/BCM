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
/// \file CtcMLlid.h
/// \brief China Telecom extended Multi-Llid module
/// \author Shaobin Wang
/// \date February 1, 2010
///
////////////////////////////////////////////////////////////////////////////////

#include "Teknovus.h"
#include "Oam.h"


#if !defined(CtcMLlid_h)
#define CtcMLlid_h

#if defined(__cplusplus)
extern "C" {
#endif


// Though we support 16 bi-LLIDs, but CTC indicates
// the Maximum of LLIDs ONU supports is 8.
#define CtcMLlidSupport       8

// The number of user up-queues is the total up-queues subtract
// the number of oam queues for the maximum LLIDs.
#define CtcUpQueueSizeDefault    16



typedef struct
    {
    U16 queueID;
    U16 wrrOfQueue;
    } PACK CtcLlidQueueEntry;

typedef struct
    {
    U8 queues;
    CtcLlidQueueEntry qCfg[1];
    } PACK CtcLlidQueueCfg;


// Not sorted
typedef struct
    {
    U8 queues;
    CtcLlidQueueEntry qCfg[8];
    } PACK CtcLlidQueueCfgLocal;

// Sorted
typedef struct
    {
    U8 queues;
    CtcLlidQueueEntry qCfg[8];
    } PACK CtcLlidQueueCfgMap;


////////////////////////////////////////////////////////////////////////////////
/// CtcMLlidGetLowestQueueId:  Get the lowest CTC queue Id configurated
///
/// This function gets the CTC queue ID whose priority is the lowest in the
/// queues configurated
///
/// Parameters:
/// \param  None
///
/// \return
/// the lowest CTC queue Id configurated
////////////////////////////////////////////////////////////////////////////////
extern
U8 CtcMLlidGetLowestQueueId(void);


////////////////////////////////////////////////////////////////////////////////
/// CtcMLlidGetLinkQueueNum:  Get the Number of queues were mapping to link
///
///
/// Parameters:
/// \param  link            The LLID index
///
/// \return
/// the Number of queues for the link
////////////////////////////////////////////////////////////////////////////////
extern
U8 CtcMLlidGetLinkQueueNum(LinkIndex link);


////////////////////////////////////////////////////////////////////////////////
/// CtcMLlidGetLinkQueueId:  Get the CTC queue Id of the Nth queue of link
///
///
/// Parameters:
/// \param  link            The LLID index
/// \param  index         The queue index
///
/// \return
/// the CTC queue Id of the Nth queue of link
////////////////////////////////////////////////////////////////////////////////
extern
U8 CtcMLlidGetLinkQueueId(LinkIndex link, U8 index);


////////////////////////////////////////////////////////////////////////////////
/// CtcMLlidInit:  Initialize the contol variables of Ctc M-LLID module
///
///
/// Parameters:
/// \param  None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void CtcMLlidInit(void);


////////////////////////////////////////////////////////////////////////////////
/// CtcMLlidInitLink:  Initialize the configuration variables for the link
///
///
/// Parameters:
/// \param  link        the link to init
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void CtcMLlidInitLink(LinkIndex link);


////////////////////////////////////////////////////////////////////////////////
/// CtcMLlidAdminControl - Act for the CTC MultiLlidAdminControl
///
/// This function performs the bi-transition between CTC single-Llid
/// and CTC multi-Llid
///
/// Parameters:
/// \param numOfLlid Number of LLIDs to set
///
/// \return
/// TRUE successful, or FALSE
////////////////////////////////////////////////////////////////////////////////
extern
BOOL CtcMLlidAdminControl (U8 numLlid);


////////////////////////////////////////////////////////////////////////////////
/// CtcMLlidQueueConfigSet - Set the LLID queue configuration
///
/// Parameters:
/// \param link     the LLID index
/// \param src      Variable information source
/// \param cont     Container to place information
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void CtcMLlidQueueConfigSet (LinkIndex link,
							OamVarContainer BULK * src,
                         	OamVarContainer BULK *cont);


////////////////////////////////////////////////////////////////////////////////
/// CtcMLlidQueueConfigGet - Get the LLID queue configuration
///
/// This function get the CTC2.1 multi-LLID queue configuration per link
///
/// Parameters:
/// \param link   the LLID index
/// \param buf    the buffer for the queue configuration
///
/// \return
/// length of the configuration
////////////////////////////////////////////////////////////////////////////////
extern
U8 CtcMLlidQueueConfigGet (LinkIndex link, const CtcLlidQueueCfg BULK* buf);


////////////////////////////////////////////////////////////////////////////////
/// CtcMLlidQueueConfigUpdate - Update the link queue configuration for given 
/// link according to the provided buffer.
///
///
/// Parameters:
/// \param link      the LLID index
/// \param queueCfg  the buffer the new link queue configuration
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void CtcMLlidQueueConfigUpdate(LinkIndex link, 
                               const CtcLlidQueueCfgLocal BULK* queueCfg);

#if defined(__cplusplus)
}
#endif

#endif

// End of File CtcMLlid.h



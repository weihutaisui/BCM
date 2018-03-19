/*
* <:copyright-BRCM:2012:proprietary:epon
* 
*    Copyright (c) 2012 Broadcom 
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
/// \file CtcMLlid.c
/// \brief China Telecom extended Multi-Llid module
/// \author Shaobin Wang
/// \date February 1, 2010
///
////////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include "Teknovus.h"
#include "CtcMLlid.h"
#include "OntConfigDb.h"
#include "PonManager.h"
#include "CtcOnuOam.h"
#include "CtcClassification.h"
#include "CtcEthControl.h"
#include "Stream.h"


CtcLlidQueueCfgLocal BULK ctcLlidQueueCfgRec[CtcMLlidSupport];
CtcLlidQueueCfgMap BULK ctcLlidQueueCfgReMap[CtcMLlidSupport];


////////////////////////////////////////////////////////////////////////////////
/// \brief  Create the queue configuration for Single-LLID mode
///
/// \param  None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void CtcSLlidQueueCfgCreate(void)
    {
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Create the new queue configuration according to the old queue
///         configuration infomation for upstream queue
///
/// \param  linkCount   The new number of multi LLID
/// \param  buf         Pointer to the configuration
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void CtcMLlidQueueCfgCreate(U8 linkCount)
    {
    }


////////////////////////////////////////////////////////////////////////////////
/// LinkQueuePriRemap:  rearrang the CTC queue ID by priority (high to low)
///
/// This function rearrang the CTC queue ID by the bubble sort.
/// For the priority of the CTC queue is high if the CTC queue ID
/// is large number. So we need to sort first before we allocate the queue.
///
/// Parameters:
/// \param
///             llid         The llid to reconfiguration
///             QueueCfg     CTC queue configuration
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void LinkQueuePriRemap (LinkIndex link, const CtcLlidQueueCfg BULK* queueCfg)
    {
    U8      FAST i;
    U8      FAST j;
    BOOL    FAST exchange;

    memset ((U8 *)&ctcLlidQueueCfgReMap[link], 0, sizeof (CtcLlidQueueCfgMap));

    // copy the queue config to the remap memory
    ctcLlidQueueCfgReMap[link].queues = queueCfg->queues;
    for (i = 0; i < queueCfg->queues; i++)
        {
        ctcLlidQueueCfgReMap[link].qCfg[i].queueID =
                                   queueCfg->qCfg[i].queueID;
        ctcLlidQueueCfgReMap[link].qCfg[i].wrrOfQueue =
                                   queueCfg->qCfg[i].wrrOfQueue;
        }

    // bubble sort the CTC queue
    exchange = TRUE;
    i = 0;
    while ((i < (ctcLlidQueueCfgReMap[link].queues - 1)) && exchange)
        {
        exchange = FALSE;
        for (j = ctcLlidQueueCfgReMap[link].queues - 1; j > i; j--)
            {
            if (ctcLlidQueueCfgReMap[link].qCfg[j].queueID >
                          ctcLlidQueueCfgReMap[link].qCfg[j-1].queueID)
                {
                U16 FAST tmp;
                exchange = TRUE;
                // exchange QId
                tmp = ctcLlidQueueCfgReMap[link].qCfg[j].queueID;
                ctcLlidQueueCfgReMap[link].qCfg[j].queueID =
                          ctcLlidQueueCfgReMap[link].qCfg[j-1].queueID;
                ctcLlidQueueCfgReMap[link].qCfg[j-1].queueID = tmp;

                //exchange WRR
                tmp = ctcLlidQueueCfgReMap[link].qCfg[j].wrrOfQueue;
                ctcLlidQueueCfgReMap[link].qCfg[j].wrrOfQueue=
                          ctcLlidQueueCfgReMap[link].qCfg[j-1].wrrOfQueue;
                ctcLlidQueueCfgReMap[link].qCfg[j-1].wrrOfQueue= tmp;
                }
            }
        i++;
        }
    } // LinkQueuePriRemap


////////////////////////////////////////////////////////////////////////////////
/// BaseLinkCfgSet:  Set the base link cfg info for CTC
///
///
/// Parameters:
/// \param  None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void BaseLinkCfgSet(void)
    {
    // create the base link CtcLlidQueueCfg
    ctcLlidQueueCfgRec[0].queues = 1;              // One queue
    ctcLlidQueueCfgRec[0].qCfg[0].queueID = 0;     // Ctc QId = 0
    ctcLlidQueueCfgRec[0].qCfg[0].wrrOfQueue = 0;  // Ctc QWrr = 0

    //queue mapping
    LinkQueuePriRemap (0, (const CtcLlidQueueCfg BULK*)&ctcLlidQueueCfgRec[0]);
    } // BaseLinkCfgSet


////////////////////////////////////////////////////////////////////////////////
/// CtcMLlidSetReportMode:   Adjust the report mode for CTC multi LLID mode
///
// Parameters:
/// \param  linkNum  The number of LLIDs
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void CtcMLlidSetReportMode (U8 linkNum)
    {
    if (linkNum <= 3)
        {
        PonMgrSetReporting(RptModeMultiPri8, FALSE);
        }
    else if (linkNum <= 6)
        {
        PonMgrSetReporting(RptModeMultiPri4, FALSE);
        }
    else
        {
        PonMgrSetReporting(RptModeMultiPri3, FALSE);
        }
    } // CtcMLlidSetReportMode


////////////////////////////////////////////////////////////////////////////////
/// ChangeCtcLlidMode:  Change ONU Ctc LLID mode and enable the LLIDs
///
///
/// Parameters:
/// \param  num     The number of LLIDs
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void ChangeCtcLlidMode(U8 linkNum)
    {
    memset((U8 *)ctcLlidQueueCfgRec, 0, sizeof(ctcLlidQueueCfgRec));
    memset((U8 *)ctcLlidQueueCfgReMap, 0, sizeof(ctcLlidQueueCfgReMap));

    CtcMLlidSetReportMode(linkNum);
    //upstream
    if (linkNum == 0)
        {
        CtcSLlidQueueCfgCreate();
        }
    else
        {
        BaseLinkCfgSet();
        CtcInitQueueSizes ();
        CtcMLlidQueueCfgCreate(linkNum);
        }

    CtcClassificationClrAllRules();
    } // ChangeCtcLlidMode


////////////////////////////////////////////////////////////////////////////////
/// ChangeCtcLlidNum:  Change the number of CTC LLIDs in multi mode
///
///
/// Parameters:
/// \param  newLinkNum     The number of LLIDs
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void ChangeCtcLlidNum(U8 newLinkNum)
    {
    U8  FAST link;
    LinkIndex FAST oldLinkNum;

    oldLinkNum = OntCfgDbGetNumUserLinks();
    if (newLinkNum < oldLinkNum)
        {
        for (link = newLinkNum; link < oldLinkNum; link++)
            {
            CtcClassificationLinkRuleClr(link);
            CtcMLlidInitLink(link);
            }
        }

    CtcMLlidSetReportMode(newLinkNum);
    //make upstream queue configuration
    CtcMLlidQueueCfgCreate(newLinkNum);
    } // ChangeCtcLlidNum


////////////////////////////////////////////////////////////////////////////////
/// LlidQueueCfgCheck - Check the Queue ID's validity and whether it is
///                     conflict with other link's Queue ID
///
// Parameters:
/// \param  link       The LLID index
/// \param  cfg        The CTC queue configuratoon
///
/// \return
/// True if no error, otherwise FALSE
////////////////////////////////////////////////////////////////////////////////
static
BOOL LlidQueueCfgCheck(LinkIndex link, const CtcLlidQueueCfg BULK* cfg)
    {
    U8  FAST i;
    U8  FAST j;
    U8  FAST linkIdx;
    U16 FAST qId;
    U16 FAST wrrSum = 0;

    if (cfg->queues == 0)
        {
        return FALSE;
        }

    for (i = 0; i < cfg->queues; i++)
        {
        qId = cfg->qCfg[i].queueID;
        wrrSum += cfg->qCfg[i].wrrOfQueue;
        //queue ID bigger than the maximum
        if (qId >= MaxCtcQueueSizes)
            {
            return FALSE;
            }

        linkIdx = OntCfgDbGetNumUserLinks();
        // if conflict with other link configuration
        while (linkIdx-- != 0)
            {
            //skip current link itself
            if (linkIdx == link)
                {
                continue;
                }
            for (j = 0; j < ctcLlidQueueCfgRec[linkIdx].queues; j++)
                {
                // the queue ID match with other link's queue ID
                if (qId == ctcLlidQueueCfgRec[linkIdx].qCfg[j].queueID)
                    {
                    return FALSE;
                    }
                }
            }

        // if there are some same queue ID in the configuration
        for (j = 0; j < cfg->queues; j++)
            {
            if (i != j)
                {
                if (qId == cfg->qCfg[j].queueID)
                    {
                    return FALSE;
                    }
                }
            }
        }

    if ((wrrSum != 0) && (wrrSum != 100))
        {
        return FALSE;
        }

    return TRUE;
    } // LlidQueueCfgCheck


////////////////////////////////////////////////////////////////////////////////
/// IsLinkQueueCfgChanged - Check whether the link queue configuratoon is
///                         changed against its old configuration when the link
///                         number is same with its old configuration
///
// Parameters:
/// \param  link       The LLID index
/// \param  cfg        The CTC queue configuratoon
///
/// \return
/// TRUE if changed, otherwise FALSE
////////////////////////////////////////////////////////////////////////////////
static
BOOL IsLinkQueueCfgChanged(LinkIndex link, const CtcLlidQueueCfg BULK* cfg)
    {
    U8 FAST i;
    U8 FAST j;
    CtcLlidQueueEntry BULK *entryPtr;

    entryPtr = (CtcLlidQueueEntry BULK *)&cfg->qCfg[0];

    for (i = 0; i < cfg->queues; i++)
        {
        for (j = 0; j < cfg->queues; j++)
            {
            if ((entryPtr[i].queueID ==
                 ctcLlidQueueCfgRec[link].qCfg[j].queueID) &&
                (entryPtr[i].wrrOfQueue ==
                 ctcLlidQueueCfgRec[link].qCfg[j].wrrOfQueue))
                {
                break;    //find
                }
            }
        if (j == cfg->queues)
            {
            return TRUE;        // Not find, changed
            }
        }
    return FALSE;
    } // IsLinkQueueCfgChanged


////////////////////////////////////////////////////////////////////////////////
/// LinkQueueCfgSet:   Adjust the queue configuration for one link in
///                    CTC multi LLID mode
///
// Parameters:
/// \param  link       The LLID index
/// \param  queueCfg   The CTC queue configuratoon
/// \param  length     The length of the CtcLlidQueueCfg configuration
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void LinkQueueCfgSet (LinkIndex link,
                      const CtcLlidQueueCfg BULK *queueCfg,
                      U8 length)
    {
    memcpy ((U8 *)&ctcLlidQueueCfgRec[link], queueCfg, length);

    // the upstream queue configuration of LLID link
    LinkQueuePriRemap (link, queueCfg);
    CtcMLlidQueueCfgCreate(OntCfgDbGetNumUserLinks());

    } // LinkQueueCfgSet


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
//extern
U8 CtcMLlidGetLowestQueueId(void)
    {
    U8 FAST i,j;
    U16 FAST qId = MaxCtcQueueSizes;

    for (i = 0; i < CtcMLlidSupport; i++)
        {
        for (j = 0; j < ctcLlidQueueCfgReMap[i].queues; j++)
            {
            if (ctcLlidQueueCfgReMap[i].qCfg[j].queueID < qId)
                {
                qId = ctcLlidQueueCfgReMap[i].qCfg[j].queueID;
                }
            }
        }

    return (U8)qId;
    } // CtcMLlidGetLowestQueueId


////////////////////////////////////////////////////////////////////////////////
/// CtcMLlidGetLinkQueueNum:  Get the Number of queues were mapping to link
///
/// This function gets the total number of CTC queues configurated
/// for the link currently
///
/// Parameters:
/// \param  link            The LLID index
///
/// \return
/// the Number of queues for the link
///////////////////////////////////////////////////////////////////////////////
//extern
U8 CtcMLlidGetLinkQueueNum(LinkIndex link)
    {
    return ctcLlidQueueCfgReMap[link].queues;
    } // CtcMLlidGetLinkQueueNum


////////////////////////////////////////////////////////////////////////////////
/// CtcMLlidGetLinkQueueId:  Get the CTC queue Id of the Nth queue of link
///
/// This function gets the CTC queue ID which is indexTh queue of the link
///
/// Parameters:
/// \param  link          The LLID index
/// \param  index         The queue index
///
/// \return
/// the CTC queue Id of the Nth queue of link
////////////////////////////////////////////////////////////////////////////////
//extern
U8 CtcMLlidGetLinkQueueId(LinkIndex link, U8 index)
    {
    return (U8)ctcLlidQueueCfgReMap[link].qCfg[index].queueID;
    } // CtcMLlidGetLinkQueueId



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
//extern
void CtcMLlidInit(void)
    {
    ctcLlidMode = CtcLlidModeNone;
    memset(ctcLlidQueueCfgRec, 0, sizeof(ctcLlidQueueCfgRec));
    memset(ctcLlidQueueCfgReMap, 0, sizeof(ctcLlidQueueCfgReMap));

    BaseLinkCfgSet();
    } // CtcMLlidInit


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
//extern
void CtcMLlidInitLink(LinkIndex link)
    {
    memset((U8 *)&ctcLlidQueueCfgRec[link], 0, sizeof(CtcLlidQueueCfgLocal));
    memset((U8 *)&ctcLlidQueueCfgReMap[link], 0, sizeof(CtcLlidQueueCfgMap));
    } // CtcMLlidInitLink


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
//extern
BOOL CtcMLlidAdminControl (U8 numLlid)
    {
    if (numLlid > CtcMLlidSupport)
        {
        return FALSE;
        }

    if (numLlid == 0)          // Single LLID mode
        {
        if (ctcLlidMode != CtcLlidModeSllid)  // Change from M-LLID mode
            {
            ChangeCtcLlidMode(numLlid);
            }
        ctcLlidMode = CtcLlidModeSllid;
        }
    else                       // Multi LLID mode
        {
        if (ctcLlidMode != CtcLlidModeMllid) // Change from single LLID mode
            {
            ChangeCtcLlidMode(numLlid);
            }
        else                   // change the num of LLIDs in MLLID mode
            {
            if ((numLlid != OntCfgDbGetNumUserLinks()))
                {
                ChangeCtcLlidNum(numLlid);
                // adjust all the remaining rules
                CtcClassificationAdjust();
                }
            }
        ctcLlidMode = CtcLlidModeMllid;
        }
    return TRUE;
    } // CtcMLlidAdminControl


////////////////////////////////////////////////////////////////////////////////
/// CtcQueueConfigOamValidate - validate whether the ram resources are OK
///                                              for CTC queue configuration.
/// Parameters:
/// \param link     the LLID index
/// \param cfg      the CtcLlidQueueCfg configuration info
///
/// \return
/// True if success, otherwise FALSE
////////////////////////////////////////////////////////////////////////////////
static
BOOL CtcQueueConfigOamValidate (LinkIndex link,
                             const CtcLlidQueueCfg BULK* cfg)
    {
	return TRUE;
    } // CtcMLlidQueueConfigSet


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
//extern
void CtcMLlidQueueConfigSet (LinkIndex link,
							OamVarContainer BULK* src,
                         	OamVarContainer BULK *cont)
    {
    CtcLlidQueueCfg BULK* cfg = (CtcLlidQueueCfg *)src->value;

	if((ctcLlidMode != CtcLlidModeMllid) || (!LlidQueueCfgCheck(link,cfg)))
		{
		cont->length = OamVarErrActBadParameters;
		return;
		}

    if (ctcLlidQueueCfgRec[link].queues == cfg->queues)
        {
        //the queue configuration is same as old
        if (!IsLinkQueueCfgChanged(link,cfg))
            {
            return;
            }
        }

	/*verify wheher the queue resources are OK*/
	if(!CtcQueueConfigOamValidate(link, cfg))
		{
		cont->length = OamVarErrActNoResources;
		return;
		}
	
    //clear the rules related with the link
    CtcClassificationLinkRuleClr(link);

    LinkQueueCfgSet(link, cfg, src->length);

    // adjust all the remaining rules
    CtcClassificationAdjust();
    } // CtcMLlidQueueConfigSet


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
//extern
U8 CtcMLlidQueueConfigGet (LinkIndex link,
                           const CtcLlidQueueCfg BULK* buf)
    {
    U8 FAST length;
    length = 1 + (ctcLlidQueueCfgRec[link].queues << 2);
    memcpy ((U8 *)buf, (U8 *)&ctcLlidQueueCfgRec[link], length);

    return length;
    } // CtcMLlidQueueConfigGet


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
//extern
void CtcMLlidQueueConfigUpdate(LinkIndex link,
                               const CtcLlidQueueCfgLocal BULK* queueCfg)
    {
    CtcClassificationLinkRuleClr(link);
    memcpy ((U8 *)&ctcLlidQueueCfgRec[link], queueCfg,
                                             sizeof(CtcLlidQueueCfgLocal));
    LinkQueuePriRemap(link, (const CtcLlidQueueCfg BULK* )queueCfg);
    } // CtcMLlidQueueConfigUpdate

// End of File CtcMLlid.c


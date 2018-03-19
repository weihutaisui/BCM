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
/// \file CtcOnuUtils.c
/// \brief China Telecom utility functions
/// \author Jason Armstrong
/// \date February 28, 2006
///
///
////////////////////////////////////////////////////////////////////////////////
#include <string.h>
#include "Teknovus.h"
#include "Oam.h"
#include "OamUtil.h"
#include "CtcOam.h"
#include "CtcOnuUtils.h"
#include "cms_log.h"

// The length for swap buffer, as now the longest CTC string is 24
// bytes width.
#define CtcStringSwapLen    24
// Buffer for CTC string swap
static  U8 BULK ctcStringSwap[CtcStringSwapLen];


////////////////////////////////////////////////////////////////////////////////
/// CtcStringSwap:  Ctc string swap
///
 // Parameters:
/// \param str     string for swap
/// \param len     string max length
///
/// \return
///         swapped string address
////////////////////////////////////////////////////////////////////////////////
U8 *CtcStringSwap(const U8 *str, U8 len)
    {
    U8 BULK sLen;

    memset(ctcStringSwap, 0x0, len);
    sLen = (U8)((strlen((char *)str) < len) ? strlen((char *)str) : len);
    memcpy(((ctcStringSwap + len) - sLen), str, sLen);

    return ctcStringSwap;
    } // CtcStringSwap


////////////////////////////////////////////////////////////////////////////////
/// OamCtcNextVarRequest - Get the next variable request
///
/// This function returns the next variable descriptor.  It may not be nessasay
/// however one workaround for the lack of object identification involves
/// sending a port number as part of the variable request.  It is better to have
/// this function in place in case that happens.
///
 // Parameters:
/// \param src      Variable descriptor
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
OamVarDesc BULK *OamCtcNextVarRequest (OamVarDesc BULK* src)
    {
    OamVarContainer const BULK * cont = (OamVarContainer const BULK *)src;
    if ((src->branch == OamCtcBranchObjInst)    ||
        (src->branch == OamCtcBranchObjInst21)  ||
        ((src->branch == OamCtcBranchExtAttribute) &&
        (OAM_NTOHS(src->leaf) == OamCtcAttrAlarmAdminState)) ||
        ((src->branch == OamCtcBranchExtAttribute) &&
        (OAM_NTOHS(src->leaf) == OamCtcAttrClassMarking)&&
        (cont->length == 0x01)) ||
        ((src->branch == OamCtcBranchExtAttribute) &&
        (OAM_NTOHS(src->leaf) == OamCtcAttrMcastCtrl) &&
        (cont->length == 0x01)))
        {
        return (OamVarDesc BULK *)NextCont (cont);
        }

    return src + 1;
    } // OamCtcNextVarRequest


////////////////////////////////////////////////////////////////////////////////
/// OamCtcInfoTlvGetExtCount - Get the number of extended OUIs in an info TLV
///
/// \param tlv TLV to check
///
/// \return Number of extended entries to process
////////////////////////////////////////////////////////////////////////////////
//extern
U8 OamCtcInfoTlvGetExtCount (const OamCtcInfoTlvHeader BULK* tlv)
    {
    // Get the number of of extended entries beyond the CTC OUI.  This will be
    // the length in the header minus the size of the header including the CTC
    // version and support flag all divided by the size of the extended OUI and
    // the version number.
    return (tlv->length - CtcInfoTvlHdrSize) / sizeof (OamCtcInfoTlvExtData);
    } // OamCtcInfoTlvGetExtCount


////////////////////////////////////////////////////////////////////////////////
/// OamCtcNextCont - Setup a next container
///
/// This function starts up a container with the same branch and leaf codes and
/// following the supplied context.  This should be used in variable responses
/// where more than one TLV must be returned from the lowest level handler
/// function.
///
 // Parameters:
/// \param cont Current OAM variable container
///
/// \return
/// Pointer to next container that was created
////////////////////////////////////////////////////////////////////////////////
//extern
OamVarContainer BULK *OamCtcNextCont (const OamVarContainer BULK *cont)
    {
    OamVarContainer BULK *next = NextCont (cont);

    next->branch = cont->branch;
    next->leaf = cont->leaf;

    return next;
    } // OamCtcNextCont


////////////////////////////////////////////////////////////////////////////////
/// \brief  This function terminates a response set
///
/// This function sets the terminal container value and returns a pointer to
/// the end of the final container
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamCtcTerminateBranch(void)
    {
    OamVarContainer BULK *cont = (OamVarContainer BULK *)oamParser.reply.cur;

    cont->branch = OamCtcBranchTermination;
    cont->leaf = 0;
    cont->length = 0;
    StreamSkip(&oamParser.reply, sizeof(OamVarContainer) - 1);
    } // OamCtcTerminateBranch


//##############################################################################
// CTC OAM Object Process Utilities
//##############################################################################
static
OamCtcObject BULK        ctcCurObject;


////////////////////////////////////////////////////////////////////////////////
/// \brief  Set the context for the following TLVs
///
/// This function sets the current object instance ID from a CTC2.0 or
/// earlier object instance TLV.  The reply container is also set.
///
/// \param src      pointer to var descriptors in request
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcSetObject20(const OamVarContainer BULK *src)
    {
    ctcCurObject.ctcVer = CtcObjVer20;
    switch (OAM_NTOHS(src->leaf))
        {
        case OamCtcContextPort:
            if(src->value[0] == 0)
                {
                ctcCurObject.objType = OamCtcObjOnu;
                ctcCurObject.instNum.uNum = OamCtcOnuInstNum;
                }
            else
                {
                ctcCurObject.objType = OamCtcObjPort;
                ctcCurObject.instNum.sNum.slotNum = 0x0;
                ctcCurObject.instNum.sNum.portNum = OAM_NTOHS(src->value[0]);
                if(src->value[0] <= OamCtcEthPortEnd)
                    {
                    ctcCurObject.instNum.sNum.portType = OamCtcPortEth;
                    }
                else if(src->value[0] <= OamCtcPotsPortEnd)
                    {
                    ctcCurObject.instNum.sNum.portType = OamCtcPortVoIP;
                    }
                else if(src->value[0] <= OamCtcE1PortEnd)
                    {
                    ctcCurObject.instNum.sNum.portType = OamCtcPortE1;
                    }
                else if(src->value[0] == 0xFF)
                    {
                    ctcCurObject.instNum.sNum.portType = OamCtcPortEth;
                    ctcCurObject.instNum.sNum.portNum = OamCtcAllUniPorts;
                    }
                else
                    {
                    //for lint
                    }
                }
            OamContextReset ((U8)(ctcCurObject.instNum.sNum.portNum), 0);
            break;

        default:
            break;
        }
    } // OamCtcSetObject20


////////////////////////////////////////////////////////////////////////////////
/// \brief  Set the context for the following TLVs
///
/// This function sets the current object instance ID from a CTC2.1 object
/// instance TLV.  The reply container is also set.
///
/// \param src      pointer to var descriptors in request
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void OamCtcSetObject21(const OamVarContainer BULK *src)
    {
    ctcCurObject.ctcVer = CtcObjVer21;
    OamCtcInstNumU      instNum;

    memcpy(&instNum.uNum, &src->value[0], src->length);
    ctcCurObject.instNum.sNum.portType = instNum.sNum.portType;
    ctcCurObject.instNum.sNum.slotNum = instNum.sNum.slotNum;
    ctcCurObject.instNum.sNum.portNum = OAM_NTOHS(instNum.sNum.portNum);
    
	cmsLog_notice("uNum=0x%x", ctcCurObject.instNum.uNum); 
    ctcCurObject.objType = (OamCtcObjType)OAM_NTOHS(src->leaf);
    switch (OAM_NTOHS(src->leaf))
        {
        case OamCtcObjPort:
            cmsLog_notice("portNum=0x%x", ctcCurObject.instNum.sNum.portNum);
            OamContextReset ((U8)(ctcCurObject.instNum.sNum.portNum), 0);
            break;

        case OamCtcObjLlid:
            OamContextReset (0, (U8)ContGetU32(src));
            break;

        default:
            break;
        }
    } // OamCtcSetObject21


////////////////////////////////////////////////////////////////////////////////
/// \brief  Is the current object the all ports object?
///
/// \return TRUE if the current object is the all ports object
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL OamCtcIsObjAllPorts(void)
    {
    return (ctcCurObject.instNum.sNum.portNum == OamCtcAllUniPorts) &&
           (ctcCurObject.instNum.uNum != OamCtcOnuInstNum);
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Reset the context for the following TLVs to default
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamCtcClearObject(void)
    {
    ctcCurObject.objType = OamCtcObjOnu;
    ctcCurObject.instNum.uNum = OamCtcOnuInstNum;
    OamContextReset (0, 0);
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Set the context for the following TLVs
///
/// This function sets the current object instance ID from a object instance
/// TLV.  The reply container is also set.  Calling this function with NULL
/// parameters resets the current instance to the default which is the EPON
/// port.
///
/// \param src      pointer to var descriptors in request
/// \param cont     pointer to var containers in reply
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamCtcSetObject (const OamVarContainer BULK *src,
                      OamVarContainer BULK *cont)
    {
    cont->length = src->length;
    memcpy(&cont->value[0],&src->value[0],cont->length);
    switch (src->branch)
        {
        case OamCtcBranchObjInst:
            OamCtcSetObject20(src);
            break;

        case OamCtcBranchObjInst21:
            OamCtcSetObject21(src);
            break;

        default:
            break;
        }
    } // OamCtcSetObject



////////////////////////////////////////////////////////////////////////////////
/// OamCtcGetObject - Get the current object ID
///
/// This function returns an object identification based on the last object
/// instance TLV that was received.  The returned object ID may not be required
/// for all messages.
///
 // Parameters:
/// \param src      Variable descriptor
///
/// \return
/// Pointer to corresponding object ID
////////////////////////////////////////////////////////////////////////////////
//extern
OamCtcObject BULK * OamCtcGetObject (OamVarDesc BULK* src)
    {
    UNUSED (src);

    return &ctcCurObject;
    } // OamCtcGetObject



////////////////////////////////////////////////////////////////////////////////
/// OamCtcCreateObject - Create a object instance TLV
///
/// This function creates an object identification based on the CTC
/// standard version,
/// If the CTC standard is CTC2.1, creates the an object identificatio in
/// new management
/// format, else, creates the an object identificatio in old management format.
///
/// Parameters:
/// \param cont     pointer to var containers in reply
/// \param type     object type
/// \param obj      port number
///
/// \return
/// Pointer to the next container
///////////////////////////////////////////////////////////////////////////////
//extern
OamVarContainer BULK * OamCtcCreateObject (OamVarContainer BULK *cont,
                                           OamCtcObjType type,
                                           OamCtcObj obj)
    {
    switch (ctcCurObject.ctcVer)
        {
        case CtcObjVer20:
            cont->branch = OamCtcBranchObjInst;
            cont->leaf = OAM_HTONS(type);
            cont->length = 1;
            cont->value[0] = obj & 0xFF;
            break;

        case CtcObjVer21:
            {
            OamCtcInstNumS BULK * BULK instPtr =
                                (OamCtcInstNumS BULK*)&cont->value[0];
            cont->branch = OamCtcBranchObjInst21;
            cont->leaf = OAM_HTONS(type);
            cont->length = sizeof(OamCtcInstNumS);
            instPtr->portType = ctcCurObject.instNum.sNum.portType;
            instPtr->slotNum  = ctcCurObject.instNum.sNum.slotNum;
            instPtr->portNum  = OAM_HTONS((obj&0xFF00)|(obj & 0xFF));
            }
            break;

        default:
            cont->branch = OamCtcBranchTermination;
            break;
        }
    StreamSkip(&oamParser.reply, OamContSize(cont));

    return NextCont (cont);
    } // OamCtcCreateObject

// End of File CtcUtils.c


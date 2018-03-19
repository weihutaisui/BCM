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
/// \file OntmOamUtil.c
/// \brief Utilities for handling OAM messages
///
////////////////////////////////////////////////////////////////////////////////


#include <string.h>
#include "Build.h"
#include "Teknovus.h"
#include "OamUtil.h"
#include "EponDevInfo.h"
#include "Stream.h"
#include "TkOamMem.h"
#include "ApiResLayer.h"

#include "cms_log.h"

static OamContext curContext;

OamParserState oamParser;


//lint --e{64} Type mismatch
/// maps StatId to OAM attribute
OamExtAttrLeaf const CODE oamAttrToStatId[StatIdNumStats] =
    {
    // Rx stats
    OamAttrMacOctetsRxOk,       // StatIdBytesRx,

    OamAttrMacFramesRxOk,       // StatIdFramesRx,
    OamExtAttrRxUnicastFrames,  // StatIdUnicastFramesRx
    OamAttrMacMcastFramesRxOk,  // StatIdMcastFramesRx,
    OamAttrMacBcastFramesRxOk,  // StatIdBcastFramesRx,

    OamAttrMacFcsErr,           // StatIdFcsErr,
    OamAttrOamEmulCrc8Err,      // StatIdCrc8Err
    OamAttrPhySymbolErrDuringCarrier,   // StatIdLineCodeErr

    OamExtAttrRxFrameTooShort,  // StatIdFrameTooShort,
    OamAttrMacFrameTooLong,     // StatIdFrameTooLong,
    OamAttrMacInRangeLenErr,    // StatIdInRangeLenErr,

    OamAttrMacOutOfRangeLenErr, // StatIdOutRangeLenErr,
    OamAttrMacAlignErr, // StatIdAlignErr

    // bin sizes available on Ethernet ports only
    OamExtAttrRxFrame64, // StatIdRx64Bytes,
    OamExtAttrRxFrame65_127, // StatIdRx65_127Bytes,
    OamExtAttrRxFrame128_255, // StatIdRx128_255Bytes,
    OamExtAttrRxFrame256_511, // StatIdRx256_511Bytes,
    OamExtAttrRxFrame512_1023, // StatIdRx512_1023Bytes,
    OamExtAttrRxFrame1024_1518, // StatIdRx1024_1518Bytes,
    OamExtAttrRxFrame1519Plus, // StatIdRx1519PlusBytes,

    OamExtAttrRxFramesDropped,
    OamExtAttrRxBytesDropped,
    OamExtAttrRxBytesDelayed,
    OamExtAttrRxDelay,
    OamAttrLeafIllegal,         // StatIdRxDelayThresh Unused $$$FEB16
    OamAttrMacCtrlPauseRx,          // StatIdRxPauseFrames,
    OamAttrLeafIllegal,          // StatIdRxControlFrames,

    OamAttrOamLocalErrFrameSecsEvent, //StatIdErrFrames,
    OamAttrOamLocalErrFramePeriodEvent, //StatIdErrFramePeriods,
    OamAttrOamLocalErrFrSecSumEvent,     //StatIdErrFrameSummary,

    // Tx stats
    OamAttrMacOctetsTxOk,           // StatIdBytesTx,
    OamAttrMacFramesTxOk,           // StatIdFramesTx,
    OamExtAttrTxUnicastFrames,      // StatIdUnicastTx
    OamAttrMacMcastFramesTxOk,      // StatIdMcastFramesTx,
    OamAttrMacBcastFramesTxOk,      // StatIdBcastFramesTx,

    OamAttrMacSingleCollFrames,     // StatIdSingleColl,
    OamAttrMacMultipleCollFrames,   // StatIdMultiColl,
    OamAttrMacLateCollisions,       // StatIdLateColl,
    OamAttrMacExcessiveCollisions,  // StatIdFrAbortXsColl,

    // bin sizes available on Ethernet ports only
    OamExtAttrTxFrame64,            // StatIdTx64Bytes,
    OamExtAttrTxFrame65_127,        // StatIdTx65_127Bytes,
    OamExtAttrTxFrame128_255,       // StatIdTx128_255Bytes,
    OamExtAttrTxFrame256_511,       // StatIdTx256_511Bytes,
    OamExtAttrTxFrame512_1023,      // StatIdTx512_1023Bytes,
    OamExtAttrTxFrame1024_1518,     // StatIdTx1024_1518Bytes,
    OamExtAttrTxFrame1519Plus,      // StatIdTx1519PlusBytes,

    OamExtAttrTxFramesDropped,
    OamExtAttrTxBytesDropped,        // dropped in queue, that is
    OamExtAttrTxBytesDelayed,
    OamExtAttrTxDelay,
    OamAttrLeafIllegal,         // StatIdTxDelayThresh Unused $$$FEB16
    OamExtAttrTxBytesUnused,         // granted but not sent
    OamAttrMacCtrlPauseTx,           // StatIdTxPauseFrames,
    OamAttrLeafIllegal,          // StatIdTxControlFrames,
    OamAttrLeafIllegal,          // StatIdTxDeferredFrames,
    OamAttrLeafIllegal,          // StatIdTxExcessiveDeferralFrames,


    OamAttrMpcpMACCtrlFramesTx,      // StatIdMpcpMACCtrlFramesTx,
    OamAttrMpcpMACCtrlFramesRx,      // StatIdMpcpMACCtrlFramesRx,
    OamAttrMpcpTxRegAck,             // StatIdMpcpTxRegAck,
    OamAttrMpcpTxRegRequest,         // StatIdMpcpTxRegRequest,
    OamAttrMpcpTxReport,             // StatIdMpcpTxReport,
    OamAttrMpcpRxGate,               // StatIdMpcpRxGate,
    OamAttrMpcpRxRegister            // StatIdMpcpRxRegister,
    };


////////////////////////////////////////////////////////////////////////////////
/// OamAttrToStatId:  Convert OAM attribute code to StatId value
///
 // Parameters:
/// \param attr     OAM attribute value to convert
///
/// \return
/// Equivalent StatId value, or  StatIdNumStats if no match
////////////////////////////////////////////////////////////////////////////////
//extern
StatId OamAttrToStatId (OamExtAttrLeaf attr)
    {
    StatId FAST stat;

    for (stat = StatIdRxFirst; stat < StatIdNumStats; ++stat)
        {
        if (oamAttrToStatId[stat] == attr)
            {
            break;
            }
        }

    return stat;
    } // OamAttrToStatIdEth


////////////////////////////////////////////////////////////////////////////////
/// StatIdToOamAttr:  Convert StatId value to OAM attribute code
///
 // Parameters:
/// \param stat     StatId value to convert
///
/// \return
/// Equivalent OamAttr value, or  0 if no match
////////////////////////////////////////////////////////////////////////////////
//extern
OamExtAttrLeaf StatIdToOamAttr (StatId stat)
    {
    return oamAttrToStatId[stat];
    } // StatIdToOamAttr


////////////////////////////////////////////////////////////////////////////////
//extern
void OamHeaderFill(LinkIndex link)
    {
    StreamInit(&oamParser.reply, (U8 *)&TxFrame->type);

    StreamWriteU16(&oamParser.reply, EthertypeSlowProt);
    StreamWriteU8(&oamParser.reply, OamSlowProtocolSubtype);
    StreamWriteU16(&oamParser.reply, OamGetMsgFlags(link));

    oamParser.dest.pift = PortIfPon;
    oamParser.dest.inst = link;
    oamParser.link = link;
    }


////////////////////////////////////////////////////////////////////////////////
//extern
void OamVendorFill(LinkIndex link, const IeeeOui* oui)
    {
    OamHeaderFill(link);
    StreamWriteU8(&oamParser.reply, OamOpVendorOui);
    StreamWriteOui(&oamParser.reply, oui);
    }


////////////////////////////////////////////////////////////////////////////////
//extern
void OamTransmit(void)
    {
    U16 size = (U16)StreamLengthInBytes(&oamParser.reply);

    switch (oamParser.dest.pift)
        {
        case PortIfPon:
            OamSendToLink(oamParser.dest.inst, size);
            break;

        default:
            cmsLog_notice("unknown oam destination");
            break;
        }
    }


////////////////////////////////////////////////////////////////////////////////
/// OamContSize:  Returns size required for container (including overhead)
///
 // Parameters:
/// \param cont     Container to measure
///
/// \return
/// Size in bytes of cont
////////////////////////////////////////////////////////////////////////////////
//extern
U8 OamContSize (OamVarContainer const BULK* cont)
    {
    return sizeof(OamVarDesc) + sizeof(cont->length) +
        OamContValLen(cont->length);
    } // OamContSize


////////////////////////////////////////////////////////////////////////////////
/// \brief  Converts between OAM length and actual length
///
/// \param length       Length to convert
///
/// \return
/// Converted length
////////////////////////////////////////////////////////////////////////////////
//extern
U8 OamContValLen(U8 length)
    {
    if (length >= OamVarErrReserved)
        {
        return 0;
        }
    else if (length == 0)
        {
        return OamMaxTlvLength;
        }
    else
        {
        return length;
        }
    } // OamContValLen


////////////////////////////////////////////////////////////////////////////////
/// CreatCont:  Create oam container
///
 // Parameters:
/// \param src      pointer to var descriptors in request
/// \param cont     pointer to var containers in reply///
/// \return
///         None
////////////////////////////////////////////////////////////////////////////////
//extern
void CreatCont (OamVarDesc const BULK* src, OamVarContainer BULK* cont)
    {
    cont->branch = src->branch;
    cont->leaf   = src->leaf;
    } // CreatCont


//////////////////////////////////////////////////////////////////////////////////
///// NextCont:  Return location of next container in OAM PDU
/////
// // Parameters:
///// \param cont Current container in PDU
/////
///// \return
///// Next container in PDU after cont
//////////////////////////////////////////////////////////////////////////////////
////extern
//OamVarContainer BULK* NextCont (OamVarContainer const BULK* cont)
//    {
//    return (OamVarContainer BULK*)((U8 BULK*)cont + OamContSize(cont));
//    } // NextCont


////////////////////////////////////////////////////////////////////////////////
/// OamTerminateBranch - This function terminates a response set
///
/// This function sets the terminal container value and returns the size of the
/// final container.
///
 // Parameters:
///
/// \return
/// Size of terminal container
////////////////////////////////////////////////////////////////////////////////
//extern
U16 OamTerminateBranch (OamVarContainer BULK *cont)
    {
    cont->branch = OamBranchTermination;
    cont->leaf = 0;
    cont->length = 0;
    return sizeof(OamVarDesc) + sizeof (cont->length);
    } // OamTerminateBranch



////////////////////////////////////////////////////////////////////////////////
/// ContGetU8:  Gets a U8 from a container
///
 // Parameters:
/// \param cont     OAM container holding integer
///
/// \return
/// Integer value in container as U8
////////////////////////////////////////////////////////////////////////////////
//extern
U8 ContGetU8 (OamVarContainer const BULK* cont)
    {
    return cont->value[cont->length - 1];
    } // ContGetU8



////////////////////////////////////////////////////////////////////////////////
/// ContGetU16:  Gets a U16 from a container
///
 // Parameters:
/// \param cont     OAM container holding integer
///
/// \return
/// Integer value in container as U16
////////////////////////////////////////////////////////////////////////////////
//extern
U16 ContGetU16 (OamVarContainer const BULK* cont)
    {
    U16 FAST result;
    Stream BULK strm;

    StreamInit(&strm, (U8 *)cont->value);
    switch (cont->length)
        {
        case sizeof(U8) :
            result = cont->value[0];
            break;

        default :
            StreamSkip(&strm, cont->length - sizeof(U16));
            result = StreamReadU16(&strm);
            break;
        }

    return result;
    } // ContGetU16



////////////////////////////////////////////////////////////////////////////////
/// ContGetU32:  Gets a U32 from a container
///
 // Parameters:
/// \param cont     OAM container holding integer
///
/// \return
/// Integer value in container as U32
////////////////////////////////////////////////////////////////////////////////
//extern
U32 ContGetU32 (OamVarContainer const BULK* cont)
    {
    U32 FAST result;
    Stream BULK strm;

    StreamInit(&strm, (U8 *)cont->value);
    switch (cont->length)
        {
        case sizeof(U8) :
            result = cont->value[0];
            break;

        case sizeof(U16) :
            result = StreamReadU16(&strm);
            break;

        default :
            StreamSkip(&strm, cont->length - sizeof(U32));
            result = StreamReadU32(&strm);
            break;
        }

    return result;
    } // ContGetU32


#if UNCALLED
////////////////////////////////////////////////////////////////////////////////
/// ContGetU64:  Gets a U64 from a container
///
 // Parameters:
/// \param cont     OAM container holding integer
/// \param val      Resulting U64 from container
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void ContGetU64 (OamVarContainer BULK* cont, U64 BULK* val)
    {
    U8 BULK* FAST ptr;
    U8 FAST len;

    if (cont->length < sizeof(U64))
        {
        len = cont->length;
        ptr = cont->value;
        memset (val, 0, sizeof(U64) - len);
        val += (sizeof(U64) - len);
        }
    else
        {
        len = sizeof(64);
        ptr = cont->value + cont->length - sizeof(U64);
        }

    memcpy (val, ptr, len);  //lint !e670 possible access beyond end
    } // ContGetU64
#endif


////////////////////////////////////////////////////////////////////////////////
/// ContPutU8:  Put U8 into OAM container
///
 // Parameters:
/// \param cont     Container to hold U64
/// \param val      Value to put into container
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void ContPutU8 (OamVarContainer BULK* cont, U8 val)
    {
    cont->length = sizeof(U8);
    cont->value[0] = val;
    } // ContPutU8



////////////////////////////////////////////////////////////////////////////////
/// ContPutU16:  Put U16 into OAM container
///
 // Parameters:
/// \param cont     Container to hold U64
/// \param val      Value to put into container
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void ContPutU16 (OamVarContainer BULK* cont, U16 val)
    {
    Stream FAST strm;

    StreamInit(&strm, cont->value);
    StreamWriteU16(&strm, val);
    cont->length = sizeof(U16);
    }



////////////////////////////////////////////////////////////////////////////////
/// ContPutU32:  Put U32 into OAM container
///
 // Parameters:
/// \param cont     Container to hold U64
/// \param val      Value to put into container
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void ContPutU32 (OamVarContainer BULK* cont, U32 val)
    {
    Stream FAST strm;

    StreamInit(&strm, cont->value);
    StreamWriteU32(&strm, val);
    cont->length = sizeof(U32);
    } // ContPutU32


#if UNCALLED
////////////////////////////////////////////////////////////////////////////////
/// ContPutU64:  Put U64 into OAM container
///
 // Parameters:
/// \param cont     Container to hold U64
/// \param val      Value to put into container
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void ContPutU64 (OamVarContainer BULK* cont, U64 const BULK* val)
    {
    cont->length = sizeof(U64);
    memcpy (cont->value, val->byte, sizeof(U64)); //lint !e419 apparent overrun
    } // ContPutU64
#endif


////////////////////////////////////////////////////////////////////////////////
/// \brief  Copy an OAM container
///
/// \param dest Destination OAM container
/// \param src  Source OAM container
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
void OamContCopy(OamVarContainer BULK* dest, const OamVarContainer BULK* src)
    {
    dest->length = src->length;
    memcpy(dest->value, src->value, dest->length);
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Sets current OAM context
///
/// \param  src     OAM name binding describing new context
/// \param  reply   OAM container for reply
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamTekContextSet(OamVarContainer const BULK* src,
                      OamVarContainer BULK* reply)
    {
    switch (OAM_NTOHS(src->leaf))
        {
        case OamNameMacName :
            {
            U8 BULK limit;

            curContext.type = OamCtxtLink;
            curContext.link = ContGetU8(src);
            limit = (curContext.port == 0) ? TkOnuNumTxLlids : 1;

            if (curContext.port >= limit)
                {
                curContext.link = 0;
                reply->length = OamVarErrAttrOverflow;
                }
            else
                {
                reply->length = src->length;
                memcpy (reply->value, src->value, reply->length);
                }

            }
            break;

        case OamNamePhyName :
            curContext.type = OamCtxtPort;
            curContext.port = ContGetU8(src);

            if (!PortIsValid(curContext.port))
                {
                curContext.port = 0;
                reply->length = OamVarErrAttrOverflow;
                }
            else
                {
                reply->length = src->length;
                memcpy (reply->value, src->value, reply->length);
                }
            break;

        case OamNameQueueName:
            {
            OamNameQueue BULK * oamCtxt = (OamNameQueue BULK *)src->value;
            if (!PortIsValid(curContext.port))
                {
                curContext.port = 0;
                reply->length = OamVarErrAttrOverflow;
                }
            else
                {
                curContext.port = (U8) oamCtxt->port;
                curContext.queue = (TkOnuQueue) oamCtxt->queue;

                if (curContext.port != 0)
                    { // Queue associated with UNI Ports being addressed
                    curContext.link = 0;
                    if (!PortQueueIsValid(curContext.port - 1, curContext.queue))
                        { //error
                        curContext.port = 0;
                        reply->length = OamVarErrAttrOverflow;
                        break;
                        }
                    // this is a queue context
                    curContext.type = OamCtxtPortQueue;
                    }
                else
                    { // Queue associated with EPON Port being addressed
                    curContext.link = (LinkIndex) oamCtxt->link;
                    if (!LinkQueueIsValid(curContext.link, curContext.queue))
                        { //error
                        curContext.port = 0;
                        reply->length = OamVarErrAttrOverflow;
                        break;
                        }
                    // this is a queue context
                    curContext.type = OamCtxtLinkQueue;
                    }

                reply->length = src->length;
                memcpy (reply->value, src->value, reply->length);
                }
            }
            break;
#if OAM_UTIL_TO_BE_FINISHED
        case OamNameAllQueues:
            {
            OamNameQueue BULK * oamCtxt = (OamNameQueue BULK *)src->value;

            // this is a queue context
            curContext.type = OamCtxtQueue;

            // check if the context specified is valid
            if (((oamCtxt->port > 0) && (oamCtxt->link > 0)) ||
                    (oamCtxt->port >= 3) ||
                    (oamCtxt->link >= TkOnuNumTxLlids))
                {
                reply->length = OamVarErrAttrOverflow;
                break;
                }

            curContext.port = (U8) oamCtxt->port;
            curContext.link = (LinkIndex) oamCtxt->link;
            curContext.queue = (TkOnuQueue) oamCtxt->queue;

            reply->length = src->length;
            if(oamCtxt->queue == 0)
                {
                if (oamCtxt->port > 0)
                    { // port was specified
                    ((OamNameQueue BULK *)reply->value)->port = oamCtxt->port;
                    ((OamNameQueue BULK *)reply->value)->link = 0;
                    ((OamNameQueue BULK *)reply->value)->queue =
                                              PortNumDnQueues(curContext.port-1);

                    }
                else
                    { // link was specified
                    ((OamNameQueue BULK *)reply->value)->port = 0;
                    ((OamNameQueue BULK *)reply->value)->link = oamCtxt->link;
                    ((OamNameQueue BULK *)reply->value)->queue =
                                                LinkNumUpQueues(curContext.link);
                    }
                }
            else
                {
                memcpy (reply->value, src->value, reply->length);
                }

            }
            break;
#endif
        case OamExtNameBridge:
            curContext.type = OamCtxtBridge;
            curContext.bridge = ContGetU8(src);
            if (curContext.bridge >= MAX_UNI_PORTS)
                {
                curContext.bridge = 0;
                reply->length = OamVarErrAttrOverflow;
                }
            else
                {
                reply->length = src->length;
                memcpy (reply->value, src->value, reply->length);
                }
            break;

        case OamExtNameBridgePort:
            curContext.type = OamCtxtBridgePort;
            // TODO: Figure out the bridge number somehow
            // curContext.bridge = ContGetU8 ((OamVarContainer BULK*)src);
            curContext.port = ContGetU8(src);

            // need to find a way to verify the port number
            // Special OAM port reference (port 255) get all ports
            if ((curContext.port != AllPorts) && !PortIsValid(curContext.port))
                {
                curContext.port = 0;
                reply->length = OamVarErrAttrOverflow;
                }
            else
                {
                reply->length = src->length;
                memcpy (reply->value, src->value, reply->length);
                }
            break;

#if defined(TkOnuNumNetworkPons)
        case OamNameNetworkPon:
            curContext.type = OamCtxtNetworkPon;
            curContext.port = ContGetU8(src);
            if (curContext.port >= TkOnuNumNetworkPons)
                {
                curContext.port = 0;
                reply->length = OamVarErrAttrOverflow;
                }
            else
                {
                reply->length = src->length;
                memcpy (reply->value, src->value, reply->length);
                }
            break;
#endif
#if defined(TkOnuNumUserPons)
        case OamNameUserPon:
            curContext.type = OamCtxtUserPon;
            curContext.port = ContGetU8(src);
            if (curContext.port >= TkOnuNumUserPons)
                {
                curContext.port = 0;
                reply->length = OamVarErrAttrOverflow;
                }
            else
                {
                reply->length = src->length;
                memcpy (reply->value, src->value, reply->length);
                }
            break;
#endif
        case OamNameFlowDirection:
            curContext.flowDirection = ContGetU8(src);
            reply->length = src->length;
            memcpy (reply->value, src->value, reply->length);
            break;

        default :
            break;
        }

    } // OamContextSet

//extern
void OamContextIopSet( OamCtxtType type,
                       U8 port,
                       LinkIndex link,
                       TkOnuQueue queue,
                       U8 bridge)
{
    curContext.type = type;
    curContext.port = port;
    curContext.link = link;
    curContext.queue = queue;
    curContext.bridge = bridge;
}




////////////////////////////////////////////////////////////////////////////////
/// OamContextReset:  resets current OAM context
///
 // Parameters:
/// \param port     New current port
/// \param link     New default link
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamContextReset (U8 port, LinkIndex link)
    {
    curContext.type = OamCtxtLink;
    curContext.port = port;
    curContext.link = link;
    } // OamContextReset


////////////////////////////////////////////////////////////////////////////////
/// \brief  Clear the current OAM context
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
void OamContextClear(void)
    {
    // sets context type to invalid
    memset(&curContext, 0, sizeof(OamContext));
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Set the type of the current OAM context
///
/// \param type     Type of OAM context
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
void OamContextTypeSet(OamCtxtType type)
    {
    curContext.type = type;
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Set the port instance of the current OAM context
///
/// \param port     Port instance of OAM context
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
void OamContextPortSet(U8 port)
    {
    curContext.port = port;
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Set the link of the current OAM context
///
/// \param link     Link of OAM context
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
void OamContextLinkSet(LinkIndex link)
    {
    curContext.link = link;
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Set the queue of the current OAM context
///
/// \param queue    Queue of OAM context
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
void OamContextQueueSet(TkOnuQueue queue)
    {
    curContext.queue = queue;
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Set the bridge of the current OAM context
///
/// \param bridge   Bridge of OAM context
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
void OamContextBridgeSet(U8 bridge)
    {
    curContext.bridge = bridge;
    }


////////////////////////////////////////////////////////////////////////////////
/// OamContextType:  Gets type of OAM context (port or link)
///
 // Parameters:
/// \param None
///
/// \return
/// Type of current context
////////////////////////////////////////////////////////////////////////////////
//extern
OamCtxtType OamContextType (void)
    {
    return curContext.type;
    } // OamContextType



////////////////////////////////////////////////////////////////////////////////
/// OamContextPort:  Gets current port for OAM context
///
 // Parameters:
/// \param None
///
/// \return
/// Current port in context
////////////////////////////////////////////////////////////////////////////////
//extern
U8 OamContextPort (void)
    {
    return curContext.port;
    } // OamContextPort


////////////////////////////////////////////////////////////////////////////////
/// OamContextLink:  Gets current link for OAM context
///
 // Parameters:
/// \param None
///
/// \return
/// Get the current link context
////////////////////////////////////////////////////////////////////////////////
//extern
LinkIndex OamContextLink (void)
    {
    return curContext.link;
    } // OamContextLink




////////////////////////////////////////////////////////////////////////////////
/// OamContextQueue:  Gets current queue for OAM context
///
 // Parameters:
/// \param None
///
/// \return
/// Get the current link context
////////////////////////////////////////////////////////////////////////////////
//extern
TkOnuQueue OamContextQueue (void)
    {
    return curContext.queue;
    } // OamContextLink


////////////////////////////////////////////////////////////////////////////////
/// OamContextFlowDirection:  Gets current flow direction for OAM context
///
 // Parameters:
/// \param None
///
/// \return
/// Get the current flow direction
////////////////////////////////////////////////////////////////////////////////
//extern
OamFlowDirection OamContextFlowDirection (void)
    {
    return curContext.flowDirection;
    } // OamContextFlowDirection


////////////////////////////////////////////////////////////////////////////////
/// OamContextGenericPort:  Gets current port for OAM context
///
 // Parameters:
/// \param None
///
/// \return
/// Current port in context
////////////////////////////////////////////////////////////////////////////////
//extern
Port OamContextGenericPort (void)
    {
    Port port;

    switch (curContext.type)
        {
/******************************************************************************
 ***********janeqiu: implementation incomplete*********************************
#if (PERS_VER == 2)
        case OamCtxtPort :
            port = OntDirPortGet (curContext.port);
            break;
#endif
*******************************************************************************/

        case OamCtxtLink:
            port.pift = PortIfLink;
            port.inst = curContext.link;
            break;

        case OamCtxtBridge:
            port.pift = PortIfBridge;
            port.inst = curContext.bridge;
            break;

        default:
            port.pift = PortIfNone;
            break;
        }
    return port;
    } // OamContextGenericPort


////////////////////////////////////////////////////////////////////////////////
/// OamContextBridge:  Gets current Bridge for OAM context
///
 // Parameters:
/// \param None
///
/// \return
/// Get the current Bridge context
////////////////////////////////////////////////////////////////////////////////
//extern
U8 OamContextBridge (void)
    {
    return curContext.bridge;
    } // OamContextBridge


// end OamUtil.c

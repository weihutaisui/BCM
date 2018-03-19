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


#if !defined(OamUtil_h)
#define OamUtil_h
////////////////////////////////////////////////////////////////////////////////
/// \file OamUtil.h
/// \brief Utilities for handling OAM messages
///
////////////////////////////////////////////////////////////////////////////////

#if defined(__cplusplus)
extern "C" {
#endif

#include "Teknovus.h"
#include "Oam.h"
#include "Stream.h"
#include "bcm_epon_cfg.h"
#include "PortCapability.h"
#include "Queue.h"

#ifndef OAM_UTIL_TO_BE_FINISHED
#define OAM_UTIL_TO_BE_FINISHED 1
#endif


#if !defined(OAM_FULLSUPPORT)
#define OAM_FULLSUPPORT 1
#endif


#if !LINKS_32
#define OnuHostIfPhyIfMsk   0xF0
#define OnuHostIfPhyIfSft   4
#define OnuHostIfLinkMsk    0x0F

//
// The upper nibble contains the physical interface as defined in the
// enumeration OnuHostInterface.
//
// The lower nibble contains the link
//
typedef U8 OamSource;

#define OnuHostIfGetInterface(source)   ((source) >> OnuHostIfPhyIfSft)
#define OnuHostIfGetInstance(source)    ((source) & OnuHostIfLinkMsk)
#define OnuHostIfMakeOamSource(inf, link) \
        (((inf) << OnuHostIfPhyIfSft) | ((link) & OnuHostIfLinkMsk))

typedef enum
    {
    OnuHostIfEpon   = 0,
    OnuHostIfUni    = 1,
    OnuHostIfI2c    = 2,
    OnuHostIfSpi    = 3,

    OnuHostIfMaxInterface
    } OnuHostInterface;

#define OnuOamPass                      0x01
#define OnuOamNotPass                   0x00

#define OamReservedFlagShift            8
#define OamReservedFlagMask             0xFF00
#define OamFlagMask                     0x00FF

#define OamFlagLinkMask                 0xF8
#define OamFlagLinkShift                3

#define OamFlagSrcIfMask                0x06
#define OamFlagSrcIfShift               1
#endif


typedef enum
    {
    OamCtxtInvalid,
    OamCtxtOnu,
    OamCtxtPort,
    OamCtxtLink,
    OamCtxtQueue,
    OamCtxtLinkQueue,
    OamCtxtPortQueue,
    OamCtxtBridge,
    OamCtxtBridgePort,
    OamCtxtNetworkPon,
    OamCtxtUserPort,
    OamCtxtUserPon,
    OamCtxtMulticastLink
    } OamCtxtType;



typedef struct
    {
    U8              port;
    LinkIndex       link;
    TkOnuQueue      queue;
    } OamContextValue;



typedef struct
    {
    OamCtxtType      type;
    U8               port;
    LinkIndex        link;
    TkOnuQueue       queue;
    U8               bridge;
    U8               TkOnuQueue;
    OamFlowDirection flowDirection;
    } OamContext;


typedef struct
    {
    Stream      src;
    Stream      reply;
    U8 *        endReply;
    U16         curOffset;
    U16         seqNum;
    BOOL        isLastItem;
    Port        dest;
    LinkIndex   link;
    } OamParserState;

extern OamParserState oamParser;


extern BOOL oamHbPause[TkOnuNumTxLlids];


#define AllPorts    0xFF


#define OamReplyFull(cont)  \
    ((U16)PointerDiff(oamParser.endReply, cont) < OamVarContMaxLength)


////////////////////////////////////////////////////////////////////////////////
/// \brief  Return location of next container in OAM PDU
///
/// \param  cont    Current container in PDU
///
/// \return Next container in PDU after cont
////////////////////////////////////////////////////////////////////////////////
#define NextCont(cont)  \
    ((OamVarContainer BULK*)((U8 BULK*)cont + OamContSize(cont)))


////////////////////////////////////////////////////////////////////////////////
/// OamAttrToStatId:  Convert OAM attribute code to StatId value
///
 // Parameters:
/// \param attr     OAM attribute value to convert
///
/// \return
/// Equivalent StatId value, or  StatIdNumStats if no match
////////////////////////////////////////////////////////////////////////////////
extern
StatId OamAttrToStatId (OamExtAttrLeaf attr);



////////////////////////////////////////////////////////////////////////////////
/// StatIdToOamAttr:  Convert StatId value to OAM attribute code
///
 // Parameters:
/// \param stat     StatId value to convert
///
/// \return
/// Equivalent OamAttr value, or  0 if no match
////////////////////////////////////////////////////////////////////////////////
extern
OamExtAttrLeaf StatIdToOamAttr (StatId stat);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Setup the OAM parser to send OAM on a link
///
/// \param link     Index of link to set up
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OamHeaderFill(LinkIndex link);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Setup the OAM parser to send a vendor specific OAM on a link
///
/// \param link     Index of link to set up
/// \param oui      OUI of vendor
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OamVendorFill(LinkIndex link, const IeeeOui* oui);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Transmit the current frame on the current interface
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OamTransmit(void);


////////////////////////////////////////////////////////////////////////////////
/// OamContSize:  Returns size required for container (including overhead)
///
 // Parameters:
/// \param cont     Container to measure
///
/// \return
/// Size in bytes of cont
////////////////////////////////////////////////////////////////////////////////
extern
U8 OamContSize (OamVarContainer const BULK* cont);



////////////////////////////////////////////////////////////////////////////////
/// \brief  Converts between OAM length and actual length
///
/// \param length       Length to convert
///
/// \return
/// Converted length
////////////////////////////////////////////////////////////////////////////////
extern
U8 OamContValLen(U8 length);



////////////////////////////////////////////////////////////////////////////////
/// CreatCont:  Create oam container
///
 // Parameters:
/// \param src      pointer to var descriptors in request
/// \param cont     pointer to var containers in reply///
/// \return
///         None
////////////////////////////////////////////////////////////////////////////////
extern
void CreatCont (OamVarDesc const BULK* src, OamVarContainer BULK* cont);



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
extern
U16 OamTerminateBranch (OamVarContainer BULK *cont);



////////////////////////////////////////////////////////////////////////////////
/// ContGetU8:  Gets a U8 from a container
///
 // Parameters:
/// \param cont     OAM container holding integer
///
/// \return
/// Integer value in container as U8
////////////////////////////////////////////////////////////////////////////////
extern
U8 ContGetU8 (OamVarContainer const BULK* cont);



////////////////////////////////////////////////////////////////////////////////
/// ContGetU16:  Gets a U16 from a container
///
 // Parameters:
/// \param cont     OAM container holding integer
///
/// \return
/// Integer value in container as U16
////////////////////////////////////////////////////////////////////////////////
extern
U16 ContGetU16 (OamVarContainer const BULK* cont);



////////////////////////////////////////////////////////////////////////////////
/// ContGetU32:  Gets a U32 from a container
///
 // Parameters:
/// \param cont     OAM container holding integer
///
/// \return
/// Integer value in container as U32
////////////////////////////////////////////////////////////////////////////////
extern
U32 ContGetU32 (OamVarContainer const BULK* cont);



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
extern
void ContGetU64 (OamVarContainer BULK* cont, U64 BULK* val);



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
extern
void ContPutU8 (OamVarContainer BULK* cont, U8 val);



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
extern
void ContPutU16 (OamVarContainer BULK* cont, U16 val);



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
extern
void ContPutU32 (OamVarContainer BULK* cont, U32 val);



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
extern
void ContPutU64 (OamVarContainer BULK* cont, U64 const BULK* val);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Copy an OAM container
///
/// \param dest Destination OAM container
/// \param src  Source OAM container
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OamContCopy(OamVarContainer BULK* dest, const OamVarContainer BULK* src);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Sets current OAM context
///
/// \param  src     OAM name binding describing new context
/// \param  reply   OAM container for reply
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void OamTekContextSet(OamVarContainer const BULK* src,
                      OamVarContainer BULK* reply);


extern
void OamContextIopSet( OamCtxtType type,
                       U8 port,
                       LinkIndex link,
                       TkOnuQueue queue,
                       U8 bridge);


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
extern
void OamContextReset (U8 port, LinkIndex link);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Clear the current OAM context
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OamContextClear(void);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Set the type of the current OAM context
///
/// \param type     Type of OAM context
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OamContextTypeSet(OamCtxtType type);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Set the port instance of the current OAM context
///
/// \param port     Port instance of OAM context
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OamContextPortSet(U8 port);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Set the link of the current OAM context
///
/// \param link     Link of OAM context
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OamContextLinkSet(LinkIndex link);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Set the queue of the current OAM context
///
/// \param queue    Queue of OAM context
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OamContextQueueSet(TkOnuQueue queue);


////////////////////////////////////////////////////////////////////////////////
/// OamContextFlowDirection:  Gets current flow direction for OAM context
///
 // Parameters:
/// \param None
///
/// \return
/// Get the current flow direction
////////////////////////////////////////////////////////////////////////////////
extern
OamFlowDirection OamContextFlowDirection (void);


////////////////////////////////////////////////////////////////////////////////
/// OamContextGenericPort:  Gets current port for OAM context
///
 // Parameters:
/// \param None
///
/// \return
/// Current port in context
////////////////////////////////////////////////////////////////////////////////
extern
Port OamContextGenericPort (void);

////////////////////////////////////////////////////////////////////////////////
/// \brief  Set the bridge of the current OAM context
///
/// \param bridge   Bridge of OAM context
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void OamContextBridgeSet(U8 bridge);


////////////////////////////////////////////////////////////////////////////////
/// OamContextType:  Gets type of OAM context (port or link)
///
 // Parameters:
/// \param None
///
/// \return
/// Type of current context
////////////////////////////////////////////////////////////////////////////////
extern
OamCtxtType OamContextType (void);



////////////////////////////////////////////////////////////////////////////////
/// OamContextPort:  Gets current port for OAM context
///
 // Parameters:
/// \param None
///
/// \return
/// Current port in context
////////////////////////////////////////////////////////////////////////////////
extern
U8 OamContextPort (void);


////////////////////////////////////////////////////////////////////////////////
/// OamContextLink:  Gets current link for OAM context
///
 // Parameters:
/// \param None
///
/// \return
/// Get the current link context
////////////////////////////////////////////////////////////////////////////////
extern
LinkIndex OamContextLink (void);



////////////////////////////////////////////////////////////////////////////////
/// OamContextQueue:  Gets current queue for OAM context
///
 // Parameters:
/// \param None
///
/// \return
/// Get the current queue context
////////////////////////////////////////////////////////////////////////////////
extern
TkOnuQueue OamContextQueue (void);



////////////////////////////////////////////////////////////////////////////////
/// OamContextBridge:  Gets current Bridge for OAM context
///
 // Parameters:
/// \param None
///
/// \return
/// Get the current Bridge context
////////////////////////////////////////////////////////////////////////////////
extern
U8 OamContextBridge (void);


#if defined(__cplusplus)
}
#endif


#endif // OamUtil.h

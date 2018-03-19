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


#if !defined(EthFrameUtil_h)
#define EthFrameUtil_h
////////////////////////////////////////////////////////////////////////////////
/// \file EthFrameUtil.h
/// \brief Useful functions for manipulating Ethernet frames
///
/// Contains routines for parsing and transmitting frames.
///
////////////////////////////////////////////////////////////////////////////////

#include "Build.h"
#include "Ethernet.h"
#include "Queue.h"
#include "Teknovus.h"
#include "Stream.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define MacAddressLen              6

#if OAM_CLI
typedef enum
    {
    MgmtFrameOam        = 1 << 0,
    MgmtFrameMaster     = 1 << 1,
    MgmtFrame802Dot1    = 1 << 2,
    MgmtFrameEapol      = 1 << 3
    } MgmtFrameType;

extern MgmtFrameType BULK upFrameDump;  //< Dump upstream
extern MgmtFrameType BULK dnFrameDump;  //< Dump downstream
extern MgmtFrameType BULK exFrameDump;  //< Dump extra (info/ping/etc.)
#endif


extern
const MacAddr CODE zeroMacAddr;


extern BOOL thowawayOamMsg;     // Default = FALSE: TRUE - don't enqueue if congested, FALSE - always enqueue

////////////////////////////////////////////////////////////////////////////////
/// AddToMacAddr:  Adds a value to a MAC address
///
 // Parameters:
/// \param macAddr  MAC address to which to add count
/// \param count    Amount by which to increment MAC address
///
/// \return
/// None.  macAddr is changed.
////////////////////////////////////////////////////////////////////////////////
extern
void AddToMacAddr (MacAddr* macAddr, U8 count);



////////////////////////////////////////////////////////////////////////////////
/// IncMacAddr:  Adds one to MAC address
///
 // Parameters:
/// \param macAddr  MAC address to increment
///
/// \return
/// None.  macAddr is changed.
////////////////////////////////////////////////////////////////////////////////
#define IncMacAddr(macAddr) AddToMacAddr(macAddr, 1)



////////////////////////////////////////////////////////////////////////////////
/// FrameVid:  Returns VID for given frame
///
 // Parameters:
/// \param frame    Pointer to Ethernet frame to parse
///
/// \return
/// VID in the frame, 0 if one doesn't exist
////////////////////////////////////////////////////////////////////////////////
extern
U16 FrameVid(EthernetFrame BULK* frame);



////////////////////////////////////////////////////////////////////////////////
/// \brief    Advance frame to ethertype
///
/// Handles offsets due to VLAN tags, SNAP headers, etc.
///
/// \param frame    Pointer to Ethernet frame to parse
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void DnFrameEthertype (Stream * frame);



////////////////////////////////////////////////////////////////////////////////
/// DnFrameStripStuffTag - Remove stuffed VLAN tag from frame if tagged
///
/// This function strips VLAN 0xFFF from the frame if the given frame is tagged.
/// Data is shifted from the end of the frame foward.  The size of the frame is
/// returned.
///
 // Parameters:
/// \param frame    Ethernet frame to modify
/// \param size     Size of the frame being passed
///
/// \return
/// Size of the frame after modification
////////////////////////////////////////////////////////////////////////////////
extern
U16 DnFrameStripStuffTag (EthernetFrame BULK* frame, U16 size);



////////////////////////////////////////////////////////////////////////////////
/// EthFrameAddVlanTag: Add VLAN Tag to Ethernet frame
///                     This routine assumes that the frame is in RxFrame and
///                     its length is stroed in rxFrameLength
///                     THIS ROUINE WILL AFFECT rxFrameLength TO REFLECT THE
///                     BYTES ADDED FOR THE VLAN TAG
///
 // Parameters:
/// \param vid      VID to add
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void EthFrameAddVlanTag (U16 vid);



////////////////////////////////////////////////////////////////////////////////
/// EthFrameDeleteVlanTag: Delete VLAN Tag from Ethernet frame
///                         This routine assumes that the frame is in RxFrame
///                         and its length is stroed in rxFrameLength
///                         THIS ROUINE WILL AFFECT rxFrameLength TO REFLECT
///                         THE BYTES REMOVED FROM THE VLAN TAG
///
 // Parameters:
/// \param vid      None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void EthFrameDeleteVlanTag (void);


////////////////////////////////////////////////////////////////////////////////
/// WaitUpQueueAny:  Wait for specified up queue to become empty
///
/// Max 10 msec; waits on TkOnuQUpProcEpon
///
 // Parameters:
/// \param q        Queue on which to wait
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
U8 WaitQueueEmpty (TkOnuQueue q);


////////////////////////////////////////////////////////////////////////////////
/// WaitDnQueue:  Wait for downstream queue to become empty
///
/// (Timer h/w has Max 10 msec)
///
 // Parameters:
/// \param q    Queue on which to wait
///
/// \return
/// 0 if not empty      1 if empty
////////////////////////////////////////////////////////////////////////////////
extern
U8 WaitDnQueue (TkOnuQueue q);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Send upstream frame
///
/// This function will fill in the provided MAC DA and the appropriate MAC SA
/// for the indicated link. The rest of the frame (starting with the ethertype)
/// should be filled by the caller.
///
/// \param  da      Destination Address for frame
/// \param  link    Link on which to transmit packet
/// \param  size    Number of bytes in packet (exclusive of DA + SA)
///
/// \return Actual number of bytes buffered
////////////////////////////////////////////////////////////////////////////////
extern
U16 FrameSendUp (const MacAddr* da, LinkIndex link, U16 size);



////////////////////////////////////////////////////////////////////////////////
/// FrameForwardUp:  Send rx buffer upstream
///
/// Note use of rx buffer, rather than the tx buffer used by the "Send"
/// routines. The frame is forwarded unchanged.
///
 // Parameters:
/// \param link     Link on which to send frame
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void FrameForwardUp (LinkIndex link);

////////////////////////////////////////////////////////////////////////////////
/// FrameSendDn:  Send downstream frame
///
/// Caller should fill in appropriate Ethertype before calling
///
 // Parameters:
/// \param da       Destination Address for frame
/// \param dest     Destination port(s) for frame
/// \param size     Number of bytes in packet (exclusive of Ethernet overhead)
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void FrameSendDn (const MacAddr* da, TkOnuQueue queue, U16 size);



////////////////////////////////////////////////////////////////////////////////
/// FrameForwardDn:  Send rx buffer downstream
///
/// Note use of rx buffer, rather than the tx buffer used by the "Send"
/// routines.  The frame is forwarded unchanged.
///
 // Parameters:
/// \param dest     Destination port(s) for frame
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void FrameForwardDn (TkOnuQueue queue);



////////////////////////////////////////////////////////////////////////////////
/// BufferUp:  Send specified buffer upstream
///
/// Note use of rx buffer, rather than the tx buffer used by the "Send"
/// routines. The frame is forwarded unchanged.
///
 // Parameters:
/// \param size     Number of bytes in packet (total)
///
/// thowawayOamMsg  TRUE - don't enqueue if congested, FALSE - always enqueue
///                 Here because of lack of code size for Tk3711
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void BufferUp (const EthernetFrame BULK* buffer, LinkIndex link, U16 size);



////////////////////////////////////////////////////////////////////////////////
/// BufferDn:  Send specified buffer downstream
///
 // Parameters:
/// \param buffer   Buffer to send
/// \param p        Port to send to
/// \param size     Number of bytes in packet (total)
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
extern
void BufferDn (const EthernetFrame BULK* buffer, Port p, U16 size);


#if OAM_CLI
////////////////////////////////////////////////////////////////////////////////
/// \brief  Print a frame on the PON port
///
/// The frame that is passed to the function should be either RxFrame or TxFrame
/// global buffers.
///
/// \param  buffer  Frame buffer to be printed
/// \param  link    Logical link ID
/// \param  size    Number of bytes in the frame
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void DumpPonFrame (const EthernetFrame BULK* buffer, LinkIndex link, U16 size);


////////////////////////////////////////////////////////////////////////////////
/// \brief  Print a frame on a UNI port
///
/// The frame that is passed to the function should be either RxFrame or TxFrame
/// global buffers.
///
/// \param  buffer  Frame buffer to be printed
/// \param  port    Logical port ID
/// \param  size    Number of bytes in the frame
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
extern
void DumpUniFrame (const EthernetFrame BULK* buffer,
                   TkOnuEthPort port,
                   U16 size);
#endif


////////////////////////////////////////////////////////////////////////////////
/// \brief  Adjust size for ethernet header and pad frame with zeroes
///
/// \param size     Size of the frame (excluding DA/SA)
///
/// \return
/// Size of the frame (including DA/SA)
////////////////////////////////////////////////////////////////////////////////
extern
U16 FixEthFrame(U16 size);


#if defined(__cplusplus)
}
#endif

#endif // EthFrameUtil.h

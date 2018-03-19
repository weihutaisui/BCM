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
/// \file EthFrameUtil.h
/// \brief Useful functions for manipulating Ethernet frames
///
/// Contains routines for parsing and transmitting frames.
///
////////////////////////////////////////////////////////////////////////////////

// generic includes
#include <string.h>     // memcpy
#include "Teknovus.h"

// base includes
#include "EthFrameUtil.h"
#include "Timer.h"

// interface includes
#include "Oam.h"

// platform includes
#include "TkOamCommon.h"
#include "PonManager.h"

#if OAM_CLI
MgmtFrameType BULK upFrameDump;
MgmtFrameType BULK dnFrameDump;
MgmtFrameType BULK exFrameDump;
#endif

#define rxCrc32Deleted FALSE
const MacAddr zeroMacAddr = { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00} }; /* lint !e708 */


BOOL throwawayOamMsg;   // Default = FALSE:
                        // TRUE - don't enqueue if congested,
                        // FALSE - always enqueue


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
//extern
void AddToMacAddr (MacAddr* macAddr, U8 count)
    {
    U8 FAST lsb;
    U8 FAST i = MacAddressLen - 1;

    lsb = macAddr->byte[i];
    macAddr->byte[i] += count;

    if(macAddr->byte[i] < lsb)
        { // overflow
        --i;
        do
            {
            macAddr->byte[i]++;
            if (macAddr->byte[i] != 0)
                { // no carry
                break;
                }
            }
        while(i-- != 0);
        }
    } // AddToMacAddr



////////////////////////////////////////////////////////////////////////////////
/// FrameVid:  Returns VID for given frame
///
 // Parameters:
/// \param frame    Pointer to Ethernet frame to parse
///
/// \return
/// VID in the frame, 0xFFFF if one doesn't exist
////////////////////////////////////////////////////////////////////////////////
//extern
U16 FrameVid(EthernetFrame BULK* frame)
    {
    U16             FAST result = 0xFFFF; //IgmpNoVid
    Stream s;

    StreamInit (&s, (U8 BULK *)&frame->type);
    if (StreamReadU16(&s) == EthertypeVlan)
        {
        result = StreamReadU16(&s);
        }

    return result;
    } // FrameVid



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
//extern
void DnFrameEthertype (Stream* frame)
    {
    // back up to Ethertype of frame
#if ETH_FRAME_UTIL_TO_BE_FINISHED
    Ethertype       FAST altvlan TkOnuGetAltVLanEtherType ();
    U8              FAST use_altvlan TkOnuGetDnUseAltVLanEtherType();
#endif

    StreamSkip(frame, sizeof(EthernetFrame) - sizeof(Ethertype));

    while (
           (StreamPeekU16(frame) == EthertypeVlan)
#if ETH_FRAME_UTIL_TO_BE_FINISHED
            || (use_altvlan && (*result == altvlan))
#endif
          )
        {
        StreamSkip(frame, sizeof(EthernetVlanData));
        }
    } // DnFrameEthertype



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
//extern
U16 DnFrameStripStuffTag (EthernetFrame BULK* frame, U16 size)
    {
    EthernetVlanData BULK * FAST vlan = (EthernetVlanData BULK *)&frame->type;

    if ((vlan->type == EthertypeVlan) &&
        (vlan->tag == 0xFFF))
        {
        size -= sizeof(EthernetVlanData);
        memmove (vlan, (vlan + 1), size - sizeof(EthernetVlanData));
        }
    return size;
    } // DnFrameStripStuffTag



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
//extern
void EthFrameAddVlanTag (U16 vid)
    {
    EthernetVlanData BULK * FAST tag =
        (EthernetVlanData BULK *)&(RxFrame->type);

    if (tag->type != EthertypeVlan)
        { // There isn't a Tag in this frame, make space for it.
        memmove ((tag + 1), tag, rxFrameLength - (2 * sizeof(MacAddr)));
        rxFrameLength += sizeof(EthernetVlanData);
        tag->type = EthertypeVlan;
        }

    tag->tag = vid;
    } // EthFrameAddVlanTag



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
//extern
void EthFrameDeleteVlanTag (void)
    {
    EthernetVlanData BULK * FAST tag;

    tag = (EthernetVlanData BULK *)&(((EthernetFrame BULK *)RxFrame)->type);

    while (tag->type == EthertypeVlan)
        { // There isn't a Tag in this frame, make space for it.
        memmove (tag, (tag + 1),
            rxFrameLength - ((2 * sizeof(MacAddr)) + sizeof(EthernetVlanData)));
        rxFrameLength -= sizeof(EthernetVlanData);
        }
    } // EthFrameDeleteVlanTag



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
//static
void BufferUp (const EthernetFrame BULK* buffer, LinkIndex link, U16 size)
    {
    MacAddr *da = (MacAddr *)&(buffer->da);

    TkOamEthSend (link, da, (U8 BULK*)buffer, size);

    } // BufferUp




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
//extern
U16 FrameSendUp (const MacAddr* da, LinkIndex link, U16 size)
    {
    // fill in Ethernet header
    memcpy (&TxFrame->da, da, sizeof(MacAddr));
    PonMgrGetMacForLink(link, &TxFrame->sa);

    size = FixEthFrame(size);

    BufferUp (TxFrame, link, size);

    return size;
    } // FrameSendUp



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
//extern
void FrameForwardUp (LinkIndex link)
    {    
    BufferUp ((EthernetFrame BULK*)RxFrame, link,
              rxFrameLength - (rxCrc32Deleted ? 0 : 4));
    } // FrameForwardUp


////////////////////////////////////////////////////////////////////////////////
/// BufferDn:  Send specified buffer downstream
///
 // Parameters:
/// \param buffer   Buffer to send
/// \param dest     Destination port(s) for frame
/// \param size     Number of bytes in packet (total)
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void BufferDn (const EthernetFrame BULK* buffer, Port p, U16 size)
    {
    //janeqiu: function should not be used in 6838 eponsdk
    return;
    } // BufferDn


#if 0 // Needed for downstream IP transmission
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
//extern
void FrameSendDn (const MacAddr* da, TkOnuDnDest dest, U16 size)
    {
    if (da)
        {
        // fill in Ethernet header
        memcpy (&TxFrame->da, da, sizeof(MacAddr));

        // OOPS! where does link come from???
        //TkOnuEthGetMac (link, &TxFrame->sa);
        }

    //size += sizeof(EthernetFrame);
    if (size < 60)
        {// finish out packet with zeros
        memset(((U8 BULK*)TxFrame) + size, 0, 60 - size);
        size = 60;
        }

    BufferDn (TxFrame, dest, size);
    } // FrameSendDn
#endif



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
//extern
void FrameForwardDn (TkOnuQueue queue)
    {
#if ETH_FRAME_UTIL_TO_BE_FINISHED
    // If the frame's CRC was stripped by the DLU, we need to modify the frame
    // length
    if (!rxCrc32Deleted)
        {
        rxFrameLength -= 4;
        }

    // rxFrameLength includes 4 bytes of Teknovus proprietary header starting
    // from rxPktBuffer which we don't want to forward
    if (dest & TkOnuDnPort1)
        {
        BufferDn (RxFrame, TkOnuDnPort1, rxFrameLength);
        }
    if (dest & TkOnuDnPort2)
        {
        BufferDn (RxFrame, TkOnuDnPort2, rxFrameLength);
        }
#else
    UNUSED (queue);
#endif
    } // FrameForwardDn



#if OAM_CLI
////////////////////////////////////////////////////////////////////////////////
/// \brief  Dump a frame to the CLI
///
/// \param  frame   Pointer to the frame
/// \param  size    Size of the frame
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void DumpFrame(const U8 BULK* frame, U16 size)
    {
    U16 FAST index;

    printf("%lX\n", OntmTimerPbiTimeGet());
    for (index = 0; index < size; ++index)
        {
        printf ("%02bX ", *frame++);
        if ((index % 16) == 15)
            {
            printf ("\n");
            }
        }
    printf ("\n\n");
    }


////////////////////////////////////////////////////////////////////////////////
//extern
void DumpPonFrame (const EthernetFrame BULK* buffer, LinkIndex link, U16 size)
    {
    if (buffer == TxFrame)
        {
        printf ("Tx Link: %bd\n", link);
        }
    else
        {
        printf(" Rx Link: %bd\n", link);    
        }

    DumpFrame((U8 BULK*)buffer, size);
    } // DumpOamFrame


////////////////////////////////////////////////////////////////////////////////
//extern
void DumpUniFrame (const EthernetFrame BULK* buffer,
                   TkOnuEthPort port,
                   U16 size)
    {
    if (buffer == TxFrame)
        {
        printf ("Tx Port: %bd\n", port);
        }
    else
        {
        printf("Rx Port: %bd\n", port); 
        }

    DumpFrame((U8 BULK*)buffer, size);
    } // DumpOamFrame
#endif


////////////////////////////////////////////////////////////////////////////////
//extern
U16 FixEthFrame(U16 size)
    {
    size += sizeof(MacAddr) * 2;
    if (size < 60)
        {// finish out packet with zeros
        memset (((unsigned char *) TxFrame) + size, 0, 60 - size);
        size = 60;
        }
    return size;
    } // FixEthFrame


// end EthFrameUtil.c

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


#if !defined(Ip_h)
#define Ip_h
////////////////////////////////////////////////////////////////////////////////
/// \file Ip.h
/// \brief Internet Protocol definitions
///
/// Useful structures and constants for dealing with IP frames
///
////////////////////////////////////////////////////////////////////////////////

#include "Teknovus.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum
    {
    IpProtocolV6HopByHop            = 0,
    IpProtocolIcmp                  = 1,
    IpProtocolIgmp                  = 2,
    IpProtocolGgp                   = 3,
    IpProtocolTcp                   = 6,
    IpProtocolEgp                   = 8,
    IpProtocolUdp                   = 17,
    IpProtocolV6OverV4              = 41,
    IpProtocolV6Routing             = 43,
    IpProtocolV6Fragment            = 44,
    IpProtocolRsvp                  = 46,
    IpProtocolIcmp6Mld              = 58,
    IpProtocolV6NoNext              = 59,
    IpProtocolV6DestOptions         = 60,
    IpProtocolOspf                  = 89,
    IpProtocolL2tp                  = 115
    } PACK IpProtocolType;

// link-local is fe80::/10 (top ten bits matter)
#define IpV6LinkLocalStart 0xfe800000000000000000000000000000
#define IpV6LinkLocalEnd   0xfebfffffffffffffffffffffffffffff

// site-local is fec0::/10 (top ten bits matter)
#define IpV6SiteLocalStart 0xfec00000000000000000000000000000
#define IpV6SiteLocalEnd   0xfeffffffffffffffffffffffffffffff

// multicast is ff00::/8 (top 8 bits matter)
#define IpV6MulticastStart 0xff000000000000000000000000000000
#define IpV6MulticastEnd   0xffffffffffffffffffffffffffffffff

// SSM range (ff3x::/32), as long as "x" is a valid scope value
#define IpV6SSMStart       0xff300000000000000000000000000000
#define IpV6SSMEnd         0xff3fffffffffffffffffffffffffffff

#define UdpProtocolType 0x0011  // UDP -IP protocol # 17

#define DhcpServerPortNum  0x0043      // UDP -DHCP Server Port #67
#define DhcpCliPortNum  0x0044      // UDP -DHCP Client Port #68

#define DhcpV6CliPortNum  0x0222    // UDP -DHCPv6 Client Port #546
#define DhcpV6ServerPortNum  0x0223    // UDP -DHCPv6 Server Port #547

#define IpV4Version      4
#define IpV6Version      6

typedef U32 IpAddr;

#define IpV6HdrLen       40
#define IpV6AddrLenBytes 16
#define IpV6AddrLenWords 8

#define IpV6HalfAddrLenBytes (IpV6AddrLenBytes/2)

#define IpV4AddrLenBytes 4

#if defined(__ghs__)

typedef struct
    {
    U8 addr[IpV4AddrLenBytes];
    } PACK IpV4Addr;

typedef struct
    {
    U8  addr[IpV6AddrLenBytes];
    } PACK IpV6Addr;

typedef struct
    {
    U32      verDsEcnFlowLbl;
    U16      payloadLen;
    U8       nxtHdr;
    U8       hopLimit;
    IpV6Addr src;
    IpV6Addr dst;
    } PACK IpV6Header;

#endif

typedef struct
    {
    U8  versionHdrLen;
    U8  tos;
    U16 length;
    U16 id;
    U16 fragmentOffset;
    U8  ttl;
    U8  protocol;
    U16 checksum;
    IpAddr sourceAddr;
    IpAddr destAddr;
    } PACK IpHeader;

typedef struct  {
    U16 srcUdpPort;
    U16 destUdpPort;
    U16 length;
    U16 checkSum;
    } PACK UdpHeader;

#define IpHdrLenMsk             0x0f
#define IpHdrLenSft             0
#define IpVersionMsk            0xf0
#define IpVersionSft            4

#define IpFragOffsetMsk         0x1fff
#define IpFragOffsetSft         0
#define IpFragDoNotFragment     0x4000
#define IpFragFragmentsFollow   0x2000


#if defined(__ghs__)

typedef union
    {
    IpHeader v4;
    IpV6Header v6;
    } PACK IpV4V6Header;


////////////////////////////////////////////////////////////////////////////////
// IPv6 Header defines
////////////////////////////////////////////////////////////////////////////////

// Put additional expected L4 protocol types at end to extend
// IpV6 parsing support for the new type. These types terminate
// IpV6 header chains, and become the last ext hdr in IpV6 Info.
// You must also add a case in IpV6ParseNextExtHdr() switch for
// new hdr type, and an IpV6GetUdpFrame() equivalent test method.
// All other undefined types are treated as illegal frames; they
// should not be encountered by SW, unless SW installs capture
// rules for them. Currently, we only capture UDP IpV6 frames.
typedef enum
    {
    // IPv6 Extension Header types:
    IpV6ExtHdrHopByHop  = 0,    // Hop-by-Hop ext header
    IpV6ExtHdrDestOpt   = 60,   // Destination Options ext header
    IpV6ExtHdrRouting   = 43,   // Routing ext header
    IpV6ExtHdrFragment  = 44,   // Frament ext header
    IpV6ExtHdrAuth      = 51,   // Authentication ext header
    IpV6ExtHdrESP       = 50,   // Encapsulating Security Payload ext header
    IpV6ExtHdrMobility  = 135,  // Mobility ext header
    IpV6ExtHdrNoNextHdr = 59,   // "No Next Header" sentinel value

    // Expected terminal L4 protocol / payload types:
    IpV6ExtHdrUDP       = 17,   // L4 UDP Protocol Type
    IpV6ExtHdrIcmp      = 58,   // ICMP header
    } PACK IpV6ExtHdrType;

#define IpV6MinExtHdrByteLen 8
#define IpV6ExtHdrLenToByteLen(extHdrLen) ((extHdrLen * 8) + 8)



////////////////////////////////////////////////////////////////////////////////
// IPV6 header types
////////////////////////////////////////////////////////////////////////////////

typedef struct
    {
    IpV6ExtHdrType nextHdr;  // An "IpV6ExtHdrType", or L4 protocol type
    U8             extHdrLen;// Hdr Len in 8-octet units, excl first 8 octets
    } PACK IpV6ExtHdrBase;

typedef struct
    {
    IpV6ExtHdrBase extHdrBase;

    U16 fragOffset   : 13;
    U16 reserved : 2;
    U16 more : 1;

    U32 identification;
    } PACK IpV6ExtHdrFrag;

typedef struct
    {
    U8 trafficClass1 : 4;  // Low nibble
    U8 version       : 4;  // High nibble == 6 for IPv6 Header

    U8 flowLabel1    : 4;
    U8 trafficClass2 : 4;

    U8 flowLabel2;
    U8 flowLabel3;

    U16 payloadLen;
    IpV6ExtHdrType nextHdr; // An "IpV6ExtHdrType", or L4 protocol type
    U8  hopLimit;

    IpV6Addr srcAddr;
    IpV6Addr destAddr;
    } PACK IpV6MainHdr;

#else

typedef union
    {
    U8 byte[16];
    U16 word[8];
    U32 dword[4];
    } PACK IpV6Addr;


typedef enum
    {
    HopByHop    = 0,  // IPv6 Hop-by-Hop Option
    Icmp        = 1,  // ICMP protocol
    Igmp        = 2,  // IGMP protocol
    IpOverIp    = 4,  // IP over IP
    Tcp         = 6,  // TCP protocol
    Udp         = 17, // UDP protocol
    IpV6        = 41, // IPv6 protocol
    IpV6Icmp    = 58, // IPv6 ICMP protocol
    IpV6NoNext  = 59  // IPv6 No Next Header (terminates a no upper layer frame)
    } PACK IpV6NextHdr; // see http://www.iana.org/assignments/protocol-numbers/


typedef enum
    {
    // IPv6 Extension Header types:
    IpV6ExtHdrHopByHop  = 0,    // Hop-by-Hop ext header
    IpV6ExtHdrRouting   = 43,   // Routing ext header
    IpV6ExtHdrFragment  = 44,   // Frament ext header
    IpV6ExtHdrESP       = 50,   // Encapsulating Security Payload ext header
    IpV6ExtHdrAuth      = 51,   // Authentication ext header
    IpV6ExtHdrNoNextHdr = 59,   // "No Next Header" sentinel value
    IpV6ExtHdrDestOpt   = 60,   // Destination Options ext header
    IpV6ExtHdrMobility  = 135,  // Mobility ext header
    
    IpV6ExtHdrUDP       = 17,   // L4 UDP Protocol Type
    IpV6ExtHdrIcmp      = 58    // IPv6 ICMP protocol
    
    } PACK IpV6ExtHdrType;


typedef struct
    {
    IpV6ExtHdrType  type;
    U8              length;
    U8              hdrData[2]; // at least 2 bytes guaranteed???
    } PACK IpV6ExtHeader;


typedef struct
    {
    U32            verTrfClassFlowLabel;
    U16            length; // payload length
    IpV6ExtHdrType nextHeader;
    U8             hopLimit;
    IpV6Addr       sourceAddr;
    IpV6Addr       destAddr;
    } PACK IpV6Header;

#endif // defined(__ghs__)


#if defined(__cplusplus)
}
#endif

#endif // Ip.h

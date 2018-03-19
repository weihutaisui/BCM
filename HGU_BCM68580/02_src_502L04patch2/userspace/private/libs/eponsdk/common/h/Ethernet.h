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


#if !defined(Ethernet_h)
#define Ethernet_h
/*
    \file Ethernet.h
    \brief Definitions for Ethernet frames
   
    Structures, constants, and types useful for dealing with Ethernet frames

*/

#if defined(__cplusplus)
extern "C" {
#endif

/* pack structures */
#if defined(CPU_ARCH_ARM) || defined(CPU_ARCH_X86)
#pragma pack (1)
#endif

#include "Teknovus.h"
#include "config.h"

#define EthMinFrameSize             64
#define EthMinFrameSizeWithoutCrc   (EthMinFrameSize - 4)

#define EthMaxFrameSize             1518
#define EthMaxFrameSizeWithoutCrc   (EthMaxFrameSize - 4)

// Offset in a frame to the ethertype (or first VID)
#define OffsetToEthertype 12

// Length of a single VLAN tag
#define VlanTagLen        4
/* Ethernet protocol type codes */

typedef enum
{
    EthertypeArp        = 0x0806,
    EthertypeEapol      = 0x888E,
    EthertypeEapolOld   = 0x8180,
    EthertypeLoopback   = 0x9000,
    EthertypeMpcp       = 0x8808,
    EthertypeOam        = 0x8809,
    EthertypeSlowProt   = 0x8809,
    EthertypePppoeDisc  = 0x8863,
    EthertypePppoeSess  = 0x8864,
    EthertypeRarp       = 0x8035,
    EthertypeCvlan      = 0x8100, // Also consider using GetVlanEtherTypeOptions
    EthertypeSvlan      = 0x88a8, // Also consider using GetVlanEtherTypeOptions
    EthertypeBtag       = 0x88a8, // Also consider using GetVlanEtherTypeOptions
    EthertypeIp         = 0x0800,
    EthertypeIpv6       = 0x86DD,
    EthertypeITag       = 0x88E7,
	EthertypePoEDisc    = 0x8863,	// pppoe discover
	EthertypePoESess    = 0x8864,	// pppoe session
    EthertypeVlan       = EthertypeCvlan, // for the ONU code    
    EthertypeLldp       = 0x88CC
} PACK Ethertype;

/* MAC address *//*
//defined in bcm_epon_xenu.h
typedef union
	{
	U8      byte[6];
	U16     word[3];
	U8      u8[6]; 
    U8 mac[6];
    struct
        {
        U16 hi;
        U32 low;
        }PACK lowHi;
	} PACK MacAddr;
*/

typedef union
{
    U8      ipv4Addr[6];        //keep same format as OAM packet. need to convert
    U8      ipv6Addr[16];
}McastIpAddr;

/* Basic Ethernet frame format */
typedef struct
	{
	MacAddr     da;
	MacAddr     sa;
#ifdef TK_NEED_MGNT_VLAN
	U16 		vlanType;
	U16 		vlanInfo;
#endif	
	U16         type;
	} PACK EthernetFrame;

/* VLAN tag */
typedef U16 VlanTag;
typedef U8  VlanCos;
typedef U16 VlanId;

#define VlanCosGet(tag)             ((VlanCos)((tag) >> 13))
#define VlanIdGet(tag)              ((VlanId)((tag) & 0xFFF))
#define VlanTagBuild(id, cos)       ((VlanTag)((id) | ((VlanTag)(cos) << 13)))
/* VLAN header */
typedef struct
    {
    U16           type;       /* EthertypeVlan */
    VlanTag       tag;        /* priority + CFI + VID */
    } PACK EthernetVlanData;


typedef struct 
	{
	MacAddr Dst;
	MacAddr Src;
	U16     VlanType;
	U16     Vlan;
	U16     Type;
	} PACK VlanTaggedEthernetFrame;


#define GET_VID(tci) (tci & 0xFFF)
#define GET_PBITS(tci) (tci >> 13)

#if defined(CPU_ARCH_ARM) || defined(CPU_ARCH_X86)
#pragma pack()
#endif

#if defined(__cplusplus)
}
#endif

#endif /* Ethernet.h */

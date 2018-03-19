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


#if !defined(McastTypes_h)
#define McastTypes_h
////////////////////////////////////////////////////////////////////////////////
/// \file McastTypes.h
/// \brief IP Multicast types
///
////////////////////////////////////////////////////////////////////////////////

#include "Teknovus.h"
#include "Ip.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define McastVlanNonVid              0
#define McastVlanGetVid(vlan)		 ((vlan) & 0xFFF)
#define McastVidLeftMask             52
#define McastVidRightMask            0


//##############################################################################
//          Protocol Related Macros, Structures and Enums
//##############################################################################

#define MaxLmqCount                  12
#define MaxRobustness                MaxLmqCount
#define DefaultLmqCount              (2) // per RFC 2236
#define DefaultRobustness            (2)
#define DefaultLmqInterval           (100/TkOsMsPerTick) // 1 second
#define DefaultLmqResponse           (80)
#define DefaultQueryInterval         (12500/TkOsMsPerTick) //125 second
#define McastlargeTimerValue         ((MaxLmqCount*255)+1)

typedef enum
	{
	IgmpMembershipQuery 	= 0x11,
	IgmpV1MembershipReport 	= 0x12,
	IgmpV2MembershipReport 	= 0x16,
	IgmpV2LeaveGroup		= 0x17,
	IgmpV3MembershipReport 	= 0x22,

    MldMembershipQuery      = 0x82, // 130
    MldV1Report             = 0x83, // 131
    MldV1Done               = 0x84, // 132
    MldV2Report             = 0x8f  // 143
    
	} McastFrameType;

#define IgmpV1V2QueryLength     8
#define IgmpV3QueryLength       12 // minimum length
#define MldV1MsgLength          24
#define MldV2MsgMinLength       26

typedef enum
	{
	InvalidMode			= 0,
	ModeIsInclude		= 1,
	ModeIsExclude		= 2,
	ChangeToIncludeMode	= 3,
	ChangeToExcludeMode	= 4,
	AllowNewSources		= 5,
	BlockOldSources		= 6,
	MaxMcastRecType		= 7
	} McastSsmFilterType;

typedef struct
	{
	U8	     type;
	U8	     maxRespTime;         // Unit is 100ms
	U16      checksum;
	IpAddr   mAddr;
	} PACK IgmpV2Frame;

#define IgmpV3QuerySBit     0x08
#define IgmpV3QueryQrvMask  0x07

typedef struct
	{
	U8	     type;
	U8	     maxRespCode;         // Unit is 100ms
	U16      checksum;
	IpAddr   mAddr;
	U8	     qrv;  // querier's robustness variable
	U8	     qqic; // querier's query interval code
	U16      numSrc;
	IpAddr   srcAddr[1];
	} PACK IgmpV3QueryFrame;

typedef struct
	{
	McastSsmFilterType	     type;         // IgmpV3RecType
	U8	     auxLen;       // Should be set to 0 per IGMP V3 RFC 3376
	U16      numSrc;
	IpAddr   mAddr;
	IpAddr   srcAddr[1];   // actual count as per "numSrc"
	} PACK IgmpV3MultiRec;

typedef struct
	{
	U8	           type;
	U8	           rsvd;
	U16            checksum;
	U16            rsvd2;
	U16            numRecs;     // number of groupRecs present
	IgmpV3MultiRec mRec[1];
	} PACK IgmpV3ReportFrame;	

typedef union
    {
    IgmpV3QueryFrame  query;
    IgmpV3ReportFrame rpt;
    } PACK IgmpV3Frame;

typedef union
    {
    IgmpV2Frame v2;
    IgmpV3Frame v3;
    } PACK IgmpFrame;

typedef struct
    {
    McastSsmFilterType       type;
    U8       auxLen;
    U16      numSrc;
    IpV6Addr mAddr;
    IpV6Addr srcAddr[1]; // repeats numSrc times
    } PACK MldV2MultiRec;

typedef struct
    {
    U8            type;
    U8            code;
    U16           checksum;
    U16           rsvd;
    U16           numRecs;
    MldV2MultiRec mRec[1]; // repeats numRecs times
    } PACK MldV2ReportFrame;

#define MldV2QuerySBit     0x08
#define MldV2QueryQrvMask  0x07

typedef struct
    {
    U8       type;
    U8       code;
    U16      checksum;
    U16      maxRespCode;   // Unit is 1ms
    U16      reserved;
    IpV6Addr mAddr;
  	U8	     qrv;  // querier's robustness variable, etc.
	U8	     qqic; // querier's query interval code
  	U16      numSrc;
    IpV6Addr srcAddr[1]; // repeats number of sources
    } PACK MldV2QueryFrame;

typedef union 
    {
    MldV2ReportFrame rpt;
    MldV2QueryFrame  query;
    } PACK MldV2Frame;

typedef struct
    {
    U8 type;
    U8 code;
    U16 checksum;           
    U16 maxRespDelay;       // Unit is 1ms
    U16 reserved;
    IpV6Addr mAddr;
    } PACK MldV1Frame;

typedef union
    {
    MldV1Frame v1;
    MldV2Frame v2;
    } PACK MldFrame;

typedef union
	{
    U8          type; // applies to all igmp/mld messages
    IgmpFrame   igmp;
    MldFrame    mld;
	} PACK McastFrame;

typedef struct
	{
	IpV6NextHdr nextHeader;
	U8          length;
	U32         RouterAlert;
	U8          pad[2];
	} PACK HopByHopHeader;


typedef struct
	{
	U8  srcAddr[16];
	U8  dstAddr[16];
	U32 upperlayerPktLen1;
	U8  zero[3];
	U8  nextHeader;    // use 58 for ICMPv6
	} PACK PseudoIpv6Header;

#if defined(__cplusplus)
}
#endif

#endif // McastTypes.h


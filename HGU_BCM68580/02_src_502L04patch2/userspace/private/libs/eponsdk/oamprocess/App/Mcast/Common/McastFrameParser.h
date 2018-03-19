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


#if !defined(McastFrame_h)
#define McastFrame_h
////////////////////////////////////////////////////////////////////////////////
/// \file McastFrame.h
/// \brief IP Multicast frame module
///
/// IP Multicast frame module
///
////////////////////////////////////////////////////////////////////////////////

#include "Teknovus.h"
#include "Ip.h"
#include "Ethernet.h"
#include "McastTypes.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define IgmpAllRoutersIgmpAddr 0xE0000002
#define IgmpV3ReportDaIp       0xE0000016
#define DefaultIpv4Addr        0xC0A80001 //192.168.0.1
#define ResponseTimeUnit       100        // 100ms


typedef enum
    {
    McastFrameCodeGeneralQuery         = 0,
    McastFrameCodeSpecificQuery        = 1,
    McastFrameCodeSsmQuery             = 2,
    McastFrameCodeGroupReport          = 3,
    McastFrameCodeGroupLeave           = 4,
    McastFrameCodeSsmReport            = 5,

    McastFrameCodeNum
    } PACK McastFrameCode;

typedef struct
    {
    BOOL                v4;
    BOOL                ssmFrame;
    McastFrameCode      frmCode;
    U8                * grpAddr;
    U16                 maxRespTime;     // Unit: 100ms
    U16                 numSrc;
    McastSsmFilterType  ssmType;
    U8                * srcByte;    
    } PACK ParsedMcastFrame;



////////////////////////////////////////////////////////////////////////////////
/// \brief Convert Ipv4 address to multicast Mac address
///
extern
void McastConvertIpv4ToMac(const IpAddr * ip, MacAddr * mac);


////////////////////////////////////////////////////////////////////////////////
/// \brief Convert Ipv6 address to multicast Mac address
///
extern
void McastConvertIpv6ToMac(const IpV6Addr * ipv6, MacAddr * mac);


////////////////////////////////////////////////////////////////////////////////
/// \brief Convert multicast Mac address to Ipv4 address
///
extern
void McastConvertMacToIpv4(MacAddr mac, IpAddr * ipv4);


////////////////////////////////////////////////////////////////////////////////
/// \brief Convert multicast Mac address to Ipv6 address
///
extern
void McastConvertMacToIpv6(MacAddr mac, IpV6Addr * ipv6);


////////////////////////////////////////////////////////////////////////////////
/// \brief Parse the given multicast frame.
///
extern
BOOL McastFrameParser(const McastFrame BULK * frm, U16 length,
                U8 ssmRptMcastAddrRecIdx, ParsedMcastFrame BULK * parsedFrm);


#if defined(__cplusplus)
}
#endif

#endif // McastFrame.h


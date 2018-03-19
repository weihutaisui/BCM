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
/// \file McastFrame.c
/// \brief IP Multicast Frame module
///
/// IP Multicast Frame module
///
////////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include "Teknovus.h"
#include "Ethernet.h"
#include "Ip.h"
#include "Stream.h"
#include "McastCommon.h"
#include "McastTypes.h"
#include "McastFrameParser.h"


////////////////////////////////////////////////////////////////////////////////
/// \brief Convert Ipv4 address to multicast Mac address
///
//extern
void McastConvertIpv4ToMac(const IpAddr * ip, MacAddr * mac)
    {
    mac->byte[0] = 0x01;
    mac->byte[1] = 0x00;
    mac->byte[2] = 0x5E;
    mac->byte[3] = (((*ip) >> 16) & 0x7F);
    mac->byte[4] = (((*ip) >> 8)  & 0xFF);
    mac->byte[5] =  ((*ip) & 0xFF);
    } // McastConvertIpv4ToMac


////////////////////////////////////////////////////////////////////////////////
/// \brief Convert Ipv6 address to multicast Mac address
///
//extern
void McastConvertIpv6ToMac(const IpV6Addr * ipv6, MacAddr * mac)
    {
    mac->byte[0] = 0x33;
    mac->byte[1] = 0x33;
    mac->byte[2] = ipv6->byte[12];
    mac->byte[3] = ipv6->byte[13];
    mac->byte[4] = ipv6->byte[14];
    mac->byte[5] = ipv6->byte[15];
    } // McastConvertIpv6ToMac


////////////////////////////////////////////////////////////////////////////////
/// \brief Convert multicast Mac address to Ipv4 address
///
//extern
void McastConvertMacToIpv4(MacAddr mac, IpAddr * ipv4)
    {
    memcpy(ipv4, &mac.word[1],sizeof(IpAddr));

    (*ipv4) &= 0x007FFFFF;
    (*ipv4) |= 0xE0000000;
    
    } // McastConvertMacToIpv4


////////////////////////////////////////////////////////////////////////////////
/// \brief Convert multicast Mac address to Ipv6 address
///
//extern
void McastConvertMacToIpv6(MacAddr mac, IpV6Addr * ipv6)
    {
    memset(ipv6->byte, 0, sizeof(IpV6Addr));
    memcpy(&ipv6->dword[3], &mac.word[1], sizeof(IpAddr));
    } // McastConvertMacToIpv6


//##############################################################################
//                 Mcast Protocols Frame Parser Functions
//##############################################################################


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the next IgmpV3 report multicast address record based on the 
///        given multicast address record address.
///
static
IgmpV3MultiRec * GetNextIgmpV3ReportAddrRec(IgmpV3MultiRec * rec)
    {
    U16 FAST recLength;
    recLength = sizeof(IgmpV3MultiRec) - sizeof(IpAddr);
    recLength += sizeof(IpAddr)*rec->numSrc;
    recLength += sizeof(U32)*rec->auxLen;
    return (IgmpV3MultiRec BULK *)(((U8 *)rec) + recLength);
    } // GetNextIgmpV3ReportAddrRec


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the next MldV2 report multicast address record based on the 
///        given multicast address record address.
///
static
MldV2MultiRec * GetNextMldV2ReportAddrRec(MldV2MultiRec * rec)
    {
    U16 FAST recLength;
    recLength = sizeof(MldV2MultiRec) - sizeof(IpV6Addr);
    recLength += sizeof(IpV6Addr)*rec->numSrc;
    recLength += sizeof(U32)*rec->auxLen;
    return (MldV2MultiRec BULK *)(((U8 *)rec) + recLength);
    } // GetNextMldV2ReportAddrRec

////////////////////////////////////////////////////////////////////////////////
/// \brief Calculate the Max Response Time(Units 100ms) of the IgmpV3 query.
///
static
U16 CalculateIgmpV3MaxRespTime(U8 maxRespCode)
    {
    U16 FAST maxRespTime;
    if(maxRespCode < IgmpV3TimeMinNonFloatValue)
        {
        maxRespTime = (U16)maxRespCode;
        }
    else
        {
        U8 FAST exp  = (maxRespCode&IgmpV3TimeExpMask) >> IgmpV3TimeExpOffset;
        U8 FAST mant = (maxRespCode&IgmpV3TimeMantMask);
        maxRespTime  = (U16)((U32)(mant|IgmpV3TimeMantBase) << 
                       (exp+IgmpV3TimeExpBase));
        }
    return maxRespTime;
    } // CalculateIgmpV3MaxRespTime


////////////////////////////////////////////////////////////////////////////////
/// \brief Calculate the Max Response Time(Units 100ms) of the MldV2 query.
///
static
U16 CalculateMldV2MaxRespTime(U16 maxRespCode)
    {
    U16 FAST maxRespTime;
    if(maxRespCode < MldV2TimeMinNonFloatValue)
        {
        maxRespTime = (maxRespCode/ResponseTimeUnit) + 1;
        }
    else
        {
        U16 FAST exp  = (maxRespCode & MldV3TimeExpMask) >> MldV2TimeExpOffset;
        U16 FAST mant = (maxRespCode&MldV2TimeMantMask);
        maxRespTime  = 
            (U16)((((U32)(mant|MldV2TimeMantBase) << 
                (exp+MldV2TimeExpBase))/ResponseTimeUnit) + 1);
        }
    return maxRespTime;
    } // CalculateMldV2MaxRespTime


////////////////////////////////////////////////////////////////////////////////
/// \brief Parse the given multicast frame.
///
//extern
BOOL McastFrameParser(const McastFrame BULK * frm, U16 length,
                U8 ssmRptMcastAddrRecIdx, ParsedMcastFrame BULK * parsedFrm)
    {
    memset(parsedFrm, 0, sizeof(ParsedMcastFrame));
    parsedFrm->v4 = TRUE;
    parsedFrm->ssmFrame = FALSE;
    switch(frm->type)
        {
        case IgmpV1MembershipReport:
        case IgmpV2MembershipReport:
        case IgmpV2LeaveGroup:
            {
            parsedFrm->grpAddr = (U8 *)&frm->igmp.v2.mAddr;
            parsedFrm->frmCode = McastFrameCodeGroupReport;
            if(frm->type == IgmpV2LeaveGroup)
                {
                parsedFrm->frmCode = McastFrameCodeGroupLeave;
                }
            }
            break;
            
        case MldV1Report:
        case MldV1Done:
            {
            parsedFrm->v4 = FALSE;
            parsedFrm->grpAddr = (U8 *)&frm->mld.v1.mAddr;
            parsedFrm->frmCode = McastFrameCodeGroupReport;
            if(frm->type == MldV1Done)
                {
                parsedFrm->frmCode = McastFrameCodeGroupLeave;
                }
            }
            break;
            
        case IgmpV3MembershipReport:
            {
            IgmpV3MultiRec BULK * BULK grpRecPtr;
            grpRecPtr = frm->igmp.v3.rpt.mRec;
            
            while(ssmRptMcastAddrRecIdx != 0)
                {
                // Move to next mcast address record
                grpRecPtr = GetNextIgmpV3ReportAddrRec(grpRecPtr);
                ssmRptMcastAddrRecIdx--;
                }
            
            parsedFrm->ssmFrame = TRUE;
            parsedFrm->frmCode = McastFrameCodeSsmReport;
            parsedFrm->grpAddr = (U8 *)&grpRecPtr->mAddr;
            parsedFrm->ssmType   = grpRecPtr->type;
            parsedFrm->numSrc    = grpRecPtr->numSrc;
            parsedFrm->srcByte   = (U8 *)&grpRecPtr->srcAddr[0];
            }
            break;
        
        case MldV2Report:
            {
            MldV2MultiRec BULK * BULK grpRecPtr;
            grpRecPtr = frm->mld.v2.rpt.mRec;
            
            while(ssmRptMcastAddrRecIdx != 0)
                {
                // Move to next mcast address record
                grpRecPtr = GetNextMldV2ReportAddrRec(grpRecPtr);
                ssmRptMcastAddrRecIdx--;
                }
            
            parsedFrm->v4 = FALSE;
            parsedFrm->ssmFrame = TRUE;
            parsedFrm->frmCode = McastFrameCodeSsmReport;
            parsedFrm->grpAddr = (U8 *)&grpRecPtr->mAddr;
            parsedFrm->ssmType   = grpRecPtr->type;
            parsedFrm->numSrc    = grpRecPtr->numSrc;
            parsedFrm->srcByte   = (U8 *)&grpRecPtr->srcAddr[0];
            }
            break;
        
        case IgmpMembershipQuery:
            {
            if(length == IgmpV1V2QueryLength)
                {
                parsedFrm->maxRespTime = 
                            (frm->igmp.v2.maxRespTime == 0) ? 
                            100 : (U16)frm->igmp.v2.maxRespTime;

                parsedFrm->grpAddr = (U8 *)&frm->igmp.v2.mAddr;
                parsedFrm->frmCode = McastFrameCodeGeneralQuery;
                if(frm->igmp.v2.mAddr != 0)
                    {
                    // Group Specific Query;
                    parsedFrm->frmCode = McastFrameCodeSpecificQuery;                   
                    }
                }
            else if(length >= IgmpV3QueryLength)
                {
                parsedFrm->ssmFrame = TRUE;
                parsedFrm->maxRespTime = 
                    CalculateIgmpV3MaxRespTime(frm->igmp.v3.query.maxRespCode);
                parsedFrm->grpAddr = (U8 *)&frm->igmp.v3.query.mAddr;
                if(frm->igmp.v3.query.mAddr == 0)
                    {
                    // general query;
                    parsedFrm->frmCode = McastFrameCodeGeneralQuery;
                    
                    }
                else if(frm->igmp.v3.query.numSrc == 0)
                    {
                    // Group Specific Query;
                    parsedFrm->frmCode = McastFrameCodeSpecificQuery;
                    }
                else
                    {
                    parsedFrm->frmCode = McastFrameCodeSsmQuery;
                    parsedFrm->ssmType = ModeIsInclude;
                    parsedFrm->numSrc  = frm->igmp.v3.query.numSrc;
                    parsedFrm->srcByte = (U8 *)&frm->igmp.v3.query.srcAddr[0];
                    }
                }
            else
                {
                // Error
                return FALSE;
                }
            }
            break;
            
        case MldMembershipQuery:
            {
            IpV6Addr FAST nullV6Addr;
            memset(&nullV6Addr, 0, sizeof(IpV6Addr));
            parsedFrm->v4 = FALSE;
            if(length  == MldV1MsgLength)
                {
                parsedFrm->maxRespTime = 
                             (frm->mld.v1.maxRespDelay/ResponseTimeUnit)+1;
                parsedFrm->grpAddr = (U8 *)&frm->mld.v1.mAddr;
                parsedFrm->frmCode = McastFrameCodeSpecificQuery;
                if(memcmp(&frm->mld.v1.mAddr, &nullV6Addr, sizeof(IpV6Addr)) 
                                    == 0)
                    {
                    parsedFrm->frmCode = McastFrameCodeGeneralQuery;
                    }
                }
            else if(length  >= MldV2MsgMinLength)
                {
                parsedFrm->ssmFrame = TRUE;
                parsedFrm->maxRespTime = 
                    CalculateMldV2MaxRespTime(frm->mld.v2.query.maxRespCode);
                parsedFrm->grpAddr = (U8 *)&frm->mld.v2.query.mAddr;
                if(memcmp(&frm->mld.v1.mAddr, &nullV6Addr, sizeof(IpV6Addr)) 
                                    == 0)
                    {
                    parsedFrm->frmCode = McastFrameCodeGeneralQuery;
                    }
                else if(frm->mld.v2.query.numSrc == 0)
                    {
                    // group specific query
                    parsedFrm->frmCode = McastFrameCodeSpecificQuery;
                    }
                else
                    {
                    // group specific source query
                    parsedFrm->frmCode = McastFrameCodeSsmQuery;
                    parsedFrm->ssmType = ModeIsInclude;
                    parsedFrm->srcByte = (U8 *)&frm->mld.v2.query.srcAddr[0],
                    parsedFrm->numSrc  = frm->mld.v2.query.numSrc;
                    }
                }
            else
                {
                return FALSE;
                }
            }
            break;
            
        default:
            return FALSE;
        }
    return TRUE;
    } // McastFrameParser


// end of McastSnoop.c


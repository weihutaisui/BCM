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
#include "McastTypes.h"
#include "McastFrame.h"
#include "McastCommon.h"

#define Ipv4HdrLengthDefault               (24)   // 4 bytes options
#define IpV4HdrVerHdrLen(len)              ((U8)(0x40 | ((len) >> 2)))
#define Ipv4HdrTosDefault                  (0xC0)
#define Ipv4HdrTtlDefault                  (1)
#define Ipv6HdrVerTrfClsFlowLableDefault   ((U32)(0x6 << 28))
#define Ipv6HdrHopLimitDefault             (1)
#define Ipv6NextHdrRouterAlertDefault      (0x5020000)

#define IgmpV3QueryFrameSize(numSrcs) \
                          ((sizeof(IgmpV3QueryFrame) - sizeof(IpAddr)) + \
                           ((numSrcs) * sizeof(IpAddr)))

#define IgmpV3ReportFrameSize(numSrcs) \
                          ((sizeof(IgmpV3ReportFrame) - sizeof(IpAddr)) + \
                               ((numSrcs) * sizeof(IpAddr)))

#define IgmpV3LeaveFrameSize() IgmpV3ReportFrameSize(0)

#define MldV2QueryFrameSize(numSrcs) \
                          ((sizeof(MldV2QueryFrame) - sizeof(IpV6Addr)) + \
                           ((numSrcs) * sizeof(IpV6Addr)))

#define MldV2ReportFrameSize(numSrcs) \
                          ((sizeof(MldV2ReportFrame) - sizeof(IpV6Addr)) + \
                           ((numSrcs) * sizeof(IpV6Addr)))

#define MldV2LeaveFrameSize() MldV2ReportFrameSize(0)

const MacAddr CODE IgmpAllRoutersMac =
      {0x01, 0x00, 0x5E, 0x00, 0x00, 0x02};  //lint !e708

const MacAddr CODE MldAllRoutersMac =
      {0x33, 0x33, 0x00, 0x00, 0x00, 0x02};  //lint !e708


////////////////////////////////////////////////////////////////////////////////
/// \brief Igmp/MLD check sum calculate common function
///
static
void IncChecksum(const U8 *addr, U32 count, U32 *sum)
    {
    while(count > 1)
        {
        *sum = *sum + *((U16 *) addr); //lint !e826
        addr+=2;
        count-=2;
        }

    if (count > 0)
        {
        *sum = *sum + (*(U8*)addr);
        }
    } // IncChecksum


////////////////////////////////////////////////////////////////////////////////
/// \brief calculate the Igmp/MLD check sum
///
static
U16 CalcChecksum(U32 sum)
    {
    while ((sum >> 16) != 0)
        {
        sum = (sum & 0xFFFF) + (sum >> 16);
        }
    return (U16)~sum;
    } // CalcChecksum


////////////////////////////////////////////////////////////////////////////////
/// \brief Igmp packets check sum calculate
///
static
U16 McastV4CheckSum (const U8 *addr, U32 count)
    {
    U32 BULK sum = 0;
    IncChecksum(addr,count,&sum);
    return CalcChecksum(sum);
    } // McastV4CheckSum


////////////////////////////////////////////////////////////////////////////////
/// \brief Mld packets check sum calculate
///
static
U16 McastV6Checksum(const IpV6Header * pIpFrame,
                    const MldFrame *  pMldFrame,
                    U16   payloadLength)
    {
    U32 sum = 0;
    PseudoIpv6Header  pHeader;

    memset(&pHeader, 0, sizeof(PseudoIpv6Header));
    memcpy(pHeader.srcAddr, &pIpFrame->sourceAddr.byte[0], sizeof(IpV6Addr));
    memcpy(pHeader.dstAddr, &pIpFrame->destAddr.byte[0], sizeof(IpV6Addr));
    pHeader.upperlayerPktLen1 = payloadLength;
    pHeader.nextHeader        = IpV6ExtHdrIcmp;
    IncChecksum((U8 BULK *)&pHeader, sizeof(IpV6Header), &sum);
    IncChecksum((U8 BULK *)pMldFrame, payloadLength, &sum);
    return CalcChecksum(sum);
    } // McastV6Checksum


////////////////////////////////////////////////////////////////////////////////
/// \brief Creates an Igmp V3 time code from time in 100 milliseconds
///
///        Per RFC3376,
///        If Max Resp Code < 128, Max Resp Time = Max Resp Code
///        If Max Resp Code >= 128, Max Resp Code represents a floating-point
///           value as follows:
///
///            0 1 2 3 4 5 6 7
///           +-+-+-+-+-+-+-+-+
///           |1| exp |  mant |
///           +-+-+-+-+-+-+-+-+
///
///           Max Resp Time = (mant | 0x10) << (exp + 3)
///
/// \return converted time code
////////////////////////////////////////////////////////////////////////////////
static
U8 MakeIgmpV3TimeCode (U16 timeIn100Ms)
    {
    U8  timeCode;
    U8  exp = IgmpV3TimeExpBase;

    if (timeIn100Ms >= IgmpV3TimeMinNonFloatValue)
        {
        BOOL done = FALSE;

        do
            {
            if ((timeIn100Ms >> exp) <= IgmpV3TimeMantMax)
                {
                done = TRUE;
                }
            else
                {
                exp++;
                }
            }
        while (!done);

        timeCode = (U8)(IgmpV3TimeMinNonFloatValue |
                        ((U8)(exp - IgmpV3TimeExpBase) << IgmpV3TimeExpOffset)|
                        ((timeIn100Ms >> exp) & IgmpV3TimeMantMask));
        }
    else
        {
        timeCode = (U8)timeIn100Ms;
        }

    if ((exp - IgmpV3TimeExpBase) > 7)
        {
        // Value was too large to fit, even with exponents.  Just return a
        // default value.
        return (IgmpV3TimeMinNonFloatValue - 1);
        }
    else
        {
        return (timeCode);
        }

    } // MakeIgmpV3TimeCode


////////////////////////////////////////////////////////////////////////////////
/// \brief Creates an MLD V2 time code from time in 100 milliseconds
///
///        Per RFC 3810
///        If Maximum Response Code < 32768,
///           Maximum Response Delay = Maximum Response Code
///
///        If Maximum Response Code >=32768, Maximum Response Code represents a
///           floating-point value as follows:
///
///            0 1 2 3 4 5 6 7 8 9 A B C D E F
///           +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
///           |1| exp |          mant         |
///           +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
///
///           Maximum Response Delay = (mant | 0x1000) << (exp+3)
///
/// \return converted time
////////////////////////////////////////////////////////////////////////////////
static
U16 MakeMldV2TimeCode (U16 timeIn100Ms)
    {
    U16 timeCode;
    U16 exp = 3;
    U32 v2Time = timeIn100Ms * 100;


    if (v2Time > MldV2TimeMinNonFloatValue)
        {
        BOOL done = FALSE;

        do
            {
            if ((v2Time >> exp) <= MldV2TimeMantMax)
                {
                done = TRUE;
                }
            else
                {
                exp++;
                }
            }
        while (!done);

        timeCode = MldV2TimeMinNonFloatValue |
                   (U16)(((U16)(exp - MldV2TimeExpBase) << MldV2TimeExpOffset))|
                   (U16)(((v2Time >> exp) & MldV2TimeMantMask));
        }
    else
        {
        // Values this low are just the number; not exponents required.
        timeCode = (U16)v2Time;
        }

    if ((exp - MldV2TimeExpBase) > 0x7)
        {
        // Value was too large to fit, even with exponents.
        return MldV2TimeMinNonFloatValue;
        }
    else
        {
        return (timeCode);
        }

    } // MakeMldV2TimeCode


////////////////////////////////////////////////////////////////////////////////
/// \brief Build the ethernet header in the given buffer
///
static
U16 BuildEthernetHeader(U8 * buf, MacAddr daMac, VlanTag vid,
                       Ethertype type)
    {
    Stream strm;
    StreamInit(&strm, buf);

    // DA
    StreamWriteBytes(&strm, (U8 *)&daMac, sizeof(MacAddr));
    // Jump SA, it will be added when sending
    StreamSkip(&strm, sizeof(MacAddr));

    if(vid != McastVlanNonVid)
        {
        // VLAN TAG
        StreamWriteU16(&strm, EthertypeCvlan);
        StreamWriteU16(&strm, (vid&0xFFF));
        }

    // EtherType
    StreamWriteU16(&strm, type);

    return (U16)StreamLengthInBytes(&strm);
    } // BuildEthernetHeader


////////////////////////////////////////////////////////////////////////////////
/// \brief Build the Ipv4 header in the given buffer
///
static
void BuildIpV4Header(U8 * buf, IpAddr daIp, U16 payloadLength,
                                              U16 * hdrLen)
    {
    IpHeader BULK * ipV4Hdr;
    *hdrLen = Ipv4HdrLengthDefault;
    ipV4Hdr = (IpHeader BULK *)buf;
    memset((U8 *)ipV4Hdr, 0, Ipv4HdrLengthDefault);

    ipV4Hdr->versionHdrLen = IpV4HdrVerHdrLen(*hdrLen);
    ipV4Hdr->tos = Ipv4HdrTosDefault;
    ipV4Hdr->length = (*hdrLen + payloadLength);
    ipV4Hdr->ttl = Ipv4HdrTtlDefault;
    ipV4Hdr->protocol = IpProtocolIgmp;
    ipV4Hdr->sourceAddr = (IpAddr)DefaultIpv4Addr;
    ipV4Hdr->destAddr = daIp;
    ipV4Hdr->checksum = McastV4CheckSum((U8*)ipV4Hdr, *hdrLen);
    } // BuildIpV4Header


////////////////////////////////////////////////////////////////////////////////
/// \brief Build the IgmpV1V2 header in the given buffer
///
static
void BuildIgmpV1V2Frame(U8 * buf, McastFrameType type,
                                  IpAddr ip,U16 maxRespTime)
    {
    IgmpV2Frame * frm = (IgmpV2Frame *)buf;
    frm->type  = type;
    frm->mAddr = ip;
    frm->maxRespTime = (type == IgmpMembershipQuery) ?
                                    (U8)maxRespTime : 0;
    frm->checksum = 0;
    frm->checksum = McastV4CheckSum((U8 *)frm, sizeof(IgmpV2Frame));
    } // BuildIgmpV1V2Frame


////////////////////////////////////////////////////////////////////////////////
/// \brief Build the IgmpV3 query frame in the given buffer
///
static
void BuildIgmpV3QueryFrame(U8       * buf,
                           IpAddr     ip,
                           U16        numSrcs,
                           const U8 * srcByte,
                           U16        maxRespTime)
    {
    Stream strmRd;
    Stream strmWr;
    IgmpV3QueryFrame * queryFrm = (IgmpV3QueryFrame *)buf;

    memset(queryFrm, 0, sizeof(IgmpV3QueryFrame));

    queryFrm->type = IgmpMembershipQuery;
    queryFrm->maxRespCode = MakeIgmpV3TimeCode(maxRespTime);
    memcpy(&queryFrm->mAddr, &ip, sizeof(IpAddr));
    queryFrm->numSrc = numSrcs;

    StreamInit(&strmRd, srcByte);
    StreamInit(&strmWr, (U8 *)&queryFrm->srcAddr[0]);
    while(numSrcs != 0)
        {
        StreamCopy(&strmWr, strmRd.cur, sizeof(IpAddr));
        StreamSkip(&strmRd, sizeof(IpAddr));
        numSrcs--;
        }
    queryFrm->checksum = McastV4CheckSum(buf,
                                  (U32)IgmpV3QueryFrameSize(queryFrm->numSrc));
    } // BuildIgmpV3QueryFrame


////////////////////////////////////////////////////////////////////////////////
/// \brief Build the IgmpV3 leave frame in the given buffer
///
static
void BuildIgmpV3ReportFrame(U8       * buf,
                            IpAddr     ip,
                            McastSsmFilterType filType,
                            U16        numSrcs,
                            const U8 * srcByte)
    {
    Stream strmRd;
    Stream strmWr;
    IgmpV3ReportFrame * reportFrm =  (IgmpV3ReportFrame *)buf;

    memset(reportFrm, 0, sizeof(IgmpV3ReportFrame));

    reportFrm->type = IgmpV3MembershipReport;
    reportFrm->numRecs = 1;

    reportFrm->mRec[0].type = filType;
    reportFrm->mRec[0].numSrc = numSrcs;
    memcpy(&reportFrm->mRec[0].mAddr, &ip, sizeof(IpAddr));

    StreamInit(&strmRd, srcByte);
    StreamInit(&strmWr, (U8 *)&reportFrm->mRec[0].srcAddr[0]);
    while(numSrcs != 0)
        {
        StreamCopy(&strmWr, strmRd.cur, sizeof(IpAddr));
        StreamSkip(&strmRd, sizeof(IpAddr));
        numSrcs--;
        }
    reportFrm->checksum = McastV4CheckSum(buf,
                        (U32)IgmpV3ReportFrameSize(reportFrm->mRec[0].numSrc));
    } // BuildIgmpV3ReportFrame


////////////////////////////////////////////////////////////////////////////////
/// \brief Build the IgmpV3 Leave frame in the given buffer
///
static
void BuildIgmpV3LeaveFrame(U8 * buf, IpAddr ip)
    {
    BuildIgmpV3ReportFrame(buf, ip, ModeIsInclude, 0, NULL);
    } // BuildIgmpV3LeaveFrame


////////////////////////////////////////////////////////////////////////////////
/// \brief Build the Ipv6 header in the given buffer
///
static
void BuildIpV6Header(U8 * buf, const IpV6Addr * daIpv6,
                         U16 payloadLength, U16 * hdrLen)
    {
    IpV6Header * ipV6Hdr;
    HopByHopHeader *hop;
    ipV6Hdr = (IpV6Header *)buf;

    memset(ipV6Hdr, 0, sizeof(IpV6Header));
    ipV6Hdr->verTrfClassFlowLabel = Ipv6HdrVerTrfClsFlowLableDefault;
    ipV6Hdr->length = sizeof(HopByHopHeader) + payloadLength;
    ipV6Hdr->nextHeader = IpV6ExtHdrHopByHop;
    ipV6Hdr->hopLimit = Ipv6HdrHopLimitDefault;
    ipV6Hdr->sourceAddr.byte[0] = 0xFF;
    ipV6Hdr->sourceAddr.dword[3] = (U32)DefaultIpv4Addr;
    memcpy(ipV6Hdr->destAddr.byte, daIpv6->byte, sizeof(IpV6Addr));

    hop = (HopByHopHeader *)(ipV6Hdr+1);
    hop->nextHeader = IpV6Icmp;
    hop->length = 0;
    hop->RouterAlert = Ipv6NextHdrRouterAlertDefault;
    *hdrLen = sizeof(IpV6Header)+sizeof(HopByHopHeader);
    } // BuildIpV6Header


////////////////////////////////////////////////////////////////////////////////
/// \brief Build the Mldv1 header in the given buffer
///
static
void BuildMldV1Frame(U8 * buf,
                     U16 ipHdrLen,
                     McastFrameType type,
                     const IpV6Addr * grpIpv6,
                     U16 maxRespTime)
    {
    IpV6Header * ipV6Hdr = (IpV6Header *)buf;
    MldV1Frame * frm = (MldV1Frame *)(buf + ipHdrLen);
    memset(frm, 0, sizeof(MldV1Frame));

    frm->type = type;
    frm->code = 0;
    frm->maxRespDelay = maxRespTime;
    memcpy(frm->mAddr.byte, grpIpv6->byte, sizeof(IpV6Addr));
    frm->checksum =
         McastV6Checksum(ipV6Hdr, (MldFrame *)frm, sizeof(MldV1Frame));
    } // BuildMldV1Frame


////////////////////////////////////////////////////////////////////////////////
/// \brief Build the MldV2 query frame in the given buffer
///
static
void BuildMldV2QueryFrame(U8       * buf,
                          U16        ipHdrLen,
                          const IpV6Addr * ipV6,
                          U16        numSrcs,
                          const U8 * srcByte,
                          U16        maxRespTime)
    {
    Stream strmRd;
    Stream strmWr;
    IpV6Header * ipV6Hdr = (IpV6Header *)buf;
    MldV2QueryFrame * queryFrm = (MldV2QueryFrame *)(buf + ipHdrLen);

    memset(queryFrm, 0, sizeof(MldV2QueryFrame));

    queryFrm->type = MldMembershipQuery;
    queryFrm->code = 0;
    queryFrm->maxRespCode = MakeMldV2TimeCode(maxRespTime);
    memcpy(&queryFrm->mAddr, ipV6, sizeof(IpV6Addr));
    queryFrm->numSrc = numSrcs;

    StreamInit(&strmRd, srcByte);
    StreamInit(&strmWr, (U8 *)&queryFrm->srcAddr[0]);
    while(numSrcs != 0)
        {
        StreamCopy(&strmWr, strmRd.cur, sizeof(IpV6Addr));
        StreamSkip(&strmRd, sizeof(IpV6Addr));
        numSrcs--;
        }
    queryFrm->checksum =
              McastV6Checksum(ipV6Hdr, (MldFrame *)queryFrm,
                                  (U16)MldV2QueryFrameSize(queryFrm->numSrc));
    } // BuildMldV2QueryFrame


////////////////////////////////////////////////////////////////////////////////
/// \brief Build the MldV2 report frame in the given buffer
///
static
void BuildMldV2ReportFrame(U8       * buf,
                           U16        ipHdrLen,
                           const IpV6Addr * ipV6,
                           McastSsmFilterType filType,
                           U16        numSrcs,
                           const U8 * srcByte)
    {
    Stream strmRd;
    Stream strmWr;
    IpV6Header * ipV6Hdr = (IpV6Header *)buf;
    MldV2ReportFrame * reportFrm = (MldV2ReportFrame *)(buf + ipHdrLen);

    memset(reportFrm, 0, sizeof(MldV2ReportFrame));

    reportFrm->type = MldV2Report;
    reportFrm->code = 0;
    reportFrm->numRecs = 1;

    reportFrm->mRec[0].type = filType;
    reportFrm->mRec[0].numSrc = numSrcs;
    memcpy(&reportFrm->mRec[0].mAddr, ipV6, sizeof(IpV6Addr));


    StreamInit(&strmRd, srcByte);
    StreamInit(&strmWr, (U8 *)&reportFrm->mRec[0].srcAddr[0]);
    while(numSrcs != 0)
        {
        StreamCopy(&strmWr, strmRd.cur, sizeof(IpV6Addr));
        StreamSkip(&strmRd, sizeof(IpV6Addr));
        numSrcs--;
        }
    reportFrm->checksum =
              McastV6Checksum(ipV6Hdr, (MldFrame *)reportFrm,
                         (U16)MldV2ReportFrameSize(reportFrm->mRec[0].numSrc));
    } // BuildMldV2ReportFrame


////////////////////////////////////////////////////////////////////////////////
/// \brief Build the MldV2 leave/done frame in the given buffer
///
static
void BuildMldV2LeaveFrame(U8 * buf, U16 ipHdrLen, const IpV6Addr * ipV6)
    {
    BuildMldV2ReportFrame(buf, ipHdrLen, ipV6, ModeIsInclude, 0, NULL);
    } // BuildMldV2LeaveFrame


////////////////////////////////////////////////////////////////////////////////
/// \brief Build the Igmp query frame in the given buffer
///
//extern
void IgmpFrameQueryFill(BOOL ssm, U8 * buf, VlanTag vid, IpAddr ipv4)
    {
    U16 ethHdrLen;
    U16 ipHdrLen;
    MacAddr daMac;
    U16 frameSize = (U16)sizeof(IgmpV2Frame);
    if(ssm)
        {
        frameSize = (U16)IgmpV3QueryFrameSize(0);
        }

    McastConvertIpv4ToMac(&ipv4, &daMac);

    // src Mac needs to be added when sending!
    ethHdrLen = BuildEthernetHeader(buf, daMac, vid, EthertypeIp);

    BuildIpV4Header(&buf[ethHdrLen], ipv4, frameSize, &ipHdrLen);
    if(ssm)
        {
        BuildIgmpV3QueryFrame(&buf[ethHdrLen+ipHdrLen],
                              ipv4,
                              0,
                              NULL,
                              DefaultLmqResponse);
        }
    else
        {
        BuildIgmpV1V2Frame(&buf[ethHdrLen+ipHdrLen], IgmpMembershipQuery,
                           ipv4, DefaultLmqResponse);
        }
    } // IgmpFrameQueryFill


////////////////////////////////////////////////////////////////////////////////
/// \brief Build the Igmp leave frame in the given buffer
///
//extern
void IgmpFrameLeaveFill(BOOL ssm, U8 * buf, VlanTag vid, IpAddr ipv4)
    {
    U16 ethHdrLen;
    U16 ipHdrLen;
    U16 frameSize = (U16)sizeof(IgmpV2Frame);
    if(ssm)
        {
        frameSize = (U16)IgmpV3LeaveFrameSize();
        }

    // src Mac needs to be added when sending!
    ethHdrLen = BuildEthernetHeader(buf, IgmpAllRoutersMac, vid, EthertypeIp);

    BuildIpV4Header(&buf[ethHdrLen],
                    (IpAddr)((ssm) ? IgmpV3ReportDaIp : IgmpAllRoutersIgmpAddr),
                    frameSize,
                    &ipHdrLen);
    if(ssm)
        {
        BuildIgmpV3LeaveFrame(&buf[ethHdrLen+ipHdrLen], ipv4);
        }
    else
        {
        BuildIgmpV1V2Frame(&buf[ethHdrLen+ipHdrLen], IgmpV2LeaveGroup,
                       ipv4, 0);
        }
    } // IgmpFrameQueryFill


////////////////////////////////////////////////////////////////////////////////
/// \brief Build the Mld query frame in the given buffer
///
//extern
void MldFrameQueryFill(BOOL ssm, U8 * buf,
                       VlanTag vid, const IpV6Addr * grpIpv6)
    {
    U16 ethHdrLen;
    U16 ipHdrLen;
    MacAddr daMac;
    U16 frameSize = (U16)sizeof(MldV1Frame);
    if(ssm)
        {
        frameSize = (U16)MldV2QueryFrameSize(0);
        }

    McastConvertIpv6ToMac(grpIpv6, &daMac);

    // src Mac needs to be added when sending!
    ethHdrLen = BuildEthernetHeader(buf, daMac, vid, EthertypeIpv6);

    BuildIpV6Header(&buf[ethHdrLen], grpIpv6, frameSize, &ipHdrLen);

    if(ssm)
        {
        BuildMldV2QueryFrame(&buf[ethHdrLen],
                             ipHdrLen,
                             grpIpv6,
                             0,
                             NULL,
                             DefaultLmqResponse);
        }
    else
        {
        BuildMldV1Frame(&buf[ethHdrLen], ipHdrLen, MldMembershipQuery,
                                         grpIpv6, DefaultLmqResponse);
        }
    } // MldFrameQueryFill


////////////////////////////////////////////////////////////////////////////////
/// \brief Build the Mld done frame in the given buffer
///
//extern
void MldFrameLeaveFill(BOOL ssm, U8 * buf,
                                 VlanTag vid, const IpV6Addr * grpIpv6)
    {
    U16 ethHdrLen;
    U16 ipHdrLen;
    IpV6Addr daIpv6;
    U16 frameSize = (U16)sizeof(MldV1Frame);
    if(ssm)
        {
        frameSize = (U16)MldV2LeaveFrameSize();
        }
    // src Mac needs to be added when sending!
    ethHdrLen = BuildEthernetHeader(buf, MldAllRoutersMac, vid, EthertypeIpv6);

    McastConvertMacToIpv6(MldAllRoutersMac, &daIpv6);
    BuildIpV6Header(&buf[ethHdrLen], &daIpv6, frameSize, &ipHdrLen);
    if(ssm)
        {
        BuildMldV2LeaveFrame(&buf[ethHdrLen], ipHdrLen, grpIpv6);
        }
    else
        {
        BuildMldV1Frame(&buf[ethHdrLen], ipHdrLen, MldV1Done, grpIpv6, 0);
        }
    } // MldFrameLeaveFill

// end of McastSnoop.c


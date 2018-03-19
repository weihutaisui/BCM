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
/// \file McastMgmt.h
/// \brief IP Multicast management module
///
/// Manages the IP multicast module
///
////////////////////////////////////////////////////////////////////////////////

//generic includes
#include <string.h>
#include "Ethernet.h"
#include "Stream.h"
#include "Teknovus.h"

// base includes
#include "EthFrameUtil.h"
#include "Fds.h"
#include "PonManager.h"
#include "SystemDispatch.h"
#include "OntmTimer.h"
#include "UniConfigDb.h"

// interface includes
#include "DebugCmds.h"

// app includes
#include "McastBaseRule.h"
#include "McastDb.h"
#include "McastFrame.h"
#include "McastCommon.h"
#include "McastMgmt.h"
#include "McastTypes.h"
#include "McastVlan.h"

#define IgmpV3MldV2Supported        1

#define SrcIdToSsmDomainId(srcId)   ((U16)(srcId))
#define SsmDomainIdNone             (SrcIdToSsmDomainId(SsmSrcIdNone))

#define SsmDomainMapPortMask        (0x111111111ULL)
#define SsmDomainMapPort(port)      (SsmDomainMapPortMask << (port))
#define SsmDomainMap(port, srcId)   (McastBitMap((port) + ((srcId) << 2)))
#define SrcIdMapInterSect(A, B)     ((A)&(B))
#define SrcIdMapAdd(A, B)           ((A)|(B))
#define SrcIdMapSub(A, B)           ((A)&(~(SrcIdMapInterSect((A), (B)))))

typedef enum
    {
    RecMcastCfg,
    RecMcastVlan,

    RecordsCount
    } RecordIds;

static U8 BULK recLookupTable[FdsLkupBuffSize(RecordsCount)];
static BOOL BULK mcastAdminState;
static BOOL BULK mcastVlanMode;

// Used for store the SA MAC of the upstream leave frame in the proxy mode.
// Proxy leave frame can't use the EPON mac, as OLT will capture it and don't
// forward to NNI port. So it's reasonable to keep the SA MAC is same as
// original Leave frame
static MacAddr BULK proxyFrameMac;
static BOOL    BULK proxyExit;
static BOOL    BULK proxyFrameSsm[McastPortMaxNum];

static McastGrpEntry BULK mcastGrpTable[McastOnuGrpMaxNum];

static SsmSrcRecord BULK ssmSrcRecord;

static McastVlanActHdl BULK mcastVlanActCbHdl;

McastGrpInfo BULK * BULK mcastMgmtGroupInfo;
static U8 BULK mcastGroupInfoBuf[McastGrpInfoBufSize];


//##############################################################################
//                      Multicast Debug Functions
//##############################################################################

#define McastDebugLevel            TkDebugHi
#define McastDebug(lvl, args)      CliDebug(DebugMcast, lvl, args)


//##############################################################################
//                      Multicast Management Funtcions
//##############################################################################


////////////////////////////////////////////////////////////////////////////////
/// \brief Reset the multicast group info buffer.
///
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void McastMgmtGroupInfoBufReset(void)
    {
    memset(mcastGroupInfoBuf, 0, McastGrpInfoBufSize);
    } // McastMgmtGroupInfoBufReset


////////////////////////////////////////////////////////////////////////////////
/// \brief Set the multicast vlan action callback handle
///
/// \param upCb Upstream call back handle
/// \param dnCb Downstream call back handle
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void McastMgmtVlanActCbHdlReg(McastVlanActCb upCb, McastVlanActCb dnCb)
    {
    mcastVlanActCbHdl.up = upCb;
    mcastVlanActCbHdl.dn = dnCb;
    } //McastMgmtVlanActCbHdlReg


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the multicast admin state
//extern
BOOL McastAdminState(void)
    {
    return mcastAdminState;
    } // McastAdminState

////////////////////////////////////////////////////////////////////////////////
/// \brief Initialize multicast vlans from the personality.
///
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void McastMgmtVlanInit(void)
    {
    TkOnuEthPort BULK port;
    McastVlanCfg BULK vlanCfg;
    McastVlanPortCfg BULK *BULK portCfgPtr;
    memset(&vlanCfg, 0, sizeof(McastVlanCfg));
    if (FdsRecRead(FdsGrpIdMcast, RecMcastVlan, &vlanCfg,
                   sizeof(McastVlanCfg)) == 0)
        {
        (void)FdsRecWrite(FdsGrpIdMcast, RecMcastVlan, &vlanCfg,
                          sizeof(McastVlanCfg));
        }

    portCfgPtr = vlanCfg.port;
    for (port = 0; port < McastPortMaxNum; port++)
        {
        (void)McastVlanCfgSet(port, portCfgPtr);
        portCfgPtr++;
        }
    } // McastMgmtVlanInit


////////////////////////////////////////////////////////////////////////////////
/// \brief set the port multicast vlans.
///
/// \param port Port to set
/// \param cfg Pointer to the multicast vlan configuration buffer
///
/// \return
/// TRUE if success, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL McastMgmtVlanSet(TkOnuEthPort port, const McastVlanPortCfg * cfg)
    {
    U16  FAST offset;
    BOOL FAST ret = FALSE;

    offset = port*sizeof(McastVlanPortCfg);
    if (McastVlanCfgSet(port, cfg))
        {
        (void)FdsSubRecWrite(FdsGrpIdMcast, RecMcastVlan, offset,
                             cfg, sizeof(McastVlanPortCfg));
        ret = TRUE;
        }

    return ret;
    } // McastMgmtVlanSet


////////////////////////////////////////////////////////////////////////////////
/// \brief Configure the mcast configuration parameters.
///
/// \param cfg Pointer to configuration buffer
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void McastMgmtParamCfgSet(const McastParamCfg * cfg)
    {
    mcastAdminState = FALSE;
    if (cfg->snoopMode != McastSnoopNone)
        {
        mcastAdminState = TRUE;
        McastDbParamCfgSet(cfg);
        }
    } // McastMgmtParamCfgSet


////////////////////////////////////////////////////////////////////////////////
/// \brief Initialize the mcast configuration parameters from the personality.
///
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void McastMgmtParamCfgInit(void)
    {
    McastParamCfg BULK paramCfg;
    paramCfg.snoopMode = McastSnoopNone;
    (void)FdsRecRead(FdsGrpIdMcast, RecMcastCfg, &paramCfg, sizeof(McastParamCfg));
    McastMgmtParamCfgSet(&paramCfg);
    } // McastMgmtParamCfgInit


////////////////////////////////////////////////////////////////////////////////
/// \brief Set the multicast mode configuration
///
/// \param cfg The mode configuration
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void McastMgmtModeSet(const McastModeCfg BULK * cfg)
    {
    McastBaseCfg BULK baseCfg;
    baseCfg.ipv4 =
    (TestBitsAny(cfg->snoopMode, McastSnoopIgmp)) ? TRUE: FALSE;
    baseCfg.ipv6 =
    (TestBitsAny(cfg->snoopMode, McastSnoopMld)) ? TRUE: FALSE;
    baseCfg.vlanType = McastDbVlanTypeGet();
    baseCfg.opt |=
    (cfg->hostCtrl) ? McastBaseCfgOptHostCtrl : McastBaseCfgOptNone;
    McastBaseConfig(&baseCfg);

    McastDbRtModeCfgSet(cfg);
    } // McastMgmtModeSet


////////////////////////////////////////////////////////////////////////////////
/// \brief Reset the multicast management module
///
/// \Param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void McastMgmtModeCfg(const McastModeCfg BULK * modeCfg)
    {
    TkOnuEthPort BULK port;
    McastMgmtModeSet(modeCfg);
    McastDbRtOnuGrpCountSet(0);
    McastDbRtRateLimitCountSet(0);
    for (port = 0; port < McastPortMaxNum; port++)
        {
        McastDbRtPortQualSet(port,
                             (modeCfg->hostCtrl) ? McastGrpNone : McastGrpByL2DaIpSa);
        McastDbRtPortFrmCreditsSet(port, McastDbRateLimitGet(port));
        McastDbRtPortGrpCountSet(port, 0);
        }
    } // McastMgmtModeCfg


////////////////////////////////////////////////////////////////////////////////
/// \brief Reset the multicast management module
///
/// \Param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void McastMgmtReset(void)
    {
    U8 FAST port;
    // clear old configuration
    for (port = 0; port < McastPortMaxNum; port++)
        {
        // clear groups
        McastMgmtGrpClr(port);
        // clear multicat vlan config
        McastVlanCfgClr(port);
        }
    McastMgmtVlanModeClr();

    McastDbInit();
    McastVlanInit();
    McastMgmtParamCfgInit();
    } // McastMgmtReset


////////////////////////////////////////////////////////////////////////////////
/// \brief Reset the multicast management module
///
/// \Param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void McastMgmtConfig(const McastParamCfg * cfg)
    {
    McastMgmtReset();
    if (cfg != NULL)
        {
        McastMgmtParamCfgSet(cfg);
        }

    if (mcastAdminState)
        {
        McastModeCfg BULK modeCfg;
        modeCfg.snoopMode = McastDbSnoopModeGet();
        modeCfg.snoopOpt  = McastDbSnoopOptGet();
        modeCfg.hostCtrl  = FALSE;

        McastMgmtModeCfg(&modeCfg);
        McastDbRtFastLeaveSet(FALSE);
        McastMgmtVlanInit();
        }
    } // McastMgmtConfig


////////////////////////////////////////////////////////////////////////////////
/// \brief  Register multicast FDS group
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
//extern
void McastMgmtFdsGroupReg(void)
    {
    FdsGrpRegister(FdsGrpIdMcast, RecordsCount, recLookupTable);
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief Initialize the multicast module.
///
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void McastMgmtInit(void)
    {
    memset(&mcastGrpTable[0], 0,
           (sizeof(McastGrpEntry) * McastOnuGrpMaxNum));
    memset(&ssmSrcRecord, 0, sizeof(SsmSrcRecord));

    mcastVlanMode = FALSE;
    mcastVlanActCbHdl.up = NULL;
    mcastVlanActCbHdl.dn = NULL;
    mcastMgmtGroupInfo = (McastGrpInfo BULK *)mcastGroupInfoBuf;

    McastDbInit();
    McastVlanInit();
    McastBaseInit();

    McastMgmtConfig(NULL);
    } // McastMgmtInit


////////////////////////////////////////////////////////////////////////////////
/// \brief Re-Install the multicast system rules
///
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void McastMgmtSysRuleReInstall(void)
    {
    McastBaseRuleReInstall();
    } // McastMgmtSysRuleReInstall


//##############################################################################
//                    Multicast Group Management Functions
//##############################################################################

////////////////////////////////////////////////////////////////////////////////
/// \brief Get the masked mac address from group address(16 bytes).
///        For v4, the masked mac is the low 23 bits with other bits are zero.
///        For v6, the masked mac is the low 32 bits with other bits are zero.
///
/// \param v4       V4 or V6 multicast mac address
/// \param mac      Pointer to the mac buffer
/// \param grpAddr  Pointer to group address
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void McastMgmtMaskMacAddrGet(BOOL v4, MacAddr * mac, const U8 * grpAddr)
    {
    Stream BULK strm;
    StreamInit(&strm, mac->byte);
    StreamWriteU16(&strm, 0x0000);

    if (v4)
        {
        StreamWriteU8(&strm, 0x00);
        StreamWriteU8(&strm, grpAddr[McastGrpAddrMacOffset + 3]&0x7F);
        StreamWriteU8(&strm, grpAddr[McastGrpAddrMacOffset + 4]);
        StreamWriteU8(&strm, grpAddr[McastGrpAddrMacOffset + 5]);
        }
    else
        {
        StreamCopy(&strm, &grpAddr[McastGrpAddrMacOffset + 2], 4);
        }
    } // McastMgmtMaskMacAddrGet


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the mac address from group address(16 bytes).
///
/// \param v4       V4 or V6 multicast mac address
/// \param mac      Pointer to the mac buffer
/// \param grpAddr  Pointer to group address
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void McastMgmtMacAddrGet(BOOL v4, MacAddr * mac, const U8 * grpAddr)
    {
    Stream BULK strm;
    StreamInit(&strm, mac->byte);
    if (v4)
        {
        StreamWriteU8(&strm, 0x01);
        StreamWriteU8(&strm, 0x00);
        StreamWriteU8(&strm, 0x5E);
        StreamWriteU8(&strm, grpAddr[McastGrpAddrMacOffset + 3]&0x7F);
        StreamWriteU8(&strm, grpAddr[McastGrpAddrMacOffset + 4]);
        StreamWriteU8(&strm, grpAddr[McastGrpAddrMacOffset + 5]);
        }
    else
        {
        StreamWriteU8(&strm, 0x33);
        StreamWriteU8(&strm, 0x33);
        StreamCopy(&strm, &grpAddr[McastGrpAddrMacOffset + 2], 4);
        }
    } // McastMgmtMacAddrGet


////////////////////////////////////////////////////////////////////////////////
/// \brief Set the mac address into group address(16 bytes) from ip address.
///
/// \param v4       V4 or V6 multicast mac address
/// \param grpAddr  Pointer to the group address
/// \param ip       Pointer to ip address
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void McastMgmtMacAddrSet(BOOL v4, McastGrpAddr * grpAddr, const U8 * ip)
    {
    Stream BULK strm;
    StreamInit(&strm, grpAddr->byte);
    StreamSkip(&strm, (U32)McastGrpAddrMacOffset);
    if (v4)
        {
        StreamWriteU8(&strm, 0x01);
        StreamWriteU8(&strm, 0x00);
        StreamWriteU8(&strm, 0x5E);
        StreamWriteU8(&strm, ip[1]&0x7F);
        StreamWriteU8(&strm, ip[2]);
        StreamWriteU8(&strm, ip[3]);
        }
    else
        {
        StreamWriteU8(&strm, 0x33);
        StreamWriteU8(&strm, 0x33);
        StreamCopy(&strm, &ip[12], 4);
        }
    } // McastMgmtMacAddrSet


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the ipv4 address from the group entry
///
/// \param ipv4     Pointer to the ipv4 buffer
/// \param grpAddr  Pointer to group address
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void McastMgmtIpv4AddrGet(IpAddr * ipv4, const U8 * grpAddr)
    {
    memcpy(ipv4, &grpAddr[McastGrpAddrIpv4Offset], sizeof(IpAddr));
    } // McastMgmtIpv4AddrGet


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the ipv6 address from the group entry
///
/// \param ipv6     Pointer to the ipv6 buffer
/// \param grpAddr  Pointer to group address
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void McastMgmtIpv6AddrGet(IpV6Addr * ipv6, const U8 * grpAddr)
    {
    memcpy(ipv6, &grpAddr[McastGrpAddrIpv6Offset], sizeof(IpV6Addr));
    } // McastMgmtIpv6AddrGet


////////////////////////////////////////////////////////////////////////////////
/// \brief Set the ip address into group address(16 bytes).
///
/// \param v4       V4 or V6 multicast mac address
/// \param grpAddr  Pointer to the group address
/// \param ip       Pointer to ip address
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void McastMgmtIpAddrSet(BOOL v4, McastGrpAddr * grpAddr, const U8 * ip)
    {
    if (v4)
        {
        memcpy(&grpAddr->byte[McastGrpAddrIpv4Offset], ip, sizeof(IpAddr));
        }
    else
        {
        memcpy(&grpAddr->byte[McastGrpAddrIpv6Offset], ip, sizeof(IpV6Addr));
        }
    } // McastMgmtIpAddrSet


////////////////////////////////////////////////////////////////////////////////
/// \brief Check if the group info is same with the mcast vlan mode.
///
/// \param grpInfo  The new group information to check
///
/// \return
/// TRUE if same, FALSE otherwise.
////////////////////////////////////////////////////////////////////////////////
static
BOOL CheckMcastVlanMode(const McastGrpInfo BULK * grpInfo)
    {
    if (mcastVlanMode == TestBitsSet(grpInfo->qual, McastGrpByVid))
        {
        return TRUE;
        }
    else
        {
        return FALSE;
        }
    } // CheckMcastVlanMode


////////////////////////////////////////////////////////////////////////////////
/// \brief Check if the group info is same with the mcast vlan mode.
///        If different, clear all the onu rules, and set the new mode
///
/// \param grpInfo  The new group information to check
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void DoCheckMcastVlanMode(const McastGrpInfo BULK * grpInfo)
    {
    if (!CheckMcastVlanMode(grpInfo))
        {
        U8   FAST port;
        BOOL FAST newMode;
        SwitchDomainInfo BULK domainInfo;
        // Clear all the onu rules
        for (port = 0; port < McastPortMaxNum; port++)
            {
            McastMgmtGrpClr(port);
            }

        domainInfo.type = SwDomainVlan;
        newMode = TestBitsSet(grpInfo->qual, McastGrpByVid);
        (void)McastSwDomainCfg(&domainInfo, newMode);
        mcastVlanMode = newMode;
        }
    } // DoCheckMcastVlanMode


////////////////////////////////////////////////////////////////////////////////
/// \brief Check if the qual is same with the port qualifier
///
/// \param port Port to check
/// \param qual New qual
///
/// \return
/// TRUE if same, FALSE otherwise.
////////////////////////////////////////////////////////////////////////////////
static
BOOL CheckPortQual(TkOnuEthPort port, McastGrpQual qual)
    {
    return(qual == McastDbRtPortQualGet(port)) ? TRUE : FALSE;
    } // CheckPortQual


////////////////////////////////////////////////////////////////////////////////
/// \brief Check if the qual is same with the port qualifier. if different,
///        clear all the port groups.
///
/// \param port Port to check
/// \param qual New qual
///
/// \return
/// TRUE if change, FALSE otherwise.
////////////////////////////////////////////////////////////////////////////////
static
void DoCheckPortQual(TkOnuEthPort port, McastGrpQual qual)
    {
    if (!CheckPortQual(port, qual))
        {
        McastMgmtGrpClr(port);
        }
    } // DoCheckPortQual


////////////////////////////////////////////////////////////////////////////////
/// \brief Check the validation of the group info
///
/// \param grpInfo Group information
///
/// \return
/// TRUE if used, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
static
BOOL ValidGroupInfo(const McastGrpInfo BULK * grpInfo)
    {
    BOOL FAST valid;
    if ((!TestBitsAny(grpInfo->qual, McastGrpByL2Da|McastGrpByIpDa)) ||
        ( TestBitsSet(grpInfo->qual, McastGrpByL2Da|McastGrpByIpDa)))
        {
        return FALSE;
        }

    valid = TRUE;
    if (valid && TestBitsSet(grpInfo->qual, McastGrpByL2Da))
        {
        if (!McastGrpMacAddrCheck(&grpInfo->grpAddr.byte[McastGrpAddrMacOffset]))
            {
            valid = FALSE;
            }
        }

    if (valid && TestBitsSet(grpInfo->qual, McastGrpByIpDa))
        {
        if (grpInfo->v4)
            {
            if (!McastGrpDaIpV4AddrCheck(
                                        &grpInfo->grpAddr.byte[McastGrpAddrIpv4Offset]))
                {
                valid = FALSE;
                }
            }
        else
            {
            if (!McastGrpDaIpV6AddrCheck(
                                        (IpV6Addr BULK *)&grpInfo->grpAddr.byte[McastGrpAddrIpv6Offset]))
                {
                valid = FALSE;
                }
            }
        }

    if (valid && TestBitsSet(grpInfo->qual, McastGrpByVid))
        {
        if (!McastVlanValid(grpInfo->grpVid))
            {
            valid = FALSE;
            }
        }

    if (!valid)
        {
        McastDebug(McastDebugLevel, ("Invalid Group Info.\n"));
        }
    return valid;
    } // ValidGroupInfo


////////////////////////////////////////////////////////////////////////////////
/// \brief Compare the group address with the group info, and return the result
///
/// \param grpEntry Group Entry
/// \param grpInfo  Group information
///
/// \return
/// TRUE if equal, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
static
BOOL CompareGroupAddr(const McastGrpEntry BULK * grpEntry,
                      const McastGrpInfo BULK * grpInfo)
    {
    BOOL FAST match = FALSE;
    U8   BULK * BULK entAddr  = grpEntry->grpAddr.byte;;
    U8   BULK * BULK infoAddr = grpInfo->grpAddr.byte;
    McastGrpQual BULK tmpQual = grpEntry->qual & grpInfo->qual;
    if (grpEntry->v4 != grpInfo->v4)
        {
        return match;
        }
    if (TestBitsSet(tmpQual, McastGrpByL2Da))
        {
        MacAddr BULK entMac;
        MacAddr BULK infoMac;
        McastMgmtMaskMacAddrGet(grpEntry->v4, &entMac, entAddr);
        McastMgmtMaskMacAddrGet(grpEntry->v4, &infoMac, infoAddr);
        if (memcmp(&entMac, &infoMac, sizeof(MacAddr)) == 0)
            {
            match = TRUE;
            }
        }
    if (TestBitsSet(tmpQual, McastGrpByIpDa))
        {
        if (grpEntry->v4)
            {
            if (memcmp(&entAddr[McastGrpAddrIpv4Offset],
                       &infoAddr[McastGrpAddrIpv4Offset], sizeof(IpAddr)) == 0)
                {
                match = TRUE;
                }
            }
        else
            {
            if (memcmp(&entAddr[McastGrpAddrIpv6Offset],
                       &infoAddr[McastGrpAddrIpv6Offset], sizeof(IpV6Addr)) == 0)
                {
                match = TRUE;
                }
            }
        }

    return match;
    } // CompareGroupAddr


////////////////////////////////////////////////////////////////////////////////
/// \brief Return if the entry is used, or not
///
/// \param grpEntry Group entry
///
/// \return
/// TRUE if used, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
static
BOOL GroupInUse(const McastGrpEntry BULK * grpEntry)
    {
    return(grpEntry->ssmDomainMap != 0);
    } // GroupInUse


////////////////////////////////////////////////////////////////////////////////
/// \brief Allocate a free group from the group table
///
/// \param None
///
/// \return
/// Group entry, Null if there is no free entry
////////////////////////////////////////////////////////////////////////////////
static
McastGrpEntry BULK * GroupAlloc(void)
    {
    U8 FAST i;
    McastGrpEntry BULK * BULK grpEntry;

    grpEntry = &mcastGrpTable[0];
    for (i = 0; i < McastOnuGrpMaxNum; i++)
        {
        if (!GroupInUse(grpEntry))
            {
            // found it
            memset(grpEntry, 0, sizeof(McastGrpEntry));
            return grpEntry;
            }
        grpEntry++;
        }

    return NULL;
    } // GroupAlloc


////////////////////////////////////////////////////////////////////////////////
/// \brief Initialize the base group information.
///
/// \param port     Port to initialize
/// \param grpEntry Group entry
/// \param grpInfo  Group information
///
/// \return
/// TRUE if success, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
static
void GroupInit(TkOnuEthPort port, McastGrpEntry BULK * grpEntry,
               const McastGrpInfo BULK * grpInfo)
    {
    UNUSED(port);
    grpEntry->qual = grpInfo->qual;
    grpEntry->grpVid = grpInfo->grpVid;
    grpEntry->v4 = grpInfo->v4;
    memcpy(&grpEntry->grpAddr, &grpInfo->grpAddr, sizeof(McastGrpAddr));
    } // GroupInit


////////////////////////////////////////////////////////////////////////////////
/// \brief Get a used group from the given addressF
///
/// \param grpEntry Group entry to begin
///
/// \return
/// NULL if not find.
////////////////////////////////////////////////////////////////////////////////
static
McastGrpEntry * GroupGet(const McastGrpEntry BULK * grpEntry)
    {
    McastGrpEntry BULK * lastEntry = &mcastGrpTable[McastOnuGrpMaxNum - 1];

    while (grpEntry <= lastEntry)
        {
        if (GroupInUse(grpEntry))
            {
            return grpEntry;
            }
        grpEntry++;
        }
    return NULL;
    } // GroupGet


////////////////////////////////////////////////////////////////////////////////
/// \brief Find a group entry from the group table
///
/// \param grpInfo Group information
///
/// \return
/// Group entry, Null if not found
////////////////////////////////////////////////////////////////////////////////
static
McastGrpEntry BULK * GroupFind(const McastGrpInfo BULK * grpInfo)
    {
    McastGrpEntry BULK * BULK startEntry;
    startEntry = &mcastGrpTable[0];
    while (TRUE)
        {
        startEntry = GroupGet(startEntry);
        if (startEntry == NULL)
            {
            break;
            }

        if (CompareGroupAddr(startEntry, grpInfo))
            {
            return startEntry;
            }
        startEntry++;
        }
    return NULL;
    } // GroupFind


#if IgmpV3MldV2Supported
//##############################################################################
//                      Multicast Ssm Source Ip Funtcions
//##############################################################################

////////////////////////////////////////////////////////////////////////////////
/// \brief Build Ssm source domain information
///
/// \param domainInfo Pointer to the domainInfo
/// \param v4    Ipv4/Ipv6 address
/// \param srcIp Pointer to Ip address
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void BuildSsmSrcDomainInfo(SwitchDomainInfo * domainInfo,
                           BOOL v4, const SaIpAddr * srcIp)
    {
    domainInfo->type = SwDomainSrcIp;
    if (v4)
        {
        domainInfo->field1 = LueFieldInIpV4IpV6Sa;
        domainInfo->value1 = (U64)srcIp->ipv4;
        domainInfo->field2 = LueFieldNumFields;
        }
    else
        {
        U64 BULK value;
        // ipv6 low 8 bytes address
        memcpy(&value, &srcIp->ipv6.byte[8], sizeof(value));
        domainInfo->field1 = LueFieldInIpV4IpV6Sa;
        domainInfo->value1 = value;

        // ipv6 high 8 bytes address
        memcpy(&value, &srcIp->ipv6.byte[0], sizeof(value));
        domainInfo->field2 = LueFieldInIpV6SaHi;
        domainInfo->value2 = value;
        }
    } // BuildSsmSrcDomainInfo


////////////////////////////////////////////////////////////////////////////////
/// \brief Get a source ip from the stream
///
/// \param v4    Ipv4 or Ipv6 source address
/// \param strm  Stream from which to get
/// \param srcIp Pointer to source ip to return
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void GetSsmSrc(BOOL v4, Stream * strm, SaIpAddr * srcIp)
    {
    if (v4)
        {
        srcIp->ipv4 = StreamReadU32(strm);
        }
    else
        {
        memcpy(srcIp->ipv6.byte, strm->cur, sizeof(IpV6Addr));
        StreamSkip(strm, sizeof(IpV6Addr));
        }
    } // GetSsmSrc


////////////////////////////////////////////////////////////////////////////////
/// \brief add the rules related to the source id
///
/// \param srcId  source Id
///
/// \return
/// TRUE if success, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
static
BOOL SsmDomainRuleAddDel(SsmSrcId srcId, BOOL add)
    {
    U8  FAST portIdx;
    U16 FAST domainId;
    MacAddr BULK mac;
    McastGrpEntry BULK * BULK startEntry;
    startEntry = &mcastGrpTable[0];

    domainId = SrcIdToSsmDomainId(srcId);
    while (TRUE)
        {
        startEntry = GroupGet(startEntry);
        if (startEntry == NULL)
            {
            break;
            }

        if (!TestBitsSet(startEntry->qual, McastGrpByL2Da))
            {
            startEntry++;
            continue;
            }
        McastMgmtMacAddrGet(startEntry->v4, &mac, startEntry->grpAddr.byte);
        for (portIdx = 0; portIdx < McastPortMaxNum; portIdx++)
            {
            U64 FAST ssmDomainMap = SsmDomainMap(portIdx, srcId);
            if (add)
                {
                if (TestBitsSet(startEntry->ssmDomainMap,
                                SsmDomainMap(portIdx, SsmSrcIdNone)))
                    {
                    if (!McastGrpBinAddMac(portIdx, &mac, domainId))
                        {
                        return FALSE;
                        }
                    McastBitsOrEq(startEntry->ssmDomainMap, ssmDomainMap);
                    }
                }
            else
                {
                if (TestBitsSet(startEntry->ssmDomainMap, ssmDomainMap))
                    {
                    (void)McastGrpBinDelMac(portIdx, &mac, domainId);
                    McastBitsAndEqNot(startEntry->ssmDomainMap, ssmDomainMap);
                    }
                }
            }
        startEntry++;
        }

    return TRUE;
    } // SsmDomainRuleAddDel




////////////////////////////////////////////////////////////////////////////////
/// \brief update entry timer according to the source list
///        entry
///
/// \param ssmEntry Pointer to the ssm entry
/// \param srcmap   The source map info.
/// \param timeVal    The new timer val.
///
/// \return
/// TRUE if success, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
static
void SsmSrcTimerUpdate(McastGrpSsmEntry BULK * ssmEntry,
							   U32 srcMap, U16 timeVal)
	{
	SsmSrcId FAST srcId;

	for (srcId = SsmSrcIdBegin; srcId < SsmSrcIdEnd; srcId++)
		{
		if ((TestBitsSet(srcMap,(U32)McastBitMap(srcId))) &&
			(TestBitsSet(ssmEntry->srcMap,(U32)McastBitMap(srcId))))
			{
			ssmEntry->srcTimer[srcId-SsmSrcIdBegin] = timeVal;
			}
		}

	}//SsmSrcTimerUpdate



////////////////////////////////////////////////////////////////////////////////
/// \brief Check if this source Id entry exists or not
///
/// \param None
///
/// \return
/// TRUE if exist, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
static
BOOL SsmSrcIdExist(SsmSrcId srcId)
    {
    BOOL FAST ret = FALSE;
    if (srcId < SsmSrcIdEnd)
        {
        ret = (srcId == SsmSrcIdNone) ? TRUE :
              (ssmSrcRecord.entryList[srcId].domainId != SsmDomainIdNone);
        }
    return ret;
    } // SsmSrcIdExist


////////////////////////////////////////////////////////////////////////////////
/// \brief Check if this source Id entry is used or not
///
/// \param None
///
/// \return
/// TRUE if used, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
static
BOOL SsmSrcIdInUse(SsmSrcId srcId)
    {
    BOOL FAST ret = FALSE;
    if (srcId < SsmSrcIdEnd)
        {
        ret = (srcId == SsmSrcIdNone) ? TRUE :
              (ssmSrcRecord.entryList[srcId].inUse != 0);
        }
    return ret;
    } // SsmSrcIdInUse


////////////////////////////////////////////////////////////////////////////////
/// \brief Free a ssm source id
///
/// \param srcId Ssm source id to free
///
/// \return
/// TRUE if used, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
static
void SsmSrcIdFree(SsmSrcId srcId)
    {
    if ((srcId < SsmSrcIdEnd)  && (srcId > SsmSrcIdNone) &&
        SsmSrcIdExist(srcId)   && (!SsmSrcIdInUse(srcId)))
        {
        SwitchDomainInfo BULK domainInfo;
        BuildSsmSrcDomainInfo(&domainInfo,
                              ssmSrcRecord.entryList[srcId].v4,
                              &ssmSrcRecord.entryList[srcId].srcIp);
        domainInfo.domainId = SrcIdToSsmDomainId(srcId);
        (void)McastSwDomainCfg(&domainInfo, FALSE);
        memset(&ssmSrcRecord.entryList[srcId], 0, sizeof(SsmSrcEntry));
        }
    } // SsmSrcIdFree


////////////////////////////////////////////////////////////////////////////////
/// \brief Decrease the reference number for each source id of the map.
///
/// \param srcIdMap Ssm source id map
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void SsmSrcIdMapDec(U32 srcIdMap)
    {
    SsmSrcId srcId;
    for (srcId = SsmSrcIdBegin; srcId < SsmSrcIdEnd; srcId++)
        {
        if (TestBitsSet(srcIdMap, (U32)McastBitMap(srcId)) &&
            SsmSrcIdExist(srcId) && SsmSrcIdInUse(srcId))
            {
            ssmSrcRecord.entryList[srcId].inUse--;
            }
        }
    } // SsmSrcIdMapDec


////////////////////////////////////////////////////////////////////////////////
/// \brief Increase the reference number for each source id of the map.
///
/// \param srcIdMap Ssm source id map
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void SsmSrcIdMapInc(U32 srcIdMap)
    {
    SsmSrcId srcId;
    for (srcId = SsmSrcIdBegin; srcId < SsmSrcIdEnd; srcId++)
        {
        if (TestBitsSet(srcIdMap, (U32)McastBitMap(srcId)) &&
            SsmSrcIdExist(srcId))
            {
            ssmSrcRecord.entryList[srcId].inUse++;
            }
        }
    } // SsmSrcIdMapInc



////////////////////////////////////////////////////////////////////////////////
/// \brief Update the ssm domain rules
///
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void SsmSrcIdUpdate(void)
    {
    SsmSrcId FAST srcId;
    for (srcId = SsmSrcIdBegin; srcId < SsmSrcIdEnd; srcId++)
        {
        if (SsmSrcIdExist(srcId) && (!SsmSrcIdInUse(srcId)))
            {
            (void)SsmDomainRuleAddDel(srcId, FALSE);
            SsmSrcIdFree(srcId);
            }
        }
    } // SsmSrcIdUpdate


////////////////////////////////////////////////////////////////////////////////
/// \brief Allocate a ssm source id for the source
///
/// \param v4    Ipv4/Ipv6 address
/// \param srcIp Pointer to Ip address
///
/// \return
/// Source Id
////////////////////////////////////////////////////////////////////////////////
static
SsmSrcId SsmSrcAlloc(BOOL v4, const SaIpAddr * srcIp)
    {
    SsmSrcId FAST srcId;
    SwitchDomainInfo BULK domainInfo;
    BuildSsmSrcDomainInfo(&domainInfo, v4, srcIp);
    for (srcId = SsmSrcIdBegin; srcId < SsmSrcIdEnd; srcId++)
        {
        if (!SsmSrcIdExist(srcId))
            {
            domainInfo.domainId = SrcIdToSsmDomainId(srcId);
            // Set the domain info
            if (!McastSwDomainCfg(&domainInfo, TRUE))
                {
                return SsmSrcIdEnd;
                }
            ssmSrcRecord.entryList[srcId].v4 = v4;
            ssmSrcRecord.entryList[srcId].inUse = 0;
            ssmSrcRecord.entryList[srcId].domainId = domainInfo.domainId;
            memcpy(&ssmSrcRecord.entryList[srcId].srcIp,
                   srcIp, sizeof(SaIpAddr));
            break;
            }
        }
    return srcId;
    } // SsmSrcAlloc


////////////////////////////////////////////////////////////////////////////////
/// \brief Find a ssm source Ip
///
/// \param v4    Ipv4/Ipv6 address
/// \param srcIp Pointer to Ip address
///
/// \return
/// Source Id if find, SsmSrcIdEnd otherwise
////////////////////////////////////////////////////////////////////////////////
static
SsmSrcId SsmSrcFind(BOOL v4, const SaIpAddr * srcIp)
    {
    SsmSrcId FAST srcId;
    for (srcId = SsmSrcIdBegin; srcId < SsmSrcIdEnd; srcId++)
        {
        if (SsmSrcIdExist(srcId) &&
            (ssmSrcRecord.entryList[srcId].v4 == v4) &&
            (memcmp(&ssmSrcRecord.entryList[srcId].srcIp,
                    srcIp, sizeof(SaIpAddr)) == 0))
            {
            // Find it.
            break;
            }
        }
    return srcId;
    } // SsmSrcFind


////////////////////////////////////////////////////////////////////////////////
/// \brief Convert the ssm entry into source Id map.
///
/// \param ssmEntry Pointer to the ssm entry
///
/// \return
/// Source Id Map;
////////////////////////////////////////////////////////////////////////////////
static
U32 SsmEntryToSrcIdMap(const McastGrpSsmEntry BULK * ssmEntry)
    {
    U32 FAST srcIdMap;
    SsmSrcId FAST srcId;
    srcIdMap = 0;
    if (ssmEntry->type == ModeIsInclude)
        {
        srcIdMap = ssmEntry->srcMap;
        }
    else
        {
        McastBitsOrEq(srcIdMap, (U32)McastBitMap(SsmSrcIdNone));
        for (srcId = SsmSrcIdBegin; srcId < SsmSrcIdEnd; srcId++)
            {
            if (SsmSrcIdExist(srcId) &&
                (!TestBitsSet(ssmEntry->srcMap, (U32)McastBitMap(srcId))))
                {
                McastBitsOrEq(srcIdMap, (U32)McastBitMap(srcId));
                }
            }
        }
    return srcIdMap;
    } // SsmEntryToSrcIdMap


////////////////////////////////////////////////////////////////////////////////
/// \brief Convert ssm information into ssm entry.
///
/// \param ssmInfo  Pointer to the ssm info
/// \param ssmEntry Pointher to the ssm entry
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void SsmInfoToSsmEntry(BOOL v4,
                       const McastGrpSsmInfo  BULK * ssmInfo,
                       McastGrpSsmEntry BULK * ssmEntry)
    {
    U8 FAST i;
    Stream BULK strm;
    SsmSrcId BULK srcId;
    SaIpAddr BULK srcIp;
    memset(ssmEntry, 0, sizeof(McastGrpSsmEntry));
    StreamInit(&strm, ssmInfo->byte);
    ssmEntry->type = ssmInfo->type;
    for (i = 0; i < ssmInfo->srcNum; i++)
        {
        memset(&srcIp, 0, sizeof(SaIpAddr));
        GetSsmSrc(v4, &strm, &srcIp);
        srcId = SsmSrcFind(v4, &srcIp);
        if (srcId == SsmSrcIdEnd)
            {
            McastBitsOrEq(ssmEntry->srcMap,
                          (U32)McastBitMap(i + SsmSrcIdEnd));
            }
        else
            {
            McastBitsOrEq(ssmEntry->srcMap, (U32)McastBitMap(srcId));
            }
        }
    } // SsmInfoToSsmEntry


////////////////////////////////////////////////////////////////////////////////
/// \brief Constructure the new ssm entry based on the ssm info and current ssm
///        entry
///
/// \param curSsmEntry Pointer to the ssm entry
/// \param extSsmInfo  Pointer to the ssm info
/// \param newSsmEntry Pointher to the ssm entry built
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void SsmEntryBuild(BOOL v4,
                   const McastGrpSsmEntry BULK * curSsmEntry,
                   const McastGrpSsmInfo  BULK * extSsmInfo,
                   McastGrpSsmEntry BULK * newSsmEntry)
    {
    BOOL FAST inMode;
    SsmInfoToSsmEntry(v4, extSsmInfo, newSsmEntry);
	
	if (extSsmInfo->type != BlockOldSources)
		{
		SsmSrcTimerUpdate(curSsmEntry,
				newSsmEntry->srcMap,McastlargeTimerValue);
		}
    inMode = ((extSsmInfo->type == ModeIsInclude) ||
              (extSsmInfo->type == ChangeToIncludeMode) ||
              (extSsmInfo->type == AllowNewSources));

    if (curSsmEntry->type == InvalidMode)
        {
        // InMode (TRUE) : Include {B}
        // InMode (FALSE): Exclude {B}
        newSsmEntry->type= (inMode) ? ModeIsInclude : ModeIsExclude;
        }
    else if (curSsmEntry->type == ModeIsExclude)
        {
        newSsmEntry->type = ModeIsExclude;
        if (inMode)
            {
            // Exclude {A - B}
            newSsmEntry->srcMap =
            SrcIdMapSub(curSsmEntry->srcMap, newSsmEntry->srcMap);
            }
        else
            {
            // Exclude {A interSect B}
            newSsmEntry->srcMap =
            SrcIdMapInterSect(curSsmEntry->srcMap, newSsmEntry->srcMap);
            }
        }
    else
        {
        if (inMode)
            {
            // Include {A + B}
            newSsmEntry->type = ModeIsInclude;
            newSsmEntry->srcMap =
            SrcIdMapAdd(curSsmEntry->srcMap, newSsmEntry->srcMap);
            }
        else
            {
            // Exclude {B - A}
            newSsmEntry->type = ModeIsExclude;
            newSsmEntry->srcMap =
            SrcIdMapSub(newSsmEntry->srcMap, curSsmEntry->srcMap);
            }
        }
    } // SsmEntryBuild


////////////////////////////////////////////////////////////////////////////////
/// \brief Add the new sources from the ssm info based on the ssm entry
///        entry
///
/// \param ssmEntry Pointer to the ssm entry
/// \param ssmInfo  Pointer to the ssm info
///
/// \return
/// TRUE if success, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
static
BOOL SsmSrcAdd(BOOL v4, McastGrpSsmInfo  BULK * ssmInfo,
               McastGrpSsmEntry BULK * ssmEntry)
    {
    U8 FAST i;
    BOOL FAST ret;
    Stream BULK strm;
    SaIpAddr BULK srcIp;
    SsmSrcId BULK srcId;

    ret = TRUE;
    StreamInit(&strm, ssmInfo->byte);
    for (i = 0; i < ssmInfo->srcNum; i++)
        {
        memset(&srcIp, 0, sizeof(SaIpAddr));
        GetSsmSrc(v4, &strm, &srcIp);
        if (TestBitsSet(ssmEntry->srcMap, (U32)McastBitMap(i + SsmSrcIdEnd)))
            {
            srcId = SsmSrcAlloc(v4, &srcIp);
            if ((srcId == SsmSrcIdEnd) || ((srcId != SsmSrcIdEnd) &&
                                           (!SsmDomainRuleAddDel(srcId, TRUE))))
                {
                ret = FALSE;
                }
            if (ret)
                {
                McastBitsOrEq(ssmEntry->srcMap, (U32)McastBitMap(srcId));
                McastBitsAndEqNot(ssmEntry->srcMap,
                                  (U32)McastBitMap(i + SsmSrcIdEnd));
				//lint -e{661}
				ssmEntry->srcTimer[srcId-SsmSrcIdBegin] = McastlargeTimerValue;
                }
            else
                {
                SsmSrcIdUpdate();
                break;
                }
            }
        }
    return ret;
    } // SsmSrcAdd



#endif

////////////////////////////////////////////////////////////////////////////////
/// \brief Add/Delete the group with vid into the group entry
///
/// \param port Port to add/delete
/// \param grpEntry Group Entry to add/delete
///
/// \return
/// TRUE if success, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
static
BOOL GroupVidRuleAddDel(TkOnuEthPort port,
                        McastGrpEntry BULK * grpEntry, BOOL add)
    {
    U16 FAST domainId;
    MacAddr BULK mac;
    U32 BULK ipLowBits;

    domainId = McastVlanGetVid(grpEntry->grpVid);
    if (TestBitsSet(grpEntry->qual, McastGrpByL2Da))
        {
        McastMgmtMacAddrGet(grpEntry->v4, &mac, grpEntry->grpAddr.byte);
        if (add)
            {
            if (!McastGrpBinAddMac(port, &mac, domainId))
                {
                return FALSE;
                }
            }
        else
            {
            (void)McastGrpBinDelMac(port, &mac, domainId);
            }
        }
    else
        {
        memcpy(&ipLowBits,
               &grpEntry->grpAddr.byte[McastGrpAddrIpv4Offset], sizeof(U32));
        if (add)
            {
            if (!McastGrpBinAddIp(port, ipLowBits, domainId))
                {
                return FALSE;
                }
            }
        else
            {
            (void)McastGrpBinDelIp(port, ipLowBits, domainId);
            }
        }
    if (add)
        {
        McastBitsOrEq(grpEntry->ssmDomainMap,
                      SsmDomainMap(port, SsmSrcIdNone));
        }
    else
        {
        McastBitsAndEqNot(grpEntry->ssmDomainMap,
                          SsmDomainMap(port, SsmSrcIdNone));
        }
    return TRUE;
    } // GroupVidRuleAddDel


#if IgmpV3MldV2Supported
////////////////////////////////////////////////////////////////////////////////
/// \brief Update the rules for current group without vid based on the new
///        ssm entry
///
/// \param port     Port to update
/// \param grpEntry Group Entry to update
/// \param ssmEntry New ssm entry
///
/// \return
/// TRUE if success, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
static
BOOL GroupNonVidRuleUpdate(TkOnuEthPort port,
                           McastGrpEntry BULK * grpEntry,
                           const McastGrpSsmEntry BULK * ssmEntry)
    {
    BOOL FAST ret;
    U16 BULK domainId;
    SsmSrcId BULK srcId;
    MacAddr BULK mac;
    U32 BULK srcIdMap;
    U64 BULK copySsmDomainMap;
    ret = TRUE;
    srcIdMap = SsmEntryToSrcIdMap(ssmEntry);
    copySsmDomainMap = grpEntry->ssmDomainMap;
    McastMgmtMacAddrGet(grpEntry->v4, &mac, grpEntry->grpAddr.byte);

    // update the rules
    for (srcId = SsmSrcIdNone; srcId < SsmSrcIdEnd; srcId++)
        {
        domainId = SrcIdToSsmDomainId(srcId);
        if (TestBitsSet(grpEntry->ssmDomainMap, SsmDomainMap(port, srcId)) &&
            (!TestBitsSet(srcIdMap, (U32)McastBitMap(srcId))))
            {
            // delete the rule
            (void)McastGrpBinDelMac(port, &mac, domainId);
            McastBitsAndEqNot(grpEntry->ssmDomainMap,
                              SsmDomainMap(port, srcId));
            }
        else if (TestBitsSet(srcIdMap, (U32)McastBitMap(srcId)) &&
                 (!TestBitsSet(grpEntry->ssmDomainMap, SsmDomainMap(port, srcId))))
            {
            if (!McastGrpBinAddMac(port, &mac, domainId))
                {
                ret = FALSE;
                break;
                }
            McastBitsOrEq(grpEntry->ssmDomainMap, SsmDomainMap(port, srcId));
            }
        else
            {
            ; // Do nothing.
            }
        }
    if (!ret)
        {
        // restore the rules
        for (srcId = SsmSrcIdNone; srcId < SsmSrcIdEnd; srcId++)
            {
            domainId = SrcIdToSsmDomainId(srcId);
            if (TestBitsSet(grpEntry->ssmDomainMap,
                            SsmDomainMap(port, srcId)) &&
                (!TestBitsSet(copySsmDomainMap, SsmDomainMap(port, srcId))))
                {
                (void)McastGrpBinDelMac(port, &mac, domainId);
                McastBitsAndEqNot(grpEntry->ssmDomainMap,
                                  SsmDomainMap(port, srcId));
                }
            else if (TestBitsSet(copySsmDomainMap,
                                 SsmDomainMap(port, srcId)) &&
                     (!TestBitsSet(grpEntry->ssmDomainMap,
                                   SsmDomainMap(port, srcId))))
                {

                (void)McastGrpBinAddMac(port, &mac, domainId);
                McastBitsOrEq(grpEntry->ssmDomainMap,
                              SsmDomainMap(port, srcId));
                }
            else
                {
                ; // Do nothing
                }
            }
        }
    return ret;
    } // GroupNonVidRuleUpdate

#else

////////////////////////////////////////////////////////////////////////////////
/// \brief Update the rules for current group without vid based on the new
///        ssm entry
///
/// \param port     Port to update
/// \param grpEntry Group Entry to update
/// \param ssmEntry New ssm entry
///
/// \return
/// TRUE if success, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
static
BOOL GroupNonVidRuleUpdate(TkOnuEthPort port,
                           McastGrpEntry BULK * grpEntry,
                           const McastGrpSsmEntry BULK * ssmEntry)
    {
    MacAddr BULK mac;
    U16 BULK domainId;
    McastMgmtMacAddrGet(grpEntry->v4, &mac, grpEntry->grpAddr.byte);
    domainId = SrcIdToSsmDomainId(SsmSrcIdNone);
    if (ssmEntry->type == ModeIsInclude) // {Include 0}
        {
        (void)McastGrpBinDelMac(port, &mac, domainId);
        McastBitsAndEqNot(grpEntry->ssmDomainMap,
                          SsmDomainMap(port, SsmSrcIdNone));
        }
    else  // {Exclude 0}
        {
        if (!McastGrpBinAddMac(port, &mac, domainId))
            {
            return FALSE;
            }
        McastBitsOrEq(grpEntry->ssmDomainMap,
                      SsmDomainMap(port, SsmSrcIdNone));
        }
    return TRUE;
    } // GroupNonVidRuleUpdate
#endif


////////////////////////////////////////////////////////////////////////////////
/// \brief Add a group into the lue and update the group management database.
///
/// \param grpEntry Group entry
/// \param grpInfo Group information
///
/// \return
/// TRUE if success, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
static
BOOL GroupAdd(TkOnuEthPort port, McastGrpEntry BULK * grpEntry,
              const McastGrpInfo BULK * grpInfo)
    {
    McastGrpSsmEntry BULK newSsmEntry;
    memset(&newSsmEntry, 0, sizeof(McastGrpSsmEntry));
    if (TestBitsSet(grpEntry->qual, McastGrpByVid))
        {
        if ((grpEntry->grpVid != grpInfo->grpVid) ||
            (!GroupVidRuleAddDel(port, grpEntry, TRUE)))
            {
            return FALSE;
            }
        }
    else
        {
#if IgmpV3MldV2Supported
        SsmEntryBuild(grpEntry->v4,
                      &grpEntry->ssmEntry[port],
                      &grpInfo->ssmInfo,
                      &newSsmEntry);

        if (!SsmSrcAdd(grpEntry->v4, &grpInfo->ssmInfo, &newSsmEntry))
            {
            return FALSE;
            }

        if (!GroupNonVidRuleUpdate(port, grpEntry, &newSsmEntry))
            {
            SsmSrcIdUpdate();
            return FALSE;
            }
#else
        newSsmEntry.type = ModeIsExclude;
        newSsmEntry.srcMap = 0;
        if (!GroupNonVidRuleUpdate(port, grpEntry, &newSsmEntry))
            {
            return FALSE;
            }
#endif
        }

    grpEntry->grpState[port] = McastGrpStateActive;
    grpEntry->timerVal[port] = McastlargeTimerValue;
    grpEntry->proxyLmqCount[port] = 0;
    grpEntry->proxyState[port] = FALSE;
    grpEntry->ssmEntry[port].type = newSsmEntry.type;
    grpEntry->ssmEntry[port].srcMap = newSsmEntry.srcMap;
#if IgmpV3MldV2Supported
	memcpy(grpEntry->ssmEntry[port].srcTimer,
			newSsmEntry.srcTimer,sizeof(newSsmEntry.srcTimer));
    SsmSrcIdMapInc(grpEntry->ssmEntry[port].srcMap);
#endif
    McastDbRtPortGrpInc(port);
    if (McastDbRtPortQualGet(port) == McastGrpNone)
        {
        McastDbRtPortQualSet(port, grpInfo->qual);
        }
    return TRUE;
    } // GroupAdd


////////////////////////////////////////////////////////////////////////////////
/// \brief Delete a group from the lue and update the group management database.
///
/// \param port The port to delete
/// \param grpEntry Group entry
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void GroupDel(TkOnuEthPort port, McastGrpEntry BULK * grpEntry)
    {
    McastGrpSsmEntry BULK newSsmEntry;
    if (!TestBitsAny(grpEntry->ssmDomainMap, SsmDomainMapPort(port)))
        {
        return;
        }

    if (TestBitsSet(grpEntry->qual, McastGrpByVid))
        {
        (void)GroupVidRuleAddDel(port, grpEntry, FALSE);
        }
    else
        {
        newSsmEntry.type = ModeIsInclude;
        newSsmEntry.srcMap = 0;
#if IgmpV3MldV2Supported
        SsmSrcIdMapDec(grpEntry->ssmEntry[port].srcMap);
        SsmSrcIdUpdate();
#endif
        (void)GroupNonVidRuleUpdate(port, grpEntry, &newSsmEntry);
        }

    if (GroupInUse(grpEntry))
        {
        grpEntry->grpState[port] = McastGrpStateUnused;
        grpEntry->timerVal[port] = 0;
        grpEntry->proxyState[port]  = FALSE;
        grpEntry->proxyLmqCount[port] = 0;
        memset(&grpEntry->ssmEntry[port], 0, sizeof(McastGrpSsmEntry));
        McastDbRtPortGrpDec(port);
        }
    else
        {
        memset(grpEntry, 0, sizeof(McastGrpEntry));
        McastDbRtPortGrpDec(port);
        McastDbRtOnuGrpCountDec();
        }
    } // GroupDel


#if IgmpV3MldV2Supported
////////////////////////////////////////////////////////////////////////////////
/// \brief Add a group into the lue and update the group management database.
///
/// \param grpEntry Group entry
/// \param grpInfo Group information
///
/// \return
/// TRUE if success, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
static
BOOL GroupUpdate(TkOnuEthPort port, McastGrpEntry BULK * grpEntry,
                 const McastGrpInfo BULK * grpInfo,
                 const McastGrpSsmEntry BULK * newSsmEntry)
    {
    BOOL FAST ret;
    BOOL FAST srcAdded;
    BOOL FAST srcDeleted;
    U32  FAST delSrcIdMap;
    U32  FAST addSrcIdMap;
    SsmSrcRecord BULK copySsmSrcRec;

    ret = TRUE;
    srcDeleted = FALSE;
    srcAdded   = FALSE;
    memcpy(&copySsmSrcRec, &ssmSrcRecord, sizeof(SsmSrcRecord));

    delSrcIdMap =
    SrcIdMapSub(grpEntry->ssmEntry[port].srcMap, newSsmEntry->srcMap);
    // Delete unused source Id
    if (delSrcIdMap != 0)
        {
        SsmSrcIdMapDec(delSrcIdMap);
        SsmSrcIdUpdate(); // Some source record may be deleted
        srcDeleted = TRUE;
        }

    // Add new sources
    if (SsmSrcAdd(grpEntry->v4, &grpInfo->ssmInfo, newSsmEntry))
        {
        srcAdded = TRUE;
        }
    else
        {
        ret = FALSE;
        }

    // Update the rules
    if (ret && (!GroupNonVidRuleUpdate(port, grpEntry, newSsmEntry)))
        {
        ret = FALSE;
        }

    // Update the data base
    if (ret)
        {
        addSrcIdMap =
        SrcIdMapSub(newSsmEntry->srcMap, grpEntry->ssmEntry[port].srcMap);
        SsmSrcIdMapInc(addSrcIdMap);
        grpEntry->ssmEntry[port].type = newSsmEntry->type;
        grpEntry->ssmEntry[port].srcMap = newSsmEntry->srcMap;
		memcpy(grpEntry->ssmEntry[port].srcTimer,
			newSsmEntry->srcTimer,sizeof(newSsmEntry->srcTimer));
        }
    else
        {
        // Delete sources added
        if (srcAdded)
            {
            SsmSrcIdUpdate();
            }

        // Restore the sources deleted.
        if (srcDeleted)
            {
            SsmSrcId FAST srcId;
            for (srcId = SsmSrcIdBegin; srcId < SsmSrcIdEnd; srcId++)
                {
                if (TestBitsSet(delSrcIdMap, (U32)McastBitMap(srcId)) &&
                    (!SsmSrcIdExist(srcId)))
                    {
                    (void)SsmDomainRuleAddDel(SsmSrcAlloc(grpInfo->v4,
                                                          &copySsmSrcRec.entryList[srcId].srcIp), TRUE);
                    }
                }
            SsmSrcIdMapInc(delSrcIdMap);
            }

        // Restore current port group rules.
        (void)GroupNonVidRuleUpdate(port, grpEntry,
                                    &grpEntry->ssmEntry[port]);
        }

    return ret;
    } // GroupUpdate
#endif


////////////////////////////////////////////////////////////////////////////////
/// \brief Set to non vlan mode
///
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void McastMgmtVlanModeClr(void)
    {
    if (mcastVlanMode)
        {
        SwitchDomainInfo BULK domainInfo;
        domainInfo.type = SwDomainVlan;
        (void)McastSwDomainCfg(&domainInfo, FALSE);
        mcastVlanMode = FALSE;
        }
    } // McastMgmtVlanModeClr


////////////////////////////////////////////////////////////////////////////////
/// \brief Add a multicast group for the port
///
/// \param port    Port to add
/// \param grpInfo Group information
///
/// \return
/// TRUE if success, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL McastMgmtGrpAdd(TkOnuEthPort port, const McastGrpInfo BULK * grpInfo)
    {
    BOOL retVal = FALSE;
    McastGrpEntry BULK * grpEntry;

    if (ValidGroupInfo(grpInfo))
        {
        DoCheckMcastVlanMode(grpInfo);
        DoCheckPortQual(port, grpInfo->qual);

        grpEntry = GroupFind(grpInfo);
        if (grpEntry != NULL)
            {
            if (grpEntry->grpState[port] == McastGrpStateUnused)
                {
                if (McastDbRtPortGrpLimitCheck(port))
                    {
                    retVal = GroupAdd(port, grpEntry, grpInfo);
                    }
                }
#if IgmpV3MldV2Supported
            else if (TestBitsSet(grpInfo->qual, McastGrpByIpSa))
                {
                McastGrpSsmEntry BULK newSsmEntry;
                SsmEntryBuild(grpEntry->v4,
                              &grpEntry->ssmEntry[port],
                              &grpInfo->ssmInfo,
                              &newSsmEntry);
				memcpy(newSsmEntry.srcTimer,grpEntry->ssmEntry[port].srcTimer,
					sizeof(newSsmEntry.srcTimer));
                if (memcmp(&newSsmEntry, &grpEntry->ssmEntry[port],
                           sizeof(McastGrpSsmEntry)) != 0)
                    {
                    // Needs to update the group rules!
                    retVal=  GroupUpdate(port, grpEntry,
                                         grpInfo, &newSsmEntry);
                    }
                }
            else
                {
                ;
                }
#endif
            }
        else
            {
            grpEntry = GroupAlloc();
            if ((grpEntry != NULL) && McastDbRtPortGrpLimitCheck(port))
                {
                GroupInit(port, grpEntry, grpInfo);
                if (GroupAdd(port, grpEntry, grpInfo))
                    {
                    McastDbRtOnuGrpCountInc();
                    retVal = TRUE;
                    }
                }
            }
        }

    if (!retVal)
        {
        McastDebug(McastDebugLevel, ("CMCGA group add failure!\n"));
        }

    return retVal;
    } // McastMgrGrpAdd


////////////////////////////////////////////////////////////////////////////////
/// \brief Delete a multicast group from the port
///
/// \param port    Port to delete
/// \param grpInfo Group information
///
/// \return
/// TRUE if success, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL McastMgmtGrpDel(TkOnuEthPort port, const McastGrpInfo BULK * grpInfo)
    {
    McastGrpEntry BULK * grpEntry;

    if ((!ValidGroupInfo(grpInfo)) ||
        (!CheckMcastVlanMode(grpInfo)) ||
        (!CheckPortQual(port, grpInfo->qual)))
        {
        return FALSE;
        }

    grpEntry = GroupFind(grpInfo);
    if (grpEntry != NULL)
        {
        if (((TestBitsSet(grpInfo->qual, McastGrpByVid)) &&
             (grpInfo->grpVid == grpEntry->grpVid)) ||
            (grpInfo->qual == McastGrpByL2Da))
            {
            GroupDel(port, grpEntry);
            }
#if IgmpV3MldV2Supported
        else if (TestBitsSet(grpInfo->qual, McastGrpByIpSa))
            {
            McastGrpSsmEntry BULK ssmEntry;
            SsmInfoToSsmEntry(grpEntry->v4,
                              &grpInfo->ssmInfo,
                              &ssmEntry);
            if ((ssmEntry.srcMap^grpEntry->ssmEntry[port].srcMap) == 0)
                {
                GroupDel(port, grpEntry);
                }
            else if (SrcIdMapSub(ssmEntry.srcMap,
                                 grpEntry->ssmEntry[port].srcMap) == 0)
                {
                ssmEntry.srcMap = SrcIdMapSub(grpEntry->ssmEntry[port].srcMap,
                                              ssmEntry.srcMap);
                (void)GroupUpdate(port, grpEntry, grpInfo, &ssmEntry);
                }
            else
                {
                ;
                }
            }
        else
            {
            ;
            }
#endif
        }
    return TRUE;
    } // McastMgmtGrpDel


////////////////////////////////////////////////////////////////////////////////
/// \brief Clear all the multicast group from the port
///
/// \param port    Port to clear
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void McastMgmtGrpClr(TkOnuEthPort port)
    {
    McastGrpEntry BULK * BULK startEntry;
    startEntry = &mcastGrpTable[0];
    while (TRUE)
        {
        startEntry = GroupGet(startEntry);
        if (startEntry == NULL)
            {
            break;
            }

        if (TestBitsAny(startEntry->ssmDomainMap, SsmDomainMapPort(port)))
            {
            GroupDel(port, startEntry);
            }
        startEntry++;
        }
    } // McastMgmtGrpClr


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the grpIdx multicast group from the port
///
/// \param port        Port to clear
/// \param startEntry  Group entry to begin
///
/// \return
/// Group Entry Pointer
////////////////////////////////////////////////////////////////////////////////
//extern
McastGrpEntry * McastMgmtGrpEntryGet(TkOnuEthPort port,
                                     McastGrpEntry * startEntry)
    {
    McastGrpEntry BULK * BULK curEntry;
    if (startEntry == NULL)
        {
        curEntry = &mcastGrpTable[0];
        }
    else
        {
        curEntry = startEntry;
        }

    while (TRUE)
        {
        curEntry = GroupGet(curEntry);
        if (curEntry == NULL)
            {
            break;
            }

        if (TestBitsAny(curEntry->ssmDomainMap, SsmDomainMapPort(port)))
            {
            return curEntry;
            }
        curEntry++;
        }
    return NULL;
    } // McastMgmtGrpEntryGet


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the grpIdx multicast group from the port
///
/// \param port        Port to clear
/// \param startEntry  Group entry to begin
/// \param grpInfo     Group information buffer
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
McastGrpEntry * McastMgmtGrpGet(TkOnuEthPort port,
                                McastGrpEntry * startEntry,
                                McastGrpInfo BULK * grpInfo)
    {
    Stream  FAST strm;
    SsmSrcId FAST srcId;
    McastGrpEntry BULK * BULK grpEntry;
    grpEntry = McastMgmtGrpEntryGet(port, startEntry);
    if (grpEntry != NULL)
        {
        grpInfo->qual = McastDbRtPortQualGet(port);
        grpInfo->v4   = grpEntry->v4;
        grpInfo->grpVid = grpEntry->grpVid;
        memcpy(&grpInfo->grpAddr,  &grpEntry->grpAddr,
               sizeof(grpInfo->grpAddr));
        grpInfo->ssmInfo.type = grpEntry->ssmEntry[port].type;
        grpInfo->ssmInfo.srcNum = 0;
        StreamInit(&strm, &grpInfo->ssmInfo.byte[0]);
        for (srcId = SsmSrcIdBegin; srcId < SsmSrcIdEnd; srcId++)
            {
            if (TestBitsSet(grpEntry->ssmEntry[port].srcMap,
                            (U32)McastBitMap(srcId)))
                {
                grpInfo->ssmInfo.srcNum++;
                if (grpInfo->v4)
                    {
                    StreamWriteBytes(&strm,
                                     (U8 *)&ssmSrcRecord.entryList[srcId].srcIp.ipv4,
                                     sizeof(IpAddr));
                    }
                else
                    {
                    StreamWriteBytes(&strm,
                                     (U8 *)&ssmSrcRecord.entryList[srcId].srcIp.ipv6,
                                     sizeof(IpV6Addr));
                    }
                }
            }
        }

    return grpEntry;
    } // McastMgmtGrpGet


//##############################################################################
//                      Multicast Protocol Funtcions
//##############################################################################


////////////////////////////////////////////////////////////////////////////////
/// \brief Check the validation of the multicast frame
///
/// \param frm Pointer to the multicast frame
///
/// \return
/// TRUE when valid, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
static
BOOL ValidMcastFrame(const McastFrame BULK * frm)
    {
    BOOL FAST ret = FALSE;
    McastSnoopMode BULK snoopMode = McastDbRtSnoopModeGet();

    switch (frm->type)
        {
        case IgmpMembershipQuery:
            if (TestBitsAny(snoopMode, McastSnoopIgmp))
                {
                ret = TRUE;
                }
            break;
        case IgmpV1MembershipReport:
        case IgmpV2MembershipReport:
        case IgmpV2LeaveGroup:
            if (TestBitsSet(snoopMode, McastSnoopIgmpV1V2))
                {
                ret = TRUE;
                }
            break;

        case IgmpV3MembershipReport:
            if (TestBitsSet(snoopMode, McastSnoopIgmpV3))
                {
                ret = TRUE;
                }

            break;
        case MldMembershipQuery:
            if (TestBitsAny(snoopMode, McastSnoopMld))
                {
                ret = TRUE;
                }
            break;

        case MldV1Report:
        case MldV1Done:
            if (TestBitsSet(snoopMode, McastSnoopMldV1))
                {
                ret = TRUE;
                }
            break;

        case MldV2Report:
            if (TestBitsSet(snoopMode, McastSnoopMldV2))
                {
                ret = TRUE;
                }
            break;

        default:
            break;
        }
    return ret;
    } // ValidMcastFrame


////////////////////////////////////////////////////////////////////////////////
/// \brief Check the port rate credits and decrease one when there has credits
///        for tht port
///
/// \param port Port
///
/// \return
/// FALSE if there is no credits, TURE otherwise
////////////////////////////////////////////////////////////////////////////////
static
BOOL CheckAndDecRateCredits(TkOnuEthPort port)
    {
    if (McastDbRateLimitGet(port) != McastFrmRateUnlimit)
        {
        if (McastDbRtPortFrmCreditsGet(port) > 0)
            {
            McastDbRtPortFrmCreditsDec(port);
            }
        else
            {
            // no credit to send!
            return FALSE;
            }
        }
    return TRUE;
    } // CheckRateCredits


////////////////////////////////////////////////////////////////////////////////
/// \brief Convert the parsed frame into the group info
///
////////////////////////////////////////////////////////////////////////////////
static
void ParsedFrmToGrpInfo(McastGrpInfo * grpInfo,
                        const ParsedMcastFrame * parsedFrm)
    {
    grpInfo->qual |= (parsedFrm->ssmFrame) ?  McastGrpByIpSa : 0;
    grpInfo->fromHost = FALSE;
    grpInfo->v4 = parsedFrm->v4;
    memset(&grpInfo->grpAddr, 0, sizeof(McastGrpAddr));
    if (TestBitsSet(grpInfo->qual, McastGrpByL2Da))
        {
        McastMgmtMacAddrSet(grpInfo->v4,
                            &grpInfo->grpAddr, parsedFrm->grpAddr);
        }
    else
        {
        McastMgmtIpAddrSet(grpInfo->v4,
                           &grpInfo->grpAddr, parsedFrm->grpAddr);
        }

    switch (parsedFrm->frmCode)
        {
        case McastFrameCodeGeneralQuery:
        case McastFrameCodeSpecificQuery:
        case McastFrameCodeGroupReport:
            grpInfo->ssmInfo.type = ModeIsExclude;
            grpInfo->ssmInfo.srcNum = 0;
            break;

        case McastFrameCodeGroupLeave:
            grpInfo->ssmInfo.type = ModeIsInclude;
            grpInfo->ssmInfo.srcNum = 0;
            break;

        case McastFrameCodeSsmQuery:
        case McastFrameCodeSsmReport:
            grpInfo->ssmInfo.type = parsedFrm->ssmType;
            if (parsedFrm->frmCode == McastFrameCodeSsmQuery)
                {
                grpInfo->ssmInfo.type = ModeIsInclude;
                }
            grpInfo->ssmInfo.srcNum =
            (parsedFrm->numSrc < McastSsmSrcMaxNum) ?
            parsedFrm->numSrc : McastSsmSrcMaxNum;
            if (grpInfo->v4)
                {
                memcpy(&grpInfo->ssmInfo.byte[0],
                       parsedFrm->srcByte,
                       sizeof(IpAddr)*grpInfo->ssmInfo.srcNum);
                }
            else
                {
                memcpy(&grpInfo->ssmInfo.byte[0],
                       parsedFrm->srcByte,
                       sizeof(IpV6Addr)*grpInfo->ssmInfo.srcNum);
                }
            break;

        default:
            break;
        }

    } // ParsedFrmToGrpInfo


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the port mac address
///
/// \param port      Port to send
/// \param grpEntry  Group for querying
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void GetPortMac(Port port, MacAddr * mac)
    {
    if (port.pift == PortIfPon)
        {
        PonMgrGetMacForLink(port.inst, mac);
        }
    else if (port.pift == PortIfUni)
        {
        UniCfgDbGetPortMacAddress(port.inst, mac);
        }
    else
        {
        ;
        }
    } // GetPortMac


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the port Map
///
/// \param port      Port to send
/// \param grpEntry  Group for querying
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
U8 GetPortMap(const McastGrpEntry BULK * grpEntry)
    {
    U8 FAST portIdx;
    U8 FAST ret = 0;
    for (portIdx = 0; portIdx < McastPortMaxNum; portIdx++)
        {
        if (TestBitsAny(grpEntry->ssmDomainMap, SsmDomainMapPort(portIdx)))
            {
            McastBitsOrEq(ret, (U8)McastBitMap(portIdx));
            }
        }
    return ret;
    } // GetPortMap


////////////////////////////////////////////////////////////////////////////////
/// \brief Calculate the downstream forward port
///
/// \param parsedFrm  Parsed frame for receiving frame
///
/// \return
/// portMap
////////////////////////////////////////////////////////////////////////////////
static
U8 CalcMcastFrameDnPort(const ParsedMcastFrame * parsedFrm)
    {
    U8 FAST portMap = 0;
    McastGrpEntry BULK * BULK grpEntry = NULL;

    if (parsedFrm->frmCode == McastFrameCodeGeneralQuery)
        {
        portMap = 0x0F;
        }
    else
        {
        // search Mac group
        McastMgmtGroupInfoBufReset();
        mcastMgmtGroupInfo->qual = McastGrpByL2Da;
        ParsedFrmToGrpInfo(mcastMgmtGroupInfo, parsedFrm);
        grpEntry = GroupFind(mcastMgmtGroupInfo);
        if (grpEntry != NULL)
            {
            portMap |= GetPortMap(grpEntry);
            }

        McastMgmtGroupInfoBufReset();
        mcastMgmtGroupInfo->qual =McastGrpByIpDa;
        ParsedFrmToGrpInfo(mcastMgmtGroupInfo, parsedFrm);
        grpEntry = GroupFind(mcastMgmtGroupInfo);
        if (grpEntry != NULL)
            {
            portMap |= GetPortMap(grpEntry);
            }
        }
    return portMap;
    } // CalcMcastFrameDnPort


////////////////////////////////////////////////////////////////////////////////
/// \brief Modify the received frame based on the vlan action info
///
/// \param vid     Original vid of the received frame
/// \param actInfo Pointer to the action info
///
/// \return
/// TRUE when valid, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
static
U16 ModifyMcastFrame(VlanTag vid, const McastVlanActInfo * actInfo)
    {
    Stream BULK rxStrm;
    Stream BULK txStrm;
    StreamInit(&rxStrm, (U8 *)RxFrame);
    StreamInit(&txStrm, (U8 *)TxFrame);

    UNUSED(vid);

    // Copy DA, SA
    StreamCopy(&txStrm, rxStrm.cur, sizeof(MacAddr)*2);
    StreamSkip(&rxStrm, sizeof(MacAddr)*2);

    switch (actInfo->type)
        {
        case McastVlanActDel:
            if (StreamPeekU16(&rxStrm) == EthertypeVlan)
                {
                StreamSkip(&rxStrm, sizeof(EthernetVlanData));
                }
            break;

        case McastVlanActTrans:
        case McastVlanActAdd:
            {
                U16 FAST vlanTag = 0;
                StreamWriteU16(&txStrm, EthertypeVlan);
                if (StreamPeekU16(&rxStrm) == EthertypeVlan)
                    {
                    StreamSkip(&rxStrm, sizeof(Ethertype));
                    // Get the priority and CFI bits
                    vlanTag = StreamReadU16(&rxStrm) & 0xF000;
                    }
                vlanTag |= McastVlanGetVid(actInfo->outVid);
                StreamWriteU16(&txStrm, vlanTag);
            }
            break;

        case McastVlanActNone:
        case McastVlanActDrop:
        default:
            break;
        }

    StreamCopy(&txStrm, rxStrm.cur,
               (rxFrameLength - (U16)StreamLengthInBytes(&rxStrm)));

    return(U16)StreamLengthInBytes(&txStrm);
    } // ModifyMcastFrame


////////////////////////////////////////////////////////////////////////////////
/// \brief Send the multicast frame to the pon port
///
/// \param link  The link to send
/// \param len   The length of the frame including DA/SA Address
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void SendMcastFrameUp(LinkIndex link, U16 len)
    {
    UNUSED(link);
    len = len - (sizeof(MacAddr)*2);
    len = FixEthFrame(len);
    BufferUp((EthernetFrame BULK*)TxFrame, 0, len);
    } // SendMcastFrameUp


////////////////////////////////////////////////////////////////////////////////
/// \brief Send the multicast frame to the UNI port
///
/// \param port  The Port to send
/// \param len   The length of the frame including DA/SA Address
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void SendMcastFrameDn(TkOnuEthPort port, U16 len)
    {
    Port BULK uniPort = {PortIfUni, port};

    len = len - (sizeof(MacAddr)*2);
    len = FixEthFrame(len);
    BufferDn((EthernetFrame BULK*)TxFrame, uniPort, len);
    } // SendMcastFrameDn


////////////////////////////////////////////////////////////////////////////////
/// \brief Build and send the query
///
/// \param port      Port to send
/// \param grpEntry  Group for querying
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void BuildAndTxQuery(TkOnuEthPort port, const McastGrpEntry BULK * grpEntry)
    {
    IpAddr   BULK ipv4;
    IpV6Addr BULK ipv6;
    McastVlanActInfo BULK actInfo;
    Port BULK txPort = {PortIfUni, port};
    VlanTag BULK vid = McastVlanNonVid;

    memset(TxFrame, 0, McastProxyFrameLength);
    memset(&actInfo, 0, sizeof(McastVlanActInfo));

    if (mcastVlanActCbHdl.dn != NULL)
        {
        mcastVlanActCbHdl.dn(port, grpEntry->grpVid, FALSE, &actInfo);
        vid = actInfo.outVid;
        }

    if (grpEntry->v4)
        {
        McastMgmtIpv4AddrGet(&ipv4, grpEntry->grpAddr.byte);
        IgmpFrameQueryFill(proxyFrameSsm[port], (U8 *)TxFrame, vid, ipv4);
        }
    else
        {
        McastMgmtIpv6AddrGet(&ipv6, grpEntry->grpAddr.byte);
        MldFrameQueryFill(proxyFrameSsm[port], (U8 *)TxFrame, vid, &ipv6);
        }

    // update the src MAC
    GetPortMac(txPort, &TxFrame->sa);
    BufferDn(TxFrame, txPort, McastProxyFrameLength);
    } // BuildAndTxQuery


////////////////////////////////////////////////////////////////////////////////
/// \brief Build and send the leave
///
/// \param port      Port to send
/// \param grpEntry  Group for querying
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void BuildAndTxLeave(TkOnuEthPort port, const McastGrpEntry BULK * grpEntry)
    {
    IpAddr   BULK ipv4;
    IpV6Addr BULK ipv6;
    McastVlanActInfo BULK actInfo;
    VlanTag BULK vid = McastVlanNonVid;

    memset(TxFrame, 0, McastProxyFrameLength);
    memset(&actInfo, 0, sizeof(McastVlanActInfo));

    if (mcastVlanActCbHdl.up != NULL)
        {
        mcastVlanActCbHdl.up(port, McastVlanNonVid, FALSE, &actInfo);
        vid = actInfo.outVid;
        }

    if (grpEntry->v4)
        {
        McastMgmtIpv4AddrGet(&ipv4, grpEntry->grpAddr.byte);
        IgmpFrameLeaveFill(proxyFrameSsm[port], (U8 *)TxFrame, vid, ipv4);
        }
    else
        {
        McastMgmtIpv6AddrGet(&ipv6, grpEntry->grpAddr.byte);
        MldFrameLeaveFill(proxyFrameSsm[port], (U8 *)TxFrame, vid, &ipv6);
        }

    // Update the src MAC as orignial leave frame.
    memcpy(&TxFrame->sa, &proxyFrameMac, sizeof(MacAddr));
    BufferUp(TxFrame, 0, McastProxyFrameLength);
    McastDebug(McastDebugLevel, ("Tx leave frame!\n"));
    } // BuildAndTxLeave


////////////////////////////////////////////////////////////////////////////////
/// \brief Set the group entry into pending state
///
/// \param grpEntry  Group entry to set
/// \param timerVal Timer value to set
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void SetEntryPending(McastFrameCode code, McastGrpSsmInfo *ssmInfo,
						McastGrpEntry BULK * grpEntry, U16 timerVal)
    {
    U8 FAST i;
    for (i = 0; i < McastPortMaxNum; i++)
        {
        if (grpEntry->grpState[i] != McastGrpStateUnused)
            {
            grpEntry->grpState[i] = McastGrpStatePending;
            }

        if (timerVal <= (grpEntry->timerVal[i] - 1))
            {
            grpEntry->timerVal[i] = timerVal;
            }

#if IgmpV3MldV2Supported
		if (code == McastFrameCodeSsmQuery)
			{
			U8 srcNum;
			SaIpAddr srcIp;
			Stream BULK strm;
			SsmSrcId srcId;
			if ((ssmInfo != NULL) && (ssmInfo->srcNum != 0))
				{
				StreamInit(&strm, ssmInfo->byte);
				for (srcNum = 0; srcNum < ssmInfo->srcNum;srcNum++)
					{
        			memset(&srcIp, 0, sizeof(SaIpAddr));
        			GetSsmSrc(grpEntry->v4, &strm, &srcIp);
					srcId = SsmSrcFind(grpEntry->v4,&srcIp);
					if (srcId < SsmSrcIdEnd)
						{
						SsmSrcTimerUpdate(&grpEntry->ssmEntry[i],
							(U32)McastBitMap(srcId),timerVal);
						}
					}
				}
			}
		else
			{
			SsmSrcTimerUpdate(&grpEntry->ssmEntry[i],grpEntry->ssmEntry[i].srcMap,timerVal);
			}
#endif
        }
    } // SetEntryPending


////////////////////////////////////////////////////////////////////////////////
/// \brief Downstream multicast frame process
///
/// \param parsedFrm  Parsed frame for receiving frame
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void McastDnProcess(const ParsedMcastFrame * parsedFrm)
    {
    U16 FAST timerVal;
    McastGrpEntry BULK * BULK grpEntry;
    timerVal = parsedFrm->maxRespTime;
    timerVal *= McastDbRobustnessCountGet();

    switch (parsedFrm->frmCode)
        {
        case McastFrameCodeGeneralQuery:
            {
                grpEntry = &mcastGrpTable[0];
                while (TRUE)
                    {
                    grpEntry = GroupGet(grpEntry);
                    if (grpEntry == NULL)
                        {
                        break;
                        }
                    SetEntryPending(parsedFrm->frmCode,	NULL,grpEntry, timerVal);
                    grpEntry++;
                    }
            }
            break;

        case McastFrameCodeSpecificQuery:
        case McastFrameCodeSsmQuery:
            {
                // Search Da Ip group
                McastMgmtGroupInfoBufReset();
                mcastMgmtGroupInfo->qual = McastGrpByIpDa;
                ParsedFrmToGrpInfo(mcastMgmtGroupInfo, parsedFrm);
                grpEntry = GroupFind(mcastMgmtGroupInfo);
                if (grpEntry != NULL)
                    {
                    SetEntryPending(parsedFrm->frmCode,&mcastMgmtGroupInfo->ssmInfo,
									grpEntry, timerVal);
                    }

                // search Mac group
                McastMgmtGroupInfoBufReset();
                mcastMgmtGroupInfo->qual = McastGrpByL2Da;
                ParsedFrmToGrpInfo(mcastMgmtGroupInfo, parsedFrm);
                grpEntry = GroupFind(mcastMgmtGroupInfo);
                if (grpEntry != NULL)
                    {
                    SetEntryPending(parsedFrm->frmCode,&mcastMgmtGroupInfo->ssmInfo,
									grpEntry, timerVal);
                    }
            }
            break;

        default:
            break;
        }
    } // McastDnProcess


////////////////////////////////////////////////////////////////////////////////
/// \brief Handle the downstream multicast frame
///
/// \param link Link from which the frame is recived
/// \param vid  Vid
/// \param frm  Pointer to the multicast frame
/// \param len  The length of the multicast frame
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void McastDnHandle(LinkIndex link, U16 vid,
                   const McastFrame BULK * frm, U16 len)
    {
    U8 FAST i;
    U8 FAST portMap;
    BOOL FAST process;
    ParsedMcastFrame BULK parsedFrm;
    McastVlanActInfo BULK actInfo;

    memset(&actInfo, 0, sizeof(McastVlanActInfo));
    memset(&parsedFrm, 0, sizeof(ParsedMcastFrame));

	U8 macv6[16];
    portMap = 0;
    process = FALSE;

    if (ValidMcastFrame(frm) &&
        McastFrameParser(frm, len, 0, &parsedFrm) &&
        ((parsedFrm.frmCode == McastFrameCodeGeneralQuery) ||
         (parsedFrm.frmCode == McastFrameCodeSpecificQuery) ||
         (parsedFrm.frmCode == McastFrameCodeSsmQuery)))
        {
        portMap = CalcMcastFrameDnPort(&parsedFrm);
        }
	else
		{
		//other ICMPV6 packets forwarding to special port by checking the Mcast MAC.
		if (McastGrpMacV6AddrCheck((U8 *)(&RxFrame->da)))
			{
			memset(macv6,0,sizeof(macv6));
			memcpy(&macv6[McastGrpAddrMacOffset + 2], 
			((U8 *)(&RxFrame->da) +2U), 4);
			parsedFrm.frmCode = McastFrameCodeNum;
			parsedFrm.v4 = FALSE;
			parsedFrm.grpAddr = macv6;
			portMap = CalcMcastFrameDnPort(&parsedFrm);
			}
		}

    // Forward frame to ports first
    for (i = 0; i < McastPortMaxNum; i++)
        {
        if (TestBitsSet(portMap, (U8)(1U << i)))
            {
            if (mcastVlanActCbHdl.dn != NULL)
                {
                mcastVlanActCbHdl.dn(i, vid, TRUE, &actInfo);
                }
            if (actInfo.type != McastVlanActDrop)
                {
                process = TRUE;
                SendMcastFrameDn(i, ModifyMcastFrame(vid, &actInfo));
                }
            }
        }

    if (McastDbRtHostCtrlGet())
        {
        return;
        }

    // process frames
    UNUSED(link);
    if (process)
        {
        McastDnProcess(&parsedFrm);
        }
    } // McastDnHandle


////////////////////////////////////////////////////////////////////////////////
/// \brief Check if it is leave frame and if ONU should drop it, or not.
///
/// \param parsedFrm Parsed frame
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
BOOL CheckLeaveFrameDrop(const ParsedMcastFrame * parsedFrm)
    {
    if ((((parsedFrm->numSrc == 0) &&
          (parsedFrm->ssmType == ModeIsInclude)) ||
         (parsedFrm->frmCode == McastFrameCodeGroupLeave)) &&
        McastDbProxyLeaveGet() && (!McastDbRtFastLeaveGet()))
        {
        return TRUE;
        }
    else
        {
        return FALSE;
        }
    } // CheckLeaveFrameDrop


////////////////////////////////////////////////////////////////////////////////
/// \brief Leave message process
///
/// \param port Port
/// \param grpIp Pointer to the ip address
/// \param grpInfo  Pointer to the group info
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void LeaveMsgProcess(TkOnuEthPort port, const U8 * grpIp,
                     const McastGrpInfo * grpInfo)
    {
    McastGrpEntry BULK * BULK grpEntry;
    grpEntry = GroupFind(grpInfo);
    if ((grpEntry == NULL) ||
        (!TestBitsAny(grpEntry->ssmDomainMap, SsmDomainMapPort(port))))
        {
        return;
        }

    if (McastDbRtFastLeaveGet())
        {
        GroupDel(port, grpEntry);
        }
    else
        {
        switch ((grpEntry->grpState[port]))
            {
            case McastGrpStateReported:
            case McastGrpStateActive:
                grpEntry->grpState[port] = McastGrpStatePending;
                if (McastDbProxyLeaveGet())
                    {
                    McastDebug(McastDebugLevel, ("Enter proxy leave!\n"));
                    grpEntry->timerVal[port] = DefaultLmqInterval;
                    grpEntry->proxyLmqCount[port] = (McastDbLmqCountGet()-1);
                    grpEntry->proxyState[port] = TRUE;
                    if (grpEntry->v4)
                        {
                        memcpy(&grpEntry->grpAddr.byte[McastGrpAddrIpv4Offset],
                               grpIp, sizeof(IpAddr));
                        }
                    else
                        {
                        memcpy(&grpEntry->grpAddr.byte[McastGrpAddrIpv6Offset],
                               grpIp, sizeof(IpV6Addr));
                        }

                    memcpy(&proxyFrameMac, &RxFrame->sa, sizeof(MacAddr));
                    proxyFrameSsm[port] = FALSE;
                    if (TestBitsSet(grpInfo->qual, McastGrpByIpSa))
                        {
                        proxyFrameSsm[port] = TRUE;
                        }
                    BuildAndTxQuery(port, grpEntry);
                    }
                else
                    {
                    grpEntry->timerVal[port] = McastlargeTimerValue;
                    }
                break;
            case McastGrpStatePending:
            default:
                break;
            }
        }
    } // LeaveMsgProcess


////////////////////////////////////////////////////////////////////////////////
/// \brief Report message process
///
/// \param port Port
/// \param grpInfo  Pointer to the group info
///
/// \return
/// TRUE if success, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
static
BOOL ReportMsgProcess(TkOnuEthPort port, const McastGrpInfo BULK * grpInfo)
    {
    BOOL FAST ret;
    McastGrpEntry BULK * BULK grpEntry;

    ret = TRUE;
    grpEntry = GroupFind(grpInfo);
    if (McastDbRtHostCtrlGet())
        {
        if ((grpEntry != NULL) &&
            (grpEntry->grpState[port] == McastGrpStatePending))
            {
            grpEntry->grpState[port] = McastGrpStateReported;
#if IgmpV3MldV2Supported
			if (grpEntry->ssmEntry[port].type == grpInfo->ssmInfo.type) 
				{
				U32 newSrcMap ;
				McastGrpSsmEntry BULK  newSsmEntry;
				SsmInfoToSsmEntry(grpEntry->v4,&grpInfo->ssmInfo, &newSsmEntry);
				newSrcMap = McastBitsAndEq(grpEntry->ssmEntry[port].srcMap, 			
							newSsmEntry.srcMap);
				SsmSrcTimerUpdate(&grpEntry->ssmEntry[port],
					newSrcMap,McastlargeTimerValue);
				}
#endif
            }

        }
    else
        {
        if (grpEntry != NULL)
            {
            if (grpEntry->grpState[port] == McastGrpStateUnused)
                {
                if (McastDbRtPortGrpLimitCheck(port))
                    {
                    return GroupAdd(port, grpEntry, grpInfo);
                    }
                else
                    {
                    ret = FALSE;
                    }
                }
            else
                {
#if IgmpV3MldV2Supported
                McastGrpSsmEntry BULK  newSsmEntry;
                SsmEntryBuild(grpEntry->v4,
                              &grpEntry->ssmEntry[port],
                              &grpInfo->ssmInfo,
                              &newSsmEntry);
				//ingore the timer infomation.
				memcpy(newSsmEntry.srcTimer,grpEntry->ssmEntry[port].srcTimer,
					sizeof(newSsmEntry.srcTimer));
                if (memcmp(&newSsmEntry, &grpEntry->ssmEntry[port],
                           sizeof(McastGrpSsmEntry)) != 0)
                    {
                    // Needs to update the group rules!
                    (void)GroupUpdate(port, grpEntry,
                                      grpInfo, &newSsmEntry);
                    }
#endif
                if (grpEntry->grpState[port] == McastGrpStatePending)
                    {
                    if (grpEntry->proxyState[port])
                        {
                        proxyExit = TRUE;
                        grpEntry->proxyLmqCount[port] = 0;
                        grpEntry->proxyState[port] = FALSE;
                        }
                    grpEntry->grpState[port] = McastGrpStateReported;
                    }
                else if (grpEntry->grpState[port] == McastGrpStateActive)
                    {
                    grpEntry->timerVal[port] = McastlargeTimerValue;
                    }
                else
                    {
                    ; // Do nothing!
                    }
                }
            }
        else
            {
            grpEntry = GroupAlloc();
            if ((grpEntry != NULL) && McastDbRtPortGrpLimitCheck(port))
                {
                GroupInit(port, grpEntry, grpInfo);
                if (GroupAdd(port, grpEntry, grpInfo))
                    {
                    McastDbRtOnuGrpCountInc();
                    return TRUE;
                    }
                }
            ret = FALSE;
            }
        }
    return ret;
    } // ReportMsgProcess


////////////////////////////////////////////////////////////////////////////////
/// \brief Process the upstream frame
///
/// \param port Port
/// \param parsedFrm Pointer to the Parsed Frame
///
/// \return
/// FALSE if there is no credits, TURE otherwise
////////////////////////////////////////////////////////////////////////////////
static
BOOL McastUpProcess(TkOnuEthPort port, const ParsedMcastFrame * parsedFrm)
    {
    BOOL FAST ret = TRUE;
    if ((parsedFrm->frmCode == McastFrameCodeGroupReport) ||
        (parsedFrm->frmCode == McastFrameCodeGroupLeave) ||
        (parsedFrm->frmCode == McastFrameCodeSsmReport))
        {
        McastMgmtGroupInfoBufReset();
        mcastMgmtGroupInfo->qual = McastDbRtPortQualGet(port);

        if (mcastMgmtGroupInfo->qual == McastGrpNone)
            {
            // For Host Control mode, there is no group added before.
            // Do nothing here!
            return TRUE;
            }
        McastBitsAndEq(mcastMgmtGroupInfo->qual,
                       (McastGrpByL2Da | McastGrpByIpDa));

        ParsedFrmToGrpInfo(mcastMgmtGroupInfo, parsedFrm);
        if (!ValidGroupInfo(mcastMgmtGroupInfo))
            {
            return FALSE;
            }

        if (((mcastMgmtGroupInfo->ssmInfo.type == ModeIsInclude) ||
             (mcastMgmtGroupInfo->ssmInfo.type == ChangeToIncludeMode)) &&
            (mcastMgmtGroupInfo->ssmInfo.srcNum == 0))
            {
            LeaveMsgProcess(port, parsedFrm->grpAddr ,mcastMgmtGroupInfo);
            }
        else
            {
            ret = ReportMsgProcess(port, mcastMgmtGroupInfo);
            }
        }
    else
        {
        ret = FALSE;
        }

    return ret;
    } // McastUpProcess


////////////////////////////////////////////////////////////////////////////////
/// \brief Handle the upstream multicast frame
///
/// \param port Port from which the frame is recived
/// \param vid  Vid
/// \param frm  Pointer to the multicast frame
/// \param len  The length of the multicast frame
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void McastUpHandle(TkOnuEthPort port, U16 vid,
                   const McastFrame BULK * frm, U16 len)
    {
    BOOL BULK success;
    BOOL BULK knownFrame;
    McastVlanActInfo BULK actInfo;
    ParsedMcastFrame BULK parsedFrm;

    memset(&parsedFrm, 0, sizeof(ParsedMcastFrame));
    memset(&actInfo, 0, sizeof(McastVlanActInfo));

    if (mcastVlanActCbHdl.up != NULL)
        {
        mcastVlanActCbHdl.up(port, vid, TRUE, &actInfo);
        if (actInfo.type == McastVlanActDrop)
            {
            McastDebug(McastDebugLevel,
                       ("Invalid vid(%d) frame for port(%d), drop it!\n", vid, port));
            return;
            }
        }

    success = TRUE;
    knownFrame = TRUE;
    if (ValidMcastFrame(frm))
        {
        U8  FAST i;
        U16 FAST recNum;
        if (frm->type == IgmpV3MembershipReport)
            {
            recNum = frm->igmp.v3.rpt.numRecs;
            }
        else if (frm->type == MldV2Report)
            {
            recNum = frm->mld.v2.rpt.numRecs;
            }
        else
            {
            recNum = 1;
            }

        for (i = 0; i < recNum; i++)
            {
            if (McastFrameParser(frm, len, i, &parsedFrm))
                {
                proxyExit = FALSE;
                if (!McastUpProcess(port, &parsedFrm))
                    {
                    success = FALSE;
                    break;
                    }

                if (proxyExit || CheckLeaveFrameDrop(&parsedFrm))
                    {
                    return;
                    }
                }
            else
                {
                knownFrame = FALSE;
                break;
                }
            }
        }
    else
        {
        knownFrame = FALSE;
        }

    if (knownFrame)
        {
        if (success)
            {
            if (!CheckAndDecRateCredits(port))
                {
                // no credit to send!
                McastDebug(McastDebugLevel,
                           ("No Credit for port(%d)!\n", port));
                return;
                }
            }
        else
            {
            McastDebug(McastDebugLevel, ("Process fails!\n"));
            }
        }

    SendMcastFrameUp(0, ModifyMcastFrame(vid, &actInfo));
    } // McastUpHandle


////////////////////////////////////////////////////////////////////////////////
/// \brief Checks to see if query response was received
///
/// \param port   Port for which Dn Query Response needs to be checked
/// \param entry  Entry to make pending
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void CheckDnQueryResponse(U8 port, McastGrpEntry BULK * entry)
    {
    switch (entry->grpState[port])
        {
        case McastGrpStatePending:
            // no response received, remove this group from this port
            GroupDel(port, entry);
            break;

        case McastGrpStateReported:
            // If a report is received, move it to active state;
            entry->grpState[port] = McastGrpStateActive;
            break;

        case McastGrpStateActive:
            if (!McastDbRtHostCtrlGet())
                {
                GroupDel(port, entry);
                }
            break;

        default:
            // do nothing
            break;
        }
    } // CheckDnQueryResponse


////////////////////////////////////////////////////////////////////////////////
/// \brief Proxy leave Handle
///
/// \param port  Port to handle
/// \param entry Entry to handle
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void ProxyLeaveHandle(U8 port, McastGrpEntry BULK * entry)
    {
    McastDebug(McastDebugLevel,
               ("PLH Group state %d!\n", entry->grpState[port]));
    switch (entry->grpState[port])
        {
        case McastGrpStatePending:
            if (entry->proxyLmqCount[port] == 0)
                {
                if (entry->proxyState[port])
                    {
                    BuildAndTxLeave(port, entry);
                    entry->proxyState[port] = FALSE;
                    }
                GroupDel(port, entry);
                }
            else
                {
                --entry->proxyLmqCount[port];
                entry->timerVal[port] = DefaultLmqInterval;
                // Send query message to the port
                BuildAndTxQuery(port, entry);
                }
            break;

        case McastGrpStateReported:
            entry->grpState[port] = McastGrpStateActive;
            entry->proxyLmqCount[port] = 0;
            entry->timerVal[port] = 0;
            entry->proxyState[port] = FALSE;
            break;

        case McastGrpStateActive:
            if (!McastDbRtHostCtrlGet())
                {
                GroupDel(port, entry);
                }
            break;

        default:
            // do nothing
            break;
        }
    } // ProxyLeaveHandle


////////////////////////////////////////////////////////////////////////////////
/// \brief Update the rate limit variable
///
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void RateLimitUpdate(void)
    {
    U8  FAST i;
    U16 FAST rateLimit;
    rateLimit = McastDbRtRateLimitCountGet();
    if (rateLimit > 0)
        {
        --rateLimit;
        McastDbRtRateLimitCountSet(rateLimit);
        }
    else
        {
        for (i = 0; i < McastPortMaxNum; i++)
            {
            McastDbRtPortFrmCreditsSet(i, McastDbRateLimitGet(i));
            }
        McastDbRtRateLimitCountSet(9);
        }
    } // RateLimitUpdate


////////////////////////////////////////////////////////////////////////////////
/// \brief Handle Timer expiry events
///
/// \param timerId Timer that has expired
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void McastHandleTimer (OntmTimerId timerId)
    {
    U8  FAST port;
    U16 FAST timerVal;
    U16 FAST count = McastDbRtOnuGrpCountGet();
    McastGrpEntry BULK * FAST entry = &mcastGrpTable[0];
    UNUSED(timerId);

    while (count != 0)
        {
        if (GroupInUse(entry))
            {
            count--;
            for (port = 0; port < McastPortMaxNum; port++)
                {
                timerVal = entry->timerVal[port];
                if (timerVal != 0)
                    {
                    // timer is still active
                    --timerVal;
                    entry->timerVal[port] = timerVal;
                    if (timerVal == 0)
                        {
                        McastDebug(McastDebugLevel, ("Group timeout!\n"));
                        if (!McastDbProxyLeaveGet())
                            {
                            CheckDnQueryResponse(port, entry);
                            }
                        else
                            {
                            ProxyLeaveHandle(port, entry);
                            }
                        }
					
#if IgmpV3MldV2Supported
					else //check source list timer
						{
						SsmSrcId FAST srcId;
						U16 domainId;
						MacAddr mac;
						U16 srctime;

		                for (srcId = SsmSrcIdBegin; srcId < SsmSrcIdEnd; srcId++)
		                    {
		                    if (TestBitsSet(entry->ssmEntry[port].srcMap,
		                                    (U32)McastBitMap(srcId)))
		                        {
		                        srctime = entry->ssmEntry[port].srcTimer[srcId-SsmSrcIdBegin];
								if (srctime != 0)
									{
									srctime--;
									entry->ssmEntry[port].srcTimer[srcId-SsmSrcIdBegin] = srctime;
									if (srctime == 0)
										{										
										domainId = SrcIdToSsmDomainId(srcId);
										McastMgmtMacAddrGet(entry->v4, &mac, 
											entry->grpAddr.byte);
										(void)McastGrpBinDelMac(port, &mac, domainId);										

										McastBitsAndEqNot(entry->ssmEntry[port].srcMap, 
											(U32)McastBitMap(srcId));
                						McastBitsOrEq(entry->ssmEntry[port].srcMap,
                                  		(U32)McastBitMap(srcId + SsmSrcIdEnd));
										
        								SsmSrcIdMapDec((U32)McastBitMap(srcId));
        								SsmSrcIdUpdate();	                    				
										}
									}
								
		                        }
		                    }
						}
#endif
                    } // active
                } // all ports
            } // group active
        entry++;
        } // end while

    RateLimitUpdate();
    } // McastHandleTimer


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the ssm source from ssm map
///
/// \param ssmMap Ssm map
///
/// \return
/// Ssm source number
////////////////////////////////////////////////////////////////////////////////
//extern
U8 McastSsmSrcNum(U32 ssmMap)
    {
    U8 srcId;
    U8 srcNum = 0;
    for (srcId = SsmSrcIdBegin; srcId < SsmSrcIdEnd; srcId++)
        {
        if ((ssmMap & ((U32)McastBitMap(srcId))) != 0)
            {
            srcNum++;
            }
        }
    return srcNum;
    } // McastSsmSrcNum


////////////////////////////////////////////////////////////////////////////////
/// \brief Get the ssm source value
///
/// \param srcId Ssm source id.
///
/// \return
/// The pointer to the source.
////////////////////////////////////////////////////////////////////////////////
//extern
SsmSrcEntry * McastSsmSrcGet(SsmSrcId srcId)
    {
    if (srcId == SsmSrcIdNone)
        {
        return NULL;
        }

    return &ssmSrcRecord.entryList[srcId];
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief Check the ipv4 group mac address
///
/// \Param mac Pointer to the mac address
///
/// \return
/// TRUE if valid, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL McastGrpMacV4AddrCheck(const U8 BULK * mac)
    {
    U64 FAST data = 0;
    memcpy(&(((U8 *)(&data))[2]), mac, sizeof(MacAddr));
    if ((data >= (U64)McastDataIpv4DaMacLow) &&
        (data <= (U64)McastDataIpv4DaMacHigh))
        {
        return TRUE;
        }
    else
        {
        return FALSE;
        }
    } // McastGrpMacV4AddrCheck


////////////////////////////////////////////////////////////////////////////////
/// \brief Check the ipv6 group mac address
///
/// \Param mac Pointer to the mac address
///
/// \return
/// TRUE if valid, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL McastGrpMacV6AddrCheck(const U8 BULK * mac)
    {
    if ((mac[0] == 0x33) && (mac[1] == 0x33))
        {
        return TRUE;
        }
    else
        {
        return FALSE;
        }
    } // McastGrpMacV6AddrCheck


////////////////////////////////////////////////////////////////////////////////
/// \brief Check the group mac address
///
/// \Param mac Pointer to the mac address
///
/// \return
/// TRUE if valid, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL McastGrpMacAddrCheck(const U8 BULK * mac)
    {
    if (mac[0] == 0x33)
        {
        return McastGrpMacV6AddrCheck(mac);
        }
    else
        {
        return McastGrpMacV4AddrCheck(mac);
        }
    } // McastGrpMacAddrCheck


////////////////////////////////////////////////////////////////////////////////
/// \brief Check the ipv4 group da ip address
///
/// \Param ipv4 Pointer to the ipv4 da ip address
///
/// \return
/// TRUE if valid, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL McastGrpDaIpV4AddrCheck(const U8 BULK * ipv4)
    {
    IpAddr FAST ipAddr;
    memcpy(&ipAddr, ipv4, sizeof(ipAddr));
    if ((ipAddr >= (IpAddr)McastDataDaIpV4Low) &&
        (ipAddr <= (IpAddr)McastDataDaIpV4High))
        {
        return TRUE;
        }
    else
        {
        return FALSE;
        }
    } // McastGrpDaIpV4AddrCheck


////////////////////////////////////////////////////////////////////////////////
/// \brief Check the ipv6 group da ip address
///
/// \Param ipv6 Pointer to the ipv4 da ip address
///
/// \return
/// TRUE if valid, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL McastGrpDaIpV6AddrCheck(const IpV6Addr BULK * ipv6)
    {
    return(ipv6->byte[0] == 0xFF) ? TRUE : FALSE;
    } // McastGrpDaIpV6AddrCheck


////////////////////////////////////////////////////////////////////////////////
/// \brief Handle Port LoS
///
/// \param port Port that encountered LoS
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void McastHandlePortLoS (Port p)
    {
    if (p.inst >= UniCfgDbGetActivePortCount())
        {
        return ;
        }
    if (!McastDbRtHostCtrlGet())
        {
        McastMgmtGrpClr(p.inst);
        }
    } // McastHandlePortLoS

// end of McastMgmt.c


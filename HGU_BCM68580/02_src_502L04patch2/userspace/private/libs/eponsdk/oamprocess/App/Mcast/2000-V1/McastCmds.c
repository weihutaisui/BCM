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
/// \file McastCmds.h
/// \brief IP Multicast VLAN CLI Commands module
///
/// The IP multicast VLAN CLI Commandsmodule
///
////////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Teknovus.h"
#include "Cli.h"
#include "McastDb.h"
#include "McastMgmt.h"
#include "McastVlan.h"
#include "Json.h"


////////////////////////////////////////////////////////////////////////////////
/// \brief  Shows the port related mcast vlan
///
/// \param  argc    Argument count
/// \param  argv    Argument values
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void CmdShowPortMcastVlan (U8 argc, const char BULK* const argv[])
    {
    TkOnuEthPort FAST port;
    U8 FAST mcastVlanIdx;
    McastVlanPortCfg BULK * BULK portCfg;

    if (argc > 1)
        {
        port = (TkOnuEthPort)strtoul(argv[1], NULL, 0);
        portCfg = McastVlanCfgGet(port);
        JsonInit();
        JsonArrayStart();
        JsonWriteNum(portCfg->op);
        JsonArrayStart();
        for (mcastVlanIdx = 0; mcastVlanIdx < portCfg->vlanNum; mcastVlanIdx++)
            {
            JsonArrayStart();
            JsonWriteNum(portCfg->entry[mcastVlanIdx].eponVid);
            JsonWriteNum(portCfg->entry[mcastVlanIdx].iptvVid);
            JsonArrayEnd();
            }
        JsonArrayEnd();
        JsonArrayEnd();
        printf("\n");
        }
    } // CmdShowPortMcastVlan


////////////////////////////////////////////////////////////////////////////////
/// \brief  Shows the port related mcast groups
///
/// \param  argc    Argument count
/// \param  argv    Argument values
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void CmdShowPortMcastGroup (U8 argc, const char BULK* const argv[])
    {
    TkOnuEthPort FAST port;
    McastGrpQual BULK portQual;
    McastGrpEntry BULK * BULK grpEntry;

    if (argc > 1)
        {
        port = (TkOnuEthPort)strtoul(argv[1], NULL, 0);
        portQual = McastDbRtPortQualGet(port);

        JsonInit();
        JsonArrayStart();
        for (grpEntry = McastMgmtGrpEntryGet(port, NULL);
             grpEntry != NULL;
             grpEntry = McastMgmtGrpEntryGet(port, grpEntry + 1))
            {
            JsonArrayStart();
            JsonWriteHexString(grpEntry->grpAddr.byte, McastGrpAddrLength);
            JsonWriteNum((U32)grpEntry->v4);
            JsonWriteNum(portQual);
            JsonWriteNum(grpEntry->grpVid);
            JsonWriteNum(grpEntry->grpState[port]);
            JsonWriteNum(grpEntry->timerVal[port]);
            JsonWriteNum(grpEntry->proxyLmqCount[port]);
            JsonWriteNum((U32)grpEntry->proxyState[port]);
            JsonWriteNum(grpEntry->ssmEntry[port].type);

            JsonArrayStart();
            if (grpEntry->ssmEntry[port].type != InvalidMode)
                {
                SsmSrcId FAST srcId;

                for (srcId = SsmSrcIdBegin; srcId < SsmSrcIdEnd; srcId++)
                    {
                    if (TestBitsSet(grpEntry->ssmEntry[port].srcMap,
                                    (U32)McastBitMap(srcId)))
                        {
                        SsmSrcEntry * entry = McastSsmSrcGet(srcId);
                        U8 * addr = (U8*)&entry->srcIp.ipv4;
                        U8 size = sizeof(IpAddr);

                        if (!entry->v4)
                            {
                            addr = (U8*)&entry->srcIp.ipv6;
                            size = sizeof(IpV6Addr);
                            }
                        JsonWriteHexString(addr, size);
                        }
                    }
                }
            JsonArrayEnd();
            JsonArrayEnd();
            printf("\n");
            }
        JsonArrayEnd();
        printf("\n");
        }
    } // CmdShowPortMcastGroup


const CliDirEntry CODE cliMcastDir[] =
{
    CliEntry("vlan",    CmdShowPortMcastVlan,   DirEntryCmd,    CliPrivCust),
    CliEntry("group",   CmdShowPortMcastGroup,  DirEntryCmd,    CliPrivCust),
    CliEntry(NULL,      NULL,            DirEntryTerminator,    CliPrivMax)
};

// End of the McastCmds.c


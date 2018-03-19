/*
*  Copyright 2011, Broadcom Corporation
*
* <:copyright-BRCM:2011:proprietary:epon
* 
*    Copyright (c) 2011 Broadcom 
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
/// \file EponCmds.c
/// \brief Tk4701 Epon block commands
///
/// Provides basic access functions for the TK4701 Epon block.
///
////////////////////////////////////////////////////////////////////////////////
#include <linux/string.h>
#include "EponCmds.h"
#include "OntDirector.h"
#include "EpnRegs.h"

char const * const  StatStr[StatStrNumStrings] =
    {
    "64",
    "0-127",
    "65-127",
    "128-255",
    "256-383",
    "384-511",
    "512-639",
    "640-767",
    "768-895",
    "896-1023",
    "1024-1151",
    "1152-1279",
    "1280-1407",
    "1408-1535",
    "1536-1663",
    "1664-1791",
    "1792-1919",
    "1920-2047",
    "64-511",
    "256-511",
    "512-1023",
    "1024-1518",
    "1519+",
    "64b66bDecErr",
    "AlignErr",
    "BcastFrmRx",
    "BcastFrmTx",
    "ByteDlyd",
    "ByteDrop",
    "ByteRx",
    "ByteTx",
    "CodeErr",
    "Col 1",
    "Col 2",
    "Col 3",
    "Col 4",
    "Col 5",
    "Col 6",
    "Col 7",
    "Col 8",
    "Col 9",
    "Col 10",
    "Col 11",
    "Col 12-15",
    "ColExc",
    "ColLate",
    "ColTot",
    "Crc32Err",
    "Crc8Err",
    "ErrByte",
    "FcsErr",
    "FecBlock",
    "FecByte",
    "FecCorBlk",
    "FecCorByte",
    "FecCorOne",
    "FecCorZero",
    "FecDecErrCor",
    "FecDecFail",
    "FecDecPass",
    "FecExceedErr",
    "FecFrm",
    "FecUnCorBlk",
    "Frm",
    "FrmRx",
    "FrmTx",
    "FrmBadSh",
    "FrmDrop",
    "GateRx",
    "GenCrc32",
    "LineCodeErr",
    "MaxLenErr",
    "MaxPktDly",
    "McastFrmRx",
    "McastFrmTx",
    "MpcpByte",
    "MpcpFrm",
    "NonFecBytes",
    "NonFecFrm",
    "OamBytesRx",
    "OamBytesTx",
    "OamFrm",
    "OamFrmRx",
    "OamFrmTx",
    "Oversized",
    "PauseRx",
    "PauseTx",
    "RegFrmRx",
    "Reports",
    "SecGoodByte",
    "SecGoodFrm",
    "SecRxBadFrm",
    "TestPrbsErr",
    "TestPseudoErr",
    "UcastFrmRx",
    "UcastFrmTx",
    "Undersized",
    "UnusedWordsTx",
    "BufUnd",
    "RxAbrt"
    };

const StatStrId  EponLinkStatStrTab[] =
    {
//bidirectional stats
    StatStrByteRx,
    StatStrFcsErr,
    StatStrOamFrmRx,
    StatStrGateRx,
    StatStr64,
    StatStr65_127,
    StatStr128_255,
    StatStr256_511,
    StatStr512_1023,
    StatStr1024_1518,
    StatStr1519Plus,       
    StatStrOversized,
    StatStrBcastFrmRx,
    StatStrMcastFrmRx,
    StatStrUcastFrmRx,
    StatStrUndersized,
    StatStrOamBytesRx,
    StatStrRegFrmRx,

//downstream only stats
    StatStrByteRx,
    StatStrFcsErr,
    StatStrOamFrmRx,
    StatStrGateRx,
    StatStrBcastFrmRx,
    StatStrMcastFrmRx,
    StatStrUcastFrmRx,
    StatStr64_511,
    StatStr512_1023,
    StatStr1024_1518,
    StatStr1519Plus,
    StatStrOversized,
    StatStrUndersized,
    StatStrOamBytesRx,

//upstream only stats
    StatStrByteTx,
    StatStrUnusedWordsTx,
    StatStrOamFrmTx,
    StatStrReports,
    StatStr64,
    StatStr65_127,
    StatStr128_255,
    StatStr256_511,
    StatStr512_1023,
    StatStr1024_1518,
    StatStr1519Plus,  
    StatStrOamFrmTx,
    StatStrBcastFrmTx,
    StatStrMcastFrmTx,
    StatStrUcastFrmTx
    };

const StatStrId  EponStatStrTab[] =
    {
    StatStrOversized,
    StatStrFrm,
    StatStrFcsErr,
    StatStrGateRx,
    StatStrOamFrm,
    StatStrUndersized
    };

#define EpnMainInrpNum 32

char const * const  EpnMainInrpStr[EpnMainInrpNum] =
    {
    "dataPortRdy",
    "localTimeNotSync",
    "localTimeInSync",
    "intCoBbhUpsFault",
    "intInvGntLength",
    "intTruantBbhHalt",
    "intDnOutOfOrder",   
    "intUpStatsOverRun",
    "intDnStatsOverRun",
    "intRcvGntTooBig",
    "intWrGntTooBig",
    "intBurstGntTooBig",
    "intBurstInfoFifoOverRun",
    "intRptXmit",
    "intUpTardyPacket",
    "intBadUpFrLen",
    "intGntFullAbort",
    "intGntMissAbort",
    "intDiscGateRx",
    "intGntInterval",
    "intGntTooFarAbort",
    "intGntMisalign",
    "intGntNonPoll",
    "intDelStaleGnt",
    "intGntPres",
    "intRptPres",
    "intL1sFifoOverFlow",
    "intL2sFifoOverFlow",
    "intDrxErrorAbort",
    "intEmptyReportPres",
    "intCoL2sBurstCapOverflowPres",
    "intBbhUpFrAbort"  
    };

////////////////////////////////////////////////////////////////////////////////
/// DisplayLinkStats: Display EPON link stats
///
/// \param argc Argument count
/// \param argv Argument values
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void DisplayLinkStats (U8 linkStart, U8 linkEnd)
    {
    EponLinkStatId  stat;
    LinkIndex  link;
    for (link = linkStart; link < linkEnd; ++link)
        {
        printk("\nLink 0x%0x ( 0x%x ) \n", link, PonMgrGetPhyLlid (link) );
        for (   stat = EponBiDnTotalBytesRx;
            stat <= EponBiDnRegisterFramesRx;
            ++stat)
            {
            U64 upbuf;
            U64 upbuf_fake;
            U64 dnbuf;
            EponReadLinkStat(link, stat, &dnbuf, FALSE);

            if (((EponBiUpTotalBytesTx + stat)) <= EponBiUpUnicastFramesTx)
                {
                EponReadLinkStat (link,
                                  EponBiUpTotalBytesTx + stat,
                                  &upbuf, FALSE);
                EponReadLinkStat (link,
                                  EponBiUpTotalBytesTx + stat,
                                  &upbuf_fake, TRUE);
                printk ( "%25s: %10lld %25s: %10lld %25s_fake: %10lld\n",
                    StatStr[EponLinkStatStrTab[stat+EponBiUpTotalBytesTx]],
                    upbuf,
                    StatStr[EponLinkStatStrTab[stat]],
                    dnbuf,
                    StatStr[EponLinkStatStrTab[stat+EponBiUpTotalBytesTx]],
                    upbuf_fake
                    );
                }
            else
                {
                printk ("%26s %10s %25s: %10lld\n",
                "","",
                StatStr[EponLinkStatStrTab[stat]],
                dnbuf);
                }
            }
        printk ("\n");
        }
    } // DisplayLinkStats



////////////////////////////////////////////////////////////////////////////////
/// DisplayMcastLinkStats: Display EPON link stats
///
/// \param argc Argument count
/// \param argv Argument values
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void DisplayMcastLinkStats (U8 linkStart, U8 linkEnd)
    {
    EponLinkStatId  stat;
    LinkIndex  link;
    for(link = linkStart; link < linkEnd; ++link)
        {
        printk("\nLink 0x%0x ( 0x%x ) \n", link, PonMgrGetPhyLlid (link) );
        for (   stat = EponDnTotalBytesRx;
            stat <= EponDnOamBytesRx;
            ++stat)
            {
            U64 dnbuf;
            EponReadLinkStat (link, stat, &dnbuf, FALSE);
            printk ("%26s %10s %25s: %10lld\n",
                    "", "",
                    StatStr[EponLinkStatStrTab[stat]],
                    dnbuf);
            }
        printk ("\n");
        }
    } // DisplayMcastLinkStats



////////////////////////////////////////////////////////////////////////////////
/// DisplayNonLinkStats: Display EPON non-link stats
///
/// \param argc Argument count
/// \param argv Argument values
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void DisplayNonLinkStats(void)
    {
    U8  stat;
    printk("unmapped stats:\n");
    //general EPON stats
    for(stat = 0; stat <= EponUnmappedLlidSmallFrameCount; ++stat)
        {
        printk("%15s: 0x%0x \n",
            StatStr[EponStatStrTab[stat]],
            EponReadStat(stat)
            );
        }
    } // DisplayNonLinkStats


////////////////////////////////////////////////////////////////////////////////
/// CmdLinkStat: Display EPON link stats
///
/// \param argc Argument count
/// \param argv Argument values
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
//extern
void CmdLinkStat (U8 linkid)
    {
    U8 link;
    U8 linkStart = TkOnuFirstTxLlid;
    U8 linkEnd = TkOnuFirstRxOnlyLlid+TkOnuNumRxOnlyLlids;

    if (linkid < linkEnd)
        {
        linkStart = linkid;
        linkEnd  = (linkid+1);
        }

    for(link = linkStart; link < linkEnd; ++link)
        {
        if(link < TkOnuNumTxLlids)
            {
            DisplayLinkStats(link,link+1);
            }
        else if( (link >= TkOnuFirstRxOnlyLlid) &&
                 (link < (TkOnuFirstRxOnlyLlid+TkOnuNumRxOnlyLlids))
                 )
            {
            DisplayMcastLinkStats(link,link+1);
            }
        else
            {
            //And one for my homey lint.
            }
        }
    } // CmdLinkStat

////////////////////////////////////////////////////////////////////////////////
/// CmdL1AccSel: selects which virtual accumulator sizes are reported
///
/// input: The L1 index
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
//extern
void CmdL1AccSel (U32 linkid)
    {

    } // CmdL1AccSel



////////////////////////////////////////////////////////////////////////////////
/// DisplayL1AccInfo: Signed number of bytes in the selected L1S Shaped Virtual Accumulator
///
/// input: None
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
static
void DisplayEpnInrptInfo(void)
    {
    U32 epnMainInrpStatus = OnuRegRead(&EpnMainInt);
    U32 l1Empty = OnuRegRead(&EpnL1QueueEmpty);
    U32 l1Stop = OnuRegRead(&EpnL1StoppedQueues);
    U32 l1UapEmpty = OnuRegRead(&EpnL1UnshapedQueueEmpty);
    U32 l2QueEmpty = OnuRegRead(&EpnL2QueueEmpty);
    U32 l2Full = OnuRegRead(&EpnL2QueueFull);
    U32 l2Stop = OnuRegRead(&EpnL2StoppedQueues);
    int i = 0;
    U32 *bbhd = (U32 *)BBH_EPON_TX_QUEUE_EMPTY_REG;
    
    printk("\n Main Interrupt status for the EPON module:0x%08X\n", epnMainInrpStatus);
    for(i = 0; i < EpnMainInrpNum; i++)
        {
        printk("%30s[%2d]: %d       ",EpnMainInrpStr[i], i, (epnMainInrpStatus&(1<<i))?1:0);
        if(((i+1)%2)==0)
            {
            printk("\n"); 
            }
        }
    
    printk("\n\n");
    printk("\nL1 status:\n");
    printk("%25s:0x%08X       %25s:0x%08X\n%25s:0x%08X\n","l1Empty",l1Empty,"l1Stop",l1Stop, "l1UapEmpty", l1UapEmpty);    
    printk("\n\n");
    
    printk("\nL2 status:\n");
    printk("%25s:0x%08X       %25s:0x%08X\n%25s:0x%08X\n","l2QueEmpty",l2QueEmpty,"l2Full",l2Full,"l2Stop",l2Stop); 
    printk("\n\n");

    printk("\nBBH empty status: 0x%08X\n", *bbhd);
    printk("\n\n");

    printk("\n\n");
        
    } // DisplayEpnInrptInfo

////////////////////////////////////////////////////////////////////////////////
/// CmdL1AccSel: show Epon MAc L1 info 
///
/// input: None
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
//extern
void CmdEponEpnInfoShow (U8 eponInfoId)
    {
    if(eponInfoId == EpnInfoInterrupt)
        {
        DisplayEpnInrptInfo();
        }
    } // CmdL1AccSel

////////////////////////////////////////////////////////////////////////////////
/// CmdEponStats: Display all EPON stats
///
/// \param argc Argument count
/// \param argv Argument values
///
/// \return None
////////////////////////////////////////////////////////////////////////////////
//extern
void CmdEponStats (void)
    {
    DisplayNonLinkStats ();
    
    DisplayLinkStats (TkOnuFirstTxLlid, TkOnuFirstRxOnlyLlid);

    DisplayMcastLinkStats (TkOnuFirstRxOnlyLlid,
                                 (TkOnuFirstRxOnlyLlid+TkOnuNumRxOnlyLlids) );
    } // CmdEponStats


// end EponCmds.c

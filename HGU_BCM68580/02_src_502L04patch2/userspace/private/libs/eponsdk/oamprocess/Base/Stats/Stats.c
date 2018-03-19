//
// <:copyright-BRCM:2014:proprietary:epon
// 
//    Copyright (c) 2014 Broadcom 
//    All Rights Reserved
// 
//  This program is the proprietary software of Broadcom and/or its
//  licensors, and may only be used, duplicated, modified or distributed pursuant
//  to the terms and conditions of a separate, written license agreement executed
//  between you and Broadcom (an "Authorized License").  Except as set forth in
//  an Authorized License, Broadcom grants no license (express or implied), right
//  to use, or waiver of any kind with respect to the Software, and Broadcom
//  expressly reserves all rights in and to the Software and all intellectual
//  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
//  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
//  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
// 
//  Except as expressly set forth in the Authorized License,
// 
//  1. This program, including its structure, sequence and organization,
//     constitutes the valuable trade secrets of Broadcom, and you shall use
//     all reasonable efforts to protect the confidentiality thereof, and to
//     use this information only in connection with your use of Broadcom
//     integrated circuit products.
// 
//  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
//     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
//     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
//     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
//     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
//     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
//     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
//     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
//     PERFORMANCE OF THE SOFTWARE.
// 
//  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
//     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
//     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
//     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
//     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
//     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
//     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
//     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
//     LIMITED REMEDY.
// :>
//


////////////////////////////////////////////////////////////////////////////////
/// \file Stats.c
/// \brief Support for EPON statistics
///
////////////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include "ethswctl_api.h"
#include "eponctl_api.h"
#include "EponDevInfo.h"
#include "OsAstMsgQ.h"
#include "OsAstEvt.h"
#include "cms_log.h"
#include "Stats.h"

typedef enum
{
    EthStatsIdOffsetRxBytes,          // rx_byte
    EthStatsIdOffsetRxFrames,         // rx_packet
    EthStatsIdOffsetRxFrame64,        // rx_frame_64
    EthStatsIdOffsetRxFrame65_127,    // rx_frame_65_127
    EthStatsIdOffsetRxFrame128_255,   // rx_frame_128_255
    EthStatsIdOffsetRxFrame256_511,   // rx_frame_256_511
    EthStatsIdOffsetRxFrame512_1023,  // rx_frame_512_1023
    EthStatsIdOffsetRxFrame1024_1518, // rx_frame_1024_1518
    EthStatsIdOffsetRxFrame1519_mtu,  // rx_frame_1519_mtu
    EthStatsIdOffsetRxFrameMcast,     // rx_multicast_packet
    EthStatsIdOffsetRxFrameBcast,     // rx_broadcast_packet
    EthStatsIdOffsetRxFrameUcast,     // rx_unicast_packet
    EthStatsIdOffsetRxAlignErr,       // rx_alignment_error
    EthStatsIdOffsetRxLenErr,         // rx_frame_length_error
    EthStatsIdOffsetRxCodeErr,        // rx_code_error
    EthStatsIdOffsetRxSenseErr,       // rx_carrier_sense_error
    EthStatsIdOffsetRxFcsErr,         // rx_fcs_error
    EthStatsIdOffsetRxTooShort,       // rx_undersize_packet
    EthStatsIdOffsetRxTooLong,        // rx_oversize_packet
    EthStatsIdOffsetRxFrags,          // rx_fragments
    EthStatsIdOffsetRxJabber,         // rx_jabber
    EthStatsIdOffsetRxOverFlow,       // rx_overflow
    EthStatsIdOffsetRxControlFrame,         //rx_control_frame
    EthStatsIdOffsetRxPauseControlFrame,    //rx_pause_control_frame
    EthStatsIdOffsetRxUnknownOpcode,        //rx_unknown_opcode
    
    EthStatsIdOffsetTxBytes,          // tx_byte
    EthStatsIdOffsetTxFrames,         // tx_packet
    EthStatsIdOffsetTxFrame64,        // tx_frame_64
    EthStatsIdOffsetTxFrame65_127,    // tx_frame_65_127
    EthStatsIdOffsetTxFrame128_255,   // tx_frame_128_255
    EthStatsIdOffsetTxFrame256_511,   // tx_frame_256_511
    EthStatsIdOffsetTxFrame512_1023,  // tx_frame_512_1023
    EthStatsIdOffsetTxFrame1024_1518, // tx_frame_1024_1518
    EthStatsIdOffsetTxFrame1519_mtu,  // tx_frame_1519_mtu
    EthStatsIdOffsetTxFcsErr,         // tx_fcs_error
    EthStatsIdOffsetTxFrameMcast,     // tx_multicast_packet
    EthStatsIdOffsetTxFrameBcast,     // tx_broadcast_packet
    EthStatsIdOffsetTxFrameUcast,     // tx_unicast_packet
    EthStatsIdOffsetTxCollision,      // tx_total_collision
    EthStatsIdOffsetTxJabber,         // tx_jabber_frame
    EthStatsIdOffsetTxTooLong,        // tx_oversize_frame
    EthStatsIdOffsetTxTooShort,       // tx_undersize_frame
    EthStatsIdOffsetTxFrags,          // tx_fragments_frame
    EthStatsIdOffsetTxError,          // tx_error
    EthStatsIdOffsetTxUnderRun,       // tx_underrun
    EthStatsIdOffsetTxExcessiveCollision,       // tx_excessive_collision
    EthStatsIdOffsetTxLateCollision,            // tx_late_collision
    EthStatsIdOffsetTxSingleCollision,          // tx_single_collision
    EthStatsIdOffsetTxMultipleCollision,        // tx_multiple_collision
    EthStatsIdOffsetTxPauseControlFrame,        // tx_pause_control_frame
    EthStatsIdOffsetTxDeferralPacket,           // tx_deferral_packet
    EthStatsIdOffsetTxControlFrame,             // tx_control_frame
    EthStatsIdOffsetTxExcessiveDeferral,        // tx_excessive_deferral

    EthStatsIdOffsetNum,
        
    EthStatsIdOffsetPortRxTooLong,
    EthStatsIdOffsetPortRxTooShort,
    EthStatsIdOffsetPortRxDropped,
    EthStatsIdOffsetPortTxDropped,

    EthStatsIdOffsetPortNum
} EthStatsIdOffset;

/// threshold crossing information
typedef struct
    {
    U32 raise;          ///< set condition when above
    U32 clear;          ///< clear condition when below
    U32 stats;          ///< stat count in current period
    U32 alarm;          ///< save alarm trigger value for alarm message
    U16 period;         ///< interval for threshold, 100 ms increments
    U16 tick;           ///< ticks for threshold, 100 ms increments
    } PACK StatThd;

typedef struct
    {
    StatThd   statsEthThd[StatIdNumStats];
    } PACK StatsThdEth;

typedef struct
    {
    U64 lastVal;
    BOOL raised;
    } PACK StatsAlarm;

typedef struct
    {
    StatsAlarm  statsAlmEth[StatIdNumStats];
    } PACK StatsAlmEth;

static U16 mapStatsIdToEmac[StatIdNumStats] = 
{
    //StatIdFirst, 
    // Rx stats
    //StatIdRxFirst = StatIdFirst,    
    EthStatsIdOffsetRxBytes,       //StatIdBytesRx = StatIdRxFirst,               
    EthStatsIdOffsetRxFrames,      //StatIdTotalFramesRx,                
    EthStatsIdOffsetRxFrameUcast,  //StatIdUnicastFramesRx,               
    EthStatsIdOffsetRxFrameMcast,  //StatIdMcastFramesRx,       
    EthStatsIdOffsetRxFrameBcast,  //StatIdBcastFramesRx,           
    EthStatsIdOffsetRxFcsErr,      //StatIdFcsErr,
    EthStatsIdOffsetNum,           //StatIdCrc8Err,
    EthStatsIdOffsetRxCodeErr,     //StatIdLineCodeErr,
    EthStatsIdOffsetRxTooShort,    //StatIdFrameTooShort, 
    EthStatsIdOffsetRxTooLong,     //StatIdFrameTooLong,
    EthStatsIdOffsetRxLenErr,      //StatIdInRangeLenErr,
    EthStatsIdOffsetNum,           //StatIdOutRangeLenErr,
    EthStatsIdOffsetRxAlignErr,    //StatIdAlignErr,

    // bin sizes available on Ethernet ports only
    EthStatsIdOffsetRxFrame64,         //StatIdRx64Bytes,
    EthStatsIdOffsetRxFrame65_127,     //StatIdRx65_127Bytes,
    EthStatsIdOffsetRxFrame128_255,    //StatIdRx128_255Bytes,
    EthStatsIdOffsetRxFrame256_511,    //StatIdRx256_511Bytes,
    EthStatsIdOffsetRxFrame512_1023,   //StatIdRx512_1023Bytes,
    EthStatsIdOffsetRxFrame1024_1518,  //StatIdRx1024_1518Bytes,
    EthStatsIdOffsetRxFrame1519_mtu,   //StatIdRx1519PlusBytes,
    EthStatsIdOffsetPortRxDropped,               //StatIdRxFramesDropped,    get from port stats
    EthStatsIdOffsetNum,               //StatIdRxBytesDropped,      // dropped in queue, that is
    EthStatsIdOffsetNum,               //StatIdRxBytesDelayed,
    EthStatsIdOffsetNum,               //StatIdRxMaxDelay,
    EthStatsIdOffsetNum,               //StatIdRxDelayThresh,
    
    EthStatsIdOffsetRxPauseControlFrame,//StatIdRxPauseFrames,
    EthStatsIdOffsetRxControlFrame,    //StatIdRxControlFrames,
    EthStatsIdOffsetNum,               //StatIdErrFrames,
    EthStatsIdOffsetNum,               //StatIdErrFramePeriods,
    EthStatsIdOffsetNum,               //StatIdErrFrameSummary,
                                       //StatIdNumRxStats,

    // Tx stats
    //StatIdTxFirst = StatIdNumRxStats,
    EthStatsIdOffsetTxBytes,           //StatIdBytesTx = StatIdTxFirst,
    EthStatsIdOffsetTxFrames,          //StatIdTotalFramesTx,
    EthStatsIdOffsetTxFrameUcast,      //StatIdUnicastFramesTx,
    EthStatsIdOffsetTxFrameMcast,      //StatIdMcastFramesTx,
    EthStatsIdOffsetTxFrameBcast,      //StatIdBcastFramesTx,
    EthStatsIdOffsetTxSingleCollision,    //StatIdSingleColl,
    EthStatsIdOffsetTxMultipleCollision,  //StatIdMultiColl,
    EthStatsIdOffsetTxLateCollision,      //StatIdLateColl,
    EthStatsIdOffsetTxExcessiveCollision, //StatIdFrAbortXsColl,

    // bin sizes available on Ethernet ports only
    EthStatsIdOffsetTxFrame64,         //StatIdTx64Bytes,
    EthStatsIdOffsetTxFrame65_127,     //StatIdTx65_127Bytes,
    EthStatsIdOffsetTxFrame128_255,    //StatIdTx128_255Bytes,
    EthStatsIdOffsetTxFrame256_511,    //StatIdTx256_511Bytes,
    EthStatsIdOffsetTxFrame512_1023,   //StatIdTx512_1023Bytes,
    EthStatsIdOffsetTxFrame1024_1518,  //StatIdTx1024_1518Bytes,
    EthStatsIdOffsetTxFrame1519_mtu,   //StatIdTx1519PlusBytes,
    EthStatsIdOffsetPortTxDropped,               //StatIdTxFramesDropped,     get from port stats
    EthStatsIdOffsetNum,               //StatIdTxBytesDropped,       // dropped in queue, that is
    EthStatsIdOffsetNum,               //StatIdTxBytesDelayed,
    EthStatsIdOffsetNum,               //StatIdTxMaxDelay,
    EthStatsIdOffsetNum,               //StatIdTxDelayThresh,
    EthStatsIdOffsetNum,               //StatIdTxUpUnusedBytes,
    EthStatsIdOffsetTxPauseControlFrame,    //StatIdTxPauseFrames,
    EthStatsIdOffsetTxExcessiveDeferral,    //StatIdTxExcessiveDeferralFrames,
    EthStatsIdOffsetTxDeferralPacket,       //StatIdTxDeferredFrames,
    EthStatsIdOffsetTxControlFrame,         //StatIdTxControlFrames,

    EthStatsIdOffsetNum,               //StatIdMpcpMACCtrlFramesTx,
    EthStatsIdOffsetNum,               //StatIdMpcpMACCtrlFramesRx,
    EthStatsIdOffsetNum,               //StatIdMpcpTxRegAck,
    EthStatsIdOffsetNum,               //StatIdMpcpTxRegRequest,
    EthStatsIdOffsetNum,               //StatIdMpcpTxReport,
    EthStatsIdOffsetNum,               //StatIdMpcpRxGate,
    EthStatsIdOffsetNum                //StatIdMpcpRxRegister,
    //StatIdNumStats
};


static StatsCntOne  epon_stat[StatIdNumStats] ={{0}};
static StatsCntOne  link_stat[TkOnuMaxBiDirLlids][StatIdNumStats] = {{{0}}};
static struct emac_stats *emac_stat;
static struct rdpa_port_stats *port_stat;
static L2cpStats   *port_l2cp_stat;
static StatThd      statsThdEpon[StatIdNumStats];
static StatThd      statsThdLink[TkOnuMaxBiDirLlids][StatIdNumStats];
static StatsThdEth *statsThdEth;
static StatsAlarm   statsAlmEpon[StatIdNumStats] ={{0}};
static StatsAlarm   statsAlmLink[TkOnuMaxBiDirLlids][StatIdNumStats];
static StatsAlmEth *statsAlmEth; 
static BOOL statsInitDone = FALSE;

static BOOL isStatsAlmRaise(U64 val, U32 thd, const StatsAlarm *almInfo)
{   
    return ((val >= thd) && (!almInfo->raised));
}

static BOOL isStatsAlmClear(U64 val, U32 thd, const StatsAlarm *almInfo)
{   
    return ((val <= thd) && (almInfo->raised));
}

static void CheckStatsThdAlm(StatId id,
                                   U8 inst,
                                   const StatThd *thd, 
                                   StatsAlarm *alm,
                                   U64 curData)
{
    U64 tmpData;

    if (thd->raise == 0)
    {
        alm->lastVal = curData;
        return;
    }
    tmpData = (curData > alm->lastVal)?(curData - alm->lastVal):0;

    if (isStatsAlmRaise(tmpData, thd->raise, alm))
    {
        OsAstMsgQSet(OnuAssertStatId(id), inst, id);
        alm->raised = TRUE;
    }
    if (isStatsAlmClear(tmpData, thd->clear, alm))
    {
        OsAstMsgQClr(OnuAssertStatId(id), inst, id);
        alm->raised = FALSE;
    }
    alm->lastVal = curData;
}


static void CheckStatsAlmEpon(BOOL isFirst)
{
	U32 i;

    if (isFirst)
    {
        for(i=0; i<StatIdNumStats; i++)
        {
            statsAlmEpon[i].lastVal = epon_stat[i].statsVal;
        }
        return;
    }

    for (i = 0; i < StatIdNumStats; i++)
    {
        CheckStatsThdAlm(i, 
                         OsAstStatEponInst(0),
                         &statsThdEpon[i], 
                         &statsAlmEpon[i],
                         epon_stat[i].statsVal);
    }
}

static void CheckStatsAlmLink(U8 link, BOOL isFirst)
{
	U32 i;

    if (isFirst)
    {
        for (i=0; i<StatIdNumStats; i++)
        {
            statsAlmLink[link][i].lastVal = link_stat[link][i].statsVal;
        }
        return;
    }

    for (i = 0; i < StatIdNumStats; i++)
    {        
        CheckStatsThdAlm(i, 
                         OsAstStatLinkInst(link),
                         &statsThdLink[link][i], 
                         &statsAlmLink[link][i],
                         link_stat[link][i].statsVal);
    }
}

static void CheckStatsAlmEth(TkOnuEthPort port, BOOL isFirst)
{
	U32 i;
    U64 tmpVal;
    if (isFirst)
    {
        for (i=0; i<StatIdNumStats; i++)
        {
            if (StatsGetEth(port, i, &tmpVal))
            {
                statsAlmEth[port].statsAlmEth[i].lastVal = tmpVal;
            }
        }
        return;
    }    

    for(i = 0; i < StatIdNumStats; i++)
    {    
        if (StatsGetEth(port, i, &tmpVal))
        {
            CheckStatsThdAlm(i, 
                             OsAstStatPortInst(port),
                             &statsThdEth[port].statsEthThd[i], 
                             &statsAlmEth[port].statsAlmEth[i],
                             tmpVal);
        }
    }
}

void StatsTimerHandle1s (void)
{
    TkOnuEthPort port;
    int rc;
    U8 id;
    U8 link, maxLinks, onlineLinks;
    static BOOL isFirst = TRUE;

    if (!statsInitDone)
    {
        return;
    }
    
    memset(&epon_stat,0,sizeof(StatsCntOne)*StatIdNumStats);
    
    for (id=0; id<StatIdNumStats; id++)
    {
        epon_stat[id].statsId = id;
    }

    rc = eponStack_CtlPonStatsGet(epon_stat, StatIdNumStats);
    if (rc < 0)
    {
       cmsLog_error("read epon stats fail\n");
    } 

    CheckStatsAlmEpon(isFirst);
    eponStack_CfgLinkNum(EponGetOpe, &maxLinks, &onlineLinks);

    for (link = 0; link < maxLinks; link++)
    {        
        memset(&link_stat[link],0,sizeof(StatsCntOne)*StatIdNumStats);
        
        for (id=0; id<StatIdNumStats; id++)
        {
            link_stat[link][id].statsId = id;
        }
        rc = eponStack_CtlLinkStatsGet(link, link_stat[link], StatIdNumStats);
        CheckStatsAlmLink(link, isFirst);
    }

    for (port = 0; port < UniCfgDbGetActivePortCount(); port++)
    {   
        rc = bcm_stat_get_emac(0, bcm_enet_map_oam_idx_to_phys_port(port), &emac_stat[port]);
        rc = bcm_stat_get_port(0, bcm_enet_map_oam_idx_to_phys_port(port), &port_stat[port]);
        rc = eponStack_CtlL2cpStatsGet(bcm_enet_map_oam_idx_to_phys_port(port),&port_l2cp_stat[port]);
        CheckStatsAlmEth(port, isFirst);
    }
    isFirst = FALSE;
} // Stats1sTimer


BOOL StatsGetEpon (StatId id, U64 *dst)
{   
    if (id >= StatIdNumStats)
    {
        return FALSE;
    }
    if(id <= StatIdMpcpRxRegister)
    {
        *dst = epon_stat[id].statsVal;
    }
    else
    {    
        switch (id)
        {
            case StatIdL2cpFramesRx:
                *dst = epon_stat[id].statsVal;
                break;
            case StatIdL2cpOctetsRx:
                *dst = epon_stat[id].statsVal;
                break;
            case StatIdL2cpFramesTx:
                *dst = epon_stat[id].statsVal;
                break;
            case StatIdL2cpOctetsTx:
                *dst = epon_stat[id].statsVal;
                break;
            case StatIdL2cpFramesDiscarded:
                *dst = epon_stat[id].statsVal;
                break;
            case StatIdL2cpOctetsDiscarded:
                *dst = epon_stat[id].statsVal;
                break;
            case StatIdL2ErrorsTx:
                *dst = epon_stat[StatIdTxFramesDropped].statsVal;
                break;
            case StatIdL2ErrorsRx:
                *dst = epon_stat[StatIdFcsErr].statsVal + epon_stat[StatIdFrameTooShort].statsVal + epon_stat[StatIdFrameTooLong].statsVal;
                break;    
            default:
                break;    
        }
    }
    return TRUE;
}


BOOL StatsGetLink (LinkIndex link, StatId id, U64 *dst)
{   
    if (id >= StatIdNumStats)
    {
        return FALSE;
    }

    *dst = link_stat[link][id].statsVal; 
    return TRUE;
}

BOOL StatsGetEth (TkOnuEthPort port, StatId id, U64 *dst)
{
    if ((port >= UniCfgDbGetActivePortCount()) || (id >= StatIdNumStats))
    {
        return FALSE;
    }

    if(id <= StatIdMpcpRxRegister)
    {
        if (mapStatsIdToEmac[id] < EthStatsIdOffsetNum)
        {
            *dst = *(((U64*)(&emac_stat[port])) + mapStatsIdToEmac[id]);
            return TRUE;
        }
        
        if ((mapStatsIdToEmac[id] == EthStatsIdOffsetPortRxDropped) ||
            (mapStatsIdToEmac[id] == EthStatsIdOffsetPortTxDropped) ||
            (mapStatsIdToEmac[id] == EthStatsIdOffsetPortRxTooLong) ||
            (mapStatsIdToEmac[id] == EthStatsIdOffsetPortRxTooShort))
        {
            switch (mapStatsIdToEmac[id])
            {
                case EthStatsIdOffsetPortRxTooLong:
                    *dst = port_stat[port].rx_discard_max_length;
                    break;
                case EthStatsIdOffsetPortRxTooShort:
                    *dst = port_stat[port].rx_discard_min_length;
                    break;
                case EthStatsIdOffsetPortRxDropped:
                    *dst = port_stat[port].rx_discard_1;
                    break;
                case EthStatsIdOffsetPortTxDropped:
                default:
                    *dst = port_stat[port].tx_discard;
                    break;
            //printf("%d stats: 0x%x\n", EthStatsIdOffsetPortTxDropped, ((U32*)dst)[1]);
            }
            return TRUE;
        }
    }
    else
    {
        switch(id)
        {
            case StatIdL2cpFramesRx:
                *dst = port_l2cp_stat[port].rxL2cpFrames;
                break;
            case StatIdL2cpOctetsRx:
                *dst = port_l2cp_stat[port].rxL2cpBytes;
                break;
            case StatIdL2cpFramesTx:
                *dst = port_l2cp_stat[port].txL2cpFrames;
                break;
            case StatIdL2cpOctetsTx:
                *dst = port_l2cp_stat[port].txL2cpBytes;
                break;
            case StatIdL2cpFramesDiscarded:
                *dst = port_l2cp_stat[port].rxL2cpFramesDropped;
                break;
            case StatIdL2cpOctetsDiscarded:
                *dst = port_l2cp_stat[port].rxL2cpBytesDropped;
                break;
            case StatIdL2ErrorsTx:
                *dst = emac_stat[port].rx_alignment_error + emac_stat[port].rx_frame_length_error + emac_stat[port].rx_code_error
                       + emac_stat[port].rx_carrier_sense_error + emac_stat[port].rx_fcs_error;
                break;
            case StatIdL2ErrorsRx:
                *dst = emac_stat[port].tx_error;
                break; 

            default:
                break;
        }
        return TRUE;
    }
    return FALSE;
}  // StatsGetEth


BOOL StatsThdGetEpon(StatId id, StatThdVal *val)
{        
    if (id >= StatIdNumStats)
    {
        return FALSE;
    }

    val->raise = statsThdEpon[id].raise;
    val->clear = statsThdEpon[id].clear;
    return TRUE;
}

BOOL StatsThdGetLink(LinkIndex link, StatId id, StatThdVal *val)
{        
    if (id >= StatIdNumStats)
    {
        return FALSE;
    }

    val->raise = statsThdLink[link][id].raise;
    val->clear = statsThdLink[link][id].clear;
    return TRUE;
}


BOOL StatsThdGetEth(TkOnuEthPort port, StatId id, StatThdVal *val)
{    
    if ( (port >= UniCfgDbGetActivePortCount()) || (id >= StatIdNumStats))
    {
        return FALSE;
    }
    
    //    printf("port is %d, id is %d, p is 0x%x\n", port, id, &dpoeStatsEthThd[port].statsEthThd[id]);
    val->raise = statsThdEth[port].statsEthThd[id].raise;
    val->clear = statsThdEth[port].statsEthThd[id].clear;
    return TRUE;
}


BOOL StatsThdSetEpon(StatId id, const StatThdVal *val)
{    
    if  (id >= StatIdNumStats)
    {
        return FALSE;
    }
    statsThdEpon[id].raise = val->raise;
    statsThdEpon[id].clear = val->clear;
    if(val->raise == 0)
    {
        statsAlmEpon[id].raised = FALSE;
        OnuOsAssertClr(OnuAssertStatId(id), OsAstStatEponInst(0));
    }    
    return TRUE;
}


BOOL StatsThdSetLink(LinkIndex link, StatId id, const StatThdVal *val)
{
    if (id >= StatIdNumStats)
    {
        return FALSE;
    }

    statsThdLink[link][id].raise = val->raise;
    statsThdLink[link][id].clear = val->clear;
    if(val->raise == 0)
    {
        statsAlmLink[link][id].raised = FALSE;
        OnuOsAssertClr(OnuAssertStatId(id), OsAstStatLinkInst(link));
    }
    return TRUE;
}


BOOL StatsThdSetEth(TkOnuEthPort port, StatId id, const StatThdVal *val)
{    
    if ( (port >= UniCfgDbGetActivePortCount()) || (id >= StatIdNumStats))
    {
        return FALSE;
    }

    statsThdEth[port].statsEthThd[id].raise = val->raise;
    statsThdEth[port].statsEthThd[id].clear = val->clear;
    if(val->raise == 0)
    {
        statsAlmEth[port].statsAlmEth[id].raised = FALSE;
        OnuOsAssertClr(OnuAssertStatId(id), OsAstStatPortInst(port));
    }
    return TRUE;
}

static void InitStatsOnce (void)
{
    emac_stat = (struct emac_stats *)malloc(sizeof(struct emac_stats)*UniCfgDbGetActivePortCount());
    if (emac_stat == NULL)
    {
        printf("no memory for emac stats!\n");
    }
    
    port_stat = (struct rdpa_port_stats *)malloc(sizeof(struct rdpa_port_stats)*UniCfgDbGetActivePortCount());
    if (port_stat == NULL)
    {
        printf("no memory for emac stats!\n");
    }
    
    port_l2cp_stat = (L2cpStats *)malloc(sizeof(L2cpStats)*UniCfgDbGetActivePortCount());
    if (port_l2cp_stat == NULL)
    {
       printf("no memory for enetx lan port stats!\n");
    }

    statsThdEth = (StatsThdEth *)malloc(sizeof(StatsThdEth)*UniCfgDbGetActivePortCount());
    if (statsThdEth == NULL)
    {
        printf("no memory for stats thd!\n");
    }       

    statsAlmEth = (StatsAlmEth *)malloc(sizeof(StatsAlmEth)*UniCfgDbGetActivePortCount());
    if (statsAlmEth == NULL)
    {
        printf("no memory for stats alarm!\n");
    }
} // StatsInitOnce

static void InitStatsThdVal(void)
{
    memset(&statsThdEpon[0], 0, sizeof(StatThd)*StatIdNumStats);
    memset(&statsThdLink[0][0], 0, sizeof(StatThd)*StatIdNumStats*TkOnuMaxBiDirLlids);
    memset(statsThdEth, 0, sizeof(StatsThdEth)*UniCfgDbGetActivePortCount());
    memset(&statsAlmEpon[0], 0, sizeof(StatsAlarm)*StatIdNumStats);
    memset(&statsAlmLink[0][0], 0, sizeof(StatsAlarm)*StatIdNumStats*TkOnuMaxBiDirLlids);    
    memset(statsAlmEth, 0, sizeof(StatsAlmEth)*UniCfgDbGetActivePortCount()); 
}

void StatsClearAll(void)
{
    TkOnuEthPort port;
    U16 link;
    int rc;

    if (eponStack_CtlPonStatsClear()<0)
    {
        cmsLog_error("clear epon stats fail\n");
    }
    memset(&epon_stat[0], 0, sizeof(StatsCntOne)*StatIdNumStats);

    for (link = 0; link < TkOnuMaxBiDirLlids; link++)
    {
        if(eponStack_CtlLinkStatsClear(link) < 0)
        {
            cmsLog_error("clear link %d statistic fail\n", link);            
        }
    }
    memset(&link_stat[0][0], 0, sizeof(StatsCntOne)*TkOnuMaxBiDirLlids*StatIdNumStats);
    
    for (port = 0; port < UniCfgDbGetActivePortCount(); port++)
    {        
        rc = bcm_stat_clear_emac(0, bcm_enet_map_oam_idx_to_phys_port(port)); 
        if (rc < 0)
        {
            cmsLog_error("clear eth%d statistic fail\n", port);
        } 
    }  
    memset(&emac_stat[0], 0, sizeof(emac_stat[0])*UniCfgDbGetActivePortCount());
    
    for (port = 0; port < UniCfgDbGetActivePortCount(); port++)
    {
        rc = bcm_stat_port_clear(0, bcm_enet_map_oam_idx_to_phys_port(port));
        if (rc < 0)
        {
            cmsLog_error("clear lan%d statistic fail\n", port);
        } 
    }  
    memset(&port_stat[0], 0, sizeof(port_stat[0])*UniCfgDbGetActivePortCount());
	
    for (port = 0; port < UniCfgDbGetActivePortCount(); port++)
    {
        rc = eponStack_CtlL2cpStatsClear(port);
        if (rc < 0)
        {
            cmsLog_error("clear lan%d statistic fail\n", port);
        } 
    }  
    memset(&port_l2cp_stat[0], 0, sizeof(port_l2cp_stat[0])*UniCfgDbGetActivePortCount());
}

void StatsInit (void)
{
    if (!statsInitDone)
    {
        InitStatsOnce();
        statsInitDone = TRUE;
    }
    StatsClearAll();
    InitStatsThdVal();
} // StatsInit


// end Stats.c

/*
* <:copyright-BRCM:2012:proprietary:epon
* 
*    Copyright (c) 2012 Broadcom 
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
/// \file CtcStats.c
/// \brief General CTC Statistics support
///
/// This file contains the CTC stats processing.
///
////////////////////////////////////////////////////////////////////////////////

#include "Build.h"
#include "CtcStats.h"
#include "CtcAlarms.h"
#include "CtcOnuOam.h"
#include "ethswctl_api.h"
#include "eponctl_api.h"
#include "EponDevInfo.h"
#include "cms_log.h"

////////////////////////////////////////////////////////////////////////////////
/// Macro definition
////////////////////////////////////////////////////////////////////////////////
#define CtcStatsDefPeriod   900 // 15 minutes


////////////////////////////////////////////////////////////////////////////////
/// Typedef struct definition
////////////////////////////////////////////////////////////////////////////////
typedef struct
    {
    BOOL    enable;    
    U32     period;
    U32     time;
    U64     stats[CtcStatIdNums];
    } CtcStats;

typedef struct 
    {
    U64     stats[CtcStatIdNums];    
    } CtcStatsCache;

typedef struct
    {
    bcm_stat_val_t  uniStat;
    } CtcStatIdMap;

typedef struct
    {
    BOOL    up;
    U64     sum;
    }CtcStatPortChg;


////////////////////////////////////////////////////////////////////////////////
/// ctcStatIdMap - CTC stat to Broadcom stat mapping
///
////////////////////////////////////////////////////////////////////////////////
const CtcStatIdMap ctcStatIdMap[CtcStatIdNums] =
    {
    //CtcStatIdDnDropEvents
    {snmpDot3StatsInternalMacTransmitErrors}, 
    
    //CtcStatIdUpDropEvents
    {snmpDot3StatsInternalMacReceiveErrors},
    
    //CtcStatIdDnBytes
    {snmpIfOutOctets},  
    
    //CtcStatIdUpBytes
    {snmpIfInOctets},  
    
    //CtcStatIdDnFrames
    {snmpDot1dTpPortOutFrames}, 
    
    //CtcStatIdUpFrames
    {snmpDot1dTpPortInFrames}, 
    
    //CtcStatIdDnBcastFrames
    {snmpIfHCOutBroadcastPckts}, 
    
    //CtcStatIdUpBcastFrames
    {snmpIfHCInBroadcastPkts}, 
    
    //CtcStatIdDnMcastFrames
    {snmpIfHCOutMulticastPkts}, 
    
    //CtcStatIdUpMcastFrames
    {snmpIfHCInMulticastPkts}, 
    
    //CtcStatIdDnCrcErrFrames
    {snmpValCount},   
    
    //CtcStatIdUpCrcErrFrames
    {snmpDot3StatsFCSErrors}, 
    
    //CtcStatIdDnUndersizeFrames
    {snmpValCount}, 
    
    //CtcStatIdUpUndersizeFrames
    {snmpEtherStatsUndersizePkts},
    
    //CtcStatIdDnOversizeFrames
    {snmpValCount}, 
    
    //CtcStatIdUpOversizeFrames
    {snmpEtherStatsOversizePkts}, 
    
    //CtcStatIdDnFragments
    {snmpValCount}, 
    
    //CtcStatIdUpFragments
    {snmpEtherStatsFragments}, 
    
    //CtcStatIdDnJabbers
    {snmpValCount}, 
    
    //CtcStatIdUpJabbers
    {snmpEtherStatsJabbers}, 
    
    //CtcStatIdDn64Bytes    
    {snmpValCount},   
    
    //CtcStatIdDn64_127Bytes
    {snmpValCount}, 
    
    //CtcStatIdDn128_255Bytes
    {snmpValCount}, 
    
    //CtcStatIdDn256_511Bytes
    {snmpValCount}, 
    
    //CtcStatIdDn512_1023Bytes
    {snmpValCount}, 
    
    //CtcStatIdDn1024_1518Bytes
    {snmpValCount}, 
    
    //CtcStatIdUp64Bytes
    {snmpEtherStatsPkts64Octets},  
    
    //CtcStatIdUp64_127Bytes
    {snmpEtherStatsPkts65to127Octets}, 
    
    //CtcStatIdUp128_255Bytes
    {snmpEtherStatsPkts128to255Octets}, 
    
    //CtcStatIdUp256_511Bytes
    {snmpEtherStatsPkts256to511Octets}, 
    
    //CtcStatIdUp512_1023Bytes
    {snmpEtherStatsPkts512to1023Octets}, 
    
    //CtcStatIdUp1024_1518Bytes
    {snmpEtherStatsPkts1024to1518Octets}, 
    
    //CtcStatIdDnDiscards
    {snmpIfOutDiscards}, 
    
    //CtcStatIdUpDiscards
    {snmpIfInDiscards}, 
    
    //CtcStatIdDnErrors
    {snmpIfOutErrors}, 
    
    //CtcStatIdUpErrors
    {snmpIfInErrors}, 
    
    //CtcStatIdPortChgs
    {snmpValCount}
    };


////////////////////////////////////////////////////////////////////////////////
/// Global variable definition
////////////////////////////////////////////////////////////////////////////////
CtcStatPortChg ctcStatsPortChg[MAX_UNI_PORTS];

/* index 0 is for EPON port, UNI port stats follows */
static CtcStats ctcStats[MAX_UNI_PORTS + 1];  
static CtcStatsCache cacheCtcStats[MAX_UNI_PORTS + 1];

U8 CtcPonPerfMonAlmMaptoStat[CtcPonPerfAlmNum] =
    {
    PerfMonDnDropEvents        ,   /* OamCtcAttrDnStreamDropEvent         */  
    PerfMonUpDropEvents        ,   /* OamCtcAttrUpStreamDropEvent         */  
    PerfMonDnCrcErrFrames      ,   /* OamCtcAttrDnStreamCRCErrFrames      */  
    PerfMonUpCrcErrFrames      ,   /* OamCtcAttrUpStreamCRCErrFrames      */  
    PerfMonDnUndersizeFrames   ,   /* OamCtcAttrDnStreamUndersizeFrames   */  
    PerfMonUpUndersizeFrames   ,   /* OamCtcAttrUpStreamUndersizeFrames   */  
    PerfMonDnOversizeFrames    ,   /* OamCtcAttrDnStreamOversizeFrames    */  
    PerfMonUpOversizeFrames    ,   /* OamCtcAttrUpStreamOversizeFrames    */  
    PerfMonDnFragments         ,   /* OamCtcAttrDnStreamFragments         */  
    PerfMonUpFragments         ,   /* OamCtcAttrUpStreamFragments         */  
    PerfMonDnJabbers           ,   /* OamCtcAttrDnStreamJabbers           */  
    PerfMonUpJabbers           ,   /* OamCtcAttrUpStreamJabbers           */  
    PerfMonDnDiscards          ,   /* OamCtcAttrDnStreamDiscards          */  
    PerfMonUpDiscards          ,   /* OamCtcAttrUpStreamDiscards          */  
    PerfMonDnErrors            ,   /* OamCtcAttrDnStreamErrors            */  
    PerfMonUpErrors            ,   /* OamCtcAttrUpStreamErrors            */  
    PerfMonDnDropEvents        ,   /* OamCtcAttrDnStreamDropEventsWarn    */  
    PerfMonUpDropEvents        ,   /* OamCtcAttrUpStreamDropEventsWarn    */  
    PerfMonDnCrcErrFrames      ,   /* OamCtcAttrDnStreamCRCErrFramesWarn  */  
    PerfMonUpCrcErrFrames      ,   /* OamCtcAttrUpStreamCRCErrFramesWarn  */  
    PerfMonDnUndersizeFrames   ,   /* OamCtcAttrDnStreamUndersizeFrmsWarn */  
    PerfMonUpUndersizeFrames   ,   /* OamCtcAttrUpStreamUndersizeFrmsWarn */  
    PerfMonDnOversizeFrames    ,   /* OamCtcAttrDnStreamOversizeFrmsWarn  */  
    PerfMonUpOversizeFrames    ,   /* OamCtcAttrUpStreamOversizeFrmsWarn  */  
    PerfMonDnFragments         ,   /* OamCtcAttrDnStreamFragmentsWarn     */  
    PerfMonUpFragments         ,   /* OamCtcAttrUpStreamFragmentsWarn     */  
    PerfMonDnJabbers           ,   /* OamCtcAttrDnStreamJabbersWarn       */  
    PerfMonUpJabbers           ,   /* OamCtcAttrUpStreamJabbersWarn       */  
    PerfMonDnDiscards          ,   /* OamCtcAttrDnStreamDiscardsWarn      */  
    PerfMonUpDiscards          ,   /* OamCtcAttrUpStreamDiscardsWarn      */  
    PerfMonDnErrors            ,   /* OamCtcAttrDnStreamErrorsWarn        */  
    PerfMonUpErrors                /* OamCtcAttrUpStreamErrorsWarn        */  
    };

U8 CtcEthPerfMonAlmMaptoStat[CtcEthPerfAlmNum] =
    {
    PerfMonDnDropEvents        , /* OamCtcAttrEthPortDnDropEvents        */   
    PerfMonUpDropEvents        , /* OamCtcAttrEthPortUpDropEvents        */   
    PerfMonDnCrcErrFrames      , /* OamCtcAttrEthPortDnCRCErrFrms        */   
    PerfMonUpCrcErrFrames      , /* OamCtcAttrEthPortUpCRCErrFrms        */   
    PerfMonDnUndersizeFrames   , /* OamCtcAttrEthPortDnUndersizeFrms     */   
    PerfMonUpUndersizeFrames   , /* OamCtcAttrEthPortUpUndersizeFrms     */   
    PerfMonDnOversizeFrames    , /* OamCtcAttrEthPortDnOversizeFrms      */   
    PerfMonUpOversizeFrames    , /* OamCtcAttrEthPortUpOversizeFrms      */   
    PerfMonDnFragments         , /* OamCtcAttrEthPortDnFragments         */   
    PerfMonUpFragments         , /* OamCtcAttrEthPortUpFragments         */   
    PerfMonDnJabbers           , /* OamCtcAttrEthPortDnJabbers           */   
    PerfMonUpJabbers           , /* OamCtcAttrEthPortUpJabbers           */   
    PerfMonDnDiscards          , /* OamCtcAttrEthPortDnDiscards          */   
    PerfMonUpDiscards          , /* OamCtcAttrEthPortUpDiscards          */   
    PerfMonDnErrors            , /* OamCtcAttrEthPortDnErrors            */   
    PerfMonUpErrors            , /* OamCtcAttrEthPortUpErrors            */   
    PerfMonStateChangetimes    , /* OamCtcAttrEthPortStatsChangeTimes    */
    PerfMonDnDropEvents        , /* OamCtcAttrEthPortDnDropEventsWarn    */
    PerfMonUpDropEvents        , /* OamCtcAttrEthPortUpDropEventsWarn    */
    PerfMonDnCrcErrFrames      , /* OamCtcAttrEthPortDnCRCErrFrmsWarn    */
    PerfMonUpCrcErrFrames      , /* OamCtcAttrEthPortUpCRCErrFrmsWarn    */
    PerfMonDnUndersizeFrames   , /* OamCtcAttrEthPortDnUndersizeFrmsWarn */
    PerfMonUpUndersizeFrames   , /* OamCtcAttrEthPortUpUndersizeFrmsWarn */
    PerfMonDnOversizeFrames    , /* OamCtcAttrEthPortDnOversizeFrmsWarn  */
    PerfMonUpOversizeFrames    , /* OamCtcAttrEthPortUpOversizeFrmsWarn  */
    PerfMonDnFragments         , /* OamCtcAttrEthPortDnFragmentsWarn     */
    PerfMonUpFragments         , /* OamCtcAttrEthPortUpFragmentsWarn     */
    PerfMonDnJabbers           , /* OamCtcAttrEthPortDnJabbersWarn       */
    PerfMonUpJabbers           , /* OamCtcAttrEthPortUpJabbersWarn       */
    PerfMonDnDiscards          , /* OamCtcAttrEthPortDnDiscardsWarn      */
    PerfMonUpDiscards          , /* OamCtcAttrEthPortUpDiscardsWarn      */
    PerfMonDnErrors            , /* OamCtcAttrEthPortDnErrorsWarn        */
    PerfMonUpErrors            , /* OamCtcAttrEthPortUpErrorsWarn        */
    PerfMonStateChangetimes      /* OamCtcAttrEthPortStatChangeTimesWarn */
    };

U8 CtcStatsEventCmtPort;//which port need to commit event . one bit for one port
static void CtcStatsClearLan(TkOnuEthPort port);
static int CtcStatsGetLan(TkOnuEthPort port, CtcStatsCache* lan_stat);
static void CtcStatsClearHisStats(TkOnuEthPort port);

////////////////////////////////////////////////////////////////////////////////
/// CtcPerformAlarmChk - check the CTC30 performance monitoring alarm
/// 
/// Parameters:
/// \param obj       alarm object
/// \param port      Ethernet port number from "0" to "TkOnuEthportNum-1"
/// 
/// \return: None
////////////////////////////////////////////////////////////////////////////////
static
void CtcStatsAlarmChk(OamCtcObjType obj, TkOnuEthPort port)
	{
	U8 i;
	CtcAlmMonThd *pAlmThd;
	OamCtcAlarmId almId;
	U64 tmpData;

	if (OamCtcObjPon == obj)
		{
		/* If Performance Monitoring is enabled on the PON, the alarm will be detected */
		if (TRUE == CtcStatsGetEnable(port))
			{			
			for (i = 0; i < CtcPonPerfAlmNum; i++)
				{
				almId = i + OamCtcAttrDnStreamDropEvent;
				pAlmThd = &ctc30AlmThd[almId - OamCtcPonIfAlarmEnd];

				CtcStatsGetStats(port, (CtcStatId)CtcPonPerfMonAlmMaptoStat[i], &tmpData);

				CtcPerfMonAlmNotify(pAlmThd, &tmpData, almId, 0);
				}
			}
		}
	else if (OamCtcObjPort == obj)
		{
		/* 
		 *	If Performance Monitoring is enabled on the port, the alarm for performance
		 *	will be detected. The array index ranges from 0 to TkOnuEthPortNum. The 
		 *	"port " ranges from 0 to TkOnuEthPortNum-1. It will not be overflow.
		 */
		if (TRUE == CtcStatsGetEnable(port + 1))
			{
			for (i = 0; i < CtcEthPerfAlmNum; i++)
				{
				almId = OamCtcAttrEthPortDnDropEvents + i;
				
				pAlmThd = &ctc30EthAlmThd[port][almId - OamCtcPortAlarmEnd];
	       	
				CtcStatsGetStats(port+1, (CtcStatId)CtcEthPerfMonAlmMaptoStat[i], &tmpData);
		        
                CtcPerfMonAlmNotify(pAlmThd, &tmpData, almId, port);
				}
			}
		}
	}


////////////////////////////////////////////////////////////////////////////////
/// CtcStatsGetEnable:  Get CTC stat enable state
///
 // Parameters:
/// \param port    port number
///
/// \return
/// TRUE if stat is enabled, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
BOOL CtcStatsGetEnable(TkOnuEthPort port)
    {
    return ctcStats[port].enable;
    }//CtcStatsGetEnable


////////////////////////////////////////////////////////////////////////////////
/// CtcStatsSetEnable:  Set CTC stat enable state
///
 // Parameters:
/// \param port    port number
/// \param enable  enable 
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
void CtcStatsSetEnable(TkOnuEthPort port, BOOL enable)
{
    if(enable != ctcStats[port].enable)
    {    
        ctcStats[port].enable = enable;
        ctcStats[port].time = 0;
        CtcStatsClear(port);
        CtcStatsClearHisStats(port);
    }
}


////////////////////////////////////////////////////////////////////////////////
/// CtcStatsGetPeriod:  Get CTC stat period
///
 // Parameters:
/// \param port    port number
///
/// \return
/// Stats period
////////////////////////////////////////////////////////////////////////////////
U32 CtcStatsGetPeriod(TkOnuEthPort port)
    {
    return ctcStats[port].period;
    }//CtcStatsGetPeriod


////////////////////////////////////////////////////////////////////////////////
/// CtcStatsSetPeriod:  Set CTC stat period
///
 // Parameters:
/// \param port    port number
/// \param period  period time
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
void CtcStatsSetPeriod(TkOnuEthPort port, U32 period)
    {
    if(period != ctcStats[port].period)
        {    
        ctcStats[port].period = period;
        ctcStats[port].time = 0;
        CtcStatsClear(port);
        CtcStatsClearHisStats(port);
        }
    }//CtcStatsSetPeriod


////////////////////////////////////////////////////////////////////////////////
/// CtcStatsSetPortChg:  Set Port Change 
///
 // Parameters:
/// \param port    port number
/// \param up      link up or down
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
void CtcStatsSetPortChg(TkOnuEthPort port, BOOL up)
    {
    if (up != ctcStatsPortChg[port].up)
        {
        ctcStatsPortChg[port].up = up;
        (ctcStatsPortChg[port].sum)++;
        }
    }//CtcStatsSetPortChg


////////////////////////////////////////////////////////////////////////////////
/// CtcStatsClearPortChg:  Clear Port Change 
///
 // Parameters:
/// \param port    port number
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
void CtcStatsClearPortChg(TkOnuEthPort port)
    {
    ctcStatsPortChg[port].sum = 0;
    }//CtcStatsClearPortChg



////////////////////////////////////////////////////////////////////////////////
/// CtcStatsClearHisStats:  Clear Port History statistics
///
 // Parameters:
/// \param port    port number
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static void CtcStatsClearHisStats(TkOnuEthPort port)
{
    memset(&ctcStats[port].stats, 0x0, sizeof(U64)*CtcStatIdNums);
}

static void CtcStatsClearLan(TkOnuEthPort port)
{
    bcm_stat_clear_emac(0, bcm_enet_map_oam_idx_to_phys_port(port));
    bcm_stat_port_clear(0, bcm_enet_map_oam_idx_to_phys_port(port));
}

static int CtcStatsGetLan(TkOnuEthPort port, CtcStatsCache* lan_stat)
{
	int rc;
    struct emac_stats emac_stat;
    struct rdpa_port_stats port_stat;
    
    rc = bcm_stat_get_emac(0, bcm_enet_map_oam_idx_to_phys_port(port), &emac_stat); 
    rc = bcm_stat_get_port(0, bcm_enet_map_oam_idx_to_phys_port(port), &port_stat);

    lan_stat->stats[CtcStatIdUpBytes] = emac_stat.rx_byte;
	lan_stat->stats[CtcStatIdUpFrames] = emac_stat.rx_packet;
	lan_stat->stats[CtcStatIdUp64Bytes] = emac_stat.rx_frame_64;
	lan_stat->stats[CtcStatIdUp64_127Bytes] = emac_stat.rx_frame_65_127;
	lan_stat->stats[CtcStatIdUp128_255Bytes] = emac_stat.rx_frame_128_255;
	lan_stat->stats[CtcStatIdUp256_511Bytes] = emac_stat.rx_frame_256_511;
	lan_stat->stats[CtcStatIdUp512_1023Bytes] = emac_stat.rx_frame_512_1023;
	lan_stat->stats[CtcStatIdUp1024_1518Bytes] = emac_stat.rx_frame_1024_1518;
    lan_stat->stats[CtcStatIdUpMcastFrames] = emac_stat.rx_multicast_packet;
	lan_stat->stats[CtcStatIdUpBcastFrames] = emac_stat.rx_broadcast_packet;
    lan_stat->stats[CtcStatIdUpCrcErrFrames] = emac_stat.rx_fcs_error;
    lan_stat->stats[CtcStatIdUpOversizeFrames] = emac_stat.rx_oversize_packet;
    lan_stat->stats[CtcStatIdUpUndersizeFrames] = emac_stat.rx_undersize_packet;
    lan_stat->stats[CtcStatIdUpJabbers] = emac_stat.rx_jabber;
    lan_stat->stats[CtcStatIdUpFragments] = emac_stat.rx_fragments;
	lan_stat->stats[CtcStatIdUpErrors] = emac_stat.rx_code_error;
    lan_stat->stats[CtcStatIdUpDiscards] =
        lan_stat->stats[CtcStatIdUpCrcErrFrames] +
        lan_stat->stats[CtcStatIdUpOversizeFrames] +
        lan_stat->stats[CtcStatIdUpUndersizeFrames] +
        lan_stat->stats[CtcStatIdUpJabbers] +
        lan_stat->stats[CtcStatIdUpFragments];
    
	lan_stat->stats[CtcStatIdDnBytes] = emac_stat.tx_byte;
	lan_stat->stats[CtcStatIdDnFrames] = emac_stat.tx_packet;
	lan_stat->stats[CtcStatIdDn64Bytes] = emac_stat.tx_frame_64;
	lan_stat->stats[CtcStatIdDn64_127Bytes] = emac_stat.tx_frame_65_127;
	lan_stat->stats[CtcStatIdDn128_255Bytes] = emac_stat.tx_frame_128_255;
	lan_stat->stats[CtcStatIdDn256_511Bytes] = emac_stat.tx_frame_256_511;
	lan_stat->stats[CtcStatIdDn512_1023Bytes] = emac_stat.tx_frame_512_1023;
	lan_stat->stats[CtcStatIdDn1024_1518Bytes] = emac_stat.tx_frame_1024_1518;
	lan_stat->stats[CtcStatIdDnMcastFrames] = emac_stat.tx_multicast_packet;
	lan_stat->stats[CtcStatIdDnBcastFrames] = emac_stat.tx_broadcast_packet;
    lan_stat->stats[CtcStatIdDnCrcErrFrames] = emac_stat.tx_fcs_error;
    lan_stat->stats[CtcStatIdDnOversizeFrames] = port_stat.rx_discard_max_length;
	lan_stat->stats[CtcStatIdDnUndersizeFrames] = port_stat.rx_discard_min_length;
    lan_stat->stats[CtcStatIdDnJabbers] = emac_stat.tx_jabber_frame;
    lan_stat->stats[CtcStatIdDnFragments] = emac_stat.tx_fragments_frame;
    lan_stat->stats[CtcStatIdDnErrors] = emac_stat.tx_error;
    lan_stat->stats[CtcStatIdDnDiscards] = 
        lan_stat->stats[CtcStatIdDnCrcErrFrames] +
        lan_stat->stats[CtcStatIdDnOversizeFrames] +
        lan_stat->stats[CtcStatIdDnUndersizeFrames] +
        lan_stat->stats[CtcStatIdDnJabbers] +
        lan_stat->stats[CtcStatIdDnFragments];
        
    memcpy(&lan_stat->stats[CtcStatIdPortChgs],
        &ctcStatsPortChg[port].sum, sizeof(U64));
	return rc; 
} 

////////////////////////////////////////////////////////////////////////////////
/// CtcStatsGather:  Collect CTC performance statistics into local memory
///
 // Parameters:
/// \param port    port number
///
/// \return        void
////////////////////////////////////////////////////////////////////////////////
void CtcStatsGather(TkOnuEthPort port)
{
    U8 state_index[CtcStatIdNums];
    int i;

    for(i=0; i<CtcStatIdNums; i++)
    {
        state_index[i] = i;
    }

    /* collecting cache in kernel module, clear local cache for every gathering */
    memset (cacheCtcStats[port].stats, 0x0, sizeof(U64)*CtcStatIdNums);
    
    if (port == 0)
    {
        eponStack_CtlGetCtcStats(FALSE, state_index, CtcStatIdNums, 
                                        &cacheCtcStats[port].stats[0]);
    }
    else
    {
        CtcStatsGetLan(port-1, &cacheCtcStats[port]);
    }
        
}


////////////////////////////////////////////////////////////////////////////////
/// CtcStatsGetStats:  Get CTC stat
///
 // Parameters:
/// \param port    port number
/// \param id      id
/// \param dst     pointer to the stat value
///
/// \return
/// TRUE if stat is supported, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
void CtcStatsGetStats(TkOnuEthPort port, CtcStatId id, U64 *dst)
    {
    memset (dst, 0x0, sizeof(U64));

    if (ctcStats[port].enable)
        {        
        if (id != CtcStatIdPortChgs)
            {
            memcpy(dst, &cacheCtcStats[port].stats[id], sizeof(U64));    
            }
        else if(port > 0)           
            {
            memcpy(dst, &ctcStatsPortChg[port - 1].sum, sizeof(U64));
            }          
        else
            {}
        }
    }//CtcStatsGetStats




////////////////////////////////////////////////////////////////////////////////
/// CtcStatsClear:  Clear CTC stat
///
 // Parameters:
/// \param port    port number
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
void CtcStatsClear(TkOnuEthPort port)
    {
    ctcStats[port].time = 0;
    if (port == 0)
        {
        eponStack_CtlClearCtcStats(); 
        }
    else
        {
        CtcStatsClearLan(port - 1);    
        CtcStatsClearPortChg (port - 1);
        }
	CtcStatsEventCmtPort |= (U8)(1U << port);
    }//CtcStatsClear


////////////////////////////////////////////////////////////////////////////////
/// CtcStatsGetHisStats:  Get CTC history stat
///
 // Parameters:
/// \param port    port number
/// \param id      id
/// \param dst     pointer to the stat value
///
/// \return
/// TRUE if stat is supported, FALSE otherwise
////////////////////////////////////////////////////////////////////////////////
void CtcStatsGetHisStats(TkOnuEthPort port, CtcStatId id, U64 *dst)
    {
    if (ctcStats[port].enable)
        {
        memcpy(dst, &ctcStats[port].stats[id], sizeof(U64));
        }
    else
        {
        cmsLog_notice("performance monitoring not enabled on port %d", port);    
        }
    }//CtcStatsGetHisStats




////////////////////////////////////////////////////////////////////////////////
/// CtcStats1sTimer:  CTC Statistics 1s timer
///
 // Parameters:
/// \param  None
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
// extern
void CtcStats1sTimer (void)
    {
    TkOnuEthPort FAST port;

    for (port = 0; port < (UniCfgDbGetActivePortCount() + 1); port++)
        {
        if (ctcStats[port].enable)
            {
            CtcStatsGather(port);

            if (++(ctcStats[port].time) >= ctcStats[port].period)
                {
                memcpy(&ctcStats[port].stats, &cacheCtcStats[port].stats, sizeof(U64)*CtcStatIdNums);
                CtcStatsClear(port);
                }

			if (port == 0)
				{
				CtcStatsAlarmChk(OamCtcObjPon, 0);
				}
			else
				{
				CtcStatsAlarmChk(OamCtcObjPort, (port-1));
				}
            }      
        }
    } // CtcStats1sTimer

////////////////////////////////////////////////////////////////////////////////
/// CtcStatsAlarmCommit:  CTC event Commit
///
 // Parameters:
/// \param  None
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
// extern
void CtcStatsEventCommit (void)
	{
	U8 port;
	for (port = 0;port < (MAX_UNI_PORTS + 1);port++)
		{
		if (TestBitsSet(CtcStatsEventCmtPort, (1U << port)))
			{
			if (port == 0)
				{
				CtcStatsAlarmChk(OamCtcObjPon, 0);
				}
			else
				{
				CtcStatsAlarmChk(OamCtcObjPort, (port-1));
				}
			CtcStatsEventCmtPort &= (U8)(~(1U << port));
			}
		}
	
	}

////////////////////////////////////////////////////////////////////////////////
/// CtcStatsInit:  CTC Statistics Init
///
 // Parameters:
/// \param  None
///
/// \return
/// none
////////////////////////////////////////////////////////////////////////////////
// extern
void CtcStatsInit (void)  
{
    U8 FAST i;

    U32 module_period;

    CtcStatsEventCmtPort = 0;    
   
    for (i = 0; i < MAX_UNI_PORTS; i++)
    {
        ctcStatsPortChg[i].up = TRUE;
    }   

    for (i = 0; i < (MAX_UNI_PORTS + 1); i++)
    {
        ctcStats[i].enable = FALSE;
        ctcStats[i].period = CtcStatsDefPeriod;
        ctcStats[i].time = 0;
        CtcStatsClearHisStats(i);
    }

    /*
     * Set gathering period in epon module to be maximum value, as stats period is controlled by 
     * timer in OAM stack currently
     * 
     */
    module_period = 0xFFFFFFFF;
    eponStack_CtlCfgCtcStatsPeriod(EponSetOpe, &ctcStats[0].enable, &module_period); 

}


// end CtcStats.c


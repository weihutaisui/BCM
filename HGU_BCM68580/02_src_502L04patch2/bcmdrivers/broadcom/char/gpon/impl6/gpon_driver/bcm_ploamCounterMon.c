/* 
 * <:copyright-BRCM:2007:proprietary:gpon
 * 
 *    Copyright (c) 2007 Broadcom 
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

#include "bcm_gponBasicDefs.h"
#include "bcm_OS_Deps.h"
#include <linux/bcm_log.h>
#include "bcm_ploamCounterMon.h"
#include <rdpa_gpon.h>
#include <rdpa_ag_gpon.h>
#include <linux/time.h>

typedef struct CounterId {
    const char* name;
    uint32_t id;
#define MACDS_COUNTER 0
#define MACUS_COUNTER 1
#define GTCUS_COUNTER 2
#define GTCDS_COUNTER 3
    int block;
} CounterId;

typedef struct gemRxPortCounter{  
    OUT UINT32 rxBytes;
    OUT UINT32 rxFragments;
    OUT UINT32 rxFrames;
    OUT UINT32 rxDroppedFrames;
    OUT UINT32 rxMcastAcceptedFrames;
    OUT UINT32 rxMcastDroppedFrames;
} GemPortRxCounter;

typedef struct gemPortTxCounter{  
    OUT UINT32 txBytes;
    OUT UINT32 txFragments;
    OUT UINT32 txFrames;
    OUT UINT32 txDroppedFrames;
} GemPortTxCounter;

typedef struct CounterMonState {
    uint32_t       macDsMibAcc[BCM_GPON_MACDS_COUNTER_MAX]; 
    GemPortRxCounter gtcDsGemAcc[RDPA_DS_GEM_FLOW_RANGE_HIGH];
    GemPortTxCounter gtcUsGemAcc[RDPA_US_GEM_FLOW_RANGE_HIGH];
    uint32_t       macDsMibLatch[BCM_GPON_MACDS_COUNTER_MAX];
    GemPortRxCounter gtcDsGem[RDPA_DS_GEM_FLOW_RANGE_HIGH];
    GemPortTxCounter gtcUsGem[RDPA_US_GEM_FLOW_RANGE_HIGH];
    int fecReadTimeSec;
    bool           initialized;
} CounterMonState;

static CounterMonState counterMonState;
static CounterMonState* statep = &counterMonState;

extern bdmf_object_handle gponObj;

#define GPON_DS_FRAME_CW 153
#define GPON_FRAME_SEC 8000

void bcm_ploamCounterMonInit(void) 
{
    memset(statep, 0, sizeof(CounterMonState));
    statep->initialized = true;
}

uint32_t bcm_ploamCounterMonGetFecCounter(BCM_Ploam_fecCounters* fec_counter) 
{
    int rc;
    rdpa_fec_stat_t fec_stat;
    struct timeval time;

    rc = rdpa_gpon_fec_stat_get(gponObj, &fec_stat);
    if (rc < 0)
    {
        BCM_LOG_ERROR(BCM_LOG_ID_PLOAM, "Failed to get FEC statistic: rc=%d", rc);
        return -EINVAL_PLOAM_INTERNAL_ERR;
    }
    
    statep->macDsMibLatch[BCM_GPON_MACDS_COUNTER_CERR_FEC] = fec_stat.corrected_codewords; 
    statep->macDsMibLatch[BCM_GPON_MACDS_COUNTER_UERR_FEC] = fec_stat.uncorrectable_codewords;    
    statep->macDsMibLatch[BCM_GPON_MACDS_COUNTER_CBYTE_FEC] = fec_stat.corrected_bytes;    
           
    statep->macDsMibAcc[BCM_GPON_MACDS_COUNTER_CERR_FEC] += statep->macDsMibLatch[BCM_GPON_MACDS_COUNTER_CERR_FEC];
    statep->macDsMibAcc[BCM_GPON_MACDS_COUNTER_UERR_FEC] += statep->macDsMibLatch[BCM_GPON_MACDS_COUNTER_UERR_FEC];
    statep->macDsMibAcc[BCM_GPON_MACDS_COUNTER_CBYTE_FEC] += statep->macDsMibLatch[BCM_GPON_MACDS_COUNTER_CBYTE_FEC];
    
    /* Calculate Total code words: = 1,224,000 X time interval since last read of the corrected/uncorrected code-words [in seconds] */
    do_gettimeofday(&time);

    if (statep->fecReadTimeSec != 0) 
    {
        statep->macDsMibLatch[BCM_GPON_MACDS_COUNTER_NUM_FEC_CW] = (time.tv_sec - statep->fecReadTimeSec)*GPON_DS_FRAME_CW*GPON_FRAME_SEC; 
        statep->macDsMibAcc[BCM_GPON_MACDS_COUNTER_NUM_FEC_CW] += statep->macDsMibLatch[BCM_GPON_MACDS_COUNTER_NUM_FEC_CW];
    }
    
    statep->fecReadTimeSec = time.tv_sec;
    
    fec_counter->fecByte = statep->macDsMibAcc[BCM_GPON_MACDS_COUNTER_CBYTE_FEC];
    fec_counter->fecCerr = statep->macDsMibAcc[BCM_GPON_MACDS_COUNTER_CERR_FEC];
    fec_counter->fecUerr = statep->macDsMibAcc[BCM_GPON_MACDS_COUNTER_UERR_FEC];
    fec_counter->fecCWs = statep->macDsMibAcc[BCM_GPON_MACDS_COUNTER_NUM_FEC_CW];
    fec_counter->fecSecs = 0; /* Not supported */

    if (fec_counter->reset)
    {
        statep->macDsMibAcc[BCM_GPON_MACDS_COUNTER_CBYTE_FEC] = 0;   
        statep->macDsMibAcc[BCM_GPON_MACDS_COUNTER_CERR_FEC] = 0;    
        statep->macDsMibAcc[BCM_GPON_MACDS_COUNTER_UERR_FEC] = 0;    
        statep->fecReadTimeSec = 0;
    }

    return 0;
}

/* return BCM_GPON_MACDS_COUNTER_BIP_ERROR, BCM_GPON_MACDS_COUNTER_UERR_PLOAM */
uint32_t bcm_ploamCounterMonGetLinkCounter(BCM_GponMacDsCounterIds counterId, int reset) 
{
    int rc;
    uint32_t result;
    rdpa_gpon_stat_t link_stat;

    rc = rdpa_gpon_link_stat_get(gponObj, &link_stat);
    if (rc < 0)
    {
        BCM_LOG_ERROR(BCM_LOG_ID_PLOAM, "Failed to get GPON Link statistics: rc=%d", rc);
        return -EINVAL_PLOAM_INTERNAL_ERR;
    }

    statep->macDsMibLatch[BCM_GPON_MACDS_COUNTER_BIP_ERROR] = link_stat.bip_errors;
    statep->macDsMibLatch[BCM_GPON_MACDS_COUNTER_UERR_PLOAM] = link_stat.crc_errors;
    
    statep->macDsMibAcc[BCM_GPON_MACDS_COUNTER_BIP_ERROR] += statep->macDsMibLatch[BCM_GPON_MACDS_COUNTER_BIP_ERROR];
    statep->macDsMibAcc[BCM_GPON_MACDS_COUNTER_UERR_PLOAM] += statep->macDsMibLatch[BCM_GPON_MACDS_COUNTER_UERR_PLOAM];
    
    result = statep->macDsMibAcc[counterId];
    if (reset)
        statep->macDsMibAcc[counterId] = 0;

    return result;
}


/* accumulate_stat - Identity if accumulation is done in PLOAM or RDPA level */
uint32_t bcm_ploamIOCgetGemCounters(uint32_t reset, uint32_t counterId, 
    BCM_Ploam_GemPortCounters *gemCounters)
{
    int rc, error = 0;
    rdpa_gem_flow_ds_cfg_t ds_cfg;
    rdpa_gem_stat_t stat;
    bdmf_object_handle gem = NULL;

    memset (gemCounters, 0, sizeof(BCM_Ploam_GemPortCounters));

    rc = rdpa_gem_get(counterId, &gem);
    rc = rc ? rc : rdpa_gem_ds_cfg_get(gem, &ds_cfg);
    if (rc < 0)
    {
        /* Unconfigured GEM flow - silently return zeroed counter */
        if (rc == BDMF_ERR_NODEV) 
            return 0;

        BCM_LOG_ERROR(BCM_LOG_ID_PLOAM, "Failed to get DS GEM configuration: rc=%d", rc);
        error = -EINVAL_PLOAM_GEM_PORT;
        goto exit;
    }

    rc = rdpa_gem_stat_get(gem, &stat);
    if (rc < 0)
    {
        BCM_LOG_ERROR(BCM_LOG_ID_PLOAM, "Failed to get GEM statistic: rc=%d", rc);
        error = -EINVAL_PLOAM_INTERNAL_ERR;
        goto exit;
    }
    
    gemCounters->rxBytes = stat.rx_bytes;
    gemCounters->rxFrames = stat.rx_packets;
    gemCounters->rxDroppedFrames = stat.rx_packets_discard;
    if (ds_cfg.destination == rdpa_flow_dest_iptv)
    {
        gemCounters->rxMcastAcceptedFrames = stat.rx_packets;
        gemCounters->rxMcastDroppedFrames = stat.rx_packets_discard;       
    }
    gemCounters->txBytes = stat.tx_bytes;
    gemCounters->txFrames = stat.tx_packets;
    gemCounters->txDroppedFrames = stat.tx_packets_discard;
     
    if (reset) 
    {
        rc = rdpa_gem_stat_set(gem, &stat);
        if (rc < 0)
        {
            BCM_LOG_ERROR(BCM_LOG_ID_PLOAM, "Failed to reset GEM statistic: rc=%d", rc);
            error = -EINVAL_PLOAM_INTERNAL_ERR;
            goto exit;
        }
    }

exit:
    if (gem)
        bdmf_put(gem);            
          
    return error;
}


/******************************************************************************
 *
 *  Copyright (c) 2009  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2012:proprietary:standard

 This program is the proprietary software of Broadcom and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").  Except as set forth in
 an Authorized License, Broadcom grants no license (express or implied), right
 to use, or waiver of any kind with respect to the Software, and Broadcom
 expressly reserves all rights in and to the Software and all intellectual
 property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,

 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use
    all reasonable efforts to protect the confidentiality thereof, and to
    use this information only in connection with your use of Broadcom
    integrated circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
    PERFORMANCE OF THE SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
    LIMITED REMEDY.
:>
 *
 ************************************************************************/
#include <devctl_moca.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <errno.h>
#include "mocalib.h"

static void *vctx = NULL;
static sNodeMacAddr gBcastMacAddr = {{0xFFFFFFFF, 0xFFFFFFFF}} ;
const  sNodeMacAddr gNullMacAddr = {{0, 0}};
const  MAC_ADDRESS  gNull_MAC_ADDRESS = {0, 0, 0, 0, 0, 0};

#define SNR_ELEMENTS        10
static const int snr_ref[SNR_ELEMENTS] =    {
                                                (int) (2 * 9.5),
                                                (int) (2 * 12.5),
                                                (int) (2 * 15.5),
                                                (int) (2 * 19),
                                                (int) (2 * 22.5),
                                                (int) (2 * 25.5),
                                                (int) (2 * 29.5),
                                                (int) (2 * 34),
                                                (int) (2 * 37),
                                                (int) (2 * 40)};


CmsRet do_open(void)
{
    static unsigned int ConErrReported=0;
    
    if(! vctx)
        vctx = moca_open(NULL);
    if(! vctx) {
        // avoid flooding msgs by printing the error msg only if it hasn't been reported
        if (0 == ConErrReported)
        {
            printf("Can't connect to mocad\n");            
            ConErrReported = 1;
        }

        return(CMSRET_REQUEST_DENIED);
    }
    else
    {
        // since we previously reported we couldn't connect, now report success.
        if (1==ConErrReported)
        {
            printf("Connected to mocad\n");            
            ConErrReported = 0;
        }
    }
    return(CMSRET_SUCCESS);
}


void * MoCACtl_Open (char * ifname)
{
    return (moca_open(ifname));
}

CmsRet MoCACtl_Close (void * handle)
{
    moca_close(handle);
    return (CMSRET_SUCCESS);
}

static int isValidMidRfChannel(int channel, int rfType)
{
    if (rfType == MoCA_RF_TYPE_F_BAND)
    {
        switch (channel)
        {
            case 0:
            case 675:
            case 700:                
            case 725:
            case 750:
            case 775:
            case 800:
            case 825:
            case 850:                
                return(1);
            default:
                return(0);
        }
    }
    else
    {
        switch (channel)
        {
            case 0:
            case 500:
            case 525:
            case 550:
            case 575:
            case 600:
                return(1);
            default:
                return(0);
        }
    }
}

static int isValidFreqMask(unsigned int mask, int rfType)
{
    if (rfType == MoCA_RF_TYPE_F_BAND)
    {
        if ((mask & 0x3FC00000) == 0)
            return(0);
    }
    else if (rfType == MoCA_RF_TYPE_E_BAND)
    {
        if ((mask & 0x1F000000) == 0)
            return(0);
    }
    else
    {
        if ((mask & 0x05555000) == 0)
            return(0);
    }
    return(1);
}

static int moca_validate_init_options (
    struct moca_init_time_options * pInitOptions)
{
    int     ret = 0;

    if (pInitOptions->const_tx_mode > MoCA_CONTINUOUS_TX_BAND) {
        ret = -1;
        printf("BcmMoCAHost: Invalid Const TX Mode %d \n", pInitOptions->const_tx_mode);
        goto _End ;
    }

    if (pInitOptions->const_tx_mode == MoCA_CONTINUOUS_TX_TONE_SC) {
        if (pInitOptions->const_tx_sc1 > 255) {
            ret = -1;
            printf("BcmMoCAHost: Invalid Sub-Carrier1 %d \n", pInitOptions->const_tx_sc1);
            goto _End ;
        }
    }

    if (pInitOptions->const_tx_mode == MoCA_CONTINUOUS_TX_DUAL_TONE_SC) {
        if (pInitOptions->const_tx_sc1 > 255) {
            ret = -1;
            printf("BcmMoCAHost: Invalid Sub-Carrier1 %d \n", pInitOptions->const_tx_sc1);
            goto _End ;
        }
        if (pInitOptions->const_tx_sc2 > 255) {
            ret = -1;
            printf("BcmMoCAHost: Invalid Sub-Carrier2 %d \n", pInitOptions->const_tx_sc2);
            goto _End ;
        }
    }

_End :
    return (ret) ;

}
static int moca_validate_init_params (struct moca_init_time * pInitParms)
{
    int     ret = 0;

    if ((pInitParms->nc_mode < MoCA_AUTO_NEGOTIATE_FOR_NC) ||
         (pInitParms->nc_mode > MoCA_NEVER_NC)) {
        ret = -1;
        printf("BcmMoCAHost: Invalid NC Mode %d \n", pInitParms->nc_mode) ;
        goto _End ;
    }

    if ((pInitParms->auto_network_search_en < MoCA_AUTO_NW_SCAN_DISABLED) ||
         (pInitParms->auto_network_search_en > MoCA_AUTO_NW_SCAN_DISABLED_NOTABOO)) {
        ret = -1;
        printf("BcmMoCAHost: Invalid AutoNwSearch val %d \n", pInitParms->auto_network_search_en) ;
        goto _End ;
    }

    if ((pInitParms->privacy_en < MoCA_PRIVACY_DISABLED) ||
         (pInitParms->privacy_en > MoCA_PRIVACY_ENABLED)) {
        ret = -1;
        printf("BcmMoCAHost: Invalid privacy val %d \n", pInitParms->privacy_en) ;
        goto _End ;
    }

    if ((pInitParms->tpc_en < MoCA_TPC_DISABLED) ||
         (pInitParms->tpc_en > MoCA_TPC_ENABLED)) {
        ret = -1;
        printf("BcmMoCAHost: Invalid txPwrControl %d \n", pInitParms->tpc_en) ;
        goto _End ;
    }

    if ((pInitParms->continuous_power_tx_mode < MoCA_NORMAL_OPERATION) ||
         (pInitParms->continuous_power_tx_mode > MoCA_CONTINUOUS_RX_LO_ON)) {
        ret = -1;
        printf("BcmMoCAHost: Invalid constTxMode %d \n", pInitParms->continuous_power_tx_mode) ;
        goto _End ;
    }

    if ((pInitParms->lof != MoCA_NULL_FREQ_MHZ) && 
        (pInitParms->lof != MoCA_FREQ_UNSET) &&
         ((pInitParms->lof < MoCA_MIN_FREQ_MHZ) ||
            (pInitParms->lof > MoCA_MAX_FREQ_MHZ))) {
        ret = -1;
        printf("BcmMoCAHost: Invalid last operation freq %d \n", pInitParms->lof) ;
        goto _End ;
    }

    if ((pInitParms->max_tx_power_beacons < MoCA_MIN_TX_POWER_BEACONS) ||
         (pInitParms->max_tx_power_beacons > MoCA_MAX_TX_POWER_BEACONS)) {
        ret = -1;
        printf("BcmMoCAHost: Invalid maxTxPowerBeacons %d \n", pInitParms->max_tx_power_beacons) ;
        goto _End ;
    }

    if ((pInitParms->max_tx_power_beacons < MoCA_MIN_TX_POWER_BEACONS) ||
         (pInitParms->max_tx_power_beacons > MoCA_MAX_TX_POWER_BEACONS)) {
        ret = -1;
        printf("BcmMoCAHost: Invalid maxTxPowerBeacons %d \n", pInitParms->max_tx_power_beacons) ;
        goto _End ;
    }

    if ((pInitParms->max_tx_power_packets < MoCA_MIN_TX_POWER_PACKETS) ||
         (pInitParms->max_tx_power_packets > MoCA_MAX_TX_POWER_PACKETS)) {
        ret = -1;
        printf("BcmMoCAHost: Invalid maxTxPowerPackets %d \n", pInitParms->max_tx_power_packets) ;
        goto _End ;
    }

    if ((pInitParms->bo_mode != MoCA_BO_MODE_FAST) &&
        (pInitParms->bo_mode != MoCA_BO_MODE_SLOW)) {
        ret = -1;
        printf("BcmMoCAHost: Invalid BO Mode %d \n", pInitParms->bo_mode) ;
        goto _End ;
    }     

    if ((pInitParms->mr_non_def_seq_num<0) ||
        (pInitParms->mr_non_def_seq_num> 0xFFFF)) {
        ret = -1;
        printf("BcmMoCAHost: Invalid mr_non_def_seq_num %d \n", pInitParms->mr_non_def_seq_num) ;
        goto _End ;
    }         

    if ((pInitParms->rf_type != MoCA_RF_TYPE_D_BAND)  &&
        (pInitParms->rf_type != MoCA_RF_TYPE_E_BAND)  &&
        (pInitParms->rf_type != MoCA_RF_TYPE_F_BAND)  &&
        (pInitParms->rf_type != MoCA_RF_TYPE_C4_BAND) &&
        (pInitParms->rf_type != 0xFFFFFFFF)) {
        ret = -1;
        printf("BcmMoCAHost: Invalid RF TYPE %d \n", pInitParms->rf_type) ;
        goto _End ;
    }     

    if (!isValidMidRfChannel(pInitParms->beacon_channel, pInitParms->rf_type))
    {
        ret = -1;
        printf("BcmMoCAHost: Invalid beaconChannel %d\n", pInitParms->beacon_channel) ;
        goto _End ;        
    }    

    if (!isValidFreqMask(pInitParms->freq_mask, pInitParms->rf_type))
    {
        ret = -1;
        printf("BcmMoCAHost: Invalid Freq Mask (no usuable channels selected) %08X\n", pInitParms->freq_mask) ;
        goto _End ;
    }

    if (!isValidFreqMask(pInitParms->pns_freq_mask, pInitParms->rf_type))
    {
        ret = -1;
        printf("BcmMoCAHost: Invalid PNS Freq Mask (no usuable channels selected) %08X\n", pInitParms->pns_freq_mask) ;
        goto _End ;
    }    
    
    if ((pInitParms->led_settings != MoCA_LED_MODE_LINK_ON_OFF) &&
        (pInitParms->led_settings != MoCA_LED_MODE_NW_SEARCH) &&
        (pInitParms->led_settings != MoCA_LED_MODE_BLINK_LOW_PHY_TRAFFIC) &&
        (pInitParms->led_settings != MoCA_LED_MODE_BLINK_LOW_PHY_NO_TRAFFIC)) {
        ret = -1;
        printf("BcmMoCAHost: Invalid led_settings %d \n", pInitParms->led_settings) ;
        goto _End ;
    }    

    if ((pInitParms->continuous_rx_mode_attn > MoCA_CONTINUOUS_RX_MODE_ATTN_MAX) ||
        (pInitParms->continuous_rx_mode_attn < MoCA_CONTINUOUS_RX_MODE_ATTN_MIN)) {
        ret = -1;
        printf("BcmMoCAHost: Invalid continuousRxModeAttn %d \n", pInitParms->continuous_rx_mode_attn) ;
        goto _End ;
    }    
    
    if ((pInitParms->multicast_mode < MoCA_MCAST_NORMAL_MODE) ||
         (pInitParms->multicast_mode > MoCA_MCAST_BCAST_MODE)) {
        ret = -1;
        printf("BcmMoCAHost: Invalid MCAST Mode %d \n", pInitParms->multicast_mode) ;
        goto _End ;
    }

    if ((pInitParms->lab_mode < MoCA_NORMAL_MODE) ||
         (pInitParms->lab_mode > MoCA_LAB_MODE)) {
        ret = -1;
        printf("BcmMoCAHost: Invalid Lab Mode %d \n", pInitParms->lab_mode) ;
        goto _End ;
    }

    if ((pInitParms->taboo_fixed_channel_mask & ~MoCA_VALID_TABOO_FIXED_CHAN_MASK) != 0) {
            ret = -1;
        printf("BcmMoCAHost: Invalid TABOO channel mask 0x%x \n", pInitParms->taboo_fixed_channel_mask) ;
            goto _End ;
        }

    if ((pInitParms->taboo_left_mask & ~MoCA_VALID_TABOO_LEFT_MASK) != 0) {
            ret = -1;
        printf("BcmMoCAHost: Invalid left TABOO channel mask 0x%x \n", pInitParms->taboo_left_mask) ;
            goto _End ;
        }

    if ((pInitParms->taboo_right_mask & ~MoCA_VALID_TABOO_RIGHT_MASK) != 0) {
        ret = -1;
        printf("BcmMoCAHost: Invalid right TABOO channel mask 0x%x \n", pInitParms->taboo_right_mask) ;
        goto _End ;
    }

    if ((pInitParms->pad_power < MoCA_MIN_PAD_POWER) ||
         (pInitParms->pad_power > MoCA_MAX_PAD_POWER)) {
        ret = -1;
        printf("BcmMoCAHost: Invalid PAD power %d \n", pInitParms->pad_power) ;
        goto _End ;
    }

    if ((pInitParms->beacon_pwr_reduction < MoCA_MIN_BEACON_PWR_REDUCTION) ||
         (pInitParms->beacon_pwr_reduction> MoCA_MAX_BEACON_PWR_REDUCTION)) {
        ret = -1;
        printf("BcmMoCAHost: Invalid Beacon Power Reduction %d \n", pInitParms->beacon_pwr_reduction) ;
        goto _End ;
    }    

    if ((pInitParms->terminal_intermediate_device < 0) ||
         (pInitParms->terminal_intermediate_device > 1)) {
        ret = -1;
        printf("BcmMoCAHost: Terminal_intermediate_device %d \n", pInitParms->terminal_intermediate_device) ;
        goto _End ;
    }

    if ((pInitParms->preferred_nc < MoCA_NO_PREFERED_NC_MODE) ||
         (pInitParms->preferred_nc > MoCA_PREFERED_NC_MODE)) {
        ret = -1;
        printf("BcmMoCAHost: Invalid Prefered NC Node %d \n",
                pInitParms->preferred_nc) ;
        goto _End ;
    }

    if ((pInitParms->moca_loopback_en != MoCA_LOOPBACK_DISABLED) &&
        (pInitParms->moca_loopback_en != MoCA_LOOPBACK_ENABLED)) {
        ret = -1;
        printf("BcmMoCAHost: Invalid moca_loopback_en %d \n",
                pInitParms->moca_loopback_en) ;
        goto _End ;
    }

_End :
    return (ret) ;
}


/* This routine scans through the given config params structure based on the
 * config mask (a valid parameter will have the corresponding config mask bit
 * set) and validates their values to make sure they are in the allowable
 * range. It reports error back incase of any invalid parameter.
 * For the parameters which are not set any values, the existing value from the
 * context structure (default/previously configured) will be taken.
 */
static int moca_validate_anytime_params (void * ctx,
                                         PMoCA_CONFIG_PARAMS pConfigParams,
                                         unsigned long long configMask)
{
    int     ret = 0 ;
    int     numberOfParams, i ;
    uint64_t startMask = 0x00000001LL ;

    for (numberOfParams = 0 ; numberOfParams < MoCA_CFG_MAX_PARAMS ; numberOfParams++) {
        switch (startMask & configMask) {
            case MoCA_CFG_PARAM_MAX_FRAME_SIZE_MASK     :
                if ((pConfigParams->maxFrameSize < MoCA_MIN_FRAME_SIZE) ||
                        (pConfigParams->maxFrameSize > MoCA_MAX_FRAME_SIZE)) {
                    ret = -1;
                    printf("BcmMoCAHost: Invalid Max Frame Size %d \n",
                            pConfigParams->maxFrameSize) ;
                    goto _End ;
                }
                break ;
            case MoCA_CFG_PARAM_MAX_TRANS_TIME_MASK     :
                if ((pConfigParams->maxTransmitTime < MoCA_MIN_TRANSMIT_TIME) ||
                        (pConfigParams->maxTransmitTime > MoCA_MAX_TRANSMIT_TIME)) {
                    ret = -1;
                    printf("BcmMoCAHost: Invalid Max Transmit Time %d \n",
                            pConfigParams->maxTransmitTime) ;
                    goto _End ;
                }
                break ;
            case MoCA_CFG_PARAM_MIN_BW_ALARM_THRE_MASK  :
                if (pConfigParams->minBwAlarmThreshold != MoCA_MIN_BW_ALARM_THRESHOLD_DISABLED) {
                    if ((pConfigParams->minBwAlarmThreshold < MoCA_MIN_BW_ALARM_THRESHOLD) ||
                            (pConfigParams->minBwAlarmThreshold > MoCA_MAX_BW_ALARM_THRESHOLD)) {
                        ret = -1;
                        printf("BcmMoCAHost: Invalid Min BW Alarm Threshold %d \n",
                                pConfigParams->minBwAlarmThreshold) ;
                        goto _End ;
                    }
                }
                break ;
            case MoCA_CFG_PARAM_OUT_OF_ORDER_LMO_MASK   :
                if ((pConfigParams->outOfOrderLmo < 0) ||
                        (pConfigParams->outOfOrderLmo > (MoCA_MAX_NODES-1))) {
                    ret = -1;
                    printf("BcmMoCAHost: Invalid Out Of Order LMO Node Id.  %d \n",
                            pConfigParams->outOfOrderLmo) ;
                    goto _End ;
                }
                break ;
            case MoCA_CFG_PARAM_CONT_IE_RR_INS_MASK     :
                if ((pConfigParams->continuousIERRInsert < MoCA_CONTINUOUS_IE_RR_INSERT_OFF) ||
                        (pConfigParams->continuousIERRInsert > MoCA_CONTINUOUS_IE_RR_INSERT_ON)) {
                    ret = -1;
                    printf("BcmMoCAHost: Invalid continuous IE RR Insert %d \n",
                            pConfigParams->continuousIERRInsert) ;
                    goto _End ;
                }
                break ;
            case MoCA_CFG_PARAM_CONT_IE_MAP_INS_MASK        :
                if ((pConfigParams->continuousIEMapInsert < MoCA_CONTINUOUS_IE_MAP_INSERT_OFF) ||
                        (pConfigParams->continuousIEMapInsert > MoCA_CONTINUOUS_IE_MAP_INSERT_ON)) {
                    ret = -1;
                    printf("BcmMoCAHost: Invalid continuous IE MAP Insert %d \n",
                            pConfigParams->continuousIEMapInsert) ;
                    goto _End ;
                }
                break ;
            case MoCA_CFG_PARAM_MAX_PKT_AGGR_MASK           :
                if ((pConfigParams->maxPktAggr < MoCA_MIN_PKT_AGGR) ||
                        (pConfigParams->maxPktAggr > MoCA_MAX_PKT_AGGR)) {
                    ret = -1;
                    printf("BcmMoCAHost: Invalid Max Pkt Aggr %d \n", pConfigParams->maxPktAggr) ;
                    goto _End ;
                }
                break ;
            case MoCA_CFG_PARAM_MAX_CONSTELLATION_MASK  :
                for (i = 0; i < MoCA_MAX_NODES; i++)
                {
                    if (pConfigParams->constellation[i]>= MAX_CONSTELLATION) {
                        ret = -1;
                        printf("BcmMoCAHost: Invalid Max Constellation values node:info -> %d:%u \n",
                                i, pConfigParams->constellation[i]) ;
                        goto _End ;
                    }
                }
                break ;
            case MoCA_CFG_PARAM_FREQ_SHIFT_MASK:
                if ((pConfigParams->freqShiftMode < MoCA_MIN_FREQ_SHIFT_MODE) ||
                    (pConfigParams->freqShiftMode > MoCA_MAX_FREQ_SHIFT_MODE))
                {
                    ret = -1;
                    printf("BcmMoCAHost: Invalid Freq Shift Mode %d \n",
                            pConfigParams->freqShiftMode) ;
                    goto _End ;
                }
                break;
            case MoCA_CFG_PARAM_PMK_EXCHG_INTVL_MASK        :
                if ((pConfigParams->pmkExchangeInterval < MoCA_MIN_PMK_EXCHANGE_INTERVAL) ||
                        (pConfigParams->pmkExchangeInterval > MoCA_MAX_PMK_EXCHANGE_INTERVAL)) {
                    ret = -1;
                    printf("BcmMoCAHost: Invalid PMK EXCHG Interval %d hrs \n",
                            pConfigParams->pmkExchangeInterval) ;
                    goto _End ;
                }
                break ;
            case MoCA_CFG_PARAM_SAPM_EN_MASK    :
                if ((pConfigParams->sapmEn < MoCA_MIN_SAPM_EN) ||
                        (pConfigParams->sapmEn > MoCA_MAX_SAPM_EN)) {
                    ret = -1;
                    printf("BcmMoCAHost: Invalid SAPM_EN %d\n",
                            pConfigParams->sapmEn) ;
                    goto _End ;
                }
                break ;   
            case MoCA_CFG_PARAM_SAPM_TABLE_MASK         :
                for (i = 0 ; i < MoCA_MAX_SAPM_TBL_INDEX ; i++) {
                    if (pConfigParams->sapmTable.sapmTableLo[i] > (MoCA_MAX_SAPM_TABLE * 2)) {
                        ret = -1;
                        printf("BcmMoCAHost: Invalid sapmTableLo %d.%d index %d \n",
                                (pConfigParams->sapmTable.sapmTableLo[i] / 2), abs((pConfigParams->sapmTable.sapmTableLo[i] % 2) * 5), i) ;
                        goto _End ;
                    }
                }     
                for (i = 0 ; i < MoCA_MAX_SAPM_TBL_INDEX ; i++) {
                    if (pConfigParams->sapmTable.sapmTableHi[i] > (MoCA_MAX_SAPM_TABLE * 2)) {
                        ret = -1;
                        printf("BcmMoCAHost: Invalid sapmTableHi %d.%d index %d \n",
                                (pConfigParams->sapmTable.sapmTableHi[i] / 2), abs((pConfigParams->sapmTable.sapmTableHi[i] % 2) * 5), i) ;
                        goto _End ;
                    }
                }                
                break;
            case MoCA_CFG_PARAM_RLAPM_EN_MASK       :
                if (pConfigParams->rlapmEn > MoCA_MAX_RLAPM_EN) {
                    ret = -1;
                    printf("BcmMoCAHost: Invalid RLAPM_EN %d\n",
                            pConfigParams->rlapmEn) ;
                    goto _End ;
                }
                break ;              
            case MoCA_CFG_PARAM_RLAPM_TABLE_MASK            :
                for (i = 0 ; i < MoCA_MAX_RLAPM_TBL_INDEX ; i++) {
                    if (pConfigParams->rlapmTable[i] > (MoCA_MAX_RLAPM_TABLE * 2)) {
                        ret = -1;
                        printf("BcmMoCAHost: Invalid rlapmTable %d.%d index %d \n",
                                (pConfigParams->rlapmTable[i] / 2), abs((pConfigParams->rlapmTable[i] % 2) * 5), i) ;
                        goto _End ;
                    }
                }              
                break;                
            case MoCA_CFG_PARAM_ARPL_TH_MASK        :
                if ((pConfigParams->arplTh < MoCA_MIN_ARPL_TH) ||
                        (pConfigParams->arplTh > MoCA_MAX_ARPL_TH)) {
                    ret = -1;
                    printf("BcmMoCAHost: Invalid ARPL_TH %d\n",
                            pConfigParams->arplTh) ;
                    goto _End ;
                }
                break ;
            case MoCA_CFG_PARAM_TEK_EXCHG_INTVL_MASK    :                
                if ((pConfigParams->tekExchangeInterval < MoCA_MIN_TEK_EXCHANGE_INTERVAL) ||
                        (pConfigParams->tekExchangeInterval > MoCA_MAX_TEK_EXCHANGE_INTERVAL)) {
                    ret = -1;
                    printf("BcmMoCAHost: Invalid TEK Exchange Interval %d min\n",
                            pConfigParams->tekExchangeInterval) ;
                    goto _End ;
                }
                break ;
            case MoCA_CFG_PARAM_SNR_MARGIN_MASK         :
                if ((pConfigParams->snrMargin < (MoCA_MIN_USER_SNR_MARGIN * 2)) ||
                        (pConfigParams->snrMargin > (MoCA_MAX_USER_SNR_MARGIN * 2))) {
                    ret = -1;
                    printf("BcmMoCAHost: Invalid SNR Margin %d.%d \n",
                            (pConfigParams->snrMargin / 2), abs((pConfigParams->snrMargin % 2) * 5)) ;
                    goto _End ;
                }
                /* No break - Fall through to Margin Offset mask, the two need to be 
                                  specified together */
            case MoCA_CFG_PARAM_SNR_MARGIN_OFFSET_MASK          :
                for (i = 0 ; i < MoCA_MAX_SNR_TBL_INDEX ; i++) {
                    if ((pConfigParams->snrMarginOffset[i] < (MoCA_MIN_USER_SNR_MARGIN_OFFSET * 2)) ||
                            (pConfigParams->snrMarginOffset[i] > (MoCA_MAX_USER_SNR_MARGIN_OFFSET * 2))) {
                        ret = -1;
                        printf("BcmMoCAHost: Invalid SNR Margin Offset %d.%d index %d \n",
                                (pConfigParams->snrMarginOffset[i] / 2), abs((pConfigParams->snrMarginOffset[i] % 2) * 5), i) ;
                        goto _End ;
                    }
                }

                for (i = 0; i < SNR_ELEMENTS; i++) {
                    if (((snr_ref[i] + pConfigParams->snrMargin + pConfigParams->snrMarginOffset[i]) > 127) ||
                        ((snr_ref[i] + pConfigParams->snrMargin + pConfigParams->snrMarginOffset[i]) < -128))
                    {
                        ret = -1;
                        printf("BcmMoCAHost: Invalid SNR Margin Base/Offset combo %d.%d/%d.%d index %d \n",
                                (pConfigParams->snrMargin / 2), abs((pConfigParams->snrMargin % 2) * 5),
                                (pConfigParams->snrMarginOffset[i] / 2), abs((pConfigParams->snrMarginOffset[i] % 2) * 5), i) ;
                        goto _End ;
                    }
                }
                break ;
            case MoCA_CFG_PARAM_PRIO_ALLOCATIONS_MASK    :
                if ((pConfigParams->prioAllocation.resvHigh > MoCA_PRIO_ALLOC_RESV_HIGH) ||
                     (pConfigParams->prioAllocation.resvMed > MoCA_PRIO_ALLOC_RESV_MED) ||
                     (pConfigParams->prioAllocation.resvLow > MoCA_PRIO_ALLOC_RESV_LOW) ||
                     (pConfigParams->prioAllocation.limitHigh > MoCA_PRIO_ALLOC_LIMIT_HIGH) ||
                     (pConfigParams->prioAllocation.limitMed > MoCA_PRIO_ALLOC_LIMIT_MED) ||
                     (pConfigParams->prioAllocation.limitLow > MoCA_PRIO_ALLOC_LIMIT_LOW) ||
                     (pConfigParams->prioAllocation.limitHigh < pConfigParams->prioAllocation.resvHigh) ||
                     (pConfigParams->prioAllocation.limitMed < pConfigParams->prioAllocation.resvMed) ||
                     (pConfigParams->prioAllocation.limitLow < pConfigParams->prioAllocation.resvLow)) {
                    ret = -1;
                    printf("BcmMoCAHost: Invalid PrioAllocation values %d(RH):%d(RM):%d(RL):%d(LH):%d(LM):%d(LL) \n",
                            pConfigParams->prioAllocation.resvHigh,
                            pConfigParams->prioAllocation.resvMed,
                            pConfigParams->prioAllocation.resvLow,
                            pConfigParams->prioAllocation.limitHigh,
                            pConfigParams->prioAllocation.limitMed,
                            pConfigParams->prioAllocation.limitLow) ;
                    goto _End ;
                }
                break ;
            case MoCA_CFG_PARAM_TARGET_PHY_RATE_QAM128_MASK  :
                if ((pConfigParams->targetPhyRateQAM128 < MoCA_MIN_TARGET_PHY_RATE_QAM128) ||
                     (pConfigParams->targetPhyRateQAM128 > MoCA_MAX_TARGET_PHY_RATE_QAM128)) {
                    ret = -1;
                    printf("BcmMoCAHost: Invalid target PHY rate QAM128 %d \n",
                                 pConfigParams->targetPhyRateQAM128) ;
                    goto _End ;
                }
                break ;
            case MoCA_CFG_PARAM_TARGET_PHY_RATE_QAM256_MASK  :
                if ((pConfigParams->targetPhyRateQAM256 < MoCA_MIN_TARGET_PHY_RATE_QAM256) ||
                     (pConfigParams->targetPhyRateQAM256 > MoCA_MAX_TARGET_PHY_RATE_QAM256)) {
                    ret = -1;
                    printf("BcmMoCAHost: Invalid target PHY rate QAM256 %d \n",
                                 pConfigParams->targetPhyRateQAM256) ;
                    goto _End ;
                }
                break ;
           case MoCA_CFG_PARAM_TARGET_PHY_RATE_TURBO_MASK  :
                if ((pConfigParams->targetPhyRateTurbo < MoCA_MIN_TARGET_PHY_RATE_TURBO) ||
                     (pConfigParams->targetPhyRateTurbo > MoCA_MAX_TARGET_PHY_RATE_TURBO)) {
                    ret = -1;
                    printf("BcmMoCAHost: Invalid target PHY rate turbo %d \n",
                                 pConfigParams->targetPhyRateTurbo) ;
                    goto _End ;
                }
                break ;
           case MoCA_CFG_PARAM_TARGET_PHY_RATE_TURBO_PLUS_MASK  :
                if ((pConfigParams->targetPhyRateTurboPlus < MoCA_MIN_TARGET_PHY_RATE_TURBO_PLUS) ||
                     (pConfigParams->targetPhyRateTurboPlus > MoCA_MAX_TARGET_PHY_RATE_TURBO_PLUS)) {
                    ret = -1;
                    printf("BcmMoCAHost: Invalid target PHY rate turbo plus %d \n",
                                 pConfigParams->targetPhyRateTurboPlus) ;
                    goto _End ;
                }
                break ;            
            case MoCA_CFG_PARAM_SELECTIVE_RR_MASK  :
                 if ((pConfigParams->selectiveRR< MoCA_MIN_SELECTIVE_RR) ||
                      (pConfigParams->selectiveRR> MoCA_MAX_SELECTIVE_RR)) {
                     ret = -1;
                     printf("BcmMoCAHost: Invalid selectiveRR %d \n",
                                  pConfigParams->selectiveRR) ;
                     goto _End ;
                 }
                 break ;                            
            case MoCA_CFG_PARAM_CORE_TRACE_CONTROL_MASK       :
                if ((pConfigParams->coreTraceControl < MoCA_CORE_TRACE_CONTROL_DISABLE) ||
                     (pConfigParams->coreTraceControl > MoCA_CORE_TRACE_CONTROL_ENABLE)) {
                    ret = -1;
                    printf("BcmMoCAHost: Invalid core trace control %d \n",
                                 pConfigParams->coreTraceControl) ;
                    goto _End ;
                }
                break ;
            case MoCA_CFG_PARAM_IQ_DIAGRAM_SET_MASK     :
                if ((pConfigParams->iq.nodeId > MoCA_MAX_NODES) ||
                        ((pConfigParams->iq.burstType != MoCA_IQ_BURST_TYPE_BEACON) &&
                         (pConfigParams->iq.burstType != MoCA_IQ_BURST_TYPE_MAP) &&
                         (pConfigParams->iq.burstType != MoCA_IQ_BURST_TYPE_UNICAST) &&
                         (pConfigParams->iq.burstType != MoCA_IQ_BURST_TYPE_BROADCAST)) ||
                        ((pConfigParams->iq.acmtSymNum != MoCA_IQ_ACMT_SYM_NUM_LAST_SYM) &&
                         (pConfigParams->iq.acmtSymNum != MoCA_IQ_ACMT_SYM_NUM_SYM_BEFORE_LAST))) {
                    ret = -1;
                    printf("BcmMoCAHost: Invalid IQ Diagram Set Value(s) Node:Burst:Acmt => %d:%d:%d \n",
                            pConfigParams->iq.nodeId, pConfigParams->iq.burstType, pConfigParams->iq.acmtSymNum) ;
                    goto _End ;
                }
                break ;
            case MoCA_CFG_PARAM_SNR_GRAPH_SET_MASK      :
                if (pConfigParams->MoCASnrGraphSetNodeId > MoCA_MAX_NODES) {
                    ret = -1;
                    printf("BcmMoCAHost: Invalid SNR GRAPH_SET Value Node: %d \n",
                            pConfigParams->MoCASnrGraphSetNodeId) ;
                    goto _End ;
                }
                break ;
            case MoCA_CFG_PARAM_LAB_CALL_FUNC_MASK      :
                if (pConfigParams->callFunc.funcAddr == 0) {
                    ret = -1;
                    printf("BcmMoCAHost: Invalid LAB CALL Func : %x \n",
                            pConfigParams->callFunc.funcAddr) ;
                    goto _End ;
                }
                break ;
            case MoCA_CFG_PARAM_MAX_MAP_CYCLE_MASK        :
            case MoCA_CFG_PARAM_MIN_MAP_CYCLE_MASK        :
                if ((configMask & MoCA_CFG_PARAM_MAX_MAP_CYCLE_MASK) == 0)
                    moca_get_max_map_cycle(ctx, &pConfigParams->maxMapCycle);
                if ((configMask & MoCA_CFG_PARAM_MIN_MAP_CYCLE_MASK) == 0)
                    moca_get_min_map_cycle(ctx, &pConfigParams->minMapCycle);

                if ((pConfigParams->minMapCycle < 750) ||
                    (pConfigParams->minMapCycle > pConfigParams->maxMapCycle) ||
                    (pConfigParams->maxMapCycle > 2000) ||
                    (pConfigParams->maxMapCycle-pConfigParams->minMapCycle<120 )  ) {
                    ret = -1;
                    printf("BcmMoCAHost: Invalid min/max Map Cycle %d/%d \n",
                            pConfigParams->minMapCycle, pConfigParams->maxMapCycle) ;
                    printf("BcmMoCAHost: 750<=min<=max<=2000 and max-min>=120\n");

                    goto _End ;
                }
                break ;
            default                                                 :
                /* Pilots may fall in here. It is an RO parameter and so an invalid
                 * one for SET.
                 */
                if (configMask & MoCA_CFG_PARAM_LAB_PILOTS_MASK) {
                    ret = -1;
                    printf("BcmMoCAHost: Invalid Parameter Mask:%llx for SET \n", configMask) ;
                    goto _End ;
                }
                break ;
        } /* switch (startMask) */
        startMask <<= 1 ;
    } /* for (numberOfParams) */

_End :
    return (ret) ;
}

/**Initializes MOCA driver
 *
 * This function is called to initialize the MOCA driver with init time
 * configuration parameters and optional any time configuration 
 * parameters.
 *
 * @param pInitParms (IN) - A pointer to MOCA_INITIALIZATION_PARMS.
 * @param pCfgParms (IN)  - A pointer to MoCA_CONFIG_PARAMS. This is an
 *                                optional parameter. Not used if set to NULL
 *                                or if configMask set to 0.
 * @param configMask (IN) - Bit mask of pCfgParms to be applied during 
 *                                initialization. This is an optional parameter.
 *                                Not used if set to 0 or if pCfgParms set to NULL.
 *
 * @return CmsRet enum.
 */
CmsRet MoCACtl2_Initialize( 
    void * ctx, 
    PMoCA_INITIALIZATION_PARMS pMoCAInitParms,
    PMoCA_CONFIG_PARAMS pCfgParams, 
    unsigned long long configMask)
{
    int ret;

    ret = MoCACtl2_SetInitParms(ctx, pMoCAInitParms, MoCA_INIT_PARAM_ALL_MASK);
    if (ret != CMSRET_SUCCESS)
        return(CMSRET_INVALID_ARGUMENTS);
    
    ret = MoCACtl2_SetCfg( ctx, pCfgParams, configMask );
    if (ret != CMSRET_SUCCESS)
        return(CMSRET_INVALID_ARGUMENTS);
    if(pMoCAInitParms->initOptions.dontStartMoca != 1) {
       moca_set_start(ctx);
    }

    return(CMSRET_SUCCESS);
}

CmsRet MoCACtl_Initialize( PMoCA_INITIALIZATION_PARMS pMoCAInitParms,
    PMoCA_CONFIG_PARAMS pCfgParams, unsigned long long configMask)
{
    CmsRet ret;

    ret = do_open();
    if (ret != CMSRET_SUCCESS) return(ret);

    return(MoCACtl2_Initialize( vctx, pMoCAInitParms, pCfgParams, configMask));
}

CmsRet MoCACtl2_Uninitialize(void * ctx)
{
    moca_set_stop(ctx);

    return(CMSRET_SUCCESS);
}

CmsRet MoCACtl_Uninitialize(void)
{
    CmsRet ret;

    ret = do_open();
    if (ret != CMSRET_SUCCESS) return(ret);

    return(MoCACtl2_Uninitialize(vctx));
}

CmsRet MoCACtl2_ReInitialize( 
    void * ctx,
    PMoCA_INITIALIZATION_PARMS pMoCAInitParms, 
    UINT64 reInitMask,
    PMoCA_CONFIG_PARAMS pCfgParams, 
    unsigned long long configMask)
{
    int ret;
    //struct moca_mc_fwd_rd mcfwd_tbl[MoCA_MAX_MC_FWD_ENTRIES];
    //int mcfwd_tbl_size = 0;

    moca_set_stop(ctx);

    ret = MoCACtl2_SetInitParms(ctx, pMoCAInitParms, reInitMask);
    if (ret != CMSRET_SUCCESS)
        return(CMSRET_INVALID_ARGUMENTS);
      
    ret = MoCACtl2_SetCfg( ctx, pCfgParams, configMask );
    if (ret != CMSRET_SUCCESS)
        return(CMSRET_INVALID_ARGUMENTS);

    if(!((reInitMask & MoCA_INIT_PARAM_OPTIONS_MASK) &&
         (pMoCAInitParms->initOptions.dontStartMoca == 1))) 
    {
       moca_set_start(ctx);
    }
   return(CMSRET_SUCCESS);
}

CmsRet MoCACtl_ReInitialize( PMoCA_INITIALIZATION_PARMS pMoCAInitParms, UINT64 reInitMask,
    PMoCA_CONFIG_PARAMS pCfgParams, unsigned long long configMask)
{
    CmsRet ret;
    
    ret = do_open();
    if (ret != CMSRET_SUCCESS) return(ret);

    return( MoCACtl2_ReInitialize( vctx, pMoCAInitParms, reInitMask,
        pCfgParams, configMask));
}

CmsRet MoCACtl2_GetInitParms(
     void * ctx,
     PMoCA_INITIALIZATION_PARMS pMoCAInitParms)
{
    struct moca_init_time par;
    struct moca_password pw;
    struct moca_gen_status gs;
    struct moca_init_time_options opt;
    uint32_t * pu32;
    int i;

    moca_get_init_time(ctx, &par);
    moca_get_password(ctx, &pw);
    moca_get_gen_status(ctx, &gs);
    moca_get_init_time_options(ctx, &opt);
    
    memset(pMoCAInitParms, 0, sizeof(*pMoCAInitParms));

    pMoCAInitParms->ncMode = (MoCA_NC_MODE)par.nc_mode;
    pMoCAInitParms->autoNetworkSearchEn = par.auto_network_search_en;
    pMoCAInitParms->privacyEn = par.privacy_en;
    pMoCAInitParms->txPwrControlEn = par.tpc_en;
    pMoCAInitParms->constTransmitMode = opt.const_tx_mode;
    pMoCAInitParms->nvParams.lastOperFreq = par.lof;
    pMoCAInitParms->maxTxPowerBeacons = par.max_tx_power_beacons;
    pMoCAInitParms->maxTxPowerPackets = par.max_tx_power_packets;    
    pMoCAInitParms->boMode = par.bo_mode;   
    pMoCAInitParms->mrNonDefSeqNum = par.mr_non_def_seq_num;
    pMoCAInitParms->rfType = par.rf_type;        
    pMoCAInitParms->mocaLoopbackEn = par.moca_loopback_en;    
    pMoCAInitParms->ledMode = par.led_settings;
    pMoCAInitParms->freqMask = par.freq_mask;    
    pMoCAInitParms->pnsFreqMask = par.pns_freq_mask;        
    pMoCAInitParms->otfEn = par.otf_en;
    pMoCAInitParms->flowControlEn = par.flow_control_en;
    pMoCAInitParms->mtmEn = par.mtm_en;
    pMoCAInitParms->turboEn = par.turbo_en;
    pMoCAInitParms->mcastMode = par.multicast_mode;
    pMoCAInitParms->labMode = par.lab_mode;
    pMoCAInitParms->tabooFixedMaskStart = par.taboo_fixed_mask_start;
    pMoCAInitParms->tabooFixedChannelMask = par.taboo_fixed_channel_mask;
    pMoCAInitParms->tabooLeftMask = par.taboo_left_mask;
    pMoCAInitParms->tabooRightMask = par.taboo_right_mask;    
    pMoCAInitParms->padPower = par.pad_power;
    pMoCAInitParms->terminalIntermediateType = par.terminal_intermediate_device;
    pMoCAInitParms->preferedNC = par.preferred_nc;
    pMoCAInitParms->beaconPwrReductionEn = par.beacon_pwr_reduction_en;
    pMoCAInitParms->beaconPwrReduction = par.beacon_pwr_reduction;
    pMoCAInitParms->lowPriQNum = par.low_pri_q_num;
    pMoCAInitParms->beaconChannel = par.beacon_channel;
    pMoCAInitParms->reservedInit1 = par.reserved_init_1;    
    pMoCAInitParms->reservedInit2 = par.reserved_init_2;    
    pMoCAInitParms->reservedInit3 = par.reserved_init_3;    
    pMoCAInitParms->reservedInit4 = par.reserved_init_4;    
    pMoCAInitParms->reservedInit5 = par.reserved_init_5;        
    pMoCAInitParms->qam256Capability = par.qam256_capability;        
    pMoCAInitParms->continuousRxModeAttn = par.continuous_rx_mode_attn;
    pMoCAInitParms->egrMcFilterEn = par.egr_mc_filter_en;    
    pMoCAInitParms->lowPriQNum = par.low_pri_q_num;
    pMoCAInitParms->operatingVersion = gs.self_moca_version;
    strcpy((char *)pMoCAInitParms->password, (const char *)pw.password);
    pMoCAInitParms->passwordSize = (UINT32)strlen((const char *)pw.password);
    pMoCAInitParms->initOptions.constTxSubCarrier1 = opt.const_tx_sc1;
    pMoCAInitParms->initOptions.constTxSubCarrier2 = opt.const_tx_sc2;

    pu32 = &opt.const_tx_band0;
    for (i = 0; i < MoCA_CONTINUOUS_TX_BAND_ARRAY_SIZE; i++)
    {
        pMoCAInitParms->initOptions.constTxNoiseBand[i] = *pu32;
        pu32++;
    }
    pMoCAInitParms->initOptions.dontStartMoca = opt.dont_start_moca;

    return(CMSRET_SUCCESS);
}

CmsRet MoCACtl_GetInitParms(
     PMoCA_INITIALIZATION_PARMS pMoCAInitParms)
{
    CmsRet ret;

    ret = do_open();
    if (ret != CMSRET_SUCCESS) return(ret);

    return(MoCACtl2_GetInitParms( vctx, pMoCAInitParms));
}

CmsRet MoCACtl2_SetInitParms( 
    void * ctx,
    PMoCA_INITIALIZATION_PARMS pMoCAInitParms, 
    UINT64 initMask)
{
    int ret;
    struct moca_init_time par;
    int i;
    struct moca_init_time_options options;
    uint32_t *pu32;

    moca_get_init_time(ctx, &par);
    moca_get_init_time_options(ctx, &options);

    if(initMask & MoCA_INIT_PARAM_NC_MODE_MASK)
        par.nc_mode = pMoCAInitParms->ncMode;
    if(initMask & MoCA_INIT_PARAM_AUTO_NETWORK_SEARCH_EN_MASK)
        par.auto_network_search_en = pMoCAInitParms->autoNetworkSearchEn;
    if(initMask & MoCA_INIT_PARAM_PRIVACY_MASK)
        par.privacy_en = pMoCAInitParms->privacyEn;
    if(initMask & MoCA_INIT_PARAM_TX_PWR_CONTROL_EN_MASK)
        par.tpc_en = pMoCAInitParms->txPwrControlEn;
    if(initMask & MoCA_INIT_PARAM_CONST_TRANSMIT_MODE_MASK) {
        par.continuous_power_tx_mode = pMoCAInitParms->constTransmitMode;
        options.const_tx_mode = pMoCAInitParms->constTransmitMode;
        switch (par.continuous_power_tx_mode)
        {
            /* These modes are all the same for the core. They
             * require some extra settings from the host */
            case MoCA_CONTINUOUS_TX_CW:
            case MoCA_CONTINUOUS_TX_TONE:
            case MoCA_CONTINUOUS_TX_TONE_SC:
            case MoCA_CONTINUOUS_TX_DUAL_TONE_SC:
            case MoCA_CONTINUOUS_TX_BAND:
                par.continuous_power_tx_mode = MoCA_CONTINUOUS_TX_PROBE_I;
                break;
            default:
                /* do nothing */
                break;
        }
    }
    if(initMask & MoCA_INIT_PARAM_NV_PARAMS_LOF_MASK)
        par.lof = pMoCAInitParms->nvParams.lastOperFreq;
    if(initMask & MoCA_INIT_PARAM_MAX_TX_POWER_PACKETS_MASK)
        par.max_tx_power_packets = pMoCAInitParms->maxTxPowerPackets;
    if(initMask & MoCA_INIT_PARAM_BO_MODE_MASK)
        par.bo_mode = pMoCAInitParms->boMode;    
    if(initMask & MoCA_INIT_PARAM_MR_NON_DEF_SEQ_NUM_MASK)
        par.mr_non_def_seq_num = pMoCAInitParms->mrNonDefSeqNum;        
    if(initMask & MoCA_INIT_PARAM_RF_TYPE_MASK)
        par.rf_type = pMoCAInitParms->rfType;    
    if(initMask & MoCA_INIT_PARAM_MoCA_LOOPBACK_EN_MASK)
        par.moca_loopback_en = pMoCAInitParms->mocaLoopbackEn;
    if(initMask & MoCA_INIT_PARAM_LED_MODE_MASK)        
        par.led_settings = pMoCAInitParms->ledMode;    
    if(initMask & MoCA_INIT_PARAM_FREQ_MASK_MASK)        
        par.freq_mask = pMoCAInitParms->freqMask;     
    if(initMask & MoCA_INIT_PARAM_PNS_FREQ_MASK_MASK) 
        par.pns_freq_mask = pMoCAInitParms->pnsFreqMask;       
    if(initMask & MoCA_INIT_PARAM_OTF_EN_MASK)        
        par.otf_en = pMoCAInitParms->otfEn;       
    if(initMask & MoCA_INIT_PARAM_FLOW_CONTROL_EN_MASK)        
        par.flow_control_en = pMoCAInitParms->flowControlEn;         
    if(initMask & MoCA_INIT_PARAM_MTM_EN_MASK)        
        par.mtm_en = pMoCAInitParms->mtmEn;
    if(initMask & MoCA_INIT_PARAM_TURBO_EN_MASK)        
        par.turbo_en = pMoCAInitParms->turboEn;               
    if(initMask & MoCA_INIT_PARAM_MAX_TX_POWER_BEACONS_MASK)     
        par.max_tx_power_beacons = pMoCAInitParms->maxTxPowerBeacons;    
    if(initMask & MoCA_INIT_PARAM_MCAST_MODE_MASK)
        par.multicast_mode = pMoCAInitParms->mcastMode;
    if(initMask & MoCA_INIT_PARAM_LAB_MODE_MASK)
        par.lab_mode = pMoCAInitParms->labMode;
    if(initMask & MoCA_INIT_PARAM_TABOO_FIXED_MASK_START_MASK)
        par.taboo_fixed_mask_start = pMoCAInitParms->tabooFixedMaskStart;
    if(initMask & MoCA_INIT_PARAM_TABOO_FIXED_CHANNEL_MASK_MASK)
        par.taboo_fixed_channel_mask = pMoCAInitParms->tabooFixedChannelMask;
    if(initMask & MoCA_INIT_PARAM_TABOO_LEFT_MASK_MASK)
        par.taboo_left_mask = pMoCAInitParms->tabooLeftMask;
    if(initMask & MoCA_INIT_PARAM_TABOO_RIGHT_MASK_MASK)
        par.taboo_right_mask = pMoCAInitParms->tabooRightMask;
    if(initMask & MoCA_INIT_PARAM_PAD_POWER_MASK)
        par.pad_power = pMoCAInitParms->padPower;
    if(initMask & MoCA_INIT_PARAM_TERMINAL_INTERMEDIATE_TYPE_MASK)
        par.terminal_intermediate_device = pMoCAInitParms->terminalIntermediateType;
    if(initMask & MoCA_INIT_PARAM_PREFERED_NC_MASK)
        par.preferred_nc = pMoCAInitParms->preferedNC;
    if (initMask & MoCA_INIT_PARAM_EGR_MC_FILTER_EN_MASK)
        par.egr_mc_filter_en = pMoCAInitParms->egrMcFilterEn;
    if (initMask & MoCA_INIT_PARAM_BEACON_PWR_REDUCTION_EN_MASK)
        par.beacon_pwr_reduction_en = pMoCAInitParms->beaconPwrReductionEn;
    if (initMask & MoCA_INIT_PARAM_BEACON_PWR_REDUCTION_MASK)
        par.beacon_pwr_reduction = pMoCAInitParms->beaconPwrReduction;
    if (initMask & MoCA_INIT_PARAM_LOW_PRI_Q_NUM_MASK)
        par.low_pri_q_num = pMoCAInitParms->lowPriQNum;        
    if(initMask & MoCA_INIT_PARAM_QAM256_CAPABILITY_MASK)
        par.qam256_capability = pMoCAInitParms->qam256Capability;     
    if(initMask & MoCA_INIT_PARAM_CONTINUOUS_RX_MODE_ATTN_MASK)
        par.continuous_rx_mode_attn = pMoCAInitParms->continuousRxModeAttn;      
    if(initMask & MoCA_INIT_PARAM_BEACON_CHANNEL_MASK)
        par.beacon_channel = pMoCAInitParms->beaconChannel;
    if(initMask & MoCA_INIT_PARAM_RESERVED_INIT_1_MASK)
        par.reserved_init_1 = pMoCAInitParms->reservedInit1;    
    if(initMask & MoCA_INIT_PARAM_RESERVED_INIT_2_MASK)
        par.reserved_init_2 = pMoCAInitParms->reservedInit2;    
    if(initMask & MoCA_INIT_PARAM_RESERVED_INIT_3_MASK)
        par.reserved_init_3 = pMoCAInitParms->reservedInit3;    
    if(initMask & MoCA_INIT_PARAM_RESERVED_INIT_4_MASK)
        par.reserved_init_4 = pMoCAInitParms->reservedInit4;    
    if(initMask & MoCA_INIT_PARAM_RESERVED_INIT_5_MASK)
        par.reserved_init_5 = pMoCAInitParms->reservedInit5;    

    if (moca_validate_init_params(&par) != 0)
        return (CMSRET_INVALID_ARGUMENTS);

    moca_set_init_time(ctx, &par);

    if(initMask & MoCA_INIT_PARAM_OPTIONS_MASK) {
        options.const_tx_sc1  = pMoCAInitParms->initOptions.constTxSubCarrier1;
        options.const_tx_sc2  = pMoCAInitParms->initOptions.constTxSubCarrier2;


        pu32 = &options.const_tx_band0;
        for (i = 0; i < MoCA_CONTINUOUS_TX_BAND_ARRAY_SIZE; i++)
        {
            *pu32 = pMoCAInitParms->initOptions.constTxNoiseBand[i];
            pu32++;
        }
        options.dont_start_moca = pMoCAInitParms->initOptions.dontStartMoca;
    }

    if (moca_validate_init_options(&options) != 0)
        return (CMSRET_INVALID_ARGUMENTS);
    
    moca_set_init_time_options(ctx, &options);

    if((initMask & MoCA_INIT_PARAM_PASSWORD_MASK) &&
       (pMoCAInitParms->passwordSize > 0)) {
        struct moca_password pw;

        if ((pMoCAInitParms->passwordSize < MoCA_MIN_PASSWORD_LEN) ||
             (pMoCAInitParms->passwordSize > MoCA_MAX_PASSWORD_LEN)) {
            printf("BcmMoCAHost: Invalid password size %d \n", pMoCAInitParms->passwordSize) ;
            return(CMSRET_INVALID_PARAM_VALUE);
        }

        /* The password must be all decimal characters */
        for ( i = 0 ; i < pMoCAInitParms->passwordSize ; i++ ) {
            if ( (pMoCAInitParms->password [i] < '0') || (pMoCAInitParms->password [i] > '9') ) {
                printf( "BcmMoCAHost: Invalid password %s \n", pMoCAInitParms->password) ;
                return(CMSRET_INVALID_PARAM_VALUE);
            }
        }

        memset(pw.password, 0, sizeof(pw.password));
        memcpy(pw.password, pMoCAInitParms->password,
            pMoCAInitParms->passwordSize);
        ret = moca_set_password(ctx, &pw);
        if (ret)
            return(CMSRET_INVALID_ARGUMENTS);
    }

#if defined(DSL_MOCA) /* DSL Code */
    if (initMask & MoCA_INIT_PARAM_OPERATING_VERSION_MASK)
    {
        struct moca_fw_file fw;

        sprintf((char *)fw.fw_file, "/etc/moca/moca1%ucore.bin", 
            (pMoCAInitParms->operatingVersion == MoCA_VERSION_10 ? 0 : 1));

        ret = moca_set_fw_file(ctx, &fw);

        if (ret)
            return(CMSRET_INVALID_ARGUMENTS);
    }
#endif
    
    return(CMSRET_SUCCESS);
}
static int get_snr_margin(void * ctx, PMoCA_CONFIG_PARAMS p)
{
    int i;
    struct moca_snr_margin_table arg;
    double base_snr;
    int ret;

    ret = moca_get_snr_margin_table(ctx, &arg);
    if (ret != 0) return(ret);

    memcpy(&p->snrMarginTable, &arg, sizeof(p->snrMarginTable));

    base_snr = (arg.mgntable[10])/2.0;

    p->snrMargin = base_snr* 2.0;       
  
    for(i = 0; i < SNR_ELEMENTS; i++) {
        double t = arg.mgntable[i]/2.0;
        
        p->snrMarginOffset[i] = (int) ((t*2 - base_snr*2 - snr_ref[i]));
    }

    return(0);
}

CmsRet MoCACtl2_GetCfg(
     void * ctx, PMoCA_CONFIG_PARAMS p, unsigned long long configMask)
{
    int ret;
    uint32_t moca_reg;
    uint32_t const_tx_mode;
    uint32_t i;
    struct moca_gen_status gs;
    uint32_t max_nodes;
    unsigned long long m = configMask;

    moca_reg = p->RegMem.input;
    
    memset(p, 0, sizeof(*p));
    memset(&gs, 0, sizeof(gs));
    
    p->RegMem.input = moca_reg;
    
    ret = moca_get_moca_const_tx_mode(ctx, &const_tx_mode);
    if (ret != 0) return(CMSRET_INTERNAL_ERROR);
     
    if (moca_get_gen_status(ctx, &gs) != 0)
        gs.self_moca_version = MoCA_VERSION_11;
        
    if(m & MoCA_CFG_PARAM_MAX_FRAME_SIZE_MASK)
        ret = moca_get_max_frame_size(ctx, &p->maxFrameSize);
    if (ret != 0) return(CMSRET_INTERNAL_ERROR);
    
    if(m & MoCA_CFG_PARAM_MAX_TRANS_TIME_MASK)
        ret = moca_get_max_transmit_time(ctx, &p->maxTransmitTime);
    if (ret != 0) return(CMSRET_INTERNAL_ERROR);
    
    if(m & MoCA_CFG_PARAM_MIN_BW_ALARM_THRE_MASK)
        ret = moca_get_min_bw_alarm_threshold(ctx, &p->minBwAlarmThreshold);
    if (ret != 0) return(CMSRET_INTERNAL_ERROR);

    if(m & MoCA_CFG_PARAM_CONT_IE_RR_INS_MASK)
        ret = moca_get_continuous_ie_rr_insert(ctx, &p->continuousIERRInsert);
    if (ret != 0) return(CMSRET_INTERNAL_ERROR);
    
    if(m & MoCA_CFG_PARAM_CONT_IE_MAP_INS_MASK)
        ret = moca_get_continuous_ie_map_insert(ctx, &p->continuousIEMapInsert);
    if (ret != 0) return(CMSRET_INTERNAL_ERROR);
    
    if(m & MoCA_CFG_PARAM_MAX_PKT_AGGR_MASK)
        ret = moca_get_max_pkt_aggr(ctx, &p->maxPktAggr);
    if (ret != 0) return(CMSRET_INTERNAL_ERROR);
    
    if(m & MoCA_CFG_PARAM_MAX_CONSTELLATION_MASK) {
        max_nodes = (gs.self_moca_version == MoCA_VERSION_10) ?
            MoCA_MAX_NODES_1_0 : MoCA_MAX_NODES;
        for (i = 0; i < max_nodes; i++) {
            ret = moca_get_max_constellation(ctx, i, &p->constellation[i]);
            if (ret != 0) return(CMSRET_INTERNAL_ERROR);
        }
    }
    
    if(m & MoCA_CFG_PARAM_FREQ_SHIFT_MASK)
        ret = moca_get_freq_shift(ctx, &p->freqShiftMode);
    if (ret != 0) return(CMSRET_INTERNAL_ERROR);
    
    if(m & MoCA_CFG_PARAM_PMK_EXCHG_INTVL_MASK)
    {
        ret = moca_get_pmk_exchange_interval(ctx,
            &p->pmkExchangeInterval);
        if (ret != 0) return(CMSRET_INTERNAL_ERROR);
        p->pmkExchangeInterval /= 3600 * 1000;        
    }

    if(m & MoCA_CFG_PARAM_TEK_EXCHG_INTVL_MASK)
    {
        ret = moca_get_tek_exchange_interval(ctx,
            &p->tekExchangeInterval);
        if (ret != 0) return(CMSRET_INTERNAL_ERROR);
        p->tekExchangeInterval /= 60 * 1000;
    }

    if(m & MoCA_CFG_PARAM_PRIO_ALLOCATIONS_MASK)
        ret = moca_get_priority_allocations(ctx, (struct moca_priority_allocations *)&p->prioAllocation);
    if (ret != 0) return(CMSRET_INTERNAL_ERROR);

    if(m & MoCA_CFG_PARAM_SAPM_EN_MASK)
        ret = moca_get_sapm_en(ctx, &p->sapmEn); 
    if (ret != 0) return(CMSRET_INTERNAL_ERROR);
    
    if(m & MoCA_CFG_PARAM_ARPL_TH_MASK)
        ret = moca_get_arpl_th(ctx, &p->arplTh);    
    if (ret != 0) return(CMSRET_INTERNAL_ERROR);
    
    if(m & MoCA_CFG_PARAM_SAPM_TABLE_MASK)
        ret = moca_get_sapm_table(ctx, (struct moca_sapm_table *)&p->sapmTable);    
    if (ret != 0) return(CMSRET_INTERNAL_ERROR);

    if(m & MoCA_CFG_PARAM_RLAPM_EN_MASK)
        ret = moca_get_rlapm_en(ctx, &p->rlapmEn);
    if (ret != 0) return(CMSRET_INTERNAL_ERROR);
    
    if(m & MoCA_CFG_PARAM_RLAPM_TABLE_MASK)
        ret = moca_get_rlapm_table(ctx, (struct moca_rlapm_table *)p->rlapmTable);
    if (ret != 0) return(CMSRET_INTERNAL_ERROR);
                
    if((m & MoCA_CFG_PARAM_SNR_MARGIN_MASK) || (m & MoCA_CFG_PARAM_SNR_MARGIN_OFFSET_MASK))
        ret = get_snr_margin(ctx, p);
    if (ret != 0) return(CMSRET_INTERNAL_ERROR);
        
    if(m & MoCA_CFG_PARAM_EGR_MC_ADDR_FILTER_MASK) {
        for (i = 0; i < MOCA_MAX_EGR_MC_FILTERS; i++) {
            ret = moca_get_egr_mc_addr_filter_get(ctx, i, (struct moca_egr_mc_addr_filter_get *)&p->mcAddrFilter[i]);

            if (ret != 0) return(CMSRET_INTERNAL_ERROR);
        }
    }
        
    if(m & MoCA_CFG_PARAM_RX_POWER_TUNING_MASK)
        moca_get_rx_power_tuning(ctx, &p->rxPowerTuning);
    if (ret != 0) return(CMSRET_INTERNAL_ERROR);

    if(m & MoCA_CFG_PARAM_EN_CAPABLE_MASK)
        moca_get_en_capable(ctx, &p->enCapable);
    if (ret != 0) return(CMSRET_INTERNAL_ERROR);
        
    if(m & MoCA_CFG_PARAM_MIN_MAP_CYCLE_MASK)
        ret = moca_get_min_map_cycle(ctx, &p->minMapCycle);
    if (ret != 0) return(CMSRET_INTERNAL_ERROR);
    
    if(m & MoCA_CFG_PARAM_MAX_MAP_CYCLE_MASK)
        ret = moca_get_max_map_cycle(ctx, &p->maxMapCycle);
    if (ret != 0) return(CMSRET_INTERNAL_ERROR);

    if(m & MoCA_CFG_PARAM_EXTRA_RX_PACKETS_PER_QM_MASK)
        ret = moca_get_extra_rx_packets_per_qm(ctx, &p->extraRxPacketsPerQM);
    if (ret != 0) return(CMSRET_INTERNAL_ERROR);

    if(m & MoCA_CFG_PARAM_RX_TX_PACKETS_PER_QM_MASK)
        ret = moca_get_rx_tx_packets_per_qm(ctx, &p->rxTxPacketsPerQM);
    if (ret != 0) return(CMSRET_INTERNAL_ERROR);
    
    if(m & MoCA_CFG_PARAM_TARGET_PHY_RATE_QAM128_MASK)
        ret = moca_get_target_phy_rate_qam128(ctx, &p->targetPhyRateQAM128);
    if (ret != 0) return(CMSRET_INTERNAL_ERROR);
    
    if(m & MoCA_CFG_PARAM_TARGET_PHY_RATE_QAM256_MASK)
        ret = moca_get_target_phy_rate_qam256(ctx, &p->targetPhyRateQAM256);
    if (ret != 0) return(CMSRET_INTERNAL_ERROR);

    if(m & MoCA_CFG_PARAM_TARGET_PHY_RATE_TURBO_MASK)
        ret = moca_get_target_phy_rate_turbo(ctx, &p->targetPhyRateTurbo);
    if (ret != 0) return(CMSRET_INTERNAL_ERROR);

    if(m & MoCA_CFG_PARAM_TARGET_PHY_RATE_TURBO_PLUS_MASK)
        ret = moca_get_target_phy_rate_turbo_plus(ctx, &p->targetPhyRateTurboPlus);
    if (ret != 0) return(CMSRET_INTERNAL_ERROR);        

    if(m & MoCA_CFG_PARAM_NBAS_CAPPING_EN_MASK)
        ret = moca_get_nbas_capping_en(ctx, &p->nbasCappingEn);
    if (ret != 0) return(CMSRET_INTERNAL_ERROR);        

    if(m & MoCA_CFG_PARAM_SELECTIVE_RR_MASK)
        ret = moca_get_selective_rr(ctx, &p->selectiveRR);
    if (ret != 0) return(CMSRET_INTERNAL_ERROR);        

    if(m & MoCA_CFG_PARAM_PSS_EN_MASK)
        ret = moca_get_pss_en(ctx, &p->pssEn);
    if (ret != 0) return(CMSRET_INTERNAL_ERROR);

    if(m & MoCA_CFG_PARAM_RES_1_MASK)
        ret = moca_get_config_reserved_1(ctx, &p->res1);
    if (ret != 0) return(CMSRET_INTERNAL_ERROR);

    if(m & MoCA_CFG_PARAM_RES_2_MASK)
        ret = moca_get_config_reserved_2(ctx, &p->res2);
    if (ret != 0) return(CMSRET_INTERNAL_ERROR);

    if(m & MoCA_CFG_PARAM_RES_3_MASK)
        ret = moca_get_config_reserved_3(ctx, &p->res3);
    if (ret != 0) return(CMSRET_INTERNAL_ERROR);

    if(m & MoCA_CFG_PARAM_MIN_AGGR_WAIT_TIME_MASK)
        ret = moca_get_min_aggr_waiting_time(ctx, &p->minAggrWaitTime);
    if (ret != 0) return(CMSRET_INTERNAL_ERROR);
    
    if(m & MoCA_CFG_PARAM_DIPLEXER_MASK)
        ret = moca_get_diplexer(ctx, &p->diplexer);
    if (ret != 0) return(CMSRET_INTERNAL_ERROR);

    if(m & MoCA_CFG_PARAM_EN_MAX_RATE_IN_MAX_BO_MASK)
        ret = moca_get_en_max_rate_in_max_bo(ctx, &p->enMaxRateInMaxBo);
    if (ret != 0) return(CMSRET_INTERNAL_ERROR);

    /* moca register access is allowed in constant tx mode */
    if ((p->RegMem.input != 0) && ( m & MoCA_CFG_PARAM_LAB_REG_MEM_MASK )) {
        ret = moca_get_lab_register(ctx, p->RegMem.input, p->RegMem.value);
        if (ret != 0) return(CMSRET_INTERNAL_ERROR);
    }

    return(CMSRET_SUCCESS);
}

CmsRet MoCACtl_GetCfg(
     PMoCA_CONFIG_PARAMS p, unsigned long long configMask)
{
    CmsRet ret; 

    ret = do_open();
    if (ret != CMSRET_SUCCESS) return(ret);

    return(MoCACtl2_GetCfg(vctx, p, configMask));
}
CmsRet MoCACtl2_SetCfg(
     void * ctx, PMoCA_CONFIG_PARAMS p, unsigned long long m)
{
    int ret;
    struct moca_max_constellation mc;
    int i;
    struct moca_gen_status gs;
    uint32_t max_nodes;

    if (moca_validate_anytime_params(ctx, p, m) != 0)
        return (CMSRET_INVALID_ARGUMENTS);
    
    if(m & MoCA_CFG_PARAM_MAX_FRAME_SIZE_MASK)
        ret = moca_set_max_frame_size(ctx, p->maxFrameSize);
    if(m & MoCA_CFG_PARAM_MAX_TRANS_TIME_MASK)
        moca_set_max_transmit_time(ctx, p->maxTransmitTime);
    if(m & MoCA_CFG_PARAM_MIN_BW_ALARM_THRE_MASK)
        moca_set_min_bw_alarm_threshold(ctx, p->minBwAlarmThreshold);
    if(m & MoCA_CFG_PARAM_OUT_OF_ORDER_LMO_MASK)
        moca_set_ooo_lmo(ctx, p->outOfOrderLmo);
    if(m & MoCA_CFG_PARAM_CONT_IE_RR_INS_MASK)
        moca_set_continuous_ie_rr_insert(ctx, p->continuousIERRInsert);
    if(m & MoCA_CFG_PARAM_CONT_IE_MAP_INS_MASK)
        moca_set_continuous_ie_map_insert(ctx, p->continuousIEMapInsert);
    if(m & MoCA_CFG_PARAM_MAX_PKT_AGGR_MASK)
        moca_set_max_pkt_aggr(ctx, p->maxPktAggr);
    if(m & MoCA_CFG_PARAM_MAX_CONSTELLATION_MASK) {
        memset(&gs, 0, sizeof(gs));
        moca_get_gen_status(ctx, &gs);

        max_nodes = (gs.self_moca_version == MoCA_VERSION_10) ? 
            MoCA_MAX_NODES_1_0 : MoCA_MAX_NODES;
        for (i = 0; i < max_nodes; i++)
        {
            if (p->constellation[i] != 0)
            {
                mc.node_id = i;
                mc.bits_per_carrier = p->constellation[i];
                moca_set_max_constellation(ctx, &mc);
            }
        }
    }

    if(m & MoCA_CFG_PARAM_EGR_MC_ADDR_FILTER_MASK) {
       
        for (i = 0; i < MOCA_MAX_EGR_MC_FILTERS; i++)
        {
            p->mcAddrFilter[i].EntryId = i;
            moca_set_egr_mc_addr_filter(ctx, (struct moca_egr_mc_addr_filter *)&p->mcAddrFilter[i]);
        }
    }

    if(m & MoCA_CFG_PARAM_EN_CAPABLE_MASK)
        moca_set_en_capable(ctx, p->enCapable);
    if(m & MoCA_CFG_PARAM_FREQ_SHIFT_MASK)
        moca_set_freq_shift(ctx, p->freqShiftMode);
    if(m & MoCA_CFG_PARAM_PMK_EXCHG_INTVL_MASK)
        moca_set_pmk_exchange_interval(ctx,
            p->pmkExchangeInterval * 3600 * 1000);
    if(m & MoCA_CFG_PARAM_TEK_EXCHG_INTVL_MASK)
        moca_set_tek_exchange_interval(ctx,
            p->tekExchangeInterval * 60 * 1000);
    if(m & MoCA_CFG_PARAM_PRIO_ALLOCATIONS_MASK)
        moca_set_priority_allocations(ctx, (struct moca_priority_allocations *)&p->prioAllocation);

    if(m & MoCA_CFG_PARAM_SAPM_EN_MASK)
        ret = moca_set_sapm_en(ctx, p->sapmEn);    
    if(m & MoCA_CFG_PARAM_ARPL_TH_MASK)
        ret = moca_set_arpl_th(ctx, p->arplTh);    
    if(m & MoCA_CFG_PARAM_SAPM_TABLE_MASK)
        ret = moca_set_sapm_table(ctx, (struct moca_sapm_table *)&p->sapmTable);    
    if(m & MoCA_CFG_PARAM_RLAPM_EN_MASK)
        ret = moca_set_rlapm_en(ctx, p->rlapmEn);
    if(m & MoCA_CFG_PARAM_RLAPM_TABLE_MASK)
        ret = moca_set_rlapm_table(ctx, (struct moca_rlapm_table *)p->rlapmTable);

    if((m & MoCA_CFG_PARAM_SNR_MARGIN_MASK) || (m & MoCA_CFG_PARAM_SNR_MARGIN_OFFSET_MASK)) {
        int i;
        struct moca_snr_margin_table arg;

        for(i = 0; i < SNR_ELEMENTS; i++) {
            arg.mgntable[i] = snr_ref[i] + p->snrMargin + p->snrMarginOffset[i];
        }

        arg.mgntable[10] = p->snrMargin;
        
        moca_set_snr_margin_table(ctx, &arg);
    }

    if(m & MoCA_CFG_PARAM_MIN_MAP_CYCLE_MASK)
        moca_set_min_map_cycle(ctx, p->minMapCycle);
    if(m & MoCA_CFG_PARAM_MAX_MAP_CYCLE_MASK)
        moca_set_max_map_cycle(ctx, p->maxMapCycle);
    if(m & MoCA_CFG_PARAM_EXTRA_RX_PACKETS_PER_QM_MASK)
        moca_set_extra_rx_packets_per_qm(ctx, p->extraRxPacketsPerQM);
    if(m & MoCA_CFG_PARAM_RX_TX_PACKETS_PER_QM_MASK)
        moca_set_rx_tx_packets_per_qm(ctx, p->rxTxPacketsPerQM);
    if(m & MoCA_CFG_PARAM_TARGET_PHY_RATE_QAM128_MASK)
        moca_set_target_phy_rate_qam128(ctx, p->targetPhyRateQAM128);
    if(m & MoCA_CFG_PARAM_TARGET_PHY_RATE_QAM256_MASK)
        moca_set_target_phy_rate_qam256(ctx, p->targetPhyRateQAM256);
    if(m & MoCA_CFG_PARAM_TARGET_PHY_RATE_TURBO_MASK)
        moca_set_target_phy_rate_turbo(ctx, p->targetPhyRateTurbo);
    if(m & MoCA_CFG_PARAM_TARGET_PHY_RATE_TURBO_PLUS_MASK)
        moca_set_target_phy_rate_turbo_plus(ctx, p->targetPhyRateTurboPlus);
    if(m & MoCA_CFG_PARAM_NBAS_CAPPING_EN_MASK)
        moca_set_nbas_capping_en(ctx, p->nbasCappingEn);    
    if(m & MoCA_CFG_PARAM_SELECTIVE_RR_MASK)
        moca_set_selective_rr(ctx, p->selectiveRR);
    if(m & MoCA_CFG_PARAM_PSS_EN_MASK)
        moca_set_pss_en(ctx, p->pssEn);            
    if(m & MoCA_CFG_PARAM_IQ_DIAGRAM_SET_MASK)
        moca_set_lab_iq_diagram_set(ctx, (struct moca_lab_iq_diagram_set *)&p->iq);
    if(m & MoCA_CFG_PARAM_SNR_GRAPH_SET_MASK)
        moca_set_lab_snr_graph_set(ctx, p->MoCASnrGraphSetNodeId);
    if(m & MoCA_CFG_PARAM_LAB_REG_MEM_MASK)
        moca_set_lab_register(ctx, (struct moca_lab_register *)&p->RegMem);
    if(m & MoCA_CFG_PARAM_LAB_CALL_FUNC_MASK)
        moca_set_lab_call_func(ctx, (const struct moca_lab_call_func *)&p->callFunc);
    if(m & MoCA_CFG_PARAM_LAB_TPCAP_MASK)
        moca_set_lab_tpcap(ctx, (void *)&p->labTPCAP);
    if(m & MoCA_CFG_PARAM_RES_1_MASK)
        moca_set_config_reserved_1(ctx, p->res1);
    if(m & MoCA_CFG_PARAM_RES_2_MASK)
        moca_set_config_reserved_2(ctx, p->res2);
    if(m & MoCA_CFG_PARAM_RES_3_MASK)
        moca_set_config_reserved_3(ctx, p->res3);
    if(m & MoCA_CFG_PARAM_MIN_AGGR_WAIT_TIME_MASK)
        moca_set_min_aggr_waiting_time(ctx, p->minAggrWaitTime);    
    if(m & MoCA_CFG_PARAM_RX_POWER_TUNING_MASK)
        moca_set_rx_power_tuning(ctx, p->rxPowerTuning);
    if(m & MoCA_CFG_PARAM_DIPLEXER_MASK)
        moca_set_diplexer(ctx, p->diplexer);
    if(m & MoCA_CFG_PARAM_EN_MAX_RATE_IN_MAX_BO_MASK)
        moca_set_en_max_rate_in_max_bo(ctx, p->enMaxRateInMaxBo);

    return(CMSRET_SUCCESS);
}

CmsRet MoCACtl_SetCfg(
     PMoCA_CONFIG_PARAMS p, unsigned long long m)
{
    CmsRet ret;

    ret = do_open();
    if (ret != CMSRET_SUCCESS) return(ret);

    return(MoCACtl2_SetCfg(vctx, p, m));
}

CmsRet MoCACtl2_GetUcFwdTbl(
        void *  ctx,
        PMoCA_UC_FWD_ENTRY pMoCAUcFwdParamsTable,
        UINT32  *pulUcFwdTblSize)
{
    int ret;

    ret = moca_get_uc_fwd(ctx, (struct moca_uc_fwd *)pMoCAUcFwdParamsTable,
        sizeof(MoCA_UC_FWD_ENTRY) * MoCA_MAX_UC_FWD_ENTRIES);
    if(ret < 0)
        return(CMSRET_INTERNAL_ERROR);
    
    *pulUcFwdTblSize = ret * sizeof(MoCA_UC_FWD_ENTRY);

    return(CMSRET_SUCCESS);
}

CmsRet MoCACtl_GetUcFwdTbl(
        PMoCA_UC_FWD_ENTRY pMoCAUcFwdParamsTable,
        UINT32  *pulUcFwdTblSize)
{
    CmsRet ret;

    ret = do_open();
    if (ret != CMSRET_SUCCESS) return(ret);

    return(MoCACtl2_GetUcFwdTbl(
        vctx, pMoCAUcFwdParamsTable, pulUcFwdTblSize));
}
CmsRet MoCACtl2_GetMcFwdTbl(
        void * ctx,
        PMoCA_MC_FWD_ENTRY pMoCAMcFwdParamsTable,
        UINT32  *pulMcFwdTblSize)
{
    CmsRet ret;
    int numofentry;
    int i;
    struct moca_mc_fwd_rd MoCAMcFwdTbl[MoCA_MAX_MC_FWD_ENTRIES];

    ret = do_open();
    if (ret != CMSRET_SUCCESS) return(ret);

    numofentry = moca_get_mc_fwd_rd(ctx, (struct moca_mc_fwd_rd *)MoCAMcFwdTbl, sizeof(MoCAMcFwdTbl));
    if(numofentry < 0)
        return(CMSRET_INTERNAL_ERROR);

    for(i = 0; i < numofentry; i++) {
        (pMoCAMcFwdParamsTable+i)->mcMacAddr[0] = MoCAMcFwdTbl[i].multicast_mac_addr_hi;
        (pMoCAMcFwdParamsTable+i)->mcMacAddr[1] = MoCAMcFwdTbl[i].multicast_mac_addr_lo;        
        (pMoCAMcFwdParamsTable+i)->destNodeId = MoCAMcFwdTbl[i].dest_node_id;
    }
    *pulMcFwdTblSize = numofentry * sizeof(MoCA_MC_FWD_ENTRY);

    return(CMSRET_SUCCESS);
}

CmsRet MoCACtl_GetMcFwdTbl(
        PMoCA_MC_FWD_ENTRY pMoCAMcFwdParamsTable,
        UINT32  *pulMcFwdTblSize)
{
    CmsRet ret;

    ret = do_open();
    if (ret != CMSRET_SUCCESS) return(ret);

    return(MoCACtl2_GetMcFwdTbl(
        vctx, pMoCAMcFwdParamsTable, pulMcFwdTblSize));
}
CmsRet MoCACtl2_CreateMcFwdTblGroup(
        void * ctx,
        PMoCA_MC_FWD_ENTRY pMoCAMcFwdEntry)
{
    int ret;
    int i;
    uint32_t isBcast = 0;
    struct moca_mc_fwd_wr entry;
    uint32_t *hi;

    for(i = 0; i < pMoCAMcFwdEntry->numOfMembers; i++) {
        if (memcmp (pMoCAMcFwdEntry->nodeAddr[i].macAddr, gBcastMacAddr.macAddr, sizeof (sNodeMacAddr)) != 0) {
            if ((pMoCAMcFwdEntry->nodeAddr[i].macAddr [0] & 0x01000000) != 0)
                return (CMSRET_INVALID_PARAM_VALUE);
        }
        else
            isBcast = 1;
    }

    memset(&entry, 0, sizeof(entry));
    entry.multicast_mac_addr_hi = pMoCAMcFwdEntry->mcMacAddr[0];
    entry.multicast_mac_addr_lo = pMoCAMcFwdEntry->mcMacAddr[1];    
    if (pMoCAMcFwdEntry->numOfMembers > MoCA_NUM_MEMBERS_FOR_NON_BCAST || isBcast) {
        entry.dest_mac_addr1_hi = gBcastMacAddr.macAddr[0];
        entry.dest_mac_addr1_lo = gBcastMacAddr.macAddr[1];
    } else {
        hi = &entry.dest_mac_addr1_hi;
        for(i = 0; i < pMoCAMcFwdEntry->numOfMembers; i++) {
            hi[0] = pMoCAMcFwdEntry->nodeAddr[i].macAddr[0];
            hi[1] = pMoCAMcFwdEntry->nodeAddr[i].macAddr[1];
            hi += 2;
        }
    }
    ret = moca_add_mc_fwd_wr(ctx, &entry);
    if(ret < 0)
        return(CMSRET_INTERNAL_ERROR);

    return(CMSRET_SUCCESS);
}

CmsRet MoCACtl_CreateMcFwdTblGroup(
        PMoCA_MC_FWD_ENTRY pMoCAMcFwdEntry)
{
    CmsRet ret;

    ret = do_open();
    if (ret != CMSRET_SUCCESS) return(ret);

    return(MoCACtl2_CreateMcFwdTblGroup(vctx, 
        pMoCAMcFwdEntry));
}

CmsRet MoCACtl2_DeleteMcFwdTblGroup (
        void *  ctx,
        PMoCA_MC_FWD_ENTRY pMoCAMcFwdEntry)
{
    int ret;
    struct moca_mc_fwd_wr entry;

    memset(&entry, 0, sizeof(entry));
    entry.multicast_mac_addr_hi = pMoCAMcFwdEntry->mcMacAddr[0];
    entry.multicast_mac_addr_lo = pMoCAMcFwdEntry->mcMacAddr[1];
    
    ret = moca_del_mc_fwd_wr(ctx, &entry);
    if(ret < 0)
        return(CMSRET_INTERNAL_ERROR);

    return(CMSRET_SUCCESS);
}

CmsRet MoCACtl_DeleteMcFwdTblGroup (
        PMoCA_MC_FWD_ENTRY pMoCAMcFwdEntry)
{
    CmsRet ret;

    ret = do_open();
    if (ret != CMSRET_SUCCESS) return(ret);

    return(MoCACtl2_DeleteMcFwdTblGroup(vctx, pMoCAMcFwdEntry));
}

CmsRet MoCACtl2_AddMcFwdTblEntry(
        void *  ctx,
        PMoCA_MC_FWD_ENTRY pMoCAMcFwdParamsTable,
        UINT32  ulMcFwdTblSize)
{
    /* Not currently used in mocactl */
    return(CMSRET_SUCCESS);
}

CmsRet MoCACtl_AddMcFwdTblEntry(
        PMoCA_MC_FWD_ENTRY pMoCAMcFwdParamsTable,
        UINT32  ulMcFwdTblSize)
{
    CmsRet ret;

    ret = do_open();
    if (ret != CMSRET_SUCCESS) return(ret);

    /* Not currently used in mocactl */

    return(MoCACtl2_AddMcFwdTblEntry(vctx, 
        pMoCAMcFwdParamsTable, ulMcFwdTblSize));
}

CmsRet MoCACtl2_DeleteMcFwdTblEntry(
        void *  ctx,
        PMoCA_MC_FWD_ENTRY pMoCAMcFwdParamsTable,
        UINT32  ulMcFwdTblSize)
{
    /* Not currently used in mocactl */
    return(CMSRET_SUCCESS);
}

CmsRet MoCACtl_DeleteMcFwdTblEntry(
        PMoCA_MC_FWD_ENTRY pMoCAMcFwdParamsTable,
        UINT32  ulMcFwdTblSize)
{
    CmsRet ret;

    ret = do_open();
    if (ret != CMSRET_SUCCESS) return(ret);

    /* Not currently used in mocactl */

    return(MoCACtl2_DeleteMcFwdTblEntry( vctx,
        pMoCAMcFwdParamsTable, ulMcFwdTblSize));
}

CmsRet MoCACtl2_GetSrcAddrTbl(
        void *  ctx,
        PMoCA_SRC_ADDR_ENTRY pMoCASrcAddrParamsTable,
        UINT32  *pulSrcAddrTblSize)
{
    int ret;

    ret = moca_get_src_addr(ctx, (struct moca_src_addr *)pMoCASrcAddrParamsTable,
        sizeof(MoCA_SRC_ADDR_ENTRY) * MoCA_MAX_SRC_ADDR_ENTRIES);
    if(ret < 0)
        return(CMSRET_INTERNAL_ERROR);
    
    *pulSrcAddrTblSize = ret * sizeof(MoCA_SRC_ADDR_ENTRY);

    return(CMSRET_SUCCESS);
}

CmsRet MoCACtl_GetSrcAddrTbl(
        PMoCA_SRC_ADDR_ENTRY pMoCASrcAddrParamsTable,
        UINT32  *pulSrcAddrTblSize)
{
    CmsRet ret;

    ret = do_open();
    if (ret != CMSRET_SUCCESS) return(ret);

    return(MoCACtl2_GetSrcAddrTbl( vctx,
        pMoCASrcAddrParamsTable, pulSrcAddrTblSize ));
}
CmsRet MoCACtl2_GetStatus(
     void * ctx,
     PMoCA_STATUS p)
{
    struct moca_init_time par;
    struct moca_drv_info info;
    struct moca_key_times key_times;

    memset(p, 0, sizeof(*p));
    moca_get_gen_status(ctx, (struct moca_gen_status *)&p->generalStatus);
    moca_get_ext_status(ctx, (struct moca_ext_status *)&p->extendedStatus);
    moca_get_init_time(ctx, &par);
    moca_get_drv_info(ctx, &info);
    moca_get_key_times(ctx, &key_times);

    p->miscStatus.isNC =
        (p->generalStatus.nodeId == p->generalStatus.ncNodeId) ? 1 : 0;
    moca_u32_to_mac((uint8_t *)&p->miscStatus.macAddr,
        par.mac_addr_hi, par.mac_addr_lo);
    p->miscStatus.driverUpTime = info.uptime;

    p->generalStatus.rfChannel *= 25;
    p->generalStatus.hwVersion = info.hw_rev;
    p->miscStatus.MoCAUpTime = info.core_uptime;
    p->miscStatus.linkUpTime = info.link_uptime;

    p->extendedStatus.lastTekExchange = key_times.tekTime;
    p->extendedStatus.lastTekInterval = key_times.tekLastInterval;
    p->extendedStatus.tekEvenOdd = key_times.tekEvenOdd;      
    p->extendedStatus.lastPmkExchange = key_times.pmkTime;
    p->extendedStatus.lastPmkInterval = key_times.pmkLastInterval;
    p->extendedStatus.pmkEvenOdd = key_times.pmkEvenOdd;

    return(CMSRET_SUCCESS);
}

CmsRet MoCACtl_GetStatus(
     PMoCA_STATUS p)
{
    CmsRet ret;

    ret = do_open();
    if (ret != CMSRET_SUCCESS) return(ret);

    return( MoCACtl2_GetStatus(vctx, p) );
}
CmsRet MoCACtl2_GetStatistics(
     void * ctx,
     PMoCA_STATISTICS pMoCAStats,
     UINT32 ulReset)
{
    struct moca_ext_octet_count eo;

    memset(pMoCAStats, 0, sizeof(*pMoCAStats));
    moca_get_gen_stats(ctx, (struct moca_gen_stats *)&pMoCAStats->generalStats);
    moca_get_ext_stats(ctx, (struct moca_ext_stats *)&pMoCAStats->extendedStats);

    moca_get_ext_octet_count(ctx, &eo);
    pMoCAStats->BitStats64.inOctets_hi = eo.in_octets_hi;
    pMoCAStats->BitStats64.outOctets_hi = eo.out_octets_hi;

    if(ulReset)
        moca_set_reset_stats(ctx);

    return(CMSRET_SUCCESS);
}

CmsRet MoCACtl_GetStatistics(
     PMoCA_STATISTICS pMoCAStats,
     UINT32 ulReset)
{
    CmsRet ret;

    ret = do_open();
    if (ret != CMSRET_SUCCESS) return(ret);

    return( MoCACtl2_GetStatistics( vctx,
        pMoCAStats, ulReset ));
}

CmsRet MoCACtl2_GetNodeStatus(
     void * ctx,
     PMoCA_NODE_STATUS_ENTRY pNodeStatusEntry)
{
    int i = pNodeStatusEntry->nodeId;

    memset (&pNodeStatusEntry->eui, 0, sizeof(pNodeStatusEntry->eui));
    memset (&pNodeStatusEntry->txUc, 0, sizeof(pNodeStatusEntry->txUc));
    memset (&pNodeStatusEntry->rxUc, 0, sizeof(pNodeStatusEntry->rxUc));
    memset (&pNodeStatusEntry->rxBc, 0, sizeof(pNodeStatusEntry->rxBc));
    memset (&pNodeStatusEntry->rxMap, 0, sizeof(pNodeStatusEntry->rxMap));

    moca_get_gen_node_status(ctx, i, (struct moca_gen_node_status *)&pNodeStatusEntry->eui);
    moca_get_tx_profile(ctx, i, (struct moca_tx_profile *)&pNodeStatusEntry->txUc);

    moca_get_rx_uc_profile(ctx, i,
        (struct moca_rx_uc_profile *)&pNodeStatusEntry->rxUc);
    moca_get_rx_bc_profile(ctx, i,
        (struct moca_rx_bc_profile *)&pNodeStatusEntry->rxBc);
    moca_get_rx_map_profile(ctx, i,
        (struct moca_rx_map_profile *)&pNodeStatusEntry->rxMap);

    pNodeStatusEntry->maxPhyRates.txUcPhyRate = moca_phy_rate(
        pNodeStatusEntry->txUc.nBas,
        pNodeStatusEntry->txUc.cp, pNodeStatusEntry->txUc.turbo);
    pNodeStatusEntry->maxPhyRates.rxUcPhyRate = moca_phy_rate(
        pNodeStatusEntry->rxUc.nBas,
        pNodeStatusEntry->rxUc.cp, pNodeStatusEntry->rxUc.turbo);
    pNodeStatusEntry->maxPhyRates.rxBcPhyRate = moca_phy_rate(
        pNodeStatusEntry->rxBc.nBas,
        pNodeStatusEntry->rxBc.cp, 0);
    pNodeStatusEntry->maxPhyRates.rxMapPhyRate = moca_phy_rate(
        pNodeStatusEntry->rxMap.nBas,
        pNodeStatusEntry->rxMap.cp, 0);


    return(CMSRET_SUCCESS);
}

CmsRet MoCACtl_GetNodeStatus(
     PMoCA_NODE_STATUS_ENTRY pNodeStatusEntry)
{
    CmsRet ret;

    ret = do_open();
    if (ret != CMSRET_SUCCESS) return(ret);

    return(MoCACtl2_GetNodeStatus(vctx, pNodeStatusEntry));
}

CmsRet MoCACtl2_GetNodeTblStatus(
     void * ctx,
     PMoCA_NODE_STATUS_ENTRY pNodeStatusEntry,
     PMoCA_NODE_COMMON_STATUS_ENTRY pNodeCommonStatusEntry,
     UINT32 *pulNodeStatusTblSize)
{
    int i, num_nodes = 0;
    struct moca_gen_status gs;
    struct moca_init_time par;    

    moca_get_init_time(ctx, &par);    

    memset(pNodeStatusEntry, 0, sizeof(*pNodeStatusEntry));
    memset(pNodeCommonStatusEntry, 0, sizeof(*pNodeCommonStatusEntry));

    /* get common status */
    moca_get_tx_profile(ctx, MOCA_TX_BC,
        (struct moca_tx_profile *)&pNodeCommonStatusEntry->txBc);
    moca_get_tx_profile(ctx, MOCA_TX_MAP,
        (struct moca_tx_profile *)&pNodeCommonStatusEntry->txMap);
    pNodeCommonStatusEntry->maxCommonPhyRates.txBcPhyRate =
        moca_phy_rate(
            pNodeCommonStatusEntry->txBc.nBas,
            pNodeCommonStatusEntry->txBc.cp,
            0);
    pNodeCommonStatusEntry->maxCommonPhyRates.txMapPhyRate =
        moca_phy_rate(
            pNodeCommonStatusEntry->txMap.nBas,
            pNodeCommonStatusEntry->txMap.cp,
            0);
    
    /* get node bitmask */
    moca_get_gen_status(ctx, &gs);

    /* get status entry for each node */
    for(i = 0; i < MOCA_MAX_NODES; i++) {
        if(! (gs.connected_nodes & (1 << i)))
            continue;
        if(gs.node_id == i)
            continue;

        pNodeStatusEntry->nodeId = i;
        MoCACtl2_GetNodeStatus(ctx, pNodeStatusEntry);
        pNodeStatusEntry++;
        num_nodes++;
    }

    /* fill in *pulNodeStatusTblSize with number of nodes * entry size */
    *pulNodeStatusTblSize = num_nodes * sizeof(*pNodeStatusEntry);

    return(CMSRET_SUCCESS);
}

CmsRet MoCACtl_GetNodeTblStatus(
     PMoCA_NODE_STATUS_ENTRY pNodeStatusEntry,
     PMoCA_NODE_COMMON_STATUS_ENTRY pNodeCommonStatusEntry,
     UINT32 *pulNodeStatusTblSize)
{
    CmsRet ret;

    ret = do_open();
    if (ret != CMSRET_SUCCESS) return(ret);

    return(MoCACtl2_GetNodeTblStatus(vctx, 
        pNodeStatusEntry, pNodeCommonStatusEntry, 
        pulNodeStatusTblSize));
}


CmsRet MoCACtl2_GetNodeStatistics(
     void * ctx,
     PMoCA_NODE_STATISTICS_ENTRY pNodeStatsEntry,
     UINT32 ulReset)
{
    moca_get_node_stats(ctx, pNodeStatsEntry->nodeId,
        (struct moca_node_stats *)&pNodeStatsEntry->txPkts);

    if(ulReset)
        moca_set_reset_stats(ctx);

    return(CMSRET_SUCCESS);
}

CmsRet MoCACtl_GetNodeStatistics(
     PMoCA_NODE_STATISTICS_ENTRY pNodeStatsEntry,
     UINT32 ulReset)
{
    CmsRet ret;

    ret = do_open();
    if (ret != CMSRET_SUCCESS) return(ret);

    return(MoCACtl2_GetNodeStatistics(vctx, 
        pNodeStatsEntry, ulReset));
}

CmsRet MoCACtl2_GetNodeStatisticsExt(
     void * ctx,
     PMoCA_NODE_STATISTICS_EXT_ENTRY pNodeStatsEntry,
     UINT32 ulReset)
{
    moca_get_node_stats_ext_acc(ctx, pNodeStatsEntry->nodeId,
        (void *)&pNodeStatsEntry->rxUcCrcError );

    if(ulReset)
        moca_set_reset_stats(ctx);

    return(CMSRET_SUCCESS);
}


CmsRet MoCACtl2_GetNodeTblStatistics(
     void * ctx,
     PMoCA_NODE_STATISTICS_ENTRY pNodeStatsEntry,
     UINT32 *pulNodeStatsTblSize,
     UINT32 ulReset)
{
    int i, num_nodes = 0;
    struct moca_gen_status gs;

    /* get node bitmask */
    moca_get_gen_status(ctx, &gs);

    /* get stats entry for each node */
    for(i = 0; i < MOCA_MAX_NODES; i++) {
        if(! (gs.connected_nodes & (1 << i)))
            continue;
        if(gs.node_id == i)
            continue;

        pNodeStatsEntry->nodeId = i;
        MoCACtl2_GetNodeStatistics(ctx, pNodeStatsEntry, 0);
        pNodeStatsEntry++;
        num_nodes++;
    }

    /* fill in *pulNodeStatusTblSize with number of nodes * entry size */
    *pulNodeStatsTblSize = num_nodes * sizeof(*pNodeStatsEntry);

    if(ulReset)
        moca_set_reset_stats(ctx);

    return(CMSRET_SUCCESS);
}


CmsRet MoCACtl2_GetNodeTblStatisticsExt(
     void * ctx,
     PMoCA_NODE_STATISTICS_EXT_ENTRY pNodeStatsEntry,
     UINT32 *pulNodeStatsTblSize,
     UINT32 ulReset)
{
    int i, num_nodes = 0;
    struct moca_gen_status gs;

    /* get node bitmask */
    moca_get_gen_status(ctx, &gs);

    /* get stats entry for each node */
    for(i = 0; i < MOCA_MAX_NODES; i++) {
        if(! (gs.connected_nodes & (1 << i)))
            continue;
        if(gs.node_id == i)
            continue;

        pNodeStatsEntry->nodeId = i;
        MoCACtl2_GetNodeStatisticsExt(ctx, pNodeStatsEntry, 0);
        pNodeStatsEntry++;
        num_nodes++;
    }

    /* fill in *pulNodeStatusTblSize with number of nodes * entry size */
    *pulNodeStatsTblSize = num_nodes * sizeof(*pNodeStatsEntry);

    if(ulReset)
        moca_set_reset_stats(ctx);

    return(CMSRET_SUCCESS);
}


CmsRet MoCACtl_GetNodeTblStatistics(
     PMoCA_NODE_STATISTICS_ENTRY pNodeStatsEntry,
     UINT32 *pulNodeStatsTblSize,
     UINT32 ulReset)
{
    CmsRet ret;

    ret = do_open();
    if (ret != CMSRET_SUCCESS) return(ret);

    return(MoCACtl2_GetNodeTblStatistics(
        vctx, pNodeStatsEntry, pulNodeStatsTblSize, ulReset));
}

CmsRet MoCACtl2_GetVersion(
     void * ctx,
     PMoCA_VERSION pMoCAVersion)
{
    struct moca_gen_status gs;
    struct moca_drv_info info;
    int ret = 0;
    
    memset(pMoCAVersion, 0, sizeof(*pMoCAVersion));

    ret = moca_get_gen_status(ctx, &gs);
    if (ret == 0)
    {
        pMoCAVersion->coreSwVersion = gs.moca_sw_version;
        pMoCAVersion->nwVersion = gs.network_moca_version;
        pMoCAVersion->selfVersion = gs.self_moca_version;
    }
    

    ret = moca_get_drv_info(ctx, &info);
    
    if (ret == 0)
    {
        pMoCAVersion->coreHwVersion = info.hw_rev;
        pMoCAVersion->drvMjVersion = info.version >> 16;
        pMoCAVersion->drvMnVersion = info.version & 0xffff;
        pMoCAVersion->drvBuildVersion = info.build_number; 
    }

    return(CMSRET_SUCCESS);
}

CmsRet MoCACtl_GetVersion(
     PMoCA_VERSION pMoCAVersion)
{
    CmsRet ret;

    ret = do_open();
    if (ret != CMSRET_SUCCESS) return(ret);

    return(MoCACtl2_GetVersion(vctx, pMoCAVersion));
}

CmsRet MoCACtl2_FunctionCall(
    void * ctx,
    PMoCA_FUNC_CALL_PARAMS dp)
{
    struct moca_lab_call_func p;

    p.address = dp->Address;
    p.param1 = dp->param1;
    p.param2 = dp->param2;

    moca_set_lab_call_func(ctx, &p);

    return(CMSRET_SUCCESS);
}

CmsRet MoCACtl_FunctionCall(PMoCA_FUNC_CALL_PARAMS dp)
{
    CmsRet ret;

    ret = do_open();
    if (ret != CMSRET_SUCCESS) return(ret);

    return(MoCACtl2_FunctionCall(vctx, dp));
}

CmsRet MoCACtl2_SetTPCAP(
    void * ctx,
    PMoCA_TPCAP_PARAMS dp)
{
    struct moca_lab_tpcap p;

    p.enable = dp->enable;
    p.type = dp->type;

    moca_set_lab_tpcap(ctx, &p);

    return(CMSRET_SUCCESS);
}

CmsRet MoCACtl2_GetTraceConfig (
     void * ctx,
     PMoCA_TRACE_PARAMS pMoCATraceParams)
{
    uint32_t status;

    pMoCATraceParams->traceLevel = 0;
    
    moca_get_moca_core_trace_enable(ctx, &status);

    if(status)
        pMoCATraceParams->traceLevel = MOCA_TRC_LEVEL_CORE;
    else
        pMoCATraceParams->traceLevel = MOCA_TRC_LEVEL_NONE;

    moca_get_verbose(ctx, &status);

    if (status & 0x1)  
        pMoCATraceParams->traceLevel |= MOCA_TRC_LEVEL_DBG;
    if (status & 0x2)  
        pMoCATraceParams->traceLevel |= MOCA_TRC_LEVEL_VERBOSE;
    if (status & 0x4)  
        pMoCATraceParams->traceLevel |= MOCA_TRC_LEVEL_INFO;
    if (status & 0x8)  
        pMoCATraceParams->traceLevel |= MOCA_TRC_LEVEL_WARN;
    if (status & 0x10) 
        pMoCATraceParams->traceLevel |= MOCA_TRC_LEVEL_ERR;
    if (status & 0x20) 
        pMoCATraceParams->traceLevel |= MOCA_TRC_LEVEL_TRAP;
    if (status & 0x40) 
        pMoCATraceParams->traceLevel |= MOCA_DUMP_HOST_CORE;        
    
    return(CMSRET_SUCCESS);
}

CmsRet MoCACtl_GetTraceConfig (
     PMoCA_TRACE_PARAMS pMoCATraceParams)
{
    CmsRet ret;

    ret = do_open();
    if (ret != CMSRET_SUCCESS) return(ret);

    return(MoCACtl2_GetTraceConfig(vctx, pMoCATraceParams));
}

CmsRet MoCACtl2_SetTraceConfig (
     void * ctx,
     PMoCA_TRACE_PARAMS pMoCATraceParams)
{
    int ret;
#if defined(DSL_MOCA) /* DSL Code */
    uint32_t level = 0x1C; /* always enable error, warn, info messages */
#else
    uint32_t level = 0x18; /* always enable error, warn messages */
#endif

    if(pMoCATraceParams->bRestoreDefault)
    {
        pMoCATraceParams->traceLevel = MOCA_TRC_LEVEL_INFO |
            MOCA_TRC_LEVEL_WARN | MOCA_TRC_LEVEL_ERR;
#if defined(MOCA_CTP_CANDIDATE) /* DSL Code */
        pMoCATraceParams->traceLevel |= MOCA_TRC_LEVEL_CORE;
#endif
    }
    if(pMoCATraceParams->traceLevel & MOCA_TRC_LEVEL_CORE)
        moca_set_moca_core_trace_enable(ctx, 1);
    else
        moca_set_moca_core_trace_enable(ctx, 0);

    if (pMoCATraceParams->traceLevel & MOCA_TRC_LEVEL_DBG)      
        level |= 0x1;
    if (pMoCATraceParams->traceLevel & MOCA_TRC_LEVEL_VERBOSE)  
        level |= 0x2;
    if (pMoCATraceParams->traceLevel & MOCA_TRC_LEVEL_INFO)   
        level |= 0x4;
    if (pMoCATraceParams->traceLevel & MOCA_TRC_LEVEL_TRAP)
        level |= 0x20;
    if (pMoCATraceParams->traceLevel & MOCA_DUMP_HOST_CORE)
        level |= 0x40;
    
    ret = moca_set_verbose(ctx, level);
    if (ret != 0)
        return(CMSRET_INTERNAL_ERROR);
    
    return(CMSRET_SUCCESS);
}

CmsRet MoCACtl_SetTraceConfig (
     PMoCA_TRACE_PARAMS pMoCATraceParams)
{
    CmsRet ret;

    ret = do_open();
    if (ret != CMSRET_SUCCESS) return(ret);

    return(MoCACtl2_SetTraceConfig(vctx, pMoCATraceParams));
}

/***************************************************************************
 * Function Name: MoCACtl_GetIQData
 * Description  : Returns the MoCA IQ Diagram Data information.
 * Returns      : CmsRet return code
 ***************************************************************************/
CmsRet MoCACtl2_GetIQData(
     void * ctx,
     PMoCA_IQ_DATA pMoCAIQData)
{
    CmsRet ret;

    ret = moca_get_iq_data(ctx, (struct moca_iq_data *)pMoCAIQData->pData);

    if (ret != 0)
        return (CMSRET_INTERNAL_ERROR);
    
    return (CMSRET_SUCCESS) ;
} /* MoCACtl_GetIQData */

CmsRet MoCACtl_GetIQData(
     PMoCA_IQ_DATA pMoCAIQData)
{
    CmsRet ret;

    ret = do_open();
    if (ret != CMSRET_SUCCESS) return(ret);

    return(MoCACtl2_GetIQData(vctx, pMoCAIQData));
}

/***************************************************************************
 * Function Name: MoCACtl_GetSNRData
 * Description  : Returns the MoCA SNR graph set Data information.
 * Returns      : CmsRet return code
 ***************************************************************************/
CmsRet MoCACtl2_GetSNRData(
     void * ctx,
     PMoCA_SNR_DATA pMoCASNRData)
{
    int ret;

    ret = moca_get_snr_data(ctx, (struct moca_snr_data *)pMoCASNRData->pData);

    if (ret != 0)
        return (CMSRET_INTERNAL_ERROR);
    
    return (CMSRET_SUCCESS) ;
}

CmsRet MoCACtl_GetSNRData(
     PMoCA_SNR_DATA pMoCASNRData)
{
    CmsRet ret;

    ret = do_open();
    if (ret != CMSRET_SUCCESS) return(ret);

    return(MoCACtl2_GetSNRData(vctx, pMoCASNRData));
}

/***************************************************************************
 * Function Name: MoCACtl_GetCIRData
 * Description  : Returns the MoCA CIR graph set Data information.
 * Returns      : CmsRet return code
 ***************************************************************************/
CmsRet MoCACtl2_GetCIRData(
     void * ctx,
     PMoCA_CIR_DATA pMoCACIRData)
{
    int ret;

    ret = moca_get_cir_data(ctx, pMoCACIRData->nodeId, (struct moca_cir_data *)pMoCACIRData->pData);

    if (ret != 0)
        return (CMSRET_INTERNAL_ERROR);
    
    return (CMSRET_SUCCESS) ;
}

CmsRet MoCACtl_GetCIRData(
     PMoCA_CIR_DATA pMoCACIRData)
{
    CmsRet ret;

    ret = do_open();
    if (ret != CMSRET_SUCCESS) return(ret);

    return (MoCACtl2_GetCIRData(vctx, pMoCACIRData));
}


/***************************************************************************
 * Function Name: MoCACtl_RegisterCallback
 * Description  : Registers a callback function for a MoCA event.
 * Returns      : CmsRet return code
 ***************************************************************************/
CmsRet MoCACtl2_RegisterCallback(
    void * ctx,
    MoCA_CALLBACK_EVENT event, 
    void (*callback)(void *userarg, MoCA_CALLBACK_DATA data), 
    void *userarg)
{
    if ((event >= MoCA_MAX_CALLBACK_EVENTS) ||
        (callback == NULL))
        return(CMSRET_INVALID_ARGUMENTS);

    switch (event)
    {
        case MoCA_CALLBACK_EVENT_LINK_STATUS:
            moca_register_link_up_state_cb(ctx, (void *)callback, userarg);
            break;
        case MoCA_CALLBACK_EVENT_LOF:
            moca_register_lof_cb(ctx, (void *)callback, userarg);
            break;
        default:
            return(CMSRET_INVALID_ARGUMENTS);
            break;
    }

    return(CMSRET_SUCCESS);
}

CmsRet MoCACtl_RegisterCallback(
    MoCA_CALLBACK_EVENT event, 
    void (*callback)(void *userarg, MoCA_CALLBACK_DATA data), 
    void *userarg)
{
    CmsRet ret;

    ret = do_open();
    if (ret != CMSRET_SUCCESS) return(ret);

    return (MoCACtl2_RegisterCallback(vctx, event, callback, userarg));
}

/***************************************************************************
 * Function Name: MoCACtl_DispatchCallback
 * Description  : Dispatches callback functions for a MoCA event.
 * Returns      : CmsRet return code
 ***************************************************************************/
CmsRet MoCACtl_DispatchCallback(
    MoCA_CALLBACK_EVENT event, 
    MoCA_CALLBACK_DATA * pData)
{
    return(CMSRET_METHOD_NOT_SUPPORTED);
}

CmsRet MoCACtl2_EventLoop(
    void * ctx)
{
    int ret = 0;

    ret = moca_event_loop(ctx);

    if (ret == 0)
        return(CMSRET_SUCCESS);
    else
        return(CMSRET_INTERNAL_ERROR);
}

/***************************************************************************
 * Function Name: MoCACtl_EventLoop
 * Description  : Listens for MoCA events and dispatches callback functions.
 *                This function will run until cancelled.
 * Returns      : CmsRet return code
 ***************************************************************************/
CmsRet MoCACtl_EventLoop(void)
{
    CmsRet ret;

    ret = do_open();
    if (ret != CMSRET_SUCCESS) return(ret);

    return (MoCACtl2_EventLoop(vctx));
}

CmsRet MoCACtl2_CancelEventLoop(
    void * ctx)
{
    moca_cancel_event_loop(ctx);
    return(CMSRET_SUCCESS);
}

/***************************************************************************
 * Function Name: MoCACtl_CancelEventLoop
 * Description  : Cancels event loop started by MoCACtl_EventLoop
 * Returns      : CmsRet return code
 ***************************************************************************/
CmsRet MoCACtl_CancelEventLoop(void)
{
    CmsRet ret;

    ret = do_open();
    if (ret != CMSRET_SUCCESS) 
        return(ret);

    return (MoCACtl2_CancelEventLoop(vctx));
}


CmsRet MoCACtl2_Fmr(
    void * ctx,
    PMoCA_FMR_PARAMS params)
{
    int ret;
    struct moca_fmr_request req;
    int i;
    struct moca_gen_status gs;
    struct moca_gen_node_status gns;
    MAC_ADDRESS mac;

    memset(&req, 0, sizeof(req));

    /* The mac address will either be a single MAC address or the broadcase mac
     * addr. In the case of the single addr, find the node ID, otherwise use
     * all active 1.1 nodes for the wave0Nodemask.
     */

    /* get node bitmask */
    moca_get_gen_status(ctx, &gs);
    if (memcmp(params->address, gBcastMacAddr.macAddr, sizeof (MAC_ADDRESS)))  
    {     
        /* get status entry for each node */
        for(i = 0; i < MOCA_MAX_NODES; i++) 
        {        
            if(! (gs.connected_nodes & (1 << i)))
                continue;
            ret = moca_get_gen_node_status(ctx, i, (struct moca_gen_node_status *)&gns);
            moca_u32_to_mac(mac, gns.eui_hi, gns.eui_lo);
            if ((ret == 0) &&           
                (gns.eui_hi == params->address[0]) &&
                (gns.eui_lo == params->address[1]) &&
                ((gns.protocol_support >> 24) == MoCA_VERSION_11))
            {           
                req.wave0Nodemask = (1 << i);
                break;
            }
        }  
    }  
    else /* broadcast */ 
    {
        req.wave0Nodemask = gs.connected_nodes;
    }

    if (req.wave0Nodemask != 0)
        ret = moca_set_fmr_request(ctx, &req);
    else
        return(CMSRET_INVALID_ARGUMENTS);

    if (ret != 0)
        return (CMSRET_INTERNAL_ERROR);

  return (CMSRET_SUCCESS) ;
}

CmsRet MoCACtl2_CreatePQoSFlow( 
    void * ctx,
    PMoCA_CREATE_PQOS_PARAMS params)
{
    int ret;
    struct moca_pqos_create_request req;
    struct moca_gen_status gs;
    struct moca_gen_node_status gns;
    struct moca_uc_fwd ucfwd[MoCA_MAX_UC_FWD_ENTRIES];
    struct moca_pqos_table pqos;
    MAC_ADDRESS mac;

    int num_ucfwd = 0;
    MAC_ADDRESS temp_mac;
    int i, j;
    int ingress_node_found = 0;
    uint32_t egress_node_id = 0x3F; /* default is broadcast */

    memset(&req, 0, sizeof(req));
    memset(&pqos, 0, sizeof(pqos));

    /* check VLAN parameters */
    if (params->vlan_id > MoCA_PQOS_VLAN_ID_MAX) 
    {
        printf("%s: Invalid vlan_id 0x%x\n", __FUNCTION__, params->vlan_id);
        return(CMSRET_INVALID_ARGUMENTS);
    }
    if ((params->vlan_prio != MoCA_PQOS_VLAN_PRIO_DEFAULT) &&
        ((params->vlan_prio > MoCA_PQOS_VLAN_PRIO_MAX) || 
         ((params->vlan_prio != 0) && 
          ((params->vlan_id != 0) && (params->vlan_id != MoCA_PQOS_VLAN_ID_DEFAULT)))))
    {
        printf("%s: Invalid vlan_prio (%u)/vlan_id (0x%x)\n",
            __FUNCTION__, params->vlan_prio, params->vlan_id);
        return(CMSRET_INVALID_ARGUMENTS);
    }

    /* Check to see if the user specified a flow ID */    
    if (memcmp(gNull_MAC_ADDRESS, params->flow_id, sizeof(MAC_ADDRESS)))
    {
        if ((params->flow_id[0] & 0x1) == 0)
        {
            printf("%s: Invalid (non-mcast) flow_id\n", __FUNCTION__);
            return(CMSRET_INVALID_ARGUMENTS);
        }

        if (((params->packet_da[0] & 0x1) != 0) &&
            (memcmp(params->packet_da, params->flow_id, sizeof(MAC_ADDRESS))))
        {
            printf("%s: Error! flow_id must match mcast packet_da\n", __FUNCTION__);
            return(CMSRET_INVALID_ARGUMENTS);
        }
    }
    else
    {
        if ((params->packet_da[0] & 0x1) == 0)
        {
            /* unicast address, need to convert to flow id */
            params->flow_id[0] = 0x01;
            params->flow_id[1] = 0x00;
            params->flow_id[2] = 0x5E;
            params->flow_id[3] = params->packet_da[3] & 0x7F;
            params->flow_id[4] = params->packet_da[4];
            params->flow_id[5] = params->packet_da[5];
        }
        else
        {
            memcpy(params->flow_id, params->packet_da, sizeof(params->flow_id));
        }
    }
    moca_mac_to_u32(&req.flowid_hi, &req.flowid_lo, params->flow_id);

    req.tPacketSize = params->packet_size;

    /* get node bitmask */
    moca_get_gen_status(ctx, &gs);

    /* find ingress and egress node id, set wave0Nodemask */
    for(i = 0; i < MOCA_MAX_NODES; i++)
    {
        if(! (gs.connected_nodes & (1 << i)))
            continue;
        ret = moca_get_gen_node_status(ctx, i, (struct moca_gen_node_status *)&gns);
        if (ret == 0)
        {
            if ((gns.protocol_support >> 24) == MoCA_VERSION_11)
                req.wave0Nodemask |= (1 << i);

            moca_u32_to_mac(mac, gns.eui_hi, gns.eui_lo);
            if (!memcmp(mac, params->ingress_node, sizeof(MAC_ADDRESS)))
            {
                req.ingressNodeId = i;
                ingress_node_found = 1;
            }
            else if (!memcmp(mac, params->egress_node, sizeof(MAC_ADDRESS)))
            {
                egress_node_id = i;
            }
        }
    }
    if (ingress_node_found == 0)
    {
        return(CMSRET_INVALID_ARGUMENTS);
    }
    if (!memcmp(gNullMacAddr.macAddr, params->egress_node, sizeof(MAC_ADDRESS)))
    {
        /* zero egress mac address, need to check listener addresses */
        num_ucfwd = moca_get_uc_fwd(ctx, ucfwd, sizeof(ucfwd));
        for (j = 0; j < MoCA_MAX_PQOS_LISTENERS; j++)
        {
            if (memcmp(gNullMacAddr.macAddr, params->listeners[j],
                sizeof(MAC_ADDRESS)))
            {
                /* non-zero listener address, try to find match in ucfwd table */
                for (i = 0; i < num_ucfwd; i++)
                {
                    moca_u32_to_mac(temp_mac, ucfwd[i].mac_addr_hi, ucfwd[i].mac_addr_lo);
                    if (!memcmp(temp_mac, params->listeners[j], sizeof(MAC_ADDRESS)))
                    {
                        /* found a match, they all have to be on the same node */
                        if (egress_node_id == 0x3F)
                        { /* first match */
                            egress_node_id = ucfwd[i].moca_dest_node_id;
                        }
                        else if (egress_node_id != ucfwd[i].moca_dest_node_id)
                        {
                            egress_node_id = 0x3F;
                            break;
                        }
                    }
                }
                if (egress_node_id == 0x3F)
                {
                    /* We didn't match the address in the ucfwd table,
                     * break and use the broadcast node id */
                     break;
                }
            }
        }
    }

    req.egressNodeId = egress_node_id;
    req.flowTag = params->flow_tag;
    moca_mac_to_u32(&req.packetda_hi, &req.packetda_lo, params->packet_da);
    req.tPeakDataRate = params->peak_rate;
    req.tLeaseTime = params->lease_time;
    req.tBurstSize = params->burst_count;

    /* If we are the ingress node, add to the table */
    if (req.ingressNodeId == gs.node_id)
    {
        pqos.flow_id_hi = req.flowid_hi;
        pqos.flow_id_lo = req.flowid_lo;
        pqos.packet_da_hi = req.packetda_hi;
        pqos.packet_da_lo = req.packetda_lo;
        moca_mac_to_u32(&pqos.talker_hi, &pqos.talker_lo, params->talker);
        pqos.lease_time = params->lease_time;
        pqos.vlan_prio = params->vlan_prio;
        pqos.vlan_id = params->vlan_id;
        if (egress_node_id == 0x3F)
            pqos.egress_node_mask = gs.connected_nodes;
        else
            pqos.egress_node_mask = (1 << egress_node_id);

        ret = moca_add_pqos_table(ctx, &pqos);
        if (ret != 0)
        {
            return (CMSRET_INTERNAL_ERROR);
        }
    }

    ret = moca_set_pqos_create_request(ctx, &req);
    if (ret != 0)
    {
        moca_del_pqos_table(ctx, &pqos);
        return (CMSRET_INTERNAL_ERROR);
    }

    return (CMSRET_SUCCESS) ;
}

CmsRet MoCACtl2_UpdatePQoSFlow(
    void * ctx,
    PMoCA_UPDATE_PQOS_PARAMS params)
{
    int ret, i;
    struct moca_pqos_update_request req;
    struct moca_gen_status gs;
    struct moca_gen_node_status gns;

    memset(&req, 0, sizeof(req));

    /* get node bitmask */
    moca_get_gen_status(ctx, &gs);

    /* set wave0Nodemask to all 1.1 nodes */
    for(i = 0; i < MOCA_MAX_NODES; i++)
    {
        if(! (gs.connected_nodes & (1 << i)))
            continue;
        ret = moca_get_gen_node_status(ctx, i, (void *)&gns);
        if (ret == 0)
        {
            if ((gns.protocol_support >> 24) == MoCA_VERSION_11)
                req.wave0Nodemask |= (1 << i);
        }
    }

    req.tPacketSize = params->packet_size;
    req.flowTag = params->flow_tag;
    req.tPeakDataRate = params->peak_rate;
    req.tLeaseTime = params->lease_time;
    req.tBurstSize = params->burst_count;
    moca_mac_to_u32(&req.flowid_hi, &req.flowid_lo, params->flow_id);
    req.tLeaseTime = params->lease_time;
    moca_mac_to_u32(&req.packetda_hi, &req.packetda_lo, params->packet_da);

    ret = moca_set_pqos_update_request(ctx, &req);

    if (ret != 0)
    {
        printf("%s: moca_set_pqos_update_request returned %d\n",
            __FUNCTION__, ret);
        return (CMSRET_INTERNAL_ERROR);
    }

    return (CMSRET_SUCCESS) ;
}

CmsRet MoCACtl2_DeletePQoSFlow(
    void * ctx,
    PMoCA_DELETE_PQOS_PARAMS params)
{
    int ret, i;
    struct moca_pqos_delete_request req;
    struct moca_gen_status gs;
    struct moca_gen_node_status gns;

    memset(&req, 0, sizeof(req));

    /* get node bitmask */
    moca_get_gen_status(ctx, &gs);

    /* set wave0Nodemask to all 1.1 nodes */
    for(i = 0; i < MOCA_MAX_NODES; i++)
    {
        if(! (gs.connected_nodes & (1 << i)))
            continue;
        ret = moca_get_gen_node_status(ctx, i, (void *)&gns);
        if (ret == 0)
        {
            if ((gns.protocol_support >> 24) == MoCA_VERSION_11)
                req.wave0Nodemask |= (1 << i);
        }
    }

    moca_mac_to_u32(&req.flowid_hi, &req.flowid_lo, &params->flow_id[0]);

    ret = moca_set_pqos_delete_request(ctx, &req);
    if (ret != 0)
    {
        return (CMSRET_INTERNAL_ERROR);
    }

    return (CMSRET_SUCCESS) ;
}

CmsRet MoCACtl2_QueryPQoSFlow(
    void * ctx,
    PMoCA_QUERY_PQOS_PARAMS params)
{
    int ret, i;
    struct moca_pqos_query_request req;
    struct moca_gen_status gs;
    struct moca_gen_node_status gns;

    memset(&req, 0, sizeof(req));

    /* get node bitmask */
    moca_get_gen_status(ctx, &gs);

    /* set wave0Nodemask to all 1.1 nodes */
    for(i = 0; i < MOCA_MAX_NODES; i++)
    {
        if(! (gs.connected_nodes & (1 << i)))
            continue;
        ret = moca_get_gen_node_status(ctx, i, (void *)&gns);
        if (ret == 0)
        {
            if ((gns.protocol_support >> 24) == MoCA_VERSION_11)
                req.wave0Nodemask |= (1 << i);
        }
    }

    moca_mac_to_u32(&req.flowid_hi, &req.flowid_lo, &params->flow_id[0]);

    ret = moca_set_pqos_query_request(ctx, &req);
    if (ret != 0)
    {
        return (CMSRET_INTERNAL_ERROR);
    }

    return (CMSRET_SUCCESS) ;
}

CmsRet MoCACtl2_ListPQoSFlow(
    void * ctx,
    PMoCA_LIST_PQOS_PARAMS params)
{
    int ret;
    struct moca_gen_status gs;
    struct moca_gen_node_status gns;
    struct moca_init_time init;
    struct moca_pqos_list_request pqos;
    MAC_ADDRESS temp_mac;
    int i;

    memset(&pqos, 0x00, sizeof(pqos));
    pqos.flowStartIndex = 0;
    pqos.flowMaxReturn = 32;
    if (memcmp(params->ingress_mac, gNullMacAddr.macAddr,
        sizeof(MAC_ADDRESS)) == 0)
    {
        /* use the node id from params */
        ret = moca_get_gen_node_status(ctx, params->node_id, (void *)&gns);
        if (ret == 0)
        {
            if ((gns.protocol_support >> 24) == MoCA_VERSION_11)
                pqos.wave0Nodemask = 0x1 << params->node_id;
        }
    }
    else
    {/* need to find the node id for this mac address */

        /* get node bitmask */
        moca_get_gen_status(ctx, &gs);

        /* Check our self node first */
        moca_get_init_time(ctx, &init);
        moca_u32_to_mac(&temp_mac[0], init.mac_addr_hi, init.mac_addr_lo);

        if (!memcmp(&temp_mac, &params->ingress_mac, sizeof(MAC_ADDRESS)))
        {
            pqos.wave0Nodemask = 0x1 << gs.node_id;
        }
        else
        {
            /* find ingress and egress node id */
            for(i = 0; i < MOCA_MAX_NODES; i++)
            {
                if(! (gs.nodes_usable_bitmask & (1 << i)))
                    continue;
                ret = moca_get_gen_node_status(ctx, i, (struct moca_gen_node_status *)&gns);
                if (ret == 0)
                {
					moca_u32_to_mac(temp_mac, gns.eui_hi, gns.eui_lo);
                    if (!memcmp(temp_mac, params->ingress_mac,
                        sizeof(MAC_ADDRESS)))
                    {
                        if ((gns.protocol_support >> 24) == MoCA_VERSION_11)
                            pqos.wave0Nodemask = 0x1 << i;
                        break;
                    }
                }
            }
        }
    }

    if (pqos.wave0Nodemask != 0)
    {
        ret = moca_set_pqos_list_request(ctx, &pqos);
    }
    else
    {
        return (CMSRET_INVALID_ARGUMENTS);
    }

    if (ret != 0)
    {
        return (CMSRET_INTERNAL_ERROR);
    }

    return (CMSRET_SUCCESS);
}


CmsRet MoCACtl2_GetPQoSStatus( 
    void * ctx)
{
    int ret, i;
    struct moca_pqos_create_request req;
    struct moca_gen_status gs;
    struct moca_gen_node_status gns;
    
    memset(&req, 0, sizeof(req));

    /* get node bitmask */
    moca_get_gen_status(ctx, &gs);

    /* set wave0Nodemask to all 1.1 nodes */
    for(i = 0; i < MOCA_MAX_NODES; i++)
    {
        if(! (gs.connected_nodes & (1 << i)))
            continue;
        ret = moca_get_gen_node_status(ctx, i, (void *)&gns);
        if (ret == 0)
        {
            if ((gns.protocol_support >> 24) == MoCA_VERSION_11)
                req.wave0Nodemask |= (1 << i);
        }
    }

    srand(time(NULL));
    req.flowid_hi = rand() | 0x01000000;
    req.flowid_lo = rand() & 0xffff0000;
    req.tPacketSize = 1518;
    req.ingressNodeId = gs.node_id;
    req.egressNodeId = 0x3F;
    req.flowTag = 0;
    req.packetda_hi = req.flowid_hi;
    req.packetda_lo = req.flowid_lo;
    req.tPeakDataRate = 0xFFFFFF;
    req.tLeaseTime = 0;
    req.tBurstSize = 2;

    ret = moca_set_pqos_create_request(ctx, &req);
    if (ret != 0)
    {
        printf("%s: moca_set_pqos_create_request returned %d\n", __FUNCTION__, ret);
        return (CMSRET_INTERNAL_ERROR);
    }

    return (CMSRET_SUCCESS) ;
}


CmsRet MoCACtl2_ChSel(
    void * ctx,
    unsigned int freq)
{
    int ret;
    
    ret = moca_set_beacon_channel_set(ctx, freq);

    if (ret != 0)
    {
        printf("%s: moca_set_beacon_channel_set returned %d\n",
            __FUNCTION__, ret);
        return (CMSRET_INTERNAL_ERROR);
    }

    return (CMSRET_SUCCESS) ;
}

CmsRet MoCACtl2_Mr(
    void * ctx,
    PMoCA_MR_PARAMS params)
{
    int ret, i;
    struct moca_mr_request req;
    struct moca_gen_status gs;
    struct moca_gen_node_status gns;

    memset(&req, 0, sizeof(req));

    /* get node bitmask */
    moca_get_gen_status(ctx, &gs);

    /* set wave0Nodemask to all 1.1 nodes */
    for(i = 0; i < MOCA_MAX_NODES; i++)
    {
        if(! (gs.connected_nodes & (1 << i)))
            continue;
        ret = moca_get_gen_node_status(ctx, i, &gns);
        if ((ret == 0) && (i != gs.node_id))
        {
            if ((gns.protocol_support >> 24) == MoCA_VERSION_11)
                req.wave0Nodemask |= (1 << i);
        }
    }

    req.entryNodeId = gs.node_id;    

    req.resetStatus = params->reset_status;
    req.resetTimer = params->reset_timer;
    req.nonDefSeqNum = params->non_def_seq_num;

    ret = moca_set_mr_request(ctx, &req);

    if (ret != 0)
    {
        printf("%s: moca_set_mr_request returned %d\n",
            __FUNCTION__, ret);
        return (CMSRET_INTERNAL_ERROR);
    }

    return (CMSRET_SUCCESS) ;
}


CmsRet MoCACtl2_RestoreDefaults(void * ctx)
{
    int ret;

    ret = moca_set_restore_defaults(ctx);

    if (ret == 0)
       return CMSRET_SUCCESS;
    else
      return CMSRET_INTERNAL_ERROR;
}

char * MoCACtl2_WriteInitParms(char *cli, const void *buf)
{
    PMoCA_INITIALIZATION_PARMS parms;
    char maxTxPowerBeacons[16];    
    char tabooFixedMaskStart[16];
    char tabooFixedChannelMask[16];
    char tabooLeftMask[16];
    char tabooRightMask[16];    
    char padPower[16];
//    char version[16];
    char ledMode[16];
    char password[33];
    char *c = cli;

    parms = (PMoCA_INITIALIZATION_PARMS) buf;

    sprintf(maxTxPowerBeacons,"%d", parms->maxTxPowerBeacons);        
    sprintf(tabooFixedMaskStart, "%d", parms->tabooFixedMaskStart);
    sprintf(tabooFixedChannelMask,"0x%08X", parms->tabooFixedChannelMask);
    sprintf(tabooLeftMask,"0x%08X", parms->tabooLeftMask);
    sprintf(tabooRightMask,"0x%08X", parms->tabooRightMask);
    
    sprintf(padPower, "%d", parms->padPower);
//    sprintf(version, "%02x", parms->operatingVersion);    // Version doesn't do anything on 7xxx
    sprintf(ledMode, "%d", parms->ledMode);
    
    if (strlen((char *)parms->password) < MoCA_MIN_PASSWORD_LEN)
    {
        strcpy(password, MoCA_DEF_USER_PASSWORD);
    }
    else
    {
        strcpy(password, (char *)parms->password);
    }

    c = cli + sprintf(cli, "%s %s  %s %s  %s %s  %s %s  %s %s  "
                 "%s %s  %s %s  %s %s  %s %s  "
                 "%s %s  %s %s  %s %s  %s %s  %s %s  %s %s  "
                 "%s %s  %s %s  %s %s  %s %s  "
                 "%s %d  %s %d  %s %s  %s %d "
                 "%s %s  %s %s  %s %d  %s 0x%08x "
                 "%s %d  %s %d  %s %d  %s %d "
                 "%s %d  %s %d  %s %d  %s 0x%08x ",
        "--nc",               parms->ncMode==MoCA_ALWAYS_NC?"on":
                              parms->ncMode==MoCA_NEVER_NC?"off":"auto",
        "--singleCh",         parms->autoNetworkSearchEn==0?"on":parms->autoNetworkSearchEn==1?"off":"on2",
        "--privacy",          parms->privacyEn?"on":"off",
        "--tpc",              parms->txPwrControlEn?"on":"off",
        "--constTxMode",      parms->constTransmitMode==MoCA_NORMAL_OPERATION?"normal":
                              parms->constTransmitMode==MoCA_CONTINUOUS_TX_PROBE_I?"tx":
                              parms->constTransmitMode==MoCA_CONTINUOUS_RX?"rx":"external",
        "--maxTxPower",        maxTxPowerBeacons,            
        "--password",         password,
        "--mcastMode",        parms->mcastMode==MoCA_MCAST_NORMAL_MODE?"normal":"bcast",
        "--labMode",          parms->labMode?"on":"off",
        "--tabooFixedMaskStart",   tabooFixedMaskStart,
        "--tabooFixedChannelMask",    tabooFixedChannelMask,
        "--tabooLeftMask",    tabooLeftMask,
        "--tabooRightMask",   tabooRightMask,
        "--padPower",         padPower,
//        "--version",          version,
        "--preferredNC",      parms->preferedNC==MoCA_PREFERED_NC_MODE?"on":"off",
        "--ledMode",          ledMode,
        "--lpbk",             parms->mocaLoopbackEn?"on":"off",
        "--backoffMode",      parms->boMode==MoCA_BO_MODE_FAST?"fast":"slow",
        "--rfType",           parms->rfType==MoCA_RF_TYPE_D_BAND?"hi":
                              parms->rfType==MoCA_RF_TYPE_F_BAND?"midhi":
                              parms->rfType==MoCA_RF_TYPE_E_BAND?"midlo":"bandC4",
        "--mrNonDefSeqNum",   parms->mrNonDefSeqNum,
        "--beaconChannel",    parms->beaconChannel,
        "--egrMcFilter",      parms->egrMcFilterEn==MoCA_EGR_MC_FILTER_DISABLED?"off":"on",
        "--lowPriQNum",       parms->lowPriQNum,
        "--nodeType",         parms->terminalIntermediateType==MoCA_NODE_TYPE_TERMINAL?
                                "terminal":"intermediate",
        "--qam256Capability", parms->qam256Capability==MoCA_QAM256_CAPABILITY_ENABLED?"on":"off",
        "--continuousRxModeAttn", parms->continuousRxModeAttn,
        "--freqMask",         parms->freqMask,
        "--otfEn",            parms->otfEn,
        "--turboEn",          parms->turboEn,        
        "--wait",             parms->initOptions.dontStartMoca,
        "--beaconPwrReduction", parms->beaconPwrReduction,
        "--beaconPwrReductionEn", parms->beaconPwrReductionEn,
        "--flowControlEn",    parms->flowControlEn,
        "--mtmEn",            parms->mtmEn,
        "--pnsFreqMask",      parms->pnsFreqMask        
        );
    return(c);
}

char * MoCACtl2_WriteCfgParms(char *cli, const void *buf)
{
    PMoCA_CONFIG_PARAMS cparms;
    int i;
    char *c = cli;
    char *constellations[] = {"qam1024", "bpsk", "qpsk", "qam8", "qam16", "qam32",
                              "qam64", "qam128", "qam256", "qam512", "qam1024"};

    cparms = (PMoCA_CONFIG_PARAMS) buf;

    c += sprintf(c,          "%s %d  %s %d  %s %d  %s %3.1f "
                             "%s %s  %s %s  %s %d  %s %d  %s %d  "
                             "%s %d %d %d %d %d %d "
                             "%s %d  %s %d  %s %d  %s %d "
                             "%s %s  %s %s  %s %d  %s %d "
                             "%s %d  %s %d  %s %d  %s %d "
                             "%s %d  %s %d  %s %d  %s %d ",
        "--frameSize",       cparms->maxFrameSize,
        "--maxTxTime",       cparms->maxTransmitTime,
        "--minBwThreshold",  cparms->minBwAlarmThreshold,
        "--snrMgn",          (float)cparms->snrMargin/2.0,
        
        "--ieRRInsert",      cparms->continuousIERRInsert?"on":"off",
        "--ieMapInsert",     cparms->continuousIEMapInsert?"on":"off",
        "--maxAggr",         cparms->maxPktAggr,
        "--pmk",             cparms->pmkExchangeInterval,
        "--tek",             cparms->tekExchangeInterval,
        
        "--prio",            cparms->prioAllocation.resvHigh,
                             cparms->prioAllocation.resvMed,
                             cparms->prioAllocation.resvLow,
                             cparms->prioAllocation.limitHigh, 
                             cparms->prioAllocation.limitMed,
                             cparms->prioAllocation.limitLow,
                             
        "--phyRate128",      cparms->targetPhyRateQAM128,
        "--phyRate256",      cparms->targetPhyRateQAM256,        
        "--minMapCycle",     cparms->minMapCycle,
        "--maxMapCycle",     cparms->maxMapCycle,
        
        "--powersave",       cparms->pssEn?"on":"off",
        "--freqShiftMode",   cparms->freqShiftMode==MoCA_FREQ_SHIFT_MODE_MINUS?"minus":
                             cparms->freqShiftMode==MoCA_FREQ_SHIFT_MODE_PLUS?"plus":"off",
        "--rxPowerTuning",   cparms->rxPowerTuning,
        "--enCapable",       cparms->enCapable,
        "--rxTxPacketsPerQM",cparms->rxTxPacketsPerQM,
        "--extraRxPacketsPerQM", cparms->extraRxPacketsPerQM,
        "--diplexer",        cparms->diplexer,
        "--enMaxRateInMaxBo", cparms->enMaxRateInMaxBo,
        "--nbasCappingEn",    cparms->nbasCappingEn,        
        "--selectiveRR",     cparms->selectiveRR,
        "--phyRateTurbo",    cparms->targetPhyRateTurbo,
        "--phyRateTurboPlus", cparms->targetPhyRateTurboPlus );

    c += sprintf(c, " --sapm " );

    if (cparms->sapmEn)
    {
        c += sprintf(c, " on %d ", cparms->arplTh);
        for (i=0;i<MoCA_MAX_SAPM_TBL_INDEX * 2;i++)
        {
           if (i < MoCA_MAX_SAPM_TBL_INDEX)
               c += sprintf (c, " %d %3.1f ", i+4, cparms->sapmTable.sapmTableLo[i]/2.0);
           else
               c += sprintf (c, " %d %3.1f ", i-MoCA_MAX_SAPM_TBL_INDEX+141, cparms->sapmTable.sapmTableHi[i-MoCA_MAX_SAPM_TBL_INDEX]/2.0);
        }
    }
    else
    {
        c += sprintf(c, " off ");            
    }

    c += sprintf(c, " --rlapm ");

    if (cparms->rlapmEn)
    {
        c += sprintf(c, " on ");
        for (i=0;i<MoCA_MAX_RLAPM_TBL_INDEX;i++)
        {
           c += sprintf (c, " %d %3.1f ", i, cparms->rlapmTable[i]/2.0);
        }
    }
    else
    {
        c += sprintf(c, " off ");            
    }

    c += sprintf(c, " --snrMgnOffset ");

    for (i=0; i<(MoCA_MAX_SNR_TBL_INDEX-1);i++)
        c += sprintf(c, " %3.1f ", cparms->snrMarginOffset[i]/2.0);

    c += sprintf(c, " --maxConstellationInfo ");

    for (i = 0; i < MoCA_MAX_NODES; i++)
    {
        c += sprintf(c, "%s ", (cparms->constellation[i] < MAX_CONSTELLATION) ?
                                constellations[cparms->constellation[i]] :
                                constellations[MoCA_DEF_CONSTELLATION_INFO]);
    }

    return(c);
}

char * MoCACtl2_WriteTraceParms(char *cli, MoCA_TRACE_PARAMS *buf)
{    
    unsigned int lvl;
    
    char *c = cli;

    lvl = buf->traceLevel;
    if (!lvl)
        c += sprintf(c, " --none"); 

    if (lvl&MOCA_TRC_LEVEL_CORE)
        c += sprintf(c, " --core");
    if (lvl&MOCA_TRC_LEVEL_FN_ENTRY)
        c += sprintf(c, " --entry");
    if (lvl&MOCA_TRC_LEVEL_FN_EXIT)
        c += sprintf(c, " --exit");
    if (lvl&MOCA_TRC_LEVEL_DBG)
        c += sprintf(c, " --dbg");
    if (lvl&MOCA_TRC_LEVEL_VERBOSE)
        c += sprintf(c, " --verbose");
    if (lvl&MOCA_TRC_LEVEL_INFO)
        c += sprintf(c, " --info");
    if (lvl&MOCA_TRC_LEVEL_TRAP)
        c += sprintf(c, " --trap");
    if (lvl&MOCA_TRC_TIMESTAMPS)
        c += sprintf(c, " --time");
    if (lvl&MOCA_DUMP_HOST_CORE)
        c += sprintf(c, " --mmpdump");

    return(c);
}

SINT32 MoCACtl2_GetPersistent(void * ctx, const char *key, void *buf, UINT32 bufLen)
{     
    if (strcmp(key, "MoCAINITPARMS") == 0) {
        if (sizeof(MoCA_INITIALIZATION_PARMS) > bufLen)
        {
            fprintf(stderr, "Warning: nvram type too small for init\n");
            return(0);
        }
        if (CMSRET_SUCCESS != MoCACtl2_GetInitParms(ctx, buf))
            return(0);
        
        return(sizeof(MoCA_INITIALIZATION_PARMS));
    }
    else if (strcmp(key, "MoCACFGPARMS") == 0) {
        MoCA_CONFIG_PARAMS *m = (MoCA_CONFIG_PARAMS *)buf;
        if (sizeof(MoCA_CONFIG_PARAMS) > bufLen)
        {
            fprintf(stderr, "Warning: nvram type too small for cfg\n");
            return(0);
        }

        m->RegMem.input = 0;
        
        if (CMSRET_SUCCESS != MoCACtl2_GetCfg(ctx, buf, MoCA_CFG_PARAM_ALL_MASK))
        {
            return(0);
        } 

        return(sizeof(MoCA_CONFIG_PARAMS));
    }
    else if (strcmp(key, "MoCATRCLEVEL") == 0) {        
        MoCA_TRACE_PARAMS trace;
        
        if (sizeof(unsigned int) > bufLen)
        {
            fprintf(stderr, "Warning: nvram type too small for trace\n");
            return(0);
        }
        
        if (CMSRET_SUCCESS != MoCACtl2_GetTraceConfig(ctx, &trace))
            return(0);

        *(unsigned int *)buf = trace.traceLevel;
        
        return(sizeof(unsigned int));         
    }

    return(0);
}


#if !defined(DSL_MOCA) /* 7xxx Code */

CmsRet MoCACtl2_SetPersistent(void * ctx, const char *key, const void *buf, UINT32 bufLen)
{
    static char cli[4096];
    char *c = cli;
    MoCA_CONFIG_PARAMS cparms;    
    MoCA_INITIALIZATION_PARMS parms;
    MoCA_TRACE_PARAMS trace;


    if (strcmp(key, "MoCAINITPARMS") == 0) 
    {
        MoCACtl2_GetPersistent(ctx, "MoCACFGPARMS", &cparms, sizeof(cparms));
        MoCACtl2_GetPersistent(ctx, "MoCATRCLEVEL", &trace, sizeof(trace));

        c = cli + sprintf(c, "mocacfg set ");   
        if (buf)
            c = MoCACtl2_WriteInitParms(c, buf);

        c += sprintf(c, " -c ");
        c = MoCACtl2_WriteCfgParms(c, &cparms);
        c += sprintf(c, " -t ");
        c = MoCACtl2_WriteTraceParms(c, &trace);
    }
    else if (strcmp(key, "MoCACFGPARMS") == 0)
    {
        MoCACtl2_GetPersistent(ctx, "MoCAINITPARMS", &parms, sizeof(parms));
        MoCACtl2_GetPersistent(ctx, "MoCATRCLEVEL", &trace, sizeof(trace));

        c = cli + sprintf(c, "mocacfg set ");
        c = MoCACtl2_WriteInitParms(c, &parms);
        c += sprintf(c, " -c ");
        if (buf)
            c = MoCACtl2_WriteCfgParms(c, buf);
        c += sprintf(c, " -t ");
        c = MoCACtl2_WriteTraceParms(c, &trace);            
    }
    else if (strcmp(key, "MoCATRCLEVEL") == 0)
    {

        MoCACtl2_GetPersistent(ctx, "MoCAINITPARMS", &parms, sizeof(parms));
        MoCACtl2_GetPersistent(ctx, "MoCACFGPARMS", &cparms, sizeof(cparms));

        c = cli + sprintf(c, "mocacfg set ");
        c = MoCACtl2_WriteInitParms(c, &parms);
        c += sprintf(c, " -c ");
        c = MoCACtl2_WriteCfgParms(c, &cparms);
        c += sprintf(c, " -t ");
        if (buf)
            c = MoCACtl2_WriteTraceParms(c, (PMoCA_TRACE_PARAMS) buf);
    }
    
    c += sprintf(c, "\n");

    system(cli);
    return(CMSRET_SUCCESS);                
}


#else
 
CmsRet MoCACtl2_SetPersistent(void * ctx, const char *key, const void *buf, UINT32 bufLen)
{
   int ret;
   uint32_t msg_id;   

   if ((strcmp(key, "MoCAINITPARMS") == 0) ||
       (strcmp(key, "MoCALOF") == 0))
   {
      msg_id = MOCA_UPDATE_FLASH_INIT;
   }
   else if (strcmp(key, "MoCACFGPARMS") == 0)
   {
      msg_id = MOCA_UPDATE_FLASH_CONFIG;
   }
   else if (strcmp(key, "MoCATRCLEVEL") == 0)
   {
      msg_id = MOCA_UPDATE_FLASH_TRACE;
   }

   ret = moca_set_message(ctx, msg_id);

   if (ret == 0)
      return(CMSRET_SUCCESS);
   else
      return(CMSRET_INTERNAL_ERROR);
}

#endif

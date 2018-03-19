/*
* <:copyright-BRCM:2015:proprietary:gpon
* 
*    Copyright (c) 2015 Broadcom 
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
 

/*****************************************************************************/
/*                                                                           */
/* Include files                                                             */
/*                                                                           */
/*****************************************************************************/

#ifdef SIM_ENV
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#endif
#include <bdmf_dev.h>
#include "opticaldet.h"
#include "ngpon_db.h"

NGPON_SOFTWARE_STATE       g_software_state = NGPON_SOFTWARE_STATE_UNINITIALIZED ;
ONU_NGPON_DB               g_xgpon_db ;
XGPON_RXPON_PM_COUNTERS    g_rx_counters ;
XGPON_TXPON_PM_COUNTERS    g_tx_counters ;

NGPON_PLOAM_MSG_PM_COUNTERS     g_pon_ploam_pm_counters;
NGPON_TUNING_REQ_PM_COUNTERS    g_pon_tuning_req_pm_counters;
NGPON_PLODS_PM_COUNTERS         g_pon_lods_pm_counters;


/*
 *  init_globals() originally tuned for simulation setup
 */

void init_globals ( void )
{
    uint32_t i ;
    uint32_t flow_index ;
    uint32_t tcont_index ;

    uint32_t preamble_tmp_val  [2] ;
    uint32_t delimiter_tmp_val [2] ;


#ifdef BCM_ROGUE_ONU_DETECT
    TRX_SIG_PRESENCE tx_sd_supported;
    int ret ;
    int bus = -1;
 
    opticaldet_get_xpon_i2c_bus_num(&bus);
#endif

    /* Init OS resources */
    memset ( &g_xgpon_db.os_resources.pon_messages_queue_id, 0, sizeof (bdmf_queue_t) ) ;
    g_xgpon_db.os_resources.pon_task_id           = 0 ;
    g_xgpon_db.os_resources.pon_task_priority     = PON_TASK_PRIORITY ;

    g_xgpon_db.os_resources.stack_size             = CE_XGPON_STACK_SIZE ;
    memset ( &g_xgpon_db.os_resources.to1_timer_id, 0, sizeof (bdmf_timer_t) ) ;
    memset ( &g_xgpon_db.os_resources.to2_timer_id, 0, sizeof (bdmf_timer_t) ) ;
    memset ( &g_xgpon_db.os_resources.tk4_timer_id, 0, sizeof (bdmf_timer_t) ) ;
    g_xgpon_db.os_resources.tk4_key_index          = 0 ;
    memset ( &g_xgpon_db.os_resources.tk5_timer_id, 0, sizeof (bdmf_timer_t) ) ;
    g_xgpon_db.os_resources.tk5_key_index          = 0 ;
    memset ( &g_xgpon_db.os_resources.ds_sync_check_timer_id, 0, sizeof (bdmf_timer_t) ) ;
    memset ( &g_xgpon_db.os_resources.ber_interval_timer_id, 0, sizeof (bdmf_timer_t) ) ;
    memset ( &g_xgpon_db.os_resources.keep_alive_timer_id, 0, sizeof (bdmf_timer_t) ) ;
    memset ( &g_xgpon_db.os_resources.pee_timer_id, 0, sizeof (bdmf_timer_t) ) ;
    memset(&g_xgpon_db.os_resources.toz_timer_id, 0, sizeof(bdmf_timer_t));
    memset(&g_xgpon_db.os_resources.to3_timer_id, 0, sizeof(bdmf_timer_t));
    memset(&g_xgpon_db.os_resources.to4_timer_id, 0, sizeof(bdmf_timer_t));
    memset(&g_xgpon_db.os_resources.to5_timer_id, 0, sizeof(bdmf_timer_t));
#ifdef G989_3_AMD1
    memset(&g_xgpon_db.os_resources.tcpi_timer_id, 0, sizeof(bdmf_timer_t));
    g_xgpon_db.os_resources.tcpi_timer_is_running = BDMF_FALSE;
#endif
    g_xgpon_db.os_resources.os_frequency           = 0 ;

    /* RX */
    g_xgpon_db.rx_params.rx_descrambler_disable = BDMF_FALSE ;
    g_xgpon_db.rx_params.rx_loopback_enable     = BDMF_FALSE ;    
    g_xgpon_db.rx_params.rx_tm_enable           = BDMF_TRUE ;
    /* DS Polarity has been changed from True to False */
    g_xgpon_db.rx_params.rx_din_polarity        = BDMF_FALSE ;
    g_xgpon_db.rx_params.rx_mac_mode            = NGPON_MODE_XGPON;
    g_xgpon_db.rx_params.rx_sync_frame_len      = 0x97e0 ;

    g_xgpon_db.rx_params.rx_8khz_phase_difference                 = 0 ;
    g_xgpon_db.rx_params.rx_8khz_increm_phase_change              = 0 ;
    g_xgpon_db.rx_params.rx_8khz_clk_output_active_sync_required  = 0 ;

    g_xgpon_db.rx_params.rx_congestion_into_threshold     = NGPON_RX_CONG_ASSERT_THR ;
    g_xgpon_db.rx_params.rx_congestion_out_of_threshold   = 0 ;
    g_xgpon_db.rx_params.tcont_rx_counter_0  = NGPON_RX_DEFAULT_TCONT_TO_GROUP_0 ;
    g_xgpon_db.rx_params.tcont_rx_counter_1  = NGPON_RX_DEFAULT_TCONT_TO_GROUP_1 ;
    g_xgpon_db.rx_params.tcont_rx_counter_2  = NGPON_RX_DEFAULT_TCONT_TO_GROUP_2 ;
    g_xgpon_db.rx_params.tcont_rx_counter_3  = NGPON_RX_DEFAULT_TCONT_TO_GROUP_3 ;

    /* physic_params */
    g_xgpon_db.physic_params.num_of_psyncs_for_lof_assertion = 2 ;
    g_xgpon_db.physic_params.num_of_psyncs_for_lof_clear     = 3 ;  
    g_xgpon_db.physic_params.ponid_acq_thr                   = 2 ;                
    g_xgpon_db.physic_params.ext_sync_holdover_enable        = 0 ;   
    g_xgpon_db.physic_params.min_response_time               = NGPON_MIN_RESPONSE_TIME ;   
    g_xgpon_db.physic_params.exp_psync_ms                    = NGPON_EXP_PSYNC_MS ;
    g_xgpon_db.physic_params.exp_psync_ls                    = NGPON_EXP_PSYNC_LS ;
    g_xgpon_db.physic_params.psync_tol_in_sync               = NGPON_PSYNC_TOL_IN_SYNC ;
    g_xgpon_db.physic_params.psync_tol_other                 = NGPON_PSYNC_TOL_OTHER ;
    
        /* DV Setup/Hold Pattern */
    g_xgpon_db.physic_params.transceiver_dv_polarity          = 0 ;
    g_xgpon_db.physic_params.transceiver_dv_setup_length      = NGPON_DEFAULT_DV_SETUP_LEN ;
    g_xgpon_db.physic_params.transceiver_dv_hold_length       = NGPON_DEFAULT_DV_HOLD_LEN ;
    g_xgpon_db.physic_params.transceiver_dv_setup_pattern_src = 0 ;
    g_xgpon_db.physic_params.transceiver_dv_hold_pattern_src  = 0 ;
    g_xgpon_db.physic_params.transceiver_dv_setup_pattern     = NGPON_DEFAULT_DV_SETUP_PATTERN ;
    g_xgpon_db.physic_params.transceiver_dv_hold_pattern      = NGPON_DEFAULT_DV_HOLD_PATTERN ;

        /* Data Setup/Hold Pattern */
    g_xgpon_db.physic_params.transceiver_data_patten_type   = 1 ;
    g_xgpon_db.physic_params.transceiver_data_pattern_reg   = NGPON_DEFAULT_DATA_PATTERN_REG ;
    g_xgpon_db.physic_params.transceiver_data_setup_length  = NGPON_MAX_DAT_SETUP_PATTERN_LEN ;
    g_xgpon_db.physic_params.transceiver_data_hold_length   = NGPON_MAX_DAT_HOLD_PATTERN_LEN ;
    for ( i=0 ; i< g_xgpon_db.physic_params.transceiver_data_setup_length ; i++ ) 
    {
        g_xgpon_db.physic_params.transceiver_data_setup_pattern [ i ] = NGPON_DEFAULT_DATA_SETUP_PATTERN ;
    }
    for ( i=0 ; i< g_xgpon_db.physic_params.transceiver_data_hold_length ; i++ ) 
    {
        g_xgpon_db.physic_params.transceiver_data_hold_pattern [ i ] = NGPON_DEFAULT_DATA_HOLD_PATTERN ;
    }

    /* TX */
    g_xgpon_db.tx_params.tx_min_data_xgem_payload_len = NGPON_TX_DEFAULT_XGEM_PYLD_MIN_LEN_VALUE ;
    g_xgpon_db.tx_params.tx_min_idle_xgem_payload_len = NGPON_TX_DEFAULT_IDLE_XGEM_PYLD_LEN_VALUE ;
    g_xgpon_db.tx_params.tx_xgem_header_options = NGPON_TX_DEFAULT_XGEM_HEADER_OPTIONS ;     

    g_xgpon_db.tx_params.tx_timeline_offset    = NGPON_TX_DEFAULT_TIMELINE_OFFSET_XGPON ;
    g_xgpon_db.tx_params.tcont_tx_counter_0    = NGPON_TX_DEFAULT_TCONT_TO_GROUP_0 ;
    g_xgpon_db.tx_params.tcont_tx_counter_1    = NGPON_TX_DEFAULT_TCONT_TO_GROUP_1 ;
    g_xgpon_db.tx_params.tcont_tx_counter_2    = NGPON_TX_DEFAULT_TCONT_TO_GROUP_2 ;
    g_xgpon_db.tx_params.tcont_tx_counter_3    = NGPON_TX_DEFAULT_TCONT_TO_GROUP_3 ;

    g_xgpon_db.tx_params.tx_line_rate          = NGPON_US_LINE_RATE_2_5G ;

    g_xgpon_db.tx_params.tx_xgem_hdr_options   = NGPON_DFLT_XGEM_HDR_OPTIONS ;
     /* US Polarity (Positive in this case means 0)*/
    g_xgpon_db.tx_params.tx_data_out_polarity  = 0 ;



    for ( i = 0 ; i < NGPON_TX_QUEUES_PER_GROUP ; i++ )
    {
        g_xgpon_db.tx_params.tx_tcont_0_7_pd_size [ i ]  = NGPON_TX_TCONT_PD_SIZE  ;
        g_xgpon_db.tx_params.tx_tcont_0_7_queue_size [ i ]  = NGPON_TX_TCONT_FRONT_END_SIZE_TCONT_0_7  ;
    }
  
    g_xgpon_db.tx_params.tx_tcont_8_15_queue_size  = NGPON_TX_TCONT_FRONT_END_SIZE_TCONT_8_15 ;
    g_xgpon_db.tx_params.tx_tcont_16_23_queue_size = NGPON_TX_TCONT_FRONT_END_SIZE_TCONT_16_23;
    g_xgpon_db.tx_params.tx_tcont_24_31_queue_size = NGPON_TX_TCONT_FRONT_END_SIZE_TCONT_24_31;
    g_xgpon_db.tx_params.tx_tcont_32_39_queue_size = NGPON_TX_TCONT_FRONT_END_SIZE_TCONT_32_39;

    g_xgpon_db.tx_params.tx_enc_enable = BDMF_TRUE ;
    g_xgpon_db.tx_params.tx_bip_enable = BDMF_TRUE ;
    g_xgpon_db.tx_params.tx_scrambler_enable = BDMF_TRUE ;

    /* ONU Params */
    g_xgpon_db.onu_params.stack_mode = NGPON_MODE_XGPON;
    g_xgpon_db.onu_params.to1_timer_timeout = CE_XGPON_DEFAULT_TO1_TIMEOUT ;
    g_xgpon_db.onu_params.to2_timer_timeout = CE_XGPON_DEFAULT_TO2_TIMEOUT ;
    g_xgpon_db.onu_params.tk4_timer_timeout = CE_XGPON_DEFAULT_TK4_TIMEOUT ;
    g_xgpon_db.onu_params.tk5_timer_timeout = CE_XGPON_DEFAULT_TK5_TIMEOUT ;
    g_xgpon_db.onu_params.ds_sync_check_timer_timeout = CE_XGPON_DEFAULT_DS_SYNC_CHECK_TIMEOUT ; 
    g_xgpon_db.onu_params.to3_timer_timeout = DEFAULT_TO3_TIMEOUT;
    g_xgpon_db.onu_params.to4_timer_timeout = DEFAULT_TO4_TIMEOUT;
    g_xgpon_db.onu_params.to5_timer_timeout = DEFAULT_TO5_TIMEOUT;
    g_xgpon_db.onu_params.toz_timer_timeout = DEFAULT_TOZ_TIMEOUT;
#ifdef G989_3_AMD1
    g_xgpon_db.onu_params.tcpi_tout = DEFAULT_TCPI_TIMEOUT ;
#endif
    g_xgpon_db.onu_params.ploam_default_key[0] = NGPON_DEFAULT_PLOAM_KEY_WORD_1;
    g_xgpon_db.onu_params.ploam_default_key[1] = NGPON_DEFAULT_PLOAM_KEY_WORD_2;
    g_xgpon_db.onu_params.ploam_default_key[2] = NGPON_DEFAULT_PLOAM_KEY_WORD_3;
    g_xgpon_db.onu_params.ploam_default_key[3] = NGPON_DEFAULT_PLOAM_KEY_WORD_4;

    g_xgpon_db.onu_params.cmac_k2_default_key[0] = NGPON_DEFAULT_CMAC_K2_KEY_WORD_1;
    g_xgpon_db.onu_params.cmac_k2_default_key[1] = NGPON_DEFAULT_CMAC_K2_KEY_WORD_2;
    g_xgpon_db.onu_params.cmac_k2_default_key[2] = NGPON_DEFAULT_CMAC_K2_KEY_WORD_3;
    g_xgpon_db.onu_params.cmac_k2_default_key[3] = NGPON_DEFAULT_CMAC_K2_KEY_WORD_4;

    g_xgpon_db.onu_params.sn.serial_number[0] = CE_XGPON_DEFAULT_SERIAL_NUMBER_VC_BYTE0 ;
    g_xgpon_db.onu_params.sn.serial_number[1] = CE_XGPON_DEFAULT_SERIAL_NUMBER_VC_BYTE1 ;
    g_xgpon_db.onu_params.sn.serial_number[2] = CE_XGPON_DEFAULT_SERIAL_NUMBER_VC_BYTE2 ;
    g_xgpon_db.onu_params.sn.serial_number[3] = CE_XGPON_DEFAULT_SERIAL_NUMBER_VC_BYTE3 ;
    g_xgpon_db.onu_params.sn.serial_number[4] = CE_XGPON_DEFAULT_SERIAL_NUMBER_SN_BYTE0 ;
    g_xgpon_db.onu_params.sn.serial_number[5] = CE_XGPON_DEFAULT_SERIAL_NUMBER_SN_BYTE1 ;
    g_xgpon_db.onu_params.sn.serial_number[6] = CE_XGPON_DEFAULT_SERIAL_NUMBER_SN_BYTE2 ;
    g_xgpon_db.onu_params.sn.serial_number[7] = CE_XGPON_DEFAULT_SERIAL_NUMBER_SN_BYTE3 ;

    for ( flow_index = 0 ; flow_index < NGPON_MAX_NUM_OF_XGEM_FLOW_IDS ; flow_index++ )
    {
        g_xgpon_db.ds_xgem_flow_table [ flow_index ].enable_flag    = BDMF_FALSE ;
        g_xgpon_db.ds_xgem_flow_table [ flow_index ].encryption_ring   = NGPON_ENCRYPTION_RING_NONE ;
        g_xgpon_db.ds_xgem_flow_table [ flow_index ].flow_id           = NGPON_XGEM_FLOW_ID_UNASSIGNED ;
        g_xgpon_db.ds_xgem_flow_table [ flow_index ].flow_priority     = PON_FLOW_PRIORITY_UNASSIGNED ;
        g_xgpon_db.ds_xgem_flow_table [ flow_index ].xgem_port_id      = NGPON_XGEM_PORT_ID_UNASSIGNED ;
        g_xgpon_db.ds_xgem_flow_table [ flow_index ].xgem_port_mask_id = NGPON_XGEM_PORT_ID_UNASSIGNED ;
        g_xgpon_db.ds_xgem_flow_table [ flow_index ].crc_enable        = BDMF_FALSE ;
    }


    for ( tcont_index = 0 ; tcont_index < NGPON_MAX_NUM_OF_TCONT_IDS ; tcont_index++ )
    {
        g_xgpon_db.tconts_table [ tcont_index ].alloc_id = tcont_index ;
        g_xgpon_db.tconts_table [ tcont_index ].tcont_id = tcont_index ;
        g_xgpon_db.tconts_table [ tcont_index ].assign_alloc_valid_flag = BDMF_TRUE ;
        g_xgpon_db.tconts_table [ tcont_index ].config_tcont_valid_flag = BDMF_TRUE ;
    }

    /* Debug */
    g_xgpon_db.debug_params.check_ds_mic = BDMF_TRUE;
    g_xgpon_db.debug_params.swap_delimiter_msb_lsb = BDMF_FALSE;
    g_xgpon_db.debug_params.swap_preamble_msb_lsb = BDMF_FALSE;
    g_xgpon_db.debug_params.use_predefined_key = BDMF_FALSE;
    /* Predefined Key : 112233445566778899AABBCCDDEEFF00*/
    for (i = 0 ; i < NGPON_DEFAULT_KEY_SIZE-1 ; i++)
    {
        g_xgpon_db.debug_params.predefined_key [ i ] = 0x11*(i+1);
    }
    g_xgpon_db.debug_params.predefined_key[NGPON_DEFAULT_KEY_SIZE-1] = 0x0;
    g_xgpon_db.debug_params.disable_onu_sm = BDMF_FALSE;

    g_xgpon_db.link_params.burst_profiles.profiles[0].profile_index = 0;
    g_xgpon_db.link_params.burst_profiles.profiles[0].profile_version = 1;
    g_xgpon_db.link_params.burst_profiles.profiles[0].fec_ind = BDMF_FALSE;
    g_xgpon_db.link_params.burst_profiles.profiles[0].preamble_len = 0x8;
    preamble_tmp_val[0] = 0xAAAAAAAA;
    preamble_tmp_val[1] = 0xAAAAAAAA;
    memcpy(g_xgpon_db.link_params.burst_profiles.profiles[0].preamble, 
        preamble_tmp_val, PREAMBLE_MAX_LENGTH);
    g_xgpon_db.link_params.burst_profiles.profiles[0].preamble_repeat_counter = 0x3;
    g_xgpon_db.link_params.burst_profiles.profiles[0].delimiter_len = 0x4;
    delimiter_tmp_val[0] = 0x11223344;
    delimiter_tmp_val[1] = 0x55667788;
    memcpy(g_xgpon_db.link_params.burst_profiles.profiles[0].delimiter, 
        delimiter_tmp_val, DELIMITER_MAX_LENGTH);
    g_xgpon_db.link_params.burst_profiles.profiles[0].pon_tag[PON_TAG_SIZE-1] = 0x1;
    g_xgpon_db.link_params.burst_profiles.profiles[0].profile_enable = BDMF_TRUE;
    g_xgpon_db.link_params.burst_profiles.profiles[1].profile_index = 1;
    g_xgpon_db.link_params.burst_profiles.profiles[1].profile_version = 1;
    g_xgpon_db.link_params.burst_profiles.profiles[1].fec_ind = BDMF_FALSE;
    g_xgpon_db.link_params.burst_profiles.profiles[1].preamble_len = 0x6;
    preamble_tmp_val[0] = 0xAAAAAAAA;
    preamble_tmp_val[1] = 0xAAAAAAAA;
    memcpy(g_xgpon_db.link_params.burst_profiles.profiles[0].preamble, 
        preamble_tmp_val, PREAMBLE_MAX_LENGTH);
    g_xgpon_db.link_params.burst_profiles.profiles[1].preamble_repeat_counter = 0x1D;
    g_xgpon_db.link_params.burst_profiles.profiles[1].delimiter_len = 0x4;
    delimiter_tmp_val[0] = 0x11223344;
    delimiter_tmp_val[1] = 0x55667788;
    memcpy(g_xgpon_db.link_params.burst_profiles.profiles[0].delimiter, 
        delimiter_tmp_val, DELIMITER_MAX_LENGTH);
    g_xgpon_db.link_params.burst_profiles.profiles[1].pon_tag[PON_TAG_SIZE-1] = 0x1;
    g_xgpon_db.link_params.burst_profiles.profiles[1].profile_enable = BDMF_TRUE;
    g_xgpon_db.link_params.burst_profiles.profiles[2].profile_index = 2;
    g_xgpon_db.link_params.burst_profiles.profiles[2].profile_version = 1;
    g_xgpon_db.link_params.burst_profiles.profiles[2].fec_ind = BDMF_FALSE;
    g_xgpon_db.link_params.burst_profiles.profiles[2].preamble_len = 0x8;
    preamble_tmp_val[0] = 0xAAAAAAAA;
    preamble_tmp_val[1] = 0xAAAAAAAA;
    memcpy(g_xgpon_db.link_params.burst_profiles.profiles[0].preamble, 
        preamble_tmp_val, PREAMBLE_MAX_LENGTH);
    g_xgpon_db.link_params.burst_profiles.profiles[2].preamble_repeat_counter = 0x3;
    g_xgpon_db.link_params.burst_profiles.profiles[2].delimiter_len = 0x4;
    delimiter_tmp_val[0] = 0x11223344;
    delimiter_tmp_val[1] = 0x55667788;
    memcpy(g_xgpon_db.link_params.burst_profiles.profiles[0].delimiter, 
        delimiter_tmp_val, DELIMITER_MAX_LENGTH);
    g_xgpon_db.link_params.burst_profiles.profiles[2].pon_tag[PON_TAG_SIZE-1]= 0x1 ;
    g_xgpon_db.link_params.burst_profiles.profiles[2].profile_enable = BDMF_TRUE;
    g_xgpon_db.link_params.burst_profiles.profiles[3].profile_index = 3;
    g_xgpon_db.link_params.burst_profiles.profiles[3].profile_version = 1;
    g_xgpon_db.link_params.burst_profiles.profiles[3].fec_ind = BDMF_FALSE;
    g_xgpon_db.link_params.burst_profiles.profiles[3].preamble_len = 0x6;
    preamble_tmp_val[0] = 0xAAAAAAAA;
    preamble_tmp_val[1] = 0xAAAAAAAA;
    memcpy(g_xgpon_db.link_params.burst_profiles.profiles[0].preamble, 
        preamble_tmp_val, PREAMBLE_MAX_LENGTH);
    g_xgpon_db.link_params.burst_profiles.profiles[3].preamble_repeat_counter = 0x19;
    g_xgpon_db.link_params.burst_profiles.profiles[3].delimiter_len = 0x4;
    delimiter_tmp_val[0] = 0x11223344;
    delimiter_tmp_val[1] = 0x55667788;
    memcpy(g_xgpon_db.link_params.burst_profiles.profiles[0].delimiter, 
        delimiter_tmp_val, DELIMITER_MAX_LENGTH);
    g_xgpon_db.link_params.burst_profiles.profiles[3].pon_tag[PON_TAG_SIZE-1] = 0x1;
    g_xgpon_db.link_params.burst_profiles.profiles[3].profile_enable = BDMF_FALSE;

    g_xgpon_db.link_params.WLCP = BDMF_FALSE;
    g_xgpon_db.link_params.init_tuning_done = BDMF_FALSE;
    g_xgpon_db.link_params.ticks_to_wait_before_chn_switch = 40;
    g_xgpon_db.link_params.lcdg_state.curr_state = BDMF_FALSE;
    

    g_xgpon_db.link_params.ch_profile.current_ds_channel = 0;
    g_xgpon_db.link_params.ch_profile.current_us_channel = 0;
    g_xgpon_db.link_params.ch_profile.target_ds_channel = 0;
    g_xgpon_db.link_params.ch_profile.target_us_channel = 0;
    g_xgpon_db.link_params.ch_profile.rollback_available = 0;
    g_xgpon_db.link_params.ch_profile.current_cpi = 0;
    g_xgpon_db.link_params.ch_profile.tuning_resp_seqno = 0;
    g_xgpon_db.link_params.system_profile.twdm_channel_count = 4;

    memset(&g_xgpon_db.rx_ier_vector, 0, sizeof(ngpon_rx_int_ier));
    g_xgpon_db.rx_ier_vector.lof_state_change_mask = BDMF_TRUE;
    g_xgpon_db.rx_ier_vector.lcdg_state_change_mask = BDMF_TRUE;
#ifdef CONFIG_BCM_GPON_TODD   
    g_xgpon_db.rx_ier_vector.tod_update_mask = BDMF_TRUE;
#endif 
         
    memset(&g_xgpon_db.tx_ier_vector, 0, sizeof (ngpon_tx_int_ier1));
    g_xgpon_db.tx_ier_vector.tx_plm_0 = BDMF_TRUE;   /* For SW PLOAM buffer mngmnt */
#ifdef BCM_ROGUE_ONU_DETECT
    ret = trx_get_tx_sd_supported(bus, &tx_sd_supported) ;    
    if ((ret == 0) && (tx_sd_supported == TRX_SIGNAL_SUPPORTED))
    {
        g_xgpon_db.tx_ier_vector.rog_dif = BDMF_TRUE;
        g_xgpon_db.tx_ier_vector.rog_len = BDMF_TRUE;
    }
    else
    {
        g_xgpon_db.tx_ier_vector.rog_dif = BDMF_FALSE;
        g_xgpon_db.tx_ier_vector.rog_len = BDMF_FALSE;
    }
#else
    g_xgpon_db.tx_ier_vector.rog_dif = BDMF_FALSE;
    g_xgpon_db.tx_ier_vector.rog_len = BDMF_FALSE;
#endif
}



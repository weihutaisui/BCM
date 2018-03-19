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
#ifndef DB_H_INCLUDED
#define DB_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/
/*                                                                            */
/* General types definitions.                                                 */
/*                                                                            */
/******************************************************************************/

#include "ngpon_general.h"
#include "ngpon_ploam.h"
#include "ngpon_sm.h"
#include "ngpon_cfg.h"
#include "ngpon_isr.h"
#include "ngpon_ind_task.h"
#include "pon_sm_common.h"






/*****************************************************************************/
/* This type defines OS resources  identification                            */
/*****************************************************************************/

#define CE_XGPON_STACK_SIZE                            4096
#define CE_XGPON_UPSTREAM_PLOAM_RATE                   ( 2000 )
#define CE_XGPON_DEFAULT_REI_SEQUENCE_NUMBER           ( 0 )

#define CE_XGPON_DEFAULT_SERIAL_NUMBER_VC_BYTE0        ( 0x00 )
#define CE_XGPON_DEFAULT_SERIAL_NUMBER_VC_BYTE1        ( 0x01 )
#define CE_XGPON_DEFAULT_SERIAL_NUMBER_VC_BYTE2        ( 0x02 )
#define CE_XGPON_DEFAULT_SERIAL_NUMBER_VC_BYTE3        ( 0x03 )
#define CE_XGPON_DEFAULT_SERIAL_NUMBER_SN_BYTE0        ( 0x03 )
#define CE_XGPON_DEFAULT_SERIAL_NUMBER_SN_BYTE1        ( 0x02 )
#define CE_XGPON_DEFAULT_SERIAL_NUMBER_SN_BYTE2        ( 0x01 )
#define CE_XGPON_DEFAULT_SERIAL_NUMBER_SN_BYTE3        ( 0x00 )
#define CE_XGPON_DEFAULT_PASSWORD_BYTE0                ( 0x00 )
#define CE_XGPON_DEFAULT_PASSWORD_BYTE1                ( 0x00 )
#define CE_XGPON_DEFAULT_PASSWORD_BYTE2                ( 0x00 )
#define CE_XGPON_DEFAULT_PASSWORD_BYTE3                ( 0x00 )
#define CE_XGPON_DEFAULT_PASSWORD_BYTE4                ( 0x00 )
#define CE_XGPON_DEFAULT_PASSWORD_BYTE5                ( 0x00 )
#define CE_XGPON_DEFAULT_PASSWORD_BYTE6                ( 0x00 )
#define CE_XGPON_DEFAULT_PASSWORD_BYTE7                ( 0x00 )
#define CE_XGPON_DEFAULT_PASSWORD_BYTE8                ( 0x00 )
#define CE_XGPON_DEFAULT_PASSWORD_BYTE9                ( 0x00 )

#define CE_XGPON_DEFAULT_BER_INTERVAL                  ( 1000 )

#define CE_XGPON_DEFAULT_GUARD_BITS_NUMBER             ( 0 )
#define CE_XGPON_DEFAULT_DELIMITER_BYTE0               ( 0xAA )
#define CE_XGPON_DEFAULT_DELIMITER_BYTE1               ( 0x85 )
#define CE_XGPON_DEFAULT_DELIMITER_BYTE2               ( 0xB3 )
#define CE_XGPON_DEFAULT_TYPE1_PREAMBLE_BITS           ( 0 )
#define CE_XGPON_DEFAULT_TYPE2_PREAMBLE_BITS           ( 0 )
#define CE_XGPON_DEFAULT_TYPE3_PREAMBLE_BITS           ( 0xFFFFFFFF )
#define CE_XGPON_DEFAULT_PREASSIGNED_EQ_DELAY          ( 0 )
#define CE_XGPON_DEFAULT_POWER_LEVEL_MODE              ( 0 )
#define CE_XGPON_DEFAULT_SERIAL_NUMBER_MASK_STATUS     ( CE_STT_FALSE )
#define CE_XGPON_DEFAULT_NUMBER_OF_PSYNCS_FOR_LOF_ASSERTION   ( 0 )
#define CE_XGPON_DEFAULT_NUMBER_OF_PSYNCS_FOR_LOF_CLEAR       ( 0 )
#define CE_XGPON_DEFAULT_EXTENDED_BURST_FLAG           ( CE_STT_FALSE )
#define CE_XGPON_DEFAULT_PRE_RANGED_TYPE3_NUMBER       ( 0 )
#define CE_XGPON_DEFAULT_RANGED_TYPE3_NUMBER           ( 0 )
#define CE_XGPON_DEFAULT_EXTRA_SN_TRANSMISSIONS        ( 0 )
#define CE_XGPON_DEFAULT_PREEQUALIZATION_STATUS        ( CE_STT_FALSE )
#define CE_XGPON_DEFAULT_AES_SWITCHING_TIME            ( 0x3FFFFFFF )
#define CE_XGPON_DEFAULT_TX_THRESHOLD                  ( 0 )
#define CE_XGPON_DEFAULT_TX_TDM_THRESHOLD_MODE         ( CE_STT_TRUE )
#define CE_XGPON_DEFAULT_TX_TDM_THRESHOLD_MIN          ( 0 )
#define CE_XGPON_DEFAULT_TX_TDM_THRESHOLD_MAX          ( 0x7FF )
#define CE_XGPON_DEFAULT_TO1_TIMEOUT                   ( 10000 ) 
#define CE_XGPON_DEFAULT_TO2_TIMEOUT                   ( 100 )
#define CE_XGPON_DEFAULT_TK4_TIMEOUT                   ( 1000000 ) 
#define CE_XGPON_DEFAULT_TK5_TIMEOUT                   ( 2000000 )
#define CE_XGPON_DEFAULT_DS_SYNC_CHECK_TIMEOUT         ( 10 )
#define CE_XGPON_DEFAULT_BER_INTERVAL                  ( 1000 )
#define CE_XGPON_DEFAULT_SD_THRESHOLD                  ( 4 )
#define CE_XGPON_DEFAULT_SF_THRESHOLD                  ( 3 )
#define CE_XGPON_DEFAULT_LOF_ASSERTION                 ( 4 )
#define CE_XGPON_DEFAULT_LOF_CLEAR                     ( 1 )
#define CE_XGPON_DEFAULT_MIN_RESPONSE_TIME             ( 35 )
#define CE_XGPON_DEFAULT_DV_HOLD_PATTERN               ( 0x00FFF0 )
#define CE_XGPON_DEFAULT_DV_POLARITY                   ( 1 )
#define CE_XGPON_DEFAULT_DV_SETUP_PATTERN              ( 0x000FFF )
#define CE_XGPON_DEFAULT_POWER_MODE                    ( CE_BL_DISABLE )
#define CE_XGPON_DEFAULT_POWER_CALIBRATION_PATTERN     ( 0xBB44AA33 )
#define CE_XGPON_DEFAULT_POWER_CALIBRATION_SIZE        ( 120 )
#define CE_XGPON_DEFAULT_DBR_VALID_PART1               ( 0xFFFFFFFF )
#define CE_XGPON_DEFAULT_DBR_VALID_PART2               ( 0xFF )
#define CE_XGPON_DEFAULT_DBR_STOP_INDEX                ( 0 )
#define CE_XGPON_DEFAULT_DATA_PATTERN_ENABLE           ( CE_BL_ENABLE )
#define CE_XGPON_DEFAULT_DATA_SETUP_LENGTH             ( 3 )
#define CE_XGPON_DEFAULT_DATA_HOLD_LENGTH              ( 3 )
#define CE_XGPON_DEFAULT_DATA_SETUP_PATTERN            ( 0 )
#define CE_XGPON_DEFAULT_DATA_HOLD_PATTERN             ( 0 )
#define CE_XGPON_DEFAULT_PON_TASK_PRIORITY             ( 150 )
#define CE_XGPON_SR_DBR_INVALID                        ( 0xFF )
#define CE_XGPON_DBR_MAX_REPORT                        ( 0xFE )
#define DEFAULT_TO3_TIMEOUT 10000 
#define DEFAULT_TO4_TIMEOUT 10000 
#define DEFAULT_TO5_TIMEOUT 10000 
#define DEFAULT_TOZ_TIMEOUT 10000 

#ifdef G989_3_AMD1
#define DEFAULT_TCPI_TIMEOUT (300 * 1000)   /* 5 minutes in ms */
#endif

/* Define the various queue length */
#define CPU_QUEUE_LENGTH                    ( 512 )
/* Define the various queue length */
#define NGPON_QUEUE_LENGTH                    ( 512 )
/* Define the various tasks priority */
#define CPU_TASK_PRIORITY                   ( 80 )
/* Define the various tasks priority */
#define PON_TASK_PRIORITY                   ( 75 )



/******************************************************************************/
/* This type defines the TCONT table structs                                  */
/******************************************************************************/

/* T-CONT entry for the table of TCONTs */
typedef struct
{
    
    /* Alloc value */
    PON_ALLOC_ID_DTE alloc_id ;
    
    /* T-CONT Queue Id */
    PON_TCONT_ID_DTE tcont_id ;
        
    /* Tcont queue size */
    uint32_t tcont_queue_size ;
    
    /* Assign flag - determine if assign alloc id message arrived */
    bdmf_boolean assign_alloc_valid_flag ;
      
    /* Configure tcont flag - determine if the user call configure tcont API to this tcont*/
    bdmf_boolean config_tcont_valid_flag ;
    
 }
NGPON_TCONT_TABLE_ENTRY ;

/* OS resources */
typedef struct
{
    /* Stack size */
    uint32_t stack_size ;

    /* OS frequency */
    uint32_t os_frequency ;
     
    /* Pon task ID*/
     bdmf_task  pon_task_id ;

    /* Pon task priority */
    uint32_t pon_task_priority ;
    
    /* PON messages queue */
    bdmf_queue_t pon_messages_queue_id ;

    /* TO1 timer */
    bdmf_timer_t to1_timer_id ;

    /* TO2 timer */
    bdmf_timer_t to2_timer_id ;

    /* TK4 timer */
    bdmf_timer_t tk4_timer_id ;
    unsigned long  tk4_key_index ;

    /* TK5 timer */
    bdmf_timer_t tk5_timer_id ;
    unsigned long  tk5_key_index ;


    /* ds_sync_check timer */
    bdmf_timer_t ds_sync_check_timer_id ;

    /* BER interval timer */
    bdmf_timer_t ber_interval_timer_id ;

#if defined ( MAC_CLEANUP )

    /* Critical BER timer */
    bdmf_timer_t critical_ber_timer_id ;

#endif /* MAC_CLEANUP */

    /* Keep alive timer */
    bdmf_timer_t keep_alive_timer_id ;

    /* PEE timer ID */
    bdmf_timer_t pee_timer_id ;

    bdmf_timer_t toz_timer_id;
    bdmf_timer_t to3_timer_id;
    bdmf_timer_t to4_timer_id;
    bdmf_timer_t to5_timer_id;
#ifdef G989_3_AMD1
    bdmf_timer_t tcpi_timer_id;
    bdmf_boolean tcpi_timer_is_running;
#endif
    
}
NGPON_RESOURCES_DTE ;


/* Physical paramters */
typedef struct
{
    /* Upstream ploam rate */
   uint32_t upstream_ploam_rate ; 

    /* Min response time */
   uint32_t min_response_time ; 

    /* EqD in bits */
   uint32_t eqd_in_bits ; 

   NGPON_DATA_PATTERN_TYPE transceiver_data_patten_type ;
   uint32_t  transceiver_data_pattern_reg ;
   uint8_t   transceiver_data_setup_length ;
   uint8_t   transceiver_data_hold_length ;
   uint32_t  transceiver_data_setup_pattern [ NGPON_MAX_DAT_SETUP_PATTERN_LEN ] ;
   uint32_t  transceiver_data_hold_pattern [ NGPON_MAX_DAT_HOLD_PATTERN_LEN ] ; 

   bdmf_boolean                  transceiver_dv_polarity ;                         
   uint32_t                  transceiver_dv_setup_length;                      
   uint32_t                  transceiver_dv_hold_length;                       
   NGPON_DV_SETUP_PATTERN_SRC  transceiver_dv_setup_pattern_src;
   NGPON_DV_HOLD_PATTERN_SRC   transceiver_dv_hold_pattern_src ;
   uint32_t                  transceiver_dv_setup_pattern ;
   uint32_t                  transceiver_dv_hold_pattern ; 

     /* Number of PSYNCs for LOF assertion */
   uint32_t num_of_psyncs_for_lof_assertion ;

    /* Number of PSYNCs for LOF clear */
   uint32_t                  num_of_psyncs_for_lof_clear ;

   uint8_t                   ponid_acq_thr ;                   
   bdmf_boolean              ext_sync_holdover_enable ;   

  uint32_t                   exp_psync_ms ;       /* Expected Psync word most significant */
  uint32_t                   exp_psync_ls ;       /* Expected Psync word least significant */

  uint32_t                   ponid_mask_ms ;      /* Mask for PON-ID consistency detection mechanism */
  uint32_t                   ponid_mask_ls ; 

  uint8_t                    psync_tol_in_sync ;   /* Psync tolerance when synced */
  uint8_t                    psync_tol_other ;

  uint32_t                   tx_word_delay ;
  uint8_t                    tx_frame_delay ; 
  uint8_t                    tx_bit_delay ;
 
}
NGPON_PHYSICAL_PARAMETERS_DTE ;

typedef struct
{
    /* TX - Tx pls enable */                             
    bdmf_boolean  tx_pls_enable ;                          
                                                         
    /* Tx data polarity*/
    bdmf_boolean tx_data_out_polarity ;

   uint32_t          tx_min_data_xgem_payload_len ;                                 
   uint32_t          tx_min_idle_xgem_payload_len ;                                 
   uint32_t          tx_xgem_header_options ;                                       
   uint32_t          tx_timeline_offset ;                                           
   uint32_t          tx_tcont_0_7_pd_size    [ NGPON_TX_QUEUES_PER_GROUP ]   ;      
   uint32_t          tx_tcont_0_7_queue_size [ NGPON_TX_QUEUES_PER_GROUP ]   ;      
   uint32_t          tx_tcont_8_15_queue_size  ;
   uint32_t          tx_tcont_16_23_queue_size ;
   uint32_t          tx_tcont_24_31_queue_size ;
   uint32_t          tx_tcont_32_39_queue_size ;

   uint32_t          tx_xgem_hdr_options;

   uint8_t           tx_line_rate ;

   uint32_t          tx_frame_length ;
   
   bdmf_boolean      tx_enc_enable ;
   bdmf_boolean      tx_bip_enable ;

   bdmf_boolean      tx_scrambler_enable ;

   PON_TCONT_ID_DTE   tcont_tx_counter_0 ;
   PON_TCONT_ID_DTE   tcont_tx_counter_1 ;
   PON_TCONT_ID_DTE   tcont_tx_counter_2 ;
   PON_TCONT_ID_DTE   tcont_tx_counter_3 ;

   uint8_t            lbe_invert_bit ;
}
NGPON_TX_PARAMETERS ;

typedef struct
{

    /* RX - Rx enable */ 
    bdmf_boolean  rx_enable ; 
   
    /* RX - Rx loopback enable */ 
    bdmf_boolean  rx_loopback_enable ; 

    /* RX - Rx fec force enable */ 
    bdmf_boolean  rx_force_fec_state ; 

    /* RX - Rx fec type (strong/weak) */
    uint8_t       rx_fec_type ;
    
    /* RX Descrambler disable */
    bdmf_boolean  rx_descrambler_disable ; 

    /* RX FEC force enable/disable */
    bdmf_boolean  rx_fec_force ;

    /* RX Data polarity */
    bdmf_boolean rx_din_polarity ;

        /* TX CRC Override */
    bdmf_boolean rx_crc_override ;

    /* TX Idle Filltering Enable */
    bdmf_boolean rx_idle_ploam_filltering_enable ;

    /* TX Forward all ploam messages */
    bdmf_boolean rx_forward_all_ploam_messages ;

    /* RX 8 KHZ clock */
    uint16_t          rx_8khz_phase_difference ;               
    bdmf_boolean      rx_8khz_increm_phase_change ;            
    bdmf_boolean      rx_8khz_clk_output_active_sync_required;
    
    /* Congestion threshold  */
    uint8_t rx_congestion_into_threshold ;
    uint8_t rx_congestion_out_of_threshold ;
    
     /* RX - TM enable */ 
    bdmf_boolean  rx_tm_enable ;

    /* RX - XGPON frame length */  
    uint16_t      rx_sync_frame_len ;

    /* RX - XGPON/NGPON MAC mode */
    uint8_t       rx_mac_mode ;

    /* RX - Disable forcing of a single PLOAM per ONU/broadcast per frame */
    bdmf_boolean  rx_enforce_single_ploam ;


    PON_TCONT_ID_DTE   tcont_rx_counter_0 ;
    PON_TCONT_ID_DTE   tcont_rx_counter_1 ;
    PON_TCONT_ID_DTE   tcont_rx_counter_2 ;
    PON_TCONT_ID_DTE   tcont_rx_counter_3 ;
}
NGPON_RX_PARAMETERS ;





/* Debug Params */
typedef struct
{    
     bdmf_boolean   check_ds_mic ; 
     bdmf_boolean   swap_delimiter_msb_lsb ;
     bdmf_boolean   swap_preamble_msb_lsb ;
     bdmf_boolean   use_predefined_key ;
     uint8_t        predefined_key [NGPON_DEFAULT_KEY_SIZE];
     bdmf_boolean   disable_onu_sm;
}
NGPON_DEBUG_PARAMETERS  ;



typedef struct
{
    /* ONU ID */
    NGPON_ONU_ID            onu_id ;

    /* Serial number */
    NGPON_SERIAL_NUMBER     sn ; 

    /* registration_id */
    NGPON_REGISTRATION_ID   reg_id ;

    /* PLOAM_IK Default */
    uint32_t   ploam_default_key [NGPON_NUM_OF_WORDS_AT_ENCRYPTION_KEY];
    /* CMAC K2 Default */
    uint32_t cmac_k2_default_key [NGPON_NUM_OF_WORDS_AT_ENCRYPTION_KEY];

    uint8_t ploam_ik_in_bytes [NGPON_DEFAULT_KEY_SIZE];
    uint8_t sk_in_bytes[NGPON_DEFAULT_KEY_SIZE];
    uint8_t omci_ik_in_bytes[NGPON_DEFAULT_KEY_SIZE];
    uint8_t kek_in_bytes[NGPON_DEFAULT_KEY_SIZE];
    uint8_t ploam_cmac_k2_in_bytes[NGPON_DEFAULT_KEY_SIZE];
    uint8_t omci_k1 [NGPON_DEFAULT_KEY_SIZE];    /* Generated keys for OMCI packets will be stored here */
    uint8_t omci_k2 [NGPON_DEFAULT_KEY_SIZE];
    

    /* Ber Interval */
    uint32_t ber_interval ;

    /* TO1 - Timer timeout */
    uint32_t to1_timer_timeout ;

    /* TO2 - Timer timeout */
    uint32_t to2_timer_timeout ;

    /* TK4 - Timer timeout */
    uint32_t tk4_timer_timeout ;

    /* TK5 - Timer timeout */
    uint32_t tk5_timer_timeout ;

    /* ds_sync_check - Timer timeout */
    uint32_t ds_sync_check_timer_timeout ;

     /* BER threshold for SF assertion */
    uint32_t ber_threshold_for_sf_assertion ; 

    /* BER threshold for SD assertion */
    uint32_t ber_threshold_for_sd_assertion ; 

    uint32_t protect_ds_pon_id;
    uint32_t protect_us_pon_id;

    uint32_t to3_timer_timeout;
    uint32_t to4_timer_timeout;
    uint32_t to5_timer_timeout;
    uint32_t toz_timer_timeout;
#ifdef G989_3_AMD1
    uint32_t tcpi_tout;
#endif

    /* stack_mode: XGPON/NGPON2_10G/NGPON2_2_5G/XGS */
    PON_MAC_MODE stack_mode;
}
NGPON_ONU_PARAMETERS ;

typedef struct
{    
    uint8_t ng2sys_id[3];
    uint8_t version;
    uint8_t us_operation_wavelength_bands_ptp_wdm;
    uint8_t us_operation_wavelength_bands_twdm;
    uint8_t twdm_channel_count;
    uint8_t channel_spacing_twdm;
    uint8_t us_mse_twdm;
    uint8_t fsr_twdm[2];
    uint8_t amcc_use_flag;
    uint8_t min_calibration_accuracy_for_activation_for_in_band;
    uint8_t loose_calibration_bound_twdm;
    uint8_t ptp_wdm_channel_count[2];
    uint8_t channel_spacing_ptp_wdm;
    uint8_t us_mse_ptp_wdm;
    uint8_t fsr_ptp_wdm[2];
    uint8_t ptp_wdm_calibration_accuracy;
    uint8_t loose_calibration_bound_ptp_wdm;

    bdmf_boolean system_profile_received;
}
SYSTEM_PROFILE;

typedef struct
{
    DS_CHANNEL_PROFILE_TWDM_PLOAM channel[16];
    bdmf_boolean channel_profile_received;
    uint8_t   channel_profile_num;
    uint8_t   current_ds_channel ;
    uint8_t   current_us_channel ;
    uint8_t   current_cpi ;
    uint8_t   target_ds_channel ;
    uint8_t   target_us_channel ;
    uint8_t   rollback_available ;
    uint8_t   tuning_resp_seqno ;
}
CHANNEL_PROFILES;

typedef struct
{
    XGPON_BURST_PROFILE_INFO profiles[NGPON_BURST_PROFILE_INDEX_HIGH + 1];
    bdmf_boolean burst_profile_received;
}
BURST_PROFILES;

typedef struct
{ 
    uint8_t vendor_id[4];
    uint8_t vssn[4];
    uint8_t current_pon_id[4];
    uint8_t correlation_tag[2];
    uint8_t target_ds_pon_id[4];
    uint8_t target_us_pon_id[4];
}
CALIBRATION;

typedef struct 
{
    bdmf_boolean curr_state; 
    uint64_t     timestamp;    
}  
NGPON_LCDG_STATE ; 

/* Link */
typedef struct
{
    /* Activation state machine control */
    NGPON_OPERATION_SM operation_sm ;
    
    /* Burst profiles */
    BURST_PROFILES burst_profiles;

    /* key exchange state machine*/
    NGPON_AES_SM key_sm;

    int sub_sm_state;

    /* System profile */
    SYSTEM_PROFILE system_profile;

    /* Channel profile */
    CHANNEL_PROFILES ch_profile;

    CALIBRATION calibration_record;

    bdmf_boolean WLCP;

    bdmf_boolean init_tuning_done;             /* For NGPON2: DS sync attained at the channel */

    int  ticks_to_wait_before_chn_switch;      /* 
                                                * For NGPON2: how many ticks of ds_sync_check_timer_timeout length 
                                                * wait in NO_SYNC state before attempting to switch the wavelength channel 
                                                */
    NGPON_LCDG_STATE    lcdg_state;
}
NGPON_LINK_PARAMETERS_DTE ;


typedef struct
{
    /* Physical parameters */
    NGPON_PHYSICAL_PARAMETERS_DTE physic_params ;

    /* Tx param */
    NGPON_TX_PARAMETERS tx_params ;

    /* Rx param */
    NGPON_RX_PARAMETERS rx_params ;

    /* ONU param */
    NGPON_ONU_PARAMETERS onu_params ;

    /* link param */
    NGPON_LINK_PARAMETERS_DTE link_params ;

    /* OS resources*/
    NGPON_RESOURCES_DTE  os_resources ;

    /* SW Tcont table */
    NGPON_TCONT_TABLE_ENTRY  tconts_table [ NGPON_MAX_NUM_OF_TCONT_IDS ] ;

    NGPON_DEBUG_PARAMETERS  debug_params ;

    /* SW Port table */
    NGPON_DS_XGEM_INDICATION_PARAMS  ds_xgem_flow_table [ NGPON_MAX_NUM_OF_XGEM_FLOW_IDS ] ;

    /* Rogue ONU Detection parameters */
    ROGUE_ONU_DETECTION_PARAMETERS rogue_onu_detection_parameters;

    ngpon_rx_int_ier rx_ier_vector;

    ngpon_tx_int_ier1 tx_ier_vector;

    PON_COUNTERS_DTE rxpon_counters;

    uint16_t optics_type;

}
ONU_NGPON_DB ;

extern NGPON_SOFTWARE_STATE             g_software_state;
extern ONU_NGPON_DB                     g_xgpon_db;
extern XGPON_RXPON_PM_COUNTERS          g_rx_counters;
extern XGPON_TXPON_PM_COUNTERS          g_tx_counters;

extern NGPON_PLOAM_MSG_PM_COUNTERS      g_pon_ploam_pm_counters;
extern NGPON_TUNING_REQ_PM_COUNTERS     g_pon_tuning_req_pm_counters;
extern NGPON_PLODS_PM_COUNTERS          g_pon_lods_pm_counters;

void init_globals ( void );

#ifdef __cplusplus
}
#endif

#endif

